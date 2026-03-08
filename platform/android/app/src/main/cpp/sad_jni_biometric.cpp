/**
 * =============================================================================
 * @file sad_jni_biometric.cpp
 * @brief (AR) جسر JNI للمصادقة الحيوية — البصمة والوجه
 * @brief (EN) JNI Biometric Bridge — Fingerprint and Face
 * =============================================================================
 */

#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "SadBiometric"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace {
    bool isBiometricAvailable = false;
    JavaVM* g_jvm = nullptr;
    jobject g_callback = nullptr;
}

extern "C" {

/**
 * فحص توفر المصادقة الحيوية
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadBiometric_nativeIsAvailable(
    JNIEnv* env,
    jobject /* this */
) {
    // Note: Actual check is done in Kotlin using BiometricManager
    // This is a placeholder for native state
    return isBiometricAvailable ? JNI_TRUE : JNI_FALSE;
}

/**
 * تعيين حالة التوفر (يُستدعى من Kotlin)
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadBiometric_nativeSetAvailable(
    JNIEnv* env,
    jobject /* this */,
    jboolean available
) {
    isBiometricAvailable = available;
    LOGI("حالة المصادقة الحيوية: %s", available ? "متاحة" : "غير متاحة");
}

/**
 * بدء المصادقة
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadBiometric_nativeAuthenticate(
    JNIEnv* env,
    jobject /* this */,
    jstring title,
    jstring subtitle,
    jstring description,
    jstring negativeButtonText,
    jobject callback
) {
    const char* titleStr = env->GetStringUTFChars(title, nullptr);
    const char* subtitleStr = env->GetStringUTFChars(subtitle, nullptr);
    
    LOGI("بدء المصادقة الحيوية: %s", titleStr);
    
    // Store callback for later use
    if (g_callback != nullptr) {
        env->DeleteGlobalRef(g_callback);
    }
    g_callback = env->NewGlobalRef(callback);
    
    // Note: Actual authentication is done in Kotlin using BiometricPrompt
    // Native side just stores the callback
    
    env->ReleaseStringUTFChars(title, titleStr);
    env->ReleaseStringUTFChars(subtitle, subtitleStr);
}

/**
 * إرسال نتيجة المصادقة (يُستدعى من Kotlin)
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadBiometric_nativeOnAuthResult(
    JNIEnv* env,
    jobject /* this */,
    jboolean success,
    jstring errorMessage
) {
    if (success) {
        LOGI("نجحت المصادقة الحيوية");
    } else {
        const char* error = env->GetStringUTFChars(errorMessage, nullptr);
        LOGE("فشلت المصادقة: %s", error);
        env->ReleaseStringUTFChars(errorMessage, error);
    }
    
    // TODO: Call stored callback with result
    if (g_callback != nullptr) {
        // env->CallVoidMethod(g_callback, methodId, success);
        env->DeleteGlobalRef(g_callback);
        g_callback = nullptr;
    }
}

/**
 * إلغاء المصادقة
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadBiometric_nativeCancel(
    JNIEnv* env,
    jobject /* this */
) {
    LOGI("إلغاء المصادقة الحيوية");
    
    if (g_callback != nullptr) {
        env->DeleteGlobalRef(g_callback);
        g_callback = nullptr;
    }
}

} // extern "C"
