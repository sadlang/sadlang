// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_gesture.h — نظام كشف الإيماءات المتقدم
// ─────────────────────────────────────────────────────────────────────────────
// يوفر نظام إيماءات شامل مستوحى من Flutter GestureDetector:
//   - نقرة (Tap)، نقرة مزدوجة (DoubleTap)، ضغط طويل (LongPress)
//   - سحب (Drag) أفقي/عمودي/حر
//   - تمرير (Swipe) بكشف الاتجاه
//   - تكبير/تصغير (Pinch Zoom) — للمستقبل
//   - ساحة الإيماءات (Gesture Arena) لفض التعارض
//   - فيزياء التمرير (Scroll Physics) — ارتداد + زخم
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once
#include <string>
#include <vector>
#include <functional>

namespace sad { namespace gesture {

// ═══════════════════════════════════════════════════════════════════
// أنواع الإيماءات
// ═══════════════════════════════════════════════════════════════════
enum class GestureType {
    Tap,            // نقرة واحدة
    DoubleTap,      // نقرة مزدوجة
    LongPress,      // ضغط طويل (> 500ms)
    DragStart,      // بداية السحب
    DragUpdate,     // تحديث السحب (حركة مستمرة)
    DragEnd,        // نهاية السحب
    SwipeLeft,      // تمرير لليسار
    SwipeRight,     // تمرير لليمين
    SwipeUp,        // تمرير للأعلى
    SwipeDown,      // تمرير للأسفل
    Hover,          // المؤشر فوق العنصر
    HoverExit,      // المؤشر غادر العنصر
    ScrollStart,    // بداية التمرير
    ScrollUpdate,   // تحديث التمرير
    ScrollEnd       // نهاية التمرير
};

// ═══════════════════════════════════════════════════════════════════
// بيانات الإيماءة
// ═══════════════════════════════════════════════════════════════════
struct GestureData {
    GestureType type;
    int widgetId = 0;       // الودجت المستهدف
    
    // موضع المؤشر
    float x = 0, y = 0;        // الموضع الحالي
    float startX = 0, startY = 0;  // موضع البداية (للسحب)
    
    // السرعة (للسحب والتمرير)
    float velocityX = 0;    // السرعة الأفقية (بكسل/ثانية)
    float velocityY = 0;    // السرعة الرأسية
    
    // إزاحة السحب
    float deltaX = 0;       // الإزاحة من آخر تحديث
    float deltaY = 0;
    float totalDeltaX = 0;  // الإزاحة الكلية من البداية
    float totalDeltaY = 0;
    
    // التمرير
    float scrollDelta = 0;  // مقدار التمرير
    
    // معلومات إضافية
    double timestamp = 0;   // وقت الإيماءة (مللي ثانية)
    int tapCount = 0;       // عدد النقرات المتتالية
};

// ═══════════════════════════════════════════════════════════════════
// معرّف مستمع الإيماءات
// ═══════════════════════════════════════════════════════════════════
using GestureListenerId = int;

// ═══════════════════════════════════════════════════════════════════
// فيزياء التمرير — تتحكم في سلوك التمرير بعد رفع الإصبع
// ═══════════════════════════════════════════════════════════════════
enum class ScrollPhysicsType {
    Clamped,        // لا ارتداد — يتوقف عند الحدود (مثل Android)
    Bouncing,       // ارتداد — يتجاوز الحدود ثم يرتد (مثل iOS)
    Never,          // تمرير معطّل تماماً
    Page            // تمرير صفحات — ينتقل لأقرب صفحة كاملة
};

struct ScrollPhysics {
    ScrollPhysicsType type = ScrollPhysicsType::Bouncing;
    float friction = 0.015f;        // معامل الاحتكاك (لإبطاء الزخم)
    float bounceFactor = 0.5f;      // قوة الارتداد (0-1)
    float minVelocity = 50.0f;      // أقل سرعة للحركة بالزخم
    float maxVelocity = 8000.0f;    // أقصى سرعة
    float overscrollMax = 100.0f;   // أقصى تجاوز للحدود (بالبكسل)
};

// ═══════════════════════════════════════════════════════════════════
// حالة التمرير — لكل عنصر قابل للتمرير
// ═══════════════════════════════════════════════════════════════════
struct ScrollState {
    int widgetId = 0;
    float offset = 0;           // الإزاحة الحالية
    float velocity = 0;         // السرعة الحالية
    float contentSize = 0;      // حجم المحتوى الكلي
    float viewportSize = 0;     // حجم المنطقة المرئية
    bool isDragging = false;    // هل يسحب المستخدم حالياً؟
    bool isAnimating = false;   // هل يتحرك بالزخم؟
    float overscroll = 0;       // مقدار التجاوز للحدود
    ScrollPhysics physics;
};

// ═══════════════════════════════════════════════════════════════════
// تسجيل مستمعي الإيماءات
// ═══════════════════════════════════════════════════════════════════

// تسجيل مستمع نقرة على ودجت
GestureListenerId onTap(int widgetId, std::function<void(const GestureData&)> callback);

// تسجيل نقرة مزدوجة
GestureListenerId onDoubleTap(int widgetId, std::function<void(const GestureData&)> callback);

// تسجيل ضغط طويل
GestureListenerId onLongPress(int widgetId, std::function<void(const GestureData&)> callback);

// تسجيل أحداث السحب (بداية + تحديث + نهاية)
GestureListenerId onDragStart(int widgetId, std::function<void(const GestureData&)> callback);
GestureListenerId onDragUpdate(int widgetId, std::function<void(const GestureData&)> callback);
GestureListenerId onDragEnd(int widgetId, std::function<void(const GestureData&)> callback);

// تسجيل تمرير (Swipe) — يكشف الاتجاه تلقائياً
GestureListenerId onSwipe(int widgetId, std::function<void(const GestureData&)> callback);

// تسجيل تحويم (Hover)
GestureListenerId onHover(int widgetId, std::function<void(const GestureData&)> callback);
GestureListenerId onHoverExit(int widgetId, std::function<void(const GestureData&)> callback);

// مستمع عام لأي إيماءة
GestureListenerId onGesture(int widgetId, GestureType type,
                            std::function<void(const GestureData&)> callback);

// ═══════════════════════════════════════════════════════════════════
// إدارة المستمعين
// ═══════════════════════════════════════════════════════════════════

// إلغاء مستمع بمعرّفه
void removeGestureListener(GestureListenerId id);

// إلغاء جميع مستمعي ودجت معيّن
void removeAllGestureListeners(int widgetId);

// إلغاء جميع المستمعين
void clearAllGestureListeners();

// عدد المستمعين النشطين
int getGestureListenerCount();

// ═══════════════════════════════════════════════════════════════════
// معالجة أحداث SDL — يُستدعى من app_update()
// ═══════════════════════════════════════════════════════════════════

// معالجة حدث ضغط الماوس
void processMouseDown(float x, float y, int button);

// معالجة حدث رفع الماوس
void processMouseUp(float x, float y, int button);

// معالجة حركة الماوس
void processMouseMove(float x, float y);

// معالجة عجلة التمرير
void processMouseWheel(float x, float y, float scrollAmount);

// تحديث الإيماءات كل إطار (للكشف عن ضغط طويل، سحب مستمر، حركة الزخم)
void updateGestures(double deltaMs);

// ═══════════════════════════════════════════════════════════════════
// فيزياء التمرير
// ═══════════════════════════════════════════════════════════════════

// تعيين فيزياء التمرير لودجت
void setScrollPhysics(int widgetId, ScrollPhysicsType type);

// تعيين فيزياء مخصصة
void setScrollPhysicsCustom(int widgetId, const ScrollPhysics& physics);

// بدء تمرير بالزخم (يُستدعى عند رفع الإصبع)
void startMomentumScroll(int widgetId, float velocity);

// إيقاف التمرير
void stopScroll(int widgetId);

// الحصول على حالة التمرير
ScrollState getScrollState(int widgetId);

// تعيين حجم المحتوى (يُحسب تلقائياً عادةً)
void setScrollContentSize(int widgetId, float size);

// ═══════════════════════════════════════════════════════════════════
// أدوات مساعدة
// ═══════════════════════════════════════════════════════════════════

// فحص ما إذا كانت نقطة (x,y) داخل ودجت
bool isPointInWidget(int widgetId, float x, float y);

// البحث عن الودجت الأعلى عند نقطة معيّنة
int findWidgetAt(float x, float y);

// تحويل إحداثيات من الشاشة إلى إحداثيات محلية لودجت
void screenToLocal(int widgetId, float screenX, float screenY,
                   float& localX, float& localY);

}} // namespace sad::gesture
