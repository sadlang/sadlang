/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: arabic_shaper.h
 * المسار: features/graphics/backends/freestanding/include/sad_ui/freestanding/arabic_shaper.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) مُشكِّل النصّ العربيّ للوضع المستقلّ — أشكال سياقيّة + اتّجاه ثنائيّ UAX#9
 * @brief (EN) Freestanding Arabic shaper — contextual forms + full UAX#9 bidi reorder
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
 *   4. إعادة الترتيب البصريّ بخوارزميّة الاتّجاه الثنائيّ Unicode (UAX#9) الكاملة
 *      لفقرةٍ واحدة: تُحسَب مستويات التضمين (تضمينات وتجاوزات صريحة LRE/RLE/LRO/
 *      RLO/PDF + عزلات LRI/RLI/FSI/PDI)، ثمّ القواعد الضعيفة (W1–W7) وأزواج
 *      الأقواس (N0/BD16) والمحايدة (N1–N2) والضمنيّة (I1–I2)، ثمّ إعادة الترتيب
 *      (L1–L2). اتّجاه الفقرة تلقائيّ
 *      من أوّل حرفٍ قويّ (P2–P3): عربيّ-أوّلًا ⇒ RTL-أساس، لاتينيّ-أوّلًا ⇒ LTR-أساس.
 *      فرقمٌ كـ«3.14» ضمن عربيّ يظهر يساره ويُقرأ 3.14 لا 41.3، ورقمٌ قائدٌ يقع
 *      يمينَ العربيّ (أوّلٌ منطقيًّا = أيمن بصريًّا)، ونصٌّ مختلط L/R يتموضع صحيحًا.
 *
 * الحدود المعلَنة (عمدًا — نطاق الشريحة نصوص واجهة بسيطة):
 *   • N0 (مطابقة أزواج الأقواس BD16) مطبَّقة لأقواس ASCII القياسيّة ( [ { (من
 *     Bidi_Paired_Bracket) + توسعةً للقوسين المزخرفين العربيّين ﴾﴿ (هذان ليسا
 *     حاصرتين في UCD، bpt=None، لكنّهما قوسان دلاليًّا فنعاملهما كذلك) — لا لكامل
 *     مجموعة Bidi_Paired_Bracket في UCD.
 *     وجدول تصنيف bidi يغطّي نطاق الشكّال (عربيّ/عبريّ/لاتينيّ/أرقام/ترقيم شائع) لا
 *     كامل قاعدة UCD. والعزلات تُعامَل كتضمينات (دقيق للتضمين، تقريبيّ للعزل العميق
 *     المتداخل). وحدةٌ واحدة (فقرة) لا متعدّدة.
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

                // (AR) تصنيف الأرقام والفواصل الرقميّة الآن داخل محرّك bidi (UAX#9) في
                //      arabic_shaper.cpp (bidiClass) — لا حاجة لثوابت/دوالّ عامّة هنا بعد
                //      إزالة المسار الاستدلاليّ «عكس المدى + تموضع أرقام».

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
