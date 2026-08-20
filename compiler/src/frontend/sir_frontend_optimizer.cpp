// ======================================================================
// sir_frontend_optimizer.cpp - تنفيذ محسّن SIR الأمامي
//                              Frontend SIR Optimizer Implementation
// ======================================================================
// 5 ممرات تحسين / 5 Optimization Passes:
//   1. طيّ الثوابت / Constant Folding
//   2. حذف الكود الميت / Dead Code Elimination
//   3. تضمين الدوال / Function Inlining
//   4. إزالة التعبيرات المتكررة / Common Subexpression Elimination
//   5. تبسيط الحلقات / Loop Simplification
//
// المطور / Developer: خالد (Khalid) — Compiler & Core
// التاريخ / Date: July 2026
// ======================================================================

#include "frontend/sir_frontend_optimizer.h"
#include <algorithm>
#include <sstream>
#include <cmath>
#include <limits> // (AR) numeric_limits لدلالة الإشباع (م-6) / (EN) numeric_limits for saturation semantics (M-6)
#include <cassert>
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ======================================================================
            // FrontendOptStats::toString
            // ======================================================================
            std::string FrontendOptStats::toString() const
            {
                std::ostringstream oss;
                oss << "=== SIR Optimization Stats ===\n"
                    << "  Constants folded:  " << constantsFolded << "\n"
                    << "  Dead code removed: " << deadInstructionsRemoved << "\n"
                    << "  Functions inlined: " << functionsInlined << "\n"
                    << "  CSE eliminated:    " << commonSubexpressionsEliminated << "\n"
                    << "  Loops simplified:  " << loopsSimplified << "\n"
                    << "  Total passes run:  " << totalPassesRun << "\n"
                    << "  Total iterations:  " << totalIterations << "\n"
                    << "  Total optimized:   " << totalOptimizations() << "\n";
                return oss.str();
            }

            // ======================================================================
            // SIRFrontendPass default module pass
            // ======================================================================
            bool SIRFrontendPass::runOnModule(SIRModule &module)
            {
                bool changed = false;
                for (auto &func : module.getFunctions())
                {
                    if (func)
                    {
                        changed |= runOnFunction(*func);
                    }
                }
                return changed;
            }

            // ======================================================================
            // Helper: opcode classification
            // ======================================================================
            static bool isArithmeticOp(SIROpcode op)
            {
                switch (op)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::FLOOR_DIV_I64:
                case SIROpcode::MOD_I64:
                case SIROpcode::NEG:
                    return true;
                default:
                    return false;
                }
            }

            static bool isComparisonOp(SIROpcode op)
            {
                switch (op)
                {
                case SIROpcode::EQ:
                case SIROpcode::NE:
                case SIROpcode::LT:
                case SIROpcode::LE:
                case SIROpcode::GT:
                case SIROpcode::GE:
                    return true;
                default:
                    return false;
                }
            }

            // ==================================================================
            // (AR) Amelia (م-6، ثمّ إصلاح CI‏ ARM): تحويل double→i64 بدلالة الإشباع
            //      — دلالة لغة ص الواحدة المستقلّة عن المنصّة (= llvm.fptosi.sat
            //      بالضبط، وهي fcvtzs على ARM): NaN ⇒ 0، فوق INT64_MAX ⇒ INT64_MAX،
            //      تحت INT64_MIN ⇒ INT64_MIN، وإلّا اقتطاع نحو الصفر. الدلالة
            //      السابقة (cvttsd2si: خارج المدى ⇒ INT64_MIN) كانت دلالة عتاد
            //      x86-64 حصرًا فكسرت التكافؤ على macOS ‏ARM64 (المسار الزمنيّ
            //      والمفسّر يعطيان الإشباع هناك). المسار الزمنيّ المترجَم يُصدر
            //      llvm.fptosi.sat.i64.f64 (emitF64ToI64Sat) والمفسّر يشبع صراحةً
            //      — فالطيّ والزمن والمفسّر متطابقون على كلّ المنصّات.
            // (EN) Amelia (M-6, then the ARM CI fix): double→i64 with saturation —
            //      Sad's single platform-independent semantics (exactly
            //      llvm.fptosi.sat, i.e. ARM's fcvtzs): NaN ⇒ 0, above INT64_MAX ⇒
            //      INT64_MAX, below INT64_MIN ⇒ INT64_MIN, else truncation toward
            //      zero. The previous cvttsd2si semantics (out-of-range ⇒ INT64_MIN)
            //      was x86-64-only and broke parity on macOS ARM64 where both the
            //      compiled runtime path and the interpreter saturate. Compiled
            //      runtime emits llvm.fptosi.sat.i64.f64 (emitF64ToI64Sat) and the
            //      interpreter saturates explicitly — fold, runtime and interpreter
            //      agree on every platform.
            // ==================================================================
            static int64_t foldDoubleToI64Saturating(double d)
            {
                constexpr int64_t kMin = std::numeric_limits<int64_t>::min();
                constexpr int64_t kMax = std::numeric_limits<int64_t>::max();
                if (std::isnan(d))
                    return 0;
                // (AR) المدى القابل للتمثيل [-2^63، 2^63) — الحدّ الأعلى 2^63 بالضبط
                //      كـdouble؛ ما بلغه أو تجاوزه يُشبَع لأعلى، وما دون -2^63 لأسفل.
                // (EN) Representable range is [-2^63, 2^63); 2^63 is exact as double —
                //      values at/above it saturate up, values below -2^63 saturate down.
                constexpr double kTwoPow63 = 9223372036854775808.0; // 2^63
                if (d >= kTwoPow63)
                    return kMax;
                if (d < -kTwoPow63)
                    return kMin;
                return static_cast<int64_t>(d); // (AR) اقتطاع نحو الصفر، معرَّف داخل المدى / (EN) in-range truncation toward zero
            }

            static bool isBitwiseOp(SIROpcode op)
            {
                switch (op)
                {
                case SIROpcode::AND:
                case SIROpcode::OR:
                case SIROpcode::XOR:
                case SIROpcode::NOT:
                case SIROpcode::SHL:
                case SIROpcode::SHR:
                case SIROpcode::SAR:
                case SIROpcode::ROL:
                    return true;
                default:
                    return false;
                }
            }

            // ======================================================================
            // ممر 1: طيّ الثوابت / Pass 1: Constant Folding
            // ======================================================================

            bool ConstantFoldingFrontendPass::isConstant(
                const SIROperand &op,
                const std::unordered_map<std::string, SIROperand> &constants) const
            {
                if (op.type == SIROperandType::CONSTANT)
                    return true;
                if (op.type == SIROperandType::REGISTER)
                {
                    return constants.count(op.name) > 0;
                }
                return false;
            }

            SIROperand ConstantFoldingFrontendPass::resolveConstant(
                const SIROperand &op,
                const std::unordered_map<std::string, SIROperand> &constants) const
            {
                if (op.type == SIROperandType::CONSTANT)
                    return op;
                auto it = constants.find(op.name);
                if (it != constants.end())
                    return it->second;
                return op;
            }

            namespace
            {
                // (AR) نوعٌ عدديٌّ لغرضِ الطيّ — المنطقيُّ مستثنًى عمدًا.
                // (EN) Numeric for folding purposes — bool deliberately excluded.
                bool isNumericFoldKind(SadTypeKind kind)
                {
                    return kind == SadTypeKind::Integer || kind == SadTypeKind::Float;
                }

                bool boolVersusNumberOperands(const SIROperand &lhs, const SIROperand &rhs)
                {
                    return (lhs.dataType == SadTypeKind::Boolean && isNumericFoldKind(rhs.dataType)) ||
                           (rhs.dataType == SadTypeKind::Boolean && isNumericFoldKind(lhs.dataType));
                }
            } // namespace

            bool ConstantFoldingFrontendPass::foldInstruction(
                SIRInstruction &inst,
                std::unordered_map<std::string, SIROperand> &constants)
            {

                if (!inst.hasResult())
                    return false;
                const auto &resultName = inst.result->name;

                // Unary: NEG
                if (inst.opcode == SIROpcode::NEG && inst.operands.size() == 1)
                {
                    if (!isConstant(inst.operands[0], constants))
                        return false;
                    auto val = resolveConstant(inst.operands[0], constants);
                    if (val.dataType == SadTypeKind::Integer)
                    {
                        constants[resultName] = SIROperand::ConstantI64(-val.intValue);
                        return true;
                    }
                    if (val.dataType == SadTypeKind::Float)
                    {
                        constants[resultName] = SIROperand::ConstantF64(-val.floatValue);
                        return true;
                    }
                    return false;
                }

                // (AR) أحاديّ: F64_TO_I64_SAT — طيّ التحويل المُشبَع (نقطة التحويل
                //      الواحدة للمعاملات البتّيّة) بنفس دلالة الإشباع المنصّيّة:
                //      عشريّ ⇒ foldDoubleToI64Saturating؛ صحيح/منطقيّ ⇒ تمرير قيمته.
                // (EN) Unary: F64_TO_I64_SAT — fold the saturating conversion (the
                //      single conversion point for bitwise operands) with the same
                //      platform-independent saturation: Float ⇒
                //      foldDoubleToI64Saturating; Integer/Boolean ⇒ passthrough.
                if (inst.opcode == SIROpcode::F64_TO_I64_SAT && inst.operands.size() == 1)
                {
                    if (!isConstant(inst.operands[0], constants))
                        return false;
                    auto val = resolveConstant(inst.operands[0], constants);
                    if (val.dataType == SadTypeKind::Float)
                    {
                        constants[resultName] = SIROperand::ConstantI64(
                            foldDoubleToI64Saturating(val.floatValue));
                        return true;
                    }
                    if (val.dataType == SadTypeKind::Integer ||
                        val.dataType == SadTypeKind::Boolean)
                    {
                        constants[resultName] = SIROperand::ConstantI64(val.intValue);
                        return true;
                    }
                    return false;
                }

                // Binary arithmetic
                if (inst.operands.size() != 2)
                    return false;
                if (!isConstant(inst.operands[0], constants) ||
                    !isConstant(inst.operands[1], constants))
                    return false;

                auto lhs = resolveConstant(inst.operands[0], constants);
                auto rhs = resolveConstant(inst.operands[1], constants);

                // (AR) ISSUE-063: وعيٌ نوعيّ موحَّد للطيّ الحسابيّ — دلالة المفسّر (المرجع):
                //      معاملٌ عشريّ ⇒ العمليّة عشريّة (useDouble)؛ قراءة intValue من ثابت
                //      Float (أو العكس) كانت تعطي 0 فتُفسِد الطيّ. القسمة `/` على صحيحين:
                //      صحيح عند انعدام الباقي وعشريّ عند وجوده؛ و`%` على عشريّ ⇒ fmod.
                // (EN) ISSUE-063: unified type-aware arithmetic folding — interpreter
                //      (reference) semantics: a float operand ⇒ float operation (useDouble);
                //      reading intValue off a Float constant (or vice versa) yielded 0 and
                //      mis-folded. `/` on two ints: int iff the remainder is zero, float
                //      otherwise; `%` with a float operand ⇒ fmod.
                const bool lhsIsFloat = (lhs.dataType == SadTypeKind::Float);
                const bool rhsIsFloat = (rhs.dataType == SadTypeKind::Float);
                const bool anyFloat = lhsIsFloat || rhsIsFloat;
                const double lhsD = lhsIsFloat ? lhs.floatValue : static_cast<double>(lhs.intValue);
                const double rhsD = rhsIsFloat ? rhs.floatValue : static_cast<double>(rhs.intValue);
                // (AR) Amelia (ISSUE-063): نوع نتيجة التعليمة يحكم دلالة الطيّ — نتيجةٌ Integer
                //      (مثل «باقي()» المدمجة) تُطوى بدلالة الاقتطاع (نظير FPToSI في الخلف)،
                //      لا بدلالة fmod/floor العشريّة الخاصّة بـ`%`/`//` ذات النتيجة Float/Any.
                //      طيُّ ثابتٍ عشريٍّ في نتيجةٍ صحيحة يُفسد نوعَ السجلّ لدى المستهلكين.
                // (EN) Amelia (ISSUE-063): the instruction's RESULT type governs fold semantics —
                //      an Integer result (e.g. the باقي() builtin) folds with truncation semantics
                //      (mirroring the backend's FPToSI), not the float fmod/floor semantics of
                //      `%`/`//` whose results are typed Float/Any. Folding a float constant into
                //      an Integer-typed result corrupts the register type for consumers.
                const bool intResult =
                    inst.result.has_value() && inst.result->dataType == SadTypeKind::Integer;

                switch (inst.opcode)
                {
                // Integer arithmetic (float-aware / بوعي عشريّ)
                case SIROpcode::ADD_I64:
                    if (anyFloat)
                    {
                        if (intResult)
                            return false; // (AR) اتركها للخلف / (EN) leave to backend
                        constants[resultName] = SIROperand::ConstantF64(lhsD + rhsD);
                    }
                    else
                        constants[resultName] = SIROperand::ConstantI64(lhs.intValue + rhs.intValue);
                    return true;
                case SIROpcode::SUB_I64:
                    if (anyFloat)
                    {
                        if (intResult)
                            return false;
                        constants[resultName] = SIROperand::ConstantF64(lhsD - rhsD);
                    }
                    else
                        constants[resultName] = SIROperand::ConstantI64(lhs.intValue - rhs.intValue);
                    return true;
                case SIROpcode::MUL_I64:
                    if (anyFloat)
                    {
                        if (intResult)
                            return false;
                        constants[resultName] = SIROperand::ConstantF64(lhsD * rhsD);
                    }
                    else
                        constants[resultName] = SIROperand::ConstantI64(lhs.intValue * rhs.intValue);
                    return true;
                case SIROpcode::DIV_I64:
                    if (anyFloat)
                        return false; // (AR) قسمة صحيحة بمعامل عشريّ — اتركها للخلف / (EN) leave to backend
                    if (rhs.intValue == 0)
                        return false;
                    // (AR) 🔑 فيضُ INT64_MIN / -1 لا يُطوى: الطيُّ نفسُه سلوكٌ غيرُ
                    //      معرَّفٍ في مصرّف C++ الذي يبني هذه الأداة، والنتيجةُ المطويّةُ
                    //      تُدفَن في الثنائيّ بلا حارس. نتركها لحارسِ زمنِ التشغيل (RUN011).
                    //      كانت خاملةً لأنّ `/` لم تكن تُصدِر DIV_I64 قطُّ؛ فلمّا صارت
                    //      قسمةً صحيحةً ساكنةً صار هذا المسارُ مطروقًا.
                    // (EN) INT64_MIN / -1 must not fold: the fold itself is UB in the C++
                    //      compiler building this tool, and a wrongly folded constant is
                    //      buried in the binary with no guard. Leave it to the runtime
                    //      guard (RUN011). Dormant until `/` started emitting DIV_I64.
                    if (lhs.intValue == INT64_MIN && rhs.intValue == -1)
                        return false;
                    constants[resultName] = SIROperand::ConstantI64(lhs.intValue / rhs.intValue);
                    return true;
                case SIROpcode::FLOOR_DIV_I64:
                {
                    if (anyFloat)
                    {
                        // (AR) نتيجةٌ صحيحة (مدمجات) ⇒ لا طيَّ عشريًّا — الخلف يقتطع (FPToSI)
                        // (EN) An Integer result (builtins) ⇒ no float fold — backend truncates
                        if (intResult)
                            return false;
                        // (AR) المفسّر: عشريّ ⇒ floor(fdiv) بنتيجة عشريّة (7.5//2=3.0)
                        // (EN) Interpreter: float ⇒ floor(fdiv) with a float result
                        if (rhsD == 0.0)
                            return false;
                        constants[resultName] = SIROperand::ConstantF64(std::floor(lhsD / rhsD));
                        return true;
                    }
                    if (rhs.intValue == 0)
                        return false;
                    // (AR) والأرضيّةُ نظيرتُها في الفيض — ثغرةٌ كامنةٌ تُسدّ هنا كذلك.
                    // (EN) Floor division overflows identically — latent hole sealed here too.
                    if (lhs.intValue == INT64_MIN && rhs.intValue == -1)
                        return false;
                    int64_t q = lhs.intValue / rhs.intValue;
                    if ((lhs.intValue ^ rhs.intValue) < 0 && lhs.intValue % rhs.intValue != 0)
                        q -= 1;
                    constants[resultName] = SIROperand::ConstantI64(q);
                    return true;
                }
                case SIROpcode::MOD_I64:
                {
                    // (AR) ISSUE-063: معاملٌ عشريّ ⇒ fmod بنتيجة عشريّة (7.5%2=1.5) —
                    //      مطابقةً للمفسّر وللمسار الديناميكيّ frem (كان يُقتطَع إلى srem).
                    // (EN) ISSUE-063: a float operand ⇒ fmod with a float result (7.5%2=1.5),
                    //      matching the interpreter and the dynamic frem path (was truncated
                    //      to srem).
                    if (anyFloat)
                    {
                        // (AR) Amelia: نتيجةٌ صحيحة («باقي()» المدمجة) ⇒ دلالة الاقتطاع (نظير
                        //      FPToSI+srem في الخلف وtoInt في المفسّر): باقي(7.5، 2) = 1 لا 1.5.
                        // (EN) Amelia: an Integer result (the باقي() builtin) ⇒ truncation
                        //      semantics (mirroring the backend's FPToSI+srem and the
                        //      interpreter's toInt): باقي(7.5، 2) = 1, not 1.5.
                        if (intResult)
                        {
                            const int64_t lhsI = lhsIsFloat ? static_cast<int64_t>(lhs.floatValue)
                                                            : lhs.intValue;
                            const int64_t rhsI = rhsIsFloat ? static_cast<int64_t>(rhs.floatValue)
                                                            : rhs.intValue;
                            if (rhsI == 0)
                                return false;
                            constants[resultName] = SIROperand::ConstantI64(lhsI % rhsI);
                            return true;
                        }
                        if (rhsD == 0.0)
                            return false;
                        constants[resultName] = SIROperand::ConstantF64(std::fmod(lhsD, rhsD));
                        return true;
                    }
                    if (rhs.intValue == 0)
                        return false;
                    constants[resultName] = SIROperand::ConstantI64(lhs.intValue % rhs.intValue);
                    return true;
                }

                // Float arithmetic (int-constant-aware / بوعي الثوابت الصحيحة)
                case SIROpcode::ADD_F64:
                    constants[resultName] = SIROperand::ConstantF64(lhsD + rhsD);
                    return true;
                case SIROpcode::SUB_F64:
                    constants[resultName] = SIROperand::ConstantF64(lhsD - rhsD);
                    return true;
                case SIROpcode::MUL_F64:
                    constants[resultName] = SIROperand::ConstantF64(lhsD * rhsD);
                    return true;
                case SIROpcode::DIV_F64:
                {
                    // (AR) ISSUE-063: القسمة `/` على ثابتين صحيحين — صحيح عند انعدام الباقي
                    //      (6/3⇒2) وعشريّ عند وجوده (7/2⇒3.5)؛ معاملٌ عشريّ ⇒ عشريّ دائمًا.
                    // (EN) ISSUE-063: `/` on two integer constants — int iff the remainder is
                    //      zero (6/3⇒2), float otherwise (7/2⇒3.5); a float operand ⇒ float.
                    if (!anyFloat)
                    {
                        if (rhs.intValue == 0)
                            return false;
                        if (lhs.intValue % rhs.intValue == 0)
                            constants[resultName] = SIROperand::ConstantI64(lhs.intValue / rhs.intValue);
                        else
                            constants[resultName] = SIROperand::ConstantF64(
                                static_cast<double>(lhs.intValue) / static_cast<double>(rhs.intValue));
                        return true;
                    }
                    if (rhsD == 0.0)
                        return false;
                    constants[resultName] = SIROperand::ConstantF64(lhsD / rhsD);
                    return true;
                }

                // (AR) Amelia (ISSUE-063): مقارناتٌ بوعيٍ نوعيّ — قراءة intValue من ثابت Float
                //      تقارن بِتّات double كأعدادٍ صحيحة: «6/2 == 3.0» كانت تُطوى خطأً (3 ≠
                //      بِتّات 3.0)، و«6/3 == 2» كانت مكسورة قبل ISSUE-063 من الجهة المقابلة.
                //      معاملٌ عشريّ ⇒ قارن القيمتين double (نظير useDouble في المفسّر).
                // (EN) Amelia (ISSUE-063): type-aware comparisons — reading intValue off a Float
                //      constant compares raw double bits as integers: "6/2 == 3.0" folded wrong
                //      (3 ≠ bits(3.0)), and "6/3 == 2" was broken the other way before ISSUE-063.
                //      A float operand ⇒ compare as doubles (mirror the interpreter's useDouble).
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
                case SIROpcode::EQ:
                    if (boolVersusNumberOperands(lhs, rhs))
                        return false;
                    constants[resultName] = SIROperand::ConstantBool(
                        anyFloat ? (lhsD == rhsD) : (lhs.intValue == rhs.intValue));
                    return true;
                case SIROpcode::NE:
                    if (boolVersusNumberOperands(lhs, rhs))
                        return false;
                    constants[resultName] = SIROperand::ConstantBool(
                        anyFloat ? (lhsD != rhsD) : (lhs.intValue != rhs.intValue));
                    return true;
                case SIROpcode::LT:
                    constants[resultName] = SIROperand::ConstantBool(
                        anyFloat ? (lhsD < rhsD) : (lhs.intValue < rhs.intValue));
                    return true;
                case SIROpcode::LE:
                    constants[resultName] = SIROperand::ConstantBool(
                        anyFloat ? (lhsD <= rhsD) : (lhs.intValue <= rhs.intValue));
                    return true;
                case SIROpcode::GT:
                    constants[resultName] = SIROperand::ConstantBool(
                        anyFloat ? (lhsD > rhsD) : (lhs.intValue > rhs.intValue));
                    return true;
                case SIROpcode::GE:
                    constants[resultName] = SIROperand::ConstantBool(
                        anyFloat ? (lhsD >= rhsD) : (lhs.intValue >= rhs.intValue));
                    return true;

                // Bitwise (integer) — مع حفظ النوع المنطقي
                // (AR) إذا كان كلا المعاملين منطقيين، تُرجع ConstantBool بدلاً من ConstantI64
                //      لأن AND/OR على Boolean يجب أن تُحافظ على نوع Boolean
                //      وإلا يطبع PRINT "1" بدلاً من "صحيح"
                // (EN) If both operands are Boolean, return ConstantBool instead of ConstantI64
                //      because AND/OR on Boolean must preserve Boolean type
                case SIROpcode::AND:
                {
                    // (AR) Amelia (م-1+م-6): معاملٌ عشريّ ⇒ اطوِ صراحةً بدلالة الإشباع
                    //      (foldDoubleToI64Saturating) — قراءة intValue من ثابت Float كانت
                    //      تعامل بِتّات double كعدد صحيح (وافق(6.0، 3) طُويت إلى 0)،
                    //      والامتناع عن الطيّ (إصلاح م-1 الأوّل) دفع الثابت الضخم لمطويّة
                    //      LLVM حيث FPToSI خارج المدى poison (وافق(1e30، -1) اعتباطيّة).
                    // (EN) Amelia (M-1+M-6): a float operand ⇒ fold explicitly with
                    //      saturation semantics — reading intValue off a Float constant
                    //      treated raw double bits as an integer, and refusing to fold
                    //      (the first M-1 fix) pushed huge constants into LLVM's folder
                    //      where out-of-range FPToSI is poison (arbitrary values).
                    const int64_t lhsI = lhsIsFloat ? foldDoubleToI64Saturating(lhs.floatValue) : lhs.intValue;
                    const int64_t rhsI = rhsIsFloat ? foldDoubleToI64Saturating(rhs.floatValue) : rhs.intValue;
                    bool isBoolOp = (lhs.dataType == SadTypeKind::Boolean && rhs.dataType == SadTypeKind::Boolean);
                    if (isBoolOp)
                        constants[resultName] = SIROperand::ConstantBool((lhsI & rhsI) != 0);
                    else
                        constants[resultName] = SIROperand::ConstantI64(lhsI & rhsI);
                    return true;
                }
                case SIROpcode::OR:
                {
                    // (AR) انظر تعليل AND أعلاه (م-1+م-6) / (EN) see AND rationale above (M-1+M-6)
                    const int64_t lhsI = lhsIsFloat ? foldDoubleToI64Saturating(lhs.floatValue) : lhs.intValue;
                    const int64_t rhsI = rhsIsFloat ? foldDoubleToI64Saturating(rhs.floatValue) : rhs.intValue;
                    bool isBoolOp = (lhs.dataType == SadTypeKind::Boolean && rhs.dataType == SadTypeKind::Boolean);
                    if (isBoolOp)
                        constants[resultName] = SIROperand::ConstantBool((lhsI | rhsI) != 0);
                    else
                        constants[resultName] = SIROperand::ConstantI64(lhsI | rhsI);
                    return true;
                }
                case SIROpcode::XOR:
                {
                    // (AR) انظر تعليل AND أعلاه (م-1+م-6) / (EN) see AND rationale above (M-1+M-6)
                    const int64_t lhsI = lhsIsFloat ? foldDoubleToI64Saturating(lhs.floatValue) : lhs.intValue;
                    const int64_t rhsI = rhsIsFloat ? foldDoubleToI64Saturating(rhs.floatValue) : rhs.intValue;
                    constants[resultName] = SIROperand::ConstantI64(lhsI ^ rhsI);
                    return true;
                }
                case SIROpcode::SHL:
                {
                    // (AR) العشريّ يُحوَّل بدلالة الإشباع (م-6)؛ وعدّادٌ خارج [0، 63]
                    //      لا يُطوى — التقنيع الزمنيّ (&63) هو صاحب الدلالة، والطيّ
                    //      بعدّاد خارجها UB في C++‎ (مسار المدمجات يطوي قناعه أوّلًا).
                    // (EN) Floats convert with saturation semantics (M-6); a count outside
                    //      [0, 63] is not folded — the runtime mask (&63) owns the
                    //      semantics and folding it is C++ UB (the builtin path folds
                    //      its mask first anyway).
                    const int64_t lhsI = lhsIsFloat ? foldDoubleToI64Saturating(lhs.floatValue) : lhs.intValue;
                    const int64_t rhsI = rhsIsFloat ? foldDoubleToI64Saturating(rhs.floatValue) : rhs.intValue;
                    if (rhsI < 0 || rhsI > 63)
                        return false;
                    constants[resultName] = SIROperand::ConstantI64(lhsI << rhsI);
                    return true;
                }
                case SIROpcode::SHR:
                {
                    // (AR) انظر تعليل SHL أعلاه / (EN) see SHL rationale above
                    const int64_t lhsI = lhsIsFloat ? foldDoubleToI64Saturating(lhs.floatValue) : lhs.intValue;
                    const int64_t rhsI = rhsIsFloat ? foldDoubleToI64Saturating(rhs.floatValue) : rhs.intValue;
                    if (rhsI < 0 || rhsI > 63)
                        return false;
                    // (AR) إزاحة يمنى حسابيّة (int64_t موقَّع) — عامل `>>` في المفسّر (المرجع)
                    //      إزاحةٌ حسابيّة تحفظ الإشارة (-8 >> 1 = -4)، والخلفيّة تُصدر AShr.
                    //      كان cast لـuint64_t يطوي إزاحةً منطقيّة فيباعد الطيَّ عن زمن التشغيل.
                    // (EN) Signed (int64_t) arithmetic right shift — the interpreter (reference)
                    //      `>>` is a sign-preserving arithmetic shift (-8 >> 1 = -4) and the
                    //      backend emits AShr. The uint64_t cast folded a logical shift, diverging
                    //      the fold from runtime.
                    constants[resultName] = SIROperand::ConstantI64(lhsI >> rhsI);
                    return true;
                }

                default:
                    return false;
                }
            }

            bool ConstantFoldingFrontendPass::runOnFunction(SIRFunction &func)
            {
                bool changed = false;
                std::unordered_map<std::string, SIROperand> constants;

                for (auto &block : func.basicBlocks)
                {
                    if (!block)
                        continue;
                    auto &instructions = block->instructions;

                    for (size_t i = 0; i < instructions.size(); ++i)
                    {
                        auto &inst = instructions[i];

                        if (foldInstruction(inst, constants))
                        {
                            // Replace operands in subsequent instructions that use this result
                            const auto &resultName = inst.result->name;
                            auto it = constants.find(resultName);
                            if (it != constants.end())
                            {
                                // Propagate constant to all users
                                for (size_t j = i + 1; j < instructions.size(); ++j)
                                {
                                    for (auto &op : instructions[j].operands)
                                    {
                                        if (op.type == SIROperandType::REGISTER &&
                                            op.name == resultName)
                                        {
                                            op = it->second;
                                        }
                                    }
                                }
                                // Also propagate to subsequent blocks
                                changed = true;
                            }
                        }
                    }
                }
                return changed;
            }

            // ======================================================================
            // ممر 2: حذف الكود الميت / Pass 2: Dead Code Elimination
            // ======================================================================

            std::unordered_set<std::string> DeadCodeEliminationFrontendPass::collectUsedRegisters(
                const SIRFunction &func) const
            {
                std::unordered_set<std::string> used;
                for (const auto &block : func.basicBlocks)
                {
                    if (!block)
                        continue;
                    for (const auto &inst : block->instructions)
                    {
                        for (const auto &op : inst.operands)
                        {
                            if (op.type == SIROperandType::REGISTER)
                            {
                                used.insert(op.name);
                            }
                        }
                    }
                }
                return used;
            }

            // (AR) إعادة تصميم: من whitelist إلى blacklist (قائمة العمليات النقيّة)
            //      النظام القديم كان يسرد كل عملية لها آثار جانبية → return true; default → return false
            //      المشكلة: أي opcode جديد يُنسى إضافته يُحذف بصمت عبر DCE
            //      الحل الجذري: نسرد فقط العمليات النقيّة (بدون آثار جانبية) → return false
            //      أي opcode غير مدرج يُعتبر تلقائياً ذا آثار جانبية → return true (آمن)
            //      هذا يطابق نفس النمط المُطبّق في middle optimizer (dead_code_elimination_pass.cpp)
            //
            // (EN) Redesign: from whitelist to blacklist (pure operations list)
            //      Old system listed every side-effect opcode → return true; default → return false
            //      Problem: any new opcode forgotten gets silently removed by DCE
            //      Root fix: list only PURE ops (no side effects) → return false
            //      Any unlisted opcode is automatically treated as having side effects → return true (safe)
            //      This matches the same pattern applied in the middle optimizer (dead_code_elimination_pass.cpp)
            bool DeadCodeEliminationFrontendPass::hasSideEffects(const SIRInstruction &inst) const
            {
                switch (inst.opcode)
                {
                // ═══════════════════════════════════════════════════════════════
                // (1) عمليات حسابية — نقيّة تماماً (Arithmetic — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::ADD_I64:
                case SIROpcode::SUB_I64:
                case SIROpcode::MUL_I64:
                case SIROpcode::DIV_I64:
                case SIROpcode::FLOOR_DIV_I64:
                case SIROpcode::MOD_I64:
                case SIROpcode::NEG:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_F64:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (2) عمليات بتّية — نقيّة (Bitwise — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::AND:
                case SIROpcode::OR:
                case SIROpcode::XOR:
                case SIROpcode::NOT:
                case SIROpcode::SHL:
                case SIROpcode::SHR:
                case SIROpcode::SAR:
                case SIROpcode::ROL:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (3) عمليات المقارنة — نقيّة (Comparisons — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::EQ:
                case SIROpcode::NE:
                case SIROpcode::LT:
                case SIROpcode::LE:
                case SIROpcode::GT:
                case SIROpcode::GE:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (4) عمليات القراءة من الذاكرة — نقيّة (Memory reads — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::LOAD:
                case SIROpcode::ADDR:
                case SIROpcode::PTR_ADD:
                case SIROpcode::PTR_CAST:
                case SIROpcode::MOVE:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (5) قراءة المصفوفات والـ tuples — نقيّة (Array/tuple reads — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::ARRAY_GET:
                case SIROpcode::ARRAY_LEN:
                case SIROpcode::ARRAY_NEW:
                case SIROpcode::ARRAY_CONCAT:
                case SIROpcode::ARRAY_ZIP:
                case SIROpcode::TUPLE_NEW:
                case SIROpcode::TUPLE_GET:
                case SIROpcode::TUPLE_LEN:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (6) عمليات النصوص — نقيّة (String operations — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::STRING_NEW:
                case SIROpcode::STRING_CONCAT:
                case SIROpcode::STRING_LEN:
                case SIROpcode::STRING_SUBSTR:
                case SIROpcode::STRING_CMP:
                case SIROpcode::STRING_ORD_CMP:
                case SIROpcode::STRING_FIND:
                case SIROpcode::STRING_REPLACE:
                case SIROpcode::STRING_TO_I64:
                case SIROpcode::STRING_TO_F64:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (7) قراءة الكائنات — نقيّة (Object reads — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::OBJECT_GET:
                case SIROpcode::OBJECT_NEW:
                case SIROpcode::INSTANCEOF:
                case SIROpcode::OBJECT_CAST:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (8) قراءة ADT/Enum — نقيّة (ADT/Enum reads — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::ENUM_CONSTRUCT:
                case SIROpcode::ENUM_GET_TAG:
                case SIROpcode::ENUM_GET_PAYLOAD:
                case SIROpcode::ENUM_IS_VARIANT:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (9) تحويلات الأنواع — نقيّة (Type conversions — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::I64_TO_F64:
                case SIROpcode::F64_TO_I64:
                case SIROpcode::F64_TO_I64_SAT:
                case SIROpcode::I64_TO_BOOL:
                case SIROpcode::BOOL_TO_I64:
                case SIROpcode::I64_TO_STRING:
                case SIROpcode::F64_TO_STRING:
                case SIROpcode::BOOL_TO_STRING:
                case SIROpcode::ARRAY_TO_STRING:
                case SIROpcode::TUPLE_TO_STRING:
                case SIROpcode::CAST:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (10) دوال رياضيات مدمجة — نقيّة (Builtin math — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::BUILTIN_SQRT:
                case SIROpcode::BUILTIN_ABS:
                case SIROpcode::BUILTIN_FLOOR:
                case SIROpcode::BUILTIN_CEIL:
                case SIROpcode::BUILTIN_ROUND:
                case SIROpcode::BUILTIN_SIN:
                case SIROpcode::BUILTIN_COS:
                case SIROpcode::BUILTIN_TAN:
                case SIROpcode::BUILTIN_POW:
                case SIROpcode::BUILTIN_MIN:
                case SIROpcode::BUILTIN_MAX:
                case SIROpcode::BUILTIN_SUM:
                case SIROpcode::BUILTIN_LOG10:
                case SIROpcode::BUILTIN_LOG2:
                case SIROpcode::BUILTIN_ASIN:
                case SIROpcode::BUILTIN_ACOS:
                case SIROpcode::BUILTIN_ATAN:
                case SIROpcode::BUILTIN_TRUNC:
                case SIROpcode::BUILTIN_FMOD:
                case SIROpcode::BUILTIN_CLAMP:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (11) دوال نصوص مدمجة — نقيّة (Builtin string funcs — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::BUILTIN_STRING_LENGTH:
                case SIROpcode::BUILTIN_STRING_CHAR_AT:
                case SIROpcode::BUILTIN_STRING_CHAR_FROM_CODE:
                case SIROpcode::BUILTIN_STRING_TO_UPPER:
                case SIROpcode::BUILTIN_STRING_TO_LOWER:
                case SIROpcode::BUILTIN_STRING_FIND:
                case SIROpcode::BUILTIN_STRING_REPLACE:
                case SIROpcode::BUILTIN_STRING_SUBSTRING:
                case SIROpcode::BUILTIN_STRING_TRIM:
                case SIROpcode::BUILTIN_STRING_SPLIT:
                case SIROpcode::BUILTIN_STRING_JOIN:
                case SIROpcode::BUILTIN_STRING_STARTS_WITH:
                case SIROpcode::BUILTIN_STRING_ENDS_WITH:
                case SIROpcode::BUILTIN_STRING_CONTAINS:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (12) عمليات مصفوفات مدمجة للقراءة — نقيّة (Builtin array read-only — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::BUILTIN_ARRAY_SIZE:
                case SIROpcode::BUILTIN_ARRAY_INDEX_OF:
                case SIROpcode::BUILTIN_ARRAY_CONTAINS:
                case SIROpcode::BUILTIN_ARRAY_FIRST:
                case SIROpcode::BUILTIN_ARRAY_LAST:
                case SIROpcode::BUILTIN_ARRAY_SLICE:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (13) فحص الأنواع — نقيّة (Type checks — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::BUILTIN_TYPE_OF:
                case SIROpcode::BUILTIN_IS_INTEGER:
                case SIROpcode::BUILTIN_IS_FLOAT:
                case SIROpcode::BUILTIN_IS_STRING:
                case SIROpcode::BUILTIN_IS_ARRAY:
                case SIROpcode::BUILTIN_TO_BOOL:
                case SIROpcode::BUILTIN_READ:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (14) دوال أمان نقيّة — لا تعدّل الحالة (Pure security — pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::BUILTIN_SECURITY_HASH:
                case SIROpcode::BUILTIN_SECURITY_VERIFY:
                case SIROpcode::BUILTIN_SECURITY_IS_SAFE:
                case SIROpcode::BUILTIN_SECURITY_ENCRYPT:
                case SIROpcode::BUILTIN_SECURITY_DECRYPT:
                case SIROpcode::BUILTIN_SECURITY_BASE64_ENCODE:
                case SIROpcode::BUILTIN_SECURITY_TIMESTAMP:
                case SIROpcode::BUILTIN_SECURITY_SECURE_RANDOM:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (15) عمليات متنوعة نقيّة (Misc pure)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::Sizeof:
                case SIROpcode::ENV_LOAD:
                case SIROpcode::PHI:
                case SIROpcode::Nop:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (16) عمليات FFI نقيّة — قراءة فقط (Pure FFI reads)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::FFI_STRLEN:
                case SIROpcode::FFI_STRCMP:
                case SIROpcode::FFI_ATOI:
                case SIROpcode::FFI_ATOF:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (17) عمليات أندرويد نقيّة — قراءة فقط (Pure Android reads)
                // ═══════════════════════════════════════════════════════════════
                case SIROpcode::ANDROID_STRING_LENGTH:
                case SIROpcode::ANDROID_STRING_COMPARE:
                case SIROpcode::ANDROID_STRING_SUBSTR:
                case SIROpcode::ANDROID_STRING_CONCAT:
                case SIROpcode::ANDROID_ARRAY_GET:
                case SIROpcode::ANDROID_ARRAY_LENGTH:
                case SIROpcode::ANDROID_MAP_GET:
                case SIROpcode::ANDROID_MAP_HAS:
                case SIROpcode::ANDROID_MAP_SIZE:
                    return false;

                // ═══════════════════════════════════════════════════════════════
                // (AR) كل ما لم يُذكر أعلاه → آثار جانبية محتملة (آمن)
                // (EN) Everything not listed above → assumed side effects (safe)
                // ═══════════════════════════════════════════════════════════════
                default:
                    return true;
                }
            }

            bool DeadCodeEliminationFrontendPass::runOnFunction(SIRFunction &func)
            {
                bool changed = false;
                auto used = collectUsedRegisters(func);

                for (auto &block : func.basicBlocks)
                {
                    if (!block)
                        continue;
                    auto &instructions = block->instructions;

                    auto newEnd = std::remove_if(instructions.begin(), instructions.end(),
                                                 [&](const SIRInstruction &inst)
                                                 {
                                                     // Keep instructions with side effects
                                                     if (hasSideEffects(inst))
                                                         return false;
                                                     // Keep instructions without results (labels etc)
                                                     if (!inst.hasResult())
                                                         return false;
                                                     // Remove if result is never used
                                                     if (used.find(inst.result->name) == used.end())
                                                     {
                                                         changed = true;
                                                         return true;
                                                     }
                                                     return false;
                                                 });

                    if (newEnd != instructions.end())
                    {
                        instructions.erase(newEnd, instructions.end());
                    }
                }
                return changed;
            }

            // ======================================================================
            // ممر 3: تضمين الدوال / Pass 3: Function Inlining
            // ======================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
