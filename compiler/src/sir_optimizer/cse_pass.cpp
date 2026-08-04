/**
 * @file cse_pass.cpp
 * @brief Common Subexpression Elimination Pass Implementation
 * @brief تنفيذ تمرير حذف التعابير الجزئية المشتركة
 *
 * @author SadLanguage Compiler Team
 * @date January 2026 - Fixed API
 */

#include "sir_optimizer/cse_pass.h"
#include <algorithm>
#include <sstream>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            // ============================================================================
            // ExpressionKey Implementation
            // ============================================================================

            size_t ExpressionKey::hash() const
            {
                size_t h = std::hash<int>{}(static_cast<int>(opcode));

                for (const auto &operand : operands)
                {
                    h ^= std::hash<std::string>{}(operand) + 0x9e3779b9 + (h << 6) + (h >> 2);
                }

                return h;
            }

            // ============================================================================
            // CSEPass Implementation
            // ============================================================================

            CSEPass::CSEPass()
                : OptimizationPass("CSE", PassType::TRANSFORMATION), replacementCount_(0)
            {
            }

            CSEPass::~CSEPass() = default;

            bool CSEPass::runOnFunction(SIR::SIRFunction *function)
            {
                if (!function)
                {
                    return false;
                }

                bool modified = false;
                replacementCount_ = 0;

                // Iterate over all basic blocks
                auto &blocks = function->getBasicBlocks();

                for (size_t blockIdx = 0; blockIdx < blocks.size(); ++blockIdx)
                {
                    auto &block = blocks[blockIdx];
                    if (!block)
                        continue;

                    // Clear expression table for each block - LOCAL CSE only.
                    // Cross-block CSE requires dominance analysis which we don't have,
                    // and causes "Instruction does not dominate all uses!" LLVM errors.
                    clearTables();

                    // instructions is vector<SIRInstruction> (not unique_ptr)
                    auto &instructions = block->instructions;

                    // Track indices of instructions to remove
                    std::vector<size_t> indicesToRemove;

                    for (size_t instIdx = 0; instIdx < instructions.size(); ++instIdx)
                    {
                        auto &inst = instructions[instIdx];

                        // Check if already marked for removal
                        bool alreadyRemoved = std::find(indicesToRemove.begin(),
                                                        indicesToRemove.end(),
                                                        instIdx) != indicesToRemove.end();
                        if (alreadyRemoved)
                        {
                            continue;
                        }

                        // (AR) إبطال جدول التعابير عند STORE لمنع CSE خاطئ:
                        // حين تكتب تعليمة STORE في متغير (محلي أو عام)، يجب إبطال
                        // جميع التعابير المخزّنة التي تعتمد على ذلك المتغير الهدف.
                        // مثال: بعد STORE %r, %var، تكون %var قد تغيّرت
                        // فأي تعبير مثل ADD %var, 1 خُزّن سابقاً لم يعد صالحاً.
                        // (EN) Invalidate expression table on STORE to prevent incorrect CSE:
                        // When a STORE instruction writes to a variable, all cached expressions
                        // that depend on that variable must be invalidated.
                        if (inst.opcode == SIR::SIROpcode::STORE && inst.operands.size() >= 2)
                        {
                            const auto &dest = inst.operands[1];
                            if (dest.type == SIR::SIROperandType::REGISTER ||
                                dest.type == SIR::SIROperandType::GLOBAL)
                            {
                                const std::string &destName = dest.name;
                                // احذف جميع التعابير التي تعتمد على المتغير الهدف للكتابة
                                for (auto it = expressionTable_.begin(); it != expressionTable_.end();)
                                {
                                    bool dependsOnDest = false;
                                    for (const auto &opName : it->first.operands)
                                    {
                                        if (opName == destName)
                                        {
                                            dependsOnDest = true;
                                            break;
                                        }
                                    }
                                    if (dependsOnDest)
                                    {
                                        it = expressionTable_.erase(it);
                                    }
                                    else
                                    {
                                        ++it;
                                    }
                                }
                            }
                        }

                        // Check if instruction is eliminable
                        if (!isEliminable(&inst))
                        {
                            continue;
                        }

                        // Analyze expression
                        auto exprKey = analyzeExpression(&inst);
                        if (!exprKey)
                        {
                            continue;
                        }

                        // Look for similar expression
                        auto it = expressionTable_.find(*exprKey);

                        if (it != expressionTable_.end())
                        {
                            // Found duplicate expression!
                            const auto &savedExpr = it->second;

                            // Get result register name from optional<SIROperand>
                            if (inst.result.has_value())
                            {
                                std::string currentResult = inst.result->name;
                                if (!currentResult.empty())
                                {
                                    replaceRegisterUses(
                                        function,
                                        currentResult,
                                        savedExpr.resultRegister,
                                        blockIdx);

                                    // Mark instruction index for removal
                                    indicesToRemove.push_back(instIdx);
                                    modified = true;
                                    replacementCount_++;

                                    recordModification();
                                }
                            }
                        }
                        else
                        {
                            // Save expression for future use
                            if (inst.result.has_value())
                            {
                                std::string result = inst.result->name;
                                if (!result.empty())
                                {
                                    expressionTable_[*exprKey] = ExpressionInfo(
                                        result,
                                        &inst,
                                        blockIdx);
                                }
                            }
                        }
                    }

                    // Remove marked instructions (in reverse order to maintain indices)
                    std::sort(indicesToRemove.rbegin(), indicesToRemove.rend());
                    for (size_t idx : indicesToRemove)
                    {
                        instructions.erase(instructions.begin() + idx);
                    }
                }

                if (modified)
                {
                    debug("CSE: Eliminated " + std::to_string(replacementCount_) +
                          " redundant expressions");
                }

                return modified;
            }

            std::unique_ptr<ExpressionKey> CSEPass::analyzeExpression(
                const SIR::SIRInstruction *inst)
            {
                if (!inst)
                {
                    return nullptr;
                }

                SIR::SIROpcode opcode = inst->opcode;

                // Only arithmetic and logical operations are eliminable
                switch (opcode)
                {
                // Arithmetic operations
                case SIR::SIROpcode::ADD_I64:
                case SIR::SIROpcode::ADD_F64:
                case SIR::SIROpcode::SUB_I64:
                case SIR::SIROpcode::SUB_F64:
                case SIR::SIROpcode::MUL_I64:
                case SIR::SIROpcode::MUL_F64:
                case SIR::SIROpcode::DIV_I64:
                case SIR::SIROpcode::DIV_F64:
                case SIR::SIROpcode::FLOOR_DIV_I64:
                case SIR::SIROpcode::MOD_I64:

                // Bitwise operations
                case SIR::SIROpcode::AND:
                case SIR::SIROpcode::OR:
                case SIR::SIROpcode::XOR:
                case SIR::SIROpcode::SHL:
                case SIR::SIROpcode::SHR:
                case SIR::SIROpcode::SAR:

                // Comparisons
                case SIR::SIROpcode::EQ:
                case SIR::SIROpcode::NE:
                case SIR::SIROpcode::LT:
                case SIR::SIROpcode::LE:
                case SIR::SIROpcode::GT:
                case SIR::SIROpcode::GE:
                    break;

                default:
                    return nullptr;
                }

                // Collect operand names from inst->operands (vector<SIROperand>)
                // FIX: For constant operands, include the actual value in the key,
                // not just the name (which is empty for ConstantI64/F64/Bool).
                // Without this, all comparisons like GE %x, 3 and GE %x, 2
                // would be treated as identical expressions.
                std::vector<std::string> operandNames;
                for (const auto &operand : inst->operands)
                {
                    if (operand.type == SIR::SIROperandType::CONSTANT)
                    {
                        switch (operand.dataType)
                        {
                        // (AR) [إصلاح عيب كامن كشفته طبقة طبيعي64] كلّ الثوابت الصحيحة (بأيّ
                        //      عرض/إشارة) تُفتَّح بقيمتها intValue. كان UInt64/بايت/الأنواع
                        //      المحدَّدة تقع في default فتستعمل operand.name **الفارغ** للثوابت
                        //      ⇒ مفتاحٌ واحد `$c_` لكلّ ثوابت UInt64 ⇒ CSE يدمج `ك%2` و`ك%3`
                        //      (قيمتان مختلفتان، مفتاحٌ واحد). ضمُّها هنا يصلح الجذر.
                        // (EN) [Latent-defect fix surfaced by the طبيعي64 layer] All integer
                        //      constants (any width/signedness) key by their intValue. UInt64/Byte/
                        //      sized types fell into default and used the (empty for constants)
                        //      operand.name ⇒ a single `$c_` key for every UInt64 constant ⇒ CSE
                        //      merged `ك%2` and `ك%3` (distinct values, one key). Grouping them fixes the root.
                        case SIR::SadTypeKind::Integer:
                        case SIR::SadTypeKind::UInt64:
                        case SIR::SadTypeKind::Byte:
                        case SIR::SadTypeKind::Int8:
                        case SIR::SadTypeKind::Int16:
                        case SIR::SadTypeKind::Int32:
                        case SIR::SadTypeKind::Int64:
                        case SIR::SadTypeKind::UInt8:
                        case SIR::SadTypeKind::UInt16:
                        case SIR::SadTypeKind::UInt32:
                            operandNames.push_back("$ci64_" + std::to_string(operand.intValue));
                            break;
                        case SIR::SadTypeKind::Float:
                            operandNames.push_back("$cf64_" + std::to_string(operand.floatValue));
                            break;
                        case SIR::SadTypeKind::Boolean:
                            operandNames.push_back(std::string("$cbool_") + (operand.boolValue ? "1" : "0"));
                            break;
                        case SIR::SadTypeKind::String:
                            operandNames.push_back("$cstr_" + operand.name);
                            break;
                        default:
                            operandNames.push_back("$c_" + operand.name);
                            break;
                        }
                    }
                    else
                    {
                        operandNames.push_back(operand.name);
                    }
                }

                if (operandNames.empty())
                {
                    return nullptr;
                }

                return std::make_unique<ExpressionKey>(opcode, operandNames);
            }

            bool CSEPass::isEliminable(const SIR::SIRInstruction *inst)
            {
                if (!inst)
                {
                    return false;
                }

                // Check for result (inst->result is std::optional<SIROperand>)
                if (!inst->result.has_value())
                {
                    return false;
                }

                // Check for no side effects
                if (hasSideEffects(inst->opcode))
                {
                    return false;
                }

                return true;
            }

            bool CSEPass::hasSideEffects(SIR::SIROpcode opcode)
            {
                switch (opcode)
                {
                // Memory operations
                case SIR::SIROpcode::STORE:
                case SIR::SIROpcode::ALLOC:
                case SIR::SIROpcode::ALLOC_HEAP:
                case SIR::SIROpcode::FREE:
                case SIR::SIROpcode::MEMCPY:
                case SIR::SIROpcode::MEMSET:

                // Function calls
                case SIR::SIROpcode::CALL:
                case SIR::SIROpcode::CALL_INDIRECT:

                // Control flow
                case SIR::SIROpcode::BR:
                case SIR::SIROpcode::BR_COND:
                case SIR::SIROpcode::RET:
                case SIR::SIROpcode::RET_VOID:

                // Built-in functions
                case SIR::SIROpcode::BUILTIN_PRINT:
                case SIR::SIROpcode::BUILTIN_READ:

                // Unified UI (all have side effects — allocate or mutate widgets)
                case SIR::SIROpcode::BUILTIN_UI_COLUMN:
                case SIR::SIROpcode::BUILTIN_UI_ROW:
                case SIR::SIROpcode::BUILTIN_UI_STACK:
                case SIR::SIROpcode::BUILTIN_UI_CONTAINER:
                case SIR::SIROpcode::BUILTIN_UI_TEXT:
                case SIR::SIROpcode::BUILTIN_UI_TEXT_STYLED:
                case SIR::SIROpcode::BUILTIN_UI_BUTTON:
                case SIR::SIROpcode::BUILTIN_UI_BUTTON_VARIANT:
                case SIR::SIROpcode::BUILTIN_UI_ICON_BUTTON:
                case SIR::SIROpcode::BUILTIN_UI_FAB:
                case SIR::SIROpcode::BUILTIN_UI_TEXT_FIELD:
                case SIR::SIROpcode::BUILTIN_UI_CHECKBOX:
                case SIR::SIROpcode::BUILTIN_UI_SWITCH:
                case SIR::SIROpcode::BUILTIN_UI_SLIDER:
                case SIR::SIROpcode::BUILTIN_UI_CARD:
                case SIR::SIROpcode::BUILTIN_UI_SCAFFOLD:
                case SIR::SIROpcode::BUILTIN_UI_APP_BAR:
                case SIR::SIROpcode::BUILTIN_UI_SPACER:
                case SIR::SIROpcode::BUILTIN_UI_DIVIDER:
                case SIR::SIROpcode::BUILTIN_UI_DIALOG:
                // مصانع إضافيّة (م-مصانع) — تُخصّص عناصر، فلها أثر جانبيّ (لا CSE)
                case SIR::SIROpcode::BUILTIN_UI_IMAGE:
                case SIR::SIROpcode::BUILTIN_UI_ICON:
                case SIR::SIROpcode::BUILTIN_UI_TEXT_BUTTON:
                case SIR::SIROpcode::BUILTIN_UI_GRID:
                case SIR::SIROpcode::BUILTIN_UI_CENTER:
                case SIR::SIROpcode::BUILTIN_UI_PADDING:
                case SIR::SIROpcode::BUILTIN_UI_ALIGN:
                case SIR::SIROpcode::BUILTIN_UI_EXPANDED:
                case SIR::SIROpcode::BUILTIN_UI_FLEXIBLE:
                case SIR::SIROpcode::BUILTIN_UI_SIZED_BOX:
                case SIR::SIROpcode::BUILTIN_UI_WRAP:
                case SIR::SIROpcode::BUILTIN_UI_BOX:
                case SIR::SIROpcode::BUILTIN_UI_SCROLL_VIEW:
                case SIR::SIROpcode::BUILTIN_UI_BOTTOM_NAV:
                case SIR::SIROpcode::BUILTIN_UI_SNACKBAR:
                case SIR::SIROpcode::BUILTIN_UI_TOOLTIP:
                case SIR::SIROpcode::BUILTIN_UI_PROGRESS:
                case SIR::SIROpcode::BUILTIN_UI_LAZY_COLUMN:
                case SIR::SIROpcode::BUILTIN_UI_LAZY_ROW:
                case SIR::SIROpcode::BUILTIN_UI_LIST_VIEW:
                case SIR::SIROpcode::BUILTIN_UI_TEXT_AREA:
                case SIR::SIROpcode::BUILTIN_UI_DRAWER:
                case SIR::SIROpcode::BUILTIN_UI_SAFE_AREA:
                case SIR::SIROpcode::BUILTIN_UI_SURFACE:
                case SIR::SIROpcode::BUILTIN_UI_SPIN_BOX:
                case SIR::SIROpcode::BUILTIN_UI_GROUP_BOX:
                case SIR::SIROpcode::BUILTIN_UI_SPINNER:
                case SIR::SIROpcode::BUILTIN_UI_STATUS_BAR:
                case SIR::SIROpcode::BUILTIN_UI_WINDOW:
                case SIR::SIROpcode::BUILTIN_UI_TITLE_BAR:
                case SIR::SIROpcode::BUILTIN_UI_SCROLL_BAR:
                case SIR::SIROpcode::BUILTIN_UI_TASKBAR:
                case SIR::SIROpcode::BUILTIN_UI_START_MENU:
                case SIR::SIROpcode::BUILTIN_UI_SYSTEM_TRAY:
                case SIR::SIROpcode::BUILTIN_UI_MENU:
                case SIR::SIROpcode::BUILTIN_UI_TOOLBAR:
                case SIR::SIROpcode::BUILTIN_UI_SEGMENTED_CONTROL:
                case SIR::SIROpcode::BUILTIN_UI_TREE_VIEW:
                case SIR::SIROpcode::BUILTIN_UI_SPLIT_VIEW:
                case SIR::SIROpcode::BUILTIN_UI_DATA_TABLE:
                case SIR::SIROpcode::BUILTIN_UI_BREADCRUMB:
                case SIR::SIROpcode::BUILTIN_UI_PICKER:
                case SIR::SIROpcode::BUILTIN_UI_DATE_PICKER:
                case SIR::SIROpcode::BUILTIN_UI_STEPPER:
                case SIR::SIROpcode::BUILTIN_UI_RATING_BAR:
                case SIR::SIROpcode::BUILTIN_UI_ALERT:
                // أثر المعدّلات (م-أ3ر): ضبط خاصّيّة = أثر جانبيّ (لا CSE)
                case SIR::SIROpcode::BUILTIN_UI_SET_PROP_STR:
                case SIR::SIROpcode::BUILTIN_UI_SET_PROP_INT:
                case SIR::SIROpcode::BUILTIN_UI_SET_PROP_NUM:
                case SIR::SIROpcode::BUILTIN_UI_SET_PROP_BOOL:
                case SIR::SIROpcode::BUILTIN_UI_ADD_EVENT:
                case SIR::SIROpcode::BUILTIN_UI_SET_EVENT_PHASE:
                case SIR::SIROpcode::BUILTIN_UI_SET_EVENT_DATA:
                case SIR::SIROpcode::BUILTIN_UI_ANIM_BEGIN:
                case SIR::SIROpcode::BUILTIN_UI_ANIM_DURATION:
                case SIR::SIROpcode::BUILTIN_UI_ANIM_EASING:
                case SIR::SIROpcode::BUILTIN_UI_ANIM_DELAY:
                case SIR::SIROpcode::BUILTIN_UI_ANIM_REPEAT:
                case SIR::SIROpcode::BUILTIN_UI_ANIM_AUTO_REVERSE:
                case SIR::SIROpcode::BUILTIN_UI_PROP_JOIN_ADD:
                case SIR::SIROpcode::BUILTIN_UI_PROP_JOIN_COMMIT:
                case SIR::SIROpcode::BUILTIN_UI_ADD_CHILD:
                case SIR::SIROpcode::BUILTIN_UI_REMOVE_CHILD:
                case SIR::SIROpcode::BUILTIN_UI_CLEAR_CHILDREN:
                case SIR::SIROpcode::BUILTIN_UI_SET_TEXT:
                case SIR::SIROpcode::BUILTIN_UI_SET_SIZE:
                case SIR::SIROpcode::BUILTIN_UI_SET_FLEX:
                case SIR::SIROpcode::BUILTIN_UI_SET_BACKGROUND:
                case SIR::SIROpcode::BUILTIN_UI_SET_FOREGROUND:
                case SIR::SIROpcode::BUILTIN_UI_SET_SPACING:
                case SIR::SIROpcode::BUILTIN_UI_SET_PADDING:
                case SIR::SIROpcode::BUILTIN_UI_SET_ALIGNMENT:
                case SIR::SIROpcode::BUILTIN_UI_SET_BORDER:
                case SIR::SIROpcode::BUILTIN_UI_SET_ELEVATION:
                case SIR::SIROpcode::BUILTIN_UI_SET_OPACITY:
                case SIR::SIROpcode::BUILTIN_UI_SET_VISIBILITY:
                case SIR::SIROpcode::BUILTIN_UI_APP_CREATE:
                case SIR::SIROpcode::BUILTIN_UI_APP_SET_ROOT:
                case SIR::SIROpcode::BUILTIN_UI_APP_LAYOUT:
                case SIR::SIROpcode::BUILTIN_UI_APP_RENDER:
                case SIR::SIROpcode::BUILTIN_UI_APP_RUN:
                case SIR::SIROpcode::BUILTIN_UI_PRINT_TREE:
                case SIR::SIROpcode::BUILTIN_UI_APP_DESTROY:
                case SIR::SIROpcode::BUILTIN_UI_WIDGET_DESTROY:
                // (AR) دوال الثيم (ترتيب مطابق لتعداد sir_types.h): تبديل/داكن/فاتح
                //      تُغيّر حالة عامّة؛ هل_داكن يقرؤها (نتيجته تتغيّر بعد تبديل)
                //      ⇒ لا يجوز دمج/حذف أيّ منها.
                case SIR::SIROpcode::BUILTIN_UI_TOGGLE_THEME:
                case SIR::SIROpcode::BUILTIN_UI_DARK_MODE:
                case SIR::SIROpcode::BUILTIN_UI_LIGHT_MODE:
                case SIR::SIROpcode::BUILTIN_UI_IS_DARK:
                // (AR) التقط_مفتاح يستنزف طابور المفاتيح (حالة متغيّرة): كلّ نداءٍ يعيد
                //      مفتاحًا مختلفًا ⇒ لا يجوز دمج/حذف نداءاته.
                case SIR::SIROpcode::BUILTIN_UI_NEXT_KEY:
                // (AR) التنقّل: انتقل/عودة/عودة_للبداية/استبدل تُغيّر مكدّس التنقّل؛
                //      عدد_الصفحات يقرؤه (نتيجته تتغيّر بعد تنقّل) ⇒ لا دمج/حذف.
                case SIR::SIROpcode::BUILTIN_UI_NAVIGATE:
                case SIR::SIROpcode::BUILTIN_UI_NAV_BACK:
                case SIR::SIROpcode::BUILTIN_UI_NAV_ROOT:
                case SIR::SIROpcode::BUILTIN_UI_REPLACE_PAGE:
                case SIR::SIROpcode::BUILTIN_UI_PAGE_COUNT:
                // (AR) الصفحة_الحاليّة تقرأ الحاليّة (تتغيّر بعد تنقّل) ⇒ لا دمج/حذف.
                case SIR::SIROpcode::BUILTIN_UI_CURRENT_PAGE:
                case SIR::SIROpcode::BUILTIN_UI_NAVIGATE_TRANSITION:
                case SIR::SIROpcode::BUILTIN_UI_BACK_TRANSITION:
                // (إكمال corui) الانتقال الكامل يغيّر المكدّس؛ الحالة/النافذة/توليد الويب
                //   ذات آثار جانبيّة (إعادة رسم/عنوان/إغلاق/تخصيص نصّ + نداء دالّة تحديث)
                //   ⇒ لا يجوز دمج/حذف أيّ منها.
                case SIR::SIROpcode::BUILTIN_UI_NAVIGATE_EXIT_TRANSITION:
                case SIR::SIROpcode::BUILTIN_UI_STOP_PROPAGATION:
                case SIR::SIROpcode::BUILTIN_UI_UPDATE_STATE:
                case SIR::SIROpcode::BUILTIN_UI_SET_STATE:
                case SIR::SIROpcode::BUILTIN_UI_SET_TITLE:
                case SIR::SIROpcode::BUILTIN_UI_CLOSE_WINDOW:
                case SIR::SIROpcode::BUILTIN_UI_GEN_WEB:
                case SIR::SIROpcode::BUILTIN_UI_GEN_ANDROID:
                case SIR::SIROpcode::BUILTIN_UI_GEN_IOS:
                case SIR::SIROpcode::BUILTIN_UI_GEN_MACOS:
                    return true;

                default:
                    return false;
                }
            }

            void CSEPass::replaceRegisterUses(
                SIR::SIRFunction *function,
                const std::string &oldReg,
                const std::string &newReg,
                int startBlock)
            {
                if (!function || oldReg == newReg)
                {
                    return;
                }

                auto &blocks = function->getBasicBlocks();

                // Only replace within the SAME basic block to avoid dominance issues.
                // Cross-block replacement can cause "Instruction does not dominate all uses!"
                if (startBlock >= 0 && startBlock < Sad::Security::SafeArithmetic::assertSafeCast<int>(blocks.size(), "cse_pass_size"))
                {
                    auto &block = blocks[startBlock];
                    if (!block)
                        return;

                    for (auto &inst : block->instructions)
                    {
                        for (auto &operand : inst.operands)
                        {
                            if (operand.name == oldReg)
                            {
                                operand.name = newReg;
                            }
                        }
                    }
                }
            }

            void CSEPass::clearTables()
            {
                expressionTable_.clear();
                replacementCount_ = 0;
            }

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
