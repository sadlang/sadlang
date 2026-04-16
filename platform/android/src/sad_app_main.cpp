// ═══════════════════════════════════════════════════════════════════════════════
// sad_app_main.cpp - نقطة الدخول الرئيسية لتطبيق ص على أندرويد
// Sad App Main Entry Point for Android
// ═══════════════════════════════════════════════════════════════════════════════
//
// @brief (AR) هذا الملف يوفر واجهات JNI للتواصل بين Java/Kotlin وكود ص الأصلي
// @brief (EN) This file provides JNI interfaces for Java/Kotlin to Sad native code
//
// ═══════════════════════════════════════════════════════════════════════════════

#include <jni.h>
#include <android/log.h>
#include <string>
#include <memory>

#include "native_ui_builder.h"
#include "native_ui_storage.h"
#include "sad_ui/ir.h"

#define LOG_TAG "SadApp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// ═══════════════════════════════════════════════════════════════════════════════
// Forward Declarations
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {
    // من sad_android_runtime.cpp
    bool sad_runtime_init();
    void sad_runtime_shutdown();
    bool sad_execute_code(const char* code);
    const char* sad_get_last_error();
    const char* sad_get_last_output();
    const char* sad_get_ui_html();
    bool sad_has_native_ui();
    void sad_clear_native_ui();
}
std::shared_ptr<sad::ui::IRModule> sad_get_native_ui_module();

// ═══════════════════════════════════════════════════════════════════════════════
// JNI Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

namespace {
    JavaVM* g_javaVM = nullptr;
    jobject g_activity = nullptr;
    
    std::string jstringToString(JNIEnv* env, jstring jstr) {
        if (!jstr) return "";
        const char* chars = env->GetStringUTFChars(jstr, nullptr);
        std::string result(chars);
        env->ReleaseStringUTFChars(jstr, chars);
        return result;
    }
    
    jstring stringToJstring(JNIEnv* env, const std::string& str) {
        return env->NewStringUTF(str.c_str());
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// JNI OnLoad/OnUnload
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("═══════════════════════════════════════");
    LOGI("  مكتبة ص الأصلية - JNI_OnLoad");
    LOGI("  Sad Native Library Loading");
    LOGI("═══════════════════════════════════════");
    
    g_javaVM = vm;
    
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        LOGE("Failed to get JNI environment");
        return JNI_ERR;
    }
    
    // تهيئة Runtime ص
    if (!sad_runtime_init()) {
        LOGE("Failed to initialize Sad runtime");
        return JNI_ERR;
    }
    
    LOGI("[✓] مكتبة ص جاهزة / Sad library ready");
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    LOGI("Sad Native Library Unloading");
    sad_runtime_shutdown();
    g_javaVM = nullptr;
    g_activity = nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
// JNI Native Methods - SadEngine
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {

/**
 * @brief (AR) تهيئة محرك ص مع Activity
 * @brief (EN) Initialize Sad engine with Activity reference
 */
JNIEXPORT jboolean JNICALL
Java_com_sad_app_SadEngine_nativeInit(JNIEnv* env, jobject thiz, jobject activity) {
    LOGI("SadEngine.nativeInit() called");
    
    // حفظ مرجع Activity
    if (g_activity) {
        env->DeleteGlobalRef(g_activity);
    }
    g_activity = env->NewGlobalRef(activity);
    
    return JNI_TRUE;
}

/**
 * @brief (AR) تنفيذ كود ص
 * @brief (EN) Execute Sad code string
 */
JNIEXPORT jboolean JNICALL
Java_com_sad_app_SadEngine_nativeExecute(JNIEnv* env, jobject thiz, jstring code) {
    std::string codeStr = jstringToString(env, code);
    LOGD("Executing Sad code: %.50s...", codeStr.c_str());
    
    bool result = sad_execute_code(codeStr.c_str());
    
    if (!result) {
        LOGE("Execution failed: %s", sad_get_last_error());
    }
    
    return result ? JNI_TRUE : JNI_FALSE;
}

/**
 * @brief (AR) الحصول على آخر خطأ
 * @brief (EN) Get last error message
 */
JNIEXPORT jstring JNICALL
Java_com_sad_app_SadEngine_nativeGetLastError(JNIEnv* env, jobject thiz) {
    const char* error = sad_get_last_error();
    return stringToJstring(env, error ? error : "");
}

/**
 * @brief (AR) إيقاف المحرك
 * @brief (EN) Shutdown the engine
 */
JNIEXPORT void JNICALL
Java_com_sad_app_SadEngine_nativeShutdown(JNIEnv* env, jobject thiz) {
    LOGI("SadEngine.nativeShutdown() called");
    
    if (g_activity) {
        env->DeleteGlobalRef(g_activity);
        g_activity = nullptr;
    }
}

/**
 * @brief (AR) الحصول على إصدار المكتبة
 * @brief (EN) Get library version
 */
JNIEXPORT jstring JNICALL
Java_com_sad_app_SadEngine_nativeGetVersion(JNIEnv* env, jobject thiz) {
    return stringToJstring(env, "1.0.0-alpha");
}

/**
 * @brief (AR) الحصول على آخر مخرجات التنفيذ
 * @brief (EN) Get last execution output
 */
JNIEXPORT jstring JNICALL
Java_com_sad_app_SadEngine_nativeGetLastOutput(JNIEnv* env, jobject thiz) {
    const char* output = sad_get_last_output();
    return stringToJstring(env, output ? output : "");
}

/**
 * @brief (AR) الحصول على HTML الواجهة المولّدة
 * @brief (EN) Get generated UI HTML from last execution
 */
JNIEXPORT jstring JNICALL
Java_com_sad_app_SadEngine_nativeGetUIHtml(JNIEnv* env, jobject thiz) {
    const char* html = sad_get_ui_html();
    return stringToJstring(env, html ? html : "");
}

/**
 * @brief (AR) هل يوجد واجهة أصلية جاهزة للعرض؟
 * @brief (EN) Is there a pending native UI to render?
 */
JNIEXPORT jboolean JNICALL
Java_com_sad_app_SadEngine_nativeHasNativeUI(JNIEnv* env, jobject thiz) {
    return sad_has_native_ui() ? JNI_TRUE : JNI_FALSE;
}

/**
 * @brief (AR) بناء واجهة أصلية وإضافتها إلى الحاوية
 * @brief (EN) Build native UI and add it to the container ViewGroup
 */
JNIEXPORT jboolean JNICALL
Java_com_sad_app_SadEngine_nativeBuildNativeUI(JNIEnv* env, jobject thiz, jobject container) {
    LOGI("nativeBuildNativeUI: بداية بناء الواجهة الأصلية");
    
    auto module = sad_get_native_ui_module();
    if (!module || !module->root) {
        LOGE("nativeBuildNativeUI: لا توجد وحدة IR مخزّنة");
        sad_clear_native_ui();
        return JNI_FALSE;
    }

    // استخدام Activity كـ Context
    if (!g_activity) {
        LOGE("nativeBuildNativeUI: Activity غير متوفر");
        sad_clear_native_ui();
        return JNI_FALSE;
    }

    NativeUIBuilder builder;
    if (!builder.init(env, g_activity)) {
        LOGE("nativeBuildNativeUI: فشل تهيئة NativeUIBuilder");
        sad_clear_native_ui();
        return JNI_FALSE;
    }

    // بناء شجرة Views الأصلية
    jobject rootView = builder.buildTree(*module->root);
    if (!rootView) {
        LOGE("nativeBuildNativeUI: فشل بناء الشجرة");
        sad_clear_native_ui();
        return JNI_FALSE;
    }

    // لف الجذر في ScrollView مع تصميم عربي
    jobject wrappedView = builder.wrapInScroll(rootView);
    if (!wrappedView) {
        wrappedView = rootView;
    }

    // إضافة إلى الحاوية
    jclass viewGroupClass = env->FindClass("android/view/ViewGroup");
    if (viewGroupClass) {
        // مسح الحاوية أولاً
        jmethodID removeAllViews = env->GetMethodID(viewGroupClass, "removeAllViews", "()V");
        if (removeAllViews) {
            env->CallVoidMethod(container, removeAllViews);
        }
        // إضافة العرض الجذري
        jmethodID addView = env->GetMethodID(viewGroupClass, "addView", "(Landroid/view/View;)V");
        if (addView) {
            env->CallVoidMethod(container, addView, wrappedView);
            LOGI("nativeBuildNativeUI: تمت إضافة الواجهة الأصلية بنجاح");
        }
        env->DeleteLocalRef(viewGroupClass);
    }

    // تنظيف المراجع المحلية
    env->DeleteLocalRef(rootView);
    if (wrappedView != rootView) {
        env->DeleteLocalRef(wrappedView);
    }

    // مسح الوحدة المخزّنة
    sad_clear_native_ui();
    
    return JNI_TRUE;
}

} // extern "C"

// ═══════════════════════════════════════════════════════════════════════════════
// JNI Native Methods - SadUI
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {

/**
 * @brief (AR) إنشاء عنصر واجهة
 * @brief (EN) Create UI element
 */
JNIEXPORT jlong JNICALL
Java_com_sad_app_SadUI_nativeCreateElement(JNIEnv* env, jobject thiz, 
                                            jstring type, jstring props) {
    std::string typeStr = jstringToString(env, type);
    std::string propsStr = jstringToString(env, props);
    
    LOGD("Creating UI element: %s", typeStr.c_str());
    
    // TODO: Implement actual UI element creation
    // For now, return a dummy handle
    static long nextHandle = 1;
    return nextHandle++;
}

/**
 * @brief (AR) تحديث خصائص عنصر
 * @brief (EN) Update element properties
 */
JNIEXPORT jboolean JNICALL
Java_com_sad_app_SadUI_nativeUpdateElement(JNIEnv* env, jobject thiz,
                                            jlong handle, jstring props) {
    std::string propsStr = jstringToString(env, props);
    LOGD("Updating element %ld: %s", (long)handle, propsStr.c_str());
    
    // TODO: Implement actual update
    return JNI_TRUE;
}

/**
 * @brief (AR) حذف عنصر
 * @brief (EN) Delete element
 */
JNIEXPORT void JNICALL
Java_com_sad_app_SadUI_nativeDeleteElement(JNIEnv* env, jobject thiz, jlong handle) {
    LOGD("Deleting element %ld", (long)handle);
    // TODO: Implement actual deletion
}

} // extern "C"
