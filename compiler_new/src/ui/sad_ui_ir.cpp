/**
 * ==========================================================================
 * ملف: sad_ui_ir.cpp
 * الوصف: تنفيذ الأنواع الأساسية لـ SadUI IR
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 */

#include "ui/sad_ui_ir.hpp"
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace sad {
namespace ui {
namespace ir {

// ═══════════════════════════════════════════════════════════════════════════
// Color
// ═══════════════════════════════════════════════════════════════════════════

Color Color::fromHex(const std::string& hex) {
    std::string h = hex;
    if (!h.empty() && h[0] == '#') h = h.substr(1);

    Color c;
    if (h.size() == 6) {
        c.r = static_cast<uint8_t>(std::stoi(h.substr(0, 2), nullptr, 16));
        c.g = static_cast<uint8_t>(std::stoi(h.substr(2, 2), nullptr, 16));
        c.b = static_cast<uint8_t>(std::stoi(h.substr(4, 2), nullptr, 16));
        c.a = 1.0f;
    } else if (h.size() == 8) {
        c.r = static_cast<uint8_t>(std::stoi(h.substr(0, 2), nullptr, 16));
        c.g = static_cast<uint8_t>(std::stoi(h.substr(2, 2), nullptr, 16));
        c.b = static_cast<uint8_t>(std::stoi(h.substr(4, 2), nullptr, 16));
        c.a = std::stoi(h.substr(6, 2), nullptr, 16) / 255.0f;
    } else if (h.size() == 3) {
        // #RGB → #RRGGBB
        c.r = static_cast<uint8_t>(std::stoi(std::string(2, h[0]), nullptr, 16));
        c.g = static_cast<uint8_t>(std::stoi(std::string(2, h[1]), nullptr, 16));
        c.b = static_cast<uint8_t>(std::stoi(std::string(2, h[2]), nullptr, 16));
        c.a = 1.0f;
    }
    return c;
}

std::string Color::toHex() const {
    std::ostringstream ss;
    ss << "#" << std::hex << std::setfill('0')
       << std::setw(2) << (int)r
       << std::setw(2) << (int)g
       << std::setw(2) << (int)b;
    if (a < 0.999f) {
        ss << std::setw(2) << (int)(a * 255);
    }
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// StyleIR
// ═══════════════════════════════════════════════════════════════════════════

bool StyleIR::hasAnyStyle() const {
    return foregroundColor.has_value()
        || backgroundColor.has_value()
        || tintColor.has_value()
        || borderColor.has_value()
        || fontSize.has_value()
        || fontWeight.has_value()
        || fontFamily.has_value()
        || lineSpacing.has_value()
        || !width.isAuto()
        || !height.isAuto()
        || minWidth.has_value()
        || minHeight.has_value()
        || maxWidth.has_value()
        || maxHeight.has_value()
        || !padding.isZero()
        || !margin.isZero()
        || spacing != 0
        || borderRadius != 0
        || borderWidth != 0
        || horizontalAlignment.has_value()
        || verticalAlignment.has_value()
        || contentAlignment.has_value()
        || shadow.has_value()
        || opacity != 1.0f
        || rotation != 0.0f
        || scale != 1.0f
        || hidden
        || disabled;
}

// ═══════════════════════════════════════════════════════════════════════════
// UINodeIR
// ═══════════════════════════════════════════════════════════════════════════

int UINodeIR::subtreeSize() const {
    int count = 1;
    for (const auto& child : children) {
        if (child) count += child->subtreeSize();
    }
    return count;
}

UINodeIRPtr UINodeIR::findByTag(const std::string& searchTag) const {
    if (tag.has_value() && *tag == searchTag) {
        // نُرجع نسخة shared_ptr — لكن هذا يتطلب enable_shared_from_this
        // بدلاً من ذلك، نبحث في الأبناء
    }
    for (const auto& child : children) {
        if (!child) continue;
        if (child->tag.has_value() && *(child->tag) == searchTag) {
            return child;
        }
        auto found = child->findByTag(searchTag);
        if (found) return found;
    }
    return nullptr;
}

std::string UINodeIR::dump(int indent) const {
    std::ostringstream ss;
    std::string pad(indent * 2, ' ');

    ss << pad << widgetTypeToArabic(type);
    if (tag.has_value()) ss << " [" << *tag << "]";
    ss << " (id=" << id << ")";

    // الخصائص
    if (!constructorArgs.empty()) {
        ss << " args=" << constructorArgs.size();
    }
    if (!eventHandlers.empty()) {
        ss << " events=" << eventHandlers.size();
    }
    if (stateBinding.has_value()) {
        ss << " state=" << stateBinding->name;
    }

    // الأنماط المعيّنة
    if (style.fontSize.has_value()) {
        ss << " fontSize=" << *style.fontSize;
    }
    if (style.foregroundColor.has_value()) {
        ss << " color=" << style.foregroundColor->toHex();
    }
    if (style.backgroundColor.has_value()) {
        ss << " bg=" << style.backgroundColor->toHex();
    }

    ss << "\n";

    // الأبناء
    for (const auto& child : children) {
        if (child) ss << child->dump(indent + 1);
    }

    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// WidgetType ↔ Arabic mapping
// ═══════════════════════════════════════════════════════════════════════════

namespace {

struct WidgetMapping {
    WidgetType type;
    const char* arabic;
    const char* english;
};

// جدول ربط شامل: نوع المكون — الاسم العربي — الاسم الإنجليزي
static const WidgetMapping widgetMappings[] = {
    // العرض
    {WidgetType::Text,           "\xd9\x86\xd8\xb5",               "text"},       // نص
    {WidgetType::Image,          "\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9",           "image"},     // صورة
    {WidgetType::Icon,           "\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9",         "icon"},      // أيقونة

    // الإدخال
    {WidgetType::Button,         "\xd8\xb2\xd8\xb1",               "button"},     // زر
    {WidgetType::IconButton,     "\xd8\xb2\xd8\xb1_\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9",   "icon_button"}, // زر_أيقونة
    {WidgetType::FloatingButton, "\xd8\xb2\xd8\xb1_\xd8\xb9\xd8\xa7\xd8\xa6\xd9\x85",       "fab"},        // زر_عائم
    {WidgetType::OutlineButton,  "\xd8\xb2\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf",       "outline_button"}, // زر_محدد
    {WidgetType::TextButton,     "\xd8\xb2\xd8\xb1_\xd9\x86\xd8\xb5\xd9\x8a",         "text_button"}, // زر_نصي
    {WidgetType::TextField,      "\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5",         "text_field"}, // حقل_نص
    {WidgetType::TextArea,       "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd9\x86\xd8\xb5",       "text_area"},  // منطقة_نص
    {WidgetType::PasswordField,  "\xd8\xad\xd9\x82\xd9\x84_\xd9\x83\xd9\x84\xd9\x85\xd8\xa9_\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1", "password_field"}, // حقل_كلمة_مرور
    {WidgetType::Checkbox,       "\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1",   "checkbox"},    // مربع_اختيار
    {WidgetType::Radio,          "\xd8\xb2\xd8\xb1_\xd8\xb1\xd8\xa7\xd8\xaf\xd9\x8a\xd9\x88",       "radio"},       // زر_راديو
    {WidgetType::Toggle,         "\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad",           "toggle"},      // مفتاح
    {WidgetType::Slider,         "\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82",           "slider"},      // منزلق
    {WidgetType::RangeSlider,    "\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82_\xd9\x85\xd8\xaf\xd9\x89",       "range_slider"}, // منزلق_مدى
    {WidgetType::Dropdown,       "\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb3\xd8\xaf\xd9\x84\xd8\xa9",   "dropdown"},    // قائمة_منسدلة
    {WidgetType::DatePicker,     "\xd9\x85\xd9\x86\xd8\xaa\xd9\x82\xd9\x8a_\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae",   "date_picker"}, // منتقي_تاريخ
    {WidgetType::ColorPicker,    "\xd9\x85\xd9\x86\xd8\xaa\xd9\x82\xd9\x8a_\xd9\x84\xd9\x88\xd9\x86",       "color_picker"}, // منتقي_لون
    {WidgetType::SearchField,    "\xd8\xad\xd9\x82\xd9\x84_\xd8\xa8\xd8\xad\xd8\xab",         "search_field"}, // حقل_بحث
    {WidgetType::NumberInput,    "\xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84_\xd8\xb1\xd9\x82\xd9\x85",       "number_input"}, // إدخال_رقم
    {WidgetType::RatingStars,    "\xd9\x86\xd8\xac\xd9\x88\xd9\x85_\xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd9\x85",     "rating_stars"}, // نجوم_تقييم

    // التخطيط
    {WidgetType::Column,         "\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf",             "column"},      // عمود
    {WidgetType::Row,            "\xd8\xb5\xd9\x81",               "row"},         // صف
    {WidgetType::Stack,          "\xd8\xb1\xd8\xb5\xd8\xa9",             "stack"},       // رصة
    {WidgetType::Grid,           "\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9",             "grid"},        // شبكة
    {WidgetType::LazyColumn,     "\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf_\xd9\x83\xd8\xb3\xd9\x88\xd9\x84",       "lazy_column"}, // عمود_كسول
    {WidgetType::LazyRow,        "\xd8\xb5\xd9\x81_\xd9\x83\xd8\xb3\xd9\x88\xd9\x84",         "lazy_row"},    // صف_كسول
    {WidgetType::LazyGrid,       "\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x83\xd8\xb3\xd9\x88\xd9\x84\xd8\xa9",     "lazy_grid"},   // شبكة_كسولة
    {WidgetType::Spacer,         "\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84",             "spacer"},      // فاصل
    {WidgetType::Divider,        "\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84_\xd8\xae\xd8\xb7",         "divider"},     // فاصل_خط
    {WidgetType::Expanded,       "\xd9\x85\xd9\x88\xd8\xb3\xd8\xb9",             "expanded"},    // موسع
    {WidgetType::SizedBox,       "\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf",     "sized_box"},   // صندوق_محدد
    {WidgetType::Center,         "\xd9\x88\xd8\xb3\xd8\xb7",             "center"},      // وسط
    {WidgetType::Wrap,           "\xd9\x84\xd9\x81",               "wrap"},        // لف

    // الحاويات
    {WidgetType::Container,      "\xd8\xad\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9",           "container"},   // حاوية
    {WidgetType::Card,           "\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9",           "card"},        // بطاقة
    {WidgetType::Surface,        "\xd8\xb3\xd8\xb7\xd8\xad",             "surface"},     // سطح
    {WidgetType::Box,            "\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82",           "box"},         // صندوق
    {WidgetType::ScrollView,     "\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1",       "scroll_view"}, // عرض_تمرير
    {WidgetType::PageView,       "\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xb5\xd9\x81\xd8\xad\xd8\xa7\xd8\xaa",       "page_view"},   // عرض_صفحات

    // الهيكل
    {WidgetType::Scaffold,       "\xd8\xb3\xd9\x82\xd8\xa7\xd9\x84\xd8\xa9",           "scaffold"},    // سقالة
    {WidgetType::AppBar,         "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82",     "app_bar"},     // شريط_تطبيق
    {WidgetType::BottomBar,      "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a",       "bottom_bar"},  // شريط_سفلي
    {WidgetType::TabBar,         "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa3\xd9\x82\xd8\xb3\xd8\xa7\xd9\x85",     "tab_bar"},     // شريط_أقسام
    {WidgetType::Drawer,         "\xd8\xaf\xd8\xb1\xd8\xac",             "drawer"},      // درج
    {WidgetType::BottomSheet,    "\xd9\x88\xd8\xb1\xd9\x82\xd8\xa9_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a\xd8\xa9",     "bottom_sheet"}, // ورقة_سفلية

    // التنقل
    {WidgetType::NavigationView, "\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84",         "nav_view"},    // عرض_تنقل
    {WidgetType::NavigationLink, "\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84",       "nav_link"},    // رابط_تنقل
    {WidgetType::TabView,        "\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xa3\xd9\x82\xd8\xb3\xd8\xa7\xd9\x85",       "tab_view"},    // عرض_أقسام

    // القوائم
    {WidgetType::List,           "\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9",           "list"},        // قائمة
    {WidgetType::ListTile,       "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9",     "list_tile"},   // عنصر_قائمة
    {WidgetType::Section,        "\xd9\x82\xd8\xb3\xd9\x85",             "section"},     // قسم
    {WidgetType::ForEach,        "\xd9\x84\xd9\x83\xd9\x84",             "for_each"},    // لكل

    // الحوارات
    {WidgetType::Dialog,         "\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1",             "dialog"},      // حوار
    {WidgetType::Alert,          "\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87",           "alert"},       // تنبيه
    {WidgetType::Toast,          "\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9",           "toast"},       // رسالة
    {WidgetType::Snackbar,       "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9",     "snackbar"},    // شريط_رسالة
    {WidgetType::Modal,          "\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd9\x85\xd8\xb4\xd8\xb1\xd9\x88\xd8\xb7\xd8\xa9",   "modal"},       // نافذة_مشروطة
    {WidgetType::Tooltip,        "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad",           "tooltip"},     // تلميح

    // البيانات
    {WidgetType::ProgressBar,    "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85",       "progress_bar"}, // شريط_تقدم
    {WidgetType::CircleProgress, "\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a",     "circle_progress"}, // تقدم_دائري
    {WidgetType::LoadingSpinner, "\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84",     "loading_spinner"}, // مؤشر_تحميل
    {WidgetType::Badge,          "\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9",             "badge"},       // شارة
    {WidgetType::Chip,           "\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9",           "chip"},        // رقاقة
    {WidgetType::Avatar,         "\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9_\xd8\xb4\xd8\xae\xd8\xb5\xd9\x8a\xd8\xa9",     "avatar"},      // صورة_شخصية
    {WidgetType::Stepper,        "\xd8\xae\xd8\xb7\xd9\x88\xd8\xa7\xd8\xaa",           "stepper"},     // خطوات

    // الرسوم البيانية
    {WidgetType::BarChart,       "\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xa3\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9",       "bar_chart"},   // رسم_أعمدة
    {WidgetType::LineChart,      "\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xae\xd8\xb7\xd9\x8a",         "line_chart"},  // رسم_خطي
    {WidgetType::PieChart,       "\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a",       "pie_chart"},   // رسم_دائري

    // متقدم
    {WidgetType::Canvas,         "\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xb1\xd8\xb3\xd9\x85",         "canvas"},      // لوحة_رسم
    {WidgetType::WebView,        "\xd8\xb9\xd8\xb1\xd8\xb6_\xd9\x88\xd9\x8a\xd8\xa8",         "web_view"},    // عرض_ويب
    {WidgetType::MapView,        "\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9",       "map_view"},    // عرض_خريطة
    {WidgetType::VideoPlayer,    "\xd9\x85\xd8\xb4\xd8\xba\xd9\x84_\xd9\x81\xd9\x8a\xd8\xaf\xd9\x8a\xd9\x88",     "video_player"}, // مشغل_فيديو

    // خاص
    {WidgetType::CustomWidget,   "\xd9\x85\xd9\x83\xd9\x88\xd9\x86_\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5",       "custom"},      // مكون_مخصص
    {WidgetType::Conditional,    "\xd8\xb4\xd8\xb1\xd8\xb7\xd9\x8a",           "conditional"}, // شرطي
    {WidgetType::Group,          "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9",         "group"},       // مجموعة
};

static const int WIDGET_MAPPING_COUNT = sizeof(widgetMappings) / sizeof(widgetMappings[0]);

} // anonymous namespace

std::string widgetTypeToArabic(WidgetType type) {
    for (int i = 0; i < WIDGET_MAPPING_COUNT; i++) {
        if (widgetMappings[i].type == type) {
            return widgetMappings[i].arabic;
        }
    }
    return "مجهول";
}

std::optional<WidgetType> arabicToWidgetType(const std::string& name) {
    for (int i = 0; i < WIDGET_MAPPING_COUNT; i++) {
        if (widgetMappings[i].arabic == name || widgetMappings[i].english == name) {
            return widgetMappings[i].type;
        }
    }
    return std::nullopt;
}

bool isContainerWidget(WidgetType type) {
    switch (type) {
        case WidgetType::Column:
        case WidgetType::Row:
        case WidgetType::Stack:
        case WidgetType::Grid:
        case WidgetType::LazyColumn:
        case WidgetType::LazyRow:
        case WidgetType::LazyGrid:
        case WidgetType::Container:
        case WidgetType::Card:
        case WidgetType::Surface:
        case WidgetType::Box:
        case WidgetType::ScrollView:
        case WidgetType::PageView:
        case WidgetType::Scaffold:
        case WidgetType::NavigationView:
        case WidgetType::TabView:
        case WidgetType::List:
        case WidgetType::Section:
        case WidgetType::ForEach:
        case WidgetType::Drawer:
        case WidgetType::BottomSheet:
        case WidgetType::Dialog:
        case WidgetType::Modal:
        case WidgetType::Center:
        case WidgetType::Wrap:
        case WidgetType::Expanded:
        case WidgetType::Group:
            return true;
        default:
            return false;
    }
}


} // namespace ir
} // namespace ui
} // namespace sad
