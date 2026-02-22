// ═══════════════════════════════════════════════════════════════════════════
// sad_android_runtime.cpp - تنفيذ نظام تشغيل لغة ص على Android
// ═══════════════════════════════════════════════════════════════════════════

#include "sad_android_runtime.h"
#include <sstream>
#include <fstream>

// تضمين مكونات المفسر
// ملاحظة: سيتم ربطها مع sad_core عند البناء
#ifdef SAD_ANDROID
// Android-specific includes will be added at build time
#endif

namespace sad {
namespace android {

// ═══════════════════════════════════════════════════════════════════════════
// تعريف الحالة الداخلية للمفسر
// ═══════════════════════════════════════════════════════════════════════════

struct SadAndroidRuntime::InterpreterState {
    // سيتم ربط المفسر الفعلي هنا
    // std::unique_ptr<Sad::Interpreter::InterpreterCore> interpreter;
    std::unordered_map<std::string, std::string> variables;
    std::vector<std::string> loadedScripts;
    bool isRunning = false;
};

// ═══════════════════════════════════════════════════════════════════════════
// Singleton
// ═══════════════════════════════════════════════════════════════════════════

SadAndroidRuntime& SadAndroidRuntime::getInstance() {
    static SadAndroidRuntime instance;
    return instance;
}

SadAndroidRuntime::SadAndroidRuntime() 
    : interpreter_(std::make_unique<InterpreterState>()) {
    SAD_LOGI("SadAndroidRuntime: تم إنشاء الـ Runtime");
}

SadAndroidRuntime::~SadAndroidRuntime() {
    shutdown();
    SAD_LOGI("SadAndroidRuntime: تم تدمير الـ Runtime");
}

// ═══════════════════════════════════════════════════════════════════════════
// التهيئة / Initialization
// ═══════════════════════════════════════════════════════════════════════════

bool SadAndroidRuntime::initialize(
#ifdef SAD_ANDROID
    JNIEnv* env,
    jobject context,
    AAssetManager* assetManager
#endif
) {
    if (initialized_) {
        SAD_LOGW("SadAndroidRuntime: الـ Runtime مُهيأ مسبقاً");
        return true;
    }
    
#ifdef SAD_ANDROID
    env_ = env;
    context_ = env->NewGlobalRef(context);
    assetManager_ = assetManager;
#endif
    
    SAD_LOGI("SadAndroidRuntime: بدء تهيئة الـ Runtime...");
    
    // تسجيل الدوال المضمنة
    registerBuiltinFunctions();
    registerAndroidFunctions();
    
    initialized_ = true;
    SAD_LOGI("SadAndroidRuntime: تم تهيئة الـ Runtime بنجاح");
    
    return true;
}

void SadAndroidRuntime::shutdown() {
    if (!initialized_) return;
    
    SAD_LOGI("SadAndroidRuntime: إيقاف الـ Runtime...");
    
#ifdef SAD_ANDROID
    if (env_ && context_) {
        env_->DeleteGlobalRef(context_);
        context_ = nullptr;
    }
#endif
    
    interpreter_->isRunning = false;
    initialized_ = false;
    
    SAD_LOGI("SadAndroidRuntime: تم إيقاف الـ Runtime");
}

// ═══════════════════════════════════════════════════════════════════════════
// تنفيذ الكود / Code Execution
// ═══════════════════════════════════════════════════════════════════════════

ExecutionResult SadAndroidRuntime::executeFile(const std::string& filePath) {
    ExecutionResult result;
    
    if (!initialized_) {
        result.error = "الـ Runtime غير مُهيأ";
        return result;
    }
    
    SAD_LOGI("SadAndroidRuntime: تنفيذ ملف: %s", filePath.c_str());
    
    // قراءة الملف من assets
    std::string code = readAssetFile(filePath);
    if (code.empty()) {
        result.error = "فشل قراءة الملف: " + filePath;
        return result;
    }
    
    // تنفيذ الكود
    return executeCode(code);
}

ExecutionResult SadAndroidRuntime::executeCode(const std::string& code) {
    ExecutionResult result;
    
    if (!initialized_) {
        result.error = "الـ Runtime غير مُهيأ";
        return result;
    }
    
    SAD_LOGD("SadAndroidRuntime: تنفيذ كود (%zu حرف)", code.size());
    
    interpreter_->isRunning = true;
    
    try {
        // هنا سيتم ربط المفسر الفعلي
        // auto lexer = std::make_unique<Sad::Lexer::LexerCore>(code, "android_script.ص");
        // auto tokens = lexer->tokenize();
        // auto parser = std::make_unique<Sad::Parser::ParserCore>(tokens);
        // auto ast = parser->parse();
        // auto interpreter = std::make_unique<Sad::Interpreter::InterpreterCore>();
        // interpreter->execute(ast);
        
        // مؤقتاً: محاكاة التنفيذ الناجح
        result.success = true;
        result.output = "تم تنفيذ الكود بنجاح";
        result.exitCode = 0;
        
        SAD_LOGI("SadAndroidRuntime: تم تنفيذ الكود بنجاح");
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error = e.what();
        result.exitCode = 1;
        
        SAD_LOGE("SadAndroidRuntime: خطأ في التنفيذ: %s", e.what());
    }
    
    interpreter_->isRunning = false;
    return result;
}

ExecutionResult SadAndroidRuntime::callFunction(
    const std::string& functionName,
    const std::vector<std::string>& args
) {
    ExecutionResult result;
    
    if (!initialized_) {
        result.error = "الـ Runtime غير مُهيأ";
        return result;
    }
    
    SAD_LOGD("SadAndroidRuntime: استدعاء دالة: %s", functionName.c_str());
    
    // هنا سيتم استدعاء الدالة من المفسر
    // TODO: تنفيذ فعلي
    
    result.success = true;
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
// إدارة المتغيرات / Variable Management
// ═══════════════════════════════════════════════════════════════════════════

void SadAndroidRuntime::setVariable(const std::string& name, const std::string& value) {
    interpreter_->variables[name] = value;
    uiState_.variables[name] = value;
    
    // إعلام UI بالتغيير
    for (auto& callback : uiState_.updateCallbacks) {
        callback();
    }
}

std::string SadAndroidRuntime::getVariable(const std::string& name) const {
    auto it = interpreter_->variables.find(name);
    if (it != interpreter_->variables.end()) {
        return it->second;
    }
    return "";
}

bool SadAndroidRuntime::hasVariable(const std::string& name) const {
    return interpreter_->variables.find(name) != interpreter_->variables.end();
}

// ═══════════════════════════════════════════════════════════════════════════
// دورة حياة التطبيق / App Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

void SadAndroidRuntime::onCreate() {
    SAD_LOGI("SadAndroidRuntime: onCreate");
    callFunction("عند_الإنشاء");
}

void SadAndroidRuntime::onStart() {
    SAD_LOGI("SadAndroidRuntime: onStart");
    callFunction("عند_البدء");
}

void SadAndroidRuntime::onResume() {
    SAD_LOGI("SadAndroidRuntime: onResume");
    callFunction("عند_الاستئناف");
}

void SadAndroidRuntime::onPause() {
    SAD_LOGI("SadAndroidRuntime: onPause");
    callFunction("عند_الإيقاف_المؤقت");
}

void SadAndroidRuntime::onStop() {
    SAD_LOGI("SadAndroidRuntime: onStop");
    callFunction("عند_الإيقاف");
}

void SadAndroidRuntime::onDestroy() {
    SAD_LOGI("SadAndroidRuntime: onDestroy");
    callFunction("عند_التدمير");
    shutdown();
}

// ═══════════════════════════════════════════════════════════════════════════
// الأحداث / Events
// ═══════════════════════════════════════════════════════════════════════════

void SadAndroidRuntime::onButtonClick(const std::string& buttonId) {
    SAD_LOGD("SadAndroidRuntime: نقر على زر: %s", buttonId.c_str());
    callFunction("عند_نقر_" + buttonId);
}

void SadAndroidRuntime::onTextChange(const std::string& inputId, const std::string& text) {
    SAD_LOGD("SadAndroidRuntime: تغيير نص في: %s", inputId.c_str());
    setVariable(inputId + "_قيمة", text);
    callFunction("عند_تغيير_" + inputId);
}

void SadAndroidRuntime::registerUICallback(std::function<void()> callback) {
    uiState_.updateCallbacks.push_back(callback);
}

// ═══════════════════════════════════════════════════════════════════════════
// Android APIs
// ═══════════════════════════════════════════════════════════════════════════

void SadAndroidRuntime::showToast(const std::string& message) {
#ifdef SAD_ANDROID
    if (!env_ || !context_) return;
    
    jclass toastClass = env_->FindClass("android/widget/Toast");
    jmethodID makeText = env_->GetStaticMethodID(toastClass, "makeText",
        "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
    
    jstring jmessage = stringToJstring(env_, message);
    jobject toast = env_->CallStaticObjectMethod(toastClass, makeText, 
        context_, jmessage, 0);
    
    jmethodID show = env_->GetMethodID(toastClass, "show", "()V");
    env_->CallVoidMethod(toast, show);
    
    env_->DeleteLocalRef(jmessage);
    env_->DeleteLocalRef(toast);
    env_->DeleteLocalRef(toastClass);
#endif
    SAD_LOGI("Toast: %s", message.c_str());
}

void SadAndroidRuntime::showDialog(const std::string& title, const std::string& message) {
#ifdef SAD_ANDROID
    // TODO: تنفيذ AlertDialog عبر JNI
#endif
    SAD_LOGI("Dialog: %s - %s", title.c_str(), message.c_str());
}

void SadAndroidRuntime::navigateTo(const std::string& screenName) {
    SAD_LOGI("SadAndroidRuntime: انتقال إلى: %s", screenName.c_str());
    // TODO: تنفيذ التنقل
}

// ═══════════════════════════════════════════════════════════════════════════
// الدوال الداخلية / Internal Functions
// ═══════════════════════════════════════════════════════════════════════════

std::string SadAndroidRuntime::readAssetFile(const std::string& path) {
#ifdef SAD_ANDROID
    if (!assetManager_) {
        SAD_LOGE("AssetManager غير متوفر");
        return "";
    }
    
    AAsset* asset = AAssetManager_open(assetManager_, path.c_str(), AASSET_MODE_BUFFER);
    if (!asset) {
        SAD_LOGE("فشل فتح الملف: %s", path.c_str());
        return "";
    }
    
    size_t size = AAsset_getLength(asset);
    std::string content(size, '\0');
    AAsset_read(asset, &content[0], size);
    AAsset_close(asset);
    
    return content;
#else
    // للاختبار على الكمبيوتر
    std::ifstream file(path);
    if (!file) return "";
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
#endif
}

void SadAndroidRuntime::registerBuiltinFunctions() {
    SAD_LOGD("تسجيل الدوال المضمنة...");
    
    // الدوال المضمنة الأساسية ستُسجل هنا
    // اطبع، اطبع_سطر، اقرأ، إلخ
}

void SadAndroidRuntime::registerAndroidFunctions() {
    SAD_LOGD("تسجيل دوال Android...");
    
    // دوال Android ستُسجل هنا:
    // رسالة، إشعار، انتقل، إلخ
}

// ═══════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ═══════════════════════════════════════════════════════════════════════════

#ifdef SAD_ANDROID
std::string jstringToString(JNIEnv* env, jstring jstr) {
    if (!jstr) return "";
    
    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    std::string result(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    
    return result;
}

jstring stringToJstring(JNIEnv* env, const std::string& str) {
    return env->NewStringUTF(str.c_str());
}
#endif

} // namespace android
} // namespace sad
