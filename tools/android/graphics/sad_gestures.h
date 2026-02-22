/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  نظام التعرف على الإيماءات المتقدم
 *  Advanced Gesture Recognition System
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 *  يدعم:
 *  - إيماءات اللمس الأساسية (Tap, Long Press, Double Tap)
 *  - إيماءات السحب (Swipe, Drag, Fling)
 *  - إيماءات التكبير (Pinch to Zoom)
 *  - إيماءات الدوران (Rotate)
 *  - إيماءات مخصصة للتطبيقات البنكية
 * 
 *  الإصدار: 2.0
 *  المؤلف: فريق تطوير لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_GESTURES_H
#define SAD_GESTURES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  الثوابت
// ═══════════════════════════════════════════════════════════════════════════════

#define SADGEST_MAX_TOUCH_POINTS    10
#define SADGEST_MAX_HISTORY         50
#define SADGEST_TAP_TIMEOUT_MS      300
#define SADGEST_LONG_PRESS_MS       500
#define SADGEST_DOUBLE_TAP_MS       300
#define SADGEST_SWIPE_MIN_DISTANCE  50.0f
#define SADGEST_SWIPE_MIN_VELOCITY  200.0f

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع الإيماءات
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    // إيماءات النقر
    SADGEST_TAP,                // نقرة واحدة
    SADGEST_DOUBLE_TAP,         // نقرة مزدوجة
    SADGEST_TRIPLE_TAP,         // ثلاث نقرات
    SADGEST_LONG_PRESS,         // ضغط مطول
    SADGEST_LONG_PRESS_UP,      // رفع بعد ضغط مطول
    
    // إيماءات السحب
    SADGEST_SWIPE_LEFT,         // سحب لليسار
    SADGEST_SWIPE_RIGHT,        // سحب لليمين
    SADGEST_SWIPE_UP,           // سحب للأعلى
    SADGEST_SWIPE_DOWN,         // سحب للأسفل
    
    // إيماءات السحب المستمر
    SADGEST_DRAG_START,         // بداية السحب
    SADGEST_DRAG_UPDATE,        // تحديث السحب
    SADGEST_DRAG_END,           // نهاية السحب
    SADGEST_DRAG_CANCEL,        // إلغاء السحب
    
    // إيماءات القذف
    SADGEST_FLING,              // قذف
    
    // إيماءات متعددة الأصابع
    SADGEST_PINCH_START,        // بداية القرص
    SADGEST_PINCH_UPDATE,       // تحديث القرص
    SADGEST_PINCH_END,          // نهاية القرص
    
    SADGEST_ROTATE_START,       // بداية الدوران
    SADGEST_ROTATE_UPDATE,      // تحديث الدوران
    SADGEST_ROTATE_END,         // نهاية الدوران
    
    // إيماءات خاصة
    SADGEST_TWO_FINGER_TAP,     // نقر بإصبعين
    SADGEST_THREE_FINGER_TAP,   // نقر بثلاثة أصابع
    SADGEST_EDGE_SWIPE_LEFT,    // سحب من الحافة اليسرى
    SADGEST_EDGE_SWIPE_RIGHT,   // سحب من الحافة اليمنى
    SADGEST_PULL_TO_REFRESH,    // سحب للتحديث
    
    SADGEST_NONE                // لا إيماءة
} SadGestureType;

typedef enum {
    SADGEST_DIRECTION_NONE,
    SADGEST_DIRECTION_LEFT,
    SADGEST_DIRECTION_RIGHT,
    SADGEST_DIRECTION_UP,
    SADGEST_DIRECTION_DOWN,
    SADGEST_DIRECTION_LEFT_UP,
    SADGEST_DIRECTION_LEFT_DOWN,
    SADGEST_DIRECTION_RIGHT_UP,
    SADGEST_DIRECTION_RIGHT_DOWN
} SadGestureDirection;

typedef enum {
    SADGEST_STATE_POSSIBLE,     // محتمل
    SADGEST_STATE_BEGAN,        // بدأ
    SADGEST_STATE_CHANGED,      // تغير
    SADGEST_STATE_ENDED,        // انتهى
    SADGEST_STATE_CANCELLED,    // ملغى
    SADGEST_STATE_FAILED        // فشل
} SadGestureState;

// ═══════════════════════════════════════════════════════════════════════════════
//  هياكل البيانات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * نقطة لمس
 */
typedef struct {
    int id;                     // معرّف الإصبع
    float x, y;                 // الموقع
    float pressure;             // الضغط
    float size;                 // حجم اللمسة
    int64_t timestamp;          // الوقت بالميلي ثانية
} SadTouchPoint;

/**
 * سجل حركة اللمس
 */
typedef struct {
    SadTouchPoint points[SADGEST_MAX_HISTORY];
    int count;
    int startIndex;
} SadTouchHistory;

/**
 * معلومات السرعة
 */
typedef struct {
    float vx;                   // السرعة الأفقية
    float vy;                   // السرعة العمودية
    float speed;                // السرعة الإجمالية
    SadGestureDirection direction; // الاتجاه
} SadVelocity;

/**
 * معلومات الإيماءة
 */
typedef struct {
    SadGestureType type;
    SadGestureState state;
    
    // موقع الإيماءة
    float x, y;                 // الموقع الحالي
    float startX, startY;       // موقع البداية
    
    // حركة الإيماءة
    float deltaX, deltaY;       // الإزاحة
    float totalDeltaX, totalDeltaY; // إجمالي الإزاحة
    
    // للتكبير/الدوران
    float scale;                // مقياس التكبير (1.0 = أصلي)
    float rotation;             // زاوية الدوران بالراديان
    float focalX, focalY;       // نقطة المركز
    
    // السرعة
    SadVelocity velocity;
    
    // معلومات إضافية
    int touchCount;             // عدد نقاط اللمس
    int64_t timestamp;          // الوقت
    int64_t duration;           // المدة بالميلي ثانية
    
    // بيانات المستخدم
    void* userData;
} SadGestureInfo;

/**
 * إعدادات الكاشف
 */
typedef struct {
    // حدود النقر
    int tapTimeout;             // مهلة النقر (ms)
    int longPressTimeout;       // مهلة الضغط المطول (ms)
    int doubleTapTimeout;       // مهلة النقر المزدوج (ms)
    
    // حدود السحب
    float swipeMinDistance;     // أدنى مسافة للسحب
    float swipeMinVelocity;     // أدنى سرعة للسحب
    float dragThreshold;        // عتبة بدء السحب
    
    // حدود الحواف
    float edgeWidth;            // عرض منطقة الحافة
    
    // تفعيل الإيماءات
    bool enableTap;
    bool enableDoubleTap;
    bool enableLongPress;
    bool enableSwipe;
    bool enableDrag;
    bool enablePinch;
    bool enableRotation;
    bool enableEdgeGestures;
    
} SadGestureConfig;

// معالج الإيماءات
typedef void (*SadGestureCallback)(const SadGestureInfo* gesture, void* userData);

// ═══════════════════════════════════════════════════════════════════════════════
//  كاشف الإيماءات
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct SadGestureDetector SadGestureDetector;

/**
 * إنشاء كاشف إيماءات جديد
 * @return مؤشر للكاشف
 */
SadGestureDetector* sadgest_create(void);

/**
 * تدمير كاشف الإيماءات
 */
void sadgest_destroy(SadGestureDetector* detector);

/**
 * تهيئة الكاشف بإعدادات مخصصة
 */
void sadgest_configure(SadGestureDetector* detector, const SadGestureConfig* config);

/**
 * تعيين معالج الإيماءات
 */
void sadgest_set_callback(
    SadGestureDetector* detector,
    SadGestureCallback callback,
    void* userData
);

/**
 * تعيين معالج لنوع إيماءة محدد
 */
void sadgest_set_gesture_callback(
    SadGestureDetector* detector,
    SadGestureType type,
    SadGestureCallback callback,
    void* userData
);

// ═══════════════════════════════════════════════════════════════════════════════
//  معالجة أحداث اللمس
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * معالجة حدث لمس جديد
 * @param detector الكاشف
 * @param action الإجراء (0=down, 1=up, 2=move, 3=cancel)
 * @param points نقاط اللمس
 * @param pointCount عدد النقاط
 * @param timestamp الوقت بالميلي ثانية
 */
void sadgest_process_touch(
    SadGestureDetector* detector,
    int action,
    const SadTouchPoint* points,
    int pointCount,
    int64_t timestamp
);

/**
 * إعادة تعيين الكاشف
 */
void sadgest_reset(SadGestureDetector* detector);

/**
 * تفعيل/تعطيل الكاشف
 */
void sadgest_set_enabled(SadGestureDetector* detector, bool enabled);

/**
 * هل الكاشف مفعّل
 */
bool sadgest_is_enabled(SadGestureDetector* detector);

// ═══════════════════════════════════════════════════════════════════════════════
//  حساب السرعة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * متتبع السرعة
 */
typedef struct SadVelocityTracker SadVelocityTracker;

/**
 * إنشاء متتبع سرعة
 */
SadVelocityTracker* sadgest_velocity_tracker_create(void);

/**
 * تدمير متتبع السرعة
 */
void sadgest_velocity_tracker_destroy(SadVelocityTracker* tracker);

/**
 * إضافة نقطة للتتبع
 */
void sadgest_velocity_add_point(
    SadVelocityTracker* tracker,
    float x, float y,
    int64_t timestamp
);

/**
 * حساب السرعة
 */
SadVelocity sadgest_velocity_compute(SadVelocityTracker* tracker);

/**
 * إعادة تعيين المتتبع
 */
void sadgest_velocity_reset(SadVelocityTracker* tracker);

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * حساب المسافة بين نقطتين
 */
float sadgest_distance(float x1, float y1, float x2, float y2);

/**
 * حساب الزاوية بين نقطتين
 */
float sadgest_angle(float x1, float y1, float x2, float y2);

/**
 * تحديد اتجاه الحركة
 */
SadGestureDirection sadgest_direction(float dx, float dy);

/**
 * اسم الإيماءة
 */
const char* sadgest_type_name(SadGestureType type);

/**
 * اسم الإيماءة بالعربية
 */
const char* sadgest_type_name_ar(SadGestureType type);

#ifdef __cplusplus
}
#endif

#endif // SAD_GESTURES_H
