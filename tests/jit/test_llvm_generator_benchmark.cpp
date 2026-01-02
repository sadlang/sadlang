// ============================================================================
// test_llvm_generator_benchmark.cpp - قياسات أداء LLVM Generator
// LLVM Generator Performance Benchmarks
// ============================================================================
// الغرض: قياس أداء LLVMGenerator بشكل دقيق ومقارنة النتائج
// Purpose: Measure LLVMGenerator performance accurately and compare results
// ============================================================================
// المصدر: compiler/backend/llvm/include/llvm_generator.h
// Source: compiler/backend/llvm/include/llvm_generator.h
// المصدر: compiler/jit/include/jit_profiler.h
// Source: compiler/jit/include/jit_profiler.h
// ============================================================================

#include "../../compiler/backend/llvm/include/llvm_generator.h"
#include "../../compiler/jit/include/jit_profiler.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>

// ============================================================================
// BenchmarkResult - نتيجة قياس واحد / Single Benchmark Result
// ============================================================================

struct BenchmarkResult {
    std::string name;                    // اسم القياس / Benchmark name
    size_t iterations;                   // عدد التكرارات / Number of iterations
    double total_time_ms;                // الوقت الكلي (ملي ثانية) / Total time (milliseconds)
    double avg_time_ms;                  // متوسط الوقت / Average time
    double min_time_ms;                  // أقل وقت / Minimum time
    double max_time_ms;                  // أقصى وقت / Maximum time
    double stddev_ms;                    // الانحراف المعياري / Standard deviation
    size_t total_memory_bytes;           // الذاكرة الكلية / Total memory
    bool success;                        // نجح القياس؟ / Benchmark succeeded?
    
    // المُنشئ / Constructor
    BenchmarkResult(const std::string& n = "")
        : name(n)
        , iterations(0)
        , total_time_ms(0.0)
        , avg_time_ms(0.0)
        , min_time_ms(std::numeric_limits<double>::max())
        , max_time_ms(0.0)
        , stddev_ms(0.0)
        , total_memory_bytes(0)
        , success(true)
    {}
};

// ============================================================================
// BenchmarkTimer - مؤقت القياس / Benchmark Timer
// ============================================================================

class BenchmarkTimer {
public:
    // البدء / Start timing
    void start() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    // الإيقاف والحصول على المدة بالملي ثانية / Stop and get duration in milliseconds
    double stop() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time_
        );
        return duration.count() / 1000.0; // تحويل إلى ملي ثانية / Convert to milliseconds
    }

private:
    std::chrono::high_resolution_clock::time_point start_time_; // وقت البدء / Start time
};

// ============================================================================
// دوال مساعدة للطباعة / Helper Functions for Printing
// ============================================================================

// طباعة خط فاصل / Print separator line
void printSeparator(char c = '=') {
    std::cout << std::string(80, c) << "\n";
}

// طباعة عنوان / Print header
void printHeader(const std::string& title) {
    printSeparator('=');
    std::cout << "  " << title << "\n";
    printSeparator('=');
}

// طباعة نتيجة قياس / Print benchmark result
void printResult(const BenchmarkResult& result) {
    std::cout << std::fixed << std::setprecision(3);
    
    std::cout << "📊 " << result.name << "\n";
    std::cout << "  ├─ التكرارات / Iterations: " << result.iterations << "\n";
    std::cout << "  ├─ الوقت الكلي / Total Time: " << result.total_time_ms << " ms\n";
    std::cout << "  ├─ متوسط الوقت / Average Time: " << result.avg_time_ms << " ms\n";
    std::cout << "  ├─ أقل وقت / Min Time: " << result.min_time_ms << " ms\n";
    std::cout << "  ├─ أقصى وقت / Max Time: " << result.max_time_ms << " ms\n";
    std::cout << "  ├─ الانحراف المعياري / StdDev: " << result.stddev_ms << " ms\n";
    std::cout << "  ├─ استهلاك الذاكرة / Memory: " << (result.total_memory_bytes / 1024) << " KB\n";
    std::cout << "  └─ الحالة / Status: " << (result.success ? "✅ نجح / Success" : "❌ فشل / Failed") << "\n";
    std::cout << "\n";
}

// طباعة جدول مقارنة / Print comparison table
void printComparisonTable(const std::vector<BenchmarkResult>& results) {
    printSeparator('-');
    std::cout << std::left << std::setw(40) << "القياس / Benchmark"
              << std::right << std::setw(15) << "الوقت / Time (ms)"
              << std::right << std::setw(15) << "الذاكرة / Memory (KB)"
              << std::right << std::setw(10) << "الحالة / Status" << "\n";
    printSeparator('-');
    
    for (const auto& result : results) {
        std::cout << std::fixed << std::setprecision(3);
        std::cout << std::left << std::setw(40) << result.name
                  << std::right << std::setw(15) << result.avg_time_ms
                  << std::right << std::setw(15) << (result.total_memory_bytes / 1024)
                  << std::right << std::setw(10) << (result.success ? "✅" : "❌") << "\n";
    }
    
    printSeparator('-');
}

// ============================================================================
// Benchmark 1: تهيئة المولد / Generator Initialization
// ============================================================================

BenchmarkResult benchmarkInitialization(size_t iterations = 1000) {
    BenchmarkResult result("تهيئة المولد / Generator Initialization");
    result.iterations = iterations;
    
    std::vector<double> times;
    times.reserve(iterations);
    
    BenchmarkTimer timer;
    
    for (size_t i = 0; i < iterations; ++i) {
        // بدء القياس / Start timing
        timer.start();
        
        // إنشاء مولد جديد / Create new generator
        Sad::JIT::LLVMGenerator generator;
        
        // تهيئة المولد / Initialize generator
        if (!generator.initialize("benchmark_init_" + std::to_string(i))) {
            result.success = false;
            break;
        }
        
        // إيقاف القياس / Stop timing
        double elapsed = timer.stop();
        times.push_back(elapsed);
        
        // تدمير المولد تلقائياً / Generator destroyed automatically
    }
    
    // حساب الإحصائيات / Calculate statistics
    if (!times.empty()) {
        result.total_time_ms = std::accumulate(times.begin(), times.end(), 0.0);
        result.avg_time_ms = result.total_time_ms / times.size();
        result.min_time_ms = *std::min_element(times.begin(), times.end());
        result.max_time_ms = *std::max_element(times.begin(), times.end());
        
        // حساب الانحراف المعياري / Calculate standard deviation
        double variance = 0.0;
        for (double time : times) {
            variance += (time - result.avg_time_ms) * (time - result.avg_time_ms);
        }
        result.stddev_ms = std::sqrt(variance / times.size());
    }
    
    return result;
}

// ============================================================================
// Benchmark 2: توليد IR بسيط / Simple IR Generation
// ============================================================================

BenchmarkResult benchmarkSimpleIRGeneration(size_t iterations = 500) {
    BenchmarkResult result("توليد IR بسيط / Simple IR Generation");
    result.iterations = iterations;
    
    std::vector<double> times;
    times.reserve(iterations);
    
    BenchmarkTimer timer;
    
    for (size_t i = 0; i < iterations; ++i) {
        // إنشاء مولد / Create generator
        Sad::JIT::LLVMGenerator generator;
        if (!generator.initialize("benchmark_simple_ir_" + std::to_string(i))) {
            result.success = false;
            break;
        }
        
        // بدء القياس / Start timing
        timer.start();
        
        // الحصول على IR (يحتوي على وحدة فارغة) / Get IR (contains empty module)
        std::string ir = generator.getIRString();
        
        // إيقاف القياس / Stop timing
        double elapsed = timer.stop();
        times.push_back(elapsed);
        
        // التحقق من النجاح / Verify success
        if (ir.empty()) {
            result.success = false;
            break;
        }
    }
    
    // حساب الإحصائيات / Calculate statistics
    if (!times.empty()) {
        result.total_time_ms = std::accumulate(times.begin(), times.end(), 0.0);
        result.avg_time_ms = result.total_time_ms / times.size();
        result.min_time_ms = *std::min_element(times.begin(), times.end());
        result.max_time_ms = *std::max_element(times.begin(), times.end());
        
        double variance = 0.0;
        for (double time : times) {
            variance += (time - result.avg_time_ms) * (time - result.avg_time_ms);
        }
        result.stddev_ms = std::sqrt(variance / times.size());
    }
    
    return result;
}

// ============================================================================
// Benchmark 3: التحقق من الوحدة / Module Verification
// ============================================================================

BenchmarkResult benchmarkModuleVerification(size_t iterations = 500) {
    BenchmarkResult result("التحقق من الوحدة / Module Verification");
    result.iterations = iterations;
    
    std::vector<double> times;
    times.reserve(iterations);
    
    BenchmarkTimer timer;
    
    for (size_t i = 0; i < iterations; ++i) {
        // إنشاء مولد / Create generator
        Sad::JIT::LLVMGenerator generator;
        if (!generator.initialize("benchmark_verify_" + std::to_string(i))) {
            result.success = false;
            break;
        }
        
        // بدء القياس / Start timing
        timer.start();
        
        // التحقق من الوحدة / Verify module
        std::string error;
        bool verified = generator.verify(&error);
        
        // إيقاف القياس / Stop timing
        double elapsed = timer.stop();
        times.push_back(elapsed);
        
        // التحقق من النجاح / Verify success
        if (!verified) {
            std::cerr << "خطأ في التحقق: " << error << "\n";
            result.success = false;
            break;
        }
    }
    
    // حساب الإحصائيات / Calculate statistics
    if (!times.empty()) {
        result.total_time_ms = std::accumulate(times.begin(), times.end(), 0.0);
        result.avg_time_ms = result.total_time_ms / times.size();
        result.min_time_ms = *std::min_element(times.begin(), times.end());
        result.max_time_ms = *std::max_element(times.begin(), times.end());
        
        double variance = 0.0;
        for (double time : times) {
            variance += (time - result.avg_time_ms) * (time - result.avg_time_ms);
        }
        result.stddev_ms = std::sqrt(variance / times.size());
    }
    
    return result;
}

// ============================================================================
// Benchmark 4: تحسين O0 / O0 Optimization
// ============================================================================

BenchmarkResult benchmarkOptimizationO0(size_t iterations = 200) {
    BenchmarkResult result("تحسين O0 / O0 Optimization");
    result.iterations = iterations;
    
    std::vector<double> times;
    times.reserve(iterations);
    
    BenchmarkTimer timer;
    
    for (size_t i = 0; i < iterations; ++i) {
        // إنشاء مولد / Create generator
        Sad::JIT::LLVMGenerator generator;
        if (!generator.initialize("benchmark_opt_o0_" + std::to_string(i))) {
            result.success = false;
            break;
        }
        
        // بدء القياس / Start timing
        timer.start();
        
        // تطبيق تحسين O0 / Apply O0 optimization
        generator.optimize(0);
        
        // إيقاف القياس / Stop timing
        double elapsed = timer.stop();
        times.push_back(elapsed);
        
        // التحقق من صحة الوحدة بعد التحسين / Verify module after optimization
        std::string error;
        if (!generator.verify(&error)) {
            result.success = false;
            break;
        }
    }
    
    // حساب الإحصائيات / Calculate statistics
    if (!times.empty()) {
        result.total_time_ms = std::accumulate(times.begin(), times.end(), 0.0);
        result.avg_time_ms = result.total_time_ms / times.size();
        result.min_time_ms = *std::min_element(times.begin(), times.end());
        result.max_time_ms = *std::max_element(times.begin(), times.end());
        
        double variance = 0.0;
        for (double time : times) {
            variance += (time - result.avg_time_ms) * (time - result.avg_time_ms);
        }
        result.stddev_ms = std::sqrt(variance / times.size());
    }
    
    return result;
}

// ============================================================================
// Benchmark 5: تحسين O3 / O3 Optimization
// ============================================================================

BenchmarkResult benchmarkOptimizationO3(size_t iterations = 200) {
    BenchmarkResult result("تحسين O3 / O3 Optimization");
    result.iterations = iterations;
    
    std::vector<double> times;
    times.reserve(iterations);
    
    BenchmarkTimer timer;
    
    for (size_t i = 0; i < iterations; ++i) {
        // إنشاء مولد / Create generator
        Sad::JIT::LLVMGenerator generator;
        if (!generator.initialize("benchmark_opt_o3_" + std::to_string(i))) {
            result.success = false;
            break;
        }
        
        // بدء القياس / Start timing
        timer.start();
        
        // تطبيق تحسين O3 / Apply O3 optimization
        generator.optimize(3);
        
        // إيقاف القياس / Stop timing
        double elapsed = timer.stop();
        times.push_back(elapsed);
        
        // التحقق من صحة الوحدة بعد التحسين / Verify module after optimization
        std::string error;
        if (!generator.verify(&error)) {
            result.success = false;
            break;
        }
    }
    
    // حساب الإحصائيات / Calculate statistics
    if (!times.empty()) {
        result.total_time_ms = std::accumulate(times.begin(), times.end(), 0.0);
        result.avg_time_ms = result.total_time_ms / times.size();
        result.min_time_ms = *std::min_element(times.begin(), times.end());
        result.max_time_ms = *std::max_element(times.begin(), times.end());
        
        double variance = 0.0;
        for (double time : times) {
            variance += (time - result.avg_time_ms) * (time - result.avg_time_ms);
        }
        result.stddev_ms = std::sqrt(variance / times.size());
    }
    
    return result;
}

// ============================================================================
// Benchmark 6: وحدات متعددة / Multiple Modules
// ============================================================================

BenchmarkResult benchmarkMultipleModules(size_t module_count = 100) {
    BenchmarkResult result("وحدات متعددة / Multiple Modules (" + std::to_string(module_count) + ")");
    result.iterations = 1;
    
    BenchmarkTimer timer;
    
    // بدء القياس / Start timing
    timer.start();
    
    // إنشاء عدة مولدات / Create multiple generators
    std::vector<std::unique_ptr<Sad::JIT::LLVMGenerator>> generators;
    generators.reserve(module_count);
    
    for (size_t i = 0; i < module_count; ++i) {
        auto gen = std::make_unique<Sad::JIT::LLVMGenerator>();
        if (!gen->initialize("benchmark_multi_" + std::to_string(i))) {
            result.success = false;
            break;
        }
        generators.push_back(std::move(gen));
    }
    
    // إيقاف القياس / Stop timing
    double elapsed = timer.stop();
    
    result.total_time_ms = elapsed;
    result.avg_time_ms = elapsed;
    result.min_time_ms = elapsed;
    result.max_time_ms = elapsed;
    result.stddev_ms = 0.0;
    
    return result;
}

// ============================================================================
// Benchmark 7: دورة كاملة / Full Cycle
// ============================================================================

BenchmarkResult benchmarkFullCycle(size_t iterations = 100) {
    BenchmarkResult result("دورة كاملة / Full Cycle (Init+IR+Verify+Opt)");
    result.iterations = iterations;
    
    std::vector<double> times;
    times.reserve(iterations);
    
    BenchmarkTimer timer;
    
    for (size_t i = 0; i < iterations; ++i) {
        // بدء القياس / Start timing
        timer.start();
        
        // 1. إنشاء مولد / Create generator
        Sad::JIT::LLVMGenerator generator;
        
        // 2. تهيئة / Initialize
        if (!generator.initialize("benchmark_full_" + std::to_string(i))) {
            result.success = false;
            break;
        }
        
        // 3. توليد IR / Generate IR
        std::string ir = generator.getIRString();
        if (ir.empty()) {
            result.success = false;
            break;
        }
        
        // 4. التحقق / Verify
        std::string error;
        if (!generator.verify(&error)) {
            result.success = false;
            break;
        }
        
        // 5. التحسين / Optimize
        generator.optimize(2);
        
        // 6. التحقق النهائي / Final verify
        if (!generator.verify(&error)) {
            result.success = false;
            break;
        }
        
        // إيقاف القياس / Stop timing
        double elapsed = timer.stop();
        times.push_back(elapsed);
    }
    
    // حساب الإحصائيات / Calculate statistics
    if (!times.empty()) {
        result.total_time_ms = std::accumulate(times.begin(), times.end(), 0.0);
        result.avg_time_ms = result.total_time_ms / times.size();
        result.min_time_ms = *std::min_element(times.begin(), times.end());
        result.max_time_ms = *std::max_element(times.begin(), times.end());
        
        double variance = 0.0;
        for (double time : times) {
            variance += (time - result.avg_time_ms) * (time - result.avg_time_ms);
        }
        result.stddev_ms = std::sqrt(variance / times.size());
    }
    
    return result;
}

// ============================================================================
// الدالة الرئيسية / Main Function
// ============================================================================

int main() {
    // تهيئة LLVM / Initialize LLVM
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    
    printHeader("🚀 قياسات أداء LLVM Generator / LLVM Generator Performance Benchmarks");
    
    std::cout << "⏱️  بدء القياسات... / Starting benchmarks...\n\n";
    
    // تخزين النتائج / Store results
    std::vector<BenchmarkResult> results;
    
    // ========================================
    // Benchmark 1: التهيئة / Initialization
    // ========================================
    std::cout << "🔄 قياس 1: تهيئة المولد...\n";
    auto result1 = benchmarkInitialization(1000);
    printResult(result1);
    results.push_back(result1);
    
    // ========================================
    // Benchmark 2: توليد IR / IR Generation
    // ========================================
    std::cout << "🔄 قياس 2: توليد IR بسيط...\n";
    auto result2 = benchmarkSimpleIRGeneration(500);
    printResult(result2);
    results.push_back(result2);
    
    // ========================================
    // Benchmark 3: التحقق / Verification
    // ========================================
    std::cout << "🔄 قياس 3: التحقق من الوحدة...\n";
    auto result3 = benchmarkModuleVerification(500);
    printResult(result3);
    results.push_back(result3);
    
    // ========================================
    // Benchmark 4: تحسين O0 / O0 Optimization
    // ========================================
    std::cout << "🔄 قياس 4: تحسين O0...\n";
    auto result4 = benchmarkOptimizationO0(200);
    printResult(result4);
    results.push_back(result4);
    
    // ========================================
    // Benchmark 5: تحسين O3 / O3 Optimization
    // ========================================
    std::cout << "🔄 قياس 5: تحسين O3...\n";
    auto result5 = benchmarkOptimizationO3(200);
    printResult(result5);
    results.push_back(result5);
    
    // ========================================
    // Benchmark 6: وحدات متعددة / Multiple Modules
    // ========================================
    std::cout << "🔄 قياس 6: وحدات متعددة...\n";
    auto result6 = benchmarkMultipleModules(100);
    printResult(result6);
    results.push_back(result6);
    
    // ========================================
    // Benchmark 7: دورة كاملة / Full Cycle
    // ========================================
    std::cout << "🔄 قياس 7: دورة كاملة...\n";
    auto result7 = benchmarkFullCycle(100);
    printResult(result7);
    results.push_back(result7);
    
    // ========================================
    // ملخص النتائج / Results Summary
    // ========================================
    printHeader("📊 ملخص النتائج / Results Summary");
    printComparisonTable(results);
    
    // ========================================
    // تحليل الأداء / Performance Analysis
    // ========================================
    printHeader("📈 تحليل الأداء / Performance Analysis");
    
    // حساب معدل العمليات في الثانية / Calculate operations per second
    if (result1.success && result1.avg_time_ms > 0) {
        double ops_per_sec = 1000.0 / result1.avg_time_ms;
        std::cout << "⚡ معدل التهيئة / Initialization Rate: "
                  << std::fixed << std::setprecision(0)
                  << ops_per_sec << " ops/sec\n";
    }
    
    if (result7.success && result7.avg_time_ms > 0) {
        double cycles_per_sec = 1000.0 / result7.avg_time_ms;
        std::cout << "⚡ معدل الدورة الكاملة / Full Cycle Rate: "
                  << std::fixed << std::setprecision(0)
                  << cycles_per_sec << " cycles/sec\n";
    }
    
    // مقارنة O0 vs O3 / Compare O0 vs O3
    if (result4.success && result5.success) {
        double overhead = (result5.avg_time_ms - result4.avg_time_ms) / result4.avg_time_ms * 100.0;
        std::cout << "\n🔍 نسبة التكلفة الإضافية O3 vs O0 / O3 vs O0 Overhead: "
                  << std::fixed << std::setprecision(1)
                  << overhead << "%\n";
    }
    
    // ========================================
    // الخلاصة / Conclusion
    // ========================================
    printHeader("✅ الخلاصة / Conclusion");
    
    size_t success_count = 0;
    for (const auto& res : results) {
        if (res.success) {
            ++success_count;
        }
    }
    
    std::cout << "نجحت " << success_count << " من " << results.size() << " قياسات\n";
    std::cout << success_count << " out of " << results.size() << " benchmarks succeeded\n\n";
    
    if (success_count == results.size()) {
        std::cout << "🎉 جميع القياسات نجحت! / All benchmarks succeeded!\n";
        return 0;
    } else {
        std::cout << "⚠️  بعض القياسات فشلت / Some benchmarks failed\n";
        return 1;
    }
}

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
// ملاحظات التنفيذ / Implementation Notes:
// - قياسات دقيقة باستخدام high_resolution_clock
// - حساب إحصائيات شاملة (متوسط، أدنى، أقصى، انحراف معياري)
// - تغطية جميع العمليات الأساسية في LLVMGenerator
// - مقارنة بين مستويات التحسين
// - قياس أداء الوحدات المتعددة
// ============================================================================
