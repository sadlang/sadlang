// بسم الله الرحمن الرحيم
/**
 * @file xml_module.cpp
 * @brief XML Module Implementation - تنفيذ وحدة XML
 * 
 * الحمد لله رب العالمين
 */

#include "xml/xml_module.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stack>

namespace sad {
namespace stdlib {
namespace xml {

// ============================================================================
// XmlAttribute Implementation - تنفيذ سمة XML
// ============================================================================

struct XmlAttribute::Impl {
    std::string name;
    std::string value;
    std::string namespace_uri;
};

XmlAttribute::XmlAttribute() : pImpl(std::make_unique<Impl>()) {}

XmlAttribute::XmlAttribute(const std::string& name, const std::string& value)
    : pImpl(std::make_unique<Impl>()) {
    pImpl->name = name;
    pImpl->value = value;
}

XmlAttribute::~XmlAttribute() = default;

XmlAttribute::XmlAttribute(const XmlAttribute& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

XmlAttribute::XmlAttribute(XmlAttribute&& other) noexcept = default;

XmlAttribute& XmlAttribute::operator=(const XmlAttribute& other) {
    if (this != &other) {
        pImpl = std::make_unique<Impl>(*other.pImpl);
    }
    return *this;
}

XmlAttribute& XmlAttribute::operator=(XmlAttribute&& other) noexcept = default;

std::string XmlAttribute::name() const {
    return pImpl->name;
}

void XmlAttribute::set_name(const std::string& name) {
    pImpl->name = name;
}

std::string XmlAttribute::value() const {
    return pImpl->value;
}

void XmlAttribute::set_value(const std::string& value) {
    pImpl->value = value;
}

std::string XmlAttribute::namespace_uri() const {
    return pImpl->namespace_uri;
}

void XmlAttribute::set_namespace_uri(const std::string& uri) {
    pImpl->namespace_uri = uri;
}

// ============================================================================
// XmlNode Implementation - تنفيذ عقدة XML
// ============================================================================

struct XmlNode::Impl {
    std::string value;
    XmlNode* parent = nullptr;
};

XmlNode::XmlNode() : pImpl(std::make_unique<Impl>()) {}
XmlNode::~XmlNode() = default;

XmlNode::XmlNode(const XmlNode& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

XmlNode::XmlNode(XmlNode&& other) noexcept = default;

XmlNode& XmlNode::operator=(const XmlNode& other) {
    if (this != &other) {
        pImpl = std::make_unique<Impl>(*other.pImpl);
    }
    return *this;
}

XmlNode& XmlNode::operator=(XmlNode&& other) noexcept = default;

std::string XmlNode::name() const {
    return "";
}

std::string XmlNode::value() const {
    return pImpl->value;
}

void XmlNode::set_value(const std::string& value) {
    pImpl->value = value;
}

XmlNode* XmlNode::parent() const {
    return pImpl->parent;
}

// ============================================================================
// XmlElement Implementation - تنفيذ عنصر XML
// ============================================================================

struct XmlElement::ElementImpl {
    std::string name;
    std::string text_content;
    std::string namespace_uri;
    std::string namespace_prefix;
    std::map<std::string, XmlAttribute> attributes;
    std::vector<XmlElement> children;
};

XmlElement::XmlElement() 
    : XmlNode(), pElementImpl(std::make_unique<ElementImpl>()) {}

XmlElement::XmlElement(const std::string& name)
    : XmlNode(), pElementImpl(std::make_unique<ElementImpl>()) {
    pElementImpl->name = name;
}

XmlElement::~XmlElement() = default;

XmlElement::XmlElement(const XmlElement& other)
    : XmlNode(other), pElementImpl(std::make_unique<ElementImpl>(*other.pElementImpl)) {}

XmlElement::XmlElement(XmlElement&& other) noexcept = default;

XmlElement& XmlElement::operator=(const XmlElement& other) {
    if (this != &other) {
        XmlNode::operator=(other);
        pElementImpl = std::make_unique<ElementImpl>(*other.pElementImpl);
    }
    return *this;
}

XmlElement& XmlElement::operator=(XmlElement&& other) noexcept = default;

XmlNodeType XmlElement::type() const {
    return XmlNodeType::ELEMENT;
}

std::string XmlElement::name() const {
    return pElementImpl->name;
}

void XmlElement::set_name(const std::string& name) {
    pElementImpl->name = name;
}

std::string XmlElement::to_string(const XmlStringifyOptions& options) const {
    return XmlGenerator::stringify(*this, options);
}

// Attributes
void XmlElement::set_attribute(const std::string& name, const std::string& value) {
    pElementImpl->attributes[name] = XmlAttribute(name, value);
}

std::string XmlElement::get_attribute(const std::string& name) const {
    auto it = pElementImpl->attributes.find(name);
    if (it != pElementImpl->attributes.end()) {
        return it->second.value();
    }
    return "";
}

bool XmlElement::has_attribute(const std::string& name) const {
    return pElementImpl->attributes.find(name) != pElementImpl->attributes.end();
}

void XmlElement::remove_attribute(const std::string& name) {
    pElementImpl->attributes.erase(name);
}

std::vector<XmlAttribute> XmlElement::attributes() const {
    std::vector<XmlAttribute> result;
    result.reserve(pElementImpl->attributes.size());
    for (const auto& pair : pElementImpl->attributes) {
        result.push_back(pair.second);
    }
    return result;
}

size_t XmlElement::attribute_count() const {
    return pElementImpl->attributes.size();
}

// Children
void XmlElement::append_child(const XmlElement& child) {
    pElementImpl->children.push_back(child);
}

void XmlElement::append_child(XmlElement&& child) {
    pElementImpl->children.push_back(std::move(child));
}

std::vector<XmlElement> XmlElement::children() const {
    return pElementImpl->children;
}

std::vector<XmlElement> XmlElement::children(const std::string& name) const {
    std::vector<XmlElement> result;
    for (const auto& child : pElementImpl->children) {
        if (child.name() == name) {
            result.push_back(child);
        }
    }
    return result;
}

std::optional<XmlElement> XmlElement::first_child() const {
    if (pElementImpl->children.empty()) {
        return std::nullopt;
    }
    return pElementImpl->children[0];
}

std::optional<XmlElement> XmlElement::first_child(const std::string& name) const {
    for (const auto& child : pElementImpl->children) {
        if (child.name() == name) {
            return child;
        }
    }
    return std::nullopt;
}

size_t XmlElement::child_count() const {
    return pElementImpl->children.size();
}

void XmlElement::clear_children() {
    pElementImpl->children.clear();
}

// Text content
std::string XmlElement::text() const {
    return pElementImpl->text_content;
}

void XmlElement::set_text(const std::string& text) {
    pElementImpl->text_content = text;
}

void XmlElement::append_text(const std::string& text) {
    pElementImpl->text_content += text;
}

// Namespace
std::string XmlElement::namespace_uri() const {
    return pElementImpl->namespace_uri;
}

void XmlElement::set_namespace_uri(const std::string& uri) {
    pElementImpl->namespace_uri = uri;
}

std::string XmlElement::namespace_prefix() const {
    return pElementImpl->namespace_prefix;
}

void XmlElement::set_namespace_prefix(const std::string& prefix) {
    pElementImpl->namespace_prefix = prefix;
}

// ============================================================================
// XmlDocument Implementation - تنفيذ مستند XML
// ============================================================================

struct XmlDocument::Impl {
    XmlElement root;
    std::string version = "1.0";
    std::string encoding = "UTF-8";
};

XmlDocument::XmlDocument() : pImpl(std::make_unique<Impl>()) {}
XmlDocument::~XmlDocument() = default;

XmlDocument::XmlDocument(const XmlDocument& other)
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

XmlDocument::XmlDocument(XmlDocument&& other) noexcept = default;

XmlDocument& XmlDocument::operator=(const XmlDocument& other) {
    if (this != &other) {
        pImpl = std::make_unique<Impl>(*other.pImpl);
    }
    return *this;
}

XmlDocument& XmlDocument::operator=(XmlDocument&& other) noexcept = default;

XmlElement& XmlDocument::root() {
    return pImpl->root;
}

const XmlElement& XmlDocument::root() const {
    return pImpl->root;
}

void XmlDocument::set_root(const XmlElement& root) {
    pImpl->root = root;
}

void XmlDocument::set_root(XmlElement&& root) {
    pImpl->root = std::move(root);
}

std::string XmlDocument::version() const {
    return pImpl->version;
}

void XmlDocument::set_version(const std::string& version) {
    pImpl->version = version;
}

std::string XmlDocument::encoding() const {
    return pImpl->encoding;
}

void XmlDocument::set_encoding(const std::string& encoding) {
    pImpl->encoding = encoding;
}

std::string XmlDocument::to_string(const XmlStringifyOptions& options) const {
    return XmlGenerator::stringify(*this, options);
}

XmlDocument XmlDocument::parse(const std::string& xml, const XmlParseOptions& options) {
    return XmlParser::parse(xml, options);
}

bool XmlDocument::validate() const {
    // Basic validation - check if root has a name
    return !pImpl->root.name().empty();
}

// ============================================================================
// XML Parser - محلل XML
// ============================================================================

namespace {
    class SimpleXmlParser {
    public:
        SimpleXmlParser(const std::string& xml, const XmlParseOptions& options)
            : xml_(xml), pos_(0), line_(1), column_(1), options_(options) {}
        
        XmlDocument parse() {
            XmlDocument doc;
            
            skip_whitespace();
            
            // Parse XML declaration if present
            if (peek_string("<?xml")) {
                parse_declaration(doc);
            }
            
            skip_whitespace();
            
            // Parse root element
            XmlElement root = parse_element();
            doc.set_root(root);
            
            return doc;
        }
        
        XmlElement parse_element() {
            if (current() != '<') {
                throw XmlException("Expected < / متوقع <", line_, column_);
            }
            advance(); // skip <
            
            // Read element name
            std::string name = read_name();
            XmlElement element(name);
            
            skip_whitespace();
            
            // Parse attributes
            while (current() != '>' && current() != '/' && current() != '\0') {
                std::string attr_name = read_name();
                skip_whitespace();
                
                if (current() != '=') {
                    throw XmlException("Expected = / متوقع =", line_, column_);
                }
                advance(); // skip =
                skip_whitespace();
                
                std::string attr_value = read_attribute_value();
                element.set_attribute(attr_name, attr_value);
                skip_whitespace();
            }
            
            // Check for self-closing tag
            if (current() == '/') {
                advance(); // skip /
                if (current() != '>') {
                    throw XmlException("Expected > / متوقع >", line_, column_);
                }
                advance(); // skip >
                return element;
            }
            
            if (current() != '>') {
                throw XmlException("Expected > / متوقع >", line_, column_);
            }
            advance(); // skip >
            
            // Parse content
            std::string text_content;
            while (current() != '\0') {
                skip_whitespace();
                
                if (current() == '<') {
                    if (peek() == '/') {
                        // Closing tag
                        break;
                    } else if (peek() == '!') {
                        // Comment or CDATA
                        if (peek_string("<!--")) {
                            skip_comment();
                        } else if (peek_string("<![CDATA[")) {
                            text_content += read_cdata();
                        }
                    } else {
                        // Child element
                        XmlElement child = parse_element();
                        element.append_child(std::move(child));
                    }
                } else {
                    // Text content
                    text_content += read_text();
                }
            }
            
            if (!text_content.empty()) {
                element.set_text(text_content);
            }
            
            // Parse closing tag
            if (current() != '<' || peek() != '/') {
                throw XmlException("Expected closing tag / متوقع وسم إغلاق", line_, column_);
            }
            advance(); // skip <
            advance(); // skip /
            
            std::string closing_name = read_name();
            if (closing_name != name) {
                throw XmlException("Mismatched closing tag / وسم إغلاق غير متطابق", line_, column_);
            }
            
            skip_whitespace();
            if (current() != '>') {
                throw XmlException("Expected > / متوقع >", line_, column_);
            }
            advance(); // skip >
            
            return element;
        }
        
    private:
        std::string xml_;
        size_t pos_;
        size_t line_;
        size_t column_;
        XmlParseOptions options_;
        
        char current() const {
            return pos_ < xml_.size() ? xml_[pos_] : '\0';
        }
        
        char peek(size_t offset = 1) const {
            size_t p = pos_ + offset;
            return p < xml_.size() ? xml_[p] : '\0';
        }
        
        bool peek_string(const std::string& str) const {
            if (pos_ + str.size() > xml_.size()) return false;
            return xml_.substr(pos_, str.size()) == str;
        }
        
        void advance() {
            if (pos_ < xml_.size()) {
                if (xml_[pos_] == '\n') {
                    line_++;
                    column_ = 1;
                } else {
                    column_++;
                }
                pos_++;
            }
        }
        
        void skip_whitespace() {
            while (pos_ < xml_.size() && std::isspace(current())) {
                advance();
            }
        }
        
        void skip_comment() {
            // Skip <!--
            for (int i = 0; i < 4; i++) advance();
            
            // Find -->
            while (current() != '\0' && !peek_string("-->")) {
                advance();
            }
            
            if (peek_string("-->")) {
                advance(); advance(); advance(); // skip -->
            }
        }
        
        std::string read_name() {
            std::string name;
            while (std::isalnum(current()) || current() == '_' || current() == ':' || current() == '-') {
                name += current();
                advance();
            }
            return name;
        }
        
        std::string read_attribute_value() {
            char quote = current();
            if (quote != '"' && quote != '\'') {
                throw XmlException("Expected quote / متوقع علامة اقتباس", line_, column_);
            }
            advance(); // skip opening quote
            
            std::string value;
            while (current() != quote && current() != '\0') {
                if (current() == '&') {
                    value += read_entity();
                } else {
                    value += current();
                    advance();
                }
            }
            
            if (current() != quote) {
                throw XmlException("Unterminated attribute value / قيمة سمة غير مكتملة", line_, column_);
            }
            advance(); // skip closing quote
            
            return value;
        }
        
        std::string read_text() {
            std::string text;
            while (current() != '<' && current() != '\0') {
                if (current() == '&') {
                    text += read_entity();
                } else {
                    text += current();
                    advance();
                }
            }
            return text;
        }
        
        std::string read_cdata() {
            // Skip <![CDATA[
            for (int i = 0; i < 9; i++) advance();
            
            std::string content;
            while (current() != '\0' && !peek_string("]]>")) {
                content += current();
                advance();
            }
            
            if (peek_string("]]>")) {
                advance(); advance(); advance(); // skip ]]>
            }
            
            return content;
        }
        
        std::string read_entity() {
            std::string entity;
            advance(); // skip &
            
            while (current() != ';' && current() != '\0') {
                entity += current();
                advance();
            }
            
            if (current() == ';') {
                advance(); // skip ;
            }
            
            // Convert common entities
            if (entity == "lt") return "<";
            if (entity == "gt") return ">";
            if (entity == "amp") return "&";
            if (entity == "quot") return "\"";
            if (entity == "apos") return "'";
            
            // Numeric entity
            if (!entity.empty() && entity[0] == '#') {
                try {
                    if (entity.size() > 1 && entity[1] == 'x') {
                        // Hex
                        int code = std::stoi(entity.substr(2), nullptr, 16);
                        return std::string(1, static_cast<char>(code));
                    } else {
                        // Decimal
                        int code = std::stoi(entity.substr(1));
                        return std::string(1, static_cast<char>(code));
                    }
                } catch (...) {
                    return "&" + entity + ";";
                }
            }
            
            return "&" + entity + ";";
        }
        
        void parse_declaration(XmlDocument& doc) {
            // Skip <?xml
            for (int i = 0; i < 5; i++) advance();
            
            skip_whitespace();
            
            // Parse version, encoding, etc.
            while (current() != '?' && current() != '\0') {
                std::string name = read_name();
                skip_whitespace();
                
                if (current() == '=') {
                    advance();
                    skip_whitespace();
                    std::string value = read_attribute_value();
                    
                    if (name == "version") {
                        doc.set_version(value);
                    } else if (name == "encoding") {
                        doc.set_encoding(value);
                    }
                }
                skip_whitespace();
            }
            
            // Skip ?>
            if (current() == '?') {
                advance();
                if (current() == '>') {
                    advance();
                }
            }
        }
    };
}

XmlDocument XmlParser::parse(const std::string& xml, const XmlParseOptions& options) {
    try {
        SimpleXmlParser parser(xml, options);
        return parser.parse();
    } catch (const std::exception& e) {
        throw XmlException(std::string("Parse error / خطأ في التحليل: ") + e.what());
    }
}

XmlElement XmlParser::parse_element(const std::string& xml, const XmlParseOptions& options) {
    try {
        SimpleXmlParser parser(xml, options);
        return parser.parse_element();
    } catch (const std::exception& e) {
        throw XmlException(std::string("Parse error / خطأ في التحليل: ") + e.what());
    }
}

bool XmlParser::validate(const std::string& xml) {
    try {
        parse(xml);
        return true;
    } catch (...) {
        return false;
    }
}

std::string XmlParser::get_validation_errors(const std::string& xml) {
    try {
        parse(xml);
        return "";
    } catch (const XmlException& e) {
        return e.what();
    } catch (const std::exception& e) {
        return e.what();
    }
}

// ============================================================================
// XML Generator - مولد XML
// ============================================================================

namespace {
    class XmlGen {
    public:
        XmlGen(const XmlStringifyOptions& options) : options_(options) {}
        
        std::string generate_document(const XmlDocument& doc) {
            std::ostringstream oss;
            
            if (!options_.omit_declaration) {
                oss << "<?xml version=\"" << doc.version() 
                    << "\" encoding=\"" << doc.encoding() << "\"?>";
                if (options_.pretty) {
                    oss << "\n";
                }
            }
            
            oss << generate_element(doc.root(), 0);
            
            return oss.str();
        }
        
        std::string generate_element(const XmlElement& element, int indent_level) {
            std::ostringstream oss;
            
            if (options_.pretty) {
                oss << std::string(indent_level * options_.indent, ' ');
            }
            
            oss << "<" << element.name();
            
            // Attributes
            for (const auto& attr : element.attributes()) {
                oss << " " << attr.name() << "=\"" << xml_escape(attr.value()) << "\"";
            }
            
            // Check if empty
            if (element.child_count() == 0 && element.text().empty()) {
                if (options_.self_close_empty) {
                    oss << "/>";
                } else {
                    oss << "></" << element.name() << ">";
                }
                if (options_.pretty) {
                    oss << "\n";
                }
                return oss.str();
            }
            
            oss << ">";
            
            // Text content
            if (!element.text().empty()) {
                oss << xml_escape(element.text());
            }
            
            // Children
            if (element.child_count() > 0) {
                if (options_.pretty) {
                    oss << "\n";
                }
                
                for (const auto& child : element.children()) {
                    oss << generate_element(child, indent_level + 1);
                }
                
                if (options_.pretty) {
                    oss << std::string(indent_level * options_.indent, ' ');
                }
            }
            
            oss << "</" << element.name() << ">";
            
            if (options_.pretty) {
                oss << "\n";
            }
            
            return oss.str();
        }
        
    private:
        XmlStringifyOptions options_;
    };
}

std::string XmlGenerator::stringify(const XmlDocument& doc, const XmlStringifyOptions& options) {
    XmlGen gen(options);
    return gen.generate_document(doc);
}

std::string XmlGenerator::stringify(const XmlElement& element, const XmlStringifyOptions& options) {
    XmlGen gen(options);
    return gen.generate_element(element, 0);
}

std::string XmlGenerator::pretty(const XmlDocument& doc, int indent) {
    XmlStringifyOptions options;
    options.pretty = true;
    options.indent = indent;
    return stringify(doc, options);
}

std::string XmlGenerator::minify(const XmlDocument& doc) {
    XmlStringifyOptions options;
    options.pretty = false;
    return stringify(doc, options);
}

// ============================================================================
// XPath - مسار XML
// ============================================================================

std::vector<XmlElement> XPath::query(const XmlDocument& doc, const std::string& xpath) {
    return query(doc.root(), xpath);
}

std::vector<XmlElement> XPath::query(const XmlElement& element, const std::string& xpath) {
    std::vector<XmlElement> results;
    
    if (xpath.empty() || xpath == "/") {
        results.push_back(element);
        return results;
    }
    
    // Simple XPath implementation
    std::vector<std::string> parts;
    std::string current;
    for (char c : xpath) {
        if (c == '/') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    // Start with root
    std::vector<XmlElement> current_elements = {element};
    
    for (const auto& part : parts) {
        std::vector<XmlElement> next_elements;
        
        for (const auto& elem : current_elements) {
            auto children = elem.children(part);
            next_elements.insert(next_elements.end(), children.begin(), children.end());
        }
        
        current_elements = next_elements;
    }
    
    return current_elements;
}

bool XPath::exists(const XmlDocument& doc, const std::string& xpath) {
    return !query(doc, xpath).empty();
}

std::optional<XmlElement> XPath::first(const XmlDocument& doc, const std::string& xpath) {
    auto results = query(doc, xpath);
    if (results.empty()) {
        return std::nullopt;
    }
    return results[0];
}

// ============================================================================
// Utility Functions - الدوال المساعدة
// ============================================================================

std::string xml_escape(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '&': oss << "&amp;"; break;
            case '"': oss << "&quot;"; break;
            case '\'': oss << "&apos;"; break;
            default: oss << c;
        }
    }
    return oss.str();
}

std::string xml_unescape(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '&') {
            size_t end = str.find(';', i);
            if (end != std::string::npos) {
                std::string entity = str.substr(i + 1, end - i - 1);
                if (entity == "lt") result += '<';
                else if (entity == "gt") result += '>';
                else if (entity == "amp") result += '&';
                else if (entity == "quot") result += '"';
                else if (entity == "apos") result += '\'';
                else result += str[i];
                i = end;
            } else {
                result += str[i];
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string xml_node_type_to_string(XmlNodeType type) {
    switch (type) {
        case XmlNodeType::ELEMENT: return "element";
        case XmlNodeType::TEXT: return "text";
        case XmlNodeType::CDATA: return "cdata";
        case XmlNodeType::COMMENT: return "comment";
        case XmlNodeType::PROCESSING_INSTRUCTION: return "processing_instruction";
        case XmlNodeType::DOCUMENT: return "document";
        default: return "unknown";
    }
}

std::string xml_to_json(const XmlDocument& doc) {
    // Simplified conversion
    return "{}"; // Placeholder
}

XmlDocument json_to_xml(const std::string& json) {
    // Simplified conversion
    return XmlDocument(); // Placeholder
}

XmlElement xml_clone(const XmlElement& element) {
    return element; // Copy constructor handles deep clone
}

bool xml_equals(const XmlElement& elem1, const XmlElement& elem2) {
    if (elem1.name() != elem2.name()) return false;
    if (elem1.text() != elem2.text()) return false;
    if (elem1.attribute_count() != elem2.attribute_count()) return false;
    if (elem1.child_count() != elem2.child_count()) return false;
    
    // Compare attributes
    for (const auto& attr : elem1.attributes()) {
        if (!elem2.has_attribute(attr.name())) return false;
        if (elem2.get_attribute(attr.name()) != attr.value()) return false;
    }
    
    // Compare children
    auto children1 = elem1.children();
    auto children2 = elem2.children();
    for (size_t i = 0; i < children1.size(); i++) {
        if (!xml_equals(children1[i], children2[i])) return false;
    }
    
    return true;
}

} // namespace xml
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
