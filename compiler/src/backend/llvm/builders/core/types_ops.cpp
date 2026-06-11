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

        // ====================================================================
        // (AR) تحويل الأنواع / (EN) Type Conversions
        // ====================================================================

        /**
         * (AR) تحويل نوع Sad إلى نوع LLVM. يستخدم typeMapper_ إن وُجد،
         *      وإلا يقع على نظام تحويل يدوي احتياطي.
         * (EN) Convert Sad type to LLVM type. Uses typeMapper_ when available,
         *      otherwise falls back to manual conversion.
         */
        llvm::Type *TypesCodeGen::convertType(std::shared_ptr<Type> sadType)
        {
            if (!sadType)
            {
                return getVoidType();
            }

            // (AR) استخدام محول الأنواع LLVMTypeMapper للتحويل الصحيح
            // (EN) Use LLVMTypeMapper for proper conversion
            if (cg_.typeMapper_)
            {
                return cg_.typeMapper_->mapSadType(sadType);
            }

            // (AR) احتياطي: إذا لم يُهيَّأ محول الأنواع بعد، نستخدم التحويل اليدوي
            // (EN) Fallback: manual conversion when typeMapper_ not initialized
            if (sadType->isVoid())
                return getVoidType();
            if (sadType->isBoolean())
                return getInt1Type();
            if (sadType->isInteger())
            {
                switch (sadType->getBitWidth())
                {
                case 8:
                    return getInt8Type();
                case 16:
                    return getInt16Type();
                case 32:
                    return getInt32Type();
                case 64:
                    return getInt64Type();
                default:
                    return llvm::Type::getIntNTy(*cg_.context_, sadType->getBitWidth());
                }
            }
            if (sadType->isFloat())
            {
                return sadType->isFloat32() ? getFloatType() : getDoubleType();
            }
            if (sadType->isString())
                return getInt8PtrType();
            if (sadType->isPointer())
                return llvm::PointerType::get(*cg_.context_, 0);

            // (AR) نوع غير معروف — نُرجع i64 كاحتياطي
            // (EN) Unknown type — fallback to i64
            return getInt64Type();
        }

        /**
         * (AR) تحويل نوع دالة Sad إلى نوع دالة LLVM.
         * (EN) Convert Sad function type to LLVM function type.
         */
        llvm::FunctionType *TypesCodeGen::convertFunctionType(
            std::shared_ptr<Type> returnType,
            const std::vector<std::shared_ptr<Type>> &paramTypes,
            bool isVarArg)
        {
            // (AR) استخدام محول الأنواع إذا كان مُهيّأ
            // (EN) Use typeMapper_ when available
            if (cg_.typeMapper_)
            {
                return cg_.typeMapper_->mapFunctionType(returnType, paramTypes, isVarArg);
            }

            // (AR) احتياطي: بناء نوع الدالة يدوياً
            // (EN) Fallback: build function type manually
            llvm::Type *retType = convertType(returnType);
            std::vector<llvm::Type *> llvmParamTypes;
            llvmParamTypes.reserve(paramTypes.size());
            for (const auto &paramType : paramTypes)
            {
                llvmParamTypes.push_back(convertType(paramType));
            }
            return llvm::FunctionType::get(retType, llvmParamTypes, isVarArg);
        }

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
