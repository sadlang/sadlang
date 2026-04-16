/**
 * @file ui_builtins.cpp
 * @brief (AR) نقطة الدخول لتسجيل جميع دوال واجهات المستخدم — يستدعي الوحدات الفرعية
 * @brief (EN) Entry point for UI builtin registration — delegates to modular sub-files
 *
 * بعد إعادة الهيكلة: هذا الملف يستدعي 10 وحدات فرعية بدلاً من تسجيل كل شيء هنا.
 * الوحدات الفرعية: core, timer, storage, dialog, audio, io, device, network, crypto, platform
 */

#include "ui_builtins.h"
#include "ui_module_builtins.h"

namespace Sad {
namespace Interpreter {

void registerUIBuiltins(Interpreter& interpreter) {
    // تستدعي جميع الوحدات الفرعية لتسجيل الدوال
    registerUICoreBuiltins(interpreter);      // المحرك + التنقل + السمة + الحالة + النافذة
    registerUIStateBuiltins(interpreter);     // الحالة التفاعلية (StateStore)
    registerUITimerBuiltins(interpreter);     // المؤقتات
    registerUIStorageBuiltins(interpreter);   // التخزين + الحافظة
    registerUIDialogBuiltins(interpreter);    // الحوارات + الإشعارات
    registerUIAudioBuiltins(interpreter);     // الصوت
    registerUIIOBuiltins(interpreter);        // الملفات + منتقي الملفات + لقطة الشاشة
    registerUIDeviceBuiltins(interpreter);    // الجهاز + المؤشر + الوقت + البطارية
    registerUINetworkBuiltins(interpreter);   // الشبكة + JSON
    registerUICryptoBuiltins(interpreter);    // التشفير + قاعدة البيانات
    registerUIPlatformBuiltins(interpreter);  // الموقع + الكاميرا + المستشعرات + الخرائط + ...
}

} // namespace Interpreter
} // namespace Sad