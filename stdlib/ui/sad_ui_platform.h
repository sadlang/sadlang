/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_ui_platform.h — كشف المنصة تلقائياً وتهيئة Backend المناسب
 *  Platform Auto-Detection & Backend Initialization
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  يُستدعى مرة واحدة عند بدء البرنامج (قبل أي استخدام للواجهة).
 *  يكتشف تلقائياً: Android / WASM / Desktop ويُنشئ Backend المناسب.
 *
 *  الاستخدام:
 *    #include "ui/sad_ui_platform.h"
 *    sad::unified::initPlatformUI();  // مرة واحدة في main()
 *
 *  أو يدوياً:
 *    sad::unified::SadUI::instance().setBackend(
 *        std::make_unique<sad::unified::DesktopBackend>());
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "sad_ui_unified.h"

namespace sad {
namespace unified {

/**
 * @brief (AR) يكتشف المنصة تلقائياً ويُهيّئ Backend المناسب
 * @brief (EN) Auto-detect platform and initialize the appropriate backend
 *
 * الأولوية:
 *   1. __ANDROID__     → AndroidBackend (EGL + GLES3)
 *   2. __EMSCRIPTEN__  → WasmBackend    (Canvas2D)
 *   3. غير ذلك         → DesktopBackend (SDL2)
 *
 * @return true إذا تمت التهيئة بنجاح
 */
bool initPlatformUI();

/**
 * @brief (AR) يُعيد اسم المنصة الحالية
 * @brief (EN) Returns the current platform name
 */
std::string currentPlatformName();

/**
 * @brief (AR) هل تمت تهيئة الواجهة الموحدة؟
 * @brief (EN) Is the unified UI system initialized?
 */
bool isUIReady();

}} // namespace sad::unified
