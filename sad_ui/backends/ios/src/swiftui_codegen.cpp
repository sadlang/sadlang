/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: swiftui_codegen.cpp
 * المسار: sad_ui/backends/ios/src/swiftui_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مولّد كود SwiftUI.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/ios/swiftui_codegen.h"
#include "sad_ui/color_utils.h" // أدوات الألوان

namespace sad
{
    namespace ui
    {
        namespace ios
        {

            SwiftUICodegen::SwiftUICodegen(const SwiftUICodegenOptions &options)
                : options_(options)
            {
            }

            std::string SwiftUICodegen::generate(const IRModule &module)
            {
                std::ostringstream out;

                // استيرادات
                generateImports(out);
                out << "\n";

                // البنية الرئيسية
                generateStruct(out, module);

                // المعاينة
                if (options_.generatePreview)
                {
                    out << "\n";
                    generatePreview(out, module.name);
                }

                return out.str();
            }

            std::string SwiftUICodegen::generateNode(const IRNode &node, int indentLevel)
            {
                std::ostringstream out;

                if (node.getRenderingHint() == RenderingHint::Canvas)
                {
                    generateCanvasView(out, node, indentLevel);
                }
                else
                {
                    generateNativeView(out, node, indentLevel);
                }

                return out.str();
            }

            // ─── توليد الأجزاء ───────────────────────

            void SwiftUICodegen::generateImports(std::ostringstream &out)
            {
                out << "import SwiftUI\n";
            }

            void SwiftUICodegen::generateStruct(std::ostringstream &out, const IRModule &module)
            {
                out << "struct " << module.name << ": View {\n";

                // متغيرات الحالة
                generateStateVars(out, module.stateDefinitions, 1);

                if (!module.stateDefinitions.empty())
                {
                    out << "\n";
                }

                // body
                out << ind(1) << "var body: some View {\n";

                if (module.root)
                {
                    out << generateNode(*module.root, 2);
                }

                // RTL للعربية — شرطي: فقط إذا كان المحتوى يحتوي على أحرف عربية
                bool isArabicContent = false;
                for (unsigned char c : module.name)
                {
                    if (c >= 0xD8 && c <= 0xDF)
                    {
                        isArabicContent = true;
                        break;
                    }
                }
                if (!isArabicContent)
                {
                    for (const auto &[sName, _] : module.stateDefinitions)
                    {
                        for (unsigned char c : sName)
                            if (c >= 0xD8 && c <= 0xDF)
                            {
                                isArabicContent = true;
                                break;
                            }
                        if (isArabicContent)
                            break;
                    }
                }
                if (isArabicContent)
                {
                    out << ind(2) << ".environment(\\.layoutDirection, .rightToLeft)\n";
                }

                out << ind(1) << "}\n";
                out << "}\n";
            }

            void SwiftUICodegen::generateStateVars(
                std::ostringstream &out,
                const std::unordered_map<std::string, std::string> &states,
                int indentLevel)
            {
                for (const auto &[name, type] : states)
                {
                    out << ind(indentLevel) << "@State private var " << name;

                    // تحويل نوع ص إلى نوع Swift
                    if (type == "رقم")
                    {
                        out << ": Int = 0";
                    }
                    else if (type == "عشري")
                    {
                        out << ": Double = 0.0";
                    }
                    else if (type == "نص")
                    {
                        out << ": String = \"\"";
                    }
                    else if (type == "منطقي")
                    {
                        out << ": Bool = false";
                    }
                    else
                    {
                        out << " = 0";
                    }

                    out << "\n";
                }
            }

            void SwiftUICodegen::generateNativeView(
                std::ostringstream &out,
                const IRNode &node,
                int indentLevel)
            {
                std::string i = ind(indentLevel);

                switch (node.getType())
                {
                case UINodeType::Column:
                {
                    out << i << "VStack";

                    // التباعد
                    const auto *spacingProp = node.findProperty("تباعد");
                    if (spacingProp)
                    {
                        if (auto *v = std::get_if<double>(&spacingProp->value))
                            out << "(spacing: " << *v << ")";
                    }

                    out << " {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Row:
                {
                    out << i << "HStack";

                    const auto *spacingProp = node.findProperty("تباعد");
                    if (spacingProp)
                    {
                        if (auto *v = std::get_if<double>(&spacingProp->value))
                            out << "(spacing: " << *v << ")";
                    }

                    out << " {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Text:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string text = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "") : "";

                    out << i << "Text(\"" << text << "\")\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Button:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string text = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "") : "";

                    // حدث النقر
                    std::string action = "{}";
                    for (const auto &evt : node.getEvents())
                    {
                        if (evt.type == sad::ui::IREventType::OnTap)
                        {
                            action = "{ " + evt.expression + " }";
                            break;
                        }
                    }

                    // لون الخلفية
                    std::string buttonColor = "Color.accentColor";
                    const auto *bgColorProp = node.findProperty("لون_خلفية");
                    if (bgColorProp)
                    {
                        if (auto *colorStr = std::get_if<std::string>(&bgColorProp->value))
                        {
                            buttonColor = arabicColorToSwift(*colorStr);
                        }
                    }

                    out << i << "Button(action: " << action << ") {\n"
                        << i << "    Text(\"" << text << "\")\n"
                        << i << "        .foregroundColor(.white)\n"
                        << i << "        .padding()\n"
                        << i << "        .background(" << buttonColor << ")\n"
                        << i << "        .cornerRadius(8)\n"
                        << i << "}\n";
                    break;
                }

                case UINodeType::Image:
                {
                    // قراءة مصدر الصورة: مصدر ، src ، source ، text
                    const auto *srcProp = node.findProperty("\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1"); // مصدر
                    if (!srcProp)
                        srcProp = node.findProperty("src");
                    if (!srcProp)
                        srcProp = node.findProperty("source");
                    if (!srcProp)
                        srcProp = node.findProperty("\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1"); // مسار
                    if (!srcProp)
                        srcProp = node.findProperty("text");
                    std::string src = "photo";
                    bool isRemote = false;
                    if (srcProp)
                    {
                        if (auto *s = std::get_if<std::string>(&srcProp->value))
                        {
                            src = *s;
                            isRemote = (src.find("http") == 0);
                        }
                    }
                    if (isRemote)
                    {
                        out << i << "// صورة URL — يتطلب مكتبة SDWebImageSwiftUI\n"
                            << i << "AsyncImage(url: URL(string: \"" << src << "\")) { img in\n"
                            << i << "    img.resizable()\n"
                            << i << "} placeholder: { ProgressView() }\n";
                    }
                    else
                    {
                        bool isSystemIcon = (src.find(".") != std::string::npos || src.length() < 30);
                        if (isSystemIcon)
                        {
                            out << i << "Image(systemName: \"" << src << "\")\n";
                        }
                        else
                        {
                            out << i << "Image(\"" << src << "\")\n"
                                << i << "    .resizable()\n"
                                << i << "    .scaledToFit()\n";
                        }
                    }
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Spacer:
                {
                    out << i << "Spacer()\n";
                    const auto *hProp = node.findProperty("ارتفاع");
                    const auto *wProp = node.findProperty("عرض");
                    if (hProp || wProp)
                    {
                        out << i << ".frame(";
                        bool hasArg = false;
                        if (wProp)
                        {
                            if (auto *v = std::get_if<double>(&wProp->value))
                            {
                                out << "width: " << *v;
                                hasArg = true;
                            }
                            else if (auto *vi = std::get_if<int64_t>(&wProp->value))
                            {
                                out << "width: " << *vi;
                                hasArg = true;
                            }
                        }
                        if (hProp)
                        {
                            if (hasArg)
                                out << ", ";
                            if (auto *v = std::get_if<double>(&hProp->value))
                                out << "height: " << *v;
                            else if (auto *vi = std::get_if<int64_t>(&hProp->value))
                                out << "height: " << *vi;
                        }
                        out << ")\n";
                    }
                    break;
                }

                case UINodeType::Divider:
                {
                    out << i << "Divider()\n";
                    break;
                }

                case UINodeType::Toggle:
                {
                    // ربط بمتغير الحالة إذا وُجد
                    std::string stateVar;
                    for (const auto &ref : node.getStateRefs())
                    {
                        if (ref.propertyBinding == "\xd9\x85\xd9\x81\xd8\xb9\xd9\x91\xd9\x84" || // مفعّل
                            ref.propertyBinding == "checked" || ref.propertyBinding.empty())
                        {
                            stateVar = ref.stateName;
                            break;
                        }
                    }
                    const auto *labelProp = node.findProperty("text");
                    if (!labelProp)
                        labelProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
                    std::string label = "\"\"";
                    if (labelProp)
                    {
                        if (auto *s = std::get_if<std::string>(&labelProp->value))
                            label = "\"" + *s + "\"";
                    }
                    std::string binding = stateVar.empty() ? ".constant(false)" : "$" + stateVar;
                    out << i << "Toggle(" << label << ", isOn: " << binding << ")\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::TextField:
                {
                    std::string stateVar;
                    for (const auto &ref : node.getStateRefs())
                    {
                        if (ref.propertyBinding == "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9" || // قيمة
                            ref.propertyBinding == "value" || ref.propertyBinding.empty())
                        {
                            stateVar = ref.stateName;
                            break;
                        }
                    }
                    const auto *placeProp = node.findProperty("\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad"); // تلميح
                    if (!placeProp)
                        placeProp = node.findProperty("placeholder");
                    std::string placeholder = "\"\"";
                    if (placeProp)
                    {
                        if (auto *s = std::get_if<std::string>(&placeProp->value))
                            placeholder = "\"" + *s + "\"";
                    }
                    std::string binding = stateVar.empty() ? ".constant(\"\")" : "$" + stateVar;
                    out << i << "TextField(" << placeholder << ", text: " << binding << ")\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Slider:
                {
                    std::string stateVar;
                    for (const auto &ref : node.getStateRefs())
                    {
                        if (ref.propertyBinding == "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9" || ref.propertyBinding == "value" || ref.propertyBinding.empty())
                        {
                            stateVar = ref.stateName;
                            break;
                        }
                    }
                    const auto *minP = node.findProperty("\xd8\xa3\xd8\xaf\xd9\x86\xd9\x89"); // أدنى
                    const auto *maxP = node.findProperty("\xd8\xa3\xd9\x82\xd8\xb5\xd9\x89"); // أقصى
                    std::string minV = "0", maxV = "1";
                    if (minP)
                    {
                        if (auto *v = std::get_if<double>(&minP->value))
                            minV = std::to_string(*v);
                    }
                    if (maxP)
                    {
                        if (auto *v = std::get_if<double>(&maxP->value))
                            maxV = std::to_string(*v);
                    }
                    std::string binding = stateVar.empty() ? ".constant(0.5)" : "$" + stateVar;
                    out << i << "Slider(value: " << binding << ", in: " << minV << "..." << maxV << ")\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Checkbox:
                {
                    std::string stateVar;
                    for (const auto &ref : node.getStateRefs())
                    {
                        if (ref.propertyBinding == "\xd9\x85\xd9\x81\xd8\xb9\xd9\x91\xd9\x84" || // مفعّل
                            ref.propertyBinding == "checked" || ref.propertyBinding.empty())
                        {
                            stateVar = ref.stateName;
                            break;
                        }
                    }
                    const auto *labelProp = node.findProperty("text");
                    if (!labelProp)
                        labelProp = node.findProperty("\xd9\x86\xd8\xb5"); // نص
                    std::string label = "\"\"";
                    if (labelProp)
                    {
                        if (auto *s = std::get_if<std::string>(&labelProp->value))
                            label = "\"" + *s + "\"";
                    }
                    std::string binding = stateVar.empty() ? ".constant(false)" : "$" + stateVar;
                    out << i << "Toggle(" << label << ", isOn: " << binding << ")\n"
                        << i << "    .toggleStyle(.checkbox)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Radio:
                {
                    out << i << "Picker(\"\", selection: .constant(0)) {\n";
                    out << i << "    Text(\"خيار\").tag(0)\n";
                    out << i << "}\n";
                    out << i << ".pickerStyle(.radioGroup)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Picker:
                {
                    out << i << "Picker(\"\", selection: .constant(0)) {\n";
                    out << i << "    Text(\"خيار 1\").tag(0)\n";
                    out << i << "    Text(\"خيار 2\").tag(1)\n";
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::SearchBar:
                {
                    out << i << "TextField(\"بحث...\", text: .constant(\"\"))\n";
                    out << i << "    .textFieldStyle(.roundedBorder)\n";
                    out << i << "    .overlay(\n";
                    out << i << "        HStack {\n";
                    out << i << "            Image(systemName: \"magnifyingglass\")\n";
                    out << i << "                .foregroundColor(.gray)\n";
                    out << i << "            Spacer()\n";
                    out << i << "        }.padding(.horizontal, 8)\n";
                    out << i << "    )\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::ScrollView:
                {
                    out << i << "ScrollView {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    break;
                }

                case UINodeType::Card:
                {
                    out << i << "VStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    out << i << ".padding()\n";
                    out << i << ".background(Color(.systemBackground))\n";
                    out << i << ".cornerRadius(12)\n";
                    out << i << ".shadow(radius: 4)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Scaffold:
                {
                    out << i << "NavigationView {\n";
                    out << i << "    VStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 2);
                    }
                    out << i << "    }\n";
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::AppBar:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string title = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "العنوان") : "العنوان";
                    out << i << "// AppBar — يُستخدم عبر .navigationTitle\n";
                    out << i << "Text(\"" << title << "\")\n";
                    out << i << "    .font(.headline)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::ProgressBar:
                {
                    out << i << "ProgressView(value: 0.5)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Grid:
                {
                    out << i << "LazyVGrid(columns: [GridItem(.flexible()), GridItem(.flexible())]) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::LazyColumn:
                {
                    out << i << "ScrollView {\n";
                    out << i << "    LazyVStack {\n";
                    int itemIdx = 0;
                    for (const auto &child : node.getChildren())
                    {
                        // ─── تحسين أداء: .id() لمنع إعادة رسم غير ضرورية ───
                        auto itemId = child->getId().empty() ? std::to_string(itemIdx) : child->getId();
                        out << generateNode(*child, indentLevel + 2);
                        out << ind(indentLevel + 2) << ".id(\"" << itemId << "\")\n";
                        itemIdx++;
                    }
                    out << i << "    }\n";
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::LazyRow:
                {
                    out << i << "ScrollView(.horizontal) {\n";
                    out << i << "    LazyHStack {\n";
                    int itemIdx = 0;
                    for (const auto &child : node.getChildren())
                    {
                        auto itemId = child->getId().empty() ? std::to_string(itemIdx) : child->getId();
                        out << generateNode(*child, indentLevel + 2);
                        out << ind(indentLevel + 2) << ".id(\"" << itemId << "\")\n";
                        itemIdx++;
                    }
                    out << i << "    }\n";
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::List:
                {
                    out << i << "List {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::NavigationView:
                {
                    out << i << "NavigationStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::TabView:
                {
                    out << i << "TabView {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Drawer:
                {
                    out << i << "NavigationSplitView {\n";
                    out << i << "    List {\n";
                    out << i << "        Text(\"عنصر القائمة\")\n";
                    out << i << "    }\n";
                    out << i << "} detail: {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::BottomSheet:
                {
                    out << i << "VStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    out << i << ".presentationDetents([.medium, .large])\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::FAB:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string label = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "+") : "+";
                    out << i << "Button(action: {}) {\n";
                    out << i << "    Image(systemName: \"plus\")\n";
                    out << i << "        .font(.title2)\n";
                    out << i << "        .foregroundColor(.white)\n";
                    out << i << "}\n";
                    out << i << ".frame(width: 56, height: 56)\n";
                    out << i << ".background(Color.blue)\n";
                    out << i << ".clipShape(Circle())\n";
                    out << i << ".shadow(radius: 6)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::SnackBar:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string msg = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "رسالة") : "رسالة";
                    out << i << "HStack {\n";
                    out << i << "    Text(\"" << msg << "\")\n";
                    out << i << "        .foregroundColor(.white)\n";
                    out << i << "    Spacer()\n";
                    out << i << "}\n";
                    out << i << ".padding()\n";
                    out << i << ".background(Color(.darkGray))\n";
                    out << i << ".cornerRadius(8)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Badge:
                {
                    out << i << "Text(\"0\")\n";
                    out << i << "    .font(.caption2)\n";
                    out << i << "    .foregroundColor(.white)\n";
                    out << i << "    .padding(4)\n";
                    out << i << "    .background(Color.red)\n";
                    out << i << "    .clipShape(Circle())\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Chip:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string label = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "رقاقة") : "رقاقة";
                    out << i << "Text(\"" << label << "\")\n";
                    out << i << "    .padding(.horizontal, 12)\n";
                    out << i << "    .padding(.vertical, 6)\n";
                    out << i << "    .background(Color.gray.opacity(0.2))\n";
                    out << i << "    .clipShape(Capsule())\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Avatar:
                {
                    out << i << "Circle()\n";
                    out << i << "    .fill(Color.blue)\n";
                    out << i << "    .frame(width: 40, height: 40)\n";
                    out << i << "    .overlay(\n";
                    out << i << "        Text(\"م\")\n";
                    out << i << "            .foregroundColor(.white)\n";
                    out << i << "            .font(.headline)\n";
                    out << i << "    )\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Icon:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string iconName = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "star.fill") : "star.fill";
                    out << i << "Image(systemName: \"" << iconName << "\")\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Canvas:
                {
                    generateCanvasView(out, node, indentLevel);
                    break;
                }

                case UINodeType::Alert:
                {
                    out << i << "// Alert — يُستخدم عبر .alert() modifier\n";
                    out << i << "EmptyView()\n";
                    out << i << "    .alert(\"تنبيه\", isPresented: .constant(false)) {\n";
                    out << i << "        Button(\"حسناً\", role: .cancel) {}\n";
                    out << i << "    }\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Dialog:
                {
                    out << i << "// Dialog — يُستخدم عبر .confirmationDialog()\n";
                    out << i << "EmptyView()\n";
                    out << i << "    .confirmationDialog(\"حوار\", isPresented: .constant(false)) {\n";
                    out << i << "        Button(\"تأكيد\") {}\n";
                    out << i << "        Button(\"إلغاء\", role: .cancel) {}\n";
                    out << i << "    }\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Stack:
                {
                    out << i << "ZStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                    // ── §11 مكونات البيانات (Data Components — v3) ──

                case UINodeType::Tooltip:
                {
                    out << i << "// Tooltip — تلميح\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel);
                    }
                    out << i << ".help(\"تلميح\")\n";
                    break;
                }

                case UINodeType::DataTable:
                {
                    out << i << "Table(of: String.self) {\n"
                        << i << "    TableColumn(\"عمود\", value: \\.self)\n"
                        << i << "} rows: {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::TreeView:
                {
                    out << i << "List {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << i << "    DisclosureGroup(\"عنصر\") {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << i << "    }\n";
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Breadcrumb:
                {
                    out << i << "HStack(spacing: 4) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                        out << i << "    Text(\" > \").foregroundColor(.gray)\n";
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Pagination:
                {
                    out << i << "HStack {\n"
                        << i << "    Button(\"«\") {}\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "    Button(\"»\") {}\n"
                        << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Timeline:
                {
                    out << i << "VStack(alignment: .leading, spacing: 16) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << i << "    HStack(alignment: .top) {\n"
                            << i << "        Circle().fill(Color.blue).frame(width: 12, height: 12)\n"
                            << i << "        VStack(alignment: .leading) {\n";
                        out << generateNode(*child, indentLevel + 3);
                        out << i << "        }\n"
                            << i << "    }\n";
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                    // ── §12 مكونات الوسائط (Media Components — v3) ──

                case UINodeType::Carousel:
                {
                    out << i << "TabView {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n"
                        << i << ".tabViewStyle(.page)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::RichText:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string text = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "") : "";
                    out << i << "Text(AttributedString(\"" << text << "\"))\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Markdown:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string text = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "") : "";
                    out << i << "Text(try! AttributedString(markdown: \"" << text << "\"))\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::CodeBlock:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string text = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "") : "";
                    out << i << "Text(\"" << text << "\")\n"
                        << i << "    .font(.system(.body, design: .monospaced))\n"
                        << i << "    .padding(8)\n"
                        << i << "    .background(Color(.systemGray6))\n"
                        << i << "    .cornerRadius(4)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::AudioPlayer:
                {
                    out << i << "// AudioPlayer — مشغل صوت\n"
                        << i << "HStack {\n"
                        << i << "    Button(action: {}) { Image(systemName: \"play.fill\") }\n"
                        << i << "    Slider(value: .constant(0.0))\n"
                        << i << "    Text(\"00:00\")\n"
                        << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::ImageGallery:
                {
                    out << i << "LazyVGrid(columns: [GridItem(.adaptive(minimum: 100))]) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                    // ── §13 مكونات الحركة (Animation Components — v3) ──

                case UINodeType::Skeleton:
                {
                    out << i << "RoundedRectangle(cornerRadius: 4)\n"
                        << i << "    .fill(Color.gray.opacity(0.3))\n"
                        << i << "    .frame(height: 20)\n"
                        << i << "    .shimmer()\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Shimmer:
                {
                    out << i << "VStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n"
                        << i << ".redacted(reason: .placeholder)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::AnimatedList:
                {
                    out << i << "List {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                        out << i << "    .transition(.slide)\n";
                    }
                    out << i << "}\n"
                        << i << ".animation(.default, value: true)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Expandable:
                case UINodeType::Collapsible:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string title = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "عنصر") : "عنصر";
                    out << i << "DisclosureGroup(\"" << title << "\") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Swipeable:
                {
                    out << i << "// Swipeable — سحب\n"
                        << i << "VStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n"
                        << i << ".swipeActions { Button(\"حذف\", role: .destructive) {} }\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                    // ── §14 مكونات التحكم المتقدمة (Advanced Controls — v3) ──

                case UINodeType::Stepper:
                {
                    out << i << "Stepper(value: .constant(0)) {\n"
                        << i << "    Text(\"القيمة\")\n"
                        << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::SegmentedControl:
                {
                    out << i << "Picker(\"\", selection: .constant(0)) {\n";
                    int segIdx = 0;
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                        out << i << "    .tag(" << segIdx++ << ")\n";
                    }
                    out << i << "}\n"
                        << i << ".pickerStyle(.segmented)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::RatingBar:
                {
                    out << i << "HStack {\n"
                        << i << "    ForEach(0..<5) { index in\n"
                        << i << "        Image(systemName: index < 3 ? \"star.fill\" : \"star\")\n"
                        << i << "            .foregroundColor(index < 3 ? .yellow : .gray)\n"
                        << i << "    }\n"
                        << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::DatePicker:
                {
                    out << i << "DatePicker(\"\", selection: .constant(Date()), displayedComponents: .date)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::TimePicker:
                {
                    out << i << "DatePicker(\"\", selection: .constant(Date()), displayedComponents: .hourAndMinute)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Calendar:
                {
                    out << i << "DatePicker(\"\", selection: .constant(Date()), displayedComponents: .date)\n"
                        << i << "    .datePickerStyle(.graphical)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::ColorPicker:
                case UINodeType::ColorWheel:
                {
                    out << i << "ColorPicker(\"\", selection: .constant(.blue))\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                    // ── §15 مكونات التنقل المتقدمة (Extended Navigation — v3) ──

                case UINodeType::BottomNav:
                {
                    out << i << "TabView {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                        out << i << "    .tabItem { Label(\"عنصر\", systemImage: \"house\") }\n";
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::SideNav:
                {
                    out << i << "NavigationSplitView {\n"
                        << i << "    List {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 2);
                    }
                    out << i << "    }\n"
                        << i << "} detail: {\n"
                        << i << "    Text(\"اختر عنصراً\")\n"
                        << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Toolbar:
                {
                    out << i << "// Toolbar\n"
                        << i << "HStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n"
                        << i << ".toolbar {\n"
                        << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::SplitView:
                {
                    out << i << "HSplitView {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Tabs:
                {
                    out << i << "TabView {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::MegaMenu:
                {
                    out << i << "Menu(\"القائمة\") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << i << "    Menu(\"قسم\") {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << i << "    }\n";
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                    // ── أنواع v1-v2 مفقودة ──

                case UINodeType::TextArea:
                {
                    out << i << "TextEditor(text: .constant(\"\"))\n"
                        << i << "    .frame(minHeight: 100)\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::LazyGrid:
                {
                    out << i << "LazyVGrid(columns: [GridItem(.adaptive(minimum: 100))]) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::NavigationLink:
                {
                    out << i << "NavigationLink(destination: EmptyView()) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::TabItem:
                {
                    out << i << "// TabItem\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel);
                    }
                    out << i << ".tabItem { Text(\"تبويب\") }\n";
                    break;
                }

                case UINodeType::Section:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string header = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "") : "";
                    out << i << "Section(\"" << header << "\") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::ForEach:
                {
                    out << i << "ForEach(0..<1, id: \\.self) { _ in\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    break;
                }

                case UINodeType::Sheet:
                {
                    out << i << "// Sheet — ورقة منبثقة\n"
                        << i << "VStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n"
                        << i << ".sheet(isPresented: .constant(false)) { EmptyView() }\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Menu:
                {
                    out << i << "Menu(\"القائمة\") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << i << "    Button(action: {}) {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << i << "    }\n";
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::MapView:
                {
                    out << i << "Map(coordinateRegion: .constant(MKCoordinateRegion()))\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::WebView:
                {
                    out << i << "// WebView — يتطلب WKWebView عبر UIViewRepresentable\n"
                        << i << "Text(\"🌐 WebView\")\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::VideoPlayer:
                {
                    out << i << "VideoPlayer(player: AVPlayer())\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::CustomWidget:
                {
                    out << i << "VStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Conditional:
                {
                    out << i << "// Conditional — عرض مشروط\n"
                        << i << "Group {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Wrap:
                {
                    out << i << "// FlowLayout — ترتيب متدفق\n"
                        << i << "LazyVGrid(columns: [GridItem(.adaptive(minimum: 80))]) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }

                case UINodeType::Container:
                case UINodeType::Box:
                case UINodeType::Surface:
                case UINodeType::Group:
                default:
                {
                    // حاوي عام
                    out << i << "VStack {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << i << "}\n";
                    generateViewModifiers(out, node, indentLevel);
                    break;
                }
                }
            }

            void SwiftUICodegen::generateCanvasView(
                std::ostringstream &out,
                const IRNode &node,
                int indentLevel)
            {
                std::string i = ind(indentLevel);

                out << i << "Canvas { context, size in\n";
                out << i << "    // رسم مخصص لـ " << nodeTypeToArabicName(node.getType()) << "\n";
                out << i << "    context.fill(\n";
                out << i << "        Path { path in\n";
                out << i << "            path.addRect(CGRect(origin: .zero, size: size))\n";
                out << i << "        },\n";
                out << i << "        with: .color(.blue)\n";
                out << i << "    )\n";
                out << i << "}\n";
                generateViewModifiers(out, node, indentLevel);
            }

            void SwiftUICodegen::generateViewModifiers(
                std::ostringstream &out,
                const IRNode &node,
                int indentLevel)
            {
                std::string i = ind(indentLevel);

                for (const auto &prop : node.getProperties())
                {
                    if (prop.key == "حجم_خط" || prop.key == "حجم_الخط")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".font(.system(size: " << *v << "))\n";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            out << i << ".font(.system(size: " << *vi << "))\n";
                    }
                    else if (prop.key == "حشوة" || prop.key == "حشو")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".padding(" << *v << ")\n";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            out << i << ".padding(" << *vi << ")\n";
                        else
                            out << i << ".padding()\n";
                    }
                    else if (prop.key == "لون_خلفية" || prop.key == "خلفية")
                    {
                        if (auto *v = std::get_if<std::string>(&prop.value))
                            out << i << ".background(" << arabicColorToSwift(*v) << ")\n";
                    }
                    else if (prop.key == "نصف_قطر" || prop.key == "زوايا")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".cornerRadius(" << *v << ")\n";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            out << i << ".cornerRadius(" << *vi << ")\n";
                    }
                    else if (prop.key == "عرض")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".frame(width: " << *v << ")\n";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            out << i << ".frame(width: " << *vi << ")\n";
                    }
                    else if (prop.key == "ارتفاع")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".frame(height: " << *v << ")\n";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            out << i << ".frame(height: " << *vi << ")\n";
                    }
                    else if (prop.key == "هامش")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".padding(" << *v << ")\n";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            out << i << ".padding(" << *vi << ")\n";
                    }
                    else if (prop.key == "ظل" || prop.key == "رفع")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".shadow(color: .black.opacity(0.15), radius: " << *v << ", x: 0, y: " << (*v * 0.5) << ")\n";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            out << i << ".shadow(color: .black.opacity(0.15), radius: " << *vi << ", x: 0, y: " << (*vi / 2) << ")\n";
                    }
                    else if (prop.key == "حد_لون")
                    {
                        if (auto *v = std::get_if<std::string>(&prop.value))
                            out << i << ".overlay(RoundedRectangle(cornerRadius: 8).stroke(" << arabicColorToSwift(*v) << ", lineWidth: 1))\n";
                    }
                    else if (prop.key == "حد_سماكة")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".overlay(RoundedRectangle(cornerRadius: 8).stroke(.gray, lineWidth: " << *v << "))\n";
                    }
                    else if (prop.key == "لون_النص" || prop.key == "لون")
                    {
                        if (auto *v = std::get_if<std::string>(&prop.value))
                            out << i << ".foregroundColor(" << arabicColorToSwift(*v) << ")\n";
                    }
                    else if (prop.key == "شفافية")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".opacity(" << *v << ")\n";
                    }
                    else if (prop.key == "دوران")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".rotationEffect(.degrees(" << *v << "))\n";
                    }
                    else if (prop.key == "مقياس")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            out << i << ".scaleEffect(" << *v << ")\n";
                    }
                    else if (prop.key == "ملء_العرض")
                    {
                        if (auto *b = std::get_if<bool>(&prop.value); b && *b)
                            out << i << ".frame(maxWidth: .infinity)\n";
                    }
                    else if (prop.key == "ملء_الارتفاع")
                    {
                        if (auto *b = std::get_if<bool>(&prop.value); b && *b)
                            out << i << ".frame(maxHeight: .infinity)\n";
                    }
                    else if (prop.key == "ملء")
                    {
                        if (auto *b = std::get_if<bool>(&prop.value); b && *b)
                            out << i << ".frame(maxWidth: .infinity, maxHeight: .infinity)\n";
                    }
                    else if (prop.key == "وصف")
                    {
                        // إمكانية الوصول (Accessibility)
                        if (auto *v = std::get_if<std::string>(&prop.value))
                            out << i << ".accessibilityLabel(\"" << *v << "\")\n";
                    }
                }
            }

            void SwiftUICodegen::generatePreview(std::ostringstream &out, const std::string &structName)
            {
                out << "#Preview {\n"
                    << ind(1) << structName << "()\n"
                    << "}\n";
            }

            // ─── أدوات مساعدة ────────────────────────────

            std::string SwiftUICodegen::ind(int level) const
            {
                return std::string(level * options_.indentSize, ' ');
            }

            std::string SwiftUICodegen::nodeTypeToSwiftUI(UINodeType type) const
            {
                switch (type)
                {
                case UINodeType::Column:
                    return "VStack";
                case UINodeType::Row:
                    return "HStack";
                case UINodeType::Stack:
                    return "ZStack";
                case UINodeType::Text:
                    return "Text";
                case UINodeType::Button:
                    return "Button";
                case UINodeType::Image:
                    return "Image";
                case UINodeType::TextField:
                    return "TextField";
                case UINodeType::Toggle:
                    return "Toggle";
                case UINodeType::Slider:
                    return "Slider";
                case UINodeType::Checkbox:
                    return "Toggle";
                case UINodeType::Radio:
                    return "Picker";
                case UINodeType::Picker:
                    return "Picker";
                case UINodeType::SearchBar:
                    return "TextField";
                case UINodeType::ScrollView:
                    return "ScrollView";
                case UINodeType::List:
                    return "List";
                case UINodeType::LazyColumn:
                    return "LazyVStack";
                case UINodeType::LazyRow:
                    return "LazyHStack";
                case UINodeType::Grid:
                    return "LazyVGrid";
                case UINodeType::Card:
                    return "VStack";
                case UINodeType::Scaffold:
                    return "NavigationView";
                case UINodeType::AppBar:
                    return "Text";
                case UINodeType::TabView:
                    return "TabView";
                case UINodeType::NavigationView:
                    return "NavigationStack";
                case UINodeType::Drawer:
                    return "NavigationSplitView";
                case UINodeType::BottomSheet:
                    return "VStack";
                case UINodeType::ProgressBar:
                    return "ProgressView";
                case UINodeType::FAB:
                    return "Button";
                case UINodeType::SnackBar:
                    return "HStack";
                case UINodeType::Badge:
                    return "Text";
                case UINodeType::Chip:
                    return "Text";
                case UINodeType::Avatar:
                    return "Circle";
                case UINodeType::Icon:
                    return "Image";
                case UINodeType::Canvas:
                    return "Canvas";
                case UINodeType::Alert:
                    return "EmptyView";
                case UINodeType::Dialog:
                    return "EmptyView";
                case UINodeType::Spacer:
                    return "Spacer";
                case UINodeType::Divider:
                    return "Divider";
                // ── 11. بيانات ──
                case UINodeType::Tooltip:
                    return "Text";
                case UINodeType::DataTable:
                    return "Table";
                case UINodeType::TreeView:
                    return "OutlineGroup";
                case UINodeType::Breadcrumb:
                    return "HStack";
                case UINodeType::Pagination:
                    return "HStack";
                case UINodeType::Timeline:
                    return "VStack";
                // ── 12. وسائط ──
                case UINodeType::Carousel:
                    return "TabView";
                case UINodeType::RichText:
                    return "Text";
                case UINodeType::Markdown:
                    return "Text";
                case UINodeType::CodeBlock:
                    return "ScrollView";
                case UINodeType::AudioPlayer:
                    return "EmptyView";
                case UINodeType::ImageGallery:
                    return "ScrollView";
                // ── 13. حركة ──
                case UINodeType::Skeleton:
                    return "RoundedRectangle";
                case UINodeType::Shimmer:
                    return "RoundedRectangle";
                case UINodeType::AnimatedList:
                    return "List";
                case UINodeType::Expandable:
                    return "DisclosureGroup";
                case UINodeType::Collapsible:
                    return "DisclosureGroup";
                case UINodeType::Swipeable:
                    return "VStack";
                // ── 14. تحكم ──
                case UINodeType::Stepper:
                    return "Stepper";
                case UINodeType::SegmentedControl:
                    return "Picker";
                case UINodeType::RatingBar:
                    return "HStack";
                case UINodeType::TimePicker:
                    return "DatePicker";
                case UINodeType::Calendar:
                    return "DatePicker";
                case UINodeType::ColorWheel:
                    return "ColorPicker";
                // ── 15. تنقل ──
                case UINodeType::BottomNav:
                    return "TabView";
                case UINodeType::SideNav:
                    return "NavigationSplitView";
                case UINodeType::Toolbar:
                    return "HStack";
                case UINodeType::SplitView:
                    return "NavigationSplitView";
                case UINodeType::Tabs:
                    return "TabView";
                case UINodeType::MegaMenu:
                    return "Menu";
                default:
                    return "VStack";
                }
            }

        } // namespace ios
    } // namespace ui
} // namespace sad
