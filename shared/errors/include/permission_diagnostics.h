/**
 * @file permission_diagnostics.h
 * @brief (AR) تشخيصات أخطاء الأذونات — رسائل عربية واضحة مع اقتراحات الحل
 *        (EN) Permission error diagnostics — clear bilingual messages with fix hints
 *
 * المكون: shared/errors (نظام الأخطاء المشترك)
 * المسار: shared/errors/include/permission_diagnostics.h
 *
 * (AR) يُوفر هذا الملف نظام رسائل أخطاء الأذونات في لغة ص.
 *
 *   الفلسفة التصميمية:
 *   لغة ص تُطبِّق نموذج أمان صارم: كل وصول لملف/شبكة/جهاز يحتاج تصريحاً
 *   صريحاً في ملف ص.toml. عندما يفتقد البرنامج إذناً، يجب أن تكون رسالة
 *   الخطأ مفيدة جداً — تشرح المشكلة وتُقدِّم الحل الدقيق بمثال فوري.
 *
 *   المحتوى:
 *   - PermissionCategory: تعداد 15 فئة إذن (ملفات/شبكة/أجهزة/نظام/FFI)
 *   - PermissionErrorTemplate: قالب رسالة ثنائية اللغة مع مثال TOML
 *   - PermissionDiagnosticBuilder: مولِّد رسائل كامل (Singleton)
 *
 *   الاستخدام:
 *   @code
 *   // عند رفض قراءة ملف
 *   auto& builder = Sad::Errors::Permissions::PermissionDiagnosticBuilder::instance();
 *   std::string msg = builder.build(
 *       Sad::Errors::Permissions::PermissionCategory::FILE_READ,
 *       "/etc/passwd", "main.ص", 42
 *   );
 *   std::cerr << msg;
 *   @endcode
 *
 *   الرسالة الناتجة تشمل:
 *   - رمز الخطأ (E-PERM-001)
 *   - شرح السبب بالعربية
 *   - الحل خطوة بخطوة
 *   - مثال ص.toml جاهز للنسخ
 *   - رابط التوثيق
 *
 *   استُبدلت به:
 *      - compiler/src/diagnostics/permission_errors_ar.cpp (Fix #26)
 *   السبب: رسائل الأذونات تنفع المفسر والمترجم معاً + تحتاج API عام في .h
 *
 * (EN) Provides bilingual permission error messages for the Sad security model.
 *      Each denied permission generates a user-friendly message with TOML example.
 *      Replaces: compiler/src/diagnostics/permission_errors_ar.cpp (Fix #26)
 *
 * @author Sad Language Development Team
 * @version 2.0 (unified in shared/errors — Fix #26)
 */

#pragma once

// ─── القسم 1: رؤوس المشروع ───────────────────────────────────────────────────
// (لا توجد dependencies على ملفات المشروع — مستقل تماماً)

// ─── القسم 2: رؤوس المكتبة القياسية ─────────────────────────────────────────
#include <sstream>
#include <string>
#include <unordered_map>

namespace Sad
{
    namespace Errors
    {
        namespace Permissions
        {

            // ============================================================================
            // (AR) فئة خطأ الإذن / (EN) Permission Error Category
            // ============================================================================

            /**
             * @enum PermissionCategory
             * @brief (AR) فئة الإذن الذي تم رفضه
             *        (EN) Category of the denied permission
             */
            enum class PermissionCategory
            {
                FILE_READ,     ///< (AR) قراءة ملف / (EN) File read
                FILE_WRITE,    ///< (AR) كتابة ملف / (EN) File write
                FILE_DELETE,   ///< (AR) حذف ملف / (EN) File delete
                FILE_CREATE,   ///< (AR) إنشاء ملف / (EN) File create
                NET_CONNECT,   ///< (AR) اتصال شبكي / (EN) Network connect
                NET_LISTEN,    ///< (AR) الاستماع على منفذ / (EN) Network listen
                SYSTEM_EXEC,   ///< (AR) تنفيذ أمر نظام / (EN) System command execution
                SYSTEM_ENV,    ///< (AR) قراءة متغيرات البيئة / (EN) Environment variable access
                CAMERA,        ///< (AR) الكاميرا / (EN) Camera
                MICROPHONE,    ///< (AR) الميكروفون / (EN) Microphone
                LOCATION,      ///< (AR) الموقع الجغرافي / (EN) Location
                NOTIFICATIONS, ///< (AR) الإشعارات / (EN) Notifications
                CLIPBOARD,     ///< (AR) الحافظة / (EN) Clipboard
                SUBPROCESS,    ///< (AR) عملية فرعية / (EN) Subprocess
                FFI            ///< (AR) استدعاء خارجي (FFI) / (EN) Foreign Function Interface
            };

            // ============================================================================
            // (AR) قالب رسالة الخطأ / (EN) Error Message Template
            // ============================================================================

            /**
             * @struct PermissionErrorTemplate
             * @brief (AR) قالب رسالة خطأ أذونات ثنائي اللغة
             *        (EN) Bilingual permission error message template
             */
            struct PermissionErrorTemplate
            {
                std::string code;         ///< رمز الخطأ (مثال: E-PERM-001)
                std::string title_ar;     ///< العنوان بالعربية
                std::string title_en;     ///< العنوان بالإنجليزية
                std::string explain_ar;   ///< شرح السبب بالعربية
                std::string explain_en;   ///< شرح السبب بالإنجليزية
                std::string solution_ar;  ///< الحل بالعربية
                std::string solution_en;  ///< الحل بالإنجليزية
                std::string toml_example; ///< مثال ص.toml مع {target} placeholder
                std::string help_url;     ///< رابط المساعدة
            };

            // ============================================================================
            // (AR) مُولِّد رسائل أخطاء الأذونات / (EN) Permission Error Message Builder
            // ============================================================================

            /**
             * @class PermissionDiagnosticBuilder
             * @brief (AR) يُولِّد رسائل خطأ أذونات مفصَّلة وواضحة
             *        (EN) Generates detailed and clear permission error messages
             *
             * (AR) يحوِّل أكواد الأذونات المجردة إلى رسائل صديقة للمستخدم
             *      مع شرح السبب واقتراح الحل بالعربية والإنجليزية
             * (EN) Converts abstract permission codes into user-friendly messages
             *      with cause explanation and fix suggestion in Arabic and English
             *
             * @example
             * @code
             * auto& builder = Sad::Errors::Permissions::PermissionDiagnosticBuilder::instance();
             * std::string msg = builder.build(
             *     Sad::Errors::Permissions::PermissionCategory::FILE_READ,
             *     "/etc/passwd", "main.ص", 42
             * );
             * std::cerr << msg;
             * @endcode
             */
            class PermissionDiagnosticBuilder
            {
            public:
                /**
                 * @brief (AR) الحصول على المثال الوحيد (Singleton)
                 *        (EN) Get singleton instance
                 */
                static PermissionDiagnosticBuilder &instance()
                {
                    static PermissionDiagnosticBuilder inst;
                    return inst;
                }

                /**
                 * @brief (AR) بناء رسالة خطأ أذونات مفصَّلة
                 *        (EN) Build detailed permission error message
                 *
                 * @param category (AR) فئة الإذن / (EN) Permission category
                 * @param target   (AR) الهدف (مسار الملف، العنوان، إلخ) / (EN) Target (file path, address, etc.)
                 * @param file     (AR) ملف الكود / (EN) Source file
                 * @param line     (AR) رقم السطر / (EN) Line number
                 * @param lang_ar  (AR) استخدم العربية للرسالة الرئيسية (افتراضي: true)
                 *                 (EN) Use Arabic for main message (default: true)
                 * @return (AR) رسالة منسَّقة / (EN) Formatted message
                 */
                std::string build(
                    PermissionCategory category,
                    const std::string &target,
                    const std::string &file = "",
                    int line = 0,
                    bool lang_ar = true) const;

                /**
                 * @brief (AR) الحصول على قالب الخطأ لفئة محددة
                 *        (EN) Get error template for specific category
                 */
                const PermissionErrorTemplate &getTemplate(PermissionCategory category) const;

            private:
                PermissionDiagnosticBuilder(); ///< الباني — يُهيِّئ جداول القوالب

                std::unordered_map<int, PermissionErrorTemplate> templates_;

                void initTemplates(); ///< يُهيِّئ جميع قوالب الرسائل
                std::string formatBox(const std::string &content, int width = 68) const;
            };

        } // namespace Permissions
    } // namespace Errors
} // namespace Sad
