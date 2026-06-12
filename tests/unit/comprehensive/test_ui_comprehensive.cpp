// بسم الله الرحمن الرحيم
/**
 * @file test_ui_comprehensive.cpp
 * @brief اختبارات شاملة لجميع عناصر الواجهة الـ 87 + المترجم SIR + الربط
 *        + اختبارات البنية التحتية Phase -1 (CommandQueue, Signal, PropertyBinding, UIEventLoop)
 */

#include "sad_test_framework.h"
#include "../../compiler/src/sir/sir_opcodes.h"
#include "sad_ui/ui_arena.h"
#include "sad_ui/command_queue.h"
#include "sad_ui/ui_command.h"
#include "sad_ui/signal.h"
#include "sad_ui/property_binding.h"
#include "sad_ui/ui_event_loop.h"

#define SAD_ASSERT(cond, msg) SAD_ASSERT_TRUE(cond)

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <chrono>

using namespace SadTest;
using namespace sad::sir;
using namespace sad::ui;
struct WidgetInfo
{
    std::string name;
    std::string category;
    std::string file;
};

static const std::vector<WidgetInfo> &get_all_widgets()
{
    static std::vector<WidgetInfo> widgets = {
        {"نص", "نصوص", "أساسيات.ص"},
        {"عنوان", "نصوص", "أساسيات.ص"},
        {"تسمية", "نصوص", "أساسيات.ص"},
        {"فقرة", "نصوص", "أساسيات.ص"},
        {"رابط", "نصوص", "أساسيات.ص"},
        {"كود", "نصوص", "أساسيات.ص"},
        {"اقتباس", "نصوص", "أساسيات.ص"},
        {"صورة", "وسائط", "أساسيات.ص"},
        {"ايقونة", "وسائط", "أساسيات.ص"},
        {"شارة", "وسائط", "أساسيات.ص"},
        {"رقاقة", "وسائط", "أساسيات.ص"},
        {"صورة_رمزية", "وسائط", "أساسيات.ص"},
        {"زر", "أزرار", "أزرار.ص"},
        {"زر_محيط", "أزرار", "أزرار.ص"},
        {"زر_نصي", "أزرار", "أزرار.ص"},
        {"زر_ايقونة", "أزرار", "أزرار.ص"},
        {"زر_عائم", "أزرار", "أزرار.ص"},
        {"زر_تبديل", "أزرار", "أزرار.ص"},
        {"حقل", "إدخال", "إدخال.ص"},
        {"حقل_بحث", "إدخال", "إدخال.ص"},
        {"حقل_سر", "إدخال", "إدخال.ص"},
        {"منطقة_نص", "إدخال", "إدخال.ص"},
        {"مفتاح", "إدخال", "إدخال.ص"},
        {"منزلق", "إدخال", "إدخال.ص"},
        {"مربع_اختيار", "إدخال", "إدخال.ص"},
        {"قائمة_منسدلة", "إدخال", "إدخال.ص"},
        {"تقييم", "إدخال", "إدخال.ص"},
        {"منتقي_تاريخ", "منتقيات", "منتقيات.ص"},
        {"منتقي_وقت", "منتقيات", "منتقيات.ص"},
        {"منتقي_لون", "منتقيات", "منتقيات.ص"},
        {"عجلة_ألوان", "منتقيات", "منتقيات.ص"},
        {"تقويم", "منتقيات", "منتقيات.ص"},
        {"زر_راديو", "منتقيات", "منتقيات.ص"},
        {"خطوات", "منتقيات", "منتقيات.ص"},
        {"تحكم_مقسم", "منتقيات", "منتقيات.ص"},
        {"شريط_تقدم", "تحميل", "تحميل.ص"},
        {"تقدم_دائري", "تحميل", "تحميل.ص"},
        {"تحميل", "تحميل", "تحميل.ص"},
        {"هيكل_تحميل", "تحميل", "تحميل.ص"},
        {"عمود", "تخطيط", "تخطيط.ص"},
        {"صف", "تخطيط", "تخطيط.ص"},
        {"تكديس", "تخطيط", "تخطيط.ص"},
        {"شبكة", "تخطيط", "تخطيط.ص"},
        {"حاوية", "تخطيط", "تخطيط.ص"},
        {"توسيط", "تخطيط", "تخطيط.ص"},
        {"مرن", "تخطيط", "تخطيط.ص"},
        {"موسّع", "تخطيط", "تخطيط.ص"},
        {"فاصل", "تخطيط", "تخطيط.ص"},
        {"خط_فاصل", "تخطيط", "تخطيط.ص"},
        {"التفاف", "تخطيط", "تخطيط.ص"},
        {"صندوق", "تخطيط", "تخطيط.ص"},
        {"سطح", "تخطيط", "تخطيط.ص"},
        {"عمود_كسول", "تخطيط", "تخطيط_كسول.ص"},
        {"صف_كسول", "تخطيط", "تخطيط_كسول.ص"},
        {"شبكة_كسولة", "تخطيط", "تخطيط_كسول.ص"},
        {"عرض_تمرير", "تخطيط", "تخطيط_كسول.ص"},
        {"عرض_مقسم", "تخطيط", "تخطيط_كسول.ص"},
        {"هيكل", "هيكل", "هيكل.ص"},
        {"شريط_تطبيق", "هيكل", "هيكل.ص"},
        {"شريط_سفلي", "هيكل", "هيكل.ص"},
        {"عنصر_تنقل", "هيكل", "هيكل.ص"},
        {"درج", "هيكل", "هيكل.ص"},
        {"عنصر_درج", "هيكل", "هيكل.ص"},
        {"شريط_تبويب", "هيكل", "هيكل.ص"},
        {"تبويب", "هيكل", "هيكل.ص"},
        {"محتوى_تبويب", "هيكل", "هيكل.ص"},
        {"عرض_تنقل", "هيكل", "تنقل.ص"},
        {"رابط_تنقل", "هيكل", "تنقل.ص"},
        {"تنقل_جانبي", "هيكل", "تنقل.ص"},
        {"شريط_أدوات", "هيكل", "تنقل.ص"},
        {"قائمة_ضخمة", "هيكل", "تنقل.ص"},
        {"قائمة", "بيانات", "بيانات.ص"},
        {"عنصر_قائمة_بيانات", "بيانات", "بيانات.ص"},
        {"بطاقة", "بيانات", "بيانات.ص"},
        {"قائمة_شبكية", "بيانات", "بيانات.ص"},
        {"قائمة_تمرير", "بيانات", "بيانات.ص"},
        {"أكورديون", "بيانات", "بيانات.ص"},
        {"حوار", "حوارات", "حوارات.ص"},
        {"حوار_تأكيد", "حوارات", "حوارات.ص"},
        {"لوحة_سفلية", "حوارات", "حوارات.ص"},
        {"رسالة_منبثقة", "حوارات", "حوارات.ص"},
        {"تلميح", "حوارات", "حوارات.ص"},
        {"قائمة_منبثقة", "حوارات", "حوارات.ص"},
        {"عنصر_قائمة", "حوارات", "حوارات.ص"},
        {"مشغل_فيديو", "وسائط_متعددة", "وسائط_متعددة.ص"},
        {"مشغل_صوت", "وسائط_متعددة", "وسائط_متعددة.ص"},
        {"معرض_صور", "وسائط_متعددة", "وسائط_متعددة.ص"},
        {"عرض_دوار", "وسائط_متعددة", "وسائط_متعددة.ص"},
        {"نص_منسق", "متقدم", "متقدم.ص"},
        {"ماركداون", "متقدم", "متقدم.ص"},
        {"لوحة", "متقدم", "متقدم.ص"},
        {"عرض_ويب", "متقدم", "متقدم.ص"},
        {"خريطة", "متقدم", "متقدم.ص"},
        {"جدول_بيانات", "متقدم", "متقدم.ص"},
        {"عرض_شجري", "متقدم", "متقدم.ص"},
        {"مسار_تنقل", "متقدم", "متقدم.ص"},
        {"ترقيم_صفحات", "متقدم", "متقدم.ص"},
        {"خط_زمني", "متقدم", "متقدم.ص"},
        {"قسم", "متقدم", "متقدم.ص"},
        {"وميض", "حركة", "حركة.ص"},
        {"قائمة_متحركة", "حركة", "حركة.ص"},
        {"قابل_للطي", "حركة", "حركة.ص"},
        {"قابل_للسحب", "حركة", "حركة.ص"},
        {"تنبيه", "حركة", "حركة.ص"},
        {"مجموعة", "حركة", "حركة.ص"},
    };
    return widgets;
}
// ١. اختبارات جرد العناصر
void test_widget_inventory()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("1. Widget Inventory");
    runner.runTest("87+ widgets total", [&]()
                   { SAD_ASSERT(get_all_widgets().size() >= 87, "Count: " + std::to_string(get_all_widgets().size())); });
    runner.runTest("No duplicates", [&]()
                   {
        std::unordered_set<std::string> seen;
        for (const auto& w : get_all_widgets()) {
            SAD_ASSERT(seen.insert(w.name).second, "Dup: " + w.name);
        } });
    runner.runTest("All names non-empty", [&]()
                   {
        for (const auto& w : get_all_widgets()) {
            SAD_ASSERT(!w.name.empty(), "Empty name!");
            SAD_ASSERT(!w.category.empty(), "Empty cat: " + w.name);
            SAD_ASSERT(!w.file.empty(), "Empty file: " + w.name);
        } });
    runner.runTest("All categorized", [&]()
                   {
        std::unordered_set<std::string> valid = {"نصوص","وسائط","أزرار","إدخال","منتقيات","تحميل","تخطيط","هيكل","بيانات","حوارات","وسائط_متعددة","متقدم","حركة"};
        for (const auto& w : get_all_widgets()) {
            SAD_ASSERT(valid.count(w.category), "Bad cat: " + w.category);
        } });
}

// ٢. اختبارات التصنيفات
void test_widget_categories()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("2. Widget Categories");
    std::unordered_map<std::string, int> cc;
    for (const auto &w : get_all_widgets())
        cc[w.category]++;
    runner.runTest("7 text", [&]()
                   { SAD_ASSERT_EQ(cc["نصوص"], 7); });
    runner.runTest("5 media", [&]()
                   { SAD_ASSERT_EQ(cc["وسائط"], 5); });
    runner.runTest("6 buttons", [&]()
                   { SAD_ASSERT_EQ(cc["أزرار"], 6); });
    runner.runTest("9 input", [&]()
                   { SAD_ASSERT_EQ(cc["إدخال"], 9); });
    runner.runTest("8 pickers", [&]()
                   { SAD_ASSERT_EQ(cc["منتقيات"], 8); });
    runner.runTest("4 loading", [&]()
                   { SAD_ASSERT_EQ(cc["تحميل"], 4); });
    runner.runTest("18 layout", [&]()
                   { SAD_ASSERT_EQ(cc["تخطيط"], 18); });
    runner.runTest("14 scaffold", [&]()
                   { SAD_ASSERT_EQ(cc["هيكل"], 14); });
    runner.runTest("6 data", [&]()
                   { SAD_ASSERT_EQ(cc["بيانات"], 6); });
    runner.runTest("7 dialogs", [&]()
                   { SAD_ASSERT_EQ(cc["حوارات"], 7); });
    runner.runTest("4 multimedia", [&]()
                   { SAD_ASSERT_EQ(cc["وسائط_متعددة"], 4); });
    runner.runTest("11 advanced", [&]()
                   { SAD_ASSERT_EQ(cc["متقدم"], 11); });
    runner.runTest("6 animation", [&]()
                   { SAD_ASSERT_EQ(cc["حركة"], 6); });
    runner.runTest("13 categories", [&]()
                   { SAD_ASSERT_EQ(static_cast<int>(cc.size()), 13); });
}
// ٣. SIR opcodes
void test_sir_ui_opcodes()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("3. SIR UI Opcodes");
    runner.runTest("UICreateWidget", [&]()
                   {
        auto inst = SirInstruction::UICreateWidget(1, "زر", {2, 3});
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode), static_cast<int>(Opcode::UICreateWidget));
        SAD_ASSERT(inst.result.has_value(), "must have result"); });
    runner.runTest("UISetProperty", [&]()
                   {
        auto inst = SirInstruction::UISetProperty(1, "لون", 2);
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode), static_cast<int>(Opcode::UISetProperty));
        SAD_ASSERT_EQ(static_cast<int>(inst.operands.size()), 3); });
    runner.runTest("UIGetProperty", [&]()
                   {
        auto inst = SirInstruction::UIGetProperty(3, 1, "عرض");
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode), static_cast<int>(Opcode::UIGetProperty));
        SAD_ASSERT(inst.result.has_value(), "must have result"); });
    runner.runTest("UIAddChild", [&]()
                   {
        auto inst = SirInstruction::UIAddChild(1, 2);
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode), static_cast<int>(Opcode::UIAddChild));
        SAD_ASSERT_EQ(static_cast<int>(inst.operands.size()), 2); });
    runner.runTest("UIBindEvent", [&]()
                   {
        auto inst = SirInstruction::UIBindEvent(1, "ضغط", 2);
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode), static_cast<int>(Opcode::UIBindEvent));
        SAD_ASSERT_EQ(static_cast<int>(inst.operands.size()), 3); });
    runner.runTest("UIRender", [&]()
                   {
        auto inst = SirInstruction::UIRender(1);
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode), static_cast<int>(Opcode::UIRender)); });
    runner.runTest("UIRunApp", [&]()
                   {
        auto inst = SirInstruction::UIRunApp(1, "test app");
        SAD_ASSERT_EQ(static_cast<int>(inst.opcode), static_cast<int>(Opcode::UIRunApp));
        SAD_ASSERT_EQ(static_cast<int>(inst.operands.size()), 2); });
    runner.runTest("OpCategory::UI = 9", [&]()
                   { SAD_ASSERT_EQ(static_cast<int>(OpCategory::UI), 9); });
    runner.runTest("All 10 UI ops exist", [&]()
                   {
        std::vector<Opcode> ops = {Opcode::UICreateWidget, Opcode::UISetProperty, Opcode::UIGetProperty, Opcode::UIAddChild, Opcode::UIRemoveChild, Opcode::UIBindEvent, Opcode::UIEmitEvent, Opcode::UISetLayout, Opcode::UIRender, Opcode::UIRunApp};
        SAD_ASSERT_EQ(static_cast<int>(ops.size()), 10); });
}

// ٤. سيناريوهات
void test_usage_scenarios()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("4. Usage Scenarios");
    runner.runTest("Simple app scaffold", [&]()
                   {
        auto s = SirInstruction::UICreateWidget(1, "هيكل");
        auto r = SirInstruction::UIRunApp(1, "app");
        SAD_ASSERT_EQ(static_cast<int>(s.opcode), static_cast<int>(Opcode::UICreateWidget));
        SAD_ASSERT_EQ(static_cast<int>(r.opcode), static_cast<int>(Opcode::UIRunApp)); });
    runner.runTest("Form scenario", [&]()
                   {
        auto f = SirInstruction::UICreateWidget(1, "عمود");
        auto a = SirInstruction::UIAddChild(1, 2);
        SAD_ASSERT_EQ(static_cast<int>(a.operands.size()), 2); });
    runner.runTest("Dialog scenario", [&]()
                   {
        auto d = SirInstruction::UICreateWidget(1, "حوار_تأكيد");
        SAD_ASSERT(d.comment.find("حوار_تأكيد") != std::string::npos, ""); });
    runner.runTest("Scroll list scenario", [&]()
                   {
        auto sv = SirInstruction::UICreateWidget(1, "عرض_تمرير");
        SAD_ASSERT_EQ(static_cast<int>(sv.opcode), static_cast<int>(Opcode::UICreateWidget)); });
    runner.runTest("Tab scenario", [&]()
                   {
        auto t = SirInstruction::UICreateWidget(2, "تبويب");
        SAD_ASSERT(t.comment.find("تبويب") != std::string::npos, ""); });
}

// ٥. توزيع الملفات
void test_file_distribution()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("5. File Distribution");
    std::unordered_map<std::string, int> fc;
    for (const auto &w : get_all_widgets())
        fc[w.file]++;
    runner.runTest("10+ files", [&]()
                   { SAD_ASSERT(fc.size() >= 10, "Too few: " + std::to_string(fc.size())); });
    runner.runTest("Max 20 per file", [&]()
                   {
        for (const auto& [f, c] : fc) SAD_ASSERT(c <= 20, f + ": " + std::to_string(c)); });
    runner.runTest("basics has 10+", [&]()
                   {
        SAD_ASSERT(fc.count("أساسيات.ص"), ""); SAD_ASSERT(fc["أساسيات.ص"] >= 10, ""); });
    runner.runTest("buttons has 6", [&]()
                   {
        SAD_ASSERT(fc.count("أزرار.ص"), ""); SAD_ASSERT_EQ(fc["أزرار.ص"], 6); });
}
// ٦. Handle liveness
void test_sadui_handle_liveness()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("6. Handle Liveness");
    runner.runTest("Handle invalid after remove", [&]()
                   {
        UIArena arena;
        auto root = arena.createRoot("عمود");
        SAD_ASSERT_TRUE(root.isValid());
        auto child = arena.createChild(root.getId(), "نص");
        SAD_ASSERT_TRUE(child.isValid());
        SAD_ASSERT_TRUE(arena.removeNode(child.getId()));
        SAD_ASSERT_FALSE(arena.exists(child.getId()));
        SAD_ASSERT_FALSE(child.isValid()); });
    runner.runTest("Handle invalid after clear", [&]()
                   {
        UIArena arena;
        auto root = arena.createRoot("صف");
        auto child = arena.createChild(root.getId(), "زر");
        arena.clear();
        SAD_ASSERT_FALSE(root.isValid());
        SAD_ASSERT_FALSE(child.isValid());
        SAD_ASSERT_TRUE(arena.getRoot() == nullptr);
        SAD_ASSERT_EQ(static_cast<int>(arena.getNodeCount()), 0); });
}

// ٧. CommandQueue
void test_command_queue()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("7. CommandQueue");
    runner.runTest("push and drain", [&]()
                   {
        CommandQueue queue;
        queue.push(UICommand::makeCreateRoot("عمود"));
        queue.push(UICommand::makeSetProperty(1, "لون", std::any(std::string("أحمر"))));
        auto batch = queue.drain();
        SAD_ASSERT_EQ(static_cast<int>(batch.size()), 2);
        SAD_ASSERT_TRUE(batch[0].type == UICommandType::CREATE_ROOT);
        SAD_ASSERT_TRUE(batch[1].type == UICommandType::SET_PROPERTY); });
    runner.runTest("drain returns empty after drain", [&]()
                   {
        CommandQueue queue;
        queue.push(UICommand::makeCreateRoot("صف"));
        auto b1 = queue.drain();
        SAD_ASSERT_EQ(static_cast<int>(b1.size()), 1);
        auto b2 = queue.drain();
        SAD_ASSERT_EQ(static_cast<int>(b2.size()), 0); });
    runner.runTest("pendingCount reflects state", [&]()
                   {
        CommandQueue queue;
        SAD_ASSERT_EQ(static_cast<int>(queue.pendingCount()), 0);
        queue.push(UICommand::makeCreateRoot("نص"));
        SAD_ASSERT_TRUE(queue.pendingCount() > 0);
        queue.drain();
        SAD_ASSERT_EQ(static_cast<int>(queue.pendingCount()), 0); });
    runner.runTest("FIFO order preserved", [&]()
                   {
        CommandQueue queue;
        queue.push(UICommand::makeCreateRoot("عمود"));
        queue.push(UICommand::makeCreateChild(1, "نص"));
        queue.push(UICommand::makeSetProperty(2, "محتوى", std::any(std::string("مرحبا"))));
        queue.push(UICommand::makeRemoveNode(2));
        queue.push(UICommand::makeClearTree());
        auto batch = queue.drain();
        SAD_ASSERT_EQ(static_cast<int>(batch.size()), 5);
        SAD_ASSERT_TRUE(batch[0].type == UICommandType::CREATE_ROOT);
        SAD_ASSERT_TRUE(batch[1].type == UICommandType::CREATE_CHILD);
        SAD_ASSERT_TRUE(batch[2].type == UICommandType::SET_PROPERTY);
        SAD_ASSERT_TRUE(batch[3].type == UICommandType::REMOVE_NODE);
        SAD_ASSERT_TRUE(batch[4].type == UICommandType::CLEAR_TREE); });
    runner.runTest("Thread safety producer/consumer", [&]()
                   {
        CommandQueue queue;
        constexpr int N = 1000;
        std::atomic<int> total{0};
        std::thread producer([&]() {
            for (int i = 0; i < N; ++i)
                queue.push(UICommand::makeSetProperty(static_cast<WidgetId>(i), "p", std::any(i)));
        });
        while (total.load() < N) {
            auto batch = queue.drain();
            total.fetch_add(static_cast<int>(batch.size()));
            if (batch.empty()) std::this_thread::yield();
        }
        producer.join();
        SAD_ASSERT_EQ(total.load(), N); });
    runner.runTest("SHUTDOWN command", [&]()
                   {
        auto cmd = UICommand::makeShutdown();
        SAD_ASSERT_TRUE(cmd.type == UICommandType::SHUTDOWN);
        SAD_ASSERT_EQ(static_cast<int>(cmd.targetId), static_cast<int>(INVALID_WIDGET_ID)); });
}
// ٨. Signal<T>
void test_signal()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("8. Signal<T>");
    runner.runTest("Initial value", [&]()
                   {
        Signal<int> sig(42);
        SAD_ASSERT_EQ(sig.get(), 42); });
    runner.runTest("set changes value", [&]()
                   {
        Signal<std::string> sig("old");
        bool changed = sig.set("new");
        SAD_ASSERT_TRUE(changed);
        SAD_ASSERT_TRUE(sig.get() == "new"); });
    runner.runTest("set same returns false", [&]()
                   {
        Signal<int> sig(10);
        bool changed = sig.set(10);
        SAD_ASSERT_FALSE(changed); });
    runner.runTest("subscribe notifies", [&]()
                   {
        Signal<int> sig(0);
        int received = -1;
        sig.subscribe([&](const int &v) { received = v; });
        sig.set(99);
        SAD_ASSERT_EQ(received, 99); });
    runner.runTest("Multiple subscribers", [&]()
                   {
        Signal<int> sig(0);
        int c1 = 0, c2 = 0;
        sig.subscribe([&](const int &) { ++c1; });
        sig.subscribe([&](const int &) { ++c2; });
        sig.set(5);
        SAD_ASSERT_EQ(c1, 1);
        SAD_ASSERT_EQ(c2, 1);
        sig.set(10);
        SAD_ASSERT_EQ(c1, 2);
        SAD_ASSERT_EQ(c2, 2); });
    runner.runTest("unsubscribe stops notifications", [&]()
                   {
        Signal<int> sig(0);
        int count = 0;
        auto subId = sig.subscribe([&](const int &) { ++count; });
        sig.set(1);
        SAD_ASSERT_EQ(count, 1);
        sig.unsubscribe(subId);
        sig.set(2);
        SAD_ASSERT_EQ(count, 1); });
    runner.runTest("clearSubscriptions", [&]()
                   {
        Signal<int> sig(0);
        int count = 0;
        sig.subscribe([&](const int &) { ++count; });
        sig.subscribe([&](const int &) { ++count; });
        SAD_ASSERT_EQ(static_cast<int>(sig.subscriberCount()), 2);
        sig.clearSubscriptions();
        SAD_ASSERT_EQ(static_cast<int>(sig.subscriberCount()), 0);
        sig.set(100);
        SAD_ASSERT_EQ(count, 0); });
    runner.runTest("forceSet notifies even if same", [&]()
                   {
        Signal<int> sig(42);
        int n = 0;
        sig.subscribe([&](const int &) { ++n; });
        sig.forceSet(42);
        SAD_ASSERT_EQ(n, 1); });
    runner.runTest("modify updates and notifies", [&]()
                   {
        Signal<std::string> sig("a");
        std::string received;
        sig.subscribe([&](const std::string &v) { received = v; });
        sig.modify([](std::string &v) { v += "b"; });
        SAD_ASSERT_TRUE(sig.get() == "ab");
        SAD_ASSERT_TRUE(received == "ab"); });
}
// ٩. PropertyBinding
void test_property_binding()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("9. PropertyBinding");
    runner.runTest("bind generates SET_PROPERTY", [&]()
                   {
        CommandQueue queue;
        BindingManager manager(queue);
        Signal<std::string> title("hello");
        auto bindId = manager.bind(1, "content", title);
        SAD_ASSERT_TRUE(bindId != INVALID_BINDING_ID);
        SAD_ASSERT_EQ(static_cast<int>(manager.getBindingCount()), 1);
        queue.drain(); // drain initial sync
        title.set("world");
        auto cmds = queue.drain();
        SAD_ASSERT_EQ(static_cast<int>(cmds.size()), 1);
        SAD_ASSERT_TRUE(cmds[0].type == UICommandType::SET_PROPERTY);
        SAD_ASSERT_TRUE(cmds[0].name == "content");
        SAD_ASSERT_EQ(static_cast<int>(cmds[0].targetId), 1); });
    runner.runTest("bindTransformed converts value", [&]()
                   {
        CommandQueue queue;
        BindingManager manager(queue);
        Signal<int> counter(0);
        manager.bindTransformed<int, std::string>(2, "title", counter,
            [](const int &val) -> std::string { return "Count: " + std::to_string(val); });
        queue.drain(); // drain initial sync
        counter.set(42);
        auto cmds = queue.drain();
        SAD_ASSERT_EQ(static_cast<int>(cmds.size()), 1);
        SAD_ASSERT_TRUE(cmds[0].name == "title");
        auto val = std::any_cast<std::string>(cmds[0].value);
        SAD_ASSERT_TRUE(val == "Count: 42"); });
    runner.runTest("unbind stops binding", [&]()
                   {
        CommandQueue queue;
        BindingManager manager(queue);
        Signal<int> sig(0);
        auto bindId = manager.bind(3, "size", sig);
        sig.set(10);
        queue.drain();
        manager.unbind(bindId);
        sig.set(20);
        auto cmds = queue.drain();
        SAD_ASSERT_EQ(static_cast<int>(cmds.size()), 0); });
    runner.runTest("unbindWidget removes all widget bindings", [&]()
                   {
        CommandQueue queue;
        BindingManager manager(queue);
        Signal<int> sig1(0);
        Signal<std::string> sig2("a");
        manager.bind(5, "w", sig1);
        manager.bind(5, "h", sig1);
        manager.bind(5, "name", sig2);
        SAD_ASSERT_EQ(static_cast<int>(manager.getWidgetBindingCount(5)), 3);
        size_t removed = manager.unbindWidget(5);
        SAD_ASSERT_EQ(static_cast<int>(removed), 3);
        SAD_ASSERT_EQ(static_cast<int>(manager.getBindingCount()), 0); });
    runner.runTest("unbindAll clears everything", [&]()
                   {
        CommandQueue queue;
        BindingManager manager(queue);
        Signal<int> sig(0);
        manager.bind(1, "a", sig);
        manager.bind(2, "b", sig);
        manager.bind(3, "c", sig);
        SAD_ASSERT_EQ(static_cast<int>(manager.getBindingCount()), 3);
        manager.unbindAll();
        SAD_ASSERT_EQ(static_cast<int>(manager.getBindingCount()), 0); });
}
// ١٠. UIEventLoop
void test_ui_event_loop()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("10. UIEventLoop");
    runner.runTest("tick processes commands", [&]()
                   {
        UIArena arena;
        CommandQueue queue;
        UIEventLoop loop(arena, queue);
        queue.push(UICommand::makeCreateRoot("عمود"));
        loop.tick();
        SAD_ASSERT_TRUE(arena.getRoot() != nullptr);
        SAD_ASSERT_TRUE(arena.getRoot()->getType() == "عمود"); });
    runner.runTest("tick processes multiple commands", [&]()
                   {
        UIArena arena;
        CommandQueue queue;
        UIEventLoop loop(arena, queue);
        queue.push(UICommand::makeCreateRoot("صف"));
        queue.push(UICommand::makeCreateChild(1, "نص"));
        queue.push(UICommand::makeCreateChild(1, "زر"));
        loop.tick();
        SAD_ASSERT_TRUE(arena.getRoot() != nullptr);
        SAD_ASSERT_TRUE(arena.getNodeCount() >= 1); });
    runner.runTest("frameCount starts at zero", [&]()
                   {
        UIArena arena;
        CommandQueue queue;
        UIEventLoop loop(arena, queue);
        SAD_ASSERT_EQ(static_cast<int>(loop.getFrameCount()), 0);
        loop.tick();
        // frameCount increments only in run(), not tick()
        SAD_ASSERT_EQ(static_cast<int>(loop.getFrameCount()), 0); });
    runner.runTest("stop stops loop", [&]()
                   {
        UIArena arena;
        CommandQueue queue;
        UIEventLoop loop(arena, queue);
        SAD_ASSERT_FALSE(loop.isRunning());
        loop.stop();
        SAD_ASSERT_FALSE(loop.isRunning()); });
    runner.runTest("paintCallback called", [&]()
                   {
        UIArena arena;
        CommandQueue queue;
        UIEventLoopConfig config;
        config.dirtyTracking = true;
        UIEventLoop loop(arena, queue, config);
        int paintCount = 0;
        loop.setPaintCallback([&](const WidgetNode *) { ++paintCount; });
        loop.tick();
        SAD_ASSERT_TRUE(paintCount >= 1); });
    runner.runTest("custom command handler", [&]()
                   {
        UIArena arena;
        CommandQueue queue;
        UIEventLoop loop(arena, queue);
        int customCount = 0;
        loop.setCommandHandler([&](UIArena &a, const UICommand &cmd) { ++customCount; });
        queue.push(UICommand::makeCreateRoot("test"));
        queue.push(UICommand::makeSetProperty(1, "color", std::any(0)));
        loop.tick();
        SAD_ASSERT_EQ(customCount, 2); });
    runner.runTest("CLEAR_TREE clears arena", [&]()
                   {
        UIArena arena;
        CommandQueue queue;
        UIEventLoop loop(arena, queue);
        queue.push(UICommand::makeCreateRoot("عمود"));
        loop.tick();
        SAD_ASSERT_TRUE(arena.getRoot() != nullptr);
        queue.push(UICommand::makeClearTree());
        loop.tick();
        SAD_ASSERT_TRUE(arena.getRoot() == nullptr); });
}

// ١١. Integration
void test_integration_phase_minus_one()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("11. Phase-1 Integration");
    runner.runTest("Signal -> Binding -> Queue -> Loop -> Arena", [&]()
                   {
        UIArena arena;
        CommandQueue queue;
        UIEventLoop loop(arena, queue);
        BindingManager bindings(queue);
        queue.push(UICommand::makeCreateRoot("عمود"));
        loop.tick();
        SAD_ASSERT_TRUE(arena.getRoot() != nullptr);
        WidgetId rootId = arena.getRoot()->getId();
        Signal<std::string> title("initial");
        bindings.bind(rootId, "title", title);
        title.set("updated");
        SAD_ASSERT_TRUE(queue.pendingCount() > 0);
        std::string receivedProp;
        loop.setCommandHandler([&](UIArena &a, const UICommand &cmd) {
            if (cmd.type == UICommandType::SET_PROPERTY) receivedProp = cmd.name;
        });
        loop.tick();
        SAD_ASSERT_TRUE(receivedProp == "title"); });
}

int main()
{
    SAD_ENABLE_COLORS();
    std::cout << "\n";
    std::cout << "== Comprehensive UI Widget Tests + Phase -1 Infrastructure ==\n\n";
    test_widget_inventory();
    test_widget_categories();
    test_sir_ui_opcodes();
    test_usage_scenarios();
    test_file_distribution();
    test_sadui_handle_liveness();
    test_command_queue();
    test_signal();
    test_property_binding();
    test_ui_event_loop();
    test_integration_phase_minus_one();
    return TestRunner::instance().printSummary();
}