// بسم الله الرحمن الرحيم
/**
 * @file xml_module.h
 * @brief XML Module - XML Parsing and Generation - وحدة XML
 * 
 * This module provides comprehensive XML functionality for Sad Language.
 * هذه الوحدة توفر وظائف XML شاملة للغة الحزينة
 * 
 * Features / المميزات:
 * - XML parsing / تحليل XML
 * - XML generation / توليد XML
 * - DOM support / دعم DOM
 * - XPath queries / استعلامات XPath
 * - Namespace support / دعم مساحات الأسماء
 * - Attribute handling / معالجة السمات
 * - CDATA sections / أقسام CDATA
 * - Pretty printing / طباعة منسقة
 * - Validation / التحقق من الصحة
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>

namespace sad {
namespace stdlib {
namespace xml {

// ============================================================================
// Forward Declarations - التصريحات المسبقة
// ============================================================================

class XmlNode;
class XmlElement;
class XmlDocument;
class XmlAttribute;

// ============================================================================
// Type Definitions - تعريفات الأنواع
// ============================================================================

/**
 * @brief XML node types - أنواع عقد XML
 */
enum class XmlNodeType {
    ELEMENT,      // عنصر
    TEXT,         // نص
    CDATA,        // CDATA
    COMMENT,      // تعليق
    PROCESSING_INSTRUCTION, // تعليمة معالجة
    DOCUMENT      // مستند
};

/**
 * @brief XML parsing options - خيارات تحليل XML
 */
struct XmlParseOptions {
    bool preserve_whitespace = false;    // الحفاظ على المسافات
    bool ignore_comments = false;        // تجاهل التعليقات
    bool validate = false;               // التحقق من الصحة
    bool resolve_entities = true;        // حل الكيانات
    bool expand_namespaces = true;       // توسيع مساحات الأسماء
};

/**
 * @brief XML generation options - خيارات توليد XML
 */
struct XmlStringifyOptions {
    bool pretty = false;                 // طباعة منسقة
    int indent = 2;                      // حجم المسافة البادئة
    bool omit_declaration = false;       // حذف التصريح
    std::string encoding = "UTF-8";      // الترميز
    bool self_close_empty = true;        // إغلاق ذاتي للعناصر الفارغة
};

// ============================================================================
// XML Exception - استثناء XML
// ============================================================================

/**
 * @brief XML exception class - فئة استثناء XML
 */
class XmlException : public std::runtime_error {
public:
    explicit XmlException(const std::string& message) 
        : std::runtime_error(message) {}
    
    XmlException(const std::string& message, size_t line, size_t column)
        : std::runtime_error(message), line_(line), column_(column) {}
    
    size_t line() const { return line_; }
    size_t column() const { return column_; }
    
private:
    size_t line_ = 0;
    size_t column_ = 0;
};

// ============================================================================
// XmlAttribute Class - فئة سمة XML
// ============================================================================

/**
 * @brief XML attribute class
 * فئة سمة XML
 */
class XmlAttribute {
public:
    XmlAttribute();
    XmlAttribute(const std::string& name, const std::string& value);
    ~XmlAttribute();
    
    XmlAttribute(const XmlAttribute& other);
    XmlAttribute(XmlAttribute&& other) noexcept;
    XmlAttribute& operator=(const XmlAttribute& other);
    XmlAttribute& operator=(XmlAttribute&& other) noexcept;
    
    /**
     * @brief Get attribute name
     * الحصول على اسم السمة
     */
    std::string name() const;
    
    /**
     * @brief Set attribute name
     * تعيين اسم السمة
     */
    void set_name(const std::string& name);
    
    /**
     * @brief Get attribute value
     * الحصول على قيمة السمة
     */
    std::string value() const;
    
    /**
     * @brief Set attribute value
     * تعيين قيمة السمة
     */
    void set_value(const std::string& value);
    
    /**
     * @brief Get namespace
     * الحصول على مساحة الاسم
     */
    std::string namespace_uri() const;
    
    /**
     * @brief Set namespace
     * تعيين مساحة الاسم
     */
    void set_namespace_uri(const std::string& uri);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ============================================================================
// XmlNode Class - فئة عقدة XML
// ============================================================================

/**
 * @brief XML node base class
 * فئة عقدة XML الأساسية
 */
class XmlNode {
public:
    XmlNode();
    virtual ~XmlNode();
    
    XmlNode(const XmlNode& other);
    XmlNode(XmlNode&& other) noexcept;
    XmlNode& operator=(const XmlNode& other);
    XmlNode& operator=(XmlNode&& other) noexcept;
    
    /**
     * @brief Get node type
     * الحصول على نوع العقدة
     */
    virtual XmlNodeType type() const = 0;
    
    /**
     * @brief Get node name
     * الحصول على اسم العقدة
     */
    virtual std::string name() const;
    
    /**
     * @brief Get node value
     * الحصول على قيمة العقدة
     */
    virtual std::string value() const;
    
    /**
     * @brief Set node value
     * تعيين قيمة العقدة
     */
    virtual void set_value(const std::string& value);
    
    /**
     * @brief Get parent node
     * الحصول على العقدة الأب
     */
    XmlNode* parent() const;
    
    /**
     * @brief Convert to string
     * تحويل إلى نص
     */
    virtual std::string to_string(const XmlStringifyOptions& options = {}) const = 0;

protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ============================================================================
// XmlElement Class - فئة عنصر XML
// ============================================================================

/**
 * @brief XML element class
 * فئة عنصر XML
 */
class XmlElement : public XmlNode {
public:
    XmlElement();
    explicit XmlElement(const std::string& name);
    ~XmlElement() override;
    
    XmlElement(const XmlElement& other);
    XmlElement(XmlElement&& other) noexcept;
    XmlElement& operator=(const XmlElement& other);
    XmlElement& operator=(XmlElement&& other) noexcept;
    
    // ========================================================================
    // Node Interface - واجهة العقدة
    // ========================================================================
    
    XmlNodeType type() const override;
    std::string name() const override;
    void set_name(const std::string& name);
    std::string to_string(const XmlStringifyOptions& options = {}) const override;
    
    // ========================================================================
    // Attributes - السمات
    // ========================================================================
    
    /**
     * @brief Set attribute
     * تعيين سمة
     */
    void set_attribute(const std::string& name, const std::string& value);
    
    /**
     * @brief Get attribute
     * الحصول على سمة
     */
    std::string get_attribute(const std::string& name) const;
    
    /**
     * @brief Check if attribute exists
     * التحقق من وجود سمة
     */
    bool has_attribute(const std::string& name) const;
    
    /**
     * @brief Remove attribute
     * حذف سمة
     */
    void remove_attribute(const std::string& name);
    
    /**
     * @brief Get all attributes
     * الحصول على جميع السمات
     */
    std::vector<XmlAttribute> attributes() const;
    
    /**
     * @brief Get attribute count
     * الحصول على عدد السمات
     */
    size_t attribute_count() const;
    
    // ========================================================================
    // Children - الأبناء
    // ========================================================================
    
    /**
     * @brief Add child element
     * إضافة عنصر ابن
     */
    void append_child(const XmlElement& child);
    void append_child(XmlElement&& child);
    
    /**
     * @brief Get child elements
     * الحصول على العناصر الأبناء
     */
    std::vector<XmlElement> children() const;
    
    /**
     * @brief Get children by name
     * الحصول على الأبناء بالاسم
     */
    std::vector<XmlElement> children(const std::string& name) const;
    
    /**
     * @brief Get first child
     * الحصول على أول ابن
     */
    std::optional<XmlElement> first_child() const;
    
    /**
     * @brief Get first child by name
     * الحصول على أول ابن بالاسم
     */
    std::optional<XmlElement> first_child(const std::string& name) const;
    
    /**
     * @brief Get child count
     * الحصول على عدد الأبناء
     */
    size_t child_count() const;
    
    /**
     * @brief Remove all children
     * حذف جميع الأبناء
     */
    void clear_children();
    
    // ========================================================================
    // Text Content - المحتوى النصي
    // ========================================================================
    
    /**
     * @brief Get text content
     * الحصول على المحتوى النصي
     */
    std::string text() const;
    
    /**
     * @brief Set text content
     * تعيين المحتوى النصي
     */
    void set_text(const std::string& text);
    
    /**
     * @brief Append text
     * إضافة نص
     */
    void append_text(const std::string& text);
    
    // ========================================================================
    // Namespace - مساحة الاسم
    // ========================================================================
    
    /**
     * @brief Get namespace URI
     * الحصول على URI لمساحة الاسم
     */
    std::string namespace_uri() const;
    
    /**
     * @brief Set namespace URI
     * تعيين URI لمساحة الاسم
     */
    void set_namespace_uri(const std::string& uri);
    
    /**
     * @brief Get namespace prefix
     * الحصول على بادئة مساحة الاسم
     */
    std::string namespace_prefix() const;
    
    /**
     * @brief Set namespace prefix
     * تعيين بادئة مساحة الاسم
     */
    void set_namespace_prefix(const std::string& prefix);

private:
    struct ElementImpl;
    std::unique_ptr<ElementImpl> pElementImpl;
};

// ============================================================================
// XmlDocument Class - فئة مستند XML
// ============================================================================

/**
 * @brief XML document class
 * فئة مستند XML
 */
class XmlDocument {
public:
    XmlDocument();
    ~XmlDocument();
    
    XmlDocument(const XmlDocument& other);
    XmlDocument(XmlDocument&& other) noexcept;
    XmlDocument& operator=(const XmlDocument& other);
    XmlDocument& operator=(XmlDocument&& other) noexcept;
    
    /**
     * @brief Get root element
     * الحصول على العنصر الجذر
     */
    XmlElement& root();
    const XmlElement& root() const;
    
    /**
     * @brief Set root element
     * تعيين العنصر الجذر
     */
    void set_root(const XmlElement& root);
    void set_root(XmlElement&& root);
    
    /**
     * @brief Get XML version
     * الحصول على إصدار XML
     */
    std::string version() const;
    
    /**
     * @brief Set XML version
     * تعيين إصدار XML
     */
    void set_version(const std::string& version);
    
    /**
     * @brief Get encoding
     * الحصول على الترميز
     */
    std::string encoding() const;
    
    /**
     * @brief Set encoding
     * تعيين الترميز
     */
    void set_encoding(const std::string& encoding);
    
    /**
     * @brief Convert to string
     * تحويل إلى نص
     */
    std::string to_string(const XmlStringifyOptions& options = {}) const;
    
    /**
     * @brief Parse from string
     * تحليل من نص
     */
    static XmlDocument parse(const std::string& xml, 
                            const XmlParseOptions& options = {});
    
    /**
     * @brief Validate document
     * التحقق من صحة المستند
     */
    bool validate() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ============================================================================
// XML Parser - محلل XML
// ============================================================================

/**
 * @brief XML parser class
 * فئة محلل XML
 */
class XmlParser {
public:
    /**
     * @brief Parse XML from string
     * تحليل XML من نص
     */
    static XmlDocument parse(const std::string& xml, 
                            const XmlParseOptions& options = {});
    
    /**
     * @brief Parse XML element
     * تحليل عنصر XML
     */
    static XmlElement parse_element(const std::string& xml,
                                    const XmlParseOptions& options = {});
    
    /**
     * @brief Validate XML syntax
     * التحقق من صحة صيغة XML
     */
    static bool validate(const std::string& xml);
    
    /**
     * @brief Get validation errors
     * الحصول على أخطاء التحقق
     */
    static std::string get_validation_errors(const std::string& xml);
};

// ============================================================================
// XML Generator - مولد XML
// ============================================================================

/**
 * @brief XML generator class
 * فئة مولد XML
 */
class XmlGenerator {
public:
    /**
     * @brief Generate XML string from document
     * توليد نص XML من مستند
     */
    static std::string stringify(const XmlDocument& doc,
                                 const XmlStringifyOptions& options = {});
    
    /**
     * @brief Generate XML string from element
     * توليد نص XML من عنصر
     */
    static std::string stringify(const XmlElement& element,
                                 const XmlStringifyOptions& options = {});
    
    /**
     * @brief Pretty print XML
     * طباعة XML بشكل منسق
     */
    static std::string pretty(const XmlDocument& doc, int indent = 2);
    
    /**
     * @brief Minify XML (remove whitespace)
     * تصغير XML (إزالة المسافات)
     */
    static std::string minify(const XmlDocument& doc);
};

// ============================================================================
// XPath - مسار XML
// ============================================================================

/**
 * @brief XPath query class
 * فئة استعلام مسار XML
 */
class XPath {
public:
    /**
     * @brief Query XML using XPath
     * استعلام XML باستخدام مسار XML
     */
    static std::vector<XmlElement> query(const XmlDocument& doc, 
                                        const std::string& xpath);
    
    /**
     * @brief Query from element
     * استعلام من عنصر
     */
    static std::vector<XmlElement> query(const XmlElement& element,
                                        const std::string& xpath);
    
    /**
     * @brief Check if path exists
     * التحقق من وجود المسار
     */
    static bool exists(const XmlDocument& doc, const std::string& xpath);
    
    /**
     * @brief Get first match
     * الحصول على أول تطابق
     */
    static std::optional<XmlElement> first(const XmlDocument& doc,
                                          const std::string& xpath);
};

// ============================================================================
// Utility Functions - الدوال المساعدة
// ============================================================================

/**
 * @brief Escape string for XML
 * تجنب النص لـ XML
 */
std::string xml_escape(const std::string& str);

/**
 * @brief Unescape XML string
 * إلغاء تجنب نص XML
 */
std::string xml_unescape(const std::string& str);

/**
 * @brief Convert XML node type to string
 * تحويل نوع عقدة XML إلى نص
 */
std::string xml_node_type_to_string(XmlNodeType type);

/**
 * @brief Convert XML to JSON
 * تحويل XML إلى JSON
 */
std::string xml_to_json(const XmlDocument& doc);

/**
 * @brief Convert JSON to XML
 * تحويل JSON إلى XML
 */
XmlDocument json_to_xml(const std::string& json);

/**
 * @brief Deep clone XML element
 * نسخ عميق لعنصر XML
 */
XmlElement xml_clone(const XmlElement& element);

/**
 * @brief Compare two XML elements deeply
 * مقارنة عنصري XML بعمق
 */
bool xml_equals(const XmlElement& elem1, const XmlElement& elem2);

} // namespace xml
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
