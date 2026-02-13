/**
 * ==========================================================================
 * ملف: ui_parser.hpp
 * الوصف: محلل واجهة المستخدم التصريحية للغة ص
 * الإصدار: 1.0.0
 * ==========================================================================
 * 
 * هذا الملف يحتوي على نظام تحليل واجهة المستخدم التصريحية (Declarative UI)
 * المستوحى من SwiftUI و Jetpack Compose ولكن بالعربية الكاملة.
 * 
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║                     هيكل واجهة المستخدم في لغة ص                          ║
 * ╠══════════════════════════════════════════════════════════════════════════╣
 * ║                                                                          ║
 * ║  واجهة شاشة_رئيسية {                                                     ║
 * ║      @حالة عداد: عدد = ٠                                                 ║
 * ║                                                                          ║
 * ║      عرض() {                                                             ║
 * ║          عمود(توسيط: صحيح) {                                             ║
 * ║              نص("العداد: \(عداد)")                                        ║
 * ║                  .حجم_خط(٢٤)                                              ║
 * ║                  .لون(.أزرق)                                              ║
 * ║                                                                          ║
 * ║              زر("+١") { عداد += ١ }                                       ║
 * ║                  .نمط(.مملوء)                                             ║
 * ║          }                                                               ║
 * ║      }                                                                   ║
 * ║  }                                                                       ║
 * ║                                                                          ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 * 
 * المميزات الرئيسية:
 * ----------------
 * 1. صيغة تصريحية عربية بالكامل
 * 2. دعم ربط الحالة (@حالة، @مراقب، @بيئة)
 * 3. مكونات أساسية (نص، زر، صورة، قائمة، شبكة، إلخ)
 * 4. معدّلات سلسلية (.لون، .حجم، .حشوة، إلخ)
 * 5. تنقل تصريحي
 * 6. تخطيطات متجاوبة
 * 7. دعم الرسوم المتحركة
 * 
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#ifndef SAD_UI_PARSER_HPP
#define SAD_UI_PARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <variant>
#include <unordered_map>
#include <functional>

namespace sad {
namespace parser {

// ═══════════════════════════════════════════════════════════════════════════
// القسم الأول: أنواع عقد الواجهة (UI Node Types)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief أنواع عقد الواجهة الأساسية
 * 
 * هذا التعداد يمثل جميع أنواع مكونات الواجهة المدعومة
 * في نظام واجهة لغة ص.
 * 
 * @note كل نوع يقابل مكوناً في كل من Android (Compose) و iOS (SwiftUI)
 */
enum class UINodeType {
    // ═══════════════════════════════════════════
    // مكونات العرض الأساسية
    // ═══════════════════════════════════════════
    
    Text,               ///< نص - عنصر نصي بسيط
    Image,              ///< صورة - عنصر صورة
    Icon,               ///< أيقونة - أيقونة متجهة
    
    // ═══════════════════════════════════════════
    // مكونات الإدخال
    // ═══════════════════════════════════════════
    
    Button,             ///< زر - زر قابل للنقر
    TextField,          ///< حقل_نص - حقل إدخال نصي
    TextArea,           ///< منطقة_نص - منطقة نص متعددة الأسطر
    Toggle,             ///< تبديل - مفتاح تشغيل/إيقاف
    Slider,             ///< منزلق - شريط قيم
    Picker,             ///< منتقي - قائمة منسدلة
    DatePicker,         ///< منتقي_تاريخ - منتقي تاريخ
    ColorPicker,        ///< منتقي_لون - منتقي لون
    
    // ═══════════════════════════════════════════
    // مكونات التخطيط
    // ═══════════════════════════════════════════
    
    Column,             ///< عمود - ترتيب رأسي
    Row,                ///< صف - ترتيب أفقي
    Stack,              ///< رصة - تراكب فوق بعض
    Grid,               ///< شبكة - تخطيط شبكي
    LazyColumn,         ///< عمود_كسول - قائمة عمودية كسولة
    LazyRow,            ///< صف_كسول - قائمة أفقية كسولة
    LazyGrid,           ///< شبكة_كسولة - شبكة كسولة
    Spacer,             ///< فاصل - مساحة فارغة
    Divider,            ///< فاصل_خط - خط فاصل
    
    // ═══════════════════════════════════════════
    // مكونات الحاويات
    // ═══════════════════════════════════════════
    
    Container,          ///< حاوية - حاوية عامة
    Card,               ///< بطاقة - بطاقة مرتفعة
    Surface,            ///< سطح - سطح Material
    Scaffold,           ///< هيكل - هيكل الشاشة
    Box,                ///< صندوق - صندوق عام
    
    // ═══════════════════════════════════════════
    // مكونات التنقل
    // ═══════════════════════════════════════════
    
    NavigationView,     ///< عرض_تنقل - حاوية التنقل
    NavigationLink,     ///< رابط_تنقل - رابط للانتقال
    TabView,            ///< عرض_أقسام - تبويبات
    TabItem,            ///< قسم - عنصر تبويب واحد
    
    // ═══════════════════════════════════════════
    // مكونات القوائم
    // ═══════════════════════════════════════════
    
    List,               ///< قائمة - قائمة عناصر
    Section,            ///< قسم - قسم في قائمة
    ForEach,            ///< لكل - تكرار على مجموعة
    
    // ═══════════════════════════════════════════
    // مكونات الحوارات
    // ═══════════════════════════════════════════
    
    Alert,              ///< تنبيه - نافذة تنبيه
    Sheet,              ///< ورقة - ورقة منبثقة
    Dialog,             ///< حوار - نافذة حوار
    Menu,               ///< قائمة_خيارات - قائمة خيارات
    
    // ═══════════════════════════════════════════
    // مكونات متقدمة
    // ═══════════════════════════════════════════
    
    Canvas,             ///< لوحة - لوحة رسم
    Map,                ///< خريطة - عنصر خرائط
    WebView,            ///< عرض_ويب - عارض ويب
    VideoPlayer,        ///< مشغل_فيديو - مشغل فيديو
    
    // ═══════════════════════════════════════════
    // عناصر خاصة
    // ═══════════════════════════════════════════
    
    ViewBuilder,        ///< منشئ_واجهة - منشئ واجهة مخصص
    CustomView,         ///< واجهة_مخصصة - واجهة مستخدم مخصصة
    Conditional,        ///< شرطي - عرض شرطي
    Group               ///< مجموعة - مجموعة عناصر
};

/**
 * @brief تحويل نوع العقدة إلى نص عربي
 * @param type نوع العقدة
 * @return الاسم العربي للنوع
 */
std::string uiNodeTypeToArabicName(UINodeType type);

/**
 * @brief تحويل الاسم العربي إلى نوع العقدة
 * @param name الاسم العربي
 * @return نوع العقدة أو nullopt إذا لم يُعرف
 */
std::optional<UINodeType> arabicNameToUINodeType(const std::string& name);


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثاني: أنواع المعدّلات (Modifier Types)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief أنواع معدّلات الواجهة
 * 
 * المعدّلات هي خصائص تُطبق على مكونات الواجهة
 * باستخدام صيغة النقطة المتسلسلة:
 * 
 * @code
 * نص("مرحباً")
 *     .لون(.أحمر)      // ModifierType::ForegroundColor
 *     .حجم_خط(٢٤)      // ModifierType::FontSize
 *     .حشوة(١٦)        // ModifierType::Padding
 * @endcode
 */
enum class ModifierType {
    // ═══════════════════════════════════════════
    // معدّلات الألوان
    // ═══════════════════════════════════════════
    
    ForegroundColor,    ///< لون - لون المقدمة/النص
    BackgroundColor,    ///< لون_خلفية - لون الخلفية
    TintColor,          ///< لون_صبغة - لون الصبغة
    
    // ═══════════════════════════════════════════
    // معدّلات الخط
    // ═══════════════════════════════════════════
    
    FontSize,           ///< حجم_خط - حجم الخط
    FontWeight,         ///< سمك_خط - سمك الخط
    FontFamily,         ///< عائلة_خط - عائلة الخط
    FontStyle,          ///< نمط_خط - نمط الخط
    LineSpacing,        ///< تباعد_أسطر - المسافة بين الأسطر
    
    // ═══════════════════════════════════════════
    // معدّلات الحجم
    // ═══════════════════════════════════════════
    
    Width,              ///< عرض - العرض
    Height,             ///< ارتفاع - الارتفاع
    MinWidth,           ///< أدنى_عرض - الحد الأدنى للعرض
    MinHeight,          ///< أدنى_ارتفاع - الحد الأدنى للارتفاع
    MaxWidth,           ///< أقصى_عرض - الحد الأقصى للعرض
    MaxHeight,          ///< أقصى_ارتفاع - الحد الأقصى للارتفاع
    
    // ═══════════════════════════════════════════
    // معدّلات التباعد
    // ═══════════════════════════════════════════
    
    Padding,            ///< حشوة - حشوة داخلية
    PaddingTop,         ///< حشوة_علوية
    PaddingBottom,      ///< حشوة_سفلية
    PaddingLeading,     ///< حشوة_بداية
    PaddingTrailing,    ///< حشوة_نهاية
    Margin,             ///< هامش - هامش خارجي
    
    // ═══════════════════════════════════════════
    // معدّلات الحدود
    // ═══════════════════════════════════════════
    
    Border,             ///< حد - حد خارجي
    BorderRadius,       ///< انحناء_حد - انحناء الزوايا
    BorderWidth,        ///< سمك_حد
    BorderColor,        ///< لون_حد
    
    // ═══════════════════════════════════════════
    // معدّلات المحاذاة
    // ═══════════════════════════════════════════
    
    Alignment,          ///< محاذاة - المحاذاة
    Frame,              ///< إطار - الإطار
    Position,           ///< موقع - الموقع
    Offset,             ///< إزاحة - الإزاحة
    
    // ═══════════════════════════════════════════
    // معدّلات الظل والعتامة
    // ═══════════════════════════════════════════
    
    Shadow,             ///< ظل - الظل
    Opacity,            ///< عتامة - العتامة
    Blur,               ///< ضبابية - الضبابية
    
    // ═══════════════════════════════════════════
    // معدّلات التحويل
    // ═══════════════════════════════════════════
    
    Rotation,           ///< دوران - الدوران
    Scale,              ///< مقياس - التكبير/التصغير
    
    // ═══════════════════════════════════════════
    // معدّلات التفاعل
    // ═══════════════════════════════════════════
    
    OnTap,              ///< عند_النقر - حدث النقر
    OnLongPress,        ///< عند_الضغط_المطول - حدث الضغط المطول
    OnDrag,             ///< عند_السحب - حدث السحب
    OnAppear,           ///< عند_الظهور - عند ظهور العنصر
    OnDisappear,        ///< عند_الاختفاء - عند اختفاء العنصر
    
    // ═══════════════════════════════════════════
    // معدّلات الحالة
    // ═══════════════════════════════════════════
    
    Disabled,           ///< معطل - تعطيل العنصر
    Hidden,             ///< مخفي - إخفاء العنصر
    
    // ═══════════════════════════════════════════
    // معدّلات الرسوم المتحركة
    // ═══════════════════════════════════════════
    
    Animation,          ///< رسوم_متحركة - إضافة حركة
    Transition,         ///< انتقال - حركة انتقالية
    
    // ═══════════════════════════════════════════
    // معدّلات متنوعة
    // ═══════════════════════════════════════════
    
    ClipShape,          ///< شكل_قص - قص بشكل معين
    ZIndex,             ///< مستوى_ع - ترتيب المستوى
    Accessibility,      ///< إمكانية_وصول - دعم الوصول
    Tag                 ///< علامة - علامة تعريفية
};

/**
 * @brief تحويل نوع المعدّل إلى نص عربي
 */
std::string modifierTypeToArabicName(ModifierType type);

/**
 * @brief تحويل الاسم العربي إلى نوع المعدّل
 */
std::optional<ModifierType> arabicNameToModifierType(const std::string& name);


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثالث: أنواع ربط الحالة (State Binding Types)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief أنواع ربط الحالة
 * 
 * هذه الأنواع تمثل الطرق المختلفة لربط البيانات
 * مع واجهة المستخدم:
 * 
 * @code
 * واجهة شاشتي {
 *     @حالة عداد: عدد = ٠          // State - حالة محلية
 *     @مراقب بيانات: بيانات        // Observed - كائن مُراقب
 *     @بيئة سمة: السمة              // Environment - من البيئة
 *     @ربط اسم: نص                  // Binding - ربط ثنائي الاتجاه
 * }
 * @endcode
 */
enum class StateBindingType {
    State,              ///< @حالة - حالة محلية للمكون
    Binding,            ///< @ربط - ربط ثنائي الاتجاه
    Observed,           ///< @مراقب - كائن مُراقب (Observable)
    Environment,        ///< @بيئة - قيمة من البيئة
    EnvironmentObject,  ///< @كائن_بيئة - كائن من البيئة
    Published,          ///< @منشور - خاصية منشورة
    AppStorage,         ///< @تخزين_تطبيق - تخزين دائم
    SceneStorage        ///< @تخزين_مشهد - تخزين المشهد
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الرابع: قيم الخصائص (Property Values)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief قيمة لونية
 * 
 * تمثل لوناً يمكن أن يكون:
 * - لوناً مسمى (.أحمر، .أزرق، إلخ)
 * - لوناً بقيم RGB أو RGBA
 * - لوناً سداسي (#FF0000)
 */
struct ColorValue {
    /// أنواع تمثيل اللون
    enum class Type {
        Named,      ///< لون مسمى
        RGB,        ///< قيم RGB (0-255)
        RGBA,       ///< قيم RGBA
        Hex         ///< سداسي عشري
    };
    
    Type type;
    std::string namedColor;     ///< اسم اللون (.أحمر)
    uint8_t r = 0, g = 0, b = 0;
    float alpha = 1.0f;
    std::string hexValue;       ///< "#FF0000"
    
    /// الألوان المسماة الأساسية
    static const std::unordered_map<std::string, ColorValue> namedColors;
    
    /// تحويل إلى نص
    std::string toString() const;
    
    /// إنشاء من لون مسمى
    static ColorValue fromNamed(const std::string& name);
    
    /// إنشاء من RGB
    static ColorValue fromRGB(uint8_t r, uint8_t g, uint8_t b);
    
    /// إنشاء من RGBA
    static ColorValue fromRGBA(uint8_t r, uint8_t g, uint8_t b, float a);
    
    /// إنشاء من قيمة سداسية
    static ColorValue fromHex(const std::string& hex);
};

/**
 * @brief قيمة الحجم/البُعد
 * 
 * تمثل قيمة بُعدية يمكن أن تكون:
 * - مطلقة بالبكسل (١٠٠)
 * - نسبية بالنسبة المئوية (٥٠٪)
 * - مرنة (مالئ، ملاءم)
 */
struct DimensionValue {
    enum class Unit {
        Points,         ///< نقاط/بكسل
        Percent,        ///< نسبة مئوية
        Fill,           ///< مالئ للمساحة المتاحة
        Fit,            ///< ملائم للمحتوى
        Auto            ///< تلقائي
    };
    
    Unit unit = Unit::Points;
    float value = 0.0f;
    
    static DimensionValue points(float v);
    static DimensionValue percent(float v);
    static DimensionValue fill();
    static DimensionValue fit();
    static DimensionValue autoSize();
    
    std::string toString() const;
};

/**
 * @brief قيمة المحاذاة
 */
struct AlignmentValue {
    enum class Horizontal {
        Leading,        ///< بداية (يمين في RTL)
        Center,         ///< وسط
        Trailing        ///< نهاية (يسار في RTL)
    };
    
    enum class Vertical {
        Top,            ///< أعلى
        Center,         ///< وسط
        Bottom          ///< أسفل
    };
    
    Horizontal horizontal = Horizontal::Center;
    Vertical vertical = Vertical::Center;
    
    static AlignmentValue topLeading();
    static AlignmentValue topCenter();
    static AlignmentValue topTrailing();
    static AlignmentValue centerLeading();
    static AlignmentValue center();
    static AlignmentValue centerTrailing();
    static AlignmentValue bottomLeading();
    static AlignmentValue bottomCenter();
    static AlignmentValue bottomTrailing();
};

/**
 * @brief قيمة الحشوة/الهامش
 */
struct EdgeInsetsValue {
    float top = 0;
    float leading = 0;      ///< بداية (يمين في RTL)
    float bottom = 0;
    float trailing = 0;     ///< نهاية (يسار في RTL)
    
    static EdgeInsetsValue all(float v);
    static EdgeInsetsValue horizontal(float v);
    static EdgeInsetsValue vertical(float v);
    static EdgeInsetsValue custom(float t, float l, float b, float tr);
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الخامس: بنية عقدة الواجهة (UI Node Structure)
// ═══════════════════════════════════════════════════════════════════════════

// تصريح مُقدّم
class UINode;
using UINodePtr = std::shared_ptr<UINode>;

/**
 * @brief قيمة خاصية
 * 
 * تمثل قيمة يمكن أن تكون من أنواع مختلفة
 */
// مغلف لمصفوفة القيم (لتجنب النوع ذاتي المرجع)
struct PropertyValueArray;

using PropertyValue = std::variant<
    std::monostate,         ///< لا قيمة
    bool,                   ///< منطقي
    int64_t,                ///< عدد صحيح
    double,                 ///< عدد عشري
    std::string,            ///< نص
    ColorValue,             ///< لون
    DimensionValue,         ///< بُعد
    AlignmentValue,         ///< محاذاة
    EdgeInsetsValue,        ///< حشوة/هامش
    std::shared_ptr<PropertyValueArray>  ///< مصفوفة قيم
>;

/// مغلف مصفوفة قيم الخصائص
struct PropertyValueArray {
    std::vector<PropertyValue> values;
};

/**
 * @brief معدّل على عقدة الواجهة
 */
struct UIModifier {
    ModifierType type;                          ///< نوع المعدّل
    std::string arabicName;                     ///< الاسم العربي
    std::vector<PropertyValue> arguments;       ///< وسائط المعدّل
    std::string closureBody;                    ///< جسم الإغلاق (للأحداث)
    
    /// موقع المصدر
    size_t line = 0;
    size_t column = 0;
};

/**
 * @brief ربط حالة
 */
struct StateBinding {
    StateBindingType type;      ///< نوع الربط
    std::string name;           ///< اسم المتغير
    std::string typeName;       ///< اسم النوع
    std::string initialValue;   ///< القيمة الأولية (اختياري)
    
    size_t line = 0;
    size_t column = 0;
};

/**
 * @brief عقدة واجهة مستخدم
 * 
 * تمثل مكوناً واحداً في شجرة الواجهة.
 * كل عقدة لها:
 * - نوع (نص، زر، عمود، إلخ)
 * - خصائص (وسائط المنشئ)
 * - معدّلات (خصائص سلسلية)
 * - أبناء (للحاويات)
 * 
 * @code
 * // مثال على شجرة العقد:
 * عمود                             ← UINode(Column)
 * ├── نص("مرحباً").لون(.أزرق)      ← UINode(Text) + modifier
 * └── زر("اضغط") { ... }           ← UINode(Button) + closure
 * @endcode
 */
class UINode {
public:
    // ═══════════════════════════════════════════
    // البيانات الأساسية
    // ═══════════════════════════════════════════
    
    UINodeType type;                                ///< نوع العقدة
    std::string arabicName;                         ///< الاسم العربي (مثل "عمود")
    
    // ═══════════════════════════════════════════
    // الخصائص والوسائط
    // ═══════════════════════════════════════════
    
    /// وسائط المنشئ (مثل محتوى النص في نص("مرحباً"))
    std::vector<PropertyValue> constructorArgs;
    
    /// الوسائط المسماة (مثل توسيط: صحيح)
    std::unordered_map<std::string, PropertyValue> namedArgs;
    
    /// المعدّلات المطبقة (.لون، .حجم، إلخ)
    std::vector<UIModifier> modifiers;
    
    // ═══════════════════════════════════════════
    // البنية الشجرية
    // ═══════════════════════════════════════════
    
    /// العقد الأبناء (للحاويات)
    std::vector<UINodePtr> children;
    
    /// إغلاق المحتوى (للأزرار والإجراءات)
    std::string actionClosure;
    
    /// إغلاق المحتوى (للحاويات مع @ViewBuilder)
    std::string contentClosure;
    
    // ═══════════════════════════════════════════
    // معلومات التشخيص
    // ═══════════════════════════════════════════
    
    size_t startLine = 0;
    size_t startColumn = 0;
    size_t endLine = 0;
    size_t endColumn = 0;
    
    // ═══════════════════════════════════════════
    // الدوال المساعدة
    // ═══════════════════════════════════════════
    
    /// هل هذه عقدة حاوية؟
    bool isContainer() const;
    
    /// هل لها أبناء؟
    bool hasChildren() const { return !children.empty(); }
    
    /// هل لها معدّلات؟
    bool hasModifiers() const { return !modifiers.empty(); }
    
    /// الحصول على معدّل بنوعه
    std::optional<UIModifier> getModifier(ModifierType type) const;
    
    /// طباعة الشجرة (للتشخيص)
    std::string dump(int indent = 0) const;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم السادس: تعريف الواجهة (View Definition)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تعريف واجهة كاملة
 * 
 * يمثل تعريف واجهة مستخدم كاملة مع:
 * - الاسم
 * - ربط الحالة
 * - جسم العرض
 * - الدوال المساعدة
 * 
 * @code
 * واجهة شاشة_رئيسية {
 *     @حالة عداد: عدد = ٠
 *     
 *     عرض() {
 *         عمود {
 *             نص("العداد: \(عداد)")
 *             زر("زد") { عداد += ١ }
 *         }
 *     }
 *     
 *     دالة مساعدة() { ... }
 * }
 * @endcode
 */
struct ViewDefinition {
    /// اسم الواجهة
    std::string name;
    
    /// متغيرات الحالة والربط
    std::vector<StateBinding> stateBindings;
    
    /// جسم دالة العرض (شجرة UI)
    UINodePtr bodyTree;
    
    /// الدوال المساعدة (أسماؤها فقط، يُعالجها المحلل العام)
    std::vector<std::string> helperFunctionNames;
    
    /// هل تنفذ سمة معينة؟
    std::vector<std::string> conformances;
    
    /// موقع المصدر
    size_t startLine = 0;
    size_t endLine = 0;
    std::string sourceFile;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم السابع: المحلل (Parser)
// ═══════════════════════════════════════════════════════════════════════════

// تصريح مبسّط لمحلل الرموز (غير مستخدم حالياً في UIParser)
class Lexer {
public:
    Lexer() = default;
    ~Lexer() = default;
};
class Token;

/**
 * @brief نتيجة التحليل
 */
template<typename T>
struct ParseResult {
    std::optional<T> value;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    bool success() const { return value.has_value() && errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

/**
 * @brief إعدادات المحلل
 */
struct UIParserConfig {
    /// هل نسمح بصيغة مختصرة؟
    bool allowShorthandSyntax = true;
    
    /// هل نُنشئ معلومات تشخيص مفصلة؟
    bool generateDetailedDiagnostics = true;
    
    /// هل ندعم الواجهات المتداخلة؟
    bool allowNestedViews = true;
    
    /// الحد الأقصى لعمق التداخل
    int maxNestingDepth = 50;
    
    /// هل نتحقق من توافق المعدّلات مع نوع العقدة؟
    bool validateModifierCompatibility = true;
};

/**
 * @brief محلل واجهة المستخدم التصريحية
 * 
 * يحلل كود واجهة المستخدم بلغة ص ويحولها إلى شجرة عقد (AST)
 * يمكن بعدها تحويلها إلى Jetpack Compose أو SwiftUI.
 * 
 * @code
 * // الاستخدام:
 * UIParser parser(config);
 * 
 * auto result = parser.parseViewDefinition(source);
 * 
 * if (result.success()) {
 *     ViewDefinition view = *result.value;
 *     // استخدم view.bodyTree للحصول على شجرة الواجهة
 * } else {
 *     for (const auto& err : result.errors) {
 *         std::cerr << err << std::endl;
 *     }
 * }
 * @endcode
 */
class UIParser {
public:
    /**
     * @brief إنشاء محلل جديد
     * @param config إعدادات التحليل
     */
    explicit UIParser(const UIParserConfig& config = UIParserConfig{});
    
    ~UIParser();
    
    // ═══════════════════════════════════════════
    // دوال التحليل الرئيسية
    // ═══════════════════════════════════════════
    
    /**
     * @brief تحليل تعريف واجهة كاملة
     * 
     * يحلل كود مثل:
     * @code
     * واجهة شاشتي {
     *     @حالة عداد: عدد = ٠
     *     عرض() { ... }
     * }
     * @endcode
     * 
     * @param source الكود المصدري
     * @param filename اسم الملف (للتشخيص)
     * @return نتيجة التحليل
     */
    ParseResult<ViewDefinition> parseViewDefinition(
        const std::string& source,
        const std::string& filename = "<unknown>"
    );
    
    /**
     * @brief تحليل جسم واجهة فقط
     * 
     * يحلل محتوى دالة عرض() فقط:
     * @code
     * عمود {
     *     نص("مرحباً")
     *     زر("OK") { }
     * }
     * @endcode
     * 
     * @param source الكود المصدري
     * @return شجرة عقد الواجهة
     */
    ParseResult<UINodePtr> parseUIBody(const std::string& source);
    
    /**
     * @brief تحليل مكون واحد
     * 
     * يحلل مكون واحد مع معدّلاته:
     * @code
     * نص("مرحباً").لون(.أزرق).حجم_خط(٢٤)
     * @endcode
     */
    ParseResult<UINodePtr> parseUIComponent(const std::string& source);
    
    // ═══════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════
    
    /**
     * @brief التحقق من صحة شجرة الواجهة
     * 
     * يتحقق من:
     * - توافق المعدّلات مع أنواع العقد
     * - صحة قيم الخصائص
     * - بنية التداخل
     */
    std::vector<std::string> validateTree(const UINodePtr& tree);
    
    /**
     * @brief الحصول على الأخطاء الأخيرة
     */
    const std::vector<std::string>& getLastErrors() const;
    
    /**
     * @brief الحصول على التحذيرات الأخيرة
     */
    const std::vector<std::string>& getLastWarnings() const;
    
    /**
     * @brief مسح الأخطاء والتحذيرات
     */
    void clearDiagnostics();

private:
    // ═══════════════════════════════════════════
    // الحالة الداخلية
    // ═══════════════════════════════════════════
    
    UIParserConfig config_;
    std::unique_ptr<Lexer> lexer_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    int currentNestingDepth_ = 0;
    
    // ═══════════════════════════════════════════
    // دوال التحليل الداخلية
    // ═══════════════════════════════════════════
    
    /// تحليل ربط الحالة (@حالة، @مراقب، إلخ)
    std::optional<StateBinding> parseStateBinding();
    
    /// تحليل عقدة واجهة مفردة
    UINodePtr parseNode();
    
    /// تحليل سلسلة معدّلات
    std::vector<UIModifier> parseModifierChain();
    
    /// تحليل معدّل واحد
    std::optional<UIModifier> parseModifier();
    
    /// تحليل وسائط المنشئ
    std::vector<PropertyValue> parseConstructorArgs();
    
    /// تحليل وسائط مسماة
    std::unordered_map<std::string, PropertyValue> parseNamedArgs();
    
    /// تحليل قيمة خاصية
    std::optional<PropertyValue> parsePropertyValue();
    
    /// تحليل إغلاق المحتوى
    std::string parseContentClosure();
    
    /// تحليل أبناء الحاوية
    std::vector<UINodePtr> parseChildren();
    
    /// تحليل عقدة من نص المصدر
    UINodePtr parseNodeFromSource(const std::string& source);
    
    // ═══════════════════════════════════════════
    // دوال التحقق
    // ═══════════════════════════════════════════
    
    /// التحقق من توافق المعدّل مع نوع العقدة
    bool isModifierCompatible(ModifierType mod, UINodeType node) const;
    
    /// التحقق من صحة قيمة الخاصية
    bool validatePropertyValue(const PropertyValue& value, ModifierType forModifier) const;
    
    // ═══════════════════════════════════════════
    // دوال الأخطاء
    // ═══════════════════════════════════════════
    
    /// إضافة خطأ
    void addError(const std::string& message, size_t line, size_t column);
    
    /// إضافة تحذير
    void addWarning(const std::string& message, size_t line, size_t column);
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثامن: دوال مساعدة عامة
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief التحقق مما إذا كان الاسم مكون واجهة معروف
 */
bool isKnownUIComponent(const std::string& arabicName);

/**
 * @brief التحقق مما إذا كان الاسم معدّل معروف
 */
bool isKnownModifier(const std::string& arabicName);

/**
 * @brief الحصول على قائمة جميع مكونات الواجهة
 */
std::vector<std::string> getAllUIComponentNames();

/**
 * @brief الحصول على قائمة جميع المعدّلات
 */
std::vector<std::string> getAllModifierNames();

/**
 * @brief الحصول على المعدّلات المتوافقة مع نوع عقدة معين
 */
std::vector<ModifierType> getCompatibleModifiers(UINodeType nodeType);

} // namespace parser
} // namespace sad

#endif // SAD_UI_PARSER_HPP
