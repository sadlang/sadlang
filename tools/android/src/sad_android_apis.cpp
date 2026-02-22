// ═══════════════════════════════════════════════════════════════════════════
// sad_android_apis.cpp - تنفيذ واجهات برمجة أندرويد
// ═══════════════════════════════════════════════════════════════════════════

#include "sad_android_apis.h"

#ifdef SAD_ANDROID

#include <android/log.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <cmath>

#define TAG "SadAndroidAPIs"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

namespace sad {
namespace android {
namespace apis {

// ═══════════════════════════════════════════════════════════════════════════
// مساعدات JNI
// ═══════════════════════════════════════════════════════════════════════════

static std::string jstringToStdString(JNIEnv* env, jstring jstr) {
    if (!jstr) return "";
    const char* utf = env->GetStringUTFChars(jstr, nullptr);
    std::string result(utf);
    env->ReleaseStringUTFChars(jstr, utf);
    return result;
}

static jstring stdStringToJstring(JNIEnv* env, const std::string& str) {
    return env->NewStringUTF(str.c_str());
}

// ═══════════════════════════════════════════════════════════════════════════
// DeviceAPI
// ═══════════════════════════════════════════════════════════════════════════

DeviceInfo DeviceAPI::getDeviceInfo(JNIEnv* env, jobject context) {
    DeviceInfo info;
    
    // الحصول على Build fields
    jclass buildClass = env->FindClass("android/os/Build");
    
    // المُصنِّع
    jfieldID manufacturerField = env->GetStaticFieldID(buildClass, "MANUFACTURER", "Ljava/lang/String;");
    jstring manufacturer = (jstring)env->GetStaticObjectField(buildClass, manufacturerField);
    info.manufacturer = jstringToStdString(env, manufacturer);
    
    // الموديل
    jfieldID modelField = env->GetStaticFieldID(buildClass, "MODEL", "Ljava/lang/String;");
    jstring model = (jstring)env->GetStaticObjectField(buildClass, modelField);
    info.model = jstringToStdString(env, model);
    
    // إصدار أندرويد
    jclass versionClass = env->FindClass("android/os/Build$VERSION");
    jfieldID releaseField = env->GetStaticFieldID(versionClass, "RELEASE", "Ljava/lang/String;");
    jstring release = (jstring)env->GetStaticObjectField(versionClass, releaseField);
    info.androidVersion = jstringToStdString(env, release);
    
    // SDK level
    jfieldID sdkField = env->GetStaticFieldID(versionClass, "SDK_INT", "I");
    info.sdkVersion = env->GetStaticIntField(versionClass, sdkField);
    
    // اللغة والبلد
    jclass localeClass = env->FindClass("java/util/Locale");
    jmethodID getDefaultMethod = env->GetStaticMethodID(localeClass, "getDefault", "()Ljava/util/Locale;");
    jobject locale = env->CallStaticObjectMethod(localeClass, getDefaultMethod);
    
    jmethodID getLangMethod = env->GetMethodID(localeClass, "getLanguage", "()Ljava/lang/String;");
    jstring lang = (jstring)env->CallObjectMethod(locale, getLangMethod);
    info.language = jstringToStdString(env, lang);
    
    jmethodID getCountryMethod = env->GetMethodID(localeClass, "getCountry", "()Ljava/lang/String;");
    jstring country = (jstring)env->CallObjectMethod(locale, getCountryMethod);
    info.country = jstringToStdString(env, country);
    
    // أبعاد الشاشة
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getResourcesMethod = env->GetMethodID(contextClass, "getResources", 
        "()Landroid/content/res/Resources;");
    jobject resources = env->CallObjectMethod(context, getResourcesMethod);
    
    jclass resourcesClass = env->GetObjectClass(resources);
    jmethodID getDisplayMetricsMethod = env->GetMethodID(resourcesClass, "getDisplayMetrics",
        "()Landroid/util/DisplayMetrics;");
    jobject metrics = env->CallObjectMethod(resources, getDisplayMetricsMethod);
    
    jclass metricsClass = env->GetObjectClass(metrics);
    jfieldID widthField = env->GetFieldID(metricsClass, "widthPixels", "I");
    jfieldID heightField = env->GetFieldID(metricsClass, "heightPixels", "I");
    jfieldID densityField = env->GetFieldID(metricsClass, "density", "F");
    
    info.screenWidth = env->GetIntField(metrics, widthField);
    info.screenHeight = env->GetIntField(metrics, heightField);
    info.screenDensity = env->GetFloatField(metrics, densityField);
    
    return info;
}

int DeviceAPI::getBatteryLevel(JNIEnv* env, jobject context) {
    // استخدام BatteryManager
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getSystemServiceMethod = env->GetMethodID(contextClass, "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;");
    
    jstring batteryService = stdStringToJstring(env, "batterymanager");
    jobject batteryManager = env->CallObjectMethod(context, getSystemServiceMethod, batteryService);
    
    if (!batteryManager) return -1;
    
    jclass bmClass = env->GetObjectClass(batteryManager);
    jmethodID getIntProperty = env->GetMethodID(bmClass, "getIntProperty", "(I)I");
    
    // BATTERY_PROPERTY_CAPACITY = 4
    return env->CallIntMethod(batteryManager, getIntProperty, 4);
}

// ═══════════════════════════════════════════════════════════════════════════
// FileSystemAPI
// ═══════════════════════════════════════════════════════════════════════════

std::string FileSystemAPI::getInternalDir(JNIEnv* env, jobject context) {
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getFilesDir = env->GetMethodID(contextClass, "getFilesDir", "()Ljava/io/File;");
    jobject filesDir = env->CallObjectMethod(context, getFilesDir);
    
    jclass fileClass = env->GetObjectClass(filesDir);
    jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring path = (jstring)env->CallObjectMethod(filesDir, getAbsolutePath);
    
    return jstringToStdString(env, path);
}

std::string FileSystemAPI::getExternalDir(JNIEnv* env, jobject context) {
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getExternalFilesDir = env->GetMethodID(contextClass, "getExternalFilesDir",
        "(Ljava/lang/String;)Ljava/io/File;");
    jobject dir = env->CallObjectMethod(context, getExternalFilesDir, nullptr);
    
    if (!dir) return "";
    
    jclass fileClass = env->GetObjectClass(dir);
    jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring path = (jstring)env->CallObjectMethod(dir, getAbsolutePath);
    
    return jstringToStdString(env, path);
}

std::string FileSystemAPI::getCacheDir(JNIEnv* env, jobject context) {
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getCacheDir = env->GetMethodID(contextClass, "getCacheDir", "()Ljava/io/File;");
    jobject cacheDir = env->CallObjectMethod(context, getCacheDir);
    
    jclass fileClass = env->GetObjectClass(cacheDir);
    jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring path = (jstring)env->CallObjectMethod(cacheDir, getAbsolutePath);
    
    return jstringToStdString(env, path);
}

bool FileSystemAPI::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool FileSystemAPI::createDirectory(const std::string& path) {
    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
}

bool FileSystemAPI::deleteFile(const std::string& path) {
    return remove(path.c_str()) == 0;
}

std::string FileSystemAPI::readTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystemAPI::writeTextFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << content;
    return true;
}

std::vector<std::string> FileSystemAPI::listDirectory(const std::string& path) {
    std::vector<std::string> result;
    DIR* dir = opendir(path.c_str());
    if (!dir) return result;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name != "." && name != "..") {
            result.push_back(name);
        }
    }
    closedir(dir);
    return result;
}

long long FileSystemAPI::getFileSize(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) return -1;
    return buffer.st_size;
}

// ═══════════════════════════════════════════════════════════════════════════
// StorageAPI (SharedPreferences)
// ═══════════════════════════════════════════════════════════════════════════

static jobject getSharedPreferences(JNIEnv* env, jobject context) {
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getPrefs = env->GetMethodID(contextClass, "getSharedPreferences",
        "(Ljava/lang/String;I)Landroid/content/SharedPreferences;");
    jstring prefsName = stdStringToJstring(env, "sad_prefs");
    return env->CallObjectMethod(context, getPrefs, prefsName, 0);
}

static jobject getPrefsEditor(JNIEnv* env, jobject prefs) {
    jclass prefsClass = env->GetObjectClass(prefs);
    jmethodID editMethod = env->GetMethodID(prefsClass, "edit",
        "()Landroid/content/SharedPreferences$Editor;");
    return env->CallObjectMethod(prefs, editMethod);
}

void StorageAPI::saveString(JNIEnv* env, jobject context,
                           const std::string& key, const std::string& value) {
    jobject prefs = getSharedPreferences(env, context);
    jobject editor = getPrefsEditor(env, prefs);
    
    jclass editorClass = env->GetObjectClass(editor);
    jmethodID putString = env->GetMethodID(editorClass, "putString",
        "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;");
    jmethodID apply = env->GetMethodID(editorClass, "apply", "()V");
    
    env->CallObjectMethod(editor, putString, 
        stdStringToJstring(env, key), stdStringToJstring(env, value));
    env->CallVoidMethod(editor, apply);
}

std::string StorageAPI::getString(JNIEnv* env, jobject context,
                                 const std::string& key, const std::string& defaultValue) {
    jobject prefs = getSharedPreferences(env, context);
    jclass prefsClass = env->GetObjectClass(prefs);
    jmethodID getString = env->GetMethodID(prefsClass, "getString",
        "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
    
    jstring result = (jstring)env->CallObjectMethod(prefs, getString,
        stdStringToJstring(env, key), stdStringToJstring(env, defaultValue));
    
    return jstringToStdString(env, result);
}

void StorageAPI::saveInt(JNIEnv* env, jobject context, const std::string& key, int value) {
    jobject prefs = getSharedPreferences(env, context);
    jobject editor = getPrefsEditor(env, prefs);
    
    jclass editorClass = env->GetObjectClass(editor);
    jmethodID putInt = env->GetMethodID(editorClass, "putInt",
        "(Ljava/lang/String;I)Landroid/content/SharedPreferences$Editor;");
    jmethodID apply = env->GetMethodID(editorClass, "apply", "()V");
    
    env->CallObjectMethod(editor, putInt, stdStringToJstring(env, key), value);
    env->CallVoidMethod(editor, apply);
}

int StorageAPI::getInt(JNIEnv* env, jobject context, const std::string& key, int defaultValue) {
    jobject prefs = getSharedPreferences(env, context);
    jclass prefsClass = env->GetObjectClass(prefs);
    jmethodID getInt = env->GetMethodID(prefsClass, "getInt",
        "(Ljava/lang/String;I)I");
    
    return env->CallIntMethod(prefs, getInt, stdStringToJstring(env, key), defaultValue);
}

void StorageAPI::saveBool(JNIEnv* env, jobject context, const std::string& key, bool value) {
    jobject prefs = getSharedPreferences(env, context);
    jobject editor = getPrefsEditor(env, prefs);
    
    jclass editorClass = env->GetObjectClass(editor);
    jmethodID putBool = env->GetMethodID(editorClass, "putBoolean",
        "(Ljava/lang/String;Z)Landroid/content/SharedPreferences$Editor;");
    jmethodID apply = env->GetMethodID(editorClass, "apply", "()V");
    
    env->CallObjectMethod(editor, putBool, stdStringToJstring(env, key), value);
    env->CallVoidMethod(editor, apply);
}

bool StorageAPI::getBool(JNIEnv* env, jobject context, const std::string& key, bool defaultValue) {
    jobject prefs = getSharedPreferences(env, context);
    jclass prefsClass = env->GetObjectClass(prefs);
    jmethodID getBool = env->GetMethodID(prefsClass, "getBoolean",
        "(Ljava/lang/String;Z)Z");
    
    return env->CallBooleanMethod(prefs, getBool, stdStringToJstring(env, key), defaultValue);
}

bool StorageAPI::contains(JNIEnv* env, jobject context, const std::string& key) {
    jobject prefs = getSharedPreferences(env, context);
    jclass prefsClass = env->GetObjectClass(prefs);
    jmethodID contains = env->GetMethodID(prefsClass, "contains",
        "(Ljava/lang/String;)Z");
    
    return env->CallBooleanMethod(prefs, contains, stdStringToJstring(env, key));
}

void StorageAPI::remove(JNIEnv* env, jobject context, const std::string& key) {
    jobject prefs = getSharedPreferences(env, context);
    jobject editor = getPrefsEditor(env, prefs);
    
    jclass editorClass = env->GetObjectClass(editor);
    jmethodID removeMethod = env->GetMethodID(editorClass, "remove",
        "(Ljava/lang/String;)Landroid/content/SharedPreferences$Editor;");
    jmethodID apply = env->GetMethodID(editorClass, "apply", "()V");
    
    env->CallObjectMethod(editor, removeMethod, stdStringToJstring(env, key));
    env->CallVoidMethod(editor, apply);
}

void StorageAPI::clear(JNIEnv* env, jobject context) {
    jobject prefs = getSharedPreferences(env, context);
    jobject editor = getPrefsEditor(env, prefs);
    
    jclass editorClass = env->GetObjectClass(editor);
    jmethodID clearMethod = env->GetMethodID(editorClass, "clear",
        "()Landroid/content/SharedPreferences$Editor;");
    jmethodID apply = env->GetMethodID(editorClass, "apply", "()V");
    
    env->CallObjectMethod(editor, clearMethod);
    env->CallVoidMethod(editor, apply);
}

// ═══════════════════════════════════════════════════════════════════════════
// LocationAPI
// ═══════════════════════════════════════════════════════════════════════════

double LocationAPI::distanceBetween(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000; // نصف قطر الأرض بالأمتار
    
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon / 2) * sin(dLon / 2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return R * c;
}

// ═══════════════════════════════════════════════════════════════════════════
// ClipboardAPI
// ═══════════════════════════════════════════════════════════════════════════

void ClipboardAPI::copyText(JNIEnv* env, jobject context, const std::string& text) {
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getSystemService = env->GetMethodID(contextClass, "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;");
    
    jstring clipboardService = stdStringToJstring(env, "clipboard");
    jobject clipboard = env->CallObjectMethod(context, getSystemService, clipboardService);
    
    jclass clipDataClass = env->FindClass("android/content/ClipData");
    jmethodID newPlainText = env->GetStaticMethodID(clipDataClass, "newPlainText",
        "(Ljava/lang/CharSequence;Ljava/lang/CharSequence;)Landroid/content/ClipData;");
    
    jstring label = stdStringToJstring(env, "text");
    jstring content = stdStringToJstring(env, text);
    jobject clipData = env->CallStaticObjectMethod(clipDataClass, newPlainText, label, content);
    
    jclass cbClass = env->GetObjectClass(clipboard);
    jmethodID setPrimary = env->GetMethodID(cbClass, "setPrimaryClip",
        "(Landroid/content/ClipData;)V");
    env->CallVoidMethod(clipboard, setPrimary, clipData);
}

std::string ClipboardAPI::pasteText(JNIEnv* env, jobject context) {
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getSystemService = env->GetMethodID(contextClass, "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;");
    
    jstring clipboardService = stdStringToJstring(env, "clipboard");
    jobject clipboard = env->CallObjectMethod(context, getSystemService, clipboardService);
    
    jclass cbClass = env->GetObjectClass(clipboard);
    jmethodID hasPrimaryClip = env->GetMethodID(cbClass, "hasPrimaryClip", "()Z");
    
    if (!env->CallBooleanMethod(clipboard, hasPrimaryClip)) {
        return "";
    }
    
    jmethodID getPrimaryClip = env->GetMethodID(cbClass, "getPrimaryClip",
        "()Landroid/content/ClipData;");
    jobject clipData = env->CallObjectMethod(clipboard, getPrimaryClip);
    
    jclass clipDataClass = env->GetObjectClass(clipData);
    jmethodID getItemAt = env->GetMethodID(clipDataClass, "getItemAt",
        "(I)Landroid/content/ClipData$Item;");
    jobject item = env->CallObjectMethod(clipData, getItemAt, 0);
    
    jclass itemClass = env->GetObjectClass(item);
    jmethodID getText = env->GetMethodID(itemClass, "getText", "()Ljava/lang/CharSequence;");
    jobject text = env->CallObjectMethod(item, getText);
    
    jclass charSeqClass = env->FindClass("java/lang/CharSequence");
    jmethodID toString = env->GetMethodID(charSeqClass, "toString", "()Ljava/lang/String;");
    jstring result = (jstring)env->CallObjectMethod(text, toString);
    
    return jstringToStdString(env, result);
}

// ═══════════════════════════════════════════════════════════════════════════
// VibrationAPI
// ═══════════════════════════════════════════════════════════════════════════

bool VibrationAPI::hasVibrator(JNIEnv* env, jobject context) {
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getSystemService = env->GetMethodID(contextClass, "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;");
    
    jstring vibratorService = stdStringToJstring(env, "vibrator");
    jobject vibrator = env->CallObjectMethod(context, getSystemService, vibratorService);
    
    jclass vibratorClass = env->GetObjectClass(vibrator);
    jmethodID hasVibrator = env->GetMethodID(vibratorClass, "hasVibrator", "()Z");
    
    return env->CallBooleanMethod(vibrator, hasVibrator);
}

void VibrationAPI::vibrate(JNIEnv* env, jobject context, int milliseconds) {
    jclass contextClass = env->GetObjectClass(context);
    jmethodID getSystemService = env->GetMethodID(contextClass, "getSystemService",
        "(Ljava/lang/String;)Ljava/lang/Object;");
    
    jstring vibratorService = stdStringToJstring(env, "vibrator");
    jobject vibrator = env->CallObjectMethod(context, getSystemService, vibratorService);
    
    jclass vibratorClass = env->GetObjectClass(vibrator);
    jmethodID vibrateMethod = env->GetMethodID(vibratorClass, "vibrate", "(J)V");
    
    env->CallVoidMethod(vibrator, vibrateMethod, (jlong)milliseconds);
}

// ═══════════════════════════════════════════════════════════════════════════
// IntentAPI
// ═══════════════════════════════════════════════════════════════════════════

void IntentAPI::openUrl(JNIEnv* env, jobject context, const std::string& url) {
    jclass uriClass = env->FindClass("android/net/Uri");
    jmethodID parse = env->GetStaticMethodID(uriClass, "parse",
        "(Ljava/lang/String;)Landroid/net/Uri;");
    jobject uri = env->CallStaticObjectMethod(uriClass, parse, stdStringToJstring(env, url));
    
    jclass intentClass = env->FindClass("android/content/Intent");
    jmethodID intentInit = env->GetMethodID(intentClass, "<init>",
        "(Ljava/lang/String;Landroid/net/Uri;)V");
    
    jstring actionView = stdStringToJstring(env, "android.intent.action.VIEW");
    jobject intent = env->NewObject(intentClass, intentInit, actionView, uri);
    
    jclass contextClass = env->GetObjectClass(context);
    jmethodID startActivity = env->GetMethodID(contextClass, "startActivity",
        "(Landroid/content/Intent;)V");
    env->CallVoidMethod(context, startActivity, intent);
}

void IntentAPI::openEmail(JNIEnv* env, jobject context,
                         const std::string& to,
                         const std::string& subject,
                         const std::string& body) {
    std::string mailto = "mailto:" + to + "?subject=" + subject + "&body=" + body;
    openUrl(env, context, mailto);
}

void IntentAPI::openPhone(JNIEnv* env, jobject context, const std::string& number) {
    std::string tel = "tel:" + number;
    openUrl(env, context, tel);
}

void IntentAPI::openMaps(JNIEnv* env, jobject context, double lat, double lon) {
    std::ostringstream ss;
    ss << "geo:" << lat << "," << lon;
    openUrl(env, context, ss.str());
}

} // namespace apis
} // namespace android
} // namespace sad

#endif // SAD_ANDROID
