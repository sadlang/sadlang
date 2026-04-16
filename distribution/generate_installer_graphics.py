"""
إنشاء رسومات المُثبّت — شعار لغة ص
Generate installer graphics — Sad Language logo
"""
from PIL import Image, ImageDraw, ImageFont
import os
import sys

ASSETS_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "assets")

# ألوان الهوية البصرية
SADIQ_BLUE   = (41, 98, 255)     # أزرق رئيسي
SADIQ_DARK   = (15, 23, 42)      # كحلي غامق
SADIQ_LIGHT  = (248, 250, 252)   # رمادي فاتح جداً
SADIQ_ACCENT = (99, 102, 241)    # بنفسجي
SADIQ_GOLD   = (234, 179, 8)     # ذهبي
WHITE        = (255, 255, 255)


def find_arabic_font(size):
    """البحث عن خط عربي مناسب"""
    font_candidates = [
        "C:/Windows/Fonts/Tahoma.ttf",
        "C:/Windows/Fonts/tahomabd.ttf",
        "C:/Windows/Fonts/Arial.ttf",
        "C:/Windows/Fonts/arialbd.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/segoeuib.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "C:/Windows/Fonts/calibrib.ttf",
    ]
    for fp in font_candidates:
        if os.path.exists(fp):
            return ImageFont.truetype(fp, size)
    return ImageFont.load_default()


def find_bold_font(size):
    """البحث عن خط عربي عريض"""
    bold_candidates = [
        "C:/Windows/Fonts/tahomabd.ttf",
        "C:/Windows/Fonts/arialbd.ttf",
        "C:/Windows/Fonts/segoeuib.ttf",
        "C:/Windows/Fonts/calibrib.ttf",
        "C:/Windows/Fonts/Tahoma.ttf",
    ]
    for fp in bold_candidates:
        if os.path.exists(fp):
            return ImageFont.truetype(fp, size)
    return ImageFont.load_default()


def draw_sad_logo(draw, cx, cy, radius, color=WHITE):
    """رسم شعار ص — حرف ص مع نقطة تنوين"""
    # دائرة خلفية
    r = radius
    draw.ellipse(
        [cx - r, cy - r, cx + r, cy + r],
        fill=SADIQ_BLUE, outline=None
    )
    # حلقة داخلية
    inner_r = int(r * 0.85)
    draw.ellipse(
        [cx - inner_r, cy - inner_r, cx + inner_r, cy + inner_r],
        fill=None, outline=WHITE, width=max(2, r // 15)
    )
    # حرف ص كبير في المنتصف
    font_size = int(r * 1.3)
    font = find_bold_font(font_size)
    text = "ص"
    bbox = draw.textbbox((0, 0), text, font=font)
    tw = bbox[2] - bbox[0]
    th = bbox[3] - bbox[1]
    tx = cx - tw // 2
    ty = cy - th // 2 - int(r * 0.1)
    draw.text((tx, ty), text, fill=color, font=font)


def draw_code_decoration(draw, x, y, w, h):
    """رسم زخرفة كود عربي"""
    font = find_arabic_font(13)
    snippets = [
        'دالة مرحبا()',
        '  اطبع("سلام")',
        'نهاية',
        '',
        'متغير س = 42',
        'إذا (س > 0)',
        '  اطبع(س)',
        'نهاية',
    ]
    line_y = y
    for snippet in snippets:
        if line_y + 16 > y + h:
            break
        # خط نصف شفاف (رمادي خافت)
        draw.text((x, line_y), snippet, fill=(255, 255, 255, 40), font=font)
        line_y += 18


def create_wizard_banner():
    """
    إنشاء صورة البانر الكبيرة (العمود الأيسر في صفحتي الترحيب والختام)
    InnoSetup modern: 164×314 (minimum), scales up
    نستخدم 164×314
    """
    W, H = 164, 314
    img = Image.new("RGB", (W, H), SADIQ_DARK)
    draw = ImageDraw.Draw(img)

    # تدرج خلفية من أعلى لأسفل
    for y in range(H):
        ratio = y / H
        r = int(SADIQ_DARK[0] * (1 - ratio * 0.3) + SADIQ_BLUE[0] * ratio * 0.3)
        g = int(SADIQ_DARK[1] * (1 - ratio * 0.3) + SADIQ_BLUE[1] * ratio * 0.3)
        b = int(SADIQ_DARK[2] * (1 - ratio * 0.3) + SADIQ_BLUE[2] * ratio * 0.3)
        draw.line([(0, y), (W, y)], fill=(r, g, b))

    # خطوط زخرفية هندسية
    for i in range(0, H, 30):
        opacity = max(20, 60 - abs(i - H // 2) // 3)
        color = (SADIQ_ACCENT[0], SADIQ_ACCENT[1], SADIQ_ACCENT[2])
        alpha_color = tuple(max(0, min(255, c * opacity // 255)) for c in color)
        draw.line([(0, i), (W, i + 15)], fill=alpha_color, width=1)

    # شعار ص في المنتصف العلوي
    logo_y = 80
    draw_sad_logo(draw, W // 2, logo_y, 45)

    # اسم اللغة
    font_name = find_bold_font(18)
    text_ar = "لغة ص"
    bbox = draw.textbbox((0, 0), text_ar, font=font_name)
    tw = bbox[2] - bbox[0]
    draw.text((W // 2 - tw // 2, logo_y + 55), text_ar, fill=SADIQ_GOLD, font=font_name)

    # النص الإنجليزي
    font_en = find_arabic_font(11)
    text_en = "Sad Language"
    bbox = draw.textbbox((0, 0), text_en, font=font_en)
    tw = bbox[2] - bbox[0]
    draw.text((W // 2 - tw // 2, logo_y + 80), text_en, fill=(180, 190, 210), font=font_en)

    # الإصدار
    font_ver = find_arabic_font(10)
    ver = "v1.0.0"
    bbox = draw.textbbox((0, 0), ver, font=font_ver)
    tw = bbox[2] - bbox[0]
    draw.text((W // 2 - tw // 2, logo_y + 98), ver, fill=(120, 130, 150), font=font_ver)

    # قوسي كود < > في الأسفل
    font_code = find_bold_font(28)
    code_symbol = "< / >"
    bbox = draw.textbbox((0, 0), code_symbol, font=font_code)
    tw = bbox[2] - bbox[0]
    draw.text((W // 2 - tw // 2, H - 50), code_symbol, fill=(60, 70, 100), font=font_code)

    # خط فاصل ذهبي
    draw.line([(20, logo_y + 118), (W - 20, logo_y + 118)], fill=SADIQ_GOLD, width=1)

    # ميزات صغيرة
    font_feat = find_arabic_font(10)
    features = ["مفسر + مترجم", "مكتبة قياسية", "دعم IDE"]
    fy = logo_y + 130
    for feat in features:
        bullet = "◆ " + feat
        bbox = draw.textbbox((0, 0), bullet, font=font_feat)
        tw = bbox[2] - bbox[0]
        draw.text((W // 2 - tw // 2, fy), bullet, fill=(160, 170, 200), font=font_feat)
        fy += 20

    path = os.path.join(ASSETS_DIR, "wizard_banner.bmp")
    img.save(path, "BMP")
    print(f"  ✓ wizard_banner.bmp ({W}×{H})")
    return path


def create_wizard_small():
    """
    إنشاء الأيقونة الصغيرة (تظهر في رأس الصفحات)
    InnoSetup modern: 55×58 (minimum)
    نستخدم 55×58
    """
    W, H = 55, 58
    img = Image.new("RGB", (W, H), SADIQ_DARK)
    draw = ImageDraw.Draw(img)

    # تدرج
    for y in range(H):
        ratio = y / H
        r = int(SADIQ_DARK[0] + (SADIQ_BLUE[0] - SADIQ_DARK[0]) * ratio * 0.4)
        g = int(SADIQ_DARK[1] + (SADIQ_BLUE[1] - SADIQ_DARK[1]) * ratio * 0.4)
        b = int(SADIQ_DARK[2] + (SADIQ_BLUE[2] - SADIQ_DARK[2]) * ratio * 0.4)
        draw.line([(0, y), (W, y)], fill=(r, g, b))

    # شعار ص
    draw_sad_logo(draw, W // 2, H // 2, 22)

    path = os.path.join(ASSETS_DIR, "wizard_small.bmp")
    img.save(path, "BMP")
    print(f"  ✓ wizard_small.bmp ({W}×{H})")
    return path


def create_installer_header():
    """
    إنشاء صورة رأسية أوسع (اختياري — لتزيين الرأس)
    """
    W, H = 497, 58
    img = Image.new("RGB", (W, H), SADIQ_DARK)
    draw = ImageDraw.Draw(img)

    # تدرج أفقي
    for x in range(W):
        ratio = x / W
        r = int(SADIQ_DARK[0] * (1 - ratio) + SADIQ_BLUE[0] * ratio * 0.5)
        g = int(SADIQ_DARK[1] * (1 - ratio) + SADIQ_BLUE[1] * ratio * 0.5)
        b = int(SADIQ_DARK[2] * (1 - ratio) + SADIQ_BLUE[2] * ratio * 0.5)
        draw.line([(x, 0), (x, H)], fill=(r, g, b))

    # شعار في اليسار
    draw_sad_logo(draw, 32, H // 2, 20)

    # اسم
    font = find_bold_font(16)
    draw.text((60, 10), "لغة ص", fill=WHITE, font=font)
    font_en = find_arabic_font(11)
    draw.text((60, 32), "Sad Programming Language", fill=(180, 190, 210), font=font_en)

    # إصدار في اليمين
    font_ver = find_arabic_font(10)
    draw.text((W - 60, H // 2 - 6), "v1.0.0", fill=SADIQ_GOLD, font=font_ver)

    path = os.path.join(ASSETS_DIR, "installer_header.bmp")
    img.save(path, "BMP")
    print(f"  ✓ installer_header.bmp ({W}×{H})")
    return path


def create_icon_256():
    """إنشاء أيقونة 256×256"""
    W = 256
    img = Image.new("RGBA", (W, W), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # خلفية دائرية
    cx, cy = W // 2, W // 2
    r = W // 2 - 4

    # ظل
    draw.ellipse([cx - r + 4, cy - r + 4, cx + r + 4, cy + r + 4], fill=(0, 0, 0, 60))

    # دائرة رئيسية مع تدرج
    draw.ellipse([cx - r, cy - r, cx + r, cy + r], fill=SADIQ_BLUE)

    # حلقة داخلية
    inner_r = int(r * 0.88)
    draw.ellipse(
        [cx - inner_r, cy - inner_r, cx + inner_r, cy + inner_r],
        fill=None, outline=WHITE + (200,), width=3
    )

    # حرف ص
    font_size = int(r * 1.4)
    font = find_bold_font(font_size)
    text = "ص"
    bbox = draw.textbbox((0, 0), text, font=font)
    tw = bbox[2] - bbox[0]
    th = bbox[3] - bbox[1]
    draw.text(
        (cx - tw // 2, cy - th // 2 - 10),
        text,
        fill=WHITE,
        font=font
    )

    # حفظ كـ PNG (يمكن تحويلها لـ ICO لاحقاً)
    path = os.path.join(ASSETS_DIR, "sad_icon_256.png")
    img.save(path, "PNG")
    print(f"  ✓ sad_icon_256.png ({W}×{W})")

    # حفظ كأيقونة ICO
    ico_path = os.path.join(ASSETS_DIR, "sad_icon.ico")
    sizes = [(16, 16), (32, 32), (48, 48), (64, 64), (128, 128), (256, 256)]
    imgs = []
    for s in sizes:
        resized = img.resize(s, Image.LANCZOS)
        imgs.append(resized)
    imgs[0].save(ico_path, format="ICO", sizes=sizes, append_images=imgs[1:])
    print(f"  ✓ sad_icon.ico (multi-size: {', '.join(f'{s[0]}' for s in sizes)})")
    return path


if __name__ == "__main__":
    print("=" * 50)
    print(" إنشاء رسومات المُثبّت — لغة ص")
    print(" Generating installer graphics — Sad Language")
    print("=" * 50)

    os.makedirs(ASSETS_DIR, exist_ok=True)

    create_wizard_banner()
    create_wizard_small()
    create_installer_header()
    create_icon_256()

    print("=" * 50)
    print(" ✅ تم إنشاء جميع الرسومات بنجاح!")
    print(f" 📁 المجلد: {ASSETS_DIR}")
    print("=" * 50)
