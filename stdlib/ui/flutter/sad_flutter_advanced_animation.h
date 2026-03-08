// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_advanced_animation.h — تأثيرات حركية متقدمة
//  Advanced Animation System
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_FLUTTER_ADVANCED_ANIMATION_H
#define SAD_FLUTTER_ADVANCED_ANIMATION_H

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include "sad_flutter_animation.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Hero Animation — تأثير البطل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مكون Hero للانتقال بين الصفحات
 * @brief (EN) Hero widget for page transitions
 */
class Hero : public StatefulWidget {
public:
    std::string tag;
    WidgetPtr child;
    std::function<WidgetPtr(BuildContext*, Size, WidgetPtr)> flightShuttleBuilder;
    std::function<WidgetPtr(BuildContext*, Animation<double>&, 
                            HeroFlightDirection, BuildContext*, BuildContext*)> placeholderBuilder;
    bool transitionOnUserGestures = false;
    
    Hero(std::string t, WidgetPtr c) : tag(std::move(t)), child(c) {}
    
    std::unique_ptr<State_Base> createState() override;
};

using بطل = Hero;

enum class HeroFlightDirection { Push, Pop };
using اتجاه_طيران_بطل = HeroFlightDirection;

class HeroState : public State<Hero> {
public:
    WidgetPtr build(BuildContext* ctx) override {
        return widget()->child;
    }
private:
    Hero* widget() const { return static_cast<Hero*>(widget_); }
};

inline std::unique_ptr<State_Base> Hero::createState() {
    return std::make_unique<HeroState>();
}

/**
 * @brief (AR) متحكم Hero
 * @brief (EN) Hero controller
 */
class HeroController {
public:
    std::function<void(Route*, Route*)> onPopRoute;
    std::function<void(Route*, Route*)> onPushRoute;
    
    Duration transitionDuration = Duration::milliseconds(300);
    Curve transitionCurve = Curves::fastOutSlowIn();
    
    void didPush(Route* route, Route* previousRoute) {
        if (onPushRoute) onPushRoute(route, previousRoute);
    }
    
    void didPop(Route* route, Route* previousRoute) {
        if (onPopRoute) onPopRoute(route, previousRoute);
    }
};

using متحكم_بطل = HeroController;

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedList — القائمة المتحركة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) حالة القائمة المتحركة العامة
 * @brief (EN) Animated list state interface
 */
class AnimatedListState_Base {
public:
    virtual ~AnimatedListState_Base() = default;
    virtual void insertItem(int index, Duration duration = Duration::milliseconds(300)) = 0;
    virtual void removeItem(int index, std::function<WidgetPtr(BuildContext*, Animation<double>&)> builder,
                           Duration duration = Duration::milliseconds(300)) = 0;
    virtual void removeAllItems(std::function<WidgetPtr(BuildContext*, Animation<double>&)> builder,
                                Duration duration = Duration::milliseconds(300)) = 0;
};

/**
 * @brief (AR) مفتاح القائمة المتحركة العام
 * @brief (EN) Animated list global key
 */
template<typename S>
class GlobalKey {
public:
    S* currentState = nullptr;
};

/**
 * @brief (AR) القائمة المتحركة
 * @brief (EN) Animated list
 */
class AnimatedList : public StatefulWidget {
public:
    std::function<WidgetPtr(BuildContext*, int, Animation<double>&)> itemBuilder;
    int initialItemCount = 0;
    Axis scrollDirection = Axis::Vertical;
    bool reverse = false;
    std::shared_ptr<ScrollController> controller;
    bool primary = false;
    ScrollPhysics physics;
    bool shrinkWrap = false;
    EdgeInsets padding;
    Clip clipBehavior = Clip::HardEdge;
    
    std::unique_ptr<State_Base> createState() override;
};

using قائمة_متحركة = AnimatedList;

class AnimatedListState : public State<AnimatedList>, public AnimatedListState_Base {
public:
    std::vector<int> items_;
    
    void initState() override {
        auto* w = widget();
        for (int i = 0; i < w->initialItemCount; ++i) {
            items_.push_back(i);
        }
    }
    
    void insertItem(int index, Duration duration) override {
        if (index >= 0 && index <= static_cast<int>(items_.size())) {
            items_.insert(items_.begin() + index, static_cast<int>(items_.size()));
            setState([]{});
        }
    }
    
    void removeItem(int index, std::function<WidgetPtr(BuildContext*, Animation<double>&)> builder,
                   Duration duration) override {
        if (index >= 0 && index < static_cast<int>(items_.size())) {
            items_.erase(items_.begin() + index);
            setState([]{});
        }
    }
    
    void removeAllItems(std::function<WidgetPtr(BuildContext*, Animation<double>&)> builder,
                        Duration duration) override {
        items_.clear();
        setState([]{});
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        // Build list with animations
        return nullptr;  // Simplified
    }
    
private:
    AnimatedList* widget() const { return static_cast<AnimatedList*>(widget_); }
};

inline std::unique_ptr<State_Base> AnimatedList::createState() {
    return std::make_unique<AnimatedListState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SliverAnimatedList — القائمة المتحركة (Sliver)
// ═══════════════════════════════════════════════════════════════════════════════

class SliverAnimatedList : public StatefulWidget {
public:
    std::function<WidgetPtr(BuildContext*, int, Animation<double>&)> itemBuilder;
    int initialItemCount = 0;
    
    std::unique_ptr<State_Base> createState() override {
        return nullptr;  // Similar to AnimatedList
    }
};

using قائمة_شريحة_متحركة = SliverAnimatedList;

// ═══════════════════════════════════════════════════════════════════════════════
//  Staggered Animation — التأثيرات المتتالية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فاصل زمني
 * @brief (EN) Interval helper
 */
class Interval : public Curve {
public:
    double begin;
    double end;
    Curve curve;
    
    Interval(double b, double e, Curve c = Curves::linear())
        : begin(b), end(e), curve(c) {}
    
    double transform(double t) const override {
        if (t <= begin) return 0.0;
        if (t >= end) return 1.0;
        double localT = (t - begin) / (end - begin);
        return curve.transform(localT);
    }
};

using فاصل = Interval;

/**
 * @brief (AR) منشئ التأثيرات المتتالية
 * @brief (EN) Staggered animation builder
 */
class StaggeredAnimationBuilder {
public:
    std::shared_ptr<AnimationController> controller;
    
    StaggeredAnimationBuilder(std::shared_ptr<AnimationController> ctrl)
        : controller(ctrl) {}
    
    template<typename T>
    Animation<T> createAnimation(T begin, T end, double startTime, double endTime,
                                  Curve curve = Curves::easeInOut()) {
        // Create animation with interval
        auto tween = Tween<T>(begin, end);
        Interval interval(startTime, endTime, curve);
        // TODO: combine with controller
        return Animation<T>();
    }
};

using منشئ_تأثيرات_متتالية = StaggeredAnimationBuilder;

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedSwitcher — مبدّل متحرك
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مبدّل الأبناء المتحرك
 * @brief (EN) Animated switcher
 */
class AnimatedSwitcher : public StatefulWidget {
public:
    WidgetPtr child;
    Duration duration;
    Duration reverseDuration;
    Curve switchInCurve = Curves::linear();
    Curve switchOutCurve = Curves::linear();
    std::function<WidgetPtr(WidgetPtr, Animation<double>&)> transitionBuilder;
    std::function<WidgetPtr(WidgetPtr, WidgetPtr, List<WidgetPtr>)> layoutBuilder;
    
    AnimatedSwitcher(WidgetPtr c, Duration d)
        : child(c), duration(d), reverseDuration(d) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    static WidgetPtr defaultTransitionBuilder(WidgetPtr child, Animation<double>& animation) {
        return std::make_shared<FadeTransition>(animation, child);
    }
    
    static WidgetPtr defaultLayoutBuilder(WidgetPtr currentChild, WidgetPtr previousChild, 
                                           List<WidgetPtr> previousChildren) {
        return std::make_shared<Stack>(
            std::vector<WidgetPtr>{currentChild}
        );
    }
};

using مبدل_متحرك = AnimatedSwitcher;

class AnimatedSwitcherState : public State<AnimatedSwitcher> {
public:
    WidgetPtr currentChild_;
    
    void initState() override {
        currentChild_ = widget()->child;
    }
    
    void didUpdateWidget(AnimatedSwitcher* oldWidget) override {
        // Check if child changed and animate
        if (widget()->child != oldWidget->child) {
            currentChild_ = widget()->child;
        }
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        return currentChild_;
    }
    
private:
    AnimatedSwitcher* widget() const { return static_cast<AnimatedSwitcher*>(widget_); }
};

inline std::unique_ptr<State_Base> AnimatedSwitcher::createState() {
    return std::make_unique<AnimatedSwitcherState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  PageRouteBuilder — منشئ مسار الصفحة المخصص
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) منشئ مسار صفحة مخصص
 * @brief (EN) Custom page route builder
 */
class PageRouteBuilder : public Route {
public:
    std::function<WidgetPtr(BuildContext*, Animation<double>&, Animation<double>&)> pageBuilder;
    std::function<WidgetPtr(BuildContext*, Animation<double>&, Animation<double>&, WidgetPtr)> transitionsBuilder;
    Duration transitionDuration_ = Duration::milliseconds(300);
    Duration reverseTransitionDuration_ = Duration::milliseconds(300);
    bool opaque_ = true;
    bool barrierDismissible_ = false;
    Color barrierColor_;
    std::string barrierLabel_;
    bool maintainState_ = true;
    bool fullscreenDialog_ = false;
    
    PageRouteBuilder() = default;
    
    Duration transitionDuration() const { return transitionDuration_; }
    Duration reverseTransitionDuration() const { return reverseTransitionDuration_; }
    bool opaque() const { return opaque_; }
    bool barrierDismissible() const { return barrierDismissible_; }
    Color barrierColor() const { return barrierColor_; }
    std::string barrierLabel() const { return barrierLabel_; }
    bool maintainState() const { return maintainState_; }
};

using منشئ_مسار_صفحة = PageRouteBuilder;

// ═══════════════════════════════════════════════════════════════════════════════
//  Transition Widgets — مكونات الانتقال
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) انتقال بالتدوير
 * @brief (EN) Rotation transition
 */
class RotationTransition : public StatelessWidget {
public:
    Animation<double> turns;
    AlignmentGeometry alignment = Alignment::center();
    FilterQuality filterQuality;
    WidgetPtr child;
    
    RotationTransition(Animation<double> t, WidgetPtr c)
        : turns(t), child(c) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using انتقال_تدوير = RotationTransition;

/**
 * @brief (AR) انتقال بتغيير الحجم
 * @brief (EN) Size transition
 */
class SizeTransition : public StatelessWidget {
public:
    Animation<double> sizeFactor;
    Axis axis = Axis::Vertical;
    double axisAlignment = 0.0;
    WidgetPtr child;
    
    SizeTransition(Animation<double> sf, WidgetPtr c)
        : sizeFactor(sf), child(c) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using انتقال_حجم = SizeTransition;

/**
 * @brief (AR) انتقال بقياس
 * @brief (EN) Scale transition
 */
class ScaleTransition : public StatelessWidget {
public:
    Animation<double> scale;
    AlignmentGeometry alignment = Alignment::center();
    FilterQuality filterQuality;
    WidgetPtr child;
    
    ScaleTransition(Animation<double> s, WidgetPtr c)
        : scale(s), child(c) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using انتقال_قياس = ScaleTransition;

/**
 * @brief (AR) انتقال بزخرفة
 * @brief (EN) Decorated box transition
 */
class DecoratedBoxTransition : public StatelessWidget {
public:
    Animation<BoxDecoration> decoration;
    DecorationPosition position = DecorationPosition::Background;
    WidgetPtr child;
    
    DecoratedBoxTransition(Animation<BoxDecoration> d, WidgetPtr c)
        : decoration(d), child(c) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using انتقال_صندوق_مزخرف = DecoratedBoxTransition;

/**
 * @brief (AR) انتقال بمحاذاة
 * @brief (EN) Align transition
 */
class AlignTransition : public StatelessWidget {
public:
    Animation<AlignmentGeometry> alignment;
    double widthFactor = 1.0;
    double heightFactor = 1.0;
    WidgetPtr child;
    
    AlignTransition(Animation<AlignmentGeometry> a, WidgetPtr c)
        : alignment(a), child(c) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using انتقال_محاذاة = AlignTransition;

/**
 * @brief (AR) انتقال بموضع افتراضي
 * @brief (EN) Default text style transition
 */
class DefaultTextStyleTransition : public StatelessWidget {
public:
    Animation<TextStyle> style;
    TextAlign textAlign = TextAlign::Start;
    bool softWrap = true;
    TextOverflow overflow = TextOverflow::Clip;
    int maxLines = 0;
    WidgetPtr child;
    
    DefaultTextStyleTransition(Animation<TextStyle> s, WidgetPtr c)
        : style(s), child(c) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using انتقال_نمط_نص_افتراضي = DefaultTextStyleTransition;

// ═══════════════════════════════════════════════════════════════════════════════
//  TweenAnimationBuilder — منشئ تأثير Tween
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) منشئ تأثير بين قيمتين
 * @brief (EN) Tween animation builder
 */
template<typename T>
class TweenAnimationBuilder : public StatefulWidget {
public:
    Tween<T> tween;
    Duration duration;
    Curve curve = Curves::linear();
    std::function<WidgetPtr(BuildContext*, T, WidgetPtr)> builder;
    WidgetPtr child;
    std::function<void()> onEnd;
    
    TweenAnimationBuilder(Tween<T> t, Duration d,
                          std::function<WidgetPtr(BuildContext*, T, WidgetPtr)> b,
                          WidgetPtr c = nullptr)
        : tween(t), duration(d), builder(b), child(c) {}
    
    std::unique_ptr<State_Base> createState() override {
        return nullptr;  // Simplified
    }
};

template<typename T>
using منشئ_تأثير_بين = TweenAnimationBuilder<T>;

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedWidget — مكون متحرك أساسي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مكون متحرك أساسي
 * @brief (EN) Animated widget base
 */
class AnimatedWidget : public StatefulWidget {
public:
    Listenable listenable;
    
    AnimatedWidget(Listenable l) : listenable(l) {}
    
    virtual WidgetPtr build(BuildContext* ctx) = 0;
    
    std::unique_ptr<State_Base> createState() override;
};

using مكون_متحرك = AnimatedWidget;

class AnimatedWidgetState : public State<AnimatedWidget> {
public:
    void initState() override {
        // TODO: listen to listenable
    }
    
    void dispose() override {
        // TODO: stop listening
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        return widget()->build(ctx);
    }
    
private:
    AnimatedWidget* widget() const { return static_cast<AnimatedWidget*>(widget_); }
};

inline std::unique_ptr<State_Base> AnimatedWidget::createState() {
    return std::make_unique<AnimatedWidgetState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Physics-based Animation — تأثيرات فيزيائية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) محاكاة فيزيائية
 * @brief (EN) Physics simulation
 */
class Simulation {
public:
    virtual ~Simulation() = default;
    
    virtual double x(double time) const = 0;
    virtual double dx(double time) const = 0;
    virtual bool isDone(double time) const = 0;
    
    double tolerance = 0.001;
};

using محاكاة = Simulation;

/**
 * @brief (AR) محاكاة الزنبرك
 * @brief (EN) Spring simulation
 */
class SpringSimulation : public Simulation {
public:
    double mass = 1.0;
    double stiffness = 100.0;
    double damping = 10.0;
    double initialPosition = 0.0;
    double initialVelocity = 0.0;
    double finalPosition = 1.0;
    
    SpringSimulation(double end, double start = 0.0, double velocity = 0.0)
        : finalPosition(end), initialPosition(start), initialVelocity(velocity) {}
    
    double x(double time) const override {
        // Simplified spring physics
        double progress = 1.0 - std::exp(-damping * time / mass);
        return initialPosition + (finalPosition - initialPosition) * progress;
    }
    
    double dx(double time) const override {
        // Simplified velocity
        return (finalPosition - initialPosition) * damping / mass * std::exp(-damping * time / mass);
    }
    
    bool isDone(double time) const override {
        return std::abs(x(time) - finalPosition) < tolerance && 
               std::abs(dx(time)) < tolerance;
    }
};

using محاكاة_زنبرك = SpringSimulation;

/**
 * @brief (AR) وصف الزنبرك
 * @brief (EN) Spring description
 */
struct SpringDescription {
    double mass = 1.0;
    double stiffness = 100.0;
    double damping = 10.0;
    
    static SpringDescription withDampingRatio(double mass, double stiffness, double ratio = 1.0) {
        double damping = ratio * 2.0 * std::sqrt(mass * stiffness);
        return SpringDescription{mass, stiffness, damping};
    }
};

using وصف_زنبرك = SpringDescription;

/**
 * @brief (AR) محاكاة الجاذبية
 * @brief (EN) Gravity simulation
 */
class GravitySimulation : public Simulation {
public:
    double acceleration = 9.8;
    double initialPosition = 0.0;
    double initialVelocity = 0.0;
    double endPosition = 1.0;
    
    double x(double time) const override {
        return initialPosition + initialVelocity * time + 0.5 * acceleration * time * time;
    }
    
    double dx(double time) const override {
        return initialVelocity + acceleration * time;
    }
    
    bool isDone(double time) const override {
        return x(time) >= endPosition;
    }
};

using محاكاة_جاذبية = GravitySimulation;

/**
 * @brief (AR) محاكاة الاحتكاك
 * @brief (EN) Friction simulation
 */
class FrictionSimulation : public Simulation {
public:
    double drag = 0.1;
    double initialPosition = 0.0;
    double initialVelocity = 100.0;
    
    double x(double time) const override {
        // Simplified friction
        return initialPosition + initialVelocity * (1.0 - std::exp(-drag * time)) / drag;
    }
    
    double dx(double time) const override {
        return initialVelocity * std::exp(-drag * time);
    }
    
    bool isDone(double time) const override {
        return std::abs(dx(time)) < tolerance;
    }
};

using محاكاة_احتكاك = FrictionSimulation;

// ═══════════════════════════════════════════════════════════════════════════════
//  Implicit Animations — تأثيرات ضمنية إضافية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) تدرج متحرك
 * @brief (EN) Animated cross fade
 */
class AnimatedCrossFade : public StatefulWidget {
public:
    WidgetPtr firstChild;
    WidgetPtr secondChild;
    CrossFadeState crossFadeState = CrossFadeState::ShowFirst;
    Duration duration;
    Duration reverseDuration;
    Curve firstCurve = Curves::linear();
    Curve secondCurve = Curves::linear();
    Curve sizeCurve = Curves::linear();
    AlignmentGeometry alignment = Alignment::topCenter();
    std::function<WidgetPtr(WidgetPtr, Key, WidgetPtr, Key)> layoutBuilder;
    bool excludeBottomFocus = true;
    
    AnimatedCrossFade(WidgetPtr first, WidgetPtr second, Duration d)
        : firstChild(first), secondChild(second), duration(d), reverseDuration(d) {}
    
    std::unique_ptr<State_Base> createState() override { return nullptr; }
    
    static WidgetPtr defaultLayoutBuilder(WidgetPtr topChild, Key topKey,
                                           WidgetPtr bottomChild, Key bottomKey) {
        return std::make_shared<Stack>(
            std::vector<WidgetPtr>{bottomChild, topChild}
        );
    }
};

using تبديل_متحرك = AnimatedCrossFade;

enum class CrossFadeState { ShowFirst, ShowSecond };
using حالة_تبديل = CrossFadeState;

/**
 * @brief (AR) سلسلة تأثيرات
 * @brief (EN) Animation sequence
 */
template<typename T>
class TweenSequence {
public:
    struct Item {
        Tween<T> tween;
        double weight;
        Curve curve = Curves::linear();
    };
    
    std::vector<Item> items;
    
    TweenSequence(std::initializer_list<Item> list) : items(list) {}
    
    T evaluate(double t) const {
        double totalWeight = 0;
        for (const auto& item : items) {
            totalWeight += item.weight;
        }
        
        double currentWeight = 0;
        for (const auto& item : items) {
            double itemProgress = item.weight / totalWeight;
            if (t <= currentWeight + itemProgress) {
                double localT = (t - currentWeight) / itemProgress;
                localT = item.curve.transform(localT);
                return item.tween.lerp(localT);
            }
            currentWeight += itemProgress;
        }
        
        return items.back().tween.end;
    }
};

template<typename T>
using سلسلة_تأثيرات = TweenSequence<T>;

} // namespace flutter
} // namespace sad

#endif // SAD_FLUTTER_ADVANCED_ANIMATION_H
