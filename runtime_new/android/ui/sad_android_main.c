/**
 * ==========================================================================
 * sad_android_main.c - نقطة دخول التطبيق الأصلي لأندرويد
 * Android Native Application Entry Point
 * ==========================================================================
 * 
 * الوصف (عربي):
 * --------------
 * هذا الملف يحتوي على نقطة دخول تطبيق أندرويد الأصلي باستخدام NativeActivity.
 * لا يحتاج هذا التطبيق إلى Java أو Kotlin — كل شيء مكتوب بـ C.
 * 
 * البنية:
 * - android_main: نقطة الدخول الرئيسية التي يستدعيها Android NDK
 * - معالجة دورة حياة التطبيق (onCreate, onResume, onPause, onDestroy)
 * - معالجة أحداث اللمس والنوافذ
 * - حلقة الأحداث الرئيسية
 * 
 * كيف يعمل:
 * ----------
 * 1. Android يُحمّل مكتبة .so ويستدعي ANativeActivity_onCreate
 * 2. نُسجّل دوال الاستدعاء لأحداث النافذة واللمس
 * 3. نُنشئ EGL context للرسم بـ OpenGL ES
 * 4. ندخل حلقة الأحداث الرئيسية
 * 5. في كل إطار: نُعالج الأحداث، نُحدّث، نرسم
 * 
 * Description (English):
 * ----------------------
 * This file contains the native Android app entry point using NativeActivity.
 * This app doesn't need Java or Kotlin — everything is written in C.
 * 
 * Structure:
 * - android_main: Main entry point called by Android NDK
 * - Application lifecycle handling (onCreate, onResume, onPause, onDestroy)
 * - Touch and window event handling
 * - Main event loop
 * 
 * ==========================================================================
 */

#ifdef __ANDROID__

#include <jni.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <poll.h>
#include <pthread.h>

#include <android/configuration.h>
#include <android/looper.h>
#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/log.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include "sad_android_ui.h"
#include "../sad_android_memory.h"

/* =========================================================================
 * ثوابت ومعرّفات / Constants and Identifiers
 * ========================================================================= */

#define SAD_LOG_TAG "SadMain"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, SAD_LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, SAD_LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, SAD_LOG_TAG, __VA_ARGS__)

/** معرّفات الأحداث في ALooper */
#define LOOPER_ID_MAIN      1
#define LOOPER_ID_INPUT     2


/* =========================================================================
 * حالة التطبيق / Application State
 * ========================================================================= */

/**
 * حالة التطبيق الأصلي
 * Native application state
 * 
 * يحتوي على كل المعلومات المطلوبة لتشغيل التطبيق:
 * - مراجع NativeActivity وANativeWindow
 * - سياق EGL للرسم
 * - حالة الحلقة الرئيسية
 */
typedef struct SadAndroidState {
    /* --- مراجع الأندرويد / Android references --- */
    ANativeActivity* activity;          /**< مرجع NativeActivity */
    ANativeWindow* window;              /**< مرجع نافذة الرسم */
    AConfiguration* config;             /**< إعدادات الجهاز */
    ALooper* looper;                    /**< حلقة الأحداث */
    AInputQueue* input_queue;           /**< طابور الإدخال */
    
    /* --- EGL / OpenGL ES --- */
    EGLDisplay egl_display;             /**< شاشة EGL */
    EGLSurface egl_surface;             /**< سطح الرسم */
    EGLContext egl_context;             /**< سياق OpenGL */
    EGLConfig egl_config;               /**< إعدادات EGL */
    
    /* --- أبعاد الشاشة / Screen dimensions --- */
    int32_t screen_width;               /**< عرض الشاشة */
    int32_t screen_height;              /**< ارتفاع الشاشة */
    float screen_density;               /**< كثافة الشاشة */
    
    /* --- حالة التطبيق / App state --- */
    bool is_running;                    /**< هل التطبيق يعمل؟ */
    bool is_focused;                    /**< هل التطبيق في المقدمة؟ */
    bool has_window;                    /**< هل النافذة جاهزة؟ */
    bool egl_initialized;               /**< هل EGL مُهيأ؟ */
    
    /* --- تطبيق لغة ص / Sad App --- */
    SadApp* sad_app;                    /**< تطبيق لغة ص */
    
    /* --- مزامنة الخيوط / Thread synchronization --- */
    pthread_mutex_t mutex;              /**< قفل المزامنة */
    pthread_cond_t cond;                /**< شرط الانتظار */
    
    /* --- التوقيت / Timing --- */
    int64_t last_frame_time;            /**< وقت آخر إطار */
    float delta_time;                   /**< الوقت منذ آخر إطار */
    
} SadAndroidState;

/* الحالة العامة / Global state */
static SadAndroidState g_state = {0};


/* =========================================================================
 * تهيئة EGL / EGL Initialization
 * ========================================================================= */

/**
 * تهيئة سياق EGL للرسم بـ OpenGL ES 3.0
 * Initialize EGL context for OpenGL ES 3.0 rendering
 * 
 * EGL هي واجهة بين OpenGL وشاشة الجهاز.
 * نحتاج إنشاء Display، Surface، وContext قبل الرسم.
 * 
 * @return true عند النجاح
 */
static bool init_egl(void) {
    if (!g_state.window) {
        LOGE("لا توجد نافذة للتهيئة / No window for EGL init");
        return false;
    }
    
    LOGI("بدء تهيئة EGL... / Starting EGL initialization...");
    
    /* --- الحصول على Display / Get display --- */
    g_state.egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_state.egl_display == EGL_NO_DISPLAY) {
        LOGE("فشل الحصول على EGL display");
        return false;
    }
    
    /* --- تهيئة EGL / Initialize EGL --- */
    EGLint major, minor;
    if (!eglInitialize(g_state.egl_display, &major, &minor)) {
        LOGE("فشل تهيئة EGL");
        return false;
    }
    LOGI("EGL version: %d.%d", major, minor);
    
    /* --- اختيار إعدادات EGL / Choose EGL config --- */
    /* نطلب: OpenGL ES 3.0، RGBA8888، depth buffer */
    const EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_STENCIL_SIZE, 0,
        EGL_NONE
    };
    
    EGLint num_configs;
    if (!eglChooseConfig(g_state.egl_display, config_attribs, &g_state.egl_config, 1, &num_configs) ||
        num_configs == 0) {
        LOGE("فشل اختيار إعدادات EGL");
        return false;
    }
    
    /* --- الحصول على تنسيق النافذة / Get native visual ID --- */
    EGLint format;
    eglGetConfigAttrib(g_state.egl_display, g_state.egl_config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(g_state.window, 0, 0, format);
    
    /* --- إنشاء سطح الرسم / Create surface --- */
    g_state.egl_surface = eglCreateWindowSurface(g_state.egl_display, g_state.egl_config, 
                                                  g_state.window, NULL);
    if (g_state.egl_surface == EGL_NO_SURFACE) {
        LOGE("فشل إنشاء سطح EGL");
        return false;
    }
    
    /* --- إنشاء سياق OpenGL ES 3.0 / Create OpenGL ES 3.0 context --- */
    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,  /* OpenGL ES 3.0 */
        EGL_NONE
    };
    
    g_state.egl_context = eglCreateContext(g_state.egl_display, g_state.egl_config, 
                                            EGL_NO_CONTEXT, context_attribs);
    if (g_state.egl_context == EGL_NO_CONTEXT) {
        LOGE("فشل إنشاء سياق EGL");
        return false;
    }
    
    /* --- تفعيل السياق / Make context current --- */
    if (!eglMakeCurrent(g_state.egl_display, g_state.egl_surface, 
                        g_state.egl_surface, g_state.egl_context)) {
        LOGE("فشل تفعيل سياق EGL");
        return false;
    }
    
    /* --- الحصول على أبعاد الشاشة / Get screen dimensions --- */
    eglQuerySurface(g_state.egl_display, g_state.egl_surface, EGL_WIDTH, &g_state.screen_width);
    eglQuerySurface(g_state.egl_display, g_state.egl_surface, EGL_HEIGHT, &g_state.screen_height);
    
    /* --- الحصول على كثافة الشاشة / Get screen density --- */
    g_state.screen_density = AConfiguration_getDensity(g_state.config) / 160.0f;
    if (g_state.screen_density <= 0) g_state.screen_density = 1.0f;
    
    LOGI("أبعاد الشاشة / Screen: %dx%d @ %.2fx density", 
         g_state.screen_width, g_state.screen_height, g_state.screen_density);
    
    /* --- إعداد OpenGL / Setup OpenGL --- */
    glViewport(0, 0, g_state.screen_width, g_state.screen_height);
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    
    g_state.egl_initialized = true;
    LOGI("تم تهيئة EGL بنجاح / EGL initialized successfully");
    
    return true;
}

/**
 * تنظيف EGL
 * Cleanup EGL resources
 */
static void cleanup_egl(void) {
    if (!g_state.egl_display) return;
    
    LOGI("تنظيف EGL... / Cleaning up EGL...");
    
    eglMakeCurrent(g_state.egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    
    if (g_state.egl_context != EGL_NO_CONTEXT) {
        eglDestroyContext(g_state.egl_display, g_state.egl_context);
        g_state.egl_context = EGL_NO_CONTEXT;
    }
    
    if (g_state.egl_surface != EGL_NO_SURFACE) {
        eglDestroySurface(g_state.egl_display, g_state.egl_surface);
        g_state.egl_surface = EGL_NO_SURFACE;
    }
    
    eglTerminate(g_state.egl_display);
    g_state.egl_display = EGL_NO_DISPLAY;
    
    g_state.egl_initialized = false;
}


/* =========================================================================
 * معالجة أحداث اللمس / Touch Event Handling
 * ========================================================================= */

/**
 * معالجة حدث إدخال من AInputQueue
 * Process an input event from AInputQueue
 */
static int32_t handle_input_event(AInputEvent* event) {
    int32_t event_type = AInputEvent_getType(event);
    
    if (event_type == AINPUT_EVENT_TYPE_MOTION) {
        /* حدث لمس / Touch event */
        int32_t action = AMotionEvent_getAction(event);
        int32_t action_type = action & AMOTION_EVENT_ACTION_MASK;
        
        SadTouchEvent touch_event = {0};
        touch_event.timestamp_ms = sad_get_time_ms();
        
        /* تحديد نوع الحدث / Determine event type */
        switch (action_type) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                touch_event.type = SAD_TOUCH_DOWN;
                break;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                touch_event.type = SAD_TOUCH_UP;
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                touch_event.type = SAD_TOUCH_MOVE;
                break;
            case AMOTION_EVENT_ACTION_CANCEL:
                touch_event.type = SAD_TOUCH_CANCEL;
                break;
            default:
                return 0;
        }
        
        /* جمع نقاط اللمس / Collect touch points */
        int32_t pointer_count = AMotionEvent_getPointerCount(event);
        if (pointer_count > SAD_UI_MAX_TOUCH_POINTS) {
            pointer_count = SAD_UI_MAX_TOUCH_POINTS;
        }
        
        touch_event.pointer_count = pointer_count;
        
        for (int32_t i = 0; i < pointer_count; i++) {
            touch_event.points[i].id = AMotionEvent_getPointerId(event, i);
            touch_event.points[i].x = AMotionEvent_getX(event, i);
            touch_event.points[i].y = AMotionEvent_getY(event, i);
            touch_event.points[i].pressure = AMotionEvent_getPressure(event, i);
        }
        
        /* إرسال الحدث لتطبيق ص / Send event to Sad app */
        if (g_state.sad_app) {
            sad_handle_touch(g_state.sad_app, &touch_event);
        }
        
        return 1;  /* الحدث تمت معالجته / Event handled */
    }
    
    return 0;  /* الحدث لم يُعالج / Event not handled */
}

/**
 * معالجة طابور الإدخال
 * Process input queue events
 */
static void process_input(void) {
    if (!g_state.input_queue) return;
    
    AInputEvent* event = NULL;
    
    while (AInputQueue_getEvent(g_state.input_queue, &event) >= 0) {
        if (AInputQueue_preDispatchEvent(g_state.input_queue, event)) {
            continue;
        }
        
        int32_t handled = handle_input_event(event);
        AInputQueue_finishEvent(g_state.input_queue, event, handled);
    }
}


/* =========================================================================
 * دوال استدعاء NativeActivity / NativeActivity Callbacks
 * ========================================================================= */

/**
 * عند بدء التطبيق
 * Called when app starts
 */
static void on_start(ANativeActivity* activity) {
    LOGI("التطبيق بدأ / App started");
    (void)activity;
}

/**
 * عند استئناف التطبيق
 * Called when app resumes
 */
static void on_resume(ANativeActivity* activity) {
    LOGI("التطبيق استُؤنف / App resumed");
    g_state.is_focused = true;
    (void)activity;
}

/**
 * عند إيقاف التطبيق مؤقتاً
 * Called when app pauses
 */
static void on_pause(ANativeActivity* activity) {
    LOGI("التطبيق أُوقف مؤقتاً / App paused");
    g_state.is_focused = false;
    (void)activity;
}

/**
 * عند إيقاف التطبيق
 * Called when app stops
 */
static void on_stop(ANativeActivity* activity) {
    LOGI("التطبيق توقف / App stopped");
    (void)activity;
}

/**
 * عند تدمير التطبيق
 * Called when app is destroyed
 */
static void on_destroy(ANativeActivity* activity) {
    LOGI("التطبيق يُدمّر / App being destroyed");
    g_state.is_running = false;
    (void)activity;
}

/**
 * عند إنشاء النافذة
 * Called when window is created
 */
static void on_native_window_created(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("النافذة أُنشئت / Window created");
    
    pthread_mutex_lock(&g_state.mutex);
    g_state.window = window;
    g_state.has_window = true;
    pthread_cond_signal(&g_state.cond);
    pthread_mutex_unlock(&g_state.mutex);
    
    (void)activity;
}

/**
 * عند تغير حجم النافذة
 * Called when window is resized
 */
static void on_native_window_resized(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("حجم النافذة تغير / Window resized");
    (void)activity;
    (void)window;
}

/**
 * عند تدمير النافذة
 * Called when window is destroyed
 */
static void on_native_window_destroyed(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("النافذة دُمّرت / Window destroyed");
    
    pthread_mutex_lock(&g_state.mutex);
    
    cleanup_egl();
    g_state.window = NULL;
    g_state.has_window = false;
    
    pthread_mutex_unlock(&g_state.mutex);
    
    (void)activity;
    (void)window;
}

/**
 * عند تغير طابور الإدخال
 * Called when input queue changes
 */
static void on_input_queue_created(ANativeActivity* activity, AInputQueue* queue) {
    LOGI("طابور الإدخال أُنشئ / Input queue created");
    
    pthread_mutex_lock(&g_state.mutex);
    g_state.input_queue = queue;
    AInputQueue_attachLooper(queue, g_state.looper, LOOPER_ID_INPUT, NULL, NULL);
    pthread_mutex_unlock(&g_state.mutex);
    
    (void)activity;
}

static void on_input_queue_destroyed(ANativeActivity* activity, AInputQueue* queue) {
    LOGI("طابور الإدخال دُمّر / Input queue destroyed");
    
    pthread_mutex_lock(&g_state.mutex);
    if (g_state.input_queue == queue) {
        AInputQueue_detachLooper(queue);
        g_state.input_queue = NULL;
    }
    pthread_mutex_unlock(&g_state.mutex);
    
    (void)activity;
}

/**
 * عند تغير الإعدادات (مثل الدوران)
 * Called when config changes (like rotation)
 */
static void on_config_changed(ANativeActivity* activity) {
    LOGI("الإعدادات تغيرت / Config changed");
    AConfiguration_fromAssetManager(g_state.config, activity->assetManager);
}


/* =========================================================================
 * الحلقة الرئيسية / Main Loop
 * ========================================================================= */

/**
 * تحديث الإطار
 * Update frame
 */
static void update_frame(void) {
    /* حساب delta time */
    int64_t now = sad_get_time_ms();
    if (g_state.last_frame_time == 0) {
        g_state.last_frame_time = now;
    }
    g_state.delta_time = (now - g_state.last_frame_time) / 1000.0f;
    g_state.last_frame_time = now;
    
    if (g_state.sad_app) {
        g_state.sad_app->delta_time = g_state.delta_time;
        g_state.sad_app->frame_count++;
    }
}

/**
 * رسم الإطار
 * Render frame
 */
static void render_frame(void) {
    if (!g_state.egl_initialized || !g_state.sad_app) return;
    
    /* بدء الإطار / Begin frame */
    sad_render_begin_frame(g_state.sad_app);
    
    /* تحديث التخطيط / Update layout */
    sad_layout_update(g_state.sad_app);
    
    /* رسم الشاشة الحالية / Render current screen */
    if (g_state.sad_app->current_screen && g_state.sad_app->current_screen->root) {
        sad_render_widget(g_state.sad_app->current_screen->root);
    }
    
    /* رسم الشاشة المنبثقة (إن وجدت) / Render modal (if any) */
    if (g_state.sad_app->modal_screen && g_state.sad_app->modal_screen->root) {
        /* رسم خلفية شبه شفافة / Draw semi-transparent overlay */
        /* TODO: إضافة تأثير التعتيم */
        sad_render_widget(g_state.sad_app->modal_screen->root);
    }
    
    /* إنهاء الإطار / End frame */
    sad_render_end_frame(g_state.sad_app);
}

/**
 * الحلقة الرئيسية للتطبيق
 * Main application loop
 */
static void main_loop(void) {
    LOGI("بدء الحلقة الرئيسية / Starting main loop");
    
    while (g_state.is_running) {
        /* معالجة أحداث ALooper / Process ALooper events */
        int events;
        int ident;
        
        /* انتظر الأحداث (أو timeout للرسم المستمر) / Wait for events */
        while ((ident = ALooper_pollAll(g_state.is_focused ? 0 : -1, NULL, &events, NULL)) >= 0) {
            
            if (ident == LOOPER_ID_INPUT) {
                /* معالجة أحداث اللمس / Process touch events */
                process_input();
            }
        }
        
        /* إذا لدينا نافذة ولم نُهيئ EGL بعد / If we have window but EGL not initialized */
        if (g_state.has_window && !g_state.egl_initialized) {
            if (!init_egl()) {
                LOGE("فشل تهيئة EGL، الخروج / EGL init failed, exiting");
                g_state.is_running = false;
                break;
            }
            
            /* تهيئة تطبيق ص / Initialize Sad app */
            if (g_state.sad_app) {
                g_state.sad_app->screen_width = g_state.screen_width;
                g_state.sad_app->screen_height = g_state.screen_height;
                g_state.sad_app->screen_density = g_state.screen_density;
                g_state.sad_app->egl_display = g_state.egl_display;
                g_state.sad_app->egl_surface = g_state.egl_surface;
                g_state.sad_app->egl_context = g_state.egl_context;
                g_state.sad_app->native_activity = g_state.activity;
                g_state.sad_app->native_window = g_state.window;
                
                /* تهيئة نظام الرسم / Initialize render system */
                sad_render_init(g_state.sad_app);
            }
        }
        
        /* الرسم إذا كان التطبيق في المقدمة ولدينا EGL / Render if focused and EGL ready */
        if (g_state.is_focused && g_state.egl_initialized) {
            update_frame();
            render_frame();
        }
    }
    
    LOGI("انتهت الحلقة الرئيسية / Main loop ended");
}


/* =========================================================================
 * نقطة الدخول الرئيسية / Main Entry Point
 * ========================================================================= */

/**
 * دالة إعداد التطبيق (يجب أن يُعرّفها مستخدم المكتبة)
 * App setup function (must be defined by library user)
 * 
 * هذه الدالة يُنفّذها المبرمج لإنشاء شاشات التطبيق.
 * This function is implemented by the programmer to create app screens.
 */
extern void sad_app_setup(SadApp* app);

/**
 * نقطة دخول NativeActivity
 * NativeActivity entry point
 * 
 * هذه الدالة يستدعيها Android عند تحميل التطبيق.
 * مسؤوليتها:
 * 1. تهيئة الحالة
 * 2. تسجيل دوال الاستدعاء
 * 3. إنشاء تطبيق ص
 * 4. بدء الحلقة الرئيسية
 */
JNIEXPORT void ANativeActivity_onCreate(ANativeActivity* activity, 
                                         void* savedState, 
                                         size_t savedStateSize) {
    LOGI("=== بدء تطبيق لغة ص / Sad Language App Starting ===");
    
    (void)savedState;
    (void)savedStateSize;
    
    /* --- تهيئة الحالة / Initialize state --- */
    memset(&g_state, 0, sizeof(g_state));
    g_state.activity = activity;
    g_state.is_running = true;
    
    pthread_mutex_init(&g_state.mutex, NULL);
    pthread_cond_init(&g_state.cond, NULL);
    
    /* --- إنشاء AConfiguration / Create AConfiguration --- */
    g_state.config = AConfiguration_new();
    AConfiguration_fromAssetManager(g_state.config, activity->assetManager);
    
    /* --- إنشاء ALooper / Create ALooper --- */
    g_state.looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    
    /* --- تسجيل دوال الاستدعاء / Register callbacks --- */
    activity->callbacks->onStart = on_start;
    activity->callbacks->onResume = on_resume;
    activity->callbacks->onPause = on_pause;
    activity->callbacks->onStop = on_stop;
    activity->callbacks->onDestroy = on_destroy;
    activity->callbacks->onNativeWindowCreated = on_native_window_created;
    activity->callbacks->onNativeWindowResized = on_native_window_resized;
    activity->callbacks->onNativeWindowDestroyed = on_native_window_destroyed;
    activity->callbacks->onInputQueueCreated = on_input_queue_created;
    activity->callbacks->onInputQueueDestroyed = on_input_queue_destroyed;
    activity->callbacks->onConfigurationChanged = on_config_changed;
    
    /* --- تهيئة نظام الذاكرة / Initialize memory system --- */
    sad_memory_init(NULL);
    
    /* --- إنشاء تطبيق ص / Create Sad app --- */
    g_state.sad_app = sad_app_create();
    if (!g_state.sad_app) {
        LOGE("فشل إنشاء تطبيق ص / Failed to create Sad app");
        return;
    }
    
    /* --- استدعاء دالة إعداد التطبيق / Call app setup function --- */
    sad_app_setup(g_state.sad_app);
    
    LOGI("تم تهيئة التطبيق، بدء الحلقة الرئيسية / App initialized, starting main loop");
    
    /* --- بدء الحلقة الرئيسية / Start main loop --- */
    main_loop();
    
    /* --- التنظيف / Cleanup --- */
    LOGI("تنظيف التطبيق / Cleaning up app");
    
    if (g_state.sad_app) {
        sad_render_cleanup(g_state.sad_app);
        sad_app_destroy(g_state.sad_app);
        g_state.sad_app = NULL;
    }
    
    cleanup_egl();
    
    if (g_state.config) {
        AConfiguration_delete(g_state.config);
        g_state.config = NULL;
    }
    
    sad_memory_shutdown();
    
    pthread_mutex_destroy(&g_state.mutex);
    pthread_cond_destroy(&g_state.cond);
    
    LOGI("=== تطبيق لغة ص انتهى / Sad Language App Ended ===");
}

#endif /* __ANDROID__ */
