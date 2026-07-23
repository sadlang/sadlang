/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 *
 * هذا الملف يحتوي على تنفيذ مولد كود LLVM IR الذي يترجم SIR إلى LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 *
 * التزام ب STRICT_CODING_RULES.md:
 * - تم قراءة جميع الملفات الرأسية بالكامل (1746 سطر)
 * - تم توثيق كل استخدام API بموقع المصدر
 * - لا توجد دوال أو متغيرات مخترعة
 *
 * STRICT_CODING_RULES.md compliance:
 * - All header files read completely (1746 lines)
 * - Every API usage documented with source location
 * - No invented functions or variables
 *
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2024
 * الإصدار (Version): 1.1.4 Phase 1
 * ============================================================================
 */

#include "llvm_codegen.h"
#include <llvm/IR/Intrinsics.h>
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
// Source: LLVM 14+ API - llvm/MC/TargetRegistry.h بدلاً من llvm/Support/TargetRegistry.h
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
#include "sir_constants.h"                            // (AR) kSadNullSentinel (NS-05)
#include "sad_dyn_repr.h" // (AR) ISSUE-063: حارس المعامل %SadDyn بالمسارات الساكنة / (EN) %SadDyn operand guard on static paths
#include "llvm_codegen.h"

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {
        namespace
        {
            // (AR) ISSUE-063: حارسٌ للمسارات الحسابيّة الساكنة — معاملٌ %SadDyn وصل رغم أنّ
            //      نوع نتيجة SIR ليس Any (نوعٌ أماميّ بائت، كقراءة خانة رقّاها المسحُ المسبق)
            //      ⇒ فوّض إلى الموزِّع الديناميّ dynBinOp بدل CreateAdd على بنية (IR فاسد).
            // (EN) ISSUE-063: guard for the static arithmetic paths — a %SadDyn operand
            //      arrived although the SIR result type is not Any (a stale frontend type,
            //      e.g. a read of a pre-scan-promoted slot) ⇒ delegate to the dynBinOp
            //      dispatcher instead of CreateAdd on a struct (invalid IR).
            llvm::Value *dispatchDynOperands(LLVMCodeGen &cg,
                                             const std::shared_ptr<SIRInstruction> &inst,
                                             llvm::Value *left, llvm::Value *right)
            {
                llvm::Value *dl = toDyn(cg, left, inst->operands[0].dataType);
                llvm::Value *dr = toDyn(cg, right, inst->operands[1].dataType);
                llvm::Value *res = dynBinOp(cg, inst->opcode, dl, dr);
                if (inst->result.has_value())
                    cg.context_info_.namedValues[inst->result->name] = res;
                return res;
            }

            // (AR) حارس القاسم العشريّ الصفريّ المشترك (د-1 + إغلاق بوّابتَي NaN
            //      المتبقّيتين RUN009/RUN010): يزرع فحص divisor == 0.0 قبل عمليّة
            //      القسمة العشريّة (fdiv/floor(fdiv)/frem) — نمط emitBoundsCheck/
            //      emitNullAssert: مستضاف ⇒ تشخيص عربيّ بالكتالوج (hostedMsg بموضع
            //      %g للمقسوم) + exit(1)؛ حرّ ⇒ __sad_panic برمز انتهاك الفحص.
            //      كان LLVM يطوي هذه العمليّات إلى nan/inf بصمت بينما المفسّر يرمي
            //      RUN001/RUN009/RUN010.
            // (EN) Shared float zero-divisor guard (D-1 + closing the two remaining
            //      NaN gates RUN009/RUN010): plants a divisor == 0.0 check before
            //      the float division op (fdiv/floor(fdiv)/frem) — the
            //      emitBoundsCheck/emitNullAssert pattern: hosted ⇒ Arabic catalog
            //      diagnostic (hostedMsg with %g for the dividend) + exit(1);
            //      freestanding ⇒ __sad_panic(check-violation). LLVM silently
            //      folded these to nan/inf while the interpreter throws.
            void emitFloatDivZeroGuard(LLVMCodeGen &cg, llvm::Value *dividend,
                                       llvm::Value *divisor, const char *hostedMsg,
                                       const char *tag)
            {
                llvm::IRBuilder<> &b = *cg.builder_;
                llvm::LLVMContext &ctx = *cg.context_;
                llvm::Value *isZeroDiv = b.CreateFCmpOEQ(
                    divisor, llvm::ConstantFP::get(cg.getDoubleType(), 0.0),
                    std::string(tag) + ".iszero");
                llvm::Function *curFunc = b.GetInsertBlock()->getParent();
                llvm::BasicBlock *failBB =
                    llvm::BasicBlock::Create(ctx, std::string(tag) + ".fail", curFunc);
                llvm::BasicBlock *contBB =
                    llvm::BasicBlock::Create(ctx, std::string(tag) + ".ok", curFunc);
                b.CreateCondBr(isZeroDiv, failBB, contBB);

                b.SetInsertPoint(failBB);
                if (cg.freestanding_)
                {
                    cg.emitFreestandingPanicCall(Sad::Compiler::kSadPanicCheckViolation);
                }
                else
                {
                    auto ptrTy = llvm::PointerType::getUnqual(ctx);
                    auto *printfType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(ctx), {ptrTy}, true);
                    auto printfFunc = cg.module_->getOrInsertFunction("printf", printfType);
                    llvm::Value *msg = b.CreateGlobalStringPtr(
                        hostedMsg, std::string(tag) + ".fmt");
                    b.CreateCall(printfFunc, {msg, dividend});
                    auto *exitType = llvm::FunctionType::get(
                        llvm::Type::getVoidTy(ctx), {llvm::Type::getInt32Ty(ctx)}, false);
                    auto exitFunc = cg.module_->getOrInsertFunction("exit", exitType);
                    b.CreateCall(exitFunc,
                                 {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1)});
                }
                b.CreateUnreachable();

                b.SetInsertPoint(contBB);
            }
        } // namespace

        llvm::Value *ArithmeticCodeGen::emitAdd(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            // Source: SIRInstruction::operands is PUBLIC member at sir_instruction.h:62
            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Add"}});
                return nullptr;
            }

            // (AR) ISSUE-076/084 (ب″): نتيجةٌ ديناميّة (Any) ⇒ معامِلٌ حمولةُ ADT مجهولةُ النوع.
            //      نُفوّض لمسارٍ يفحص الوسم زمنَ التشغيل (عشريّ مُعلَّب/صحيح) ويُنتج نتيجةً موسومة.
            // (EN) ISSUE-076/084 (ب″): a dynamic (Any) result ⇒ an operand is a statically-unknown
            //      ADT payload. Delegate to the runtime-tag-dispatching dynamic path.
            if (inst->result.has_value() && inst->result->dataType == SadTypeKind::Any)
                return emitDynamicNumericBinOp(inst);

            // الحصول على المعاملات باستخدام resolveOperand لدعم الثوابت والسجلات
            // Get operands using resolveOperand to support both constants and registers
            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معاملٌ %SadDyn بمسارٍ ساكن (نوع SIR أماميّ بائت) ⇒ الحسابيّات
            //      تُفوَّض للموزِّع الديناميّ، والمنطقيّات/البتّيّات تفكّ الحمولة i64.
            // (EN) ISSUE-063: a %SadDyn operand on a static path (stale frontend SIR type) ⇒
            //      arithmetic delegates to the dynamic dispatcher; logical/bitwise ops
            //      unpack the i64 payload.
            if (isSadDyn(left) || isSadDyn(right))
            {
                switch (inst->opcode)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::MOD_I64:
                case SIROpcode::FLOOR_DIV_I64:
                    return dispatchDynOperands(cg_, inst, left, right);
                default:
                    // (AR) منطقيّات/بتّيّات: فكّ i64 دقيق (عشريّ⇒fptosi، صحيح⇒الحمولة)
                    // (EN) logical/bitwise: precise i64 unpack (Float⇒fptosi, Int⇒payload)
                    if (isSadDyn(left))
                        left = unpackI64(cg_, left);
                    if (isSadDyn(right))
                        right = unpackI64(cg_, right);
                    break;
                }
            }

            // (AR) تطبيع الأنواع: ptr→i64 و i1→i64 لضمان توافق العمليات الثنائية
            // (EN) Normalize types: ptr→i64 and i1→i64 to ensure binary op type compatibility
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "i1toi64.l");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "i1toi64.r");

            // التحقق من النوع
            // Check type
            // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
            llvm::Value *result = nullptr;
            if (inst->opcode == SIROpcode::ADD_F64)
            {
                // (AR) تحويل i64 إلى double إذا لزم الأمر
                // (EN) Coerce i64 operands to double for float operations
                // (AR) ISSUE-076/084 (ب″): فكّ تعليب أيّ معامل غير-double (صحيح/صندوق Any/%SadDyn
                //      بنيويّ = حمولة ADT مربوطة) قبل العمليّة العشريّة. الحارس القديم isIntegerTy
                //      تخطّى %SadDyn ⇒ FAdd على بنية خام ⇒ فشل verifyModule.
                // (EN) ISSUE-076/084 (ب″): unbox any non-double operand (integer/Any box/structural
                //      %SadDyn = a pattern-bound ADT payload) before the float op. The old isIntegerTy
                //      guard skipped %SadDyn ⇒ FAdd on a raw struct ⇒ verifyModule failure.
                if (!left->getType()->isDoubleTy())
                    left = coerceFloatOperandToDouble(inst->operands[0], left);
                if (!right->getType()->isDoubleTy())
                    right = coerceFloatOperandToDouble(inst->operands[1], right);
                // Source: cg_.builder_ is defined at llvm_codegen.h:637
                result = cg_.builder_->CreateFAdd(left, right, "addtmp");
            }
            else
            {
                // (AR) الجمع الصحيح: تحويل double إلى i64 إذا لزم الأمر
                // (EN) Integer add: convert double to i64 if needed
                if (left->getType()->isDoubleTy())
                    left = cg_.builder_->CreateFPToSI(left, cg_.getInt64Type(), "f64toi64.l");
                if (right->getType()->isDoubleTy())
                    right = cg_.builder_->CreateFPToSI(right, cg_.getInt64Type(), "f64toi64.r");
                result = cg_.builder_->CreateAdd(left, right, "addtmp");
            }

            // حفظ النتيجة
            // Save result
            // Source: SIRInstruction::result is PUBLIC member at sir_instruction.h:61
            if (inst->result.has_value())
            {
                // Source: SIROperand::name is PUBLIC member at sir_types.h:293
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة طرح
         * Emit subtract instruction
         *
         * Source: llvm_codegen.h:412
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *ArithmeticCodeGen::emitSub(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Sub"}});
                return nullptr;
            }

            // (AR) ISSUE-076/084 (ب″): نتيجةٌ ديناميّة (Any) ⇒ مسارٌ ديناميّ يفحص الوسم زمنَ التشغيل.
            // (EN) ISSUE-076/084 (ب″): a dynamic (Any) result ⇒ the runtime-tag-dispatching path.
            if (inst->result.has_value() && inst->result->dataType == SadTypeKind::Any)
                return emitDynamicNumericBinOp(inst);

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معاملٌ %SadDyn بمسارٍ ساكن (نوع SIR أماميّ بائت) ⇒ الحسابيّات
            //      تُفوَّض للموزِّع الديناميّ، والمنطقيّات/البتّيّات تفكّ الحمولة i64.
            // (EN) ISSUE-063: a %SadDyn operand on a static path (stale frontend SIR type) ⇒
            //      arithmetic delegates to the dynamic dispatcher; logical/bitwise ops
            //      unpack the i64 payload.
            if (isSadDyn(left) || isSadDyn(right))
            {
                switch (inst->opcode)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::MOD_I64:
                case SIROpcode::FLOOR_DIV_I64:
                    return dispatchDynOperands(cg_, inst, left, right);
                default:
                    // (AR) منطقيّات/بتّيّات: فكّ i64 دقيق (عشريّ⇒fptosi، صحيح⇒الحمولة)
                    // (EN) logical/bitwise: precise i64 unpack (Float⇒fptosi, Int⇒payload)
                    if (isSadDyn(left))
                        left = unpackI64(cg_, left);
                    if (isSadDyn(right))
                        right = unpackI64(cg_, right);
                    break;
                }
            }

            // (AR) تطبيع الأنواع: ptr→i64 و i1→i64
            // (EN) Normalize types: ptr→i64 and i1→i64
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "i1toi64.l");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "i1toi64.r");

            llvm::Value *result = nullptr;
            if (inst->opcode == SIROpcode::SUB_F64)
            {
                // (AR) ISSUE-076/084 (ب″): فكّ تعليب أيّ معامل غير-double (صحيح/صندوق Any/%SadDyn
                //      بنيويّ = حمولة ADT مربوطة) قبل العمليّة العشريّة. الحارس القديم isIntegerTy
                //      تخطّى %SadDyn ⇒ FSub على بنية خام ⇒ فشل verifyModule.
                // (EN) ISSUE-076/084 (ب″): unbox any non-double operand (integer/Any box/structural
                //      %SadDyn = a pattern-bound ADT payload) before the float op. The old isIntegerTy
                //      guard skipped %SadDyn ⇒ FSub on a raw struct ⇒ verifyModule failure.
                if (!left->getType()->isDoubleTy())
                    left = coerceFloatOperandToDouble(inst->operands[0], left);
                if (!right->getType()->isDoubleTy())
                    right = coerceFloatOperandToDouble(inst->operands[1], right);
                result = cg_.builder_->CreateFSub(left, right, "subtmp");
            }
            else
            {
                // (AR) الطرح الصحيح: تحويل double إلى i64 إذا لزم الأمر
                // (EN) Integer sub: convert double to i64 if needed
                if (left->getType()->isDoubleTy())
                    left = cg_.builder_->CreateFPToSI(left, cg_.getInt64Type(), "f64toi64.l");
                if (right->getType()->isDoubleTy())
                    right = cg_.builder_->CreateFPToSI(right, cg_.getInt64Type(), "f64toi64.r");
                result = cg_.builder_->CreateSub(left, right, "subtmp");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة ضرب
         * Emit multiply instruction
         *
         * Source: llvm_codegen.h:413
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *ArithmeticCodeGen::emitMul(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Mul"}});
                return nullptr;
            }

            // (AR) ISSUE-076/084 (ب″): نتيجةٌ ديناميّة (Any) ⇒ مسارٌ ديناميّ يفحص الوسم زمنَ التشغيل.
            // (EN) ISSUE-076/084 (ب″): a dynamic (Any) result ⇒ the runtime-tag-dispatching path.
            if (inst->result.has_value() && inst->result->dataType == SadTypeKind::Any)
                return emitDynamicNumericBinOp(inst);

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معاملٌ %SadDyn بمسارٍ ساكن (نوع SIR أماميّ بائت) ⇒ الحسابيّات
            //      تُفوَّض للموزِّع الديناميّ، والمنطقيّات/البتّيّات تفكّ الحمولة i64.
            // (EN) ISSUE-063: a %SadDyn operand on a static path (stale frontend SIR type) ⇒
            //      arithmetic delegates to the dynamic dispatcher; logical/bitwise ops
            //      unpack the i64 payload.
            if (isSadDyn(left) || isSadDyn(right))
            {
                switch (inst->opcode)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::MOD_I64:
                case SIROpcode::FLOOR_DIV_I64:
                    return dispatchDynOperands(cg_, inst, left, right);
                default:
                    // (AR) منطقيّات/بتّيّات: فكّ i64 دقيق (عشريّ⇒fptosi، صحيح⇒الحمولة)
                    // (EN) logical/bitwise: precise i64 unpack (Float⇒fptosi, Int⇒payload)
                    if (isSadDyn(left))
                        left = unpackI64(cg_, left);
                    if (isSadDyn(right))
                        right = unpackI64(cg_, right);
                    break;
                }
            }

            // (AR) تطبيع الأنواع: ptr→i64 و i1→i64
            // (EN) Normalize types: ptr→i64 and i1→i64
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "i1toi64.l");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "i1toi64.r");

            llvm::Value *result = nullptr;
            if (inst->opcode == SIROpcode::MUL_F64)
            {
                // (AR) ISSUE-076/084 (ب″): فكّ تعليب أيّ معامل غير-double قبل العمليّة العشريّة —
                //      صحيحٌ (sitofp) أو صندوق Any مُرمَّز i64 أو قيمة %SadDyn بنيويّة (حمولة ADT
                //      مربوطة بمطابقة نمط). الحارس القديم isIntegerTy تخطّى %SadDyn (بنية لا i64)
                //      فيصل خام إلى FMul ⇒ «operands not of the same type» في verifyModule.
                // (EN) ISSUE-076/084 (ب″): unbox any non-double operand before the float op —
                //      an integer (sitofp), an i64-tagged Any box, or a structural %SadDyn value
                //      (a pattern-bound ADT payload). The old isIntegerTy guard skipped %SadDyn
                //      (a struct, not i64), so it reached FMul raw ⇒ verifyModule "operands not
                //      of the same type". coerceFloatOperandToDouble unboxes %SadDyn via unpackDouble.
                if (!left->getType()->isDoubleTy())
                    left = coerceFloatOperandToDouble(inst->operands[0], left);
                if (!right->getType()->isDoubleTy())
                    right = coerceFloatOperandToDouble(inst->operands[1], right);
                result = cg_.builder_->CreateFMul(left, right, "multmp");
            }
            else
            {
                // (AR) الضرب الصحيح: تحويل double إلى i64 إذا لزم الأمر
                // (EN) Integer mul: convert double to i64 if needed
                if (left->getType()->isDoubleTy())
                    left = cg_.builder_->CreateFPToSI(left, cg_.getInt64Type(), "f64toi64.l");
                if (right->getType()->isDoubleTy())
                    right = cg_.builder_->CreateFPToSI(right, cg_.getInt64Type(), "f64toi64.r");
                result = cg_.builder_->CreateMul(left, right, "multmp");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة قسمة
         * Emit divide instruction
         *
         * Source: llvm_codegen.h:414
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *ArithmeticCodeGen::emitDiv(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Div"}});
                return nullptr;
            }

            // (AR) ISSUE-076/084 (ب″): نتيجةٌ ديناميّة (Any) ⇒ مسارٌ ديناميّ (يشمل // على حمولة).
            // (EN) ISSUE-076/084 (ب″): a dynamic (Any) result ⇒ the dynamic path (incl. // on a payload).
            if (inst->result.has_value() && inst->result->dataType == SadTypeKind::Any)
                return emitDynamicNumericBinOp(inst);

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معاملٌ %SadDyn بمسارٍ ساكن (نوع SIR أماميّ بائت) ⇒ الحسابيّات
            //      تُفوَّض للموزِّع الديناميّ، والمنطقيّات/البتّيّات تفكّ الحمولة i64.
            // (EN) ISSUE-063: a %SadDyn operand on a static path (stale frontend SIR type) ⇒
            //      arithmetic delegates to the dynamic dispatcher; logical/bitwise ops
            //      unpack the i64 payload.
            if (isSadDyn(left) || isSadDyn(right))
            {
                switch (inst->opcode)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::MOD_I64:
                case SIROpcode::FLOOR_DIV_I64:
                    return dispatchDynOperands(cg_, inst, left, right);
                default:
                    // (AR) منطقيّات/بتّيّات: فكّ i64 دقيق (عشريّ⇒fptosi، صحيح⇒الحمولة)
                    // (EN) logical/bitwise: precise i64 unpack (Float⇒fptosi, Int⇒payload)
                    if (isSadDyn(left))
                        left = unpackI64(cg_, left);
                    if (isSadDyn(right))
                        right = unpackI64(cg_, right);
                    break;
                }
            }

            // (AR) تطبيع الأنواع: ptr→i64 و i1→i64
            // (EN) Normalize types: ptr→i64 and i1→i64
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "i1toi64.l");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "i1toi64.r");

            llvm::Value *result = nullptr;
            if (inst->opcode == SIROpcode::DIV_F64)
            {
                // (AR) ISSUE-076/084 (ب″): فكّ تعليب أيّ معامل غير-double (صحيح/صندوق Any/%SadDyn
                //      بنيويّ = حمولة ADT مربوطة) قبل العمليّة العشريّة. الحارس القديم isIntegerTy
                //      تخطّى %SadDyn ⇒ FDiv على بنية خام ⇒ فشل verifyModule.
                // (EN) ISSUE-076/084 (ب″): unbox any non-double operand (integer/Any box/structural
                //      %SadDyn = a pattern-bound ADT payload) before the float op. The old isIntegerTy
                //      guard skipped %SadDyn ⇒ FDiv on a raw struct ⇒ verifyModule failure.
                if (!left->getType()->isDoubleTy())
                    left = coerceFloatOperandToDouble(inst->operands[0], left);
                if (!right->getType()->isDoubleTy())
                    right = coerceFloatOperandToDouble(inst->operands[1], right);

                // (AR) حارس القسمة العشريّة على صفر (د-1، توافق المحرّكين): كان fdiv
                //      يُطوى إلى nan/inf بصمت — بوّابة NaN الوحيدة في ص (لا حرفيّة
                //      NaN) فتعطي وافق(0.0/0.0، س) قيمة اعتباطيّة (FPToSI-poison).
                //      المفسّر يرمي RUN001؛ هنا نطابقه بفحص المقسوم عليه == 0.0 قبل
                //      fdiv (نمط emitBoundsCheck/emitNullAssert): مستضاف ⇒ تشخيص
                //      RUN001 العربيّ + exit(1)؛ حرّ ⇒ __sad_panic برمز انتهاك الفحص
                //      (كنمط فحص الحدود — اللافتة السياديّة هي التشخيص الوحيد).
                //      طيّ الثوابت في الواجهة يرفض أصلًا طيَّ مقسومٍ عليه صفريّ
                //      (sir_frontend_optimizer/constant_folding_pass) فيصل الثابتان
                //      إلى هذا الحارس ويُرفَضان زمنيًّا لا يُطويان إلى NaN.
                // (EN) Float division-by-zero guard (D-1, engine parity): fdiv folded
                //      silently to nan/inf — the language's only NaN gate (no NaN
                //      literal), making وافق(0.0/0.0, x) arbitrary (FPToSI poison).
                //      The interpreter throws RUN001; we match it by testing the
                //      divisor == 0.0 before fdiv (emitBoundsCheck/emitNullAssert
                //      pattern): hosted ⇒ Arabic RUN001 diagnostic + exit(1);
                //      freestanding ⇒ __sad_panic(check-violation), like the bounds
                //      check. Frontend constant folding already refuses to fold a
                //      zero divisor, so constant operands reach this runtime guard.
                emitFloatDivZeroGuard(cg_, left, right,
                                      Sad::Compiler::kDivZeroRun001Msg, "fdiv.dz");
                result = cg_.builder_->CreateFDiv(left, right, "divtmp");
            }
            else if (inst->opcode == SIROpcode::FLOOR_DIV_I64)
            {
                // (AR) Amelia (ISSUE-063): نتيجةٌ عشريّة ساكنة (معاملٌ عشريّ) ⇒ floor(fdiv)
                //      على double كالمفسّر (7.5//2=3.0، -7.5//2=-4.0) — مرآةُ مسار `%` العشريّ
                //      في emitMod؛ كان FPToSI يقتطع ثمّ sdiv فيتناقض مع الطيّ والمفسّر.
                // (EN) Amelia (ISSUE-063): a static Float result (a float operand) ⇒ floor(fdiv)
                //      on doubles like the interpreter (7.5//2=3.0, -7.5//2=-4.0) — mirror of
                //      emitMod's float path; FPToSI-then-sdiv contradicted folding + interpreter.
                if (inst->result.has_value() && inst->result->dataType == SadTypeKind::Float)
                {
                    auto toDouble = [&](const SIROperand &op, llvm::Value *v) -> llvm::Value *
                    {
                        if (v->getType()->isDoubleTy())
                            return v;
                        // (AR) قيمةٌ عشريّة النوع وصلت i64 = بِتّات double خام ⇒ bitcast لا sitofp
                        // (EN) A Float-typed value arriving as i64 = raw double bits ⇒ bitcast
                        if (op.dataType == SadTypeKind::Float && v->getType()->isIntegerTy(64))
                            return cg_.builder_->CreateBitCast(v, cg_.getDoubleType(), "fdiv.bitsf64");
                        return coerceFloatOperandToDouble(op, v);
                    };
                    left = toDouble(inst->operands[0], left);
                    right = toDouble(inst->operands[1], right);
                    // (AR) حارس القاسم الصفريّ (إغلاق بوّابة NaN المتبقّية للأرضيّة
                    //      العشريّة): المفسّر يرمي RUN009 — نطابقه قبل fdiv.
                    // (EN) Zero-divisor guard (closing the float floor-div NaN gate):
                    //      the interpreter throws RUN009 — match it before fdiv.
                    emitFloatDivZeroGuard(cg_, left, right,
                                          Sad::Compiler::kFloorDivZeroRun009Msg,
                                          "ffloordiv.dz");
                    llvm::Value *q = cg_.builder_->CreateFDiv(left, right, "ffloordiv.q");
                    llvm::Function *floorFn = llvm::Intrinsic::getDeclaration(
                        cg_.module_.get(), llvm::Intrinsic::floor, {cg_.getDoubleType()});
                    result = cg_.builder_->CreateCall(floorFn, {q}, "ffloordivtmp");
                }
                else
                {
                    // (AR) القسمة الصحيحة الأرضية على i64 — مع تسويةٍ أرضيّة للسالب
                    //      (-7//2=-4 كالمفسّر) بدل اقتطاع sdiv نحو الصفر (-3).
                    // (EN) Integer floor division on i64 — with a floor adjustment for negatives
                    //      (-7//2=-4 like the interpreter) instead of sdiv truncation toward 0 (-3).
                    if (left->getType()->isDoubleTy())
                        left = cg_.builder_->CreateFPToSI(left, cg_.getInt64Type(), "f64toi64.l");
                    if (right->getType()->isDoubleTy())
                        right = cg_.builder_->CreateFPToSI(right, cg_.getInt64Type(), "f64toi64.r");
                    llvm::Value *q = cg_.builder_->CreateSDiv(left, right, "floordivtmp");
                    llvm::Value *rem = cg_.builder_->CreateSRem(left, right, "floordiv.rem");
                    llvm::Value *zero64 = llvm::ConstantInt::get(cg_.getInt64Type(), 0);
                    llvm::Value *signsDiffer = cg_.builder_->CreateICmpSLT(
                        cg_.builder_->CreateXor(left, right, "floordiv.sx"), zero64, "floordiv.sd");
                    llvm::Value *inexact = cg_.builder_->CreateICmpNE(rem, zero64, "floordiv.ix");
                    llvm::Value *needAdj = cg_.builder_->CreateAnd(signsDiffer, inexact, "floordiv.na");
                    llvm::Value *adj = cg_.builder_->CreateZExt(needAdj, cg_.getInt64Type(), "floordiv.adj");
                    result = cg_.builder_->CreateSub(q, adj, "floordivadj");
                }
            }
            else
            {
                // (AR) القسمة الصحيحة: تحويل double إلى i64 إذا لزم الأمر
                // (EN) Integer division: convert double to i64 if needed
                if (left->getType()->isDoubleTy())
                    left = cg_.builder_->CreateFPToSI(left, cg_.getInt64Type(), "f64toi64.l");
                if (right->getType()->isDoubleTy())
                    right = cg_.builder_->CreateFPToSI(right, cg_.getInt64Type(), "f64toi64.r");
                result = cg_.builder_->CreateSDiv(left, right, "divtmp");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة باقي القسمة
         * Emit modulo instruction
         *
         * Source: llvm_codegen.h:415
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *ArithmeticCodeGen::emitMod(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Mod"}});
                return nullptr;
            }

            // (AR) ISSUE-076/084 (ب″): نتيجةٌ ديناميّة (Any) ⇒ مسارٌ ديناميّ يفكّ وسم الحمولة.
            // (EN) ISSUE-076/084 (ب″): a dynamic (Any) result ⇒ the dynamic path that decodes the tag.
            if (inst->result.has_value() && inst->result->dataType == SadTypeKind::Any)
                return emitDynamicNumericBinOp(inst);

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معاملٌ %SadDyn بمسارٍ ساكن (نوع SIR أماميّ بائت) ⇒ الحسابيّات
            //      تُفوَّض للموزِّع الديناميّ، والمنطقيّات/البتّيّات تفكّ الحمولة i64.
            // (EN) ISSUE-063: a %SadDyn operand on a static path (stale frontend SIR type) ⇒
            //      arithmetic delegates to the dynamic dispatcher; logical/bitwise ops
            //      unpack the i64 payload.
            if (isSadDyn(left) || isSadDyn(right))
            {
                switch (inst->opcode)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::MOD_I64:
                case SIROpcode::FLOOR_DIV_I64:
                    return dispatchDynOperands(cg_, inst, left, right);
                default:
                    // (AR) منطقيّات/بتّيّات: فكّ i64 دقيق (عشريّ⇒fptosi، صحيح⇒الحمولة)
                    // (EN) logical/bitwise: precise i64 unpack (Float⇒fptosi, Int⇒payload)
                    if (isSadDyn(left))
                        left = unpackI64(cg_, left);
                    if (isSadDyn(right))
                        right = unpackI64(cg_, right);
                    break;
                }
            }

            // (AR) تطبيع الأنواع: ptr→i64 و i1→i64 و double→i64
            // (EN) Normalize types: ptr→i64, i1→i64, double→i64 (mod is always integer)
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "ptr2i64.l");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "ptr2i64.r");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "i1toi64.l");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "i1toi64.r");
            // (AR) ISSUE-063: نتيجةٌ عشريّة ساكنة (معاملٌ عشريّ) ⇒ frem (fmod) على double —
            //      مطابقةً للمفسّر (7.5 % 2 = 1.5) وللمسار الديناميكيّ (dynBinOp). كان المسار
            //      الساكن يقتطع إلى i64 ثمّ srem فيعطي 1 (تناقضٌ داخليّ مع الديناميكيّ).
            // (EN) ISSUE-063: a static Float result (a float operand) ⇒ frem (fmod) on
            //      doubles — matching the interpreter (7.5 % 2 = 1.5) and the dynamic path
            //      (dynBinOp). The static path used to truncate to i64 then srem, yielding 1
            //      (internally inconsistent with the dynamic path).
            if (inst->result.has_value() && inst->result->dataType == SadTypeKind::Float)
            {
                auto toDouble = [&](const SIROperand &op, llvm::Value *v) -> llvm::Value *
                {
                    if (v->getType()->isDoubleTy())
                        return v;
                    // (AR) قيمةٌ عشريّة النوع وصلت i64 = بِتّات double خام ⇒ bitcast لا sitofp
                    // (EN) A Float-typed value arriving as i64 = raw double bits ⇒ bitcast
                    if (op.dataType == SadTypeKind::Float && v->getType()->isIntegerTy(64))
                        return cg_.builder_->CreateBitCast(v, cg_.getDoubleType(), "mod.bitsf64");
                    return coerceFloatOperandToDouble(op, v);
                };
                left = toDouble(inst->operands[0], left);
                right = toDouble(inst->operands[1], right);
                // (AR) حارس القاسم الصفريّ (إغلاق بوّابة NaN المتبقّية للباقي
                //      العشريّ): المفسّر يرمي RUN010 — نطابقه قبل frem.
                // (EN) Zero-divisor guard (closing the float modulo NaN gate): the
                //      interpreter throws RUN010 — match it before frem.
                emitFloatDivZeroGuard(cg_, left, right,
                                      Sad::Compiler::kModZeroRun010Msg, "fmod.dz");
                llvm::Value *fresult = cg_.builder_->CreateFRem(left, right, "fmodtmp");
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = fresult;
                }
                return fresult;
            }

            // (AR) عملية الباقي الصحيحة — تحويل double إلى i64
            // (EN) Integer modulo — convert double to i64
            if (left->getType()->isDoubleTy())
                left = cg_.builder_->CreateFPToSI(left, cg_.getInt64Type(), "f64toi64.l");
            if (right->getType()->isDoubleTy())
                right = cg_.builder_->CreateFPToSI(right, cg_.getInt64Type(), "f64toi64.r");

            llvm::Value *result = cg_.builder_->CreateSRem(left, right, "modtmp");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة نفي
         * Emit negate instruction
         *
         * Source: llvm_codegen.h:416
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *ArithmeticCodeGen::emitNeg(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Neg"}});
                return nullptr;
            }

            llvm::Value *operand = resolveOperand(inst->operands[0]);

            if (!operand)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operand"}});
                return nullptr;
            }

            // (AR) ISSUE-063: نفيُ قيمةٍ %SadDyn ⇒ 0 − قيمة عبر الموزِّع الديناميّ
            //      (يحفظ الوسم: صحيح يبقى صحيحًا وعشريّ عشريًّا) بدل CreateNeg على بنية.
            // (EN) ISSUE-063: negating a %SadDyn value ⇒ 0 − value via the dynamic
            //      dispatcher (kind-preserving) instead of CreateNeg on a struct.
            if (isSadDyn(operand))
            {
                llvm::Value *dynZero = packDyn(
                    cg_, llvm::ConstantInt::get(cg_.getInt64Type(), 0), DynKind::Int);
                llvm::Value *dynRes = dynBinOp(cg_, SIROpcode::SUB_I64, dynZero, operand);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = dynRes;
                return dynRes;
            }

            llvm::Value *result;
            if (operand->getType()->isDoubleTy() || operand->getType()->isFloatTy())
            {
                // (AR) استخدام FNeg للأنواع العشرية
                // (EN) Use FNeg for floating-point types
                result = cg_.builder_->CreateFNeg(operand, "negtmp");
            }
            else
            {
                result = cg_.builder_->CreateNeg(operand, "negtmp");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * (AR) إصدار تعليمة تأكيد عدم الفراغ (NS-05): قيمة مؤكَّد — T؟ → T.
         *      إن ساوت القيمةُ الحارسَ (kSadNullSentinel) ⇒ طباعة خطأ RUN056 ثم exit(1)
         *      (نمط emitBoundsCheck)؛ وإلّا تُمرَّر القيمة كما هي.
         * (EN) Emit null-assertion (NS-05): value مؤكَّد — T? → T. If the value equals the
         *      sentinel (kSadNullSentinel) → print RUN056 then exit(1) (emitBoundsCheck pattern),
         *      otherwise pass the value through.
         */
        llvm::Value *ArithmeticCodeGen::emitNullAssert(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "NullAssert"}});
                return nullptr;
            }

            llvm::Value *operand = resolveOperand(inst->operands[0]);
            if (!operand)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "NullAssert operand"}});
                return nullptr;
            }

            // (AR) حوّل القيمة إلى i64 للمقارنة بالحارس (مؤشّر→ptrtoint، صحيح→تمديد/قصّ)
            // (EN) Normalize value to i64 for sentinel compare (pointer→ptrtoint, int→ext/trunc)
            auto i64Ty = cg_.getInt64Type();
            llvm::Type *ot = operand->getType();
            llvm::Value *asI64 = nullptr;
            if (ot->isPointerTy())
                asI64 = cg_.builder_->CreatePtrToInt(operand, i64Ty, "na.p2i");
            else if (ot->isIntegerTy())
                asI64 = (ot == i64Ty) ? operand : cg_.builder_->CreateSExtOrTrunc(operand, i64Ty, "na.ext");
            // (AR) الأنواع العشرية لا تُمثَّل بالحارس ⇒ لا فحص / floats can't be the sentinel ⇒ no check

            if (asI64)
            {
                llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                    asI64,
                    llvm::ConstantInt::get(i64Ty, static_cast<uint64_t>(Sad::Compiler::kSadNullSentinel)),
                    "na.isnull");

                llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
                llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "na.fail", curFunc);
                llvm::BasicBlock *contBB = llvm::BasicBlock::Create(*cg_.context_, "na.ok", curFunc);
                cg_.builder_->CreateCondBr(isNull, failBB, contBB);

                // (AR) كتلة الفشل: مستضاف ⇒ تشخيص RUN056 العربيّ + exit(1)؛ وضع حرّ
                //      ⇒ __sad_panic برمز التأكيد المميَّز وحده (النواة تُصنّف لافتته
                //      بدقّة؛ وسطرٌ عربيّ سابق يخالف مبدأ «اللافتةُ هي التشخيصُ الوحيد»).
                // (EN) Fail block: hosted ⇒ Arabic RUN056 diagnostic + exit(1);
                //      freestanding ⇒ __sad_panic with the distinct null-assert code
                //      only (the kernel banner classifies it precisely).
                cg_.builder_->SetInsertPoint(failBB);
                if (cg_.freestanding_)
                {
                    // (AR) وضع حرّ: __sad_panic برمز kSadPanicNullAssert (weak، النواة
                    //      تتجاوزه فتُصنّف اللافتة «تأكيد على عدم» لا «انتهاك مترجم»).
                    //      لا printf: اللافتة العربيّة السياديّة هي التشخيص الوحيد.
                    // (EN) Freestanding: __sad_panic with kSadPanicNullAssert (weak;
                    //      kernel overrides → banner classifies as null-assert). No
                    //      printf: the sovereign Arabic banner is the sole diagnostic.
                    cg_.emitFreestandingPanicCall(Sad::Compiler::kSadPanicNullAssert);
                }
                else
                {
                    // (AR) مستضاف: تشخيص RUN056 العربيّ للمطوّر (منفذ libc) ثم exit(1)
                    // (EN) Hosted: Arabic RUN056 developer diagnostic (libc) then exit(1)
                    auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                    auto *printfType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(*cg_.context_), {ptrTy}, true);
                    auto printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);
                    llvm::Value *msg = cg_.builder_->CreateGlobalStringPtr(
                        Sad::Compiler::kNullAssertRun056Msg, "na.fmt");
                    cg_.builder_->CreateCall(printfFunc, {msg});
                    auto *exitType = llvm::FunctionType::get(
                        llvm::Type::getVoidTy(*cg_.context_), {llvm::Type::getInt32Ty(*cg_.context_)}, false);
                    auto exitFunc = cg_.module_->getOrInsertFunction("exit", exitType);
                    cg_.builder_->CreateCall(exitFunc, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 1)});
                }
                cg_.builder_->CreateUnreachable();

                // (AR) كتلة الاستمرار: القيمة حاضرة / (EN) Continue: value present
                cg_.builder_->SetInsertPoint(contBB);
            }

            // (AR) تمرير القيمة الحاضرة كنتيجة / (EN) Pass present value through as result
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = operand;
            }
            return operand;
        }

        /**
         * إصدار تعليمة AND ثنائي
         * Emit bitwise AND instruction
         *
         * Source: llvm_codegen.h:422
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        // (AR) انظر التوثيق في arithmetic_codegen.h — دلالة الإشباع الموحَّدة منصّيًّا
        // (EN) See arithmetic_codegen.h — platform-independent saturation semantics
        llvm::Value *ArithmeticCodeGen::emitF64ToI64Sat(llvm::Value *v, const llvm::Twine &name)
        {
            llvm::Function *satFn = llvm::Intrinsic::getDeclaration(
                cg_.module_.get(), llvm::Intrinsic::fptosi_sat,
                {cg_.getInt64Type(), cg_.getDoubleType()});
            return cg_.builder_->CreateCall(satFn, {v}, name);
        }

        // ====================================================================
        // (AR) F64_TO_I64_SAT — نقطة التحويل الواحدة لمعاملات البتّيّات: تفكّ
        //      بحسب ما تراه فعليًّا (لا بحسب مسار الوصول):
        //      • double ⇒ llvm.fptosi.sat مباشرة.
        //      • %SadDyn ⇒ فكّ بالوسم (unpackI64 — عشريّ⇒sat، صحيح⇒الحمولة).
        //      • i64 وSIR يقول Float ⇒ بتّات double خام (معامل دالة منمَّط
        //        عشريًّا يُمرَّر i64-بتّات في موضع النداء) ⇒ bitcast إلى double
        //        ثمّ sat — هذا جوهر عطب «البتّات الخام» المقيس.
        //      • i64 مجهول/صحيح ⇒ تمرير كما هو. • i1 ⇒ zext.
        // (EN) F64_TO_I64_SAT — the single conversion point for bitwise operands;
        //      dispatches on what it actually sees: double ⇒ llvm.fptosi.sat;
        //      %SadDyn ⇒ tag-unpack (unpackI64: Float⇒sat, Int⇒payload); an i64
        //      whose SIR type says Float is raw double bits (double-typed function
        //      params are passed as raw i64 at call sites) ⇒ bitcast to double
        //      then sat — the measured "raw bits" corruption; unknown/integer
        //      i64 ⇒ passthrough; i1 ⇒ zext.
        // ====================================================================
        llvm::Value *ArithmeticCodeGen::emitF64ToI64SatOp(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "F64ToI64Sat"}});
                return nullptr;
            }
            llvm::Value *v = resolveOperand(inst->operands[0]);
            if (!v)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "F64ToI64Sat"}});
                return nullptr;
            }

            llvm::Value *result = nullptr;
            if (isSadDyn(v))
            {
                result = unpackI64(cg_, v); // (AR) فكّ بالوسم — مُشبَع للعشريّ / (EN) tag-unpack, saturating for Float
            }
            else if (v->getType()->isDoubleTy())
            {
                result = emitF64ToI64Sat(v, "f2i.sat");
            }
            else if (v->getType()->isIntegerTy(64) &&
                     inst->operands[0].dataType == Compiler::SIR::SadTypeKind::Float)
            {
                // (AR) بتّات double خام في وعاء i64 ⇒ أعد تفسيرها ثمّ أشبِع
                // (EN) Raw double bits in an i64 ⇒ reinterpret, then saturate
                llvm::Value *asDbl = cg_.builder_->CreateBitCast(v, cg_.getDoubleType(), "f2i.sat.bc");
                result = emitF64ToI64Sat(asDbl, "f2i.sat");
            }
            else if (v->getType()->isIntegerTy(1))
            {
                result = cg_.builder_->CreateZExt(v, cg_.getInt64Type(), "f2i.sat.zext");
            }
            else if (v->getType()->isPointerTy())
            {
                result = cg_.builder_->CreatePtrToInt(v, cg_.getInt64Type(), "f2i.sat.ptoi");
            }
            else
            {
                result = v; // (AR) i64 صحيح/مجهول — تمرير / (EN) integer/unknown i64 — passthrough
            }

            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *ArithmeticCodeGen::emitAnd(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "And"}});
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معاملٌ %SadDyn بمسارٍ ساكن (نوع SIR أماميّ بائت) ⇒ الحسابيّات
            //      تُفوَّض للموزِّع الديناميّ، والمنطقيّات/البتّيّات تفكّ الحمولة i64.
            // (EN) ISSUE-063: a %SadDyn operand on a static path (stale frontend SIR type) ⇒
            //      arithmetic delegates to the dynamic dispatcher; logical/bitwise ops
            //      unpack the i64 payload.
            if (isSadDyn(left) || isSadDyn(right))
            {
                switch (inst->opcode)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::MOD_I64:
                case SIROpcode::FLOOR_DIV_I64:
                    return dispatchDynOperands(cg_, inst, left, right);
                default:
                    // (AR) منطقيّات/بتّيّات: فكّ i64 دقيق (عشريّ⇒fptosi، صحيح⇒الحمولة)
                    // (EN) logical/bitwise: precise i64 unpack (Float⇒fptosi, Int⇒payload)
                    if (isSadDyn(left))
                        left = unpackI64(cg_, left);
                    if (isSadDyn(right))
                        right = unpackI64(cg_, right);
                    break;
                }
            }

            // (AR) تطبيع الأنواع: تحويل ptr→i64 و i1→i64 و double→i64
            // (EN) Type normalization: convert ptr→i64 and i1→i64 and double→i64
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "and.l.ptoi");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "and.r.ptoi");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "and.l.zext");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "and.r.zext");
            if (left->getType()->isDoubleTy())
                left = emitF64ToI64Sat(left, "and.l.f2i.sat");
            if (right->getType()->isDoubleTy())
                right = emitF64ToI64Sat(right, "and.r.f2i.sat");

            llvm::Value *result = cg_.builder_->CreateAnd(left, right, "andtmp");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة OR ثنائي
         * Emit bitwise OR instruction
         *
         * Source: llvm_codegen.h:423
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *ArithmeticCodeGen::emitOr(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Or"}});
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معاملٌ %SadDyn بمسارٍ ساكن (نوع SIR أماميّ بائت) ⇒ الحسابيّات
            //      تُفوَّض للموزِّع الديناميّ، والمنطقيّات/البتّيّات تفكّ الحمولة i64.
            // (EN) ISSUE-063: a %SadDyn operand on a static path (stale frontend SIR type) ⇒
            //      arithmetic delegates to the dynamic dispatcher; logical/bitwise ops
            //      unpack the i64 payload.
            if (isSadDyn(left) || isSadDyn(right))
            {
                switch (inst->opcode)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::MOD_I64:
                case SIROpcode::FLOOR_DIV_I64:
                    return dispatchDynOperands(cg_, inst, left, right);
                default:
                    // (AR) منطقيّات/بتّيّات: فكّ i64 دقيق (عشريّ⇒fptosi، صحيح⇒الحمولة)
                    // (EN) logical/bitwise: precise i64 unpack (Float⇒fptosi, Int⇒payload)
                    if (isSadDyn(left))
                        left = unpackI64(cg_, left);
                    if (isSadDyn(right))
                        right = unpackI64(cg_, right);
                    break;
                }
            }

            // (AR) تطبيع الأنواع: تحويل ptr→i64 و i1→i64 و double→i64
            // (EN) Type normalization: convert ptr→i64 and i1→i64 and double→i64
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "or.l.ptoi");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "or.r.ptoi");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "or.l.zext");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "or.r.zext");
            if (left->getType()->isDoubleTy())
                left = emitF64ToI64Sat(left, "or.l.f2i.sat");
            if (right->getType()->isDoubleTy())
                right = emitF64ToI64Sat(right, "or.r.f2i.sat");

            llvm::Value *result = cg_.builder_->CreateOr(left, right, "ortmp");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة XOR ثنائي
         * Emit bitwise XOR instruction
         *
         * Source: llvm_codegen.h:424
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *ArithmeticCodeGen::emitXor(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Xor"}});
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operands"}});
                return nullptr;
            }

            // (AR) ISSUE-063: معاملٌ %SadDyn بمسارٍ ساكن (نوع SIR أماميّ بائت) ⇒ الحسابيّات
            //      تُفوَّض للموزِّع الديناميّ، والمنطقيّات/البتّيّات تفكّ الحمولة i64.
            // (EN) ISSUE-063: a %SadDyn operand on a static path (stale frontend SIR type) ⇒
            //      arithmetic delegates to the dynamic dispatcher; logical/bitwise ops
            //      unpack the i64 payload.
            if (isSadDyn(left) || isSadDyn(right))
            {
                switch (inst->opcode)
                {
                case SIROpcode::ADD_I64:
                case SIROpcode::ADD_F64:
                case SIROpcode::SUB_I64:
                case SIROpcode::SUB_F64:
                case SIROpcode::MUL_I64:
                case SIROpcode::MUL_F64:
                case SIROpcode::DIV_I64:
                case SIROpcode::DIV_F64:
                case SIROpcode::MOD_I64:
                case SIROpcode::FLOOR_DIV_I64:
                    return dispatchDynOperands(cg_, inst, left, right);
                default:
                    // (AR) منطقيّات/بتّيّات: فكّ i64 دقيق (عشريّ⇒fptosi، صحيح⇒الحمولة)
                    // (EN) logical/bitwise: precise i64 unpack (Float⇒fptosi, Int⇒payload)
                    if (isSadDyn(left))
                        left = unpackI64(cg_, left);
                    if (isSadDyn(right))
                        right = unpackI64(cg_, right);
                    break;
                }
            }

            // (AR) تطبيع الأنواع: تحويل ptr→i64 و i1→i64 و double→i64
            // (EN) Type normalization: convert ptr→i64 and i1→i64 and double→i64
            if (left->getType()->isPointerTy())
                left = cg_.builder_->CreatePtrToInt(left, cg_.getInt64Type(), "xor.l.ptoi");
            if (right->getType()->isPointerTy())
                right = cg_.builder_->CreatePtrToInt(right, cg_.getInt64Type(), "xor.r.ptoi");
            if (left->getType()->isIntegerTy(1))
                left = cg_.builder_->CreateZExt(left, cg_.getInt64Type(), "xor.l.zext");
            if (right->getType()->isIntegerTy(1))
                right = cg_.builder_->CreateZExt(right, cg_.getInt64Type(), "xor.r.zext");
            if (left->getType()->isDoubleTy())
                left = emitF64ToI64Sat(left, "xor.l.f2i.sat");
            if (right->getType()->isDoubleTy())
                right = emitF64ToI64Sat(right, "xor.r.f2i.sat");

            llvm::Value *result = cg_.builder_->CreateXor(left, right, "xortmp");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة NOT ثنائي
         * Emit bitwise NOT instruction
         *
         * Source: llvm_codegen.h:425
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *ArithmeticCodeGen::emitNot(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Not"}});
                return nullptr;
            }

            llvm::Value *operand = resolveOperand(inst->operands[0]);

            if (!operand)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "Operand"}});
                return nullptr;
            }

            // (AR) تطبيع الأنواع: تحويل ptr→i64 و i1→i64 و double→i64
            // (EN) Type normalization: convert ptr→i64 and i1→i64 and double→i64
            if (operand->getType()->isPointerTy())
                operand = cg_.builder_->CreatePtrToInt(operand, cg_.getInt64Type(), "not.ptoi");
            if (operand->getType()->isDoubleTy())
                operand = emitF64ToI64Sat(operand, "not.f2i.sat");

            // (AR) التمييز بين النفي المنطقي (ليس/!) والنفي البِتّي (~)
            // (EN) Distinguish logical NOT (ليس/!) from bitwise NOT (~)
            // النفي المنطقي: النتيجة BOOL — يجب مقارنة مع 0 (ICmpEQ)
            // النفي البِتّي: النتيجة INTEGER — يجب عكس كل البتات (XOR -1)
            bool isLogicalNot = inst->result.has_value() &&
                                inst->result->dataType == SadTypeKind::Boolean;

            llvm::Value *result;
            if (isLogicalNot)
            {
                // (AR) نفي منطقي: أي قيمة غير صفرية → 0، صفر → 1
                // (EN) Logical NOT: any non-zero → 0, zero → 1
                if (operand->getType()->isIntegerTy(1))
                {
                    // i1 مباشرة — استخدم XOR مع 1
                    result = cg_.builder_->CreateXor(operand, llvm::ConstantInt::get(operand->getType(), 1), "lnot");
                }
                else
                {
                    // i64 أو غيره — قارن مع 0 ثم وسّع لـ i64
                    llvm::Value *isZero = cg_.builder_->CreateICmpEQ(
                        operand, llvm::ConstantInt::get(operand->getType(), 0), "lnot.cmp");
                    result = cg_.builder_->CreateZExt(isZero, cg_.getInt64Type(), "lnot.ext");
                }
            }
            else
            {
                // (AR) نفي بِتّي: عكس كل البتات
                // (EN) Bitwise NOT: flip all bits
                if (operand->getType()->isIntegerTy(1))
                    operand = cg_.builder_->CreateZExt(operand, cg_.getInt64Type(), "not.zext");
                result = cg_.builder_->CreateNot(operand, "nottmp");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة إزاحة يسار
         * Emit shift left instruction
         *
         * Source: llvm_codegen.h:426
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */

    } // namespace LLVM
} // namespace Sad

