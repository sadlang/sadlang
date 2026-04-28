/*
 * ============================================================================
 * TypesCodeGen — Phase 9: 17 method(s)
 * ============================================================================
 * (AR) sub-codegen لتوليد الأنواع والثوابت والتحويلات (Type accessors,
 *      constant builders, type conversions). يحوّل أنواع Sad إلى أنواع LLVM
 *      ويوفر منشئات للثوابت الأساسية (i1/i8/i32/i64/float/double/string/null).
 * (EN) Sub-codegen for type accessors, constant builders, and type conversions.
 *      Converts Sad types to LLVM types and provides primitive constant
 *      builders (i1/i8/i32/i64/float/double/string/null).
 * ============================================================================
 */
#ifndef SAD_LLVM_TYPES_CODEGEN_H
#define SAD_LLVM_TYPES_CODEGEN_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>

namespace Sad {

    // (AR) إعلان أمامي لنوع Sad / (EN) Forward declaration of Sad Type
    class Type;

    namespace LLVM {

        class LLVMCodeGen;

        /**
         * (AR) مكوّن فرعي للأنواع والثوابت — Phase 9
         * (EN) Types & Constants sub-codegen — Phase 9
         */
        class TypesCodeGen
        {
            LLVMCodeGen &cg_;

        public:
            explicit TypesCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
            TypesCodeGen(const TypesCodeGen &) = delete;
            TypesCodeGen &operator=(const TypesCodeGen &) = delete;

            // ====================================================================
            // (AR) تحويل الأنواع / (EN) Type Conversions
            // ====================================================================
            llvm::Type *convertType(std::shared_ptr<Type> sadType);
            llvm::FunctionType *convertFunctionType(
                std::shared_ptr<Type> returnType,
                const std::vector<std::shared_ptr<Type>> &paramTypes,
                bool isVarArg = false);

            // ====================================================================
            // (AR) الأنواع الأساسية / (EN) Primitive Types
            // ====================================================================
            llvm::Type *getVoidType();
            llvm::Type *getInt1Type();
            llvm::Type *getInt8Type();
            llvm::Type *getInt16Type();
            llvm::Type *getInt32Type();
            llvm::Type *getInt64Type();
            llvm::Type *getFloatType();
            llvm::Type *getDoubleType();
            llvm::Type *getInt8PtrType();

            // ====================================================================
            // (AR) الثوابت / (EN) Constants
            // ====================================================================
            llvm::Constant *getConstantInt(int64_t value, int bits = 64);
            llvm::Constant *getConstantFloat(double value, bool isDouble = false);
            llvm::Constant *getConstantString(const std::string &value);
            llvm::Constant *getConstantBool(bool value);
            llvm::Constant *getNullPtr(llvm::Type *type);
        };

    } // namespace LLVM
} // namespace Sad

#endif // SAD_LLVM_TYPES_CODEGEN_H
