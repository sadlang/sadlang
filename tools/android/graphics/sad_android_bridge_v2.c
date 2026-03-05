/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  Android NativeActivity Bridge v2.0 - Implementation
 *  تنفيذ جسر NativeActivity المحسّن لتطبيقات لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_android_bridge_v2.h"
#include "sad_ui_engine.h"
#include "sad_arabic_text.h"
#include "sad_gestures.h"

#include <android/looper.h>
#include <android/native_window_jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <jni.h>

#define LOG_TAG "SadApp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// ═══════════════════════════════════════════════════════════════════════════════
//  الحالة العامة
// ═══════════════════════════════════════════════════════════════════════════════

static SadAndroidApp g_app = {0};
static SadAppMode g_mode = SAD_APP_MODE_SIMPLE;
static pthread_t g_mainThread;
static int g_threadStarted = 0;
static SadGestureDetector* g_gestures = NULL;

// Pipe لالتقاط stdout
static int g_capturePipe[2];

// ═══════════════════════════════════════════════════════════════════════════════
//  التقاط stdout إلى logcat
// ═══════════════════════════════════════════════════════════════════════════════

static void* capture_thread_func(void* arg) {
    ssize_t readlen;
    char buf[512];
    while ((readlen = read(g_capturePipe[0], buf, sizeof(buf) - 1)) > 0) {
        buf[readlen] = '\0';
        // إزالة أسطر جديدة في النهاية
        while (readlen > 0 && (buf[readlen-1] == '\n' || buf[readlen-1] == '\r')) {
            buf[--readlen] = '\0';
        }
        if (readlen > 0) {
            LOGI("%s", buf);
        }
    }
    return NULL;
}

static void setup_stdout_capture(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);
    
    if (pipe(g_capturePipe) == 0) {
        dup2(g_capturePipe[1], STDOUT_FILENO);
        dup2(g_capturePipe[1], STDERR_FILENO);
        
        pthread_t captureThread;
        pthread_create(&captureThread, NULL, capture_thread_func, NULL);
        pthread_detach(captureThread);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  مساعدات الوقت
// ═══════════════════════════════════════════════════════════════════════════════

static double get_current_time(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  نظام الإيماءات
// ═══════════════════════════════════════════════════════════════════════════════

static void gesture_callback(const SadGestureInfo* info, void* userData) {
    const char* name = sadgest_type_name_ar(info->type);
    LOGI("إيماءة: %s (x=%.0f, y=%.0f)", name, info->x, info->y);
}

static void init_gestures(void) {
    if (g_gestures) return;
    g_gestures = sadgest_create();
    if (g_gestures) {
        // تفعيل callback عام لجميع الإيماءات
        sadgest_set_callback(g_gestures, gesture_callback, NULL);
        LOGI("Gesture system initialized");
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الحلقة الرئيسية - الوضع البسيط
// ═══════════════════════════════════════════════════════════════════════════════

static void run_simple_mode(void) {
    LOGI("══════════════════════════════════════");
    LOGI("  لغة ص - تطبيق أندرويد (الوضع البسيط)");
    LOGI("  Sad Language Android App (Simple Mode)");
    LOGI("══════════════════════════════════════");
    
    // تهيئة EGL للعرض الأساسي
    if (g_app.window && sadui_init(g_app.window)) {
        // رسم خلفية زرقاء
        sadui_set_screen_size(ANativeWindow_getWidth(g_app.window), 
                              ANativeWindow_getHeight(g_app.window));
    }
    
    // تشغيل البرنامج الرئيسي
    LOGI("Running program...");
    __sad_main();
    
    LOGI("══════════════════════════════════════");
    LOGI("  البرنامج انتهى بنجاح!");
    LOGI("  Program completed successfully!");
    LOGI("══════════════════════════════════════");
    
    // الانتظار مع إبقاء التطبيق مفتوحاً
    while (g_app.running && g_app.window) {
        // رسم خلفية خضراء للإشارة إلى الاكتمال
        if (g_app.window) {
            // Simple green screen
            usleep(32000); // ~30 FPS
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الحلقة الرئيسية - وضع الواجهة الرسومية
// ═══════════════════════════════════════════════════════════════════════════════

static void run_ui_mode(void) {
    LOGI("══════════════════════════════════════");
    LOGI("  لغة ص - تطبيق رسومي");
    LOGI("  Sad Language UI App");
    LOGI("══════════════════════════════════════");
    
    // انتظار النافذة
    while (!g_app.window && g_app.running) {
        usleep(50000);
    }
    
    if (!g_app.window || !g_app.running) return;
    
    // تهيئة محرك الرسوم
    if (!sadui_init(g_app.window)) {
        LOGE("Failed to initialize graphics engine");
        return;
    }
    
    int screenWidth, screenHeight;
    sadui_get_screen_size(&screenWidth, &screenHeight);
    LOGI("Screen: %dx%d", screenWidth, screenHeight);
    
    // حساب كثافة الشاشة (نسبة تقريبية بناء على عرض الشاشة)
    // 1080p → density ≈ 2.625, 720p → 2.0, 1440p → 3.5
    float density = screenWidth / 411.0f;  // 411 dp هو العرض الأساسي
    if (density < 1.0f) density = 1.0f;
    LOGI("Screen density: %.2f", density);
    
    // تهيئة نظام النصوص وتحميل خط نظامي
    if (sad_font_init()) {
        sad_text_set_screen_size(screenWidth, screenHeight);
        
        // حجم الخط بالبكسل يأخذ الكثافة بالاعتبار
        float baseFontSize = 48.0f * density;
        
        // محاولة تحميل خطوط نظامية من Android
        static const char* fontPaths[] = {
            "/system/fonts/NotoSansArabic-Regular.ttf",
            "/system/fonts/NotoNaskhArabic-Regular.ttf",
            "/system/fonts/DroidSansArabic.ttf",
            "/system/fonts/NotoSans-Regular.ttf",
            "/system/fonts/Roboto-Regular.ttf",
            "/system/fonts/DroidSans.ttf",
            NULL
        };
        
        SadFontHandle loadedFont = 0;
        for (int i = 0; fontPaths[i]; i++) {
            loadedFont = sad_font_load(fontPaths[i], baseFontSize);
            if (loadedFont) {
                LOGI("Font loaded: %s at size %.0f", fontPaths[i], baseFontSize);
                sad_font_set_default(loadedFont);
                break;
            }
        }
        if (!loadedFont) {
            LOGE("No system font found!");
        }
    }
    
    // تهيئة نظام الإيماءات
    init_gestures();
    
    // استدعاء دالة إعداد الواجهة من كود المستخدم
    if (__sad_ui_main) {
        LOGI("Calling __sad_ui_main...");
        __sad_ui_main(&g_app);
    } else {
        // إنشاء واجهة افتراضية بسيطة
        LOGI("No __sad_ui_main found, creating default UI");
        
        // إنشاء حاوية رئيسية
        SadWidgetId root = sadui_column();
        sadui_set_root(root);
        
        SadLayout layout = {0};
        layout.width = (SadSize){SAD_SIZE_FILL, 0};
        layout.height = (SadSize){SAD_SIZE_FILL, 0};
        layout.mainAxis = SAD_ALIGN_START; // start from top
        layout.crossAxis = SAD_ALIGN_CENTER;
        layout.padding = sadui_edges(32);
        layout.gap = 16;
        sadui_set_layout(root, &layout);

        // إضافة عنوان كبير
        SadWidgetId title = sadui_heading("استعراض 40+ مكون لغة ص");
        sadui_add_child(root, title);

        // صندوق قابل للتمرير لتجربة كل المكونات
        SadWidgetId scrollArea = sadui_create(SAD_WIDGET_SCROLL);
        sadui_add_child(root, scrollArea);

        SadLayout scrollLayout = {0};
        scrollLayout.width = (SadSize){SAD_SIZE_PERCENT, 100};
        scrollLayout.height = (SadSize){SAD_SIZE_FILL, 0};
        sadui_set_layout(scrollArea, &scrollLayout);

        // اختبار المكونات المتقدمة الـ 40+ (محاكاة إضافة على واجهة المستخدم)
        SadWidgetType advanced_widgets[] = {
            SAD_WIDGET_VIDEO_PLAYER, SAD_WIDGET_MAP, SAD_WIDGET_DATE_PICKER,
            SAD_WIDGET_RATING_BAR, SAD_WIDGET_PIE_CHART, SAD_WIDGET_SIGNATURE_PAD,
            SAD_WIDGET_PULL_TO_REFRESH, SAD_WIDGET_STEPPER, SAD_WIDGET_ACCORDION,
            SAD_WIDGET_WEB_VIEW, SAD_WIDGET_QR_SCANNER, SAD_WIDGET_LOTTIE_ANIMATION
        };

        const char* widget_labels[] = {
            "فيديو تفاعلي", "خريطة الموقع", "اختر التاريخ",
            "نظام التقييم", "قرص الإحصائيات", "توقيع حر",
            "اسحب للتحديث", "خطوات التنفيذ", "قائمة طيات",
            "متصفح ويب", "قارئ باركود", "رسوم Lottie"
        };
        
        int n_widgets = sizeof(advanced_widgets) / sizeof(advanced_widgets[0]);
        for(int i = 0; i < n_widgets; i++) {
            SadWidgetId card = sadui_card();
            sadui_add_child(scrollArea, card);
            
            SadWidgetId w = sadui_create(advanced_widgets[i]);
            sadui_set_text(w, widget_labels[i]); // محاكاة العرض
            sadui_add_child(card, w);
        }

        // زر لتشغيل الحساسات
        SadWidgetId btn = sadui_button("تجربة حساسات الهاتف (التسارع)");
        sadui_add_child(root, btn);
        // معالجة الأحداث
        if (g_app.inputQueue) {
            AInputEvent* event = NULL;
            while (AInputQueue_getEvent(g_app.inputQueue, &event) >= 0) {
                if (AInputQueue_preDispatchEvent(g_app.inputQueue, event)) {
                    continue;
                }
                
                int handled = 0;
                
                // معالجة أحداث اللمس
                if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
                    int action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
                    float x = AMotionEvent_getX(event, 0);
                    float y = AMotionEvent_getY(event, 0);
                    
                    SadTouchEvent touchEvent = {0};
                    touchEvent.x = x;
                    touchEvent.y = y;
                    touchEvent.pointerId = AMotionEvent_getPointerId(event, 0);
                    
                    switch (action) {
                        case AMOTION_EVENT_ACTION_DOWN:
                            touchEvent.action = SAD_TOUCH_DOWN;
                            break;
                        case AMOTION_EVENT_ACTION_UP:
                            touchEvent.action = SAD_TOUCH_UP;
                            break;
                        case AMOTION_EVENT_ACTION_MOVE:
                            touchEvent.action = SAD_TOUCH_MOVE;
                            break;
                        case AMOTION_EVENT_ACTION_CANCEL:
                            touchEvent.action = SAD_TOUCH_CANCEL;
                            break;
                    }
                    
                    sadui_handle_touch(&touchEvent);
                    
                    // تمرير اللمس لنظام الإيماءات
                    if (g_gestures) {
                        SadTouchPoint tp = {0};
                        tp.id = touchEvent.pointerId;
                        tp.x = x;
                        tp.y = y;
                        tp.pressure = 1.0f;
                        struct timeval tv;
                        gettimeofday(&tv, NULL);
                        tp.timestamp = (int64_t)tv.tv_sec * 1000 + (int64_t)tv.tv_usec / 1000;
                        
                        int gestAction;
                        switch (action) {
                            case AMOTION_EVENT_ACTION_DOWN:  gestAction = 0; break;
                            case AMOTION_EVENT_ACTION_UP:    gestAction = 1; break;
                            case AMOTION_EVENT_ACTION_MOVE:  gestAction = 2; break;
                            default:                         gestAction = 3; break;
                        }
                        sadgest_process_touch(g_gestures, gestAction, &tp, 1, tp.timestamp);
                    }
                    
                    // استدعاء دالة المستخدم
                    if (__sad_ui_touch) {
                        __sad_ui_touch(action, x, y);
                    }
                    
                    handled = 1;
                }
                
                AInputQueue_finishEvent(g_app.inputQueue, event, handled);
            }
        }
        
        // حساب وقت الإطار
        double currentTime = get_current_time();
        float deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;
        
        // تحديث من المستخدم
        if (__sad_ui_update) {
            __sad_ui_update(deltaTime);
        }
        
        // رسم الإطار
        sadui_render(deltaTime);
        
        // تحديد معدل الإطارات (~60 FPS)
        usleep(16000);
    }
    
    // إغلاق المحرك
    if (g_gestures) {
        sadgest_destroy(g_gestures);
        g_gestures = NULL;
    }
    sadui_shutdown();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  خيط التشغيل الرئيسي
// ═══════════════════════════════════════════════════════════════════════════════

static void* main_thread_func(void* arg) {
    // انتظار النافذة
    while (!g_app.window && g_app.running) {
        usleep(50000);
    }
    
    if (!g_app.running) return NULL;
    
    // كشف الوضع تلقائياً: إذا عُرّفت __sad_ui_main نفعّل وضع الواجهة
    if (__sad_ui_main) {
        g_mode = SAD_APP_MODE_UI;
    }
    
    // تشغيل الوضع المناسب
    if (g_mode == SAD_APP_MODE_UI) {
        run_ui_mode();
    } else {
        run_simple_mode();
    }
    
    return NULL;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  callbacks حياة التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

static void onStart(ANativeActivity* activity) {
    LOGD("onStart");
    if (g_app.onStart) g_app.onStart(&g_app);
}

static void onResume(ANativeActivity* activity) {
    LOGD("onResume");
    g_app.paused = 0;
    if (g_app.onResume) g_app.onResume(&g_app);
}

static void onPause(ANativeActivity* activity) {
    LOGD("onPause");
    g_app.paused = 1;
    if (g_app.onPause) g_app.onPause(&g_app);
}

static void onStop(ANativeActivity* activity) {
    LOGD("onStop");
    if (g_app.onStop) g_app.onStop(&g_app);
}

static void onDestroy(ANativeActivity* activity) {
    LOGD("onDestroy");
    g_app.destroyed = 1;
    g_app.running = 0;
    
    if (g_app.onDestroy) g_app.onDestroy(&g_app);
    
    // انتظار انتهاء الخيط الرئيسي
    if (g_threadStarted) {
        pthread_join(g_mainThread, NULL);
        g_threadStarted = 0;
    }
}

static void onWindowFocusChanged(ANativeActivity* activity, int focused) {
    LOGD("onWindowFocusChanged: %d", focused);
    g_app.focused = focused;
}

static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
    LOGD("onNativeWindowCreated");
    g_app.window = window;
    
    if (g_app.onWindowCreated) g_app.onWindowCreated(&g_app);
    
    // بدء الخيط الرئيسي إذا لم يبدأ بعد
    if (!g_threadStarted) {
        g_threadStarted = 1;
        pthread_create(&g_mainThread, NULL, main_thread_func, NULL);
    }
}

static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {
    LOGD("onNativeWindowDestroyed");
    g_app.window = NULL;
    
    if (g_app.onWindowDestroyed) g_app.onWindowDestroyed(&g_app);
}

static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) {
    LOGD("onInputQueueCreated");
    g_app.inputQueue = queue;
}

static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) {
    LOGD("onInputQueueDestroyed");
    g_app.inputQueue = NULL;
}

static void onConfigurationChanged(ANativeActivity* activity) {
    LOGD("onConfigurationChanged");
}

static void onLowMemory(ANativeActivity* activity) {
    LOGD("onLowMemory");
}

static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen) {
    *outLen = 0;
    return NULL;
}

static void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window) {
    LOGD("onNativeWindowResized");
    if (window) {
        int w = ANativeWindow_getWidth(window);
        int h = ANativeWindow_getHeight(window);
        sadui_set_screen_size(w, h);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  نقطة دخول Android
// ═══════════════════════════════════════════════════════════════════════════════

__attribute__((visibility("default")))
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    LOGI("══════════════════════════════════════");
    LOGI("  ANativeActivity_onCreate");
    LOGI("  SDK Version: %d", activity->sdkVersion);
    LOGI("══════════════════════════════════════");
    
    // إعداد التقاط stdout
    setup_stdout_capture();
    
    // تهيئة حالة التطبيق
    memset(&g_app, 0, sizeof(g_app));
    g_app.activity = activity;
    g_app.running = 1;
    
    // تعيين callbacks
    activity->callbacks->onStart = onStart;
    activity->callbacks->onResume = onResume;
    activity->callbacks->onPause = onPause;
    activity->callbacks->onStop = onStop;
    activity->callbacks->onDestroy = onDestroy;
    activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated = onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;
    activity->callbacks->onConfigurationChanged = onConfigurationChanged;
    activity->callbacks->onLowMemory = onLowMemory;
    activity->callbacks->onSaveInstanceState = onSaveInstanceState;
    activity->callbacks->onNativeWindowResized = onNativeWindowResized;
    
    activity->instance = &g_app;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  API العام
// ═══════════════════════════════════════════════════════════════════════════════

void sad_android_set_mode(SadAppMode mode) {
    g_mode = mode;
}

SadAndroidApp* sad_android_get_app(void) {
    return &g_app;
}

ANativeWindow* sad_android_get_window(void) {
    return g_app.window;
}

void sad_android_finish(void) {
    if (g_app.activity) {
        ANativeActivity_finish(g_app.activity);
    }
}

float sad_android_get_density(void) {
    // TODO: الحصول على الكثافة من Android
    return 2.0f;  // افتراضي
}

const char* sad_android_get_internal_data_path(void) {
    return g_app.activity ? g_app.activity->internalDataPath : NULL;
}

const char* sad_android_get_external_data_path(void) {
    return g_app.activity ? g_app.activity->externalDataPath : NULL;
}

void sad_android_show_keyboard(void) {
    if (!g_app.activity) return;
    
    JNIEnv* env = g_app.activity->env;
    JavaVM* vm = g_app.activity->vm;
    
    (*vm)->AttachCurrentThread(vm, &env, NULL);
    
    jclass activityClass = (*env)->GetObjectClass(env, g_app.activity->clazz);
    jmethodID getSystemService = (*env)->GetMethodID(env, activityClass, "getSystemService", 
                                                      "(Ljava/lang/String;)Ljava/lang/Object;");
    
    jstring serviceName = (*env)->NewStringUTF(env, "input_method");
    jobject imm = (*env)->CallObjectMethod(env, g_app.activity->clazz, getSystemService, serviceName);
    
    if (imm) {
        jclass immClass = (*env)->GetObjectClass(env, imm);
        jmethodID showSoftInput = (*env)->GetMethodID(env, immClass, "toggleSoftInput", "(II)V");
        (*env)->CallVoidMethod(env, imm, showSoftInput, 2, 0);
    }
    
    (*env)->DeleteLocalRef(env, serviceName);
}

void sad_android_hide_keyboard(void) {
    if (!g_app.activity) return;
    
    JNIEnv* env = g_app.activity->env;
    JavaVM* vm = g_app.activity->vm;
    
    (*vm)->AttachCurrentThread(vm, &env, NULL);
    
    jclass activityClass = (*env)->GetObjectClass(env, g_app.activity->clazz);
    jmethodID getSystemService = (*env)->GetMethodID(env, activityClass, "getSystemService", 
                                                      "(Ljava/lang/String;)Ljava/lang/Object;");
    
    jstring serviceName = (*env)->NewStringUTF(env, "input_method");
    jobject imm = (*env)->CallObjectMethod(env, g_app.activity->clazz, getSystemService, serviceName);
    
    if (imm) {
        jclass immClass = (*env)->GetObjectClass(env, imm);
        jmethodID hideSoftInput = (*env)->GetMethodID(env, immClass, "hideSoftInputFromWindow", 
                                                       "(Landroid/os/IBinder;I)Z");
        
        jmethodID getWindowToken = (*env)->GetMethodID(env, activityClass, "getWindow", 
                                                        "()Landroid/view/Window;");
        jobject window = (*env)->CallObjectMethod(env, g_app.activity->clazz, getWindowToken);
        
        if (window) {
            jclass windowClass = (*env)->GetObjectClass(env, window);
            jmethodID getDecorView = (*env)->GetMethodID(env, windowClass, "getDecorView", 
                                                          "()Landroid/view/View;");
            jobject decorView = (*env)->CallObjectMethod(env, window, getDecorView);
            
            if (decorView) {
                jclass viewClass = (*env)->GetObjectClass(env, decorView);
                jmethodID getWindowTokenMethod = (*env)->GetMethodID(env, viewClass, 
                                                                      "getWindowToken", 
                                                                      "()Landroid/os/IBinder;");
                jobject token = (*env)->CallObjectMethod(env, decorView, getWindowTokenMethod);
                
                if (token) {
                    (*env)->CallBooleanMethod(env, imm, hideSoftInput, token, 0);
                }
            }
        }
    }
    
    (*env)->DeleteLocalRef(env, serviceName);
}
