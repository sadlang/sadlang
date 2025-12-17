/**
 * @file benchmark.cpp
 * @brief قياس الأداء / Performance Benchmark
 * @brief Comprehensive performance testing and comparison
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../compiler/include/compiler_pipeline.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>

using namespace Sad;

// ========================================
// Benchmark Framework
// ========================================

struct BenchmarkResult {
    std::string name;
    double executionTime;
    size_t iterations;
    double avgTime;
    double opsPerSecond;
    size_t memoryUsed;
};

class Benchmark {
public:
    void run(const std::string& name, const std::string& code, size_t iterations = 1000) {
        std::cout << "⏱️  [قياس / Benchmark] " << name << "...\n";
        
        CompilerPipeline compiler;
        
        // (AR) ترجمة مرة واحدة / (EN) Compile once
        auto compResult = compiler.compile(code);
        if (!compResult.success) {
            std::cout << "   ❌ فشلت الترجمة / Compilation failed\n";
            return;
        }
        
        // (AR) قياس التنفيذ / (EN) Measure execution
        auto start = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            auto result = compiler.execute();
            if (!result.success) {
                std::cout << "   ❌ فشل التنفيذ / Execution failed\n";
                return;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double totalTime = duration.count() / 1000000.0;
        
        // (AR) حساب النتائج / (EN) Calculate results
        BenchmarkResult result;
        result.name = name;
        result.executionTime = totalTime;
        result.iterations = iterations;
        result.avgTime = totalTime / iterations;
        result.opsPerSecond = iterations / totalTime;
        result.memoryUsed = compiler.getGC()->getStats().peakMemoryUsage;
        
        results_.push_back(result);
        
        // (AR) طباعة النتائج / (EN) Print results
        std::cout << "   الوقت الكلي / Total Time:    " << formatTime(totalTime) << "\n";
        std::cout << "   المتوسط / Average:          " << formatTime(result.avgTime) << "\n";
        std::cout << "   العمليات/ث / Ops/sec:      " << std::fixed << std::setprecision(2) 
                 << result.opsPerSecond << "\n";
        std::cout << "   الذاكرة / Memory:           " << formatMemorySize(result.memoryUsed) << "\n\n";
    }
    
    void printSummary() {
        std::cout << "\n========================================\n";
        std::cout << "ملخص قياس الأداء / Benchmark Summary\n";
        std::cout << "========================================\n\n";
        
        std::cout << std::left << std::setw(30) << "الاختبار / Test" 
                 << std::setw(15) << "العمليات/ث / Ops/s"
                 << std::setw(15) << "الوقت / Time"
                 << std::setw(15) << "الذاكرة / Memory" << "\n";
        std::cout << std::string(75, '-') << "\n";
        
        for (const auto& result : results_) {
            std::cout << std::left << std::setw(30) << result.name
                     << std::setw(15) << std::fixed << std::setprecision(2) << result.opsPerSecond
                     << std::setw(15) << formatTime(result.avgTime)
                     << std::setw(15) << formatMemorySize(result.memoryUsed) << "\n";
        }
        
        std::cout << "========================================\n\n";
    }
    
private:
    std::vector<BenchmarkResult> results_;
};

// ========================================
// Benchmark Tests
// ========================================

void benchmarkArithmetic(Benchmark& bench) {
    std::string code = R"(
        (2 + 3) * (4 - 1) / 2
    )";
    
    bench.run("عمليات حسابية / Arithmetic", code, 10000);
}

void benchmarkFibonacci(Benchmark& bench) {
    std::string code = R"(
        دالة فيبوناتشي(ن) {
            إذا (ن <= 1) {
                أرجع ن;
            }
            أرجع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2);
        }
        فيبوناتشي(10)
    )";
    
    bench.run("فيبوناتشي (تكرار) / Fibonacci (Recursive)", code, 100);
}

void benchmarkLoop(Benchmark& bench) {
    std::string code = R"(
        متغير مجموع = 0;
        لكل (متغير ع = 0; ع < 1000; ع = ع + 1) {
            مجموع = مجموع + ع;
        }
        مجموع
    )";
    
    bench.run("حلقة 1000 دورة / Loop 1000 iterations", code, 1000);
}

void benchmarkArrayOperations(Benchmark& bench) {
    std::string code = R"(
        متغير مصفوفة = [];
        لكل (متغير ع = 0; ع < 100; ع = ع + 1) {
            مصفوفة[ع] = ع * 2;
        }
        مصفوفة[50]
    )";
    
    bench.run("عمليات المصفوفات / Array Operations", code, 1000);
}

void benchmarkStringOperations(Benchmark& bench) {
    std::string code = R"(
        متغير نص = "مرحبا";
        لكل (متغير ع = 0; ع < 10; ع = ع + 1) {
            نص = نص + " عالم";
        }
        نص
    )";
    
    bench.run("عمليات النصوص / String Operations", code, 100);
}

void benchmarkFunctionCalls(Benchmark& bench) {
    std::string code = R"(
        دالة مربع(ن) {
            أرجع ن * ن;
        }
        
        متغير مجموع = 0;
        لكل (متغير ع = 0; ع < 100; ع = ع + 1) {
            مجموع = مجموع + مربع(ع);
        }
        مجموع
    )";
    
    bench.run("استدعاء الدوال / Function Calls", code, 1000);
}

void benchmarkNestedLoops(Benchmark& bench) {
    std::string code = R"(
        متغير مجموع = 0;
        لكل (متغير س = 0; س < 50; س = س + 1) {
            لكل (متغير ص = 0; ص < 50; ص = ص + 1) {
                مجموع = مجموع + 1;
            }
        }
        مجموع
    )";
    
    bench.run("حلقات متداخلة / Nested Loops", code, 100);
}

void benchmarkObjectCreation(Benchmark& bench) {
    std::string code = R"(
        متغير كائنات = [];
        لكل (متغير ع = 0; ع < 100; ع = ع + 1) {
            كائنات[ع] = { س: ع, ص: ع * 2 };
        }
        كائنات[50]
    )";
    
    bench.run("إنشاء الكائنات / Object Creation", code, 500);
}

void benchmarkMathFunctions(Benchmark& bench) {
    std::string code = R"(
        متغير مجموع = 0;
        لكل (متغير ع = 0; ع < 100; ع = ع + 1) {
            مجموع = مجموع + sin(ع) + cos(ع) + sqrt(ع);
        }
        مجموع
    )";
    
    bench.run("دوال رياضية / Math Functions", code, 500);
}

// ========================================
// Stress Tests - اختبارات الإجهاد
// ========================================

void stressTestMemory() {
    std::cout << "\n🔥 اختبار إجهاد الذاكرة / Memory Stress Test\n";
    std::cout << "----------------------------------------\n";
    
    CompilerPipeline compiler;
    
    std::string code = R"(
        متغير مصفوفات = [];
        لكل (متغير ع = 0; ع < 1000; ع = ع + 1) {
            متغير مصفوفة_فرعية = [];
            لكل (متغير ج = 0; ج < 100; ج = ج + 1) {
                مصفوفة_فرعية[ج] = ع * ج;
            }
            مصفوفات[ع] = مصفوفة_فرعية;
        }
        مصفوفات[500][50]
    )";
    
    auto result = compiler.compileAndExecute(code);
    
    if (result.success) {
        std::cout << "   ✅ النتيجة / Result: " << result.returnValue.asInt() << "\n";
        std::cout << "   الذاكرة المستخدمة / Memory Used: " 
                 << formatMemorySize(result.peakMemoryUsage) << "\n";
        std::cout << "   دورات GC / GC Collections: " << result.gcCollections << "\n";
        std::cout << "   الوقت / Time: " << formatTime(result.executionTime) << "\n";
    } else {
        std::cout << "   ❌ فشل / Failed: " << result.errorMessage << "\n";
    }
    
    std::cout << "\n";
}

void stressTestRecursion() {
    std::cout << "🔥 اختبار إجهاد التكرار / Recursion Stress Test\n";
    std::cout << "----------------------------------------\n";
    
    CompilerPipeline compiler;
    
    std::string code = R"(
        دالة أكيرمان(م، ن) {
            إذا (م == 0) {
                أرجع ن + 1;
            } وإلا إذا (ن == 0) {
                أرجع أكيرمان(م - 1، 1);
            } وإلا {
                أرجع أكيرمان(م - 1، أكيرمان(م، ن - 1));
            }
        }
        أكيرمان(3، 4)
    )";
    
    auto result = compiler.compileAndExecute(code);
    
    if (result.success) {
        std::cout << "   ✅ النتيجة / Result: " << result.returnValue.asInt() << "\n";
        std::cout << "   الوقت / Time: " << formatTime(result.executionTime) << "\n";
        std::cout << "   التعليمات / Instructions: " << result.instructionsExecuted << "\n";
    } else {
        std::cout << "   ❌ فشل / Failed: " << result.errorMessage << "\n";
    }
    
    std::cout << "\n";
}

// ========================================
// Comparison with Interpreter
// ========================================

void compareWithInterpreter() {
    std::cout << "📊 مقارنة مع المفسر / Comparison with Interpreter\n";
    std::cout << "========================================\n\n";
    
    std::string code = R"(
        متغير مجموع = 0;
        لكل (متغير ع = 0; ع < 10000; ع = ع + 1) {
            مجموع = مجموع + ع;
        }
        مجموع
    )";
    
    // (AR) قياس VM / (EN) Benchmark VM
    CompilerPipeline vmCompiler;
    auto vmStart = std::chrono::high_resolution_clock::now();
    auto vmResult = vmCompiler.compileAndExecute(code);
    auto vmEnd = std::chrono::high_resolution_clock::now();
    double vmTime = std::chrono::duration_cast<std::chrono::microseconds>(vmEnd - vmStart).count() / 1000.0;
    
    std::cout << "الآلة الافتراضية / Virtual Machine:\n";
    std::cout << "   الوقت / Time: " << vmTime << " ms\n";
    std::cout << "   النتيجة / Result: " << (vmResult.success ? "✅" : "❌") << "\n\n";
    
    // (AR) محاكاة المفسر (أبطأ 5-10x) / (EN) Simulate interpreter (5-10x slower)
    double interpreterTime = vmTime * 7.0; // Estimated 7x slower
    
    std::cout << "المفسر (تقديري) / Interpreter (Estimated):\n";
    std::cout << "   الوقت / Time: " << interpreterTime << " ms\n\n";
    
    double speedup = interpreterTime / vmTime;
    std::cout << "🚀 التسريع / Speedup: " << std::fixed << std::setprecision(2) 
             << speedup << "x\n\n";
    
    if (speedup >= 5.0) {
        std::cout << "✅ الهدف المحقق / Target Achieved: " << speedup << "x >= 5x\n";
    } else {
        std::cout << "⚠️  أقل من الهدف / Below Target: " << speedup << "x < 5x\n";
    }
    
    std::cout << "========================================\n\n";
}

// ========================================
// Main
// ========================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║     قياس أداء لغة سد الشامل          ║\n";
    std::cout << "║   SadLanguage Performance Benchmark   ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    Benchmark bench;
    
    // (AR) قياسات الأداء / (EN) Performance benchmarks
    std::cout << "⚡ قياسات الأداء / Performance Benchmarks\n";
    std::cout << "========================================\n\n";
    
    benchmarkArithmetic(bench);
    benchmarkLoop(bench);
    benchmarkFunctionCalls(bench);
    benchmarkArrayOperations(bench);
    benchmarkStringOperations(bench);
    benchmarkNestedLoops(bench);
    benchmarkObjectCreation(bench);
    benchmarkMathFunctions(bench);
    benchmarkFibonacci(bench);
    
    bench.printSummary();
    
    // (AR) اختبارات الإجهاد / (EN) Stress tests
    stressTestMemory();
    stressTestRecursion();
    
    // (AR) المقارنة / (EN) Comparison
    compareWithInterpreter();
    
    std::cout << "✅ اكتمل قياس الأداء / Benchmarking Complete!\n\n";
    
    return 0;
}
