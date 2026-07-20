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
/* (AR) لا <ctype.h>: تحويل الحالة يستعمل طيّ ASCII صريحًا [A-Z]↔[a-z] مستقلًّا عن
 *      اللغة المحليّة (locale) — لا toupper/tolower (المعتمدَين على LC_CTYPE). */

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
 * تحويل حالة الأحرف (ASCII) / ASCII case conversion
 * ============================================================================ */

/* (AR) تحويل الأحرف اللاتينيّة إلى كبيرة في المكان: طيّ ASCII صريح [a-z]→[A-Z]
 *      فقط، مستقلّ عن اللغة المحليّة (لا toupper/LC_CTYPE). البايتات العالية
 *      (UTF-8 العربيّ/اللاتينيّ المُشكَّل) تبقى دون تغيير. مطابق بايتيًّا لمسار
 *      المفسّر StringFunctions::toUpper. يستدعيه الكود المُولَّد لـ«تحويل_كبير»
 *      بدلاً من _strupr — رمز MSVC غير قياسيّ لا يُربَط على Linux/macOS.
 * (EN) Uppercase Latin letters in place: explicit ASCII-only fold [a-z]→[A-Z],
 *      locale-independent (no toupper/LC_CTYPE). High bytes (Arabic / accented
 *      Latin UTF-8) are left unchanged. Byte-identical to the interpreter
 *      StringFunctions::toUpper. Called by codegen for «تحويل_كبير» instead of
 *      _strupr — a non-standard MSVC symbol absent on Linux/macOS. */
char *sad_llvm_str_upper(char *s)
{
    if (s)
    {
        unsigned char *p;
        for (p = (unsigned char *)s; *p; ++p)
            if (*p >= 'a' && *p <= 'z')
                *p = (unsigned char)(*p - ('a' - 'A'));
    }
    return s;
}

/* (AR) نظيرة التحويل إلى صغيرة: طيّ ASCII صريح [A-Z]→[a-z] مستقلّ عن اللغة
 *      المحليّة (بديل _strlwr المحمول، مطابق لـ StringFunctions::toLower).
 * (EN) Lowercase counterpart: explicit ASCII-only fold [A-Z]→[a-z],
 *      locale-independent (portable replacement for _strlwr, matches
 *      StringFunctions::toLower). */
char *sad_llvm_str_lower(char *s)
{
    if (s)
    {
        unsigned char *p;
        for (p = (unsigned char *)s; *p; ++p)
            if (*p >= 'A' && *p <= 'Z')
                *p = (unsigned char)(*p + ('a' - 'A'));
    }
    return s;
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
    /* (AR) ISSUE-076 (Amelia #8): %.6f قد يبلغ ~316 حرفًا لـDBL_MAX (غير أُسّيّ) —
     *      64 يفيض. 512 آمن (>DBL_MAX_10_EXP+هامش). */
    /* (EN) ISSUE-076 (Amelia #8): %.6f can reach ~316 chars for DBL_MAX (never
     *      exponential) — 64 overflows. 512 is safe (>DBL_MAX_10_EXP + margin). */
    char buf[512];
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

/* ============================================================================
 * SHA-256 (FIPS 180-4) — نواة مشتركة لِـ«هاش»/«شفّر»/«فك_تشفير»
 * ============================================================================
 * (AR) توحيد مع تنفيذ المفسّر (interpreter/src/builtins/builtin_module_assertions.cpp)
 *      لإغلاق التباعد السلوكيّ الموثَّق في RFC 0010 — نفس الثوابت وترتيب البتّات
 *      حرفيًّا، بلا أيّ تبعيّة خارجيّة (C خالص، يعمل على المعدن العاري كذلك).
 * (EN) Unified with the interpreter's implementation to close the documented
 *      RFC 0010 behavioral divergence — bit-identical constants/ordering, pure
 *      C with zero external dependencies.
 * ============================================================================ */
static unsigned int sad_sha256_rotr(unsigned int x, unsigned int n)
{
    return (x >> n) | (x << (32 - n));
}

/* (AR) يحسب SHA-256 لمخزن بايتات خام ويكتب 32 بايت في out */
static void sad_sha256_raw(const unsigned char *data, size_t len, unsigned char out[32])
{
    static const unsigned int k[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
    unsigned int h0 = 0x6a09e667, h1 = 0xbb67ae85, h2 = 0x3c6ef372, h3 = 0xa54ff53a;
    unsigned int h4 = 0x510e527f, h5 = 0x9b05688c, h6 = 0x1f83d9ab, h7 = 0x5be0cd19;
    unsigned long long bitLen = (unsigned long long)len * 8;
    size_t padded_len = ((len + 8) / 64 + 1) * 64;
    unsigned char *padded = (unsigned char *)malloc(padded_len);
    size_t off;
    int i;
    if (!padded)
    {
        memset(out, 0, 32);
        return;
    }
    memset(padded, 0, padded_len);
    memcpy(padded, data, len);
    padded[len] = 0x80;
    for (i = 7; i >= 0; --i)
        padded[padded_len - 1 - i] = (unsigned char)(bitLen >> (i * 8));

    for (off = 0; off < padded_len; off += 64)
    {
        unsigned int w[64];
        unsigned int a, b, c, d, e, f, g, hh;
        for (i = 0; i < 16; ++i)
            w[i] = ((unsigned int)padded[off + i * 4] << 24) | ((unsigned int)padded[off + i * 4 + 1] << 16) |
                   ((unsigned int)padded[off + i * 4 + 2] << 8) | (unsigned int)padded[off + i * 4 + 3];
        for (i = 16; i < 64; ++i)
        {
            unsigned int s0 = sad_sha256_rotr(w[i - 15], 7) ^ sad_sha256_rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            unsigned int s1 = sad_sha256_rotr(w[i - 2], 17) ^ sad_sha256_rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        a = h0; b = h1; c = h2; d = h3; e = h4; f = h5; g = h6; hh = h7;
        for (i = 0; i < 64; ++i)
        {
            unsigned int S1 = sad_sha256_rotr(e, 6) ^ sad_sha256_rotr(e, 11) ^ sad_sha256_rotr(e, 25);
            unsigned int ch = (e & f) ^ (~e & g);
            unsigned int t1 = hh + S1 + ch + k[i] + w[i];
            unsigned int S0 = sad_sha256_rotr(a, 2) ^ sad_sha256_rotr(a, 13) ^ sad_sha256_rotr(a, 22);
            unsigned int maj = (a & b) ^ (a & c) ^ (b & c);
            unsigned int t2 = S0 + maj;
            hh = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
        }
        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e; h5 += f; h6 += g; h7 += hh;
    }
    free(padded);
    {
        unsigned int hs[8] = {h0, h1, h2, h3, h4, h5, h6, h7};
        for (i = 0; i < 8; ++i)
        {
            out[i * 4] = (unsigned char)(hs[i] >> 24);
            out[i * 4 + 1] = (unsigned char)(hs[i] >> 16);
            out[i * 4 + 2] = (unsigned char)(hs[i] >> 8);
            out[i * 4 + 3] = (unsigned char)hs[i];
        }
    }
}

/* (AR) مصدر عشوائيّة النظام (CSPRNG حقيقيّ) — Windows: BCryptGenRandom (تحميل
 *      ديناميكيّ عبر LoadLibraryA/GetProcAddress، فلا حاجة لربط bcrypt.lib في
 *      أيّ من مسارات الربط الثلاثة). POSIX: /dev/urandom. يُرجع 1 عند النجاح.
 *      غير مُستعمَل تحت SAD_FREESTANDING (انظر SAD_FREESTANDING أدناه) — الهدف
 *      الحرّ قد لا يوجد تحته نظام تشغيل مضيف أصلًا (كنواة sad-os)، فيبقى على
 *      مسار libc القديم عمدًا (فجوة معروفة، موثَّقة في RFC توسيع مكتبة
 *      التشفير — "أسئلة غير محسومة": مصدر عشوائيّة للوضع الحرّ البحت).
 * (EN) Real OS-backed CSPRNG source. Not used under SAD_FREESTANDING — that
 *      target may have no host OS at all, so it intentionally keeps the old
 *      libc-only path (tracked gap, see the crypto-library-expansion RFC). */
#ifndef SAD_FREESTANDING
#ifdef _WIN32
static int sad_crypto_os_random(unsigned char *buf, size_t len)
{
    typedef long (WINAPI *BCryptGenRandomFn)(void *, unsigned char *, unsigned long, unsigned long);
    static BCryptGenRandomFn fn = NULL;
    static int tried = 0;
    if (!tried)
    {
        HMODULE h;
        tried = 1;
        h = LoadLibraryA("bcrypt.dll");
        if (h)
            fn = (BCryptGenRandomFn)(void *)GetProcAddress(h, "BCryptGenRandom");
    }
    /* BCRYPT_USE_SYSTEM_PREFERRED_RNG = 0x00000002 */
    return (fn && fn(NULL, buf, (unsigned long)len, 0x00000002) == 0) ? 1 : 0;
}
#else
static int sad_crypto_os_random(unsigned char *buf, size_t len)
{
    FILE *f = fopen("/dev/urandom", "rb");
    size_t got;
    if (!f)
        return 0;
    got = fread(buf, 1, len, f);
    fclose(f);
    return got == len;
}
#endif
#endif /* !SAD_FREESTANDING */

/* (AR) مولّد nonce 64 بت لتشفير-التيار — CSPRNG حقيقيّ على الأهداف المستضافة؛
 *      يتراجع لمسار srand/rand القديم (غير آمن تشفيريًّا) فقط تحت
 *      SAD_FREESTANDING أو إن تعذّر الوصول لمصدر عشوائيّة النظام. */
static unsigned long long sad_crypto_random_u64(void)
{
#ifndef SAD_FREESTANDING
    {
        unsigned char bytes[8];
        if (sad_crypto_os_random(bytes, sizeof(bytes)))
        {
            unsigned long long r = 0;
            int i;
            for (i = 0; i < 8; ++i)
                r = (r << 8) | bytes[i];
            return r;
        }
    }
#endif
    {
        static int seeded = 0;
        unsigned long long r = 0;
        int i;
        if (!seeded)
        {
            srand((unsigned int)time(NULL));
            seeded = 1;
        }
        for (i = 0; i < 4; ++i)
            r = (r << 16) | ((unsigned long long)rand() & 0xFFFFu);
        return r;
    }
}

/* هاش / Hash — SHA-256 (يطابق FIPS 180-4 ومفسّر ص حرفيًّا) */
const char *sad_security_hash(const char *str)
{
    unsigned char digest[32];
    char *out;
    size_t i;
    if (!str)
        str = "";
    sad_sha256_raw((const unsigned char *)str, strlen(str), digest);
    out = (char *)malloc(65);
    if (!out)
        return "";
    for (i = 0; i < 32; ++i)
        snprintf(out + i * 2, 3, "%02x", digest[i]);
    out[64] = '\0';
    return out;
}

/* ============================================================================
 * BLAKE3 (وحدة تشفير: بلايك3/هاش_مفتاح) — تنفيذ مرجعيّ محمول (بلا SIMD/تعدّد
 * خيوط)، مطابق حرفيًّا لنظير المفسّر
 * (interpreter/src/builtins/builtin_module_crypto.cpp). كلاهما تحقّق بنجاح
 * مقابل شعاعات BLAKE3 الرسميّة (test_vectors.json من مستودع BLAKE3-team)
 * قبل الدمج.
 * ============================================================================ */
static const unsigned int sad_blake3_iv[8] = {
    0x6A09E667u, 0xBB67AE85u, 0x3C6EF372u, 0xA54FF53Au,
    0x510E527Fu, 0x9B05688Cu, 0x1F83D9ABu, 0x5BE0CD19u};
static const int sad_blake3_msg_perm[16] = {2, 6, 3, 10, 7, 0, 4, 13, 1, 11, 12, 5, 9, 14, 15, 8};

#define SAD_BLAKE3_CHUNK_START 1u
#define SAD_BLAKE3_CHUNK_END   2u
#define SAD_BLAKE3_PARENT      4u
#define SAD_BLAKE3_ROOT        8u
#define SAD_BLAKE3_KEYED_HASH  16u
#define SAD_BLAKE3_BLOCK_LEN 64
#define SAD_BLAKE3_CHUNK_LEN 1024

static unsigned int sad_blake3_rotr32(unsigned int x, int n) { return (x >> n) | (x << (32 - n)); }

static void sad_blake3_g(unsigned int *st, int a, int b, int c, int d, unsigned int mx, unsigned int my)
{
    st[a] = st[a] + st[b] + mx;
    st[d] = sad_blake3_rotr32(st[d] ^ st[a], 16);
    st[c] = st[c] + st[d];
    st[b] = sad_blake3_rotr32(st[b] ^ st[c], 12);
    st[a] = st[a] + st[b] + my;
    st[d] = sad_blake3_rotr32(st[d] ^ st[a], 8);
    st[c] = st[c] + st[d];
    st[b] = sad_blake3_rotr32(st[b] ^ st[c], 7);
}

static void sad_blake3_round(unsigned int *st, const unsigned int *m)
{
    sad_blake3_g(st, 0, 4, 8, 12, m[0], m[1]);
    sad_blake3_g(st, 1, 5, 9, 13, m[2], m[3]);
    sad_blake3_g(st, 2, 6, 10, 14, m[4], m[5]);
    sad_blake3_g(st, 3, 7, 11, 15, m[6], m[7]);
    sad_blake3_g(st, 0, 5, 10, 15, m[8], m[9]);
    sad_blake3_g(st, 1, 6, 11, 12, m[10], m[11]);
    sad_blake3_g(st, 2, 7, 8, 13, m[12], m[13]);
    sad_blake3_g(st, 3, 4, 9, 14, m[14], m[15]);
}

static void sad_blake3_permute(unsigned int *m)
{
    unsigned int t[16];
    int i;
    for (i = 0; i < 16; ++i)
        t[i] = m[sad_blake3_msg_perm[i]];
    memcpy(m, t, sizeof(t));
}

/* out يجب أن يتّسع لـ16 unsigned int */
static void sad_blake3_compress(const unsigned int cv[8], const unsigned int block_words[16],
                                 unsigned long long counter, unsigned int block_len,
                                 unsigned int flags, unsigned int out[16])
{
    unsigned int st[16];
    unsigned int m[16];
    int r;
    memcpy(st, cv, 8 * sizeof(unsigned int));
    memcpy(st + 8, sad_blake3_iv, 4 * sizeof(unsigned int));
    st[12] = (unsigned int)(counter & 0xFFFFFFFFu);
    st[13] = (unsigned int)(counter >> 32);
    st[14] = block_len;
    st[15] = flags;
    memcpy(m, block_words, 16 * sizeof(unsigned int));
    for (r = 0; r < 7; ++r)
    {
        sad_blake3_round(st, m);
        if (r < 6)
            sad_blake3_permute(m);
    }
    for (r = 0; r < 8; ++r)
    {
        out[r] = st[r] ^ st[r + 8];
        out[r + 8] = st[r + 8] ^ cv[r];
    }
}

static void sad_blake3_words_from_bytes(const unsigned char *b, size_t len, unsigned int out[16])
{
    unsigned char buf[64];
    int i;
    memset(buf, 0, 64);
    if (len)
        memcpy(buf, b, len);
    for (i = 0; i < 16; ++i)
        out[i] = (unsigned int)buf[i * 4] | ((unsigned int)buf[i * 4 + 1] << 8) |
                 ((unsigned int)buf[i * 4 + 2] << 16) | ((unsigned int)buf[i * 4 + 3] << 24);
}

typedef struct
{
    unsigned int cv[8];
    unsigned long long chunk_counter;
    unsigned char block[SAD_BLAKE3_BLOCK_LEN];
    size_t block_len;
    int blocks_compressed;
    unsigned int flags;
} SadBlake3ChunkState;

static void sad_blake3_chunk_init(SadBlake3ChunkState *cs, const unsigned int key[8],
                                   unsigned long long counter, unsigned int flags)
{
    memcpy(cs->cv, key, 8 * sizeof(unsigned int));
    cs->chunk_counter = counter;
    cs->block_len = 0;
    cs->blocks_compressed = 0;
    cs->flags = flags;
}

static size_t sad_blake3_chunk_len(const SadBlake3ChunkState *cs)
{
    return (size_t)SAD_BLAKE3_BLOCK_LEN * cs->blocks_compressed + cs->block_len;
}

static unsigned int sad_blake3_chunk_start_flag(const SadBlake3ChunkState *cs)
{
    return cs->blocks_compressed == 0 ? SAD_BLAKE3_CHUNK_START : 0;
}

static void sad_blake3_chunk_update(SadBlake3ChunkState *cs, const unsigned char *data, size_t len)
{
    while (len > 0)
    {
        size_t take;
        if (cs->block_len == SAD_BLAKE3_BLOCK_LEN)
        {
            unsigned int block_words[16];
            unsigned int out[16];
            sad_blake3_words_from_bytes(cs->block, SAD_BLAKE3_BLOCK_LEN, block_words);
            sad_blake3_compress(cs->cv, block_words, cs->chunk_counter, SAD_BLAKE3_BLOCK_LEN,
                                 cs->flags | sad_blake3_chunk_start_flag(cs), out);
            memcpy(cs->cv, out, 8 * sizeof(unsigned int));
            cs->blocks_compressed++;
            cs->block_len = 0;
        }
        take = (size_t)SAD_BLAKE3_BLOCK_LEN - cs->block_len;
        if (take > len)
            take = len;
        memcpy(cs->block + cs->block_len, data, take);
        cs->block_len += take;
        data += take;
        len -= take;
    }
}

typedef struct
{
    unsigned int input_cv[8];
    unsigned int block_words[16];
    unsigned long long counter;
    unsigned int block_len;
    unsigned int flags;
} SadBlake3Output;

static void sad_blake3_chunk_output(const SadBlake3ChunkState *cs, SadBlake3Output *out)
{
    memcpy(out->input_cv, cs->cv, 8 * sizeof(unsigned int));
    sad_blake3_words_from_bytes(cs->block, cs->block_len, out->block_words);
    out->counter = cs->chunk_counter;
    out->block_len = (unsigned int)cs->block_len;
    out->flags = cs->flags | sad_blake3_chunk_start_flag(cs) | SAD_BLAKE3_CHUNK_END;
}

static void sad_blake3_output_cv(const SadBlake3Output *o, unsigned int cv[8])
{
    unsigned int out[16];
    sad_blake3_compress(o->input_cv, o->block_words, o->counter, o->block_len, o->flags, out);
    memcpy(cv, out, 8 * sizeof(unsigned int));
}

static void sad_blake3_output_root_bytes(const SadBlake3Output *o, unsigned char *out, size_t out_len)
{
    unsigned long long block_counter = 0;
    size_t written = 0;
    while (written < out_len)
    {
        unsigned int words[16];
        int i;
        sad_blake3_compress(o->input_cv, o->block_words, block_counter, o->block_len,
                             o->flags | SAD_BLAKE3_ROOT, words);
        for (i = 0; i < 16 && written < out_len; ++i)
        {
            unsigned char b[4];
            size_t n = 4;
            b[0] = (unsigned char)words[i]; b[1] = (unsigned char)(words[i] >> 8);
            b[2] = (unsigned char)(words[i] >> 16); b[3] = (unsigned char)(words[i] >> 24);
            if (n > out_len - written)
                n = out_len - written;
            memcpy(out + written, b, n);
            written += n;
        }
        block_counter++;
    }
}

static void sad_blake3_parent_output(const unsigned int left_cv[8], const unsigned int right_cv[8],
                                      const unsigned int key[8], unsigned int flags, SadBlake3Output *out)
{
    memcpy(out->input_cv, key, 8 * sizeof(unsigned int));
    memcpy(out->block_words, left_cv, 8 * sizeof(unsigned int));
    memcpy(out->block_words + 8, right_cv, 8 * sizeof(unsigned int));
    out->counter = 0;
    out->block_len = SAD_BLAKE3_BLOCK_LEN;
    out->flags = flags | SAD_BLAKE3_PARENT;
}

typedef struct
{
    unsigned int key[8];
    SadBlake3ChunkState chunk_state;
    unsigned int cv_stack[54][8];
    int cv_stack_len;
    unsigned int flags;
} SadBlake3Hasher;

static void sad_blake3_hasher_init_internal(SadBlake3Hasher *h, const unsigned int key[8], unsigned int flags)
{
    memcpy(h->key, key, 8 * sizeof(unsigned int));
    sad_blake3_chunk_init(&h->chunk_state, key, 0, flags);
    h->cv_stack_len = 0;
    h->flags = flags;
}

static void sad_blake3_hasher_init(SadBlake3Hasher *h) { sad_blake3_hasher_init_internal(h, sad_blake3_iv, 0); }

static void sad_blake3_hasher_init_keyed(SadBlake3Hasher *h, const unsigned char key[32])
{
    unsigned int key_words[8];
    int i;
    for (i = 0; i < 8; ++i)
        key_words[i] = (unsigned int)key[i * 4] | ((unsigned int)key[i * 4 + 1] << 8) |
                       ((unsigned int)key[i * 4 + 2] << 16) | ((unsigned int)key[i * 4 + 3] << 24);
    sad_blake3_hasher_init_internal(h, key_words, SAD_BLAKE3_KEYED_HASH);
}

static void sad_blake3_hasher_add_chunk_cv(SadBlake3Hasher *h, unsigned int new_cv[8],
                                            unsigned long long total_chunks)
{
    while ((total_chunks & 1) == 0)
    {
        unsigned int left[8];
        SadBlake3Output po;
        h->cv_stack_len--;
        memcpy(left, h->cv_stack[h->cv_stack_len], 8 * sizeof(unsigned int));
        sad_blake3_parent_output(left, new_cv, h->key, h->flags, &po);
        sad_blake3_output_cv(&po, new_cv);
        total_chunks >>= 1;
    }
    memcpy(h->cv_stack[h->cv_stack_len], new_cv, 8 * sizeof(unsigned int));
    h->cv_stack_len++;
}

static void sad_blake3_hasher_update(SadBlake3Hasher *h, const unsigned char *data, size_t len)
{
    while (len > 0)
    {
        size_t take;
        if (sad_blake3_chunk_len(&h->chunk_state) == (size_t)SAD_BLAKE3_CHUNK_LEN)
        {
            SadBlake3Output co;
            unsigned int chunk_cv[8];
            unsigned long long total_chunks;
            sad_blake3_chunk_output(&h->chunk_state, &co);
            sad_blake3_output_cv(&co, chunk_cv);
            total_chunks = h->chunk_state.chunk_counter + 1;
            sad_blake3_hasher_add_chunk_cv(h, chunk_cv, total_chunks);
            sad_blake3_chunk_init(&h->chunk_state, h->key, total_chunks, h->flags);
        }
        take = (size_t)SAD_BLAKE3_CHUNK_LEN - sad_blake3_chunk_len(&h->chunk_state);
        if (take > len)
            take = len;
        sad_blake3_chunk_update(&h->chunk_state, data, take);
        data += take;
        len -= take;
    }
}

static void sad_blake3_hasher_finalize(SadBlake3Hasher *h, unsigned char *out, size_t out_len)
{
    SadBlake3Output output;
    int remaining;
    sad_blake3_chunk_output(&h->chunk_state, &output);
    remaining = h->cv_stack_len;
    while (remaining > 0)
    {
        unsigned int cv[8];
        SadBlake3Output po;
        remaining--;
        sad_blake3_output_cv(&output, cv);
        sad_blake3_parent_output(h->cv_stack[remaining], cv, h->key, h->flags, &po);
        output = po;
    }
    sad_blake3_output_root_bytes(&output, out, out_len);
}

static void sad_blake3_raw(const char *str, unsigned char digest[32])
{
    SadBlake3Hasher h;
    size_t len = str ? strlen(str) : 0;
    sad_blake3_hasher_init(&h);
    sad_blake3_hasher_update(&h, (const unsigned char *)(str ? str : ""), len);
    sad_blake3_hasher_finalize(&h, digest, 32);
}

/* بلايك3 / BLAKE3 hash — سلسلة ست عشريّة 64 حرفًا */
const char *sad_blake3_hash(const char *str)
{
    unsigned char digest[32];
    char *out;
    size_t i;
    sad_blake3_raw(str, digest);
    out = (char *)malloc(65);
    if (!out)
        return "";
    for (i = 0; i < 32; ++i)
        snprintf(out + i * 2, 3, "%02x", digest[i]);
    out[64] = '\0';
    return out;
}

/* هاش_مفتاح / BLAKE3 keyed hash (MAC) — سلسلة ست عشريّة 64 حرفًا.
 * المفتاح: 32 بايت يُستعمَل مباشرة؛ غير ذلك يُشتقّ منه مفتاح 32 بايت عبر
 * sad_blake3_raw أوّلًا (يطابق منطق المفسّر). */
const char *sad_blake3_keyed_hash(const char *str, const char *key)
{
    unsigned char key32[32];
    unsigned char digest[32];
    SadBlake3Hasher h;
    char *out;
    size_t i, klen, slen;
    if (!key)
        key = "";
    if (!str)
        str = "";
    klen = strlen(key);
    slen = strlen(str);
    if (klen == 32)
        memcpy(key32, key, 32);
    else
        sad_blake3_raw(key, key32);
    sad_blake3_hasher_init_keyed(&h, key32);
    sad_blake3_hasher_update(&h, (const unsigned char *)str, slen);
    sad_blake3_hasher_finalize(&h, digest, 32);
    out = (char *)malloc(65);
    if (!out)
        return "";
    for (i = 0; i < 32; ++i)
        snprintf(out + i * 2, 3, "%02x", digest[i]);
    out[64] = '\0';
    return out;
}

/* ============================================================================
 * KDF — اشتق_مفتاح_مرور (PBKDF2-HMAC-SHA256, RFC 8018) و اشتق_مفتاح
 * (HKDF-SHA256, RFC 5869). ذاتيّة التنفيذ بالكامل فوق sad_sha256_raw أعلاه.
 * مطابقة حرفيًّا لنظير المفسّر (interpreter/src/builtins/builtin_module_crypto.cpp)،
 * ومُتحقَّقة قبل الدمج مقابل تنفيذ hashlib.pbkdf2_hmac المدقَّق في بايثون
 * (PBKDF2) وشعاعات RFC 5869 Appendix A الرسميّة الثلاثة (HKDF).
 * ============================================================================ */

/* HMAC-SHA256 خام — RFC 2104، فوق sad_sha256_raw */
static void sad_hmac_sha256_raw(const unsigned char *key, size_t klen,
                                 const unsigned char *msg, size_t mlen, unsigned char out[32])
{
    unsigned char k0[64];
    unsigned char ipad[64], opad[64];
    unsigned char inner_hash[32];
    unsigned char *inner_buf;
    unsigned char outer_buf[64 + 32];
    size_t i;
    memset(k0, 0, 64);
    if (klen > 64)
        sad_sha256_raw(key, klen, k0);
    else
        memcpy(k0, key, klen);
    for (i = 0; i < 64; ++i)
    {
        ipad[i] = (unsigned char)(k0[i] ^ 0x36);
        opad[i] = (unsigned char)(k0[i] ^ 0x5c);
    }
    inner_buf = (unsigned char *)malloc(64 + mlen);
    if (!inner_buf)
    {
        memset(out, 0, 32);
        return;
    }
    memcpy(inner_buf, ipad, 64);
    memcpy(inner_buf + 64, msg, mlen);
    sad_sha256_raw(inner_buf, 64 + mlen, inner_hash);
    free(inner_buf);
    memcpy(outer_buf, opad, 64);
    memcpy(outer_buf + 64, inner_hash, 32);
    sad_sha256_raw(outer_buf, 96, out);
}

/* PBKDF2-HMAC-SHA256 خام — RFC 8018 */
static void sad_pbkdf2_hmac_sha256_raw(const unsigned char *pw, size_t pwlen,
                                        const unsigned char *salt, size_t saltlen,
                                        unsigned long long iterations, unsigned char *out, size_t dklen)
{
    unsigned int nblocks = (unsigned int)((dklen + 31) / 32);
    unsigned int blk;
    unsigned char *salt_ctr = (unsigned char *)malloc(saltlen + 4);
    if (!salt_ctr)
    {
        memset(out, 0, dklen);
        return;
    }
    memcpy(salt_ctr, salt, saltlen);
    for (blk = 1; blk <= nblocks; ++blk)
    {
        unsigned char u[32], t[32];
        unsigned long long iter;
        size_t off, take;
        salt_ctr[saltlen + 0] = (unsigned char)(blk >> 24);
        salt_ctr[saltlen + 1] = (unsigned char)(blk >> 16);
        salt_ctr[saltlen + 2] = (unsigned char)(blk >> 8);
        salt_ctr[saltlen + 3] = (unsigned char)(blk);
        sad_hmac_sha256_raw(pw, pwlen, salt_ctr, saltlen + 4, u);
        memcpy(t, u, 32);
        for (iter = 1; iter < iterations; ++iter)
        {
            unsigned char un[32];
            int j;
            sad_hmac_sha256_raw(pw, pwlen, u, 32, un);
            memcpy(u, un, 32);
            for (j = 0; j < 32; ++j)
                t[j] ^= u[j];
        }
        off = (size_t)(blk - 1) * 32;
        take = (dklen - off < 32) ? (dklen - off) : 32;
        memcpy(out + off, t, take);
    }
    free(salt_ctr);
}

/* اشتق_مفتاح_مرور / PBKDF2-HMAC-SHA256 — مفتاح 32 بايت ثابت، سلسلة ست عشريّة 64 حرفًا */
const char *sad_kdf_pbkdf2(const char *password, const char *salt, long long iterations)
{
    unsigned char digest[32];
    char *out;
    size_t i, pwlen, saltlen;
    unsigned long long iters;
    if (iterations <= 0)
    {
        fprintf(stderr, "[sad] خطأ: عدد تكرارات PBKDF2 يجب أن يكون أكبر من صفر\n");
        return "";
    }
    if (!password)
        password = "";
    if (!salt)
        salt = "";
    pwlen = strlen(password);
    saltlen = strlen(salt);
    iters = (unsigned long long)iterations;
    sad_pbkdf2_hmac_sha256_raw((const unsigned char *)password, pwlen,
                                (const unsigned char *)salt, saltlen, iters, digest, 32);
    out = (char *)malloc(65);
    if (!out)
        return "";
    for (i = 0; i < 32; ++i)
        snprintf(out + i * 2, 3, "%02x", digest[i]);
    out[64] = '\0';
    return out;
}

/* HKDF-Extract — RFC 5869 §2.2 */
static void sad_hkdf_extract(const unsigned char *salt, size_t saltlen,
                              const unsigned char *ikm, size_t ikmlen, unsigned char prk[32])
{
    unsigned char zero_salt[32];
    if (saltlen == 0)
    {
        memset(zero_salt, 0, 32);
        sad_hmac_sha256_raw(zero_salt, 32, ikm, ikmlen, prk);
    }
    else
    {
        sad_hmac_sha256_raw(salt, saltlen, ikm, ikmlen, prk);
    }
}

/* HKDF-Expand — RFC 5869 §2.3 */
static void sad_hkdf_expand(const unsigned char prk[32], const unsigned char *info, size_t infolen,
                             unsigned char *okm, size_t l)
{
    unsigned char t[32];
    size_t tlen = 0;
    unsigned char *buf = (unsigned char *)malloc(32 + infolen + 1);
    unsigned int i = 1;
    size_t produced = 0;
    if (!buf)
    {
        memset(okm, 0, l);
        return;
    }
    while (produced < l)
    {
        size_t buflen = 0;
        size_t take;
        memcpy(buf, t, tlen);
        buflen += tlen;
        memcpy(buf + buflen, info, infolen);
        buflen += infolen;
        buf[buflen] = (unsigned char)i;
        buflen += 1;
        sad_hmac_sha256_raw(prk, 32, buf, buflen, t);
        tlen = 32;
        take = (l - produced < 32) ? (l - produced) : 32;
        memcpy(okm + produced, t, take);
        produced += take;
        ++i;
    }
    free(buf);
}

/* اشتق_مفتاح / HKDF-SHA256 — طول مخرَج اختياريّ، سلسلة ست عشريّة (2×الطول حرفًا) */
const char *sad_kdf_hkdf(const char *secret, const char *salt, const char *info, long long length)
{
    unsigned char prk[32];
    unsigned char *okm;
    char *out;
    size_t i, secretlen, saltlen, infolen, l;
    /* RFC 5869 §2.3: الحدّ الأقصى للناتج L <= 255 * HashLen (32 بايت) = 8160 بايت */
    if (length <= 0 || length > 8160)
    {
        fprintf(stderr, "[sad] خطأ: طول ناتج HKDF يجب أن يكون بين 1 و8160 بايت\n");
        return "";
    }
    if (!secret)
        secret = "";
    if (!salt)
        salt = "";
    if (!info)
        info = "";
    secretlen = strlen(secret);
    saltlen = strlen(salt);
    infolen = strlen(info);
    l = (size_t)length;
    okm = (unsigned char *)malloc(l);
    if (!okm)
        return "";
    sad_hkdf_extract((const unsigned char *)salt, saltlen, (const unsigned char *)secret, secretlen, prk);
    sad_hkdf_expand(prk, (const unsigned char *)info, infolen, okm, l);
    out = (char *)malloc(l * 2 + 1);
    if (!out)
    {
        free(okm);
        return "";
    }
    for (i = 0; i < l; ++i)
        snprintf(out + i * 2, 3, "%02x", okm[i]);
    out[l * 2] = '\0';
    free(okm);
    return out;
}

/* ============================================================================
 * أرجون2 / Argon2id (RFC 9106), parallelism fixed at 1 — memory-hard password
 * hashing. Built on a self-implemented BLAKE2b (RFC 7693), independent of
 * SHA-256 above. Verified byte-for-byte against libargon2 (argon2-cffi)
 * across varying memory cost, iterations, and input lengths before merge.
 * ============================================================================ */
static const unsigned long long SAD_B2B_IV[8] = {
    0x6A09E667F3BCC908ULL, 0xBB67AE8584CAA73BULL, 0x3C6EF372FE94F82BULL, 0xA54FF53A5F1D36F1ULL,
    0x510E527FADE682D1ULL, 0x9B05688C2B3E6C1FULL, 0x1F83D9ABFB41BD6BULL, 0x5BE0CD19137E2179ULL};

static const unsigned char SAD_B2B_SIGMA[12][16] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3},
    {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4},
    {7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8},
    {9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13},
    {2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9},
    {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11},
    {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10},
    {6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5},
    {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3}};

static unsigned long long sad_rotr64(unsigned long long x, int n) { return (x >> n) | (x << (64 - n)); }

typedef struct
{
    unsigned long long h[8];
    unsigned long long t[2];
    unsigned char buf[128];
    size_t buflen;
    size_t outlen;
} sad_b2b_state;

static void sad_b2b_g(unsigned long long *v, int a, int b, int c, int d, unsigned long long x, unsigned long long y)
{
    v[a] = v[a] + v[b] + x;
    v[d] = sad_rotr64(v[d] ^ v[a], 32);
    v[c] = v[c] + v[d];
    v[b] = sad_rotr64(v[b] ^ v[c], 24);
    v[a] = v[a] + v[b] + y;
    v[d] = sad_rotr64(v[d] ^ v[a], 16);
    v[c] = v[c] + v[d];
    v[b] = sad_rotr64(v[b] ^ v[c], 63);
}

static void sad_b2b_compress(sad_b2b_state *s, const unsigned char block[128], int last)
{
    unsigned long long m[16], v[16];
    int i, j;
    for (i = 0; i < 16; ++i)
    {
        m[i] = 0;
        for (j = 0; j < 8; ++j)
            m[i] |= ((unsigned long long)block[i * 8 + j]) << (8 * j);
    }
    for (i = 0; i < 8; ++i)
        v[i] = s->h[i];
    for (i = 0; i < 8; ++i)
        v[8 + i] = SAD_B2B_IV[i];
    v[12] ^= s->t[0];
    v[13] ^= s->t[1];
    if (last)
        v[14] = ~v[14];
    for (i = 0; i < 12; ++i)
    {
        const unsigned char *sg = SAD_B2B_SIGMA[i];
        sad_b2b_g(v, 0, 4, 8, 12, m[sg[0]], m[sg[1]]);
        sad_b2b_g(v, 1, 5, 9, 13, m[sg[2]], m[sg[3]]);
        sad_b2b_g(v, 2, 6, 10, 14, m[sg[4]], m[sg[5]]);
        sad_b2b_g(v, 3, 7, 11, 15, m[sg[6]], m[sg[7]]);
        sad_b2b_g(v, 0, 5, 10, 15, m[sg[8]], m[sg[9]]);
        sad_b2b_g(v, 1, 6, 11, 12, m[sg[10]], m[sg[11]]);
        sad_b2b_g(v, 2, 7, 8, 13, m[sg[12]], m[sg[13]]);
        sad_b2b_g(v, 3, 4, 9, 14, m[sg[14]], m[sg[15]]);
    }
    for (i = 0; i < 8; ++i)
        s->h[i] ^= v[i] ^ v[i + 8];
}

static void sad_b2b_init(sad_b2b_state *s, size_t outlen)
{
    memset(s, 0, sizeof(*s));
    memcpy(s->h, SAD_B2B_IV, sizeof(s->h));
    s->h[0] ^= 0x01010000ULL ^ (unsigned long long)outlen;
    s->outlen = outlen;
}

static void sad_b2b_update(sad_b2b_state *s, const unsigned char *data, size_t len)
{
    size_t i;
    for (i = 0; i < len; ++i)
    {
        if (s->buflen == 128)
        {
            s->t[0] += 128;
            if (s->t[0] < 128)
                s->t[1]++;
            sad_b2b_compress(s, s->buf, 0);
            s->buflen = 0;
        }
        s->buf[s->buflen++] = data[i];
    }
}

static void sad_b2b_final(sad_b2b_state *s, unsigned char *out)
{
    size_t i;
    s->t[0] += s->buflen;
    if (s->t[0] < s->buflen)
        s->t[1]++;
    while (s->buflen < 128)
        s->buf[s->buflen++] = 0;
    sad_b2b_compress(s, s->buf, 1);
    for (i = 0; i < s->outlen; ++i)
        out[i] = (unsigned char)(s->h[i / 8] >> (8 * (i % 8)));
}

static void sad_blake2b(unsigned char *out, size_t outlen, const unsigned char *in, size_t inlen)
{
    sad_b2b_state s;
    sad_b2b_init(&s, outlen);
    sad_b2b_update(&s, in, inlen);
    sad_b2b_final(&s, out);
}

/* Argon2's variable-length hash H' (RFC 9106 §3.2) built on BLAKE2b */
static void sad_argon2_hprime(unsigned char *out, size_t outlen, const unsigned char *in, size_t inlen)
{
    unsigned char le_len[4];
    le_len[0] = (unsigned char)outlen;
    le_len[1] = (unsigned char)(outlen >> 8);
    le_len[2] = (unsigned char)(outlen >> 16);
    le_len[3] = (unsigned char)(outlen >> 24);

    if (outlen <= 64)
    {
        sad_b2b_state s;
        sad_b2b_init(&s, outlen);
        sad_b2b_update(&s, le_len, 4);
        sad_b2b_update(&s, in, inlen);
        sad_b2b_final(&s, out);
        return;
    }
    {
        unsigned char v[64];
        sad_b2b_state s;
        size_t produced;
        sad_b2b_init(&s, 64);
        sad_b2b_update(&s, le_len, 4);
        sad_b2b_update(&s, in, inlen);
        sad_b2b_final(&s, v);
        memcpy(out, v, 32);
        produced = 32;
        while (outlen - produced > 64)
        {
            sad_blake2b(v, 64, v, 64);
            memcpy(out + produced, v, 32);
            produced += 32;
        }
        sad_blake2b(v, outlen - produced, v, 64);
        memcpy(out + produced, v, outlen - produced);
    }
}

#define SAD_ARGON2_QWORDS_IN_BLOCK 128
#define SAD_ARGON2_BLOCK_SIZE (SAD_ARGON2_QWORDS_IN_BLOCK * 8)
#define SAD_ARGON2_SYNC_POINTS 4

typedef struct
{
    unsigned long long v[SAD_ARGON2_QWORDS_IN_BLOCK];
} sad_argon2_block_t;

static void sad_argon2_block_xor(sad_argon2_block_t *dst, const sad_argon2_block_t *a, const sad_argon2_block_t *b)
{
    int i;
    for (i = 0; i < SAD_ARGON2_QWORDS_IN_BLOCK; ++i)
        dst->v[i] = a->v[i] ^ b->v[i];
}

/* Argon2's "GB" mixing function (Blake2b round function with the message
 * terms replaced by 2*lo32(a)*lo32(b) — RFC 9106 §3.5) applied in-place on a
 * positional 16-word vector. */
static void sad_argon2_p(unsigned long long *v)
{
    int i;
    static const int perm[8][4] = {
        {0, 4, 8, 12}, {1, 5, 9, 13}, {2, 6, 10, 14}, {3, 7, 11, 15}, {0, 5, 10, 15}, {1, 6, 11, 12}, {2, 7, 8, 13}, {3, 4, 9, 14}};
    for (i = 0; i < 8; ++i)
    {
        int a = perm[i][0], b = perm[i][1], c = perm[i][2], d = perm[i][3];
        v[a] = v[a] + v[b] + 2ULL * (unsigned long long)(unsigned int)v[a] * (unsigned long long)(unsigned int)v[b];
        v[d] = sad_rotr64(v[d] ^ v[a], 32);
        v[c] = v[c] + v[d] + 2ULL * (unsigned long long)(unsigned int)v[c] * (unsigned long long)(unsigned int)v[d];
        v[b] = sad_rotr64(v[b] ^ v[c], 24);
        v[a] = v[a] + v[b] + 2ULL * (unsigned long long)(unsigned int)v[a] * (unsigned long long)(unsigned int)v[b];
        v[d] = sad_rotr64(v[d] ^ v[a], 16);
        v[c] = v[c] + v[d] + 2ULL * (unsigned long long)(unsigned int)v[c] * (unsigned long long)(unsigned int)v[d];
        v[b] = sad_rotr64(v[b] ^ v[c], 63);
    }
}

/* Argon2 compression function G(X, Y) -> out, RFC 9106 §3.5. If with_xor,
 * out is additionally XORed with the block's OLD value (*prev_out), read
 * before overwriting — prev_out is allowed to alias out itself. */
static void sad_argon2_fill_block(const sad_argon2_block_t *x, const sad_argon2_block_t *y, sad_argon2_block_t *out,
                                   int with_xor, const sad_argon2_block_t *prev_out)
{
    sad_argon2_block_t r, q, newval;
    int i, m;
    sad_argon2_block_xor(&r, x, y);
    q = r;
    for (i = 0; i < 8; ++i)
        sad_argon2_p(&q.v[16 * i]);
    for (i = 0; i < 8; ++i)
    {
        unsigned long long col[16];
        for (m = 0; m < 8; ++m)
        {
            col[2 * m] = q.v[2 * i + 16 * m];
            col[2 * m + 1] = q.v[2 * i + 16 * m + 1];
        }
        sad_argon2_p(col);
        for (m = 0; m < 8; ++m)
        {
            q.v[2 * i + 16 * m] = col[2 * m];
            q.v[2 * i + 16 * m + 1] = col[2 * m + 1];
        }
    }
    for (i = 0; i < SAD_ARGON2_QWORDS_IN_BLOCK; ++i)
        newval.v[i] = q.v[i] ^ r.v[i];
    if (with_xor)
        for (i = 0; i < SAD_ARGON2_QWORDS_IN_BLOCK; ++i)
            newval.v[i] ^= prev_out->v[i];
    *out = newval;
}

typedef struct
{
    unsigned long long pass, lane, slice, m_prime, t_prime, type, counter;
} sad_argon2_addr_input_t;

static void sad_argon2_gen_addr_block(sad_argon2_block_t *addr, const sad_argon2_addr_input_t *ai)
{
    sad_argon2_block_t zero, in, tmp;
    memset(&zero, 0, sizeof(sad_argon2_block_t));
    memset(&in, 0, sizeof(sad_argon2_block_t));
    in.v[0] = ai->pass;
    in.v[1] = ai->lane;
    in.v[2] = ai->slice;
    in.v[3] = ai->m_prime;
    in.v[4] = ai->t_prime;
    in.v[5] = ai->type;
    in.v[6] = ai->counter;
    sad_argon2_fill_block(&zero, &in, &tmp, 0, NULL);
    sad_argon2_fill_block(&zero, &tmp, addr, 0, NULL);
}

/* RFC 9106 §3.4 indexing, restricted to parallelism = 1 (always same-lane). */
static unsigned int sad_argon2_index_alpha(unsigned int pass, unsigned int slice, unsigned int seg_len, unsigned int index,
                                            unsigned long long rand64, unsigned int lane_len)
{
    unsigned long long reference_area_size;
    unsigned long long rel;
    unsigned int start_position;

    if (pass == 0)
    {
        if (slice == 0)
            reference_area_size = index - 1;
        else
            reference_area_size = (unsigned long long)slice * seg_len + index - 1;
    }
    else
    {
        reference_area_size = (unsigned long long)lane_len - seg_len + index - 1;
    }

    rel = rand64 & 0xFFFFFFFFULL;
    rel = (rel * rel) >> 32;
    rel = reference_area_size - 1 - ((reference_area_size * rel) >> 32);

    start_position = 0;
    if (pass != 0)
        start_position = (slice == SAD_ARGON2_SYNC_POINTS - 1) ? 0 : (slice + 1) * seg_len;

    return (unsigned int)((start_position + rel) % lane_len);
}

/* أرجون2 / Argon2id — اشتقاق مفتاح صعب الحساب ذاكرةً وزمنًا (RFC 9106)،
 * مقاوم لتسريع الاختراق بالعتاد المخصَّص (ASIC/GPU) بخلاف PBKDF2 البسيط
 * زمنيًّا فقط. يُرجع مفتاحًا 32 بايت (256 بت) كسلسلة ست عشريّة من 64 حرفًا.
 * التوازي (p) ثابت على 1 عمدًا (يبسّط التنفيذ، يتجنّب تعقيد التزامن متعدّد
 * الخيوط في وقت التشغيل المضمَّن). */
const char *sad_kdf_argon2id(const char *password, const char *salt, long long memory_cost_kib, long long iterations)
{
    unsigned int m_cost, t_cost;
    size_t pwlen, saltlen;
    unsigned int lanes = 1;
    unsigned int m_prime, lane_len, seg_len;
    unsigned char h0[64];
    sad_argon2_block_t *B;
    unsigned int pass, slice;
    char *out;
    size_t i;

    if (!password)
        password = "";
    if (!salt)
        salt = "";
    pwlen = strlen(password);
    saltlen = strlen(salt);

    if (memory_cost_kib < 8)
    {
        fprintf(stderr, "[sad] خطأ: تكلفة ذاكرة أرجون2 يجب أن تكون 8 كيلوبايت على الأقلّ\n");
        return "";
    }
    if (iterations <= 0)
    {
        fprintf(stderr, "[sad] خطأ: عدد تكرارات أرجون2 يجب أن يكون أكبر من صفر\n");
        return "";
    }
    if (saltlen < 8)
    {
        fprintf(stderr, "[sad] خطأ: ملح أرجون2 يجب أن يكون 8 بايت على الأقلّ\n");
        return "";
    }

    m_cost = (unsigned int)memory_cost_kib;
    t_cost = (unsigned int)iterations;
    m_prime = (m_cost / 4) * 4;
    if (m_prime < 2 * SAD_ARGON2_SYNC_POINTS * lanes)
        m_prime = 2 * SAD_ARGON2_SYNC_POINTS * lanes;
    lane_len = m_prime / lanes;
    seg_len = lane_len / SAD_ARGON2_SYNC_POINTS;

    {
        unsigned char buf[4];
        sad_b2b_state s;
        sad_b2b_init(&s, 64);
#define SAD_ARGON2_PUT32(val)                          \
    buf[0] = (unsigned char)(val);                     \
    buf[1] = (unsigned char)((val) >> 8);              \
    buf[2] = (unsigned char)((val) >> 16);             \
    buf[3] = (unsigned char)((val) >> 24);             \
    sad_b2b_update(&s, buf, 4)
        SAD_ARGON2_PUT32(lanes);
        SAD_ARGON2_PUT32(32u); /* tag length */
        SAD_ARGON2_PUT32(m_cost); /* RFC 9106 §3.2 H0 uses the raw memory cost, not the rounded-down m' */
        SAD_ARGON2_PUT32(t_cost);
        SAD_ARGON2_PUT32(0x13u); /* version 19 */
        SAD_ARGON2_PUT32(2u);    /* type = Argon2id */
        SAD_ARGON2_PUT32((unsigned int)pwlen);
        sad_b2b_update(&s, (const unsigned char *)password, pwlen);
        SAD_ARGON2_PUT32((unsigned int)saltlen);
        sad_b2b_update(&s, (const unsigned char *)salt, saltlen);
        SAD_ARGON2_PUT32(0u); /* secret length */
        SAD_ARGON2_PUT32(0u); /* associated data length */
        sad_b2b_final(&s, h0);
#undef SAD_ARGON2_PUT32
    }

    B = (sad_argon2_block_t *)malloc((size_t)m_prime * sizeof(sad_argon2_block_t));
    if (!B)
        return "";

    {
        unsigned char seed[72];
        memcpy(seed, h0, 64);
        memset(seed + 64, 0, 8);
        sad_argon2_hprime((unsigned char *)&B[0], SAD_ARGON2_BLOCK_SIZE, seed, 72);
        seed[64] = 1;
        sad_argon2_hprime((unsigned char *)&B[1], SAD_ARGON2_BLOCK_SIZE, seed, 72);
    }

    for (pass = 0; pass < t_cost; ++pass)
    {
        for (slice = 0; slice < SAD_ARGON2_SYNC_POINTS; ++slice)
        {
            int data_independent = (pass == 0 && slice < SAD_ARGON2_SYNC_POINTS / 2) ? 1 : 0;
            sad_argon2_block_t addr_block;
            sad_argon2_addr_input_t ai;
            unsigned int start_index = (pass == 0 && slice == 0) ? 2 : 0;
            unsigned int j;
            ai.pass = pass;
            ai.lane = 0;
            ai.slice = slice;
            ai.m_prime = m_prime;
            ai.t_prime = t_cost;
            ai.type = 2;
            ai.counter = 0;

            for (j = 0; j < seg_len; ++j)
            {
                unsigned int pos_in_lane, curr_index, prev_index;
                unsigned long long rand64;
                unsigned int ref_index;
                int with_xor;

                if (data_independent && j % SAD_ARGON2_QWORDS_IN_BLOCK == 0)
                {
                    ai.counter++;
                    sad_argon2_gen_addr_block(&addr_block, &ai);
                }
                if (j < start_index)
                    continue;

                pos_in_lane = slice * seg_len + j;
                curr_index = pos_in_lane;
                prev_index = (pos_in_lane == 0) ? (lane_len - 1) : (curr_index - 1);

                if (data_independent)
                    rand64 = addr_block.v[j % SAD_ARGON2_QWORDS_IN_BLOCK];
                else
                    rand64 = B[prev_index].v[0];

                ref_index = sad_argon2_index_alpha(pass, slice, seg_len, j, rand64, lane_len);
                with_xor = (pass != 0);
                sad_argon2_fill_block(&B[prev_index], &B[ref_index], &B[curr_index], with_xor, &B[curr_index]);
            }
        }
    }

    {
        unsigned char tag[32];
        sad_argon2_hprime(tag, 32, (unsigned char *)&B[lane_len - 1], SAD_ARGON2_BLOCK_SIZE);
        free(B);
        out = (char *)malloc(65);
        if (!out)
            return "";
        for (i = 0; i < 32; ++i)
            snprintf(out + i * 2, 3, "%02x", tag[i]);
        out[64] = '\0';
        return out;
    }
}

/* وقت_الآن / Current timestamp */
long long sad_security_timestamp(void)
{
    return (long long)time(NULL);
}

/* شفّر / Encrypt — تشفير-تيار SHA-256-CTR (يطابق مخطط المفسّر حرفيًّا) */
const char *sad_security_encrypt(const char *text, const char *key)
{
    size_t tlen, klen, pos, i;
    unsigned long long nonce, ctr;
    unsigned char *cipher; /* [8-byte nonce][ciphertext bytes] */
    char *result;

    if (!text || !key || !*key)
        return text;

    tlen = strlen(text);
    klen = strlen(key);

    if (tlen > (SIZE_MAX / 4 - 16))
    {
        fprintf(stderr, "[sad] خطأ: النص طويل جداً للتشفير\n");
        return text;
    }

    cipher = (unsigned char *)malloc(8 + tlen);
    if (!cipher)
    {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة للتشفير\n");
        return text;
    }

    nonce = sad_crypto_random_u64();
    for (i = 0; i < 8; ++i)
        cipher[i] = (unsigned char)(nonce >> ((7 - i) * 8));

    /* (AR) مخزن [مفتاح][nonce][عدّاد] بلا سقفٍ لطول المفتاح — يطابق المفسّر
     *      الذي يقبل مفاتيح بأيّ طول (std::vector غير محدود). تخصيص واحد
     *      خارج الحلقة بدل مصفوفة مكدّس ثابتة كانت تقطع صامتًا عند >256 بايت. */
    unsigned char *input = (unsigned char *)malloc(klen + 16);
    if (!input)
    {
        free(cipher);
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة للتشفير\n");
        return text;
    }
    memcpy(input, key, klen);

    pos = 0;
    ctr = 0;
    while (pos < tlen)
    {
        unsigned char block[32];
        size_t j;
        for (i = 0; i < 8; ++i)
            input[klen + i] = (unsigned char)(nonce >> ((7 - i) * 8));
        for (i = 0; i < 8; ++i)
            input[klen + 8 + i] = (unsigned char)(ctr >> ((7 - i) * 8));
        sad_sha256_raw(input, klen + 16, block);
        for (j = 0; j < 32 && pos < tlen; ++j, ++pos)
            cipher[8 + pos] = (unsigned char)text[pos] ^ block[j];
        ++ctr;
    }
    free(input);

    result = (char *)malloc((8 + tlen) * 2 + 1);
    if (!result)
    {
        free(cipher);
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة للتشفير\n");
        return text;
    }
    for (i = 0; i < 8 + tlen; ++i)
        snprintf(result + i * 2, 3, "%02x", cipher[i]);
    result[(8 + tlen) * 2] = '\0';
    free(cipher);
    return result;
}

/* فك_تشفير / Decrypt — فك تشفير SHA-256-CTR (يطابق مخطط المفسّر حرفيًّا) */
const char *sad_security_decrypt(const char *hex, const char *key)
{
    size_t hlen, rlen, klen, pos, i;
    unsigned char *raw;
    unsigned long long nonce, ctr;
    char *result;

    if (!hex || !key || !*key)
        return hex;

    hlen = strlen(hex);
    if (hlen % 2 != 0)
    {
        fprintf(stderr, "[sad] خطأ: طول النص المشفر غير صالح\n");
        return hex;
    }

    rlen = hlen / 2;
    if (rlen < 8)
    {
        fprintf(stderr, "[sad] خطأ: طول النص المشفر غير صالح\n");
        return hex;
    }
    klen = strlen(key);

    raw = (unsigned char *)malloc(rlen);
    if (!raw)
    {
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة لفك التشفير\n");
        return hex;
    }
    for (i = 0; i < rlen; i++)
    {
        unsigned int byte = 0;
        if (sscanf(hex + i * 2, "%02x", &byte) != 1 || byte > 255)
        {
            free(raw);
            fprintf(stderr, "[sad] خطأ: تنسيق hex غير صالح عند الموقع %zu\n", i);
            return hex;
        }
        raw[i] = (unsigned char)byte;
    }

    nonce = 0;
    for (i = 0; i < 8; ++i)
        nonce = (nonce << 8) | raw[i];

    result = (char *)malloc(rlen - 8 + 1);
    if (!result)
    {
        free(raw);
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة لفك التشفير\n");
        return hex;
    }

    /* (AR) بلا سقفٍ لطول المفتاح — انظر التعليق المطابق في sad_security_encrypt. */
    unsigned char *input = (unsigned char *)malloc(klen + 16);
    if (!input)
    {
        free(raw);
        free(result);
        fprintf(stderr, "[sad] خطأ: فشل تخصيص الذاكرة لفك التشفير\n");
        return hex;
    }
    memcpy(input, key, klen);

    pos = 8;
    ctr = 0;
    while (pos < rlen)
    {
        unsigned char block[32];
        size_t j;
        for (i = 0; i < 8; ++i)
            input[klen + i] = (unsigned char)(nonce >> ((7 - i) * 8));
        for (i = 0; i < 8; ++i)
            input[klen + 8 + i] = (unsigned char)(ctr >> ((7 - i) * 8));
        sad_sha256_raw(input, klen + 16, block);
        for (j = 0; j < 32 && pos < rlen; ++j, ++pos)
            result[pos - 8] = (char)(raw[pos] ^ block[j]);
        ++ctr;
    }
    free(input);
    result[rlen - 8] = '\0';
    free(raw);
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

/* عشوائي_آمن / Secure random in range — CSPRNG حقيقيّ على الأهداف المستضافة
 * (انظر sad_crypto_os_random أعلاه)؛ يتراجع لمسار srand/rand القديم فقط تحت
 * SAD_FREESTANDING أو إن تعذّر الوصول لمصدر عشوائيّة النظام. */
long long sad_security_secure_random(long long min_val, long long max_val)
{
    unsigned long long range;
    if (min_val >= max_val)
        return min_val;
    range = (unsigned long long)(max_val - min_val + 1);
#ifndef SAD_FREESTANDING
    {
        unsigned char bytes[8];
        if (sad_crypto_os_random(bytes, sizeof(bytes)))
        {
            unsigned long long r = 0;
            int i;
            for (i = 0; i < 8; ++i)
                r = (r << 8) | bytes[i];
            return min_val + (long long)(r % range);
        }
    }
#endif
    {
        static int seeded = 0;
        if (!seeded)
        {
            srand((unsigned int)time(NULL));
            seeded = 1;
        }
        return min_val + (long long)(rand() % (int)range);
    }
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
                char buf[512]; /* ISSUE-076 (Amelia #8): %.6f DBL_MAX ~316 chars */
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
                char buf[512]; /* ISSUE-076 (Amelia #8): %.6f DBL_MAX ~316 chars */
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
