/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: arabic_shaper.h
 * المسار: features/graphics/backends/freestanding/include/sad_ui/freestanding/arabic_shaper.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) مُشكِّل النصّ العربيّ للوضع المستقلّ — اختيار الأشكال السياقيّة + عكس RTL
 * @brief (EN) Freestanding Arabic shaper — contextual forms + simple RTL run reversal
 *
 * الوصف:
 * ------
 * الخطوط النقطيّة (المدمج/PSF) تفهرس الغليفات بنقطة Unicode واحدة لكلّ غليف —
 * لا HarfBuzz ولا منطق تشكيل فيها. هذه الوحدة دالّة صافية واحدة `shape()` تحوّل
 * تسلسل نقاط منطقيًّا (كما جاء في النصّ) إلى تسلسل نقاط عرضيّ جاهز لحلقة رسم
 * تتقدّم يسارًا:
 *
 *   1. أصناف اتّصال الحروف U+0621–U+064A (لا يتّصل / يتّصل يمينًا فقط / بالجهتين).
 *   2. اختيار الشكل السياقيّ (منفصل/ابتدائيّ/وسطيّ/نهائيّ) ⇒ نقاط أشكال العرض-B
 *      U+FE70–U+FEFF. الجدول مبنيّ بعكس الجدول المرجعيّ الموثوق في المترجم
 *      (compiler/src/backend/llvm/arabic_normalization.cpp — decomposePresBForm):
 *      ترتيب الشيفرات هناك منفصل/نهائيّ[/ابتدائيّ/وسطيّ] لكلّ أساس، وهو نفس
 *      ترتيب مقطع Unicode Presentation Forms-B.
 *   3. ليغاتورة لام-ألف: ل + (آ/أ/إ/ا) ⇒ U+FEF5–U+FEFC (منفصلة أو نهائيّة
 *      بحسب اتّصال ما قبل اللام) — حرفان منطقيًّا يصيران غليفًا واحدًا.
 *   4. عكس ترتيب المدى العربيّ + تموضع الأرقام RTL: كلّ مدًى متّصل من نقاط
 *      عربيّة (مع المسافات والأرقام البينيّة) يُعكس، ثمّ تُعاد المقاطعُ الرقميّة
 *      داخله إلى ترتيبها LTR — فرقمٌ كـ«3.14» ضمن عربيّ يظهر يسار العربيّ
 *      ويُقرأ 3.14 لا 41.3. اللاتينيّ خارج المدى يبقى بترتيبه الأصليّ.
 *
 * الحدود المعلَنة (عمدًا — نطاق الشريحة نصوص واجهة بسيطة):
 *   • لا UAX#9 (bidi كامل): لا تضمينات صريحة (LRE/RLE/PDF) ولا حلّ محايدات
 *     متداخل ولا أرقام تسبق المدى العربيّ ابتداءً — «عكس المدى + تموضع أرقام
 *     RTL» فقط (يغطّي أحجام/تواريخ/إصدارات نصوص الواجهة). UAX#9 الكامل دَين
 *     لاحق مسمًّى.
 *   • الفاصل الرقميّ يُضمّ للمقطع الرقميّ بين رقمين فقط؛ فاصلٌ طرفيّ («نص3.»)
 *     ينفصل عن رقمه بصريًّا — أثرٌ مقبول ضمن الحدّ (نادرٌ في نصوص الواجهة).
 *   • الحركات (U+064B وما بعدها) شفّافة اتّصاليًّا: تُسقَط قبل التشكيل فلا تكسر
 *     اتّصال الحرفين المحيطين بها (المرحلة 0)، لكنّها لا تُموضَع فوقيًّا — تموضعها
 *     الفوقيّ (advance=0 + تركيب) دَين FreeType المعلَن (المسار المتّجه).
 *   • التطويل U+0640 خارج الجدول (لا شكل عرض له) — يمرّ كما هو.
 *   • ليغاتورات أخرى غير لام-ألف (اختياريّة في Unicode) غير مطبَّقة.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_FREESTANDING_ARABIC_SHAPER_H
#define SAD_UI_FREESTANDING_ARABIC_SHAPER_H

#include <cstdint>
#include <vector>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {
            namespace arabic
            {

                // ─── مدايات Unicode المسمّاة (لا أرقام سحريّة في الاستهلاك) ───────

                /// (AR) أوّل حروف الجدول العربيّ الأساس المدعومة (ء)
                inline constexpr uint32_t ARABIC_LETTERS_FIRST = 0x0621;
                /// (AR) آخر حروف الجدول العربيّ الأساس المدعومة (ي)
                inline constexpr uint32_t ARABIC_LETTERS_LAST = 0x064A;

                /// (AR) بداية/نهاية الكتلة العربيّة الأساس (لكشف المدى العربيّ عند العكس)
                inline constexpr uint32_t ARABIC_BLOCK_FIRST = 0x0600;
                inline constexpr uint32_t ARABIC_BLOCK_LAST = 0x06FF;

                /// (AR) مقطع أشكال العرض-A (قد يرد في نصوص مُشكَّلة مسبقًا)
                inline constexpr uint32_t PRESENTATION_FORMS_A_FIRST = 0xFB50;
                inline constexpr uint32_t PRESENTATION_FORMS_A_LAST = 0xFDFF;

                /// (AR) مقطع أشكال العرض-B — ناتج هذا المُشكِّل
                inline constexpr uint32_t PRESENTATION_FORMS_B_FIRST = 0xFE70;
                inline constexpr uint32_t PRESENTATION_FORMS_B_LAST = 0xFEFF;

                /// (AR) المسافة — الفاصل الوحيد الذي يُضمّ داخل المدى العربيّ المعكوس
                inline constexpr uint32_t SPACE_CODEPOINT = 0x0020;

                // ─── مدايات الأرقام (تُضمّ للمدى العربيّ ثمّ تُعاد لترتيبها LTR) ──────
                /// (AR) أرقام ASCII اللاتينيّة ٠..٩ (0x30..0x39)
                inline constexpr uint32_t ASCII_DIGIT_FIRST = 0x0030;
                inline constexpr uint32_t ASCII_DIGIT_LAST = 0x0039;
                /// (AR) الأرقام العربيّة-الهنديّة ٠..٩ (0x0660..0x0669)
                inline constexpr uint32_t ARABIC_INDIC_DIGIT_FIRST = 0x0660;
                inline constexpr uint32_t ARABIC_INDIC_DIGIT_LAST = 0x0669;
                /// (AR) الأرقام العربيّة-الهنديّة الممتدّة (فارسيّ/أردو) ۰..۹ (0x06F0..0x06F9)
                inline constexpr uint32_t EXT_ARABIC_INDIC_DIGIT_FIRST = 0x06F0;
                inline constexpr uint32_t EXT_ARABIC_INDIC_DIGIT_LAST = 0x06F9;

                /// (AR) هل النقطة رقم (بأيّ من مجموعات الأرقام الثلاث)؟
                inline bool isBidiDigit(uint32_t cp)
                {
                    return (cp >= ASCII_DIGIT_FIRST && cp <= ASCII_DIGIT_LAST) ||
                           (cp >= ARABIC_INDIC_DIGIT_FIRST && cp <= ARABIC_INDIC_DIGIT_LAST) ||
                           (cp >= EXT_ARABIC_INDIC_DIGIT_FIRST && cp <= EXT_ARABIC_INDIC_DIGIT_LAST);
                }

                // ─── فواصل رقميّة داخليّة مسمّاة (تُضمّ للمقطع الرقميّ بين رقمين فقط) ──
                inline constexpr uint32_t FULL_STOP_CODEPOINT = 0x002E;        // .
                inline constexpr uint32_t COMMA_CODEPOINT = 0x002C;            // ,
                inline constexpr uint32_t COLON_CODEPOINT = 0x003A;            // :
                inline constexpr uint32_t ARABIC_DECIMAL_SEP_CODEPOINT = 0x066B; // ٫
                inline constexpr uint32_t ARABIC_THOUSANDS_SEP_CODEPOINT = 0x066C; // ٬

                /// (AR) فاصل رقميّ داخليّ (يُضمّ للمقطع الرقميّ فقط بين رقمين): النقطة/
                ///      الفاصلة/النقطتان + الفاصلة العربيّة العشريّة/الألفيّة.
                inline bool isNumberSeparator(uint32_t cp)
                {
                    return cp == FULL_STOP_CODEPOINT || cp == COMMA_CODEPOINT ||
                           cp == COLON_CODEPOINT || cp == ARABIC_DECIMAL_SEP_CODEPOINT ||
                           cp == ARABIC_THOUSANDS_SEP_CODEPOINT;
                }

                /// (AR) هل النقطة من نطاق أشكال العرض-B؟ (يستعملها عدّاد غليفات الإثبات)
                inline bool isPresentationFormB(uint32_t cp)
                {
                    return cp >= PRESENTATION_FORMS_B_FIRST && cp <= PRESENTATION_FORMS_B_LAST;
                }

                /**
                 * @brief (AR) دالّة التشكيل الصافية: نقاط منطقيّة ⇒ نقاط عرضيّة
                 * @brief (EN) Pure shaping: logical codepoints ⇒ display codepoints
                 *
                 * لا حالة ولا مؤثّرات جانبيّة — المدخل بترتيب المنطق (أوّل حرف = أقصى
                 * اليمين بصريًّا)، والمخرج جاهز لحلقة رسم تتقدّم يسارًا. النقاط خارج
                 * الجدول العربيّ (لاتينيّ/أرقام/رموز) تمرّ كما هي وبترتيبها.
                 */
                std::vector<uint32_t> shape(const std::vector<uint32_t> &codepoints);

            } // namespace arabic
        } // namespace freestanding
    } // namespace ui
} // namespace sad

#endif // SAD_UI_FREESTANDING_ARABIC_SHAPER_H
