/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  Android NativeActivity Bridge with Full UI System
 *  جسر NativeActivity مع نظام واجهات كامل
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 *  هذا الجسر يدعم:
 *  - رسم واجهات حقيقية (مثل Flutter)
 *  - أحداث اللمس والتفاعل
 *  - تخطيط RTL للعربية
 *  - رسوم متحركة سلسة
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/log.h>
#include <android/looper.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "graphics/sad_flutter_ui.h"

#define LOG_TAG "SadApp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

// دالة البناء من لغة ص - تُعيد Widget الجذر
typedef SadWidget* (*SadBuildUIFunc)(void);

// يجب على برنامج ص تعريف هذه الدالة
extern SadWidget* ابنِ_الواجهة(void);

// اختياري: دالة main للتوافق
extern int main(void) __attribute__((weak));

#ifdef __cplusplus
}
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  حالة التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    ANativeActivity* activity;
    AAssetManager* assetManager;
    ANativeWindow* window;
    AInputQueue* inputQueue;
    ALooper* looper;
    
    // EGL
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLConfig config;
    
    // الأبعاد
    int width;
    int height;
    float density;
    
    // نظام UI
    SadApp* uiApp;
    SadWidget* rootWidget;
    
    // الحالة
    int running;
    int animating;
    int hasFocus;
    int initialized;
    
    // الخيط
    pthread_t renderThread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    
    // الأوامر
    int pendingCmd;
    
} AppState;

static AppState g_state = {0};

// الأوامر
enum {
    CMD_NONE = 0,
    CMD_INIT_WINDOW,
    CMD_TERM_WINDOW,
    CMD_GAINED_FOCUS,
    CMD_LOST_FOCUS,
    CMD_DESTROY
};

// ═══════════════════════════════════════════════════════════════════════════════
//  EGL
// ═══════════════════════════════════════════════════════════════════════════════

static int init_egl(void) {
    if (g_state.window == NULL) {
        LOGE("No window");
        return 0;
    }
    
    const EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    
    // الحصول على العرض
    g_state.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_state.display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return 0;
    }
    
    // التهيئة
    if (!eglInitialize(g_state.display, NULL, NULL)) {
        LOGE("eglInitialize failed");
        return 0;
    }
    
    // اختيار الإعدادات
    EGLint numConfigs;
    if (!eglChooseConfig(g_state.display, configAttribs, &g_state.config, 1, &numConfigs) || numConfigs == 0) {
        // محاولة ES2 إذا فشل ES3
        const EGLint fallbackAttribs[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_NONE
        };
        if (!eglChooseConfig(g_state.display, fallbackAttribs, &g_state.config, 1, &numConfigs)) {
            LOGE("eglChooseConfig failed");
            return 0;
        }
    }
    
    // تعيين تنسيق النافذة
    EGLint format;
    eglGetConfigAttrib(g_state.display, g_state.config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(g_state.window, 0, 0, format);
    
    // إنشاء السطح
    g_state.surface = eglCreateWindowSurface(g_state.display, g_state.config, g_state.window, NULL);
    if (g_state.surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return 0;
    }
    
    // إنشاء السياق
    g_state.context = eglCreateContext(g_state.display, g_state.config, EGL_NO_CONTEXT, contextAttribs);
    if (g_state.context == EGL_NO_CONTEXT) {
        // محاولة ES2
        const EGLint fallbackContext[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };
        g_state.context = eglCreateContext(g_state.display, g_state.config, EGL_NO_CONTEXT, fallbackContext);
        if (g_state.context == EGL_NO_CONTEXT) {
            LOGE("eglCreateContext failed");
            return 0;
        }
    }
    
    // تفعيل السياق
    if (!eglMakeCurrent(g_state.display, g_state.surface, g_state.surface, g_state.context)) {
        LOGE("eglMakeCurrent failed");
        return 0;
    }
    
    // الحصول على الأبعاد
    eglQuerySurface(g_state.display, g_state.surface, EGL_WIDTH, &g_state.width);
    eglQuerySurface(g_state.display, g_state.surface, EGL_HEIGHT, &g_state.height);
    
    // الكثافة
    g_state.density = 2.0f; // افتراضي
    
    LOGI("EGL initialized: %dx%d, density=%.1f", g_state.width, g_state.height, g_state.density);
    LOGI("GL_VENDOR: %s", glGetString(GL_VENDOR));
    LOGI("GL_RENDERER: %s", glGetString(GL_RENDERER));
    LOGI("GL_VERSION: %s", glGetString(GL_VERSION));
    
    return 1;
}

static void term_egl(void) {
    if (g_state.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(g_state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        
        if (g_state.context != EGL_NO_CONTEXT) {
            eglDestroyContext(g_state.display, g_state.context);
        }
        if (g_state.surface != EGL_NO_SURFACE) {
            eglDestroySurface(g_state.display, g_state.surface);
        }
        eglTerminate(g_state.display);
    }
    
    g_state.display = EGL_NO_DISPLAY;
    g_state.surface = EGL_NO_SURFACE;
    g_state.context = EGL_NO_CONTEXT;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  نظام الواجهات
// ═══════════════════════════════════════════════════════════════════════════════

static void init_ui_system(void) {
    if (g_state.uiApp) return;
    
    // إنشاء التطبيق
    g_state.uiApp = sadapp_create(g_state.width, g_state.height, g_state.density);
    if (!g_state.uiApp) {
        LOGE("Failed to create SadApp");
        return;
    }
    
    LOGI("UI system initialized");
    
    // بناء الواجهة من كود ص
    if (ابنِ_الواجهة) {
        LOGI("Building UI from ابنِ_الواجهة()...");
        g_state.rootWidget = ابنِ_الواجهة();
        if (g_state.rootWidget) {
            sadapp_set_root(g_state.uiApp, g_state.rootWidget);
            LOGI("UI built successfully");
        } else {
            LOGE("ابنِ_الواجهة() returned NULL");
        }
    } else if (main) {
        // وضع التوافق: تشغيل main وعرض خلفية
        LOGI("Running main() in compatibility mode...");
        main();
        LOGI("main() completed");
        
        // إنشاء واجهة بسيطة تظهر أن البرنامج اكتمل
        SadWidget* col = sad_column();
        sad_set_background(col, SADCOLOR_BACKGROUND);
        sad_set_alignment(col, SADALIGN_CENTER, SADALIGN_CENTER);
        sad_set_padding(col, sadpadding_all(32));
        
        SadWidget* card = sad_card();
        sad_set_padding(card, sadpadding_all(24));
        
        SadWidget* cardContent = sad_column();
        sad_set_spacing(cardContent, 16);
        sad_set_alignment(cardContent, SADALIGN_CENTER, SADALIGN_CENTER);
        
        SadWidget* title = sad_text_styled("✓ تم تنفيذ البرنامج بنجاح", SADTEXT_HEADLINE5);
        sad_add_child(cardContent, title);
        
        SadWidget* subtitle = sad_text_styled("انظر إلى logcat للمخرجات", SADTEXT_BODY1);
        sad_add_child(cardContent, subtitle);
        
        sad_add_child(card, cardContent);
        sad_add_child(col, card);
        
        sadapp_set_root(g_state.uiApp, col);
        g_state.rootWidget = col;
    } else {
        LOGE("No UI builder or main function found!");
    }
    
    g_state.initialized = 1;
}

static void term_ui_system(void) {
    if (g_state.uiApp) {
        sadapp_destroy(g_state.uiApp);
        g_state.uiApp = NULL;
    }
    g_state.rootWidget = NULL;
    g_state.initialized = 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  معالجة الإدخال
// ═══════════════════════════════════════════════════════════════════════════════

static int handle_input(AInputEvent* event) {
    if (!g_state.uiApp) return 0;
    
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        
        int touchAction = 2; // move
        switch (action) {
            case AMOTION_EVENT_ACTION_DOWN:
                touchAction = 0; // down
                break;
            case AMOTION_EVENT_ACTION_UP:
                touchAction = 1; // up
                break;
            case AMOTION_EVENT_ACTION_CANCEL:
                touchAction = 3; // cancel
                break;
        }
        
        return sadapp_touch(g_state.uiApp, touchAction, x, y) ? 1 : 0;
    }
    
    return 0;
}

static void process_input(void) {
    if (!g_state.inputQueue) return;
    
    AInputEvent* event = NULL;
    while (AInputQueue_getEvent(g_state.inputQueue, &event) >= 0) {
        if (AInputQueue_preDispatchEvent(g_state.inputQueue, event)) {
            continue;
        }
        int handled = handle_input(event);
        AInputQueue_finishEvent(g_state.inputQueue, event, handled);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  حلقة الرسم
// ═══════════════════════════════════════════════════════════════════════════════

static void render_frame(void) {
    if (g_state.display == EGL_NO_DISPLAY) return;
    if (!g_state.uiApp) return;
    
    // رسم الإطار
    sadapp_render(g_state.uiApp);
    
    // تبديل المخازن
    eglSwapBuffers(g_state.display, g_state.surface);
}

static void* render_thread_func(void* arg) {
    LOGI("Render thread started");
    
    while (g_state.running) {
        pthread_mutex_lock(&g_state.mutex);
        
        // معالجة الأوامر
        int cmd = g_state.pendingCmd;
        g_state.pendingCmd = CMD_NONE;
        
        pthread_mutex_unlock(&g_state.mutex);
        
        switch (cmd) {
            case CMD_INIT_WINDOW:
                LOGI("CMD_INIT_WINDOW");
                if (init_egl()) {
                    init_ui_system();
                }
                break;
                
            case CMD_TERM_WINDOW:
                LOGI("CMD_TERM_WINDOW");
                term_ui_system();
                term_egl();
                break;
                
            case CMD_GAINED_FOCUS:
                LOGI("CMD_GAINED_FOCUS");
                g_state.hasFocus = 1;
                break;
                
            case CMD_LOST_FOCUS:
                LOGI("CMD_LOST_FOCUS");
                g_state.hasFocus = 0;
                break;
                
            case CMD_DESTROY:
                LOGI("CMD_DESTROY");
                g_state.running = 0;
                break;
        }
        
        // معالجة الإدخال
        process_input();
        
        // الرسم
        if (g_state.initialized && g_state.hasFocus) {
            render_frame();
        }
        
        // انتظار قصير
        usleep(16666); // ~60 FPS
    }
    
    term_ui_system();
    term_egl();
    
    LOGI("Render thread ended");
    return NULL;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Callbacks
// ═══════════════════════════════════════════════════════════════════════════════

static void send_cmd(int cmd) {
    pthread_mutex_lock(&g_state.mutex);
    g_state.pendingCmd = cmd;
    pthread_mutex_unlock(&g_state.mutex);
}

static void onDestroy(ANativeActivity* activity) {
    LOGI("onDestroy");
    send_cmd(CMD_DESTROY);
    pthread_join(g_state.renderThread, NULL);
    pthread_mutex_destroy(&g_state.mutex);
    pthread_cond_destroy(&g_state.cond);
}

static void onStart(ANativeActivity* activity) {
    LOGI("onStart");
}

static void onResume(ANativeActivity* activity) {
    LOGI("onResume");
}

static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen) {
    *outLen = 0;
    return NULL;
}

static void onPause(ANativeActivity* activity) {
    LOGI("onPause");
}

static void onStop(ANativeActivity* activity) {
    LOGI("onStop");
}

static void onConfigurationChanged(ANativeActivity* activity) {
    LOGI("onConfigurationChanged");
}

static void onLowMemory(ANativeActivity* activity) {
    LOGI("onLowMemory");
}

static void onWindowFocusChanged(ANativeActivity* activity, int focused) {
    LOGI("onWindowFocusChanged: %d", focused);
    send_cmd(focused ? CMD_GAINED_FOCUS : CMD_LOST_FOCUS);
}

static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("onNativeWindowCreated");
    g_state.window = window;
    send_cmd(CMD_INIT_WINDOW);
}

static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("onNativeWindowDestroyed");
    send_cmd(CMD_TERM_WINDOW);
    g_state.window = NULL;
}

static void onNativeWindowResized(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("onNativeWindowResized");
}

static void onNativeWindowRedrawNeeded(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("onNativeWindowRedrawNeeded");
}

static void onContentRectChanged(ANativeActivity* activity, const ARect* rect) {
    LOGI("onContentRectChanged: %d,%d - %d,%d", rect->left, rect->top, rect->right, rect->bottom);
}

static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) {
    LOGI("onInputQueueCreated");
    g_state.inputQueue = queue;
}

static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) {
    LOGI("onInputQueueDestroyed");
    g_state.inputQueue = NULL;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  نقطة الدخول
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef __cplusplus
extern "C"
#endif
__attribute__((visibility("default")))
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    LOGI("═══════════════════════════════════════════════════════════");
    LOGI("  لغة ص - نظام واجهات أندرويد الثوري");
    LOGI("  Sad Language Revolutionary Android UI System");
    LOGI("═══════════════════════════════════════════════════════════");
    
    // تهيئة الحالة
    memset(&g_state, 0, sizeof(g_state));
    g_state.activity = activity;
    g_state.assetManager = activity->assetManager;
    g_state.display = EGL_NO_DISPLAY;
    g_state.surface = EGL_NO_SURFACE;
    g_state.context = EGL_NO_CONTEXT;
    g_state.running = 1;
    
    pthread_mutex_init(&g_state.mutex, NULL);
    pthread_cond_init(&g_state.cond, NULL);
    
    // تعيين الـ callbacks
    activity->callbacks->onDestroy = onDestroy;
    activity->callbacks->onStart = onStart;
    activity->callbacks->onResume = onResume;
    activity->callbacks->onSaveInstanceState = onSaveInstanceState;
    activity->callbacks->onPause = onPause;
    activity->callbacks->onStop = onStop;
    activity->callbacks->onConfigurationChanged = onConfigurationChanged;
    activity->callbacks->onLowMemory = onLowMemory;
    activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    activity->callbacks->onNativeWindowResized = onNativeWindowResized;
    activity->callbacks->onNativeWindowRedrawNeeded = onNativeWindowRedrawNeeded;
    activity->callbacks->onContentRectChanged = onContentRectChanged;
    activity->callbacks->onInputQueueCreated = onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;
    
    // بدء خيط الرسم
    pthread_create(&g_state.renderThread, NULL, render_thread_func, NULL);
    
    LOGI("App initialized, render thread started");
}
