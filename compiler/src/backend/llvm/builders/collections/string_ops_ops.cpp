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
#include "builders/collections/strings_codegen.h"
#include "sir_constants.h" // (AR) kSadNullSentinel لوعي نوع() بـعدم زمن التشغيل
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

        // Helper function for array struct type
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

        llvm::Value *StringsCodeGen::emitBuiltinTypeOf(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                return cg_.builder_->CreateGlobalStringPtr("مجهول", "typeof_unknown");
            }

            // Determine type from operand's dataType at compile time
            const char *typeName = "مجهول";
            switch (inst->operands[0].dataType)
            {
            case SadTypeKind::Integer:
                typeName = "عدد_صحيح";
                break;
            case SadTypeKind::Float:
                typeName = "عدد_عشري";
                break;
            case SadTypeKind::Boolean:
                typeName = "منطقي";
                break;
            case SadTypeKind::String:
                typeName = "نص";
                break;
            case SadTypeKind::Array:
                typeName = "مصفوفة";
                break;
            case SadTypeKind::Struct:
                typeName = "كائن";
                break;
            case SadTypeKind::Pointer:
                typeName = "مؤشر";
                break;
            case SadTypeKind::Void:
                typeName = "فراغ";
                break;
            case SadTypeKind::Null:
                // (AR) عدم (Null) متمايز عن فراغ (Void) — S-TS-P1/P4
                // (EN) null (Null) is distinct from void (Void) — S-TS-P1/P4
                typeName = "عدم";
                break;
            default:
                typeName = "مجهول";
                break;
            }

            llvm::Value *staticStr = cg_.builder_->CreateGlobalStringPtr(typeName, "typeof_str");

            // ================================================================
            // (AR) [S-TS-P4 codegen] وعي زمن-التشغيل بـعدم (Null):
            //      قيمة i64 قد تحمل حارس Null (kSadNullSentinel) حتى لو كان نوعها
            //      الساكن «عدد_صحيح» — مثل متغير اختياري `رقم؟` أُسنِد إليه `لاشيء`.
            //      نُصدر اختيارًا زمن-التشغيل: (القيمة == الحارس) ? «عدم» : الاسم الساكن.
            //      الأنواع غير i64 (عشري/نص/مصفوفة/منطقي) لا يمكن أن تحمل الحارس → تتخطّى.
            // (EN) [S-TS-P4 codegen] Runtime null awareness: an i64 value may carry the
            //      Null sentinel even when its static type is «عدد_صحيح» (e.g. an optional
            //      `رقم؟` assigned `لاشيء`). Emit a runtime select: (val == sentinel)
            //      ? «عدم» : static-name. Non-i64 types can't hold the sentinel → skipped.
            // ================================================================
            llvm::Value *result = staticStr;
            llvm::Value *val = cg_.resolveOperand(inst->operands[0]);
            if (val && val->getType()->isIntegerTy(64))
            {
                auto *i64Ty = cg_.getInt64Type();
                llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                    val, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel), "typeof.isnull");
                llvm::Value *nullStr = cg_.builder_->CreateGlobalStringPtr("عدم", "typeof_null");
                result = cg_.builder_->CreateSelect(isNull, nullStr, staticStr, "typeof.sel");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

    } // namespace LLVM
} // namespace Sad
