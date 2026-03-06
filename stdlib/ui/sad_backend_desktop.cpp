/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_backend_desktop.cpp — محوّل (Adapter) من الواجهة الموحدة إلى SDL2
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  يستدعي مباشرة دوال sad::ui:: (sad_ui_framework.h) الموجودة مسبقاً.
 *  لا يُعيد تنفيذ أي شيء — فقط يُترجم من unified API إلى legacy API.
 * ═══════════════════════════════════════════════════════════════════════════════
 */

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
    // نستخدم الدوال المتخصصة الموجودة
    switch (legacy) {
        case sad::ui::WidgetType::Button:   return sad::ui::widget_button("");
        case sad::ui::WidgetType::Text:     return sad::ui::widget_text("");
        case sad::ui::WidgetType::Column:   return sad::ui::widget_column();
        case sad::ui::WidgetType::Row:      return sad::ui::widget_row();
        case sad::ui::WidgetType::Card:     return sad::ui::widget_card();
        case sad::ui::WidgetType::Divider:  return sad::ui::widget_divider();
        case sad::ui::WidgetType::Spacer:   return sad::ui::widget_spacer(0);
        case sad::ui::WidgetType::TextField:return sad::ui::widget_textfield("");
        default:
            // عنصر عام — يُنشأ كصندوق Container
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
