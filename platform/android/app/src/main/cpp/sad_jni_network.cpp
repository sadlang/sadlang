/**
 * =============================================================================
 * @file sad_jni_network.cpp
 * @brief (AR) جسر JNI للشبكة — HTTP و WebSocket
 * @brief (EN) JNI Network Bridge — HTTP and WebSocket
 * =============================================================================
 */

#include <jni.h>
#include <string>
#include <map>
#include <android/log.h>

#define LOG_TAG "SadNetwork"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Network callbacks stored to call back to Java
namespace {
    JavaVM* g_jvm = nullptr;
    std::map<int, jobject> activeCallbacks;
    int nextRequestId = 1;
}

extern "C" {

/**
 * حفظ JVM للاستخدام اللاحق
 */
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_jvm = vm;
    LOGI("تم تحميل مكتبة الشبكة");
    return JNI_VERSION_1_6;
}

/**
 * تهيئة الشبكة
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadNetwork_nativeInit(
    JNIEnv* env,
    jobject /* this */
) {
    LOGI("تهيئة نظام الشبكة...");
    activeCallbacks.clear();
    nextRequestId = 1;
    return JNI_TRUE;
}

/**
 * إرسال طلب HTTP GET
 */
JNIEXPORT jint JNICALL
Java_sad_lang_android_SadNetwork_nativeHttpGet(
    JNIEnv* env,
    jobject /* this */,
    jstring url,
    jobject callback
) {
    const char* urlStr = env->GetStringUTFChars(url, nullptr);
    
    int requestId = nextRequestId++;
    LOGI("HTTP GET [%d]: %s", requestId, urlStr);
    
    // Store global ref to callback
    if (callback != nullptr) {
        jobject globalCallback = env->NewGlobalRef(callback);
        activeCallbacks[requestId] = globalCallback;
    }
    
    // TODO: Implement actual HTTP request using socket or curl
    
    env->ReleaseStringUTFChars(url, urlStr);
    
    return requestId;
}

/**
 * إرسال طلب HTTP POST
 */
JNIEXPORT jint JNICALL
Java_sad_lang_android_SadNetwork_nativeHttpPost(
    JNIEnv* env,
    jobject /* this */,
    jstring url,
    jstring body,
    jstring contentType,
    jobject callback
) {
    const char* urlStr = env->GetStringUTFChars(url, nullptr);
    const char* bodyStr = env->GetStringUTFChars(body, nullptr);
    const char* contentTypeStr = env->GetStringUTFChars(contentType, nullptr);
    
    int requestId = nextRequestId++;
    LOGI("HTTP POST [%d]: %s", requestId, urlStr);
    
    // TODO: Implement actual HTTP POST
    
    env->ReleaseStringUTFChars(url, urlStr);
    env->ReleaseStringUTFChars(body, bodyStr);
    env->ReleaseStringUTFChars(contentType, contentTypeStr);
    
    return requestId;
}

/**
 * إلغاء طلب
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadNetwork_nativeCancelRequest(
    JNIEnv* env,
    jobject /* this */,
    jint requestId
) {
    LOGI("إلغاء الطلب: %d", requestId);
    
    auto it = activeCallbacks.find(requestId);
    if (it != activeCallbacks.end()) {
        env->DeleteGlobalRef(it->second);
        activeCallbacks.erase(it);
    }
}

/**
 * WebSocket: الاتصال
 */
JNIEXPORT jint JNICALL
Java_sad_lang_android_SadNetwork_nativeWebSocketConnect(
    JNIEnv* env,
    jobject /* this */,
    jstring url,
    jobject listener
) {
    const char* urlStr = env->GetStringUTFChars(url, nullptr);
    
    int connectionId = nextRequestId++;
    LOGI("WebSocket Connect [%d]: %s", connectionId, urlStr);
    
    // TODO: Implement WebSocket connection
    
    env->ReleaseStringUTFChars(url, urlStr);
    
    return connectionId;
}

/**
 * WebSocket: إرسال رسالة
 */
JNIEXPORT jboolean JNICALL
Java_sad_lang_android_SadNetwork_nativeWebSocketSend(
    JNIEnv* env,
    jobject /* this */,
    jint connectionId,
    jstring message
) {
    const char* msgStr = env->GetStringUTFChars(message, nullptr);
    
    LOGI("WebSocket Send [%d]: %s", connectionId, msgStr);
    
    // TODO: Send via WebSocket
    
    env->ReleaseStringUTFChars(message, msgStr);
    
    return JNI_TRUE;
}

/**
 * WebSocket: إغلاق الاتصال
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadNetwork_nativeWebSocketClose(
    JNIEnv* env,
    jobject /* this */,
    jint connectionId
) {
    LOGI("WebSocket Close: %d", connectionId);
    // TODO: Close WebSocket connection
}

/**
 * تنظيف الشبكة
 */
JNIEXPORT void JNICALL
Java_sad_lang_android_SadNetwork_nativeShutdown(
    JNIEnv* env,
    jobject /* this */
) {
    LOGI("إنهاء نظام الشبكة");
    
    // Cleanup all global refs
    for (auto& pair : activeCallbacks) {
        env->DeleteGlobalRef(pair.second);
    }
    activeCallbacks.clear();
}

} // extern "C"
