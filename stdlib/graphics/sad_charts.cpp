// ═══════════════════════════════════════════════════════════════════════════════
// تطبيق نظام المخططات البيانية — لغة ص
// Charts & Data Visualization System Implementation — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يحتوي على التطبيق الكامل لنظام المخططات البيانية.
// يستخدم مكتبة SDL2 لرسم المخططات على الشاشة.
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_charts.h"
#include <SDL.h>
#include <cmath>
#include <algorithm>
#include <numeric>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace sad { namespace charts {

// ═══════════════════════════════════════════════════════════════════
// أنواع المخططات — تحديد نوع كل مخطط
// ═══════════════════════════════════════════════════════════════════
enum class ChartType {
    Bar,        // مخطط أعمدة
    Line,       // مخطط خطي
    Pie,        // مخطط دائري
    Radar,      // مخطط رادار/عنكبوتي
    Gauge       // مؤشر/عداد
};

// ═══════════════════════════════════════════════════════════════════
// بيانات العداد/المؤشر
// ═══════════════════════════════════════════════════════════════════
struct GaugeData {
    float currentValue = 0;   // القيمة الحالية
    float minValue = 0;       // الحد الأدنى
    float maxValue = 100;     // الحد الأقصى
};

// ═══════════════════════════════════════════════════════════════════
// هيكل المخطط الموحّد — يحتوي على كل البيانات الممكنة
// ═══════════════════════════════════════════════════════════════════
struct Chart {
    ChartType type;                         // نوع المخطط
    ChartConfig config;                     // الإعدادات العامة
    std::vector<BarData> bars;              // بيانات الأعمدة
    std::vector<DataPoint> points;          // نقاط المخطط الخطي
    std::vector<PieSlice> slices;           // شرائح المخطط الدائري
    std::vector<RadarAxis> radarAxes;       // محاور الرادار
    std::vector<float> radarValues;         // قيم الرادار
    GaugeData gauge;                        // بيانات العداد
    ChartColor lineColor = {100, 200, 255, 255};  // لون الخط الافتراضي
    float lineThickness = 2.0f;            // سمك الخطوط
};

// ═══════════════════════════════════════════════════════════════════
// الحالة العامة — تخزين جميع المخططات
// ═══════════════════════════════════════════════════════════════════
static std::unordered_map<int, Chart> g_charts;   // خريطة المخططات بحسب المعرّف
static int g_nextChartId = 1;                      // المعرّف التالي — تزايد تلقائي

// ═══════════════════════════════════════════════════════════════════
// دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════

// البحث عن مخطط بمعرّفه — يُرجع nullptr إذا لم يُوجد
static Chart* findChart(int id) {
    auto it = g_charts.find(id);
    if (it == g_charts.end()) return nullptr;
    return &it->second;
}

// تعيين لون الرسم على SDL_Renderer
static void setColor(SDL_Renderer* r, const ChartColor& c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
}

// رسم مستطيل مملوء
static void fillRect(SDL_Renderer* r, int x, int y, int w, int h, const ChartColor& c) {
    setColor(r, c);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(r, &rect);
}

// رسم خط مع سمك (تقريب بسيط بخطوط متجاورة)
static void drawThickLine(SDL_Renderer* r, int x1, int y1, int x2, int y2, float thickness) {
    // رسم خطوط متوازية لمحاكاة السمك
    int t = static_cast<int>(thickness / 2.0f);
    if (t < 1) t = 1;
    for (int i = -t; i <= t; ++i) {
        SDL_RenderDrawLine(r, x1, y1 + i, x2, y2 + i);
        if (t > 1) {
            SDL_RenderDrawLine(r, x1 + i, y1, x2 + i, y2);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم الخلفية والشبكة — مشترك لجميع المخططات
// ═══════════════════════════════════════════════════════════════════
static void renderBackground(SDL_Renderer* r, const ChartConfig& cfg) {
    // رسم خلفية المخطط
    fillRect(r, cfg.x, cfg.y, cfg.width, cfg.height, cfg.bgColor);

    // رسم إطار خارجي خفيف
    ChartColor borderColor = { 80, 80, 100, 180 };
    setColor(r, borderColor);
    SDL_Rect border = { cfg.x, cfg.y, cfg.width, cfg.height };
    SDL_RenderDrawRect(r, &border);
}

// رسم خطوط الشبكة الأفقية والعمودية
static void renderGrid(SDL_Renderer* r, const ChartConfig& cfg) {
    if (!cfg.showGrid) return;

    setColor(r, cfg.gridColor);
    int left   = cfg.x + cfg.padding;
    int right  = cfg.x + cfg.width - cfg.padding;
    int top    = cfg.y + cfg.padding;
    int bottom = cfg.y + cfg.height - cfg.padding;

    // خطوط أفقية — ٥ أقسام
    int gridLines = 5;
    for (int i = 0; i <= gridLines; ++i) {
        int yy = top + (bottom - top) * i / gridLines;
        SDL_RenderDrawLine(r, left, yy, right, yy);
    }
    // خطوط عمودية — ٥ أقسام
    for (int i = 0; i <= gridLines; ++i) {
        int xx = left + (right - left) * i / gridLines;
        SDL_RenderDrawLine(r, xx, top, xx, bottom);
    }
}

// رسم المحاور (محور أفقي + عمودي)
static void renderAxes(SDL_Renderer* r, const ChartConfig& cfg) {
    if (!cfg.showAxes) return;

    setColor(r, cfg.axisColor);
    int left   = cfg.x + cfg.padding;
    int right  = cfg.x + cfg.width - cfg.padding;
    int top    = cfg.y + cfg.padding;
    int bottom = cfg.y + cfg.height - cfg.padding;

    // المحور العمودي
    SDL_RenderDrawLine(r, left, top, left, bottom);
    // المحور الأفقي
    SDL_RenderDrawLine(r, left, bottom, right, bottom);
}

// ═══════════════════════════════════════════════════════════════════
// رسم المخطط العمودي — Bar Chart
// ═══════════════════════════════════════════════════════════════════
static void renderBarChart(SDL_Renderer* r, const Chart& chart) {
    const auto& cfg = chart.config;
    renderBackground(r, cfg);
    renderGrid(r, cfg);
    renderAxes(r, cfg);

    if (chart.bars.empty()) return;

    int left   = cfg.x + cfg.padding;
    int right  = cfg.x + cfg.width - cfg.padding;
    int top    = cfg.y + cfg.padding;
    int bottom = cfg.y + cfg.height - cfg.padding;
    int chartW = right - left;
    int chartH = bottom - top;

    // إيجاد القيمة القصوى لتوحيد المقياس
    float maxVal = 0;
    for (const auto& bar : chart.bars) {
        if (bar.value > maxVal) maxVal = bar.value;
    }
    if (maxVal <= 0) maxVal = 1.0f; // تجنب القسمة على صفر

    int numBars = static_cast<int>(chart.bars.size());
    int barSpacing = 4;  // المسافة بين الأعمدة
    int totalSpacing = barSpacing * (numBars + 1);
    int barWidth = (chartW - totalSpacing) / numBars;
    if (barWidth < 4) barWidth = 4; // حد أدنى لعرض العمود

    // رسم كل عمود
    for (int i = 0; i < numBars; ++i) {
        const auto& bar = chart.bars[i];
        float ratio = bar.value / maxVal;
        int barH = static_cast<int>(ratio * chartH);
        int bx = left + barSpacing + i * (barWidth + barSpacing);
        int by = bottom - barH;

        // رسم العمود
        fillRect(r, bx, by, barWidth, barH, bar.color);

        // رسم حدود العمود — لون أغمق قليلاً
        ChartColor borderCol = { 
            static_cast<uint8_t>(bar.color.r * 0.7),
            static_cast<uint8_t>(bar.color.g * 0.7),
            static_cast<uint8_t>(bar.color.b * 0.7),
            255 
        };
        setColor(r, borderCol);
        SDL_Rect barRect = { bx, by, barWidth, barH };
        SDL_RenderDrawRect(r, &barRect);

        // رسم نقطة صغيرة أعلى العمود كعلامة
        ChartColor topMark = { 255, 255, 255, 200 };
        fillRect(r, bx, by, barWidth, 2, topMark);
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم المخطط الخطي — Line Chart
// ═══════════════════════════════════════════════════════════════════
static void renderLineChart(SDL_Renderer* r, const Chart& chart) {
    const auto& cfg = chart.config;
    renderBackground(r, cfg);
    renderGrid(r, cfg);
    renderAxes(r, cfg);

    if (chart.points.size() < 2) return;

    int left   = cfg.x + cfg.padding;
    int right  = cfg.x + cfg.width - cfg.padding;
    int top    = cfg.y + cfg.padding;
    int bottom = cfg.y + cfg.height - cfg.padding;
    int chartW = right - left;
    int chartH = bottom - top;

    // إيجاد نطاق القيم
    float minX = chart.points[0].x, maxX = chart.points[0].x;
    float minY = chart.points[0].y, maxY = chart.points[0].y;
    for (const auto& pt : chart.points) {
        if (pt.x < minX) minX = pt.x;
        if (pt.x > maxX) maxX = pt.x;
        if (pt.y < minY) minY = pt.y;
        if (pt.y > maxY) maxY = pt.y;
    }
    float rangeX = maxX - minX;
    float rangeY = maxY - minY;
    if (rangeX <= 0) rangeX = 1.0f;
    if (rangeY <= 0) rangeY = 1.0f;

    // تحويل من إحداثيات البيانات إلى إحداثيات الشاشة
    auto toScreenX = [&](float x) -> int {
        return left + static_cast<int>((x - minX) / rangeX * chartW);
    };
    auto toScreenY = [&](float y) -> int {
        return bottom - static_cast<int>((y - minY) / rangeY * chartH);
    };

    // رسم منطقة مظللة تحت الخط (تأثير مخطط مساحي خفيف)
    ChartColor fillColor = { 
        chart.lineColor.r, chart.lineColor.g, chart.lineColor.b, 40 
    };
    setColor(r, fillColor);
    for (size_t i = 0; i + 1 < chart.points.size(); ++i) {
        int x1 = toScreenX(chart.points[i].x);
        int y1 = toScreenY(chart.points[i].y);
        int x2 = toScreenX(chart.points[i + 1].x);
        int y2 = toScreenY(chart.points[i + 1].y);
        // ملء عمودي بسيط بين كل نقطتين
        int startX = std::min(x1, x2);
        int endX = std::max(x1, x2);
        for (int px = startX; px <= endX; ++px) {
            float t = (endX == startX) ? 0.0f : static_cast<float>(px - x1) / (x2 - x1);
            int py = y1 + static_cast<int>(t * (y2 - y1));
            SDL_RenderDrawLine(r, px, py, px, bottom);
        }
    }

    // رسم الخطوط بين النقاط
    setColor(r, chart.lineColor);
    for (size_t i = 0; i + 1 < chart.points.size(); ++i) {
        int x1 = toScreenX(chart.points[i].x);
        int y1 = toScreenY(chart.points[i].y);
        int x2 = toScreenX(chart.points[i + 1].x);
        int y2 = toScreenY(chart.points[i + 1].y);
        drawThickLine(r, x1, y1, x2, y2, chart.lineThickness);
    }

    // رسم نقاط (دوائر صغيرة) على كل بيانة
    ChartColor dotColor = { 255, 255, 255, 230 };
    for (const auto& pt : chart.points) {
        int sx = toScreenX(pt.x);
        int sy = toScreenY(pt.y);
        int dotR = 4; // نصف قطر النقطة
        // رسم مربع صغير (كتقريب للدائرة)
        fillRect(r, sx - dotR, sy - dotR, dotR * 2, dotR * 2, chart.lineColor);
        // إطار أبيض حول النقطة
        setColor(r, dotColor);
        SDL_Rect dotRect = { sx - dotR, sy - dotR, dotR * 2, dotR * 2 };
        SDL_RenderDrawRect(r, &dotRect);
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم المخطط الدائري — Pie Chart
// ═══════════════════════════════════════════════════════════════════
static void renderPieChart(SDL_Renderer* r, const Chart& chart) {
    const auto& cfg = chart.config;
    renderBackground(r, cfg);

    if (chart.slices.empty()) return;

    // حساب المجموع الكلي للشرائح
    float total = 0;
    for (const auto& s : chart.slices) {
        total += s.value;
    }
    if (total <= 0) return;

    // مركز الدائرة ونصف القطر
    int cx = cfg.x + cfg.width / 2;
    int cy = cfg.y + cfg.height / 2;
    int radius = std::min(cfg.width, cfg.height) / 2 - cfg.padding;
    if (radius < 10) radius = 10;

    // رسم كل شريحة بملء البكسلات ضمن نطاق الزاوية
    float startAngle = 0; // بالراديان

    for (const auto& slice : chart.slices) {
        float sweepAngle = (slice.value / total) * 2.0f * static_cast<float>(M_PI);
        float endAngle = startAngle + sweepAngle;

        setColor(r, slice.color);

        // مسح منطقة مربعة حول الدائرة وتلوين البكسلات داخل القطاع
        for (int py = cy - radius; py <= cy + radius; ++py) {
            for (int px = cx - radius; px <= cx + radius; ++px) {
                int dx = px - cx;
                int dy = py - cy;
                float dist = std::sqrt(static_cast<float>(dx * dx + dy * dy));
                if (dist > radius) continue;

                // حساب زاوية البكسل (atan2 يُرجع من -π إلى π)
                float angle = std::atan2(static_cast<float>(dy), static_cast<float>(dx));
                if (angle < 0) angle += 2.0f * static_cast<float>(M_PI);

                // التحقق أن البكسل ضمن القطاع
                if (angle >= startAngle && angle < endAngle) {
                    SDL_RenderDrawPoint(r, px, py);
                }
                // معالجة حالة الالتفاف حول ٢π
                if (endAngle > 2.0f * static_cast<float>(M_PI)) {
                    float wrappedEnd = endAngle - 2.0f * static_cast<float>(M_PI);
                    if (angle < wrappedEnd) {
                        SDL_RenderDrawPoint(r, px, py);
                    }
                }
            }
        }

        // رسم خط فاصل بين الشرائح
        ChartColor sepColor = { cfg.bgColor.r, cfg.bgColor.g, cfg.bgColor.b, 255 };
        setColor(r, sepColor);
        int lx = cx + static_cast<int>(radius * std::cos(startAngle));
        int ly = cy + static_cast<int>(radius * std::sin(startAngle));
        SDL_RenderDrawLine(r, cx, cy, lx, ly);

        startAngle = endAngle;
    }

    // رسم خط فاصل أخير (لإغلاق آخر شريحة)
    ChartColor sepColor = { cfg.bgColor.r, cfg.bgColor.g, cfg.bgColor.b, 255 };
    setColor(r, sepColor);
    int lx = cx + static_cast<int>(radius * std::cos(startAngle));
    int ly = cy + static_cast<int>(radius * std::sin(startAngle));
    SDL_RenderDrawLine(r, cx, cy, lx, ly);

    // رسم حدود الدائرة الخارجية
    setColor(r, cfg.axisColor);
    int segments = 72; // عدد الأجزاء لتقريب الدائرة
    for (int i = 0; i < segments; ++i) {
        float a1 = 2.0f * static_cast<float>(M_PI) * i / segments;
        float a2 = 2.0f * static_cast<float>(M_PI) * (i + 1) / segments;
        int x1 = cx + static_cast<int>(radius * std::cos(a1));
        int y1 = cy + static_cast<int>(radius * std::sin(a1));
        int x2 = cx + static_cast<int>(radius * std::cos(a2));
        int y2 = cy + static_cast<int>(radius * std::sin(a2));
        SDL_RenderDrawLine(r, x1, y1, x2, y2);
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم المخطط الرادار/العنكبوتي — Radar Chart
// ═══════════════════════════════════════════════════════════════════
static void renderRadarChart(SDL_Renderer* r, const Chart& chart) {
    const auto& cfg = chart.config;
    renderBackground(r, cfg);

    int numAxes = static_cast<int>(chart.radarAxes.size());
    if (numAxes < 3) return; // يحتاج ٣ محاور على الأقل

    // المركز ونصف القطر
    int cx = cfg.x + cfg.width / 2;
    int cy = cfg.y + cfg.height / 2;
    int radius = std::min(cfg.width, cfg.height) / 2 - cfg.padding;
    if (radius < 10) radius = 10;

    float angleStep = 2.0f * static_cast<float>(M_PI) / numAxes;

    // رسم حلقات الشبكة (٤ مستويات)
    if (cfg.showGrid) {
        setColor(r, cfg.gridColor);
        int levels = 4;
        for (int lvl = 1; lvl <= levels; ++lvl) {
            float lvlR = radius * lvl / static_cast<float>(levels);
            // رسم شكل متعدد الأضلاع لكل مستوى
            for (int i = 0; i < numAxes; ++i) {
                float a1 = -static_cast<float>(M_PI) / 2 + angleStep * i;
                float a2 = -static_cast<float>(M_PI) / 2 + angleStep * ((i + 1) % numAxes);
                int x1 = cx + static_cast<int>(lvlR * std::cos(a1));
                int y1 = cy + static_cast<int>(lvlR * std::sin(a1));
                int x2 = cx + static_cast<int>(lvlR * std::cos(a2));
                int y2 = cy + static_cast<int>(lvlR * std::sin(a2));
                SDL_RenderDrawLine(r, x1, y1, x2, y2);
            }
        }
    }

    // رسم خطوط المحاور من المركز إلى الأطراف
    setColor(r, cfg.axisColor);
    for (int i = 0; i < numAxes; ++i) {
        float angle = -static_cast<float>(M_PI) / 2 + angleStep * i;
        int ex = cx + static_cast<int>(radius * std::cos(angle));
        int ey = cy + static_cast<int>(radius * std::sin(angle));
        SDL_RenderDrawLine(r, cx, cy, ex, ey);
    }

    // رسم مضلع البيانات المملوء
    if (!chart.radarValues.empty()) {
        // حساب رؤوس المضلع بناءً على القيم
        std::vector<int> polyX(numAxes), polyY(numAxes);
        for (int i = 0; i < numAxes; ++i) {
            float value = 0;
            if (i < static_cast<int>(chart.radarValues.size())) {
                value = chart.radarValues[i];
            }
            float maxVal = chart.radarAxes[i].maxValue;
            if (maxVal <= 0) maxVal = 1.0f;
            float ratio = value / maxVal;
            if (ratio > 1.0f) ratio = 1.0f;
            if (ratio < 0.0f) ratio = 0.0f;

            float angle = -static_cast<float>(M_PI) / 2 + angleStep * i;
            float dist = radius * ratio;
            polyX[i] = cx + static_cast<int>(dist * std::cos(angle));
            polyY[i] = cy + static_cast<int>(dist * std::sin(angle));
        }

        // ملء المضلع باستخدام مسح أفقي (خط بخط)
        ChartColor fillCol = { chart.lineColor.r, chart.lineColor.g, chart.lineColor.b, 60 };
        setColor(r, fillCol);
        // إيجاد الحدود العمودية للمضلع
        int minPY = *std::min_element(polyY.begin(), polyY.end());
        int maxPY = *std::max_element(polyY.begin(), polyY.end());
        for (int scanY = minPY; scanY <= maxPY; ++scanY) {
            std::vector<int> intersections;
            for (int i = 0; i < numAxes; ++i) {
                int j = (i + 1) % numAxes;
                int y1 = polyY[i], y2 = polyY[j];
                int x1 = polyX[i], x2 = polyX[j];
                if ((y1 <= scanY && y2 > scanY) || (y2 <= scanY && y1 > scanY)) {
                    float t = static_cast<float>(scanY - y1) / (y2 - y1);
                    intersections.push_back(x1 + static_cast<int>(t * (x2 - x1)));
                }
            }
            std::sort(intersections.begin(), intersections.end());
            for (size_t k = 0; k + 1 < intersections.size(); k += 2) {
                SDL_RenderDrawLine(r, intersections[k], scanY, intersections[k + 1], scanY);
            }
        }

        // رسم حدود المضلع
        setColor(r, chart.lineColor);
        for (int i = 0; i < numAxes; ++i) {
            int j = (i + 1) % numAxes;
            drawThickLine(r, polyX[i], polyY[i], polyX[j], polyY[j], chart.lineThickness);
        }

        // رسم نقاط على رؤوس المضلع
        ChartColor vertexCol = { 255, 255, 255, 220 };
        for (int i = 0; i < numAxes; ++i) {
            fillRect(r, polyX[i] - 3, polyY[i] - 3, 6, 6, chart.lineColor);
            setColor(r, vertexCol);
            SDL_Rect vr = { polyX[i] - 3, polyY[i] - 3, 6, 6 };
            SDL_RenderDrawRect(r, &vr);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم العداد/المؤشر — Gauge
// ═══════════════════════════════════════════════════════════════════
static void renderGaugeChart(SDL_Renderer* r, const Chart& chart) {
    const auto& cfg = chart.config;
    renderBackground(r, cfg);

    int cx = cfg.x + cfg.width / 2;
    int cy = cfg.y + cfg.height - cfg.padding;
    int radius = std::min(cfg.width / 2 - cfg.padding, cfg.height - cfg.padding * 2);
    if (radius < 10) radius = 10;

    const auto& g = chart.gauge;
    float range = g.maxValue - g.minValue;
    if (range <= 0) range = 1.0f;

    // رسم قوس نصف دائري (من π إلى ٢π — أي النصف العلوي)
    // مقسّم لـ ٣ أقسام ملونة: أخضر ← أصفر ← أحمر
    int arcSegments = 180;
    for (int i = 0; i < arcSegments; ++i) {
        float t = static_cast<float>(i) / arcSegments;
        float angle = static_cast<float>(M_PI) + t * static_cast<float>(M_PI);

        // تحديد اللون بناءً على الموقع (أخضر → أصفر → أحمر)
        uint8_t cr, cg, cb;
        if (t < 0.33f) {
            // أخضر
            cr = 80; cg = 200; cb = 80;
        } else if (t < 0.66f) {
            // أصفر
            cr = 230; cg = 200; cb = 50;
        } else {
            // أحمر
            cr = 220; cg = 60; cb = 60;
        }
        SDL_SetRenderDrawColor(r, cr, cg, cb, 200);

        // رسم قطاع القوس كخطوط من نصف القطر الداخلي إلى الخارجي
        float innerR = radius * 0.7f;
        int ix = cx + static_cast<int>(innerR * std::cos(angle));
        int iy = cy + static_cast<int>(innerR * std::sin(angle));
        int ox = cx + static_cast<int>(radius * std::cos(angle));
        int oy = cy + static_cast<int>(radius * std::sin(angle));
        SDL_RenderDrawLine(r, ix, iy, ox, oy);
    }

    // رسم حدود القوس الخارجي
    setColor(r, cfg.axisColor);
    for (int i = 0; i < arcSegments; ++i) {
        float a1 = static_cast<float>(M_PI) + static_cast<float>(M_PI) * i / arcSegments;
        float a2 = static_cast<float>(M_PI) + static_cast<float>(M_PI) * (i + 1) / arcSegments;
        int x1 = cx + static_cast<int>(radius * std::cos(a1));
        int y1 = cy + static_cast<int>(radius * std::sin(a1));
        int x2 = cx + static_cast<int>(radius * std::cos(a2));
        int y2 = cy + static_cast<int>(radius * std::sin(a2));
        SDL_RenderDrawLine(r, x1, y1, x2, y2);
    }

    // رسم الإبرة — بناءً على القيمة الحالية
    float normalized = (g.currentValue - g.minValue) / range;
    if (normalized < 0.0f) normalized = 0.0f;
    if (normalized > 1.0f) normalized = 1.0f;
    float needleAngle = static_cast<float>(M_PI) + normalized * static_cast<float>(M_PI);

    // خط الإبرة
    ChartColor needleCol = { 255, 255, 255, 240 };
    setColor(r, needleCol);
    int needleLen = static_cast<int>(radius * 0.85f);
    int nx = cx + static_cast<int>(needleLen * std::cos(needleAngle));
    int ny = cy + static_cast<int>(needleLen * std::sin(needleAngle));
    drawThickLine(r, cx, cy, nx, ny, 3.0f);

    // رسم دائرة صغيرة في المركز (محور الإبرة)
    ChartColor centerCol = { 200, 200, 200, 255 };
    fillRect(r, cx - 5, cy - 5, 10, 10, centerCol);

    // رسم علامتي البداية والنهاية على القاعدة
    ChartColor markCol = { 180, 180, 180, 200 };
    setColor(r, markCol);
    // نقطة البداية (يسار)
    int sxL = cx + static_cast<int>((radius + 5) * std::cos(static_cast<float>(M_PI)));
    int syL = cy + static_cast<int>((radius + 5) * std::sin(static_cast<float>(M_PI)));
    fillRect(r, sxL - 2, syL - 2, 4, 4, markCol);
    // نقطة النهاية (يمين)
    int sxR = cx + static_cast<int>((radius + 5) * std::cos(2.0f * static_cast<float>(M_PI)));
    int syR = cy + static_cast<int>((radius + 5) * std::sin(2.0f * static_cast<float>(M_PI)));
    fillRect(r, sxR - 2, syR - 2, 4, 4, markCol);
}

// ═══════════════════════════════════════════════════════════════════
// إنشاء المخططات — الواجهة العامة
// ═══════════════════════════════════════════════════════════════════

// مساعد لإنشاء مخطط جديد بإعدادات عامة
static int createChartInternal(ChartType type, int x, int y, int w, int h) {
    Chart chart;
    chart.type = type;
    chart.config.x = x;
    chart.config.y = y;
    chart.config.width = w;
    chart.config.height = h;
    int id = g_nextChartId++;
    g_charts[id] = std::move(chart);
    return id;
}

int createBarChart(int x, int y, int w, int h) {
    return createChartInternal(ChartType::Bar, x, y, w, h);
}

int createLineChart(int x, int y, int w, int h) {
    return createChartInternal(ChartType::Line, x, y, w, h);
}

int createPieChart(int x, int y, int w, int h) {
    return createChartInternal(ChartType::Pie, x, y, w, h);
}

int createRadarChart(int x, int y, int w, int h) {
    return createChartInternal(ChartType::Radar, x, y, w, h);
}

int createGauge(int x, int y, int w, int h) {
    return createChartInternal(ChartType::Gauge, x, y, w, h);
}

// ═══════════════════════════════════════════════════════════════════
// حذف المخططات
// ═══════════════════════════════════════════════════════════════════

void destroyChart(int id) {
    g_charts.erase(id);
}

void destroyAllCharts() {
    g_charts.clear();
    g_nextChartId = 1; // إعادة العدّاد
}

// ═══════════════════════════════════════════════════════════════════
// إضافة البيانات
// ═══════════════════════════════════════════════════════════════════

void addBar(int chartId, const std::string& label, float value, uint8_t r, uint8_t g, uint8_t b) {
    Chart* c = findChart(chartId);
    if (!c || c->type != ChartType::Bar) return;
    BarData bar;
    bar.label = label;
    bar.value = value;
    bar.color = { r, g, b, 255 };
    c->bars.push_back(std::move(bar));
}

void addPoint(int chartId, float x, float y) {
    Chart* c = findChart(chartId);
    if (!c || c->type != ChartType::Line) return;
    DataPoint pt;
    pt.x = x;
    pt.y = y;
    c->points.push_back(pt);
}

void addSlice(int chartId, const std::string& label, float value, uint8_t r, uint8_t g, uint8_t b) {
    Chart* c = findChart(chartId);
    if (!c || c->type != ChartType::Pie) return;
    PieSlice slice;
    slice.label = label;
    slice.value = value;
    slice.color = { r, g, b, 255 };
    c->slices.push_back(std::move(slice));
}

void addRadarAxis(int chartId, const std::string& label, float maxValue) {
    Chart* c = findChart(chartId);
    if (!c || c->type != ChartType::Radar) return;
    RadarAxis axis;
    axis.label = label;
    axis.maxValue = maxValue;
    c->radarAxes.push_back(std::move(axis));
    // إضافة قيمة افتراضية صفرية
    c->radarValues.push_back(0.0f);
}

void setRadarValue(int chartId, int axisIndex, float value) {
    Chart* c = findChart(chartId);
    if (!c || c->type != ChartType::Radar) return;
    if (axisIndex < 0 || axisIndex >= static_cast<int>(c->radarValues.size())) return;
    c->radarValues[axisIndex] = value;
}

void setGaugeValue(int chartId, float value, float minVal, float maxVal) {
    Chart* c = findChart(chartId);
    if (!c || c->type != ChartType::Gauge) return;
    c->gauge.currentValue = value;
    c->gauge.minValue = minVal;
    c->gauge.maxValue = maxVal;
}

void clearChartData(int chartId) {
    Chart* c = findChart(chartId);
    if (!c) return;
    c->bars.clear();
    c->points.clear();
    c->slices.clear();
    c->radarAxes.clear();
    c->radarValues.clear();
    c->gauge = GaugeData(); // إعادة القيم الافتراضية
}

// ═══════════════════════════════════════════════════════════════════
// التخصيص — تغيير إعدادات المخطط
// ═══════════════════════════════════════════════════════════════════

void setChartTitle(int chartId, const std::string& title) {
    Chart* c = findChart(chartId);
    if (!c) return;
    c->config.title = title;
}

void setChartBgColor(int chartId, uint8_t r, uint8_t g, uint8_t b) {
    Chart* c = findChart(chartId);
    if (!c) return;
    c->config.bgColor = { r, g, b, 255 };
}

void setChartGrid(int chartId, bool show) {
    Chart* c = findChart(chartId);
    if (!c) return;
    c->config.showGrid = show;
}

void setLineColor(int chartId, uint8_t r, uint8_t g, uint8_t b) {
    Chart* c = findChart(chartId);
    if (!c) return;
    c->lineColor = { r, g, b, 255 };
}

void setLineThickness(int chartId, float thickness) {
    Chart* c = findChart(chartId);
    if (!c) return;
    if (thickness < 1.0f) thickness = 1.0f;
    if (thickness > 10.0f) thickness = 10.0f; // حد أقصى معقول
    c->lineThickness = thickness;
}

// ═══════════════════════════════════════════════════════════════════
// الرسم — تحويل المخطط إلى بكسلات على الشاشة
// ═══════════════════════════════════════════════════════════════════

void renderChart(void* renderer, int chartId) {
    if (!renderer) return;
    Chart* c = findChart(chartId);
    if (!c) return;

    SDL_Renderer* sdlRenderer = static_cast<SDL_Renderer*>(renderer);

    // تفعيل دمج الألوان للشفافية
    SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);

    // اختيار دالة الرسم المناسبة حسب نوع المخطط
    switch (c->type) {
        case ChartType::Bar:
            renderBarChart(sdlRenderer, *c);
            break;
        case ChartType::Line:
            renderLineChart(sdlRenderer, *c);
            break;
        case ChartType::Pie:
            renderPieChart(sdlRenderer, *c);
            break;
        case ChartType::Radar:
            renderRadarChart(sdlRenderer, *c);
            break;
        case ChartType::Gauge:
            renderGaugeChart(sdlRenderer, *c);
            break;
    }
}

void renderAllCharts(void* renderer) {
    if (!renderer) return;
    // رسم جميع المخططات المسجلة بالترتيب
    for (auto& pair : g_charts) {
        renderChart(renderer, pair.first);
    }
}

}} // namespace sad::charts
