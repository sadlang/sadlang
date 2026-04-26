/**
 * @file value.h
 * @brief (AR) نظام القيم الموحَّد للآلة الافتراضية — النوع الأساسي لجميع البيانات
 * @brief (EN) Unified Value system for the Virtual Machine — core data type
 *
 * @details
 * ═══════════════════════════════════════════════════════════════════════════
 * (AR) شرح موسّع — نظام القيم (Value System)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * ما هو نظام القيم؟
 * ─────────────────
 * في أي لغة برمجة، يجب أن يكون هناك تمثيل داخلي موحَّد لجميع القيم التي
 * يتعامل معها البرنامج (أرقام، نصوص، مصفوفات، كائنات...). في لغة ص، هذا
 * التمثيل هو بنية Value التي تستخدم tagged union (اتحاد موسوم) بحجم 16 بايت.
 *
 * هيكل الملف — 6 أقسام مرتبة بعناية:
 * ────────────────────────────────────
 * ⚠ ترتيب التعريفات في هذا الملف حرج بسبب الاعتماديات الدائرية!
 *
 *   القسم 1: التعدادات (ValueType, ObjectType, GCColor)
 *   ├── ValueType: 9 أنواع (NULL_TYPE, INTEGER, FLOAT, BOOLEAN, STRING,
 *   │              ARRAY, OBJECT, FUNCTION, POINTER)
 *   ├── ObjectType: أنواع الكائنات المكدَّسة (STRING, ARRAY, MAP, ...)
 *   └── GCColor: ألوان جامع القمامة ثلاثي الألوان (WHITE, GRAY, BLACK)
 *
 *   القسم 2: التصريحات الأمامية (Forward Declarations)
 *   ├── StringObject, ArrayObject, MapObject
 *   ├── FunctionObject, ClosureObject, UpvalueObject
 *   └── ClassObject, InstanceObject, ModuleObject
 *
 *   القسم 3: الصنف الأساسي Object
 *   └── يحتوي: type, gc_color, marked, next (لقائمة GC المتصلة)
 *
 *   القسم 4: بنية Value (التعريف الرئيسي)
 *   ├── type: ValueType — نوع القيمة
 *   ├── data: union {int64_t, double, bool, Object*} — البيانات
 *   ├── دوال المصنع الساكنة: Int(), Float(), Bool(), Null(), Obj()
 *   ├── دوال الوصول: asInt(), asFloat(), asBool(), asObject()
 *   ├── دوال الاستعلام: isInt(), isFloat(), isBool(), isNull(), isObject()
 *   └── العمليات الحسابية: +, -, *, /, ==, <, إلخ
 *
 *   القسم 5: أنواع الكائنات المشتقة
 *   ├── StringObject: نص (يغلِّف std::string)
 *   ├── ArrayObject: مصفوفة (std::vector<Value>)
 *   ├── MapObject: خريطة (std::unordered_map<std::string, Value>)
 *   ├── FunctionObject: دالة بايت كود (اسم، عدد المعاملات، الكود)
 *   ├── ClosureObject: إغلاق (FunctionObject + upvalues)
 *   └── UpvalueObject: قيمة مُلتقَطة (مؤشر أو قيمة مغلقة)
 *
 *   القسم 6: التعريفات المؤجلة (Deferred Definitions)
 *   └── دوال Value التي تعتمد على الأنواع المشتقة (مثل asString())
 *
 * نموذج الذاكرة:
 * ─────────────
 * - القيم الأوّلية (رقم، عشري، منطقي): تُخزَّن مباشرة في union (بالقيمة)
 * - الكائنات (نص، مصفوفة، خريطة): تُخزَّن كمؤشر Object* (بالمرجع)
 * - حجم Value ثابت: 16 بايت (8 بايت type+padding + 8 بايت data)
 * - جامع القمامة: ثلاثي الألوان (tri-color mark-and-sweep)
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * (EN) Extended Description — Value System
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * This file defines the complete runtime value representation using a 16-byte
 * tagged union. It is organized into 6 carefully-ordered sections to handle
 * circular dependencies between Value and Object-derived types:
 *   1. Enums (ValueType with 9 types, ObjectType, GCColor)
 *   2. Forward declarations for all object types
 *   3. Object base class (with GC metadata)
 *   4. Value struct (static factories, accessors, operators)
 *   5. Derived object types (String, Array, Map, Function, Closure, Upvalue)
 *   6. Deferred inline definitions for Value methods
 *
 * Memory model: primitives stored by value, objects by Object* pointer.
 * GC: tri-color mark-and-sweep via Object linked list.
 *
 * @note الاعتماديات / Dependencies: <cstdint>, <string>, <vector>,
 *       <unordered_map>, <memory>
 * @note هذا الملف يُستخدم من vm.h وbytecode_compat.h — أي تغيير هنا يؤثر
 *       على الآلة الافتراضية بأكملها
 *
 * @see runtime_new/include/vm/vm.h — الآلة الافتراضية التي تستخدم Value
 * @see runtime_new/include/bytecode_compat.h — يستورد Value في BytecodeLoader
 * @see shared/types/include/value.h — نوع Value في المفسر (مختلف عن هذا)
 *
 * @memory_model Tagged union with type field
 * @size 16 bytes per value (type + data)
 * @gc Tri-color mark-and-sweep
 *
 * @author فريق لغة ص / Sad Language Team
 * @date ديسمبر 2025 — فبراير 2026 / December 2025 — February 2026
 * @version 2.0 — إعادة هيكلة ترتيب التعريفات / Restructured declaration order
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

// ============================================================================
// التصريحات الأمامية / Forward Declarations
// (AR) نصرّح عن جميع الأنواع مسبقاً لأن بعضها يعتمد على بعض دائرياً.
//      Value يحتاج مؤشرات لأنواع الكائنات المشتقة،
//      وأنواع الكائنات تحتاج Value كعضو (مثل UpvalueObject::closed).
// (EN) Forward-declare all types because some have circular dependencies.
//      Value needs pointers to derived object types,
//      and object types need Value as a member (e.g., UpvalueObject::closed).
// ============================================================================
class SadVM;
class GarbageCollector;
struct Value;
struct Object;
struct StringObject;
struct ArrayObject;
struct MapObject;
struct FunctionObject;
struct NativeObject;
struct UpvalueObject;
struct ClosureObject;

// ============================================================================
// القسم 1: التعدادات / Section 1: Enumerations
// ============================================================================

// ========================================
// Value Types — أنواع القيم
// ========================================

/**
 * @brief نوع القيمة / Value Type
 * @brief Enumeration of all value types
 * 
 * (AR) يحدد نوع البيانات المخزنة في بنية Value.
 *      الأنواع الأساسية (NULL-FLOAT) لا تحتاج GC.
 *      الأنواع المرجعية (STRING-NATIVE) تحتاج GC.
 * (EN) Determines the data type stored in a Value struct.
 *      Primitive types (NULL-FLOAT) don't need GC.
 *      Reference types (STRING-NATIVE) need GC.
 */
enum class ValueType : uint8_t {
    // Primitive Types (غير مرجعية — لا تحتاج جمع قمامة)
    VAL_NULL = 0,      ///< Null value / قيمة فارغة
    VAL_BOOL,          ///< Boolean / منطقي
    VAL_INT,           ///< 64-bit integer / عدد صحيح 64 بت
    VAL_FLOAT,         ///< 64-bit float / عدد عشري 64 بت
    
    // Reference Types (مرجعية — تحتاج جمع قمامة GC)
    VAL_STRING,        ///< String object / كائن نص
    VAL_ARRAY,         ///< Array object / كائن مصفوفة
    VAL_OBJECT,        ///< Object / كائن
    VAL_FUNCTION,      ///< Function / دالة
    VAL_CLOSURE,       ///< Closure / إغلاق
    VAL_NATIVE,        ///< Native function / دالة أصلية
    
    // Pointer Types (للمرحلة 1 — التوافق مع C)
    VAL_POINTER,       ///< Raw pointer / مؤشر خام
};

// ========================================
// Object Types — أنواع الكائنات
// ========================================

/**
 * @brief نوع الكائن / Object Type
 * @brief Base type for all heap-allocated objects
 * 
 * (AR) يحدد النوع الفعلي للكائن المُخصص على الـ heap.
 *      يُستخدم في GC لمعرفة كيفية تتبع وتحرير الكائن.
 * (EN) Determines the actual type of a heap-allocated object.
 *      Used by GC to know how to trace and free the object.
 */
enum class ObjectType : uint8_t {
    OBJ_STRING,        ///< String object / كائن نصّي
    OBJ_ARRAY,         ///< Array object / كائن مصفوفة
    OBJ_MAP,           ///< Map/Dictionary object / كائن خريطة
    OBJ_FUNCTION,      ///< Function object / كائن دالة
    OBJ_CLOSURE,       ///< Closure object / كائن إغلاق
    OBJ_NATIVE,        ///< Native function / دالة أصلية
    OBJ_UPVALUE,       ///< Upvalue (for closures) / قيمة علوية
    OBJ_OBJECT,        ///< Generic object (alias for MAP) / كائن عام (اسم بديل للخريطة)
};

// ========================================
// GC Color — لون جامع القمامة
// ========================================

/**
 * @brief لون GC / GC Color
 * @brief Tri-color marking for garbage collection
 * 
 * (AR) نظام الألوان الثلاثية لجمع القمامة:
 *      أبيض = لم يُصل إليه (قابل للتحرير)
 *      رمادي = وُصل إليه لكن لم يُعالج بعد
 *      أسود = وُصل إليه وعُولج
 * (EN) Tri-color scheme for garbage collection:
 *      White = not reached (eligible for collection)
 *      Gray = reached but not yet processed
 *      Black = reached and fully processed
 */
enum class GCColor : uint8_t {
    WHITE = 0,         ///< Not reached (قابل للجمع)
    GRAY,              ///< Reached but not processed (رمادي)
    BLACK,             ///< Reached and processed (أسود)
};

// ============================================================================
// القسم 2: الصنف الأساسي Object / Section 2: Object Base Class
// ============================================================================

/**
 * @brief كائن أساسي / Base Object
 * @brief Base class for all heap-allocated objects
 * 
 * @details
 * (AR) جميع الكائنات المُخصصة على الـ heap ترث من هذا الصنف.
 *      يحتوي على:
 *        - type: نوع الكائن (لتحديد الصنف المشتق)
 *        - gcColor: لون جمع القمامة (أبيض/رمادي/أسود)
 *        - next: مؤشر للكائن التالي في السلسلة المترابطة
 *        - isMarked: هل تم تعليمه أثناء جمع القمامة؟
 * 
 * (EN) All heap-allocated objects inherit from this class.
 *      Contains:
 *        - type: object type (to identify derived class)
 *        - gcColor: GC color (white/gray/black)
 *        - next: pointer to next object in linked list
 *        - isMarked: marked during garbage collection?
 */
struct Object {
    ObjectType type;       ///< نوع الكائن / Object type
    GCColor gcColor;       ///< لون GC / GC color
    Object* next;          ///< السلسلة المترابطة لـ GC / Linked list for GC
    bool isMarked;         ///< هل مُعلَّم؟ / Is marked?
    
    Object(ObjectType t) 
        : type(t), gcColor(GCColor::WHITE), next(nullptr), isMarked(false) {}
    
    virtual ~Object() = default;
};

// ============================================================================
// القسم 3: بنية القيمة Value / Section 3: Value Structure
// (AR) تُعرَّف هنا قبل الأنواع المشتقة لأن بعض الكائنات (مثل ArrayObject,
//      MapObject, UpvalueObject) تحتاج Value كعضو كامل (وليس مؤشراً فقط).
//      الدوال التي تعتمد على الأنواع المشتقة (مثل String(), asString())
//      مُصرَّح عنها هنا ومُعرَّفة لاحقاً بعد تعريف جميع الأنواع المشتقة.
// (EN) Defined here before derived types because some objects (e.g., ArrayObject,
//      MapObject, UpvalueObject) need Value as a full member (not just pointer).
//      Methods that depend on derived types (e.g., String(), asString())
//      are declared here and defined later after all derived types are defined.
// ============================================================================

/**
 * @brief نوع الدالة الأصلية / Native Function Type
 * (AR) مؤشر لدالة C++ يمكن استدعاؤها من لغة ص.
 *      تأخذ: مؤشر VM، عدد الوسائط، مصفوفة الوسائط.
 *      تُرجع: قيمة Value.
 * (EN) Pointer to C++ function callable from Sad language.
 *      Takes: VM pointer, arg count, args array.
 *      Returns: a Value.
 */
using NativeFn = Value (*)(SadVM* vm, int argCount, Value* args);

/**
 * @brief قيمة / Value
 * @brief Tagged union for all Sad values
 * 
 * @details
 * (AR) قيمة واحدة في الآلة الافتراضية. تستخدم tagged union
 *      لتخزين أنواع مختلفة من البيانات بكفاءة.
 *      الحجم: 16 بايت (8 بايت للنوع + 8 بايت للبيانات).
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
    
    // ====================================================================
    // المنشئ الافتراضي / Default Constructor
    // ====================================================================
    
    Value() : type(ValueType::VAL_NULL) {
        as.integer = 0;
    }
    
    // ====================================================================
    // منشئات ثابتة — أنواع أساسية (مُعرَّفة هنا مباشرة)
    // Static Constructors — Primitive Types (defined inline here)
    // ====================================================================
    
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
    
    static Value Pointer(void* ptr) {
        Value v;
        v.type = ValueType::VAL_POINTER;
        v.as.pointer = ptr;
        return v;
    }
    
    // ====================================================================
    // منشئات ثابتة — أنواع الكائنات (مُصرَّح عنها فقط — مُعرَّفة لاحقاً)
    // Static Constructors — Object Types (declared only — defined below)
    // (AR) هذه الدوال تحتاج أن تعرف أن الأنواع المشتقة ترث من Object
    //      لذا تُعرَّف بعد تعريف جميع الأنواع المشتقة.
    // (EN) These methods need to know derived types inherit from Object,
    //      so they are defined after all derived types are defined.
    // ====================================================================
    
    static Value String(StringObject* str);
    static Value Array(ArrayObject* arr);
    static Value Obj(MapObject* obj);      ///< (AR) أُعيد تسميته من Object() / (EN) Renamed from Object()
    static Value Function(FunctionObject* fn);
    static Value Closure(ClosureObject* closure);
    static Value Native(NativeObject* native);
    
    // ====================================================================
    // فحوصات النوع / Type Checks
    // (AR) لا تعتمد على الأنواع المشتقة — آمنة للتعريف المباشر
    // (EN) Don't depend on derived types — safe to define inline
    // ====================================================================
    
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
    
    // ====================================================================
    // تحويلات أساسية / Primitive Conversions
    // (AR) هذه آمنة — تتعامل مع union مباشرة بدون أنواع مشتقة
    // (EN) These are safe — work directly with union, no derived types
    // ====================================================================
    
    bool asBool() const { return as.boolean; }
    int64_t asInt() const { return as.integer; }
    double asFloat() const { return as.floating; }
    Object* asObject() const { return as.object; }
    void* asPointer() const { return as.pointer; }
    
    // ====================================================================
    // تحويلات الكائنات (مُصرَّح عنها — مُعرَّفة لاحقاً)
    // Object Conversions (declared — defined below)
    // (AR) تحتاج static_cast من Object* إلى النوع المشتق
    //      مما يتطلب معرفة علاقة الوراثة (تعريف كامل).
    // (EN) Need static_cast from Object* to derived type,
    //      which requires knowing inheritance relationship (full definition).
    // ====================================================================
    
    StringObject* asString() const;
    ArrayObject* asArray() const;
    MapObject* asMap() const;
    FunctionObject* asFunction() const;
    ClosureObject* asClosure() const;
    NativeObject* asNative() const;
    
    // ====================================================================
    // أدوات مساعدة / Utilities
    // ====================================================================
    
    /**
     * @brief تحويل إلى نص / Convert to string
     * (AR) يُنفَّذ في value.cpp لأنه يعتمد على StringObject::toString()
     * (EN) Implemented in value.cpp because it depends on StringObject::toString()
     */
    std::string toString() const;
    
    /**
     * @brief تحويل إلى قيمة منطقية / Convert to boolean
     * (AR) القيم الأساسية: null=false, 0=false, 0.0=false. الكائنات: true دائماً.
     * (EN) Primitives: null=false, 0=false, 0.0=false. Objects: always true.
     */
    bool toBool() const {
        switch (type) {
            case ValueType::VAL_NULL: return false;
            case ValueType::VAL_BOOL: return as.boolean;
            case ValueType::VAL_INT: return as.integer != 0;
            case ValueType::VAL_FLOAT: return as.floating != 0.0;
            default: return true; // Objects are truthy / الكائنات دائماً صادقة
        }
    }
    
    /**
     * @brief تحويل إلى عدد / Convert to number
     * (AR) يحوّل أي قيمة إلى عدد عشري. الأنواع غير العددية تُرجع 0.
     * (EN) Converts any value to double. Non-numeric types return 0.
     */
    double toNumber() const {
        if (isInt()) return static_cast<double>(as.integer);
        if (isFloat()) return as.floating;
        if (isBool()) return as.boolean ? 1.0 : 0.0;
        return 0.0;
    }
    
    /**
     * @brief المساواة / Equality
     * (AR) يُنفَّذ في value.cpp لأنه يحتاج مقارنة الكائنات.
     * (EN) Implemented in value.cpp because it needs object comparison.
     */
    bool equals(const Value& other) const;
};

// ============================================================================
// القسم 4: أنواع الكائنات المشتقة / Section 4: Derived Object Types
// (AR) الآن بعد تعريف Value كاملاً، يمكن للأنواع المشتقة استخدامه
//      كعضو (وليس فقط كمؤشر). مثال: ArrayObject::elements هو vector<Value>.
// (EN) Now that Value is fully defined, derived types can use it
//      as a member (not just pointer). E.g., ArrayObject::elements is vector<Value>.
// ============================================================================

// ========================================
// String Object — كائن النص
// ========================================

/**
 * @brief كائن نص / String Object
 * @brief Immutable string object
 * 
 * (AR) يمثل نصاً غير قابل للتعديل. يُخزَّن في الذاكرة كمصفوفة char
 *      مع حقل hash للبحث السريع في الخرائط.
 * (EN) Represents an immutable string. Stored in memory as char array
 *      with hash field for fast lookup in maps.
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
// Array Object — كائن المصفوفة
// ========================================

/**
 * @brief كائن مصفوفة / Array Object
 * @brief Dynamic array of values
 * 
 * (AR) مصفوفة ديناميكية من القيم. تستخدم std::vector<Value> داخلياً
 *      مما يوفر إدارة الذاكرة تلقائياً وعمليات push/pop فعالة.
 * (EN) Dynamic array of values. Uses std::vector<Value> internally,
 *      providing automatic memory management and efficient push/pop ops.
 */
struct ArrayObject : public Object {
    std::vector<Value> elements;  ///< العناصر / Elements
    
    ArrayObject() : Object(ObjectType::OBJ_ARRAY) {}
    
    size_t length() const {
        return elements.size();
    }
};

// ========================================
// Map Object — كائن الخريطة
// ========================================

/**
 * @brief كائن خريطة / Map Object
 * @brief Hash map for object properties
 * 
 * (AR) خريطة hash لتخزين خصائص الكائنات. المفاتيح نصية،
 *      القيم من نوع Value (أي نوع).
 * (EN) Hash map for object properties. Keys are strings,
 *      values are Value (any type).
 */
struct MapObject : public Object {
    std::unordered_map<std::string, Value> fields;  ///< الحقول / Fields
    
    MapObject() : Object(ObjectType::OBJ_MAP) {}
};

// ========================================
// Function Object — كائن الدالة
// ========================================

/**
 * @brief كائن دالة / Function Object
 * @brief Compiled function
 * 
 * (AR) يمثل دالة مترجمة إلى بايت كود. يحتوي على:
 *      - name: اسم الدالة (للتنقيح)
 *      - arity: عدد المعاملات المتوقعة
 *      - upvalueCount: عدد القيم العلوية (للإغلاقات)
 *      - code: البايت كود المترجم
 *      - lines: أرقام الأسطر المقابلة (للتنقيح)
 * (EN) Represents a function compiled to bytecode. Contains:
 *      - name: function name (for debugging)
 *      - arity: expected parameter count
 *      - upvalueCount: upvalue count (for closures)
 *      - code: compiled bytecode
 *      - lines: corresponding line numbers (for debugging)
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
// Native Function Object — كائن الدالة الأصلية
// ========================================

/**
 * @brief كائن دالة أصلية / Native Function Object
 * @brief C++ function callable from Sad
 * 
 * (AR) يمثل دالة C++ مُسجَّلة في الآلة الافتراضية يمكن استدعاؤها
 *      من كود لغة ص. تُستخدم لتوفير الدوال المدمجة.
 * (EN) Represents a C++ function registered in the VM that can be called
 *      from Sad code. Used to provide built-in functions.
 */
struct NativeObject : public Object {
    NativeFn function;         ///< مؤشر الدالة / Function pointer
    std::string name;          ///< الاسم / Name
    
    NativeObject(NativeFn fn, const std::string& n)
        : Object(ObjectType::OBJ_NATIVE), function(fn), name(n) {}
};

// ========================================
// Upvalue Object — كائن القيمة العلوية
// ========================================

/**
 * @brief قيمة علوية / Upvalue
 * @brief Captured variable for closures
 * 
 * (AR) يمثل متغيراً محتجزاً بواسطة إغلاق (closure).
 *      عندما يكون المتغير على المكدس، location يشير إليه.
 *      عندما يخرج المتغير من النطاق، القيمة تُنسخ إلى closed
 *      ويُعاد توجيه location ليشير إلى closed.
 * (EN) Represents a variable captured by a closure.
 *      While the variable is on the stack, location points to it.
 *      When the variable goes out of scope, the value is copied to closed
 *      and location is redirected to point to closed.
 */
struct UpvalueObject : public Object {
    Value* location;           ///< الموقع في المكدس / Location on stack
    Value closed;              ///< القيمة المغلقة / Closed value (OK — Value is fully defined above)
    UpvalueObject* next;       ///< السلسلة / Linked list
    
    UpvalueObject(Value* loc)
        : Object(ObjectType::OBJ_UPVALUE), location(loc), next(nullptr) {}
};

// ========================================
// Closure Object — كائن الإغلاق
// ========================================

/**
 * @brief إغلاق / Closure
 * @brief Function with captured variables
 * 
 * (AR) يجمع بين دالة وقيمها العلوية المحتجزة.
 *      كل استدعاء للدالة التي تحتجز متغيرات يُنشئ إغلاقاً جديداً.
 * (EN) Combines a function with its captured upvalues.
 *      Each call to a function that captures variables creates a new closure.
 */
struct ClosureObject : public Object {
    FunctionObject* function;              ///< الدالة / Function
    std::vector<UpvalueObject*> upvalues;  ///< القيم المحتجزة / Captured values
    
    ClosureObject(FunctionObject* fn)
        : Object(ObjectType::OBJ_CLOSURE), function(fn) {
        upvalues.resize(fn->upvalueCount, nullptr);
    }
};

// ============================================================================
// القسم 5: التعريفات المؤجلة لدوال Value
// Section 5: Deferred Inline Definitions for Value Methods
// (AR) الآن بعد تعريف جميع الأنواع المشتقة، يمكننا تعريف الدوال
//      التي تحتاج static_cast أو تحويل مؤشرات مشتقة → أساسية.
// (EN) Now that all derived types are defined, we can define the methods
//      that need static_cast or derived-to-base pointer conversions.
// ============================================================================

// ====================================================================
// منشئات ثابتة — أنواع الكائنات / Static Constructors — Object Types
// ====================================================================

inline Value Value::String(StringObject* str) {
    Value v;
    v.type = ValueType::VAL_STRING;
    v.as.object = str;
    return v;
}

inline Value Value::Array(ArrayObject* arr) {
    Value v;
    v.type = ValueType::VAL_ARRAY;
    v.as.object = arr;
    return v;
}

/**
 * @brief إنشاء قيمة كائن / Create object value
 * (AR) أُعيد تسميتها من Object() إلى Obj() لتجنب التعارض مع struct Object.
 * (EN) Renamed from Object() to Obj() to avoid collision with struct Object.
 */
inline Value Value::Obj(MapObject* obj) {
    Value v;
    v.type = ValueType::VAL_OBJECT;
    v.as.object = obj;
    return v;
}

inline Value Value::Function(FunctionObject* fn) {
    Value v;
    v.type = ValueType::VAL_FUNCTION;
    v.as.object = fn;
    return v;
}

inline Value Value::Closure(ClosureObject* closure) {
    Value v;
    v.type = ValueType::VAL_CLOSURE;
    v.as.object = closure;
    return v;
}

inline Value Value::Native(NativeObject* native) {
    Value v;
    v.type = ValueType::VAL_NATIVE;
    v.as.object = native;
    return v;
}

// ====================================================================
// تحويلات الكائنات / Object Conversions
// ====================================================================

inline StringObject* Value::asString() const { 
    return static_cast<StringObject*>(as.object); 
}

inline ArrayObject* Value::asArray() const { 
    return static_cast<ArrayObject*>(as.object); 
}

inline MapObject* Value::asMap() const { 
    return static_cast<MapObject*>(as.object); 
}

inline FunctionObject* Value::asFunction() const { 
    return static_cast<FunctionObject*>(as.object); 
}

inline ClosureObject* Value::asClosure() const { 
    return static_cast<ClosureObject*>(as.object); 
}

inline NativeObject* Value::asNative() const { 
    return static_cast<NativeObject*>(as.object); 
}

// ============================================================================
// القسم 6: الأدوات والمساعدات / Section 6: Utilities and Helpers
// ============================================================================

/**
 * @brief الحصول على اسم النوع / Get type name
 * (AR) يُرجع اسم النوع كنص (مفيد للتنقيح والرسائل).
 * (EN) Returns type name as string (useful for debugging and messages).
 */
const char* getValueTypeName(ValueType type);

/**
 * @brief طباعة قيمة / Print value
 * (AR) تطبع القيمة على stdout بالتنسيق المناسب.
 * (EN) Prints the value to stdout in appropriate format.
 */
void printValue(const Value& value);

/**
 * @brief طباعة قيمة بالعربية / Print value in Arabic
 * (AR) تطبع القيمة مع أسماء الأنواع بالعربية.
 * (EN) Prints value with Arabic type names.
 */
void printValueArabic(const Value& value);

// ============================================================================
// دوال مساعدة — سد الفجوة مع الكود القديم
// Helper functions — bridge gap with legacy code
// ============================================================================

/**
 * @brief مقارنة قيمتين بالتساوي / Compare two values for equality
 * (AR) دالة مساعدة يستخدمها vm_opcodes.cpp بدلاً من Value::equals()
 *      الكود القديم يستدعي valuesEqual(a, b) كدالة حرة.
 * (EN) Helper function used by vm_opcodes.cpp instead of Value::equals()
 *      Legacy code calls valuesEqual(a, b) as a free function.
 */
inline bool valuesEqual(const Value& a, const Value& b) {
    return a.equals(b);
}

/**
 * @brief تحويل نوع الكائن إلى نص / Convert object type to string
 * (AR) يستخدمها vm_gc.cpp لطباعة معلومات التنقيح عند تخصيص الكائنات.
 *      الكود القديم يستدعي objectTypeToString(type) كدالة حرة.
 * (EN) Used by vm_gc.cpp for debug printing when allocating objects.
 *      Legacy code calls objectTypeToString(type) as a free function.
 */
inline const char* objectTypeToString(ObjectType type) {
    switch (type) {
        case ObjectType::OBJ_STRING:   return "String/نص";
        case ObjectType::OBJ_ARRAY:    return "Array/مصفوفة";
        case ObjectType::OBJ_MAP:      return "Map/خريطة";
        case ObjectType::OBJ_FUNCTION: return "Function/دالة";
        case ObjectType::OBJ_CLOSURE:  return "Closure/إغلاق";
        case ObjectType::OBJ_NATIVE:   return "Native/أصلية";
        case ObjectType::OBJ_UPVALUE:  return "Upvalue/قيمة_علوية";
        case ObjectType::OBJ_OBJECT:   return "Object/كائن";
        default:                       return "Unknown/مجهول";
    }
}

} // namespace VM
} // namespace Sad
