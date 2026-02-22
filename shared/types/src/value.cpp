/**
 * @file value.cpp
 * @brief (AR) تنفيذ فئة القيمة في وقت التشغيل
 * @brief (EN) Runtime value class implementation
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) هذا الملف يحتوي على تنفيذ جميع دوال فئة Value
 *      بما في ذلك دعم نوع OBJECT الجديد لتمثيل كائنات الأصناف
 *      
 *      التغييرات الرئيسية في الإصدار 2.0:
 *      - إضافة منشئ Value(ObjectPtr) لإنشاء قيم كائنات
 *      - إضافة toObject() للحصول على مؤشر الكائن
 *      - إضافة getClassName() للحصول على اسم الصنف
 *      - إضافة isObjectLike() للتوافق مع الكود القديم
 *      - تحديث toString(), toBool(), getTypeName(), clone() لدعم OBJECT
 *
 * (EN) This file contains the implementation of all Value class methods
 *      including support for the new OBJECT type for class instances
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @author S Language Development Team
 * @date November 21, 2025 - يناير 2026
 * @version 2.0
 */

// NOTE(#23): (AR) لا يوجد حالياً تجميع/مشاركة النصوص (string interning/pool).
//            كل نص يُخزّن كنسخة مستقلة. يمكن إضافة string pool في المستقبل لتوفير الذاكرة.
// NOTE(#23): (EN) No string interning/deduplication is currently implemented.
//            Each string is stored as an independent copy. A string pool could be added later.
//
// NOTE(#26): (AR) MapType يستخدم std::unordered_map — ترتيب التكرار غير حتمي.
//            إذا أراد المستخدم ترتيباً ثابتاً، يجب استخدام مصفوفة من الأزواج أو std::map.
// NOTE(#26): (EN) MapType uses std::unordered_map — iteration order is non-deterministic.
//            If deterministic order is needed, use an array of pairs or std::map.

#include "value.h"
#include "object_instance.h"
#include <cmath>
#include <iomanip>
#include <limits>
#include <climits>

namespace {

// (AR) حماية طفحان الأعداد الصحيحة — تمنع السلوك غير المحدد
// (EN) Integer overflow protection — prevents undefined behavior
inline bool willAddOverflow(int a, int b) {
    if (b > 0 && a > INT_MAX - b) return true;
    if (b < 0 && a < INT_MIN - b) return true;
    return false;
}
inline bool willSubOverflow(int a, int b) {
    if (b < 0 && a > INT_MAX + b) return true;
    if (b > 0 && a < INT_MIN + b) return true;
    return false;
}
inline bool willMulOverflow(int a, int b) {
    if (a == 0 || b == 0) return false;
    if (a > 0 && b > 0 && a > INT_MAX / b) return true;
    if (a < 0 && b < 0 && a < INT_MAX / b) return true;
    if (a > 0 && b < 0 && b < INT_MIN / a) return true;
    if (a < 0 && b > 0 && a < INT_MIN / b) return true;
    return false;
}

// (AR) حساب عدد أحرف UTF-8 (ليس البايتات)
// (EN) Count UTF-8 characters (not bytes)
inline size_t utf8CharCount(const std::string& s) {
    size_t count = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) { i += 1; }
        else if ((c & 0xE0) == 0xC0) { i += 2; }
        else if ((c & 0xF0) == 0xE0) { i += 3; }
        else if ((c & 0xF8) == 0xF0) { i += 4; }
        else { i += 1; } // invalid byte, skip
        ++count;
    }
    return count;
}

// (AR) الحد الأقصى لتكرار النصوص/المصفوفات لمنع استنفاد الذاكرة
// (EN) Max repetition limit to prevent memory exhaustion  
static constexpr int MAX_REPETITION = 1000000;
static constexpr size_t MAX_STRING_SIZE = 100 * 1024 * 1024; // 100MB

} // anonymous namespace

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

// ════════════════════════════════════════════════════════════════════════
// (AR) منشئ الكائن — يُنشئ قيمة من نوع OBJECT تحمل مؤشراً مشتركاً
//      للكائن. الكائن يُمرّر بالمرجع عند تمريره للدوال أو تعيينه
//      لمتغيرات أخرى (سلوك مشابه لـ Python/Java)
//
// (EN) Object constructor — creates an OBJECT value holding a shared_ptr
//      to the object instance. Objects are passed by reference when passed
//      to functions or assigned to other variables (Python/Java-like behavior)
// ════════════════════════════════════════════════════════════════════════
Value::Value(ObjectPtr obj) 
    : type_(ValueType::OBJECT), data_(std::move(obj)) {}

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
        case ValueType::OBJECT: {
            // (AR) نسخ عميق للكائن: نُنشئ ObjectInstance جديد بنفس الحقول
            // (EN) Deep clone for object: create new ObjectInstance with same fields
            auto srcPtr = std::get<std::shared_ptr<ObjectInstance>>(data_);
            if (srcPtr) {
                // (AR) نسخ عميق بمعرف فريد جديد
                // (EN) Deep clone with new unique object ID
                auto newObj = std::make_shared<ObjectInstance>(srcPtr->classType, generateObjectId());
                newObj->fields = srcPtr->fields;
                newObj->isConstructed = srcPtr->isConstructed;
                // (AR) نسخ الكائن الأساسي إذا وُجد
                // (EN) Clone base instance if present
                if (srcPtr->baseInstance) {
                    newObj->baseInstance = std::make_unique<ObjectInstance>(
                        srcPtr->baseInstance->classType, generateObjectId());
                    newObj->baseInstance->fields = srcPtr->baseInstance->fields;
                    newObj->baseInstance->isConstructed = srcPtr->baseInstance->isConstructed;
                }
                return Value(std::move(newObj));
            }
            return *this;
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
        
        case ValueType::OBJECT: {
            // (AR) تحويل الكائن إلى نص — يستخدم دالة toString() من ObjectInstance
            //      إذا كان المؤشر فارغاً، يُرجع "كائن_فارغ"
            // (EN) Convert object to string — uses toString() from ObjectInstance
            //      If pointer is null, returns "null_object"
            const auto& objPtr = std::get<std::shared_ptr<ObjectInstance>>(data_);
            if (objPtr) {
                return objPtr->toString();
            }
            return "كائن_فارغ";  // null object
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
        
        case ValueType::OBJECT: {
            // (AR) الكائن صحيح إذا كان المؤشر غير فارغ
            // (EN) Object is true if the pointer is not null
            const auto& objPtr = std::get<std::shared_ptr<ObjectInstance>>(data_);
            return objPtr != nullptr;
        }
        
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
    // (AR) إذا كان كائناً، نحوله لقاموس من الحقول للتوافق مع الكود القديم
    // (EN) If it's an object, convert to map of fields for backward compatibility
    if (type_ == ValueType::OBJECT) {
        const auto& objPtr = std::get<std::shared_ptr<ObjectInstance>>(data_);
        if (objPtr) {
            MapType result = objPtr->fields;
            result["__class__"] = Value(objPtr->getClassName());
            return result;
        }
    }
    throwInvalidType("toMap - value is not a map");
    return MapType();
}

// ════════════════════════════════════════════════════════════════════════
// (AR) إصدارات مرجعية ثابتة — تتجنب النسخ عندما نحتاج فقط للقراءة
// (EN) Const reference versions — avoid copying when only reading
// ════════════════════════════════════════════════════════════════════════

const Value::ArrayType& Value::toArrayRef() const {
    if (type_ == ValueType::ARRAY) {
        return *std::get<std::shared_ptr<ArrayType>>(data_);
    }
    throwInvalidType("toArrayRef - value is not an array");
    // (AR) لن يصل هنا أبداً بسبب الاستثناء أعلاه
    // (EN) Never reached due to exception above
    static const ArrayType empty;
    return empty;
}

const Value::MapType& Value::toMapRef() const {
    if (type_ == ValueType::MAP) {
        return *std::get<std::shared_ptr<MapType>>(data_);
    }
    throwInvalidType("toMapRef - value is not a map");
    // (AR) لن يصل هنا أبداً بسبب الاستثناء أعلاه
    // (EN) Never reached due to exception above
    static const MapType empty;
    return empty;
}

// ════════════════════════════════════════════════════════════════════════
// (AR) دالة toObject — الحصول على مؤشر الكائن
//      يجب أن تكون القيمة من نوع OBJECT
//
// (EN) toObject function — get the object pointer
//      Value must be of OBJECT type
// ════════════════════════════════════════════════════════════════════════
Value::ObjectPtr Value::toObject() const {
    if (type_ == ValueType::OBJECT) {
        return std::get<std::shared_ptr<ObjectInstance>>(data_);
    }
    throwInvalidType("toObject - القيمة ليست كائناً / value is not an object");
    return nullptr;
}

// ════════════════════════════════════════════════════════════════════════
// (AR) دالة getClassName — الحصول على اسم الصنف
//      تعمل مع نوع OBJECT الجديد ومع MAP القديم الذي يحتوي على __class__
//
// (EN) getClassName function — get the class name
//      Works with new OBJECT type and legacy MAP with __class__
// ════════════════════════════════════════════════════════════════════════
std::string Value::getClassName() const {
    // (AR) أولاً: التحقق من نوع OBJECT الحقيقي
    // (EN) First: check for real OBJECT type
    if (type_ == ValueType::OBJECT) {
        const auto& objPtr = std::get<std::shared_ptr<ObjectInstance>>(data_);
        if (objPtr) {
            return objPtr->getClassName();
        }
        return "";
    }
    // (AR) ثانياً: التوافق مع MAP القديم
    // (EN) Second: backward compatibility with legacy MAP
    if (type_ == ValueType::MAP) {
        const auto& map = *std::get<std::shared_ptr<MapType>>(data_);
        auto it = map.find("__class__");
        if (it != map.end()) {
            return it->second.toString();
        }
    }
    return "";
}

// ════════════════════════════════════════════════════════════════════════
// (AR) دالة isObjectLike — هل القيمة تمثل كائناً بأي شكل؟
//      تكشف كلاً من:
//      1. نوع OBJECT الحقيقي (الجديد)
//      2. MAP القديم الذي يحتوي على مفتاح __class__
//
// (EN) isObjectLike function — does the value represent an object?
//      Detects both:
//      1. Real OBJECT type (new)
//      2. Legacy MAP containing __class__ key
// ════════════════════════════════════════════════════════════════════════
bool Value::isObjectLike() const {
    if (type_ == ValueType::OBJECT) return true;
    if (type_ == ValueType::MAP) {
        const auto& map = *std::get<std::shared_ptr<MapType>>(data_);
        return map.find("__class__") != map.end();
    }
    return false;
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
    
    // (AR) جمع الأعداد مع حماية الطفحان / (EN) Numeric addition with overflow protection
    if (isNumeric() && other.isNumeric()) {
        if (type_ == ValueType::DOUBLE || other.type_ == ValueType::DOUBLE) {
            return Value(toDouble() + other.toDouble());
        }
        int a = toInt(), b = other.toInt();
        if (willAddOverflow(a, b)) {
            // (AR) ترقية تلقائية إلى عشري عند الطفحان
            return Value(static_cast<double>(a) + static_cast<double>(b));
        }
        return Value(a + b);
    }
    
    throwTypeMismatch("addition (+)", other);
    return Value();
}

Value Value::operator-(const Value& other) const {
    if (isNumeric() && other.isNumeric()) {
        if (type_ == ValueType::DOUBLE || other.type_ == ValueType::DOUBLE) {
            return Value(toDouble() - other.toDouble());
        }
        int a = toInt(), b = other.toInt();
        if (willSubOverflow(a, b)) {
            return Value(static_cast<double>(a) - static_cast<double>(b));
        }
        return Value(a - b);
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
        if (count > MAX_REPETITION) {
            throw std::runtime_error("(AR) خطأ: عدد التكرار كبير جداً (" + std::to_string(count) + "). الحد الأقصى: " + std::to_string(MAX_REPETITION) + "\n(EN) Error: Repetition count too large.");
        }
        ArrayType result;
        result.reserve(arr.size() * count);
        for (int i = 0; i < count; ++i)
            result.insert(result.end(), arr.begin(), arr.end());
        return Value(result);
    }
    if (other.type_ == ValueType::ARRAY && isNumeric()) {
        ArrayType arr = other.toArray();
        int count = toInt();
        if (count <= 0) return Value(ArrayType{});
        if (count > MAX_REPETITION) {
            throw std::runtime_error("(AR) خطأ: عدد التكرار كبير جداً.\n(EN) Error: Repetition count too large.");
        }
        ArrayType result;
        result.reserve(arr.size() * count);
        for (int i = 0; i < count; ++i)
            result.insert(result.end(), arr.begin(), arr.end());
        return Value(result);
    }
    // (AR) تكرار النصوص مع حماية الذاكرة
    if (type_ == ValueType::STRING && other.isNumeric()) {
        std::string s = toString();
        int count = other.toInt();
        if (count <= 0) return Value(std::string(""));
        if (count > MAX_REPETITION || s.size() * count > MAX_STRING_SIZE) {
            throw std::runtime_error("(AR) خطأ: تكرار النص سينتج نصاً كبيراً جداً.\n(EN) Error: String repetition would produce too large a string.");
        }
        std::string result;
        result.reserve(s.size() * count);
        for (int i = 0; i < count; ++i) result += s;
        return Value(result);
    }
    if (other.type_ == ValueType::STRING && isNumeric()) {
        std::string s = other.toString();
        int count = toInt();
        if (count <= 0) return Value(std::string(""));
        if (count > MAX_REPETITION || s.size() * count > MAX_STRING_SIZE) {
            throw std::runtime_error("(AR) خطأ: تكرار النص سينتج نصاً كبيراً جداً.\n(EN) Error: String repetition would produce too large a string.");
        }
        std::string result;
        result.reserve(s.size() * count);
        for (int i = 0; i < count; ++i) result += s;
        return Value(result);
    }
    
    if (isNumeric() && other.isNumeric()) {
        if (type_ == ValueType::DOUBLE || other.type_ == ValueType::DOUBLE) {
            return Value(toDouble() * other.toDouble());
        }
        int a = toInt(), b = other.toInt();
        if (willMulOverflow(a, b)) {
            return Value(static_cast<double>(a) * static_cast<double>(b));
        }
        return Value(a * b);
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
    // (AR) باقي القسمة يعمل مع الأعداد الصحيحة والعشرية
    // (EN) Modulus works with integers and doubles
    if (isInteger() && other.isInteger()) {
        int otherInt = other.toInt();
        if (otherInt == 0) {
            throw std::runtime_error(
                "(AR) خطأ: باقي القسمة على صفر. (EN) Error: Modulus by zero."
            );
        }
        return Value(toInt() % otherInt);
    }
    
    // (AR) دعم باقي القسمة للأعداد العشرية باستخدام fmod
    // (EN) Support modulus for doubles using fmod
    if (isNumeric() && other.isNumeric()) {
        double otherD = other.toDouble();
        if (otherD == 0.0) {
            throw std::runtime_error(
                "(AR) خطأ: باقي القسمة على صفر. (EN) Error: Modulus by zero."
            );
        }
        return Value(std::fmod(toDouble(), otherD));
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
        
        case ValueType::ARRAY: {
            // (AR) مقارنة عنصرية للمصفوفات — [1,2] == [1,2] ترجع صحيح
            // (EN) Element-wise array comparison — [1,2] == [1,2] returns true
            const auto& arr1 = *std::get<std::shared_ptr<ArrayType>>(data_);
            const auto& arr2 = *std::get<std::shared_ptr<ArrayType>>(other.data_);
            if (arr1.size() != arr2.size()) return Value(false);
            for (size_t i = 0; i < arr1.size(); ++i) {
                if (!(arr1[i] == arr2[i]).toBool()) return Value(false);
            }
            return Value(true);
        }
        
        case ValueType::MAP: {
            // (AR) مقارنة عنصرية للقواميس
            // (EN) Element-wise map comparison
            const auto& map1 = *std::get<std::shared_ptr<MapType>>(data_);
            const auto& map2 = *std::get<std::shared_ptr<MapType>>(other.data_);
            if (map1.size() != map2.size()) return Value(false);
            for (const auto& [key, val] : map1) {
                auto it = map2.find(key);
                if (it == map2.end()) return Value(false);
                if (!(val == it->second).toBool()) return Value(false);
            }
            return Value(true);
        }
        
        case ValueType::OBJECT: {
            // (AR) مقارنة الكائنات: نقارن بالمرجع (هل هما نفس الكائن؟)
            // (EN) Object comparison: compare by reference (are they the same object?)
            const auto& obj1 = std::get<std::shared_ptr<ObjectInstance>>(data_);
            const auto& obj2 = std::get<std::shared_ptr<ObjectInstance>>(other.data_);
            return Value(obj1.get() == obj2.get());
        }
        
        default:
            break;
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
    // (AR) تحسين الأداء: تجنب التقييم المزدوج
    // (EN) Performance: avoid double evaluation
    if (isNumeric() && other.isNumeric()) {
        return Value(toDouble() <= other.toDouble());
    }
    if (isString() && other.isString()) {
        return Value(std::get<std::string>(data_) <= std::get<std::string>(other.data_));
    }
    return Value((*this < other).toBool() || (*this == other).toBool());
}

Value Value::operator>=(const Value& other) const {
    if (isNumeric() && other.isNumeric()) {
        return Value(toDouble() >= other.toDouble());
    }
    if (isString() && other.isString()) {
        return Value(std::get<std::string>(data_) >= std::get<std::string>(other.data_));
    }
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
        case ValueType::OBJECT: {
            // (AR) للكائنات: نُرجع "OBJECT:اسم_الصنف" للتوضيح
            // (EN) For objects: return "OBJECT:ClassName" for clarity
            const auto& objPtr = std::get<std::shared_ptr<ObjectInstance>>(data_);
            if (objPtr) {
                return "OBJECT:" + objPtr->getClassName();
            }
            return "OBJECT";
        }
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
            // (AR) إرجاع عدد الأحرف وليس البايتات — مهم للنصوص العربية
            // (EN) Return character count not byte count — important for Arabic text
            return utf8CharCount(std::get<std::string>(data_));
        
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

void Value::remove(size_t index) {
    // (AR) حذف عنصر من المصفوفة بالفهرس / (EN) Remove element from array by index
    if (type_ != ValueType::ARRAY) {
        throwInvalidType("remove()");
    }
    
    auto& arr = *std::get<std::shared_ptr<ArrayType>>(data_);
    if (index >= arr.size()) {
        std::ostringstream oss;
        oss << "(AR) الفهرس " << index << " خارج حدود المصفوفة (الحجم: " << arr.size() << ").\n"
            << "(EN) Index " << index << " out of bounds (size: " << arr.size() << ").";
        throw std::runtime_error(oss.str());
    }
    arr.erase(arr.begin() + static_cast<std::ptrdiff_t>(index));
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
