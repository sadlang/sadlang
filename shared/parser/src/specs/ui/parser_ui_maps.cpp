/**
 * ==========================================================================
 * ملف: ui_parser.cpp
 * الوصف: تنفيذ محلل واجهة المستخدم التصريحية للغة ص
 * الإصدار: 1.0.0
 * ==========================================================================
 * 
 * هذا الملف يحتوي على تنفيذ محلل واجهة المستخدم الذي يحول
 * صيغة الواجهة التصريحية العربية إلى شجرة عقد (AST).
 * 
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║                        مراحل التحليل                                      ║
 * ╠══════════════════════════════════════════════════════════════════════════╣
 * ║                                                                          ║
 * ║  ┌─────────────┐   ┌─────────────┐   ┌─────────────┐   ┌─────────────┐  ║
 * ║  │  كود المصدر │ → │   التحليل   │ → │  شجرة AST  │ → │  التحقق    │   ║
 * ║  │   العربي    │   │   اللغوي   │   │   للواجهة  │   │   والتصحيح │   ║
 * ║  └─────────────┘   └─────────────┘   └─────────────┘   └─────────────┘  ║
 * ║                                                                          ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 * 
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#include "ui_parser.hpp"
#include <sstream>
#include <algorithm>
#include <cassert>
#include <regex>

namespace sad {
namespace parser {

// ═══════════════════════════════════════════════════════════════════════════
// القسم الأول: جداول الترجمة العربية
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief خريطة تحويل أنواع العقد إلى أسماء عربية
 * 
 * هذه الخريطة تربط كل نوع عقدة بالكلمة المفتاحية العربية
 * المستخدمة في كود المصدر.
 */
static const std::unordered_map<UINodeType, std::string> nodeTypeToArabic = {
    // ═══════════════════════════════════════════
    // مكونات العرض الأساسية
    // ═══════════════════════════════════════════
    {UINodeType::Text,              "نص"},
    {UINodeType::Image,             "صورة"},
    {UINodeType::Icon,              "أيقونة"},
    
    // ═══════════════════════════════════════════
    // مكونات الإدخال
    // ═══════════════════════════════════════════
    {UINodeType::Button,            "زر"},
    {UINodeType::TextField,         "حقل_نص"},
    {UINodeType::TextArea,          "منطقة_نص"},
    {UINodeType::Toggle,            "تبديل"},
    {UINodeType::Slider,            "منزلق"},
    {UINodeType::Picker,            "منتقي"},
    {UINodeType::DatePicker,        "منتقي_تاريخ"},
    {UINodeType::ColorPicker,       "منتقي_لون"},
    
    // ═══════════════════════════════════════════
    // مكونات التخطيط
    // ═══════════════════════════════════════════
    {UINodeType::Column,            "عمود"},
    {UINodeType::Row,               "صف"},
    {UINodeType::Stack,             "رصة"},
    {UINodeType::Grid,              "شبكة"},
    {UINodeType::LazyColumn,        "عمود_كسول"},
    {UINodeType::LazyRow,           "صف_كسول"},
    {UINodeType::LazyGrid,          "شبكة_كسولة"},
    {UINodeType::Spacer,            "فاصل"},
    {UINodeType::Divider,           "فاصل_خط"},
    
    // ═══════════════════════════════════════════
    // مكونات الحاويات
    // ═══════════════════════════════════════════
    {UINodeType::Container,         "حاوية"},
    {UINodeType::Card,              "بطاقة"},
    {UINodeType::Surface,           "سطح"},
    {UINodeType::Scaffold,          "هيكل"},
    {UINodeType::Box,               "صندوق"},
    
    // ═══════════════════════════════════════════
    // مكونات التنقل
    // ═══════════════════════════════════════════
    {UINodeType::NavigationView,    "عرض_تنقل"},
    {UINodeType::NavigationLink,    "رابط_تنقل"},
    {UINodeType::TabView,           "عرض_أقسام"},
    {UINodeType::TabItem,           "قسم"},
    
    // ═══════════════════════════════════════════
    // مكونات القوائم
    // ═══════════════════════════════════════════
    {UINodeType::List,              "قائمة"},
    {UINodeType::Section,           "قسم_قائمة"},
    {UINodeType::ForEach,           "لكل"},
    
    // ═══════════════════════════════════════════
    // مكونات الحوارات
    // ═══════════════════════════════════════════
    {UINodeType::Alert,             "تنبيه"},
    {UINodeType::Sheet,             "ورقة"},
    {UINodeType::Dialog,            "حوار"},
    {UINodeType::Menu,              "قائمة_خيارات"},
    
    // ═══════════════════════════════════════════
    // مكونات متقدمة
    // ═══════════════════════════════════════════
    {UINodeType::Canvas,            "لوحة"},
    {UINodeType::Map,               "خريطة"},
    {UINodeType::WebView,           "عرض_ويب"},
    {UINodeType::VideoPlayer,       "مشغل_فيديو"},
    
    // ═══════════════════════════════════════════
    // عناصر خاصة
    // ═══════════════════════════════════════════
    {UINodeType::ViewBuilder,       "منشئ_واجهة"},
    {UINodeType::CustomView,        "واجهة_مخصصة"},
    {UINodeType::Conditional,       "شرطي"},
    {UINodeType::Group,             "مجموعة"}
};

/**
 * @brief خريطة عكسية للتحويل من العربية إلى نوع العقدة
 */
static const std::unordered_map<std::string, UINodeType> arabicToNodeType = []() {
    std::unordered_map<std::string, UINodeType> result;
    for (const auto& [type, name] : nodeTypeToArabic) {
        result[name] = type;
    }
    return result;
}();

/**
 * @brief خريطة تحويل أنواع المعدّلات إلى أسماء عربية
 */
static const std::unordered_map<ModifierType, std::string> modifierTypeToArabic = {
    // ═══════════════════════════════════════════
    // معدّلات الألوان
    // ═══════════════════════════════════════════
    {ModifierType::ForegroundColor,     "لون"},
    {ModifierType::BackgroundColor,     "لون_خلفية"},
    {ModifierType::TintColor,           "لون_صبغة"},
    
    // ═══════════════════════════════════════════
    // معدّلات الخط
    // ═══════════════════════════════════════════
    {ModifierType::FontSize,            "حجم_خط"},
    {ModifierType::FontWeight,          "سمك_خط"},
    {ModifierType::FontFamily,          "عائلة_خط"},
    {ModifierType::FontStyle,           "نمط_خط"},
    {ModifierType::LineSpacing,         "تباعد_أسطر"},
    
    // ═══════════════════════════════════════════
    // معدّلات الحجم
    // ═══════════════════════════════════════════
    {ModifierType::Width,               "عرض"},
    {ModifierType::Height,              "ارتفاع"},
    {ModifierType::MinWidth,            "أدنى_عرض"},
    {ModifierType::MinHeight,           "أدنى_ارتفاع"},
    {ModifierType::MaxWidth,            "أقصى_عرض"},
    {ModifierType::MaxHeight,           "أقصى_ارتفاع"},
    
    // ═══════════════════════════════════════════
    // معدّلات التباعد
    // ═══════════════════════════════════════════
    {ModifierType::Padding,             "حشوة"},
    {ModifierType::PaddingTop,          "حشوة_علوية"},
    {ModifierType::PaddingBottom,       "حشوة_سفلية"},
    {ModifierType::PaddingLeading,      "حشوة_بداية"},
    {ModifierType::PaddingTrailing,     "حشوة_نهاية"},
    {ModifierType::Margin,              "هامش"},
    
    // ═══════════════════════════════════════════
    // معدّلات الحدود
    // ═══════════════════════════════════════════
    {ModifierType::Border,              "حد"},
    {ModifierType::BorderRadius,        "انحناء_حد"},
    {ModifierType::BorderWidth,         "سمك_حد"},
    {ModifierType::BorderColor,         "لون_حد"},
    
    // ═══════════════════════════════════════════
    // معدّلات المحاذاة
    // ═══════════════════════════════════════════
    {ModifierType::Alignment,           "محاذاة"},
    {ModifierType::Frame,               "إطار"},
    {ModifierType::Position,            "موقع"},
    {ModifierType::Offset,              "إزاحة"},
    
    // ═══════════════════════════════════════════
    // معدّلات الظل والعتامة
    // ═══════════════════════════════════════════
    {ModifierType::Shadow,              "ظل"},
    {ModifierType::Opacity,             "عتامة"},
    {ModifierType::Blur,                "ضبابية"},
    
    // ═══════════════════════════════════════════
    // معدّلات التحويل
    // ═══════════════════════════════════════════
    {ModifierType::Rotation,            "دوران"},
    {ModifierType::Scale,               "مقياس"},
    
    // ═══════════════════════════════════════════
    // معدّلات التفاعل
    // ═══════════════════════════════════════════
    {ModifierType::OnTap,               "عند_النقر"},
    {ModifierType::OnLongPress,         "عند_الضغط_المطول"},
    {ModifierType::OnDrag,              "عند_السحب"},
    {ModifierType::OnAppear,            "عند_الظهور"},
    {ModifierType::OnDisappear,         "عند_الاختفاء"},
    
    // ═══════════════════════════════════════════
    // معدّلات الحالة
    // ═══════════════════════════════════════════
    {ModifierType::Disabled,            "معطل"},
    {ModifierType::Hidden,              "مخفي"},
    
    // ═══════════════════════════════════════════
    // معدّلات الرسوم المتحركة
    // ═══════════════════════════════════════════
    {ModifierType::Animation,           "رسوم_متحركة"},
    {ModifierType::Transition,          "انتقال"},
    
    // ═══════════════════════════════════════════
    // معدّلات متنوعة
    // ═══════════════════════════════════════════
    {ModifierType::ClipShape,           "شكل_قص"},
    {ModifierType::ZIndex,              "مستوى_ع"},
    {ModifierType::Accessibility,       "إمكانية_وصول"},
    {ModifierType::Tag,                 "علامة"}
};

/**
 * @brief خريطة عكسية للتحويل من العربية إلى نوع المعدّل
 */
static const std::unordered_map<std::string, ModifierType> arabicToModifierType = []() {
    std::unordered_map<std::string, ModifierType> result;
    for (const auto& [type, name] : modifierTypeToArabic) {
        result[name] = type;
    }
    return result;
}();

/**
 * @brief الألوان المسماة الأساسية
 */
const std::unordered_map<std::string, ColorValue> ColorValue::namedColors = {
    // الألوان الأساسية
    {"أحمر",        ColorValue::fromRGB(255, 0, 0)},
    {"أخضر",        ColorValue::fromRGB(0, 128, 0)},
    {"أزرق",        ColorValue::fromRGB(0, 0, 255)},
    {"أصفر",        ColorValue::fromRGB(255, 255, 0)},
    {"برتقالي",     ColorValue::fromRGB(255, 165, 0)},
    {"بنفسجي",      ColorValue::fromRGB(128, 0, 128)},
    {"وردي",        ColorValue::fromRGB(255, 192, 203)},
    {"بني",         ColorValue::fromRGB(165, 42, 42)},
    
    // الألوان المحايدة
    {"أبيض",        ColorValue::fromRGB(255, 255, 255)},
    {"أسود",        ColorValue::fromRGB(0, 0, 0)},
    {"رمادي",       ColorValue::fromRGB(128, 128, 128)},
    {"رمادي_فاتح",  ColorValue::fromRGB(211, 211, 211)},
    {"رمادي_غامق",  ColorValue::fromRGB(64, 64, 64)},
    
    // الألوان الدلالية
    {"أساسي",       ColorValue::fromRGB(0, 122, 255)},     // iOS Blue
    {"ثانوي",       ColorValue::fromRGB(142, 142, 147)},   // iOS Gray
    {"نجاح",        ColorValue::fromRGB(52, 199, 89)},     // iOS Green
    {"تحذير",       ColorValue::fromRGB(255, 204, 0)},     // iOS Yellow
    {"خطر",         ColorValue::fromRGB(255, 59, 48)},     // iOS Red
    
    // شفاف
    {"شفاف",        ColorValue::fromRGBA(0, 0, 0, 0.0f)}
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثاني: تنفيذ دوال التحويل
// ═══════════════════════════════════════════════════════════════════════════

std::string uiNodeTypeToArabicName(UINodeType type) {
    auto it = nodeTypeToArabic.find(type);
    if (it != nodeTypeToArabic.end()) {
        return it->second;
    }
    return "غير_معروف";
}

std::optional<UINodeType> arabicNameToUINodeType(const std::string& name) {
    auto it = arabicToNodeType.find(name);
    if (it != arabicToNodeType.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::string modifierTypeToArabicName(ModifierType type) {
    auto it = modifierTypeToArabic.find(type);
    if (it != modifierTypeToArabic.end()) {
        return it->second;
    }
    return "غير_معروف";
}

std::optional<ModifierType> arabicNameToModifierType(const std::string& name) {
    auto it = arabicToModifierType.find(name);
    if (it != arabicToModifierType.end()) {
        return it->second;
    }
    return std::nullopt;
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثالث: تنفيذ ColorValue
// ═══════════════════════════════════════════════════════════════════════════

std::string ColorValue::toString() const {
    switch (type) {
        case Type::Named:
            return "." + namedColor;
        case Type::RGB:
            return "rgb(" + std::to_string(r) + ", " + 
                   std::to_string(g) + ", " + std::to_string(b) + ")";
        case Type::RGBA:
            return "rgba(" + std::to_string(r) + ", " + 
                   std::to_string(g) + ", " + std::to_string(b) + 
                   ", " + std::to_string(alpha) + ")";
        case Type::Hex:
            return hexValue;
    }
    return "";
}

ColorValue ColorValue::fromNamed(const std::string& name) {
    // إزالة النقطة من البداية إن وجدت
    std::string cleanName = name;
    if (!cleanName.empty() && cleanName[0] == '.') {
        cleanName = cleanName.substr(1);
    }
    
    ColorValue cv;
    cv.type = Type::Named;
    cv.namedColor = cleanName;
    
    // البحث عن القيم الفعلية
    auto it = namedColors.find(cleanName);
    if (it != namedColors.end()) {
        cv.r = it->second.r;
        cv.g = it->second.g;
        cv.b = it->second.b;
        cv.alpha = it->second.alpha;
    }
    
    return cv;
}

ColorValue ColorValue::fromRGB(uint8_t r, uint8_t g, uint8_t b) {
    ColorValue cv;
    cv.type = Type::RGB;
    cv.r = r;
    cv.g = g;
    cv.b = b;
    cv.alpha = 1.0f;
    return cv;
}

ColorValue ColorValue::fromRGBA(uint8_t r, uint8_t g, uint8_t b, float a) {
    ColorValue cv;
    cv.type = Type::RGBA;
    cv.r = r;
    cv.g = g;
    cv.b = b;
    cv.alpha = a;
    return cv;
}

ColorValue ColorValue::fromHex(const std::string& hex) {
    ColorValue cv;
    cv.type = Type::Hex;
    cv.hexValue = hex;
    
    // تحليل القيمة السداسية
    std::string cleanHex = hex;
    if (!cleanHex.empty() && cleanHex[0] == '#') {
        cleanHex = cleanHex.substr(1);
    }
    
    if (cleanHex.length() == 6 || cleanHex.length() == 8) {
        cv.r = static_cast<uint8_t>(std::stoul(cleanHex.substr(0, 2), nullptr, 16));
        cv.g = static_cast<uint8_t>(std::stoul(cleanHex.substr(2, 2), nullptr, 16));
        cv.b = static_cast<uint8_t>(std::stoul(cleanHex.substr(4, 2), nullptr, 16));
        
        if (cleanHex.length() == 8) {
            cv.alpha = std::stoul(cleanHex.substr(6, 2), nullptr, 16) / 255.0f;
        } else {
            cv.alpha = 1.0f;
        }
    }
    
    return cv;
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الرابع: تنفيذ DimensionValue
// ═══════════════════════════════════════════════════════════════════════════

DimensionValue DimensionValue::points(float v) {
    DimensionValue dv;
    dv.unit = Unit::Points;
    dv.value = v;
    return dv;
}

DimensionValue DimensionValue::percent(float v) {
    DimensionValue dv;
    dv.unit = Unit::Percent;
    dv.value = v;
    return dv;
}

DimensionValue DimensionValue::fill() {
    DimensionValue dv;
    dv.unit = Unit::Fill;
    return dv;
}

DimensionValue DimensionValue::fit() {
    DimensionValue dv;
    dv.unit = Unit::Fit;
    return dv;
}

DimensionValue DimensionValue::autoSize() {
    DimensionValue dv;
    dv.unit = Unit::Auto;
    return dv;
}

std::string DimensionValue::toString() const {
    switch (unit) {
        case Unit::Points:
            return std::to_string(static_cast<int>(value));
        case Unit::Percent:
            return std::to_string(static_cast<int>(value)) + "%";
        case Unit::Fill:
            return "مالئ";
        case Unit::Fit:
            return "ملائم";
        case Unit::Auto:
            return "تلقائي";
    }
    return "";
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الخامس: تنفيذ AlignmentValue
// ═══════════════════════════════════════════════════════════════════════════

AlignmentValue AlignmentValue::topLeading() {
    return {Horizontal::Leading, Vertical::Top};
}

AlignmentValue AlignmentValue::topCenter() {
    return {Horizontal::Center, Vertical::Top};
}

AlignmentValue AlignmentValue::topTrailing() {
    return {Horizontal::Trailing, Vertical::Top};
}

AlignmentValue AlignmentValue::centerLeading() {
    return {Horizontal::Leading, Vertical::Center};
}

AlignmentValue AlignmentValue::center() {
    return {Horizontal::Center, Vertical::Center};
}

AlignmentValue AlignmentValue::centerTrailing() {
    return {Horizontal::Trailing, Vertical::Center};
}

AlignmentValue AlignmentValue::bottomLeading() {
    return {Horizontal::Leading, Vertical::Bottom};
}

AlignmentValue AlignmentValue::bottomCenter() {
    return {Horizontal::Center, Vertical::Bottom};
}

AlignmentValue AlignmentValue::bottomTrailing() {
    return {Horizontal::Trailing, Vertical::Bottom};
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم السادس: تنفيذ EdgeInsetsValue
// ═══════════════════════════════════════════════════════════════════════════

EdgeInsetsValue EdgeInsetsValue::all(float v) {
    return {v, v, v, v};
}

EdgeInsetsValue EdgeInsetsValue::horizontal(float v) {
    return {0, v, 0, v};
}

EdgeInsetsValue EdgeInsetsValue::vertical(float v) {
    return {v, 0, v, 0};
}

EdgeInsetsValue EdgeInsetsValue::custom(float t, float l, float b, float tr) {
    return {t, l, b, tr};
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم السابع: تنفيذ UINode
// ═══════════════════════════════════════════════════════════════════════════

bool UINode::isContainer() const {
    switch (type) {
        case UINodeType::Column:
        case UINodeType::Row:
        case UINodeType::Stack:
        case UINodeType::Grid:
        case UINodeType::LazyColumn:
        case UINodeType::LazyRow:
        case UINodeType::LazyGrid:
        case UINodeType::Container:
        case UINodeType::Card:
        case UINodeType::Surface:
        case UINodeType::Scaffold:
        case UINodeType::Box:
        case UINodeType::NavigationView:
        case UINodeType::TabView:
        case UINodeType::List:
        case UINodeType::Section:
        case UINodeType::Group:
        case UINodeType::ForEach:
            return true;
        default:
            return false;
    }
}

std::optional<UIModifier> UINode::getModifier(ModifierType type) const {
    for (const auto& mod : modifiers) {
        if (mod.type == type) {
            return mod;
        }
    }
    return std::nullopt;
}

std::string UINode::dump(int indent) const {
    std::stringstream ss;
    std::string pad(indent * 2, ' ');
    
    // طباعة العقدة
    ss << pad << arabicName << "(" << uiNodeTypeToArabicName(type) << ")";
    
    // طباعة الوسائط
    if (!constructorArgs.empty()) {
        ss << " [" << constructorArgs.size() << " وسائط]";
    }
    
    // طباعة المعدّلات
    if (!modifiers.empty()) {
        ss << "\n" << pad << "  معدّلات: ";
        for (const auto& mod : modifiers) {
            ss << "." << mod.arabicName << " ";
        }
    }
    
    // طباعة الأبناء
    if (!children.empty()) {
        ss << "\n" << pad << "  أبناء: " << children.size() << "\n";
        for (const auto& child : children) {
            ss << child->dump(indent + 2);
        }
    } else {
        ss << "\n";
    }
    
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثامن: تنفيذ UIParser
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief منشئ المحلل
 */
UIParser::UIParser(const UIParserConfig& config)
    : config_(config) {
    // لا حاجة لتهيئة إضافية حالياً
}

UIParser::~UIParser() = default;

/**
 * @brief تحليل تعريف واجهة كاملة
 * 
 * يحلل كود مثل:
 * @code
 * واجهة شاشة_رئيسية {
 *     @حالة عداد: عدد = ٠
 *     عرض() {
 *         عمود { ... }
 *     }
 * }
 * @endcode
 */
ParseResult<ViewDefinition> UIParser::parseViewDefinition(
    const std::string& source,
    const std::string& filename
) {
    ParseResult<ViewDefinition> result;
    clearDiagnostics();
    
    // التحقق من الكود الفارغ
    if (source.empty()) {
        addError("كود المصدر فارغ", 1, 1);
        result.errors = errors_;
        return result;
    }
    
    ViewDefinition view;
    view.sourceFile = filename;
    view.startLine = 1;
    
    // ═══════════════════════════════════════════
    // المرحلة 1: البحث عن كلمة "واجهة"
    // ═══════════════════════════════════════════
    
    // تعبير نمطي للبحث عن تعريف الواجهة
    std::regex viewRegex(R"(واجهة\s+(\S+)\s*\{)");
    std::smatch match;
    
    if (!std::regex_search(source, match, viewRegex)) {
        addError("لم يُعثر على تعريف واجهة صالح. استخدم: واجهة اسم_الواجهة { ... }", 1, 1);
        result.errors = errors_;
        return result;
    }
    
    view.name = match[1].str();
    
    // ═══════════════════════════════════════════
    // المرحلة 2: استخراج محتوى الواجهة
    // ═══════════════════════════════════════════
    
    size_t bodyStart = match.position() + match.length();
    int braceCount = 1;
    size_t bodyEnd = bodyStart;
    
    for (size_t i = bodyStart; i < source.length(); ++i) {
        if (source[i] == '{') {
            braceCount++;
        } else if (source[i] == '}') {
            braceCount--;
            if (braceCount == 0) {
                bodyEnd = i;
                break;
            }
        }
    }
    
    if (braceCount != 0) {
        addError("أقواس غير متطابقة في تعريف الواجهة", 1, bodyStart);
        result.errors = errors_;
        return result;
    }
    
    std::string bodyContent = source.substr(bodyStart, bodyEnd - bodyStart);
    
    // ═══════════════════════════════════════════
    // المرحلة 3: تحليل ربط الحالة (@حالة، @مراقب، إلخ)
    // ═══════════════════════════════════════════
    
    std::regex stateRegex(R"(@(حالة|مراقب|بيئة|ربط)\s+(\w+)\s*:\s*(\w+)(?:\s*=\s*([^\n]+))?)");
    std::string::const_iterator searchStart(bodyContent.cbegin());
    
    while (std::regex_search(searchStart, bodyContent.cend(), match, stateRegex)) {
        StateBinding binding;
        
        std::string bindingType = match[1].str();
        binding.name = match[2].str();
        binding.typeName = match[3].str();
        
        if (match[4].matched) {
            binding.initialValue = match[4].str();
            // إزالة المسافات من البداية والنهاية
            binding.initialValue.erase(0, binding.initialValue.find_first_not_of(" \t"));
            binding.initialValue.erase(binding.initialValue.find_last_not_of(" \t") + 1);
        }
        
        // تحديد نوع الربط
        if (bindingType == "حالة") {
            binding.type = StateBindingType::State;
        } else if (bindingType == "مراقب") {
            binding.type = StateBindingType::Observed;
        } else if (bindingType == "بيئة") {
            binding.type = StateBindingType::Environment;
        } else if (bindingType == "ربط") {
            binding.type = StateBindingType::Binding;
        }
        
        view.stateBindings.push_back(binding);
        searchStart = match.suffix().first;
    }
    
    // ═══════════════════════════════════════════
    // المرحلة 4: البحث عن دالة عرض()
    // ═══════════════════════════════════════════
    
    std::regex viewBodyRegex(R"(عرض\s*\(\s*\)\s*\{)");
    
    if (!std::regex_search(bodyContent, match, viewBodyRegex)) {
        addError("لم يُعثر على دالة عرض() في الواجهة", 1, 1);
        result.errors = errors_;
        return result;
    }
    
    size_t viewBodyStart = match.position() + match.length();
    braceCount = 1;
    size_t viewBodyEnd = viewBodyStart;
    
    for (size_t i = viewBodyStart; i < bodyContent.length(); ++i) {
        if (bodyContent[i] == '{') {
            braceCount++;
        } else if (bodyContent[i] == '}') {
            braceCount--;
            if (braceCount == 0) {
                viewBodyEnd = i;
                break;
            }
        }
    }
    
    std::string viewBodyContent = bodyContent.substr(viewBodyStart, viewBodyEnd - viewBodyStart);
    
    // ═══════════════════════════════════════════
    // المرحلة 5: تحليل جسم الواجهة
    // ═══════════════════════════════════════════
    
    auto bodyResult = parseUIBody(viewBodyContent);
    
    if (!bodyResult.success()) {
        result.errors = bodyResult.errors;
        result.warnings = bodyResult.warnings;
        return result;
    }
    
    view.bodyTree = *bodyResult.value;
    view.endLine = std::count(source.begin(), source.begin() + bodyEnd, '\n') + 1;
    
    // النجاح!
    result.value = view;
    result.warnings = warnings_;
    
    return result;
}

/**
 * @brief تحليل جسم واجهة
 */
ParseResult<UINodePtr> UIParser::parseUIBody(const std::string& source) {
    ParseResult<UINodePtr> result;
    clearDiagnostics();
    
    if (source.empty()) {
        addError("جسم الواجهة فارغ", 1, 1);
        result.errors = errors_;
        return result;
    }
    
    // تنظيف المصدر
    std::string cleanSource = source;
    
    // إزالة المسافات الزائدة
    cleanSource.erase(0, cleanSource.find_first_not_of(" \t\n\r"));
    cleanSource.erase(cleanSource.find_last_not_of(" \t\n\r") + 1);
    
    if (cleanSource.empty()) {
        // جسم فارغ، إنشاء عقدة مجموعة فارغة
        auto emptyGroup = std::make_shared<UINode>();
        emptyGroup->type = UINodeType::Group;
        emptyGroup->arabicName = "مجموعة";
        result.value = emptyGroup;
        return result;
    }
    
    // ═══════════════════════════════════════════
    // تحليل العقدة الجذرية
    // ═══════════════════════════════════════════
    
    auto rootNode = parseNodeFromSource(cleanSource);
    
    if (!rootNode) {
        addError("فشل تحليل جسم الواجهة", 1, 1);
        result.errors = errors_;
        return result;
    }
    
    result.value = rootNode;
    result.warnings = warnings_;
    
    return result;
}

/**
 * @brief تحليل مكون واحد
 */
ParseResult<UINodePtr> UIParser::parseUIComponent(const std::string& source) {
    return parseUIBody(source);
}

/**
 * @brief التحقق من صحة شجرة الواجهة
 */
std::vector<std::string> UIParser::validateTree(const UINodePtr& tree) {
    std::vector<std::string> errors;
    
    if (!tree) {
        errors.push_back("شجرة الواجهة فارغة");
        return errors;
    }
    
    // التحقق من توافق المعدّلات
    if (config_.validateModifierCompatibility) {
        for (const auto& mod : tree->modifiers) {
            if (!isModifierCompatible(mod.type, tree->type)) {
                errors.push_back(
                    "المعدّل '" + mod.arabicName + 
                    "' غير متوافق مع المكون '" + tree->arabicName + "'"
                );
            }
        }
    }
    
    // التحقق من الأبناء بشكل تكراري
    for (const auto& child : tree->children) {
        auto childErrors = validateTree(child);
        errors.insert(errors.end(), childErrors.begin(), childErrors.end());
    }
    
    return errors;
}

const std::vector<std::string>& UIParser::getLastErrors() const {
    return errors_;
}

const std::vector<std::string>& UIParser::getLastWarnings() const {
    return warnings_;
}

void UIParser::clearDiagnostics() {
    errors_.clear();
    warnings_.clear();
    currentNestingDepth_ = 0;
}

bool UIParser::isModifierCompatible(ModifierType mod, UINodeType node) const {
    // معظم المعدّلات متوافقة مع معظم العقد
    // هذه هي الاستثناءات:
    
    switch (mod) {
        case ModifierType::FontSize:
        case ModifierType::FontWeight:
        case ModifierType::FontFamily:
        case ModifierType::FontStyle:
        case ModifierType::LineSpacing:
            // معدّلات الخط متوافقة فقط مع Text و TextField و TextArea
            return node == UINodeType::Text || 
                   node == UINodeType::TextField || 
                   node == UINodeType::TextArea ||
                   node == UINodeType::Button;
            
        case ModifierType::OnTap:
        case ModifierType::OnLongPress:
            // أحداث النقر غير منطقية على Spacer
            return node != UINodeType::Spacer && node != UINodeType::Divider;
            
        default:
            return true;
    }
}

void UIParser::addError(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "خطأ [سطر " << line << "، عمود " << column << "]: " << message;
    errors_.push_back(ss.str());
}

void UIParser::addWarning(const std::string& message, size_t line, size_t column) {
    std::stringstream ss;
    ss << "تحذير [سطر " << line << "، عمود " << column << "]: " << message;
    warnings_.push_back(ss.str());
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم التاسع: دوال التحليل الداخلية
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تحليل عقدة من نص المصدر
 */
UINodePtr UIParser::parseNodeFromSource(const std::string& source) {
    // التحقق من عمق التداخل
    if (currentNestingDepth_ >= config_.maxNestingDepth) {
        addError("تم تجاوز الحد الأقصى لعمق التداخل", 1, 1);
        return nullptr;
    }
    
    currentNestingDepth_++;
    
    // البحث عن اسم المكون
    std::regex componentRegex(R"(^([^\s\(\{\.]+))");
    std::smatch match;
    
    if (!std::regex_search(source, match, componentRegex)) {
        currentNestingDepth_--;
        return nullptr;
    }
    
    std::string componentName = match[1].str();
    
    // التحقق من أن المكون معروف
    auto nodeType = arabicNameToUINodeType(componentName);
    if (!nodeType.has_value()) {
        // قد يكون واجهة مخصصة
        auto node = std::make_shared<UINode>();
        node->type = UINodeType::CustomView;
        node->arabicName = componentName;
        currentNestingDepth_--;
        return node;
    }
    
    auto node = std::make_shared<UINode>();
    node->type = *nodeType;
    node->arabicName = componentName;
    
    // موقع ما بعد اسم المكون
    size_t pos = match.length();
    
    // ═══════════════════════════════════════════
    // تحليل وسائط المنشئ ()
    // ═══════════════════════════════════════════
    
    if (pos < source.length() && source[pos] == '(') {
        int parenCount = 1;
        size_t argStart = pos + 1;
        size_t argEnd = argStart;
        
        for (size_t i = argStart; i < source.length(); ++i) {
            if (source[i] == '(') {
                parenCount++;
            } else if (source[i] == ')') {
                parenCount--;
                if (parenCount == 0) {
                    argEnd = i;
                    break;
                }
            }
        }
        
        std::string argsStr = source.substr(argStart, argEnd - argStart);
        
        // تحليل الوسائط (بشكل مبسط)
        if (!argsStr.empty()) {
            // التحقق مما إذا كان نصاً
            if (argsStr[0] == '"' || argsStr[0] == '\'') {
                // استخراج النص
                std::string textValue = argsStr.substr(1, argsStr.length() - 2);
                node->constructorArgs.push_back(textValue);
            } else {
                // قيمة أخرى
                node->constructorArgs.push_back(argsStr);
            }
        }
        
        pos = argEnd + 1;
    }
    
    // ═══════════════════════════════════════════
    // تحليل المعدّلات .xxx()
    // ═══════════════════════════════════════════
    
    std::regex modifierRegex(R"(\.([^\s\(\)]+)(?:\(([^)]*)\))?)");
    std::string remainingSource = source.substr(pos);
    std::string::const_iterator searchStart(remainingSource.cbegin());
    
    while (std::regex_search(searchStart, remainingSource.cend(), match, modifierRegex)) {
        UIModifier modifier;
        modifier.arabicName = match[1].str();
        
        auto modType = arabicNameToModifierType(modifier.arabicName);
        if (modType.has_value()) {
            modifier.type = *modType;
        }
        
        // تحليل وسائط المعدّل
        if (match[2].matched) {
            std::string modArgs = match[2].str();
            if (!modArgs.empty()) {
                // لون مسمى
                if (modArgs[0] == '.') {
                    modifier.arguments.push_back(ColorValue::fromNamed(modArgs));
                } else {
                    // قيمة رقمية أو نصية
                    try {
                        double numValue = std::stod(modArgs);
                        modifier.arguments.push_back(numValue);
                    } catch (...) {
                        modifier.arguments.push_back(modArgs);
                    }
                }
            }
        }
        
        node->modifiers.push_back(modifier);
        searchStart = match.suffix().first;
    }
    
    // ═══════════════════════════════════════════
    // تحليل الأبناء { }
    // ═══════════════════════════════════════════
    
    size_t bracePos = source.find('{', pos);
    if (bracePos != std::string::npos && node->isContainer()) {
        int braceCount = 1;
        size_t childrenStart = bracePos + 1;
        size_t childrenEnd = childrenStart;
        
        for (size_t i = childrenStart; i < source.length(); ++i) {
            if (source[i] == '{') {
                braceCount++;
            } else if (source[i] == '}') {
                braceCount--;
                if (braceCount == 0) {
                    childrenEnd = i;
                    break;
                }
            }
        }
        
        std::string childrenContent = source.substr(childrenStart, childrenEnd - childrenStart);
        
        // تحليل الأبناء (بشكل مبسط - كل سطر مكون)
        std::istringstream stream(childrenContent);
        std::string line;
        std::string currentBlock;
        int blockBraceCount = 0;
        
        while (std::getline(stream, line)) {
            // إزالة المسافات
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty()) continue;
            
            // تتبع الأقواس
            for (char c : line) {
                if (c == '{') blockBraceCount++;
                else if (c == '}') blockBraceCount--;
            }
            
            currentBlock += line + "\n";
            
            // إذا أُغلقت جميع الأقواس، هذا مكون كامل
            if (blockBraceCount == 0 && !currentBlock.empty()) {
                auto childNode = parseNodeFromSource(currentBlock);
                if (childNode) {
                    node->children.push_back(childNode);
                }
                currentBlock.clear();
            }
        }
        
        // معالجة أي كتلة متبقية
        if (!currentBlock.empty()) {
            auto childNode = parseNodeFromSource(currentBlock);
            if (childNode) {
                node->children.push_back(childNode);
            }
        }
    }
    
    currentNestingDepth_--;
    return node;
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم العاشر: الدوال المساعدة العامة
// ═══════════════════════════════════════════════════════════════════════════

bool isKnownUIComponent(const std::string& arabicName) {
    return arabicToNodeType.find(arabicName) != arabicToNodeType.end();
}

bool isKnownModifier(const std::string& arabicName) {
    return arabicToModifierType.find(arabicName) != arabicToModifierType.end();
}

std::vector<std::string> getAllUIComponentNames() {
    std::vector<std::string> names;
    names.reserve(nodeTypeToArabic.size());
    for (const auto& [type, name] : nodeTypeToArabic) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> getAllModifierNames() {
    std::vector<std::string> names;
    names.reserve(modifierTypeToArabic.size());
    for (const auto& [type, name] : modifierTypeToArabic) {
        names.push_back(name);
    }
    return names;
}

std::vector<ModifierType> getCompatibleModifiers(UINodeType nodeType) {
    std::vector<ModifierType> compatible;
    
    // جميع المعدّلات الأساسية متوافقة مع معظم العقد
    std::vector<ModifierType> universal = {
        ModifierType::Padding,
        ModifierType::Margin,
        ModifierType::BackgroundColor,
        ModifierType::Border,
        ModifierType::BorderRadius,
        ModifierType::Shadow,
        ModifierType::Opacity,
        ModifierType::Frame,
        ModifierType::Position,
        ModifierType::Offset,
        ModifierType::Rotation,
        ModifierType::Scale,
        ModifierType::Hidden,
        ModifierType::Disabled,
        ModifierType::Animation,
        ModifierType::Transition,
        ModifierType::Accessibility,
        ModifierType::Tag
    };
    
    compatible.insert(compatible.end(), universal.begin(), universal.end());
    
    // إضافة معدّلات خاصة بنوع العقدة
    switch (nodeType) {
        case UINodeType::Text:
        case UINodeType::TextField:
        case UINodeType::TextArea:
            compatible.push_back(ModifierType::ForegroundColor);
            compatible.push_back(ModifierType::FontSize);
            compatible.push_back(ModifierType::FontWeight);
            compatible.push_back(ModifierType::FontFamily);
            compatible.push_back(ModifierType::FontStyle);
            compatible.push_back(ModifierType::LineSpacing);
            break;
            
        case UINodeType::Button:
            compatible.push_back(ModifierType::ForegroundColor);
            compatible.push_back(ModifierType::TintColor);
            compatible.push_back(ModifierType::OnTap);
            break;
            
        case UINodeType::Image:
        case UINodeType::Icon:
            compatible.push_back(ModifierType::TintColor);
            compatible.push_back(ModifierType::ClipShape);
            compatible.push_back(ModifierType::Width);
            compatible.push_back(ModifierType::Height);
            break;
            
        case UINodeType::Column:
        case UINodeType::Row:
        case UINodeType::Stack:
            compatible.push_back(ModifierType::Alignment);
            break;
            
        default:
            break;
    }
    
    return compatible;
}

} // namespace parser
} // namespace sad
