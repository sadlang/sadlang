/**
 * =============================================================================
 * ملف: arabic_semantic_tokens.cpp
 * الوصف: رموز دلالية عربية لتلوين الكود
 * المهمة: T256 - Arabic semantic tokens
 * المرحلة: Phase 26 - User Story 23 (LSP Advanced)
 * =============================================================================
 * 
 * 🎨 دليل المبتدئ للرموز الدلالية
 * ═══════════════════════════════════════
 * 
 * ما هي الرموز الدلالية؟
 * ───────────────────────
 * طريقة لإخبار المحرر بمعنى كل كلمة في الكود لتلوينها بشكل صحيح.
 * 
 * الفرق عن التلوين العادي:
 * ────────────────────────
 * ```
 * التلوين بالقواعد (Syntax):     التلوين الدلالي (Semantic):
 * ──────────────────────────     ────────────────────────────
 * "كل كلمة تبدأ بـ دالة          "هذه دالة، هذا متغير،
 *  لونها أخضر"                    هذا نوع بيانات"
 * ```
 * 
 * أنواع الرموز في لغة ص:
 * ─────────────────────
 * - دالة     → أخضر
 * - متغير    → أزرق فاتح
 * - ثابت     → أزرق داكن
 * - نوع      → أصفر
 * - معامل    → برتقالي
 * - تعليق    → رمادي
 * - نص       → أحمر فاتح
 * - عدد      → بنفسجي
 * - كلمة_محجوزة → وردي
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>

namespace sad::lsp {

// =============================================================================
// أنواع الرموز الدلالية
// =============================================================================

/**
 * نوع الرمز الدلالي (معيار LSP)
 */
enum class SemanticTokenType {
    Namespace,      // مجال أسماء
    Type,           // نوع بيانات
    Class,          // صنف
    Enum,           // تعداد
    Interface,      // واجهة
    Struct,         // هيكل
    TypeParameter,  // معامل نوع
    Parameter,      // معامل دالة
    Variable,       // متغير
    Property,       // خاصية
    EnumMember,     // عضو تعداد
    Event,          // حدث
    Function,       // دالة
    Method,         // وظيفة
    Macro,          // ماكرو
    Keyword,        // كلمة محجوزة
    Modifier,       // معدّل
    Comment,        // تعليق
    String,         // نص
    Number,         // عدد
    Regexp,         // تعبير نمطي
    Operator,       // عامل
    Decorator       // زخرفة/سمة
};

/**
 * معدّلات الرمز
 */
enum class SemanticTokenModifier {
    Declaration,    // تعريف
    Definition,     // تنفيذ
    Readonly,       // للقراءة فقط
    Static,         // ساكن
    Deprecated,     // مهمل
    Abstract,       // مجرد
    Async,          // غير متزامن
    Modification,   // يُعدّل قيمة
    Documentation,  // توثيق
    DefaultLibrary  // مكتبة قياسية
};

// =============================================================================
// رمز دلالي واحد
// =============================================================================

/**
 * رمز دلالي واحد
 */
struct SemanticToken {
    int line;
    int startChar;
    int length;
    SemanticTokenType type;
    std::set<SemanticTokenModifier> modifiers;
    
    /**
     * تحويل لـ "delta encoding" (معيار LSP)
     */
    struct EncodedToken {
        int deltaLine;
        int deltaStart;
        int length;
        int tokenType;
        int tokenModifiers;
    };
    
    EncodedToken encode(int prevLine, int prevStart) const {
        EncodedToken enc;
        enc.deltaLine = line - prevLine;
        enc.deltaStart = (enc.deltaLine == 0) ? (startChar - prevStart) : startChar;
        enc.length = length;
        enc.tokenType = static_cast<int>(type);
        
        enc.tokenModifiers = 0;
        for (auto mod : modifiers) {
            enc.tokenModifiers |= (1 << static_cast<int>(mod));
        }
        
        return enc;
    }
};

// =============================================================================
// الكلمات المحجوزة العربية
// =============================================================================

class ArabicKeywords {
public:
    static const std::set<std::string>& getKeywords() {
        static std::set<std::string> keywords = {
            // التحكم
            "إذا", "وإلا", "بينما", "لكل", "في", "توقف", "استمر", "أرجع",
            // التعريفات
            "دالة", "متغير", "ثابت", "صنف", "هيكل", "تعداد", "واجهة", "نوع",
            // الملكية
            "انقل", "استعر", "استنسخ", "أسقط",
            // المعدلات
            "عام", "خاص", "محمي", "ساكن", "مجرد", "نهائي",
            // الأنواع الأساسية
            "عدد", "عشري", "نص", "منطقي", "فراغ", "مصفوفة", "قاموس",
            // القيم
            "صحيح", "خطأ", "عدم",
            // أخرى
            "استورد", "من", "كـ", "ذاتي", "سوبر", "جديد", "نهاية"
        };
        return keywords;
    }
    
    static const std::set<std::string>& getBuiltinTypes() {
        static std::set<std::string> types = {
            "عدد", "عدد8", "عدد16", "عدد32", "عدد64",
            "طبيعي", "طبيعي8", "طبيعي16", "طبيعي32", "طبيعي64",
            "عشري32", "عشري64",
            "نص", "حرف", "منطقي", "فراغ",
            "مصفوفة", "قائمة", "قاموس", "مجموعة", "اختياري", "نتيجة"
        };
        return types;
    }
    
    static const std::set<std::string>& getBuiltinFunctions() {
        static std::set<std::string> funcs = {
            "اطبع", "اقرأ", "طول", "نوع", "حجم",
            "إلى_نص", "إلى_عدد", "إلى_عشري",
            "أضف", "احذف", "أدرج", "ابحث", "رتب", "اعكس"
        };
        return funcs;
    }
    
    /**
     * أسماء عناصر الواجهة — تُلوَّن كأصناف (Class)
     */
    static const std::set<std::string>& getUiWidgets() {
        static std::set<std::string> widgets = {
            "نص", "عنوان", "تسمية", "فقرة", "رابط", "كود", "اقتباس",
            "صورة", "ايقونة", "شارة", "رقاقة", "صورة_رمزية",
            "زر", "زر_محيط", "زر_نصي", "زر_ايقونة", "زر_عائم", "زر_تبديل",
            "حقل", "حقل_بحث", "حقل_سر", "منطقة_نص", "مفتاح", "منزلق",
            "مربع_اختيار", "قائمة_منسدلة", "تقييم",
            "منتقي_تاريخ", "منتقي_وقت", "منتقي_لون", "عجلة_ألوان", "تقويم",
            "زر_راديو", "خطوات", "تحكم_مقسم",
            "شريط_تقدم", "تقدم_دائري", "تحميل", "هيكل_تحميل",
            "عمود", "صف", "تكديس", "شبكة", "حاوية", "توسيط",
            "مرن", "موسّع", "فاصل", "خط_فاصل",
            "التفاف", "صندوق", "سطح", "عمود_كسول", "صف_كسول",
            "شبكة_كسولة", "عرض_تمرير", "عرض_مقسم",
            "هيكل", "شريط_تطبيق", "شريط_سفلي", "عنصر_تنقل",
            "درج", "عنصر_درج", "شريط_تبويب", "تبويب", "محتوى_تبويب",
            "عرض_تنقل", "رابط_تنقل", "تنقل_جانبي", "شريط_أدوات", "قائمة_ضخمة",
            "قائمة", "عنصر_قائمة_بيانات", "بطاقة", "قائمة_شبكية",
            "قائمة_تمرير", "أكورديون",
            "حوار", "حوار_تأكيد", "لوحة_سفلية", "رسالة_منبثقة",
            "تلميح", "قائمة_منبثقة", "عنصر_قائمة",
            "مشغل_فيديو", "مشغل_صوت", "معرض_صور", "عرض_دوار",
            "نص_منسق", "ماركداون", "لوحة", "عرض_ويب", "خريطة",
            "جدول_بيانات", "عرض_شجري", "مسار_تنقل", "ترقيم_صفحات",
            "خط_زمني", "قسم",
            "وميض", "قائمة_متحركة", "قابل_للطي", "قابل_للسحب", "تنبيه", "مجموعة"
        };
        return widgets;
    }
};

// =============================================================================
// محلل الرموز الدلالية
// =============================================================================

class SemanticTokenAnalyzer {
public:
    /**
     * تحليل ملف وإرجاع الرموز الدلالية
     */
    std::vector<SemanticToken> analyze(const std::string& source) {
        std::vector<SemanticToken> tokens;
        
        std::istringstream iss(source);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(iss, line)) {
            auto lineTokens = analyzeLine(line, lineNum);
            tokens.insert(tokens.end(), lineTokens.begin(), lineTokens.end());
            lineNum++;
        }
        
        return tokens;
    }
    
private:
    std::vector<SemanticToken> analyzeLine(const std::string& line, int lineNum) {
        std::vector<SemanticToken> tokens;
        
        // تحليل التعليقات
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            SemanticToken tok;
            tok.line = lineNum;
            tok.startChar = commentPos;
            tok.length = line.length() - commentPos;
            tok.type = SemanticTokenType::Comment;
            tokens.push_back(tok);
        }
        
        // تحليل النصوص
        size_t strStart = 0;
        while ((strStart = line.find('"', strStart)) != std::string::npos) {
            size_t strEnd = line.find('"', strStart + 1);
            if (strEnd != std::string::npos) {
                SemanticToken tok;
                tok.line = lineNum;
                tok.startChar = strStart;
                tok.length = strEnd - strStart + 1;
                tok.type = SemanticTokenType::String;
                tokens.push_back(tok);
                strStart = strEnd + 1;
            } else {
                break;
            }
        }
        
        // تحليل الكلمات
        std::string word;
        size_t wordStart = 0;
        bool inWord = false;
        
        for (size_t i = 0; i <= line.length(); i++) {
            char c = (i < line.length()) ? line[i] : ' ';
            bool isWordChar = isArabicChar(c) || std::isalnum(static_cast<unsigned char>(c)) || c == '_';
            
            if (isWordChar && !inWord) {
                wordStart = i;
                inWord = true;
                word.clear();
            }
            
            if (isWordChar) {
                word += c;
            }
            
            if (!isWordChar && inWord) {
                // نهاية الكلمة
                auto token = classifyWord(word, lineNum, wordStart);
                if (token.length > 0) {
                    tokens.push_back(token);
                }
                inWord = false;
            }
        }
        
        // تحليل الأعداد
        for (size_t i = 0; i < line.length(); i++) {
            if (std::isdigit(static_cast<unsigned char>(line[i]))) {
                size_t numStart = i;
                while (i < line.length() && 
                       (std::isdigit(static_cast<unsigned char>(line[i])) || line[i] == '.')) {
                    i++;
                }
                SemanticToken tok;
                tok.line = lineNum;
                tok.startChar = numStart;
                tok.length = i - numStart;
                tok.type = SemanticTokenType::Number;
                tokens.push_back(tok);
            }
        }
        
        return tokens;
    }
    
    SemanticToken classifyWord(const std::string& word, int line, int start) {
        SemanticToken tok;
        tok.line = line;
        tok.startChar = start;
        tok.length = word.length();
        
        // الكلمات المحجوزة
        if (ArabicKeywords::getKeywords().count(word)) {
            tok.type = SemanticTokenType::Keyword;
            return tok;
        }
        
        // الأنواع المدمجة
        if (ArabicKeywords::getBuiltinTypes().count(word)) {
            tok.type = SemanticTokenType::Type;
            tok.modifiers.insert(SemanticTokenModifier::DefaultLibrary);
            return tok;
        }
        
        // الدوال المدمجة
        if (ArabicKeywords::getBuiltinFunctions().count(word)) {
            tok.type = SemanticTokenType::Function;
            tok.modifiers.insert(SemanticTokenModifier::DefaultLibrary);
            return tok;
        }
        
        // عناصر الواجهة — تُلوَّن كأصناف
        if (ArabicKeywords::getUiWidgets().count(word)) {
            tok.type = SemanticTokenType::Class;
            tok.modifiers.insert(SemanticTokenModifier::DefaultLibrary);
            return tok;
        }
        
        // افتراضي: متغير
        tok.type = SemanticTokenType::Variable;
        tok.length = 0;  // تجاهل
        return tok;
    }
    
    bool isArabicChar(char c) {
        // تحقق بسيط - الأحرف العربية في UTF-8
        return (static_cast<unsigned char>(c) >= 0xD8);
    }
};

// =============================================================================
// مُزود الرموز الدلالية
// =============================================================================

class SemanticTokensProvider {
public:
    /**
     * الحصول على الرموز الدلالية لملف
     */
    std::vector<SemanticToken> getTokens(const std::string& source) {
        SemanticTokenAnalyzer analyzer;
        return analyzer.analyze(source);
    }
    
    /**
     * تحويل الرموز لتنسيق LSP
     */
    std::vector<int> encodeTokens(const std::vector<SemanticToken>& tokens) {
        std::vector<int> data;
        
        int prevLine = 0;
        int prevStart = 0;
        
        for (const auto& token : tokens) {
            auto enc = token.encode(prevLine, prevStart);
            
            data.push_back(enc.deltaLine);
            data.push_back(enc.deltaStart);
            data.push_back(enc.length);
            data.push_back(enc.tokenType);
            data.push_back(enc.tokenModifiers);
            
            if (enc.deltaLine > 0) {
                prevStart = 0;
            }
            prevLine = token.line;
            prevStart = token.startChar;
        }
        
        return data;
    }
    
    /**
     * الحصول على أسطورة الأنواع
     */
    std::vector<std::string> getTokenTypes() {
        return {
            "namespace", "type", "class", "enum", "interface",
            "struct", "typeParameter", "parameter", "variable",
            "property", "enumMember", "event", "function", "method",
            "macro", "keyword", "modifier", "comment", "string",
            "number", "regexp", "operator", "decorator"
        };
    }
    
    /**
     * الحصول على أسطورة المعدّلات
     */
    std::vector<std::string> getTokenModifiers() {
        return {
            "declaration", "definition", "readonly", "static",
            "deprecated", "abstract", "async", "modification",
            "documentation", "defaultLibrary"
        };
    }
    
    /**
     * الحصول على التهيئة كـ JSON
     */
    std::string getLegendJson() {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"tokenTypes\": [";
        auto types = getTokenTypes();
        for (size_t i = 0; i < types.size(); i++) {
            if (i > 0) ss << ", ";
            ss << "\"" << types[i] << "\"";
        }
        ss << "],\n";
        
        ss << "  \"tokenModifiers\": [";
        auto mods = getTokenModifiers();
        for (size_t i = 0; i < mods.size(); i++) {
            if (i > 0) ss << ", ";
            ss << "\"" << mods[i] << "\"";
        }
        ss << "]\n";
        ss << "}";
        
        return ss.str();
    }
    
    /**
     * الحصول على البيانات كـ JSON
     */
    std::string getTokensJson(const std::string& source) {
        auto tokens = getTokens(source);
        auto data = encodeTokens(tokens);
        
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"data\": [";
        for (size_t i = 0; i < data.size(); i++) {
            if (i > 0) ss << ", ";
            ss << data[i];
        }
        ss << "]\n";
        ss << "}";
        
        return ss.str();
    }
};

} // namespace sad::lsp

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadSemanticTokensProvider {
    sad::lsp::SemanticTokensProvider* provider;
};

SadSemanticTokensProvider* sad_semantic_tokens_new() {
    auto* ctx = new SadSemanticTokensProvider();
    ctx->provider = new sad::lsp::SemanticTokensProvider();
    return ctx;
}

const char* sad_semantic_tokens_legend(SadSemanticTokensProvider* ctx) {
    static std::string result;
    result = ctx->provider->getLegendJson();
    return result.c_str();
}

const char* sad_semantic_tokens_get(SadSemanticTokensProvider* ctx, const char* source) {
    static std::string result;
    result = ctx->provider->getTokensJson(source);
    return result.c_str();
}

void sad_semantic_tokens_free(SadSemanticTokensProvider* ctx) {
    if (ctx) {
        delete ctx->provider;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef SEMANTIC_TOKENS_TEST

#include <iostream>
#include <cassert>

void testKeywordDetection() {
    std::cout << "🔑 اختبار اكتشاف الكلمات المحجوزة...\n";
    
    sad::lsp::SemanticTokenAnalyzer analyzer;
    
    std::string source = "دالة مرحبا() {\n    أرجع 0\n}\n";
    
    auto tokens = analyzer.analyze(source);
    
    bool hasKeyword = false;
    for (const auto& t : tokens) {
        if (t.type == sad::lsp::SemanticTokenType::Keyword) {
            hasKeyword = true;
            break;
        }
    }
    
    assert(hasKeyword);
    std::cout << "   ✅ نجح\n";
}

void testStringDetection() {
    std::cout << "📝 اختبار اكتشاف النصوص...\n";
    
    sad::lsp::SemanticTokenAnalyzer analyzer;
    
    std::string source = R"(متغير س = "مرحبا بالعالم")";
    
    auto tokens = analyzer.analyze(source);
    
    bool hasString = false;
    for (const auto& t : tokens) {
        if (t.type == sad::lsp::SemanticTokenType::String) {
            hasString = true;
            break;
        }
    }
    
    assert(hasString);
    std::cout << "   ✅ نجح\n";
}

void testNumberDetection() {
    std::cout << "🔢 اختبار اكتشاف الأعداد...\n";
    
    sad::lsp::SemanticTokenAnalyzer analyzer;
    
    std::string source = "متغير س = 42\nمتغير ع = 3.14";
    
    auto tokens = analyzer.analyze(source);
    
    int numberCount = 0;
    for (const auto& t : tokens) {
        if (t.type == sad::lsp::SemanticTokenType::Number) {
            numberCount++;
        }
    }
    
    assert(numberCount >= 2);
    std::cout << "   ✅ نجح\n";
}

void testCommentDetection() {
    std::cout << "💬 اختبار اكتشاف التعليقات...\n";
    
    sad::lsp::SemanticTokenAnalyzer analyzer;
    
    std::string source = "// هذا تعليق\nمتغير س = 10";
    
    auto tokens = analyzer.analyze(source);
    
    bool hasComment = false;
    for (const auto& t : tokens) {
        if (t.type == sad::lsp::SemanticTokenType::Comment) {
            hasComment = true;
            break;
        }
    }
    
    assert(hasComment);
    std::cout << "   ✅ نجح\n";
}

void testLspEncoding() {
    std::cout << "📊 اختبار ترميز LSP...\n";
    
    sad::lsp::SemanticTokensProvider provider;
    
    std::string source = "دالة اختبار() {\n}\n";
    
    auto tokens = provider.getTokens(source);
    auto encoded = provider.encodeTokens(tokens);
    
    // يجب أن يكون عدد العناصر مضاعف 5
    assert(encoded.size() % 5 == 0);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات الرموز الدلالية\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testKeywordDetection();
    testStringDetection();
    testNumberDetection();
    testCommentDetection();
    testLspEncoding();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // SEMANTIC_TOKENS_TEST
