/*
 * ============================================================================
 * LLVM Runtime Functions - رأس الملف
 * LLVM Runtime Functions - Header File
 * ============================================================================
 * 
 * الوصف (Description):
 *   هذا الملف يحتوي على دوال runtime التي يستدعيها كود LLVM المولد. يوفر
 *   الوظائف الأساسية للذاكرة، المصفوفات، النصوص، الإدخال/الإخراج، وجامع القمامة.
 * 
 *   This file contains runtime functions called by generated LLVM code. Provides
 *   core functionality for memory, arrays, strings, I/O, and garbage collection.
 * 
 * المكونات الرئيسية (Main Components):
 *   - Memory Management: إدارة الذاكرة مع GC
 *   - Memory Management: Memory management with GC
 * 
 *   - Array Operations: عمليات المصفوفات الديناميكية
 *   - Array Operations: Dynamic array operations
 * 
 *   - String Operations: معالجة النصوص
 *   - String Operations: String handling
 * 
 *   - I/O Functions: دوال الإدخال والإخراج
 *   - I/O Functions: Input/output functions
 * 
 *   - GC Integration: تكامل مع جامع القمامة
 *   - GC Integration: Garbage collector integration
 * 
 * الاستخدام (Usage):
 *   هذه الدوال مصدّرة بـ "C" linkage ويتم استدعاؤها من كود LLVM المولّد
 *   These functions are exported with "C" linkage and called from generated LLVM code
 * 
 *   ```llvm
 *   ; LLVM IR
 *   %arr = call i8* @sad_llvm_array_new(i64 10)
 *   %str = call i8* @sad_llvm_string_concat(i8* %s1, i8* %s2)
 *   call void @sad_llvm_print(i8* %value)
 *   ```
 * 
 * التبعيات (Dependencies):
 *   - GC (Garbage Collector)
 *   - Standard C/C++ Library
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_LLVM_RUNTIME_H
#define SAD_LLVM_RUNTIME_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// Type Definitions / تعريفات الأنواع
// ============================================================================

/**
 * قيمة Sad العامة (tagged union)
 * Sad generic value (tagged union)
 */
typedef struct SadValue {
    uint8_t type;      // نوع القيمة / Value type
    union {
        int64_t i64;   // صحيح 64 بت / 64-bit integer
        double f64;    // عشري 64 بت / 64-bit float
        void* ptr;     // مؤشر / Pointer
        uint8_t boolean; // منطقي / Boolean
    } data;
} SadValue;

/**
 * أنواع القيم / Value types
 */
typedef enum {
    SAD_TYPE_NULL = 0,      // قيمة فارغة / Null
    SAD_TYPE_INT = 1,       // عدد صحيح / Integer
    SAD_TYPE_FLOAT = 2,     // عدد عشري / Float
    SAD_TYPE_BOOL = 3,      // منطقي / Boolean
    SAD_TYPE_STRING = 4,    // نص / String
    SAD_TYPE_ARRAY = 5,     // مصفوفة / Array
    SAD_TYPE_OBJECT = 6,    // كائن / Object
    SAD_TYPE_FUNCTION = 7,  // دالة / Function
} SadValueType;

/**
 * بنية المصفوفة / Array structure
 */
typedef struct SadArray {
    uint64_t length;        // طول المصفوفة / Array length
    uint64_t capacity;      // السعة / Capacity
    SadValue* elements;     // العناصر / Elements
} SadArray;

/**
 * بنية النص / String structure
 */
typedef struct SadString {
    uint64_t length;        // طول النص / String length
    char* data;             // البيانات / Data
} SadString;

/**
 * بنية الكائن / Object structure
 */
typedef struct SadObject {
    uint64_t refCount;      // عداد المراجع / Reference count
    void* data;             // البيانات / Data
    void (*destructor)(void*); // المدمر / Destructor
} SadObject;

// ============================================================================
// Memory Management / إدارة الذاكرة
// ============================================================================

/**
 * تخصيص ذاكرة مع GC
 * Allocate memory with GC
 * 
 * @param size حجم الذاكرة بالبايتات / Size in bytes
 * @return مؤشر للذاكرة المخصصة أو NULL / Pointer to allocated memory or NULL
 */
void* sad_llvm_alloc(uint64_t size);

/**
 * إعادة تخصيص ذاكرة
 * Reallocate memory
 * 
 * @param ptr المؤشر القديم / Old pointer
 * @param new_size الحجم الجديد / New size
 * @return مؤشر للذاكرة الجديدة / Pointer to new memory
 */
void* sad_llvm_realloc(void* ptr, uint64_t new_size);

/**
 * تحرير ذاكرة
 * Free memory
 * 
 * @param ptr مؤشر الذاكرة / Memory pointer
 */
void sad_llvm_free(void* ptr);

/**
 * تخصيص ذاكرة بدون GC (للاستخدام الداخلي)
 * Allocate memory without GC (for internal use)
 * 
 * @param size حجم الذاكرة / Size
 * @return مؤشر الذاكرة / Memory pointer
 */
void* sad_llvm_malloc(uint64_t size);

/**
 * نسخ الذاكرة
 * Copy memory
 * 
 * @param dest الوجهة / Destination
 * @param src المصدر / Source
 * @param size الحجم / Size
 * @return مؤشر الوجهة / Destination pointer
 */
void* sad_llvm_memcpy(void* dest, const void* src, uint64_t size);

/**
 * ملء الذاكرة بقيمة
 * Fill memory with value
 * 
 * @param ptr المؤشر / Pointer
 * @param value القيمة / Value
 * @param size الحجم / Size
 * @return مؤشر الذاكرة / Memory pointer
 */
void* sad_llvm_memset(void* ptr, int value, uint64_t size);

/**
 * مقارنة الذاكرة
 * Compare memory
 * 
 * @param ptr1 المؤشر الأول / First pointer
 * @param ptr2 المؤشر الثاني / Second pointer
 * @param size الحجم / Size
 * @return 0 إذا متساوية، <0 إذا ptr1 أصغر، >0 إذا ptr1 أكبر
 */
int sad_llvm_memcmp(const void* ptr1, const void* ptr2, uint64_t size);

// ============================================================================
// Array Operations / عمليات المصفوفات
// ============================================================================

/**
 * إنشاء مصفوفة جديدة
 * Create new array
 * 
 * @param capacity السعة الابتدائية / Initial capacity
 * @return مؤشر المصفوفة / Array pointer
 */
void* sad_llvm_array_new(uint64_t capacity);

/**
 * الحصول على عنصر من المصفوفة
 * Get array element
 * 
 * @param arr مؤشر المصفوفة / Array pointer
 * @param index الفهرس / Index
 * @return القيمة / Value
 */
SadValue sad_llvm_array_get(void* arr, uint64_t index);

/**
 * تعيين عنصر في المصفوفة
 * Set array element
 * 
 * @param arr مؤشر المصفوفة / Array pointer
 * @param index الفهرس / Index
 * @param value القيمة / Value
 */
void sad_llvm_array_set(void* arr, uint64_t index, SadValue value);

/**
 * إضافة عنصر في نهاية المصفوفة
 * Append element to array
 * 
 * @param arr مؤشر المصفوفة / Array pointer
 * @param value القيمة / Value
 */
void sad_llvm_array_push(void* arr, SadValue value);

/**
 * إزالة وإرجاع آخر عنصر
 * Remove and return last element
 * 
 * @param arr مؤشر المصفوفة / Array pointer
 * @return القيمة / Value
 */
SadValue sad_llvm_array_pop(void* arr);

/**
 * الحصول على طول المصفوفة
 * Get array length
 * 
 * @param arr مؤشر المصفوفة / Array pointer
 * @return الطول / Length
 */
uint64_t sad_llvm_array_length(void* arr);

/**
 * تغيير حجم المصفوفة
 * Resize array
 * 
 * @param arr مؤشر المصفوفة / Array pointer
 * @param new_size الحجم الجديد / New size
 */
void sad_llvm_array_resize(void* arr, uint64_t new_size);

/**
 * نسخ المصفوفة
 * Clone array
 * 
 * @param arr مؤشر المصفوفة / Array pointer
 * @return مؤشر المصفوفة الجديدة / New array pointer
 */
void* sad_llvm_array_clone(void* arr);

/**
 * دمج مصفوفتين
 * Concatenate arrays
 * 
 * @param arr1 المصفوفة الأولى / First array
 * @param arr2 المصفوفة الثانية / Second array
 * @return مصفوفة جديدة / New array
 */
void* sad_llvm_array_concat(void* arr1, void* arr2);

/**
 * شريحة من المصفوفة
 * Array slice
 * 
 * @param arr مؤشر المصفوفة / Array pointer
 * @param start البداية / Start index
 * @param end النهاية / End index
 * @return مصفوفة جديدة / New array
 */
void* sad_llvm_array_slice(void* arr, uint64_t start, uint64_t end);

// ============================================================================
// String Operations / عمليات النصوص
// ============================================================================

/**
 * إنشاء نص جديد
 * Create new string
 * 
 * @param data البيانات / Data
 * @param length الطول / Length
 * @return مؤشر النص / String pointer
 */
void* sad_llvm_string_new(const char* data, uint64_t length);

/**
 * إنشاء نص من C string
 * Create string from C string
 * 
 * @param cstr نص C / C string
 * @return مؤشر النص / String pointer
 */
void* sad_llvm_string_from_cstr(const char* cstr);

/**
 * دمج نصين
 * Concatenate strings
 * 
 * @param str1 النص الأول / First string
 * @param str2 النص الثاني / Second string
 * @return نص جديد / New string
 */
void* sad_llvm_string_concat(void* str1, void* str2);

/**
 * الحصول على طول النص
 * Get string length
 * 
 * @param str مؤشر النص / String pointer
 * @return الطول / Length
 */
uint64_t sad_llvm_string_length(void* str);

/**
 * الحصول على حرف بفهرس معين
 * Get character at index
 * 
 * @param str مؤشر النص / String pointer
 * @param index الفهرس / Index
 * @return الحرف / Character
 */
char sad_llvm_string_char_at(void* str, uint64_t index);

/**
 * شريحة من النص
 * String slice
 * 
 * @param str مؤشر النص / String pointer
 * @param start البداية / Start index
 * @param end النهاية / End index
 * @return نص جديد / New string
 */
void* sad_llvm_string_slice(void* str, uint64_t start, uint64_t end);

/**
 * مقارنة نصين
 * Compare strings
 * 
 * @param str1 النص الأول / First string
 * @param str2 النص الثاني / Second string
 * @return 0 إذا متساوية / 0 if equal
 */
int sad_llvm_string_compare(void* str1, void* str2);

/**
 * البحث عن نص فرعي
 * Find substring
 * 
 * @param str النص الأصلي / Original string
 * @param substr النص الفرعي / Substring
 * @return فهرس أول ظهور أو -1 / Index of first occurrence or -1
 */
int64_t sad_llvm_string_find(void* str, void* substr);

/**
 * استبدال نص فرعي
 * Replace substring
 * 
 * @param str النص الأصلي / Original string
 * @param old النص القديم / Old substring
 * @param new النص الجديد / New substring
 * @return نص جديد / New string
 */
void* sad_llvm_string_replace(void* str, void* old, void* new_str);

/**
 * تقسيم النص
 * Split string
 * 
 * @param str النص / String
 * @param delimiter الفاصل / Delimiter
 * @return مصفوفة نصوص / Array of strings
 */
void* sad_llvm_string_split(void* str, void* delimiter);

/**
 * تحويل إلى أحرف كبيرة
 * Convert to uppercase
 * 
 * @param str النص / String
 * @return نص جديد / New string
 */
void* sad_llvm_string_to_upper(void* str);

/**
 * تحويل إلى أحرف صغيرة
 * Convert to lowercase
 * 
 * @param str النص / String
 * @return نص جديد / New string
 */
void* sad_llvm_string_to_lower(void* str);

/**
 * إزالة المسافات من البداية والنهاية
 * Trim whitespace
 * 
 * @param str النص / String
 * @return نص جديد / New string
 */
void* sad_llvm_string_trim(void* str);

// ============================================================================
// I/O Operations / عمليات الإدخال والإخراج
// ============================================================================

/**
 * طباعة قيمة
 * Print value
 * 
 * @param value القيمة / Value
 */
void sad_llvm_print(SadValue value);

/**
 * طباعة قيمة مع سطر جديد
 * Print value with newline
 * 
 * @param value القيمة / Value
 */
void sad_llvm_println(SadValue value);

/**
 * طباعة نص
 * Print string
 * 
 * @param str مؤشر النص / String pointer
 */
void sad_llvm_print_string(void* str);

/**
 * طباعة عدد صحيح
 * Print integer
 * 
 * @param value العدد / Integer
 */
void sad_llvm_print_int(int64_t value);

/**
 * طباعة عدد عشري
 * Print float
 * 
 * @param value العدد / Float
 */
void sad_llvm_print_float(double value);

/**
 * قراءة سطر من الإدخال
 * Read line from input
 * 
 * @return نص / String
 */
void* sad_llvm_input();

/**
 * قراءة عدد صحيح
 * Read integer
 * 
 * @return عدد صحيح / Integer
 */
int64_t sad_llvm_input_int();

/**
 * قراءة عدد عشري
 * Read float
 * 
 * @return عدد عشري / Float
 */
double sad_llvm_input_float();

/**
 * فتح ملف
 * Open file
 * 
 * @param filename اسم الملف / Filename
 * @param mode الوضع / Mode
 * @return معرّف الملف / File descriptor
 */
int64_t sad_llvm_file_open(void* filename, void* mode);

/**
 * إغلاق ملف
 * Close file
 * 
 * @param fd معرّف الملف / File descriptor
 */
void sad_llvm_file_close(int64_t fd);

/**
 * قراءة من ملف
 * Read from file
 * 
 * @param fd معرّف الملف / File descriptor
 * @param size حجم القراءة / Size to read
 * @return البيانات / Data
 */
void* sad_llvm_file_read(int64_t fd, uint64_t size);

/**
 * كتابة إلى ملف
 * Write to file
 * 
 * @param fd معرّف الملف / File descriptor
 * @param data البيانات / Data
 * @param size الحجم / Size
 * @return عدد البايتات المكتوبة / Bytes written
 */
uint64_t sad_llvm_file_write(int64_t fd, void* data, uint64_t size);

// ============================================================================
// Garbage Collector Integration / تكامل جامع القمامة
// ============================================================================

/**
 * تسجيل كائن مع GC
 * Register object with GC
 * 
 * @param ptr مؤشر الكائن / Object pointer
 * @param size حجم الكائن / Object size
 */
void sad_llvm_gc_register(void* ptr, uint64_t size);

/**
 * إلغاء تسجيل كائن
 * Unregister object
 * 
 * @param ptr مؤشر الكائن / Object pointer
 */
void sad_llvm_gc_unregister(void* ptr);

/**
 * تشغيل جامع القمامة
 * Trigger garbage collection
 */
void sad_llvm_gc_collect();

/**
 * تشغيل جمع تدريجي
 * Trigger incremental collection
 * 
 * @param steps عدد الخطوات / Number of steps
 */
void sad_llvm_gc_collect_incremental(uint64_t steps);

/**
 * الحصول على إحصائيات GC
 * Get GC statistics
 * 
 * @param allocated الذاكرة المخصصة / Allocated memory (output)
 * @param used الذاكرة المستخدمة / Used memory (output)
 * @param collections عدد عمليات الجمع / Collection count (output)
 */
void sad_llvm_gc_stats(uint64_t* allocated, uint64_t* used, uint64_t* collections);

/**
 * تعليق GC مؤقتاً
 * Pause GC temporarily
 */
void sad_llvm_gc_pause();

/**
 * استئناف GC
 * Resume GC
 */
void sad_llvm_gc_resume();

/**
 * إضافة جذر GC
 * Add GC root
 * 
 * @param ptr مؤشر الجذر / Root pointer
 */
void sad_llvm_gc_add_root(void* ptr);

/**
 * إزالة جذر GC
 * Remove GC root
 * 
 * @param ptr مؤشر الجذر / Root pointer
 */
void sad_llvm_gc_remove_root(void* ptr);

// ============================================================================
// Type Conversion / تحويل الأنواع
// ============================================================================

/**
 * تحويل عدد صحيح إلى نص
 * Convert integer to string
 * 
 * @param value العدد / Integer
 * @return نص / String
 */
void* sad_llvm_int_to_string(int64_t value);

/**
 * تحويل عدد عشري إلى نص
 * Convert float to string
 * 
 * @param value العدد / Float
 * @return نص / String
 */
void* sad_llvm_float_to_string(double value);

/**
 * تحويل نص إلى عدد صحيح
 * Convert string to integer
 * 
 * @param str النص / String
 * @return عدد صحيح / Integer
 */
int64_t sad_llvm_string_to_int(void* str);

/**
 * تحويل نص إلى عدد عشري
 * Convert string to float
 * 
 * @param str النص / String
 * @return عدد عشري / Float
 */
double sad_llvm_string_to_float(void* str);

/**
 * تحويل قيمة إلى منطقي
 * Convert value to boolean
 * 
 * @param value القيمة / Value
 * @return منطقي / Boolean
 */
uint8_t sad_llvm_to_bool(SadValue value);

// ============================================================================
// Utility Functions / دوال مساعدة
// ============================================================================

/**
 * الحصول على الوقت الحالي (milliseconds منذ epoch)
 * Get current time (milliseconds since epoch)
 * 
 * @return الوقت / Time
 */
uint64_t sad_llvm_time_ms();

/**
 * انتظار (sleep) بالميلي ثانية
 * Sleep in milliseconds
 * 
 * @param ms عدد الميلي ثانية / Milliseconds
 */
void sad_llvm_sleep_ms(uint64_t ms);

/**
 * الخروج من البرنامج
 * Exit program
 * 
 * @param code كود الخروج / Exit code
 */
void sad_llvm_exit(int code);

/**
 * طباعة خطأ والخروج
 * Print error and exit
 * 
 * @param message رسالة الخطأ / Error message
 */
void sad_llvm_panic(void* message);

#ifdef __cplusplus
}
#endif

#endif // SAD_LLVM_RUNTIME_H
