// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_app.h — نظام التطبيق بأسلوب Flutter
//  Application Framework: runApp, WidgetsApp, MaterialApp, Theme, Navigator
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_layout.h"
#include <stack>
#include <unordered_map>
#include <memory>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Forward Declarations
// ═══════════════════════════════════════════════════════════════════════════════

class Navigator;
class NavigatorState;
class Route;
class ThemeData;
class BuildOwner;
class WidgetsBinding;

// ═══════════════════════════════════════════════════════════════════════════════
//  ThemeData — بيانات السمة
// ═══════════════════════════════════════════════════════════════════════════════

/// سمة التطبيق - الألوان والخطوط
class ThemeData {
public:
    // Primary colors
    Color primaryColor = Color::blue();
    Color primaryColorLight = Color(100, 181, 246);
    Color primaryColorDark = Color(25, 118, 210);
    Color accentColor = Color::orange();
    
    // Background colors
    Color backgroundColor = Color::white();
    Color scaffoldBackgroundColor = Color(250, 250, 250);
    Color cardColor = Color::white();
    Color dialogBackgroundColor = Color::white();
    
    // Text colors
    Color textColor = Color::black();
    Color textColorSecondary = Color::grey();
    Color hintColor = Color(158, 158, 158);
    
    // Other colors
    Color errorColor = Color::red();
    Color successColor = Color::green();
    Color warningColor = Color::orange();
    Color dividerColor = Color(224, 224, 224);
    
    // Typography
    float fontSizeSmall = 12;
    float fontSizeMedium = 14;
    float fontSizeLarge = 18;
    float fontSizeHeadline = 24;
    float fontSizeTitle = 20;
    std::string fontFamily;
    
    // Shapes
    float borderRadiusSmall = 4;
    float borderRadiusMedium = 8;
    float borderRadiusLarge = 16;
    
    // Elevation (shadows)
    float elevationSmall = 2;
    float elevationMedium = 4;
    float elevationLarge = 8;
    
    // Spacing
    float spacingSmall = 4;
    float spacingMedium = 8;
    float spacingLarge = 16;
    float spacingXLarge = 24;
    
    // Button styles
    EdgeInsets buttonPadding = EdgeInsets::symmetric(12, 24);
    float buttonBorderRadius = 8;
    
    /// سمة فاتحة افتراضية
    static ThemeData light() {
        return ThemeData();
    }
    
    /// سمة داكنة
    static ThemeData dark() {
        ThemeData theme;
        theme.primaryColor = Color(100, 181, 246);
        theme.backgroundColor = Color(18, 18, 18);
        theme.scaffoldBackgroundColor = Color(30, 30, 30);
        theme.cardColor = Color(48, 48, 48);
        theme.dialogBackgroundColor = Color(48, 48, 48);
        theme.textColor = Color::white();
        theme.textColorSecondary = Color(176, 176, 176);
        theme.dividerColor = Color(66, 66, 66);
        return theme;
    }
    
    /// نسخ مع تعديلات
    ThemeData copyWith(
        std::optional<Color> primaryColor = std::nullopt,
        std::optional<Color> accentColor = std::nullopt,
        std::optional<Color> backgroundColor = std::nullopt
    ) const {
        ThemeData copy = *this;
        if (primaryColor) copy.primaryColor = *primaryColor;
        if (accentColor) copy.accentColor = *accentColor;
        if (backgroundColor) copy.backgroundColor = *backgroundColor;
        return copy;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Theme Widget — مزود السمة
// ═══════════════════════════════════════════════════════════════════════════════

class Theme : public InheritedWidget {
public:
    Theme(ThemeData data, WidgetPtr child, Key key = Key())
        : InheritedWidget(std::move(child), std::move(key))
        , data_(std::move(data))
    {}
    
    const ThemeData& data() const { return data_; }
    
    /// الحصول على السمة من السياق
    static const ThemeData& of(BuildContext* context) {
        auto* theme = context->findAncestorWidgetOfExactType<Theme>();
        if (theme) {
            return theme->data();
        }
        static ThemeData defaultTheme;
        return defaultTheme;
    }
    
    bool updateShouldNotify(const InheritedWidget* oldWidget) const override {
        // Compare themes - simplified
        return true;
    }
    
    std::string typeName() const override { return "Theme"; }
    
private:
    ThemeData data_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  MediaQuery — معلومات الشاشة
// ═══════════════════════════════════════════════════════════════════════════════

struct MediaQueryData {
    Size size = {0, 0};
    float devicePixelRatio = 1.0f;
    float textScaleFactor = 1.0f;
    EdgeInsets padding;        // Safe area
    EdgeInsets viewInsets;     // Keyboard, etc.
    EdgeInsets viewPadding;
    bool alwaysUse24HourFormat = false;
    bool accessibleNavigation = false;
    bool invertColors = false;
    bool highContrast = false;
    bool disableAnimations = false;
    bool boldText = false;
    
    /// عرض الشاشة
    float width() const { return size.width; }
    
    /// ارتفاع الشاشة
    float height() const { return size.height; }
    
    /// هل الجهاز في الوضع الأفقي؟
    bool isLandscape() const { return size.width > size.height; }
    
    /// هل الجهاز في الوضع العمودي؟
    bool isPortrait() const { return size.height > size.width; }
};

class MediaQuery : public InheritedWidget {
public:
    MediaQuery(MediaQueryData data, WidgetPtr child, Key key = Key())
        : InheritedWidget(std::move(child), std::move(key))
        , data_(std::move(data))
    {}
    
    const MediaQueryData& data() const { return data_; }
    
    /// الحصول على معلومات الشاشة من السياق
    static const MediaQueryData& of(BuildContext* context) {
        auto* mq = context->findAncestorWidgetOfExactType<MediaQuery>();
        if (mq) {
            return mq->data();
        }
        static MediaQueryData defaultData;
        return defaultData;
    }
    
    /// الحصول على الحجم فقط
    static Size sizeOf(BuildContext* context) {
        return of(context).size;
    }
    
    bool updateShouldNotify(const InheritedWidget* oldWidget) const override {
        return true;
    }
    
    std::string typeName() const override { return "MediaQuery"; }
    
private:
    MediaQueryData data_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Route — المسار
// ═══════════════════════════════════════════════════════════════════════════════

/// إعدادات المسار
struct RouteSettings {
    std::string name;
    std::any arguments;
};

/// مسار أساسي
class Route {
public:
    virtual ~Route() = default;
    
    RouteSettings settings;
    Navigator* navigator = nullptr;
    
    /// بناء المحتوى
    virtual WidgetPtr build(BuildContext* context) = 0;
    
    /// هل يمكن الخروج من المسار؟
    virtual bool canPop() const { return true; }
    
    /// عند الدخول للمسار
    virtual void didPush() {}
    
    /// عند الخروج من المسار
    virtual void didPop() {}
    
    /// عند العودة للمسار
    virtual void didPopNext() {}
};

/// مسار صفحة (مع انتقال)
class PageRoute : public Route {
public:
    virtual bool get maintainState() const { return true; }
    virtual bool get fullscreenDialog() const { return false; }
};

/// مسار صفحة مع builder
class MaterialPageRoute : public PageRoute {
public:
    using WidgetBuilder = std::function<WidgetPtr(BuildContext*)>;
    
    MaterialPageRoute(WidgetBuilder builder, RouteSettings settings = {})
        : builder_(std::move(builder))
    {
        this->settings = std::move(settings);
    }
    
    WidgetPtr build(BuildContext* context) override {
        return builder_(context);
    }
    
private:
    WidgetBuilder builder_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Navigator — المتنقل
// ═══════════════════════════════════════════════════════════════════════════════

/// نطاق Navigator
class NavigatorState;

class Navigator : public StatefulWidget {
public:
    using RouteFactory = std::function<std::shared_ptr<Route>(const RouteSettings&)>;
    
    Navigator(
        std::string initialRoute = "/",
        RouteFactory onGenerateRoute = nullptr,
        std::unordered_map<std::string, WidgetBuilder> routes = {},
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , initialRoute_(std::move(initialRoute))
        , onGenerateRoute_(std::move(onGenerateRoute))
        , routes_(std::move(routes))
    {}
    
    const std::string& initialRoute() const { return initialRoute_; }
    const RouteFactory& onGenerateRoute() const { return onGenerateRoute_; }
    const std::unordered_map<std::string, WidgetBuilder>& routes() const { return routes_; }
    
    /// الحصول على Navigator من السياق
    static NavigatorState* of(BuildContext* context);
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "Navigator"; }
    
private:
    std::string initialRoute_;
    RouteFactory onGenerateRoute_;
    std::unordered_map<std::string, WidgetBuilder> routes_;
    
    using WidgetBuilder = std::function<WidgetPtr(BuildContext*)>;
};

/// حالة Navigator
class NavigatorState : public State<Navigator> {
public:
    /// دفع مسار جديد
    void push(std::shared_ptr<Route> route) {
        route->navigator = static_cast<Navigator*>(widget().get());
        routes_.push(std::move(route));
        routes_.top()->didPush();
        setState([]{});
    }
    
    /// دفع مسار باسم
    void pushNamed(const std::string& routeName, std::any arguments = {}) {
        RouteSettings settings{routeName, std::move(arguments)};
        auto w = widget();
        
        // Try named routes map first
        auto it = w->routes().find(routeName);
        if (it != w->routes().end()) {
            auto route = std::make_shared<MaterialPageRoute>(it->second, settings);
            push(route);
            return;
        }
        
        // Try onGenerateRoute
        if (w->onGenerateRoute()) {
            auto route = w->onGenerateRoute()(settings);
            if (route) {
                push(route);
            }
        }
    }
    
    /// إزالة المسار الحالي
    void pop() {
        if (routes_.size() > 1) {
            auto top = routes_.top();
            routes_.pop();
            top->didPop();
            if (!routes_.empty()) {
                routes_.top()->didPopNext();
            }
            setState([]{});
        }
    }
    
    /// إزالة المسار مع نتيجة
    template<typename T>
    void popWithResult(T result) {
        // Store result for the previous route to retrieve
        pop();
    }
    
    /// استبدال المسار الحالي
    void pushReplacement(std::shared_ptr<Route> route) {
        if (!routes_.empty()) {
            routes_.pop();
        }
        push(std::move(route));
    }
    
    /// العودة للمسار الأول
    void popUntil(std::function<bool(Route*)> predicate) {
        while (routes_.size() > 1 && !predicate(routes_.top().get())) {
            auto top = routes_.top();
            routes_.pop();
            top->didPop();
        }
        setState([]{});
    }
    
    /// هل يمكن العودة؟
    bool canPop() const {
        return routes_.size() > 1 && routes_.top()->canPop();
    }
    
    void initState() override {
        // Push initial route
        auto w = widget();
        pushNamed(w->initialRoute());
    }
    
    WidgetPtr build(BuildContext* context) override {
        if (routes_.empty()) {
            return std::make_shared<SizedBox>();
        }
        return routes_.top()->build(context);
    }
    
private:
    std::stack<std::shared_ptr<Route>> routes_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Scaffold — الهيكل الأساسي للصفحة
// ═══════════════════════════════════════════════════════════════════════════════

/// شريط التطبيق
class AppBar : public StatelessWidget {
public:
    AppBar(
        std::string title = "",
        WidgetPtr titleWidget = nullptr,
        WidgetPtr leading = nullptr,
        std::vector<WidgetPtr> actions = {},
        Color backgroundColor = Color::blue(),
        Color foregroundColor = Color::white(),
        float elevation = 4,
        bool automaticallyImplyLeading = true,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , title_(std::move(title))
        , titleWidget_(std::move(titleWidget))
        , leading_(std::move(leading))
        , actions_(std::move(actions))
        , backgroundColor_(backgroundColor)
        , foregroundColor_(foregroundColor)
        , elevation_(elevation)
        , automaticallyImplyLeading_(automaticallyImplyLeading)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "AppBar"; }
    
private:
    std::string title_;
    WidgetPtr titleWidget_;
    WidgetPtr leading_;
    std::vector<WidgetPtr> actions_;
    Color backgroundColor_, foregroundColor_;
    float elevation_;
    bool automaticallyImplyLeading_;
};

/// الزر العائم
class FloatingActionButton : public StatelessWidget {
public:
    FloatingActionButton(
        VoidCallback onPressed,
        WidgetPtr child = nullptr,
        Color backgroundColor = Color::orange(),
        Color foregroundColor = Color::white(),
        float elevation = 6,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , onPressed_(std::move(onPressed))
        , child_(std::move(child))
        , backgroundColor_(backgroundColor)
        , foregroundColor_(foregroundColor)
        , elevation_(elevation)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "FloatingActionButton"; }
    
private:
    VoidCallback onPressed_;
    WidgetPtr child_;
    Color backgroundColor_, foregroundColor_;
    float elevation_;
};

/// شريط التنقل السفلي
class BottomNavigationBar : public StatelessWidget {
public:
    struct Item {
        Icons icon;
        std::string label;
    };
    
    BottomNavigationBar(
        std::vector<Item> items,
        int currentIndex = 0,
        std::function<void(int)> onTap = nullptr,
        Color backgroundColor = Color::white(),
        Color selectedItemColor = Color::blue(),
        Color unselectedItemColor = Color::grey(),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , items_(std::move(items))
        , currentIndex_(currentIndex)
        , onTap_(std::move(onTap))
        , backgroundColor_(backgroundColor)
        , selectedItemColor_(selectedItemColor)
        , unselectedItemColor_(unselectedItemColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "BottomNavigationBar"; }
    
private:
    std::vector<Item> items_;
    int currentIndex_;
    std::function<void(int)> onTap_;
    Color backgroundColor_, selectedItemColor_, unselectedItemColor_;
};

/// الهيكل الأساسي للصفحة
class Scaffold : public StatelessWidget {
public:
    Scaffold(
        WidgetPtr body = nullptr,
        std::shared_ptr<AppBar> appBar = nullptr,
        std::shared_ptr<FloatingActionButton> floatingActionButton = nullptr,
        std::shared_ptr<BottomNavigationBar> bottomNavigationBar = nullptr,
        WidgetPtr drawer = nullptr,
        Color backgroundColor = Color(250, 250, 250),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , body_(std::move(body))
        , appBar_(std::move(appBar))
        , floatingActionButton_(std::move(floatingActionButton))
        , bottomNavigationBar_(std::move(bottomNavigationBar))
        , drawer_(std::move(drawer))
        , backgroundColor_(backgroundColor)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "Scaffold"; }
    
private:
    WidgetPtr body_;
    std::shared_ptr<AppBar> appBar_;
    std::shared_ptr<FloatingActionButton> floatingActionButton_;
    std::shared_ptr<BottomNavigationBar> bottomNavigationBar_;
    WidgetPtr drawer_;
    Color backgroundColor_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  WidgetsApp — تطبيق المكونات الأساسي
// ═══════════════════════════════════════════════════════════════════════════════

class WidgetsApp : public StatefulWidget {
public:
    using RouteFactory = Navigator::RouteFactory;
    using WidgetBuilder = std::function<WidgetPtr(BuildContext*)>;
    
    WidgetsApp(
        WidgetPtr home = nullptr,
        std::string initialRoute = "/",
        std::unordered_map<std::string, WidgetBuilder> routes = {},
        RouteFactory onGenerateRoute = nullptr,
        ThemeData theme = ThemeData::light(),
        std::string title = "",
        bool debugShowCheckedModeBanner = true,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , home_(std::move(home))
        , initialRoute_(std::move(initialRoute))
        , routes_(std::move(routes))
        , onGenerateRoute_(std::move(onGenerateRoute))
        , theme_(std::move(theme))
        , title_(std::move(title))
        , debugShowCheckedModeBanner_(debugShowCheckedModeBanner)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "WidgetsApp"; }
    
    WidgetPtr home() const { return home_; }
    const std::string& initialRoute() const { return initialRoute_; }
    const auto& routes() const { return routes_; }
    const RouteFactory& onGenerateRoute() const { return onGenerateRoute_; }
    const ThemeData& theme() const { return theme_; }
    const std::string& title() const { return title_; }
    bool debugShowCheckedModeBanner() const { return debugShowCheckedModeBanner_; }
    
private:
    WidgetPtr home_;
    std::string initialRoute_;
    std::unordered_map<std::string, WidgetBuilder> routes_;
    RouteFactory onGenerateRoute_;
    ThemeData theme_;
    std::string title_;
    bool debugShowCheckedModeBanner_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  MaterialApp — تطبيق Material Design
// ═══════════════════════════════════════════════════════════════════════════════

/// تطبيق بأسلوب Material Design
class MaterialApp : public WidgetsApp {
public:
    MaterialApp(
        WidgetPtr home = nullptr,
        std::string initialRoute = "/",
        std::unordered_map<std::string, WidgetBuilder> routes = {},
        RouteFactory onGenerateRoute = nullptr,
        ThemeData theme = ThemeData::light(),
        ThemeData darkTheme = ThemeData::dark(),
        bool useDarkTheme = false,
        std::string title = "",
        bool debugShowCheckedModeBanner = true,
        Key key = Key()
    )
        : WidgetsApp(
            std::move(home), std::move(initialRoute), std::move(routes),
            std::move(onGenerateRoute),
            useDarkTheme ? darkTheme : theme,
            std::move(title), debugShowCheckedModeBanner, std::move(key)
          )
        , darkTheme_(std::move(darkTheme))
        , useDarkTheme_(useDarkTheme)
    {}
    
    std::string typeName() const override { return "MaterialApp"; }
    
private:
    ThemeData darkTheme_;
    bool useDarkTheme_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  WidgetsBinding — ربط المكونات بالمنصة
// ═══════════════════════════════════════════════════════════════════════════════

/// معلومات التطبيق
struct AppInfo {
    std::string title;
    Size windowSize = {800, 600};
    bool resizable = true;
    bool fullscreen = false;
};

/// الربط بين المكونات والمنصة
class WidgetsBinding {
public:
    static WidgetsBinding& instance() {
        static WidgetsBinding binding;
        return binding;
    }
    
    /// تعيين جذر التطبيق
    void attachRootWidget(WidgetPtr rootWidget) {
        rootWidget_ = std::move(rootWidget);
        rootElement_ = rootWidget_->createElement();
        rootElement_->mount(nullptr);
    }
    
    /// تشغيل حلقة الأحداث
    void runApp() {
        running_ = true;
        // Platform-specific event loop
        while (running_) {
            processEvents();
            if (needsRebuild_) {
                rebuild();
                needsRebuild_ = false;
            }
            render();
        }
    }
    
    /// إيقاف التطبيق
    void stop() {
        running_ = false;
    }
    
    /// جدولة إعادة البناء
    void scheduleRebuild() {
        needsRebuild_ = true;
    }
    
    /// معلومات الشاشة
    MediaQueryData mediaQueryData() const {
        return mediaQueryData_;
    }
    
    void setMediaQueryData(MediaQueryData data) {
        mediaQueryData_ = std::move(data);
    }
    
private:
    WidgetsBinding() = default;
    
    void processEvents() {
        // Platform-specific: poll events, handle input
    }
    
    void rebuild() {
        if (rootElement_) {
            rootElement_->rebuild();
        }
    }
    
    void render() {
        // Platform-specific: render the element tree
    }
    
    WidgetPtr rootWidget_;
    std::unique_ptr<Element> rootElement_;
    MediaQueryData mediaQueryData_;
    bool running_ = false;
    bool needsRebuild_ = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  runApp — تشغيل التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

/// تشغيل التطبيق — نقطة الدخول الرئيسية
inline void runApp(WidgetPtr app) {
    WidgetsBinding::instance().attachRootWidget(std::move(app));
    WidgetsBinding::instance().runApp();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Arabic Aliases — أسماء عربية
// ═══════════════════════════════════════════════════════════════════════════════

using بيانات_سمة = ThemeData;
using سمة = Theme;
using بيانات_استعلام_وسائط = MediaQueryData;
using استعلام_وسائط = MediaQuery;
using مسار = Route;
using مسار_صفحة = MaterialPageRoute;
using متنقل = Navigator;
using حالة_متنقل = NavigatorState;
using شريط_تطبيق = AppBar;
using زر_عائم = FloatingActionButton;
using شريط_تنقل_سفلي = BottomNavigationBar;
using هيكل = Scaffold;
using تطبيق = MaterialApp;

/// تشغيل التطبيق
inline void شغل(WidgetPtr تطبيق) {
    runApp(std::move(تطبيق));
}

} // namespace flutter
} // namespace sad
