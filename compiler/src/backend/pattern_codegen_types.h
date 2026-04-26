#pragma once

#include "pattern_nodes.h"
#include <functional>
#include <unordered_map>
#include <vector>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>

namespace Sad
{
    namespace Backend
    {
        struct GeneratedCaseInfo
        {
            llvm::BasicBlock *testBlock;
            llvm::BasicBlock *bodyBlock;
            llvm::BasicBlock *guardBlock;
            std::unordered_map<std::string, llvm::Value *> bindings;
        };

        using BodyCodeGenCallback = std::function<llvm::Value *(
            const std::vector<AST::StmtPtr> &body,
            const std::unordered_map<std::string, llvm::Value *> &bindings)>;

        using ExprCodeGenCallback = std::function<llvm::Value *(
            const AST::Expression &expr,
            const std::unordered_map<std::string, llvm::Value *> &bindings)>;
    }
}
