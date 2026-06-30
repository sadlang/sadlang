/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: control_widgets.cpp
 * المسار: features/graphics/widgets/controls/src/control_widgets.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مكونات التحكم: Stepper، SegmentedControl، RatingBar، TimePicker، Calendar، ColorWheel
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/widgets/controls/control_widgets.h"

namespace sad {
namespace ui {
namespace widgets {

// ── Stepper ─────────────────────────────────────────────────────────

StepperBuilder::StepperBuilder() = default;
StepperBuilder& StepperBuilder::value(float v) { value_ = v; return *this; }
StepperBuilder& StepperBuilder::minValue(float v) { min_ = v; return *this; }
StepperBuilder& StepperBuilder::maxValue(float v) { max_ = v; return *this; }
StepperBuilder& StepperBuilder::step(float s) { step_ = s; return *this; }
StepperBuilder& StepperBuilder::onValueChange(const std::string& expr) { onChangeExpr_ = expr; return *this; }

std::shared_ptr<UINode> StepperBuilder::build() const {
    auto node = UINode::create(UINodeType::Stepper);
    node->addModifier(Modifier::numeric(ModifierType::OnValueChange, value_));
    if (onChangeExpr_) node->addModifier(Modifier::event(ModifierType::OnValueChange, *onChangeExpr_));
    return node;
}

// ── SegmentedControl ────────────────────────────────────────────────

SegmentedControlBuilder::SegmentedControlBuilder() = default;
SegmentedControlBuilder& SegmentedControlBuilder::segments(const std::vector<std::string>& segs) { segments_ = segs; return *this; }
SegmentedControlBuilder& SegmentedControlBuilder::selectedIndex(int i) { selectedIndex_ = i; return *this; }
SegmentedControlBuilder& SegmentedControlBuilder::tintColor(Color c) { tintColor_ = c; return *this; }

std::shared_ptr<UINode> SegmentedControlBuilder::build() const {
    auto node = UINode::create(UINodeType::SegmentedControl);
    for (const auto& seg : segments_) {
        auto child = UINode::create(UINodeType::Text);
        child->setPrimaryArg(seg);
        node->addChild(child);
    }
    if (tintColor_) node->addModifier(Modifier::color(ModifierType::TintColor, *tintColor_));
    return node;
}

// ── RatingBar ───────────────────────────────────────────────────────

RatingBarBuilder::RatingBarBuilder() = default;
RatingBarBuilder& RatingBarBuilder::rating(float r) { rating_ = r; return *this; }
RatingBarBuilder& RatingBarBuilder::maxRating(int m) { maxRating_ = m; return *this; }
RatingBarBuilder& RatingBarBuilder::halfStars(bool h) { halfStars_ = h; return *this; }
RatingBarBuilder& RatingBarBuilder::activeColor(Color c) { activeColor_ = c; return *this; }
RatingBarBuilder& RatingBarBuilder::readOnly(bool r) { readOnly_ = r; return *this; }

std::shared_ptr<UINode> RatingBarBuilder::build() const {
    auto node = UINode::create(UINodeType::RatingBar);
    node->addModifier(Modifier::numeric(ModifierType::OnValueChange, rating_));
    if (activeColor_) node->addModifier(Modifier::color(ModifierType::TintColor, *activeColor_));
    if (readOnly_) node->addModifier({ModifierType::Disabled, true, {}});
    return node;
}

// ── TimePicker ──────────────────────────────────────────────────────

TimePickerBuilder::TimePickerBuilder() = default;
TimePickerBuilder& TimePickerBuilder::hour(int h) { hour_ = h; return *this; }
TimePickerBuilder& TimePickerBuilder::minute(int m) { minute_ = m; return *this; }
TimePickerBuilder& TimePickerBuilder::is24Hour(bool h) { is24Hour_ = h; return *this; }
TimePickerBuilder& TimePickerBuilder::onTimeChange(const std::string& expr) { onChangeExpr_ = expr; return *this; }

std::shared_ptr<UINode> TimePickerBuilder::build() const {
    auto node = UINode::create(UINodeType::TimePicker);
    if (onChangeExpr_) node->addModifier(Modifier::event(ModifierType::OnValueChange, *onChangeExpr_));
    return node;
}

// ── Calendar ────────────────────────────────────────────────────────

CalendarBuilder::CalendarBuilder() = default;
CalendarBuilder& CalendarBuilder::selectedDate(const std::string& date) { selectedDate_ = date; return *this; }
CalendarBuilder& CalendarBuilder::minDate(const std::string& date) { minDate_ = date; return *this; }
CalendarBuilder& CalendarBuilder::maxDate(const std::string& date) { maxDate_ = date; return *this; }
CalendarBuilder& CalendarBuilder::firstDayOfWeek(int day) { firstDayOfWeek_ = day; return *this; }

std::shared_ptr<UINode> CalendarBuilder::build() const {
    auto node = UINode::create(UINodeType::Calendar);
    if (selectedDate_) node->setPrimaryArg(*selectedDate_);
    return node;
}

// ── ColorWheel ──────────────────────────────────────────────────────

ColorWheelBuilder::ColorWheelBuilder() = default;
ColorWheelBuilder& ColorWheelBuilder::initialColor(Color c) { initialColor_ = c; return *this; }
ColorWheelBuilder& ColorWheelBuilder::showAlpha(bool s) { showAlpha_ = s; return *this; }
ColorWheelBuilder& ColorWheelBuilder::showHex(bool s) { showHex_ = s; return *this; }

std::shared_ptr<UINode> ColorWheelBuilder::build() const {
    auto node = UINode::create(UINodeType::ColorWheel);
    if (initialColor_) node->addModifier(Modifier::color(ModifierType::ForegroundColor, *initialColor_));
    return node;
}

} // namespace widgets
} // namespace ui
} // namespace sad
