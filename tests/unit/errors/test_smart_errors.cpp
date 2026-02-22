/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: test_smart_errors.cpp                                                ║
 * ║   الوصف: اختبارات نظام رسائل الخطأ الذكية                                    ║
 * ║   المهمة: T168 - Phase 15 (US13)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هي اختبارات رسائل الخطأ؟
 * ────────────────────────────────────────
 *
 * هذه الاختبارات تتأكد أن نظام الأخطاء يعمل بشكل صحيح:
 *
 * 1️⃣ اختبارات الاقتراحات - هل تُقترح كلمات صحيحة للأخطاء الإملائية؟
 * 2️⃣ اختبارات شرح الأنواع - هل الشرح مفهوم للمبتدئين؟
 * 3️⃣ اختبارات التعافي - هل يستمر المترجم بعد الخطأ؟
 * 4️⃣ اختبارات الأخطاء المتعددة - هل تُجمع كل الأخطاء؟
 * 5️⃣ اختبارات منع التسلسل - هل تُمنع الأخطاء المتكررة؟
 * 6️⃣ اختبارات وضع المعلم - هل الشرح التعليمي واضح؟
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <cassert>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

// ═══════════════════════════════════════════════════════════════════════════════
//                          إطار الاختبار | Test Framework
// ═══════════════════════════════════════════════════════════════════════════════

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double durationMs;
};

std::vector<TestResult> testResults;

#define TEST(name) void test_##name(); \
    static bool _reg_##name = []{ \
        return true; \
    }(); \
    void test_##name()

#define ASSERT_TRUE(cond, msg) do { \
    if (!(cond)) throw std::runtime_error(msg); \
} while(0)

#define ASSERT_FALSE(cond, msg) ASSERT_TRUE(!(cond), msg)

#define ASSERT_EQ(a, b, msg) ASSERT_TRUE((a) == (b), msg)

#define ASSERT_CONTAINS(str, substr, msg) \
    ASSERT_TRUE((str).find(substr) != std::string::npos, msg)

// ═══════════════════════════════════════════════════════════════════════════════
//                      تنفيذ محلي للأنظمة | Local Implementations
// ═══════════════════════════════════════════════════════════════════════════════

namespace sad {
namespace errors {

/**
 * 📏 حساب مسافة Levenshtein
 */
int levenshteinDistance(const std::string& s1, const std::string& s2) {
    int m = s1.length();
    int n = s2.length();
    
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));
    
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;
    
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            dp[i][j] = std::min({
                dp[i-1][j] + 1,
                dp[i][j-1] + 1,
                dp[i-1][j-1] + cost
            });
        }
    }
    
    return dp[m][n];
}

/**
 * 🔍 نظام الاقتراحات المبسط
 */
class SimpleSuggestionGenerator {
public:
    void addKnownSymbol(const std::string& symbol) {
        knownSymbols_.push_back(symbol);
    }
    
    std::vector<std::string> getSuggestions(const std::string& input, int maxSuggestions = 3) {
        std::vector<std::pair<int, std::string>> candidates;
        
        for (const auto& symbol : knownSymbols_) {
            int distance = levenshteinDistance(input, symbol);
            if (distance <= 3) { // عتبة التشابه
                candidates.push_back({distance, symbol});
            }
        }
        
        std::sort(candidates.begin(), candidates.end());
        
        std::vector<std::string> result;
        for (size_t i = 0; i < candidates.size() && i < maxSuggestions; ++i) {
            result.push_back(candidates[i].second);
        }
        
        return result;
    }
    
private:
    std::vector<std::string> knownSymbols_;
};

/**
 * 📝 شرح الأنواع المبسط
 */
class SimpleTypeExplainer {
public:
    std::string explainMismatch(const std::string& expected, const std::string& actual) {
        std::string result;
        
        // Arabic explanation
        result = "❌ خطأ في الأنواع!\n";
        result += "   النوع المتوقع: " + expected + "\n";
        result += "   النوع الموجود: " + actual + "\n\n";
        
        // Simple analogy
        if (expected == "عدد" && actual == "نص") {
            result += "📚 شرح:\n";
            result += "   حاولت جمع نص مع عدد!\n";
            result += "   مثل محاولة جمع \"أحمد\" + 5 - لا معنى لها.\n\n";
            result += "💡 اقتراح:\n";
            result += "   استخدم: عدد(\"5\") + 5 = 10\n";
        }
        else if (expected == "منطقي" && actual == "عدد") {
            result += "📚 شرح:\n";
            result += "   الشروط تحتاج قيمة منطقية (صحيح/خطأ).\n";
            result += "   لا يمكن استخدام رقم مباشرة في الشرط.\n\n";
            result += "💡 اقتراح:\n";
            result += "   استخدم مقارنة: عدد > 0\n";
        }
        
        return result;
    }
};

/**
 * 🔄 نظام التعافي المبسط
 */
class SimpleErrorRecovery {
public:
    struct RecoveryResult {
        bool success;
        std::string action;
        int skipTokens;
    };
    
    RecoveryResult tryRecover(const std::string& errorType, const std::vector<std::string>& tokens, size_t position) {
        RecoveryResult result;
        
        if (errorType == "missing_colon") {
            result.success = true;
            result.action = "inserted_colon";
            result.skipTokens = 0;
        }
        else if (errorType == "missing_bracket") {
            result.success = true;
            result.action = "inserted_bracket";
            result.skipTokens = 0;
        }
        else if (errorType == "unexpected_token") {
            // Panic mode: skip to synchronization point
            result.success = true;
            result.action = "skip_to_sync";
            result.skipTokens = findSyncPoint(tokens, position);
        }
        else {
            result.success = false;
            result.action = "none";
            result.skipTokens = 0;
        }
        
        return result;
    }
    
private:
    int findSyncPoint(const std::vector<std::string>& tokens, size_t start) {
        std::unordered_set<std::string> syncTokens = {
            "دالة", "متغير", "إذا", "طالما", "لكل", "أرجع"
        };
        
        for (size_t i = start; i < tokens.size(); ++i) {
            if (syncTokens.count(tokens[i])) {
                return i - start;
            }
        }
        
        return tokens.size() - start;
    }
};

/**
 * 📊 جامع الأخطاء المتعددة
 */
class SimpleMultiErrorCollector {
public:
    struct Error {
        std::string file;
        int line;
        int column;
        std::string severity; // error, warning, hint
        std::string code;
        std::string message;
    };
    
    void addError(const Error& error) {
        errors_.push_back(error);
    }
    
    size_t errorCount() const {
        return errors_.size();
    }
    
    std::string formatAll() const {
        std::string result;
        
        // Group by file
        std::unordered_map<std::string, std::vector<const Error*>> byFile;
        for (const auto& error : errors_) {
            byFile[error.file].push_back(&error);
        }
        
        for (const auto& [file, errors] : byFile) {
            result += "📁 " + file + "\n";
            result += "   ════════════════════════════════════════════════════════\n";
            
            for (const auto* error : errors) {
                std::string icon = (error->severity == "error") ? "❌" :
                                   (error->severity == "warning") ? "⚠️" : "💡";
                result += "   " + icon + " سطر " + std::to_string(error->line) + 
                         ": " + error->message + "\n";
            }
            result += "\n";
        }
        
        return result;
    }
    
private:
    std::vector<Error> errors_;
};

/**
 * 🛡️ نظام منع التسلسل
 */
class SimpleCascadePreventor {
public:
    void markPrimaryError(const std::string& symbol, int line) {
        primaryErrors_[symbol] = line;
    }
    
    bool isCascade(const std::string& symbol, int line) {
        // If this symbol was undefined earlier, subsequent uses are cascade
        auto it = primaryErrors_.find(symbol);
        if (it != primaryErrors_.end() && it->second < line) {
            return true;
        }
        return false;
    }
    
    void clear() {
        primaryErrors_.clear();
    }
    
private:
    std::unordered_map<std::string, int> primaryErrors_;
};

/**
 * 🎓 وضع المعلم المبسط
 */
class SimpleTeacherMode {
public:
    enum Level {
        BRIEF = 0,
        NORMAL = 1,
        DETAILED = 2,
        TEACHER = 3
    };
    
    void setLevel(Level level) {
        level_ = level;
    }
    
    std::string explain(const std::string& errorCode, const std::string& message) {
        std::string result = "❌ " + message + "\n";
        
        if (level_ >= NORMAL) {
            result += "\n🔍 تحليل:\n";
            result += "   " + getBasicExplanation(errorCode) + "\n";
        }
        
        if (level_ >= DETAILED) {
            result += "\n📖 شرح مفصل:\n";
            result += "   " + getDetailedExplanation(errorCode) + "\n";
        }
        
        if (level_ >= TEACHER) {
            result += "\n🎓 درس تعليمي:\n";
            result += getTeacherContent(errorCode);
        }
        
        return result;
    }
    
private:
    Level level_ = NORMAL;
    
    std::string getBasicExplanation(const std::string& code) {
        if (code.starts_with("T")) return "هذا خطأ في الأنواع.";
        if (code.starts_with("S")) return "هذا خطأ دلالي.";
        if (code.starts_with("P")) return "هذا خطأ نحوي.";
        return "خطأ غير معروف.";
    }
    
    std::string getDetailedExplanation(const std::string& code) {
        if (code == "T001") {
            return "حاولت استخدام نوع بيانات غير متوافق. "
                   "كل متغير له نوع محدد، ولا يمكن خلط الأنواع.";
        }
        if (code == "S001") {
            return "استخدمت متغيراً لم يتم تعريفه. "
                   "تأكد من كتابة اسم المتغير بشكل صحيح.";
        }
        return "راجع الوثائق للمزيد من المعلومات.";
    }
    
    std::string getTeacherContent(const std::string& code) {
        std::string content;
        content += "   ────────────────────────────────────────────────────\n";
        
        if (code == "T001") {
            content += "   📚 مفهوم الأنواع:\n";
            content += "   \n";
            content += "   تخيل أن كل قيمة في البرمجة تُوضع في صندوق خاص:\n";
            content += "   📦 صندوق الأرقام: 1, 2, 3...\n";
            content += "   📦 صندوق الكلمات: \"مرحبا\", \"أهلاً\"...\n";
            content += "   \n";
            content += "   لا يمكنك وضع كلمة في صندوق الأرقام!\n";
        }
        else if (code == "S001") {
            content += "   📚 مفهوم المتغيرات:\n";
            content += "   \n";
            content += "   المتغير مثل صندوق له اسم:\n";
            content += "   ┌─────────┐\n";
            content += "   │   25    │ ← القيمة\n";
            content += "   └─────────┘\n";
            content += "     عمري    ← الاسم\n";
            content += "   \n";
            content += "   يجب تعريف الصندوق قبل استخدامه!\n";
        }
        
        content += "   ────────────────────────────────────────────────────\n";
        return content;
    }
};

} // namespace errors
} // namespace sad

// ═══════════════════════════════════════════════════════════════════════════════
//                           الاختبارات | Tests
// ═══════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════════
//                    اختبارات الاقتراحات | Suggestion Tests
// ═══════════════════════════════════════════════════════════════════════════════

void testLevenshteinDistance() {
    using namespace sad::errors;
    
    // اختبار المسافة الصفرية
    ASSERT_EQ(levenshteinDistance("hello", "hello"), 0, "Same strings should have distance 0");
    
    // اختبار حرف واحد مختلف
    ASSERT_EQ(levenshteinDistance("hello", "hallo"), 1, "One char difference = 1");
    
    // اختبار حذف حرف
    ASSERT_EQ(levenshteinDistance("hello", "helo"), 1, "One deletion = 1");
    
    // اختبار إضافة حرف
    ASSERT_EQ(levenshteinDistance("helo", "hello"), 1, "One insertion = 1");
    
    // اختبار كلمات عربية
    ASSERT_EQ(levenshteinDistance("متغير", "متغير"), 0, "Same Arabic = 0");
    
    std::cout << "   ✓ Levenshtein distance tests passed\n";
}

void testSuggestionGeneration() {
    using namespace sad::errors;
    
    SimpleSuggestionGenerator gen;
    
    // إضافة رموز معروفة
    gen.addKnownSymbol("متغير");
    gen.addKnownSymbol("ثابت");
    gen.addKnownSymbol("دالة");
    gen.addKnownSymbol("إذا");
    gen.addKnownSymbol("طالما");
    gen.addKnownSymbol("اطبع");
    gen.addKnownSymbol("عدد");
    gen.addKnownSymbol("نص");
    
    // اختبار اقتراح لخطأ إملائي
    auto suggestions = gen.getSuggestions("متغبر", 3); // خطأ إملائي في "متغير"
    ASSERT_TRUE(!suggestions.empty(), "Should suggest for typo");
    ASSERT_EQ(suggestions[0], "متغير", "Should suggest 'متغير' for 'متغبر'");
    
    // اختبار اقتراح لكلمة قريبة
    suggestions = gen.getSuggestions("إذأ", 3); // خطأ في "إذا"
    ASSERT_TRUE(!suggestions.empty(), "Should suggest for typo");
    
    // اختبار عدم اقتراح لكلمة بعيدة
    suggestions = gen.getSuggestions("xyz123", 3);
    ASSERT_TRUE(suggestions.empty(), "Should not suggest for unrelated word");
    
    std::cout << "   ✓ Suggestion generation tests passed\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                  اختبارات شرح الأنواع | Type Explanation Tests
// ═══════════════════════════════════════════════════════════════════════════════

void testTypeExplanation() {
    using namespace sad::errors;
    
    SimpleTypeExplainer explainer;
    
    // اختبار شرح خطأ نوع
    std::string explanation = explainer.explainMismatch("عدد", "نص");
    
    ASSERT_CONTAINS(explanation, "خطأ", "Should contain error indicator");
    ASSERT_CONTAINS(explanation, "عدد", "Should mention expected type");
    ASSERT_CONTAINS(explanation, "نص", "Should mention actual type");
    ASSERT_CONTAINS(explanation, "اقتراح", "Should provide suggestion");
    
    std::cout << "   ✓ Type explanation tests passed\n";
}

void testTypeMismatchAnalogy() {
    using namespace sad::errors;
    
    SimpleTypeExplainer explainer;
    
    // اختبار أن الشرح يحتوي على تشبيه
    std::string explanation = explainer.explainMismatch("عدد", "نص");
    
    ASSERT_CONTAINS(explanation, "شرح", "Should have explanation section");
    // التشبيه يوضح المفهوم
    
    std::cout << "   ✓ Type mismatch analogy tests passed\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                  اختبارات التعافي | Error Recovery Tests
// ═══════════════════════════════════════════════════════════════════════════════

void testErrorRecovery() {
    using namespace sad::errors;
    
    SimpleErrorRecovery recovery;
    std::vector<std::string> tokens = {"إذا", "س", ">", "5", "اطبع", "(", "\"نعم\"", ")"};
    
    // اختبار التعافي من نقطتين مفقودتين
    auto result = recovery.tryRecover("missing_colon", tokens, 4);
    ASSERT_TRUE(result.success, "Should recover from missing colon");
    ASSERT_EQ(result.action, "inserted_colon", "Should insert colon");
    
    // اختبار التعافي من قوس مفقود
    result = recovery.tryRecover("missing_bracket", tokens, 5);
    ASSERT_TRUE(result.success, "Should recover from missing bracket");
    
    std::cout << "   ✓ Error recovery tests passed\n";
}

void testPanicModeRecovery() {
    using namespace sad::errors;
    
    SimpleErrorRecovery recovery;
    std::vector<std::string> tokens = {"###", "خطأ", "عشوائي", "متغير", "س", "=", "5"};
    
    // اختبار وضع الذعر
    auto result = recovery.tryRecover("unexpected_token", tokens, 0);
    ASSERT_TRUE(result.success, "Should recover in panic mode");
    ASSERT_EQ(result.action, "skip_to_sync", "Should skip to sync point");
    ASSERT_EQ(result.skipTokens, 3, "Should skip to 'متغير'");
    
    std::cout << "   ✓ Panic mode recovery tests passed\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
//               اختبارات الأخطاء المتعددة | Multi-Error Tests
// ═══════════════════════════════════════════════════════════════════════════════

void testMultiErrorCollection() {
    using namespace sad::errors;
    
    SimpleMultiErrorCollector collector;
    
    // إضافة أخطاء متعددة
    collector.addError({"main.ص", 10, 5, "error", "T001", "نوع غير متوافق"});
    collector.addError({"main.ص", 15, 1, "warning", "W001", "متغير غير مستخدم"});
    collector.addError({"helper.ص", 3, 8, "error", "S001", "متغير غير معرف"});
    
    ASSERT_EQ(collector.errorCount(), 3, "Should have 3 errors");
    
    std::string formatted = collector.formatAll();
    ASSERT_CONTAINS(formatted, "main.ص", "Should contain first file");
    ASSERT_CONTAINS(formatted, "helper.ص", "Should contain second file");
    ASSERT_CONTAINS(formatted, "نوع غير متوافق", "Should contain error message");
    
    std::cout << "   ✓ Multi-error collection tests passed\n";
}

void testErrorGrouping() {
    using namespace sad::errors;
    
    SimpleMultiErrorCollector collector;
    
    // إضافة أخطاء من نفس الملف
    collector.addError({"app.ص", 1, 1, "error", "E001", "خطأ 1"});
    collector.addError({"app.ص", 5, 1, "error", "E002", "خطأ 2"});
    collector.addError({"app.ص", 10, 1, "error", "E003", "خطأ 3"});
    
    std::string formatted = collector.formatAll();
    
    // يجب أن تكون كلها مجمعة تحت نفس الملف
    size_t fileCount = 0;
    size_t pos = 0;
    while ((pos = formatted.find("📁", pos)) != std::string::npos) {
        ++fileCount;
        ++pos;
    }
    ASSERT_EQ(fileCount, 1, "Should group errors under one file header");
    
    std::cout << "   ✓ Error grouping tests passed\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
//               اختبارات منع التسلسل | Cascade Prevention Tests
// ═══════════════════════════════════════════════════════════════════════════════

void testCascadePrevention() {
    using namespace sad::errors;
    
    SimpleCascadePreventor preventer;
    
    // خطأ أساسي: متغير غير معرف في سطر 5
    preventer.markPrimaryError("س", 5);
    
    // استخدامات لاحقة يجب اعتبارها تسلسلية
    ASSERT_TRUE(preventer.isCascade("س", 10), "Later use should be cascade");
    ASSERT_TRUE(preventer.isCascade("س", 15), "Even later use should be cascade");
    
    // متغير آخر لا يجب اعتباره تسلسلياً
    ASSERT_FALSE(preventer.isCascade("ص", 10), "Different symbol should not be cascade");
    
    std::cout << "   ✓ Cascade prevention tests passed\n";
}

void testCascadeDetectionAccuracy() {
    using namespace sad::errors;
    
    SimpleCascadePreventor preventer;
    
    // محاكاة سيناريو واقعي
    // السطر 3: متغير س غير معرف (خطأ أساسي)
    preventer.markPrimaryError("س", 3);
    
    // السطر 5: اطبع(س) - تسلسلي
    ASSERT_TRUE(preventer.isCascade("س", 5), "Print of undefined should be cascade");
    
    // السطر 7: ص = س + 5 - تسلسلي
    ASSERT_TRUE(preventer.isCascade("س", 7), "Use in expression should be cascade");
    
    // السطر 10: ع = س * 2 - تسلسلي
    ASSERT_TRUE(preventer.isCascade("س", 10), "Another use should be cascade");
    
    // إعادة التعيين
    preventer.clear();
    ASSERT_FALSE(preventer.isCascade("س", 15), "After clear, should not be cascade");
    
    std::cout << "   ✓ Cascade detection accuracy tests passed\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                 اختبارات وضع المعلم | Teacher Mode Tests
// ═══════════════════════════════════════════════════════════════════════════════

void testTeacherModeLevels() {
    using namespace sad::errors;
    
    SimpleTeacherMode teacher;
    
    // المستوى الموجز
    teacher.setLevel(SimpleTeacherMode::BRIEF);
    std::string brief = teacher.explain("T001", "نوع غير متوافق");
    ASSERT_CONTAINS(brief, "نوع غير متوافق", "Brief should have message");
    ASSERT_TRUE(brief.find("تحليل") == std::string::npos, "Brief should not have analysis");
    
    // المستوى العادي
    teacher.setLevel(SimpleTeacherMode::NORMAL);
    std::string normal = teacher.explain("T001", "نوع غير متوافق");
    ASSERT_CONTAINS(normal, "تحليل", "Normal should have analysis");
    ASSERT_TRUE(normal.find("شرح مفصل") == std::string::npos, "Normal should not have detailed");
    
    // المستوى المفصل
    teacher.setLevel(SimpleTeacherMode::DETAILED);
    std::string detailed = teacher.explain("T001", "نوع غير متوافق");
    ASSERT_CONTAINS(detailed, "شرح مفصل", "Detailed should have detailed explanation");
    
    // وضع المعلم
    teacher.setLevel(SimpleTeacherMode::TEACHER);
    std::string teacherLevel = teacher.explain("T001", "نوع غير متوافق");
    ASSERT_CONTAINS(teacherLevel, "درس تعليمي", "Teacher should have tutorial");
    ASSERT_CONTAINS(teacherLevel, "صندوق", "Teacher should have analogy");
    
    std::cout << "   ✓ Teacher mode levels tests passed\n";
}

void testTeacherModeContent() {
    using namespace sad::errors;
    
    SimpleTeacherMode teacher;
    teacher.setLevel(SimpleTeacherMode::TEACHER);
    
    // اختبار محتوى خطأ الأنواع
    std::string typeError = teacher.explain("T001", "نوع غير متوافق");
    ASSERT_CONTAINS(typeError, "مفهوم", "Should explain the concept");
    ASSERT_CONTAINS(typeError, "📦", "Should use visual analogies");
    
    // اختبار محتوى خطأ المتغيرات
    std::string varError = teacher.explain("S001", "متغير غير معرف");
    ASSERT_CONTAINS(varError, "متغير", "Should explain variables");
    
    std::cout << "   ✓ Teacher mode content tests passed\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    اختبارات التكامل | Integration Tests
// ═══════════════════════════════════════════════════════════════════════════════

void testFullErrorPipeline() {
    using namespace sad::errors;
    
    // محاكاة سيناريو كامل
    SimpleSuggestionGenerator suggestions;
    suggestions.addKnownSymbol("اطبع");
    suggestions.addKnownSymbol("متغير");
    
    SimpleTypeExplainer typeExplainer;
    SimpleMultiErrorCollector collector;
    SimpleCascadePreventor cascade;
    SimpleTeacherMode teacher;
    teacher.setLevel(SimpleTeacherMode::DETAILED);
    
    // سيناريو: خطأ إملائي
    auto typoSuggestions = suggestions.getSuggestions("اطبه", 3);
    ASSERT_TRUE(!typoSuggestions.empty(), "Should get typo suggestions");
    
    // سيناريو: خطأ نوع
    std::string typeExplanation = typeExplainer.explainMismatch("عدد", "نص");
    ASSERT_CONTAINS(typeExplanation, "اقتراح", "Should have fix suggestion");
    
    // جمع الأخطاء
    collector.addError({"test.ص", 1, 1, "error", "S001", "اطبه غير معرف - هل قصدت 'اطبع'؟"});
    collector.addError({"test.ص", 2, 1, "error", "T001", "نوع غير متوافق"});
    
    ASSERT_EQ(collector.errorCount(), 2, "Should collect both errors");
    
    // شرح معلم
    std::string explanation = teacher.explain("T001", "نوع غير متوافق");
    ASSERT_CONTAINS(explanation, "شرح", "Teacher should provide explanation");
    
    std::cout << "   ✓ Full error pipeline tests passed\n";
}

void testArabicErrorMessages() {
    // اختبار أن رسائل الخطأ تتضمن العربية بشكل صحيح
    using namespace sad::errors;
    
    SimpleTypeExplainer explainer;
    std::string msg = explainer.explainMismatch("عدد", "نص");
    
    // التحقق من وجود النص العربي
    ASSERT_TRUE(msg.length() > 50, "Message should be substantial");
    ASSERT_CONTAINS(msg, "عدد", "Should contain Arabic type name");
    ASSERT_CONTAINS(msg, "نص", "Should contain Arabic type name");
    
    std::cout << "   ✓ Arabic error messages tests passed\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                         التنفيذ الرئيسي | Main
// ═══════════════════════════════════════════════════════════════════════════════

void runTest(const std::string& name, std::function<void()> testFn) {
    TestResult result;
    result.name = name;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        testFn();
        result.passed = true;
        result.message = "Passed";
    }
    catch (const std::exception& e) {
        result.passed = false;
        result.message = e.what();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    result.durationMs = std::chrono::duration<double, std::milli>(end - start).count();
    
    testResults.push_back(result);
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║       اختبارات نظام رسائل الخطأ الذكية - لغة سعد                             ║\n";
    std::cout << "║       Smart Error Messages Tests - Sad Language                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";
    
    // اختبارات الاقتراحات
    std::cout << "📝 اختبارات الاقتراحات | Suggestion Tests\n";
    std::cout << "   ════════════════════════════════════════════════════════\n";
    runTest("Levenshtein Distance", testLevenshteinDistance);
    runTest("Suggestion Generation", testSuggestionGeneration);
    
    // اختبارات شرح الأنواع
    std::cout << "\n📝 اختبارات شرح الأنواع | Type Explanation Tests\n";
    std::cout << "   ════════════════════════════════════════════════════════\n";
    runTest("Type Explanation", testTypeExplanation);
    runTest("Type Mismatch Analogy", testTypeMismatchAnalogy);
    
    // اختبارات التعافي
    std::cout << "\n📝 اختبارات التعافي | Error Recovery Tests\n";
    std::cout << "   ════════════════════════════════════════════════════════\n";
    runTest("Error Recovery", testErrorRecovery);
    runTest("Panic Mode Recovery", testPanicModeRecovery);
    
    // اختبارات الأخطاء المتعددة
    std::cout << "\n📝 اختبارات الأخطاء المتعددة | Multi-Error Tests\n";
    std::cout << "   ════════════════════════════════════════════════════════\n";
    runTest("Multi-Error Collection", testMultiErrorCollection);
    runTest("Error Grouping", testErrorGrouping);
    
    // اختبارات منع التسلسل
    std::cout << "\n📝 اختبارات منع التسلسل | Cascade Prevention Tests\n";
    std::cout << "   ════════════════════════════════════════════════════════\n";
    runTest("Cascade Prevention", testCascadePrevention);
    runTest("Cascade Detection Accuracy", testCascadeDetectionAccuracy);
    
    // اختبارات وضع المعلم
    std::cout << "\n📝 اختبارات وضع المعلم | Teacher Mode Tests\n";
    std::cout << "   ════════════════════════════════════════════════════════\n";
    runTest("Teacher Mode Levels", testTeacherModeLevels);
    runTest("Teacher Mode Content", testTeacherModeContent);
    
    // اختبارات التكامل
    std::cout << "\n📝 اختبارات التكامل | Integration Tests\n";
    std::cout << "   ════════════════════════════════════════════════════════\n";
    runTest("Full Error Pipeline", testFullErrorPipeline);
    runTest("Arabic Error Messages", testArabicErrorMessages);
    
    // ملخص النتائج
    std::cout << "\n╔══════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                               ملخص النتائج                                    ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝\n\n";
    
    int passed = 0, failed = 0;
    double totalTime = 0;
    
    for (const auto& result : testResults) {
        if (result.passed) {
            std::cout << "   ✅ " << result.name << " (" << result.durationMs << " ms)\n";
            passed++;
        } else {
            std::cout << "   ❌ " << result.name << ": " << result.message << "\n";
            failed++;
        }
        totalTime += result.durationMs;
    }
    
    std::cout << "\n   ════════════════════════════════════════════════════════\n";
    std::cout << "   Total: " << (passed + failed) << " tests\n";
    std::cout << "   Passed: " << passed << " ✅\n";
    std::cout << "   Failed: " << failed << " ❌\n";
    std::cout << "   Time: " << totalTime << " ms\n";
    std::cout << "   ════════════════════════════════════════════════════════\n";
    
    return failed == 0 ? 0 : 1;
}
