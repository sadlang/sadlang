// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_navigation.cpp — تطبيق نظام التنقل 2.0
//  Navigation 2.0 Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_navigation.h"
#include "sad_flutter_layout.h"
#include "sad_flutter_app.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  MaterialPage
// ═══════════════════════════════════════════════════════════════════════════════

std::shared_ptr<Route> MaterialPage::createRoute(BuildContext* ctx) {
    return std::make_shared<MaterialPageRoute>(
        [this](BuildContext*) { return child_; },
        RouteSettings(name(), arguments())
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Router State
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class RouterState : public State<Router<T>> {
public:
    void initState() override {
        auto* w = widget();
        
        // الاستماع للتغييرات
        if (w->routerDelegate()) {
            w->routerDelegate()->addListener([this] {
                this->setState([]{});
            });
        }
        
        if (w->provider()) {
            w->provider()->addListener([this] {
                handleRouteInformationChanged();
            });
        }
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        return widget()->routerDelegate()->build(ctx);
    }
    
private:
    void handleRouteInformationChanged() {
        auto* w = widget();
        if (w->provider() && w->parser() && w->routerDelegate()) {
            auto info = w->provider()->value();
            auto config = w->parser()->parseRouteInformation(info);
            w->routerDelegate()->setNewRoutePath(config);
        }
    }
    
    Router<T>* widget() { 
        return static_cast<Router<T>*>(this->widget_); 
    }
};

template<typename T>
std::unique_ptr<State_Base> Router<T>::createState() {
    return std::make_unique<RouterState<T>>();
}

// Explicit instantiation for GoRouterConfig
template class Router<GoRouterConfig>;

// ═══════════════════════════════════════════════════════════════════════════════
//  GoRouterDelegate Implementation
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr GoRouterDelegate::build(BuildContext* ctx) {
    return findRoute(config_.currentPath, ctx);
}

WidgetPtr GoRouterDelegate::findRoute(const std::string& path, BuildContext* ctx) {
    std::map<std::string, std::string> params;
    
    // البحث في المسارات
    for (const auto& route : routes_) {
        if (matchRoute(route, path, params)) {
            config_.pathParameters = params;
            return route.builder(ctx, params);
        }
    }
    
    // صفحة 404
    return std::make_shared<Center>(
        std::make_shared<Column>(
            std::vector<WidgetPtr>{
                std::make_shared<Text>("404", TextStyle{.fontSize = 48}),
                std::make_shared<SizedBox>(std::nullopt, 16.0f),
                std::make_shared<Text>("الصفحة غير موجودة: " + path),
                std::make_shared<SizedBox>(std::nullopt, 24.0f),
                std::make_shared<ElevatedButton>(
                    std::make_shared<Text>("العودة للرئيسية"),
                    [this]{ go("/"); }
                )
            },
            MainAxisAlignment::Center,
            MainAxisSize::Min
        )
    );
}

bool GoRouterDelegate::matchRoute(
    const GoRoute& route, 
    const std::string& path,
    std::map<std::string, std::string>& params
) {
    // المسار الثابت
    if (route.path == path) {
        return true;
    }
    
    // المسار مع معاملات (مثل /users/:id)
    if (route.path.find(':') != std::string::npos) {
        std::vector<std::string> routeParts, pathParts;
        
        // تقسيم المسارات
        size_t pos = 0;
        std::string routePath = route.path;
        std::string targetPath = path;
        
        // تقسيم route.path
        while (!routePath.empty()) {
            if (routePath[0] == '/') routePath = routePath.substr(1);
            size_t nextSlash = routePath.find('/');
            if (nextSlash == std::string::npos) {
                if (!routePath.empty()) routeParts.push_back(routePath);
                break;
            }
            routeParts.push_back(routePath.substr(0, nextSlash));
            routePath = routePath.substr(nextSlash);
        }
        
        // تقسيم path
        while (!targetPath.empty()) {
            if (targetPath[0] == '/') targetPath = targetPath.substr(1);
            size_t nextSlash = targetPath.find('/');
            if (nextSlash == std::string::npos) {
                if (!targetPath.empty()) pathParts.push_back(targetPath);
                break;
            }
            pathParts.push_back(targetPath.substr(0, nextSlash));
            targetPath = targetPath.substr(nextSlash);
        }
        
        // التحقق من التطابق
        if (routeParts.size() != pathParts.size()) {
            return false;
        }
        
        for (size_t i = 0; i < routeParts.size(); ++i) {
            if (routeParts[i][0] == ':') {
                // معامل
                std::string paramName = routeParts[i].substr(1);
                params[paramName] = pathParts[i];
            } else if (routeParts[i] != pathParts[i]) {
                return false;
            }
        }
        
        return true;
    }
    
    // التحقق من المسارات الفرعية
    for (const auto& subRoute : route.routes) {
        std::string fullPath = route.path + subRoute.path;
        if (matchRoute(GoRoute(fullPath, subRoute.builder, subRoute.routes), path, params)) {
            return true;
        }
    }
    
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Navigation Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

/// إنشاء GoRouter مع مسارات
std::shared_ptr<Router<GoRouterConfig>> createGoRouter(
    std::vector<GoRoute> routes,
    const std::string& initialRoute,
    std::function<std::optional<std::string>(const GoRouterConfig&)> redirect
) {
    auto delegate = std::make_shared<GoRouterDelegate>(std::move(routes), initialRoute);
    auto parser = std::make_shared<GoRouteInformationParser>();
    auto provider = std::make_shared<PlatformRouteInformationProvider>(initialRoute);
    
    return std::make_shared<Router<GoRouterConfig>>(
        delegate,
        parser,
        provider
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Page Transitions
// ═══════════════════════════════════════════════════════════════════════════════

/// انتقال صفحة مخصص
class CustomPageRoute : public MaterialPageRoute {
public:
    CustomPageRoute(
        std::function<WidgetPtr(BuildContext*)> builder,
        PageTransitionType transitionType = PageTransitionType::SlideRight,
        Duration transitionDuration = Duration::milliseconds(300),
        RouteSettings settings = RouteSettings()
    )
        : MaterialPageRoute(std::move(builder), std::move(settings))
        , transitionType_(transitionType)
        , transitionDuration_(transitionDuration)
    {}
    
    WidgetPtr buildTransitions(
        BuildContext* ctx,
        std::shared_ptr<Animation<double>> animation,
        WidgetPtr child
    ) {
        switch (transitionType_) {
            case PageTransitionType::Fade:
                return std::make_shared<FadeTransition>(animation, child);
                
            case PageTransitionType::SlideRight:
                return std::make_shared<SlideTransition>(
                    animation, Position{1, 0}, Position{0, 0}, child
                );
                
            case PageTransitionType::SlideLeft:
                return std::make_shared<SlideTransition>(
                    animation, Position{-1, 0}, Position{0, 0}, child
                );
                
            case PageTransitionType::SlideUp:
                return std::make_shared<SlideTransition>(
                    animation, Position{0, 1}, Position{0, 0}, child
                );
                
            case PageTransitionType::SlideDown:
                return std::make_shared<SlideTransition>(
                    animation, Position{0, -1}, Position{0, 0}, child
                );
                
            case PageTransitionType::Scale:
                return std::make_shared<ScaleTransition>(animation, child);
                
            case PageTransitionType::None:
            default:
                return child;
        }
    }
    
private:
    PageTransitionType transitionType_;
    Duration transitionDuration_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Nested Navigation
// ═══════════════════════════════════════════════════════════════════════════════

/// Navigator متداخل (لكل Tab خاص)
class NestedNavigator : public StatefulWidget {
public:
    NestedNavigator(
        std::string navigatorKey,
        WidgetPtr initialRoute,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , navigatorKey_(std::move(navigatorKey))
        , initialRoute_(std::move(initialRoute))
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "NestedNavigator"; }
    
private:
    std::string navigatorKey_;
    WidgetPtr initialRoute_;
    
    friend class NestedNavigatorState;
};

class NestedNavigatorState : public State<NestedNavigator> {
public:
    void initState() override {
        // إنشاء Navigator محلي
    }
    
    void push(WidgetPtr page) {
        pages_.push_back(std::move(page));
        setState([]{});
    }
    
    bool pop() {
        if (pages_.size() > 1) {
            pages_.pop_back();
            setState([]{});
            return true;
        }
        return false;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = static_cast<NestedNavigator*>(widget_);
        
        if (pages_.empty()) {
            pages_.push_back(w->initialRoute_);
        }
        
        return pages_.back();
    }
    
private:
    std::vector<WidgetPtr> pages_;
};

std::unique_ptr<State_Base> NestedNavigator::createState() {
    return std::make_unique<NestedNavigatorState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Tab Navigation with GoRouter
// ═══════════════════════════════════════════════════════════════════════════════

/// StatefulShellRoute للتنقل بين التبويبات
class StatefulShellRoute {
public:
    struct Branch {
        std::vector<GoRoute> routes;
        WidgetPtr navigatorKey;
    };
    
    StatefulShellRoute(
        std::function<WidgetPtr(BuildContext*, GoRouterConfig, WidgetPtr)> builder,
        std::vector<Branch> branches
    )
        : builder_(std::move(builder))
        , branches_(std::move(branches))
    {}
    
    WidgetPtr build(BuildContext* ctx, const GoRouterConfig& config, int currentIndex) {
        // بناء الطفل من الفرع الحالي
        // TODO: تطبيق كامل
        return nullptr;
    }
    
private:
    std::function<WidgetPtr(BuildContext*, GoRouterConfig, WidgetPtr)> builder_;
    std::vector<Branch> branches_;
};

} // namespace flutter
} // namespace sad
