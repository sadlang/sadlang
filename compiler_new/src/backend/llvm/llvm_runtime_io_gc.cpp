/*
 * ============================================================================
 * LLVM Runtime Functions - الجزء الثاني (I/O & GC)
 * LLVM Runtime Functions - Part 2 (I/O & GC)
 * ============================================================================
 */

#include "llvm_runtime.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// External declarations from llvm_runtime.cpp
extern void* sad_llvm_string_new(const char* data, uint64_t length);
extern void* sad_llvm_string_from_cstr(const char* cstr);
extern uint64_t sad_llvm_string_length(void* str);

// External GC context (defined in llvm_runtime.cpp)
typedef struct {
    void** objects;
    uint64_t* sizes;
    uint64_t count;
    uint64_t capacity;
    uint64_t total_allocated;
    uint64_t collections;
    int paused;
    void** roots;
    uint64_t root_count;
    uint64_t root_capacity;
} GCContext;

extern GCContext gc_context;
extern FILE* open_files[256];

// Helper functions from llvm_runtime.cpp
extern void gc_init();
extern void gc_expand();
extern int gc_find_object(void* ptr);

// ============================================================================
// I/O Operations / عمليات الإدخال والإخراج
// ============================================================================

/**
 * طباعة قيمة
 * Print value
 */
void sad_llvm_print(SadValue value) {
    switch (value.type) {
        case SAD_TYPE_NULL:
            printf("null");
            break;
        case SAD_TYPE_INT:
            printf("%lld", (long long)value.data.i64);
            break;
        case SAD_TYPE_FLOAT:
            printf("%g", value.data.f64);
            break;
        case SAD_TYPE_BOOL:
            printf("%s", value.data.boolean ? "true" : "false");
            break;
        case SAD_TYPE_STRING:
            sad_llvm_print_string(value.data.ptr);
            break;
        case SAD_TYPE_ARRAY:
            printf("[Array]");
            break;
        case SAD_TYPE_OBJECT:
            printf("[Object]");
            break;
        default:
            printf("[Unknown]");
            break;
    }
}

/**
 * طباعة قيمة مع سطر جديد
 * Print value with newline
 */
void sad_llvm_println(SadValue value) {
    sad_llvm_print(value);
    printf("\n");
}

/**
 * طباعة نص
 * Print string
 */
void sad_llvm_print_string(void* str) {
    if (str == NULL) {
        printf("null");
        return;
    }
    
    SadString* s = (SadString*)str;
    printf("%.*s", (int)s->length, s->data);
}

/**
 * طباعة عدد صحيح
 * Print integer
 */
void sad_llvm_print_int(int64_t value) {
    printf("%lld", (long long)value);
}

/**
 * طباعة عدد عشري
 * Print float
 */
void sad_llvm_print_float(double value) {
    printf("%g", value);
}

/**
 * قراءة سطر
 * Read line
 */
void* sad_llvm_input() {
    char buffer[4096];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return sad_llvm_string_from_cstr("");
    }
    
    // إزالة السطر الجديد / Remove newline
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
        len--;
    }
    
    return sad_llvm_string_new(buffer, len);
}

/**
 * قراءة عدد صحيح
 * Read integer
 */
int64_t sad_llvm_input_int() {
    int64_t value = 0;
    scanf("%lld", (long long*)&value);
    
    // تنظيف المخزن المؤقت / Clear buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    return value;
}

/**
 * قراءة عدد عشري
 * Read float
 */
double sad_llvm_input_float() {
    double value = 0.0;
    scanf("%lf", &value);
    
    // تنظيف المخزن المؤقت / Clear buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    return value;
}

/**
 * فتح ملف
 * Open file
 */
int64_t sad_llvm_file_open(void* filename, void* mode) {
    if (filename == NULL || mode == NULL) return -1;
    
    SadString* fname = (SadString*)filename;
    SadString* fmode = (SadString*)mode;
    
    // تحويل إلى C strings / Convert to C strings
    char* fname_cstr = (char*)malloc(fname->length + 1);
    char* fmode_cstr = (char*)malloc(fmode->length + 1);
    
    memcpy(fname_cstr, fname->data, fname->length);
    fname_cstr[fname->length] = '\0';
    
    memcpy(fmode_cstr, fmode->data, fmode->length);
    fmode_cstr[fmode->length] = '\0';
    
    FILE* file = fopen(fname_cstr, fmode_cstr);
    
    free(fname_cstr);
    free(fmode_cstr);
    
    if (file == NULL) {
        return -1;
    }
    
    // البحث عن فتحة فارغة / Find empty slot
    for (int i = 0; i < 256; i++) {
        if (open_files[i] == NULL) {
            open_files[i] = file;
            return i;
        }
    }
    
    // لا توجد فتحات متاحة / No slots available
    fclose(file);
    return -1;
}

/**
 * إغلاق ملف
 * Close file
 */
void sad_llvm_file_close(int64_t fd) {
    if (fd < 0 || fd >= 256) return;
    
    if (open_files[fd] != NULL) {
        fclose(open_files[fd]);
        open_files[fd] = NULL;
    }
}

/**
 * قراءة من ملف
 * Read from file
 */
void* sad_llvm_file_read(int64_t fd, uint64_t size) {
    if (fd < 0 || fd >= 256 || open_files[fd] == NULL) {
        return NULL;
    }
    
    char* buffer = (char*)sad_llvm_alloc(size + 1);
    if (buffer == NULL) return NULL;
    
    size_t bytes_read = fread(buffer, 1, size, open_files[fd]);
    buffer[bytes_read] = '\0';
    
    void* result = sad_llvm_string_new(buffer, bytes_read);
    sad_llvm_free(buffer);
    
    return result;
}

/**
 * كتابة إلى ملف
 * Write to file
 */
uint64_t sad_llvm_file_write(int64_t fd, void* data, uint64_t size) {
    if (fd < 0 || fd >= 256 || open_files[fd] == NULL || data == NULL) {
        return 0;
    }
    
    SadString* str = (SadString*)data;
    return fwrite(str->data, 1, str->length, open_files[fd]);
}

// ============================================================================
// Garbage Collector Integration / تكامل جامع القمامة
// ============================================================================

/**
 * تسجيل كائن مع GC
 * Register object with GC
 */
void sad_llvm_gc_register(void* ptr, uint64_t size) {
    if (ptr == NULL) return;
    
    gc_init();
    
    // توسيع إذا لزم الأمر / Expand if needed
    if (gc_context.count >= gc_context.capacity) {
        gc_expand();
    }
    
    gc_context.objects[gc_context.count] = ptr;
    gc_context.sizes[gc_context.count] = size;
    gc_context.count++;
    gc_context.total_allocated += size;
}

/**
 * إلغاء تسجيل كائن
 * Unregister object
 */
void sad_llvm_gc_unregister(void* ptr) {
    if (ptr == NULL) return;
    
    int index = gc_find_object(ptr);
    if (index < 0) return;
    
    // تحديث الإحصائيات / Update stats
    gc_context.total_allocated -= gc_context.sizes[index];
    
    // إزالة من القائمة / Remove from list
    for (uint64_t i = index; i < gc_context.count - 1; i++) {
        gc_context.objects[i] = gc_context.objects[i + 1];
        gc_context.sizes[i] = gc_context.sizes[i + 1];
    }
    
    gc_context.count--;
}

/**
 * تشغيل جامع القمامة
 * Trigger garbage collection
 */
void sad_llvm_gc_collect() {
    if (gc_context.paused) return;
    
    gc_context.collections++;
    
    // ✅ TODO 5: تنفيذ خوارزمية mark-and-sweep / Implement mark-and-sweep algorithm
    
    // المرحلة 1: وضع علامة على جميع الكائنات كغير محددة / Phase 1: Mark all as unmarked
    std::vector<bool> marked(gc_context.count, false);
    
    // المرحلة 2: وضع علامة على الجذور / Phase 2: Mark roots
    // في هذا التنفيذ المبسط، نعتبر جميع الكائنات جذوراً
    // In this simplified implementation, consider all objects as roots
    for (uint64_t i = 0; i < gc_context.count; i++) {
        if (gc_context.objects[i] != nullptr) {
            marked[i] = true;
            // TODO: في تنفيذ كامل، نحتاج لتتبع المراجع
            // TODO: In full implementation, need to trace references
        }
    }
    
    // المرحلة 3: مسح الكائنات غير المُعلَّمة / Phase 3: Sweep unmarked objects
    uint64_t freed = 0;
    for (uint64_t i = 0; i < gc_context.count; ) {
        if (!marked[i] && gc_context.objects[i] != nullptr) {
            // تحرير الكائن / Free object
            free(gc_context.objects[i]);
            gc_context.objects[i] = nullptr;
            
            // إزالة من القائمة / Remove from list
            for (uint64_t j = i; j < gc_context.count - 1; j++) {
                gc_context.objects[j] = gc_context.objects[j + 1];
                gc_context.sizes[j] = gc_context.sizes[j + 1];
                marked[j] = marked[j + 1];
            }
            gc_context.count--;
            freed++;
        } else {
            i++;
        }
    }
    
    // تحديث الإحصائيات / Update statistics
    if (freed > 0) {
        // في تنفيذ كامل، نحتفظ بإحصائيات التحرير
        // In full implementation, keep free statistics
    }
}

/**
 * جمع تدريجي
 * Incremental collection
 */
void sad_llvm_gc_collect_incremental(uint64_t steps) {
    if (gc_context.paused) return;
    
    // ✅ TODO 5: تنفيذ جمع تدريجي / Implement incremental collection
    
    // جمع تدريجي - نجمع عدد محدود من الكائنات في كل خطوة
    // Incremental collection - collect limited objects per step
    
    static uint64_t current_phase = 0; // 0=mark, 1=sweep
    static uint64_t current_index = 0;
    static std::vector<bool> incremental_marked;
    
    if (current_phase == 0) {
        // مرحلة الوضع / Mark phase
        if (current_index == 0) {
            // تهيئة / Initialize
            incremental_marked.resize(gc_context.count, false);
        }
        
        // وضع علامة على عدد من الكائنات / Mark some objects
        uint64_t end = (std::min)(current_index + steps, gc_context.count);
        for (uint64_t i = current_index; i < end; i++) {
            if (gc_context.objects[i] != nullptr) {
                incremental_marked[i] = true;
            }
        }
        
        current_index = end;
        if (current_index >= gc_context.count) {
            // انتقل لمرحلة المسح / Move to sweep phase
            current_phase = 1;
            current_index = 0;
        }
    } else {
        // مرحلة المسح / Sweep phase
        uint64_t swept = 0;
        
        while (current_index < gc_context.count && swept < steps) {
            if (!incremental_marked[current_index] && gc_context.objects[current_index] != nullptr) {
                // تحرير / Free
                free(gc_context.objects[current_index]);
                gc_context.objects[current_index] = nullptr;
                
                // إزالة / Remove
                for (uint64_t j = current_index; j < gc_context.count - 1; j++) {
                    gc_context.objects[j] = gc_context.objects[j + 1];
                    gc_context.sizes[j] = gc_context.sizes[j + 1];
                    incremental_marked[j] = incremental_marked[j + 1];
                }
                gc_context.count--;
            } else {
                current_index++;
            }
            swept++;
        }
        
        if (current_index >= gc_context.count) {
            // اكتملت الدورة / Cycle complete
            current_phase = 0;
            current_index = 0;
            incremental_marked.clear();
            gc_context.collections++;
        }
    }
}

/**
 * إحصائيات GC
 * GC statistics
 */
void sad_llvm_gc_stats(uint64_t* allocated, uint64_t* used, uint64_t* collections) {
    if (allocated) *allocated = gc_context.total_allocated;
    if (used) *used = gc_context.total_allocated; // Simplified
    if (collections) *collections = gc_context.collections;
}

/**
 * تعليق GC
 * Pause GC
 */
void sad_llvm_gc_pause() {
    gc_context.paused = 1;
}

/**
 * استئناف GC
 * Resume GC
 */
void sad_llvm_gc_resume() {
    gc_context.paused = 0;
}

/**
 * إضافة جذر GC
 * Add GC root
 */
void sad_llvm_gc_add_root(void* ptr) {
    if (ptr == NULL) return;
    
    gc_init();
    
    // توسيع إذا لزم الأمر / Expand if needed
    if (gc_context.root_count >= gc_context.root_capacity) {
        gc_context.root_capacity *= 2;
        gc_context.roots = (void**)realloc(gc_context.roots,
                                          gc_context.root_capacity * sizeof(void*));
    }
    
    gc_context.roots[gc_context.root_count++] = ptr;
}

/**
 * إزالة جذر GC
 * Remove GC root
 */
void sad_llvm_gc_remove_root(void* ptr) {
    if (ptr == NULL) return;
    
    for (uint64_t i = 0; i < gc_context.root_count; i++) {
        if (gc_context.roots[i] == ptr) {
            // إزالة / Remove
            for (uint64_t j = i; j < gc_context.root_count - 1; j++) {
                gc_context.roots[j] = gc_context.roots[j + 1];
            }
            gc_context.root_count--;
            return;
        }
    }
}

// ============================================================================
// Type Conversion / تحويل الأنواع
// ============================================================================

/**
 * عدد صحيح إلى نص
 * Integer to string
 */
void* sad_llvm_int_to_string(int64_t value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lld", (long long)value);
    return sad_llvm_string_from_cstr(buffer);
}

/**
 * عدد عشري إلى نص
 * Float to string
 */
void* sad_llvm_float_to_string(double value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%g", value);
    return sad_llvm_string_from_cstr(buffer);
}

/**
 * نص إلى عدد صحيح
 * String to integer
 */
int64_t sad_llvm_string_to_int(void* str) {
    if (str == NULL) return 0;
    
    SadString* s = (SadString*)str;
    
    // نسخ مؤقتة لإضافة null terminator / Temp copy for null terminator
    char* temp = (char*)malloc(s->length + 1);
    memcpy(temp, s->data, s->length);
    temp[s->length] = '\0';
    
    int64_t value = atoll(temp);
    free(temp);
    
    return value;
}

/**
 * نص إلى عدد عشري
 * String to float
 */
double sad_llvm_string_to_float(void* str) {
    if (str == NULL) return 0.0;
    
    SadString* s = (SadString*)str;
    
    char* temp = (char*)malloc(s->length + 1);
    memcpy(temp, s->data, s->length);
    temp[s->length] = '\0';
    
    double value = atof(temp);
    free(temp);
    
    return value;
}

/**
 * تحويل إلى منطقي
 * Convert to boolean
 */
uint8_t sad_llvm_to_bool(SadValue value) {
    switch (value.type) {
        case SAD_TYPE_NULL:
            return 0;
        case SAD_TYPE_INT:
            return value.data.i64 != 0;
        case SAD_TYPE_FLOAT:
            return value.data.f64 != 0.0;
        case SAD_TYPE_BOOL:
            return value.data.boolean;
        case SAD_TYPE_STRING:
            return sad_llvm_string_length(value.data.ptr) > 0;
        case SAD_TYPE_ARRAY:
        case SAD_TYPE_OBJECT:
            return value.data.ptr != NULL;
        default:
            return 0;
    }
}

// ============================================================================
// Utility Functions / دوال مساعدة
// ============================================================================

/**
 * الوقت الحالي بالميلي ثانية
 * Current time in milliseconds
 */
uint64_t sad_llvm_time_ms() {
#ifdef _WIN32
    return (uint64_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

/**
 * انتظار (sleep)
 * Sleep
 */
void sad_llvm_sleep_ms(uint64_t ms) {
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    usleep(ms * 1000);
#endif
}

/**
 * الخروج من البرنامج
 * Exit program
 */
void sad_llvm_exit(int code) {
    // تنظيف الموارد / Cleanup resources
    
    // إغلاق الملفات المفتوحة / Close open files
    for (int i = 0; i < 256; i++) {
        if (open_files[i] != NULL) {
            fclose(open_files[i]);
            open_files[i] = NULL;
        }
    }
    
    // تنظيف GC / Cleanup GC
    if (gc_context.objects != NULL) {
        for (uint64_t i = 0; i < gc_context.count; i++) {
            free(gc_context.objects[i]);
        }
        free(gc_context.objects);
        free(gc_context.sizes);
        free(gc_context.roots);
    }
    
    exit(code);
}

/**
 * خطأ فادح
 * Panic with error
 */
void sad_llvm_panic(void* message) {
    fprintf(stderr, "خطأ فادح / Panic: ");
    if (message != NULL) {
        sad_llvm_print_string(message);
    } else {
        fprintf(stderr, "Unknown error");
    }
    fprintf(stderr, "\n");
    
    sad_llvm_exit(1);
}
