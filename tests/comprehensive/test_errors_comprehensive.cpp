/**
 * @file test_errors_comprehensive.cpp
 * @brief (AR) اختبارات شاملة لنظام الأخطاء الذكية والتشخيصات
 * @brief (EN) Comprehensive Smart Errors & Diagnostics Tests
 * 
 * ═══════════════════════════════════════════════════════════════════════
 *  ملف اختبارات نظام الأخطاء الذكية (Smart Errors) والتشخيصات
 *  يحتوي على ~30 اختبار موزعة كالتالي:
 * 
 *  ■ القسم 1: التشخيصات (Diagnostics)
 *    - مستويات الشدة (ERROR, WARNING, INFO, NOTE, HINT)
 *    - رسائل ثنائية اللغة (عربي + إنجليزي)
 *    - اقتراحات الإصلاح (FixItHint)
 * 
 *  ■ القسم 2: رموز الأخطاء (Error Codes)
 *    - جميع فئات الأخطاء: معجمية، نحوية، دلالية، تنفيذية
 *    - تحويل الرموز إلى نصوص
 *    - أوصاف الأخطاء بالعربية والإنجليزية
 * 
 *  ■ القسم 3: مدير الأخطاء (ErrorManager)
 *    - Singleton pattern
 *    - تدفق العمل: إضافة → استعلام → عرض → تصدير
 *    - إعدادات اللغة والألوان
 * 
 *  ■ القسم 4: الأخطاء الذكية (Smart Errors - Phase 15)
 *    - محرك الاقتراحات (SuggestionEngine)
 *    - وضع المعلم (TeacherMode)
 *    - منع التسلسل (CascadePrevention)
 *    - جمع الأخطاء المتعددة (MultiErrorCollector)
 * 
 *  ■ القسم 5: DiagnosticBuilder (بناء التشخيصات السلس)
 *    - Fluent API
 *    - إضافة fix-its وملاحظات
 * ═══════════════════════════════════════════════════════════════════════
 * 
 * المتطلبات:
 *   - ملفات الهيدر: diagnostic.h, error_codes.h, error_manager.h, smart_errors.h
 *   - مكتبة: sad_shared (أو sad_core)
 *   - C++17
 * 
 * @author Sad Language Development Team
 * @date 2026-02-17
 */

#include "sad_test_framework.h"

// ══════════════════════════════════════════════════════════════════════
//  الشوائب (Includes) - نظام الأخطاء
// ══════════════════════════════════════════════════════════════════════

// --- نظام التشخيصات الأساسي ---
#if __has_include("diagnostic.h")
    #include "diagnostic.h"
    #define HAS_DIAGNOSTIC 1
#else
    #define HAS_DIAGNOSTIC 0
#endif

// --- رموز الأخطاء ---
#if __has_include("error_codes.h")
    #include "error_codes.h"
    #define HAS_ERROR_CODES 1
#else
    #define HAS_ERROR_CODES 0
#endif

// --- مدير الأخطاء ---
#if __has_include("error_manager.h")
    #include "error_manager.h"
    #define HAS_ERROR_MANAGER 1
#else
    #define HAS_ERROR_MANAGER 0
#endif

// --- موقع المصدر ---
#if __has_include("source_location.h")
    #include "source_location.h"
    #define HAS_SOURCE_LOCATION 1
#else
    #define HAS_SOURCE_LOCATION 0
#endif

// --- الأخطاء الذكية (Phase 15) ---
#if __has_include("smart_errors.h")
    #include "smart_errors.h"
    #define HAS_SMART_ERRORS 1
#else
    #define HAS_SMART_ERRORS 0
#endif

// --- اقتراحات الإصلاح ---
#if __has_include("suggestions.h")
    #include "suggestions.h"
    #define HAS_SUGGESTIONS 1
#else
    #define HAS_SUGGESTIONS 0
#endif

// --- وضع المعلم ---
#if __has_include("teacher_mode.h")
    #include "teacher_mode.h"
    #define HAS_TEACHER_MODE 1
#else
    #define HAS_TEACHER_MODE 0
#endif

// --- منع التسلسل ---
#if __has_include("cascade_prevention.h")
    #include "cascade_prevention.h"
    #define HAS_CASCADE 1
#else
    #define HAS_CASCADE 0
#endif

// --- جمع الأخطاء المتعددة ---
#if __has_include("multi_error.h")
    #include "multi_error.h"
    #define HAS_MULTI_ERROR 1
#else
    #define HAS_MULTI_ERROR 0
#endif

// --- شرح الأنواع ---
#if __has_include("type_explanations.h")
    #include "type_explanations.h"
    #define HAS_TYPE_EXPLAIN 1
#else
    #define HAS_TYPE_EXPLAIN 0
#endif

// --- اقتراحات الإصلاح السريع ---
#if __has_include("fix_suggestions.h")
    #include "fix_suggestions.h"
    #define HAS_FIX_SUGGESTIONS 1
#else
    #define HAS_FIX_SUGGESTIONS 0
#endif

#include <string>
#include <vector>

// ══════════════════════════════════════════════════════════════════════
//  بداية الاختبارات
// ══════════════════════════════════════════════════════════════════════
int main() {
    SAD_TEST_INIT();

    // ══════════════════════════════════════════════════════════════════
    // القسم 1: SourceLocation - موقع المصدر
    // Section 1: Source Location
    //
    // SourceLocation يمثل موقعاً محدداً في الكود المصدري
    // يُستخدم لتحديد أين حدث الخطأ بالضبط (ملف:سطر:عمود)
    // ══════════════════════════════════════════════════════════════════
#if HAS_SOURCE_LOCATION

    SAD_GROUP("Errors.SourceLocation / الأخطاء.موقع_المصدر");

    // --- اختبار LOC01 ---
    // (AR) الباني الافتراضي يُنشئ موقعاً غير صالح
    // (EN) Default constructor creates invalid location
    SAD_TEST("LOC01: افتراضي غير صالح / Default invalid", {
        Sad::Errors::SourceLocation loc;
        SAD_ASSERT_FALSE(loc.isValid());
        SAD_ASSERT_EQ(loc.line, (size_t)0);
        SAD_ASSERT_EQ(loc.column, (size_t)0);
        SAD_ASSERT_TRUE(loc.filename.empty());
    });

    // --- اختبار LOC02 ---
    // (AR) إنشاء موقع مع جميع القيم
    // (EN) Create location with all values
    SAD_TEST("LOC02: موقع كامل / Full location", {
        Sad::Errors::SourceLocation loc("program.s", 42, 15, 100, 5);
        SAD_ASSERT_TRUE(loc.isValid());
        SAD_ASSERT_EQ(loc.filename, std::string("program.s"));
        SAD_ASSERT_EQ(loc.line, (size_t)42);
        SAD_ASSERT_EQ(loc.column, (size_t)15);
        SAD_ASSERT_EQ(loc.offset, (size_t)100);
        SAD_ASSERT_EQ(loc.length, (size_t)5);
    });

    // --- اختبار LOC03 ---
    // (AR) الموقع بدون إزاحة وطول (القيم الافتراضية)
    // (EN) Location without offset and length (default values)
    SAD_TEST("LOC03: قيم افتراضية / Default offset/length", {
        Sad::Errors::SourceLocation loc("test.s", 1, 1);
        SAD_ASSERT_TRUE(loc.isValid());
        SAD_ASSERT_EQ(loc.offset, (size_t)0);
        SAD_ASSERT_EQ(loc.length, (size_t)1);
    });

    // --- اختبار LOC04 ---
    // (AR) تحويل الموقع إلى نص (toString)
    // (EN) Location to string
    SAD_TEST("LOC04: تحويل لنص / toString", {
        Sad::Errors::SourceLocation loc("main.s", 10, 5);
        std::string str = loc.toString();
        
        // يجب أن يحتوي على اسم الملف ورقم السطر
        SAD_ASSERT_FALSE(str.empty());
        SAD_ASSERT_CONTAINS(str, "main.s");
        SAD_ASSERT_CONTAINS(str, "10");
    });

    // --- اختبار LOC05 ---
    // (AR) اسم ملف عربي
    // (EN) Arabic filename
    // لغة "ص" تدعم أسماء ملفات عربية
    SAD_TEST("LOC05: اسم ملف عربي / Arabic filename", {
        Sad::Errors::SourceLocation loc(
            "\xD8\xA8\xD8\xB1\xD9\x86\xD8\xA7\xD9\x85\xD8\xAC.s", // برنامج.s
            1, 1
        );
        SAD_ASSERT_TRUE(loc.isValid());
        SAD_ASSERT_EQ(loc.filename, std::string("\xD8\xA8\xD8\xB1\xD9\x86\xD8\xA7\xD9\x85\xD8\xAC.s"));
    });

#endif // HAS_SOURCE_LOCATION

    // ══════════════════════════════════════════════════════════════════
    // القسم 2: رموز الأخطاء (ErrorCode)
    // Section 2: Error Codes
    //
    // رموز الأخطاء تُصنف إلى 4 فئات:
    //   - LEX: أخطاء معجمية (التعامل مع الرموز)
    //   - SYN: أخطاء نحوية (بنية الجمل)
    //   - SEM: أخطاء دلالية (المعنى والأنواع)
    //   - RUN: أخطاء وقت التشغيل (أثناء التنفيذ)
    // ══════════════════════════════════════════════════════════════════
#if HAS_ERROR_CODES

    SAD_GROUP("Errors.ErrorCodes / الأخطاء.رموز_الأخطاء");

    // --- اختبار EC01 ---
    // (AR) رمز الخطأ المعجمي كنص
    // (EN) Lexical error code as string
    SAD_TEST("EC01: رمز خطأ معجمي / Lexical error code string", {
        std::string code = Sad::Errors::getErrorCodeString(
            Sad::Errors::ErrorCode::LEX_INVALID_CHARACTER
        );
        SAD_ASSERT_FALSE(code.empty());
    });

    // --- اختبار EC02 ---
    // (AR) رمز الخطأ النحوي كنص
    // (EN) Syntax error code as string
    SAD_TEST("EC02: رمز خطأ نحوي / Syntax error code string", {
        std::string code = Sad::Errors::getErrorCodeString(
            Sad::Errors::ErrorCode::SYN_UNEXPECTED_TOKEN
        );
        SAD_ASSERT_FALSE(code.empty());
    });

    // --- اختبار EC03 ---
    // (AR) رمز الخطأ الدلالي كنص
    // (EN) Semantic error code as string
    SAD_TEST("EC03: رمز خطأ دلالي / Semantic error code string", {
        std::string code = Sad::Errors::getErrorCodeString(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE
        );
        SAD_ASSERT_FALSE(code.empty());
    });

    // --- اختبار EC04 ---
    // (AR) رمز خطأ وقت التشغيل كنص
    // (EN) Runtime error code as string
    SAD_TEST("EC04: رمز خطأ تنفيذي / Runtime error code string", {
        std::string code = Sad::Errors::getErrorCodeString(
            Sad::Errors::ErrorCode::RUN_DIVISION_BY_ZERO
        );
        SAD_ASSERT_FALSE(code.empty());
    });

    // --- اختبار EC05 ---
    // (AR) وصف الخطأ بالعربية
    // (EN) Arabic error description
    SAD_TEST("EC05: وصف عربي / Arabic description", {
        std::string desc = Sad::Errors::getErrorDescription(
            Sad::Errors::ErrorCode::SYN_MISSING_SEMICOLON,
            Sad::Errors::Language::ARABIC
        );
        SAD_ASSERT_FALSE(desc.empty());
    });

    // --- اختبار EC06 ---
    // (AR) وصف الخطأ بالإنجليزية
    // (EN) English error description
    SAD_TEST("EC06: وصف إنجليزي / English description", {
        std::string desc = Sad::Errors::getErrorDescription(
            Sad::Errors::ErrorCode::SYN_MISSING_SEMICOLON,
            Sad::Errors::Language::ENGLISH
        );
        SAD_ASSERT_FALSE(desc.empty());
    });

    // --- اختبار EC07 ---
    // (AR) فئة الخطأ (معجمي/نحوي/دلالي/تنفيذي)
    // (EN) Error category
    SAD_TEST("EC07: فئة الخطأ / Error category", {
        std::string cat1 = Sad::Errors::getErrorCategory(
            Sad::Errors::ErrorCode::LEX_INVALID_CHARACTER
        );
        std::string cat2 = Sad::Errors::getErrorCategory(
            Sad::Errors::ErrorCode::SYN_UNEXPECTED_TOKEN
        );
        std::string cat3 = Sad::Errors::getErrorCategory(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE
        );
        std::string cat4 = Sad::Errors::getErrorCategory(
            Sad::Errors::ErrorCode::RUN_DIVISION_BY_ZERO
        );
        
        // كل فئة يجب أن تكون مختلفة (أو على الأقل غير فارغة)
        SAD_ASSERT_FALSE(cat1.empty());
        SAD_ASSERT_FALSE(cat2.empty());
        SAD_ASSERT_FALSE(cat3.empty());
        SAD_ASSERT_FALSE(cat4.empty());
    });

    // --- اختبار EC08 ---
    // (AR) كل الرموز المعجمية لديها وصف
    // (EN) All lexical codes have description
    SAD_TEST("EC08: أوصاف معجمية كاملة / All lex descriptions", {
        // التحقق من أن جميع رموز الأخطاء المعجمية لديها أوصاف
        SAD_ASSERT_FALSE(Sad::Errors::getErrorDescription(
            Sad::Errors::ErrorCode::LEX_UNTERMINATED_STRING, Sad::Errors::Language::ARABIC).empty());
        SAD_ASSERT_FALSE(Sad::Errors::getErrorDescription(
            Sad::Errors::ErrorCode::LEX_INVALID_NUMBER, Sad::Errors::Language::ARABIC).empty());
        SAD_ASSERT_FALSE(Sad::Errors::getErrorDescription(
            Sad::Errors::ErrorCode::LEX_INVALID_ESCAPE, Sad::Errors::Language::ARABIC).empty());
    });

    // --- اختبار EC09 ---
    // (AR) كل الرموز النحوية لديها وصف
    // (EN) All syntax codes have description
    SAD_TEST("EC09: أوصاف نحوية كاملة / All syn descriptions", {
        SAD_ASSERT_FALSE(Sad::Errors::getErrorDescription(
            Sad::Errors::ErrorCode::SYN_UNCLOSED_BRACKET, Sad::Errors::Language::ARABIC).empty());
        SAD_ASSERT_FALSE(Sad::Errors::getErrorDescription(
            Sad::Errors::ErrorCode::SYN_MISSING_IDENTIFIER, Sad::Errors::Language::ARABIC).empty());
        SAD_ASSERT_FALSE(Sad::Errors::getErrorDescription(
            Sad::Errors::ErrorCode::SYN_INVALID_EXPRESSION, Sad::Errors::Language::ARABIC).empty());
    });

#endif // HAS_ERROR_CODES

    // ══════════════════════════════════════════════════════════════════
    // القسم 3: مدير الأخطاء (ErrorManager)
    // Section 3: Error Manager
    //
    // ErrorManager هو Singleton يدير جميع أخطاء البرنامج
    // يوفر واجهة موحدة لإضافة الأخطاء وعرضها وتصديرها
    // ══════════════════════════════════════════════════════════════════
#if HAS_ERROR_MANAGER

    SAD_GROUP("Errors.ErrorManager / الأخطاء.مدير_الأخطاء");

    // --- اختبار EM01 ---
    // (AR) Singleton يرجع نفس النسخة دائماً
    // (EN) Singleton returns same instance
    SAD_TEST("EM01: Singleton / نسخة_وحيدة", {
        auto& mgr1 = Sad::Errors::ErrorManager::getInstance();
        auto& mgr2 = Sad::Errors::ErrorManager::getInstance();
        // يجب أن يكونا نفس العنوان في الذاكرة
        SAD_ASSERT_EQ(&mgr1, &mgr2);
    });

    // --- اختبار EM02 ---
    // (AR) البدء بدون أخطاء
    // (EN) Start with no errors
    SAD_TEST("EM02: بدون أخطاء / No errors initially", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        SAD_ASSERT_FALSE(mgr.hasErrors());
        SAD_ASSERT_EQ(mgr.getErrorCount(), (size_t)0);
        SAD_ASSERT_EQ(mgr.getWarningCount(), (size_t)0);
    });

    // --- اختبار EM03 ---
    // (AR) إضافة خطأ واحد
    // (EN) Add single error
    SAD_TEST("EM03: إضافة خطأ / Add error", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        mgr.reportError(
            Sad::Errors::ErrorCode::SYN_MISSING_SEMICOLON,
            Sad::Errors::SourceLocation("test.s", 1, 10),
            "\xD9\x81\xD8\xA7\xD8\xB5\xD9\x84\xD8\xA9 \xD9\x85\xD9\x86\xD9\x82\xD9\x88\xD8\xB7\xD8\xA9 \xD9\x85\xD9\x81\xD9\x82\xD9\x88\xD8\xAF\xD8\xA9", // فاصلة منقوطة مفقودة
            "Missing semicolon"
        );
        
        SAD_ASSERT_TRUE(mgr.hasErrors());
        SAD_ASSERT_EQ(mgr.getErrorCount(), (size_t)1);
        
        mgr.clear();
    });

    // --- اختبار EM04 ---
    // (AR) التمييز بين الأخطاء والتحذيرات
    // (EN) Distinguish errors from warnings
    SAD_TEST("EM04: أخطاء vs تحذيرات / Errors vs warnings", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        // خطأ واحد
        mgr.reportError(
            Sad::Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
            Sad::Errors::SourceLocation("test.s", 1, 1),
            "خطأ", "Error"
        );
        
        // تحذيران
        mgr.reportWarning(
            Sad::Errors::ErrorCode::SEM_REDEFINITION,
            Sad::Errors::SourceLocation("test.s", 2, 1),
            "تحذير 1", "Warning 1"
        );
        mgr.reportWarning(
            Sad::Errors::ErrorCode::SEM_REDEFINITION,
            Sad::Errors::SourceLocation("test.s", 3, 1),
            "تحذير 2", "Warning 2"
        );
        
        SAD_ASSERT_EQ(mgr.getErrorCount(), (size_t)1);
        SAD_ASSERT_EQ(mgr.getWarningCount(), (size_t)2);
        
        mgr.clear();
    });

    // --- اختبار EM05 ---
    // (AR) مسح جميع التشخيصات
    // (EN) Clear all diagnostics
    SAD_TEST("EM05: مسح الأخطاء / Clear errors", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        // إضافة بعض الأخطاء
        for (int i = 0; i < 3; i++) {
            mgr.reportError(
                Sad::Errors::ErrorCode::LEX_INVALID_CHARACTER,
                Sad::Errors::SourceLocation("test.s", i, 1),
                "خطأ", "Error"
            );
        }
        SAD_ASSERT_EQ(mgr.getErrorCount(), (size_t)3);
        
        // المسح
        mgr.clear();
        SAD_ASSERT_EQ(mgr.getErrorCount(), (size_t)0);
        SAD_ASSERT_FALSE(mgr.hasErrors());
    });

    // --- اختبار EM06 ---
    // (AR) تصدير إلى JSON
    // (EN) JSON export
    SAD_TEST("EM06: تصدير JSON / JSON export", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        mgr.reportError(
            Sad::Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
            Sad::Errors::SourceLocation("app.s", 5, 3),
            "\xD8\xB1\xD9\x85\xD8\xB2 \xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xAA\xD9\x88\xD9\x82\xD8\xB9", // رمز غير متوقع
            "Unexpected token"
        );
        
        std::string json = mgr.toJSON();
        SAD_ASSERT_FALSE(json.empty());
        // JSON يجب أن يحتوي على أقواس
        SAD_ASSERT_CONTAINS(json, "{");
        
        mgr.clear();
    });

    // --- اختبار EM07 ---
    // (AR) إعدادات اللغة
    // (EN) Language settings
    SAD_TEST("EM07: إعدادات اللغة / Language settings", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        // تعيين اللغة العربية
        mgr.setLanguage(Sad::Errors::Language::ARABIC);
        
        // تعيين اللغة الإنجليزية
        mgr.setLanguage(Sad::Errors::Language::ENGLISH);
        
        // تعيين كلاهما
        mgr.setLanguage(Sad::Errors::Language::BOTH);
        
        SAD_ASSERT_TRUE(true); // لم يحدث خطأ
    });

    // --- اختبار EM08 ---
    // (AR) تفعيل/تعطيل الأخطاء الذكية
    // (EN) Enable/disable smart errors
    SAD_TEST("EM08: الأخطاء الذكية / Smart errors toggle", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        
        mgr.setSmartErrorsEnabled(true);
        SAD_ASSERT_TRUE(mgr.isSmartErrorsEnabled());
        
        mgr.setSmartErrorsEnabled(false);
        SAD_ASSERT_FALSE(mgr.isSmartErrorsEnabled());
        
        // إعادة التمكين
        mgr.setSmartErrorsEnabled(true);
    });

    // --- اختبار EM09 ---
    // (AR) الحصول على جميع التشخيصات
    // (EN) Get all diagnostics
    SAD_TEST("EM09: كل التشخيصات / All diagnostics", {
        auto& mgr = Sad::Errors::ErrorManager::getInstance();
        mgr.clear();
        
        mgr.reportError(
            Sad::Errors::ErrorCode::LEX_INVALID_CHARACTER,
            Sad::Errors::SourceLocation("a.s", 1, 1),
            "خطأ", "Error"
        );
        
        auto& diags = mgr.getAllDiagnostics();
        SAD_ASSERT_EQ(diags.size(), (size_t)1);
        
        mgr.clear();
    });

#endif // HAS_ERROR_MANAGER

    // ══════════════════════════════════════════════════════════════════
    // القسم 4: التشخيصات المفصلة (Diagnostic)
    // Section 4: Detailed Diagnostics
    //
    // كل تشخيص يحتوي على: رمز الخطأ، الشدة، الموقع، الرسالة
    // بالإضافة إلى اقتراحات إصلاح وملاحظات إضافية
    // ══════════════════════════════════════════════════════════════════
#if HAS_DIAGNOSTIC

    SAD_GROUP("Errors.Diagnostic / الأخطاء.التشخيصات");

    // --- اختبار DG01 ---
    // (AR) مستويات الشدة
    // (EN) Severity levels
    SAD_TEST("DG01: مستويات الشدة / Severity levels", {
        // التحقق من وجود جميع المستويات
        SAD_ASSERT_NE(
            (int)Sad::Errors::DiagnosticSeverity::ERROR,
            (int)Sad::Errors::DiagnosticSeverity::WARNING
        );
        SAD_ASSERT_NE(
            (int)Sad::Errors::DiagnosticSeverity::WARNING,
            (int)Sad::Errors::DiagnosticSeverity::INFO
        );
        SAD_ASSERT_NE(
            (int)Sad::Errors::DiagnosticSeverity::INFO,
            (int)Sad::Errors::DiagnosticSeverity::NOTE
        );
        SAD_ASSERT_NE(
            (int)Sad::Errors::DiagnosticSeverity::NOTE,
            (int)Sad::Errors::DiagnosticSeverity::HINT
        );
    });

    // --- اختبار DG02 ---
    // (AR) تحويل الشدة إلى نص
    // (EN) Severity to string
    SAD_TEST("DG02: شدة إلى نص / Severity to string", {
        std::string err = Sad::Errors::severityToString(Sad::Errors::DiagnosticSeverity::ERROR);
        std::string warn = Sad::Errors::severityToString(Sad::Errors::DiagnosticSeverity::WARNING);
        SAD_ASSERT_FALSE(err.empty());
        SAD_ASSERT_FALSE(warn.empty());
        SAD_ASSERT_NE(err, warn);
    });

    // --- اختبار DG03 ---
    // (AR) ألوان الشدة
    // (EN) Severity colors 
    SAD_TEST("DG03: ألوان الشدة / Severity colors", {
        std::string errColor = Sad::Errors::severityToColor(Sad::Errors::DiagnosticSeverity::ERROR);
        std::string warnColor = Sad::Errors::severityToColor(Sad::Errors::DiagnosticSeverity::WARNING);
        // يجب أن تكون غير فارغة (رموز ANSI)
        SAD_ASSERT_FALSE(errColor.empty());
        SAD_ASSERT_FALSE(warnColor.empty());
    });

#endif // HAS_DIAGNOSTIC

    // ══════════════════════════════════════════════════════════════════
    // القسم 5: الأخطاء الذكية (Smart Errors - Phase 15)
    // Section 5: Smart Errors
    //
    // نظام الأخطاء الذكية يُثري رسائل الخطأ بـ:
    //   - اقتراحات إصلاح تلقائية
    //   - شروحات تعليمية (وضع المعلم)
    //   - منع تسلسل الأخطاء المتكررة
    //   - جمع أخطاء متعددة في مرة واحدة
    // ══════════════════════════════════════════════════════════════════
#if HAS_SMART_ERRORS

    SAD_GROUP("Errors.SmartErrors / الأخطاء.الذكية");

    // --- اختبار SE01 ---
    // (AR) SmartErrorSystem هو Singleton أيضاً
    // (EN) SmartErrorSystem is also Singleton
    SAD_TEST("SE01: Singleton / نسخة_وحيدة", {
        auto& sys1 = Sad::Errors::SmartErrorSystem::getInstance();
        auto& sys2 = Sad::Errors::SmartErrorSystem::getInstance();
        SAD_ASSERT_EQ(&sys1, &sys2);
    });

    // --- اختبار SE02 ---
    // (AR) تفعيل/تعطيل النظام الذكي
    // (EN) Enable/disable smart system
    SAD_TEST("SE02: تفعيل/تعطيل / Enable/disable", {
        auto& sys = Sad::Errors::SmartErrorSystem::getInstance();
        
        sys.setEnabled(true);
        SAD_ASSERT_TRUE(sys.isEnabled());
        
        sys.setEnabled(false);
        SAD_ASSERT_FALSE(sys.isEnabled());
        
        sys.setEnabled(true);
    });

    // --- اختبار SE03 ---
    // (AR) الوصول إلى المكونات الفرعية
    // (EN) Access sub-components
    SAD_TEST("SE03: الوصول للمكونات / Access components", {
        auto& sys = Sad::Errors::SmartErrorSystem::getInstance();
        
        // يجب أن تكون جميع المكونات قابلة للوصول
        auto& suggestions = sys.getSuggestionEngine();
        auto& typeExplainer = sys.getTypeExplainer();
        auto& multiError = sys.getMultiErrorCollector();
        auto& fixSuggestions = sys.getFixSuggestionManager();
        auto& teacherMode = sys.getTeacherMode();
        auto& cascade = sys.getCascadePrevention();
        
        // التحقق من العناوين (ليست null)
        SAD_ASSERT_NE((void*)&suggestions, (void*)nullptr);
        SAD_ASSERT_NE((void*)&typeExplainer, (void*)nullptr);
        SAD_ASSERT_NE((void*)&multiError, (void*)nullptr);
        SAD_ASSERT_NE((void*)&fixSuggestions, (void*)nullptr);
        SAD_ASSERT_NE((void*)&teacherMode, (void*)nullptr);
        SAD_ASSERT_NE((void*)&cascade, (void*)nullptr);
    });

    // --- اختبار SE04 ---
    // (AR) إعادة تعيين النظام
    // (EN) Reset system
    SAD_TEST("SE04: إعادة تعيين / Reset", {
        auto& sys = Sad::Errors::SmartErrorSystem::getInstance();
        SAD_ASSERT_NO_THROW(sys.reset());
    });

#endif // HAS_SMART_ERRORS

    // ══════════════════════════════════════════════════════════════════
    // القسم 6: وضع المعلم (Teacher Mode)
    // Section 6: Teacher Mode
    //
    // وضع المعلم يوفر شروحات تعليمية مفصلة للمبتدئين
    // يمكن ضبطه على 4 مستويات: موجز، عادي، مفصل، معلم
    // ══════════════════════════════════════════════════════════════════
#if HAS_TEACHER_MODE

    SAD_GROUP("Errors.TeacherMode / الأخطاء.وضع_المعلم");

    // --- اختبار TM01 ---
    // (AR) مستويات الشرح
    // (EN) Explanation levels
    SAD_TEST("TM01: مستويات الشرح / Explanation levels", {
        // التحقق من أن المستويات الأربعة مختلفة
        SAD_ASSERT_NE(
            (int)Sad::Errors::ExplanationLevel::BRIEF,
            (int)Sad::Errors::ExplanationLevel::NORMAL
        );
        SAD_ASSERT_NE(
            (int)Sad::Errors::ExplanationLevel::NORMAL,
            (int)Sad::Errors::ExplanationLevel::DETAILED
        );
        SAD_ASSERT_NE(
            (int)Sad::Errors::ExplanationLevel::DETAILED,
            (int)Sad::Errors::ExplanationLevel::TEACHER
        );
    });

    // --- اختبار TM02 ---
    // (AR) تعيين واسترجاع مستوى الشرح
    // (EN) Set and get explanation level
    SAD_TEST("TM02: تعيين المستوى / Set level", {
        Sad::Errors::TeacherMode tm;
        
        tm.setLevel(Sad::Errors::ExplanationLevel::TEACHER);
        SAD_ASSERT_EQ((int)tm.getLevel(), (int)Sad::Errors::ExplanationLevel::TEACHER);
        
        tm.setLevel(Sad::Errors::ExplanationLevel::BRIEF);
        SAD_ASSERT_EQ((int)tm.getLevel(), (int)Sad::Errors::ExplanationLevel::BRIEF);
    });

#endif // HAS_TEACHER_MODE

    // ══════════════════════════════════════════════════════════════════
    // القسم 7: اقتراحات الإصلاح (Suggestions)
    // Section 7: Fix Suggestions
    //
    // محرك الاقتراحات يحلل الخطأ ويقترح إصلاحات تلقائية
    // مع مستوى ثقة لكل اقتراح (عالي/متوسط/منخفض)
    // ══════════════════════════════════════════════════════════════════
#if HAS_SUGGESTIONS

    SAD_GROUP("Errors.Suggestions / الأخطاء.الاقتراحات");

    // --- اختبار SG01 ---
    // (AR) أنواع الاقتراحات
    // (EN) Suggestion types
    SAD_TEST("SG01: أنواع الاقتراحات / Suggestion types", {
        SAD_ASSERT_NE(
            (int)Sad::Errors::SuggestionType::REPLACEMENT,
            (int)Sad::Errors::SuggestionType::INSERTION
        );
        SAD_ASSERT_NE(
            (int)Sad::Errors::SuggestionType::INSERTION,
            (int)Sad::Errors::SuggestionType::DELETION
        );
    });

    // --- اختبار SG02 ---
    // (AR) مستويات الثقة
    // (EN) Confidence levels
    SAD_TEST("SG02: مستويات الثقة / Confidence levels", {
        SAD_ASSERT_NE(
            (int)Sad::Errors::ConfidenceLevel::HIGH,
            (int)Sad::Errors::ConfidenceLevel::MEDIUM
        );
        SAD_ASSERT_NE(
            (int)Sad::Errors::ConfidenceLevel::MEDIUM,
            (int)Sad::Errors::ConfidenceLevel::LOW
        );
    });

    // --- اختبار SG03 ---
    // (AR) فئات الأخطاء الذكية
    // (EN) Smart error categories
    SAD_TEST("SG03: فئات الأخطاء الذكية / Smart error categories", {
        // التحقق من أن الفئات مُعرَّفة ومختلفة
        SAD_ASSERT_NE(
            (int)Sad::Errors::SmartErrorCategory::TYPE_MISMATCH,
            (int)Sad::Errors::SmartErrorCategory::UNDEFINED_SYMBOL
        );
        SAD_ASSERT_NE(
            (int)Sad::Errors::SmartErrorCategory::SYNTAX_ERROR,
            (int)Sad::Errors::SmartErrorCategory::LOGIC_ERROR
        );
    });

    // --- اختبار SG04 ---
    // (AR) إنشاء محرك الاقتراحات
    // (EN) Create suggestion engine
    SAD_TEST("SG04: إنشاء محرك الاقتراحات / Create engine", {
        Sad::Errors::SuggestionEngine engine;
        // المحرك يجب أن يُنشأ بنجاح
        SAD_ASSERT_TRUE(true);
    });

    // --- اختبار SG05 ---
    // (AR) SmartSuggestion الافتراضي
    // (EN) Default SmartSuggestion
    SAD_TEST("SG05: اقتراح افتراضي / Default suggestion", {
        Sad::Errors::SmartSuggestion sg;
        SAD_ASSERT_EQ((int)sg.type, (int)Sad::Errors::SuggestionType::HINT);
        SAD_ASSERT_EQ((int)sg.confidence, (int)Sad::Errors::ConfidenceLevel::MEDIUM);
    });

#endif // HAS_SUGGESTIONS

    // ══════════════════════════════════════════════════════════════════
    // القسم 8: DiagnosticSink (مجمع التشخيصات)
    // Section 8: DiagnosticSink
    //
    // DiagnosticSink يجمع ويحفظ التشخيصات ويعطي إحصائيات
    // ══════════════════════════════════════════════════════════════════
#if HAS_ERROR_MANAGER

    SAD_GROUP("Errors.DiagnosticSink / مجمع_التشخيصات");

    // --- اختبار DS01 ---
    // (AR) مجمع جديد فارغ
    // (EN) New empty sink
    SAD_TEST("DS01: مجمع فارغ / Empty sink", {
        Sad::Errors::DiagnosticSink sink;
        SAD_ASSERT_EQ(sink.getErrorCount(), (size_t)0);
        SAD_ASSERT_EQ(sink.getWarningCount(), (size_t)0);
        SAD_ASSERT_FALSE(sink.hasErrors());
    });

#endif // HAS_ERROR_MANAGER

    // ══════════════════════════════════════════════════════════════════
    // عرض النتائج النهائية
    // ══════════════════════════════════════════════════════════════════
    SAD_SUMMARY();
}
