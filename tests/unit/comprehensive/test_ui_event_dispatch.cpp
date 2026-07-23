// بسم الله الرحمن الرحيم
// ======================================================================
// test_ui_event_dispatch.cpp — حارس انحدار لقلب إرسال الأحداث المشترك
// ======================================================================
// الوصف:
//   يؤكّد عقد `dispatchEvent` — المُرسِل الواحد الذي صار يخدم المحرّكات
//   الثلاثة (سطح المكتب، الوضع الحرّ، المفسّر) بعد نزعه من سطح المكتب:
//
//     ١) الطور مُعلَن على **المعالِج** لا على الحدث؛ والافتراض `None`
//        يعني اشتراكًا صريحًا ⇒ جدٌّ صامت لا يستقبل حدث ابنه (صفر انحدار).
//     ٢) طور الهدف يُطلق **كلّ** معالِجات العقدة المطابقة — لا أوّلها فقط
//        (هذا ما كان يبعثر المفسّر عن المترجم).
//     ٣) الترتيب: التقاط من الجذر نزولًا ← الهدف ← فقاعات صعودًا للجذر.
//     ٤) `أوقف_الانتشار` يُفحص **بين كلّ معالِجَين** لا بين المستويات فقط.
//     ٥) `Both` يشارك في الطورين معًا.
// ======================================================================

#include "sad_test_framework.h"

#include "sad_ui/event_dispatch.h"
#include "sad_ui/ir.h"
#include "sad_ui/mouse_processor.h"
#include "sad_ui/types.h"

#include <memory>
#include <string>
#include <vector>

using namespace sad::ui;

namespace
{
    /// أسماء المعالِجات الوهميّة — لا نصوص حرفيّة متناثرة في التأكيدات.
    const std::string H_ROOT_CAP = "جذر_التقاط";
    const std::string H_ROOT_BUB = "جذر_فقاعة";
    const std::string H_MID_BOTH = "وسط_كلاهما";
    const std::string H_MID_NONE = "وسط_صامت";
    const std::string H_TARGET_A = "هدف_أ";
    const std::string H_TARGET_B = "هدف_ب";

    /// حدث مُعرَّف على عقدة بطور محدّد.
    IREvent makeEvent(IREventType type, const std::string &expr, EventPropagation prop)
    {
        IREvent e;
        e.type = type;
        e.expression = expr;
        e.propagation = prop;
        return e;
    }

    /// مشهد ثلاثيّ المستويات: جذر ← وسط ← هدف.
    struct Scene
    {
        std::shared_ptr<IRNode> root;
        std::shared_ptr<IRNode> mid;
        std::shared_ptr<IRNode> target;

        Scene()
            : root(IRNode::create(UINodeType::Container)),
              mid(IRNode::create(UINodeType::Container)),
              target(IRNode::create(UINodeType::Button))
        {
            root->addChild(mid);
            mid->addChild(target);
        }
    };

    /// يجمع أسماء المعالِجات بترتيب إطلاقها الفعليّ (بلا إيقاف انتشار).
    std::vector<std::string> run(const Scene &scene, const EventData &data)
    {
        std::vector<std::string> order;
        dispatchEvent(IREventType::OnTap, scene.target.get(), data,
                      [&order](IREventType, const std::string &expr,
                               const IRNode *, const EventData &)
                      { order.push_back(expr); });
        return order;
    }

    /// كسابقه، لكن كلّ معالِج يوقف الانتشار فور تنفيذه.
    std::vector<std::string> runStopping(const IRNode *target, const EventData &data)
    {
        std::vector<std::string> order;
        dispatchEvent(IREventType::OnTap, target, data,
                      [&order](IREventType, const std::string &expr,
                               const IRNode *, const EventData &ed)
                      {
                          order.push_back(expr);
                          ed.stopPropagation();
                      });
        return order;
    }
} // namespace

int main()
{
    SAD_TEST_INIT();

    SAD_GROUP("قلب إرسال الأحداث المشترك");

    // ─── ١) الافتراض None ⇒ لا انتشار (حارس صفر-انحدار) ───
    SAD_TEST("جدٌّ بلا طور مُعلَن لا يستقبل حدث حفيده",
             {
                 Scene s;
                 s.mid->addEvent(makeEvent(IREventType::OnTap, H_MID_NONE, EventPropagation::None));
                 s.root->addEvent(makeEvent(IREventType::OnTap, H_ROOT_BUB, EventPropagation::None));
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::None));

                 EventData d;
                 const auto order = run(s, d);
                 SAD_ASSERT_EQ(static_cast<size_t>(1), order.size());
                 SAD_ASSERT_EQ(H_TARGET_A, order[0]);
             });

    // ─── ٢) طور الهدف يُطلق كلّ المطابقات (توحيد المفسّر/المترجم) ───
    SAD_TEST("طور الهدف يُطلق كلّ معالِجات العقدة لا أوّلها",
             {
                 Scene s;
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::None));
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_B, EventPropagation::None));

                 EventData d;
                 const auto order = run(s, d);
                 SAD_ASSERT_EQ(static_cast<size_t>(2), order.size());
                 SAD_ASSERT_EQ(H_TARGET_A, order[0]);
                 SAD_ASSERT_EQ(H_TARGET_B, order[1]);
             });

    // ─── ٣) نوعٌ غير مطابق لا يُطلَق ───
    SAD_TEST("معالِج بنوع آخر لا يُطلَق",
             {
                 Scene s;
                 s.target->addEvent(makeEvent(IREventType::OnDrag, H_TARGET_A, EventPropagation::None));

                 EventData d;
                 SAD_ASSERT_TRUE(run(s, d).empty());
             });

    // ─── ٤) الترتيب الكامل: التقاط ← هدف ← فقاعات ───
    SAD_TEST("الترتيب: التقاط من الجذر ثمّ الهدف ثمّ فقاعات للجذر",
             {
                 Scene s;
                 s.root->addEvent(makeEvent(IREventType::OnTap, H_ROOT_CAP, EventPropagation::Capture));
                 s.root->addEvent(makeEvent(IREventType::OnTap, H_ROOT_BUB, EventPropagation::Bubble));
                 s.mid->addEvent(makeEvent(IREventType::OnTap, H_MID_BOTH, EventPropagation::Both));
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::None));

                 EventData d;
                 const auto order = run(s, d);
                 // التقاط: جذر ثمّ وسط(Both) — الهدف — فقاعات: وسط(Both) ثمّ جذر
                 SAD_ASSERT_EQ(static_cast<size_t>(5), order.size());
                 SAD_ASSERT_EQ(H_ROOT_CAP, order[0]);
                 SAD_ASSERT_EQ(H_MID_BOTH, order[1]);
                 SAD_ASSERT_EQ(H_TARGET_A, order[2]);
                 SAD_ASSERT_EQ(H_MID_BOTH, order[3]);
                 SAD_ASSERT_EQ(H_ROOT_BUB, order[4]);
             });

    // ─── ٥) إيقاف الانتشار عند الهدف يمنع الصعود ───
    SAD_TEST("إيقاف الانتشار عند الهدف يمنع طور الفقاعات",
             {
                 Scene s;
                 s.mid->addEvent(makeEvent(IREventType::OnTap, H_MID_BOTH, EventPropagation::Bubble));
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::None));

                 EventData d;
                 const auto order = runStopping(s.target.get(), d);
                 SAD_ASSERT_EQ(static_cast<size_t>(1), order.size());
                 SAD_ASSERT_EQ(H_TARGET_A, order[0]);
             });

    // ─── ٦) الإيقاف يُفحص بين معالِجَين على العقدة نفسها ───
    SAD_TEST("إيقاف الانتشار يمنع المعالِج التالي على العقدة نفسها",
             {
                 Scene s;
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::None));
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_B, EventPropagation::None));

                 EventData d;
                 const auto order = runStopping(s.target.get(), d);
                 SAD_ASSERT_EQ(static_cast<size_t>(1), order.size());
                 SAD_ASSERT_EQ(H_TARGET_A, order[0]);
             });

    // ─── ٧) الإيقاف في طور الالتقاط يمنع الهدف نفسه ───
    SAD_TEST("إيقاف الانتشار أثناء الالتقاط يمنع بلوغ الهدف",
             {
                 Scene s;
                 s.root->addEvent(makeEvent(IREventType::OnTap, H_ROOT_CAP, EventPropagation::Capture));
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::None));

                 EventData d;
                 const auto order = runStopping(s.target.get(), d);
                 SAD_ASSERT_EQ(static_cast<size_t>(1), order.size());
                 SAD_ASSERT_EQ(H_ROOT_CAP, order[0]);
             });

    // ─── ٨) عقدة يتيمة: الهدف وحده بلا أجداد ───
    SAD_TEST("عقدة بلا أب تُرسِل طور الهدف فقط بلا تعطّل",
             {
                 auto lone = IRNode::create(UINodeType::Button);
                 lone->addEvent(makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::Both));

                 EventData d;
                 std::vector<std::string> order;
                 dispatchEvent(IREventType::OnTap, lone.get(), d,
                               [&order](IREventType, const std::string &expr,
                                        const IRNode *, const EventData &)
                               { order.push_back(expr); });
                 SAD_ASSERT_EQ(static_cast<size_t>(1), order.size());
                 SAD_ASSERT_EQ(H_TARGET_A, order[0]);
             });

    // ─── ٩) هدف معدوم لا يتعطّل ───
    SAD_TEST("هدف معدوم لا يُطلق شيئًا ولا يتعطّل",
             {
                 EventData d;
                 int calls = 0;
                 dispatchEvent(IREventType::OnTap, nullptr, d,
                               [&calls](IREventType, const std::string &,
                                        const IRNode *, const EventData &)
                               { ++calls; });
                 SAD_ASSERT_EQ(0, calls);
             });

    // ─── ١٠) العقدة المُمرَّرة للمصرِف هي صاحبة المعالِج لا الهدف ───
    SAD_TEST("المصرِف يستقبل العقدة صاحبة المعالِج في طور الفقاعات",
             {
                 Scene s;
                 s.mid->addEvent(makeEvent(IREventType::OnTap, H_MID_BOTH, EventPropagation::Bubble));

                 EventData d;
                 const IRNode *seen = nullptr;
                 dispatchEvent(IREventType::OnTap, s.target.get(), d,
                               [&seen](IREventType, const std::string &,
                                       const IRNode *n, const EventData &)
                               { seen = n; });
                 SAD_ASSERT_TRUE(seen == s.mid.get());
             });

    // ─── ١١) لا إطلاق مضاعف: المُطلِق يُطلق مرّة والمُرسِل يختار ───
    // (AR) حارس N²: كانت المعالجات تمرّ على getEvents() ثمّ تنادي مدخلًا يمرّ
    //      عليها ثانيةً، فعقدةٌ بمعالِجَين من النوع نفسه تُطلق ٤ مرّات لا ٢،
    //      وكلّ جدٍّ مُتفرِّع يُطلق مرّتين. النقر هنا يمرّ بالمسار الحقيقيّ
    //      (MouseEventProcessor → fireEvent → dispatchEvent).
    SAD_TEST("نقرةٌ واحدة على عقدة بمعالِجَين تُطلق مرّتين لا أربعًا",
             {
                 Scene s;
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::None));
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_B, EventPropagation::None));
                 s.root->addEvent(makeEvent(IREventType::OnTap, H_ROOT_BUB, EventPropagation::Bubble));

                 std::vector<std::string> order;
                 MouseEventProcessor proc;
                 proc.setHitTestCallback(
                     [&s](float, float) -> const IRNode *
                     { return s.target.get(); });
                 proc.setGetTimeMsCallback([]() -> uint32_t
                                           { return 0; });
                 // (AR) نظير مدخل المنصّة: يُرسل عبر الأطوار ثمّ يُنفّذ المختار.
                 proc.setFireEventCallback(
                     [&order](IREventType t, const std::string &, const IRNode *n,
                              const EventData &d)
                     {
                         dispatchEvent(t, n, d,
                                       [&order](IREventType, const std::string &expr,
                                                const IRNode *, const EventData &)
                                       { order.push_back(expr); });
                     });

                 proc.onMouseDown(MouseButton::Left, 10.0f, 10.0f);

                 SAD_ASSERT_EQ(static_cast<size_t>(3), order.size());
                 SAD_ASSERT_EQ(H_TARGET_A, order[0]);
                 SAD_ASSERT_EQ(H_TARGET_B, order[1]);
                 SAD_ASSERT_EQ(H_ROOT_BUB, order[2]);
             });

    SAD_GROUP("طور الانتشار وبيانات المعالِج (م٢/م٥)");

    // ─── ١٢) SoT الطور: الأسماء العربيّة ↔ EventPropagation ───
    SAD_TEST("تحويل أسماء أطوار الانتشار من SoT",
             {
                 SAD_ASSERT_TRUE(stringToEventPropagation("فقاعة") == EventPropagation::Bubble);
                 SAD_ASSERT_TRUE(stringToEventPropagation("التقاط") == EventPropagation::Capture);
                 SAD_ASSERT_TRUE(stringToEventPropagation("كلاهما") == EventPropagation::Both);
                 SAD_ASSERT_TRUE(stringToEventPropagation("لا_انتشار") == EventPropagation::None);
                 // فشل-آمن: اسمٌ مجهول ⇒ لا انتشار
                 SAD_ASSERT_TRUE(stringToEventPropagation("غير_موجود") == EventPropagation::None);
             });

    // ─── ١٣) أوقف_الانتشار عبر النطاق النشط ───
    SAD_TEST("ActiveEventScope يمكّن stopActiveEventPropagation من إيقاف الانتشار",
             {
                 Scene s;
                 s.root->addEvent(makeEvent(IREventType::OnTap, H_ROOT_BUB, EventPropagation::Bubble));
                 s.target->addEvent(makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::None));

                 EventData d;
                 std::vector<std::string> order;
                 dispatchEvent(IREventType::OnTap, s.target.get(), d,
                               [&order](IREventType, const std::string &expr,
                                        const IRNode *, const EventData &ed)
                               {
                                   // (AR) نظير ما يفعله المفسّر حول كلّ معالِج
                                   ActiveEventScope scope(ed);
                                   order.push_back(expr);
                                   if (expr == H_TARGET_A)
                                       stopActiveEventPropagation(); // الهدف يوقف
                               });
                 // لم يبلغ الجدّ (فقاعة أُوقفت)
                 SAD_ASSERT_EQ(static_cast<size_t>(1), order.size());
                 SAD_ASSERT_EQ(H_TARGET_A, order[0]);
             });

    // ─── ١٤) بيانات المعالِج (customData) تصل لكلّ نداء ───
    SAD_TEST("بيانات كلّ معالِج تُملأ في EventData قبل ندائه",
             {
                 Scene s;
                 IREvent a = makeEvent(IREventType::OnTap, H_TARGET_A, EventPropagation::None);
                 a.userData = "بيانات_أ";
                 IREvent b = makeEvent(IREventType::OnTap, H_TARGET_B, EventPropagation::None);
                 b.userData = "بيانات_ب";
                 s.target->addEvent(a);
                 s.target->addEvent(b);

                 EventData d;
                 std::vector<std::string> seen;
                 dispatchEvent(IREventType::OnTap, s.target.get(), d,
                               [&seen](IREventType, const std::string &,
                                       const IRNode *, const EventData &ed)
                               { seen.push_back(ed.customData); });
                 SAD_ASSERT_EQ(static_cast<size_t>(2), seen.size());
                 SAD_ASSERT_EQ(std::string("بيانات_أ"), seen[0]);
                 SAD_ASSERT_EQ(std::string("بيانات_ب"), seen[1]);
             });

    // ─── ١٥) المعدّلات الجديدة تُنتج أحداثًا لا خصائص (حارس القائمة البيضاء) ───
    SAD_TEST("معدّلات الأحداث الجديدة كلّها تُصنَّف أحداثًا",
             {
                 // عيّنة من السبعة المُطلَقات المُضافة + قدامى
                 SAD_ASSERT_TRUE(isEventModifier(ModifierType::OnFocus));
                 SAD_ASSERT_TRUE(isEventModifier(ModifierType::OnBlur));
                 SAD_ASSERT_TRUE(isEventModifier(ModifierType::OnRelease));
                 SAD_ASSERT_TRUE(isEventModifier(ModifierType::OnSubmit));
                 SAD_ASSERT_TRUE(isEventModifier(ModifierType::OnLoad));
                 SAD_ASSERT_TRUE(isEventModifier(ModifierType::OnTap));
                 // وتحويلها صحيح
                 SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnFocus) ==
                                 IREventType::OnFocus);
                 SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnValueChange) ==
                                 IREventType::OnChange);
                 // ليس معدّل حدث
                 SAD_ASSERT_FALSE(isEventModifier(ModifierType::Width));
             });

    // ─── ١٦) اسم معدّل الحدث مُشتقّ من SoT الأحداث ───
    SAD_TEST("اسم معدّل الحدث يطابق اسم الحدث القانونيّ",
             {
                 SAD_ASSERT_EQ(irEventTypeToString(IREventType::OnFocus),
                               modifierTypeToArabicName(ModifierType::OnFocus));
             });

    SAD_SUMMARY();
}
