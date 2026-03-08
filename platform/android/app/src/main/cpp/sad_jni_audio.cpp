/**
 * =============================================================================
 * @file sad_jni_audio.cpp
 * @brief (AR) جسر JNI للصوت — تشغيل وتسجيل
 * @brief (EN) JNI Audio Bridge — Playback and Recording
 * =============================================================================
 */

#include <jni.h>
#include <string>
#include <map>
#include <android/log.h>

#define LOG_TAG "SadAudio"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ═══════════════════════════════════════════════════════════════════════════════
//  Audio State
// ═══════════════════════════════════════════════════════════════════════════════

namespace {
    std::map<int, std::string> loadedSounds;
    int nextSoundId = 1;
    bool isRecording = false;
    float currentVolume = 1.0f;
}

extern "C" {

/**
 * تهيئة نظام الصوت
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadAudio_nativeInit(
    JNIEnv* env,
    jobject /* this */
) {
    LOGI("تهيئة نظام الصوت...");
    loadedSounds.clear();
    nextSoundId = 1;
    isRecording = false;
    currentVolume = 1.0f;
    return JNI_TRUE;
}

/**
 * تحميل ملف صوتي
 */
JNIEXPORT jint JNICALL
Java_sad_lang_android_SadAudio_nativeLoadSound(
    JNIEnv* env,
    jobject /* this */,
    jstring filePath
) {
    const char* path = env->GetStringUTFChars(filePath, nullptr);
    
    int soundId = nextSoundId++;
    loadedSounds[soundId] = path;
    
    LOGI("تم تحميل الصوت [%d]: %s", soundId, path);
    
    env->ReleaseStringUTFChars(filePath, path);
    
    return soundId;
}

/**
 * تشغيل صوت
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadAudio_nativePlaySound(
    JNIEnv* env,
    jobject /* this */,
    jint soundId,
    jboolean loop
) {
    auto it = loadedSounds.find(soundId);
    if (it == loadedSounds.end()) {
        LOGE("الصوت غير موجود: %d", soundId);
        return JNI_FALSE;
    }
    
    LOGI("تشغيل الصوت [%d]: %s (تكرار: %d)", soundId, it->second.c_str(), loop);
    
    // TODO: Use OpenSL ES or AAudio for actual playback
    
    return JNI_TRUE;
}

/**
 * إيقاف صوت
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadAudio_nativeStopSound(
    JNIEnv* env,
    jobject /* this */,
    jint soundId
) {
    LOGI("إيقاف الصوت: %d", soundId);
    // TODO: Stop actual playback
}

/**
 * إيقاف جميع الأصوات
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadAudio_nativeStopAll(
    JNIEnv* env,
    jobject /* this */
) {
    LOGI("إيقاف جميع الأصوات");
    // TODO: Stop all sounds
}

/**
 * تعيين مستوى الصوت
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadAudio_nativeSetVolume(
    JNIEnv* env,
    jobject /* this */,
    jfloat volume
) {
    currentVolume = volume;
    LOGI("مستوى الصوت: %.2f", volume);
}

/**
 * بدء التسجيل
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadAudio_nativeStartRecording(
    JNIEnv* env,
    jobject /* this */,
    jstring outputPath
) {
    if (isRecording) {
        LOGE("التسجيل قيد التشغيل بالفعل");
        return JNI_FALSE;
    }
    
    const char* path = env->GetStringUTFChars(outputPath, nullptr);
    LOGI("بدء التسجيل: %s", path);
    
    isRecording = true;
    
    env->ReleaseStringUTFChars(outputPath, path);
    
    return JNI_TRUE;
}

/**
 * إيقاف التسجيل
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadAudio_nativeStopRecording(
    JNIEnv* env,
    jobject /* this */
) {
    if (!isRecording) {
        LOGE("لا يوجد تسجيل جاري");
        return JNI_FALSE;
    }
    
    LOGI("إيقاف التسجيل");
    isRecording = false;
    
    return JNI_TRUE;
}

/**
 * تحرير صوت
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadAudio_nativeReleaseSound(
    JNIEnv* env,
    jobject /* this */,
    jint soundId
) {
    auto it = loadedSounds.find(soundId);
    if (it != loadedSounds.end()) {
        LOGI("تحرير الصوت: %d", soundId);
        loadedSounds.erase(it);
    }
}

/**
 * تنظيف نظام الصوت
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadAudio_nativeShutdown(
    JNIEnv* env,
    jobject /* this */
) {
    LOGI("إنهاء نظام الصوت");
    loadedSounds.clear();
    isRecording = false;
}

} // extern "C"
