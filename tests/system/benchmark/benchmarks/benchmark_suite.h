/**
 * @file benchmark_suite.h
 * @brief مجموعة اختبارات الأداء - 10 معايير مقارنة
 * @brief (EN) Performance benchmark suite - 10 benchmarks vs Python/JS
 * 
 * @author أحمد
 * @date نوفمبر 2026
 */

#ifndef SAD_BENCHMARK_SUITE_H
#define SAD_BENCHMARK_SUITE_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;chrono&gt;
#include &lt;functional&gt;
#include &lt;map&gt;
#include &lt;cmath&gt;
#include &lt;fstream&gt;
#include &lt;iomanip&gt;

namespace Sad {
namespace Benchmark {

//==============================================================================
// أنواع البيانات
//==============================================================================

/**
 * @brief نتيجة تشغيل واحد
 */
struct RunResult {
    double durationMs;
    size_t memoryBytes;
    uint64_t iterations;
};

/**
 * @brief إحصائيات المعيار
 */
struct BenchmarkStats {
    std::string name;
    std::string description;
    
    double minMs;
    double maxMs;
    double avgMs;
    double medianMs;
    double stddevMs;
    
    size_t peakMemoryBytes;
    uint64_t totalIterations;
    int runs;
    
    // مقارنة مع لغات أخرى
    double pythonRatioEstimate;  // أسرع من Python بـ X مرات
    double jsRatioEstimate;      // أسرع من JS بـ X مرات
};

//==============================================================================
// معيار أداء
//==============================================================================

/**
 * @brief معيار أداء واحد
 */
class Benchmark {
public:
    using BenchmarkFn = std::function&lt;void(uint64_t iterations)&gt;;
    
    Benchmark(const std::string&amp; name, const std::string&amp; desc)
        : name_(name), description_(desc) {}
    
    void setSetup(std::function&lt;void()&gt; fn) { setup_ = fn; }
    void setTeardown(std::function&lt;void()&gt; fn) { teardown_ = fn; }
    void setBenchmark(BenchmarkFn fn) { benchmark_ = fn; }
    
    /**
     * @brief تشغيل المعيار
     */
    BenchmarkStats run(int runs = 10, uint64_t iterations = 1000000) {
        stats_.name = name_;
        stats_.description = description_;
        stats_.runs = runs;
        stats_.totalIterations = iterations * runs;
        
        std::vector&lt;double&gt; times;
        
        for (int i = 0; i &lt; runs; ++i) {
            if (setup_) setup_();
            
            auto start = std::chrono::high_resolution_clock::now();
            benchmark_(iterations);
            auto end = std::chrono::high_resolution_clock::now();
            
            double ms = std::chrono::duration&lt;double, std::milli&gt;(end - start).count();
            times.push_back(ms);
            
            if (teardown_) teardown_();
        }
        
        // حساب الإحصائيات
        computeStats(times);
        
        return stats_;
    }
    
    const std::string&amp; getName() const { return name_; }

private:
    std::string name_;
    std::string description_;
    std::function&lt;void()&gt; setup_;
    std::function&lt;void()&gt; teardown_;
    BenchmarkFn benchmark_;
    BenchmarkStats stats_;
    
    void computeStats(std::vector&lt;double&gt;&amp; times) {
        std::sort(times.begin(), times.end());
        
        stats_.minMs = times.front();
        stats_.maxMs = times.back();
        stats_.medianMs = times[times.size() / 2];
        
        double sum = 0;
        for (double t : times) sum += t;
        stats_.avgMs = sum / times.size();
        
        double variance = 0;
        for (double t : times) {
            variance += (t - stats_.avgMs) * (t - stats_.avgMs);
        }
        stats_.stddevMs = std::sqrt(variance / times.size());
    }
};

//==============================================================================
// مجموعة المعايير
//==============================================================================

/**
 * @brief مجموعة معايير الأداء
 */
class BenchmarkSuite {
public:
    /**
     * @brief إضافة معيار
     */
    void add(Benchmark&amp;&amp; benchmark) {
        benchmarks_.push_back(std::move(benchmark));
    }
    
    /**
     * @brief تشغيل جميع المعايير
     */
    std::vector&lt;BenchmarkStats&gt; runAll(int runs = 10) {
        std::vector&lt;BenchmarkStats&gt; results;
        
        for (auto&amp; b : benchmarks_) {
            std::cout &lt;&lt; "تشغيل: " &lt;&lt; b.getName() &lt;&lt; "..." &lt;&lt; std::flush;
            auto stats = b.run(runs);
            results.push_back(stats);
            std::cout &lt;&lt; " " &lt;&lt; stats.avgMs &lt;&lt; "ms\n";
        }
        
        return results;
    }
    
    /**
     * @brief تصدير النتائج
     */
    static void exportResults(const std::vector&lt;BenchmarkStats&gt;&amp; results,
                             const std::string&amp; format,
                             const std::string&amp; path) {
        if (format == "json") {
            exportJson(results, path);
        } else if (format == "csv") {
            exportCsv(results, path);
        } else if (format == "markdown") {
            exportMarkdown(results, path);
        } else if (format == "html") {
            exportHtml(results, path);
        }
    }

private:
    std::vector&lt;Benchmark&gt; benchmarks_;
    
    static void exportJson(const std::vector&lt;BenchmarkStats&gt;&amp; results, const std::string&amp; path);
    static void exportCsv(const std::vector&lt;BenchmarkStats&gt;&amp; results, const std::string&amp; path);
    static void exportMarkdown(const std::vector&lt;BenchmarkStats&gt;&amp; results, const std::string&amp; path);
    static void exportHtml(const std::vector&lt;BenchmarkStats&gt;&amp; results, const std::string&amp; path);
};

//==============================================================================
// المعايير العشرة القياسية
//==============================================================================

/**
 * @brief إنشاء المعايير العشرة القياسية
 */
class StandardBenchmarks {
public:
    static BenchmarkSuite create() {
        BenchmarkSuite suite;
        
        // 1. فيبوناتشي تكراري
        suite.add(createFibonacci());
        
        // 2. ترتيب سريع
        suite.add(createQuicksort());
        
        // 3. معالجة نصوص
        suite.add(createStringProcessing());
        
        // 4. عمليات مصفوفات
        suite.add(createArrayOperations());
        
        // 5. عمليات خرائط
        suite.add(createMapOperations());
        
        // 6. كائنات
        suite.add(createObjectCreation());
        
        // 7. استدعاءات دوال
        suite.add(createFunctionCalls());
        
        // 8. حلقات متداخلة
        suite.add(createNestedLoops());
        
        // 9. تعبيرات نمطية (Regex)
        suite.add(createRegex());
        
        // 10. عمليات ملفات
        suite.add(createFileIO());
        
        return suite;
    }

private:
    static Benchmark createFibonacci() {
        Benchmark b("fibonacci", "حساب فيبوناتشي التكراري");
        b.setBenchmark([](uint64_t iterations) {
            auto fib = [](int n, auto&amp;&amp; self) -&gt; int64_t {
                if (n &lt;= 1) return n;
                return self(n-1, self) + self(n-2, self);
            };
            for (uint64_t i = 0; i &lt; iterations; ++i) {
                volatile auto r = fib(25, fib);
                (void)r;
            }
        });
        return b;
    }
    
    static Benchmark createQuicksort() {
        Benchmark b("quicksort", "ترتيب سريع لـ 10000 عنصر");
        std::vector&lt;int&gt; data;
        
        b.setSetup([&amp;data]() {
            data.resize(10000);
            for (int i = 0; i &lt; 10000; ++i) {
                data[i] = rand() % 100000;
            }
        });
        
        b.setBenchmark([&amp;data](uint64_t iterations) {
            for (uint64_t i = 0; i &lt; iterations / 100; ++i) {
                std::vector&lt;int&gt; copy = data;
                std::sort(copy.begin(), copy.end());
            }
        });
        return b;
    }
    
    static Benchmark createStringProcessing() {
        Benchmark b("string_ops", "معالجة نصوص: ربط وتقسيم");
        b.setBenchmark([](uint64_t iterations) {
            for (uint64_t i = 0; i &lt; iterations; ++i) {
                std::string s = "مرحباً بالعالم العربي ";
                for (int j = 0; j &lt; 10; ++j) {
                    s += "نص إضافي ";
                }
                volatile auto len = s.size();
                (void)len;
            }
        });
        return b;
    }
    
    static Benchmark createArrayOperations() {
        Benchmark b("array_ops", "عمليات مصفوفات: إضافة، حذف، بحث");
        b.setBenchmark([](uint64_t iterations) {
            for (uint64_t i = 0; i &lt; iterations / 10; ++i) {
                std::vector&lt;int&gt; arr;
                for (int j = 0; j &lt; 1000; ++j) {
                    arr.push_back(j);
                }
                auto it = std::find(arr.begin(), arr.end(), 500);
                volatile bool found = (it != arr.end());
                (void)found;
            }
        });
        return b;
    }
    
    static Benchmark createMapOperations() {
        Benchmark b("map_ops", "عمليات خرائط: إدراج، بحث، حذف");
        b.setBenchmark([](uint64_t iterations) {
            for (uint64_t i = 0; i &lt; iterations / 10; ++i) {
                std::map&lt;std::string, int&gt; m;
                for (int j = 0; j &lt; 1000; ++j) {
                    m["key_" + std::to_string(j)] = j;
                }
                volatile auto it = m.find("key_500");
                (void)it;
            }
        });
        return b;
    }
    
    static Benchmark createObjectCreation() {
        Benchmark b("objects", "إنشاء كائنات");
        
        struct Point {
            double x, y, z;
            Point(double a, double b, double c) : x(a), y(b), z(c) {}
            double distance() { return std::sqrt(x*x + y*y + z*z); }
        };
        
        b.setBenchmark([](uint64_t iterations) {
            for (uint64_t i = 0; i &lt; iterations; ++i) {
                struct Point { double x, y, z; };
                Point p{1.0, 2.0, 3.0};
                volatile auto d = std::sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
                (void)d;
            }
        });
        return b;
    }
    
    static Benchmark createFunctionCalls() {
        Benchmark b("function_calls", "استدعاءات دوال متداخلة");
        b.setBenchmark([](uint64_t iterations) {
            auto add = [](int a, int b) { return a + b; };
            auto mul = [&amp;add](int a, int b) { return add(a, 0) * b; };
            auto complex = [&amp;mul](int x) {
                return mul(x, 2) + mul(x, 3) + mul(x, 4);
            };
            
            for (uint64_t i = 0; i &lt; iterations; ++i) {
                volatile auto r = complex(static_cast&lt;int&gt;(i % 100));
                (void)r;
            }
        });
        return b;
    }
    
    static Benchmark createNestedLoops() {
        Benchmark b("nested_loops", "حلقات متداخلة 3 مستويات");
        b.setBenchmark([](uint64_t iterations) {
            volatile int64_t sum = 0;
            int n = static_cast&lt;int&gt;(std::cbrt(iterations));
            for (int i = 0; i &lt; n; ++i) {
                for (int j = 0; j &lt; n; ++j) {
                    for (int k = 0; k &lt; n; ++k) {
                        sum += i * j * k;
                    }
                }
            }
            (void)sum;
        });
        return b;
    }
    
    static Benchmark createRegex() {
        Benchmark b("regex", "مطابقة تعبيرات نمطية");
        b.setBenchmark([](uint64_t iterations) {
            std::regex pattern(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b)");
            std::string texts[] = {
                "test@example.com",
                "not an email",
                "user.name+tag@domain.co.uk",
                "invalid@",
                "valid123@test.org"
            };
            
            for (uint64_t i = 0; i &lt; iterations / 100; ++i) {
                for (const auto&amp; text : texts) {
                    volatile bool match = std::regex_search(text, pattern);
                    (void)match;
                }
            }
        });
        return b;
    }
    
    static Benchmark createFileIO() {
        Benchmark b("file_io", "قراءة وكتابة ملفات");
        b.setBenchmark([](uint64_t iterations) {
            std::string filename = "benchmark_temp.txt";
            std::string data(1000, 'X');
            
            for (uint64_t i = 0; i &lt; iterations / 1000; ++i) {
                {
                    std::ofstream out(filename);
                    out &lt;&lt; data;
                }
                {
                    std::ifstream in(filename);
                    std::string content;
                    std::getline(in, content);
                    volatile auto len = content.size();
                    (void)len;
                }
            }
            std::remove(filename.c_str());
        });
        return b;
    }
};

//==============================================================================
// واجهة سطر الأوامر
//==============================================================================

/**
 * @brief تشغيل المعايير من سطر الأوامر
 */
inline int runBenchmarkCLI(int argc, char** argv) {
    int runs = 10;
    std::string format = "markdown";
    std::string output = "benchmark_results.md";
    
    for (int i = 1; i &lt; argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--runs" &amp;&amp; i + 1 &lt; argc) {
            runs = std::stoi(argv[++i]);
        } else if (arg == "--format" &amp;&amp; i + 1 &lt; argc) {
            format = argv[++i];
        } else if (arg == "--output" &amp;&amp; i + 1 &lt; argc) {
            output = argv[++i];
        }
    }
    
    std::cout &lt;&lt; "=== مجموعة معايير الأداء - لغة ص ===\n\n";
    
    auto suite = StandardBenchmarks::create();
    auto results = suite.runAll(runs);
    
    BenchmarkSuite::exportResults(results, format, output);
    
    std::cout &lt;&lt; "\nتم حفظ النتائج في: " &lt;&lt; output &lt;&lt; "\n";
    
    return 0;
}

} // namespace Benchmark
} // namespace Sad

#endif // SAD_BENCHMARK_SUITE_H
