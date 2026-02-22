// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_responsive.cpp — تنفيذ محرك التخطيط المتجاوب
// ─────────────────────────────────────────────────────────────────────────────
// يتفوق على Flutter بدعم RTL أصلي ونظام شبكة ذكي
// ═══════════════════════════════════════════════════════════════════════════════
#include "sad_responsive.h"
#include <algorithm>
#include <cmath>

namespace sad { namespace responsive {

// ═══════════════════════════════════════════════════════════════════
// المتغيرات العالمية
// ═══════════════════════════════════════════════════════════════════

static MediaQueryData g_mediaQuery;
static std::unordered_map<std::string, Breakpoint> g_breakpoints;
static GridConfig g_gridConfig;

// LayoutBuilder
struct LayoutBuilderEntry {
    int id;
    int widgetId;
    LayoutBuilderCallback callback;
};
static std::vector<LayoutBuilderEntry> g_layoutBuilders;
static int g_nextLayoutBuilderId = 1;

// مراقبون
struct ResizeEntry { int id; ResizeCallback callback; };
struct BreakpointEntry { int id; BreakpointChangeCallback callback; };
static std::vector<ResizeEntry> g_resizeListeners;
static std::vector<BreakpointEntry> g_breakpointListeners;
static int g_nextResizeId = 1;
static int g_nextBreakpointId = 1;
static std::string g_lastBreakpointName;

// ═══════════════════════════════════════════════════════════════════
// تحديد نوع الجهاز والتوجه
// ═══════════════════════════════════════════════════════════════════

static DeviceType classifyDevice(float width) {
    if (width < 600) return DeviceType::Mobile;
    if (width < 1024) return DeviceType::Tablet;
    if (width < 1440) return DeviceType::Desktop;
    return DeviceType::LargeDesktop;
}

static Orientation classifyOrientation(float w, float h) {
    return (h > w) ? Orientation::Portrait : Orientation::Landscape;
}

// ═══════════════════════════════════════════════════════════════════
// تحديث بيانات MediaQuery
// ═══════════════════════════════════════════════════════════════════

void updateMediaQuery(float screenWidth, float screenHeight, float pixelRatio) {
    float oldW = g_mediaQuery.screenWidth;
    float oldH = g_mediaQuery.screenHeight;
    
    g_mediaQuery.screenWidth = screenWidth;
    g_mediaQuery.screenHeight = screenHeight;
    g_mediaQuery.pixelRatio = pixelRatio;
    g_mediaQuery.orientation = classifyOrientation(screenWidth, screenHeight);
    g_mediaQuery.deviceType = classifyDevice(screenWidth);
    
    // إعلام مراقبي تغير الحجم
    if (std::abs(oldW - screenWidth) > 0.5f || std::abs(oldH - screenHeight) > 0.5f) {
        for (const auto& entry : g_resizeListeners) {
            if (entry.callback) entry.callback(screenWidth, screenHeight);
        }
    }
    
    // إعلام مراقبي تغير نقطة التوقف
    std::string currentBp = getCurrentBreakpoint();
    if (currentBp != g_lastBreakpointName) {
        g_lastBreakpointName = currentBp;
        for (const auto& entry : g_breakpointListeners) {
            if (entry.callback) entry.callback(currentBp);
        }
    }
}

void setTextScaleFactor(float scale) {
    g_mediaQuery.textScaleFactor = scale;
}

void setIsRTL(bool rtl) {
    g_mediaQuery.isRTL = rtl;
}

void setPadding(float top, float bottom) {
    g_mediaQuery.paddingTop = top;
    g_mediaQuery.paddingBottom = bottom;
}

void setViewInset(float bottom) {
    g_mediaQuery.viewInsetBottom = bottom;
}

// ═══════════════════════════════════════════════════════════════════
// قراءة بيانات MediaQuery
// ═══════════════════════════════════════════════════════════════════

const MediaQueryData& getMediaQuery() { return g_mediaQuery; }
float getScreenWidth()  { return g_mediaQuery.screenWidth; }
float getScreenHeight() { return g_mediaQuery.screenHeight; }
float getPixelRatio()   { return g_mediaQuery.pixelRatio; }
Orientation getOrientation() { return g_mediaQuery.orientation; }
DeviceType getDeviceType()   { return g_mediaQuery.deviceType; }
float getTextScaleFactor()   { return g_mediaQuery.textScaleFactor; }
bool isRTL()                 { return g_mediaQuery.isRTL; }

// ═══════════════════════════════════════════════════════════════════
// نقاط التوقف
// ═══════════════════════════════════════════════════════════════════

void setBreakpoint(const std::string& name, float minWidth, float maxWidth) {
    g_breakpoints[name] = {name, minWidth, maxWidth};
}

void removeBreakpoint(const std::string& name) {
    g_breakpoints.erase(name);
}

bool isBreakpointActive(const std::string& name) {
    auto it = g_breakpoints.find(name);
    if (it == g_breakpoints.end()) return false;
    float w = g_mediaQuery.screenWidth;
    return (w >= it->second.minWidth && w <= it->second.maxWidth);
}

std::string getCurrentBreakpoint() {
    // تعريف نقاط التوقف الافتراضية إذا لم تكن محددة
    if (g_breakpoints.empty()) {
        g_breakpoints["mobile"]       = {"mobile",       0, 599};
        g_breakpoints["tablet"]       = {"tablet",       600, 1023};
        g_breakpoints["desktop"]      = {"desktop",      1024, 1439};
        g_breakpoints["largeDesktop"] = {"largeDesktop",  1440, 99999};
    }
    
    float w = g_mediaQuery.screenWidth;
    for (const auto& [name, bp] : g_breakpoints) {
        if (w >= bp.minWidth && w <= bp.maxWidth) return name;
    }
    return "desktop";
}

std::vector<std::string> getActiveBreakpoints() {
    std::vector<std::string> result;
    for (const auto& [name, bp] : g_breakpoints) {
        if (isBreakpointActive(name)) result.push_back(name);
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════
// LayoutBuilder
// ═══════════════════════════════════════════════════════════════════

int registerLayoutBuilder(int widgetId, LayoutBuilderCallback callback) {
    int id = g_nextLayoutBuilderId++;
    g_layoutBuilders.push_back({id, widgetId, callback});
    return id;
}

void unregisterLayoutBuilder(int id) {
    g_layoutBuilders.erase(
        std::remove_if(g_layoutBuilders.begin(), g_layoutBuilders.end(),
            [id](const LayoutBuilderEntry& e) { return e.id == id; }),
        g_layoutBuilders.end());
}

int buildWithConstraints(int builderId, float maxW, float maxH) {
    for (const auto& entry : g_layoutBuilders) {
        if (entry.id == builderId && entry.callback) {
            BoxConstraints c;
            c.maxWidth = maxW;
            c.maxHeight = maxH;
            return entry.callback(c);
        }
    }
    return -1;
}

// ═══════════════════════════════════════════════════════════════════
// قيم متجاوبة
// ═══════════════════════════════════════════════════════════════════

float responsiveValue(float mobile, float tablet, float desktop) {
    switch (g_mediaQuery.deviceType) {
        case DeviceType::Mobile: return mobile;
        case DeviceType::Tablet: return tablet;
        case DeviceType::Desktop:
        case DeviceType::LargeDesktop: return desktop;
    }
    return desktop;
}

float responsiveValue4(float mobile, float tablet, float desktop, float largeDesktop) {
    switch (g_mediaQuery.deviceType) {
        case DeviceType::Mobile: return mobile;
        case DeviceType::Tablet: return tablet;
        case DeviceType::Desktop: return desktop;
        case DeviceType::LargeDesktop: return largeDesktop;
    }
    return desktop;
}

float responsiveFontSize(float baseFontSize) {
    float scale = g_mediaQuery.textScaleFactor;
    switch (g_mediaQuery.deviceType) {
        case DeviceType::Mobile: return baseFontSize * 0.85f * scale;
        case DeviceType::Tablet: return baseFontSize * 0.95f * scale;
        case DeviceType::Desktop: return baseFontSize * scale;
        case DeviceType::LargeDesktop: return baseFontSize * 1.1f * scale;
    }
    return baseFontSize * scale;
}

float percentWidth(float percent) {
    return g_mediaQuery.screenWidth * percent / 100.0f;
}

float percentHeight(float percent) {
    return g_mediaQuery.screenHeight * percent / 100.0f;
}

float vw(float units) { return g_mediaQuery.screenWidth * units / 100.0f; }
float vh(float units) { return g_mediaQuery.screenHeight * units / 100.0f; }

// ═══════════════════════════════════════════════════════════════════
// حشوة وهوامش متجاوبة
// ═══════════════════════════════════════════════════════════════════

ResponsivePadding responsivePadding(float mobilePd, float tabletPd, float desktopPd) {
    float pd = responsiveValue(mobilePd, tabletPd, desktopPd);
    return {pd, pd, pd, pd};
}

ResponsivePadding safePadding() {
    return {
        g_mediaQuery.isRTL ? 0.0f : 0.0f,
        g_mediaQuery.paddingTop,
        g_mediaQuery.isRTL ? 0.0f : 0.0f,
        std::max(g_mediaQuery.paddingBottom, g_mediaQuery.viewInsetBottom)
    };
}

// ═══════════════════════════════════════════════════════════════════
// شبكة متجاوبة
// ═══════════════════════════════════════════════════════════════════

void setGridConfig(const GridConfig& config) {
    g_gridConfig = config;
}

const GridConfig& getGridConfig() { return g_gridConfig; }

float getContentWidth() {
    float available = g_mediaQuery.screenWidth - 2 * g_gridConfig.margin;
    return std::min(available, g_gridConfig.maxContentWidth);
}

int getEffectiveColumns() {
    // على الموبايل نقلل عدد الأعمدة
    switch (g_mediaQuery.deviceType) {
        case DeviceType::Mobile: return std::min(g_gridConfig.columns, 4);
        case DeviceType::Tablet: return std::min(g_gridConfig.columns, 8);
        default: return g_gridConfig.columns;
    }
}

float getColumnWidth(int span) {
    int cols = getEffectiveColumns();
    float content = getContentWidth();
    float totalGutter = g_gridConfig.gutter * (cols - 1);
    float singleCol = (content - totalGutter) / cols;
    return singleCol * span + g_gridConfig.gutter * (span - 1);
}

// ═══════════════════════════════════════════════════════════════════
// مراقبون
// ═══════════════════════════════════════════════════════════════════

int onResize(ResizeCallback callback) {
    int id = g_nextResizeId++;
    g_resizeListeners.push_back({id, callback});
    return id;
}

int onBreakpointChange(BreakpointChangeCallback callback) {
    int id = g_nextBreakpointId++;
    g_breakpointListeners.push_back({id, callback});
    return id;
}

void removeResizeListener(int id) {
    g_resizeListeners.erase(
        std::remove_if(g_resizeListeners.begin(), g_resizeListeners.end(),
            [id](const ResizeEntry& e) { return e.id == id; }),
        g_resizeListeners.end());
}

void removeBreakpointListener(int id) {
    g_breakpointListeners.erase(
        std::remove_if(g_breakpointListeners.begin(), g_breakpointListeners.end(),
            [id](const BreakpointEntry& e) { return e.id == id; }),
        g_breakpointListeners.end());
}

// ═══════════════════════════════════════════════════════════════════
// تحويل RTL ذكي
// ═══════════════════════════════════════════════════════════════════

float rtlX(float x, float width) {
    if (!g_mediaQuery.isRTL) return x;
    return g_mediaQuery.screenWidth - x - width;
}

float rtlAlign(float x, float totalWidth, float itemWidth) {
    if (!g_mediaQuery.isRTL) return x;
    return totalWidth - x - itemWidth;
}

float rtlPaddingStart(float padding) {
    return padding; // start = right في RTL (مدار بواسطة نظام التخطيط)
}

float rtlPaddingEnd(float padding) {
    return padding; // end = left في RTL
}

}} // namespace sad::responsive
