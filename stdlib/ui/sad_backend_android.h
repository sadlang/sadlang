/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_backend_android.h — تنفيذ Backend أندرويد (EGL + GLES3)
 *  Android Backend — wraps tools/android/graphics/sad_ui_engine API
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  يُستخدم فقط عندما يُبنى المشروع بأدوات Android NDK.
 *  يحوّل من الواجهة الموحدة (sad::unified) إلى واجهة C الأصلية (sadui_*).
 *
 *  ملاحظة: من الناحية العملية، يتم تفعيل هذا الملف بشرط الترجمة:
 *    #ifdef __ANDROID__
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "sad_ui_unified.h"

#ifdef __ANDROID__

namespace sad {
namespace unified {

class AndroidBackend : public SadUIBackend {
public:
    AndroidBackend();
    ~AndroidBackend() override;

    std::string backendName() const override { return "Android-GLES3"; }

    // دورة الحياة
    int  appCreate(const std::string& title, int width, int height) override;
    bool appIsRunning(int appId) override;
    void appUpdate(int appId) override;
    void appSetContent(int appId, WidgetId rootWidget) override;
    void appSetBgColor(int appId, int r, int g, int b) override;
    void appClose(int appId) override;
    void appSetTitle(int appId, const std::string& title) override;
    int  appGetWidth(int appId) override;
    int  appGetHeight(int appId) override;
    double appGetFps(int appId) override;

    // إنشاء
    WidgetId createWidget(WidgetType type) override;

    // خصائص
    void setText(WidgetId id, const std::string& text) override;
    void setColor(WidgetId id, const std::string& prop, int r, int g, int b, int a) override;
    void setFloat(WidgetId id, const std::string& prop, float value) override;
    void setInt(WidgetId id, const std::string& prop, int value) override;
    void setBool(WidgetId id, const std::string& prop, bool value) override;
    void setString(WidgetId id, const std::string& prop, const std::string& value) override;

    std::string getText(WidgetId id) override;
    float       getFloat(WidgetId id, const std::string& prop) override;
    bool        getBool(WidgetId id, const std::string& prop) override;

    // شجرة
    void addChild(WidgetId parent, WidgetId child) override;
    void removeChild(WidgetId parent, WidgetId child) override;
    void clearChildren(WidgetId parent) override;
    int  getParent(WidgetId id) override;

    // تخطيط
    void setPadding(WidgetId id, float top, float right, float bottom, float left) override;
    void setMargin(WidgetId id, float top, float right, float bottom, float left) override;
    void setAlignment(WidgetId id, const std::string& main, const std::string& cross) override;
    void setSpacing(WidgetId id, float spacing) override;
    void setFlex(WidgetId id, float flex) override;

    // أحداث
    bool wasClicked(WidgetId id) override;
    std::string getInputText(WidgetId id) override;
};

}} // namespace sad::unified

#endif // __ANDROID__
