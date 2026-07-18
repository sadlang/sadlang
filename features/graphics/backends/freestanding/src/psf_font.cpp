/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: psf_font.cpp — تنفيذ محمّل خطوط PSF1/PSF2 بجدول Unicode
 * المسار: features/graphics/backends/freestanding/src/psf_font.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 * حقوق النشر (c) 2024-2026 فريق لغة ص — رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/freestanding/psf_font.h"

#include <cstddef>
#include <fstream>
#include <memory>
#include <vector>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {
            namespace psf
            {
                namespace
                {

                    // ─── رسائل الفشل الناعم (ثوابت مسمّاة — لا سلاسل خام مبعثرة) ───
                    constexpr const char *ERR_OPEN = "تعذّر فتح ملفّ الخطّ: ";
                    constexpr const char *ERR_MAGIC = "ليس ملفّ PSF (التوقيع غير مطابق لـPSF1/PSF2): ";
                    constexpr const char *ERR_NO_TABLE = "خطّ PSF بلا جدول Unicode — لا يمكن الفهرسة بنقاط الحروف: ";
                    constexpr const char *ERR_TRUNCATED = "ملفّ PSF مبتور (بيانات الغليفات/الجدول أقصر من المعلَن): ";
                    constexpr const char *ERR_GEOMETRY = "أبعاد خطّ PSF2 غير متّسقة (charsize لا يطابق العرض×الارتفاع): ";
                    constexpr const char *ERR_EMPTY_MAP = "جدول Unicode في خطّ PSF لم يُنتج أيّ فهرسة: ";

                    /// (AR) حجم ترويسة PSF1 وحقول PSF2 (بايتات) — من مواصفة الصيغة
                    constexpr std::size_t PSF1_HEADER_SIZE = 4;
                    constexpr std::size_t PSF2_HEADER_SIZE = 32;
                    constexpr std::size_t PSF1_GLYPH_COUNT_BASE = 256;
                    constexpr std::size_t PSF1_GLYPH_COUNT_512 = 512;
                    constexpr uint8_t PSF1_GLYPH_WIDTH = 8; ///< PSF1 ثابت العرض 8

                    /// (AR) هامش ارتفاع السطر فوق ارتفاع الغليف (نفس روح الخطّ المدمج 16⇒18)
                    constexpr uint8_t LINE_HEIGHT_MARGIN = 2;

                    uint32_t readLE32(const std::vector<uint8_t> &d, std::size_t off)
                    {
                        return static_cast<uint32_t>(d[off]) |
                               (static_cast<uint32_t>(d[off + 1]) << 8) |
                               (static_cast<uint32_t>(d[off + 2]) << 16) |
                               (static_cast<uint32_t>(d[off + 3]) << 24);
                    }

                    uint16_t readLE16(const std::vector<uint8_t> &d, std::size_t off)
                    {
                        return static_cast<uint16_t>(
                            static_cast<uint16_t>(d[off]) |
                            (static_cast<uint16_t>(d[off + 1]) << 8));
                    }

                    /// (AR) إضافة مدخل غليف للخطّ (نقطة واحدة ⇒ نفس بيانات الغليف)
                    void addGlyphEntry(BitmapFont &font, uint32_t codepoint,
                                       const uint8_t *bitmap, uint8_t width,
                                       uint8_t height)
                    {
                        BitmapGlyph g;
                        g.codepoint = codepoint;
                        g.width = width;
                        g.height = height;
                        g.xOffset = 0;
                        g.yOffset = 0;
                        g.advance = width;
                        g.bitmap = bitmap;
                        font.glyphs.push_back(g);
                    }

                    /// (AR) فكّ UTF-8 بسيط لجدول PSF2 — يعيد النقطة ويقدّم i، أو 0 عند فساد
                    uint32_t decodeUTF8Table(const std::vector<uint8_t> &d,
                                             std::size_t &i, std::size_t end)
                    {
                        const uint8_t c = d[i];
                        if (c < 0x80)
                        {
                            ++i;
                            return c;
                        }
                        if ((c & 0xE0) == 0xC0 && i + 1 < end)
                        {
                            const uint32_t cp = (static_cast<uint32_t>(c & 0x1F) << 6) |
                                                (d[i + 1] & 0x3F);
                            i += 2;
                            return cp;
                        }
                        if ((c & 0xF0) == 0xE0 && i + 2 < end)
                        {
                            const uint32_t cp = (static_cast<uint32_t>(c & 0x0F) << 12) |
                                                (static_cast<uint32_t>(d[i + 1] & 0x3F) << 6) |
                                                (d[i + 2] & 0x3F);
                            i += 3;
                            return cp;
                        }
                        if ((c & 0xF8) == 0xF0 && i + 3 < end)
                        {
                            const uint32_t cp = (static_cast<uint32_t>(c & 0x07) << 18) |
                                                (static_cast<uint32_t>(d[i + 1] & 0x3F) << 12) |
                                                (static_cast<uint32_t>(d[i + 2] & 0x3F) << 6) |
                                                (d[i + 3] & 0x3F);
                            i += 4;
                            return cp;
                        }
                        ++i; // بايت فاسد — تجاوز (متانة لا صمت: الفهرسة الفارغة تُرفض لاحقًا)
                        return 0;
                    }

                } // namespace

                bool loadPsfFont(const std::string &path, BitmapFont &font,
                                 std::string &error)
                {
                    std::ifstream in(path, std::ios::binary);
                    if (!in)
                    {
                        error = std::string(ERR_OPEN) + path;
                        return false;
                    }
                    auto data = std::make_shared<std::vector<uint8_t>>(
                        (std::istreambuf_iterator<char>(in)),
                        std::istreambuf_iterator<char>());
                    const std::vector<uint8_t> &d = *data;

                    std::size_t glyphCount = 0;
                    std::size_t glyphSize = 0; // بايتات الغليف الواحد
                    std::size_t glyphsOffset = 0;
                    uint8_t width = 0;
                    uint8_t height = 0;
                    bool isPsf2 = false;

                    if (d.size() >= PSF1_HEADER_SIZE &&
                        d[0] == PSF1_MAGIC_0 && d[1] == PSF1_MAGIC_1)
                    {
                        const uint8_t mode = d[2];
                        if ((mode & PSF1_MODE_HAS_TAB) == 0 &&
                            (mode & PSF1_MODE_HAS_SEQ) == 0)
                        {
                            error = std::string(ERR_NO_TABLE) + path;
                            return false;
                        }
                        glyphCount = (mode & PSF1_MODE_512) ? PSF1_GLYPH_COUNT_512
                                                            : PSF1_GLYPH_COUNT_BASE;
                        glyphSize = d[3]; // charsize = الارتفاع (عرض PSF1 ثابت 8)
                        glyphsOffset = PSF1_HEADER_SIZE;
                        width = PSF1_GLYPH_WIDTH;
                        height = static_cast<uint8_t>(glyphSize);
                    }
                    else if (d.size() >= PSF2_HEADER_SIZE &&
                             readLE32(d, 0) == PSF2_MAGIC)
                    {
                        isPsf2 = true;
                        const std::size_t headerSize = readLE32(d, 8);
                        const uint32_t flags = readLE32(d, 12);
                        glyphCount = readLE32(d, 16);
                        glyphSize = readLE32(d, 20);
                        const uint32_t h32 = readLE32(d, 24);
                        const uint32_t w32 = readLE32(d, 28);
                        if ((flags & PSF2_HAS_UNICODE_TABLE) == 0)
                        {
                            error = std::string(ERR_NO_TABLE) + path;
                            return false;
                        }
                        if (w32 == 0 || h32 == 0 || w32 > 0xFF || h32 > 0xFF ||
                            glyphSize != ((w32 + 7) / 8) * h32)
                        {
                            error = std::string(ERR_GEOMETRY) + path;
                            return false;
                        }
                        glyphsOffset = headerSize;
                        width = static_cast<uint8_t>(w32);
                        height = static_cast<uint8_t>(h32);
                    }
                    else
                    {
                        error = std::string(ERR_MAGIC) + path;
                        return false;
                    }

                    const std::size_t tableOffset = glyphsOffset + glyphCount * glyphSize;
                    if (glyphSize == 0 || glyphCount == 0 || tableOffset >= d.size())
                    {
                        error = std::string(ERR_TRUNCATED) + path;
                        return false;
                    }

                    font = BitmapFont{};
                    font.name = path; // مسار الملفّ معرِّفًا تشخيصيًّا (بيانات وقت التشغيل)
                    font.charWidth = width;
                    font.charHeight = height;
                    font.lineHeight = static_cast<uint8_t>(height + LINE_HEIGHT_MARGIN);
                    font.monospace = true;
                    font.ownedGlyphData = data; // ملكيّة البيانات تعيش مع الخطّ (نسخًا آمنًا)

                    // ── قراءة جدول Unicode: مدخل لكلّ غليف بالترتيب ──
                    std::size_t glyph = 0;
                    if (!isPsf2)
                    {
                        // PSF1: قيم UTF-16LE؛ 0xFFFF نهاية مدخل الغليف، 0xFFFE بداية
                        // تسلسلات التراكيب (تُتجاهَل حتى نهاية المدخل — حدّ معلَن).
                        std::size_t i = tableOffset;
                        bool inSequences = false;
                        while (i + 1 < d.size() && glyph < glyphCount)
                        {
                            const uint16_t v = readLE16(d, i);
                            i += 2;
                            if (v == PSF1_SEPARATOR)
                            {
                                ++glyph;
                                inSequences = false;
                                continue;
                            }
                            if (v == PSF1_START_SEQ)
                            {
                                inSequences = true;
                                continue;
                            }
                            if (!inSequences)
                            {
                                addGlyphEntry(font, v,
                                              d.data() + glyphsOffset + glyph * glyphSize,
                                              width, height);
                            }
                        }
                    }
                    else
                    {
                        // PSF2: بايتات UTF-8؛ 0xFF نهاية مدخل الغليف، 0xFE بداية التراكيب.
                        std::size_t i = tableOffset;
                        bool inSequences = false;
                        while (i < d.size() && glyph < glyphCount)
                        {
                            const uint8_t b = d[i];
                            if (b == PSF2_SEPARATOR)
                            {
                                ++i;
                                ++glyph;
                                inSequences = false;
                                continue;
                            }
                            if (b == PSF2_START_SEQ)
                            {
                                ++i;
                                inSequences = true;
                                continue;
                            }
                            const uint32_t cp = decodeUTF8Table(d, i, d.size());
                            if (!inSequences && cp != 0)
                            {
                                addGlyphEntry(font, cp,
                                              d.data() + glyphsOffset + glyph * glyphSize,
                                              width, height);
                            }
                        }
                    }

                    if (font.glyphs.empty())
                    {
                        error = std::string(ERR_EMPTY_MAP) + path;
                        return false;
                    }
                    return true;
                }

            } // namespace psf
        } // namespace freestanding
    } // namespace ui
} // namespace sad
