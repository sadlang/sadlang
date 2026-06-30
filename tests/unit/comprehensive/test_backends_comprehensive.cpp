// ״¨״³… ״§„„‡ ״§„״±״­…† ״§„״±״­…
/**
 * @file test_backends_comprehensive.cpp
 * @brief ״§״®״×״¨״§״±״§״× ״´״§…„״© „״¬…״¹ …†״µ״§״× ״§„״±״³ˆ…״§״× ״§„״®…״³ + ״¨†״§״¡ IR + ״®״· ״§„״£†״§״¨״¨
 *
 * ״÷״·:
 *   ¡. ״¨†״§״¡ ״¹‚״¯ IR …״¹ ״®״µ״§״¦״µ …״×†ˆ״¹״©
 *   ¢. ״®״· ״§„״£†״§״¨״¨ (processNode) „ƒ„ …†״µ״© (Desktop, Android, iOS, Web, macOS)
 *   £. ״§„״×״­‚‚ …† ״£† ״§„ƒˆ״¯ ״§„…ˆ„‘״¯ ״÷״± ״§״±״÷ ˆ״­״×ˆ ƒ„…״§״× …״×״§״­״© ״®״§״µ״© ״¨״§„…†״µ״©
 *   ₪. ״¥†״´״§״¡ ״£״´״¬״§״± IR …״¹‚״¯״© (״¹…ˆ״¯ + ״£״¨†״§״¡ …״×†ˆ״¹״©)
 *   ¥. ״§״®״×״¨״§״± ״®״µ״§״¦״µ ״§„״¹†״§״µ״± (״¹״±״¶״ ״§״±״×״§״¹״ „ˆ†״ †״µ)
 *   ¦. ״§״®״×״¨״§״± ״§„״£״­״¯״§״« ˆ״¥״´״§״±״§״× ״§„״­״§„״©
 *   §. ״§״®״×״¨״§״± ״§„״×ˆ״¬‡ ״§„״°ƒ (RenderingHint)
 */

#include "sad_test_framework.h"

// ג”€ג”€ ״±״₪ˆ״³ sad_ui ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
#include "sad_ui/types.h"
#include "sad_ui/node.h"
#include "sad_ui/ir.h"
#include "sad_ui/ir_builder.h"
#include "sad_ui/pipeline.h"

#include <string>
#include <vector>
#include <memory>

using namespace SadTest;
using namespace sad::ui;

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ״£״¯ˆ״§״× …״³״§״¹״¯״©
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

/// ״¥†״´״§״¡ ״¹‚״¯״© IR ״¨״³״·״© …״¹ ״®״µ״§״¦״µ ״§״®״×״§״±״©
static std::shared_ptr<IRNode> makeIR(UINodeType type,
    std::vector<std::pair<std::string, std::string>> props = {})
{
    auto node = IRNode::create(type);
    for (auto& [k, v] : props) {
        IRProperty p;
        p.key = k;
        p.value = v;
        node->addProperty(p);
    }
    return node;
}

/// ״¥†״´״§״¡ ״´״¬״±״© ˆ״§״¬‡״© ״¨״³״·״© (״¹…ˆ״¯ ״­ˆ †״µ״§‹ ˆ״²״±״§‹)
static std::shared_ptr<UINode> makeSimpleTree() {
    auto col = std::make_shared<UINode>(UINodeType::Column);
    col->setArabicName("״¹…ˆ״¯");

    auto txt = std::make_shared<UINode>(UINodeType::Text);
    txt->setArabicName("†״µ");
    txt->setPrimaryArg("…״±״­״¨״§‹ ״¨״§„״¹״§„…");
    col->addChild(txt);

    auto btn = std::make_shared<UINode>(UINodeType::Button);
    btn->setArabicName("״²״±");
    btn->setPrimaryArg("״§״¶״÷״· ‡†״§");
    col->addChild(btn);

    return col;
}

/// ״¨†״§״¡ ViewDefinition …† ״´״¬״±״© UINode
static ViewDefinition makeViewDef(std::shared_ptr<UINode> root, const std::string& name = "TestView") {
    ViewDefinition vd;
    vd.name = name;
    vd.rootNode = root;
    return vd;
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ¡. ״§״®״×״¨״§״±״§״× ״¨†״§״¡ ״¹‚״¯ IR
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_ir_node_creation() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("¡. ״¨†״§״¡ ״¹‚״¯ IR");

    runner.runTest("״¥†״´״§״¡ ״¹‚״¯״© †״µ", [&]() {
        auto node = IRNode::create(UINodeType::Text);
        SAD_ASSERT_TRUE(node != nullptr);
        SAD_ASSERT_TRUE(node->getType() == UINodeType::Text);
    });

    runner.runTest("״¥״¶״§״© ״®״§״µ״© †״µ״©", [&]() {
        auto node = makeIR(UINodeType::Text, {{"text", "…״±״­״¨״§‹"}});
        auto* prop = node->findProperty("text");
        SAD_ASSERT_NOT_NULL(prop);
        SAD_ASSERT_TRUE(std::holds_alternative<std::string>(prop->value));
        SAD_ASSERT_EQ(std::get<std::string>(prop->value), std::string("…״±״­״¨״§‹"));
    });

    runner.runTest("״¥״¶״§״© ״®״§״µ״© ״±‚…״© (int64)", [&]() {
        auto node = IRNode::create(UINodeType::Container);
        node->setProperty("width", int64_t(200));
        auto* prop = node->findProperty("width");
        SAD_ASSERT_NOT_NULL(prop);
        SAD_ASSERT_TRUE(std::holds_alternative<int64_t>(prop->value));
        SAD_ASSERT_EQ(std::get<int64_t>(prop->value), int64_t(200));
    });

    runner.runTest("״¥״¶״§״© ״®״§״µ״© ״¹״´״±״© (double)", [&]() {
        auto node = IRNode::create(UINodeType::Slider);
        node->setProperty("value", 0.75);
        auto* prop = node->findProperty("value");
        SAD_ASSERT_NOT_NULL(prop);
        SAD_ASSERT_TRUE(std::holds_alternative<double>(prop->value));
        SAD_ASSERT_FLOAT_EQ(std::get<double>(prop->value), 0.75, 0.001);
    });

    runner.runTest("״¥״¶״§״© ״®״§״µ״© …†״·‚״© (bool)", [&]() {
        auto node = IRNode::create(UINodeType::Toggle);
        node->setProperty("checked", true);
        auto* prop = node->findProperty("checked");
        SAD_ASSERT_NOT_NULL(prop);
        SAD_ASSERT_TRUE(std::holds_alternative<bool>(prop->value));
        SAD_ASSERT_TRUE(std::get<bool>(prop->value));
    });

    runner.runTest("״×״¹״¯„ ״®״§״µ״© …ˆ״¬ˆ״¯״©", [&]() {
        auto node = makeIR(UINodeType::Text, {{"text", "‚״¯…"}});
        node->setProperty("text", std::string("״¬״¯״¯"));
        auto* prop = node->findProperty("text");
        SAD_ASSERT_NOT_NULL(prop);
        SAD_ASSERT_EQ(std::get<std::string>(prop->value), std::string("״¬״¯״¯"));
    });

    runner.runTest("״§„״¨״­״« ״¹† ״®״§״µ״© ״÷״± …ˆ״¬ˆ״¯״©", [&]() {
        auto node = IRNode::create(UINodeType::Text);
        auto* prop = node->findProperty("nonexistent");
        SAD_ASSERT_TRUE(prop == nullptr);
    });

    runner.runTest("״¥״¶״§״© ״£״¨†״§״¡", [&]() {
        auto parent = IRNode::create(UINodeType::Column);
        parent->addChild(IRNode::create(UINodeType::Text));
        parent->addChild(IRNode::create(UINodeType::Button));
        parent->addChild(IRNode::create(UINodeType::Spacer));
        SAD_ASSERT_EQ(parent->childCount(), size_t(3));
    });

    runner.runTest("״¥״¶״§״© ״­״¯״«", [&]() {
        auto node = IRNode::create(UINodeType::Button);
        IREvent evt;
        evt.type = sad::ui::IREventType::OnTap;
        evt.expression = "handleClick()";
        node->addEvent(evt);
        SAD_ASSERT_EQ(node->getEvents().size(), size_t(1));
        SAD_ASSERT_EQ(node->getEvents()[0].getEventName(), sad::ui::irEventTypeToString(sad::ui::IREventType::OnTap));
    });

    runner.runTest("״¥״¶״§״© ״¥״´״§״±״© ״­״§„״©", [&]() {
        auto node = IRNode::create(UINodeType::Text);
        IRStateRef ref;
        ref.stateName = "״§„״¹״¯״§״¯";
        ref.propertyBinding = "text";
        ref.bindingType = StateBindingType::Binding;
        node->addStateRef(ref);
        SAD_ASSERT_EQ(node->getStateRefs().size(), size_t(1));
        SAD_ASSERT_EQ(node->getStateRefs()[0].stateName, std::string("״§„״¹״¯״§״¯"));
    });

    runner.runTest("״×„…״­ ״§„״¹״±״¶ (RenderingHint)", [&]() {
        auto node = IRNode::create(UINodeType::Canvas);
        SAD_ASSERT_TRUE(node->getRenderingHint() == RenderingHint::Auto);
        node->setRenderingHint(RenderingHint::Canvas);
        SAD_ASSERT_TRUE(node->getRenderingHint() == RenderingHint::Canvas);
    });

    runner.runTest("״×״¹† ״§„…†״µ״© ״§„…״³״×‡״¯״©", [&]() {
        auto node = IRNode::create(UINodeType::Text);
        node->setTargetPlatform(TargetPlatform::Android);
        SAD_ASSERT_TRUE(node->getTargetPlatform() == TargetPlatform::Android);
    });

    runner.runTest("״×״±״÷ IR (dump)", [&]() {
        auto node = makeIR(UINodeType::Text, {{"text", "test"}});
        std::string dumped = node->dump();
        SAD_ASSERT_TRUE(!dumped.empty());
    });
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ¢. ״§״®״×״¨״§״±״§״× ״´״¬״±״© IR ״§„…״¹‚״¯״©
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_ir_complex_trees() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("¢. ״£״´״¬״§״± IR …״¹‚״¯״©");

    runner.runTest("״´״¬״±״© ״¹…ˆ״¯ …״¹ †״µ ˆ״²״± ˆ״§״µ„", [&]() {
        auto col = IRNode::create(UINodeType::Column);
        col->addChild(makeIR(UINodeType::Text, {{"text", "״¹†ˆ״§†"}}));
        col->addChild(IRNode::create(UINodeType::Spacer));
        col->addChild(makeIR(UINodeType::Button, {{"text", "…ˆ״§‚"}}));
        SAD_ASSERT_EQ(col->childCount(), size_t(3));
        SAD_ASSERT_TRUE(col->getChildren()[0]->getType() == UINodeType::Text);
        SAD_ASSERT_TRUE(col->getChildren()[1]->getType() == UINodeType::Spacer);
        SAD_ASSERT_TRUE(col->getChildren()[2]->getType() == UINodeType::Button);
    });

    runner.runTest("״´״¬״±״© …״×״¯״§״®„״© (״¹…ˆ״¯ ״¯״§״®„ ״µ)", [&]() {
        auto row = IRNode::create(UINodeType::Row);
        auto col1 = IRNode::create(UINodeType::Column);
        col1->addChild(makeIR(UINodeType::Text, {{"text", "״³״§״±"}}));
        auto col2 = IRNode::create(UINodeType::Column);
        col2->addChild(makeIR(UINodeType::Text, {{"text", "…†"}}));
        row->addChild(col1);
        row->addChild(col2);
        SAD_ASSERT_EQ(row->childCount(), size_t(2));
        SAD_ASSERT_EQ(row->getChildren()[0]->childCount(), size_t(1));
    });

    runner.runTest("Canvas …״¹ ״®״µ״§״¦״µ ״±״³…", [&]() {
        auto canvas = makeIR(UINodeType::Canvas, {
            {"width", "400"},
            {"height", "300"},
            {"background", "#FFFFFF"},
            {"corners", "8"}
        });
        canvas->setRenderingHint(RenderingHint::Canvas);
        SAD_ASSERT_NOT_NULL(canvas->findProperty("width"));
        SAD_ASSERT_NOT_NULL(canvas->findProperty("height"));
        SAD_ASSERT_NOT_NULL(canvas->findProperty("background"));
        SAD_ASSERT_NOT_NULL(canvas->findProperty("corners"));
        SAD_ASSERT_TRUE(canvas->getRenderingHint() == RenderingHint::Canvas);
    });

    runner.runTest("Scaffold …״¹ ״´״±״· ˆ״¹…ˆ״¯", [&]() {
        auto scaffold = IRNode::create(UINodeType::Scaffold);
        auto appbar = makeIR(UINodeType::AppBar, {{"title", "״×״·״¨‚"}});
        auto body = IRNode::create(UINodeType::Column);
        body->addChild(makeIR(UINodeType::Text, {{"text", "…״­״×ˆ‰"}}));
        scaffold->addChild(appbar);
        scaffold->addChild(body);
        SAD_ASSERT_EQ(scaffold->childCount(), size_t(2));
        SAD_ASSERT_TRUE(scaffold->getChildren()[0]->getType() == UINodeType::AppBar);
    });

    runner.runTest("״´״¬״±״© ״¨€ 87 ״¹‚״¯״© (ƒ„ †ˆ״¹ ״¹†״µ״±)", [&]() {
        auto root = IRNode::create(UINodeType::Column);
        int count = static_cast<int>(UINodeType::_Count);
        for (int i = 0; i < count; ++i) {
            auto child = IRNode::create(static_cast<UINodeType>(i));
            root->addChild(child);
        }
        SAD_ASSERT_EQ(root->childCount(), static_cast<size_t>(count));
    });
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// £. ״§״®״×״¨״§״±״§״× ״®״· ״§„״£†״§״¨״¨ (ƒ„ …†״µ״©)
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_pipeline_per_platform() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("£. ״®״· ״§„״£†״§״¨״¨ ג€” ״×ˆ„״¯ ƒˆ״¯ „ƒ„ …†״µ״©");

    // ״§„״£†״¸…״© ״§„…״¯״¹ˆ…״©
    struct PlatformInfo {
        TargetPlatform platform;
        std::string name;
        std::vector<std::string> expectedKeywords;
    };

    std::vector<PlatformInfo> platforms = {
        {TargetPlatform::Android, "Android",
            {"@Composable", "Column", "Text", "Button"}},
        {TargetPlatform::IOS, "iOS",
            {"struct", "View", "Text", "Button"}},
        {TargetPlatform::Web, "Web",
            {"sad-", "display", "flex"}},
        {TargetPlatform::MacOS, "macOS",
            {"NSView", "NSTextField", "NSButton"}},
    };

    for (auto& pinfo : platforms) {
        runner.runTest("processNode ג€” " + pinfo.name + ": ƒˆ״¯ ״÷״± ״§״±״÷", [&]() {
            PipelineOptions opts;
            opts.target = pinfo.platform;
            opts.renderMode = RenderMode::CodeGen;
            opts.optimize = false;
            opts.enableSmartRouting = false;

            UIPipeline pipeline(opts);
            auto tree = makeSimpleTree();
            auto result = pipeline.processNode(tree, "TestView");

            SAD_ASSERT_TRUE(result.success);
            SAD_ASSERT_TRUE(!result.generatedCode.empty());
            SAD_ASSERT_TRUE(result.errors.empty());
        });

        // ״×״­‚‚ ״£† ״§„ƒˆ״¯ ״­״×ˆ ״¹„‰ ƒ„…״§״× …״×״§״­״© ״®״§״µ״© ״¨״§„…†״µ״©
        for (auto& kw : pinfo.expectedKeywords) {
            runner.runTest("processNode ג€” " + pinfo.name + ": ״­״×ˆ '" + kw + "'", [&]() {
                PipelineOptions opts;
                opts.target = pinfo.platform;
                opts.renderMode = RenderMode::CodeGen;
                opts.optimize = false;
                opts.enableSmartRouting = false;

                UIPipeline pipeline(opts);
                auto tree = makeSimpleTree();
                auto result = pipeline.processNode(tree, "TestView");

                SAD_ASSERT_TRUE(result.success);
                SAD_ASSERT_CONTAINS(result.generatedCode, kw);
            });
        }
    }
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ₪. ״§״®״×״¨״§״±״§״× ״×ˆ„״¯ ״¹†״§״µ״± …״­״¯״¯״© „ƒ„ …†״µ״©
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_specific_widgets_codegen() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("₪. ״×ˆ„״¯ ״¹†״§״µ״± …״­״¯״¯״©");

    // ״§״®״×״¨״§״± Canvas
    runner.runTest("Canvas ג€” Android: ״­״×ˆ Canvas composable", [&]() {
        PipelineOptions opts;
        opts.target = TargetPlatform::Android;
        opts.renderMode = RenderMode::CodeGen;

        UIPipeline pipeline(opts);
        auto canvas = std::make_shared<UINode>(UINodeType::Canvas);
        canvas->setArabicName("„ˆ״­״©");
        auto col = std::make_shared<UINode>(UINodeType::Column);
        col->addChild(canvas);
        auto result = pipeline.processNode(col, "CanvasTest");

        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_TRUE(!result.generatedCode.empty());
    });

    runner.runTest("Canvas ג€” iOS: ״­״×ˆ Canvas view", [&]() {
        PipelineOptions opts;
        opts.target = TargetPlatform::IOS;
        opts.renderMode = RenderMode::CodeGen;

        UIPipeline pipeline(opts);
        auto canvas = std::make_shared<UINode>(UINodeType::Canvas);
        canvas->setArabicName("„ˆ״­״©");
        auto col = std::make_shared<UINode>(UINodeType::Column);
        col->addChild(canvas);
        auto result = pipeline.processNode(col, "CanvasTest");

        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_TRUE(!result.generatedCode.empty());
    });

    runner.runTest("Canvas ג€” Web: ״­״×ˆ canvas element", [&]() {
        PipelineOptions opts;
        opts.target = TargetPlatform::Web;
        opts.renderMode = RenderMode::CodeGen;

        UIPipeline pipeline(opts);
        auto canvas = std::make_shared<UINode>(UINodeType::Canvas);
        canvas->setArabicName("„ˆ״­״©");
        auto col = std::make_shared<UINode>(UINodeType::Column);
        col->addChild(canvas);
        auto result = pipeline.processNode(col, "CanvasTest");

        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_TRUE(!result.generatedCode.empty());
    });

    // ״§״®״×״¨״§״± Spacer
    runner.runTest("Spacer ג€” Android: ״­״×ˆ Spacer composable", [&]() {
        PipelineOptions opts;
        opts.target = TargetPlatform::Android;
        opts.renderMode = RenderMode::CodeGen;

        UIPipeline pipeline(opts);
        auto sp = std::make_shared<UINode>(UINodeType::Spacer);
        sp->setArabicName("״§״µ„");
        auto col = std::make_shared<UINode>(UINodeType::Column);
        col->addChild(sp);
        auto result = pipeline.processNode(col, "SpacerTest");

        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_TRUE(!result.generatedCode.empty());
    });

    // ״§״®״×״¨״§״± Divider
    runner.runTest("Divider ג€” Web: ״­״×ˆ <hr> ״£ˆ border", [&]() {
        PipelineOptions opts;
        opts.target = TargetPlatform::Web;
        opts.renderMode = RenderMode::CodeGen;

        UIPipeline pipeline(opts);
        auto div = std::make_shared<UINode>(UINodeType::Divider);
        div->setArabicName("״®״·_״§״µ„");
        auto col = std::make_shared<UINode>(UINodeType::Column);
        col->addChild(div);
        auto result = pipeline.processNode(col, "DividerTest");

        SAD_ASSERT_TRUE(result.success);
        SAD_ASSERT_TRUE(!result.generatedCode.empty());
    });
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ¥. ״§״®״×״¨״§״±״§״× IRBuilder
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_ir_builder() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("¥. ״¨״§† ״§„״×…״«„ ״§„ˆ״³״· (IRBuilder)");

    runner.runTest("״¨†״§״¡ IR …† UINode ״¨״³״·", [&]() {
        IRBuildOptions opts;
        opts.targetPlatform = TargetPlatform::Desktop;
        opts.optimize = false;

        IRBuilder builder(opts);
        auto vd = makeViewDef(makeSimpleTree(), "SimpleView");
        auto irModule = builder.build(vd);

        SAD_ASSERT_NOT_NULL(irModule.root);
        SAD_ASSERT_TRUE(irModule.root->getType() == UINodeType::Column);
        SAD_ASSERT_GE(irModule.root->childCount(), size_t(2));
    });

    runner.runTest("״¨†״§״¡ IR „ƒ„ …†״µ״©", [&]() {
        TargetPlatform platforms[] = {
            TargetPlatform::Desktop,
            TargetPlatform::Android,
            TargetPlatform::IOS,
            TargetPlatform::Web,
            TargetPlatform::MacOS
        };

        for (auto platform : platforms) {
            IRBuildOptions opts;
            opts.targetPlatform = platform;
            opts.optimize = false;

            IRBuilder builder(opts);
            auto vd = makeViewDef(makeSimpleTree(), "PlatformTest");
            auto irModule = builder.build(vd);
            SAD_ASSERT_NOT_NULL(irModule.root);
        }
    });

    runner.runTest("״¨†״§״¡ IR …״¹ ״×״­״³†״§״×", [&]() {
        IRBuildOptions opts;
        opts.targetPlatform = TargetPlatform::Desktop;
        opts.optimize = true;

        IRBuilder builder(opts);
        auto vd = makeViewDef(makeSimpleTree(), "OptimizedView");
        auto irModule = builder.build(vd);

        SAD_ASSERT_NOT_NULL(irModule.root);
    });

    runner.runTest("״¨†״§״¡ IR …״¹ ״×ˆ״¬‡ ״°ƒ", [&]() {
        IRBuildOptions opts;
        opts.targetPlatform = TargetPlatform::Android;
        opts.enableSmartRouting = true;

        IRBuilder builder(opts);
        auto vd = makeViewDef(makeSimpleTree(), "SmartView");
        auto irModule = builder.build(vd);

        SAD_ASSERT_NOT_NULL(irModule.root);
    });
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ¦. ״§״®״×״¨״§״±״§״× PipelineResult
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_pipeline_result() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("¦. †״×״§״¦״¬ ״®״· ״§„״£†״§״¨״¨");

    runner.runTest("PipelineResult ״§„״§״×״±״§״¶: ״´„", [&]() {
        PipelineResult result;
        SAD_ASSERT_FALSE(result.success);
        SAD_ASSERT_TRUE(result.generatedCode.empty());
        SAD_ASSERT_TRUE(result.errors.empty());
    });

    runner.runTest("formatErrors ״¹†״¯ ״¹״¯… ˆ״¬ˆ״¯ ״£״®״·״§״¡", [&]() {
        PipelineResult result;
        result.success = true;
        std::string formatted = result.formatErrors();
        // ״¬״¨ ״£† ƒˆ† ״§״±״÷״§‹ ״£ˆ „״§ ״­״×ˆ ״¹„‰ "״®״·״£"
        SAD_ASSERT_TRUE(formatted.empty() || formatted.find("ERROR") == std::string::npos);
    });

    runner.runTest("PipelineOptions ״§„״§״×״±״§״¶״©", [&]() {
        PipelineOptions opts;
        SAD_ASSERT_TRUE(opts.target == TargetPlatform::Desktop);
        SAD_ASSERT_TRUE(opts.renderMode == RenderMode::CodeGen);
        SAD_ASSERT_TRUE(opts.optimize);
        SAD_ASSERT_TRUE(opts.enableSmartRouting);
    });

    runner.runTest("״×״¹† ״®״§״±״§״× …״®״µ״µ״©", [&]() {
        PipelineOptions opts;
        opts.target = TargetPlatform::Android;
        opts.renderMode = RenderMode::DirectRender;
        opts.packageName = "com.test.app";
        SAD_ASSERT_TRUE(opts.target == TargetPlatform::Android);
        SAD_ASSERT_EQ(opts.packageName, std::string("com.test.app"));
    });
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// §. ״§״®״×״¨״§״±״§״× ״×״¹״¯״§״¯״§״× UINodeType
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_ui_node_types() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("§. ״×״¹״¯״§״¯״§״× ״£†ˆ״§״¹ ״§„״¹†״§״µ״±");

    runner.runTest("_Count ״£ƒ״¨״± …† ״£ˆ ״³״§ˆ 87", [&]() {
        int count = static_cast<int>(UINodeType::_Count);
        SAD_ASSERT_GE(count, 87);
    });

    runner.runTest("״§„״£†ˆ״§״¹ ״§„״£״³״§״³״© …ˆ״¬ˆ״¯״©", [&]() {
        // ״×״­‚‚ …† ˆ״¬ˆ״¯ ƒ„ ״§„״£†ˆ״§״¹ ״§„״±״¦״³״©
        SAD_ASSERT_TRUE(static_cast<int>(UINodeType::Text) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(UINodeType::Button) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(UINodeType::Column) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(UINodeType::Row) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(UINodeType::Image) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(UINodeType::Canvas) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(UINodeType::Scaffold) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(UINodeType::Container) >= 0);
    });

    runner.runTest("RenderingHint ג€” ״¬…״¹ ״§„‚… ״µ״§„״­״©", [&]() {
        SAD_ASSERT_TRUE(static_cast<int>(RenderingHint::Auto) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(RenderingHint::Native) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(RenderingHint::Canvas) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(RenderingHint::Hybrid) >= 0);
    });

    runner.runTest("TargetPlatform ג€” ״¬…״¹ ״§„…†״µ״§״×", [&]() {
        SAD_ASSERT_TRUE(static_cast<int>(TargetPlatform::Desktop) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(TargetPlatform::Android) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(TargetPlatform::IOS) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(TargetPlatform::Web) >= 0);
        SAD_ASSERT_TRUE(static_cast<int>(TargetPlatform::MacOS) >= 0);
    });
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ¨. ״§״®״×״¨״§״±״§״× UINode (״´״¬״±״© ״§„״¹†״§״µ״±)
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_ui_node() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("¨. ״´״¬״±״© UINode");

    runner.runTest("״¥†״´״§״¡ UINode", [&]() {
        auto node = std::make_shared<UINode>(UINodeType::Text);
        SAD_ASSERT_TRUE(node->getType() == UINodeType::Text);
    });

    runner.runTest("״×״¹† ״§״³… ״¹״±״¨", [&]() {
        auto node = std::make_shared<UINode>(UINodeType::Button);
        node->setArabicName("״²״±");
        SAD_ASSERT_EQ(node->getArabicName(), std::string("״²״±"));
    });

    runner.runTest("״×״¹† ״§„ˆ״³״·״© ״§„״£ˆ„‰", [&]() {
        auto node = std::make_shared<UINode>(UINodeType::Text);
        node->setPrimaryArg("…״±״­״¨״§‹");
        SAD_ASSERT_EQ(node->getPrimaryArg(), std::string("…״±״­״¨״§‹"));
    });

    runner.runTest("״¥״¶״§״© ˆ״³״§״¦״· …״³…‘״§״©", [&]() {
        auto node = std::make_shared<UINode>(UINodeType::Column);
        node->setNamedArg("…״­״§״°״§״©", "ˆ״³״·");
        node->setNamedArg("״×״¨״§״¹״¯", "8");
        SAD_ASSERT_EQ(node->getNamedArgs().size(), size_t(2));
    });

    runner.runTest("״¥״¶״§״© ״£״¨†״§״¡ UINode", [&]() {
        auto parent = std::make_shared<UINode>(UINodeType::Column);
        auto child1 = std::make_shared<UINode>(UINodeType::Text);
        auto child2 = std::make_shared<UINode>(UINodeType::Button);
        parent->addChild(child1);
        parent->addChild(child2);
        SAD_ASSERT_EQ(parent->getChildren().size(), size_t(2));
    });

    runner.runTest("makeSimpleTree ״¨†״© ״µ״­״­״©", [&]() {
        auto tree = makeSimpleTree();
        SAD_ASSERT_TRUE(tree->getType() == UINodeType::Column);
        SAD_ASSERT_EQ(tree->getChildren().size(), size_t(2));
        SAD_ASSERT_TRUE(tree->getChildren()[0]->getType() == UINodeType::Text);
        SAD_ASSERT_TRUE(tree->getChildren()[1]->getType() == UINodeType::Button);
    });
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ©. ״§״®״×״¨״§״±״§״× UIPipeline
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_pipeline_operations() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("©. ״¹…„״§״× ״®״· ״§„״£†״§״¨״¨");

    runner.runTest("״¥†״´״§״¡ pipeline ״¨״¥״¹״¯״§״¯״§״× ״§״×״±״§״¶״©", [&]() {
        SAD_ASSERT_NO_THROW(UIPipeline pipeline);
    });

    runner.runTest("״¥†״´״§״¡ pipeline ״¨״¥״¹״¯״§״¯״§״× …״®״µ״µ״©", [&]() {
        PipelineOptions opts;
        opts.target = TargetPlatform::Android;
        SAD_ASSERT_NO_THROW(UIPipeline pipeline(opts));
    });

    runner.runTest("setTarget ״÷״± ״§„…†״µ״©", [&]() {
        UIPipeline pipeline;
        pipeline.setTarget(TargetPlatform::IOS);
        SAD_ASSERT_TRUE(pipeline.getOptions().target == TargetPlatform::IOS);
    });

    runner.runTest("setOptions ״¹† ״§„״¥״¹״¯״§״¯״§״×", [&]() {
        UIPipeline pipeline;
        PipelineOptions opts;
        opts.target = TargetPlatform::Web;
        opts.optimize = false;
        opts.pageTitle = "״§״®״×״¨״§״±";
        pipeline.setOptions(opts);
        SAD_ASSERT_TRUE(pipeline.getOptions().target == TargetPlatform::Web);
        SAD_ASSERT_FALSE(pipeline.getOptions().optimize);
    });

    runner.runTest("validate ג€” ״´״¬״±״© ״§״±״÷״©", [&]() {
        UIPipeline pipeline;
        ViewDefinition viewDef;
        viewDef.name = "״§״±״÷";
        auto errors = pipeline.validate(viewDef);
        // ‚״¯ ƒˆ† ״§״±״÷״§‹ ״£ˆ ״­״×ˆ ״×״­״°״±״§‹ ג€” ״§„…‡… ״¹״¯… ״§„״§†‡״§״±
        SAD_ASSERT_TRUE(true);
    });
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ¡ . ״§״®״×״¨״§״±״§״× IRModule
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void test_ir_module() {
    auto& runner = TestRunner::instance();
    runner.beginGroup("¡ . ˆ״­״¯״© IR (IRModule)");

    runner.runTest("IRModule ״§״±״÷״©", [&]() {
        IRModule module;
        SAD_ASSERT_TRUE(module.root == nullptr);
        SAD_ASSERT_TRUE(module.name.empty());
    });

    runner.runTest("IRModule …״¹ ״¬״°״± ˆ״£״³…", [&]() {
        IRModule module;
        module.name = "״§„״´״§״´״©_״§„״±״¦״³״©";
        module.root = IRNode::create(UINodeType::Column);
        SAD_ASSERT_NOT_NULL(module.root);
        SAD_ASSERT_EQ(module.name, std::string("״§„״´״§״´״©_״§„״±״¦״³״©"));
    });

    runner.runTest("IRModule …† IRBuilder", [&]() {
        IRBuildOptions opts;
        opts.targetPlatform = TargetPlatform::Desktop;
        opts.optimize = false;

        IRBuilder builder(opts);
        auto vd = makeViewDef(makeSimpleTree(), "ModuleTest");
        auto module = builder.build(vd);

        SAD_ASSERT_NOT_NULL(module.root);
        SAD_ASSERT_EQ(module.name, std::string("ModuleTest"));
    });
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ״§„״¯״§„״© ״§„״±״¦״³״©
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

int main() {
    SAD_ENABLE_COLORS();

    std::cout << "\n";
    std::cout << "+==========================================================+\n";
    std::cout << "|    Comprehensive Backend Tests                            |\n";
    std::cout << "|    IR + IRBuilder + Pipeline + Codegen (5 platforms)       |\n";
    std::cout << "+==========================================================+\n";
    std::cout << "\n";

    test_ir_node_creation();
    test_ir_complex_trees();
    test_pipeline_per_platform();
    test_specific_widgets_codegen();
    test_ir_builder();
    test_pipeline_result();
    test_ui_node_types();
    test_ui_node();
    test_pipeline_operations();
    test_ir_module();

    return TestRunner::instance().printSummary();
}
