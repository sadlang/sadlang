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
 *
 * @warning (AR) ⚠️ عُرف إلزامي (مراجعة أميليا): BuiltinError يرث std::runtime_error،
 *          فأي `catch(const std::exception&)` أو `catch(...)` **يلفّ استدعاء دالة مضمنة
 *          أخرى** سيبتلعه قبل بلوغ callNative (نقطة الالتقاط). لتفادي ذلك في مثل تلك
 *          الكتل، أعِد رميه صراحةً قبل الالتقاط الواسع:
 *              catch (const ::Sad::Errors::BuiltinError&) { throw; }
 *          (تدقيق 2026-06-11: الكتل الواسعة الحالية تلفّ عمليات منخفضة المستوى
 *           — stoi/تحميل مكتبة/mutex — لا استدعاءات دوال مضمنة، فلا ابتلاع فعلي.)
 * @warning (EN) ⚠️ Convention: BuiltinError derives from std::runtime_error, so a broad
 *          `catch(std::exception)`/`catch(...)` that wraps a *call to another builtin*
 *          will swallow it before it reaches callNative. Re-throw it first in such blocks.
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
