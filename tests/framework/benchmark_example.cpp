/**
 * @file benchmark_example.cpp
 * @brief (AR) مثال على استخدام إطار قياس الأداء
 * @brief (EN) Example usage of the benchmark framework
 * 
 * @author عمر — مهندس البنية التحتية
 * @date 2026-03-06
 */

#include "sad_benchmark.h"
#include <string>
#include <vector>
#include <cmath>

using namespace SadBench;

// ══════════════════════════════════════════════════════════════════════════════
// قياسات المحلل المعجمي (Lexer) — مثال
// ══════════════════════════════════════════════════════════════════════════════

// محاكاة تحليل معجمي بسيط
std::vector<std::string> fake_lex(const std::string& code) {
    std::vector<std::string> tokens;
    std::string current;
    for (char c : code) {
        if (c == ' ' || c == '\n') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) tokens.push_back(current);
    return tokens;
}

BENCHMARK(Lexer, تحليل_نص_قصير) {
    for (auto _ : state) {
        auto tokens = fake_lex(u8"دالة جمع() ارجع 5 نهاية");
        (void)tokens;
    }
}

BENCHMARK(Lexer, تحليل_نص_متوسط) {
    std::string code;
    for (int i = 0; i < 100; ++i) {
        code += u8"دالة اختبار" + std::to_string(i) + u8"() ارجع " + std::to_string(i) + u8" نهاية\n";
    }
    
    for (auto _ : state) {
        auto tokens = fake_lex(code);
        (void)tokens;
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// قياسات حسابية
// ══════════════════════════════════════════════════════════════════════════════

BENCHMARK(Math, جمع_بسيط) {
    volatile int sum = 0;
    for (auto _ : state) {
        for (int i = 0; i < 1000; ++i) {
            sum += i;
        }
    }
}

BENCHMARK(Math, حساب_الجذر_التربيعي) {
    volatile double result = 0;
    for (auto _ : state) {
        for (int i = 1; i <= 1000; ++i) {
            result += std::sqrt(static_cast<double>(i));
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// قياسات النصوص
// ══════════════════════════════════════════════════════════════════════════════

BENCHMARK(String, إنشاء_نص_قصير) {
    for (auto _ : state) {
        std::string s = u8"مرحبا بالعالم";
        (void)s;
    }
}

BENCHMARK(String, دمج_نصوص) {
    for (auto _ : state) {
        std::string s;
        for (int i = 0; i < 100; ++i) {
            s += u8"كلمة";
        }
        (void)s;
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// قياسات المصفوفات
// ══════════════════════════════════════════════════════════════════════════════

BENCHMARK(Array, إنشاء_مصفوفة_1000) {
    for (auto _ : state) {
        std::vector<int> arr(1000);
        (void)arr;
    }
}

BENCHMARK(Array, ترتيب_مصفوفة_1000) {
    std::vector<int> base(1000);
    for (int i = 0; i < 1000; ++i) base[i] = 1000 - i;
    
    for (auto _ : state) {
        auto arr = base;
        std::sort(arr.begin(), arr.end());
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// نقطة الدخول
// ══════════════════════════════════════════════════════════════════════════════
int main(int argc, char** argv) {
    std::cout << u8"\n════════════════════════════════════════════════════════════\n";
    std::cout << u8"  مثال على قياس الأداء — Benchmark Example\n";
    std::cout << u8"════════════════════════════════════════════════════════════\n";

    int result = SAD_RUN_BENCHMARKS();
    
    // تصدير النتائج لتتبع الأداء في CI
    BenchmarkRunner::instance().exportJSON("benchmark_results.json");
    
    std::cout << u8"✓ النتائج مُصدَّرة إلى benchmark_results.json\n\n";
    
    return result;
}
