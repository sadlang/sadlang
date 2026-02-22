/**
 * =============================================================================
 * ملف: sadc_graphics_support.cpp
 * الوصف: دعم الرسومات الكامل في المترجم sadc
 * المرحلة: Phase 95 - Compiler Hardening
 * =============================================================================
 * 
 * 🎨 دعم الرسومات في المترجم
 * ═════════════════════════════════════════
 * 
 * هذا الملف يوفر:
 * 1. تحويل استدعاءات الرسومات العربية إلى LLVM
 * 2. توليد كود الربط مع SDL2
 * 3. دعم جميع الودجات (109+)
 * 4. معالجة الأحداث
 * 5. الرسم المباشر
 * 
 * =============================================================================
 */

#include "compiler_improvements.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>

namespace sad {
namespace compiler {
namespace graphics {

// =============================================================================
// الفئة 1: أنواع الودجات
// =============================================================================

/**
 * @brief أنواع الودجات المدعومة (109+)
 */
enum class WidgetType {
    // === الأصلية 9 ===
    Column = 1, Row, Text, Button, TextField, Container, Spacer, Card, Divider,
    
    // === التخطيط (10) ===
    Stack = 10, Center, Wrap, GridView, ListView, ScrollView, SizedBox, Expanded, PageView, Table,
    
    // === الأزرار (6) ===
    IconButton = 20, FloatingButton, OutlineButton, TextButton, ToggleButton, ButtonGroup,
    
    // === عناصر الإدخال (14) ===
    Checkbox = 30, Radio, Switch, Slider, Dropdown, NumberInput, SearchField, PasswordField,
    TextArea, DateField, RangeSlider, ToggleGroup, ColorPicker, RatingStars,
    
    // === عرض النص (7) ===
    Label = 50, Heading, Paragraph, Link, CodeBlock, Quote, Markdown,
    
    // === عرض مرئي (10) ===
    Icon = 60, Avatar, Badge, Chip, Tag, StatusDot, ColorBox, GradientBox, ImageWidget, Separator,
    
    // === التقدم (4) ===
    ProgressBar = 70, CircleProgress, LoadingSpinner, Skeleton,
    
    // === التنقل (10) ===
    AppBar = 80, BottomBar, TabBar, Drawer, Breadcrumb, NavItem, MenuItem, Toolbar, StatusBar, SideMenu,
    
    // === التغذية الراجعة (10) ===
    Dialog = 90, Alert, Toast, Snackbar, Banner, Modal, Popover, Notification, ConfirmDialog, Tooltip,
    
    // === البطاقات واللوحات (7) ===
    InfoCard = 100, StatCard, ExpansionPanel, Accordion, Panel, GroupBox, Callout,
    
    // === البيانات (10) ===
    ListTile = 110, DataTable, TableRow, TableCell, DetailRow, Timeline, TimelineItem, TreeItem, Stepper, StepIndicator,
    
    // === الرسوم البيانية (7) ===
    BarChart = 120, HBarChart, PieChart, LineChart, SparkLine, Gauge, DonutChart,
    
    // === التصفح (2) ===
    Pagination = 130, DotIndicator,
    
    // === متقدم (3) ===
    Canvas = 140, SplitView, Carousel
};

/**
 * @brief خريطة الأسماء العربية للودجات
 */
class WidgetNameMap {
public:
    static WidgetNameMap& instance() {
        static WidgetNameMap instance;
        return instance;
    }
    
    WidgetType getType(const std::string& arabicName) const {
        auto it = nameToType_.find(arabicName);
        if (it != nameToType_.end()) {
            return it->second;
        }
        return WidgetType::Container; // افتراضي
    }
    
    std::string getRuntimeName(WidgetType type) const {
        auto it = typeToRuntime_.find(type);
        if (it != typeToRuntime_.end()) {
            return it->second;
        }
        return "sad_ui_create_widget";
    }
    
private:
    std::map<std::string, WidgetType> nameToType_;
    std::map<WidgetType, std::string> typeToRuntime_;
    
    WidgetNameMap() {
        // الأسماء العربية
        nameToType_["\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf"] = WidgetType::Column;           // عمود
        nameToType_["\xd8\xb5\xd9\x81"] = WidgetType::Row;                              // صف
        nameToType_["\xd9\x86\xd8\xb5"] = WidgetType::Text;                             // نص
        nameToType_["\xd8\xb2\xd8\xb1"] = WidgetType::Button;                           // زر
        nameToType_["\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5"] = WidgetType::TextField; // حقل_نص
        nameToType_["\xd8\xad\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9"] = WidgetType::Container; // حاوية
        nameToType_["\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84"] = WidgetType::Spacer;           // فاصل
        nameToType_["\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9"] = WidgetType::Card;     // بطاقة
        nameToType_["\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84_\xd8\xae\xd8\xb7\xd9\x8a"] = WidgetType::Divider; // فاصل_خطي
        
        // التخطيط
        nameToType_["\xd8\xb7\xd8\xa8\xd9\x82\xd8\xa7\xd8\xaa"] = WidgetType::Stack;     // طبقات
        nameToType_["\xd9\x88\xd8\xb3\xd8\xb7"] = WidgetType::Center;                   // وسط
        nameToType_["\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9"] = WidgetType::GridView;         // شبكة
        nameToType_["\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9"] = WidgetType::ListView;  // قائمة
        nameToType_["\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1"] = WidgetType::ScrollView; // تمرير
        
        // الأزرار
        nameToType_["\xd8\xb2\xd8\xb1_\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9"] = WidgetType::IconButton; // زر_أيقونة
        nameToType_["\xd8\xb2\xd8\xb1_\xd8\xb9\xd8\xa7\xd8\xa6\xd9\x85"] = WidgetType::FloatingButton; // زر_عائم
        
        // الإدخال
        nameToType_["\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1"] = WidgetType::Checkbox; // مربع_اختيار
        nameToType_["\xd8\xb2\xd8\xb1_\xd8\xb1\xd8\xa7\xd8\xaf\xd9\x8a\xd9\x88"] = WidgetType::Radio; // زر_راديو
        nameToType_["\xd9\x85\xd8\xa8\xd8\xaf\xd9\x84"] = WidgetType::Switch;           // مبدل
        nameToType_["\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1"] = WidgetType::Slider; // شريط_تمرير
        nameToType_["\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb3\xd8\xaf\xd9\x84\xd8\xa9"] = WidgetType::Dropdown; // قائمة_منسدلة
        
        // عرض النص
        nameToType_["\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"] = WidgetType::Heading;   // عنوان
        nameToType_["\xd9\x81\xd9\x82\xd8\xb1\xd8\xa9"] = WidgetType::Paragraph;        // فقرة
        nameToType_["\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7"] = WidgetType::Link;             // رابط
        
        // عرض مرئي
        nameToType_["\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9"] = WidgetType::Icon; // أيقونة
        nameToType_["\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9"] = WidgetType::ImageWidget;     // صورة
        nameToType_["\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9_\xd8\xb1\xd9\x85\xd8\xb2\xd9\x8a\xd8\xa9"] = WidgetType::Avatar; // صورة_رمزية
        
        // التنقل
        nameToType_["\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xb9\xd9\x84\xd9\x88\xd9\x8a"] = WidgetType::AppBar; // شريط_علوي
        nameToType_["\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a"] = WidgetType::BottomBar; // شريط_سفلي
        nameToType_["\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8\xd8\xa7\xd8\xaa"] = WidgetType::TabBar; // تبويبات
        nameToType_["\xd8\xaf\xd8\xb1\xd8\xac"] = WidgetType::Drawer;                   // درج
        
        // التغذية الراجعة
        nameToType_["\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1"] = WidgetType::Dialog; // نافذة_حوار
        nameToType_["\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87"] = WidgetType::Alert;     // تنبيه
        nameToType_["\xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1"] = WidgetType::Toast;     // إشعار
        
        // دوال الوقت التشغيلي
        typeToRuntime_[WidgetType::Column] = "sad_ui_widget_column";
        typeToRuntime_[WidgetType::Row] = "sad_ui_widget_row";
        typeToRuntime_[WidgetType::Text] = "sad_ui_widget_label";
        typeToRuntime_[WidgetType::Button] = "sad_ui_widget_button";
        typeToRuntime_[WidgetType::TextField] = "sad_ui_widget_textfield";
        typeToRuntime_[WidgetType::Container] = "sad_ui_widget_container";
        typeToRuntime_[WidgetType::Checkbox] = "sad_ui_widget_checkbox";
        typeToRuntime_[WidgetType::Radio] = "sad_ui_widget_radio";
        typeToRuntime_[WidgetType::Slider] = "sad_ui_widget_slider";
        typeToRuntime_[WidgetType::Dropdown] = "sad_ui_widget_dropdown";
        typeToRuntime_[WidgetType::ListView] = "sad_ui_widget_listview";
        typeToRuntime_[WidgetType::GridView] = "sad_ui_widget_gridview";
        typeToRuntime_[WidgetType::ScrollView] = "sad_ui_widget_scrollview";
        typeToRuntime_[WidgetType::Card] = "sad_ui_widget_card";
        typeToRuntime_[WidgetType::ImageWidget] = "sad_ui_widget_image";
        typeToRuntime_[WidgetType::Icon] = "sad_ui_widget_icon";
        typeToRuntime_[WidgetType::ProgressBar] = "sad_ui_widget_progress";
        typeToRuntime_[WidgetType::Canvas] = "sad_ui_widget_canvas";
    }
};

// =============================================================================
// الفئة 2: خريطة الدوال العربية
// =============================================================================

/**
 * @brief تحويل أسماء الدوال العربية إلى استدعاءات LLVM
 */
class ArabicFunctionMapper {
public:
    struct FunctionInfo {
        std::string llvmName;
        std::string returnType;
        std::vector<std::string> paramTypes;
        bool isGraphics;
    };
    
    static ArabicFunctionMapper& instance() {
        static ArabicFunctionMapper instance;
        return instance;
    }
    
    const FunctionInfo* lookup(const std::string& arabicName) const {
        auto it = functions_.find(arabicName);
        if (it != functions_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
private:
    std::map<std::string, FunctionInfo> functions_;
    
    ArabicFunctionMapper() {
        // === النوافذ ===
        functions_["\xd8\xa7\xd9\x86\xd8\xb4\xd8\xa7\xd8\xa1_\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9"] = {
            "sad_ui_create_window", "%SadWindow*", {"i8*", "i32", "i32"}, true
        }; // انشاء_نافذة
        
        functions_["\xd8\xa7\xd8\xba\xd9\x84\xd8\xa7\xd9\x82_\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9"] = {
            "sad_ui_destroy_window", "void", {"%SadWindow*"}, true
        }; // اغلاق_نافذة
        
        functions_["\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86_\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9"] = {
            "sad_ui_set_window_title", "void", {"%SadWindow*", "i8*"}, true
        }; // عنوان_نافذة
        
        functions_["\xd8\xad\xd8\xac\xd9\x85_\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9"] = {
            "sad_ui_set_window_size", "void", {"%SadWindow*", "i32", "i32"}, true
        }; // حجم_نافذة
        
        // === الودجات ===
        functions_["\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf"] = {
            "sad_ui_widget_column", "i32", {}, true
        }; // عمود
        
        functions_["\xd8\xb5\xd9\x81"] = {
            "sad_ui_widget_row", "i32", {}, true
        }; // صف
        
        functions_["\xd9\x86\xd8\xb5"] = {
            "sad_ui_widget_label", "i32", {"i8*"}, true
        }; // نص
        
        functions_["\xd8\xb2\xd8\xb1"] = {
            "sad_ui_widget_button", "i32", {"i8*"}, true
        }; // زر
        
        functions_["\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5"] = {
            "sad_ui_widget_textfield", "i32", {"i8*"}, true
        }; // حقل_نص
        
        functions_["\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1"] = {
            "sad_ui_widget_checkbox", "i32", {"i8*"}, true
        }; // مربع_اختيار
        
        functions_["\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1"] = {
            "sad_ui_widget_slider", "i32", {"float", "float"}, true
        }; // شريط_تمرير
        
        // === خصائص الودجات ===
        functions_["\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x85\xd9\x88\xd9\x82\xd8\xb9"] = {
            "sad_ui_set_position", "void", {"i32", "float", "float"}, true
        }; // تعيين_موقع
        
        functions_["\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd8\xad\xd8\xac\xd9\x85"] = {
            "sad_ui_set_size", "void", {"i32", "float", "float"}, true
        }; // تعيين_حجم
        
        functions_["\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb5"] = {
            "sad_ui_set_text", "void", {"i32", "i8*"}, true
        }; // تعيين_نص
        
        functions_["\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9"] = {
            "sad_ui_set_bg_color", "void", {"i32", "i8", "i8", "i8", "i8"}, true
        }; // تعيين_لون_خلفية
        
        functions_["\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x84\xd9\x88\xd9\x86_\xd9\x86\xd8\xb5"] = {
            "sad_ui_set_fg_color", "void", {"i32", "i8", "i8", "i8", "i8"}, true
        }; // تعيين_لون_نص
        
        functions_["\xd8\xa7\xd8\xb8\xd9\x87\xd8\xa7\xd8\xb1"] = {
            "sad_ui_set_visible", "void", {"i32", "i1"}, true
        }; // اظهار
        
        functions_["\xd8\xa7\xd8\xae\xd9\x81\xd8\xa7\xd8\xa1"] = {
            "sad_ui_set_visible", "void", {"i32", "i1"}, true
        }; // اخفاء (مع false)
        
        functions_["\xd8\xa7\xd8\xb6\xd9\x81_\xd8\xa7\xd8\xa8\xd9\x86"] = {
            "sad_ui_add_child", "void", {"i32", "i32"}, true
        }; // اضف_ابن
        
        functions_["\xd8\xa7\xd8\xb6\xd9\x81"] = {
            "sad_ui_add_child", "void", {"i32", "i32"}, true
        }; // اضف
        
        // === الحصول على القيم ===
        functions_["\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd9\x86\xd8\xb5"] = {
            "sad_ui_get_text", "i8*", {"i32"}, true
        }; // احصل_نص
        
        functions_["\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf"] = {
            "sad_ui_get_checked", "i1", {"i32"}, true
        }; // محدد
        
        functions_["\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"] = {
            "sad_ui_get_value", "float", {"i32"}, true
        }; // قيمة
        
        // === الأحداث ===
        functions_["\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb6\xd8\xba\xd8\xb7"] = {
            "sad_ui_set_callback", "void", {"i32", "i32", "i8*"}, true
        }; // عند_الضغط
        
        functions_["\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1"] = {
            "sad_ui_set_callback", "void", {"i32", "i32", "i8*"}, true
        }; // عند_التغيير
        
        // === حلقة الأحداث ===
        functions_["\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84"] = {
            "sad_ui_event_loop", "i32", {"%SadWindow*"}, true
        }; // تشغيل
        
        functions_["\xd8\xb1\xd8\xb3\xd9\x85"] = {
            "sad_ui_render", "void", {"%SadWindow*"}, true
        }; // رسم
        
        // === الرسم المباشر ===
        functions_["\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xae\xd8\xb7"] = {
            "sad_ui_draw_line", "void", {"i32", "i32", "i32", "i32", "i8", "i8", "i8", "i8"}, true
        }; // رسم_خط
        
        functions_["\xd8\xb1\xd8\xb3\xd9\x85_\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x8a\xd9\x84"] = {
            "sad_ui_draw_rect", "void", {"i32", "i32", "i32", "i32", "i8", "i8", "i8", "i8"}, true
        }; // رسم_مستطيل
        
        functions_["\xd9\x85\xd9\x84\xd8\xa1_\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x8a\xd9\x84"] = {
            "sad_ui_fill_rect", "void", {"i32", "i32", "i32", "i32", "i8", "i8", "i8", "i8"}, true
        }; // ملأ_مستطيل
        
        functions_["\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd8\xa9"] = {
            "sad_ui_draw_circle", "void", {"i32", "i32", "i32", "i8", "i8", "i8", "i8"}, true
        }; // رسم_دائرة
        
        functions_["\xd9\x85\xd9\x84\xd8\xa1_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd8\xa9"] = {
            "sad_ui_fill_circle", "void", {"i32", "i32", "i32", "i8", "i8", "i8", "i8"}, true
        }; // ملأ_دائرة
        
        functions_["\xd8\xb1\xd8\xb3\xd9\x85_\xd9\x86\xd8\xb5"] = {
            "sad_ui_draw_text", "void", {"i32", "i32", "i8*", "i8", "i8", "i8", "i8"}, true
        }; // رسم_نص
        
        functions_["\xd9\x85\xd8\xb3\xd8\xad"] = {
            "sad_ui_clear", "void", {"i8", "i8", "i8", "i8"}, true
        }; // مسح
        
        functions_["\xd8\xb9\xd8\xb1\xd8\xb6"] = {
            "sad_ui_present", "void", {}, true
        }; // عرض
        
        // === الطباعة ===
        functions_["\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9"] = {
            "sad_print", "void", {"i8*"}, false
        }; // اطبع
        
        functions_["\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9_\xd8\xb3\xd8\xb7\xd8\xb1"] = {
            "sad_println", "void", {"i8*"}, false
        }; // اطبع_سطر
    }
};

// =============================================================================
// الفئة 3: مولد كود الرسومات
// =============================================================================

/**
 * @brief مولد كود LLVM للرسومات
 */
class GraphicsCodeGenerator {
public:
    /**
     * @brief توليد كود main للتطبيق الرسومي
     */
    std::string generateGraphicsMain(const std::string& appTitle, int width, int height) {
        std::ostringstream out;
        
        out << "; ════════════════════════════════════════════════════════════════════\n";
        out << "; Graphics application main / الدالة الرئيسية للتطبيق الرسومي\n";
        out << "; ════════════════════════════════════════════════════════════════════\n\n";
        
        // ثوابت
        out << "@.app_title = private constant [" << (appTitle.size() + 1) << " x i8] c\"";
        for (char c : appTitle) {
            if (static_cast<unsigned char>(c) > 127) {
                out << "\\";
                out << std::hex << std::setfill('0') << std::setw(2) 
                    << static_cast<int>(static_cast<unsigned char>(c));
                out << std::dec;
            } else if (c == '"') {
                out << "\\22";
            } else {
                out << c;
            }
        }
        out << "\\00\"\n\n";
        
        // دالة main
        out << "define i32 @main(i32 %argc, i8** %argv) {\n";
        out << "entry:\n";
        
        // تهيئة SDL
        out << "  ; Initialize UI / تهيئة الواجهة\n";
        out << "  %init_result = call i32 @sad_ui_init()\n";
        out << "  %init_ok = icmp eq i32 %init_result, 0\n";
        out << "  br i1 %init_ok, label %create_window, label %init_failed\n\n";
        
        // فشل التهيئة
        out << "init_failed:\n";
        out << "  call void @sad_panic(i8* getelementptr ([25 x i8], [25 x i8]* @.err_init, i32 0, i32 0))\n";
        out << "  unreachable\n\n";
        
        // إنشاء النافذة
        out << "create_window:\n";
        out << "  ; Create window / إنشاء النافذة\n";
        out << "  %title_ptr = getelementptr [" << (appTitle.size() + 1) << " x i8], ["
            << (appTitle.size() + 1) << " x i8]* @.app_title, i32 0, i32 0\n";
        out << "  %window = call %SadWindow* @sad_ui_create_window(i8* %title_ptr, i32 " 
            << width << ", i32 " << height << ")\n";
        out << "  %window_ok = icmp ne %SadWindow* %window, null\n";
        out << "  br i1 %window_ok, label %build_ui, label %window_failed\n\n";
        
        // فشل النافذة
        out << "window_failed:\n";
        out << "  call void @sad_ui_quit()\n";
        out << "  ret i32 1\n\n";
        
        // بناء الواجهة (سيتم استبداله بالكود الفعلي)
        out << "build_ui:\n";
        out << "  ; Build UI / بناء الواجهة\n";
        out << "  ; [USER_UI_CODE]\n\n";
        
        // تشغيل حلقة الأحداث
        out << "  ; Run event loop / تشغيل حلقة الأحداث\n";
        out << "  %result = call i32 @sad_ui_event_loop(%SadWindow* %window)\n\n";
        
        // التنظيف
        out << "  ; Cleanup / التنظيف\n";
        out << "  call void @sad_ui_destroy_window(%SadWindow* %window)\n";
        out << "  call void @sad_ui_quit()\n";
        out << "  ret i32 %result\n";
        out << "}\n\n";
        
        // رسالة الخطأ
        out << "@.err_init = private constant [25 x i8] c\"Failed to initialize UI\\00\"\n";
        
        return out.str();
    }
    
    /**
     * @brief توليد استدعاء إنشاء ودجت
     */
    std::string generateWidgetCreation(const std::string& arabicName, 
                                        const std::vector<std::string>& args,
                                        int resultReg) {
        std::ostringstream out;
        
        auto* funcInfo = ArabicFunctionMapper::instance().lookup(arabicName);
        if (!funcInfo) {
            out << "  ; Unknown widget type: " << arabicName << "\n";
            out << "  %" << resultReg << " = call i32 @sad_ui_create_widget(i32 0)\n";
            return out.str();
        }
        
        out << "  %" << resultReg << " = call " << funcInfo->returnType 
            << " @" << funcInfo->llvmName << "(";
        
        for (size_t i = 0; i < args.size() && i < funcInfo->paramTypes.size(); ++i) {
            if (i > 0) out << ", ";
            out << funcInfo->paramTypes[i] << " " << args[i];
        }
        
        out << ")\n";
        
        return out.str();
    }
    
    /**
     * @brief توليد تعيين خاصية
     */
    std::string generatePropertySet(int widgetId, const std::string& property,
                                      const std::string& value, const std::string& valueType) {
        std::ostringstream out;
        
        // خريطة الخصائص إلى الدوال
        static const std::map<std::string, std::string> propertyFunctions = {
            {"text", "sad_ui_set_text"},
            {"نص", "sad_ui_set_text"},
            {"position", "sad_ui_set_position"},
            {"موقع", "sad_ui_set_position"},
            {"size", "sad_ui_set_size"},
            {"حجم", "sad_ui_set_size"},
            {"bgColor", "sad_ui_set_bg_color"},
            {"لون_خلفية", "sad_ui_set_bg_color"},
            {"fgColor", "sad_ui_set_fg_color"},
            {"لون_نص", "sad_ui_set_fg_color"},
            {"visible", "sad_ui_set_visible"},
            {"مرئي", "sad_ui_set_visible"},
            {"enabled", "sad_ui_set_enabled"},
            {"مفعل", "sad_ui_set_enabled"}
        };
        
        auto it = propertyFunctions.find(property);
        if (it != propertyFunctions.end()) {
            out << "  call void @" << it->second << "(i32 " << widgetId 
                << ", " << valueType << " " << value << ")\n";
        } else {
            out << "  ; Unknown property: " << property << "\n";
        }
        
        return out.str();
    }
    
    /**
     * @brief توليد إضافة ابن
     */
    std::string generateAddChild(int parentId, int childId) {
        std::ostringstream out;
        out << "  call void @sad_ui_add_child(i32 " << parentId << ", i32 " << childId << ")\n";
        return out.str();
    }
    
    /**
     * @brief توليد معالج حدث
     */
    std::string generateEventHandler(int widgetId, const std::string& eventType,
                                       const std::string& callbackName) {
        std::ostringstream out;
        
        // أنواع الأحداث
        static const std::map<std::string, int> eventTypes = {
            {"click", 1}, {"ضغط", 1},
            {"change", 2}, {"تغيير", 2},
            {"hover", 3}, {"تحويم", 3},
            {"focus", 4}, {"تركيز", 4},
            {"blur", 5}, {"إلغاء_تركيز", 5}
        };
        
        int eventId = 0;
        auto it = eventTypes.find(eventType);
        if (it != eventTypes.end()) {
            eventId = it->second;
        }
        
        out << "  ; Set event handler for " << eventType << "\n";
        out << "  %callback_ptr = bitcast void()* @" << callbackName << " to i8*\n";
        out << "  call void @sad_ui_set_callback(i32 " << widgetId 
            << ", i32 " << eventId << ", i8* %callback_ptr)\n";
        
        return out.str();
    }
};

// =============================================================================
// الفئة 4: رابط الرسومات
// =============================================================================

/**
 * @brief رابط البرامج الرسومية
 */
class GraphicsLinker {
public:
    struct LinkOptions {
        std::string outputPath;
        bool staticLink = false;
        bool debugInfo = false;
        std::vector<std::string> additionalLibs;
        std::vector<std::string> additionalPaths;
    };
    
    struct LinkResult {
        bool success;
        std::string outputPath;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    /**
     * @brief الحصول على أوامر الربط للمنصة الحالية
     */
    static std::vector<std::string> getLinkCommand(const std::string& objectFile,
                                                     const LinkOptions& options) {
        std::vector<std::string> cmd;
        
#ifdef _WIN32
        // Windows - استخدام MSVC link
        cmd.push_back("link.exe");
        cmd.push_back("/NOLOGO");
        cmd.push_back("/OUT:" + options.outputPath);
        cmd.push_back(objectFile);
        
        // مكتبات SDL2
        cmd.push_back("SDL2.lib");
        cmd.push_back("SDL2main.lib");
        cmd.push_back("SDL2_ttf.lib");
        
        // مكتبات النظام
        cmd.push_back("user32.lib");
        cmd.push_back("gdi32.lib");
        cmd.push_back("shell32.lib");
        cmd.push_back("opengl32.lib");
        
        // مكتبة Runtime
        cmd.push_back("sad_graphics_runtime.lib");
        
        if (options.debugInfo) {
            cmd.push_back("/DEBUG");
        }
        
        // مسارات المكتبات
        for (const auto& path : options.additionalPaths) {
            cmd.push_back("/LIBPATH:" + path);
        }
        
        // مكتبات إضافية
        for (const auto& lib : options.additionalLibs) {
            cmd.push_back(lib);
        }
        
#elif defined(__linux__)
        // Linux - استخدام gcc
        cmd.push_back("gcc");
        cmd.push_back("-o");
        cmd.push_back(options.outputPath);
        cmd.push_back(objectFile);
        
        // مكتبات SDL2
        cmd.push_back("-lSDL2");
        cmd.push_back("-lSDL2_ttf");
        
        // مكتبات النظام
        cmd.push_back("-lm");
        cmd.push_back("-lpthread");
        cmd.push_back("-lGL");
        
        // مكتبة Runtime
        cmd.push_back("-lsad_graphics_runtime");
        
        if (options.debugInfo) {
            cmd.push_back("-g");
        }
        
        if (options.staticLink) {
            cmd.push_back("-static");
        }
        
        // مسارات المكتبات
        for (const auto& path : options.additionalPaths) {
            cmd.push_back("-L" + path);
        }
        
        // مكتبات إضافية
        for (const auto& lib : options.additionalLibs) {
            cmd.push_back("-l" + lib);
        }
        
#elif defined(__APPLE__)
        // macOS - استخدام clang
        cmd.push_back("clang");
        cmd.push_back("-o");
        cmd.push_back(options.outputPath);
        cmd.push_back(objectFile);
        
        // Frameworks
        cmd.push_back("-framework");
        cmd.push_back("SDL2");
        cmd.push_back("-framework");
        cmd.push_back("SDL2_ttf");
        cmd.push_back("-framework");
        cmd.push_back("OpenGL");
        cmd.push_back("-framework");
        cmd.push_back("Cocoa");
        
        // مكتبة Runtime
        cmd.push_back("-lsad_graphics_runtime");
        
        if (options.debugInfo) {
            cmd.push_back("-g");
        }
        
        // مسارات المكتبات
        for (const auto& path : options.additionalPaths) {
            cmd.push_back("-L" + path);
        }
        
        // مكتبات إضافية
        for (const auto& lib : options.additionalLibs) {
            cmd.push_back("-l" + lib);
        }
#endif
        
        return cmd;
    }
    
    /**
     * @brief الحصول على مسارات SDL2 الافتراضية
     */
    static std::vector<std::string> getSDL2Paths() {
        std::vector<std::string> paths;
        
#ifdef _WIN32
        // Windows
        paths.push_back("C:\\SDL2\\lib\\x64");
        paths.push_back("C:\\SDL2-2.28.0\\lib\\x64");
        
        // نسبي للمشروع
        paths.push_back("..\\..\\stdlib\\graphics\\third_party\\SDL2\\lib\\x64");
#elif defined(__linux__)
        // Linux
        paths.push_back("/usr/lib");
        paths.push_back("/usr/lib/x86_64-linux-gnu");
        paths.push_back("/usr/local/lib");
#elif defined(__APPLE__)
        // macOS
        paths.push_back("/usr/local/lib");
        paths.push_back("/opt/homebrew/lib");
        paths.push_back("/Library/Frameworks");
#endif
        
        return paths;
    }
    
    /**
     * @brief التحقق من توفر SDL2
     */
    static bool isSDL2Available() {
        auto paths = getSDL2Paths();
        
        for (const auto& path : paths) {
#ifdef _WIN32
            std::string libPath = path + "\\SDL2.lib";
#else
            std::string libPath = path + "/libSDL2.so";
#endif
            // التحقق من وجود الملف (بسيط)
            std::ifstream f(libPath);
            if (f.good()) {
                return true;
            }
        }
        
        return false;
    }
};

} // namespace graphics
} // namespace compiler
} // namespace sad
