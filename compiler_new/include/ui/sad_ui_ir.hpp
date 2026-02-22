/**
 * ==========================================================================
 * ملف: sad_ui_ir.hpp
 * الوصف: التمثيل الوسيط الموحد لواجهات لغة ص — SadUI IR
 * الإصدار: 1.0.0
 * ==========================================================================
 *
 * بسم الله الرحمن الرحيم
 *
 * هذا الملف يُعرّف SadUI IR — التمثيل الوسيط الموحد الذي يجلس بين
 * محلل الواجهات (UIParser) ومولدات الكود الأصلي (Compose/SwiftUI/SDL2).
 *
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║                     خط أنابيب SadUI IR                                    ║
 * ╠══════════════════════════════════════════════════════════════════════════╣
 * ║                                                                          ║
 * ║   .ص مصدر → UIParser → UINode AST → SadUI IR Builder → SadUI IR        ║
 * ║                                                       ↓                 ║
 * ║                                               SadUI IR Validator        ║
 * ║                                                       ↓                 ║
 * ║                                               SadUI IR Optimizer        ║
 * ║                                                       ↓                 ║
 * ║                                    ┌──────────┬──────────┬──────────┐   ║
 * ║                                    ↓          ↓          ↓          ↓   ║
 * ║                                 Desktop   Android     iOS       Web     ║
 * ║                                (SDL2)   (Compose)  (SwiftUI)  (HTML)   ║
 * ║                                    ↓          ↓          ↓          ↓   ║
 * ║                                  .exe       .apk       .ipa      .html  ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 *
 * الفرق عن UINode:
 * - UINode = تمثيل نحوي خام (ناتج المحلل مباشرة)
 * - SadUI IR = تمثيل دلالي موحد (أنماط محلولة، حالة مربوطة، أحداث موصولة)
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#ifndef SAD_UI_IR_HPP
#define SAD_UI_IR_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include <unordered_map>
#include <functional>
#include <cstdint>

namespace sad {
namespace ui {
namespace ir {

// ═══════════════════════════════════════════════════════════════════════════
// القسم 1: الأنواع الأساسية
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief المنصة المستهدفة
 */
enum class TargetPlatform {
    Desktop,        ///< سطح المكتب (Windows/Linux/macOS) — SDL2
    Android,        ///< أندرويد — Jetpack Compose
    iOS,            ///< آيفون/آيباد — SwiftUI
    Web,            ///< الويب — HTML/CSS/JS
    All             ///< جميع المنصات
};

/**
 * @brief لون موحد (RGBA)
 */
struct Color {
    uint8_t r = 0, g = 0, b = 0;
    float a = 1.0f;

    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b, float a = 1.0f)
        : r(r), g(g), b(b), a(a) {}

    /// إنشاء من قيمة سداسية "#RRGGBB" أو "#RRGGBBAA"
    static Color fromHex(const std::string& hex);

    /// تحويل إلى نص سداسي
    std::string toHex() const;

    /// ألوان مسماة شائعة
    static Color red()    { return {255, 0, 0}; }
    static Color green()  { return {0, 128, 0}; }
    static Color blue()   { return {0, 0, 255}; }
    static Color white()  { return {255, 255, 255}; }
    static Color black()  { return {0, 0, 0}; }
    static Color transparent() { return {0, 0, 0, 0.0f}; }

    bool operator==(const Color& o) const {
        return r == o.r && g == o.g && b == o.b && a == o.a;
    }
};

/**
 * @brief وحدة البُعد
 */
enum class DimensionUnit {
    Points,     ///< نقاط (بكسل * كثافة)
    Percent,    ///< نسبة مئوية من الأب
    Fill,       ///< مالئ لكل المساحة المتاحة
    Fit,        ///< ملائم للمحتوى
    Auto        ///< تلقائي
};

/**
 * @brief قيمة بُعدية موحدة
 */
struct Dimension {
    DimensionUnit unit = DimensionUnit::Auto;
    float value = 0.0f;

    Dimension() = default;
    Dimension(DimensionUnit u, float v) : unit(u), value(v) {}

    static Dimension points(float v) { return {DimensionUnit::Points, v}; }
    static Dimension percent(float v) { return {DimensionUnit::Percent, v}; }
    static Dimension fill() { return {DimensionUnit::Fill, 0}; }
    static Dimension fit() { return {DimensionUnit::Fit, 0}; }
    static Dimension autoSize() { return {DimensionUnit::Auto, 0}; }

    bool isAuto() const { return unit == DimensionUnit::Auto; }
    bool isFill() const { return unit == DimensionUnit::Fill; }
};

/**
 * @brief حشوة/هامش (أربعة اتجاهات)
 *
 * نستخدم leading/trailing بدل left/right لدعم RTL أصلي
 */
struct EdgeInsets {
    float top = 0;
    float leading = 0;     ///< بداية (يمين في RTL، يسار في LTR)
    float bottom = 0;
    float trailing = 0;    ///< نهاية (يسار في RTL، يمين في LTR)

    EdgeInsets() = default;
    EdgeInsets(float all) : top(all), leading(all), bottom(all), trailing(all) {}
    EdgeInsets(float t, float l, float b, float tr) : top(t), leading(l), bottom(b), trailing(tr) {}

    static EdgeInsets all(float v) { return {v}; }
    static EdgeInsets symmetric(float h, float v) { return {v, h, v, h}; }
    static EdgeInsets zero() { return {}; }
    bool isZero() const { return top == 0 && leading == 0 && bottom == 0 && trailing == 0; }
};

/**
 * @brief محاذاة أفقية
 */
enum class HAlignment {
    Leading,    ///< بداية (RTL-aware)
    Center,     ///< وسط
    Trailing    ///< نهاية (RTL-aware)
};

/**
 * @brief محاذاة رأسية
 */
enum class VAlignment {
    Top,        ///< أعلى
    Center,     ///< وسط
    Bottom      ///< أسفل
};

/**
 * @brief محاذاة ثنائية الأبعاد
 */
struct Alignment {
    HAlignment horizontal = HAlignment::Center;
    VAlignment vertical = VAlignment::Center;

    static Alignment topLeading()    { return {HAlignment::Leading, VAlignment::Top}; }
    static Alignment topCenter()     { return {HAlignment::Center, VAlignment::Top}; }
    static Alignment topTrailing()   { return {HAlignment::Trailing, VAlignment::Top}; }
    static Alignment centerLeading() { return {HAlignment::Leading, VAlignment::Center}; }
    static Alignment center()        { return {HAlignment::Center, VAlignment::Center}; }
    static Alignment centerTrailing(){ return {HAlignment::Trailing, VAlignment::Center}; }
    static Alignment bottomLeading() { return {HAlignment::Leading, VAlignment::Bottom}; }
    static Alignment bottomCenter()  { return {HAlignment::Center, VAlignment::Bottom}; }
    static Alignment bottomTrailing(){ return {HAlignment::Trailing, VAlignment::Bottom}; }
};

/**
 * @brief سمك الخط
 */
enum class FontWeight {
    Thin,
    Light,
    Regular,
    Medium,
    SemiBold,
    Bold,
    ExtraBold,
    Black
};

/**
 * @brief ظل
 */
struct Shadow {
    Color color = Color(0, 0, 0, 0.25f);
    float offsetX = 0;
    float offsetY = 2;
    float blur = 4;
};

// ═══════════════════════════════════════════════════════════════════════════
// القسم 2: أنواع المكونات
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief نوع المكون في SadUI IR
 *
 * هذا التعداد يغطي جميع مكونات الواجهة المدعومة.
 * كل نوع له ترجمة محددة لكل منصة مستهدفة.
 */
enum class WidgetType {
    // === العرض الأساسي ===
    Text,               ///< نص — عنصر نصي
    Image,              ///< صورة — عنصر صورة
    Icon,               ///< أيقونة — أيقونة

    // === الإدخال ===
    Button,             ///< زر — زر عادي
    IconButton,         ///< زر_أيقونة — زر بأيقونة
    FloatingButton,     ///< زر_عائم — زر عائم (FAB)
    OutlineButton,      ///< زر_محدد — زر بحد خارجي
    TextButton,         ///< زر_نصي — زر نصي فقط
    TextField,          ///< حقل_نص — حقل إدخال
    TextArea,           ///< منطقة_نص — منطقة نص متعددة
    PasswordField,      ///< حقل_كلمة_مرور — حقل كلمة مرور
    Checkbox,           ///< مربع_اختيار — مربع اختيار
    Radio,              ///< زر_راديو — زر اختيار واحد
    Toggle,             ///< مفتاح — مفتاح تشغيل/إيقاف
    Slider,             ///< منزلق — شريط قيم
    RangeSlider,        ///< منزلق_مدى — منزلق بمقبضين
    Dropdown,           ///< قائمة_منسدلة — قائمة خيارات
    DatePicker,         ///< منتقي_تاريخ — منتقي تاريخ
    ColorPicker,        ///< منتقي_لون — منتقي لون
    SearchField,        ///< حقل_بحث — حقل بحث
    NumberInput,        ///< إدخال_رقم — إدخال رقمي
    RatingStars,        ///< نجوم_تقييم — تقييم بالنجوم

    // === التخطيط ===
    Column,             ///< عمود — ترتيب رأسي
    Row,                ///< صف — ترتيب أفقي
    Stack,              ///< رصة — تراكب
    Grid,               ///< شبكة — شبكة
    LazyColumn,         ///< عمود_كسول — قائمة رأسية كسولة
    LazyRow,            ///< صف_كسول — قائمة أفقية كسولة
    LazyGrid,           ///< شبكة_كسولة — شبكة كسولة
    Spacer,             ///< فاصل — مساحة فارغة
    Divider,            ///< فاصل_خط — خط فاصل
    Expanded,           ///< موسع — عنصر يملأ المتبقي
    SizedBox,           ///< صندوق_محدد — صندوق بحجم ثابت
    Center,             ///< وسط — توسيط المحتوى
    Wrap,               ///< لف — لف العناصر

    // === الحاويات ===
    Container,          ///< حاوية — حاوية عامة
    Card,               ///< بطاقة — بطاقة مرتفعة
    Surface,            ///< سطح — سطح Material
    Box,                ///< صندوق — صندوق عام
    ScrollView,         ///< عرض_تمرير — عرض قابل للتمرير
    PageView,           ///< عرض_صفحات — عرض صفحات

    // === الهيكل ===
    Scaffold,           ///< سقالة/هيكل — هيكل الشاشة الرئيسي
    AppBar,             ///< شريط_تطبيق — شريط التطبيق العلوي
    BottomBar,          ///< شريط_سفلي — شريط التنقل السفلي
    TabBar,             ///< شريط_أقسام — تبويبات
    Drawer,             ///< درج — قائمة جانبية
    BottomSheet,        ///< ورقة_سفلية — ورقة منبثقة من الأسفل

    // === التنقل ===
    NavigationView,     ///< عرض_تنقل — حاوية التنقل
    NavigationLink,     ///< رابط_تنقل — رابط للانتقال
    TabView,            ///< عرض_أقسام — تبويبات

    // === القوائم ===
    List,               ///< قائمة — قائمة عناصر
    ListTile,           ///< عنصر_قائمة — عنصر في قائمة مع عنوان فرعي
    Section,            ///< قسم — قسم في قائمة
    ForEach,            ///< لكل — تكرار

    // === الملاحظات والحوارات ===
    Dialog,             ///< حوار — نافذة حوار
    Alert,              ///< تنبيه — تنبيه
    Toast,              ///< رسالة — رسالة عابرة
    Snackbar,           ///< شريط_رسالة — شريط رسالة
    Modal,              ///< نافذة_مشروطة — نافذة مشروطة
    Tooltip,            ///< تلميح — تلميح

    // === البيانات ===
    ProgressBar,        ///< شريط_تقدم — شريط تقدم خطي
    CircleProgress,     ///< تقدم_دائري — تقدم دائري
    LoadingSpinner,     ///< مؤشر_تحميل — مؤشر تحميل
    Badge,              ///< شارة — شارة رقمية
    Chip,               ///< رقاقة — رقاقة خيار
    Avatar,             ///< صورة_شخصية — صورة رمزية
    Stepper,            ///< خطوات — مؤشر خطوات

    // === الرسوم البيانية ===
    BarChart,           ///< رسم_أعمدة — رسم بياني أعمدة
    LineChart,          ///< رسم_خطي — رسم بياني خطي
    PieChart,           ///< رسم_دائري — رسم بياني دائري

    // === متقدم ===
    Canvas,             ///< لوحة_رسم — لوحة رسم مخصصة
    WebView,            ///< عرض_ويب — عارض ويب
    MapView,            ///< عرض_خريطة — عارض خرائط
    VideoPlayer,        ///< مشغل_فيديو — مشغل فيديو

    // === خاص ===
    CustomWidget,       ///< مكون_مخصص — مكون مخصص
    Conditional,        ///< شرطي — عرض شرطي
    Group               ///< مجموعة — مجموعة عناصر بدون حاوية
};

/**
 * @brief الحصول على الاسم العربي لنوع المكون
 */
std::string widgetTypeToArabic(WidgetType type);

/**
 * @brief تحويل اسم عربي إلى نوع مكون
 */
std::optional<WidgetType> arabicToWidgetType(const std::string& name);

/**
 * @brief هل هذا المكون حاوية (يقبل أبناء)؟
 */
bool isContainerWidget(WidgetType type);


// ═══════════════════════════════════════════════════════════════════════════
// القسم 3: نظام الأنماط الموحد — StyleIR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief الأنماط الموحدة لمكون UI
 *
 * هذا الهيكل يجمع كل الأنماط البصرية بشكل موحد.
 * كل خاصية اختيارية (optional) — فقط الخصائص المعيّنة تُترجم.
 */
struct StyleIR {
    // === الألوان ===
    std::optional<Color> foregroundColor;   ///< لون النص/المقدمة
    std::optional<Color> backgroundColor;   ///< لون الخلفية
    std::optional<Color> tintColor;         ///< لون الصبغة
    std::optional<Color> borderColor;       ///< لون الحد

    // === الخطوط ===
    std::optional<float> fontSize;          ///< حجم الخط
    std::optional<FontWeight> fontWeight;   ///< سمك الخط
    std::optional<std::string> fontFamily;  ///< عائلة الخط
    std::optional<float> lineSpacing;       ///< تباعد الأسطر

    // === الأبعاد ===
    Dimension width;                        ///< العرض (افتراضي: Auto)
    Dimension height;                       ///< الارتفاع (افتراضي: Auto)
    std::optional<float> minWidth;
    std::optional<float> minHeight;
    std::optional<float> maxWidth;
    std::optional<float> maxHeight;

    // === التباعد ===
    EdgeInsets padding;                     ///< حشوة داخلية
    EdgeInsets margin;                      ///< هامش خارجي
    float spacing = 0;                      ///< تباعد بين الأبناء (للحاويات)

    // === الحدود ===
    float borderRadius = 0;                 ///< انحناء الزوايا
    float borderWidth = 0;                  ///< سمك الحد

    // === المحاذاة ===
    std::optional<HAlignment> horizontalAlignment;
    std::optional<VAlignment> verticalAlignment;
    std::optional<Alignment> contentAlignment; ///< محاذاة المحتوى داخل الحاوية

    // === التأثيرات البصرية ===
    std::optional<Shadow> shadow;           ///< الظل
    float opacity = 1.0f;                   ///< العتامة (0-1)
    float rotation = 0.0f;                  ///< الدوران (بالدرجات)
    float scale = 1.0f;                     ///< المقياس

    // === الحالة ===
    bool hidden = false;                    ///< مخفي؟
    bool disabled = false;                  ///< معطل؟

    // === RTL ===
    bool isRTL = true;                      ///< اتجاه من اليمين لليسار (افتراضي: عربي)

    /// هل هناك أي نمط معيّن؟
    bool hasAnyStyle() const;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم 4: نظام الحالة — StateBindingIR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief نوع ربط الحالة
 */
enum class StateBindingKind {
    Signal,             ///< إشارة — حالة تفاعلية محلية
                        ///< Compose: mutableStateOf / SwiftUI: @State / SDL2: sad::state
    Computed,           ///< محسوب — قيمة مشتقة من حالات أخرى
                        ///< Compose: derivedStateOf / SwiftUI: computed / SDL2: observer
    Binding,            ///< ربط — ربط ثنائي الاتجاه
                        ///< Compose: MutableState param / SwiftUI: @Binding
    Observable,         ///< مراقب — كائن مُراقب
                        ///< Compose: ViewModel / SwiftUI: @ObservedObject
    Environment,        ///< بيئة — قيمة من السياق
                        ///< Compose: LocalComposition / SwiftUI: @Environment
    AppStorage          ///< تخزين — حالة دائمة (SharedPreferences/UserDefaults)
                        ///< Compose: DataStore / SwiftUI: @AppStorage / SDL2: sad::android::save
};

/**
 * @brief نوع بيانات الحالة
 */
enum class StateValueType {
    Number,         ///< رقم
    Text,           ///< نص
    Boolean,        ///< منطقي
    NumberList,     ///< قائمة أرقام
    TextList,       ///< قائمة نصوص
    ObjectList,     ///< قائمة كائنات
    Map,            ///< خريطة
    Custom          ///< نوع مخصص
};

/**
 * @brief ربط حالة في SadUI IR
 */
struct StateBindingIR {
    StateBindingKind kind;              ///< نوع الربط
    std::string name;                   ///< اسم المتغير (عربي)
    StateValueType valueType;           ///< نوع القيمة
    std::string typeName;               ///< اسم النوع (كنص)
    std::string initialValue;           ///< التعبير الأولي
    std::optional<std::string> computeExpression; ///< تعبير الحساب (للمحسوب)
    std::vector<std::string> dependencies;        ///< الاعتماديات (للمحسوب)
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم 5: نظام الأحداث — EventHandlerIR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief نوع الحدث
 */
enum class EventType {
    Tap,            ///< عند_النقر — نقرة على المكون
    DoubleTap,      ///< عند_النقر_المزدوج
    LongPress,      ///< عند_الضغط_المطول
    TextChange,     ///< عند_تغير_النص — تغير نص حقل إدخال
    ValueChange,    ///< عند_تغير_القيمة — تغير قيمة (منزلق، مفتاح)
    ToggleChange,   ///< عند_التبديل — تغيير حالة التبديل
    Submit,         ///< عند_الإرسال — إرسال نموذج
    SelectionChange,///< عند_تغيير_الاختيار — تغيير في قائمة منسدلة
    Swipe,          ///< عند_السحب — سحب المكون
    Appear,         ///< عند_الظهور — ظهور المكون على الشاشة
    Disappear,      ///< عند_الاختفاء — اختفاء المكون
    Scroll,         ///< عند_التمرير — تمرير
    DragStart,      ///< عند_بدء_السحب
    DragEnd,        ///< عند_انتهاء_السحب
    Focus,          ///< عند_التركيز — تركيز المؤشر
    Blur            ///< عند_فقد_التركيز — فقدان التركيز
};

/**
 * @brief معالج حدث في SadUI IR
 */
struct EventHandlerIR {
    EventType type;                     ///< نوع الحدث
    std::string handlerName;            ///< اسم الدالة المعالجة
    std::string closureBody;            ///< جسم الإغلاق (inline handler)
    std::vector<std::string> parameters; ///< معاملات المعالج
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم 6: نظام التنقل — NavigationIR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief نوع إجراء التنقل
 */
enum class NavigationActionType {
    Push,           ///< انتقل_إلى — إضافة شاشة جديدة
    Pop,            ///< ارجع — العودة للخلف
    Replace,        ///< استبدل — استبدال الشاشة الحالية
    PopToRoot,      ///< ارجع_للبداية — العودة للشاشة الأولى
    ShowDialog,     ///< أظهر_حوار — عرض نافذة حوار
    ShowSheet,      ///< أظهر_ورقة — عرض ورقة منبثقة
    ShowToast,      ///< أظهر_رسالة — عرض رسالة عابرة
    SwitchTab       ///< غيّر_القسم — تغيير التبويب
};

/**
 * @brief إجراء تنقل
 */
struct NavigationActionIR {
    NavigationActionType type;
    std::string targetRoute;            ///< المسار المستهدف
    std::unordered_map<std::string, std::string> arguments; ///< الوسائط
    std::string transitionType;         ///< نوع الانتقال (اختياري)
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم 7: نظام الرسوم المتحركة — AnimationIR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief نوع الرسوم المتحركة
 */
enum class AnimationType {
    None,           ///< بدون حركة
    Fade,           ///< تلاشي
    Slide,          ///< انزلاق
    Scale,          ///< تكبير/تصغير
    Rotation,       ///< دوران
    Spring,         ///< زنبرك (ارتداد)
    EaseIn,         ///< تسارع
    EaseOut,        ///< تباطؤ
    EaseInOut,      ///< تسارع ثم تباطؤ
    Linear          ///< خطي
};

/**
 * @brief وصف رسوم متحركة
 */
struct AnimationIR {
    AnimationType type = AnimationType::None;
    float duration = 300.0f;            ///< المدة بالمللي ثانية
    float delay = 0.0f;                 ///< التأخير
    bool repeats = false;               ///< هل يتكرر؟
    bool autoReverse = false;           ///< هل يعكس تلقائيًا؟
    std::string property;               ///< الخاصية المستهدفة
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم 8: عقدة الواجهة الموحدة — UINodeIR (الهيكل الجوهري)
// ═══════════════════════════════════════════════════════════════════════════

// تصريح مُقدّم
struct UINodeIR;
using UINodeIRPtr = std::shared_ptr<UINodeIR>;

/**
 * @brief قيمة خاصية في IR
 */
using IRPropertyValue = std::variant<
    std::monostate,     ///< لا قيمة
    bool,               ///< منطقي
    int64_t,            ///< عدد صحيح
    double,             ///< عدد عشري
    std::string,        ///< نص
    Color,              ///< لون
    Dimension,          ///< بُعد
    Alignment           ///< محاذاة
>;

/**
 * @brief عقدة واجهة مستخدم في التمثيل الوسيط الموحد
 *
 * هذه العقدة هي الوحدة الأساسية في SadUI IR.
 * تحتوي على كل المعلومات اللازمة لتوليد كود أصلي لأي منصة.
 */
struct UINodeIR {
    // ═══════════════════════════════════════════
    // الهوية
    // ═══════════════════════════════════════════

    int id = 0;                                 ///< معرّف فريد
    WidgetType type;                            ///< نوع المكون
    std::string arabicName;                     ///< الاسم العربي الأصلي
    std::optional<std::string> tag;             ///< علامة تعريفية (للربط)

    // ═══════════════════════════════════════════
    // الخصائص المحددة للنوع
    // ═══════════════════════════════════════════

    /// وسائط البناء (مثل نص الزر، تلميح الحقل)
    std::vector<IRPropertyValue> constructorArgs;

    /// خصائص مسماة (مثل أعمدة: 3)
    std::unordered_map<std::string, IRPropertyValue> properties;

    // ═══════════════════════════════════════════
    // الأنماط
    // ═══════════════════════════════════════════

    StyleIR style;                              ///< الأنماط البصرية الموحدة

    // ═══════════════════════════════════════════
    // البنية الشجرية
    // ═══════════════════════════════════════════

    std::vector<UINodeIRPtr> children;          ///< الأبناء

    // ═══════════════════════════════════════════
    // التفاعل
    // ═══════════════════════════════════════════

    std::vector<EventHandlerIR> eventHandlers;  ///< معالجات الأحداث
    std::optional<StateBindingIR> stateBinding; ///< ربط بحالة (لعناصر الإدخال)

    // ═══════════════════════════════════════════
    // التنقل
    // ═══════════════════════════════════════════

    std::optional<NavigationActionIR> navAction; ///< إجراء تنقل

    // ═══════════════════════════════════════════
    // الرسوم المتحركة
    // ═══════════════════════════════════════════

    std::vector<AnimationIR> animations;        ///< رسوم متحركة

    // ═══════════════════════════════════════════
    // التشخيص
    // ═══════════════════════════════════════════

    size_t sourceLine = 0;                      ///< سطر المصدر
    size_t sourceColumn = 0;                    ///< عمود المصدر

    // ═══════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════

    /// هل هذا المكون حاوية؟
    bool isContainer() const { return ir::isContainerWidget(type); }

    /// هل له أبناء؟
    bool hasChildren() const { return !children.empty(); }

    /// هل له أحداث؟
    bool hasEvents() const { return !eventHandlers.empty(); }

    /// هل له رسوم متحركة؟
    bool hasAnimations() const { return !animations.empty(); }

    /// عدد العقد في الشجرة الفرعية (شاملة هذه العقدة)
    int subtreeSize() const;

    /// البحث عن عقدة بالعلامة
    UINodeIRPtr findByTag(const std::string& tag) const;

    /// طباعة الشجرة للتشخيص
    std::string dump(int indent = 0) const;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم 9: تعريف الشاشة/الواجهة — ScreenIR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تعريف شاشة/واجهة كاملة في SadUI IR
 *
 * يمثل واجهة مستخدم كاملة مع:
 * - اسم الشاشة
 * - ربط الحالة
 * - شجرة المكونات
 * - الدوال المعالجة
 */
struct ScreenIR {
    /// اسم الشاشة/الواجهة
    std::string name;

    /// اسم المسار (للتنقل)
    std::string routeName;

    /// متغيرات الحالة
    std::vector<StateBindingIR> stateBindings;

    /// شجرة المكونات (الجذر)
    UINodeIRPtr rootNode;

    /// الدوال المعالجة (اسم → جسم الدالة كنص)
    std::unordered_map<std::string, std::string> handlerFunctions;

    /// مصدر الملف
    std::string sourceFile;
    size_t startLine = 0;
    size_t endLine = 0;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم 10: نظام الثيمات — ThemeIR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief ثيمة موحدة تُترجم لكل منصة
 */
struct ThemeIR {
    // === الألوان الأساسية ===
    Color primaryColor      = Color(25, 118, 210);     ///< أساسي
    Color secondaryColor    = Color(255, 152, 0);      ///< ثانوي
    Color backgroundColor   = Color(250, 250, 250);    ///< خلفية
    Color surfaceColor      = Color(255, 255, 255);    ///< سطح/بطاقات
    Color errorColor        = Color(244, 67, 54);      ///< خطأ
    Color successColor      = Color(76, 175, 80);      ///< نجاح
    Color warningColor      = Color(255, 193, 7);      ///< تحذير

    // === ألوان النص ===
    Color textPrimary       = Color(33, 33, 33);       ///< نص أساسي
    Color textSecondary     = Color(117, 117, 117);    ///< نص ثانوي
    Color textOnPrimary     = Color(255, 255, 255);    ///< نص على اللون الأساسي

    // === الخطوط ===
    float fontSizeCaption   = 12.0f;
    float fontSizeBody      = 16.0f;
    float fontSizeTitle     = 20.0f;
    float fontSizeHeadline  = 24.0f;
    float fontSizeDisplay   = 32.0f;

    // === التباعد ===
    float spacingXS = 4.0f;
    float spacingSM = 8.0f;
    float spacingMD = 16.0f;
    float spacingLG = 24.0f;
    float spacingXL = 32.0f;

    // === الشكل ===
    float borderRadiusSM = 4.0f;
    float borderRadiusMD = 8.0f;
    float borderRadiusLG = 16.0f;
    float elevation = 4.0f;

    // === الاتجاه ===
    bool isRTL = true;                  ///< اتجاه عربي (يمين لليسار)

    // === الوضع ===
    bool isDarkMode = false;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم 11: تعريف التطبيق — AppIR
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تعريف تطبيق كامل في SadUI IR
 *
 * هذا هو الهيكل الأعلى مستوى — يمثل تطبيقًا كاملاً
 * جاهزًا للترجمة لأي منصة.
 */
struct AppIR {
    // === معلومات التطبيق ===
    std::string appName;                            ///< اسم التطبيق
    std::string packageName;                        ///< اسم الحزمة (com.example.app)
    std::string version = "1.0.0";                  ///< الإصدار
    std::string description;                        ///< الوصف

    // === الثيمة ===
    ThemeIR theme;                                  ///< ثيمة التطبيق

    // === الشاشات ===
    std::vector<ScreenIR> screens;                  ///< جميع الشاشات
    std::string initialScreenName;                  ///< الشاشة الأولى

    // === التنقل ===
    struct Route {
        std::string name;                           ///< اسم المسار
        std::string screenName;                     ///< اسم الشاشة
        std::string transitionType;                 ///< نوع الانتقال
    };
    std::vector<Route> routes;                      ///< جدول المسارات

    // === المنصة ===
    TargetPlatform targetPlatform = TargetPlatform::All;

    // === إعدادات أندرويد ===
    struct AndroidConfig {
        int minSdk = 24;
        int targetSdk = 34;
        std::vector<std::string> architectures = {"arm64-v8a", "x86_64"};
        std::vector<std::string> permissions;
    };
    std::optional<AndroidConfig> androidConfig;

    // === إعدادات iOS ===
    struct IOSConfig {
        std::string minIOSVersion = "16.0";
        std::string teamId;
    };
    std::optional<IOSConfig> iosConfig;

    // === إعدادات سطح المكتب ===
    struct DesktopConfig {
        int windowWidth = 400;
        int windowHeight = 700;
        std::string iconPath;
    };
    std::optional<DesktopConfig> desktopConfig;

    // === الحالة العامة ===
    std::vector<StateBindingIR> globalState;         ///< حالة عامة للتطبيق

    // === ملف المصدر ===
    std::string sourceFile;
};


} // namespace ir
} // namespace ui
} // namespace sad

#endif // SAD_UI_IR_HPP
