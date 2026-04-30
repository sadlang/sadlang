// ============================================================================
// input_sanitizer.h — تعقيم المدخلات (أوامر، مسارات، SQL، HTML، أعداد)
// (AR) فئة InputSanitizer: حماية من حقن الأوامر، اجتياز المسارات، حقن SQL،
//      هجمات XSS، وتحليل غير آمن للأعداد. مشتركة بين كل المسارات.
// (EN) InputSanitizer class: command-injection / path-traversal / SQLi / XSS
//      protection + safe number parsing. Shared across all execution paths.
// ============================================================================
//
// (AR) لماذا في الطبقة المشتركة؟
//      - المترجم: يقرأ ملفات .ص (مسارات) وقد يستدعي عمليات بناء (أوامر)
//      - المفسر: نفس الشيء + استدعاءات subprocess من كود المستخدم
//      - VM: نفس الشيء عند تنفيذ bytecode FFI
//      - runtime/stdlib: دوال HTTP/SQL/HTML المستخدِمة لمدخلات المستخدم
// ============================================================================

#ifndef SAD_SHARED_SECURITY_INPUT_SANITIZER_H
#define SAD_SHARED_SECURITY_INPUT_SANITIZER_H

#include <cstdint>
#include <string>

namespace Sad
{
    namespace Security
    {

        /**
         * @brief فئة تعقيم المدخلات
         * @brief (EN) Input sanitization class
         */
        class InputSanitizer
        {
        public:
            // ------------------------------------------------------------------
            // (AR) منع حقن الأوامر
            // (EN) Command injection prevention
            // ------------------------------------------------------------------

            /// @brief (AR) يعقّم نص أمر بإزالة الأحرف الخطرة
            /// @brief (EN) Sanitizes a command string by replacing dangerous characters
            static std::string sanitizeCommand(const std::string &input);

            /// @brief (AR) يفحص ما إذا كان الأمر آمناً (لا يحوي أنماطاً خطرة)
            /// @brief (EN) Returns true if command lacks known dangerous patterns
            static bool isCommandSafe(const std::string &cmd);

            /// @brief (AR) ينفذ أمر نظام بعد فحص أمانه (يرمي std::runtime_error إذا خطر)
            /// @brief (EN) Runs a system command after safety check (throws on danger)
            static int safeSystem(const std::string &cmd);

            // ------------------------------------------------------------------
            // (AR) منع اجتياز المسارات
            // (EN) Path traversal prevention
            // ------------------------------------------------------------------

            /// @brief (AR) يعقّم مساراً بإزالة "../" والأحرف المشبوهة
            /// @brief (EN) Sanitizes a file path (removes "../" and suspicious chars)
            static std::string sanitizePath(const std::string &path);

            /// @brief (AR) يفحص أن المسار يقع داخل مجلد مسموح به
            /// @brief (EN) Returns true if path is canonically inside allowedDir
            static bool isPathWithinDirectory(const std::string &path,
                                              const std::string &allowedDir);

            /// @brief (AR) يفحص صحة اسم ملف (طول، أحرف ممنوعة، أسماء محجوزة)
            /// @brief (EN) Validates a file name (length, forbidden chars, reserved names)
            static bool isValidFileName(const std::string &filename);

            // ------------------------------------------------------------------
            // (AR) منع حقن SQL
            // (EN) SQL injection prevention
            // ------------------------------------------------------------------

            /// @brief (AR) يعقّم نصاً لاستخدامه في استعلام SQL (escape للأحرف الخطرة)
            /// @brief (EN) Escapes dangerous characters for safe SQL embedding
            static std::string sanitizeSQL(const std::string &input);

            /// @brief (AR) يفحص أن استعلام SQL لا يحوي أنماطاً مدمرة
            /// @brief (EN) Returns true if SQL query lacks destructive patterns
            static bool isSQLSafe(const std::string &query);

            // ------------------------------------------------------------------
            // (AR) منع هجمات XSS
            // (EN) XSS prevention
            // ------------------------------------------------------------------

            /// @brief (AR) يعقّم نصاً ليُعرض في HTML (escape لكيانات HTML)
            /// @brief (EN) Escapes HTML entities to prevent XSS
            static std::string sanitizeHTML(const std::string &input);

            /// @brief (AR) يزيل كل وسوم HTML من النص
            /// @brief (EN) Strips all HTML tags from a string
            static std::string stripTags(const std::string &input);

            // ------------------------------------------------------------------
            // (AR) تحليل آمن للأعداد
            // (EN) Safe number parsing
            // ------------------------------------------------------------------

            /// @brief (AR) يحلل عدداً صحيحاً مع التحقق من استهلاك كامل النص
            /// @brief (EN) Parses an int64 ensuring entire string was consumed
            static bool safeParseInt(const std::string &str, std::int64_t &result);

            /// @brief (AR) يحلل عدداً عشرياً ويفحص أنه finite (ليس NaN/Inf)
            /// @brief (EN) Parses a double, ensuring it is finite
            static bool safeParseDouble(const std::string &str, double &result);

        private:
            static bool isAlphaNumeric(char c) noexcept;
            static std::string toLower(const std::string &str);
            static std::string toUpper(const std::string &str);
        };

    } // namespace Security
} // namespace Sad

#endif // SAD_SHARED_SECURITY_INPUT_SANITIZER_H
