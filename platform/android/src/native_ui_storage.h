/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: native_ui_storage.h
 * المسار: platform/android/src/native_ui_storage.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) تخزين مشترك لوحدة IR بين ui_bridge والعارض الأصلي
 * @brief (EN) Shared storage for IR module between ui_bridge and native renderer
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_NATIVE_UI_STORAGE_H
#define SAD_NATIVE_UI_STORAGE_H

#include "sad_ui/ir.h"
#include <memory>

/// (AR) تخزين وحدة IR للعرض الأصلي لاحقاً
/// (EN) Store IR module for native rendering later
void sad_native_ui_store(std::shared_ptr<sad::ui::IRModule> module);

/// (AR) استرجاع وحدة IR المخزّنة
/// (EN) Get stored IR module
std::shared_ptr<sad::ui::IRModule> sad_native_ui_get();

/// (AR) هل يوجد واجهة أصلية معلّقة؟
/// (EN) Is there a pending native UI?
bool sad_native_ui_has();

/// (AR) مسح وحدة IR المخزّنة
/// (EN) Clear stored IR module
void sad_native_ui_clear();

#endif // SAD_NATIVE_UI_STORAGE_H
