/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: control_widgets.h
 * المسار: features/graphics/widgets/controls/include/sad_ui/widgets/controls/control_widgets.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * مكونات التحكم المتقدمة: Stepper، SegmentedControl، RatingBar، TimePicker، Calendar، ColorWheel
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_UI_WIDGETS_CONTROLS_H
#define SAD_UI_WIDGETS_CONTROLS_H

#include "sad_ui/node.h"
#include "sad_ui/style.h"
#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace sad {
namespace ui {
namespace widgets {

// ── Stepper ─────────────────────────────────────────────────────────

class StepperBuilder {
public:
    StepperBuilder();
    StepperBuilder& value(float v);
    StepperBuilder& minValue(float v);
    StepperBuilder& maxValue(float v);
    StepperBuilder& step(float s);
    StepperBuilder& onValueChange(const std::string& expr);
    std::shared_ptr<UINode> build() const;
private:
    float value_ = 0.0f;
    float min_ = 0.0f;
    float max_ = 100.0f;
    float step_ = 1.0f;
    std::optional<std::string> onChangeExpr_;
};

// ── SegmentedControl ────────────────────────────────────────────────

class SegmentedControlBuilder {
public:
    SegmentedControlBuilder();
    SegmentedControlBuilder& segments(const std::vector<std::string>& segs);
    SegmentedControlBuilder& selectedIndex(int i);
    SegmentedControlBuilder& tintColor(Color c);
    std::shared_ptr<UINode> build() const;
private:
    std::vector<std::string> segments_;
    int selectedIndex_ = 0;
    std::optional<Color> tintColor_;
};

// ── RatingBar ───────────────────────────────────────────────────────

class RatingBarBuilder {
public:
    RatingBarBuilder();
    RatingBarBuilder& rating(float r);
    RatingBarBuilder& maxRating(int m);
    RatingBarBuilder& halfStars(bool h);
    RatingBarBuilder& activeColor(Color c);
    RatingBarBuilder& readOnly(bool r);
    std::shared_ptr<UINode> build() const;
private:
    float rating_ = 0.0f;
    int maxRating_ = 5;
    bool halfStars_ = false;
    std::optional<Color> activeColor_;
    bool readOnly_ = false;
};

// ── TimePicker ──────────────────────────────────────────────────────

class TimePickerBuilder {
public:
    TimePickerBuilder();
    TimePickerBuilder& hour(int h);
    TimePickerBuilder& minute(int m);
    TimePickerBuilder& is24Hour(bool h);
    TimePickerBuilder& onTimeChange(const std::string& expr);
    std::shared_ptr<UINode> build() const;
private:
    int hour_ = 0;
    int minute_ = 0;
    bool is24Hour_ = true;
    std::optional<std::string> onChangeExpr_;
};

// ── Calendar ────────────────────────────────────────────────────────

class CalendarBuilder {
public:
    CalendarBuilder();
    CalendarBuilder& selectedDate(const std::string& date);
    CalendarBuilder& minDate(const std::string& date);
    CalendarBuilder& maxDate(const std::string& date);
    CalendarBuilder& firstDayOfWeek(int day);
    std::shared_ptr<UINode> build() const;
private:
    std::optional<std::string> selectedDate_;
    std::optional<std::string> minDate_;
    std::optional<std::string> maxDate_;
    int firstDayOfWeek_ = 6; // السبت
};

// ── ColorWheel ──────────────────────────────────────────────────────

class ColorWheelBuilder {
public:
    ColorWheelBuilder();
    ColorWheelBuilder& initialColor(Color c);
    ColorWheelBuilder& showAlpha(bool s);
    ColorWheelBuilder& showHex(bool s);
    std::shared_ptr<UINode> build() const;
private:
    std::optional<Color> initialColor_;
    bool showAlpha_ = false;
    bool showHex_ = true;
};

} // namespace widgets
} // namespace ui
} // namespace sad

#endif // SAD_UI_WIDGETS_CONTROLS_H
