// ═══════════════════════════════════════════════════════════════════════════
// sad_jni_bridge.h - جسر JNI للتواصل مع Java/Kotlin
// ═══════════════════════════════════════════════════════════════════════════
//
// هذا الجسر يسمح لـ Activity الخفيفة (Kotlin shell) بالتواصل مع Runtime لغة ص.
// الـ Activity لا تحتوي على أي منطق - فقط تمرر الأحداث للـ Runtime.
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef SAD_JNI_BRIDGE_H
#define SAD_JNI_BRIDGE_H

#ifdef SAD_ANDROID

#include <jni.h>

extern "C" {

// ═══════════════════════════════════════════════════════════════════════════
// تهيئة الـ Runtime / Runtime Initialization
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تحميل المكتبة
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved);

/**
 * @brief تفريغ المكتبة
 */
JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved);

// ═══════════════════════════════════════════════════════════════════════════
// دوال SadRuntime (تُستدعى من Kotlin)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تهيئة الـ Runtime
 */
JNIEXPORT jboolean JNICALL
Java_sad_android_runtime_SadRuntime_nativeInit(
    JNIEnv* env,
    jobject thiz,
    jobject context,
    jobject assetManager
);

/**
 * @brief إيقاف الـ Runtime
 */
JNIEXPORT void JNICALL
Java_sad_android_runtime_SadRuntime_nativeShutdown(
    JNIEnv* env,
    jobject thiz
);

/**
 * @brief تنفيذ ملف ص
 */
JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeExecuteFile(
    JNIEnv* env,
    jobject thiz,
    jstring filePath
);

/**
 * @brief تنفيذ كود ص مباشرة
 */
JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeExecuteCode(
    JNIEnv* env,
    jobject thiz,
    jstring code
);

/**
 * @brief استدعاء دالة ص
 */
JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeCallFunction(
    JNIEnv* env,
    jobject thiz,
    jstring functionName,
    jobjectArray args
);

// ═══════════════════════════════════════════════════════════════════════════
// دورة حياة Activity / Activity Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnCreate(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnStart(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnResume(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnPause(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnStop(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnDestroy(JNIEnv* env, jobject thiz);

// ═══════════════════════════════════════════════════════════════════════════
// أحداث UI / UI Events
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnButtonClick(
    JNIEnv* env,
    jobject thiz,
    jstring buttonId
);

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnTextChange(
    JNIEnv* env,
    jobject thiz,
    jstring inputId,
    jstring text
);

// ═══════════════════════════════════════════════════════════════════════════
// إدارة المتغيرات / Variable Management  
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadRuntime_nativeSetVariable(
    JNIEnv* env,
    jobject thiz,
    jstring name,
    jstring value
);

JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeGetVariable(
    JNIEnv* env,
    jobject thiz,
    jstring name
);

// ═══════════════════════════════════════════════════════════════════════════
// الحصول على UI الحالي / Get Current UI
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief الحصول على وصف UI بتنسيق JSON
 * يُستخدم لبناء Compose UI من كود ص
 */
JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeGetUIDescription(
    JNIEnv* env,
    jobject thiz
);

/**
 * @brief تحديث UI من JSON
 */
JNIEXPORT void JNICALL
Java_sad_android_runtime_SadRuntime_nativeUpdateUI(
    JNIEnv* env,
    jobject thiz,
    jstring uiJson
);

} // extern "C"

#endif // SAD_ANDROID

#endif // SAD_JNI_BRIDGE_H
