/**
 * @file dead_code_elimination_pass.cpp
 * @brief تطبيق تمرير حذف الكود الميت
 * @brief Dead Code Elimination Pass Implementation
 *
 * @details
 * (AR) يزيل التعليمات التي لا تؤثر على نتيجة البرنامج.
 * (EN) Removes instructions that do not affect program output.
 *
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "middle/dead_code_elimination_pass.h"
#include <queue>
#include <algorithm>
#include <unordered_map>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            /**
             * @brief Constructor / البناء
             */
            DeadCodeEliminationPass::DeadCodeEliminationPass()
                : OptimizationPass("Dead Code Elimination", PassType::TRANSFORMATION)
            {
            }

            /**
             * @brief تشغيل التمرير على دالة
             * @brief Run pass on function
             */
            bool DeadCodeEliminationPass::runOnFunction(SIR::SIRFunction *function)
            {
                if (!function)
                {
                    return false;
                }

                bool modified = false;

                // المرحلة 1: حذف الكتل غير القابلة للوصول / Phase 1: Remove unreachable blocks
                int unreachableRemoved = removeUnreachableBlocks(function);
                if (unreachableRemoved > 0)
                {
                    modified = true;
                    recordModification();
                    debug("Removed " + std::to_string(unreachableRemoved) + " unreachable blocks");
                }

                // المرحلة 2: تحليل الحيوية / Phase 2: Liveness analysis
                auto liveInstructions = computeLiveInstructions(function);
                debug("Found " + std::to_string(liveInstructions.size()) + " live instructions");

                // المرحلة 3: حذف التعليمات الميتة / Phase 3: Remove dead instructions
                const auto &blocks = function->getBasicBlocks();
                for (auto &block : blocks)
                {
                    if (block)
                    {
                        int deadRemoved = removeDeadInstructions(block.get(), liveInstructions);
                        if (deadRemoved > 0)
                        {
                            modified = true;
                            debug("Removed " + std::to_string(deadRemoved) + " dead instructions from block");
                        }
                    }
                }

                return modified;
            }

            /**
             * @brief حساب التعليمات الحية
             * @brief Compute live instructions
             *
             * @details
             * (AR) يستخدم تحليل الحيوية بالرجوع للخلف لتحديد التعليمات الحية.
             * (EN) Uses backward liveness analysis to determine live instructions.
             */
            std::unordered_set<SIR::SIRInstruction *>
            DeadCodeEliminationPass::computeLiveInstructions(SIR::SIRFunction *function)
            {
                std::unordered_set<SIR::SIRInstruction *> liveSet;
                std::queue<SIR::SIRInstruction *> workList;

                // بناء خريطة التعريفات / Build definition map
                auto defMap = buildDefMap(function);

                // المرحلة 1: إضافة التعليمات ذات التأثيرات الجانبية / Phase 1: Add instructions with side effects
                const auto &blocks = function->getBasicBlocks();
                for (auto &block : blocks)
                {
                    if (!block)
                        continue;

                    // تكرار على تعليمات الكتلة / Iterate over block instructions
                    for (auto &inst : block->instructions)
                    {
                        // التعليمات ذات التأثيرات الجانبية حية دائماً
                        // Instructions with side effects are always live
                        if (hasSideEffect(&inst))
                        {
                            liveSet.insert(&inst);
                            workList.push(&inst);
                        }
                    }
                }

                // المرحلة 2: الانتشار العكسي / Phase 2: Backward propagation
                while (!workList.empty())
                {
                    auto *inst = workList.front();
                    workList.pop();

                    // وضع علامة على المعاملات كحية / Mark operands as live
                    markAsLive(inst, liveSet, workList, defMap);
                }

                return liveSet;
            }

            /**
             * @brief وضع علامة على التعليمة كحية
             * @brief Mark instruction as live
             */
            void DeadCodeEliminationPass::markAsLive(
                SIR::SIRInstruction *inst,
                std::unordered_set<SIR::SIRInstruction *> &liveSet,
                std::queue<SIR::SIRInstruction *> &workList,
                const std::unordered_map<std::string, SIR::SIRInstruction *> &defMap)
            {
                if (!inst)
                    return;

                // الحصول على السجلات المستخدمة / Get used registers
                auto usedRegs = getUsedRegisters(inst);

                for (const auto &reg : usedRegs)
                {
                    // البحث عن التعليمة التي تعرّف هذا السجل / Find instruction defining this register
                    auto it = defMap.find(reg);
                    if (it != defMap.end())
                    {
                        auto *defInst = it->second;

                        // إذا لم تكن حية بعد، أضفها / If not already live, add it
                        if (liveSet.find(defInst) == liveSet.end())
                        {
                            liveSet.insert(defInst);
                            workList.push(defInst);
                        }
                    }
                }
            }

            /**
             * @brief بناء خريطة التعريفات
             * @brief Build definition map
             *
             * @details
             * (AR) يربط كل سجل بالتعليمة التي تعرّفه.
             * (EN) Maps each register to the instruction that defines it.
             */
            std::unordered_map<std::string, SIR::SIRInstruction *>
            DeadCodeEliminationPass::buildDefMap(SIR::SIRFunction *function)
            {
                std::unordered_map<std::string, SIR::SIRInstruction *> defMap;

                const auto &blocks = function->getBasicBlocks();
                for (auto &block : blocks)
                {
                    if (!block)
                        continue;

                    // تكرار على تعليمات الكتلة / Iterate over block instructions
                    for (auto &inst : block->instructions)
                    {
                        // الحصول على السجل المعرّف / Get defined register
                        auto defReg = getDefinedRegister(&inst);
                        if (defReg.has_value())
                        {
                            defMap[defReg.value()] = &inst;
                        }
                    }
                }

                return defMap;
            }

            /**
             * @brief حذف التعليمات الميتة من كتلة
             * @brief Remove dead instructions from block
             */
            int DeadCodeEliminationPass::removeDeadInstructions(
                SIR::SIRBasicBlock *block,
                const std::unordered_set<SIR::SIRInstruction *> &liveSet)
            {
                if (!block)
                    return 0;

                int removedCount = 0;

                // إنشاء قائمة جديدة بدون التعليمات الميتة / Create new list without dead instructions
                std::vector<SIR::SIRInstruction> newInstructions;

                for (auto &inst : block->instructions)
                {
                    // الاحتفاظ بالتعليمات الحية فقط / Keep only live instructions
                    if (liveSet.find(&inst) != liveSet.end())
                    {
                        newInstructions.push_back(inst);
                    }
                    else
                    {
                        removedCount++;
                        recordModification();

                        // تسجيل التعليمة المحذوفة / Log removed instruction
                        std::string instStr = "  ";
                        if (inst.hasLabel())
                        {
                            instStr += inst.label + ": ";
                        }
                        if (inst.hasResult())
                        {
                            instStr += inst.result->name + " = ";
                        }
                        instStr += SIR::sirOpcodeToString(inst.opcode);
                        debug("Removing dead instruction: " + instStr);
                    }
                }

                // تحديث قائمة التعليمات في الكتلة / Update block's instruction list
                block->instructions = std::move(newInstructions);

                return removedCount;
            }

            /**
             * @brief حذف الكتل غير القابلة للوصول
             * @brief Remove unreachable blocks
             */
            int DeadCodeEliminationPass::removeUnreachableBlocks(SIR::SIRFunction *function)
            {
                if (!function)
                    return 0;

                // حساب الكتل القابلة للوصول / Compute reachable blocks
                auto reachableBlocks = computeReachableBlocks(function);

                const auto &blocks = function->getBasicBlocks();
                int totalBlocks = blocks.size();
                int reachableCount = reachableBlocks.size();

                if (reachableCount == totalBlocks)
                {
                    return 0; // كل الكتل قابلة للوصول / All blocks are reachable
                }

                // إنشاء قائمة جديدة بالكتل القابلة للوصول فقط
                // Create new list with only reachable blocks
                std::vector<std::shared_ptr<SIR::SIRBasicBlock>> newBlocks;

                for (auto &block : blocks)
                {
                    if (!block)
                        continue;

                    if (reachableBlocks.find(block.get()) != reachableBlocks.end())
                    {
                        newBlocks.push_back(block);
                    }
                    else
                    {
                        recordModification();
                        debug("Removing unreachable block: " + block->name);
                    }
                }

                // تحديث قائمة الكتل في الدالة / Update function's block list
                function->basicBlocks = std::move(newBlocks);

                int removedCount = totalBlocks - reachableCount;
                return removedCount;
            }

            /**
             * @brief حساب الكتل القابلة للوصول
             * @brief Compute reachable blocks
             *
             * @details
             * (AR) يستخدم BFS من كتلة الدخول. يستنتج الخلفاء من تعليمات القفز
             *      (BR و BR_COND) بدلاً من الاعتماد على قائمة successors المسبقة،
             *      لأن المُبني (SIRBuilder) لا يملأ تلك القائمة حالياً.
             * (EN) Uses BFS from entry block. Derives successors from branch
             *      instructions (BR and BR_COND) rather than relying on pre-populated
             *      successors list, because SIRBuilder does not populate it currently.
             */
            std::unordered_set<SIR::SIRBasicBlock *>
            DeadCodeEliminationPass::computeReachableBlocks(SIR::SIRFunction *function)
            {
                std::unordered_set<SIR::SIRBasicBlock *> reachable;

                // الحصول على كتلة الدخول / Get entry block
                const auto &blocks = function->getBasicBlocks();
                if (blocks.empty())
                {
                    return reachable;
                }

                // ================================================================
                // (AR) بناء خريطة اسم → كتلة لاستخدامها في حل أهداف القفز
                // (EN) Build name→block map for resolving branch targets
                // ================================================================
                std::unordered_map<std::string, SIR::SIRBasicBlock *> blockMap;
                for (const auto &block : blocks)
                {
                    if (block)
                    {
                        blockMap[block->name] = block.get();
                    }
                }

                auto *entryBlock = blocks[0].get();
                if (!entryBlock)
                {
                    return reachable;
                }

                // BFS من كتلة الدخول / BFS from entry block
                std::queue<SIR::SIRBasicBlock *> workList;
                workList.push(entryBlock);
                reachable.insert(entryBlock);

                while (!workList.empty())
                {
                    auto *block = workList.front();
                    workList.pop();

                    // ============================================================
                    // (AR) استنتاج الخلفاء من تعليمات القفز في الكتلة
                    //      نبحث عن BR (قفز غير شرطي) و BR_COND (قفز شرطي)
                    //      ونستخرج أسماء الكتل المستهدفة من المعاملات
                    // (EN) Derive successors from branch instructions in the block.
                    //      Look for BR (unconditional) and BR_COND (conditional)
                    //      and extract target block names from operands.
                    // ============================================================

                    // أولاً: استخدم القائمة المسبقة إن وُجدت / First: use pre-populated list if available
                    for (auto *succ : block->successors)
                    {
                        if (succ && reachable.find(succ) == reachable.end())
                        {
                            reachable.insert(succ);
                            workList.push(succ);
                        }
                    }

                    // ثانياً: استنتج الخلفاء من التعليمات / Second: derive successors from instructions
                    for (const auto &inst : block->instructions)
                    {
                        if (inst.opcode == SIR::SIROpcode::BR)
                        {
                            // BR has 1 label operand: target
                            if (!inst.operands.empty() && inst.operands[0].type == SIR::SIROperandType::LABEL)
                            {
                                auto it = blockMap.find(inst.operands[0].name);
                                if (it != blockMap.end() && reachable.find(it->second) == reachable.end())
                                {
                                    reachable.insert(it->second);
                                    workList.push(it->second);
                                }
                            }
                        }
                        else if (inst.opcode == SIR::SIROpcode::BR_COND)
                        {
                            // BR_COND has operands: condition, thenLabel, elseLabel
                            for (const auto &op : inst.operands)
                            {
                                if (op.type == SIR::SIROperandType::LABEL)
                                {
                                    auto it = blockMap.find(op.name);
                                    if (it != blockMap.end() && reachable.find(it->second) == reachable.end())
                                    {
                                        reachable.insert(it->second);
                                        workList.push(it->second);
                                    }
                                }
                            }
                        }
                    }
                }

                return reachable;
            }

            /**
             * @brief (AR) التحقق من وجود تأثير جانبي — نهج القائمة السوداء (الآمن)
             * @brief (EN) Check if instruction has side effect — blacklist/pure-list approach (safe)
             *
             * (AR) [إعادة تصميم جذرية — تحويل من whitelist إلى blacklist]
             *
             *      المنطق القديم (whitelist): سرد كل opcode ذي تأثير جانبي يدوياً.
             *      المشكلة: عند إضافة opcode جديد ونسيان إدراجه، يحذفه DCE كـ dead code.
             *      هذا ما حدث في Fix #48 مع BUILTIN_ARRAY_SORT و BUILTIN_ARRAY_REVERSE.
             *
             *      المنطق الجديد (blacklist/pure-list): سرد فقط الحسابات النقية (pure computations)
             *      التي يمكن حذفها بأمان إذا لم تُستخدم نتيجتها.
             *      أي opcode جديد يُضاف مستقبلاً يُعتبر تلقائياً ذا تأثير جانبي (الافتراض الآمن).
             *
             * (EN) [Fundamental redesign — whitelist to blacklist conversion]
             *
             *      Old logic (whitelist): manually list every opcode with side effects.
             *      Problem: forgetting to add a new opcode causes DCE to remove it as dead code.
             *      This caused Fix #48 where BUILTIN_ARRAY_SORT/REVERSE were silently removed.
             *
             *      New logic (blacklist/pure-list): list only pure computations that can be
             *      safely removed when their result is unused.
             *      Any new opcode added in the future defaults to having side effects (safe default).
             */
            bool DeadCodeEliminationPass::hasSideEffect(SIR::SIRInstruction *inst) const
            {
                if (!inst)
                    return false;

                auto opcode = inst->opcode;

                // ══════════════════════════════════════════════════════════════════
                // (AR) الحسابات النقية فقط — إذا لم تُستخدم نتيجتها يمكن حذفها بأمان
                //      هذه التعليمات لا تُعدّل أي حالة خارجية، ولا تُنتج I/O، ولا تُغيّر الذاكرة
                // (EN) Pure computations only — safe to remove if result is unused
                //      These instructions do not modify external state, produce I/O, or mutate memory
                // ══════════════════════════════════════════════════════════════════
                switch (opcode)
                {
                // ────────────────────────────────────────────────────────────────
                // (AR) 1. العمليات الحسابية — حسابات رياضية نقية
                // (EN) 1. Arithmetic operations — pure math computations
                // ────────────────────────────────────────────────────────────────
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
                case SIR::SIROpcode::NEG:

                // ────────────────────────────────────────────────────────────────
                // (AR) 2. العمليات الثنائية — تحويل بتات نقي
                // (EN) 2. Bitwise operations — pure bit manipulation
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::AND:
                case SIR::SIROpcode::OR:
                case SIR::SIROpcode::XOR:
                case SIR::SIROpcode::NOT:
                case SIR::SIROpcode::SHL:
                case SIR::SIROpcode::SHR:
                case SIR::SIROpcode::SAR:
                case SIR::SIROpcode::ROL:

                // ────────────────────────────────────────────────────────────────
                // (AR) 3. المقارنات — تنتج منطقي نقي
                // (EN) 3. Comparisons — produce pure boolean
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::EQ:
                case SIR::SIROpcode::NE:
                case SIR::SIROpcode::LT:
                case SIR::SIROpcode::LE:
                case SIR::SIROpcode::GT:
                case SIR::SIROpcode::GE:

                // ────────────────────────────────────────────────────────────────
                // (AR) 4. قراءة الذاكرة والعناوين — لا تُعدّل شيئاً
                // (EN) 4. Memory reads and addresses — no mutation
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::LOAD:
                case SIR::SIROpcode::ADDR:
                case SIR::SIROpcode::PTR_ADD:
                case SIR::SIROpcode::PTR_CAST:
                case SIR::SIROpcode::MOVE:

                // ────────────────────────────────────────────────────────────────
                // (AR) 5. قراءة المصفوفات والصفوف — لا تُعدّل المصفوفة
                // (EN) 5. Array/Tuple reads — no mutation
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::ARRAY_GET:
                case SIR::SIROpcode::ARRAY_LEN:
                case SIR::SIROpcode::ARRAY_CONCAT:
                case SIR::SIROpcode::TUPLE_GET:
                case SIR::SIROpcode::TUPLE_LEN:

                // ────────────────────────────────────────────────────────────────
                // (AR) 6. عمليات النصوص — تنشئ نصوصاً جديدة (لا تُعدّل الأصلي)
                // (EN) 6. String operations — create new strings (no mutation)
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::STRING_LEN:
                case SIR::SIROpcode::STRING_CONCAT:
                case SIR::SIROpcode::STRING_CMP:
                case SIR::SIROpcode::STRING_SUBSTR:
                case SIR::SIROpcode::STRING_FIND:
                case SIR::SIROpcode::STRING_REPLACE:
                case SIR::SIROpcode::STRING_TO_I64:
                case SIR::SIROpcode::STRING_TO_F64:

                // ────────────────────────────────────────────────────────────────
                // (AR) 7. قراءة الكائنات والتحقق من الأنواع — لا تأثير جانبي
                // (EN) 7. Object reads and type checks — no side effects
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::OBJECT_GET:
                case SIR::SIROpcode::INSTANCEOF:
                case SIR::SIROpcode::OBJECT_CAST:

                // ────────────────────────────────────────────────────────────────
                // (AR) 8. قراءة التعدادات الجبرية — استخراج نقي
                // (EN) 8. ADT enum reads — pure extraction
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::ENUM_GET_TAG:
                case SIR::SIROpcode::ENUM_GET_PAYLOAD:
                case SIR::SIROpcode::ENUM_IS_VARIANT:

                // ────────────────────────────────────────────────────────────────
                // (AR) 9. تحويل الأنواع — تحويل نقي لقيمة
                // (EN) 9. Type conversions — pure value transformation
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::I64_TO_F64:
                case SIR::SIROpcode::F64_TO_I64:
                case SIR::SIROpcode::I64_TO_BOOL:
                case SIR::SIROpcode::BOOL_TO_I64:
                case SIR::SIROpcode::I64_TO_STRING:
                case SIR::SIROpcode::F64_TO_STRING:
                case SIR::SIROpcode::BOOL_TO_STRING:
                case SIR::SIROpcode::ARRAY_TO_STRING:
                case SIR::SIROpcode::TUPLE_TO_STRING:
                case SIR::SIROpcode::CAST:

                // ────────────────────────────────────────────────────────────────
                // (AR) 10. الدوال الرياضية المدمجة — حسابات نقية
                // (EN) 10. Builtin math functions — pure computations
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::BUILTIN_SQRT:
                case SIR::SIROpcode::BUILTIN_POW:
                case SIR::SIROpcode::BUILTIN_SIN:
                case SIR::SIROpcode::BUILTIN_COS:
                case SIR::SIROpcode::BUILTIN_TAN:
                case SIR::SIROpcode::BUILTIN_ABS:
                case SIR::SIROpcode::BUILTIN_FLOOR:
                case SIR::SIROpcode::BUILTIN_CEIL:
                case SIR::SIROpcode::BUILTIN_ROUND:
                case SIR::SIROpcode::BUILTIN_MIN:
                case SIR::SIROpcode::BUILTIN_MAX:
                case SIR::SIROpcode::BUILTIN_SUM:

                // ────────────────────────────────────────────────────────────────
                // (AR) 11. دوال النصوص المدمجة — قراءة/تحويل نقي
                // (EN) 11. Builtin string functions — pure read/transform
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::BUILTIN_STRING_LENGTH:
                case SIR::SIROpcode::BUILTIN_STRING_CHAR_AT:
                case SIR::SIROpcode::BUILTIN_STRING_TO_UPPER:
                case SIR::SIROpcode::BUILTIN_STRING_TO_LOWER:
                case SIR::SIROpcode::BUILTIN_STRING_FIND:
                case SIR::SIROpcode::BUILTIN_STRING_REPLACE:
                case SIR::SIROpcode::BUILTIN_STRING_SUBSTRING:
                case SIR::SIROpcode::BUILTIN_STRING_TRIM:
                case SIR::SIROpcode::BUILTIN_STRING_SPLIT:
                case SIR::SIROpcode::BUILTIN_STRING_JOIN:
                case SIR::SIROpcode::BUILTIN_STRING_STARTS_WITH:
                case SIR::SIROpcode::BUILTIN_STRING_ENDS_WITH:
                case SIR::SIROpcode::BUILTIN_STRING_CONTAINS:

                // ────────────────────────────────────────────────────────────────
                // (AR) 12. دوال المصفوفات المدمجة (قراءة فقط) — لا تُعدّل المصفوفة
                // (EN) 12. Builtin array functions (read-only) — no mutation
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::BUILTIN_ARRAY_SIZE:
                case SIR::SIROpcode::BUILTIN_ARRAY_INDEX_OF:
                case SIR::SIROpcode::BUILTIN_ARRAY_CONTAINS:
                case SIR::SIROpcode::BUILTIN_ARRAY_FIRST:
                case SIR::SIROpcode::BUILTIN_ARRAY_LAST:
                case SIR::SIROpcode::BUILTIN_ARRAY_SLICE:

                // ────────────────────────────────────────────────────────────────
                // (AR) 13. فحص الأنواع المدمج — قراءة نقية
                // (EN) 13. Builtin type checks — pure reads
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::BUILTIN_TYPE_OF:
                case SIR::SIROpcode::BUILTIN_IS_INTEGER:
                case SIR::SIROpcode::BUILTIN_IS_FLOAT:
                case SIR::SIROpcode::BUILTIN_IS_STRING:
                case SIR::SIROpcode::BUILTIN_IS_ARRAY:
                case SIR::SIROpcode::BUILTIN_TO_BOOL:

                // ────────────────────────────────────────────────────────────────
                // (AR) 14. دوال الأمان النقية — فحص/هاش بدون تأثير خارجي
                // (EN) 14. Pure security functions — check/hash without external effect
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::BUILTIN_SECURITY_VERIFY:
                case SIR::SIROpcode::BUILTIN_SECURITY_IS_SAFE:
                case SIR::SIROpcode::BUILTIN_SECURITY_HASH:
                case SIR::SIROpcode::BUILTIN_SECURITY_BASE64_ENCODE:

                // ────────────────────────────────────────────────────────────────
                // (AR) 15. توجيهات نقية — حجم النوع
                // (EN) 15. Pure directives — sizeof
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::Sizeof:

                // ────────────────────────────────────────────────────────────────
                // (AR) 16. قراءة بيئة الإغلاق — لا تُعدّل البيئة
                // (EN) 16. Closure env read — no env mutation
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::ENV_LOAD:

                // ────────────────────────────────────────────────────────────────
                // (AR) 17. عقدة SSA PHI — عقدة دمج SSA لا تأثير لها
                // (EN) 17. SSA PHI node — merge node with no side effect
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::PHI:

                // ────────────────────────────────────────────────────────────────
                // (AR) 18. لا عملية — لا تأثير
                // (EN) 18. No operation — no effect
                // ────────────────────────────────────────────────────────────────
                case SIR::SIROpcode::Nop:
                    // (AR) كل ما سبق حسابات نقية — يمكن حذفها إذا لم تُستخدم نتيجتها
                    // (EN) All above are pure computations — safe to remove if result unused
                    return false;

                // ══════════════════════════════════════════════════════════════════
                // (AR) الافتراض الآمن: أي opcode غير مدرج أعلاه يُعتبر ذا تأثير جانبي
                //      هذا يمنع حذف التعليمات الجديدة بالخطأ عند إضافتها مستقبلاً
                // (EN) Safe default: any opcode NOT listed above is assumed to have side effects
                //      This prevents accidental removal of new instructions added in the future
                // ══════════════════════════════════════════════════════════════════
                default:
                    return true;
                }
            }

            /**
             * @brief الحصول على السجل المعرّف
             * @brief Get defined register
             *
             * (AR) بدلاً من استخدام قائمة بيضاء يدوية للأوامر التي تُعرّف سجلاً،
             *      نستخدم فحصاً عاماً عبر hasResult(). هذا يضمن أن أي تعليمة
             *      تُنتج نتيجة (ALLOC, LOAD, ADD, CALL, STRING_CONCAT, OBJECT_NEW, ...)
             *      يتم إدراجها في defMap تلقائياً — مما يمنع حذفها خطأً بواسطة DCE.
             *
             * (EN) Instead of a manual whitelist of opcodes that define a register,
             *      we use a generic hasResult() check. This ensures that ANY instruction
             *      producing a result (ALLOC, LOAD, ADD, CALL, STRING_CONCAT, OBJECT_NEW,
             *      ...) is automatically added to defMap — preventing erroneous DCE deletion.
             */
            std::optional<std::string> DeadCodeEliminationPass::getDefinedRegister(
                SIR::SIRInstruction *inst) const
            {
                if (!inst)
                    return std::nullopt;

                // (AR) فحص عام: أي تعليمة لها نتيجة تُعرّف سجلاً
                // (EN) Generic check: any instruction with a result defines a register
                if (inst->hasResult())
                {
                    return inst->result->name;
                }
                return std::nullopt;
            }

            /**
             * @brief الحصول على السجلات المستخدمة
             * @brief Get used registers
             */
            std::vector<std::string> DeadCodeEliminationPass::getUsedRegisters(
                SIR::SIRInstruction *inst) const
            {
                std::vector<std::string> usedRegs;

                if (!inst)
                    return usedRegs;

                // الحصول على المعاملات الفعلية / Get actual operands
                // تكرار على كل المعاملات واستخراج السجلات / Iterate over operands and extract registers
                for (const auto &operand : inst->operands)
                {
                    // استخراج السجلات فقط (تجاهل الثوابت والتسميات)
                    // Extract only registers (ignore constants and labels)
                    if (operand.type == SIR::SIROperandType::REGISTER)
                    {
                        usedRegs.push_back(operand.name);
                    }
                }

                return usedRegs;
            }

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
