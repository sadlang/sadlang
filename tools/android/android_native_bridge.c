/**
 * Android NativeActivity Bridge with Visual Display (Fixed)
 * جسر NativeActivity لتطبيقات لغة ص مع عرض مرئي
 */

#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/log.h>
#include <android/looper.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define LOG_TAG "SadApp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
// (AR) استخدام main بدلاً من __sad_main للتوافق مع المترجم
// (EN) Use main instead of __sad_main for compiler compatibility
extern int main(void);
#ifdef __cplusplus
}
#endif

// App state
typedef struct {
    ANativeActivity* activity;
    ANativeWindow* window;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int width;
    int height;
    int animating;
    int program_done;
} AppState;

static AppState g_state = {0};

// Capture stdout to logcat
static int capture_pfd[2];

static void* capture_thread_func(void* arg) {
    ssize_t readlen;
    char buf[512];
    while ((readlen = read(capture_pfd[0], buf, sizeof(buf) - 1)) > 0) {
        buf[readlen] = '\0';
        while (readlen > 0 && (buf[readlen-1] == '\n' || buf[readlen-1] == '\r')) {
            buf[--readlen] = '\0';
        }
        if (readlen > 0) {
            LOGI("%s", buf);
        }
    }
    return NULL;
}

static void setup_capture(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);
    if (pipe(capture_pfd) == 0) {
        dup2(capture_pfd[1], STDOUT_FILENO);
        dup2(capture_pfd[1], STDERR_FILENO);
        pthread_t t;
        pthread_create(&t, NULL, capture_thread_func, NULL);
        pthread_detach(t);
    }
}

// Initialize EGL display
static int init_display(void) {
    if (g_state.window == NULL) return 0;
    
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };
    
    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    g_state.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_state.display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return 0;
    }
    
    if (!eglInitialize(g_state.display, NULL, NULL)) {
        LOGE("eglInitialize failed");
        return 0;
    }
    
    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(g_state.display, attribs, &config, 1, &numConfigs);
    
    EGLint format;
    eglGetConfigAttrib(g_state.display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(g_state.window, 0, 0, format);
    
    g_state.surface = eglCreateWindowSurface(g_state.display, config, g_state.window, NULL);
    if (g_state.surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return 0;
    }
    
    g_state.context = eglCreateContext(g_state.display, config, EGL_NO_CONTEXT, context_attribs);
    if (g_state.context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return 0;
    }
    
    if (!eglMakeCurrent(g_state.display, g_state.surface, g_state.surface, g_state.context)) {
        LOGE("eglMakeCurrent failed");
        return 0;
    }
    
    eglQuerySurface(g_state.display, g_state.surface, EGL_WIDTH, &g_state.width);
    eglQuerySurface(g_state.display, g_state.surface, EGL_HEIGHT, &g_state.height);
    
    glViewport(0, 0, g_state.width, g_state.height);
    
    LOGI("Display initialized: %dx%d", g_state.width, g_state.height);
    return 1;
}

static void term_display(void) {
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
    g_state.context = EGL_NO_CONTEXT;
    g_state.surface = EGL_NO_SURFACE;
}

static void draw_frame(void) {
    if (g_state.display == EGL_NO_DISPLAY) return;
    
    // Green when done, blue when running
    if (g_state.program_done) {
        glClearColor(0.1f, 0.7f, 0.2f, 1.0f);  // Green
    } else {
        glClearColor(0.1f, 0.3f, 0.7f, 1.0f);  // Blue
    }
    glClear(GL_COLOR_BUFFER_BIT);
    
    eglSwapBuffers(g_state.display, g_state.surface);
}

// Main loop thread
static void* main_loop_func(void* arg) {
    // Wait for window
    while (g_state.window == NULL) {
        usleep(50000);
    }
    
    // Initialize display in this thread
    if (!init_display()) {
        LOGE("Failed to initialize display");
        return NULL;
    }
    
    // Draw initial blue frame
    draw_frame();
    
    // Run program
    LOGI("====================================");
    LOGI("  لغة ص - تطبيق أندرويد");
    LOGI("  Sad Language Android App");
    LOGI("====================================");
    LOGI("Running program...");
    
    // (AR) استدعاء دالة main المولدة من المترجم
    // (EN) Call the compiler-generated main function
    main();
    
    LOGI("====================================");
    LOGI("  Program completed successfully!");
    LOGI("====================================");
    
    g_state.program_done = 1;
    
    // Keep rendering green
    while (g_state.animating) {
        draw_frame();
        usleep(100000);  // 10 FPS
    }
    
    return NULL;
}

// Callbacks
static void onDestroy(ANativeActivity* activity) {
    LOGI("onDestroy");
    g_state.animating = 0;
    term_display();
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

static void onConfigurationChanged(ANativeActivity* activity) {}
static void onLowMemory(ANativeActivity* activity) {}
static void onWindowFocusChanged(ANativeActivity* activity, int focused) {}

static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("onNativeWindowCreated");
    g_state.window = window;
}

static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {
    LOGI("onNativeWindowDestroyed");
    g_state.animating = 0;
    term_display();
    g_state.window = NULL;
}

static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) {}
static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) {}

// Entry point
#ifdef __cplusplus
extern "C"
#endif
__attribute__((visibility("default")))
void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    LOGI("ANativeActivity_onCreate");
    
    // Setup capture
    setup_capture();
    
    // Initialize state
    memset(&g_state, 0, sizeof(g_state));
    g_state.activity = activity;
    g_state.display = EGL_NO_DISPLAY;
    g_state.surface = EGL_NO_SURFACE;
    g_state.context = EGL_NO_CONTEXT;
    g_state.animating = 1;
    
    // Set callbacks
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
    activity->callbacks->onInputQueueCreated = onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;
    
    // Start main loop in separate thread
    pthread_t t;
    pthread_create(&t, NULL, main_loop_func, NULL);
    pthread_detach(t);
    
    LOGI("App initialized");
}
