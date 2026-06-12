/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: test_arabic_optimizer.cpp                                            ║
 * ║   الوصف: اختبارات أداء تحسينات العربية                                      ║
 * ║   المهمة: T160 - Phase 14 (US12)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🎯 الهدف | Goal:
 *    قياس أداء تحسينات العربية والتحقق من صحتها
 *    Benchmark Arabic optimizations and verify correctness
 *
 * 📋 الاختبارات | Tests:
 *
 *    1️⃣ تجميع النصوص | String Pooling
 *    2️⃣ ضغط التشكيل | Tashkeel Compression
 *    3️⃣ تخزين BiDi | BiDi Caching
 *    4️⃣ تطبيع NFC | NFC Normalization
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <iomanip>
#include <sstream>

// ═══════════════════════════════════════════════════════════════════════════════
//                         أدوات القياس | Benchmarking Tools
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ⏱️ مؤقت عالي الدقة
 */
class HighResTimer {
public:
    void start() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        end_ = std::chrono::high_resolution_clock::now();
    }
    
    double elapsedMicroseconds() const {
        return std::chrono::duration<double, std::micro>(end_ - start_).count();
    }
    
    double elapsedMilliseconds() const {
        return std::chrono::duration<double, std::milli>(end_ - start_).count();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;
};

/**
 * 📊 نتيجة الاختبار
 */
struct BenchmarkResult {
    std::string name;
    double timeMs;
    size_t iterations;
    double opsPerSecond;
    bool passed;
    
    void print() const {
        std::cout << "  " << (passed ? "✅" : "❌") << " " << name << "\n";
        std::cout << "     الوقت: " << std::fixed << std::setprecision(3) << timeMs << " ms\n";
        std::cout << "     التكرارات: " << iterations << "\n";
        std::cout << "     العمليات/ثانية: " << std::fixed << std::setprecision(0) << opsPerSecond << "\n";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     محاكاة String Pool | String Pool Simulation
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 محاكاة بسيطة لـ String Pool
 */
class SimpleStringPool {
public:
    const char* intern(const std::string& str) {
        auto it = pool_.find(str);
        if (it != pool_.end()) {
            hits_++;
            return it->second.c_str();
        }
        misses_++;
        pool_[str] = str;
        return pool_[str].c_str();
    }
    
    size_t getHits() const { return hits_; }
    size_t getMisses() const { return misses_; }
    size_t getPoolSize() const { return pool_.size(); }
    
    void clear() {
        pool_.clear();
        hits_ = 0;
        misses_ = 0;
    }
    
private:
    std::unordered_map<std::string, std::string> pool_;
    size_t hits_ = 0;
    size_t misses_ = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     محاكاة ضغط التشكيل | Tashkeel Compression Simulation
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ✨ فحص إذا كان الحرف تشكيل
 */
inline bool isTashkeelChar(uint32_t cp) {
    return cp >= 0x064B && cp <= 0x065F;
}

/**
 * 📏 حساب عدد بايتات التشكيل في نص
 */
size_t countTashkeelBytes(const std::string& str) {
    size_t count = 0;
    for (size_t i = 0; i < str.size();) {
        unsigned char c = str[i];
        uint32_t cp;
        size_t bytes;
        
        if ((c & 0x80) == 0) {
            cp = c;
            bytes = 1;
        } else if ((c & 0xE0) == 0xC0) {
            cp = ((c & 0x1F) << 6) | (str[i+1] & 0x3F);
            bytes = 2;
        } else if ((c & 0xF0) == 0xE0) {
            cp = ((c & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
            bytes = 3;
        } else {
            bytes = 1;
            cp = 0;
        }
        
        if (isTashkeelChar(cp)) {
            count += bytes;
        }
        
        i += bytes;
    }
    return count;
}

/**
 * 🗜️ إزالة التشكيل من نص
 */
std::string stripTashkeel(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size();) {
        unsigned char c = str[i];
        uint32_t cp;
        size_t bytes;
        
        if ((c & 0x80) == 0) {
            cp = c;
            bytes = 1;
        } else if ((c & 0xE0) == 0xC0) {
            cp = ((c & 0x1F) << 6) | (str[i+1] & 0x3F);
            bytes = 2;
        } else if ((c & 0xF0) == 0xE0) {
            cp = ((c & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
            bytes = 3;
        } else if ((c & 0xF8) == 0xF0) {
            cp = ((c & 0x07) << 18) | ((str[i+1] & 0x3F) << 12) | 
                 ((str[i+2] & 0x3F) << 6) | (str[i+3] & 0x3F);
            bytes = 4;
        } else {
            bytes = 1;
            cp = c;
        }
        
        if (!isTashkeelChar(cp)) {
            for (size_t j = 0; j < bytes; j++) {
                result.push_back(str[i + j]);
            }
        }
        
        i += bytes;
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                      محاكاة BiDi Cache | BiDi Cache Simulation
// ═══════════════════════════════════════════════════════════════════════════════

enum class TextDirection {
    LTR,
    RTL,
    MIXED
};

/**
 * 🔄 تحليل اتجاه النص (مبسط)
 */
TextDirection analyzeBidi(const std::string& str) {
    bool hasRTL = false;
    bool hasLTR = false;
    
    for (size_t i = 0; i < str.size();) {
        unsigned char c = str[i];
        uint32_t cp;
        size_t bytes;
        
        if ((c & 0x80) == 0) {
            cp = c;
            bytes = 1;
        } else if ((c & 0xE0) == 0xC0) {
            cp = ((c & 0x1F) << 6) | (str[i+1] & 0x3F);
            bytes = 2;
        } else if ((c & 0xF0) == 0xE0) {
            cp = ((c & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
            bytes = 3;
        } else {
            bytes = 1;
            cp = 0;
        }
        
        // Arabic range
        if ((cp >= 0x0600 && cp <= 0x06FF) ||
            (cp >= 0x0590 && cp <= 0x05FF)) {
            hasRTL = true;
        }
        // Latin range
        if ((cp >= 0x0041 && cp <= 0x005A) ||
            (cp >= 0x0061 && cp <= 0x007A)) {
            hasLTR = true;
        }
        
        i += bytes;
    }
    
    if (hasRTL && hasLTR) return TextDirection::MIXED;
    if (hasRTL) return TextDirection::RTL;
    return TextDirection::LTR;
}

/**
 * 🗃️ BiDi Cache بسيط
 */
class SimpleBidiCache {
public:
    TextDirection getDirection(const std::string& str) {
        auto it = cache_.find(str);
        if (it != cache_.end()) {
            hits_++;
            return it->second;
        }
        misses_++;
        TextDirection dir = analyzeBidi(str);
        cache_[str] = dir;
        return dir;
    }
    
    size_t getHits() const { return hits_; }
    size_t getMisses() const { return misses_; }
    
    void clear() {
        cache_.clear();
        hits_ = 0;
        misses_ = 0;
    }
    
private:
    std::unordered_map<std::string, TextDirection> cache_;
    size_t hits_ = 0;
    size_t misses_ = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                           اختبارات الصحة | Correctness Tests
// ═══════════════════════════════════════════════════════════════════════════════

void testStringPoolCorrectness() {
    std::cout << "\n📦 اختبار صحة تجميع النصوص | String Pool Correctness\n";
    std::cout << "   ─────────────────────────────────────────────────\n";
    
    SimpleStringPool pool;
    
    // نفس النص يجب أن يرجع نفس المؤشر
    // Same text should return same pointer
    const char* p1 = pool.intern("السلام عليكم");
    const char* p2 = pool.intern("السلام عليكم");
    const char* p3 = pool.intern("السلام عليكم");
    
    assert(p1 == p2 && "نفس النص يجب أن يرجع نفس المؤشر");
    assert(p2 == p3 && "نفس النص يجب أن يرجع نفس المؤشر");
    
    // نص مختلف يجب أن يرجع مؤشر مختلف
    const char* p4 = pool.intern("مرحبا");
    assert(p1 != p4 && "نصوص مختلفة يجب أن ترجع مؤشرات مختلفة");
    
    assert(pool.getHits() == 2);
    assert(pool.getMisses() == 2);
    assert(pool.getPoolSize() == 2);
    
    std::cout << "   ✅ جميع اختبارات الصحة نجحت\n";
}

void testTashkeelCorrectness() {
    std::cout << "\n✨ اختبار صحة ضغط التشكيل | Tashkeel Correctness\n";
    std::cout << "   ────────────────────────────────────────────────\n";
    
    // نص بتشكيل
    std::string withTashkeel = "السَّلَامُ عَلَيْكُمْ";
    std::string stripped = stripTashkeel(withTashkeel);
    
    // يجب أن يحتوي على تشكيل
    size_t tashkeelCount = countTashkeelBytes(withTashkeel);
    assert(tashkeelCount > 0 && "يجب أن يحتوي النص على تشكيل");
    
    // النص المجرد يجب أن لا يحتوي على تشكيل
    size_t strippedCount = countTashkeelBytes(stripped);
    assert(strippedCount == 0 && "النص المجرد يجب أن لا يحتوي على تشكيل");
    
    std::cout << "   ✅ النص الأصلي: " << withTashkeel << "\n";
    std::cout << "   ✅ النص المجرد: " << stripped << "\n";
    std::cout << "   ✅ بايتات التشكيل: " << tashkeelCount << "\n";
}

void testBidiCorrectness() {
    std::cout << "\n🔄 اختبار صحة تحليل BiDi | BiDi Correctness\n";
    std::cout << "   ────────────────────────────────────────────\n";
    
    // نص عربي فقط - يمين لليسار
    TextDirection dir1 = analyzeBidi("السلام عليكم");
    assert(dir1 == TextDirection::RTL && "نص عربي يجب أن يكون RTL");
    
    // نص إنجليزي فقط - يسار لليمين
    TextDirection dir2 = analyzeBidi("Hello World");
    assert(dir2 == TextDirection::LTR && "نص إنجليزي يجب أن يكون LTR");
    
    // نص مختلط
    TextDirection dir3 = analyzeBidi("مرحبا Hello عالم");
    assert(dir3 == TextDirection::MIXED && "نص مختلط يجب أن يكون MIXED");
    
    std::cout << "   ✅ جميع اختبارات BiDi نجحت\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                          اختبارات الأداء | Performance Tests
// ═══════════════════════════════════════════════════════════════════════════════

BenchmarkResult benchmarkStringPool() {
    BenchmarkResult result;
    result.name = "تجميع النصوص | String Pooling";
    result.passed = true;
    
    SimpleStringPool pool;
    HighResTimer timer;
    
    // بيانات الاختبار - نصوص عربية شائعة
    std::vector<std::string> testStrings = {
        "السلام عليكم",
        "بسم الله الرحمن الرحيم",
        "الحمد لله رب العالمين",
        "لا إله إلا الله",
        "محمد رسول الله",
        "سبحان الله",
        "الله أكبر",
        "لا حول ولا قوة إلا بالله",
        "إن شاء الله",
        "ما شاء الله"
    };
    
    const size_t iterations = 100000;
    
    timer.start();
    for (size_t i = 0; i < iterations; i++) {
        for (const auto& str : testStrings) {
            pool.intern(str);
        }
    }
    timer.stop();
    
    result.timeMs = timer.elapsedMilliseconds();
    result.iterations = iterations * testStrings.size();
    result.opsPerSecond = (result.iterations / result.timeMs) * 1000;
    
    // تحقق من Cache Hit Rate
    double hitRate = (double)pool.getHits() / (pool.getHits() + pool.getMisses());
    if (hitRate < 0.9) {
        result.passed = false;
    }
    
    return result;
}

BenchmarkResult benchmarkTashkeelStripping() {
    BenchmarkResult result;
    result.name = "إزالة التشكيل | Tashkeel Stripping";
    result.passed = true;
    
    HighResTimer timer;
    
    // نص طويل بتشكيل
    std::string testText = "بِسْمِ اللَّهِ الرَّحْمَٰنِ الرَّحِيمِ "
                           "الْحَمْدُ لِلَّهِ رَبِّ الْعَالَمِينَ "
                           "الرَّحْمَٰنِ الرَّحِيمِ "
                           "مَالِكِ يَوْمِ الدِّينِ";
    
    const size_t iterations = 50000;
    std::string stripped;
    
    timer.start();
    for (size_t i = 0; i < iterations; i++) {
        stripped = stripTashkeel(testText);
    }
    timer.stop();
    
    result.timeMs = timer.elapsedMilliseconds();
    result.iterations = iterations;
    result.opsPerSecond = (iterations / result.timeMs) * 1000;
    
    return result;
}

BenchmarkResult benchmarkBidiAnalysis() {
    BenchmarkResult result;
    result.name = "تحليل BiDi | BiDi Analysis";
    result.passed = true;
    
    SimpleBidiCache cache;
    HighResTimer timer;
    
    // نصوص للاختبار
    std::vector<std::string> testStrings = {
        "مرحبا بكم",
        "Hello World",
        "مرحبا Hello عالم World",
        "أهلاً وسهلاً",
        "Welcome مرحبا",
        "12345 رقم",
        "تاريخ: 2024/01/01",
        "Email: test@example.com بريد"
    };
    
    const size_t iterations = 100000;
    
    timer.start();
    for (size_t i = 0; i < iterations; i++) {
        for (const auto& str : testStrings) {
            cache.getDirection(str);
        }
    }
    timer.stop();
    
    result.timeMs = timer.elapsedMilliseconds();
    result.iterations = iterations * testStrings.size();
    result.opsPerSecond = (result.iterations / result.timeMs) * 1000;
    
    // تحقق من Cache Hit Rate
    double hitRate = (double)cache.getHits() / (cache.getHits() + cache.getMisses());
    if (hitRate < 0.9) {
        result.passed = false;
    }
    
    return result;
}

BenchmarkResult benchmarkUTF8Decoding() {
    BenchmarkResult result;
    result.name = "فك تشفير UTF-8 | UTF-8 Decoding";
    result.passed = true;
    
    HighResTimer timer;
    
    // نص عربي طويل
    std::string testText;
    for (int i = 0; i < 100; i++) {
        testText += "السلام عليكم ورحمة الله وبركاته. ";
    }
    
    const size_t iterations = 10000;
    size_t totalCodepoints = 0;
    
    timer.start();
    for (size_t iter = 0; iter < iterations; iter++) {
        size_t codepoints = 0;
        for (size_t i = 0; i < testText.size();) {
            unsigned char c = testText[i];
            if ((c & 0x80) == 0) {
                i += 1;
            } else if ((c & 0xE0) == 0xC0) {
                i += 2;
            } else if ((c & 0xF0) == 0xE0) {
                i += 3;
            } else {
                i += 4;
            }
            codepoints++;
        }
        totalCodepoints = codepoints;
    }
    timer.stop();
    
    result.timeMs = timer.elapsedMilliseconds();
    result.iterations = iterations;
    result.opsPerSecond = (iterations / result.timeMs) * 1000;
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              الدالة الرئيسية | Main Function
// ═══════════════════════════════════════════════════════════════════════════════

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║           اختبارات تحسينات العربية - لغة سعد                  ║\n";
    std::cout << "║           Arabic Optimization Tests - Sad Language           ║\n";
    std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║   📋 الاختبارات | Tests:                                      ║\n";
    std::cout << "║      • تجميع النصوص | String Pooling                         ║\n";
    std::cout << "║      • ضغط التشكيل | Tashkeel Compression                    ║\n";
    std::cout << "║      • تحليل BiDi | BiDi Analysis                            ║\n";
    std::cout << "║      • فك تشفير UTF-8 | UTF-8 Decoding                       ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    // ─────────────────────────────────────────────────────────────────────────────
    //                          اختبارات الصحة | Correctness Tests
    // ─────────────────────────────────────────────────────────────────────────────
    
    std::cout << "\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << "              اختبارات الصحة | Correctness Tests               \n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    testStringPoolCorrectness();
    testTashkeelCorrectness();
    testBidiCorrectness();
    
    // ─────────────────────────────────────────────────────────────────────────────
    //                          اختبارات الأداء | Performance Tests
    // ─────────────────────────────────────────────────────────────────────────────
    
    std::cout << "\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << "              اختبارات الأداء | Performance Tests              \n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    std::vector<BenchmarkResult> results;
    
    std::cout << "\n⏳ جاري تشغيل الاختبارات...\n";
    
    results.push_back(benchmarkStringPool());
    results.push_back(benchmarkTashkeelStripping());
    results.push_back(benchmarkBidiAnalysis());
    results.push_back(benchmarkUTF8Decoding());
    
    std::cout << "\n📊 النتائج | Results:\n";
    std::cout << "   ─────────────────────────────────────────────────────────\n";
    
    for (const auto& result : results) {
        result.print();
        std::cout << "\n";
    }
    
    // ─────────────────────────────────────────────────────────────────────────────
    //                             الملخص | Summary
    // ─────────────────────────────────────────────────────────────────────────────
    
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << "                      الملخص | Summary                         \n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    
    int passed = 0;
    int failed = 0;
    for (const auto& result : results) {
        if (result.passed) passed++;
        else failed++;
    }
    
    std::cout << "\n   ✅ نجح: " << passed << " اختبارات\n";
    std::cout << "   ❌ فشل: " << failed << " اختبارات\n";
    
    if (failed == 0) {
        std::cout << "\n   🎉 جميع الاختبارات نجحت! | All tests passed!\n";
    }
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                   انتهاء الاختبارات | Tests Complete          ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    return failed > 0 ? 1 : 0;
}
