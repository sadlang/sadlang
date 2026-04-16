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

        llvm::Value *LLVMCodeGen::emitI64ToF64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("I64_TO_F64 requires 1 operand");
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
                llvm::Value *ext = builder_->CreateZExt(val, getInt64Type(), "bool2i64");
                result = builder_->CreateSIToFP(ext, getDoubleType(), "i64tof64");
            }
            else
            {
                result = builder_->CreateSIToFP(val, getDoubleType(), "i64tof64");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitF64ToI64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("F64_TO_I64 requires 1 operand");
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
                result = builder_->CreateFPToSI(val, getInt64Type(), "f64toi64");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitI64ToBool(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("I64_TO_BOOL requires 1 operand");
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
                result = builder_->CreateFCmpONE(val,
                                                 llvm::ConstantFP::get(getDoubleType(), 0.0), "f64tobool");
            }
            else
            {
                result = builder_->CreateICmpNE(val,
                                                llvm::ConstantInt::get(val->getType(), 0), "i64tobool");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitBoolToI64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("BOOL_TO_I64 requires 1 operand");
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
                result = builder_->CreateZExt(val, getInt64Type(), "booltoi64");
            }
            else
            {
                result = builder_->CreateZExt(val, getInt64Type(), "exttoi64");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitI64ToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("I64_TO_STRING requires 1 operand");
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // Allocate buffer: 21 bytes enough for i64 range + sign + null
            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
            auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = builder_->CreateCall(mallocFunc,
                                                    {llvm::ConstantInt::get(getInt64Type(), 32)}, "i64str_buf");

            if (freestanding_)
            {
                // (AR) في الوضع الحر: استخدم __sad_itoa بدلاً من sprintf المتغيرة الوسائط
                // (EN) Freestanding: use __sad_itoa instead of variadic sprintf
                auto *itoaType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*context_),
                    {llvm::PointerType::getUnqual(*context_),
                     llvm::Type::getInt64Ty(*context_)},
                    false);
                auto itoaFunc = module_->getOrInsertFunction("__sad_itoa", itoaType);
                builder_->CreateCall(itoaFunc, {buf, val});
            }
            else
            {
                // sprintf(buf, "%lld", val)
                auto *sprintfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*context_),
                    {llvm::PointerType::getUnqual(*context_),
                     llvm::PointerType::getUnqual(*context_)},
                    true);
                auto sprintfFunc = module_->getOrInsertFunction("sprintf", sprintfType);

                llvm::Value *fmt = builder_->CreateGlobalStringPtr("%lld", "fmt_i64");
                builder_->CreateCall(sprintfFunc, {buf, fmt, val});
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }

        llvm::Value *LLVMCodeGen::emitF64ToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("F64_TO_STRING requires 1 operand");
                return nullptr;
            }
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            if (!val->getType()->isDoubleTy())
            {
                val = builder_->CreateSIToFP(val, getDoubleType(), "tof64");
            }

            auto *mallocType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
            auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = builder_->CreateCall(mallocFunc,
                                                    {llvm::ConstantInt::get(getInt64Type(), 64)}, "f64str_buf");

            if (freestanding_)
            {
                // (AR) في الوضع الحر: استخدم __sad_ftoa بدلاً من sprintf المتغيرة الوسائط
                // (EN) Freestanding: use __sad_ftoa instead of variadic sprintf
                auto *ftoaType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*context_),
                    {llvm::PointerType::getUnqual(*context_),
                     llvm::Type::getDoubleTy(*context_)},
                    false);
                auto ftoaFunc = module_->getOrInsertFunction("__sad_ftoa", ftoaType);
                builder_->CreateCall(ftoaFunc, {buf, val});
            }
            else
            {
                auto *sprintfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(*context_),
                    {llvm::PointerType::getUnqual(*context_),
                     llvm::PointerType::getUnqual(*context_)},
                    true);
                auto sprintfFunc = module_->getOrInsertFunction("sprintf", sprintfType);

                llvm::Value *fmt = builder_->CreateGlobalStringPtr("%g", "fmt_f64");
                builder_->CreateCall(sprintfFunc, {buf, fmt, val});
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }

        llvm::Value *LLVMCodeGen::emitBoolToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("BOOL_TO_STRING requires 1 operand");
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
                    result = builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "true_str");
                }
                else
                {
                    result = builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "false_str");
                }
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            // Convert to i1 if not already
            if (!val->getType()->isIntegerTy(1))
            {
                val = builder_->CreateICmpNE(val,
                                             llvm::ConstantInt::get(val->getType(), 0), "tobool");
            }

            // (AR) "صحيح" (true) / "خطأ" (false) بالعربية
            // (EN) "صحيح" (true) / "خطأ" (false) in Arabic
            llvm::Value *trueStr = builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "true_str");
            llvm::Value *falseStr = builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "false_str");
            llvm::Value *result = builder_->CreateSelect(val, trueStr, falseStr, "boolstr");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // (AR) تحويل مصفوفة إلى نص "[عنصر1, عنصر2, ...]"
        // (EN) Convert array to string "[elem1, elem2, ...]"
        // ============================================================================
        llvm::Value *LLVMCodeGen::emitArrayToString(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("ARRAY_TO_STRING requires 1 operand");
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
                    llvm::Value *ptrAsInt = builder_->CreateLoad(getInt64Type(), allocaInst, "ats.ptr.int");
                    arrPtr = builder_->CreateIntToPtr(ptrAsInt, llvm::PointerType::getUnqual(*context_), "ats.ptr");
                }
            }
            else if (auto *gvInst = llvm::dyn_cast<llvm::GlobalVariable>(arrPtr))
            {
                if (gvInst->getValueType()->isIntegerTy(64))
                {
                    llvm::Value *ptrAsInt = builder_->CreateLoad(getInt64Type(), gvInst, "ats.glob.int");
                    arrPtr = builder_->CreateIntToPtr(ptrAsInt, llvm::PointerType::getUnqual(*context_), "ats.glob.ptr");
                }
            }
            else if (arrPtr->getType()->isIntegerTy(64))
            {
                arrPtr = builder_->CreateIntToPtr(arrPtr, llvm::PointerType::getUnqual(*context_), "ats.raw.ptr");
            }

            auto i64Ty = getInt64Type();
            auto ptrTy = llvm::PointerType::getUnqual(*context_);

            // Ensure helper function exists
            ensureArrayToStringHelper();

            // Load array length and data from SadArray struct
            llvm::StructType *arrTy = getArrayStructType(*context_);
            llvm::Value *lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "ats.len.gep");
            llvm::Value *arrLen = builder_->CreateLoad(i64Ty, lenGep, "ats.len");
            llvm::Value *dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "ats.data.gep");
            llvm::Value *dataPtr = builder_->CreateLoad(ptrTy, dataGep, "ats.data");

            // Allocate buffer: len * 34 + 4 bytes
            llvm::Value *bufLen = builder_->CreateAdd(
                builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 34)),
                llvm::ConstantInt::get(i64Ty, 4), "ats.bufsz");

            llvm::FunctionType *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            llvm::FunctionCallee mallocFn = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = builder_->CreateCall(mallocFn, {bufLen}, "ats.buf");

            // Call __sad_array_to_string(buf, len, data)
            llvm::FunctionType *helperType = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
            llvm::FunctionCallee helperFn = module_->getOrInsertFunction("__sad_array_to_string", helperType);
            llvm::Value *result = builder_->CreateCall(helperFn, {buf, arrLen, dataPtr}, "ats.result");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitCast(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("CAST requires at least 1 operand");
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
                    result = builder_->CreateFPToSI(val, getInt64Type(), "cast_i64");
                else if (valType->isIntegerTy(1))
                    result = builder_->CreateZExt(val, getInt64Type(), "cast_i64");
                else if (valType->isPointerTy())
                    result = builder_->CreatePtrToInt(val, getInt64Type(), "cast_i64");
                break;
            case SadTypeKind::Float:
                if (valType->isIntegerTy())
                    result = builder_->CreateSIToFP(val, getDoubleType(), "cast_f64");
                break;
            case SadTypeKind::Boolean:
                if (valType->isIntegerTy(64))
                    result = builder_->CreateICmpNE(val, llvm::ConstantInt::get(getInt64Type(), 0), "cast_bool");
                else if (valType->isDoubleTy())
                    result = builder_->CreateFCmpONE(val, llvm::ConstantFP::get(getDoubleType(), 0.0), "cast_bool");
                break;
            case SadTypeKind::Pointer:
            case SadTypeKind::String:
                if (valType->isIntegerTy())
                    result = builder_->CreateIntToPtr(val, llvm::PointerType::getUnqual(*context_), "cast_ptr");
                break;
            default:
                break;
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

    } // namespace LLVM
} // namespace Sad
