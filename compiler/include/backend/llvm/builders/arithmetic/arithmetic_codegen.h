/*
 * ============================================================================
 * ArithmeticCodeGen — توليد LLVM IR للحساب/الثنائيات/المقارنات/التحويلات
 * Phase 7 Step 1 — استخراج 25 method + helper من god-class LLVMCodeGen
 * ============================================================================
 */
#ifndef SAD_LLVM_ARITHMETIC_CODEGEN_H
#define SAD_LLVM_ARITHMETIC_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad
{
    namespace LLVM
    {

        class LLVMCodeGen;
        using SIRInstruction = Compiler::SIR::SIRInstruction;
        using SIROperand = Compiler::SIR::SIROperand;

        class ArithmeticCodeGen
        {
            LLVMCodeGen &cg_;

        public:
            explicit ArithmeticCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
            ArithmeticCodeGen(const ArithmeticCodeGen &) = delete;
            ArithmeticCodeGen &operator=(const ArithmeticCodeGen &) = delete;

            // العمليات الحسابية
            llvm::Value *emitAdd(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitSub(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitMul(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitDiv(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitMod(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitNeg(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitNullAssert(std::shared_ptr<SIRInstruction>); ///< تأكيد عدم الفراغ (NS-05)
            // الثنائية + الإزاحة
            llvm::Value *emitAnd(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitOr(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitXor(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitNot(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitShl(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitShr(std::shared_ptr<SIRInstruction>);
            // المقارنات (Eq/Ne/Lt/Le فقط — Gt/Ge في llvm_codegen_memory_control.cpp)
            llvm::Value *emitCmpEq(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitCmpNe(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitCmpLt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitCmpLe(std::shared_ptr<SIRInstruction>);
            // (AR) Phase 7 Step 1.5: Gt/Ge منقولتان الآن
            llvm::Value *emitCmpGt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitCmpGe(std::shared_ptr<SIRInstruction>);
            // التحويلات
            llvm::Value *emitCast(std::shared_ptr<SIRInstruction>);
            // (AR) Phase 7 Step 1.5: 6 LLVM casts منقولة من file_casts.cpp
            llvm::Value *emitBitCast(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitIntToPtr(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitPtrToInt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitTrunc(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitZExt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitSExt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitI64ToF64(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitF64ToI64(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitI64ToBool(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitBoolToI64(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitI64ToString(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitF64ToString(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitBoolToString(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArrayToString(std::shared_ptr<SIRInstruction>);
            // المساعد
            llvm::Value *resolveOperand(const SIROperand &operand);

            /// (AR) حوّل معاملًا إلى double لعمليّة عشريّة (FADD/FCMP…). إن كان المعامل ديناميًّا
            ///      (Any) وقيمته i64 موسومة (حمولة ADT)، يفكّ التعليب زمنَ التشغيل: 01 صندوق عشريّ
            ///      ⇒ حمّل الـdouble؛ وإلّا فكّ وسم bit63 ثمّ SIToFP. غير-Any ⇒ SIToFP كالسابق
            ///      (لا انحدار). ISSUE-076/084.
            /// (EN) Coerce an operand to double for a float op (FADD/FCMP…). If the operand is
            ///      dynamic (Any) and its i64 value is a tagged ADT payload, unbox at runtime:
            ///      01 boxed float ⇒ load the double; else clear the bit63 int-tag then SIToFP.
            ///      Non-Any ⇒ plain SIToFP as before (no regression). ISSUE-076/084.
            llvm::Value *coerceFloatOperandToDouble(const SIROperand &op, llvm::Value *v);

            /// (AR) عمليّة حسابيّة ثنائيّة **ديناميّة** (نتيجتها Any): معامِلٌ واحدٌ على الأقلّ
            ///      حمولةُ ADT مجهولةُ النوع سكونيًّا. نفحص وسم كلّ معامل زمنَ التشغيل: إن كان
            ///      أيٌّ منهما صندوقًا عشريًّا (01) ⇒ عمليّة عشريّة والنتيجة صندوقٌ عشريّ (01)؛
            ///      وإلّا عمليّة صحيحة والنتيجة موسومةٌ صحيحًا (10). فتُطابق نص()/اطبع المفسّرَ
            ///      لكلا النوعين (5.5 مقابل 44) دون تثبيت نوعٍ ساكن. ISSUE-076/084.
            /// (EN) **Dynamic** binary arithmetic (Any result): at least one operand is a
            ///      statically-unknown ADT payload. Inspect each operand's tag at runtime: if
            ///      either is a boxed float (01) ⇒ float op and the result is a boxed float (01);
            ///      else an integer op and the result is int-tagged (10). So نص()/print match the
            ///      interpreter for both kinds (5.5 vs 44) without pinning a static type. ISSUE-076/084.
            llvm::Value *emitDynamicNumericBinOp(std::shared_ptr<SIRInstruction> inst);

            /// (AR) فكّ وسم معامل Any i64 لمقارنة صحيحة/منطقيّة (== / !=): يمسح البتّتين 63،62
            ///      فتُقارَن القيمة الصحيحة (10) أو المنطقيّة (11) الحقيقيّة. لا يمسّ إلّا Any i64
            ///      (الصندوق العشريّ 01 يُفكَّ في مسار coerce). يُعدِّل v في مكانه. ISSUE-076/082/084.
            /// (EN) Untag an Any i64 operand for an integer/bool compare (== / !=): clears bits
            ///      63,62 so the real int (10) or bool (11) value is compared. Touches only an Any
            ///      i64 (a boxed float 01 is unboxed in the coerce path). Mutates v in place.
            void untagAnyIntCompareOperand(const SIROperand &op, llvm::Value *&v);

            /// (AR) مقارنة == / != واعيةٌ بالوسم لمعامِلٍ ديناميّ Any: تفحص زمنَ التشغيل هل أيٌّ
            ///      من الطرفين صندوقٌ عشريّ (01)؛ إن كان ⇒ تفكّ تعليب الطرفين وتقارن FCMP؛ وإلّا
            ///      تفكّ وسم الصحيح/المنطقيّ (10/11) وتقارن ICMP. تُصلح == بين صندوقين عشريّين
            ///      مختلفين (كانت تقارن مؤشّري الصندوق). ISSUE-076/082/084 (Amelia #9).
            /// (EN) Tag-aware == / != for a dynamic Any operand: at runtime, is either side a boxed
            ///      float (01)? If so ⇒ unbox both and FCMP; else untag the int/bool (10/11) and
            ///      ICMP. Fixes == between two distinct boxed floats (which compared box pointers).
            ///      ISSUE-076/082/084 (Amelia #9). Returns the i1 result.
            llvm::Value *emitDynamicEqNe(const SIROperand &lop, const SIROperand &rop,
                                         llvm::Value *left, llvm::Value *right, bool isEq);
        };

    }
} // namespace Sad::LLVM
#endif
