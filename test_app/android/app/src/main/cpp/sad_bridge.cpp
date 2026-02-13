// ╔════════════════════════════════════════════════════════════════════╗
// ║       جسر JNI للربط بين Java والكود الأصلي من لغة ص               ║
// ╚════════════════════════════════════════════════════════════════════╝

#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "SadApp"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" {

// دالة تهيئة runtime لغة ص
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("تم تحميل مكتبة لغة ص");
    return JNI_VERSION_1_6;
}

} // extern "C"
