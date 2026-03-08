// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  JNI Bridge — جسر Java ⟷ C++
//  Sad Language Android Native Bridge
// ═══════════════════════════════════════════════════════════════════════════════

#include <jni.h>
#include <string>
#include <android/log.h>

// لغة ص Includes
// #include "interpreter_new/include/core/interpreter_core.h"
// #include "shared/lexer/include/lexer.h"
// #include "shared/parser/include/parser.h"

#define LOG_TAG "SadNotes"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ═══════════════════════════════════════════════════════════════════════════════
//  التهيئة
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT jboolean JNICALL
Java_com_sad_notes_SadEngine_nativeInit(
    JNIEnv* env,
    jobject /* this */
) {
    LOGI("تهيئة محرك لغة ص...");
    
    // TODO: تهيئة المفسر
    // Sad::Interpreter::InterpreterCore::getInstance().initialize();
    
    LOGI("تم تهيئة محرك لغة ص بنجاح");
    return JNI_TRUE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_sad_notes_SadEngine_nativeShutdown(
    JNIEnv* env,
    jobject /* this */
) {
    LOGI("إنهاء محرك لغة ص...");
    
    // TODO: تنظيف الموارد
    
    LOGI("تم إنهاء محرك لغة ص");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تنفيذ الكود
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT jstring JNICALL
Java_com_sad_notes_SadEngine_nativeEval(
    JNIEnv* env,
    jobject /* this */,
    jstring code
) {
    const char* codeStr = env->GetStringUTFChars(code, nullptr);
    
    LOGI("تنفيذ كود: %s", codeStr);
    
    // TODO: تنفيذ الكود فعلياً
    // try {
    //     auto result = Sad::Interpreter::InterpreterCore::getInstance().evaluate(codeStr);
    //     return env->NewStringUTF(result.toString().c_str());
    // } catch (const std::exception& e) {
    //     LOGE("خطأ: %s", e.what());
    //     return env->NewStringUTF(e.what());
    // }
    
    std::string result = "نتيجة تنفيذ: ";
    result += codeStr;
    
    env->ReleaseStringUTFChars(code, codeStr);
    
    return env->NewStringUTF(result.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_sad_notes_SadEngine_nativeEvalFile(
    JNIEnv* env,
    jobject /* this */,
    jstring filepath
) {
    const char* path = env->GetStringUTFChars(filepath, nullptr);
    
    LOGI("تنفيذ ملف: %s", path);
    
    // TODO: قراءة وتنفيذ الملف
    
    std::string result = "تم تنفيذ الملف: ";
    result += path;
    
    env->ReleaseStringUTFChars(filepath, path);
    
    return env->NewStringUTF(result.c_str());
}

// ═══════════════════════════════════════════════════════════════════════════════
//  استدعاء الدوال
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT jstring JNICALL
Java_com_sad_notes_SadEngine_nativeCall(
    JNIEnv* env,
    jobject /* this */,
    jstring functionName,
    jobjectArray args
) {
    const char* funcName = env->GetStringUTFChars(functionName, nullptr);
    
    LOGI("استدعاء دالة: %s", funcName);
    
    // جمع المعاملات
    int argc = env->GetArrayLength(args);
    std::vector<std::string> argValues;
    
    for (int i = 0; i < argc; i++) {
        jstring arg = (jstring) env->GetObjectArrayElement(args, i);
        const char* argStr = env->GetStringUTFChars(arg, nullptr);
        argValues.push_back(argStr);
        env->ReleaseStringUTFChars(arg, argStr);
    }
    
    // TODO: استدعاء الدالة فعلياً
    
    std::string result = "نتيجة استدعاء ";
    result += funcName;
    result += " مع ";
    result += std::to_string(argc);
    result += " معاملات";
    
    env->ReleaseStringUTFChars(functionName, funcName);
    
    return env->NewStringUTF(result.c_str());
}

// ═══════════════════════════════════════════════════════════════════════════════
//  معلومات النسخة
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT jstring JNICALL
Java_com_sad_notes_SadEngine_nativeGetVersion(
    JNIEnv* env,
    jobject /* this */
) {
    return env->NewStringUTF("لغة ص 2.0.0");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال UI المساعدة
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" JNIEXPORT jint JNICALL
Java_com_sad_notes_SadEngine_nativeParseColor(
    JNIEnv* env,
    jobject /* this */,
    jstring colorHex
) {
    const char* hex = env->GetStringUTFChars(colorHex, nullptr);
    
    // تحويل #RRGGBB إلى int
    std::string hexStr = hex;
    if (hexStr[0] == '#') {
        hexStr = hexStr.substr(1);
    }
    
    int color = (int) strtol(hexStr.c_str(), nullptr, 16);
    color |= 0xFF000000; // إضافة alpha
    
    env->ReleaseStringUTFChars(colorHex, hex);
    
    return color;
}
