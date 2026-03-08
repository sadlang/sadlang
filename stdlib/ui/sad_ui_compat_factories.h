/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_ui_compat_factories.h — طبقة توافق إنشاء المكونات عبر المنصات
 *  Cross-platform widget factory compatibility layer
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  على سطح المكتب: يشمل sad_ui_framework.h مباشرة (بدون تغيير)
 *  على WASM/أندرويد: يوفر دوال sad::ui::* تعيد التوجيه عبر الطبقة الموحدة
 *
 *  الاستخدام: استبدل #include "graphics/sad_ui_framework.h" بـ:
 *    #include "ui/sad_ui_compat_factories.h"
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

// ─── سطح المكتب: استخدم الإطار الأصلي مباشرة ───────────────────────────────
#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
#include "graphics/sad_ui_framework.h"

#else
// ─── WASM / أندرويد: أعد التوجيه عبر الطبقة الموحدة ─────────────────────────
#include "sad_ui_unified.h"
#include "sad_ui_platform.h"
#include "sad_ui_compat.h"

#include <string>
#include <vector>
#include <unordered_set>

namespace sad {
namespace ui {

// ═══════════════════════════════════════════════════════════════════════════════
//  مساعد: إنشاء مكون وتعيين نص اختياري
// ═══════════════════════════════════════════════════════════════════════════════
static inline int _createUnified(sad::unified::WidgetType type) {
    return sad::unified::SadUI::get().createWidget(type);
}
static inline int _createUnifiedWithText(sad::unified::WidgetType type, const std::string& text) {
    int id = _createUnified(type);
    sad::unified::SadUI::get().setText(id, text);
    return id;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال إنشاء المكونات (109) — تعيد التوجيه إلى الطبقة الموحدة
// ═══════════════════════════════════════════════════════════════════════════════

// --- حاويات ---
inline int widget_column()       { return _createUnified(sad::unified::WidgetType::Column); }
inline int widget_row()          { return _createUnified(sad::unified::WidgetType::Row); }
inline int widget_container()    { return _createUnified(sad::unified::WidgetType::Container); }
inline int widget_stack()        { return _createUnified(sad::unified::WidgetType::Stack); }
inline int widget_card()         { return _createUnified(sad::unified::WidgetType::Card); }
inline int widget_scrollview()   { return _createUnified(sad::unified::WidgetType::ScrollView); }
inline int widget_center()       { return _createUnified(sad::unified::WidgetType::Container); } // fallback
inline int widget_wrap()         { return _createUnified(sad::unified::WidgetType::Container); }
inline int widget_expanded()     { return _createUnified(sad::unified::WidgetType::Container); }
inline int widget_pageview()     { return _createUnified(sad::unified::WidgetType::Container); }
inline int widget_split_view()   { return _createUnified(sad::unified::WidgetType::SplitView); }

inline int widget_gridview(int cols) {
    int id = _createUnified(sad::unified::WidgetType::Grid);
    sad::unified::SadUI::get().setInt(id, "columns", cols);
    return id;
}
inline int widget_listview()     { return _createUnified(sad::unified::WidgetType::ListView); }
inline int widget_sizedbox(float w, float h) {
    int id = _createUnified(sad::unified::WidgetType::Container);
    sad::unified::SadUI::get().setFloat(id, "width", w);
    sad::unified::SadUI::get().setFloat(id, "height", h);
    return id;
}

// --- نصوص ---
inline int widget_text(const std::string& content)    { return _createUnifiedWithText(sad::unified::WidgetType::Text, content); }
inline int widget_label(const std::string& text)       { return _createUnifiedWithText(sad::unified::WidgetType::Label, text); }
inline int widget_paragraph(const std::string& text)   { return _createUnifiedWithText(sad::unified::WidgetType::Paragraph, text); }
inline int widget_link(const std::string& text)        { return _createUnifiedWithText(sad::unified::WidgetType::Text, text); }
inline int widget_code_block(const std::string& code)  { return _createUnifiedWithText(sad::unified::WidgetType::CodeBlock, code); }
inline int widget_quote(const std::string& text)       { return _createUnifiedWithText(sad::unified::WidgetType::Text, text); }
inline int widget_markdown(const std::string& text)    { return _createUnifiedWithText(sad::unified::WidgetType::Markdown, text); }
inline int widget_heading(const std::string& text, int level = 1) {
    int id = _createUnifiedWithText(sad::unified::WidgetType::Heading, text);
    sad::unified::SadUI::get().setInt(id, "level", level);
    return id;
}

// --- أزرار ---
inline int widget_button(const std::string& text)          { return _createUnifiedWithText(sad::unified::WidgetType::Button, text); }
inline int widget_icon_button(const std::string& icon)     { return _createUnifiedWithText(sad::unified::WidgetType::IconButton, icon); }
inline int widget_floating_button(const std::string& icon) { return _createUnifiedWithText(sad::unified::WidgetType::FloatingButton, icon); }
inline int widget_outline_button(const std::string& text)  { return _createUnifiedWithText(sad::unified::WidgetType::OutlineButton, text); }
inline int widget_text_button(const std::string& text)     { return _createUnifiedWithText(sad::unified::WidgetType::TextButton, text); }
inline int widget_toggle_button(const std::string& text)   { return _createUnifiedWithText(sad::unified::WidgetType::Button, text); }
inline int widget_button_group()  { return _createUnified(sad::unified::WidgetType::ButtonGroup); }

// --- إدخال ---
inline int widget_textfield(const std::string& hint)     { return _createUnifiedWithText(sad::unified::WidgetType::TextField, hint); }
inline int widget_password_field(const std::string& hint) { return _createUnifiedWithText(sad::unified::WidgetType::PasswordField, hint); }
inline int widget_textarea(const std::string& hint)       { return _createUnifiedWithText(sad::unified::WidgetType::TextArea, hint); }
inline int widget_search_field(const std::string& hint)   { return _createUnifiedWithText(sad::unified::WidgetType::TextField, hint); }
inline int widget_number_input(float min, float max)       {
    int id = _createUnified(sad::unified::WidgetType::TextField);
    sad::unified::SadUI::get().setFloat(id, "min", min);
    sad::unified::SadUI::get().setFloat(id, "max", max);
    return id;
}
inline int widget_checkbox(const std::string& label)    { return _createUnifiedWithText(sad::unified::WidgetType::Checkbox, label); }
inline int widget_radio(const std::string& label)       { return _createUnifiedWithText(sad::unified::WidgetType::Radio, label); }
inline int widget_switch(const std::string& label)      { return _createUnifiedWithText(sad::unified::WidgetType::Switch, label); }
inline int widget_slider(float min, float max) {
    int id = _createUnified(sad::unified::WidgetType::Slider);
    sad::unified::SadUI::get().setFloat(id, "min", min);
    sad::unified::SadUI::get().setFloat(id, "max", max);
    return id;
}
inline int widget_dropdown(const std::string& hint)   { return _createUnifiedWithText(sad::unified::WidgetType::Dropdown, hint); }
inline int widget_date_field()                         { return _createUnified(sad::unified::WidgetType::DatePicker); }
inline int widget_range_slider(float min, float max)   { return widget_slider(min, max); }
inline int widget_toggle_group()  { return _createUnified(sad::unified::WidgetType::Container); }
inline int widget_color_picker()  { return _createUnified(sad::unified::WidgetType::ColorPicker); }
inline int widget_rating_stars(int maxStars) {
    int id = _createUnified(sad::unified::WidgetType::RatingStars);
    sad::unified::SadUI::get().setInt(id, "max", maxStars);
    return id;
}

// --- عرض بصري ---
inline int widget_icon(const std::string& icon)       { return _createUnifiedWithText(sad::unified::WidgetType::Icon, icon); }
inline int widget_image(const std::string& placeholder) { return _createUnifiedWithText(sad::unified::WidgetType::Image, placeholder); }
inline int widget_avatar(const std::string& letter, int r = 100, int g = 100, int b = 200) {
    int id = _createUnifiedWithText(sad::unified::WidgetType::Avatar, letter);
    sad::unified::SadUI::get().setColor(id, "bg", r, g, b, 255);
    return id;
}
inline int widget_badge(int count) {
    int id = _createUnified(sad::unified::WidgetType::Badge);
    sad::unified::SadUI::get().setText(id, std::to_string(count));
    return id;
}
inline int widget_chip(const std::string& label)  { return _createUnifiedWithText(sad::unified::WidgetType::Chip, label); }
inline int widget_tag(const std::string& label, int r, int g, int b) {
    int id = _createUnifiedWithText(sad::unified::WidgetType::Chip, label);
    sad::unified::SadUI::get().setColor(id, "bg", r, g, b, 255);
    return id;
}
inline int widget_status_dot(int r, int g, int b) {
    int id = _createUnified(sad::unified::WidgetType::Container);
    sad::unified::SadUI::get().setColor(id, "bg", r, g, b, 255);
    return id;
}
inline int widget_color_box(int r, int g, int b) {
    int id = _createUnified(sad::unified::WidgetType::Container);
    sad::unified::SadUI::get().setColor(id, "bg", r, g, b, 255);
    return id;
}
inline int widget_gradient_box(int r1, int g1, int b1, int r2, int g2, int b2) {
    int id = _createUnified(sad::unified::WidgetType::Container);
    sad::unified::SadUI::get().setColor(id, "bg", r1, g1, b1, 255);
    return id;
}
inline int widget_divider()   { return _createUnified(sad::unified::WidgetType::Divider); }
inline int widget_separator() { return _createUnified(sad::unified::WidgetType::Divider); }
inline int widget_spacer(float size = 8.0f) {
    int id = _createUnified(sad::unified::WidgetType::Spacer);
    sad::unified::SadUI::get().setFloat(id, "size", size);
    return id;
}

// --- تقدم ---
inline int widget_progress_bar(float value) {
    int id = _createUnified(sad::unified::WidgetType::ProgressBar);
    sad::unified::SadUI::get().setFloat(id, "value", value);
    return id;
}
inline int widget_circle_progress(float value) {
    int id = _createUnified(sad::unified::WidgetType::CircleProgress);
    sad::unified::SadUI::get().setFloat(id, "value", value);
    return id;
}
inline int widget_loading_spinner()  { return _createUnified(sad::unified::WidgetType::CircleProgress); }
inline int widget_skeleton()         { return _createUnified(sad::unified::WidgetType::Container); }

// --- تنقل ---
inline int widget_app_bar(const std::string& title) { return _createUnifiedWithText(sad::unified::WidgetType::AppBar, title); }
inline int widget_bottom_bar()    { return _createUnified(sad::unified::WidgetType::BottomBar); }
inline int widget_tab_bar()       { return _createUnified(sad::unified::WidgetType::TabBar); }
inline int widget_drawer()        { return _createUnified(sad::unified::WidgetType::Drawer); }
inline int widget_breadcrumb()    { return _createUnified(sad::unified::WidgetType::Breadcrumb); }
inline int widget_nav_item(const std::string& label) { return _createUnifiedWithText(sad::unified::WidgetType::Container, label); }
inline int widget_menu_item(const std::string& label) { return _createUnifiedWithText(sad::unified::WidgetType::Container, label); }
inline int widget_toolbar()       { return _createUnified(sad::unified::WidgetType::Row); }
inline int widget_status_bar(const std::string& text) { return _createUnifiedWithText(sad::unified::WidgetType::Text, text); }
inline int widget_side_menu()     { return _createUnified(sad::unified::WidgetType::Column); }
inline int widget_stepper(int steps) {
    int id = _createUnified(sad::unified::WidgetType::Stepper);
    sad::unified::SadUI::get().setInt(id, "steps", steps);
    return id;
}

// --- حوارات ---
inline int widget_dialog(const std::string& title, const std::string& msg) {
    int id = _createUnified(sad::unified::WidgetType::Dialog);
    sad::unified::SadUI::get().setText(id, title);
    return id;
}
inline int widget_alert(const std::string& msg, const std::string& type) {
    return _createUnifiedWithText(sad::unified::WidgetType::Snackbar, msg);
}
inline int widget_toast(const std::string& msg)    { return _createUnifiedWithText(sad::unified::WidgetType::Snackbar, msg); }
inline int widget_snackbar(const std::string& msg) { return _createUnifiedWithText(sad::unified::WidgetType::Snackbar, msg); }
inline int widget_banner(const std::string& msg)   { return _createUnifiedWithText(sad::unified::WidgetType::Text, msg); }
inline int widget_modal()         { return _createUnified(sad::unified::WidgetType::Dialog); }
inline int widget_popover()       { return _createUnified(sad::unified::WidgetType::Dialog); }
inline int widget_notification(const std::string& title, const std::string& msg) {
    return _createUnifiedWithText(sad::unified::WidgetType::Snackbar, title);
}
inline int widget_confirm_dialog(const std::string& title, const std::string& msg) {
    return widget_dialog(title, msg);
}
inline int widget_tooltip(const std::string& text) { return _createUnifiedWithText(sad::unified::WidgetType::Tooltip, text); }
inline int widget_info_card(const std::string& title, const std::string& body) {
    int id = _createUnified(sad::unified::WidgetType::Card);
    sad::unified::SadUI::get().setText(id, title);
    return id;
}
inline int widget_stat_card(const std::string& label, const std::string& value) {
    int id = _createUnified(sad::unified::WidgetType::Card);
    sad::unified::SadUI::get().setText(id, label + ": " + value);
    return id;
}
inline int widget_expansion_panel(const std::string& title) {
    return _createUnifiedWithText(sad::unified::WidgetType::ExpandableList, title);
}
inline int widget_accordion()     { return _createUnified(sad::unified::WidgetType::Accordion); }
inline int widget_panel(const std::string& title)    { return _createUnifiedWithText(sad::unified::WidgetType::Card, title); }
inline int widget_group_box(const std::string& title) { return _createUnifiedWithText(sad::unified::WidgetType::Card, title); }
inline int widget_callout(const std::string& text, const std::string& type) {
    return _createUnifiedWithText(sad::unified::WidgetType::Card, text);
}
inline int widget_list_tile(const std::string& title, const std::string& subtitle) {
    int id = _createUnifiedWithText(sad::unified::WidgetType::Container, title);
    return id;
}

// --- بيانات ---
inline int widget_table(int cols) { return widget_gridview(cols); }
inline int widget_data_table(int cols) { return widget_gridview(cols); }
inline int widget_table_row()         { return _createUnified(sad::unified::WidgetType::Row); }
inline int widget_table_cell(const std::string& text) { return _createUnifiedWithText(sad::unified::WidgetType::Text, text); }
inline int widget_detail_row(const std::string& key, const std::string& val) {
    int id = _createUnified(sad::unified::WidgetType::Row);
    int k = _createUnifiedWithText(sad::unified::WidgetType::Text, key);
    int v = _createUnifiedWithText(sad::unified::WidgetType::Text, val);
    sad::unified::SadUI::get().addChild(id, k);
    sad::unified::SadUI::get().addChild(id, v);
    return id;
}
inline int widget_timeline()         { return _createUnified(sad::unified::WidgetType::Column); }
inline int widget_timeline_item(const std::string& title) { return _createUnifiedWithText(sad::unified::WidgetType::Container, title); }
inline int widget_tree_item(const std::string& label, int depth = 0) {
    return _createUnifiedWithText(sad::unified::WidgetType::TreeView, label);
}
inline int widget_step_indicator(int total, int current) {
    int id = _createUnified(sad::unified::WidgetType::Stepper);
    return id;
}

// --- مخططات ---
inline int widget_bar_chart()    { return _createUnified(sad::unified::WidgetType::BarChart); }
inline int widget_hbar_chart()   { return _createUnified(sad::unified::WidgetType::BarChart); }
inline int widget_pie_chart()    { return _createUnified(sad::unified::WidgetType::PieChart); }
inline int widget_line_chart()   { return _createUnified(sad::unified::WidgetType::LineChart); }
inline int widget_spark_line()   { return _createUnified(sad::unified::WidgetType::SparkLine); }
inline int widget_donut_chart()  { return _createUnified(sad::unified::WidgetType::DonutChart); }
inline int widget_gauge(float value, float max) {
    int id = _createUnified(sad::unified::WidgetType::Gauge);
    sad::unified::SadUI::get().setFloat(id, "value", value);
    sad::unified::SadUI::get().setFloat(id, "max", max);
    return id;
}
inline int widget_pagination(int totalPages) {
    int id = _createUnified(sad::unified::WidgetType::Container);
    return id;
}
inline int widget_dot_indicator(int total, int current) {
    return _createUnified(sad::unified::WidgetType::Container);
}
inline int widget_canvas(int w, int h) {
    int id = _createUnified(sad::unified::WidgetType::Canvas);
    sad::unified::SadUI::get().setFloat(id, "width", (float)w);
    sad::unified::SadUI::get().setFloat(id, "height", (float)h);
    return id;
}
inline int widget_carousel() { return _createUnified(sad::unified::WidgetType::Carousel); }

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال التلاعب بالمكونات — إعادة التوجيه الموحدة
// ═══════════════════════════════════════════════════════════════════════════════

inline void widget_add_child(int parent, int child) {
    sad::unified::SadUI::get().addChild(parent, child);
}
inline void widget_add_child_tracked(int parent, int child) {
    sad::unified::SadUI::get().addChild(parent, child);
}
inline void widget_remove_child(int parent, int child) {
    sad::unified::SadUI::get().removeChild(parent, child);
}
inline void widget_clear_children(int parent) {
    sad::unified::SadUI::get().clearChildren(parent);
}

// --- خصائص ---
inline void widgetSetProp(int id, const std::string& name, const std::string& val) {
    sad::unified::SadUI::get().setString(id, name, val);
}
inline void widgetSetProp(int id, const std::string& name, double val) {
    sad::unified::SadUI::get().setFloat(id, name, (float)val);
}
inline void widgetSetProp(int id, const std::string& name, bool val) {
    sad::unified::SadUI::get().setBool(id, name, val);
}

// --- العرض/الارتفاع والأبعاد ---
inline void widget_set_width(int id, float w) {
    sad::unified::SadUI::get().setFloat(id, "width", w);
}
inline void widget_set_height(int id, float h) {
    sad::unified::SadUI::get().setFloat(id, "height", h);
}
inline void widget_set_bg(int id, int r, int g, int b) {
    sad::unified::SadUI::get().setColor(id, "bg", r, g, b, 255);
}
inline void widget_set_padding(int id, float all) {
    sad::unified::SadUI::get().setPadding(id, all, all, all, all);
}
inline void widget_set_padding4(int id, float t, float r, float b, float l) {
    sad::unified::SadUI::get().setPadding(id, t, r, b, l);
}
inline void widget_set_margin(int id, float all) {
    sad::unified::SadUI::get().setMargin(id, all, all, all, all);
}
inline void widget_set_margin4(int id, float t, float r, float b, float l) {
    sad::unified::SadUI::get().setMargin(id, t, r, b, l);
}
inline void widget_set_flex(int id, float flex) {
    sad::unified::SadUI::get().setFlex(id, flex);
}
inline void widget_set_border_radius(int id, float radius) {
    sad::unified::SadUI::get().setFloat(id, "borderRadius", radius);
}
inline void widget_set_spacing(int id, float gap) {
    sad::unified::SadUI::get().setSpacing(id, gap);
}
inline void widget_set_visible(int id, bool v) {
    sad::unified::SadUI::get().setBool(id, "visible", v);
}
inline void widget_set_border(int id, float width, int r, int g, int b) {
    sad::unified::SadUI::get().setFloat(id, "borderWidth", width);
    sad::unified::SadUI::get().setColor(id, "border", r, g, b, 255);
}
inline void widget_set_checked(int id, bool v) {
    sad::unified::SadUI::get().setBool(id, "checked", v);
}
inline void widget_set_value(int id, float v) {
    sad::unified::SadUI::get().setFloat(id, "value", v);
}
inline void widget_set_selected(int id, int v) {
    sad::unified::SadUI::get().setInt(id, "selected", v);
}
inline void widget_set_expanded(int id, bool v) {
    sad::unified::SadUI::get().setBool(id, "expanded", v);
}
inline void widget_set_active(int id, bool v) {
    sad::unified::SadUI::get().setBool(id, "active", v);
}
inline void widget_add_item(int id, const std::string& item) {
    sad::unified::SadUI::get().setString(id, "addItem", item);
}
inline void widget_add_data(int id, float val) {
    sad::unified::SadUI::get().setFloat(id, "addData", val);
}
inline void widget_set_icon(int id, const std::string& icon) {
    sad::unified::SadUI::get().setString(id, "icon", icon);
}
inline void widget_set_subtitle(int id, const std::string& sub) {
    sad::unified::SadUI::get().setString(id, "subtitle", sub);
}
inline void widget_set_level(int id, int lvl) {
    sad::unified::SadUI::get().setInt(id, "level", lvl);
}
inline void widget_set_columns(int id, int cols) {
    sad::unified::SadUI::get().setInt(id, "columns", cols);
}
inline void widget_set_page(int id, int pg) {
    sad::unified::SadUI::get().setInt(id, "page", pg);
}
inline void widget_set_accent_color(int id, int r, int g, int b) {
    sad::unified::SadUI::get().setColor(id, "accent", r, g, b, 255);
}
inline void widget_show_dialog(int id, bool show) {
    sad::unified::SadUI::get().setBool(id, "visible", show);
}

// --- نصوص ---
inline void text_set_size(int id, float sz) {
    sad::unified::SadUI::get().setFloat(id, "fontSize", sz);
}
inline void text_set_color(int id, int r, int g, int b) {
    sad::unified::SadUI::get().setColor(id, "text", r, g, b, 255);
}
inline void text_set_content(int id, const std::string& content) {
    sad::unified::SadUI::get().setText(id, content);
}
inline std::string text_get_content(int id) {
    return sad::unified::SadUI::get().getText(id);
}
inline void text_set_align(int id, const std::string& align) {
    sad::unified::SadUI::get().setString(id, "textAlign", align);
}

// --- أزرار ---
inline void button_set_color(int id, int r, int g, int b) {
    sad::unified::SadUI::get().setColor(id, "bg", r, g, b, 255);
}
inline bool button_was_clicked(int id) {
    return sad::unified::SadUI::get().wasClicked(id);
}

// --- حقول النص ---
inline std::string textfield_get_text(int id) {
    return sad::unified::SadUI::get().getInputText(id);
}
inline void textfield_set_text(int id, const std::string& text) {
    sad::unified::SadUI::get().setText(id, text);
}

// --- قراءة الخصائص ---
inline bool widget_get_checked(int id) {
    return sad::unified::SadUI::get().getBool(id, "checked");
}
inline float widget_get_value(int id) {
    return sad::unified::SadUI::get().getFloat(id, "value");
}
inline int widget_get_selected(int id) {
    // إرجاع الفهرس المحدد للـ Dropdown, TabBar, BottomBar, Pagination
    return static_cast<int>(sad::unified::SadUI::get().getFloat(id, "selectedIndex"));
}
inline bool widget_get_expanded(int id) {
    return sad::unified::SadUI::get().getBool(id, "expanded");
}
inline bool widget_is_dialog_visible(int id) {
    return sad::unified::SadUI::get().getBool(id, "visible");
}

// --- خصائص متقدمة ---
inline void widget_set_opacity(int id, float v) {
    sad::unified::SadUI::get().setFloat(id, "opacity", v);
}
inline void widget_set_enabled(int id, bool v) {
    sad::unified::SadUI::get().setBool(id, "enabled", v);
}
inline void widget_set_shadow(int id, float radius, float ox, float oy, int r, int g, int b, int a) {
    sad::unified::SadUI::get().setFloat(id, "elevation", radius);
}
inline void widget_set_tooltip(int id, const std::string& text) {
    sad::unified::SadUI::get().setString(id, "tooltip", text);
}
inline void widget_set_alignment(int id, const std::string& main, const std::string& cross) {
    sad::unified::SadUI::get().setAlignment(id, main, cross);
}
inline void widget_set_font_weight(int id, const std::string& weight) {
    sad::unified::SadUI::get().setString(id, "fontWeight", weight);
}
inline void widget_set_italic(int id, bool v) {
    sad::unified::SadUI::get().setBool(id, "italic", v);
}
inline void widget_set_line_height(int id, float v) {
    sad::unified::SadUI::get().setFloat(id, "lineHeight", v);
}
inline void widget_set_rotation(int id, float v) {
    sad::unified::SadUI::get().setFloat(id, "rotation", v);
}
inline void widget_set_scale(int id, float sx, float sy) {
    sad::unified::SadUI::get().setFloat(id, "scaleX", sx);
}
inline void widget_set_z_index(int id, int v) {
    sad::unified::SadUI::get().setInt(id, "zIndex", v);
}
inline void widget_set_min_size(int id, float w, float h) {
    sad::unified::SadUI::get().setFloat(id, "minWidth", w);
    sad::unified::SadUI::get().setFloat(id, "minHeight", h);
}
inline void widget_set_max_size(int id, float w, float h) {
    sad::unified::SadUI::get().setFloat(id, "maxWidth", w);
    sad::unified::SadUI::get().setFloat(id, "maxHeight", h);
}
inline void widget_set_gradient(int id, int r1, int g1, int b1, int r2, int g2, int b2) {
    sad::unified::SadUI::get().setColor(id, "bg", r1, g1, b1, 255);
}
inline void widget_set_hover_color(int id, int r, int g, int b, int a) {
    sad::unified::SadUI::get().setColor(id, "hover", r, g, b, a);
}
inline void widget_set_press_color(int id, int r, int g, int b, int a) {
    sad::unified::SadUI::get().setColor(id, "press", r, g, b, a);
}

// --- حياة التطبيق ---
inline int app_create(const std::string& title, int w, int h) {
    return sad::unified::SadUI::get().appCreate(title, w, h);
}
inline bool app_is_running(int id) {
    return sad::unified::SadUI::get().appIsRunning(id);
}
inline void app_update(int id) {
    sad::unified::SadUI::get().appUpdate(id);
}
inline void app_set_content(int id, int widgetId) {
    sad::unified::SadUI::get().appSetContent(id, widgetId);
}
inline void app_set_bg_color(int id, int r, int g, int b) {
    sad::unified::SadUI::get().appSetBgColor(id, r, g, b);
}
inline void app_close(int id) {
    sad::unified::SadUI::get().appClose(id);
}
inline void app_set_title(int id, const std::string& title) {
    sad::unified::SadUI::get().appSetTitle(id, title);
}
inline int app_get_width(int id) {
    return sad::unified::SadUI::get().appGetWidth(id);
}
inline int app_get_height(int id) {
    return sad::unified::SadUI::get().appGetHeight(id);
}
inline double app_get_fps(int id) {
    return sad::unified::SadUI::get().appGetFps(id);
}

// --- تنظيف ---
inline void widgetCleanupAll() { /* unified backend manages memory */ }
inline void widgetCleanupTree(int id) { /* unified backend manages memory */ }

// --- استعلام ---
inline bool isKnownPropertyName(const std::string& name) {
    static const std::unordered_set<std::string> known = {
        // ── خصائص نصية ──
        "text", "نص", "title", "عنوان", "hint", "تلميح",
        "placeholder", "عنصر_نائب", "label", "تسمية",
        "icon", "أيقونة", "textAlign", "محاذاة_النص",
        "fontFamily", "عائلة_خط", "fontWeight", "وزن_الخط",
        "textDecoration", "زخرفة_نص", "overflow", "فيض",
        "tooltip", "تلميح_عائم", "subtitle", "عنوان_فرعي",
        "cursor", "مؤشر", "direction", "اتجاه",
        "display", "عرض_نمط", "position", "موضع",
        "name", "اسم", "type", "نوع", "src", "مصدر",
        // ── خصائص رقمية ──
        "fontSize", "حجم_الخط", "width", "عرض",
        "height", "ارتفاع", "padding", "حشو",
        "margin", "هامش", "borderRadius", "استدارة_الحدود",
        "borderWidth", "عرض_الحدود", "opacity", "شفافية",
        "elevation", "ارتفاع_ظل", "spacing", "تباعد",
        "gap", "فراغ_بين", "flex", "مرونة",
        "rotation", "دوران", "zIndex", "ترتيب",
        "maxLines", "أقصى_سطور", "minWidth", "أدنى_عرض",
        "maxWidth", "أقصى_عرض", "minHeight", "أدنى_ارتفاع",
        "maxHeight", "أقصى_ارتفاع", "columns", "أعمدة",
        "rows", "صفوف", "value", "قيمة",
        "min", "أدنى", "max", "أقصى",
        "lineHeight", "ارتفاع_سطر", "letterSpacing", "تباعد_أحرف",
        "tabIndex", "ترتيب_تبويب", "shadowRadius", "نصف_قطر_ظل",
        "shadowOffsetX", "إزاحة_ظل_أفقي", "shadowOffsetY", "إزاحة_ظل_عمودي",
        "scrollX", "تمرير_أفقي", "scrollY", "تمرير_عمودي",
        "scaleX", "مقياس_أفقي", "scaleY", "مقياس_عمودي",
        "paddingTop", "حشو_أعلى", "paddingRight", "حشو_يمين",
        "paddingBottom", "حشو_أسفل", "paddingLeft", "حشو_يسار",
        "marginTop", "هامش_أعلى", "marginRight", "هامش_يمين",
        "marginBottom", "هامش_أسفل", "marginLeft", "هامش_يسار",
        "size", "حجم", "page", "صفحة", "level", "مستوى",
        // ── ألوان ──
        "color", "لون", "خلفية", "background", "لون_نص",
        "foreground", "لون_حدود", "borderColor",
        "accentColor", "لون_تمييز",
        // ── منطقية ──
        "visible", "مرئي", "enabled", "مفعل",
        "checked", "محدد", "expanded", "موسع",
        "selected", "مختار", "rtl", "يمين_لليسار",
        "italic", "مائل", "bold", "عريض",
        "wrap", "التفاف", "editable", "قابل_للتعديل",
        "resizable", "قابل_للتغيير", "scrollable", "قابل_للتمرير",
        "active", "نشط",
        // ── أحداث ──
        "عند_الضغط", "onClick",
        "عند_التغيير", "onChange",
        "عند_التركيز", "onFocus",
        "عند_التحويم", "onHover",
        "عند_فقد_التركيز", "onBlur"
    };
    return known.count(name) > 0;
}

// --- دوال متنوعة (stubs) ---
inline float widget_get_opacity(int id) { return 1.0f; }
inline bool widget_is_enabled(int id) { return true; }
inline float widget_get_shadow_radius(int id) { return 0.0f; }
inline std::string widget_get_tooltip(int id) { return ""; }
inline std::string widget_get_icon_text(int id) { return ""; }
inline float widget_get_scroll_y(int id) { return 0.0f; }
inline float widget_get_scroll_x(int id) { return 0.0f; }
inline std::string widget_get_name(int id) { return ""; }
inline std::string widget_get_type_name(int id) { return ""; }
inline int widget_get_children_count(int id) { return 0; }
inline int widget_get_child_at(int id, int index) { return -1; }
inline float widget_get_x(int id) { return 0.0f; }
inline float widget_get_y(int id) { return 0.0f; }
inline float widget_get_width(int id) { return 0.0f; }
inline float widget_get_height(int id) { return 0.0f; }
inline float widget_get_value2(int id) { return 0.0f; }
inline std::string widget_get_input_text(int id) { return sad::unified::SadUI::get().getInputText(id); }
inline bool widget_get_masked(int id) { return false; }
inline int widget_count() { return 0; }
inline int widget_get_parent(int id) { return sad::unified::SadUI::get().getParent(id); }
inline int widget_find_by_name(const std::string& name) { return -1; }
inline int widget_find_by_name_fast(const std::string& name) { return -1; }
inline int widget_clone(int id) { return -1; }
inline int widget_deep_clone(int id) { return -1; }
inline void widget_remove(int id) {}
inline void widget_reparent(int id, int newParent) {
    sad::unified::SadUI::get().removeChild(sad::unified::SadUI::get().getParent(id), id);
    sad::unified::SadUI::get().addChild(newParent, id);
}
inline void widget_scroll_by(int id, float v) {}
inline void widget_scroll_to(int id, float v) {}
inline void widget_move_to(int id, float x, float y) {}
inline void widget_set_tab_index(int id, int v) {}
inline void widget_set_icon_text(int id, const std::string& t) {}
inline void widget_set_scroll_position(int id, float x, float y) {}
inline void widget_set_overflow(int id, const std::string& v) {}
inline void widget_set_name(int id, const std::string& n) {}
inline void widget_set_total_pages(int id, int n) {}
inline void widget_set_value2(int id, float v) {}
inline void widget_set_input_text(int id, const std::string& t) {
    sad::unified::SadUI::get().setText(id, t);
}
inline void widget_clear_items(int id) {}
inline void widget_clear_data(int id) {}
inline void widget_add_data_color(int id, int r, int g, int b) {}
inline void widget_set_masked(int id, bool v) {}
inline void widgetSetPropString_v2(int id, const std::string& prop, const std::string& val) {
    sad::unified::SadUI::get().setString(id, prop, val);
}
inline void widgetSetChildren(int parent, const std::vector<int>& children) {
    sad::unified::SadUI::get().clearChildren(parent);
    for (int c : children) sad::unified::SadUI::get().addChild(parent, c);
}

// --- أحداث ---
inline void widgetSetOnClick(int id, int callbackId) {}
inline void widgetSetOnHover(int id, int callbackId) {}
inline void widgetSetOnFocus(int id, int callbackId) {}
inline void widgetSetOnBlur(int id, int callbackId) {}
inline void widgetSetOnChange(int id, int callbackId) {}

// --- سمة ---
inline void setThemeProperty(const std::string& key, const std::string& val) {}

// --- أحداث المُستمع ---
struct UIEvent {
    std::string type;
    int widgetId = 0;
    std::string text;
    double number = 0;
    bool boolVal = false;
};
inline std::vector<UIEvent> collectEvents(int appId) { return {}; }
inline int events_count(int appId) { return 0; }
inline std::string event_get_type(int idx) { return ""; }
inline int event_get_widget_id(int idx) { return -1; }
inline std::string event_get_text(int idx) { return ""; }
inline double event_get_number(int idx) { return 0; }
inline bool event_get_bool(int idx) { return false; }
inline void events_clear() {}

// --- الحصول على مكون (stub) ---
inline void* getWidget(int id) { return nullptr; }

// --- حياة التطبيق إضافية ---
inline std::string app_get_title(int id) { return ""; }
inline double app_get_delta_time(int id) { return 1.0/60.0; }
inline void app_set_size(int id, int w, int h) {}

}} // namespace sad::ui

#endif // platform switch
