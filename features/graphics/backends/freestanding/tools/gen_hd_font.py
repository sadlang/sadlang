#!/usr/bin/env python3
# بسم الله الرحمن الرحيم
# gen_hd_font.py — مولِّد أطلس الخطّ الرماديّ عالي الدقّة (HD) للباطن الحرّ
# ══════════════════════════════════════════════════════════════════════════════
# (AR) ينقّط مجموعة غليفات من خطّ TTF/OTF جيّد (Amiri افتراضًا) بتغطية رماديّة
#      8-بت (مضادّ التعرّج) عبر FreeType، ويُخرج ترويسة C++ مولَّدة يضمّها
#      builtin_font.cpp لبناء BitmapFont بـbpp=8. الحروف العربيّة تُنقّط بأشكال
#      العرض المتّصلة (FE70–FEFC) مباشرةً — وهي ما يُخرجه المُشكِّل shape() — فيظهر
#      النصّ **متّصلًا** وناعمًا بلا حاجة لطيّ أو HarfBuzz وقت التشغيل.
# (EN) Rasterizes a glyph set from a quality TTF (Amiri) to 8-bit grayscale
#      coverage via FreeType, emits a generated C++ header consumed by
#      builtin_font.cpp. Arabic is rasterized as connected Presentation Forms-B
#      (the shaper's output) so text renders connected + antialiased.
#
# الاستعمال:
#   python3 gen_hd_font.py <خطّ.ttf> <المخرَج.h> [حجم-البكسل]
# مثال (WSL):
#   python3 gen_hd_font.py /usr/share/fonts/opentype/fonts-hosny-amiri/Amiri-Regular.ttf \
#           ../include/sad_ui/freestanding/hd_font_generated.h 32
import sys

import freetype


def glyph_ranges():
    """(AR) مجموعة الغليفات المطلوبة — يُتخطّى ما لا شكل له في الخطّ."""
    cps = []
    cps += range(0x0020, 0x007F)              # ASCII قابل للطباعة (أرقام/لاتينيّ/رموز)
    cps += range(0x0621, 0x064B)              # عربيّ أساسيّ (احتياط + قياس)
    cps += range(0x0660, 0x066A)              # أرقام هنديّة عربيّة ٠–٩
    cps += [0x060C, 0x061B, 0x061F, 0x0640]   # فاصلة/فاصلة منقوطة/استفهام/تطويل
    cps += range(0x0670, 0x0671)              # ألف خنجريّة
    cps += range(0xFE70, 0xFEFD)              # أشكال العرض-B (متّصلة) — الأساس
    # إزالة التكرار مع حفظ الترتيب:
    seen = set()
    out = []
    for cp in cps:
        if cp not in seen:
            seen.add(cp)
            out.append(cp)
    return out


def main():
    if len(sys.argv) < 3:
        print("الاستعمال: gen_hd_font.py <خطّ.ttf> <المخرَج.h> [حجم-البكسل=32]")
        return 1
    font_path = sys.argv[1]
    out_path = sys.argv[2]
    px = int(sys.argv[3]) if len(sys.argv) > 3 else 32

    face = freetype.Face(font_path)
    face.set_pixel_sizes(0, px)
    ascender = face.size.ascender >> 6
    descender = face.size.descender >> 6     # سالب عادةً
    line_height = face.size.height >> 6
    if line_height <= 0:
        line_height = ascender - descender

    # (AR) charHeight/lineHeight في BitmapFont نوعهما uint8_t ⇒ مقاييس الخطّ يجب
    #      أن تسع 0..255. حارس فشل-مُغلق: حجم بكسل كبير (px≳120) يفيضها صامتًا
    #      فيُشوّه تباعد الأسطر. (أبعاد الغليف محروسة بـ_u8/_i8 لكلّ غليف أدناه.)
    _u8(px)
    _u8(ascender)
    _u8(line_height)

    data = bytearray()      # كلّ بايتات الألفا متسلسلة
    metas = []              # (cp, w, h, xo, yo, adv, offset)

    for cp in glyph_ranges():
        face.load_char(cp, freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_NORMAL)
        g = face.glyph
        bmp = g.bitmap
        w, h, pitch = bmp.width, bmp.rows, bmp.pitch
        adv = g.advance.x >> 6
        left = g.bitmap_left
        top = g.bitmap_top
        # المسافة وأمثالها بلا صورة لكن بتقدُّم — نُبقيها كتقدُّم فقط:
        if w == 0 or h == 0:
            if adv > 0:
                metas.append((cp, 0, 0, 0, 0, _u8(adv), 0))
            continue
        yo = ascender - top                 # قمّة الغليف نسبةً لقمّة صندوق السطر
        # إعادة رصّ الصفوف إلى w بايت/صفّ (إزالة حشو pitch):
        off = len(data)
        src = bmp.buffer
        for row in range(h):
            base = row * pitch
            data += bytes(src[base:base + w])
        metas.append((cp, _u8(w), _u8(h), _i8(left), _i8(yo), _u8(adv), off))

    _emit(out_path, font_path, px, ascender, line_height, data, metas)
    print(f"OK: {len(metas)} غليفًا، {len(data)} بايت ألفا ⇒ {out_path}")
    print(f"    px={px} ascender={ascender} line_height={line_height}")
    return 0


def _u8(v):
    if not (0 <= v <= 255):
        raise ValueError(f"قيمة خارج uint8: {v} — قلّل حجم البكسل")
    return v


def _i8(v):
    if not (-128 <= v <= 127):
        raise ValueError(f"قيمة خارج int8: {v} — قلّل حجم البكسل")
    return v & 0xFF


def _emit(out_path, font_path, px, ascender, line_height, data, metas):
    import os

    font_name = os.path.basename(font_path)
    lines = []
    a = lines.append
    a("// بسم الله الرحمن الرحيم")
    a("// hd_font_generated.h — أطلس الخطّ الرماديّ HD (مولَّد آليًّا — لا تحرّره يدويًّا)")
    a("// ══════════════════════════════════════════════════════════════════════════")
    a(f"// المصدر: {font_name}  |  حجم البكسل: {px}px  |  تغطية: 8-بت رماديّ")
    a("// المولِّد: features/graphics/backends/freestanding/tools/gen_hd_font.py")
    a("// يضمّه builtin_font.cpp لبناء BitmapFont بـbpp=8 (نصّ عربيّ متّصل ناعم).")
    a("#ifndef SAD_UI_FREESTANDING_HD_FONT_GENERATED_H")
    a("#define SAD_UI_FREESTANDING_HD_FONT_GENERATED_H")
    a("")
    a("#include <cstdint>")
    a("")
    a("namespace sad { namespace ui { namespace freestanding { namespace hdfont {")
    a("")
    a(f"inline constexpr int HD_PIXEL_SIZE = {px};")
    a(f"inline constexpr int HD_ASCENDER = {ascender};")
    a(f"inline constexpr int HD_LINE_HEIGHT = {line_height};")
    a(f"inline constexpr int HD_GLYPH_COUNT = {len(metas)};")
    a("")
    a("// (AR) وصف غليف: نقطة يونيكود، أبعاد، إزاحتان، تقدُّم، وإزاحة داخل مصفوفة الألفا.")
    a("struct HDGlyphMeta {")
    a("    uint32_t codepoint;")
    a("    uint8_t  width;")
    a("    uint8_t  height;")
    a("    int8_t   xOffset;")
    a("    int8_t   yOffset;")
    a("    uint8_t  advance;")
    a("    uint32_t dataOffset;")
    a("};")
    a("")
    a(f"inline constexpr uint8_t HD_ALPHA[{max(1, len(data))}] = {{")
    a(_hex_block(data))
    a("};")
    a("")
    a("inline constexpr HDGlyphMeta HD_GLYPHS[HD_GLYPH_COUNT] = {")
    for (cp, w, h, xo, yo, adv, off) in metas:
        a(f"    {{0x{cp:04X},{w},{h},{_s8(xo)},{_s8(yo)},{adv},{off}}},")
    a("};")
    a("")
    a("} } } } // namespace sad::ui::freestanding::hdfont")
    a("")
    a("#endif // SAD_UI_FREESTANDING_HD_FONT_GENERATED_H")
    with open(out_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")


def _s8(v):
    """(AR) بايت int8 مخزَّن كـ0..255 ⇒ نصّ int8 صحيح (-128..127)."""
    return v - 256 if v > 127 else v


def _hex_block(data):
    if not data:
        return "    0"
    out = []
    for i in range(0, len(data), 20):
        chunk = data[i:i + 20]
        out.append("    " + ",".join(str(b) for b in chunk) + ",")
    return "\n".join(out)


if __name__ == "__main__":
    sys.exit(main())
