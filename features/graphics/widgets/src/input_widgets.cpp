/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: input_widgets.cpp
 * المسار: features/graphics/widgets/src/input_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عناصر الإدخال: Toggle، Slider، Picker، Checkbox، Radio، SearchBar
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/input_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ═══════════════════════════════════════════════════════════════════════════════
// ToggleBuilder
// ═══════════════════════════════════════════════════════════════════════════════

ToggleBuilder::ToggleBuilder(const std::string& label) : label_(label) {}

ToggleBuilder& ToggleBuilder::isOn(bool value) { isOn_ = value; return *this; }
ToggleBuilder& ToggleBuilder::tintColor(Color c) { tintColor_ = c; return *this; }
ToggleBuilder& ToggleBuilder::onToggle(const std::string& expression) { onToggleExpr_ = expression; return *this; }
ToggleBuilder& ToggleBuilder::disabled(bool d) { disabled_ = d; return *this; }

std::shared_ptr<UINode> ToggleBuilder::build() const {
    auto node = UINode::create(UINodeType::Toggle);
    node->setPrimaryArg(label_);
    if (isOn_) node->addModifier({ModifierType::OnValueChange, *isOn_, {}});
    if (tintColor_) node->addModifier(Modifier::color(ModifierType::TintColor, *tintColor_));
    if (onToggleExpr_) node->addModifier(Modifier::event(ModifierType::OnValueChange, *onToggleExpr_));
    if (disabled_) node->addModifier({ModifierType::Disabled, true, {}});
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SliderBuilder
// ═══════════════════════════════════════════════════════════════════════════════

SliderBuilder::SliderBuilder() = default;

SliderBuilder& SliderBuilder::value(float v) { value_ = v; return *this; }
SliderBuilder& SliderBuilder::minValue(float v) { min_ = v; return *this; }
SliderBuilder& SliderBuilder::maxValue(float v) { max_ = v; return *this; }
SliderBuilder& SliderBuilder::step(float s) { step_ = s; return *this; }
SliderBuilder& SliderBuilder::tintColor(Color c) { tintColor_ = c; return *this; }
SliderBuilder& SliderBuilder::onValueChange(const std::string& expression) { onChangeExpr_ = expression; return *this; }

std::shared_ptr<UINode> SliderBuilder::build() const {
    auto node = UINode::create(UINodeType::Slider);
    if (value_) node->addModifier(Modifier::numeric(ModifierType::OnValueChange, *value_));
    if (tintColor_) node->addModifier(Modifier::color(ModifierType::TintColor, *tintColor_));
    if (onChangeExpr_) node->addModifier(Modifier::event(ModifierType::OnValueChange, *onChangeExpr_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// PickerBuilder
// ═══════════════════════════════════════════════════════════════════════════════

PickerBuilder::PickerBuilder(const std::string& label) : label_(label) {}

PickerBuilder& PickerBuilder::options(const std::vector<std::string>& opts) { options_ = opts; return *this; }
PickerBuilder& PickerBuilder::selectedIndex(int index) { selectedIndex_ = index; return *this; }
PickerBuilder& PickerBuilder::onSelect(const std::string& expression) { onSelectExpr_ = expression; return *this; }

std::shared_ptr<UINode> PickerBuilder::build() const {
    auto node = UINode::create(UINodeType::Picker);
    node->setPrimaryArg(label_);
    for (const auto& opt : options_) {
        auto child = UINode::create(UINodeType::Text);
        child->setPrimaryArg(opt);
        node->addChild(child);
    }
    if (onSelectExpr_) node->addModifier(Modifier::event(ModifierType::OnValueChange, *onSelectExpr_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// CheckboxBuilder
// ═══════════════════════════════════════════════════════════════════════════════

CheckboxBuilder::CheckboxBuilder(const std::string& label) : label_(label) {}

CheckboxBuilder& CheckboxBuilder::isChecked(bool value) { isChecked_ = value; return *this; }
CheckboxBuilder& CheckboxBuilder::tintColor(Color c) { tintColor_ = c; return *this; }
CheckboxBuilder& CheckboxBuilder::onToggle(const std::string& expression) { onToggleExpr_ = expression; return *this; }
CheckboxBuilder& CheckboxBuilder::disabled(bool d) { disabled_ = d; return *this; }

std::shared_ptr<UINode> CheckboxBuilder::build() const {
    auto node = UINode::create(UINodeType::Checkbox);
    node->setPrimaryArg(label_);
    if (isChecked_) node->addModifier({ModifierType::OnValueChange, *isChecked_, {}});
    if (tintColor_) node->addModifier(Modifier::color(ModifierType::TintColor, *tintColor_));
    if (onToggleExpr_) node->addModifier(Modifier::event(ModifierType::OnValueChange, *onToggleExpr_));
    if (disabled_) node->addModifier({ModifierType::Disabled, true, {}});
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// RadioBuilder
// ═══════════════════════════════════════════════════════════════════════════════

RadioBuilder::RadioBuilder(const std::string& label) : label_(label) {}

RadioBuilder& RadioBuilder::groupName(const std::string& group) { groupName_ = group; return *this; }
RadioBuilder& RadioBuilder::isSelected(bool value) { isSelected_ = value; return *this; }
RadioBuilder& RadioBuilder::tintColor(Color c) { tintColor_ = c; return *this; }
RadioBuilder& RadioBuilder::onSelect(const std::string& expression) { onSelectExpr_ = expression; return *this; }

std::shared_ptr<UINode> RadioBuilder::build() const {
    auto node = UINode::create(UINodeType::Radio);
    node->setPrimaryArg(label_);
    if (isSelected_) node->addModifier({ModifierType::OnValueChange, *isSelected_, {}});
    if (tintColor_) node->addModifier(Modifier::color(ModifierType::TintColor, *tintColor_));
    if (onSelectExpr_) node->addModifier(Modifier::event(ModifierType::OnValueChange, *onSelectExpr_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// SearchBarBuilder
// ═══════════════════════════════════════════════════════════════════════════════

SearchBarBuilder::SearchBarBuilder(const std::string& placeholder) : placeholder_(placeholder) {}

SearchBarBuilder& SearchBarBuilder::text(const std::string& t) { text_ = t; return *this; }
SearchBarBuilder& SearchBarBuilder::onSearch(const std::string& expression) { onSearchExpr_ = expression; return *this; }
SearchBarBuilder& SearchBarBuilder::onTextChange(const std::string& expression) { onTextChangeExpr_ = expression; return *this; }

std::shared_ptr<UINode> SearchBarBuilder::build() const {
    auto node = UINode::create(UINodeType::SearchBar);
    node->setPrimaryArg(placeholder_);
    if (text_) node->addModifier({ModifierType::OnValueChange, *text_, {}});
    if (onSearchExpr_) node->addModifier(Modifier::event(ModifierType::OnTap, *onSearchExpr_));
    if (onTextChangeExpr_) node->addModifier(Modifier::event(ModifierType::OnValueChange, *onTextChangeExpr_));
    return node;
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeToggle(const std::string& label) {
    return ToggleBuilder(label).build();
}

std::shared_ptr<UINode> makeSlider(float min, float max) {
    return SliderBuilder().minValue(min).maxValue(max).build();
}

std::shared_ptr<UINode> makePicker(const std::string& label, const std::vector<std::string>& opts) {
    return PickerBuilder(label).options(opts).build();
}

std::shared_ptr<UINode> makeCheckbox(const std::string& label) {
    return CheckboxBuilder(label).build();
}

std::shared_ptr<UINode> makeRadio(const std::string& label, const std::string& group) {
    return RadioBuilder(label).groupName(group).build();
}

std::shared_ptr<UINode> makeSearchBar(const std::string& placeholder) {
    return SearchBarBuilder(placeholder).build();
}

} // namespace widgets
} // namespace ui
} // namespace sad
