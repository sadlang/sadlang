/**
 * @file test_suite.cpp
 * @brief مجموعة الاختبار الشاملة / Comprehensive Test Suite
 * @brief Complete test coverage for compiler pipeline
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../compiler/include/compiler_pipeline.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>

using namespace Sad;

// ========================================
// Test Framework - إطار الاختبار
// ========================================

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double executionTime;
};

class TestSuite {
public:
    void runTest(const std::string& name, std::function<bool()> test) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::cout << "🧪 [اختبار / Test] " << name << "... ";
        
        try {
            bool passed = test();
            auto duration = measureTime(start);
            
            if (passed) {
                std::cout << "✅ نجح / PASSED";
                passedTests_++;
            } else {
                std::cout << "❌ فشل / FAILED";
                failedTests_++;
            }
            
            std::cout << " (" << formatTime(duration) << ")\n";
            
            TestResult result;
            result.name = name;
            result.passed = passed;
            result.executionTime = duration;
            results_.push_back(result);
            
        } catch (const std::exception& e) {
            std::cout << "❌ استثناء / EXCEPTION: " << e.what() << "\n";
            failedTests_++;
            
            TestResult result;
            result.name = name;
            result.passed = false;
            result.message = e.what();
            results_.push_back(result);
        }
        
        totalTests_++;
    }
    
    void printSummary() {
        std::cout << "\n========================================\n";
        std::cout << "ملخص الاختبارات / Test Summary\n";
        std::cout << "========================================\n";
        std::cout << "المجموع / Total:   " << totalTests_ << "\n";
        std::cout << "نجح / Passed:       " << passedTests_ << " ✅\n";
        std::cout << "فشل / Failed:       " << failedTests_ << " ❌\n";
        std::cout << "معدل النجاح / Rate: " 
                 << (totalTests_ > 0 ? (passedTests_ * 100.0 / totalTests_) : 0) 
                 << "%\n";
        std::cout << "========================================\n\n";
    }
    
    bool allPassed() const {
        return failedTests_ == 0 && totalTests_ > 0;
    }
    
private:
    int totalTests_ = 0;
    int passedTests_ = 0;
    int failedTests_ = 0;
    std::vector<TestResult> results_;
};

// ========================================
// Basic Tests - اختبارات أساسية
// ========================================

bool testArithmetic() {
    CompilerPipeline compiler;
    
    auto result = compiler.compileAndExecute("2 + 3 * 4");
    if (!result.success) return false;
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 14;
}

bool testVariables() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        متغير س = 10;
        متغير ص = 20;
        س + ص
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 30;
}

bool testFunctions() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        دالة جمع(أ، ب) {
            أرجع أ + ب;
        }
        
        جمع(15, 25)
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 40;
}

bool testArrays() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        متغير مصفوفة = [1, 2, 3, 4, 5];
        مصفوفة[2]
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 3;
}

bool testStrings() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        متغير نص = "مرحبا";
        نص
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    return result.returnValue.isString();
}

bool testLoops() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        متغير مجموع = 0;
        لكل (متغير ع = 1; ع <= 10; ع = ع + 1) {
            مجموع = مجموع + ع;
        }
        مجموع
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 55;
}

bool testConditionals() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        متغير س = 15;
        إذا (س > 10) {
            س * 2
        } وإلا {
            س
        }
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 30;
}

bool testRecursion() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        دالة عاملي(ن) {
            إذا (ن <= 1) {
                أرجع 1;
            } وإلا {
                أرجع ن * عاملي(ن - 1);
            }
        }
        
        عاملي(5)
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 120;
}

// ========================================
// Runtime Library Tests
// ========================================

bool testMathFunctions() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        sqrt(16) + floor(3.7) + ceil(2.3)
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    // 4 + 3 + 3 = 10
    return result.returnValue.isFloat() && 
           std::abs(result.returnValue.asFloat() - 10.0) < 0.001;
}

bool testStringFunctions() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        متغير نص = "مرحبا بك";
        to_upper(نص)
    )";
    
    auto result = compiler.compileAndExecute(code);
    return result.success && result.returnValue.isString();
}

bool testArrayFunctions() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        متغير أرقام = [1, 2, 3, 4, 5];
        متغير مضاعف = map(أرقام، دالة(ع) { أرجع ع * 2; });
        مضاعف[2]
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 6;
}

// ========================================
// Performance Tests - اختبارات الأداء
// ========================================

bool testPerformance() {
    CompilerPipeline compiler;
    
    // (AR) اختبار حسابي كثيف / (EN) Compute-intensive test
    std::string code = R"(
        متغير مجموع = 0;
        لكل (متغير ع = 0; ع < 10000; ع = ع + 1) {
            مجموع = مجموع + ع;
        }
        مجموع
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    // (AR) تحقق من السرعة / (EN) Check speed
    if (result.executionTime > 1.0) { // Should take less than 1 second
        std::cout << "\n⚠️ تحذير / Warning: بطيء / Slow execution time: " 
                 << result.executionTime << "s\n";
    }
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 49995000;
}

bool testMemoryEfficiency() {
    CompilerPipeline compiler;
    
    // (AR) اختبار استخدام الذاكرة / (EN) Memory usage test
    std::string code = R"(
        متغير مصفوفات = [];
        لكل (متغير ع = 0; ع < 100; ع = ع + 1) {
            مصفوفات[ع] = [ع, ع * 2, ع * 3];
        }
        مصفوفات[50][1]
    )";
    
    auto result = compiler.compileAndExecute(code);
    if (!result.success) return false;
    
    // (AR) تحقق من استخدام الذاكرة / (EN) Check memory usage
    if (result.peakMemoryUsage > 10 * 1024 * 1024) { // > 10MB
        std::cout << "\n⚠️ تحذير / Warning: استخدام ذاكرة عالٍ / High memory usage: " 
                 << formatMemorySize(result.peakMemoryUsage) << "\n";
    }
    
    return result.returnValue.isInt() && result.returnValue.asInt() == 100;
}

// ========================================
// File Format Tests
// ========================================

bool testBytecodeSaveLoad() {
    CompilerPipeline compiler;
    
    // (AR) ترجمة وحفظ / (EN) Compile and save
    std::string code = "2 + 3 * 4";
    auto compResult = compiler.compile(code);
    if (!compResult.success) return false;
    
    std::string filename = "test_output.sbc";
    if (!compiler.saveBytecode(filename)) return false;
    
    // (AR) تحميل وتنفيذ / (EN) Load and execute
    CompilerPipeline loader;
    auto execResult = loader.loadAndExecute(filename);
    
    // (AR) احذف الملف / (EN) Delete file
    std::remove(filename.c_str());
    
    return execResult.success && 
           execResult.returnValue.isInt() && 
           execResult.returnValue.asInt() == 14;
}

// ========================================
// Error Handling Tests
// ========================================

bool testSyntaxError() {
    CompilerPipeline compiler;
    
    std::string code = "متغير س = ;"; // Syntax error
    auto result = compiler.compileAndExecute(code);
    
    return !result.success; // Should fail
}

bool testRuntimeError() {
    CompilerPipeline compiler;
    
    std::string code = R"(
        متغير مصفوفة = [1, 2, 3];
        مصفوفة[10]  // Out of bounds
    )";
    
    auto result = compiler.compileAndExecute(code);
    
    return !result.success; // Should fail
}

// ========================================
// Main Test Runner
// ========================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║   مجموعة اختبارات لغة سد الشاملة    ║\n";
    std::cout << "║   SadLanguage Comprehensive Tests    ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    TestSuite suite;
    
    // (AR) الاختبارات الأساسية / (EN) Basic tests
    std::cout << "📦 الاختبارات الأساسية / Basic Tests\n";
    std::cout << "----------------------------------------\n";
    suite.runTest("العمليات الحسابية / Arithmetic", testArithmetic);
    suite.runTest("المتغيرات / Variables", testVariables);
    suite.runTest("الدوال / Functions", testFunctions);
    suite.runTest("المصفوفات / Arrays", testArrays);
    suite.runTest("النصوص / Strings", testStrings);
    suite.runTest("الحلقات / Loops", testLoops);
    suite.runTest("الشروط / Conditionals", testConditionals);
    suite.runTest("التكرار / Recursion", testRecursion);
    
    // (AR) اختبارات المكتبة / (EN) Runtime library tests
    std::cout << "\n📚 اختبارات المكتبة / Runtime Library Tests\n";
    std::cout << "----------------------------------------\n";
    suite.runTest("دوال الرياضيات / Math Functions", testMathFunctions);
    suite.runTest("دوال النصوص / String Functions", testStringFunctions);
    suite.runTest("دوال المصفوفات / Array Functions", testArrayFunctions);
    
    // (AR) اختبارات الأداء / (EN) Performance tests
    std::cout << "\n⚡ اختبارات الأداء / Performance Tests\n";
    std::cout << "----------------------------------------\n";
    suite.runTest("الأداء / Performance", testPerformance);
    suite.runTest("كفاءة الذاكرة / Memory Efficiency", testMemoryEfficiency);
    
    // (AR) اختبارات تنسيق الملف / (EN) File format tests
    std::cout << "\n💾 اختبارات الملفات / File Format Tests\n";
    std::cout << "----------------------------------------\n";
    suite.runTest("حفظ/تحميل البايت كود / Bytecode Save/Load", testBytecodeSaveLoad);
    
    // (AR) اختبارات معالجة الأخطاء / (EN) Error handling tests
    std::cout << "\n🔧 اختبارات الأخطاء / Error Handling Tests\n";
    std::cout << "----------------------------------------\n";
    suite.runTest("خطأ نحوي / Syntax Error", testSyntaxError);
    suite.runTest("خطأ تنفيذي / Runtime Error", testRuntimeError);
    
    // (AR) الملخص / (EN) Summary
    suite.printSummary();
    
    return suite.allPassed() ? 0 : 1;
}
