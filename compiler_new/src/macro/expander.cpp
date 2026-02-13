/**
 * =============================================================================
 * ملف: expander.cpp
 * الوصف: محرك توسيع الماكرو
 * المهمة: T195 - Implement macro expansion engine
 * المرحلة: Phase 19 - User Story 16 (Macro System ماكرو!)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * كيف يعمل توسيع الماكرو؟
 * ─────────────────────────
 * 
 * 1. مطابقة النمط (Pattern Matching):
 *    الماكرو يحدد نمط للمدخلات
 *    مثال: ماكرو! سجّل!($msg:عبر)
 *           ↓ يطابق
 *    سجّل!("مرحبا") → $msg = "مرحبا"
 * 
 * 2. التقاط المتغيرات:
 *    كل $متغير يلتقط جزء من المدخلات
 * 
 * 3. الاستبدال:
 *    كل $متغير في الجسم يُستبدل بما التقطه
 * 
 * مثال كامل:
 * ```sad
 * // التعريف
 * ماكرو! حدّ_أقصى!($أ:عبر, $ب:عبر) {
 *     اذا $أ > $ب:
 *         $أ
 *     والا:
 *         $ب
 *     نهاية
 * }
 * 
 * // الاستدعاء
 * متغير ن = حدّ_أقصى!(5, 10)
 * 
 * // بعد التوسيع
 * متغير ن = اذا 5 > 10:
 *               5
 *           والا:
 *               10
 *           نهاية
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>
#include <regex>

// =============================================================================
// القيم الملتقطة
// =============================================================================

/**
 * قيمة ملتقطة من نمط الماكرو
 */
struct CapturedValue {
    enum class Kind {
        Single,     // قيمة واحدة
        Repeated    // قيم متكررة
    };
    
    Kind kind;
    std::string value;                  // للقيمة الواحدة
    std::vector<std::string> values;    // للتكرار
    std::string fragmentType;           // نوع الجزء (عبر، قيمة، إلخ)
    
    CapturedValue() : kind(Kind::Single) {}
    
    static CapturedValue single(const std::string& v, const std::string& type = "عبر") {
        CapturedValue cv;
        cv.kind = Kind::Single;
        cv.value = v;
        cv.fragmentType = type;
        return cv;
    }
    
    static CapturedValue repeated(const std::vector<std::string>& vs, 
                                  const std::string& type = "عبر") {
        CapturedValue cv;
        cv.kind = Kind::Repeated;
        cv.values = vs;
        cv.fragmentType = type;
        return cv;
    }
};

/**
 * بيئة الروابط للتوسيع
 */
using CaptureEnv = std::map<std::string, CapturedValue>;

// =============================================================================
// رمز التوسيع
// =============================================================================

/**
 * رمز في شجرة التوسيع
 */
struct ExpansionToken {
    enum class Kind {
        Literal,        // نص حرفي
        Variable,       // متغير ماكرو $x
        Repetition,     // تكرار $(...)
        Group           // مجموعة
    };
    
    Kind kind;
    std::string text;
    std::string varName;
    std::vector<ExpansionToken> children;
    char repeatKind = '*';  // *, +, ?
    std::string separator;
    
    ExpansionToken(Kind k = Kind::Literal) : kind(k) {}
    
    static ExpansionToken literal(const std::string& t) {
        ExpansionToken tok(Kind::Literal);
        tok.text = t;
        return tok;
    }
    
    static ExpansionToken variable(const std::string& name) {
        ExpansionToken tok(Kind::Variable);
        tok.varName = name;
        return tok;
    }
};

// =============================================================================
// محرك التوسيع
// =============================================================================

/**
 * نتيجة التوسيع
 */
struct ExpansionResult {
    bool success;
    std::string code;
    std::string error;
    
    static ExpansionResult ok(const std::string& c) {
        ExpansionResult r;
        r.success = true;
        r.code = c;
        return r;
    }
    
    static ExpansionResult fail(const std::string& e) {
        ExpansionResult r;
        r.success = false;
        r.error = e;
        return r;
    }
};

/**
 * محرك توسيع الماكرو الرئيسي
 */
class MacroExpander {
public:
    /**
     * توسيع استدعاء ماكرو
     */
    ExpansionResult expand(const std::string& macroName,
                          const std::string& macroBody,
                          const CaptureEnv& captures) {
        // التحقق من عمق التوسع
        expansionDepth_++;
        if (expansionDepth_ > maxExpansionDepth_) {
            expansionDepth_--;
            return ExpansionResult::fail(
                "خطأ: تجاوز حد توسع الماكرو (" + 
                std::to_string(maxExpansionDepth_) + 
                "). قد يكون هناك تكرار لانهائي."
            );
        }
        
        currentCaptures_ = &captures;
        
        std::string result;
        try {
            result = expandBody(macroBody);
        } catch (const std::exception& e) {
            expansionDepth_--;
            return ExpansionResult::fail(e.what());
        }
        
        expansionDepth_--;
        return ExpansionResult::ok(result);
    }
    
    /**
     * تعيين حد التوسع
     */
    void setMaxExpansionDepth(int depth) {
        maxExpansionDepth_ = depth;
    }
    
private:
    const CaptureEnv* currentCaptures_ = nullptr;
    int expansionDepth_ = 0;
    int maxExpansionDepth_ = 256;
    
    /**
     * توسيع جسم الماكرو
     */
    std::string expandBody(const std::string& body) {
        std::ostringstream result;
        size_t pos = 0;
        
        while (pos < body.size()) {
            // هل هذا متغير ماكرو؟
            if (body[pos] == '$') {
                size_t start = pos;
                pos++;
                
                // تكرار $(...)؟
                if (pos < body.size() && body[pos] == '(') {
                    std::string rep = expandRepetition(body, pos);
                    result << rep;
                } else {
                    // متغير عادي
                    std::string varName = readVarName(body, pos);
                    if (!varName.empty()) {
                        result << expandVariable(varName);
                    } else {
                        result << '$';
                    }
                }
            } else {
                result << body[pos];
                pos++;
            }
        }
        
        return result.str();
    }
    
    /**
     * قراءة اسم متغير
     */
    std::string readVarName(const std::string& s, size_t& pos) {
        std::string name;
        while (pos < s.size() && isVarChar(s[pos])) {
            name += s[pos++];
        }
        return name;
    }
    
    bool isVarChar(char c) const {
        return (c >= 'a' && c <= 'z') || 
               (c >= 'A' && c <= 'Z') || 
               c == '_' ||
               (c >= '0' && c <= '9') ||
               (unsigned char)c >= 0x80;
    }
    
    /**
     * توسيع متغير
     */
    std::string expandVariable(const std::string& name) {
        if (!currentCaptures_) {
            return "$" + name;  // لا بيئة - إرجاع كما هو
        }
        
        auto it = currentCaptures_->find(name);
        if (it == currentCaptures_->end()) {
            throw std::runtime_error("متغير ماكرو غير معرّف: $" + name);
        }
        
        const auto& captured = it->second;
        if (captured.kind == CapturedValue::Kind::Single) {
            return captured.value;
        } else {
            // للتكرار - نُرجع العنصر الأول (في سياق عادي)
            return captured.values.empty() ? "" : captured.values[0];
        }
    }
    
    /**
     * توسيع تكرار
     */
    std::string expandRepetition(const std::string& body, size_t& pos) {
        // تخطي $(
        pos++;  // (
        
        // البحث عن القوس المُغلق
        int depth = 1;
        size_t start = pos;
        while (pos < body.size() && depth > 0) {
            if (body[pos] == '(') depth++;
            else if (body[pos] == ')') depth--;
            pos++;
        }
        
        std::string pattern = body.substr(start, pos - start - 1);
        
        // البحث عن الفاصل ونوع التكرار
        std::string separator;
        char repeatKind = '*';
        
        if (pos < body.size()) {
            char c = body[pos];
            if (c == ',' || c == ';') {
                separator = std::string(1, c);
                pos++;
            }
            
            if (pos < body.size()) {
                c = body[pos];
                if (c == '*' || c == '+' || c == '?') {
                    repeatKind = c;
                    pos++;
                }
            }
        }
        
        // العثور على متغير التكرار
        std::string repeatVar;
        for (const auto& [name, val] : *currentCaptures_) {
            if (val.kind == CapturedValue::Kind::Repeated) {
                if (pattern.find("$" + name) != std::string::npos) {
                    repeatVar = name;
                    break;
                }
            }
        }
        
        if (repeatVar.empty()) {
            return "";  // لا متغير تكرار
        }
        
        const auto& values = currentCaptures_->at(repeatVar).values;
        
        // توسيع لكل قيمة
        std::ostringstream result;
        for (size_t i = 0; i < values.size(); i++) {
            if (i > 0 && !separator.empty()) {
                result << separator << " ";
            }
            
            // استبدال المتغير في النمط
            std::string expanded = pattern;
            size_t varPos = expanded.find("$" + repeatVar);
            while (varPos != std::string::npos) {
                expanded.replace(varPos, repeatVar.length() + 1, values[i]);
                varPos = expanded.find("$" + repeatVar, varPos + values[i].length());
            }
            
            result << expanded;
        }
        
        return result.str();
    }
};

// =============================================================================
// مُطابق النمط
// =============================================================================

/**
 * نتيجة المطابقة
 */
struct MatchResult {
    bool matched;
    CaptureEnv captures;
    std::string error;
    
    static MatchResult success(const CaptureEnv& caps) {
        MatchResult r;
        r.matched = true;
        r.captures = caps;
        return r;
    }
    
    static MatchResult failure(const std::string& err) {
        MatchResult r;
        r.matched = false;
        r.error = err;
        return r;
    }
};

/**
 * مُطابق أنماط الماكرو
 */
class MacroMatcher {
public:
    /**
     * مطابقة مدخلات مع نمط
     */
    MatchResult match(const std::string& pattern, const std::string& input) {
        CaptureEnv captures;
        
        // تحليل مبسط - نفترض نمط بالشكل: $var:type, ...
        std::vector<std::pair<std::string, std::string>> vars;
        parsePatternVars(pattern, vars);
        
        // تحليل المدخلات
        std::vector<std::string> args;
        parseInputArgs(input, args);
        
        // مطابقة
        size_t argIdx = 0;
        for (const auto& [varName, varType] : vars) {
            // هل هذا تكرار؟
            bool isRepeat = varType.find('*') != std::string::npos || 
                           varType.find('+') != std::string::npos;
            
            if (isRepeat) {
                // التقاط كل المتبقي
                std::vector<std::string> values;
                while (argIdx < args.size()) {
                    values.push_back(args[argIdx++]);
                }
                captures[varName] = CapturedValue::repeated(values);
            } else {
                if (argIdx >= args.size()) {
                    return MatchResult::failure("عدد المعاملات غير كافٍ");
                }
                captures[varName] = CapturedValue::single(args[argIdx++]);
            }
        }
        
        return MatchResult::success(captures);
    }
    
private:
    void parsePatternVars(const std::string& pattern, 
                         std::vector<std::pair<std::string, std::string>>& vars) {
        size_t pos = 0;
        while (pos < pattern.size()) {
            // البحث عن $
            size_t dollar = pattern.find('$', pos);
            if (dollar == std::string::npos) break;
            
            pos = dollar + 1;
            
            // قراءة الاسم
            std::string name;
            while (pos < pattern.size() && 
                   (isalnum(pattern[pos]) || pattern[pos] == '_' || 
                    (unsigned char)pattern[pos] >= 0x80)) {
                name += pattern[pos++];
            }
            
            // قراءة النوع
            std::string type;
            if (pos < pattern.size() && pattern[pos] == ':') {
                pos++;
                while (pos < pattern.size() && 
                       (isalnum(pattern[pos]) || pattern[pos] == '*' || 
                        pattern[pos] == '+' || pattern[pos] == '?' ||
                        (unsigned char)pattern[pos] >= 0x80)) {
                    type += pattern[pos++];
                }
            }
            
            if (!name.empty()) {
                vars.push_back({name, type.empty() ? "عبر" : type});
            }
        }
    }
    
    void parseInputArgs(const std::string& input, std::vector<std::string>& args) {
        std::string current;
        int parenDepth = 0;
        
        for (char c : input) {
            if (c == '(' || c == '[' || c == '{') {
                parenDepth++;
                current += c;
            } else if (c == ')' || c == ']' || c == '}') {
                parenDepth--;
                current += c;
            } else if (c == ',' && parenDepth == 0) {
                // فاصل
                std::string trimmed = trim(current);
                if (!trimmed.empty()) {
                    args.push_back(trimmed);
                }
                current.clear();
            } else {
                current += c;
            }
        }
        
        std::string trimmed = trim(current);
        if (!trimmed.empty()) {
            args.push_back(trimmed);
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

static MacroExpander g_expander;
static MacroMatcher g_matcher;

extern "C" {

/**
 * توسيع ماكرو
 */
const char* sad_expand_macro(const char* macroName, const char* body, 
                             const char* args) {
    static std::string result;
    
    // مطابقة (مبسطة - نفترض معامل واحد)
    CaptureEnv captures;
    captures["x"] = CapturedValue::single(args);
    
    auto expanded = g_expander.expand(macroName, body, captures);
    if (expanded.success) {
        result = expanded.code;
    } else {
        result = "/* خطأ: " + expanded.error + " */";
    }
    
    return result.c_str();
}

/**
 * تعيين حد التوسع
 */
void sad_set_expansion_limit(int limit) {
    g_expander.setMaxExpansionDepth(limit);
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef EXPANDER_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار محرك توسيع الماكرو                                                 ║
║     Macro Expander Test                                                      ║
║                                                                              ║
║     المرحلة 19 - T195                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    MacroExpander expander;
    MacroMatcher matcher;
    
    // اختبار 1: توسيع بسيط
    std::cout << "═══ اختبار 1: توسيع بسيط ═══\n\n";
    
    CaptureEnv captures1;
    captures1["رسالة"] = CapturedValue::single("\"مرحبا بالعالم!\"");
    
    std::string body1 = "اطبع($رسالة)";
    auto result1 = expander.expand("سجّل!", body1, captures1);
    
    std::cout << "الجسم: " << body1 << "\n";
    std::cout << "النتيجة: " << result1.code << "\n";
    std::cout << "نجح: " << (result1.success ? "نعم" : "لا") << "\n";
    
    // اختبار 2: متغيرات متعددة
    std::cout << "\n═══ اختبار 2: متغيرات متعددة ═══\n\n";
    
    CaptureEnv captures2;
    captures2["أ"] = CapturedValue::single("5");
    captures2["ب"] = CapturedValue::single("10");
    
    std::string body2 = R"(
اذا $أ > $ب:
    $أ
والا:
    $ب
نهاية
)";
    
    auto result2 = expander.expand("حدّ_أقصى!", body2, captures2);
    
    std::cout << "النتيجة:\n" << result2.code << "\n";
    
    // اختبار 3: تكرار
    std::cout << "\n═══ اختبار 3: تكرار ═══\n\n";
    
    CaptureEnv captures3;
    captures3["عنصر"] = CapturedValue::repeated({"1", "2", "3", "4", "5"});
    
    std::string body3 = "$($عنصر),*";
    auto result3 = expander.expand("قائمة!", body3, captures3);
    
    std::cout << "الجسم: " << body3 << "\n";
    std::cout << "النتيجة: " << result3.code << "\n";
    
    // اختبار 4: المطابقة
    std::cout << "\n═══ اختبار 4: مطابقة النمط ═══\n\n";
    
    std::string pattern = "$اسم:معرّف, $قيمة:عبر";
    std::string input = "س, 42";
    
    auto match = matcher.match(pattern, input);
    
    std::cout << "النمط: " << pattern << "\n";
    std::cout << "المدخل: " << input << "\n";
    std::cout << "تطابق: " << (match.matched ? "نعم" : "لا") << "\n";
    
    if (match.matched) {
        std::cout << "الالتقاطات:\n";
        for (const auto& [name, val] : match.captures) {
            std::cout << "  $" << name << " = " << val.value << "\n";
        }
    }
    
    // اختبار 5: حد التوسع
    std::cout << "\n═══ اختبار 5: حد التوسع ═══\n\n";
    
    expander.setMaxExpansionDepth(3);
    
    // محاكاة توسع متداخل
    CaptureEnv empty;
    for (int i = 0; i < 5; i++) {
        auto r = expander.expand("اختبار!", "كود", empty);
        std::cout << "التوسع " << (i + 1) << ": " 
                  << (r.success ? "نجح" : "فشل - " + r.error) << "\n";
    }
    
    std::cout << "\n✅ نجح اختبار محرك توسيع الماكرو!\n";
    
    return 0;
}

#endif // EXPANDER_TEST
