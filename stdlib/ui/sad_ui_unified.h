/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_ui_unified.h — طبقة التجريد الموحدة لواجهات المستخدم
 *  SadUI Unified Abstraction Layer
 * ═══════════════════════════════════════════════════════════════════════════════
 *  
 *  الغرض: واجهة واحدة فقط يستدعيها مفسر لغة ص (والمترجم)، مع ثلاث
 *  تنفيذات (Backends):
 *    1) سطح المكتب — SDL2  (Windows / Linux / macOS)
 *    2) أندرويد   — EGL + GLES3 (Android NDK)
 *    3) الويب     — Canvas2D عبر Emscripten / WASM
 *
 *  الفكرة الجوهرية:
 *    كود لغة ص يستدعي دوال عربية مثل عمود() وزر("نص")  
 *    → المفسر يستدعي sad::unified::* من هذه الواجهة
 *    → الواجهة تُحول الاستدعاء إلى Backend المناسب تلقائياً
 *
 *  المؤلف: فريق لغة ص
 *  التاريخ: 2026-03-05
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

namespace sad {
namespace unified {

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع المكونات الموحدة — يُغطّي كلاً من سطح المكتب وأندرويد
//  Unified Widget Types — superset of Desktop (109) + Android (85)
// ═══════════════════════════════════════════════════════════════════════════════

enum class WidgetType {
    // ─── حاويات / Containers ───
    Container,          // حاوية
    Column,             // عمود
    Row,                // صف
    Stack,              // مكدس / طبقات
    Center,             // وسط
    Wrap,               // التفاف
    ScrollView,         // تمرير
    SizedBox,           // صندوق بحجم ثابت
    Expanded,           // مُوسّع
    Card,               // بطاقة
    Grid,               // شبكة
    ListView,           // قائمة
    PageView,           // عرض صفحات
    Table,              // جدول
    SplitView,          // عرض مقسوم
    SlidingPanel,       // لوحة منزلقة

    // ─── نصوص / Text ───
    Text,               // نص
    Label,              // تسمية
    Heading,            // عنوان
    Paragraph,          // فقرة
    Link,               // رابط
    CodeBlock,          // كتلة كود
    Quote,              // اقتباس
    Markdown,           // ماركداون
    RichText,           // نص غني

    // ─── أزرار / Buttons ───
    Button,             // زر
    IconButton,         // زر أيقونة
    FloatingButton,     // زر عائم (FAB)
    OutlineButton,      // زر بإطار
    TextButton,         // زر نصي
    ToggleButton,       // زر تبديل
    ButtonGroup,        // مجموعة أزرار
    SpeedDial,          // أزرار سريعة متفرعة

    // ─── إدخال / Input ───
    TextField,          // حقل نصي
    PasswordField,      // حقل كلمة مرور
    TextArea,           // منطقة نص
    Checkbox,           // مربع اختيار
    Radio,              // زر راديو
    Switch,             // مفتاح تبديل
    Slider,             // منزلق
    RangeSlider,        // منزلق نطاقي
    Dropdown,           // قائمة منسدلة
    NumberInput,        // إدخال رقمي
    SearchField,        // حقل بحث
    DateField,          // حقل تاريخ
    DatePicker,         // منتقي التاريخ
    TimePicker,         // منتقي الوقت
    Calendar,           // تقويم
    ColorPicker,        // منتقي الألوان
    RatingStars,        // تقييم نجوم
    ToggleGroup,        // مجموعة تبديل
    SegmentedControl,   // تحكم مجزأ
    Knob,               // مقبض دوران
    SignaturePad,       // لوحة توقيع

    // ─── عرض بصري / Display ───
    Icon,               // أيقونة
    Image,              // صورة
    Avatar,             // صورة رمزية
    Badge,              // شارة
    Chip,               // رقاقة
    Tag,                // علامة
    StatusDot,          // نقطة حالة
    ColorBox,           // صندوق لون
    GradientBox,        // صندوق تدرج
    Divider,            // خط فاصل
    Spacer,             // فاصل
    Separator,          // فاصل

    // ─── تقدم / Progress ───
    ProgressBar,        // شريط تقدم
    CircleProgress,     // تقدم دائري
    LoadingSpinner,     // مؤشر تحميل
    Skeleton,           // هيكل عظمي (تحميل)
    Gauge,              // مؤشر قياس

    // ─── تنقل / Navigation ───
    AppBar,             // شريط التطبيق
    BottomBar,          // تنقل سفلي
    TabBar,             // علامات تبويب
    Drawer,             // درج جانبي
    Breadcrumb,         // فتات الخبز
    NavItem,            // عنصر تنقل
    MenuItem,           // عنصر قائمة
    Toolbar,            // شريط أدوات
    StatusBar,          // شريط حالة
    SideMenu,           // قائمة جانبية

    // ─── حوارات وتنبيهات / Feedback ───
    Dialog,             // حوار
    Alert,              // تنبيه
    Toast,              // رسالة مؤقتة
    Snackbar,           // شريط إشعار
    Banner,             // لافتة
    Modal,              // نافذة منبثقة
    Popover,            // بالون
    Notification,       // إشعار
    ConfirmDialog,      // حوار تأكيد
    Tooltip,            // تلميح
    ContextMenu,        // قائمة سياقية
    BottomSheet,        // ورقة سفلية

    // ─── بطاقات ولوحات / Cards & Panels ───
    InfoCard,           // بطاقة معلومات
    StatCard,           // بطاقة إحصائيات
    ExpansionPanel,     // لوحة قابلة للتوسيع
    Accordion,          // طيات
    Panel,              // لوحة
    GroupBox,           // صندوق مجموعة
    Callout,            // صندوق تنبيه

    // ─── بيانات / Data ───
    ListTile,           // عنصر قائمة
    DataTable,          // جدول بيانات
    DataGrid,           // شبكة بيانات متقدمة
    TableRow,           // صف جدول
    TableCell,          // خلية جدول
    DetailRow,          // صف تفاصيل
    Timeline,           // خط زمني
    TimelineItem,       // عنصر خط زمني
    TreeView,           // عرض شجري
    TreeItem,           // عنصر شجري
    Stepper,            // متتبع خطوات
    StepIndicator,      // مؤشر خطوة
    ExpandableList,     // قائمة قابلة للتوسيع
    PullToRefresh,      // اسحب للتحديث

    // ─── مخططات / Charts ───
    BarChart,           // مخطط أعمدة
    HBarChart,          // مخطط أعمدة أفقي
    PieChart,           // مخطط دائري
    LineChart,          // مخطط خطي
    SparkLine,          // خط بياني مصغر
    DonutChart,         // مخطط حلقي
    RadarChart,         // مخطط راداري
    Heatmap,            // خريطة حرارية
    Waveform,           // موجة صوتية

    // ─── ترقيم / Pagination ───
    Pagination,         // ترقيم صفحات
    DotIndicator,       // مؤشر نقطي

    // ─── وسائط / Media ───
    VideoPlayer,        // مشغل فيديو
    AudioPlayer,        // مشغل صوت
    CameraPreview,      // معاينة الكاميرا
    VoiceRecorder,      // مسجل صوت
    LottieAnimation,    // رسوم Lottie

    // ─── متقدم / Advanced ───
    Canvas,             // لوحة رسم
    Carousel,           // دائرة عرض
    WebView,            // عارض ويب
    PDFViewer,          // عارض PDF
    Map,                // خريطة
    QRScanner,          // قارئ QR
    BarcodeScanner,     // قارئ باركود
    CodeEditor,         // محرر كود

    // ─── عداد ─── 
    _WidgetTypeCount
};

// ═══════════════════════════════════════════════════════════════════════════════
//  لون RGBA
// ═══════════════════════════════════════════════════════════════════════════════

struct Color {
    int r = 255, g = 255, b = 255, a = 255;
    Color() = default;
    Color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//  خصائص المكون (Property Bag) — حقيبة خصائص عامة لكل المنصات
// ═══════════════════════════════════════════════════════════════════════════════

struct WidgetProps {
    // النص
    std::string text;
    std::string subtitle;
    float fontSize = 20;
    std::string fontWeight = "normal";
    std::string textAlign = "center";

    // الألوان
    Color bgColor      = {0,0,0,0};
    Color textColor    = {33,33,33,255};
    Color borderColor  = {180,180,180,255};
    Color accentColor  = {66,133,244,255};

    // الأبعاد
    float width  = -1;    // -1 = auto
    float height = -1;
    float padding = 0;
    float margin  = 0;
    float spacing = 4;
    float borderWidth = 0;
    float borderRadius = 0;
    float opacity = 1.0f;
    float flex = 0;
    float elevation = 0;

    // التخطيط
    std::string mainAxisAlignment  = "start";
    std::string crossAxisAlignment = "start";
    std::string overflow = "visible";

    // الحالة
    bool visible = true;
    bool enabled = true;
    bool checked = false;
    float value = 0;
    float minVal = 0, maxVal = 100;
    int selectedIndex = -1;
    bool expanded = false;

    // الأيقونة
    std::string iconText;

    // الأحداث (معرفات callback — يربطها المفسر)
    int onClickCallbackId  = -1;
    int onChangeCallbackId = -1;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  معرّف المكون
// ═══════════════════════════════════════════════════════════════════════════════
using WidgetId = int;

// ═══════════════════════════════════════════════════════════════════════════════
//  واجهة Backend المجرّدة — كل منصة تنفّذ هذه الواجهة
//  Abstract Backend Interface — each platform implements this
// ═══════════════════════════════════════════════════════════════════════════════

class SadUIBackend {
public:
    virtual ~SadUIBackend() = default;

    // ─── دورة حياة التطبيق ───
    virtual int  appCreate(const std::string& title, int width, int height) = 0;
    virtual bool appIsRunning(int appId) = 0;
    virtual void appUpdate(int appId) = 0;
    virtual void appSetContent(int appId, WidgetId rootWidget) = 0;
    virtual void appSetBgColor(int appId, int r, int g, int b) = 0;
    virtual void appClose(int appId) = 0;
    virtual void appSetTitle(int appId, const std::string& title) = 0;
    virtual int  appGetWidth(int appId) = 0;
    virtual int  appGetHeight(int appId) = 0;
    virtual double appGetFps(int appId) = 0;

    // ─── إنشاء المكونات ───
    virtual WidgetId createWidget(WidgetType type) = 0;

    // ─── تعديل الخصائص ───
    virtual void setText(WidgetId id, const std::string& text) = 0;
    virtual void setColor(WidgetId id, const std::string& prop, int r, int g, int b, int a = 255) = 0;
    virtual void setFloat(WidgetId id, const std::string& prop, float value) = 0;
    virtual void setInt(WidgetId id, const std::string& prop, int value) = 0;
    virtual void setBool(WidgetId id, const std::string& prop, bool value) = 0;
    virtual void setString(WidgetId id, const std::string& prop, const std::string& value) = 0;

    // ─── قراءة الخصائص ───
    virtual std::string getText(WidgetId id) = 0;
    virtual float       getFloat(WidgetId id, const std::string& prop) = 0;
    virtual bool        getBool(WidgetId id, const std::string& prop) = 0;

    // ─── شجرة المكونات ───
    virtual void addChild(WidgetId parent, WidgetId child) = 0;
    virtual void removeChild(WidgetId parent, WidgetId child) = 0;
    virtual void clearChildren(WidgetId parent) = 0;
    virtual int  getParent(WidgetId id) = 0;

    // ─── التخطيط ───
    virtual void setPadding(WidgetId id, float top, float right, float bottom, float left) = 0;
    virtual void setMargin(WidgetId id, float top, float right, float bottom, float left) = 0;
    virtual void setAlignment(WidgetId id, const std::string& main, const std::string& cross) = 0;
    virtual void setSpacing(WidgetId id, float spacing) = 0;
    virtual void setFlex(WidgetId id, float flex) = 0;

    // ─── الحدث ───
    virtual bool wasClicked(WidgetId id) = 0;
    virtual std::string getInputText(WidgetId id) = 0;

    // ─── أدوات مُركّبة (Convenience) ───
    virtual WidgetId createButton(const std::string& text);
    virtual WidgetId createText(const std::string& text);
    virtual WidgetId createColumn();
    virtual WidgetId createRow();
    virtual WidgetId createCard();
    virtual WidgetId createTextField(const std::string& hint);
    virtual WidgetId createHeading(const std::string& text);
    virtual WidgetId createScaffold(const std::string& title, WidgetId body, WidgetId bottomBar = -1);

    // ─── اسم Backend ───
    virtual std::string backendName() const = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  مدير الواجهة الموحدة — Singleton يحمل Backend الفعّال
//  Unified UI Manager — Singleton holding the active backend
// ═══════════════════════════════════════════════════════════════════════════════

class SadUI {
public:
    /// الحصول على المثيل الوحيد
    static SadUI& instance();

    /// تعيين الـ Backend الفعال (SDL2 / Android / WASM)
    /// يتم استدعاؤها مرة واحدة عند بدء البرنامج
    void setBackend(std::unique_ptr<SadUIBackend> backend);

    /// الحصول على Backend الفعال
    SadUIBackend* backend();

    /// هل تم تهيئة الواجهة؟
    bool isInitialized() const;

private:
    SadUI() = default;
    std::unique_ptr<SadUIBackend> m_backend;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مختصرة عامة — يستدعيها المفسر مباشرة
//  Shorthand global functions — called directly by the interpreter
// ═══════════════════════════════════════════════════════════════════════════════

// التطبيق
int    ui_app_create(const std::string& title, int w = 800, int h = 600);
bool   ui_app_running(int appId);
void   ui_app_update(int appId);
void   ui_app_set_content(int appId, WidgetId root);
void   ui_app_set_bg(int appId, int r, int g, int b);
void   ui_app_close(int appId);

// إنشاء المكونات
WidgetId ui_create(WidgetType type);
WidgetId ui_button(const std::string& text);
WidgetId ui_text(const std::string& text);
WidgetId ui_column();
WidgetId ui_row();
WidgetId ui_card();
WidgetId ui_text_field(const std::string& hint);
WidgetId ui_heading(const std::string& text);
WidgetId ui_scaffold(const std::string& title, WidgetId body, WidgetId bottomBar = -1);

// تعديل
void   ui_set_text(WidgetId id, const std::string& text);
void   ui_add_child(WidgetId parent, WidgetId child);
void   ui_set_padding(WidgetId id, float all);
void   ui_set_padding4(WidgetId id, float t, float r, float b, float l);
void   ui_set_spacing(WidgetId id, float s);
void   ui_set_alignment(WidgetId id, const std::string& main, const std::string& cross);
void   ui_set_flex(WidgetId id, float f);
void   ui_set_bg_color(WidgetId id, int r, int g, int b);
void   ui_set_text_color(WidgetId id, int r, int g, int b);
void   ui_set_font_size(WidgetId id, float size);
void   ui_set_border_radius(WidgetId id, float radius);
void   ui_set_visible(WidgetId id, bool v);
void   ui_set_enabled(WidgetId id, bool e);

// قراءة
std::string ui_get_text(WidgetId id);
bool        ui_was_clicked(WidgetId id);
std::string ui_get_input(WidgetId id);

}} // namespace sad::unified
