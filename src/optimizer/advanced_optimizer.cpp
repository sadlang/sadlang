#include "optimizer/advanced_optimizer.h"
#include "optimizer/optimization_pass.h"
#include <chrono>
#include <iostream>

namespace sad {

AdvancedOptimizer::AdvancedOptimizer() {
    // إضافة ممرات التحسين الافتراضية
    // Add default optimization passes
    
    // Level 0: Basic optimizations (always safe)
    addOptimizationPass(std::make_unique<ConstantFoldingPass>());
    
    // Level 1: Moderate optimizations (safe)
    addOptimizationPass(std::make_unique<DeadCodeEliminationPass>());
    addOptimizationPass(std::make_unique<ExpressionSimplificationPass>());
    addOptimizationPass(std::make_unique<RedundantAssignmentEliminationPass>());
    
    // Level 2: Aggressive optimizations
    addOptimizationPass(std::make_unique<LoopOptimizationPass>());
    addOptimizationPass(std::make_unique<FunctionInliningPass>());
    
    // تفعيل جميع التحسينات الآمنة افتراضياً
    for (const auto& pass : passes_) {
        enabledOptimizations_[pass->getName()] = pass->isSafe();
    }
}

AdvancedOptimizer::~AdvancedOptimizer() = default;

std::shared_ptr<ASTNode> AdvancedOptimizer::optimize(std::shared_ptr<ASTNode> ast) {
    if (!ast) {
        return nullptr;
    }

    auto startTime = std::chrono::high_resolution_clock::now();
    
    // تطبيق جميع الممرات المفعلة
    auto optimizedAst = applyAllPasses(ast);
    
    // التحقق من صحة الـ AST
    if (!validateAST(optimizedAst)) {
        std::cerr << "تحذير: فشل التحقق من صحة AST بعد التحسين، سيتم إرجاع AST الأصلي\n";
        std::cerr << "Warning: AST validation failed after optimization, returning original AST\n";
        return ast;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = endTime - startTime;
    stats_.optimizationTime = elapsed.count();
    
    return optimizedAst;
}

void AdvancedOptimizer::addOptimizationPass(std::unique_ptr<OptimizationPass> pass) {
    if (pass) {
        enabledOptimizations_[pass->getName()] = pass->isSafe();
        passes_.push_back(std::move(pass));
    }
}

void AdvancedOptimizer::enableOptimization(const std::string& name, bool enabled) {
    enabledOptimizations_[name] = enabled;
}

void AdvancedOptimizer::resetStatistics() {
    stats_ = OptimizationStats();
}

std::shared_ptr<ASTNode> AdvancedOptimizer::applyAllPasses(std::shared_ptr<ASTNode> ast) {
    auto currentAst = ast;
    
    // تطبيق كل ممر مفعل
    for (const auto& pass : passes_) {
        const std::string& passName = pass->getName();
        
        if (enabledOptimizations_[passName]) {
            std::cout << "تطبيق تحسين: " << pass->getDescription() << "\n";
            std::cout << "Applying optimization: " << passName << "\n";
            
            try {
                auto optimizedAst = pass->apply(currentAst);
                if (optimizedAst) {
                    currentAst = optimizedAst;
                    stats_.totalPasses++;
                }
            } catch (const std::exception& e) {
                std::cerr << "خطأ في تطبيق التحسين " << passName << ": " << e.what() << "\n";
                std::cerr << "Error applying optimization " << passName << ": " << e.what() << "\n";
            }
        }
    }
    
    return currentAst;
}

bool AdvancedOptimizer::validateAST(std::shared_ptr<ASTNode> ast) {
    if (!ast) {
        return false;
    }
    
    // التحقق الأساسي من صحة الـ AST
    // Basic AST validation
    
    // TODO: إضافة تحققات أكثر تفصيلاً
    // TODO: Add more detailed validation checks
    
    return true;
}

} // namespace sad
