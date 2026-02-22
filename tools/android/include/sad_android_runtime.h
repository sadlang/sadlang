// ═══════════════════════════════════════════════════════════════════════════
// sad_android_runtime.h - نظام تشغيل لغة ص على Android
// ═══════════════════════════════════════════════════════════════════════════
//
// هذا هو المكون الرئيسي الذي يسمح بتشغيل كود ص مباشرة على Android
// بدون ترجمته إلى أي لغة أخرى.
//
// المعمارية:
// ┌─────────────────────────────────────────────────────────────────────────┐
// │                         تطبيق Android                                   │
// ├─────────────────────────────────────────────────────────────────────────┤
// │  SadActivity (Kotlin shell)                                             │
// │       ↓                                                                 │
// │  JNI Bridge                                                             │
// │       ↓                                                                 │
// │  ┌─────────────────────────────────────────────────────────────────┐   │
// │  │              Sad Android Runtime                                 │   │
// │  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │   │
// │  │  │   Lexer     │→ │   Parser    │→ │   Interpreter/VM        │  │   │
// │  │  └─────────────┘  └─────────────┘  └─────────────────────────┘  │   │
// │  │                          ↓                                       │   │
// │  │  ┌─────────────────────────────────────────────────────────────┐│   │
// │  │  │                 Android API Bindings                        ││   │
// │  │  │  UI | Camera | Location | Sensors | Storage | Network      ││   │
// │  │  └─────────────────────────────────────────────────────────────┘│   │
// │  └─────────────────────────────────────────────────────────────────┘   │
// └─────────────────────────────────────────────────────────────────────────┘
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef SAD_ANDROID_RUNTIME_H
#define SAD_ANDROID_RUNTIME_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

#ifdef SAD_ANDROID
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#endif

namespace sad {
namespace android {

// ═══════════════════════════════════════════════════════════════════════════
// تسجيل الأحداث / Logging
// ═══════════════════════════════════════════════════════════════════════════

#ifdef SAD_ANDROID
#define SAD_LOG_TAG "SadRuntime"
#define SAD_LOGI(...) __android_log_print(ANDROID_LOG_INFO, SAD_LOG_TAG, __VA_ARGS__)
#define SAD_LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, SAD_LOG_TAG, __VA_ARGS__)
#define SAD_LOGW(...) __android_log_print(ANDROID_LOG_WARN, SAD_LOG_TAG, __VA_ARGS__)
#define SAD_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, SAD_LOG_TAG, __VA_ARGS__)
#else
#define SAD_LOGI(...) printf(__VA_ARGS__)
#define SAD_LOGD(...) printf(__VA_ARGS__)
#define SAD_LOGW(...) printf(__VA_ARGS__)
#define SAD_LOGE(...) printf(__VA_ARGS__)
#endif

// ═══════════════════════════════════════════════════════════════════════════
// أنواع البيانات / Data Types
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief نتيجة تنفيذ كود ص
 */
struct ExecutionResult {
    bool success = false;
    std::string output;
    std::string error;
    int exitCode = 0;
};

/**
 * @brief إعدادات التطبيق
 */
struct AppConfig {
    std::string appName;          // اسم التطبيق
    std::string packageId;        // معرف الحزمة
    std::string version;          // الإصدار
    std::string mainScript;       // الملف الرئيسي
    int minSdk = 21;              // أقل SDK
    int targetSdk = 34;           // SDK المستهدف
    bool debugMode = false;       // وضع التصحيح
};

/**
 * @brief حالة الـ UI
 */
struct UIState {
    std::unordered_map<std::string, std::string> variables;
    std::vector<std::function<void()>> updateCallbacks;
};

// ═══════════════════════════════════════════════════════════════════════════
// الـ Runtime الرئيسي / Main Runtime
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class SadAndroidRuntime
 * @brief نظام تشغيل لغة ص على Android
 * 
 * هذا الكلاس يدير:
 * 1. تحميل وتحليل ملفات ص
 * 2. تنفيذ الكود
 * 3. التواصل مع Android APIs
 * 4. إدارة دورة حياة التطبيق
 */
class SadAndroidRuntime {
public:
    /**
     * @brief الحصول على instance وحيد
     */
    static SadAndroidRuntime& getInstance();
    
    // ═══════════════════════════════════════════════════════════════════════
    // التهيئة / Initialization
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تهيئة الـ runtime
     * @param env JNI environment
     * @param context Android context
     * @param assetManager مدير الأصول
     */
    bool initialize(
#ifdef SAD_ANDROID
        JNIEnv* env,
        jobject context,
        AAssetManager* assetManager
#endif
    );
    
    /**
     * @brief إيقاف الـ runtime
     */
    void shutdown();
    
    /**
     * @brief هل الـ runtime مُهيأ؟
     */
    bool isInitialized() const { return initialized_; }
    
    // ═══════════════════════════════════════════════════════════════════════
    // تنفيذ الكود / Code Execution
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تنفيذ ملف ص
     * @param filePath مسار الملف (داخل assets)
     */
    ExecutionResult executeFile(const std::string& filePath);
    
    /**
     * @brief تنفيذ كود ص مباشرة
     * @param code الكود كنص
     */
    ExecutionResult executeCode(const std::string& code);
    
    /**
     * @brief استدعاء دالة ص
     * @param functionName اسم الدالة
     * @param args المعاملات
     */
    ExecutionResult callFunction(
        const std::string& functionName,
        const std::vector<std::string>& args = {}
    );
    
    // ═══════════════════════════════════════════════════════════════════════
    // إدارة المتغيرات / Variable Management  
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تعيين متغير
     */
    void setVariable(const std::string& name, const std::string& value);
    
    /**
     * @brief الحصول على قيمة متغير
     */
    std::string getVariable(const std::string& name) const;
    
    /**
     * @brief هل المتغير موجود؟
     */
    bool hasVariable(const std::string& name) const;
    
    // ═══════════════════════════════════════════════════════════════════════
    // دورة حياة التطبيق / App Lifecycle
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief عند إنشاء التطبيق
     */
    void onCreate();
    
    /**
     * @brief عند بدء التطبيق
     */
    void onStart();
    
    /**
     * @brief عند استئناف التطبيق
     */
    void onResume();
    
    /**
     * @brief عند إيقاف التطبيق مؤقتاً
     */
    void onPause();
    
    /**
     * @brief عند إيقاف التطبيق
     */
    void onStop();
    
    /**
     * @brief عند تدمير التطبيق
     */
    void onDestroy();
    
    // ═══════════════════════════════════════════════════════════════════════
    // الأحداث / Events
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief معالجة حدث نقر
     */
    void onButtonClick(const std::string& buttonId);
    
    /**
     * @brief معالجة تغيير نص
     */
    void onTextChange(const std::string& inputId, const std::string& text);
    
    /**
     * @brief تسجيل callback لتحديث UI
     */
    void registerUICallback(std::function<void()> callback);
    
    // ═══════════════════════════════════════════════════════════════════════
    // الوصول إلى Android APIs
    // ═══════════════════════════════════════════════════════════════════════
    
#ifdef SAD_ANDROID
    JNIEnv* getJNIEnv() const { return env_; }
    jobject getContext() const { return context_; }
    AAssetManager* getAssetManager() const { return assetManager_; }
#endif

    /**
     * @brief إظهار Toast
     */
    void showToast(const std::string& message);
    
    /**
     * @brief إظهار Dialog
     */
    void showDialog(const std::string& title, const std::string& message);
    
    /**
     * @brief الانتقال إلى شاشة
     */
    void navigateTo(const std::string& screenName);

private:
    SadAndroidRuntime();
    ~SadAndroidRuntime();
    
    // منع النسخ
    SadAndroidRuntime(const SadAndroidRuntime&) = delete;
    SadAndroidRuntime& operator=(const SadAndroidRuntime&) = delete;
    
    // ═══════════════════════════════════════════════════════════════════════
    // الأعضاء الداخلية / Internal Members
    // ═══════════════════════════════════════════════════════════════════════
    
    bool initialized_ = false;
    AppConfig config_;
    UIState uiState_;
    
#ifdef SAD_ANDROID
    JNIEnv* env_ = nullptr;
    jobject context_ = nullptr;
    AAssetManager* assetManager_ = nullptr;
#endif
    
    // Interpreter state (forward declarations)
    struct InterpreterState;
    std::unique_ptr<InterpreterState> interpreter_;
    
    // ═══════════════════════════════════════════════════════════════════════
    // الدوال الداخلية / Internal Functions
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief قراءة ملف من assets
     */
    std::string readAssetFile(const std::string& path);
    
    /**
     * @brief تسجيل الدوال المضمنة
     */
    void registerBuiltinFunctions();
    
    /**
     * @brief تسجيل دوال Android
     */
    void registerAndroidFunctions();
};

// ═══════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تحويل jstring إلى std::string
 */
#ifdef SAD_ANDROID
std::string jstringToString(JNIEnv* env, jstring jstr);

/**
 * @brief تحويل std::string إلى jstring
 */
jstring stringToJstring(JNIEnv* env, const std::string& str);
#endif

} // namespace android
} // namespace sad

#endif // SAD_ANDROID_RUNTIME_H
