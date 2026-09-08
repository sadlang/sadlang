/*
 * ============================================================================
 * TypesCodeGen — Phase 9: تنفيذ الأنواع والثوابت
 * TypesCodeGen — Phase 9: Types & Constants Implementation
 * ============================================================================
 * (AR) منقول من llvm_codegen_init.cpp (السطور 286-605) كجزء من Phase 9.
 *      الوظائف: 17 (تحويل أنواع، 9 type accessors، 5 constant builders).
 *      الوصول للحالة: عبر cg_.context_, cg_.builder_, cg_.typeMapper_,
 *      cg_.reportError().
 * (EN) Extracted from llvm_codegen_init.cpp (lines 286-605) as part of Phase 9.
 *      Contains 17 methods: 2 type conversions + 9 type accessors + 5 constant
 *      builders. Accesses parent state via cg_.context_, cg_.builder_,
 *      cg_.typeMapper_, cg_.reportError().
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/core/types_codegen.h"
#include "llvm_type_mapper.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <string>

namespace Sad
{
    namespace LLVM
    {

        // ════════════════════════════════════════════════════════════════════════
        // (AR) 🔑 وكان ههنا هرمُ أنواعٍ **ثالثٌ** اسمُه `Sad::LLVM::Type` —
        //      قاعدةٌ مجرّدةٌ وثلاثةُ فروعٍ (`TupleType` · `TaggedUnionType` ·
        //      `FunctionPointerType`) في `llvm_type_mapper_composite.h`. وقِيسَ
        //      أنّ **لا موضعَ في المستودعِ كلِّه يُنشئُ واحدًا منها**: لا
        //      `make_shared` ولا `new`؛ وكلُّ ما كان يمسُّها `dynamic_cast` من
        //      مؤشّرٍ لا يُملأُ أبدًا — فالهرمُ كان يُقرأُ ولا يُكتَب.
        //      أمرُ القياس (⇒ صفر):
        //        grep -rEn "make_shared<TupleType>|new TupleType" compiler shared
        //      فحُذِفَ الهرمُ ومعه عنقودُه المغلق: مُحوِّلاتُ الأنواعِ الخمسةُ في
        //      LLVMTypeMapper وطريقتا التحويلِ في TypesCodeGen — لا مُنادِيَ
        //      لواحدةٍ منها من خارجِ العنقود.
        // (EN) A THIRD type hierarchy (Sad::LLVM::Type + 3 subclasses) lived
        //      here. Measured: nothing in the tree ever constructs one, so the
        //      whole closed cluster consuming it was dead code. Removed.
        // ════════════════════════════════════════════════════════════════════════


        // ====================================================================
        // (AR) الأنواع الأساسية / (EN) Primitive Types
        // ====================================================================

        llvm::Type *TypesCodeGen::getVoidType()
        {
            return llvm::Type::getVoidTy(*cg_.context_);
        }

        llvm::Type *TypesCodeGen::getInt1Type()
        {
            return llvm::Type::getInt1Ty(*cg_.context_);
        }

        llvm::Type *TypesCodeGen::getInt8Type()
        {
            return llvm::Type::getInt8Ty(*cg_.context_);
        }

        llvm::Type *TypesCodeGen::getInt16Type()
        {
            return llvm::Type::getInt16Ty(*cg_.context_);
        }

        llvm::Type *TypesCodeGen::getInt32Type()
        {
            return llvm::Type::getInt32Ty(*cg_.context_);
        }

        llvm::Type *TypesCodeGen::getInt64Type()
        {
            return llvm::Type::getInt64Ty(*cg_.context_);
        }

        llvm::Type *TypesCodeGen::getFloatType()
        {
            return llvm::Type::getFloatTy(*cg_.context_);
        }

        llvm::Type *TypesCodeGen::getDoubleType()
        {
            return llvm::Type::getDoubleTy(*cg_.context_);
        }

        llvm::Type *TypesCodeGen::getInt8PtrType()
        {
            // (AR) LLVM 15+ يستخدم المؤشرات الشفافة (opaque pointers)
            // (EN) LLVM 15+ uses opaque pointers
            return llvm::PointerType::get(*cg_.context_, 0);
        }

        // ====================================================================
        // (AR) الثوابت / (EN) Constants
        // ====================================================================

        llvm::Constant *TypesCodeGen::getConstantInt(int64_t value, int bits)
        {
            llvm::Type *type = nullptr;

            switch (bits)
            {
            case 1:
                type = getInt1Type();
                break;
            case 8:
                type = getInt8Type();
                break;
            case 16:
                type = getInt16Type();
                break;
            case 32:
                type = getInt32Type();
                break;
            case 64:
                type = getInt64Type();
                break;
            default:
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", std::string("Unsupported integer bit width:") + std::to_string(bits)}});
                return nullptr;
            }

            return llvm::ConstantInt::get(type, value, true);
        }

        llvm::Constant *TypesCodeGen::getConstantFloat(double value, bool isDouble)
        {
            if (isDouble)
            {
                return llvm::ConstantFP::get(getDoubleType(), value);
            }
            return llvm::ConstantFP::get(getFloatType(), static_cast<float>(value));
        }

        llvm::Constant *TypesCodeGen::getConstantString(const std::string &value)
        {
            return cg_.builder_->CreateGlobalStringPtr(value);
        }

        llvm::Constant *TypesCodeGen::getConstantBool(bool value)
        {
            return llvm::ConstantInt::get(getInt1Type(), value ? 1 : 0);
        }

        llvm::Constant *TypesCodeGen::getNullPtr(llvm::Type *type)
        {
            if (!type)
            {
                type = getInt8PtrType();
            }
            return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(type));
        }

    } // namespace LLVM
} // namespace Sad
