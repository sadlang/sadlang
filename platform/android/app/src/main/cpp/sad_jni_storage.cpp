/**
 * =============================================================================
 * @file sad_jni_storage.cpp
 * @brief (AR) جسر JNI للتخزين — قراءة/كتابة الملفات
 * @brief (EN) JNI Storage Bridge — File Read/Write
 * =============================================================================
 */

#include <jni.h>
#include <string>
#include <fstream>
#include <sstream>
#include <android/log.h>

#define LOG_TAG "SadStorage"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {

/**
 * قراءة ملف نصي
 */
JNIEXPORT jstring JNICALL
Java_sad_lang_android_SadStorage_nativeReadFile(
    JNIEnv* env,
    jobject /* this */,
    jstring filePath
) {
    const char* path = env->GetStringUTFChars(filePath, nullptr);
    
    std::ifstream file(path);
    if (!file.is_open()) {
        LOGE("فشل فتح الملف: %s", path);
        env->ReleaseStringUTFChars(filePath, path);
        return env->NewStringUTF("");
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    LOGI("تم قراءة الملف: %s", path);
    env->ReleaseStringUTFChars(filePath, path);
    
    return env->NewStringUTF(buffer.str().c_str());
}

/**
 * كتابة ملف نصي
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadStorage_nativeWriteFile(
    JNIEnv* env,
    jobject /* this */,
    jstring filePath,
    jstring content
) {
    const char* path = env->GetStringUTFChars(filePath, nullptr);
    const char* data = env->GetStringUTFChars(content, nullptr);
    
    std::ofstream file(path);
    if (!file.is_open()) {
        LOGE("فشل إنشاء الملف: %s", path);
        env->ReleaseStringUTFChars(filePath, path);
        env->ReleaseStringUTFChars(content, data);
        return JNI_FALSE;
    }
    
    file << data;
    file.close();
    
    LOGI("تم كتابة الملف: %s", path);
    env->ReleaseStringUTFChars(filePath, path);
    env->ReleaseStringUTFChars(content, data);
    
    return JNI_TRUE;
}

/**
 * إضافة للملف
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadStorage_nativeAppendFile(
    JNIEnv* env,
    jobject /* this */,
    jstring filePath,
    jstring content
) {
    const char* path = env->GetStringUTFChars(filePath, nullptr);
    const char* data = env->GetStringUTFChars(content, nullptr);
    
    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        LOGE("فشل فتح الملف للإضافة: %s", path);
        env->ReleaseStringUTFChars(filePath, path);
        env->ReleaseStringUTFChars(content, data);
        return JNI_FALSE;
    }
    
    file << data;
    file.close();
    
    LOGI("تم الإضافة للملف: %s", path);
    env->ReleaseStringUTFChars(filePath, path);
    env->ReleaseStringUTFChars(content, data);
    
    return JNI_TRUE;
}

/**
 * حذف ملف
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadStorage_nativeDeleteFile(
    JNIEnv* env,
    jobject /* this */,
    jstring filePath
) {
    const char* path = env->GetStringUTFChars(filePath, nullptr);
    
    int result = std::remove(path);
    
    if (result == 0) {
        LOGI("تم حذف الملف: %s", path);
    } else {
        LOGE("فشل حذف الملف: %s", path);
    }
    
    env->ReleaseStringUTFChars(filePath, path);
    
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

/**
 * فحص وجود ملف
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadStorage_nativeFileExists(
    JNIEnv* env,
    jobject /* this */,
    jstring filePath
) {
    const char* path = env->GetStringUTFChars(filePath, nullptr);
    
    std::ifstream file(path);
    bool exists = file.good();
    file.close();
    
    env->ReleaseStringUTFChars(filePath, path);
    
    return exists ? JNI_TRUE : JNI_FALSE;
}

/**
 * حجم الملف
 */
JNIEXPORT jlong JNICALL
Java_sad_lang_android_SadStorage_nativeFileSize(
    JNIEnv* env,
    jobject /* this */,
    jstring filePath
) {
    const char* path = env->GetStringUTFChars(filePath, nullptr);
    
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        env->ReleaseStringUTFChars(filePath, path);
        return -1;
    }
    
    long size = file.tellg();
    file.close();
    
    env->ReleaseStringUTFChars(filePath, path);
    
    return static_cast<jlong>(size);
}

} // extern "C"
