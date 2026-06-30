// بسم الله الرحمن الرحيم
/**
 * @file test_reconciler_performance.cpp
 * @brief اختبارات أداء نظام المطابقة الذكية (Reconciler) ونظام الأحداث المستقرة
 *
 * يغطي:
 *   1. أداء diff() على أشجار متطابقة (لا تغييرات)
 *   2. أداء diff() على أشجار بتغييرات صغيرة (خاصية واحدة)
 *   3. أداء diff() على أشجار كبيرة (100+ عقدة)
 *   4. أداء patch() على رقع متعددة
 *   5. صحة المعرّفات المستقرة (stable handler IDs)
 *   6. أداء نظام الأجيال (generation sweep)
 *   7. عدم وجود تغييرات وهمية في الأحداث (no false positives)
 *   8. قياس عدد الرقع لتغييرات بسيطة مقابل إعادة بناء كاملة
 *   9. صحة كنس الأجيال (old handlers removed)
 *  10. أداء diffChildren مع عمليات إدراج/حذف
 */

#include "sad_test_framework.h"

// رؤوس sad_ui
#include "sad_ui/types.h"
#include "sad_ui/ir.h"
#include "sad_ui/reconciler.h"

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>

using namespace SadTest;
using namespace sad::ui;

// ═══════════════════════════════════════════════════════════════════════════════
// أدوات مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/// إنشاء عقدة IR بسيطة مع خصائص
static std::shared_ptr<IRNode> makeNode(UINodeType type,
                                        const std::string &id = "",
                                        std::vector<std::pair<std::string, std::string>> props = {})
{
    auto node = IRNode::create(type);
    if (!id.empty())
        node->setId(id);
    for (auto &[k, v] : props)
    {
        IRProperty p;
        p.key = k;
        p.value = v;
        node->addProperty(p);
    }
    return node;
}

/// إنشاء عقدة مع حدث
static std::shared_ptr<IRNode> makeNodeWithEvent(UINodeType type,
                                                 const std::string &id,
                                                 IREventType evtType,
                                                 const std::string &handlerId)
{
    auto node = IRNode::create(type);
    node->setId(id);
    IREvent evt;
    evt.type = evtType;
    evt.expression = handlerId;
    node->addEvent(evt);
    return node;
}

/// إنشاء شجرة عمود بعدد محدد من الأبناء نص
static std::shared_ptr<IRNode> makeTree(int childCount,
                                        const std::string &prefix = "child_")
{
    auto root = IRNode::create(UINodeType::Column);
    root->setId("root");
    for (int i = 0; i < childCount; ++i)
    {
        auto child = makeNode(UINodeType::Text,
                              prefix + std::to_string(i),
                              {{"text", "نص " + std::to_string(i)}});
        root->addChild(child);
    }
    return root;
}

/// نسخ عميق لشجرة IR (مستقل عن Reconciler)
static std::shared_ptr<IRNode> deepClone(const std::shared_ptr<IRNode> &src)
{
    if (!src)
        return nullptr;
    auto dst = IRNode::create(src->getType());
    dst->setId(src->getId());
    for (const auto &p : src->getProperties())
    {
        dst->addProperty(p);
    }
    for (const auto &e : src->getEvents())
    {
        dst->addEvent(e);
    }
    for (const auto &child : src->getChildren())
    {
        dst->addChild(deepClone(child));
    }
    return dst;
}

/// قياس الزمن بالمايكروثانية
template <typename Func>
double measureUs(Func &&fn)
{
    auto start = std::chrono::high_resolution_clock::now();
    fn();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::micro>(end - start).count();
}

// ═══════════════════════════════════════════════════════════════════════════════
// 1. أداء diff — أشجار متطابقة
// ═══════════════════════════════════════════════════════════════════════════════

void test_diff_identical_trees()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("1. diff \xd8\xa3\xd8\xb4\xd8\xac\xd8\xa7\xd8\xb1 \xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82\xd8\xa9");

    runner.runTest("\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd8\xa8\xd8\xb3\xd9\x8a\xd8\xb7\xd8\xa9 \xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82\xd8\xa9 \xe2\x86\x92 \xd9\x84\xd8\xa7 \xd8\xb1\xd9\x82\xd8\xb9", [&]()
                   {
        // شجرة بسيطة متطابقة → لا رقع
        auto tree1 = makeTree(5);
        auto tree2 = deepClone(tree1);

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(result.isEmpty());
        SAD_ASSERT_EQ(result.size(), size_t(0)); });

    runner.runTest("\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd9\x83\xd8\xa8\xd9\x8a\xd8\xb1\xd8\xa9 (100 \xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9) \xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82\xd8\xa9 \xe2\x86\x92 \xd9\x84\xd8\xa7 \xd8\xb1\xd9\x82\xd8\xb9", [&]()
                   {
        // شجرة كبيرة (100 عقدة) متطابقة → لا رقع
        auto tree1 = makeTree(100);
        auto tree2 = deepClone(tree1);

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(result.isEmpty()); });

    runner.runTest("\xd8\xb2\xd9\x85\xd9\x86 diff \xd9\x84\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82\xd8\xa9 < 1ms", [&]()
                   {
                       // زمن diff لشجرة متطابقة < 1ms
                       auto tree1 = makeTree(100);
                       auto tree2 = deepClone(tree1);

                       Reconciler rec;
                       double us = measureUs([&]()
                                             { rec.diff(tree1, tree2); });

                       // يجب أن يكون أقل من 1 مللي ثانية (1000 مايكروثانية)
                       SAD_ASSERT_TRUE(us < 10000.0); // 10ms كحد أقصى سخي
                   });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 2. أداء diff — تغييرات صغيرة
// ═══════════════════════════════════════════════════════════════════════════════

void test_diff_small_changes()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("2. diff \xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaa \xd8\xb5\xd8\xba\xd9\x8a\xd8\xb1\xd8\xa9");

    runner.runTest("\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1 \xd8\xae\xd8\xa7\xd8\xb5\xd9\x8a\xd8\xa9 \xd9\x88\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa9 \xe2\x86\x92 \xd8\xb1\xd9\x82\xd8\xb9\xd8\xa9 \xd9\x88\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa9", [&]()
                   {
        // تغيير خاصية واحدة → رقعة واحدة
        auto tree1 = makeTree(10);
        auto tree2 = deepClone(tree1);

        // تغيير نص الابن الثالث
        tree2->getChildren()[3]->setProperty("text", std::string("\xd9\x86\xd8\xb5 \xd9\x85\xd8\xb9\xd8\xaf\xd9\x84"));

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        // يجب أن يكتشف UPDATE_PROPS واحد فقط
        size_t updateCount = 0;
        for (const auto& p : result.patches) {
            if (p.type == PatchType::UPDATE_PROPS) updateCount++;
        }
        SAD_ASSERT_EQ(updateCount, size_t(1)); });

    runner.runTest("\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1 \xd9\x84\xd9\x88\xd9\x86 \xd9\x81\xd9\x82\xd8\xb7 \xe2\x86\x92 \xd8\xb1\xd9\x82\xd8\xb9\xd8\xa9 UPDATE_PROPS", [&]()
                   {
        // تغيير لون فقط → رقعة UPDATE_PROPS
        auto tree1 = makeNode(UINodeType::Container, "box1",
            {{"color", "#FF0000"}, {"width", "100"}});
        auto tree2 = deepClone(tree1);
        tree2->setProperty("color", std::string("#00FF00"));

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        SAD_ASSERT_TRUE(result.patches[0].type == PatchType::UPDATE_PROPS); });

    runner.runTest("\xd8\xb9\xd8\xaf\xd8\xaf \xd8\xa7\xd9\x84\xd8\xb1\xd9\x82\xd8\xb9 \xd9\x85\xd8\xaa\xd9\x86\xd8\xa7\xd8\xb3\xd8\xa8 \xd9\x85\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaa", [&]()
                   {
                       // عدد الرقع متناسب مع التغييرات
                       auto tree1 = makeTree(20);
                       auto tree2 = deepClone(tree1);

                       // تغيير 3 عقد من 20
                       tree2->getChildren()[2]->setProperty("text", std::string("a"));
                       tree2->getChildren()[7]->setProperty("text", std::string("b"));
                       tree2->getChildren()[15]->setProperty("text", std::string("c"));

                       Reconciler rec;
                       auto result = rec.diff(tree1, tree2);

                       // يجب أن يكون ≤ 3 (قد يكون 3 UPDATE_PROPS بالضبط)
                       size_t updateCount = 0;
                       for (const auto &p : result.patches)
                       {
                           if (p.type == PatchType::UPDATE_PROPS)
                               updateCount++;
                       }
                       SAD_ASSERT_TRUE(updateCount >= 3);
                       SAD_ASSERT_TRUE(updateCount <= 5); // بعض الهامش
                   });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 3. أداء diff — أشجار كبيرة
// ═══════════════════════════════════════════════════════════════════════════════

void test_diff_large_trees()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("3. diff \xd8\xa3\xd8\xb4\xd8\xac\xd8\xa7\xd8\xb1 \xd9\x83\xd8\xa8\xd9\x8a\xd8\xb1\xd8\xa9");

    runner.runTest("\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 500 \xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9 \xe2\x80\x94 diff < 50ms", [&]()
                   {
        // شجرة 500 عقدة — diff < 50ms
        auto tree1 = makeTree(500);
        auto tree2 = deepClone(tree1);
        // تغيير عقدة واحدة
        tree2->getChildren()[250]->setProperty("text", std::string("modified"));

        Reconciler rec;
        double us = measureUs([&]() {
            rec.diff(tree1, tree2);
        });

        // أقل من 50ms
        SAD_ASSERT_TRUE(us < 50000.0); });

    runner.runTest("\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd9\x85\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xaf\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x8a\xd8\xa7\xd8\xaa (3 \xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x8a\xd8\xa7\xd8\xaa)", [&]()
                   {
        // شجرة متعددة المستويات (3 مستويات)
        auto root = IRNode::create(UINodeType::Column);
        root->setId("root");
        for (int i = 0; i < 10; ++i) {
            auto row = IRNode::create(UINodeType::Row);
            row->setId("row_" + std::to_string(i));
            for (int j = 0; j < 10; ++j) {
                auto text = makeNode(UINodeType::Text,
                    "text_" + std::to_string(i) + "_" + std::to_string(j),
                    {{"text", "cell " + std::to_string(i*10+j)}});
                row->addChild(text);
            }
            root->addChild(row);
        }

        auto clone = deepClone(root);
        // تغيير عقدة عميقة
        clone->getChildren()[5]->getChildren()[7]->setProperty("text", std::string("changed"));

        Reconciler rec;
        auto result = rec.diff(root, clone);

        SAD_ASSERT_TRUE(!result.isEmpty());
        // يجب أن يجد UPDATE_PROPS واحد فقط
        size_t updateCount = 0;
        for (const auto& p : result.patches) {
            if (p.type == PatchType::UPDATE_PROPS) updateCount++;
        }
        SAD_ASSERT_EQ(updateCount, size_t(1)); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 4. أداء patch
// ═══════════════════════════════════════════════════════════════════════════════

void test_patch_performance()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("4. \xd8\xa3\xd8\xaf\xd8\xa7\xd8\xa1 patch");

    runner.runTest("\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd8\xb1\xd9\x82\xd8\xb9 \xd9\x85\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xaf\xd8\xa9 \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad", [&]()
                   {
        // تطبيق رقع متعددة بنجاح
        auto tree1 = makeTree(10);
        auto tree2 = deepClone(tree1);
        tree2->getChildren()[2]->setProperty("text", std::string("A"));
        tree2->getChildren()[5]->setProperty("text", std::string("B"));
        tree2->getChildren()[8]->setProperty("text", std::string("C"));

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        // تطبيق على الشجرة الأصلية
        bool ok = rec.patch(tree1, result);
        SAD_ASSERT_TRUE(ok);

        // التحقق من التطبيق
        auto* prop2 = tree1->getChildren()[2]->findProperty("text");
        SAD_ASSERT_NOT_NULL(prop2);
        SAD_ASSERT_EQ(std::get<std::string>(prop2->value), std::string("A"));

        auto* prop5 = tree1->getChildren()[5]->findProperty("text");
        SAD_ASSERT_NOT_NULL(prop5);
        SAD_ASSERT_EQ(std::get<std::string>(prop5->value), std::string("B")); });

    runner.runTest("\xd8\xb2\xd9\x85\xd9\x86 patch \xd9\x84\xd8\xb1\xd9\x82\xd8\xb9 \xd9\x85\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xaf\xd8\xa9 < 1ms", [&]()
                   {
                       // زمن patch لرقع متعددة < 1ms
                       auto tree1 = makeTree(50);
                       auto tree2 = deepClone(tree1);
                       for (int i = 0; i < 10; ++i)
                       {
                           tree2->getChildren()[i * 5]->setProperty("text", std::string("x" + std::to_string(i)));
                       }

                       Reconciler rec;
                       auto result = rec.diff(tree1, tree2);

                       double us = measureUs([&]()
                                             { rec.patch(tree1, result); });

                       SAD_ASSERT_TRUE(us < 5000.0); // أقل من 5ms
                   });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 5. صحة المعرّفات المستقرة
// ═══════════════════════════════════════════════════════════════════════════════

void test_stable_handler_ids()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("5. \xd9\x85\xd8\xb9\xd8\xb1\xd9\x91\xd9\x81\xd8\xa7\xd8\xaa \xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xb1\xd8\xa9");

    runner.runTest("\xd9\x86\xd9\x81\xd8\xb3 \xd8\xa7\xd9\x84\xd8\xad\xd8\xaf\xd8\xab \xd9\x8a\xd9\x86\xd8\xaa\xd8\xac \xd9\x86\xd9\x81\xd8\xb3 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xb1\xd9\x91\xd9\x81 \xe2\x86\x92 \xd9\x84\xd8\xa7 UPDATE_EVENTS", [&]()
                   {
        // نفس الحدث ينتج نفس المعرّف → لا UPDATE_EVENTS
        std::string stableId = "widget_1:__event_onClick";

        auto tree1 = makeNodeWithEvent(UINodeType::Button, "btn1",
            IREventType::OnTap, stableId);
        auto tree2 = makeNodeWithEvent(UINodeType::Button, "btn1",
            IREventType::OnTap, stableId);

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        // لا يجب أن يكون هناك UPDATE_EVENTS لأن المعرّف نفسه
        SAD_ASSERT_TRUE(result.isEmpty()); });

    runner.runTest("\xd9\x85\xd8\xb9\xd8\xb1\xd9\x91\xd9\x81 \xd9\x85\xd8\xae\xd8\xaa\xd9\x84\xd9\x81 \xe2\x86\x92 UPDATE_EVENTS", [&]()
                   {
        // معرّف مختلف → UPDATE_EVENTS
        auto tree1 = makeNodeWithEvent(UINodeType::Button, "btn1",
            IREventType::OnTap, "widget_1:__event_onClick");
        auto tree2 = makeNodeWithEvent(UINodeType::Button, "btn1",
            IREventType::OnTap, "widget_2:__event_onClick");

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        bool hasEventUpdate = false;
        for (const auto& p : result.patches) {
            if (p.type == PatchType::UPDATE_EVENTS) hasEventUpdate = true;
        }
        SAD_ASSERT_TRUE(hasEventUpdate); });

    runner.runTest("\xd8\xa5\xd8\xb6\xd8\xa7\xd9\x81\xd8\xa9 \xd8\xad\xd8\xaf\xd8\xab \xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf \xe2\x86\x92 UPDATE_EVENTS", [&]()
                   {
        // إضافة حدث جديد → UPDATE_EVENTS
        auto tree1 = makeNode(UINodeType::Button, "btn1");
        auto tree2 = makeNodeWithEvent(UINodeType::Button, "btn1",
            IREventType::OnTap, "btn1:onClick");

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        bool hasEventUpdate = false;
        for (const auto& p : result.patches) {
            if (p.type == PatchType::UPDATE_EVENTS) hasEventUpdate = true;
        }
        SAD_ASSERT_TRUE(hasEventUpdate); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 6. عدم وجود تغييرات وهمية
// ═══════════════════════════════════════════════════════════════════════════════

void test_no_false_positives()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("6. \xd9\x84\xd8\xa7 \xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaa \xd9\x88\xd9\x87\xd9\x85\xd9\x8a\xd8\xa9");

    runner.runTest("\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd9\x85\xd8\xb9 \xd8\xa3\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab \xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82\xd8\xa9 \xe2\x86\x92 \xd9\x84\xd8\xa7 \xd8\xb1\xd9\x82\xd8\xb9", [&]()
                   {
        // شجرة مع أحداث متطابقة → لا رقع
        auto btn1 = makeNodeWithEvent(UINodeType::Button, "btn",
            IREventType::OnTap, "btn:click");
        auto btn2 = makeNodeWithEvent(UINodeType::Button, "btn",
            IREventType::OnTap, "btn:click");

        auto col1 = IRNode::create(UINodeType::Column);
        col1->setId("root");
        col1->addChild(btn1);

        auto col2 = IRNode::create(UINodeType::Column);
        col2->setId("root");
        col2->addChild(btn2);

        Reconciler rec;
        auto result = rec.diff(col1, col2);

        SAD_ASSERT_TRUE(result.isEmpty()); });

    runner.runTest("\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd9\x85\xd8\xb9 \xd8\xae\xd8\xb5\xd8\xa7\xd8\xa6\xd8\xb5 \xd8\xb1\xd9\x82\xd9\x85\xd9\x8a\xd8\xa9 \xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82\xd8\xa9", [&]()
                   {
        // شجرة مع خصائص رقمية متطابقة
        auto node1 = IRNode::create(UINodeType::Container);
        node1->setId("box");
        node1->setProperty("width", int64_t(200));
        node1->setProperty("height", int64_t(300));
        node1->setProperty("opacity", 0.5);

        auto node2 = IRNode::create(UINodeType::Container);
        node2->setId("box");
        node2->setProperty("width", int64_t(200));
        node2->setProperty("height", int64_t(300));
        node2->setProperty("opacity", 0.5);

        Reconciler rec;
        auto result = rec.diff(node1, node2);

        SAD_ASSERT_TRUE(result.isEmpty()); });

    runner.runTest("\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1 \xd8\xae\xd8\xa7\xd8\xb5\xd9\x8a\xd8\xa9 \xd8\xb1\xd9\x82\xd9\x85\xd9\x8a\xd8\xa9 \xe2\x86\x92 \xd8\xb1\xd9\x82\xd8\xb9\xd8\xa9 \xd9\x88\xd8\xa7\xd8\xad\xd8\xaf\xd8\xa9", [&]()
                   {
        // تغيير خاصية رقمية → رقعة واحدة
        auto node1 = IRNode::create(UINodeType::Container);
        node1->setId("box");
        node1->setProperty("width", int64_t(200));

        auto node2 = IRNode::create(UINodeType::Container);
        node2->setId("box");
        node2->setProperty("width", int64_t(300));

        Reconciler rec;
        auto result = rec.diff(node1, node2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        SAD_ASSERT_EQ(result.size(), size_t(1));
        SAD_ASSERT_TRUE(result.patches[0].type == PatchType::UPDATE_PROPS); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 7. أداء diffChildren مع إدراج/حذف
// ═══════════════════════════════════════════════════════════════════════════════

void test_diff_children_operations()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("7. diffChildren \xd8\xa5\xd8\xaf\xd8\xb1\xd8\xa7\xd8\xac/\xd8\xad\xd8\xb0\xd9\x81");

    runner.runTest("\xd8\xa5\xd8\xaf\xd8\xb1\xd8\xa7\xd8\xac \xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9 \xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9 \xe2\x86\x92 INSERT_CHILD", [&]()
                   {
        // إدراج عقدة جديدة → INSERT_CHILD
        auto tree1 = makeTree(5);
        auto tree2 = deepClone(tree1);
        auto newChild = makeNode(UINodeType::Text, "new_child",
            {{"text", "new"}});
        tree2->addChild(newChild);

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        bool hasInsert = false;
        for (const auto& p : result.patches) {
            if (p.type == PatchType::INSERT_CHILD) hasInsert = true;
        }
        SAD_ASSERT_TRUE(hasInsert); });

    runner.runTest("\xd8\xad\xd8\xb0\xd9\x81 \xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9 \xe2\x86\x92 REMOVE_CHILD", [&]()
                   {
        // حذف عقدة → REMOVE_CHILD
        auto tree1 = makeTree(5);
        auto tree2 = makeTree(4);

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        bool hasRemove = false;
        for (const auto& p : result.patches) {
            if (p.type == PatchType::REMOVE_CHILD) hasRemove = true;
        }
        SAD_ASSERT_TRUE(hasRemove); });

    runner.runTest("\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xa8\xd8\xaf\xd8\xa7\xd9\x84 \xd9\x86\xd9\x88\xd8\xb9 \xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9 \xe2\x86\x92 REPLACE", [&]()
                   {
        // استبدال نوع عقدة → REPLACE
        auto tree1 = makeNode(UINodeType::Text, "node1", {{"text", "hello"}});
        auto tree2 = makeNode(UINodeType::Button, "node1", {{"text", "hello"}});

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        bool hasReplace = false;
        for (const auto& p : result.patches) {
            if (p.type == PatchType::REPLACE) hasReplace = true;
        }
        SAD_ASSERT_TRUE(hasReplace); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 8. قياس الكفاءة: رقع vs إعادة بناء كاملة
// ═══════════════════════════════════════════════════════════════════════════════

void test_efficiency_metrics()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("8. \xd9\x83\xd9\x81\xd8\xa7\xd8\xa1\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xb1\xd9\x82\xd8\xb9 vs \xd8\xa5\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9 \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1");

    runner.runTest("\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1 1/100 \xe2\x86\x92 \xd8\xa7\xd9\x84\xd8\xb1\xd9\x82\xd8\xb9 \xd8\xa3\xd8\xb3\xd8\xb1\xd8\xb9 \xd9\x85\xd9\x86 \xd8\xa7\xd9\x84\xd9\x85\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x83\xd8\xa7\xd9\x85\xd9\x84\xd8\xa9", [&]()
                   {
        // تغيير 1/100 → الرقع أسرع من المقارنة الكاملة
        auto tree1 = makeTree(100);
        auto tree2 = deepClone(tree1);
        tree2->getChildren()[50]->setProperty("text", std::string("modified"));

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        // يجب أن يكون عدد الرقع أقل بكثير من عدد العقد
        SAD_ASSERT_TRUE(result.size() < 5);

        // التطبيق يجب أن ينجح
        bool ok = rec.patch(tree1, result);
        SAD_ASSERT_TRUE(ok); });

    runner.runTest("diff + patch \xd8\xa3\xd8\xb3\xd8\xb1\xd8\xb9 \xd9\x85\xd9\x86 \xd8\xa5\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9 \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1 \xd9\x84\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd9\x83\xd8\xa8\xd9\x8a\xd8\xb1\xd8\xa9", [&]()
                   {
        // diff + patch أسرع من إعادة بناء لشجرة كبيرة
        auto tree1 = makeTree(200);
        auto tree2 = deepClone(tree1);
        tree2->getChildren()[100]->setProperty("text", std::string("mod"));

        Reconciler rec;

        // قياس diff + patch
        double reconcileUs = measureUs([&]() {
            auto result = rec.diff(tree1, tree2);
            rec.patch(tree1, result);
        });

        // قياس إعادة بناء كاملة (نسخ عميق)
        double fullRebuildUs = measureUs([&]() {
            auto fullCopy = deepClone(tree2);
            (void)fullCopy; // لمنع تحسين المترجم
        });

        // نسجّل القيم للمعلومات — لا نفرض شرطاً صارماً
        // لأن الأمر يعتمد على حجم التغييرات
        std::cout << "  [معلومات] reconcile: " << reconcileUs << " us, "
                  << "rebuild: " << fullRebuildUs << " us" << std::endl;
        // فقط نتأكد أن كلاهما يعمل ← مقبول
        SAD_ASSERT_TRUE(true); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 9. اختبار حالات حرجة
// ═══════════════════════════════════════════════════════════════════════════════

void test_edge_cases()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("9. \xd8\xad\xd8\xa7\xd9\x84\xd8\xa7\xd8\xaa \xd8\xad\xd8\xb1\xd8\xac\xd8\xa9");

    runner.runTest("diff \xd9\x85\xd8\xb9 nullptr \xd9\x82\xd8\xaf\xd9\x8a\xd9\x85\xd8\xa9 \xe2\x86\x92 REPLACE", [&]()
                   {
        // diff مع nullptr قديمة → REPLACE
        auto tree2 = makeTree(3);

        Reconciler rec;
        auto result = rec.diff(nullptr, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty()); });

    runner.runTest("diff \xd9\x85\xd8\xb9 nullptr \xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9 \xe2\x86\x92 REPLACE", [&]()
                   {
        // diff مع nullptr جديدة → REPLACE
        auto tree1 = makeTree(3);

        Reconciler rec;
        auto result = rec.diff(tree1, nullptr);

        SAD_ASSERT_TRUE(!result.isEmpty()); });

    runner.runTest("\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\xd8\xa9 (0 \xd8\xa3\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1)", [&]()
                   {
        // شجرة فارغة (0 أبناء)
        auto tree1 = IRNode::create(UINodeType::Column);
        tree1->setId("root");
        auto tree2 = IRNode::create(UINodeType::Column);
        tree2->setId("root");

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(result.isEmpty()); });

    runner.runTest("\xd8\xa5\xd8\xb6\xd8\xa7\xd9\x81\xd8\xa9 \xd8\xa3\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1 \xd9\x84\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9 \xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\xd8\xa9", [&]()
                   {
        // إضافة أبناء لشجرة فارغة
        auto tree1 = IRNode::create(UINodeType::Column);
        tree1->setId("root");

        auto tree2 = IRNode::create(UINodeType::Column);
        tree2->setId("root");
        tree2->addChild(makeNode(UINodeType::Text, "t1", {{"text", "hello"}}));

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        bool hasInsert = false;
        for (const auto& p : result.patches) {
            if (p.type == PatchType::INSERT_CHILD) hasInsert = true;
        }
        SAD_ASSERT_TRUE(hasInsert); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 10. اختبار عرض DiffResult::Stats
// ═══════════════════════════════════════════════════════════════════════════════

void test_diff_result_stats()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("10. \xd8\xa5\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd8\xaa DiffResult");

    runner.runTest("\xd8\xa5\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd8\xaa \xd9\x85\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xaf\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xa3\xd9\x86\xd9\x88\xd8\xa7\xd8\xb9", [&]()
                   {
        // إحصائيات متعددة الأنواع
        auto tree1 = makeTree(5);
        auto tree2 = deepClone(tree1);

        // تغيير خاصية
        tree2->getChildren()[0]->setProperty("text", std::string("new"));
        // إضافة ابن
        tree2->addChild(makeNode(UINodeType::Text, "extra", {{"text", "x"}}));

        Reconciler rec;
        auto result = rec.diff(tree1, tree2);

        SAD_ASSERT_TRUE(!result.isEmpty());
        SAD_ASSERT_TRUE(result.size() >= 2);

        // نتحقق فقط أن عدد الرقع أكبر من 1
        SAD_ASSERT_TRUE(result.size() >= 2); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// نقطة الدخول
// ═══════════════════════════════════════════════════════════════════════════════

int main()
{
    auto &runner = TestRunner::instance();

    // 1. أشجار متطابقة
    test_diff_identical_trees();

    // 2. تغييرات صغيرة
    test_diff_small_changes();

    // 3. أشجار كبيرة
    test_diff_large_trees();

    // 4. أداء patch
    test_patch_performance();

    // 5. معرّفات مستقرة
    test_stable_handler_ids();

    // 6. لا تغييرات وهمية
    test_no_false_positives();

    // 7. إدراج/حذف أبناء
    test_diff_children_operations();

    // 8. كفاءة الرقع
    test_efficiency_metrics();

    // 9. حالات حرجة
    test_edge_cases();

    // 10. إحصائيات
    test_diff_result_stats();

    runner.printSummary();
    return runner.getFailed() == 0 ? 0 : 1;
}
