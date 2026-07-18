/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: linuxfb.h
 * المسار: features/graphics/backends/freestanding/include/sad_ui/freestanding/linuxfb.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) جسر جهاز framebuffer اللينكسيّ (/dev/fb0) إلى FreestandingRenderer
 * @brief (EN) Linux framebuffer device (/dev/fb0) bridge for FreestandingRenderer
 *
 * الوصف:
 * ------
 * المُصيّر المستقلّ (FreestandingRenderer/FreestandingWindow) يرسم على أيّ ذاكرة
 * إطار خام تُوصف له عبر FramebufferConfig. هذا الملفّ يسدّ الفجوة الوحيدة على
 * لينكس بلا X11/SDL: فتح جهاز fbdev، استعلام أبعاده (FBIOGET_VSCREENINFO/
 * FBIOGET_FSCREENINFO)، وmmap ذاكرته، ثمّ ملء FramebufferConfig جاهزة.
 *
 * القيود المعماريّة (مقصودة — لا التفاف عليها):
 *   • فشل-مُغلق على عمق لون 32-بت حصرًا: المُصيّر يكتب كلمات ARGB 32-بت مباشرة،
 *     وأيّ عمق آخر (16-بت من vga=791 مثلًا) يُنتج ألوانًا مشوّهة صامتة — نرفضه
 *     برسالة عربيّة واضحة بدل «نجاح» كاذب.
 *   • لا vsync على fbdev: جهاز fb0 لا يوفّر مزامنة رأسيّة موثوقة عبر ioctl
 *     محمولة، فيُكتفى بالتخزين المزدوج (الرسم في مخزن خلفيّ ثمّ نسخه دفعة
 *     واحدة في endFrame) — تمزّق نادر محتمل نظريًّا لكنّه غير مرئيّ عمليًّا
 *     في رسم ثابت.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_FREESTANDING_LINUXFB_H
#define SAD_UI_FREESTANDING_LINUXFB_H

#ifdef __linux__

#include "sad_ui/freestanding/renderer.h"

#include <string>
#include <vector>
#include <cstddef>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {
            namespace linuxfb
            {

                /// (AR) المسار الافتراضيّ لجهاز framebuffer الأوّل — ثابت مسمّى
                ///      (قاعدة المشروع: لا سلاسل خام ذات معنى في مواضع الاستعمال).
                inline constexpr const char *DEFAULT_FB_DEVICE_PATH = "/dev/fb0";

                /// (AR) عمق اللون الوحيد المدعوم (بت لكلّ بكسل) — عقد المُصيّر المستقلّ.
                inline constexpr unsigned SUPPORTED_BPP = 32;

                /**
                 * @brief (AR) جهاز framebuffer مفتوح + إعداداته الجاهزة للمُصيّر
                 * @brief (EN) An opened fbdev handle plus its renderer-ready config
                 *
                 * يُملأ بواسطة openFramebufferDevice() ويُحرَّر بـcloseFramebufferDevice().
                 * config جاهزة للتمرير مباشرة إلى FreestandingWindow::initializeFramebuffer.
                 */
                struct LinuxFbDevice
                {
                    FramebufferConfig config{}; ///< إعدادات جاهزة (address/width/height/pitch/bpp/backBuffer)
                    int fd = -1;                ///< واصف ملفّ الجهاز (-1 = غير مفتوح)
                    void *mapped = nullptr;     ///< بداية منطقة mmap (قد تسبق config.address بإزاحات vscreeninfo)
                    std::size_t mappedLength = 0; ///< طول منطقة mmap بالبايتات (smem_len)

                    /// (AR) المخزن الخلفيّ للتخزين المزدوج — مملوك هنا (RAII عبر vector)
                    std::vector<uint32_t> backBufferStorage;

                    /// (AR) رسالة الخطأ العربيّة الأخيرة (فارغة = لا خطأ)
                    std::string error;
                };

                /**
                 * @brief (AR) يفتح جهاز framebuffer ويملأ dev.config — فشل-مُغلق
                 * @brief (EN) Open an fbdev device and fill dev.config — fail-closed
                 *
                 * @param dev  البنية التي تُملأ (تُغلق تلقائيًّا عند الفشل الجزئيّ)
                 * @param devicePath مسار الجهاز (افتراضيًّا DEFAULT_FB_DEVICE_PATH)
                 * @return true عند النجاح؛ عند الفشل: false وdev.error تشرح السبب بالعربيّة
                 */
                bool openFramebufferDevice(LinuxFbDevice &dev,
                                           const std::string &devicePath = DEFAULT_FB_DEVICE_PATH);

                /**
                 * @brief (AR) يُحرّر الجهاز: munmap + close (آمنة الاستدعاء المتكرّر)
                 * @brief (EN) Release the device: munmap + close (idempotent)
                 */
                void closeFramebufferDevice(LinuxFbDevice &dev);

            } // namespace linuxfb
        } // namespace freestanding
    } // namespace ui
} // namespace sad

#endif // __linux__
#endif // SAD_UI_FREESTANDING_LINUXFB_H
