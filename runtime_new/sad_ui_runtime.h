/* ============================================================================
 * sad_ui_runtime.h — رأس مكتبة وقت التشغيل لواجهات ص
 * Sad UI Runtime Header — C API for LLVM-compiled UI programs
 * ============================================================================
 *
 * (AR) هذا الملف يُعرّف دوال C التي يستدعيها الكود المُترجم بـ LLVM.
 *      يربط بين llvm_codegen_ui.cpp ومكتبة sad_ui.
 *
 * (EN) This file declares the C functions called by LLVM-compiled code.
 *      It bridges llvm_codegen_ui.cpp and the sad_ui library.
 *
 * الدوال المُعرّفة هنا تتطابق 1:1 مع emitUIRuntimeCall() في:
 *   compiler_new/src/backend/llvm/llvm_codegen_ui.cpp
 *
 * ============================================================================
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ============================================================================
 */

#ifndef SAD_UI_RUNTIME_H
#define SAD_UI_RUNTIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ─── أنواع مبهمة / Opaque Types ─── */
typedef void* SadWidget;
typedef void* SadApp;
typedef void (*SadCallback)(void* userData);

/* ═══════════════════════════════════════════════════════════════════
 * 1. مصانع العناصر / Widget Factories
 * ═══════════════════════════════════════════════════════════════════ */

/** عمود() — حاوية عمودية */
SadWidget sad_column(void);

/** صف() — حاوية أفقية */
SadWidget sad_row(void);

/** رصّة() — حاوية متراكبة */
SadWidget sad_stack(void);

/** حاوية() — حاوية عامة */
SadWidget sad_container(void);

/** نص(محتوى) — عنصر نصي */
SadWidget sad_text(const char* text);

/** نص_منسق(محتوى, حجم, ر, خ, ز, ش) — نص بتنسيق */
SadWidget sad_text_styled(const char* text, float fontSize,
                          float r, float g, float b, float a);

/** زر(عنوان, دالة_ضغط, بيانات) */
SadWidget sad_button(const char* label, SadCallback onTap, void* userData);

/** زر_نوع(عنوان, نوع, ر,خ,ز,ش, دالة, بيانات) */
SadWidget sad_button_variant(const char* label, int32_t variant,
                             float r, float g, float b, float a,
                             SadCallback onTap, void* userData);

/** زر_أيقونة(أيقونة, دالة, بيانات) */
SadWidget sad_icon_button(const char* icon, SadCallback onTap, void* userData);

/** زر_عائم(أيقونة, ر,خ,ز,ش, دالة, بيانات) */
SadWidget sad_fab(const char* icon,
                  float r, float g, float b, float a,
                  SadCallback onTap, void* userData);

/** حقل_نص(تلميح, دالة, بيانات) */
SadWidget sad_text_field(const char* hint, SadCallback onChange, void* userData);

/** مربع_اختيار(دالة, بيانات) */
SadWidget sad_checkbox(SadCallback onChanged, void* userData);

/** مفتاح_تبديل(دالة, بيانات) */
SadWidget sad_switch_toggle(SadCallback onChanged, void* userData);

/** منزلق(أدنى, أقصى, دالة, بيانات) */
SadWidget sad_slider(float minVal, float maxVal, SadCallback onChanged, void* userData);

/** بطاقة() */
SadWidget sad_card(void);

/** هيكل() — scaffold */
SadWidget sad_scaffold(void);

/** شريط_تطبيق(عنوان) */
SadWidget sad_app_bar(const char* title);

/** فاصل() — spacer */
SadWidget sad_spacer(void);

/** خط_فاصل() — divider */
SadWidget sad_divider(void);

/** حوار(عنوان, رسالة) */
SadWidget sad_dialog(const char* title, const char* message);

/** صورة(مصدر) */
SadWidget sad_image(const char* source);

/* ═══════════════════════════════════════════════════════════════════
 * 2. إدارة الشجرة / Tree Management
 * ═══════════════════════════════════════════════════════════════════ */

/** أضف_ابن(أب, ابن) */
void sad_add_child(SadWidget parent, SadWidget child);

/** أزل_ابن(أب, ابن) */
void sad_remove_child(SadWidget parent, SadWidget child);

/** امسح_الأبناء(عنصر) */
void sad_clear_children(SadWidget widget);

/* ═══════════════════════════════════════════════════════════════════
 * 3. ضبط الخصائص / Property Setters
 * ═══════════════════════════════════════════════════════════════════ */

void sad_set_text(SadWidget widget, const char* text);
void sad_set_size(SadWidget widget, float width, float height);
void sad_set_flex(SadWidget widget, float flex);
void sad_set_background(SadWidget widget, float r, float g, float b, float a);
void sad_set_foreground(SadWidget widget, float r, float g, float b, float a);
void sad_set_spacing(SadWidget widget, float spacing);
void sad_set_padding(SadWidget widget, float top, float right, float bottom, float left);
void sad_set_alignment(SadWidget widget, int32_t mainAxis, int32_t crossAxis);
void sad_set_border(SadWidget widget, float width);
void sad_set_elevation(SadWidget widget, float elevation);
void sad_set_opacity(SadWidget widget, float opacity);
void sad_set_visibility(SadWidget widget, int32_t visible);

/* ═══════════════════════════════════════════════════════════════════
 * 4. إدارة التطبيق / App Management
 * ═══════════════════════════════════════════════════════════════════ */

/** انشئ_تطبيق() */
SadApp sad_app_create(void);

/** عين_جذر(تطبيق, عنصر) */
void sad_app_set_root(SadApp app, SadWidget root);

/** خطط(تطبيق, عرض, ارتفاع) */
void sad_app_layout(SadApp app, float width, float height);

/** ارسم(تطبيق) */
void sad_app_render(SadApp app);

/** دمّر_تطبيق(تطبيق) */
void sad_app_destroy(SadApp app);

/** دمّر_عنصر(عنصر) */
void sad_widget_destroy(SadWidget widget);

#ifdef __cplusplus
}
#endif

#endif /* SAD_UI_RUNTIME_H */
