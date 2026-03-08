/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_backend_desktop.cpp — محوّل (Adapter) من الواجهة الموحدة إلى SDL2
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  يستدعي مباشرة دوال sad::ui:: (sad_ui_framework.h) الموجودة مسبقاً.
 *  لا يُعيد تنفيذ أي شيء — فقط يُترجم من unified API إلى legacy API.
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include "sad_backend_desktop.h"

// استدعاء النظام القديم مباشرة
#include "../graphics/sad_ui_framework.h"

#include <unordered_map>

namespace sad {
namespace unified {

// ═══════════════════════════════════════════════════════════════════
// تحويل WidgetType الموحد → WidgetType القديم (sad::ui)
// ═══════════════════════════════════════════════════════════════════

static sad::ui::WidgetType toLegacy(WidgetType t) {
    // خريطة ثابتة لتحويل الأنواع
    static const std::unordered_map<int, sad::ui::WidgetType> map = {
        {(int)WidgetType::Container,     sad::ui::WidgetType::Container},
        {(int)WidgetType::Column,        sad::ui::WidgetType::Column},
        {(int)WidgetType::Row,           sad::ui::WidgetType::Row},
        {(int)WidgetType::Stack,         sad::ui::WidgetType::Stack},
        {(int)WidgetType::Center,        sad::ui::WidgetType::Center},
        {(int)WidgetType::Wrap,          sad::ui::WidgetType::Wrap},
        {(int)WidgetType::ScrollView,    sad::ui::WidgetType::ScrollView},
        {(int)WidgetType::SizedBox,      sad::ui::WidgetType::SizedBox},
        {(int)WidgetType::Expanded,      sad::ui::WidgetType::Expanded},
        {(int)WidgetType::Card,          sad::ui::WidgetType::Card},
        {(int)WidgetType::Grid,          sad::ui::WidgetType::GridView},
        {(int)WidgetType::ListView,      sad::ui::WidgetType::ListView},
        {(int)WidgetType::PageView,      sad::ui::WidgetType::PageView},
        {(int)WidgetType::Table,         sad::ui::WidgetType::Table},
        {(int)WidgetType::Text,          sad::ui::WidgetType::Text},
        {(int)WidgetType::Label,         sad::ui::WidgetType::Label},
        {(int)WidgetType::Heading,       sad::ui::WidgetType::Heading},
        {(int)WidgetType::Paragraph,     sad::ui::WidgetType::Paragraph},
        {(int)WidgetType::Link,          sad::ui::WidgetType::Link},
        {(int)WidgetType::CodeBlock,     sad::ui::WidgetType::CodeBlock},
        {(int)WidgetType::Quote,         sad::ui::WidgetType::Quote},
        {(int)WidgetType::Markdown,      sad::ui::WidgetType::Markdown},
        {(int)WidgetType::Button,        sad::ui::WidgetType::Button},
        {(int)WidgetType::IconButton,    sad::ui::WidgetType::IconButton},
        {(int)WidgetType::FloatingButton,sad::ui::WidgetType::FloatingButton},
        {(int)WidgetType::OutlineButton, sad::ui::WidgetType::OutlineButton},
        {(int)WidgetType::TextButton,    sad::ui::WidgetType::TextButton},
        {(int)WidgetType::ToggleButton,  sad::ui::WidgetType::ToggleButton},
        {(int)WidgetType::ButtonGroup,   sad::ui::WidgetType::ButtonGroup},
        {(int)WidgetType::TextField,     sad::ui::WidgetType::TextField},
        {(int)WidgetType::PasswordField, sad::ui::WidgetType::PasswordField},
        {(int)WidgetType::TextArea,      sad::ui::WidgetType::TextArea},
        {(int)WidgetType::Checkbox,      sad::ui::WidgetType::Checkbox},
        {(int)WidgetType::Radio,         sad::ui::WidgetType::Radio},
        {(int)WidgetType::Switch,        sad::ui::WidgetType::Switch},
        {(int)WidgetType::Slider,        sad::ui::WidgetType::Slider},
        {(int)WidgetType::RangeSlider,   sad::ui::WidgetType::RangeSlider},
        {(int)WidgetType::Dropdown,      sad::ui::WidgetType::Dropdown},
        {(int)WidgetType::NumberInput,   sad::ui::WidgetType::NumberInput},
        {(int)WidgetType::SearchField,   sad::ui::WidgetType::SearchField},
        {(int)WidgetType::DateField,     sad::ui::WidgetType::DateField},
        {(int)WidgetType::ColorPicker,   sad::ui::WidgetType::ColorPicker},
        {(int)WidgetType::RatingStars,   sad::ui::WidgetType::RatingStars},
        {(int)WidgetType::ToggleGroup,   sad::ui::WidgetType::ToggleGroup},
        {(int)WidgetType::Icon,          sad::ui::WidgetType::Icon},
        {(int)WidgetType::Image,         sad::ui::WidgetType::ImageWidget},
        {(int)WidgetType::Avatar,        sad::ui::WidgetType::Avatar},
        {(int)WidgetType::Badge,         sad::ui::WidgetType::Badge},
        {(int)WidgetType::Chip,          sad::ui::WidgetType::Chip},
        {(int)WidgetType::Tag,           sad::ui::WidgetType::Tag},
        {(int)WidgetType::StatusDot,     sad::ui::WidgetType::StatusDot},
        {(int)WidgetType::ColorBox,      sad::ui::WidgetType::ColorBox},
        {(int)WidgetType::GradientBox,   sad::ui::WidgetType::GradientBox},
        {(int)WidgetType::Divider,       sad::ui::WidgetType::Divider},
        {(int)WidgetType::Spacer,        sad::ui::WidgetType::Spacer},
        {(int)WidgetType::Separator,     sad::ui::WidgetType::Separator},
        {(int)WidgetType::ProgressBar,   sad::ui::WidgetType::ProgressBar},
        {(int)WidgetType::CircleProgress,sad::ui::WidgetType::CircleProgress},
        {(int)WidgetType::LoadingSpinner,sad::ui::WidgetType::LoadingSpinner},
        {(int)WidgetType::Skeleton,      sad::ui::WidgetType::Skeleton},
        {(int)WidgetType::Gauge,         sad::ui::WidgetType::Gauge},
        {(int)WidgetType::AppBar,        sad::ui::WidgetType::AppBar},
        {(int)WidgetType::BottomBar,     sad::ui::WidgetType::BottomBar},
        {(int)WidgetType::TabBar,        sad::ui::WidgetType::TabBar},
        {(int)WidgetType::Drawer,        sad::ui::WidgetType::Drawer},
        {(int)WidgetType::Breadcrumb,    sad::ui::WidgetType::Breadcrumb},
        {(int)WidgetType::NavItem,       sad::ui::WidgetType::NavItem},
        {(int)WidgetType::MenuItem,      sad::ui::WidgetType::MenuItem},
        {(int)WidgetType::Toolbar,       sad::ui::WidgetType::Toolbar},
        {(int)WidgetType::StatusBar,     sad::ui::WidgetType::StatusBar},
        {(int)WidgetType::SideMenu,      sad::ui::WidgetType::SideMenu},
        {(int)WidgetType::Dialog,        sad::ui::WidgetType::Dialog},
        {(int)WidgetType::Alert,         sad::ui::WidgetType::Alert},
        {(int)WidgetType::Toast,         sad::ui::WidgetType::Toast},
        {(int)WidgetType::Snackbar,      sad::ui::WidgetType::Snackbar},
        {(int)WidgetType::Banner,        sad::ui::WidgetType::Banner},
        {(int)WidgetType::Modal,         sad::ui::WidgetType::Modal},
        {(int)WidgetType::Popover,       sad::ui::WidgetType::Popover},
        {(int)WidgetType::Notification,  sad::ui::WidgetType::Notification},
        {(int)WidgetType::ConfirmDialog, sad::ui::WidgetType::ConfirmDialog},
        {(int)WidgetType::Tooltip,       sad::ui::WidgetType::Tooltip},
        {(int)WidgetType::InfoCard,      sad::ui::WidgetType::InfoCard},
        {(int)WidgetType::StatCard,      sad::ui::WidgetType::StatCard},
        {(int)WidgetType::ExpansionPanel,sad::ui::WidgetType::ExpansionPanel},
        {(int)WidgetType::Accordion,     sad::ui::WidgetType::Accordion},
        {(int)WidgetType::Panel,         sad::ui::WidgetType::Panel},
        {(int)WidgetType::GroupBox,      sad::ui::WidgetType::GroupBox},
        {(int)WidgetType::Callout,       sad::ui::WidgetType::Callout},
        {(int)WidgetType::ListTile,      sad::ui::WidgetType::ListTile},
        {(int)WidgetType::DataTable,     sad::ui::WidgetType::DataTable},
        {(int)WidgetType::TableRow,      sad::ui::WidgetType::TableRow},
        {(int)WidgetType::TableCell,     sad::ui::WidgetType::TableCell},
        {(int)WidgetType::DetailRow,     sad::ui::WidgetType::DetailRow},
        {(int)WidgetType::Timeline,      sad::ui::WidgetType::Timeline},
        {(int)WidgetType::TimelineItem,  sad::ui::WidgetType::TimelineItem},
        {(int)WidgetType::TreeItem,      sad::ui::WidgetType::TreeItem},
        {(int)WidgetType::Stepper,       sad::ui::WidgetType::Stepper},
        {(int)WidgetType::StepIndicator, sad::ui::WidgetType::StepIndicator},
        {(int)WidgetType::BarChart,      sad::ui::WidgetType::BarChart},
        {(int)WidgetType::HBarChart,     sad::ui::WidgetType::HBarChart},
        {(int)WidgetType::PieChart,      sad::ui::WidgetType::PieChart},
        {(int)WidgetType::LineChart,     sad::ui::WidgetType::LineChart},
        {(int)WidgetType::SparkLine,     sad::ui::WidgetType::SparkLine},
        {(int)WidgetType::DonutChart,    sad::ui::WidgetType::DonutChart},
        {(int)WidgetType::Pagination,    sad::ui::WidgetType::Pagination},
        {(int)WidgetType::DotIndicator,  sad::ui::WidgetType::DotIndicator},
        {(int)WidgetType::Canvas,        sad::ui::WidgetType::Canvas},
        {(int)WidgetType::Carousel,      sad::ui::WidgetType::Carousel},
        {(int)WidgetType::SplitView,     sad::ui::WidgetType::SplitView},
    };

    auto it = map.find((int)t);
    if (it != map.end()) return it->second;
    // المكونات الجديدة التي لا يوجد لها نظير — نُعيدها كـ Container
    return sad::ui::WidgetType::Container;
}

// ═══════════════════════════════════════════════════════════════════
// التنفيذ الفعلي — يستدعي sad::ui::* مباشرة
// ═══════════════════════════════════════════════════════════════════

DesktopBackend::DesktopBackend() {}
DesktopBackend::~DesktopBackend() {}

int DesktopBackend::appCreate(const std::string& title, int width, int height) {
    return sad::ui::app_create(title, width, height);
}

bool DesktopBackend::appIsRunning(int appId) {
    return sad::ui::app_is_running(appId);
}

void DesktopBackend::appUpdate(int appId) {
    sad::ui::app_update(appId);
}

void DesktopBackend::appSetContent(int appId, WidgetId rootWidget) {
    sad::ui::app_set_content(appId, rootWidget);
}

void DesktopBackend::appSetBgColor(int appId, int r, int g, int b) {
    sad::ui::app_set_bg_color(appId, r, g, b);
}

void DesktopBackend::appClose(int appId) {
    sad::ui::app_close(appId);
}

void DesktopBackend::appSetTitle(int appId, const std::string& title) {
    sad::ui::app_set_title(appId, title);
}

int DesktopBackend::appGetWidth(int appId) {
    return sad::ui::app_get_width(appId);
}

int DesktopBackend::appGetHeight(int appId) {
    return sad::ui::app_get_height(appId);
}

double DesktopBackend::appGetFps(int appId) {
    return sad::ui::app_get_fps(appId);
}

WidgetId DesktopBackend::createWidget(WidgetType type) {
    auto legacy = toLegacy(type);
    // نستخدم الدوال المتخصصة الموجودة — دعم شامل لـ 109 widget
    switch (legacy) {
        // === Layout (10) ===
        case sad::ui::WidgetType::Column:       return sad::ui::widget_column();
        case sad::ui::WidgetType::Row:          return sad::ui::widget_row();
        case sad::ui::WidgetType::Stack:        return sad::ui::widget_stack();
        case sad::ui::WidgetType::Center:       return sad::ui::widget_center();
        case sad::ui::WidgetType::Wrap:         return sad::ui::widget_wrap();
        case sad::ui::WidgetType::GridView:     return sad::ui::widget_gridview(2);
        case sad::ui::WidgetType::ListView:     return sad::ui::widget_listview();
        case sad::ui::WidgetType::ScrollView:   return sad::ui::widget_scrollview();
        case sad::ui::WidgetType::SizedBox:     return sad::ui::widget_sizedbox(100, 100);
        case sad::ui::WidgetType::Expanded:     return sad::ui::widget_expanded();
        case sad::ui::WidgetType::PageView:     return sad::ui::widget_pageview();
        case sad::ui::WidgetType::Table:        return sad::ui::widget_table(2);

        // === Buttons (7) ===
        case sad::ui::WidgetType::Button:       return sad::ui::widget_button("");
        case sad::ui::WidgetType::IconButton:   return sad::ui::widget_icon_button('*');
        case sad::ui::WidgetType::FloatingButton: return sad::ui::widget_floating_button('+');
        case sad::ui::WidgetType::OutlineButton:return sad::ui::widget_outline_button("");
        case sad::ui::WidgetType::TextButton:   return sad::ui::widget_text_button("");
        case sad::ui::WidgetType::ToggleButton: return sad::ui::widget_toggle_button("");
        case sad::ui::WidgetType::ButtonGroup:  return sad::ui::widget_button_group();

        // === Text (8) ===
        case sad::ui::WidgetType::Text:         return sad::ui::widget_text("");
        case sad::ui::WidgetType::Label:        return sad::ui::widget_label("");
        case sad::ui::WidgetType::Heading:      return sad::ui::widget_heading("", 1);
        case sad::ui::WidgetType::Paragraph:    return sad::ui::widget_paragraph("");
        case sad::ui::WidgetType::Link:         return sad::ui::widget_link("");
        case sad::ui::WidgetType::CodeBlock:    return sad::ui::widget_code_block("");
        case sad::ui::WidgetType::Quote:        return sad::ui::widget_quote("");
        case sad::ui::WidgetType::Markdown:     return sad::ui::widget_markdown("");

        // === Input (14) ===
        case sad::ui::WidgetType::TextField:    return sad::ui::widget_textfield("");
        case sad::ui::WidgetType::Checkbox:     return sad::ui::widget_checkbox("");
        case sad::ui::WidgetType::Radio:        return sad::ui::widget_radio("");
        case sad::ui::WidgetType::Switch:       return sad::ui::widget_switch("");
        case sad::ui::WidgetType::Slider:       return sad::ui::widget_slider(0, 100);
        case sad::ui::WidgetType::Dropdown:     return sad::ui::widget_dropdown("");
        case sad::ui::WidgetType::NumberInput:  return sad::ui::widget_number_input(0, 100);
        case sad::ui::WidgetType::SearchField:  return sad::ui::widget_search_field("");
        case sad::ui::WidgetType::PasswordField:return sad::ui::widget_password_field("");
        case sad::ui::WidgetType::TextArea:     return sad::ui::widget_textarea("");
        case sad::ui::WidgetType::DateField:    return sad::ui::widget_date_field();
        case sad::ui::WidgetType::RangeSlider:  return sad::ui::widget_range_slider(0, 100);
        case sad::ui::WidgetType::ColorPicker:  return sad::ui::widget_color_picker();
        case sad::ui::WidgetType::RatingStars:  return sad::ui::widget_rating_stars(5);
        case sad::ui::WidgetType::ToggleGroup:  return sad::ui::widget_toggle_group();

        // === Visual (11) ===
        case sad::ui::WidgetType::Icon:         return sad::ui::widget_icon('*');
        case sad::ui::WidgetType::ImageWidget:  return sad::ui::widget_image("");
        case sad::ui::WidgetType::Avatar:       return sad::ui::widget_avatar("A", 100, 100, 200);
        case sad::ui::WidgetType::Badge:        return sad::ui::widget_badge(0);
        case sad::ui::WidgetType::Chip:         return sad::ui::widget_chip("");
        case sad::ui::WidgetType::Tag:          return sad::ui::widget_tag("", 100, 100, 100);
        case sad::ui::WidgetType::StatusDot:    return sad::ui::widget_status_dot(0, 255, 0);
        case sad::ui::WidgetType::ColorBox:     return sad::ui::widget_color_box(128, 128, 128);
        case sad::ui::WidgetType::GradientBox:  return sad::ui::widget_gradient_box(255,0,0, 0,0,255);
        case sad::ui::WidgetType::Divider:      return sad::ui::widget_divider();
        case sad::ui::WidgetType::Spacer:       return sad::ui::widget_spacer(8);
        case sad::ui::WidgetType::Separator:    return sad::ui::widget_separator();

        // === Progress (5) ===
        case sad::ui::WidgetType::ProgressBar:  return sad::ui::widget_progress_bar(0);
        case sad::ui::WidgetType::CircleProgress: return sad::ui::widget_circle_progress(0);
        case sad::ui::WidgetType::LoadingSpinner: return sad::ui::widget_loading_spinner();
        case sad::ui::WidgetType::Skeleton:     return sad::ui::widget_skeleton();
        case sad::ui::WidgetType::Gauge:        return sad::ui::widget_gauge(0, 100);

        // === Navigation (10) ===
        case sad::ui::WidgetType::AppBar:       return sad::ui::widget_app_bar("");
        case sad::ui::WidgetType::BottomBar:    return sad::ui::widget_bottom_bar();
        case sad::ui::WidgetType::TabBar:       return sad::ui::widget_tab_bar();
        case sad::ui::WidgetType::Drawer:       return sad::ui::widget_drawer();
        case sad::ui::WidgetType::Breadcrumb:   return sad::ui::widget_breadcrumb();
        case sad::ui::WidgetType::NavItem:      return sad::ui::widget_nav_item("");
        case sad::ui::WidgetType::MenuItem:     return sad::ui::widget_menu_item("");
        case sad::ui::WidgetType::Toolbar:      return sad::ui::widget_toolbar();
        case sad::ui::WidgetType::StatusBar:    return sad::ui::widget_status_bar("");
        case sad::ui::WidgetType::SideMenu:     return sad::ui::widget_side_menu();

        // === Feedback (10) ===
        case sad::ui::WidgetType::Dialog:       return sad::ui::widget_dialog("", "");
        case sad::ui::WidgetType::Alert:        return sad::ui::widget_alert("", "info");
        case sad::ui::WidgetType::Toast:        return sad::ui::widget_toast("");
        case sad::ui::WidgetType::Snackbar:     return sad::ui::widget_snackbar("");
        case sad::ui::WidgetType::Banner:       return sad::ui::widget_banner("");
        case sad::ui::WidgetType::Modal:        return sad::ui::widget_modal();
        case sad::ui::WidgetType::Popover:      return sad::ui::widget_popover();
        case sad::ui::WidgetType::Notification: return sad::ui::widget_notification("", "");
        case sad::ui::WidgetType::ConfirmDialog:return sad::ui::widget_confirm_dialog("", "");
        case sad::ui::WidgetType::Tooltip:      return sad::ui::widget_tooltip("");

        // === Cards & Panels (7) ===
        case sad::ui::WidgetType::Card:         return sad::ui::widget_card();
        case sad::ui::WidgetType::InfoCard:     return sad::ui::widget_info_card("", "");
        case sad::ui::WidgetType::StatCard:     return sad::ui::widget_stat_card("", "");
        case sad::ui::WidgetType::ExpansionPanel: return sad::ui::widget_expansion_panel("");
        case sad::ui::WidgetType::Accordion:    return sad::ui::widget_accordion();
        case sad::ui::WidgetType::Panel:        return sad::ui::widget_panel("");
        case sad::ui::WidgetType::GroupBox:     return sad::ui::widget_group_box("");
        case sad::ui::WidgetType::Callout:      return sad::ui::widget_callout("", "info");

        // === Data (10) ===
        case sad::ui::WidgetType::ListTile:     return sad::ui::widget_list_tile("", "");
        case sad::ui::WidgetType::DataTable:    return sad::ui::widget_data_table(3);
        case sad::ui::WidgetType::TableRow:     return sad::ui::widget_table_row();
        case sad::ui::WidgetType::TableCell:    return sad::ui::widget_table_cell("");
        case sad::ui::WidgetType::DetailRow:    return sad::ui::widget_detail_row("", "");
        case sad::ui::WidgetType::Timeline:     return sad::ui::widget_timeline();
        case sad::ui::WidgetType::TimelineItem: return sad::ui::widget_timeline_item("");
        case sad::ui::WidgetType::TreeItem:     return sad::ui::widget_tree_item("", 0);
        case sad::ui::WidgetType::Stepper:      return sad::ui::widget_stepper(3);
        case sad::ui::WidgetType::StepIndicator:return sad::ui::widget_step_indicator(3, 0);

        // === Charts (7) ===
        case sad::ui::WidgetType::BarChart:     return sad::ui::widget_bar_chart();
        case sad::ui::WidgetType::HBarChart:    return sad::ui::widget_hbar_chart();
        case sad::ui::WidgetType::PieChart:     return sad::ui::widget_pie_chart();
        case sad::ui::WidgetType::LineChart:    return sad::ui::widget_line_chart();
        case sad::ui::WidgetType::SparkLine:    return sad::ui::widget_spark_line();
        case sad::ui::WidgetType::DonutChart:   return sad::ui::widget_donut_chart();

        // === Pagination (2) ===
        case sad::ui::WidgetType::Pagination:   return sad::ui::widget_pagination(10);
        case sad::ui::WidgetType::DotIndicator: return sad::ui::widget_dot_indicator(5, 0);

        // === Advanced (3) ===
        case sad::ui::WidgetType::Canvas:       return sad::ui::widget_canvas(200, 200);
        case sad::ui::WidgetType::Carousel:     return sad::ui::widget_carousel();
        case sad::ui::WidgetType::SplitView:    return sad::ui::widget_split_view();

        default:
            // عنصر غير معروف — يُنشأ كـ Container
            return sad::ui::widget_container();
    }
}

void DesktopBackend::setText(WidgetId id, const std::string& text) {
    sad::ui::widgetSetPropString_v2(id, "text", text);
}

void DesktopBackend::setColor(WidgetId id, const std::string& prop, int r, int g, int b, int a) {
    if (prop == "bg" || prop == "background") {
        sad::ui::widget_set_bg(id, r, g, b);
    } else if (prop == "text" || prop == "foreground") {
        sad::ui::text_set_color(id, r, g, b);
    } else if (prop == "border") {
        sad::ui::widget_set_border(id, 1.0f, r, g, b);
    } else if (prop == "accent") {
        sad::ui::widgetSetProp(id, "لون_التمييز", std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b));
    }
}

void DesktopBackend::setFloat(WidgetId id, const std::string& prop, float value) {
    if (prop == "fontSize") {
        sad::ui::text_set_size(id, value);
    } else if (prop == "borderRadius") {
        sad::ui::widget_set_border_radius(id, value);
    } else if (prop == "opacity") {
        sad::ui::widget_set_opacity(id, value);
    } else if (prop == "elevation") {
        sad::ui::widgetSetProp(id, "elevation", (double)value);
    } else if (prop == "flex") {
        sad::ui::widget_set_flex(id, value);
    } else if (prop == "value") {
        sad::ui::widget_set_value(id, value);
    } else {
        sad::ui::widgetSetProp(id, prop, (double)value);
    }
}

void DesktopBackend::setInt(WidgetId id, const std::string& prop, int value) {
    sad::ui::widgetSetProp(id, prop, (double)value);
}

void DesktopBackend::setBool(WidgetId id, const std::string& prop, bool value) {
    if (prop == "visible") {
        sad::ui::widget_set_visible(id, value);
    } else if (prop == "enabled") {
        sad::ui::widget_set_enabled(id, value);
    } else if (prop == "checked") {
        sad::ui::widget_set_checked(id, value);
    } else {
        sad::ui::widgetSetProp(id, prop, value);
    }
}

void DesktopBackend::setString(WidgetId id, const std::string& prop, const std::string& value) {
    sad::ui::widgetSetPropString_v2(id, prop, value);
}

std::string DesktopBackend::getText(WidgetId id) {
    return sad::ui::textfield_get_text(id);
}

float DesktopBackend::getFloat(WidgetId id, const std::string& prop) {
    if (prop == "value") return sad::ui::widget_get_value(id);
    return 0;
}

bool DesktopBackend::getBool(WidgetId id, const std::string& prop) {
    if (prop == "clicked") return sad::ui::button_was_clicked(id);
    if (prop == "checked") return sad::ui::widget_get_checked(id);
    return false;
}

void DesktopBackend::addChild(WidgetId parent, WidgetId child) {
    sad::ui::widget_add_child(parent, child);
}

void DesktopBackend::removeChild(WidgetId parent, WidgetId child) {
    sad::ui::widget_remove_child(parent, child);
}

void DesktopBackend::clearChildren(WidgetId parent) {
    sad::ui::widget_clear_children(parent);
}

int DesktopBackend::getParent(WidgetId id) {
    return sad::ui::widget_get_parent(id);
}

void DesktopBackend::setPadding(WidgetId id, float top, float right, float bottom, float left) {
    sad::ui::widget_set_padding4(id, top, right, bottom, left);
}

void DesktopBackend::setMargin(WidgetId id, float top, float right, float bottom, float left) {
    sad::ui::widget_set_margin4(id, top, right, bottom, left);
}

void DesktopBackend::setAlignment(WidgetId id, const std::string& main, const std::string& cross) {
    sad::ui::widget_set_alignment(id, main, cross);
}

void DesktopBackend::setSpacing(WidgetId id, float spacing) {
    sad::ui::widget_set_spacing(id, spacing);
}

void DesktopBackend::setFlex(WidgetId id, float flex) {
    sad::ui::widget_set_flex(id, flex);
}

bool DesktopBackend::wasClicked(WidgetId id) {
    return sad::ui::button_was_clicked(id);
}

std::string DesktopBackend::getInputText(WidgetId id) {
    return sad::ui::textfield_get_text(id);
}

}} // namespace sad::unified
