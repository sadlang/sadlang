/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: system_bridge.cpp
 * المسار: features/graphics/core/src/system_bridge.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ الدوال المشتركة لواجهة الجسر + المصنع.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/system_bridge.h"

// التضمينات الشرطية لكل منصة
#ifdef SAD_UI_USE_SDL2
#include "sad_ui/desktop/system_bridge.h"
#endif

// Web/Android/iOS/macOS bridges included conditionally
// (يتم تضمينها حسب المنصة المستهدفة)

namespace sad {
namespace ui {

std::unique_ptr<SystemBridge> SystemBridge::createForCurrentPlatform() {
#if defined(__EMSCRIPTEN__)
    return createForPlatform(PlatformType::Web);
#elif defined(__ANDROID__)
    return createForPlatform(PlatformType::Android);
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS
        return createForPlatform(PlatformType::iOS);
    #else
        return createForPlatform(PlatformType::macOS);
    #endif
#else
    return createForPlatform(PlatformType::Desktop);
#endif
}

std::unique_ptr<SystemBridge> SystemBridge::createForPlatform(PlatformType type) {
    switch (type) {
#ifdef SAD_UI_USE_SDL2
        case PlatformType::Desktop:
            return std::make_unique<desktop::DesktopSystemBridge>();
#endif
        // المنصات الأخرى تُفعّل حسب شروط البناء
        default:
            return nullptr;
    }
}

} // namespace ui
} // namespace sad
