/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: psf_font.h
 * المسار: features/graphics/backends/freestanding/include/sad_ui/freestanding/psf_font.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) محمّل خطوط PSF (‏PSF1/PSF2) بجدول Unicode إلى BitmapFont
 * @brief (EN) PSF (PSF1/PSF2) console font loader with unicode table → BitmapFont
 *
 * الوصف:
 * ------
 * الخطّ المدمج يغطّي ASCII + الأشكال العربيّة المنفصلة فقط — لدعم أشكال العرض
 * (FE70–FEFF) يُحمَّل خطّ PSF خارجيّ (مثل خطوط الطرفيّة اللينكسيّة). هذا المحمّل:
 *
 *   • يدعم PSF1 (التوقيع 0x36 0x04) وPSF2 (التوقيع 0x72 0xB5 0x4A 0x86).
 *   • يتطلّب جدول Unicode في الملفّ (بدونه لا معنى لفهرسة codepoint ⇒ فشل ناعم).
 *   • يملأ بنية BitmapFont القائمة كما هي — findGlyph يقبل أيّ codepoint أصلًا،
 *     فلا تغيير بنيويًّا في المُصيّر (الغليف الواحد المقابل لعدّة نقاط يُسجَّل
 *     مدخلًا لكلّ نقطة بنفس مؤشّر البيانات).
 *   • فشل ناعم معلَن: ملفّ غائب/توقيع فاسد/جدول غائب ⇒ false ورسالة عربيّة في
 *     error — المستهلك يسقط للخطّ المدمج ولا ينهار.
 *
 * ملاحظات الصيغة (مطبَّقة هنا):
 *   • PSF1: جدول Unicode تسلسلات UTF-16LE، ‏0xFFFF فاصل غليفات و0xFFFE يبدأ
 *     تسلسلات التراكيب (تُتجاهَل التراكيب، وتُؤخذ كلّ النقاط المفردة قبلها).
 *   • PSF2: جدول Unicode بترميز UTF-8، ‏0xFF فاصل غليفات و0xFE يبدأ التراكيب.
 *
 * الحدود المعلَنة:
 *   • تسلسلات التراكيب (حرف أساس + حركات ⇒ غليف واحد) تُتجاهَل — النقاط
 *     المفردة فقط تُفهرَس (يكفي غرض الشريحة: أشكال العرض مفردة النقطة).
 *   • لا دعم لخطوط بلا جدول Unicode (فهرسة موضعيّة خام) — فشل ناعم صريح.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_FREESTANDING_PSF_FONT_H
#define SAD_UI_FREESTANDING_PSF_FONT_H

#include "sad_ui/freestanding/renderer.h"

#include <string>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {
            namespace psf
            {

                // ─── ثوابت صيغة PSF المسمّاة (لا أرقام سحريّة في التنفيذ) ─────────

                /// (AR) توقيع PSF1 (بايتان)
                inline constexpr uint8_t PSF1_MAGIC_0 = 0x36;
                inline constexpr uint8_t PSF1_MAGIC_1 = 0x04;
                /// (AR) أعلام نمط PSF1
                inline constexpr uint8_t PSF1_MODE_512 = 0x01;    ///< 512 غليفًا بدل 256
                inline constexpr uint8_t PSF1_MODE_HAS_TAB = 0x02; ///< جدول Unicode موجود
                inline constexpr uint8_t PSF1_MODE_HAS_SEQ = 0x04; ///< تسلسلات تراكيب
                /// (AR) علامتا جدول PSF1 (‏UTF-16LE)
                inline constexpr uint16_t PSF1_SEPARATOR = 0xFFFF; ///< نهاية مدخل الغليف
                inline constexpr uint16_t PSF1_START_SEQ = 0xFFFE; ///< بداية التراكيب

                /// (AR) توقيع PSF2 (‏little-endian)
                inline constexpr uint32_t PSF2_MAGIC = 0x864AB572;
                /// (AR) علم وجود جدول Unicode في PSF2
                inline constexpr uint32_t PSF2_HAS_UNICODE_TABLE = 0x01;
                /// (AR) علامتا جدول PSF2 (بايتات UTF-8)
                inline constexpr uint8_t PSF2_SEPARATOR = 0xFF; ///< نهاية مدخل الغليف
                inline constexpr uint8_t PSF2_START_SEQ = 0xFE; ///< بداية التراكيب

                /**
                 * @brief (AR) تحميل خطّ PSF من ملفّ إلى BitmapFont — فشل ناعم
                 * @param path مسار ملفّ الخطّ
                 * @param font (خرج) الخطّ الممتلئ عند النجاح
                 * @param error (خرج) رسالة عربيّة تشرح سبب الفشل عند إعادة false
                 * @return true عند النجاح؛ false (مع error) عند الغياب/الفساد —
                 *         المستهلك يسقط للخطّ المدمج (لا انهيار)
                 */
                bool loadPsfFont(const std::string &path, BitmapFont &font,
                                 std::string &error);

            } // namespace psf
        } // namespace freestanding
    } // namespace ui
} // namespace sad

#endif // SAD_UI_FREESTANDING_PSF_FONT_H
