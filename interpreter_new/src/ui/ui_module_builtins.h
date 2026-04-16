/**
 * @file ui_module_builtins.h
 * @brief (AR) تصريحات دوال تسجيل وحدات واجهات المستخدم المنفصلة
 * @brief (EN) Declarations for modular UI builtin registration functions
 *
 * كل وحدة تُسجّل مجموعة دوال مترابطة — بدلاً من ملف واحد ضخم.
 * Each module registers a cohesive group of functions.
 */

#ifndef SAD_UI_MODULE_BUILTINS_H
#define SAD_UI_MODULE_BUILTINS_H

namespace Sad {
namespace Interpreter {

class Interpreter;

// ═══════════════════════════════════════════════════════════════════════════════
// وحدات واجهات المستخدم — UI Modules
// ═══════════════════════════════════════════════════════════════════════════════

/// محرك الواجهات الأساسي + التنقل + السمة + الحالة + النافذة
void registerUICoreBuiltins(Interpreter& interpreter);

/// نظام الحالة التفاعلية (StateStore) — عرّف/عيّن/اقرأ/راقب/تجميع
void registerUIStateBuiltins(Interpreter& interpreter);

/// نظام المؤقتات (setTimeout, setInterval)
void registerUITimerBuiltins(Interpreter& interpreter);

/// نظام التخزين المحلي + الحافظة
void registerUIStorageBuiltins(Interpreter& interpreter);

/// نظام الحوارات + الإشعارات + الرسائل المنبثقة
void registerUIDialogBuiltins(Interpreter& interpreter);

/// نظام الصوت + التحكم بالمستوى
void registerUIAudioBuiltins(Interpreter& interpreter);

/// نظام الملفات + اختيار الملفات + لقطة الشاشة + سحب وإفلات
void registerUIIOBuiltins(Interpreter& interpreter);

/// معلومات الجهاز + المؤشر + الروابط + الوقت + الاهتزاز + السجل + البطارية
void registerUIDeviceBuiltins(Interpreter& interpreter);

/// نظام الشبكة + JSON
void registerUINetworkBuiltins(Interpreter& interpreter);

/// نظام التشفير + قاعدة البيانات
void registerUICryptoBuiltins(Interpreter& interpreter);

/// الأنظمة المنصّية: الموقع، الكاميرا، المستشعرات، المشاركة، الأذونات،
/// السمات، التدويل، الوصولية، التخطيط المتجاوب، الطباعة، المقاييس الحيوية،
/// الإيماءات، الخرائط، الرسم الحر
void registerUIPlatformBuiltins(Interpreter& interpreter);

} // namespace Interpreter
} // namespace Sad

#endif // SAD_UI_MODULE_BUILTINS_H
