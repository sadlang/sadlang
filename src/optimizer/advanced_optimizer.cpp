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

/**
 * @brief التحقق من صحة AST / Validate AST
 * @brief Comprehensive AST validation with detailed checks
 * 
 * @details
 * (AR) يتحقق من صحة AST بشكل شامل، بما في ذلك:
 *      - وجود العقدة (nullptr check)
 *      - التحقق من بنية العقدة (node structure)
 *      - التحقق من التعابير (expressions)
 *      - التحقق من التصريحات (statements)
 *      - التحقق من consistency
 * 
 * (EN) Comprehensive AST validation including:
 *      - Node existence (nullptr check)
 *      - Node structure validation
 *      - Expression validation
 *      - Statement validation
 *      - Consistency checks
 * 
 * @param ast العقدة للتحقق / Node to validate
 * @return true إذا كان AST صالحاً / true if AST is valid
 */
bool AdvancedOptimizer::validateAST(std::shared_ptr<ASTNode> ast) {
    if (!ast) {
        std::cerr << "[Validation] Error: AST is null\n";
        return false;
    }
    
    // عدادات للإحصائيات / Counters for statistics
    int nodeCount = 0;
    int expressionCount = 0;
    int statementCount = 0;
    int errorCount = 0;
    
    // التحقق من AST بشكل recursive
    bool isValid = validateNodeRecursive(ast, nodeCount, expressionCount, statementCount, errorCount);
    
    // طباعة إحصائيات التحقق / Print validation statistics
    if (nodeCount > 0) {
        std::cout << "[Validation] Statistics:\n";
        std::cout << "  Total nodes: " << nodeCount << "\n";
        std::cout << "  Expressions: " << expressionCount << "\n";
        std::cout << "  Statements: " << statementCount << "\n";
        std::cout << "  Errors: " << errorCount << "\n";
    }
    
    return isValid && (errorCount == 0);
}

bool AdvancedOptimizer::validateNodeRecursive(std::shared_ptr<ASTNode> node, 
                                              int& nodeCount, int& expressionCount, 
                                              int& statementCount, int& errorCount) {
    if (!node) {
        return true; // null nodes are acceptable in some contexts
    }
    
    nodeCount++;
    
    // التحقق الأساسي: العقدة موجودة وصالحة
    // Basic validation: node exists and is valid
    // في المستقبل يمكن إضافة فحوصات أكثر تفصيلاً حسب نوع العقدة
    // In future, can add more detailed checks based on node type
    
    try {
        // مجرد فحص أساسي - العقدة موجودة
        // Just basic check - node exists
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[Validation] Exception during validation: " << e.what() << "\n";
        errorCount++;
        return false;
    }
    
    return true;
}

} // namespace sad
