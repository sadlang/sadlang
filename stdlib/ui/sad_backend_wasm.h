/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_backend_wasm.h — تنفيذ Backend الويب (Canvas2D عبر Emscripten)
 *  WASM/Web Backend — renders via HTML Canvas2D using EM_ASM
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  يُبنى فقط عندما يُترجم المشروع بـ Emscripten (__EMSCRIPTEN__).
 *  يُصدِر أوامر JavaScript لرسم المكونات على عنصر <canvas>.
 *
 *  البنية:
 *    - كل widget يُخزّن كـ WasmWidget في unordered_map
 *    - عند appUpdate يُصدر أوامر JS لإعادة الرسم الكامل
 *    - الأحداث تُلتقط عبر addEventListener في JS وتُمرَّر عبر ccall
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "sad_ui_unified.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace sad {
namespace unified {

// ─── بنية Widget داخلية للويب ────────────────────────────────────────────
struct WasmWidget {
    WidgetType type = WidgetType::Container;
    int id   = 0;
    int parent = -1;
    std::vector<int> children;

    // خصائص
    std::string text;
    float x = 0, y = 0, w = 0, h = 0;
    float fontSize = 16;
    float padding = 0, paddingL = 0, paddingR = 0, paddingT = 0, paddingB = 0;
    float margin  = 0;
    float marginT = 0, marginR = 0, marginB = 0, marginL = 0;
    float spacing = 4;
    int fontWeight = 400;
    int zIndex = 0;
    float borderRadius = 0;
    float borderWidth  = 0;
    float opacity = 1.0f;
    float flex = 0;
    float value = 0;

    int bgR = 255, bgG = 255, bgB = 255, bgA = 255;
    int txR = 33,  txG = 33,  txB = 33,  txA = 255;
    int brR = 180, brG = 180, brB = 180, brA = 255;

    std::string mainAlign = "start";
    std::string crossAlign = "start";

    bool visible = true;
    bool enabled = true;
    bool checked = false;
    bool clickFired = false;
    std::string inputText;
};

class WasmBackend : public SadUIBackend {
public:
    WasmBackend();
    ~WasmBackend() override;

    std::string backendName() const override { return "WASM-Canvas2D"; }

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

    // ─── للاستخدام من JavaScript callbacks ───
    void notifyClick(int widgetId);
    void notifyInput(int widgetId, const std::string& text);

private:
    std::unordered_map<int, WasmWidget> m_widgets;
    int m_nextId = 1;
    int m_rootWidget = -1;
    int m_canvasWidth = 800, m_canvasHeight = 600;
    int m_bgR = 245, m_bgG = 245, m_bgB = 250;
    bool m_running = false;
    double m_currentFps = 60.0;

    WasmWidget* get(int id);

    // تخطيط وسم
    void layoutWidget(WasmWidget& w, float x, float y, float availW, float availH);
    void renderWidget(const WasmWidget& w);
    void renderAll();
};

}} // namespace sad::unified
