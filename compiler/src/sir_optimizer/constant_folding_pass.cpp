/**
 * @file constant_folding_pass.cpp
 * @brief تنفيذ ممرّ طيّ الثوابت
 * @brief Constant Folding Pass Implementation
 *
 * @details
 * (AR) يطوي هذا الممرّ العمليّات الحسابيّة على الثوابت في وقت الترجمة.
 * (EN) This pass folds arithmetic operations on constants at compile time.
 *
 * @author SadLanguage Compiler Team
 * @date December 2025
 */

#include "sir_optimizer/constant_folding_pass.h"
#include <cmath>
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            // ============================================================================
            // Constructor / البانِي
            // ============================================================================

            ConstantFoldingPass::ConstantFoldingPass()
                : OptimizationPass("Constant Folding", PassType::TRANSFORMATION), foldCount_(0)
            {
            }

            ConstantFoldingPass::~ConstantFoldingPass() = default;

            // ============================================================================
            // Main Entry Point
            // ============================================================================

            bool ConstantFoldingPass::runOnFunction(SIR::SIRFunction *function)
            {
                if (!function)
                {
                    return false;
                }

                clearConstantTable();
                bool modified = false;

                // Process all basic blocks
                for (auto &blockPtr : function->basicBlocks)
                {
                    if (processBlock(blockPtr.get()))
                    {
                        modified = true;
                    }
                }

                if (foldCount_ > 0)
                {
                    std::cerr << "[ConstantFolding] Folded " << foldCount_ << " operations\n";
                }

                return modified;
            }

            // ============================================================================
            // Block Processing
            // ============================================================================

            bool ConstantFoldingPass::processBlock(SIR::SIRBasicBlock *block)
            {
                if (!block)
                {
                    return false;
                }

                bool modified = false;

                for (size_t i = 0; i < block->instructions.size(); i++)
                {
                    auto &inst = block->instructions[i];

                    // Try to fold based on opcode
                    using SIR::SIROpcode;

                    switch (inst.opcode)
                    {
                    // Integer arithmetic
                    case SIROpcode::ADD_I64:
                    case SIROpcode::SUB_I64:
                    case SIROpcode::MUL_I64:
                    case SIROpcode::DIV_I64:
                    case SIROpcode::FLOOR_DIV_I64:
                    case SIROpcode::MOD_I64:
                        if (tryFoldIntegerArithmetic(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    // Float arithmetic
                    case SIROpcode::ADD_F64:
                    case SIROpcode::SUB_F64:
                    case SIROpcode::MUL_F64:
                    case SIROpcode::DIV_F64:
                        if (tryFoldFloatArithmetic(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    // Comparisons
                    case SIROpcode::EQ:
                    case SIROpcode::NE:
                    case SIROpcode::LT:
                    case SIROpcode::LE:
                    case SIROpcode::GT:
                    case SIROpcode::GE:
                        if (tryFoldComparison(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    // Bitwise operations
                    case SIROpcode::AND:
                    case SIROpcode::OR:
                    case SIROpcode::XOR:
                    case SIROpcode::NOT:
                    case SIROpcode::SHL:
                    case SIROpcode::SHR:
                        if (tryFoldBitwise(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    // Negation
                    case SIROpcode::NEG:
                        if (tryFoldNegation(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    default:
                        // Track constants for propagation
                        trackConstant(inst);
                        break;
                    }
                }

                return modified;
            }

            // ============================================================================
            // Integer Arithmetic Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldIntegerArithmetic(SIR::SIRInstruction &inst)
            {
                if (inst.operands.size() < 2)
                {
                    return false;
                }

                const auto &leftOp = inst.operands[0];
                const auto &rightOp = inst.operands[1];

                using SIR::SIROpcode;

                // (AR) ISSUE-063: معاملٌ عشريّ على opcode «صحيح» (يحدث مع المسارات
                //      الديناميّة Any ومع «باقي(7.5، 2)») — دلالة المفسّر: العمليّة تصير
                //      عشريّة (fmod لـ%، floor(fdiv) لـ//). كان stoll يقتطع «7.500000» إلى
                //      7 بصمتٍ فيُطوى 7.5%2 إلى 1 بدل 1.5.
                // (EN) ISSUE-063: a float operand on an "integer" opcode (dynamic Any paths,
                //      or «باقي(7.5، 2)») — interpreter semantics: the operation becomes a
                //      float one (fmod for %, floor(fdiv) for //). stoll used to silently
                //      truncate "7.500000" to 7, folding 7.5%2 to 1 instead of 1.5.
                if (isFloatOperand(leftOp) || isFloatOperand(rightOp))
                {
                    // (AR) Amelia (ISSUE-063): نوع نتيجة التعليمة يحكم دلالة الطيّ — نتيجةٌ
                    //      Integer (مثل «باقي()» المدمجة) تُطوى بالاقتطاع (نظير FPToSI+srem في
                    //      الخلف) لا بـfmod/floor العشريّة؛ وطيُّ ثابتٍ عشريٍّ في سجلٍّ نتيجتُه
                    //      Integer يُفسد نوعَه لدى المستهلكين (باتت تُطبع بِتّاتٍ خامًا).
                    // (EN) Amelia (ISSUE-063): the instruction's RESULT type governs fold
                    //      semantics — an Integer result (e.g. the باقي() builtin) folds with
                    //      truncation (mirroring the backend's FPToSI+srem), not float
                    //      fmod/floor; folding a float constant into an Integer-typed register
                    //      corrupts its type for consumers (raw bits get printed).
                    if (inst.result.has_value() &&
                        inst.result->dataType == SIR::SadTypeKind::Integer)
                    {
                        if (inst.opcode == SIROpcode::MOD_I64)
                        {
                            std::optional<double> leftD = getFloatConstant(leftOp);
                            std::optional<double> rightD = getFloatConstant(rightOp);
                            if (!leftD || !rightD)
                                return false;
                            const int64_t lhsI = static_cast<int64_t>(*leftD);
                            const int64_t rhsI = static_cast<int64_t>(*rightD);
                            if (rhsI == 0)
                                return false;
                            replaceWithConstant(inst, lhsI % rhsI);
                            return true;
                        }
                        // (AR) بقيّة العمليّات: اتركها للخلف (يقتطع) / (EN) leave to backend
                        return false;
                    }

                    std::optional<double> leftD = getFloatConstant(leftOp);
                    std::optional<double> rightD = getFloatConstant(rightOp);
                    if (!leftD || !rightD)
                    {
                        return false;
                    }

                    double fresult = 0.0;
                    switch (inst.opcode)
                    {
                    case SIROpcode::ADD_I64:
                        fresult = *leftD + *rightD;
                        break;
                    case SIROpcode::SUB_I64:
                        fresult = *leftD - *rightD;
                        break;
                    case SIROpcode::MUL_I64:
                        fresult = *leftD * *rightD;
                        break;
                    case SIROpcode::FLOOR_DIV_I64:
                        if (*rightD == 0.0)
                            return false;
                        fresult = std::floor(*leftD / *rightD);
                        break;
                    case SIROpcode::MOD_I64:
                        if (*rightD == 0.0)
                            return false;
                        fresult = std::fmod(*leftD, *rightD);
                        break;
                    default:
                        // (AR) DIV_I64 بمعامل عشريّ — اتركها للخلف / (EN) leave to backend
                        return false;
                    }

                    replaceWithConstant(inst, fresult);
                    return true;
                }

                // Check if both operands are constants
                std::optional<int64_t> leftVal = getIntegerConstant(leftOp);
                std::optional<int64_t> rightVal = getIntegerConstant(rightOp);

                if (!leftVal || !rightVal)
                {
                    return false;
                }

                int64_t result = 0;

                // (AR) [طبقة طبيعي — الخطوة ٧] طيُّ القسمة/الباقي لا-موقَّعًا حين تكون نتيجة
                //      التعليمة طبيعي (ثبّتها الواجهةُ الأماميّة UInt64 لـ//،%). دلالةُ
                //      الطيّ من نوع النتيجة (كنمط فرع العشريّ أعلاه) كي يطابق زمنَ الترجمة
                //      urem/udiv زمنَ التشغيل والمفسّرَ (MAX%2=1 لا ‎-1‏، MAX//2=INT64_MAX لا ‎-1‏)
                //      لو صار المعامل ثابتًا (نشرُ نسخٍ). ‎-1‏ يُعاد تفسيره MAX لا-موقَّعًا؛ لا فيض.
                // (EN) [طبيعي layer — Step 7] Fold division/modulo unsigned when the instruction's
                //      result is طبيعي (the frontend pins //,% to UInt64). Fold semantics from the
                //      result type (as the float branch above) so compile-time urem/udiv matches the
                //      runtime and the interpreter (MAX%2=1 not -1, MAX//2=INT64_MAX not -1) should an
                //      operand become constant (copy propagation). -1 reinterprets as unsigned MAX; no overflow.
                const bool foldUnsignedU64 =
                    inst.result.has_value() &&
                    inst.result->dataType == SIR::SadTypeKind::UInt64;

                switch (inst.opcode)
                {
                case SIROpcode::ADD_I64:
                    result = *leftVal + *rightVal;
                    break;
                case SIROpcode::SUB_I64:
                    result = *leftVal - *rightVal;
                    break;
                case SIROpcode::MUL_I64:
                    result = *leftVal * *rightVal;
                    break;
                case SIROpcode::DIV_I64:
                    if (*rightVal == 0)
                        return false; // Avoid division by zero
                    // (AR) 🔑 فيضُ INT64_MIN / -1 لا يُطوى: الطيُّ نفسُه سلوكٌ غيرُ
                    //      معرَّفٍ في مصرّف C++ الذي يبني هذه الأداة، والنتيجةُ المطويّةُ
                    //      تُدفَن في الثنائيّ بلا حارس. نتركها لحارسِ زمنِ التشغيل (RUN011).
                    //      كانت خاملةً لأنّ `/` لم تكن تُصدِر DIV_I64 قطُّ؛ فلمّا صارت
                    //      قسمةً صحيحةً ساكنةً صار هذا المسارُ مطروقًا.
                    // (EN) INT64_MIN / -1 must not fold: the fold itself is UB in the C++
                    //      compiler building this tool, and a wrongly folded constant is
                    //      buried in the binary with no guard. Leave it to the runtime
                    //      guard (RUN011). Dormant until `/` started emitting DIV_I64.
                    if (!foldUnsignedU64 && *leftVal == INT64_MIN && *rightVal == -1)
                        return false;
                    result = foldUnsignedU64
                                 ? static_cast<int64_t>(static_cast<uint64_t>(*leftVal) /
                                                        static_cast<uint64_t>(*rightVal))
                                 : *leftVal / *rightVal;
                    break;
                case SIROpcode::FLOOR_DIV_I64:
                {
                    if (*rightVal == 0)
                        return false;
                    if (foldUnsignedU64)
                    {
                        // (AR) لا-موقَّع: لا سالب فالأرضيّة = الاقتطاع (udiv)
                        // (EN) unsigned: no negatives so floor == truncation (udiv)
                        result = static_cast<int64_t>(static_cast<uint64_t>(*leftVal) /
                                                      static_cast<uint64_t>(*rightVal));
                        break;
                    }
                    // (AR) والأرضيّةُ نظيرتُها: INT64_MIN // -1 تفيض كذلك — ثغرةٌ
                    //      كامنةٌ سبقت هذا القرار، تُسدّ معه.
                    // (EN) Floor division overflows identically — a latent hole
                    //      that predates this change, sealed with it.
                    if (*leftVal == INT64_MIN && *rightVal == -1)
                        return false;
                    int64_t q = *leftVal / *rightVal;
                    if ((*leftVal ^ *rightVal) < 0 && *leftVal % *rightVal != 0)
                        q -= 1;
                    result = q;
                    break;
                }
                case SIROpcode::MOD_I64:
                    if (*rightVal == 0)
                        return false; // Avoid division by zero
                    result = foldUnsignedU64
                                 ? static_cast<int64_t>(static_cast<uint64_t>(*leftVal) %
                                                        static_cast<uint64_t>(*rightVal))
                                 : *leftVal % *rightVal;
                    break;
                default:
                    return false;
                }

                // Replace instruction with constant
                replaceWithConstant(inst, result);
                return true;
            }

            // ============================================================================
            // Float Arithmetic Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldFloatArithmetic(SIR::SIRInstruction &inst)
            {
                if (inst.operands.size() < 2)
                {
                    return false;
                }

                const auto &leftOp = inst.operands[0];
                const auto &rightOp = inst.operands[1];

                // Check if both operands are constants
                std::optional<double> leftVal = getFloatConstant(leftOp);
                std::optional<double> rightVal = getFloatConstant(rightOp);

                if (!leftVal || !rightVal)
                {
                    return false;
                }

                double result = 0.0;
                using SIR::SIROpcode;

                switch (inst.opcode)
                {
                case SIROpcode::ADD_F64:
                    result = *leftVal + *rightVal;
                    break;
                case SIROpcode::SUB_F64:
                    result = *leftVal - *rightVal;
                    break;
                case SIROpcode::MUL_F64:
                    result = *leftVal * *rightVal;
                    break;
                case SIROpcode::DIV_F64:
                {
                    if (*rightVal == 0.0)
                        return false; // Avoid division by zero
                    // (AR) ISSUE-063: القسمة `/` على ثابتين صحيحين — دلالة المفسّر: صحيح عند
                    //      انعدام الباقي (6/3⇒2) وعشريّ عند وجوده (7/2⇒3.5).
                    // (EN) ISSUE-063: `/` on two integer constants — interpreter semantics:
                    //      int iff the remainder is zero (6/3⇒2), float otherwise (7/2⇒3.5).
                    if (!isFloatOperand(leftOp) && !isFloatOperand(rightOp))
                    {
                        std::optional<int64_t> leftI = getIntegerConstant(leftOp);
                        std::optional<int64_t> rightI = getIntegerConstant(rightOp);
                        if (leftI && rightI && *rightI != 0 && (*leftI % *rightI == 0))
                        {
                            replaceWithConstant(inst, *leftI / *rightI);
                            return true;
                        }
                    }
                    result = *leftVal / *rightVal;
                    break;
                }
                default:
                    return false;
                }

                // Replace instruction with constant
                replaceWithConstant(inst, result);
                return true;
            }

            // ============================================================================
            // Comparison Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldComparison(SIR::SIRInstruction &inst)
            {
                if (inst.operands.size() < 2)
                {
                    return false;
                }

                const auto &leftOp = inst.operands[0];
                const auto &rightOp = inst.operands[1];

                // (AR) Amelia (ISSUE-063): معاملٌ عشريّ ⇒ تجاوزِ المقارنةَ الصحيحة —
                //      getIntegerConstant يقتطع «3.500000» إلى 3 بصمت فيطوي «7/2 == 3.6»
                //      إلى صحيح. الفرع العشريّ أدناه يقارن double كاملًا (نظير useDouble).
                // (EN) Amelia (ISSUE-063): a float operand ⇒ skip the integer comparison —
                //      getIntegerConstant silently truncates "3.500000" to 3, folding
                //      "7/2 == 3.6" to true. The float branch below compares full doubles
                //      (mirroring the interpreter's useDouble).
                const bool cmpHasFloat = isFloatOperand(leftOp) || isFloatOperand(rightOp);

                // (AR) منطقيٌّ في مواجهةِ عدد: **لا يُطوى** إلى مقارنةٍ عدديّة.
                //      عقدُ المفسّرِ يجعل المنطقيَّ نوعًا قائمًا بذاته لا عددًا متنكّرًا،
                //      فـ«1 == صحيح» عندَه «خطأ». وكان الطيُّ يقرأ قيمتَيهما الصحيحتَين
                //      (١ و١) فيطويها إلى «صحيح» — جوابٌ يخالف المحرّكَ الآخرَ لبرنامجٍ
                //      واحدٍ، ويُحسَم زمنَ الترجمةِ فلا يبقى في المُصدَرِ أثرٌ يُقاس.
                //      والتسليمُ للمُصدِر هو الصواب: هناك بوّابةُ boolVersusNumber تُجيب
                //      الجوابَ الثابتَ الصحيحَ (خطأ/صحيح) بدلَ اختراعِه هنا.
                // (EN) Bool versus number: do **not** fold to a numeric comparison.
                //      The interpreter's contract makes bool its own type, not an integer in
                //      disguise, so `1 == true` is false there. This folder was reading both
                //      integer values (1 and 1) and folding to true — an answer that disagrees
                //      with the other engine for one program, decided at compile time so no
                //      trace survives in the emitted code to be measured. Deferring to the
                //      emitter is the fix: its boolVersusNumber gate gives the correct constant
                //      verdict (false/true) instead of inventing one here.
                const bool leftIsBool = leftOp.dataType == SIR::SadTypeKind::Boolean;
                const bool rightIsBool = rightOp.dataType == SIR::SadTypeKind::Boolean;
                const bool leftIsNumber = leftOp.dataType == SIR::SadTypeKind::Integer ||
                                          leftOp.dataType == SIR::SadTypeKind::Float;
                const bool rightIsNumber = rightOp.dataType == SIR::SadTypeKind::Integer ||
                                           rightOp.dataType == SIR::SadTypeKind::Float;
                if ((leftIsBool && rightIsNumber) || (rightIsBool && leftIsNumber))
                {
                    return false;
                }

                // Try integer comparison first
                std::optional<int64_t> leftInt = getIntegerConstant(leftOp);
                std::optional<int64_t> rightInt = getIntegerConstant(rightOp);

                if (leftInt && rightInt && !cmpHasFloat)
                {
                    bool result = false;
                    using SIR::SIROpcode;

                    switch (inst.opcode)
                    {
                    case SIROpcode::EQ:
                        result = (*leftInt == *rightInt);
                        break;
                    case SIROpcode::NE:
                        result = (*leftInt != *rightInt);
                        break;
                    case SIROpcode::LT:
                        result = (*leftInt < *rightInt);
                        break;
                    case SIROpcode::LE:
                        result = (*leftInt <= *rightInt);
                        break;
                    case SIROpcode::GT:
                        result = (*leftInt > *rightInt);
                        break;
                    case SIROpcode::GE:
                        result = (*leftInt >= *rightInt);
                        break;
                    default:
                        return false;
                    }

                    replaceWithConstant(inst, result);
                    return true;
                }

                // Try float comparison
                std::optional<double> leftFloat = getFloatConstant(leftOp);
                std::optional<double> rightFloat = getFloatConstant(rightOp);

                if (leftFloat && rightFloat)
                {
                    bool result = false;
                    using SIR::SIROpcode;

                    switch (inst.opcode)
                    {
                    case SIROpcode::EQ:
                        result = (*leftFloat == *rightFloat);
                        break;
                    case SIROpcode::NE:
                        result = (*leftFloat != *rightFloat);
                        break;
                    case SIROpcode::LT:
                        result = (*leftFloat < *rightFloat);
                        break;
                    case SIROpcode::LE:
                        result = (*leftFloat <= *rightFloat);
                        break;
                    case SIROpcode::GT:
                        result = (*leftFloat > *rightFloat);
                        break;
                    case SIROpcode::GE:
                        result = (*leftFloat >= *rightFloat);
                        break;
                    default:
                        return false;
                    }

                    replaceWithConstant(inst, result);
                    return true;
                }

                return false;
            }

            // ============================================================================
            // Bitwise Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldBitwise(SIR::SIRInstruction &inst)
            {
                using SIR::SIROpcode;

                // NOT عمليّة أحاديّة — تُعالَج هنا قبل مسار العمليّات الثنائيّة
                if (inst.opcode == SIROpcode::NOT)
                {
                    if (inst.operands.empty())
                        return false;

                    bool isBooleanNot = (inst.operands[0].dataType == SIR::SadTypeKind::Boolean);
                    std::optional<int64_t> val = getIntegerConstant(inst.operands[0]);
                    if (!val)
                        return false;

                    if (isBooleanNot)
                    {
                        // NOT على منطقيّ: اقلب القيمة المنطقيّة
                        replaceWithConstant(inst, (*val == 0));
                    }
                    else
                    {
                        replaceWithConstant(inst, ~(*val));
                    }
                    return true;
                }

                // العمليّات الثنائيّة
                if (inst.operands.size() < 2)
                {
                    return false;
                }

                // تحقّق ممّا إذا كان كلا المعاملَين منطقيًّا (Boolean)
                bool isBooleanOp = (inst.operands[0].dataType == SIR::SadTypeKind::Boolean &&
                                    inst.operands[1].dataType == SIR::SadTypeKind::Boolean);

                std::optional<int64_t> leftVal = getIntegerConstant(inst.operands[0]);
                std::optional<int64_t> rightVal = getIntegerConstant(inst.operands[1]);

                if (!leftVal || !rightVal)
                {
                    return false;
                }

                int64_t result = 0;

                switch (inst.opcode)
                {
                case SIROpcode::AND:
                    result = *leftVal & *rightVal;
                    break;
                case SIROpcode::OR:
                    result = *leftVal | *rightVal;
                    break;
                case SIROpcode::XOR:
                    result = *leftVal ^ *rightVal;
                    break;
                case SIROpcode::SHL:
                    result = *leftVal << *rightVal;
                    break;
                case SIROpcode::SHR:
                    // (AR) [الخطوة ٨] النتيجة طبيعي (ثبّتتها الواجهة UInt64 لإزاحةٍ يسارُها
                    //      طبيعي) ⇒ إزاحةٌ منطقيّة على uint64_t (تطابق CreateLShr زمنَ التشغيل:
                    //      MAX>>1 = 2^63-1). غير ذلك: إزاحةٌ حسابيّة موقَّعة (int64_t، تطابق AShr:
                    //      -8>>1=-4). دلالةُ الطيّ من نوع النتيجة كنمط فرع العشريّ/القسمة.
                    // (EN) [Step 8] طبيعي result (the frontend pins UInt64 for a shift whose left is
                    //      طبيعي) ⇒ logical shift on uint64_t (matches runtime CreateLShr:
                    //      MAX>>1 = 2^63-1). Otherwise: signed arithmetic shift (int64_t, matches AShr:
                    //      -8>>1=-4). Fold semantics from the result type, as the float/division branches.
                    result = (inst.result.has_value() &&
                              inst.result->dataType == SIR::SadTypeKind::UInt64)
                                 ? static_cast<int64_t>(static_cast<uint64_t>(*leftVal) >>
                                                        static_cast<uint64_t>(*rightVal))
                                 : (*leftVal >> *rightVal);
                    break;
                default:
                    return false;
                }

                // اطوِ عمليّات AND/OR على المعاملَين المنطقيَّين أوّلًا قبل مسار الطيّ الحسابيّ
                if (isBooleanOp && (inst.opcode == SIROpcode::AND || inst.opcode == SIROpcode::OR))
                {
                    replaceWithConstant(inst, result != 0);
                }
                else
                {
                    replaceWithConstant(inst, result);
                }
                return true;
            }

            // ============================================================================
            // Negation Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldNegation(SIR::SIRInstruction &inst)
            {
                if (inst.operands.empty())
                {
                    return false;
                }

                // Try integer negation
                std::optional<int64_t> intVal = getIntegerConstant(inst.operands[0]);
                if (intVal)
                {
                    replaceWithConstant(inst, -(*intVal));
                    return true;
                }

                // Try float negation
                std::optional<double> floatVal = getFloatConstant(inst.operands[0]);
                if (floatVal)
                {
                    replaceWithConstant(inst, -(*floatVal));
                    return true;
                }

                return false;
            }

            // ============================================================================
            // Helper Methods
            // ============================================================================

            std::optional<int64_t> ConstantFoldingPass::getIntegerConstant(const SIR::SIROperand &operand)
            {
                if (operand.type == SIR::SIROperandType::CONSTANT)
                {
                    // Try to parse as integer
                    try
                    {
                        return std::stoll(operand.name);
                    }
                    catch (...)
                    {
                        return std::nullopt;
                    }
                }

                // Check constant table
                auto it = intConstants_.find(operand.name);
                if (it != intConstants_.end())
                {
                    return it->second;
                }

                return std::nullopt;
            }

            bool ConstantFoldingPass::isFloatOperand(const SIR::SIROperand &operand)
            {
                // (AR) ISSUE-063: عشريّ نوعًا، أو ثابتٌ نصّه يحمل فاصلة عشريّة (يقتطعه stoll
                //      بصمت)، أو سجّله الطيّ السابق في جدول الثوابت العشريّة.
                // (EN) ISSUE-063: float by dataType, a constant whose text carries a decimal
                //      point (silently truncated by stoll), or recorded in the float table.
                if (operand.dataType == SIR::SadTypeKind::Float)
                {
                    return true;
                }
                if (operand.type == SIR::SIROperandType::CONSTANT)
                {
                    return operand.name.find('.') != std::string::npos;
                }
                return floatConstants_.count(operand.name) > 0 &&
                       intConstants_.count(operand.name) == 0;
            }

            std::optional<double> ConstantFoldingPass::getFloatConstant(const SIR::SIROperand &operand)
            {
                if (operand.type == SIR::SIROperandType::CONSTANT)
                {
                    // Try to parse as float
                    try
                    {
                        return std::stod(operand.name);
                    }
                    catch (...)
                    {
                        return std::nullopt;
                    }
                }

                // Check constant table
                auto it = floatConstants_.find(operand.name);
                if (it != floatConstants_.end())
                {
                    return it->second;
                }

                return std::nullopt;
            }

            void ConstantFoldingPass::replaceWithConstant(SIR::SIRInstruction &inst, int64_t value)
            {
                // Keep the result, clear operands, set constant
                inst.operands.clear();

                SIR::SIROperand constOp;
                constOp.type = SIR::SIROperandType::CONSTANT;
                constOp.name = std::to_string(value);
                constOp.dataType = SIR::SadTypeKind::Integer;
                inst.operands.push_back(constOp);

                // Change opcode to MOVE (simple copy)
                inst.opcode = SIR::SIROpcode::MOVE;

                // Track the result as a constant
                if (inst.result.has_value())
                {
                    intConstants_[inst.result->name] = value;
                }
            }

            void ConstantFoldingPass::replaceWithConstant(SIR::SIRInstruction &inst, double value)
            {
                inst.operands.clear();

                SIR::SIROperand constOp;
                constOp.type = SIR::SIROperandType::CONSTANT;
                constOp.name = std::to_string(value);
                constOp.dataType = SIR::SadTypeKind::Float;
                inst.operands.push_back(constOp);

                inst.opcode = SIR::SIROpcode::MOVE;

                if (inst.result.has_value())
                {
                    floatConstants_[inst.result->name] = value;
                }
            }

            void ConstantFoldingPass::replaceWithConstant(SIR::SIRInstruction &inst, bool value)
            {
                inst.operands.clear();

                // (AR) استخدام ConstantBool يضبط حقل boolValue في الـunion ضبطًا صحيحًا
                //      بدلًا من ضبط الحقول يدويًّا (كان يترك boolValue = 0)
                // (EN) Use ConstantBool to properly set boolValue in the union
                //      instead of manually setting fields (which left boolValue = 0)
                SIR::SIROperand constOp = SIR::SIROperand::ConstantBool(value);
                inst.operands.push_back(constOp);

                inst.opcode = SIR::SIROpcode::MOVE;

                if (inst.result.has_value())
                {
                    intConstants_[inst.result->name] = value ? 1 : 0;
                }
            }

            void ConstantFoldingPass::trackConstant(const SIR::SIRInstruction &inst)
            {
                // Track MOVE instructions with constant operands
                if (inst.opcode == SIR::SIROpcode::MOVE &&
                    inst.result.has_value() &&
                    !inst.operands.empty())
                {

                    const auto &operand = inst.operands[0];
                    if (operand.type == SIR::SIROperandType::CONSTANT)
                    {
                        // (AR) ISSUE-063: كان stoll يُنجَح على «3.500000» فيسجّل الثابت
                        //      العشريّ صحيحًا مقتطَعًا (3) في جدول الصحيحين — نميّز العشريّ
                        //      أوّلًا (نوعًا أو نصًّا) ثمّ نسجّله في جدوله الصحيح.
                        // (EN) ISSUE-063: stoll used to succeed on "3.500000", recording the
                        //      float constant as a truncated integer (3) in the int table —
                        //      classify floats first (by type or text) into the right table.
                        if (isFloatOperand(operand))
                        {
                            try
                            {
                                floatConstants_[inst.result->name] = std::stod(operand.name);
                            }
                            catch (...)
                            {
                                // Not a numeric constant
                            }
                        }
                        else
                        {
                            try
                            {
                                intConstants_[inst.result->name] = std::stoll(operand.name);
                            }
                            catch (...)
                            {
                                try
                                {
                                    floatConstants_[inst.result->name] = std::stod(operand.name);
                                }
                                catch (...)
                                {
                                    // Not a numeric constant
                                }
                            }
                        }
                    }
                }
            }

            void ConstantFoldingPass::clearConstantTable()
            {
                intConstants_.clear();
                floatConstants_.clear();
            }

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
