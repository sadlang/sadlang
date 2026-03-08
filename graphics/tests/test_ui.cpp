// ==============================================================================
// test_ui.cpp - اختبارات واجهة المستخدم / UI Widget Tests
// ==============================================================================
// ملاحظة: هذه الاختبارات لا تحتاج GPU لأنها تختبر المنطق الداخلي
// Note: No GPU needed - tests internal logic only
// ==============================================================================
#include "catch.hpp"
#include "ui/widget.h"
#include "ui/label.h"
#include "ui/button.h"

using namespace sad::graphics::ui;

// ============================================================================
// اختبارات Rect / Rect Tests
// ============================================================================

TEST_CASE("UI Rect default constructor", "[ui][rect]") {
    Rect r;
    REQUIRE(r.x == 0.0f);
    REQUIRE(r.y == 0.0f);
    REQUIRE(r.width == 0.0f);
    REQUIRE(r.height == 0.0f);
}

TEST_CASE("UI Rect parameterized constructor", "[ui][rect]") {
    Rect r(10.0f, 20.0f, 100.0f, 50.0f);
    REQUIRE(r.x == Approx(10.0f));
    REQUIRE(r.y == Approx(20.0f));
    REQUIRE(r.width == Approx(100.0f));
    REQUIRE(r.height == Approx(50.0f));
}

TEST_CASE("UI Rect Contains - point inside", "[ui][rect]") {
    Rect r(10.0f, 10.0f, 100.0f, 100.0f);
    REQUIRE(r.Contains(50.0f, 50.0f) == true);
    REQUIRE(r.Contains(10.0f, 10.0f) == true);   // top-left corner
    REQUIRE(r.Contains(110.0f, 110.0f) == true);  // bottom-right corner
}

TEST_CASE("UI Rect Contains - point outside", "[ui][rect]") {
    Rect r(10.0f, 10.0f, 100.0f, 100.0f);
    REQUIRE(r.Contains(5.0f, 50.0f) == false);    // left of rect
    REQUIRE(r.Contains(120.0f, 50.0f) == false);   // right of rect
    REQUIRE(r.Contains(50.0f, 5.0f) == false);     // above rect
    REQUIRE(r.Contains(50.0f, 120.0f) == false);    // below rect
}

TEST_CASE("UI Rect GetCenter", "[ui][rect]") {
    Rect r(10.0f, 20.0f, 100.0f, 50.0f);
    float cx, cy;
    r.GetCenter(cx, cy);
    REQUIRE(cx == Approx(60.0f));   // 10 + 100/2
    REQUIRE(cy == Approx(45.0f));   // 20 + 50/2
}

TEST_CASE("UI Rect GetCenter as Point2D", "[ui][rect]") {
    Rect r(0.0f, 0.0f, 200.0f, 100.0f);
    auto center = r.GetCenter();
    REQUIRE(center.x == Approx(100.0f));
    REQUIRE(center.y == Approx(50.0f));
}

// ============================================================================
// اختبارات WidgetStyle / WidgetStyle Tests
// ============================================================================

TEST_CASE("WidgetStyle defaults", "[ui][style]") {
    WidgetStyle style;
    REQUIRE(style.borderWidth == Approx(1.0f));
    REQUIRE(style.borderRadius == Approx(0.0f));
    REQUIRE(style.padding == Approx(5.0f));
    REQUIRE(style.opacity == Approx(1.0f));
    REQUIRE(style.backgroundColor.r == Approx(1.0f));  // White
    REQUIRE(style.foregroundColor.r == Approx(0.0f));   // Black
}

// ============================================================================
// اختبارات Widget / Widget Tests
// ============================================================================

TEST_CASE("Widget default state", "[ui][widget]") {
    Widget w;
    REQUIRE(w.IsVisible() == true);
    REQUIRE(w.IsEnabled() == true);
    REQUIRE(w.IsFocused() == false);
    REQUIRE(w.GetState() == WidgetState::Normal);
    REQUIRE(w.GetParent() == nullptr);
    REQUIRE(w.GetChildren().empty());
}

TEST_CASE("Widget set position and size", "[ui][widget]") {
    Widget w;
    w.SetPosition(50.0f, 75.0f);
    w.SetSize(200.0f, 100.0f);
    REQUIRE(w.GetX() == Approx(50.0f));
    REQUIRE(w.GetY() == Approx(75.0f));
    REQUIRE(w.GetWidth() == Approx(200.0f));
    REQUIRE(w.GetHeight() == Approx(100.0f));
}

TEST_CASE("Widget set bounds", "[ui][widget]") {
    Widget w;
    Rect bounds(10.0f, 20.0f, 300.0f, 150.0f);
    w.SetBounds(bounds);
    REQUIRE(w.GetBounds().x == Approx(10.0f));
    REQUIRE(w.GetBounds().y == Approx(20.0f));
    REQUIRE(w.GetBounds().width == Approx(300.0f));
    REQUIRE(w.GetBounds().height == Approx(150.0f));
}

TEST_CASE("Widget visibility", "[ui][widget]") {
    Widget w;
    REQUIRE(w.IsVisible() == true);
    w.SetVisible(false);
    REQUIRE(w.IsVisible() == false);
    w.SetVisible(true);
    REQUIRE(w.IsVisible() == true);
}

TEST_CASE("Widget enabled/disabled", "[ui][widget]") {
    Widget w;
    REQUIRE(w.IsEnabled() == true);
    w.SetEnabled(false);
    REQUIRE(w.IsEnabled() == false);
    REQUIRE(w.GetState() == WidgetState::Disabled);
    w.SetEnabled(true);
    REQUIRE(w.IsEnabled() == true);
}

TEST_CASE("Widget focus", "[ui][widget]") {
    Widget w;
    REQUIRE(w.IsFocused() == false);
    w.SetFocused(true);
    REQUIRE(w.IsFocused() == true);
    REQUIRE(w.GetState() == WidgetState::Focused);
    w.SetFocused(false);
    REQUIRE(w.IsFocused() == false);
}

TEST_CASE("Widget name", "[ui][widget]") {
    Widget w;
    w.SetName("TestWidget");
    REQUIRE(w.GetName() == "TestWidget");
}

TEST_CASE("Widget style", "[ui][widget]") {
    Widget w;
    WidgetStyle style;
    style.backgroundColor = Color::Red;
    style.borderWidth = 3.0f;
    w.SetStyle(style);
    REQUIRE(w.GetStyle().backgroundColor.r == Approx(1.0f));
    REQUIRE(w.GetStyle().borderWidth == Approx(3.0f));
}

TEST_CASE("Widget contains point", "[ui][widget]") {
    Widget w;
    w.SetPosition(10.0f, 10.0f);
    w.SetSize(100.0f, 100.0f);
    REQUIRE(w.ContainsPoint(50.0f, 50.0f) == true);
    REQUIRE(w.ContainsPoint(5.0f, 5.0f) == false);
}

TEST_CASE("Widget hierarchy - add child", "[ui][widget]") {
    Widget parent;
    auto child = std::make_shared<Widget>();
    parent.AddChild(child);
    REQUIRE(parent.GetChildren().size() == 1);
    REQUIRE(child->GetParent() == &parent);
}

TEST_CASE("Widget hierarchy - remove child", "[ui][widget]") {
    Widget parent;
    auto child = std::make_shared<Widget>();
    parent.AddChild(child);
    REQUIRE(parent.GetChildren().size() == 1);
    parent.RemoveChild(child);
    REQUIRE(parent.GetChildren().empty());
}

TEST_CASE("Widget hierarchy - clear children", "[ui][widget]") {
    Widget parent;
    parent.AddChild(std::make_shared<Widget>());
    parent.AddChild(std::make_shared<Widget>());
    parent.AddChild(std::make_shared<Widget>());
    REQUIRE(parent.GetChildren().size() == 3);
    parent.ClearChildren();
    REQUIRE(parent.GetChildren().empty());
}

TEST_CASE("Widget unique IDs", "[ui][widget]") {
    Widget w1;
    Widget w2;
    Widget w3;
    // كل عنصر يجب أن يكون له معرف فريد / Each widget should have unique ID
    REQUIRE(w1.GetID() != w2.GetID());
    REQUIRE(w2.GetID() != w3.GetID());
}

// ============================================================================
// اختبارات Label / Label Tests
// ============================================================================

TEST_CASE("Label default constructor", "[ui][label]") {
    Label label;
    REQUIRE(label.GetText().empty());
    REQUIRE(label.GetAlignment() == TextAlignment::MiddleCenter);
}

TEST_CASE("Label constructor with text", "[ui][label]") {
    Label label("Hello World");
    REQUIRE(label.GetText() == "Hello World");
}

TEST_CASE("Label set/get text", "[ui][label]") {
    Label label;
    label.SetText("Test");
    REQUIRE(label.GetText() == "Test");
    label.SetText("Changed");
    REQUIRE(label.GetText() == "Changed");
}

TEST_CASE("Label text color", "[ui][label]") {
    Label label("Test");
    label.SetTextColor(Color::Red);
    REQUIRE(label.GetTextColor().r == Approx(1.0f));
    REQUIRE(label.GetTextColor().g == Approx(0.0f));
    REQUIRE(label.GetTextColor().b == Approx(0.0f));
}

TEST_CASE("Label alignment", "[ui][label]") {
    Label label;
    label.SetAlignment(TextAlignment::Center);
    REQUIRE(label.GetAlignment() == TextAlignment::Center);
    label.SetAlignment(TextAlignment::Right);
    REQUIRE(label.GetAlignment() == TextAlignment::Right);
}

TEST_CASE("Label word wrap", "[ui][label]") {
    Label label;
    label.SetWordWrap(true);
    REQUIRE(label.GetWordWrap() == true);
    label.SetWordWrap(false);
    REQUIRE(label.GetWordWrap() == false);
}

TEST_CASE("Label auto size", "[ui][label]") {
    Label label;
    label.SetAutoSize(true);
    REQUIRE(label.GetAutoSize() == true);
}

TEST_CASE("Label is a Widget", "[ui][label]") {
    Label label("Test");
    label.SetPosition(10.0f, 20.0f);
    label.SetSize(100.0f, 30.0f);
    REQUIRE(label.GetX() == Approx(10.0f));
    REQUIRE(label.GetWidth() == Approx(100.0f));
    REQUIRE(label.IsVisible() == true);
}

// ============================================================================
// اختبارات Button / Button Tests
// ============================================================================

TEST_CASE("Button default constructor", "[ui][button]") {
    Button btn;
    REQUIRE(btn.GetText().empty());
    REQUIRE(btn.IsEnabled() == true);
}

TEST_CASE("Button constructor with text", "[ui][button]") {
    Button btn("Click Me");
    REQUIRE(btn.GetText() == "Click Me");
}

TEST_CASE("Button click callback", "[ui][button]") {
    Button btn("Test");
    btn.SetPosition(0.0f, 0.0f);
    btn.SetSize(100.0f, 50.0f);
    
    bool clicked = false;
    btn.SetOnClick([&clicked]() { clicked = true; });
    
    // محاكاة النقر / Simulate click
    btn.OnMouseDown(50, 25, 1);   // ضغط داخل الزر / Press inside button
    btn.OnMouseUp(50, 25, 1);     // رفع داخل الزر / Release inside button
    
    REQUIRE(clicked == true);
}

TEST_CASE("Button click outside does not trigger", "[ui][button]") {
    Button btn("Test");
    btn.SetPosition(0.0f, 0.0f);
    btn.SetSize(100.0f, 50.0f);
    
    bool clicked = false;
    btn.SetOnClick([&clicked]() { clicked = true; });
    
    // نقر خارج الزر / Click outside button
    btn.OnMouseDown(200, 200, 1);
    btn.OnMouseUp(200, 200, 1);
    
    REQUIRE(clicked == false);
}

TEST_CASE("Button hover state", "[ui][button]") {
    Button btn("Test");
    btn.SetPosition(0.0f, 0.0f);
    btn.SetSize(100.0f, 50.0f);
    
    // تحويم فوق الزر / Hover over button
    btn.OnMouseMove(50, 25);
    REQUIRE(btn.GetState() == WidgetState::Hovered);
    
    // خروج من الزر / Leave button
    btn.OnMouseMove(200, 200);
    REQUIRE(btn.GetState() == WidgetState::Normal);
}

TEST_CASE("Button disabled does not respond to clicks", "[ui][button]") {
    Button btn("Test");
    btn.SetPosition(0.0f, 0.0f);
    btn.SetSize(100.0f, 50.0f);
    btn.SetEnabled(false);
    
    bool clicked = false;
    btn.SetOnClick([&clicked]() { clicked = true; });
    
    btn.OnMouseDown(50, 25, 1);
    btn.OnMouseUp(50, 25, 1);
    
    REQUIRE(clicked == false);
}

TEST_CASE("Button is a Label is a Widget", "[ui][button]") {
    Button btn("Test");
    // يمكن استخدام دوال Label / Can use Label functions
    btn.SetTextColor(Color::Blue);
    REQUIRE(btn.GetTextColor().b == Approx(1.0f));
    // ويمكن استخدام دوال Widget / And Widget functions
    btn.SetName("MyButton");
    REQUIRE(btn.GetName() == "MyButton");
}

TEST_CASE("Button styles per state", "[ui][button]") {
    Button btn("Test");
    
    WidgetStyle normalStyle;
    normalStyle.backgroundColor = Color::White;
    btn.SetNormalStyle(normalStyle);
    
    WidgetStyle hoverStyle;
    hoverStyle.backgroundColor = Color::LightGray;
    btn.SetHoverStyle(hoverStyle);
    
    WidgetStyle pressedStyle;
    pressedStyle.backgroundColor = Color::DarkGray;
    btn.SetPressedStyle(pressedStyle);
    
    // لا نتحقق من التطبيق لكن نتأكد أنها لا تسبب خطأ / No crash check
    REQUIRE(true);
}
