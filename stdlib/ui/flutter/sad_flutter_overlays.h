// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_overlays.h — نظام الطبقات العلوية
//  Overlay System
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_FLUTTER_OVERLAYS_H
#define SAD_FLUTTER_OVERLAYS_H

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"
#include <algorithm>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  OverlayEntry — مدخل الطبقة العلوية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مدخل الطبقة العلوية
 * @brief (EN) Overlay entry
 */
class OverlayEntry {
public:
    std::function<WidgetPtr(BuildContext*)> builder;
    bool opaque = false;
    bool maintainState = false;
    bool canSizeOverlay = false;
    
    OverlayEntry(std::function<WidgetPtr(BuildContext*)> b,
                 bool op = false, bool ms = false)
        : builder(b), opaque(op), maintainState(ms) {}
    
    void markNeedsBuild() {
        // Trigger rebuild
        if (onMarkNeedsBuild_) onMarkNeedsBuild_();
    }
    
    void remove() {
        if (onRemove_) onRemove_();
    }
    
    bool mounted() const { return mounted_; }
    
    // Internal
    std::function<void()> onMarkNeedsBuild_;
    std::function<void()> onRemove_;
    bool mounted_ = false;
};

using مدخل_طبقة = OverlayEntry;

// ═══════════════════════════════════════════════════════════════════════════════
//  Overlay — الطبقة العلوية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) حالة الطبقة العلوية
 * @brief (EN) Overlay state (forward declaration)
 */
class OverlayState;

/**
 * @brief (AR) الطبقة العلوية
 * @brief (EN) Overlay widget
 */
class Overlay : public StatefulWidget {
public:
    std::vector<std::shared_ptr<OverlayEntry>> initialEntries;
    Clip clipBehavior = Clip::HardEdge;
    
    Overlay(std::vector<std::shared_ptr<OverlayEntry>> entries = {})
        : initialEntries(std::move(entries)) {}
    
    static OverlayState* of(BuildContext* ctx, 
                            bool rootOverlay = false,
                            Widget* debugRequiredFor = nullptr);
    
    static OverlayState* maybeOf(BuildContext* ctx, bool rootOverlay = false);
    
    std::unique_ptr<State_Base> createState() override;
};

using طبقة_علوية = Overlay;

/**
 * @brief (AR) حالة الطبقة العلوية
 * @brief (EN) Overlay state
 */
class OverlayState : public State<Overlay> {
public:
    void initState() override {
        auto* w = widget();
        for (auto& entry : w->initialEntries) {
            insert(entry);
        }
    }
    
    void insert(std::shared_ptr<OverlayEntry> entry, 
                std::shared_ptr<OverlayEntry> below = nullptr,
                std::shared_ptr<OverlayEntry> above = nullptr) {
        entry->mounted_ = true;
        entry->onRemove_ = [this, entry]{ 
            entries_.erase(std::remove(entries_.begin(), entries_.end(), entry), entries_.end());
            setState([]{});
        };
        entry->onMarkNeedsBuild_ = [this]{ setState([]{});};
        
        if (above) {
            auto it = std::find(entries_.begin(), entries_.end(), above);
            if (it != entries_.end()) {
                entries_.insert(it + 1, entry);
            } else {
                entries_.push_back(entry);
            }
        } else if (below) {
            auto it = std::find(entries_.begin(), entries_.end(), below);
            if (it != entries_.end()) {
                entries_.insert(it, entry);
            } else {
                entries_.push_back(entry);
            }
        } else {
            entries_.push_back(entry);
        }
        
        setState([]{});
    }
    
    void insertAll(std::vector<std::shared_ptr<OverlayEntry>> entries,
                   std::shared_ptr<OverlayEntry> below = nullptr,
                   std::shared_ptr<OverlayEntry> above = nullptr) {
        for (auto& entry : entries) {
            insert(entry, below, above);
        }
    }
    
    void rearrange(std::vector<std::shared_ptr<OverlayEntry>> newEntries,
                   std::shared_ptr<OverlayEntry> below = nullptr,
                   std::shared_ptr<OverlayEntry> above = nullptr) {
        entries_ = newEntries;
        setState([]{});
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        std::vector<WidgetPtr> children;
        for (auto& entry : entries_) {
            if (entry->builder) {
                children.push_back(entry->builder(ctx));
            }
        }
        return std::make_shared<Stack>(children);
    }
    
private:
    std::vector<std::shared_ptr<OverlayEntry>> entries_;
    Overlay* widget() const { return static_cast<Overlay*>(widget_); }
};

inline std::unique_ptr<State_Base> Overlay::createState() {
    return std::make_unique<OverlayState>();
}

inline OverlayState* Overlay::of(BuildContext* ctx, bool rootOverlay, Widget* debugRequiredFor) {
    // TODO: Traverse context to find overlay
    return nullptr;
}

inline OverlayState* Overlay::maybeOf(BuildContext* ctx, bool rootOverlay) {
    return of(ctx, rootOverlay);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  OverlayPortal — بوابة الطبقة العلوية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) متحكم البوابة
 * @brief (EN) Overlay portal controller
 */
class OverlayPortalController {
public:
    bool isShowing = false;
    
    void show() { 
        isShowing = true;
        notifyListeners();
    }
    
    void hide() {
        isShowing = false;
        notifyListeners();
    }
    
    void toggle() {
        isShowing = !isShowing;
        notifyListeners();
    }
    
    void addListener(std::function<void()> listener) {
        listeners_.push_back(listener);
    }
    
    void removeListener(std::function<void()> listener) {
        // Simplified removal
    }
    
private:
    void notifyListeners() {
        for (auto& l : listeners_) l();
    }
    std::vector<std::function<void()>> listeners_;
};

using متحكم_بوابة_طبقة = OverlayPortalController;

/**
 * @brief (AR) بوابة الطبقة العلوية
 * @brief (EN) Overlay portal
 */
class OverlayPortal : public StatefulWidget {
public:
    std::shared_ptr<OverlayPortalController> controller;
    std::function<WidgetPtr(BuildContext*)> overlayChildBuilder;
    WidgetPtr child;
    
    OverlayPortal(std::shared_ptr<OverlayPortalController> ctrl,
                  std::function<WidgetPtr(BuildContext*)> builder,
                  WidgetPtr c)
        : controller(ctrl), overlayChildBuilder(builder), child(c) {}
    
    std::unique_ptr<State_Base> createState() override;
};

using بوابة_طبقة = OverlayPortal;

class OverlayPortalState : public State<OverlayPortal> {
public:
    std::shared_ptr<OverlayEntry> entry_;
    
    void initState() override {
        auto* w = widget();
        w->controller->addListener([this]{ 
            updateOverlay();
        });
        
        entry_ = std::make_shared<OverlayEntry>([this](BuildContext* ctx) {
            if (widget()->controller->isShowing) {
                return widget()->overlayChildBuilder(ctx);
            }
            return WidgetPtr(nullptr);
        });
    }
    
    void updateOverlay() {
        if (entry_) entry_->markNeedsBuild();
        setState([]{});
    }
    
    void dispose() override {
        if (entry_) entry_->remove();
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        return widget()->child;
    }
    
private:
    OverlayPortal* widget() const { return static_cast<OverlayPortal*>(widget_); }
};

inline std::unique_ptr<State_Base> OverlayPortal::createState() {
    return std::make_unique<OverlayPortalState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  CompositedTransformFollower — تابع التحويل المركب
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) رابط الطبقة
 * @brief (EN) Layer link
 */
class LayerLink {
public:
    Size leaderSize;
};

using رابط_طبقة = LayerLink;

/**
 * @brief (AR) قائد التحويل المركب
 * @brief (EN) Composited transform target
 */
class CompositedTransformTarget : public StatelessWidget {
public:
    std::shared_ptr<LayerLink> link;
    WidgetPtr child;
    
    CompositedTransformTarget(std::shared_ptr<LayerLink> l, WidgetPtr c)
        : link(l), child(c) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using هدف_تحويل_مركب = CompositedTransformTarget;

/**
 * @brief (AR) تابع التحويل المركب
 * @brief (EN) Composited transform follower
 */
class CompositedTransformFollower : public StatelessWidget {
public:
    std::shared_ptr<LayerLink> link;
    bool showWhenUnlinked = true;
    Offset offset = Offset::zero();
    AlignmentGeometry targetAnchor = Alignment::topLeft();
    AlignmentGeometry followerAnchor = Alignment::topLeft();
    WidgetPtr child;
    
    CompositedTransformFollower(std::shared_ptr<LayerLink> l, WidgetPtr c = nullptr)
        : link(l), child(c) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using تابع_تحويل_مركب = CompositedTransformFollower;

// ═══════════════════════════════════════════════════════════════════════════════
//  Tooltip — تلميح
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نمط التلميح
 * @brief (EN) Tooltip trigger mode
 */
enum class TooltipTriggerMode {
    Manual,
    LongPress,
    Tap
};

using نمط_تشغيل_تلميح = TooltipTriggerMode;

/**
 * @brief (AR) التلميح
 * @brief (EN) Tooltip widget
 */
class Tooltip : public StatefulWidget {
public:
    std::string message;
    WidgetPtr richMessage;
    double height = 32.0;
    EdgeInsets padding = EdgeInsets::symmetric(8.0, 16.0);
    EdgeInsets margin;
    double verticalOffset = 24.0;
    bool preferBelow = true;
    bool excludeFromSemantics = false;
    BoxDecoration decoration;
    TextStyle textStyle;
    TextAlign textAlign = TextAlign::Start;
    Duration waitDuration = Duration::milliseconds(0);
    Duration showDuration = Duration::milliseconds(1500);
    Duration exitDuration = Duration::milliseconds(100);
    TooltipTriggerMode triggerMode = TooltipTriggerMode::LongPress;
    bool enableFeedback = true;
    std::function<void()> onTriggered;
    WidgetPtr child;
    
    Tooltip(std::string msg, WidgetPtr c) : message(std::move(msg)), child(c) {}
    
    std::unique_ptr<State_Base> createState() override;
};

using تلميح = Tooltip;

class TooltipState : public State<Tooltip> {
public:
    bool isVisible_ = false;
    std::shared_ptr<OverlayEntry> entry_;
    
    void showTooltip() {
        if (isVisible_) return;
        isVisible_ = true;
        // Create and insert overlay
        setState([]{});
    }
    
    void hideTooltip() {
        if (!isVisible_) return;
        isVisible_ = false;
        if (entry_) entry_->remove();
        setState([]{});
    }
    
    WidgetPtr build(BuildContext* ctx) override {
        return widget()->child;
    }
    
private:
    Tooltip* widget() const { return static_cast<Tooltip*>(widget_); }
};

inline std::unique_ptr<State_Base> Tooltip::createState() {
    return std::make_unique<TooltipState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  PopupMenuButton — زر القائمة المنبثقة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) عنصر القائمة المنبثقة
 * @brief (EN) Popup menu entry
 */
template<typename T>
class PopupMenuEntry {
public:
    virtual ~PopupMenuEntry() = default;
    virtual double height() const { return 48.0; }
    virtual bool enabled() const { return true; }
};

/**
 * @brief (AR) عنصر القائمة المنبثقة
 * @brief (EN) Popup menu item
 */
template<typename T>
class PopupMenuItem : public PopupMenuEntry<T> {
public:
    T value;
    std::function<void()> onTap;
    bool enabled_ = true;
    double height_ = 48.0;
    EdgeInsets padding;
    TextStyle textStyle;
    TextStyle labelTextStyle;
    MouseCursor mouseCursor = MouseCursor::Click;
    WidgetPtr child;
    
    PopupMenuItem(T v, WidgetPtr c) : value(v), child(c) {}
    
    double height() const override { return height_; }
    bool enabled() const override { return enabled_; }
};

template<typename T>
using عنصر_قائمة_منبثقة = PopupMenuItem<T>;

/**
 * @brief (AR) فاصل القائمة المنبثقة
 * @brief (EN) Popup menu divider
 */
template<typename T>
class PopupMenuDivider : public PopupMenuEntry<T> {
public:
    double height_ = 16.0;
    double height() const override { return height_; }
    bool enabled() const override { return false; }
};

template<typename T>
using فاصل_قائمة_منبثقة = PopupMenuDivider<T>;

/**
 * @brief (AR) زر القائمة المنبثقة
 * @brief (EN) Popup menu button
 */
template<typename T>
class PopupMenuButton : public StatefulWidget {
public:
    std::function<std::vector<std::shared_ptr<PopupMenuEntry<T>>>(BuildContext*)> itemBuilder;
    T initialValue;
    std::function<void()> onOpened;
    std::function<void(T)> onSelected;
    std::function<void()> onCanceled;
    std::string tooltip;
    double elevation = 8.0;
    EdgeInsets padding = EdgeInsets::all(8.0);
    Color splashRadius;
    WidgetPtr icon;
    double iconSize = 24.0;
    Offset offset = Offset::zero();
    bool enabled = true;
    ShapeBorder shape;
    Color color;
    Color iconColor;
    Color shadowColor;
    Color surfaceTintColor;
    bool enableFeedback = true;
    BoxConstraints constraints;
    PopupMenuPosition position = PopupMenuPosition::Over;
    Clip clipBehavior = Clip::None;
    WidgetPtr child;
    
    std::unique_ptr<State_Base> createState() override { return nullptr; }
};

template<typename T>
using زر_قائمة_منبثقة = PopupMenuButton<T>;

enum class PopupMenuPosition { Over, Under };
using موضع_قائمة_منبثقة = PopupMenuPosition;

// ═══════════════════════════════════════════════════════════════════════════════
//  DropdownButton — زر القائمة المنسدلة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) عنصر القائمة المنسدلة
 * @brief (EN) Dropdown menu item
 */
template<typename T>
struct DropdownMenuItem {
    T value;
    WidgetPtr child;
    std::function<void()> onTap;
    bool enabled = true;
    AlignmentGeometry alignment = Alignment::centerLeft();
    
    DropdownMenuItem(T v, WidgetPtr c) : value(v), child(c) {}
};

template<typename T>
using عنصر_قائمة_منسدلة = DropdownMenuItem<T>;

/**
 * @brief (AR) زر القائمة المنسدلة
 * @brief (EN) Dropdown button
 */
template<typename T>
class DropdownButton : public StatefulWidget {
public:
    std::vector<DropdownMenuItem<T>> items;
    T value;
    WidgetPtr hint;
    WidgetPtr disabledHint;
    std::function<void(T?)> onChanged;
    std::function<void()> onTap;
    int elevation = 8;
    TextStyle style;
    WidgetPtr underline;
    WidgetPtr icon;
    Color iconDisabledColor;
    Color iconEnabledColor;
    double iconSize = 24.0;
    bool isDense = false;
    bool isExpanded = false;
    double itemHeight = 48.0;
    Color focusColor;
    bool autofocus = false;
    Color dropdownColor;
    double menuMaxHeight = 0.0;
    bool enableFeedback = true;
    AlignmentGeometry alignment = Alignment::centerLeft();
    BorderRadius borderRadius;
    EdgeInsets padding;
    
    std::unique_ptr<State_Base> createState() override { return nullptr; }
};

template<typename T>
using زر_قائمة_منسدلة = DropdownButton<T>;

// ═══════════════════════════════════════════════════════════════════════════════
//  DropdownMenu — قائمة منسدلة (Material 3)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مدخل قائمة منسدلة
 * @brief (EN) Dropdown menu entry
 */
template<typename T>
struct DropdownMenuEntry {
    T value;
    std::string label;
    WidgetPtr labelWidget;
    WidgetPtr leadingIcon;
    WidgetPtr trailingIcon;
    bool enabled = true;
    ButtonStyle style;
    
    DropdownMenuEntry(T v, std::string lbl) : value(v), label(std::move(lbl)) {}
};

template<typename T>
using مدخل_قائمة_منسدلة = DropdownMenuEntry<T>;

/**
 * @brief (AR) قائمة منسدلة (Material 3)
 * @brief (EN) Dropdown menu (Material 3)
 */
template<typename T>
class DropdownMenu : public StatefulWidget {
public:
    bool enabled = true;
    double width = 0.0;
    double menuHeight = 0.0;
    WidgetPtr leadingIcon;
    WidgetPtr trailingIcon;
    WidgetPtr label;
    std::string hintText;
    std::string helperText;
    std::string errorText;
    WidgetPtr selectedTrailingIcon;
    bool enableFilter = false;
    bool enableSearch = true;
    std::shared_ptr<TextEditingController> controller;
    T initialSelection;
    std::function<void(T?)> onSelected;
    bool requestFocusOnTap = true;
    EdgeInsets expandedInsets;
    FilterCallback<T> filterCallback;
    SearchCallback<T> searchCallback;
    InputDecorationTheme inputDecorationTheme;
    MenuStyle menuStyle;
    std::vector<DropdownMenuEntry<T>> dropdownMenuEntries;
    
    std::unique_ptr<State_Base> createState() override { return nullptr; }
};

template<typename T>
using قائمة_منسدلة = DropdownMenu<T>;

template<typename T>
using FilterCallback = std::function<std::vector<DropdownMenuEntry<T>>(std::vector<DropdownMenuEntry<T>>, std::string)>;

template<typename T>
using SearchCallback = std::function<std::optional<int>(std::vector<DropdownMenuEntry<T>>, std::string)>;

// ═══════════════════════════════════════════════════════════════════════════════
//  MenuBar — شريط القوائم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) شريط القوائم
 * @brief (EN) Menu bar
 */
class MenuBar : public StatelessWidget {
public:
    MenuStyle style;
    Clip clipBehavior = Clip::None;
    std::shared_ptr<MenuController> controller;
    std::vector<WidgetPtr> children;
    
    MenuBar(std::vector<WidgetPtr> c) : children(std::move(c)) {}
    
    WidgetPtr build(BuildContext* ctx) override {
        return std::make_shared<Row>(children);
    }
};

using شريط_قوائم = MenuBar;

/**
 * @brief (AR) متحكم القائمة
 * @brief (EN) Menu controller
 */
class MenuController {
public:
    bool isOpen = false;
    
    void open() { isOpen = true; }
    void close() { isOpen = false; }
};

using متحكم_قائمة = MenuController;

/**
 * @brief (AR) مرتكز القائمة
 * @brief (EN) Menu anchor
 */
class MenuAnchor : public StatefulWidget {
public:
    std::shared_ptr<MenuController> controller;
    std::function<std::vector<WidgetPtr>(BuildContext*, std::shared_ptr<MenuController>)> menuChildren;
    std::function<WidgetPtr(BuildContext*, std::shared_ptr<MenuController>, WidgetPtr)> builder;
    WidgetPtr child;
    MenuStyle style;
    AlignmentGeometry alignmentOffset;
    Clip clipBehavior = Clip::HardEdge;
    bool anchorTapClosesMenu = true;
    std::function<void(bool)> onOpen;
    std::function<void()> onClose;
    bool crossAxisUnconstrained = true;
    bool consumeOutsideTap = false;
    
    std::unique_ptr<State_Base> createState() override { return nullptr; }
};

using مرتكز_قائمة = MenuAnchor;

/**
 * @brief (AR) عنصر زر القائمة
 * @brief (EN) Menu item button
 */
class MenuItemButton : public StatelessWidget {
public:
    std::function<void()> onPressed;
    std::function<void()> onHover;
    bool requestFocusOnHover = true;
    std::function<void(bool)> onFocusChange;
    bool autofocus = false;
    Clip clipBehavior = Clip::None;
    ButtonStyle style;
    bool closeOnActivate = true;
    WidgetPtr leadingIcon;
    WidgetPtr trailingIcon;
    WidgetPtr child;
    
    MenuItemButton(WidgetPtr c, std::function<void()> onPressed = nullptr)
        : child(c), onPressed(onPressed) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using زر_عنصر_قائمة = MenuItemButton;

/**
 * @brief (AR) زر قائمة فرعية
 * @brief (EN) Submenu button
 */
class SubmenuButton : public StatelessWidget {
public:
    std::function<void()> onHover;
    std::function<void(bool)> onFocusChange;
    std::function<void(bool)> onOpen;
    std::function<void()> onClose;
    std::shared_ptr<MenuController> controller;
    ButtonStyle style;
    MenuStyle menuStyle;
    AlignmentGeometry alignmentOffset;
    Clip clipBehavior = Clip::HardEdge;
    WidgetPtr leadingIcon;
    WidgetPtr trailingIcon;
    std::vector<WidgetPtr> menuChildren;
    WidgetPtr child;
    
    SubmenuButton(WidgetPtr c, std::vector<WidgetPtr> children)
        : child(c), menuChildren(std::move(children)) {}
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using زر_قائمة_فرعية = SubmenuButton;

/**
 * @brief (AR) نمط القائمة
 * @brief (EN) Menu style
 */
struct MenuStyle {
    Color backgroundColor;
    Color shadowColor;
    Color surfaceTintColor;
    double elevation = 8.0;
    EdgeInsets padding;
    Size minimumSize;
    Size fixedSize;
    Size maximumSize;
    BorderSide side;
    ShapeBorder shape;
    AlignmentGeometry alignment;
    MouseCursor mouseCursor = MouseCursor::Click;
    VisualDensity visualDensity;
};

using نمط_قائمة = MenuStyle;

// ═══════════════════════════════════════════════════════════════════════════════
//  ContextMenu — قائمة السياق
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) منطقة قائمة السياق
 * @brief (EN) Context menu region
 */
class ContextMenuRegion : public StatefulWidget {
public:
    std::function<WidgetPtr(BuildContext*, Offset)> contextMenuBuilder;
    WidgetPtr child;
    
    ContextMenuRegion(std::function<WidgetPtr(BuildContext*, Offset)> builder, WidgetPtr c)
        : contextMenuBuilder(builder), child(c) {}
    
    std::unique_ptr<State_Base> createState() override { return nullptr; }
};

using منطقة_قائمة_سياق = ContextMenuRegion;

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إظهار قائمة منبثقة
 * @brief (EN) Show popup menu
 */
template<typename T>
inline void اظهر_قائمة_منبثقة(
    BuildContext* سياق,
    RelativeRect موضع,
    std::vector<std::shared_ptr<PopupMenuEntry<T>>> عناصر,
    std::function<void(T)> عند_الاختيار = nullptr
) {
    // Show popup menu at position
}

/**
 * @brief (AR) إنشاء تلميح
 * @brief (EN) Create tooltip
 */
inline std::shared_ptr<Tooltip> انشئ_تلميح(const std::string& رسالة, WidgetPtr ابن) {
    return std::make_shared<Tooltip>(رسالة, ابن);
}

} // namespace flutter
} // namespace sad

#endif // SAD_FLUTTER_OVERLAYS_H
