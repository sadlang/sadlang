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

        llvm::Value *LLVMCodeGen::emitCmpGt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("CmpGt instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                reportError("Operands not found for cmpgt");
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
                result = builder_->CreateFCmpOGT(left, right, "cmpgttmp");
            }
            else
            {
                result = builder_->CreateICmpSGT(left, right, "cmpgttmp");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة مقارنة أكبر من أو يساوي
         * Emit greater than or equal comparison instruction
         *
         * Source: llvm_codegen.h:438
         */
        llvm::Value *LLVMCodeGen::emitCmpGe(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.size() < 2)
            {
                reportError("CmpGe instruction requires 2 operands");
                return nullptr;
            }

            llvm::Value *left = resolveOperand(inst->operands[0]);
            llvm::Value *right = resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                reportError("Operands not found for cmpge");
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
                result = builder_->CreateFCmpOGE(left, right, "cmpgetmp");
            }
            else
            {
                result = builder_->CreateICmpSGE(left, right, "cmpgetmp");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }

        /**
         * إصدار تعليمة تحميل من الذاكرة
         * Emit load from memory instruction
         *
         * Source: llvm_codegen.h:444
         */
        llvm::Value *LLVMCodeGen::emitLoad(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst)
                return nullptr;

            if (inst->operands.empty())
            {
                reportError("Load instruction requires 1 operand");
                return nullptr;
            }

            // (AR) تحميل عضو (LOAD بمعاملين مع اسم عضو نصي): نمرر إلى OBJECT_GET
            //      لا نُحوّل أي LOAD ثنائي بشكل أعمى لتجنّب كسر مسارات أخرى.
            // (EN) Member load (2-operand LOAD with string member name): delegate to OBJECT_GET
            //      Do not blindly redirect every 2-operand LOAD to avoid breaking other paths.
            if (inst->operands.size() >= 2 &&
                inst->operands[1].type == SIROperandType::CONSTANT &&
                inst->operands[1].dataType == SadTypeKind::String)
            {
                auto objectGetInst = std::make_shared<SIRInstruction>(SIROpcode::OBJECT_GET);
                objectGetInst->operands = inst->operands;
                objectGetInst->result = inst->result;
                return emitObjectGet(objectGetInst);
            }

            // (AR) حالة كائن heap مخزن كمؤشر مباشر: ارجع عنوانه كـ i64
            // (EN) Heap-object direct pointer case: return pointer address as i64
            {
                std::string opName = inst->operands[0].name;
                auto objIt = context_info_.objectClassMap.find(opName);
                if (objIt != context_info_.objectClassMap.end())
                {
                    llvm::Value *objVal = context_info_.namedValues[opName];
                    if (objVal && objVal->getType()->isPointerTy() && !llvm::isa<llvm::AllocaInst>(objVal) && !llvm::isa<llvm::GlobalVariable>(objVal))
                    {
                        llvm::Value *result = builder_->CreatePtrToInt(objVal, getInt64Type(), "obj.ptr2i");
                        if (inst->result.has_value())
                        {
                            context_info_.namedValues[inst->result->name] = result;
                            context_info_.objectClassMap[inst->result->name] = objIt->second;
                        }
                        return result;
                    }
                }
            }

            // Try namedValues first (for alloca pointers), then resolveOperand
            llvm::Value *ptr = context_info_.namedValues[inst->operands[0].name];
            if (!ptr)
            {
                ptr = resolveOperand(inst->operands[0]);
            }

            if (!ptr)
            {
                reportError("Pointer not found for load");
                return nullptr;
            }

            // (AR) إذا لم يكن مؤشراً (مثل ثابت مخزن مباشرة)، أرجعه كما هو
            // (EN) If not a pointer (e.g. constant stored directly), return as-is
            if (!ptr->getType()->isPointerTy())
            {
                if (inst->result.has_value())
                {
                    context_info_.namedValues[inst->result->name] = ptr;
                }
                return ptr;
            }

            // (AR) تحديد نوع التحميل
            // (EN) Determine load type
            llvm::Type *loadType = getInt64Type();
            if (auto *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(ptr))
            {
                loadType = allocaInst->getAllocatedType();
            }
            else if (auto *gv = llvm::dyn_cast<llvm::GlobalVariable>(ptr))
            {
                loadType = gv->getValueType();
            }
            else if (inst->operands[0].dataType == SadTypeKind::Float)
            {
                loadType = getDoubleType();
            }
            else if (inst->operands[0].dataType == SadTypeKind::Boolean)
            {
                loadType = llvm::Type::getInt1Ty(*context_);
            }
            else if (inst->operands[0].dataType == SadTypeKind::String ||
                     inst->operands[0].dataType == SadTypeKind::Pointer)
            {
                loadType = llvm::PointerType::getUnqual(*context_);
            }

            llvm::Value *result = builder_->CreateLoad(loadType, ptr, "loadtmp");

            if (inst->operands[0].name.find("volatile") != std::string::npos ||
                inst->operands[0].name.find("\xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd9\x8a\xd8\xb1") != std::string::npos)
            {
                if (auto *loadInst = llvm::dyn_cast<llvm::LoadInst>(result))
                {
                    loadInst->setVolatile(true);
                }
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;

                std::string srcName = inst->operands[0].name;
                auto classIt = context_info_.objectClassMap.find(srcName);
                if (classIt != context_info_.objectClassMap.end())
                {
                    context_info_.objectClassMap[inst->result->name] = classIt->second;
                }
            }

            return result;
        }

    } // namespace LLVM
} // namespace Sad
