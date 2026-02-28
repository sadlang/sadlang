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
    FUNCTION,   ///< دالة / function pointer
    MAP,        ///< قاموس/خريطة / dictionary/map
    BYTE,       ///< بايت (8-bit) / byte
    ERROR       ///< خطأ / error type
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
        case SIRType::MAP:      return "map";
        case SIRType::BYTE:     return "byte";
        case SIRType::ERROR:    return "error";
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
    // 5. الذاكرة والمؤشرات / Memory (11)
    // ==========================================
    ALLOC,      ///< تخصيص على Stack / Stack allocation
    ALLOC_HEAP, ///< تخصيص على Heap / Heap allocation
    FREE,       ///< تحرير الذاكرة / Free memory
    LOAD,       ///< تحميل من الذاكرة / Load from memory
    STORE,      ///< تخزين في الذاكرة / Store to memory
    MOVE,       ///< نسخ قيمة / Move/Copy value
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
    STRING_TO_F64, ///< تحويل نص لعشري / String to float (atof)
    
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
    // 10. الدوال المضمنة / Builtin (50+)
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
    BUILTIN_DEBUG,  ///< تنقيح / Debug print
    
    // String functions (12)
    BUILTIN_STRING_LENGTH,   ///< طول_نص / String length
    BUILTIN_STRING_CHAR_AT,  ///< رمز_حرف / Char at index
    BUILTIN_STRING_TO_UPPER, ///< تحويل_كبير / To uppercase
    BUILTIN_STRING_TO_LOWER, ///< تحويل_صغير / To lowercase
    BUILTIN_STRING_FIND,     ///< بحث / Find substring
    BUILTIN_STRING_REPLACE,  ///< استبدل / Replace
    BUILTIN_STRING_SUBSTRING,///< استخراج / Substring
    BUILTIN_STRING_TRIM,     ///< قص_أطراف / Trim
    BUILTIN_STRING_SPLIT,    ///< تقسيم / Split
    BUILTIN_STRING_JOIN,     ///< دمج / Join
    BUILTIN_STRING_STARTS_WITH, ///< يبدأ_ب / Starts with
    BUILTIN_STRING_ENDS_WITH,   ///< ينتهي_ب / Ends with
    BUILTIN_STRING_CONTAINS,    ///< يحتوي_على / Contains
    
    // Array functions (10)
    BUILTIN_ARRAY_APPEND,    ///< إضافة_عنصر / Append
    BUILTIN_ARRAY_REMOVE,    ///< إزالة_عنصر / Remove
    BUILTIN_ARRAY_SIZE,      ///< حجم_مصفوفة / Size
    BUILTIN_ARRAY_INDEX_OF,  ///< فهرس / Index of
    BUILTIN_ARRAY_CONTAINS,  ///< يحتوي_عنصر / Contains
    BUILTIN_ARRAY_REVERSE,   ///< قلب / Reverse
    BUILTIN_ARRAY_SORT,      ///< فرز / Sort
    BUILTIN_ARRAY_FIRST,     ///< أول / First element
    BUILTIN_ARRAY_LAST,      ///< آخر / Last element
    BUILTIN_ARRAY_SLICE,     ///< شريحة / Slice
    
    // File I/O functions (8)
    BUILTIN_FILE_READ,       ///< اقرأ_ملف / Read file
    BUILTIN_FILE_WRITE,      ///< اكتب_ملف / Write file
    BUILTIN_FILE_APPEND,     ///< أضف_إلى_ملف / Append to file
    BUILTIN_FILE_DELETE,     ///< احذف_ملف / Delete file
    BUILTIN_FILE_COPY,       ///< انسخ_ملف / Copy file
    BUILTIN_FILE_MOVE,       ///< انقل_ملف / Move file
    BUILTIN_FILE_CREATE_DIR, ///< أنشئ_مجلد / Create directory
    BUILTIN_FILE_LIST_DIR,   ///< اسرد_مجلد / List directory
    
    // Utility functions (6)
    BUILTIN_RANDOM,          ///< عشوائي / Random
    BUILTIN_SLEEP,           ///< نم / Sleep
    BUILTIN_EXIT,            ///< اخرج / Exit
    BUILTIN_TYPE_OF,         ///< النوع / Type of
    
    // ==========================================
    // 11. عمليات برمجة أنظمة التشغيل / OS Development Operations
    // ==========================================
    // (AR) هذا القسم يضيف التعليمات اللازمة لبرمجة أنظمة التشغيل
    //      والتعامل المباشر مع العتاد والمعالج
    // (EN) This section adds instructions needed for OS development
    //      and direct hardware/CPU interaction
    
    INLINE_ASM,              ///< (AR) تجميع مضمّن / (EN) Inline assembly
    BUILTIN_PORT_WRITE,      ///< (AR) منفذ_اكتب — كتابة بايت على منفذ I/O / (EN) Port write (outb)
    BUILTIN_PORT_READ,       ///< (AR) منفذ_اقرأ — قراءة بايت من منفذ I/O / (EN) Port read (inb)
    BUILTIN_PORT_WRITE_16,   ///< (AR) منفذ_اكتب16 — كتابة كلمة (16 بت) / (EN) Port write 16-bit (outw)
    BUILTIN_PORT_READ_16,    ///< (AR) منفذ_اقرأ16 — قراءة كلمة (16 بت) / (EN) Port read 16-bit (inw)
    BUILTIN_PORT_WRITE_32,   ///< (AR) منفذ_اكتب32 — كتابة كلمة مزدوجة (32 بت) / (EN) Port write 32-bit (outl)
    BUILTIN_PORT_READ_32,    ///< (AR) منفذ_اقرأ32 — قراءة كلمة مزدوجة (32 بت) / (EN) Port read 32-bit (inl)
    BUILTIN_MEM_WRITE_8,     ///< (AR) ذاكرة_اكتب — كتابة بايت في عنوان ذاكرة / (EN) Memory write byte (poke)
    BUILTIN_MEM_READ_8,      ///< (AR) ذاكرة_اقرأ — قراءة بايت من عنوان ذاكرة / (EN) Memory read byte (peek)
    BUILTIN_MEM_WRITE_16,    ///< (AR) ذاكرة_اكتب16 / (EN) Memory write 16-bit
    BUILTIN_MEM_READ_16,     ///< (AR) ذاكرة_اقرأ16 / (EN) Memory read 16-bit
    BUILTIN_MEM_WRITE_32,    ///< (AR) ذاكرة_اكتب32 / (EN) Memory write 32-bit
    BUILTIN_MEM_READ_32,     ///< (AR) ذاكرة_اقرأ32 / (EN) Memory read 32-bit
    BUILTIN_MEM_WRITE_64,    ///< (AR) ذاكرة_اكتب64 / (EN) Memory write 64-bit
    BUILTIN_MEM_READ_64,     ///< (AR) ذاكرة_اقرأ64 / (EN) Memory read 64-bit
    BUILTIN_INTERRUPT,       ///< (AR) مقاطعة — إطلاق مقاطعة برمجية / (EN) Software interrupt (int N)
    BUILTIN_HALT,            ///< (AR) توقف — إيقاف المعالج / (EN) Halt CPU (hlt)
    BUILTIN_CLI,             ///< (AR) تعطيل_مقاطعات — تعطيل المقاطعات / (EN) Clear interrupt flag (cli)
    BUILTIN_STI,             ///< (AR) تفعيل_مقاطعات — تفعيل المقاطعات / (EN) Set interrupt flag (sti)
    BUILTIN_ADDR_OF,         ///< (AR) عنوان — الحصول على عنوان متغير / (EN) Address-of operator
    BUILTIN_MEM_COPY,        ///< (AR) انسخ_ذاكرة — نسخ كتلة ذاكرة / (EN) Memory block copy
    BUILTIN_MEM_SET,         ///< (AR) املأ_ذاكرة — ملء كتلة ذاكرة بقيمة / (EN) Memory block fill
    BUILTIN_VGA_WRITE,       ///< (AR) شاشة_اكتب — كتابة حرف في ذاكرة VGA / (EN) Write char to VGA memory
    BUILTIN_VGA_CLEAR,       ///< (AR) شاشة_امسح — مسح شاشة VGA / (EN) Clear VGA screen
    
    // 11b. عمليات Embedded المتقدمة / Advanced Embedded Operations (18)
    // تسلسلي — Serial I/O (4)
    BUILTIN_SERIAL_INIT,     ///< (AR) تسلسلي_هيئ — تهيئة منفذ تسلسلي / (EN) serial_init(port, baud)
    BUILTIN_SERIAL_WRITE,    ///< (AR) تسلسلي_ارسل — إرسال بايت / (EN) serial_send(port, byte)
    BUILTIN_SERIAL_READ,     ///< (AR) تسلسلي_استقبل — استقبال بايت / (EN) serial_receive(port) → byte
    BUILTIN_SERIAL_READY,    ///< (AR) تسلسلي_جاهز — فحص جاهزية / (EN) serial_ready(port) → bool
    // GPIO — منافذ رقمية (3)
    BUILTIN_GPIO_WRITE,      ///< (AR) منفذ_رقمي_اكتب — كتابة GPIO / (EN) gpio_write(pin, value)
    BUILTIN_GPIO_READ,       ///< (AR) منفذ_رقمي_اقرأ — قراءة GPIO / (EN) gpio_read(pin) → value
    BUILTIN_GPIO_MODE,       ///< (AR) حدد_وضع_منفذ — وضع GPIO / (EN) gpio_mode(pin, mode)
    // مؤقت — Timer (3)
    BUILTIN_TIMER_INIT,      ///< (AR) مؤقت_هيئ — تهيئة مؤقت / (EN) timer_init(freq)
    BUILTIN_TIMER_READ,      ///< (AR) مؤقت_قراءة — قراءة مؤقت / (EN) timer_read() → value
    BUILTIN_TIMER_WAIT,      ///< (AR) مؤقت_انتظر — انتظار / (EN) timer_wait(us)
    // تحكم بالنظام — System Control (3)
    BUILTIN_RESET,           ///< (AR) اعد_تشغيل — إعادة تشغيل / (EN) reset / reboot
    BUILTIN_CPUID,           ///< (AR) معرف_المعالج — معرّف CPU / (EN) cpu_id / cpuid
    BUILTIN_RDTSC,           ///< (AR) عداد_الدورات — عداد الساعة / (EN) rdtsc / cycle_count
    // حواجز ذاكرة — Memory Barriers (3)
    BUILTIN_MFENCE,          ///< (AR) حاجز_ذاكرة — حاجز كامل / (EN) mfence / memory_barrier
    BUILTIN_LFENCE,          ///< (AR) حاجز_قراءة — حاجز قراءة / (EN) lfence / read_barrier
    BUILTIN_SFENCE,          ///< (AR) حاجز_كتابة — حاجز كتابة / (EN) sfence / write_barrier
    // DMA — نقل مباشر (2)
    BUILTIN_DMA_INIT,        ///< (AR) نقل_مباشر_هيئ — تهيئة DMA / (EN) dma_init(channel, src, dest, size)
    BUILTIN_DMA_START,       ///< (AR) نقل_مباشر_ابدأ — بدء نقل DMA / (EN) dma_start(channel)
    
    // ==========================================
    // 12. دوال الأمان / Security Functions (14)
    // ==========================================
    // (AR) هذا القسم يضيف الدوال المضمنة لنظام الأمان
    // (EN) This section adds security system builtin functions
    
    BUILTIN_SECURITY_ASSERT,         ///< تأكد / Assert - throws on false
    BUILTIN_SECURITY_VERIFY,         ///< تحقق / Verify - returns bool
    BUILTIN_SECURITY_IS_SAFE,        ///< آمن / Is safe - returns bool
    BUILTIN_SECURITY_PANIC,          ///< ذعر / Panic - halt with message
    BUILTIN_SECURITY_HASH,           ///< هاش / Hash - FNV-1a hash
    BUILTIN_SECURITY_ENCRYPT,        ///< شفّر / Encrypt - XOR encryption
    BUILTIN_SECURITY_DECRYPT,        ///< فك_تشفير / Decrypt - XOR decryption
    BUILTIN_SECURITY_ASSERT_TYPE,    ///< تأكد_نوع / Assert type
    BUILTIN_SECURITY_ASSERT_EQUAL,   ///< تأكد_مساواة / Assert equal
    BUILTIN_SECURITY_ASSERT_GREATER, ///< تأكد_أكبر / Assert greater
    BUILTIN_SECURITY_SANITIZE,       ///< نظّف / Sanitize - HTML entity encoding
    BUILTIN_SECURITY_TIMESTAMP,      ///< وقت_الآن / Current timestamp
    BUILTIN_SECURITY_SECURE_RANDOM,  ///< عشوائي_آمن / Secure random number
    BUILTIN_SECURITY_BASE64_ENCODE,  ///< ترميز_64 / Base64 encode
    
    // ==========================================
    // 13. التكامل مع C/C++ — FFI Functions (15)
    // ==========================================
    // (AR) هذا القسم يضيف دوال التكامل مع لغة C/C++
    // (EN) This section adds C/C++ Foreign Function Interface builtins
    
    FFI_PRINTF,          ///< طباعة_تنسيق / printf — formatted print
    FFI_MALLOC,          ///< حجز / malloc — allocate memory
    FFI_FREE,            ///< حرر / free — free memory
    FFI_REALLOC,         ///< اعد_حجز / realloc — reallocate memory
    FFI_CALLOC,          ///< حجز_صفري / calloc — zero-initialized allocation
    FFI_STRLEN,          ///< طول_نص_س / strlen — C string length
    FFI_STRCPY,          ///< انسخ_نص_س / strcpy — C string copy
    FFI_STRCMP,           ///< قارن_نص_س / strcmp — C string compare
    FFI_STRCAT,          ///< الحق_نص_س / strcat — C string append
    FFI_MEMCPY,          ///< انسخ_ذاكرة_س / memcpy — memory copy
    FFI_MEMSET,          ///< عبئ_ذاكرة_س / memset — memory set
    FFI_FOPEN,           ///< افتح_ملف_س / fopen — open file
    FFI_FCLOSE,          ///< اغلق_ملف_س / fclose — close file
    FFI_FWRITE,          ///< اكتب_ملف_س / fputs — write to file
    FFI_FREAD,           ///< اقرأ_ملف_س / fgets — read from file
    FFI_SYSTEM,          ///< نفذ_امر / system — execute system command
    FFI_GETENV,          ///< قيمة_بيئة / getenv — get environment variable
    FFI_ATOI,            ///< نص_لعدد / atoi — string to integer
    FFI_ATOF,            ///< نص_لعشري / atof — string to float
    FFI_SNPRINTF,        ///< تنسيق_نص / snprintf — format to string buffer

    // ========================================================================
    // Section 14: Async/Await والتزامن / Async/Await & Concurrency
    // ========================================================================
    ASYNC_SPAWN,         ///< أنشئ_مهمة / spawn — spawn async task
    ASYNC_AWAIT,         ///< انتظر_مهمة / await — await a future/task
    ASYNC_YIELD,         ///< أنتج / yield — yield from generator/coroutine
    ASYNC_SLEEP,         ///< نوم_غير_متزامن / async_sleep — non-blocking sleep
    ASYNC_CREATE_FUTURE, ///< أنشئ_مستقبل / create_future — create a promise/future
    ASYNC_RESOLVE_FUTURE,///< أوفِ_مستقبل / resolve_future — resolve a future with value
    ASYNC_GET_FUTURE,    ///< احصل_مستقبل / get_future — get future result (blocking)
    ASYNC_CREATE_CHANNEL,///< أنشئ_قناة / create_channel — create async channel
    ASYNC_CHANNEL_SEND,  ///< أرسل_قناة / channel_send — send to channel
    ASYNC_CHANNEL_RECV,  ///< استقبل_قناة / channel_recv — receive from channel
    ASYNC_CHANNEL_CLOSE, ///< أغلق_قناة / channel_close — close channel
    ASYNC_MUTEX_CREATE,  ///< أنشئ_قفل / create_mutex — create mutex
    ASYNC_MUTEX_LOCK,    ///< اقفل / lock — acquire mutex
    ASYNC_MUTEX_UNLOCK,  ///< افتح_قفل / unlock — release mutex
    ASYNC_THREAD_SPAWN,  ///< أنشئ_خيط / thread_spawn — spawn OS thread
    ASYNC_THREAD_JOIN,   ///< انضم_خيط / thread_join — join thread
    ASYNC_ATOMIC_LOAD,   ///< حمّل_ذري / atomic_load — atomic read
    ASYNC_ATOMIC_STORE,  ///< خزّن_ذري / atomic_store — atomic write
    ASYNC_ATOMIC_ADD,    ///< أضف_ذري / atomic_add — atomic add
    ASYNC_ATOMIC_CAS,    ///< قارن_وبدّل / compare_and_swap — CAS operation
    ASYNC_WAIT_ALL,      ///< انتظر_الكل / wait_all — wait for all tasks
    ASYNC_WAIT_ANY,      ///< انتظر_أي / wait_any — wait for any task
    ASYNC_SELECT,        ///< اختر_قناة / select — select on multiple channels

    // ========================================================================
    // Section 15: عمليات وحدات نظام التشغيل المتقدمة / Advanced OS Module Operations
    // ========================================================================
    // (AR) هذا القسم يضيف دعم المترجم للمكتبة المنخفضة المستوى الكاملة (19 وحدة)
    //      للعمل في وضع بدون نظام تشغيل (freestanding/bare-metal)
    // (EN) This section adds compiler support for the full low-level library (19 modules)
    //      for freestanding/bare-metal OS development

    // --- 15a. وحدة المعالج المتقدمة / Advanced CPU Module (8) ---
    LOWLEVEL_CPU_GET_INFO,       ///< معلومات_المعالج / cpu_get_info — get full CPU info struct
    LOWLEVEL_CPU_GET_FEATURES,   ///< ميزات_المعالج / cpu_get_features — get CPU feature flags
    LOWLEVEL_CPU_READ_MSR,       ///< اقرأ_سجل_نموذج / read_msr — read model-specific register
    LOWLEVEL_CPU_WRITE_MSR,      ///< اكتب_سجل_نموذج / write_msr — write model-specific register
    LOWLEVEL_CPU_READ_CR,        ///< اقرأ_سجل_تحكم / read_cr — read control register (0-4)
    LOWLEVEL_CPU_WRITE_CR,       ///< اكتب_سجل_تحكم / write_cr — write control register (0,3,4)
    LOWLEVEL_CPU_INVLPG,         ///< ابطل_صفحة / invlpg — invalidate TLB entry
    LOWLEVEL_CPU_GET_REPORT,     ///< تقرير_المعالج / cpu_report — full CPU diagnostic report

    // --- 15b. وحدة GDT / GDT Module (3) ---
    LOWLEVEL_GDT_INIT,           ///< هيئ_جدول_واصفات / gdt_init — initialize GDT
    LOWLEVEL_GDT_LOAD,           ///< حمل_جدول_واصفات / gdt_load — load GDT register (lgdt)
    LOWLEVEL_GDT_GET_REPORT,     ///< تقرير_واصفات / gdt_report — GDT diagnostic report

    // --- 15c. وحدة الترحيل / Paging Module (5) ---
    LOWLEVEL_PAGING_INIT,        ///< هيئ_ترحيل / paging_init — initialize paging
    LOWLEVEL_PAGING_MAP,         ///< رحل_صفحة / paging_map — map virtual to physical page
    LOWLEVEL_PAGING_UNMAP,       ///< الغ_ترحيل / paging_unmap — unmap a virtual page
    LOWLEVEL_PAGING_FLUSH_TLB,   ///< افرغ_ذاكرة_ترجمة / paging_flush_tlb — flush entire TLB
    LOWLEVEL_PAGING_GET_REPORT,  ///< تقرير_ترحيل / paging_report — paging diagnostic report

    // --- 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts Module (5) ---
    LOWLEVEL_IDT_INIT,           ///< هيئ_جدول_مقاطعات / idt_init — initialize IDT
    LOWLEVEL_IDT_LOAD,           ///< حمل_جدول_مقاطعات / idt_load — load IDT register (lidt)
    LOWLEVEL_IDT_REGISTER_ISR,   ///< سجل_معالج_مقاطعة / register_isr — register ISR handler
    LOWLEVEL_IDT_ENABLE_IRQ,     ///< فعل_طلب_مقاطعة / enable_irq — enable IRQ line
    LOWLEVEL_IDT_GET_REPORT,     ///< تقرير_مقاطعات / idt_report — interrupt diagnostic report

    // --- 15e. وحدة PCI / PCI Module (5) ---
    LOWLEVEL_PCI_ENUMERATE,      ///< عدد_أجهزة_ناقل / pci_enumerate — enumerate PCI devices
    LOWLEVEL_PCI_READ_CONFIG,    ///< اقرأ_اعدادات_ناقل / pci_read_config — read PCI config register
    LOWLEVEL_PCI_WRITE_CONFIG,   ///< اكتب_اعدادات_ناقل / pci_write_config — write PCI config register
    LOWLEVEL_PCI_GET_DEVICE_COUNT, ///< عدد_الأجهزة / pci_device_count — get PCI device count
    LOWLEVEL_PCI_GET_REPORT,     ///< تقرير_ناقل / pci_report — PCI diagnostic report

    // --- 15f. وحدة DMA المتقدمة / Advanced DMA Module (4) ---
    LOWLEVEL_DMA_INIT,           ///< هيئ_نقل_مباشر / dma_init_full — initialize DMA controller
    LOWLEVEL_DMA_TRANSFER,       ///< ابدأ_نقل / dma_transfer — start DMA transfer
    LOWLEVEL_DMA_STATUS,         ///< حالة_نقل / dma_status — check DMA transfer status
    LOWLEVEL_DMA_GET_REPORT,     ///< تقرير_نقل / dma_report — DMA diagnostic report

    // --- 15g. وحدة الشاشة / Framebuffer Module (8) ---
    LOWLEVEL_FB_INIT,            ///< هيئ_شاشة / fb_init — initialize framebuffer
    LOWLEVEL_FB_SET_PIXEL,       ///< ارسم_نقطة / fb_set_pixel — set pixel (x, y, color)
    LOWLEVEL_FB_DRAW_RECT,       ///< ارسم_مستطيل / fb_draw_rect — draw rectangle outline
    LOWLEVEL_FB_FILL_RECT,       ///< املأ_مستطيل / fb_fill_rect — fill rectangle
    LOWLEVEL_FB_DRAW_LINE,       ///< ارسم_خط / fb_draw_line — draw line
    LOWLEVEL_FB_DRAW_STRING,     ///< ارسم_نص / fb_draw_string — draw text string
    LOWLEVEL_FB_CLEAR,           ///< امسح_شاشة / fb_clear — clear framebuffer
    LOWLEVEL_FB_GET_REPORT,      ///< تقرير_شاشة / fb_report — framebuffer diagnostic report

    // --- 15h. وحدة ACPI / ACPI Module (4) ---
    LOWLEVEL_ACPI_INIT,          ///< هيئ_طاقة / acpi_init — initialize ACPI
    LOWLEVEL_ACPI_FIND_TABLE,    ///< ابحث_جدول_طاقة / acpi_find_table — find ACPI table
    LOWLEVEL_ACPI_SHUTDOWN,      ///< اطفئ / acpi_shutdown — power off system
    LOWLEVEL_ACPI_GET_REPORT,    ///< تقرير_طاقة / acpi_report — ACPI diagnostic report

    // --- 15i. وحدة التزامن / Sync Module (8) ---
    LOWLEVEL_SPINLOCK_INIT,      ///< هيئ_قفل_دوار / spinlock_init — init spinlock
    LOWLEVEL_SPINLOCK_LOCK,      ///< اقفل_دوار / spinlock_lock — acquire spinlock
    LOWLEVEL_SPINLOCK_UNLOCK,    ///< افتح_قفل_دوار / spinlock_unlock — release spinlock
    LOWLEVEL_MUTEX_INIT,         ///< هيئ_كابح / mutex_init — init mutex
    LOWLEVEL_MUTEX_LOCK,         ///< اقفل_كابح / mutex_lock — acquire mutex
    LOWLEVEL_MUTEX_UNLOCK,       ///< افتح_كابح / mutex_unlock — release mutex
    LOWLEVEL_SEMAPHORE_INIT,     ///< هيئ_اشارة / semaphore_init — init semaphore
    LOWLEVEL_BARRIER_INIT,       ///< هيئ_حاجز / barrier_init — init barrier

    // --- 15j. وحدة المجدول / Scheduler Module (6) ---
    LOWLEVEL_SCHED_INIT,         ///< هيئ_مجدول / sched_init — initialize scheduler
    LOWLEVEL_SCHED_CREATE_PROC,  ///< انشئ_عملية / sched_create_process — create process
    LOWLEVEL_SCHED_CREATE_THREAD,///< انشئ_خيط_نواة / sched_create_thread — create kernel thread
    LOWLEVEL_SCHED_YIELD,        ///< تنازل / sched_yield — yield current thread
    LOWLEVEL_SCHED_SLEEP,        ///< نوم_مجدول / sched_sleep — sleep thread (ms)
    LOWLEVEL_SCHED_GET_REPORT,   ///< تقرير_مجدول / sched_report — scheduler diagnostic report

    // --- 15k. وحدة الإقلاع / Boot Module (3) ---
    LOWLEVEL_BOOT_GET_INFO,      ///< معلومات_اقلاع / boot_info — get boot information
    LOWLEVEL_BOOT_GET_MEMORY_MAP,///< خريطة_ذاكرة_اقلاع / boot_memory_map — get boot memory map
    LOWLEVEL_BOOT_GET_REPORT,    ///< تقرير_اقلاع / boot_report — boot diagnostic report

    // --- 15l. وحدة نظام الملفات الافتراضي / VFS Module (7) ---
    LOWLEVEL_VFS_MOUNT,          ///< حمل_قرص / vfs_mount — mount filesystem
    LOWLEVEL_VFS_UNMOUNT,        ///< افصل_قرص / vfs_unmount — unmount filesystem
    LOWLEVEL_VFS_OPEN,           ///< افتح_ملف / vfs_open — open file
    LOWLEVEL_VFS_READ,           ///< اقرأ_ملف_نواة / vfs_read — read from file
    LOWLEVEL_VFS_WRITE,          ///< اكتب_ملف_نواة / vfs_write — write to file
    LOWLEVEL_VFS_CLOSE,          ///< اغلق_ملف_نواة / vfs_close — close file
    LOWLEVEL_VFS_GET_REPORT,     ///< تقرير_ملفات / vfs_report — VFS diagnostic report

    // --- 15m. وحدة APIC / APIC Module (5) ---
    LOWLEVEL_APIC_INIT,          ///< هيئ_متحكم_مقاطعات / apic_init — initialize APIC
    LOWLEVEL_APIC_SEND_EOI,      ///< ارسل_نهاية_مقاطعة / apic_send_eoi — send End-Of-Interrupt
    LOWLEVEL_APIC_SEND_IPI,      ///< ارسل_مقاطعة_معالج / apic_send_ipi — send inter-processor interrupt
    LOWLEVEL_APIC_SET_TIMER,     ///< اضبط_مؤقت_متحكم / apic_set_timer — configure APIC timer
    LOWLEVEL_APIC_GET_REPORT,    ///< تقرير_متحكم_مقاطعات / apic_report — APIC diagnostic report

    // --- 15n. وحدة HPET / HPET Module (4) ---
    LOWLEVEL_HPET_INIT,          ///< هيئ_مؤقت_دقيق / hpet_init — initialize HPET timer
    LOWLEVEL_HPET_READ,          ///< اقرأ_مؤقت_دقيق / hpet_read — read HPET counter
    LOWLEVEL_HPET_SLEEP,         ///< نوم_دقيق / hpet_sleep — precision sleep (nanoseconds)
    LOWLEVEL_HPET_GET_REPORT,    ///< تقرير_مؤقت_دقيق / hpet_report — HPET diagnostic report

    // --- 15o. وحدة استدعاءات النظام / Syscall Module (4) ---
    LOWLEVEL_SYSCALL_INIT,       ///< هيئ_استدعاءات / syscall_init — initialize SYSCALL/SYSRET
    LOWLEVEL_SYSCALL_REGISTER,   ///< سجل_استدعاء / syscall_register — register syscall handler
    LOWLEVEL_SYSCALL_INVOKE,     ///< نفذ_استدعاء / syscall_invoke — invoke syscall by number
    LOWLEVEL_SYSCALL_GET_REPORT, ///< تقرير_استدعاءات / syscall_report — syscall diagnostic report

    // --- 15p. عمليات الذاكرة المتقدمة / Advanced Memory Operations (4) ---
    LOWLEVEL_MEM_ALLOC_PHYS,     ///< خصص_فيزيائي / alloc_physical — allocate physical memory frame
    LOWLEVEL_MEM_FREE_PHYS,      ///< حرر_فيزيائي / free_physical — free physical memory frame
    LOWLEVEL_MEM_MAP_REGION,     ///< رحل_منطقة / map_region — map memory region (MMIO)
    LOWLEVEL_MEM_GET_REPORT,     ///< تقرير_ذاكرة_نواة / mem_report — memory diagnostic report

    // =================================================================
    // القسم 16: بروتوكول الإقلاع الموحد UEFI / UEFI Boot Protocol
    // =================================================================

    // --- 16a. التهيئة والتحكم / Initialization & Control (5) ---
    LOWLEVEL_UEFI_INIT,               ///< uefi_تهيئة / uefi_initialize — initialize UEFI environment
    LOWLEVEL_UEFI_EXIT_BOOT_SERVICES, ///< uefi_إنهاء_خدمات_إقلاع / uefi_exit_boot_services — exit boot services
    LOWLEVEL_UEFI_IS_INITIALIZED,     ///< uefi_هل_مهيأ / uefi_is_initialized — check if UEFI initialized
    LOWLEVEL_UEFI_BS_EXITED,          ///< uefi_خدمات_إقلاع_منتهية / uefi_boot_services_exited
    LOWLEVEL_UEFI_RESET_SYSTEM,       ///< uefi_إعادة_تشغيل / uefi_reset_system — reset/shutdown

    // --- 16b. إدارة الذاكرة / Memory Services (7) ---
    LOWLEVEL_UEFI_ALLOC_PAGES,        ///< uefi_تخصيص_صفحات / uefi_allocate_pages
    LOWLEVEL_UEFI_FREE_PAGES,         ///< uefi_تحرير_صفحات / uefi_free_pages
    LOWLEVEL_UEFI_ALLOC_POOL,         ///< uefi_تخصيص_كتلة / uefi_allocate_pool
    LOWLEVEL_UEFI_FREE_POOL,          ///< uefi_تحرير_كتلة / uefi_free_pool
    LOWLEVEL_UEFI_GET_MEMORY_MAP,     ///< uefi_خريطة_ذاكرة / uefi_get_memory_map
    LOWLEVEL_UEFI_GET_MEMMAP_KEY,     ///< uefi_مفتاح_خريطة / uefi_get_memory_map_key
    LOWLEVEL_UEFI_TOTAL_MEMORY,       ///< uefi_ذاكرة_متاحة / uefi_total_memory

    // --- 16c. بروتوكول الرسوميات GOP / Graphics Output Protocol (10) ---
    LOWLEVEL_UEFI_INIT_GOP,           ///< uefi_تهيئة_رسوميات / uefi_init_gop
    LOWLEVEL_UEFI_SET_GOP_MODE,       ///< uefi_تعيين_وضع_رسوميات / uefi_set_gop_mode
    LOWLEVEL_UEFI_QUERY_GOP_MODE,     ///< uefi_استعلام_وضع / uefi_query_gop_mode
    LOWLEVEL_UEFI_GOP_MODE_COUNT,     ///< uefi_عدد_أوضاع_رسوميات / uefi_gop_mode_count
    LOWLEVEL_UEFI_CURRENT_GOP_MODE,   ///< uefi_وضع_رسوميات_حالي / uefi_current_gop_mode
    LOWLEVEL_UEFI_FRAMEBUFFER_BASE,   ///< uefi_عنوان_إطار / uefi_framebuffer_base
    LOWLEVEL_UEFI_FRAMEBUFFER_SIZE,   ///< uefi_حجم_إطار / uefi_framebuffer_size
    LOWLEVEL_UEFI_FILL_SCREEN,        ///< uefi_ملء_شاشة / uefi_fill_screen
    LOWLEVEL_UEFI_DRAW_RECT,          ///< uefi_رسم_مستطيل / uefi_draw_rect
    LOWLEVEL_UEFI_GOP_BLT,            ///< uefi_blt / uefi_gop_blt — raw BLT operation

    // --- 16d. خدمات وقت التشغيل / Runtime Services (4) ---
    LOWLEVEL_UEFI_GET_TIME,           ///< uefi_الوقت / uefi_get_time
    LOWLEVEL_UEFI_SET_TIME,           ///< uefi_تعيين_وقت / uefi_set_time
    LOWLEVEL_UEFI_GET_VARIABLE,       ///< uefi_قراءة_متغير / uefi_get_variable
    LOWLEVEL_UEFI_SET_VARIABLE,       ///< uefi_كتابة_متغير / uefi_set_variable

    // --- 16e. نظام الملفات / File System (6) ---
    LOWLEVEL_UEFI_OPEN_VOLUME,        ///< uefi_فتح_وحدة_تخزين / uefi_open_volume
    LOWLEVEL_UEFI_OPEN_FILE,          ///< uefi_فتح_ملف / uefi_open_file
    LOWLEVEL_UEFI_READ_FILE,          ///< uefi_قراءة_ملف / uefi_read_file
    LOWLEVEL_UEFI_WRITE_FILE,         ///< uefi_كتابة_ملف / uefi_write_file
    LOWLEVEL_UEFI_CLOSE_FILE,         ///< uefi_إغلاق_ملف / uefi_close_file
    LOWLEVEL_UEFI_FILE_INFO,          ///< uefi_معلومات_ملف / uefi_file_info

    // --- 16f. بروتوكولات ومعلومات / Protocols & System Info (5) ---
    LOWLEVEL_UEFI_LOCATE_PROTOCOL,    ///< uefi_بحث_بروتوكول / uefi_locate_protocol
    LOWLEVEL_UEFI_REVISION,           ///< uefi_إصدار / uefi_revision
    LOWLEVEL_UEFI_VENDOR,             ///< uefi_بائع / uefi_firmware_vendor
    LOWLEVEL_UEFI_FW_REVISION,        ///< uefi_إصدار_بائع / uefi_firmware_revision
    LOWLEVEL_UEFI_REPORT,             ///< uefi_تقرير / uefi_report — comprehensive report

    // ============================================================================
    // القسم 17: ACPI الموسّع / Extended ACPI (Section 17)
    // ============================================================================

    // --- 17a. تهيئة وتفعيل ACPI / ACPI Init & Control (6) ---
    LOWLEVEL_ACPI_INIT_FULL,          ///< acpi_تهيئة / acpi_init — full initialization
    LOWLEVEL_ACPI_INIT_RSDP,          ///< acpi_تهيئة_من_rsdp / acpi_init_from_rsdp
    LOWLEVEL_ACPI_ENABLE,             ///< acpi_تفعيل / acpi_enable
    LOWLEVEL_ACPI_DISABLE,            ///< acpi_تعطيل / acpi_disable
    LOWLEVEL_ACPI_IS_INITIALIZED,     ///< acpi_هل_مهيأ / acpi_is_initialized
    LOWLEVEL_ACPI_VERSION,            ///< acpi_إصدار / acpi_version

    // --- 17b. إدارة الطاقة / Power Management (3) ---
    LOWLEVEL_ACPI_REBOOT,             ///< acpi_إعادة_تشغيل / acpi_reboot
    LOWLEVEL_ACPI_SLEEP,              ///< acpi_نوم / acpi_sleep — enter sleep state
    LOWLEVEL_ACPI_DELAY_US,           ///< acpi_تأخير / acpi_delay_us — microsecond delay

    // --- 17c. مؤقت ومعالجات / Timer & Processors (4) ---
    LOWLEVEL_ACPI_READ_PM_TIMER,      ///< acpi_قراءة_مؤقت / acpi_read_pm_timer
    LOWLEVEL_ACPI_IS_PM_32BIT,        ///< acpi_مؤقت_32بت / acpi_is_pm_timer_32bit
    LOWLEVEL_ACPI_PROCESSOR_COUNT,    ///< acpi_عدد_معالجات / acpi_processor_count
    LOWLEVEL_ACPI_LOCAL_APIC_ADDR,    ///< acpi_عنوان_apic / acpi_local_apic_address

    // --- 17d. PCIe ECAM ---
    LOWLEVEL_ACPI_ECAM_BASE,          ///< acpi_ecam_قاعدة / acpi_ecam_base

    // ============================================================================
    // القسم 18: APIC الموسّع / Extended APIC (Section 18)
    // ============================================================================

    // --- 18a. استعلام ومعلومات / Query & Info (4) ---
    LOWLEVEL_APIC_SUPPORTED,          ///< apic_مدعوم / apic_supported
    LOWLEVEL_APIC_X2_SUPPORTED,       ///< apic_x2_مدعوم / apic_x2_supported
    LOWLEVEL_APIC_ID,                 ///< apic_معرّف / apic_id
    LOWLEVEL_APIC_IO_COUNT,           ///< apic_عدد_io / apic_io_count

    // --- 18b. مؤقت APIC / APIC Timer (5) ---
    LOWLEVEL_APIC_INIT_TIMER,         ///< apic_تهيئة_مؤقت / apic_init_timer
    LOWLEVEL_APIC_START_TIMER,        ///< apic_بدء_مؤقت / apic_start_timer
    LOWLEVEL_APIC_STOP_TIMER,         ///< apic_إيقاف_مؤقت / apic_stop_timer
    LOWLEVEL_APIC_TIMER_COUNT,        ///< apic_عداد_مؤقت / apic_timer_count
    LOWLEVEL_APIC_CALIBRATE,          ///< apic_معايرة_مؤقت / apic_calibrate_timer

    // --- 18c. أولوية ومقاطعات / Priority & Interrupts (5) ---
    LOWLEVEL_APIC_SET_PRIORITY,       ///< apic_أولوية_مهمة / apic_set_priority
    LOWLEVEL_APIC_DISABLE_PIC,        ///< apic_عطّل_pic / apic_disable_pic
    LOWLEVEL_APIC_MASK_IRQ,           ///< apic_قناع_irq / apic_mask_irq
    LOWLEVEL_APIC_UNMASK_IRQ,         ///< apic_إلغاء_قناع_irq / apic_unmask_irq
    LOWLEVEL_APIC_ROUTE_IRQ,          ///< apic_وجّه_irq / apic_route_irq

    // --- 18d. IPI متقدم / Advanced IPI (5) ---
    LOWLEVEL_APIC_SEND_IPI_ALL,       ///< apic_أرسل_للكل / apic_send_ipi_all
    LOWLEVEL_APIC_SEND_INIT_IPI,      ///< apic_أرسل_init / apic_send_init
    LOWLEVEL_APIC_SEND_SIPI,          ///< apic_أرسل_sipi / apic_send_sipi
    LOWLEVEL_APIC_WAIT_DELIVERY,      ///< apic_انتظر_تسليم / apic_wait_delivery
    LOWLEVEL_APIC_INIT_IO,            ///< apic_تهيئة_io / apic_init_io
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
