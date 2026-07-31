// ======================================================================
// sir_types.h - أنواع التمثيل الوسيط البسيط (SIR)
//              Simple Intermediate Representation Types
// ======================================================================
// الوصف بالعربية:
//   تعريف أنواع البيانات الأساسية في SIR:
//   - SadTypeKind: أنواع البيانات (من نظام الأنواع الموحد)
//   - SIROpcode: رموز العمليات (90 opcode)
//   - SIROperand: المعاملات (registers, constants, labels)
//
// English Description:
//   Definition of basic data types in SIR:
//   - SadTypeKind: Data types (from unified type system)
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
#include <iostream>
#include "sad_type_system.h"

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ======================================================================
            // أنواع البيانات / Data Types
            // ======================================================================

            /**
             * @brief (AR) جلب SadTypeKind من نظام الأنواع الموحد إلى فضاء SIR
             * @brief (EN) Bring SadTypeKind from the unified type system into SIR namespace
             */
            using Sad::Types::SadTypeKind;

            /**
             * @brief (AR) تحويل SadTypeKind إلى نص SIR الكلاسيكي
             * @brief (EN) Convert SadTypeKind to classic SIR text
             */
            inline const char *sirTypeToString(SadTypeKind type)
            {
                switch (type)
                {
                case SadTypeKind::Void:
                    return "void";
                case SadTypeKind::Integer:
                    return "i64";
                case SadTypeKind::Float:
                    return "f64";
                case SadTypeKind::Boolean:
                    return "bool";
                case SadTypeKind::Pointer:
                    return "ptr";
                case SadTypeKind::Array:
                    return "array";
                case SadTypeKind::String:
                    return "string";
                case SadTypeKind::Struct:
                    return "struct";
                case SadTypeKind::Function:
                    return "function";
                case SadTypeKind::Map:
                    return "map";
                case SadTypeKind::Tuple:
                    return "tuple";
                case SadTypeKind::Byte:
                    return "byte";
                case SadTypeKind::Error:
                    return "error";
                case SadTypeKind::Class:
                    return "class";
                case SadTypeKind::Enum:
                    return "enum";
                case SadTypeKind::Trait:
                    return "trait";
                case SadTypeKind::Closure:
                    return "closure";
                case SadTypeKind::Union:
                    return "union";
                case SadTypeKind::Optional:
                    return "optional";
                case SadTypeKind::Generic:
                    return "generic";
                case SadTypeKind::Reference:
                    return "ref";
                case SadTypeKind::MutableRef:
                    return "mut_ref";
                case SadTypeKind::Any:
                    return "any";
                case SadTypeKind::Never:
                    return "never";
                case SadTypeKind::Unknown:
                    return "unknown";
                case SadTypeKind::Future:
                    return "future";
                case SadTypeKind::Generator:
                    return "generator";
                default:
                    return "unknown";
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
            enum class SIROpcode
            {
                // ==========================================
                // 1. العمليات الحسابية / Arithmetic (10)
                // ==========================================
                ADD_I64,       ///< إضافة أعداد صحيحة / Integer addition
                ADD_F64,       ///< إضافة أعداد عشرية / Float addition
                SUB_I64,       ///< طرح أعداد صحيحة / Integer subtraction
                SUB_F64,       ///< طرح أعداد عشرية / Float subtraction
                MUL_I64,       ///< ضرب أعداد صحيحة / Integer multiplication
                MUL_F64,       ///< ضرب أعداد عشرية / Float multiplication
                DIV_I64,       ///< قسمة أعداد صحيحة / Integer division
                DIV_F64,       ///< قسمة أعداد عشرية / Float division
                FLOOR_DIV_I64, ///< قسمة صحيحة أرضية / Floor division (always integer)
                MOD_I64,       ///< باقي القسمة / Modulo
                NEG,           ///< السالب / Negation
                NULL_ASSERT,   ///< تأكيد عدم الفراغ مؤكَّد T؟→T؛ يُجهِض عند الحارس / Null assertion (NS-05)

                // ==========================================
                // 2. العمليات الثنائية / Bitwise (8)
                // ==========================================
                AND, ///< AND الثنائي / Bitwise AND
                OR,  ///< OR الثنائي / Bitwise OR
                XOR, ///< XOR الثنائي / Bitwise XOR
                NOT, ///< NOT الثنائي / Bitwise NOT
                SHL, ///< إزاحة لليسار / Shift left
                SHR, ///< إزاحة لليمين (منطقية) / Shift right (logical)
                SAR, ///< إزاحة لليمين (حسابية) / Shift right (arithmetic)
                ROL, ///< دوران لليسار / Rotate left

                // ==========================================
                // 3. المقارنات / Comparison (6)
                // ==========================================
                EQ, ///< يساوي / Equal
                NE, ///< لا يساوي / Not equal
                LT, ///< أصغر من / Less than
                LE, ///< أصغر أو يساوي / Less or equal
                GT, ///< أكبر من / Greater than
                GE, ///< أكبر أو يساوي / Greater or equal

                // ==========================================
                // 4. التحكم في التدفق / Control Flow (8)
                // ==========================================
                BR,            ///< قفز غير مشروط / Unconditional branch
                BR_COND,       ///< قفز مشروط / Conditional branch
                RET,           ///< إرجاع / Return
                RET_VOID,      ///< إرجاع فارغ / Return void
                CALL,          ///< استدعاء دالة / Function call
                CALL_INDIRECT, ///< استدعاء غير مباشر / Indirect call
                SWITCH,        ///< قفز محسوب / Switch statement
                PHI,           ///< عقدة PHI لـ SSA / PHI node for SSA

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
                ARRAY_NEW,    ///< إنشاء مصفوفة / Create array
                ARRAY_GET,    ///< الحصول على عنصر / Get element
                ARRAY_SET,    ///< تعديل عنصر / Set element
                ARRAY_LEN,    ///< طول المصفوفة / Array length
                ARRAY_APPEND, ///< إضافة عنصر / Append element
                ARRAY_REMOVE, ///< حذف عنصر / Remove element
                ARRAY_CONCAT, ///< دمج مصفوفتين / Concatenate two arrays
                ARRAY_ZIP,    ///< زاوج — اقتران مصفوفتين أزواجًا / Zip two arrays into pairs (Maps::ZIP)

                // ==========================================
                // 6b. الصفوف / Tuples (3)
                // ==========================================
                TUPLE_NEW, ///< إنشاء صف / Create tuple
                TUPLE_GET, ///< الحصول على عنصر صف / Get tuple element
                TUPLE_LEN, ///< طول الصف / Tuple length

                // ==========================================
                // 7. النصوص / Strings (8)
                // ==========================================
                STRING_NEW,     ///< إنشاء نص / Create string
                STRING_LEN,     ///< طول النص / String length
                STRING_CONCAT,  ///< دمج نصوص / Concatenate strings
                STRING_CMP,     ///< مقارنة نصوص / Compare strings
                STRING_SUBSTR,  ///< استخراج جزء / Substring
                STRING_FIND,    ///< البحث / Find substring
                STRING_REPLACE, ///< الاستبدال / Replace
                STRING_TO_I64,  ///< تحويل نص لرقم / String to integer
                STRING_TO_F64,  ///< تحويل نص لعشري / String to float (atof)

                // ==========================================
                // 8. البرمجة الكائنية / OOP (10)
                // ==========================================
                OBJECT_NEW,       ///< إنشاء كائن / Create object
                OBJECT_GET,       ///< الحصول على خاصية / Get property
                OBJECT_SET,       ///< تعديل خاصية / Set property
                OBJECT_CALL,      ///< استدعاء دالة / Call method
                INSTANCEOF,       ///< تحقق من النوع / Type check
                OBJECT_CAST,      ///< تحويل كائن / Object cast
                CLASS_DEF,        ///< تعريف صنف / Define class
                METHOD_DEF,       ///< تعريف دالة / Define method
                FIELD_DEF,        ///< تعريف خاصية / Define field
                CONSTRUCTOR_CALL, ///< استدعاء منشئ / Constructor call

                // ==========================================
                // 8b. التعدادات الجبرية / ADT Enums (4)
                // ==========================================
                // (AR) عمليات التعدادات الجبرية (tagged unions)
                //      تعداد شكل { دائرة(نصف_القطر)، مستطيل(عرض، ارتفاع)، نقطة }
                //      كل عملية تتعامل مع البنية: { tag: i64, field0: any, field1: any, ... }
                // (EN) ADT enum operations (tagged unions)
                //      Each operates on struct: { tag: i64, field0: any, field1: any, ... }

                ENUM_CONSTRUCT, ///< (AR) إنشاء قيمة تعداد جبري: %r = EnumConstruct(enumName, variantIdx, [حقول])
                                ///< (EN) Create ADT enum value: %r = EnumConstruct(enumName, variantIdx, [fields])
                                ///< operands[0] = ConstantString(enumName), operands[1] = ConstantI64(variantIdx)
                                ///< operands[2..N] = field values; result = STRUCT register

                ENUM_GET_TAG, ///< (AR) استخراج المميّز من قيمة تعداد: %r = EnumGetTag(%enum_val)
                              ///< (EN) Extract discriminant from enum value: %r = EnumGetTag(%enum_val)
                              ///< operands[0] = STRUCT register; result = I64 register

                ENUM_GET_PAYLOAD, ///< (AR) استخراج حقل من حمولة التعداد: %r = EnumGetPayload(%enum_val, fieldIdx)
                                  ///< (EN) Extract field from enum payload: %r = EnumGetPayload(%enum_val, fieldIdx)
                                  ///< operands[0] = STRUCT register, operands[1] = ConstantI64(fieldIdx)
                                  ///< result = field value (type depends on variant definition)

                ENUM_IS_VARIANT, ///< (AR) فحص هل التعداد من حالة معينة: %r = EnumIsVariant(%enum_val, variantIdx)
                                 ///< (EN) Check if enum is specific variant: %r = EnumIsVariant(%enum_val, variantIdx)
                                 ///< operands[0] = STRUCT register, operands[1] = ConstantI64(variantIdx)
                                 ///< result = BOOL register

                ENUM_FREE, ///< (AR) تحرير ذاكرة قيمة تعداد جبري: EnumFree(%enum_val)
                           ///< (EN) Free ADT enum value memory: EnumFree(%enum_val)
                           ///< operands[0] = STRUCT register (ptr to heap-allocated ADT)
                           ///< no result — void operation

                // ==========================================
                // 9. تحويل الأنواع / Type Conversion (8)
                // ==========================================
                I64_TO_F64,      ///< رقم صحيح → عشري / Integer to float
                F64_TO_I64,      ///< رقم عشري → صحيح / Float to integer
                F64_TO_I64_SAT,  ///< عشري → صحيح مُشبَع (llvm.fptosi.sat؛ دلالة البتّيّات الموحَّدة منصّيًّا) / Saturating float→int (bitwise-builtin semantics)
                I64_TO_BOOL,     ///< رقم → منطقي / Integer to boolean
                BOOL_TO_I64,     ///< منطقي → رقم / Boolean to integer
                I64_TO_STRING,   ///< رقم → نص / Integer to string
                F64_TO_STRING,   ///< عشري → نص / Float to string
                BOOL_TO_STRING,  ///< منطقي → نص / Boolean to string
                ARRAY_TO_STRING, ///< مصفوفة → نص / Array to string
                TUPLE_TO_STRING, ///< صف → نص / Tuple to string
                CAST,            ///< تحويل عام / General cast

                // ==========================================
                // 10. الدوال المضمنة / Builtin (50+)
                // ==========================================
                BUILTIN_PRINT,  ///< اطبع / Print
                BUILTIN_READ,   ///< اقرأ / Read
                BUILTIN_SQRT,   ///< جذر / Square root
                BUILTIN_LOG,    ///< لوغ / Natural logarithm
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

                // Math functions - extended (8)
                BUILTIN_LOG10, ///< لوغ10 / Log base 10
                BUILTIN_LOG2,  ///< لوغ2 / Log base 2
                BUILTIN_ASIN,  ///< قوس_جيب / Arc sine
                BUILTIN_ACOS,  ///< قوس_جيب_تمام / Arc cosine
                BUILTIN_TRUNC, ///< اقتطاع / Truncate (toward zero)
                BUILTIN_FMOD,  ///< باقي / Floating-point remainder
                BUILTIN_CLAMP, ///< قيد / Clamp (min ≤ x ≤ max)
                BUILTIN_ATAN,  ///< قوس_ظل / Arc tangent

                // String functions (12)
                BUILTIN_STRING_LENGTH,      ///< طول_نص / String length
                BUILTIN_STRING_CHAR_AT,     ///< رمز_حرف / Char at index
                BUILTIN_STRING_TO_UPPER,    ///< تحويل_كبير / To uppercase
                BUILTIN_STRING_TO_LOWER,    ///< تحويل_صغير / To lowercase
                BUILTIN_STRING_FIND,        ///< بحث / Find substring
                BUILTIN_STRING_REPLACE,     ///< استبدل / Replace
                BUILTIN_STRING_SUBSTRING,   ///< استخراج / Substring
                BUILTIN_STRING_TRIM,        ///< قص_أطراف / Trim
                BUILTIN_STRING_SPLIT,       ///< تقسيم / Split
                BUILTIN_STRING_JOIN,        ///< دمج / Join
                BUILTIN_STRING_STARTS_WITH, ///< يبدأ_ب / Starts with
                BUILTIN_STRING_ENDS_WITH,   ///< ينتهي_ب / Ends with
                BUILTIN_STRING_CONTAINS,    ///< يحتوي_على / Contains

                // Array functions (10)
                BUILTIN_ARRAY_APPEND,   ///< إضافة_عنصر / Append
                BUILTIN_ARRAY_REMOVE,   ///< إزالة_عنصر / Remove
                BUILTIN_ARRAY_SIZE,     ///< حجم_مصفوفة / Size
                BUILTIN_ARRAY_INDEX_OF, ///< فهرس / Index of
                BUILTIN_ARRAY_CONTAINS, ///< يحتوي_عنصر / Contains
                BUILTIN_ARRAY_REVERSE,  ///< قلب / Reverse
                BUILTIN_ARRAY_SORT,     ///< فرز / Sort
                BUILTIN_ARRAY_FIRST,    ///< أول / First element
                BUILTIN_ARRAY_LAST,     ///< آخر / Last element
                BUILTIN_ARRAY_SLICE,    ///< شريحة / Slice

                // File I/O functions (8)
                BUILTIN_FILE_READ,       ///< اقرأ_ملف / Read file
                BUILTIN_FILE_WRITE,      ///< اكتب_ملف / Write file
                BUILTIN_FILE_APPEND,     ///< أضف_إلى_ملف / Append to file
                BUILTIN_FILE_DELETE,     ///< احذف_ملف / Delete file
                BUILTIN_FILE_COPY,       ///< انسخ_ملف / Copy file
                BUILTIN_FILE_MOVE,       ///< انقل_ملف / Move file
                BUILTIN_FILE_CREATE_DIR, ///< أنشئ_مجلد / Create directory
                BUILTIN_FILE_REMOVE_DIR, ///< احذف_مجلد / Remove an empty directory
                BUILTIN_FILE_LIST_DIR,   ///< اسرد_مجلد / List directory
                BUILTIN_FILE_IS_DIR,     ///< هل_مجلد / Is directory (stat-based)
                BUILTIN_FILE_EXISTS,     ///< هل_موجود / Path exists (file or directory)
                BUILTIN_FILE_IS_FILE,    ///< هل_ملف / Is a regular file (stat-based, follows links)
                BUILTIN_REGEX_SEARCH,    ///< تعبير_بحث / Regex search → matched text or void
                BUILTIN_REGEX_MATCH,     ///< تعبير_مطابقة / Regex full match → boolean
                BUILTIN_FILE_IS_SYMLINK,  ///< هل_رابط_رمزي / Is symlink (lstat-based, does not follow)
                BUILTIN_FILE_REAL_PATH,   ///< المسار_الحقيقي / Canonical real path (follows symlinks)
                BUILTIN_FILE_ABS_PATH,    ///< المسار_المطلق / Absolute path (textual, no symlink resolution)
                BUILTIN_FILE_WRITE_BYTES, ///< اكتب_بايتات / Write raw bytes (fwrite, embedded NUL)
                BUILTIN_FILE_READ_BYTES,  ///< اقرأ_بايتات / Read raw bytes (fread → byte array)

                // Utility functions (6)
                BUILTIN_RANDOM,  ///< عشوائي / Random
                BUILTIN_SLEEP,   ///< نم / Sleep
                BUILTIN_EXIT,    ///< اخرج / Exit
                BUILTIN_TYPE_OF, ///< النوع / Type of

                // Type checking functions (4)
                BUILTIN_IS_INTEGER, ///< هو_رقم / Is integer
                BUILTIN_IS_FLOAT,   ///< هو_عشري / Is float
                BUILTIN_IS_STRING,  ///< هو_نص / Is string
                BUILTIN_IS_ARRAY,   ///< هو_مصفوفة / Is array

                // Additional conversion functions (1)
                BUILTIN_TO_BOOL, ///< لمنطقي / To boolean

                // Additional IO functions (2)
                BUILTIN_READ_LINE,    ///< قراءة_سطر / Read line
                BUILTIN_CLEAR_SCREEN, ///< مسح_الشاشة / Clear screen

                // Additional math/collection functions (1)
                BUILTIN_SUM, ///< جمع / Sum of array

                // ==========================================
                // 11. عمليات برمجة أنظمة التشغيل / OS Development Operations
                // ==========================================
                // (AR) هذا القسم يضيف التعليمات اللازمة لبرمجة أنظمة التشغيل
                //      والتعامل المباشر مع العتاد والمعالج
                // (EN) This section adds instructions needed for OS development
                //      and direct hardware/CPU interaction

                INLINE_ASM,            ///< (AR) تجميع مضمّن / (EN) Inline assembly
                BUILTIN_PORT_WRITE,    ///< (AR) اكتب_منفذ — كتابة بايت على منفذ I/O / (EN) Port write (outb)
                BUILTIN_PORT_READ,     ///< (AR) اقرأ_منفذ — قراءة بايت من منفذ I/O / (EN) Port read (inb)
                BUILTIN_PORT_WRITE_16, ///< (AR) اكتب_منفذ16 — كتابة كلمة (16 بت) / (EN) Port write 16-bit (outw)
                BUILTIN_PORT_READ_16,  ///< (AR) اقرأ_منفذ16 — قراءة كلمة (16 بت) / (EN) Port read 16-bit (inw)
                BUILTIN_PORT_WRITE_32, ///< (AR) اكتب_منفذ32 — كتابة كلمة مزدوجة (32 بت) / (EN) Port write 32-bit (outl)
                BUILTIN_PORT_READ_32,  ///< (AR) اقرأ_منفذ32 — قراءة كلمة مزدوجة (32 بت) / (EN) Port read 32-bit (inl)
                BUILTIN_MEM_WRITE_8,   ///< (AR) اكتب_ذاكرة — كتابة بايت في عنوان ذاكرة / (EN) Memory write byte (poke)
                BUILTIN_MEM_READ_8,    ///< (AR) اقرأ_ذاكرة — قراءة بايت من عنوان ذاكرة / (EN) Memory read byte (peek)
                BUILTIN_MEM_WRITE_16,  ///< (AR) اكتب_ذاكرة16 / (EN) Memory write 16-bit
                BUILTIN_MEM_READ_16,   ///< (AR) اقرأ_ذاكرة16 / (EN) Memory read 16-bit
                BUILTIN_MEM_WRITE_32,  ///< (AR) اكتب_ذاكرة32 / (EN) Memory write 32-bit
                BUILTIN_MEM_READ_32,   ///< (AR) اقرأ_ذاكرة32 / (EN) Memory read 32-bit
                BUILTIN_MEM_WRITE_64,  ///< (AR) اكتب_ذاكرة64 / (EN) Memory write 64-bit
                BUILTIN_MEM_READ_64,   ///< (AR) اقرأ_ذاكرة64 / (EN) Memory read 64-bit
                BUILTIN_INTERRUPT,     ///< (AR) مقاطعة — إطلاق مقاطعة برمجية / (EN) Software interrupt (int N)
                BUILTIN_HALT,          ///< (AR) توقف — إيقاف المعالج / (EN) Halt CPU (hlt)
                BUILTIN_CLI,           ///< (AR) تعطيل_مقاطعات — تعطيل المقاطعات / (EN) Clear interrupt flag (cli)
                BUILTIN_STI,           ///< (AR) تفعيل_مقاطعات — تفعيل المقاطعات / (EN) Set interrupt flag (sti)
                BUILTIN_ADDR_OF,       ///< (AR) عنوان — الحصول على عنوان متغير / (EN) Address-of operator
                BUILTIN_MEM_COPY,      ///< (AR) انسخ_ذاكرة — نسخ كتلة ذاكرة / (EN) Memory block copy
                BUILTIN_MEM_SET,       ///< (AR) املأ_ذاكرة — ملء كتلة ذاكرة بقيمة / (EN) Memory block fill
                BUILTIN_MEM_FILL_32,   ///< (AR) املأ_ذاكرة32 — ملء بكلمات 32-بت (REP STOSD) / (EN) Fill 32-bit words
                BUILTIN_MEM_COPY_32,   ///< (AR) انسخ_ذاكرة32 — نسخ كلمات 32-بت (REP MOVSD) / (EN) Copy 32-bit words
                BUILTIN_VGA_WRITE,     ///< (AR) شاشة_اكتب — كتابة حرف في ذاكرة VGA / (EN) Write char to VGA memory
                BUILTIN_VGA_CLEAR,     ///< (AR) شاشة_امسح — مسح شاشة VGA / (EN) Clear VGA screen

                // 11b. عمليات Embedded المتقدمة / Advanced Embedded Operations (18)
                // تسلسلي — Serial I/O (4)
                BUILTIN_SERIAL_INIT,  ///< (AR) تسلسلي_هيئ — تهيئة منفذ تسلسلي / (EN) serial_init(port, baud)
                BUILTIN_SERIAL_WRITE, ///< (AR) تسلسلي_ارسل — إرسال بايت / (EN) serial_send(port, byte)
                BUILTIN_SERIAL_READ,  ///< (AR) تسلسلي_استقبل — استقبال بايت / (EN) serial_receive(port) → byte
                BUILTIN_SERIAL_READY, ///< (AR) تسلسلي_جاهز — فحص جاهزية / (EN) serial_ready(port) → bool
                                      // GPIO — منافذ رقمية (3)
                BUILTIN_GPIO_WRITE,   ///< (AR) منفذ_رقمي_اكتب — كتابة GPIO / (EN) gpio_write(pin, value)
                BUILTIN_GPIO_READ,    ///< (AR) منفذ_رقمي_اقرأ — قراءة GPIO / (EN) gpio_read(pin) → value
                BUILTIN_GPIO_MODE,    ///< (AR) حدد_وضع_منفذ — وضع GPIO / (EN) gpio_mode(pin, mode)
                                      // مؤقت — Timer (3)
                BUILTIN_TIMER_INIT,   ///< (AR) مؤقت_هيئ — تهيئة مؤقت / (EN) timer_init(freq)
                BUILTIN_TIMER_READ,   ///< (AR) مؤقت_قراءة — قراءة مؤقت / (EN) timer_read() → value
                BUILTIN_TIMER_WAIT,   ///< (AR) مؤقت_انتظر — انتظار / (EN) timer_wait(us)
                                      // تحكم بالنظام — System Control (3)
                BUILTIN_RESET,        ///< (AR) اعد_تشغيل — إعادة تشغيل / (EN) reset / reboot
                BUILTIN_CPUID,        ///< (AR) معرف_المعالج — معرّف CPU / (EN) cpu_id / cpuid
                BUILTIN_RDTSC,        ///< (AR) عداد_الدورات — عداد الساعة / (EN) rdtsc / cycle_count
                                      // حواجز ذاكرة — Memory Barriers (3)
                BUILTIN_MFENCE,       ///< (AR) حاجز_ذاكرة — حاجز كامل / (EN) mfence / memory_barrier
                BUILTIN_LFENCE,       ///< (AR) حاجز_قراءة — حاجز قراءة / (EN) lfence / read_barrier
                BUILTIN_SFENCE,       ///< (AR) حاجز_كتابة — حاجز كتابة / (EN) sfence / write_barrier
                                      // DMA — نقل مباشر (2)
                BUILTIN_DMA_INIT,     ///< (AR) نقل_مباشر_هيئ — تهيئة DMA / (EN) dma_init(channel, src, dest, size)
                BUILTIN_DMA_START,    ///< (AR) نقل_مباشر_ابدأ — بدء نقل DMA / (EN) dma_start(channel)

                // ==========================================
                // 12. دوال الأمان / Security Functions (14)
                // ==========================================
                // (AR) هذا القسم يضيف الدوال المضمنة لنظام الأمان
                // (EN) This section adds security system builtin functions

                BUILTIN_SECURITY_ASSERT,         ///< تأكد / Assert - throws on false
                BUILTIN_SECURITY_VERIFY,         ///< تحقق / Verify - returns bool
                BUILTIN_SECURITY_IS_SAFE,        ///< آمن / Is safe - returns bool
                BUILTIN_SECURITY_PANIC,          ///< ذعر / Panic - halt with message
                BUILTIN_SECURITY_HASH,           ///< هاش / Hash - SHA-256 hex string
                BUILTIN_SECURITY_ENCRYPT,        ///< شفّر / Encrypt - SHA-256-CTR stream cipher
                BUILTIN_SECURITY_DECRYPT,        ///< فك_تشفير / Decrypt - SHA-256-CTR stream cipher
                BUILTIN_SECURITY_ASSERT_TYPE,    ///< تأكد_نوع / Assert type
                BUILTIN_SECURITY_ASSERT_EQUAL,   ///< تأكد_مساواة / Assert equal
                BUILTIN_SECURITY_ASSERT_GREATER, ///< تأكد_أكبر / Assert greater
                BUILTIN_SECURITY_SANITIZE,       ///< نظّف / Sanitize - HTML entity encoding
                BUILTIN_SECURITY_TIMESTAMP,      ///< وقت_الآن / Current timestamp
                BUILTIN_SECURITY_SECURE_RANDOM,  ///< عشوائي_آمن / Secure random number
                BUILTIN_SECURITY_BASE64_ENCODE,  ///< ترميز_64 / Base64 encode

                // ==========================================
                // 12ب. وحدة تشفير — Crypto module builtins (BLAKE3)
                // ==========================================
                BUILTIN_CRYPTO_BLAKE3_HASH,       ///< بلايك3 / BLAKE3 hash - hex string
                BUILTIN_CRYPTO_BLAKE3_KEYED_HASH, ///< هاش_مفتاح / BLAKE3 keyed hash (MAC) - hex string
                BUILTIN_CRYPTO_KDF_PBKDF2,        ///< اشتق_مفتاح_مرور / PBKDF2-HMAC-SHA256 - hex string
                BUILTIN_CRYPTO_KDF_HKDF,          ///< اشتق_مفتاح / HKDF-SHA256 - hex string
                BUILTIN_CRYPTO_AEAD_ENCRYPT,      ///< شفّر_موثّق / ChaCha20-Poly1305 AEAD encrypt - hex envelope
                BUILTIN_CRYPTO_AEAD_DECRYPT,      ///< فك_تشفير_موثّق / ChaCha20-Poly1305 AEAD decrypt - plaintext (fail-closed)
                BUILTIN_CRYPTO_KDF_ARGON2ID,      ///< أرجون2 / Argon2id (RFC 9106) - hex string
                BUILTIN_CRYPTO_X25519_KEYGEN_PRIV,  ///< ولّد_مفتاح_خاص_x25519 / X25519 private key - hex
                BUILTIN_CRYPTO_X25519_DERIVE_PUB,   ///< اشتق_مفتاح_عام_x25519 / X25519 public from private - hex
                BUILTIN_CRYPTO_X25519_EXCHANGE,     ///< تبادل_مفتاح / X25519 DH shared secret - hex (all-zero rejected)
                BUILTIN_CRYPTO_ED25519_KEYGEN_PRIV, ///< ولّد_مفتاح_خاص_توقيع / Ed25519 seed - hex
                BUILTIN_CRYPTO_ED25519_DERIVE_PUB,  ///< اشتق_مفتاح_عام_توقيع / Ed25519 public from seed - hex
                BUILTIN_CRYPTO_ED25519_SIGN,        ///< وقّع / Ed25519 signature - hex (128 chars)
                BUILTIN_CRYPTO_ED25519_VERIFY,      ///< تحقق_توقيع / Ed25519 verify - boolean (query, never throws)

                // ==========================================
                // 13. التكامل مع C/C++ — FFI Functions (15)
                // ==========================================
                // (AR) هذا القسم يضيف دوال التكامل مع لغة C/C++
                // (EN) This section adds C/C++ Foreign Function Interface builtins

                FFI_PRINTF,   ///< طباعة_تنسيق / printf — formatted print
                FFI_MALLOC,   ///< حجز / malloc — allocate memory
                FFI_FREE,     ///< حرر / free — free memory
                FFI_REALLOC,  ///< اعد_حجز / realloc — reallocate memory
                FFI_CALLOC,   ///< حجز_صفري / calloc — zero-initialized allocation
                FFI_STRLEN,   ///< طول_نص_س / strlen — C string length
                FFI_STRCPY,   ///< انسخ_نص_س / strcpy — C string copy
                FFI_STRCMP,   ///< قارن_نص_س / strcmp — C string compare
                FFI_STRCAT,   ///< الحق_نص_س / strcat — C string append
                FFI_MEMCPY,   ///< انسخ_ذاكرة_س / memcpy — memory copy
                FFI_MEMSET,   ///< عبئ_ذاكرة_س / memset — memory set
                FFI_FOPEN,    ///< افتح_ملف_س / fopen — open file
                FFI_FCLOSE,   ///< اغلق_ملف_س / fclose — close file
                FFI_FWRITE,   ///< اكتب_ملف_س / fputs — write to file
                FFI_FREAD,    ///< اقرأ_ملف_س / fgets — read from file
                FFI_SYSTEM,   ///< نفذ_امر / system — execute system command
                FFI_GETENV,   ///< قيمة_بيئة / getenv — get environment variable
                FFI_ATOI,     ///< نص_لعدد / atoi — string to integer
                FFI_ATOF,     ///< نص_لعشري / atof — string to float
                FFI_SNPRINTF, ///< تنسيق_نص / snprintf — format to string buffer

                // ========================================================================
                // Section 14: Async/Await والتزامن / Async/Await & Concurrency
                // ========================================================================
                ASYNC_SPAWN,                ///< أنشئ_مهمة / spawn — spawn async task
                ASYNC_AWAIT,                ///< انتظر_مهمة / await — await a future/task
                ASYNC_YIELD,                ///< أنتج / yield — yield from generator/coroutine
                ASYNC_SLEEP,                ///< نوم_غير_متزامن / async_sleep — non-blocking sleep
                ASYNC_CREATE_FUTURE,        ///< أنشئ_مستقبل / create_future — create a promise/future
                ASYNC_RESOLVE_FUTURE,       ///< أوفِ_مستقبل / resolve_future — resolve a future with value
                ASYNC_GET_FUTURE,           ///< احصل_مستقبل / get_future — get future result (blocking)
                ASYNC_CREATE_CHANNEL,       ///< أنشئ_قناة / create_channel — create async channel
                ASYNC_CHANNEL_SEND,         ///< أرسل_قناة / channel_send — send to channel
                ASYNC_CHANNEL_RECV,         ///< استقبل_قناة / channel_recv — receive from channel
                ASYNC_CHANNEL_CLOSE,        ///< أغلق_قناة / channel_close — close channel
                ASYNC_CHANNEL_IS_CLOSED,    ///< هل_القناة_مغلقة / channel_is_closed — check if channel is closed
                ASYNC_CHANNEL_HAS_DATA,     ///< هل_القناة_تحتوي_بيانات / channel_has_data — check if channel has data
                ASYNC_CHANNEL_SIZE,         ///< حجم_القناة / channel_size — get channel element count
                ASYNC_CHANNEL_CAPACITY,     ///< سعة_القناة / channel_capacity — get channel max capacity
                ASYNC_CHANNEL_TRY_SEND,     ///< حاول_ارسل / channel_try_send — non-blocking send
                ASYNC_CHANNEL_TRY_RECV,     ///< حاول_استقبل / channel_try_recv — non-blocking receive
                ASYNC_CHANNEL_SEND_TIMEOUT, ///< أرسل_بمهلة / channel_send_timeout — send with timeout
                ASYNC_CHANNEL_RECV_TIMEOUT, ///< استقبل_بمهلة / channel_recv_timeout — recv with timeout
                ASYNC_MUTEX_CREATE,         ///< أنشئ_قفل / create_mutex — create mutex
                ASYNC_MUTEX_LOCK,           ///< اقفل / lock — acquire mutex
                ASYNC_MUTEX_UNLOCK,         ///< افتح_قفل / unlock — release mutex
                ASYNC_MUTEX_TRY_LOCK,       ///< حاول_قفل / try_lock — non-blocking lock attempt
                ASYNC_MUTEX_IS_LOCKED,      ///< مقفل / is_locked — check if mutex is locked
                ASYNC_FUTURE_IS_READY,      ///< جاهز / is_ready — check if future has value
                ASYNC_THREAD_SPAWN,         ///< أنشئ_خيط / thread_spawn — spawn OS thread
                ASYNC_THREAD_JOIN,          ///< انضم_خيط / thread_join — join thread
                ASYNC_ATOMIC_LOAD,          ///< حمّل_ذري / atomic_load — atomic read
                ASYNC_ATOMIC_STORE,         ///< خزّن_ذري / atomic_store — atomic write
                ASYNC_ATOMIC_ADD,           ///< أضف_ذري / atomic_add — atomic add
                ASYNC_ATOMIC_CAS,           ///< قارن_وبدّل / compare_and_swap — CAS operation
                ASYNC_WAIT_ALL,             ///< انتظر_الكل / wait_all — wait for all tasks
                ASYNC_WAIT_ANY,             ///< انتظر_أي / wait_any — wait for any task
                ASYNC_SELECT,               ///< اختر_قناة / select — select on multiple channels

                // ========================================================================
                // Section 14c: WaitGroup — مجموعة الانتظار
                // ========================================================================
                // (AR) عمليات مجموعة الانتظار: إنشاء، إضافة، إنهاء، انتظار، عداد
                // (EN) WaitGroup operations: create, add, done, wait, count
                ASYNC_WG_CREATE, ///< مجموعة_انتظار / waitgroup_create — create waitgroup
                ASYNC_WG_ADD,    ///< أضف / waitgroup_add — increment counter
                ASYNC_WG_DONE,   ///< أنهي / waitgroup_done — decrement counter
                ASYNC_WG_WAIT,   ///< انتظر / waitgroup_wait — block until counter is 0
                ASYNC_WG_COUNT,  ///< العداد / waitgroup_count — read current counter

                // ========================================================================
                // Section 14b: LLVM Coroutine Opcodes / تعليمات كوروتين LLVM
                // ========================================================================
                // (AR) تعليمات خاصة لدعم غير_متزامن/انتظر عبر LLVM Coroutines
                // (EN) Special opcodes for async/await via LLVM Coroutines
                CORO_SUSPEND,      ///< انتظر / await — suspend coroutine at await point
                CORO_RETURN,       ///< ارجع من كوروتين / return from coroutine — store result in promise
                GENERATOR_YIELD,   ///< أنتج / yield — yield value from generator (store in promise + non-final suspend)
                GENERATOR_CONSUME, ///< استهلاك مولد / consume generator — resume-loop collecting all yields

                // ========================================================================
                // Section 15: عمليات وحدات نظام التشغيل المتقدمة / Advanced OS Module Operations
                // ========================================================================
                // (AR) هذا القسم يضيف دعم المترجم للمكتبة المنخفضة المستوى الكاملة (19 وحدة)
                //      للعمل في وضع بدون نظام تشغيل (freestanding/bare-metal)
                // (EN) This section adds compiler support for the full low-level library (19 modules)
                //      for freestanding/bare-metal OS development

                // --- 15a. وحدة المعالج المتقدمة / Advanced CPU Module (8) ---
                LOWLEVEL_CPU_GET_INFO,     ///< معلومات_المعالج / cpu_get_info — get full CPU info struct
                LOWLEVEL_CPU_GET_FEATURES, ///< ميزات_المعالج / cpu_get_features — get CPU feature flags
                LOWLEVEL_CPU_READ_MSR,     ///< اقرأ_سجل_نموذج / read_msr — read model-specific register
                LOWLEVEL_CPU_WRITE_MSR,    ///< اكتب_سجل_نموذج / write_msr — write model-specific register
                LOWLEVEL_CPU_READ_CR,      ///< اقرأ_سجل_تحكم / read_cr — read control register (0-4)
                LOWLEVEL_CPU_WRITE_CR,     ///< اكتب_سجل_تحكم / write_cr — write control register (0,3,4)
                LOWLEVEL_CPU_INVLPG,       ///< ابطل_صفحة / invlpg — invalidate TLB entry
                LOWLEVEL_CPU_GET_REPORT,   ///< تقرير_المعالج / cpu_report — full CPU diagnostic report
                LOWLEVEL_SYMBOL_ADDR,      ///< عنوان_رمز / symbol_addr — address of external linker symbol as integer

                // --- 15b. وحدة GDT / GDT Module (3) ---
                LOWLEVEL_GDT_INIT,       ///< هيئ_جدول_واصفات / gdt_init — initialize GDT
                LOWLEVEL_GDT_LOAD,       ///< حمل_جدول_واصفات / gdt_load — load GDT register (lgdt)
                LOWLEVEL_GDT_GET_REPORT, ///< تقرير_واصفات / gdt_report — GDT diagnostic report

                // --- 15c. وحدة الترحيل / Paging Module (5) ---
                LOWLEVEL_PAGING_INIT,       ///< هيئ_ترحيل / paging_init — initialize paging
                LOWLEVEL_PAGING_MAP,        ///< رحل_صفحة / paging_map — map virtual to physical page
                LOWLEVEL_PAGING_UNMAP,      ///< الغ_ترحيل / paging_unmap — unmap a virtual page
                LOWLEVEL_PAGING_FLUSH_TLB,  ///< افرغ_ذاكرة_ترجمة / paging_flush_tlb — flush entire TLB
                LOWLEVEL_PAGING_GET_REPORT, ///< تقرير_ترحيل / paging_report — paging diagnostic report

                // --- 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts Module (5) ---
                LOWLEVEL_IDT_INIT,         ///< هيئ_جدول_مقاطعات / idt_init — initialize IDT
                LOWLEVEL_IDT_LOAD,         ///< حمل_جدول_مقاطعات / idt_load — load IDT register (lidt)
                LOWLEVEL_IDT_REGISTER_ISR, ///< سجل_معالج_مقاطعة / register_isr — register ISR handler
                LOWLEVEL_IDT_ENABLE_IRQ,   ///< فعل_طلب_مقاطعة / enable_irq — enable IRQ line
                LOWLEVEL_IDT_GET_REPORT,   ///< تقرير_مقاطعات / idt_report — interrupt diagnostic report

                // --- 15e. وحدة PCI / PCI Module (5) ---
                LOWLEVEL_PCI_ENUMERATE,        ///< عدد_أجهزة_ناقل / pci_enumerate — enumerate PCI devices
                LOWLEVEL_PCI_READ_CONFIG,      ///< اقرأ_اعدادات_ناقل / pci_read_config — read PCI config register
                LOWLEVEL_PCI_WRITE_CONFIG,     ///< اكتب_اعدادات_ناقل / pci_write_config — write PCI config register
                LOWLEVEL_PCI_GET_DEVICE_COUNT, ///< عدد_الأجهزة / pci_device_count — get PCI device count
                LOWLEVEL_PCI_GET_REPORT,       ///< تقرير_ناقل / pci_report — PCI diagnostic report

                // --- 15f. وحدة DMA المتقدمة / Advanced DMA Module (4) ---
                LOWLEVEL_DMA_INIT,       ///< هيئ_نقل_مباشر / dma_init_full — initialize DMA controller
                LOWLEVEL_DMA_TRANSFER,   ///< ابدأ_نقل / dma_transfer — start DMA transfer
                LOWLEVEL_DMA_STATUS,     ///< حالة_نقل / dma_status — check DMA transfer status
                LOWLEVEL_DMA_GET_REPORT, ///< تقرير_نقل / dma_report — DMA diagnostic report

                // --- 15g. وحدة الشاشة / Framebuffer Module (8) ---
                LOWLEVEL_FB_INIT,        ///< هيئ_شاشة / fb_init — initialize framebuffer
                LOWLEVEL_FB_SET_PIXEL,   ///< ارسم_نقطة / fb_set_pixel — set pixel (x, y, color)
                LOWLEVEL_FB_DRAW_RECT,   ///< ارسم_مستطيل / fb_draw_rect — draw rectangle outline
                LOWLEVEL_FB_FILL_RECT,   ///< املأ_مستطيل / fb_fill_rect — fill rectangle
                LOWLEVEL_FB_DRAW_LINE,   ///< ارسم_خط / fb_draw_line — draw line
                LOWLEVEL_FB_DRAW_STRING, ///< ارسم_نص / fb_draw_string — draw text string
                LOWLEVEL_FB_CLEAR,       ///< امسح_شاشة / fb_clear — clear framebuffer
                LOWLEVEL_FB_GET_REPORT,  ///< تقرير_شاشة / fb_report — framebuffer diagnostic report

                // --- 15h. وحدة ACPI / ACPI Module (4) ---
                LOWLEVEL_ACPI_INIT,       ///< هيئ_طاقة / acpi_init — initialize ACPI
                LOWLEVEL_ACPI_FIND_TABLE, ///< ابحث_جدول_طاقة / acpi_find_table — find ACPI table
                LOWLEVEL_ACPI_SHUTDOWN,   ///< اطفئ / acpi_shutdown — power off system
                LOWLEVEL_ACPI_GET_REPORT, ///< تقرير_طاقة / acpi_report — ACPI diagnostic report

                // --- 15i. وحدة التزامن / Sync Module (8) ---
                LOWLEVEL_SPINLOCK_INIT,   ///< هيئ_قفل_دوار / spinlock_init — init spinlock
                LOWLEVEL_SPINLOCK_LOCK,   ///< اقفل_دوار / spinlock_lock — acquire spinlock
                LOWLEVEL_SPINLOCK_UNLOCK, ///< افتح_قفل_دوار / spinlock_unlock — release spinlock
                LOWLEVEL_MUTEX_INIT,      ///< هيئ_كابح / mutex_init — init mutex
                LOWLEVEL_MUTEX_LOCK,      ///< اقفل_كابح / mutex_lock — acquire mutex
                LOWLEVEL_MUTEX_UNLOCK,    ///< افتح_كابح / mutex_unlock — release mutex
                LOWLEVEL_SEMAPHORE_INIT,  ///< هيئ_اشارة / semaphore_init — init semaphore
                LOWLEVEL_BARRIER_INIT,    ///< هيئ_حاجز / barrier_init — init barrier

                // --- 15j. وحدة المجدول / Scheduler Module (6) ---
                LOWLEVEL_SCHED_INIT,          ///< هيئ_مجدول / sched_init — initialize scheduler
                LOWLEVEL_SCHED_CREATE_PROC,   ///< انشئ_عملية / sched_create_process — create process
                LOWLEVEL_SCHED_CREATE_THREAD, ///< انشئ_خيط_نواة / sched_create_thread — create kernel thread
                LOWLEVEL_SCHED_YIELD,         ///< تنازل / sched_yield — yield current thread
                LOWLEVEL_SCHED_SLEEP,         ///< نوم_مجدول / sched_sleep — sleep thread (ms)
                LOWLEVEL_SCHED_GET_REPORT,    ///< تقرير_مجدول / sched_report — scheduler diagnostic report

                // --- 15k. وحدة الإقلاع / Boot Module (3) ---
                LOWLEVEL_BOOT_GET_INFO,       ///< معلومات_اقلاع / boot_info — get boot information
                LOWLEVEL_BOOT_GET_MEMORY_MAP, ///< خريطة_ذاكرة_اقلاع / boot_memory_map — get boot memory map
                LOWLEVEL_BOOT_GET_REPORT,     ///< تقرير_اقلاع / boot_report — boot diagnostic report

                // --- 15l. وحدة نظام الملفات الافتراضي / VFS Module (7) ---
                LOWLEVEL_VFS_MOUNT,      ///< حمل_قرص / vfs_mount — mount filesystem
                LOWLEVEL_VFS_UNMOUNT,    ///< افصل_قرص / vfs_unmount — unmount filesystem
                LOWLEVEL_VFS_OPEN,       ///< افتح_ملف / vfs_open — open file
                LOWLEVEL_VFS_READ,       ///< اقرأ_ملف_نواة / vfs_read — read from file
                LOWLEVEL_VFS_WRITE,      ///< اكتب_ملف_نواة / vfs_write — write to file
                LOWLEVEL_VFS_CLOSE,      ///< اغلق_ملف_نواة / vfs_close — close file
                LOWLEVEL_VFS_GET_REPORT, ///< تقرير_ملفات / vfs_report — VFS diagnostic report

                // --- 15m. وحدة APIC / APIC Module (5) ---
                LOWLEVEL_APIC_INIT,       ///< هيئ_متحكم_مقاطعات / apic_init — initialize APIC
                LOWLEVEL_APIC_SEND_EOI,   ///< ارسل_نهاية_مقاطعة / apic_send_eoi — send End-Of-Interrupt
                LOWLEVEL_APIC_SEND_IPI,   ///< ارسل_مقاطعة_معالج / apic_send_ipi — send inter-processor interrupt
                LOWLEVEL_APIC_SET_TIMER,  ///< اضبط_مؤقت_متحكم / apic_set_timer — configure APIC timer
                LOWLEVEL_APIC_GET_REPORT, ///< تقرير_متحكم_مقاطعات / apic_report — APIC diagnostic report

                // --- 15n. وحدة HPET / HPET Module (4) ---
                LOWLEVEL_HPET_INIT,       ///< هيئ_مؤقت_دقيق / hpet_init — initialize HPET timer
                LOWLEVEL_HPET_READ,       ///< اقرأ_مؤقت_دقيق / hpet_read — read HPET counter
                LOWLEVEL_HPET_SLEEP,      ///< نوم_دقيق / hpet_sleep — precision sleep (nanoseconds)
                LOWLEVEL_HPET_GET_REPORT, ///< تقرير_مؤقت_دقيق / hpet_report — HPET diagnostic report

                // --- 15o. وحدة استدعاءات النظام / Syscall Module (4) ---
                LOWLEVEL_SYSCALL_INIT,       ///< هيئ_استدعاءات / syscall_init — initialize SYSCALL/SYSRET
                LOWLEVEL_SYSCALL_REGISTER,   ///< سجل_استدعاء / syscall_register — register syscall handler
                LOWLEVEL_SYSCALL_INVOKE,     ///< نفذ_استدعاء / syscall_invoke — invoke syscall by number
                LOWLEVEL_SYSCALL_GET_REPORT, ///< تقرير_استدعاءات / syscall_report — syscall diagnostic report

                // --- 15p. عمليات الذاكرة المتقدمة / Advanced Memory Operations (4) ---
                LOWLEVEL_MEM_ALLOC_PHYS, ///< خصص_فيزيائي / alloc_physical — allocate physical memory frame
                LOWLEVEL_MEM_FREE_PHYS,  ///< حرر_فيزيائي / free_physical — free physical memory frame
                LOWLEVEL_MEM_MAP_REGION, ///< رحل_منطقة / map_region — map memory region (MMIO)
                LOWLEVEL_MEM_GET_REPORT, ///< تقرير_ذاكرة_نواة / mem_report — memory diagnostic report

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
                LOWLEVEL_UEFI_ALLOC_PAGES,    ///< uefi_تخصيص_صفحات / uefi_allocate_pages
                LOWLEVEL_UEFI_FREE_PAGES,     ///< uefi_تحرير_صفحات / uefi_free_pages
                LOWLEVEL_UEFI_ALLOC_POOL,     ///< uefi_تخصيص_كتلة / uefi_allocate_pool
                LOWLEVEL_UEFI_FREE_POOL,      ///< uefi_تحرير_كتلة / uefi_free_pool
                LOWLEVEL_UEFI_GET_MEMORY_MAP, ///< uefi_خريطة_ذاكرة / uefi_get_memory_map
                LOWLEVEL_UEFI_GET_MEMMAP_KEY, ///< uefi_مفتاح_خريطة / uefi_get_memory_map_key
                LOWLEVEL_UEFI_TOTAL_MEMORY,   ///< uefi_ذاكرة_متاحة / uefi_total_memory

                // --- 16c. بروتوكول الرسوميات GOP / Graphics Output Protocol (10) ---
                LOWLEVEL_UEFI_INIT_GOP,         ///< uefi_تهيئة_رسوميات / uefi_init_gop
                LOWLEVEL_UEFI_SET_GOP_MODE,     ///< uefi_تعيين_وضع_رسوميات / uefi_set_gop_mode
                LOWLEVEL_UEFI_QUERY_GOP_MODE,   ///< uefi_استعلام_وضع / uefi_query_gop_mode
                LOWLEVEL_UEFI_GOP_MODE_COUNT,   ///< uefi_عدد_أوضاع_رسوميات / uefi_gop_mode_count
                LOWLEVEL_UEFI_CURRENT_GOP_MODE, ///< uefi_وضع_رسوميات_حالي / uefi_current_gop_mode
                LOWLEVEL_UEFI_FRAMEBUFFER_BASE, ///< uefi_عنوان_إطار / uefi_framebuffer_base
                LOWLEVEL_UEFI_FRAMEBUFFER_SIZE, ///< uefi_حجم_إطار / uefi_framebuffer_size
                LOWLEVEL_UEFI_FILL_SCREEN,      ///< uefi_ملء_شاشة / uefi_fill_screen
                LOWLEVEL_UEFI_DRAW_RECT,        ///< uefi_رسم_مستطيل / uefi_draw_rect
                LOWLEVEL_UEFI_GOP_BLT,          ///< uefi_blt / uefi_gop_blt — raw BLT operation

                // --- 16d. خدمات وقت التشغيل / Runtime Services (4) ---
                LOWLEVEL_UEFI_GET_TIME,     ///< uefi_الوقت / uefi_get_time
                LOWLEVEL_UEFI_SET_TIME,     ///< uefi_تعيين_وقت / uefi_set_time
                LOWLEVEL_UEFI_GET_VARIABLE, ///< uefi_قراءة_متغير / uefi_get_variable
                LOWLEVEL_UEFI_SET_VARIABLE, ///< uefi_كتابة_متغير / uefi_set_variable

                // --- 16e. نظام الملفات / File System (6) ---
                LOWLEVEL_UEFI_OPEN_VOLUME, ///< uefi_فتح_وحدة_تخزين / uefi_open_volume
                LOWLEVEL_UEFI_OPEN_FILE,   ///< uefi_فتح_ملف / uefi_open_file
                LOWLEVEL_UEFI_READ_FILE,   ///< uefi_قراءة_ملف / uefi_read_file
                LOWLEVEL_UEFI_WRITE_FILE,  ///< uefi_كتابة_ملف / uefi_write_file
                LOWLEVEL_UEFI_CLOSE_FILE,  ///< uefi_إغلاق_ملف / uefi_close_file
                LOWLEVEL_UEFI_FILE_INFO,   ///< uefi_معلومات_ملف / uefi_file_info

                // --- 16f. بروتوكولات ومعلومات / Protocols & System Info (5) ---
                LOWLEVEL_UEFI_LOCATE_PROTOCOL, ///< uefi_بحث_بروتوكول / uefi_locate_protocol
                LOWLEVEL_UEFI_REVISION,        ///< uefi_إصدار / uefi_revision
                LOWLEVEL_UEFI_VENDOR,          ///< uefi_بائع / uefi_firmware_vendor
                LOWLEVEL_UEFI_FW_REVISION,     ///< uefi_إصدار_بائع / uefi_firmware_revision
                LOWLEVEL_UEFI_REPORT,          ///< uefi_تقرير / uefi_report — comprehensive report

                // ============================================================================
                // القسم 17: ACPI الموسّع / Extended ACPI (Section 17)
                // ============================================================================

                // --- 17a. تهيئة وتفعيل ACPI / ACPI Init & Control (6) ---
                LOWLEVEL_ACPI_INIT_FULL,      ///< acpi_تهيئة / acpi_init — full initialization
                LOWLEVEL_ACPI_INIT_RSDP,      ///< acpi_تهيئة_من_rsdp / acpi_init_from_rsdp
                LOWLEVEL_ACPI_ENABLE,         ///< acpi_تفعيل / acpi_enable
                LOWLEVEL_ACPI_DISABLE,        ///< acpi_تعطيل / acpi_disable
                LOWLEVEL_ACPI_IS_INITIALIZED, ///< acpi_هل_مهيأ / acpi_is_initialized
                LOWLEVEL_ACPI_VERSION,        ///< acpi_إصدار / acpi_version

                // --- 17b. إدارة الطاقة / Power Management (3) ---
                LOWLEVEL_ACPI_REBOOT,   ///< acpi_إعادة_تشغيل / acpi_reboot
                LOWLEVEL_ACPI_SLEEP,    ///< acpi_نوم / acpi_sleep — enter sleep state
                LOWLEVEL_ACPI_DELAY_US, ///< acpi_تأخير / acpi_delay_us — microsecond delay

                // --- 17c. مؤقت ومعالجات / Timer & Processors (4) ---
                LOWLEVEL_ACPI_READ_PM_TIMER,   ///< acpi_قراءة_مؤقت / acpi_read_pm_timer
                LOWLEVEL_ACPI_IS_PM_32BIT,     ///< acpi_مؤقت_32بت / acpi_is_pm_timer_32bit
                LOWLEVEL_ACPI_PROCESSOR_COUNT, ///< acpi_عدد_معالجات / acpi_processor_count
                LOWLEVEL_ACPI_LOCAL_APIC_ADDR, ///< acpi_عنوان_apic / acpi_local_apic_address

                // --- 17d. PCIe ECAM ---
                LOWLEVEL_ACPI_ECAM_BASE, ///< acpi_ecam_قاعدة / acpi_ecam_base

                // ============================================================================
                // القسم 18: APIC الموسّع / Extended APIC (Section 18)
                // ============================================================================

                // --- 18a. استعلام ومعلومات / Query & Info (4) ---
                LOWLEVEL_APIC_SUPPORTED,    ///< apic_مدعوم / apic_supported
                LOWLEVEL_APIC_X2_SUPPORTED, ///< apic_x2_مدعوم / apic_x2_supported
                LOWLEVEL_APIC_ID,           ///< apic_معرّف / apic_id
                LOWLEVEL_APIC_IO_COUNT,     ///< apic_عدد_io / apic_io_count

                // --- 18b. مؤقت APIC / APIC Timer (5) ---
                LOWLEVEL_APIC_INIT_TIMER,  ///< apic_تهيئة_مؤقت / apic_init_timer
                LOWLEVEL_APIC_START_TIMER, ///< apic_بدء_مؤقت / apic_start_timer
                LOWLEVEL_APIC_STOP_TIMER,  ///< apic_إيقاف_مؤقت / apic_stop_timer
                LOWLEVEL_APIC_TIMER_COUNT, ///< apic_عداد_مؤقت / apic_timer_count
                LOWLEVEL_APIC_CALIBRATE,   ///< apic_معايرة_مؤقت / apic_calibrate_timer

                // --- 18c. أولوية ومقاطعات / Priority & Interrupts (5) ---
                LOWLEVEL_APIC_SET_PRIORITY, ///< apic_أولوية_مهمة / apic_set_priority
                LOWLEVEL_APIC_DISABLE_PIC,  ///< apic_عطّل_pic / apic_disable_pic
                LOWLEVEL_APIC_MASK_IRQ,     ///< apic_قناع_irq / apic_mask_irq
                LOWLEVEL_APIC_UNMASK_IRQ,   ///< apic_إلغاء_قناع_irq / apic_unmask_irq
                LOWLEVEL_APIC_ROUTE_IRQ,    ///< apic_وجّه_irq / apic_route_irq

                // --- 18d. IPI متقدم / Advanced IPI (5) ---
                LOWLEVEL_APIC_SEND_IPI_ALL,  ///< apic_أرسل_للكل / apic_send_ipi_all
                LOWLEVEL_APIC_SEND_INIT_IPI, ///< apic_أرسل_init / apic_send_init
                LOWLEVEL_APIC_SEND_SIPI,     ///< apic_أرسل_sipi / apic_send_sipi
                LOWLEVEL_APIC_WAIT_DELIVERY, ///< apic_انتظر_تسليم / apic_wait_delivery
                LOWLEVEL_APIC_INIT_IO,       ///< apic_تهيئة_io / apic_init_io

                // ============================================================================
                // القسم 19: عمليات أندرويد / Android Operations (Section 19)
                // ============================================================================
                // (AR) هذا القسم يضيف دعم المترجم لمنصة أندرويد بشكل أصلي
                //      بدون استخدام Java أو Kotlin — كل شيء بلغة ص
                // (EN) This section adds compiler support for native Android platform
                //      without Java or Kotlin — everything in Sad language

                // --- 19a. إدارة الذاكرة / Memory Management (2) ---
                ANDROID_ALLOC, ///< خصص_اندرويد / android_alloc — allocate memory
                ANDROID_FREE,  ///< حرر_اندرويد / android_free — free memory

                // --- 19b. النصوص / Strings (6) ---
                ANDROID_STRING_CREATE,  ///< نص_جديد_اندرويد / android_string_create
                ANDROID_STRING_CONCAT,  ///< دمج_نص_اندرويد / android_string_concat
                ANDROID_STRING_LENGTH,  ///< طول_نص_اندرويد / android_string_length
                ANDROID_STRING_SUBSTR,  ///< جزء_نص_اندرويد / android_string_substr
                ANDROID_STRING_COMPARE, ///< قارن_نص_اندرويد / android_string_compare
                ANDROID_STRING_FREE,    ///< حرر_نص_اندرويد / android_string_free

                // --- 19c. المصفوفات / Arrays (7) ---
                ANDROID_ARRAY_CREATE, ///< مصفوفة_جديدة_اندرويد / android_array_create
                ANDROID_ARRAY_GET,    ///< عنصر_مصفوفة_اندرويد / android_array_get
                ANDROID_ARRAY_SET,    ///< عين_عنصر_مصفوفة_اندرويد / android_array_set
                ANDROID_ARRAY_LENGTH, ///< طول_مصفوفة_اندرويد / android_array_length
                ANDROID_ARRAY_PUSH,   ///< اضف_عنصر_اندرويد / android_array_push
                ANDROID_ARRAY_POP,    ///< احذف_اخر_اندرويد / android_array_pop
                ANDROID_ARRAY_FREE,   ///< حرر_مصفوفة_اندرويد / android_array_free

                // --- 19d. الخرائط / Maps (7) ---
                ANDROID_MAP_CREATE, ///< خريطة_جديدة_اندرويد / android_map_create
                ANDROID_MAP_GET,    ///< قيمة_خريطة_اندرويد / android_map_get
                ANDROID_MAP_SET,    ///< عين_قيمة_خريطة_اندرويد / android_map_set
                ANDROID_MAP_HAS,    ///< موجود_خريطة_اندرويد / android_map_has
                ANDROID_MAP_DELETE, ///< احذف_من_خريطة_اندرويد / android_map_delete
                ANDROID_MAP_SIZE,   ///< حجم_خريطة_اندرويد / android_map_size
                ANDROID_MAP_FREE,   ///< حرر_خريطة_اندرويد / android_map_free

                // --- 19e. الشبكات / Network (8) ---
                ANDROID_NET_CONNECT,  ///< اتصل_شبكة_اندرويد / android_net_connect
                ANDROID_NET_SEND,     ///< ارسل_شبكة_اندرويد / android_net_send
                ANDROID_NET_RECV,     ///< استقبل_شبكة_اندرويد / android_net_recv
                ANDROID_NET_CLOSE,    ///< اغلق_شبكة_اندرويد / android_net_close
                ANDROID_HTTP_REQUEST, ///< طلب_http_اندرويد / android_http_request
                ANDROID_WS_CONNECT,   ///< اتصل_ويب_سوكت_اندرويد / android_ws_connect
                ANDROID_WS_SEND,      ///< ارسل_ويب_سوكت_اندرويد / android_ws_send
                ANDROID_WS_RECV,      ///< استقبل_ويب_سوكت_اندرويد / android_ws_recv

                // --- 19f. الخيوط والقنوات / Threads & Channels (8) ---
                ANDROID_THREAD_CREATE,  ///< انشئ_خيط_اندرويد / android_thread_create
                ANDROID_THREAD_JOIN,    ///< انضم_خيط_اندرويد / android_thread_join
                ANDROID_MUTEX_CREATE,   ///< انشئ_قفل_اندرويد / android_mutex_create
                ANDROID_MUTEX_LOCK,     ///< اقفل_اندرويد / android_mutex_lock
                ANDROID_MUTEX_UNLOCK,   ///< افتح_قفل_اندرويد / android_mutex_unlock
                ANDROID_CHANNEL_CREATE, ///< انشئ_قناة_اندرويد / android_channel_create
                ANDROID_CHANNEL_SEND,   ///< ارسل_قناة_اندرويد / android_channel_send
                ANDROID_CHANNEL_RECV,   ///< استقبل_قناة_اندرويد / android_channel_recv

                // --- 19g. واجهة المستخدم / UI (8) ---
                ANDROID_UI_INIT,          ///< هيئ_واجهة_اندرويد / android_ui_init
                ANDROID_UI_CREATE_WIDGET, ///< انشئ_عنصر_اندرويد / android_ui_create_widget
                ANDROID_UI_SET_TEXT,      ///< عين_نص_عنصر_اندرويد / android_ui_set_text
                ANDROID_UI_SET_CALLBACK,  ///< عين_دالة_استدعاء_اندرويد / android_ui_set_callback
                ANDROID_UI_SHOW,          ///< اظهر_عنصر_اندرويد / android_ui_show
                ANDROID_UI_HIDE,          ///< اخف_عنصر_اندرويد / android_ui_hide
                ANDROID_UI_UPDATE,        ///< حدث_واجهة_اندرويد / android_ui_update
                ANDROID_UI_RUN,           ///< شغل_واجهة_اندرويد / android_ui_run

                // --- 19h. الطباعة والتنقيح / Logging (2) ---
                ANDROID_LOG,   ///< سجل_اندرويد / android_log — Android logcat
                ANDROID_PRINT, ///< اطبع_اندرويد / android_print — console print

                // =====================================================================
                // 20. نظام الواجهة الموحد / Unified UI System (sad_ui.h)
                // =====================================================================
                // (AR) أوامر إنشاء العناصر المرئية — كل دالة ترجع مؤشراً لعنصر SadWidget*
                // (EN) Widget factory builtins — each returns a SadWidget* pointer

                // --- 20a. مصانع العناصر / Widget Factories (20) ---
                BUILTIN_UI_COLUMN,         ///< عمود() / sad_column() — حاوية عمودية
                BUILTIN_UI_ROW,            ///< صف() / sad_row() — حاوية أفقية
                BUILTIN_UI_STACK,          ///< مكدس() / sad_stack() — تراكب عناصر
                BUILTIN_UI_CONTAINER,      ///< حاوية() / sad_container() — حاوية عامة
                BUILTIN_UI_TEXT,           ///< نص_عرض(نص) / sad_text(text) — عنصر نصي
                BUILTIN_UI_TEXT_STYLED,    ///< نص_منسق(نص,حجم,لون) / sad_text_styled(text,size,color) — نص بتنسيق
                BUILTIN_UI_BUTTON,         ///< زر(عنوان,دالة,بيانات) / sad_button(label,cb,data)
                BUILTIN_UI_BUTTON_VARIANT, ///< زر_نوع(عنوان,نوع,لون,دالة,بيانات) / sad_button_variant(...)
                BUILTIN_UI_ICON_BUTTON,    ///< زر_ايقونة(ايقونة,دالة,بيانات) / sad_icon_button(icon,cb,data)
                BUILTIN_UI_FAB,            ///< زر_عائم(ايقونة,لون,دالة,بيانات) / sad_fab(icon,color,cb,data)
                BUILTIN_UI_TEXT_FIELD,     ///< حقل_نص(تلميح,دالة,بيانات) / sad_text_field(hint,cb,data)
                BUILTIN_UI_CHECKBOX,       ///< مربع_تحقق(دالة,بيانات) / sad_checkbox(cb,data)
                BUILTIN_UI_SWITCH,         ///< مبدل(دالة,بيانات) / sad_switch_toggle(cb,data)
                BUILTIN_UI_SLIDER,         ///< منزلق(حد_أدنى,حد_أقصى,دالة,بيانات) / sad_slider(min,max,cb,data)
                BUILTIN_UI_CARD,           ///< بطاقة() / sad_card()
                BUILTIN_UI_SCAFFOLD,       ///< هيكل() / sad_scaffold()
                BUILTIN_UI_APP_BAR,        ///< شريط_تطبيق(عنوان) / sad_app_bar(title)
                BUILTIN_UI_SPACER,         ///< فاصل() / sad_spacer()
                BUILTIN_UI_DIVIDER,        ///< خط_فاصل() / sad_divider()
                BUILTIN_UI_DIALOG,         ///< حوار(عنوان,رسالة) / sad_dialog(title,msg)

                // --- 20a-ب. مصانع إضافيّة (م-مصانع، RFC sadlang-rfcs#1) — سدّ فجوة
                //          المترجم: 24 عنصرًا كان المفسّر يدعمها والمترجم لا. تُطابق
                //          تسجيل المفسّر في widget_builtins.cpp عنصرًا بعنصر. ---
                BUILTIN_UI_IMAGE,          ///< صورة(مصدر) / sad_image(source)
                BUILTIN_UI_ICON,           ///< أيقونة(اسم) / sad_icon(name)
                BUILTIN_UI_TEXT_BUTTON,    ///< زر_نصي(عنوان) / sad_text_button(label,cb,data)
                BUILTIN_UI_GRID,           ///< شبكة() / sad_grid()
                BUILTIN_UI_CENTER,         ///< وسط() / sad_center()
                BUILTIN_UI_PADDING,        ///< حشوة() / sad_padding()
                BUILTIN_UI_ALIGN,          ///< محاذاة() / sad_align()
                BUILTIN_UI_EXPANDED,       ///< موسع() / sad_expanded()
                BUILTIN_UI_FLEXIBLE,       ///< مرن() / sad_flexible()
                BUILTIN_UI_SIZED_BOX,      ///< مقاس(عرض,ارتفاع) / sad_sized_box(w,h)
                BUILTIN_UI_WRAP,           ///< التفاف() / sad_wrap()
                BUILTIN_UI_BOX,            ///< صندوق() / sad_box()
                BUILTIN_UI_SCROLL_VIEW,    ///< عرض_تمرير() / sad_scroll_view()
                BUILTIN_UI_BOTTOM_NAV,     ///< تنقل_سفلي() / sad_bottom_nav()
                BUILTIN_UI_SNACKBAR,       ///< شريط_إشعار(رسالة) / sad_snackbar(msg)
                BUILTIN_UI_TOOLTIP,        ///< تلميح(نص) / sad_tooltip(text)
                BUILTIN_UI_PROGRESS,       ///< شريط_تقدم(قيمة) / sad_progress(value)
                BUILTIN_UI_LAZY_COLUMN,    ///< عمود_كسول() / sad_lazy_column()
                BUILTIN_UI_LAZY_ROW,       ///< صف_كسول() / sad_lazy_row()
                BUILTIN_UI_LIST_VIEW,      ///< قائمة() / sad_list_view()
                BUILTIN_UI_TEXT_AREA,      ///< منطقة_نص(تلميح) / sad_text_area(hint)
                BUILTIN_UI_DRAWER,         ///< درج() / sad_drawer()
                BUILTIN_UI_SAFE_AREA,      ///< منطقة_آمنة() / sad_safe_area()
                BUILTIN_UI_SURFACE,        ///< سطح() / sad_surface()
                // ── تحكّم متقدّم + تغذية راجعة (م٥-ب/م٦-ب) ──
                BUILTIN_UI_SPIN_BOX,          ///< مربع_دوار() / sad_spin_box()
                BUILTIN_UI_GROUP_BOX,         ///< صندوق_تجميع() / sad_group_box()
                BUILTIN_UI_SPINNER,           ///< مؤشر_انشغال() / sad_spinner()
                BUILTIN_UI_STATUS_BAR,        ///< شريط_حالة() / sad_status_bar()
                // ── ودجات سطح المكتب (م١–م٦) / Desktop-shell widgets ──
                BUILTIN_UI_WINDOW,            ///< نافذة() / sad_window()
                BUILTIN_UI_TITLE_BAR,         ///< شريط_عنوان() / sad_title_bar()
                BUILTIN_UI_SCROLL_BAR,        ///< شريط_تمرير() / sad_scroll_bar()
                BUILTIN_UI_TASKBAR,           ///< شريط_مهام() / sad_taskbar()
                BUILTIN_UI_START_MENU,        ///< قائمة_ابدأ() / sad_start_menu()
                BUILTIN_UI_SYSTEM_TRAY,       ///< لوحة_نظام() / sad_system_tray()
                BUILTIN_UI_MENU,              ///< قائمة_خيارات() / sad_menu()
                BUILTIN_UI_TOOLBAR,           ///< شريط_أدوات() / sad_toolbar()
                BUILTIN_UI_SEGMENTED_CONTROL, ///< تحكم_مقسم() / sad_segmented_control()
                BUILTIN_UI_TREE_VIEW,         ///< عرض_شجري() / sad_tree_view()
                BUILTIN_UI_SPLIT_VIEW,        ///< لوح_مقسم() / sad_split_view()
                BUILTIN_UI_DATA_TABLE,        ///< جدول_بيانات() / sad_data_table()
                BUILTIN_UI_BREADCRUMB,        ///< مسار_تنقل() / sad_breadcrumb()
                BUILTIN_UI_PICKER,            ///< قائمة_منسدلة() / sad_picker()
                BUILTIN_UI_DATE_PICKER,       ///< منتقي_تاريخ() / sad_date_picker()
                BUILTIN_UI_STEPPER,           ///< خطوات() / sad_stepper()
                BUILTIN_UI_RATING_BAR,        ///< تقييم() / sad_rating_bar()
                BUILTIN_UI_ALERT,             ///< تنبيه() / sad_alert()

                // --- 20b. إدارة الشجرة / Tree Management (3) ---
                BUILTIN_UI_ADD_CHILD,      ///< أضف_ابن(أب,ابن) / sad_add_child(parent,child)
                BUILTIN_UI_REMOVE_CHILD,   ///< أزل_ابن(أب,ابن) / sad_remove_child(parent,child)
                BUILTIN_UI_CLEAR_CHILDREN, ///< امسح_الأبناء(عنصر) / sad_clear_children(widget)

                // --- 20c. ضبط الخصائص / Property Setters (12) ---
                BUILTIN_UI_SET_TEXT,       ///< عين_النص(عنصر,نص) / sad_set_text(w,text)
                BUILTIN_UI_SET_SIZE,       ///< عين_الحجم(عنصر,عرض,ارتفاع) / sad_set_size(w,w,h)
                BUILTIN_UI_SET_FLEX,       ///< عين_المرونة(عنصر,مرونة) / sad_set_flex(w,flex)
                BUILTIN_UI_SET_BACKGROUND, ///< عين_الخلفية(عنصر,أحمر,أخضر,أزرق,شفافية) / sad_set_background(w,r,g,b,a)
                BUILTIN_UI_SET_FOREGROUND, ///< عين_اللون(عنصر,أحمر,أخضر,أزرق,شفافية) / sad_set_foreground(w,r,g,b,a)
                BUILTIN_UI_SET_SPACING,    ///< عين_التباعد(عنصر,تباعد) / sad_set_spacing(w,spacing)
                BUILTIN_UI_SET_PADDING,    ///< عين_الحشوة(عنصر,فوق,يمين,تحت,يسار) / sad_set_padding(w,t,r,b,l)
                BUILTIN_UI_SET_ALIGNMENT,  ///< عين_المحاذاة(عنصر,رئيسية,فرعية) / sad_set_alignment(w,main,cross)
                BUILTIN_UI_SET_BORDER,     ///< عين_الحدود(عنصر,عرض,أحمر,أخضر,أزرق,شفافية) / sad_set_border(w,width) + color
                BUILTIN_UI_SET_ELEVATION,  ///< عين_الارتفاع(عنصر,ارتفاع) / sad_set_elevation(w,elev)
                BUILTIN_UI_SET_OPACITY,    ///< عين_الشفافية(عنصر,شفافية) / sad_set_opacity(w,opacity)
                BUILTIN_UI_SET_VISIBILITY, ///< عين_الظهور(عنصر,مرئي) / sad_set_visibility(w,visible)

                // --- 20d. إدارة التطبيق / App Management (7) ---
                BUILTIN_UI_APP_CREATE,     ///< انشئ_تطبيق() / sad_app_create()
                BUILTIN_UI_APP_SET_ROOT,   ///< عين_الجذر(تطبيق,عنصر) / sad_app_set_root(app,widget)
                BUILTIN_UI_APP_LAYOUT,     ///< خطط(تطبيق,عرض,ارتفاع) / sad_app_layout(app,w,h)
                BUILTIN_UI_APP_RENDER,     ///< ارسم(تطبيق) / sad_app_render(app)
                BUILTIN_UI_APP_RUN,        ///< تشغيل_تطبيق(عنصر) / sad_app_run(root) — حلقة + إرسال
                BUILTIN_UI_PRINT_TREE,     ///< طباعة_شجرة(عنصر) / sad_print_tree(root) — تصحيح
                BUILTIN_UI_APP_DESTROY,    ///< دمر_تطبيق(تطبيق) / sad_app_destroy(app)
                BUILTIN_UI_WIDGET_DESTROY, ///< دمر_عنصر(عنصر) / sad_widget_destroy(widget)

                // --- 20h. م-تحكّم UICore: الثيم — جسرٌ فوق حالة الثيم المكتبيّة
                //          المشتركة (sad::ui::*)، نظير دوال المفسّر ذاتها. ---
                BUILTIN_UI_TOGGLE_THEME,   ///< تبديل_الثيم() / sad_toggle_theme()
                BUILTIN_UI_DARK_MODE,      ///< وضع_داكن() / sad_set_dark()
                BUILTIN_UI_LIGHT_MODE,     ///< وضع_فاتح() / sad_set_light()
                BUILTIN_UI_IS_DARK,        ///< هل_داكن() / sad_is_dark() → bool
                BUILTIN_UI_NEXT_KEY,       ///< التقط_مفتاح() / sad_next_key() → int (رمز مفتاح منتظِر أو 0)

                // --- 20i. م-تحكّم UICore: التنقّل — جسرٌ فوق مكدّس التنقّل المكتبيّ
                //          المشترك (sad::ui::nav). عمق-فقط في هذه الشريحة. ---
                BUILTIN_UI_NAVIGATE,       ///< انتقل(صفحة) / sad_navigate(page)
                BUILTIN_UI_NAV_BACK,       ///< عودة() / sad_navigate_back()
                BUILTIN_UI_NAV_ROOT,       ///< عودة_للبداية() / sad_navigate_root()
                BUILTIN_UI_REPLACE_PAGE,   ///< استبدل(صفحة) / sad_replace_page(page)
                BUILTIN_UI_PAGE_COUNT,     ///< عدد_الصفحات() / sad_page_count() → i64
                BUILTIN_UI_CURRENT_PAGE,   ///< الصفحة_الحالية() / sad_current_page() → SadWidget
                // --- 20j. م2: التنقّل بتحريك بصريّ ---
                BUILTIN_UI_NAVIGATE_TRANSITION, ///< انتقل_بتحريك(صفحة,نوع,مدة) / sad_navigate_transition
                BUILTIN_UI_BACK_TRANSITION,      ///< عودة_بتحريك(نوع,مدة) / sad_navigate_back_transition
                // --- 20k. إكمال corui: انتقال كامل + الحالة + النافذة + توليد ويب ---
                BUILTIN_UI_NAVIGATE_EXIT_TRANSITION, ///< انتقل_بتحريك_كامل(صفحة,دخول,خروج,مدة) / sad_navigate_exit_transition
                BUILTIN_UI_UPDATE_STATE,             ///< تحديث_حالة() / sad_update_state — يعلّم إعادة رسم
                BUILTIN_UI_STOP_PROPAGATION,         ///< أوقف_الانتشار() / sad_stop_propagation — يوقف انتشار الحدث
                BUILTIN_UI_SET_STATE,                ///< عين_الحالة(دالّة) / نداء إغلاق + sad_update_state
                BUILTIN_UI_SET_TITLE,                ///< عنوان_النافذة(نص) / sad_set_window_title
                BUILTIN_UI_CLOSE_WINDOW,             ///< أغلق_النافذة() / sad_close_window
                BUILTIN_UI_GEN_WEB,                  ///< توليد_ويب(عنصر,عنوان؟) / sad_generate_web → String

                // --- 20e. أثر المعدّلات الانسيابيّة (م-أ3ر) — خاصّيّة عامّة بالاسم
                //          نظير setIRProperty في المفسّر. اسم الطريقة = اسم الخاصيّة. ---
                BUILTIN_UI_SET_PROP_STR,   ///< .م(نص) → sad_set_prop_str(w,name,val)
                BUILTIN_UI_SET_PROP_INT,   ///< .م(صحيح) → sad_set_prop_int(w,name,val) — int64_t كالمفسّر
                BUILTIN_UI_SET_PROP_NUM,   ///< .م(عشري) → sad_set_prop_num(w,name,val)
                BUILTIN_UI_SET_PROP_BOOL,  ///< .م() / .م(منطقي) → sad_set_prop_bool(w,name,val)
                BUILTIN_UI_ADD_EVENT,      ///< .عند_*(دالة) → sad_add_event(w,name,cb,data) — نظير addIREvent
                // --- 20f. سلسلة التحريك (م-أ3ر، L3) — آلة حالة فوق IRNode ---
                BUILTIN_UI_ANIM_BEGIN,        ///< .حرّك(أنواع) → sad_anim_begin(w,csv) — يبدأ مجموعة (مركّب بالفاصلة)
                BUILTIN_UI_ANIM_DURATION,     ///< .مدة(ث) → sad_anim_duration(w,sec)
                BUILTIN_UI_ANIM_EASING,       ///< .منحنى(اسم) → sad_anim_easing(w,name)
                BUILTIN_UI_SET_EVENT_PHASE,   ///< .تفرع(طور) → sad_set_event_phase(w,phase) — طور آخر معالِج
                BUILTIN_UI_SET_EVENT_DATA,    ///< .عند_*(د،بيانات) → sad_set_event_data(w,str) — «بيانات» آخر معالِج
                BUILTIN_UI_ANIM_DELAY,        ///< .تأخير(ث) → sad_anim_delay(w,sec)
                BUILTIN_UI_ANIM_REPEAT,       ///< .تكرار(ن) → sad_anim_repeat(w,count)
                BUILTIN_UI_ANIM_AUTO_REVERSE, ///< .عكس_تلقائي([منطقي]) → sad_anim_auto_reverse(w,bool)
                // --- 20g. خاصّيّة متعدّدة الوسائط (م-أ3ر) — قيمٌ مفصولة بفواصل ---
                BUILTIN_UI_PROP_JOIN_ADD,     ///< وسيطٌ يُنسَّق ويُضاف لمجمِّع العنصر (sad_prop_join_add_*)
                BUILTIN_UI_PROP_JOIN_COMMIT,  ///< يدمج المجمِّع بفواصل ⇒ خاصّيّة نصّيّة (sad_prop_join_commit)

                // ==========================================
                // التوجيهات / Directives (@حجم, @ذري, etc.)
                // ==========================================
                Sizeof,         ///< @حجم(نوع) / sizeof(type) — get size of type in bytes
                AtomicLoad,     ///< @ذري(تحميل, ptr) / atomic load
                AtomicStore,    ///< @ذري(تخزين, ptr, val) / atomic store
                AtomicAdd,      ///< @ذري(إضافة, ptr, val) / atomic add
                AtomicSub,      ///< @ذري(طرح, ptr, val) / atomic sub
                AtomicExchange, ///< @ذري(تبادل, ptr, val) / atomic exchange
                AtomicCmpXchg,  ///< @ذري(مقارنة_وتبديل, ptr, exp, des) / atomic CAS

                // ==========================================
                // نظام الوحدات / Module System
                // ==========================================
                MODULE_LOAD,   ///< تحميل وحدة: %m = ModuleLoad("path") / Load module
                MODULE_SYMBOL, ///< استيراد رمز: %s = ModuleSymbol(%m, "name") / Import symbol from module
                MODULE_EXPORT, ///< تصدير رمز: ModuleExport("name", %v) / Export symbol
                MODULE_INIT,   ///< تهيئة وحدة: ModuleInit(%m) / Initialize module

                // ==========================================
                // الإغلاقات / Closures
                // ==========================================
                CLOSURE_CREATE, ///< إنشاء بنية إغلاق على الكومة: %r = closure_create @fn, cap0, cap1, ...
                                ///< Creates closure struct {fn_ptr, env_ptr} on heap
                                ///< operands[0] = Function (@fn_name)
                                ///< operands[1..N] = captured values (packed into env array)
                                ///< Result: i64 pointer to closure struct

                CLOSURE_CALL, ///< استدعاء إغلاق: %r = closure_call %closure, arg0, arg1, ...
                              ///< Extracts fn_ptr and env_ptr from closure struct
                              ///< Calls fn_ptr(args..., env_ptr)
                              ///< operands[0] = closure pointer (i64)
                              ///< operands[1..N] = explicit arguments

                ENV_LOAD, ///< تحميل متغير ملتقط من البيئة: %r = env_load %env, INDEX
                          ///< Loads captured variable from environment array by index
                          ///< operands[0] = env pointer (i64, from __env param)
                          ///< operands[1] = index (constant i64)
                          ///< Result: captured value (i64)

                // ================================================================
                // (AR) [Fix #51] ENV_STORE — تخزين قيمة في مصفوفة بيئة الإغلاق
                //      يُستخدم عند إسناد متغير ملتقط داخل اللامدا
                //      لضمان أن التعديل ينعكس على env المشترك بين الاستدعاءات
                // (EN) [Fix #51] ENV_STORE — Store value into closure env array
                //      Used when assigning to a captured variable inside lambda
                //      Ensures mutations persist across closure calls
                // ================================================================
                ENV_STORE, ///< تخزين قيمة في بيئة الإغلاق: env_store %value, %env, INDEX
                           ///< Stores value into environment array by index
                           ///< operands[0] = value to store (i64)
                           ///< operands[1] = env pointer (i64, from __env param)
                           ///< operands[2] = index (constant i64)

                // ========================================================================
                // القسم 21: وحدة الشبكة — مقابس TCP/UDP وعميل/خادم HTTP
                // Section 21: Network Module — TCP/UDP sockets and HTTP client/server
                // ========================================================================
                // (AR) هذا القسم يدعم مكتبة الشبكة الكاملة في المترجم (sadc).
                //      كل opcode يُترجم إلى استدعاء دالة C API من features/network/core.
                //      الأسماء العربية مُعرّفة في shared/builtins/include/builtin_registry.h
                // (EN) This section supports the full network library in the compiler (sadc).
                //      Each opcode maps to a C API call from features/network/core.
                //      Arabic names are defined in shared/builtins/include/builtin_registry.h

                // --- 21a. مقابس TCP / TCP Sockets (10) ---
                BUILTIN_NET_TCP_NEW,         ///< أنشئ_وصلة / sad_tcp_socket_new
                BUILTIN_NET_TCP_CONNECT,     ///< اتصل_بـ / sad_tcp_connect
                BUILTIN_NET_TCP_SEND,        ///< أرسل_عبر_وصلة / sad_tcp_send
                BUILTIN_NET_TCP_RECV,        ///< استقبل_عبر_وصلة / sad_tcp_recv
                BUILTIN_NET_TCP_CLOSE,       ///< أغلق_وصلة / sad_tcp_close
                BUILTIN_NET_TCP_BIND,        ///< خصص_منفذ / sad_tcp_bind
                BUILTIN_NET_TCP_LISTEN,      ///< انتظر_اتصالات / sad_tcp_listen
                BUILTIN_NET_TCP_ACCEPT,      ///< اقبل_متصل / sad_tcp_accept
                BUILTIN_NET_TCP_CONNECTED,   ///< هل_متصلة / sad_tcp_is_connected
                BUILTIN_NET_TCP_REMOTE_ADDR, ///< عنوان_الطرف_الآخر / sad_tcp_remote_addr

                // --- 21b. مقابس UDP / UDP Sockets (5) ---
                BUILTIN_NET_UDP_NEW,   ///< أنشئ_مرسال / sad_udp_socket_new
                BUILTIN_NET_UDP_BIND,  ///< خصص_منفذ_رسائل / sad_udp_bind
                BUILTIN_NET_UDP_SEND,  ///< أرسل_رسالة / sad_udp_send_to
                BUILTIN_NET_UDP_RECV,  ///< استقبل_رسالة / sad_udp_recv_from
                BUILTIN_NET_UDP_CLOSE, ///< أغلق_مرسال / sad_udp_close

                // --- 21c. إعدادات المقابس / Socket Settings (3) ---
                BUILTIN_NET_RECV_TIMEOUT, ///< حدد_انتظار_الاستقبال / sad_socket_set_recv_timeout
                BUILTIN_NET_SEND_TIMEOUT, ///< حدد_انتظار_الإرسال / sad_socket_set_send_timeout
                BUILTIN_NET_NO_DELAY,     ///< أرسل_فوراً / sad_tcp_set_nodelay
                BUILTIN_NET_LOCAL_PORT,   ///< منفذ_محلي / sad_socket_local_port

                // --- 21d. عميل HTTP — إنشاء وطلبات / HTTP Client (13) ---
                BUILTIN_NET_HTTP_NEW_CLIENT,  ///< أنشئ_متصفح / sad_http_client_new
                BUILTIN_NET_HTTP_FREE_CLIENT, ///< أغلق_متصفح / sad_http_client_free
                BUILTIN_NET_HTTP_GET,         ///< اجلب / sad_http_get
                BUILTIN_NET_HTTP_POST,        ///< أرسل / sad_http_post
                BUILTIN_NET_HTTP_PUT,         ///< استبدل / sad_http_put
                BUILTIN_NET_HTTP_DELETE,      ///< احذف_مورد / sad_http_delete
                BUILTIN_NET_HTTP_PATCH,       ///< عدّل_مورد / sad_http_patch
                BUILTIN_NET_HTTP_SET_BASE,    ///< حدد_الموقع / sad_http_client_set_base_url
                BUILTIN_NET_HTTP_SET_HEADER,  ///< أضف_ترويسة / sad_http_client_set_header
                BUILTIN_NET_HTTP_SET_TIMEOUT, ///< حدد_الانتظار / sad_http_client_set_timeout
                BUILTIN_NET_HTTP_SET_BEARER,  ///< سجّل_دخول_برمز / sad_http_client_set_bearer_token
                BUILTIN_NET_HTTP_LAST_ERROR,  ///< سبب_الفشل / sad_http_client_last_error
                BUILTIN_NET_HTTP_IS_OK,       ///< هل_نجح / sad_http_client_is_ok

                // --- 21e. استجابة HTTP / HTTP Response (5) ---
                BUILTIN_NET_RESP_FREE,    ///< تجاهل_الرد / sad_http_response_free
                BUILTIN_NET_RESP_STATUS,  ///< رمز_الحالة / sad_http_response_status
                BUILTIN_NET_RESP_BODY,    ///< نص_الرد / sad_http_response_body
                BUILTIN_NET_RESP_HEADER,  ///< معلومة_الرد / sad_http_response_header
                BUILTIN_NET_RESP_SUCCESS, ///< هل_الرد_ناجح / sad_http_response_is_success

                // --- 21f. خادم HTTP / HTTP Server (9) ---
                BUILTIN_NET_SRV_NEW,       ///< أنشئ_خادم / sad_http_server_new
                BUILTIN_NET_SRV_FREE,      ///< أزل_خادم / sad_http_server_free
                BUILTIN_NET_SRV_ON_GET,    ///< عند_طلب_جلب / sad_http_server_get_cb
                BUILTIN_NET_SRV_ON_POST,   ///< عند_طلب_إرسال / sad_http_server_post_cb
                BUILTIN_NET_SRV_ON_PUT,    ///< عند_طلب_استبدال / sad_http_server_put_cb
                BUILTIN_NET_SRV_ON_DELETE, ///< عند_طلب_حذف / sad_http_server_delete_cb
                BUILTIN_NET_SRV_LISTEN,    ///< ابدأ_الاستماع / sad_http_server_listen
                BUILTIN_NET_SRV_STOP,      ///< أوقف_الخادم / sad_http_server_stop
                BUILTIN_NET_SRV_CORS,      ///< اسمح_بالوصول_الخارجي / sad_http_server_enable_cors

                // --- 21g. بيانات الطلب الوارد / Incoming Request (5) ---
                BUILTIN_NET_REQ_METHOD, ///< نوع_الطلب / sad_http_request_method
                BUILTIN_NET_REQ_PATH,   ///< وجهة_الطلب / sad_http_request_path
                BUILTIN_NET_REQ_BODY,   ///< بيانات_الطلب / sad_http_request_body
                BUILTIN_NET_REQ_HEADER, ///< معلومة_الطلب / sad_http_request_header
                BUILTIN_NET_REQ_QUERY,  ///< قيمة_من_الرابط / sad_http_request_query_param

                // --- 21h. بناء الاستجابة / Response Building (5) ---
                BUILTIN_NET_RESP_SET_STATUS, ///< عيّن_حالة_الرد / sad_http_response_set_status
                BUILTIN_NET_RESP_SET_BODY,   ///< عيّن_نص_الرد / sad_http_response_set_body
                BUILTIN_NET_RESP_SET_JSON,   ///< عيّن_رد_جيسون / sad_http_response_set_json
                BUILTIN_NET_RESP_SET_HTML,   ///< عيّن_رد_صفحة / sad_http_response_set_html
                BUILTIN_NET_RESP_SET_HEADER, ///< عيّن_ترويسة_الرد / sad_http_response_set_header

                // --- 21i. أدوات الشبكة العامة / Network Utilities (4) ---
                BUILTIN_NET_INIT,       ///< جهّز_الشبكة / sad_network_init
                BUILTIN_NET_CLEANUP,    ///< نظّف_الشبكة / sad_network_cleanup
                BUILTIN_NET_ERROR_CODE, ///< رمز_آخر_خطأ / sad_network_last_error_code
                BUILTIN_NET_ERROR_MSG,  ///< وصف_آخر_خطأ / sad_network_last_error_message

                // --- 21j. العناوين / Network Addresses (8) ---
                BUILTIN_NET_ADDR_NEW,    ///< عنوان / sad_address_new
                BUILTIN_NET_ADDR_NEW_V6, ///< عنوان_حديث / sad_address_new_v6
                BUILTIN_NET_ADDR_FREE,   ///< حرر_عنوان / sad_address_free
                BUILTIN_NET_ADDR_IP,     ///< رقم_الجهاز / sad_address_ip
                BUILTIN_NET_ADDR_PORT,   ///< رقم_المنفذ / sad_address_port
                BUILTIN_NET_ADDR_IS_V4,  ///< هل_عنوان_قديم / sad_address_is_v4
                BUILTIN_NET_ADDR_IS_V6,  ///< هل_عنوان_حديث / sad_address_is_v6
                BUILTIN_NET_ADDR_STR,    ///< العنوان_كنص / sad_address_to_string

                // --- 21k. عميل WebSocket / WebSocket Client (13) ---
                BUILTIN_NET_WS_CLIENT_NEW,          ///< أنشئ_عميل_ويبسوكت / sad_ws_client_new
                BUILTIN_NET_WS_CLIENT_FREE,         ///< أزل_عميل_ويبسوكت / sad_ws_client_free
                BUILTIN_NET_WS_CLIENT_CONNECT,      ///< اتصل_بويبسوكت / sad_ws_client_connect
                BUILTIN_NET_WS_CLIENT_CLOSE,        ///< أغلق_ويبسوكت / sad_ws_client_close
                BUILTIN_NET_WS_CLIENT_IS_CONNECTED, ///< هل_متصل_ويبسوكت / sad_ws_client_is_connected
                BUILTIN_NET_WS_CLIENT_SEND,         ///< أرسل_ويبسوكت / sad_ws_client_send
                BUILTIN_NET_WS_CLIENT_PING,         ///< نبض_ويبسوكت / sad_ws_client_ping
                BUILTIN_NET_WS_CLIENT_RECEIVE,      ///< استقبل_ويبسوكت / sad_ws_client_receive
                BUILTIN_NET_WS_CLIENT_HAS_MESSAGE,  ///< يوجد_رسالة_ويبسوكت / sad_ws_client_has_message
                BUILTIN_NET_WS_CLIENT_GET_URL,      ///< رابط_ويبسوكت / sad_ws_client_get_url
                BUILTIN_NET_WS_CLIENT_LAST_ERROR,   ///< خطأ_ويبسوكت / sad_ws_client_last_error
                BUILTIN_NET_WS_CLIENT_RECV_TIMEOUT, ///< حدد_مهلة_استقبال_ويبسوكت / sad_ws_client_set_recv_timeout
                BUILTIN_NET_WS_CLIENT_SEND_TIMEOUT, ///< حدد_مهلة_إرسال_ويبسوكت / sad_ws_client_set_send_timeout

                // --- 21l. خادم WebSocket / WebSocket Server (12) ---
                BUILTIN_NET_WS_SERVER_NEW,            ///< أنشئ_خادم_ويبسوكت / sad_ws_server_new
                BUILTIN_NET_WS_SERVER_FREE,           ///< أزل_خادم_ويبسوكت / sad_ws_server_free
                BUILTIN_NET_WS_SERVER_START,          ///< ابدأ_خادم_ويبسوكت / sad_ws_server_start
                BUILTIN_NET_WS_SERVER_STOP,           ///< أوقف_خادم_ويبسوكت / sad_ws_server_stop
                BUILTIN_NET_WS_SERVER_IS_RUNNING,     ///< هل_يعمل_خادم_ويبسوكت / sad_ws_server_is_running
                BUILTIN_NET_WS_SERVER_CONN_COUNT,     ///< عدد_اتصالات_ويبسوكت / sad_ws_server_connection_count
                BUILTIN_NET_WS_SERVER_GET_PORT,       ///< منفذ_خادم_ويبسوكت / sad_ws_server_get_port
                BUILTIN_NET_WS_SERVER_BROADCAST,      ///< بث_ويبسوكت / sad_ws_server_broadcast
                BUILTIN_NET_WS_SERVER_BROADCAST_ROOM, ///< بث_لغرفة_ويبسوكت / sad_ws_server_broadcast_to_room
                BUILTIN_NET_WS_SERVER_ROOM_COUNT,     ///< عدد_غرف_ويبسوكت / sad_ws_server_room_count
                BUILTIN_NET_WS_SERVER_ROOM_SIZE,      ///< حجم_غرفة_ويبسوكت / sad_ws_server_room_size
                BUILTIN_NET_WS_SERVER_CLOSE_ALL,      ///< أغلق_كل_اتصالات_ويبسوكت / sad_ws_server_close_all

                // ==========================================
                // 22. عمليات SIMD / SIMD Vector Operations (Phase 3)
                // ==========================================
                // (AR) عمليات على متجهات SIMD <N x T> — تخريط مباشر إلى تعليمات
                //      LLVM المتجهة. Auto-vectorization تتولى توليد SSE/AVX/NEON.
                // (EN) SIMD vector operations <N x T> — direct mapping to LLVM
                //      vector instructions. Auto-vectorization handles SSE/AVX/NEON.
                //
                // البيانات الإضافية المُخزَّنة في extras[]:
                //   extras[0] = elementType (SadTypeKind as int)
                //   extras[1] = laneCount (N: 2, 4, 8, 16, 32, 64)
                //
                VECTOR_BUILD,   ///< (AR) بناء متجه من قيم سُلَّمية: %v = VECTOR_BUILD(elt0, elt1, ..., eltN-1)
                                ///< (EN) Build vector from scalars
                VECTOR_SPLAT,   ///< (AR) بث قيمة سُلَّمية إلى كل lanes: %v = VECTOR_SPLAT(scalar)
                                ///< (EN) Broadcast scalar to all lanes
                VECTOR_EXTRACT, ///< (AR) استخراج عنصر: %s = VECTOR_EXTRACT(%v, idx)
                VECTOR_INSERT,  ///< (AR) إدراج عنصر: %v2 = VECTOR_INSERT(%v, idx, scalar)
                VECTOR_LOAD,    ///< (AR) تحميل من ذاكرة محاذاة: %v = VECTOR_LOAD(%ptr)
                VECTOR_STORE,   ///< (AR) تخزين في ذاكرة محاذاة: VECTOR_STORE(%v, %ptr)
                VECTOR_ADD,     ///< (AR) جمع متجهين: %v3 = VECTOR_ADD(%v1, %v2)
                VECTOR_SUB,     ///< (AR) طرح متجهين
                VECTOR_MUL,     ///< (AR) ضرب عناصر-عناصر
                VECTOR_DIV,     ///< (AR) قسمة عناصر-عناصر
                VECTOR_FMA,     ///< (AR) ضرب-جمع مُدمج: %v = a*b+c (fused multiply-add)
                VECTOR_NEG,     ///< (AR) السالب
                VECTOR_ABS,     ///< (AR) القيمة المطلقة
                VECTOR_SQRT,    ///< (AR) الجذر التربيعي
                VECTOR_MIN,     ///< (AR) الحد الأدنى عنصراً-عنصراً
                VECTOR_MAX,     ///< (AR) الحد الأقصى عنصراً-عنصراً
                VECTOR_AND,     ///< (AR) AND ثنائي bitwise
                VECTOR_OR,      ///< (AR) OR ثنائي bitwise
                VECTOR_XOR,     ///< (AR) XOR ثنائي bitwise
                VECTOR_EQ,      ///< (AR) مقارنة == → متجه منطقي <N x i1>
                VECTOR_NE,      ///< (AR) مقارنة != → متجه منطقي
                VECTOR_LT,      ///< (AR) مقارنة < → متجه منطقي
                VECTOR_LE,      ///< (AR) مقارنة <= → متجه منطقي
                VECTOR_GT,      ///< (AR) مقارنة > → متجه منطقي
                VECTOR_GE,      ///< (AR) مقارنة >= → متجه منطقي
                VECTOR_HSUM,    ///< (AR) جمع عرضي: scalar = sum(v[0..N-1])
                VECTOR_HMUL,    ///< (AR) ضرب عرضي
                VECTOR_HMIN,    ///< (AR) أدنى عرضي
                VECTOR_HMAX,    ///< (AR) أقصى عرضي
                VECTOR_DOT,     ///< (AR) جداء قياسي (dot product) → scalar
                VECTOR_SHUFFLE, ///< (AR) خلط: %v3 = VECTOR_SHUFFLE(%v1, %v2, mask...)
                VECTOR_SELECT,  ///< (AR) اختيار حسب قناع: %v3 = SELECT(mask, v1, v2)

                // (AR) [الخطوة ٩] اقتطاع بايت (u8) واعٍ بالوسم الديناميّ: يُقنِّع البايت
                //      الأدنى (& 0xFF) للقيم الصحيحة فقط، ويترك العشريّ سليمًا — مرآةٌ
                //      حرفيّةٌ لحارس المفسّر getKind()==Integer عند إسناد بايت. للقيم
                //      %SadDyn (Any، كنتيجة قسمة /،// ديناميّة النوع): يقنّع إذا كان
                //      الوسم Int وإلا يُعيد القيمة كما هي (select زمنَ التشغيل).
                // (EN) [Step 9] Kind-aware u8 truncation: masks the low byte (& 0xFF) for
                //      integer values only, leaving floats intact — a literal mirror of the
                //      interpreter's getKind()==Integer guard on byte assignment. For %SadDyn
                //      (Any, e.g. a runtime-typed /,// result): masks iff the tag is Int, else
                //      returns the value unchanged (runtime select).
                TRUNCATE_U8,

                // (AR) تعليبُ قيمةٍ في %SadDyn مُخصَّصٍ في الكومة، وإرجاعُ مؤشّرٍ إليه —
                //      «option A» لعناصر المصفوفات مختلطة الأنواع (ISSUE-052/070/080/082):
                //      خانةُ المصفوفة 8 بايت لا تسع الوسمَ، فنُعلّب العنصرَ (toDyn) ونخزّن
                //      المؤشّرَ (كنظير عنصر النصّ/المصفوفة المتداخلة)؛ ARRAY_GET يفكّه Any.
                // (EN) Box a value into a heap-allocated %SadDyn and return a pointer to it —
                //      "option A" for heterogeneous-array elements (ISSUE-052/070/080/082): an
                //      8-byte slot can't hold the tag, so box the element (toDyn) and store the
                //      pointer (like a string/nested-array element); ARRAY_GET decodes it to Any.
                BOX_DYN,

                Nop ///< لا عملية (markers) / No operation (for markers)
            };

            /**
             * @brief (AR) تحويل SIROpcode إلى نص
             * @brief (EN) Convert SIROpcode to string
             */
            const char *sirOpcodeToString(SIROpcode opcode);

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
            enum class SIROperandType
            {
                REGISTER, ///< سجل افتراضي / Virtual register (%0, %1, %name)
                CONSTANT, ///< ثابت / Constant (42, 3.14, "hello")
                LABEL,    ///< تسمية / Label (for branches)
                GLOBAL,   ///< متغير عام / Global variable ($name)
                FUNCTION  ///< اسم دالة / Function name (@name)
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
            class SIROperand
            {
            public:
                SIROperandType type;  ///< نوع المعامل / Operand type
                SadTypeKind dataType; ///< نوع البيانات / Data type
                // (AR) نوع عنصر المصفوفة (للمعاملات من نوع Array) — يُستخدم لطبع المصفوفات
                //      النصّيّة بـ%s بدل %lld. افتراضيّ Void ⇒ سلوك عدديّ (متوافق مع السابق).
                // (EN) Array element type (for Array-typed operands) — used to print string arrays
                //      via %s instead of %lld. Defaults to Void ⇒ integer behavior (back-compatible).
                SadTypeKind elementType = SadTypeKind::Void;

                // القيمة / Value (based on type)
                union
                {
                    int64_t intValue;  ///< قيمة صحيحة / Integer value
                    double floatValue; ///< قيمة عشرية / Float value
                    bool boolValue;    ///< قيمة منطقية / Boolean value
                };

                std::string name; ///< اسم (للسجلات، التسميات، الدوال) / Name

                // ==========================================
                // Constructors
                // ==========================================

                /**
                 * @brief (AR) منشئ افتراضي
                 * @brief (EN) Default constructor
                 */
                SIROperand() : type(SIROperandType::REGISTER), dataType(SadTypeKind::Void), intValue(0) {}

                /**
                 * @brief (AR) إنشاء سجل افتراضي
                 * @brief (EN) Create virtual register
                 */
                static SIROperand Register(const std::string &name, SadTypeKind type)
                {
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
                static SIROperand ConstantI64(int64_t value)
                {
                    SIROperand op;
                    op.type = SIROperandType::CONSTANT;
                    op.dataType = SadTypeKind::Integer;
                    op.intValue = value;
                    return op;
                }

                /**
                 * @brief (AR) إنشاء ثابت رقمي عشري
                 * @brief (EN) Create float constant
                 */
                static SIROperand ConstantF64(double value)
                {
                    SIROperand op;
                    op.type = SIROperandType::CONSTANT;
                    op.dataType = SadTypeKind::Float;
                    op.floatValue = value;
                    return op;
                }

                /**
                 * @brief (AR) إنشاء ثابت منطقي
                 * @brief (EN) Create boolean constant
                 */
                static SIROperand ConstantBool(bool value)
                {
                    SIROperand op;
                    op.type = SIROperandType::CONSTANT;
                    op.dataType = SadTypeKind::Boolean;
                    op.boolValue = value;
                    return op;
                }

                /**
                 * @brief (AR) إنشاء ثابت نصي
                 * @brief (EN) Create string constant
                 */
                static SIROperand ConstantString(const std::string &value)
                {
                    SIROperand op;
                    op.type = SIROperandType::CONSTANT;
                    op.dataType = SadTypeKind::String;
                    op.name = value;
                    return op;
                }

                /**
                 * @brief (AR) إنشاء تسمية
                 * @brief (EN) Create label
                 */
                static SIROperand Label(const std::string &name)
                {
                    SIROperand op;
                    op.type = SIROperandType::LABEL;
                    op.dataType = SadTypeKind::Void;
                    op.name = name;
                    return op;
                }

                /**
                 * @brief (AR) إنشاء متغير عام
                 * @brief (EN) Create global variable
                 */
                static SIROperand Global(const std::string &name, SadTypeKind type)
                {
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
                static SIROperand Function(const std::string &name)
                {
                    SIROperand op;
                    op.type = SIROperandType::FUNCTION;
                    op.dataType = SadTypeKind::Function;
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
