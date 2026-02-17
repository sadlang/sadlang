// ═══════════════════════════════════════════════════════════════════════════════
// نظام المخططات البيانية — لغة ص
// Charts & Data Visualization System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام رسم مخططات بيانية متكامل — Flutter لا يملك نظام مخططات مدمج!
// يحتاج المطور في Flutter لاستخدام مكتبات خارجية مثل fl_chart أو charts_flutter
// بينما في لغة ص، المخططات مدمجة وجاهزة للاستخدام فوراً.
//
// الأنواع المدعومة:
//   📊 مخطط أعمدة (Bar Chart) — عمودي وأفقي
//   📈 مخطط خطي (Line Chart) — مع تنعيم اختياري
//   🥧 مخطط دائري (Pie Chart) — مع تسميات ونسب
//   🕸️ مخطط رادار/عنكبوتي (Radar Chart) — لمقارنة متعددة الأبعاد
//   📏 مؤشر/عداد (Gauge) — لعرض القيم بين حد أدنى وأقصى
//   📉 مخطط مساحي (Area Chart) — مخطط خطي مع تعبئة
//
// المميزات:
//   - محاور تلقائية مع تسميات
//   - ألوان قابلة للتخصيص
//   - شبكة خلفية اختيارية
//   - وسيلة إيضاح (Legend) تلقائية
//   - دعم البيانات العربية
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

namespace sad { namespace charts {

// ═══════════════════════════════════════════════════════════════════
// هياكل البيانات — الألوان ونقاط البيانات
// ═══════════════════════════════════════════════════════════════════

// لون RGBA للمخططات
struct ChartColor {
    uint8_t r = 100, g = 150, b = 255, a = 255;
};

// نقطة بيانات واحدة (للمخطط الخطي والمساحي)
struct DataPoint {
    float x = 0;        // الإحداثي الأفقي
    float y = 0;        // الإحداثي العمودي
    std::string label;   // تسمية اختيارية
};

// عمود بيانات (للمخطط العمودي)
struct BarData {
    float value = 0;         // القيمة
    std::string label;       // التسمية
    ChartColor color;        // اللون
};

// شريحة دائرية (للمخطط الدائري)
struct PieSlice {
    float value = 0;         // القيمة
    std::string label;       // التسمية
    ChartColor color;        // اللون
};

// بُعد رادار (للمخطط العنكبوتي)
struct RadarAxis {
    std::string label;       // اسم البُعد
    float maxValue = 100;    // القيمة القصوى لهذا البُعد
};

// ═══════════════════════════════════════════════════════════════════
// إعدادات المخططات
// ═══════════════════════════════════════════════════════════════════

// إعدادات عامة لجميع المخططات
struct ChartConfig {
    int x = 0, y = 0;           // موضع المخطط على الشاشة
    int width = 400;             // عرض منطقة الرسم
    int height = 300;            // ارتفاع منطقة الرسم
    int padding = 40;            // الهامش الداخلي
    bool showGrid = true;        // عرض الشبكة الخلفية
    bool showAxes = true;        // عرض المحاور
    bool showLabels = true;      // عرض التسميات
    bool showLegend = true;      // عرض وسيلة الإيضاح
    ChartColor bgColor = {30, 30, 40, 255};       // لون الخلفية
    ChartColor gridColor = {60, 60, 80, 100};     // لون الشبكة
    ChartColor axisColor = {200, 200, 200, 255};  // لون المحاور
    std::string title;           // عنوان المخطط
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — API
// ═══════════════════════════════════════════════════════════════════

// ─── إنشاء المخططات — كل دالة تُرجع معرّف المخطط ───

// إنشاء مخطط أعمدة
int createBarChart(int x, int y, int w, int h);

// إنشاء مخطط خطي
int createLineChart(int x, int y, int w, int h);

// إنشاء مخطط دائري
int createPieChart(int x, int y, int w, int h);

// إنشاء مخطط رادار/عنكبوتي
int createRadarChart(int x, int y, int w, int h);

// إنشاء مؤشر/عداد
int createGauge(int x, int y, int w, int h);

// حذف مخطط
void destroyChart(int id);

// حذف جميع المخططات
void destroyAllCharts();

// ─── إضافة بيانات ───

// إضافة عمود للمخطط العمودي
void addBar(int chartId, const std::string& label, float value, uint8_t r, uint8_t g, uint8_t b);

// إضافة نقطة للمخطط الخطي
void addPoint(int chartId, float x, float y);

// إضافة شريحة للمخطط الدائري
void addSlice(int chartId, const std::string& label, float value, uint8_t r, uint8_t g, uint8_t b);

// إضافة بُعد للرادار
void addRadarAxis(int chartId, const std::string& label, float maxValue);

// إضافة قيمة على محور الرادار
void setRadarValue(int chartId, int axisIndex, float value);

// تعيين قيمة المؤشر/العداد
void setGaugeValue(int chartId, float value, float minVal, float maxVal);

// مسح بيانات المخطط
void clearChartData(int chartId);

// ─── التخصيص ───

// تعيين عنوان المخطط
void setChartTitle(int chartId, const std::string& title);

// تعيين لون الخلفية
void setChartBgColor(int chartId, uint8_t r, uint8_t g, uint8_t b);

// تعيين عرض/إخفاء الشبكة
void setChartGrid(int chartId, bool show);

// تعيين لون خطوط المخطط الخطي
void setLineColor(int chartId, uint8_t r, uint8_t g, uint8_t b);

// تعيين سمك الخطوط
void setLineThickness(int chartId, float thickness);

// ─── الرسم ───

// رسم المخطط على الشاشة
void renderChart(void* renderer, int chartId);

// رسم جميع المخططات
void renderAllCharts(void* renderer);

}} // namespace sad::charts
