/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  واجهة عربية لنظام واجهات ص
 *  Arabic API for Sad UI System
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 *  هذا الملف يوفر دوال عربية للمبرمجين لإنشاء واجهات رسومية
 *  يُستخدم في runtime لغة ص لربط الكود العربي بنظام الرسم
 * 
 *  مثال:
 *  ```ص
 *  متغير زر = أنشئ_زر("اضغط هنا")
 *  عند_الضغط(زر, دالة_المعالجة)
 *  ```
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_ARABIC_H
#define SAD_UI_ARABIC_H

#include "sad_flutter_ui.h"

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  الـ API العربي
// ═══════════════════════════════════════════════════════════════════════════════

// ─── إنشاء التطبيق ───

/**
 * أنشئ_تطبيق - إنشاء تطبيق جديد
 * @param عرض عرض الشاشة
 * @param ارتفاع ارتفاع الشاشة
 */
#define أنشئ_تطبيق(عرض, ارتفاع) sadapp_create(عرض, ارتفاع, 2.0f)

/**
 * دمر_التطبيق - تدمير التطبيق
 */
#define دمر_التطبيق(تطبيق) sadapp_destroy(تطبيق)

/**
 * عيّن_الجذر - تعيين العنصر الجذر
 */
#define عيّن_الجذر(تطبيق, عنصر) sadapp_set_root(تطبيق, عنصر)

/**
 * ارسم_الإطار - رسم إطار واحد
 */
#define ارسم_الإطار(تطبيق) sadapp_render(تطبيق)

/**
 * شغّل_الإطار - تشغيل دورة واحدة
 */
#define شغّل_الإطار(تطبيق) sadapp_run_frame(تطبيق)

// ─── الحاويات ───

/**
 * حاوية - إنشاء حاوية بسيطة
 */
#define حاوية() sad_container()

/**
 * عمود - تخطيط عمودي (من أعلى لأسفل)
 */
#define عمود() sad_column()

/**
 * صف - تخطيط أفقي (من اليمين لليسار)
 */
#define صف() sad_row()

/**
 * مكدس - تخطيط متراكب
 */
#define مكدس() sad_stack()

/**
 * بطاقة - بطاقة مرتفعة
 */
#define بطاقة() sad_card()

/**
 * هيكل - هيكل الصفحة الكامل
 */
#define هيكل() sad_scaffold()

/**
 * قائمة - قائمة قابلة للتمرير
 */
#define قائمة() sad_listview()

/**
 * تمرير - عرض قابل للتمرير
 */
#define تمرير() sad_scrollview()

// ─── عناصر النص ───

/**
 * نص - إنشاء نص
 */
#define نص(محتوى) sad_text(محتوى)

/**
 * نص_منسق - نص بتنسيق مخصص
 */
#define نص_منسق(محتوى, نمط) sad_text_styled(محتوى, نمط)

/**
 * عنوان - نص عنوان كبير
 */
static inline SadWidget* عنوان(const char* محتوى) {
    return sad_text_styled(محتوى, SADTEXT_HEADLINE5);
}

/**
 * عنوان_فرعي - نص عنوان فرعي
 */
static inline SadWidget* عنوان_فرعي(const char* محتوى) {
    return sad_text_styled(محتوى, SADTEXT_HEADLINE6);
}

/**
 * فقرة - نص فقرة عادية
 */
static inline SadWidget* فقرة(const char* محتوى) {
    return sad_text_styled(محتوى, SADTEXT_BODY1);
}

/**
 * تعليق - نص صغير
 */
static inline SadWidget* تعليق(const char* محتوى) {
    return sad_text_styled(محتوى, SADTEXT_CAPTION);
}

// ─── الأزرار ───

/**
 * زر - زر مرتفع
 */
#define زر(عنوان, دالة, بيانات) sad_button(عنوان, دالة, بيانات)

/**
 * زر_ملون - زر بلون مخصص
 */
#define زر_ملون(عنوان, لون, دالة, بيانات) sad_elevated_button(عنوان, لون, دالة, بيانات)

/**
 * زر_نص - زر نصي بدون خلفية
 */
#define زر_نص(عنوان, دالة, بيانات) sad_text_button(عنوان, دالة, بيانات)

/**
 * زر_أيقونة - زر بأيقونة
 */
#define زر_أيقونة(أيقونة, دالة, بيانات) sad_icon_button(أيقونة, دالة, بيانات)

/**
 * زر_عائم - زر عائم (FAB)
 */
#define زر_عائم(أيقونة, لون, دالة, بيانات) sad_fab(أيقونة, لون, دالة, بيانات)

// ─── حقول الإدخال ───

/**
 * حقل_نص - حقل إدخال نصي
 */
#define حقل_نص(عنوان, تلميح) sad_textfield(عنوان, تلميح)

/**
 * مربع_اختيار - مربع اختيار
 */
#define مربع_اختيار(محدد, دالة, بيانات) sad_checkbox(محدد, دالة, بيانات)

/**
 * مفتاح - مفتاح تبديل
 */
#define مفتاح(قيمة, دالة, بيانات) sad_switch(قيمة, دالة, بيانات)

/**
 * شريط_تمرير - شريط تمرير
 */
#define شريط_تمرير(قيمة, أدنى, أقصى, دالة, بيانات) sad_slider(قيمة, أدنى, أقصى, دالة, بيانات)

// ─── الأيقونات والصور ───

/**
 * أيقونة - أيقونة
 */
#define أيقونة(رمز, حجم, لون) sad_icon(رمز, حجم, لون)

/**
 * صورة - صورة
 */
#define صورة(مسار) sad_image(مسار)

/**
 * صورة_رمزية - صورة دائرية
 */
#define صورة_رمزية(مسار, نصف_قطر) sad_avatar(مسار, نصف_قطر)

// ─── عناصر خاصة ───

/**
 * فاصل - خط فاصل
 */
#define فاصل() sad_divider()

/**
 * مسافة - مسافة فارغة
 */
#define مسافة(حجم) sad_spacer(حجم)

/**
 * شريط_تقدم - شريط تقدم
 */
#define شريط_تقدم(قيمة) sad_progressbar(قيمة)

/**
 * دائرة_تحميل - دائرة تحميل
 */
#define دائرة_تحميل() sad_circular_progress()

/**
 * شارة - شارة صغيرة
 */
#define شارة(نص, لون) sad_badge(نص, لون)

/**
 * رقاقة - رقاقة
 */
#define رقاقة(عنوان) sad_chip(عنوان)

/**
 * شريط_التطبيق - شريط علوي
 */
#define شريط_التطبيق(عنوان) sad_appbar(عنوان)

/**
 * شريط_التنقل - شريط تنقل سفلي
 */
#define شريط_التنقل() sad_bottomnav()

/**
 * لوحة_رسم - لوحة رسم مخصصة
 */
#define لوحة_رسم(دالة, بيانات) sad_canvas(دالة, بيانات)

// ─── إضافة الأبناء ───

/**
 * أضف - إضافة عنصر ابن
 */
#define أضف(أب, ابن) sad_add_child(أب, ابن)

/**
 * أضف_عدة - إضافة عدة أبناء
 */
#define أضف_عدة(أب, أبناء, عدد) sad_add_children(أب, أبناء, عدد)

// ─── تعديل الخصائص ───

/**
 * عيّن_الحجم - تعيين الأبعاد
 */
#define عيّن_الحجم(عنصر, عرض, ارتفاع) sad_set_size(عنصر, عرض, ارتفاع)

/**
 * عيّن_الهوامش - تعيين الهوامش
 */
#define عيّن_الهوامش(عنصر, هوامش) sad_set_margin(عنصر, هوامش)

/**
 * عيّن_الحشو - تعيين الحشو الداخلي
 */
#define عيّن_الحشو(عنصر, حشو) sad_set_padding(عنصر, حشو)

/**
 * عيّن_الخلفية - تعيين لون الخلفية
 */
#define عيّن_الخلفية(عنصر, لون) sad_set_background(عنصر, لون)

/**
 * عيّن_التدرج - تعيين تدرج لوني
 */
#define عيّن_التدرج(عنصر, لون1, لون2, زاوية) sad_set_gradient(عنصر, لون1, لون2, زاوية)

/**
 * عيّن_الحدود - تعيين الحدود
 */
#define عيّن_الحدود(عنصر, عرض, لون) sad_set_border(عنصر, عرض, لون)

/**
 * عيّن_الزوايا - تعيين زوايا مستديرة
 */
#define عيّن_الزوايا(عنصر, نصف_قطر) sad_set_border_radius(عنصر, نصف_قطر)

/**
 * عيّن_الظل - تعيين الظل
 */
#define عيّن_الظل(عنصر, لون, ضبابية, إزاحة_س, إزاحة_ص) sad_set_shadow(عنصر, لون, ضبابية, إزاحة_س, إزاحة_ص)

/**
 * عيّن_الشفافية - تعيين الشفافية
 */
#define عيّن_الشفافية(عنصر, شفافية) sad_set_opacity(عنصر, شفافية)

/**
 * عيّن_المحاذاة - تعيين المحاذاة
 */
#define عيّن_المحاذاة(عنصر, رئيسية, ثانوية) sad_set_alignment(عنصر, رئيسية, ثانوية)

/**
 * عيّن_التباعد - تعيين التباعد بين الأبناء
 */
#define عيّن_التباعد(عنصر, تباعد) sad_set_spacing(عنصر, تباعد)

/**
 * عيّن_المرونة - تعيين المرونة (flex)
 */
#define عيّن_المرونة(عنصر, مرونة) sad_set_flex(عنصر, مرونة)

/**
 * أظهر - إظهار العنصر
 */
#define أظهر(عنصر) sad_set_visible(عنصر, true)

/**
 * أخفِ - إخفاء العنصر
 */
#define أخفِ(عنصر) sad_set_visible(عنصر, false)

/**
 * فعّل - تفعيل العنصر
 */
#define فعّل(عنصر) sad_set_enabled(عنصر, true)

/**
 * عطّل - تعطيل العنصر
 */
#define عطّل(عنصر) sad_set_enabled(عنصر, false)

// ─── الأحداث ───

/**
 * عند_الضغط - حدث النقر
 */
#define عند_الضغط(عنصر, دالة, بيانات) sad_on_tap(عنصر, دالة, بيانات)

/**
 * عند_الضغط_المطول - حدث الضغط المطول
 */
#define عند_الضغط_المطول(عنصر, دالة, بيانات) sad_on_long_press(عنصر, دالة, بيانات)

/**
 * عند_التغيير - حدث التغيير
 */
#define عند_التغيير(عنصر, دالة, بيانات) sad_on_changed(عنصر, دالة, بيانات)

// ─── الحوارات ───

/**
 * أعرض_حوار - عرض حوار
 */
#define أعرض_حوار(تطبيق, عنوان, رسالة, تأكيد, إلغاء, بيانات) \
    sad_show_dialog(تطبيق, عنوان, رسالة, تأكيد, إلغاء, بيانات)

/**
 * أعرض_رسالة - عرض رسالة مؤقتة
 */
#define أعرض_رسالة(تطبيق, رسالة, مدة) sad_show_snackbar(تطبيق, رسالة, مدة)

/**
 * أعرض_ورقة - عرض ورقة سفلية
 */
#define أعرض_ورقة(تطبيق, محتوى) sad_show_bottom_sheet(تطبيق, محتوى)

/**
 * أغلق - إغلاق الحوار
 */
#define أغلق(تطبيق) sad_dismiss(تطبيق)

// ─── الرسم المباشر ───

/**
 * ارسم_مستطيل - رسم مستطيل
 */
#define ارسم_مستطيل(س, ص, عرض, ارتفاع, لون) sad_draw_rect(س, ص, عرض, ارتفاع, لون)

/**
 * ارسم_مستطيل_مستدير - رسم مستطيل مستدير
 */
#define ارسم_مستطيل_مستدير(س, ص, عرض, ارتفاع, نصف_قطر, لون) \
    sad_draw_rounded_rect(س, ص, عرض, ارتفاع, نصف_قطر, لون)

/**
 * ارسم_دائرة - رسم دائرة
 */
#define ارسم_دائرة(س, ص, نصف_قطر, لون) sad_draw_circle(س, ص, نصف_قطر, لون)

/**
 * ارسم_خط - رسم خط
 */
#define ارسم_خط(س1, ص1, س2, ص2, عرض, لون) sad_draw_line(س1, ص1, س2, ص2, عرض, لون)

/**
 * ارسم_نص - رسم نص
 */
#define ارسم_نص(نص, س, ص, نمط) sad_draw_text(نص, س, ص, نمط)

// ─── دوال الهوامش والحشو ───

/**
 * هوامش - إنشاء هوامش متساوية
 */
#define هوامش(قيمة) sadpadding_all(قيمة)

/**
 * هوامش_متناظرة - هوامش رأسية وأفقية
 */
#define هوامش_متناظرة(رأسي, أفقي) sadpadding_symmetric(رأسي, أفقي)

/**
 * حشو - إنشاء حشو متساوي
 */
#define حشو(قيمة) sadpadding_all(قيمة)

/**
 * حشو_متناظر - حشو رأسي وأفقي
 */
#define حشو_متناظر(رأسي, أفقي) sadpadding_symmetric(رأسي, أفقي)

/**
 * زوايا - زوايا مستديرة متساوية
 */
#define زوايا(قيمة) sadradius_all(قيمة)

// ─── الألوان ───

/**
 * لون - إنشاء لون RGBA
 */
#define لون(أحمر, أخضر, أزرق, شفافية) sadcolor(أحمر, أخضر, أزرق, شفافية)

/**
 * لون_هكس - إنشاء لون من قيمة هكس
 */
#define لون_هكس(قيمة) sadcolor_hex(قيمة)

// ألوان مسبقة
#define أبيض       SADCOLOR_WHITE
#define أسود       SADCOLOR_BLACK
#define أحمر       SADCOLOR_RED
#define أخضر       SADCOLOR_GREEN
#define أزرق       SADCOLOR_BLUE
#define أصفر       SADCOLOR_YELLOW
#define برتقالي    SADCOLOR_ORANGE
#define بنفسجي     SADCOLOR_PURPLE
#define سماوي      SADCOLOR_CYAN
#define رمادي      SADCOLOR_GREY
#define شفاف       SADCOLOR_TRANSPARENT

// ألوان التصميم
#define لون_أساسي   SADCOLOR_PRIMARY
#define لون_ثانوي   SADCOLOR_SECONDARY
#define لون_السطح   SADCOLOR_SURFACE
#define لون_الخلفية SADCOLOR_BACKGROUND
#define لون_الخطأ   SADCOLOR_ERROR

// ─── المحاذاة ───

#define بداية         SADALIGN_START
#define منتصف         SADALIGN_CENTER
#define نهاية         SADALIGN_END
#define تمدد          SADALIGN_STRETCH
#define توزيع_متساوي  SADALIGN_SPACE_BETWEEN
#define توزيع_حول     SADALIGN_SPACE_AROUND
#define توزيع_متكافئ  SADALIGN_SPACE_EVENLY

// ─── أيقونات ───

#define أيقونة_الرئيسية    SADICON_HOME
#define أيقونة_الإعدادات   SADICON_SETTINGS
#define أيقونة_الشخص       SADICON_PERSON
#define أيقونة_البحث       SADICON_SEARCH
#define أيقونة_القائمة     SADICON_MENU
#define أيقونة_الإغلاق     SADICON_CLOSE
#define أيقونة_الإضافة     SADICON_ADD
#define أيقونة_الحذف       SADICON_DELETE
#define أيقونة_التعديل     SADICON_EDIT
#define أيقونة_التحقق      SADICON_CHECK
#define أيقونة_الرجوع      SADICON_ARROW_BACK
#define أيقونة_التالي      SADICON_ARROW_FORWARD
#define أيقونة_للأعلى      SADICON_ARROW_UP
#define أيقونة_للأسفل      SADICON_ARROW_DOWN
#define أيقونة_التحديث     SADICON_REFRESH
#define أيقونة_المشاركة    SADICON_SHARE
#define أيقونة_المفضلة     SADICON_FAVORITE
#define أيقونة_النجمة      SADICON_STAR
#define أيقونة_الإشعارات   SADICON_NOTIFICATIONS
#define أيقونة_البريد      SADICON_EMAIL
#define أيقونة_الهاتف      SADICON_PHONE
#define أيقونة_الموقع      SADICON_LOCATION
#define أيقونة_الكاميرا    SADICON_CAMERA
#define أيقونة_الصورة      SADICON_IMAGE
#define أيقونة_القفل       SADICON_LOCK

// أيقونات بنكية
#define أيقونة_الرصيد      SADICON_ACCOUNT_BALANCE
#define أيقونة_البطاقة     SADICON_CREDIT_CARD
#define أيقونة_الدفع       SADICON_PAYMENT
#define أيقونة_التحويل     SADICON_TRANSFER
#define أيقونة_المحفظة     SADICON_WALLET
#define أيقونة_الصراف      SADICON_ATM
#define أيقونة_الفاتورة    SADICON_RECEIPT
#define أيقونة_السجل       SADICON_HISTORY

#ifdef __cplusplus
}
#endif

#endif // SAD_UI_ARABIC_H
