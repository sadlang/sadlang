// بسم الله الرحمن الرحيم
// ======================================================================
// test_ui_render_displaylist.cpp — اختبار ذهبيّ لطبقة التخطيط + الرسم
// ======================================================================
// الوصف:
//   يمرّر أشجار IR تمثيليّة عبر LayoutEngine::layout() ثم
//   PlatformRenderer::render() باستخدام RecordingRenderer، ويقارن قائمة
//   الرسم الناتجة (display list) بقائمة ذهبيّة مثبَّتة. هذا يغطّي المنطق
//   المشترك أسفل شجرة IR (التخطيط + renderNode) الذي لا تمسّه طباعة_شجرة.
//
//   الحدّ المعروف: يتحقّق من هندسة الرسم وألوانه وترتيب النداءات فقط،
//   لا من صحّة البكسل الفعليّة (تنعيم/rasterization الخطّ) — تلك طبقة
//   لاحقة منفصلة (لقطة SDL بخطّ مثبَّت + عتبة، خارج بوّابة CI الحتميّة).
//
//   لتحديث القائمة الذهبيّة بعد تغيير مقصود في التخطيط/الرسم: شغّل
//   الاختبار بمتغيّر البيئة SAD_UI_DUMP_GOLDEN=1 — يطبع **كل** الحالات
//   دفعةً واحدة بين علامتَي BEGIN/END لتُنسَخ القائمة منها مباشرةً.
// ======================================================================

#include "sad_test_framework.h"
#include "recording_renderer.h"

#include "sad_ui/ir.h"
#include "sad_ui/layout.h"
#include "sad_ui/types.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

using namespace SadTest;
using namespace sad::ui;

// ── أبعاد سطح العرض الثابتة لكل الحالات ───────────────────────────────
static constexpr float kViewportW = 400.0f;
static constexpr float kViewportH = 600.0f;

// ── (أ) مفاتيح مُشتقّة من مصدر الحقيقة (أسماء معدّلات SoT) ──────────────
// تُشتقّ من modifierTypeToArabicName (types.cpp) بدل كتابتها يدويًّا — فأيّ
// تباعد بين الاختبار وSoT يُكشَف بالبناء، ولا نكرّر السلسلة العربيّة.
static const std::string K_COLOR = modifierTypeToArabicName(ModifierType::ForegroundColor); // لون
static const std::string K_ALIGN = modifierTypeToArabicName(ModifierType::Alignment);       // محاذاة
static const std::string K_GRAD = modifierTypeToArabicName(ModifierType::GradientColor);    // تدرج

// ── (ب) مفاتيح خصائص IR خاصّة بالمحرّك (ليست أسماء معدّلات) ─────────────
// هذه مفاتيح يقرؤها renderNode مباشرةً من IRNode ولا تقابل معدّلًا في SoT
// (أو تخالف اسمه)، فتبقى ثوابت hex موثَّقة بموضع قراءتها في platform_renderer.cpp:
static const std::string K_TITLE = "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86";                 // عنوان — نصّ الزرّ (فرع Button)
static const std::string K_CONTENT = "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89";               // محتوى — نصّ العنصر النصّيّ (فرع Text)
static const std::string K_TEXT = "\xd9\x86\xd8\xb5";                                          // نص — مفتاح نصّ احتياطيّ عامّ
static const std::string K_BG = "\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9";                     // خلفية — مفتاح IR (المعدّل الرسميّ «لون_خلفية»)
static const std::string K_FONTSZ = "\xd8\xad\xd8\xac\xd9\x85_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7"; // حجم_الخط — يقرؤه renderNode (المعدّل «حجم_خط»)
static const std::string K_GRAD_END = "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9"; // تدرج_نهاية — لون نهاية التدرّج (فرع Button)
static const std::string K_VALUE = "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9";                          // قيمة — قيمة حقل النصّ (فرع TextField)
static const std::string K_OPACITY = "\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81\xd9\x8a\xd8\xa9";         // شفافية — مفتاح IR (المعدّل «عتامة»)
static const std::string K_SRC = "\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1";                            // مصدر — مسار الصورة (فرع Image)

// ── قيَم نصّيّة للاختبار ───────────────────────────────────────────────
static const std::string V_HELLO_AR = "\xd9\x85\xd8\xb1\xd8\xad\xd8\xa8\xd8\xa7";               // مرحبا

// ── أدوات بناء موجزة لأشجار IR ────────────────────────────────────────
// النمط: mk(نوع) لإنشاء عقدة، ثمّ withStr/withNum لإسناد خاصيّة IR وإرجاع
// المرجع للتسلسل. مقصودة للإيجاز داخل الحالات فقط.
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

// تشغيل التخطيط + الرسم وإرجاع قائمة الرسم النصّيّة (سطح 400×600، اتّجاه محدَّد).
static std::string renderTreeDir(const std::shared_ptr<IRNode> &root,
                                 LayoutDirection dir)
{
    LayoutEngine engine;
    engine.setDirection(dir);
    engine.setViewportSize(kViewportW, kViewportH);
    LayoutConstraints constraints;
    constraints.maxWidth = kViewportW;
    constraints.maxHeight = kViewportH;
    auto layoutResult = engine.layout(root, constraints);

    testing::RecordingRenderer r;
    r.initialize(nullptr);
    r.render(root, layoutResult);
    return r.str();
}

// يقارن القائمة بالذهبيّة (باتّجاه محدَّد)، أو يطبعها في وضع التحديث.
static void goldenCheckDir(const std::string &name,
                           const std::shared_ptr<IRNode> &root,
                           LayoutDirection dir,
                           const std::string &expected)
{
    std::string actual = renderTreeDir(root, dir);
    if (std::getenv("SAD_UI_DUMP_GOLDEN"))
    {
        std::cout << "\n===== BEGIN " << name << " =====\n"
                  << actual
                  << "===== END " << name << " =====\n";
        return;
    }
    if (actual != expected)
    {
        // عرضٌ متعدّد الأسطر غير مُهرَّب للعين (أوضح من رسالة SAD_ASSERT_EQ
        // المسطّرة) — ثمّ التوكيد يفرض الفشل ويطبع Expected/Actual رسميًّا.
        std::cout << "\n[MISMATCH " << name << "] actual display list:\n"
                  << actual << "----\n";
    }
    SAD_ASSERT_EQ(actual, expected);
}

// الحالة الافتراضيّة: RTL (العربيّة).
static void goldenCheck(const std::string &name,
                        const std::shared_ptr<IRNode> &root,
                        const std::string &expected)
{
    goldenCheckDir(name, root, LayoutDirection::RTL, expected);
}

int main()
{
    SAD_TEST_INIT();
    SAD_GROUP("SadUI Render Display-List Golden Tests");

    // 1) زرّ مفرد بعنوان ASCII — ظلّ + خلفية دائريّة أساسيّة + نصّ موسَّط
    SAD_TEST("زرّ مفرد بعنوان", {
        auto btn = mk(UINodeType::Button);
        withStr(btn, K_TITLE, "OK");
        goldenCheck("button_ok", btn,
                    "rect (1.00,1.20) 120.00x48.00 #00000033\n"
                    "rrect (0.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                    "text \"OK\" (52.00,14.40) size=16.00 #FFFFFF\n");
    });

    // 2) نصّ لاتينيّ — محاذاة يسار (البداية)
    SAD_TEST("نصّ لاتينيّ", {
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Hello");
        goldenCheck("text_hello", txt,
                    "text \"Hello\" (0.00,0.00) size=16.00 #000000\n");
    });

    // 3) نصّ عربيّ — يُحاذى يمينًا (يختبر مسار المحاذاة RTL في renderNode)
    SAD_TEST("نصّ عربيّ", {
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, V_HELLO_AR);
        goldenCheck("text_arabic", txt,
                    "text \"" + V_HELLO_AR + "\" (10.00,0.00) size=16.00 #000000\n");
    });

    // 4) عمود بخلفية + زرّ + نصّ.
    //    خلفية الحاوية تُرسَم مرّةً واحدة (سطر rect واحد) بعد إصلاح الرسم
    //    المزدوج في renderNode — كان الاختبار قد كشف رسمها مرّتين.
    SAD_TEST("عمود بأبناء وخلفية", {
        auto col = mk(UINodeType::Column);
        withStr(col, K_BG, "#eeeeee");
        auto btn = mk(UINodeType::Button);
        withStr(btn, K_TITLE, "Go");
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Hi");
        col->addChild(btn);
        col->addChild(txt);
        goldenCheck("column_children", col,
                    "rect (0.00,0.00) 120.00x68.00 #EEEEEE\n"
                    "rect (1.00,1.20) 120.00x48.00 #00000033\n"
                    "rrect (0.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                    "text \"Go\" (52.00,14.40) size=16.00 #FFFFFF\n"
                    "text \"Hi\" (0.00,48.00) size=16.00 #000000\n");
    });

    // 5) بطاقة — ظلّ مزاح + خلفية دائريّة بيضاء
    SAD_TEST("بطاقة", {
        auto card = mk(UINodeType::Card);
        withStr(card, K_COLOR, "#ffffff");
        goldenCheck("card", card,
                    "rect (2.00,4.00) 50.00x50.00 #00000026\n"
                    "rrect (0.00,0.00) 50.00x50.00 r=12.00 #FFFFFF\n");
    });

    // 6) نصّ بحجم خطّ مخصّص — يتدفّق الحجم إلى قياس النصّ والارتفاع
    SAD_TEST("نصّ بحجم خطّ", {
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Big");
        withNum(txt, K_FONTSZ, 32.0);
        goldenCheck("text_sized", txt,
                    "text \"Big\" (0.00,0.00) size=32.00 #000000\n");
    });

    // 7) نصّ لاتينيّ موسَّط — مسار حساب المحاذاة المركزيّ (مستقلّ عن RTL)
    SAD_TEST("نصّ موسَّط", {
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Mid");
        withStr(txt, K_ALIGN, "center");
        goldenCheck("text_center", txt,
                    "text \"Mid\" (3.00,0.00) size=16.00 #000000\n");
    });

    // 8) زرّ بتدرّج — يختبر مسار drawLinearGradient (سطر gradient)
    SAD_TEST("زرّ بتدرّج", {
        auto btn = mk(UINodeType::Button);
        withStr(btn, K_TITLE, "G");
        withStr(btn, K_GRAD, "#ff0000");
        withStr(btn, K_GRAD_END, "#0000ff");
        goldenCheck("button_gradient", btn,
                    "rect (1.00,1.20) 120.00x48.00 #00000033\n"
                    "gradient (0.00,0.00) 120.00x48.00 #FF0000->#0000FF vertical r=8.00\n"
                    "text \"G\" (56.00,14.40) size=16.00 #FFFFFF\n");
    });

    // 9) حقل نصّ غير مركَّز — خلفية دائريّة + حدّ + نصّ القيمة (لا مؤشّر وامض)
    SAD_TEST("حقل نصّ غير مركَّز", {
        auto tf = mk(UINodeType::TextField);
        withStr(tf, K_VALUE, "abc");
        goldenCheck("textfield_unfocused", tf,
                    "rrect (0.00,0.00) 50.00x50.00 r=4.00 #FFFFFF\n"
                    "routline (0.00,0.00) 50.00x50.00 t=1.00 #9E9E9E\n"
                    "text \"abc\" (8.00,18.00) size=14.00 #000000\n");
    });

    // 10) صفّ بابنين — يختبر التخطيط المعتمد على الاتّجاه (RTL مقابل LTR)
    SAD_TEST("صفّ RTL", {
        auto row = mk(UINodeType::Row);
        auto a = mk(UINodeType::Button);
        withStr(a, K_TITLE, "A");
        auto b = mk(UINodeType::Button);
        withStr(b, K_TITLE, "B");
        row->addChild(a);
        row->addChild(b);
        // RTL: الابن الأوّل (A) يمينًا (x=120)، الثاني (B) يسارًا (x=0).
        goldenCheckDir("row_rtl", row, LayoutDirection::RTL,
                       "rect (121.00,1.20) 120.00x48.00 #00000033\n"
                       "rrect (120.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                       "text \"A\" (176.00,14.40) size=16.00 #FFFFFF\n"
                       "rect (1.00,1.20) 120.00x48.00 #00000033\n"
                       "rrect (0.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                       "text \"B\" (56.00,14.40) size=16.00 #FFFFFF\n");
    });

    SAD_TEST("صفّ LTR", {
        auto row = mk(UINodeType::Row);
        auto a = mk(UINodeType::Button);
        withStr(a, K_TITLE, "A");
        auto b = mk(UINodeType::Button);
        withStr(b, K_TITLE, "B");
        row->addChild(a);
        row->addChild(b);
        // LTR: معكوس — A يسارًا (x=0)، B يمينًا (x=120). يثبت اعتماد التخطيط
        // على الاتّجاه (نفس الشجرة، مواضع أفقيّة معكوسة).
        goldenCheckDir("row_ltr", row, LayoutDirection::LTR,
                       "rect (1.00,1.20) 120.00x48.00 #00000033\n"
                       "rrect (0.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                       "text \"A\" (56.00,14.40) size=16.00 #FFFFFF\n"
                       "rect (121.00,1.20) 120.00x48.00 #00000033\n"
                       "rrect (120.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                       "text \"B\" (176.00,14.40) size=16.00 #FFFFFF\n");
    });

    // 11) تمرير — حاوية ScrollView تُصدِر clip ثمّ clip-clear حول الأبناء
    SAD_TEST("تمرير وقصّ", {
        auto sv = mk(UINodeType::ScrollView);
        auto child = mk(UINodeType::Text);
        withStr(child, K_CONTENT, "X");
        sv->addChild(child);
        goldenCheck("scrollview_clip", sv,
                    "clip (0.00,0.00) 10.00x20.00\n"
                    "text \"X\" (0.00,0.00) size=16.00 #000000\n"
                    "clip-clear\n");
    });

    // 12) شفافية — currentNodeOpacity_ < 1 ⇒ applyOpacity يعدّل ألفا اللون (#RRGGBBAA)
    SAD_TEST("شفافية", {
        auto col = mk(UINodeType::Column);
        withStr(col, K_BG, "#ff0000");
        withNum(col, K_OPACITY, 0.5);
        auto t = mk(UINodeType::Text);
        withStr(t, K_CONTENT, "T");
        col->addChild(t);
        // شفافية 0.5 ⇒ ألفا اللون = 0.5×255 = 127.5 ⇒ بتر (لا تقريب، عبر
        // Color::toHexString) ⇒ 127 = 0x7F ⇒ #FF00007F. (0.5 يُمثَّل بدقّة
        // في float فالناتج حتميّ؛ عند إضافة شفافيّات أخرى اختر قيمًا يبعد
        // ناتج alpha×255 فيها عن حدود الأعداد الصحيحة تجنّبًا لبترٍ متذبذب.)
        // خلفية واحدة (rect واحد) بعد إصلاح الرسم المزدوج — وهو مهمّ هنا
        // تحديدًا: الرسم المزدوج كان يراكم الألفا بصريًّا على شبه الشفّاف.
        goldenCheck("opacity_half", col,
                    "rect (0.00,0.00) 10.00x20.00 #FF00007F\n"
                    "text \"T\" (0.00,0.00) size=16.00 #000000\n");
    });

    // 13) صورة بمسار — يختبر مسار drawImage (سطر image)
    SAD_TEST("صورة", {
        auto img = mk(UINodeType::Image);
        withStr(img, K_SRC, "logo.png");
        goldenCheck("image_src", img,
                    "image \"logo.png\" (0.00,0.00) 100.00x100.00\n");
    });

    // 14) خطّ زمنيّ — يختبر مسار drawLine (سطر line)
    SAD_TEST("خطّ", {
        auto tl = mk(UINodeType::Timeline);
        goldenCheck("timeline_line", tl,
                    "line (7.00,0.00) (7.00,50.00) t=2.00 #2196F3\n");
    });

    // 15) هيكل عظميّ — يختبر تدرّجًا أفقيّا (vertical=false ⇒ horizontal)
    SAD_TEST("تدرّج أفقيّ", {
        auto sk = mk(UINodeType::Skeleton);
        goldenCheck("skeleton_hgradient", sk,
                    "gradient (0.00,0.00) 50.00x50.00 #D1D1D1->#EAEAEA horizontal r=4.00\n");
    });

    // 16) تهريب النصّ — نصّ يحوي اقتباسًا ⇒ يجب أن يظهر \" في القائمة
    SAD_TEST("تهريب اقتباس", {
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_TEXT, "a\"b");
        goldenCheck("text_escape", txt,
                    "text \"a\\\"b\" (0.00,0.00) size=16.00 #000000\n");
    });

    // 17) نداء مباشر للأوّليّات — يغطّي beginFrame/endFrame/circle/line
    //     التي لا يصلها render() (لا عنصر IR يخفّض إليها)، ويتحقّق من تنسيقها.
    SAD_TEST("أوّليّات مباشرة", {
        testing::RecordingRenderer r;
        r.initialize(nullptr);
        r.beginFrame(Color{0, 0, 0, 1});
        r.drawCircle(10.0f, 20.0f, 5.0f, Color::fromNamed(NamedColor::Red));
        r.drawCircleOutline(10.0f, 20.0f, 5.0f, Color::fromNamed(NamedColor::Blue), 2.0f);
        r.drawLine(0.0f, 0.0f, 100.0f, 50.0f, Color::fromNamed(NamedColor::Green), 1.0f);
        r.endFrame();
        std::string actual = r.str();
        std::string expected =
            "begin-frame #000000\n"
            "circle (10.00,20.00) r=5.00 #F44336\n"
            "circleoutline (10.00,20.00) r=5.00 t=2.00 #2196F3\n"
            "line (0.00,0.00) (100.00,50.00) t=1.00 #4CAF50\n"
            "end-frame\n";
        if (std::getenv("SAD_UI_DUMP_GOLDEN"))
            std::cout << "\n===== BEGIN primitives_direct =====\n"
                      << actual << "===== END primitives_direct =====\n";
        else
        {
            if (actual != expected)
                std::cout << "\n[MISMATCH primitives_direct] actual:\n" << actual << "----\n";
            SAD_ASSERT_EQ(actual, expected);
        }
    });

    SAD_SUMMARY();
}
