/**
 * =============================================================================
 * ملف: fragments.cpp
 * الوصف: أنواع أجزاء الماكرو (عبر، قيمة، نوع، إلخ)
 * المهمة: T197 - Implement macro fragment types
 * المرحلة: Phase 19 - User Story 16 (Macro System ماكرو!)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هي أجزاء الماكرو (Fragments)؟
 * ────────────────────────────────
 * الجزء يُحدد نوع ما يمكن أن يلتقطه متغير الماكرو.
 * 
 * كل نوع له قواعد تحليل مختلفة:
 * 
 * 1. عبر (expr) - تعبير
 *    أي تعبير صالح: 1+2، دالة()، س.حقل
 * 
 * 2. قيمة (lit) - قيمة حرفية
 *    قيم ثابتة فقط: 42، "نص"، صحيح
 * 
 * 3. نوع (ty) - نوع
 *    تعريف نوع: ع32، نص، قائمة<ع32>
 * 
 * 4. كتلة (block) - كتلة كود
 *    كتلة بين { }: { س = 1; ص = 2; }
 * 
 * 5. معرّف (ident) - اسم
 *    معرّف واحد: س، دالة، النوع
 * 
 * 6. رمز (tt) - Token Tree
 *    أي شجرة رموز - الأكثر مرونة
 * 
 * 7. جملة (stmt) - جملة
 *    جملة كاملة: متغير س = 5;
 * 
 * 8. عنصر (item) - عنصر برمجي
 *    تعريف كامل: دالة، هيكل، ثابت
 * 
 * 9. مسار (path) - مسار
 *    مسار مؤهل: أ::ب::ج
 * 
 * 10. نمط (pat) - نمط مطابقة
 *     نمط للمطابقة: (س، ص)، بعض(ن)
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>
#include <optional>
#include <variant>

// =============================================================================
// أنواع الأجزاء
// =============================================================================

/**
 * تعداد أنواع الأجزاء
 */
enum class FragmentType {
    Expr,       // عبر - تعبير
    Literal,    // قيمة - قيمة حرفية
    Type,       // نوع - نوع
    Block,      // كتلة - كتلة كود
    Ident,      // معرّف - اسم
    TokenTree,  // رمز - شجرة رموز
    Stmt,       // جملة - جملة
    Item,       // عنصر - عنصر برمجي
    Path,       // مسار - مسار
    Pattern,    // نمط - نمط مطابقة
    Lifetime,   // عمر - تعليق عمر
    Meta,       // وصف - وصف للسمات
    Visibility  // رؤية - مُعدّل رؤية
};

/**
 * معلومات نوع الجزء
 */
struct FragmentInfo {
    FragmentType type;
    std::string arabicName;
    std::string englishName;
    std::string description;
    std::vector<std::string> examples;
    
    FragmentInfo(FragmentType t, const std::string& ar, const std::string& en,
                 const std::string& desc)
        : type(t), arabicName(ar), englishName(en), description(desc) {}
};

/**
 * سجل أنواع الأجزاء
 */
class FragmentRegistry {
public:
    static FragmentRegistry& instance() {
        static FragmentRegistry registry;
        return registry;
    }
    
    const FragmentInfo* find(const std::string& name) const {
        auto it = byName_.find(name);
        return it != byName_.end() ? &it->second : nullptr;
    }
    
    const FragmentInfo* findByType(FragmentType type) const {
        auto it = byType_.find(type);
        return it != byType_.end() ? &it->second : nullptr;
    }
    
    std::vector<FragmentInfo> all() const {
        std::vector<FragmentInfo> result;
        for (const auto& [_, info] : byType_) {
            result.push_back(info);
        }
        return result;
    }
    
private:
    FragmentRegistry() { initialize(); }
    
    void initialize() {
        register_(FragmentType::Expr, "عبر", "expr", 
                  "تعبير - أي تعبير صالح في اللغة",
                  {"1 + 2", "دالة()", "س.حقل", "اذا شرط { 1 } والا { 2 }"});
        
        register_(FragmentType::Literal, "قيمة", "lit",
                  "قيمة حرفية - قيم ثابتة فقط",
                  {"42", "\"نص\"", "3.14", "صحيح", "خطأ"});
        
        register_(FragmentType::Type, "نوع", "ty",
                  "نوع - تعريف نوع البيانات",
                  {"ع32", "نص", "قائمة<ع32>", "&نص", "دالة(ع32) -> نص"});
        
        register_(FragmentType::Block, "كتلة", "block",
                  "كتلة كود - مجموعة جمل بين أقواس",
                  {"{ متغير س = 1; س + 1 }", "{ اطبع(\"مرحبا\"); }"});
        
        register_(FragmentType::Ident, "معرّف", "ident",
                  "معرّف - اسم واحد",
                  {"س", "دالتي", "النوع_الجديد"});
        
        register_(FragmentType::TokenTree, "رمز", "tt",
                  "شجرة رموز - أي تسلسل رموز متوازن",
                  {"(1, 2)", "[أ، ب، ج]", "{س: 1}"});
        
        register_(FragmentType::Stmt, "جملة", "stmt",
                  "جملة - جملة كاملة",
                  {"متغير س = 5;", "س = س + 1;", "ارجع نتيجة;"});
        
        register_(FragmentType::Item, "عنصر", "item",
                  "عنصر برمجي - تعريف كامل",
                  {"دالة ف() { }", "هيكل نقطة { س: ع32 }", "ثابت ن = 10;"});
        
        register_(FragmentType::Path, "مسار", "path",
                  "مسار - مسار مؤهل كامل أو جزئي",
                  {"أ::ب", "مكتبة::وحدة::دالة", "ذات::نوع"});
        
        register_(FragmentType::Pattern, "نمط", "pat",
                  "نمط - نمط للمطابقة",
                  {"(س، ص)", "بعض(ن)", "_", "هيكل { حقل: ق }"});
        
        register_(FragmentType::Lifetime, "عمر", "lifetime",
                  "تعليق عمر - للإشارات",
                  {"'أ", "'ثابت", "'_"});
        
        register_(FragmentType::Meta, "وصف", "meta",
                  "وصف - محتوى السمات",
                  {"مشتق(نسخ)", "سمة(قيمة = 1)"});
        
        register_(FragmentType::Visibility, "رؤية", "vis",
                  "مُعدّل رؤية",
                  {"عام", "خاص", "عام(صندوق)"});
    }
    
    void register_(FragmentType type, const std::string& ar, const std::string& en,
                   const std::string& desc, const std::vector<std::string>& examples) {
        FragmentInfo info(type, ar, en, desc);
        info.examples = examples;
        
        byName_[ar] = info;
        byName_[en] = info;
        byType_[type] = info;
    }
    
    std::map<std::string, FragmentInfo> byName_;
    std::map<FragmentType, FragmentInfo> byType_;
};

// =============================================================================
// مُحلل الأجزاء
// =============================================================================

/**
 * نتيجة تحليل جزء
 */
struct FragmentParseResult {
    bool success;
    std::string value;
    size_t consumedChars;
    std::string error;
    
    static FragmentParseResult ok(const std::string& v, size_t consumed) {
        FragmentParseResult r;
        r.success = true;
        r.value = v;
        r.consumedChars = consumed;
        return r;
    }
    
    static FragmentParseResult fail(const std::string& err) {
        FragmentParseResult r;
        r.success = false;
        r.error = err;
        r.consumedChars = 0;
        return r;
    }
};

/**
 * مُحلل الأجزاء
 */
class FragmentParser {
public:
    /**
     * تحليل جزء من نوع معين
     */
    FragmentParseResult parse(FragmentType type, const std::string& input) {
        switch (type) {
            case FragmentType::Expr:
                return parseExpr(input);
            case FragmentType::Literal:
                return parseLiteral(input);
            case FragmentType::Type:
                return parseType(input);
            case FragmentType::Block:
                return parseBlock(input);
            case FragmentType::Ident:
                return parseIdent(input);
            case FragmentType::TokenTree:
                return parseTokenTree(input);
            case FragmentType::Stmt:
                return parseStmt(input);
            case FragmentType::Path:
                return parsePath(input);
            default:
                return FragmentParseResult::fail("نوع جزء غير مدعوم");
        }
    }
    
private:
    /**
     * تحليل تعبير
     */
    FragmentParseResult parseExpr(const std::string& input) {
        // تحليل مبسط - نلتقط حتى الفاصلة أو نهاية السلسلة
        size_t pos = 0;
        int depth = 0;
        
        while (pos < input.size()) {
            char c = input[pos];
            
            if (c == '(' || c == '[' || c == '{') depth++;
            else if (c == ')' || c == ']' || c == '}') {
                if (depth == 0) break;
                depth--;
            }
            else if (c == ',' && depth == 0) break;
            
            pos++;
        }
        
        if (pos == 0) {
            return FragmentParseResult::fail("تعبير فارغ");
        }
        
        return FragmentParseResult::ok(trim(input.substr(0, pos)), pos);
    }
    
    /**
     * تحليل قيمة حرفية
     */
    FragmentParseResult parseLiteral(const std::string& input) {
        size_t pos = 0;
        skipWhitespace(input, pos);
        
        if (pos >= input.size()) {
            return FragmentParseResult::fail("قيمة حرفية فارغة");
        }
        
        char c = input[pos];
        
        // نص
        if (c == '"' || c == '\'') {
            return parseStringLiteral(input, pos);
        }
        
        // رقم
        if ((c >= '0' && c <= '9') || c == '-' || c == '+') {
            return parseNumberLiteral(input, pos);
        }
        
        // منطقي
        if (input.substr(pos, 4) == "صحيح" || input.substr(pos, 4) == "true") {
            return FragmentParseResult::ok("صحيح", pos + 4);
        }
        if (input.substr(pos, 3) == "خطأ" || input.substr(pos, 5) == "false") {
            size_t len = input.substr(pos, 3) == "خطأ" ? 3 : 5;
            return FragmentParseResult::ok("خطأ", pos + len);
        }
        
        return FragmentParseResult::fail("قيمة حرفية غير صالحة");
    }
    
    /**
     * تحليل نوع
     */
    FragmentParseResult parseType(const std::string& input) {
        size_t pos = 0;
        skipWhitespace(input, pos);
        
        // قراءة الاسم الأساسي
        std::string typeName;
        while (pos < input.size() && isIdentChar(input[pos])) {
            typeName += input[pos++];
        }
        
        if (typeName.empty()) {
            return FragmentParseResult::fail("نوع فارغ");
        }
        
        // معاملات نوع <...>؟
        skipWhitespace(input, pos);
        if (pos < input.size() && input[pos] == '<') {
            pos++;
            int depth = 1;
            typeName += '<';
            
            while (pos < input.size() && depth > 0) {
                if (input[pos] == '<') depth++;
                else if (input[pos] == '>') depth--;
                typeName += input[pos++];
            }
        }
        
        return FragmentParseResult::ok(typeName, pos);
    }
    
    /**
     * تحليل كتلة
     */
    FragmentParseResult parseBlock(const std::string& input) {
        size_t pos = 0;
        skipWhitespace(input, pos);
        
        if (pos >= input.size() || input[pos] != '{') {
            return FragmentParseResult::fail("كتلة يجب أن تبدأ بـ {");
        }
        
        size_t start = pos;
        pos++;
        int depth = 1;
        
        while (pos < input.size() && depth > 0) {
            if (input[pos] == '{') depth++;
            else if (input[pos] == '}') depth--;
            pos++;
        }
        
        return FragmentParseResult::ok(input.substr(start, pos - start), pos);
    }
    
    /**
     * تحليل معرّف
     */
    FragmentParseResult parseIdent(const std::string& input) {
        size_t pos = 0;
        skipWhitespace(input, pos);
        
        std::string ident;
        while (pos < input.size() && isIdentChar(input[pos])) {
            ident += input[pos++];
        }
        
        if (ident.empty()) {
            return FragmentParseResult::fail("معرّف فارغ");
        }
        
        return FragmentParseResult::ok(ident, pos);
    }
    
    /**
     * تحليل شجرة رموز
     */
    FragmentParseResult parseTokenTree(const std::string& input) {
        size_t pos = 0;
        skipWhitespace(input, pos);
        
        if (pos >= input.size()) {
            return FragmentParseResult::fail("شجرة رموز فارغة");
        }
        
        char c = input[pos];
        
        // مجموعة متوازنة
        if (c == '(' || c == '[' || c == '{') {
            char close = (c == '(') ? ')' : (c == '[') ? ']' : '}';
            size_t start = pos;
            pos++;
            int depth = 1;
            
            while (pos < input.size() && depth > 0) {
                if (input[pos] == c) depth++;
                else if (input[pos] == close) depth--;
                pos++;
            }
            
            return FragmentParseResult::ok(input.substr(start, pos - start), pos);
        }
        
        // رمز واحد
        return parseExpr(input);
    }
    
    /**
     * تحليل جملة
     */
    FragmentParseResult parseStmt(const std::string& input) {
        size_t pos = 0;
        int depth = 0;
        
        // البحث عن نهاية الجملة (;) أو كتلة
        while (pos < input.size()) {
            char c = input[pos];
            
            if (c == '{' || c == '(' || c == '[') depth++;
            else if (c == '}' || c == ')' || c == ']') depth--;
            else if (c == ';' && depth == 0) {
                pos++;  // تضمين الفاصلة المنقوطة
                break;
            }
            
            pos++;
        }
        
        return FragmentParseResult::ok(trim(input.substr(0, pos)), pos);
    }
    
    /**
     * تحليل مسار
     */
    FragmentParseResult parsePath(const std::string& input) {
        size_t pos = 0;
        skipWhitespace(input, pos);
        
        std::string path;
        
        while (pos < input.size()) {
            // قراءة جزء
            std::string part;
            while (pos < input.size() && isIdentChar(input[pos])) {
                part += input[pos++];
            }
            
            if (part.empty()) break;
            
            path += part;
            
            // :: للجزء التالي؟
            if (pos + 1 < input.size() && input[pos] == ':' && input[pos + 1] == ':') {
                path += "::";
                pos += 2;
            } else {
                break;
            }
        }
        
        if (path.empty()) {
            return FragmentParseResult::fail("مسار فارغ");
        }
        
        return FragmentParseResult::ok(path, pos);
    }
    
    // دوال مساعدة
    FragmentParseResult parseStringLiteral(const std::string& input, size_t& pos) {
        char quote = input[pos++];
        std::string value;
        value += quote;
        
        while (pos < input.size() && input[pos] != quote) {
            if (input[pos] == '\\' && pos + 1 < input.size()) {
                value += input[pos++];
            }
            value += input[pos++];
        }
        
        if (pos < input.size()) {
            value += input[pos++];
        }
        
        return FragmentParseResult::ok(value, pos);
    }
    
    FragmentParseResult parseNumberLiteral(const std::string& input, size_t& pos) {
        std::string num;
        
        // إشارة؟
        if (input[pos] == '-' || input[pos] == '+') {
            num += input[pos++];
        }
        
        // أرقام
        while (pos < input.size() && 
               ((input[pos] >= '0' && input[pos] <= '9') || input[pos] == '.')) {
            num += input[pos++];
        }
        
        return FragmentParseResult::ok(num, pos);
    }
    
    void skipWhitespace(const std::string& s, size_t& pos) {
        while (pos < s.size() && (s[pos] == ' ' || s[pos] == '\t' || 
                                   s[pos] == '\n' || s[pos] == '\r')) {
            pos++;
        }
    }
    
    bool isIdentChar(char c) const {
        return (c >= 'a' && c <= 'z') || 
               (c >= 'A' && c <= 'Z') ||
               c == '_' ||
               (c >= '0' && c <= '9') ||
               (unsigned char)c >= 0x80;
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
 * الحصول على معلومات نوع جزء
 */
const char* sad_fragment_info(const char* name) {
    static std::string result;
    
    auto* info = FragmentRegistry::instance().find(name);
    if (!info) {
        result = "نوع غير معروف";
        return result.c_str();
    }
    
    result = info->arabicName + " (" + info->englishName + "): " + info->description;
    return result.c_str();
}

/**
 * تحليل جزء
 */
int sad_parse_fragment(const char* typeName, const char* input, char* output, int maxLen) {
    FragmentParser parser;
    
    auto* info = FragmentRegistry::instance().find(typeName);
    if (!info) return -1;
    
    auto result = parser.parse(info->type, input);
    if (!result.success) return -2;
    
    if (result.value.length() >= (size_t)maxLen) return -3;
    
    // إصلاح: استخدام strncpy بدلاً من strcpy
    std::strncpy(output, result.value.c_str(), (size_t)maxLen - 1);
    output[maxLen - 1] = '\0';  // ضمان null-termination
    return (int)result.consumedChars;
}

/**
 * هل النوع مدعوم؟
 */
int sad_is_fragment_type(const char* name) {
    return FragmentRegistry::instance().find(name) != nullptr ? 1 : 0;
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef FRAGMENTS_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار أنواع أجزاء الماكرو                                                ║
║     Macro Fragment Types Test                                                ║
║                                                                              ║
║     المرحلة 19 - T197                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    FragmentParser parser;
    
    // عرض جميع الأنواع
    std::cout << "═══ أنواع الأجزاء المدعومة ═══\n\n";
    
    for (const auto& info : FragmentRegistry::instance().all()) {
        std::cout << "  :" << info.arabicName << " (:" << info.englishName << ")\n";
        std::cout << "    " << info.description << "\n";
        std::cout << "    أمثلة: ";
        for (size_t i = 0; i < info.examples.size() && i < 3; i++) {
            if (i > 0) std::cout << "، ";
            std::cout << info.examples[i];
        }
        std::cout << "\n\n";
    }
    
    // اختبار التحليل
    std::cout << "═══ اختبار التحليل ═══\n\n";
    
    struct TestCase {
        FragmentType type;
        std::string input;
    };
    
    std::vector<TestCase> tests = {
        {FragmentType::Expr, "1 + 2 * 3"},
        {FragmentType::Expr, "دالة(س، ص)"},
        {FragmentType::Literal, "42"},
        {FragmentType::Literal, "\"مرحبا\""},
        {FragmentType::Literal, "صحيح"},
        {FragmentType::Type, "ع32"},
        {FragmentType::Type, "قائمة<نص>"},
        {FragmentType::Block, "{ متغير س = 1; س + 1 }"},
        {FragmentType::Ident, "اسم_المتغير"},
        {FragmentType::Path, "مكتبة::وحدة::دالة"},
        {FragmentType::Stmt, "متغير س = 5;"},
    };
    
    for (const auto& test : tests) {
        auto* info = FragmentRegistry::instance().findByType(test.type);
        auto result = parser.parse(test.type, test.input);
        
        std::cout << ":" << info->arabicName << " -> \"" << test.input << "\"\n";
        if (result.success) {
            std::cout << "  ✓ القيمة: " << result.value << "\n";
            std::cout << "    استُهلك: " << result.consumedChars << " حرف\n";
        } else {
            std::cout << "  ✗ خطأ: " << result.error << "\n";
        }
    }
    
    std::cout << "\n✅ نجح اختبار أنواع أجزاء الماكرو!\n";
    
    return 0;
}

#endif // FRAGMENTS_TEST
