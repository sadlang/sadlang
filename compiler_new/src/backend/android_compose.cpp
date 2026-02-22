/**
 * ==========================================================================
 * ملف: android_compose.cpp
 * الوصف: تنفيذ مولد كود Jetpack Compose من شجرة واجهة لغة ص
 * الإصدار: 1.0.0
 * ==========================================================================
 * 
 * هذا الملف يحتوي على التنفيذ الكامل لمولد كود Compose الذي يحول
 * شجرة واجهة لغة ص إلى كود Kotlin/Jetpack Compose.
 * 
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║                         أمثلة التحويل                                     ║
 * ╠══════════════════════════════════════════════════════════════════════════╣
 * ║                                                                          ║
 * ║  نص("مرحباً").لون(.أزرق)                                                 ║
 * ║  ════════════════════════════════════════>                               ║
 * ║  Text(                                                                   ║
 * ║      text = "مرحباً",                                                    ║
 * ║      color = Color.Blue                                                  ║
 * ║  )                                                                       ║
 * ║                                                                          ║
 * ║  عمود(توسيط: صحيح) { نص("١") نص("٢") }                                   ║
 * ║  ════════════════════════════════════════>                               ║
 * ║  Column(                                                                 ║
 * ║      horizontalAlignment = Alignment.CenterHorizontally                  ║
 * ║  ) {                                                                     ║
 * ║      Text("١")                                                           ║
 * ║      Text("٢")                                                           ║
 * ║  }                                                                       ║
 * ║                                                                          ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 * 
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#include "android_compose.hpp"
#include <algorithm>
#include <regex>
#include <iomanip>

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════
// دوال الأمان / Security Functions
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief Escape string for Kotlin string literal (prevents code injection)
 * تهريب النص لاستخدامه كنص Kotlin (يمنع حقن الكود)
 * 
 * @param input النص المُدخل / Input string
 * @return النص المُهرَّب / Escaped string
 */
inline std::string escapeKotlinString(const std::string& input) {
    std::string result;
    result.reserve(input.length() * 2);  // Reserve extra space
    
    for (char c : input) {
        switch (c) {
            case '\\': result += "\\\\"; break;  // Backslash
            case '"':  result += "\\\""; break;  // Double quote
            case '\'': result += "\\'"; break;   // Single quote
            case '\n': result += "\\n"; break;   // Newline
            case '\r': result += "\\r"; break;   // Carriage return
            case '\t': result += "\\t"; break;   // Tab
            case '\b': result += "\\b"; break;   // Backspace
            case '\f': result += "\\f"; break;   // Form feed
            case '$':  result += "\\$"; break;   // Dollar sign (Kotlin string template)
            default:
                // Check for control characters
                if (static_cast<unsigned char>(c) < 32) {
                    // Use unicode escape for control chars
                    char hex[8];
                    std::snprintf(hex, sizeof(hex), "\\u%04x", static_cast<unsigned char>(c));
                    result += hex;
                } else {
                    result += c;
                }
        }
    }
    return result;
}

/**
 * @brief Create a safe Kotlin string literal from raw text
 * إنشاء نص Kotlin آمن من النص الخام
 */
inline std::string toKotlinStringLiteral(const std::string& input) {
    return "\"" + escapeKotlinString(input) + "\"";
}

// ═══════════════════════════════════════════════════════════════════════════
// القسم الأول: خرائط التحويل الثابتة
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief خريطة تحويل مكونات ص إلى Compose
 * 
 * هذه الخريطة تحتوي على تعريفات كيفية تحويل كل مكون عربي
 * إلى مكافئه في Jetpack Compose.
 */
const std::unordered_map<parser::UINodeType, ComposeComponentMapping> 
ComposeCodeGen::componentMappings_ = {
    
    // ═══════════════════════════════════════════
    // مكونات العرض الأساسية
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Text, {
        "Text",
        {"androidx.compose.material3.Text", "androidx.compose.ui.graphics.Color"},
        false,
        {},
        ""
    }},
    
    {parser::UINodeType::Image, {
        "Image",
        {"androidx.compose.foundation.Image", "androidx.compose.ui.res.painterResource"},
        false,
        {"painter", "contentDescription"},
        ""
    }},
    
    {parser::UINodeType::Icon, {
        "Icon",
        {"androidx.compose.material3.Icon", "androidx.compose.material.icons.Icons"},
        false,
        {"imageVector", "contentDescription"},
        ""
    }},
    
    // ═══════════════════════════════════════════
    // مكونات الإدخال
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Button, {
        "Button",
        {"androidx.compose.material3.Button", "androidx.compose.material3.Text"},
        true,
        {"onClick"},
        ""
    }},
    
    {parser::UINodeType::TextField, {
        "TextField",
        {"androidx.compose.material3.TextField", "androidx.compose.runtime.mutableStateOf"},
        false,
        {"value", "onValueChange"},
        ""
    }},
    
    {parser::UINodeType::TextArea, {
        "TextField",
        {"androidx.compose.material3.TextField"},
        false,
        {"value", "onValueChange", "maxLines"},
        ""
    }},
    
    {parser::UINodeType::Toggle, {
        "Switch",
        {"androidx.compose.material3.Switch"},
        false,
        {"checked", "onCheckedChange"},
        ""
    }},
    
    {parser::UINodeType::Slider, {
        "Slider",
        {"androidx.compose.material3.Slider"},
        false,
        {"value", "onValueChange"},
        ""
    }},
    
    {parser::UINodeType::Picker, {
        "DropdownMenu",
        {"androidx.compose.material3.DropdownMenu", "androidx.compose.material3.DropdownMenuItem"},
        true,
        {},
        ""
    }},
    
    // ═══════════════════════════════════════════
    // مكونات التخطيط
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Column, {
        "Column",
        {"androidx.compose.foundation.layout.Column", "androidx.compose.ui.Alignment"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::Row, {
        "Row",
        {"androidx.compose.foundation.layout.Row", "androidx.compose.ui.Alignment"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::Stack, {
        "Box",
        {"androidx.compose.foundation.layout.Box"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::Grid, {
        "LazyVerticalGrid",
        {"androidx.compose.foundation.lazy.grid.LazyVerticalGrid", 
         "androidx.compose.foundation.lazy.grid.GridCells"},
        true,
        {"columns"},
        ""
    }},
    
    {parser::UINodeType::LazyColumn, {
        "LazyColumn",
        {"androidx.compose.foundation.lazy.LazyColumn"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::LazyRow, {
        "LazyRow",
        {"androidx.compose.foundation.lazy.LazyRow"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::Spacer, {
        "Spacer",
        {"androidx.compose.foundation.layout.Spacer"},
        false,
        {},
        ""
    }},
    
    {parser::UINodeType::Divider, {
        "Divider",
        {"androidx.compose.material3.Divider"},
        false,
        {},
        ""
    }},
    
    // ═══════════════════════════════════════════
    // مكونات الحاويات
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Container, {
        "Box",
        {"androidx.compose.foundation.layout.Box"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::Card, {
        "Card",
        {"androidx.compose.material3.Card"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::Surface, {
        "Surface",
        {"androidx.compose.material3.Surface"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::Scaffold, {
        "Scaffold",
        {"androidx.compose.material3.Scaffold"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::Box, {
        "Box",
        {"androidx.compose.foundation.layout.Box"},
        true,
        {},
        ""
    }},
    
    // ═══════════════════════════════════════════
    // مكونات التنقل
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::NavigationView, {
        "NavHost",
        {"androidx.navigation.compose.NavHost", "androidx.navigation.compose.rememberNavController"},
        true,
        {"navController", "startDestination"},
        ""
    }},
    
    {parser::UINodeType::TabView, {
        "TabRow",
        {"androidx.compose.material3.TabRow", "androidx.compose.material3.Tab"},
        true,
        {"selectedTabIndex"},
        ""
    }},
    
    // ═══════════════════════════════════════════
    // مكونات القوائم
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::List, {
        "LazyColumn",
        {"androidx.compose.foundation.lazy.LazyColumn", "androidx.compose.foundation.lazy.items"},
        true,
        {},
        ""
    }},
    
    {parser::UINodeType::ForEach, {
        "items",
        {"androidx.compose.foundation.lazy.items"},
        true,
        {},
        ""
    }},
    
    // ═══════════════════════════════════════════
    // مكونات الحوارات
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Alert, {
        "AlertDialog",
        {"androidx.compose.material3.AlertDialog"},
        true,
        {"onDismissRequest"},
        ""
    }},
    
    {parser::UINodeType::Dialog, {
        "Dialog",
        {"androidx.compose.ui.window.Dialog"},
        true,
        {"onDismissRequest"},
        ""
    }},
    
    {parser::UINodeType::Sheet, {
        "ModalBottomSheet",
        {"androidx.compose.material3.ModalBottomSheet"},
        true,
        {"onDismissRequest"},
        ""
    }},
    
    {parser::UINodeType::Menu, {
        "DropdownMenu",
        {"androidx.compose.material3.DropdownMenu"},
        true,
        {"expanded", "onDismissRequest"},
        ""
    }}
};

/**
 * @brief خريطة تحويل معدّلات ص إلى Compose Modifier
 */
const std::unordered_map<parser::ModifierType, ComposeModifierMapping>
ComposeCodeGen::modifierMappings_ = {
    
    // ═══════════════════════════════════════════
    // معدّلات الألوان
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::BackgroundColor, {
        "background",
        {"androidx.compose.foundation.background"},
        nullptr
    }},
    
    // ═══════════════════════════════════════════
    // معدّلات الحجم
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Width, {
        "width",
        {"androidx.compose.foundation.layout.width"},
        nullptr
    }},
    
    {parser::ModifierType::Height, {
        "height",
        {"androidx.compose.foundation.layout.height"},
        nullptr
    }},
    
    {parser::ModifierType::MinWidth, {
        "widthIn",
        {"androidx.compose.foundation.layout.widthIn"},
        nullptr
    }},
    
    {parser::ModifierType::MinHeight, {
        "heightIn",
        {"androidx.compose.foundation.layout.heightIn"},
        nullptr
    }},
    
    // ═══════════════════════════════════════════
    // معدّلات التباعد
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Padding, {
        "padding",
        {"androidx.compose.foundation.layout.padding"},
        nullptr
    }},
    
    // ═══════════════════════════════════════════
    // معدّلات الحدود
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Border, {
        "border",
        {"androidx.compose.foundation.border"},
        nullptr
    }},
    
    {parser::ModifierType::BorderRadius, {
        "clip",
        {"androidx.compose.foundation.shape.RoundedCornerShape", "androidx.compose.ui.draw.clip"},
        nullptr
    }},
    
    // ═══════════════════════════════════════════
    // معدّلات الظل والعتامة
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Shadow, {
        "shadow",
        {"androidx.compose.ui.draw.shadow"},
        nullptr
    }},
    
    {parser::ModifierType::Opacity, {
        "alpha",
        {"androidx.compose.ui.draw.alpha"},
        nullptr
    }},
    
    {parser::ModifierType::Blur, {
        "blur",
        {"androidx.compose.ui.draw.blur"},
        nullptr
    }},
    
    // ═══════════════════════════════════════════
    // معدّلات التحويل
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Rotation, {
        "rotate",
        {"androidx.compose.ui.draw.rotate"},
        nullptr
    }},
    
    {parser::ModifierType::Scale, {
        "scale",
        {"androidx.compose.ui.draw.scale"},
        nullptr
    }},
    
    // ═══════════════════════════════════════════
    // معدّلات التفاعل
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::OnTap, {
        "clickable",
        {"androidx.compose.foundation.clickable"},
        nullptr
    }},
    
    {parser::ModifierType::OnLongPress, {
        "combinedClickable",
        {"androidx.compose.foundation.combinedClickable"},
        nullptr
    }}
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثاني: تنفيذ المنشئ
// ═══════════════════════════════════════════════════════════════════════════

ComposeCodeGen::ComposeCodeGen(const ComposeCodeGenConfig& config)
    : config_(config) {
    // إضافة الاستيرادات الأساسية
    addImport("androidx.compose.runtime.Composable");
    
    if (config_.useMaterial3) {
        addImport("androidx.compose.material3.*");
    }
    
    if (config_.useRememberForState) {
        addImport("androidx.compose.runtime.remember");
        addImport("androidx.compose.runtime.mutableStateOf");
        addImport("androidx.compose.runtime.getValue");
        addImport("androidx.compose.runtime.setValue");
    }
}

ComposeCodeGen::~ComposeCodeGen() = default;


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثالث: تنفيذ دوال التوليد الرئيسية
// ═══════════════════════════════════════════════════════════════════════════

ComposeCodeGenResult ComposeCodeGen::generateFromView(const parser::ViewDefinition& viewDef) {
    ComposeCodeGenResult result;
    
    // إعادة تهيئة الحالة
    output_.str("");
    output_.clear();
    currentIndent_ = 0;
    componentCount_ = 0;
    modifierCount_ = 0;
    
    try {
        // ═══════════════════════════════════════════
        // توليد الدالة @Composable
        // ═══════════════════════════════════════════
        
        // تعليق عربي
        if (config_.includeArabicComments) {
            writeArabicComment("واجهة " + viewDef.name);
        }
        
        // توثيق KDoc
        if (config_.generateDocComments) {
            writeLine("/**");
            writeLine(" * " + viewDef.name);
            writeLine(" * تم توليده تلقائياً من كود لغة ص");
            writeLine(" */");
        }
        
        // توقيع الدالة
        writeLine("@Composable");
        writeLine("fun " + arabicToKotlinIdentifier(viewDef.name) + "() {");
        indent();
        
        // ═══════════════════════════════════════════
        // توليد متغيرات الحالة
        // ═══════════════════════════════════════════
        
        for (const auto& binding : viewDef.stateBindings) {
            switch (binding.type) {
                case parser::StateBindingType::State:
                    if (config_.useRememberForState) {
                        writeLine("var " + binding.name + " by remember { mutableStateOf(" +
                                 (binding.initialValue.empty() ? "null" : binding.initialValue) + 
                                 ") }");
                    } else {
                        writeLine("var " + binding.name + " = mutableStateOf(" +
                                 (binding.initialValue.empty() ? "null" : binding.initialValue) + 
                                 ")");
                    }
                    break;
                    
                case parser::StateBindingType::Observed:
                    writeLine("val " + binding.name + " = remember { " + binding.typeName + "() }");
                    break;
                    
                default:
                    break;
            }
        }
        
        // سطر فارغ بعد الحالة
        if (!viewDef.stateBindings.empty()) {
            writeLine();
        }
        
        // ═══════════════════════════════════════════
        // دعم RTL
        // ═══════════════════════════════════════════
        
        if (config_.enableRTLSupport && config_.wrapWithLayoutDirection) {
            addImport("androidx.compose.runtime.CompositionLocalProvider");
            addImport("androidx.compose.ui.platform.LocalLayoutDirection");
            addImport("androidx.compose.ui.unit.LayoutDirection");
            
            writeLine("CompositionLocalProvider(LocalLayoutDirection provides LayoutDirection.Rtl) {");
            indent();
        }
        
        // ═══════════════════════════════════════════
        // توليد شجرة الواجهة
        // ═══════════════════════════════════════════
        
        if (viewDef.bodyTree) {
            generateNode(viewDef.bodyTree);
        }
        
        // إغلاق RTL wrapper
        if (config_.enableRTLSupport && config_.wrapWithLayoutDirection) {
            dedent();
            writeLine("}");
        }
        
        // إغلاق الدالة
        dedent();
        writeLine("}");
        
        // ═══════════════════════════════════════════
        // توليد Preview
        // ═══════════════════════════════════════════
        
        if (config_.generatePreviews) {
            writeLine();
            addImport("androidx.compose.ui.tooling.preview.Preview");
            
            writeLine("@Preview(showBackground = true)");
            writeLine("@Composable");
            writeLine("fun " + arabicToKotlinIdentifier(viewDef.name) + "Preview() {");
            indent();
            writeLine(arabicToKotlinIdentifier(viewDef.name) + "()");
            dedent();
            writeLine("}");
        }
        
        // النجاح
        result.success = true;
        result.kotlinCode = output_.str();
        result.imports = collectedImports_;
        result.stats.totalComponents = componentCount_;
        result.stats.totalModifiers = modifierCount_;
        result.stats.totalLines = static_cast<int>(
            std::count(result.kotlinCode.begin(), result.kotlinCode.end(), '\n')
        );
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("خطأ في التوليد: ") + e.what());
    }
    
    return result;
}

ComposeCodeGenResult ComposeCodeGen::generateFromNode(const parser::UINodePtr& node) {
    ComposeCodeGenResult result;
    
    output_.str("");
    output_.clear();
    currentIndent_ = 0;
    componentCount_ = 0;
    modifierCount_ = 0;
    
    try {
        if (node) {
            generateNode(node);
        }
        
        result.success = true;
        result.kotlinCode = output_.str();
        result.imports = collectedImports_;
        result.stats.totalComponents = componentCount_;
        result.stats.totalModifiers = modifierCount_;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("خطأ: ") + e.what());
    }
    
    return result;
}

std::string ComposeCodeGen::generateKotlinFile(
    const parser::ViewDefinition& viewDef,
    const std::string& className
) {
    std::stringstream file;
    
    // توليد الكود أولاً للحصول على الاستيرادات
    auto codeResult = generateFromView(viewDef);
    
    // ═══════════════════════════════════════════
    // رأس الملف
    // ═══════════════════════════════════════════
    
    file << "/**" << std::endl;
    file << " * تم توليد هذا الملف تلقائياً بواسطة مترجم لغة ص" << std::endl;
    file << " * الملف المصدر: " << viewDef.sourceFile << std::endl;
    file << " * لا تقم بتعديل هذا الملف يدوياً!" << std::endl;
    file << " */" << std::endl;
    file << std::endl;
    
    // ═══════════════════════════════════════════
    // الحزمة
    // ═══════════════════════════════════════════
    
    file << "package " << config_.packageName << std::endl;
    file << std::endl;
    
    // ═══════════════════════════════════════════
    // الاستيرادات
    // ═══════════════════════════════════════════
    
    // إزالة التكرار وترتيب الاستيرادات
    std::vector<std::string> sortedImports = codeResult.imports;
    std::sort(sortedImports.begin(), sortedImports.end());
    sortedImports.erase(
        std::unique(sortedImports.begin(), sortedImports.end()),
        sortedImports.end()
    );
    
    for (const auto& import : sortedImports) {
        file << "import " << import << std::endl;
    }
    file << std::endl;
    
    // ═══════════════════════════════════════════
    // الكود
    // ═══════════════════════════════════════════
    
    file << codeResult.kotlinCode;
    
    return file.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الرابع: تنفيذ دوال توليد العقد
// ═══════════════════════════════════════════════════════════════════════════

void ComposeCodeGen::generateNode(const parser::UINodePtr& node) {
    if (!node) return;
    
    componentCount_++;
    
    // البحث عن التعريف في الخريطة
    auto mappingIt = componentMappings_.find(node->type);
    
    if (mappingIt == componentMappings_.end()) {
        // مكون غير معروف، استخدام الاسم مباشرة
        generateGenericComponent(node);
        return;
    }
    
    const auto& mapping = mappingIt->second;
    
    // إضافة الاستيرادات المطلوبة
    for (const auto& import : mapping.requiredImports) {
        addImport(import);
    }
    
    // توليد حسب نوع المكون
    switch (node->type) {
        case parser::UINodeType::Text:
            generateText(node);
            break;
            
        case parser::UINodeType::Button:
            generateButton(node);
            break;
            
        case parser::UINodeType::Image:
            generateImage(node);
            break;
            
        case parser::UINodeType::Column:
            generateColumn(node);
            break;
            
        case parser::UINodeType::Row:
            generateRow(node);
            break;
            
        case parser::UINodeType::List:
        case parser::UINodeType::LazyColumn:
            generateList(node);
            break;
            
        case parser::UINodeType::Grid:
        case parser::UINodeType::LazyGrid:
            generateGrid(node);
            break;
            
        case parser::UINodeType::Card:
            generateCard(node);
            break;
            
        case parser::UINodeType::TextField:
            generateTextField(node);
            break;
            
        case parser::UINodeType::NavigationView:
            generateNavigation(node);
            break;
            
        default:
            generateGenericComponent(node);
            break;
    }
}

void ComposeCodeGen::generateText(const parser::UINodePtr& node) {
    std::string text = "\"\"";
    
    // استخراج نص المحتوى / Extract text content
    if (!node->constructorArgs.empty()) {
        if (std::holds_alternative<std::string>(node->constructorArgs[0])) {
            // استخدام دالة التهريب الآمنة / Use safe escape function
            text = toKotlinStringLiteral(std::get<std::string>(node->constructorArgs[0]));
        }
    }
    
    // بدء Text
    writeLine("Text(");
    indent();
    writeLine("text = " + text + ",");
    
    // معالجة المعدّلات الخاصة بالنص
    for (const auto& mod : node->modifiers) {
        modifierCount_++;
        
        switch (mod.type) {
            case parser::ModifierType::ForegroundColor:
                if (!mod.arguments.empty() && 
                    std::holds_alternative<parser::ColorValue>(mod.arguments[0])) {
                    writeLine("color = " + colorToCompose(
                        std::get<parser::ColorValue>(mod.arguments[0])) + ",");
                }
                break;
                
            case parser::ModifierType::FontSize:
                if (!mod.arguments.empty() && std::holds_alternative<double>(mod.arguments[0])) {
                    addImport("androidx.compose.ui.unit.sp");
                    writeLine("fontSize = " + 
                             std::to_string(static_cast<int>(std::get<double>(mod.arguments[0]))) + 
                             ".sp,");
                }
                break;
                
            case parser::ModifierType::FontWeight:
                addImport("androidx.compose.ui.text.font.FontWeight");
                writeLine("fontWeight = FontWeight.Bold,"); // مبسط
                break;
                
            default:
                break;
        }
    }
    
    // Modifier chain للمعدّلات الأخرى
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    writeLine(")");
}

void ComposeCodeGen::generateButton(const parser::UINodePtr& node) {
    std::string buttonText = "\"\"";
    
    if (!node->constructorArgs.empty()) {
        if (std::holds_alternative<std::string>(node->constructorArgs[0])) {
            // استخدام دالة التهريب الآمنة / Use safe escape function
            buttonText = toKotlinStringLiteral(std::get<std::string>(node->constructorArgs[0]));
        }
    }
    
    // بدء Button
    writeLine("Button(");
    indent();
    
    // onClick handler
    writeLine("onClick = {");
    indent();
    if (!node->actionClosure.empty()) {
        // تهريب تعليق الإجراء أيضاً / Also escape action comment
        writeLine("// " + escapeKotlinString(node->actionClosure));
    }
    dedent();
    writeLine("},");
    
    // Modifier
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    writeLine(") {");
    
    // محتوى الزر
    indent();
    writeLine("Text(" + buttonText + ")");
    dedent();
    
    writeLine("}");
}

void ComposeCodeGen::generateImage(const parser::UINodePtr& node) {
    writeLine("Image(");
    indent();
    
    // مصدر الصورة
    if (!node->constructorArgs.empty() && 
        std::holds_alternative<std::string>(node->constructorArgs[0])) {
        std::string imageSource = std::get<std::string>(node->constructorArgs[0]);
        writeLine("painter = painterResource(id = R.drawable." + imageSource + "),");
    } else {
        writeLine("painter = painterResource(id = R.drawable.placeholder),");
    }
    
    writeLine("contentDescription = null,");
    
    // Modifier
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    writeLine(")");
}

void ComposeCodeGen::generateColumn(const parser::UINodePtr& node) {
    writeLine("Column(");
    indent();
    
    // معالجة الوسائط المسماة
    auto centeringIt = node->namedArgs.find("توسيط");
    if (centeringIt != node->namedArgs.end()) {
        writeLine("horizontalAlignment = Alignment.CenterHorizontally,");
    }
    
    // Modifier
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    writeLine(") {");
    
    // الأبناء
    indent();
    for (const auto& child : node->children) {
        generateNode(child);
        if (config_.addBlankLinesBetweenComponents) {
            writeLine();
        }
    }
    dedent();
    
    writeLine("}");
}

void ComposeCodeGen::generateRow(const parser::UINodePtr& node) {
    writeLine("Row(");
    indent();
    
    // معالجة الوسائط المسماة
    auto centeringIt = node->namedArgs.find("توسيط");
    if (centeringIt != node->namedArgs.end()) {
        writeLine("verticalAlignment = Alignment.CenterVertically,");
    }
    
    // Modifier
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    writeLine(") {");
    
    // الأبناء
    indent();
    for (const auto& child : node->children) {
        generateNode(child);
    }
    dedent();
    
    writeLine("}");
}

void ComposeCodeGen::generateList(const parser::UINodePtr& node) {
    addImport("androidx.compose.foundation.lazy.items");
    
    writeLine("LazyColumn(");
    indent();
    
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    writeLine(") {");
    indent();
    
    // إذا كان هناك ForEach
    if (!node->children.empty()) {
        for (const auto& child : node->children) {
            if (child->type == parser::UINodeType::ForEach) {
                writeLine("items(items) { item ->");
                indent();
                for (const auto& forEachChild : child->children) {
                    generateNode(forEachChild);
                }
                dedent();
                writeLine("}");
            } else {
                writeLine("item {");
                indent();
                generateNode(child);
                dedent();
                writeLine("}");
            }
        }
    }
    
    dedent();
    writeLine("}");
}

void ComposeCodeGen::generateGrid(const parser::UINodePtr& node) {
    addImport("androidx.compose.foundation.lazy.grid.LazyVerticalGrid");
    addImport("androidx.compose.foundation.lazy.grid.GridCells");
    
    int columns = 2; // افتراضي
    auto columnsIt = node->namedArgs.find("أعمدة");
    if (columnsIt != node->namedArgs.end() && 
        std::holds_alternative<int64_t>(columnsIt->second)) {
        columns = static_cast<int>(std::get<int64_t>(columnsIt->second));
    }
    
    writeLine("LazyVerticalGrid(");
    indent();
    writeLine("columns = GridCells.Fixed(" + std::to_string(columns) + "),");
    
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    writeLine(") {");
    indent();
    
    for (const auto& child : node->children) {
        writeLine("item {");
        indent();
        generateNode(child);
        dedent();
        writeLine("}");
    }
    
    dedent();
    writeLine("}");
}

void ComposeCodeGen::generateCard(const parser::UINodePtr& node) {
    writeLine("Card(");
    indent();
    
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    writeLine(") {");
    indent();
    
    for (const auto& child : node->children) {
        generateNode(child);
    }
    
    dedent();
    writeLine("}");
}

void ComposeCodeGen::generateTextField(const parser::UINodePtr& node) {
    writeLine("var textValue by remember { mutableStateOf(\"\") }");
    writeLine("TextField(");
    indent();
    
    writeLine("value = textValue,");
    writeLine("onValueChange = { textValue = it },");
    
    // placeholder
    if (!node->constructorArgs.empty() && 
        std::holds_alternative<std::string>(node->constructorArgs[0])) {
        std::string placeholder = std::get<std::string>(node->constructorArgs[0]);
        writeLine("placeholder = { Text(\"" + placeholder + "\") },");
    }
    
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    writeLine(")");
}

void ComposeCodeGen::generateNavigation(const parser::UINodePtr& node) {
    addImport("androidx.navigation.compose.NavHost");
    addImport("androidx.navigation.compose.composable");
    addImport("androidx.navigation.compose.rememberNavController");
    
    writeLine("val navController = rememberNavController()");
    writeLine("NavHost(navController = navController, startDestination = \"home\") {");
    indent();
    
    for (const auto& child : node->children) {
        writeLine("composable(\"" + child->arabicName + "\") {");
        indent();
        generateNode(child);
        dedent();
        writeLine("}");
    }
    
    dedent();
    writeLine("}");
}

void ComposeCodeGen::generateGenericComponent(const parser::UINodePtr& node) {
    auto mappingIt = componentMappings_.find(node->type);
    std::string composeName = mappingIt != componentMappings_.end() 
        ? mappingIt->second.composableName 
        : arabicToKotlinIdentifier(node->arabicName);
    
    bool hasContent = node->isContainer() && !node->children.empty();
    
    writeLine(composeName + "(");
    indent();
    
    std::string modifierChain = generateModifiers(node->modifiers);
    if (!modifierChain.empty()) {
        writeLine("modifier = " + modifierChain);
    }
    
    dedent();
    
    if (hasContent) {
        writeLine(") {");
        indent();
        for (const auto& child : node->children) {
            generateNode(child);
        }
        dedent();
        writeLine("}");
    } else {
        writeLine(")");
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الخامس: تنفيذ دوال المعدّلات
// ═══════════════════════════════════════════════════════════════════════════

std::string ComposeCodeGen::generateModifiers(const std::vector<parser::UIModifier>& modifiers) {
    if (modifiers.empty()) return "";
    
    std::stringstream ss;
    ss << "Modifier";
    
    for (const auto& mod : modifiers) {
        modifierCount_++;
        
        auto mappingIt = modifierMappings_.find(mod.type);
        if (mappingIt == modifierMappings_.end()) continue;
        
        const auto& mapping = mappingIt->second;
        
        // إضافة الاستيرادات
        for (const auto& import : mapping.requiredImports) {
            addImport(import);
        }
        
        ss << "\n" << getIndent() << "    ." << mapping.modifierName << "(";
        
        // تحويل القيمة
        if (!mod.arguments.empty()) {
            const auto& arg = mod.arguments[0];
            
            if (std::holds_alternative<parser::ColorValue>(arg)) {
                ss << colorToCompose(std::get<parser::ColorValue>(arg));
            } else if (std::holds_alternative<parser::DimensionValue>(arg)) {
                ss << dimensionToCompose(std::get<parser::DimensionValue>(arg));
            } else if (std::holds_alternative<double>(arg)) {
                addImport("androidx.compose.ui.unit.dp");
                ss << static_cast<int>(std::get<double>(arg)) << ".dp";
            } else if (std::holds_alternative<std::string>(arg)) {
                ss << std::get<std::string>(arg);
            }
        }
        
        ss << ")";
    }
    
    return ss.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم السادس: تنفيذ دوال التنسيق
// ═══════════════════════════════════════════════════════════════════════════

void ComposeCodeGen::writeLine(const std::string& line) {
    output_ << getIndent() << line << std::endl;
}

void ComposeCodeGen::indent() {
    currentIndent_++;
}

void ComposeCodeGen::dedent() {
    if (currentIndent_ > 0) {
        currentIndent_--;
    }
}

std::string ComposeCodeGen::getIndent() const {
    if (config_.useTabs) {
        return std::string(currentIndent_, '\t');
    }
    return std::string(currentIndent_ * config_.indentSize, ' ');
}

void ComposeCodeGen::addImport(const std::string& import) {
    if (std::find(collectedImports_.begin(), collectedImports_.end(), import) 
        == collectedImports_.end()) {
        collectedImports_.push_back(import);
    }
}

void ComposeCodeGen::writeArabicComment(const std::string& comment) {
    writeLine("// " + comment);
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم السابع: تنفيذ دوال التحويل الثابتة
// ═══════════════════════════════════════════════════════════════════════════

std::string ComposeCodeGen::colorToCompose(const parser::ColorValue& color) {
    // خريطة الألوان العربية إلى Compose
    static const std::unordered_map<std::string, std::string> colorMap = {
        {"أحمر", "Color.Red"},
        {"أخضر", "Color.Green"},
        {"أزرق", "Color.Blue"},
        {"أصفر", "Color.Yellow"},
        {"برتقالي", "Color(0xFFFFA500)"},
        {"بنفسجي", "Color.Magenta"},
        {"وردي", "Color(0xFFFFC0CB)"},
        {"بني", "Color(0xFFA52A2A)"},
        {"أبيض", "Color.White"},
        {"أسود", "Color.Black"},
        {"رمادي", "Color.Gray"},
        {"رمادي_فاتح", "Color.LightGray"},
        {"رمادي_غامق", "Color.DarkGray"},
        {"أساسي", "MaterialTheme.colorScheme.primary"},
        {"ثانوي", "MaterialTheme.colorScheme.secondary"},
        {"شفاف", "Color.Transparent"}
    };
    
    switch (color.type) {
        case parser::ColorValue::Type::Named: {
            auto it = colorMap.find(color.namedColor);
            if (it != colorMap.end()) {
                return it->second;
            }
            return "Color.Unspecified";
        }
        
        case parser::ColorValue::Type::RGB:
            return "Color(" + std::to_string(color.r) + ", " + 
                   std::to_string(color.g) + ", " + std::to_string(color.b) + ")";
        
        case parser::ColorValue::Type::RGBA:
            return "Color(" + std::to_string(color.r) + ", " + 
                   std::to_string(color.g) + ", " + std::to_string(color.b) + 
                   ", " + std::to_string(color.alpha) + "f)";
        
        case parser::ColorValue::Type::Hex:
            return "Color(0xFF" + color.hexValue.substr(1) + ")";
    }
    
    return "Color.Unspecified";
}

std::string ComposeCodeGen::dimensionToCompose(const parser::DimensionValue& dim, bool isFont) {
    std::string unit = isFont ? ".sp" : ".dp";
    
    switch (dim.unit) {
        case parser::DimensionValue::Unit::Points:
            return std::to_string(static_cast<int>(dim.value)) + unit;
            
        case parser::DimensionValue::Unit::Percent:
            return "fillMaxWidth(" + std::to_string(dim.value / 100.0f) + "f)";
            
        case parser::DimensionValue::Unit::Fill:
            return "fillMaxSize()";
            
        case parser::DimensionValue::Unit::Fit:
            return "wrapContentSize()";
            
        case parser::DimensionValue::Unit::Auto:
            return "wrapContentSize()";
    }
    
    return "0.dp";
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثامن: تنفيذ الدوال المساعدة العامة
// ═══════════════════════════════════════════════════════════════════════════

std::string arabicToKotlinIdentifier(const std::string& arabicName) {
    // قائمة بسيطة للتحويل
    static const std::unordered_map<std::string, std::string> translations = {
        {"شاشة_رئيسية", "HomeScreen"},
        {"شاشة_تفاصيل", "DetailScreen"},
        {"شاشة_إعدادات", "SettingsScreen"},
        {"شاشة_ملف", "ProfileScreen"},
        {"قائمة_رئيسية", "MainList"},
        {"زر_رئيسي", "MainButton"},
        {"عداد", "counter"},
        {"اسم", "name"},
        {"عمر", "age"},
        {"بريد", "email"}
    };
    
    auto it = translations.find(arabicName);
    if (it != translations.end()) {
        return it->second;
    }
    
    // إذا لم يُعثر عليه، استخدم transliteration بسيط
    // (في الإنتاج، يجب استخدام خوارزمية تحويل حقيقية)
    return "Generated_" + std::to_string(std::hash<std::string>{}(arabicName) % 10000);
}

std::string arabicTextToStringResource(const std::string& text, const std::string& resourceId) {
    return "stringResource(R.string." + resourceId + ")";
}

bool requiresViewModel(const parser::UINodePtr& tree) {
    if (!tree) return false;
    
    // التحقق من وجود حالة معقدة
    for (const auto& child : tree->children) {
        if (requiresViewModel(child)) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::pair<std::string, std::string>> extractStringsForLocalization(
    const parser::UINodePtr& tree
) {
    std::vector<std::pair<std::string, std::string>> strings;
    
    if (!tree) return strings;
    
    // استخراج النصوص من وسائط المنشئ
    for (const auto& arg : tree->constructorArgs) {
        if (std::holds_alternative<std::string>(arg)) {
            std::string text = std::get<std::string>(arg);
            std::string resourceId = "str_" + std::to_string(
                std::hash<std::string>{}(text) % 100000
            );
            strings.push_back({resourceId, text});
        }
    }
    
    // البحث في الأبناء
    for (const auto& child : tree->children) {
        auto childStrings = extractStringsForLocalization(child);
        strings.insert(strings.end(), childStrings.begin(), childStrings.end());
    }
    
    return strings;
}

} // namespace codegen
} // namespace sad
