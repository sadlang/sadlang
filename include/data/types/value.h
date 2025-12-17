/**
 * @file value.h
 * @brief (AR) فئة القيمة في وقت التشغيل
 * @brief (EN) Runtime value class
 * 
 * Represents any value during program execution.
 * Supports all data types, type conversion, and operators.
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#pragma once

#include <variant>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <sstream>

namespace Sad {
namespace Data {

/**
 * @brief (AR) أنواع القيم المدعومة في وقت التشغيل
 * @brief (EN) Supported runtime value types
 */
enum class ValueType {
    VOID,       ///< (AR) فراغ (لاشيء) / (EN) Void (None/null)
    INTEGER,    ///< (AR) رقم صحيح / (EN) Integer number
    DOUBLE,     ///< (AR) رقم عشري / (EN) Double/floating-point number
    STRING,     ///< (AR) نص / (EN) String text
    BOOLEAN,    ///< (AR) منطقي (صحيح/خطأ) / (EN) Boolean (true/false)
    ARRAY,      ///< (AR) مصفوفة / (EN) Array (dynamic list)
    MAP         ///< (AR) قاموس / (EN) Map (dictionary/hash table)
};

/**
 * @brief (AR) فئة تمثل قيمة في وقت التشغيل
 * @brief (EN) Runtime value class
 * 
 * Core class for all runtime values in S language.
 */
class Value {
public:
    // Forward declarations for complex types
    using ArrayType = std::vector<Value>;
    using MapType = std::unordered_map<std::string, Value>;
    
    // Constructors
    Value();                                    // VOID
    explicit Value(int val);                    // INTEGER
    explicit Value(double val);                 // DOUBLE
    Value(const std::string& val);              // STRING (non-explicit for literals)
    Value(const char* val);                     // STRING from C-string
    explicit Value(bool val);                   // BOOLEAN
    explicit Value(const ArrayType& val);       // ARRAY
    explicit Value(const MapType& val);         // MAP
    
    // Clone method (deep copy)
    Value clone() const;
    
    // Type checking
    ValueType getType() const { return type_; }
    bool isVoid() const { return type_ == ValueType::VOID; }
    bool isInteger() const { return type_ == ValueType::INTEGER; }
    bool isDouble() const { return type_ == ValueType::DOUBLE; }
    bool isString() const { return type_ == ValueType::STRING; }
    bool isBoolean() const { return type_ == ValueType::BOOLEAN; }
    bool isArray() const { return type_ == ValueType::ARRAY; }
    bool isMap() const { return type_ == ValueType::MAP; }
    bool isNumeric() const { return isInteger() || isDouble(); }
    bool isContainer() const { return isArray() || isMap(); }
    
    // Type conversion
    int toInt() const;
    double toDouble() const;
    std::string toString() const;
    bool toBool() const;
    ArrayType toArray() const;
    MapType toMap() const;
    
    // Array/Map operations
    // (AR) عمليات المصفوفات والقواميس / (EN) Array/Map operations
    
    /**
     * @brief (AR) الحصول على عنصر من مصفوفة أو قاموس
     * @brief (EN) Get element from array or map
     */
    Value& operator[](size_t index);              // Array indexing
    const Value& operator[](size_t index) const;
    Value& operator[](const std::string& key);    // Map indexing
    const Value& operator[](const std::string& key) const;
    
    /**
     * @brief (AR) الحصول على طول المصفوفة أو القاموس
     * @brief (EN) Get length of array or map
     */
    size_t size() const;
    size_t length() const { return size(); }
    
    /**
     * @brief (AR) التحقق من وجود مفتاح في القاموس
     * @brief (EN) Check if key exists in map
     */
    bool hasKey(const std::string& key) const;
    
    /**
     * @brief (AR) إضافة عنصر لنهاية المصفوفة
     * @brief (EN) Append element to end of array
     */
    void push(const Value& val);
    void append(const Value& val) { push(val); }
    
    /**
     * @brief (AR) حذف آخر عنصر من المصفوفة
     * @brief (EN) Remove last element from array
     */
    Value pop();
    
    /**
     * @brief (AR) حذف عنصر من القاموس
     * @brief (EN) Remove element from map
     */
    bool remove(const std::string& key);
    
    /**
     * @brief (AR) مسح جميع العناصر
     * @brief (EN) Clear all elements
     */
    void clear();
    
    /**
     * @brief (AR) التحقق من الفراغ
     * @brief (EN) Check if empty
     */
    bool isEmpty() const;
    
    /**
     * @brief (AR) الحصول على جميع المفاتيح (للقاموس)
     * @brief (EN) Get all keys (for map)
     */
    std::vector<std::string> keys() const;
    
    /**
     * @brief (AR) الحصول على جميع القيم (للقاموس)
     * @brief (EN) Get all values (for map)
     */
    std::vector<Value> values() const;
    
    // Arithmetic operators
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    Value operator%(const Value& other) const;
    Value operator-() const;  // Unary minus
    
    // Comparison operators
    Value operator==(const Value& other) const;
    Value operator!=(const Value& other) const;
    Value operator<(const Value& other) const;
    Value operator>(const Value& other) const;
    Value operator<=(const Value& other) const;
    Value operator>=(const Value& other) const;
    
    // Logical operators
    Value operator&&(const Value& other) const;
    Value operator||(const Value& other) const;
    Value operator!() const;
    
    // Increment/Decrement
    Value& operator++();      // ++val
    Value operator++(int);    // val++
    Value& operator--();      // --val
    Value operator--(int);    // val--
    
    // Compound assignment
    Value& operator+=(const Value& other);
    Value& operator-=(const Value& other);
    Value& operator*=(const Value& other);
    Value& operator/=(const Value& other);
    
    // Debug
    std::string debugString() const;
    std::string getTypeName() const;
    
private:
    ValueType type_;
    std::variant<
        std::monostate,                              // VOID
        int,                                         // INTEGER
        double,                                      // DOUBLE
        std::string,                                 // STRING
        bool,                                        // BOOLEAN
        std::shared_ptr<ArrayType>,                  // ARRAY
        std::shared_ptr<MapType>                     // MAP
    > data_;
    
    void throwTypeMismatch(const std::string& operation, const Value& other) const;
    void throwInvalidType(const std::string& operation) const;
};

} // namespace Data
} // namespace Sad
