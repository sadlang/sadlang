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
    float contentHeight = 0;     // [NEW] إجمالي ارتفاع المحتوى (للتمرير)
    bool masked = false;         // PasswordField
    int level = 1;               // Heading (1-6), TreeItem depth
    bool active = false;         // NavItem, MenuItem, StepIndicator
    int currentPage = 0;         // PageView, Pagination
    int totalPages = 1;          // Pagination
    bool dialogVisible = false;  // Dialog, Modal, Popover, Drawer

    // [NEW] محاذاة المحاور — Flutter-like alignment
    std::string mainAxisAlignment = "start";   // "start", "center", "end", "spaceBetween", "spaceEvenly"
    std::string crossAxisAlignment = "start";  // "start", "center", "end", "stretch"

    // ═══ NEW: Enhanced Properties (v2.0) ═══
    // رؤية وشفافية
    float opacity = 1.0f;           // 0.0 = شفاف، 1.0 = مرئي بالكامل
    bool enabled = true;            // تفعيل/تعطيل التفاعل

    // ظل (Shadow / Elevation)
    float shadowRadius = 0;         // نصف قطر الظل (0 = بدون ظل)
    float shadowOffsetX = 0;        // إزاحة الظل أفقياً
    float shadowOffsetY = 2;        // إزاحة الظل عمودياً
    RGBA shadowColor = {0,0,0,64};  // لون الظل

    // تلميح (Tooltip)
    std::string tooltipText;        // نص التلميح عند التحويم

    // قيود الحجم (Size Constraints)
    float minWidth = -1, maxWidth = -1;
    float minHeight = -1, maxHeight = -1;

    // هامش (Margin) — خارج الحدود
    float marginTop=0, marginRight=0, marginBottom=0, marginLeft=0;
    float margin = 0;                // هامش موحد

    // تدرج لوني (Gradient)
    RGBA gradientStart = {0,0,0,0};
    RGBA gradientEnd = {0,0,0,0};
    bool hasGradient = false;

    // ألوان تحويم وضغط مخصصة
    RGBA hoverColor = {0,0,0,0};    // لون مخصص عند التحويم (0,0,0,0 = تلقائي)
    RGBA pressColor = {0,0,0,0};    // لون مخصص عند الضغط

    // خصائص النص المتقدمة
    std::string fontWeight = "normal"; // "normal", "bold"
    bool italic = false;
    float lineHeight = 1.2f;        // تباعد الأسطر

    // تحويلات (Transforms)
    float rotation = 0;             // الدوران بالدرجات
    float scaleX = 1.0f, scaleY = 1.0f;

    // ترتيب العرض
    int zIndex = 0;                 // ترتيب الطبقة (أعلى = فوق)
    int tabIndex = -1;              // ترتيب التنقل بالتاب

    // أيقونة متعددة المحارف (بديل iconChar)
    std::string iconText;           // يدعم إيموجي ونص كامل

    // الأب والـ callbacks
    int parentId = -1;              // معرف الأب
    int callbackId = -1;            // معرف دالة الاستدعاء
    float animationProgress = 0;    // تقدم الحركة (0-1)

    // تمرير أفقي
    float scrollX = 0;

    // نوع الفائض
    std::string overflow = "visible"; // "visible", "clip", "scroll"

    // المؤشر
    std::string cursor = "default";   // "default", "pointer", "text", "grab"

    // القيمة السابقة للمنزلق (لاكتشاف التغيّر)
    float _prevSliderValue = -99999.f;

    // ═══ FIX #1-30: خصائص جديدة (v3.0) ═══
    // [FIX #1] تغليف النص (Text Wrapping)
    bool textWrap = false;              // تفعيل التغليف التلقائي
    std::string textOverflow = "clip";  // "clip", "ellipsis", "fade"
    int maxLines = 0;                   // الحد الأقصى للأسطر (0 = بلا حد)

    // [FIX #2] تنسيق النص المتقدم
    float letterSpacing = 0;            // تباعد بين الحروف
    std::string textDecoration = "none"; // "none", "underline", "strikethrough", "overline"
    std::string textTransform = "none";  // "none", "uppercase", "lowercase", "capitalize"

    // [FIX #3] نسبة الأبعاد
    float aspectRatio = 0;              // 0 = لا نسبة (مثل 16.0/9.0 = 1.778)

    // [FIX #4] الارتفاع والشكل
    float elevation = 0;               // مستوى الارتفاع بديل للظل (0-24)
    std::string shape = "rectangle";    // "rectangle", "circle", "stadium", "rounded"
    bool clipToBounds = false;          // قص الأبناء الخارجين عن الحدود

    // [FIX #5] اتجاه التخطيط
    bool reverse = false;               // عكس ترتيب الأبناء
    float gap = 0;                      // [FIX #6] فجوة بين الأبناء (بديل spacing)

    // [FIX #7] مرونة CSS Flex
    float flexGrow = 0, flexShrink = 1;
    std::string alignSelf = "auto";     // "auto", "start", "center", "end", "stretch"

    // [FIX #8] التركيز والتنقل
    bool focusable = true;              // يمكن التركيز عليه
    bool hasFocus = false;              // هل مركّز حالياً
    RGBA focusColor = {66,133,244,128}; // لون حلقة التركيز

    // [FIX #9] السحب والإفلات
    bool draggable = false;
    bool droppable = false;

    // [FIX #10] تأثير الضغط
    float rippleRadius = 0;             // تأثير موجة عند النقر
    float _rippleProgress = 0;
    int _rippleX = 0, _rippleY = 0;

    // [FIX #11] حقول الإدخال المتقدمة
    int cursorPosition = -1;            // موضع المؤشر (-1 = آخر)
    int selectionStart = -1, selectionEnd = -1; // نطاق التحديد
    bool readOnly = false;              // للقراءة فقط
    std::string placeholder;             // نص بديل (بدل text للتلميح)
    std::string inputType = "text";      // "text", "number", "email", "url", "phone"

    // [FIX #12] حدود مخصصة لكل جانب
    float borderTopWidth = 0, borderRightWidth = 0, borderBottomWidth = 0, borderLeftWidth = 0;

    // [FIX #13] تمرير متقدم
    float scrollSpeed = 40;             // سرعة التمرير
    bool scrollbarVisible = true;       // إظهار/إخفاء شريط التمرير
    std::string scrollDirection = "vertical"; // "vertical", "horizontal", "both"

    // [FIX #14] التدرج المتقدم
    std::string gradientDirection = "vertical"; // "vertical", "horizontal", "diagonal"
    float gradientAngle = 0;                    // زاوية مخصصة

    // [FIX #15] خلفية ثانية (overlay)
    RGBA overlayColor = {0,0,0,0};      // لون تراكب فوق الخلفية

    // [FIX #16-20] متنوعات
    std::string semanticLabel;           // [FIX #16] تسمية وصفية (Accessibility)
    bool selected = false;               // [FIX #17] حالة التحديد
    float progress = 0;                  // [FIX #18] تقدم عام (0-1)
    std::string badge;                   // [FIX #19] نص شارة صغيرة
    int badgeCount = 0;                  // [FIX #20] عدد الشارة

    // [FIX #21-25] حالات مرئية
    bool showDivider = false;            // [FIX #21] إظهار فاصل تحت العنصر
    bool showShadow = false;             // [FIX #22] تفعيل الظل التلقائي
    bool compact = false;                // [FIX #23] وضع مضغوط
    bool dense = false;                  // [FIX #24] وضع كثيف
    float contentPadding = 0;            // [FIX #25] حشوة المحتوى الداخلي

    // [FIX #26-30] قيمة ثالثة ومتنوعات
    float value3 = 0;                   // [FIX #26] قيمة ثالثة (للمخططات)
    std::string hint;                    // [FIX #27] تلميح ثانوي
    std::string errorText;               // [FIX #28] نص الخطأ للحقول
    bool hasError = false;               // [FIX #29] حالة الخطأ
    std::string helperText;              // [FIX #30] نص المساعدة

    // ═══════════════════════════════════════════════════════════════════════════
    // [NEW v4.0] 100 حقل Widget جديد — Flutter-Level Comprehensive Properties
    // ═══════════════════════════════════════════════════════════════════════════

    // ═══ [31-40] Animation & Transitions ═══
    float animationDuration = 300;        // [31] مدة الرسوم المتحركة (ms)
    std::string animationCurve = "ease";  // [32] "ease", "linear", "easeIn", "easeOut", "bounce"
    float animationDelay = 0;             // [33] تأخير البدء
    bool animating = false;               // [34] هل يتم تحريكه حالياً
    float animationValue = 0;             // [35] القيمة الحالية للحركة (0-1)
    std::string transitionProperty = "";  // [36] الخاصية المتحركة
    float springStiffness = 200;          // [37] صلابة الزنبرك
    float springDamping = 20;             // [38] تخميد الزنبرك
    float springVelocity = 0;             // [39] سرعة الزنبرك
    bool useSpringPhysics = false;        // [40] استخدام فيزياء الزنبرك

    // ═══ [41-50] Gestures & Touch ═══
    bool swipeEnabled = true;             // [41] تمكين السحب
    float swipeThreshold = 50;            // [42] عتبة السحب
    std::string swipeDirection = "both"; // [43] "horizontal", "vertical", "both"
    bool longPressEnabled = true;         // [44] تمكين الضغط المطول
    int longPressDuration = 500;          // [45] مدة الضغط المطول (ms)
    bool panEnabled = false;              // [46] تمكين السحب الحر
    float panX = 0, panY = 0;             // [47-48] إزاحة السحب الحر
    bool pinchEnabled = false;            // [49] تمكين القرص للتكبير
    float pinchScale = 1.0f;              // [50] مقياس القرص

    // ═══ [51-60] Layout Advanced ═══
    std::string position = "relative";    // [51] "relative", "absolute", "fixed", "sticky"
    float posTop = 0, posRight = 0, posBottom = 0, posLeft = 0; // [52-55] للموضع المطلق
    std::string display = "flex";         // [56] "flex", "block", "inline", "none", "grid"
    std::string flexDirection = "column"; // [57] "row", "column", "row-reverse", "column-reverse"
    std::string flexWrap = "nowrap";      // [58] "nowrap", "wrap", "wrap-reverse"
    std::string justifyContent = "start"; // [59] "start", "center", "end", "space-between", "space-around"
    std::string alignItems = "stretch";   // [60] "stretch", "start", "center", "end", "baseline"

    // ═══ [61-70] Grid Layout ═══
    int gridRows = 0;                     // [61] عدد صفوف Grid (0 = تلقائي)
    float gridRowGap = 0;                 // [62] فجوة بين الصفوف
    float gridColGap = 0;                 // [63] فجوة بين الأعمدة  
    int gridColumn = 0, gridRow = 0;      // [64-65] موضع العنصر في الشبكة
    int gridColSpan = 1, gridRowSpan = 1; // [66-67] امتداد العنصر
    std::string gridAutoFlow = "row";     // [68] "row", "column", "dense"
    std::string gridTemplateColumns = ""; // [69] قالب الأعمدة "1fr 2fr auto"
    std::string gridTemplateRows = "";    // [70] قالب الصفوف

    // ═══ [71-80] Typography Extended ═══
    std::string fontFamily = "";          // [71] عائلة الخط
    std::string fontStyle = "normal";     // [72] "normal", "italic", "oblique"
    int fontWeightNum = 400;              // [73] 100-900 وزن الخط رقمياً
    std::string textShadow = "";          // [74] ظل النص "2px 2px 4px black"
    float wordSpacing = 0;                // [75] تباعد الكلمات
    std::string textDirection = "auto";   // [76] "auto", "ltr", "rtl"
    std::string writingMode = "horizontal"; // [77] "horizontal", "vertical"
    float textIndent = 0;                 // [78] إزاحة السطر الأول
    std::string whiteSpace = "normal";    // [79] "normal", "nowrap", "pre", "pre-wrap"
    std::string textJustify = "auto";     // [80] "auto", "inter-word", "inter-character"

    // ═══ [81-90] Borders Extended ═══
    RGBA borderTopColor = {0,0,0,0};      // [81] لون الحد العلوي
    RGBA borderRightColor = {0,0,0,0};    // [82] لون الحد الأيمن
    RGBA borderBottomColor = {0,0,0,0};   // [83] لون الحد السفلي
    RGBA borderLeftColor = {0,0,0,0};     // [84] لون الحد الأيسر
    std::string borderStyle = "solid";    // [85] "solid", "dashed", "dotted", "double"
    float borderTopLeftRadius = -1;       // [86] نصف قطر - أعلى يسار (-1 = عام)
    float borderTopRightRadius = -1;      // [87] نصف قطر - أعلى يمين
    float borderBottomRightRadius = -1;   // [88] نصف قطر - أسفل يمين
    float borderBottomLeftRadius = -1;    // [89] نصف قطر - أسفل يسار
    std::string boxSizing = "border-box"; // [90] "border-box", "content-box"

    // ═══ [91-100] Effects & Filters ═══
    float blur = 0;                       // [91] تمويه (px)
    float brightness = 1.0f;              // [92] سطوع (0-2)
    float contrast = 1.0f;                // [93] تباين (0-2) 
    float saturation = 1.0f;              // [94] تشبع (0-2)
    float hueRotate = 0;                  // [95] تدوير اللون (درجات)
    bool grayscale = false;               // [96] تدرج رمادي
    bool invert = false;                  // [97] عكس الألوان
    float sepia = 0;                      // [98] تأثير بني قديم (0-1)
    std::string blendMode = "normal";     // [99] "normal", "multiply", "screen", "overlay"
    std::string backdropFilter = "";      // [100] فلتر الخلفية

    // ═══ [101-110] State & Interaction ═══
    std::string state = "normal";         // [101] "normal", "hover", "active", "disabled", "focus"
    bool isLoading = false;               // [102] حالة التحميل
    std::string loadingText = "";         // [103] نص أثناء التحميل
    bool isSuccess = false;               // [104] حالة النجاح
    bool isWarning = false;               // [105] حالة التحذير
    std::string validationState = "";     // [106] "valid", "invalid", "pending"
    int clickCount = 0;                   // [107] عدد النقرات
    double lastClickTime = 0;             // [108] وقت آخر نقرة
    bool preventDoubleClick = false;      // [109] منع النقر المزدوج
    bool capturePointer = false;          // [110] التقاط المؤشر

    // ═══ [111-120] Content & Data ═══
    std::string contentType = "";         // [111] نوع المحتوى
    std::string dataSource = "";          // [112] مصدر البيانات
    std::string bindingPath = "";         // [113] مسار الربط
    bool isTemplate = false;              // [114] هل هو قالب
    int repeatCount = 0;                  // [115] عدد التكرار
    std::string filterPredicate = "";     // [116] شرط الفلترة
    std::string sortProperty = "";        // [117] خاصية الترتيب
    bool sortAscending = true;            // [118] ترتيب تصاعدي
    int pageSize = 20;                    // [119] حجم الصفحة
    int currentIndex = 0;                 // [120] الفهرس الحالي

    // ═══ [121-130] Accessibility Extended ═══
    std::string role = "";                // [121] دور ARIA
    std::string ariaLabel = "";           // [122] تسمية ARIA
    std::string ariaDescribedBy = "";     // [123] موصوف بواسطة
    std::string ariaLabelledBy = "";      // [124] مسمى بواسطة
    bool ariaHidden = false;              // [125] مخفي من قارئ الشاشة
    bool ariaExpanded = false;            // [126] موسع
    bool ariaSelected = false;            // [127] محدد
    int ariaLevel = 0;                    // [128] المستوى
    int ariaSetSize = 0;                  // [129] حجم المجموعة
    int ariaPosInSet = 0;                 // [130] الموضع في المجموعة

    // ═══ [131-140] Callbacks & Events Extended ═══
    int hoverCallbackId = -1;             // [131] معرف دالة التحويم
    int focusCallbackId = -1;             // [132] معرف دالة التركيز
    int blurCallbackId = -1;              // [133] معرف دالة فقدان التركيز
    int changeCallbackId = -1;            // [134] معرف دالة التغيير
    bool required = false;                // [135] حقل مطلوب

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

// ═══ NEW v2.0: Enhanced Property APIs ═══
// شفافية
void   widget_set_opacity(int id, float opacity);
float  widget_get_opacity(int id);
// تفعيل/تعطيل
void   widget_set_enabled(int id, bool enabled);
bool   widget_is_enabled(int id);
// ظل
void   widget_set_shadow(int id, float radius, float offsetX, float offsetY, int r, int g, int b, int a);
float  widget_get_shadow_radius(int id);
// تلميح
void   widget_set_tooltip(int id, const std::string& text);
std::string widget_get_tooltip(int id);
// قيود الحجم
void   widget_set_min_size(int id, float minW, float minH);
void   widget_set_max_size(int id, float maxW, float maxH);
// هامش
void   widget_set_margin(int id, float all);
void   widget_set_margin4(int id, float top, float right, float bottom, float left);
// تدرج لوني
void   widget_set_gradient(int id, int r1, int g1, int b1, int r2, int g2, int b2);
// ألوان تحويم وضغط مخصصة
void   widget_set_hover_color(int id, int r, int g, int b);
void   widget_set_press_color(int id, int r, int g, int b);
// خصائص النص المتقدمة
void   widget_set_font_weight(int id, const std::string& weight);
void   widget_set_italic(int id, bool italic);
void   widget_set_line_height(int id, float lh);
// تحويلات
void   widget_set_rotation(int id, float degrees);
void   widget_set_scale(int id, float sx, float sy);
// ترتيب
void   widget_set_z_index(int id, int z);
void   widget_set_tab_index(int id, int idx);
// أيقونة نصية
void   widget_set_icon_text(int id, const std::string& text);
std::string widget_get_icon_text(int id);
// تمرير
void   widget_set_scroll_position(int id, float x, float y);
float  widget_get_scroll_x(int id);
float  widget_get_scroll_y(int id);
// نوع الفائض
void   widget_set_overflow(int id, const std::string& mode);
// اسم المكون
void   widget_set_name(int id, const std::string& name);
std::string widget_get_name(int id);
// استعلام المكون
std::string widget_get_type_name(int id);
int    widget_get_children_count(int id);
int    widget_get_child_at(int id, int index);
void   widget_remove_child(int parentId, int childId);
int    widget_find_by_name(const std::string& name);
// موضع وحجم
float  widget_get_x(int id);
float  widget_get_y(int id);
float  widget_get_width(int id);
float  widget_get_height(int id);
// مجموع الصفحات
void   widget_set_total_pages(int id, int total);
// القيمة الثانية للمنزلق المزدوج
void   widget_set_value2(int id, float val);
float  widget_get_value2(int id);
// نص الإدخال
void   widget_set_input_text(int id, const std::string& text);
std::string widget_get_input_text(int id);
// العناصر والبيانات بالجملة
void   widget_clear_items(int id);
void   widget_clear_data(int id);
void   widget_add_data_color(int id, int r, int g, int b);
// مقنّع (PasswordField)
void   widget_set_masked(int id, bool masked);
bool   widget_get_masked(int id);

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

// ═══ NEW v2.0: Full Event Data Access ═══
// بعد جمع الأحداث, يمكن الوصول لتفاصيل كل حدث
int    events_count(int rootWidget);           // عدد الأحداث
std::string event_get_type(int index);         // نوع الحدث
int    event_get_widget_id(int index);         // معرف المكون
std::string event_get_text(int index);         // بيانات نصية
double event_get_number(int index);            // بيانات رقمية
bool   event_get_bool(int index);              // بيانات منطقية
void   events_clear();                         // مسح الأحداث المخزنة مؤقتاً

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

// ═══════════════════════════════════════════════════════════════════
// [NEW v2.1] دوال API إضافية — تحسينات شاملة (FIX #90-150)
// ═══════════════════════════════════════════════════════════════════

// ───── استعلامات ─────
int widget_count();                               // عدد العناصر المسجلة
int widget_get_parent(int widgetId);              // معرف الأب (-1 إذا لم يجد)
std::vector<int> widget_find_by_type(WidgetType type); // البحث حسب النوع
int widget_find_by_name_fast(const std::string& name); // بحث سريع بالاسم

// ───── تعديل الشجرة ─────
void widget_add_child_tracked(int parentId, int childId); // إضافة ابن مع تتبع الأب
int  widget_clone(int widgetId);                   // نسخ عنصر (بدون أبناء)
int  widget_deep_clone(int widgetId);              // نسخ عنصر مع الأبناء
void widget_remove(int widgetId);                  // إزالة عنصر وأبنائه
void widget_reparent(int widgetId, int newParentId); // نقل عنصر لأب جديد

// ───── التطبيق ─────
void app_set_title(int appId, const std::string& title);
std::string app_get_title(int appId);
double app_get_fps(int appId);
double app_get_delta_time(int appId);
int  app_get_width(int appId);
int  app_get_height(int appId);
void app_set_size(int appId, int w, int h);

// ───── تمرير ─────
void widget_scroll_by(int widgetId, float deltaY);
void widget_scroll_to(int widgetId, float y);

// ───── تموضع ─────
void widget_move_to(int widgetId, float x, float y);

// ───── تنسيق متقدم ─────
void widget_set_alignment(int widgetId, const std::string& main, const std::string& cross);
void widgetSetPropString_v2(int widgetId, const std::string& prop, const std::string& val);

// ═══════════════════════════════════════════════════════════════════
// [NEW v3.0] نظام الخصائص السطرية (Inline Property System)
// ─────────────────────────────────────────────────────────────────
// يدعم 120+ خاصية للتعيين المباشر عند إنشاء المكونات
// مثال: نص_واجهة("مرحبا"، "لون"، "احمر"، "حجم_خط"، 20)
// ═══════════════════════════════════════════════════════════════════
bool isKnownPropertyName(const std::string& name);

// ═══════════════════════════════════════════════════════════════════
// [NEW v3.0] 40 مصنع مكونات مركبة (Convenience Widget Factories)
// ─────────────────────────────────────────────────────────────────
// مكونات مبنية من تركيب عناصر موجودة — لا تحتاج WidgetType جديد
// ═══════════════════════════════════════════════════════════════════
int convenience_scaffold(const std::string& title, int bodyId, int bottomBarId = -1);
int convenience_safe_area(int childId, float topInset=24, float bottomInset=24);
int convenience_padding_widget(int childId, float all);
int convenience_padding_widget4(int childId, float top, float right, float bottom, float left);
int convenience_align(int childId, const std::string& mainAlign, const std::string& crossAlign);
int convenience_aspect_ratio(int childId, float ratio);
int convenience_constrained_box(int childId, float minW, float minH, float maxW, float maxH);
int convenience_opacity_widget(int childId, float opacity);
int convenience_decorated_box(int childId, int r, int g, int b, float radius, float borderW);
int convenience_clip_rrect(int childId, float radius);
int convenience_flexible(int childId, float flexFactor);
int convenience_circle_avatar(const std::string& letter, int r, int g, int b, float size);
int convenience_search_bar(const std::string& hint, int r=33, int g=150, int b=243);
int convenience_bottom_sheet(int contentId, float height);
int convenience_navigation_rail(int selectedIdx);
int convenience_segmented_button();
int convenience_filter_chip(const std::string& label, bool selected);
int convenience_choice_chip(const std::string& label, bool selected);
int convenience_input_chip(const std::string& label);
int convenience_action_chip(const std::string& label);
int convenience_form_group(const std::string& label);
int convenience_list_section(const std::string& title);
int convenience_empty_state(const std::string& message, const std::string& icon);
int convenience_error_widget(const std::string& message);
int convenience_placeholder(float w, float h);
int convenience_kpi_card(const std::string& label, const std::string& value, const std::string& trend);
int convenience_countdown(int seconds);
int convenience_divider_themed(const std::string& color, float thickness);
int convenience_gradient_card(const std::string& title, int r1, int g1, int b1, int r2, int g2, int b2);
int convenience_outlined_card(const std::string& title, int r, int g, int b);
int convenience_hero_header(const std::string& title, const std::string& subtitle, int r, int g, int b);
int convenience_login_form(const std::string& title);
int convenience_avatar_group(int count);
int convenience_toolbar_action(const std::string& icon, const std::string& label);
int convenience_tag_group();
int convenience_rating_bar(int maxStars, float initialValue);
int convenience_progress_indicator(float value, const std::string& label);
int convenience_split_button(const std::string& text);
int convenience_fab_extended(const std::string& label, char icon);
int convenience_chip_group();
int convenience_nav_drawer(const std::string& title);

}} // namespace sad::ui
