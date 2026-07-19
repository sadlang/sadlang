/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: compose_codegen.cpp
 * المسار: features/graphics/backends/android/src/compose_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مولّد كود Jetpack Compose.
 *
 * يحوّل كل عقدة IR إلى الـ Composable المقابل:
 *   Column → Column { ... }
 *   Text → Text("...")
 *   Button → Button(onClick = { ... }) { Text("...") }
 *   Canvas → Canvas(modifier = ...) { ... }
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/android/compose_codegen.h"
#include "sad_ui/color_utils.h" // أدوات الألوان
#include "sad_ui/types.h"
#include "sad_ui/prop_keys.h" // مصدر الحقيقة لمفاتيح الخصائص (لا سلاسل حرفيّة)

namespace sad
{
    namespace ui
    {
        namespace android
        {

            namespace
            {
                // (AR) يستخرج نصّ العنصر بالترتيب القانونيّ («عنوان» أوّلًا كما يكتبه
                //   مصنع زر())، مع بدائل، ويهرب محارف Kotlin كي لا يكسر عنوانٌ فيه
                //   " أو \ أو $ الكودَ المولَّد. كان كلّ فرع يقرأ "text" وحده فتظهر
                //   أزرار/نصوص Compose فارغة (فقدان «عنوان»/«محتوى»).
                std::string composeLabel(const IRNode &node)
                {
                    static const char *keys[] = {props::TITLE, props::ICON, props::TEXT_LATIN, props::TEXT, props::CONTENT};
                    std::string raw;
                    for (const char *k : keys)
                    {
                        const auto *p = node.findProperty(k);
                        if (!p)
                            continue;
                        if (auto *s = std::get_if<std::string>(&p->value)) { raw = *s; break; }
                        if (auto *iv = std::get_if<int64_t>(&p->value)) { raw = std::to_string(*iv); break; }
                        if (auto *dv = std::get_if<double>(&p->value)) { raw = std::to_string(*dv); break; }
                    }
                    std::string out;
                    for (char c : raw)
                    {
                        if (c == '\\' || c == '"') { out += '\\'; out += c; }
                        else if (c == '$') { out += "\\$"; }
                        else out += c;
                    }
                    return out;
                }
            } // namespace

            // ═══════════════════════════════════════════════════════════════════════════════
            // تنفيذ ComposeCodegen
            // ═══════════════════════════════════════════════════════════════════════════════

            ComposeCodegen::ComposeCodegen(const ComposeCodegenOptions &options)
                : options_(options)
            {
            }

            std::string ComposeCodegen::generate(const IRModule &module)
            {
                std::ostringstream out;

                // رأس الملف
                out << "package " << options_.packageName << "\n\n";

                // الاستيرادات
                generateImports(out);
                out << "\n";

                // الدالة الرئيسية
                generateComposable(out, module);

                // المعاينة
                if (options_.generatePreview)
                {
                    out << "\n";
                    generatePreview(out, module.name);
                }

                return out.str();
            }

            std::string ComposeCodegen::generateNode(const IRNode &node, int indentLevel)
            {
                std::ostringstream out;

                // التوجيه الذكي: أصلي أم Canvas؟
                if (node.getRenderingHint() == RenderingHint::Canvas)
                {
                    generateCanvasWidget(out, node, indentLevel);
                }
                else
                {
                    generateNativeWidget(out, node, indentLevel);
                }

                return out.str();
            }

            // ─── توليد الأجزاء ───────────────────────

            void ComposeCodegen::generateImports(std::ostringstream &out)
            {
                out << "import androidx.compose.foundation.layout.*\n"
                    << "import androidx.compose.material3.*\n"
                    << "import androidx.compose.runtime.*\n"
                    << "import androidx.compose.ui.Alignment\n"
                    << "import androidx.compose.ui.Modifier\n"
                    << "import androidx.compose.ui.unit.dp\n"
                    << "import androidx.compose.ui.unit.sp\n"
                    << "import androidx.compose.ui.tooling.preview.Preview\n"
                    // ─── تحسينات الأداء ───
                    << "import androidx.compose.foundation.lazy.LazyColumn\n"
                    << "import androidx.compose.foundation.lazy.items\n"
                    << "import androidx.compose.runtime.saveable.rememberSaveable\n"
                    << "import androidx.compose.ui.platform.LocalLifecycleOwner\n"
                    << "import androidx.compose.foundation.Canvas\n"
                    << "import androidx.compose.ui.graphics.Color\n";
            }

            void ComposeCodegen::generateComposable(std::ostringstream &out, const IRModule &module)
            {
                // @Composable function
                out << "@Composable\n"
                    << "fun " << module.name << "() {\n";

                // متغيرات الحالة
                generateState(out, module.stateDefinitions, 1);

                if (!module.stateDefinitions.empty())
                {
                    out << "\n";
                }

                // محتوى الواجهة
                if (module.root)
                {
                    out << generateNode(*module.root, 1);
                }

                out << "}\n";
            }

            void ComposeCodegen::generateState(
                std::ostringstream &out,
                const std::unordered_map<std::string, std::string> &states,
                int indentLevel)
            {
                for (const auto &[name, type] : states)
                {
                    out << indent(indentLevel);

                    // تحويل نوع ص إلى نوع Kotlin
                    std::string kotlinType = "Any";
                    std::string defaultVal = "null";

                    if (type == "رقم")
                    {
                        kotlinType = "Int";
                        defaultVal = "0";
                    }
                    else if (type == "عشري")
                    {
                        kotlinType = "Float";
                        defaultVal = "0f";
                    }
                    else if (type == "نص")
                    {
                        kotlinType = "String";
                        defaultVal = "\"\"";
                    }
                    else if (type == "منطقي")
                    {
                        kotlinType = "Boolean";
                        defaultVal = "false";
                    }

                    out << "var " << name
                        << " by remember { mutableStateOf<" << kotlinType << ">("
                        << defaultVal << ") }\n";
                }
            }

            void ComposeCodegen::generateNativeWidget(
                std::ostringstream &out,
                const IRNode &node,
                int indentLevel)
            {
                std::string ind = indent(indentLevel);
                std::string composable = nodeTypeToComposable(node.getType());
                std::string modifiers = generateModifiers(node);

                switch (node.getType())
                {
                case UINodeType::Column:
                {
                    out << ind << "Column(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Row:
                {
                    out << ind << "Row(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Text:
                {
                    std::string text = composeLabel(node);

                    out << ind << "Text(\n"
                        << ind << "    text = \"" << text << "\",\n";

                    // حجم الخط
                    const auto *sizeProp = node.findProperty("حجم_خط");
                    if (sizeProp)
                    {
                        if (auto *v = std::get_if<double>(&sizeProp->value))
                            out << ind << "    fontSize = " << *v << ".sp,\n";
                    }

                    out << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Button:
                {
                    std::string text = composeLabel(node);

                    // البحث عن حدث النقر
                    std::string onClickExpr = "{}";
                    for (const auto &evt : node.getEvents())
                    {
                        if (evt.type == sad::ui::IREventType::OnTap)
                        {
                            onClickExpr = "{ " + evt.expression + " }";
                            break;
                        }
                    }

                    // لون الخلفية
                    std::string buttonColor = "MaterialTheme.colorScheme.primary";
                    const auto *bgColorProp = node.findProperty("لون_خلفية");
                    if (bgColorProp)
                    {
                        if (auto *colorStr = std::get_if<std::string>(&bgColorProp->value))
                        {
                            buttonColor = arabicColorToKotlin(*colorStr);
                        }
                    }

                    out << ind << "Button(\n"
                        << ind << "    onClick = " << onClickExpr << ",\n"
                        << ind << "    colors = ButtonDefaults.buttonColors(containerColor = " << buttonColor << "),\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n"
                        << ind << "    Text(\"" << text << "\")\n"
                        << ind << "}\n";
                    break;
                }

                case UINodeType::Spacer:
                {
                    const auto *hProp = node.findProperty("ارتفاع");
                    const auto *wProp = node.findProperty("عرض");
                    std::string modifier;
                    if (hProp)
                    {
                        if (auto *v = std::get_if<double>(&hProp->value))
                            modifier = "Modifier.height(" + std::to_string(static_cast<int>(*v)) + ".dp)";
                        else if (auto *vi = std::get_if<int64_t>(&hProp->value))
                            modifier = "Modifier.height(" + std::to_string(*vi) + ".dp)";
                    }
                    else if (wProp)
                    {
                        if (auto *v = std::get_if<double>(&wProp->value))
                            modifier = "Modifier.width(" + std::to_string(static_cast<int>(*v)) + ".dp)";
                        else if (auto *vi = std::get_if<int64_t>(&wProp->value))
                            modifier = "Modifier.width(" + std::to_string(*vi) + ".dp)";
                    }
                    else
                    {
                        modifier = "Modifier.height(8.dp)";
                    }
                    out << ind << "Spacer(modifier = " << modifier << ")\n";
                    break;
                }

                case UINodeType::Divider:
                {
                    const auto *colorProp = node.findProperty("لون");
                    const auto *thickProp = node.findProperty("سمك");
                    out << ind << "Divider(";
                    bool hasArg = false;
                    if (colorProp)
                    {
                        if (auto *s = std::get_if<std::string>(&colorProp->value))
                        {
                            out << "color = " << arabicColorToKotlin(*s);
                            hasArg = true;
                        }
                    }
                    if (thickProp)
                    {
                        if (hasArg)
                            out << ", ";
                        if (auto *v = std::get_if<double>(&thickProp->value))
                            out << "thickness = " << static_cast<int>(*v) << ".dp";
                        else if (auto *vi = std::get_if<int64_t>(&thickProp->value))
                            out << "thickness = " << *vi << ".dp";
                    }
                    out << ")\n";
                    break;
                }

                case UINodeType::TextField:
                {
                    // البحث عن state binding للنص
                    std::string stateVar;
                    for (const auto &ref : node.getStateRefs())
                    {
                        if (ref.propertyBinding == "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9" || // قيمة
                            ref.propertyBinding == "value" ||
                            ref.propertyBinding.empty())
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
                    std::string valExpr = stateVar.empty() ? "\"\"" : stateVar;
                    std::string onChange = stateVar.empty() ? "{}" : "{ " + stateVar + " = it }";
                    out << ind << "TextField(\n"
                        << ind << "    value = " << valExpr << ",\n"
                        << ind << "    onValueChange = " << onChange << ",\n"
                        << ind << "    placeholder = { Text(" << placeholder << ") },\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Toggle:
                {
                    std::string stateVar;
                    for (const auto &ref : node.getStateRefs())
                    {
                        if (ref.propertyBinding == "\xd9\x85\xd9\x81\xd8\xb9\xd9\x91\xd9\x84" || // مفعّل
                            ref.propertyBinding == "checked" ||
                            ref.propertyBinding.empty())
                        {
                            stateVar = ref.stateName;
                            break;
                        }
                    }
                    std::string valExpr = stateVar.empty() ? "false" : stateVar;
                    std::string onChange = stateVar.empty() ? "{}" : "{ " + stateVar + " = it }";
                    out << ind << "Switch(\n"
                        << ind << "    checked = " << valExpr << ",\n"
                        << ind << "    onCheckedChange = " << onChange << ",\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
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
                    std::string minV = "0f", maxV = "1f";
                    if (minP)
                    {
                        if (auto *v = std::get_if<double>(&minP->value))
                            minV = std::to_string(*v) + "f";
                    }
                    if (maxP)
                    {
                        if (auto *v = std::get_if<double>(&maxP->value))
                            maxV = std::to_string(*v) + "f";
                    }
                    std::string valExpr = stateVar.empty() ? "0.5f" : stateVar;
                    std::string onChange = stateVar.empty() ? "{}" : "{ " + stateVar + " = it }";
                    out << ind << "Slider(\n"
                        << ind << "    value = " << valExpr << ",\n"
                        << ind << "    onValueChange = " << onChange << ",\n"
                        << ind << "    valueRange = " << minV << ".." << maxV << ",\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Checkbox:
                {
                    std::string stateVar;
                    for (const auto &ref : node.getStateRefs())
                    {
                        if (ref.propertyBinding == "\xd9\x85\xd9\x81\xd8\xb9\xd9\x91\xd9\x84" || // مفعّل
                            ref.propertyBinding == "checked" ||
                            ref.propertyBinding.empty())
                        {
                            stateVar = ref.stateName;
                            break;
                        }
                    }
                    std::string valExpr = stateVar.empty() ? "false" : stateVar;
                    std::string onChange = stateVar.empty() ? "{}" : "{ " + stateVar + " = it }";
                    out << ind << "Checkbox(\n"
                        << ind << "    checked = " << valExpr << ",\n"
                        << ind << "    onCheckedChange = " << onChange << ",\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Radio:
                {
                    std::string stateVar;
                    for (const auto &ref : node.getStateRefs())
                    {
                        if (ref.propertyBinding.empty() || ref.propertyBinding == "selected")
                        {
                            stateVar = ref.stateName;
                            break;
                        }
                    }
                    const auto *valProp = node.findProperty("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"); // قيمة
                    if (!valProp)
                        valProp = node.findProperty("value");
                    std::string radioValue = "\"\"";
                    if (valProp)
                    {
                        if (auto *s = std::get_if<std::string>(&valProp->value))
                            radioValue = "\"" + *s + "\"";
                    }
                    std::string selected = stateVar.empty() ? "false" : (stateVar + " == " + radioValue);
                    std::string onClick = stateVar.empty() ? "{}" : "{ " + stateVar + " = " + radioValue + " }";
                    out << ind << "RadioButton(\n"
                        << ind << "    selected = " << selected << ",\n"
                        << ind << "    onClick = " << onClick << ",\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Image:
                {
                    // قراءة مصدر الصورة: مصدر ، src ، source
                    const auto *srcProp = node.findProperty("\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1"); // مصدر
                    if (!srcProp)
                        srcProp = node.findProperty("src");
                    if (!srcProp)
                        srcProp = node.findProperty("source");
                    if (!srcProp)
                        srcProp = node.findProperty("\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1"); // مسار
                    std::string src = "placeholder";
                    bool isUrl = false;
                    if (srcProp)
                    {
                        if (auto *s = std::get_if<std::string>(&srcProp->value))
                        {
                            src = *s;
                            isUrl = (src.find("http") == 0 || src.find("/") != std::string::npos);
                        }
                    }
                    const auto *altProp = node.findProperty("\xd9\x86\xd8\xb5_\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84"); // نص_بديل
                    if (!altProp)
                        altProp = node.findProperty("alt");
                    std::string altText = "null";
                    if (altProp)
                    {
                        if (auto *s = std::get_if<std::string>(&altProp->value))
                            altText = "\"" + *s + "\"";
                    }
                    if (isUrl)
                    {
                        out << ind << "// صورة من URL — يتطلب مكتبة Coil أو Glide\n"
                            << ind << "AsyncImage(\n"
                            << ind << "    model = \"" << src << "\",\n"
                            << ind << "    contentDescription = " << altText << ",\n"
                            << ind << "    " << modifiers << "\n"
                            << ind << ")\n";
                    }
                    else
                    {
                        out << ind << "Image(\n"
                            << ind << "    painter = painterResource(id = R.drawable." << src << "),\n"
                            << ind << "    contentDescription = " << altText << ",\n"
                            << ind << "    " << modifiers << "\n"
                            << ind << ")\n";
                    }
                    break;
                }

                case UINodeType::Icon:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string iconName = "Star";
                    if (textProp)
                    {
                        if (auto *s = std::get_if<std::string>(&textProp->value))
                            iconName = *s;
                    }
                    out << ind << "Icon(\n"
                        << ind << "    imageVector = Icons.Default." << iconName << ",\n"
                        << ind << "    contentDescription = null,\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Card:
                {
                    out << ind << "Card(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Scaffold:
                {
                    out << ind << "Scaffold(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") { paddingValues ->\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::AppBar:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string title;
                    if (textProp)
                    {
                        if (auto *s = std::get_if<std::string>(&textProp->value))
                            title = *s;
                    }
                    out << ind << "TopAppBar(\n"
                        << ind << "    title = { Text(\"" << title << "\") },\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Stack:
                {
                    out << ind << "Box(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Grid:
                {
                    out << ind << "LazyVerticalGrid(\n"
                        << ind << "    columns = GridCells.Fixed(2),\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    item {\n"
                            << generateNode(*child, indentLevel + 2)
                            << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::LazyColumn:
                {
                    out << ind << "LazyColumn(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    int itemIdx = 0;
                    for (const auto &child : node.getChildren())
                    {
                        // ─── تحسين أداء: مفتاح فريد لكل عنصر ───
                        auto itemId = child->getId().empty() ? std::to_string(itemIdx) : child->getId();
                        out << ind << "    item(key = \"" << itemId << "\") {\n"
                            << generateNode(*child, indentLevel + 2)
                            << ind << "    }\n";
                        itemIdx++;
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::LazyRow:
                {
                    out << ind << "LazyRow(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    int itemIdx = 0;
                    for (const auto &child : node.getChildren())
                    {
                        auto itemId = child->getId().empty() ? std::to_string(itemIdx) : child->getId();
                        out << ind << "    item(key = \"" << itemId << "\") {\n"
                            << generateNode(*child, indentLevel + 2)
                            << ind << "    }\n";
                        itemIdx++;
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::ScrollView:
                {
                    out << ind << "Column(\n"
                        << ind << "    modifier = Modifier.verticalScroll(rememberScrollState())\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::List:
                {
                    out << ind << "LazyColumn(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    item {\n"
                            << generateNode(*child, indentLevel + 2)
                            << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::NavigationView:
                {
                    out << ind << "// NavigationView\n"
                        << ind << "Column(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::TabView:
                {
                    out << ind << "TabRow(selectedTabIndex = 0) {\n";
                    int tabIdx = 0;
                    for (const auto &child : node.getChildren())
                    {
                        const auto *tp = child->findProperty("text");
                        std::string title = "Tab " + std::to_string(tabIdx);
                        if (tp)
                        {
                            if (auto *s = std::get_if<std::string>(&tp->value))
                                title = *s;
                        }
                        out << ind << "    Tab(selected = " << (tabIdx == 0 ? "true" : "false")
                            << ", onClick = {}, text = { Text(\"" << title << "\") })\n";
                        tabIdx++;
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Picker:
                {
                    out << ind << "// Picker — DropdownMenu\n"
                        << ind << "ExposedDropdownMenuBox(\n"
                        << ind << "    expanded = false,\n"
                        << ind << "    onExpandedChange = {}\n"
                        << ind << ") {\n"
                        << ind << "    TextField(value = \"\", onValueChange = {}, readOnly = true, label = { Text(\"\") }, modifier = Modifier.menuAnchor())\n"
                        << ind << "}\n";
                    break;
                }

                case UINodeType::ProgressBar:
                {
                    out << ind << "LinearProgressIndicator(\n"
                        << ind << "    progress = 0.5f,\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::SearchBar:
                {
                    out << ind << "SearchBar(\n"
                        << ind << "    query = \"\",\n"
                        << ind << "    onQueryChange = {},\n"
                        << ind << "    onSearch = {},\n"
                        << ind << "    active = false,\n"
                        << ind << "    onActiveChange = {},\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {}\n";
                    break;
                }

                case UINodeType::FAB:
                {
                    std::string onClickExpr = "{}";
                    for (const auto &evt : node.getEvents())
                    {
                        if (evt.type == sad::ui::IREventType::OnTap)
                        {
                            onClickExpr = "{ " + evt.expression + " }";
                            break;
                        }
                    }
                    out << ind << "FloatingActionButton(\n"
                        << ind << "    onClick = " << onClickExpr << ",\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::SnackBar:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string msg;
                    if (textProp)
                    {
                        if (auto *s = std::get_if<std::string>(&textProp->value))
                            msg = *s;
                    }
                    out << ind << "Snackbar {\n"
                        << ind << "    Text(\"" << msg << "\")\n"
                        << ind << "}\n";
                    break;
                }

                case UINodeType::Badge:
                {
                    out << ind << "BadgedBox(\n"
                        << ind << "    badge = { Badge { Text(\"0\") } }\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Chip:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string text;
                    if (textProp)
                    {
                        if (auto *s = std::get_if<std::string>(&textProp->value))
                            text = *s;
                    }
                    out << ind << "FilterChip(\n"
                        << ind << "    selected = false,\n"
                        << ind << "    onClick = {},\n"
                        << ind << "    label = { Text(\"" << text << "\") }\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Avatar:
                {
                    out << ind << "Box(\n"
                        << ind << "    modifier = Modifier.size(40.dp).clip(CircleShape),\n"
                        << ind << "    contentAlignment = Alignment.Center\n"
                        << ind << ") {\n";
                    const auto *textProp = node.findProperty("text");
                    if (textProp)
                    {
                        if (auto *s = std::get_if<std::string>(&textProp->value))
                            out << ind << "    Text(\"" << *s << "\")\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Drawer:
                {
                    out << ind << "ModalNavigationDrawer(\n"
                        << ind << "    drawerContent = {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 2);
                    }
                    out << ind << "    }\n"
                        << ind << ") {}\n";
                    break;
                }

                case UINodeType::BottomSheet:
                {
                    out << ind << "ModalBottomSheet(\n"
                        << ind << "    onDismissRequest = {}\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Alert:
                case UINodeType::Dialog:
                {
                    out << ind << "AlertDialog(\n"
                        << ind << "    onDismissRequest = {},\n"
                        << ind << "    confirmButton = { TextButton(onClick = {}) { Text(\"OK\") } },\n"
                        << ind << "    title = { Text(\"\") },\n"
                        << ind << "    text = { Text(\"\") }\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Canvas:
                {
                    out << ind << "Canvas(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n"
                        << ind << "    // رسم مخصص\n"
                        << ind << "}\n";
                    break;
                }

                    // ── §11 مكونات البيانات (Data Components — v3) ──

                case UINodeType::Tooltip:
                {
                    out << ind << "PlainTooltip(\n"
                        << ind << "    tooltip = { Text(\"تلميح\") }\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::DataTable:
                {
                    out << ind << "LazyColumn(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n"
                        << ind << "    // DataTable — جدول بيانات\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    item {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::TreeView:
                {
                    out << ind << "LazyColumn(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n"
                        << ind << "    // TreeView — عرض شجري\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    item {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Breadcrumb:
                {
                    out << ind << "Row(\n"
                        << ind << "    " << modifiers << ",\n"
                        << ind << "    horizontalArrangement = Arrangement.spacedBy(4.dp)\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                        out << ind << "    Text(\" > \")\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Pagination:
                {
                    out << ind << "Row(\n"
                        << ind << "    " << modifiers << ",\n"
                        << ind << "    horizontalArrangement = Arrangement.spacedBy(4.dp)\n"
                        << ind << ") {\n"
                        << ind << "    TextButton(onClick = {}) { Text(\"«\") }\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "    TextButton(onClick = {}) { Text(\"»\") }\n"
                        << ind << "}\n";
                    break;
                }

                case UINodeType::Timeline:
                {
                    out << ind << "LazyColumn(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    item {\n"
                            << ind << "        Row(verticalAlignment = Alignment.Top) {\n"
                            << ind << "            Box(modifier = Modifier.size(12.dp).background(MaterialTheme.colorScheme.primary, CircleShape))\n"
                            << ind << "            Spacer(modifier = Modifier.width(8.dp))\n"
                            << ind << "            Column {\n";
                        out << generateNode(*child, indentLevel + 4);
                        out << ind << "            }\n"
                            << ind << "        }\n"
                            << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                    // ── §12 مكونات الوسائط (Media Components — v3) ──

                case UINodeType::Carousel:
                {
                    out << ind << "HorizontalPager(\n"
                        << ind << "    state = rememberPagerState { " << node.childCount() << " },\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") { page ->\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::RichText:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string text = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "") : "";
                    out << ind << "Text(\n"
                        << ind << "    text = buildAnnotatedString {\n"
                        << ind << "        append(\"" << text << "\")\n"
                        << ind << "    },\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Markdown:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string text = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "") : "";
                    out << ind << "// Markdown — عرض نص منسق\n"
                        << ind << "Text(\n"
                        << ind << "    text = \"" << text << "\",\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::CodeBlock:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string text = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "") : "";
                    out << ind << "Text(\n"
                        << ind << "    text = \"" << text << "\",\n"
                        << ind << "    fontFamily = FontFamily.Monospace,\n"
                        << ind << "    modifier = " << modifiers << ".background(Color(0xFFF5F5F5)).padding(8.dp)\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::AudioPlayer:
                {
                    out << ind << "// AudioPlayer — مشغل صوت\n"
                        << ind << "Row(\n"
                        << ind << "    " << modifiers << ",\n"
                        << ind << "    verticalAlignment = Alignment.CenterVertically\n"
                        << ind << ") {\n"
                        << ind << "    IconButton(onClick = {}) { Icon(Icons.Default.PlayArrow, \"تشغيل\") }\n"
                        << ind << "    Slider(value = 0f, onValueChange = {}, modifier = Modifier.weight(1f))\n"
                        << ind << "    Text(\"00:00\")\n"
                        << ind << "}\n";
                    break;
                }

                case UINodeType::ImageGallery:
                {
                    out << ind << "LazyVerticalGrid(\n"
                        << ind << "    columns = GridCells.Fixed(3),\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    item {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                    // ── §13 مكونات الحركة (Animation Components — v3) ──

                case UINodeType::Skeleton:
                {
                    out << ind << "Box(\n"
                        << ind << "    modifier = " << modifiers << "\n"
                        << ind << "        .background(Color.LightGray, RoundedCornerShape(4.dp))\n"
                        << ind << "        .shimmer()\n"
                        << ind << ") {}\n";
                    break;
                }

                case UINodeType::Shimmer:
                {
                    out << ind << "Box(\n"
                        << ind << "    modifier = " << modifiers << ".shimmer()\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::AnimatedList:
                {
                    out << ind << "LazyColumn(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    item {\n"
                            << ind << "        AnimatedVisibility(visible = true) {\n";
                        out << generateNode(*child, indentLevel + 3);
                        out << ind << "        }\n"
                            << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Expandable:
                case UINodeType::Collapsible:
                {
                    const auto *textProp = node.findProperty("text");
                    std::string title = textProp ? (std::get_if<std::string>(&textProp->value) ? *std::get_if<std::string>(&textProp->value) : "عنصر") : "عنصر";
                    out << ind << "var expanded by remember { mutableStateOf(false) }\n"
                        << ind << "Column(" << modifiers << ") {\n"
                        << ind << "    TextButton(onClick = { expanded = !expanded }) { Text(\"" << title << "\") }\n"
                        << ind << "    AnimatedVisibility(visible = expanded) {\n"
                        << ind << "        Column {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 3);
                    }
                    out << ind << "        }\n"
                        << ind << "    }\n"
                        << ind << "}\n";
                    break;
                }

                case UINodeType::Swipeable:
                {
                    out << ind << "SwipeToDismissBox(\n"
                        << ind << "    state = rememberSwipeToDismissBoxState(),\n"
                        << ind << "    backgroundContent = {},\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                    // ── §14 مكونات التحكم المتقدمة (Advanced Controls — v3) ──

                case UINodeType::Stepper:
                {
                    out << ind << "Row(\n"
                        << ind << "    " << modifiers << ",\n"
                        << ind << "    verticalAlignment = Alignment.CenterVertically\n"
                        << ind << ") {\n"
                        << ind << "    IconButton(onClick = {}) { Text(\"−\") }\n"
                        << ind << "    Text(\"0\", modifier = Modifier.padding(horizontal = 16.dp))\n"
                        << ind << "    IconButton(onClick = {}) { Text(\"+\") }\n"
                        << ind << "}\n";
                    break;
                }

                case UINodeType::SegmentedControl:
                {
                    out << ind << "SingleChoiceSegmentedButtonRow(" << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    SegmentedButton(selected = false, onClick = {}, shape = RoundedCornerShape(0)) {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::RatingBar:
                {
                    out << ind << "Row(" << modifiers << ") {\n"
                        << ind << "    repeat(5) { index ->\n"
                        << ind << "        Icon(\n"
                        << ind << "            imageVector = if (index < 3) Icons.Filled.Star else Icons.Default.Star,\n"
                        << ind << "            contentDescription = \"نجمة\",\n"
                        << ind << "            tint = if (index < 3) Color(0xFFFFD700) else Color.Gray\n"
                        << ind << "        )\n"
                        << ind << "    }\n"
                        << ind << "}\n";
                    break;
                }

                case UINodeType::DatePicker:
                {
                    out << ind << "DatePicker(\n"
                        << ind << "    state = rememberDatePickerState(),\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::TimePicker:
                {
                    out << ind << "TimePicker(\n"
                        << ind << "    state = rememberTimePickerState(),\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::Calendar:
                {
                    out << ind << "DatePicker(\n"
                        << ind << "    state = rememberDatePickerState(),\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::ColorPicker:
                case UINodeType::ColorWheel:
                {
                    out << ind << "// ColorPicker — منتقي لون\n"
                        << ind << "Box(\n"
                        << ind << "    modifier = " << modifiers << ".size(200.dp).background(Color.White)\n"
                        << ind << ") {\n"
                        << ind << "    Canvas(modifier = Modifier.fillMaxSize()) {\n"
                        << ind << "        // رسم عجلة ألوان\n"
                        << ind << "    }\n"
                        << ind << "}\n";
                    break;
                }

                    // ── §15 مكونات التنقل المتقدمة (Extended Navigation — v3) ──

                case UINodeType::BottomNav:
                {
                    out << ind << "NavigationBar(" << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    NavigationBarItem(\n"
                            << ind << "        selected = false,\n"
                            << ind << "        onClick = {},\n"
                            << ind << "        icon = { Icon(Icons.Default.Home, \"\") },\n"
                            << ind << "        label = {\n";
                        out << generateNode(*child, indentLevel + 3);
                        out << ind << "        }\n"
                            << ind << "    )\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::SideNav:
                {
                    out << ind << "NavigationRail(" << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    NavigationRailItem(\n"
                            << ind << "        selected = false,\n"
                            << ind << "        onClick = {},\n"
                            << ind << "        icon = { Icon(Icons.Default.Menu, \"\") },\n"
                            << ind << "        label = {\n";
                        out << generateNode(*child, indentLevel + 3);
                        out << ind << "        }\n"
                            << ind << "    )\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Toolbar:
                {
                    out << ind << "TopAppBar(\n"
                        << ind << "    title = { Text(\"\") },\n"
                        << ind << "    actions = {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 2);
                    }
                    out << ind << "    },\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::SplitView:
                {
                    out << ind << "Row(" << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    Box(modifier = Modifier.weight(1f)) {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Tabs:
                {
                    out << ind << "TabRow(selectedTabIndex = 0, " << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    Tab(selected = false, onClick = {}) {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::MegaMenu:
                {
                    out << ind << "DropdownMenu(expanded = true, onDismissRequest = {}, " << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    DropdownMenuItem(text = {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << ind << "    }, onClick = {})\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                    // ── أنواع v1-v2 مفقودة ──

                case UINodeType::TextArea:
                {
                    out << ind << "TextField(\n"
                        << ind << "    value = \"\",\n"
                        << ind << "    onValueChange = {},\n"
                        << ind << "    modifier = " << modifiers << ".height(120.dp),\n"
                        << ind << "    maxLines = 5\n"
                        << ind << ")\n";
                    break;
                }

                case UINodeType::LazyGrid:
                {
                    out << ind << "LazyVerticalGrid(\n"
                        << ind << "    columns = GridCells.Fixed(2),\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    item {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << ind << "    }\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::NavigationLink:
                {
                    out << ind << "TextButton(onClick = {}) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::TabItem:
                {
                    out << ind << "Tab(selected = false, onClick = {}) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Section:
                {
                    out << ind << "Column(" << modifiers << ") {\n";
                    const auto *textProp = node.findProperty("text");
                    if (textProp)
                    {
                        if (auto *text = std::get_if<std::string>(&textProp->value))
                            out << ind << "    Text(\"" << *text << "\", style = MaterialTheme.typography.titleSmall)\n";
                    }
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::ForEach:
                {
                    out << ind << "// ForEach — تكرار تصريحي\n"
                        << ind << "Column(" << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Sheet:
                {
                    out << ind << "ModalBottomSheet(onDismissRequest = {}) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Menu:
                {
                    out << ind << "DropdownMenu(expanded = true, onDismissRequest = {}) {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << ind << "    DropdownMenuItem(text = {\n";
                        out << generateNode(*child, indentLevel + 2);
                        out << ind << "    }, onClick = {})\n";
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::MapView:
                {
                    out << ind << "// MapView — خريطة (يتطلب مكتبة خرائط)\n"
                        << ind << "Box(" << modifiers << ") { Text(\"🗺️ خريطة\") }\n";
                    break;
                }

                case UINodeType::WebView:
                {
                    out << ind << "// WebView — عرض ويب (يتطلب AndroidView)\n"
                        << ind << "AndroidView(factory = { context -> android.webkit.WebView(context) }, " << modifiers << ")\n";
                    break;
                }

                case UINodeType::VideoPlayer:
                {
                    out << ind << "// VideoPlayer — مشغل فيديو (يتطلب ExoPlayer)\n"
                        << ind << "Box(" << modifiers << ") { Text(\"▶ فيديو\") }\n";
                    break;
                }

                case UINodeType::CustomWidget:
                {
                    out << ind << "Box(" << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Conditional:
                {
                    out << ind << "// Conditional — عرض مشروط\n"
                        << ind << "Column(" << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Wrap:
                {
                    out << ind << "FlowRow(" << modifiers << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }

                case UINodeType::Container:
                case UINodeType::Box:
                case UINodeType::Surface:
                case UINodeType::Group:
                default:
                {
                    // حاوي عام — Box
                    out << ind << "Box(\n"
                        << ind << "    " << modifiers << "\n"
                        << ind << ") {\n";
                    for (const auto &child : node.getChildren())
                    {
                        out << generateNode(*child, indentLevel + 1);
                    }
                    out << ind << "}\n";
                    break;
                }
                }
            }

            void ComposeCodegen::generateCanvasWidget(
                std::ostringstream &out,
                const IRNode &node,
                int indentLevel)
            {
                std::string ind = indent(indentLevel);

                // توليد Canvas للرسم المخصص
                // قراءة العرض والارتفاع من الخصائص
                const auto *wProp = node.findProperty("\xd8\xb9\xd8\xb1\xd8\xb6");
                const auto *hProp = node.findProperty("\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9");

                out << ind << "Canvas(\n";
                if (wProp || hProp)
                {
                    out << ind << "    modifier = Modifier";
                    if (wProp)
                    {
                        if (auto *v = std::get_if<double>(&wProp->value))
                            out << ".width(" << static_cast<int>(*v) << ".dp)";
                        else if (auto *vi = std::get_if<int64_t>(&wProp->value))
                            out << ".width(" << *vi << ".dp)";
                    }
                    if (hProp)
                    {
                        if (auto *v = std::get_if<double>(&hProp->value))
                            out << ".height(" << static_cast<int>(*v) << ".dp)";
                        else if (auto *vi = std::get_if<int64_t>(&hProp->value))
                            out << ".height(" << *vi << ".dp)";
                    }
                    out << "\n";
                }
                else
                {
                    out << ind << "    modifier = Modifier.fillMaxSize()\n";
                }
                out << ind << ") {\n";

                // توليد أوامر الرسم بناءً على خصائص العقدة
                out << ind << "    // رسم مخصص لـ " << nodeTypeToArabicName(node.getType()) << "\n";

                // لون الخلفية → drawRect
                const auto *bgProp = node.findProperty("\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9");
                if (bgProp)
                {
                    if (auto *cs = std::get_if<std::string>(&bgProp->value))
                        out << ind << "    drawRect(" << arabicColorToKotlin(*cs) << ")\n";
                    else
                        out << ind << "    drawRect(Color.Blue)\n";
                }

                // زوايا دائرية → drawRoundRect
                const auto *radiusProp = node.findProperty("\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7");
                if (!radiusProp)
                    radiusProp = node.findProperty("\xd9\x86\xd8\xb5\xd9\x81_\xd9\x82\xd8\xb7\xd8\xb1");
                if (radiusProp && !bgProp)
                {
                    float r = 8.0f;
                    if (auto *v = std::get_if<double>(&radiusProp->value))
                        r = static_cast<float>(*v);
                    else if (auto *vi = std::get_if<int64_t>(&radiusProp->value))
                        r = static_cast<float>(*vi);
                    out << ind << "    drawRoundRect(\n"
                        << ind << "        color = Color.Gray,\n"
                        << ind << "        cornerRadius = CornerRadius(" << r << ".dp.toPx())\n"
                        << ind << "    )\n";
                }

                // حدود → drawRect مع StrokeStyle
                const auto *borderProp = node.findProperty("\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf");
                if (borderProp)
                {
                    out << ind << "    drawRect(\n"
                        << ind << "        color = Color.Black,\n"
                        << ind << "        style = Stroke(width = 1.dp.toPx())\n"
                        << ind << "    )\n";
                }

                // دائرة → drawCircle
                const auto *circleProp = node.findProperty("\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd8\xa9");
                if (circleProp)
                {
                    out << ind << "    drawCircle(\n"
                        << ind << "        color = Color.Blue,\n"
                        << ind << "        radius = size.minDimension / 2f\n"
                        << ind << "    )\n";
                }

                // رسم الأبناء إذا وُجدوا
                for (const auto &child : node.getChildren())
                {
                    generateCanvasWidget(out, *child, indentLevel + 1);
                }

                out << ind << "}\n";
            }

            std::string ComposeCodegen::generateModifiers(const IRNode &node)
            {
                std::ostringstream mod;
                mod << "modifier = Modifier";

                for (const auto &prop : node.getProperties())
                {
                    if (prop.key == props::PADDING) // SoT «حشوة»
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".padding(" << *v << ".dp)";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            mod << ".padding(" << *vi << ".dp)";
                    }
                    else if (prop.key == "عرض")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".width(" << *v << ".dp)";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            mod << ".width(" << *vi << ".dp)";
                    }
                    else if (prop.key == "ارتفاع")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".height(" << *v << ".dp)";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            mod << ".height(" << *vi << ".dp)";
                    }
                    else if (prop.key == "نصف_قطر" || prop.key == "زوايا")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".clip(RoundedCornerShape(" << *v << ".dp))";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            mod << ".clip(RoundedCornerShape(" << *vi << ".dp))";
                    }
                    else if (prop.key == "هامش")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".padding(" << *v << ".dp)";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            mod << ".padding(" << *vi << ".dp)";
                    }
                    else if (prop.key == "ظل" || prop.key == "رفع")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".shadow(elevation = " << *v << ".dp, shape = RoundedCornerShape(8.dp))";
                        else if (auto *vi = std::get_if<int64_t>(&prop.value))
                            mod << ".shadow(elevation = " << *vi << ".dp, shape = RoundedCornerShape(8.dp))";
                    }
                    else if (prop.key == "حد_لون")
                    {
                        if (auto *v = std::get_if<std::string>(&prop.value))
                            mod << ".border(1.dp, " << arabicColorToKotlin(*v) << ")";
                    }
                    else if (prop.key == "حد_سماكة")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".border(" << *v << ".dp, Color.Gray)";
                    }
                    else if (prop.key == "لون_خلفية" || prop.key == "خلفية")
                    {
                        if (auto *v = std::get_if<std::string>(&prop.value))
                            mod << ".background(" << arabicColorToKotlin(*v) << ")";
                    }
                    else if (prop.key == "شفافية")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".alpha(" << *v << "f)";
                    }
                    else if (prop.key == "دوران")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".rotate(" << *v << "f)";
                    }
                    else if (prop.key == "مقياس")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".scale(" << *v << "f)";
                    }
                    else if (prop.key == "أدنى_عرض")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".widthIn(min = " << *v << ".dp)";
                    }
                    else if (prop.key == "أقصى_عرض")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".widthIn(max = " << *v << ".dp)";
                    }
                    else if (prop.key == "أدنى_ارتفاع")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".heightIn(min = " << *v << ".dp)";
                    }
                    else if (prop.key == "أقصى_ارتفاع")
                    {
                        if (auto *v = std::get_if<double>(&prop.value))
                            mod << ".heightIn(max = " << *v << ".dp)";
                    }
                    else if (prop.key == "ملء_العرض")
                    {
                        if (auto *b = std::get_if<bool>(&prop.value); b && *b)
                            mod << ".fillMaxWidth()";
                    }
                    else if (prop.key == "ملء_الارتفاع")
                    {
                        if (auto *b = std::get_if<bool>(&prop.value); b && *b)
                            mod << ".fillMaxHeight()";
                    }
                    else if (prop.key == "ملء")
                    {
                        if (auto *b = std::get_if<bool>(&prop.value); b && *b)
                            mod << ".fillMaxSize()";
                    }
                    else if (prop.key == "وصف")
                    {
                        // إمكانية الوصول (Accessibility)
                        if (auto *v = std::get_if<std::string>(&prop.value))
                            mod << ".semantics { contentDescription = \"" << *v << "\" }";
                    }
                }

                return mod.str();
            }

            void ComposeCodegen::generatePreview(std::ostringstream &out, const std::string &funcName)
            {
                out << "@Preview(showBackground = true)\n"
                    << "@Composable\n"
                    << "fun " << funcName << "Preview() {\n"
                    << "    MaterialTheme {\n"
                    << "        " << funcName << "()\n"
                    << "    }\n"
                    << "}\n";
            }

            // ─── أدوات مساعدة ────────────────────────────

            std::string ComposeCodegen::indent(int level) const
            {
                return std::string(level * options_.indentSize, ' ');
            }

            std::string ComposeCodegen::nodeTypeToComposable(UINodeType type) const
            {
                switch (type)
                {
                case UINodeType::Column:
                    return "Column";
                case UINodeType::Row:
                    return "Row";
                case UINodeType::Box:
                    return "Box";
                case UINodeType::Stack:
                    return "Box";
                case UINodeType::Text:
                    return "Text";
                case UINodeType::Button:
                    return "Button";
                case UINodeType::Image:
                    return "Image";
                case UINodeType::TextField:
                    return "TextField";
                case UINodeType::Toggle:
                    return "Switch";
                case UINodeType::Slider:
                    return "Slider";
                case UINodeType::Checkbox:
                    return "Checkbox";
                case UINodeType::Radio:
                    return "RadioButton";
                case UINodeType::ScrollView:
                    return "LazyColumn";
                case UINodeType::List:
                    return "LazyColumn";
                case UINodeType::Spacer:
                    return "Spacer";
                case UINodeType::Divider:
                    return "Divider";
                case UINodeType::Card:
                    return "Card";
                case UINodeType::Scaffold:
                    return "Scaffold";
                case UINodeType::AppBar:
                    return "TopAppBar";
                case UINodeType::ProgressBar:
                    return "LinearProgressIndicator";
                case UINodeType::SearchBar:
                    return "SearchBar";
                case UINodeType::FAB:
                    return "FloatingActionButton";
                case UINodeType::SnackBar:
                    return "Snackbar";
                case UINodeType::Badge:
                    return "BadgedBox";
                case UINodeType::Chip:
                    return "FilterChip";
                case UINodeType::Avatar:
                    return "Box";
                case UINodeType::Drawer:
                    return "ModalNavigationDrawer";
                case UINodeType::BottomSheet:
                    return "ModalBottomSheet";
                case UINodeType::TabView:
                    return "TabRow";
                case UINodeType::NavigationView:
                    return "NavigationHost";
                case UINodeType::Grid:
                    return "LazyVerticalGrid";
                case UINodeType::LazyColumn:
                    return "LazyColumn";
                case UINodeType::LazyRow:
                    return "LazyRow";
                case UINodeType::Picker:
                    return "ExposedDropdownMenuBox";
                case UINodeType::Icon:
                    return "Icon";
                case UINodeType::Canvas:
                    return "Canvas";
                case UINodeType::Container:
                    return "Box";
                case UINodeType::Surface:
                    return "Surface";
                case UINodeType::Alert:
                    return "AlertDialog";
                case UINodeType::Dialog:
                    return "AlertDialog";
                // ── v3 Data ──
                case UINodeType::Tooltip:
                    return "PlainTooltip";
                case UINodeType::DataTable:
                    return "LazyColumn";
                case UINodeType::TreeView:
                    return "LazyColumn";
                case UINodeType::Breadcrumb:
                    return "Row";
                case UINodeType::Pagination:
                    return "Row";
                case UINodeType::Timeline:
                    return "LazyColumn";
                // ── v3 Media ──
                case UINodeType::Carousel:
                    return "HorizontalPager";
                case UINodeType::RichText:
                    return "Text";
                case UINodeType::Markdown:
                    return "Text";
                case UINodeType::CodeBlock:
                    return "Text";
                case UINodeType::AudioPlayer:
                    return "Box";
                case UINodeType::ImageGallery:
                    return "LazyVerticalGrid";
                // ── v3 Animation ──
                case UINodeType::Skeleton:
                    return "Box";
                case UINodeType::Shimmer:
                    return "Box";
                case UINodeType::AnimatedList:
                    return "LazyColumn";
                case UINodeType::Expandable:
                    return "AnimatedVisibility";
                case UINodeType::Collapsible:
                    return "AnimatedVisibility";
                case UINodeType::Swipeable:
                    return "SwipeToDismissBox";
                // ── v3 Controls ──
                case UINodeType::Stepper:
                    return "Row";
                case UINodeType::SegmentedControl:
                    return "SegmentedButton";
                case UINodeType::RatingBar:
                    return "Row";
                case UINodeType::TimePicker:
                    return "TimePicker";
                case UINodeType::Calendar:
                    return "DatePicker";
                case UINodeType::ColorWheel:
                    return "Box";
                // ── v3 Navigation ──
                case UINodeType::BottomNav:
                    return "NavigationBar";
                case UINodeType::SideNav:
                    return "NavigationRail";
                case UINodeType::Toolbar:
                    return "TopAppBar";
                case UINodeType::SplitView:
                    return "Row";
                case UINodeType::Tabs:
                    return "TabRow";
                case UINodeType::MegaMenu:
                    return "DropdownMenu";
                default:
                    return "Box";
                }
            }

        } // namespace android
    } // namespace ui
} // namespace sad
