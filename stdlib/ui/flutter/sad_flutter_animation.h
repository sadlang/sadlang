// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_animation.h — نظام الرسوم المتحركة بأسلوب Flutter
//  Animation System: AnimationController, Tween, Curves, Transitions
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include <chrono>
#include <cmath>
#include <vector>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Duration — المدة الزمنية
// ═══════════════════════════════════════════════════════════════════════════════

class Duration {
public:
    constexpr Duration() : microseconds_(0) {}
    
    static constexpr Duration microseconds(int64_t us) { Duration d; d.microseconds_ = us; return d; }
    static constexpr Duration milliseconds(int64_t ms) { return microseconds(ms * 1000); }
    static constexpr Duration seconds(int64_t s) { return milliseconds(s * 1000); }
    
    int64_t inMicroseconds() const { return microseconds_; }
    int64_t inMilliseconds() const { return microseconds_ / 1000; }
    double inSeconds() const { return microseconds_ / 1000000.0; }
    
    Duration operator+(const Duration& other) const {
        return microseconds(microseconds_ + other.microseconds_);
    }
    
    Duration operator-(const Duration& other) const {
        return microseconds(microseconds_ - other.microseconds_);
    }
    
    Duration operator*(double factor) const {
        return microseconds(static_cast<int64_t>(microseconds_ * factor));
    }
    
    bool operator<(const Duration& other) const { return microseconds_ < other.microseconds_; }
    bool operator<=(const Duration& other) const { return microseconds_ <= other.microseconds_; }
    bool operator>(const Duration& other) const { return microseconds_ > other.microseconds_; }
    bool operator>=(const Duration& other) const { return microseconds_ >= other.microseconds_; }
    bool operator==(const Duration& other) const { return microseconds_ == other.microseconds_; }
    
private:
    int64_t microseconds_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Curves — منحنيات التسارع
// ═══════════════════════════════════════════════════════════════════════════════

/// واجهة أساسية للمنحنى
class Curve {
public:
    virtual ~Curve() = default;
    
    /// تحويل قيمة t (0.0 إلى 1.0) حسب المنحنى
    virtual double transform(double t) const = 0;
    
    /// عكس المنحنى
    double transformInternal(double t) const {
        return transform(t);
    }
};

/// منحنى خطي
class LinearCurve : public Curve {
public:
    double transform(double t) const override { return t; }
};

/// منحنى تباطؤ (Easing)
class EasingCurve : public Curve {
public:
    enum Type {
        EaseIn,
        EaseOut,
        EaseInOut,
        EaseInQuad,
        EaseOutQuad,
        EaseInOutQuad,
        EaseInCubic,
        EaseOutCubic,
        EaseInOutCubic,
        EaseInQuart,
        EaseOutQuart,
        EaseInOutQuart,
        EaseInQuint,
        EaseOutQuint,
        EaseInOutQuint,
        EaseInSine,
        EaseOutSine,
        EaseInOutSine,
        EaseInExpo,
        EaseOutExpo,
        EaseInOutExpo,
        EaseInCirc,
        EaseOutCirc,
        EaseInOutCirc,
        EaseInBack,
        EaseOutBack,
        EaseInOutBack,
        EaseInElastic,
        EaseOutElastic,
        EaseInOutElastic,
        EaseInBounce,
        EaseOutBounce,
        EaseInOutBounce
    };
    
    EasingCurve(Type type = EaseInOut) : type_(type) {}
    
    double transform(double t) const override {
        switch (type_) {
            case EaseIn: return easeIn(t);
            case EaseOut: return easeOut(t);
            case EaseInOut: return easeInOut(t);
            case EaseInQuad: return t * t;
            case EaseOutQuad: return t * (2 - t);
            case EaseInOutQuad: return t < 0.5 ? 2*t*t : -1 + (4-2*t)*t;
            case EaseInCubic: return t * t * t;
            case EaseOutCubic: { double f = t - 1; return f*f*f + 1; }
            case EaseInOutCubic: return t < 0.5 ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1;
            case EaseInQuart: return t * t * t * t;
            case EaseOutQuart: { double f = t - 1; return 1 - f*f*f*f; }
            case EaseInOutQuart: return t < 0.5 ? 8*t*t*t*t : 1-8*(t-1)*(t-1)*(t-1)*(t-1);
            case EaseInSine: return 1 - std::cos(t * M_PI / 2);
            case EaseOutSine: return std::sin(t * M_PI / 2);
            case EaseInOutSine: return 0.5 * (1 - std::cos(M_PI * t));
            case EaseInExpo: return t == 0 ? 0 : std::pow(2, 10 * (t - 1));
            case EaseOutExpo: return t == 1 ? 1 : 1 - std::pow(2, -10 * t);
            case EaseInCirc: return 1 - std::sqrt(1 - t * t);
            case EaseOutCirc: return std::sqrt(1 - (t-1) * (t-1));
            case EaseOutBounce: return bounceOut(t);
            case EaseInBounce: return 1 - bounceOut(1 - t);
            case EaseInOutBounce: 
                return t < 0.5 
                    ? (1 - bounceOut(1 - 2*t)) / 2 
                    : (1 + bounceOut(2*t - 1)) / 2;
            default: return t;
        }
    }
    
private:
    Type type_;
    
    static double easeIn(double t) { return t * t * t; }
    static double easeOut(double t) { double f = t - 1; return f*f*f + 1; }
    static double easeInOut(double t) {
        return t < 0.5 ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1;
    }
    
    static double bounceOut(double t) {
        if (t < 1/2.75) {
            return 7.5625 * t * t;
        } else if (t < 2/2.75) {
            t -= 1.5/2.75;
            return 7.5625 * t * t + 0.75;
        } else if (t < 2.5/2.75) {
            t -= 2.25/2.75;
            return 7.5625 * t * t + 0.9375;
        } else {
            t -= 2.625/2.75;
            return 7.5625 * t * t + 0.984375;
        }
    }
};

/// منحنيات جاهزة
namespace Curves {
    inline const LinearCurve linear;
    inline const EasingCurve easeIn{EasingCurve::EaseIn};
    inline const EasingCurve easeOut{EasingCurve::EaseOut};
    inline const EasingCurve easeInOut{EasingCurve::EaseInOut};
    inline const EasingCurve easeInQuad{EasingCurve::EaseInQuad};
    inline const EasingCurve easeOutQuad{EasingCurve::EaseOutQuad};
    inline const EasingCurve easeInOutQuad{EasingCurve::EaseInOutQuad};
    inline const EasingCurve easeInCubic{EasingCurve::EaseInCubic};
    inline const EasingCurve easeOutCubic{EasingCurve::EaseOutCubic};
    inline const EasingCurve easeInOutCubic{EasingCurve::EaseInOutCubic};
    inline const EasingCurve easeInSine{EasingCurve::EaseInSine};
    inline const EasingCurve easeOutSine{EasingCurve::EaseOutSine};
    inline const EasingCurve easeInOutSine{EasingCurve::EaseInOutSine};
    inline const EasingCurve easeInExpo{EasingCurve::EaseInExpo};
    inline const EasingCurve easeOutExpo{EasingCurve::EaseOutExpo};
    inline const EasingCurve bounceIn{EasingCurve::EaseInBounce};
    inline const EasingCurve bounceOut{EasingCurve::EaseOutBounce};
    inline const EasingCurve bounceInOut{EasingCurve::EaseInOutBounce};
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimationStatus — حالة الرسوم المتحركة
// ═══════════════════════════════════════════════════════════════════════════════

enum class AnimationStatus {
    Dismissed,  // متوقفة عند البداية
    Forward,    // تعمل للأمام
    Reverse,    // تعمل للخلف
    Completed   // انتهت عند النهاية
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Animation — واجهة الرسوم المتحركة
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class Animation {
public:
    virtual ~Animation() = default;
    
    /// القيمة الحالية
    virtual T value() const = 0;
    
    /// الحالة الحالية
    virtual AnimationStatus status() const = 0;
    
    /// هل انتهت؟
    bool isCompleted() const { return status() == AnimationStatus::Completed; }
    bool isDismissed() const { return status() == AnimationStatus::Dismissed; }
    bool isAnimating() const { 
        auto s = status();
        return s == AnimationStatus::Forward || s == AnimationStatus::Reverse;
    }
    
    /// إضافة مستمعين
    void addListener(std::function<void()> listener) {
        listeners_.push_back(std::move(listener));
    }
    
    void addStatusListener(std::function<void(AnimationStatus)> listener) {
        statusListeners_.push_back(std::move(listener));
    }
    
protected:
    void notifyListeners() {
        for (auto& l : listeners_) l();
    }
    
    void notifyStatusListeners(AnimationStatus status) {
        for (auto& l : statusListeners_) l(status);
    }
    
    std::vector<std::function<void()>> listeners_;
    std::vector<std::function<void(AnimationStatus)>> statusListeners_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimationController — متحكم الرسوم المتحركة
// ═══════════════════════════════════════════════════════════════════════════════

class TickerProvider;  // Forward declaration

class AnimationController : public Animation<double> {
public:
    AnimationController(
        Duration duration,
        double lowerBound = 0.0,
        double upperBound = 1.0,
        double initialValue = 0.0,
        TickerProvider* vsync = nullptr
    )
        : duration_(duration)
        , lowerBound_(lowerBound)
        , upperBound_(upperBound)
        , value_(initialValue)
        , vsync_(vsync)
    {}
    
    /// القيمة الحالية (0.0 إلى 1.0)
    double value() const override { return value_; }
    
    /// الحالة
    AnimationStatus status() const override { return status_; }
    
    /// تشغيل للأمام
    void forward() {
        direction_ = 1;
        status_ = AnimationStatus::Forward;
        notifyStatusListeners(status_);
        start();
    }
    
    /// تشغيل للخلف
    void reverse() {
        direction_ = -1;
        status_ = AnimationStatus::Reverse;
        notifyStatusListeners(status_);
        start();
    }
    
    /// تكرار
    void repeat(bool reverse = false) {
        repeat_ = true;
        reverseOnRepeat_ = reverse;
        forward();
    }
    
    /// إيقاف
    void stop() {
        running_ = false;
    }
    
    /// إعادة التعيين
    void reset() {
        value_ = lowerBound_;
        status_ = AnimationStatus::Dismissed;
        notifyStatusListeners(status_);
        notifyListeners();
    }
    
    /// تعيين قيمة مباشرة
    void animateTo(double target, Duration duration = Duration::milliseconds(300)) {
        targetValue_ = std::max(lowerBound_, std::min(upperBound_, target));
        duration_ = duration;
        if (targetValue_ > value_) forward();
        else if (targetValue_ < value_) reverse();
    }
    
    /// تحديث (يُنادى كل إطار)
    void tick(Duration elapsed) {
        if (!running_) return;
        
        double progress = elapsed.inMicroseconds() / 
                          static_cast<double>(duration_.inMicroseconds());
        
        if (direction_ > 0) {
            value_ = lowerBound_ + (upperBound_ - lowerBound_) * progress;
            if (value_ >= upperBound_) {
                value_ = upperBound_;
                if (repeat_) {
                    if (reverseOnRepeat_) reverse();
                    else reset(), forward();
                } else {
                    status_ = AnimationStatus::Completed;
                    notifyStatusListeners(status_);
                    running_ = false;
                }
            }
        } else {
            value_ = upperBound_ - (upperBound_ - lowerBound_) * progress;
            if (value_ <= lowerBound_) {
                value_ = lowerBound_;
                if (repeat_) {
                    if (reverseOnRepeat_) forward();
                    else reset(), reverse();
                } else {
                    status_ = AnimationStatus::Dismissed;
                    notifyStatusListeners(status_);
                    running_ = false;
                }
            }
        }
        
        notifyListeners();
    }
    
    /// المدة
    Duration duration() const { return duration_; }
    void setDuration(Duration d) { duration_ = d; }
    
private:
    void start() {
        running_ = true;
        startTime_ = std::chrono::steady_clock::now();
    }
    
    Duration duration_;
    double lowerBound_, upperBound_;
    double value_;
    double targetValue_ = 1.0;
    int direction_ = 1;
    AnimationStatus status_ = AnimationStatus::Dismissed;
    TickerProvider* vsync_;
    bool running_ = false;
    bool repeat_ = false;
    bool reverseOnRepeat_ = false;
    std::chrono::steady_clock::time_point startTime_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Tween — التحويل بين قيمتين
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class Tween {
public:
    Tween(T begin, T end) : begin_(begin), end_(end) {}
    
    T begin() const { return begin_; }
    T end() const { return end_; }
    
    /// تحويل قيمة t (0.0 إلى 1.0) إلى قيمة من النوع T
    virtual T lerp(double t) const = 0;
    
    /// تحريك باستخدام Animation
    T evaluate(const Animation<double>& animation) const {
        return lerp(animation.value());
    }
    
protected:
    T begin_, end_;
};

/// Tween للأرقام
class DoubleTween : public Tween<double> {
public:
    using Tween::Tween;
    
    double lerp(double t) const override {
        return begin_ + (end_ - begin_) * t;
    }
};

/// Tween للأعداد الصحيحة
class IntTween : public Tween<int> {
public:
    using Tween::Tween;
    
    int lerp(double t) const override {
        return static_cast<int>(begin_ + (end_ - begin_) * t);
    }
};

/// Tween للألوان
class ColorTween : public Tween<Color> {
public:
    using Tween::Tween;
    
    Color lerp(double t) const override {
        return Color(
            static_cast<uint8_t>(begin_.r + (end_.r - begin_.r) * t),
            static_cast<uint8_t>(begin_.g + (end_.g - begin_.g) * t),
            static_cast<uint8_t>(begin_.b + (end_.b - begin_.b) * t),
            static_cast<uint8_t>(begin_.a + (end_.a - begin_.a) * t)
        );
    }
};

/// Tween للحواف
class EdgeInsetsTween : public Tween<EdgeInsets> {
public:
    using Tween::Tween;
    
    EdgeInsets lerp(double t) const override {
        return EdgeInsets(
            begin_.top + (end_.top - begin_.top) * t,
            begin_.right + (end_.right - begin_.right) * t,
            begin_.bottom + (end_.bottom - begin_.bottom) * t,
            begin_.left + (end_.left - begin_.left) * t
        );
    }
};

/// Tween للحجم
class SizeTween : public Tween<Size> {
public:
    using Tween::Tween;
    
    Size lerp(double t) const override {
        return Size{
            begin_.width + (end_.width - begin_.width) * static_cast<float>(t),
            begin_.height + (end_.height - begin_.height) * static_cast<float>(t)
        };
    }
};

/// Tween للموقع
class OffsetTween : public Tween<Position> {
public:
    using Tween::Tween;
    
    Position lerp(double t) const override {
        return Position{
            begin_.x + (end_.x - begin_.x) * static_cast<float>(t),
            begin_.y + (end_.y - begin_.y) * static_cast<float>(t)
        };
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  CurvedAnimation — رسوم متحركة مع منحنى
// ═══════════════════════════════════════════════════════════════════════════════

class CurvedAnimation : public Animation<double> {
public:
    CurvedAnimation(
        std::shared_ptr<AnimationController> parent,
        const Curve& curve,
        const Curve* reverseCurve = nullptr
    )
        : parent_(std::move(parent))
        , curve_(&curve)
        , reverseCurve_(reverseCurve ? reverseCurve : &curve)
    {
        parent_->addListener([this]{ notifyListeners(); });
        parent_->addStatusListener([this](AnimationStatus s){ 
            notifyStatusListeners(s); 
        });
    }
    
    double value() const override {
        const Curve* activeCurve = (parent_->status() == AnimationStatus::Reverse)
            ? reverseCurve_ : curve_;
        return activeCurve->transform(parent_->value());
    }
    
    AnimationStatus status() const override {
        return parent_->status();
    }
    
private:
    std::shared_ptr<AnimationController> parent_;
    const Curve* curve_;
    const Curve* reverseCurve_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  TickerProvider — مزود الإطارات
// ═══════════════════════════════════════════════════════════════════════════════

class Ticker {
public:
    using TickCallback = std::function<void(Duration elapsed)>;
    
    Ticker(TickCallback callback) : callback_(std::move(callback)) {}
    
    void start() {
        active_ = true;
        startTime_ = std::chrono::steady_clock::now();
    }
    
    void stop() {
        active_ = false;
    }
    
    bool isActive() const { return active_; }
    
    void tick() {
        if (!active_) return;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_);
        callback_(Duration::microseconds(elapsed.count()));
    }
    
private:
    TickCallback callback_;
    bool active_ = false;
    std::chrono::steady_clock::time_point startTime_;
};

/// مزود الـ Ticker
class TickerProvider {
public:
    virtual ~TickerProvider() = default;
    virtual std::unique_ptr<Ticker> createTicker(Ticker::TickCallback callback) = 0;
};

/// Mixin للـ State مع TickerProvider
class SingleTickerProviderStateMixin {
protected:
    std::unique_ptr<Ticker> createTicker(Ticker::TickCallback callback) {
        return std::make_unique<Ticker>(std::move(callback));
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Animated Widgets — مكونات متحركة
// ═══════════════════════════════════════════════════════════════════════════════

/// مكون يُبنى عند تغير الـ Animation
class AnimatedBuilder : public StatelessWidget {
public:
    AnimatedBuilder(
        std::shared_ptr<Animation<double>> animation,
        std::function<WidgetPtr(BuildContext*, WidgetPtr)> builder,
        WidgetPtr child = nullptr,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , animation_(std::move(animation))
        , builder_(std::move(builder))
        , child_(std::move(child))
    {}
    
    WidgetPtr build(BuildContext* ctx) override {
        return builder_(ctx, child_);
    }
    
    std::string typeName() const override { return "AnimatedBuilder"; }
    
private:
    std::shared_ptr<Animation<double>> animation_;
    std::function<WidgetPtr(BuildContext*, WidgetPtr)> builder_;
    WidgetPtr child_;
};

/// حاوية متحركة (Implicit Animation)
class AnimatedContainer : public StatefulWidget {
public:
    AnimatedContainer(
        WidgetPtr child = nullptr,
        std::optional<float> width = std::nullopt,
        std::optional<float> height = std::nullopt,
        EdgeInsets padding = {},
        EdgeInsets margin = {},
        BoxDecoration decoration = {},
        Duration duration = Duration::milliseconds(300),
        const Curve& curve = Curves::easeInOut,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
        , width_(width)
        , height_(height)
        , padding_(padding)
        , margin_(margin)
        , decoration_(std::move(decoration))
        , duration_(duration)
        , curve_(&curve)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "AnimatedContainer"; }
    
    WidgetPtr child() const { return child_; }
    std::optional<float> width() const { return width_; }
    std::optional<float> height() const { return height_; }
    EdgeInsets padding() const { return padding_; }
    EdgeInsets margin() const { return margin_; }
    const BoxDecoration& decoration() const { return decoration_; }
    Duration duration() const { return duration_; }
    const Curve* curve() const { return curve_; }
    
private:
    WidgetPtr child_;
    std::optional<float> width_, height_;
    EdgeInsets padding_, margin_;
    BoxDecoration decoration_;
    Duration duration_;
    const Curve* curve_;
};

/// شفافية متحركة
class AnimatedOpacity : public StatefulWidget {
public:
    AnimatedOpacity(
        WidgetPtr child,
        float opacity,
        Duration duration = Duration::milliseconds(300),
        const Curve& curve = Curves::linear,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
        , opacity_(opacity)
        , duration_(duration)
        , curve_(&curve)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "AnimatedOpacity"; }
    
    WidgetPtr child() const { return child_; }
    float opacity() const { return opacity_; }
    Duration duration() const { return duration_; }
    
private:
    WidgetPtr child_;
    float opacity_;
    Duration duration_;
    const Curve* curve_;
};

/// انتقال التلاشي
class FadeTransition : public StatelessWidget {
public:
    FadeTransition(
        std::shared_ptr<Animation<double>> opacity,
        WidgetPtr child,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , opacity_(std::move(opacity))
        , child_(std::move(child))
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "FadeTransition"; }
    
private:
    std::shared_ptr<Animation<double>> opacity_;
    WidgetPtr child_;
};

/// انتقال التحريك
class SlideTransition : public StatelessWidget {
public:
    SlideTransition(
        std::shared_ptr<Animation<double>> animation,
        Position begin = {-1, 0},
        Position end = {0, 0},
        WidgetPtr child = nullptr,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , animation_(std::move(animation))
        , begin_(begin)
        , end_(end)
        , child_(std::move(child))
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "SlideTransition"; }
    
private:
    std::shared_ptr<Animation<double>> animation_;
    Position begin_, end_;
    WidgetPtr child_;
};

/// انتقال التحجيم
class ScaleTransition : public StatelessWidget {
public:
    ScaleTransition(
        std::shared_ptr<Animation<double>> scale,
        WidgetPtr child,
        AlignmentGeometry alignment = AlignmentGeometry::center(),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , scale_(std::move(scale))
        , child_(std::move(child))
        , alignment_(alignment)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "ScaleTransition"; }
    
private:
    std::shared_ptr<Animation<double>> scale_;
    WidgetPtr child_;
    AlignmentGeometry alignment_;
};

/// انتقال الدوران
class RotationTransition : public StatelessWidget {
public:
    RotationTransition(
        std::shared_ptr<Animation<double>> turns,
        WidgetPtr child,
        AlignmentGeometry alignment = AlignmentGeometry::center(),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , turns_(std::move(turns))
        , child_(std::move(child))
        , alignment_(alignment)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "RotationTransition"; }
    
private:
    std::shared_ptr<Animation<double>> turns_;
    WidgetPtr child_;
    AlignmentGeometry alignment_;
};

/// حجم متحرك
class AnimatedSize : public StatefulWidget {
public:
    AnimatedSize(
        WidgetPtr child,
        Duration duration = Duration::milliseconds(300),
        const Curve& curve = Curves::easeInOut,
        AlignmentGeometry alignment = AlignmentGeometry::center(),
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
        , duration_(duration)
        , curve_(&curve)
        , alignment_(alignment)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "AnimatedSize"; }
    
private:
    WidgetPtr child_;
    Duration duration_;
    const Curve* curve_;
    AlignmentGeometry alignment_;
};

/// تبديل متحرك
class AnimatedSwitcher : public StatefulWidget {
public:
    AnimatedSwitcher(
        WidgetPtr child,
        Duration duration = Duration::milliseconds(300),
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
        , duration_(duration)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "AnimatedSwitcher"; }
    
private:
    WidgetPtr child_;
    Duration duration_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Arabic Aliases — أسماء عربية
// ═══════════════════════════════════════════════════════════════════════════════

using مدة = Duration;
using منحنى = Curve;
using حالة_حركة = AnimationStatus;
using حركة = Animation<double>;
using متحكم_حركة = AnimationController;
using حركة_منحنية = CurvedAnimation;

// منحنيات عربية
namespace منحنيات {
    inline const auto& خطي = Curves::linear;
    inline const auto& تسارع = Curves::easeIn;
    inline const auto& تباطؤ = Curves::easeOut;
    inline const auto& تسارع_تباطؤ = Curves::easeInOut;
    inline const auto& ارتداد_دخول = Curves::bounceIn;
    inline const auto& ارتداد_خروج = Curves::bounceOut;
}

// دوال مساعدة
inline WidgetPtr حاوية_متحركة(
    WidgetPtr طفل,
    Duration مدة = Duration::milliseconds(300),
    std::optional<float> عرض = std::nullopt,
    std::optional<float> ارتفاع = std::nullopt,
    Color خلفية = Color::transparent()
) {
    BoxDecoration تزيين;
    تزيين.setColor(خلفية);
    return std::make_shared<AnimatedContainer>(
        std::move(طفل), عرض, ارتفاع, EdgeInsets{}, EdgeInsets{}, تزيين, مدة
    );
}

inline WidgetPtr شفافية_متحركة(
    WidgetPtr طفل,
    float شفافية,
    Duration مدة = Duration::milliseconds(300)
) {
    return std::make_shared<AnimatedOpacity>(std::move(طفل), شفافية, مدة);
}

} // namespace flutter
} // namespace sad
