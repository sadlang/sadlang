// بسم الله الرحمن الرحيم
// sad_ui_framework.h - إطار عمل واجهات المستخدم 109 مكون (Flutter-like)
// SAD UI Framework - 109 widgets Flutter-like component system
#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace sad { namespace ui {

// ===== 109 Widget Types =====
enum class WidgetType {
    // === Original 9 ===
    Column, Row, Text, Button, TextField, Container, Spacer, Card, Divider,
    // === Layout (10) ===
    Stack, Center, Wrap, GridView, ListView, ScrollView, SizedBox, Expanded, PageView, Table,
    // === Buttons (6) ===
    IconButton, FloatingButton, OutlineButton, TextButton, ToggleButton, ButtonGroup,
    // === Input Controls (14) ===
    Checkbox, Radio, Switch, Slider, Dropdown, NumberInput, SearchField, PasswordField,
    TextArea, DateField, RangeSlider, ToggleGroup, ColorPicker, RatingStars,
    // === Display Text (7) ===
    Label, Heading, Paragraph, Link, CodeBlock, Quote, Markdown,
    // === Display Visual (10) ===
    Icon, Avatar, Badge, Chip, Tag, StatusDot, ColorBox, GradientBox, ImageWidget, Separator,
    // === Progress (4) ===
    ProgressBar, CircleProgress, LoadingSpinner, Skeleton,
    // === Navigation (10) ===
    AppBar, BottomBar, TabBar, Drawer, Breadcrumb, NavItem, MenuItem, Toolbar, StatusBar, SideMenu,
    // === Feedback (10) ===
    Dialog, Alert, Toast, Snackbar, Banner, Modal, Popover, Notification, ConfirmDialog, Tooltip,
    // === Cards & Panels (7) ===
    InfoCard, StatCard, ExpansionPanel, Accordion, Panel, GroupBox, Callout,
    // === Data (10) ===
    ListTile, DataTable, TableRow, TableCell, DetailRow, Timeline, TimelineItem, TreeItem, Stepper, StepIndicator,
    // === Charts (7) ===
    BarChart, HBarChart, PieChart, LineChart, SparkLine, Gauge, DonutChart,
    // === Pagination (2) ===
    Pagination, DotIndicator,
    // === Advanced (3) ===
    Canvas, SplitView, Carousel
};

// ===== Color =====
struct RGBA { int r=255, g=255, b=255, a=255; };

// ===== Widget =====
struct Widget {
    int id = 0;
    WidgetType type = WidgetType::Container;
    std::string text;
    std::string subtitle;   // ListTile subtitle, secondary text, dialog message

    // Style
    RGBA bgColor     = {0,0,0,0};
    RGBA textColor   = {33,33,33,255};
    RGBA btnColor    = {66,133,244,255};
    RGBA btnHover    = {90,155,255,255};
    RGBA btnPress    = {40,100,200,255};
    RGBA borderColor = {180,180,180,255};
    RGBA accentColor = {255,159,10,255};  // for charts, progress, etc.
    float padding = 0, padTop=0, padRight=0, padBottom=0, padLeft=0;
    float fixedWidth = -1, fixedHeight = -1;
    float flex = 0, fontSize = 20, borderRadius = 0;
    float spacing = 4, borderWidth = 0;
    std::string textAlign = "center";
    std::string widgetName;           // المعرف - string name for lookup

    // Computed layout
    float cx=0, cy=0, cw=0, ch=0;

    // علامات التغيير (Dirty Flags) — لتجنب إعادة التخطيط والرسم بدون تغيير
    bool layoutDirty = true;            // يحتاج إعادة تخطيط
    bool renderDirty = true;            // يحتاج إعادة رسم

    // العرض الافتراضي (Virtualization) — للقوائم الطويلة
    int virtualFirstVisible = 0;        // أول عنصر مرئي
    int virtualLastVisible = -1;        // آخر عنصر مرئي
    float estimatedItemHeight = 50.0f;  // الارتفاع التقديري لكل عنصر

    // Interaction state
    bool hovered=false, pressed=false, wasClicked=false;
    std::string inputText;
    bool inputFocused=false, visible=true;

    // Extended state for new widgets
    bool checked = false;         // Checkbox, Radio, Switch, ToggleButton
    float value = 0;              // Slider, ProgressBar, Gauge, Rating, etc.
    float minVal = 0, maxVal = 100; // Slider/RangeSlider range
    float value2 = 0;            // RangeSlider second handle
    int selectedIndex = -1;       // Dropdown, TabBar, BottomBar, Pagination
    bool expanded = false;        // ExpansionPanel, Accordion, Drawer
    char iconChar = '*';          // Icon, IconButton
    int columns = 2;             // GridView, DataTable
    float scrollY = 0;           // ListView, ScrollView
    bool masked = false;         // PasswordField
    int level = 1;               // Heading (1-6), TreeItem depth
    bool active = false;         // NavItem, MenuItem, StepIndicator
    int currentPage = 0;         // PageView, Pagination
    int totalPages = 1;          // Pagination
    float animProgress = 0;      // LoadingSpinner angle
    bool dialogVisible = false;  // Dialog, Modal, Popover, Drawer

    std::vector<int> children;
    std::vector<std::string> items;   // Dropdown items, TabBar labels, etc.
    std::vector<float> dataValues;    // Chart data
    std::vector<RGBA> dataColors;     // Chart colors
};

// ===== App Management =====
int    app_create(const std::string& title, int w, int h);
bool   app_is_running(int appId);
void   app_update(int appId);
void   app_set_content(int appId, int widgetId);
void   app_set_bg_color(int appId, int r, int g, int b);
void   app_close(int appId);

// ===== Widget Access =====
Widget* getWidget(int id);
Widget* getWidgetByName(const std::string& name);

// ===== Original 9 Widget Creation =====
int widget_column();
int widget_row();
int widget_text(const std::string& content);
int widget_button(const std::string& text);
int widget_textfield(const std::string& hint);
int widget_container();
int widget_spacer(float size);
int widget_card();
int widget_divider();

// ===== Layout (10) =====
int widget_stack();
int widget_center();
int widget_wrap();
int widget_gridview(int cols);
int widget_listview();
int widget_scrollview();
int widget_sizedbox(float w, float h);
int widget_expanded();
int widget_pageview();
int widget_table(int cols);

// ===== Buttons (6) =====
int widget_icon_button(char icon);
int widget_floating_button(char icon);
int widget_outline_button(const std::string& text);
int widget_text_button(const std::string& text);
int widget_toggle_button(const std::string& text);
int widget_button_group();

// ===== Input Controls (14) =====
int widget_checkbox(const std::string& label);
int widget_radio(const std::string& label);
int widget_switch(const std::string& label);
int widget_slider(float min_val, float max_val);
int widget_dropdown(const std::string& hint);
int widget_number_input(float min_val, float max_val);
int widget_search_field(const std::string& hint);
int widget_password_field(const std::string& hint);
int widget_textarea(const std::string& hint);
int widget_date_field();
int widget_range_slider(float min_val, float max_val);
int widget_toggle_group();
int widget_color_picker();
int widget_rating_stars(int max_stars);

// ===== Display Text (7) =====
int widget_label(const std::string& text);
int widget_heading(const std::string& text, int level);
int widget_paragraph(const std::string& text);
int widget_link(const std::string& text);
int widget_code_block(const std::string& code);
int widget_quote(const std::string& text);
int widget_markdown(const std::string& text);

// ===== Display Visual (10) =====
int widget_icon(char icon);
int widget_avatar(const std::string& letter, int r, int g, int b);
int widget_badge(int count);
int widget_chip(const std::string& label);
int widget_tag(const std::string& label, int r, int g, int b);
int widget_status_dot(int r, int g, int b);
int widget_color_box(int r, int g, int b);
int widget_gradient_box(int r1, int g1, int b1, int r2, int g2, int b2);
int widget_image(const std::string& placeholder);
int widget_separator();

// ===== Progress (4) =====
int widget_progress_bar(float value);
int widget_circle_progress(float value);
int widget_loading_spinner();
int widget_skeleton();

// ===== Navigation (10) =====
int widget_app_bar(const std::string& title);
int widget_bottom_bar();
int widget_tab_bar();
int widget_drawer();
int widget_breadcrumb();
int widget_nav_item(const std::string& label);
int widget_menu_item(const std::string& label);
int widget_toolbar();
int widget_status_bar(const std::string& text);
int widget_side_menu();

// ===== Feedback (10) =====
int widget_dialog(const std::string& title, const std::string& message);
int widget_alert(const std::string& message, const std::string& type);
int widget_toast(const std::string& message);
int widget_snackbar(const std::string& message);
int widget_banner(const std::string& message);
int widget_modal();
int widget_popover();
int widget_notification(const std::string& title, const std::string& msg);
int widget_confirm_dialog(const std::string& title, const std::string& msg);
int widget_tooltip(const std::string& text);

// ===== Cards & Panels (7) =====
int widget_info_card(const std::string& title, const std::string& body);
int widget_stat_card(const std::string& label, const std::string& value);
int widget_expansion_panel(const std::string& title);
int widget_accordion();
int widget_panel(const std::string& title);
int widget_group_box(const std::string& title);
int widget_callout(const std::string& text, const std::string& type);

// ===== Data (10) =====
int widget_list_tile(const std::string& title, const std::string& subtitle);
int widget_data_table(int cols);
int widget_table_row();
int widget_table_cell(const std::string& text);
int widget_detail_row(const std::string& key, const std::string& val);
int widget_timeline();
int widget_timeline_item(const std::string& title);
int widget_tree_item(const std::string& label, int depth);
int widget_stepper(int steps);
int widget_step_indicator(int total, int current);

// ===== Charts (7) =====
int widget_bar_chart();
int widget_hbar_chart();
int widget_pie_chart();
int widget_line_chart();
int widget_spark_line();
int widget_gauge(float value, float max_val);
int widget_donut_chart();

// ===== Pagination (2) =====
int widget_pagination(int total_pages);
int widget_dot_indicator(int total, int current);

// ===== Advanced (3) =====
int widget_canvas(float w, float h);
int widget_split_view();
int widget_carousel();

// ===== Widget Tree =====
void widget_add_child(int parentId, int childId);
void widget_clear_children(int parentId);

// ===== Widget Properties =====
void widget_set_bg(int id, int r, int g, int b);
void widget_set_padding(int id, float all);
void widget_set_padding4(int id, float t, float r, float b, float l);
void widget_set_height(int id, float h);
void widget_set_width(int id, float w);
void widget_set_flex(int id, float flex);
void widget_set_border_radius(int id, float radius);
void widget_set_spacing(int id, float gap);
void widget_set_visible(int id, bool v);
void widget_set_border(int id, float width, int r, int g, int b);

// ===== Text Properties =====
void        text_set_size(int id, float size);
void        text_set_color(int id, int r, int g, int b);
void        text_set_content(int id, const std::string& content);
std::string text_get_content(int id);
void        text_set_align(int id, const std::string& align);

// ===== Button Properties =====
void button_set_color(int id, int r, int g, int b);
bool button_was_clicked(int id);
bool button_was_clicked(const std::string& name);

// ===== TextField =====
std::string textfield_get_text(int id);
std::string textfield_get_text(const std::string& name);
void        textfield_set_text(int id, const std::string& text);

// ===== Extended Properties for new widgets =====
void   widget_set_checked(int id, bool checked);
bool   widget_get_checked(int id);
bool   widget_get_checked(const std::string& name);
void   widget_set_value(int id, float val);
float  widget_get_value(int id);
float  widget_get_value(const std::string& name);
void   widget_set_selected(int id, int index);
int    widget_get_selected(int id);
void   widget_set_expanded(int id, bool exp);
bool   widget_get_expanded(int id);
void   widget_set_items(int id, const std::vector<std::string>& items);
void   widget_add_item(int id, const std::string& item);
void   widget_set_data(int id, const std::vector<float>& data);
void   widget_add_data(int id, float val);
void   widget_set_icon(int id, char icon);
void   widget_set_active(int id, bool active);
void   widget_set_subtitle(int id, const std::string& sub);
void   widget_set_level(int id, int level);
void   widget_set_columns(int id, int cols);
void   widget_set_page(int id, int page);
void   widget_set_accent_color(int id, int r, int g, int b);
void   widget_show_dialog(int id, bool show);
bool   widget_is_dialog_visible(int id);

// ═══════════════════════════════════════════════════════════════════
// القسم الجديد: نظام إنشاء المكونات بالخصائص (Declarative/Flutter-like)
// ─────────────────────────────────────────────────────────────────
// يتيح إنشاء مكونات باستخدام خريطة خصائص (Map) بأسماء عربية
// مثال: نص({"محتوى": "مرحبا", "حجم": 24, "لون": "#FF0000"})
// ═══════════════════════════════════════════════════════════════════

// تحويل لون من سلسلة هيكس "#RRGGBB" أو "#RGB" إلى RGBA
RGBA parseColor(const std::string& hex);

// إنشاء مكون من اسم النوع + خريطة خصائص
// يُرجع معرّف المكون الجديد
// typeName: اسم النوع بالعربية (مثل "نص"، "زر"، "عمود"، "صف"...)
// props: خريطة الخصائص بأسماء عربية
struct PropValue {
    enum Type { NONE, STRING, NUMBER, BOOL, COLOR };
    Type type = NONE;
    std::string strVal;
    double numVal = 0;
    bool boolVal = false;
    RGBA colorVal = {0,0,0,255};
};

int widget_create_from_props(const std::string& typeName,
                             const std::unordered_map<std::string, PropValue>& props);

// إنشاء تطبيق من خريطة خصائص
int app_create_from_props(const std::unordered_map<std::string, PropValue>& props);

// تطبيق خصائص على مكون موجود (إعادة استخدام من الداخل)
void apply_props_to_widget(int id, const std::unordered_map<std::string, PropValue>& props);

// ═══════════════════════════════════════════════════════════════════
// القسم الجديد: النظام التفاعلي (Reactive Framework)
// ─────────────────────────────────────────────────────────────────
// نظام شبيه بـ Flutter يوفر:
// - إعادة بناء تلقائية عند تغيّر الحالة
// - اكتشاف الأحداث (ضغط، نص، قيمة، تبديل)
// - حفظ واستعادة حالة الودجات التفاعلية
// - نظام ثيمات عالمي
// - تنظيف الودجات القديمة
// ═══════════════════════════════════════════════════════════════════

// ───── حدث واجهة المستخدم ─────
// يُمثّل حدثاً واحداً حصل على ودجت (ضغط زر، تغيّر نص، إلخ)
struct UIEvent {
    std::string type;       // نوع الحدث: "ضغط", "نص", "قيمة", "تبديل"
    int widgetId;           // معرّف الودجت
    std::string textData;   // بيانات نصية (نص الزر، نص الحقل...)
    double numData;         // بيانات رقمية (قيمة المنزلق...)
    bool boolData;          // بيانات منطقية (حالة مربع الاختيار...)
};

// ───── نظام الثيمات ─────
// ثيمة عالمية تُطبّق كقيم افتراضية على جميع الودجات الجديدة
struct Theme {
    // الألوان الأساسية
    RGBA primaryColor   = {25, 118, 210, 255};    // أزرق Material
    RGBA secondaryColor = {255, 152, 0, 255};     // برتقالي Material
    RGBA backgroundColor= {250, 250, 250, 255};   // خلفية فاتحة
    RGBA surfaceColor   = {255, 255, 255, 255};    // لون السطح/البطاقات
    RGBA textColor      = {33, 33, 33, 255};       // لون النص الأساسي
    RGBA textSecondary  = {117, 117, 117, 255};    // لون النص الثانوي
    RGBA errorColor     = {244, 67, 54, 255};      // أحمر الخطأ
    RGBA successColor   = {76, 175, 80, 255};      // أخضر النجاح
    RGBA warningColor   = {255, 193, 7, 255};      // أصفر التحذير
    RGBA dividerColor   = {224, 224, 224, 255};    // لون الفواصل
    
    // الخطوط والأحجام
    float fontSizeSmall   = 12;
    float fontSizeMedium  = 16;
    float fontSizeLarge   = 20;
    float fontSizeTitle   = 24;
    float fontSizeHeading = 32;
    
    // التباعد والحشوة
    float paddingSmall  = 4;
    float paddingMedium = 8;
    float paddingLarge  = 16;
    float paddingXLarge = 24;
    float spacingDefault = 8;
    
    // الحدود
    float borderRadius  = 8;
    float borderWidth   = 1;
    
    // ارتفاعات العناصر
    float buttonHeight  = 48;
    float appBarHeight  = 56;
    float inputHeight   = 48;
};

// الحصول على الثيمة الحالية (للقراءة)
const Theme& getTheme();

// تعيين خاصية ثيمة بالاسم العربي
// مثال: setThemeProperty("أساسي", "#1976D2")
// مثال: setThemeProperty("خط", 18)
void setThemeProperty(const std::string& propName, const std::string& strValue);
void setThemeProperty(const std::string& propName, double numValue);

// ───── جمع الأحداث ─────
// يمسح شجرة الودجات ويُرجع قائمة بجميع الأحداث التي حصلت
// (أزرار مضغوطة، نصوص تغيّرت، قيم تغيّرت، مفاتيح بُدّلت)
std::vector<UIEvent> collectEvents(int rootWidget);

// ───── حفظ واستعادة حالة الحقول ─────
// يحفظ نصوص جميع حقول الإدخال في الشجرة (المفتاح = التلميح أو الترتيب)
std::map<std::string, std::string> saveTextFieldStates(int rootWidget);
// يستعيد النصوص المحفوظة إلى حقول الإدخال المطابقة
void restoreTextFieldStates(int rootWidget, const std::map<std::string, std::string>& states);

// ───── تنظيف الودجات ─────
// يحذف جميع الودجات من السجل العالمي (لإعادة البناء)
void widgetCleanupAll();
// يحذف ودجت محدد وجميع أبنائه بشكل تكراري
void widgetCleanupTree(int rootId);

// ───── ضبط خاصية واحدة ─────
// يُعدّل خاصية واحدة على ودجت موجود
// propName: اسم الخاصية بالعربية (محتوى، حجم، لون، خلفية...)
// value: قيمة نصية أو رقمية
void widgetSetProp(int id, const std::string& propName, const std::string& strValue);
void widgetSetProp(int id, const std::string& propName, double numValue);
void widgetSetProp(int id, const std::string& propName, bool boolValue);

// ───── ضبط الأطفال من مصفوفة ─────
// يمسح الأطفال الحاليين ويُعيّن أطفال جدد من المصفوفة
void widgetSetChildren(int parentId, const std::vector<int>& childIds);

}} // namespace sad::ui
