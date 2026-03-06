/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_ui_platform.cpp — كشف المنصة وتهيئة Backend
 *  Platform Detection & Backend Initialization
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui_platform.h"

// ─── include الـ backends المتاحة حسب المنصة ──────────────────────────────

#ifdef __ANDROID__
    #include "sad_backend_android.h"
#elif defined(__EMSCRIPTEN__)
    #include "sad_backend_wasm.h"
#else
    #include "sad_backend_desktop.h"
#endif

#include <memory>

namespace sad {
namespace unified {

bool initPlatformUI() {
    auto& ui = SadUI::instance();
    if (ui.isInitialized()) {
        return true; // تمت التهيئة سابقاً
    }

#ifdef __ANDROID__
    ui.setBackend(std::make_unique<AndroidBackend>());
#elif defined(__EMSCRIPTEN__)
    ui.setBackend(std::make_unique<WasmBackend>());
#else
    ui.setBackend(std::make_unique<DesktopBackend>());
#endif

    return ui.isInitialized();
}

std::string currentPlatformName() {
#ifdef __ANDROID__
    return "Android";
#elif defined(__EMSCRIPTEN__)
    return "WASM/Web";
#elif defined(_WIN32)
    return "Windows";
#elif defined(__APPLE__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#else
    return "Unknown";
#endif
}

bool isUIReady() {
    return SadUI::instance().isInitialized();
}

}} // namespace sad::unified
