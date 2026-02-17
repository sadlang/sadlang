/**
 * @file value.cpp
 * @brief (AR) تنفيذ فئة القيمة في وقت التشغيل
 * @brief (EN) Runtime value class implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#include "value.h"
#include <cmath>
#include <iomanip>

namespace Sad {
namespace Data {

// ========================================
// Constructors / المُنشئات
// ========================================

Value::Value() : type_(ValueType::VOID), data_(std::monostate{}) {}

Value::Value(int val) : type_(ValueType::INTEGER), data_(val) {}

Value::Value(double val) : type_(ValueType::DOUBLE), data_(val) {}

Value::Value(const std::string& val) : type_(ValueType::STRING), data_(val) {}

Value::Value(const char* val) : type_(ValueType::STRING), data_(std::string(val)) {}

Value::Value(bool val) : type_(ValueType::BOOLEAN), data_(val) {}

Value::Value(const ArrayType& val) 
    : type_(ValueType::ARRAY), data_(std::make_shared<ArrayType>(val)) {}

Value::Value(const MapType& val) 
    : type_(ValueType::MAP), data_(std::make_shared<MapType>(val)) {}

// ========================================
// Clone / النسخ العميق
// ========================================

Value Value::clone() const {
    switch (type_) {
        case ValueType::ARRAY: {
            const auto& arr = *std::get<std::shared_ptr<ArrayType>>(data_);
            return Value(arr);  // Creates new shared_ptr with copy
        }
        case ValueType::MAP: {
            const auto& map = *std::get<std::shared_ptr<MapType>>(data_);
            return Value(map);  // Creates new shared_ptr with copy
        }
        default:
            return *this;  // Shallow copy for primitives
    }
}

// ========================================
// Type Conversion / تحويل النوع
// ========================================

int Value::toInt() const {
    switch (type_) {
        case ValueType::INTEGER:
            return std::get<int>(data_);
        
        case ValueType::DOUBLE:
            return static_cast<int>(std::get<double>(data_));
        
        case ValueType::BOOLEAN:
            return std::get<bool>(data_) ? 1 : 0;
        
        case ValueType::STRING: {
            try {
                return std::stoi(std::get<std::string>(data_));
            } catch (...) {
                throwInvalidType("toInt - cannot convert string to integer");
            }
        }
        
        case ValueType::VOID:
            throwInvalidType("toInt - cannot convert void to integer");
    }
    return 0;
}

double Value::toDouble() const {
    switch (type_) {
        case ValueType::INTEGER:
            return static_cast<double>(std::get<int>(data_));
        
        case ValueType::DOUBLE:
            return std::get<double>(data_);
        
        case ValueType::BOOLEAN:
            return std::get<bool>(data_) ? 1.0 : 0.0;
        
        case ValueType::STRING: {
            try {
                return std::stod(std::get<std::string>(data_));
            } catch (...) {
                throwInvalidType("toDouble - cannot convert string to double");
            }
        }
        
        case ValueType::VOID:
            throwInvalidType("toDouble - cannot convert void to double");
    }
    return 0.0;
}

std::string Value::toString() const {
    switch (type_) {
        case ValueType::INTEGER:
            return std::to_string(std::get<int>(data_));
        
        case ValueType::DOUBLE: {
            std::ostringstream oss;
            oss << std::get<double>(data_);
            return oss.str();
        }
        
        case ValueType::STRING:
            return std::get<std::string>(data_);
        
        case ValueType::BOOLEAN:
            return std::get<bool>(data_) ? "true" : "false";
        
        case ValueType::ARRAY: {
            std::ostringstream oss;
            oss << "[";
            const auto& arr = *std::get<std::shared_ptr<ArrayType>>(data_);
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arr[i].toString();
            }
            oss << "]";
            return oss.str();
        }
        
        case ValueType::MAP: {
            std::ostringstream oss;
            oss << "{";
            const auto& map = *std::get<std::shared_ptr<MapType>>(data_);
            size_t i = 0;
            for (const auto& pair : map) {
                if (i++ > 0) oss << ", ";
                oss << pair.first << ": " << pair.second.toString();
            }
            oss << "}";
            return oss.str();
        }
        
        case ValueType::VOID:
            return "void";
    }
    return "";
}

bool Value::toBool() const {
    switch (type_) {
        case ValueType::INTEGER:
            return std::get<int>(data_) != 0;
        
        case ValueType::DOUBLE:
            return std::get<double>(data_) != 0.0;
        
        case ValueType::STRING:
            return !std::get<std::string>(data_).empty();
        
        case ValueType::BOOLEAN:
            return std::get<bool>(data_);
        
        case ValueType::ARRAY:
            return !std::get<std::shared_ptr<ArrayType>>(data_)->empty();
        
        case ValueType::MAP:
            return !std::get<std::shared_ptr<MapType>>(data_)->empty();
        
        case ValueType::VOID:
            return false;
    }
    return false;
}

Value::ArrayType Value::toArray() const {
    if (type_ == ValueType::ARRAY) {
        return *std::get<std::shared_ptr<ArrayType>>(data_);
    }
    throwInvalidType("toArray - value is not an array");
    return ArrayType();
}

Value::MapType Value::toMap() const {
    if (type_ == ValueType::MAP) {
        return *std::get<std::shared_ptr<MapType>>(data_);
    }
    throwInvalidType("toMap - value is not a map");
    return MapType();
}

// ========================================
// Arithmetic Operators / العوامل الحسابية
// ========================================

Value Value::operator+(const Value& other) const {
    // ═══════════════════════════════════════════════════════════════════
    // (AR) دمج المصفوفات: [1,2] + [3,4] → [1,2,3,4]
    //      إضافة عنصر: [1,2] + 3 → [1,2,3]
    // (EN) Array concatenation: [1,2] + [3,4] → [1,2,3,4]
    //      Element append: [1,2] + 3 → [1,2,3]
    // ═══════════════════════════════════════════════════════════════════
    if (type_ == ValueType::ARRAY) {
        ArrayType result = toArray();
        if (other.type_ == ValueType::ARRAY) {
            // (AR) دمج مصفوفتين
            ArrayType otherArr = other.toArray();
            result.insert(result.end(), otherArr.begin(), otherArr.end());
        } else {
            // (AR) إضافة عنصر واحد للمصفوفة
            result.push_back(other);
        }
        return Value(result);
    }
    if (other.type_ == ValueType::ARRAY) {
        // (AR) إضافة عنصر في بداية المصفوفة: 0 + [1,2] → [0,1,2]
        ArrayType result;
        result.push_back(*this);
        ArrayType otherArr = other.toArray();
        result.insert(result.end(), otherArr.begin(), otherArr.end());
        return Value(result);
    }
    
    // (AR) جمع النصوص / (EN) String concatenation
    if (type_ == ValueType::STRING || other.type_ == ValueType::STRING) {
        return Value(toString() + other.toString());
    }
    
    // (AR) جمع الأعداد / (EN) Numeric addition
    if (isNumeric() && other.isNumeric()) {
        if (type_ == ValueType::DOUBLE || other.type_ == ValueType::DOUBLE) {
            return Value(toDouble() + other.toDouble());
        }
        return Value(toInt() + other.toInt());
    }
    
    throwTypeMismatch("addition (+)", other);
    return Value();
}

Value Value::operator-(const Value& other) const {
    if (isNumeric() && other.isNumeric()) {
        if (type_ == ValueType::DOUBLE || other.type_ == ValueType::DOUBLE) {
            return Value(toDouble() - other.toDouble());
        }
        return Value(toInt() - other.toInt());
    }
    
    throwTypeMismatch("subtraction (-)", other);
    return Value();
}

Value Value::operator*(const Value& other) const {
    // ═══════════════════════════════════════════════════════════════════
    // (AR) تكرار المصفوفات: [1,2] * 3 → [1,2,1,2,1,2]
    // (EN) Array repetition: [1,2] * 3 → [1,2,1,2,1,2]
    // ═══════════════════════════════════════════════════════════════════
    if (type_ == ValueType::ARRAY && other.isNumeric()) {
        ArrayType arr = toArray();
        int count = other.toInt();
        if (count <= 0) return Value(ArrayType{});
        ArrayType result;
        result.reserve(arr.size() * count);
        for (int i = 0; i < count; ++i)
            result.insert(result.end(), arr.begin(), arr.end());
        return Value(result);
    }
    if (other.type_ == ValueType::ARRAY && isNumeric()) {
        // (AR) 3 * [1,2] → [1,2,1,2,1,2] (ضرب من اليسار)
        ArrayType arr = other.toArray();
        int count = toInt();
        if (count <= 0) return Value(ArrayType{});
        ArrayType result;
        result.reserve(arr.size() * count);
        for (int i = 0; i < count; ++i)
            result.insert(result.end(), arr.begin(), arr.end());
        return Value(result);
    }
    // (AR) تكرار النصوص: "ها" * 3 → "هاهاها"
    if (type_ == ValueType::STRING && other.isNumeric()) {
        std::string s = toString();
        int count = other.toInt();
        std::string result;
        for (int i = 0; i < count; ++i) result += s;
        return Value(result);
    }
    if (other.type_ == ValueType::STRING && isNumeric()) {
        std::string s = other.toString();
        int count = toInt();
        std::string result;
        for (int i = 0; i < count; ++i) result += s;
        return Value(result);
    }
    
    if (isNumeric() && other.isNumeric()) {
        if (type_ == ValueType::DOUBLE || other.type_ == ValueType::DOUBLE) {
            return Value(toDouble() * other.toDouble());
        }
        return Value(toInt() * other.toInt());
    }
    
    throwTypeMismatch("multiplication (*)", other);
    return Value();
}

Value Value::operator/(const Value& other) const {
    if (isNumeric() && other.isNumeric()) {
        // (AR) التحقق من القسمة على صفر / (EN) Check for division by zero
        if ((other.isInteger() && other.toInt() == 0) ||
            (other.isDouble() && other.toDouble() == 0.0)) {
            throw std::runtime_error(
                "(AR) خطأ: القسمة على صفر. (EN) Error: Division by zero."
            );
        }
        
        if (type_ == ValueType::DOUBLE || other.type_ == ValueType::DOUBLE) {
            return Value(toDouble() / other.toDouble());
        }
        return Value(toInt() / other.toInt());
    }
    
    throwTypeMismatch("division (/)", other);
    return Value();
}

Value Value::operator%(const Value& other) const {
    // (AR) باقي القسمة يعمل فقط مع الأعداد الصحيحة
    // (EN) Modulus only works with integers
    if (isInteger() && other.isInteger()) {
        int otherInt = other.toInt();
        if (otherInt == 0) {
            throw std::runtime_error(
                "(AR) خطأ: باقي القسمة على صفر. (EN) Error: Modulus by zero."
            );
        }
        return Value(toInt() % otherInt);
    }
    
    throwTypeMismatch("modulus (%)", other);
    return Value();
}

Value Value::operator-() const {
    // (AR) العكس الحسابي / (EN) Unary minus
    if (isInteger()) {
        return Value(-toInt());
    }
    if (isDouble()) {
        return Value(-toDouble());
    }
    
    throwInvalidType("unary minus (-)");
    return Value();
}

// ========================================
// Comparison Operators / عوامل المقارنة
// ========================================

Value Value::operator==(const Value& other) const {
    // (AR) مقارنة الأنواع المختلفة / (EN) Compare different types
    if (type_ != other.type_) {
        // (AR) مقارنة رقمية بين int و double / (EN) Numeric comparison
        if (isNumeric() && other.isNumeric()) {
            return Value(toDouble() == other.toDouble());
        }
        return Value(false);
    }
    
    switch (type_) {
        case ValueType::INTEGER:
            return Value(std::get<int>(data_) == std::get<int>(other.data_));
        
        case ValueType::DOUBLE:
            return Value(std::abs(std::get<double>(data_) - std::get<double>(other.data_)) < 1e-10);
        
        case ValueType::STRING:
            return Value(std::get<std::string>(data_) == std::get<std::string>(other.data_));
        
        case ValueType::BOOLEAN:
            return Value(std::get<bool>(data_) == std::get<bool>(other.data_));
        
        case ValueType::VOID:
            return Value(true);
    }
    
    return Value(false);
}

Value Value::operator!=(const Value& other) const {
    return Value(!(*this == other).toBool());
}

Value Value::operator<(const Value& other) const {
    if (isNumeric() && other.isNumeric()) {
        return Value(toDouble() < other.toDouble());
    }
    
    if (isString() && other.isString()) {
        return Value(std::get<std::string>(data_) < std::get<std::string>(other.data_));
    }
    
    throwTypeMismatch("less than (<)", other);
    return Value(false);
}

Value Value::operator>(const Value& other) const {
    if (isNumeric() && other.isNumeric()) {
        return Value(toDouble() > other.toDouble());
    }
    
    if (isString() && other.isString()) {
        return Value(std::get<std::string>(data_) > std::get<std::string>(other.data_));
    }
    
    throwTypeMismatch("greater than (>)", other);
    return Value(false);
}

Value Value::operator<=(const Value& other) const {
    return Value((*this < other).toBool() || (*this == other).toBool());
}

Value Value::operator>=(const Value& other) const {
    return Value((*this > other).toBool() || (*this == other).toBool());
}

// ========================================
// Logical Operators / العوامل المنطقية
// ========================================

Value Value::operator&&(const Value& other) const {
    return Value(toBool() && other.toBool());
}

Value Value::operator||(const Value& other) const {
    return Value(toBool() || other.toBool());
}

Value Value::operator!() const {
    return Value(!toBool());
}

// ========================================
// Increment/Decrement / الزيادة/النقصان
// ========================================

Value& Value::operator++() {
    // (AR) زيادة قبلية / (EN) Pre-increment
    if (isInteger()) {
        data_ = std::get<int>(data_) + 1;
    } else if (isDouble()) {
        data_ = std::get<double>(data_) + 1.0;
    } else {
        throwInvalidType("pre-increment (++)");
    }
    return *this;
}

Value Value::operator++(int) {
    // (AR) زيادة بعدية / (EN) Post-increment
    Value temp = *this;
    ++(*this);
    return temp;
}

Value& Value::operator--() {
    // (AR) نقصان قبلي / (EN) Pre-decrement
    if (isInteger()) {
        data_ = std::get<int>(data_) - 1;
    } else if (isDouble()) {
        data_ = std::get<double>(data_) - 1.0;
    } else {
        throwInvalidType("pre-decrement (--)");
    }
    return *this;
}

Value Value::operator--(int) {
    // (AR) نقصان بعدي / (EN) Post-decrement
    Value temp = *this;
    --(*this);
    return temp;
}

// ========================================
// Compound Assignment / التعيين المركب
// ========================================

Value& Value::operator+=(const Value& other) {
    *this = *this + other;
    return *this;
}

Value& Value::operator-=(const Value& other) {
    *this = *this - other;
    return *this;
}

Value& Value::operator*=(const Value& other) {
    *this = *this * other;
    return *this;
}

Value& Value::operator/=(const Value& other) {
    *this = *this / other;
    return *this;
}

// ========================================
// Debug Methods / دوال التصحيح
// ========================================

std::string Value::debugString() const {
    std::ostringstream oss;
    oss << "Type: " << getTypeName() << ", Value: " << toString();
    return oss.str();
}

std::string Value::getTypeName() const {
    switch (type_) {
        case ValueType::VOID:    return "VOID";
        case ValueType::INTEGER: return "INTEGER";
        case ValueType::DOUBLE:  return "DOUBLE";
        case ValueType::STRING:  return "STRING";
        case ValueType::BOOLEAN: return "BOOLEAN";
        case ValueType::ARRAY:   return "ARRAY";
        case ValueType::MAP:     return "MAP";
    }
    return "UNKNOWN";
}

// ========================================
// Private Helper Methods / دوال مساعدة خاصة
// ========================================

// ========================================
// Array/Map Operations / عمليات المصفوفات والقواميس
// ========================================

Value& Value::operator[](size_t index) {
    // (AR) الوصول للعنصر حسب الفهرس / (EN) Access element by index
    if (type_ != ValueType::ARRAY) {
        throwInvalidType("array indexing [size_t]");
    }
    
    auto& arr = *std::get<std::shared_ptr<ArrayType>>(data_);
    if (index >= arr.size()) {
        std::ostringstream oss;
        oss << "(AR) فهرس خارج النطاق: " << index << " >= " << arr.size() << ".\n"
            << "(EN) Index out of range: " << index << " >= " << arr.size() << ".";
        throw std::out_of_range(oss.str());
    }
    
    return arr[index];
}

const Value& Value::operator[](size_t index) const {
    if (type_ != ValueType::ARRAY) {
        throwInvalidType("array indexing [size_t] const");
    }
    
    const auto& arr = *std::get<std::shared_ptr<ArrayType>>(data_);
    if (index >= arr.size()) {
        std::ostringstream oss;
        oss << "(AR) فهرس خارج النطاق: " << index << " >= " << arr.size() << ".\n"
            << "(EN) Index out of range: " << index << " >= " << arr.size() << ".";
        throw std::out_of_range(oss.str());
    }
    
    return arr[index];
}

Value& Value::operator[](const std::string& key) {
    // (AR) الوصول للعنصر حسب المفتاح / (EN) Access element by key
    if (type_ != ValueType::MAP) {
        throwInvalidType("map indexing [string]");
    }
    
    auto& map = *std::get<std::shared_ptr<MapType>>(data_);
    return map[key];  // Creates if doesn't exist
}

const Value& Value::operator[](const std::string& key) const {
    if (type_ != ValueType::MAP) {
        throwInvalidType("map indexing [string] const");
    }
    
    const auto& map = *std::get<std::shared_ptr<MapType>>(data_);
    auto it = map.find(key);
    if (it == map.end()) {
        std::ostringstream oss;
        oss << "(AR) المفتاح غير موجود: '" << key << "'.\n"
            << "(EN) Key not found: '" << key << "'.";
        throw std::out_of_range(oss.str());
    }
    
    return it->second;
}

size_t Value::size() const {
    // (AR) حجم المصفوفة أو القاموس / (EN) Size of array or map
    switch (type_) {
        case ValueType::ARRAY:
            return std::get<std::shared_ptr<ArrayType>>(data_)->size();
        
        case ValueType::MAP:
            return std::get<std::shared_ptr<MapType>>(data_)->size();
        
        case ValueType::STRING:
            return std::get<std::string>(data_).size();
        
        default:
            throwInvalidType("size()");
            return 0;
    }
}

bool Value::hasKey(const std::string& key) const {
    // (AR) التحقق من وجود مفتاح / (EN) Check key existence
    if (type_ != ValueType::MAP) {
        throwInvalidType("hasKey()");
    }
    
    const auto& map = *std::get<std::shared_ptr<MapType>>(data_);
    return map.find(key) != map.end();
}

void Value::push(const Value& val) {
    // (AR) إضافة عنصر لنهاية المصفوفة / (EN) Append to array
    if (type_ != ValueType::ARRAY) {
        throwInvalidType("push()");
    }
    
    auto& arr = *std::get<std::shared_ptr<ArrayType>>(data_);
    arr.push_back(val);
}

Value Value::pop() {
    // (AR) حذف وإرجاع آخر عنصر / (EN) Remove and return last element
    if (type_ != ValueType::ARRAY) {
        throwInvalidType("pop()");
    }
    
    auto& arr = *std::get<std::shared_ptr<ArrayType>>(data_);
    if (arr.empty()) {
        std::ostringstream oss;
        oss << "(AR) لا يمكن pop من مصفوفة فارغة.\n"
            << "(EN) Cannot pop from empty array.";
        throw std::runtime_error(oss.str());
    }
    
    Value val = arr.back();
    arr.pop_back();
    return val;
}

bool Value::remove(const std::string& key) {
    // (AR) حذف عنصر من القاموس / (EN) Remove element from map
    if (type_ != ValueType::MAP) {
        throwInvalidType("remove()");
    }
    
    auto& map = *std::get<std::shared_ptr<MapType>>(data_);
    return map.erase(key) > 0;
}

void Value::clear() {
    // (AR) مسح جميع العناصر / (EN) Clear all elements
    switch (type_) {
        case ValueType::ARRAY:
            std::get<std::shared_ptr<ArrayType>>(data_)->clear();
            break;
        
        case ValueType::MAP:
            std::get<std::shared_ptr<MapType>>(data_)->clear();
            break;
        
        default:
            throwInvalidType("clear()");
    }
}

bool Value::isEmpty() const {
    // (AR) التحقق من الفراغ / (EN) Check if empty
    switch (type_) {
        case ValueType::ARRAY:
            return std::get<std::shared_ptr<ArrayType>>(data_)->empty();
        
        case ValueType::MAP:
            return std::get<std::shared_ptr<MapType>>(data_)->empty();
        
        case ValueType::STRING:
            return std::get<std::string>(data_).empty();
        
        default:
            throwInvalidType("isEmpty()");
            return true;
    }
}

std::vector<std::string> Value::keys() const {
    // (AR) الحصول على جميع المفاتيح / (EN) Get all keys
    if (type_ != ValueType::MAP) {
        throwInvalidType("keys()");
    }
    
    const auto& map = *std::get<std::shared_ptr<MapType>>(data_);
    std::vector<std::string> result;
    result.reserve(map.size());
    
    for (const auto& pair : map) {
        result.push_back(pair.first);
    }
    
    return result;
}

std::vector<Value> Value::values() const {
    // (AR) الحصول على جميع القيم / (EN) Get all values
    if (type_ != ValueType::MAP) {
        throwInvalidType("values()");
    }
    
    const auto& map = *std::get<std::shared_ptr<MapType>>(data_);
    std::vector<Value> result;
    result.reserve(map.size());
    
    for (const auto& pair : map) {
        result.push_back(pair.second);
    }
    
    return result;
}

// ========================================
// Private Helper Methods / دوال مساعدة خاصة
// ========================================

void Value::throwTypeMismatch(const std::string& operation, const Value& other) const {
    std::ostringstream oss;
    oss << "(AR) خطأ في الأنواع: لا يمكن تطبيق " << operation 
        << " على " << getTypeName() << " و " << other.getTypeName() << ".\n"
        << "(EN) Type mismatch: Cannot apply " << operation 
        << " to " << getTypeName() << " and " << other.getTypeName() << ".";
    throw std::runtime_error(oss.str());
}

void Value::throwInvalidType(const std::string& operation) const {
    std::ostringstream oss;
    oss << "(AR) نوع غير صالح: لا يمكن تطبيق " << operation 
        << " على " << getTypeName() << ".\n"
        << "(EN) Invalid type: Cannot apply " << operation 
        << " to " << getTypeName() << ".";
    throw std::runtime_error(oss.str());
}

} // namespace Data
} // namespace Sad
