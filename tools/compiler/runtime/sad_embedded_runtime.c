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
#include <sys/stat.h>   /* stat/S_ISDIR — لدعم sad_file_is_dir على POSIX */
#include <unistd.h>     /* getcwd — لدعم sad_file_abs_path على POSIX */
#endif

/* ============================================================================
 * (AR) جسر نظام الملفّات — يُستدعى من مدمَج «هل_مجلد» في المترجم.
 *      يُرجع 1 إن كان المسار مجلدًا موجودًا، و0 خلاف ذلك (غير موجود/ملفّ).
 *      يوحّد سلوك المفسّر (sad::stdlib::filesystem::is_directory) في المُصرَّف.
 * (EN) Filesystem bridge — called by the compiler's «هل_مجلد» builtin.
 *      Returns 1 if the path is an existing directory, else 0 (missing/file).
 *      Unifies the interpreter's is_directory semantics for compiled programs.
 * ============================================================================ */
int sad_file_is_dir(const char *path)
{
    if (!path)
        return 0;
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES)
        return 0;
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
#else
    struct stat st;
    if (stat(path, &st) != 0)
        return 0;
    return S_ISDIR(st.st_mode) ? 1 : 0;
#endif
}

/* ============================================================================
 * (AR) جسر «أنشئ_مجلد» — كانت الواجهةُ الخلفيّة تُصدِر نداءً إلى `sad_file_create_dir`
 *      بلا تعريفٍ له في زمن التشغيل المُضمَّن، فيفشل **الربطُ** لا التصريف: رسالةٌ
 *      عن clang لا عن المدمَجة، فيظنّ المستعمل أنّ سلسلة أدواته معطوبة.
 * (EN) «أنشئ_مجلد» bridge — the backend emitted a call to `sad_file_create_dir`
 *      with no definition in the embedded runtime, so LINKING failed rather than
 *      compilation: the error blamed clang, not the builtin, sending users to
 *      debug a toolchain that was fine.
 * ============================================================================ */
int sad_file_create_dir(const char *path)
{
    if (!path)
        return 0;
#ifdef _WIN32
    return CreateDirectoryA(path, NULL) ? 1 : 0;
#else
    return mkdir(path, 0755) == 0 ? 1 : 0;
#endif
}

/* ============================================================================
 * (AR) جسر «احذف_مجلد» — يحذف مجلّدًا **فارغًا** فقط (نظير rmdir). الحذفُ العَوديّ
 *      عمليّةٌ مدمّرةٌ لا تُشتَقّ ضمنًا من «احذف مجلّدًا»؛ من أرادها يسردْ ويحذفْ صراحةً.
 * (EN) «احذف_مجلد» bridge — removes an EMPTY directory only (rmdir). Recursive
 *      deletion is a destructive operation and is not implied by "remove a
 *      directory"; callers that want it must list and delete explicitly.
 * ============================================================================ */
int sad_file_remove_dir(const char *path)
{
    if (!path)
        return 0;
#ifdef _WIN32
    return RemoveDirectoryA(path) ? 1 : 0;
#else
    return rmdir(path) == 0 ? 1 : 0;
#endif
}

/* ============================================================================
 * (AR) جسر «هل_موجود» — أيُّ مدخلٍ موجود (ملفًّا كان أو مجلّدًا)، كدلالة المفسّر.
 *      لا يكفي «هل_ملف» هنا: المجلّد موجودٌ وليس ملفًّا، والخلطُ بينهما يجعل
 *      فحصَ الوجود يكذب على المجلّدات.
 * (EN) «هل_موجود» bridge — any existing entry, file or directory (interpreter
 *      semantics). «هل_ملف» is not a substitute: a directory exists but is not a
 *      file, and conflating them makes existence checks lie about directories.
 * ============================================================================ */
int sad_file_exists(const char *path)
{
    if (!path)
        return 0;
#ifdef _WIN32
    return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES ? 1 : 0;
#else
    struct stat st;
    return stat(path, &st) == 0 ? 1 : 0;
#endif
}

/* ============================================================================
 * (AR) جسر «هل_ملف» — ملفٌّ عاديٌّ موجود. يتبع الرابطَ الرمزيّ فيصف هدفه (stat لا
 *      lstat)، مطابقةً لدلالة المفسّر. ولذلك لا يصلح لكشف الرابط نفسه — لذاك
 *      «هل_رابط_رمزي» أدناه.
 * (EN) «هل_ملف» bridge — an existing regular file. Follows symlinks and describes
 *      the target (stat, not lstat), matching the interpreter. Hence it cannot
 *      detect the link itself — «هل_رابط_رمزي» below does that.
 * ============================================================================ */
int sad_file_is_file(const char *path)
{
    if (!path)
        return 0;
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES)
        return 0;
    return (attrs & FILE_ATTRIBUTE_DIRECTORY) ? 0 : 1;
#else
    struct stat st;
    if (stat(path, &st) != 0)
        return 0;
    return S_ISREG(st.st_mode) ? 1 : 0;
#endif
}

/* ============================================================================
 * (AR) جسر «هل_رابط_رمزي» — يفحص المدخلَ نفسه بلا اتّباع الرابط (نظير lstat).
 *      لا يكفي «هل_ملف»/«هل_مجلد» هنا: كلاهما يتبع الرابطَ فيصف الهدف، فلا
 *      يكشف الرابطَ نفسه — وهو بالضبط ما يلزم لفرض احتواء المسارات.
 * (EN) «هل_رابط_رمزي» bridge — inspects the entry itself without following the
 *      link (an lstat equivalent). «هل_ملف»/«هل_مجلد» both follow the link and
 *      describe its target, so neither detects the link — which is exactly what
 *      path-containment enforcement needs.
 * ============================================================================ */
int sad_file_is_symlink(const char *path)
{
    if (!path)
        return 0;
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES)
        return 0;
    return (attrs & FILE_ATTRIBUTE_REPARSE_POINT) ? 1 : 0;
#else
    struct stat st;
    if (lstat(path, &st) != 0)
        return 0;
    return S_ISLNK(st.st_mode) ? 1 : 0;
#endif
}

/* ============================================================================
 * (AR) جسر «المسار_الحقيقي» — يحلّ الروابط الرمزيّة ويطبّع «..».
 *      يُرجع مؤشّرًا مخصَّصًا (على المستدعي تحريره) أو NULL إن تعذّر الحلّ —
 *      والغيابُ حالةٌ متوقّعة (ملفٌّ يُنشَأ) لا خطأ، فيُترجَم في اللغة إلى «عدم».
 * (EN) «المسار_الحقيقي» bridge — resolves symlinks and normalises "..".
 *      Returns an allocated pointer (caller frees) or NULL when resolution
 *      fails; a missing path is an expected case (a file about to be created),
 *      surfaced in the language as null rather than an error.
 * ============================================================================ */
char *sad_file_real_path(const char *path)
{
    if (!path)
        return NULL;
#ifdef _WIN32
    /* (AR) GetFullPathNameA يطبّع نصًّا فقط؛ الحلّ الفعليّ للروابط يحتاج فتحَ
     *      المقبض ثمّ GetFinalPathNameByHandleA — وهو ما يوازي realpath. */
    HANDLE h = CreateFileA(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (h == INVALID_HANDLE_VALUE)
        return NULL;
    char buf[32768];
    DWORD n = GetFinalPathNameByHandleA(h, buf, (DWORD)sizeof(buf), FILE_NAME_NORMALIZED);
    CloseHandle(h);
    if (n == 0 || n >= sizeof(buf))
        return NULL;
    /* (AR) أسقِط بادئة «\\?\» التي يضيفها ويندوز كي يطابق شكلُ المخرَج المفسّرَ. */
    const char *start = buf;
    if (n > 4 && buf[0] == '\\' && buf[1] == '\\' && buf[2] == '?' && buf[3] == '\\')
        start = buf + 4;
    size_t len = strlen(start);
    char *out = (char *)malloc(len + 1);
    if (!out)
        return NULL;
    memcpy(out, start, len + 1);
    return out;
#else
    return realpath(path, NULL); /* يُخصِّص، ويُرجع NULL عند الفشل */
#endif
}

/* ============================================================================
 * (AR) جسر «المسار_المطلق» — تطبيعٌ نصّيّ بلا حلِّ الروابط، فيعمل على مسارٍ
 *      غير موجود. لا يصلح وحده لفرض الاحتواء الأمنيّ (رابطٌ رمزيّ يخترقه) —
 *      استعمل sad_file_real_path لذلك.
 * (EN) «المسار_المطلق» bridge — textual normalisation without symlink resolution,
 *      so it works on a missing path. Not sound alone for security containment
 *      (a symlink defeats it) — use sad_file_real_path for that.
 * ============================================================================ */
char *sad_file_abs_path(const char *path)
{
    if (!path)
        return NULL;
#ifdef _WIN32
    char buf[32768];
    DWORD n = GetFullPathNameA(path, (DWORD)sizeof(buf), buf, NULL);
    if (n == 0 || n >= sizeof(buf))
        return NULL;
    size_t len = strlen(buf);
    char *out = (char *)malloc(len + 1);
    if (!out)
        return NULL;
    memcpy(out, buf, len + 1);
    return out;
#else
    /* (AR) POSIX بلا realpath (فهو يلزمه وجودُ المسار): ضمُّ مجلّدِ العمل يدويًّا. */
    if (path[0] == '/')
    {
        size_t len = strlen(path);
        char *out = (char *)malloc(len + 1);
        if (!out)
            return NULL;
        memcpy(out, path, len + 1);
        return out;
    }
    char cwd[4096];
    if (!getcwd(cwd, sizeof(cwd)))
        return NULL;
    size_t cl = strlen(cwd), pl = strlen(path);
    char *out = (char *)malloc(cl + pl + 2);
    if (!out)
        return NULL;
    memcpy(out, cwd, cl);
    out[cl] = '/';
    memcpy(out + cl + 1, path, pl + 1);
    return out;
#endif
}

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

/* استثناء غير معالَج خارج حاول/امسك — طباعة ثم خروج نظيف (لا longjmp بلا معالج مسجَّل) */
/* Unhandled exception outside try/catch — print then clean exit (no longjmp with no registered handler) */
void sad_report_unhandled_exception(const char *type, const char *msg)
{
    fprintf(stderr, "[sad] استثناء غير معالَج (%s): %s\n",
            type ? type : "خطأ", msg ? msg : "");
    exit(1);
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

/* (AR) رسائل خطأ دوال وحدة تشفير — ثوابت مسمّاة بدل نصوص حرفيّة مكرَّرة داخل
 *      أجسام الدوالّ أدناه (بعضها يتكرّر حرفيًّا داخل هذا الملف نفسه: رسالة
 *      فشل مصادقة AEAD ثلاث مرّات، ورسالتا فشل تخصيص ذاكرة التشفير/التوقيع
 *      الموثَّق مرّتين لكلٍّ). ملاحظة معماريّة: هذا الملف مصدر C مستقلّ يُكتَب
 *      إلى القرص ويُصرَّف بمُصرِّف خارجيّ (راجع sad_embedded_runtime_data.h في
 *      compiler_driver_linker.cpp) — لا رابط مصدريّ متاح مع security_builtins_ops.cpp
 *      (توليد LLVM IR، وحدة ترجمة مختلفة تمامًا)، فتطابق نصّ الرسالة بين
 *      الملفَّين يبقى تكرارًا يدويًّا حتى تُوحَّد الخوارزميّات ضمن مكتبة
 *      C مشتركة (راجع توصية النقل في تقرير مراجعة مكتبة التشفير).
 * (EN) Crypto builtin error messages — named constants instead of literals
 *      duplicated across the function bodies below (some repeat verbatim
 *      within this very file). Architectural note: this is a standalone C
 *      source written to disk and compiled by an external toolchain — no
 *      shared header is feasible with security_builtins_ops.cpp (a different
 *      translation unit entirely), so the message text matching between the
 *      two files remains manual duplication until the algorithms are unified
 *      into a shared C library. */
static const char *const kErrPbkdf2Iterations = "عدد تكرارات PBKDF2 يجب أن يكون أكبر من صفر";
static const char *const kErrHkdfLength = "طول ناتج HKDF يجب أن يكون بين 1 و8160 بايت";
static const char *const kErrX25519KeygenAlloc = "فشل تخصيص الذاكرة لتوليد مفتاح x25519";
static const char *const kErrX25519InvalidPriv = "مفتاح x25519 الخاصّ غير صالح (يجب 64 حرفًا ست عشريًّا)";
static const char *const kErrX25519DerivePubAlloc = "فشل تخصيص الذاكرة لاشتقاق مفتاح x25519";
static const char *const kErrX25519ExchangeInvalidInput = "مدخل تبادل_مفتاح غير صالح (كلا المفتاحين 64 حرفًا ست عشريًّا)";
static const char *const kErrX25519ZeroSharedSecret = "سرّ مشترك صفريّ مرفوض (مفتاح عامّ منخفض الرتبة — RFC 7748 §6.1)";
static const char *const kErrX25519ExchangeAlloc = "فشل تخصيص الذاكرة لتبادل_مفتاح";
static const char *const kErrEd25519KeygenAlloc = "فشل تخصيص الذاكرة لتوليد مفتاح توقيع";
static const char *const kErrEd25519InvalidSeed = "بذرة مفتاح التوقيع غير صالحة (يجب 64 حرفًا ست عشريًّا)";
static const char *const kErrEd25519DerivePubAlloc = "فشل تخصيص الذاكرة لاشتقاق مفتاح توقيع";
static const char *const kErrEd25519InvalidPriv = "مفتاح التوقيع الخاصّ غير صالح (يجب 64 حرفًا ست عشريًّا)";
static const char *const kErrEd25519SignAlloc = "فشل تخصيص الذاكرة للتوقيع";
static const char *const kErrAeadTextTooLong = "النص طويل جداً للتشفير الموثّق";
static const char *const kErrAeadEncryptAlloc = "فشل تخصيص الذاكرة للتشفير الموثّق";
static const char *const kErrAeadDecryptMissingKey = "فشل فكّ التشفير الموثّق — مفتاح مفقود";
static const char *const kErrAeadAuthFailed = "فشل المصادقة — المغلّف مُحرَّف أو المفتاح خاطئ";
static const char *const kErrAeadDecryptAlloc = "فشل تخصيص الذاكرة لفكّ التشفير الموثّق";
static const char *const kErrArgon2idMemoryCost = "تكلفة ذاكرة أرجون2 يجب أن تكون 8 كيلوبايت على الأقلّ";
static const char *const kErrArgon2idIterations = "عدد تكرارات أرجون2 يجب أن يكون أكبر من صفر";
static const char *const kErrArgon2idSaltLength = "ملح أرجون2 يجب أن يكون 8 بايت على الأقلّ";

/* اشتق_مفتاح_مرور / PBKDF2-HMAC-SHA256 — مفتاح 32 بايت ثابت، سلسلة ست عشريّة 64 حرفًا */
const char *sad_kdf_pbkdf2(const char *password, const char *salt, long long iterations)
{
    unsigned char digest[32];
    char *out;
    size_t i, pwlen, saltlen;
    unsigned long long iters;
    if (iterations <= 0)
    {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrPbkdf2Iterations);
        return NULL;
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
        return NULL;
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
        fprintf(stderr, "[sad] خطأ: %s\n", kErrHkdfLength);
        return NULL;
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
        return NULL;
    sad_hkdf_extract((const unsigned char *)salt, saltlen, (const unsigned char *)secret, secretlen, prk);
    sad_hkdf_expand(prk, (const unsigned char *)info, infolen, okm, l);
    out = (char *)malloc(l * 2 + 1);
    if (!out)
    {
        free(okm);
        return NULL;
    }
    for (i = 0; i < l; ++i)
        snprintf(out + i * 2, 3, "%02x", okm[i]);
    out[l * 2] = '\0';
    free(okm);
    return out;
}

/* ============================================================================
 * sadx_core — SHA-512 (FIPS 180-4) + X25519 (RFC 7748) + Ed25519 (RFC 8032)
 * تشفير غير متماثل ذاتيّ التنفيذ بالكامل (بلا OpenSSL/libsodium)، بلا __int128
 * (محمول 32/64 بت وعلى الوضع الحرّ). حساب المجال/المنحنى بتمثيل gf[16] بجذر 2^16،
 * وSHA-512 حسب FIPS 180-4. مطابق حرفيًّا (نفس النصّ) لنظير المفسّر
 * (interpreter/src/builtins/builtin_module_crypto.cpp، namespace CryptoDetail)
 * ونسخة رابط أندرويد (compiler_driver_android_linker.cpp). مُتحقَّق مقابل
 * FIPS 180-4 SHA-512 + RFC 7748 §5.2/§6.1 + RFC 8032 §7.1 (TEST 1/2/3) + أطوال
 * حدود كتلة SHA-512 (111/112/128) عبر برنامج مرجعيّ مستقلّ قبل الدمج.
 * ============================================================================ */
typedef long long sadx_gf[16];

static const unsigned long long SADX_K512[80] = {
0x428a2f98d728ae22ULL,0x7137449123ef65cdULL,0xb5c0fbcfec4d3b2fULL,0xe9b5dba58189dbbcULL,
0x3956c25bf348b538ULL,0x59f111f1b605d019ULL,0x923f82a4af194f9bULL,0xab1c5ed5da6d8118ULL,
0xd807aa98a3030242ULL,0x12835b0145706fbeULL,0x243185be4ee4b28cULL,0x550c7dc3d5ffb4e2ULL,
0x72be5d74f27b896fULL,0x80deb1fe3b1696b1ULL,0x9bdc06a725c71235ULL,0xc19bf174cf692694ULL,
0xe49b69c19ef14ad2ULL,0xefbe4786384f25e3ULL,0x0fc19dc68b8cd5b5ULL,0x240ca1cc77ac9c65ULL,
0x2de92c6f592b0275ULL,0x4a7484aa6ea6e483ULL,0x5cb0a9dcbd41fbd4ULL,0x76f988da831153b5ULL,
0x983e5152ee66dfabULL,0xa831c66d2db43210ULL,0xb00327c898fb213fULL,0xbf597fc7beef0ee4ULL,
0xc6e00bf33da88fc2ULL,0xd5a79147930aa725ULL,0x06ca6351e003826fULL,0x142929670a0e6e70ULL,
0x27b70a8546d22ffcULL,0x2e1b21385c26c926ULL,0x4d2c6dfc5ac42aedULL,0x53380d139d95b3dfULL,
0x650a73548baf63deULL,0x766a0abb3c77b2a8ULL,0x81c2c92e47edaee6ULL,0x92722c851482353bULL,
0xa2bfe8a14cf10364ULL,0xa81a664bbc423001ULL,0xc24b8b70d0f89791ULL,0xc76c51a30654be30ULL,
0xd192e819d6ef5218ULL,0xd69906245565a910ULL,0xf40e35855771202aULL,0x106aa07032bbd1b8ULL,
0x19a4c116b8d2d0c8ULL,0x1e376c085141ab53ULL,0x2748774cdf8eeb99ULL,0x34b0bcb5e19b48a8ULL,
0x391c0cb3c5c95a63ULL,0x4ed8aa4ae3418acbULL,0x5b9cca4f7763e373ULL,0x682e6ff3d6b2b8a3ULL,
0x748f82ee5defb2fcULL,0x78a5636f43172f60ULL,0x84c87814a1f0ab72ULL,0x8cc702081a6439ecULL,
0x90befffa23631e28ULL,0xa4506cebde82bde9ULL,0xbef9a3f7b2c67915ULL,0xc67178f2e372532bULL,
0xca273eceea26619cULL,0xd186b8c721c0c207ULL,0xeada7dd6cde0eb1eULL,0xf57d4f7fee6ed178ULL,
0x06f067aa72176fbaULL,0x0a637dc5a2c898a6ULL,0x113f9804bef90daeULL,0x1b710b35131c471bULL,
0x28db77f523047d84ULL,0x32caab7b40c72493ULL,0x3c9ebe0a15c9bebcULL,0x431d67c49c100d4cULL,
0x4cc5d4becb3e42b6ULL,0x597f299cfc657e2aULL,0x5fcb6fab3ad6faecULL,0x6c44198c4a475817ULL};

static unsigned long long sadx_rotr64(unsigned long long x, int n) { return (x >> n) | (x << (64 - n)); }
static unsigned long long sadx_load64be(const unsigned char *p) {
    return ((unsigned long long)p[0] << 56) | ((unsigned long long)p[1] << 48) |
           ((unsigned long long)p[2] << 40) | ((unsigned long long)p[3] << 32) |
           ((unsigned long long)p[4] << 24) | ((unsigned long long)p[5] << 16) |
           ((unsigned long long)p[6] << 8) | ((unsigned long long)p[7]);
}
static void sadx_store64be(unsigned char *p, unsigned long long v) {
    p[0] = (unsigned char)(v >> 56); p[1] = (unsigned char)(v >> 48);
    p[2] = (unsigned char)(v >> 40); p[3] = (unsigned char)(v >> 32);
    p[4] = (unsigned char)(v >> 24); p[5] = (unsigned char)(v >> 16);
    p[6] = (unsigned char)(v >> 8);  p[7] = (unsigned char)v;
}
static void sadx_sha512_compress(unsigned long long h[8], const unsigned char block[128]) {
    unsigned long long w[80], a, b, c, d, e, f, g, hh, t1, t2, s0, s1, ch, maj;
    int t;
    for (t = 0; t < 16; ++t) w[t] = sadx_load64be(block + t * 8);
    for (t = 16; t < 80; ++t) {
        unsigned long long x0 = w[t - 15], x1 = w[t - 2];
        s0 = sadx_rotr64(x0, 1) ^ sadx_rotr64(x0, 8) ^ (x0 >> 7);
        s1 = sadx_rotr64(x1, 19) ^ sadx_rotr64(x1, 61) ^ (x1 >> 6);
        w[t] = w[t - 16] + s0 + w[t - 7] + s1;
    }
    a = h[0]; b = h[1]; c = h[2]; d = h[3]; e = h[4]; f = h[5]; g = h[6]; hh = h[7];
    for (t = 0; t < 80; ++t) {
        s1 = sadx_rotr64(e, 14) ^ sadx_rotr64(e, 18) ^ sadx_rotr64(e, 41);
        ch = (e & f) ^ ((~e) & g);
        t1 = hh + s1 + ch + SADX_K512[t] + w[t];
        s0 = sadx_rotr64(a, 28) ^ sadx_rotr64(a, 34) ^ sadx_rotr64(a, 39);
        maj = (a & b) ^ (a & c) ^ (b & c);
        t2 = s0 + maj;
        hh = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
    }
    h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
}
static void sadx_sha512(const unsigned char *msg, size_t len, unsigned char out[64]) {
    unsigned long long h[8] = {
        0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
        0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL};
    unsigned char last[256];
    unsigned long long lenbits = (unsigned long long)len;
    unsigned long long bitlenhi = lenbits >> 61;
    unsigned long long bitlenlo = lenbits << 3;
    size_t i, off = 0, rem, padlen;
    while (len - off >= 128) { sadx_sha512_compress(h, msg + off); off += 128; }
    rem = len - off;
    for (i = 0; i < sizeof(last); ++i) last[i] = 0;
    for (i = 0; i < rem; ++i) last[i] = msg[off + i];
    last[rem] = 0x80;
    padlen = (rem < 112) ? 128 : 256;
    sadx_store64be(last + padlen - 16, bitlenhi);
    sadx_store64be(last + padlen - 8, bitlenlo);
    sadx_sha512_compress(h, last);
    if (padlen == 256) sadx_sha512_compress(h, last + 128);
    for (i = 0; i < 8; ++i) sadx_store64be(out + i * 8, h[i]);
}

static const sadx_gf
sadx_gf0 = {0},
sadx_gf1 = {1},
sadx_121665 = {0xDB41, 1},
sadx_D  = {0x78a3, 0x1359, 0x4dca, 0x75eb, 0xd8ab, 0x4141, 0x0a4d, 0x0070, 0xe898, 0x7779, 0x4079, 0x8cc7, 0xfe73, 0x2b6f, 0x6cee, 0x5203},
sadx_D2 = {0xf159, 0x26b2, 0x9b94, 0xebd6, 0xb156, 0x8283, 0x149a, 0x00e0, 0xd130, 0xeef3, 0x80f2, 0x198e, 0xfce7, 0x56df, 0xd9dc, 0x2406},
sadx_X  = {0xd51a, 0x8f25, 0x2d60, 0xc956, 0xa7b2, 0x9525, 0xc760, 0x692c, 0xdc5c, 0xfdd6, 0xe231, 0xc0a4, 0x53fe, 0xcd6e, 0x36d3, 0x2169},
sadx_Y  = {0x6658, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666},
sadx_I  = {0xa0b0, 0x4a0e, 0x1b27, 0xc4ee, 0xe478, 0xad2f, 0x1806, 0x2f43, 0xd7a7, 0x3dfb, 0x0099, 0x2b4d, 0xdf0b, 0x4fc1, 0x2480, 0x2b83};

static int sadx_vn(const unsigned char *x, const unsigned char *y, int n) {
    unsigned int i, d = 0;
    for (i = 0; i < (unsigned int)n; ++i) d |= x[i] ^ y[i];
    return (1 & ((d - 1) >> 8)) - 1;
}
static int sadx_verify32(const unsigned char *x, const unsigned char *y) { return sadx_vn(x, y, 32); }
static void sadx_set(sadx_gf r, const sadx_gf a) { int i; for (i = 0; i < 16; ++i) r[i] = a[i]; }
static void sadx_car(sadx_gf o) {
    int i; long long c;
    for (i = 0; i < 16; ++i) {
        o[i] += (1LL << 16);
        c = o[i] >> 16;
        o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);
        o[i] -= c << 16;
    }
}
static void sadx_sel(sadx_gf p, sadx_gf q, int b) {
    long long t, i, c = ~(b - 1);
    for (i = 0; i < 16; ++i) { t = c & (p[i] ^ q[i]); p[i] ^= t; q[i] ^= t; }
}
static void sadx_pack(unsigned char *o, const sadx_gf n) {
    int i, j, b;
    sadx_gf m, t;
    for (i = 0; i < 16; ++i) t[i] = n[i];
    sadx_car(t); sadx_car(t); sadx_car(t);
    for (j = 0; j < 2; ++j) {
        m[0] = t[0] - 0xffed;
        for (i = 1; i < 15; i++) { m[i] = t[i] - 0xffff - ((m[i - 1] >> 16) & 1); m[i - 1] &= 0xffff; }
        m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
        b = (m[15] >> 16) & 1;
        m[14] &= 0xffff;
        sadx_sel(t, m, 1 - b);
    }
    for (i = 0; i < 16; ++i) { o[2 * i] = t[i] & 0xff; o[2 * i + 1] = t[i] >> 8; }
}
static int sadx_neq(const sadx_gf a, const sadx_gf b) {
    unsigned char c[32], d[32];
    sadx_pack(c, a); sadx_pack(d, b);
    return sadx_verify32(c, d);
}
static unsigned char sadx_par(const sadx_gf a) { unsigned char d[32]; sadx_pack(d, a); return d[0] & 1; }
static void sadx_unpack(sadx_gf o, const unsigned char *n) {
    int i;
    for (i = 0; i < 16; ++i) o[i] = n[2 * i] + ((long long)n[2 * i + 1] << 8);
    o[15] &= 0x7fff;
}
static void sadx_add_fe(sadx_gf o, const sadx_gf a, const sadx_gf b) { int i; for (i = 0; i < 16; ++i) o[i] = a[i] + b[i]; }
static void sadx_sub_fe(sadx_gf o, const sadx_gf a, const sadx_gf b) { int i; for (i = 0; i < 16; ++i) o[i] = a[i] - b[i]; }
static void sadx_mul(sadx_gf o, const sadx_gf a, const sadx_gf b) {
    long long i, j, t[31];
    for (i = 0; i < 31; ++i) t[i] = 0;
    for (i = 0; i < 16; ++i) for (j = 0; j < 16; ++j) t[i + j] += a[i] * b[j];
    for (i = 0; i < 15; ++i) t[i] += 38 * t[i + 16];
    for (i = 0; i < 16; ++i) o[i] = t[i];
    sadx_car(o); sadx_car(o);
}
static void sadx_sqr(sadx_gf o, const sadx_gf a) { sadx_mul(o, a, a); }
static void sadx_inv(sadx_gf o, const sadx_gf i) {
    sadx_gf c; int a;
    for (a = 0; a < 16; ++a) c[a] = i[a];
    for (a = 253; a >= 0; a--) { sadx_sqr(c, c); if (a != 2 && a != 4) sadx_mul(c, c, i); }
    for (a = 0; a < 16; ++a) o[a] = c[a];
}
static void sadx_pow2523(sadx_gf o, const sadx_gf i) {
    sadx_gf c; int a;
    for (a = 0; a < 16; ++a) c[a] = i[a];
    for (a = 250; a >= 0; a--) { sadx_sqr(c, c); if (a != 1) sadx_mul(c, c, i); }
    for (a = 0; a < 16; ++a) o[a] = c[a];
}

/* X25519 (RFC 7748) — سلّم مونتغومري بزمن ثابت + تقييد سلميّ §5 */
static int sadx_x25519(unsigned char *q, const unsigned char *n, const unsigned char *p) {
    unsigned char z[32];
    long long x[80], r, i;
    sadx_gf a, b, c, d, e, f;
    for (i = 0; i < 31; ++i) z[i] = n[i];
    z[31] = (n[31] & 127) | 64;
    z[0] &= 248;
    sadx_unpack(x, p);
    for (i = 0; i < 16; ++i) { b[i] = x[i]; d[i] = a[i] = c[i] = 0; }
    a[0] = d[0] = 1;
    for (i = 254; i >= 0; --i) {
        r = (z[i >> 3] >> (i & 7)) & 1;
        sadx_sel(a, b, r); sadx_sel(c, d, r);
        sadx_add_fe(e, a, c); sadx_sub_fe(a, a, c);
        sadx_add_fe(c, b, d); sadx_sub_fe(b, b, d);
        sadx_sqr(d, e); sadx_sqr(f, a);
        sadx_mul(a, c, a); sadx_mul(c, b, e);
        sadx_add_fe(e, a, c); sadx_sub_fe(a, a, c);
        sadx_sqr(b, a); sadx_sub_fe(c, d, f);
        sadx_mul(a, c, sadx_121665); sadx_add_fe(a, a, d);
        sadx_mul(c, c, a); sadx_mul(a, d, f);
        sadx_mul(d, b, x); sadx_sqr(b, e);
        sadx_sel(a, b, r); sadx_sel(c, d, r);
    }
    for (i = 0; i < 16; ++i) { x[i + 16] = a[i]; x[i + 32] = c[i]; x[i + 48] = b[i]; x[i + 64] = d[i]; }
    sadx_inv(x + 32, x + 32);
    sadx_mul(x + 16, x + 16, x + 32);
    sadx_pack(q, x + 16);
    return 0;
}
static const unsigned char SADX_BASE9[32] = {9};
static int sadx_x25519_base(unsigned char *q, const unsigned char *n) { return sadx_x25519(q, n, SADX_BASE9); }

/* Ed25519 (RFC 8032) */
static void sadx_ed_add(sadx_gf p[4], sadx_gf q[4]) {
    sadx_gf a, b, c, d, t, e, f, g, h;
    sadx_sub_fe(a, p[1], p[0]); sadx_sub_fe(t, q[1], q[0]); sadx_mul(a, a, t);
    sadx_add_fe(b, p[0], p[1]); sadx_add_fe(t, q[0], q[1]); sadx_mul(b, b, t);
    sadx_mul(c, p[3], q[3]); sadx_mul(c, c, sadx_D2);
    sadx_mul(d, p[2], q[2]); sadx_add_fe(d, d, d);
    sadx_sub_fe(e, b, a); sadx_sub_fe(f, d, c); sadx_add_fe(g, d, c); sadx_add_fe(h, b, a);
    sadx_mul(p[0], e, f); sadx_mul(p[1], h, g); sadx_mul(p[2], g, f); sadx_mul(p[3], e, h);
}
static void sadx_ed_cswap(sadx_gf p[4], sadx_gf q[4], unsigned char b) { int i; for (i = 0; i < 4; ++i) sadx_sel(p[i], q[i], b); }
static void sadx_ed_pack(unsigned char *r, sadx_gf p[4]) {
    sadx_gf tx, ty, zi;
    sadx_inv(zi, p[2]);
    sadx_mul(tx, p[0], zi); sadx_mul(ty, p[1], zi);
    sadx_pack(r, ty);
    r[31] ^= sadx_par(tx) << 7;
}
static void sadx_ed_scalarmult(sadx_gf p[4], sadx_gf q[4], const unsigned char *s) {
    int i;
    sadx_set(p[0], sadx_gf0); sadx_set(p[1], sadx_gf1); sadx_set(p[2], sadx_gf1); sadx_set(p[3], sadx_gf0);
    for (i = 255; i >= 0; --i) {
        unsigned char b = (s[i / 8] >> (i & 7)) & 1;
        sadx_ed_cswap(p, q, b);
        sadx_ed_add(q, p);
        sadx_ed_add(p, p);
        sadx_ed_cswap(p, q, b);
    }
}
static void sadx_ed_scalarbase(sadx_gf p[4], const unsigned char *s) {
    sadx_gf q[4];
    sadx_set(q[0], sadx_X); sadx_set(q[1], sadx_Y); sadx_set(q[2], sadx_gf1); sadx_mul(q[3], sadx_X, sadx_Y);
    sadx_ed_scalarmult(p, q, s);
}
static const unsigned long long SADX_L[32] = {
    0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x10};
static void sadx_modL(unsigned char *r, long long x[64]) {
    long long carry, i, j;
    for (i = 63; i >= 32; --i) {
        carry = 0;
        for (j = i - 32; j < i - 12; ++j) {
            x[j] += carry - 16 * x[i] * (long long)SADX_L[j - (i - 32)];
            carry = (x[j] + 128) >> 8;
            x[j] -= carry << 8;
        }
        x[j] += carry;
        x[i] = 0;
    }
    carry = 0;
    for (j = 0; j < 32; ++j) { x[j] += carry - (x[31] >> 4) * (long long)SADX_L[j]; carry = x[j] >> 8; x[j] &= 255; }
    for (j = 0; j < 32; ++j) x[j] -= carry * (long long)SADX_L[j];
    for (i = 0; i < 32; ++i) { x[i + 1] += x[i] >> 8; r[i] = (unsigned char)(x[i] & 255); }
}
static void sadx_reduce(unsigned char *r) {
    long long x[64], i;
    for (i = 0; i < 64; ++i) x[i] = (unsigned long long)r[i];
    for (i = 0; i < 64; ++i) r[i] = 0;
    sadx_modL(r, x);
}
static void sadx_ed25519_pubkey(unsigned char pk[32], const unsigned char seed[32]) {
    unsigned char d[64];
    sadx_gf p[4];
    sadx_sha512(seed, 32, d);
    d[0] &= 248; d[31] &= 127; d[31] |= 64;
    sadx_ed_scalarbase(p, d);
    sadx_ed_pack(pk, p);
}
static void sadx_ed25519_sign(unsigned char sig[64], const unsigned char *m, size_t mlen,
                              const unsigned char seed[32], unsigned char *scratch) {
    unsigned char d[64], h[64], r[64], pk[32];
    long long i, j, x[64];
    sadx_gf p[4];
    sadx_sha512(seed, 32, d);
    d[0] &= 248; d[31] &= 127; d[31] |= 64;
    sadx_ed_scalarbase(p, d); sadx_ed_pack(pk, p);
    for (i = 0; i < 32; ++i) scratch[i] = d[32 + i];
    for (i = 0; (size_t)i < mlen; ++i) scratch[32 + i] = m[i];
    sadx_sha512(scratch, 32 + mlen, r);
    sadx_reduce(r);
    sadx_ed_scalarbase(p, r); sadx_ed_pack(sig, p);
    for (i = 0; i < 32; ++i) scratch[i] = sig[i];
    for (i = 0; i < 32; ++i) scratch[32 + i] = pk[i];
    for (i = 0; (size_t)i < mlen; ++i) scratch[64 + i] = m[i];
    sadx_sha512(scratch, 64 + mlen, h);
    sadx_reduce(h);
    for (i = 0; i < 64; ++i) x[i] = 0;
    for (i = 0; i < 32; ++i) x[i] = (unsigned long long)r[i];
    for (i = 0; i < 32; ++i) for (j = 0; j < 32; ++j) x[i + j] += (long long)h[i] * (long long)d[j];
    sadx_modL(sig + 32, x);
}
static int sadx_ed_unpackneg(sadx_gf r[4], const unsigned char p[32]) {
    sadx_gf t, chk, num, den, den2, den4, den6;
    sadx_set(r[2], sadx_gf1);
    sadx_unpack(r[1], p);
    sadx_sqr(num, r[1]); sadx_mul(den, num, sadx_D);
    sadx_sub_fe(num, num, r[2]); sadx_add_fe(den, r[2], den);
    sadx_sqr(den2, den); sadx_sqr(den4, den2); sadx_mul(den6, den4, den2);
    sadx_mul(t, den6, num); sadx_mul(t, t, den);
    sadx_pow2523(t, t);
    sadx_mul(t, t, num); sadx_mul(t, t, den); sadx_mul(t, t, den); sadx_mul(r[0], t, den);
    sadx_sqr(chk, r[0]); sadx_mul(chk, chk, den);
    if (sadx_neq(chk, num)) sadx_mul(r[0], r[0], sadx_I);
    sadx_sqr(chk, r[0]); sadx_mul(chk, chk, den);
    if (sadx_neq(chk, num)) return -1;
    if (sadx_par(r[0]) == (p[31] >> 7)) sadx_sub_fe(r[0], sadx_gf0, r[0]);
    sadx_mul(r[3], r[0], r[1]);
    return 0;
}
static int sadx_ed25519_verify(const unsigned char *m, size_t mlen, const unsigned char sig[64],
                               const unsigned char pk[32], unsigned char *scratch) {
    unsigned char t[32], h[64];
    sadx_gf p[4], q[4];
    long long i;
    int lt = 0, k;
    for (k = 31; k >= 0; --k) {
        unsigned char sb = sig[32 + k], lb = (unsigned char)SADX_L[k];
        if (sb < lb) { lt = 1; break; }
        if (sb > lb) { break; }
    }
    if (!lt) return 0;
    if (sadx_ed_unpackneg(q, pk)) return 0;
    for (i = 0; i < 32; ++i) scratch[i] = sig[i];
    for (i = 0; i < 32; ++i) scratch[32 + i] = pk[i];
    for (i = 0; (size_t)i < mlen; ++i) scratch[64 + i] = m[i];
    sadx_sha512(scratch, 64 + mlen, h);
    sadx_reduce(h);
    sadx_ed_scalarmult(p, q, h);
    sadx_ed_scalarbase(q, sig + 32);
    sadx_ed_add(p, q);
    sadx_ed_pack(t, p);
    return sadx_verify32(sig, t) == 0 ? 1 : 0;
}
static int sadx_is_all_zero(const unsigned char *b, size_t n) {
    unsigned char acc = 0; size_t i;
    for (i = 0; i < n; ++i) acc |= b[i];
    return acc == 0;
}

/* بايتات عشوائيّة (توليد مفاتيح): CSPRNG النظام على الأهداف المستضافة، مع تراجع
 * srand/rand غير الآمن فقط تحت SAD_FREESTANDING أو تعذّر مصدر النظام. */
static void sadx_random_bytes(unsigned char *buf, size_t len)
{
#ifndef SAD_FREESTANDING
    if (sad_crypto_os_random(buf, len))
        return;
#endif
    {
        static int seeded = 0;
        size_t i;
        if (!seeded) { srand((unsigned int)time(NULL)); seeded = 1; }
        for (i = 0; i < len; ++i) buf[i] = (unsigned char)(rand() & 0xFF);
    }
}
/* فكّ سلسلة ست عشريّة إلى بايتات: يُرجع 1 عند النجاح، 0 على طول خاطئ/حرف غير صالح */
static int sadx_hex2bin(const char *hex, unsigned char *out, size_t n)
{
    size_t i;
    if (strlen(hex) != n * 2) return 0;
    for (i = 0; i < n; ++i) {
        unsigned int byte = 0;
        char c0 = hex[2 * i], c1 = hex[2 * i + 1];
        if (!((c0 >= '0' && c0 <= '9') || (c0 >= 'a' && c0 <= 'f') || (c0 >= 'A' && c0 <= 'F')) ||
            !((c1 >= '0' && c1 <= '9') || (c1 >= 'a' && c1 <= 'f') || (c1 >= 'A' && c1 <= 'F')))
            return 0;
        if (sscanf(hex + 2 * i, "%02x", &byte) != 1) return 0;
        out[i] = (unsigned char)byte;
    }
    return 1;
}
/* ترميز بايتات إلى سلسلة ست عشريّة مخصَّصة (يجب على المتّصل تحريرها) */
static char *sadx_bin2hex(const unsigned char *b, size_t n)
{
    char *out = (char *)malloc(n * 2 + 1);
    size_t i;
    if (!out) return NULL;
    for (i = 0; i < n; ++i) snprintf(out + i * 2, 3, "%02x", b[i]);
    out[n * 2] = '\0';
    return out;
}

/* ولّد_مفتاح_خاص_x25519 / X25519 private key — 32 بايت عشوائيّة (ست عشريّ 64 حرفًا) */
const char *sad_security_x25519_keygen_priv(void)
{
    unsigned char sk[32];
    char *out;
    sadx_random_bytes(sk, 32);
    out = sadx_bin2hex(sk, 32);
    if (!out) { fprintf(stderr, "[sad] خطأ: %s\n", kErrX25519KeygenAlloc); return ""; }
    return out;
}
/* اشتق_مفتاح_عام_x25519 / X25519 derive public. مدخل غير صالح: stderr + سلسلة
 * فارغة (نفس نمط تباعد اشتق_مفتاح_مرور/اشتق_مفتاح — لا exit). */
const char *sad_security_x25519_derive_pub(const char *priv)
{
    unsigned char sk[32], pk[32];
    char *out;
    if (!priv) priv = "";
    if (!sadx_hex2bin(priv, sk, 32)) {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrX25519InvalidPriv);
        return NULL;
    }
    sadx_x25519_base(pk, sk);
    out = sadx_bin2hex(pk, 32);
    if (!out) { fprintf(stderr, "[sad] خطأ: %s\n", kErrX25519DerivePubAlloc); return NULL; }
    return out;
}
/* تبادل_مفتاح / X25519 exchange. مدخل غير صالح أو سرّ مشترك كلّه أصفار: stderr +
 * سلسلة فارغة. فحص السرّ الصفريّ إلزاميّ (RFC 7748 §6.1) — رفض المفاتيح منخفضة الرتبة. */
const char *sad_security_x25519_exchange(const char *priv, const char *peer_pub)
{
    unsigned char sk[32], pp[32], ss[32];
    char *out;
    if (!priv) priv = "";
    if (!peer_pub) peer_pub = "";
    if (!sadx_hex2bin(priv, sk, 32) || !sadx_hex2bin(peer_pub, pp, 32)) {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrX25519ExchangeInvalidInput);
        return NULL;
    }
    sadx_x25519(ss, sk, pp);
    if (sadx_is_all_zero(ss, 32)) {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrX25519ZeroSharedSecret);
        return NULL;
    }
    out = sadx_bin2hex(ss, 32);
    if (!out) { fprintf(stderr, "[sad] خطأ: %s\n", kErrX25519ExchangeAlloc); return NULL; }
    return out;
}
/* ولّد_مفتاح_خاص_توقيع / Ed25519 seed — 32 بايت عشوائيّة (ست عشريّ 64 حرفًا) */
const char *sad_security_ed25519_keygen_priv(void)
{
    unsigned char sk[32];
    char *out;
    sadx_random_bytes(sk, 32);
    out = sadx_bin2hex(sk, 32);
    if (!out) { fprintf(stderr, "[sad] خطأ: %s\n", kErrEd25519KeygenAlloc); return ""; }
    return out;
}
/* اشتق_مفتاح_عام_توقيع / Ed25519 derive public. مدخل غير صالح: stderr + فارغة. */
const char *sad_security_ed25519_derive_pub(const char *seed_hex)
{
    unsigned char seed[32], pk[32];
    char *out;
    if (!seed_hex) seed_hex = "";
    if (!sadx_hex2bin(seed_hex, seed, 32)) {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrEd25519InvalidSeed);
        return "";
    }
    sadx_ed25519_pubkey(pk, seed);
    out = sadx_bin2hex(pk, 32);
    if (!out) { fprintf(stderr, "[sad] خطأ: %s\n", kErrEd25519DerivePubAlloc); return ""; }
    return out;
}
/* وقّع / Ed25519 sign — توقيع 64 بايت (ست عشريّ 128 حرفًا). مفتاح غير صالح: stderr + فارغة. */
const char *sad_security_ed25519_sign(const char *msg, const char *seed_hex)
{
    unsigned char seed[32], sig[64];
    unsigned char *scratch;
    size_t mlen;
    char *out;
    if (!msg) msg = "";
    if (!seed_hex) seed_hex = "";
    if (!sadx_hex2bin(seed_hex, seed, 32)) {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrEd25519InvalidPriv);
        return "";
    }
    mlen = strlen(msg);
    scratch = (unsigned char *)malloc(mlen + 64);
    if (!scratch) { fprintf(stderr, "[sad] خطأ: %s\n", kErrEd25519SignAlloc); return ""; }
    sadx_ed25519_sign(sig, (const unsigned char *)msg, mlen, seed, scratch);
    free(scratch);
    out = sadx_bin2hex(sig, 64);
    if (!out) { fprintf(stderr, "[sad] خطأ: %s\n", kErrEd25519SignAlloc); return ""; }
    return out;
}
/* تحقق_توقيع / Ed25519 verify — دالّة استعلام: تُرجع 0/1، لا تُبلّغ عن خطأ ولا
 * تنهي البرنامج على توقيع فاسد أو مدخل مشوَّه (كلاهما 0). */
int sad_security_ed25519_verify(const char *msg, const char *sig_hex, const char *pub_hex)
{
    unsigned char sig[64], pk[32];
    unsigned char *scratch;
    size_t mlen;
    int ok;
    if (!msg) msg = "";
    if (!sig_hex) sig_hex = "";
    if (!pub_hex) pub_hex = "";
    if (!sadx_hex2bin(sig_hex, sig, 64)) return 0;
    if (!sadx_hex2bin(pub_hex, pk, 32)) return 0;
    mlen = strlen(msg);
    scratch = (unsigned char *)malloc(mlen + 64);
    if (!scratch) return 0;
    ok = sadx_ed25519_verify((const unsigned char *)msg, mlen, sig, pk, scratch);
    free(scratch);
    return ok;
}

/* ============================================================================
 * ChaCha20-Poly1305 AEAD (وحدة تشفير: شفّر_موثّق/فك_تشفير_موثّق) — تنفيذ
 * مرجعيّ محمول ذاتيّ (بلا OpenSSL/libsodium)، مطابق حرفيًّا لنظير المفسّر
 * (interpreter/src/builtins/builtin_module_crypto.cpp). Poly1305 بجذر 2^26
 * (خمس أطراف 26-بت) فيعمل بحساب 64-بت فقط — بلا __int128 — على 32/64 بت
 * وعلى الوضع الحرّ. مُتحقَّق حرفيًّا مقابل شعاعات RFC 8439 الرسميّة
 * (§2.3.2 كتلة، §2.4.2 تشفير، §2.5.2 Poly1305، §2.6.2 توليد مفتاح، §2.8.2
 * AEAD الكامل) قبل الدمج.
 * ============================================================================ */
static unsigned int sad_cc_rotl32(unsigned int x, int n) { return (x << n) | (x >> (32 - n)); }
static unsigned int sad_cc_load32le(const unsigned char *p)
{
    return (unsigned int)p[0] | ((unsigned int)p[1] << 8) |
           ((unsigned int)p[2] << 16) | ((unsigned int)p[3] << 24);
}
static void sad_cc_store32le(unsigned char *p, unsigned int v)
{
    p[0] = (unsigned char)v; p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16); p[3] = (unsigned char)(v >> 24);
}

#define SAD_CC_QR(a, b, c, d)                       \
    a += b; d ^= a; d = sad_cc_rotl32(d, 16);       \
    c += d; b ^= c; b = sad_cc_rotl32(b, 12);       \
    a += b; d ^= a; d = sad_cc_rotl32(d, 8);        \
    c += d; b ^= c; b = sad_cc_rotl32(b, 7);

/* كتلة ChaCha20 (RFC 8439 §2.3): مفتاح 32 بايت، nonce 12 بايت، خرج 64 بايت */
static void sad_chacha20_block(const unsigned char key[32], unsigned int counter,
                               const unsigned char nonce[12], unsigned char out[64])
{
    unsigned int s[16], x[16];
    int i;
    s[0] = 0x61707865u; s[1] = 0x3320646eu; s[2] = 0x79622d32u; s[3] = 0x6b206574u;
    for (i = 0; i < 8; ++i) s[4 + i] = sad_cc_load32le(key + i * 4);
    s[12] = counter;
    s[13] = sad_cc_load32le(nonce + 0);
    s[14] = sad_cc_load32le(nonce + 4);
    s[15] = sad_cc_load32le(nonce + 8);
    for (i = 0; i < 16; ++i) x[i] = s[i];
    for (i = 0; i < 10; ++i)
    {
        SAD_CC_QR(x[0], x[4], x[8], x[12]);
        SAD_CC_QR(x[1], x[5], x[9], x[13]);
        SAD_CC_QR(x[2], x[6], x[10], x[14]);
        SAD_CC_QR(x[3], x[7], x[11], x[15]);
        SAD_CC_QR(x[0], x[5], x[10], x[15]);
        SAD_CC_QR(x[1], x[6], x[11], x[12]);
        SAD_CC_QR(x[2], x[7], x[8], x[13]);
        SAD_CC_QR(x[3], x[4], x[9], x[14]);
    }
    for (i = 0; i < 16; ++i) sad_cc_store32le(out + i * 4, x[i] + s[i]);
}

/* تدفّق ChaCha20 XOR بعدّاد ابتدائيّ (RFC 8439 §2.4) */
static void sad_chacha20_xor(const unsigned char key[32], unsigned int counter,
                             const unsigned char nonce[12], const unsigned char *in,
                             size_t len, unsigned char *out)
{
    unsigned char ks[64];
    size_t off = 0;
    while (off < len)
    {
        size_t i, take = len - off;
        if (take > 64) take = 64;
        sad_chacha20_block(key, counter, nonce, ks);
        for (i = 0; i < take; ++i) out[off + i] = in[off + i] ^ ks[i];
        off += take;
        ++counter;
    }
}

/* Poly1305 MAC (RFC 8439 §2.5) — جذر 2^26، بلا __int128 (محمول 32/64 بت) */
static void sad_poly1305_mac(const unsigned char *msg, size_t len,
                             const unsigned char key[32], unsigned char tag[16])
{
    unsigned int r0, r1, r2, r3, r4;
    unsigned int s1, s2, s3, s4;
    unsigned int h0 = 0, h1 = 0, h2 = 0, h3 = 0, h4 = 0;
    unsigned int t0, t1, t2, t3;
    unsigned long long d0, d1, d2, d3, d4;
    unsigned int c;
    unsigned long long f;
    unsigned int g0, g1, g2, g3, g4;
    unsigned int mask;

    t0 = sad_cc_load32le(key + 0);
    t1 = sad_cc_load32le(key + 4);
    t2 = sad_cc_load32le(key + 8);
    t3 = sad_cc_load32le(key + 12);
    r0 = t0 & 0x3ffffff; t0 = (t0 >> 26) | (t1 << 6);
    r1 = t0 & 0x3ffff03; t1 = (t1 >> 20) | (t2 << 12);
    r2 = t1 & 0x3ffc0ff; t2 = (t2 >> 14) | (t3 << 18);
    r3 = t2 & 0x3f03fff; t3 = (t3 >> 8);
    r4 = t3 & 0x00fffff;

    s1 = r1 * 5; s2 = r2 * 5; s3 = r3 * 5; s4 = r4 * 5;

    while (len > 0)
    {
        unsigned char block[16];
        size_t i, n = len < 16 ? len : 16;
        unsigned int hibit;
        for (i = 0; i < n; ++i) block[i] = msg[i];
        if (n < 16)
        {
            block[n] = 1;
            for (i = n + 1; i < 16; ++i) block[i] = 0;
            hibit = 0;
        }
        else
        {
            hibit = (1u << 24);
        }
        t0 = sad_cc_load32le(block + 0);
        t1 = sad_cc_load32le(block + 4);
        t2 = sad_cc_load32le(block + 8);
        t3 = sad_cc_load32le(block + 12);
        h0 += t0 & 0x3ffffff;
        h1 += ((t0 >> 26) | (t1 << 6)) & 0x3ffffff;
        h2 += ((t1 >> 20) | (t2 << 12)) & 0x3ffffff;
        h3 += ((t2 >> 14) | (t3 << 18)) & 0x3ffffff;
        h4 += (t3 >> 8) | hibit;

        d0 = (unsigned long long)h0 * r0 + (unsigned long long)h1 * s4 + (unsigned long long)h2 * s3 + (unsigned long long)h3 * s2 + (unsigned long long)h4 * s1;
        d1 = (unsigned long long)h0 * r1 + (unsigned long long)h1 * r0 + (unsigned long long)h2 * s4 + (unsigned long long)h3 * s3 + (unsigned long long)h4 * s2;
        d2 = (unsigned long long)h0 * r2 + (unsigned long long)h1 * r1 + (unsigned long long)h2 * r0 + (unsigned long long)h3 * s4 + (unsigned long long)h4 * s3;
        d3 = (unsigned long long)h0 * r3 + (unsigned long long)h1 * r2 + (unsigned long long)h2 * r1 + (unsigned long long)h3 * r0 + (unsigned long long)h4 * s4;
        d4 = (unsigned long long)h0 * r4 + (unsigned long long)h1 * r3 + (unsigned long long)h2 * r2 + (unsigned long long)h3 * r1 + (unsigned long long)h4 * r0;

        c = (unsigned int)(d0 >> 26); h0 = (unsigned int)d0 & 0x3ffffff;
        d1 += c; c = (unsigned int)(d1 >> 26); h1 = (unsigned int)d1 & 0x3ffffff;
        d2 += c; c = (unsigned int)(d2 >> 26); h2 = (unsigned int)d2 & 0x3ffffff;
        d3 += c; c = (unsigned int)(d3 >> 26); h3 = (unsigned int)d3 & 0x3ffffff;
        d4 += c; c = (unsigned int)(d4 >> 26); h4 = (unsigned int)d4 & 0x3ffffff;
        h0 += c * 5; c = h0 >> 26; h0 &= 0x3ffffff;
        h1 += c;

        msg += n;
        len -= n;
    }

    c = h1 >> 26; h1 &= 0x3ffffff;
    h2 += c; c = h2 >> 26; h2 &= 0x3ffffff;
    h3 += c; c = h3 >> 26; h3 &= 0x3ffffff;
    h4 += c; c = h4 >> 26; h4 &= 0x3ffffff;
    h0 += c * 5; c = h0 >> 26; h0 &= 0x3ffffff;
    h1 += c;

    g0 = h0 + 5; c = g0 >> 26; g0 &= 0x3ffffff;
    g1 = h1 + c; c = g1 >> 26; g1 &= 0x3ffffff;
    g2 = h2 + c; c = g2 >> 26; g2 &= 0x3ffffff;
    g3 = h3 + c; c = g3 >> 26; g3 &= 0x3ffffff;
    g4 = h4 + c - (1u << 26);

    mask = (g4 >> 31) - 1;
    g0 &= mask; g1 &= mask; g2 &= mask; g3 &= mask; g4 &= mask;
    mask = ~mask;
    h0 = (h0 & mask) | g0;
    h1 = (h1 & mask) | g1;
    h2 = (h2 & mask) | g2;
    h3 = (h3 & mask) | g3;
    h4 = (h4 & mask) | g4;

    h0 = (h0) | (h1 << 26);
    h1 = (h1 >> 6) | (h2 << 20);
    h2 = (h2 >> 12) | (h3 << 14);
    h3 = (h3 >> 18) | (h4 << 8);

    f = (unsigned long long)h0 + sad_cc_load32le(key + 16); h0 = (unsigned int)f;
    f = (unsigned long long)h1 + sad_cc_load32le(key + 20) + (f >> 32); h1 = (unsigned int)f;
    f = (unsigned long long)h2 + sad_cc_load32le(key + 24) + (f >> 32); h2 = (unsigned int)f;
    f = (unsigned long long)h3 + sad_cc_load32le(key + 28) + (f >> 32); h3 = (unsigned int)f;

    sad_cc_store32le(tag + 0, h0);
    sad_cc_store32le(tag + 4, h1);
    sad_cc_store32le(tag + 8, h2);
    sad_cc_store32le(tag + 12, h3);
}

/* توليد مفتاح Poly1305 لمرّة واحدة (RFC 8439 §2.6): كتلة ChaCha20 بعدّاد 0 */
static void sad_poly1305_keygen(const unsigned char key[32], const unsigned char nonce[12],
                                unsigned char otk[32])
{
    unsigned char blk[64];
    sad_chacha20_block(key, 0, nonce, blk);
    memcpy(otk, blk, 32);
}

/* حساب وسم AEAD (RFC 8439 §2.8.1): mac على pad16(AAD)||pad16(ct)||le64(|AAD|)||le64(|ct|).
 * سطح لغة ص بلا AAD ⇒ aadlen يُمرَّر 0 دومًا (حالة AAD فارغة موثَّقة). */
static void sad_aead_compute_tag(const unsigned char otk[32],
                                 const unsigned char *aad, size_t aadlen,
                                 const unsigned char *ct, size_t ctlen, unsigned char tag[16])
{
    size_t apad = (16 - (aadlen % 16)) % 16;
    size_t cpad = (16 - (ctlen % 16)) % 16;
    size_t cap = aadlen + apad + ctlen + cpad + 16;
    unsigned char *buf = (unsigned char *)malloc(cap ? cap : 1);
    size_t mlen = 0;
    int i;
    unsigned long long a = (unsigned long long)aadlen, cl = (unsigned long long)ctlen;
    if (!buf) { memset(tag, 0, 16); return; }
    memcpy(buf, aad, aadlen); mlen += aadlen;
    memset(buf + mlen, 0, apad); mlen += apad;
    memcpy(buf + mlen, ct, ctlen); mlen += ctlen;
    memset(buf + mlen, 0, cpad); mlen += cpad;
    for (i = 0; i < 8; ++i) buf[mlen + i] = (unsigned char)(a >> (8 * i));
    mlen += 8;
    for (i = 0; i < 8; ++i) buf[mlen + i] = (unsigned char)(cl >> (8 * i));
    mlen += 8;
    sad_poly1305_mac(buf, mlen, otk, tag);
    free(buf);
}

/* مقارنة وسم بزمن ثابت (لا تُسرّب موضع أوّل اختلاف) */
static int sad_ct_equal(const unsigned char *a, const unsigned char *b, size_t n)
{
    unsigned char d = 0;
    size_t i;
    for (i = 0; i < n; ++i) d |= (unsigned char)(a[i] ^ b[i]);
    return d == 0;
}

/* مولّد بايتات عشوائيّة بأيّ طول — CSPRNG حقيقيّ على الأهداف المستضافة، يتراجع
 * لمسار srand/rand غير الآمن فقط تحت SAD_FREESTANDING أو عند تعذّر مصدر النظام
 * (يعمّم sad_crypto_random_u64 لأطوال كيفيّة، وله نفس بنية التراجع). */
static void sad_crypto_random_bytes(unsigned char *buf, size_t len)
{
#ifndef SAD_FREESTANDING
    if (sad_crypto_os_random(buf, len))
        return;
#endif
    {
        static int seeded = 0;
        size_t i;
        if (!seeded)
        {
            srand((unsigned int)time(NULL));
            seeded = 1;
        }
        for (i = 0; i < len; ++i)
            buf[i] = (unsigned char)(rand() & 0xFF);
    }
}

/* اشتقاق مفتاح 32 بايت: يُستعمَل المفتاح مباشرةً إن كان طوله 32 بايت بالضبط،
 * وإلّا يُشتقّ عبر SHA-256(المفتاح) — نفس نمط اشتقاق مفاتيح عائلة التشفير. */
static void sad_aead_key32(const char *key, unsigned char out[32])
{
    size_t klen = strlen(key);
    if (klen == 32)
        memcpy(out, key, 32);
    else
        sad_sha256_raw((const unsigned char *)key, klen, out);
}

/* شفّر_موثّق / AEAD encrypt — ChaCha20-Poly1305 (RFC 8439). المغلّف (ست عشريّ):
 * [nonce 12 بايت][النصّ المشفَّر][وسم Poly1305 16 بايت]. nonce عشوائيّ جديد لكل
 * استدعاء (⇒ مغلّفات مختلفة لنفس المدخل — خاصّيّة مقصودة). */
const char *sad_security_aead_encrypt(const char *text, const char *key)
{
    unsigned char key32[32], nonce[12], tag[16];
    unsigned char *ct;
    size_t tlen, i, envlen;
    char *result;

    if (!text) text = "";
    if (!key || !*key) return text;
    tlen = strlen(text);
    if (tlen > (SIZE_MAX / 2 - 64))
    {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrAeadTextTooLong);
        return text;
    }
    sad_aead_key32(key, key32);
    sad_crypto_random_bytes(nonce, 12);

    ct = (unsigned char *)malloc(tlen ? tlen : 1);
    if (!ct)
    {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrAeadEncryptAlloc);
        return text;
    }
    sad_chacha20_xor(key32, 1, nonce, (const unsigned char *)text, tlen, ct);
    {
        unsigned char otk[32];
        unsigned char aad_empty = 0; /* AAD طوله صفر (سطح ص بلا AAD) */
        sad_poly1305_keygen(key32, nonce, otk);
        sad_aead_compute_tag(otk, &aad_empty, 0, ct, tlen, tag);
    }

    envlen = 12 + tlen + 16;
    result = (char *)malloc(envlen * 2 + 1);
    if (!result)
    {
        free(ct);
        fprintf(stderr, "[sad] خطأ: %s\n", kErrAeadEncryptAlloc);
        return text;
    }
    for (i = 0; i < 12; ++i) snprintf(result + i * 2, 3, "%02x", nonce[i]);
    for (i = 0; i < tlen; ++i) snprintf(result + (12 + i) * 2, 3, "%02x", ct[i]);
    for (i = 0; i < 16; ++i) snprintf(result + (12 + tlen + i) * 2, 3, "%02x", tag[i]);
    result[envlen * 2] = '\0';
    free(ct);
    return result;
}

/* فك_تشفير_موثّق / AEAD decrypt — يتحقّق من الوسم ثمّ يفكّ. على فشل المصادقة أو
 * مغلّف مُشوَّه: يفشل مُغلَقًا (fail-closed) — رسالة stderr + NULL، لا يُرجع
 * قمامة أبدًا. المولّد اللغويّ (security_builtins_ops.cpp) يفحص NULL بعد النداء
 * ويرفع استثناءً قابلًا للالتقاط عبر __sad_raise (نفس آليّة عقود «يتطلب») —
 * فيتطابق سلوك المترجم الآن مع المفسّر (حاول/امسك يلتقط الفشل في الاثنين).
 * الرسالة عامّة عمدًا (لا تُسرّب الوسم/المفتاح). */
const char *sad_security_aead_decrypt(const char *hex, const char *key)
{
    unsigned char key32[32], nonce[12], tag[16], tag2[16];
    unsigned char *raw, *ct;
    size_t hlen, rlen, ctlen, i;
    char *result;

    if (!hex) hex = "";
    if (!key || !*key)
    {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrAeadDecryptMissingKey);
        return NULL;
    }
    hlen = strlen(hex);
    if (hlen % 2 != 0 || (hlen / 2) < (12 + 16))
    {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrAeadAuthFailed);
        return NULL;
    }
    rlen = hlen / 2;
    raw = (unsigned char *)malloc(rlen);
    if (!raw)
    {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrAeadDecryptAlloc);
        return NULL;
    }
    for (i = 0; i < rlen; ++i)
    {
        unsigned int byte = 0;
        if (sscanf(hex + i * 2, "%02x", &byte) != 1 || byte > 255)
        {
            free(raw);
            fprintf(stderr, "[sad] خطأ: %s\n", kErrAeadAuthFailed);
            return NULL;
        }
        raw[i] = (unsigned char)byte;
    }

    memcpy(nonce, raw, 12);
    memcpy(tag, raw + rlen - 16, 16);
    ct = raw + 12;
    ctlen = rlen - 12 - 16;

    sad_aead_key32(key, key32);
    {
        unsigned char otk[32];
        unsigned char aad_empty = 0; /* AAD طوله صفر (سطح ص بلا AAD) */
        sad_poly1305_keygen(key32, nonce, otk);
        sad_aead_compute_tag(otk, &aad_empty, 0, ct, ctlen, tag2);
    }
    if (!sad_ct_equal(tag, tag2, 16))
    {
        free(raw);
        fprintf(stderr, "[sad] خطأ: %s\n", kErrAeadAuthFailed);
        return NULL;
    }

    result = (char *)malloc(ctlen + 1);
    if (!result)
    {
        free(raw);
        fprintf(stderr, "[sad] خطأ: %s\n", kErrAeadDecryptAlloc);
        return NULL;
    }
    sad_chacha20_xor(key32, 1, nonce, ct, ctlen, (unsigned char *)result);
    result[ctlen] = '\0';
    free(raw);
    return result;
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
        fprintf(stderr, "[sad] خطأ: %s\n", kErrArgon2idMemoryCost);
        return NULL;
    }
    if (iterations <= 0)
    {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrArgon2idIterations);
        return NULL;
    }
    if (saltlen < 8)
    {
        fprintf(stderr, "[sad] خطأ: %s\n", kErrArgon2idSaltLength);
        return NULL;
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
        return NULL;

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
            return NULL;
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
