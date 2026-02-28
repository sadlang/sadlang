/**
 * @file optimizer.cpp
 * @brief تطبيق محسّن التمثيل الوسيط SIR
 * @brief SIR Intermediate Representation Optimizer Implementation
 * 
 * @details
 * (AR) يطبق تمريرات التحسين المتعددة على وحدة SIR حسب المستوى المحدد.
 * (EN) Applies multiple optimization passes to SIR module based on level.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "middle/optimizer.h"
#include "middle/constant_folding_pass.h"
#include "middle/dead_code_elimination_pass.h"
#include "middle/cse_pass.h"
#include "middle/copy_propagation_pass.h"
#include "middle/register_coalescing_pass.h"
#include "middle/licm_pass.h"
#include "middle/strength_reduction_pass.h"
#include <iostream>
#include <iomanip>
#include <chrono>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief Constructor / البناء
 */
Optimizer::Optimizer(OptimizationLevel level)
    : level_(level)
    , passManager_(std::make_unique<PassManager>())
    , debugMode_(false)
{
    initializePasses();
}

/**
 * @brief Destructor / الهدم
 */
Optimizer::~Optimizer() = default;

/**
 * @brief تحسين وحدة SIR كاملة
 * @brief Optimize complete SIR module
 */
bool Optimizer::optimize(std::shared_ptr<SIR::SIRModule> module) {
    if (!module) {
        std::cerr << "Error: Null module passed to optimizer\n";
        return false;
    }
    
    // إعادة تعيين الإحصائيات / Reset statistics
    stats_ = OptimizationStats();
    stats_.originalInstructionCount = 0;
    stats_.optimizedInstructionCount = 0;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (debugMode_) {
        std::cout << "\n╔════════════════════════════════════════════════════╗\n";
        std::cout << "║         SIR Optimizer / محسّن SIR                 ║\n";
        std::cout << "╠════════════════════════════════════════════════════╣\n";
        std::cout << "║  Optimization Level: O" << static_cast<int>(level_) 
                  << "                              ║\n";
        std::cout << "║  Module: " << std::left << std::setw(38) 
                  << module->name << " ║\n";
        std::cout << "╚════════════════════════════════════════════════════╝\n\n";
        
        passManager_->printPassInfo();
    }
    
    // جمع إحصائيات ما قبل التحسين / Collect pre-optimization stats
    collectStats(module);
    int originalCount = stats_.originalInstructionCount;
    
    // تحسين كل دالة / Optimize each function
    bool modified = false;
    auto functions = module->getFunctions();
    
    for (auto& func : functions) {
        if (func) {
            bool funcModified = optimizeFunction(func.get());
            if (funcModified) {
                modified = true;
            }
        }
    }
    
    // جمع إحصائيات ما بعد التحسين / Collect post-optimization stats
    collectStats(module);
    stats_.instructionsRemoved = originalCount - stats_.optimizedInstructionCount;
    
    // حساب وقت التحسين / Calculate optimization time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - startTime
    );
    stats_.optimizationTimeMs = duration.count();
    
    // نسخ إحصائيات التمريرات / Copy pass statistics
    stats_.passStats = passManager_->getPassStats();
    
    if (debugMode_) {
        printStats();
    }
    
    return modified;
}

/**
 * @brief تحسين دالة واحدة
 * @brief Optimize single function
 */
bool Optimizer::optimizeFunction(SIR::SIRFunction* function) {
    if (!function) {
        return false;
    }
    
    if (debugMode_) {
        std::cout << "\n→ Optimizing function: " << function->getName() << "\n";
    }
    
    // تطبيق جميع التمريرات / Apply all passes
    bool modified = passManager_->runOnFunction(function);
    
    return modified;
}

/**
 * @brief إضافة تمرير تحسين مخصص
 * @brief Add custom optimization pass
 */
void Optimizer::addPass(std::unique_ptr<OptimizationPass> pass) {
    if (pass && passManager_) {
        passManager_->addPass(std::move(pass));
    }
}

/**
 * @brief طباعة إحصائيات التحسين
 * @brief Print optimization statistics
 */
void Optimizer::printStats() const {
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    std::cout << "║      Optimization Statistics / إحصائيات التحسين   ║\n";
    std::cout << "╠════════════════════════════════════════════════════╣\n";
    std::cout << "║  Original Instructions:   " << std::setw(23) 
              << stats_.originalInstructionCount << " ║\n";
    std::cout << "║  Optimized Instructions:  " << std::setw(23)
              << stats_.optimizedInstructionCount << " ║\n";
    std::cout << "║  Instructions Removed:    " << std::setw(23)
              << stats_.instructionsRemoved << " ║\n";
    std::cout << "║  Improvement:             " << std::setw(20)
              << std::fixed << std::setprecision(2) 
              << stats_.getImprovementPercentage() << "% ║\n";
    std::cout << "║  Optimization Time:       " << std::setw(19)
              << stats_.optimizationTimeMs << " ms ║\n";
    std::cout << "╠════════════════════════════════════════════════════╣\n";
    
    if (!stats_.passStats.empty()) {
        std::cout << "║  Pass Statistics:                                  ║\n";
        for (const auto& passStats : stats_.passStats) {
            std::cout << "║    • " << std::left << std::setw(30) 
                      << passStats.passName
                      << " : " << std::setw(8) << passStats.modificationsCount
                      << " ║\n";
        }
    }
    
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
}

/**
 * @brief التحقق من صحة التحسين
 * @brief Verify optimization correctness
 */
bool Optimizer::verify(std::shared_ptr<SIR::SIRModule> module) const {
    if (!module) {
        return false;
    }
    
    // تطبيق التحقق من الدلالات / Implement semantic verification
    bool valid = true;
    
    // 1. التحقق من الدوال / Verify functions
    const auto& functions = module->getFunctions();
    for (const auto& func : functions) {
        if (!func) {
            std::cerr << "[Optimizer] Error: Null function pointer\n";
            valid = false;
            continue;
        }
        
        // التحقق من صحة الدالة / Validate function
        if (!func->validate()) {
            std::cerr << "[Optimizer] Error: Invalid function '" << func->getName() << "'\n";
            valid = false;
        }
        
        // التحقق من الكتل الأساسية / Verify basic blocks
        const auto& blocks = func->getBasicBlocks();
        if (blocks.empty()) {
            std::cerr << "[Optimizer] Warning: Function '" << func->getName() 
                     << "' has no basic blocks\n";
        }
        
        for (const auto& block : blocks) {
            if (!block) {
                std::cerr << "[Optimizer] Error: Null basic block in function '" 
                         << func->getName() << "'\n";
                valid = false;
                continue;
            }
            
            // التحقق من التعليمات / Verify instructions
            if (block->instructions.empty()) {
                std::cerr << "[Optimizer] Warning: Empty basic block '" 
                         << block->name << "' in function '" << func->getName() << "'\n";
            }
            
            // التحقق من terminator / Verify terminator
            if (!block->instructions.empty()) {
                const auto& lastInst = block->instructions.back();
                if (lastInst.isTerminatorInst()) {
                    // جيد - آخر تعليمة terminator / Good - last instruction is terminator
                } else {
                    std::cerr << "[Optimizer] Error: Basic block '" << block->name 
                             << "' missing terminator instruction\n";
                    valid = false;
                }
            }
        }
    }
    
    // 2. التحقق من المتغيرات العامة / Verify global variables
    // يمكن إضافة فحوصات إضافية هنا / Can add more checks here
    
    if (valid && debugMode_) {
        std::cout << "[Optimizer] Semantic verification passed for module\n";
    }
    
    return valid;
}

/**
 * @brief تهيئة التمريرات حسب المستوى
 * @brief Initialize passes based on level
 */
void Optimizer::initializePasses() {
    if (!passManager_) {
        return;
    }
    
    switch (level_) {
        case OptimizationLevel::O0:
            // لا توجد تحسينات / No optimizations
            break;
            
        case OptimizationLevel::O1:
            addLevel1Passes();
            break;
            
        case OptimizationLevel::O2:
            addLevel1Passes();
            addLevel2Passes();
            break;
            
        case OptimizationLevel::O3:
            addLevel1Passes();
            addLevel2Passes();
            addLevel3Passes();
            break;
    }
}

/**
 * @brief إضافة تمريرات المستوى 1
 * @brief Add level 1 passes
 * 
 * @details
 * (AR) تحسينات أساسية: طي الثوابت وإزالة الكود الميت.
 * (EN) Basic optimizations: constant folding and dead code elimination.
 */
void Optimizer::addLevel1Passes() {
    // طي الثوابت / Constant Folding
    addPass(std::make_unique<ConstantFoldingPass>());
    
    // إزالة الكود الميت (أساسي) / Dead Code Elimination (basic)
    addPass(std::make_unique<DeadCodeEliminationPass>());
}

/**
 * @brief إضافة تمريرات المستوى 2
 * @brief Add level 2 passes
 * 
 * @details
 * (AR) تحسينات قياسية: CSE ونشر النسخ وتقليل القوة.
 * (EN) Standard optimizations: CSE, copy propagation, and strength reduction.
 */
void Optimizer::addLevel2Passes() {
    // القضاء على التعبيرات الفرعية المشتركة / Common Subexpression Elimination
    addPass(std::make_unique<CSEPass>());
    
    // نشر النسخ / Copy Propagation
    addPass(std::make_unique<CopyPropagationPass>());
    
    // تقليل القوة / Strength Reduction (x * 8 → x << 3, etc.)
    addPass(std::make_unique<StrengthReductionPass>());
    
    // إزالة الكود الميت (عدواني) / Dead Code Elimination (aggressive)
    addPass(std::make_unique<DeadCodeEliminationPass>());
}

/**
 * @brief إضافة تمريرات المستوى 3
 * @brief Add level 3 passes
 * 
 * @details
 * (AR) تحسينات عدوانية: دمج السجلات ونقل الكود الثابت خارج الحلقة.
 * (EN) Aggressive optimizations: register coalescing and LICM.
 */
void Optimizer::addLevel3Passes() {
    // نقل الكود الثابت خارج الحلقة / Loop Invariant Code Motion
    addPass(std::make_unique<LICMPass>());
    
    // دمج السجلات / Register Coalescing
    addPass(std::make_unique<RegisterCoalescingPass>());
    
    // تطبيق DCE مرة أخرى / Apply DCE again
    addPass(std::make_unique<DeadCodeEliminationPass>());
}

/**
 * @brief جمع الإحصائيات من الوحدة
 * @brief Collect statistics from module
 */
void Optimizer::collectStats(std::shared_ptr<SIR::SIRModule> module) {
    if (!module) {
        return;
    }
    
    int instructionCount = 0;
    int registerCount = 0;
    
    auto functions = module->getFunctions();
    for (const auto& func : functions) {
        if (!func) continue;
        
        const auto& blocks = func->getBasicBlocks();
        for (const auto& block : blocks) {
            if (!block) continue;
            
            // Source: SIRBasicBlock::instructions is PUBLIC member at sir_instruction.h:356
            auto instructions = block->instructions;
            instructionCount += instructions.size();
        }
    }
    
    // تحديث الإحصائيات / Update statistics
    if (stats_.originalInstructionCount == 0) {
        stats_.originalInstructionCount = instructionCount;
    }
    stats_.optimizedInstructionCount = instructionCount;
}

/**
 * @brief إنشاء محسّن بمستوى محدد
 * @brief Create optimizer with specified level
 */
std::unique_ptr<Optimizer> createOptimizer(OptimizationLevel level) {
    return std::make_unique<Optimizer>(level);
}

/**
 * @brief تحويل نص إلى مستوى تحسين
 * @brief Convert string to optimization level
 */
OptimizationLevel parseOptimizationLevel(const std::string& str) {
    if (str == "O0" || str == "-O0") return OptimizationLevel::O0;
    if (str == "O1" || str == "-O1") return OptimizationLevel::O1;
    if (str == "O2" || str == "-O2") return OptimizationLevel::O2;
    if (str == "O3" || str == "-O3") return OptimizationLevel::O3;
    
    // افتراضياً O2 / Default O2
    return OptimizationLevel::O2;
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
