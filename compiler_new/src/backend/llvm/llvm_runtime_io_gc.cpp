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
// _strdup is MSVC-specific
#define _strdup strdup
#endif

// External declarations from llvm_runtime.cpp
extern "C" void *sad_llvm_string_new(const char *data, uint64_t length);
extern "C" void *sad_llvm_string_from_cstr(const char *cstr);
extern "C" uint64_t sad_llvm_string_length(void *str);
extern "C" void *sad_llvm_alloc(uint64_t size);
extern "C" void sad_llvm_free(void *ptr);

// GC context defined in llvm_runtime.cpp
typedef struct
{
    void **objects;
    uint64_t *sizes;
    uint64_t count;
    uint64_t capacity;
    uint64_t total_allocated;
    uint64_t collections;
    int paused;
    void **roots;
    uint64_t root_count;
    uint64_t root_capacity;
} GCContext;
extern GCContext gc_context;
extern FILE *open_files[256];

// Helper functions from llvm_runtime.cpp
extern void gc_init();
extern void gc_expand();
extern int gc_find_object(void *ptr);

// ============================================================================
// I/O Operations / عمليات الإدخال والإخراج
// ============================================================================

/**
 * (AR) دالة مساعدة لتحضير stdin للقراءة التفاعلية على Windows
 * (EN) Helper to prepare stdin for interactive reading on Windows
 *
 * ConPTY (مثل طرفية VS Code) يُرسل تسلسلات هروب ANSI إلى stdin
 * (مثل رد موقع المؤشر \x1b[row;colR الذي يحتوي على عدد الأسطر).
 * هذه الدالة تنتظر قليلاً ثم تُنظّف كل البيانات الوهمية.
 */
#ifdef _WIN32
#include <conio.h>
static DWORD _saved_console_mode = 0;
static bool _console_mode_saved = false;

static void prepareStdinForRead()
{
    fflush(stdout);

    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE)
        return;

    DWORD mode = 0;
    if (!GetConsoleMode(hStdin, &mode))
        return; // ليس console (pipe) — لا حاجة للتنظيف

    // حفظ الوضع الأصلي
    _saved_console_mode = mode;
    _console_mode_saved = true;

    // (1) تعطيل ENABLE_VIRTUAL_TERMINAL_INPUT لمنع تسلسلات هروب جديدة
    DWORD newMode = (mode & ~0x0200u);
    newMode |= (ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(hStdin, newMode);

    // (2) انتظار قصير ليصل أي escape sequence متأخر من ConPTY
    Sleep(50);

    // (3) تنظيف كل أحداث الإدخال المُعلّقة (بما فيها بقايا escape sequences)
    DWORD numEvents = 0;
    while (GetNumberOfConsoleInputEvents(hStdin, &numEvents) && numEvents > 0)
    {
        INPUT_RECORD irBuf[256];
        DWORD eventsRead = 0;
        ReadConsoleInputW(hStdin, irBuf, (numEvents < 256 ? numEvents : 256), &eventsRead);
        if (eventsRead == 0)
            break;
    }

    // (4) تنظيف مخزن C Runtime
    fflush(stdin);
}

static void restoreStdinAfterRead()
{
    if (!_console_mode_saved)
        return;

    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin != INVALID_HANDLE_VALUE)
    {
        SetConsoleMode(hStdin, _saved_console_mode);
    }
    _console_mode_saved = false;
}
#else
static void prepareStdinForRead() { fflush(stdout); }
static void restoreStdinAfterRead() {}
#endif

/**
 * طباعة قيمة
 * Print value
 */
void sad_llvm_print(SadValue value)
{
    switch (value.type)
    {
    case SAD_TYPE_NULL:
        printf("null");
        break;
    case SAD_TYPE_INT:
        printf("%lld", (long long)value.data.i64);
        break;
    case SAD_TYPE_FLOAT:
    {
        // (AR) إصلاح: 6 خانات عشرية + حذف أصفار زائدة لمطابقة المفسر
        // (EN) Fix: 6 decimal places + strip trailing zeros to match interpreter
        char buf[64];
        snprintf(buf, sizeof(buf), "%.6f", value.data.f64);
        char *dot = strchr(buf, '.');
        if (dot)
        {
            char *end = buf + strlen(buf) - 1;
            while (end > dot && *end == '0')
                end--;
            if (*end == '.')
                end--;
            *(end + 1) = '\0';
        }
        fputs(buf, stdout);
        break;
    }
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
void sad_llvm_println(SadValue value)
{
    sad_llvm_print(value);
    printf("\n");
}

/**
 * طباعة نص
 * Print string
 */
void sad_llvm_print_string(void *str)
{
    if (str == NULL)
    {
        printf("null");
        return;
    }

    SadString *s = (SadString *)str;
    printf("%.*s", (int)s->length, s->data);
}

/**
 * طباعة عدد صحيح
 * Print integer
 */
void sad_llvm_print_int(int64_t value)
{
    printf("%lld", (long long)value);
}

/**
 * (AR) طباعة عدد عشري — 6 خانات عشرية + حذف أصفار زائدة لمطابقة المفسر
 * (EN) Print float — 6 decimal places + strip trailing zeros to match interpreter
 */
void sad_llvm_print_float(double value)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%.6f", value);
    char *dot = strchr(buf, '.');
    if (dot)
    {
        char *end = buf + strlen(buf) - 1;
        while (end > dot && *end == '0')
            end--;
        if (*end == '.')
            end--;
        *(end + 1) = '\0';
    }
    fputs(buf, stdout);
}

/**
 * قراءة سطر - تُرجع SadString*
 * Read line - returns SadString*
 */
void *sad_llvm_input()
{
    char buffer[4096];
    prepareStdinForRead();
    char *ret = fgets(buffer, sizeof(buffer), stdin);
    restoreStdinAfterRead();

    if (ret == NULL)
    {
        return sad_llvm_string_from_cstr("");
    }

    // إزالة السطر الجديد و \r / Remove newline and \r
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
    {
        buffer[--len] = '\0';
    }

    return sad_llvm_string_new(buffer, len);
}

/**
 * قراءة سطر - تُرجع char* مباشرة (للتوافق مع printf)
 * Read line - returns char* directly (for printf compatibility)
 */
const char *sad_llvm_input_cstr()
{
    char buffer[4096];
    prepareStdinForRead();
    char *ret = fgets(buffer, sizeof(buffer), stdin);
    restoreStdinAfterRead();

    if (ret)
    {
        size_t len = strlen(buffer);
        while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
        {
            buffer[--len] = '\0';
        }
        // إرجاع نسخة مستقلة حتى لا تتداخل المتغيرات
        // Return a heap-allocated copy so each variable gets its own string
        return _strdup(buffer);
    }
    return _strdup("");
}

/**
 * قراءة عدد صحيح
 * Read integer
 */
int64_t sad_llvm_input_int()
{
    int64_t value = 0;
    prepareStdinForRead();
    // إصلاح: التحقق من نجاح scanf
    if (scanf("%lld", (long long *)&value) != 1)
    {
        value = 0; // قيمة افتراضية عند الفشل
    }
    // تنظيف المخزن المؤقت / Clear buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    restoreStdinAfterRead();

    return value;
}

/**
 * قراءة عدد عشري
 * Read float
 */
double sad_llvm_input_float()
{
    double value = 0.0;
    prepareStdinForRead();
    // إصلاح: التحقق من نجاح scanf
    if (scanf("%lf", &value) != 1)
    {
        value = 0.0; // قيمة افتراضية عند الفشل
    }
    // تنظيف المخزن المؤقت / Clear buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    restoreStdinAfterRead();

    return value;
}

/**
 * فتح ملف
 * Open file
 */
int64_t sad_llvm_file_open(void *filename, void *mode)
{
    if (filename == NULL || mode == NULL)
        return -1;

    SadString *fname = (SadString *)filename;
    SadString *fmode = (SadString *)mode;

    // تحويل إلى C strings / Convert to C strings
    // إصلاح: التحقق من نجاح malloc
    char *fname_cstr = (char *)malloc(fname->length + 1);
    char *fmode_cstr = (char *)malloc(fmode->length + 1);

    if (!fname_cstr || !fmode_cstr)
    {
        free(fname_cstr); // آمن حتى لو null
        free(fmode_cstr);
        return -1; // فشل تخصيص الذاكرة
    }

    memcpy(fname_cstr, fname->data, fname->length);
    fname_cstr[fname->length] = '\0';

    memcpy(fmode_cstr, fmode->data, fmode->length);
    fmode_cstr[fmode->length] = '\0';

    FILE *file = fopen(fname_cstr, fmode_cstr);

    free(fname_cstr);
    free(fmode_cstr);

    if (file == NULL)
    {
        return -1;
    }

    // البحث عن فتحة فارغة / Find empty slot
    for (int i = 0; i < 256; i++)
    {
        if (open_files[i] == NULL)
        {
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
void sad_llvm_file_close(int64_t fd)
{
    if (fd < 0 || fd >= 256)
        return;

    if (open_files[fd] != NULL)
    {
        fclose(open_files[fd]);
        open_files[fd] = NULL;
    }
}

/**
 * قراءة من ملف
 * Read from file
 */
void *sad_llvm_file_read(int64_t fd, uint64_t size)
{
    if (fd < 0 || fd >= 256 || open_files[fd] == NULL)
    {
        return NULL;
    }

    char *buffer = (char *)sad_llvm_alloc(size + 1);
    if (buffer == NULL)
        return NULL;

    size_t bytes_read = fread(buffer, 1, size, open_files[fd]);
    buffer[bytes_read] = '\0';

    void *result = sad_llvm_string_new(buffer, bytes_read);
    sad_llvm_free(buffer);

    return result;
}

/**
 * كتابة إلى ملف
 * Write to file
 */
uint64_t sad_llvm_file_write(int64_t fd, void *data, uint64_t size)
{
    if (fd < 0 || fd >= 256 || open_files[fd] == NULL || data == NULL)
    {
        return 0;
    }

    SadString *str = (SadString *)data;
    return fwrite(str->data, 1, str->length, open_files[fd]);
}

// ============================================================================
// Garbage Collector Integration / تكامل جامع القمامة
// ============================================================================

/**
 * تسجيل كائن مع GC
 * Register object with GC
 */
void sad_llvm_gc_register(void *ptr, uint64_t size)
{
    if (ptr == NULL)
        return;

    gc_init();

    // توسيع إذا لزم الأمر / Expand if needed
    if (gc_context.count >= gc_context.capacity)
    {
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
void sad_llvm_gc_unregister(void *ptr)
{
    if (ptr == NULL)
        return;

    int index = gc_find_object(ptr);
    if (index < 0)
        return;

    // تحديث الإحصائيات / Update stats
    gc_context.total_allocated -= gc_context.sizes[index];

    // إزالة من القائمة / Remove from list
    for (uint64_t i = index; i < gc_context.count - 1; i++)
    {
        gc_context.objects[i] = gc_context.objects[i + 1];
        gc_context.sizes[i] = gc_context.sizes[i + 1];
    }

    gc_context.count--;
}

/**
 * تشغيل جامع القمامة
 * Trigger garbage collection
 */
void sad_llvm_gc_collect()
{
    if (gc_context.paused)
        return;

    gc_context.collections++;

    // (AR) خوارزمية mark-and-sweep كاملة مع تتبع الجذور
    // (EN) Full mark-and-sweep algorithm with root tracing

    if (gc_context.count == 0)
        return;

    // المرحلة 1: وضع علامة على جميع الكائنات كغير محددة / Phase 1: Mark all as unmarked
    std::vector<bool> marked(gc_context.count, false);

    // المرحلة 2: وضع علامة على الجذور وتتبع المراجع
    // Phase 2: Mark roots and trace references
    if (gc_context.root_count > 0 && gc_context.roots != nullptr)
    {
        // (AR) تتبع من الجذور المسجّلة
        // (EN) Trace from registered roots
        for (uint64_t r = 0; r < gc_context.root_count; r++)
        {
            void *rootPtr = gc_context.roots[r];
            if (rootPtr == nullptr)
                continue;

            // (AR) البحث عن هذا الجذر في قائمة الكائنات
            // (EN) Find this root in the object list
            int idx = gc_find_object(rootPtr);
            if (idx >= 0 && !marked[idx])
            {
                marked[idx] = true;

                // (AR) تتبع المراجع الداخلية — نفحص محتوى الكائن
                // كل 8 بايت قد يكون مؤشراً لكائن آخر
                // (EN) Trace internal references — scan object contents
                // every 8 bytes may be a pointer to another object
                uint64_t objSize = gc_context.sizes[idx];
                void **objData = (void **)rootPtr;
                uint64_t ptrCount = objSize / sizeof(void *);

                for (uint64_t p = 0; p < ptrCount; p++)
                {
                    void *maybeRef = objData[p];
                    if (maybeRef == nullptr)
                        continue;
                    int refIdx = gc_find_object(maybeRef);
                    if (refIdx >= 0 && !marked[refIdx])
                    {
                        marked[refIdx] = true;
                    }
                }
            }
        }
    }
    else
    {
        // (AR) لا توجد جذور مسجّلة — نعتبر جميع الكائنات حيّة (سلوك محافظ)
        // (EN) No roots registered — consider all objects alive (conservative)
        for (uint64_t i = 0; i < gc_context.count; i++)
        {
            if (gc_context.objects[i] != nullptr)
            {
                marked[i] = true;
            }
        }
    }

    // المرحلة 3: مسح الكائنات غير المُعلَّمة / Phase 3: Sweep unmarked objects
    uint64_t freed = 0;
    for (uint64_t i = 0; i < gc_context.count;)
    {
        if (!marked[i] && gc_context.objects[i] != nullptr)
        {
            // تحرير الكائن / Free object
            free(gc_context.objects[i]);
            gc_context.objects[i] = nullptr;

            // إزالة من القائمة / Remove from list
            for (uint64_t j = i; j < gc_context.count - 1; j++)
            {
                gc_context.objects[j] = gc_context.objects[j + 1];
                gc_context.sizes[j] = gc_context.sizes[j + 1];
                marked[j] = marked[j + 1];
            }
            gc_context.count--;
            freed++;
        }
        else
        {
            i++;
        }
    }

    // تحديث الإحصائيات / Update statistics
    if (freed > 0)
    {
        // في تنفيذ كامل، نحتفظ بإحصائيات التحرير
        // In full implementation, keep free statistics
    }
}

/**
 * جمع تدريجي
 * Incremental collection
 */
void sad_llvm_gc_collect_incremental(uint64_t steps)
{
    if (gc_context.paused)
        return;

    // جمع تدريجي — مرحلة وضع/مسح
    // Incremental collection — mark/sweep phases

    static uint64_t current_phase = 0; // 0=mark, 1=sweep
    static uint64_t current_index = 0;
    static std::vector<bool> incremental_marked;

    if (current_phase == 0)
    {
        // مرحلة الوضع / Mark phase
        if (current_index == 0)
        {
            // تهيئة / Initialize
            incremental_marked.resize(gc_context.count, false);
        }

        // وضع علامة على عدد من الكائنات / Mark some objects
        uint64_t end = (std::min)(current_index + steps, gc_context.count);
        for (uint64_t i = current_index; i < end; i++)
        {
            if (gc_context.objects[i] != nullptr)
            {
                incremental_marked[i] = true;
            }
        }

        current_index = end;
        if (current_index >= gc_context.count)
        {
            // انتقل لمرحلة المسح / Move to sweep phase
            current_phase = 1;
            current_index = 0;
        }
    }
    else
    {
        // مرحلة المسح / Sweep phase
        uint64_t swept = 0;

        while (current_index < gc_context.count && swept < steps)
        {
            if (!incremental_marked[current_index] && gc_context.objects[current_index] != nullptr)
            {
                // تحرير / Free
                free(gc_context.objects[current_index]);
                gc_context.objects[current_index] = nullptr;

                // إزالة / Remove
                for (uint64_t j = current_index; j < gc_context.count - 1; j++)
                {
                    gc_context.objects[j] = gc_context.objects[j + 1];
                    gc_context.sizes[j] = gc_context.sizes[j + 1];
                    incremental_marked[j] = incremental_marked[j + 1];
                }
                gc_context.count--;
            }
            else
            {
                current_index++;
            }
            swept++;
        }

        if (current_index >= gc_context.count)
        {
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
void sad_llvm_gc_stats(uint64_t *allocated, uint64_t *used, uint64_t *collections)
{
    if (allocated)
        *allocated = gc_context.total_allocated;
    if (used)
        *used = gc_context.total_allocated; // Simplified
    if (collections)
        *collections = gc_context.collections;
}

/**
 * تعليق GC
 * Pause GC
 */
void sad_llvm_gc_pause()
{
    gc_context.paused = 1;
}

/**
 * استئناف GC
 * Resume GC
 */
void sad_llvm_gc_resume()
{
    gc_context.paused = 0;
}

/**
 * إضافة جذر GC
 * Add GC root
 */
void sad_llvm_gc_add_root(void *ptr)
{
    if (ptr == NULL)
        return;

    gc_init();

    // توسيع إذا لزم الأمر / Expand if needed
    if (gc_context.root_count >= gc_context.root_capacity)
    {
        gc_context.root_capacity *= 2;
        gc_context.roots = (void **)realloc(gc_context.roots,
                                            gc_context.root_capacity * sizeof(void *));
    }

    gc_context.roots[gc_context.root_count++] = ptr;
}

/**
 * إزالة جذر GC
 * Remove GC root
 */
void sad_llvm_gc_remove_root(void *ptr)
{
    if (ptr == NULL)
        return;

    for (uint64_t i = 0; i < gc_context.root_count; i++)
    {
        if (gc_context.roots[i] == ptr)
        {
            // إزالة / Remove
            for (uint64_t j = i; j < gc_context.root_count - 1; j++)
            {
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
void *sad_llvm_int_to_string(int64_t value)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lld", (long long)value);
    return sad_llvm_string_from_cstr(buffer);
}

/**
 * عدد عشري إلى نص
 * Float to string
 */
void *sad_llvm_float_to_string(double value)
{
    // (AR) إصلاح: 6 خانات عشرية + حذف أصفار زائدة لمطابقة المفسر
    // (EN) Fix: 6 decimal places + strip trailing zeros to match interpreter
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.6f", value);
    char *dot = strchr(buffer, '.');
    if (dot)
    {
        char *end = buffer + strlen(buffer) - 1;
        while (end > dot && *end == '0')
            end--;
        if (*end == '.')
            end--;
        *(end + 1) = '\0';
    }
    return sad_llvm_string_from_cstr(buffer);
}

/**
 * نص إلى عدد صحيح
 * String to integer
 */
int64_t sad_llvm_string_to_int(void *str)
{
    if (str == NULL)
        return 0;

    SadString *s = (SadString *)str;

    // نسخ مؤقتة لإضافة null terminator / Temp copy for null terminator
    char *temp = (char *)malloc(s->length + 1);
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
double sad_llvm_string_to_float(void *str)
{
    if (str == NULL)
        return 0.0;

    SadString *s = (SadString *)str;

    char *temp = (char *)malloc(s->length + 1);
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
uint8_t sad_llvm_to_bool(SadValue value)
{
    switch (value.type)
    {
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
uint64_t sad_llvm_time_ms()
{
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
void sad_llvm_sleep_ms(uint64_t ms)
{
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
void sad_llvm_exit(int code)
{
    // تنظيف الموارد / Cleanup resources

    // إغلاق الملفات المفتوحة / Close open files
    for (int i = 0; i < 256; i++)
    {
        if (open_files[i] != NULL)
        {
            fclose(open_files[i]);
            open_files[i] = NULL;
        }
    }

    // تنظيف GC / Cleanup GC
    if (gc_context.objects != NULL)
    {
        for (uint64_t i = 0; i < gc_context.count; i++)
        {
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
void sad_llvm_panic(void *message)
{
    fprintf(stderr, "خطأ فادح / Panic: ");
    if (message != NULL)
    {
        sad_llvm_print_string(message);
    }
    else
    {
        fprintf(stderr, "Unknown error");
    }
    fprintf(stderr, "\n");

    sad_llvm_exit(1);
}
