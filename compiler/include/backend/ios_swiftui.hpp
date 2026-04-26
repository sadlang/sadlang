/**
 * ==========================================================================
 * ملف: ios_swiftui.hpp
 * الوصف: مولد كود SwiftUI من شجرة واجهة لغة ص
 * الإصدار: 1.0.0
 * ==========================================================================
 * 
 * هذا الملف يحتوي على نظام تحويل شجرة واجهة لغة ص (UINode)
 * إلى كود Swift مع SwiftUI.
 * 
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║                      تدفق توليد الكود                                     ║
 * ╠══════════════════════════════════════════════════════════════════════════╣
 * ║                                                                          ║
 * ║  ┌─────────────┐   ┌─────────────────┐   ┌─────────────────────────────┐ ║
 * ║  │  UINode     │ → │ SwiftUICodeGen  │ → │ Swift/SwiftUI Code          │ ║
 * ║  │  (شجرة ص)   │   │ (المولد)        │   │ struct ... : View           │ ║
 * ║  └─────────────┘   └─────────────────┘   └─────────────────────────────┘ ║
 * ║                                                                          ║
 * ║  عمود {                                    VStack {                       ║
 * ║      نص("مرحباً")        ═══════>             Text("مرحباً")              ║
 * ║          .لون(.أزرق)                              .foregroundColor(.blue)║
 * ║      زر("OK") { ... }                        Button("OK") { ... }        ║
 * ║  }                                          }                            ║
 * ║                                                                          ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 * 
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#ifndef SAD_IOS_SWIFTUI_HPP
#define SAD_IOS_SWIFTUI_HPP

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <functional>

// تضمين محلل الواجهة
#include "ui_parser.hpp"

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════
// القسم الأول: إعدادات مولد SwiftUI
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief إعدادات توليد كود SwiftUI
 */
struct SwiftUICodeGenConfig {
    // ═══════════════════════════════════════════
    // إعدادات التنسيق
    // ═══════════════════════════════════════════
    
    /// عدد المسافات للإزاحة
    int indentSize = 4;
    
    /// استخدام tabs بدلاً من spaces
    bool useTabs = false;
    
    /// إضافة أسطر فارغة بين المكونات
    bool addBlankLinesBetweenComponents = true;
    
    // ═══════════════════════════════════════════
    // إعدادات الكود المُولَّد
    // ═══════════════════════════════════════════
    
    /// إضافة تعليقات توثيقية
    bool generateDocComments = true;
    
    /// إضافة تعليقات عربية
    bool includeArabicComments = true;
    
    /// توليد Preview provider
    bool generatePreviews = true;
    
    /// استخدام iOS 17+ APIs
    bool useModernAPIs = true;
    
    // ═══════════════════════════════════════════
    // إعدادات الحالة
    // ═══════════════════════════════════════════
    
    /// استخدام @StateObject للكائنات المعقدة
    bool useStateObject = true;
    
    /// استخدام @Observable (iOS 17+)
    bool useObservable = false;
    
    // ═══════════════════════════════════════════
    // إعدادات RTL
    // ═══════════════════════════════════════════
    
    /// دعم اتجاه RTL تلقائياً
    bool enableRTLSupport = true;
    
    /// تعيين الاتجاه صراحة
    bool setExplicitLayoutDirection = true;
    
    // ═══════════════════════════════════════════
    // إعدادات الهدف
    // ═══════════════════════════════════════════
    
    /// الحد الأدنى لإصدار iOS
    std::string minimumIOSVersion = "15.0";
    
    /// دعم macOS Catalyst
    bool supportMacCatalyst = false;
};

/**
 * @brief نتيجة توليد كود SwiftUI
 */
struct SwiftUICodeGenResult {
    /// هل نجح التوليد؟
    bool success = false;
    
    /// كود Swift المُولَّد
    std::string swiftCode;
    
    /// الاستيرادات المطلوبة
    std::vector<std::string> imports;
    
    /// الأخطاء (إن وُجدت)
    std::vector<std::string> errors;
    
    /// التحذيرات
    std::vector<std::string> warnings;
    
    /// إحصائيات التوليد
    struct {
        int totalViews = 0;
        int totalModifiers = 0;
        int totalLines = 0;
    } stats;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثاني: خريطة التحويل
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief معلومات تحويل View
 */
struct SwiftUIViewMapping {
    /// اسم الـ View في SwiftUI
    std::string viewName;
    
    /// هل يقبل محتوى (children)؟
    bool acceptsContent = false;
    
    /// الاستيرادات الإضافية المطلوبة
    std::vector<std::string> additionalImports;
    
    /// قالب الكود (اختياري)
    std::string codeTemplate;
};

/**
 * @brief معلومات تحويل Modifier
 */
struct SwiftUIModifierMapping {
    /// اسم الـ modifier في SwiftUI
    std::string modifierName;
    
    /// هل يحتاج لتحويل القيمة؟
    bool needsValueTransform = false;
    
    /// دالة تحويل القيمة (اختياري)
    std::function<std::string(const parser::PropertyValue&)> valueTransformer;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثالث: مولد الكود الرئيسي
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief مولد كود SwiftUI
 * 
 * يحول شجرة واجهة لغة ص إلى كود Swift مع SwiftUI.
 * 
 * @code
 * // الاستخدام:
 * SwiftUICodeGen codegen(config);
 * 
 * auto result = codegen.generateFromView(viewDef);
 * 
 * if (result.success) {
 *     std::cout << result.swiftCode << std::endl;
 * }
 * @endcode
 */
class SwiftUICodeGen {
public:
    /**
     * @brief إنشاء مولد جديد
     */
    explicit SwiftUICodeGen(const SwiftUICodeGenConfig& config = SwiftUICodeGenConfig{});
    
    ~SwiftUICodeGen();
    
    // ═══════════════════════════════════════════
    // دوال التوليد الرئيسية
    // ═══════════════════════════════════════════
    
    /**
     * @brief توليد كود من تعريف واجهة كاملة
     */
    SwiftUICodeGenResult generateFromView(const parser::ViewDefinition& viewDef);
    
    /**
     * @brief توليد كود من عقدة واجهة واحدة
     */
    SwiftUICodeGenResult generateFromNode(const parser::UINodePtr& node);
    
    /**
     * @brief توليد ملف Swift كامل
     */
    std::string generateSwiftFile(
        const parser::ViewDefinition& viewDef,
        const std::string& structName = ""
    );
    
    // ═══════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════
    
    /**
     * @brief تحويل لون عربي إلى SwiftUI Color
     */
    static std::string colorToSwiftUI(const parser::ColorValue& color);
    
    /**
     * @brief تحويل بُعد إلى SwiftUI
     */
    static std::string dimensionToSwiftUI(const parser::DimensionValue& dim, bool isFont = false);

private:
    SwiftUICodeGenConfig config_;
    std::stringstream output_;
    int currentIndent_ = 0;
    std::vector<std::string> collectedImports_;
    int viewCount_ = 0;
    int modifierCount_ = 0;
    
    // ═══════════════════════════════════════════
    // دوال التوليد الداخلية
    // ═══════════════════════════════════════════
    
    /// توليد كود لعقدة
    void generateNode(const parser::UINodePtr& node);
    
    /// توليد سلسلة المعدّلات
    void generateModifiers(const std::vector<parser::UIModifier>& modifiers);
    
    /// توليد Text
    void generateText(const parser::UINodePtr& node);
    
    /// توليد Button
    void generateButton(const parser::UINodePtr& node);
    
    /// توليد Image
    void generateImage(const parser::UINodePtr& node);
    
    /// توليد VStack
    void generateVStack(const parser::UINodePtr& node);
    
    /// توليد HStack
    void generateHStack(const parser::UINodePtr& node);
    
    /// توليد List
    void generateList(const parser::UINodePtr& node);
    
    /// توليد LazyVGrid
    void generateGrid(const parser::UINodePtr& node);
    
    /// توليد NavigationView/NavigationStack
    void generateNavigation(const parser::UINodePtr& node);
    
    /// توليد TextField
    void generateTextField(const parser::UINodePtr& node);
    
    /// توليد View عام
    void generateGenericView(const parser::UINodePtr& node);
    
    // ═══════════════════════════════════════════
    // دوال التنسيق
    // ═══════════════════════════════════════════
    
    void write(const std::string& text);
    void writeLine(const std::string& line = "");
    void indent();
    void dedent();
    std::string getIndent() const;
    void addImport(const std::string& import);
    void writeArabicComment(const std::string& comment);
    
    // ═══════════════════════════════════════════
    // خرائط التحويل الثابتة
    // ═══════════════════════════════════════════
    
    static const std::unordered_map<parser::UINodeType, SwiftUIViewMapping> viewMappings_;
    static const std::unordered_map<parser::ModifierType, SwiftUIModifierMapping> modifierMappings_;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الرابع: دوال مساعدة عامة
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تحويل اسم عربي إلى اسم Swift صالح
 */
std::string arabicToSwiftIdentifier(const std::string& arabicName);

/**
 * @brief التحقق من توافق الإصدار
 */
bool isFeatureAvailable(const std::string& feature, const std::string& minVersion);

} // namespace codegen
} // namespace sad

#endif // SAD_IOS_SWIFTUI_HPP
