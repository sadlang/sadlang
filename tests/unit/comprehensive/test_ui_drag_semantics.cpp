// بسم الله الرحمن الرحيم
// ======================================================================
// test_ui_drag_semantics.cpp — حارس انحدار لدلالة السحب المستمرّ
// ======================================================================
// الوصف:
//   يغذّي MouseEventProcessor بتسلسل أحداث حقيقيّ (ضغط ← حركات ← رفع)
//   ويؤكّد عقد السحب الذي تعتمده ميزة «سحب النافذة بمنطق ص» (FR-009):
//
//     ١) OnDrag يُطلَق **مستمرًّا** لكلّ حركة بعد تجاوز العتبة، لا مرّةً
//        واحدة عند الرفع.
//     ٢) دلتاه **تفاضليّة** (منذ الحركة السابقة) لا إجماليّة — ومجموعها
//        يساوي المسافة الكاملة بالضبط (لا تضيع حركة عبور العتبة).
//     ٣) يُطلَق على العنصر **المُمسَك** (تحت نقطة بدء الضغط) ولو غادره
//        المؤشّر — لأنّ العنصر نفسه يتحرّك أثناء السحب.
//     ٤) لا يُعاد إطلاقه بالدلتا الإجماليّة عند الرفع (وإلّا قفز العنصر).
//     ٥) لا سحب بلا ضغطةٍ سابقة (لا سحب «شبحيّ»).
//
//   هذه الدلالة مشتركة بين كلّ المنصّات (سطح المكتب/المفسّر/الوضع الحرّ)
//   لأنّ الجميع يمرّ عبر MouseEventProcessor نفسه.
// ======================================================================

#include "sad_test_framework.h"

#include "sad_ui/ir.h"
#include "sad_ui/ir_builder.h"
#include "sad_ui/node.h"
#include "sad_ui/mouse_processor.h"
#include "sad_ui/types.h"

#include <memory>
#include <string>
#include <vector>

using namespace SadTest;
using namespace sad::ui;

namespace
{
    /// سجلّ إطلاق حدث واحد (النوع + العقدة + الدلتا) لفحص التسلسل.
    struct FiredEvent
    {
        IREventType type;
        const IRNode *node;
        double deltaX;
        double deltaY;
    };

    /// (AR) هل العقدة الهدف ما زالت داخل الشجرة الحيّة؟ مقارنة مؤشّرات فقط
    ///      (بلا فكّ إسناد الهدف) فتصحّ حتّى لو كانت العقدة قد تحرّرت.
    bool nodeInTree(const IRNode &root, const IRNode *target)
    {
        if (&root == target)
            return true;
        for (const auto &child : root.getChildren())
            if (child && nodeInTree(*child, target))
                return true;
        return false;
    }

    /// مشهد اختبار: شجرة من عنصرين متجاورين + معالج موصول بسجلّ الإطلاق.
    struct DragScene
    {
        std::shared_ptr<IRNode> root;
        IRNode *target = nullptr; ///< العنصر المسحوب (يحمل أحداث السحب)
        IRNode *other = nullptr;  ///< عنصر مجاور (لالتقاط الإمساك الخاطئ)
        std::vector<FiredEvent> fired;
        MouseEventProcessor proc;
        uint32_t clock = 0;

        /// اختبار نقر هندسيّ بسيط: العنصر الهدف يشغل [0,100]×[0,100]،
        /// والمجاور [200,300]×[0,100] — بينهما فجوة تُرجع nullptr.
        /// (AR) كإنتاجٍ فعليّ: لا يُرجَع إلّا عقدٌ حيّةٌ في الشجرة الحاليّة —
        ///      فبعد استبدال الجذر تُحرَّر target/other فلا تُعاد (مقارنة
        ///      مؤشّرات فقط، بلا فكّ إسناد العقدة المحرَّرة).
        const IRNode *hit(float x, float y)
        {
            const IRNode *candidate = nullptr;
            if (y >= 0.0f && y <= 100.0f)
            {
                if (x >= 0.0f && x <= 100.0f)
                    candidate = target;
                else if (x >= 200.0f && x <= 300.0f)
                    candidate = other;
            }
            if (candidate && root && nodeInTree(*root, candidate))
                return candidate;
            return nullptr;
        }
    };

    std::unique_ptr<DragScene> makeScene()
    {
        auto s = std::make_unique<DragScene>();
        s->root = std::make_shared<IRNode>(UINodeType::Column);

        auto targetNode = std::make_shared<IRNode>(UINodeType::Button);
        targetNode->addEvent(IREvent{IREventType::OnDrag, "عند_سحب"});
        targetNode->addEvent(IREvent{IREventType::OnDragStart, "عند_بدء"});
        targetNode->addEvent(IREvent{IREventType::OnDragEnd, "عند_انتهاء"});
        s->target = targetNode.get();
        s->root->addChild(targetNode);

        auto otherNode = std::make_shared<IRNode>(UINodeType::Button);
        otherNode->addEvent(IREvent{IREventType::OnDrag, "عند_سحب_آخر"});
        s->other = otherNode.get();
        s->root->addChild(otherNode);

        DragScene *raw = s.get();
        raw->proc.setHitTestCallback([raw](float x, float y) -> const IRNode *
                                     { return raw->hit(x, y); });
        raw->proc.setGetContentRootCallback([raw]() -> const IRNode *
                                            { return raw->root.get(); });
        raw->proc.setGetTimeMsCallback([raw]() -> uint32_t
                                       { return raw->clock; });
        raw->proc.setFireEventCallback(
            [raw](IREventType type, const std::string &, const IRNode *node,
                  const EventData &d)
            { raw->fired.push_back(FiredEvent{type, node, d.deltaX, d.deltaY}); });
        return s;
    }

    std::vector<FiredEvent> only(const std::vector<FiredEvent> &all, IREventType t)
    {
        std::vector<FiredEvent> out;
        for (const auto &e : all)
            if (e.type == t)
                out.push_back(e);
        return out;
    }

    MouseButtonState leftDown()
    {
        MouseButtonState b;
        b.leftPressed = true;
        return b;
    }
} // namespace

int main()
{
    SAD_TEST_INIT();
    SAD_GROUP("دلالة السحب المستمرّ / Continuous drag semantics");

    // ── ١+٢: إطلاق مستمرّ بدلتا تفاضليّة مجموعها = المسافة الكاملة ──
    SAD_TEST("OnDrag مستمرّ بدلتا تفاضليّة لا إجماليّة",
    {
        auto s = makeScene();
        s->proc.onMouseDown(MouseButton::Left, 10.0f, 10.0f);
        // ثلاث حركات: الأولى تعبر العتبة (5 بكسل)، ثمّ حركتان
        s->proc.onMouseMove(30.0f, 20.0f, leftDown());
        s->proc.onMouseMove(50.0f, 30.0f, leftDown());
        s->proc.onMouseMove(70.0f, 40.0f, leftDown());

        auto drags = only(s->fired, IREventType::OnDrag);
        // إطلاق واحد لكلّ حركة (لا واحد فقط عند الرفع)
        SAD_ASSERT_EQ(static_cast<size_t>(3), drags.size());

        // الدلتا تفاضليّة: (30-10,20-10) ثمّ (20,10) ثمّ (20,10)
        SAD_ASSERT_EQ(20.0, drags[0].deltaX);
        SAD_ASSERT_EQ(10.0, drags[0].deltaY);
        SAD_ASSERT_EQ(20.0, drags[1].deltaX);
        SAD_ASSERT_EQ(20.0, drags[2].deltaX);

        // مجموع الدلتا = المسافة الكاملة بالضبط (لا تضيع حركة عبور العتبة)
        double sumX = 0.0, sumY = 0.0;
        for (const auto &d : drags) { sumX += d.deltaX; sumY += d.deltaY; }
        SAD_ASSERT_EQ(60.0, sumX); // 70 - 10
        SAD_ASSERT_EQ(30.0, sumY); // 40 - 10
    });

    // ── ٣: الإطلاق على العنصر المُمسَك ولو غادره المؤشّر ──
    SAD_TEST("السحب يبقى على العنصر المُمسَك بعد مغادرة المؤشّر",
    {
        auto s = makeScene();
        s->proc.onMouseDown(MouseButton::Left, 50.0f, 50.0f); // فوق الهدف
        s->proc.onMouseMove(150.0f, 50.0f, leftDown());       // الفجوة (لا عنصر)
        s->proc.onMouseMove(250.0f, 50.0f, leftDown());       // فوق المجاور

        auto drags = only(s->fired, IREventType::OnDrag);
        SAD_ASSERT_EQ(static_cast<size_t>(2), drags.size());
        for (const auto &d : drags)
            SAD_ASSERT_TRUE(d.node == s->target); // لا يتحوّل للمجاور ولا يتوقّف
    });

    // ── ٤: لا إعادة إطلاق بالدلتا الإجماليّة عند الرفع ──
    SAD_TEST("الرفع لا يُعيد إطلاق OnDrag بالدلتا الإجماليّة",
    {
        auto s = makeScene();
        s->proc.onMouseDown(MouseButton::Left, 10.0f, 50.0f);
        // حركة تتجاوز عتبة التمرير (50) كي يدخل مسار كشف الإيماءة عند الرفع
        s->proc.onMouseMove(40.0f, 50.0f, leftDown());
        s->proc.onMouseMove(90.0f, 50.0f, leftDown());
        const size_t beforeUp = only(s->fired, IREventType::OnDrag).size();
        s->proc.onMouseUp(MouseButton::Left, 90.0f, 50.0f);

        SAD_ASSERT_EQ(beforeUp, only(s->fired, IREventType::OnDrag).size());
        // وOnDragEnd يصل العنصر المُمسَك (لا يضيع لأنّ العنصر تحرّك)
        auto ends = only(s->fired, IREventType::OnDragEnd);
        SAD_ASSERT_EQ(static_cast<size_t>(1), ends.size());
        SAD_ASSERT_TRUE(ends[0].node == s->target);
    });

    // ── ٥: لا سحب شبحيّ بلا ضغطة سابقة ──
    SAD_TEST("حركة بزرّ مضغوط بلا onMouseDown لا تُنتج سحبًا",
    {
        auto s = makeScene();
        s->proc.onMouseMove(50.0f, 50.0f, leftDown());
        s->proc.onMouseMove(90.0f, 50.0f, leftDown());
        SAD_ASSERT_EQ(static_cast<size_t>(0), only(s->fired, IREventType::OnDrag).size());
        SAD_ASSERT_EQ(static_cast<size_t>(0), only(s->fired, IREventType::OnDragStart).size());
    });

    // ── ٦: حركة دون العتبة لا تبدأ سحبًا ──
    SAD_TEST("حركة أصغر من عتبة السحب لا تُطلق شيئًا",
    {
        auto s = makeScene();
        s->proc.onMouseDown(MouseButton::Left, 50.0f, 50.0f);
        s->proc.onMouseMove(52.0f, 51.0f, leftDown()); // < 5 بكسل
        SAD_ASSERT_EQ(static_cast<size_t>(0), only(s->fired, IREventType::OnDrag).size());
    });

    // ── ٧: ضغطة جديدة تُصفّر حالة السحب (لا قفزة من سحبة سابقة) ──
    SAD_TEST("ضغطة جديدة تُصفّر الحالة فلا قفزة من السحبة السابقة",
    {
        auto s = makeScene();
        s->proc.onMouseDown(MouseButton::Left, 10.0f, 50.0f);
        s->proc.onMouseMove(80.0f, 50.0f, leftDown());
        // رفع «ضائع» (نحاكي فقدان التركيز): ننتقل لضغطة جديدة مباشرةً
        s->fired.clear();
        s->proc.onMouseDown(MouseButton::Left, 20.0f, 50.0f);
        s->proc.onMouseMove(40.0f, 50.0f, leftDown());

        auto drags = only(s->fired, IREventType::OnDrag);
        SAD_ASSERT_EQ(static_cast<size_t>(1), drags.size());
        // الدلتا من نقطة الضغط الجديدة (20) لا من موضع السحبة السابقة (80)
        SAD_ASSERT_EQ(20.0, drags[0].deltaX);
    });

    // ── ٨: استبدال الشجرة أثناء السحب يوقف الإطلاق (حارس استعمال-بعد-التحرير) ──
    //     ردّ نداء السحب قد يستدعي تحديث_حالة فتُعاد بناء الشجرة وتتحرّر العقدة
    //     المُمسَكة؛ يجب أن يتوقّف السحب بأمان لا أن يقرأ ذاكرةً محرَّرة.
    SAD_TEST("استبدال الشجرة أثناء السحب يوقف الإطلاق بأمان",
    {
        auto s = makeScene();
        s->proc.onMouseDown(MouseButton::Left, 50.0f, 50.0f);
        s->proc.onMouseMove(70.0f, 50.0f, leftDown());
        SAD_ASSERT_EQ(static_cast<size_t>(1), only(s->fired, IREventType::OnDrag).size());

        // تُستبدل الشجرة بأخرى لا تحوي العقدة المُمسَكة (نظير rebuildUI/REPLACE).
        // (AR) نُحاكي تسلسل الإنتاج بدقّة: setContent يصفّر المؤشّرات الخام
        //      (سحب/ضغط/تحويم) قبل تحرير العقد القديمة — وإلّا بقي تحويمٌ/ضغطٌ
        //      مُعلَّقًا على عقدةٍ محرَّرة فقُرئت ذاكرتها. الحارس هنا: بعد التصفير
        //      الصحيح لا يُطلَق أيّ حدثٍ على العقدة الميتة.
        s->proc.clearNodeRefs();
        s->root = std::make_shared<IRNode>(UINodeType::Column);
        s->fired.clear();
        s->proc.onMouseMove(90.0f, 50.0f, leftDown());
        s->proc.onMouseUp(MouseButton::Left, 90.0f, 50.0f);

        // لا إطلاق على عقدة محرَّرة — لا سحبًا ولا انتهاءَ سحب
        SAD_ASSERT_EQ(static_cast<size_t>(0), only(s->fired, IREventType::OnDrag).size());
        for (const auto &e : s->fired)
            SAD_ASSERT_TRUE(e.node != s->target);
    });

    // ── ٩: معدّلا التمرير يُنتجان أحداث IR فعليّة (لا خصائص) ──
    //     حارس لانحدارٍ حقيقيّ: إضافة ModifierType جديد بلا تحديث قائمة
    //     معدّلات الأحداث في IRBuilder تجعله يسقط خاصّيّةً صامتة بلا حدث.
    SAD_TEST("معدّلا عند_السحب_يسار/يمين يُنتجان أحداث IR",
    {
        SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnSwipeLeft) ==
                        IREventType::OnSwipeLeft);
        SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnSwipeRight) ==
                        IREventType::OnSwipeRight);

        auto ui = UINode::create(UINodeType::Swipeable);
        ui->addModifier(Modifier::event(ModifierType::OnSwipeLeft, "إجراء_يسار"));
        ui->addModifier(Modifier::event(ModifierType::OnSwipeRight, "إجراء_يمين"));

        IRBuilder builder;
        auto ir = builder.buildNode(ui);
        SAD_ASSERT_TRUE(ir != nullptr);

        bool sawLeft = false, sawRight = false;
        for (const auto &e : ir->getEvents())
        {
            if (e.type == IREventType::OnSwipeLeft && e.expression == "إجراء_يسار") sawLeft = true;
            if (e.type == IREventType::OnSwipeRight && e.expression == "إجراء_يمين") sawRight = true;
        }
        SAD_ASSERT_TRUE(sawLeft);
        SAD_ASSERT_TRUE(sawRight);
    });

    SAD_SUMMARY();
}
