/**
 * @file builtin_module_platform.cpp
 * @brief (AR) وحدة المنصة — كشف نظام التشغيل
 * @brief (EN) Platform module — OS detection
 *
 * @details
 * (AR) الأقسام:
 *   1. كشف المنصة الحالية
 *
 * @note يتطلب: builtin_common.h (الـ headers المشتركة)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPlatform(Interpreter& interpreter) {
    // (AR) EM-CPP: التوقيع الموحَّد — دوال المنصة لا تستخدم الوسائط (المعامل غير مُسمّى).
    using Args = Sad::Interpreter::BuiltinContext &;

    // المنصة / get_platform
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9",
        [](Args) -> std::shared_ptr<Data::Value> {
#if defined(_WIN32)
            return std::make_shared<Data::Value>(std::string("Windows"));
#elif defined(__ANDROID__)
            return std::make_shared<Data::Value>(std::string("Android"));
#elif defined(__EMSCRIPTEN__)
            return std::make_shared<Data::Value>(std::string("WASM/Web"));
#elif defined(__linux__)
            return std::make_shared<Data::Value>(std::string("Linux"));
#elif defined(__APPLE__)
            return std::make_shared<Data::Value>(std::string("macOS"));
#else
            return std::make_shared<Data::Value>(std::string("Unknown"));
#endif
        });

    // هل_سطح_المكتب / is_desktop
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd8\xb3\xd8\xb7\xd8\xad_\xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8",
        [](Args) -> std::shared_ptr<Data::Value> {
#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
            return std::make_shared<Data::Value>(false);
#else
            return std::make_shared<Data::Value>(true);
#endif
        });

    // هل_اندرويد / is_android
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd8\xa7\xd9\x86\xd8\xaf\xd8\xb1\xd9\x88\xd9\x8a\xd8\xaf",
        [](Args) -> std::shared_ptr<Data::Value> {
#ifdef __ANDROID__
            return std::make_shared<Data::Value>(true);
#else
            return std::make_shared<Data::Value>(false);
#endif
        });

    // هل_ويب / is_web
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd9\x88\xd9\x8a\xd8\xa8",
        [](Args) -> std::shared_ptr<Data::Value> {
#ifdef __EMSCRIPTEN__
            return std::make_shared<Data::Value>(true);
#else
            return std::make_shared<Data::Value>(false);
#endif
        });
}

} // namespace Interpreter
} // namespace Sad