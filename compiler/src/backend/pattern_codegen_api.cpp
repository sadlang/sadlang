#include "pattern_codegen_internal.h"

namespace Sad
{
    namespace Backend
    {
        llvm::Value *generateMatchCode(
            const AST::MatchStmt &matchStmt,
            llvm::Value *matchedValue,
            llvm::Function *currentFunction,
            llvm::LLVMContext &context,
            llvm::Module &module,
            llvm::IRBuilder<> &builder,
            BodyCodeGenCallback bodyCallback,
            ExprCodeGenCallback exprCallback)
        {
            return generateMatchCodeInternal(
                matchStmt,
                matchedValue,
                currentFunction,
                context,
                module,
                builder,
                std::move(bodyCallback),
                std::move(exprCallback));
        }

        llvm::Value *generateMatchCode(
            const AST::MatchStmt &matchStmt,
            llvm::Value *matchedValue,
            llvm::Function *currentFunction,
            llvm::LLVMContext &context,
            llvm::Module &module,
            llvm::IRBuilder<> &builder)
        {
            return generateMatchCodeInternal(
                matchStmt,
                matchedValue,
                currentFunction,
                context,
                module,
                builder,
                nullptr,
                nullptr);
        }
    }
}
