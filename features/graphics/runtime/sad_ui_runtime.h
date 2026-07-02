/* ============================================================================
 * sad_ui_runtime.h — رأس مكتبة وقت التشغيل لواجهات ص
 * Sad UI Runtime Header — C API for LLVM-compiled UI programs
 * ============================================================================
 *
 * (AR) هذا الملف يُعرّف دوال C التي يستدعيها الكود المُترجم بـ LLVM.
 *      يربط بين مولّد شفرة الواجهة (UICodeGen) ومكتبة sad_ui.
 *
 * (EN) This file declares the C functions called by LLVM-compiled code.
 *      It bridges the UI codegen (UICodeGen) and the sad_ui library.
 *
 * الدوال المُعرّفة هنا تتطابق 1:1 مع emitUIRuntimeCall() في:
 *   compiler/src/backend/llvm/builders/platform/ui_ops.cpp
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
/* (م1-ج، توقيع البانِي) باني صفحة: يُنتج عنصرًا **طازجًا** عند كلّ رسم (تفاعليّة) */
typedef SadWidget (*SadPageBuilder)(void* data);
/* (م1-ج/Q5) مُحرِّر بيئة الإغلاق: يُستدعى عند إسقاط إدخال التنقّل (ملكيّة مُدارة) */
typedef void (*SadReleaseFn)(void* data);

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

/* ─── مصانع إضافيّة (م-مصانع) — تطابق تسجيل المفسّر (widget_builtins.cpp) ───
 * Extra factories closing the compiler↔interpreter gap. */
/** أيقونة(اسم) */
SadWidget sad_icon(const char* name);
/** زر_نصي(عنوان) */
SadWidget sad_text_button(const char* label);
/** شريط_إشعار(رسالة) */
SadWidget sad_snackbar(const char* message);
/** تلميح(نص) */
SadWidget sad_tooltip(const char* text);
/** منطقة_نص(تلميح) */
SadWidget sad_text_area(const char* hint);
/** شريط_تقدم(قيمة) */
SadWidget sad_progress(float value);
/** مقاس(عرض, ارتفاع) */
SadWidget sad_sized_box(float width, float height);
/** شبكة() */         SadWidget sad_grid(void);
/** وسط() */          SadWidget sad_center(void);
/** حشوة() */         SadWidget sad_padding(void);
/** محاذاة() */       SadWidget sad_align(void);
/** موسع() */         SadWidget sad_expanded(void);
/** مرن() */          SadWidget sad_flexible(void);
/** التفاف() */       SadWidget sad_wrap(void);
/** صندوق() */        SadWidget sad_box(void);
/** عرض_تمرير() */    SadWidget sad_scroll_view(void);
/** تنقل_سفلي() */    SadWidget sad_bottom_nav(void);
/** عمود_كسول() */    SadWidget sad_lazy_column(void);
/** صف_كسول() */      SadWidget sad_lazy_row(void);
/** قائمة() */        SadWidget sad_list_view(void);
/** درج() */          SadWidget sad_drawer(void);
/** منطقة_آمنة() */   SadWidget sad_safe_area(void);
/** سطح() */          SadWidget sad_surface(void);

/* ─── أثر المعدّلات الانسيابيّة (م-أ3ر، L1): خاصّيّة عامّة بالاسم ───
 * Generic named-property setters (mirror interpreter setIRProperty). */
/** .م(نص) → خاصّيّة نصّيّة */     void sad_set_prop_str(SadWidget w, const char* name, const char* value);
/** .م(صحيح) → خاصّيّة int64 */    void sad_set_prop_int(SadWidget w, const char* name, int64_t value);
/** .م(عشري) → خاصّيّة عدديّة */   void sad_set_prop_num(SadWidget w, const char* name, double value);
/** .م()/.م(منطقي) → منطقيّة */   void sad_set_prop_bool(SadWidget w, const char* name, int32_t value);
/** .عند_*(دالة) → ربط حدث */     void sad_add_event(SadWidget w, const char* name, SadCallback cb, void* data);

/* ─── سلسلة التحريك الانسيابيّة (م-أ3ر، L3) — نظائر WidgetBuilder ─── */
/** .حرّك(أنواع) — يبدأ سلسلة (مركّب بالفاصلة) */ void sad_anim_begin(SadWidget w, const char* typesCsv);
/** .مدة(ث) */                    void sad_anim_duration(SadWidget w, double seconds);
/** .منحنى(اسم) */                void sad_anim_easing(SadWidget w, const char* name);
/** .تأخير(ث) */                  void sad_anim_delay(SadWidget w, double seconds);
/** .تكرار(ن) */                  void sad_anim_repeat(SadWidget w, int32_t count);
/** .عكس_تلقائي([منطقي]) */       void sad_anim_auto_reverse(SadWidget w, int32_t reverse);

/* ─── خاصّيّة متعدّدة الوسائط (م-أ3ر): دمج بفواصل ─── */
void sad_prop_join_add_str(SadWidget w, const char* value);
void sad_prop_join_add_int(SadWidget w, int64_t value);
void sad_prop_join_add_num(SadWidget w, double value);
void sad_prop_join_add_bool(SadWidget w, int32_t value);
void sad_prop_join_commit(SadWidget w, const char* name);

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

/** تشغيل_تطبيق(عنصر) — حلقة سطح المكتب: نافذة + رسم + إرسال الأحداث (جسر فوق DesktopWindow) */
void sad_app_run(SadWidget root);

/** طباعة_شجرة(عنصر) — طباعة شجرة العناصر للتصحيح (منطق المكتبة المشترك) */
void sad_print_tree(SadWidget root);

/* ─── دوال الثيم (م-تحكّم) — جسرٌ فوق حالة الثيم المكتبيّة المشتركة مع المفسّر ─── */
/** تبديل_الثيم() — يبدّل بين الفاتح/الداكن */
void sad_toggle_theme(void);
/** وضع_داكن() — يضبط الثيم داكنًا */
void sad_set_dark(void);
/** وضع_فاتح() — يضبط الثيم فاتحًا */
void sad_set_light(void);
/** هل_داكن() — هل الثيم الحاليّ داكن؟ */
bool sad_is_dark(void);

/* ─── دوال التنقّل (م-تحكّم) — جسرٌ فوق مكدّس التنقّل المكتبيّ المشترك (nav.h) ─── */
/** انتقل(صفحة) — يدفع الحالية ويجعل الصفحة الجديدة حاليّة */
void sad_navigate(SadWidget page);
/** عودة() — يعود للصفحة السابقة؛ يُرجع false إن لا صفحة */
bool sad_navigate_back(void);
/** عودة_للبداية() — يعود لأوّل صفحة ويُفرغ المكدّس */
void sad_navigate_root(void);
/** انتقل_بتحريك(صفحة, نوع, مدة) — تنقّل مع انتقال بصريّ (م2) */
void sad_navigate_transition(SadWidget page, const char* transType, float durationSec);
/** عودة_بتحريك(نوع, مدة) — عودة مع انتقال بصريّ (م2)؛ يُرجع 1 إن نجحت العودة، 0 إن لا صفحة */
int sad_navigate_back_transition(const char* transType, float durationSec);
/** استبدل(صفحة) — يبدّل الحالية دون دفع */
void sad_replace_page(SadWidget page);
/** انتقل(دالّة_بناء) — نموذج البانِي (م1-ج): يخزّن بانيًا يُستدعى عند كلّ رسم ⇒
 *  تفاعليّةٌ كاملة داخل الصفحة المُنتقَل إليها (نظير Flutter routes) */
void sad_navigate_builder(SadPageBuilder build, void* data, SadReleaseFn release);
/** استبدل(دالّة_بناء) — نموذج البانِي: يبدّل الحالية ببانٍ دون دفع */
void sad_replace_page_builder(SadPageBuilder build, void* data, SadReleaseFn release);
/** انتقل_بتحريك(دالّة_بناء, نوع, مدة) — نموذج البانِي + انتقال بصريّ (م1-ج + م2) */
void sad_navigate_transition_builder(SadPageBuilder build, void* data, SadReleaseFn release,
                                     const char* transType, float durationSec);
/** عدد_الصفحات() — عمق مكدّس التنقّل */
long long sad_page_count(void);
/** الصفحة_الحالية() — الصفحة الحالية (SadWidget) من مكدّس التنقّل (حارس بنيويّ) */
SadWidget sad_current_page(void);
/** انتقل_بتحريك_كامل(صفحة, دخول, خروج, مدة) — تنقّل (لقطة) + انتقال دخول على الصفحة الجديدة.
 *  (انتقال الخروج على الصفحة القديمة تحسينٌ بصريٌّ حيّ لا تستهلكه حلقة نافذة المترجم بعدُ،
 *   نظير sad_navigate_transition؛ أثر المكدّس — العمق/الصفحة الحاليّة — مطابقٌ بالبناء.) */
void sad_navigate_exit_transition(SadWidget page, const char* entryType,
                                  const char* exitType, float durationSec);
/** انتقل_بتحريك_كامل(دالّة_بناء, دخول, خروج, مدة) — نموذج البانِي (م1-ج) + انتقال دخول */
void sad_navigate_exit_transition_builder(SadPageBuilder build, void* data, SadReleaseFn release,
                                          const char* entryType, const char* exitType,
                                          float durationSec);

/* ─── م-تحكّم UICore: الحالة والنافذة (شريحة إكمال corui) ─── */
/** تحديث_حالة() / عين_الحالة(دالّة) — يطلب إعادة رسمٍ (يعلّم مكدّس التنقّل dirty). عين_الحالة
 *  في المترجم = استدعاء ثانك الإغلاق تزامنيًّا (دالّة التحديث) ثمّ sad_update_state، فلا دالّة
 *  runtime مستقلّة لها. نظير المفسّر (rebuildUI). */
void sad_update_state(void);
/** عنوان_النافذة(نص) — يطلب تغيير عنوان النافذة عبر المتحكّم المشترك (window_control) */
void sad_set_window_title(const char* title);
/** أغلق_النافذة() — يطلب إغلاق النافذة عبر المتحكّم المشترك (window_control) */
void sad_close_window(void);
/** توليد_ويب(عنصر, عنوان؟) — يولّد HTML من شجرة العنصر عبر HtmlCodegen المكتبيّ. يُرجع نصًّا
 *  مخصّصًا في الكومة يملكه المستدعي (نظير sad_readline). عنوانٌ فارغ/null ⇒ العنوان الافتراضيّ
 *  من HtmlCodegenOptions (مطابقٌ للمفسّر). */
char* sad_generate_web(SadWidget root, const char* title);

/** دمّر_تطبيق(تطبيق) */
void sad_app_destroy(SadApp app);

/** دمّر_عنصر(عنصر) */
void sad_widget_destroy(SadWidget widget);

#ifdef __cplusplus
}
#endif

#endif /* SAD_UI_RUNTIME_H */
