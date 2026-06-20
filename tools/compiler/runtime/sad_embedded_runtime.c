/* ============================================================================
 * بسم الله الرحمن الرحيم
 * ============================================================================
 * Sad Language - Embedded Runtime / مكتبة وقت التشغيل المضمّنة
 * ============================================================================
 * الملف: sad_embedded_runtime.c
 * المكوّن: مترجم sadc — وقت التشغيل المضمّن
 * ============================================================================
 * الوصف:
 *   هذا الملف يحتوي على جميع دوال وقت التشغيل (runtime) اللازمة لتشغيل
 *   البرامج المترجمة بواسطة sadc. يُضمّن تلقائياً أثناء البناء ويُكتب
 *   كملف مؤقت .c أثناء عملية الربط (linking).
 *
 *   يحل محل النسختين المكررتين اللتين كانتا مضمّنتين في
 *   compiler_driver_linker.cpp (إصدار MSVC/LLD وإصدار gcc/linux).
 *
 * الدوال المُصدّرة:
 *   - الإدخال: sad_llvm_input, sad_llvm_input_int, sad_llvm_input_float
 *   - الذاكرة: sad_llvm_alloc, sad_llvm_free
 *   - النصوص: sad_llvm_string_new, sad_llvm_string_from_cstr,
 *              sad_llvm_string_length, sad_llvm_print_string
 *   - الأمان: sad_security_assert, sad_security_panic, sad_security_hash,
 *             sad_security_timestamp, sad_security_encrypt, sad_security_decrypt,
 *             sad_security_assert_type, sad_security_assert_equal_str,
 *             sad_security_sanitize, sad_security_secure_random,
 *             sad_security_base64_encode
 *   - الخرائط: __sad_map_create, __sad_map_set, __sad_map_set_typed,
 *              __sad_map_get, __sad_map_get_i64, __sad_map_get_type
 *
 * ملاحظات:
 *   - هذا ملف C خالص (ليس C++) لتجنب مشاكل توافق مكتبات MSVC
 *   - الملف مستقل عن المنصة — يعمل على Windows و Linux و macOS
 *   - يُستخدم `#ifdef _WIN32` للتوافق مع Windows حيث لزم الأمر
 *   - لا يحتوي على دوال __android_log_print (مسار Android منفصل)
 *
 * Description (EN):
 *   This file contains all runtime functions needed by programs compiled
 *   with sadc. It is embedded at build time and written as a temporary
 *   .c file during the linking phase.
 *
 *   Replaces the two duplicate copies that were inline in
 *   compiler_driver_linker.cpp (MSVC/LLD version and gcc/linux version).
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>  /* SIZE_MAX — مطلوب صراحةً على clang/Linux (يتسرّب ضمنًا على MSVC) */
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

/* ============================================================================
 * (AR) دوالّ تغليف التزامن — تجريد عابر للمنصّات للقفل والخيوط.
 *      يستدعيها مولّد LLVM بدل واجهات Win32 الخام (CreateMutexA…) ليعمل
 *      الكود المُولَّد على Linux/macOS عبر pthread أيضًا.
 *      المقبض الرمزيّ (<= 1) ناتج التنفيذ المتزامن لـ go/lambda — يُعامَل no-op.
 * (EN) Cross-platform concurrency wrappers. The LLVM backend calls these instead
 *      of raw Win32 APIs so generated code links/runs on POSIX via pthread.
 *      Symbolic handles (<= 1) come from synchronous go/lambda execution — no-op.
 * ============================================================================ */

void *sad_rt_mutex_create(void)
{
#ifdef _WIN32
    return (void *)CreateMutexA(NULL, FALSE, NULL);
#else
    /* (AR) قفل تعاوديّ ليطابق دلالة Win32 (إعادة الاقتناص من نفس الخيط) —
     *      وإلّا قد يحدث deadlock عند القفل المتداخل (سبب مهلة سويتة الضغط). */
    pthread_mutex_t *m = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (m)
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(m, &attr);
        pthread_mutexattr_destroy(&attr);
    }
    return (void *)m;
#endif
}

void sad_rt_mutex_lock(void *h)
{
    if ((uintptr_t)h <= 1)
        return;
#ifdef _WIN32
    WaitForSingleObject((HANDLE)h, 0xFFFFFFFF);
#else
    pthread_mutex_lock((pthread_mutex_t *)h);
#endif
}

void sad_rt_mutex_unlock(void *h)
{
    if ((uintptr_t)h <= 1)
        return;
#ifdef _WIN32
    ReleaseMutex((HANDLE)h);
#else
    pthread_mutex_unlock((pthread_mutex_t *)h);
#endif
}

/* (AR) يُرجع 0 عند نجاح الاقتناص (مطابق لـ WAIT_OBJECT_0 وpthread_mutex_trylock). */
int sad_rt_mutex_trylock(void *h)
{
    if ((uintptr_t)h <= 1)
        return 0;
#ifdef _WIN32
    return (int)WaitForSingleObject((HANDLE)h, 0);
#else
    return pthread_mutex_trylock((pthread_mutex_t *)h);
#endif
}

void *sad_rt_thread_spawn(void *fn, void *arg)
{
#ifdef _WIN32
    return (void *)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)fn, arg, 0, NULL);
#else
    pthread_t *t = (pthread_t *)malloc(sizeof(pthread_t));
    if (t && pthread_create(t, NULL, (void *(*)(void *))fn, arg) == 0)
        return (void *)t;
    if (t)
        free(t);
    return (void *)0;
#endif
}

void sad_rt_thread_join(void *h)
{
    if ((uintptr_t)h <= 1)
        return; /* مقبض رمزيّ من التنفيذ المتزامن */
#ifdef _WIN32
    WaitForSingleObject((HANDLE)h, 0xFFFFFFFF);
    CloseHandle((HANDLE)h);
#else
    pthread_join(*(pthread_t *)h, NULL);
    free(h);
#endif
}

/* (AR) النوم بالمللي ثانية — Win32 Sleep أو POSIX nanosleep. */
void sad_rt_sleep_ms(unsigned int ms)
{
#ifdef _WIN32
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec = (time_t)(ms / 1000u);
    ts.tv_nsec = (long)((ms % 1000u) * 1000000L);
    nanosleep(&ts, NULL);
#endif
}

/* (AR) أحداث المستقبل (future) — Win32 Event أو علامة بسيطة على POSIX.
 *      النموذج متزامن: القيمة تُخزَّن قبل الانتظار، فالانتظار على POSIX no-op. */
void *sad_rt_event_create(void)
{
#ifdef _WIN32
    return (void *)CreateEventA(NULL, TRUE, FALSE, NULL);
#else
    long *e = (long *)malloc(sizeof(long));
    if (e)
        *e = 0;
    return (void *)e;
#endif
}

void sad_rt_event_set(void *h)
{
    if ((uintptr_t)h <= 1)
        return;
#ifdef _WIN32
    SetEvent((HANDLE)h);
#else
    *(volatile long *)h = 1;
#endif
}

void sad_rt_event_wait(void *h)
{
    if ((uintptr_t)h <= 1)
        return;
#ifdef _WIN32
    WaitForSingleObject((HANDLE)h, 0xFFFFFFFF);
#else
    /* (AR) النموذج متزامن: القيمة محسومة سلفًا — لا حاجة لانتظار فعليّ. */
    (void)h;
#endif
}

/* ============================================================================
 * دوال الإدخال / Input Functions
 * ============================================================================ */

/* قراءة نص من المستخدم / Read string from user */
const char *sad_llvm_input(void)
{
    /* ملاحظة: المخزن المؤقت الثابت ليس آمناً للخيوط المتعددة */
    /* في الإصدارات المستقبلية، يجب استخدام thread_local أو تخصيص ديناميكي */
    static char buffer[4096];
    buffer[0] = '\0';

    if (fgets(buffer, sizeof(buffer), stdin))
    {
        size_t len = strlen(buffer);
        /* إزالة حرف السطر الجديد */
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }
        return buffer;
    }
    return "";
}

/* قراءة عدد صحيح من المستخدم / Read integer from user */
long long sad_llvm_input_int(void)
{
    long long val = 0;

    /* التحقق من نجاح scanf */
    if (scanf("%lld", &val) != 1)
    {
        /* فشل القراءة — تنظيف المخزن المؤقت */
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return 0;
    }

    /* تنظيف المخزن المؤقت بعد القراءة الناجحة */
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;

    return val;
}

/* قراءة عدد عشري من المستخدم / Read float from user */
double sad_llvm_input_float(void)
{
    double val = 0.0;

    /* التحقق من نجاح scanf */
    if (scanf("%lf", &val) != 1)
    {
        /* فشل القراءة — تنظيف المخزن المؤقت */
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
        return 0.0;
    }

    /* تنظيف المخزن المؤقت بعد القراءة الناجحة */
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;

    return val;
}

/* ============================================================================
 * دوال الذاكرة / Memory Functions
 * ============================================================================ */

void *sad_llvm_alloc(unsigned long long size)
{
    /* التحقق من الحجم المعقول (حد أقصى 1 جيجابايت) */
    const unsigned long long MAX_ALLOC = 1ULL << 30;
    if (size == 0 || size > MAX_ALLOC)
    {
        fprintf(stderr, "[sad] تحذير: طلب تخصيص غير صالح: %llu بايت\n", size);
        return NULL;
    }

    void *ptr = malloc((size_t)size);
    if (!ptr)
    {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص %llu بايت\n", size);
    }
    return ptr;
}

void sad_llvm_free(void *ptr)
{
    if (ptr)
        free(ptr);
}

/* ============================================================================
 * دوال النصوص / String Functions
 * ============================================================================ */

void *sad_llvm_string_new(const char *data, unsigned long long length)
{
    /* التحقق من المدخلات قبل التخصيص */
    if (!data || length > (1ULL << 30))
    {
        return NULL;
    }
    char *str = (char *)malloc((size_t)(length + 1));
    if (!str)
    {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص %llu بايت\n", length + 1);
        return NULL;
    }
    memcpy(str, data, (size_t)length);
    str[length] = '\0';
    return str;
}

void *sad_llvm_string_from_cstr(const char *cstr)
{
    if (!cstr)
        return NULL;
    size_t len = strlen(cstr);
    if (len > (1ULL << 30))
    {
        fprintf(stderr, "[sad] خطأ: نص طويل جداً\n");
        return NULL;
    }
    char *str = (char *)malloc(len + 1);
    if (!str)
    {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص %zu بايت\n", len + 1);
        return NULL;
    }
    memcpy(str, cstr, len + 1);
    return str;
}

unsigned long long sad_llvm_string_length(void *str)
{
    if (!str)
        return 0;
    return (unsigned long long)strlen((const char *)str);
}

/* (AR) الحصول على حرف UTF-8 بالفهرس — يُرجع نصاً من حرف واحد */
/* (EN) Get UTF-8 character at index — returns a new string of one character */
void *sad_llvm_string_utf8_char_at(void *str, long long index)
{
    if (!str)
        return sad_llvm_string_new("", 0);
    const char *s = (const char *)str;
    unsigned long long slen = (unsigned long long)strlen(s);

    /* (AR) حساب عدد أحرف UTF-8 */
    long long charCount = 0;
    unsigned long long i = 0;
    while (i < slen)
    {
        unsigned char c = (unsigned char)s[i];
        unsigned long long charLen = 1;
        if (c >= 0xF0)
            charLen = 4;
        else if (c >= 0xE0)
            charLen = 3;
        else if (c >= 0xC0)
            charLen = 2;
        if (i + charLen > slen)
            charLen = 1;

        if (charCount == index)
            return sad_llvm_string_new(s + i, charLen);
        i += charLen;
        charCount++;
    }

    /* (AR) دعم الفهرس السلبي */
    if (index < 0)
    {
        long long actualIdx = charCount + index;
        if (actualIdx < 0)
            return sad_llvm_string_new("", 0);
        i = 0;
        charCount = 0;
        while (i < slen)
        {
            unsigned char c = (unsigned char)s[i];
            unsigned long long charLen = 1;
            if (c >= 0xF0)
                charLen = 4;
            else if (c >= 0xE0)
                charLen = 3;
            else if (c >= 0xC0)
                charLen = 2;
            if (i + charLen > slen)
                charLen = 1;

            if (charCount == actualIdx)
                return sad_llvm_string_new(s + i, charLen);
            i += charLen;
            charCount++;
        }
    }

    return sad_llvm_string_new("", 0);
}

/* ============================================================================
 * تنسيق الأعداد العشرية / Double Formatting
 * ============================================================================
 * (AR) تنسيق عدد عشري بنفس دقة المفسر: 6 خانات عشرية + حذف الأصفار الزائدة
 *      هذا يضمن تطابق مخرجات المترجم والمفسر تماماً
 *      مثال: 3.14159265 → "3.141593"، 2.5 → "2.5"، 100.0 → "100"
 * (EN) Format double with same precision as interpreter: 6 decimal places + strip trailing zeros
 *      This ensures compiler and interpreter output match exactly
 *      Example: 3.14159265 → "3.141593", 2.5 → "2.5", 100.0 → "100"
 * ============================================================================ */

/* (AR) تنسيق عدد عشري إلى مخزن مؤقت — 6 خانات عشرية + حذف أصفار زائدة */
/* (EN) Format double to buffer — 6 decimal places + strip trailing zeros */
void __sad_format_double(char *buf, double value)
{
    sprintf(buf, "%.6f", value);
    /* (AR) حذف الأصفار الزائدة بعد النقطة العشرية */
    /* (EN) Strip trailing zeros after decimal point */
    char *dot = strchr(buf, '.');
    if (dot)
    {
        /* (AR) احذف الأصفار الزائدة لكن ابق رقماً واحداً بعد النقطة: 10.0 لا 10 */
        /* (EN) Remove trailing zeros but keep at least one digit after dot: 10.0 not 10 */
        char *end = buf + strlen(buf) - 1;
        while (end > dot + 1 && *end == '0')
            end--;
        *(end + 1) = '\0';
    }
}

/* (AR) طباعة عدد عشري مباشرة — يُستدعى من الكود المُولّد بدلاً من printf("%g") */
/* (EN) Print double directly — called from generated code instead of printf("%g") */
void __sad_print_double(double value)
{
    char buf[64];
    __sad_format_double(buf, value);
    fputs(buf, stdout);
}

void sad_llvm_print_string(void *str)
{
    if (str)
        printf("%s", (const char *)str);
    else
        printf("null");
}

/* ============================================================================
 * دوال الأمان / Security Functions
 * ============================================================================ */

/* تأكد / Assert — abort on false */
void sad_security_assert(int condition, const char *msg)
{
    if (!condition)
    {
        fprintf(stderr, "[ASSERT FAILED] %s\n", msg ? msg : "assertion failed");
        abort();
    }
}

/* ذعر / Panic — emergency halt */
void sad_security_panic(const char *msg)
{
    fprintf(stderr, "[PANIC] %s\n", msg ? msg : "panic");
    abort();
}

/* هاش / Hash — FNV-1a */
long long sad_security_hash(const char *str)
{
    if (!str)
        return 0;
    unsigned long long hash = 14695981039346656037ULL;
    while (*str)
    {
        hash ^= (unsigned char)*str++;
        hash *= 1099511628211ULL;
    }
    return (long long)hash;
}

/* وقت_الآن / Current timestamp */
long long sad_security_timestamp(void)
{
    return (long long)time(NULL);
}

/* شفّر / Encrypt — XOR cipher (للاستخدام البسيط فقط، ليس للأمان الحقيقي) */
const char *sad_security_encrypt(const char *text, const char *key)
{
    if (!text || !key || !*key)
        return text;

    size_t tlen = strlen(text);
    size_t klen = strlen(key);

    /* التحقق من الحد الأقصى للحجم لمنع overflow */
    if (tlen > (SIZE_MAX / 4 - 1))
    {
        fprintf(stderr, "[sad] خطأ: النص طويل جداً للتشفير\n");
        return text;
    }

    char *result = (char *)malloc(tlen * 2 + 1);
    if (!result)
    {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة للتشفير\n");
        return text;
    }

    size_t pos = 0;
    for (size_t i = 0; i < tlen; i++)
    {
        unsigned char c = (unsigned char)text[i] ^ (unsigned char)key[i % klen];
        int written = snprintf(result + pos, 3, "%02x", c);
        if (written != 2)
        {
            free(result);
            return text;
        }
        pos += 2;
    }
    result[pos] = '\0';

    return result;
}

/* فك_تشفير / Decrypt — XOR decipher */
const char *sad_security_decrypt(const char *hex, const char *key)
{
    if (!hex || !key || !*key)
        return hex;

    size_t hlen = strlen(hex);
    if (hlen % 2 != 0)
    {
        fprintf(stderr, "[sad] خطأ: طول النص المشفر غير صالح\n");
        return hex;
    }

    size_t rlen = hlen / 2;
    size_t klen = strlen(key);

    char *result = (char *)malloc(rlen + 1);
    if (!result)
    {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة لفك التشفير\n");
        return hex;
    }

    for (size_t i = 0; i < rlen; i++)
    {
        unsigned int byte = 0;
        if (sscanf(hex + i * 2, "%02x", &byte) != 1)
        {
            free(result);
            fprintf(stderr, "[sad] خطأ: تنسيق hex غير صالح عند الموقع %zu\n", i);
            return hex;
        }
        if (byte > 255)
        {
            free(result);
            return hex;
        }
        result[i] = (char)((unsigned char)byte ^ (unsigned char)key[i % klen]);
    }
    result[rlen] = '\0';

    return result;
}

/* تأكد_نوع / Assert type */
void sad_security_assert_type(const char *expected, const char *actual)
{
    if (!expected || !actual || strcmp(expected, actual) != 0)
    {
        fprintf(stderr, "[TYPE ASSERT FAILED] expected=%s actual=%s\n",
                expected ? expected : "null", actual ? actual : "null");
        abort();
    }
}

/* تأكد_مساواة / Assert equal (strings) */
void sad_security_assert_equal_str(const char *a, const char *b)
{
    if (!a || !b || strcmp(a, b) != 0)
    {
        fprintf(stderr, "[EQUAL ASSERT FAILED] '%s' != '%s'\n",
                a ? a : "null", b ? b : "null");
        abort();
    }
}

/* نظّف / Sanitize — HTML entity encoding */
const char *sad_security_sanitize(const char *text)
{
    if (!text)
        return "";
    size_t len = strlen(text);
    char *result = (char *)malloc(len * 6 + 1);
    if (!result)
        return text;
    size_t pos = 0;
    for (size_t i = 0; i < len; i++)
    {
        switch (text[i])
        {
        case '<':
            memcpy(result + pos, "&lt;", 4);
            pos += 4;
            break;
        case '>':
            memcpy(result + pos, "&gt;", 4);
            pos += 4;
            break;
        case '&':
            memcpy(result + pos, "&amp;", 5);
            pos += 5;
            break;
        case '"':
            memcpy(result + pos, "&quot;", 6);
            pos += 6;
            break;
        case '\'':
            memcpy(result + pos, "&#39;", 5);
            pos += 5;
            break;
        default:
            result[pos++] = text[i];
            break;
        }
    }
    result[pos] = '\0';
    return result;
}

/* عشوائي_آمن / Secure random in range */
long long sad_security_secure_random(long long min_val, long long max_val)
{
    static int seeded = 0;
    if (!seeded)
    {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }
    if (min_val >= max_val)
        return min_val;
    long long range = max_val - min_val + 1;
    return min_val + (long long)(rand() % (int)range);
}

/* ترميز_64 / Base64 encode */
const char *sad_security_base64_encode(const char *text)
{
    static const char b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    if (!text)
        return "";
    size_t len = strlen(text);
    size_t olen = 4 * ((len + 2) / 3);
    char *result = (char *)malloc(olen + 1);
    if (!result)
        return "";
    size_t j = 0;
    for (size_t i = 0; i < len; i += 3)
    {
        unsigned int a = (unsigned char)text[i];
        unsigned int b2 = (i + 1 < len) ? (unsigned char)text[i + 1] : 0;
        unsigned int c = (i + 2 < len) ? (unsigned char)text[i + 2] : 0;
        unsigned int triple = (a << 16) | (b2 << 8) | c;
        result[j++] = b64[(triple >> 18) & 0x3F];
        result[j++] = b64[(triple >> 12) & 0x3F];
        result[j++] = (i + 1 < len) ? b64[(triple >> 6) & 0x3F] : '=';
        result[j++] = (i + 2 < len) ? b64[triple & 0x3F] : '=';
    }
    result[j] = '\0';
    return result;
}

/* ============================================================================
 * دوال الخرائط / Map Functions — FNV-1a Hash Table with Open Addressing
 * ============================================================================
 * خريطة تجزئة (hash map) عالية الأداء بمفاتيح نصية وقيم مُوسّمة بالنوع.
 * تستخدم خوارزمية FNV-1a للتجزئة مع فحص خطي (linear probing).
 * معامل الحمل (load factor) أقصى 75% — إعادة تجزئة تلقائية عند التجاوز.
 * ============================================================================ */

/* حالة الخانة / Slot state */
#define SLOT_EMPTY 0
#define SLOT_OCCUPIED 1
#define SLOT_DELETED 2

/* أنواع قيم الخريطة / Map value types */
#define SVAL_STRING 0
#define SVAL_INT 1
#define SVAL_FLOAT 2
#define SVAL_BOOL 3
#define SVAL_NULL 4

/* هيكل إدخال الخريطة / Map entry structure */
typedef struct
{
    char *key;
    char *value;              /* تمثيل نصي للتوافق / String representation for compat */
    long long value_i64;      /* قيمة عددية أو مؤشر / Numeric value or pointer */
    unsigned char value_type; /* SVAL_* نوع القيمة / Value type tag */
    unsigned char state;      /* SLOT_EMPTY, SLOT_OCCUPIED, SLOT_DELETED */
} SadMapSlot;

/* هيكل الخريطة / Map structure */
typedef struct
{
    SadMapSlot *slots;
    long long size;     /* عدد العناصر الفعلية / Active element count */
    long long capacity; /* عدد الخانات الكلي / Total slot count (always power of 2) */
} SadMap;

/* تجزئة FNV-1a — سريعة وتوزيع جيد / FNV-1a hash — fast with good distribution */
static unsigned long long sad_fnv1a(const char *key)
{
    unsigned long long h = 14695981039346656037ULL;
    while (*key)
    {
        h ^= (unsigned char)*key++;
        h *= 1099511628211ULL;
    }
    return h;
}

/* نسخ نص / Duplicate string */
static char *sad_strdup(const char *s)
{
    if (!s)
        return NULL;
    size_t len = strlen(s);
    char *d = (char *)malloc(len + 1);
    if (d)
        memcpy(d, s, len + 1);
    return d;
}

/* أقرب قوة من 2 أكبر أو تساوي n / Next power of 2 >= n */
static long long sad_next_pow2(long long n)
{
    long long p = 8;
    while (p < n)
        p *= 2;
    return p;
}

/* إعادة تجزئة / Rehash all entries into bigger table */
static void sad_map_rehash(SadMap *map)
{
    long long old_cap = map->capacity;
    SadMapSlot *old_slots = map->slots;
    long long new_cap = old_cap * 2;
    SadMapSlot *new_slots = (SadMapSlot *)calloc((size_t)new_cap, sizeof(SadMapSlot));
    if (!new_slots)
        return;
    long long mask = new_cap - 1;
    for (long long i = 0; i < old_cap; i++)
    {
        if (old_slots[i].state == SLOT_OCCUPIED)
        {
            unsigned long long h = sad_fnv1a(old_slots[i].key);
            long long idx = (long long)(h & (unsigned long long)mask);
            while (new_slots[idx].state == SLOT_OCCUPIED)
                idx = (idx + 1) & mask;
            new_slots[idx] = old_slots[i];
        }
    }
    free(old_slots);
    map->slots = new_slots;
    map->capacity = new_cap;
}

/* إنشاء خريطة جديدة / Create new map */
long long __sad_map_create(long long hint)
{
    SadMap *map = (SadMap *)malloc(sizeof(SadMap));
    if (!map)
        return 0;
    long long cap = sad_next_pow2(hint < 4 ? 8 : hint * 2);
    map->slots = (SadMapSlot *)calloc((size_t)cap, sizeof(SadMapSlot));
    if (!map->slots)
    {
        free(map);
        return 0;
    }
    map->size = 0;
    map->capacity = cap;
    return (long long)(uintptr_t)map;
}

/* تعيين قيمة نصية في الخريطة / Set string value in map — O(1) amortised */
void __sad_map_set(long long map_i64, const char *key, const char *value)
{
    SadMap *map = (SadMap *)(uintptr_t)map_i64;
    if (!map || !key)
        return;
    /* إعادة تجزئة إذا تجاوز معامل الحمل 75% */
    if (map->size * 4 >= map->capacity * 3)
        sad_map_rehash(map);
    long long mask = map->capacity - 1;
    unsigned long long h = sad_fnv1a(key);
    long long idx = (long long)(h & (unsigned long long)mask);
    long long first_deleted = -1;
    while (1)
    {
        if (map->slots[idx].state == SLOT_EMPTY)
        {
            long long ins = (first_deleted >= 0) ? first_deleted : idx;
            free(map->slots[ins].key);
            free(map->slots[ins].value);
            map->slots[ins].key = sad_strdup(key);
            map->slots[ins].value = sad_strdup(value);
            map->slots[ins].value_type = SVAL_STRING;
            map->slots[ins].value_i64 = (long long)(uintptr_t)map->slots[ins].value;
            map->slots[ins].state = SLOT_OCCUPIED;
            map->size++;
            return;
        }
        if (map->slots[idx].state == SLOT_DELETED)
        {
            if (first_deleted < 0)
                first_deleted = idx;
        }
        else if (strcmp(map->slots[idx].key, key) == 0)
        {
            free(map->slots[idx].value);
            map->slots[idx].value = sad_strdup(value);
            map->slots[idx].value_type = SVAL_STRING;
            map->slots[idx].value_i64 = (long long)(uintptr_t)map->slots[idx].value;
            return;
        }
        idx = (idx + 1) & mask;
    }
}

/* تعيين قيمة مُوسّمة / Set typed value — type: 0=string, 1=int, 2=float, 3=bool, 4=null */
void __sad_map_set_typed(long long map_i64, const char *key, long long val, int type)
{
    SadMap *map = (SadMap *)(uintptr_t)map_i64;
    if (!map || !key)
        return;
    if (map->size * 4 >= map->capacity * 3)
        sad_map_rehash(map);
    long long mask = map->capacity - 1;
    unsigned long long h = sad_fnv1a(key);
    long long idx = (long long)(h & (unsigned long long)mask);
    long long first_deleted = -1;
    while (1)
    {
        if (map->slots[idx].state == SLOT_EMPTY)
        {
            long long ins = (first_deleted >= 0) ? first_deleted : idx;
            free(map->slots[ins].key);
            free(map->slots[ins].value);
            map->slots[ins].key = sad_strdup(key);
            map->slots[ins].value_i64 = val;
            map->slots[ins].value_type = (unsigned char)type;
            /* إنشاء تمثيل نصي / Create string representation */
            if (type == SVAL_STRING)
            {
                map->slots[ins].value = sad_strdup((const char *)(uintptr_t)val);
            }
            else if (type == SVAL_INT)
            {
                char buf[32];
                sprintf(buf, "%lld", val);
                map->slots[ins].value = sad_strdup(buf);
            }
            else if (type == SVAL_FLOAT)
            {
                double dv;
                memcpy(&dv, &val, sizeof(double));
                char buf[64];
                __sad_format_double(buf, dv);
                map->slots[ins].value = sad_strdup(buf);
            }
            else if (type == SVAL_BOOL)
            {
                map->slots[ins].value = sad_strdup(val ? "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad" : "\xd8\xae\xd8\xb7\xd8\xa3");
            }
            else
            {
                map->slots[ins].value = sad_strdup("\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1");
            }
            map->slots[ins].state = SLOT_OCCUPIED;
            map->size++;
            return;
        }
        if (map->slots[idx].state == SLOT_DELETED)
        {
            if (first_deleted < 0)
                first_deleted = idx;
        }
        else if (strcmp(map->slots[idx].key, key) == 0)
        {
            free(map->slots[idx].value);
            map->slots[idx].value_i64 = val;
            map->slots[idx].value_type = (unsigned char)type;
            if (type == SVAL_STRING)
            {
                map->slots[idx].value = sad_strdup((const char *)(uintptr_t)val);
            }
            else if (type == SVAL_INT)
            {
                char buf[32];
                sprintf(buf, "%lld", val);
                map->slots[idx].value = sad_strdup(buf);
            }
            else if (type == SVAL_FLOAT)
            {
                double dv;
                memcpy(&dv, &val, sizeof(double));
                char buf[64];
                __sad_format_double(buf, dv);
                map->slots[idx].value = sad_strdup(buf);
            }
            else if (type == SVAL_BOOL)
            {
                map->slots[idx].value = sad_strdup(val ? "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad" : "\xd8\xae\xd8\xb7\xd8\xa3");
            }
            else
            {
                map->slots[idx].value = sad_strdup("\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1");
            }
            return;
        }
        idx = (idx + 1) & mask;
    }
}

/* الحصول على القيمة العددية / Get raw i64 value */
long long __sad_map_get_i64(long long map_i64, const char *key)
{
    SadMap *map = (SadMap *)(uintptr_t)map_i64;
    if (!map || !key)
        return 0;
    long long mask = map->capacity - 1;
    unsigned long long h = sad_fnv1a(key);
    long long idx = (long long)(h & (unsigned long long)mask);
    while (1)
    {
        if (map->slots[idx].state == SLOT_EMPTY)
            return 0;
        if (map->slots[idx].state == SLOT_OCCUPIED && strcmp(map->slots[idx].key, key) == 0)
            return map->slots[idx].value_i64;
        idx = (idx + 1) & mask;
    }
}

/* الحصول على نوع القيمة / Get value type: 0=string, 1=int, 2=float, 3=bool, 4=null */
int __sad_map_get_type(long long map_i64, const char *key)
{
    SadMap *map = (SadMap *)(uintptr_t)map_i64;
    if (!map || !key)
        return SVAL_NULL;
    long long mask = map->capacity - 1;
    unsigned long long h = sad_fnv1a(key);
    long long idx = (long long)(h & (unsigned long long)mask);
    while (1)
    {
        if (map->slots[idx].state == SLOT_EMPTY)
            return SVAL_NULL;
        if (map->slots[idx].state == SLOT_OCCUPIED && strcmp(map->slots[idx].key, key) == 0)
            return (int)map->slots[idx].value_type;
        idx = (idx + 1) & mask;
    }
}

/* الحصول على قيمة نصية من الخريطة / Get string value from map — O(1) amortised */
const char *__sad_map_get(long long map_i64, const char *key)
{
    SadMap *map = (SadMap *)(uintptr_t)map_i64;
    if (!map || !key)
        return "";
    long long mask = map->capacity - 1;
    unsigned long long h = sad_fnv1a(key);
    long long idx = (long long)(h & (unsigned long long)mask);
    while (1)
    {
        if (map->slots[idx].state == SLOT_EMPTY)
            return "";
        if (map->slots[idx].state == SLOT_OCCUPIED && strcmp(map->slots[idx].key, key) == 0)
            return map->slots[idx].value;
        idx = (idx + 1) & mask;
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 * (AR) دالة مدى — إنشاء مصفوفة أرقام من start إلى end-1 بخطوة step
 *      تُرجع مؤشر SadArray {i64 length, i64 capacity, ptr data}
 * (EN) Range function — create integer array from start to end-1 with step
 *      Returns pointer to SadArray {i64 length, i64 capacity, ptr data}
 * ═══════════════════════════════════════════════════════════════════════════ */

/* (AR) بنية المصفوفة — مطابقة لتعريف SadArray في LLVM codegen */
/* (EN) Array struct — matching SadArray definition in LLVM codegen */
typedef struct
{
    long long length;
    long long capacity;
    void *data;
} SadRuntimeArray;

/* (AR) مدى(start, end, step) — ثلاث معاملات */
/* (EN) range(start, end, step) — three arguments */
SadRuntimeArray *__sad_range(long long start, long long end, long long step)
{
    /* (AR) حساب عدد العناصر */
    /* (EN) Calculate element count */
    long long count = 0;
    if (step > 0 && start < end)
        count = (end - start + step - 1) / step;
    else if (step < 0 && start > end)
        count = (start - end - step - 1) / (-step);

    if (count <= 0)
        count = 0;

    long long capacity = count > 0 ? count : 1;

    /* (AR) تخصيص بنية المصفوفة + مخزن البيانات */
    /* (EN) Allocate array struct + data buffer */
    SadRuntimeArray *arr = (SadRuntimeArray *)malloc(sizeof(SadRuntimeArray));
    long long *data = (long long *)malloc(capacity * sizeof(long long));

    /* (AR) ملء البيانات */
    /* (EN) Fill data */
    long long idx = 0;
    if (step > 0)
    {
        for (long long i = start; i < end && idx < count; i += step)
            data[idx++] = i;
    }
    else if (step < 0)
    {
        for (long long i = start; i > end && idx < count; i += step)
            data[idx++] = i;
    }

    arr->length = idx;
    arr->capacity = capacity;
    arr->data = (void *)data;
    return arr;
}
