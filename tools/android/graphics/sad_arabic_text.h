/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  نظام عرض النصوص العربية - SadUI Arabic Text Renderer
 * ═══════════════════════════════════════════════════════════════════════════════
 *  
 *  يوفر:
 *    - تحميل خطوط TrueType/OpenType
 *    - تشكيل النص العربي (Arabic Text Shaping)
 *    - دعم الاتجاه من اليمين لليسار (RTL)
 *    - ربط الحروف العربية (Ligatures)
 *    - دعم التشكيل (Harakat)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_ARABIC_TEXT_H
#define SAD_ARABIC_TEXT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  الثوابت
// ═══════════════════════════════════════════════════════════════════════════════

#define SAD_FONT_ATLAS_SIZE 2048
#define SAD_MAX_LOADED_FONTS 16
#define SAD_GLYPH_CACHE_SIZE 4096

// نطاقات Unicode العربية
#define SAD_ARABIC_START         0x0600
#define SAD_ARABIC_END           0x06FF
#define SAD_ARABIC_EXTENDED_A    0x08A0
#define SAD_ARABIC_EXTENDED_B    0x0870
#define SAD_ARABIC_PRESENTATION_A 0xFB50
#define SAD_ARABIC_PRESENTATION_B 0xFE70

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع البيانات
// ═══════════════════════════════════════════════════════════════════════════════

typedef uint32_t SadFontHandle;

// شكل الحرف العربي
typedef enum {
    SAD_ARABIC_FORM_ISOLATED = 0,  // منفصل
    SAD_ARABIC_FORM_INITIAL,       // بداية الكلمة
    SAD_ARABIC_FORM_MEDIAL,        // وسط الكلمة
    SAD_ARABIC_FORM_FINAL          // نهاية الكلمة
} SadArabicForm;

// معلومات حرف رسومي (Glyph)
typedef struct {
    uint32_t codepoint;    // Unicode codepoint
    uint16_t glyphIndex;   // فهرس في الخط
    float x0, y0, x1, y1;  // إحداثيات في atlas
    float xoff, yoff;      // إزاحة الرسم
    float xadvance;        // المسافة للحرف التالي
    float width, height;   // الحجم بالبكسل
} SadGlyphInfo;

// معلومات الخط
typedef struct {
    float ascent;          // المسافة فوق خط الأساس
    float descent;         // المسافة تحت خط الأساس
    float lineGap;         // المسافة بين الأسطر
    float lineHeight;      // ارتفاع السطر الكامل
    float unitsPerEm;      // وحدات تصميم الخط
} SadFontMetrics;

// خيارات عرض النص
typedef struct {
    float fontSize;        // حجم الخط بالبكسل
    float lineSpacing;     // تباعد الأسطر (مضاعف)
    float letterSpacing;   // تباعد الحروف
    bool rtl;              // الاتجاه من اليمين لليسار
    bool enableShaping;    // تفعيل تشكيل النص العربي
    bool enableKerning;    // تفعيل kerning
    uint32_t textColor;    // لون النص (RGBA)
} SadTextOptions;

// نتيجة قياس النص
typedef struct {
    float width;           // عرض النص
    float height;          // ارتفاع النص
    int lineCount;         // عدد الأسطر
    float* lineWidths;     // عرض كل سطر (اختياري)
} SadTextMetrics;

// ═══════════════════════════════════════════════════════════════════════════════
//  تحميل الخطوط
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تهيئة نظام الخطوط
 * يجب استدعاؤها مرة واحدة عند بدء التطبيق
 */
bool sad_font_init(void);

/**
 * إغلاق نظام الخطوط وتحرير الموارد
 */
void sad_font_shutdown(void);

/**
 * تحميل خط من ملف TTF/OTF
 * @param path مسار ملف الخط
 * @param size الحجم بالبكسل
 * @return مقبض الخط أو 0 في حالة الفشل
 */
SadFontHandle sad_font_load(const char* path, float size);

/**
 * تحميل خط من الذاكرة
 * @param data بيانات الخط
 * @param dataSize حجم البيانات
 * @param size الحجم بالبكسل
 * @return مقبض الخط أو 0 في حالة الفشل
 */
SadFontHandle sad_font_load_memory(const void* data, int dataSize, float size);

/**
 * تحميل الخط العربي الافتراضي المدمج
 */
SadFontHandle sad_font_load_default_arabic(float size);

/**
 * تحرير خط
 */
void sad_font_unload(SadFontHandle font);

/**
 * الحصول على مقاييس الخط
 */
SadFontMetrics sad_font_get_metrics(SadFontHandle font);

/**
 * تعيين الخط الافتراضي
 */
void sad_font_set_default(SadFontHandle font);

/**
 * الحصول على الخط الافتراضي
 */
SadFontHandle sad_font_get_default(void);

// ═══════════════════════════════════════════════════════════════════════════════
//  تشكيل النص العربي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * التحقق مما إذا كان الحرف عربياً
 */
bool sad_is_arabic_char(uint32_t codepoint);

/**
 * التحقق مما إذا كان الحرف يتصل من اليمين
 */
bool sad_arabic_joins_right(uint32_t codepoint);

/**
 * التحقق مما إذا كان الحرف يتصل من اليسار
 */
bool sad_arabic_joins_left(uint32_t codepoint);

/**
 * الحصول على الشكل الصحيح للحرف العربي
 * @param codepoint الحرف الأصلي
 * @param prevJoins هل الحرف السابق يتصل
 * @param nextJoins هل الحرف التالي يتصل
 * @return codepoint الشكل المناسب
 */
uint32_t sad_arabic_shape(uint32_t codepoint, bool prevJoins, bool nextJoins);

/**
 * تشكيل نص عربي كامل
 * @param input النص المُدخل (UTF-8)
 * @param output مخرج النص المُشكّل (يجب تخصيص المساحة مسبقاً)
 * @param outputSize حجم المخرج
 * @return طول النص المُشكّل
 */
int sad_arabic_shape_text(const char* input, uint32_t* output, int outputSize);

// ═══════════════════════════════════════════════════════════════════════════════
//  قياس ورسم النص
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * قياس أبعاد النص
 * @param font مقبض الخط
 * @param text النص (UTF-8)
 * @param options خيارات العرض
 * @param maxWidth أقصى عرض (0 = بلا حد)
 * @return أبعاد النص
 */
SadTextMetrics sad_text_measure(SadFontHandle font, const char* text, 
                                 const SadTextOptions* options, float maxWidth);

/**
 * رسم النص على الشاشة
 * @param font مقبض الخط
 * @param text النص (UTF-8)
 * @param x الإحداثي الأفقي
 * @param y الإحداثي الرأسي
 * @param options خيارات العرض
 */
void sad_text_draw(SadFontHandle font, const char* text, 
                   float x, float y, const SadTextOptions* options);

/**
 * رسم النص مع التفاف تلقائي
 * @param font مقبض الخط
 * @param text النص (UTF-8)
 * @param x الإحداثي الأفقي
 * @param y الإحداثي الرأسي
 * @param maxWidth أقصى عرض
 * @param options خيارات العرض
 */
void sad_text_draw_wrapped(SadFontHandle font, const char* text,
                           float x, float y, float maxWidth,
                           const SadTextOptions* options);

// ═══════════════════════════════════════════════════════════════════════════════
//  خيارات افتراضية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * الحصول على خيارات افتراضية للنص العربي
 */
SadTextOptions sad_text_options_arabic(float fontSize);

/**
 * الحصول على خيارات افتراضية للنص اللاتيني
 */
SadTextOptions sad_text_options_latin(float fontSize);

#ifdef __cplusplus
}
#endif

#endif // SAD_ARABIC_TEXT_H
