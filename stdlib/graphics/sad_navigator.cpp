// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_navigator.cpp — تنفيذ نظام التنقل والتوجيه
// ═══════════════════════════════════════════════════════════════════════════════
#include "sad_navigator.h"
#include "sad_ui_framework.h"
#include <algorithm>
#include <cmath>

namespace sad { namespace nav {

// ───── المتغيرات العالمية ─────

// المسارات المسجّلة
static std::unordered_map<std::string, PageBuilder> g_routes;
static std::unordered_map<std::string, TransitionType> g_routeTransitions;

// مكدس الصفحات
static std::vector<Page> g_pageStack;

// إعدادات الانتقال
static TransitionType g_defaultTransition = TransitionType::SlideRight;
static int g_transitionDurationMs = 300;

// حالة الانتقال
static bool g_isTransitioning = false;
static float g_transitionProgress = 0;
static Page g_incomingPage;
static Page g_outgoingPage;
static double g_transitionElapsed = 0;

// الحوارات
static int g_dialogWidgetId = 0;
static bool g_dialogShowing = false;
static int g_bottomSheetWidgetId = 0;
static bool g_bottomSheetShowing = false;

// التنقل بالتبويب
struct TabNavigator {
    int tabCount = 0;
    int currentTab = 0;
    std::vector<int> tabWidgets;
};
static TabNavigator g_tabNav;

// تاريخ التنقل
static std::vector<std::string> g_history;

// آخر نتيجة عودة
static std::string g_lastPopResult;

// ═══════════════════════════════════════════════════════════════════
// إدارة المسارات
// ═══════════════════════════════════════════════════════════════════

void registerRoute(const std::string& routeName, PageBuilder builder) {
    g_routes[routeName] = builder;
}

void registerRoute(const std::string& routeName, PageBuilder builder,
                   TransitionType transition) {
    g_routes[routeName] = builder;
    g_routeTransitions[routeName] = transition;
}

void unregisterRoute(const std::string& routeName) {
    g_routes.erase(routeName);
    g_routeTransitions.erase(routeName);
}

bool hasRoute(const std::string& routeName) {
    return g_routes.count(routeName) > 0;
}

std::vector<std::string> getRegisteredRoutes() {
    std::vector<std::string> routes;
    for (const auto& [name, _] : g_routes) {
        routes.push_back(name);
    }
    return routes;
}

// ═══════════════════════════════════════════════════════════════════
// دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════

// بناء صفحة من مسار
static Page buildPage(const std::string& routeName,
                      const std::unordered_map<std::string, std::string>& args) {
    Page page;
    page.name = routeName;
    page.args = args;
    
    // تحديد نوع الانتقال
    auto tIt = g_routeTransitions.find(routeName);
    page.transition = (tIt != g_routeTransitions.end()) ? tIt->second : g_defaultTransition;
    
    // بناء شجرة الودجات
    auto it = g_routes.find(routeName);
    if (it != g_routes.end()) {
        page.rootWidgetId = it->second(args);
    }
    
    return page;
}

// بدء انتقال متحرك
static void startTransition(const Page& incoming, TransitionType type) {
    g_isTransitioning = true;
    g_transitionProgress = 0;
    g_transitionElapsed = 0;
    g_incomingPage = incoming;
    g_incomingPage.transition = type;
    g_incomingPage.isAnimating = true;
    
    if (g_pageStack.size() > 1) {
        g_outgoingPage = g_pageStack[g_pageStack.size() - 2];
    }
}

// ═══════════════════════════════════════════════════════════════════
// التنقل الأساسي
// ═══════════════════════════════════════════════════════════════════

bool push(const std::string& routeName) {
    return push(routeName, {});
}

bool push(const std::string& routeName,
          const std::unordered_map<std::string, std::string>& args) {
    auto it = g_routes.find(routeName);
    if (it == g_routes.end()) return false;
    
    Page page = buildPage(routeName, args);
    g_pageStack.push_back(page);
    g_history.push_back(routeName);
    
    // بدء الانتقال
    if (g_transitionDurationMs > 0 && page.transition != TransitionType::None) {
        startTransition(page, page.transition);
    }
    
    return true;
}

bool pushWidget(int widgetId, const std::string& pageName) {
    Page page;
    page.name = pageName.empty() ? ("page_" + std::to_string(g_pageStack.size())) : pageName;
    page.rootWidgetId = widgetId;
    page.transition = g_defaultTransition;
    g_pageStack.push_back(page);
    g_history.push_back(page.name);
    
    if (g_transitionDurationMs > 0 && page.transition != TransitionType::None) {
        startTransition(page, page.transition);
    }
    return true;
}

bool pop() {
    if (g_pageStack.size() <= 1) return false;
    
    g_pageStack.pop_back();
    
    if (!g_pageStack.empty()) {
        g_history.push_back(g_pageStack.back().name);
    }
    
    return true;
}

bool popWithResult(const std::string& result) {
    g_lastPopResult = result;
    return pop();
}

bool replace(const std::string& routeName) {
    if (g_pageStack.empty()) return push(routeName);
    
    auto it = g_routes.find(routeName);
    if (it == g_routes.end()) return false;
    
    g_pageStack.pop_back();
    Page page = buildPage(routeName, {});
    g_pageStack.push_back(page);
    g_history.push_back(routeName);
    
    return true;
}

bool pushAndClearAll(const std::string& routeName) {
    auto it = g_routes.find(routeName);
    if (it == g_routes.end()) return false;
    
    g_pageStack.clear();
    Page page = buildPage(routeName, {});
    g_pageStack.push_back(page);
    g_history.clear();
    g_history.push_back(routeName);
    
    return true;
}

bool popUntil(const std::string& routeName) {
    while (g_pageStack.size() > 1) {
        if (g_pageStack.back().name == routeName) return true;
        g_pageStack.pop_back();
    }
    return (!g_pageStack.empty() && g_pageStack.back().name == routeName);
}

// ═══════════════════════════════════════════════════════════════════
// استعلامات المكدس
// ═══════════════════════════════════════════════════════════════════

std::string currentRoute() {
    if (g_pageStack.empty()) return "";
    return g_pageStack.back().name;
}

bool canPop() {
    return g_pageStack.size() > 1;
}

int stackSize() {
    return static_cast<int>(g_pageStack.size());
}

std::string getArg(const std::string& key, const std::string& defaultVal) {
    if (g_pageStack.empty()) return defaultVal;
    const auto& args = g_pageStack.back().args;
    auto it = args.find(key);
    return (it != args.end()) ? it->second : defaultVal;
}

int currentPageWidget() {
    // أولاً الحوار إن كان ظاهراً
    if (g_dialogShowing && g_dialogWidgetId > 0) return g_dialogWidgetId;
    if (g_bottomSheetShowing && g_bottomSheetWidgetId > 0) return g_bottomSheetWidgetId;
    
    // التبويب الحالي إن كان متنقل التبويب فعّالاً
    if (g_tabNav.tabCount > 0 && g_tabNav.currentTab < (int)g_tabNav.tabWidgets.size()) {
        return g_tabNav.tabWidgets[g_tabNav.currentTab];
    }
    
    if (g_pageStack.empty()) return 0;
    return g_pageStack.back().rootWidgetId;
}

std::vector<std::string> getNavigationHistory() {
    return g_history;
}

// ═══════════════════════════════════════════════════════════════════
// الانتقالات المتحركة
// ═══════════════════════════════════════════════════════════════════

void setDefaultTransition(TransitionType type) {
    g_defaultTransition = type;
}

void setTransitionDuration(int durationMs) {
    g_transitionDurationMs = durationMs;
}

void updateTransitions(double deltaMs) {
    if (!g_isTransitioning) return;
    
    g_transitionElapsed += deltaMs;
    g_transitionProgress = static_cast<float>(g_transitionElapsed / g_transitionDurationMs);
    
    if (g_transitionProgress >= 1.0f) {
        g_transitionProgress = 1.0f;
        g_isTransitioning = false;
        
        // تحديث الصفحة في المكدس
        if (!g_pageStack.empty()) {
            g_pageStack.back().transitionProgress = 1.0f;
            g_pageStack.back().isAnimating = false;
        }
        return;
    }
    
    // تطبيق تسهيل (ease-out cubic)
    float t = g_transitionProgress;
    float eased = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
    
    // تطبيق الانتقال على الودجات
    if (!g_pageStack.empty()) {
        Page& current = g_pageStack.back();
        sad::ui::Widget* w = sad::ui::getWidget(current.rootWidgetId);
        if (!w) return;
        
        switch (current.transition) {
            case TransitionType::Fade: {
                // تغيير الشفافية تدريجياً
                w->bgColor.a = static_cast<int>(eased * 255);
                break;
            }
            case TransitionType::SlideRight: {
                // الصفحة الجديدة تنزلق من اليمين
                float screenWidth = w->cw > 0 ? w->cw : 800;
                w->cx = screenWidth * (1.0f - eased);
                break;
            }
            case TransitionType::SlideLeft: {
                float screenWidth = w->cw > 0 ? w->cw : 800;
                w->cx = -screenWidth * (1.0f - eased);
                break;
            }
            case TransitionType::SlideUp: {
                float screenHeight = w->ch > 0 ? w->ch : 600;
                w->cy = screenHeight * (1.0f - eased);
                break;
            }
            case TransitionType::SlideDown: {
                float screenHeight = w->ch > 0 ? w->ch : 600;
                w->cy = -screenHeight * (1.0f - eased);
                break;
            }
            case TransitionType::Scale: {
                // تأثير التكبير — نغيّر العرض والارتفاع
                float scale = 0.5f + eased * 0.5f;
                w->fixedWidth = w->cw * scale;
                w->fixedHeight = w->ch * scale;
                break;
            }
            case TransitionType::ScaleAndFade: {
                float scale = 0.8f + eased * 0.2f;
                w->fixedWidth = w->cw * scale;
                w->fixedHeight = w->ch * scale;
                w->bgColor.a = static_cast<int>(eased * 255);
                break;
            }
            default: break;
        }
    }
}

bool isTransitioning() {
    return g_isTransitioning;
}

float getTransitionProgress() {
    return g_transitionProgress;
}

// ═══════════════════════════════════════════════════════════════════
// الحوارات
// ═══════════════════════════════════════════════════════════════════

int showDialog(int dialogWidgetId) {
    g_dialogWidgetId = dialogWidgetId;
    g_dialogShowing = true;
    return dialogWidgetId;
}

void dismissDialog() {
    g_dialogShowing = false;
    g_dialogWidgetId = 0;
}

bool isDialogShowing() {
    return g_dialogShowing;
}

int showBottomSheet(int widgetId) {
    g_bottomSheetWidgetId = widgetId;
    g_bottomSheetShowing = true;
    return widgetId;
}

void dismissBottomSheet() {
    g_bottomSheetShowing = false;
    g_bottomSheetWidgetId = 0;
}

// ═══════════════════════════════════════════════════════════════════
// التنقل بالتبويب
// ═══════════════════════════════════════════════════════════════════

int createTabNavigator(int tabCount) {
    g_tabNav.tabCount = tabCount;
    g_tabNav.currentTab = 0;
    g_tabNav.tabWidgets.clear();
    g_tabNav.tabWidgets.resize(tabCount, 0);
    return tabCount;
}

void switchTab(int tabIndex) {
    if (tabIndex >= 0 && tabIndex < g_tabNav.tabCount) {
        g_tabNav.currentTab = tabIndex;
    }
}

int currentTab() {
    return g_tabNav.currentTab;
}

void setTabWidget(int tabIndex, int widgetId) {
    if (tabIndex >= 0 && tabIndex < (int)g_tabNav.tabWidgets.size()) {
        g_tabNav.tabWidgets[tabIndex] = widgetId;
    }
}

int getTabWidget(int tabIndex) {
    if (tabIndex >= 0 && tabIndex < (int)g_tabNav.tabWidgets.size()) {
        return g_tabNav.tabWidgets[tabIndex];
    }
    return 0;
}

// ═══════════════════════════════════════════════════════════════════
// تنظيف
// ═══════════════════════════════════════════════════════════════════

void clearAll() {
    g_routes.clear();
    g_routeTransitions.clear();
    g_pageStack.clear();
    g_history.clear();
    g_isTransitioning = false;
    g_dialogShowing = false;
    g_bottomSheetShowing = false;
    g_tabNav = TabNavigator();
}

void reset() {
    g_pageStack.clear();
    g_history.clear();
    g_isTransitioning = false;
    g_dialogShowing = false;
    g_bottomSheetShowing = false;
}

}} // namespace sad::nav
