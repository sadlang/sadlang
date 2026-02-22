// ═══════════════════════════════════════════════════════════════════════════
// sad_android_ui.cpp - تنفيذ نظام واجهة المستخدم
// ═══════════════════════════════════════════════════════════════════════════

#include "sad_android_ui.h"

#ifdef SAD_ANDROID

#include <sstream>
#include <algorithm>

namespace sad {
namespace android {
namespace ui {

// ═══════════════════════════════════════════════════════════════════════════
// Color
// ═══════════════════════════════════════════════════════════════════════════

Color Color::fromHex(const std::string& hex) {
    Color c;
    std::string h = hex;
    if (h[0] == '#') h = h.substr(1);
    
    if (h.length() == 6) {
        c.r = std::stoi(h.substr(0, 2), nullptr, 16);
        c.g = std::stoi(h.substr(2, 2), nullptr, 16);
        c.b = std::stoi(h.substr(4, 2), nullptr, 16);
        c.a = 255;
    } else if (h.length() == 8) {
        c.a = std::stoi(h.substr(0, 2), nullptr, 16);
        c.r = std::stoi(h.substr(2, 2), nullptr, 16);
        c.g = std::stoi(h.substr(4, 2), nullptr, 16);
        c.b = std::stoi(h.substr(6, 2), nullptr, 16);
    }
    return c;
}

std::string Color::toHex() const {
    char buf[10];
    snprintf(buf, sizeof(buf), "#%02X%02X%02X%02X", a, r, g, b);
    return buf;
}

std::string Color::toJson() const {
    std::ostringstream ss;
    ss << "{\"r\":" << r << ",\"g\":" << g << ",\"b\":" << b << ",\"a\":" << a << "}";
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// Padding
// ═══════════════════════════════════════════════════════════════════════════

std::string Padding::toJson() const {
    std::ostringstream ss;
    ss << "{\"top\":" << top << ",\"right\":" << right 
       << ",\"bottom\":" << bottom << ",\"left\":" << left << "}";
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// Size
// ═══════════════════════════════════════════════════════════════════════════

std::string Size::toJson() const {
    std::ostringstream ss;
    switch (type) {
        case Type::FIXED:
            ss << "{\"type\":\"fixed\",\"value\":" << value << "}";
            break;
        case Type::WRAP:
            ss << "{\"type\":\"wrap\"}";
            break;
        case Type::FILL:
            ss << "{\"type\":\"fill\",\"weight\":" << value << "}";
            break;
        case Type::PERCENT:
            ss << "{\"type\":\"percent\",\"value\":" << value << "}";
            break;
    }
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// Widget base
// ═══════════════════════════════════════════════════════════════════════════

static std::string escapeJson(const std::string& s) {
    std::ostringstream ss;
    for (char c : s) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c;
        }
    }
    return ss.str();
}

static std::string alignmentToString(Alignment a) {
    switch (a) {
        case Alignment::START: return "start";
        case Alignment::CENTER: return "center";
        case Alignment::END: return "end";
        case Alignment::TOP: return "top";
        case Alignment::BOTTOM: return "bottom";
        case Alignment::TOP_START: return "topStart";
        case Alignment::TOP_CENTER: return "topCenter";
        case Alignment::TOP_END: return "topEnd";
        case Alignment::CENTER_START: return "centerStart";
        case Alignment::CENTER_CENTER: return "centerCenter";
        case Alignment::CENTER_END: return "centerEnd";
        case Alignment::BOTTOM_START: return "bottomStart";
        case Alignment::BOTTOM_CENTER: return "bottomCenter";
        case Alignment::BOTTOM_END: return "bottomEnd";
        default: return "start";
    }
}

std::string Widget::basePropsJson() const {
    std::ostringstream ss;
    if (!id.empty()) ss << "\"id\":\"" << escapeJson(id) << "\",";
    ss << "\"width\":" << width.toJson() << ",";
    ss << "\"height\":" << height.toJson() << ",";
    ss << "\"padding\":" << padding.toJson() << ",";
    ss << "\"margin\":" << margin.toJson() << ",";
    ss << "\"bgColor\":" << backgroundColor.toJson() << ",";
    ss << "\"cornerRadius\":" << cornerRadius << ",";
    ss << "\"visible\":" << (visible ? "true" : "false") << ",";
    ss << "\"enabled\":" << (enabled ? "true" : "false") << ",";
    ss << "\"elevation\":" << elevation;
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// Container widgets
// ═══════════════════════════════════════════════════════════════════════════

std::string ContainerWidget::childrenJson() const {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < children.size(); i++) {
        if (i > 0) ss << ",";
        ss << children[i]->toJson();
    }
    ss << "]";
    return ss.str();
}

std::string Column::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"column\"," << basePropsJson();
    ss << ",\"alignment\":\"" << alignmentToString(alignment) << "\"";
    ss << ",\"spacing\":" << spacing;
    ss << ",\"children\":" << childrenJson() << "}";
    return ss.str();
}

std::string Row::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"row\"," << basePropsJson();
    ss << ",\"alignment\":\"" << alignmentToString(alignment) << "\"";
    ss << ",\"spacing\":" << spacing;
    ss << ",\"children\":" << childrenJson() << "}";
    return ss.str();
}

std::string Box::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"box\"," << basePropsJson();
    ss << ",\"alignment\":\"" << alignmentToString(alignment) << "\"";
    ss << ",\"children\":" << childrenJson() << "}";
    return ss.str();
}

std::string Card::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"card\"," << basePropsJson();
    ss << ",\"elevation\":" << elevation;
    ss << ",\"children\":" << childrenJson() << "}";
    return ss.str();
}

std::string ScrollView::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"scroll\"," << basePropsJson();
    ss << ",\"horizontal\":" << (horizontal ? "true" : "false");
    ss << ",\"children\":" << childrenJson() << "}";
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// Display widgets
// ═══════════════════════════════════════════════════════════════════════════

static std::string fontWeightToString(FontWeight w) {
    switch (w) {
        case FontWeight::THIN: return "thin";
        case FontWeight::LIGHT: return "light";
        case FontWeight::NORMAL: return "normal";
        case FontWeight::MEDIUM: return "medium";
        case FontWeight::SEMI_BOLD: return "semiBold";
        case FontWeight::BOLD: return "bold";
        case FontWeight::EXTRA_BOLD: return "extraBold";
        case FontWeight::BLACK: return "black";
        default: return "normal";
    }
}

std::string Text::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"text\"," << basePropsJson();
    ss << ",\"content\":\"" << escapeJson(content) << "\"";
    ss << ",\"fontSize\":" << fontSize;
    ss << ",\"fontWeight\":\"" << fontWeightToString(fontWeight) << "\"";
    ss << ",\"color\":" << color.toJson();
    ss << ",\"textAlign\":\"" << alignmentToString(textAlign) << "\"";
    ss << ",\"maxLines\":" << maxLines;
    ss << ",\"selectable\":" << (selectable ? "true" : "false") << "}";
    return ss.str();
}

std::string Image::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"image\"," << basePropsJson();
    ss << ",\"source\":\"" << escapeJson(source) << "\"";
    ss << ",\"fit\":\"";
    switch (fit) {
        case Fit::FILL: ss << "fill"; break;
        case Fit::CONTAIN: ss << "contain"; break;
        case Fit::COVER: ss << "cover"; break;
        case Fit::NONE: ss << "none"; break;
    }
    ss << "\"}";
    return ss.str();
}

std::string Icon::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"icon\"," << basePropsJson();
    ss << ",\"name\":\"" << escapeJson(name) << "\"";
    ss << ",\"size\":" << size;
    ss << ",\"color\":" << color.toJson() << "}";
    return ss.str();
}

std::string Spacer::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"spacer\"," << basePropsJson() << "}";
    return ss.str();
}

std::string Divider::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"divider\"," << basePropsJson();
    ss << ",\"color\":" << color.toJson();
    ss << ",\"thickness\":" << thickness << "}";
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// Interactive widgets
// ═══════════════════════════════════════════════════════════════════════════

std::string Button::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"button\"," << basePropsJson();
    ss << ",\"label\":\"" << escapeJson(label) << "\"";
    if (!iconName.empty()) {
        ss << ",\"icon\":\"" << escapeJson(iconName) << "\"";
    }
    ss << ",\"color\":" << color.toJson();
    ss << ",\"textColor\":" << textColor.toJson();
    ss << ",\"style\":\"";
    switch (style) {
        case Style::FILLED: ss << "filled"; break;
        case Style::OUTLINED: ss << "outlined"; break;
        case Style::TEXT: ss << "text"; break;
    }
    ss << "\"";
    if (!onClickHandler.empty()) {
        ss << ",\"onClick\":\"" << escapeJson(onClickHandler) << "\"";
    }
    ss << "}";
    return ss.str();
}

std::string TextField::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"input\"," << basePropsJson();
    ss << ",\"value\":\"" << escapeJson(value) << "\"";
    ss << ",\"placeholder\":\"" << escapeJson(placeholder) << "\"";
    ss << ",\"label\":\"" << escapeJson(label) << "\"";
    ss << ",\"password\":" << (password ? "true" : "false");
    ss << ",\"multiline\":" << (multiline ? "true" : "false");
    ss << ",\"maxLength\":" << maxLength;
    ss << ",\"inputType\":\"";
    switch (inputType) {
        case Type::TEXT: ss << "text"; break;
        case Type::NUMBER: ss << "number"; break;
        case Type::EMAIL: ss << "email"; break;
        case Type::PHONE: ss << "phone"; break;
        case Type::URL: ss << "url"; break;
    }
    ss << "\"";
    if (!onChangeHandler.empty()) {
        ss << ",\"onChange\":\"" << escapeJson(onChangeHandler) << "\"";
    }
    ss << "}";
    return ss.str();
}

std::string Checkbox::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"checkbox\"," << basePropsJson();
    ss << ",\"checked\":" << (checked ? "true" : "false");
    ss << ",\"label\":\"" << escapeJson(label) << "\"";
    if (!onChangeHandler.empty()) {
        ss << ",\"onChange\":\"" << escapeJson(onChangeHandler) << "\"";
    }
    ss << "}";
    return ss.str();
}

std::string Switch::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"switch\"," << basePropsJson();
    ss << ",\"checked\":" << (checked ? "true" : "false");
    ss << ",\"label\":\"" << escapeJson(label) << "\"";
    if (!onChangeHandler.empty()) {
        ss << ",\"onChange\":\"" << escapeJson(onChangeHandler) << "\"";
    }
    ss << "}";
    return ss.str();
}

std::string Slider::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"slider\"," << basePropsJson();
    ss << ",\"value\":" << value;
    ss << ",\"min\":" << min;
    ss << ",\"max\":" << max;
    ss << ",\"step\":" << step;
    if (!onChangeHandler.empty()) {
        ss << ",\"onChange\":\"" << escapeJson(onChangeHandler) << "\"";
    }
    ss << "}";
    return ss.str();
}

std::string Dropdown::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"dropdown\"," << basePropsJson();
    ss << ",\"options\":[";
    for (size_t i = 0; i < options.size(); i++) {
        if (i > 0) ss << ",";
        ss << "\"" << escapeJson(options[i]) << "\"";
    }
    ss << "]";
    ss << ",\"selectedIndex\":" << selectedIndex;
    ss << ",\"placeholder\":\"" << escapeJson(placeholder) << "\"";
    if (!onChangeHandler.empty()) {
        ss << ",\"onChange\":\"" << escapeJson(onChangeHandler) << "\"";
    }
    ss << "}";
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// Progress widgets
// ═══════════════════════════════════════════════════════════════════════════

std::string ProgressBar::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"progress\"," << basePropsJson();
    ss << ",\"progress\":" << progress;
    ss << ",\"indeterminate\":" << (indeterminate ? "true" : "false");
    ss << ",\"color\":" << color.toJson() << "}";
    return ss.str();
}

std::string CircularProgress::toJson() const {
    std::ostringstream ss;
    ss << "{\"type\":\"circularProgress\"," << basePropsJson();
    ss << ",\"progress\":" << progress;
    ss << ",\"indeterminate\":" << (indeterminate ? "true" : "false");
    ss << ",\"strokeWidth\":" << strokeWidth;
    ss << ",\"color\":" << color.toJson() << "}";
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════
// UIManager
// ═══════════════════════════════════════════════════════════════════════════

void UIManager::setRoot(std::shared_ptr<Widget> root) {
    rootWidget_ = root;
    widgetMap_.clear();
    buildWidgetMap(root);
}

void UIManager::buildWidgetMap(std::shared_ptr<Widget> widget) {
    if (!widget) return;
    
    if (!widget->id.empty()) {
        widgetMap_[widget->id] = widget;
    }
    
    // إذا كان container، أضف الأبناء
    auto container = std::dynamic_pointer_cast<ContainerWidget>(widget);
    if (container) {
        for (auto& child : container->children) {
            buildWidgetMap(child);
        }
    }
}

std::shared_ptr<Widget> UIManager::findById(const std::string& id) {
    auto it = widgetMap_.find(id);
    return (it != widgetMap_.end()) ? it->second : nullptr;
}

std::string UIManager::toJson() const {
    if (rootWidget_) {
        return rootWidget_->toJson();
    }
    return "{}";
}

void UIManager::handleButtonClick(const std::string& buttonId) {
    auto it = clickHandlers_.find(buttonId);
    if (it != clickHandlers_.end()) {
        it->second();
    }
}

void UIManager::handleTextChange(const std::string& inputId, const std::string& text) {
    auto it = textHandlers_.find(inputId);
    if (it != textHandlers_.end()) {
        it->second(text);
    }
    
    // تحديث القيمة في العنصر
    auto widget = findById(inputId);
    if (auto tf = std::dynamic_pointer_cast<TextField>(widget)) {
        tf->value = text;
    }
}

void UIManager::registerClickHandler(const std::string& id, ClickHandler handler) {
    clickHandlers_[id] = handler;
}

void UIManager::registerTextHandler(const std::string& id, TextChangeHandler handler) {
    textHandlers_[id] = handler;
}

// ═══════════════════════════════════════════════════════════════════════════
// مساعدات البناء
// ═══════════════════════════════════════════════════════════════════════════

std::shared_ptr<Column> column() {
    return std::make_shared<Column>();
}

std::shared_ptr<Row> row() {
    return std::make_shared<Row>();
}

std::shared_ptr<Text> text(const std::string& content) {
    auto t = std::make_shared<Text>();
    t->content = content;
    return t;
}

std::shared_ptr<Button> button(const std::string& label) {
    auto b = std::make_shared<Button>();
    b->label = label;
    return b;
}

std::shared_ptr<TextField> textField(const std::string& placeholder) {
    auto tf = std::make_shared<TextField>();
    tf->placeholder = placeholder;
    return tf;
}

std::shared_ptr<Image> image(const std::string& source) {
    auto img = std::make_shared<Image>();
    img->source = source;
    return img;
}

std::shared_ptr<Spacer> spacer(int size) {
    auto s = std::make_shared<Spacer>();
    s->height = Size::fixed(size);
    return s;
}

} // namespace ui
} // namespace android
} // namespace sad

#endif // SAD_ANDROID
