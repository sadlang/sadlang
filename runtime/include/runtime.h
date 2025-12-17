/**
 * @file runtime.h
 * @brief مكتبة التشغيل / Runtime Library
 * @brief Core runtime functions for I/O, memory, and system operations
 * 
 * @details
 * (AR) مكتبة التشغيل توفر الدوال الأساسية للإدخال/الإخراج، إدارة الذاكرة،
 *      والعمليات النظامية. تتضمن دعم المرحلة 1 والدوال المدمجة.
 * 
 * (EN) Runtime library provides core functions for I/O, memory management,
 *      and system operations. Includes Stage 1 compatibility and builtins.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 * @phase Phase 3: Bytecode Backend
 */

#pragma once

#include "../../vm/include/vm.h"
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>

namespace Sad {
namespace Runtime {

// ========================================
// I/O Functions
// دوال الإدخال والإخراج
// ========================================

/**
 * @brief طباعة قيمة / Print value
 * @brief Print value to stdout without newline
 */
VM::Value runtime_print(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief طباعة قيمة مع سطر جديد / Print with newline
 * @brief Print value to stdout with newline
 */
VM::Value runtime_println(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief طباعة منسقة / Formatted print
 * @brief Printf-style formatted output
 */
VM::Value runtime_printf(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief قراءة سطر / Read line
 * @brief Read line from stdin
 */
VM::Value runtime_input(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief قراءة عدد صحيح / Read integer
 * @brief Read integer from stdin
 */
VM::Value runtime_read_int(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief قراءة عدد عشري / Read float
 * @brief Read float from stdin
 */
VM::Value runtime_read_float(VM::VirtualMachine* vm, int argc, VM::Value* args);

// ========================================
// File Operations
// عمليات الملفات
// ========================================

/**
 * @brief فتح ملف / Open file
 * @param filename اسم الملف / Filename
 * @param mode الوضع / Mode ("r", "w", "a", "rb", "wb")
 * @return معرّف الملف / File descriptor or -1 on error
 */
VM::Value runtime_fopen(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief قراءة من ملف / Read from file
 * @param fd معرّف الملف / File descriptor
 * @param size الحجم / Size to read
 * @return البيانات المقروءة / Read data as string
 */
VM::Value runtime_fread(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief كتابة إلى ملف / Write to file
 * @param fd معرّف الملف / File descriptor
 * @param data البيانات / Data to write
 * @return عدد البايتات المكتوبة / Bytes written
 */
VM::Value runtime_fwrite(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief إغلاق ملف / Close file
 * @param fd معرّف الملف / File descriptor
 */
VM::Value runtime_fclose(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief قراءة ملف كامل / Read entire file
 * @param filename اسم الملف / Filename
 * @return محتوى الملف / File contents as string
 */
VM::Value runtime_read_file(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief كتابة ملف كامل / Write entire file
 * @param filename اسم الملف / Filename
 * @param data البيانات / Data to write
 */
VM::Value runtime_write_file(VM::VirtualMachine* vm, int argc, VM::Value* args);

// ========================================
// Memory Operations
// عمليات الذاكرة
// ========================================

/**
 * @brief تخصيص ذاكرة / Allocate memory
 * @param size الحجم بالبايت / Size in bytes
 * @return مؤشر الذاكرة / Memory pointer
 */
VM::Value runtime_malloc(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief تحرير ذاكرة / Free memory
 * @param ptr المؤشر / Pointer
 */
VM::Value runtime_free(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief إعادة تخصيص ذاكرة / Reallocate memory
 * @param ptr المؤشر القديم / Old pointer
 * @param size الحجم الجديد / New size
 * @return المؤشر الجديد / New pointer
 */
VM::Value runtime_realloc(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief نسخ ذاكرة / Copy memory
 * @param dest الوجهة / Destination
 * @param src المصدر / Source
 * @param size الحجم / Size
 */
VM::Value runtime_memcpy(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief ملء ذاكرة / Fill memory
 * @param ptr المؤشر / Pointer
 * @param value القيمة / Value
 * @param size الحجم / Size
 */
VM::Value runtime_memset(VM::VirtualMachine* vm, int argc, VM::Value* args);

// ========================================
// Stage 1 Compatibility
// توافق المرحلة 1
// ========================================

/**
 * @brief mmap - Memory mapping
 * @brief Stage 1 compatibility function
 */
VM::Value runtime_mmap(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief munmap - Unmap memory
 * @brief Stage 1 compatibility function
 */
VM::Value runtime_munmap(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief io_read - Read from file descriptor
 * @brief Stage 1 compatibility function
 */
VM::Value runtime_io_read(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief io_write - Write to file descriptor
 * @brief Stage 1 compatibility function
 */
VM::Value runtime_io_write(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief int_get - Get integer from memory
 * @brief Stage 1 compatibility function
 */
VM::Value runtime_int_get(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief int_set - Set integer in memory
 * @brief Stage 1 compatibility function
 */
VM::Value runtime_int_set(VM::VirtualMachine* vm, int argc, VM::Value* args);

// ========================================
// Math Library
// مكتبة الرياضيات
// ========================================

/**
 * @brief sin - Sine function
 */
VM::Value runtime_sin(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief cos - Cosine function
 */
VM::Value runtime_cos(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief tan - Tangent function
 */
VM::Value runtime_tan(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief log - Natural logarithm
 */
VM::Value runtime_log(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief log10 - Base-10 logarithm
 */
VM::Value runtime_log10(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief exp - Exponential function
 */
VM::Value runtime_exp(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief floor - Floor function
 */
VM::Value runtime_floor(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief ceil - Ceiling function
 */
VM::Value runtime_ceil(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief round - Round function
 */
VM::Value runtime_round(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief min - Minimum of two values
 */
VM::Value runtime_min(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief max - Maximum of two values
 */
VM::Value runtime_max(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief random - Generate random number
 */
VM::Value runtime_random(VM::VirtualMachine* vm, int argc, VM::Value* args);

// ========================================
// String Library
// مكتبة النصوص
// ========================================

/**
 * @brief split - Split string by delimiter
 * @param str النص / String
 * @param delimiter الفاصل / Delimiter
 * @return مصفوفة الأجزاء / Array of parts
 */
VM::Value runtime_split(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief join - Join array with separator
 * @param array المصفوفة / Array
 * @param separator الفاصل / Separator
 * @return النص المدموج / Joined string
 */
VM::Value runtime_join(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief replace - Replace substring
 * @param str النص / String
 * @param old القديم / Old substring
 * @param new الجديد / New substring
 * @return النص الجديد / New string
 */
VM::Value runtime_replace(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief trim - Remove whitespace
 * @param str النص / String
 * @return النص المُشذّب / Trimmed string
 */
VM::Value runtime_trim(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief to_upper - Convert to uppercase
 * @param str النص / String
 * @return النص بأحرف كبيرة / Uppercase string
 */
VM::Value runtime_to_upper(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief to_lower - Convert to lowercase
 * @param str النص / String
 * @return النص بأحرف صغيرة / Lowercase string
 */
VM::Value runtime_to_lower(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief starts_with - Check if string starts with prefix
 */
VM::Value runtime_starts_with(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief ends_with - Check if string ends with suffix
 */
VM::Value runtime_ends_with(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief contains - Check if string contains substring
 */
VM::Value runtime_contains(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief index_of - Find index of substring
 */
VM::Value runtime_index_of(VM::VirtualMachine* vm, int argc, VM::Value* args);

// ========================================
// Array Utilities
// أدوات المصفوفات
// ========================================

/**
 * @brief map - Map function over array
 */
VM::Value runtime_map(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief filter - Filter array elements
 */
VM::Value runtime_filter(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief reduce - Reduce array to single value
 */
VM::Value runtime_reduce(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief sort - Sort array
 */
VM::Value runtime_sort(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief reverse - Reverse array
 */
VM::Value runtime_reverse(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief slice - Extract array slice
 */
VM::Value runtime_slice(VM::VirtualMachine* vm, int argc, VM::Value* args);

// ========================================
// System Functions
// دوال النظام
// ========================================

/**
 * @brief exit - Exit program
 * @param code كود الخروج / Exit code
 */
VM::Value runtime_exit(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief sleep - Sleep for milliseconds
 * @param ms الميلي ثانية / Milliseconds
 */
VM::Value runtime_sleep(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief time - Get current time
 * @return الوقت بالثواني / Time in seconds
 */
VM::Value runtime_time(VM::VirtualMachine* vm, int argc, VM::Value* args);

/**
 * @brief clock - Get CPU time
 * @return وقت المعالج / CPU time
 */
VM::Value runtime_clock(VM::VirtualMachine* vm, int argc, VM::Value* args);

// ========================================
// Utility Functions
// دوال مساعدة
// ========================================

/**
 * @brief تسجيل جميع الدوال المدمجة / Register all runtime functions
 * @param vm الآلة الافتراضية / Virtual machine
 */
void registerAllRuntimeFunctions(VM::VirtualMachine* vm);

/**
 * @brief تسجيل دوال I/O / Register I/O functions
 */
void registerIOFunctions(VM::VirtualMachine* vm);

/**
 * @brief تسجيل دوال الذاكرة / Register memory functions
 */
void registerMemoryFunctions(VM::VirtualMachine* vm);

/**
 * @brief تسجيل دوال المرحلة 1 / Register Stage 1 functions
 */
void registerStage1Functions(VM::VirtualMachine* vm);

/**
 * @brief تسجيل دوال الرياضيات / Register math functions
 */
void registerMathFunctions(VM::VirtualMachine* vm);

/**
 * @brief تسجيل دوال النصوص / Register string functions
 */
void registerStringFunctions(VM::VirtualMachine* vm);

/**
 * @brief تسجيل دوال المصفوفات / Register array functions
 */
void registerArrayFunctions(VM::VirtualMachine* vm);

/**
 * @brief تسجيل دوال النظام / Register system functions
 */
void registerSystemFunctions(VM::VirtualMachine* vm);

// ========================================
// File Management
// إدارة الملفات
// ========================================

/**
 * @brief مدير الملفات / File manager
 * @brief Manages open file handles
 */
class FileManager {
public:
    /**
     * @brief فتح ملف / Open file
     * @return معرّف الملف / File descriptor
     */
    int openFile(const std::string& filename, const std::string& mode);
    
    /**
     * @brief إغلاق ملف / Close file
     */
    bool closeFile(int fd);
    
    /**
     * @brief الحصول على ملف / Get file stream
     */
    std::fstream* getFile(int fd);
    
    /**
     * @brief إغلاق جميع الملفات / Close all files
     */
    void closeAll();
    
private:
    std::vector<std::fstream*> files_;
    int nextFd_ = 3; // Start after stdin/stdout/stderr
};

} // namespace Runtime
} // namespace Sad
