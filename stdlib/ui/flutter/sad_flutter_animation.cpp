// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_animation.cpp — تطبيق نظام الرسوم المتحركة
//  Animation System Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_flutter_animation.h"
#include "sad_flutter_layout.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedContainer State
// ═══════════════════════════════════════════════════════════════════════════════

class AnimatedContainerState : public State<AnimatedContainer>,
                               public SingleTickerProviderStateMixin {
public:
    void initState() override {
        controller_ = std::make_shared<AnimationController>(
            widget()->duration()
        );
        
        // تعيين القيم الأولية
        currentWidth_ = widget()->width();
        currentHeight_ = widget()->height();
        currentPadding_ = widget()->padding();
        currentMargin_ = widget()->margin();
        currentDecoration_ = widget()->decoration();
        
        controller_->addListener([this] {
            // تحديث القيم المتحركة
            double t = controller_->value();
            const Curve* curve = widget()->curve();
            double curved = curve->transform(t);
            
            // حساب القيم الوسيطة
            if (targetWidth_.has_value() && startWidth_.has_value()) {
                currentWidth_ = *startWidth_ + (*targetWidth_ - *startWidth_) * static_cast<float>(curved);
            }
            if (targetHeight_.has_value() && startHeight_.has_value()) {
                currentHeight_ = *startHeight_ + (*targetHeight_ - *startHeight_) * static_cast<float>(curved);
            }
            
            // Padding lerp
            currentPadding_ = EdgeInsets(
                startPadding_.top + (targetPadding_.top - startPadding_.top) * curved,
                startPadding_.right + (targetPadding_.right - startPadding_.right) * curved,
                startPadding_.bottom + (targetPadding_.bottom - startPadding_.bottom) * curved,
                startPadding_.left + (targetPadding_.left - startPadding_.left) * curved
            );
            
            // Margin lerp
            currentMargin_ = EdgeInsets(
                startMargin_.top + (targetMargin_.top - startMargin_.top) * curved,
                startMargin_.right + (targetMargin_.right - startMargin_.right) * curved,
                startMargin_.bottom + (targetMargin_.bottom - startMargin_.bottom) * curved,
                startMargin_.left + (targetMargin_.left - startMargin_.left) * curved
            );
            
            // Color lerp in decoration
            Color startColor = startDecoration_.color();
            Color targetColor = targetDecoration_.color();
            currentDecoration_.setColor(Color(
                static_cast<uint8_t>(startColor.r + (targetColor.r - startColor.r) * curved),
                static_cast<uint8_t>(startColor.g + (targetColor.g - startColor.g) * curved),
                static_cast<uint8_t>(startColor.b + (targetColor.b - startColor.b) * curved),
                static_cast<uint8_t>(startColor.a + (targetColor.a - startColor.a) * curved)
            ));
            
            setState([]{});
        });
    }
    
    void didUpdateWidget(AnimatedContainer* oldWidget) override {
        // تحقق من التغييرات وبدء الحركة
        bool changed = false;
        
        if (widget()->width() != currentWidth_) {
            startWidth_ = currentWidth_;
            targetWidth_ = widget()->width();
            changed = true;
        }
        
        if (widget()->height() != currentHeight_) {
            startHeight_ = currentHeight_;
            targetHeight_ = widget()->height();
            changed = true;
        }
        
        if (widget()->padding().top != currentPadding_.top ||
            widget()->padding().right != currentPadding_.right ||
            widget()->padding().bottom != currentPadding_.bottom ||
            widget()->padding().left != currentPadding_.left) {
            startPadding_ = currentPadding_;
            targetPadding_ = widget()->padding();
            changed = true;
        }
        
        if (widget()->margin().top != currentMargin_.top ||
            widget()->margin().right != currentMargin_.right ||
            widget()->margin().bottom != currentMargin_.bottom ||
            widget()->margin().left != currentMargin_.left) {
            startMargin_ = currentMargin_;
            targetMargin_ = widget()->margin();
            changed = true;
        }
        
        // Check decoration color
        if (widget()->decoration().color().r != currentDecoration_.color().r ||
            widget()->decoration().color().g != currentDecoration_.color().g ||
            widget()->decoration().color().b != currentDecoration_.color().b) {
            startDecoration_ = currentDecoration_;
            targetDecoration_ = widget()->decoration();
            changed = true;
        }
        
        if (changed) {
            controller_->setDuration(widget()->duration());
            controller_->reset();
            controller_->forward();
        }
    }
    
    void dispose() override {
        controller_->stop();
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        return std::make_shared<Container>(
            widget()->child(),
            currentWidth_,
            currentHeight_,
            currentPadding_,
            currentMargin_,
            currentDecoration_
        );
    }
    
private:
    std::shared_ptr<AnimationController> controller_;
    
    // القيم الحالية
    std::optional<float> currentWidth_, currentHeight_;
    EdgeInsets currentPadding_, currentMargin_;
    BoxDecoration currentDecoration_;
    
    // القيم البداية والهدف
    std::optional<float> startWidth_, targetWidth_;
    std::optional<float> startHeight_, targetHeight_;
    EdgeInsets startPadding_, targetPadding_;
    EdgeInsets startMargin_, targetMargin_;
    BoxDecoration startDecoration_, targetDecoration_;
};

std::unique_ptr<State_Base> AnimatedContainer::createState() {
    return std::make_unique<AnimatedContainerState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedOpacity State
// ═══════════════════════════════════════════════════════════════════════════════

class AnimatedOpacityState : public State<AnimatedOpacity>,
                             public SingleTickerProviderStateMixin {
public:
    void initState() override {
        controller_ = std::make_shared<AnimationController>(widget()->duration());
        currentOpacity_ = widget()->opacity();
        
        controller_->addListener([this] {
            double t = controller_->value();
            currentOpacity_ = static_cast<float>(
                startOpacity_ + (targetOpacity_ - startOpacity_) * t
            );
            setState([]{});
        });
    }
    
    void didUpdateWidget(AnimatedOpacity* oldWidget) override {
        if (widget()->opacity() != currentOpacity_) {
            startOpacity_ = currentOpacity_;
            targetOpacity_ = widget()->opacity();
            controller_->setDuration(widget()->duration());
            controller_->reset();
            controller_->forward();
        }
    }
    
    void dispose() override {
        controller_->stop();
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        return std::make_shared<Opacity>(widget()->child(), currentOpacity_);
    }
    
private:
    std::shared_ptr<AnimationController> controller_;
    float currentOpacity_ = 1.0f;
    float startOpacity_ = 1.0f;
    float targetOpacity_ = 1.0f;
};

std::unique_ptr<State_Base> AnimatedOpacity::createState() {
    return std::make_unique<AnimatedOpacityState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  FadeTransition
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr FadeTransition::build(BuildContext* ctx) {
    return std::make_shared<Opacity>(
        child_,
        static_cast<float>(opacity_->value())
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SlideTransition
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr SlideTransition::build(BuildContext* ctx) {
    double t = animation_->value();
    Position offset{
        begin_.x + (end_.x - begin_.x) * static_cast<float>(t),
        begin_.y + (end_.y - begin_.y) * static_cast<float>(t)
    };
    
    // نستخدم Transform (سنضيفه لاحقاً) أو Padding كبديل مؤقت
    return std::make_shared<Padding>(
        child_,
        EdgeInsets(offset.y * 50, -offset.x * 50, -offset.y * 50, offset.x * 50)
    );
}

// ═══════════════════════════════════════════════════════════════════════════════
//  ScaleTransition
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr ScaleTransition::build(BuildContext* ctx) {
    double s = scale_->value();
    // TODO: استخدام Transform.scale الحقيقي
    // حالياً نستخدم Opacity كبديل مرئي
    return std::make_shared<Opacity>(child_, static_cast<float>(s));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  RotationTransition
// ═══════════════════════════════════════════════════════════════════════════════

WidgetPtr RotationTransition::build(BuildContext* ctx) {
    // double turns = turns_->value();
    // TODO: استخدام Transform.rotate الحقيقي
    return child_;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedSize State
// ═══════════════════════════════════════════════════════════════════════════════

class AnimatedSizeState : public State<AnimatedSize>,
                          public SingleTickerProviderStateMixin {
public:
    void initState() override {
        // AnimatedSize يتتبع حجم الطفل تلقائياً
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        // TODO: قياس حجم الطفل وتحريكه
        return widget_->child();
    }
    
private:
    WidgetPtr widget() { return std::make_shared<AnimatedSize>(*static_cast<AnimatedSize*>(widget_)); }
};

std::unique_ptr<State_Base> AnimatedSize::createState() {
    return std::make_unique<AnimatedSizeState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedSwitcher State
// ═══════════════════════════════════════════════════════════════════════════════

class AnimatedSwitcherState : public State<AnimatedSwitcher>,
                              public SingleTickerProviderStateMixin {
public:
    void initState() override {
        controller_ = std::make_shared<AnimationController>(
            widget()->duration()
        );
        currentChild_ = widget()->child();
        controller_->forward();
        
        controller_->addListener([this] {
            setState([]{});
        });
    }
    
    void didUpdateWidget(AnimatedSwitcher* oldWidget) override {
        // تحقق إذا تغير الطفل
        if (widget()->child().get() != currentChild_.get()) {
            outgoingChild_ = currentChild_;
            currentChild_ = widget()->child();
            controller_->reset();
            controller_->forward();
        }
    }
    
    void dispose() override {
        controller_->stop();
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        double t = controller_->value();
        
        std::vector<WidgetPtr> children;
        
        // الطفل الخارج (يتلاشى)
        if (outgoingChild_ && t < 1.0) {
            children.push_back(
                std::make_shared<Opacity>(outgoingChild_, static_cast<float>(1.0 - t))
            );
        }
        
        // الطفل الداخل (يظهر)
        children.push_back(
            std::make_shared<Opacity>(currentChild_, static_cast<float>(t))
        );
        
        return std::make_shared<Stack>(std::move(children));
    }
    
private:
    std::shared_ptr<AnimationController> controller_;
    WidgetPtr currentChild_;
    WidgetPtr outgoingChild_;
    
    AnimatedSwitcher* widget() const {
        return static_cast<AnimatedSwitcher*>(widget_);
    }
};

std::unique_ptr<State_Base> AnimatedSwitcher::createState() {
    return std::make_unique<AnimatedSwitcherState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Hero Animation (Shared Element Transition)
// ═══════════════════════════════════════════════════════════════════════════════

/// خريطة الـ Hero العالمية
static std::unordered_map<std::string, WidgetPtr> heroRegistry_;

class Hero : public StatelessWidget {
public:
    Hero(std::string tag, WidgetPtr child, Key key = Key())
        : StatelessWidget(std::move(key))
        , tag_(std::move(tag))
        , child_(std::move(child))
    {
        // تسجيل في الخريطة
        heroRegistry_[tag_] = child_;
    }
    
    std::string typeName() const override { return "Hero"; }
    
    WidgetPtr build(BuildContext* ctx) override {
        return child_;
    }
    
    const std::string& tag() const { return tag_; }
    
    /// البحث عن Hero بعلامة معينة
    static WidgetPtr findByTag(const std::string& tag) {
        auto it = heroRegistry_.find(tag);
        return it != heroRegistry_.end() ? it->second : nullptr;
    }
    
private:
    std::string tag_;
    WidgetPtr child_;
};

// دالة عربية لإنشاء Hero
inline WidgetPtr بطل(const std::string& علامة, WidgetPtr طفل) {
    return std::make_shared<Hero>(علامة, std::move(طفل));
}

// ═══════════════════════════════════════════════════════════════════════════════
//  TweenAnimationBuilder — بناء حركة بـ Tween
// ═══════════════════════════════════════════════════════════════════════════════

template<typename T>
class TweenAnimationBuilder : public StatefulWidget {
public:
    TweenAnimationBuilder(
        std::shared_ptr<Tween<T>> tween,
        Duration duration,
        std::function<WidgetPtr(BuildContext*, T, WidgetPtr)> builder,
        WidgetPtr child = nullptr,
        const Curve& curve = Curves::linear,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , tween_(std::move(tween))
        , duration_(duration)
        , builder_(std::move(builder))
        , child_(std::move(child))
        , curve_(&curve)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "TweenAnimationBuilder"; }
    
    std::shared_ptr<Tween<T>> tween() const { return tween_; }
    Duration duration() const { return duration_; }
    const std::function<WidgetPtr(BuildContext*, T, WidgetPtr)>& builder() const { return builder_; }
    WidgetPtr child() const { return child_; }
    const Curve* curve() const { return curve_; }
    
private:
    std::shared_ptr<Tween<T>> tween_;
    Duration duration_;
    std::function<WidgetPtr(BuildContext*, T, WidgetPtr)> builder_;
    WidgetPtr child_;
    const Curve* curve_;
};

template<typename T>
class TweenAnimationBuilderState : public State<TweenAnimationBuilder<T>>,
                                   public SingleTickerProviderStateMixin {
public:
    void initState() override {
        controller_ = std::make_shared<AnimationController>(
            this->widget()->duration()
        );
        controller_->forward();
        
        controller_->addListener([this] {
            this->setState([]{});
        });
    }
    
    void dispose() override {
        controller_->stop();
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        auto* w = this->widget();
        const Curve* curve = w->curve();
        double t = curve->transform(controller_->value());
        T value = w->tween()->lerp(t);
        return w->builder()(ctx, value, w->child());
    }
    
private:
    std::shared_ptr<AnimationController> controller_;
};

template<typename T>
std::unique_ptr<State_Base> TweenAnimationBuilder<T>::createState() {
    return std::make_unique<TweenAnimationBuilderState<T>>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedDefaultTextStyle
// ═══════════════════════════════════════════════════════════════════════════════

class AnimatedDefaultTextStyle : public StatefulWidget {
public:
    AnimatedDefaultTextStyle(
        WidgetPtr child,
        TextStyle style,
        Duration duration = Duration::milliseconds(300),
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
        , style_(std::move(style))
        , duration_(duration)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "AnimatedDefaultTextStyle"; }
    
private:
    WidgetPtr child_;
    TextStyle style_;
    Duration duration_;
    
    friend class AnimatedDefaultTextStyleState;
};

class AnimatedDefaultTextStyleState : public State<AnimatedDefaultTextStyle>,
                                      public SingleTickerProviderStateMixin {
public:
    void initState() override {
        currentStyle_ = widget()->style_;
    }
    
    void didUpdateWidget(AnimatedDefaultTextStyle* oldWidget) override {
        // TODO: تحريك التغييرات في النمط
        currentStyle_ = widget()->style_;
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        // TODO: تطبيق النمط على الأطفال
        return widget()->child_;
    }
    
private:
    TextStyle currentStyle_;
};

std::unique_ptr<State_Base> AnimatedDefaultTextStyle::createState() {
    return std::make_unique<AnimatedDefaultTextStyleState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  AnimatedPositioned
// ═══════════════════════════════════════════════════════════════════════════════

class AnimatedPositioned : public StatefulWidget {
public:
    AnimatedPositioned(
        WidgetPtr child,
        Duration duration = Duration::milliseconds(300),
        std::optional<float> left = std::nullopt,
        std::optional<float> top = std::nullopt,
        std::optional<float> right = std::nullopt,
        std::optional<float> bottom = std::nullopt,
        std::optional<float> width = std::nullopt,
        std::optional<float> height = std::nullopt,
        Key key = Key()
    )
        : StatefulWidget(std::move(key))
        , child_(std::move(child))
        , duration_(duration)
        , left_(left), top_(top), right_(right), bottom_(bottom)
        , width_(width), height_(height)
    {}
    
    std::unique_ptr<State_Base> createState() override;
    std::string typeName() const override { return "AnimatedPositioned"; }
    
    WidgetPtr child() const { return child_; }
    Duration duration() const { return duration_; }
    std::optional<float> left() const { return left_; }
    std::optional<float> top() const { return top_; }
    std::optional<float> right() const { return right_; }
    std::optional<float> bottom() const { return bottom_; }
    std::optional<float> width() const { return width_; }
    std::optional<float> height() const { return height_; }
    
private:
    WidgetPtr child_;
    Duration duration_;
    std::optional<float> left_, top_, right_, bottom_;
    std::optional<float> width_, height_;
};

class AnimatedPositionedState : public State<AnimatedPositioned>,
                                public SingleTickerProviderStateMixin {
public:
    void initState() override {
        controller_ = std::make_shared<AnimationController>(widget()->duration());
        
        currentLeft_ = widget()->left();
        currentTop_ = widget()->top();
        currentRight_ = widget()->right();
        currentBottom_ = widget()->bottom();
        currentWidth_ = widget()->width();
        currentHeight_ = widget()->height();
        
        controller_->addListener([this] {
            double t = controller_->value();
            if (targetLeft_ && startLeft_) {
                currentLeft_ = *startLeft_ + (*targetLeft_ - *startLeft_) * static_cast<float>(t);
            }
            if (targetTop_ && startTop_) {
                currentTop_ = *startTop_ + (*targetTop_ - *startTop_) * static_cast<float>(t);
            }
            if (targetWidth_ && startWidth_) {
                currentWidth_ = *startWidth_ + (*targetWidth_ - *startWidth_) * static_cast<float>(t);
            }
            if (targetHeight_ && startHeight_) {
                currentHeight_ = *startHeight_ + (*targetHeight_ - *startHeight_) * static_cast<float>(t);
            }
            setState([]{});
        });
    }
    
    void didUpdateWidget(AnimatedPositioned* oldWidget) override {
        bool changed = false;
        
        if (widget()->left() != currentLeft_) {
            startLeft_ = currentLeft_;
            targetLeft_ = widget()->left();
            changed = true;
        }
        if (widget()->top() != currentTop_) {
            startTop_ = currentTop_;
            targetTop_ = widget()->top();
            changed = true;
        }
        if (widget()->width() != currentWidth_) {
            startWidth_ = currentWidth_;
            targetWidth_ = widget()->width();
            changed = true;
        }
        if (widget()->height() != currentHeight_) {
            startHeight_ = currentHeight_;
            targetHeight_ = widget()->height();
            changed = true;
        }
        
        if (changed) {
            controller_->setDuration(widget()->duration());
            controller_->reset();
            controller_->forward();
        }
    }
    
    void dispose() override {
        controller_->stop();
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        return std::make_shared<Positioned>(
            widget()->child(),
            currentLeft_,
            currentTop_,
            currentRight_,
            currentBottom_,
            currentWidth_,
            currentHeight_
        );
    }
    
private:
    std::shared_ptr<AnimationController> controller_;
    std::optional<float> currentLeft_, currentTop_, currentRight_, currentBottom_;
    std::optional<float> currentWidth_, currentHeight_;
    std::optional<float> startLeft_, startTop_, startWidth_, startHeight_;
    std::optional<float> targetLeft_, targetTop_, targetWidth_, targetHeight_;
};

std::unique_ptr<State_Base> AnimatedPositioned::createState() {
    return std::make_unique<AnimatedPositionedState>();
}

} // namespace flutter
} // namespace sad
