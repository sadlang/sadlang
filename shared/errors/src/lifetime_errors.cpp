/**
 * =============================================================================
 * ملف: lifetime_errors.cpp
 * الوصف: رسائل خطأ العمر
 * المهمة: T207 - Implement lifetime error messages
 * المرحلة: Phase 20 - User Story 17 (Lifetime Annotations)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * لماذا رسائل خطأ واضحة؟
 * ───────────────────────
 * أخطاء العمر من أصعب الأخطاء على المبرمجين الجدد.
 * لذلك نحتاج رسائل:
 * 
 * 1. واضحة: تشرح المشكلة بالعربي
 * 2. مفيدة: تقترح حلولاً
 * 3. تعليمية: تشرح المفاهيم
 * 
 * أنواع أخطاء العمر:
 * ──────────────────
 * 
 * 1. مرجع متدلي (Dangling Reference):
 *    ❌ خطأ: المرجع يشير إلى قيمة لم تعد موجودة
 *    💡 تلميح: لا يمكن إرجاع مرجع لمتغير محلي
 * 
 * 2. عمر قصير جداً:
 *    ❌ خطأ: العمر 'أ قصير جداً
 *    💡 تلميح: المرجع المُرجع يجب أن يعيش أطول من المدخل
 * 
 * 3. عمر غير معرّف:
 *    ❌ خطأ: العمر 'ب غير معرّف
 *    💡 تلميح: أضف 'ب إلى قائمة المعاملات العامة
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include <iostream>
#include <optional>

namespace sad::errors::lifetime {

// =============================================================================
// أنواع الأخطاء
// =============================================================================

/**
 * نوع خطأ العمر
 */
enum class LifetimeErrorKind {
    // أخطاء المرجع
    DanglingReference,      // مرجع متدلي
    UseAfterFree,          // استخدام بعد التحرير
    ReturnLocalRef,        // إرجاع مرجع محلي
    
    // أخطاء العمر
    LifetimeTooShort,      // عمر قصير جداً
    LifetimeNotLongEnough, // عمر لا يعيش طويلاً بما يكفي
    LifetimeMismatch,      // عدم تطابق الأعمار
    
    // أخطاء التعريف
    UndefinedLifetime,     // عمر غير معرّف
    DuplicateLifetime,     // عمر مكرر
    UnusedLifetime,        // عمر غير مستخدم
    
    // أخطاء الاستعارة
    BorrowConflict,        // تعارض استعارة
    MutBorrowWhileBorrowed, // استعارة قابلة للتغيير أثناء استعارة
    MoveWhileBorrowed,     // نقل أثناء استعارة
    
    // أخطاء النطاق
    ScopeEscape,           // هروب من النطاق
    InvalidScope,          // نطاق غير صالح
    
    // أخطاء القيود
    ConstraintViolation,   // انتهاك قيد
    CircularLifetime,      // عمر دائري
    
    // تحذيرات
    ElisionAmbiguity,      // غموض في الحذف
    StaticNotNeeded,       // 'ثابت غير ضروري
};

/**
 * شدة الخطأ
 */
enum class Severity {
    Error,      // خطأ (يمنع الترجمة)
    Warning,    // تحذير
    Note,       // ملاحظة
    Help,       // مساعدة
};

// =============================================================================
// موقع الخطأ
// =============================================================================

/**
 * موقع في الكود
 */
struct SourceLocation {
    std::string file;
    size_t line;
    size_t column;
    size_t endLine;
    size_t endColumn;
    
    std::string toString() const {
        std::ostringstream ss;
        if (!file.empty()) ss << file << ":";
        ss << line << ":" << column;
        return ss.str();
    }
};

/**
 * مقتطف من الكود
 */
struct CodeSnippet {
    std::string line;
    size_t lineNumber;
    size_t highlightStart;
    size_t highlightEnd;
    std::string annotation;
};

// =============================================================================
// رسالة الخطأ
// =============================================================================

/**
 * رسالة خطأ عمر كاملة
 */
struct LifetimeError {
    LifetimeErrorKind kind;
    Severity severity;
    std::string code;           // رمز الخطأ (مثل E0106)
    std::string message;        // الرسالة الرئيسية
    SourceLocation location;
    
    std::vector<CodeSnippet> snippets;
    std::vector<std::string> notes;
    std::vector<std::string> helps;
    std::string suggestion;     // اقتراح تصحيح
};

// =============================================================================
// منشئ رسائل الخطأ
// =============================================================================

/**
 * منشئ رسائل خطأ العمر
 * يوفر رسائل واضحة ومفيدة بالعربية
 */
class LifetimeErrorBuilder {
public:
    /**
     * بناء خطأ مرجع متدلي
     */
    LifetimeError danglingReference(const std::string& refExpr,
                                    const std::string& sourceVar,
                                    const SourceLocation& loc) {
        LifetimeError err;
        err.kind = LifetimeErrorKind::DanglingReference;
        err.severity = Severity::Error;
        err.code = "E0106";
        err.message = "مرجع متدلي: `" + refExpr + "` يشير إلى قيمة لم تعد موجودة";
        err.location = loc;
        
        err.notes.push_back(
            "المتغير `" + sourceVar + "` يُدمر في نهاية النطاق"
        );
        
        err.helps.push_back(
            "المراجع يجب أن تشير إلى قيم تعيش أطول منها"
        );
        
        err.suggestion = "حاول نقل ملكية القيمة بدلاً من استعارتها";
        
        return err;
    }
    
    /**
     * بناء خطأ إرجاع مرجع محلي
     */
    LifetimeError returnLocalReference(const std::string& varName,
                                       const SourceLocation& loc) {
        LifetimeError err;
        err.kind = LifetimeErrorKind::ReturnLocalRef;
        err.severity = Severity::Error;
        err.code = "E0515";
        err.message = "لا يمكن إرجاع مرجع لمتغير محلي `" + varName + "`";
        err.location = loc;
        
        err.notes.push_back(
            "المتغيرات المحلية تُدمر عند نهاية الدالة"
        );
        err.notes.push_back(
            "المرجع المُرجع سيشير إلى ذاكرة غير صالحة"
        );
        
        err.helps.push_back(
            "أرجع القيمة نفسها بدلاً من مرجع لها"
        );
        err.helps.push_back(
            "أو استخدم `'ثابت` إذا كانت القيمة ثابتة"
        );
        
        return err;
    }
    
    /**
     * بناء خطأ عمر قصير
     */
    LifetimeError lifetimeTooShort(const std::string& lifetime,
                                   const std::string& required,
                                   const SourceLocation& loc) {
        LifetimeError err;
        err.kind = LifetimeErrorKind::LifetimeTooShort;
        err.severity = Severity::Error;
        err.code = "E0312";
        err.message = "العمر `" + lifetime + "` قصير جداً";
        err.location = loc;
        
        err.notes.push_back(
            "العمر `" + lifetime + "` يجب أن يعيش على الأقل بقدر `" + required + "`"
        );
        
        err.helps.push_back(
            "تأكد من أن المرجع يأتي من مصدر يعيش طويلاً بما يكفي"
        );
        
        return err;
    }
    
    /**
     * بناء خطأ عمر غير معرّف
     */
    LifetimeError undefinedLifetime(const std::string& lifetime,
                                    const SourceLocation& loc) {
        LifetimeError err;
        err.kind = LifetimeErrorKind::UndefinedLifetime;
        err.severity = Severity::Error;
        err.code = "E0261";
        err.message = "العمر `" + lifetime + "` غير معرّف";
        err.location = loc;
        
        err.helps.push_back(
            "أضف `" + lifetime + "` إلى قائمة المعاملات العامة"
        );
        
        err.suggestion = "دالة foo<" + lifetime + ">(...) { ... }";
        
        return err;
    }
    
    /**
     * بناء خطأ تعارض استعارة
     */
    LifetimeError borrowConflict(const std::string& varName,
                                 const std::string& existingBorrow,
                                 const std::string& newBorrow,
                                 const SourceLocation& loc) {
        LifetimeError err;
        err.kind = LifetimeErrorKind::BorrowConflict;
        err.severity = Severity::Error;
        err.code = "E0502";
        err.message = "لا يمكن استعارة `" + varName + "` كـ " + newBorrow +
                     " لأنها مستعارة بالفعل كـ " + existingBorrow;
        err.location = loc;
        
        err.notes.push_back(
            "الاستعارة " + existingBorrow + " تستمر حتى نهاية النطاق"
        );
        
        err.helps.push_back(
            "انقل الاستعارة الجديدة إلى نطاق مختلف"
        );
        err.helps.push_back(
            "أو انتظر حتى تنتهي الاستعارة الأولى"
        );
        
        return err;
    }
    
    /**
     * بناء خطأ هروب من النطاق
     */
    LifetimeError scopeEscape(const std::string& refName,
                             const std::string& scopeName,
                             const SourceLocation& loc) {
        LifetimeError err;
        err.kind = LifetimeErrorKind::ScopeEscape;
        err.severity = Severity::Error;
        err.code = "E0373";
        err.message = "المرجع `" + refName + "` يهرب من النطاق `" + scopeName + "`";
        err.location = loc;
        
        err.notes.push_back(
            "المرجع لا يمكن أن يعيش أطول من القيمة التي يشير إليها"
        );
        
        return err;
    }
    
    /**
     * بناء تحذير غموض الحذف
     */
    LifetimeError elisionAmbiguity(const std::string& context,
                                   const SourceLocation& loc) {
        LifetimeError err;
        err.kind = LifetimeErrorKind::ElisionAmbiguity;
        err.severity = Severity::Warning;
        err.code = "W0001";
        err.message = "غموض في استنتاج العمر في `" + context + "`";
        err.location = loc;
        
        err.helps.push_back(
            "أضف تعليقات العمر صراحة لتوضيح نيتك"
        );
        
        return err;
    }
};

// =============================================================================
// منسّق الأخطاء
// =============================================================================

/**
 * منسّق رسائل الخطأ
 * يُنتج رسائل ملونة وواضحة
 */
class LifetimeErrorFormatter {
public:
    /**
     * تنسيق خطأ للطرفية
     */
    std::string formatForTerminal(const LifetimeError& err, 
                                  const std::string& sourceLine = "") {
        std::ostringstream ss;
        
        // العنوان
        ss << severityColor(err.severity);
        ss << severityLabel(err.severity) << "[" << err.code << "]: ";
        ss << err.message << resetColor() << "\n";
        
        // الموقع
        ss << "  --> " << err.location.toString() << "\n";
        
        // مقتطف الكود (إذا توفر)
        if (!sourceLine.empty()) {
            ss << "   |\n";
            ss << " " << err.location.line << " | " << sourceLine << "\n";
            ss << "   | " << std::string(err.location.column - 1, ' ') 
               << "^" << underlineColor() << "~~~" << resetColor() << "\n";
        }
        
        // الملاحظات
        for (const auto& note : err.notes) {
            ss << noteColor() << "ملاحظة: " << resetColor() << note << "\n";
        }
        
        // المساعدة
        for (const auto& help : err.helps) {
            ss << helpColor() << "مساعدة: " << resetColor() << help << "\n";
        }
        
        // الاقتراح
        if (!err.suggestion.empty()) {
            ss << suggestionColor() << "اقتراح: " << resetColor() 
               << err.suggestion << "\n";
        }
        
        return ss.str();
    }
    
    /**
     * تنسيق خطأ كـ JSON
     */
    std::string formatAsJson(const LifetimeError& err) {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"code\": \"" << err.code << "\",\n";
        ss << "  \"severity\": \"" << severityName(err.severity) << "\",\n";
        ss << "  \"message\": \"" << escapeJson(err.message) << "\",\n";
        ss << "  \"location\": {\n";
        ss << "    \"file\": \"" << escapeJson(err.location.file) << "\",\n";
        ss << "    \"line\": " << err.location.line << ",\n";
        ss << "    \"column\": " << err.location.column << "\n";
        ss << "  },\n";
        
        ss << "  \"notes\": [";
        for (size_t i = 0; i < err.notes.size(); i++) {
            if (i > 0) ss << ", ";
            ss << "\"" << escapeJson(err.notes[i]) << "\"";
        }
        ss << "],\n";
        
        ss << "  \"helps\": [";
        for (size_t i = 0; i < err.helps.size(); i++) {
            if (i > 0) ss << ", ";
            ss << "\"" << escapeJson(err.helps[i]) << "\"";
        }
        ss << "],\n";
        
        ss << "  \"suggestion\": \"" << escapeJson(err.suggestion) << "\"\n";
        ss << "}";
        
        return ss.str();
    }
    
private:
    // ألوان ANSI
    std::string severityColor(Severity s) const {
        switch (s) {
            case Severity::Error: return "\033[1;31m";   // أحمر غامق
            case Severity::Warning: return "\033[1;33m"; // أصفر غامق
            case Severity::Note: return "\033[1;36m";    // سماوي غامق
            case Severity::Help: return "\033[1;32m";    // أخضر غامق
        }
        return "";
    }
    
    std::string noteColor() const { return "\033[1;36m"; }
    std::string helpColor() const { return "\033[1;32m"; }
    std::string suggestionColor() const { return "\033[1;34m"; }
    std::string underlineColor() const { return "\033[4;31m"; }
    std::string resetColor() const { return "\033[0m"; }
    
    std::string severityLabel(Severity s) const {
        switch (s) {
            case Severity::Error: return "خطأ";
            case Severity::Warning: return "تحذير";
            case Severity::Note: return "ملاحظة";
            case Severity::Help: return "مساعدة";
        }
        return "";
    }
    
    std::string severityName(Severity s) const {
        switch (s) {
            case Severity::Error: return "error";
            case Severity::Warning: return "warning";
            case Severity::Note: return "note";
            case Severity::Help: return "help";
        }
        return "";
    }
    
    std::string escapeJson(const std::string& s) const {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
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
 * مصنع أخطاء العمر الشائعة
 */
class LifetimeErrorFactory {
public:
    LifetimeErrorBuilder& builder() { return builder_; }
    LifetimeErrorFormatter& formatter() { return formatter_; }
    
    /**
     * خطأ: إرجاع مرجع محلي
     */
    void reportReturnLocalRef(const std::string& varName, 
                             const SourceLocation& loc,
                             const std::string& sourceLine = "") {
        auto err = builder_.returnLocalReference(varName, loc);
        std::cout << formatter_.formatForTerminal(err, sourceLine);
    }
    
    /**
     * خطأ: عمر غير معرّف
     */
    void reportUndefinedLifetime(const std::string& lifetime,
                                const SourceLocation& loc,
                                const std::string& sourceLine = "") {
        auto err = builder_.undefinedLifetime(lifetime, loc);
        std::cout << formatter_.formatForTerminal(err, sourceLine);
    }
    
    /**
     * خطأ: تعارض استعارة
     */
    void reportBorrowConflict(const std::string& varName,
                             bool existingIsMut, bool newIsMut,
                             const SourceLocation& loc,
                             const std::string& sourceLine = "") {
        std::string existing = existingIsMut ? "قابل للتغيير" : "مشترك";
        std::string newBorrow = newIsMut ? "قابل للتغيير" : "مشترك";
        auto err = builder_.borrowConflict(varName, existing, newBorrow, loc);
        std::cout << formatter_.formatForTerminal(err, sourceLine);
    }
    
private:
    LifetimeErrorBuilder builder_;
    LifetimeErrorFormatter formatter_;
};

} // namespace sad::errors::lifetime

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::errors::lifetime;

/**
 * إنشاء مصنع أخطاء
 */
void* sad_lifetime_error_factory_new() {
    return new LifetimeErrorFactory();
}

/**
 * تحرير المصنع
 */
void sad_lifetime_error_factory_free(void* factory) {
    delete static_cast<LifetimeErrorFactory*>(factory);
}

/**
 * الإبلاغ عن خطأ إرجاع مرجع محلي
 */
void sad_report_return_local_ref(void* factory, const char* varName,
                                 const char* file, size_t line, size_t col) {
    if (!factory || !varName) return;
    auto* f = static_cast<LifetimeErrorFactory*>(factory);
    f->reportReturnLocalRef(varName, {file ? file : "", line, col, line, col});
}

/**
 * الإبلاغ عن خطأ عمر غير معرّف
 */
void sad_report_undefined_lifetime(void* factory, const char* lifetime,
                                   const char* file, size_t line, size_t col) {
    if (!factory || !lifetime) return;
    auto* f = static_cast<LifetimeErrorFactory*>(factory);
    f->reportUndefinedLifetime(lifetime, {file ? file : "", line, col, line, col});
}

/**
 * الإبلاغ عن تعارض استعارة
 */
void sad_report_borrow_conflict(void* factory, const char* varName,
                               int existingIsMut, int newIsMut,
                               const char* file, size_t line, size_t col) {
    if (!factory || !varName) return;
    auto* f = static_cast<LifetimeErrorFactory*>(factory);
    f->reportBorrowConflict(varName, existingIsMut != 0, newIsMut != 0,
                           {file ? file : "", line, col, line, col});
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef LIFETIME_ERRORS_TEST

void testReturnLocalRefError() {
    sad::errors::lifetime::LifetimeErrorFactory factory;
    
    std::cout << "\n═══ خطأ إرجاع مرجع محلي ═══\n\n";
    
    factory.reportReturnLocalRef("x", {"مثال.ص", 5, 12},
        "    أرجع &x");
}

void testUndefinedLifetimeError() {
    sad::errors::lifetime::LifetimeErrorFactory factory;
    
    std::cout << "\n═══ خطأ عمر غير معرّف ═══\n\n";
    
    factory.reportUndefinedLifetime("'ب", {"مثال.ص", 3, 20},
        "دالة foo(x: &'ب نص) -> &'ب نص");
}

void testBorrowConflictError() {
    sad::errors::lifetime::LifetimeErrorFactory factory;
    
    std::cout << "\n═══ خطأ تعارض استعارة ═══\n\n";
    
    factory.reportBorrowConflict("قائمة", false, true, {"مثال.ص", 10, 5},
        "    متغير مرجع = &متغير قائمة");
}

void testJsonFormat() {
    sad::errors::lifetime::LifetimeErrorBuilder builder;
    sad::errors::lifetime::LifetimeErrorFormatter formatter;
    
    auto err = builder.returnLocalReference("x", {"test.ص", 5, 12, 5, 14});
    
    std::cout << "\n═══ تنسيق JSON ═══\n\n";
    std::cout << formatter.formatAsJson(err) << "\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات رسائل خطأ العمر\n";
    std::cout << "═══════════════════════════════════════════════\n";
    
    testReturnLocalRefError();
    testUndefinedLifetimeError();
    testBorrowConflictError();
    testJsonFormat();
    
    std::cout << "\n✅ اكتملت اختبارات رسائل الخطأ!\n";
    return 0;
}

#endif // LIFETIME_ERRORS_TEST
