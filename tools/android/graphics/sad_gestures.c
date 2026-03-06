/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ نظام التعرف على الإيماءات
 *  Gesture Recognition System Implementation
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_gestures.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SadGesture", __VA_ARGS__)
#else
#include <stdio.h>
#define LOGI(...) do { printf("[SadGesture] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  متتبع السرعة
// ═══════════════════════════════════════════════════════════════════════════════

#define VT_MAX_POINTS 20

struct SadVelocityTracker {
    struct {
        float x, y;
        int64_t timestamp;
    } points[VT_MAX_POINTS];
    int count;
    int head;
};

SadVelocityTracker* sadgest_velocity_tracker_create(void) {
    SadVelocityTracker* vt = calloc(1, sizeof(SadVelocityTracker));
    return vt;
}

void sadgest_velocity_tracker_destroy(SadVelocityTracker* tracker) {
    free(tracker);
}

void sadgest_velocity_add_point(SadVelocityTracker* tracker, float x, float y, int64_t timestamp) {
    if (!tracker) return;
    tracker->points[tracker->head].x = x;
    tracker->points[tracker->head].y = y;
    tracker->points[tracker->head].timestamp = timestamp;
    tracker->head = (tracker->head + 1) % VT_MAX_POINTS;
    if (tracker->count < VT_MAX_POINTS) tracker->count++;
}

SadVelocity sadgest_velocity_compute(SadVelocityTracker* tracker) {
    SadVelocity v = {0};
    if (!tracker || tracker->count < 2) return v;
    
    // استخدام آخر نقطتين لحساب السرعة
    int last = (tracker->head - 1 + VT_MAX_POINTS) % VT_MAX_POINTS;
    int prev = (tracker->head - 2 + VT_MAX_POINTS) % VT_MAX_POINTS;
    
    float dx = tracker->points[last].x - tracker->points[prev].x;
    float dy = tracker->points[last].y - tracker->points[prev].y;
    int64_t dt = tracker->points[last].timestamp - tracker->points[prev].timestamp;
    
    if (dt > 0) {
        v.vx = dx * 1000.0f / dt;  // بكسل/ثانية
        v.vy = dy * 1000.0f / dt;
        v.speed = sqrtf(v.vx * v.vx + v.vy * v.vy);
        v.direction = sadgest_direction(dx, dy);
    }
    
    return v;
}

void sadgest_velocity_reset(SadVelocityTracker* tracker) {
    if (!tracker) return;
    tracker->count = 0;
    tracker->head = 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  كاشف الإيماءات
// ═══════════════════════════════════════════════════════════════════════════════

#define MAX_GESTURE_CALLBACKS 32

typedef struct {
    SadGestureType type;
    SadGestureCallback callback;
    void* userData;
} GestureCallbackEntry;

struct SadGestureDetector {
    SadGestureConfig config;
    bool enabled;
    
    // رد اتصال عام
    SadGestureCallback globalCallback;
    void* globalUserData;
    
    // ردود اتصال خاصة بنوع الإيماءة
    GestureCallbackEntry callbacks[MAX_GESTURE_CALLBACKS];
    int callbackCount;
    
    // حالة اللمس
    SadTouchPoint currentPoints[SADGEST_MAX_TOUCH_POINTS];
    int currentPointCount;
    
    SadTouchPoint downPoint;   // نقطة الضغط الأولى
    int64_t downTime;          // وقت الضغط
    
    // تتبع النقرات
    int tapCount;
    int64_t lastTapTime;
    float lastTapX, lastTapY;
    
    // حالة السحب
    bool isDragging;
    float dragStartX, dragStartY;
    float dragLastX, dragLastY;
    
    // حالة التكبير/الدوران
    bool isPinching;
    float initialPinchDist;
    float initialPinchAngle;
    float pinchFocalX, pinchFocalY;
    
    // متتبع السرعة
    SadVelocityTracker* velocityTracker;
    
    // ضغط مطول
    bool longPressTriggered;
};

static void fire_gesture(SadGestureDetector* det, SadGestureInfo* info) {
    // ابحث عن رد اتصال خاص بالنوع
    for (int i = 0; i < det->callbackCount; i++) {
        if (det->callbacks[i].type == info->type && det->callbacks[i].callback) {
            det->callbacks[i].callback(info, det->callbacks[i].userData);
            return;
        }
    }
    // رد الاتصال العام
    if (det->globalCallback) {
        det->globalCallback(info, det->globalUserData);
    }
}

SadGestureDetector* sadgest_create(void) {
    SadGestureDetector* det = calloc(1, sizeof(SadGestureDetector));
    det->enabled = true;
    
    // إعدادات افتراضية
    det->config.tapTimeout = SADGEST_TAP_TIMEOUT_MS;
    det->config.longPressTimeout = SADGEST_LONG_PRESS_MS;
    det->config.doubleTapTimeout = SADGEST_DOUBLE_TAP_MS;
    det->config.swipeMinDistance = SADGEST_SWIPE_MIN_DISTANCE;
    det->config.swipeMinVelocity = SADGEST_SWIPE_MIN_VELOCITY;
    det->config.dragThreshold = 10.0f;
    det->config.edgeWidth = 30.0f;
    det->config.enableTap = true;
    det->config.enableDoubleTap = true;
    det->config.enableLongPress = true;
    det->config.enableSwipe = true;
    det->config.enableDrag = true;
    det->config.enablePinch = true;
    det->config.enableRotation = true;
    det->config.enableEdgeGestures = true;
    
    det->velocityTracker = sadgest_velocity_tracker_create();
    
    return det;
}

void sadgest_destroy(SadGestureDetector* detector) {
    if (!detector) return;
    if (detector->velocityTracker) {
        sadgest_velocity_tracker_destroy(detector->velocityTracker);
    }
    free(detector);
}

void sadgest_configure(SadGestureDetector* detector, const SadGestureConfig* config) {
    if (!detector || !config) return;
    detector->config = *config;
}

void sadgest_set_callback(SadGestureDetector* detector, SadGestureCallback callback, void* userData) {
    if (!detector) return;
    detector->globalCallback = callback;
    detector->globalUserData = userData;
}

void sadgest_set_gesture_callback(SadGestureDetector* detector, SadGestureType type,
                                   SadGestureCallback callback, void* userData) {
    if (!detector || detector->callbackCount >= MAX_GESTURE_CALLBACKS) return;
    
    // تحقق هل موجود
    for (int i = 0; i < detector->callbackCount; i++) {
        if (detector->callbacks[i].type == type) {
            detector->callbacks[i].callback = callback;
            detector->callbacks[i].userData = userData;
            return;
        }
    }
    
    GestureCallbackEntry* entry = &detector->callbacks[detector->callbackCount++];
    entry->type = type;
    entry->callback = callback;
    entry->userData = userData;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  معالجة أحداث اللمس
// ═══════════════════════════════════════════════════════════════════════════════

static void handle_single_touch_down(SadGestureDetector* det, const SadTouchPoint* point, int64_t ts) {
    det->downPoint = *point;
    det->downTime = ts;
    det->isDragging = false;
    det->longPressTriggered = false;
    
    sadgest_velocity_reset(det->velocityTracker);
    sadgest_velocity_add_point(det->velocityTracker, point->x, point->y, ts);
    
    det->dragStartX = point->x;
    det->dragStartY = point->y;
    det->dragLastX = point->x;
    det->dragLastY = point->y;
}

static void handle_single_touch_move(SadGestureDetector* det, const SadTouchPoint* point, int64_t ts) {
    sadgest_velocity_add_point(det->velocityTracker, point->x, point->y, ts);
    
    float dx = point->x - det->downPoint.x;
    float dy = point->y - det->downPoint.y;
    float dist = sqrtf(dx * dx + dy * dy);
    
    // كشف بدء السحب
    if (!det->isDragging && det->config.enableDrag && dist > det->config.dragThreshold) {
        det->isDragging = true;
        
        SadGestureInfo info = {0};
        info.type = SADGEST_DRAG_START;
        info.state = SADGEST_STATE_BEGAN;
        info.x = point->x;
        info.y = point->y;
        info.startX = det->downPoint.x;
        info.startY = det->downPoint.y;
        info.touchCount = 1;
        info.timestamp = ts;
        fire_gesture(det, &info);
    }
    
    // تحديث السحب
    if (det->isDragging) {
        SadGestureInfo info = {0};
        info.type = SADGEST_DRAG_UPDATE;
        info.state = SADGEST_STATE_CHANGED;
        info.x = point->x;
        info.y = point->y;
        info.startX = det->downPoint.x;
        info.startY = det->downPoint.y;
        info.deltaX = point->x - det->dragLastX;
        info.deltaY = point->y - det->dragLastY;
        info.totalDeltaX = dx;
        info.totalDeltaY = dy;
        info.touchCount = 1;
        info.timestamp = ts;
        fire_gesture(det, &info);
        
        det->dragLastX = point->x;
        det->dragLastY = point->y;
    }
    
    // كشف الضغط المطول
    if (!det->longPressTriggered && det->config.enableLongPress && 
        dist < det->config.dragThreshold && 
        (ts - det->downTime) >= det->config.longPressTimeout) {
        det->longPressTriggered = true;
        
        SadGestureInfo info = {0};
        info.type = SADGEST_LONG_PRESS;
        info.state = SADGEST_STATE_BEGAN;
        info.x = point->x;
        info.y = point->y;
        info.startX = det->downPoint.x;
        info.startY = det->downPoint.y;
        info.touchCount = 1;
        info.timestamp = ts;
        info.duration = ts - det->downTime;
        fire_gesture(det, &info);
    }
}

static void handle_single_touch_up(SadGestureDetector* det, const SadTouchPoint* point, int64_t ts) {
    sadgest_velocity_add_point(det->velocityTracker, point->x, point->y, ts);
    
    float dx = point->x - det->downPoint.x;
    float dy = point->y - det->downPoint.y;
    float dist = sqrtf(dx * dx + dy * dy);
    int64_t duration = ts - det->downTime;
    
    SadVelocity vel = sadgest_velocity_compute(det->velocityTracker);
    
    if (det->isDragging) {
        // نهاية السحب
        SadGestureInfo info = {0};
        info.type = SADGEST_DRAG_END;
        info.state = SADGEST_STATE_ENDED;
        info.x = point->x;
        info.y = point->y;
        info.startX = det->downPoint.x;
        info.startY = det->downPoint.y;
        info.totalDeltaX = dx;
        info.totalDeltaY = dy;
        info.velocity = vel;
        info.touchCount = 1;
        info.timestamp = ts;
        info.duration = duration;
        fire_gesture(det, &info);
        
        // كشف القذف (fling)
        if (vel.speed > det->config.swipeMinVelocity) {
            SadGestureInfo flingInfo = {0};
            flingInfo.type = SADGEST_FLING;
            flingInfo.state = SADGEST_STATE_ENDED;
            flingInfo.x = point->x;
            flingInfo.y = point->y;
            flingInfo.startX = det->downPoint.x;
            flingInfo.startY = det->downPoint.y;
            flingInfo.totalDeltaX = dx;
            flingInfo.totalDeltaY = dy;
            flingInfo.velocity = vel;
            flingInfo.touchCount = 1;
            flingInfo.timestamp = ts;
            flingInfo.duration = duration;
            fire_gesture(det, &flingInfo);
        }
        
        // كشف السحب (swipe)
        if (det->config.enableSwipe && dist >= det->config.swipeMinDistance &&
            vel.speed >= det->config.swipeMinVelocity) {
            SadGestureType swipeType = SADGEST_NONE;
            SadGestureDirection dir = sadgest_direction(dx, dy);
            
            switch (dir) {
                case SADGEST_DIRECTION_LEFT:  swipeType = SADGEST_SWIPE_LEFT; break;
                case SADGEST_DIRECTION_RIGHT: swipeType = SADGEST_SWIPE_RIGHT; break;
                case SADGEST_DIRECTION_UP:    swipeType = SADGEST_SWIPE_UP; break;
                case SADGEST_DIRECTION_DOWN:  swipeType = SADGEST_SWIPE_DOWN; break;
                default: break;
            }
            
            if (swipeType != SADGEST_NONE) {
                SadGestureInfo swipeInfo = {0};
                swipeInfo.type = swipeType;
                swipeInfo.state = SADGEST_STATE_ENDED;
                swipeInfo.x = point->x;
                swipeInfo.y = point->y;
                swipeInfo.startX = det->downPoint.x;
                swipeInfo.startY = det->downPoint.y;
                swipeInfo.totalDeltaX = dx;
                swipeInfo.totalDeltaY = dy;
                swipeInfo.velocity = vel;
                swipeInfo.touchCount = 1;
                swipeInfo.timestamp = ts;
                swipeInfo.duration = duration;
                fire_gesture(det, &swipeInfo);
            }
        }
        
        det->isDragging = false;
    } else if (!det->longPressTriggered && duration < det->config.tapTimeout &&
               dist < det->config.dragThreshold) {
        // نقرة!
        if (det->config.enableTap) {
            // تحقق من النقر المزدوج
            if (det->config.enableDoubleTap && det->tapCount > 0 && 
                (ts - det->lastTapTime) < det->config.doubleTapTimeout &&
                fabsf(point->x - det->lastTapX) < 30 && 
                fabsf(point->y - det->lastTapY) < 30) {
                det->tapCount++;
                
                SadGestureType tapType = (det->tapCount >= 3) ? SADGEST_TRIPLE_TAP : SADGEST_DOUBLE_TAP;
                SadGestureInfo info = {0};
                info.type = tapType;
                info.state = SADGEST_STATE_ENDED;
                info.x = point->x;
                info.y = point->y;
                info.touchCount = 1;
                info.timestamp = ts;
                fire_gesture(det, &info);
                
                if (det->tapCount >= 3) det->tapCount = 0;
            } else {
                det->tapCount = 1;
                
                SadGestureInfo info = {0};
                info.type = SADGEST_TAP;
                info.state = SADGEST_STATE_ENDED;
                info.x = point->x;
                info.y = point->y;
                info.touchCount = 1;
                info.timestamp = ts;
                fire_gesture(det, &info);
            }
            
            det->lastTapTime = ts;
            det->lastTapX = point->x;
            det->lastTapY = point->y;
        }
    }
    
    if (det->longPressTriggered) {
        SadGestureInfo info = {0};
        info.type = SADGEST_LONG_PRESS_UP;
        info.state = SADGEST_STATE_ENDED;
        info.x = point->x;
        info.y = point->y;
        info.touchCount = 1;
        info.timestamp = ts;
        info.duration = duration;
        fire_gesture(det, &info);
    }
}

static void handle_multi_touch(SadGestureDetector* det, const SadTouchPoint* points, int count, int64_t ts) {
    if (count < 2) return;
    
    float x1 = points[0].x, y1 = points[0].y;
    float x2 = points[1].x, y2 = points[1].y;
    float dist = sadgest_distance(x1, y1, x2, y2);
    float angle = sadgest_angle(x1, y1, x2, y2);
    float fx = (x1 + x2) / 2.0f;
    float fy = (y1 + y2) / 2.0f;
    
    if (!det->isPinching) {
        det->isPinching = true;
        det->initialPinchDist = dist;
        det->initialPinchAngle = angle;
        det->pinchFocalX = fx;
        det->pinchFocalY = fy;
        
        if (det->config.enablePinch) {
            SadGestureInfo info = {0};
            info.type = SADGEST_PINCH_START;
            info.state = SADGEST_STATE_BEGAN;
            info.scale = 1.0f;
            info.focalX = fx;
            info.focalY = fy;
            info.touchCount = count;
            info.timestamp = ts;
            fire_gesture(det, &info);
        }
        
        if (det->config.enableRotation) {
            SadGestureInfo info = {0};
            info.type = SADGEST_ROTATE_START;
            info.state = SADGEST_STATE_BEGAN;
            info.rotation = 0.0f;
            info.focalX = fx;
            info.focalY = fy;
            info.touchCount = count;
            info.timestamp = ts;
            fire_gesture(det, &info);
        }
    } else {
        float scale = det->initialPinchDist > 0 ? dist / det->initialPinchDist : 1.0f;
        float rotation = angle - det->initialPinchAngle;
        
        if (det->config.enablePinch) {
            SadGestureInfo info = {0};
            info.type = SADGEST_PINCH_UPDATE;
            info.state = SADGEST_STATE_CHANGED;
            info.scale = scale;
            info.focalX = fx;
            info.focalY = fy;
            info.touchCount = count;
            info.timestamp = ts;
            fire_gesture(det, &info);
        }
        
        if (det->config.enableRotation) {
            SadGestureInfo info = {0};
            info.type = SADGEST_ROTATE_UPDATE;
            info.state = SADGEST_STATE_CHANGED;
            info.rotation = rotation;
            info.focalX = fx;
            info.focalY = fy;
            info.touchCount = count;
            info.timestamp = ts;
            fire_gesture(det, &info);
        }
    }
}

void sadgest_process_touch(SadGestureDetector* detector, int action,
                           const SadTouchPoint* points, int pointCount, int64_t timestamp) {
    if (!detector || !detector->enabled || !points || pointCount <= 0) return;
    
    // تحديث النقاط الحالية
    detector->currentPointCount = pointCount < SADGEST_MAX_TOUCH_POINTS ? 
                                  pointCount : SADGEST_MAX_TOUCH_POINTS;
    memcpy(detector->currentPoints, points, 
           detector->currentPointCount * sizeof(SadTouchPoint));
    
    if (pointCount >= 2) {
        // معالجة اللمس المتعدد
        handle_multi_touch(detector, points, pointCount, timestamp);
    } else {
        switch (action) {
            case 0: // DOWN
                handle_single_touch_down(detector, &points[0], timestamp);
                break;
            case 1: // UP
                handle_single_touch_up(detector, &points[0], timestamp);
                break;
            case 2: // MOVE
                handle_single_touch_move(detector, &points[0], timestamp);
                break;
            case 3: // CANCEL
                if (detector->isDragging) {
                    SadGestureInfo info = {0};
                    info.type = SADGEST_DRAG_CANCEL;
                    info.state = SADGEST_STATE_CANCELLED;
                    info.x = points[0].x;
                    info.y = points[0].y;
                    info.touchCount = 1;
                    info.timestamp = timestamp;
                    fire_gesture(detector, &info);
                    detector->isDragging = false;
                }
                if (detector->isPinching) {
                    detector->isPinching = false;
                }
                break;
        }
    }
}

void sadgest_reset(SadGestureDetector* detector) {
    if (!detector) return;
    detector->isDragging = false;
    detector->isPinching = false;
    detector->longPressTriggered = false;
    detector->tapCount = 0;
    detector->currentPointCount = 0;
    sadgest_velocity_reset(detector->velocityTracker);
}

void sadgest_set_enabled(SadGestureDetector* detector, bool enabled) {
    if (!detector) return;
    detector->enabled = enabled;
}

bool sadgest_is_enabled(SadGestureDetector* detector) {
    return detector ? detector->enabled : false;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

float sadgest_distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

float sadgest_angle(float x1, float y1, float x2, float y2) {
    return atan2f(y2 - y1, x2 - x1);
}

SadGestureDirection sadgest_direction(float dx, float dy) {
    if (fabsf(dx) < 1.0f && fabsf(dy) < 1.0f) return SADGEST_DIRECTION_NONE;
    
    float angle = atan2f(dy, dx) * 180.0f / 3.14159265f;
    
    if (angle >= -22.5f && angle < 22.5f) return SADGEST_DIRECTION_RIGHT;
    if (angle >= 22.5f && angle < 67.5f) return SADGEST_DIRECTION_RIGHT_DOWN;
    if (angle >= 67.5f && angle < 112.5f) return SADGEST_DIRECTION_DOWN;
    if (angle >= 112.5f && angle < 157.5f) return SADGEST_DIRECTION_LEFT_DOWN;
    if (angle >= -67.5f && angle < -22.5f) return SADGEST_DIRECTION_RIGHT_UP;
    if (angle >= -112.5f && angle < -67.5f) return SADGEST_DIRECTION_UP;
    if (angle >= -157.5f && angle < -112.5f) return SADGEST_DIRECTION_LEFT_UP;
    
    return SADGEST_DIRECTION_LEFT;
}

const char* sadgest_type_name(SadGestureType type) {
    switch (type) {
        case SADGEST_TAP: return "Tap";
        case SADGEST_DOUBLE_TAP: return "Double Tap";
        case SADGEST_TRIPLE_TAP: return "Triple Tap";
        case SADGEST_LONG_PRESS: return "Long Press";
        case SADGEST_LONG_PRESS_UP: return "Long Press Up";
        case SADGEST_SWIPE_LEFT: return "Swipe Left";
        case SADGEST_SWIPE_RIGHT: return "Swipe Right";
        case SADGEST_SWIPE_UP: return "Swipe Up";
        case SADGEST_SWIPE_DOWN: return "Swipe Down";
        case SADGEST_DRAG_START: return "Drag Start";
        case SADGEST_DRAG_UPDATE: return "Drag Update";
        case SADGEST_DRAG_END: return "Drag End";
        case SADGEST_DRAG_CANCEL: return "Drag Cancel";
        case SADGEST_FLING: return "Fling";
        case SADGEST_PINCH_START: return "Pinch Start";
        case SADGEST_PINCH_UPDATE: return "Pinch Update";
        case SADGEST_PINCH_END: return "Pinch End";
        case SADGEST_ROTATE_START: return "Rotate Start";
        case SADGEST_ROTATE_UPDATE: return "Rotate Update";
        case SADGEST_ROTATE_END: return "Rotate End";
        case SADGEST_TWO_FINGER_TAP: return "Two Finger Tap";
        case SADGEST_THREE_FINGER_TAP: return "Three Finger Tap";
        case SADGEST_EDGE_SWIPE_LEFT: return "Edge Swipe Left";
        case SADGEST_EDGE_SWIPE_RIGHT: return "Edge Swipe Right";
        case SADGEST_PULL_TO_REFRESH: return "Pull to Refresh";
        default: return "Unknown";
    }
}

const char* sadgest_type_name_ar(SadGestureType type) {
    switch (type) {
        case SADGEST_TAP: return "نقرة";
        case SADGEST_DOUBLE_TAP: return "نقرة مزدوجة";
        case SADGEST_TRIPLE_TAP: return "ثلاث نقرات";
        case SADGEST_LONG_PRESS: return "ضغط مطول";
        case SADGEST_LONG_PRESS_UP: return "رفع بعد ضغط مطول";
        case SADGEST_SWIPE_LEFT: return "سحب يسار";
        case SADGEST_SWIPE_RIGHT: return "سحب يمين";
        case SADGEST_SWIPE_UP: return "سحب أعلى";
        case SADGEST_SWIPE_DOWN: return "سحب أسفل";
        case SADGEST_DRAG_START: return "بدء السحب";
        case SADGEST_DRAG_UPDATE: return "تحديث السحب";
        case SADGEST_DRAG_END: return "نهاية السحب";
        case SADGEST_DRAG_CANCEL: return "إلغاء السحب";
        case SADGEST_FLING: return "قذف";
        case SADGEST_PINCH_START: return "بدء القرص";
        case SADGEST_PINCH_UPDATE: return "تحديث القرص";
        case SADGEST_PINCH_END: return "نهاية القرص";
        case SADGEST_ROTATE_START: return "بدء الدوران";
        case SADGEST_ROTATE_UPDATE: return "تحديث الدوران";
        case SADGEST_ROTATE_END: return "نهاية الدوران";
        case SADGEST_TWO_FINGER_TAP: return "نقر بإصبعين";
        case SADGEST_THREE_FINGER_TAP: return "نقر بثلاثة أصابع";
        case SADGEST_EDGE_SWIPE_LEFT: return "سحب من الحافة اليسرى";
        case SADGEST_EDGE_SWIPE_RIGHT: return "سحب من الحافة اليمنى";
        case SADGEST_PULL_TO_REFRESH: return "سحب للتحديث";
        default: return "غير معروف";
    }
}
