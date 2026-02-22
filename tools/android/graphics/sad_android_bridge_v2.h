/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  Android NativeActivity Bridge v2.0
 *  جسر NativeActivity المحسّن لتطبيقات لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *  
 *  يدعم وضعين:
 *    1) الوضع البسيط (Simple Mode): للتطبيقات التي تعمل في الخلفية فقط
 *    2) الوضع الرسومي (UI Mode): للتطبيقات التي تستخدم واجهة المستخدم الرسومية
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_ANDROID_BRIDGE_V2_H
#define SAD_ANDROID_BRIDGE_V2_H

#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  أوضاع التشغيل
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SAD_APP_MODE_SIMPLE,    // وضع بسيط - بدون واجهة رسومية
    SAD_APP_MODE_UI         // وضع واجهة المستخدم - كامل الرسوميات
} SadAppMode;

// ═══════════════════════════════════════════════════════════════════════════════
//  حالة التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct SadAndroidApp {
    // Android
    ANativeActivity* activity;
    ANativeWindow* window;
    AInputQueue* inputQueue;
    ALooper* looper;
    
    // الإعدادات
    SadAppMode mode;
    
    // الحالة
    int running;
    int paused;
    int focused;
    int destroyed;
    
    // كتل الأحداث
    void (*onStart)(struct SadAndroidApp* app);
    void (*onResume)(struct SadAndroidApp* app);
    void (*onPause)(struct SadAndroidApp* app);
    void (*onStop)(struct SadAndroidApp* app);
    void (*onDestroy)(struct SadAndroidApp* app);
    void (*onWindowCreated)(struct SadAndroidApp* app);
    void (*onWindowDestroyed)(struct SadAndroidApp* app);
    void (*onInputEvent)(struct SadAndroidApp* app, AInputEvent* event);
    void (*onFrame)(struct SadAndroidApp* app, float deltaTime);
    
    // بيانات المستخدم
    void* userData;
    
} SadAndroidApp;

// ═══════════════════════════════════════════════════════════════════════════════
//  API الرئيسي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تعيين وضع التطبيق
 * يجب استدعاؤها قبل بدء الحلقة الرئيسية
 */
void sad_android_set_mode(SadAppMode mode);

/**
 * الحصول على مؤشر التطبيق الحالي
 */
SadAndroidApp* sad_android_get_app(void);

/**
 * الحصول على النافذة الأصلية
 */
ANativeWindow* sad_android_get_window(void);

/**
 * إنهاء التطبيق
 */
void sad_android_finish(void);

/**
 * الحصول على كثافة الشاشة (DPI)
 */
float sad_android_get_density(void);

/**
 * الحصول على مسارات مهمة
 */
const char* sad_android_get_internal_data_path(void);
const char* sad_android_get_external_data_path(void);

/**
 * إظهار/إخفاء لوحة المفاتيح
 */
void sad_android_show_keyboard(void);
void sad_android_hide_keyboard(void);

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال اللغة ص (يتم توفيرها من قبل الكود المترجم)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * نقطة الدخول الرئيسية (الوضع البسيط)
 */
extern void __sad_main(void);

/**
 * نقطة الدخول للوضع الرسومي (اختياري)
 * إذا لم يتم تعريفها، يستخدم __sad_main
 */
extern void __sad_ui_main(SadAndroidApp* app) __attribute__((weak));

/**
 * دالة التحديث للرسوميات (اختياري)
 * تُستدعى كل إطار
 */
extern void __sad_ui_update(float deltaTime) __attribute__((weak));

/**
 * دالة معالجة اللمس (اختياري)
 */
extern void __sad_ui_touch(int action, float x, float y) __attribute__((weak));

#ifdef __cplusplus
}
#endif

#endif // SAD_ANDROID_BRIDGE_V2_H
