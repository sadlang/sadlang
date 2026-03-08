// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_accessibility.h — دعم الوصول
//  Accessibility Support
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_FLUTTER_ACCESSIBILITY_H
#define SAD_FLUTTER_ACCESSIBILITY_H

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Semantics Actions — إجراءات الدلالات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إجراءات الوصول
 * @brief (EN) Semantics actions
 */
enum class SemanticsAction {
    Tap,
    LongPress,
    ScrollLeft,
    ScrollRight,
    ScrollUp,
    ScrollDown,
    Increase,
    Decrease,
    ShowOnScreen,
    MoveCursorForwardByCharacter,
    MoveCursorBackwardByCharacter,
    SetSelection,
    Copy,
    Cut,
    Paste,
    DidGainAccessibilityFocus,
    DidLoseAccessibilityFocus,
    CustomAction,
    Dismiss,
    MoveCursorForwardByWord,
    MoveCursorBackwardByWord,
    SetText
};

using اجراء_دلالي = SemanticsAction;

/**
 * @brief (AR) أعلام الدلالات
 * @brief (EN) Semantics flags
 */
enum class SemanticsFlag {
    HasCheckedState,
    IsChecked,
    IsSelected,
    IsButton,
    IsLink,
    IsTextField,
    IsReadOnly,
    IsFocusable,
    IsFocused,
    HasEnabledState,
    IsEnabled,
    IsInMutuallyExclusiveGroup,
    IsHeader,
    IsObscured,
    ScopesRoute,
    NamesRoute,
    IsHidden,
    IsImage,
    IsLiveRegion,
    HasToggledState,
    IsToggled,
    HasImplicitScrolling,
    IsMultiline,
    IsSlider,
    IsKeyboardKey
};

using علم_دلالي = SemanticsFlag;

// ═══════════════════════════════════════════════════════════════════════════════
//  SemanticsProperties — خصائص الدلالات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) خصائص الدلالات
 * @brief (EN) Semantics properties
 */
struct SemanticsProperties {
    bool enabled = true;
    bool checked = false;
    bool toggled = false;
    bool selected = false;
    bool button = false;
    bool link = false;
    bool header = false;
    bool textField = false;
    bool readOnly = false;
    bool focusable = false;
    bool focused = false;
    bool inMutuallyExclusiveGroup = false;
    bool hidden = false;
    bool obscured = false;
    bool multiline = false;
    bool scopesRoute = false;
    bool namesRoute = false;
    bool image = false;
    bool liveRegion = false;
    int maxValueLength = -1;
    int currentValueLength = -1;
    
    std::string label;
    std::string value;
    std::string increasedValue;
    std::string decreasedValue;
    std::string hint;
    std::string tooltip;
    
    TextDirection textDirection = TextDirection::RTL;
    
    std::function<void()> onTap;
    std::function<void()> onLongPress;
    std::function<void()> onScrollLeft;
    std::function<void()> onScrollRight;
    std::function<void()> onScrollUp;
    std::function<void()> onScrollDown;
    std::function<void()> onIncrease;
    std::function<void()> onDecrease;
    std::function<void()> onCopy;
    std::function<void()> onCut;
    std::function<void()> onPaste;
    std::function<void()> onDismiss;
    std::function<void()> onDidGainAccessibilityFocus;
    std::function<void()> onDidLoseAccessibilityFocus;
    std::function<void(int, int)> onMoveCursorForwardByCharacter;
    std::function<void(int, int)> onMoveCursorBackwardByCharacter;
    std::function<void(TextSelection)> onSetSelection;
    std::function<void(std::string)> onSetText;
    
    std::vector<std::pair<int, std::string>> customActions;
};

using خصائص_دلالات = SemanticsProperties;

// ═══════════════════════════════════════════════════════════════════════════════
//  Semantics Widget — مكون الدلالات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مكون الدلالات للوصول
 * @brief (EN) Semantics widget for accessibility
 */
class Semantics : public StatelessWidget {
public:
    Semantics(WidgetPtr child,
              SemanticsProperties properties = SemanticsProperties{},
              bool container = false,
              bool explicitChildNodes = false,
              bool excludeSemantics = false,
              bool blockUserActions = false)
        : child_(child), properties_(properties), container_(container),
          explicitChildNodes_(explicitChildNodes), excludeSemantics_(excludeSemantics),
          blockUserActions_(blockUserActions) {}
    
    // مصنعات مختصرة
    static std::shared_ptr<Semantics> label(WidgetPtr child, const std::string& text) {
        SemanticsProperties props;
        props.label = text;
        return std::make_shared<Semantics>(child, props);
    }
    
    static std::shared_ptr<Semantics> button(WidgetPtr child, 
                                              const std::string& text,
                                              std::function<void()> onTap) {
        SemanticsProperties props;
        props.label = text;
        props.button = true;
        props.onTap = onTap;
        return std::make_shared<Semantics>(child, props);
    }
    
    static std::shared_ptr<Semantics> image(WidgetPtr child, const std::string& text) {
        SemanticsProperties props;
        props.label = text;
        props.image = true;
        return std::make_shared<Semantics>(child, props);
    }
    
    static std::shared_ptr<Semantics> header(WidgetPtr child, const std::string& text) {
        SemanticsProperties props;
        props.label = text;
        props.header = true;
        return std::make_shared<Semantics>(child, props);
    }
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
    const SemanticsProperties& properties() const { return properties_; }
    
private:
    WidgetPtr child_;
    SemanticsProperties properties_;
    bool container_;
    bool explicitChildNodes_;
    bool excludeSemantics_;
    bool blockUserActions_;
};

using دلالات = Semantics;

// ═══════════════════════════════════════════════════════════════════════════════
//  MergeSemantics — دمج الدلالات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) دمج دلالات الأبناء
 * @brief (EN) Merge child semantics
 */
class MergeSemantics : public StatelessWidget {
public:
    explicit MergeSemantics(WidgetPtr child) : child_(child) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
private:
    WidgetPtr child_;
};

using دمج_دلالات = MergeSemantics;

// ═══════════════════════════════════════════════════════════════════════════════
//  ExcludeSemantics — استبعاد الدلالات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) استبعاد من شجرة الوصول
 * @brief (EN) Exclude from accessibility tree
 */
class ExcludeSemantics : public StatelessWidget {
public:
    ExcludeSemantics(WidgetPtr child, bool excluding = true)
        : child_(child), excluding_(excluding) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
private:
    WidgetPtr child_;
    bool excluding_;
};

using استبعاد_دلالات = ExcludeSemantics;

// ═══════════════════════════════════════════════════════════════════════════════
//  SemanticsDebugger — مصحح الدلالات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) عرض معلومات الوصول للتصحيح
 * @brief (EN) Debug semantics overlay
 */
class SemanticsDebugger : public StatelessWidget {
public:
    SemanticsDebugger(WidgetPtr child,
                      Color labelColor = Color(255, 255, 0),
                      bool enabled = true)
        : child_(child), labelColor_(labelColor), enabled_(enabled) {}
    
    WidgetPtr build(BuildContext* ctx) override {
        if (!enabled_) return child_;
        
        // TODO: تطبيق عرض الدلالات
        return child_;
    }
    
private:
    WidgetPtr child_;
    Color labelColor_;
    bool enabled_;
};

using مصحح_دلالات = SemanticsDebugger;

// ═══════════════════════════════════════════════════════════════════════════════
//  Focus Widgets — مكونات التركيز
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) عقدة التركيز
 * @brief (EN) Focus node
 */
class FocusNode {
public:
    FocusNode(bool canRequestFocus = true,
              bool skipTraversal = false,
              std::string debugLabel = "")
        : canRequestFocus_(canRequestFocus), skipTraversal_(skipTraversal),
          debugLabel_(debugLabel) {}
    
    bool hasFocus() const { return hasFocus_; }
    bool hasPrimaryFocus() const { return hasPrimaryFocus_; }
    bool canRequestFocus() const { return canRequestFocus_; }
    
    void requestFocus() {
        if (canRequestFocus_) {
            hasFocus_ = true;
            hasPrimaryFocus_ = true;
            // TODO: notify
        }
    }
    
    void unfocus() {
        hasFocus_ = false;
        hasPrimaryFocus_ = false;
    }
    
    void addListener(std::function<void()> listener) {
        listeners_.push_back(listener);
    }
    
    void dispose() {
        listeners_.clear();
    }
    
private:
    bool canRequestFocus_;
    bool skipTraversal_;
    std::string debugLabel_;
    bool hasFocus_ = false;
    bool hasPrimaryFocus_ = false;
    std::vector<std::function<void()>> listeners_;
};

using عقدة_تركيز = FocusNode;

/**
 * @brief (AR) مكون التركيز
 * @brief (EN) Focus widget
 */
class Focus : public StatefulWidget {
public:
    Focus(WidgetPtr child,
          std::shared_ptr<FocusNode> focusNode = nullptr,
          bool autofocus = false,
          bool canRequestFocus = true,
          bool skipTraversal = false,
          bool descendantsAreFocusable = true,
          bool descendantsAreTraversable = true,
          std::function<void(bool)> onFocusChange = nullptr,
          std::function<bool(FocusNode&, KeyEvent)> onKey = nullptr)
        : child_(child), focusNode_(focusNode), autofocus_(autofocus),
          canRequestFocus_(canRequestFocus), skipTraversal_(skipTraversal),
          descendantsAreFocusable_(descendantsAreFocusable),
          descendantsAreTraversable_(descendantsAreTraversable),
          onFocusChange_(onFocusChange), onKey_(onKey) {}
    
    std::unique_ptr<State_Base> createState() override;
    
    WidgetPtr child_;
    std::shared_ptr<FocusNode> focusNode_;
    bool autofocus_;
    bool canRequestFocus_;
    bool skipTraversal_;
    bool descendantsAreFocusable_;
    bool descendantsAreTraversable_;
    std::function<void(bool)> onFocusChange_;
    std::function<bool(FocusNode&, KeyEvent)> onKey_;
};

using تركيز = Focus;

/**
 * @brief (AR) منطقة التركيز
 * @brief (EN) Focus scope
 */
class FocusScope : public StatelessWidget {
public:
    FocusScope(WidgetPtr child,
               std::shared_ptr<FocusNode> node = nullptr,
               bool autofocus = false,
               bool canRequestFocus = true,
               bool skipTraversal = false)
        : child_(child), node_(node), autofocus_(autofocus),
          canRequestFocus_(canRequestFocus), skipTraversal_(skipTraversal) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
    static void of(BuildContext* ctx) {
        // TODO: implement
    }
    
private:
    WidgetPtr child_;
    std::shared_ptr<FocusNode> node_;
    bool autofocus_;
    bool canRequestFocus_;
    bool skipTraversal_;
};

using نطاق_تركيز = FocusScope;

/**
 * @brief (AR) فخ التركيز
 * @brief (EN) Focus trap
 */
class FocusTrap : public StatelessWidget {
public:
    FocusTrap(WidgetPtr child, bool active = true)
        : child_(child), active_(active) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
private:
    WidgetPtr child_;
    bool active_;
};

using فخ_تركيز = FocusTrap;

// ═══════════════════════════════════════════════════════════════════════════════
//  FocusTraversalGroup — مجموعة التنقل بالتركيز
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) سياسة التنقل
 * @brief (EN) Traversal policy
 */
class FocusTraversalPolicy {
public:
    virtual ~FocusTraversalPolicy() = default;
    
    virtual std::shared_ptr<FocusNode> findFirstFocus() { return nullptr; }
    virtual std::shared_ptr<FocusNode> findLastFocus() { return nullptr; }
    virtual std::shared_ptr<FocusNode> findNextFocus() { return nullptr; }
    virtual std::shared_ptr<FocusNode> findPreviousFocus() { return nullptr; }
};

/**
 * @brief (AR) سياسة التنقل بالترتيب
 * @brief (EN) Ordered traversal policy
 */
class OrderedTraversalPolicy : public FocusTraversalPolicy {
public:
    // TODO: implement
};

/**
 * @brief (AR) سياسة التنقل بالقراءة
 * @brief (EN) Reading order traversal policy
 */
class ReadingOrderTraversalPolicy : public FocusTraversalPolicy {
public:
    // TODO: implement
};

/**
 * @brief (AR) مجموعة التنقل بالتركيز
 * @brief (EN) Focus traversal group
 */
class FocusTraversalGroup : public StatelessWidget {
public:
    FocusTraversalGroup(WidgetPtr child,
                        std::shared_ptr<FocusTraversalPolicy> policy = nullptr,
                        bool descendantsAreFocusable = true,
                        bool descendantsAreTraversable = true)
        : child_(child), policy_(policy),
          descendantsAreFocusable_(descendantsAreFocusable),
          descendantsAreTraversable_(descendantsAreTraversable) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
private:
    WidgetPtr child_;
    std::shared_ptr<FocusTraversalPolicy> policy_;
    bool descendantsAreFocusable_;
    bool descendantsAreTraversable_;
};

using مجموعة_تنقل_تركيز = FocusTraversalGroup;

// ═══════════════════════════════════════════════════════════════════════════════
//  Focus State
// ═══════════════════════════════════════════════════════════════════════════════

class FocusState : public State<Focus> {
public:
    void initState() override {
        auto* w = widget();
        if (w->focusNode_) {
            focusNode_ = w->focusNode_;
        } else {
            focusNode_ = std::make_shared<FocusNode>(
                w->canRequestFocus_,
                w->skipTraversal_
            );
        }
        
        focusNode_->addListener([this]{
            if (widget()->onFocusChange_) {
                widget()->onFocusChange_(focusNode_->hasFocus());
            }
            setState([]{});
        });
        
        if (w->autofocus_) {
            focusNode_->requestFocus();
        }
    }
    
    void dispose() override {
        focusNode_->dispose();
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        return widget()->child_;
    }
    
private:
    std::shared_ptr<FocusNode> focusNode_;
    
    Focus* widget() const {
        return static_cast<Focus*>(widget_);
    }
};

inline std::unique_ptr<State_Base> Focus::createState() {
    return std::make_unique<FocusState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Accessibility Shortcuts
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إجراء مختصر
 * @brief (EN) Shortcut action
 */
struct ShortcutAction {
    std::string intent;
    std::function<void()> callback;
};

/**
 * @brief (AR) اختصار لوحة المفاتيح
 * @brief (EN) Keyboard shortcut
 */
struct KeyboardShortcut {
    int key;  // Virtual key code
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    bool meta = false;
    
    bool matches(const KeyEvent& event) const {
        // TODO: implement key matching
        return false;
    }
};

using اختصار_لوحة_مفاتيح = KeyboardShortcut;

/**
 * @brief (AR) مكون الاختصارات
 * @brief (EN) Shortcuts widget
 */
class Shortcuts : public StatelessWidget {
public:
    Shortcuts(WidgetPtr child,
              std::map<KeyboardShortcut, ShortcutAction> shortcuts)
        : child_(child), shortcuts_(std::move(shortcuts)) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child_; }
    
private:
    WidgetPtr child_;
    std::map<KeyboardShortcut, ShortcutAction> shortcuts_;
};

using اختصارات = Shortcuts;

// ═══════════════════════════════════════════════════════════════════════════════
//  MediaQuery Accessibility
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إعدادات الوصول في MediaQuery
 * @brief (EN) Accessibility settings in MediaQuery
 */
struct AccessibilityFeatures {
    bool accessibleNavigation = false;
    bool invertColors = false;
    bool disableAnimations = false;
    bool boldText = false;
    bool reduceMotion = false;
    bool highContrast = false;
    bool onOffSwitchLabels = false;
};

using ميزات_وصول = AccessibilityFeatures;

// ═══════════════════════════════════════════════════════════════════════════════
//  Live Region
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) منطقة حية للإعلانات
 * @brief (EN) Live region for announcements
 */
class LiveRegion : public StatelessWidget {
public:
    enum class Mode { Off, Polite, Assertive };
    
    LiveRegion(WidgetPtr child, Mode mode = Mode::Polite)
        : child_(child), mode_(mode) {}
    
    WidgetPtr build(BuildContext* ctx) override {
        SemanticsProperties props;
        props.liveRegion = (mode_ != Mode::Off);
        return std::make_shared<Semantics>(child_, props);
    }
    
private:
    WidgetPtr child_;
    Mode mode_;
};

using منطقة_حية = LiveRegion;

// ═══════════════════════════════════════════════════════════════════════════════
//  Announce — إعلان للقارئ
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إعلان للقارئ الصوتي
 * @brief (EN) Announce to screen reader
 */
inline void announce(const std::string& message, TextDirection direction = TextDirection::RTL) {
    // TODO: تطبيق إرسال الإعلان لقارئ الشاشة
    // سيتم ربطه مع واجهة النظام
}

inline void اعلن(const std::string& رسالة) {
    announce(رسالة, TextDirection::RTL);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إضافة وصف لمكون
 * @brief (EN) Add description to widget
 */
inline WidgetPtr اضف_وصف(WidgetPtr مكون, const std::string& وصف) {
    return Semantics::label(مكون, وصف);
}

/**
 * @brief (AR) جعل مكون قابل للتركيز
 * @brief (EN) Make widget focusable
 */
inline WidgetPtr اجعل_قابل_للتركيز(WidgetPtr مكون, 
                                     bool تركيز_تلقائي = false) {
    return std::make_shared<Focus>(مكون, nullptr, تركيز_تلقائي);
}

/**
 * @brief (AR) إضافة اختصار لوحة مفاتيح
 * @brief (EN) Add keyboard shortcut
 */
inline KeyboardShortcut اختصار(int مفتاح, 
                                bool ctrl = false, 
                                bool shift = false,
                                bool alt = false) {
    return KeyboardShortcut{مفتاح, ctrl, shift, alt, false};
}

} // namespace flutter
} // namespace sad

#endif // SAD_FLUTTER_ACCESSIBILITY_H
