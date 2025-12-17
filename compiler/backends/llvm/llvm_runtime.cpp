/*
 * ============================================================================
 * LLVM Runtime Functions - ملف التنفيذ
 * LLVM Runtime Functions - Implementation File
 * ============================================================================
 * 
 * تنفيذ دوال runtime التي يستدعيها كود LLVM المولد
 * Implementation of runtime functions called by generated LLVM code
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include "llvm_runtime.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// ============================================================================
// Internal Data Structures / الهياكل الداخلية
// ============================================================================

/**
 * GC Context - سياق جامع القمامة
 * GC Context - Garbage collector context
 */
typedef struct {
    void** objects;           // قائمة الكائنات / Objects list
    uint64_t* sizes;          // أحجام الكائنات / Object sizes
    uint64_t count;           // عدد الكائنات / Object count
    uint64_t capacity;        // السعة / Capacity
    uint64_t total_allocated; // الذاكرة المخصصة الكلية / Total allocated
    uint64_t collections;     // عدد عمليات الجمع / Collection count
    int paused;               // هل GC متوقف؟ / Is GC paused?
    void** roots;             // الجذور / Roots
    uint64_t root_count;      // عدد الجذور / Root count
    uint64_t root_capacity;   // سعة الجذور / Root capacity
} GCContext;

// السياق العام / Global context
static GCContext gc_context = {NULL, NULL, 0, 0, 0, 0, 0, NULL, 0, 0};

// قائمة الملفات المفتوحة / Open files list
#define MAX_OPEN_FILES 256
static FILE* open_files[MAX_OPEN_FILES] = {NULL};

// ============================================================================
// Internal Helper Functions / دوال مساعدة داخلية
// ============================================================================

/**
 * تهيئة GC
 * Initialize GC
 */
static void gc_init() {
    if (gc_context.objects == NULL) {
        gc_context.capacity = 1024;
        gc_context.objects = (void**)malloc(gc_context.capacity * sizeof(void*));
        gc_context.sizes = (uint64_t*)malloc(gc_context.capacity * sizeof(uint64_t));
        gc_context.count = 0;
        gc_context.total_allocated = 0;
        gc_context.collections = 0;
        gc_context.paused = 0;
        
        gc_context.root_capacity = 256;
        gc_context.roots = (void**)malloc(gc_context.root_capacity * sizeof(void*));
        gc_context.root_count = 0;
    }
}

/**
 * توسيع سعة GC
 * Expand GC capacity
 */
static void gc_expand() {
    gc_context.capacity *= 2;
    gc_context.objects = (void**)realloc(gc_context.objects, 
                                         gc_context.capacity * sizeof(void*));
    gc_context.sizes = (uint64_t*)realloc(gc_context.sizes,
                                          gc_context.capacity * sizeof(uint64_t));
}

/**
 * البحث عن كائن في GC
 * Find object in GC
 */
static int gc_find_object(void* ptr) {
    for (uint64_t i = 0; i < gc_context.count; i++) {
        if (gc_context.objects[i] == ptr) {
            return (int)i;
        }
    }
    return -1;
}

// ============================================================================
// Memory Management / إدارة الذاكرة
// ============================================================================

/**
 * تخصيص ذاكرة مع GC
 * Allocate memory with GC
 */
void* sad_llvm_alloc(uint64_t size) {
    gc_init();
    
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "خطأ: فشل تخصيص الذاكرة / Error: Memory allocation failed\n");
        return NULL;
    }
    
    // تسجيل في GC / Register with GC
    sad_llvm_gc_register(ptr, size);
    
    return ptr;
}

/**
 * إعادة تخصيص ذاكرة
 * Reallocate memory
 */
void* sad_llvm_realloc(void* ptr, uint64_t new_size) {
    if (ptr == NULL) {
        return sad_llvm_alloc(new_size);
    }
    
    // البحث عن الكائن / Find object
    int index = gc_find_object(ptr);
    
    void* new_ptr = realloc(ptr, new_size);
    if (new_ptr == NULL) {
        fprintf(stderr, "خطأ: فشل إعادة تخصيص الذاكرة / Error: Reallocation failed\n");
        return NULL;
    }
    
    // تحديث في GC / Update in GC
    if (index >= 0) {
        gc_context.objects[index] = new_ptr;
        gc_context.total_allocated -= gc_context.sizes[index];
        gc_context.sizes[index] = new_size;
        gc_context.total_allocated += new_size;
    }
    
    return new_ptr;
}

/**
 * تحرير ذاكرة
 * Free memory
 */
void sad_llvm_free(void* ptr) {
    if (ptr == NULL) return;
    
    sad_llvm_gc_unregister(ptr);
    free(ptr);
}

/**
 * تخصيص ذاكرة بدون GC
 * Allocate memory without GC
 */
void* sad_llvm_malloc(uint64_t size) {
    return malloc(size);
}

/**
 * نسخ الذاكرة
 * Copy memory
 */
void* sad_llvm_memcpy(void* dest, const void* src, uint64_t size) {
    return memcpy(dest, src, size);
}

/**
 * ملء الذاكرة
 * Fill memory
 */
void* sad_llvm_memset(void* ptr, int value, uint64_t size) {
    return memset(ptr, value, size);
}

/**
 * مقارنة الذاكرة
 * Compare memory
 */
int sad_llvm_memcmp(const void* ptr1, const void* ptr2, uint64_t size) {
    return memcmp(ptr1, ptr2, size);
}

// ============================================================================
// Array Operations / عمليات المصفوفات
// ============================================================================

/**
 * إنشاء مصفوفة جديدة
 * Create new array
 */
void* sad_llvm_array_new(uint64_t capacity) {
    SadArray* arr = (SadArray*)sad_llvm_alloc(sizeof(SadArray));
    if (arr == NULL) return NULL;
    
    arr->length = 0;
    arr->capacity = capacity > 0 ? capacity : 16;
    arr->elements = (SadValue*)sad_llvm_alloc(arr->capacity * sizeof(SadValue));
    
    if (arr->elements == NULL) {
        sad_llvm_free(arr);
        return NULL;
    }
    
    return arr;
}

/**
 * الحصول على عنصر
 * Get element
 */
SadValue sad_llvm_array_get(void* arr, uint64_t index) {
    SadArray* array = (SadArray*)arr;
    
    if (array == NULL || index >= array->length) {
        SadValue null_val = {SAD_TYPE_NULL, {0}};
        return null_val;
    }
    
    return array->elements[index];
}

/**
 * تعيين عنصر
 * Set element
 */
void sad_llvm_array_set(void* arr, uint64_t index, SadValue value) {
    SadArray* array = (SadArray*)arr;
    
    if (array == NULL) return;
    
    // توسيع المصفوفة إذا لزم الأمر / Expand if needed
    if (index >= array->capacity) {
        uint64_t new_capacity = (index + 1) * 2;
        array->elements = (SadValue*)sad_llvm_realloc(
            array->elements,
            new_capacity * sizeof(SadValue)
        );
        array->capacity = new_capacity;
    }
    
    array->elements[index] = value;
    
    if (index >= array->length) {
        array->length = index + 1;
    }
}

/**
 * إضافة عنصر
 * Push element
 */
void sad_llvm_array_push(void* arr, SadValue value) {
    SadArray* array = (SadArray*)arr;
    if (array == NULL) return;
    
    sad_llvm_array_set(arr, array->length, value);
}

/**
 * إزالة آخر عنصر
 * Pop element
 */
SadValue sad_llvm_array_pop(void* arr) {
    SadArray* array = (SadArray*)arr;
    
    if (array == NULL || array->length == 0) {
        SadValue null_val = {SAD_TYPE_NULL, {0}};
        return null_val;
    }
    
    array->length--;
    return array->elements[array->length];
}

/**
 * الحصول على الطول
 * Get length
 */
uint64_t sad_llvm_array_length(void* arr) {
    SadArray* array = (SadArray*)arr;
    return array ? array->length : 0;
}

/**
 * تغيير الحجم
 * Resize
 */
void sad_llvm_array_resize(void* arr, uint64_t new_size) {
    SadArray* array = (SadArray*)arr;
    if (array == NULL) return;
    
    if (new_size > array->capacity) {
        array->elements = (SadValue*)sad_llvm_realloc(
            array->elements,
            new_size * sizeof(SadValue)
        );
        array->capacity = new_size;
    }
    
    // تهيئة العناصر الجديدة / Initialize new elements
    for (uint64_t i = array->length; i < new_size; i++) {
        array->elements[i].type = SAD_TYPE_NULL;
        array->elements[i].data.i64 = 0;
    }
    
    array->length = new_size;
}

/**
 * نسخ المصفوفة
 * Clone array
 */
void* sad_llvm_array_clone(void* arr) {
    SadArray* array = (SadArray*)arr;
    if (array == NULL) return NULL;
    
    void* new_arr = sad_llvm_array_new(array->capacity);
    SadArray* new_array = (SadArray*)new_arr;
    
    new_array->length = array->length;
    sad_llvm_memcpy(new_array->elements, array->elements,
                    array->length * sizeof(SadValue));
    
    return new_arr;
}

/**
 * دمج مصفوفتين
 * Concatenate arrays
 */
void* sad_llvm_array_concat(void* arr1, void* arr2) {
    SadArray* array1 = (SadArray*)arr1;
    SadArray* array2 = (SadArray*)arr2;
    
    if (array1 == NULL) return sad_llvm_array_clone(arr2);
    if (array2 == NULL) return sad_llvm_array_clone(arr1);
    
    uint64_t total_length = array1->length + array2->length;
    void* new_arr = sad_llvm_array_new(total_length);
    SadArray* new_array = (SadArray*)new_arr;
    
    sad_llvm_memcpy(new_array->elements, array1->elements,
                    array1->length * sizeof(SadValue));
    sad_llvm_memcpy(new_array->elements + array1->length, array2->elements,
                    array2->length * sizeof(SadValue));
    
    new_array->length = total_length;
    
    return new_arr;
}

/**
 * شريحة من المصفوفة
 * Array slice
 */
void* sad_llvm_array_slice(void* arr, uint64_t start, uint64_t end) {
    SadArray* array = (SadArray*)arr;
    if (array == NULL) return NULL;
    
    // ضبط الحدود / Adjust bounds
    if (start > array->length) start = array->length;
    if (end > array->length) end = array->length;
    if (start > end) start = end;
    
    uint64_t slice_length = end - start;
    void* new_arr = sad_llvm_array_new(slice_length);
    SadArray* new_array = (SadArray*)new_arr;
    
    sad_llvm_memcpy(new_array->elements, array->elements + start,
                    slice_length * sizeof(SadValue));
    new_array->length = slice_length;
    
    return new_arr;
}

// ============================================================================
// String Operations / عمليات النصوص
// ============================================================================

/**
 * إنشاء نص جديد
 * Create new string
 */
void* sad_llvm_string_new(const char* data, uint64_t length) {
    SadString* str = (SadString*)sad_llvm_alloc(sizeof(SadString));
    if (str == NULL) return NULL;
    
    str->length = length;
    str->data = (char*)sad_llvm_alloc(length + 1);
    
    if (str->data == NULL) {
        sad_llvm_free(str);
        return NULL;
    }
    
    sad_llvm_memcpy(str->data, data, length);
    str->data[length] = '\0';
    
    return str;
}

/**
 * إنشاء نص من C string
 * Create string from C string
 */
void* sad_llvm_string_from_cstr(const char* cstr) {
    if (cstr == NULL) return NULL;
    return sad_llvm_string_new(cstr, strlen(cstr));
}

/**
 * دمج نصين
 * Concatenate strings
 */
void* sad_llvm_string_concat(void* str1, void* str2) {
    SadString* s1 = (SadString*)str1;
    SadString* s2 = (SadString*)str2;
    
    if (s1 == NULL && s2 == NULL) return NULL;
    if (s1 == NULL) return sad_llvm_string_new(s2->data, s2->length);
    if (s2 == NULL) return sad_llvm_string_new(s1->data, s1->length);
    
    uint64_t total_length = s1->length + s2->length;
    SadString* result = (SadString*)sad_llvm_alloc(sizeof(SadString));
    result->length = total_length;
    result->data = (char*)sad_llvm_alloc(total_length + 1);
    
    sad_llvm_memcpy(result->data, s1->data, s1->length);
    sad_llvm_memcpy(result->data + s1->length, s2->data, s2->length);
    result->data[total_length] = '\0';
    
    return result;
}

/**
 * طول النص
 * String length
 */
uint64_t sad_llvm_string_length(void* str) {
    SadString* s = (SadString*)str;
    return s ? s->length : 0;
}

/**
 * الحصول على حرف
 * Get character
 */
char sad_llvm_string_char_at(void* str, uint64_t index) {
    SadString* s = (SadString*)str;
    if (s == NULL || index >= s->length) return '\0';
    return s->data[index];
}

/**
 * شريحة من النص
 * String slice
 */
void* sad_llvm_string_slice(void* str, uint64_t start, uint64_t end) {
    SadString* s = (SadString*)str;
    if (s == NULL) return NULL;
    
    if (start > s->length) start = s->length;
    if (end > s->length) end = s->length;
    if (start > end) start = end;
    
    uint64_t slice_length = end - start;
    return sad_llvm_string_new(s->data + start, slice_length);
}

/**
 * مقارنة نصين
 * Compare strings
 */
int sad_llvm_string_compare(void* str1, void* str2) {
    SadString* s1 = (SadString*)str1;
    SadString* s2 = (SadString*)str2;
    
    if (s1 == NULL && s2 == NULL) return 0;
    if (s1 == NULL) return -1;
    if (s2 == NULL) return 1;
    
    uint64_t min_len = s1->length < s2->length ? s1->length : s2->length;
    int cmp = memcmp(s1->data, s2->data, min_len);
    
    if (cmp == 0) {
        if (s1->length < s2->length) return -1;
        if (s1->length > s2->length) return 1;
        return 0;
    }
    
    return cmp;
}

/**
 * البحث عن نص فرعي
 * Find substring
 */
int64_t sad_llvm_string_find(void* str, void* substr) {
    SadString* s = (SadString*)str;
    SadString* sub = (SadString*)substr;
    
    if (s == NULL || sub == NULL || sub->length == 0 || sub->length > s->length) {
        return -1;
    }
    
    for (uint64_t i = 0; i <= s->length - sub->length; i++) {
        if (memcmp(s->data + i, sub->data, sub->length) == 0) {
            return (int64_t)i;
        }
    }
    
    return -1;
}

/**
 * استبدال نص فرعي
 * Replace substring
 */
void* sad_llvm_string_replace(void* str, void* old, void* new_str) {
    SadString* s = (SadString*)str;
    SadString* old_s = (SadString*)old;
    SadString* new_s = (SadString*)new_str;
    
    if (s == NULL || old_s == NULL || new_s == NULL) return str;
    
    int64_t pos = sad_llvm_string_find(str, old);
    if (pos < 0) {
        return sad_llvm_string_new(s->data, s->length);
    }
    
    uint64_t result_len = s->length - old_s->length + new_s->length;
    char* result_data = (char*)sad_llvm_alloc(result_len + 1);
    
    // نسخ الجزء قبل / Copy part before
    sad_llvm_memcpy(result_data, s->data, pos);
    
    // نسخ النص الجديد / Copy new string
    sad_llvm_memcpy(result_data + pos, new_s->data, new_s->length);
    
    // نسخ الجزء بعد / Copy part after
    sad_llvm_memcpy(result_data + pos + new_s->length,
                    s->data + pos + old_s->length,
                    s->length - pos - old_s->length);
    
    result_data[result_len] = '\0';
    
    void* result = sad_llvm_string_new(result_data, result_len);
    sad_llvm_free(result_data);
    
    return result;
}

/**
 * تقسيم النص
 * Split string
 */
void* sad_llvm_string_split(void* str, void* delimiter) {
    SadString* s = (SadString*)str;
    SadString* delim = (SadString*)delimiter;
    
    if (s == NULL || delim == NULL) return NULL;
    
    void* result_arr = sad_llvm_array_new(16);
    
    if (delim->length == 0) {
        // تقسيم كل حرف / Split each character
        for (uint64_t i = 0; i < s->length; i++) {
            void* char_str = sad_llvm_string_new(s->data + i, 1);
            SadValue val = {SAD_TYPE_STRING, {.ptr = char_str}};
            sad_llvm_array_push(result_arr, val);
        }
        return result_arr;
    }
    
    uint64_t start = 0;
    for (uint64_t i = 0; i <= s->length - delim->length; i++) {
        if (memcmp(s->data + i, delim->data, delim->length) == 0) {
            void* substr = sad_llvm_string_new(s->data + start, i - start);
            SadValue val = {SAD_TYPE_STRING, {.ptr = substr}};
            sad_llvm_array_push(result_arr, val);
            
            i += delim->length - 1;
            start = i + 1;
        }
    }
    
    // الجزء الأخير / Last part
    if (start < s->length) {
        void* substr = sad_llvm_string_new(s->data + start, s->length - start);
        SadValue val = {SAD_TYPE_STRING, {.ptr = substr}};
        sad_llvm_array_push(result_arr, val);
    }
    
    return result_arr;
}

/**
 * تحويل إلى أحرف كبيرة
 * Convert to uppercase
 */
void* sad_llvm_string_to_upper(void* str) {
    SadString* s = (SadString*)str;
    if (s == NULL) return NULL;
    
    char* upper_data = (char*)sad_llvm_alloc(s->length + 1);
    for (uint64_t i = 0; i < s->length; i++) {
        upper_data[i] = toupper((unsigned char)s->data[i]);
    }
    upper_data[s->length] = '\0';
    
    void* result = sad_llvm_string_new(upper_data, s->length);
    sad_llvm_free(upper_data);
    
    return result;
}

/**
 * تحويل إلى أحرف صغيرة
 * Convert to lowercase
 */
void* sad_llvm_string_to_lower(void* str) {
    SadString* s = (SadString*)str;
    if (s == NULL) return NULL;
    
    char* lower_data = (char*)sad_llvm_alloc(s->length + 1);
    for (uint64_t i = 0; i < s->length; i++) {
        lower_data[i] = tolower((unsigned char)s->data[i]);
    }
    lower_data[s->length] = '\0';
    
    void* result = sad_llvm_string_new(lower_data, s->length);
    sad_llvm_free(lower_data);
    
    return result;
}

/**
 * إزالة المسافات
 * Trim whitespace
 */
void* sad_llvm_string_trim(void* str) {
    SadString* s = (SadString*)str;
    if (s == NULL || s->length == 0) return str;
    
    // البحث عن البداية / Find start
    uint64_t start = 0;
    while (start < s->length && isspace((unsigned char)s->data[start])) {
        start++;
    }
    
    // البحث عن النهاية / Find end
    uint64_t end = s->length;
    while (end > start && isspace((unsigned char)s->data[end - 1])) {
        end--;
    }
    
    return sad_llvm_string_new(s->data + start, end - start);
}

// يتبع في الملف التالي...
// Continued in next file...
