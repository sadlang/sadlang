/**
 * @file opcodes.h
 * @brief أكواد تعليمات البايت كود / Bytecode Instruction Opcodes
 * @brief Bytecode Instruction Set for Sad Language Virtual Machine
 * 
 * @details
 * (AR) يحدد هذا الملف مجموعة التعليمات الكاملة للآلة الافتراضية.
 *      الآلة الافتراضية تستخدم معمارية Stack-based مع register hints
 *      للأداء الأمثل. التعليمات بطول متغير (1-25 بايت).
 * 
 * (EN) This file defines the complete instruction set for the virtual machine.
 *      The VM uses a stack-based architecture with register hints for
 *      optimal performance. Instructions are variable length (1-25 bytes).
 * 
 * @architecture Stack-Based VM with Register Hints
 * @instruction_format Variable length (1-25 bytes)
 * @opcode_space 0x00 - 0xFF (256 opcodes)
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 * @phase Phase 3: Bytecode Backend
 */

#pragma once

#include <cstdint>
#include <string>

namespace Sad {
namespace VM {

/**
 * @brief نوع البايت / Byte Type
 * @brief Basic byte type for bytecode
 */
using Byte = uint8_t;

/**
 * @brief نوع رمز العملية / Opcode Type
 * @brief Opcode enumeration type
 */
using Opcode = uint8_t;

/**
 * @brief أكواد العمليات / Operation Codes
 * @brief Complete opcode definitions (0x00 - 0xFF)
 * 
 * @details
 * (AR) التعليمات منظمة في مجموعات حسب الوظيفة:
 *      - 0x00-0x0F: عمليات المكدس (Stack Operations)
 *      - 0x10-0x1F: العمليات الحسابية (Arithmetic)
 *      - 0x20-0x2F: العمليات البتية (Bitwise)
 *      - 0x30-0x3F: المقارنات (Comparison)
 *      - 0x40-0x4F: التحكم في التدفق (Control Flow)
 *      - 0x50-0x5F: المتغيرات (Variables)
 *      - 0x60-0x6F: الذاكرة (Memory)
 *      - 0x70-0x7F: المصفوفات (Arrays)
 *      - 0x80-0x8F: الكائنات (Objects)
 *      - 0x90-0x9F: الدوال المدمجة (Builtins)
 *      - 0xA0-0xAF: النوع والتحويل (Type & Cast)
 *      - 0xB0-0xBF: التوافق مع المرحلة 1 (Stage 1 Compat)
 *      - 0xF0-0xFF: تعليمات خاصة (Special)
 * 
 * (EN) Instructions organized by functional groups
 */
enum OpcodeValues : Opcode {
    // ========================================
    // Stack Operations (0x00-0x0F)
    // عمليات المكدس
    // ========================================
    
    OP_NOP          = 0x00,  ///< No operation / لا عملية
    OP_PUSH_I8      = 0x01,  ///< Push 8-bit integer / دفع عدد صحيح 8 بت
    OP_PUSH_I16     = 0x02,  ///< Push 16-bit integer / دفع عدد صحيح 16 بت
    OP_PUSH_I32     = 0x03,  ///< Push 32-bit integer / دفع عدد صحيح 32 بت
    OP_PUSH_I64     = 0x04,  ///< Push 64-bit integer / دفع عدد صحيح 64 بت
    OP_PUSH_F64     = 0x05,  ///< Push 64-bit float / دفع عدد عشري 64 بت
    OP_PUSH_STR     = 0x06,  ///< Push string (from pool) / دفع نص من المجمع
    OP_PUSH_NULL    = 0x07,  ///< Push null / دفع null
    OP_PUSH_TRUE    = 0x08,  ///< Push true / دفع صحيح
    OP_PUSH_FALSE   = 0x09,  ///< Push false / دفع خطأ
    OP_POP          = 0x0A,  ///< Pop and discard / إزالة وتجاهل
    OP_DUP          = 0x0B,  ///< Duplicate top / نسخ القمة
    OP_DUP2         = 0x0C,  ///< Duplicate top 2 / نسخ القمتين
    OP_SWAP         = 0x0D,  ///< Swap top two / تبديل القمتين
    OP_ROT          = 0x0E,  ///< Rotate top 3 / تدوير القمة 3
    OP_OVER         = 0x0F,  ///< Copy 2nd to top / نسخ الثاني للقمة
    
    // ========================================
    // Arithmetic Operations (0x10-0x1F)
    // العمليات الحسابية
    // ========================================
    
    OP_ADD          = 0x10,  ///< Add: a + b / جمع
    OP_SUB          = 0x11,  ///< Subtract: a - b / طرح
    OP_MUL          = 0x12,  ///< Multiply: a * b / ضرب
    OP_DIV          = 0x13,  ///< Divide: a / b / قسمة
    OP_MOD          = 0x14,  ///< Modulo: a % b / باقي القسمة
    OP_NEG          = 0x15,  ///< Negate: -a / نفي
    OP_ABS          = 0x16,  ///< Absolute: |a| / القيمة المطلقة
    OP_POW          = 0x17,  ///< Power: a ** b / أس
    OP_SQRT         = 0x18,  ///< Square root / جذر تربيعي
    OP_INC          = 0x19,  ///< Increment: a + 1 / زيادة
    OP_DEC          = 0x1A,  ///< Decrement: a - 1 / نقصان
    
    // ========================================
    // Bitwise Operations (0x20-0x2F)
    // العمليات البتية
    // ========================================
    
    OP_AND          = 0x20,  ///< Bitwise AND: a & b / و البتية
    OP_OR           = 0x21,  ///< Bitwise OR: a | b / أو البتية
    OP_XOR          = 0x22,  ///< Bitwise XOR: a ^ b / أو الخاصة البتية
    OP_NOT          = 0x23,  ///< Bitwise NOT: ~a / نفي بتي
    OP_SHL          = 0x24,  ///< Shift left: a << b / إزاحة يسار
    OP_SHR          = 0x25,  ///< Shift right: a >> b / إزاحة يمين
    OP_SAR          = 0x26,  ///< Arithmetic shift right / إزاحة حسابية يمين
    
    // ========================================
    // Comparison Operations (0x30-0x3F)
    // عمليات المقارنة
    // ========================================
    
    OP_EQ           = 0x30,  ///< Equal: a == b / مساواة
    OP_NE           = 0x31,  ///< Not equal: a != b / عدم مساواة
    OP_LT           = 0x32,  ///< Less than: a < b / أصغر من
    OP_LE           = 0x33,  ///< Less or equal: a <= b / أصغر أو يساوي
    OP_GT           = 0x34,  ///< Greater than: a > b / أكبر من
    OP_GE           = 0x35,  ///< Greater or equal: a >= b / أكبر أو يساوي
    
    // ========================================
    // Control Flow (0x40-0x4F)
    // التحكم في التدفق
    // ========================================
    
    OP_JMP          = 0x40,  ///< Unconditional jump / قفز غير مشروط
    OP_JMP_IF       = 0x41,  ///< Jump if true / قفز إذا صحيح
    OP_JMP_NOT      = 0x42,  ///< Jump if false / قفز إذا خطأ
    OP_CALL         = 0x43,  ///< Call function / استدعاء دالة
    OP_CALL_NATIVE  = 0x44,  ///< Call native function / استدعاء دالة أصلية
    OP_RET          = 0x45,  ///< Return (void) / عودة بدون قيمة
    OP_RET_VAL      = 0x46,  ///< Return with value / عودة بقيمة
    OP_LOOP         = 0x47,  ///< Loop back / رجوع للحلقة
    
    // ========================================
    // Variables (0x50-0x5F)
    // المتغيرات
    // ========================================
    
    OP_LOAD_LOCAL   = 0x50,  ///< Load local variable / تحميل متغير محلي
    OP_STORE_LOCAL  = 0x51,  ///< Store local variable / تخزين متغير محلي
    OP_LOAD_GLOBAL  = 0x52,  ///< Load global variable / تحميل متغير عام
    OP_STORE_GLOBAL = 0x53,  ///< Store global variable / تخزين متغير عام
    OP_LOAD_CAPTURE = 0x54,  ///< Load captured variable / تحميل متغير محتجز
    OP_STORE_CAPTURE= 0x55,  ///< Store captured variable / تخزين متغير محتجز
    OP_LOAD_UPVALUE = 0x56,  ///< Load upvalue / تحميل قيمة علوية
    OP_CLOSE_UPVALUE= 0x57,  ///< Close upvalue / إغلاق قيمة علوية
    
    // ========================================
    // Memory Operations (0x60-0x6F)
    // عمليات الذاكرة
    // ========================================
    
    OP_ALLOC        = 0x60,  ///< Allocate memory / تخصيص ذاكرة
    OP_FREE         = 0x61,  ///< Free memory / تحرير ذاكرة
    OP_LOAD_PTR     = 0x62,  ///< Load from pointer / تحميل من مؤشر
    OP_STORE_PTR    = 0x63,  ///< Store to pointer / تخزين إلى مؤشر
    OP_ADDR_OF      = 0x64,  ///< Get address of variable / الحصول على عنوان
    OP_DEREF        = 0x65,  ///< Dereference pointer / فك المؤشر
    OP_WRITE_TO     = 0x66,  ///< Write to address / كتابة إلى عنوان
    
    // ========================================
    // Array Operations (0x70-0x7F)
    // عمليات المصفوفات
    // ========================================
    
    OP_ARRAY_NEW    = 0x70,  ///< Create new array / إنشاء مصفوفة جديدة
    OP_ARRAY_GET    = 0x71,  ///< Get array element / الحصول على عنصر
    OP_ARRAY_SET    = 0x72,  ///< Set array element / تعيين عنصر
    OP_ARRAY_LEN    = 0x73,  ///< Get array length / الحصول على طول المصفوفة
    OP_ARRAY_PUSH   = 0x74,  ///< Push to array / دفع إلى المصفوفة
    OP_ARRAY_POP    = 0x75,  ///< Pop from array / إزالة من المصفوفة
    OP_ARRAY_SLICE  = 0x76,  ///< Slice array / تقطيع المصفوفة
    
    // ========================================
    // Object Operations (0x80-0x8F)
    // عمليات الكائنات
    // ========================================
    
    OP_OBJ_NEW      = 0x80,  ///< Create new object / إنشاء كائن جديد
    OP_OBJ_GET      = 0x81,  ///< Get object field / الحصول على حقل
    OP_OBJ_SET      = 0x82,  ///< Set object field / تعيين حقل
    OP_OBJ_HAS      = 0x83,  ///< Check if field exists / التحقق من وجود حقل
    OP_OBJ_DEL      = 0x84,  ///< Delete object field / حذف حقل
    
    // ========================================
    // Builtin Functions (0x90-0x9F)
    // الدوال المدمجة
    // ========================================
    
    OP_PRINT        = 0x90,  ///< Print to stdout / طباعة
    OP_PRINT_AR     = 0x91,  ///< Print in Arabic / طباعة بالعربية
    OP_INPUT        = 0x92,  ///< Read from stdin / قراءة من المدخل
    OP_LEN          = 0x93,  ///< Get length / الحصول على الطول
    OP_TYPE         = 0x94,  ///< Get type / الحصول على النوع
    OP_STR          = 0x95,  ///< Convert to string / تحويل إلى نص
    OP_INT          = 0x96,  ///< Convert to int / تحويل إلى عدد صحيح
    OP_FLOAT        = 0x97,  ///< Convert to float / تحويل إلى عدد عشري
    OP_BOOL         = 0x98,  ///< Convert to bool / تحويل إلى منطقي
    
    // ========================================
    // Type & Cast (0xA0-0xAF)
    // النوع والتحويل
    // ========================================
    
    OP_IS_NULL      = 0xA0,  ///< Check if null / التحقق من null
    OP_IS_BOOL      = 0xA1,  ///< Check if bool / التحقق من منطقي
    OP_IS_INT       = 0xA2,  ///< Check if int / التحقق من عدد صحيح
    OP_IS_FLOAT     = 0xA3,  ///< Check if float / التحقق من عدد عشري
    OP_IS_STR       = 0xA4,  ///< Check if string / التحقق من نص
    OP_IS_ARRAY     = 0xA5,  ///< Check if array / التحقق من مصفوفة
    OP_IS_OBJ       = 0xA6,  ///< Check if object / التحقق من كائن
    OP_IS_PTR       = 0xA7,  ///< Check if pointer / التحقق من مؤشر
    OP_CAST         = 0xA8,  ///< Type cast / تحويل نوع
    
    // ========================================
    // Stage 1 Compatibility (0xB0-0xBF)
    // التوافق مع المرحلة 1
    // ========================================
    
    OP_MALLOC       = 0xB0,  ///< malloc(size) / تخصيص ذاكرة
    OP_MMAP         = 0xB1,  ///< mmap(addr, size) / تخطيط ذاكرة
    OP_IO_READ      = 0xB2,  ///< io_read(port) / قراءة من منفذ
    OP_IO_WRITE     = 0xB3,  ///< io_write(port, val) / كتابة إلى منفذ
    OP_INT_GET      = 0xB4,  ///< int_get(index) / الحصول على مقاطعة
    OP_INT_SET      = 0xB5,  ///< int_set(index, handler) / تعيين معالج مقاطعة
    
    // ========================================
    // Special Instructions (0xF0-0xFF)
    // تعليمات خاصة
    // ========================================
    
    OP_HALT         = 0xF0,  ///< Halt execution / إيقاف التنفيذ
    OP_ASSERT       = 0xF1,  ///< Assert condition / تأكيد شرط
    OP_DEBUG        = 0xF2,  ///< Debug breakpoint / نقطة توقف للتصحيح
    OP_GC_COLLECT   = 0xF3,  ///< Trigger GC / تشغيل جامع القمامة
    OP_IMPORT       = 0xF4,  ///< Import module / استيراد وحدة
    OP_EXPORT       = 0xF5,  ///< Export symbol / تصدير رمز
    OP_UNREACHABLE  = 0xFF,  ///< Unreachable code / كود غير قابل للوصول
};

/**
 * @brief الحصول على اسم رمز العملية / Get opcode name
 * @brief Returns human-readable name for opcode
 * 
 * @param op Opcode to get name for
 * @return String representation of opcode
 * 
 * @example
 * ```cpp
 * std::string name = getOpcodeName(OP_ADD);
 * // name = "OP_ADD"
 * ```
 */
const char* getOpcodeName(Opcode op);

/**
 * @brief حجم المعامل / Operand size
 * @brief Get operand size for opcode in bytes
 * 
 * @param op Opcode
 * @return Number of bytes for operands (0-24)
 * 
 * @details
 * (AR) ترجع عدد البايتات المطلوبة للمعاملات.
 *      0 = لا معاملات
 *      1-8 = معامل واحد
 *      9-16 = معاملان
 *      17-24 = ثلاثة معاملات
 * 
 * (EN) Returns number of bytes required for operands.
 */
int getOperandSize(Opcode op);

/**
 * @brief طول التعليمة / Instruction length
 * @brief Total instruction length (opcode + operands)
 * 
 * @param op Opcode
 * @return Total length in bytes (1-25)
 */
inline int getInstructionLength(Opcode op) {
    return 1 + getOperandSize(op);
}

} // namespace VM
} // namespace Sad
