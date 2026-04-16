/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: native_ui_storage.cpp
 * المسار: platform/android/src/native_ui_storage.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) تخزين وحدة IR مشترك بين المفسر والعارض الأصلي
 * @brief (EN) Shared IR module storage between interpreter and native renderer
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "native_ui_storage.h"

namespace {
    std::shared_ptr<sad::ui::IRModule> s_pendingModule;
    bool s_hasPendingUI = false;
}

void sad_native_ui_store(std::shared_ptr<sad::ui::IRModule> module) {
    s_pendingModule = std::move(module);
    s_hasPendingUI = true;
}

std::shared_ptr<sad::ui::IRModule> sad_native_ui_get() {
    return s_pendingModule;
}

bool sad_native_ui_has() {
    return s_hasPendingUI;
}

void sad_native_ui_clear() {
    s_pendingModule.reset();
    s_hasPendingUI = false;
}
