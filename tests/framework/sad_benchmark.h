/**
 * @file sad_benchmark.h
 * @brief (AR) إطار قياس الأداء للغة ص — قياس السرعة والذاكرة
 * @brief (EN) Performance benchmark framework for Sad Language
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  إطار قياس الأداء لغة ص / Sad Benchmark Framework                         ║
 * ║                                                                           ║
 * ║  يوفر:                                                                    ║
 * ║  • قياس وقت التنفيذ (مللي ثانية / ثانية)                                 ║
 * ║  • قياس استهلاك الذاكرة                                                  ║
 * ║  • مقارنة مع baseline سابق                                               ║
 * ║  • تصدير JSON للتتبع في CI                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * @example
 * ```cpp
 * #include "sad_benchmark.h"
 * 
 * BENCHMARK(Lexer, تحليل_ملف_كبير) {
 *     for (auto _ : state) {
 *         lex(largeFile);
 *     }
 * }
 * 
 * BENCHMARK(Parser, تحليل_1000_دالة)->Iterations(100);
 * 
 * int main(int argc, char** argv) {
 *     return SAD_RUN_BENCHMARKS();
 * }
 * ```
 * 
 * @author عمر — مهندس البنية التحتية / Omar — Infrastructure Engineer
 * @date 2026-03-06
 */

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <map>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <psapi.h>
    #undef VOID
    #undef ERROR
    #undef TRUE
    #undef FALSE
    
    inline void sad_bench_enable_colors() {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode;
        GetConsoleMode(h, &mode);
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        SetConsoleOutputCP(65001);
    }
    
    inline size_t sad_get_memory_usage() {
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize;
        }
        return 0;
    }
#else
    #include <sys/resource.h>
    
    inline void sad_bench_enable_colors() {}
    
    inline size_t sad_get_memory_usage() {
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            return usage.ru_maxrss * 1024; // KB to bytes
        }
        return 0;
    }
#endif

namespace SadBench {

// ══════════════════════════════════════════════════════════════════════════════
// Colors
// ══════════════════════════════════════════════════════════════════════════════
namespace Color {
    constexpr const char* RESET   = "\033[0m";
    constexpr const char* RED     = "\033[31m";
    constexpr const char* GREEN   = "\033[32m";
    constexpr const char* YELLOW  = "\033[33m";
    constexpr const char* BLUE    = "\033[34m";
    constexpr const char* CYAN    = "\033[36m";
    constexpr const char* BOLD    = "\033[1m";
    constexpr const char* DIM     = "\033[2m";
}

// ══════════════════════════════════════════════════════════════════════════════
// نتيجة قياس فردي / Single Benchmark Result
// ══════════════════════════════════════════════════════════════════════════════
struct BenchmarkResult {
    std::string suite;
    std::string name;
    int iterations;
    double totalTimeMs;     // إجمالي الوقت
    double avgTimeMs;       // متوسط الوقت لكل تكرار
    double minTimeMs;       // أقل وقت
    double maxTimeMs;       // أعلى وقت
    double stdDevMs;        // الانحراف المعياري
    size_t memoryBytes;     // استهلاك الذاكرة
    double opsPerSecond;    // عمليات في الثانية
};

// ══════════════════════════════════════════════════════════════════════════════
// حالة التكرار / Iteration State
// ══════════════════════════════════════════════════════════════════════════════
class State {
public:
    class Iterator {
    public:
        Iterator(int i) : i_(i) {}
        bool operator!=(const Iterator& other) const { return i_ != other.i_; }
        void operator++() { ++i_; }
        int operator*() const { return i_; }
    private:
        int i_;
    };

    State(int iters) : iterations_(iters) {}

    Iterator begin() const { return Iterator(0); }
    Iterator end() const { return Iterator(iterations_); }

    int iterations() const { return iterations_; }

private:
    int iterations_;
};

// ══════════════════════════════════════════════════════════════════════════════
// إعدادات القياس / Benchmark Configuration
// ══════════════════════════════════════════════════════════════════════════════
struct BenchmarkConfig {
    int iterations = 1000;      // عدد التكرارات الافتراضي
    int warmupRuns = 3;         // تشغيلات تسخين
    bool measureMemory = true;  // قياس الذاكرة
};

// ══════════════════════════════════════════════════════════════════════════════
// محرك القياس / Benchmark Runner
// ══════════════════════════════════════════════════════════════════════════════
class BenchmarkRunner {
public:
    static BenchmarkRunner& instance() {
        static BenchmarkRunner inst;
        return inst;
    }

    void registerBenchmark(const std::string& suite, const std::string& name,
                           std::function<void(State&)> fn, BenchmarkConfig config = {}) {
        benchmarks_.push_back({suite, name, fn, config});
    }

    int run(int argc = 0, char** argv = nullptr) {
        sad_bench_enable_colors();
        parseArgs(argc, argv);

        std::cout << Color::BOLD << Color::BLUE
                  << "\n╔════════════════════════════════════════════════════════════╗\n"
                  << "║  قياس أداء لغة ص / Sad Language Benchmarks                  ║\n"
                  << "╚════════════════════════════════════════════════════════════╝"
                  << Color::RESET << "\n\n";

        std::string lastSuite;
        for (auto& bench : benchmarks_) {
            // فلترة
            if (!filter_.empty() && bench.name.find(filter_) == std::string::npos &&
                bench.suite.find(filter_) == std::string::npos) {
                continue;
            }

            // طباعة رأس المجموعة
            if (bench.suite != lastSuite) {
                lastSuite = bench.suite;
                std::cout << Color::CYAN << "━━━ " << bench.suite << " ━━━" 
                          << Color::RESET << "\n";
            }

            runSingleBenchmark(bench);
        }

        printSummary();
        return 0;
    }

    void loadBaseline(const std::string& path) {
        // تحميل baseline من ملف JSON سابق
        std::ifstream file(path);
        if (!file.is_open()) return;
        
        // TODO: parse JSON baseline
        baselinePath_ = path;
    }

    void exportJSON(const std::string& path) {
        std::ofstream out(path);
        out << "{\n  \"date\": \"" << getCurrentDate() << "\",\n";
        out << "  \"benchmarks\": [\n";
        
        for (size_t i = 0; i < results_.size(); ++i) {
            auto& r = results_[i];
            out << "    {\n";
            out << "      \"suite\": \"" << escapeJSON(r.suite) << "\",\n";
            out << "      \"name\": \"" << escapeJSON(r.name) << "\",\n";
            out << "      \"iterations\": " << r.iterations << ",\n";
            out << "      \"avgTimeMs\": " << r.avgTimeMs << ",\n";
            out << "      \"minTimeMs\": " << r.minTimeMs << ",\n";
            out << "      \"maxTimeMs\": " << r.maxTimeMs << ",\n";
            out << "      \"stdDevMs\": " << r.stdDevMs << ",\n";
            out << "      \"memoryBytes\": " << r.memoryBytes << ",\n";
            out << "      \"opsPerSecond\": " << r.opsPerSecond << "\n";
            out << "    }" << (i < results_.size() - 1 ? "," : "") << "\n";
        }
        
        out << "  ]\n}\n";
    }

    const std::vector<BenchmarkResult>& results() const { return results_; }

private:
    BenchmarkRunner() = default;

    struct BenchmarkEntry {
        std::string suite;
        std::string name;
        std::function<void(State&)> fn;
        BenchmarkConfig config;
    };

    std::vector<BenchmarkEntry> benchmarks_;
    std::vector<BenchmarkResult> results_;
    std::string filter_;
    std::string baselinePath_;
    std::map<std::string, double> baseline_;

    void parseArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.rfind("--filter=", 0) == 0) filter_ = arg.substr(9);
            else if (arg.rfind("-f=", 0) == 0) filter_ = arg.substr(3);
            else if (arg.rfind("--baseline=", 0) == 0) loadBaseline(arg.substr(11));
        }
    }

    void runSingleBenchmark(BenchmarkEntry& bench) {
        std::cout << "  " << bench.name << " ... " << std::flush;

        // Warmup
        for (int w = 0; w < bench.config.warmupRuns; ++w) {
            State warmupState(1);
            bench.fn(warmupState);
        }

        // القياس الفعلي
        std::vector<double> times;
        times.reserve(bench.config.iterations);

        size_t memBefore = bench.config.measureMemory ? sad_get_memory_usage() : 0;

        for (int i = 0; i < bench.config.iterations; ++i) {
            State state(1);
            auto start = std::chrono::high_resolution_clock::now();
            bench.fn(state);
            auto end = std::chrono::high_resolution_clock::now();
            double ms = std::chrono::duration<double, std::milli>(end - start).count();
            times.push_back(ms);
        }

        size_t memAfter = bench.config.measureMemory ? sad_get_memory_usage() : 0;

        // حساب الإحصائيات
        BenchmarkResult result;
        result.suite = bench.suite;
        result.name = bench.name;
        result.iterations = bench.config.iterations;
        result.totalTimeMs = std::accumulate(times.begin(), times.end(), 0.0);
        result.avgTimeMs = result.totalTimeMs / bench.config.iterations;
        result.minTimeMs = *std::min_element(times.begin(), times.end());
        result.maxTimeMs = *std::max_element(times.begin(), times.end());
        
        // الانحراف المعياري
        double variance = 0;
        for (double t : times) {
            variance += (t - result.avgTimeMs) * (t - result.avgTimeMs);
        }
        result.stdDevMs = std::sqrt(variance / bench.config.iterations);
        
        result.memoryBytes = memAfter > memBefore ? memAfter - memBefore : 0;
        result.opsPerSecond = result.avgTimeMs > 0 ? 1000.0 / result.avgTimeMs : 0;

        results_.push_back(result);

        // طباعة النتيجة
        std::cout << Color::GREEN << std::fixed << std::setprecision(3)
                  << result.avgTimeMs << " ms" << Color::RESET
                  << Color::DIM << " (±" << std::setprecision(2) << result.stdDevMs 
                  << " ms, " << formatOps(result.opsPerSecond) << ")" 
                  << Color::RESET;

        // مقارنة مع baseline
        auto key = bench.suite + "." + bench.name;
        if (baseline_.count(key)) {
            double baselineMs = baseline_[key];
            double change = ((result.avgTimeMs - baselineMs) / baselineMs) * 100;
            if (change > 5) {
                std::cout << Color::RED << " ▲" << std::setprecision(1) << change << "%" << Color::RESET;
            } else if (change < -5) {
                std::cout << Color::GREEN << " ▼" << std::setprecision(1) << -change << "%" << Color::RESET;
            }
        }

        std::cout << "\n";
    }

    void printSummary() {
        std::cout << "\n" << Color::BOLD
                  << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
                  << "  ملخص / Summary\n"
                  << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
                  << Color::RESET << "\n";

        std::cout << "  عدد القياسات / Benchmarks: " << results_.size() << "\n";

        if (!results_.empty()) {
            // الأسرع
            auto fastest = std::min_element(results_.begin(), results_.end(),
                [](const auto& a, const auto& b) { return a.avgTimeMs < b.avgTimeMs; });
            std::cout << Color::GREEN << "  الأسرع / Fastest: " << Color::RESET
                      << fastest->name << " (" << std::setprecision(3) << fastest->avgTimeMs << " ms)\n";

            // الأبطأ
            auto slowest = std::max_element(results_.begin(), results_.end(),
                [](const auto& a, const auto& b) { return a.avgTimeMs < b.avgTimeMs; });
            std::cout << Color::YELLOW << "  الأبطأ / Slowest: " << Color::RESET
                      << slowest->name << " (" << std::setprecision(3) << slowest->avgTimeMs << " ms)\n";
        }

        std::cout << "\n";
    }

    static std::string formatOps(double ops) {
        std::ostringstream ss;
        if (ops >= 1e6) {
            ss << std::setprecision(2) << (ops / 1e6) << "M ops/s";
        } else if (ops >= 1e3) {
            ss << std::setprecision(2) << (ops / 1e3) << "K ops/s";
        } else {
            ss << std::setprecision(2) << ops << " ops/s";
        }
        return ss.str();
    }

    static std::string escapeJSON(const std::string& s) {
        std::string out;
        for (char c : s) {
            switch (c) {
                case '"':  out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\n': out += "\\n";  break;
                default:   out += c;
            }
        }
        return out;
    }

    static std::string getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&t);
        std::ostringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// ══════════════════════════════════════════════════════════════════════════════
// Builder pattern للإعدادات المتقدمة
// ══════════════════════════════════════════════════════════════════════════════
class BenchmarkBuilder {
public:
    BenchmarkBuilder(const std::string& suite, const std::string& name,
                     std::function<void(State&)> fn)
        : suite_(suite), name_(name), fn_(fn) {}

    BenchmarkBuilder& Iterations(int n) { config_.iterations = n; return *this; }
    BenchmarkBuilder& WarmupRuns(int n) { config_.warmupRuns = n; return *this; }
    BenchmarkBuilder& MeasureMemory(bool m) { config_.measureMemory = m; return *this; }

    ~BenchmarkBuilder() {
        BenchmarkRunner::instance().registerBenchmark(suite_, name_, fn_, config_);
    }

private:
    std::string suite_;
    std::string name_;
    std::function<void(State&)> fn_;
    BenchmarkConfig config_;
};

// ══════════════════════════════════════════════════════════════════════════════
// Macros
// ══════════════════════════════════════════════════════════════════════════════

#define SAD_BENCH_CONCAT_(a, b) a##b
#define SAD_BENCH_CONCAT(a, b) SAD_BENCH_CONCAT_(a, b)

#define BENCHMARK(Suite, Name) \
    static void SAD_BENCH_CONCAT(bench_##Suite##_##Name, _fn)(SadBench::State&); \
    static SadBench::BenchmarkBuilder SAD_BENCH_CONCAT(bench_##Suite##_##Name, _builder)( \
        #Suite, #Name, SAD_BENCH_CONCAT(bench_##Suite##_##Name, _fn)); \
    static void SAD_BENCH_CONCAT(bench_##Suite##_##Name, _fn)(SadBench::State& state)

#define SAD_RUN_BENCHMARKS() SadBench::BenchmarkRunner::instance().run(argc, argv)

} // namespace SadBench
