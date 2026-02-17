// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_responsive.h — محرك التخطيط المتجاوب
// ─────────────────────────────────────────────────────────────────────────────
// MediaQuery, LayoutBuilder, Breakpoints, تخطيطات متكيفة
// يتفوق على Flutter بدعم RTL ذكي وتخطيط عربي أصلي
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

namespace sad { namespace responsive {

// ═══════════════════════════════════════════════════════════════════
// استعلام الوسائط (MediaQuery)
// ═══════════════════════════════════════════════════════════════════

enum class Orientation {
    Portrait,
    Landscape
};

enum class DeviceType {
    Mobile,     // < 600px
    Tablet,     // 600–1024px
    Desktop,    // 1024–1440px
    LargeDesktop // > 1440px
};

struct MediaQueryData {
    float screenWidth = 800;
    float screenHeight = 600;
    float pixelRatio = 1.0f;
    Orientation orientation = Orientation::Landscape;
    DeviceType deviceType = DeviceType::Desktop;
    float textScaleFactor = 1.0f;
    bool isRTL = true;          // عربي افتراضياً — ميزة SAD الفريدة!
    float paddingTop = 0;       // شريط الحالة
    float paddingBottom = 0;    // شريط التنقل
    float viewInsetBottom = 0;  // لوحة المفاتيح
};

// تحديث بيانات MediaQuery
void updateMediaQuery(float screenWidth, float screenHeight, float pixelRatio = 1.0f);
void setTextScaleFactor(float scale);
void setIsRTL(bool rtl);
void setPadding(float top, float bottom);
void setViewInset(float bottom);

// قراءة بيانات MediaQuery
const MediaQueryData& getMediaQuery();
float getScreenWidth();
float getScreenHeight();
float getPixelRatio();
Orientation getOrientation();
DeviceType getDeviceType();
float getTextScaleFactor();
bool isRTL();

// ═══════════════════════════════════════════════════════════════════
// نقاط التوقف (Breakpoints)
// ═══════════════════════════════════════════════════════════════════

struct Breakpoint {
    std::string name;
    float minWidth;
    float maxWidth;
};

// نقاط توقف مسبقة
void setBreakpoint(const std::string& name, float minWidth, float maxWidth);
void removeBreakpoint(const std::string& name);
bool isBreakpointActive(const std::string& name);
std::string getCurrentBreakpoint();
std::vector<std::string> getActiveBreakpoints();

// ═══════════════════════════════════════════════════════════════════
// LayoutBuilder — تخطيط بناءً على القيود
// ═══════════════════════════════════════════════════════════════════

struct BoxConstraints {
    float minWidth  = 0;
    float maxWidth  = 99999;
    float minHeight = 0;
    float maxHeight = 99999;
};

using LayoutBuilderCallback = std::function<int(const BoxConstraints& constraints)>;

int registerLayoutBuilder(int widgetId, LayoutBuilderCallback callback);
void unregisterLayoutBuilder(int id);
int buildWithConstraints(int builderId, float maxW, float maxH);

// ═══════════════════════════════════════════════════════════════════
// قيم متجاوبة (Responsive Values)
// ═══════════════════════════════════════════════════════════════════

// قيمة تتغير حسب حجم الشاشة
float responsiveValue(float mobile, float tablet, float desktop);
float responsiveValue4(float mobile, float tablet, float desktop, float largeDesktop);
float responsiveFontSize(float baseFontSize);

// حساب نسبة من الشاشة
float percentWidth(float percent);
float percentHeight(float percent);
float vw(float units);   // viewport width units (مثل CSS vw)
float vh(float units);   // viewport height units

// ═══════════════════════════════════════════════════════════════════
// حشوة وهوامش متجاوبة
// ═══════════════════════════════════════════════════════════════════

struct ResponsivePadding {
    float left = 0, top = 0, right = 0, bottom = 0;
};

ResponsivePadding responsivePadding(float mobilePd, float tabletPd, float desktopPd);
ResponsivePadding safePadding();  // يأخذ في الاعتبار شريط الحالة والتنقل

// ═══════════════════════════════════════════════════════════════════
// شبكة متجاوبة (Responsive Grid)
// ═══════════════════════════════════════════════════════════════════

struct GridConfig {
    int columns = 12;       // عدد الأعمدة
    float gutter = 16;      // المسافة بين الأعمدة
    float margin = 24;      // الهامش الجانبي
    float maxContentWidth = 1200;  // أقصى عرض للمحتوى
};

void setGridConfig(const GridConfig& config);
const GridConfig& getGridConfig();
float getColumnWidth(int span);      // عرض عدد أعمدة محدد
float getContentWidth();              // عرض المحتوى الفعلي
int getEffectiveColumns();            // عدد الأعمدة الفعلي (يتغير على الموبايل)

// ═══════════════════════════════════════════════════════════════════
// مراقبة تغيرات الحجم
// ═══════════════════════════════════════════════════════════════════

using ResizeCallback = std::function<void(float newWidth, float newHeight)>;
using BreakpointChangeCallback = std::function<void(const std::string& newBreakpoint)>;

int onResize(ResizeCallback callback);
int onBreakpointChange(BreakpointChangeCallback callback);
void removeResizeListener(int id);
void removeBreakpointListener(int id);

// ═══════════════════════════════════════════════════════════════════
// تخطيط RTL ذكي — ميزة SAD الفريدة!
// ═══════════════════════════════════════════════════════════════════

// تحويل إحداثيات LTR إلى RTL تلقائياً
float rtlX(float x, float width);
float rtlAlign(float x, float totalWidth, float itemWidth);
float rtlPaddingStart(float padding);  // start = right في RTL
float rtlPaddingEnd(float padding);    // end = left في RTL

}} // namespace sad::responsive
