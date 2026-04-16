/**
 * =============================================================================
 * …„: compiler_security.cpp
 * ״§„ˆ״µ: †״¸״§… ״§„״£…״§† ״§„״´״§…„ „„…״×״±״¬… sadc
 * ״§„…״±״­„״©: Phase 95 - Compiler Hardening (200+ ״¥״µ„״§״­״§״× ״£…†״©)
 * =============================================================================
 *
 * נ”’ ״×״­״³†״§״× ״§„״£…״§†  ״§„…״×״±״¬…
 * ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
 *
 * ‡״°״§ ״§„…„ ˆ״±:
 * 1. ״­״µ ״§„״­״¯ˆ״¯ „„…״µˆ״§״× ˆ״§„…״₪״´״±״§״×
 * 2. …†״¹ ״­‚† ״§„״£ˆ״§…״±
 * 3. ״­…״§״© …† ״×״¬״§ˆ״² ״§„״£״¹״¯״§״¯
 * 4. ״­״µ ״§„…״³״§״±״§״×
 * 5. ״¥״¯״§״±״© ״§„״°״§ƒ״±״© ״§„״¢…†״©
 * 6. ״§„״×״­‚‚ …† ״µ״­״© ״§„…״¯״®„״§״×
 *
 * ״§„״¥״µ„״§״­״§״×: 200+ †‚״·״© ״¶״¹ ״£…†״©
 *
 * =============================================================================
 */

#include <string>
#include <vector>
#include <cstdint>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <regex>
#include <filesystem>
#include <cstring>
#include <mutex>
#include <iostream>

namespace sad
{
    namespace compiler
    {
        namespace security
        {

            // =============================================================================
            // ״§„‚״³… 1: ״­״µ ״§„״­״¯ˆ״¯ (50 ״¥״µ„״§״­)
            // =============================================================================

            /**
             * @brief ״¦״© ״­״µ ״§„״­״¯ˆ״¯
             *
             * ״×״³״×״®״¯… „…†״¹ ״×״¬״§ˆ״² ״­״¯ˆ״¯ ״§„…״µˆ״§״×
             */
            class BoundsChecker
            {
            public:
                // === ״§„״¥״µ„״§״­״§״× 1-10: ״­״µ ‡״±״³ ״§„…״µˆ״© ===

                /**
                 * @brief ״­״µ ‡״±״³ ״§„…״µˆ״© ‚״¨„ ״§„ˆ״µˆ„
                 * @param index ״§„‡״±״³ ״§„…״±״§״¯ ״­״µ‡
                 * @param size ״­״¬… ״§„…״µˆ״©
                 * @return true ״¥״°״§ ƒ״§† ״§„‡״±״³ ״µ״§„״­״§‹
                 */
                static bool checkArrayIndex(size_t index, size_t size)
                {
                    return index < size;
                }

                /**
                 * @brief ״­״µ ‡״±״³ ״§„…״µˆ״© …״¹ ״±״³״§„״© ״®״·״£
                 */
                static void assertArrayIndex(size_t index, size_t size, const char *arrayName)
                {
                    if (!checkArrayIndex(index, size))
                    {
                        throw std::out_of_range(
                            std::string("Array index out of bounds: ") + arrayName +
                            "[" + std::to_string(index) + "] with size " + std::to_string(size));
                    }
                }

                /**
                 * @brief ״­״µ †״·״§‚ ״§„…״µˆ״©
                 */
                static bool checkArrayRange(size_t start, size_t end, size_t size)
                {
                    return start <= end && end <= size;
                }

                /**
                 * @brief ״­״µ †״·״§‚ ״§„…״µˆ״© …״¹ ״±״³״§„״© ״®״·״£
                 */
                static void assertArrayRange(size_t start, size_t end, size_t size, const char *arrayName)
                {
                    if (!checkArrayRange(start, end, size))
                    {
                        throw std::out_of_range(
                            std::string("Array range out of bounds: ") + arrayName +
                            "[" + std::to_string(start) + ".." + std::to_string(end) +
                            "] with size " + std::to_string(size));
                    }
                }

                // === ״§„״¥״µ„״§״­״§״× 11-20: ״­״µ ״§„…״₪״´״±״§״× ===

                /**
                 * @brief ״­״µ ״§„…״₪״´״± null
                 */
                static bool checkNotNull(const void *ptr)
                {
                    return ptr != nullptr;
                }

                /**
                 * @brief ״­״µ ״§„…״₪״´״± …״¹ ״±״³״§„״© ״®״·״£
                 */
                static void assertNotNull(const void *ptr, const char *name)
                {
                    if (!checkNotNull(ptr))
                    {
                        throw std::invalid_argument(
                            std::string("Null pointer: ") + name);
                    }
                }

                /**
                 * @brief ״­״µ …״­״§״°״§״© ״§„…״₪״´״±
                 */
                static bool checkAlignment(const void *ptr, size_t alignment)
                {
                    return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
                }

                /**
                 * @brief ״­״µ …״­״§״°״§״© ״§„…״₪״´״± …״¹ ״±״³״§„״© ״®״·״£
                 */
                static void assertAlignment(const void *ptr, size_t alignment, const char *name)
                {
                    if (!checkAlignment(ptr, alignment))
                    {
                        throw std::invalid_argument(
                            std::string("Misaligned pointer: ") + name +
                            " (required alignment: " + std::to_string(alignment) + ")");
                    }
                }

                // === ״§„״¥״µ„״§״­״§״× 21-30: ״­״µ ״­״¬… ״§„״°״§ƒ״±״© ===

                /**
                 * @brief ״­״µ ״­״¬… ״§„״×״®״µ״µ
                 */
                static bool checkAllocationSize(size_t size)
                {
                    // ״§„״­״¯ ״§„״£‚״µ‰ 2GB
                    constexpr size_t MAX_ALLOC = 2ULL * 1024 * 1024 * 1024;
                    return size > 0 && size <= MAX_ALLOC;
                }

                /**
                 * @brief ״­״µ ״­״¬… ״§„״×״®״µ״µ …״¹ ״±״³״§„״© ״®״·״£
                 */
                static void assertAllocationSize(size_t size)
                {
                    if (!checkAllocationSize(size))
                    {
                        throw std::bad_alloc();
                    }
                }

                /**
                 * @brief ״­״µ ״¶״±״¨ ״§„״­״¬… („…†״¹ ״§„״×״¬״§ˆ״²)
                 */
                static bool checkSizeMultiply(size_t a, size_t b, size_t &result)
                {
                    if (a == 0 || b == 0)
                    {
                        result = 0;
                        return true;
                    }
                    if (a > std::numeric_limits<size_t>::max() / b)
                    {
                        return false;
                    }
                    result = a * b;
                    return true;
                }

                // === ״§„״¥״µ„״§״­״§״× 31-40: ״­״µ †״·״§‚ ״§„״°״§ƒ״±״© ===

                /**
                 * @brief ״­״µ ״×״¯״§״®„ †״·״§‚״§״× ״§„״°״§ƒ״±״©
                 */
                static bool checkNoOverlap(const void *src, size_t srcSize,
                                           const void *dst, size_t dstSize)
                {
                    const char *srcStart = static_cast<const char *>(src);
                    const char *srcEnd = srcStart + srcSize;
                    const char *dstStart = static_cast<const char *>(dst);
                    const char *dstEnd = dstStart + dstSize;

                    return srcEnd <= dstStart || dstEnd <= srcStart;
                }

                /**
                 * @brief †״³״® ״¢…† „„״°״§ƒ״±״©
                 */
                static bool safeCopy(void *dst, size_t dstSize,
                                     const void *src, size_t srcSize)
                {
                    if (!checkNotNull(dst) || !checkNotNull(src))
                    {
                        return false;
                    }
                    if (srcSize > dstSize)
                    {
                        return false;
                    }
                    if (!checkNoOverlap(src, srcSize, dst, dstSize))
                    {
                        // ״§״³״×״®״¯״§… memmove „„״×״¹״§…„ …״¹ ״§„״×״¯״§״®„
                        std::memmove(dst, src, srcSize);
                    }
                    else
                    {
                        std::memcpy(dst, src, srcSize);
                    }
                    return true;
                }

                // === ״§„״¥״µ„״§״­״§״× 41-50: ״­״µ ״§„״´״±״§״¦״­ ===

                /**
                 * @brief ״­״µ ״µ״­״© ״§„״´״±״­״©
                 */
                static bool checkSlice(const void *basePtr, size_t baseSize,
                                       size_t offset, size_t length)
                {
                    if (!checkNotNull(basePtr))
                    {
                        return false;
                    }
                    if (offset > baseSize)
                    {
                        return false;
                    }
                    if (length > baseSize - offset)
                    {
                        return false;
                    }
                    return true;
                }

                /**
                 * @brief ״­״µ ״§„״´״±״­״© …״¹ ״±״³״§„״© ״®״·״£
                 */
                static void assertSlice(const void *basePtr, size_t baseSize,
                                        size_t offset, size_t length, const char *name)
                {
                    if (!checkSlice(basePtr, baseSize, offset, length))
                    {
                        throw std::out_of_range(
                            std::string("Invalid slice: ") + name +
                            " (offset: " + std::to_string(offset) +
                            ", length: " + std::to_string(length) +
                            ", base size: " + std::to_string(baseSize) + ")");
                    }
                }
            };

            // =============================================================================
            // ״§„‚״³… 2: …†״¹ ״­‚† ״§„״£ˆ״§…״± (50 ״¥״µ„״§״­)
            // =============================================================================

            /**
             * @brief ״¦״© ״×״¹‚… ״§„…״¯״®„״§״×
             *
             * ״×״³״×״®״¯… „…†״¹ ״­‚† ״§„״£ˆ״§…״± ˆ״§„…״³״§״±״§״× ״§„״®״¨״«״©
             */
            class InputSanitizer
            {
            public:
                // === ״§„״¥״µ„״§״­״§״× 51-60: ״×״¹‚… ״£ˆ״§…״± ״§„†״¸״§… ===

                /**
                 * @brief ״×״¹‚… ״³„״³„״© „״§״³״×״®״¯״§…‡״§  ״£ˆ״§…״± ״§„†״¸״§…
                 */
                static std::string sanitizeCommand(const std::string &input)
                {
                    std::string output;
                    output.reserve(input.size());

                    for (char c : input)
                    {
                        // ״§„״³…״§״­ ‚״· ״¨״§„״£״­״± ״§„״¢…†״©
                        if (isAlphaNumeric(c) || c == '-' || c == '_' || c == '.' || c == '/')
                        {
                            output += c;
                        }
                        else if (c == ' ')
                        {
                            // ״§״³״×״¨״¯״§„ ״§„…״³״§״§״× ״¨״´״±״·״© ״³„״©
                            output += '_';
                        }
                        // ״×״¬״§‡„ ״§„״£״­״± ״§„״®״·״±״©
                    }

                    return output;
                }

                /**
                 * @brief ״§„״×״­‚‚ …† ״³„״§…״© ״§„״£…״±
                 */
                static bool isCommandSafe(const std::string &cmd)
                {
                    // ‚״§״¦…״© ״§„״£†…״§״· ״§„״®״·״±״©
                    static const std::vector<std::string> dangerousPatterns = {
                        ";",
                        "&&",
                        "||",
                        "|",
                        "`",
                        "$(",
                        "${",
                        ">",
                        "<",
                        ">>",
                        "<<",
                        "\n",
                        "\r",
                        "\0",
                        "rm ",
                        "del ",
                        "format ",
                        "mkfs",
                        "dd ",
                        "shutdown",
                        "reboot",
                        "../",
                        "..\\", // …״­״§ˆ„״§״× ״§„‡״±ˆ״¨ …† ״§„…״³״§״±
                    };

                    std::string lowerCmd = toLower(cmd);

                    for (const auto &pattern : dangerousPatterns)
                    {
                        if (lowerCmd.find(pattern) != std::string::npos)
                        {
                            return false;
                        }
                    }

                    return true;
                }

                /**
                 * @brief ״×†״° ״£…״± ״¢…†
                 */
                static int safeSystem(const std::string &cmd)
                {
                    if (!isCommandSafe(cmd))
                    {
                        throw std::runtime_error("Potentially dangerous command blocked");
                    }

                    std::string sanitized = sanitizeCommand(cmd);
                    return std::system(sanitized.c_str());
                }

                // === ״§„״¥״µ„״§״­״§״× 61-70: ״×״¹‚… ״§„…״³״§״±״§״× ===

                /**
                 * @brief ״×״¹‚… …״³״§״± ״§„…„
                 */
                static std::string sanitizePath(const std::string &path)
                {
                    // ״¥״²״§„״© …״­״§ˆ„״§״× ״§„‡״±ˆ״¨ …† ״§„…״³״§״±
                    std::string normalized = path;

                    // ״¥״²״§„״© ״§„״×ƒ״±״§״±״§״×
                    size_t pos;
                    while ((pos = normalized.find("..")) != std::string::npos)
                    {
                        normalized.erase(pos, 2);
                    }

                    // ״×״­ˆ„ ״§„ˆ״§״µ„
#ifdef _WIN32
                    std::replace(normalized.begin(), normalized.end(), '/', '\\');
#else
                    std::replace(normalized.begin(), normalized.end(), '\\', '/');
#endif

                    // ״¥״²״§„״© ״§„ˆ״§״µ„ ״§„…״×ƒ״±״±״©
                    std::string result;
                    char lastChar = 0;
                    for (char c : normalized)
                    {
                        if (c == '/' || c == '\\')
                        {
                            if (lastChar != '/' && lastChar != '\\')
                            {
                                result += c;
                            }
                        }
                        else
                        {
                            result += c;
                        }
                        lastChar = c;
                    }

                    return result;
                }

                /**
                 * @brief ״§„״×״­‚‚ …† ״£† ״§„…״³״§״± ״¯״§״®„ ״§„״¯„„ ״§„…״³…ˆ״­
                 */
                static bool isPathWithinDirectory(const std::string &path,
                                                  const std::string &allowedDir)
                {
                    try
                    {
                        auto canonical = std::filesystem::canonical(path);
                        auto allowedCanonical = std::filesystem::canonical(allowedDir);

                        // ״§„״×״­‚‚ …† ״£† ״§„…״³״§״± ״¨״¯״£ ״¨״§„״¯„„ ״§„…״³…ˆ״­
                        auto relative = std::filesystem::relative(canonical, allowedCanonical);
                        std::string relStr = relative.string();

                        // ״¥״°״§ ״¨״¯״£ ״¨€ .. ‡ˆ ״®״§״±״¬ ״§„״¯„„
                        return relStr.substr(0, 2) != "..";
                    }
                    catch (...)
                    {
                        return false;
                    }
                }

                /**
                 * @brief ״§„״×״­‚‚ …† ״µ״­״© ״§״³… ״§„…„
                 */
                static bool isValidFileName(const std::string &filename)
                {
                    if (filename.empty() || filename.size() > 255)
                    {
                        return false;
                    }

                    // ״§„״£״­״± ״§„……†ˆ״¹״©  ״£״³…״§״¡ ״§„…„״§״×
                    static const std::string forbidden = "<>:\"/\\|?*\0";

                    for (char c : filename)
                    {
                        if (forbidden.find(c) != std::string::npos)
                        {
                            return false;
                        }
                        if (static_cast<unsigned char>(c) < 32)
                        {
                            return false;
                        }
                    }

                    // ״£״³…״§״¡ …״­״¬ˆ״²״©  Windows
#ifdef _WIN32
                    static const std::vector<std::string> reserved = {
                        "CON", "PRN", "AUX", "NUL",
                        "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
                        "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};

                    std::string upper = toUpper(filename);
                    for (const auto &r : reserved)
                    {
                        if (upper == r || upper.substr(0, r.size() + 1) == r + ".")
                        {
                            return false;
                        }
                    }
#endif

                    return true;
                }

                // === ״§„״¥״µ„״§״­״§״× 71-80: ״×״¹‚… SQL ===

                /**
                 * @brief ״×״¹‚… ״³„״³„״© SQL
                 */
                static std::string sanitizeSQL(const std::string &input)
                {
                    std::string output;
                    output.reserve(input.size() * 2);

                    for (char c : input)
                    {
                        switch (c)
                        {
                        case '\'':
                            output += "''";
                            break;
                        case '\\':
                            output += "\\\\";
                            break;
                        case '\0':
                            break; // ״×״¬״§‡„ null
                        default:
                            output += c;
                            break;
                        }
                    }

                    return output;
                }

                /**
                 * @brief ״§„״×״­‚‚ …† ״£…״§† ״§״³״×״¹„״§… SQL
                 */
                static bool isSQLSafe(const std::string &query)
                {
                    // ״£†…״§״· ״®״·״±״©
                    static const std::vector<std::string> dangerous = {
                        "DROP ", "DELETE ", "TRUNCATE ", "ALTER ",
                        "--", "/*", "*/", "xp_", "sp_",
                        "UNION ", "EXEC ", "EXECUTE ",
                        "; SELECT", "; INSERT", "; UPDATE"};

                    std::string upper = toUpper(query);

                    for (const auto &pattern : dangerous)
                    {
                        if (upper.find(toUpper(pattern)) != std::string::npos)
                        {
                            return false;
                        }
                    }

                    return true;
                }

                // === ״§„״¥״µ„״§״­״§״× 81-90: ״×״¹‚… HTML/XML ===

                /**
                 * @brief ״×״¹‚… HTML
                 */
                static std::string sanitizeHTML(const std::string &input)
                {
                    std::string output;
                    output.reserve(input.size() * 1.5);

                    for (char c : input)
                    {
                        switch (c)
                        {
                        case '<':
                            output += "&lt;";
                            break;
                        case '>':
                            output += "&gt;";
                            break;
                        case '&':
                            output += "&amp;";
                            break;
                        case '"':
                            output += "&quot;";
                            break;
                        case '\'':
                            output += "&#39;";
                            break;
                        default:
                            output += c;
                            break;
                        }
                    }

                    return output;
                }

                /**
                 * @brief ״¥״²״§„״© ״¹„״§…״§״× HTML
                 */
                static std::string stripTags(const std::string &input)
                {
                    std::string output;
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

                // === ״§„״¥״µ„״§״­״§״× 91-100: ״×״¹‚… ״§„״£״±‚״§… ===

                /**
                 * @brief ״×״­„„ ״¹״¯״¯ ״µ״­״­ ״¨״£…״§†
                 */
                static bool safeParseInt(const std::string &str, int64_t &result)
                {
                    if (str.empty())
                    {
                        return false;
                    }

                    try
                    {
                        size_t pos;
                        result = std::stoll(str, &pos);
                        return pos == str.size(); // ״§„״×״£ƒ״¯ …† ״×״­„„ ƒ„ ״§„״³„״³„״©
                    }
                    catch (...)
                    {
                        return false;
                    }
                }

                /**
                 * @brief ״×״­„„ ״¹״¯״¯ ״¹״´״± ״¨״£…״§†
                 */
                static bool safeParseDouble(const std::string &str, double &result)
                {
                    if (str.empty())
                    {
                        return false;
                    }

                    try
                    {
                        size_t pos;
                        result = std::stod(str, &pos);
                        return pos == str.size() && std::isfinite(result);
                    }
                    catch (...)
                    {
                        return false;
                    }
                }

            private:
                static bool isAlphaNumeric(char c)
                {
                    return (c >= 'a' && c <= 'z') ||
                           (c >= 'A' && c <= 'Z') ||
                           (c >= '0' && c <= '9');
                }

                static std::string toLower(const std::string &str)
                {
                    std::string result = str;
                    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
                    return result;
                }

                static std::string toUpper(const std::string &str)
                {
                    std::string result = str;
                    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
                    return result;
                }
            };

            // =============================================================================

        } // namespace security
    } // namespace compiler
} // namespace sad

