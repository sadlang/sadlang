/*
 * ============================================================================
 * LLVM IR Code Generator - Type Conversion Instructions
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
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

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        // (AR) دالة مساعدة: نوع بنية المصفوفة SadArray
        // (EN) Helper: SadArray struct type
        static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
        {
            static llvm::StructType *arrTy = nullptr;
            if (!arrTy)
            {
                arrTy = llvm::StructType::create(ctx, {
                                                          llvm::Type::getInt64Ty(ctx),      // length
                                                          llvm::Type::getInt64Ty(ctx),      // capacity
                                                          llvm::PointerType::getUnqual(ctx) // data pointer
                                                      },
                                                 "SadArray");
            }
            return arrTy;
        }

        // ============================================================================
        // Phase N: Type Conversion Instructions / تعليمات تحويل الأنواع
        // ============================================================================

        llvm::Value *ArithmeticCodeGen::emitI64ToF64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError("I64_TO_F64 requires 1 operand");
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
                cg_.reportError("F64_TO_I64 requires 1 operand");
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            llvm::Value *result;
            if (val->getType()->isIntegerTy())
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
                cg_.reportError("I64_TO_BOOL requires 1 operand");
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
                cg_.reportError("BOOL_TO_I64 requires 1 operand");
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

        llvm::Value *ArithmeticCodeGen::emitI64ToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError("I64_TO_STRING requires 1 operand");
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // Allocate buffer: 21 bytes enough for i64 range + sign + null
            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*cg_.context_), {cg_.getInt64Type()}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc,
                                                    {llvm::ConstantInt::get(cg_.getInt64Type(), 32)}, "i64str_buf");

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

                llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%lld", "fmt_i64");
                cg_.builder_->CreateCall(sprintfFunc, {buf, fmt, val});
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }

        llvm::Value *ArithmeticCodeGen::emitF64ToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError("F64_TO_STRING requires 1 operand");
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            if (!val->getType()->isDoubleTy())
            {
                val = cg_.builder_->CreateSIToFP(val, cg_.getDoubleType(), "tof64");
            }

            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*cg_.context_), {cg_.getInt64Type()}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc,
                                                    {llvm::ConstantInt::get(cg_.getInt64Type(), 64)}, "f64str_buf");

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
                cg_.reportError("BOOL_TO_STRING requires 1 operand");
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
                cg_.reportError("ARRAY_TO_STRING requires 1 operand");
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

            // Ensure helper function exists
            cg_.ensureArrayToStringHelper();

            // Load array length and data from SadArray struct
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "ats.len.gep");
            llvm::Value *arrLen = cg_.builder_->CreateLoad(i64Ty, lenGep, "ats.len");
            llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "ats.data.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "ats.data");

            // Allocate buffer: len * 34 + 4 bytes
            llvm::Value *bufLen = cg_.builder_->CreateAdd(
                cg_.builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 34)),
                llvm::ConstantInt::get(i64Ty, 4), "ats.bufsz");

            llvm::FunctionType *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            llvm::FunctionCallee mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFn, {bufLen}, "ats.buf");

            // Call __sad_array_to_string(buf, len, data)
            llvm::FunctionType *helperType = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
            llvm::FunctionCallee helperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string", helperType);
            llvm::Value *result = cg_.builder_->CreateCall(helperFn, {buf, arrLen, dataPtr}, "ats.result");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ArithmeticCodeGen::emitCast(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError("CAST requires at least 1 operand");
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
