// ═══════════════════════════════════════════════════════════════════════════
// sad_jni_bridge.cpp - تنفيذ جسر JNI
// ═══════════════════════════════════════════════════════════════════════════

#include "sad_jni_bridge.h"
#include "sad_android_runtime.h"

#ifdef SAD_ANDROID

#include <android/asset_manager_jni.h>

using namespace sad::android;

// متغير عام لـ JavaVM
static JavaVM* g_javaVM = nullptr;

extern "C" {

// ═══════════════════════════════════════════════════════════════════════════
// تحميل/تفريغ المكتبة
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_javaVM = vm;
    SAD_LOGI("JNI_OnLoad: تم تحميل مكتبة لغة ص");
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    SAD_LOGI("JNI_OnUnload: تم تفريغ مكتبة لغة ص");
    g_javaVM = nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// تهيئة الـ Runtime
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT jboolean JNICALL
Java_sad_android_runtime_SadRuntime_nativeInit(
    JNIEnv* env,
    jobject thiz,
    jobject context,
    jobject assetManager
) {
    SAD_LOGI("nativeInit: بدء تهيئة Runtime...");
    
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    if (!mgr) {
        SAD_LOGE("nativeInit: فشل الحصول على AssetManager");
        return JNI_FALSE;
    }
    
    bool success = SadAndroidRuntime::getInstance().initialize(env, context, mgr);
    
    return success ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadRuntime_nativeShutdown(
    JNIEnv* env,
    jobject thiz
) {
    SAD_LOGI("nativeShutdown: إيقاف Runtime...");
    SadAndroidRuntime::getInstance().shutdown();
}

// ═══════════════════════════════════════════════════════════════════════════
// تنفيذ الكود
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeExecuteFile(
    JNIEnv* env,
    jobject thiz,
    jstring filePath
) {
    std::string path = jstringToString(env, filePath);
    SAD_LOGI("nativeExecuteFile: %s", path.c_str());
    
    auto result = SadAndroidRuntime::getInstance().executeFile(path);
    
    std::string response;
    if (result.success) {
        response = "{\"success\":true,\"output\":\"" + result.output + "\"}";
    } else {
        response = "{\"success\":false,\"error\":\"" + result.error + "\"}";
    }
    
    return stringToJstring(env, response);
}

JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeExecuteCode(
    JNIEnv* env,
    jobject thiz,
    jstring code
) {
    std::string codeStr = jstringToString(env, code);
    SAD_LOGD("nativeExecuteCode: %zu chars", codeStr.size());
    
    auto result = SadAndroidRuntime::getInstance().executeCode(codeStr);
    
    std::string response;
    if (result.success) {
        response = "{\"success\":true,\"output\":\"" + result.output + "\"}";
    } else {
        response = "{\"success\":false,\"error\":\"" + result.error + "\"}";
    }
    
    return stringToJstring(env, response);
}

JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeCallFunction(
    JNIEnv* env,
    jobject thiz,
    jstring functionName,
    jobjectArray args
) {
    std::string funcName = jstringToString(env, functionName);
    
    std::vector<std::string> argList;
    if (args) {
        int len = env->GetArrayLength(args);
        for (int i = 0; i < len; i++) {
            jstring jarg = (jstring)env->GetObjectArrayElement(args, i);
            argList.push_back(jstringToString(env, jarg));
            env->DeleteLocalRef(jarg);
        }
    }
    
    auto result = SadAndroidRuntime::getInstance().callFunction(funcName, argList);
    
    std::string response;
    if (result.success) {
        response = "{\"success\":true,\"output\":\"" + result.output + "\"}";
    } else {
        response = "{\"success\":false,\"error\":\"" + result.error + "\"}";
    }
    
    return stringToJstring(env, response);
}

// ═══════════════════════════════════════════════════════════════════════════
// دورة حياة Activity
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnCreate(JNIEnv* env, jobject thiz) {
    SadAndroidRuntime::getInstance().onCreate();
}

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnStart(JNIEnv* env, jobject thiz) {
    SadAndroidRuntime::getInstance().onStart();
}

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnResume(JNIEnv* env, jobject thiz) {
    SadAndroidRuntime::getInstance().onResume();
}

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnPause(JNIEnv* env, jobject thiz) {
    SadAndroidRuntime::getInstance().onPause();
}

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnStop(JNIEnv* env, jobject thiz) {
    SadAndroidRuntime::getInstance().onStop();
}

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnDestroy(JNIEnv* env, jobject thiz) {
    SadAndroidRuntime::getInstance().onDestroy();
}

// ═══════════════════════════════════════════════════════════════════════════
// أحداث UI
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnButtonClick(
    JNIEnv* env,
    jobject thiz,
    jstring buttonId
) {
    std::string id = jstringToString(env, buttonId);
    SadAndroidRuntime::getInstance().onButtonClick(id);
}

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadActivity_nativeOnTextChange(
    JNIEnv* env,
    jobject thiz,
    jstring inputId,
    jstring text
) {
    std::string id = jstringToString(env, inputId);
    std::string txt = jstringToString(env, text);
    SadAndroidRuntime::getInstance().onTextChange(id, txt);
}

// ═══════════════════════════════════════════════════════════════════════════
// إدارة المتغيرات
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadRuntime_nativeSetVariable(
    JNIEnv* env,
    jobject thiz,
    jstring name,
    jstring value
) {
    std::string n = jstringToString(env, name);
    std::string v = jstringToString(env, value);
    SadAndroidRuntime::getInstance().setVariable(n, v);
}

JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeGetVariable(
    JNIEnv* env,
    jobject thiz,
    jstring name
) {
    std::string n = jstringToString(env, name);
    std::string value = SadAndroidRuntime::getInstance().getVariable(n);
    return stringToJstring(env, value);
}

// ═══════════════════════════════════════════════════════════════════════════
// UI Description
// ═══════════════════════════════════════════════════════════════════════════

JNIEXPORT jstring JNICALL
Java_sad_android_runtime_SadRuntime_nativeGetUIDescription(
    JNIEnv* env,
    jobject thiz
) {
    // إرجاع وصف UI بتنسيق JSON
    // سيتم تحويله إلى Compose UI في Kotlin
    std::string uiJson = R"({
        "type": "column",
        "align": "center",
        "children": [
            {"type": "text", "content": "مرحباً من لغة ص!", "size": 28},
            {"type": "spacer", "height": 20},
            {"type": "button", "id": "btn1", "label": "اضغط هنا"}
        ]
    })";
    
    return stringToJstring(env, uiJson);
}

JNIEXPORT void JNICALL
Java_sad_android_runtime_SadRuntime_nativeUpdateUI(
    JNIEnv* env,
    jobject thiz,
    jstring uiJson
) {
    std::string json = jstringToString(env, uiJson);
    // TODO: تحديث UI بناءً على JSON
    SAD_LOGD("updateUI: %s", json.c_str());
}

} // extern "C"

#endif // SAD_ANDROID
