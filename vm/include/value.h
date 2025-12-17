/**
 * @file value.h
 * @brief نظام القيم للآلة الافتراضية / Value System for Virtual Machine
 * @brief Complete value representation system for Sad VM
 * 
 * @details
 * (AR) يحدد هذا الملف نظام القيم الكامل للآلة الافتراضية، بما في ذلك:
 *      - أنواع البيانات الأساسية (integers, floats, booleans)
 *      - الأنواع المرجعية (strings, arrays, objects)
 *      - المؤشرات (للمرحلة 1)
 *      - الدوال والإغلاقات
 * 
 * (EN) This file defines the complete value system for the VM, including:
 *      - Primitive types (integers, floats, booleans)
 *      - Reference types (strings, arrays, objects)
 *      - Pointers (for Stage 1 compatibility)
 *      - Functions and closures
 * 
 * @memory_model Tagged union with type field
 * @size 16 bytes per value (type + data)
 * @gc Tri-color mark-and-sweep
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 * @phase Phase 3: Bytecode Backend
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Sad {
namespace VM {

// Forward declarations
class SadVM;
class GarbageCollector;
struct Value;
struct Object;

// ========================================
// Value Types
// أنواع القيم
// ========================================

/**
 * @brief نوع القيمة / Value Type
 * @brief Enumeration of all value types
 */
enum class ValueType : uint8_t {
    // Primitive Types (غير مرجعية)
    VAL_NULL = 0,      ///< Null value / قيمة فارغة
    VAL_BOOL,          ///< Boolean / منطقي
    VAL_INT,           ///< 64-bit integer / عدد صحيح 64 بت
    VAL_FLOAT,         ///< 64-bit float / عدد عشري 64 بت
    
    // Reference Types (مرجعية - تحتاج GC)
    VAL_STRING,        ///< String object / كائن نص
    VAL_ARRAY,         ///< Array object / كائن مصفوفة
    VAL_OBJECT,        ///< Object / كائن
    VAL_FUNCTION,      ///< Function / دالة
    VAL_CLOSURE,       ///< Closure / إغلاق
    VAL_NATIVE,        ///< Native function / دالة أصلية
    
    // Pointer Types (للمرحلة 1)
    VAL_POINTER,       ///< Raw pointer / مؤشر خام
};

// ========================================
// Object Types
// أنواع الكائنات
// ========================================

/**
 * @brief نوع الكائن / Object Type
 * @brief Base type for all heap-allocated objects
 */
enum class ObjectType : uint8_t {
    OBJ_STRING,        ///< String object
    OBJ_ARRAY,         ///< Array object
    OBJ_MAP,           ///< Map/Dictionary object
    OBJ_FUNCTION,      ///< Function object
    OBJ_CLOSURE,       ///< Closure object
    OBJ_NATIVE,        ///< Native function
    OBJ_UPVALUE,       ///< Upvalue (for closures)
};

// ========================================
// GC Color
// لون جامع القمامة
// ========================================

/**
 * @brief لون GC / GC Color
 * @brief Tri-color marking for garbage collection
 */
enum class GCColor : uint8_t {
    WHITE = 0,         ///< Not reached (قابل للجمع)
    GRAY,              ///< Reached but not processed (رمادي)
    BLACK,             ///< Reached and processed (أسود)
};

// ========================================
// Object Base Class
// الصنف الأساسي للكائنات
// ========================================

/**
 * @brief كائن أساسي / Base Object
 * @brief Base class for all heap-allocated objects
 * 
 * @details
 * (AR) جميع الكائنات في الذاكرة heap ترث من هذا الصنف.
 *      يحتوي على المعلومات اللازمة لـ GC.
 * 
 * (EN) All heap-allocated objects inherit from this class.
 *      Contains necessary information for GC.
 */
struct Object {
    ObjectType type;       ///< نوع الكائن / Object type
    GCColor gcColor;       ///< لون GC / GC color
    Object* next;          ///< السلسلة المترابطة / Linked list
    bool isMarked;         ///< هل مُعلَّم؟ / Is marked?
    
    Object(ObjectType t) 
        : type(t), gcColor(GCColor::WHITE), next(nullptr), isMarked(false) {}
    
    virtual ~Object() = default;
};

// ========================================
// String Object
// كائن النص
// ========================================

/**
 * @brief كائن نص / String Object
 * @brief Immutable string object
 */
struct StringObject : public Object {
    size_t length;         ///< الطول / Length
    uint32_t hash;         ///< Hash للبحث السريع / Hash for fast lookup
    char* chars;           ///< البيانات / Data
    
    StringObject() : Object(ObjectType::OBJ_STRING), length(0), hash(0), chars(nullptr) {}
    
    ~StringObject() {
        if (chars) {
            delete[] chars;
        }
    }
    
    std::string toString() const {
        return std::string(chars, length);
    }
};

// ========================================
// Array Object
// كائن المصفوفة
// ========================================

/**
 * @brief كائن مصفوفة / Array Object
 * @brief Dynamic array of values
 */
struct ArrayObject : public Object {
    std::vector<Value> elements;  ///< العناصر / Elements
    
    ArrayObject() : Object(ObjectType::OBJ_ARRAY) {}
    
    size_t length() const {
        return elements.size();
    }
};

// ========================================
// Map Object
// كائن الخريطة
// ========================================

/**
 * @brief كائن خريطة / Map Object
 * @brief Hash map for object properties
 */
struct MapObject : public Object {
    std::unordered_map<std::string, Value> fields;  ///< الحقول / Fields
    
    MapObject() : Object(ObjectType::OBJ_MAP) {}
};

// ========================================
// Function Object
// كائن الدالة
// ========================================

/**
 * @brief كائن دالة / Function Object
 * @brief Compiled function
 */
struct FunctionObject : public Object {
    std::string name;          ///< اسم الدالة / Function name
    int arity;                 ///< عدد المعاملات / Parameter count
    int upvalueCount;          ///< عدد القيم العلوية / Upvalue count
    std::vector<uint8_t> code; ///< البايت كود / Bytecode
    std::vector<int> lines;    ///< أرقام الأسطر / Line numbers
    
    FunctionObject() 
        : Object(ObjectType::OBJ_FUNCTION), arity(0), upvalueCount(0) {}
};

// ========================================
// Native Function
// دالة أصلية
// ========================================

/**
 * @brief دالة أصلية / Native Function
 * @brief C++ function callable from Sad
 */
using NativeFn = Value (*)(SadVM* vm, int argCount, Value* args);

struct NativeObject : public Object {
    NativeFn function;         ///< مؤشر الدالة / Function pointer
    std::string name;          ///< الاسم / Name
    
    NativeObject(NativeFn fn, const std::string& n)
        : Object(ObjectType::OBJ_NATIVE), function(fn), name(n) {}
};

// ========================================
// Upvalue Object
// كائن القيمة العلوية
// ========================================

/**
 * @brief قيمة علوية / Upvalue
 * @brief Captured variable for closures
 */
struct UpvalueObject : public Object {
    Value* location;           ///< الموقع في المكدس / Location on stack
    Value closed;              ///< القيمة المغلقة / Closed value
    UpvalueObject* next;       ///< السلسلة / Linked list
    
    UpvalueObject(Value* loc)
        : Object(ObjectType::OBJ_UPVALUE), location(loc), next(nullptr) {}
};

// ========================================
// Closure Object
// كائن الإغلاق
// ========================================

/**
 * @brief إغلاق / Closure
 * @brief Function with captured variables
 */
struct ClosureObject : public Object {
    FunctionObject* function;              ///< الدالة / Function
    std::vector<UpvalueObject*> upvalues;  ///< القيم المحتجزة / Captured values
    
    ClosureObject(FunctionObject* fn)
        : Object(ObjectType::OBJ_CLOSURE), function(fn) {
        upvalues.resize(fn->upvalueCount, nullptr);
    }
};

// ========================================
// Value Structure
// بنية القيمة
// ========================================

/**
 * @brief قيمة / Value
 * @brief Tagged union for all Sad values
 * 
 * @details
 * (AR) قيمة واحدة في الآلة الافتراضية. تستخدم tagged union
 *      لتخزين أنواع مختلفة من البيانات بكفاءة.
 * 
 * (EN) Single value in the VM. Uses tagged union for efficient
 *      storage of different data types.
 * 
 * @size 16 bytes (8 byte type + 8 byte data)
 */
struct Value {
    ValueType type;            ///< النوع / Type
    
    union {
        bool boolean;          ///< قيمة منطقية / Boolean value
        int64_t integer;       ///< عدد صحيح / Integer value
        double floating;       ///< عدد عشري / Float value
        Object* object;        ///< كائن / Object pointer
        void* pointer;         ///< مؤشر خام / Raw pointer
    } as;
    
    // ========================================
    // Constructors
    // ========================================
    
    Value() : type(ValueType::VAL_NULL) {
        as.integer = 0;
    }
    
    static Value Null() {
        Value v;
        v.type = ValueType::VAL_NULL;
        return v;
    }
    
    static Value Bool(bool b) {
        Value v;
        v.type = ValueType::VAL_BOOL;
        v.as.boolean = b;
        return v;
    }
    
    static Value Int(int64_t i) {
        Value v;
        v.type = ValueType::VAL_INT;
        v.as.integer = i;
        return v;
    }
    
    static Value Float(double f) {
        Value v;
        v.type = ValueType::VAL_FLOAT;
        v.as.floating = f;
        return v;
    }
    
    static Value String(StringObject* str) {
        Value v;
        v.type = ValueType::VAL_STRING;
        v.as.object = str;
        return v;
    }
    
    static Value Array(ArrayObject* arr) {
        Value v;
        v.type = ValueType::VAL_ARRAY;
        v.as.object = arr;
        return v;
    }
    
    static Value Object(MapObject* obj) {
        Value v;
        v.type = ValueType::VAL_OBJECT;
        v.as.object = obj;
        return v;
    }
    
    static Value Function(FunctionObject* fn) {
        Value v;
        v.type = ValueType::VAL_FUNCTION;
        v.as.object = fn;
        return v;
    }
    
    static Value Closure(ClosureObject* closure) {
        Value v;
        v.type = ValueType::VAL_CLOSURE;
        v.as.object = closure;
        return v;
    }
    
    static Value Native(NativeObject* native) {
        Value v;
        v.type = ValueType::VAL_NATIVE;
        v.as.object = native;
        return v;
    }
    
    static Value Pointer(void* ptr) {
        Value v;
        v.type = ValueType::VAL_POINTER;
        v.as.pointer = ptr;
        return v;
    }
    
    // ========================================
    // Type Checks
    // ========================================
    
    bool isNull() const { return type == ValueType::VAL_NULL; }
    bool isBool() const { return type == ValueType::VAL_BOOL; }
    bool isInt() const { return type == ValueType::VAL_INT; }
    bool isFloat() const { return type == ValueType::VAL_FLOAT; }
    bool isNumber() const { return isInt() || isFloat(); }
    bool isString() const { return type == ValueType::VAL_STRING; }
    bool isArray() const { return type == ValueType::VAL_ARRAY; }
    bool isObject() const { return type == ValueType::VAL_OBJECT; }
    bool isFunction() const { return type == ValueType::VAL_FUNCTION; }
    bool isClosure() const { return type == ValueType::VAL_CLOSURE; }
    bool isNative() const { return type == ValueType::VAL_NATIVE; }
    bool isPointer() const { return type == ValueType::VAL_POINTER; }
    bool isCallable() const { return isFunction() || isClosure() || isNative(); }
    
    // ========================================
    // Type Conversions
    // ========================================
    
    bool asBool() const { return as.boolean; }
    int64_t asInt() const { return as.integer; }
    double asFloat() const { return as.floating; }
    Object* asObject() const { return as.object; }
    void* asPointer() const { return as.pointer; }
    
    StringObject* asString() const { 
        return static_cast<StringObject*>(as.object); 
    }
    
    ArrayObject* asArray() const { 
        return static_cast<ArrayObject*>(as.object); 
    }
    
    MapObject* asMap() const { 
        return static_cast<MapObject*>(as.object); 
    }
    
    FunctionObject* asFunction() const { 
        return static_cast<FunctionObject*>(as.object); 
    }
    
    ClosureObject* asClosure() const { 
        return static_cast<ClosureObject*>(as.object); 
    }
    
    NativeObject* asNative() const { 
        return static_cast<NativeObject*>(as.object); 
    }
    
    // ========================================
    // Utilities
    // ========================================
    
    /**
     * @brief تحويل إلى نص / Convert to string
     */
    std::string toString() const;
    
    /**
     * @brief تحويل إلى قيمة منطقية / Convert to boolean
     */
    bool toBool() const {
        switch (type) {
            case ValueType::VAL_NULL: return false;
            case ValueType::VAL_BOOL: return as.boolean;
            case ValueType::VAL_INT: return as.integer != 0;
            case ValueType::VAL_FLOAT: return as.floating != 0.0;
            default: return true; // Objects are truthy
        }
    }
    
    /**
     * @brief تحويل إلى عدد / Convert to number
     */
    double toNumber() const {
        if (isInt()) return static_cast<double>(as.integer);
        if (isFloat()) return as.floating;
        if (isBool()) return as.boolean ? 1.0 : 0.0;
        return 0.0;
    }
    
    /**
     * @brief المساواة / Equality
     */
    bool equals(const Value& other) const;
};

// ========================================
// Value Utilities
// أدوات القيم
// ========================================

/**
 * @brief الحصول على اسم النوع / Get type name
 */
const char* getValueTypeName(ValueType type);

/**
 * @brief طباعة قيمة / Print value
 */
void printValue(const Value& value);

/**
 * @brief طباعة قيمة بالعربية / Print value in Arabic
 */
void printValueArabic(const Value& value);

} // namespace VM
} // namespace Sad
