#pragma once

#include "pattern_codegen_types.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

namespace Sad
{
    namespace Backend
    {
        llvm::Value *generateMatchCodeInternal(
            const AST::MatchStmt &matchStmt,
            llvm::Value *matchedValue,
            llvm::Function *currentFunction,
            llvm::LLVMContext &context,
            llvm::Module &module,
            llvm::IRBuilder<> &builder,
            BodyCodeGenCallback bodyCallback,
            ExprCodeGenCallback exprCallback);
    }
}
