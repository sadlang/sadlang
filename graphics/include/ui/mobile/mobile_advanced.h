/**
 * @file mobile_advanced.h
 * @brief …״±ƒ״¨״§״× ˆ״§״¬‡״© ״§„…״³״×״®״¯… ״§„…״×‚״¯…״© „„״×״·״¨‚״§״× ״§„…״­…ˆ„״©
 * 
 * ״­״×ˆ ״¹„‰ 18 …״±ƒ״¨ …״×‚״¯…:
 * - …״®״·״· ״¨״§† Chart
 * - …״®״·״· ״¯״§״¦״± PieChart
 * - …״®״·״· ״®״· LineChart
 * - ״×‚ˆ… Calendar
 * - ״´״±״· ״×‚… RatingBar
 * - …״×״­ƒ… ״§„״®״·ˆ״§״× Stepper
 * - ״×״­ƒ… …‚״³… SegmentedControl
 * - …†״×‚ ״§„״£„ˆ״§† ColorPicker
 * - …†״×‚ ״§„״×״§״±״® DateTimePicker
 * - ״´״±״· ״¨״­״« …״×‚״¯… SearchBar
 * - „״§״×״© Banner
 * - ״´״±״­״© Chip / ChipGroup
 * - ˆ״±‚״© ״³„״© BottomSheet
 * - ‚״§״¦…״© ״³״±״¹״© SpeedDial
 * - ״¹״±״¶ ״´״¬״± TreeView
 * - ״¬״¯ˆ„ ״¨״§†״§״× DataTable
 * - ״®״· ״²…† Timeline
 * - „ˆ״­״© ƒ״§†״¨״§† KanbanBoard
 * 
 * @author ״±‚ „״÷״© ״µ
 * @date 2026
 */
#pragma once

#include "mobile_core.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <map>
#include <cmath>
#include <algorithm>
#include <sstream>

namespace sad {
namespace graphics {
namespace ui {
namespace mobile {

// =============================================================================
// 1. …״®״·״· ״¨״§† - Chart (‚״§״¹״¯״©)
// =============================================================================

/// †‚״·״© ״¨״§†״§״×  ״§„…״®״·״·
struct ChartDataPoint {
    float x = 0.0f;        ///< ״§„‚…״© ״§„״£‚״©
    float y = 0.0f;        ///< ״§„‚…״© ״§„״±״£״³״©
    std::string label;      ///< ״§„״×״³…״©
    Color color;            ///< ״§„„ˆ†
};

/// ״³„״³„״© ״¨״§†״§״×
struct ChartSeries {
    std::string name;                       ///< ״§״³… ״§„״³„״³„״©
    std::vector<ChartDataPoint> points;     ///< †‚״§״· ״§„״¨״§†״§״×
    Color color = {0.2f, 0.6f, 1.0f, 1.0f}; ///< „ˆ† ״§„״³„״³„״©
    float lineWidth = 2.0f;                 ///< ״³…ƒ ״§„״®״·
    bool showPoints = true;                 ///< ״¥״¸‡״§״± ״§„†‚״§״·
    bool showLabels = false;                ///< ״¥״¸‡״§״± ״§„״×״³…״§״×
    bool filled = false;                    ///< ״×״¹״¨״¦״© ״§„…†״·‚״© ״×״­״× ״§„״®״·
};

/// †ˆ״¹ ״§„…״®״·״·
enum class ChartType {
    Bar,        ///< …״®״·״· ״£״¹…״¯״©
    Line,       ///< …״®״·״· ״®״·
    Area,       ///< …״®״·״· …״³״§״­״©
    Scatter,    ///< …״®״·״· †‚״§״· …״¨״¹״«״±״©
    Bubble      ///< …״®״·״· ‚״§״¹״§״×
};

/// ״¥״¹״¯״§״¯״§״× ״§„…״­ˆ״±
struct ChartAxis {
    std::string title;          ///< ״¹†ˆ״§† ״§„…״­ˆ״±
    float min = 0.0f;          ///< ״§„‚…״© ״§„״¯†״§
    float max = 100.0f;        ///< ״§„‚…״© ״§„״¹„״§
    float step = 10.0f;        ///< ״§„״®״·ˆ״©
    bool autoRange = true;     ///< †״·״§‚ ״×„‚״§״¦
    bool showGrid = true;      ///< ״¥״¸‡״§״± ״§„״´״¨ƒ״©
    Color gridColor = {0.3f, 0.3f, 0.3f, 0.3f}; ///< „ˆ† ״§„״´״¨ƒ״©
};

/// …״®״·״· ״¨״§† ״£״³״§״³ - …ƒ† ״×…״¯״¯‡ „ƒ„ †ˆ״¹
class Chart : public Widget {
public:
    ChartType type = ChartType::Bar;    ///< †ˆ״¹ ״§„…״®״·״·
    std::vector<ChartSeries> series;    ///< ״³„״§״³„ ״§„״¨״§†״§״×
    ChartAxis xAxis;                    ///< ״§„…״­ˆ״± ״§„״£‚
    ChartAxis yAxis;                    ///< ״§„…״­ˆ״± ״§„״±״£״³
    bool showLegend = true;             ///< ״¥״¸‡״§״± ˆ״³„״© ״§„״¥״¶״§״­
    bool animated = true;               ///< …״®״·״· …״×״­״±ƒ
    float animationProgress = 1.0f;     ///< ״×‚״¯… ״§„״±״³ˆ… ״§„…״×״­״±ƒ״©
    Color backgroundColor = {0.1f, 0.1f, 0.15f, 1.0f}; ///< „ˆ† ״§„״®„״©
    Padding chartPadding = {40, 40, 50, 60}; ///< ״­״´ˆ ״§„…״®״·״·

    /// ״¥״¶״§״© ״³„״³„״© ״¨״§†״§״×
    void addSeries(const ChartSeries& s) {
        series.push_back(s);
        if (yAxis.autoRange) recalculateRange();
    }

    /// ״¥״¶״§״© †‚״·״© „״³„״³„״© …״¹†״©
    void addPoint(size_t seriesIndex, const ChartDataPoint& point) {
        if (seriesIndex < series.size()) {
            series[seriesIndex].points.push_back(point);
            if (yAxis.autoRange) recalculateRange();
        }
    }

    /// …״³״­ ״¬…״¹ ״§„״¨״§†״§״×
    void clearData() {
        series.clear();
    }

    /// ״¨״¯״¡ ״§„״±״³ˆ… ״§„…״×״­״±ƒ״©
    void animate() {
        animationProgress = 0.0f;
        animated = true;
    }

    /// ״×״­״¯״« ״§„״±״³ˆ… ״§„…״×״­״±ƒ״©
    void updateAnimation(float deltaTime) {
        if (animationProgress < 1.0f) {
            animationProgress = std::min(1.0f, animationProgress + deltaTime * 2.0f);
        }
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„†״·״§‚ ״§„״×„‚״§״¦ „„‚…
    void recalculateRange() {
        float minY = 1e30f, maxY = -1e30f;
        float minX = 1e30f, maxX = -1e30f;
        for (auto& s : series) {
            for (auto& p : s.points) {
                minY = std::min(minY, p.y);
                maxY = std::max(maxY, p.y);
                minX = std::min(minX, p.x);
                maxX = std::max(maxX, p.x);
            }
        }
        if (xAxis.autoRange) { xAxis.min = minX; xAxis.max = maxX; }
        if (yAxis.autoRange) { yAxis.min = minY; yAxis.max = maxY; }
    }

    /// ״×״­ˆ„ ‚…״© ״¥„‰ ״¥״­״¯״§״« ״´״§״´״©
    float mapToScreenX(float value) const {
        float range = xAxis.max - xAxis.min;
        if (range == 0) return bounds.x + chartPadding.left;
        return bounds.x + chartPadding.left + 
               (value - xAxis.min) / range * (bounds.w - chartPadding.left - chartPadding.right);
    }

    float mapToScreenY(float value) const {
        float range = yAxis.max - yAxis.min;
        if (range == 0) return bounds.y + bounds.h - chartPadding.bottom;
        return bounds.y + bounds.h - chartPadding.bottom - 
               (value - yAxis.min) / range * (bounds.h - chartPadding.top - chartPadding.bottom);
    }
};

// =============================================================================
// 2. …״®״·״· ״¯״§״¦״± - PieChart
// =============================================================================

/// ״´״±״­״©  ״§„…״®״·״· ״§„״¯״§״¦״±
struct PieSlice {
    std::string label;      ///< ״§„״×״³…״©
    float value = 0.0f;     ///< ״§„‚…״©
    Color color;            ///< ״§„„ˆ†
    bool exploded = false;  ///< …†״µ„״© ״¹† ״§„…״±ƒ״²
    float explodeOffset = 10.0f; ///< …״³״§״© ״§„״§†״µ״§„
};

/// …״®״·״· ״¯״§״¦״±
class PieChart : public Widget {
public:
    std::vector<PieSlice> slices;       ///< ״§„״´״±״§״¦״­
    bool showLabels = true;             ///< ״¥״¸‡״§״± ״§„״×״³…״§״×
    bool showPercentages = true;        ///< ״¥״¸‡״§״± ״§„†״³״¨ ״§„…״¦ˆ״©
    bool donut = false;                 ///< …״®״·״· ״­„‚ (״¯ˆ†״§״×)
    float donutWidth = 0.4f;            ///< ״³…ƒ ״§„״­„‚״© (0-1)
    float startAngle = -90.0f;          ///< ״²״§ˆ״© ״§„״¨״¯״§״© ״¨״§„״¯״±״¬״§״×
    bool animated = true;               ///< ״±״³ˆ… …״×״­״±ƒ״©
    float animationProgress = 1.0f;     ///< ״×‚״¯… ״§„״­״±ƒ״©
    Color backgroundColor = {0.1f, 0.1f, 0.15f, 1.0f};

    /// ״¥״¶״§״© ״´״±״­״©
    void addSlice(const std::string& label, float value, const Color& color) {
        slices.push_back({label, value, color, false, 10.0f});
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„…״¬…ˆ״¹ ״§„ƒ„
    float getTotal() const {
        float total = 0;
        for (auto& s : slices) total += s.value;
        return total;
    }

    /// ״§„״­״µˆ„ ״¹„‰ †״³״¨״© ״´״±״­״©
    float getPercentage(size_t index) const {
        float total = getTotal();
        if (total == 0 || index >= slices.size()) return 0;
        return (slices[index].value / total) * 100.0f;
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״²״§ˆ״© ״´״±״­״©
    float getSliceAngle(size_t index) const {
        float total = getTotal();
        if (total == 0 || index >= slices.size()) return 0;
        return (slices[index].value / total) * 360.0f;
    }

    /// ״¨״¯״¡ ״§„״±״³ˆ… ״§„…״×״­״±ƒ״©
    void animate() { animationProgress = 0.0f; }

    void updateAnimation(float dt) {
        if (animationProgress < 1.0f)
            animationProgress = std::min(1.0f, animationProgress + dt * 2.0f);
    }
};

// =============================================================================
// 3. …״®״·״· ״®״· …״×‚״¯… - LineChart
// =============================================================================

/// …״®״·״· ״®״· …״¹ …״²״§״× „…״³ ״×״§״¹„״©
class LineChart : public Chart {
public:
    bool smoothCurve = true;            ///< …†״­†‰ ״³„״³ (Bezier)
    bool showCrosshair = false;         ///< ״¥״¸‡״§״± ״®״·ˆ״· ״§„״×‚״§״·״¹ ״¹†״¯ ״§„„…״³
    float crosshairX = 0.0f;           ///< …ˆ״¶״¹ ״§„״×‚״§״·״¹ ״§„״£‚
    bool showTooltip = true;            ///< ״¥״¸‡״§״± ״×„…״­ ״¹†״¯ ״§„„…״³
    bool zoomable = false;              ///< ‚״§״¨„ „„״×ƒ״¨״±
    float zoomLevel = 1.0f;             ///< …״³״×ˆ‰ ״§„״×ƒ״¨״±
    float panOffset = 0.0f;             ///< ״¥״²״§״­״© ״§„״×…״±״±

    LineChart() { type = ChartType::Line; }

    /// ״×״­״¯״¯ …ˆ״¶״¹ ״§„״×‚״§״·״¹ …† ״¥״­״¯״§״« „…״³
    void setCrosshairFromTouch(float screenX) {
        float range = xAxis.max - xAxis.min;
        float chartLeft = bounds.x + chartPadding.left;
        float chartWidth = bounds.w - chartPadding.left - chartPadding.right;
        if (chartWidth > 0) {
            crosshairX = xAxis.min + (screenX - chartLeft) / chartWidth * range;
            showCrosshair = true;
        }
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״£‚״±״¨ †‚״·״© „‚…״© X
    ChartDataPoint getNearestPoint(size_t seriesIdx, float x) const {
        if (seriesIdx >= series.size() || series[seriesIdx].points.empty())
            return {};
        auto& pts = series[seriesIdx].points;
        auto it = std::min_element(pts.begin(), pts.end(),
            [x](const ChartDataPoint& a, const ChartDataPoint& b) {
                return std::abs(a.x - x) < std::abs(b.x - x);
            });
        return *it;
    }
};

// =============================================================================
// 4. ״×‚ˆ… - Calendar
// =============================================================================

/// ״­״¯״«  ״§„״×‚ˆ…
struct CalendarEvent {
    std::string title;          ///< ״¹†ˆ״§† ״§„״­״¯״«
    int year, month, day;       ///< ״§„״×״§״±״®
    int hour = 0, minute = 0;   ///< ״§„ˆ‚״×
    int durationMinutes = 60;   ///< ״§„…״¯״© ״¨״§„״¯‚״§״¦‚
    Color color = {0.3f, 0.6f, 1.0f, 1.0f}; ///< „ˆ† ״§„״­״¯״«
    std::string description;    ///< ״§„ˆ״µ
};

/// ˆ״¶״¹ ״¹״±״¶ ״§„״×‚ˆ…
enum class CalendarViewMode {
    Month,      ///< ״¹״±״¶ ״´‡״±
    Week,       ///< ״¹״±״¶ ״£״³״¨ˆ״¹
    Day,        ///< ״¹״±״¶ ˆ…
    Year        ///< ״¹״±״¶ ״³†ˆ
};

/// …״±ƒ״¨ ״§„״×‚ˆ… - ״¯״¹… ״§„״×‚ˆ… ״§„‡״¬״± ˆ״§„…„״§״¯
class Calendar : public Widget {
public:
    int currentYear = 2026;             ///< ״§„״³†״© ״§„״­״§„״©
    int currentMonth = 1;               ///< ״§„״´‡״± ״§„״­״§„ (1-12)
    int selectedDay = -1;               ///< ״§„ˆ… ״§„…״®״×״§״± (-1 = „״§ ״´״¡)
    CalendarViewMode viewMode = CalendarViewMode::Month;
    bool hijriMode = false;             ///< ˆ״¶״¹ ״§„״×‚ˆ… ״§„‡״¬״±
    bool showWeekNumbers = false;       ///< ״¥״¸‡״§״± ״£״±‚״§… ״§„״£״³״§״¨״¹
    bool rtl = true;                    ///< ״¯״¹… ״§„״§״×״¬״§‡ …† ״§„…† „„״³״§״±
    std::vector<CalendarEvent> events;  ///< ״§„״£״­״¯״§״«
    Color selectedColor = {0.2f, 0.5f, 1.0f, 1.0f};
    Color todayColor = {1.0f, 0.4f, 0.2f, 1.0f};
    Color eventDotColor = {0.3f, 0.8f, 0.5f, 1.0f};

    /// ״¯״§„״© ״±״¯ ״¹†״¯ ״§״®״×״§״± ˆ…
    std::function<void(int year, int month, int day)> onDateSelected;
    /// ״¯״§„״© ״±״¯ ״¹†״¯ ״×״÷״± ״§„״´‡״±
    std::function<void(int year, int month)> onMonthChanged;

    /// ״¥״¶״§״© ״­״¯״«
    void addEvent(const CalendarEvent& event) {
        events.push_back(event);
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״£״­״¯״§״« ˆ… …״¹†
    std::vector<CalendarEvent> getEventsForDay(int year, int month, int day) const {
        std::vector<CalendarEvent> result;
        for (auto& e : events) {
            if (e.year == year && e.month == month && e.day == day)
                result.push_back(e);
        }
        return result;
    }

    /// ״§„״§†״×‚״§„ „„״´‡״± ״§„״×״§„
    void nextMonth() {
        currentMonth++;
        if (currentMonth > 12) { currentMonth = 1; currentYear++; }
        if (onMonthChanged) onMonthChanged(currentYear, currentMonth);
    }

    /// ״§„״§†״×‚״§„ „„״´‡״± ״§„״³״§״¨‚
    void previousMonth() {
        currentMonth--;
        if (currentMonth < 1) { currentMonth = 12; currentYear--; }
        if (onMonthChanged) onMonthChanged(currentYear, currentMonth);
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״¹״¯״¯ ״£״§… ״§„״´‡״±
    int getDaysInMonth(int year, int month) const {
        static const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (month < 1 || month > 12) return 30;
        int d = days[month - 1];
        if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
            d = 29;
        return d;
    }

    /// ״§״®״×״§״± ˆ…
    void selectDay(int day) {
        selectedDay = day;
        if (onDateSelected) onDateSelected(currentYear, currentMonth, day);
    }
};

// =============================================================================
// 5. ״´״±״· ״×‚… - RatingBar
// =============================================================================

/// …״±ƒ״¨ ״§„״×‚… ״¨״§„†״¬ˆ…
class RatingBar : public Widget {
public:
    float rating = 0.0f;            ///< ״§„״×‚… ״§„״­״§„
    float maxRating = 5.0f;         ///< ״£‚״µ‰ ״×‚…
    float stepSize = 0.5f;          ///< ״­״¬… ״§„״®״·ˆ״© (0.5 = †״µ †״¬…״©)
    float starSize = 32.0f;         ///< ״­״¬… ״§„†״¬…״©
    float spacing = 4.0f;           ///< ״§„…״³״§״© ״¨† ״§„†״¬ˆ…
    Color activeColor = {1.0f, 0.84f, 0.0f, 1.0f};   ///< „ˆ† ״§„†״¬…״© ״§„״¹״§„״© (״°‡״¨)
    Color inactiveColor = {0.4f, 0.4f, 0.4f, 0.5f};   ///< „ˆ† ״§„†״¬…״© ״÷״± ״§„״¹״§„״©
    bool readOnly = false;          ///< „„‚״±״§״¡״© ‚״·
    bool showValue = false;         ///< ״¥״¸‡״§״± ״§„‚…״© ״§„״±‚…״©
    std::function<void(float)> onRatingChanged; ///< ״¯״§„״© ״±״¯ ״¹†״¯ ״×״÷״± ״§„״×‚…

    /// ״¶״¨״· ״§„״×‚…
    void setRating(float value) {
        // ״×‚״±״¨ „„״®״·ˆ״© ״§„״£‚״±״¨
        value = std::round(value / stepSize) * stepSize;
        rating = std::max(0.0f, std::min(maxRating, value));
        if (onRatingChanged) onRatingChanged(rating);
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„״×‚… ƒ†״³״¨״© …״¦ˆ״©
    float getPercentage() const {
        return maxRating > 0 ? (rating / maxRating) * 100.0f : 0.0f;
    }

    /// …״¹״§„״¬״© ״§„„…״³
    void handleTouch(float touchX) {
        if (readOnly) return;
        float startX = bounds.x;
        float totalWidth = maxRating * (starSize + spacing);
        float relX = touchX - startX;
        float newRating = (relX / totalWidth) * maxRating;
        setRating(newRating);
    }
};

// =============================================================================
// 6. …״×״­ƒ… ״§„״®״·ˆ״§״× - Stepper
// =============================================================================

/// ״®״·ˆ״©  ״§„…״×״­ƒ…
struct StepperStep {
    std::string title;          ///< ״¹†ˆ״§† ״§„״®״·ˆ״©
    std::string subtitle;       ///< ״¹†ˆ״§† ״±״¹
    bool completed = false;     ///< …ƒ״×…„״©
    bool active = false;        ///< †״´״·״© ״­״§„״§‹
    bool hasError = false;      ///< ״¨‡״§ ״®״·״£
    Color color;                ///< „ˆ† …״®״µ״µ
};

/// ״§״×״¬״§‡ ״§„…״×״­ƒ…
enum class StepperOrientation {
    Horizontal,  ///< ״£‚
    Vertical     ///< ״±״£״³
};

/// …״×״­ƒ… ״§„״®״·ˆ״§״× - „״¹״±״¶ ״×‚״¯… ״§„״¹…„״§״×
class Stepper : public Widget {
public:
    std::vector<StepperStep> steps;     ///< ״§„״®״·ˆ״§״×
    int currentStep = 0;                ///< ״§„״®״·ˆ״© ״§„״­״§„״©
    StepperOrientation orientation = StepperOrientation::Horizontal;
    float stepSize = 40.0f;             ///< ״­״¬… ״¯״§״¦״±״© ״§„״®״·ˆ״©
    float lineThickness = 2.0f;         ///< ״³…ƒ ״®״· ״§„״±״¨״·
    Color activeColor = {0.2f, 0.6f, 1.0f, 1.0f};
    Color completedColor = {0.2f, 0.8f, 0.4f, 1.0f};
    Color inactiveColor = {0.5f, 0.5f, 0.5f, 0.5f};
    Color errorColor = {1.0f, 0.3f, 0.3f, 1.0f};
    bool linear = true;                 ///< ״¬״¨ ״¥ƒ…״§„ ״§„״®״·ˆ״§״× ״¨״§„״×״±״×״¨

    std::function<void(int)> onStepChanged;     ///< ״¯״§„״© ״±״¯ ״¹†״¯ ״×״÷״± ״§„״®״·ˆ״©
    std::function<void()> onCompleted;          ///< ״¯״§„״© ״±״¯ ״¹†״¯ ״§„״§ƒ״×…״§„

    /// ״¥״¶״§״© ״®״·ˆ״©
    void addStep(const std::string& title, const std::string& subtitle = "") {
        steps.push_back({title, subtitle, false, false, false, {}});
        if (steps.size() == 1) steps[0].active = true;
    }

    /// ״§„״§†״×‚״§„ „„״®״·ˆ״© ״§„״×״§„״©
    bool next() {
        if (currentStep < (int)steps.size() - 1) {
            steps[currentStep].completed = true;
            steps[currentStep].active = false;
            currentStep++;
            steps[currentStep].active = true;
            if (onStepChanged) onStepChanged(currentStep);
            return true;
        } else if (currentStep == (int)steps.size() - 1) {
            steps[currentStep].completed = true;
            if (onCompleted) onCompleted();
            return true;
        }
        return false;
    }

    /// ״§„״±״¬ˆ״¹ „„״®״·ˆ״© ״§„״³״§״¨‚״©
    bool previous() {
        if (currentStep > 0) {
            steps[currentStep].active = false;
            currentStep--;
            steps[currentStep].active = true;
            steps[currentStep].completed = false;
            if (onStepChanged) onStepChanged(currentStep);
            return true;
        }
        return false;
    }

    /// ״§„״°‡״§״¨ „״®״·ˆ״© …״¹†״© ( ״§„ˆ״¶״¹ ״÷״± ״§„״®״·)
    bool goToStep(int index) {
        if (linear) return false; // ……†ˆ״¹  ״§„ˆ״¶״¹ ״§„״®״·
        if (index >= 0 && index < (int)steps.size()) {
            steps[currentStep].active = false;
            currentStep = index;
            steps[currentStep].active = true;
            if (onStepChanged) onStepChanged(currentStep);
            return true;
        }
        return false;
    }

    /// ״×״¹† ״®״·״£  ״®״·ˆ״©
    void setError(int index) {
        if (index >= 0 && index < (int)steps.size())
            steps[index].hasError = true;
    }

    /// †״³״¨״© ״§„״§ƒ״×…״§„
    float getProgress() const {
        if (steps.empty()) return 0;
        int completed = 0;
        for (auto& s : steps) if (s.completed) completed++;
        return (float)completed / (float)steps.size();
    }
};

// =============================================================================
// 7. ״×״­ƒ… …‚״³… - SegmentedControl
// =============================================================================

/// ״¹†״µ״±  ״§„״×״­ƒ… ״§„…‚״³…
struct Segment {
    std::string label;      ///< ״§„†״µ
    std::string icon;       ///< ״£‚ˆ†״© (״§״®״×״§״±)
    bool enabled = true;    ///< …״¹‘„
    int badgeCount = 0;     ///< ״¹״¯״¯ ״§„״´״§״±״©
};

/// ״×״­ƒ… …‚״³… (…״´״§״¨‡ „€ iOS SegmentedControl)
class SegmentedControl : public Widget {
public:
    std::vector<Segment> segments;      ///< ״§„״£‚״³״§…
    int selectedIndex = 0;              ///< ״§„‚״³… ״§„…״®״×״§״±
    float cornerRadius = 8.0f;          ///< †״µ ‚״·״± ״§„״²ˆ״§״§
    Color selectedBg = {0.2f, 0.6f, 1.0f, 1.0f};    ///< ״®„״© ״§„…״®״×״§״±
    Color selectedFg = {1.0f, 1.0f, 1.0f, 1.0f};    ///< †״µ ״§„…״®״×״§״±
    Color normalBg = {0.2f, 0.2f, 0.25f, 1.0f};     ///< ״®„״© ״§„״¹״§״¯
    Color normalFg = {0.7f, 0.7f, 0.7f, 1.0f};      ///< †״µ ״§„״¹״§״¯
    Color borderColor = {0.3f, 0.3f, 0.35f, 1.0f};  ///< „ˆ† ״§„״­״¯ˆ״¯
    bool animated = true;               ///< ״§†״×‚״§„ …״×״­״±ƒ
    std::function<void(int)> onSelectionChanged; ///< ״¯״§„״© ״±״¯ ״¹†״¯ ״§„״×״÷״±

    /// ״¥״¶״§״© ‚״³…
    void addSegment(const std::string& label, const std::string& icon = "") {
        segments.push_back({label, icon, true, 0});
    }

    /// ״§״®״×״§״± ‚״³…
    void select(int index) {
        if (index >= 0 && index < (int)segments.size() && segments[index].enabled) {
            selectedIndex = index;
            if (onSelectionChanged) onSelectionChanged(index);
        }
    }

    /// ״×״¹† ״´״§״±״©
    void setBadge(int index, int count) {
        if (index >= 0 && index < (int)segments.size())
            segments[index].badgeCount = count;
    }
};

// =============================================================================
// 8. …†״×‚ ״§„״£„ˆ״§† - ColorPicker
// =============================================================================

/// ˆ״¶״¹ …†״×‚ ״§„״£„ˆ״§†
enum class ColorPickerMode {
    Wheel,      ///< ״¹״¬„״© ״§„״£„ˆ״§† (״¯״§״¦״±״©)
    Palette,    ///< „ˆ״­ ״£„ˆ״§† (״´״¨ƒ״©)
    Sliders,    ///< ״£״´״±״·״© …†״²„‚״© (RGB/HSL)
    Hex         ///< ״¥״¯״®״§„ ״³״¯״§״³ ״¹״´״±
};

/// …†״×‚ ״§„״£„ˆ״§†
class ColorPicker : public Widget {
public:
    Color selectedColor = {1.0f, 0.0f, 0.0f, 1.0f}; ///< ״§„„ˆ† ״§„…״®״×״§״±
    ColorPickerMode mode = ColorPickerMode::Wheel;
    bool showAlpha = true;              ///< ״¥״¸‡״§״± ״´״±״· ״§„״´״§״©
    bool showPreview = true;            ///< ״¥״¸‡״§״± …״¹״§†״© ״§„„ˆ†
    bool showHex = true;                ///< ״¥״¸‡״§״± ״§„‚…״© ״§„״³״¯״§״³״©
    std::vector<Color> recentColors;    ///< ״§„״£„ˆ״§† ״§„״£״®״±״©
    std::vector<Color> paletteColors;   ///< ״£„ˆ״§† ״§„„ˆ״­
    int maxRecentColors = 10;           ///< ״£‚״µ‰ ״¹״¯״¯ „„״£„ˆ״§† ״§„״£״®״±״©
    std::function<void(const Color&)> onColorChanged; ///< ״¯״§„״© ״±״¯ ״¹†״¯ ״§„״×״÷״±

    ColorPicker() {
        // ״£„ˆ״§† ״§״×״±״§״¶״© „„ˆ״­
        paletteColors = {
            {1,0,0,1}, {0,1,0,1}, {0,0,1,1}, {1,1,0,1},
            {1,0,1,1}, {0,1,1,1}, {1,0.5f,0,1}, {0.5f,0,1,1},
            {1,1,1,1}, {0.75f,0.75f,0.75f,1}, {0.5f,0.5f,0.5f,1}, {0,0,0,1}
        };
    }

    /// ״¶״¨״· ״§„„ˆ† ״¨״§„‚… RGB
    void setRGB(float r, float g, float b, float a = 1.0f) {
        selectedColor = {r, g, b, a};
        addToRecent(selectedColor);
        if (onColorChanged) onColorChanged(selectedColor);
    }

    /// ״¶״¨״· ״§„„ˆ† ״¨״§„‚… HSL
    void setHSL(float h, float s, float l, float a = 1.0f) {
        // ״×״­ˆ„ HSL ״¥„‰ RGB
        float c = (1.0f - std::abs(2.0f * l - 1.0f)) * s;
        float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
        float m = l - c / 2.0f;
        float r, g, b;
        if (h < 60)       { r = c; g = x; b = 0; }
        else if (h < 120) { r = x; g = c; b = 0; }
        else if (h < 180) { r = 0; g = c; b = x; }
        else if (h < 240) { r = 0; g = x; b = c; }
        else if (h < 300) { r = x; g = 0; b = c; }
        else               { r = c; g = 0; b = x; }
        setRGB(r + m, g + m, b + m, a);
    }

    /// ״¶״¨״· ״§„„ˆ† ״¨‚…״© ״³״¯״§״³״© ״¹״´״±״©
    void setHex(const std::string& hex) {
        std::string h = hex;
        if (!h.empty() && h[0] == '#') h = h.substr(1);
        if (h.length() >= 6) {
            unsigned int val;
            std::stringstream ss;
            ss << std::hex << h;
            ss >> val;
            float r = ((val >> 16) & 0xFF) / 255.0f;
            float g = ((val >> 8) & 0xFF) / 255.0f;
            float b = (val & 0xFF) / 255.0f;
            float a = h.length() >= 8 ? ((val >> 24) & 0xFF) / 255.0f : 1.0f;
            setRGB(r, g, b, a);
        }
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„‚…״© ״§„״³״¯״§״³״© ״§„״¹״´״±״©
    std::string getHex() const {
        char buf[10];
        snprintf(buf, sizeof(buf), "#%02X%02X%02X",
            (int)(selectedColor.r * 255), (int)(selectedColor.g * 255), (int)(selectedColor.b * 255));
        return buf;
    }

private:
    /// ״¥״¶״§״© „ˆ† „„‚״§״¦…״© ״§„״£״®״±״©
    void addToRecent(const Color& c) {
        recentColors.insert(recentColors.begin(), c);
        if ((int)recentColors.size() > maxRecentColors)
            recentColors.resize(maxRecentColors);
    }
};

// =============================================================================
// 9. …†״×‚ ״§„״×״§״±״® ˆ״§„ˆ‚״× - DateTimePicker
// =============================================================================

/// ˆ״¶״¹ ״§„…†״×‚
enum class DateTimePickerMode {
    Date,       ///< ״×״§״±״® ‚״·
    Time,       ///< ˆ‚״× ‚״·
    DateTime,   ///< ״×״§״±״® ˆˆ‚״×
    DateRange   ///< †״·״§‚ ״×ˆ״§״±״®
};

/// †…״· ״§„…†״×‚
enum class DateTimePickerStyle {
    Spinner,    ///< ״¹״¬„״© ״¯ˆ‘״§״±״© (iOS style)
    Calendar,   ///< ״×‚ˆ… (Android style)
    Input       ///< ״¥״¯״®״§„ ״¯ˆ
};

/// …״±ƒ״¨ …†״×‚ ״§„״×״§״±״® ˆ״§„ˆ‚״×
class DateTimePicker : public Widget {
public:
    DateTimePickerMode mode = DateTimePickerMode::DateTime;
    DateTimePickerStyle style = DateTimePickerStyle::Spinner;
    int year = 2026, month = 1, day = 1;    ///< ״§„״×״§״±״®
    int hour = 12, minute = 0;              ///< ״§„ˆ‚״×
    int endYear = 2026, endMonth = 1, endDay = 1; ///< †‡״§״© ״§„†״·״§‚
    int minYear = 1900, maxYear = 2100;     ///< †״·״§‚ ״§„״³†ˆ״§״×
    bool use24Hour = true;                  ///< †״¸״§… 24 ״³״§״¹״©
    bool hijri = false;                     ///< ״§„״×‚ˆ… ״§„‡״¬״±
    Color accentColor = {0.2f, 0.6f, 1.0f, 1.0f};
    std::function<void(int,int,int,int,int)> onChanged; ///< ״¯״§„״© ״±״¯ (״³†״©,״´‡״±,ˆ…,״³״§״¹״©,״¯‚‚״©)

    /// ״¶״¨״· ״§„״×״§״±״®
    void setDate(int y, int m, int d) {
        year = y; month = m; day = d;
        notifyChange();
    }

    /// ״¶״¨״· ״§„ˆ‚״×
    void setTime(int h, int m) {
        hour = h; minute = m;
        notifyChange();
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„״×״§״±״® ƒ†״µ
    std::string getDateString() const {
        char buf[32];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
        return buf;
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„ˆ‚״× ƒ†״µ
    std::string getTimeString() const {
        char buf[16];
        if (use24Hour)
            snprintf(buf, sizeof(buf), "%02d:%02d", hour, minute);
        else {
            int h12 = hour % 12;
            if (h12 == 0) h12 = 12;
            snprintf(buf, sizeof(buf), "%02d:%02d %s", h12, minute, hour < 12 ? "״µ" : "…");
        }
        return buf;
    }

private:
    void notifyChange() {
        if (onChanged) onChanged(year, month, day, hour, minute);
    }
};

// =============================================================================
// 10. ״´״±״· ״¨״­״« …״×‚״¯… - SearchBar
// =============================================================================

/// ״§‚״×״±״§״­ ״¨״­״«
struct SearchSuggestion {
    std::string text;           ///< ״§„†״µ
    std::string subtitle;       ///< †״µ ״±״¹
    std::string icon;           ///< ״£‚ˆ†״©
    std::string category;       ///< ״§„״¦״©
    int priority = 0;           ///< ״§„״£ˆ„ˆ״©
};

/// …״±ƒ״¨ ״´״±״· ״§„״¨״­״« ״§„…״×‚״¯…
class SearchBar : public Widget {
public:
    std::string query;                          ///< †״µ ״§„״¨״­״« ״§„״­״§„
    std::string placeholder = "״§״¨״­״«...";        ///< †״µ ״§„״×„…״­
    std::vector<SearchSuggestion> suggestions;  ///< ״§„״§‚״×״±״§״­״§״×
    std::vector<std::string> searchHistory;     ///< ״³״¬„ ״§„״¨״­״«
    bool showSuggestions = true;                ///< ״¥״¸‡״§״± ״§„״§‚״×״±״§״­״§״×
    bool showHistory = true;                    ///< ״¥״¸‡״§״± ״§„״³״¬„
    bool showClearButton = true;                ///< ״¥״¸‡״§״± ״²״± ״§„…״³״­
    bool showFilterButton = false;              ///< ״¥״¸‡״§״± ״²״± ״§„״×״µ״©
    bool autoComplete = true;                   ///< ״¥ƒ…״§„ ״×„‚״§״¦
    int maxHistory = 20;                        ///< ״£‚״µ‰ ״¹״¯״¯  ״§„״³״¬„
    float debounceMs = 300.0f;                  ///< ״×״£״®״± ״§„״¨״­״« (…„„ ״«״§†״©)
    Color barColor = {0.15f, 0.15f, 0.2f, 1.0f};

    std::function<void(const std::string&)> onSearch;       ///< ״¹†״¯ ״§„״¨״­״«
    std::function<void(const std::string&)> onQueryChanged; ///< ״¹†״¯ ״×״÷״± ״§„†״µ
    std::function<void()> onFilterTapped;                   ///< ״¹†״¯ ״§„†‚״± ״¹„‰ ״§„״×״µ״©

    /// ״¶״¨״· †״µ ״§„״¨״­״«
    void setQuery(const std::string& text) {
        query = text;
        if (onQueryChanged) onQueryChanged(query);
    }

    /// ״×†״° ״§„״¨״­״«
    void search() {
        if (!query.empty()) {
            addToHistory(query);
            if (onSearch) onSearch(query);
        }
    }

    /// …״³״­ ״§„״¨״­״«
    void clear() {
        query.clear();
        suggestions.clear();
    }

    /// ״¥״¶״§״© ״§‚״×״±״§״­
    void addSuggestion(const SearchSuggestion& s) {
        suggestions.push_back(s);
    }

    /// …״³״­ ״§„״³״¬„
    void clearHistory() { searchHistory.clear(); }

private:
    /// ״¥״¶״§״© „„״³״¬„
    void addToHistory(const std::string& text) {
        // ״¥״²״§„״© ״§„״×ƒ״±״§״±
        auto it = std::find(searchHistory.begin(), searchHistory.end(), text);
        if (it != searchHistory.end()) searchHistory.erase(it);
        searchHistory.insert(searchHistory.begin(), text);
        if ((int)searchHistory.size() > maxHistory)
            searchHistory.resize(maxHistory);
    }
};

// =============================================================================
// 11. „״§״×״© - Banner
// =============================================================================

/// †ˆ״¹ ״§„„״§״×״©
enum class BannerType {
    Info,       ///< …״¹„ˆ…״§״×
    Success,    ///< †״¬״§״­
    Warning,    ///< ״×״­״°״±
    Error       ///< ״®״·״£
};

/// …ˆ״¶״¹ ״§„„״§״×״©
enum class BannerPosition {
    Top,        ///< ״£״¹„‰ ״§„״´״§״´״©
    Bottom      ///< ״£״³„ ״§„״´״§״´״©
};

/// …״±ƒ״¨ ״§„„״§״×״© (״¥״´״¹״§״± ״¯״§״®„ ״§„״×״·״¨‚)
class Banner : public Widget {
public:
    std::string message;                ///< ״§„״±״³״§„״©
    std::string actionText;             ///< †״µ ״§„״¥״¬״±״§״¡
    BannerType bannerType = BannerType::Info;
    BannerPosition position = BannerPosition::Top;
    bool dismissible = true;            ///< ‚״§״¨„״© „„״¥״÷„״§‚
    float displayDuration = 3000.0f;    ///< …״¯״© ״§„״¹״±״¶ (…„„ ״«״§†״©״ 0 = ״¯״§״¦…״©)
    bool showIcon = true;               ///< ״¥״¸‡״§״± ״£‚ˆ†״© ״­״³״¨ ״§„†ˆ״¹
    float animationDuration = 300.0f;   ///< …״¯״© ״­״±ƒ״© ״§„״¸‡ˆ״±/״§„״§״®״×״§״¡
    float currentTimer = 0.0f;          ///< ״§„…״₪‚״× ״§„״­״§„

    std::function<void()> onAction;     ///< ״¹†״¯ ״§„†‚״± ״¹„‰ ״§„״¥״¬״±״§״¡
    std::function<void()> onDismissed;  ///< ״¹†״¯ ״§„״¥״÷„״§‚

    /// ״¥״¸‡״§״± ״§„„״§״×״©
    void show(const std::string& msg, BannerType type = BannerType::Info) {
        message = msg;
        bannerType = type;
        visible = true;
        currentTimer = 0.0f;
    }

    /// ״¥״®״§״¡ ״§„„״§״×״©
    void dismiss() {
        visible = false;
        if (onDismissed) onDismissed();
    }

    /// ״×״­״¯״« („„…״₪‚״×)
    void update(float deltaTime) {
        if (visible && displayDuration > 0) {
            currentTimer += deltaTime * 1000.0f;
            if (currentTimer >= displayDuration) dismiss();
        }
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„„ˆ† ״­״³״¨ ״§„†ˆ״¹
    Color getTypeColor() const {
        switch (bannerType) {
            case BannerType::Info:    return {0.2f, 0.6f, 1.0f, 1.0f};
            case BannerType::Success: return {0.2f, 0.8f, 0.4f, 1.0f};
            case BannerType::Warning: return {1.0f, 0.8f, 0.0f, 1.0f};
            case BannerType::Error:   return {1.0f, 0.3f, 0.3f, 1.0f};
            default: return {0.5f, 0.5f, 0.5f, 1.0f};
        }
    }
};

// =============================================================================
// 12. ״´״±״­״© - Chip / ChipGroup
// =============================================================================

/// †ˆ״¹ ״§„״´״±״­״©
enum class ChipType {
    Action,     ///< ״´״±״­״© ״¥״¬״±״§״¡
    Filter,     ///< ״´״±״­״© ״×״µ״©
    Input,      ///< ״´״±״­״© ״¥״¯״®״§„
    Choice      ///< ״´״±״­״© ״§״®״×״§״±
};

/// ״´״±״­״© (Chip) - ״¹†״µ״± …״¯…״¬ …״«„ ״³…״© ״£ˆ ״¥״¬״±״§״¡
class Chip : public Widget {
public:
    std::string label;                  ///< ״§„†״µ
    std::string avatar;                 ///< ״µˆ״±״© ״±…״²״© (…״³״§״±)
    ChipType chipType = ChipType::Action;
    bool selected = false;              ///< …״®״×״§״±״©
    bool deletable = false;             ///< ‚״§״¨„״© „„״­״°
    float chipHeight = 32.0f;           ///< ״§״±״×״§״¹ ״§„״´״±״­״©
    float cornerRadius = 16.0f;         ///< †״µ ‚״·״± ״§„״²ˆ״§״§
    Color selectedBg = {0.2f, 0.5f, 1.0f, 0.3f};
    Color normalBg = {0.2f, 0.2f, 0.25f, 1.0f};
    Color textColor = {0.9f, 0.9f, 0.9f, 1.0f};

    std::function<void()> onTap;        ///< ״¹†״¯ ״§„†‚״±
    std::function<void()> onDelete;     ///< ״¹†״¯ ״§„״­״°

    /// ״×״¨״¯„ ״§„״§״®״×״§״±
    void toggle() {
        selected = !selected;
        if (onTap) onTap();
    }
};

/// …״¬…ˆ״¹״© ״´״±״§״¦״­
class ChipGroup : public Widget {
public:
    std::vector<Chip> chips;            ///< ״§„״´״±״§״¦״­
    bool singleSelection = false;       ///< ״§״®״×״§״± ˆ״§״­״¯ ‚״·
    float spacing = 8.0f;              ///< ״§„…״³״§״© ״¨† ״§„״´״±״§״¦״­
    float rowSpacing = 8.0f;           ///< ״§„…״³״§״© ״¨† ״§„״µˆ
    bool wrap = true;                   ///< ״§„״×״§ ״×„‚״§״¦

    std::function<void(int)> onChipSelected;    ///< ״¹†״¯ ״§״®״×״§״± ״´״±״­״©
    std::function<void(int)> onChipDeleted;     ///< ״¹†״¯ ״­״° ״´״±״­״©

    /// ״¥״¶״§״© ״´״±״­״©
    void addChip(const std::string& label, ChipType type = ChipType::Action) {
        Chip c;
        c.label = label;
        c.chipType = type;
        chips.push_back(c);
    }

    /// ״§״®״×״§״± ״´״±״­״©
    void selectChip(int index) {
        if (index < 0 || index >= (int)chips.size()) return;
        if (singleSelection) {
            for (auto& c : chips) c.selected = false;
        }
        chips[index].selected = !chips[index].selected;
        if (onChipSelected) onChipSelected(index);
    }

    /// ״­״° ״´״±״­״©
    void removeChip(int index) {
        if (index >= 0 && index < (int)chips.size()) {
            chips.erase(chips.begin() + index);
            if (onChipDeleted) onChipDeleted(index);
        }
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„״´״±״§״¦״­ ״§„…״®״×״§״±״©
    std::vector<int> getSelectedIndices() const {
        std::vector<int> result;
        for (int i = 0; i < (int)chips.size(); i++)
            if (chips[i].selected) result.push_back(i);
        return result;
    }
};

// =============================================================================
// 13. ˆ״±‚״© ״³„״© - BottomSheet
// =============================================================================

/// ״­״§„״© ״§„ˆ״±‚״© ״§„״³„״©
enum class BottomSheetState {
    Hidden,     ///< …״®״©
    Collapsed,  ///< …״·ˆ״© (״¬״²״¡ ״µ״÷״± ״¸״§‡״±)
    HalfExpanded, ///< †״µ ……״×״¯״©
    Expanded    ///< ……״×״¯״© ״¨״§„ƒ״§…„
};

/// …״±ƒ״¨ ״§„ˆ״±‚״© ״§„״³„״©
class BottomSheet : public Widget {
public:
    BottomSheetState state = BottomSheetState::Hidden;
    float collapsedHeight = 100.0f;     ///< ״§״±״×״§״¹ ״§„…״·ˆ״©
    float halfHeight = 0.5f;            ///< †״³״¨״© ״§„†״µ (0-1)
    float expandedHeight = 0.9f;        ///< †״³״¨״© ״§„……״×״¯״© (0-1)
    bool draggable = true;              ///< ‚״§״¨„״© „„״³״­״¨
    bool dismissOnTapOutside = true;    ///< ״¥״÷„״§‚ ״¹†״¯ ״§„†‚״± ״®״§״±״¬‡״§
    bool showHandle = true;             ///< ״¥״¸‡״§״± …‚״¨״¶ ״§„״³״­״¨
    float handleWidth = 40.0f;          ///< ״¹״±״¶ ״§„…‚״¨״¶
    float handleHeight = 4.0f;          ///< ״§״±״×״§״¹ ״§„…‚״¨״¶
    Color sheetColor = {0.15f, 0.15f, 0.2f, 1.0f};
    Color handleColor = {0.4f, 0.4f, 0.45f, 1.0f};
    Color scrimColor = {0.0f, 0.0f, 0.0f, 0.5f};  ///< „ˆ† ״§„״®„״© ״§„…״¹״×…״©
    float cornerRadius = 16.0f;
    float currentHeight = 0.0f;         ///< ״§„״§״±״×״§״¹ ״§„״­״§„ („„״­״±ƒ״©)

    std::function<void(BottomSheetState)> onStateChanged;
    std::vector<std::shared_ptr<MobileWidget>> children; ///< ״§„…״­״×ˆ״§״×

    /// ״×״­ ״§„ˆ״±‚״©
    void expand() {
        setState(BottomSheetState::Expanded);
    }

    /// ״· ״§„ˆ״±‚״©
    void collapse() {
        setState(BottomSheetState::Collapsed);
    }

    /// †״µ ״×ˆ״³״¹
    void halfExpand() {
        setState(BottomSheetState::HalfExpanded);
    }

    /// ״¥״®״§״¡ ״§„ˆ״±‚״©
    void hide() {
        setState(BottomSheetState::Hidden);
    }

    /// ״¶״¨״· ״§„״­״§„״©
    void setState(BottomSheetState newState) {
        state = newState;
        visible = (state != BottomSheetState::Hidden);
        if (onStateChanged) onStateChanged(state);
    }

    /// ״¥״¶״§״© …״±ƒ״¨ ״·„
    void addChild(std::shared_ptr<MobileWidget> child) {
        children.push_back(child);
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„״§״±״×״§״¹ ״§„…״³״×‡״¯
    float getTargetHeight(float screenHeight) const {
        switch (state) {
            case BottomSheetState::Hidden:       return 0;
            case BottomSheetState::Collapsed:    return collapsedHeight;
            case BottomSheetState::HalfExpanded: return screenHeight * halfHeight;
            case BottomSheetState::Expanded:     return screenHeight * expandedHeight;
            default: return 0;
        }
    }
};

// =============================================================================
// 14. ‚״§״¦…״© ״³״±״¹״© - SpeedDial (FAB ……״×״¯)
// =============================================================================

/// ״¹†״µ״±  ״§„‚״§״¦…״© ״§„״³״±״¹״©
struct SpeedDialItem {
    std::string label;          ///< ״§„״×״³…״©
    std::string icon;           ///< ״§„״£‚ˆ†״©
    Color color = {0.2f, 0.6f, 1.0f, 1.0f}; ///< ״§„„ˆ†
    std::function<void()> onTap; ///< ״¹†״¯ ״§„†‚״±
    bool visible = true;        ///< …״±״¦
};

/// …״±ƒ״¨ ״§„‚״§״¦…״© ״§„״³״±״¹״© (FAB …״¹ ״®״§״±״§״×)
class SpeedDial : public Widget {
public:
    std::vector<SpeedDialItem> items;   ///< ״§„״¹†״§״µ״±
    bool isOpen = false;                ///< …״×ˆ״­״©
    float fabSize = 56.0f;              ///< ״­״¬… ״§„״²״± ״§„״±״¦״³
    float miniFabSize = 40.0f;          ///< ״­״¬… ״§„״£״²״±״§״± ״§„״±״¹״©
    float spacing = 16.0f;             ///< ״§„…״³״§״© ״¨† ״§„״£״²״±״§״±
    Color fabColor = {0.2f, 0.6f, 1.0f, 1.0f};
    Color fabIconColor = {1.0f, 1.0f, 1.0f, 1.0f};
    Color scrimColor = {0.0f, 0.0f, 0.0f, 0.3f};
    float animationProgress = 0.0f;     ///< ״×‚״¯… ״§„״­״±ƒ״©
    bool showLabels = true;             ///< ״¥״¸‡״§״± ״§„״×״³…״§״×
    bool closeOnSelect = true;          ///< ״¥״÷„״§‚ ״¹†״¯ ״§„״§״®״×״§״±

    std::function<void()> onMainTap;    ///< ״¹†״¯ ״§„†‚״± ״¹„‰ ״§„״²״± ״§„״±״¦״³
    std::function<void(bool)> onToggle; ///< ״¹†״¯ ״§„״×״­/״§„״¥״÷„״§‚

    /// ״×״­/״¥״÷„״§‚
    void toggle() {
        isOpen = !isOpen;
        if (onToggle) onToggle(isOpen);
    }

    /// ״×״­
    void open() { if (!isOpen) toggle(); }

    /// ״¥״÷„״§‚
    void close() { if (isOpen) toggle(); }

    /// ״¥״¶״§״© ״¹†״µ״±
    void addItem(const std::string& label, const std::string& icon,
                 std::function<void()> action, const Color& color = {0.3f, 0.3f, 0.35f, 1.0f}) {
        items.push_back({label, icon, color, action, true});
    }

    /// ״×״­״¯״« ״§„״­״±ƒ״©
    void updateAnimation(float dt) {
        float target = isOpen ? 1.0f : 0.0f;
        float speed = 5.0f;
        if (animationProgress < target)
            animationProgress = std::min(target, animationProgress + dt * speed);
        else if (animationProgress > target)
            animationProgress = std::max(target, animationProgress - dt * speed);
    }
};

// =============================================================================
// 15. ״¹״±״¶ ״´״¬״± - TreeView
// =============================================================================

/// ״¹‚״¯״©  ״§„״´״¬״±״©
struct TreeNode {
    std::string id;                             ///< …״¹״± ״±״¯
    std::string label;                          ///< ״§„†״µ
    std::string icon;                           ///< ״§„״£‚ˆ†״©
    bool expanded = false;                      ///< …ˆ״³״¹״©
    bool selected = false;                      ///< …״®״×״§״±״©
    bool hasChildren = false;                   ///< „‡״§ ״£״¨†״§״¡
    std::vector<std::shared_ptr<TreeNode>> children; ///< ״§„״£״¨†״§״¡
    void* userData = nullptr;                   ///< ״¨״§†״§״× …״®״µ״µ״©

    /// ״¥״¶״§״© ״§״¨†
    std::shared_ptr<TreeNode> addChild(const std::string& childId, const std::string& childLabel) {
        auto child = std::make_shared<TreeNode>();
        child->id = childId;
        child->label = childLabel;
        children.push_back(child);
        hasChildren = true;
        return child;
    }

    /// ״×״¨״¯„ ״§„״×ˆ״³״¹
    void toggle() { expanded = !expanded; }
};

/// …״±ƒ״¨ ״§„״¹״±״¶ ״§„״´״¬״±
class TreeView : public Widget {
public:
    std::vector<std::shared_ptr<TreeNode>> roots; ///< ״§„״¬״°ˆ״±
    float indentWidth = 24.0f;          ///< ״¹״±״¶ ״§„…״³״§״© ״§„״¨״§״¯״¦״©
    float nodeHeight = 40.0f;           ///< ״§״±״×״§״¹ ״§„״¹‚״¯״©
    bool singleExpand = false;          ///< ״×ˆ״³״¹ ״¹‚״¯״© ˆ״§״­״¯״© ‚״·
    bool showLines = true;              ///< ״¥״¸‡״§״± ״®״·ˆ״· ״§„״±״¨״·
    bool showIcons = true;              ///< ״¥״¸‡״§״± ״§„״£‚ˆ†״§״×
    Color selectedColor = {0.2f, 0.5f, 1.0f, 0.3f};
    Color lineColor = {0.3f, 0.3f, 0.35f, 0.5f};

    std::function<void(const std::string&)> onNodeSelected;  ///< ״¹†״¯ ״§״®״×״§״± ״¹‚״¯״©
    std::function<void(const std::string&)> onNodeExpanded;  ///< ״¹†״¯ ״×ˆ״³״¹ ״¹‚״¯״©
    std::function<void(const std::string&)> onNodeCollapsed; ///< ״¹†״¯ ״· ״¹‚״¯״©

    /// ״¥״¶״§״© ״¬״°״±
    std::shared_ptr<TreeNode> addRoot(const std::string& id, const std::string& label) {
        auto root = std::make_shared<TreeNode>();
        root->id = id;
        root->label = label;
        roots.push_back(root);
        return root;
    }

    /// ״×ˆ״³״¹ ״§„ƒ„
    void expandAll() {
        for (auto& r : roots) expandRecursive(r);
    }

    /// ״· ״§„ƒ„
    void collapseAll() {
        for (auto& r : roots) collapseRecursive(r);
    }

    /// ״§„״¨״­״« ״¹† ״¹‚״¯״©
    std::shared_ptr<TreeNode> findNode(const std::string& id) const {
        for (auto& r : roots) {
            auto found = findNodeRecursive(r, id);
            if (found) return found;
        }
        return nullptr;
    }

    /// ״§״®״×״§״± ״¹‚״¯״©
    void selectNode(const std::string& id) {
        // ״¥„״÷״§״¡ ״§״®״×״§״± ״§„ƒ„ ״£ˆ„״§‹
        deselectAll();
        auto node = findNode(id);
        if (node) {
            node->selected = true;
            if (onNodeSelected) onNodeSelected(id);
        }
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״¹״¯״¯ ״§„״¹‚״¯ ״§„ƒ„
    int getTotalNodeCount() const {
        int count = 0;
        for (auto& r : roots) count += countRecursive(r);
        return count;
    }

private:
    void expandRecursive(std::shared_ptr<TreeNode>& node) {
        node->expanded = true;
        for (auto& c : node->children) expandRecursive(c);
    }

    void collapseRecursive(std::shared_ptr<TreeNode>& node) {
        node->expanded = false;
        for (auto& c : node->children) collapseRecursive(c);
    }

    void deselectAll() {
        for (auto& r : roots) deselectRecursive(r);
    }

    void deselectRecursive(std::shared_ptr<TreeNode>& node) {
        node->selected = false;
        for (auto& c : node->children) deselectRecursive(c);
    }

    std::shared_ptr<TreeNode> findNodeRecursive(const std::shared_ptr<TreeNode>& node,
                                                 const std::string& id) const {
        if (node->id == id) return node;
        for (auto& c : node->children) {
            auto found = findNodeRecursive(c, id);
            if (found) return found;
        }
        return nullptr;
    }

    int countRecursive(const std::shared_ptr<TreeNode>& node) const {
        int count = 1;
        for (auto& c : node->children) count += countRecursive(c);
        return count;
    }
};

// =============================================================================
// 16. ״¬״¯ˆ„ ״¨״§†״§״× - DataTable
// =============================================================================

/// …״­״§״°״§״© ״§„״¹…ˆ״¯
enum class ColumnAlign {
    Start,      ///< ״¨״¯״§״©
    Center,     ///< ˆ״³״·
    End         ///< †‡״§״©
};

/// †ˆ״¹ ״§„״×״±״×״¨
enum class SortOrder {
    None,       ///< ״¨״¯ˆ† ״×״±״×״¨
    Ascending,  ///< ״×״µ״§״¹״¯
    Descending  ///< ״×†״§״²„
};

/// ״×״¹״± ״¹…ˆ״¯
struct DataColumn {
    std::string id;             ///< …״¹״± ״§„״¹…ˆ״¯
    std::string label;          ///< ״¹†ˆ״§† ״§„״¹…ˆ״¯
    float width = 100.0f;       ///< ״§„״¹״±״¶
    float minWidth = 50.0f;     ///< ״§„״­״¯ ״§„״£״¯†‰ „„״¹״±״¶
    bool sortable = true;       ///< ‚״§״¨„ „„״×״±״×״¨
    bool resizable = true;      ///< ‚״§״¨„ „״×״÷״± ״§„״­״¬…
    ColumnAlign align = ColumnAlign::Start;
    SortOrder sortOrder = SortOrder::None;
};

/// ״®„״©  ״§„״¬״¯ˆ„
struct DataCell {
    std::string text;           ///< ״§„†״µ
    Color textColor = {0.9f, 0.9f, 0.9f, 1.0f};
    Color bgColor = {0, 0, 0, 0};  ///< ״®„״© …״®״µ״µ״© (״´״§ = ״§״×״±״§״¶)
    bool editable = false;      ///< ‚״§״¨„״© „„״×״­״±״±
};

/// ״µ  ״§„״¬״¯ˆ„
struct DataRow {
    std::string id;                     ///< …״¹״± ״§„״µ
    std::vector<DataCell> cells;        ///< ״§„״®„״§״§
    bool selected = false;              ///< …״®״×״§״±
    bool highlighted = false;           ///< ……״²
};

/// …״±ƒ״¨ ״¬״¯ˆ„ ״§„״¨״§†״§״×
class DataTable : public Widget {
public:
    std::vector<DataColumn> columns;    ///< ״§„״£״¹…״¯״©
    std::vector<DataRow> rows;          ///< ״§„״µˆ
    float headerHeight = 48.0f;        ///< ״§״±״×״§״¹ ״§„״±״£״³
    float rowHeight = 44.0f;           ///< ״§״±״×״§״¹ ״§„״µ
    bool showCheckboxes = false;        ///< ״¥״¸‡״§״± …״±״¨״¹״§״× ״§״®״×״§״±
    bool singleSelection = false;       ///< ״§״®״×״§״± ˆ״§״­״¯ ‚״·
    bool striped = true;                ///< ״µˆ …״×״¨״§״¯„״© ״§„״£„ˆ״§†
    bool hoverable = true;              ///< ״×…״² ״¹†״¯ ״§„״×…״±״±
    bool paginated = false;             ///< …‚״³‘… „״µ״­״§״×
    int pageSize = 20;                  ///< ״­״¬… ״§„״µ״­״©
    int currentPage = 0;               ///< ״§„״µ״­״© ״§„״­״§„״©
    float scrollY = 0.0f;             ///< ״×…״±״± ״±״£״³
    Color headerBg = {0.15f, 0.15f, 0.2f, 1.0f};
    Color rowBg = {0.1f, 0.1f, 0.14f, 1.0f};
    Color altRowBg = {0.12f, 0.12f, 0.17f, 1.0f};
    Color selectedBg = {0.2f, 0.4f, 0.8f, 0.3f};
    Color hoverBg = {0.2f, 0.2f, 0.25f, 0.5f};
    Color borderColor = {0.25f, 0.25f, 0.3f, 1.0f};

    std::function<void(int, int)> onCellTap;        ///< ״¹†״¯ ״§„†‚״± ״¹„‰ ״®„״© (״µ, ״¹…ˆ״¯)
    std::function<void(int)> onRowSelected;          ///< ״¹†״¯ ״§״®״×״§״± ״µ
    std::function<void(const std::string&, SortOrder)> onSort; ///< ״¹†״¯ ״§„״×״±״×״¨

    /// ״¥״¶״§״© ״¹…ˆ״¯
    void addColumn(const std::string& id, const std::string& label, float width = 100.0f) {
        columns.push_back({id, label, width, 50.0f, true, true, ColumnAlign::Start, SortOrder::None});
    }

    /// ״¥״¶״§״© ״µ
    void addRow(const std::string& id, const std::vector<std::string>& cellTexts) {
        DataRow row;
        row.id = id;
        for (auto& text : cellTexts) {
            row.cells.push_back({text, {0.9f,0.9f,0.9f,1}, {0,0,0,0}, false});
        }
        rows.push_back(row);
    }

    /// ״×״±״×״¨ ״­״³״¨ ״¹…ˆ״¯
    void sortByColumn(int colIndex) {
        if (colIndex < 0 || colIndex >= (int)columns.size()) return;
        auto& col = columns[colIndex];
        // ״×״¨״¯„ ״§„״×״±״×״¨
        if (col.sortOrder == SortOrder::Ascending) col.sortOrder = SortOrder::Descending;
        else col.sortOrder = SortOrder::Ascending;
        // ״¥״¹״§״¯״© ״×״±״×״¨ ״§„״µˆ
        std::sort(rows.begin(), rows.end(), [&](const DataRow& a, const DataRow& b) {
            if (colIndex >= (int)a.cells.size() || colIndex >= (int)b.cells.size()) return false;
            if (col.sortOrder == SortOrder::Ascending)
                return a.cells[colIndex].text < b.cells[colIndex].text;
            else
                return a.cells[colIndex].text > b.cells[colIndex].text;
        });
        if (onSort) onSort(col.id, col.sortOrder);
    }

    /// ״§״®״×״§״± ״µ
    void selectRow(int index) {
        if (index < 0 || index >= (int)rows.size()) return;
        if (singleSelection) for (auto& r : rows) r.selected = false;
        rows[index].selected = !rows[index].selected;
        if (onRowSelected) onRowSelected(index);
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״§„״µˆ ״§„…״®״×״§״±״©
    std::vector<int> getSelectedRows() const {
        std::vector<int> result;
        for (int i = 0; i < (int)rows.size(); i++)
            if (rows[i].selected) result.push_back(i);
        return result;
    }

    /// ״¹״¯״¯ ״§„״µ״­״§״×
    int getPageCount() const {
        return pageSize > 0 ? ((int)rows.size() + pageSize - 1) / pageSize : 1;
    }

    /// ״§„״°‡״§״¨ „״µ״­״©
    void goToPage(int page) {
        currentPage = std::max(0, std::min(page, getPageCount() - 1));
    }

    /// …״³״­ ״§„״¬״¯ˆ„
    void clear() {
        rows.clear();
    }
};

// =============================================================================
// 17. ״®״· ״²…† - Timeline
// =============================================================================

/// ״¹†״µ״±  ״§„״®״· ״§„״²…†
struct TimelineItem {
    std::string title;          ///< ״§„״¹†ˆ״§†
    std::string subtitle;       ///< ״§„״¹†ˆ״§† ״§„״±״¹
    std::string time;           ///< ״§„ˆ‚״×/״§„״×״§״±״®
    std::string description;    ///< ״§„ˆ״µ
    std::string icon;           ///< ״§„״£‚ˆ†״©
    Color dotColor = {0.2f, 0.6f, 1.0f, 1.0f}; ///< „ˆ† ״§„†‚״·״©
    bool completed = false;     ///< …ƒ״×…„
    bool active = false;        ///< †״´״· ״­״§„״§‹
};

/// ״§״×״¬״§‡ ״§„״®״· ״§„״²…†
enum class TimelineOrientation {
    Vertical,       ///< ״±״£״³
    Horizontal      ///< ״£‚
};

/// †…״· ״§„״®״· ״§„״²…†
enum class TimelineStyle {
    Simple,         ///< ״¨״³״·
    Alternating,    ///< …״×״¨״§״¯„ (…†/״³״§״±)
    Compact         ///< …״¶״÷ˆ״·
};

/// …״±ƒ״¨ ״§„״®״· ״§„״²…†
class Timeline : public Widget {
public:
    std::vector<TimelineItem> items;            ///< ״§„״¹†״§״µ״±
    TimelineOrientation orientation = TimelineOrientation::Vertical;
    TimelineStyle timelineStyle = TimelineStyle::Simple;
    float dotRadius = 8.0f;                     ///< †״µ ‚״·״± ״§„†‚״·״©
    float lineWidth = 2.0f;                     ///< ״³…ƒ ״§„״®״·
    float itemSpacing = 24.0f;                  ///< ״§„…״³״§״© ״¨† ״§„״¹†״§״µ״±
    Color lineColor = {0.3f, 0.3f, 0.35f, 0.8f};
    Color completedColor = {0.2f, 0.8f, 0.4f, 1.0f};
    Color activeColor = {0.2f, 0.6f, 1.0f, 1.0f};
    Color inactiveColor = {0.4f, 0.4f, 0.45f, 0.6f};
    bool animated = true;                       ///< ״±״³ˆ… …״×״­״±ƒ״©

    std::function<void(int)> onItemTap;         ///< ״¹†״¯ ״§„†‚״± ״¹„‰ ״¹†״µ״±

    /// ״¥״¶״§״© ״¹†״µ״±
    void addItem(const std::string& title, const std::string& time,
                 const std::string& desc = "", bool completed = false) {
        items.push_back({title, "", time, desc, "", {0.2f,0.6f,1.0f,1.0f}, completed, false});
    }

    /// ״×״¹† ״§„״¹†״µ״± ״§„†״´״·
    void setActive(int index) {
        for (auto& item : items) item.active = false;
        if (index >= 0 && index < (int)items.size())
            items[index].active = true;
    }

    /// ״§„״­״µˆ„ ״¹„‰ †״³״¨״© ״§„״§ƒ״×…״§„
    float getCompletionRate() const {
        if (items.empty()) return 0;
        int completed = 0;
        for (auto& item : items) if (item.completed) completed++;
        return (float)completed / (float)items.size();
    }
};

// =============================================================================
// 18. „ˆ״­״© ƒ״§†״¨״§† - KanbanBoard
// =============================================================================

/// ״¨״·״§‚״© ƒ״§†״¨״§†
struct KanbanCard {
    std::string id;             ///< ״§„…״¹״±
    std::string title;          ///< ״§„״¹†ˆ״§†
    std::string description;    ///< ״§„ˆ״µ
    std::string assignee;       ///< ״§„…״³†״¯ ״¥„‡
    Color labelColor = {0.2f, 0.6f, 1.0f, 1.0f}; ///< „ˆ† ״§„״×״³…״©
    int priority = 0;           ///< ״§„״£ˆ„ˆ״© (0=״¹״§״¯״ 1=…‡…״ 2=״¹״§״¬„)
    std::string dueDate;        ///< ״×״§״±״® ״§„״§״³״×״­‚״§‚
    std::vector<std::string> tags; ///< ״§„״¹„״§…״§״×
};

/// ״¹…ˆ״¯ ƒ״§†״¨״§†
struct KanbanColumn {
    std::string id;                     ///< ״§„…״¹״±
    std::string title;                  ///< ״§„״¹†ˆ״§†
    Color headerColor = {0.2f, 0.2f, 0.25f, 1.0f};
    int maxCards = 0;                   ///< ״§„״­״¯ ״§„״£‚״µ‰ „„״¨״·״§‚״§״× (0 = ״÷״± …״­״¯ˆ״¯)
    std::vector<KanbanCard> cards;      ///< ״§„״¨״·״§‚״§״×
};

/// …״±ƒ״¨ „ˆ״­״© ƒ״§†״¨״§† - „״¥״¯״§״±״© ״§„…‡״§… ״¨״µ״±״§‹
class KanbanBoard : public Widget {
public:
    std::vector<KanbanColumn> columns;  ///< ״§„״£״¹…״¯״©
    float columnWidth = 280.0f;         ///< ״¹״±״¶ ״§„״¹…ˆ״¯
    float cardHeight = 100.0f;          ///< ״§״±״×״§״¹ ״§„״¨״·״§‚״©
    float cardSpacing = 8.0f;           ///< ״§„…״³״§״© ״¨† ״§„״¨״·״§‚״§״×
    float columnSpacing = 12.0f;        ///< ״§„…״³״§״© ״¨† ״§„״£״¹…״¯״©
    float headerHeight = 44.0f;         ///< ״§״±״×״§״¹ ״±״£״³ ״§„״¹…ˆ״¯
    Color boardBg = {0.08f, 0.08f, 0.1f, 1.0f};
    Color cardBg = {0.15f, 0.15f, 0.2f, 1.0f};
    bool dragEnabled = true;            ///< ״×…ƒ† ״§„״³״­״¨ ˆ״§„״¥„״§״×
    float scrollX = 0.0f;              ///< ״×…״±״± ״£‚

    std::function<void(const std::string&, const std::string&, const std::string&)> onCardMoved;
    std::function<void(const std::string&)> onCardTap;

    /// ״¥״¶״§״© ״¹…ˆ״¯
    void addColumn(const std::string& id, const std::string& title, int maxCards = 0) {
        columns.push_back({id, title, {0.2f,0.2f,0.25f,1.0f}, maxCards, {}});
    }

    /// ״¥״¶״§״© ״¨״·״§‚״© „״¹…ˆ״¯
    bool addCard(const std::string& columnId, const KanbanCard& card) {
        for (auto& col : columns) {
            if (col.id == columnId) {
                if (col.maxCards > 0 && (int)col.cards.size() >= col.maxCards)
                    return false; // ״§„״¹…ˆ״¯ ……״×„״¦
                col.cards.push_back(card);
                return true;
            }
        }
        return false;
    }

    /// †‚„ ״¨״·״§‚״© ״¨† ״£״¹…״¯״©
    bool moveCard(const std::string& cardId, const std::string& fromCol, const std::string& toCol) {
        KanbanCard card;
        bool found = false;
        // ״§„״¨״­״« ״¹† ״§„״¨״·״§‚״© ˆ״¥״²״§„״×‡״§
        for (auto& col : columns) {
            if (col.id == fromCol) {
                for (auto it = col.cards.begin(); it != col.cards.end(); ++it) {
                    if (it->id == cardId) {
                        card = *it;
                        col.cards.erase(it);
                        found = true;
                        break;
                    }
                }
                break;
            }
        }
        if (!found) return false;
        // ״¥״¶״§״© „„״¹…ˆ״¯ ״§„״¬״¯״¯
        for (auto& col : columns) {
            if (col.id == toCol) {
                if (col.maxCards > 0 && (int)col.cards.size() >= col.maxCards)
                    return false;
                col.cards.push_back(card);
                if (onCardMoved) onCardMoved(cardId, fromCol, toCol);
                return true;
            }
        }
        return false;
    }

    /// ״§„״­״µˆ„ ״¹„‰ ״¹״¯״¯ ״§„״¨״·״§‚״§״× ״§„ƒ„
    int getTotalCards() const {
        int count = 0;
        for (auto& col : columns) count += (int)col.cards.size();
        return count;
    }

    /// ״§„״¨״­״« ״¹† ״¨״·״§‚״©
    KanbanCard* findCard(const std::string& cardId) {
        for (auto& col : columns)
            for (auto& card : col.cards)
                if (card.id == cardId) return &card;
        return nullptr;
    }
};

} // namespace mobile
} // namespace ui
} // namespace graphics
} // namespace sad
