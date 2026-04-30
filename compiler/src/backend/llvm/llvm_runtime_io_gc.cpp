/*
 * ============================================================================
 * LLVM Runtime Functions - الجزء الثاني (I/O & GC)
 * LLVM Runtime Functions - Part 2 (I/O & GC)
 * ============================================================================
 */

#include "llvm_runtime.h"
#include "memory/gc/engine/garbage_collector.h" // (AR) Phase B-step2: المحرك الموحَّد
#include "input_sanitizer.h"                    // (AR) تحليل آمن للأرقام بديلاً عن scanf / (EN) safe numeric parsing
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <string>

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

extern FILE *open_files[256];

// ============================================================================
// (AR) Phase B-step2: GCContext + الدوال gc_* انتقلت إلى shared/memory_gc/.
//      كل الدوال sad_llvm_gc_* أدناه أصبحت تفويضاً مباشراً للمحرك الموحَّد
//      Sad::Memory::GC::defaultEngine() (الجسر الوحيد بين runtime sadc والمحرك).
// (EN) GCContext + gc_* helpers are now in shared/memory_gc/. The sad_llvm_gc_*
//      functions below are thin delegates over Sad::Memory::GC::defaultEngine().
// ============================================================================

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
    // (AR) بديل آمن لـ scanf: fgets + safeParseInt (حدود واضحة للمخزن)
    // (EN) safe alternative to scanf: fgets + InputSanitizer::safeParseInt
    char buffer[64];
    if (fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        // إزالة أي سطر جديد متبقٍ
        size_t blen = strlen(buffer);
        while (blen > 0 && (buffer[blen - 1] == '\n' || buffer[blen - 1] == '\r'))
            buffer[--blen] = '\0';
        std::int64_t parsed = 0;
        if (Sad::Security::InputSanitizer::safeParseInt(std::string(buffer), parsed))
        {
            value = parsed;
        }
        // إذا لم يجتز السطر بالكامل (buffer امتلأ)، صرف الباقي
        if (blen + 1 == sizeof(buffer))
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
    }
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
    // (AR) بديل آمن لـ scanf: fgets + safeParseDouble
    // (EN) safe alternative to scanf: fgets + InputSanitizer::safeParseDouble
    char buffer[64];
    if (fgets(buffer, sizeof(buffer), stdin) != nullptr)
    {
        size_t blen = strlen(buffer);
        while (blen > 0 && (buffer[blen - 1] == '\n' || buffer[blen - 1] == '\r'))
            buffer[--blen] = '\0';
        double parsed = 0.0;
        if (Sad::Security::InputSanitizer::safeParseDouble(std::string(buffer), parsed))
        {
            value = parsed;
        }
        if (blen + 1 == sizeof(buffer))
        {
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;
        }
    }
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
//
// (AR) جميع هذه الدوال تفويض رفيع لمحرك Sad::Memory::GC::GarbageCollector
//      المحرَّر في shared/memory_gc/. التطبيق الفعلي (mark-and-sweep، التوسيع،
//      البحث، الجمع التدريجي) موجود هناك. أي تغيير في الخوارزمية يجب أن يحصل
//      في المحرك — لا تعد كتابة منطق GC هنا.
// (EN) Thin delegates over the unified GC engine. All algorithmic logic lives in
//      shared/memory_gc/. Do not re-implement GC logic in this file.

void sad_llvm_gc_register(void *ptr, uint64_t size)
{
    Sad::Memory::GC::defaultEngine().registerObject(ptr, size);
}

void sad_llvm_gc_unregister(void *ptr)
{
    Sad::Memory::GC::defaultEngine().unregisterObject(ptr);
}

void sad_llvm_gc_collect()
{
    Sad::Memory::GC::defaultEngine().collect();
}

void sad_llvm_gc_collect_incremental(uint64_t steps)
{
    Sad::Memory::GC::defaultEngine().collectIncremental(steps);
}

void sad_llvm_gc_stats(uint64_t *allocated, uint64_t *used, uint64_t *collections)
{
    auto stats = Sad::Memory::GC::defaultEngine().getStats();
    if (allocated)
        *allocated = stats.totalAllocated;
    if (used)
        *used = stats.totalAllocated; // (AR) تبسيط مطابق للسلوك القديم
    if (collections)
        *collections = stats.collections;
}

void sad_llvm_gc_pause()
{
    Sad::Memory::GC::defaultEngine().pause();
}

void sad_llvm_gc_resume()
{
    Sad::Memory::GC::defaultEngine().resume();
}

void sad_llvm_gc_add_root(void *ptr)
{
    Sad::Memory::GC::defaultEngine().addRoot(ptr);
}

void sad_llvm_gc_remove_root(void *ptr)
{
    Sad::Memory::GC::defaultEngine().removeRoot(ptr);
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
    // (AR) Phase B-step2: لم نعد ندير الذاكرة هنا — defaultEngine() هو الذي
    //      يحرّر كائناته عبر دورة collect() (إن وُجدت جذور) أو يَخرج برامج
    //      sadc وعندها يحرّر الـ OS كل شيء. آمن للحذف هنا.
    // (EN) GC cleanup is now handled by Sad::Memory::GC::defaultEngine() via
    //      collect(), or implicitly by process exit. Removed redundant cleanup.

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
