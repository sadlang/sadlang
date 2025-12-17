// ======================================================================
// sir_types.h - أنواع التمثيل الوسيط البسيط (SIR)
//              Simple Intermediate Representation Types
// ======================================================================
// الوصف بالعربية:
//   تعريف أنواع البيانات الأساسية في SIR:
//   - SIRType: أنواع البيانات (i64, f64, bool, ptr, etc.)
//   - SIROpcode: رموز العمليات (90 opcode)
//   - SIROperand: المعاملات (registers, constants, labels)
//
// English Description:
//   Definition of basic data types in SIR:
//   - SIRType: Data types (i64, f64, bool, ptr, etc.)
//   - SIROpcode: Operation codes (90 opcodes)
//   - SIROperand: Operands (registers, constants, labels)
//
// الإصدار / Version: 1.0
// التاريخ / Date: December 2, 2025
// المرحلة / Phase: 1 - Frontend
// ======================================================================

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

namespace Sad {
namespace Compiler {
namespace SIR {

// ======================================================================
// أنواع البيانات / Data Types
// ======================================================================

/**
 * @brief (AR) أنواع البيانات في SIR
 * @brief (EN) Data types in SIR
 * 
 * يدعم SIR 9 أنواع بيانات أساسية:
 * SIR supports 9 basic data types:
 * 
 * - VOID: فارغ / void (for functions with no return)
 * - I64: رقم صحيح 64-bit / 64-bit integer
 * - F64: رقم عشري 64-bit / 64-bit floating point
 * - BOOL: منطقي / boolean (true/false)
 * - PTR: مؤشر / pointer
 * - ARRAY: مصفوفة / array
 * - STRING: نص / string
 * - STRUCT: بنية/كائن / struct/object
 * - FUNCTION: دالة / function pointer
 */
enum class SIRType {
    VOID,       ///< فارغ / void
    I64,        ///< رقم صحيح 64-bit / 64-bit integer
    F64,        ///< رقم عشري 64-bit / 64-bit floating point
    BOOL,       ///< منطقي / boolean
    PTR,        ///< مؤشر / pointer
    ARRAY,      ///< مصفوفة / array
    STRING,     ///< نص / string
    STRUCT,     ///< بنية/كائن / struct/object
    FUNCTION    ///< دالة / function pointer
};

/**
 * @brief (AR) تحويل SIRType إلى نص
 * @brief (EN) Convert SIRType to string
 */
inline const char* sirTypeToString(SIRType type) {
    switch (type) {
        case SIRType::VOID:     return "void";
        case SIRType::I64:      return "i64";
        case SIRType::F64:      return "f64";
        case SIRType::BOOL:     return "bool";
        case SIRType::PTR:      return "ptr";
        case SIRType::ARRAY:    return "array";
        case SIRType::STRING:   return "string";
        case SIRType::STRUCT:   return "struct";
        case SIRType::FUNCTION: return "function";
        default:                return "unknown";
    }
}

// ======================================================================
// رموز العمليات / Operation Codes (90 opcodes)
// ======================================================================

/**
 * @brief (AR) رموز عمليات SIR (90 opcode)
 * @brief (EN) SIR operation codes (90 opcodes)
 * 
 * مُصنَّفة في 10 فئات:
 * Classified into 10 categories:
 * 
 * 1. Arithmetic (10): add, sub, mul, div, mod, neg
 * 2. Bitwise (8): and, or, xor, not, shl, shr, sar, rol
 * 3. Comparison (6): eq, ne, lt, le, gt, ge
 * 4. Control Flow (8): br, ret, call, switch, phi
 * 5. Memory (10): alloc, load, store, addr, memcpy, memset
 * 6. Array (6): array_new, array_get, array_set, array_len
 * 7. String (8): string_new, string_len, string_concat
 * 8. OOP (10): object_new, object_get, object_set, object_call
 * 9. Type Conversion (8): i64_to_f64, f64_to_i64, cast
 * 10. Builtin (15): print, read, sqrt, pow, sin, cos
 */
enum class SIROpcode {
    // ==========================================
    // 1. العمليات الحسابية / Arithmetic (10)
    // ==========================================
    ADD_I64,    ///< إضافة أعداد صحيحة / Integer addition
    ADD_F64,    ///< إضافة أعداد عشرية / Float addition
    SUB_I64,    ///< طرح أعداد صحيحة / Integer subtraction
    SUB_F64,    ///< طرح أعداد عشرية / Float subtraction
    MUL_I64,    ///< ضرب أعداد صحيحة / Integer multiplication
    MUL_F64,    ///< ضرب أعداد عشرية / Float multiplication
    DIV_I64,    ///< قسمة أعداد صحيحة / Integer division
    DIV_F64,    ///< قسمة أعداد عشرية / Float division
    MOD_I64,    ///< باقي القسمة / Modulo
    NEG,        ///< السالب / Negation
    
    // ==========================================
    // 2. العمليات الثنائية / Bitwise (8)
    // ==========================================
    AND,        ///< AND الثنائي / Bitwise AND
    OR,         ///< OR الثنائي / Bitwise OR
    XOR,        ///< XOR الثنائي / Bitwise XOR
    NOT,        ///< NOT الثنائي / Bitwise NOT
    SHL,        ///< إزاحة لليسار / Shift left
    SHR,        ///< إزاحة لليمين (منطقية) / Shift right (logical)
    SAR,        ///< إزاحة لليمين (حسابية) / Shift right (arithmetic)
    ROL,        ///< دوران لليسار / Rotate left
    
    // ==========================================
    // 3. المقارنات / Comparison (6)
    // ==========================================
    EQ,         ///< يساوي / Equal
    NE,         ///< لا يساوي / Not equal
    LT,         ///< أصغر من / Less than
    LE,         ///< أصغر أو يساوي / Less or equal
    GT,         ///< أكبر من / Greater than
    GE,         ///< أكبر أو يساوي / Greater or equal
    
    // ==========================================
    // 4. التحكم في التدفق / Control Flow (8)
    // ==========================================
    BR,         ///< قفز غير مشروط / Unconditional branch
    BR_COND,    ///< قفز مشروط / Conditional branch
    RET,        ///< إرجاع / Return
    RET_VOID,   ///< إرجاع فارغ / Return void
    CALL,       ///< استدعاء دالة / Function call
    CALL_INDIRECT, ///< استدعاء غير مباشر / Indirect call
    SWITCH,     ///< قفز محسوب / Switch statement
    PHI,        ///< عقدة PHI لـ SSA / PHI node for SSA
    
    // ==========================================
    // 5. الذاكرة والمؤشرات / Memory (10)
    // ==========================================
    ALLOC,      ///< تخصيص على Stack / Stack allocation
    ALLOC_HEAP, ///< تخصيص على Heap / Heap allocation
    FREE,       ///< تحرير الذاكرة / Free memory
    LOAD,       ///< تحميل من الذاكرة / Load from memory
    STORE,      ///< تخزين في الذاكرة / Store to memory
    ADDR,       ///< الحصول على العنوان / Get address
    PTR_ADD,    ///< إضافة مؤشر / Pointer arithmetic
    PTR_CAST,   ///< تحويل مؤشر / Pointer cast
    MEMCPY,     ///< نسخ الذاكرة / Memory copy
    MEMSET,     ///< تعيين الذاكرة / Memory set
    
    // ==========================================
    // 6. المصفوفات / Arrays (6)
    // ==========================================
    ARRAY_NEW,  ///< إنشاء مصفوفة / Create array
    ARRAY_GET,  ///< الحصول على عنصر / Get element
    ARRAY_SET,  ///< تعديل عنصر / Set element
    ARRAY_LEN,  ///< طول المصفوفة / Array length
    ARRAY_APPEND, ///< إضافة عنصر / Append element
    ARRAY_REMOVE, ///< حذف عنصر / Remove element
    
    // ==========================================
    // 7. النصوص / Strings (8)
    // ==========================================
    STRING_NEW, ///< إنشاء نص / Create string
    STRING_LEN, ///< طول النص / String length
    STRING_CONCAT, ///< دمج نصوص / Concatenate strings
    STRING_CMP, ///< مقارنة نصوص / Compare strings
    STRING_SUBSTR, ///< استخراج جزء / Substring
    STRING_FIND, ///< البحث / Find substring
    STRING_REPLACE, ///< الاستبدال / Replace
    STRING_TO_I64, ///< تحويل نص لرقم / String to integer
    
    // ==========================================
    // 8. البرمجة الكائنية / OOP (10)
    // ==========================================
    OBJECT_NEW, ///< إنشاء كائن / Create object
    OBJECT_GET, ///< الحصول على خاصية / Get property
    OBJECT_SET, ///< تعديل خاصية / Set property
    OBJECT_CALL, ///< استدعاء دالة / Call method
    INSTANCEOF, ///< تحقق من النوع / Type check
    OBJECT_CAST, ///< تحويل كائن / Object cast
    CLASS_DEF,  ///< تعريف صنف / Define class
    METHOD_DEF, ///< تعريف دالة / Define method
    FIELD_DEF,  ///< تعريف خاصية / Define field
    CONSTRUCTOR_CALL, ///< استدعاء منشئ / Constructor call
    
    // ==========================================
    // 9. تحويل الأنواع / Type Conversion (8)
    // ==========================================
    I64_TO_F64, ///< رقم صحيح → عشري / Integer to float
    F64_TO_I64, ///< رقم عشري → صحيح / Float to integer
    I64_TO_BOOL, ///< رقم → منطقي / Integer to boolean
    BOOL_TO_I64, ///< منطقي → رقم / Boolean to integer
    I64_TO_STRING, ///< رقم → نص / Integer to string
    F64_TO_STRING, ///< عشري → نص / Float to string
    BOOL_TO_STRING, ///< منطقي → نص / Boolean to string
    CAST,       ///< تحويل عام / General cast
    
    // ==========================================
    // 10. الدوال المضمنة / Builtin (15)
    // ==========================================
    BUILTIN_PRINT,  ///< اطبع / Print
    BUILTIN_READ,   ///< اقرأ / Read
    BUILTIN_SQRT,   ///< جذر / Square root
    BUILTIN_POW,    ///< قوة / Power
    BUILTIN_SIN,    ///< جا / Sine
    BUILTIN_COS,    ///< جتا / Cosine
    BUILTIN_TAN,    ///< ظا / Tangent
    BUILTIN_ABS,    ///< القيمة المطلقة / Absolute value
    BUILTIN_FLOOR,  ///< أرضية / Floor
    BUILTIN_CEIL,   ///< سقف / Ceiling
    BUILTIN_ROUND,  ///< تقريب / Round
    BUILTIN_MIN,    ///< الأصغر / Minimum
    BUILTIN_MAX,    ///< الأكبر / Maximum
    BUILTIN_ASSERT, ///< تأكيد / Assert
    BUILTIN_DEBUG   ///< تنقيح / Debug print
};

/**
 * @brief (AR) تحويل SIROpcode إلى نص
 * @brief (EN) Convert SIROpcode to string
 */
const char* sirOpcodeToString(SIROpcode opcode);

/**
 * @brief (AR) الحصول على عدد المعاملات لكل opcode
 * @brief (EN) Get number of operands for each opcode
 */
int sirOpcodeOperandCount(SIROpcode opcode);

// ======================================================================
// المعاملات / Operands
// ======================================================================

/**
 * @brief (AR) نوع المعامل
 * @brief (EN) Operand type
 */
enum class SIROperandType {
    REGISTER,   ///< سجل افتراضي / Virtual register (%0, %1, %name)
    CONSTANT,   ///< ثابت / Constant (42, 3.14, "hello")
    LABEL,      ///< تسمية / Label (for branches)
    GLOBAL,     ///< متغير عام / Global variable ($name)
    FUNCTION    ///< اسم دالة / Function name (@name)
};

/**
 * @brief (AR) معامل في تعليمة SIR
 * @brief (EN) Operand in SIR instruction
 * 
 * يمكن أن يكون المعامل:
 * An operand can be:
 * - سجل افتراضي: %0, %1, %variableName
 * - ثابت: 42, 3.14, true, "hello"
 * - تسمية: label_name (for jumps)
 * - متغير عام: $globalVar
 * - دالة: @functionName
 */
class SIROperand {
public:
    SIROperandType type;    ///< نوع المعامل / Operand type
    SIRType dataType;       ///< نوع البيانات / Data type
    
    // القيمة / Value (based on type)
    union {
        int64_t intValue;   ///< قيمة صحيحة / Integer value
        double floatValue;  ///< قيمة عشرية / Float value
        bool boolValue;     ///< قيمة منطقية / Boolean value
    };
    
    std::string name;       ///< اسم (للسجلات، التسميات، الدوال) / Name
    
    // ==========================================
    // Constructors
    // ==========================================
    
    /**
     * @brief (AR) منشئ افتراضي
     * @brief (EN) Default constructor
     */
    SIROperand() : type(SIROperandType::REGISTER), dataType(SIRType::VOID), intValue(0) {}
    
    /**
     * @brief (AR) إنشاء سجل افتراضي
     * @brief (EN) Create virtual register
     */
    static SIROperand Register(const std::string& name, SIRType type) {
        SIROperand op;
        op.type = SIROperandType::REGISTER;
        op.dataType = type;
        op.name = name;
        return op;
    }
    
    /**
     * @brief (AR) إنشاء ثابت رقمي صحيح
     * @brief (EN) Create integer constant
     */
    static SIROperand ConstantI64(int64_t value) {
        SIROperand op;
        op.type = SIROperandType::CONSTANT;
        op.dataType = SIRType::I64;
        op.intValue = value;
        return op;
    }
    
    /**
     * @brief (AR) إنشاء ثابت رقمي عشري
     * @brief (EN) Create float constant
     */
    static SIROperand ConstantF64(double value) {
        SIROperand op;
        op.type = SIROperandType::CONSTANT;
        op.dataType = SIRType::F64;
        op.floatValue = value;
        return op;
    }
    
    /**
     * @brief (AR) إنشاء ثابت منطقي
     * @brief (EN) Create boolean constant
     */
    static SIROperand ConstantBool(bool value) {
        SIROperand op;
        op.type = SIROperandType::CONSTANT;
        op.dataType = SIRType::BOOL;
        op.boolValue = value;
        return op;
    }
    
    /**
     * @brief (AR) إنشاء ثابت نصي
     * @brief (EN) Create string constant
     */
    static SIROperand ConstantString(const std::string& value) {
        SIROperand op;
        op.type = SIROperandType::CONSTANT;
        op.dataType = SIRType::STRING;
        op.name = value;
        return op;
    }
    
    /**
     * @brief (AR) إنشاء تسمية
     * @brief (EN) Create label
     */
    static SIROperand Label(const std::string& name) {
        SIROperand op;
        op.type = SIROperandType::LABEL;
        op.dataType = SIRType::VOID;
        op.name = name;
        return op;
    }
    
    /**
     * @brief (AR) إنشاء متغير عام
     * @brief (EN) Create global variable
     */
    static SIROperand Global(const std::string& name, SIRType type) {
        SIROperand op;
        op.type = SIROperandType::GLOBAL;
        op.dataType = type;
        op.name = name;
        return op;
    }
    
    /**
     * @brief (AR) إنشاء دالة
     * @brief (EN) Create function reference
     */
    static SIROperand Function(const std::string& name) {
        SIROperand op;
        op.type = SIROperandType::FUNCTION;
        op.dataType = SIRType::FUNCTION;
        op.name = name;
        return op;
    }
    
    /**
     * @brief (AR) تحويل المعامل إلى نص
     * @brief (EN) Convert operand to string
     */
    std::string toString() const;
};

} // namespace SIR
} // namespace Compiler
} // namespace Sad
