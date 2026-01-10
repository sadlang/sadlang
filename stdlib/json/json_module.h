// بسم الله الرحمن الرحيم
/**
 * @file json_module.h
 * @brief JSON Module - JSON Parsing and Generation - وحدة JSON
 * 
 * This module provides comprehensive JSON functionality for Sad Language.
 * هذه الوحدة توفر وظائف JSON شاملة للغة الحزينة
 * 
 * Features / المميزات:
 * - JSON parsing / تحليل JSON
 * - JSON generation / توليد JSON
 * - JSON validation / التحقق من صحة JSON
 * - JSONPath queries / استعلامات JSONPath
 * - Pretty printing / طباعة منسقة
 * - Minification / تصغير
 * - Type safety / أمان الأنواع
 * - Unicode support / دعم Unicode
 * - Streaming support / دعم البث
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
#include <variant>
#include <optional>
#include <functional>
#include <stdexcept>

namespace sad {
namespace stdlib {
namespace json {

// ============================================================================
// Forward Declarations - التصريحات المسبقة
// ============================================================================

class JsonValue;
class JsonObject;
class JsonArray;

// ============================================================================
// Type Definitions - تعريفات الأنواع
// ============================================================================

/**
 * @brief JSON value types - أنواع قيم JSON
 */
enum class JsonType {
    NULL_TYPE,   // null / فارغ
    BOOLEAN,     // true/false / منطقي
    NUMBER,      // number / رقم
    STRING,      // string / نص
    ARRAY,       // array / مصفوفة
    OBJECT       // object / كائن
};

/**
 * @brief JSON parsing options - خيارات تحليل JSON
 */
struct JsonParseOptions {
    bool allow_comments = false;           // السماح بالتعليقات
    bool allow_trailing_commas = false;    // السماح بالفواصل الزائدة
    bool allow_nan_inf = false;            // السماح بـ NaN و Infinity
    bool strict_mode = true;               // الوضع الصارم
    size_t max_depth = 100;                // أقصى عمق للتداخل
};

/**
 * @brief JSON generation options - خيارات توليد JSON
 */
struct JsonStringifyOptions {
    bool pretty = false;                   // طباعة منسقة
    int indent = 2;                        // حجم المسافة البادئة
    bool sort_keys = false;                // ترتيب المفاتيح
    bool escape_unicode = false;           // تجنب Unicode
    bool compact = false;                  // تصغير (بدون مسافات)
};

// ============================================================================
// JSON Exception - استثناء JSON
// ============================================================================

/**
 * @brief JSON exception class - فئة استثناء JSON
 */
class JsonException : public std::runtime_error {
public:
    explicit JsonException(const std::string& message) 
        : std::runtime_error(message) {}
    
    JsonException(const std::string& message, size_t line, size_t column)
        : std::runtime_error(message), line_(line), column_(column) {}
    
    size_t line() const { return line_; }
    size_t column() const { return column_; }
    
private:
    size_t line_ = 0;
    size_t column_ = 0;
};

// ============================================================================
// JsonValue Class - فئة قيمة JSON
// ============================================================================

/**
 * @brief JSON value class - represents any JSON value
 * فئة قيمة JSON - تمثل أي قيمة JSON
 */
class JsonValue {
public:
    /**
     * @brief Default constructor (null value)
     * المنشئ الافتراضي (قيمة فارغة)
     */
    JsonValue();
    
    /**
     * @brief Construct from boolean
     * المنشئ من قيمة منطقية
     */
    explicit JsonValue(bool value);
    
    /**
     * @brief Construct from number
     * المنشئ من رقم
     */
    explicit JsonValue(double value);
    
    /**
     * @brief Construct from integer
     * المنشئ من عدد صحيح
     */
    explicit JsonValue(int value);
    
    /**
     * @brief Construct from string
     * المنشئ من نص
     */
    explicit JsonValue(const std::string& value);
    explicit JsonValue(const char* value);
    
    /**
     * @brief Construct from array
     * المنشئ من مصفوفة
     */
    explicit JsonValue(const JsonArray& value);
    
    /**
     * @brief Construct from object
     * المنشئ من كائن
     */
    explicit JsonValue(const JsonObject& value);
    
    /**
     * @brief Copy constructor
     * منشئ النسخ
     */
    JsonValue(const JsonValue& other);
    
    /**
     * @brief Move constructor
     * منشئ النقل
     */
    JsonValue(JsonValue&& other) noexcept;
    
    /**
     * @brief Copy assignment
     * إسناد النسخ
     */
    JsonValue& operator=(const JsonValue& other);
    
    /**
     * @brief Move assignment
     * إسناد النقل
     */
    JsonValue& operator=(JsonValue&& other) noexcept;
    
    /**
     * @brief Destructor
     * المدمر
     */
    ~JsonValue();
    
    // ========================================================================
    // Type Checking - فحص النوع
    // ========================================================================
    
    /**
     * @brief Get JSON type
     * الحصول على نوع JSON
     */
    JsonType type() const;
    
    /**
     * @brief Check if null
     * التحقق مما إذا كانت فارغة
     */
    bool is_null() const;
    
    /**
     * @brief Check if boolean
     * التحقق مما إذا كانت منطقية
     */
    bool is_boolean() const;
    
    /**
     * @brief Check if number
     * التحقق مما إذا كانت رقمًا
     */
    bool is_number() const;
    
    /**
     * @brief Check if string
     * التحقق مما إذا كانت نصًا
     */
    bool is_string() const;
    
    /**
     * @brief Check if array
     * التحقق مما إذا كانت مصفوفة
     */
    bool is_array() const;
    
    /**
     * @brief Check if object
     * التحقق مما إذا كانت كائنًا
     */
    bool is_object() const;
    
    // ========================================================================
    // Value Access - الوصول للقيم
    // ========================================================================
    
    /**
     * @brief Get as boolean (throws if not boolean)
     * الحصول كقيمة منطقية (يرمي استثناء إذا لم تكن منطقية)
     */
    bool as_boolean() const;
    
    /**
     * @brief Get as number (throws if not number)
     * الحصول كرقم (يرمي استثناء إذا لم تكن رقمًا)
     */
    double as_number() const;
    
    /**
     * @brief Get as integer (throws if not number)
     * الحصول كعدد صحيح (يرمي استثناء إذا لم تكن رقمًا)
     */
    int as_int() const;
    
    /**
     * @brief Get as string (throws if not string)
     * الحصول كنص (يرمي استثناء إذا لم تكن نصًا)
     */
    std::string as_string() const;
    
    /**
     * @brief Get as array (throws if not array)
     * الحصول كمصفوفة (يرمي استثناء إذا لم تكن مصفوفة)
     */
    JsonArray& as_array();
    const JsonArray& as_array() const;
    
    /**
     * @brief Get as object (throws if not object)
     * الحصول ككائن (يرمي استثناء إذا لم تكن كائنًا)
     */
    JsonObject& as_object();
    const JsonObject& as_object() const;
    
    // ========================================================================
    // Optional Access - الوصول الاختياري
    // ========================================================================
    
    /**
     * @brief Get as boolean with default
     * الحصول كقيمة منطقية مع قيمة افتراضية
     */
    bool get_boolean(bool default_value = false) const;
    
    /**
     * @brief Get as number with default
     * الحصول كرقم مع قيمة افتراضية
     */
    double get_number(double default_value = 0.0) const;
    
    /**
     * @brief Get as integer with default
     * الحصول كعدد صحيح مع قيمة افتراضية
     */
    int get_int(int default_value = 0) const;
    
    /**
     * @brief Get as string with default
     * الحصول كنص مع قيمة افتراضية
     */
    std::string get_string(const std::string& default_value = "") const;
    
    // ========================================================================
    // Array/Object Access - الوصول للمصفوفة/الكائن
    // ========================================================================
    
    /**
     * @brief Array element access by index
     * الوصول لعنصر المصفوفة بالفهرس
     */
    JsonValue& operator[](size_t index);
    const JsonValue& operator[](size_t index) const;
    
    /**
     * @brief Object property access by key
     * الوصول لخاصية الكائن بالمفتاح
     */
    JsonValue& operator[](const std::string& key);
    const JsonValue& operator[](const std::string& key) const;
    
    // ========================================================================
    // Serialization - التسلسل
    // ========================================================================
    
    /**
     * @brief Convert to JSON string
     * تحويل إلى نص JSON
     */
    std::string to_string(const JsonStringifyOptions& options = {}) const;
    
    /**
     * @brief Parse from JSON string
     * تحليل من نص JSON
     */
    static JsonValue parse(const std::string& json, 
                          const JsonParseOptions& options = {});
    
    // ========================================================================
    // Comparison - المقارنة
    // ========================================================================
    
    bool operator==(const JsonValue& other) const;
    bool operator!=(const JsonValue& other) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ============================================================================
// JsonArray Class - فئة مصفوفة JSON
// ============================================================================

/**
 * @brief JSON array class
 * فئة مصفوفة JSON
 */
class JsonArray {
public:
    JsonArray();
    ~JsonArray();
    
    JsonArray(const JsonArray& other);
    JsonArray(JsonArray&& other) noexcept;
    JsonArray& operator=(const JsonArray& other);
    JsonArray& operator=(JsonArray&& other) noexcept;
    
    /**
     * @brief Add element to array
     * إضافة عنصر للمصفوفة
     */
    void push(const JsonValue& value);
    void push(JsonValue&& value);
    
    /**
     * @brief Remove last element
     * حذف آخر عنصر
     */
    void pop();
    
    /**
     * @brief Get element at index
     * الحصول على عنصر بالفهرس
     */
    JsonValue& at(size_t index);
    const JsonValue& at(size_t index) const;
    
    /**
     * @brief Get array size
     * الحصول على حجم المصفوفة
     */
    size_t size() const;
    
    /**
     * @brief Check if empty
     * التحقق مما إذا كانت فارغة
     */
    bool empty() const;
    
    /**
     * @brief Clear all elements
     * مسح جميع العناصر
     */
    void clear();
    
    /**
     * @brief Array element access
     * الوصول لعنصر المصفوفة
     */
    JsonValue& operator[](size_t index);
    const JsonValue& operator[](size_t index) const;
    
    /**
     * @brief Iterator support
     * دعم المكرر
     */
    std::vector<JsonValue>::iterator begin();
    std::vector<JsonValue>::iterator end();
    std::vector<JsonValue>::const_iterator begin() const;
    std::vector<JsonValue>::const_iterator end() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<JsonValue> values_; // For compatibility
};

// ============================================================================
// JsonObject Class - فئة كائن JSON
// ============================================================================

/**
 * @brief JSON object class
 * فئة كائن JSON
 */
class JsonObject {
public:
    JsonObject();
    ~JsonObject();
    
    JsonObject(const JsonObject& other);
    JsonObject(JsonObject&& other) noexcept;
    JsonObject& operator=(const JsonObject& other);
    JsonObject& operator=(JsonObject&& other) noexcept;
    
    /**
     * @brief Set property
     * تعيين خاصية
     */
    void set(const std::string& key, const JsonValue& value);
    void set(const std::string& key, JsonValue&& value);
    
    /**
     * @brief Get property
     * الحصول على خاصية
     */
    JsonValue& get(const std::string& key);
    const JsonValue& get(const std::string& key) const;
    
    /**
     * @brief Check if property exists
     * التحقق من وجود خاصية
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove property
     * حذف خاصية
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all keys
     * الحصول على جميع المفاتيح
     */
    std::vector<std::string> keys() const;
    
    /**
     * @brief Get object size
     * الحصول على حجم الكائن
     */
    size_t size() const;
    
    /**
     * @brief Check if empty
     * التحقق مما إذا كان فارغًا
     */
    bool empty() const;
    
    /**
     * @brief Clear all properties
     * مسح جميع الخصائص
     */
    void clear();
    
    /**
     * @brief Property access
     * الوصول للخاصية
     */
    JsonValue& operator[](const std::string& key);
    const JsonValue& operator[](const std::string& key) const;
    
    /**
     * @brief Iterator support
     * دعم المكرر
     */
    std::map<std::string, JsonValue>::iterator begin();
    std::map<std::string, JsonValue>::iterator end();
    std::map<std::string, JsonValue>::const_iterator begin() const;
    std::map<std::string, JsonValue>::const_iterator end() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::map<std::string, JsonValue> properties_; // For compatibility
};

// ============================================================================
// JSON Parser - محلل JSON
// ============================================================================

/**
 * @brief JSON parser class
 * فئة محلل JSON
 */
class JsonParser {
public:
    /**
     * @brief Parse JSON from string
     * تحليل JSON من نص
     */
    static JsonValue parse(const std::string& json, 
                          const JsonParseOptions& options = {});
    
    /**
     * @brief Validate JSON syntax
     * التحقق من صحة صيغة JSON
     */
    static bool validate(const std::string& json);
    
    /**
     * @brief Get validation errors
     * الحصول على أخطاء التحقق
     */
    static std::string get_validation_errors(const std::string& json);
};

// ============================================================================
// JSON Generator - مولد JSON
// ============================================================================

/**
 * @brief JSON generator class
 * فئة مولد JSON
 */
class JsonGenerator {
public:
    /**
     * @brief Generate JSON string from value
     * توليد نص JSON من قيمة
     */
    static std::string stringify(const JsonValue& value,
                                 const JsonStringifyOptions& options = {});
    
    /**
     * @brief Pretty print JSON
     * طباعة JSON بشكل منسق
     */
    static std::string pretty(const JsonValue& value, int indent = 2);
    
    /**
     * @brief Minify JSON (remove whitespace)
     * تصغير JSON (إزالة المسافات)
     */
    static std::string minify(const JsonValue& value);
};

// ============================================================================
// JSONPath - مسار JSON
// ============================================================================

/**
 * @brief JSONPath query class
 * فئة استعلام مسار JSON
 */
class JsonPath {
public:
    /**
     * @brief Query JSON using JSONPath
     * استعلام JSON باستخدام مسار JSON
     */
    static std::vector<JsonValue> query(const JsonValue& root, 
                                       const std::string& path);
    
    /**
     * @brief Check if path exists
     * التحقق من وجود المسار
     */
    static bool exists(const JsonValue& root, const std::string& path);
    
    /**
     * @brief Get value at path
     * الحصول على قيمة في المسار
     */
    static JsonValue get(const JsonValue& root, const std::string& path);
    
    /**
     * @brief Set value at path
     * تعيين قيمة في المسار
     */
    static void set(JsonValue& root, const std::string& path, 
                    const JsonValue& value);
};

// ============================================================================
// Utility Functions - الدوال المساعدة
// ============================================================================

/**
 * @brief Escape string for JSON
 * تجنب النص لـ JSON
 */
std::string json_escape(const std::string& str);

/**
 * @brief Unescape JSON string
 * إلغاء تجنب نص JSON
 */
std::string json_unescape(const std::string& str);

/**
 * @brief Convert JSON type to string
 * تحويل نوع JSON إلى نص
 */
std::string json_type_to_string(JsonType type);

/**
 * @brief Merge two JSON objects
 * دمج كائني JSON
 */
JsonValue json_merge(const JsonValue& obj1, const JsonValue& obj2);

/**
 * @brief Deep clone JSON value
 * نسخ عميق لقيمة JSON
 */
JsonValue json_clone(const JsonValue& value);

/**
 * @brief Compare two JSON values deeply
 * مقارنة قيمتي JSON بعمق
 */
bool json_equals(const JsonValue& value1, const JsonValue& value2);

} // namespace json
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
