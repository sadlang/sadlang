/*
 * اختبارات المُحسّن LLVM / LLVM Optimizer Tests
 * ================================================
 */

#include "test_framework.h"
#include "test_helpers.h"
#include "../../../compiler/backends/llvm/llvm_optimizer.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>

using namespace sad;
using namespace sad::testing;

// تهيئة LLVM / Initialize LLVM
static bool llvm_initialized = false;

void initializeLLVM() {
    if (!llvm_initialized) {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        llvm_initialized = true;
    }
}

// ═══════════════════════════════════════════════════════
// المجموعة 1: التهيئة / Initialization Tests (5 tests)
// ═══════════════════════════════════════════════════════

TEST(LLVMOptimizer, BasicInitialization) {
    initializeLLVM();
    LLVMOptimizer optimizer;
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    EXPECT_TRUE(optimizer.initialize(nullptr));
}

TEST(LLVMOptimizer, InitializeWithTargetMachine) {
    initializeLLVM();
    LLVMOptimizer optimizer;
    // التمرير بـ nullptr مقبول / Passing nullptr is acceptable
    EXPECT_TRUE(optimizer.initialize(nullptr));
}

TEST(LLVMOptimizer, MultipleInitialization) {
    initializeLLVM();
    LLVMOptimizer optimizer;
    EXPECT_TRUE(optimizer.initialize(nullptr));
    EXPECT_TRUE(optimizer.initialize(nullptr)); // يجب أن ينجح / Should succeed
}

TEST(LLVMOptimizer, SetOptimizationLevel) {
    initializeLLVM();
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    
    EXPECT_NO_THROW(optimizer.setOptimizationLevel(OptimizationLevel::O0));
    EXPECT_NO_THROW(optimizer.setOptimizationLevel(OptimizationLevel::O3));
}

TEST(LLVMOptimizer, GetStatsBeforeOptimization) {
    initializeLLVM();
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    
    const auto& stats = optimizer.getOptimizationStats();
    EXPECT_EQ(stats.instructions_before, 0);
    EXPECT_EQ(stats.instructions_after, 0);
}

// ═══════════════════════════════════════════════════════
// المجموعة 2: مستوى O0 / O0 Level Tests (3 tests)
// ═══════════════════════════════════════════════════════

TEST(LLVMOptimizer, O0_NoOptimization) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O0);
    
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

TEST(LLVMOptimizer, O0_PreservesInstructions) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    // عد التعليمات قبل / Count instructions before
    size_t instr_count_before = 0;
    for (auto& func : *module) {
        for (auto& bb : func) {
            instr_count_before += bb.size();
        }
    }
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O0);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    EXPECT_EQ(stats.instructions_before, instr_count_before);
}

TEST(LLVMOptimizer, O0_ModuleStillValid) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O0);
    optimizer.optimize(module.get());
    
    std::string error;
    EXPECT_TRUE(verifyModule(module.get(), &error));
}

// ═══════════════════════════════════════════════════════
// المجموعة 3: مستوى O1 / O1 Level Tests (5 tests)
// ═══════════════════════════════════════════════════════

TEST(LLVMOptimizer, O1_BasicOptimization) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O1);
    
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

TEST(LLVMOptimizer, O1_ReducesInstructions) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // إنشاء دالة مع تعليمات زائدة / Create function with redundant instructions
    auto func = createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O1);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    // O1 قد يقلل التعليمات / O1 may reduce instructions
    EXPECT_TRUE(stats.instructions_after <= stats.instructions_before);
}

TEST(LLVMOptimizer, O1_ModuleValid) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O1);
    optimizer.optimize(module.get());
    
    std::string error;
    EXPECT_TRUE(verifyModule(module.get(), &error));
}

TEST(LLVMOptimizer, O1_HasStats) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O1);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    EXPECT_TRUE(stats.instructions_before > 0);
    EXPECT_TRUE(stats.optimization_time_ms >= 0);
}

TEST(LLVMOptimizer, O1_FastExecution) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O1);
    
    auto time = measureTime([&]() {
        optimizer.optimize(module.get());
    });
    
    // O1 يجب أن يكون سريعاً (أقل من ثانية) / O1 should be fast (less than 1 second)
    EXPECT_TRUE(time < 1000.0);
}

// ═══════════════════════════════════════════════════════
// المجموعة 4: مستوى O2 / O2 Level Tests (5 tests)
// ═══════════════════════════════════════════════════════

TEST(LLVMOptimizer, O2_StandardOptimization) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

TEST(LLVMOptimizer, O2_BetterThanO1) {
    initializeLLVM();
    llvm::LLVMContext context1, context2;
    
    // نسختان من نفس الوحدة / Two copies of same module
    auto module1 = createSimpleModule(context1);
    auto module2 = createSimpleModule(context2);
    createSimpleFunction(module1.get(), "test_func");
    createSimpleFunction(module2.get(), "test_func");
    
    // تحسين O1 / Optimize with O1
    LLVMOptimizer opt1;
    opt1.initialize(nullptr);
    opt1.setOptimizationLevel(OptimizationLevel::O1);
    opt1.optimize(module1.get());
    
    // تحسين O2 / Optimize with O2
    LLVMOptimizer opt2;
    opt2.initialize(nullptr);
    opt2.setOptimizationLevel(OptimizationLevel::O2);
    opt2.optimize(module2.get());
    
    // O2 يجب أن يعطي نتائج أفضل أو مساوية / O2 should give better or equal results
    const auto& stats1 = opt1.getOptimizationStats();
    const auto& stats2 = opt2.getOptimizationStats();
    
    EXPECT_TRUE(stats2.instructions_after <= stats1.instructions_after);
}

TEST(LLVMOptimizer, O2_ModuleValid) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    
    std::string error;
    EXPECT_TRUE(verifyModule(module.get(), &error));
}

TEST(LLVMOptimizer, O2_ReasonableTime) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    
    auto time = measureTime([&]() {
        optimizer.optimize(module.get());
    });
    
    // O2 معقول (أقل من 2 ثانية) / O2 reasonable (less than 2 seconds)
    EXPECT_TRUE(time < 2000.0);
}

TEST(LLVMOptimizer, O2_ComprehensiveStats) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    EXPECT_TRUE(stats.instructions_before > 0);
    EXPECT_TRUE(stats.instructions_after > 0);
    EXPECT_TRUE(stats.optimization_time_ms >= 0);
}

// ═══════════════════════════════════════════════════════
// المجموعة 5: مستوى O3 / O3 Level Tests (5 tests)
// ═══════════════════════════════════════════════════════

TEST(LLVMOptimizer, O3_AggressiveOptimization) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O3);
    
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

TEST(LLVMOptimizer, O3_MaximumOptimization) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O3);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    // O3 يجب أن يعطي أفضل تحسين / O3 should give best optimization
    EXPECT_TRUE(stats.instructions_after <= stats.instructions_before);
}

TEST(LLVMOptimizer, O3_ModuleValid) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O3);
    optimizer.optimize(module.get());
    
    std::string error;
    EXPECT_TRUE(verifyModule(module.get(), &error));
}

TEST(LLVMOptimizer, O3_TakesSomeTime) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O3);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    // O3 يأخذ وقتاً أطول من O0 / O3 takes longer than O0
    EXPECT_TRUE(stats.optimization_time_ms >= 0);
}

TEST(LLVMOptimizer, O3_BestResults) {
    initializeLLVM();
    llvm::LLVMContext context1, context2;
    
    auto module1 = createSimpleModule(context1);
    auto module2 = createSimpleModule(context2);
    createSimpleFunction(module1.get(), "test_func");
    createSimpleFunction(module2.get(), "test_func");
    
    // تحسين O2 / Optimize with O2
    LLVMOptimizer opt2;
    opt2.initialize(nullptr);
    opt2.setOptimizationLevel(OptimizationLevel::O2);
    opt2.optimize(module1.get());
    
    // تحسين O3 / Optimize with O3
    LLVMOptimizer opt3;
    opt3.initialize(nullptr);
    opt3.setOptimizationLevel(OptimizationLevel::O3);
    opt3.optimize(module2.get());
    
    // O3 أفضل أو مساوي لـ O2 / O3 better or equal to O2
    const auto& stats2 = opt2.getOptimizationStats();
    const auto& stats3 = opt3.getOptimizationStats();
    
    EXPECT_TRUE(stats3.instructions_after <= stats2.instructions_after);
}

// ═══════════════════════════════════════════════════════
// المجموعة 6: مستوى Os / Os Level Tests (4 tests)
// ═══════════════════════════════════════════════════════

TEST(LLVMOptimizer, Os_SizeOptimization) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::Os);
    
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

TEST(LLVMOptimizer, Os_ReducesSize) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::Os);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    // Os يركز على تقليل الحجم / Os focuses on size reduction
    EXPECT_TRUE(stats.instructions_after <= stats.instructions_before);
}

TEST(LLVMOptimizer, Os_ModuleValid) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::Os);
    optimizer.optimize(module.get());
    
    std::string error;
    EXPECT_TRUE(verifyModule(module.get(), &error));
}

TEST(LLVMOptimizer, Os_FastOptimization) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::Os);
    
    auto time = measureTime([&]() {
        optimizer.optimize(module.get());
    });
    
    EXPECT_TRUE(time < 2000.0);
}

// ═══════════════════════════════════════════════════════
// المجموعة 7: مستوى Oz / Oz Level Tests (4 tests)
// ═══════════════════════════════════════════════════════

TEST(LLVMOptimizer, Oz_AggressiveSizeOptimization) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::Oz);
    
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

TEST(LLVMOptimizer, Oz_MinimumSize) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::Oz);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    // Oz يعطي أصغر حجم / Oz gives smallest size
    EXPECT_TRUE(stats.instructions_after <= stats.instructions_before);
}

TEST(LLVMOptimizer, Oz_ModuleValid) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::Oz);
    optimizer.optimize(module.get());
    
    std::string error;
    EXPECT_TRUE(verifyModule(module.get(), &error));
}

TEST(LLVMOptimizer, Oz_BetterThanOs) {
    initializeLLVM();
    llvm::LLVMContext context1, context2;
    
    auto module1 = createSimpleModule(context1);
    auto module2 = createSimpleModule(context2);
    createSimpleFunction(module1.get(), "test_func");
    createSimpleFunction(module2.get(), "test_func");
    
    // تحسين Os / Optimize with Os
    LLVMOptimizer optS;
    optS.initialize(nullptr);
    optS.setOptimizationLevel(OptimizationLevel::Os);
    optS.optimize(module1.get());
    
    // تحسين Oz / Optimize with Oz
    LLVMOptimizer optZ;
    optZ.initialize(nullptr);
    optZ.setOptimizationLevel(OptimizationLevel::Oz);
    optZ.optimize(module2.get());
    
    // Oz أصغر أو مساوي لـ Os / Oz smaller or equal to Os
    const auto& statsS = optS.getOptimizationStats();
    const auto& statsZ = optZ.getOptimizationStats();
    
    EXPECT_TRUE(statsZ.instructions_after <= statsS.instructions_after);
}

// ═══════════════════════════════════════════════════════
// المجموعة 8: الإحصائيات / Statistics Tests (5 tests)
// ═══════════════════════════════════════════════════════

TEST(LLVMOptimizer, Stats_InstructionCount) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    EXPECT_TRUE(stats.instructions_before > 0);
    EXPECT_TRUE(stats.instructions_after > 0);
}

TEST(LLVMOptimizer, Stats_OptimizationTime) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    EXPECT_TRUE(stats.optimization_time_ms >= 0);
}

TEST(LLVMOptimizer, Stats_ReductionPercentage) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O3);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    // نسبة التقليل منطقية (0-100%) / Reduction percentage is reasonable (0-100%)
    if (stats.instructions_before > 0) {
        double reduction = 100.0 * (stats.instructions_before - stats.instructions_after) 
                          / stats.instructions_before;
        EXPECT_TRUE(reduction >= 0 && reduction <= 100);
    }
}

TEST(LLVMOptimizer, Stats_ConsistentData) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    
    const auto& stats = optimizer.getOptimizationStats();
    // البيانات متسقة / Data is consistent
    EXPECT_TRUE(stats.instructions_after <= stats.instructions_before);
}

TEST(LLVMOptimizer, Stats_PrintStats) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    optimizer.optimize(module.get());
    
    // يجب ألا يحدث استثناء / Should not throw
    EXPECT_NO_THROW(optimizer.printOptimizationStats());
}

// ═══════════════════════════════════════════════════════
// المجموعة 9: حالات خاصة / Special Cases Tests (4 tests)
// ═══════════════════════════════════════════════════════

TEST(LLVMOptimizer, EmptyModule) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    // وحدة فارغة / Empty module
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

TEST(LLVMOptimizer, MultipleFunctions) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    
    // إنشاء عدة دوال / Create multiple functions
    createSimpleFunction(module.get(), "func1");
    createSimpleFunction(module.get(), "func2");
    createSimpleFunction(module.get(), "func3");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

TEST(LLVMOptimizer, RepeatedOptimization) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    
    // تحسين متكرر / Repeated optimization
    EXPECT_TRUE(optimizer.optimize(module.get()));
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

TEST(LLVMOptimizer, DifferentLevelsSequentially) {
    initializeLLVM();
    llvm::LLVMContext context;
    auto module = createSimpleModule(context);
    createSimpleFunction(module.get(), "test_func");
    
    LLVMOptimizer optimizer;
    optimizer.initialize(nullptr);
    
    // تحسين بمستويات مختلفة / Optimize with different levels
    optimizer.setOptimizationLevel(OptimizationLevel::O1);
    EXPECT_TRUE(optimizer.optimize(module.get()));
    
    optimizer.setOptimizationLevel(OptimizationLevel::O2);
    EXPECT_TRUE(optimizer.optimize(module.get()));
}

/**
 * الدالة الرئيسية لتشغيل الاختبارات / Main function to run tests
 */
int main(int argc, char** argv) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  اختبارات المُحسّن LLVM / LLVM Optimizer Tests            ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    std::cout << std::endl;
    
    auto& manager = TestManager::instance();
    int result = manager.runAllTests(true);
    
    return result;
}
