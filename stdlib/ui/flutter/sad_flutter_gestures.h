// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_gestures.h — نظام الإيماءات
//  Gesture System
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_FLUTTER_GESTURES_H
#define SAD_FLUTTER_GESTURES_H

#include "sad_flutter_core.h"
#include "sad_flutter_widgets.h"

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Gesture Details — تفاصيل الإيماءات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) تفاصيل النقر
 * @brief (EN) Tap details
 */
struct TapDownDetails {
    Offset globalPosition;
    Offset localPosition;
    PointerDeviceKind kind = PointerDeviceKind::Touch;
};

struct TapUpDetails {
    Offset globalPosition;
    Offset localPosition;
    PointerDeviceKind kind = PointerDeviceKind::Touch;
};

using تفاصيل_ضغط = TapDownDetails;
using تفاصيل_رفع = TapUpDetails;

/**
 * @brief (AR) تفاصيل الضغط المطول
 * @brief (EN) Long press details
 */
struct LongPressStartDetails {
    Offset globalPosition;
    Offset localPosition;
};

struct LongPressMoveUpdateDetails {
    Offset globalPosition;
    Offset localPosition;
    Offset offsetFromOrigin;
    Offset localOffsetFromOrigin;
};

struct LongPressEndDetails {
    Offset globalPosition;
    Offset localPosition;
    Velocity velocity;
};

using تفاصيل_بدء_ضغط_مطول = LongPressStartDetails;
using تفاصيل_تحديث_ضغط_مطول = LongPressMoveUpdateDetails;
using تفاصيل_نهاية_ضغط_مطول = LongPressEndDetails;

/**
 * @brief (AR) تفاصيل السحب
 * @brief (EN) Drag details
 */
struct DragStartDetails {
    Offset globalPosition;
    Offset localPosition;
    Duration sourceTimeStamp;
};

struct DragUpdateDetails {
    Offset globalPosition;
    Offset localPosition;
    Offset delta;
    double primaryDelta = 0.0;
    Duration sourceTimeStamp;
};

struct DragEndDetails {
    Velocity velocity;
    double primaryVelocity = 0.0;
};

using تفاصيل_بدء_سحب = DragStartDetails;
using تفاصيل_تحديث_سحب = DragUpdateDetails;
using تفاصيل_نهاية_سحب = DragEndDetails;

/**
 * @brief (AR) تفاصيل التكبير
 * @brief (EN) Scale details
 */
struct ScaleStartDetails {
    Offset focalPoint;
    Offset localFocalPoint;
    int pointerCount = 1;
};

struct ScaleUpdateDetails {
    Offset focalPoint;
    Offset localFocalPoint;
    double scale = 1.0;
    double horizontalScale = 1.0;
    double verticalScale = 1.0;
    double rotation = 0.0;
    int pointerCount = 1;
};

struct ScaleEndDetails {
    Velocity velocity;
    int pointerCount = 1;
};

using تفاصيل_بدء_تكبير = ScaleStartDetails;
using تفاصيل_تحديث_تكبير = ScaleUpdateDetails;
using تفاصيل_نهاية_تكبير = ScaleEndDetails;

/**
 * @brief (AR) تفاصيل السحب بإصبعين
 * @brief (EN) Pan details (alias for drag)
 */
using PanStartDetails = DragStartDetails;
using PanUpdateDetails = DragUpdateDetails;
using PanEndDetails = DragEndDetails;

// ═══════════════════════════════════════════════════════════════════════════════
//  Velocity — السرعة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) السرعة
 * @brief (EN) Velocity
 */
struct Velocity {
    Offset pixelsPerSecond;
    
    static Velocity zero() { return Velocity{{0, 0}}; }
    
    Velocity operator+(const Velocity& other) const {
        return Velocity{{pixelsPerSecond.dx + other.pixelsPerSecond.dx,
                         pixelsPerSecond.dy + other.pixelsPerSecond.dy}};
    }
    
    Velocity operator-(const Velocity& other) const {
        return Velocity{{pixelsPerSecond.dx - other.pixelsPerSecond.dx,
                         pixelsPerSecond.dy - other.pixelsPerSecond.dy}};
    }
    
    Velocity operator*(double factor) const {
        return Velocity{{pixelsPerSecond.dx * factor, pixelsPerSecond.dy * factor}};
    }
};

using سرعة = Velocity;

// ═══════════════════════════════════════════════════════════════════════════════
//  GestureDetector — كاشف الإيماءات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) كاشف الإيماءات الرئيسي
 * @brief (EN) Main gesture detector widget
 */
class GestureDetector : public StatelessWidget {
public:
    // Callbacks
    std::function<void()> onTap;
    std::function<void(TapDownDetails)> onTapDown;
    std::function<void(TapUpDetails)> onTapUp;
    std::function<void()> onTapCancel;
    std::function<void()> onDoubleTap;
    std::function<void()> onDoubleTapCancel;
    std::function<void(TapDownDetails)> onDoubleTapDown;
    
    // Long press
    std::function<void()> onLongPress;
    std::function<void(LongPressStartDetails)> onLongPressStart;
    std::function<void(LongPressMoveUpdateDetails)> onLongPressMoveUpdate;
    std::function<void()> onLongPressUp;
    std::function<void(LongPressEndDetails)> onLongPressEnd;
    std::function<void()> onLongPressCancel;
    
    // Secondary (right-click)
    std::function<void()> onSecondaryTap;
    std::function<void(TapDownDetails)> onSecondaryTapDown;
    std::function<void(TapUpDetails)> onSecondaryTapUp;
    std::function<void()> onSecondaryTapCancel;
    std::function<void()> onSecondaryLongPress;
    std::function<void(LongPressStartDetails)> onSecondaryLongPressStart;
    std::function<void(LongPressMoveUpdateDetails)> onSecondaryLongPressMoveUpdate;
    std::function<void()> onSecondaryLongPressUp;
    std::function<void(LongPressEndDetails)> onSecondaryLongPressEnd;
    
    // Vertical drag
    std::function<void(DragStartDetails)> onVerticalDragStart;
    std::function<void(DragUpdateDetails)> onVerticalDragUpdate;
    std::function<void(DragEndDetails)> onVerticalDragEnd;
    std::function<void()> onVerticalDragCancel;
    std::function<void(DragDownDetails)> onVerticalDragDown;
    
    // Horizontal drag
    std::function<void(DragStartDetails)> onHorizontalDragStart;
    std::function<void(DragUpdateDetails)> onHorizontalDragUpdate;
    std::function<void(DragEndDetails)> onHorizontalDragEnd;
    std::function<void()> onHorizontalDragCancel;
    std::function<void(DragDownDetails)> onHorizontalDragDown;
    
    // Pan (free drag)
    std::function<void(DragStartDetails)> onPanStart;
    std::function<void(DragUpdateDetails)> onPanUpdate;
    std::function<void(DragEndDetails)> onPanEnd;
    std::function<void()> onPanCancel;
    std::function<void(DragDownDetails)> onPanDown;
    
    // Scale (pinch)
    std::function<void(ScaleStartDetails)> onScaleStart;
    std::function<void(ScaleUpdateDetails)> onScaleUpdate;
    std::function<void(ScaleEndDetails)> onScaleEnd;
    
    // Force press (3D Touch)
    std::function<void(ForcePressDetails)> onForcePressStart;
    std::function<void(ForcePressDetails)> onForcePressPeak;
    std::function<void(ForcePressDetails)> onForcePressUpdate;
    std::function<void(ForcePressDetails)> onForcePressEnd;
    
    // Properties
    WidgetPtr child;
    HitTestBehavior behavior = HitTestBehavior::DeferToChild;
    bool excludeFromSemantics = false;
    DragStartBehavior dragStartBehavior = DragStartBehavior::Start;
    
    GestureDetector() = default;
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using كاشف_ايماءات = GestureDetector;

/**
 * @brief (AR) تفاصيل السحب للأسفل
 * @brief (EN) Drag down details
 */
struct DragDownDetails {
    Offset globalPosition;
    Offset localPosition;
};

/**
 * @brief (AR) تفاصيل الضغط القوي
 * @brief (EN) Force press details
 */
struct ForcePressDetails {
    Offset globalPosition;
    Offset localPosition;
    double pressure = 0.0;
};

/**
 * @brief (AR) سلوك بدء السحب
 * @brief (EN) Drag start behavior
 */
enum class DragStartBehavior {
    Down,
    Start
};

/**
 * @brief (AR) سلوك اختبار الإصابة
 * @brief (EN) Hit test behavior
 */
enum class HitTestBehavior {
    DeferToChild,
    Opaque,
    Translucent
};

// ═══════════════════════════════════════════════════════════════════════════════
//  InkWell — تأثير الحبر
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) منطقة قابلة للنقر مع تأثير حبر
 * @brief (EN) Tappable area with ink splash effect
 */
class InkWell : public StatelessWidget {
public:
    WidgetPtr child;
    std::function<void()> onTap;
    std::function<void()> onDoubleTap;
    std::function<void()> onLongPress;
    std::function<void(TapDownDetails)> onTapDown;
    std::function<void(TapUpDetails)> onTapUp;
    std::function<void()> onTapCancel;
    std::function<void(bool)> onHighlightChanged;
    std::function<void(bool)> onHover;
    std::function<void(bool)> onFocusChange;
    
    Color focusColor;
    Color hoverColor;
    Color highlightColor;
    Color splashColor;
    
    BorderRadius borderRadius;
    ShapeBorder customBorder;
    bool enableFeedback = true;
    bool excludeFromSemantics = false;
    bool canRequestFocus = true;
    bool autofocus = false;
    
    InkWell() = default;
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using بئر_حبر = InkWell;

/**
 * @brief (AR) استجابة الحبر (مساحة أكبر)
 * @brief (EN) Ink response (larger area)
 */
class InkResponse : public StatelessWidget {
public:
    WidgetPtr child;
    std::function<void()> onTap;
    std::function<void()> onDoubleTap;
    std::function<void()> onLongPress;
    std::function<void(TapDownDetails)> onTapDown;
    std::function<void(TapUpDetails)> onTapUp;
    std::function<void()> onTapCancel;
    
    Color focusColor;
    Color hoverColor;
    Color highlightColor;
    Color splashColor;
    
    double radius = 0.0;
    BorderRadius borderRadius;
    bool containedInkWell = false;
    BoxShape highlightShape = BoxShape::Circle;
    
    InkResponse() = default;
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using استجابة_حبر = InkResponse;

// ═══════════════════════════════════════════════════════════════════════════════
//  Dismissible — قابل للإزالة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) اتجاه الإزالة
 * @brief (EN) Dismiss direction
 */
enum class DismissDirection {
    Vertical,
    Horizontal,
    EndToStart,
    StartToEnd,
    Up,
    Down,
    None
};

using اتجاه_ازالة = DismissDirection;

/**
 * @brief (AR) تفاصيل الإزالة
 * @brief (EN) Dismiss update details
 */
struct DismissUpdateDetails {
    DismissDirection direction;
    bool reached = false;
    bool previousReached = false;
    double progress = 0.0;
};

/**
 * @brief (AR) مكون قابل للسحب والإزالة
 * @brief (EN) Dismissible widget
 */
class Dismissible : public StatefulWidget {
public:
    WidgetPtr child;
    WidgetPtr background;
    WidgetPtr secondaryBackground;
    std::function<bool(DismissDirection)> confirmDismiss;
    std::function<void(DismissDirection)> onDismissed;
    std::function<void()> onResize;
    std::function<void(DismissUpdateDetails)> onUpdate;
    DismissDirection direction = DismissDirection::Horizontal;
    Duration resizeDuration = Duration::milliseconds(300);
    std::map<DismissDirection, double> dismissThresholds;
    Duration movementDuration = Duration::milliseconds(200);
    double crossAxisEndOffset = 0.0;
    DragStartBehavior dragStartBehavior = DragStartBehavior::Start;
    HitTestBehavior hitTestBehavior = HitTestBehavior::Opaque;
    
    Key key_;
    
    Dismissible(Key k = Key()) : key_(k) {}
    
    std::unique_ptr<State_Base> createState() override;
};

using قابل_للازالة = Dismissible;

// ═══════════════════════════════════════════════════════════════════════════════
//  Draggable & DragTarget — السحب والإفلات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مكون قابل للسحب
 * @brief (EN) Draggable widget
 */
template<typename T>
class Draggable : public StatefulWidget {
public:
    T data;
    WidgetPtr child;
    WidgetPtr feedback;
    WidgetPtr childWhenDragging;
    Offset feedbackOffset = Offset::zero();
    DragAnchor dragAnchor = DragAnchor::Child;
    Axis affinity = Axis::Vertical;  // Optional
    int maxSimultaneousDrags = 1;
    std::function<void()> onDragStarted;
    std::function<void(DraggableDetails)> onDragEnd;
    std::function<void(DragUpdateDetails)> onDragUpdate;
    std::function<void()> onDraggableCanceled;
    std::function<void(Velocity, Offset)> onDragCompleted;
    bool ignoringFeedbackSemantics = true;
    bool ignoringFeedbackPointer = true;
    bool rootOverlay = false;
    HitTestBehavior hitTestBehavior = HitTestBehavior::DeferToChild;
    
    std::unique_ptr<State_Base> createState() override { return nullptr; }
};

template<typename T>
using قابل_للسحب = Draggable<T>;

/**
 * @brief (AR) مرتكز السحب
 * @brief (EN) Drag anchor
 */
enum class DragAnchor {
    Child,
    Pointer
};

/**
 * @brief (AR) تفاصيل السحب
 * @brief (EN) Draggable details
 */
struct DraggableDetails {
    bool wasAccepted = false;
    Velocity velocity;
    Offset offset;
};

/**
 * @brief (AR) هدف السحب
 * @brief (EN) Drag target
 */
template<typename T>
class DragTarget : public StatefulWidget {
public:
    std::function<WidgetPtr(BuildContext*, std::vector<T*>, std::vector<T*>)> builder;
    std::function<bool(T*)> onWillAccept;
    std::function<bool(DragTargetDetails<T>)> onWillAcceptWithDetails;
    std::function<void(T*)> onAccept;
    std::function<void(DragTargetDetails<T>)> onAcceptWithDetails;
    std::function<void(T*)> onLeave;
    std::function<void(DragTargetDetails<T>)> onMove;
    HitTestBehavior hitTestBehavior = HitTestBehavior::Translucent;
    
    std::unique_ptr<State_Base> createState() override { return nullptr; }
};

template<typename T>
using هدف_سحب = DragTarget<T>;

/**
 * @brief (AR) تفاصيل هدف السحب
 * @brief (EN) Drag target details
 */
template<typename T>
struct DragTargetDetails {
    T data;
    Offset offset;
};

/**
 * @brief (AR) سحب وإفلات طويل
 * @brief (EN) Long press draggable
 */
template<typename T>
class LongPressDraggable : public Draggable<T> {
public:
    Duration delay = Duration::milliseconds(500);
    bool hapticFeedbackOnStart = true;
};

template<typename T>
using قابل_للسحب_المطول = LongPressDraggable<T>;

// ═══════════════════════════════════════════════════════════════════════════════
//  Swipe Actions — إجراءات السحب
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إجراء سحب
 * @brief (EN) Swipe action
 */
struct SwipeAction {
    WidgetPtr icon;
    WidgetPtr label;
    Color backgroundColor;
    Color foregroundColor;
    std::function<void()> onTap;
    bool autoClose = true;
};

using اجراء_سحب = SwipeAction;

/**
 * @brief (AR) مكون إجراءات السحب
 * @brief (EN) Swipe action cell
 */
class SwipeActionCell : public StatefulWidget {
public:
    WidgetPtr child;
    std::vector<SwipeAction> leadingActions;
    std::vector<SwipeAction> trailingActions;
    bool closeOnTap = true;
    double actionExtentRatio = 0.25;
    
    std::unique_ptr<State_Base> createState() override { return nullptr; }
};

using خلية_اجراءات_سحب = SwipeActionCell;

// ═══════════════════════════════════════════════════════════════════════════════
//  Pointer Events — أحداث المؤشر
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نوع جهاز المؤشر
 * @brief (EN) Pointer device kind
 */
enum class PointerDeviceKind {
    Touch,
    Mouse,
    Stylus,
    InvertedStylus,
    Trackpad,
    Unknown
};

using نوع_جهاز_مؤشر = PointerDeviceKind;

/**
 * @brief (AR) حدث المؤشر
 * @brief (EN) Pointer event
 */
struct PointerEvent {
    int pointer = 0;
    PointerDeviceKind kind = PointerDeviceKind::Touch;
    Offset position;
    Offset localPosition;
    Offset delta;
    Offset localDelta;
    double pressure = 1.0;
    double pressureMin = 0.0;
    double pressureMax = 1.0;
    double distance = 0.0;
    double distanceMax = 0.0;
    double size = 0.0;
    double radiusMajor = 0.0;
    double radiusMinor = 0.0;
    double radiusMin = 0.0;
    double radiusMax = 0.0;
    double orientation = 0.0;
    double tilt = 0.0;
    int buttons = 0;
    bool obscured = false;
    bool synthesized = false;
    int embedderId = 0;
    Duration timeStamp;
};

using حدث_مؤشر = PointerEvent;

/**
 * @brief (AR) مستمع المؤشر
 * @brief (EN) Pointer listener
 */
class Listener : public StatelessWidget {
public:
    WidgetPtr child;
    std::function<void(PointerDownEvent)> onPointerDown;
    std::function<void(PointerMoveEvent)> onPointerMove;
    std::function<void(PointerUpEvent)> onPointerUp;
    std::function<void(PointerHoverEvent)> onPointerHover;
    std::function<void(PointerCancelEvent)> onPointerCancel;
    std::function<void(PointerPanZoomStartEvent)> onPointerPanZoomStart;
    std::function<void(PointerPanZoomUpdateEvent)> onPointerPanZoomUpdate;
    std::function<void(PointerPanZoomEndEvent)> onPointerPanZoomEnd;
    std::function<void(PointerSignalEvent)> onPointerSignal;
    HitTestBehavior behavior = HitTestBehavior::DeferToChild;
    
    Listener() = default;
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using مستمع = Listener;

// Pointer event types
struct PointerDownEvent : PointerEvent {};
struct PointerMoveEvent : PointerEvent {};
struct PointerUpEvent : PointerEvent {};
struct PointerHoverEvent : PointerEvent {};
struct PointerCancelEvent : PointerEvent {};
struct PointerPanZoomStartEvent : PointerEvent {};
struct PointerPanZoomUpdateEvent : PointerEvent {};
struct PointerPanZoomEndEvent : PointerEvent {};
struct PointerSignalEvent : PointerEvent {};
struct PointerScrollEvent : PointerSignalEvent {
    Offset scrollDelta;
};

/**
 * @brief (AR) منطقة الماوس
 * @brief (EN) Mouse region
 */
class MouseRegion : public StatelessWidget {
public:
    WidgetPtr child;
    std::function<void(PointerEnterEvent)> onEnter;
    std::function<void(PointerExitEvent)> onExit;
    std::function<void(PointerHoverEvent)> onHover;
    MouseCursor cursor = MouseCursor::Defer;
    bool opaque = true;
    HitTestBehavior hitTestBehavior = HitTestBehavior::Opaque;
    
    MouseRegion() = default;
    
    WidgetPtr build(BuildContext* ctx) override { return child; }
};

using منطقة_ماوس = MouseRegion;

struct PointerEnterEvent : PointerEvent {};
struct PointerExitEvent : PointerEvent {};

/**
 * @brief (AR) مؤشر الماوس
 * @brief (EN) Mouse cursor
 */
enum class MouseCursor {
    Defer,
    Uncontrolled,
    None,
    Basic,
    Click,
    Forbidden,
    Wait,
    Progress,
    ContextMenu,
    Help,
    Text,
    VerticalText,
    Cell,
    Precise,
    Move,
    Grab,
    Grabbing,
    NoDrop,
    Alias,
    Copy,
    Disappearing,
    AllScroll,
    ResizeLeftRight,
    ResizeUpDown,
    ResizeUpLeftDownRight,
    ResizeUpRightDownLeft,
    ResizeUp,
    ResizeDown,
    ResizeLeft,
    ResizeRight,
    ResizeUpLeft,
    ResizeUpRight,
    ResizeDownLeft,
    ResizeDownRight,
    ResizeColumn,
    ResizeRow,
    ZoomIn,
    ZoomOut
};

using مؤشر_ماوس = MouseCursor;

// ═══════════════════════════════════════════════════════════════════════════════
//  Dismissible State
// ═══════════════════════════════════════════════════════════════════════════════

class DismissibleState : public State<Dismissible> {
public:
    void initState() override {}
    void dispose() override {}
    
    WidgetPtr build(BuildContext* ctx) override {
        return widget()->child;
    }

private:
    Dismissible* widget() const {
        return static_cast<Dismissible*>(widget_);
    }
};

inline std::unique_ptr<State_Base> Dismissible::createState() {
    return std::make_unique<DismissibleState>();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة للإيماءات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء كاشف إيماءات بسيط
 * @brief (EN) Create simple gesture detector
 */
inline std::shared_ptr<GestureDetector> انشئ_كاشف_ايماءات(
    WidgetPtr ابن,
    std::function<void()> عند_النقر = nullptr,
    std::function<void()> عند_النقر_المزدوج = nullptr,
    std::function<void()> عند_الضغط_المطول = nullptr
) {
    auto detector = std::make_shared<GestureDetector>();
    detector->child = ابن;
    detector->onTap = عند_النقر;
    detector->onDoubleTap = عند_النقر_المزدوج;
    detector->onLongPress = عند_الضغط_المطول;
    return detector;
}

/**
 * @brief (AR) إنشاء مكون قابل للسحب
 * @brief (EN) Create draggable widget
 */
template<typename T>
inline std::shared_ptr<Draggable<T>> انشئ_قابل_للسحب(
    T بيانات,
    WidgetPtr ابن,
    WidgetPtr ردود = nullptr
) {
    auto draggable = std::make_shared<Draggable<T>>();
    draggable->data = بيانات;
    draggable->child = ابن;
    draggable->feedback = ردود ? ردود : ابن;
    return draggable;
}

} // namespace flutter
} // namespace sad

#endif // SAD_FLUTTER_GESTURES_H
