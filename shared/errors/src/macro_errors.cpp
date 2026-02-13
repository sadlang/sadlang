/**
 * =============================================================================
 * ملف: macro_errors.cpp
 * الوصف: رسائل خطأ الماكرو التشخيصية
 * المهمة: T199 - Implement macro error diagnostics
 * المرحلة: Phase 19 - User Story 16 (Macro System ماكرو!)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * لماذا رسائل الخطأ مهمة للماكرو؟
 * ────────────────────────────────
 * الماكروات معقدة لأن الخطأ قد يكون:
 * 1. في تعريف الماكرو
 * 2. في استدعاء الماكرو
 * 3. في الكود المُولّد بعد التوسيع
 * 
 * رسائل الخطأ الجيدة يجب أن:
 * - تحدد مصدر الخطأ بدقة
 * - تظهر السياق (الماكرو والتوسيع)
 * - تقترح حلولاً
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <memory>
#include <optional>

// =============================================================================
// أنواع الأخطاء
// =============================================================================

/**
 * فئة خطأ الماكرو
 */
enum class MacroErrorKind {
    // أخطاء التعريف
    InvalidMacroName,           // اسم ماكرو غير صالح
    InvalidPatternSyntax,       // صيغة نمط غير صالحة
    DuplicateMacroVar,          // متغير ماكرو مكرر
    InvalidFragmentType,        // نوع جزء غير صالح
    
    // أخطاء الاستدعاء
    UndefinedMacro,             // ماكرو غير معرّف
    ArgumentCountMismatch,      // عدد المعاملات غير متطابق
    ArgumentTypeMismatch,       // نوع المعامل غير متطابق
    
    // أخطاء التوسيع
    RecursionLimitExceeded,     // تجاوز حد التكرار
    UndefinedMacroVar,          // متغير ماكرو غير معرّف
    RepetitionMismatch,         // عدم تطابق التكرار
    
    // أخطاء النظافة
    HygieneViolation,           // انتهاك النظافة
    NameCollision,              // تصادم الأسماء
    
    // أخطاء النطاق
    MacroNotVisible,            // ماكرو غير مرئي
    MacroUsedBeforeDefinition,  // استخدام قبل التعريف
    
    // أخطاء عامة
    InternalError,              // خطأ داخلي
    SyntaxError                 // خطأ صياغة
};

/**
 * مستوى الخطورة
 */
enum class ErrorSeverity {
    Error,      // خطأ - يوقف الترجمة
    Warning,    // تحذير - لا يوقف الترجمة
    Note,       // ملاحظة - معلومات إضافية
    Help        // مساعدة - اقتراح حل
};

// =============================================================================
// هيكل الخطأ
// =============================================================================

/**
 * موقع في الكود
 */
struct SourceLocation {
    std::string filePath;
    int line;
    int column;
    int length;
    std::string sourceLine;
    
    SourceLocation() : line(0), column(0), length(0) {}
    
    SourceLocation(const std::string& file, int l, int c, int len = 1)
        : filePath(file), line(l), column(c), length(len) {}
};

/**
 * سلسلة توسيع الماكرو
 */
struct ExpansionChain {
    std::string macroName;
    SourceLocation callSite;
    SourceLocation definitionSite;
    std::optional<ExpansionChain*> parent;
};

/**
 * خطأ ماكرو
 */
struct MacroError {
    MacroErrorKind kind;
    ErrorSeverity severity;
    std::string message;
    SourceLocation location;
    std::vector<std::pair<ErrorSeverity, std::string>> notes;
    std::optional<std::string> suggestion;
    std::vector<ExpansionChain> expansionStack;
    
    MacroError(MacroErrorKind k = MacroErrorKind::InternalError)
        : kind(k), severity(ErrorSeverity::Error) {}
};

// =============================================================================
// منشئ الأخطاء
// =============================================================================

/**
 * منشئ أخطاء الماكرو
 */
class MacroErrorBuilder {
public:
    MacroErrorBuilder& error(MacroErrorKind kind) {
        error_.kind = kind;
        error_.severity = ErrorSeverity::Error;
        return *this;
    }
    
    MacroErrorBuilder& warning(MacroErrorKind kind) {
        error_.kind = kind;
        error_.severity = ErrorSeverity::Warning;
        return *this;
    }
    
    MacroErrorBuilder& message(const std::string& msg) {
        error_.message = msg;
        return *this;
    }
    
    MacroErrorBuilder& at(const SourceLocation& loc) {
        error_.location = loc;
        return *this;
    }
    
    MacroErrorBuilder& at(const std::string& file, int line, int col) {
        error_.location = SourceLocation(file, line, col);
        return *this;
    }
    
    MacroErrorBuilder& note(const std::string& msg) {
        error_.notes.push_back({ErrorSeverity::Note, msg});
        return *this;
    }
    
    MacroErrorBuilder& help(const std::string& msg) {
        error_.notes.push_back({ErrorSeverity::Help, msg});
        return *this;
    }
    
    MacroErrorBuilder& suggestion(const std::string& sug) {
        error_.suggestion = sug;
        return *this;
    }
    
    MacroErrorBuilder& inMacro(const std::string& name, 
                               const SourceLocation& callSite,
                               const SourceLocation& defSite) {
        ExpansionChain chain;
        chain.macroName = name;
        chain.callSite = callSite;
        chain.definitionSite = defSite;
        error_.expansionStack.push_back(chain);
        return *this;
    }
    
    MacroError build() {
        return error_;
    }
    
private:
    MacroError error_;
};

// =============================================================================
// منسق الأخطاء
// =============================================================================

/**
 * منسق رسائل الخطأ للعرض
 */
class MacroErrorFormatter {
public:
    /**
     * تنسيق خطأ للطرفية
     */
    std::string format(const MacroError& error) {
        std::ostringstream ss;
        
        // العنوان
        ss << formatSeverity(error.severity) << ": " << error.message << "\n";
        
        // الموقع
        if (error.location.line > 0) {
            ss << formatLocation(error.location) << "\n";
            
            // عرض السطر إن وُجد
            if (!error.location.sourceLine.empty()) {
                ss << formatSourceLine(error.location);
            }
        }
        
        // سلسلة التوسيع
        if (!error.expansionStack.empty()) {
            ss << "\n";
            for (size_t i = 0; i < error.expansionStack.size(); i++) {
                const auto& chain = error.expansionStack[i];
                ss << std::string(i * 2, ' ');
                ss << "في توسيع الماكرو `" << chain.macroName << "!`\n";
                
                if (chain.callSite.line > 0) {
                    ss << std::string(i * 2 + 2, ' ');
                    ss << "استُدعي في " << formatLocation(chain.callSite) << "\n";
                }
            }
        }
        
        // الملاحظات
        for (const auto& [sev, msg] : error.notes) {
            ss << formatSeverity(sev) << ": " << msg << "\n";
        }
        
        // الاقتراح
        if (error.suggestion) {
            ss << "\n💡 اقتراح: " << *error.suggestion << "\n";
        }
        
        return ss.str();
    }
    
    /**
     * تنسيق للـ JSON
     */
    std::string formatJson(const MacroError& error) {
        std::ostringstream ss;
        
        ss << "{\n";
        ss << "  \"kind\": \"" << errorKindName(error.kind) << "\",\n";
        ss << "  \"severity\": \"" << severityName(error.severity) << "\",\n";
        ss << "  \"message\": \"" << escapeJson(error.message) << "\",\n";
        
        ss << "  \"location\": {\n";
        ss << "    \"file\": \"" << escapeJson(error.location.filePath) << "\",\n";
        ss << "    \"line\": " << error.location.line << ",\n";
        ss << "    \"column\": " << error.location.column << "\n";
        ss << "  }";
        
        if (!error.notes.empty()) {
            ss << ",\n  \"notes\": [\n";
            for (size_t i = 0; i < error.notes.size(); i++) {
                if (i > 0) ss << ",\n";
                ss << "    {\"severity\": \"" << severityName(error.notes[i].first) 
                   << "\", \"message\": \"" << escapeJson(error.notes[i].second) << "\"}";
            }
            ss << "\n  ]";
        }
        
        ss << "\n}\n";
        
        return ss.str();
    }
    
private:
    std::string formatSeverity(ErrorSeverity sev) {
        switch (sev) {
            case ErrorSeverity::Error:   return "❌ خطأ";
            case ErrorSeverity::Warning: return "⚠️  تحذير";
            case ErrorSeverity::Note:    return "ℹ️  ملاحظة";
            case ErrorSeverity::Help:    return "💡 مساعدة";
            default: return "؟";
        }
    }
    
    std::string severityName(ErrorSeverity sev) {
        switch (sev) {
            case ErrorSeverity::Error:   return "error";
            case ErrorSeverity::Warning: return "warning";
            case ErrorSeverity::Note:    return "note";
            case ErrorSeverity::Help:    return "help";
            default: return "unknown";
        }
    }
    
    std::string errorKindName(MacroErrorKind kind) {
        static const std::map<MacroErrorKind, std::string> names = {
            {MacroErrorKind::InvalidMacroName, "invalid_macro_name"},
            {MacroErrorKind::InvalidPatternSyntax, "invalid_pattern_syntax"},
            {MacroErrorKind::UndefinedMacro, "undefined_macro"},
            {MacroErrorKind::ArgumentCountMismatch, "argument_count_mismatch"},
            {MacroErrorKind::RecursionLimitExceeded, "recursion_limit_exceeded"},
            {MacroErrorKind::UndefinedMacroVar, "undefined_macro_var"},
            {MacroErrorKind::HygieneViolation, "hygiene_violation"},
            {MacroErrorKind::MacroNotVisible, "macro_not_visible"},
            {MacroErrorKind::SyntaxError, "syntax_error"},
        };
        auto it = names.find(kind);
        return it != names.end() ? it->second : "unknown";
    }
    
    std::string formatLocation(const SourceLocation& loc) {
        std::ostringstream ss;
        ss << "  --> " << loc.filePath << ":" << loc.line << ":" << loc.column;
        return ss.str();
    }
    
    std::string formatSourceLine(const SourceLocation& loc) {
        std::ostringstream ss;
        
        // رقم السطر
        std::string lineNum = std::to_string(loc.line);
        ss << std::string(3 - lineNum.length(), ' ') << lineNum << " | ";
        ss << loc.sourceLine << "\n";
        
        // السهم
        ss << std::string(3, ' ') << " | ";
        ss << std::string(loc.column > 0 ? loc.column - 1 : 0, ' ');
        ss << "^" << std::string(loc.length > 1 ? loc.length - 1 : 0, '~') << "\n";
        
        return ss.str();
    }
    
    std::string escapeJson(const std::string& s) {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c;
            }
        }
        return result;
    }
};

// =============================================================================
// مصنع الأخطاء الشائعة
// =============================================================================

/**
 * مصنع أخطاء ماكرو جاهزة
 */
class MacroErrorFactory {
public:
    static MacroError undefinedMacro(const std::string& name, 
                                     const SourceLocation& loc) {
        return MacroErrorBuilder()
            .error(MacroErrorKind::UndefinedMacro)
            .message("ماكرو غير معرّف: `" + name + "!`")
            .at(loc)
            .help("تأكد من تعريف الماكرو قبل استخدامه أو استيراده من وحدة أخرى")
            .suggestion("ماكرو! " + name + "!($x:عبر) { /* التنفيذ */ }")
            .build();
    }
    
    static MacroError argumentCountMismatch(const std::string& macroName,
                                            int expected, int got,
                                            const SourceLocation& loc) {
        return MacroErrorBuilder()
            .error(MacroErrorKind::ArgumentCountMismatch)
            .message("عدد المعاملات غير متطابق في `" + macroName + "!`")
            .at(loc)
            .note("متوقع " + std::to_string(expected) + " معامل، وُجد " + std::to_string(got))
            .build();
    }
    
    static MacroError undefinedMacroVar(const std::string& varName,
                                        const std::string& macroName,
                                        const SourceLocation& loc) {
        return MacroErrorBuilder()
            .error(MacroErrorKind::UndefinedMacroVar)
            .message("متغير ماكرو غير معرّف: `$" + varName + "`")
            .at(loc)
            .note("في جسم الماكرو `" + macroName + "!`")
            .help("تأكد من أن المتغير معرّف في نمط الماكرو")
            .build();
    }
    
    static MacroError recursionLimitExceeded(const std::string& macroName,
                                             int limit,
                                             const SourceLocation& loc) {
        return MacroErrorBuilder()
            .error(MacroErrorKind::RecursionLimitExceeded)
            .message("تجاوز حد توسيع الماكرو (" + std::to_string(limit) + ")")
            .at(loc)
            .note("الماكرو `" + macroName + "!` قد يحتوي على تكرار لانهائي")
            .help("تحقق من شروط التوقف في الماكرو")
            .build();
    }
    
    static MacroError invalidPatternSyntax(const std::string& pattern,
                                           const std::string& reason,
                                           const SourceLocation& loc) {
        return MacroErrorBuilder()
            .error(MacroErrorKind::InvalidPatternSyntax)
            .message("صيغة نمط ماكرو غير صالحة")
            .at(loc)
            .note(reason)
            .help("النمط الصحيح: $اسم:نوع (مثل $x:عبر)")
            .build();
    }
    
    static MacroError hygieneViolation(const std::string& varName,
                                       const SourceLocation& macroLoc,
                                       const SourceLocation& outerLoc) {
        return MacroErrorBuilder()
            .error(MacroErrorKind::HygieneViolation)
            .message("انتهاك نظافة الماكرو: `" + varName + "`")
            .at(macroLoc)
            .note("المتغير `" + varName + "` معرّف خارج الماكرو")
            .help("استخدم اسماً مختلفاً داخل الماكرو أو مرر المتغير كمعامل")
            .build();
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

static MacroErrorFormatter g_formatter;
static std::vector<MacroError> g_errors;

extern "C" {

/**
 * إضافة خطأ
 */
void sad_macro_error(int kind, const char* message, const char* file, 
                     int line, int col) {
    MacroError error;
    error.kind = static_cast<MacroErrorKind>(kind);
    error.severity = ErrorSeverity::Error;
    error.message = message;
    error.location = SourceLocation(file, line, col);
    
    g_errors.push_back(error);
}

/**
 * عدد الأخطاء
 */
int sad_macro_error_count() {
    return static_cast<int>(g_errors.size());
}

/**
 * طباعة جميع الأخطاء
 */
void sad_macro_print_errors() {
    for (const auto& error : g_errors) {
        std::cerr << g_formatter.format(error) << "\n";
    }
}

/**
 * مسح الأخطاء
 */
void sad_macro_clear_errors() {
    g_errors.clear();
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef MACRO_ERRORS_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار رسائل خطأ الماكرو                                                  ║
║     Macro Error Diagnostics Test                                             ║
║                                                                              ║
║     المرحلة 19 - T199                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    MacroErrorFormatter formatter;
    
    // خطأ 1: ماكرو غير معرّف
    std::cout << "═══ خطأ: ماكرو غير معرّف ═══\n\n";
    
    SourceLocation loc1("main.ص", 15, 5, 10);
    loc1.sourceLine = "    سجّل!(\"مرحبا\")";
    
    auto error1 = MacroErrorFactory::undefinedMacro("سجّل", loc1);
    std::cout << formatter.format(error1) << "\n";
    
    // خطأ 2: عدد المعاملات
    std::cout << "═══ خطأ: عدد المعاملات ═══\n\n";
    
    SourceLocation loc2("app.ص", 23, 1, 20);
    loc2.sourceLine = "حدّ_أقصى!(1, 2, 3)";
    
    auto error2 = MacroErrorFactory::argumentCountMismatch("حدّ_أقصى", 2, 3, loc2);
    std::cout << formatter.format(error2) << "\n";
    
    // خطأ 3: متغير غير معرّف
    std::cout << "═══ خطأ: متغير ماكرو غير معرّف ═══\n\n";
    
    SourceLocation loc3("macros.ص", 8, 10);
    loc3.sourceLine = "    اطبع($ص)";
    
    auto error3 = MacroErrorFactory::undefinedMacroVar("ص", "طباعة", loc3);
    std::cout << formatter.format(error3) << "\n";
    
    // خطأ 4: تجاوز حد التكرار
    std::cout << "═══ خطأ: تجاوز حد التكرار ═══\n\n";
    
    SourceLocation loc4("recursive.ص", 1, 1);
    auto error4 = MacroErrorFactory::recursionLimitExceeded("تكرار", 256, loc4);
    std::cout << formatter.format(error4) << "\n";
    
    // خطأ 5: مع سلسلة توسيع
    std::cout << "═══ خطأ: مع سلسلة توسيع ═══\n\n";
    
    SourceLocation callLoc("main.ص", 50, 3);
    SourceLocation defLoc("macros.ص", 10, 1);
    SourceLocation innerLoc("macros.ص", 12, 8);
    innerLoc.sourceLine = "        $غير_موجود";
    
    auto error5 = MacroErrorBuilder()
        .error(MacroErrorKind::UndefinedMacroVar)
        .message("متغير ماكرو غير معرّف: `$غير_موجود`")
        .at(innerLoc)
        .inMacro("خارجي", callLoc, defLoc)
        .inMacro("داخلي", innerLoc, innerLoc)
        .help("تأكد من تعريف المتغير في نمط الماكرو")
        .build();
    
    std::cout << formatter.format(error5) << "\n";
    
    // عرض JSON
    std::cout << "═══ تنسيق JSON ═══\n\n";
    std::cout << formatter.formatJson(error1);
    
    std::cout << "\n✅ نجح اختبار رسائل خطأ الماكرو!\n";
    
    return 0;
}

#endif // MACRO_ERRORS_TEST
