// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// compiler_driver_android_linker.cpp - ربط ملفات الكائن لأندرويد
// Android Object File Linking
// ═══════════════════════════════════════════════════════════════════════════
//
// الوصف (عربي):
// --------------
// هذا الملف يوفر دالة ربط ملفات الكائن (.o) لإنتاج ملفات أندرويد
// (مكتبات مشتركة .so أو ملفات تنفيذية).
//
// المحتويات:
//   - link_android_executable() — ربط ملف كائن باستخدام NDK clang
//     مع runtime مخصص لأندرويد يستخدم __android_log_print
//
// ملف android.cpp المرافق يحتوي على أوامر بناء المشروع:
//   - buildAndroidApp()          — إنشاء هيكل مشروع أندرويد كامل
//   - handleBuildAndroidCommand() — معالجة أمر "sadc build android"
//
// Description (English):
// ----------------------
// This file provides Android object file linking to produce .so libraries
// or executables using NDK clang with Android-specific runtime.
//
// Contents:
//   - link_android_executable() — links object file using NDK clang
//     with Android-specific runtime using __android_log_print
//
// The companion android.cpp file handles project scaffolding:
//   - buildAndroidApp()          — creates full Android project structure
//   - handleBuildAndroidCommand() — handles "sadc build android" command
//
// ═══════════════════════════════════════════════════════════════════════════

#include "compiler_driver.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace sad
{
    namespace driver
    {
        // ============================================================================
        // (AR) ربط ملف كائن لأندرويد باستخدام NDK clang
        // ============================================================================
        // هذه الدالة تقوم بالترجمة المتقاطعة لأندرويد:
        //   1. تبحث عن NDK clang (ليس clang العادي)
        //   2. تستخدم --target و --sysroot لأندرويد
        //   3. تولّد ملف runtime مخصص لأندرويد (يستخدم __android_log_print)
        //   4. تربط مع مكتبات أندرويد: -llog -landroid -lm -lc
        //   5. تنتج .so (مكتبة مشتركة) لتحميلها في NativeActivity
        // ============================================================================
        // (EN) Link object file for Android using NDK clang
        // Cross-compilation to Android ARM64 with proper sysroot and libraries.
        // ============================================================================

        bool CompilerDriver::link_android_executable(const std::string &obj_path,
                                                     const std::string &output_file,
                                                     llvm::Module *module)
        {
            // ================================================================
            // (AR) الخطوة 1: البحث عن NDK clang
            // (EN) Step 1: Find Android NDK clang
            // ================================================================
            auto ndk_clang_opt = find_android_ndk_clang();

            if (!ndk_clang_opt)
            {
                diagnostics_.report_fatal(
                    "لم يتم العثور على Android NDK clang\n"
                    "Android NDK clang not found\n"
                    "ثبّت Android NDK وعيّن ANDROID_NDK_HOME\n"
                    "Install Android NDK and set ANDROID_NDK_HOME\n"
                    "تحميل: https://developer.android.com/ndk/downloads");
                return false;
            }

            std::string ndk_clang = *ndk_clang_opt;
            std::string sysroot = get_android_sysroot(ndk_clang);
            std::string target_triple = options_.target.to_string();
            int api_level = options_.target.get_android_api_level();

            if (options_.verbose)
            {
                std::cerr << "\n  ══════════════════════════════════════════════\n";
                std::cerr << "  ربط لأندرويد / Linking for Android\n";
                std::cerr << "  الهدف / Target: " << target_triple << "\n";
                std::cerr << "  مستوى API / API Level: " << api_level << "\n";
                std::cerr << "  NDK clang: " << ndk_clang << "\n";
                std::cerr << "  Sysroot: " << sysroot << "\n";
                std::cerr << "  ══════════════════════════════════════════════\n";
            }

            // ================================================================
            // (AR) الخطوة 2: تحديد نوع الإخراج
            // (EN) Step 2: Determine output type
            // ================================================================
            // (AR) أندرويد يستخدم NativeActivity — الإخراج الافتراضي .so
            //      إلا إذا طلب المستخدم ملف تنفيذي صراحةً
            // (EN) Android uses NativeActivity — default output is .so
            //      unless user explicitly requested an executable
            bool output_shared = true;
            std::string actual_output = output_file;

            // (AR) إذا انتهى الاسم بـ .so فهو مكتبة مشتركة
            //      وإلا نضيف .so إذا لم ينتهِ بامتداد آخر
            if (output_file.size() >= 3 && output_file.substr(output_file.size() - 3) == ".so")
            {
                output_shared = true;
            }
            else if (output_file.find('.') == std::string::npos)
            {
                // (AR) لا امتداد — ربما يريد ملف تنفيذي (للاختبار على الجهاز عبر adb)
                output_shared = false;
            }

            // ================================================================
            // (AR) الخطوة 3: إنشاء ملف runtime لأندرويد
            // (EN) Step 3: Create Android-specific runtime file
            // ================================================================
            auto temp_runtime = get_temp_file(".c");
            temp_files_.push_back(temp_runtime);

            std::ofstream rt_file(temp_runtime);
            if (rt_file.is_open())
            {
                rt_file << R"(
/* ============================================================================
 * Sad Language - Android Runtime / مكتبة وقت التشغيل لأندرويد
 * ============================================================================
 * هذا الملف يُنشأ تلقائياً بواسطة مترجم Sad لتوفير الدوال
 * الأساسية اللازمة لتشغيل البرنامج على أندرويد.
 * ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __ANDROID__
  #include <android/log.h>
  #define SAD_LOG_TAG "SadLang"
  #define SAD_LOGI(...) __android_log_print(ANDROID_LOG_INFO, SAD_LOG_TAG, __VA_ARGS__)
  #define SAD_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, SAD_LOG_TAG, __VA_ARGS__)
#else
  #define SAD_LOGI(...) fprintf(stdout, __VA_ARGS__)
  #define SAD_LOGE(...) fprintf(stderr, __VA_ARGS__)
#endif

/* ============================================================================
 * دوال الإدخال / Input Functions (محدودة على أندرويد)
 * ============================================================================ */

const char* sad_llvm_input(void) {
    /* (AR) على أندرويد، الإدخال يأتي من واجهة المستخدم وليس stdin */
    static char buffer[4096];
    buffer[0] = '\0';
    #ifndef __ANDROID__
    if (fgets(buffer, sizeof(buffer), stdin)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        return buffer;
    }
    #endif
    SAD_LOGI("sad_llvm_input: تحذير - الإدخال غير متاح على أندرويد بدون واجهة\n");
    return "";
}

long long sad_llvm_input_int(void) {
    SAD_LOGI("sad_llvm_input_int: تحذير - الإدخال غير متاح على أندرويد بدون واجهة\n");
    return 0;
}

double sad_llvm_input_float(void) {
    SAD_LOGI("sad_llvm_input_float: تحذير - الإدخال غير متاح على أندرويد بدون واجهة\n");
    return 0.0;
}

/* ============================================================================
 * دوال الذاكرة / Memory Functions
 * ============================================================================ */

void* sad_llvm_alloc(unsigned long long size) {
    const unsigned long long MAX_ALLOC = 1ULL << 30;
    if (size == 0 || size > MAX_ALLOC) {
        SAD_LOGE("[sad] تحذير: طلب تخصيص غير صالح: %llu بايت\n", size);
        return NULL;
    }
    void* ptr = malloc((size_t)size);
    if (!ptr) {
        SAD_LOGE("[sad] خطأ: فشل تخصيص %llu بايت\n", size);
    }
    return ptr;
}

void sad_llvm_free(void* ptr) {
    if (ptr) free(ptr);
}

/* ============================================================================
 * دوال النصوص / String Functions
 * ============================================================================ */

void* sad_llvm_string_new(const char* data, unsigned long long length) {
    if (!data || length > (1ULL << 30)) return NULL;
    char* str = (char*)malloc((size_t)(length + 1));
    if (!str) return NULL;
    memcpy(str, data, (size_t)length);
    str[length] = '\0';
    return str;
}

void* sad_llvm_string_from_cstr(const char* cstr) {
    if (!cstr) return NULL;
    size_t len = strlen(cstr);
    char* str = (char*)malloc(len + 1);
    if (!str) return NULL;
    memcpy(str, cstr, len + 1);
    return str;
}

unsigned long long sad_llvm_string_length(void* str) {
    if (!str) return 0;
    return (unsigned long long)strlen((const char*)str);
}

void sad_llvm_print_string(void* str) {
    if (str) {
        #ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_INFO, "SadLang", "%s", (const char*)str);
        #else
        printf("%s", (const char*)str);
        #endif
    }
}

/* ============================================================================
 * دوال الأمان / Security Functions
 * ============================================================================ */

#include <time.h>

void sad_security_assert(int condition, const char* msg) {
    if (!condition) {
        SAD_LOGE("[ASSERT FAILED] %s\n", msg ? msg : "assertion failed");
        abort();
    }
}

void sad_security_panic(const char* msg) {
    SAD_LOGE("[PANIC] %s\n", msg ? msg : "panic");
    abort();
}

/* (AR) SHA-256 (FIPS 180-4) — يطابق حرفيًّا tools/compiler/runtime/sad_embedded_runtime.c
 *      والمفسّر (interpreter/src/builtins/builtin_module_assertions.cpp)، ليعيد
 *      «هاش» نصًّا ست عشريًّا متطابقًا عبر كل الأهداف بما فيها أندرويد. */
static unsigned int sad_sha256_rotr(unsigned int x, unsigned int n) {
    return (x >> n) | (x << (32 - n));
}

static void sad_sha256_raw(const unsigned char* data, size_t len, unsigned char out[32]) {
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
    unsigned char* padded = (unsigned char*)malloc(padded_len);
    size_t off;
    int i;
    if (!padded) {
        memset(out, 0, 32);
        return;
    }
    memset(padded, 0, padded_len);
    memcpy(padded, data, len);
    padded[len] = 0x80;
    for (i = 7; i >= 0; --i)
        padded[padded_len - 1 - i] = (unsigned char)(bitLen >> (i * 8));

    for (off = 0; off < padded_len; off += 64) {
        unsigned int w[64];
        unsigned int a, b, c, d, e, f, g, hh;
        for (i = 0; i < 16; ++i)
            w[i] = ((unsigned int)padded[off + i * 4] << 24) | ((unsigned int)padded[off + i * 4 + 1] << 16) |
                   ((unsigned int)padded[off + i * 4 + 2] << 8) | (unsigned int)padded[off + i * 4 + 3];
        for (i = 16; i < 64; ++i) {
            unsigned int s0 = sad_sha256_rotr(w[i - 15], 7) ^ sad_sha256_rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            unsigned int s1 = sad_sha256_rotr(w[i - 2], 17) ^ sad_sha256_rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }
        a = h0; b = h1; c = h2; d = h3; e = h4; f = h5; g = h6; hh = h7;
        for (i = 0; i < 64; ++i) {
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
        for (i = 0; i < 8; ++i) {
            out[i * 4] = (unsigned char)(hs[i] >> 24);
            out[i * 4 + 1] = (unsigned char)(hs[i] >> 16);
            out[i * 4 + 2] = (unsigned char)(hs[i] >> 8);
            out[i * 4 + 3] = (unsigned char)hs[i];
        }
    }
}

const char* sad_security_hash(const char* str) {
    unsigned char digest[32];
    char* out;
    size_t i;
    if (!str) str = "";
    sad_sha256_raw((const unsigned char*)str, strlen(str), digest);
    out = (char*)malloc(65);
    if (!out) return "";
    for (i = 0; i < 32; ++i)
        snprintf(out + i * 2, 3, "%02x", digest[i]);
    out[64] = '\0';
    return out;
}
)";
                // (AR) قسّمنا الحرف الخام إلى عدّة نداءات << — MSVC يرفض حرفًا خامًا
                //      واحدًا طويلًا جدًّا (C2026 "string too big") بعد إضافة BLAKE3؛
                //      الانقسام لا يغيّر محتوى الملف المكتوب (نفس البايتات بالتسلسل).
                // (EN) Split into multiple << calls — MSVC rejects one overly long
                //      raw string literal (C2026) after adding BLAKE3; splitting
                //      doesn't change the written file content (same bytes in order).
                rt_file << R"(
/* أندرويد نظام تشغيل مضيف دائمًا — نفس منطق BLAKE3 المطابق للمفسّر والمترجم
 * (sad_embedded_runtime.c)، بلا حاجة لتمييز حرّ (لا يوجد لأندرويد). */
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

long long sad_security_timestamp(void) {
    return (long long)time(NULL);
}

/* أندرويد نظام تشغيل مضيف دائمًا (لا وضع حرّ) — /dev/urandom متاح دون أذونات
 * خاصّة على كل إصدارات API، فلا حاجة لتحميل مكتبة NDK إضافيّة أو تمييز حرّ/مضيف
 * كما في sad_embedded_runtime.c. */
static int sad_crypto_os_random(unsigned char* buf, size_t len) {
    FILE* f = fopen("/dev/urandom", "rb");
    size_t got;
    if (!f) return 0;
    got = fread(buf, 1, len, f);
    fclose(f);
    return got == len;
}

long long sad_security_secure_random(long long min_val, long long max_val) {
    unsigned long long range;
    if (min_val >= max_val) return min_val;
    range = (unsigned long long)(max_val - min_val + 1);
    {
        unsigned char bytes[8];
        if (sad_crypto_os_random(bytes, sizeof(bytes))) {
            unsigned long long r = 0;
            int i;
            for (i = 0; i < 8; ++i) r = (r << 8) | bytes[i];
            return min_val + (long long)(r % range);
        }
    }
    {
        static int seeded = 0;
        if (!seeded) { srand((unsigned int)time(NULL)); seeded = 1; }
        return min_val + (long long)(rand() % (int)range);
    }
}

)";
                rt_file.close();

                if (options_.verbose)
                {
                    std::cerr << "  استخدام runtime أندرويد مؤقت: " << temp_runtime.string() << "\n";
                }
            }

            // ================================================================
            // (AR) الخطوة 4: بناء أمر الربط
            // (EN) Step 4: Build link command
            // ================================================================
            std::string command = "\"" + ndk_clang + "\"";

            // (AR) تحديد الهدف (target triple) ومسار sysroot
            command += " --target=" + target_triple;
            command += " --sysroot=\"" + sysroot + "\"";

            // (AR) إضافة ملف الكائن
            command += " \"" + obj_path + "\"";

            // (AR) إضافة ملف runtime الأندرويد
            command += " \"" + temp_runtime.string() + "\"";

            // (AR) تحديد ملف الإخراج
            command += " -o \"" + actual_output + "\"";

            // (AR) أعلام المكتبة المشتركة
            if (output_shared)
            {
                command += " -shared -fPIC";
                command += " -Wl,--build-id";     // معرف بناء لأندرويد
                command += " -Wl,--no-undefined"; // اكتشاف الرموز المفقودة
                command += " -Wl,-soname,lib" + std::filesystem::path(actual_output).stem().string() + ".so";
            }

            // (AR) مكتبات أندرويد الأساسية
            command += " -llog";     // __android_log_print
            command += " -landroid"; // NativeActivity, AAssetManager
            command += " -lm";       // دوال الرياضيات
            command += " -lc";       // مكتبة C القياسية
            command += " -ldl";      // dlopen/dlsym

            // (AR) تعطيل التحذيرات غير الضرورية
            command += " -w";

            // (AR) إضافة مسارات ومكتبات المستخدم
            for (const auto &path : options_.library_paths)
            {
                command += " -L\"" + path + "\"";
            }
            for (const auto &lib : options_.libraries)
            {
                command += " -l" + lib;
            }

            // (AR) الربط الثابت إذا طُلب
            if (options_.link_static)
            {
                command += " -static";
            }

            if (options_.verbose)
            {
                std::cerr << "\n  أمر ربط أندرويد / Android link command:\n  " << command << "\n\n";
            }

// ================================================================
// (AR) الخطوة 5: تنفيذ أمر الربط
// (EN) Step 5: Execute link command
// ================================================================
#ifdef _WIN32
            int result = std::system(("\"" + command + "\"").c_str());
#else
            int result = std::system(command.c_str());
#endif

            if (result != 0)
            {
                diagnostics_.report_fatal(
                    "فشل ربط ملف أندرويد (رمز الخطأ: " + std::to_string(result) + ")\n"
                                                                                  "Android linking failed (error code: " +
                    std::to_string(result) + ")\n"
                                             "تأكد من:\n"
                                             "  1. تثبيت Android NDK بشكل صحيح\n"
                                             "  2. تعيين ANDROID_NDK_HOME\n"
                                             "  3. أن الهدف " +
                    target_triple + " مدعوم في NDK\n"
                                    "Make sure:\n"
                                    "  1. Android NDK is properly installed\n"
                                    "  2. ANDROID_NDK_HOME is set\n"
                                    "  3. Target " +
                    target_triple + " is supported by NDK");
                return false;
            }

            // (AR) طباعة معلومات النجاح
            if (options_.verbose)
            {
                std::cerr << "  ✓ تم ربط ملف أندرويد بنجاح: " << actual_output << "\n";
                if (output_shared)
                {
                    std::cerr << "  نوع الإخراج: مكتبة مشتركة (.so)\n";
                    std::cerr << "  لاستخدامها: انسخها إلى مجلد jniLibs/arm64-v8a/ في مشروع أندرويد\n";
                }
                else
                {
                    std::cerr << "  نوع الإخراج: ملف تنفيذي\n";
                    std::cerr << "  للاختبار: adb push " << actual_output << " /data/local/tmp/\n";
                }
            }

            return true;
        }

    } // namespace driver
} // namespace sad
