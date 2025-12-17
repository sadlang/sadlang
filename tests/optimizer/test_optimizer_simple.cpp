// ===============================================
// test_optimizer_simple.cpp
// ملف اختبار بسيط للمحسن المتقدم بدون GTest
// Simple optimizer test without GTest dependency
// ===============================================

#include <iostream>
#include <cassert>
#include <memory>
#include "optimizer/advanced_optimizer.h"
#include "optimizer/optimization_pass.h"

using namespace Sad;

// ===================================
// اختبارات أساسية للممرات
// Basic Pass Tests
// ===================================

void testConstantFoldingPass() {
    std::cout << "\n=== Testing Constant Folding Pass ===\n";
    
    // إنشاء ممر طي الثوابت
    sad::ConstantFoldingPass pass;
    
    // التحقق من الاسم
    assert(pass.getName() == "ConstantFolding");
    std::cout << "✓ Pass name: " << pass.getName() << "\n";
    
    // التحقق من المستوى
    assert(pass.getLevel() == 0);
    std::cout << "✓ Pass level: " << pass.getLevel() << "\n";
    
    // التحقق من الأمان
    assert(pass.isSafe() == true);
    std::cout << "✓ Pass is safe\n";
    
    std::cout << "✓ Description: " << pass.getDescription() << "\n";
    
    std::cout << "=== Constant Folding Pass Test PASSED ===\n";
}

void testExpressionSimplificationPass() {
    std::cout << "\n=== Testing Expression Simplification Pass ===\n";
    
    sad::ExpressionSimplificationPass pass;
    
    assert(pass.getName() == "ExpressionSimplification");
    std::cout << "✓ Pass name: " << pass.getName() << "\n";
    
    assert(pass.getLevel() == 1);
    std::cout << "✓ Pass level: " << pass.getLevel() << "\n";
    
    assert(pass.isSafe() == true);
    std::cout << "✓ Pass is safe\n";
    
    std::cout << "✓ Description: " << pass.getDescription() << "\n";
    
    std::cout << "=== Expression Simplification Pass Test PASSED ===\n";
}

void testDeadCodeEliminationPass() {
    std::cout << "\n=== Testing Dead Code Elimination Pass ===\n";
    
    sad::DeadCodeEliminationPass pass;
    
    assert(pass.getName() == "DeadCodeElimination");
    std::cout << "✓ Pass name: " << pass.getName() << "\n";
    
    assert(pass.getLevel() == 1);
    std::cout << "✓ Pass level: " << pass.getLevel() << "\n";
    
    assert(pass.isSafe() == true);
    std::cout << "✓ Pass is safe\n";
    
    std::cout << "✓ Description: " << pass.getDescription() << "\n";
    
    std::cout << "=== Dead Code Elimination Pass Test PASSED ===\n";
}

void testLoopOptimizationPass() {
    std::cout << "\n=== Testing Loop Optimization Pass ===\n";
    
    sad::LoopOptimizationPass pass;
    
    assert(pass.getName() == "LoopOptimization");
    std::cout << "✓ Pass name: " << pass.getName() << "\n";
    
    assert(pass.getLevel() == 2);
    std::cout << "✓ Pass level: " << pass.getLevel() << "\n";
    
    assert(pass.isSafe() == true);
    std::cout << "✓ Pass is safe\n";
    
    std::cout << "✓ Description: " << pass.getDescription() << "\n";
    
    std::cout << "=== Loop Optimization Pass Test PASSED ===\n";
}

void testFunctionInliningPass() {
    std::cout << "\n=== Testing Function Inlining Pass ===\n";
    
    sad::FunctionInliningPass pass;
    
    assert(pass.getName() == "FunctionInlining");
    std::cout << "✓ Pass name: " << pass.getName() << "\n";
    
    assert(pass.getLevel() == 2);
    std::cout << "✓ Pass level: " << pass.getLevel() << "\n";
    
    // Function inlining is aggressive - marked as NOT safe (may increase code size)
    assert(pass.isSafe() == false);
    std::cout << "✓ Pass is aggressive (not safe - may increase code size)\n";
    
    std::cout << "✓ Description: " << pass.getDescription() << "\n";
    
    std::cout << "=== Function Inlining Pass Test PASSED ===\n";
}

void testRedundantAssignmentEliminationPass() {
    std::cout << "\n=== Testing Redundant Assignment Elimination Pass ===\n";
    
    sad::RedundantAssignmentEliminationPass pass;
    
    assert(pass.getName() == "RedundantAssignmentElimination");
    std::cout << "✓ Pass name: " << pass.getName() << "\n";
    
    assert(pass.getLevel() == 1);
    std::cout << "✓ Pass level: " << pass.getLevel() << "\n";
    
    assert(pass.isSafe() == true);
    std::cout << "✓ Pass is safe\n";
    
    std::cout << "✓ Description: " << pass.getDescription() << "\n";
    
    std::cout << "=== Redundant Assignment Elimination Pass Test PASSED ===\n";
}

void testAdvancedOptimizer() {
    std::cout << "\n=== Testing Advanced Optimizer ===\n";
    
    sad::AdvancedOptimizer optimizer;
    
    // التحقق من أن المحسن يمكن إنشاؤه
    std::cout << "✓ Advanced optimizer created successfully\n";
    
    // هنا يمكن إضافة اختبارات أكثر تفصيلاً للمحسن عندما يكون لدينا AST فعلي
    
    std::cout << "=== Advanced Optimizer Test PASSED ===\n";
}

// ===================================
// الدالة الرئيسية
// Main Function
// ===================================

int main() {
    std::cout << "\n";
    std::cout << "===========================================\n";
    std::cout << "   Phase 10 - Advanced Optimizer Tests    \n";
    std::cout << "   المرحلة 10 - اختبارات المحسن المتقدم   \n";
    std::cout << "===========================================\n";
    
    try {
        // تشغيل جميع الاختبارات
        testConstantFoldingPass();
        testExpressionSimplificationPass();
        testDeadCodeEliminationPass();
        testLoopOptimizationPass();
        testFunctionInliningPass();
        testRedundantAssignmentEliminationPass();
        testAdvancedOptimizer();
        
        std::cout << "\n";
        std::cout << "===========================================\n";
        std::cout << "   ✓ All Tests PASSED!                    \n";
        std::cout << "   ✓ جميع الاختبارات نجحت!               \n";
        std::cout << "===========================================\n";
        std::cout << "\n";
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n";
        std::cerr << "===========================================\n";
        std::cerr << "   ✗ Test FAILED!                         \n";
        std::cerr << "   ✗ فشل الاختبار!                       \n";
        std::cerr << "===========================================\n";
        std::cerr << "Error: " << e.what() << "\n";
        std::cerr << "\n";
        return 1;
    }
}
