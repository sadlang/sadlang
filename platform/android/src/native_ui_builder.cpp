/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: native_ui_builder.cpp
 * المسار: platform/android/src/native_ui_builder.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) تنفيذ بناء واجهات Android أصلية عبر JNI مباشر
 * @brief (EN) Implementation of native Android View builder via direct JNI
 *
 * يمشي على شجرة IR ويستدعي SadViewFactory من Java لكل عقدة
 * لإنشاء Views أصلية مباشرة (بدون JSON وبدون WebView)
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "native_ui_builder.h"
#include <android/log.h>
#include <sstream>

#define LOG_TAG "NativeUIBuilder"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// الحد الأقصى لعمق الشجرة لمنع التعاود اللانهائي
static constexpr int MAX_DEPTH = 50;

// ═══════════════════════════════════════════════════════════════════════════════
// تهيئة مراجع JNI
// ═══════════════════════════════════════════════════════════════════════════════

bool NativeUIBuilder::init(JNIEnv* env, jobject context) {
    env_ = env;
    context_ = context;

    // البحث عن صنف SadViewFactory
    jclass localClass = env->FindClass("com/sad/app/SadViewFactory");
    if (!localClass) {
        LOGE("لم يُعثر على SadViewFactory");
        return false;
    }
    factoryClass_ = (jclass)env->NewGlobalRef(localClass);
    env->DeleteLocalRef(localClass);

    // البحث عن الدوال الثابتة
    // View createView(Context ctx, int typeCode)
    createViewMethod_ = env->GetStaticMethodID(
        factoryClass_, "createView",
        "(Landroid/content/Context;I)Landroid/view/View;");
    if (!createViewMethod_) {
        LOGE("لم يُعثر على createView");
        return false;
    }

    // void setProperty(View v, String key, String value)
    setPropertyMethod_ = env->GetStaticMethodID(
        factoryClass_, "setProperty",
        "(Landroid/view/View;Ljava/lang/String;Ljava/lang/String;)V");
    if (!setPropertyMethod_) {
        LOGE("لم يُعثر على setProperty");
        return false;
    }

    // void addChild(View parent, View child)
    addChildMethod_ = env->GetStaticMethodID(
        factoryClass_, "addChild",
        "(Landroid/view/View;Landroid/view/View;)V");
    if (!addChildMethod_) {
        LOGE("لم يُعثر على addChild");
        return false;
    }

    // View wrapRoot(Context ctx, View root)
    wrapRootMethod_ = env->GetStaticMethodID(
        factoryClass_, "wrapRoot",
        "(Landroid/content/Context;Landroid/view/View;)Landroid/view/View;");
    if (!wrapRootMethod_) {
        LOGE("لم يُعثر على wrapRoot");
        return false;
    }

    LOGI("NativeUIBuilder: تمت التهيئة بنجاح");
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════════
// بناء شجرة Views كاملة
// ═══════════════════════════════════════════════════════════════════════════════

jobject NativeUIBuilder::buildTree(const sad::ui::IRNode& root) {
    LOGI("بناء شجرة واجهات أصلية...");
    jobject rootView = buildNode(root, 0);
    if (!rootView) {
        LOGE("فشل بناء الشجرة الجذرية");
        return nullptr;
    }
    LOGI("تم بناء الشجرة بنجاح");
    return rootView;
}

jobject NativeUIBuilder::wrapInScroll(jobject rootView) {
    if (!rootView) return nullptr;
    return env_->CallStaticObjectMethod(
        factoryClass_, wrapRootMethod_, context_, rootView);
}

// ═══════════════════════════════════════════════════════════════════════════════
// بناء عقدة واحدة (تعاودي)
// ═══════════════════════════════════════════════════════════════════════════════

jobject NativeUIBuilder::buildNode(const sad::ui::IRNode& node, int depth) {
    if (depth > MAX_DEPTH) {
        LOGE("تجاوز الحد الأقصى لعمق الشجرة (%d)", MAX_DEPTH);
        return nullptr;
    }

    // 1. إنشاء العرض الأصلي
    int typeCode = static_cast<int>(node.getType());
    LOGD("إنشاء عنصر: نوع=%d عمق=%d", typeCode, depth);

    jobject view = env_->CallStaticObjectMethod(
        factoryClass_, createViewMethod_, context_, (jint)typeCode);

    if (env_->ExceptionCheck()) {
        env_->ExceptionDescribe();
        env_->ExceptionClear();
        LOGE("استثناء عند إنشاء عنصر نوع=%d", typeCode);
        return nullptr;
    }

    if (!view) {
        LOGE("فشل إنشاء عنصر نوع=%d", typeCode);
        return nullptr;
    }

    // 2. تعيين الخصائص
    for (const auto& prop : node.getProperties()) {
        std::string valueStr = propertyValueToString(prop.value);

        jstring jKey = env_->NewStringUTF(prop.key.c_str());
        jstring jVal = env_->NewStringUTF(valueStr.c_str());

        env_->CallStaticVoidMethod(
            factoryClass_, setPropertyMethod_, view, jKey, jVal);

        env_->DeleteLocalRef(jKey);
        env_->DeleteLocalRef(jVal);

        if (env_->ExceptionCheck()) {
            env_->ExceptionDescribe();
            env_->ExceptionClear();
        }
    }

    // 3. إضافة الأبناء (تعاودي)
    for (const auto& child : node.getChildren()) {
        if (!child) continue;

        jobject childView = buildNode(*child, depth + 1);
        if (childView) {
            env_->CallStaticVoidMethod(
                factoryClass_, addChildMethod_, view, childView);
            env_->DeleteLocalRef(childView);

            if (env_->ExceptionCheck()) {
                env_->ExceptionDescribe();
                env_->ExceptionClear();
            }
        }
    }

    return view;
}

// ═══════════════════════════════════════════════════════════════════════════════
// تحويل قيمة خاصية إلى نص
// ═══════════════════════════════════════════════════════════════════════════════

std::string NativeUIBuilder::propertyValueToString(
    const sad::ui::IRProperty::Value& value)
{
    if (auto* s = std::get_if<std::string>(&value)) {
        return *s;
    }
    if (auto* i = std::get_if<int64_t>(&value)) {
        return std::to_string(*i);
    }
    if (auto* d = std::get_if<double>(&value)) {
        std::ostringstream oss;
        oss << *d;
        return oss.str();
    }
    if (auto* b = std::get_if<bool>(&value)) {
        return *b ? "true" : "false";
    }
    return "";
}
