/**
 * @file builtin_registry_part39_unified.cpp
 * @brief (AR) الجزء 39: تسجيل دوال الواجهة الموحدة (SadUI Unified)
 * @brief (EN) Part 39: Register unified UI platform functions
 *
 *  يسجل:
 *    - هيئ_واجهة_موحدة() / init_unified_ui — تهيئة الطبقة الموحدة
 *    - المنصة() / get_platform — إرجاع اسم المنصة الحالية
 *    - هل_سطح_المكتب() / is_desktop — هل نعمل على سطح المكتب؟
 *    - هل_اندرويد() / is_android — هل نعمل على أندرويد؟
 *    - هل_ويب() / is_web — هل نعمل على الويب (WASM)؟
 *    - اسم_المحرك() / backend_name — اسم Backend الفعال
 *
 *  تلقائياً: عند تحميل وحدة "واجهات_موحدة" يتم استدعاء initPlatformUI()
 */

#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <string>
#include <vector>

// ───── Unified UI headers ─────
#include "ui/sad_ui_platform.h"
#include "ui/sad_ui_unified.h"

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart39(Interpreter& interpreter) {
    using Args = const std::vector<std::shared_ptr<Data::Value>>&;

    // تهيئة الطبقة الموحدة تلقائياً عند تحميل هذا الجزء
    sad::unified::initPlatformUI();

    // ═══════════════════════════════════════════════════════════════════
    // (AR) هيئ_واجهة_موحدة — تهيئة صريحة (يمكن استدعاؤها من ص)
    // (EN) init_unified_ui — explicit initialization
    // ═══════════════════════════════════════════════════════════════════
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x8a\xd8\xa6_\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9_\xd9\x85\xd9\x88\xd8\xad\xd8\xaf\xd8\xa9", // هيئ_واجهة_موحدة
        [](Args args) -> std::shared_ptr<Data::Value> {
            bool ok = sad::unified::initPlatformUI();
            return std::make_shared<Data::Value>(ok);
        });
    interpreter.getFunctionManager().registerBuiltinFunction(
        "init_unified_ui",
        [](Args args) -> std::shared_ptr<Data::Value> {
            bool ok = sad::unified::initPlatformUI();
            return std::make_shared<Data::Value>(ok);
        });

    // ═══════════════════════════════════════════════════════════════════
    // (AR) المنصة — إرجاع اسم المنصة (Windows / Android / WASM/Web / ...)
    // (EN) get_platform — returns platform name
    // ═══════════════════════════════════════════════════════════════════
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9", // المنصة
        [](Args args) -> std::shared_ptr<Data::Value> {
            return std::make_shared<Data::Value>(sad::unified::currentPlatformName());
        });
    interpreter.getFunctionManager().registerBuiltinFunction(
        "get_platform",
        [](Args args) -> std::shared_ptr<Data::Value> {
            return std::make_shared<Data::Value>(sad::unified::currentPlatformName());
        });

    // ═══════════════════════════════════════════════════════════════════
    // (AR) هل_سطح_المكتب — true إذا نعمل على Windows/macOS/Linux
    // ═══════════════════════════════════════════════════════════════════
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd8\xb3\xd8\xb7\xd8\xad_\xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8", // هل_سطح_المكتب
        [](Args args) -> std::shared_ptr<Data::Value> {
#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
            return std::make_shared<Data::Value>(false);
#else
            return std::make_shared<Data::Value>(true);
#endif
        });
    interpreter.getFunctionManager().registerBuiltinFunction(
        "is_desktop",
        [](Args args) -> std::shared_ptr<Data::Value> {
#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
            return std::make_shared<Data::Value>(false);
#else
            return std::make_shared<Data::Value>(true);
#endif
        });

    // ═══════════════════════════════════════════════════════════════════
    // (AR) هل_اندرويد — true إذا نعمل على Android
    // ═══════════════════════════════════════════════════════════════════
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd8\xa7\xd9\x86\xd8\xaf\xd8\xb1\xd9\x88\xd9\x8a\xd8\xaf", // هل_اندرويد
        [](Args args) -> std::shared_ptr<Data::Value> {
#ifdef __ANDROID__
            return std::make_shared<Data::Value>(true);
#else
            return std::make_shared<Data::Value>(false);
#endif
        });
    interpreter.getFunctionManager().registerBuiltinFunction(
        "is_android",
        [](Args args) -> std::shared_ptr<Data::Value> {
#ifdef __ANDROID__
            return std::make_shared<Data::Value>(true);
#else
            return std::make_shared<Data::Value>(false);
#endif
        });

    // ═══════════════════════════════════════════════════════════════════
    // (AR) هل_ويب — true إذا نعمل على WASM/Web
    // ═══════════════════════════════════════════════════════════════════
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd9\x88\xd9\x8a\xd8\xa8", // هل_ويب
        [](Args args) -> std::shared_ptr<Data::Value> {
#ifdef __EMSCRIPTEN__
            return std::make_shared<Data::Value>(true);
#else
            return std::make_shared<Data::Value>(false);
#endif
        });
    interpreter.getFunctionManager().registerBuiltinFunction(
        "is_web",
        [](Args args) -> std::shared_ptr<Data::Value> {
#ifdef __EMSCRIPTEN__
            return std::make_shared<Data::Value>(true);
#else
            return std::make_shared<Data::Value>(false);
#endif
        });

    // ═══════════════════════════════════════════════════════════════════
    // (AR) اسم_المحرك — اسم Backend المُفعّل (SDL2 / Android-GLES3 / WASM-Canvas2D)
    // (EN) backend_name — active backend's name
    // ═══════════════════════════════════════════════════════════════════
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd8\xb3\xd9\x85_\xd8\xa7\xd9\x84\xd9\x85\xd8\xad\xd8\xb1\xd9\x83", // اسم_المحرك
        [](Args args) -> std::shared_ptr<Data::Value> {
            auto* b = sad::unified::SadUI::instance().backend();
            if (b) return std::make_shared<Data::Value>(b->backendName());
            return std::make_shared<Data::Value>(std::string("غير مهيأ"));
        });
    interpreter.getFunctionManager().registerBuiltinFunction(
        "backend_name",
        [](Args args) -> std::shared_ptr<Data::Value> {
            auto* b = sad::unified::SadUI::instance().backend();
            if (b) return std::make_shared<Data::Value>(b->backendName());
            return std::make_shared<Data::Value>(std::string("not initialized"));
        });

} // registerBuiltinsPart39

} // namespace Interpreter
} // namespace Sad
