/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: appkit_codegen.cpp
 * المسار: features/graphics/backends/macos/src/appkit_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ مولّد كود AppKit/Cocoa لمنصة macOS.
 *
 * يحوّل كل عقدة IR إلى مكوّن AppKit أصلي:
 *   Column → NSStackView(.vertical)
 *   Row → NSStackView(.horizontal)
 *   Text → NSTextField(labelWithString:)
 *   Button → NSButton(title:target:action:)
 *   Image → NSImageView
 *   Toggle → NSSwitch
 *   Slider → NSSlider
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/macos/appkit_codegen.h"
#include "sad_ui/color_utils.h"  // أدوات تحويل الألوان
#include "sad_ui/prop_keys.h"    // مصدر الحقيقة لمفاتيح الخصائص (لا سلاسل حرفيّة)
#include <locale>  // (AR) std::locale::classic — لا تعتمد على تضمينٍ عبوريّ

namespace sad {
namespace ui {
namespace macos {

namespace {
// (AR) نصّ العنصر بالترتيب القانونيّ («عنوان» أوّلًا) مع بدائل وهروب محارف
//   Swift (\ و") كي لا يكسر عنوانٌ فيهما الكودَ. كان الفرع يقرأ "text" وحده
//   فتظهر NSButton/NSTextField فارغة.
inline std::string appkitLabel(const IRNode &node) {
    static const char *keys[] = {props::TITLE, props::ICON, props::TEXT_LATIN, props::TEXT, props::CONTENT};
    std::string raw;
    for (const char *k : keys) {
        const auto *p = node.findProperty(k);
        if (!p) continue;
        if (auto *s = std::get_if<std::string>(&p->value)) { raw = *s; break; }
        if (auto *iv = std::get_if<int64_t>(&p->value)) { raw = std::to_string(*iv); break; }
        if (auto *dv = std::get_if<double>(&p->value)) { raw = std::to_string(*dv); break; }
    }
    std::string out;
    for (char c : raw) {
        if (c == '\\' || c == '"') out += '\\';
        out += c;
    }
    return out;
}

// (AR) قيمةٌ عدديّةٌ من خاصّيّةٍ ببديلٍ عند غيابها، بصيغةٍ لا تتأثّر بالمحلّيّةِ
//   كي لا ينقلب الفاصلُ العشريُّ فاصلةً فيكسر كودَ Swift المولَّد.
inline std::string appkitNumber(const IRNode &node, const char *key, double fallback) {
    double value = fallback;
    if (const auto *p = node.findProperty(key)) {
        if (auto *d = std::get_if<double>(&p->value)) value = *d;
        else if (auto *iv = std::get_if<int64_t>(&p->value)) value = static_cast<double>(*iv);
    }
    std::ostringstream ss;
    ss.imbue(std::locale::classic());
    ss << value;
    return ss.str();
}

// (AR) قيدُ حدٍّ أقصى: غيابُ المفتاحِ يعني «بلا حدّ» فلا يُصدَر قيدٌ أصلًا.
//   قيدُ ‎<= 0‎ مع قيدِ ‎>= min‎ غيرُ قابلٍ للإرضاءِ فيكسر تخطيطَ AppKit كلَّه.
inline std::string appkitBound(const char *dimension, const IRNode &node,
                               const char *key, const std::string &ind) {
    if (!node.findProperty(key)) return "";
    return ind + "container." + dimension +
           "Anchor.constraint(lessThanOrEqualToConstant: " +
           appkitNumber(node, key, 0.0) + ").isActive = true\n";
}

// (AR) قيدُ المحاذاةِ المقابلُ لقيمةِ «محاذاة». البدايةُ في لغةٍ عربيّةٍ يمينُ
//   الشاشة، وAppKit يعبّر عنها بـ leadingAnchor المُدرِكةِ للاتّجاه.
inline std::string appkitAlignmentConstraint(const IRNode &node) {
    std::string value;
    if (const auto *p = node.findProperty(props::ALIGN)) {
        if (auto *s = std::get_if<std::string>(&p->value)) value = *s;
    }
    if (value == propval::ALIGN_CENTER_AR || value == propval::ALIGN_CENTER_EN)
        return "view_.centerXAnchor.constraint(equalTo: container.centerXAnchor).isActive = true";
    if (value == propval::ALIGN_RIGHT_AR || value == propval::ALIGN_RIGHT_EN)
        return "view_.trailingAnchor.constraint(equalTo: container.trailingAnchor).isActive = true";
    return "view_.leadingAnchor.constraint(equalTo: container.leadingAnchor).isActive = true";
}
} // namespace

// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ AppKitCodegen
// ═══════════════════════════════════════════════════════════════════════════════

AppKitCodegen::AppKitCodegen(const AppKitCodegenOptions& options)
    : options_(options)
{
}

std::string AppKitCodegen::generate(const IRModule& module) {
    std::ostringstream out;

    // الاستيرادات
    generateImports(out);
    out << "\n";

    // ViewController
    generateViewController(out, module);

    // WindowController
    if (options_.generateWindowController) {
        out << "\n";
        generateWindowController(out, module);
    }

    return out.str();
}

std::string AppKitCodegen::generateNode(const IRNode& node, int indentLevel) {
    std::ostringstream out;

    if (node.getRenderingHint() == RenderingHint::Canvas) {
        generateCanvasWidget(out, node, indentLevel);
    } else {
        generateNativeWidget(out, node, indentLevel);
    }

    return out.str();
}

// ─── توليد الأجزاء ────────────────────────────

void AppKitCodegen::generateImports(std::ostringstream& out) {
    out << "import Cocoa\n"
        << "import AppKit\n";
}

void AppKitCodegen::generateViewController(std::ostringstream& out, const IRModule& module) {
    std::string className = module.name + "ViewController";

    out << "class " << className << ": NSViewController {\n\n";

    // متغيرات الحالة
    generateState(out, module.stateDefinitions, 1);
    if (!module.stateDefinitions.empty()) out << "\n";

    // viewDidLoad
    out << indent(1) << "override func viewDidLoad() {\n"
        << indent(2) << "super.viewDidLoad()\n"
        << indent(2) << "setupUI()\n"
        << indent(1) << "}\n\n";

    // setupUI — مع تحسين أداء CATransaction لتجميع التحديثات
    out << indent(1) << "private func setupUI() {\n"
        << indent(2) << "// ─── تحسين أداء: تجميع تحديثات الواجهة ───\n"
        << indent(2) << "CATransaction.begin()\n"
        << indent(2) << "CATransaction.setDisableActions(true)\n\n";

    if (module.root) {
        out << indent(2) << "let rootView = createRootView()\n"
            << indent(2) << "view.addSubview(rootView)\n";
        if (options_.useAutoLayout) {
            out << indent(2) << "rootView.translatesAutoresizingMaskIntoConstraints = false\n"
                << indent(2) << "NSLayoutConstraint.activate([\n"
                << indent(3) << "rootView.topAnchor.constraint(equalTo: view.topAnchor),\n"
                << indent(3) << "rootView.leadingAnchor.constraint(equalTo: view.leadingAnchor),\n"
                << indent(3) << "rootView.trailingAnchor.constraint(equalTo: view.trailingAnchor),\n"
                << indent(3) << "rootView.bottomAnchor.constraint(equalTo: view.bottomAnchor)\n"
                << indent(2) << "])\n";
        }
    }

    out << "\n" << indent(2) << "CATransaction.commit()\n"
        << indent(1) << "}\n\n";

    // createRootView
    out << indent(1) << "private func createRootView() -> NSView {\n";
    if (module.root) {
        out << generateNode(*module.root, 2);
    } else {
        out << indent(2) << "return NSView()\n";
    }
    out << indent(1) << "}\n";

    out << "}\n";
}

void AppKitCodegen::generateWindowController(std::ostringstream& out, const IRModule& module) {
    std::string vcName = module.name + "ViewController";

    out << "class " << module.name << "WindowController: NSWindowController {\n\n"
        << indent(1) << "convenience init() {\n"
        << indent(2) << "let viewController = " << vcName << "()\n"
        << indent(2) << "let window = NSWindow(\n"
        << indent(3) << "contentRect: NSRect(x: 0, y: 0, width: 800, height: 600),\n"
        << indent(3) << "styleMask: [.titled, .closable, .miniaturizable, .resizable],\n"
        << indent(3) << "backing: .buffered,\n"
        << indent(3) << "defer: false\n"
        << indent(2) << ")\n"
        << indent(2) << "window.contentViewController = viewController\n"
        << indent(2) << "window.title = \"" << module.name << "\"\n"
        << indent(2) << "window.center()\n"
        << indent(2) << "self.init(window: window)\n"
        << indent(1) << "}\n"
        << "}\n";
}

void AppKitCodegen::generateState(
    std::ostringstream& out,
    const std::unordered_map<std::string, std::string>& states,
    int indentLevel
) {
    for (const auto& [name, type] : states) {
        out << indent(indentLevel);

        std::string swiftType = "Any";
        std::string defaultVal = "nil";

        if (type == "رقم") {
            swiftType = "Int";
            defaultVal = "0";
        } else if (type == "عشري") {
            swiftType = "Double";
            defaultVal = "0.0";
        } else if (type == "نص") {
            swiftType = "String";
            defaultVal = "\"\"";
        } else if (type == "منطقي") {
            swiftType = "Bool";
            defaultVal = "false";
        }

        out << "@objc dynamic var " << name
            << ": " << swiftType << " = " << defaultVal << "\n";
    }
}

void AppKitCodegen::generateNativeWidget(
    std::ostringstream& out,
    const IRNode& node,
    int indentLevel
) {
    std::string ind = indent(indentLevel);

    switch (node.getType()) {
        case UINodeType::Column: {
            out << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .vertical\n"
                << ind << "stackView.alignment = .centerX\n"
                << ind << "stackView.spacing = 8\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "stackView.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = stackView\n";
            break;
        }

        case UINodeType::Row: {
            out << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .horizontal\n"
                << ind << "stackView.alignment = .centerY\n"
                << ind << "stackView.spacing = 8\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "stackView.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = stackView\n";
            break;
        }

        case UINodeType::Text: {
            std::string text = appkitLabel(node);
            out << ind << "let label = NSTextField(labelWithString: \"" << text << "\")\n"
                << ind << "label.isBezeled = false\n"
                << ind << "label.isEditable = false\n"
                << ind << "label.drawsBackground = false\n";

            const auto* sizeProp = node.findProperty(props::FONT_SIZE);
            if (sizeProp) {
                if (auto* v = std::get_if<double>(&sizeProp->value))
                    out << ind << "label.font = NSFont.systemFont(ofSize: " << *v << ")\n";
            }
            out << ind << "let view_ = label\n";
            break;
        }

        case UINodeType::Button: {
            std::string text = appkitLabel(node);
            out << ind << "let button = NSButton(title: \"" << text
                << "\", target: self, action: #selector(buttonClicked(_:)))\n"
                << ind << "button.bezelStyle = .rounded\n"
                << ind << "let view_ = button\n";
            break;
        }

        case UINodeType::Image: {
            const auto* srcProp = node.findProperty(props::SOURCE_LATIN);
            std::string src;
            if (srcProp) {
                if (auto* s = std::get_if<std::string>(&srcProp->value))
                    src = *s;
            }
            out << ind << "let imageView = NSImageView()\n"
                << ind << "imageView.image = NSImage(named: \"" << src << "\")\n"
                << ind << "imageView.imageScaling = .scaleProportionallyUpOrDown\n"
                << ind << "let view_ = imageView\n";
            break;
        }

        case UINodeType::TextField: {
            out << ind << "let textField = NSTextField()\n"
                << ind << "textField.placeholderString = \"\"\n"
                << ind << "let view_ = textField\n";
            break;
        }

        case UINodeType::Toggle: {
            out << ind << "let toggle = NSSwitch()\n"
                << ind << "toggle.target = self\n"
                << ind << "let view_ = toggle\n";
            break;
        }

        case UINodeType::Slider: {
            out << ind << "let slider = NSSlider()\n"
                << ind << "slider.minValue = 0\n"
                << ind << "slider.maxValue = 100\n"
                << ind << "slider.target = self\n"
                << ind << "let view_ = slider\n";
            break;
        }

        case UINodeType::Checkbox: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string text;
            if (textProp) {
                if (auto* s = std::get_if<std::string>(&textProp->value))
                    text = *s;
            }
            out << ind << "let checkbox = NSButton(checkboxWithTitle: \"" << text
                << "\", target: self, action: nil)\n"
                << ind << "let view_ = checkbox\n";
            break;
        }

        case UINodeType::Radio: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string text;
            if (textProp) {
                if (auto* s = std::get_if<std::string>(&textProp->value))
                    text = *s;
            }
            out << ind << "let radio = NSButton(radioButtonWithTitle: \"" << text
                << "\", target: self, action: nil)\n"
                << ind << "let view_ = radio\n";
            break;
        }

        case UINodeType::Picker: {
            out << ind << "let popup = NSPopUpButton()\n";
            for (const auto& child : node.getChildren()) {
                const auto* tp = child->findProperty(props::TEXT_LATIN);
                if (tp) {
                    if (auto* s = std::get_if<std::string>(&tp->value))
                        out << ind << "popup.addItem(withTitle: \"" << *s << "\")\n";
                }
            }
            out << ind << "let view_ = popup\n";
            break;
        }

        case UINodeType::ProgressBar: {
            out << ind << "let progress = NSProgressIndicator()\n"
                << ind << "progress.style = .bar\n"
                << ind << "progress.isIndeterminate = false\n"
                << ind << "let view_ = progress\n";
            break;
        }

        case UINodeType::ScrollView: {
            out << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.hasVerticalScroller = true\n"
                << ind << "scrollView.hasHorizontalScroller = false\n";
            if (!node.getChildren().empty()) {
                out << generateNode(*node.getChildren()[0], indentLevel);
                out << ind << "scrollView.documentView = view_\n";
            }
            out << ind << "let view_ = scrollView\n";
            break;
        }

        case UINodeType::Divider: {
            out << ind << "let separator = NSBox()\n"
                << ind << "separator.boxType = .separator\n"
                << ind << "let view_ = separator\n";
            break;
        }

        case UINodeType::Spacer: {
            out << ind << "let spacer = NSView()\n"
                << ind << "spacer.setContentHuggingPriority(.defaultLow, for: .vertical)\n"
                << ind << "let view_ = spacer\n";
            break;
        }

        case UINodeType::Card: {
            out << ind << "let card = NSBox()\n"
                << ind << "card.boxType = .custom\n"
                << ind << "card.borderType = .lineBorder\n"
                << ind << "card.cornerRadius = 8\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "card.contentView?.addSubview(view_)\n";
            }
            out << ind << "let view_ = card\n";
            break;
        }

        case UINodeType::TabView: {
            out << ind << "let tabView = NSTabView()\n";
            for (const auto& child : node.getChildren()) {
                const auto* tp = child->findProperty(props::TEXT_LATIN);
                std::string title = "Tab";
                if (tp) {
                    if (auto* s = std::get_if<std::string>(&tp->value))
                        title = *s;
                }
                out << ind << "let tabItem = NSTabViewItem()\n"
                    << ind << "tabItem.label = \"" << title << "\"\n"
                    << ind << "tabView.addTabViewItem(tabItem)\n";
            }
            out << ind << "let view_ = tabView\n";
            break;
        }

        case UINodeType::SearchBar: {
            out << ind << "let searchField = NSSearchField()\n";
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            if (textProp) {
                if (auto* s = std::get_if<std::string>(&textProp->value))
                    out << ind << "searchField.placeholderString = \"" << *s << "\"\n";
            }
            out << ind << "let view_ = searchField\n";
            break;
        }

        case UINodeType::Icon: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string iconName = "star";
            if (textProp) {
                if (auto* s = std::get_if<std::string>(&textProp->value))
                    iconName = *s;
            }
            out << ind << "let imageView = NSImageView()\n"
                << ind << "imageView.image = NSImage(systemSymbolName: \"" << iconName
                << "\", accessibilityDescription: nil)\n"
                << ind << "let view_ = imageView\n";
            break;
        }

        case UINodeType::Scaffold: {
            out << ind << "let container = NSStackView()\n"
                << ind << "container.orientation = .vertical\n"
                << ind << "container.spacing = 0\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::AppBar: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string title;
            if (textProp) {
                if (auto* s = std::get_if<std::string>(&textProp->value))
                    title = *s;
            }
            out << ind << "let toolbar = NSStackView()\n"
                << ind << "toolbar.orientation = .horizontal\n"
                << ind << "toolbar.spacing = 8\n"
                << ind << "let titleLabel = NSTextField(labelWithString: \"" << title << "\")\n"
                << ind << "titleLabel.font = NSFont.boldSystemFont(ofSize: 18)\n"
                << ind << "toolbar.addArrangedSubview(titleLabel)\n"
                << ind << "let view_ = toolbar\n";
            break;
        }

        case UINodeType::FAB: {
            out << ind << "let fab = NSButton()\n"
                << ind << "fab.bezelStyle = .circular\n"
                << ind << "fab.isBordered = true\n";
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            if (textProp) {
                if (auto* s = std::get_if<std::string>(&textProp->value))
                    out << ind << "fab.title = \"" << *s << "\"\n";
            }
            out << ind << "let view_ = fab\n";
            break;
        }

        case UINodeType::Badge: {
            out << ind << "let badge = NSTextField(labelWithString: \"\")\n"
                << ind << "badge.isBezeled = true\n"
                << ind << "badge.bezelStyle = .roundedRect\n"
                << ind << "badge.backgroundColor = .systemRed\n"
                << ind << "badge.textColor = .white\n"
                << ind << "badge.alignment = .center\n";
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            if (textProp) {
                if (auto* s = std::get_if<std::string>(&textProp->value))
                    out << ind << "badge.stringValue = \"" << *s << "\"\n";
            }
            out << ind << "let view_ = badge\n";
            break;
        }

        case UINodeType::Chip: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string text;
            if (textProp) {
                if (auto* s = std::get_if<std::string>(&textProp->value))
                    text = *s;
            }
            out << ind << "let chip = NSButton(title: \"" << text << "\", target: self, action: nil)\n"
                << ind << "chip.bezelStyle = .roundRect\n"
                << ind << "let view_ = chip\n";
            break;
        }

        case UINodeType::Avatar: {
            out << ind << "let avatar = NSImageView()\n"
                << ind << "avatar.imageScaling = .scaleProportionallyUpOrDown\n"
                << ind << "avatar.wantsLayer = true\n"
                << ind << "avatar.layer?.cornerRadius = 20\n"
                << ind << "avatar.layer?.masksToBounds = true\n";
            const auto* srcProp = node.findProperty(props::TEXT_LATIN);
            if (srcProp) {
                if (auto* s = std::get_if<std::string>(&srcProp->value))
                    out << ind << "avatar.image = NSImage(named: \"" << *s << "\")\n";
            }
            out << ind << "let view_ = avatar\n";
            break;
        }

        case UINodeType::SnackBar: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string msg;
            if (textProp) {
                if (auto* s = std::get_if<std::string>(&textProp->value))
                    msg = *s;
            }
            out << ind << "let snackBar = NSTextField(labelWithString: \"" << msg << "\")\n"
                << ind << "snackBar.backgroundColor = .darkGray\n"
                << ind << "snackBar.textColor = .white\n"
                << ind << "let view_ = snackBar\n";
            break;
        }

        case UINodeType::Drawer: {
            out << ind << "let drawer = NSStackView()\n"
                << ind << "drawer.orientation = .vertical\n"
                << ind << "drawer.spacing = 4\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "drawer.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = drawer\n";
            break;
        }

        case UINodeType::BottomSheet:
        case UINodeType::Alert:
        case UINodeType::Dialog: {
            // حوارات — تُعرض كـ NSAlert / NSPanel
            out << ind << "let panel = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "panel.addSubview(view_)\n";
            }
            out << ind << "let view_ = panel\n";
            break;
        }

        case UINodeType::Grid: {
            out << ind << "let gridView = NSGridView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "gridView.addRow(with: [view_])\n";
            }
            out << ind << "let view_ = gridView\n";
            break;
        }

        case UINodeType::List:
        case UINodeType::LazyColumn: {
            out << ind << "let scrollView = NSScrollView()\n"
                << ind << "let tableView = NSTableView()\n"
                << ind << "scrollView.documentView = tableView\n"
                << ind << "scrollView.hasVerticalScroller = true\n"
                // ─── تحسين أداء: طبقة رسم مسرّعة ───
                << ind << "scrollView.wantsLayer = true\n"
                << ind << "tableView.usesAlternatingRowBackgroundColors = true\n"
                << ind << "let view_ = scrollView\n";
            break;
        }

        case UINodeType::LazyRow: {
            out << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.hasHorizontalScroller = true\n"
                << ind << "scrollView.hasVerticalScroller = false\n"
                << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .horizontal\n"
                << ind << "scrollView.documentView = stackView\n"
                << ind << "let view_ = scrollView\n";
            break;
        }

        case UINodeType::NavigationView: {
            out << ind << "let splitView = NSSplitView()\n"
                << ind << "splitView.isVertical = true\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "splitView.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = splitView\n";
            break;
        }

        case UINodeType::Canvas: {
            out << ind << "let canvasView = NSView()\n"
                << ind << "canvasView.wantsLayer = true\n"
                << ind << "let view_ = canvasView\n";
            break;
        }

        // ── §11 مكونات البيانات (Data Components — v3) ──

        case UINodeType::Tooltip: {
            out << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "container.toolTip = \"تلميح\"\n"
                << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::DataTable: {
            out << ind << "let tableView = NSTableView()\n"
                << ind << "tableView.usesAlternatingRowBackgroundColors = true\n"
                << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.documentView = tableView\n"
                << ind << "let view_ = scrollView\n";
            break;
        }

        case UINodeType::TreeView: {
            out << ind << "let outlineView = NSOutlineView()\n"
                << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.documentView = outlineView\n"
                << ind << "let view_ = scrollView\n";
            break;
        }

        case UINodeType::Breadcrumb: {
            out << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .horizontal\n"
                << ind << "stackView.spacing = 4\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "stackView.addArrangedSubview(view_)\n";
                out << ind << "let sep = NSTextField(labelWithString: \" > \")\n"
                    << ind << "stackView.addArrangedSubview(sep)\n";
            }
            out << ind << "let view_ = stackView\n";
            break;
        }

        case UINodeType::Pagination: {
            out << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .horizontal\n"
                << ind << "stackView.spacing = 4\n"
                << ind << "let prevBtn = NSButton(title: \"«\", target: nil, action: nil)\n"
                << ind << "stackView.addArrangedSubview(prevBtn)\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "stackView.addArrangedSubview(view_)\n";
            }
            out << ind << "let nextBtn = NSButton(title: \"»\", target: nil, action: nil)\n"
                << ind << "stackView.addArrangedSubview(nextBtn)\n"
                << ind << "let view_ = stackView\n";
            break;
        }

        case UINodeType::Timeline: {
            out << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .vertical\n"
                << ind << "stackView.spacing = 16\n";
            for (const auto& child : node.getChildren()) {
                out << ind << "let row = NSStackView()\n"
                    << ind << "row.orientation = .horizontal\n"
                    << ind << "let dot = NSView()\n"
                    << ind << "dot.wantsLayer = true\n"
                    << ind << "dot.layer?.backgroundColor = NSColor.blue.cgColor\n"
                    << ind << "dot.layer?.cornerRadius = 6\n"
                    << ind << "dot.frame = NSRect(x: 0, y: 0, width: 12, height: 12)\n"
                    << ind << "row.addArrangedSubview(dot)\n";
                out << generateNode(*child, indentLevel);
                out << ind << "row.addArrangedSubview(view_)\n"
                    << ind << "stackView.addArrangedSubview(row)\n";
            }
            out << ind << "let view_ = stackView\n";
            break;
        }

        // ── §12 مكونات الوسائط (Media Components — v3) ──

        case UINodeType::Carousel: {
            out << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .horizontal\n"
                << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.hasHorizontalScroller = true\n"
                << ind << "scrollView.documentView = stackView\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "stackView.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = scrollView\n";
            break;
        }

        case UINodeType::RichText: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string text = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "") : "";
            out << ind << "let textView = NSTextView()\n"
                << ind << "textView.isRichText = true\n"
                << ind << "textView.string = \"" << text << "\"\n"
                << ind << "let view_ = textView\n";
            break;
        }

        case UINodeType::Markdown: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string text = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "") : "";
            out << ind << "let textView = NSTextView()\n"
                << ind << "textView.string = \"" << text << "\"\n"
                << ind << "let view_ = textView\n";
            break;
        }

        case UINodeType::CodeBlock: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string text = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "") : "";
            out << ind << "let textView = NSTextView()\n"
                << ind << "textView.font = NSFont.monospacedSystemFont(ofSize: 13, weight: .regular)\n"
                << ind << "textView.string = \"" << text << "\"\n"
                << ind << "textView.isEditable = false\n"
                << ind << "textView.backgroundColor = NSColor.windowBackgroundColor\n"
                << ind << "let view_ = textView\n";
            break;
        }

        case UINodeType::AudioPlayer: {
            out << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .horizontal\n"
                << ind << "let playBtn = NSButton(title: \"▶\", target: nil, action: nil)\n"
                << ind << "let slider = NSSlider(value: 0, minValue: 0, maxValue: 100, target: nil, action: nil)\n"
                << ind << "let timeLabel = NSTextField(labelWithString: \"00:00\")\n"
                << ind << "stackView.addArrangedSubview(playBtn)\n"
                << ind << "stackView.addArrangedSubview(slider)\n"
                << ind << "stackView.addArrangedSubview(timeLabel)\n"
                << ind << "let view_ = stackView\n";
            break;
        }

        case UINodeType::ImageGallery: {
            out << ind << "let collectionView = NSCollectionView()\n"
                << ind << "let layout = NSCollectionViewFlowLayout()\n"
                << ind << "layout.itemSize = NSSize(width: 100, height: 100)\n"
                << ind << "collectionView.collectionViewLayout = layout\n"
                << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.documentView = collectionView\n"
                << ind << "let view_ = scrollView\n";
            break;
        }

        // ── §13 مكونات الحركة (Animation Components — v3) ──

        case UINodeType::Skeleton:
        case UINodeType::Shimmer: {
            out << ind << "let placeholder = NSView()\n"
                << ind << "placeholder.wantsLayer = true\n"
                << ind << "placeholder.layer?.backgroundColor = NSColor.lightGray.cgColor\n"
                << ind << "placeholder.layer?.cornerRadius = 4\n"
                << ind << "let view_ = placeholder\n";
            break;
        }

        case UINodeType::AnimatedList: {
            out << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .vertical\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "stackView.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = stackView\n";
            break;
        }

        case UINodeType::Expandable:
        case UINodeType::Collapsible: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string title = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "عنصر") : "عنصر";
            out << ind << "let disclosure = NSStackView()\n"
                << ind << "disclosure.orientation = .vertical\n"
                << ind << "let header = NSButton(title: \"" << title << "\", target: nil, action: nil)\n"
                << ind << "header.bezelStyle = .disclosure\n"
                << ind << "disclosure.addArrangedSubview(header)\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "disclosure.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = disclosure\n";
            break;
        }

        case UINodeType::Swipeable: {
            out << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        // ── §14 مكونات التحكم المتقدمة (Advanced Controls — v3) ──

        case UINodeType::Stepper: {
            out << ind << "let stepper = NSStepper()\n"
                << ind << "stepper.minValue = 0\n"
                << ind << "stepper.maxValue = 100\n"
                << ind << "stepper.integerValue = 0\n"
                << ind << "let view_ = stepper\n";
            break;
        }

        case UINodeType::SegmentedControl: {
            out << ind << "let segmented = NSSegmentedControl()\n"
                << ind << "segmented.segmentCount = " << node.childCount() << "\n";
            {
                int segIdx = 0;
                for (const auto& child : node.getChildren()) {
                    (void)child;
                    out << ind << "segmented.setLabel(\"عنصر " << segIdx << "\", forSegment: " << segIdx << ")\n";
                    segIdx++;
                }
            }
            out << ind << "let view_ = segmented\n";
            break;
        }

        case UINodeType::RatingBar: {
            out << ind << "let stackView = NSStackView()\n"
                << ind << "stackView.orientation = .horizontal\n"
                << ind << "for i in 0..<5 {\n"
                << ind << "    let star = NSButton(title: i < 3 ? \"★\" : \"☆\", target: nil, action: nil)\n"
                << ind << "    star.isBordered = false\n"
                << ind << "    stackView.addArrangedSubview(star)\n"
                << ind << "}\n"
                << ind << "let view_ = stackView\n";
            break;
        }

        case UINodeType::DatePicker: {
            out << ind << "let datePicker = NSDatePicker()\n"
                << ind << "datePicker.datePickerStyle = .textFieldAndStepper\n"
                << ind << "datePicker.datePickerElements = .yearMonthDay\n"
                << ind << "let view_ = datePicker\n";
            break;
        }

        case UINodeType::TimePicker: {
            out << ind << "let datePicker = NSDatePicker()\n"
                << ind << "datePicker.datePickerStyle = .textFieldAndStepper\n"
                << ind << "datePicker.datePickerElements = .hourMinute\n"
                << ind << "let view_ = datePicker\n";
            break;
        }

        case UINodeType::Calendar: {
            out << ind << "let datePicker = NSDatePicker()\n"
                << ind << "datePicker.datePickerStyle = .clockAndCalendar\n"
                << ind << "datePicker.datePickerElements = .yearMonthDay\n"
                << ind << "let view_ = datePicker\n";
            break;
        }

        case UINodeType::ColorPicker:
        case UINodeType::ColorWheel: {
            out << ind << "let colorWell = NSColorWell()\n"
                << ind << "colorWell.color = .blue\n"
                << ind << "let view_ = colorWell\n";
            break;
        }

        // ── §15 مكونات التنقل المتقدمة (Extended Navigation — v3) ──

        case UINodeType::BottomNav:
        case UINodeType::Tabs: {
            out << ind << "let tabView = NSTabView()\n";
            {
                int tabIdx = 0;
                for (const auto& child : node.getChildren()) {
                    out << ind << "let tab" << tabIdx << " = NSTabViewItem(identifier: " << tabIdx << ")\n"
                        << ind << "tab" << tabIdx << ".label = \"تبويب " << tabIdx << "\"\n";
                    out << generateNode(*child, indentLevel);
                    out << ind << "tab" << tabIdx << ".view = view_\n"
                        << ind << "tabView.addTabViewItem(tab" << tabIdx << ")\n";
                    tabIdx++;
                }
            }
            out << ind << "let view_ = tabView\n";
            break;
        }

        case UINodeType::SideNav: {
            out << ind << "let splitView = NSSplitView()\n"
                << ind << "splitView.isVertical = true\n"
                << ind << "let sideList = NSTableView()\n"
                << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.documentView = sideList\n"
                << ind << "splitView.addArrangedSubview(scrollView)\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "splitView.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = splitView\n";
            break;
        }

        case UINodeType::Toolbar: {
            out << ind << "let toolbar = NSToolbar(identifier: \"toolbar\")\n"
                << ind << "// إضافة عناصر شريط الأدوات\n"
                << ind << "let container = NSView()\n"
                << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::SplitView: {
            out << ind << "let splitView = NSSplitView()\n"
                << ind << "splitView.isVertical = true\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "splitView.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = splitView\n";
            break;
        }

        case UINodeType::MegaMenu: {
            out << ind << "let menu = NSMenu(title: \"القائمة\")\n";
            for (const auto& child : node.getChildren()) {
                (void)child;
                out << ind << "menu.addItem(NSMenuItem(title: \"عنصر\", action: nil, keyEquivalent: \"\"))\n";
            }
            out << ind << "let container = NSView()\n"
                << ind << "let view_ = container\n";
            break;
        }

        // ── أنواع v1-v2 مفقودة ──

        case UINodeType::TextArea: {
            out << ind << "let scrollView = NSScrollView()\n"
                << ind << "let textView = NSTextView()\n"
                << ind << "textView.isEditable = true\n"
                << ind << "textView.minSize = NSSize(width: 0, height: 100)\n"
                << ind << "scrollView.documentView = textView\n"
                << ind << "let view_ = scrollView\n";
            break;
        }

        case UINodeType::LazyGrid: {
            out << ind << "let collectionView = NSCollectionView()\n"
                << ind << "let layout = NSCollectionViewFlowLayout()\n"
                << ind << "collectionView.collectionViewLayout = layout\n"
                << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.documentView = collectionView\n"
                << ind << "let view_ = scrollView\n";
            break;
        }

        case UINodeType::NavigationLink: {
            out << ind << "let button = NSButton(title: \"رابط\", target: nil, action: nil)\n"
                << ind << "button.bezelStyle = .inline\n"
                << ind << "let view_ = button\n";
            break;
        }

        case UINodeType::TabItem: {
            out << ind << "let tabItem = NSTabViewItem()\n"
                << ind << "tabItem.label = \"تبويب\"\n"
                << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "tabItem.view = container\n"
                << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::Section: {
            const auto* textProp = node.findProperty(props::TEXT_LATIN);
            std::string header = textProp ?
                (std::get_if<std::string>(&textProp->value) ?
                    *std::get_if<std::string>(&textProp->value) : "") : "";
            out << ind << "let section = NSStackView()\n"
                << ind << "section.orientation = .vertical\n";
            if (!header.empty()) {
                out << ind << "let sectionHeader = NSTextField(labelWithString: \"" << header << "\")\n"
                    << ind << "sectionHeader.font = .boldSystemFont(ofSize: 14)\n"
                    << ind << "section.addArrangedSubview(sectionHeader)\n";
            }
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "section.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = section\n";
            break;
        }

        case UINodeType::ForEach: {
            out << ind << "let container = NSStackView()\n"
                << ind << "container.orientation = .vertical\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::Sheet: {
            out << ind << "// Sheet — نافذة منبثقة\n"
                << ind << "let panel = NSPanel()\n"
                << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "panel.contentView = container\n"
                << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::Menu: {
            out << ind << "let menu = NSMenu(title: \"القائمة\")\n";
            for (const auto& child : node.getChildren()) {
                (void)child;
                out << ind << "menu.addItem(NSMenuItem(title: \"عنصر\", action: nil, keyEquivalent: \"\"))\n";
            }
            out << ind << "let container = NSView()\n"
                << ind << "container.menu = menu\n"
                << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::MapView: {
            out << ind << "let mapView = MKMapView()\n"
                << ind << "let view_ = mapView\n";
            break;
        }

        case UINodeType::WebView: {
            out << ind << "let webView = WKWebView()\n"
                << ind << "let view_ = webView\n";
            break;
        }

        case UINodeType::VideoPlayer: {
            out << ind << "let playerView = AVPlayerView()\n"
                << ind << "let view_ = playerView\n";
            break;
        }

        case UINodeType::CustomWidget: {
            out << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::Conditional: {
            out << ind << "// Conditional — عرض مشروط\n"
                << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::Wrap: {
            out << ind << "let collectionView = NSCollectionView()\n"
                << ind << "let layout = NSCollectionViewFlowLayout()\n"
                << ind << "collectionView.collectionViewLayout = layout\n"
                << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.documentView = collectionView\n"
                << ind << "let view_ = scrollView\n";
            break;
        }

        // ══════════════════════════════════════════════════════════════════════
        // (AR) عناصرُ التخطيطِ المحمولة (ث٨) — كانت تسقط إلى NSView العارية أدناه
        //      فتضيع دلالةُ التخطيطِ كلُّها. AppKit يعبّر عنها بقيودِ التخطيطِ
        //      التلقائيّ (Auto Layout)، فالفجوةُ كانت غيابَ حالةٍ لا غيابَ قدرة.
        // ══════════════════════════════════════════════════════════════════════

        case UINodeType::Center: {
            out << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n"
                    << ind << "view_.translatesAutoresizingMaskIntoConstraints = false\n"
                    << ind << "view_.centerXAnchor.constraint(equalTo: container.centerXAnchor).isActive = true\n"
                    << ind << "view_.centerYAnchor.constraint(equalTo: container.centerYAnchor).isActive = true\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::Padding: {
            const std::string pad = appkitNumber(node, props::PADDING, 8.0);
            out << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n"
                    << ind << "view_.translatesAutoresizingMaskIntoConstraints = false\n"
                    << ind << "view_.leadingAnchor.constraint(equalTo: container.leadingAnchor, constant: " << pad << ").isActive = true\n"
                    << ind << "view_.trailingAnchor.constraint(equalTo: container.trailingAnchor, constant: -" << pad << ").isActive = true\n"
                    << ind << "view_.topAnchor.constraint(equalTo: container.topAnchor, constant: " << pad << ").isActive = true\n"
                    << ind << "view_.bottomAnchor.constraint(equalTo: container.bottomAnchor, constant: -" << pad << ").isActive = true\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::SizedBox: {
            out << ind << "let container = NSView()\n"
                << ind << "container.translatesAutoresizingMaskIntoConstraints = false\n"
                << ind << "container.widthAnchor.constraint(equalToConstant: "
                << appkitNumber(node, props::WIDTH, 0.0) << ").isActive = true\n"
                << ind << "container.heightAnchor.constraint(equalToConstant: "
                << appkitNumber(node, props::HEIGHT, 0.0) << ").isActive = true\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::Expanded:
        case UINodeType::Flexible: {
            // (AR) «الوزن» في AppKit أولويّةُ ضغطٍ/شدٍّ لا معامِلٌ عدديّ: الموسَّعُ
            //      يقاوم الانكماشَ بأدنى أولويّةٍ فيلتهم الفائض.
            out << ind << "let container = NSView()\n"
                << ind << "container.setContentHuggingPriority(.defaultLow, for: .horizontal)\n"
                << ind << "container.setContentHuggingPriority(.defaultLow, for: .vertical)\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::Align: {
            out << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n"
                    << ind << "view_.translatesAutoresizingMaskIntoConstraints = false\n"
                    << ind << appkitAlignmentConstraint(node) << "\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::SafeArea: {
            out << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n"
                    << ind << "view_.translatesAutoresizingMaskIntoConstraints = false\n"
                    << ind << "view_.topAnchor.constraint(equalTo: container.safeAreaLayoutGuide.topAnchor).isActive = true\n"
                    << ind << "view_.bottomAnchor.constraint(equalTo: container.safeAreaLayoutGuide.bottomAnchor).isActive = true\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::GestureDetector:
        case UINodeType::InkWell: {
            out << ind << "let container = NSView()\n"
                << ind << "let clickRecognizer = NSClickGestureRecognizer()\n"
                << ind << "container.addGestureRecognizer(clickRecognizer)\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::ListView: {
            out << ind << "let tableView = NSTableView()\n"
                << ind << "let scrollView = NSScrollView()\n"
                << ind << "scrollView.documentView = tableView\n"
                << ind << "let view_ = scrollView\n";
            break;
        }

        case UINodeType::FractionallySizedBox: {
            out << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n"
                    << ind << "view_.translatesAutoresizingMaskIntoConstraints = false\n"
                    << ind << "view_.widthAnchor.constraint(equalTo: container.widthAnchor, multiplier: "
                    << appkitNumber(node, props::WIDTH_FACTOR, 1.0) << ").isActive = true\n"
                    << ind << "view_.heightAnchor.constraint(equalTo: container.heightAnchor, multiplier: "
                    << appkitNumber(node, props::HEIGHT_FACTOR, 1.0) << ").isActive = true\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::ConstrainedBox: {
            out << ind << "let container = NSView()\n"
                << ind << "container.translatesAutoresizingMaskIntoConstraints = false\n"
                << ind << "container.widthAnchor.constraint(greaterThanOrEqualToConstant: "
                << appkitNumber(node, props::MIN_WIDTH, 0.0) << ").isActive = true\n"
                << appkitBound("width", node, props::MAX_WIDTH, ind)
                << ind << "container.heightAnchor.constraint(greaterThanOrEqualToConstant: "
                << appkitNumber(node, props::MIN_HEIGHT, 0.0) << ").isActive = true\n"
                << appkitBound("height", node, props::MAX_HEIGHT, ind);
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::AspectRatio: {
            out << ind << "let container = NSView()\n"
                << ind << "container.translatesAutoresizingMaskIntoConstraints = false\n"
                << ind << "container.widthAnchor.constraint(equalTo: container.heightAnchor, multiplier: "
                << appkitNumber(node, props::RATIO, 1.0) << ").isActive = true\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }

        // ══════════════════════════════════════════════════════════════════════
        // (AR) قشرةُ سطحِ المكتب — هنا وحدَها لها نظائرُ أصليّةٌ تامّة: ماك نظامُ
        //      سطحِ مكتب. NSWindow وNSTitlebar وNSScroller وNSStatusBar…
        // ══════════════════════════════════════════════════════════════════════

        case UINodeType::Window: {
            out << ind << "let window = NSWindow(contentRect: NSRect(x: 0, y: 0, width: 800, height: 600),\n"
                << ind << "                     styleMask: [.titled, .closable, .resizable],\n"
                << ind << "                     backing: .buffered, defer: false)\n"
                << ind << "window.title = \"" << appkitLabel(node) << "\"\n"
                << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "window.contentView = container\n"
                << ind << "let view_ = container\n";
            break;
        }

        case UINodeType::TitleBar: {
            out << ind << "let titleLabel = NSTextField(labelWithString: \"" << appkitLabel(node) << "\")\n"
                << ind << "titleLabel.font = NSFont.titleBarFont(ofSize: NSFont.systemFontSize)\n"
                << ind << "titleLabel.alignment = .center\n"
                << ind << "let view_ = titleLabel\n";
            break;
        }

        case UINodeType::ScrollBar: {
            out << ind << "let scroller = NSScroller()\n"
                << ind << "scroller.scrollerStyle = .overlay\n"
                << ind << "let view_ = scroller\n";
            break;
        }

        case UINodeType::Taskbar:
        case UINodeType::StatusBar: {
            out << ind << "let bar = NSStackView()\n"
                << ind << "bar.orientation = .horizontal\n";
            {
                const std::string barText = appkitLabel(node);
                if (!barText.empty())
                    out << ind << "bar.addArrangedSubview(NSTextField(labelWithString: \"" << barText << "\"))\n";
            }
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "bar.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = bar\n";
            break;
        }

        case UINodeType::StartMenu: {
            out << ind << "let menu = NSMenu(title: \"" << appkitLabel(node) << "\")\n"
                << ind << "let menuButton = NSPopUpButton()\n"
                << ind << "menuButton.menu = menu\n"
                << ind << "let view_ = menuButton\n";
            break;
        }

        case UINodeType::SystemTray: {
            out << ind << "let tray = NSStackView()\n"
                << ind << "tray.orientation = .horizontal\n";
            {
                const std::string trayText = appkitLabel(node);
                if (!trayText.empty())
                    out << ind << "tray.addArrangedSubview(NSTextField(labelWithString: \"" << trayText << "\"))\n";
            }
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "tray.addArrangedSubview(view_)\n";
            }
            out << ind << "let view_ = tray\n";
            break;
        }

        case UINodeType::SpinBox: {
            out << ind << "let stepper = NSStepper()\n"
                << ind << "stepper.doubleValue = " << appkitNumber(node, props::VALUE, 0.0) << "\n"
                << ind << "let view_ = stepper\n";
            break;
        }

        case UINodeType::GroupBox: {
            out << ind << "let groupBox = NSBox()\n"
                << ind << "groupBox.title = \"" << appkitLabel(node) << "\"\n"
                << ind << "let groupContent = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "groupContent.addSubview(view_)\n";
            }
            out << ind << "groupBox.contentView = groupContent\n"
                << ind << "let view_ = groupBox\n";
            break;
        }

        case UINodeType::Spinner: {
            out << ind << "let spinner = NSProgressIndicator()\n"
                << ind << "spinner.style = .spinning\n"
                << ind << "spinner.startAnimation(nil)\n"
                << ind << "let view_ = spinner\n";
            break;
        }

        case UINodeType::Container:
        case UINodeType::Box:
        case UINodeType::Surface:
        case UINodeType::Stack:
        case UINodeType::Group:
        default: {
            out << ind << "let container = NSView()\n";
            for (const auto& child : node.getChildren()) {
                out << generateNode(*child, indentLevel);
                out << ind << "container.addSubview(view_)\n";
            }
            out << ind << "let view_ = container\n";
            break;
        }
    }
}

void AppKitCodegen::generateCanvasWidget(
    std::ostringstream& out,
    const IRNode& node,
    int indentLevel
) {
    std::string ind = indent(indentLevel);

    out << ind << "let canvasView = NSView()\n"
        << ind << "canvasView.wantsLayer = true\n"
        << ind << "// رسم مخصص لـ " << nodeTypeToArabicName(node.getType()) << "\n"
        << ind << "let view_ = canvasView\n";
}

// ─── أدوات مساعدة ────────────────────────────

std::string AppKitCodegen::generateModifiers(const IRNode& node) const {
    std::ostringstream mod;

    for (const auto& prop : node.getProperties()) {
        if (prop.key == props::PADDING) { // SoT «حشوة»
            if (auto* v = std::get_if<double>(&prop.value))
                mod << "view.edgeInsets = NSEdgeInsetsMake(" << *v << ", " << *v << ", " << *v << ", " << *v << ")\n";
            else if (auto* vi = std::get_if<int64_t>(&prop.value))
                mod << "view.edgeInsets = NSEdgeInsetsMake(" << *vi << ", " << *vi << ", " << *vi << ", " << *vi << ")\n";
        } else if (prop.key == "عرض") {
            if (auto* v = std::get_if<double>(&prop.value))
                mod << "view.widthAnchor.constraint(equalToConstant: " << *v << ").isActive = true\n";
            else if (auto* vi = std::get_if<int64_t>(&prop.value))
                mod << "view.widthAnchor.constraint(equalToConstant: " << *vi << ").isActive = true\n";
        } else if (prop.key == "ارتفاع") {
            if (auto* v = std::get_if<double>(&prop.value))
                mod << "view.heightAnchor.constraint(equalToConstant: " << *v << ").isActive = true\n";
            else if (auto* vi = std::get_if<int64_t>(&prop.value))
                mod << "view.heightAnchor.constraint(equalToConstant: " << *vi << ").isActive = true\n";
        } else if (prop.key == "لون_خلفية" || prop.key == "خلفية") {
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                auto hex = arabicColorToHex(*v);
                if (hex) {
                    std::string h = hex->substr(1);
                    mod << "view.wantsLayer = true; view.layer?.backgroundColor = NSColor(calibratedRed: "
                        << "CGFloat(0x" << h.substr(0,2) << ") / 255.0, green: "
                        << "CGFloat(0x" << h.substr(2,2) << ") / 255.0, blue: "
                        << "CGFloat(0x" << h.substr(4,2) << ") / 255.0, alpha: 1.0).cgColor\n";
                }
            }
        } else if (prop.key == "زوايا" || prop.key == "نصف_قطر") {
            if (auto* v = std::get_if<double>(&prop.value))
                mod << "view.wantsLayer = true; view.layer?.cornerRadius = " << *v
                    << "; view.layer?.masksToBounds = true\n";
            else if (auto* vi = std::get_if<int64_t>(&prop.value))
                mod << "view.wantsLayer = true; view.layer?.cornerRadius = " << *vi
                    << "; view.layer?.masksToBounds = true\n";
        } else if (prop.key == "ظل" || prop.key == "رفع") {
            if (auto* v = std::get_if<double>(&prop.value))
                mod << "view.wantsLayer = true\n"
                    << "view.shadow = NSShadow()\n"
                    << "view.shadow?.shadowBlurRadius = " << *v << "\n"
                    << "view.shadow?.shadowOffset = NSSize(width: 0, height: -" << (*v * 0.5) << ")\n"
                    << "view.shadow?.shadowColor = NSColor.black.withAlphaComponent(0.15)\n";
        } else if (prop.key == "حد_سماكة") {
            if (auto* v = std::get_if<double>(&prop.value))
                mod << "view.wantsLayer = true; view.layer?.borderWidth = " << *v << "\n";
        } else if (prop.key == "حد_لون") {
            if (auto* v = std::get_if<std::string>(&prop.value)) {
                auto hex = arabicColorToHex(*v);
                if (hex) {
                    std::string h = hex->substr(1);
                    mod << "view.wantsLayer = true; view.layer?.borderColor = NSColor(calibratedRed: "
                        << "CGFloat(0x" << h.substr(0,2) << ") / 255.0, green: "
                        << "CGFloat(0x" << h.substr(2,2) << ") / 255.0, blue: "
                        << "CGFloat(0x" << h.substr(4,2) << ") / 255.0, alpha: 1.0).cgColor\n";
                }
            }
        } else if (prop.key == "شفافية") {
            if (auto* v = std::get_if<double>(&prop.value))
                mod << "view.alphaValue = " << *v << "\n";
        } else if (prop.key == "وصف") {
            if (auto* v = std::get_if<std::string>(&prop.value))
                mod << "view.setAccessibilityLabel(\"" << *v << "\")\n";
        }
    }

    // RTL support
    mod << "view.userInterfaceLayoutDirection = .rightToLeft\n";

    return mod.str();
}

std::string AppKitCodegen::indent(int level) const {
    return std::string(level * options_.indentSize, ' ');
}

std::string AppKitCodegen::nodeTypeToAppKit(UINodeType type) const {
    switch (type) {
        case UINodeType::Column:        return "NSStackView(.vertical)";
        case UINodeType::Row:           return "NSStackView(.horizontal)";
        case UINodeType::Text:          return "NSTextField";
        case UINodeType::Button:        return "NSButton";
        case UINodeType::Image:         return "NSImageView";
        case UINodeType::TextField:     return "NSTextField";
        case UINodeType::Toggle:        return "NSSwitch";
        case UINodeType::Slider:        return "NSSlider";
        case UINodeType::Checkbox:      return "NSButton(.checkbox)";
        case UINodeType::Radio:         return "NSButton(.radio)";
        case UINodeType::Picker:        return "NSPopUpButton";
        case UINodeType::ScrollView:    return "NSScrollView";
        case UINodeType::List:          return "NSTableView";
        case UINodeType::TabView:       return "NSTabView";
        case UINodeType::Card:          return "NSBox";
        case UINodeType::Divider:       return "NSBox(.separator)";
        case UINodeType::Spacer:        return "NSView";
        case UINodeType::Grid:          return "NSGridView";
        case UINodeType::SearchBar:     return "NSSearchField";
        case UINodeType::ProgressBar:   return "NSProgressIndicator";
        case UINodeType::Icon:          return "NSImageView(systemSymbol)";
        case UINodeType::NavigationView:return "NSSplitView";
        case UINodeType::AppBar:        return "NSToolbar";
        case UINodeType::FAB:           return "NSButton(.circular)";
        case UINodeType::Badge:         return "NSTextField(.badge)";
        case UINodeType::Chip:          return "NSButton(.roundRect)";
        case UINodeType::Avatar:        return "NSImageView(.circular)";
        case UINodeType::SnackBar:      return "NSTextField(.snack)";
        case UINodeType::Drawer:        return "NSStackView(.sidebar)";
        case UINodeType::BottomSheet:   return "NSPanel";
        case UINodeType::Alert:         return "NSAlert";
        case UINodeType::Dialog:        return "NSPanel";        // ── 11. بيانات ──
        case UINodeType::Tooltip:        return "NSView";
        case UINodeType::DataTable:      return "NSTableView";
        case UINodeType::TreeView:       return "NSOutlineView";
        case UINodeType::Breadcrumb:     return "NSPathControl";
        case UINodeType::Pagination:     return "NSSegmentedControl";
        case UINodeType::Timeline:       return "NSStackView";
        // ── 12. وسائط ──
        case UINodeType::Carousel:       return "NSPageController";
        case UINodeType::RichText:       return "NSTextView";
        case UINodeType::Markdown:       return "NSTextView";
        case UINodeType::CodeBlock:      return "NSTextView";
        case UINodeType::AudioPlayer:    return "AVPlayerView";
        case UINodeType::ImageGallery:   return "NSCollectionView";
        // ── 13. حركة ──
        case UINodeType::Skeleton:       return "NSView";
        case UINodeType::Shimmer:        return "NSView";
        case UINodeType::AnimatedList:   return "NSTableView";
        case UINodeType::Expandable:     return "NSOutlineView";
        case UINodeType::Collapsible:    return "NSOutlineView";
        case UINodeType::Swipeable:      return "NSView";
        // ── 14. تحكم ──
        case UINodeType::Stepper:        return "NSStepper";
        case UINodeType::SegmentedControl: return "NSSegmentedControl";
        case UINodeType::RatingBar:      return "NSLevelIndicator";
        case UINodeType::TimePicker:     return "NSDatePicker";
        case UINodeType::Calendar:       return "NSDatePicker";
        case UINodeType::ColorWheel:     return "NSColorWell";
        // ── 15. تنقل ──
        case UINodeType::BottomNav:      return "NSSegmentedControl";
        case UINodeType::SideNav:        return "NSSplitView";
        case UINodeType::Toolbar:        return "NSToolbar";
        case UINodeType::SplitView:      return "NSSplitView";
        case UINodeType::Tabs:           return "NSTabView";
        case UINodeType::MegaMenu:       return "NSMenu";        default:                        return "NSView";
    }
}

std::string AppKitCodegen::colorToNSColor(const std::string& colorStr) const {
    if (colorStr == "أحمر" || colorStr == "red") return ".systemRed";
    if (colorStr == "أزرق" || colorStr == "blue") return ".systemBlue";
    if (colorStr == "أخضر" || colorStr == "green") return ".systemGreen";
    if (colorStr == "أبيض" || colorStr == "white") return ".white";
    if (colorStr == "أسود" || colorStr == "black") return ".black";
    if (colorStr == "رمادي" || colorStr == "gray") return ".systemGray";
    if (colorStr == "برتقالي" || colorStr == "orange") return ".systemOrange";
    if (colorStr == "بنفسجي" || colorStr == "purple") return ".systemPurple";
    if (colorStr == "وردي" || colorStr == "pink") return ".systemPink";
    if (colorStr == "أصفر" || colorStr == "yellow") return ".systemYellow";
    return ".controlAccentColor";
}

} // namespace macos
} // namespace ui
} // namespace sad
