/**
 * ==========================================================================
 * ملف: ios_swiftui.cpp
 * الوصف: تنفيذ مولد كود SwiftUI من شجرة واجهة لغة ص
 * الإصدار: 1.0.0
 * ==========================================================================
 * 
 * هذا الملف يحتوي على التنفيذ الكامل لمولد كود SwiftUI الذي يحول
 * شجرة واجهة لغة ص إلى كود Swift/SwiftUI.
 * 
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║                         أمثلة التحويل                                     ║
 * ╠══════════════════════════════════════════════════════════════════════════╣
 * ║                                                                          ║
 * ║  نص("مرحباً").لون(.أزرق).حجم_خط(٢٤)                                       ║
 * ║  ════════════════════════════════════════>                               ║
 * ║  Text("مرحباً")                                                          ║
 * ║      .foregroundColor(.blue)                                             ║
 * ║      .font(.system(size: 24))                                            ║
 * ║                                                                          ║
 * ║  عمود { نص("أ") نص("ب") }                                                 ║
 * ║  ════════════════════════════════════════>                               ║
 * ║  VStack {                                                                ║
 * ║      Text("أ")                                                           ║
 * ║      Text("ب")                                                           ║
 * ║  }                                                                       ║
 * ║                                                                          ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 * 
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#include "ios_swiftui.hpp"
#include <algorithm>
#include <regex>
#include <iomanip>

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════
// القسم الأول: خرائط التحويل الثابتة
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief خريطة تحويل مكونات ص إلى SwiftUI Views
 */
const std::unordered_map<parser::UINodeType, SwiftUIViewMapping>
SwiftUICodeGen::viewMappings_ = {
    
    // ═══════════════════════════════════════════
    // مكونات العرض الأساسية
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Text, {"Text", false, {}, ""}},
    {parser::UINodeType::Image, {"Image", false, {}, ""}},
    {parser::UINodeType::Icon, {"Image", false, {}, ""}}, // systemName
    
    // ═══════════════════════════════════════════
    // مكونات الإدخال
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Button, {"Button", true, {}, ""}},
    {parser::UINodeType::TextField, {"TextField", false, {}, ""}},
    {parser::UINodeType::TextArea, {"TextEditor", false, {}, ""}},
    {parser::UINodeType::Toggle, {"Toggle", true, {}, ""}},
    {parser::UINodeType::Slider, {"Slider", false, {}, ""}},
    {parser::UINodeType::Picker, {"Picker", true, {}, ""}},
    {parser::UINodeType::DatePicker, {"DatePicker", false, {}, ""}},
    {parser::UINodeType::ColorPicker, {"ColorPicker", false, {}, ""}},
    
    // ═══════════════════════════════════════════
    // مكونات التخطيط
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Column, {"VStack", true, {}, ""}},
    {parser::UINodeType::Row, {"HStack", true, {}, ""}},
    {parser::UINodeType::Stack, {"ZStack", true, {}, ""}},
    {parser::UINodeType::Grid, {"LazyVGrid", true, {}, ""}},
    {parser::UINodeType::LazyColumn, {"LazyVStack", true, {}, ""}},
    {parser::UINodeType::LazyRow, {"LazyHStack", true, {}, ""}},
    {parser::UINodeType::LazyGrid, {"LazyVGrid", true, {}, ""}},
    {parser::UINodeType::Spacer, {"Spacer", false, {}, ""}},
    {parser::UINodeType::Divider, {"Divider", false, {}, ""}},
    
    // ═══════════════════════════════════════════
    // مكونات الحاويات
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Container, {"Group", true, {}, ""}},
    {parser::UINodeType::Card, {"GroupBox", true, {}, ""}}, // أو custom Card
    {parser::UINodeType::Surface, {"Group", true, {}, ""}},
    {parser::UINodeType::Box, {"ZStack", true, {}, ""}},
    
    // ═══════════════════════════════════════════
    // مكونات التنقل
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::NavigationView, {"NavigationStack", true, {}, ""}}, // iOS 16+
    {parser::UINodeType::NavigationLink, {"NavigationLink", true, {}, ""}},
    {parser::UINodeType::TabView, {"TabView", true, {}, ""}},
    {parser::UINodeType::TabItem, {"Tab", true, {}, ""}},
    
    // ═══════════════════════════════════════════
    // مكونات القوائم
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::List, {"List", true, {}, ""}},
    {parser::UINodeType::Section, {"Section", true, {}, ""}},
    {parser::UINodeType::ForEach, {"ForEach", true, {}, ""}},
    
    // ═══════════════════════════════════════════
    // مكونات الحوارات
    // ═══════════════════════════════════════════
    
    {parser::UINodeType::Alert, {"alert", false, {}, ""}}, // modifier
    {parser::UINodeType::Sheet, {"sheet", false, {}, ""}}, // modifier
    {parser::UINodeType::Dialog, {"confirmationDialog", false, {}, ""}},
    {parser::UINodeType::Menu, {"Menu", true, {}, ""}}
};

/**
 * @brief خريطة تحويل معدّلات ص إلى SwiftUI Modifiers
 */
const std::unordered_map<parser::ModifierType, SwiftUIModifierMapping>
SwiftUICodeGen::modifierMappings_ = {
    
    // ═══════════════════════════════════════════
    // معدّلات الألوان
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::ForegroundColor, {"foregroundColor", false, nullptr}},
    {parser::ModifierType::BackgroundColor, {"background", false, nullptr}},
    {parser::ModifierType::TintColor, {"tint", false, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات الخط
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::FontSize, {"font", true, nullptr}},
    {parser::ModifierType::FontWeight, {"fontWeight", false, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات الحجم
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Width, {"frame", true, nullptr}},
    {parser::ModifierType::Height, {"frame", true, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات التباعد
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Padding, {"padding", false, nullptr}},
    {parser::ModifierType::PaddingTop, {"padding", true, nullptr}},
    {parser::ModifierType::PaddingBottom, {"padding", true, nullptr}},
    {parser::ModifierType::PaddingLeading, {"padding", true, nullptr}},
    {parser::ModifierType::PaddingTrailing, {"padding", true, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات الحدود
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Border, {"border", true, nullptr}},
    {parser::ModifierType::BorderRadius, {"cornerRadius", false, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات الظل والعتامة
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Shadow, {"shadow", true, nullptr}},
    {parser::ModifierType::Opacity, {"opacity", false, nullptr}},
    {parser::ModifierType::Blur, {"blur", false, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات التحويل
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Rotation, {"rotationEffect", true, nullptr}},
    {parser::ModifierType::Scale, {"scaleEffect", false, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات التفاعل
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::OnTap, {"onTapGesture", true, nullptr}},
    {parser::ModifierType::OnLongPress, {"onLongPressGesture", true, nullptr}},
    {parser::ModifierType::OnAppear, {"onAppear", true, nullptr}},
    {parser::ModifierType::OnDisappear, {"onDisappear", true, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات الحالة
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Disabled, {"disabled", false, nullptr}},
    {parser::ModifierType::Hidden, {"hidden", false, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات الرسوم المتحركة
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::Animation, {"animation", true, nullptr}},
    {parser::ModifierType::Transition, {"transition", true, nullptr}},
    
    // ═══════════════════════════════════════════
    // معدّلات متنوعة
    // ═══════════════════════════════════════════
    
    {parser::ModifierType::ClipShape, {"clipShape", true, nullptr}},
    {parser::ModifierType::Accessibility, {"accessibilityLabel", true, nullptr}},
    {parser::ModifierType::Tag, {"tag", false, nullptr}}
};


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثاني: تنفيذ المنشئ
// ═══════════════════════════════════════════════════════════════════════════

SwiftUICodeGen::SwiftUICodeGen(const SwiftUICodeGenConfig& config)
    : config_(config) {
    addImport("SwiftUI");
}

SwiftUICodeGen::~SwiftUICodeGen() = default;


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثالث: تنفيذ دوال التوليد الرئيسية
// ═══════════════════════════════════════════════════════════════════════════

SwiftUICodeGenResult SwiftUICodeGen::generateFromView(const parser::ViewDefinition& viewDef) {
    SwiftUICodeGenResult result;
    
    // إعادة تهيئة الحالة
    output_.str("");
    output_.clear();
    currentIndent_ = 0;
    viewCount_ = 0;
    modifierCount_ = 0;
    
    try {
        // ═══════════════════════════════════════════
        // توليد الـ struct
        // ═══════════════════════════════════════════
        
        // تعليق عربي
        if (config_.includeArabicComments) {
            writeArabicComment("واجهة " + viewDef.name);
        }
        
        // توثيق
        if (config_.generateDocComments) {
            writeLine("/// " + viewDef.name);
            writeLine("/// تم توليده تلقائياً من كود لغة ص");
        }
        
        // تعريف الـ struct
        std::string structName = arabicToSwiftIdentifier(viewDef.name);
        writeLine("struct " + structName + ": View {");
        indent();
        
        // ═══════════════════════════════════════════
        // توليد متغيرات الحالة
        // ═══════════════════════════════════════════
        
        for (const auto& binding : viewDef.stateBindings) {
            switch (binding.type) {
                case parser::StateBindingType::State:
                    writeLine("@State private var " + binding.name + ": " + 
                             binding.typeName + 
                             (binding.initialValue.empty() ? "" : " = " + binding.initialValue));
                    break;
                    
                case parser::StateBindingType::Binding:
                    writeLine("@Binding var " + binding.name + ": " + binding.typeName);
                    break;
                    
                case parser::StateBindingType::Observed:
                    if (config_.useObservable) {
                        writeLine("@Observable var " + binding.name + ": " + binding.typeName);
                    } else {
                        writeLine("@ObservedObject var " + binding.name + ": " + binding.typeName);
                    }
                    break;
                    
                case parser::StateBindingType::Environment:
                    writeLine("@Environment(\\." + binding.name + ") var " + binding.name);
                    break;
                    
                case parser::StateBindingType::EnvironmentObject:
                    writeLine("@EnvironmentObject var " + binding.name + ": " + binding.typeName);
                    break;
                    
                default:
                    break;
            }
        }
        
        // سطر فارغ بعد الخصائص
        if (!viewDef.stateBindings.empty()) {
            writeLine();
        }
        
        // ═══════════════════════════════════════════
        // توليد body
        // ═══════════════════════════════════════════
        
        writeLine("var body: some View {");
        indent();
        
        // دعم RTL
        if (config_.enableRTLSupport && config_.setExplicitLayoutDirection) {
            writeLine("Group {");
            indent();
        }
        
        // توليد شجرة الواجهة
        if (viewDef.bodyTree) {
            generateNode(viewDef.bodyTree);
        } else {
            writeLine("EmptyView()");
        }
        
        // إغلاق RTL wrapper مع modifier
        if (config_.enableRTLSupport && config_.setExplicitLayoutDirection) {
            dedent();
            writeLine("}");
            writeLine(".environment(\\.layoutDirection, .rightToLeft)");
        }
        
        dedent();
        writeLine("}");
        
        // إغلاق الـ struct
        dedent();
        writeLine("}");
        
        // ═══════════════════════════════════════════
        // توليد Preview
        // ═══════════════════════════════════════════
        
        if (config_.generatePreviews) {
            writeLine();
            writeLine("#Preview {");
            indent();
            writeLine(structName + "()");
            dedent();
            writeLine("}");
        }
        
        // النجاح
        result.success = true;
        result.swiftCode = output_.str();
        result.imports = collectedImports_;
        result.stats.totalViews = viewCount_;
        result.stats.totalModifiers = modifierCount_;
        result.stats.totalLines = static_cast<int>(
            std::count(result.swiftCode.begin(), result.swiftCode.end(), '\n')
        );
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("خطأ في التوليد: ") + e.what());
    }
    
    return result;
}

SwiftUICodeGenResult SwiftUICodeGen::generateFromNode(const parser::UINodePtr& node) {
    SwiftUICodeGenResult result;
    
    output_.str("");
    output_.clear();
    currentIndent_ = 0;
    viewCount_ = 0;
    modifierCount_ = 0;
    
    try {
        if (node) {
            generateNode(node);
        }
        
        result.success = true;
        result.swiftCode = output_.str();
        result.imports = collectedImports_;
        result.stats.totalViews = viewCount_;
        result.stats.totalModifiers = modifierCount_;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back(std::string("خطأ: ") + e.what());
    }
    
    return result;
}

std::string SwiftUICodeGen::generateSwiftFile(
    const parser::ViewDefinition& viewDef,
    const std::string& structName
) {
    std::stringstream file;
    
    // توليد الكود أولاً
    auto codeResult = generateFromView(viewDef);
    
    // ═══════════════════════════════════════════
    // رأس الملف
    // ═══════════════════════════════════════════
    
    file << "//\n";
    file << "//  " << (structName.empty() ? arabicToSwiftIdentifier(viewDef.name) : structName) << ".swift\n";
    file << "//  تم توليده تلقائياً بواسطة مترجم لغة ص\n";
    file << "//  الملف المصدر: " << viewDef.sourceFile << "\n";
    file << "//\n";
    file << "//  ⚠️ لا تقم بتعديل هذا الملف يدوياً!\n";
    file << "//\n\n";
    
    // ═══════════════════════════════════════════
    // الاستيرادات
    // ═══════════════════════════════════════════
    
    std::vector<std::string> sortedImports = codeResult.imports;
    std::sort(sortedImports.begin(), sortedImports.end());
    sortedImports.erase(
        std::unique(sortedImports.begin(), sortedImports.end()),
        sortedImports.end()
    );
    
    for (const auto& import : sortedImports) {
        file << "import " << import << "\n";
    }
    file << "\n";
    
    // ═══════════════════════════════════════════
    // الكود
    // ═══════════════════════════════════════════
    
    file << codeResult.swiftCode;
    
    return file.str();
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الرابع: تنفيذ دوال توليد العقد
// ═══════════════════════════════════════════════════════════════════════════

void SwiftUICodeGen::generateNode(const parser::UINodePtr& node) {
    if (!node) return;
    
    viewCount_++;
    
    // البحث عن التعريف في الخريطة
    auto mappingIt = viewMappings_.find(node->type);
    
    if (mappingIt == viewMappings_.end()) {
        generateGenericView(node);
        return;
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
        case parser::UINodeType::Icon:
            generateImage(node);
            break;
            
        case parser::UINodeType::Column:
        case parser::UINodeType::LazyColumn:
            generateVStack(node);
            break;
            
        case parser::UINodeType::Row:
        case parser::UINodeType::LazyRow:
            generateHStack(node);
            break;
            
        case parser::UINodeType::List:
            generateList(node);
            break;
            
        case parser::UINodeType::Grid:
        case parser::UINodeType::LazyGrid:
            generateGrid(node);
            break;
            
        case parser::UINodeType::NavigationView:
            generateNavigation(node);
            break;
            
        case parser::UINodeType::TextField:
            generateTextField(node);
            break;
            
        default:
            generateGenericView(node);
            break;
    }
}

void SwiftUICodeGen::generateText(const parser::UINodePtr& node) {
    std::string text = "\"\"";
    
    if (!node->constructorArgs.empty()) {
        if (std::holds_alternative<std::string>(node->constructorArgs[0])) {
            text = "\"" + std::get<std::string>(node->constructorArgs[0]) + "\"";
        }
    }
    
    write("Text(" + text + ")");
    
    // المعدّلات
    generateModifiers(node->modifiers);
    
    writeLine();
}

void SwiftUICodeGen::generateButton(const parser::UINodePtr& node) {
    std::string buttonText = "\"\"";
    
    if (!node->constructorArgs.empty()) {
        if (std::holds_alternative<std::string>(node->constructorArgs[0])) {
            buttonText = "\"" + std::get<std::string>(node->constructorArgs[0]) + "\"";
        }
    }
    
    writeLine("Button(" + buttonText + ") {");
    indent();
    
    // action
    if (!node->actionClosure.empty()) {
        writeLine("// " + node->actionClosure);
    } else {
        writeLine("// TODO: إضافة الإجراء");
    }
    
    dedent();
    write("}");
    
    generateModifiers(node->modifiers);
    
    writeLine();
}

void SwiftUICodeGen::generateImage(const parser::UINodePtr& node) {
    bool isSystemImage = (node->type == parser::UINodeType::Icon);
    std::string imageName = "placeholder";
    
    if (!node->constructorArgs.empty()) {
        if (std::holds_alternative<std::string>(node->constructorArgs[0])) {
            imageName = std::get<std::string>(node->constructorArgs[0]);
        }
    }
    
    if (isSystemImage) {
        write("Image(systemName: \"" + imageName + "\")");
    } else {
        write("Image(\"" + imageName + "\")");
    }
    
    generateModifiers(node->modifiers);
    
    writeLine();
}

void SwiftUICodeGen::generateVStack(const parser::UINodePtr& node) {
    std::string alignment = "";
    std::string spacing = "";
    
    // معالجة الوسائط المسماة
    auto centeringIt = node->namedArgs.find("توسيط");
    if (centeringIt != node->namedArgs.end()) {
        alignment = "alignment: .center";
    }
    
    auto spacingIt = node->namedArgs.find("تباعد");
    if (spacingIt != node->namedArgs.end() && 
        std::holds_alternative<double>(spacingIt->second)) {
        spacing = "spacing: " + std::to_string(
            static_cast<int>(std::get<double>(spacingIt->second))
        );
    }
    
    // بناء الوسائط
    std::string params = "";
    if (!alignment.empty() || !spacing.empty()) {
        params = "(";
        if (!alignment.empty()) params += alignment;
        if (!alignment.empty() && !spacing.empty()) params += ", ";
        if (!spacing.empty()) params += spacing;
        params += ")";
    }
    
    bool isLazy = (node->type == parser::UINodeType::LazyColumn);
    std::string viewName = isLazy ? "LazyVStack" : "VStack";
    
    writeLine(viewName + params + " {");
    indent();
    
    for (const auto& child : node->children) {
        generateNode(child);
        if (config_.addBlankLinesBetweenComponents) {
            writeLine();
        }
    }
    
    dedent();
    write("}");
    
    generateModifiers(node->modifiers);
    
    writeLine();
}

void SwiftUICodeGen::generateHStack(const parser::UINodePtr& node) {
    std::string alignment = "";
    std::string spacing = "";
    
    auto centeringIt = node->namedArgs.find("توسيط");
    if (centeringIt != node->namedArgs.end()) {
        alignment = "alignment: .center";
    }
    
    auto spacingIt = node->namedArgs.find("تباعد");
    if (spacingIt != node->namedArgs.end() && 
        std::holds_alternative<double>(spacingIt->second)) {
        spacing = "spacing: " + std::to_string(
            static_cast<int>(std::get<double>(spacingIt->second))
        );
    }
    
    std::string params = "";
    if (!alignment.empty() || !spacing.empty()) {
        params = "(";
        if (!alignment.empty()) params += alignment;
        if (!alignment.empty() && !spacing.empty()) params += ", ";
        if (!spacing.empty()) params += spacing;
        params += ")";
    }
    
    bool isLazy = (node->type == parser::UINodeType::LazyRow);
    std::string viewName = isLazy ? "LazyHStack" : "HStack";
    
    writeLine(viewName + params + " {");
    indent();
    
    for (const auto& child : node->children) {
        generateNode(child);
    }
    
    dedent();
    write("}");
    
    generateModifiers(node->modifiers);
    
    writeLine();
}

void SwiftUICodeGen::generateList(const parser::UINodePtr& node) {
    writeLine("List {");
    indent();
    
    for (const auto& child : node->children) {
        if (child->type == parser::UINodeType::ForEach) {
            writeLine("ForEach(items, id: \\.self) { item in");
            indent();
            for (const auto& forEachChild : child->children) {
                generateNode(forEachChild);
            }
            dedent();
            writeLine("}");
        } else if (child->type == parser::UINodeType::Section) {
            std::string sectionTitle = "";
            if (!child->constructorArgs.empty() && 
                std::holds_alternative<std::string>(child->constructorArgs[0])) {
                sectionTitle = std::get<std::string>(child->constructorArgs[0]);
            }
            
            writeLine("Section(\"" + sectionTitle + "\") {");
            indent();
            for (const auto& sectionChild : child->children) {
                generateNode(sectionChild);
            }
            dedent();
            writeLine("}");
        } else {
            generateNode(child);
        }
    }
    
    dedent();
    write("}");
    
    generateModifiers(node->modifiers);
    
    writeLine();
}

void SwiftUICodeGen::generateGrid(const parser::UINodePtr& node) {
    int columns = 2;
    auto columnsIt = node->namedArgs.find("أعمدة");
    if (columnsIt != node->namedArgs.end() && 
        std::holds_alternative<int64_t>(columnsIt->second)) {
        columns = static_cast<int>(std::get<int64_t>(columnsIt->second));
    }
    
    writeLine("let columns = Array(repeating: GridItem(.flexible()), count: " + 
              std::to_string(columns) + ")");
    writeLine("LazyVGrid(columns: columns) {");
    indent();
    
    for (const auto& child : node->children) {
        generateNode(child);
    }
    
    dedent();
    write("}");
    
    generateModifiers(node->modifiers);
    
    writeLine();
}

void SwiftUICodeGen::generateNavigation(const parser::UINodePtr& node) {
    // iOS 16+ uses NavigationStack, earlier uses NavigationView
    std::string navView = config_.useModernAPIs ? "NavigationStack" : "NavigationView";
    
    writeLine(navView + " {");
    indent();
    
    for (const auto& child : node->children) {
        generateNode(child);
    }
    
    dedent();
    write("}");
    
    generateModifiers(node->modifiers);
    
    writeLine();
}

void SwiftUICodeGen::generateTextField(const parser::UINodePtr& node) {
    std::string placeholder = "";
    
    if (!node->constructorArgs.empty() && 
        std::holds_alternative<std::string>(node->constructorArgs[0])) {
        placeholder = std::get<std::string>(node->constructorArgs[0]);
    }
    
    // نحتاج متغير للربط
    writeLine("@State private var textValue = \"\"");
    write("TextField(\"" + placeholder + "\", text: $textValue)");
    
    generateModifiers(node->modifiers);
    
    writeLine();
}

void SwiftUICodeGen::generateGenericView(const parser::UINodePtr& node) {
    auto mappingIt = viewMappings_.find(node->type);
    std::string viewName = mappingIt != viewMappings_.end()
        ? mappingIt->second.viewName
        : arabicToSwiftIdentifier(node->arabicName);
    
    bool hasContent = node->isContainer() && !node->children.empty();
    
    if (hasContent) {
        writeLine(viewName + " {");
        indent();
        for (const auto& child : node->children) {
            generateNode(child);
        }
        dedent();
        write("}");
    } else {
        write(viewName + "()");
    }
    
    generateModifiers(node->modifiers);
    
    writeLine();
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الخامس: تنفيذ دوال المعدّلات
// ═══════════════════════════════════════════════════════════════════════════

void SwiftUICodeGen::generateModifiers(const std::vector<parser::UIModifier>& modifiers) {
    if (modifiers.empty()) return;
    
    for (const auto& mod : modifiers) {
        modifierCount_++;
        
        auto mappingIt = modifierMappings_.find(mod.type);
        if (mappingIt == modifierMappings_.end()) continue;
        
        const auto& mapping = mappingIt->second;
        
        writeLine();
        write(getIndent() + "." + mapping.modifierName + "(");
        
        // تحويل القيمة
        if (!mod.arguments.empty()) {
            const auto& arg = mod.arguments[0];
            
            if (std::holds_alternative<parser::ColorValue>(arg)) {
                write(colorToSwiftUI(std::get<parser::ColorValue>(arg)));
            } else if (std::holds_alternative<parser::DimensionValue>(arg)) {
                bool isFont = (mod.type == parser::ModifierType::FontSize);
                write(dimensionToSwiftUI(std::get<parser::DimensionValue>(arg), isFont));
            } else if (std::holds_alternative<double>(arg)) {
                double value = std::get<double>(arg);
                
                // معالجة خاصة لبعض المعدّلات
                switch (mod.type) {
                    case parser::ModifierType::FontSize:
                        write(".system(size: " + std::to_string(static_cast<int>(value)) + ")");
                        break;
                    case parser::ModifierType::BorderRadius:
                        write(std::to_string(static_cast<int>(value)));
                        break;
                    case parser::ModifierType::Opacity:
                    case parser::ModifierType::Scale:
                        write(std::to_string(value));
                        break;
                    case parser::ModifierType::Rotation:
                        write(".degrees(" + std::to_string(value) + ")");
                        break;
                    default:
                        write(std::to_string(static_cast<int>(value)));
                }
            } else if (std::holds_alternative<bool>(arg)) {
                write(std::get<bool>(arg) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(arg)) {
                write("\"" + std::get<std::string>(arg) + "\"");
            }
        }
        
        write(")");
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم السادس: تنفيذ دوال التنسيق
// ═══════════════════════════════════════════════════════════════════════════

void SwiftUICodeGen::write(const std::string& text) {
    output_ << text;
}

void SwiftUICodeGen::writeLine(const std::string& line) {
    output_ << getIndent() << line << "\n";
}

void SwiftUICodeGen::indent() {
    currentIndent_++;
}

void SwiftUICodeGen::dedent() {
    if (currentIndent_ > 0) {
        currentIndent_--;
    }
}

std::string SwiftUICodeGen::getIndent() const {
    if (config_.useTabs) {
        return std::string(currentIndent_, '\t');
    }
    return std::string(currentIndent_ * config_.indentSize, ' ');
}

void SwiftUICodeGen::addImport(const std::string& import) {
    if (std::find(collectedImports_.begin(), collectedImports_.end(), import) 
        == collectedImports_.end()) {
        collectedImports_.push_back(import);
    }
}

void SwiftUICodeGen::writeArabicComment(const std::string& comment) {
    writeLine("// " + comment);
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم السابع: تنفيذ دوال التحويل الثابتة
// ═══════════════════════════════════════════════════════════════════════════

std::string SwiftUICodeGen::colorToSwiftUI(const parser::ColorValue& color) {
    static const std::unordered_map<std::string, std::string> colorMap = {
        {"أحمر", ".red"},
        {"أخضر", ".green"},
        {"أزرق", ".blue"},
        {"أصفر", ".yellow"},
        {"برتقالي", ".orange"},
        {"بنفسجي", ".purple"},
        {"وردي", ".pink"},
        {"بني", ".brown"},
        {"أبيض", ".white"},
        {"أسود", ".black"},
        {"رمادي", ".gray"},
        {"رمادي_فاتح", "Color(white: 0.8)"},
        {"رمادي_غامق", "Color(white: 0.3)"},
        {"أساسي", ".accentColor"},
        {"ثانوي", ".secondary"},
        {"شفاف", ".clear"}
    };
    
    switch (color.type) {
        case parser::ColorValue::Type::Named: {
            auto it = colorMap.find(color.namedColor);
            if (it != colorMap.end()) {
                return it->second;
            }
            return ".primary";
        }
        
        case parser::ColorValue::Type::RGB:
            return "Color(red: " + std::to_string(color.r / 255.0) + 
                   ", green: " + std::to_string(color.g / 255.0) + 
                   ", blue: " + std::to_string(color.b / 255.0) + ")";
        
        case parser::ColorValue::Type::RGBA:
            return "Color(red: " + std::to_string(color.r / 255.0) + 
                   ", green: " + std::to_string(color.g / 255.0) + 
                   ", blue: " + std::to_string(color.b / 255.0) + 
                   ", opacity: " + std::to_string(color.alpha) + ")";
        
        case parser::ColorValue::Type::Hex:
            return "Color(hex: \"" + color.hexValue + "\")";
    }
    
    return ".primary";
}

std::string SwiftUICodeGen::dimensionToSwiftUI(const parser::DimensionValue& dim, bool isFont) {
    switch (dim.unit) {
        case parser::DimensionValue::Unit::Points:
            if (isFont) {
                return ".system(size: " + std::to_string(static_cast<int>(dim.value)) + ")";
            }
            return std::to_string(static_cast<int>(dim.value));
            
        case parser::DimensionValue::Unit::Percent:
            return "GeometryReader percentage: " + std::to_string(dim.value / 100.0);
            
        case parser::DimensionValue::Unit::Fill:
            return ".infinity";
            
        case parser::DimensionValue::Unit::Fit:
            return "nil";
            
        case parser::DimensionValue::Unit::Auto:
            return "nil";
    }
    
    return "0";
}


// ═══════════════════════════════════════════════════════════════════════════
// القسم الثامن: تنفيذ الدوال المساعدة العامة
// ═══════════════════════════════════════════════════════════════════════════

std::string arabicToSwiftIdentifier(const std::string& arabicName) {
    static const std::unordered_map<std::string, std::string> translations = {
        {"شاشة_رئيسية", "HomeView"},
        {"شاشة_تفاصيل", "DetailView"},
        {"شاشة_إعدادات", "SettingsView"},
        {"شاشة_ملف", "ProfileView"},
        {"قائمة_رئيسية", "MainListView"},
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
    
    return "Generated_" + std::to_string(std::hash<std::string>{}(arabicName) % 10000);
}

bool isFeatureAvailable(const std::string& feature, const std::string& minVersion) {
    // بسيط - مقارنة إصدارات
    static const std::unordered_map<std::string, std::string> featureVersions = {
        {"NavigationStack", "16.0"},
        {"Observable", "17.0"},
        {"ContentUnavailableView", "17.0"},
        {"confirmationDialog", "15.0"}
    };
    
    auto it = featureVersions.find(feature);
    if (it == featureVersions.end()) {
        return true; // غير معروف = متاح
    }
    
    // مقارنة بسيطة
    return minVersion >= it->second;
}

} // namespace codegen
} // namespace sad
