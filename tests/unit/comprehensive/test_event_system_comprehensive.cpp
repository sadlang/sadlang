// بسم الله الرحمن الرحيم
/**
 * @file test_event_system_comprehensive.cpp
 * @brief اختبارات شاملة لنظام الأحداث الموحد (IREventType enum)
 *
 * يغطي:
 *   1. تحويل الأسماء العربية إلى IREventType (stringToIREventType)
 *   2. تحويل الأسماء الإنجليزية والمستعارة إلى IREventType
 *   3. تحويل IREventType إلى الاسم العربي (irEventTypeToString)
 *   4. التعرف على أسماء الأحداث المعروفة (isKnownEventName)
 *   5. تحويل ModifierType إلى IREventType (modifierTypeToIREventType)
 *   6. بنية IREvent والدالة getEventName()
 *   7. التحقق من اكتمال جداول التحويل (لا قيم ناقصة)
 *   8. أسماء مجهولة تُرجع Custom
 *   9. ذهاب وإياب (round-trip): enum → string → enum
 */

#include "sad_test_framework.h"

// ── رؤوس sad_ui ──
#include "../../sad_ui/core/include/sad_ui/types.h"
#include "../../sad_ui/core/include/sad_ui/ir.h"

#include <string>
#include <vector>
#include <set>

using namespace SadTest;
using namespace sad::ui;

// ═══════════════════════════════════════════════════════════════════════════════
// 1. تحويل الأسماء العربية الأساسية → IREventType
// ═══════════════════════════════════════════════════════════════════════════════

void test_arabic_names_to_event_type()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("1. \xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x84 \xd8\xa7\xd9\x84\xd8\xa3\xd8\xb3\xd9\x85\xd8\xa7\xd8\xa1 \xd8\xa7\xd9\x84\xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a\xd8\xa9 \xe2\x86\x92 IREventType");
    // 1. تحويل الأسماء العربية → IREventType

    // مصفوفة: {الاسم العربي، القيمة المتوقعة}
    struct ArabicTestCase
    {
        const char *arabicName;
        IREventType expected;
        const char *description;
    };

    // الأسماء العربية الأساسية (من getIREventTypeNames)
    std::vector<ArabicTestCase> cases = {
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x86\xd9\x82\xd8\xb1", IREventType::OnTap, "OnTap"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x86\xd9\x82\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac", IREventType::OnDoubleTap, "OnDoubleTap"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb6\xd8\xba\xd8\xb7_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb7\xd9\x88\xd9\x84", IREventType::OnLongPress, "OnLongPress"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8", IREventType::OnDrag, "OnDrag"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8_\xd9\x8a\xd8\xb3\xd8\xa7\xd8\xb1", IREventType::OnSwipeLeft, "OnSwipeLeft"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8_\xd9\x8a\xd9\x85\xd9\x8a\xd9\x86", IREventType::OnSwipeRight, "OnSwipeRight"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8_\xd8\xa3\xd8\xb9\xd9\x84\xd9\x89", IREventType::OnSwipeUp, "OnSwipeUp"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8_\xd8\xa3\xd8\xb3\xd9\x81\xd9\x84", IREventType::OnSwipeDown, "OnSwipeDown"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd9\x83\xd8\xa8\xd9\x8a\xd8\xb1", IREventType::OnZoom, "OnZoom"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1", IREventType::OnChange, "OnChange"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84", IREventType::OnInput, "OnInput"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd8\xb1", IREventType::OnRelease, "OnRelease"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", IREventType::OnScroll, "OnScroll"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x85", IREventType::OnHover, "OnHover"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb1\xd9\x88\xd8\xac", IREventType::OnHoverExit, "OnHoverExit"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2", IREventType::OnFocus, "OnFocus"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd9\x81\xd9\x82\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2", IREventType::OnBlur, "OnBlur"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb8\xd9\x87\xd9\x88\xd8\xb1", IREventType::OnAppear, "OnAppear"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa7\xd8\xae\xd8\xaa\xd9\x81\xd8\xa7\xd8\xa1", IREventType::OnDisappear, "OnDisappear"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xb6\xd8\xba\xd8\xb7_\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad", IREventType::OnKeyDown, "OnKeyDown"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xb1\xd9\x81\xd8\xb9_\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad", IREventType::OnKeyUp, "OnKeyUp"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84", IREventType::OnSubmit, "OnSubmit"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa8\xd8\xaf\xd8\xa1_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8", IREventType::OnDragStart, "OnDragStart"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x86\xd8\xaa\xd9\x87\xd8\xa7\xd8\xa1_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8", IREventType::OnDragEnd, "OnDragEnd"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb3\xd9\x82\xd8\xa7\xd8\xb7", IREventType::OnDrop, "OnDrop"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa7\xd9\x86", IREventType::OnRotate, "OnRotate"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xb3\xd9\x8a\xd8\xa7\xd9\x82\xd9\x8a\xd8\xa9", IREventType::OnContextMenu, "OnContextMenu"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xaf", IREventType::OnSelect, "OnSelect"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1_\xd8\xa7\xd9\x84\xd8\xad\xd8\xac\xd9\x85", IREventType::OnResize, "OnResize"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x86\xd8\xaa\xd9\x87\xd8\xa7\xd8\xa1_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd9\x83", IREventType::OnAnimationEnd, "OnAnimationEnd"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84", IREventType::OnLoad, "OnLoad"},
        {"\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7\xd8\xa3", IREventType::OnError, "OnError"},
    };

    for (auto &tc : cases)
    {
        runner.runTest(std::string("Arabic -> ") + tc.description, [&]()
                       {
            IREventType result = stringToIREventType(tc.arabicName);
            SAD_ASSERT_TRUE(result == tc.expected); });
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// 2. تحويل الأسماء الإنجليزية والمستعارة → IREventType
// ═══════════════════════════════════════════════════════════════════════════════

void test_english_names_to_event_type()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("2. English/Alias names \xe2\x86\x92 IREventType");
    // 2. الأسماء الإنجليزية والمستعارة → IREventType

    struct EnglishTestCase
    {
        const char *name;
        IREventType expected;
    };

    std::vector<EnglishTestCase> cases = {
        // ─── camelCase ───
        {"onTap", IREventType::OnTap},
        {"onClick", IREventType::OnTap},
        {"onDoubleTap", IREventType::OnDoubleTap},
        {"onLongPress", IREventType::OnLongPress},
        {"onDrag", IREventType::OnDrag},
        {"onSwipe", IREventType::OnDrag},
        {"onSwipeLeft", IREventType::OnSwipeLeft},
        {"onSwipeRight", IREventType::OnSwipeRight},
        {"onSwipeUp", IREventType::OnSwipeUp},
        {"onSwipeDown", IREventType::OnSwipeDown},
        {"onZoom", IREventType::OnZoom},
        {"onPinch", IREventType::OnZoom},
        {"onChange", IREventType::OnChange},
        {"onInput", IREventType::OnInput},
        {"onRelease", IREventType::OnRelease},
        {"onScroll", IREventType::OnScroll},
        {"onHover", IREventType::OnHover},
        {"onMouseEnter", IREventType::OnHover},
        {"onHoverExit", IREventType::OnHoverExit},
        {"onMouseLeave", IREventType::OnHoverExit},
        {"onFocus", IREventType::OnFocus},
        {"onBlur", IREventType::OnBlur},
        {"onAppear", IREventType::OnAppear},
        {"onDisappear", IREventType::OnDisappear},
        {"onKeyDown", IREventType::OnKeyDown},
        {"onKeyUp", IREventType::OnKeyUp},
        {"onSubmit", IREventType::OnSubmit},
        {"onDragStart", IREventType::OnDragStart},
        {"onDragEnd", IREventType::OnDragEnd},
        {"onDrop", IREventType::OnDrop},
        {"onRotate", IREventType::OnRotate},
        {"onContextMenu", IREventType::OnContextMenu},
        {"onRightClick", IREventType::OnContextMenu},
        {"onSelect", IREventType::OnSelect},
        {"onResize", IREventType::OnResize},
        {"onAnimationEnd", IREventType::OnAnimationEnd},
        {"onLoad", IREventType::OnLoad},
        {"onError", IREventType::OnError},

        // ─── snake_case ───
        {"on_tap", IREventType::OnTap},
        {"on_click", IREventType::OnTap},
        {"on_double_tap", IREventType::OnDoubleTap},
        {"on_long_press", IREventType::OnLongPress},
        {"on_drag", IREventType::OnDrag},
        {"on_zoom", IREventType::OnZoom},
        {"on_change", IREventType::OnChange},
        {"on_input", IREventType::OnInput},
        {"on_release", IREventType::OnRelease},
        {"on_scroll", IREventType::OnScroll},
        {"on_hover", IREventType::OnHover},
        {"on_hover_exit", IREventType::OnHoverExit},
        {"on_focus", IREventType::OnFocus},
        {"on_blur", IREventType::OnBlur},
        {"on_appear", IREventType::OnAppear},
        {"on_disappear", IREventType::OnDisappear},
        {"on_key_down", IREventType::OnKeyDown},
        {"on_key_up", IREventType::OnKeyUp},
        {"on_submit", IREventType::OnSubmit},
        {"on_drag_start", IREventType::OnDragStart},
        {"on_drag_end", IREventType::OnDragEnd},
        {"on_drop", IREventType::OnDrop},
        {"on_rotate", IREventType::OnRotate},
        {"on_context_menu", IREventType::OnContextMenu},
        {"on_select", IREventType::OnSelect},
        {"on_resize", IREventType::OnResize},
        {"on_animation_end", IREventType::OnAnimationEnd},
        {"on_load", IREventType::OnLoad},
        {"on_error", IREventType::OnError},
    };

    for (auto &tc : cases)
    {
        runner.runTest(std::string("\"") + tc.name + "\"", [&]()
                       {
            IREventType result = stringToIREventType(tc.name);
            SAD_ASSERT_TRUE(result == tc.expected); });
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// 3. تحويل الأسماء المستعارة العربية → IREventType
// ═══════════════════════════════════════════════════════════════════════════════

void test_arabic_aliases()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("3. Arabic aliases \xe2\x86\x92 IREventType");
    // 3. الأسماء المستعارة العربية

    runner.runTest("\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb6\xd8\xba\xd8\xb7 -> OnTap", [&]()
                   {
        // عند_الضغط → عند_النقر (OnTap)
        IREventType result = stringToIREventType("\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb6\xd8\xba\xd8\xb7");
        SAD_ASSERT_TRUE(result == IREventType::OnTap); });

    runner.runTest("\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xaa\xd8\xba\xd9\x8a\xd9\x91\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9 -> OnChange", [&]()
                   {
        // عند_تغيّر_القيمة → عند_التغيير (OnChange)
        IREventType result = stringToIREventType("\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xaa\xd8\xba\xd9\x8a\xd9\x91\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");
        SAD_ASSERT_TRUE(result == IREventType::OnChange); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 4. تحويل IREventType → الاسم العربي (irEventTypeToString)
// ═══════════════════════════════════════════════════════════════════════════════

void test_event_type_to_string()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("4. IREventType \xe2\x86\x92 Arabic string");
    // 4. IREventType → الاسم العربي

    // التأكد من أن كل نوع (باستثناء _Count) له اسم عربي غير فارغ
    runner.runTest("All types have non-empty Arabic names", [&]()
                   {
        int count = static_cast<int>(IREventType::_Count);
        for (int i = 0; i < count; ++i) {
            IREventType t = static_cast<IREventType>(i);
            const std::string& name = irEventTypeToString(t);
            SAD_ASSERT_TRUE(!name.empty());
        } });

    // التأكد من أن OnTap يُرجع "عند_النقر"
    runner.runTest("OnTap -> \xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x86\xd9\x82\xd8\xb1", [&]()
                   {
        const std::string& name = irEventTypeToString(IREventType::OnTap);
        SAD_ASSERT_EQ(name, std::string("\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x86\xd9\x82\xd8\xb1")); });

    // Custom يُرجع "مخصص"
    runner.runTest("Custom -> \xd9\x85\xd8\xae\xd8\xb5\xd8\xb5", [&]()
                   {
        const std::string& name = irEventTypeToString(IREventType::Custom);
        SAD_ASSERT_EQ(name, std::string("\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5")); });

    // عدد الأنواع المتوقع (33 قيمة: 32 حدث + Custom)
    runner.runTest("_Count == 33", [&]()
                   { SAD_ASSERT_EQ(static_cast<int>(IREventType::_Count), 33); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 5. التعرف على أسماء الأحداث المعروفة (isKnownEventName)
// ═══════════════════════════════════════════════════════════════════════════════

void test_is_known_event_name()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("5. isKnownEventName");

    // أسماء معروفة
    runner.runTest("onTap is known", [&]()
                   { SAD_ASSERT_TRUE(isKnownEventName("onTap")); });

    runner.runTest("onClick is known", [&]()
                   { SAD_ASSERT_TRUE(isKnownEventName("onClick")); });

    runner.runTest("on_drag_start is known", [&]()
                   { SAD_ASSERT_TRUE(isKnownEventName("on_drag_start")); });

    runner.runTest("\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x86\xd9\x82\xd8\xb1 is known", [&]()
                   {
        // عند_النقر
        SAD_ASSERT_TRUE(isKnownEventName("\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x86\xd9\x82\xd8\xb1")); });

    runner.runTest("onRightClick is known", [&]()
                   { SAD_ASSERT_TRUE(isKnownEventName("onRightClick")); });

    runner.runTest("onPinch is known (alias for OnZoom)", [&]()
                   { SAD_ASSERT_TRUE(isKnownEventName("onPinch")); });

    // أسماء مجهولة
    runner.runTest("\"foobar\" is NOT known", [&]()
                   { SAD_ASSERT_TRUE(!isKnownEventName("foobar")); });

    runner.runTest("\"\" (empty) is NOT known", [&]()
                   { SAD_ASSERT_TRUE(!isKnownEventName("")); });

    runner.runTest("\"onFly\" is NOT known", [&]()
                   { SAD_ASSERT_TRUE(!isKnownEventName("onFly")); });

    runner.runTest("\"click\" (no prefix) is NOT known", [&]()
                   { SAD_ASSERT_TRUE(!isKnownEventName("click")); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 6. أسماء مجهولة تُرجع Custom
// ═══════════════════════════════════════════════════════════════════════════════

void test_unknown_names_return_custom()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("6. Unknown names \xe2\x86\x92 Custom");

    runner.runTest("unknown string -> Custom", [&]()
                   { SAD_ASSERT_TRUE(stringToIREventType("onJump") == IREventType::Custom); });

    runner.runTest("empty string -> Custom", [&]()
                   { SAD_ASSERT_TRUE(stringToIREventType("") == IREventType::Custom); });

    runner.runTest("random arabic -> Custom", [&]()
                   {
                       SAD_ASSERT_TRUE(stringToIREventType("\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb7\xd9\x8a\xd8\xb1\xd8\xa7\xd9\x86") == IREventType::Custom);
                       // عند_الطيران
                   });

    runner.runTest("partial match -> Custom", [&]()
                   { SAD_ASSERT_TRUE(stringToIREventType("onTa") == IREventType::Custom); });

    runner.runTest("case sensitive: OnTap -> Custom", [&]()
                   {
        // جدول التحويل حساس لحالة الأحرف — "OnTap" (حرف O كبير) غير مسجل
        SAD_ASSERT_TRUE(stringToIREventType("OnTap") == IREventType::Custom); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 7. تحويل ModifierType → IREventType
// ═══════════════════════════════════════════════════════════════════════════════

void test_modifier_to_event_type()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("7. ModifierType \xe2\x86\x92 IREventType");

    runner.runTest("OnTap modifier", [&]()
                   { SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnTap) == IREventType::OnTap); });

    runner.runTest("OnLongPress modifier", [&]()
                   { SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnLongPress) == IREventType::OnLongPress); });

    runner.runTest("OnDrag modifier", [&]()
                   { SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnDrag) == IREventType::OnDrag); });

    runner.runTest("OnAppear modifier", [&]()
                   { SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnAppear) == IREventType::OnAppear); });

    runner.runTest("OnDisappear modifier", [&]()
                   { SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnDisappear) == IREventType::OnDisappear); });

    runner.runTest("OnValueChange modifier -> OnChange", [&]()
                   { SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::OnValueChange) == IREventType::OnChange); });

    runner.runTest("Non-event modifier -> Custom", [&]()
                   {
        // Padding ليست حدث — يجب أن تُرجع Custom
        SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::Padding) == IREventType::Custom); });

    runner.runTest("BackgroundColor modifier -> Custom", [&]()
                   { SAD_ASSERT_TRUE(modifierTypeToIREventType(ModifierType::BackgroundColor) == IREventType::Custom); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 8. بنية IREvent و getEventName()
// ═══════════════════════════════════════════════════════════════════════════════

void test_ir_event_struct()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("8. IREvent struct + getEventName()");

    runner.runTest("Default event is Custom", [&]()
                   {
        IREvent evt;
        SAD_ASSERT_TRUE(evt.type == IREventType::Custom); });

    runner.runTest("getEventName() for standard event (OnTap)", [&]()
                   {
        IREvent evt;
        evt.type = IREventType::OnTap;
        evt.expression = "handleTap()";
        const std::string& name = evt.getEventName();
        SAD_ASSERT_EQ(name, irEventTypeToString(IREventType::OnTap)); });

    runner.runTest("getEventName() for custom event", [&]()
                   {
        IREvent evt;
        evt.type = IREventType::Custom;
        evt.customEventName = "myCustomEvent";
        const std::string& name = evt.getEventName();
        SAD_ASSERT_EQ(name, std::string("myCustomEvent")); });

    runner.runTest("IREvent with modifiedStates", [&]()
                   {
        IREvent evt;
        evt.type = IREventType::OnChange;
        evt.expression = "updateValue()";
        evt.modifiedStates = {"counter", "label"};
        SAD_ASSERT_EQ(static_cast<int>(evt.modifiedStates.size()), 2);
        SAD_ASSERT_EQ(evt.modifiedStates[0], std::string("counter"));
        SAD_ASSERT_EQ(evt.modifiedStates[1], std::string("label")); });

    runner.runTest("IREvent attached to IRNode", [&]()
                   {
        auto node = IRNode::create(UINodeType::Button);
        IREvent evt;
        evt.type = IREventType::OnTap;
        evt.expression = "doSomething()";
        node->addEvent(evt);
        SAD_ASSERT_EQ(static_cast<int>(node->getEvents().size()), 1);
        SAD_ASSERT_TRUE(node->getEvents()[0].type == IREventType::OnTap); });

    runner.runTest("Multiple events on single IRNode", [&]()
                   {
        auto node = IRNode::create(UINodeType::TextField);
        IREvent e1, e2, e3;
        e1.type = IREventType::OnInput;
        e2.type = IREventType::OnChange;
        e3.type = IREventType::OnFocus;
        node->addEvent(e1);
        node->addEvent(e2);
        node->addEvent(e3);
        SAD_ASSERT_EQ(static_cast<int>(node->getEvents().size()), 3);
        SAD_ASSERT_TRUE(node->getEvents()[0].type == IREventType::OnInput);
        SAD_ASSERT_TRUE(node->getEvents()[1].type == IREventType::OnChange);
        SAD_ASSERT_TRUE(node->getEvents()[2].type == IREventType::OnFocus); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 9. ذهاب وإياب (Round-trip): enum → string → enum
// ═══════════════════════════════════════════════════════════════════════════════

void test_round_trip()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("9. Round-trip: enum \xe2\x86\x92 string \xe2\x86\x92 enum");

    runner.runTest("All standard types survive round-trip", [&]()
                   {
        int count = static_cast<int>(IREventType::_Count);
        int passed = 0;
        for (int i = 0; i < count; ++i) {
            IREventType original = static_cast<IREventType>(i);
            const std::string& name = irEventTypeToString(original);
            IREventType recovered = stringToIREventType(name);
            // Custom يُرجع "مخصص" ← stringToIREventType("مخصص") يجب أن يُرجع Custom
            if (recovered == original) {
                passed++;
            }
        }
        // يجب أن ينجح الجميع (34 قيمة شاملة Custom)
        SAD_ASSERT_EQ(passed, count); });

    runner.runTest("English round-trip: onTap -> Arabic -> onTap", [&]()
                   {
        IREventType t1 = stringToIREventType("onTap");
        const std::string& arabic = irEventTypeToString(t1);
        IREventType t2 = stringToIREventType(arabic);
        SAD_ASSERT_TRUE(t1 == t2);
        SAD_ASSERT_TRUE(t1 == IREventType::OnTap); });

    runner.runTest("Alias round-trip: onClick -> Arabic -> same enum", [&]()
                   {
        IREventType t1 = stringToIREventType("onClick");
        IREventType t2 = stringToIREventType("onTap");
        SAD_ASSERT_TRUE(t1 == t2); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 10. اكتمال جداول التحويل
// ═══════════════════════════════════════════════════════════════════════════════

void test_completeness()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("10. Table completeness");

    runner.runTest("Every enum value has a unique Arabic name", [&]()
                   {
        std::set<std::string> names;
        int count = static_cast<int>(IREventType::_Count);
        for (int i = 0; i < count; ++i) {
            IREventType t = static_cast<IREventType>(i);
            const std::string& name = irEventTypeToString(t);
            // يجب ألا يتكرر اسم
            bool inserted = names.insert(name).second;
            SAD_ASSERT_TRUE(inserted);
        }
        // عدد الأسماء الفريدة = عدد الأنواع
        SAD_ASSERT_EQ(static_cast<int>(names.size()), count); });

    runner.runTest("Every English primary alias maps correctly", [&]()
                   {
        // التأكد من أن كل نوع (باستثناء Custom و _Count) له على الأقل
        // اسم إنجليزي camelCase مسجل
        std::vector<std::pair<const char*, IREventType>> primaryEnglish = {
            {"onTap", IREventType::OnTap},
            {"onDoubleTap", IREventType::OnDoubleTap},
            {"onLongPress", IREventType::OnLongPress},
            {"onDrag", IREventType::OnDrag},
            {"onSwipeLeft", IREventType::OnSwipeLeft},
            {"onSwipeRight", IREventType::OnSwipeRight},
            {"onSwipeUp", IREventType::OnSwipeUp},
            {"onSwipeDown", IREventType::OnSwipeDown},
            {"onZoom", IREventType::OnZoom},
            {"onChange", IREventType::OnChange},
            {"onInput", IREventType::OnInput},
            {"onRelease", IREventType::OnRelease},
            {"onScroll", IREventType::OnScroll},
            {"onHover", IREventType::OnHover},
            {"onHoverExit", IREventType::OnHoverExit},
            {"onFocus", IREventType::OnFocus},
            {"onBlur", IREventType::OnBlur},
            {"onAppear", IREventType::OnAppear},
            {"onDisappear", IREventType::OnDisappear},
            {"onKeyDown", IREventType::OnKeyDown},
            {"onKeyUp", IREventType::OnKeyUp},
            {"onSubmit", IREventType::OnSubmit},
            {"onDragStart", IREventType::OnDragStart},
            {"onDragEnd", IREventType::OnDragEnd},
            {"onDrop", IREventType::OnDrop},
            {"onRotate", IREventType::OnRotate},
            {"onContextMenu", IREventType::OnContextMenu},
            {"onSelect", IREventType::OnSelect},
            {"onResize", IREventType::OnResize},
            {"onAnimationEnd", IREventType::OnAnimationEnd},
            {"onLoad", IREventType::OnLoad},
            {"onError", IREventType::OnError},
        };

        for (auto& [name, expected] : primaryEnglish) {
            IREventType result = stringToIREventType(name);
            SAD_ASSERT_TRUE(result == expected);
        } });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 11. EventPropagation enum
// ═══════════════════════════════════════════════════════════════════════════════

void test_event_propagation_enum()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("11. EventPropagation enum");

    runner.runTest("EventPropagation::None value is 0", [&]()
                   { SAD_ASSERT_EQ(static_cast<int>(EventPropagation::None), 0); });

    runner.runTest("EventPropagation::Bubble value is 1", [&]()
                   { SAD_ASSERT_EQ(static_cast<int>(EventPropagation::Bubble), 1); });

    runner.runTest("EventPropagation::Capture value is 2", [&]()
                   { SAD_ASSERT_EQ(static_cast<int>(EventPropagation::Capture), 2); });

    runner.runTest("EventPropagation::Both value is 3", [&]()
                   { SAD_ASSERT_EQ(static_cast<int>(EventPropagation::Both), 3); });

    runner.runTest("All four values are distinct", [&]()
                   {
        SAD_ASSERT_TRUE(EventPropagation::None != EventPropagation::Bubble);
        SAD_ASSERT_TRUE(EventPropagation::Bubble != EventPropagation::Capture);
        SAD_ASSERT_TRUE(EventPropagation::Capture != EventPropagation::Both);
        SAD_ASSERT_TRUE(EventPropagation::None != EventPropagation::Both); });

    runner.runTest("IREvent default propagation is None", [&]()
                   {
        IREvent evt;
        SAD_ASSERT_TRUE(evt.propagation == EventPropagation::None); });

    runner.runTest("IREvent propagation can be set to Bubble", [&]()
                   {
        IREvent evt;
        evt.propagation = EventPropagation::Bubble;
        SAD_ASSERT_TRUE(evt.propagation == EventPropagation::Bubble); });

    runner.runTest("IREvent propagation can be set to Both", [&]()
                   {
        IREvent evt;
        evt.type = IREventType::OnTap;
        evt.propagation = EventPropagation::Both;
        SAD_ASSERT_TRUE(evt.propagation == EventPropagation::Both);
        SAD_ASSERT_TRUE(evt.type == IREventType::OnTap); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 12. EventData struct
// ═══════════════════════════════════════════════════════════════════════════════

void test_event_data_struct()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("12. EventData struct");

    runner.runTest("Default values are zero/empty", [&]()
                   {
        EventData data;
        SAD_ASSERT_FLOAT_EQ(data.x, 0.0f, 0.001f);
        SAD_ASSERT_FLOAT_EQ(data.y, 0.0f, 0.001f);
        SAD_ASSERT_FLOAT_EQ(data.deltaX, 0.0f, 0.001f);
        SAD_ASSERT_FLOAT_EQ(data.deltaY, 0.0f, 0.001f);
        SAD_ASSERT_EQ(data.keyCode, 0);
        SAD_ASSERT_TRUE(data.keyName.empty());
        SAD_ASSERT_TRUE(!data.shiftKey);
        SAD_ASSERT_TRUE(!data.ctrlKey);
        SAD_ASSERT_TRUE(!data.altKey);
        SAD_ASSERT_EQ(data.button, 0);
        SAD_ASSERT_FLOAT_EQ(data.angle, 0.0f, 0.001f);
        SAD_ASSERT_TRUE(data.value.empty());
        SAD_ASSERT_TRUE(data.customData.empty());
        SAD_ASSERT_TRUE(!data.propagationStopped); });

    runner.runTest("Position fields (x, y)", [&]()
                   {
        EventData data;
        data.x = 150.5f;
        data.y = 320.0f;
        SAD_ASSERT_FLOAT_EQ(data.x, 150.5f, 0.001f);
        SAD_ASSERT_FLOAT_EQ(data.y, 320.0f, 0.001f); });

    runner.runTest("Delta fields (deltaX, deltaY)", [&]()
                   {
        EventData data;
        data.deltaX = -10.0f;
        data.deltaY = 25.5f;
        SAD_ASSERT_FLOAT_EQ(data.deltaX, -10.0f, 0.001f);
        SAD_ASSERT_FLOAT_EQ(data.deltaY, 25.5f, 0.001f); });

    runner.runTest("Keyboard fields (keyCode, keyName)", [&]()
                   {
        EventData data;
        data.keyCode = 13;
        data.keyName = "Enter";
        SAD_ASSERT_EQ(data.keyCode, 13);
        SAD_ASSERT_EQ(data.keyName, std::string("Enter")); });

    runner.runTest("Modifier keys (shift, ctrl, alt)", [&]()
                   {
        EventData data;
        data.shiftKey = true;
        data.ctrlKey = true;
        data.altKey = false;
        SAD_ASSERT_TRUE(data.shiftKey);
        SAD_ASSERT_TRUE(data.ctrlKey);
        SAD_ASSERT_TRUE(!data.altKey); });

    runner.runTest("Mouse button field", [&]()
                   {
        EventData data;
        data.button = 2;
        SAD_ASSERT_EQ(data.button, 2); });

    runner.runTest("Angle field (rotation)", [&]()
                   {
        EventData data;
        data.angle = 45.0f;
        SAD_ASSERT_FLOAT_EQ(data.angle, 45.0f, 0.001f); });

    runner.runTest("Value field (input/slider)", [&]()
                   {
        EventData data;
        data.value = "Hello World";
        SAD_ASSERT_EQ(data.value, std::string("Hello World")); });

    runner.runTest("Custom data payload", [&]()
                   {
        EventData data;
        data.customData = "{\"action\": \"save\"}";
        SAD_ASSERT_EQ(data.customData, std::string("{\"action\": \"save\"}")); });

    runner.runTest("stopPropagation() stops propagation", [&]()
                   {
        EventData data;
        SAD_ASSERT_TRUE(!data.propagationStopped);
        data.stopPropagation();
        SAD_ASSERT_TRUE(data.propagationStopped); });

    runner.runTest("stopPropagation() works on const EventData", [&]()
                   {
        const EventData data;
        SAD_ASSERT_TRUE(!data.propagationStopped);
        data.stopPropagation();
        SAD_ASSERT_TRUE(data.propagationStopped); });

    runner.runTest("Full mouse click event data", [&]()
                   {
        EventData data;
        data.x = 200.0f;
        data.y = 300.0f;
        data.button = 0;
        data.shiftKey = true;
        SAD_ASSERT_FLOAT_EQ(data.x, 200.0f, 0.001f);
        SAD_ASSERT_FLOAT_EQ(data.y, 300.0f, 0.001f);
        SAD_ASSERT_EQ(data.button, 0);
        SAD_ASSERT_TRUE(data.shiftKey); });

    runner.runTest("Full keyboard event data", [&]()
                   {
        EventData data;
        data.keyCode = 32;
        data.keyName = "Space";
        data.ctrlKey = true;
        data.altKey = true;
        SAD_ASSERT_EQ(data.keyCode, 32);
        SAD_ASSERT_EQ(data.keyName, std::string("Space"));
        SAD_ASSERT_TRUE(data.ctrlKey);
        SAD_ASSERT_TRUE(data.altKey); });

    runner.runTest("Full drag event data", [&]()
                   {
        EventData data;
        data.x = 100.0f;
        data.y = 200.0f;
        data.deltaX = 50.0f;
        data.deltaY = -30.0f;
        SAD_ASSERT_FLOAT_EQ(data.deltaX, 50.0f, 0.001f);
        SAD_ASSERT_FLOAT_EQ(data.deltaY, -30.0f, 0.001f); });

    runner.runTest("Copy semantics", [&]()
                   {
        EventData original;
        original.x = 10.0f;
        original.y = 20.0f;
        original.keyName = "A";
        original.value = "test";
        EventData copy = original;
        SAD_ASSERT_FLOAT_EQ(copy.x, 10.0f, 0.001f);
        SAD_ASSERT_FLOAT_EQ(copy.y, 20.0f, 0.001f);
        SAD_ASSERT_EQ(copy.keyName, std::string("A"));
        SAD_ASSERT_EQ(copy.value, std::string("test"));
        copy.x = 99.0f;
        SAD_ASSERT_FLOAT_EQ(original.x, 10.0f, 0.001f); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 13. IRNode parent tracking
// ═══════════════════════════════════════════════════════════════════════════════

void test_irnode_parent_tracking()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("13. IRNode parent tracking");

    runner.runTest("New node has no parent", [&]()
                   {
        auto node = IRNode::create(UINodeType::Button);
        SAD_ASSERT_TRUE(!node->hasParent());
        SAD_ASSERT_TRUE(node->getParent() == nullptr); });

    runner.runTest("addChild sets parent", [&]()
                   {
        auto parent = IRNode::create(UINodeType::Column);
        auto child = IRNode::create(UINodeType::Button);
        parent->addChild(child);
        SAD_ASSERT_TRUE(child->hasParent());
        SAD_ASSERT_TRUE(child->getParent() == parent); });

    runner.runTest("Parent has correct child count", [&]()
                   {
        auto parent = IRNode::create(UINodeType::Row);
        auto c1 = IRNode::create(UINodeType::Text);
        auto c2 = IRNode::create(UINodeType::Button);
        parent->addChild(c1);
        parent->addChild(c2);
        SAD_ASSERT_EQ(static_cast<int>(parent->childCount()), 2); });

    runner.runTest("Root node ancestor path is empty", [&]()
                   {
        auto root = IRNode::create(UINodeType::Column);
        auto path = root->getAncestorPath();
        SAD_ASSERT_EQ(static_cast<int>(path.size()), 0); });

    runner.runTest("Child ancestor path has one element (parent)", [&]()
                   {
        auto parent = IRNode::create(UINodeType::Column);
        auto child = IRNode::create(UINodeType::Button);
        parent->addChild(child);
        auto path = child->getAncestorPath();
        SAD_ASSERT_EQ(static_cast<int>(path.size()), 1);
        SAD_ASSERT_TRUE(path[0] == parent.get()); });

    runner.runTest("Deep tree ancestor path (3 levels)", [&]()
                   {
        auto root = IRNode::create(UINodeType::Column);
        root->setId("root");
        auto mid = IRNode::create(UINodeType::Row);
        mid->setId("mid");
        auto leaf = IRNode::create(UINodeType::Text);
        leaf->setId("leaf");
        root->addChild(mid);
        mid->addChild(leaf);

        auto path = leaf->getAncestorPath();
        SAD_ASSERT_EQ(static_cast<int>(path.size()), 2);
        SAD_ASSERT_TRUE(path[0] == mid.get());
        SAD_ASSERT_TRUE(path[1] == root.get()); });

    runner.runTest("Deep tree ancestor path (4 levels)", [&]()
                   {
        auto n1 = IRNode::create(UINodeType::Column);
        auto n2 = IRNode::create(UINodeType::Row);
        auto n3 = IRNode::create(UINodeType::Column);
        auto n4 = IRNode::create(UINodeType::Button);
        n1->addChild(n2);
        n2->addChild(n3);
        n3->addChild(n4);

        auto path = n4->getAncestorPath();
        SAD_ASSERT_EQ(static_cast<int>(path.size()), 3);
        SAD_ASSERT_TRUE(path[0] == n3.get());
        SAD_ASSERT_TRUE(path[1] == n2.get());
        SAD_ASSERT_TRUE(path[2] == n1.get()); });

    runner.runTest("Multiple children share same parent", [&]()
                   {
        auto parent = IRNode::create(UINodeType::Row);
        auto c1 = IRNode::create(UINodeType::Text);
        auto c2 = IRNode::create(UINodeType::Text);
        auto c3 = IRNode::create(UINodeType::Button);
        parent->addChild(c1);
        parent->addChild(c2);
        parent->addChild(c3);
        SAD_ASSERT_TRUE(c1->getParent() == parent);
        SAD_ASSERT_TRUE(c2->getParent() == parent);
        SAD_ASSERT_TRUE(c3->getParent() == parent); });

    runner.runTest("Events on nodes in parent tree", [&]()
                   {
        auto parent = IRNode::create(UINodeType::Column);
        auto child = IRNode::create(UINodeType::Button);
        parent->addChild(child);

        IREvent parentEvt;
        parentEvt.type = IREventType::OnTap;
        parentEvt.propagation = EventPropagation::Bubble;
        parent->addEvent(parentEvt);

        IREvent childEvt;
        childEvt.type = IREventType::OnTap;
        childEvt.propagation = EventPropagation::Bubble;
        child->addEvent(childEvt);

        SAD_ASSERT_EQ(static_cast<int>(parent->getEvents().size()), 1);
        SAD_ASSERT_EQ(static_cast<int>(child->getEvents().size()), 1);
        SAD_ASSERT_TRUE(child->hasParent());

        auto ancestors = child->getAncestorPath();
        bool parentHasOnTap = false;
        for (auto* ancestor : ancestors) {
            for (auto& evt : ancestor->getEvents()) {
                if (evt.type == IREventType::OnTap) {
                    parentHasOnTap = true;
                }
            }
        }
        SAD_ASSERT_TRUE(parentHasOnTap); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// 14. EventData + IREvent integration
// ═══════════════════════════════════════════════════════════════════════════════

void test_event_data_integration()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("14. EventData + IREvent integration");

    runner.runTest("IREvent with Bubble propagation on IRNode", [&]()
                   {
        auto node = IRNode::create(UINodeType::Button);
        IREvent evt;
        evt.type = IREventType::OnTap;
        evt.propagation = EventPropagation::Bubble;
        evt.expression = "handleTap()";
        node->addEvent(evt);
        SAD_ASSERT_TRUE(node->getEvents()[0].propagation == EventPropagation::Bubble); });

    runner.runTest("Multiple events with different propagation modes", [&]()
                   {
        auto node = IRNode::create(UINodeType::TextField);
        IREvent e1;
        e1.type = IREventType::OnInput;
        e1.propagation = EventPropagation::None;
        IREvent e2;
        e2.type = IREventType::OnChange;
        e2.propagation = EventPropagation::Bubble;
        IREvent e3;
        e3.type = IREventType::OnFocus;
        e3.propagation = EventPropagation::Capture;
        node->addEvent(e1);
        node->addEvent(e2);
        node->addEvent(e3);
        SAD_ASSERT_TRUE(node->getEvents()[0].propagation == EventPropagation::None);
        SAD_ASSERT_TRUE(node->getEvents()[1].propagation == EventPropagation::Bubble);
        SAD_ASSERT_TRUE(node->getEvents()[2].propagation == EventPropagation::Capture); });

    runner.runTest("Custom event with custom data", [&]()
                   {
        IREvent evt;
        evt.type = IREventType::Custom;
        evt.customEventName = "onSave";
        evt.propagation = EventPropagation::Bubble;
        SAD_ASSERT_EQ(evt.getEventName(), std::string("onSave"));
        SAD_ASSERT_TRUE(evt.propagation == EventPropagation::Bubble); });

    runner.runTest("Simulated bubbling path with event matching", [&]()
                   {
        auto root = IRNode::create(UINodeType::Column);
        auto container = IRNode::create(UINodeType::Row);
        auto button = IRNode::create(UINodeType::Button);
        root->addChild(container);
        container->addChild(button);

        IREvent rootEvt;
        rootEvt.type = IREventType::OnTap;
        rootEvt.expression = "rootTap()";
        root->addEvent(rootEvt);

        IREvent containerEvt;
        containerEvt.type = IREventType::OnTap;
        containerEvt.expression = "containerTap()";
        container->addEvent(containerEvt);

        auto ancestors = button->getAncestorPath();
        SAD_ASSERT_EQ(static_cast<int>(ancestors.size()), 2);

        int handlersFound = 0;
        for (auto* ancestor : ancestors) {
            for (auto& evt : ancestor->getEvents()) {
                if (evt.type == IREventType::OnTap) {
                    handlersFound++;
                }
            }
        }
        SAD_ASSERT_EQ(handlersFound, 2); });

    runner.runTest("Simulated stopPropagation during bubbling", [&]()
                   {
        auto root = IRNode::create(UINodeType::Column);
        auto mid = IRNode::create(UINodeType::Row);
        auto leaf = IRNode::create(UINodeType::Button);
        root->addChild(mid);
        mid->addChild(leaf);

        IREvent rootEvt;
        rootEvt.type = IREventType::OnTap;
        root->addEvent(rootEvt);

        IREvent midEvt;
        midEvt.type = IREventType::OnTap;
        mid->addEvent(midEvt);

        auto ancestors = leaf->getAncestorPath();
        EventData data;
        data.x = 100.0f;
        data.y = 200.0f;
        int handlersInvoked = 0;
        for (auto* ancestor : ancestors) {
            if (data.propagationStopped) break;
            for (auto& evt : ancestor->getEvents()) {
                if (evt.type == IREventType::OnTap) {
                    handlersInvoked++;
                    if (ancestor == mid.get()) {
                        data.stopPropagation();
                    }
                }
            }
        }
        SAD_ASSERT_EQ(handlersInvoked, 1);
        SAD_ASSERT_TRUE(data.propagationStopped); });
}

// ═══════════════════════════════════════════════════════════════════════════════
// main
// ═══════════════════════════════════════════════════════════════════════════════

int main()
{
    SAD_ENABLE_COLORS();

    std::cout << "\n"
              << C_BOLD << C_MAGENTA
              << "\xE2\x95\x94\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x97\n"
              << "\xE2\x95\x91  " << "\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa \xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xa3\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab \xd8\xa7\xd9\x84\xd9\x85\xd9\x88\xd8\xad\xd8\xaf (IREventType)" << "  \xE2\x95\x91\n"
              << "\xE2\x95\x9A\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x9D"
              << C_RESET << "\n";

    test_arabic_names_to_event_type();
    test_english_names_to_event_type();
    test_arabic_aliases();
    test_event_type_to_string();
    test_is_known_event_name();
    test_unknown_names_return_custom();
    test_modifier_to_event_type();
    test_ir_event_struct();
    test_round_trip();
    test_completeness();
    test_event_propagation_enum();
    test_event_data_struct();
    test_irnode_parent_tracking();
    test_event_data_integration();

    return TestRunner::instance().printSummary();
}
