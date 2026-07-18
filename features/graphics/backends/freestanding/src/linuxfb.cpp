/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: linuxfb.cpp
 * المسار: features/graphics/backends/freestanding/src/linuxfb.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) تنفيذ جسر /dev/fb0: فتح + استعلام ioctl + mmap + ملء FramebufferConfig
 * @brief (EN) Linux fbdev bridge implementation: open + ioctl query + mmap + config fill
 *
 * راجع linuxfb.h لشرح القيود (فشل-مُغلق على 32-بت، لا vsync على fbdev —
 * يُكتفى بنسخ المخزن الخلفيّ دفعةً واحدة في endFrame).
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifdef __linux__

#include "sad_ui/freestanding/linuxfb.h"

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstring>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {
            namespace linuxfb
            {

                bool openFramebufferDevice(LinuxFbDevice &dev, const std::string &devicePath)
                {
                    // (AR) نظافة إعادة الاستعمال: أيّ فتح سابق يُحرَّر أوّلًا (idempotent).
                    closeFramebufferDevice(dev);
                    dev.error.clear();

                    dev.fd = ::open(devicePath.c_str(), O_RDWR);
                    if (dev.fd < 0)
                    {
                        dev.error = "تعذّر فتح جهاز الإطار «" + devicePath + "»: " + std::strerror(errno);
                        return false;
                    }

                    // (AR) استعلام الإعدادات المتغيّرة (الأبعاد وعمق اللون) والثابتة (pitch وطول الذاكرة):
                    fb_var_screeninfo vinfo{};
                    fb_fix_screeninfo finfo{};
                    if (::ioctl(dev.fd, FBIOGET_VSCREENINFO, &vinfo) != 0)
                    {
                        dev.error = "فشل FBIOGET_VSCREENINFO على «" + devicePath + "»: " + std::strerror(errno);
                        closeFramebufferDevice(dev);
                        return false;
                    }
                    if (::ioctl(dev.fd, FBIOGET_FSCREENINFO, &finfo) != 0)
                    {
                        dev.error = "فشل FBIOGET_FSCREENINFO على «" + devicePath + "»: " + std::strerror(errno);
                        closeFramebufferDevice(dev);
                        return false;
                    }

                    // (AR) فشل-مُغلق على عمق اللون: المُصيّر المستقلّ يكتب كلمات 32-بت
                    //      حصرًا؛ عمق 16-بت (مثل نمط vga=791) أو 24-بت يُنتج ألوانًا
                    //      مشوّهة صامتة لو «نجحنا» زورًا — نرفض بوضوح وبالعربيّة:
                    if (vinfo.bits_per_pixel != SUPPORTED_BPP)
                    {
                        dev.error = "عمق اللون غير مدعوم: الجهاز يقدّم " +
                                    std::to_string(vinfo.bits_per_pixel) +
                                    " بت/بكسل والمُصيّر المستقلّ يتطلّب " +
                                    std::to_string(SUPPORTED_BPP) +
                                    " بت/بكسل — أقلع بنمط عرض 32-بت (مثل vga=0x344 للـ1024×768×32)";
                        closeFramebufferDevice(dev);
                        return false;
                    }
                    if (vinfo.xres == 0 || vinfo.yres == 0 || finfo.line_length == 0 ||
                        finfo.smem_len == 0)
                    {
                        dev.error = "إعدادات الجهاز غير سليمة (أبعاد أو خطوة سطر أو طول ذاكرة صفريّ)";
                        closeFramebufferDevice(dev);
                        return false;
                    }
                    // (AR) فشل-مُغلق على مدى النسخ: مع panning غير صفريّ (yoffset) قد يمتدّ
                    //      الإطار المرئيّ نظريًّا خلف نهاية mmap (smem_len) — نرفض بوضوح:
                    if (static_cast<std::uint64_t>(finfo.smem_len) <
                        static_cast<std::uint64_t>(vinfo.yoffset) * finfo.line_length +
                            static_cast<std::uint64_t>(finfo.line_length) * vinfo.yres)
                    {
                        dev.error = "ذاكرة الجهاز أقصر من الإطار المرئيّ (smem_len < yoffset×خطوة_السطر + خطوة_السطر×الارتفاع) — النسخ سيتجاوز نهاية mmap";
                        closeFramebufferDevice(dev);
                        return false;
                    }

                    // (AR) mmap كامل ذاكرة الجهاز (smem_len) — القراءة والكتابة معًا لأنّ
                    //      خلط الألفا البرمجيّ في المُصيّر قد يقرأ البكسل الخلفيّ:
                    void *mapped = ::mmap(nullptr, finfo.smem_len, PROT_READ | PROT_WRITE,
                                          MAP_SHARED, dev.fd, 0);
                    if (mapped == MAP_FAILED)
                    {
                        dev.error = "فشل mmap على «" + devicePath + "»: " + std::strerror(errno);
                        closeFramebufferDevice(dev);
                        return false;
                    }
                    dev.mapped = mapped;
                    dev.mappedLength = finfo.smem_len;

                    // (AR) عنوان أوّل بكسل مرئيّ مع احترام إزاحات اللوحة الافتراضيّة
                    //      (panning) إن وُجدت — عادةً صفر على vesafb:
                    auto *base = static_cast<uint8_t *>(mapped) +
                                 static_cast<std::size_t>(vinfo.yoffset) * finfo.line_length +
                                 static_cast<std::size_t>(vinfo.xoffset) * (SUPPORTED_BPP / 8u);

                    // (AR) المخزن الخلفيّ: نخصّصه بنفس تخطيط الجهاز (خطوة السطر كاملة لا
                    //      العرض فقط) كي تتطابق فهرسة الرسم والنسخ النهائيّ بايتًا ببايت.
                    //      لا vsync على fbdev — النسخ الدفعيّ في endFrame هو البديل العمليّ:
                    const std::size_t pitchPixels = finfo.line_length / sizeof(uint32_t);
                    dev.backBufferStorage.assign(pitchPixels * vinfo.yres, 0u);

                    dev.config.address = reinterpret_cast<uint32_t *>(base);
                    dev.config.width = vinfo.xres;
                    dev.config.height = vinfo.yres;
                    dev.config.pitch = finfo.line_length;
                    dev.config.bpp = static_cast<uint8_t>(vinfo.bits_per_pixel);
                    dev.config.backBuffer = dev.backBufferStorage.data();
                    return true;
                }

                void closeFramebufferDevice(LinuxFbDevice &dev)
                {
                    if (dev.mapped && dev.mapped != MAP_FAILED)
                    {
                        ::munmap(dev.mapped, dev.mappedLength);
                    }
                    dev.mapped = nullptr;
                    dev.mappedLength = 0;
                    if (dev.fd >= 0)
                    {
                        ::close(dev.fd);
                        dev.fd = -1;
                    }
                    dev.backBufferStorage.clear();
                    dev.config = FramebufferConfig{};
                }

            } // namespace linuxfb
        } // namespace freestanding
    } // namespace ui
} // namespace sad

#endif // __linux__
