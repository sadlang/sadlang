/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: input_widgets.h
 * المسار: sad_ui/widgets/include/sad_ui/widgets/input_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * عناصر الإدخال: Toggle، Slider، Picker، Checkbox، Radio، SearchBar
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_INPUT_WIDGETS_H
#define SAD_UI_WIDGETS_INPUT_WIDGETS_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"

#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace sad {
namespace ui {
namespace widgets {

// ═══════════════════════════════════════════════════════════════════════════════
// ToggleBuilder — مفتاح تبديل (تشغيل/إيقاف)
// ═══════════════════════════════════════════════════════════════════════════════

class ToggleBuilder {
public:
    explicit ToggleBuilder(const std::string& label = "");

    ToggleBuilder& isOn(bool value);
    ToggleBuilder& tintColor(Color c);
    ToggleBuilder& onToggle(const std::string& expression);
    ToggleBuilder& disabled(bool d);

    std::shared_ptr<UINode> build() const;

private:
    std::string label_;
    std::optional<bool> isOn_;
    std::optional<Color> tintColor_;
    std::optional<std::string> onToggleExpr_;
    bool disabled_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// SliderBuilder — منزلق قيمة رقمية
// ═══════════════════════════════════════════════════════════════════════════════

class SliderBuilder {
public:
    SliderBuilder();

    SliderBuilder& value(float v);
    SliderBuilder& minValue(float v);
    SliderBuilder& maxValue(float v);
    SliderBuilder& step(float s);
    SliderBuilder& tintColor(Color c);
    SliderBuilder& onValueChange(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::optional<float> value_;
    float min_ = 0.0f;
    float max_ = 100.0f;
    std::optional<float> step_;
    std::optional<Color> tintColor_;
    std::optional<std::string> onChangeExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// PickerBuilder — قائمة منسدلة
// ═══════════════════════════════════════════════════════════════════════════════

class PickerBuilder {
public:
    explicit PickerBuilder(const std::string& label = "");

    PickerBuilder& options(const std::vector<std::string>& opts);
    PickerBuilder& selectedIndex(int index);
    PickerBuilder& onSelect(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::string label_;
    std::vector<std::string> options_;
    std::optional<int> selectedIndex_;
    std::optional<std::string> onSelectExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// CheckboxBuilder — خانة اختيار
// ═══════════════════════════════════════════════════════════════════════════════

class CheckboxBuilder {
public:
    explicit CheckboxBuilder(const std::string& label = "");

    CheckboxBuilder& isChecked(bool value);
    CheckboxBuilder& tintColor(Color c);
    CheckboxBuilder& onToggle(const std::string& expression);
    CheckboxBuilder& disabled(bool d);

    std::shared_ptr<UINode> build() const;

private:
    std::string label_;
    std::optional<bool> isChecked_;
    std::optional<Color> tintColor_;
    std::optional<std::string> onToggleExpr_;
    bool disabled_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// RadioBuilder — زر اختيار من مجموعة
// ═══════════════════════════════════════════════════════════════════════════════

class RadioBuilder {
public:
    explicit RadioBuilder(const std::string& label = "");

    RadioBuilder& groupName(const std::string& group);
    RadioBuilder& isSelected(bool value);
    RadioBuilder& tintColor(Color c);
    RadioBuilder& onSelect(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::string label_;
    std::string groupName_;
    std::optional<bool> isSelected_;
    std::optional<Color> tintColor_;
    std::optional<std::string> onSelectExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// SearchBarBuilder — شريط بحث مع أيقونة
// ═══════════════════════════════════════════════════════════════════════════════

class SearchBarBuilder {
public:
    explicit SearchBarBuilder(const std::string& placeholder = "");

    SearchBarBuilder& text(const std::string& t);
    SearchBarBuilder& onSearch(const std::string& expression);
    SearchBarBuilder& onTextChange(const std::string& expression);

    std::shared_ptr<UINode> build() const;

private:
    std::string placeholder_;
    std::optional<std::string> text_;
    std::optional<std::string> onSearchExpr_;
    std::optional<std::string> onTextChangeExpr_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// دوال إنشاء سريعة
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<UINode> makeToggle(const std::string& label = "");
std::shared_ptr<UINode> makeSlider(float min = 0, float max = 100);
std::shared_ptr<UINode> makePicker(const std::string& label, const std::vector<std::string>& opts);
std::shared_ptr<UINode> makeCheckbox(const std::string& label = "");
std::shared_ptr<UINode> makeRadio(const std::string& label, const std::string& group);
std::shared_ptr<UINode> makeSearchBar(const std::string& placeholder = "");

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_INPUT_WIDGETS_H
