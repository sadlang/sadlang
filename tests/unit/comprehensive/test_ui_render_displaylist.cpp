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
static const std::string K_WIDTH = "\xd8\xb9\xd8\xb1\xd8\xb6";                                  // عرض — عرض ثابت للعنصر
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
                    "rrect (-4.00,-2.80) 128.00x56.00 r=10.00 #00000006\n"
                    "rrect (-2.67,-1.47) 125.33x53.33 r=9.33 #0000000C\n"
                    "rrect (-1.33,-0.13) 122.67x50.67 r=8.67 #00000013\n"
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
    //    محاذاة متقاطعة RTL: الزرّ بعرض كامل (120) يبقى عند x=0، والنصّ «Hi»
    //    الأضيق (20) يُحاذى إلى يمين العمود (x = 120 − 20 = 100) — بداية المحتوى
    //    من اليمين في RTL (يختبر مسار المحور المتقاطع في LayoutEngine::arrange).
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
                    "rect (0.00,0.00) 120.00x72.00 #EEEEEE\n"
                    "rrect (-4.00,-2.80) 128.00x56.00 r=10.00 #00000006\n"
                    "rrect (-2.67,-1.47) 125.33x53.33 r=9.33 #0000000C\n"
                    "rrect (-1.33,-0.13) 122.67x50.67 r=8.67 #00000013\n"
                    "rrect (0.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                    "text \"Go\" (52.00,14.40) size=16.00 #FFFFFF\n"
                    "text \"Hi\" (100.00,48.00) size=16.00 #000000\n");
    });

    // 5) بطاقة — ظلّ مزاح + خلفية دائريّة بيضاء
    SAD_TEST("بطاقة", {
        auto card = mk(UINodeType::Card);
        withStr(card, K_COLOR, "#ffffff");
        goldenCheck("card", card,
                    "rrect (-14.00,-12.00) 78.00x78.00 r=19.00 #00000001\n"
                    "rrect (-12.00,-10.00) 74.00x74.00 r=18.00 #00000003\n"
                    "rrect (-10.00,-8.00) 70.00x70.00 r=17.00 #00000004\n"
                    "rrect (-8.00,-6.00) 66.00x66.00 r=16.00 #00000006\n"
                    "rrect (-6.00,-4.00) 62.00x62.00 r=15.00 #00000007\n"
                    "rrect (-4.00,-2.00) 58.00x58.00 r=14.00 #00000009\n"
                    "rrect (-2.00,0.00) 54.00x54.00 r=13.00 #0000000B\n"
                    "rrect (-8.00,-4.00) 66.00x66.00 r=16.00 #00000004\n"
                    "rrect (-6.00,-2.00) 62.00x62.00 r=15.00 #00000009\n"
                    "rrect (-4.00,0.00) 58.00x58.00 r=14.00 #0000000D\n"
                    "rrect (-2.00,2.00) 54.00x54.00 r=13.00 #00000012\n"
                    "rrect (-4.00,-2.40) 58.00x58.00 r=14.00 #00000003\n"
                    "rrect (-2.67,-1.07) 55.33x55.33 r=13.33 #00000007\n"
                    "rrect (-1.33,0.27) 52.67x52.67 r=12.67 #0000000B\n"
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
                    "rrect (-4.00,-2.80) 128.00x56.00 r=10.00 #00000006\n"
                    "rrect (-2.67,-1.47) 125.33x53.33 r=9.33 #0000000C\n"
                    "rrect (-1.33,-0.13) 122.67x50.67 r=8.67 #00000013\n"
                    "gradient (0.00,0.00) 120.00x48.00 #FF0000->#0000FF vertical r=8.00\n"
                    "text \"G\" (56.00,14.40) size=16.00 #FFFFFF\n");
    });

    // 9) حقل نصّ غير مركَّز — خلفية دائريّة + حدّ + نصّ القيمة (لا مؤشّر وامض)
    SAD_TEST("حقل نصّ غير مركَّز", {
        auto tf = mk(UINodeType::TextField);
        withStr(tf, K_VALUE, "abc");
        goldenCheck("textfield_unfocused", tf,
                    "rrect (0.00,0.00) 50.00x50.00 r=4.00 #9E9E9E\n"
                    "rrect (1.00,1.00) 48.00x48.00 r=3.00 #FFFFFF\n"
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
                       "rrect (116.00,-2.80) 128.00x56.00 r=10.00 #00000006\n"
                    "rrect (117.33,-1.47) 125.33x53.33 r=9.33 #0000000C\n"
                    "rrect (118.67,-0.13) 122.67x50.67 r=8.67 #00000013\n"
                    "rrect (120.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                    "text \"A\" (176.00,14.40) size=16.00 #FFFFFF\n"
                    "rrect (-4.00,-2.80) 128.00x56.00 r=10.00 #00000006\n"
                    "rrect (-2.67,-1.47) 125.33x53.33 r=9.33 #0000000C\n"
                    "rrect (-1.33,-0.13) 122.67x50.67 r=8.67 #00000013\n"
                    "rrect (0.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                    "text \"B\" (56.00,14.40) size=16.00 #FFFFFF\n");
    });

    // 10و) محاذاة الصفّ المتقاطعة (عموديّة): وسط/نهاية/تمدّد. صفّ بارتفاع 100.
    SAD_TEST("صفّ محاذاة وسط", {
        auto row = mk(UINodeType::Row);
        withNum(row, "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", 100.0); // ارتفاع=100
        withStr(row, "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9", "\xd9\x88\xd8\xb3\xd8\xb7"); // وسط
        auto ch = mk(UINodeType::Container);
        withStr(ch, K_BG, "#abcdef");
        withNum(ch, K_WIDTH, 40.0);
        withNum(ch, "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", 40.0); // ارتفاع=40
        row->addChild(ch);
        // وسط عموديّ: y = (100−40)/2 = 30. الصفّ بلا عرض صريح ينكمش لعرض المحتوى
        // (40)، فالطفل الوحيد عند x=0 (بداية RTL داخل صندوق العرض 40).
        goldenCheckDir("row_cross_center", row, LayoutDirection::RTL,
                       "rect (0.00,30.00) 40.00x40.00 #ABCDEF\n");
    });

    SAD_TEST("صفّ محاذاة تمدّد", {
        auto row = mk(UINodeType::Row);
        withNum(row, "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", 100.0); // ارتفاع=100
        withStr(row, "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9",
                "\xd8\xaa\xd9\x85\xd8\xaf\xd9\x91\xd8\xaf"); // تمدّد
        auto ch = mk(UINodeType::Container);
        withStr(ch, K_BG, "#abcdef");
        withNum(ch, K_WIDTH, 40.0);
        row->addChild(ch);
        // تمدّد عموديّ: يملأ ارتفاع الصفّ 100 عند y=0 (x=0، الصفّ بعرض المحتوى 40).
        goldenCheckDir("row_cross_stretch", row, LayoutDirection::RTL,
                       "rect (0.00,0.00) 40.00x100.00 #ABCDEF\n");
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
                       "rrect (-4.00,-2.80) 128.00x56.00 r=10.00 #00000006\n"
                    "rrect (-2.67,-1.47) 125.33x53.33 r=9.33 #0000000C\n"
                    "rrect (-1.33,-0.13) 122.67x50.67 r=8.67 #00000013\n"
                    "rrect (0.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                    "text \"A\" (56.00,14.40) size=16.00 #FFFFFF\n"
                    "rrect (116.00,-2.80) 128.00x56.00 r=10.00 #00000006\n"
                    "rrect (117.33,-1.47) 125.33x53.33 r=9.33 #0000000C\n"
                    "rrect (118.67,-0.13) 122.67x50.67 r=8.67 #00000013\n"
                    "rrect (120.00,0.00) 120.00x48.00 r=8.00 #1E88E5\n"
                    "text \"B\" (176.00,14.40) size=16.00 #FFFFFF\n");
    });

    // 10ب) محاذاة عمود متقاطعة — نصّ ضيّق داخل عمود عريض: RTL يمينًا، LTR يسارًا.
    //      يثبت أنّ المحور المتقاطع للعمود يعتمد الاتّجاه (أصل «الشاشة تبدأ من
    //      الشمال»: كان النصّ يلتصق يسارًا في الحالتين قبل الإصلاح).
    SAD_TEST("عمود RTL متقاطع", {
        auto col = mk(UINodeType::Column);
        withNum(col, K_WIDTH, 200.0);
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Hi");  // عرض 20 ⇒ يمينًا عند x = 200 − 20 = 180
        col->addChild(txt);
        goldenCheckDir("column_cross_rtl", col, LayoutDirection::RTL,
                       "text \"Hi\" (180.00,0.00) size=16.00 #000000\n");
    });

    SAD_TEST("عمود LTR متقاطع", {
        auto col = mk(UINodeType::Column);
        withNum(col, K_WIDTH, 200.0);
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Hi");  // LTR ⇒ يسارًا عند x = 0
        col->addChild(txt);
        goldenCheckDir("column_cross_ltr", col, LayoutDirection::LTR,
                       "text \"Hi\" (0.00,0.00) size=16.00 #000000\n");
    });

    // 10ب٢) محاذاة عمود صريحة: وسط ونهاية (اتّجاهيّة) + إقحام الهامش.
    SAD_TEST("عمود محاذاة وسط", {
        auto col = mk(UINodeType::Column);
        withNum(col, K_WIDTH, 200.0);
        withStr(col, "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9", "\xd9\x88\xd8\xb3\xd8\xb7"); // محاذاة=وسط
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Hi");  // عرض 20 ⇒ وسط عند (200−20)/2 = 90
        col->addChild(txt);
        goldenCheckDir("column_center", col, LayoutDirection::RTL,
                       "text \"Hi\" (90.00,0.00) size=16.00 #000000\n");
    });

    SAD_TEST("عمود محاذاة نهاية RTL", {
        auto col = mk(UINodeType::Column);
        withNum(col, K_WIDTH, 200.0);
        withStr(col, "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9", "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9"); // محاذاة=نهاية
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Hi");  // نهاية في RTL = يسار المحتوى ⇒ x=0
        col->addChild(txt);
        goldenCheckDir("column_end_rtl", col, LayoutDirection::RTL,
                       "text \"Hi\" (0.00,0.00) size=16.00 #000000\n");
    });

    SAD_TEST("عمود بهامش RTL", {
        auto col = mk(UINodeType::Column);
        withNum(col, K_WIDTH, 200.0);
        withNum(col, "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4", 20.0); // هامش=20
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Hi");  // بداية RTL بهامش 20 ⇒ x = 200−20−20 = 160
        col->addChild(txt);
        goldenCheckDir("column_margin_rtl", col, LayoutDirection::RTL,
                       "text \"Hi\" (160.00,20.00) size=16.00 #000000\n");
    });

    // 10ب٣) عمود موزون بحشو — الابن الموزون يملأ منطقة المحتوى (ارتفاع القيد
    //       المنفذ 600 − حشو×2 = 580) بدءًا من y=حشو، لا 560 (كان خصم padding
    //       مزدوجًا يُنقِصه بمقدار حشو×2). يثبت إصلاح دَين الخصم المزدوج.
    SAD_TEST("عمود موزون بحشو", {
        auto col = mk(UINodeType::Column);
        withNum(col, K_WIDTH, 100.0);
        withNum(col, "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9", 10.0); // حشوة=10
        auto child = mk(UINodeType::Container);
        withStr(child, K_BG, "#123456");
        withNum(child, "\xd9\x88\xd8\xb2\xd9\x86", 1.0); // وزن=1
        col->addChild(child);
        // الحاوية الموزونة تملأ منطقة المحتوى الرأسيّة كاملةً: ارتفاع 580 (لا 560).
        goldenCheckDir("column_weight_padding", col, LayoutDirection::RTL,
                       "rect (40.00,10.00) 50.00x580.00 #123456\n");
    });

    // 10ب٤) عمود موزون بارتفاع صريح — التوزيع ضمن الارتفاع الصريح (200−حشو×2=180)
    //       لا القيد المنفذ (كان يتجاهل الارتفاع الصريح فيملأ 580).
    SAD_TEST("عمود موزون بارتفاع صريح", {
        auto col = mk(UINodeType::Column);
        withNum(col, K_WIDTH, 100.0);
        withNum(col, "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", 200.0); // ارتفاع=200
        withNum(col, "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9", 10.0); // حشوة=10
        auto child = mk(UINodeType::Container);
        withStr(child, K_BG, "#123456");
        withNum(child, "\xd9\x88\xd8\xb2\xd9\x86", 1.0); // وزن=1
        col->addChild(child);
        // الابن الموزون يملأ الارتفاع الصريح المنقوص: 200 − 20 = 180 عند y=10.
        goldenCheckDir("column_weight_explicit_h", col, LayoutDirection::RTL,
                       "rect (40.00,10.00) 50.00x180.00 #123456\n");
    });

    // 10ب٥) عمود محاذاة تمدّد — الابن يملأ عرض المحتوى كاملًا (لا عرضه الجوهريّ).
    //       عرض 200، حشو 10 ⇒ عرض المحتوى 180؛ الحاوية تُرسَم بعرض 180 عند x=10.
    SAD_TEST("عمود محاذاة تمدّد", {
        auto col = mk(UINodeType::Column);
        withNum(col, K_WIDTH, 200.0);
        withNum(col, "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9", 10.0); // حشوة=10
        withStr(col, "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9",
                "\xd8\xaa\xd9\x85\xd8\xaf\xd9\x91\xd8\xaf"); // محاذاة=تمدّد
        auto child = mk(UINodeType::Container);
        withStr(child, K_BG, "#abcdef");
        col->addChild(child);
        // الحاوية تملأ عرض المحتوى 180 عند x=10 (لا عرضها الجوهريّ 50).
        goldenCheckDir("column_stretch", col, LayoutDirection::RTL,
                       "rect (10.00,10.00) 180.00x50.00 #ABCDEF\n");
    });

    // 10ب٦) تمدّد + ابن بعرض صريح — العرض الصريح يفوز (لا تمدّد)، ويُحاذى بداية RTL.
    //       عرض العمود 200/حشو 10 ⇒ محتوى [10,190]؛ ابن عرضه الصريح 60 ⇒ يبقى 60
    //       ويُحاذى يمين المحتوى (بداية RTL) عند x = 190−60 = 130 (لا يتمدّد لـ180).
    SAD_TEST("تمدّد مع عرض صريح للابن", {
        auto col = mk(UINodeType::Column);
        withNum(col, K_WIDTH, 200.0);
        withNum(col, "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9", 10.0); // حشوة=10
        withStr(col, "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9",
                "\xd8\xaa\xd9\x85\xd8\xaf\xd9\x91\xd8\xaf"); // محاذاة=تمدّد
        auto child = mk(UINodeType::Container);
        withStr(child, K_BG, "#abcdef");
        withNum(child, K_WIDTH, 60.0); // عرض صريح=60 ⇒ يفوز على التمدّد
        col->addChild(child);
        goldenCheckDir("column_stretch_explicit_child", col, LayoutDirection::RTL,
                       "rect (130.00,10.00) 60.00x50.00 #ABCDEF\n");
    });

    // 10ج) مكدّس RTL — أبناء متراكبون يُحاذَون يمينًا (نصّ ضيّق في مكدّس عريض).
    SAD_TEST("مكدّس RTL", {
        auto st = mk(UINodeType::Stack);
        withNum(st, K_WIDTH, 200.0);
        auto txt = mk(UINodeType::Text);
        withStr(txt, K_CONTENT, "Hi");  // عرض 20 ⇒ يمينًا عند x = 200 − 20 = 180
        st->addChild(txt);
        goldenCheckDir("stack_rtl", st, LayoutDirection::RTL,
                       "text \"Hi\" (180.00,0.00) size=16.00 #000000\n");
    });

    // 10د) شبكة RTL — عكس ترتيب الأعمدة: العنصر 0 في العمود الأيمن، العنصر 1 يساره.
    //      عرض الخليّة = قيد المنفذ/الأعمدة = 400/2 = 200؛ عكس الأعمدة ⇒ A في العمود 1
    //      (x=200) وB في العمود 0 (x=0). (يثبت التدفّق يمين⇒يسار لا القياس المطلق.)
    SAD_TEST("شبكة RTL", {
        auto grid = mk(UINodeType::Grid);
        withNum(grid, "\xd8\xa3\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9", 2.0);  // أعمدة=2
        auto a = mk(UINodeType::Text); withStr(a, K_CONTENT, "A");
        auto b = mk(UINodeType::Text); withStr(b, K_CONTENT, "B");
        grid->addChild(a);
        grid->addChild(b);
        // عكس الأعمدة: A (العنصر 0) في العمود الأيمن x=200، B (العنصر 1) في الأيسر x=0.
        goldenCheckDir("grid_rtl", grid, LayoutDirection::RTL,
                       "text \"A\" (200.00,0.00) size=16.00 #000000\n"
                       "text \"B\" (0.00,0.00) size=16.00 #000000\n");
    });

    SAD_TEST("شبكة LTR", {
        auto grid = mk(UINodeType::Grid);
        withNum(grid, "\xd8\xa3\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9", 2.0);
        auto a = mk(UINodeType::Text); withStr(a, K_CONTENT, "A");
        auto b = mk(UINodeType::Text); withStr(b, K_CONTENT, "B");
        grid->addChild(a);
        grid->addChild(b);
        // LTR معكوس: A في العمود 0 (x=0)، B في العمود 1 (x=200).
        goldenCheckDir("grid_ltr", grid, LayoutDirection::LTR,
                       "text \"A\" (0.00,0.00) size=16.00 #000000\n"
                       "text \"B\" (200.00,0.00) size=16.00 #000000\n");
    });

    // 10ه) التفاف RTL — عنصران على سطر واحد يتدفّقان من اليمين (الأوّل أقصى اليمين).
    SAD_TEST("التفاف RTL", {
        auto wrap = mk(UINodeType::Wrap);
        withNum(wrap, K_WIDTH, 400.0);
        auto a = mk(UINodeType::Text); withStr(a, K_CONTENT, "A");  // عرض 10
        auto b = mk(UINodeType::Text); withStr(b, K_CONTENT, "B");  // عرض 10
        wrap->addChild(a);
        wrap->addChild(b);
        // A أوّلًا أقصى اليمين: حافّته اليمنى عند 400 ⇒ x=390؛ B يساره ⇒ x=380.
        goldenCheckDir("wrap_rtl", wrap, LayoutDirection::RTL,
                       "text \"A\" (390.00,0.00) size=16.00 #000000\n"
                       "text \"B\" (380.00,0.00) size=16.00 #000000\n");
    });

    // 11) تمرير — حاوية ScrollView تُصدِر clip ثمّ clip-clear حول الأبناء
    SAD_TEST("تمرير وقصّ", {
        auto sv = mk(UINodeType::ScrollView);
        auto child = mk(UINodeType::Text);
        withStr(child, K_CONTENT, "X");
        sv->addChild(child);
        goldenCheck("scrollview_clip", sv,
                    "clip (0.00,0.00) 10.00x24.00\n"
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
                    "rect (0.00,0.00) 10.00x24.00 #FF00007F\n"
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

    // ══════════════════════════════════════════════════════════════════════
    // 18) توحيدُ مسارِ الرسم — عُقَدٌ كانت محبوسةً في شوكةِ مُرسِّمٍ واحد.
    //
    //     قبلَ التوحيد: عُقَدُ قشرةِ سطحِ المكتب (نافذة/شريط_مهام/…) لا حالةَ
    //     لها في مُرسِّمِ SDL2 فتسقط للافتراضيِّ صامتةً (ث٤)، وعُقَدُ العرضِ
    //     المتقدّمِ (كتلة_كود/ورقة/…) لا حالةَ لها في المسارِ المشترَك فلا
    //     يراها الويبُ ولا المحمولُ ولا الوضعُ الحرّ.
    //
    //     الاختبارُ يُثبِت أنّ كلتا العائلتَينِ تُنتجان نداءاتِ رسمٍ فعليّةً
    //     من المسارِ المشترَكِ وحدَه — وهو ما يستهلكه كلُّ مُرسِّمٍ اليوم.
    //     البرهانُ سلوكيٌّ (قائمةُ رسمٍ غيرُ فارغة) لا شكليّ.
    // ══════════════════════════════════════════════════════════════════════
    SAD_TEST("عُقَد سطح المكتب تُرسَم على المسار المشترَك", {
        const UINodeType shellNodes[] = {
            UINodeType::Window, UINodeType::TitleBar, UINodeType::ScrollBar,
            UINodeType::Taskbar, UINodeType::StartMenu, UINodeType::SystemTray,
            UINodeType::SpinBox, UINodeType::GroupBox, UINodeType::Spinner,
            UINodeType::StatusBar};

        for (UINodeType type : shellNodes)
        {
            auto node = mk(type);
            // (AR) لا نضع خلفيّةً: الخطوةُ العامّةُ قبلَ الـswitch كانت سترسمها
            //      فيمرُّ الاختبارُ بلا تنفيذِ جسمِ الحالةِ أصلًا — برهانٌ كاذب.
            //      يُستثنى صندوقُ النظام: فارغًا لا يرسم شيئًا بحقّ (حاويةُ
            //      أيقوناتٍ)، فنمنحه نصًّا كي يسلك جسمَ حالتِه.
            if (type == UINodeType::SystemTray)
                withStr(node, K_TEXT, "10:30");
            std::string list = renderTreeDir(node, LayoutDirection::RTL);
            if (list.empty())
                std::cout << "\n[EMPTY] عقدةُ قشرةٍ لم تُنتج نداءَ رسم: "
                          << static_cast<int>(type) << "\n";
            SAD_ASSERT_TRUE(!list.empty());
        }
    });

    SAD_TEST("عُقَد العرض المتقدّم تُرسَم على المسار المشترَك", {
        const UINodeType advancedNodes[] = {
            UINodeType::ColorPicker, UINodeType::TabView, UINodeType::Sheet,
            UINodeType::Canvas, UINodeType::WebView, UINodeType::BottomSheet,
            UINodeType::Calendar, UINodeType::TimePicker, UINodeType::SideNav,
            UINodeType::MegaMenu, UINodeType::AudioPlayer, UINodeType::ImageGallery,
            UINodeType::ColorWheel, UINodeType::Expandable, UINodeType::Collapsible};

        for (UINodeType type : advancedNodes)
        {
            auto node = mk(type);
            std::string list = renderTreeDir(node, LayoutDirection::RTL);
            if (list.empty())
                std::cout << "\n[EMPTY] عقدةُ عرضٍ متقدّمٍ لم تُنتج نداءَ رسم: "
                          << static_cast<int>(type) << "\n";
            SAD_ASSERT_TRUE(!list.empty());
        }
    });

    // 19) «كتلة_كود» — العقدةُ الوحيدةُ التي كانت تعتمد على مُلوِّنٍ نحويٍّ
    //     يسكن خلفيّةَ سطحِ المكتب. نُقِل المُلوِّنُ إلى المكتبة، فصار الرسمُ
    //     مُلوَّنًا في كلِّ مُرسِّم. الذهبيّةُ تُثبِت التلوينَ لا مجرّدَ الخلفيّة.
    SAD_TEST("كتلة كود مُلوَّنة نحويًّا", {
        auto code = mk(UINodeType::CodeBlock);
        withStr(code, K_TEXT, "س = 1");
        goldenCheck("codeblock_highlighted", code,
                    "rrect (0.00,0.00) 50.00x50.00 r=6.00 #1E1E23\n"
                    "rrect (0.00,0.00) 23.80x50.00 r=6.00 #19191E\n"
                    "text \"1\" (11.80,8.00) size=13.00 #72727F\n"
                    "text \"س\" (31.80,8.00) size=13.00 #DBDBDB\n"
                    "text \"=\" (44.80,8.00) size=13.00 #DBDBDB\n"
                    "text \"1\" (57.80,8.00) size=13.00 #B5D66D\n");
    });

    // 20) أصنافُ الرموزِ الأربعةُ التي تأتي من جداولَ **مولَّدةٍ منفصلة**:
    //     كلمةٌ محجوزة (متغير)، وقيمةٌ منطقيّة (صحيح)، وعدمٌ (لاشيء)، ومدمجةٌ
    //     عامّة (اطبع). ولكلٍّ لونُه. والاختبارُ ضروريٌّ لا تجميليّ: «صحيح» و«خطأ»
    //     و«لاشيء» تسكن تصنيفَ `reserved` في مصدرِ الحقيقة، وفحصُ `classifyIdent`
    //     يبدأ بالكلماتِ المحجوزة — فلو وُلِّدت ضمنَها لعادت جميعًا بلونِ الكلمةِ
    //     المحجوزةِ ولَما بلغ الفحصُ `Boolean`/`NullLiteral` أبدًا. انحرافٌ في
    //     اللونِ وحدَه: لا خطأَ بناءٍ ولا تشغيل. هذه الذهبيّةُ تُثبِّت الفصل.
    SAD_TEST("كتلة كود — محجوزة ومنطقيّة وعدم ومدمجة", {
        auto code = mk(UINodeType::CodeBlock);
        withStr(code, K_TEXT, "متغير س = صحيح\nاطبع(لاشيء)");
        goldenCheck("codeblock_token_classes", code,
                    "rrect (0.00,0.00) 50.00x50.00 r=6.00 #1E1E23\n"
                    "rrect (0.00,0.00) 23.80x50.00 r=6.00 #19191E\n"
                    "text \"1\" (11.80,8.00) size=13.00 #72727F\n"
                    "text \"2\" (11.80,25.00) size=13.00 #72727F\n"
                    // متغير — كلمةٌ محجوزة
                    "text \"\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1\" (31.80,8.00) size=13.00 #C677FC\n"
                    "text \"\xd8\xb3\" (70.80,8.00) size=13.00 #DBDBDB\n"
                    "text \"=\" (83.80,8.00) size=13.00 #DBDBDB\n"
                    // صحيح — قيمةٌ منطقيّة: لونٌ مغايرٌ للمحجوزةِ وهو بيتُ القصيد
                    "text \"\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\" (96.80,8.00) size=13.00 #56B5E8\n"
                    // اطبع — مدمجةٌ عامّة
                    "text \"\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9\" (31.80,25.00) size=13.00 #60ADEF\n"
                    "text \"(\" (57.80,25.00) size=13.00 #FFD666\n"
                    // لاشيء — قيمةُ العدم
                    "text \"\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1\" (64.30,25.00) size=13.00 #56B5E8\n"
                    "text \")\" (96.80,25.00) size=13.00 #FFD666\n");
    });

    SAD_SUMMARY();
}
