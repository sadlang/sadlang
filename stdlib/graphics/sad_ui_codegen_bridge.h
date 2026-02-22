/**
 * ==========================================================================
 * ملف: sad_ui_codegen_bridge.h
 * الوصف: جسر بين الكود المولّد من SadUI IR ← والإطار الفعلي
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 *
 * الكود المولّد من DesktopSDL2CodeGen يستخدم API مبني على مؤشرات:
 *   SadWidget*, sad_ui_create_widget(), sad_ui_add_child() ...
 *
 * بينما الإطار الفعلي في sad_ui_framework.h يستخدم معرّفات عددية:
 *   int widgetId, widget_column(), widget_add_child(int,int) ...
 *
 * هذا الملف يوفر الجسر بين الاثنين.
 */

#ifndef SAD_UI_CODEGEN_BRIDGE_H
#define SAD_UI_CODEGEN_BRIDGE_H

#include "sad_ui_framework.h"
#include "sad_navigator.h"
#include "sad_state.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

// ═══════════════════════════════════════════════════════════════════════════
// النوع الوسيط — SadWidget يغلّف معرّف widget
// ═══════════════════════════════════════════════════════════════════════════

struct SadWidget {
    int id = 0;
    std::string type;   // نوع الـ widget بالعربية
    std::string text;
    float value = 0;
    RGBA foregroundColor = {33, 33, 33, 255};
    RGBA backgroundColor = {0, 0, 0, 0};
    float fontSize = 20;
    bool bold = false;
    float width = -1;   // -1 = auto
    float height = -1;
    struct { float top=0, leading=0, bottom=0, trailing=0; } padding;
    struct { float top=0, leading=0, bottom=0, trailing=0; } margin;
    float borderRadius = 0;
    float borderWidth = 0;
    RGBA borderColor = {180, 180, 180, 255};
    float opacity = 1.0f;
    float shadowBlur = 0;
    RGBA shadowColor = {0, 0, 0, 64};
    bool visible = true;
    bool enabled = true;
    bool rtl = false;

    // الأحداث (callbacks)
    std::function<void()> onClick;
    std::function<void()> onDoubleTap;
    std::function<void()> onLongPress;
    std::function<void()> onTextChange;
    std::function<void()> onValueChange;
    std::function<void()> onToggle;
    std::function<void()> onSubmit;
    std::function<void()> onAppear;
    std::function<void()> onDisappear;
    std::function<void()> onScroll;
};

// ═══════════════════════════════════════════════════════════════════════════
// إعدادات التطبيق والثيمة
// ═══════════════════════════════════════════════════════════════════════════

struct SadTheme {
    RGBA primaryColor   = {41, 98, 255, 255};
    RGBA secondaryColor = {0, 200, 83, 255};
    RGBA backgroundColor= {250, 250, 252, 255};
    RGBA surfaceColor   = {255, 255, 255, 255};
    RGBA textPrimary    = {30, 30, 30, 255};
};

struct SadUIConfig {
    std::string windowTitle = "تطبيق ص";
    int windowWidth = 420;
    int windowHeight = 750;
    bool rtl = true;
    SadTheme theme;
};

// ═══════════════════════════════════════════════════════════════════════════
// حالة عامة للجسر
// ═══════════════════════════════════════════════════════════════════════════

namespace sad_bridge {

inline int g_appId = 0;
inline int g_rootWidget = 0;
inline std::vector<std::unique_ptr<SadWidget>> g_widgets;
inline std::unordered_map<int, SadWidget*> g_widgetMap;  // framework id → SadWidget*

// خريطة الأنواع العربية → WidgetType
inline WidgetType typeFromArabic(const std::string& name) {
    static const std::unordered_map<std::string, WidgetType> map = {
        {"\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf", WidgetType::Column},           // عمود
        {"\xd8\xb5\xd9\x81", WidgetType::Row},                                // صف
        {"\xd9\x86\xd8\xb5", WidgetType::Text},                               // نص
        {"\xd8\xb2\xd8\xb1", WidgetType::Button},                             // زر
        {"\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5", WidgetType::TextField}, // حقل_نص
        {"\xd8\xad\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9", WidgetType::Container}, // حاوية
        {"\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9", WidgetType::Card},      // بطاقة
        {"\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9", WidgetType::Image},             // صورة
        {"\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad", WidgetType::Switch},    // مفتاح
        {"\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82", WidgetType::Slider},    // منزلق
        {"\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84", WidgetType::Spacer},            // فاصل
        {"\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9", WidgetType::List},      // قائمة
        {"\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", WidgetType::GridView},          // شبكة
        {"\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", WidgetType::ScrollView}, // تمرير
    };
    auto it = map.find(name);
    return (it != map.end()) ? it->second : WidgetType::Container;
}

// إنشاء widget حقيقي من الإطار حسب النوع
inline int createFrameworkWidget(WidgetType type, const std::string& text = "") {
    switch (type) {
        case WidgetType::Column:    return widget_column();
        case WidgetType::Row:       return widget_row();
        case WidgetType::Text:      return widget_text(text);
        case WidgetType::Button:    return widget_button(text);
        case WidgetType::TextField: return widget_textfield(text);
        case WidgetType::Container: return widget_container();
        case WidgetType::Card:      return widget_card();
        case WidgetType::Switch:    return widget_switch();
        case WidgetType::Slider:    return widget_slider(0, 100);
        case WidgetType::Spacer:    return widget_spacer(10);
        case WidgetType::ScrollView:return widget_scrollview();
        case WidgetType::List:      return widget_list();
        case WidgetType::GridView:  return widget_grid(2);
        default:                    return widget_container();
    }
}

} // namespace sad_bridge

// ═══════════════════════════════════════════════════════════════════════════
// API المُولّد — الدوال التي يستدعيها الكود المولّد
// ═══════════════════════════════════════════════════════════════════════════

inline SadWidget* sad_ui_create_widget() {
    auto w = std::make_unique<SadWidget>();
    SadWidget* ptr = w.get();
    sad_bridge::g_widgets.push_back(std::move(w));
    return ptr;
}

inline void sad_ui_add_child(SadWidget* parent, SadWidget* child) {
    if (!parent || !child) return;
    widget_add_child(parent->id, child->id);
}

// تحويل SadWidget المؤقت إلى widget حقيقي في الإطار
inline void sad_bridge_materialize(SadWidget* w) {
    if (!w || w->id != 0) return; // already materialized

    WidgetType type = sad_bridge::typeFromArabic(w->type);
    w->id = sad_bridge::createFrameworkWidget(type, w->text);

    // تطبيق الأنماط
    if (w->backgroundColor.a > 0) {
        widget_set_bg(w->id, w->backgroundColor.r, w->backgroundColor.g, w->backgroundColor.b);
    }
    if (w->foregroundColor.a > 0 &&
        (type == WidgetType::Text || type == WidgetType::Button)) {
        text_set_color(w->id, w->foregroundColor.r, w->foregroundColor.g, w->foregroundColor.b);
    }
    if (w->fontSize != 20) {
        text_set_size(w->id, w->fontSize);
    }
    if (w->width >= 0) {
        widget_set_width(w->id, w->width);
    }
    if (w->height >= 0) {
        widget_set_height(w->id, w->height);
    }
    if (w->padding.top > 0 || w->padding.leading > 0 || w->padding.bottom > 0 || w->padding.trailing > 0) {
        // Use max padding as uniform padding (framework only has uniform)
        float maxPad = std::max({w->padding.top, w->padding.leading, w->padding.bottom, w->padding.trailing});
        widget_set_padding(w->id, maxPad);
    }
    if (w->borderRadius > 0) {
        widget_set_border_radius(w->id, w->borderRadius);
    }
    if (w->borderWidth > 0) {
        widget_set_border(w->id, w->borderWidth,
                         w->borderColor.r, w->borderColor.g, w->borderColor.b);
    }
    if (!w->visible) {
        widget_set_visible(w->id, false);
    }

    // تسجيل في الخريطة
    sad_bridge::g_widgetMap[w->id] = w;
}

inline bool sad_ui_init(SadUIConfig config) {
    sad_bridge::g_appId = app_create(config.windowTitle, config.windowWidth, config.windowHeight);
    if (sad_bridge::g_appId <= 0) return false;
    app_set_bg_color(sad_bridge::g_appId,
                     config.theme.backgroundColor.r,
                     config.theme.backgroundColor.g,
                     config.theme.backgroundColor.b);
    return true;
}

inline void sad_ui_run() {
    // Use navigator's current page if available
    int rootWidget = sad::nav::currentPageWidget();
    if (rootWidget > 0) {
        app_set_content(sad_bridge::g_appId, rootWidget);
    }

    while (app_is_running(sad_bridge::g_appId)) {
        app_update(sad_bridge::g_appId);

        // Check for button clicks and dispatch callbacks
        for (auto& [id, w] : sad_bridge::g_widgetMap) {
            if (w->onClick && button_was_clicked(id)) {
                w->onClick();
            }
        }

        // Update navigator page if changed
        int currentPage = sad::nav::currentPageWidget();
        if (currentPage > 0 && currentPage != rootWidget) {
            rootWidget = currentPage;
            app_set_content(sad_bridge::g_appId, rootWidget);
        }
    }
}

inline void sad_ui_cleanup() {
    app_close(sad_bridge::g_appId);
    sad_bridge::g_widgets.clear();
    sad_bridge::g_widgetMap.clear();
}

inline void sad_nav_register(const std::string& name, std::function<SadWidget*()> buildFunc) {
    sad::nav::registerRoute(name, [buildFunc](const std::unordered_map<std::string,std::string>&) -> int {
        SadWidget* root = buildFunc();
        if (!root) return 0;
        // Materialize all widgets
        for (auto& w : sad_bridge::g_widgets) {
            sad_bridge_materialize(w.get());
        }
        return root->id;
    });
}

inline void sad_nav_push(const std::string& name) {
    sad::nav::push(name);
}

// ═══════════════════════════════════════════════════════════════════════════
// حالة تفاعلية — Signal<T> للكود المولّد
// ═══════════════════════════════════════════════════════════════════════════

namespace sad {
namespace state {

template<typename T>
class Signal {
    std::string key_;
public:
    Signal() = default;
    Signal(T initial) : key_("signal_" + std::to_string(reinterpret_cast<uintptr_t>(this))) {
        setState(key_, initial);
    }
    Signal(const std::string& name, T initial) : key_(name) {
        setState(key_, initial);
    }

    T get() const;
    void set(T val) { setState(key_, val); }
    const std::string& key() const { return key_; }
};

template<> inline double Signal<double>::get() const { return getStateNumber(key_); }
template<> inline std::string Signal<std::string>::get() const { return getStateText(key_); }
template<> inline bool Signal<bool>::get() const { return getStateBool(key_); }

// Computed — placeholder
template<typename T>
class Computed {
    std::function<T()> fn_;
public:
    Computed(std::function<T()> fn) : fn_(fn) {}
    T get() const { return fn_(); }
};

// Observable — alias for Signal
template<typename T>
using Observable = Signal<T>;

} // namespace state
} // namespace sad


#endif // SAD_UI_CODEGEN_BRIDGE_H
