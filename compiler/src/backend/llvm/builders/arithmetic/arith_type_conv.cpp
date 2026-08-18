/*
 * ============================================================================
 * LLVM IR Code Generator - Type Conversion Instructions
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "sir_constants.h" // (AR) kSadNullSentinel — حارسُ العدمِ في نص()
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>
#include "builders/arithmetic/arithmetic_codegen.h" // (Phase 7 Step 1)
#include "llvm_codegen.h"
#include "sir_constants.h"
#include "adt_payload_tags.h"
#include "sad_dyn_repr.h"

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        // (AR) نوعُ بنيةِ SadArray — يُفوَّض إلى التعريفِ الوحيدِ في sad_dyn_repr (كان مكرَّرًا
        //      في خمسةِ ملفّاتٍ فانفتح بابُ الانجراف؛ التخطيطُ عقدٌ بين مُنتِجِ المصفوفةِ ومستهلكِها).
        // (EN) SadArray struct type — delegated to the single definition in sad_dyn_repr (it used
        //      to be duplicated in five files, an open door to drift; the layout is a contract).
        static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
        {
            return sadArrayStructType(ctx);
        }

        // ============================================================================
        // Phase N: Type Conversion Instructions / تعليمات تحويل الأنواع
        // ============================================================================

        llvm::Value *ArithmeticCodeGen::emitI64ToF64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "I64_TO_F64"}});
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            llvm::Value *result;
            if (val->getType()->isDoubleTy())
            {
                result = val; // Already double
            }
            // (AR) ISSUE-063: قيمةٌ ديناميّة (%SadDyn) أو معامل Any (حمولة موسومة) ⇒ فكّ
            //      عبر coerceFloatOperandToDouble (عشريّ⇒bitcast/فكّ صندوق، صحيح⇒sitofp)
            //      بدل sitofp الأعمى الذي يقرأ بِتّات double كعددٍ صحيح فيُنتج قمامة.
            // (EN) ISSUE-063: a dynamic (%SadDyn) value or an Any operand (tagged payload)
            //      ⇒ decode via coerceFloatOperandToDouble (float⇒bitcast/unbox, int⇒sitofp)
            //      instead of a blind sitofp that reads raw double bits as an integer.
            else if (isSadDyn(val) || inst->operands[0].dataType == SadTypeKind::Any)
            {
                result = coerceFloatOperandToDouble(inst->operands[0], val);
            }
            else if (val->getType()->isIntegerTy(1))
            {
                // bool → i64 → f64
                llvm::Value *ext = cg_.builder_->CreateZExt(val, cg_.getInt64Type(), "bool2i64");
                result = cg_.builder_->CreateSIToFP(ext, cg_.getDoubleType(), "i64tof64");
            }
            else
            {
                result = cg_.builder_->CreateSIToFP(val, cg_.getDoubleType(), "i64tof64");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArithmeticCodeGen::emitF64ToI64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "F64_TO_I64"}});
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            llvm::Value *result;
            // (AR) ISSUE-063: قيمةٌ ديناميّة (%SadDyn) ⇒ فكّ إلى double (unpackDouble عبر
            //      coerceFloatOperandToDouble) ثمّ FPToSI — كان FPToSI يُطبَّق على هيكل
            //      %SadDyn مباشرة ⇒ IR فاسد (verifyModule).
            // (EN) ISSUE-063: a dynamic (%SadDyn) value ⇒ decode to double (unpackDouble via
            //      coerceFloatOperandToDouble) then FPToSI — FPToSI used to be applied to the
            //      raw %SadDyn struct ⇒ invalid IR (verifyModule).
            if (isSadDyn(val))
            {
                llvm::Value *asDbl = coerceFloatOperandToDouble(inst->operands[0], val);
                result = cg_.builder_->CreateFPToSI(asDbl, cg_.getInt64Type(), "f64toi64");
            }
            else if (val->getType()->isIntegerTy())
            {
                result = val; // Already integer
            }
            else
            {
                result = cg_.builder_->CreateFPToSI(val, cg_.getInt64Type(), "f64toi64");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArithmeticCodeGen::emitI64ToBool(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "I64_TO_BOOL"}});
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            llvm::Value *result;
            if (val->getType()->isIntegerTy(1))
            {
                result = val; // Already bool
            }
            else if (val->getType()->isDoubleTy())
            {
                result = cg_.builder_->CreateFCmpONE(val,
                                                 llvm::ConstantFP::get(cg_.getDoubleType(), 0.0), "f64tobool");
            }
            else
            {
                result = cg_.builder_->CreateICmpNE(val,
                                                llvm::ConstantInt::get(val->getType(), 0), "i64tobool");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArithmeticCodeGen::emitBoolToI64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "BOOL_TO_I64"}});
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            llvm::Value *result;
            if (val->getType()->isIntegerTy(64))
            {
                result = val;
            }
            else if (val->getType()->isIntegerTy(1))
            {
                result = cg_.builder_->CreateZExt(val, cg_.getInt64Type(), "booltoi64");
            }
            else
            {
                result = cg_.builder_->CreateZExt(val, cg_.getInt64Type(), "exttoi64");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArithmeticCodeGen::coerceFloatOperandToDouble(const SIROperand &op, llvm::Value *v)
        {
            auto *dblTy = cg_.builder_->getDoubleTy();
            if (!v || v->getType()->isDoubleTy())
                return v;
            // (AR) ISSUE-076: قيمة %SadDyn ⇒ استخرج double (Float⇒bitcast، غيره⇒sitofp) — بلا فروع.
            // (EN) ISSUE-076: a %SadDyn value ⇒ extract a double (Float⇒bitcast, else⇒sitofp) — branchless.
            if (isSadDyn(v))
                return unpackDouble(cg_, v);
            if (!v->getType()->isIntegerTy())
                return v;

            // (AR) معامل غير-Any: عدد صحيح صريح ⇒ SIToFP الرقميّ (سلوكٌ سابقٌ بلا انحدار).
            // (EN) Non-Any operand: an explicit integer ⇒ numeric SIToFP (legacy, no regression).
            if (op.dataType != SadTypeKind::Any)
                return cg_.builder_->CreateSIToFP(v, dblTy, "i64tof64");

            // (AR) ISSUE-076/084 (ب″): معامل Any = حمولة ADT موسومة (إحالة أماميّة/تعارُض).
            //      فرعٌ زمنَ التشغيل على الوسم: 01 صندوق عشريّ ⇒ امسح bit62 ⇒ inttoptr ⇒ حمّل
            //      double (فكّ التعليب)؛ غير ذلك (10 صحيح/00) ⇒ امسح bit63 ثمّ SIToFP. هكذا
            //      يعمل `س + ك`/`س == ك` على حمولةٍ عشريّةٍ عابرةٍ للدوال دون قمامة sitofp.
            // (EN) ISSUE-076/084 (ب″): an Any operand is a tagged ADT payload (forward-ref/
            //      conflict). Runtime branch on the tag: 01 boxed float ⇒ clear bit62 ⇒ inttoptr ⇒
            //      load double (unbox); else (10 int/00) ⇒ clear bit63 then SIToFP. So `s + k` /
            //      `s == k` on a cross-function float payload works without sitofp garbage.
            auto *i64Ty = cg_.getInt64Type();
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::Value *bit63 = cg_.builder_->CreateAnd(
                v, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit63), "coerce.b63");
            llvm::Value *bit62 = cg_.builder_->CreateAnd(
                v, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit62), "coerce.b62");
            llvm::Value *hiClear = cg_.builder_->CreateICmpEQ(
                bit63, llvm::ConstantInt::get(i64Ty, 0), "coerce.hiclear");
            llvm::Value *loSet = cg_.builder_->CreateICmpNE(
                bit62, llvm::ConstantInt::get(i64Ty, 0), "coerce.loset");
            llvm::Value *isFloatBox = cg_.builder_->CreateAnd(hiClear, loSet, "coerce.isfloat");

            auto *parent = cg_.builder_->GetInsertBlock()->getParent();
            auto *floatBB = llvm::BasicBlock::Create(*cg_.context_, "coerce.float", parent);
            auto *intBB = llvm::BasicBlock::Create(*cg_.context_, "coerce.int", parent);
            auto *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "coerce.merge", parent);
            cg_.builder_->CreateCondBr(isFloatBox, floatBB, intBB);

            cg_.builder_->SetInsertPoint(floatBB);
            llvm::Value *boxI64 = cg_.builder_->CreateAnd(
                v, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadBit62), "coerce.fclear");
            llvm::Value *boxPtr = cg_.builder_->CreateIntToPtr(boxI64, ptrTy, "coerce.fptr");
            llvm::Value *fdbl = cg_.builder_->CreateLoad(dblTy, boxPtr, "coerce.fload");
            cg_.builder_->CreateBr(mergeBB);

            cg_.builder_->SetInsertPoint(intBB);
            llvm::Value *cleanInt = cg_.builder_->CreateAnd(
                v, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadBit63), "coerce.iclean");
            llvm::Value *idbl = cg_.builder_->CreateSIToFP(cleanInt, dblTy, "coerce.i2f");
            cg_.builder_->CreateBr(mergeBB);

            cg_.builder_->SetInsertPoint(mergeBB);
            auto *phi = cg_.builder_->CreatePHI(dblTy, 2, "coerce.dbl");
            phi->addIncoming(fdbl, floatBB);
            phi->addIncoming(idbl, intBB);
            return phi;
        }

        void ArithmeticCodeGen::untagAnyIntCompareOperand(const SIROperand &op, llvm::Value *&v)
        {
            // (AR) معامل Any i64 (لم يُفكّ عشريًّا) ⇒ حمولةٌ موسومة صحيح(10)/منطقيّ(11)؛ امسح
            //      البتّتين 63،62 لاستعادة القيمة الحقيقيّة قبل المقارنة الصحيحة. ISSUE-076/082/084.
            // (EN) An Any i64 operand (not float-unboxed) ⇒ a tagged int(10)/bool(11) payload; clear
            //      bits 63,62 to recover the real value before the integer compare. ISSUE-076/082/084.
            if (op.dataType == SadTypeKind::Any && v && v->getType()->isIntegerTy(64))
            {
                v = cg_.builder_->CreateAnd(
                    v, llvm::ConstantInt::get(cg_.getInt64Type(), ~kAdtPayloadTagMask), "cmp.any.untag");
            }
        }

        llvm::Value *ArithmeticCodeGen::emitDynamicEqNe(const SIROperand &lop, const SIROperand &rop,
                                                        llvm::Value *left, llvm::Value *right, bool isEq)
        {
            auto *i64Ty = cg_.getInt64Type();

            // (AR) ISSUE-076 (حلّ %SadDyn): أيّ طرفٍ %SadDyn ⇒ وحّد الطرفين إلى %SadDyn وقارِن
            //      عبر الموزِّع (عشريّ⇒fcmp، صحيح⇒icmp). يُغلق == بين صندوقين عشريّين بنيويًّا.
            // (EN) ISSUE-076 (%SadDyn): either side %SadDyn ⇒ normalize both to %SadDyn and compare
            //      via the dispatcher (float⇒fcmp, int⇒icmp). Structurally closes == between boxes.
            if (isSadDyn(left) || isSadDyn(right))
            {
                llvm::Value *l = toDyn(cg_, left, lop.dataType);
                llvm::Value *r = toDyn(cg_, right, rop.dataType);
                return dynCompare(cg_, isEq ? DynCmp::EQ : DynCmp::NE, l, r);
            }

            bool lAny = (lop.dataType == SadTypeKind::Any) && left->getType()->isIntegerTy(64);
            bool rAny = (rop.dataType == SadTypeKind::Any) && right->getType()->isIntegerTy(64);

            // (AR) لا معامل Any i64 ⇒ المسار الساكن المعتاد (double⇒FCMP، غيره⇒ICMP).
            // (EN) No Any i64 operand ⇒ the usual static path (double⇒FCMP, else⇒ICMP).
            if (!lAny && !rAny)
            {
                if (left->getType()->isDoubleTy() && right->getType()->isDoubleTy())
                    return isEq ? cg_.builder_->CreateFCmpOEQ(left, right, "cmpeqtmp")
                                : cg_.builder_->CreateFCmpONE(left, right, "cmpnetmp");
                return isEq ? cg_.builder_->CreateICmpEQ(left, right, "cmpeqtmp")
                            : cg_.builder_->CreateICmpNE(left, right, "cmpnetmp");
            }

            // (AR) معامل Any i64 ⇒ فرعٌ زمنَ التشغيل: أيّ طرفٍ صندوقٌ عشريّ (01 = bit63=0 و bit62=1)؟
            // (EN) An Any i64 operand ⇒ runtime branch: is either side a boxed float (01)?
            auto isFloatBox = [&](llvm::Value *v) -> llvm::Value *
            {
                llvm::Value *b63 = cg_.builder_->CreateAnd(
                    v, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit63), "eq.b63");
                llvm::Value *b62 = cg_.builder_->CreateAnd(
                    v, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit62), "eq.b62");
                return cg_.builder_->CreateAnd(
                    cg_.builder_->CreateICmpEQ(b63, llvm::ConstantInt::get(i64Ty, 0), "eq.hiclear"),
                    cg_.builder_->CreateICmpNE(b62, llvm::ConstantInt::get(i64Ty, 0), "eq.loset"),
                    "eq.isfloat");
            };
            llvm::Value *lFB = lAny ? isFloatBox(left) : llvm::ConstantInt::getFalse(*cg_.context_);
            llvm::Value *rFB = rAny ? isFloatBox(right) : llvm::ConstantInt::getFalse(*cg_.context_);
            llvm::Value *eitherFB = cg_.builder_->CreateOr(lFB, rFB, "eq.either.float");

            auto *parent = cg_.builder_->GetInsertBlock()->getParent();
            auto *floatBB = llvm::BasicBlock::Create(*cg_.context_, "eq.float", parent);
            auto *intBB = llvm::BasicBlock::Create(*cg_.context_, "eq.int", parent);
            auto *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "eq.merge", parent);
            cg_.builder_->CreateCondBr(eitherFB, floatBB, intBB);

            // (AR) فرع العشريّ: فكّ تعليب الطرفين (coerce يفكّ الصندوق أو يُرقّي الصحيح) ثمّ FCMP.
            // (EN) Float branch: unbox both (coerce unboxes a box or promotes an int) then FCMP.
            cg_.builder_->SetInsertPoint(floatBB);
            llvm::Value *lD = coerceFloatOperandToDouble(lop, left);
            llvm::Value *rD = coerceFloatOperandToDouble(rop, right);
            llvm::Value *fr = isEq ? cg_.builder_->CreateFCmpOEQ(lD, rD, "eq.fcmp")
                                   : cg_.builder_->CreateFCmpONE(lD, rD, "eq.fcmp");
            cg_.builder_->CreateBr(mergeBB);
            floatBB = cg_.builder_->GetInsertBlock();

            // (AR) فرع الصحيح/المنطقيّ: فكّ وسم معامل Any (مسح البتّتين) ثمّ ICMP.
            // (EN) Int/bool branch: untag the Any operand (clear both bits) then ICMP.
            cg_.builder_->SetInsertPoint(intBB);
            llvm::Value *lI = left;
            llvm::Value *rI = right;
            if (lAny)
                lI = cg_.builder_->CreateAnd(left, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadTagMask), "eq.l.untag");
            if (rAny)
                rI = cg_.builder_->CreateAnd(right, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadTagMask), "eq.r.untag");
            llvm::Value *ir = isEq ? cg_.builder_->CreateICmpEQ(lI, rI, "eq.icmp")
                                   : cg_.builder_->CreateICmpNE(lI, rI, "eq.icmp");
            cg_.builder_->CreateBr(mergeBB);
            intBB = cg_.builder_->GetInsertBlock();

            cg_.builder_->SetInsertPoint(mergeBB);
            auto *phi = cg_.builder_->CreatePHI(cg_.getInt1Type(), 2, "eq.result");
            phi->addIncoming(fr, floatBB);
            phi->addIncoming(ir, intBB);
            return phi;
        }

        llvm::Value *ArithmeticCodeGen::emitDynamicNumericBinOp(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) ISSUE-076 (حلّ %SadDyn الجذريّ): عمليّةٌ ثنائيّة نتيجتها ديناميّة (Any) ⇒
            //      وحّد المعاملين إلى %SadDyn (تغليفٌ للمحسوس، تمريرٌ للـ%SadDyn) ثمّ فوّض إلى
            //      موزِّع dynBinOp الذي يفحص وسم النوع زمنَ التشغيل: عشريّ⇒fadd/… والنتيجة Float؛
            //      صحيح⇒add/… والنتيجة Int؛ %///⇒صحيح دائمًا. **بلا malloc** (insertvalue فقط).
            //      يزول تعليبُ الصندوق وتسريبُه ونظامُ الأعلام الجانبيّة (النوع %SadDyn هو المعلومة).
            // (EN) ISSUE-076 (%SadDyn root fix): a binary op whose result is dynamic (Any) ⇒
            //      normalize both operands to %SadDyn (pack concretes, pass %SadDyn through) then
            //      delegate to dynBinOp, which inspects the runtime kind tag: float⇒fadd/… with a
            //      Float result; int⇒add/… with an Int result; %///⇒always integer. **No malloc**
            //      (insertvalue only). The heap box, its leak, and the side flags are gone (the
            //      %SadDyn type *is* the information).
            llvm::Value *lv = resolveOperand(inst->operands[0]);
            llvm::Value *rv = resolveOperand(inst->operands[1]);
            if (!lv || !rv)
                return nullptr;

            llvm::Value *l = toDyn(cg_, lv, inst->operands[0].dataType);
            llvm::Value *r = toDyn(cg_, rv, inst->operands[1].dataType);
            llvm::Value *result = dynBinOp(cg_, inst->opcode, l, r);

            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *ArithmeticCodeGen::emitI64ToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "I64_TO_STRING"}});
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // (AR) [طبقة طبيعي64 — الخطوة ٤] طباعة لا-موقَّعة: نمط بتّات i64 لقيمة طبيعي64
            //      يُنسَّق %llu لا %lld (يُطابق المفسّر renderUnsignedArgs). النوع من المعامل
            //      الساكن (dataType). Byte دائمًا [0،255] فطباعته الموقَّعة صحيحة. الوضع الحرّ
            //      (__sad_itoa) يبقى موقَّعًا مؤقّتًا (لا __sad_utoa بعد) — مؤجَّل موثَّق.
            // (EN) [طبيعي64 layer — Step 4] Unsigned printing: طبيعي64's i64 bit pattern formats
            //      with %llu not %lld (mirrors the interpreter's renderUnsignedArgs). Type from the
            //      operand's static dataType. Byte is always [0,255] so its signed print is fine.
            //      Freestanding (__sad_itoa) stays signed for now (no __sad_utoa yet) — documented.
            const bool i64ToStrUnsigned =
                (inst->operands[0].dataType == SadTypeKind::UInt64);

            // (AR) ISSUE-076 (حلّ %SadDyn الجذريّ): نص(%SadDyn) ⇒ موزِّع dynToString الذي يفحص
            //      وسم النوع ويطابق المفسّر لكلّ نوع (صحيح/عشريّ/منطقيّ/نصّ/عدم). النوع %SadDyn هو
            //      المعلومة — لا فكّ بتّاتٍ يدويّ.
            // (EN) ISSUE-076 (%SadDyn root fix): نص(%SadDyn) ⇒ the dynToString dispatcher, which
            //      inspects the kind tag and matches the interpreter per type (int/float/bool/str/
            //      null). The %SadDyn type is the information — no manual bit decode.
            if (isSadDyn(val))
            {
                llvm::Value *s = dynToString(cg_, val);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = s;
                return s;
            }

            // ================================================================
            // (AR) الخريطة: `نص(خريطة)` كان يُلوَّن هنا على **مؤشّرِ الخريطة** فيُنسَّق
            //   `%lld` ⇒ يُطبَعُ عنوانٌ خام: `نص({"أ": 1})` تُعطي «1825978152560» بينما
            //   المفسّرُ يعطي «{أ: 1}». عائلةُ ISSUE-080 نفسُها التي أُغلِقت للمصفوفاتِ
            //   بـARRAY_TO_STRING ولم تبلغِ الخرائط. والصيغةُ هنا **غيرُ المقتبسة**
            //   (نظيرُ `toString` في المفسّر) لا صيغةُ الطباعةِ المقتبسة — للمفسّرِ
            //   صيغتان مقيستان: `اطبع(خريطة)` ⇒ «{"أ": 1}» و`نص(خريطة)` ⇒ «{أ: 1}».
            //   والمدى **يُقالُ ولا يُوسَّع**: الشرطُ على النوعِ **الساكن**، فخريطةٌ تعبرُ
            //   `أي` لا تُلتقَطُ هنا وتبقى على المسارِ العدديّ — عائلةٌ قائمةٌ يشترك فيها
            //   `نص(أي = مصفوفة)` أيضًا، لا يُغلِقُها هذا الفرعُ ولا يدّعي إغلاقَها.
            //   وأمّا المخزن: يهربُ إلى البرنامجِ (قيمةُ `نص()` نفسُها) فلا يُحرَّرُ هنا —
            //   وهذا نموذجُ الملكيّةِ القائمُ لكلِّ تحويلاتِ النصِّ لا استثناءٌ لها. وقياسُ
            //   «المصفوفةُ لا تُسرّب» **مضلّل**: مساعِدُها يُدمَجُ فيُرقَّى تخصيصُه إلى مكدّس،
            //   ومساعِدُ الخريطةِ أكبرُ من أن يُدمَجَ فيظهرُ الأثرُ. الفارقُ في التحسينِ لا
            //   في الملكيّة، والتحريرُ العامُّ يقتضي تمريرةَ ملكيّةٍ لا رقعةً هنا.
            // (EN) Map: `نص(map)` used to lower onto the map POINTER and format it %lld,
            //   printing a raw address. Same ISSUE-080 family closed for arrays but never
            //   reaching maps. The UNQUOTED spelling is used here (the interpreter's
            //   toString), not the quoted print spelling — it has two, measured.
            // ================================================================
            if (inst->operands[0].dataType == SadTypeKind::Map)
            {
                auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                llvm::Value *mapPtr = val;
                if (mapPtr->getType()->isIntegerTy())
                    mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "m2s.i2p");
                if (mapPtr->getType()->isPointerTy())
                {
                    // (AR) والعدمُ وجهان هنا أيضًا: مساعِدُ الخريطةِ يحرس المؤشّرَ الصفريَّ
                    //      وحدَه، و`خريطة عدمية س = لاشيء` تُخزِّن الرمزَ فيمرُّ وينهار.
                    //      فيُبدَّل الرمزُ صفرًا قبل النداءِ ليبلغ الحارسَ القائم.
                    // (EN) Null has two shapes for maps too: the helper guards only the zero
                    //      pointer, so `خريطة عدمية س = لاشيء` (which stores the sentinel)
                    //      slipped through. Normalise the sentinel to null before the call so
                    //      the existing guard sees it.
                    // (AR) البابُ الواحدُ بذراعِ «لا نائب» — المساعِدُ يحرس الصفرَ سلفًا.
                    // (EN) Single door, "no placeholder" arm — the helper guards zero.
                    mapPtr = cg_.emitContainerNullGuard(
                                    mapPtr, /*containerStructTy=*/nullptr,
                                    /*placeholderName=*/nullptr, "m2s")
                                 .safePtr;
                    cg_.ensureMapToStringHelper(/*quoteKeys=*/false);
                    llvm::FunctionCallee mapHelper = cg_.module_->getOrInsertFunction(
                        ::Sad::Compiler::kMapToStringPlainFn, llvm::FunctionType::get(ptrTy, {ptrTy}, false));
                    llvm::Value *mapText = cg_.builder_->CreateCall(mapHelper, {mapPtr}, "m2s.result");
                    if (inst->result.has_value())
                        cg_.context_info_.namedValues[inst->result->name] = mapText;
                    return mapText;
                }
            }

            // ================================================================
            // (AR) ISSUE-076/082/084 (ب″): معامل ديناميّ Any = حمولة ADT موسومة.
            //      نص(Any) يُلوَّن I64_TO_STRING (لا مسار سلسلة ديناميّ)، فنفكّ الوسم رباعيًّا
            //      هنا زمنَ التشغيل: 00 نصّ · 01 صندوق عشريّ · 10 صحيح · 11 منطقيّ. يعبر حدود
            //      الدوال/الإرجاع حيث تضيع الأعلام. القيمة غير-Any (عدد صحيح صريح) لا تُفكّ.
            // (EN) ISSUE-076/082/084 (ب″): a dynamic Any operand = a tagged ADT payload.
            //      نص(Any) lowers to I64_TO_STRING (no dynamic string path), so decode the tag
            //      4-way at runtime here: 00 string · 01 boxed float · 10 int · 11 bool. Works
            //      across function/return boundaries where flags are lost. A non-Any (explicit
            //      integer) operand is never decoded.
            // ================================================================
            if (inst->operands[0].dataType == SadTypeKind::Any &&
                val->getType()->isIntegerTy(64) && !cg_.freestanding_)
            {
                auto *i64Ty = cg_.getInt64Type();
                auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                auto *dblTy = llvm::Type::getDoubleTy(*cg_.context_);

                // (AR) ISSUE-076 (Amelia #8): 512 لا 64 — فرع العشريّ يكتب __sad_format_double
                //      و%.6f لـDBL_MAX ~316 حرفًا يفيض. (EN) 512 not 64 — the float branch writes
                //      __sad_format_double; %.6f for DBL_MAX ~316 chars overflows 64.
                llvm::Value *dbuf = cg_.emitMalloc(llvm::ConstantInt::get(i64Ty, 512), "any.tostr.buf");

                auto *sprintfTy = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*cg_.context_), {ptrTy, ptrTy}, true);
                auto sprintfFn = cg_.module_->getOrInsertFunction("sprintf", sprintfTy);

                auto *parentFunc = cg_.builder_->GetInsertBlock()->getParent();
                auto *pofBB = llvm::BasicBlock::Create(*cg_.context_, "any.ts.pof", parentFunc);
                auto *ptrBB = llvm::BasicBlock::Create(*cg_.context_, "any.ts.ptr", parentFunc);
                auto *floatBB = llvm::BasicBlock::Create(*cg_.context_, "any.ts.float", parentFunc);
                auto *iobBB = llvm::BasicBlock::Create(*cg_.context_, "any.ts.iob", parentFunc);
                auto *boolBB = llvm::BasicBlock::Create(*cg_.context_, "any.ts.bool", parentFunc);
                auto *intBB = llvm::BasicBlock::Create(*cg_.context_, "any.ts.int", parentFunc);
                auto *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "any.ts.merge", parentFunc);

                // (AR) فحص bit63: مصفّر ⇒ نصّ/عشريّ (00/01)؛ مضبوط ⇒ صحيح/منطقيّ (10/11)
                // (EN) bit63: clear ⇒ string/float (00/01); set ⇒ int/bool (10/11)
                llvm::Value *bit63 = cg_.builder_->CreateAnd(
                    val, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit63), "any.ts.b63");
                llvm::Value *hiClear = cg_.builder_->CreateICmpEQ(
                    bit63, llvm::ConstantInt::get(i64Ty, 0), "any.ts.hiclear");
                cg_.builder_->CreateCondBr(hiClear, pofBB, iobBB);

                // (AR) 00 نصّ مقابل 01 صندوق عشريّ عبر bit62
                cg_.builder_->SetInsertPoint(pofBB);
                llvm::Value *bit62 = cg_.builder_->CreateAnd(
                    val, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit62), "any.ts.b62");
                llvm::Value *isFloat = cg_.builder_->CreateICmpNE(
                    bit62, llvm::ConstantInt::get(i64Ty, 0), "any.ts.isfloat");
                cg_.builder_->CreateCondBr(isFloat, floatBB, ptrBB);

                // (AR) 00 نصّ: inttoptr (مع حماية null ⇒ "void")
                cg_.builder_->SetInsertPoint(ptrBB);
                // (AR) لفظُ «void» مكتوبًا يدًا وفحصُ `nullptr` وحدَه ⇒ لا يرى الحارس.
                //      عبرَ بابِ العرضِ الآن، واللفظُ من مصدرِ الحقيقة كما يطبع المفسّر.
                // (EN) Hand-written "void" with a nullptr-only check missed the sentinel.
                llvm::Value *safeStr = cg_.emitSafeStringPtr(val, "any.ts");
                cg_.builder_->CreateBr(mergeBB);

                // (AR) 01 صندوق عشريّ: امسح bit62 ⇒ مؤشّر ⇒ حمّل double ⇒ __sad_format_double
                cg_.builder_->SetInsertPoint(floatBB);
                llvm::Value *fboxI64 = cg_.builder_->CreateAnd(
                    val, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadBit62), "any.ts.fclear");
                llvm::Value *fboxPtr = cg_.builder_->CreateIntToPtr(fboxI64, ptrTy, "any.ts.fptr");
                llvm::Value *fdbl = cg_.builder_->CreateLoad(dblTy, fboxPtr, "any.ts.fload");
                auto *fmtDblTy = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg_.context_), {ptrTy, dblTy}, false);
                auto fmtDblFn = cg_.module_->getOrInsertFunction("__sad_format_double", fmtDblTy);
                cg_.builder_->CreateCall(fmtDblFn, {dbuf, fdbl});
                cg_.builder_->CreateBr(mergeBB);

                // (AR) bit63 مضبوط: 11 منطقيّ مقابل 10 صحيح عبر bit62
                cg_.builder_->SetInsertPoint(iobBB);
                llvm::Value *bit62b = cg_.builder_->CreateAnd(
                    val, llvm::ConstantInt::get(i64Ty, kAdtPayloadBit62), "any.ts.b62b");
                llvm::Value *isBool = cg_.builder_->CreateICmpNE(
                    bit62b, llvm::ConstantInt::get(i64Ty, 0), "any.ts.isbool");
                cg_.builder_->CreateCondBr(isBool, boolBB, intBB);

                // (AR) 11 منطقيّ: امسح البتّتين ⇒ صحيح/خطأ
                cg_.builder_->SetInsertPoint(boolBB);
                llvm::Value *cleanBool = cg_.builder_->CreateAnd(
                    val, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadTagMask), "any.ts.bclean");
                llvm::Value *boolCond = cg_.builder_->CreateICmpNE(
                    cleanBool, llvm::ConstantInt::get(i64Ty, 0), "any.ts.bcond");
                llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr(
                    "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "any.ts.true"); // صحيح
                llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr(
                    "\xd8\xae\xd8\xb7\xd8\xa3", "any.ts.false"); // خطأ
                llvm::Value *boolStr = cg_.builder_->CreateSelect(boolCond, trueStr, falseStr, "any.ts.bstr");
                cg_.builder_->CreateBr(mergeBB);

                // (AR) 10 صحيح: امسح bit63 ⇒ sprintf %lld
                cg_.builder_->SetInsertPoint(intBB);
                llvm::Value *cleanInt = cg_.builder_->CreateAnd(
                    val, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadBit63), "any.ts.iclean");
                llvm::Value *ifmt = cg_.builder_->CreateGlobalStringPtr("%lld", "any.ts.ifmt");
                cg_.builder_->CreateCall(sprintfFn, {dbuf, ifmt, cleanInt});
                cg_.builder_->CreateBr(mergeBB);

                // (AR) دمج (5 مصادر: نصّ/عشريّ/منطقيّ/صحيح — dbuf مشترك للعشريّ والصحيح)
                cg_.builder_->SetInsertPoint(mergeBB);
                auto *phi = cg_.builder_->CreatePHI(ptrTy, 4, "any.ts.result");
                phi->addIncoming(safeStr, ptrBB);
                phi->addIncoming(dbuf, floatBB);
                phi->addIncoming(boolStr, boolBB);
                phi->addIncoming(dbuf, intBB);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = phi;
                return phi;
            }

            // Allocate buffer: 21 bytes enough for i64 range + sign + null

            llvm::Value *buf = cg_.emitMalloc(llvm::ConstantInt::get(cg_.getInt64Type(), 32), "i64str_buf");

            if (cg_.freestanding_)
            {
                // (AR) في الوضع الحر: استخدم __sad_itoa بدلاً من sprintf المتغيرة الوسائط
                // (EN) Freestanding: use __sad_itoa instead of variadic sprintf
                auto *itoaType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*cg_.context_),
                    {llvm::PointerType::getUnqual(*cg_.context_),
                     llvm::Type::getInt64Ty(*cg_.context_)},
                    false);
                auto itoaFunc = cg_.module_->getOrInsertFunction("__sad_itoa", itoaType);
                cg_.builder_->CreateCall(itoaFunc, {buf, val});
            }
            else
            {
                // sprintf(buf, "%lld", val)
                auto *sprintfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*cg_.context_),
                    {llvm::PointerType::getUnqual(*cg_.context_),
                     llvm::PointerType::getUnqual(*cg_.context_)},
                    true);
                auto sprintfFunc = cg_.module_->getOrInsertFunction("sprintf", sprintfType);

                llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr(
                    i64ToStrUnsigned ? "%llu" : "%lld", "fmt_i64");
                cg_.builder_->CreateCall(sprintfFunc, {buf, fmt, val});
            }

            llvm::Value *i64StrResult = buf;

            // ================================================================
            // (AR) حارسُ kSadNullSentinel — المسارُ الثاني للتحويل إلى نصّ
            //      `اطبع(س)` تُلوَّن على مسارِ السَلسلة (string_ops.cpp) وفيه هذا
            //      الحارسُ منذ زمن، بينما `نص(س)` تُلوَّن I64_TO_STRING وهنا **لم
            //      يكن** — فطبعت الرقعةُ نفسُها «لاشيء» من طريقٍ و
            //      «‎-9223372036854775807» من طريقٍ آخرَ في البرنامجِ الواحد.
            //      والاستثناءُ منقولٌ حرفًا عن التوأم: طبيعي64/بايت لا يكونان نوعَ
            //      العدمِ أبدًا (العدمُ يُخزَّن i64 نوعُه Integer)، فقيمةٌ شرعيّةٌ
            //      تساوي الرمزَ كانت ستُحوَّل «لاشيء» خطأً.
            //
            //      🔑 والدرسُ: عَلَمٌ يلزمه **كلُّ** مُستهلِكيه. وقد كُتِب هذا الدرسُ
            //      في هذه الشجرةِ مرّتَين قبل اليوم، ثمّ وُجِد مسارٌ ثالثٌ ينقضه.
            //
            //      ⚠️ وحدُّه: `sprintf` يُنفَّذُ في الحالتَين ثمّ يُنتقى المخرَج،
            //      فلا فرعَ يُتجنَّب — كلفةٌ ثابتةٌ مقبولةٌ مقابل ألّا تُشقَّ الكتلة.
            // (EN) kSadNullSentinel guard — the SECOND to-string path. `print(x)`
            //      lowers through the concat path (string_ops.cpp) which has had this
            //      guard for a long time, while `نص(x)` lowers to I64_TO_STRING which
            //      did NOT — so one program printed «لاشيء» via one route and
            //      «-9223372036854775807» via the other. The exception mirrors the
            //      twin: طبيعي64/Byte are never the null type.
            // ================================================================
            if (!cg_.freestanding_ && val->getType()->isIntegerTy(64) &&
                inst->operands[0].dataType != SadTypeKind::UInt64 &&
                inst->operands[0].dataType != SadTypeKind::Byte)
            {
                auto *sentinelConst = llvm::ConstantInt::get(
                    llvm::Type::getInt64Ty(*cg_.context_), Sad::Compiler::kSadNullSentinel);
                llvm::Value *isSentinel =
                    cg_.builder_->CreateICmpEQ(val, sentinelConst, "i64str.is.null");
                llvm::Value *nullText = cg_.builder_->CreateGlobalStringPtr(
                    "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "i64str.null"); // لاشيء
                i64StrResult =
                    cg_.builder_->CreateSelect(isSentinel, nullText, buf, "i64str.result");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = i64StrResult;
            }
            return i64StrResult;
        }

        llvm::Value *ArithmeticCodeGen::emitF64ToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "F64_TO_STRING"}});
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            if (!val->getType()->isDoubleTy())
            {
                // (AR) ISSUE-076/084: قيمةٌ عشريّة النوع لكنّها وصلت i64 (بِتّات double خام —
                //      مثل عنصر مصفوفة عشريّة يُخزَّن bitcast(double→i64)). التأويل الصحيح
                //      **bitcast** (نفس البِتّات) لا SIToFP (الذي يُفسّر البِتّات عددًا صحيحًا
                //      فيُنتج قمامة ~4.6e18). المصادر غير-i64 (int32…) تبقى على SIToFP الرقميّ.
                // (EN) ISSUE-076/084: a Float-typed value that arrived as i64 (raw double bits —
                //      e.g. a float array element stored as bitcast(double→i64)). The correct
                //      reinterpretation is **bitcast** (same bits), NOT SIToFP (which reads the
                //      bits as an integer → garbage ~4.6e18). Non-i64 sources (int32…) keep the
                //      numeric SIToFP path.
                if (val->getType()->isIntegerTy(64))
                    val = cg_.builder_->CreateBitCast(val, cg_.getDoubleType(), "bitsf64");
                else
                    val = cg_.builder_->CreateSIToFP(val, cg_.getDoubleType(), "tof64");
            }


            // (AR) ISSUE-076 (Amelia #8): 512 لا 64 — __sad_format_double بـ%.6f لـDBL_MAX ~316 حرفًا.
            // (EN) ISSUE-076 (Amelia #8): 512 not 64 — __sad_format_double %.6f for DBL_MAX ~316 chars.
            llvm::Value *buf = cg_.emitMalloc(llvm::ConstantInt::get(cg_.getInt64Type(), 512), "f64str_buf");

            if (cg_.freestanding_)
            {
                // (AR) في الوضع الحر: استخدم __sad_ftoa بدلاً من sprintf المتغيرة الوسائط
                // (EN) Freestanding: use __sad_ftoa instead of variadic sprintf
                auto *ftoaType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*cg_.context_),
                    {llvm::PointerType::getUnqual(*cg_.context_),
                     llvm::Type::getDoubleTy(*cg_.context_)},
                    false);
                auto ftoaFunc = cg_.module_->getOrInsertFunction("__sad_ftoa", ftoaType);
                cg_.builder_->CreateCall(ftoaFunc, {buf, val});
            }
            else
            {
                // (AR) إصلاح: استخدام __sad_format_double بدلاً من sprintf("%g")
                //      لمطابقة دقة المفسر: 6 خانات عشرية + حذف أصفار زائدة
                // (EN) Fix: use __sad_format_double instead of sprintf("%g")
                //      to match interpreter precision: 6 decimal places + strip trailing zeros
                auto *fmtDblType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*cg_.context_),
                    {llvm::PointerType::getUnqual(*cg_.context_),
                     llvm::Type::getDoubleTy(*cg_.context_)},
                    false);
                auto fmtDblFunc = cg_.module_->getOrInsertFunction("__sad_format_double", fmtDblType);
                cg_.builder_->CreateCall(fmtDblFunc, {buf, val});
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }

        llvm::Value *ArithmeticCodeGen::emitBoolToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "BOOL_TO_STRING"}});
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // (AR) إصلاح: إذا كانت القيمة المنطقية ثابتة (نتيجة constant folding)
            //      نُرجع المؤشر للنص مباشرة بدون select — وهذا يمنع
            //      تخزين GlobalVariable في namedValues ثم تحميلها خطأً كـ [N x i8]
            // (EN) Fix: if boolean value is a constant (result of constant folding),
            //      return the string pointer directly without select — this prevents
            //      storing GlobalVariable in namedValues then erroneously loading as [N x i8]
            if (auto *constInt = llvm::dyn_cast<llvm::ConstantInt>(val))
            {
                llvm::Value *result;
                if (constInt->isOne() || (constInt->getBitWidth() > 1 && !constInt->isZero()))
                {
                    result = cg_.builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "true_str");
                }
                else
                {
                    result = cg_.builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "false_str");
                }
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            // Convert to i1 if not already
            if (!val->getType()->isIntegerTy(1))
            {
                val = cg_.builder_->CreateICmpNE(val,
                                             llvm::ConstantInt::get(val->getType(), 0), "tobool");
            }

            // (AR) "صحيح" (true) / "خطأ" (false) بالعربية
            // (EN) "صحيح" (true) / "خطأ" (false) in Arabic
            llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "true_str");
            llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "false_str");
            llvm::Value *result = cg_.builder_->CreateSelect(val, trueStr, falseStr, "boolstr");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // (AR) تحويل مصفوفة إلى نص "[عنصر1, عنصر2, ...]"
        // (EN) Convert array to string "[elem1, elem2, ...]"
        // ============================================================================
        llvm::Value *ArithmeticCodeGen::emitArrayToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "ARRAY_TO_STRING"}});
                return nullptr;
            }
            llvm::Value *arrPtr = resolveOperand(inst->operands[0]);
            if (!arrPtr)
                return nullptr;

            // (AR) معالجة المؤشرات المخزنة في متغيرات i64 — تحميل ثم تحويل لمؤشر
            // (EN) Handle pointers stored in i64 variables — load then convert to pointer
            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(arrPtr))
            {
                if (allocaInst->getAllocatedType()->isIntegerTy(64))
                {
                    llvm::Value *ptrAsInt = cg_.builder_->CreateLoad(cg_.getInt64Type(), allocaInst, "ats.ptr.int");
                    arrPtr = cg_.builder_->CreateIntToPtr(ptrAsInt, llvm::PointerType::getUnqual(*cg_.context_), "ats.ptr");
                }
            }
            else if (auto *gvInst = llvm::dyn_cast<llvm::GlobalVariable>(arrPtr))
            {
                if (gvInst->getValueType()->isIntegerTy(64))
                {
                    llvm::Value *ptrAsInt = cg_.builder_->CreateLoad(cg_.getInt64Type(), gvInst, "ats.glob.int");
                    arrPtr = cg_.builder_->CreateIntToPtr(ptrAsInt, llvm::PointerType::getUnqual(*cg_.context_), "ats.glob.ptr");
                }
            }
            else if (arrPtr->getType()->isIntegerTy(64))
            {
                arrPtr = cg_.builder_->CreateIntToPtr(arrPtr, llvm::PointerType::getUnqual(*cg_.context_), "ats.raw.ptr");
            }

            auto i64Ty = cg_.getInt64Type();
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // Load array length and data from SadArray struct
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 حارسُ المصفوفةِ العدمِ — **ثالثُ موضعٍ، وهو الذي يقع فيه العطب**
            //
            //      `مصفوفة س` عاريةً ثمّ `نص(س)` تُلوَّن `array.to.string` وتصل
            //      **هنا**، فتفكُّ `CreateStructGEP` مؤشّرًا عدمًا ⇒ `0xC0000005`
            //      ورمزُ ترجمةٍ **صفر**. والمفسّرُ يطبع «لاشيء».
            //
            //      ⚠️ وقصّةُ هذه الرقعةِ تُكتَب لأنّها الدرس: وُضِع الحارسُ أوّلًا في
            //      مساعِداتِ التحويلِ الأربعةِ (نظيرَ حارسِ الخريطة) — فبقي الانهيار.
            //      ثمّ وُضِع في مسارِ **ضمِّ النصوص** (`string_ops.cpp`) — فبقي.
            //      لأنّ `نص(مصفوفة)` لا تمرُّ بأيٍّ منهما. وثلاثُ رقعاتٍ صحيحةٍ في
            //      ثلاثِ طبقاتٍ لا يمرُّ بها العطبُ = صفرُ إصلاحٍ ورمزُ بناءٍ ناجح.
            //
            //      🔑 والدرسُ: **رقعةٌ في الطبقةِ الخطأِ تُبنى وتُصرَّف وتبدو صحيحةً
            //      في المراجعة** — ولا يفرّق بينها وبين الإصلاحِ إلّا القياس. ولو
            //      اكتُفِي بـ«أُضيف الحارسُ» لسُجِّل العيبُ مُغلَقًا وهو حيّ.
            //
            //      والقرارُ على **مؤشّرِ المصفوفةِ** لا على بياناتِها: مصفوفةٌ فارغةٌ
            //      مُهيّأةٌ (`مصفوفة س = []`) حيّةٌ وتبقى «[]».
            // (EN) Null-array guard — the THIRD site, and the one the defect passes
            //      through. It was first added to the four to-string helpers (the crash
            //      persisted), then to the string-concat path (it persisted), because
            //      `نص(array)` lowers to array.to.string and reaches HERE. Three correct
            //      patches in three layers the defect never traverses = zero fix and a
            //      green build. Only measurement tells a fix from a patch.
            // ════════════════════════════════════════════════════════════════
            // (AR) والعدمُ هنا **وجهان** لا وجهٌ واحد: مؤشّرٌ صفريّ (تصريحٌ عارٍ)
            //      و`kSadNullSentinel` (تهيئةٌ صريحةٌ بـ«لاشيء» — `مصفوفة عدمية س = لاشيء`
            //      تُخزِّن الرمزَ في خانةِ المؤشّر). وقِيس أنّ حارسَ المؤشّرِ وحدَه يترك
            //      الثانيةَ تنهار: فالرمزُ ليس صفرًا، فيمرُّ ويُفَكُّ عنوانًا.
            // (EN) "Null" has TWO shapes here: a zero pointer (bare declaration) and
            //      kSadNullSentinel (explicit `= لاشيء`, which stores the sentinel in the
            //      pointer slot). A pointer-only guard lets the second one crash.
            // (AR) وهذه أوّلُ النسخِ وأصحُّها — صارت نداءً للبابِ الواحدِ لا نسخةً
            //      خامسة. ونصُّها محفوظٌ في سجلِّ الحارسِ نفسِه لأنّه سجلُّ الدرس.
            // (EN) The first and most correct copy — now a call to the single door.
            auto atsGuard = cg_.emitContainerNullGuard(
                arrPtr, arrTy, "__sad_null_array_placeholder", "ats");
            llvm::Value *atsIsNull = atsGuard.isNull;
            arrPtr = atsGuard.safePtr;

            // (AR) يُنتقى المخرَجُ عند كلِّ مخرجٍ من الدالّة — موضعٌ واحدٌ لأربعةِ مسارات.
            auto atsFinish = [&](llvm::Value *live) -> llvm::Value *
            {
                llvm::Value *picked = cg_.builder_->CreateSelect(
                    atsIsNull, cg_.emitSafeStringPtr(nullptr, "ats.null"), live, "ats.picked");
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = picked;
                return picked;
            };

            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "ats.len.gep");
            llvm::Value *arrLen = cg_.builder_->CreateLoad(i64Ty, lenGep, "ats.len");
            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "ats.data.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "ats.data");

            // ================================================================
            // (AR) توزيعٌ حسب نوع العنصر المعروف ساكنًا (ISSUE-080): العناصر النصّيّة
            //      تحمل مؤشّرات (%s)، والعشريّة بتّات double (bitcast⇒__sad_format_double)،
            //      وغيرها تُنسَّق %lld. النصّيّ والعشريّ يخصّصان مخزنهما (طول متغيّر) ويُعيدانه.
            //      المصفوفة المختلطة النوع تبقى elementType=Void ⇒ المسار العدديّ (%lld) —
            //      تعليب عناصرها الديناميّة بـ%SadDyn مؤجَّل (الخيار أ، ISSUE-082/084).
            // (EN) Dispatch by the statically-known element type (ISSUE-080): string
            //      elements hold pointers (%s), float elements hold double bits
            //      (bitcast⇒__sad_format_double), else format as %lld. String/float
            //      variants malloc their own (variable-length) buffer and return it.
            //      Heterogeneous arrays stay elementType=Void ⇒ the integer (%lld) path;
            //      boxing their dynamic elements as %SadDyn is deferred (option A).
            // ================================================================
            const Compiler::SIR::SadTypeKind elemTy = inst->operands[0].elementType;
            if (elemTy == Compiler::SIR::SadTypeKind::String)
            {
                cg_.ensureArrayToStringStrHelper();
                llvm::FunctionType *strHelperType = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy}, false);
                llvm::FunctionCallee strHelperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string_str", strHelperType);
                llvm::Value *sResult = cg_.builder_->CreateCall(strHelperFn, {arrLen, dataPtr}, "ats.sresult");
                return atsFinish(sResult);
            }
            if (elemTy == Compiler::SIR::SadTypeKind::Float)
            {
                cg_.ensureArrayToStringFloatHelper();
                llvm::FunctionType *fHelperType = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy}, false);
                llvm::FunctionCallee fHelperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string_float", fHelperType);
                llvm::Value *fResult = cg_.builder_->CreateCall(fHelperFn, {arrLen, dataPtr}, "ats.fresult");
                return atsFinish(fResult);
            }
            if (elemTy == Compiler::SIR::SadTypeKind::Any)
            {
                // (AR) [وسم زمن-التشغيل] مصفوفةٌ مختلطةٌ قياسيّة: الخانةُ حمولةُ i64 خام،
                //      والنوعُ الحقيقيّ في مخزن الوسوم الموازي (الحقل 3). نمرّر (الطول،
                //      البيانات، الوسوم) للمساعد فيعيد بناء كلّ عنصرٍ عبر dynToString (نظير المفسّر).
                // (EN) [runtime tags] scalar-heterogeneous array: each slot is a raw i64
                //      payload; the real type lives in the parallel tags buffer (field 3).
                //      Pass (len, data, tags) so the helper reconstructs each element via
                //      dynToString (like the interpreter). tags==null ⇒ helper falls back to Int.
                llvm::Value *tagsGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "ats.tags.gep");
                llvm::Value *tagsPtr = cg_.builder_->CreateLoad(ptrTy, tagsGep, "ats.tags");
                cg_.ensureArrayToStringDynHelper();
                llvm::FunctionType *dHelperType = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy, ptrTy}, false);
                llvm::FunctionCallee dHelperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string_dyn", dHelperType);
                llvm::Value *dResult = cg_.builder_->CreateCall(dHelperFn, {arrLen, dataPtr, tagsPtr}, "ats.dresult");
                return atsFinish(dResult);
            }

            // Ensure helper function exists
            cg_.ensureArrayToStringHelper();

            // Allocate buffer: len * 34 + 4 bytes
            llvm::Value *bufLen = cg_.builder_->CreateAdd(
                cg_.builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 34)),
                llvm::ConstantInt::get(i64Ty, 4), "ats.bufsz");

            llvm::Value *buf = cg_.emitMalloc(bufLen, "ats.buf");

            // Call __sad_array_to_string(buf, len, data)
            llvm::FunctionType *helperType = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
            llvm::FunctionCallee helperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string", helperType);
            llvm::Value *result = cg_.builder_->CreateCall(helperFn, {buf, arrLen, dataPtr}, "ats.result");

            return atsFinish(result);
        }

        llvm::Value *ArithmeticCodeGen::emitCast(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "CAST"}});
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // Determine target type from result dataType
            SadTypeKind targetType = SadTypeKind::Integer;
            if (inst->result.has_value())
            {
                targetType = inst->result->dataType;
            }
            else if (inst->operands.size() >= 2)
            {
                targetType = inst->operands[1].dataType;
            }

            llvm::Value *result = val;
            llvm::Type *valType = val->getType();

            switch (targetType)
            {
            case SadTypeKind::Integer:
                if (valType->isDoubleTy())
                    result = cg_.builder_->CreateFPToSI(val, cg_.getInt64Type(), "cast_i64");
                else if (valType->isIntegerTy(1))
                    result = cg_.builder_->CreateZExt(val, cg_.getInt64Type(), "cast_i64");
                else if (valType->isPointerTy())
                    result = cg_.builder_->CreatePtrToInt(val, cg_.getInt64Type(), "cast_i64");
                break;
            case SadTypeKind::Float:
                if (valType->isIntegerTy())
                    result = cg_.builder_->CreateSIToFP(val, cg_.getDoubleType(), "cast_f64");
                break;
            case SadTypeKind::Boolean:
                if (valType->isIntegerTy(64))
                    result = cg_.builder_->CreateICmpNE(val, llvm::ConstantInt::get(cg_.getInt64Type(), 0), "cast_bool");
                else if (valType->isDoubleTy())
                    result = cg_.builder_->CreateFCmpONE(val, llvm::ConstantFP::get(cg_.getDoubleType(), 0.0), "cast_bool");
                break;
            case SadTypeKind::Pointer:
            case SadTypeKind::String:
                if (valType->isIntegerTy())
                    result = cg_.builder_->CreateIntToPtr(val, llvm::PointerType::getUnqual(*cg_.context_), "cast_ptr");
                break;
            default:
                break;
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

    } // namespace LLVM
} // namespace Sad
