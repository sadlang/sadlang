// ============================================================================
// llvm_codegen_arithmetic_cmp.cpp — عمليات الإزاحة والمقارنة
// LLVM IR: Shift operations and comparison operators
// ============================================================================
// تم استخراج هذا الملف من llvm_codegen_arithmetic.cpp وفقاً لقاعدة CW-05
// ============================================================================
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

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {


        llvm::Value *LLVMCodeGen::emitShl(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("Shl instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                reportError("Operands not found for shl");
                return nullptr;
            }

            // (AR) تطبيع الأنواع: تحويل ptr→i64 و i1→i64 و double→i64
            // (EN) Type normalization: convert ptr→i64 and i1→i64 and double→i64
            if (left->getType()->isPointerTy())
                left = builder_->CreatePtrToInt(left, getInt64Type(), "shl.l.ptoi");
            if (right->getType()->isPointerTy())
                right = builder_->CreatePtrToInt(right, getInt64Type(), "shl.r.ptoi");
            if (left->getType()->isIntegerTy(1))
                left = builder_->CreateZExt(left, getInt64Type(), "shl.l.zext");
            if (right->getType()->isIntegerTy(1))
                right = builder_->CreateZExt(right, getInt64Type(), "shl.r.zext");
            if (left->getType()->isDoubleTy())
                left = builder_->CreateFPToSI(left, getInt64Type(), "shl.l.f2i");
            if (right->getType()->isDoubleTy())
                right = builder_->CreateFPToSI(right, getInt64Type(), "shl.r.f2i");

            llvm::Value *result = builder_->CreateShl(left, right, "shltmp");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة إزاحة يمين
         * Emit shift right instruction
         *
         * Source: llvm_codegen.h:427
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *LLVMCodeGen::emitShr(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("Shr instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                reportError("Operands not found for shr");
                return nullptr;
            }

            // (AR) تطبيع الأنواع: تحويل ptr→i64 و i1→i64 و double→i64
            // (EN) Type normalization: convert ptr→i64 and i1→i64 and double→i64
            if (left->getType()->isPointerTy())
                left = builder_->CreatePtrToInt(left, getInt64Type(), "shr.l.ptoi");
            if (right->getType()->isPointerTy())
                right = builder_->CreatePtrToInt(right, getInt64Type(), "shr.r.ptoi");
            if (left->getType()->isIntegerTy(1))
                left = builder_->CreateZExt(left, getInt64Type(), "shr.l.zext");
            if (right->getType()->isIntegerTy(1))
                right = builder_->CreateZExt(right, getInt64Type(), "shr.r.zext");
            if (left->getType()->isDoubleTy())
                left = builder_->CreateFPToSI(left, getInt64Type(), "shr.l.f2i");
            if (right->getType()->isDoubleTy())
                right = builder_->CreateFPToSI(right, getInt64Type(), "shr.r.f2i");

            llvm::Value *result = builder_->CreateLShr(left, right, "shrtmp");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        // ============================================================================
        // المرحلة 5: تعليمات المقارنة والذاكرة
        // Phase 5: Comparison & Memory Instructions
        // ============================================================================

        /**
         * إصدار تعليمة مقارنة متساوي
         * Emit equal comparison instruction
         *
         * Source: llvm_codegen.h:433
         * @param inst تعليمة SIR / SIR instruction
         * @return قيمة LLVM / LLVM value
         */
        llvm::Value *LLVMCodeGen::emitCmpEq(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("CmpEq instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                reportError("Operands not found for cmpeq: " + inst->operands[0].name + ", " + inst->operands[1].name);
                return nullptr;
            }

            llvm::Value *result = nullptr;
            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة مقارنة المؤشرات/النصوص أولاً (حتى لو الأنواع متطابقة)
            // (EN) Handle pointer/string comparison first (even if types match)
            if (leftTy->isPointerTy() && rightTy->isPointerTy())
            {
                // (AR) كلاهما مؤشران - استخدام strcmp لمقارنة النصوص
                // (EN) Both pointers - use strcmp for string comparison
                bool isStringCmp = (inst->operands[0].dataType == SadTypeKind::String ||
                                    inst->operands[1].dataType == SadTypeKind::String);
                if (isStringCmp)
                {
                    llvm::FunctionType *strcmpType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(*context_),
                        {llvm::PointerType::getUnqual(*context_), llvm::PointerType::getUnqual(*context_)},
                        false);
                    llvm::FunctionCallee strcmpFn = module_->getOrInsertFunction("strcmp", strcmpType);
                    llvm::Value *cmpResult = builder_->CreateCall(strcmpFn, {left, right}, "strcmp.ret");
                    result = builder_->CreateICmpEQ(cmpResult,
                                                    llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), "streq");
                }
                else
                {
                    result = builder_->CreateICmpEQ(left, right, "cmpeqtmp");
                }
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            {
                // (AR) تطبيع المؤشرات أولاً: ptr→i64
                // (EN) Normalize pointers first: ptr→i64
                if (leftTy->isPointerTy())
                {
                    left = builder_->CreatePtrToInt(left, getInt64Type(), "ptr2i64.l");
                    leftTy = left->getType();
                }
                if (rightTy->isPointerTy())
                {
                    right = builder_->CreatePtrToInt(right, getInt64Type(), "ptr2i64.r");
                    rightTy = right->getType();
                }
                if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = builder_->CreateZExt(right, leftTy, "zext_r");
                }
                else if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = builder_->CreateSIToFP(left, rightTy, "sitofp_l");
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = builder_->CreateSIToFP(right, leftTy, "sitofp_r");
                }
                // (AR) تحديث الأنواع بعد التحويل
                // (EN) Update types after conversion
                leftTy = left->getType();
                rightTy = right->getType();
            }

            // (AR) مقارنة حسب النوع
            // (EN) Compare based on type
            if (leftTy->isDoubleTy() && rightTy->isDoubleTy())
            {
                result = builder_->CreateFCmpOEQ(left, right, "cmpeqtmp");
            }
            else
            {
                result = builder_->CreateICmpEQ(left, right, "cmpeqtmp");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة مقارنة غير متساوي
         * Emit not equal comparison instruction
         *
         * Source: llvm_codegen.h:434
         */
        llvm::Value *LLVMCodeGen::emitCmpNe(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("CmpNe instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                reportError("Operands not found for cmpne");
                return nullptr;
            }

            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة مقارنة النصوص
            // (EN) Handle string comparison
            if (leftTy->isPointerTy() && rightTy->isPointerTy())
            {
                bool isStringCmp = (inst->operands[0].dataType == SadTypeKind::String ||
                                    inst->operands[1].dataType == SadTypeKind::String);
                if (isStringCmp)
                {
                    llvm::FunctionType *strcmpType = llvm::FunctionType::get(
                        llvm::Type::getInt32Ty(*context_),
                        {llvm::PointerType::getUnqual(*context_), llvm::PointerType::getUnqual(*context_)},
                        false);
                    llvm::FunctionCallee strcmpFn = module_->getOrInsertFunction("strcmp", strcmpType);
                    llvm::Value *cmpResult = builder_->CreateCall(strcmpFn, {left, right}, "strcmp.ret");
                    llvm::Value *result = builder_->CreateICmpNE(cmpResult,
                                                                 llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), "strne");
                    if (inst->result.has_value())
                        context_info_.namedValues[inst->result->name] = result;
                    return result;
                }
            }

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            {
                if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = builder_->CreateZExt(right, leftTy, "zext_r");
                }
                else if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = builder_->CreateSIToFP(left, rightTy, "sitofp_l");
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = builder_->CreateSIToFP(right, leftTy, "sitofp_r");
                }
                else if (leftTy->isPointerTy())
                {
                    left = builder_->CreatePtrToInt(left, getInt64Type(), "ptr2i64.l");
                }
                else if (rightTy->isPointerTy())
                {
                    right = builder_->CreatePtrToInt(right, getInt64Type(), "ptr2i64.r");
                }
                leftTy = left->getType();
                rightTy = right->getType();
            }

            llvm::Value *result;
            if (leftTy->isDoubleTy() && rightTy->isDoubleTy())
            {
                result = builder_->CreateFCmpONE(left, right, "cmpnetmp");
            }
            else
            {
                result = builder_->CreateICmpNE(left, right, "cmpnetmp");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة مقارنة أقل من
         * Emit less than comparison instruction
         *
         * Source: llvm_codegen.h:435
         */
        llvm::Value *LLVMCodeGen::emitCmpLt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("CmpLt instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                reportError("Operands not found for cmplt");
                return nullptr;
            }

            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            { // (AR) تطبيع المؤشرات أولاً: ptr→i64
                // (EN) Normalize pointers first: ptr→i64
                if (leftTy->isPointerTy())
                {
                    left = builder_->CreatePtrToInt(left, getInt64Type(), "ptr2i64.l");
                    leftTy = left->getType();
                }
                if (rightTy->isPointerTy())
                {
                    right = builder_->CreatePtrToInt(right, getInt64Type(), "ptr2i64.r");
                    rightTy = right->getType();
                }
                if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = builder_->CreateSIToFP(left, rightTy, "sitofp_l");
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = builder_->CreateSIToFP(right, leftTy, "sitofp_r");
                }
                else if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = builder_->CreateZExt(right, leftTy, "zext_r");
                }
                leftTy = left->getType();
                rightTy = right->getType();
            }

            llvm::Value *result;
            if (leftTy->isDoubleTy() && rightTy->isDoubleTy())
            {
                result = builder_->CreateFCmpOLT(left, right, "cmplttmp");
            }
            else
            {
                result = builder_->CreateICmpSLT(left, right, "cmplttmp");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة مقارنة أقل من أو يساوي
         * Emit less than or equal comparison instruction
         *
         * Source: llvm_codegen.h:436
         */
        llvm::Value *LLVMCodeGen::emitCmpLe(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("CmpLe instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                reportError("Operands not found for cmple");
                return nullptr;
            }

            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) معالجة عدم تطابق الأنواع
            // (EN) Handle type mismatches
            if (leftTy != rightTy)
            {
                // (AR) تطبيع المؤشرات أولاً: ptr→i64
                // (EN) Normalize pointers first: ptr→i64
                if (leftTy->isPointerTy())
                {
                    left = builder_->CreatePtrToInt(left, getInt64Type(), "ptr2i64.l");
                    leftTy = left->getType();
                }
                if (rightTy->isPointerTy())
                {
                    right = builder_->CreatePtrToInt(right, getInt64Type(), "ptr2i64.r");
                    rightTy = right->getType();
                }
                if (leftTy->isIntegerTy() && rightTy->isDoubleTy())
                {
                    left = builder_->CreateSIToFP(left, rightTy, "sitofp_l");
                }
                else if (leftTy->isDoubleTy() && rightTy->isIntegerTy())
                {
                    right = builder_->CreateSIToFP(right, leftTy, "sitofp_r");
                }
                else if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64))
                {
                    left = builder_->CreateZExt(left, rightTy, "zext_l");
                }
                else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1))
                {
                    right = builder_->CreateZExt(right, leftTy, "zext_r");
                }
                leftTy = left->getType();
                rightTy = right->getType();
            }

            llvm::Value *result;
            if (leftTy->isDoubleTy() && rightTy->isDoubleTy())
            {
                result = builder_->CreateFCmpOLE(left, right, "cmpletmp");
            }
            else
            {
                result = builder_->CreateICmpSLE(left, right, "cmpletmp");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة مقارنة أكبر من
         * Emit greater than comparison instruction
         *
         * Source: llvm_codegen.h:437
         */


    } // namespace LLVM
} // namespace Sad

