// ============================================================================
// input_sanitizer.cpp — تنفيذ فئة InputSanitizer
// (AR) منقول من compiler/src/security/compiler_security.cpp إلى الطبقة المشتركة
//      مع توحيد فضاء الأسماء تحت Sad::Security وإصلاح الترميز.
// (EN) Moved from compiler/src/security/compiler_security.cpp to the shared
//      layer, unified under namespace Sad::Security, encoding fixed.
// ============================================================================

#include "input_sanitizer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <vector>

namespace Sad
{
    namespace Security
    {

        // ----------------------------------------------------------------------------
        // (AR) منع حقن الأوامر
        // (EN) Command injection prevention
        // ----------------------------------------------------------------------------

        std::string InputSanitizer::sanitizeCommand(const std::string &input)
        {
            std::string output;
            output.reserve(input.size());
            for (char c : input)
            {
                if (isAlphaNumeric(c) || c == ' ' || c == '-' || c == '_' ||
                    c == '.' || c == '/' || c == '\\')
                {
                    output += c;
                }
                else
                {
                    output += ' ';
                }
            }
            return output;
        }

        bool InputSanitizer::isCommandSafe(const std::string &cmd)
        {
            if (cmd.empty())
                return false;

            // (AR) أنماط مدمرة معروفة
            // (EN) known destructive patterns
            static const std::vector<std::string> dangerousPatterns = {
                "rm -rf", "del /f", "format", "mkfs",
                "dd if=", ":(){:|:&};:", "chmod 777",
                "curl | bash", "wget | bash", "eval",
                "exec", "system", "popen", "subprocess",
                "> /dev/", "| sh", "| bash", "; rm",
                "&& rm", "| rm", "$(", "`"};

            const std::string lowerCmd = toLower(cmd);
            for (const auto &pattern : dangerousPatterns)
            {
                if (lowerCmd.find(pattern) != std::string::npos)
                    return false;
            }
            return true;
        }

        int InputSanitizer::safeSystem(const std::string &cmd)
        {
            if (!isCommandSafe(cmd))
            {
                throw std::runtime_error("Potentially dangerous command blocked");
            }
            const std::string sanitized = sanitizeCommand(cmd);
            return std::system(sanitized.c_str());
        }

        // ----------------------------------------------------------------------------
        // (AR) منع اجتياز المسارات
        // (EN) Path traversal prevention
        // ----------------------------------------------------------------------------

        std::string InputSanitizer::sanitizePath(const std::string &path)
        {
            std::string normalized = path;
            std::size_t pos;

            // (AR) إزالة جميع تسلسلات ".."
            // (EN) strip every ".." occurrence
            while ((pos = normalized.find("..")) != std::string::npos)
            {
                normalized.erase(pos, 2);
                if (pos < normalized.size() &&
                    (normalized[pos] == '/' || normalized[pos] == '\\'))
                {
                    normalized.erase(pos, 1);
                }
            }

            // (AR) توحيد الفواصل ثم تصفية الأحرف غير المسموح بها
            // (EN) normalize separators then filter disallowed characters
            std::replace(normalized.begin(), normalized.end(), '\\', '/');

            std::string result;
            result.reserve(normalized.size());
            for (char c : normalized)
            {
                if (isAlphaNumeric(c) || c == '/' || c == '.' ||
                    c == '-' || c == '_' || c == ':' || c == ' ')
                {
                    result += c;
                }
            }
            return result;
        }

        bool InputSanitizer::isPathWithinDirectory(const std::string &path,
                                                   const std::string &allowedDir)
        {
            try
            {
                const auto canonical = std::filesystem::canonical(path);
                const auto allowedCanonical = std::filesystem::canonical(allowedDir);
                const auto relative = std::filesystem::relative(canonical, allowedCanonical);
                const std::string relStr = relative.string();
                return relStr.size() < 2 || relStr.substr(0, 2) != "..";
            }
            catch (...)
            {
                return false;
            }
        }

        bool InputSanitizer::isValidFileName(const std::string &filename)
        {
            if (filename.empty() || filename.size() > 255)
                return false;

            static const std::string forbidden = "<>:\"/\\|?*";
            for (char c : filename)
            {
                if (forbidden.find(c) != std::string::npos)
                    return false;
                if (static_cast<unsigned char>(c) < 32)
                    return false;
            }

            // (AR) أسماء محجوزة في Windows
            // (EN) Windows reserved names
            static const std::vector<std::string> reserved = {
                "CON", "PRN", "AUX", "NUL",
                "COM1", "COM2", "COM3", "COM4",
                "LPT1", "LPT2", "LPT3", "LPT4"};
            const std::string upper = toUpper(filename);
            for (const auto &name : reserved)
            {
                if (upper == name)
                    return false;
            }
            return true;
        }

        // ----------------------------------------------------------------------------
        // (AR) منع حقن SQL
        // (EN) SQL injection prevention
        // ----------------------------------------------------------------------------

        std::string InputSanitizer::sanitizeSQL(const std::string &input)
        {
            std::string output;
            output.reserve(input.size() + (input.size() / 4));
            for (char c : input)
            {
                switch (c)
                {
                case '\'':
                    output += "\\'";
                    break;
                case '"':
                    output += "\\\"";
                    break;
                case '\\':
                    output += "\\\\";
                    break;
                case '\0':
                    output += "\\0";
                    break;
                case '\n':
                    output += "\\n";
                    break;
                case '\r':
                    output += "\\r";
                    break;
                case '\x1a':
                    output += "\\Z";
                    break;
                default:
                    output += c;
                    break;
                }
            }
            return output;
        }

        bool InputSanitizer::isSQLSafe(const std::string &query)
        {
            if (query.empty())
                return false;
            static const std::vector<std::string> dangerous = {
                "DROP", "DELETE", "TRUNCATE", "ALTER", "EXEC", "--"};
            const std::string upper = toUpper(query);
            for (const auto &pattern : dangerous)
            {
                if (upper.find(pattern) != std::string::npos)
                    return false;
            }
            return true;
        }

        // ----------------------------------------------------------------------------
        // (AR) منع XSS
        // (EN) XSS prevention
        // ----------------------------------------------------------------------------

        std::string InputSanitizer::sanitizeHTML(const std::string &input)
        {
            std::string output;
            output.reserve(input.size() + (input.size() / 4));
            for (char c : input)
            {
                switch (c)
                {
                case '&':
                    output += "&amp;";
                    break;
                case '<':
                    output += "&lt;";
                    break;
                case '>':
                    output += "&gt;";
                    break;
                case '"':
                    output += "&quot;";
                    break;
                case '\'':
                    output += "&#x27;";
                    break;
                case '/':
                    output += "&#x2F;";
                    break;
                default:
                    output += c;
                    break;
                }
            }
            return output;
        }

        std::string InputSanitizer::stripTags(const std::string &input)
        {
            std::string output;
            output.reserve(input.size());
            bool inTag = false;
            for (char c : input)
            {
                if (c == '<')
                {
                    inTag = true;
                }
                else if (c == '>')
                {
                    inTag = false;
                }
                else if (!inTag)
                {
                    output += c;
                }
            }
            return output;
        }

        // ----------------------------------------------------------------------------
        // (AR) تحليل آمن للأعداد
        // (EN) Safe number parsing
        // ----------------------------------------------------------------------------

        bool InputSanitizer::safeParseInt(const std::string &str, std::int64_t &result)
        {
            if (str.empty())
                return false;
            try
            {
                std::size_t pos = 0;
                result = std::stoll(str, &pos);
                return pos == str.size();
            }
            catch (...)
            {
                return false;
            }
        }

        bool InputSanitizer::safeParseDouble(const std::string &str, double &result)
        {
            if (str.empty())
                return false;
            try
            {
                std::size_t pos = 0;
                result = std::stod(str, &pos);
                return pos == str.size() && std::isfinite(result);
            }
            catch (...)
            {
                return false;
            }
        }

        // ----------------------------------------------------------------------------
        // (AR) مساعدات داخلية
        // (EN) Private helpers
        // ----------------------------------------------------------------------------

        bool InputSanitizer::isAlphaNumeric(char c) noexcept
        {
            return (c >= 'a' && c <= 'z') ||
                   (c >= 'A' && c <= 'Z') ||
                   (c >= '0' && c <= '9');
        }

        std::string InputSanitizer::toLower(const std::string &str)
        {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(),
                           [](unsigned char c)
                           { return static_cast<char>(std::tolower(c)); });
            return result;
        }

        std::string InputSanitizer::toUpper(const std::string &str)
        {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(),
                           [](unsigned char c)
                           { return static_cast<char>(std::toupper(c)); });
            return result;
        }

    } // namespace Security
} // namespace Sad
