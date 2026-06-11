/**
 * @file builtin_error.h
 * @brief (AR) حامل خطأ للطبقة الأدنى (Sad::StdLib::BuiltinFunctions) — يحمل ErrorCode
 *             + placeholders بلا موقع؛ يلتقطه موزّع المفسر ويرندره بموقع الاستدعاء.
 * @brief (EN) Lower-layer error carrier for Sad::StdLib::BuiltinFunctions — carries an
 *             ErrorCode + placeholders without a position; the interpreter dispatcher
 *             catches it and renders via the catalog at the call-site position.
 *
 * @details
 * (AR) الطبقة الأدنى (shared/builtins) لا تملك Position ولا BuiltinContext (طبقية).
 *      بدل `throw std::runtime_error("نص")`، ترمي `BuiltinError(code, placeholders)`؛
 *      الموزّع (الذي يملك node.position) يلتقطه ويستدعي throwRuntime → الكتالوج.
 *      هذا يحفظ دقّة الموقع ويوحّد الرسائل من YAML.
 */
#ifndef SAD_ERRORS_BUILTIN_ERROR_H
#define SAD_ERRORS_BUILTIN_ERROR_H

#include <map>
#include <stdexcept>
#include <string>
#include <utility>

#include "error_codes.h" // Sad::Errors::ErrorCode

namespace Sad
{
    namespace Errors
    {

        /**
         * @class BuiltinError
         * @brief (AR) استثناء حامل (code + placeholders) للطبقة الأدنى.
         * @brief (EN) Carrier exception (code + placeholders) for the lower layer.
         */
        class BuiltinError : public std::runtime_error
        {
        public:
            ErrorCode code;
            std::map<std::string, std::string> placeholders;

            explicit BuiltinError(ErrorCode c,
                                  std::map<std::string, std::string> ph = {})
                : std::runtime_error("BuiltinError"),
                  code(c),
                  placeholders(std::move(ph))
            {
            }
        };

        /// (AR) مساعد رمي مختصر. / (EN) Convenience thrower.
        [[noreturn]] inline void throwBuiltin(ErrorCode code,
                                              std::map<std::string, std::string> ph = {})
        {
            throw BuiltinError(code, std::move(ph));
        }

    } // namespace Errors
} // namespace Sad

#endif // SAD_ERRORS_BUILTIN_ERROR_H
