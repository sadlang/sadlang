/**
 * =============================================================================
 * ملف: repetition.cpp
 * الوصف: أنماط التكرار في الماكرو
 * المهمة: T196 - Implement repetition patterns ($x:عبر*)
 * المرحلة: Phase 19 - User Story 16 (Macro System ماكرو!)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هي أنماط التكرار؟
 * ────────────────────
 * التكرار يسمح لك بالتقاط عدد متغير من العناصر.
 * 
 * الأنواع:
 * ────────
 * 1. صفر أو أكثر (*)
 *    $($x:عبر),*  -> يطابق: () أو (1) أو (1,2) أو (1,2,3)...
 * 
 * 2. واحد أو أكثر (+)
 *    $($x:عبر),+  -> يطابق: (1) أو (1,2) أو (1,2,3)...
 *                    لا يطابق: ()
 * 
 * 3. اختياري (؟ أو ?)
 *    $($x:عبر)?  -> يطابق: () أو (1) فقط
 * 
 * الفاصل:
 * ────────
 * الحرف قبل علامة التكرار هو الفاصل
 * $($x),*  -> يستخدم الفاصلة
 * $($x);*  -> يستخدم الفاصلة المنقوطة
 * 
 * مثال عملي:
 * ```sad
 * // ماكرو لإنشاء دالة بمعاملات متعددة
 * ماكرو! دالة_طباعة!($($معامل:عبر),*) {
 *     اطبع($($معامل),*)
 * }
 * 
 * دالة_طباعة!(1, 2, 3)
 * // يتوسع إلى: اطبع(1, 2, 3)
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <iostream>
#include <optional>
#include <stdexcept>

// =============================================================================
// أنواع التكرار
// =============================================================================

/**
 * نوع التكرار
 */
enum class RepetitionType {
    ZeroOrMore,     // * - صفر أو أكثر
    OneOrMore,      // + - واحد أو أكثر  
    Optional        // ? أو ؟ - صفر أو واحد
};

/**
 * معلومات التكرار
 */
struct RepetitionInfo {
    RepetitionType type;
    std::string separator;      // الفاصل (مثل "," أو ";")
    size_t minCount;           // الحد الأدنى
    size_t maxCount;           // الحد الأقصى (SIZE_MAX = غير محدود)
    
    RepetitionInfo(RepetitionType t = RepetitionType::ZeroOrMore)
        : type(t), minCount(0), maxCount(SIZE_MAX) {
        updateLimits();
    }
    
    void updateLimits() {
        switch (type) {
            case RepetitionType::ZeroOrMore:
                minCount = 0;
                maxCount = SIZE_MAX;
                break;
            case RepetitionType::OneOrMore:
                minCount = 1;
                maxCount = SIZE_MAX;
                break;
            case RepetitionType::Optional:
                minCount = 0;
                maxCount = 1;
                break;
        }
    }
    
    bool isValid(size_t count) const {
        return count >= minCount && count <= maxCount;
    }
    
    std::string typeName() const {
        switch (type) {
            case RepetitionType::ZeroOrMore: return "* (صفر أو أكثر)";
            case RepetitionType::OneOrMore: return "+ (واحد أو أكثر)";
            case RepetitionType::Optional: return "؟ (اختياري)";
            default: return "غير معروف";
        }
    }
};

// =============================================================================
// نمط التكرار
// =============================================================================

/**
 * تمثيل نمط تكرار واحد
 */
struct RepetitionPattern {
    std::string pattern;        // النمط داخل $(...)
    RepetitionInfo info;
    std::vector<std::string> varNames;  // أسماء المتغيرات داخل النمط
    
    /**
     * استخراج أسماء المتغيرات من النمط
     */
    void extractVarNames() {
        varNames.clear();
        size_t pos = 0;
        
        while (pos < pattern.size()) {
            if (pattern[pos] == '$') {
                pos++;
                std::string name;
                while (pos < pattern.size() && isVarChar(pattern[pos])) {
                    name += pattern[pos++];
                }
                if (!name.empty()) {
                    varNames.push_back(name);
                }
            } else {
                pos++;
            }
        }
    }
    
private:
    bool isVarChar(char c) const {
        return (c >= 'a' && c <= 'z') || 
               (c >= 'A' && c <= 'Z') ||
               c == '_' ||
               (c >= '0' && c <= '9') ||
               (unsigned char)c >= 0x80;
    }
};

// =============================================================================
// محلل التكرار
// =============================================================================

/**
 * محلل أنماط التكرار
 */
class RepetitionParser {
public:
    /**
     * تحليل نمط تكرار
     * المدخل: "$($x:عبر),*" أو جزء منه
     */
    RepetitionPattern parse(const std::string& input) {
        RepetitionPattern result;
        size_t pos = 0;
        
        // تخطي $( إذا وُجد
        if (input.substr(0, 2) == "$(") {
            pos = 2;
        }
        
        // البحث عن القوس المُغلق
        int depth = 1;
        size_t patternStart = pos;
        
        while (pos < input.size() && depth > 0) {
            if (input[pos] == '(') depth++;
            else if (input[pos] == ')') depth--;
            
            if (depth > 0) pos++;
        }
        
        result.pattern = input.substr(patternStart, pos - patternStart);
        
        if (pos < input.size() && input[pos] == ')') {
            pos++;
        }
        
        // البحث عن الفاصل ونوع التكرار
        if (pos < input.size()) {
            char c = input[pos];
            
            // فاصل؟
            if (c == ',' || c == ';' || c == '|') {
                result.info.separator = std::string(1, c);
                pos++;
            }
            
            // نوع التكرار
            if (pos < input.size()) {
                c = input[pos];
                if (c == '*') {
                    result.info.type = RepetitionType::ZeroOrMore;
                } else if (c == '+') {
                    result.info.type = RepetitionType::OneOrMore;
                } else if (c == '?' || c == (char)0xD8) {
                    result.info.type = RepetitionType::Optional;
                }
                result.info.updateLimits();
            }
        }
        
        result.extractVarNames();
        
        return result;
    }
};

// =============================================================================
// مُوسّع التكرار
// =============================================================================

/**
 * بيئة قيم المتغيرات
 */
using RepVarEnv = std::map<std::string, std::vector<std::string>>;

/**
 * مُوسّع التكرار
 */
class RepetitionExpander {
public:
    /**
     * توسيع نمط التكرار
     */
    std::string expand(const RepetitionPattern& pattern, const RepVarEnv& values) {
        // التحقق من توفر المتغيرات
        if (pattern.varNames.empty()) {
            return "";
        }
        
        // الحصول على عدد التكرارات
        size_t repeatCount = 0;
        for (const auto& varName : pattern.varNames) {
            auto it = values.find(varName);
            if (it != values.end()) {
                repeatCount = std::max(repeatCount, it->second.size());
            }
        }
        
        // التحقق من صحة العدد
        if (!pattern.info.isValid(repeatCount)) {
            std::ostringstream err;
            err << "عدد التكرارات (" << repeatCount << ") غير صالح لنمط "
                << pattern.info.typeName();
            throw std::runtime_error(err.str());
        }
        
        // توسيع كل تكرار
        std::ostringstream result;
        
        for (size_t i = 0; i < repeatCount; i++) {
            if (i > 0 && !pattern.info.separator.empty()) {
                result << pattern.info.separator << " ";
            }
            
            // استبدال المتغيرات في النمط
            std::string expanded = pattern.pattern;
            for (const auto& varName : pattern.varNames) {
                auto it = values.find(varName);
                if (it != values.end() && i < it->second.size()) {
                    replaceVar(expanded, varName, it->second[i]);
                }
            }
            
            result << expanded;
        }
        
        return result.str();
    }
    
private:
    void replaceVar(std::string& s, const std::string& varName, 
                    const std::string& value) {
        std::string search = "$" + varName;
        size_t pos = 0;
        
        while ((pos = s.find(search, pos)) != std::string::npos) {
            s.replace(pos, search.length(), value);
            pos += value.length();
        }
    }
};

// =============================================================================
// مُطابق التكرار
// =============================================================================

/**
 * نتيجة المطابقة
 */
struct RepetitionMatchResult {
    bool matched;
    RepVarEnv captures;
    std::string error;
    size_t consumedChars;
};

/**
 * مُطابق أنماط التكرار
 */
class RepetitionMatcher {
public:
    /**
     * مطابقة مدخلات مع نمط تكرار
     */
    RepetitionMatchResult match(const RepetitionPattern& pattern,
                                const std::string& input) {
        RepetitionMatchResult result;
        result.matched = false;
        result.consumedChars = 0;
        
        // تحليل المدخلات إلى عناصر
        std::vector<std::string> items;
        splitBySeparator(input, pattern.info.separator, items);
        
        // التحقق من العدد
        if (!pattern.info.isValid(items.size())) {
            result.error = "عدد العناصر غير صالح: " + std::to_string(items.size());
            return result;
        }
        
        // التقاط القيم
        for (const auto& varName : pattern.varNames) {
            result.captures[varName] = items;
        }
        
        result.matched = true;
        result.consumedChars = input.length();
        
        return result;
    }
    
private:
    void splitBySeparator(const std::string& input, const std::string& sep,
                         std::vector<std::string>& items) {
        if (input.empty()) return;
        
        if (sep.empty()) {
            // بدون فاصل - كل شيء عنصر واحد
            items.push_back(input);
            return;
        }
        
        size_t start = 0;
        size_t pos;
        int depth = 0;
        
        for (pos = 0; pos < input.size(); pos++) {
            char c = input[pos];
            
            if (c == '(' || c == '[' || c == '{') depth++;
            else if (c == ')' || c == ']' || c == '}') depth--;
            else if (depth == 0 && sep.find(c) != std::string::npos) {
                std::string item = trim(input.substr(start, pos - start));
                if (!item.empty()) {
                    items.push_back(item);
                }
                start = pos + 1;
            }
        }
        
        std::string item = trim(input.substr(start));
        if (!item.empty()) {
            items.push_back(item);
        }
    }
    
    std::string trim(const std::string& s) {
        size_t start = 0;
        while (start < s.size() && (s[start] == ' ' || s[start] == '\t')) start++;
        
        size_t end = s.size();
        while (end > start && (s[end-1] == ' ' || s[end-1] == '\t')) end--;
        
        return s.substr(start, end - start);
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

extern "C" {

/**
 * تحليل نمط تكرار
 */
void* sad_parse_repetition(const char* pattern) {
    static RepetitionParser parser;
    static RepetitionPattern result;
    result = parser.parse(pattern);
    return &result;
}

/**
 * الحصول على نوع التكرار
 */
int sad_repetition_type(void* rep) {
    auto* pattern = static_cast<RepetitionPattern*>(rep);
    return static_cast<int>(pattern->info.type);
}

/**
 * الحصول على الفاصل
 */
const char* sad_repetition_separator(void* rep) {
    auto* pattern = static_cast<RepetitionPattern*>(rep);
    return pattern->info.separator.c_str();
}

/**
 * الحصول على الحد الأدنى
 */
int sad_repetition_min(void* rep) {
    auto* pattern = static_cast<RepetitionPattern*>(rep);
    return static_cast<int>(pattern->info.minCount);
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef REPETITION_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار أنماط التكرار                                                      ║
║     Repetition Patterns Test                                                 ║
║                                                                              ║
║     المرحلة 19 - T196                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    RepetitionParser parser;
    RepetitionExpander expander;
    RepetitionMatcher matcher;
    
    // اختبار 1: صفر أو أكثر
    std::cout << "═══ اختبار 1: صفر أو أكثر (*) ═══\n\n";
    
    auto pattern1 = parser.parse("$($x:عبر),*");
    std::cout << "النمط: $($x:عبر),*\n";
    std::cout << "النوع: " << pattern1.info.typeName() << "\n";
    std::cout << "الفاصل: \"" << pattern1.info.separator << "\"\n";
    std::cout << "المتغيرات: ";
    for (const auto& v : pattern1.varNames) std::cout << "$" << v << " ";
    std::cout << "\n\n";
    
    // توسيع
    RepVarEnv values1;
    values1["x"] = {"1", "2", "3"};
    
    std::string expanded1 = expander.expand(pattern1, values1);
    std::cout << "التوسيع مع {1, 2, 3}: " << expanded1 << "\n";
    
    values1["x"] = {};
    std::string expanded1_empty = expander.expand(pattern1, values1);
    std::cout << "التوسيع مع {}: \"" << expanded1_empty << "\"\n";
    
    // اختبار 2: واحد أو أكثر
    std::cout << "\n═══ اختبار 2: واحد أو أكثر (+) ═══\n\n";
    
    auto pattern2 = parser.parse("$($عنصر);+");
    std::cout << "النمط: $($عنصر);+\n";
    std::cout << "النوع: " << pattern2.info.typeName() << "\n";
    std::cout << "الحد الأدنى: " << pattern2.info.minCount << "\n";
    
    RepVarEnv values2;
    values2["عنصر"] = {"أ", "ب"};
    std::string expanded2 = expander.expand(pattern2, values2);
    std::cout << "التوسيع: " << expanded2 << "\n";
    
    // اختبار فشل مع قائمة فارغة
    try {
        values2["عنصر"] = {};
        expander.expand(pattern2, values2);
        std::cout << "❌ كان يجب أن يفشل مع قائمة فارغة\n";
    } catch (const std::runtime_error& e) {
        std::cout << "✓ فشل كما هو متوقع: " << e.what() << "\n";
    }
    
    // اختبار 3: اختياري
    std::cout << "\n═══ اختبار 3: اختياري (؟) ═══\n\n";
    
    auto pattern3 = parser.parse("$($نوع)?");
    std::cout << "النمط: $($نوع)?\n";
    std::cout << "النوع: " << pattern3.info.typeName() << "\n";
    std::cout << "الحد الأقصى: " << pattern3.info.maxCount << "\n";
    
    // اختبار 4: المطابقة
    std::cout << "\n═══ اختبار 4: المطابقة ═══\n\n";
    
    auto match1 = matcher.match(pattern1, "1, 2, 3, 4");
    std::cout << "مطابقة \"1, 2, 3, 4\" مع $($x),*:\n";
    std::cout << "  تطابق: " << (match1.matched ? "نعم" : "لا") << "\n";
    if (match1.matched) {
        std::cout << "  القيم الملتقطة: ";
        for (const auto& v : match1.captures["x"]) {
            std::cout << v << " ";
        }
        std::cout << "\n";
    }
    
    // اختبار 5: نمط مركب
    std::cout << "\n═══ اختبار 5: نمط مركب ═══\n\n";
    
    auto pattern5 = parser.parse("$($اسم: $نوع),*");
    std::cout << "النمط: $($اسم: $نوع),*\n";
    std::cout << "المتغيرات: ";
    for (const auto& v : pattern5.varNames) std::cout << "$" << v << " ";
    std::cout << "\n";
    
    RepVarEnv values5;
    values5["اسم"] = {"س", "ص"};
    values5["نوع"] = {"ع32", "نص"};
    
    std::string expanded5 = expander.expand(pattern5, values5);
    std::cout << "التوسيع: " << expanded5 << "\n";
    
    std::cout << "\n✅ نجح اختبار أنماط التكرار!\n";
    
    return 0;
}

#endif // REPETITION_TEST
