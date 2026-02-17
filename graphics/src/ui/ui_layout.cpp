// ==============================================================================
// ui_layout.cpp - تنفيذ نظام تخطيط واجهة المستخدم / UI Layout Implementation
// ==============================================================================

#include "../../include/ui/ui_layout.h"
#include "../../include/rendering/renderer2d.h"
#include <algorithm>
#include <cmath>

namespace sad {
namespace graphics {
namespace ui {

// ==============================================================================
// Global UI Renderer
// ==============================================================================

static Renderer2D* s_uiRenderer = nullptr;

void SetUIRenderer(Renderer2D* renderer) { s_uiRenderer = renderer; }
Renderer2D* GetUIRenderer() { return s_uiRenderer; }

// ==============================================================================
// Element - تنفيذ العنصر الأساسي / Base Element Implementation
// ==============================================================================

ElementId Element::s_nextId = 1;

Element::Element() : m_id(s_nextId++) {}

void Element::AddChild(std::shared_ptr<Element> child) {
    if (!child) return;
    
    // إزالة من الأب السابق / Remove from previous parent
    if (child->m_parent) {
        child->m_parent->RemoveChild(child.get());
    }
    
    child->m_parent = this;
    m_children.push_back(child);
    MarkDirty();
}

void Element::RemoveChild(Element* child) {
    if (!child) return;
    
    auto it = std::find_if(m_children.begin(), m_children.end(),
        [child](const std::shared_ptr<Element>& e) { return e.get() == child; });
    
    if (it != m_children.end()) {
        (*it)->m_parent = nullptr;
        m_children.erase(it);
        MarkDirty();
    }
}

void Element::RemoveChild(Int32 index) {
    if (index < 0 || index >= static_cast<Int32>(m_children.size())) return;
    
    m_children[index]->m_parent = nullptr;
    m_children.erase(m_children.begin() + index);
    MarkDirty();
}

void Element::ClearChildren() {
    for (auto& child : m_children) {
        child->m_parent = nullptr;
    }
    m_children.clear();
    MarkDirty();
}

Element* Element::FindById(ElementId id) {
    if (m_id == id) return this;
    
    for (auto& child : m_children) {
        Element* found = child->FindById(id);
        if (found) return found;
    }
    
    return nullptr;
}

Element* Element::FindByName(const std::string& name) {
    if (m_name == name) return this;
    
    for (auto& child : m_children) {
        Element* found = child->FindByName(name);
        if (found) return found;
    }
    
    return nullptr;
}

std::vector<Element*> Element::FindByTag(const std::string& tag) {
    std::vector<Element*> results;
    
    if (m_tag == tag) {
        results.push_back(this);
    }
    
    for (auto& child : m_children) {
        auto childResults = child->FindByTag(tag);
        results.insert(results.end(), childResults.begin(), childResults.end());
    }
    
    return results;
}

LayoutRect Element::GetContentBounds() const {
    LayoutRect content = m_bounds;
    content.x += m_style.padding.left.value;
    content.y += m_style.padding.top.value;
    content.width -= m_style.padding.left.value + m_style.padding.right.value;
    content.height -= m_style.padding.top.value + m_style.padding.bottom.value;
    if (content.width < 0) content.width = 0;
    if (content.height < 0) content.height = 0;
    return content;
}

void Element::UpdateLayout() {
    // سيتم تحديث التخطيط من محرك التخطيط
    m_dirty = false;
}

void Element::MarkDirty() {
    m_dirty = true;
    if (m_parent) {
        m_parent->MarkDirty();
    }
}

void Element::Draw() {
    if (!m_style.visible) return;
    
    auto* r = GetUIRenderer();
    
    // Draw background
    if (m_style.backgroundColor.a > 0 && r) {
        r->DrawFilledRect(m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height,
                         m_style.backgroundColor);
    }
    
    // Draw border
    if (m_style.borderWidth > 0 && m_style.borderColor.a > 0 && r) {
        r->DrawRect(m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height,
                   m_style.borderColor, false);
    }
    
    // Draw children
    DrawChildren();
}

void Element::DrawChildren() {
    // ترتيب حسب zIndex / Sort by zIndex
    std::vector<Element*> sortedChildren;
    for (auto& child : m_children) {
        sortedChildren.push_back(child.get());
    }
    std::sort(sortedChildren.begin(), sortedChildren.end(),
        [](Element* a, Element* b) { return a->m_style.zIndex < b->m_style.zIndex; });
    
    for (Element* child : sortedChildren) {
        child->Draw();
    }
}

bool Element::HandleMouseDown(Float32 x, Float32 y) {
    if (!m_style.visible || !m_style.enabled) return false;
    
    // فحص الأبناء أولاً (من الأعلى للأسفل) / Check children first (top to bottom)
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->HandleMouseDown(x, y)) {
            return true;
        }
    }
    
    // فحص هذا العنصر / Check this element
    if (m_bounds.Contains(x, y)) {
        m_pressed = true;
        if (OnMouseDown) {
            OnMouseDown(this, x, y);
        }
        return true;
    }
    
    return false;
}

bool Element::HandleMouseUp(Float32 x, Float32 y) {
    if (!m_style.visible) return false;
    
    bool wasPressed = m_pressed;
    m_pressed = false;
    
    // فحص الأبناء / Check children
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->HandleMouseUp(x, y)) {
            return true;
        }
    }
    
    if (m_bounds.Contains(x, y)) {
        if (OnMouseUp) {
            OnMouseUp(this, x, y);
        }
        
        // كشف النقر / Detect click
        if (wasPressed && m_style.enabled) {
            return HandleClick(x, y);
        }
        return true;
    }
    
    return false;
}

bool Element::HandleMouseMove(Float32 x, Float32 y) {
    if (!m_style.visible) return false;
    
    bool wasHovered = m_hovered;
    m_hovered = m_bounds.Contains(x, y);
    
    // أحداث الدخول والخروج / Enter/leave events
    if (m_hovered && !wasHovered) {
        if (OnMouseEnter) {
            OnMouseEnter(this, x, y);
        }
    } else if (!m_hovered && wasHovered) {
        if (OnMouseLeave) {
            OnMouseLeave(this, x, y);
        }
    }
    
    if (m_hovered && OnMouseMove) {
        OnMouseMove(this, x, y);
    }
    
    // فحص الأبناء / Check children
    for (auto& child : m_children) {
        child->HandleMouseMove(x, y);
    }
    
    return m_hovered;
}

bool Element::HandleClick(Float32 x, Float32 y) {
    if (!m_style.visible || !m_style.enabled) return false;
    
    if (m_bounds.Contains(x, y)) {
        if (OnClick) {
            OnClick(this, x, y);
            return true;
        }
    }
    
    return false;
}

void Element::SetFocused(bool focused) {
    if (m_focused == focused) return;
    
    m_focused = focused;
    if (focused) {
        if (OnFocus) OnFocus(this);
    } else {
        if (OnBlur) OnBlur(this);
    }
}

// ==============================================================================
// FlexContainer - حاوية Flex / Flex Container Implementation
// ==============================================================================

FlexContainer::FlexContainer(FlexDirection direction) {
    m_style.flexDirection = direction;
}

// ==============================================================================
// ScrollView - عرض قابل للتمرير / Scrollable View
// ==============================================================================

ScrollView::ScrollView() {
    // تعيين خصائص افتراضية / Set default properties
}

void ScrollView::SetScrollOffset(Vec2 offset) {
    m_scrollOffset = offset;
    // TODO: تحديد الحدود
}

void ScrollView::ScrollTo(Float32 x, Float32 y) {
    SetScrollOffset(Vec2(x, y));
}

void ScrollView::ScrollToElement(Element* element) {
    if (!element) return;
    
    // TODO: حساب موقع العنصر والتمرير إليه
}

void ScrollView::Draw() {
    Element::Draw();
}

bool ScrollView::HandleMouseMove(Float32 x, Float32 y) {
    // TODO: التعامل مع عجلة الفأرة
    return Element::HandleMouseMove(x, y);
}

// ==============================================================================
// Stack - المكدس / Stack
// ==============================================================================

Stack::Stack() {
    // العناصر الأبناء ستكون فوق بعضها / Children will overlap
}

// ==============================================================================
// Label - النص / Label
// ==============================================================================

Label::Label(const std::string& text) : m_text(text) {}

void Label::Draw() {
    if (!m_style.visible) return;
    
    Element::Draw();
    
    // Note: Text rendering requires Font - not yet wired
    // DrawText(m_text, m_bounds.x, m_bounds.y, m_fontSize, m_textColor);
}

// ==============================================================================
// Button - الزر / Button
// ==============================================================================

Button::Button(const std::string& text) : m_text(text) {
    m_style.padding = EdgeInsets::Symmetric(16, 8);
    m_style.borderRadius = 4;
}

void Button::Draw() {
    if (!m_style.visible) return;
    
    // تحديد اللون حسب الحالة / Determine color based on state
    Color bgColor = NormalColor;
    if (!m_style.enabled) {
        bgColor = DisabledColor;
    } else if (m_pressed) {
        bgColor = PressedColor;
    } else if (m_hovered) {
        bgColor = HoverColor;
    }
    
    m_style.backgroundColor = bgColor;
    
    Element::Draw();
    
    // Note: Text rendering requires Font - not yet wired
    // DrawTextCentered(m_text, m_bounds, TextColor);
}

bool Button::HandleClick(Float32 x, Float32 y) {
    if (!m_style.enabled) return false;
    return Element::HandleClick(x, y);
}

// ==============================================================================
// Image - الصورة / Image
// ==============================================================================

Image::Image() {}

void Image::Draw() {
    if (!m_style.visible || m_textureId == 0) return;
    
    Element::Draw();
    
    // Note: Texture drawing requires TextureManager - not yet wired
    // DrawTexture(m_textureId, m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height, m_tint);
}

// ==============================================================================
// ProgressBar - شريط التقدم / Progress Bar
// ==============================================================================

ProgressBar::ProgressBar() {
    m_style.height = StyleValue::Px(20);
    m_style.borderRadius = 4;
}

void ProgressBar::Draw() {
    if (!m_style.visible) return;
    
    // رسم الخلفية / Draw background
    auto* r = GetUIRenderer();
    if (r) {
        r->DrawFilledRect(m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height, m_backgroundColor);
    }
    
    // رسم الشريط الممتلئ / Draw filled bar
    Float32 fillWidth = m_bounds.width * m_progress;
    if (r) {
        r->DrawFilledRect(m_bounds.x, m_bounds.y, fillWidth, m_bounds.height, m_fillColor);
    }
    
    Element::Draw();
}

// ==============================================================================
// Slider - المنزلق / Slider
// ==============================================================================

Slider::Slider() {
    m_style.height = StyleValue::Px(24);
}

void Slider::SetValue(Float32 value) {
    Float32 newValue = std::max(m_min, std::min(m_max, value));
    
    // تطبيق الخطوة / Apply step
    if (m_step > 0) {
        newValue = std::round((newValue - m_min) / m_step) * m_step + m_min;
    }
    
    if (newValue != m_value) {
        m_value = newValue;
        if (OnValueChanged) {
            OnValueChanged(m_value);
        }
    }
}

void Slider::Draw() {
    if (!m_style.visible) return;
    
    Element::Draw();
    
    // رسم المسار / Draw track
    Float32 trackHeight = 4;
    Float32 trackY = m_bounds.y + (m_bounds.height - trackHeight) / 2;
    auto* r = GetUIRenderer();
    if (r) {
        r->DrawFilledRect(m_bounds.x, trackY, m_bounds.width, trackHeight, Color(0.3f, 0.3f, 0.3f));
    }
    
    // رسم المؤشر / Draw thumb
    Float32 thumbRadius = 10;
    Float32 normalizedValue = (m_value - m_min) / (m_max - m_min);
    Float32 thumbX = m_bounds.x + normalizedValue * m_bounds.width;
    Float32 thumbY = m_bounds.y + m_bounds.height / 2;
    if (r) {
        Color thumbColor = m_hovered ? Color(0.4f, 0.6f, 1.0f) : Color(0.2f, 0.5f, 0.9f);
        r->DrawFilledCircle(thumbX, thumbY, thumbRadius, thumbColor);
    }
}

bool Slider::HandleMouseDown(Float32 x, Float32 y) {
    if (!m_style.visible || !m_style.enabled) return false;
    
    if (m_bounds.Contains(x, y)) {
        m_dragging = true;
        m_pressed = true;
        UpdateValueFromMouse(x);
        return true;
    }
    
    return false;
}

bool Slider::HandleMouseMove(Float32 x, Float32 y) {
    Element::HandleMouseMove(x, y);
    
    if (m_dragging && m_pressed) {
        UpdateValueFromMouse(x);
        return true;
    }
    
    return false;
}

void Slider::UpdateValueFromMouse(Float32 x) {
    Float32 normalizedX = (x - m_bounds.x) / m_bounds.width;
    normalizedX = std::max(0.0f, std::min(1.0f, normalizedX));
    SetValue(m_min + normalizedX * (m_max - m_min));
}

// ==============================================================================
// Checkbox - مربع الاختيار / Checkbox
// ==============================================================================

Checkbox::Checkbox(const std::string& label) : m_label(label) {
    m_style.height = StyleValue::Px(24);
}

void Checkbox::SetChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;
        if (OnValueChanged) {
            OnValueChanged(m_checked);
        }
    }
}

void Checkbox::Draw() {
    if (!m_style.visible) return;
    
    Element::Draw();
    
    Float32 boxSize = 18;
    Float32 boxY = m_bounds.y + (m_bounds.height - boxSize) / 2;
    
    // رسم المربع / Draw box
    Color boxColor = m_hovered ? Color(0.4f, 0.4f, 0.4f) : Color(0.3f, 0.3f, 0.3f);
    auto* r = GetUIRenderer();
    if (r) {
        r->DrawRect(m_bounds.x, boxY, boxSize, boxSize, boxColor, false);
    }
    
    // رسم علامة الصح / Draw checkmark
    if (m_checked) {
        if (r) {
            // Approximate checkmark with two lines
            Float32 cx = m_bounds.x;
            Float32 cy = boxY;
            r->DrawLine(cx + boxSize * 0.2f, cy + boxSize * 0.5f,
                        cx + boxSize * 0.4f, cy + boxSize * 0.75f, Color::White);
            r->DrawLine(cx + boxSize * 0.4f, cy + boxSize * 0.75f,
                        cx + boxSize * 0.8f, cy + boxSize * 0.2f, Color::White);
        }
    }
    
    // رسم النص / Draw label
    if (!m_label.empty()) {
        // Note: Text rendering requires Font - not yet wired
    }
}

bool Checkbox::HandleClick(Float32 x, Float32 y) {
    if (!m_style.enabled) return false;
    
    if (m_bounds.Contains(x, y)) {
        SetChecked(!m_checked);
        if (OnClick) OnClick(this, x, y);
        return true;
    }
    
    return false;
}

// ==============================================================================
// RadioButton - زر الاختيار / Radio Button
// ==============================================================================

std::unordered_map<std::string, std::vector<RadioButton*>> RadioButton::s_groups;

RadioButton::RadioButton(const std::string& label, const std::string& group)
    : m_label(label), m_group(group) {
    
    if (!group.empty()) {
        s_groups[group].push_back(this);
    }
    
    m_style.height = StyleValue::Px(24);
}

void RadioButton::SetSelected(bool selected) {
    if (m_selected == selected) return;
    
    m_selected = selected;
    
    // إلغاء تحديد الآخرين في المجموعة / Deselect others in group
    if (selected && !m_group.empty()) {
        for (RadioButton* rb : s_groups[m_group]) {
            if (rb != this) {
                rb->m_selected = false;
            }
        }
    }
    
    if (selected && OnSelected) {
        OnSelected();
    }
}

void RadioButton::Draw() {
    if (!m_style.visible) return;
    
    Element::Draw();
    
    Float32 circleRadius = 9;
    Float32 circleY = m_bounds.y + m_bounds.height / 2;
    Float32 circleX = m_bounds.x + circleRadius;
    
    // رسم الدائرة الخارجية / Draw outer circle
    Color circleColor = m_hovered ? Color(0.5f, 0.5f, 0.5f) : Color(0.4f, 0.4f, 0.4f);
    auto* r = GetUIRenderer();
    if (r) {
        r->DrawCircle(circleX, circleY, circleRadius, circleColor, false);
    }
    
    // رسم الدائرة الداخلية / Draw inner circle
    if (m_selected) {
        if (r) {
            r->DrawFilledCircle(circleX, circleY, circleRadius * 0.5f, Color(0.2f, 0.6f, 1.0f));
        }
    }
    
    // رسم النص / Draw label
    if (!m_label.empty()) {
        // Note: Text rendering requires Font - not yet wired
    }
}

bool RadioButton::HandleClick(Float32 x, Float32 y) {
    if (!m_style.enabled) return false;
    
    if (m_bounds.Contains(x, y)) {
        SetSelected(true);
        if (OnClick) OnClick(this, x, y);
        return true;
    }
    
    return false;
}

// ==============================================================================
// TextInput - حقل النص / Text Input
// ==============================================================================

TextInput::TextInput() {
    m_style.padding = EdgeInsets::Symmetric(8, 6);
    m_style.borderWidth = 1;
    m_style.borderColor = Color(0.4f, 0.4f, 0.4f);
    m_style.backgroundColor = Color(0.15f, 0.15f, 0.15f);
    m_style.borderRadius = 4;
}

void TextInput::SetText(const std::string& text) {
    if (m_text != text) {
        m_text = text;
        m_cursorPosition = static_cast<Int32>(text.length());
        if (OnTextChanged) {
            OnTextChanged(m_text);
        }
    }
}

void TextInput::Draw() {
    if (!m_style.visible) return;
    
    // تغيير لون الحدود عند التركيز / Change border color on focus
    m_style.borderColor = m_focused ? Color(0.2f, 0.5f, 0.9f) : Color(0.4f, 0.4f, 0.4f);
    
    Element::Draw();
    
    // رسم النص أو العنصر النائب / Draw text or placeholder
    if (m_text.empty() && !m_placeholder.empty() && !m_focused) {
        // Note: Text rendering requires Font - not yet wired
        // DrawText(m_placeholder, m_bounds.x + padding.left, m_bounds.y + padding.top, 16, Color(0.5f, 0.5f, 0.5f));
    } else {
        // Note: Text rendering requires Font - not yet wired
        // DrawText(m_text, m_bounds.x + padding.left, m_bounds.y + padding.top, 16, Color::White);
    }
    
    // رسم المؤشر / Draw cursor
    if (m_focused && m_showCursor) {
        auto* r = GetUIRenderer();
        if (r) {
            // Approximate cursor as a thin line at text end
            Float32 cursorX = m_bounds.x + m_style.padding.left.value;
            r->DrawLine(cursorX, m_bounds.y + m_style.padding.top.value,
                        cursorX, m_bounds.y + m_bounds.height - m_style.padding.bottom.value, Color::White);
        }
    }
}

bool TextInput::HandleClick(Float32 x, Float32 y) {
    if (!m_style.enabled) return false;
    
    if (m_bounds.Contains(x, y)) {
        SetFocused(true);
        // TODO: حساب موقع المؤشر من الإحداثي x
        if (OnClick) OnClick(this, x, y);
        return true;
    }
    
    return false;
}

// ==============================================================================
// LayoutEngine - محرك التخطيط / Layout Engine
// ==============================================================================

LayoutEngine::LayoutEngine() {}

LayoutEngine::~LayoutEngine() {}

bool LayoutEngine::Initialize(Int32 viewportWidth, Int32 viewportHeight) {
    m_viewportWidth = viewportWidth;
    m_viewportHeight = viewportHeight;
    
    m_root = std::make_shared<Element>();
    m_root->GetStyle().width = StyleValue::Px(static_cast<Float32>(viewportWidth));
    m_root->GetStyle().height = StyleValue::Px(static_cast<Float32>(viewportHeight));
    
    return true;
}

void LayoutEngine::Resize(Int32 width, Int32 height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
    
    if (m_root) {
        m_root->GetStyle().width = StyleValue::Px(static_cast<Float32>(width));
        m_root->GetStyle().height = StyleValue::Px(static_cast<Float32>(height));
        m_root->MarkDirty();
    }
}

void LayoutEngine::SetRoot(std::shared_ptr<Element> root) {
    m_root = root;
    if (m_root) {
        m_root->MarkDirty();
    }
}

void LayoutEngine::CalculateLayout() {
    if (!m_root || !m_root->IsDirty()) return;
    
    CalculateFlexLayout(m_root.get(), 
                        static_cast<Float32>(m_viewportWidth),
                        static_cast<Float32>(m_viewportHeight));
}

void LayoutEngine::Draw() {
    if (!m_root) return;
    
    CalculateLayout();
    m_root->Draw();
}

void LayoutEngine::Update(Float32 deltaTime) {
    // TODO: تحديث الرسوم المتحركة
}

bool LayoutEngine::HandleMouseMove(Float32 x, Float32 y) {
    if (!m_root) return false;
    
    Element* newHovered = ElementAtPoint(x, y);
    
    if (newHovered != m_hoveredElement) {
        if (m_hoveredElement) {
            m_hoveredElement->HandleMouseMove(x, y);
        }
        m_hoveredElement = newHovered;
    }
    
    return m_root->HandleMouseMove(x, y);
}

bool LayoutEngine::HandleMouseDown(Float32 x, Float32 y) {
    if (!m_root) return false;
    
    // إزالة التركيز من العنصر الحالي / Remove focus from current element
    if (m_focusedElement) {
        Element* clicked = ElementAtPoint(x, y);
        if (clicked != m_focusedElement) {
            m_focusedElement->SetFocused(false);
            m_focusedElement = nullptr;
        }
    }
    
    return m_root->HandleMouseDown(x, y);
}

bool LayoutEngine::HandleMouseUp(Float32 x, Float32 y) {
    if (!m_root) return false;
    return m_root->HandleMouseUp(x, y);
}

bool LayoutEngine::HandleMouseClick(Float32 x, Float32 y) {
    if (!m_root) return false;
    return m_root->HandleClick(x, y);
}

bool LayoutEngine::HandleKeyDown(Int32 keyCode) {
    // TODO: معالجة لوحة المفاتيح
    return false;
}

bool LayoutEngine::HandleKeyUp(Int32 keyCode) {
    // TODO: معالجة لوحة المفاتيح
    return false;
}

bool LayoutEngine::HandleTextInput(const std::string& text) {
    if (m_focusedElement) {
        // TODO: إرسال النص للعنصر ذي التركيز
        return true;
    }
    return false;
}

Element* LayoutEngine::ElementAtPoint(Float32 x, Float32 y) {
    if (!m_root) return nullptr;
    
    std::function<Element*(Element*)> findElement = [&](Element* element) -> Element* {
        if (!element->GetStyle().visible) return nullptr;
        
        // فحص الأبناء أولاً (من الأعلى للأسفل) / Check children first
        const auto& children = element->GetChildren();
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            Element* found = findElement(it->get());
            if (found) return found;
        }
        
        // فحص هذا العنصر / Check this element
        if (element->GetBounds().Contains(x, y)) {
            return element;
        }
        
        return nullptr;
    };
    
    return findElement(m_root.get());
}

void LayoutEngine::SetFocusedElement(Element* element) {
    if (m_focusedElement == element) return;
    
    if (m_focusedElement) {
        m_focusedElement->SetFocused(false);
    }
    
    m_focusedElement = element;
    
    if (m_focusedElement) {
        m_focusedElement->SetFocused(true);
    }
}

Float32 LayoutEngine::ResolveValue(StyleValue value, Float32 parentSize, Float32 fontSize) {
    switch (value.unit) {
        case Unit::Auto:
            return 0.0f; // سيتم حسابها لاحقاً / Will be calculated later
        case Unit::Pixels:
            return value.value;
        case Unit::Percent:
            return parentSize * value.value / 100.0f;
        case Unit::Em:
            return value.value * fontSize;
        case Unit::Vh:
            return static_cast<Float32>(m_viewportHeight) * value.value / 100.0f;
        case Unit::Vw:
            return static_cast<Float32>(m_viewportWidth) * value.value / 100.0f;
        default:
            return 0.0f;
    }
}

void LayoutEngine::CalculateFlexLayout(Element* element, Float32 availableWidth, Float32 availableHeight) {
    if (!element) return;
    
    auto& style = element->GetStyle();
    auto& bounds = const_cast<LayoutRect&>(element->GetBounds());
    
    // حساب الأبعاد / Calculate dimensions
    Float32 width = style.width.IsAuto() ? availableWidth : ResolveValue(style.width, availableWidth);
    Float32 height = style.height.IsAuto() ? availableHeight : ResolveValue(style.height, availableHeight);
    
    // تطبيق الحدود القصوى والدنيا / Apply min/max constraints
    Float32 minW = ResolveValue(style.minWidth, availableWidth);
    Float32 minH = ResolveValue(style.minHeight, availableHeight);
    Float32 maxW = style.maxWidth.IsAuto() ? std::numeric_limits<Float32>::max() : ResolveValue(style.maxWidth, availableWidth);
    Float32 maxH = style.maxHeight.IsAuto() ? std::numeric_limits<Float32>::max() : ResolveValue(style.maxHeight, availableHeight);
    
    width = std::max(minW, std::min(maxW, width));
    height = std::max(minH, std::min(maxH, height));
    
    bounds.width = width;
    bounds.height = height;
    
    // حساب الحشوة / Calculate padding
    Float32 paddingTop = ResolveValue(style.padding.top, height);
    Float32 paddingRight = ResolveValue(style.padding.right, width);
    Float32 paddingBottom = ResolveValue(style.padding.bottom, height);
    Float32 paddingLeft = ResolveValue(style.padding.left, width);
    
    Float32 contentWidth = width - paddingLeft - paddingRight;
    Float32 contentHeight = height - paddingTop - paddingBottom;
    
    // ترتيب الأبناء / Get children to layout
    auto& children = element->GetChildren();
    if (children.empty()) {
        element->UpdateLayout();
        return;
    }
    
    // حساب تخطيط Flex / Calculate Flex layout
    bool isRow = (style.flexDirection == FlexDirection::Row || 
                  style.flexDirection == FlexDirection::RowReverse);
    bool isReverse = (style.flexDirection == FlexDirection::RowReverse || 
                      style.flexDirection == FlexDirection::ColumnReverse);
    
    Float32 mainAxisSize = isRow ? contentWidth : contentHeight;
    Float32 crossAxisSize = isRow ? contentHeight : contentWidth;
    
    // المرحلة 1: حساب أحجام الأبناء / Phase 1: Calculate child sizes
    std::vector<Float32> childMainSizes(children.size());
    std::vector<Float32> childCrossSizes(children.size());
    Float32 totalMainSize = 0;
    Float32 totalFlexGrow = 0;
    Float32 totalFlexShrink = 0;
    
    for (size_t i = 0; i < children.size(); ++i) {
        auto& childStyle = children[i]->GetStyle();
        
        Float32 childWidth = childStyle.width.IsAuto() ? 0 : ResolveValue(childStyle.width, contentWidth);
        Float32 childHeight = childStyle.height.IsAuto() ? 0 : ResolveValue(childStyle.height, contentHeight);
        
        Float32 basis = ResolveValue(childStyle.flexBasis, mainAxisSize);
        if (basis == 0 && childStyle.flexBasis.IsAuto()) {
            basis = isRow ? childWidth : childHeight;
        }
        
        childMainSizes[i] = basis;
        childCrossSizes[i] = isRow ? childHeight : childWidth;
        totalMainSize += basis;
        totalFlexGrow += childStyle.flexGrow;
        totalFlexShrink += childStyle.flexShrink;
        
        // المسافة بين العناصر / Gap
        if (i > 0) {
            totalMainSize += style.gap;
        }
    }
    
    // المرحلة 2: توزيع المساحة الإضافية / Phase 2: Distribute extra space
    Float32 remainingSpace = mainAxisSize - totalMainSize;
    
    if (remainingSpace > 0 && totalFlexGrow > 0) {
        // النمو / Grow
        for (size_t i = 0; i < children.size(); ++i) {
            Float32 grow = children[i]->GetStyle().flexGrow;
            if (grow > 0) {
                childMainSizes[i] += (remainingSpace * grow / totalFlexGrow);
            }
        }
    } else if (remainingSpace < 0 && totalFlexShrink > 0) {
        // الانكماش / Shrink
        for (size_t i = 0; i < children.size(); ++i) {
            Float32 shrink = children[i]->GetStyle().flexShrink;
            if (shrink > 0) {
                childMainSizes[i] += (remainingSpace * shrink / totalFlexShrink);
            }
        }
        remainingSpace = 0;
    }
    
    // المرحلة 3: حساب المواقع / Phase 3: Calculate positions
    Float32 mainOffset = 0;
    Float32 gapBetween = style.gap;
    
    // محاذاة المحور الرئيسي / Main axis alignment
    switch (style.justifyContent) {
        case JustifyContent::FlexEnd:
            mainOffset = remainingSpace;
            break;
        case JustifyContent::Center:
            mainOffset = remainingSpace / 2;
            break;
        case JustifyContent::SpaceBetween:
            if (children.size() > 1) {
                gapBetween = remainingSpace / (children.size() - 1);
            }
            break;
        case JustifyContent::SpaceAround:
            if (!children.empty()) {
                Float32 space = remainingSpace / children.size();
                mainOffset = space / 2;
                gapBetween = space;
            }
            break;
        case JustifyContent::SpaceEvenly:
            if (!children.empty()) {
                gapBetween = remainingSpace / (children.size() + 1);
                mainOffset = gapBetween;
            }
            break;
        default:
            break;
    }
    
    // المرحلة 4: وضع الأبناء / Phase 4: Position children
    for (size_t i = 0; i < children.size(); ++i) {
        size_t index = isReverse ? (children.size() - 1 - i) : i;
        auto& child = children[index];
        auto& childBounds = const_cast<LayoutRect&>(child->GetBounds());
        
        Float32 crossOffset = 0;
        
        // محاذاة المحور الثانوي / Cross axis alignment
        AlignItems align = style.alignItems;
        if (child->GetStyle().alignSelf != AlignSelf::Auto) {
            align = static_cast<AlignItems>(child->GetStyle().alignSelf);
        }
        
        Float32 childCrossSize = childCrossSizes[index];
        if (align == AlignItems::Stretch && childCrossSize == 0) {
            childCrossSize = crossAxisSize;
        }
        
        switch (align) {
            case AlignItems::FlexEnd:
                crossOffset = crossAxisSize - childCrossSize;
                break;
            case AlignItems::Center:
                crossOffset = (crossAxisSize - childCrossSize) / 2;
                break;
            default:
                break;
        }
        
        // تعيين الحدود / Set bounds
        if (isRow) {
            childBounds.x = bounds.x + paddingLeft + mainOffset;
            childBounds.y = bounds.y + paddingTop + crossOffset;
            childBounds.width = childMainSizes[index];
            childBounds.height = childCrossSize;
        } else {
            childBounds.x = bounds.x + paddingLeft + crossOffset;
            childBounds.y = bounds.y + paddingTop + mainOffset;
            childBounds.width = childCrossSize;
            childBounds.height = childMainSizes[index];
        }
        
        mainOffset += childMainSizes[index] + gapBetween;
        
        // حساب تخطيط الابن بشكل متكرر / Recursively calculate child layout
        CalculateFlexLayout(child.get(), childBounds.width, childBounds.height);
    }
    
    element->UpdateLayout();
}

} // namespace ui
} // namespace graphics
} // namespace sad
