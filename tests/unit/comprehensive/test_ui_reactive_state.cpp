// بسم الله الرحمن الرحيم
// ======================================================================
// test_ui_reactive_state.cpp — اختبارات قبول STORY-UI-W15-03
// ======================================================================
// الوصف بالعربية:
//   يختبر هذا الملف السلسلة التفاعلية لـ@حالة في UIStateManager:
//   تسجيل المكوّن → مراقبة setField → تصفية حقول @حالة → جدولة إعادة البناء
//   → دمج التغييرات (batching) → استدعاء callback عبر flush().
//
//   يُغطّي معايير القبول الخمسة:
//     - AC-1: التسجيل يحفظ حقول @حالة فقط (componentCount/isRegistered).
//     - AC-2/AC-5: تعيين حقل @حالة → flush → استدعاء rebuild callback.
//     - AC-3: عدة تعيينات متتالية → إعادة بناء واحدة (batching).
//     - AC-4: تعيين حقل عادي (غير @حالة) لا يُطلق إعادة بناء.
//     - (إضافي): تعيين بنفس القيمة لا يُطلق إعادة بناء؛ إلغاء التسجيل يفصل المراقب.
//
// English Description:
//   Tests the @حالة reactive chain in UIStateManager directly:
//   registerComponent → setField observer → state-field filtering →
//   scheduleRebuild → batching → rebuild callback via flush().
// ======================================================================

#include "sad_test_framework.h"
#include "ui_state_manager.h"
#include "object_instance.h"
#include "value.h"

#include <unordered_set>
#include <string>

using namespace SadTest;
using Sad::Interpreter::UIStateManager;
using Sad::Data::ObjectInstance;
using Sad::Data::Value;

// =====================================================================
// (AR) مساعد: إعادة تهيئة المدير وربط عدّاد إعادة بناء جديد
// (EN) Helper: reset manager and hook a fresh rebuild counter
// =====================================================================
namespace
{
    // (AR) عدّاد عام لاستدعاءات إعادة البناء — يُصفَّر في كل اختبار
    // (EN) Global rebuild-invocation counter — reset per test
    int g_rebuildCount = 0;

    void resetManagerWithCounter()
    {
        auto &mgr = UIStateManager::instance();
        mgr.reset();
        g_rebuildCount = 0;
        mgr.setRebuildCallback([]() { ++g_rebuildCount; });
    }
}

int main()
{
    auto &runner = TestRunner::instance();

    runner.beginGroup("W15-03.Reactivity");

    // ─────────────────────────────────────────────────────────────────
    // AC-1: التسجيل يحفظ المكوّن وحقول @حالة فقط
    // ─────────────────────────────────────────────────────────────────
    runner.runTest("AC-1: registerComponent records component + state fields", [&]()
    {
        resetManagerWithCounter();
        auto &mgr = UIStateManager::instance();

        // (AR) classType=nullptr → setField يعامل الحقول كديناميكية ويُطلق المراقب
        // (EN) classType=nullptr → setField treats fields as dynamic and fires observer
        ObjectInstance obj(nullptr, 1);
        std::unordered_set<std::string> stateFields = {"عدد"};
        mgr.registerComponent(&obj, "عداد", stateFields);

        SAD_ASSERT_EQ(mgr.componentCount(), static_cast<size_t>(1));
        SAD_ASSERT_TRUE(mgr.isRegistered(&obj));

        mgr.unregisterComponent(&obj);
        SAD_ASSERT_EQ(mgr.componentCount(), static_cast<size_t>(0));
        SAD_ASSERT_FALSE(mgr.isRegistered(&obj));
    });

    // ─────────────────────────────────────────────────────────────────
    // AC-2/AC-5: تعيين حقل @حالة → flush يستدعي rebuild callback
    // ─────────────────────────────────────────────────────────────────
    runner.runTest("AC-2/AC-5: state field change schedules rebuild + flush fires callback", [&]()
    {
        resetManagerWithCounter();
        auto &mgr = UIStateManager::instance();

        ObjectInstance obj(nullptr, 1);
        mgr.registerComponent(&obj, "عداد", {"عدد"});

        SAD_ASSERT_FALSE(mgr.hasPendingChanges());
        obj.setField("عدد", Value(5));         // (AR) تغيير حقل @حالة
        SAD_ASSERT_TRUE(mgr.hasPendingChanges());

        mgr.flush();
        SAD_ASSERT_EQ(g_rebuildCount, 1);
        SAD_ASSERT_FALSE(mgr.hasPendingChanges());

        mgr.unregisterComponent(&obj);
    });

    // ─────────────────────────────────────────────────────────────────
    // AC-4: تعيين حقل عادي (غير @حالة) لا يُطلق إعادة بناء
    // ─────────────────────────────────────────────────────────────────
    runner.runTest("AC-4: non-state field change does NOT trigger rebuild", [&]()
    {
        resetManagerWithCounter();
        auto &mgr = UIStateManager::instance();

        ObjectInstance obj(nullptr, 1);
        mgr.registerComponent(&obj, "عداد", {"عدد"});

        obj.setField("عنوان", Value("مرحبا"));  // (AR) حقل عادي غير مُعلَن كـ@حالة
        SAD_ASSERT_FALSE(mgr.hasPendingChanges());

        mgr.flush();
        SAD_ASSERT_EQ(g_rebuildCount, 0);

        mgr.unregisterComponent(&obj);
    });

    // ─────────────────────────────────────────────────────────────────
    // AC-3: عدة تعيينات متتالية تُدمج في إعادة بناء واحدة (batching)
    // ─────────────────────────────────────────────────────────────────
    runner.runTest("AC-3: three state changes batch into one rebuild", [&]()
    {
        resetManagerWithCounter();
        auto &mgr = UIStateManager::instance();

        ObjectInstance obj(nullptr, 1);
        mgr.registerComponent(&obj, "عداد", {"عدد"});

        obj.setField("عدد", Value(1));
        obj.setField("عدد", Value(2));
        obj.setField("عدد", Value(3));
        // (AR) flush واحد بعد ثلاثة تغييرات → استدعاء واحد
        // (EN) single flush after three changes → one invocation
        mgr.flush();
        SAD_ASSERT_EQ(g_rebuildCount, 1);

        mgr.unregisterComponent(&obj);
    });

    // ─────────────────────────────────────────────────────────────────
    // إضافي: تعيين بنفس القيمة لا يُطلق إعادة بناء
    // ─────────────────────────────────────────────────────────────────
    runner.runTest("Extra: no-op assignment (same value) does not trigger rebuild", [&]()
    {
        resetManagerWithCounter();
        auto &mgr = UIStateManager::instance();

        ObjectInstance obj(nullptr, 1);
        mgr.registerComponent(&obj, "عداد", {"عدد"});

        obj.setField("عدد", Value(7));  // (AR) تغيير حقيقي
        mgr.flush();
        SAD_ASSERT_EQ(g_rebuildCount, 1);

        obj.setField("عدد", Value(7));  // (AR) نفس القيمة → لا تغيير
        SAD_ASSERT_FALSE(mgr.hasPendingChanges());
        mgr.flush();
        SAD_ASSERT_EQ(g_rebuildCount, 1);  // (AR) لم يزِد

        mgr.unregisterComponent(&obj);
    });

    // ─────────────────────────────────────────────────────────────────
    // إضافي: إلغاء التسجيل يفصل المراقب فلا تُطلق تغييرات لاحقة
    // ─────────────────────────────────────────────────────────────────
    runner.runTest("Extra: unregister detaches observer (later changes inert)", [&]()
    {
        resetManagerWithCounter();
        auto &mgr = UIStateManager::instance();

        ObjectInstance obj(nullptr, 1);
        mgr.registerComponent(&obj, "عداد", {"عدد"});
        mgr.unregisterComponent(&obj);

        obj.setField("عدد", Value(99));  // (AR) بعد الفصل
        SAD_ASSERT_FALSE(mgr.hasPendingChanges());
        mgr.flush();
        SAD_ASSERT_EQ(g_rebuildCount, 0);
    });

    // (AR) تنظيف نهائي حتى لا يبقى مؤشر معلّق في المفرد
    // (EN) Final cleanup so no dangling pointer remains in the singleton
    UIStateManager::instance().reset();

    return runner.printSummary();
}
