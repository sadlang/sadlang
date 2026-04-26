/**
 * ==========================================================================
 * ملف: android_compose.hpp
 * الوصف: مولد كود Jetpack Compose من شجرة واجهة لغة ص
 * الإصدار: 1.0.0
 * ==========================================================================
 * 
 * هذا الملف يحتوي على نظام تحويل شجرة واجهة لغة ص (UINode)
 * إلى كود Kotlin مع Jetpack Compose.
 * 
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║                      تدفق توليد الكود                                     ║
 * ╠══════════════════════════════════════════════════════════════════════════╣
 * ║                                                                          ║
 * ║  ┌─────────────┐   ┌─────────────────┐   ┌─────────────────────────────┐ ║
 * ║  │  UINode     │ → │ ComposeCodeGen  │ → │ Kotlin/Compose Code         │ ║
 * ║  │  (شجرة ص)   │   │ (المولد)        │   │ @Composable fun ...         │ ║
 * ║  └─────────────┘   └─────────────────┘   └─────────────────────────────┘ ║
 * ║                                                                          ║
 * ║  عمود                                     Column {                       ║
 * ║      نص("مرحباً")        ═══════>             Text("مرحباً")              ║
 * ║          .لون(.أزرق)                              color = Color.Blue     ║
 * ║      زر("OK") { ... }                        Button(...) { ... }         ║
 * ║  نهاية                                          }                            ║
 * ║                                                                          ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 * 
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#ifndef SAD_ANDROID_COMPOSE_HPP
#define SAD_ANDROID_COMPOSE_HPP

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <functional>

// تضمين محلل الواجهة
#include "../../shared/parser/include/ui_parser.hpp"

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════
// القسم الأول: إعدادات مولد Compose
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief إعدادات توليد كود Compose
 */
struct ComposeCodeGenConfig {
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
    
    /// اسم الحزمة (Package Name)
    std::string packageName = "com.example.app";
    
    /// إضافة تعليقات توثيقية
    bool generateDocComments = true;
    
    /// إضافة تعليقات عربية
    bool includeArabicComments = true;
    
    /// توليد Preview functions
    bool generatePreviews = true;
    
    /// استخدام Material 3
    bool useMaterial3 = true;
    
    // ═══════════════════════════════════════════
    // إعدادات الحالة
    // ═══════════════════════════════════════════
    
    /// استخدام ViewModel
    bool useViewModel = false;
    
    /// استخدام remember للحالة
    bool useRememberForState = true;
    
    /// استخدام mutableStateOf
    bool useMutableStateOf = true;
    
    // ═══════════════════════════════════════════
    // إعدادات RTL
    // ═══════════════════════════════════════════
    
    /// دعم اتجاه RTL تلقائياً
    bool enableRTLSupport = true;
    
    /// استخدام CompositionLocalProvider للاتجاه
    bool wrapWithLayoutDirection = true;
};

/**
 * @brief نتيجة توليد الكود
 */
struct ComposeCodeGenResult {
    /// هل نجح التوليد؟
    bool success = false;
    
    /// كود Kotlin المُولَّد
    std::string kotlinCode;
    
    /// الاستيرادات المطلوبة
    std::vector<std::string> imports;
    
    /// الأخطاء (إن وُجدت)
    std::vector<std::string> errors;
    
    /// التحذيرات
    std::vector<std::string> warnings;
    
    /// إحصائيات التوليد
    struct {
        int totalComponents = 0;
        int totalModifiers = 0;
        int totalLines = 0;
    } stats;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثاني: خريطة التحويل
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief معلومات تحويل مكون
 */
struct ComposeComponentMapping {
    /// اسم الدالة في Compose
    std::string composableName;
    
    /// الاستيرادات المطلوبة
    std::vector<std::string> requiredImports;
    
    /// هل يقبل محتوى (children)؟
    bool acceptsContent = false;
    
    /// هل يتطلب معاملات معينة؟
    std::vector<std::string> requiredParams;
    
    /// قالب الكود (اختياري)
    std::string codeTemplate;
};

/**
 * @brief معلومات تحويل معدّل
 */
struct ComposeModifierMapping {
    /// اسم الدالة في Modifier
    std::string modifierName;
    
    /// الاستيرادات المطلوبة
    std::vector<std::string> requiredImports;
    
    /// دالة تحويل القيمة
    std::function<std::string(const parser::PropertyValue&)> valueTransformer;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثالث: مولد الكود الرئيسي
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief مولد كود Jetpack Compose
 * 
 * يحول شجرة واجهة لغة ص إلى كود Kotlin مع Jetpack Compose.
 * 
 * @code
 * // الاستخدام:
 * ComposeCodeGen codegen(config);
 * 
 * auto result = codegen.generateFromView(viewDef);
 * 
 * if (result.success) {
 *     std::cout << result.kotlinCode << std::endl;
 * }
 * @endcode
 */
class ComposeCodeGen {
public:
    /**
     * @brief إنشاء مولد جديد
     */
    explicit ComposeCodeGen(const ComposeCodeGenConfig& config = ComposeCodeGenConfig{});
    
    ~ComposeCodeGen();
    
    // ═══════════════════════════════════════════
    // دوال التوليد الرئيسية
    // ═══════════════════════════════════════════
    
    /**
     * @brief توليد كود من تعريف واجهة كاملة
     * 
     * يُنتج ملف Kotlin كامل مع:
     * - الاستيرادات
     * - @Composable function
     * - Preview (اختياري)
     * 
     * @param viewDef تعريف الواجهة
     * @return نتيجة التوليد
     */
    ComposeCodeGenResult generateFromView(const parser::ViewDefinition& viewDef);
    
    /**
     * @brief توليد كود من عقدة واجهة واحدة
     * 
     * يُنتج كود Compose لمكون واحد فقط.
     * 
     * @param node عقدة الواجهة
     * @return نتيجة التوليد
     */
    ComposeCodeGenResult generateFromNode(const parser::UINodePtr& node);
    
    /**
     * @brief توليد ملف Kotlin كامل
     * 
     * يُنتج ملف .kt كامل جاهز للاستخدام.
     * 
     * @param viewDef تعريف الواجهة
     * @param className اسم الصنف (اختياري)
     * @return كود Kotlin كامل
     */
    std::string generateKotlinFile(
        const parser::ViewDefinition& viewDef,
        const std::string& className = ""
    );
    
    // ═══════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════
    
    /**
     * @brief الحصول على الاستيرادات المطلوبة
     */
    std::vector<std::string> getRequiredImports(const parser::UINodePtr& tree);
    
    /**
     * @brief تنسيق كود Kotlin
     */
    std::string formatKotlinCode(const std::string& code);
    
    /**
     * @brief تحويل لون عربي إلى Compose Color
     */
    static std::string colorToCompose(const parser::ColorValue& color);
    
    /**
     * @brief تحويل بُعد إلى Compose Dp/Sp
     */
    static std::string dimensionToCompose(const parser::DimensionValue& dim, bool isFont = false);

private:
    ComposeCodeGenConfig config_;
    std::stringstream output_;
    int currentIndent_ = 0;
    std::vector<std::string> collectedImports_;
    int componentCount_ = 0;
    int modifierCount_ = 0;
    
    // ═══════════════════════════════════════════
    // دوال التوليد الداخلية
    // ═══════════════════════════════════════════
    
    /// توليد كود لعقدة
    void generateNode(const parser::UINodePtr& node);
    
    /// توليد كود المعدّلات
    std::string generateModifiers(const std::vector<parser::UIModifier>& modifiers);
    
    /// توليد كود لمكون نصي
    void generateText(const parser::UINodePtr& node);
    
    /// توليد كود لزر
    void generateButton(const parser::UINodePtr& node);
    
    /// توليد كود لصورة
    void generateImage(const parser::UINodePtr& node);
    
    /// توليد كود لعمود
    void generateColumn(const parser::UINodePtr& node);
    
    /// توليد كود لصف
    void generateRow(const parser::UINodePtr& node);
    
    /// توليد كود لقائمة
    void generateList(const parser::UINodePtr& node);
    
    /// توليد كود لشبكة
    void generateGrid(const parser::UINodePtr& node);
    
    /// توليد كود لبطاقة
    void generateCard(const parser::UINodePtr& node);
    
    /// توليد كود للتنقل
    void generateNavigation(const parser::UINodePtr& node);
    
    /// توليد كود لحقل نص
    void generateTextField(const parser::UINodePtr& node);
    
    /// توليد كود لمكون عام
    void generateGenericComponent(const parser::UINodePtr& node);
    
    // ═══════════════════════════════════════════
    // دوال التنسيق
    // ═══════════════════════════════════════════
    
    /// إضافة سطر مع إزاحة
    void writeLine(const std::string& line = "");
    
    /// زيادة الإزاحة
    void indent();
    
    /// إنقاص الإزاحة
    void dedent();
    
    /// الحصول على نص الإزاحة الحالية
    std::string getIndent() const;
    
    /// إضافة استيراد
    void addImport(const std::string& import);
    
    /// إضافة تعليق عربي
    void writeArabicComment(const std::string& comment);
    
    // ═══════════════════════════════════════════
    // خرائط التحويل الثابتة
    // ═══════════════════════════════════════════
    
    static const std::unordered_map<parser::UINodeType, ComposeComponentMapping> componentMappings_;
    static const std::unordered_map<parser::ModifierType, ComposeModifierMapping> modifierMappings_;
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الرابع: دوال مساعدة عامة
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief تحويل اسم عربي إلى اسم Kotlin صالح
 */
std::string arabicToKotlinIdentifier(const std::string& arabicName);

/**
 * @brief تحويل نص عربي للاستخدام في String resources
 */
std::string arabicTextToStringResource(const std::string& text, const std::string& resourceId);

/**
 * @brief التحقق من أن المكون يتطلب ViewModel
 */
bool requiresViewModel(const parser::UINodePtr& tree);

/**
 * @brief استخراج جميع النصوص للترجمة
 */
std::vector<std::pair<std::string, std::string>> extractStringsForLocalization(
    const parser::UINodePtr& tree
);

} // namespace codegen
} // namespace sad

#endif // SAD_ANDROID_COMPOSE_HPP
