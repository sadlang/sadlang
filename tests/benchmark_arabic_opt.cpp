// ============================================================================
// benchmark_arabic_opt.cpp
// Real-World Benchmarks for Arabic Optimizations
// ============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// ============================================================================
// Test Data - Real Sad Code Samples
// ============================================================================

struct TestCase {
    string name;
    string code;
    string description;
};

vector<TestCase> test_cases = {
    {
        "String Operations",
        R"(
دالة رئيسية() 
    متغير اسم = "محمد";
    متغير رسالة = "مرحباً " + اسم + "!";
    متغير ترحيب = "أهلاً " + اسم;
    متغير وداع = "إلى اللقاء " + اسم;
    متغير نص1 = "اللغة العربية";
    متغير نص2 = "لغة البرمجة";
    متغير نص3 = "اللغة العربية"; # duplicate
    اطبع(رسالة);
نهاية 
)",
        "String concatenation and pooling"
    },
    
    {
        "Pattern Matching",
        R"(
دالة معالج_الحالة(حالة) 
    طابق حالة 
        عندما "بداية" => اطبع("البداية"),
        عندما "وسط" => اطبع("الوسط"),
        عندما "نهاية" => اطبع("النهاية"),
        عندما "توقف" => اطبع("توقف"),
        عندما "استمرار" => اطبع("استمرار"),
        عندما "خطأ" => اطبع("خطأ"),
        عندما "نجاح" => اطبع("نجاح"),
        عندما "تحذير" => اطبع("تحذير"),
        عندما "معلومات" => اطبع("معلومات"),
        عندما "تصحيح" => اطبع("تصحيح"),
        _ => اطبع("غير معروف")
    نهاية
نهاية
)",
        "Pattern matching with 10+ cases (jump table)"
    },
    
    {
        "UTF-8 Text Processing",
        R"(
دالة معالج_نص(نص) 
    متغير طول = طول_نص(نص);
    متغير عربي = هل_حرف_عربي(نص[0]);
    متغير رقم = هل_رقم(نص[0]);
    متغير مسافة = هل_مسافة(نص[0]);
    
    لكل حرف في نص 
        إذا هل_حرف_عربي(حرف) 
            اطبع("حرف عربي: " + حرف);
         وإلا_إذا هل_رقم(حرف) 
            اطبع("رقم: " + حرف);
        نهاية
    نهاية
نهاية
)",
        "UTF-8 operations and Arabic char classification"
    },
    
    {
        "Complex Application",
        R"(
دالة رئيسية() {
    متغير اسم_المستخدم = "أحمد";
    متغير كلمة_المرور = "سري123";
    متغير البريد = "ahmad@example.com";
    متغير رسالة_الترحيب = "مرحباً " + اسم_المستخدم + "!";
    
    إذا طول(كلمة_المرور) >= 6 {
        متغير الحالة = "نجاح";
        طابق الحالة {
            عندما "نجاح" => اطبع(رسالة_الترحيب),
            عندما "خطأ" => اطبع("فشل"),
            _ => اطبع("غير معروف")
        };
    } وإلا {
        اطبع("كلمة المرور قصيرة جداً");
    };
    
    متغير القائمة = ["عنصر1", "عنصر2", "عنصر3"];
    لكل عنصر في القائمة {
        اطبع("معالجة: " + عنصر);
    };
}
)",
        "Complex real-world scenario with multiple optimizations"
    },
    
    {
        "Graphics Application",
        R"(
استورد رسومات من "graphics/ui";

دالة رئيسية() {
    متغير اسم = "";
    متغير كلمة_المرور = "";
    متغير الحالة = "مرحباً بك! الرجاء تسجيل الدخول";
    
    متغير النافذة = رسومات.نافذة.جديد({
        العنوان: "🔐 تطبيق تسجيل الدخول",
        العرض: 900,
        الارتفاع: 700
    });
    
    متغير العنوان = رسومات.تسمية.جديدة({
        النص: "🔐 نظام تسجيل الدخول الآمن",
        المحاذاة: "وسط_وسط"
    });
    
    متغير زر_الدخول = رسومات.زر.جديد({
        النص: "🔓 تسجيل الدخول",
        عند_النقر: دالة() {
            اطبع("محاولة تسجيل الدخول");
        }
    });
}
)",
        "Graphics application with UI elements"
    },
    
    {
        "Data Processing",
        R"(
دالة معالج_بيانات(بيانات) {
    متغير النتائج = [];
    
    لكل عنصر في بيانات {
        إذا عنصر.النوع == "نص" {
            متغير معالج = نظف_نص(عنصر.القيمة);
            إذا طول(معالج) > 0 {
                أضف(النتائج, معالج);
            };
        } وإلا_إذا عنصر.النوع == "رقم" {
            إذا عنصر.القيمة > 0 {
                أضف(النتائج, عنصر.القيمة * 2);
            };
        } وإلا_إذا عنصر.النوع == "قائمة" {
            لكل فرعي في عنصر.القيمة {
                أضف(النتائج, فرعي);
            };
        };
    };
    
    ارجع النتائج;
}

دالة نظف_نص(نص) {
    متغير نتيجة = "";
    لكل حرف في نص {
        إذا هل_حرف_عربي(حرف) أو هل_رقم(حرف) {
            نتيجة = نتيجة + حرف;
        };
    };
    ارجع نتيجة;
}
)",
        "Data processing with nested loops and conditions"
    }
};

// ============================================================================
// Benchmark Infrastructure
// ============================================================================

struct BenchmarkResult {
    string test_name;
    int iterations;
    double time_ms;
    double time_per_iteration_us;
    size_t code_size;
};

class Benchmark {
public:
    static BenchmarkResult run(const TestCase& test, int iterations = 1000) {
        BenchmarkResult result;
        result.test_name = test.name;
        result.iterations = iterations;
        result.code_size = test.code.size();
        
        // Simulate optimization analysis
        auto start = high_resolution_clock::now();
        
        for (int i = 0; i < iterations; i++) {
            analyzeCode(test.code);
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        result.time_ms = duration.count() / 1000.0;
        result.time_per_iteration_us = duration.count() / static_cast<double>(iterations);
        
        return result;
    }
    
private:
    static void analyzeCode(const string& code) {
        // Simulate what optimizers would do:
        
        // 1. String pooling
        countStringLiterals(code);
        
        // 2. Pattern matching analysis
        countPatternCases(code);
        
        // 3. UTF-8 operations
        countUTF8Ops(code);
        
        // 4. Function calls
        countFunctionCalls(code);
    }
    
    static int countStringLiterals(const string& code) {
        int count = 0;
        bool in_string = false;
        
        for (size_t i = 0; i < code.size(); i++) {
            if (code[i] == '"' && (i == 0 || code[i-1] != '\\')) {
                if (!in_string) {
                    count++;
                }
                in_string = !in_string;
            }
        }
        
        return count;
    }
    
    static int countPatternCases(const string& code) {
        int count = 0;
        size_t pos = 0;
        
        while ((pos = code.find("عندما", pos)) != string::npos) {
            count++;
            pos += 6; // length of "عندما"
        }
        
        return count;
    }
    
    static int countUTF8Ops(const string& code) {
        int count = 0;
        
        // Count UTF-8 function calls
        vector<string> utf8_funcs = {
            "هل_حرف_عربي", "هل_رقم", "هل_مسافة",
            "طول_نص", "طول"
        };
        
        for (const auto& func : utf8_funcs) {
            size_t pos = 0;
            while ((pos = code.find(func, pos)) != string::npos) {
                count++;
                pos += func.length();
            }
        }
        
        return count;
    }
    
    static int countFunctionCalls(const string& code) {
        int count = 0;
        size_t pos = 0;
        
        while ((pos = code.find("(", pos)) != string::npos) {
            count++;
            pos++;
        }
        
        return count;
    }
};

// ============================================================================
// Analysis & Statistics
// ============================================================================

struct Statistics {
    double mean;
    double min;
    double max;
    double total;
};

Statistics calculateStats(const vector<BenchmarkResult>& results) {
    Statistics stats;
    
    if (results.empty()) {
        return stats;
    }
    
    stats.min = results[0].time_per_iteration_us;
    stats.max = results[0].time_per_iteration_us;
    stats.total = 0;
    
    for (const auto& result : results) {
        stats.total += result.time_per_iteration_us;
        stats.min = min(stats.min, result.time_per_iteration_us);
        stats.max = max(stats.max, result.time_per_iteration_us);
    }
    
    stats.mean = stats.total / results.size();
    
    return stats;
}

// ============================================================================
// Output Formatting
// ============================================================================

void printHeader() {
    cout << "\n";
    cout << "========================================\n";
    cout << " Arabic Optimizer Benchmarks\n";
    cout << " قياسات أداء محسّن اللغة العربية\n";
    cout << "========================================\n";
    cout << "\n";
}

void printResult(const BenchmarkResult& result) {
    cout << "Test: " << result.test_name << "\n";
    cout << "  Iterations:     " << result.iterations << "\n";
    cout << "  Total time:     " << fixed << setprecision(3) 
         << result.time_ms << " ms\n";
    cout << "  Per iteration:  " << fixed << setprecision(3) 
         << result.time_per_iteration_us << " μs\n";
    cout << "  Code size:      " << result.code_size << " bytes\n";
    cout << "  Throughput:     " << fixed << setprecision(0)
         << (1000000.0 / result.time_per_iteration_us) << " iterations/sec\n";
    cout << "\n";
}

void printStatistics(const Statistics& stats) {
    cout << "========================================\n";
    cout << " Overall Statistics / إحصائيات عامة\n";
    cout << "========================================\n";
    cout << "  Mean time:      " << fixed << setprecision(3) 
         << stats.mean << " μs\n";
    cout << "  Min time:       " << fixed << setprecision(3) 
         << stats.min << " μs\n";
    cout << "  Max time:       " << fixed << setprecision(3) 
         << stats.max << " μs\n";
    cout << "  Total time:     " << fixed << setprecision(3) 
         << stats.total << " μs\n";
    cout << "\n";
}

void printComparison(const vector<BenchmarkResult>& results) {
    cout << "========================================\n";
    cout << " Performance Comparison / مقارنة الأداء\n";
    cout << "========================================\n";
    cout << "\n";
    
    // Sort by time per iteration
    vector<BenchmarkResult> sorted = results;
    sort(sorted.begin(), sorted.end(), 
         [](const BenchmarkResult& a, const BenchmarkResult& b) {
             return a.time_per_iteration_us < b.time_per_iteration_us;
         });
    
    cout << left;
    cout << setw(30) << "Test Name" 
         << setw(15) << "Time (μs)" 
         << setw(15) << "Relative" << "\n";
    cout << string(60, '-') << "\n";
    
    double baseline = sorted[0].time_per_iteration_us;
    
    for (const auto& result : sorted) {
        double relative = result.time_per_iteration_us / baseline;
        cout << setw(30) << result.test_name
             << setw(15) << fixed << setprecision(3) 
             << result.time_per_iteration_us
             << setw(15) << fixed << setprecision(2) 
             << relative << "x\n";
    }
    
    cout << "\n";
}

void saveResultsToFile(const vector<BenchmarkResult>& results, 
                       const Statistics& stats) {
    ofstream file("benchmark_results.txt");
    
    if (!file.is_open()) {
        cerr << "Failed to open output file\n";
        return;
    }
    
    file << "========================================\n";
    file << "Arabic Optimizer Benchmark Results\n";
    file << "========================================\n\n";
    
    for (const auto& result : results) {
        file << "Test: " << result.test_name << "\n";
        file << "  Iterations: " << result.iterations << "\n";
        file << "  Total time: " << result.time_ms << " ms\n";
        file << "  Per iteration: " << result.time_per_iteration_us << " μs\n";
        file << "  Code size: " << result.code_size << " bytes\n\n";
    }
    
    file << "\nStatistics:\n";
    file << "  Mean: " << stats.mean << " μs\n";
    file << "  Min: " << stats.min << " μs\n";
    file << "  Max: " << stats.max << " μs\n";
    file << "  Total: " << stats.total << " μs\n";
    
    file.close();
    cout << "✓ Results saved to benchmark_results.txt\n";
}

// ============================================================================
// Main Function
// ============================================================================

int main() {
    printHeader();
    
    cout << "Running benchmarks on " << test_cases.size() 
         << " real-world test cases...\n\n";
    
    vector<BenchmarkResult> results;
    
    // Run benchmarks
    for (const auto& test : test_cases) {
        cout << "▶ Running: " << test.name << "\n";
        cout << "  Description: " << test.description << "\n";
        
        auto result = Benchmark::run(test, 1000);
        results.push_back(result);
        
        printResult(result);
    }
    
    // Calculate and print statistics
    auto stats = calculateStats(results);
    printStatistics(stats);
    
    // Print comparison
    printComparison(results);
    
    // Save to file
    saveResultsToFile(results, stats);
    
    cout << "========================================\n";
    cout << " Benchmark Complete / اكتمل القياس\n";
    cout << "========================================\n";
    cout << "\n";
    cout << "✅ All " << test_cases.size() << " benchmarks completed successfully\n";
    cout << "✅ Average time: " << fixed << setprecision(3) 
         << stats.mean << " μs per iteration\n";
    cout << "\n";
    
    return 0;
}
