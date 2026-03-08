// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_navigation.h — نظام التنقل 2.0 بأسلوب Flutter
//  Navigation 2.0: Router, Pages API, RouterDelegate, RouteInformationParser
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_animation.h"
#include <stack>
#include <variant>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  RouteSettings — إعدادات المسار
// ═══════════════════════════════════════════════════════════════════════════════

struct RouteSettings {
    std::string name;
    std::any arguments;
    
    RouteSettings(const std::string& n = "", std::any args = {})
        : name(n), arguments(std::move(args)) {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//  RouteInformation — معلومات المسار
// ═══════════════════════════════════════════════════════════════════════════════

struct RouteInformation {
    std::string location;        // المسار (مثل /home/profile)
    std::any state;              // حالة إضافية
    
    RouteInformation(const std::string& loc = "/", std::any s = {})
        : location(loc), state(std::move(s)) {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Page — صفحة (Navigation 2.0)
// ═══════════════════════════════════════════════════════════════════════════════

/// مفتاح الصفحة
class LocalKey {
public:
    LocalKey() = default;
    explicit LocalKey(const std::string& value) : value_(value) {}
    
    const std::string& value() const { return value_; }
    
    bool operator==(const LocalKey& other) const { return value_ == other.value_; }
    bool operator!=(const LocalKey& other) const { return value_ != other.value_; }
    
private:
    std::string value_;
};

class ValueKey : public LocalKey {
public:
    template<typename T>
    explicit ValueKey(const T& v) : LocalKey(std::to_string(v)) {}
    
    explicit ValueKey(const std::string& v) : LocalKey(v) {}
};

/// الصفحة الأساسية
template<typename T = void>
class Page {
public:
    Page(
        LocalKey key,
        std::string name = "",
        std::any arguments = {}
    )
        : key_(std::move(key))
        , name_(std::move(name))
        , arguments_(std::move(arguments))
    {}
    
    virtual ~Page() = default;
    
    const LocalKey& key() const { return key_; }
    const std::string& name() const { return name_; }
    const std::any& arguments() const { return arguments_; }
    
    /// إنشاء المسار (Route) من الصفحة
    virtual std::shared_ptr<Route> createRoute(BuildContext* ctx) = 0;
    
private:
    LocalKey key_;
    std::string name_;
    std::any arguments_;
};

/// صفحة Material
class MaterialPage : public Page<void> {
public:
    MaterialPage(
        WidgetPtr child,
        LocalKey key = LocalKey(),
        std::string name = "",
        std::any arguments = {},
        bool maintainState = true,
        bool fullscreenDialog = false
    )
        : Page(std::move(key), std::move(name), std::move(arguments))
        , child_(std::move(child))
        , maintainState_(maintainState)
        , fullscreenDialog_(fullscreenDialog)
    {}
    
    std::shared_ptr<Route> createRoute(BuildContext* ctx) override;
    
    WidgetPtr child() const { return child_; }
    
private:
    WidgetPtr child_;
    bool maintainState_;
    bool fullscreenDialog_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  BackButtonDispatcher — موزع زر الرجوع
// ═══════════════════════════════════════════════════════════════════════════════

class BackButtonDispatcher {
public:
    using Callback = std::function<bool()>; // يُرجع true إذا تم التعامل مع الحدث
    
    void addCallback(Callback callback) {
        callbacks_.push_back(std::move(callback));
    }
    
    bool invokeCallback() {
        // استدعاء من الأخير إلى الأول
        for (auto it = callbacks_.rbegin(); it != callbacks_.rend(); ++it) {
            if ((*it)()) return true;
        }
        return false;
    }
    
private:
    std::vector<Callback> callbacks_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  RouteInformationParser — محلل معلومات المسار
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class RouteInformationParser {
public:
    virtual ~RouteInformationParser() = default;
    
    /// تحليل معلومات المسار إلى تكوين
    virtual T parseRouteInformation(const RouteInformation& info) = 0;
    
    /// استعادة معلومات المسار من التكوين
    virtual RouteInformation restoreRouteInformation(const T& config) = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  RouterDelegate — مندوب الموجه
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class RouterDelegate {
public:
    virtual ~RouterDelegate() = default;
    
    /// التكوين الحالي
    virtual T currentConfiguration() = 0;
    
    /// تعيين مسار جديد
    virtual void setNewRoutePath(const T& config) = 0;
    
    /// بناء الـ Navigator
    virtual WidgetPtr build(BuildContext* ctx) = 0;
    
    /// معالجة زر الرجوع
    virtual bool popRoute() { return false; }
    
    /// إضافة مستمع
    void addListener(std::function<void()> listener) {
        listeners_.push_back(std::move(listener));
    }
    
protected:
    void notifyListeners() {
        for (auto& l : listeners_) l();
    }
    
    std::vector<std::function<void()>> listeners_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  RouteInformationProvider — مزود معلومات المسار
// ═══════════════════════════════════════════════════════════════════════════════

class RouteInformationProvider {
public:
    virtual ~RouteInformationProvider() = default;
    
    /// معلومات المسار الحالية
    virtual RouteInformation value() const = 0;
    
    /// تحديث معلومات المسار
    virtual void routerReportsNewRouteInformation(const RouteInformation& info) = 0;
    
    /// إضافة مستمع
    void addListener(std::function<void()> listener) {
        listeners_.push_back(std::move(listener));
    }
    
protected:
    void notifyListeners() {
        for (auto& l : listeners_) l();
    }
    
    std::vector<std::function<void()>> listeners_;
};

/// مزود معلومات المسار للمنصة
class PlatformRouteInformationProvider : public RouteInformationProvider {
public:
    PlatformRouteInformationProvider(const std::string& initialRoute = "/")
        : currentRoute_(initialRoute)
    {}
    
    RouteInformation value() const override {
        return RouteInformation(currentRoute_);
    }
    
    void routerReportsNewRouteInformation(const RouteInformation& info) override {
        if (currentRoute_ != info.location) {
            currentRoute_ = info.location;
            // TODO: تحديث URL في المتصفح (للويب)
            notifyListeners();
        }
    }
    
private:
    std::string currentRoute_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Router — الموجه (Navigation 2.0)
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class Router : public StatefulWidget {
public:
    Router(
        std::shared_ptr<RouterDelegate<T>> routerDelegate,
        std::shared_ptr<RouteInformationParser<T>> routeInformationParser = nullptr,
        std::shared_ptr<RouteInformationProvider> routeInformationProvider = nullptr,
        std::shared_ptr<BackButtonDispatcher> backButtonDispatcher = nullptr,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , routerDelegate_(std::move(routerDelegate))
        , routeInformationParser_(std::move(routeInformationParser))
        , routeInformationProvider_(std::move(routeInformationProvider))
        , backButtonDispatcher_(std::move(backButtonDispatcher))
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "Router"; }
    
    std::shared_ptr<RouterDelegate<T>> routerDelegate() const { return routerDelegate_; }
    std::shared_ptr<RouteInformationParser<T>> parser() const { return routeInformationParser_; }
    std::shared_ptr<RouteInformationProvider> provider() const { return routeInformationProvider_; }
    
private:
    std::shared_ptr<RouterDelegate<T>> routerDelegate_;
    std::shared_ptr<RouteInformationParser<T>> routeInformationParser_;
    std::shared_ptr<RouteInformationProvider> routeInformationProvider_;
    std::shared_ptr<BackButtonDispatcher> backButtonDispatcher_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  TransitionDelegate — مندوب الانتقالات
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class TransitionDelegate {
public:
    virtual ~TransitionDelegate() = default;
    
    /// تحديد كيفية الانتقال بين المسارات
    virtual std::vector<std::shared_ptr<Route>> resolve(
        std::vector<std::shared_ptr<Route>> newRoutes,
        std::vector<std::shared_ptr<Route>> oldRoutes
    ) {
        return newRoutes; // افتراضياً: استبدال كامل
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  PageTransitionsBuilder — بناء انتقالات الصفحات
// ═══════════════════════════════════════════════════════════════════════════════

enum class PageTransitionType {
    Fade,          // تلاشي
    SlideRight,    // انزلاق من اليمين
    SlideLeft,     // انزلاق من اليسار
    SlideUp,       // انزلاق من الأسفل
    SlideDown,     // انزلاق من الأعلى
    Scale,         // تكبير
    Rotation,      // دوران
    None           // بدون انتقال
};

class PageTransitionsBuilder {
public:
    virtual ~PageTransitionsBuilder() = default;
    
    virtual WidgetPtr buildTransitions(
        BuildContext* ctx,
        std::shared_ptr<Animation<double>> animation,
        std::shared_ptr<Animation<double>> secondaryAnimation,
        WidgetPtr child
    ) = 0;
};

/// انتقال التلاشي
class FadePageTransitionsBuilder : public PageTransitionsBuilder {
public:
    WidgetPtr buildTransitions(
        BuildContext* ctx,
        std::shared_ptr<Animation<double>> animation,
        std::shared_ptr<Animation<double>> secondaryAnimation,
        WidgetPtr child
    ) override {
        return std::make_shared<FadeTransition>(animation, child);
    }
};

/// انتقال الانزلاق
class SlidePageTransitionsBuilder : public PageTransitionsBuilder {
public:
    SlidePageTransitionsBuilder(PageTransitionType type = PageTransitionType::SlideRight)
        : type_(type) {}
    
    WidgetPtr buildTransitions(
        BuildContext* ctx,
        std::shared_ptr<Animation<double>> animation,
        std::shared_ptr<Animation<double>> secondaryAnimation,
        WidgetPtr child
    ) override {
        Position begin, end{0, 0};
        
        switch (type_) {
            case PageTransitionType::SlideRight: begin = {1, 0}; break;
            case PageTransitionType::SlideLeft: begin = {-1, 0}; break;
            case PageTransitionType::SlideUp: begin = {0, 1}; break;
            case PageTransitionType::SlideDown: begin = {0, -1}; break;
            default: begin = {1, 0}; break;
        }
        
        return std::make_shared<SlideTransition>(animation, begin, end, child);
    }
    
private:
    PageTransitionType type_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  GoRouter-style API (مبسط)
// ═══════════════════════════════════════════════════════════════════════════════

/// تعريف مسار
struct GoRoute {
    std::string path;                               // المسار (مثل /users/:id)
    std::function<WidgetPtr(BuildContext*, std::map<std::string, std::string>)> builder;
    std::vector<GoRoute> routes;                    // مسارات فرعية
    std::string name = "";                          // اسم المسار (اختياري)
    
    GoRoute(
        const std::string& p,
        std::function<WidgetPtr(BuildContext*, std::map<std::string, std::string>)> b,
        std::vector<GoRoute> r = {},
        const std::string& n = ""
    )
        : path(p), builder(std::move(b)), routes(std::move(r)), name(n) {}
};

/// تكوين GoRouter
class GoRouterConfig {
public:
    std::string currentPath = "/";
    std::map<std::string, std::string> pathParameters;
    std::map<std::string, std::string> queryParameters;
    
    static GoRouterConfig fromLocation(const std::string& location) {
        GoRouterConfig config;
        config.currentPath = location;
        
        // تحليل Query parameters
        size_t queryStart = location.find('?');
        if (queryStart != std::string::npos) {
            config.currentPath = location.substr(0, queryStart);
            std::string query = location.substr(queryStart + 1);
            
            // تقسيم &
            size_t pos = 0;
            while (pos < query.length()) {
                size_t ampPos = query.find('&', pos);
                if (ampPos == std::string::npos) ampPos = query.length();
                
                std::string param = query.substr(pos, ampPos - pos);
                size_t eqPos = param.find('=');
                if (eqPos != std::string::npos) {
                    config.queryParameters[param.substr(0, eqPos)] = param.substr(eqPos + 1);
                }
                
                pos = ampPos + 1;
            }
        }
        
        return config;
    }
};

/// GoRouter Parser
class GoRouteInformationParser : public RouteInformationParser<GoRouterConfig> {
public:
    GoRouterConfig parseRouteInformation(const RouteInformation& info) override {
        return GoRouterConfig::fromLocation(info.location);
    }
    
    RouteInformation restoreRouteInformation(const GoRouterConfig& config) override {
        std::string location = config.currentPath;
        
        if (!config.queryParameters.empty()) {
            location += "?";
            bool first = true;
            for (const auto& [key, value] : config.queryParameters) {
                if (!first) location += "&";
                location += key + "=" + value;
                first = false;
            }
        }
        
        return RouteInformation(location);
    }
};

/// GoRouter Delegate
class GoRouterDelegate : public RouterDelegate<GoRouterConfig> {
public:
    GoRouterDelegate(std::vector<GoRoute> routes, const std::string& initialRoute = "/")
        : routes_(std::move(routes))
    {
        config_ = GoRouterConfig::fromLocation(initialRoute);
    }
    
    GoRouterConfig currentConfiguration() override {
        return config_;
    }
    
    void setNewRoutePath(const GoRouterConfig& config) override {
        config_ = config;
        notifyListeners();
    }
    
    WidgetPtr build(BuildContext* ctx) override;
    
    bool popRoute() override {
        // TODO: معالجة الرجوع
        return false;
    }
    
    /// التنقل إلى مسار
    void go(const std::string& location) {
        config_ = GoRouterConfig::fromLocation(location);
        notifyListeners();
    }
    
    /// التنقل بالاسم
    void goNamed(const std::string& name, 
                 std::map<std::string, std::string> params = {},
                 std::map<std::string, std::string> query = {}) {
        // TODO: البحث عن المسار بالاسم وبناء URL
    }
    
    /// إضافة مسار جديد (push)
    void push(const std::string& location) {
        history_.push(config_);
        go(location);
    }
    
    /// العودة
    void pop() {
        if (!history_.empty()) {
            config_ = history_.top();
            history_.pop();
            notifyListeners();
        }
    }
    
private:
    std::vector<GoRoute> routes_;
    GoRouterConfig config_;
    std::stack<GoRouterConfig> history_;
    
    WidgetPtr findRoute(const std::string& path, BuildContext* ctx);
    bool matchRoute(const GoRoute& route, const std::string& path,
                   std::map<std::string, std::string>& params);
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Redirect — إعادة التوجيه
// ═══════════════════════════════════════════════════════════════════════════════

class RedirectGuard {
public:
    using RedirectFunction = std::function<std::optional<std::string>(const GoRouterConfig&)>;
    
    void addRedirect(RedirectFunction redirect) {
        redirects_.push_back(std::move(redirect));
    }
    
    std::optional<std::string> check(const GoRouterConfig& config) {
        for (const auto& redirect : redirects_) {
            auto result = redirect(config);
            if (result.has_value()) {
                return result;
            }
        }
        return std::nullopt;
    }
    
private:
    std::vector<RedirectFunction> redirects_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  ShellRoute — مسار القالب
// ═══════════════════════════════════════════════════════════════════════════════

class ShellRoute {
public:
    ShellRoute(
        std::function<WidgetPtr(BuildContext*, GoRouterConfig, WidgetPtr)> builder,
        std::vector<GoRoute> routes
    )
        : builder_(std::move(builder))
        , routes_(std::move(routes))
    {}
    
    WidgetPtr build(BuildContext* ctx, const GoRouterConfig& config, WidgetPtr child) const {
        return builder_(ctx, config, child);
    }
    
    const std::vector<GoRoute>& routes() const { return routes_; }
    
private:
    std::function<WidgetPtr(BuildContext*, GoRouterConfig, WidgetPtr)> builder_;
    std::vector<GoRoute> routes_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Arabic Aliases — أسماء عربية
// ═══════════════════════════════════════════════════════════════════════════════

using معلومات_مسار = RouteInformation;
using اعدادات_مسار = RouteSettings;
using صفحة_مادية = MaterialPage;
using مفتاح_محلي = LocalKey;
using مفتاح_قيمة = ValueKey;

// GoRouter aliases
using تكوين_موجه = GoRouterConfig;
using مسار = GoRoute;

// دوال مساعدة
inline std::shared_ptr<GoRouterDelegate> انشاء_موجه(
    std::initializer_list<GoRoute> مسارات,
    const std::string& مسار_اولي = "/"
) {
    return std::make_shared<GoRouterDelegate>(
        std::vector<GoRoute>(مسارات),
        مسار_اولي
    );
}

inline GoRoute مسار_جديد(
    const std::string& المسار,
    std::function<WidgetPtr(BuildContext*, std::map<std::string, std::string>)> الباني,
    const std::string& الاسم = ""
) {
    return GoRoute(المسار, std::move(الباني), {}, الاسم);
}

inline GoRoute مسار_مع_ابناء(
    const std::string& المسار,
    std::function<WidgetPtr(BuildContext*, std::map<std::string, std::string>)> الباني,
    std::initializer_list<GoRoute> ابناء
) {
    return GoRoute(المسار, std::move(الباني), std::vector<GoRoute>(ابناء));
}

} // namespace flutter
} // namespace sad
