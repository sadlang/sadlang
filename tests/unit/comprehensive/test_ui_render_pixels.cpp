// بسم الله الرحمن الرحيم
// ======================================================================
// test_ui_render_pixels.cpp — بوّابةُ البرهانِ البصريّ (لقطاتٌ ذهبيّةٌ للبكسل)
// ======================================================================
// الوصف:
//   الاختبارُ الجارُ `test_ui_render_displaylist` يقيسُ **نداءاتِ الرسم**
//   (هندسةً وألوانًا وترتيبًا) عبر RecordingRenderer، ويُعلِنُ حدَّه صراحةً:
//   لا بكسلَ يُقاس. فمُنقِّطٌ يكتبُ في الإحداثيِّ الخطأ، أو خلطٌ مقلوبٌ لألفا،
//   أو غليفٌ لا يُرسَم أصلًا — كلُّها تمرُّ خضراءَ هناك.
//
//   هذا الملفُّ يسدُّ ذلك الحدّ: يُنقِّطُ الشجرةَ نفسَها إلى **بكسلاتٍ فعليّة**
//   ويقارنُها بلقطةٍ ذهبيّةٍ مثبَّتةٍ في المستودع.
//
// لماذا المُصيِّرُ الحرُّ لا SDL2 — وهو قرارُ التصميمِ الأهمُّ هنا:
//   لقطةُ SDL2 غيرُ قابلةٍ للتكرارِ عبرَ المنصّاتِ لثلاثةِ أسبابٍ مقيسةٍ:
//     ١. الخطُّ من **النظام** لا من المستودع (arial.ttf على ويندوز،
//        Helvetica.ttc على ماك، DejaVuSans.ttf على لينكس) ⇒ ثلاثةُ خطوطٍ
//        مختلفةٍ لثلاثِ منصّات، فاللقطةُ تختلفُ حتمًا.
//     ٢. `SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC` مفروضانِ
//        عند إنشاءِ الرسّام ⇒ التنقيطُ على عتادٍ مختلفٍ في كلِّ عدّاء.
//     ٣. `SDL_WINDOW_SHOWN` مفروضٌ بلا خيارِ إخفاء ⇒ يلزمُ عرضٌ حيٌّ
//        (xvfb على لينكس، ولا مقابلَ له على ويندوز/ماك).
//   والمُصيِّرُ الحرُّ نقيضُ ذلك تمامًا: يكتبُ ARGB في ذاكرةٍ نعطيه إيّاها،
//   وخطُّه **نقطيٌّ مورَّدٌ داخلَ المستودع** (builtin_font.cpp)، وتشكيلُه
//   العربيُّ لنا (arabic_shaper.cpp) لا HarfBuzz، وبلا تنعيمِ حواف.
//   وحدُّ الدقّةِ صريحٌ هنا: الخلطُ عدديٌّ صحيح، أمّا ما فوقَه (التخطيطُ
//   وتكميمُ اللونِ وحسابُ الظلّ) فعائمٌ محلّيٌّ بحت — بلا عتادٍ ولا نظامٍ ولا
//   ملفٍّ خارجيّ. فالحتميّةُ **متوقَّعةٌ بالبناء** لا مضمونةٌ بالتصميم، والذي
//   يُثبِتُها فعلًا تشغيلُ البوّابةِ على المنصّاتِ الثلاث: تقريبُ العائمِ عند
//   التكميمِ هو الموضعُ الوحيدُ المرشَّحُ للانحراف، لا الخلط.
//   ويُبنى على المنصّاتِ الثلاثِ أصلًا ضمنَ `sad_graphics`.
//
// حدُّ الادّعاءِ صراحةً:
//   ما يُقاسُ هنا بكسلاتُ **المُصيِّرِ الحرّ**. مُصيِّرُ سطحِ المكتبِ (SDL2)
//   يتقاسمُ معه `PlatformRenderer::renderNode` — أي منطقَ التخطيطِ والقرارِ
//   والترتيبِ كلَّه — ويفترقُ عنه في الأوّليّاتِ وحدَها. فهذه البوّابةُ تُبرهِنُ
//   المشترَكَ حتّى البكسل، ولا تُبرهِنُ تنقيطَ SDL2 نفسَه. لا يُدَّعى غيرُ ذلك.
//
// صيغةُ اللقطة: PPM ثنائيّةٌ (P6) — بلا أيِّ اعتماديّة (stb_image_write غيرُ
//   مورَّدٍ في هذا المستودعِ أصلًا). يقرؤها GIMP وIrfanView وImageMagick
//   وإضافاتُ محرِّراتِ الكود — لا عارضُ صورِ ويندوز ولا المتصفّحات، فحوِّلْها
//   للعرضِ السريع: `magick convert x.ppm x.png`.
//   وتحملُ ترويستُها سطرَ تعليقٍ يُثبِّتُ **وضعَ الخطّ**، فانقلابُ الخطِّ إلى
//   الأطلسِ الرماديِّ (SAD_UI_HD_FONT) يُنتِجُ إخفاقًا صريحًا لا لقطةً مختلفةً
//   صامتة.
//
// لتحديثِ اللقطاتِ بعدَ تغييرٍ **مقصودٍ** في التخطيطِ أو التنقيط:
//   شغّلِ الاختبارَ بمتغيّرِ البيئةِ SAD_UI_DUMP_GOLDEN=1 — يكتبُ كلَّ اللقطاتِ
//   إلى مجلَّدِ الذهبيّاتِ مباشرةً (لا إلى الطرفيّةِ: هذه بياناتٌ ثنائيّة)،
//   ثمّ راجِعِ الفرقَ بعينِك قبلَ الإيداع.
//   ولا يعملُ الوضعُ إن كان `CI` مضبوطًا — ولو محلّيًّا في صدفتِك — كي لا
//   تُبارِكَ جولةُ التكاملِ انحدارًا بإعادةِ كتابةِ مرجعِها؛ ويُطبَعُ سببُ
//   التعطيلِ صراحةً بدلَ أن يسقطَ الطلبُ صامتًا إلى مقارنة.
// ======================================================================

#include "sad_test_framework.h"
#include "ui_tests_shared.h"

#include "sad_ui/ir.h"
#include "sad_ui/layout.h"
#include "sad_ui/types.h"
#include "sad_ui/freestanding/renderer.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace SadTest;
using namespace sad::ui;

// ── مجلَّدُ اللقطاتِ الذهبيّة (يحقنه CMake كي لا يعتمدَ على مجلَّدِ العمل) ──
#ifndef SAD_UI_PIXEL_GOLDEN_DIR
#error "SAD_UI_PIXEL_GOLDEN_DIR must be defined by the build system"
#endif

// ── أبعادُ سطحِ العرض ──────────────────────────────────────────────────
// (AR) صغيرةٌ عمدًا: اللقطةُ الواحدةُ تُخزَّنُ في المستودعِ فلا تُنفَخُ بلا
//      طائل — و200×150 تسعُ الزرَّ الافتراضيَّ (120×48) وعمودًا بابنَينِ معًا،
//      وتترك هامشًا يكشفُ رسمًا خارجَ الحدّ.
static constexpr uint32_t kPixelW = 200;
static constexpr uint32_t kPixelH = 150;

// (AR) لونُ المسحِ ثابتٌ ومعتِمٌ تمامًا: لو تُرِك شفّافًا لاختلط أثرُ خلطِ
//      ألفا بأثرِ الخلفيّةِ فصار الفشلُ غامضَ السبب.
static constexpr uint8_t kClearR = 0x10;
static constexpr uint8_t kClearG = 0x12;
static constexpr uint8_t kClearB = 0x18;

// ── ثوابتُ صيغةِ اللقطةِ ومساراتُها (لا سلاسلَ حرفيّةً في المنطق) ────────
static const char *const kPPMMagic = "P6";
static const char *const kPPMMaxValue = "255";
static const char *const kFontMarker = "font=";
static const char *const kGoldenSuffix = ".ppm";
static const char *const kActualSuffix = ".actual.ppm";
static const char *const kPathSeparator = "/";
static const char *const kPPMCommentPrefix = "# sad-ui pixel golden — ";
// (AR) متغيّرا البيئةِ في `ui_tests_shared.h` — يشترِكُ فيهما اختبارُ قائمةِ الرسم.

// ── أدواتُ بناءٍ موجزةٌ لأشجارِ IR ──────────────────────────────────────
static std::shared_ptr<IRNode> mk(UINodeType t) { return IRNode::create(t); }

static std::shared_ptr<IRNode> &withStr(std::shared_ptr<IRNode> &n,
                                        const std::string &key, const std::string &val)
{
    n->setProperty(key, val);
    return n;
}

static std::shared_ptr<IRNode> &withNum(std::shared_ptr<IRNode> &n,
                                        const std::string &key, double val)
{
    n->setProperty(key, val);
    return n;
}

// ══════════════════════════════════════════════════════════════════════
// اللقطة: مخزنُ بكسلاتٍ + ترميزُ/فكُّ PPM
// ══════════════════════════════════════════════════════════════════════

struct Snapshot
{
    uint32_t width = 0;
    uint32_t height = 0;
    std::string fontMode;
    std::vector<uint8_t> rgb; // ثلاثةُ بايتاتٍ لكلِّ بكسل
};

// (AR) الترميزُ يُسقِطُ قناةَ ألفا عمدًا: المخزنُ مُمسوحٌ بلونٍ معتِمٍ ثمّ
//      يُخلَطُ فوقَه، فالألفا الناتجةُ ثابتةٌ ولا تحملُ معلومةً — وإسقاطُها
//      يوفّرُ ربعَ حجمِ الملفِّ في المستودع.
static std::string encodePPM(const Snapshot &snap)
{
    std::ostringstream out;
    out << kPPMMagic << "\n";
    out << kPPMCommentPrefix << kFontMarker << snap.fontMode << "\n";
    out << snap.width << ' ' << snap.height << "\n" << kPPMMaxValue << "\n";
    std::string blob = out.str();
    blob.append(reinterpret_cast<const char *>(snap.rgb.data()), snap.rgb.size());
    return blob;
}

// (AR) فكُّ ترميزٍ متساهلٌ بقدرِ ما تسمحُ الصيغة: يقبلُ أسطرَ التعليقِ في أيِّ
//      موضعٍ من الترويسةِ كما ينصُّ تعريفُ PPM، ويستخرجُ وضعَ الخطِّ منها.
static bool decodePPM(const std::string &blob, Snapshot &out, std::string &error)
{
    size_t pos = 0;
    auto readToken = [&](std::string &token) -> bool {
        token.clear();
        while (pos < blob.size())
        {
            char c = blob[pos];
            if (c == '#')
            {
                size_t lineEnd = blob.find('\n', pos);
                if (lineEnd == std::string::npos)
                    return false;
                std::string comment = blob.substr(pos, lineEnd - pos);
                const std::string marker = kFontMarker;
                size_t at = comment.find(marker);
                if (at != std::string::npos)
                    out.fontMode = comment.substr(at + marker.size());
                pos = lineEnd + 1;
                continue;
            }
            if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
            {
                if (!token.empty())
                    return true;
                ++pos;
                continue;
            }
            token.push_back(c);
            ++pos;
        }
        return !token.empty();
    };

    std::string magic, w, h, maxVal;
    if (!readToken(magic) || magic != kPPMMagic)
    {
        error = "ليست لقطةَ P6 / not a P6 snapshot";
        return false;
    }
    if (!readToken(w) || !readToken(h) || !readToken(maxVal))
    {
        error = "ترويسةٌ ناقصة / truncated header";
        return false;
    }
    if (maxVal != kPPMMaxValue)
    {
        error = "عمقُ لونٍ غيرُ مدعوم / unsupported max value: " + maxVal;
        return false;
    }
    // (AR) بايتٌ فاصلٌ واحدٌ بعد الترويسةِ حسبَ التعريف.
    if (pos < blob.size())
        ++pos;

    // (AR) بُعدانِ غيرُ رقميَّينِ أو صفريّانِ يمرّانِ صامتَينِ عبرَ strtoul،
    //      فتصيرُ ذهبيّةٌ تالفةُ الترويسةِ «لقطةً بأبعادٍ مختلفة» — تشخيصٌ
    //      مضلِّلٌ يُطارَدُ في المُنقِّط. ونحرسُ الضربَ من الفيضِ كي لا يلتفَّ
    //      حجمٌ ضخمٌ إلى رقمٍ صغيرٍ يوافقُ حمولةً قصيرة.
    // (EN) Reject non-numeric/zero dimensions instead of letting strtoul turn a
    //      corrupt header into a plausible "size differs", and guard the product
    //      against wrapping.
    // (AR) و`unsigned long` أوسعُ من `uint32_t` على لينكس وماك، فقالبٌ بلا
    //      حدٍّ **يقتطعُ صامتًا**: ترويسةٌ فيها 4294967496 تصيرُ 200 فتوافقُ
    //      حمولةً سليمةً وتمرُّ ذهبيّةٌ تالفةُ الترويسةِ خضراء.
    // (EN) unsigned long is wider than uint32_t on Linux/macOS, so an unchecked
    //      cast truncates silently and a corrupt header passes as valid.
    char *widthEnd = nullptr;
    char *heightEnd = nullptr;
    errno = 0;
    unsigned long parsedWidth = std::strtoul(w.c_str(), &widthEnd, 10);
    unsigned long parsedHeight = std::strtoul(h.c_str(), &heightEnd, 10);
    const unsigned long maxDimension = std::numeric_limits<uint32_t>::max();
    const bool dimensionsWellFormed =
        errno == 0 && widthEnd && *widthEnd == '\0' && heightEnd &&
        *heightEnd == '\0' && parsedWidth > 0 && parsedHeight > 0 &&
        parsedWidth <= maxDimension && parsedHeight <= maxDimension;
    if (!dimensionsWellFormed)
    {
        error = "أبعادٌ غيرُ صالحة / invalid dimensions: " + w + "x" + h;
        return false;
    }
    out.width = static_cast<uint32_t>(parsedWidth);
    out.height = static_cast<uint32_t>(parsedHeight);
    const size_t maxRowsForWidth = (SIZE_MAX / 3u) / out.width;
    if (out.height > maxRowsForWidth)
    {
        error = "أبعادٌ تفيضُ الحجمَ / dimensions overflow the payload size";
        return false;
    }
    size_t expected = static_cast<size_t>(out.width) * out.height * 3u;
    if (blob.size() - pos != expected)
    {
        error = "حجمُ البياناتِ لا يطابقُ الأبعاد / payload size mismatch";
        return false;
    }
    out.rgb.assign(blob.begin() + static_cast<std::string::difference_type>(pos), blob.end());
    return true;
}

// ══════════════════════════════════════════════════════════════════════
// التنقيط: شجرةُ IR ⇒ بكسلات
// ══════════════════════════════════════════════════════════════════════

static Snapshot rasterize(const std::shared_ptr<IRNode> &root, LayoutDirection dir)
{
    LayoutEngine engine;
    engine.setDirection(dir);
    engine.setViewportSize(static_cast<float>(kPixelW), static_cast<float>(kPixelH));
    LayoutConstraints constraints;
    constraints.maxWidth = static_cast<float>(kPixelW);
    constraints.maxHeight = static_cast<float>(kPixelH);
    auto layoutResult = engine.layout(root, constraints);

    std::vector<uint32_t> pixels(static_cast<size_t>(kPixelW) * kPixelH, 0u);

    freestanding::FramebufferConfig fb;
    fb.address = pixels.data();
    fb.width = kPixelW;
    fb.height = kPixelH;
    fb.pitch = kPixelW * 4u;
    fb.bpp = 32;
    fb.backBuffer = nullptr; // (AR) بلا تخزينٍ مزدوج: نقرأُ ما رُسِم مباشرةً

    freestanding::FreestandingRenderOptions options;
    options.doubleBuffer = false;
    options.alphaBlending = true;
    options.debugBounds = false;
    options.scaleFactor = 1.0f;

    freestanding::FreestandingRenderer renderer;
    renderer.initializeFramebuffer(fb, options);
    renderer.loadBuiltinFont();

    Color clear;
    clear.r = static_cast<float>(kClearR) / 255.0f;
    clear.g = static_cast<float>(kClearG) / 255.0f;
    clear.b = static_cast<float>(kClearB) / 255.0f;
    clear.a = 1.0f;

    renderer.beginFrame(clear);
    renderer.render(root, layoutResult);
    renderer.endFrame();

    Snapshot snap;
    snap.width = kPixelW;
    snap.height = kPixelH;
    // (AR) من حالةِ المُصيِّرِ الفعليّةِ لا من ثابتٍ هنا — التعليلُ الكاملُ عند
    //      تصريحِ loadedFontName في رأسِ المُصيِّرِ الحرّ.
    snap.fontMode = renderer.loadedFontName();
    snap.rgb.resize(static_cast<size_t>(kPixelW) * kPixelH * 3u);
    for (size_t i = 0; i < pixels.size(); ++i)
    {
        uint32_t argb = pixels[i];
        snap.rgb[i * 3 + 0] = static_cast<uint8_t>((argb >> 16) & 0xFFu);
        snap.rgb[i * 3 + 1] = static_cast<uint8_t>((argb >> 8) & 0xFFu);
        snap.rgb[i * 3 + 2] = static_cast<uint8_t>(argb & 0xFFu);
    }
    return snap;
}

// ══════════════════════════════════════════════════════════════════════
// المقارنة والتقرير
// ══════════════════════════════════════════════════════════════════════

static std::string goldenPath(const std::string &name)
{
    return std::string(SAD_UI_PIXEL_GOLDEN_DIR) + kPathSeparator + name + kGoldenSuffix;
}

static std::string actualPath(const std::string &name)
{
    return std::string(SAD_UI_PIXEL_GOLDEN_DIR) + kPathSeparator + name + kActualSuffix;
}

static bool readFile(const std::string &path, std::string &out)
{
    std::ifstream in(path, std::ios::binary);
    if (!in)
        return false;
    std::ostringstream buf;
    buf << in.rdbuf();
    out = buf.str();
    return true;
}

static bool writeFile(const std::string &path, const std::string &data)
{
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out)
        return false;
    out.write(data.data(), static_cast<std::streamsize>(data.size()));
    return static_cast<bool>(out);
}

// (AR) تقريرُ الفرقِ هو ما يجعلُ لقطةً ثنائيّةً قابلةً للتشخيص: عددُ البكسلاتِ
//      المختلفةِ ونسبتُها، وأوّلُ موضعٍ اختلفَ بإحداثيَّيه وقيمتَيه، وأقصى
//      انحرافٍ في قناة. بلا هذا تكونُ الرسالةُ «الملفّانِ مختلفان» ولا أكثر.
static std::string diffReport(const Snapshot &expected, const Snapshot &actual)
{
    std::ostringstream rep;
    if (expected.width != actual.width || expected.height != actual.height)
    {
        rep << "  الأبعاد اختلفت / size differs: ذهبيّة "
            << expected.width << "x" << expected.height
            << " ≠ فعليّة " << actual.width << "x" << actual.height << "\n";
        return rep.str();
    }
    if (expected.fontMode != actual.fontMode)
    {
        rep << "  وضعُ الخطِّ اختلف / font mode differs: ذهبيّة '"
            << expected.fontMode << "' ≠ فعليّة '" << actual.fontMode << "'\n"
            << "  ⇒ هذا ليس انحدارَ تنقيطٍ بل تبدُّلَ خطٍّ (SAD_UI_HD_FONT؟).\n";
    }

    size_t differing = 0;
    int maxDelta = 0;
    bool haveFirst = false;
    size_t firstIndex = 0;
    // (AR) صندوقُ إحاطةِ الاختلاف: «أوّلُ اختلافٍ عند (س،ص)» يقولُ أين بدأ فقط،
    //      والصندوقُ يفصلُ غليفًا واحدًا تغيّر عن إزاحةِ تخطيطٍ شاملة — وهو
    //      الفرقُ بين دقيقةِ تشخيصٍ وساعة.
    uint32_t minX = expected.width;
    uint32_t maxX = 0;
    uint32_t minY = expected.height;
    uint32_t maxY = 0;
    const size_t total = expected.rgb.size() / 3u;
    for (size_t p = 0; p < total; ++p)
    {
        bool same = true;
        for (size_t c = 0; c < 3; ++c)
        {
            int delta = static_cast<int>(actual.rgb[p * 3 + c]) -
                        static_cast<int>(expected.rgb[p * 3 + c]);
            if (delta != 0)
            {
                same = false;
                int magnitude = delta < 0 ? -delta : delta;
                if (magnitude > maxDelta)
                    maxDelta = magnitude;
            }
        }
        if (!same)
        {
            ++differing;
            if (!haveFirst)
            {
                haveFirst = true;
                firstIndex = p;
            }
            const uint32_t x = static_cast<uint32_t>(p % expected.width);
            const uint32_t y = static_cast<uint32_t>(p / expected.width);
            if (x < minX) minX = x;
            if (x > maxX) maxX = x;
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
        }
    }

    if (differing == 0)
        return rep.str();

    double percent = total ? (100.0 * static_cast<double>(differing) / static_cast<double>(total)) : 0.0;
    rep << "  بكسلاتٌ مختلفة / differing pixels: " << differing << " من " << total
        << " (" << percent << "%)، أقصى انحرافِ قناة / max channel delta: " << maxDelta << "\n";

    char line[256];
    std::snprintf(line, sizeof(line),
                  "  أوّلُ اختلافٍ عند / first difference at (x=%u, y=%u): "
                  "ذهبيّة #%02X%02X%02X ≠ فعليّة #%02X%02X%02X\n",
                  static_cast<unsigned>(firstIndex % expected.width),
                  static_cast<unsigned>(firstIndex / expected.width),
                  expected.rgb[firstIndex * 3 + 0], expected.rgb[firstIndex * 3 + 1],
                  expected.rgb[firstIndex * 3 + 2],
                  actual.rgb[firstIndex * 3 + 0], actual.rgb[firstIndex * 3 + 1],
                  actual.rgb[firstIndex * 3 + 2]);
    rep << line;

    std::snprintf(line, sizeof(line),
                  "  صندوقُ الاختلاف / bounding box: x=%u..%u  y=%u..%u  (%ux%u)\n",
                  minX, maxX, minY, maxY, maxX - minX + 1u, maxY - minY + 1u);
    rep << line;
    return rep.str();
}

static void pixelGoldenWithDirection(const std::string &name,
                                     const std::shared_ptr<IRNode> &root,
                                     LayoutDirection dir)
{
    Snapshot actual = rasterize(root, dir);
    const std::string path = goldenPath(name);

    // (AR) وضعُ التحديثِ يُعيدُ كتابةَ الذهبيّاتِ ثمّ يمرُّ أخضرَ — وهو بابٌ خلفيٌّ
    //      للبوّابةِ لو تلوّثت بيئةُ CI بهذا المتغيّرِ عرَضًا: تصيرُ البوّابةُ
    //      تُبارِكُ كلَّ انحدارٍ بدل أن تكشفَه. يُقصَرُ على التشغيلِ المحلّيّ.
    // (EN) The dump mode rewrites goldens and passes — a back door if CI ever
    //      inherits the variable. Restrict it to local runs.
    const bool dumpRequested = goldenDumpRequested();
    const bool insideCI = insideContinuousIntegration();
    if (dumpRequested && !insideCI)
    {
        // (AR) تُكتَبُ ملفًّا لا إلى الطرفيّة: هذه بياناتٌ ثنائيّة.
        bool ok = writeFile(path, encodePPM(actual));
        std::cout << (ok ? "[GOLDEN WRITTEN] " : "[GOLDEN WRITE FAILED] ") << path << "\n";
        SAD_ASSERT_TRUE(ok);
        return;
    }
    // (AR) الرفضُ الصامتُ يُضلِّل: من ضبطَ `CI` في صدفتِه المحلّيّةِ يطلبُ
    //      التحديثَ فيراهُ يقارنُ، ثمّ تنصحُه رسالةُ الإخفاقِ بأن يفعلَ ما
    //      فعلَه للتوّ. نقولُ السببَ صراحةً مرّةً واحدة.
    // (EN) A silent refusal misleads: with CI set locally the update request
    //      silently becomes a comparison, and the failure message then advises
    //      exactly what was just attempted. Say why, once.
    if (dumpRequested && insideCI)
    {
        static bool warned = false;
        if (!warned)
        {
            warned = true;
            std::cerr << "  [تنبيه] " << kDumpGoldenEnv << " مضبوطٌ لكنّ "
                      << kContinuousIntegrationEnv
                      << " مضبوطٌ أيضًا ⇒ التحديثُ مُعطَّلٌ والمقارنةُ تجري / "
                      << "dump disabled because " << kContinuousIntegrationEnv
                      << " is set; comparing instead.\n";
        }
    }

    std::string blob;
    if (!readFile(path, blob))
    {
        std::cout << "\n[MISSING GOLDEN] " << path << "\n"
                  << "  شغّلِ الاختبارَ بـSAD_UI_DUMP_GOLDEN=1 لتوليدِها ثمّ راجِعْها بعينِك.\n";
        SAD_ASSERT_TRUE(false);
        return;
    }

    Snapshot expected;
    std::string error;
    if (!decodePPM(blob, expected, error))
    {
        std::cout << "\n[CORRUPT GOLDEN] " << path << " — " << error << "\n";
        SAD_ASSERT_TRUE(false);
        return;
    }

    const bool identical = (expected.width == actual.width &&
                            expected.height == actual.height &&
                            expected.fontMode == actual.fontMode &&
                            expected.rgb == actual.rgb);
    if (!identical)
    {
        // (AR) نكتبُ الفعليّةَ بجوارِ الذهبيّةِ باسمٍ مُعلَّم: مقارنةُ صورتَينِ
        //      بالعينِ أسرعُ من أيِّ تقريرٍ نصّيّ، وهذا الملفُّ **غيرُ متعقَّبٍ**
        //      في git (مستثنًى في .gitignore) فلا يتسرّبُ إلى إيداع.
        const std::string sidecar = actualPath(name);
        const bool wrote = writeFile(sidecar, encodePPM(actual));
        // (AR) إلى cerr لا cout: إطارُ الاختبارِ يطبعُ الإخفاقَ إلى cerr، فخلطُ
        //      المجريَينِ يُبعثِرُ التقريرَ عن سببِه في سجلِّ CI.
        std::cerr << "\n[PIXEL MISMATCH " << name << "]\n"
                  << diffReport(expected, actual)
                  << "  الذهبيّة / golden: " << path << "\n";
        if (wrote)
            std::cerr << "  الفعليّة / actual: " << sidecar << "\n";
        else
            std::cerr << "  تعذّرت كتابةُ اللقطةِ الفعليّة (شجرةٌ للقراءةِ فقط؟) / "
                         "could not write the actual snapshot: "
                      << sidecar << "\n";
        // (AR) تشخيصٌ بلا خطوةٍ تالية يترك القارئَ واقفًا: فرعُ «ذهبيّةٌ مفقودة»
        //      يذكرُ أمرَ التحديثِ وهذا كان يغفلُه.
        std::cerr << "  إن كان التغييرُ مقصودًا: شغّلْ محلّيًّا بـ" << kDumpGoldenEnv
                  << "=1 ثمّ راجِعِ الفرقَ بعينِك قبلَ الإيداع.\n"
                  << "  للعرض: GIMP أو IrfanView أو `magick convert x.ppm x.png`.\n";
    }
    SAD_ASSERT_TRUE(identical);
}

static void pixelGolden(const std::string &name, const std::shared_ptr<IRNode> &root)
{
    pixelGoldenWithDirection(name, root, LayoutDirection::RTL);
}

// ══════════════════════════════════════════════════════════════════════
// أشجارٌ تُستعمَلُ في أكثرَ من حالة
// ══════════════════════════════════════════════════════════════════════

// (AR) عمودٌ بابنَينِ مختلفَي العرض: الفارقُ في العرضِ هو ما يجعلُ محاذاةَ
//      المحورِ المتقاطعِ مرئيّةً أصلًا — ابنانِ بعرضٍ واحدٍ يُنتجانِ اللقطةَ
//      نفسَها في الاتّجاهَين فتصيرُ الحالةُ بلا قيمة.
static std::shared_ptr<IRNode> makeTwoChildColumn()
{
    auto column = mk(UINodeType::Column);
    auto first = mk(UINodeType::Container);
    withStr(first, K_BG, "#E53935");
    withNum(first, K_WIDTH, 60.0);
    auto second = mk(UINodeType::Container);
    withStr(second, K_BG, "#43A047");
    withNum(second, K_WIDTH, 100.0);
    column->addChild(first);
    column->addChild(second);
    return column;
}

int main()
{
    SAD_TEST_INIT();
    SAD_GROUP("SadUI Pixel Golden Snapshots");

    // (AR) ١) سطحٌ فارغ: يُثبِّتُ لونَ المسحِ وحدَه. لو انكسرَ هذا وحدَه فالعطبُ
    //      في beginFrame أو في ترتيبِ قنواتِ ARGB لا في رسمِ العناصر — عزلٌ
    //      يوفّرُ جولةَ تشخيصٍ كاملة.
    SAD_TEST("سطحٌ فارغ — لونُ المسحِ وترتيبُ القنوات", {
        auto root = mk(UINodeType::Container);
        pixelGolden("empty_surface", root);
    });

    // (AR) ٢) مستطيلٌ مصمَت: أبسطُ أوّليّةٍ — حدودٌ حادّةٌ بلا تنعيم.
    SAD_TEST("حاويةٌ بخلفيّةٍ مصمتة", {
        auto box = mk(UINodeType::Container);
        withStr(box, K_BG, "#1E88E5");
        withNum(box, K_WIDTH, 90.0);
        pixelGolden("container_solid", box);
    });

    // (AR) ٣) الزرّ: يجمعُ المستطيلَ الدائريَّ والظلَّ الطبقيَّ والنصَّ الموسَّط
    //      — أكثفُ حالةٍ أوّليّاتٍ في المكتبة.
    SAD_TEST("زرٌّ بعنوانٍ لاتينيّ — دائريّةٌ وظلٌّ ونصّ", {
        auto btn = mk(UINodeType::Button);
        withStr(btn, K_TITLE, "OK");
        pixelGolden("button_ok", btn);
    });

    // (AR) ٤) النصُّ العربيُّ: يمرُّ بالمُشكِّلِ السياقيِّ ثمّ بغليفاتِ الخطِّ
    //      المورَّد. هذه الحالةُ وحدَها تُبرهِنُ أنّ الحروفَ **رُسمت** لا أنّ
    //      نداءَ الرسمِ صدر — وهو الفرقُ الذي لا يراه الاختبارُ الجار.
    //      اللقطةُ المثبَّتةُ فُحصت بالعين: الترتيبُ من اليمينِ إلى اليسارِ
    //      صحيحٌ (م ر ح ب ا)، والأشكالُ **منفصلةٌ لا متّصلة** — وهذا حدُّ
    //      الخطِّ النقطيِّ المدمجِ لا عيبَ تنقيط: تغطيتُه U+0621–U+064A
    //      (أشكالٌ مستقلّة) بلا أشكالِ العرضِ المتّصلة (FE70–FEFC)، وتلك
    //      يحملُها الأطلسُ الرماديُّ HD خلفَ ماكرو بناءٍ خارجيّ. أُثبِتُ الحالةَ
    //      القائمةَ ولا أُجمّلُها.
    SAD_TEST("نصٌّ عربيٌّ — تشكيلٌ سياقيٌّ وغليفاتٌ مرسومة", {
        auto text = mk(UINodeType::Text);
        withStr(text, K_CONTENT, V_HELLO_AR);
        withStr(text, K_COLOR, "#FFFFFF");
        withNum(text, K_FONTSZ, 16.0);
        pixelGolden("text_arabic", text);
    });

    // (AR) ٥) الاتّجاهُ يُغيّرُ البكسلَ لا الشجرة: الشجرةُ **واحدةٌ** تُبنى مرّةً
    //      وتُنقَّطُ باتّجاهَين. النسخُ اليدويُّ كان سيجعلُ «العمودُ نفسُه» ادّعاءً
    //      يتعفّنُ عند أوّلِ تعديلٍ لنسخةٍ دونَ أختِها.
    SAD_TEST("عمودٌ بابنَين — RTL", {
        pixelGoldenWithDirection("column_children_rtl", makeTwoChildColumn(),
                                 LayoutDirection::RTL);
    });

    // (AR) ٦) الاتّجاهُ المقابل. ولا يكفي وجودُ لقطتَين: لو تساوتا لَمرّتِ
    //      البوّابةُ خضراءَ وهي تُبارِكُ محاذاةً لا أثرَ لها. التوكيدُ الصريحُ
    //      أدناه يجعلُ التساويَ فشلًا لا صمتًا.
    SAD_TEST("عمودٌ بابنَين — LTR", {
        pixelGoldenWithDirection("column_children_ltr", makeTwoChildColumn(),
                                 LayoutDirection::LTR);
    });

    SAD_TEST("الاتّجاهُ يُغيّرُ البكسلَ فعلًا — RTL ≠ LTR", {
        SAD_ASSERT_TRUE(rasterize(makeTwoChildColumn(), LayoutDirection::RTL).rgb !=
                        rasterize(makeTwoChildColumn(), LayoutDirection::LTR).rgb);
    });

    // (AR) ٧) محرفٌ خارجَ تغطيةِ الخطِّ المدمج: يسلكُ **مربّعَ الاستبدال** —
    //      وهو مسارٌ لا تراه قائمةُ الرسمِ إطلاقًا (النداءُ `text` نفسُه سواءٌ
    //      وُجد الغليفُ أم لا)، فلا يكشفُه إلّا البكسل. وقد كشفَ فعلًا: كان
    //      المربّعُ يُرسَمُ أبيضَ معتِمًا دائمًا لأنّ مكوّناتِ اللونِ أُسنِدت
    //      بمقياسِ ٠–٢٥٥ إلى بنيةٍ عقدُها ٠٫٠–١٫٠ فقُصَّت إلى ١٫٠. الآن يحملُ
    //      لونَ النصِّ ونصفَ العتامة، واللقطةُ تقفلُ ذلك.
    SAD_TEST("محرفٌ خارجَ تغطيةِ الخطّ — مربّعُ الاستبدال", {
        auto text = mk(UINodeType::Text);
        // (AR) 𝔄 (U+1D504) — خارجَ تغطيةِ الخطِّ النقطيِّ المدمجِ قطعًا.
        withStr(text, K_CONTENT, "\xf0\x9d\x94\x84");
        withStr(text, K_COLOR, "#E53935");
        withNum(text, K_FONTSZ, 16.0);
        pixelGolden("text_missing_glyph", text);
    });

    // (AR) ٨) بديلُ الصورة: الوضعُ الحرُّ بلا نظامِ ملفّاتٍ فيرسمُ مستطيلًا
    //      رماديًّا وصليبًا. مسارٌ آخرُ لا تراه قائمةُ الرسمِ (النداءُ `image`
    //      نفسُه سواءٌ حُمِّلت الصورةُ أم لا)، وهو ثاني موضعٍ أُسنِدت فيه مكوّناتُ
    //      اللونِ بمقياسِ ٠–٢٥٥ إلى بنيةٍ عقدُها ٠٫٠–١٫٠ ⇒ «رماديٌّ» أبيضُ معتِم.
    //      أُصلِح الموضعانِ معًا، واللقطةُ تقفلُ الاثنَين.
    SAD_TEST("صورةٌ متعذّرةٌ — مستطيلُ البديلِ والصليب", {
        auto image = mk(UINodeType::Image);
        withStr(image, K_SRC, "missing.png");
        pixelGolden("image_placeholder", image);
    });

    // (AR) ٩) عنصرٌ أعرضُ من السطح: يقفلُ حدَّ الكتابةِ في المخزن. مُنقِّطٌ
    //      يتجاوزُ الحدَّ إمّا يُفسِدُ ذاكرةً مجاورةً وإمّا يلتفُّ إلى بدايةِ
    //      السطرِ التالي — والالتفافُ **لا يُعطِّلُ شيئًا**، فيمرُّ صامتًا في كلِّ
    //      اختبارٍ لا يقرأُ البكسل.
    SAD_TEST("عنصرٌ يتجاوزُ حافّةَ السطح — حدُّ الكتابة", {
        auto wide = mk(UINodeType::Container);
        withStr(wide, K_BG, "#FB8C00");
        withNum(wide, K_WIDTH, 320.0); // أعرضُ من 200
        pixelGolden("overflow_clip", wide);
    });

    SAD_SUMMARY();
}
