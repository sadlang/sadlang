/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: native_ui_builder.h
 * المسار: platform/android/src/native_ui_builder.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) بناء واجهات Android أصلية مباشرة من شجرة IR عبر JNI
 * @brief (EN) Build native Android views directly from IR tree via JNI
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_NATIVE_UI_BUILDER_H
#define SAD_NATIVE_UI_BUILDER_H

#include <jni.h>
#include "sad_ui/ir.h"
#include <memory>
#include <string>

/**
 * @brief (AR) يبني شجرة Android Views أصلية من شجرة IR عبر JNI مباشر
 * @brief (EN) Builds native Android View tree from IR tree via direct JNI
 *
 * المسار: IRNode → JNI → SadViewFactory.createView() → Android View
 * كل IRNode يُحوّل إلى View أصلي (LinearLayout, TextView, Button, ...)
 */
class NativeUIBuilder {
public:
    NativeUIBuilder() = default;

    /**
     * @brief (AR) تهيئة مراجع JNI — يجب استدعاؤها مرة واحدة
     * @brief (EN) Initialize JNI references — must be called once
     */
    bool init(JNIEnv* env, jobject context);

    /**
     * @brief (AR) بناء شجرة Views كاملة من عقدة IR جذرية
     * @brief (EN) Build complete View tree from root IR node
     * @return jobject — العرض الجذري (root View) أو nullptr
     */
    jobject buildTree(const sad::ui::IRNode& root);

    /**
     * @brief (AR) لف العرض الجذري في ScrollView مع تصميم عربي
     * @brief (EN) Wrap root view in styled ScrollView with Arabic design
     */
    jobject wrapInScroll(jobject rootView);

private:
    /// (AR) بناء عقدة واحدة وأبنائها بشكل تعاودي
    jobject buildNode(const sad::ui::IRNode& node, int depth = 0);

    /// (AR) تحويل قيمة خاصية IR إلى نص
    std::string propertyValueToString(const sad::ui::IRProperty::Value& value);

    JNIEnv* env_ = nullptr;
    jobject context_ = nullptr;

    // مراجع JNI المخزّنة (Cached JNI references)
    jclass factoryClass_ = nullptr;
    jmethodID createViewMethod_ = nullptr;
    jmethodID setPropertyMethod_ = nullptr;
    jmethodID addChildMethod_ = nullptr;
    jmethodID wrapRootMethod_ = nullptr;
};

#endif // SAD_NATIVE_UI_BUILDER_H
