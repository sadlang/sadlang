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
)";
                rt_file << R"(
/* ============================================================================
 * KDF — اشتق_مفتاح_مرور (PBKDF2-HMAC-SHA256, RFC 8018) و اشتق_مفتاح
 * (HKDF-SHA256, RFC 5869). مطابقة حرفيًّا لنظير sad_embedded_runtime.c.
 * ============================================================================ */

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
    for (i = 0; i < 64; ++i) {
        ipad[i] = (unsigned char)(k0[i] ^ 0x36);
        opad[i] = (unsigned char)(k0[i] ^ 0x5c);
    }
    inner_buf = (unsigned char *)malloc(64 + mlen);
    if (!inner_buf) { memset(out, 0, 32); return; }
    memcpy(inner_buf, ipad, 64);
    memcpy(inner_buf + 64, msg, mlen);
    sad_sha256_raw(inner_buf, 64 + mlen, inner_hash);
    free(inner_buf);
    memcpy(outer_buf, opad, 64);
    memcpy(outer_buf + 64, inner_hash, 32);
    sad_sha256_raw(outer_buf, 96, out);
}

static void sad_pbkdf2_hmac_sha256_raw(const unsigned char *pw, size_t pwlen,
                                        const unsigned char *salt, size_t saltlen,
                                        unsigned long long iterations, unsigned char *out, size_t dklen)
{
    unsigned int nblocks = (unsigned int)((dklen + 31) / 32);
    unsigned int blk;
    unsigned char *salt_ctr = (unsigned char *)malloc(saltlen + 4);
    if (!salt_ctr) { memset(out, 0, dklen); return; }
    memcpy(salt_ctr, salt, saltlen);
    for (blk = 1; blk <= nblocks; ++blk) {
        unsigned char u[32], t[32];
        unsigned long long iter;
        size_t off, take;
        salt_ctr[saltlen + 0] = (unsigned char)(blk >> 24);
        salt_ctr[saltlen + 1] = (unsigned char)(blk >> 16);
        salt_ctr[saltlen + 2] = (unsigned char)(blk >> 8);
        salt_ctr[saltlen + 3] = (unsigned char)(blk);
        sad_hmac_sha256_raw(pw, pwlen, salt_ctr, saltlen + 4, u);
        memcpy(t, u, 32);
        for (iter = 1; iter < iterations; ++iter) {
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

const char *sad_kdf_pbkdf2(const char *password, const char *salt, long long iterations)
{
    unsigned char digest[32];
    char *out;
    size_t i, pwlen, saltlen;
    unsigned long long iters;
    if (iterations <= 0) {
        fprintf(stderr, "[sad] خطأ: عدد تكرارات PBKDF2 يجب أن يكون أكبر من صفر\n");
        return "";
    }
    if (!password) password = "";
    if (!salt) salt = "";
    pwlen = strlen(password);
    saltlen = strlen(salt);
    iters = (unsigned long long)iterations;
    sad_pbkdf2_hmac_sha256_raw((const unsigned char *)password, pwlen,
                                (const unsigned char *)salt, saltlen, iters, digest, 32);
    out = (char *)malloc(65);
    if (!out) return "";
    for (i = 0; i < 32; ++i)
        snprintf(out + i * 2, 3, "%02x", digest[i]);
    out[64] = '\0';
    return out;
}

static void sad_hkdf_extract(const unsigned char *salt, size_t saltlen,
                              const unsigned char *ikm, size_t ikmlen, unsigned char prk[32])
{
    unsigned char zero_salt[32];
    if (saltlen == 0) {
        memset(zero_salt, 0, 32);
        sad_hmac_sha256_raw(zero_salt, 32, ikm, ikmlen, prk);
    } else {
        sad_hmac_sha256_raw(salt, saltlen, ikm, ikmlen, prk);
    }
}

static void sad_hkdf_expand(const unsigned char prk[32], const unsigned char *info, size_t infolen,
                             unsigned char *okm, size_t l)
{
    unsigned char t[32];
    size_t tlen = 0;
    unsigned char *buf = (unsigned char *)malloc(32 + infolen + 1);
    unsigned int i = 1;
    size_t produced = 0;
    if (!buf) { memset(okm, 0, l); return; }
    while (produced < l) {
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

const char *sad_kdf_hkdf(const char *secret, const char *salt, const char *info, long long length)
{
    unsigned char prk[32];
    unsigned char *okm;
    char *out;
    size_t i, secretlen, saltlen, infolen, l;
    /* RFC 5869 §2.3: الحدّ الأقصى للناتج L <= 255 * HashLen (32 بايت) = 8160 بايت */
    if (length <= 0 || length > 8160) {
        fprintf(stderr, "[sad] خطأ: طول ناتج HKDF يجب أن يكون بين 1 و8160 بايت\n");
        return "";
    }
    if (!secret) secret = "";
    if (!salt) salt = "";
    if (!info) info = "";
    secretlen = strlen(secret);
    saltlen = strlen(salt);
    infolen = strlen(info);
    l = (size_t)length;
    okm = (unsigned char *)malloc(l);
    if (!okm) return "";
    sad_hkdf_extract((const unsigned char *)salt, saltlen, (const unsigned char *)secret, secretlen, prk);
    sad_hkdf_expand(prk, (const unsigned char *)info, infolen, okm, l);
    out = (char *)malloc(l * 2 + 1);
    if (!out) { free(okm); return ""; }
    for (i = 0; i < l; ++i)
        snprintf(out + i * 2, 3, "%02x", okm[i]);
    out[l * 2] = '\0';
    free(okm);
    return out;
}
)";
                rt_file << R"(
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

typedef struct {
    unsigned long long h[8];
    unsigned long long t[2];
    unsigned char buf[128];
    size_t buflen;
    size_t outlen;
} sad_b2b_state;

static void sad_b2b_g(unsigned long long *v, int a, int b, int c, int d, unsigned long long x, unsigned long long y) {
    v[a] = v[a] + v[b] + x; v[d] = sad_rotr64(v[d] ^ v[a], 32);
    v[c] = v[c] + v[d];     v[b] = sad_rotr64(v[b] ^ v[c], 24);
    v[a] = v[a] + v[b] + y; v[d] = sad_rotr64(v[d] ^ v[a], 16);
    v[c] = v[c] + v[d];     v[b] = sad_rotr64(v[b] ^ v[c], 63);
}

static void sad_b2b_compress(sad_b2b_state *s, const unsigned char block[128], int last) {
    unsigned long long m[16], v[16];
    int i, j;
    for (i = 0; i < 16; ++i) {
        m[i] = 0;
        for (j = 0; j < 8; ++j)
            m[i] |= ((unsigned long long)block[i * 8 + j]) << (8 * j);
    }
    for (i = 0; i < 8; ++i) v[i] = s->h[i];
    for (i = 0; i < 8; ++i) v[8 + i] = SAD_B2B_IV[i];
    v[12] ^= s->t[0];
    v[13] ^= s->t[1];
    if (last) v[14] = ~v[14];
    for (i = 0; i < 12; ++i) {
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
    for (i = 0; i < 8; ++i) s->h[i] ^= v[i] ^ v[i + 8];
}

static void sad_b2b_init(sad_b2b_state *s, size_t outlen) {
    memset(s, 0, sizeof(*s));
    memcpy(s->h, SAD_B2B_IV, sizeof(s->h));
    s->h[0] ^= 0x01010000ULL ^ (unsigned long long)outlen;
    s->outlen = outlen;
}

static void sad_b2b_update(sad_b2b_state *s, const unsigned char *data, size_t len) {
    size_t i;
    for (i = 0; i < len; ++i) {
        if (s->buflen == 128) {
            s->t[0] += 128;
            if (s->t[0] < 128) s->t[1]++;
            sad_b2b_compress(s, s->buf, 0);
            s->buflen = 0;
        }
        s->buf[s->buflen++] = data[i];
    }
}

static void sad_b2b_final(sad_b2b_state *s, unsigned char *out) {
    size_t i;
    s->t[0] += s->buflen;
    if (s->t[0] < s->buflen) s->t[1]++;
    while (s->buflen < 128) s->buf[s->buflen++] = 0;
    sad_b2b_compress(s, s->buf, 1);
    for (i = 0; i < s->outlen; ++i)
        out[i] = (unsigned char)(s->h[i / 8] >> (8 * (i % 8)));
}

static void sad_blake2b(unsigned char *out, size_t outlen, const unsigned char *in, size_t inlen) {
    sad_b2b_state s;
    sad_b2b_init(&s, outlen);
    sad_b2b_update(&s, in, inlen);
    sad_b2b_final(&s, out);
}

static void sad_argon2_hprime(unsigned char *out, size_t outlen, const unsigned char *in, size_t inlen) {
    unsigned char le_len[4];
    le_len[0] = (unsigned char)outlen; le_len[1] = (unsigned char)(outlen >> 8);
    le_len[2] = (unsigned char)(outlen >> 16); le_len[3] = (unsigned char)(outlen >> 24);
    if (outlen <= 64) {
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
        while (outlen - produced > 64) {
            sad_blake2b(v, 64, v, 64);
            memcpy(out + produced, v, 32);
            produced += 32;
        }
        sad_blake2b(v, outlen - produced, v, 64);
        memcpy(out + produced, v, outlen - produced);
    }
}
)";
                rt_file << R"(
#define SAD_ARGON2_QWORDS_IN_BLOCK 128
#define SAD_ARGON2_BLOCK_SIZE (SAD_ARGON2_QWORDS_IN_BLOCK * 8)
#define SAD_ARGON2_SYNC_POINTS 4

typedef struct { unsigned long long v[SAD_ARGON2_QWORDS_IN_BLOCK]; } sad_argon2_block_t;

static void sad_argon2_block_xor(sad_argon2_block_t *dst, const sad_argon2_block_t *a, const sad_argon2_block_t *b) {
    int i;
    for (i = 0; i < SAD_ARGON2_QWORDS_IN_BLOCK; ++i) dst->v[i] = a->v[i] ^ b->v[i];
}

static void sad_argon2_p(unsigned long long *v) {
    int i;
    static const int perm[8][4] = {
        {0, 4, 8, 12}, {1, 5, 9, 13}, {2, 6, 10, 14}, {3, 7, 11, 15}, {0, 5, 10, 15}, {1, 6, 11, 12}, {2, 7, 8, 13}, {3, 4, 9, 14}};
    for (i = 0; i < 8; ++i) {
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

static void sad_argon2_fill_block(const sad_argon2_block_t *x, const sad_argon2_block_t *y, sad_argon2_block_t *out,
                                   int with_xor, const sad_argon2_block_t *prev_out) {
    sad_argon2_block_t r, q, newval;
    int i, m;
    sad_argon2_block_xor(&r, x, y);
    q = r;
    for (i = 0; i < 8; ++i) sad_argon2_p(&q.v[16 * i]);
    for (i = 0; i < 8; ++i) {
        unsigned long long col[16];
        for (m = 0; m < 8; ++m) {
            col[2 * m] = q.v[2 * i + 16 * m];
            col[2 * m + 1] = q.v[2 * i + 16 * m + 1];
        }
        sad_argon2_p(col);
        for (m = 0; m < 8; ++m) {
            q.v[2 * i + 16 * m] = col[2 * m];
            q.v[2 * i + 16 * m + 1] = col[2 * m + 1];
        }
    }
    for (i = 0; i < SAD_ARGON2_QWORDS_IN_BLOCK; ++i) newval.v[i] = q.v[i] ^ r.v[i];
    if (with_xor)
        for (i = 0; i < SAD_ARGON2_QWORDS_IN_BLOCK; ++i) newval.v[i] ^= prev_out->v[i];
    *out = newval;
}

typedef struct { unsigned long long pass, lane, slice, m_prime, t_prime, type, counter; } sad_argon2_addr_input_t;

static void sad_argon2_gen_addr_block(sad_argon2_block_t *addr, const sad_argon2_addr_input_t *ai) {
    sad_argon2_block_t zero, in, tmp;
    memset(&zero, 0, sizeof(sad_argon2_block_t));
    memset(&in, 0, sizeof(sad_argon2_block_t));
    in.v[0] = ai->pass; in.v[1] = ai->lane; in.v[2] = ai->slice;
    in.v[3] = ai->m_prime; in.v[4] = ai->t_prime; in.v[5] = ai->type; in.v[6] = ai->counter;
    sad_argon2_fill_block(&zero, &in, &tmp, 0, NULL);
    sad_argon2_fill_block(&zero, &tmp, addr, 0, NULL);
}

static unsigned int sad_argon2_index_alpha(unsigned int pass, unsigned int slice, unsigned int seg_len, unsigned int index,
                                            unsigned long long rand64, unsigned int lane_len) {
    unsigned long long reference_area_size;
    unsigned long long rel;
    unsigned int start_position;
    if (pass == 0) {
        if (slice == 0) reference_area_size = index - 1;
        else reference_area_size = (unsigned long long)slice * seg_len + index - 1;
    } else {
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

const char *sad_kdf_argon2id(const char *password, const char *salt, long long memory_cost_kib, long long iterations) {
    unsigned int m_cost, t_cost;
    size_t pwlen, saltlen;
    unsigned int lanes = 1;
    unsigned int m_prime, lane_len, seg_len;
    unsigned char h0[64];
    sad_argon2_block_t *B;
    unsigned int pass, slice;
    char *out;
    size_t i;

    if (!password) password = "";
    if (!salt) salt = "";
    pwlen = strlen(password);
    saltlen = strlen(salt);

    if (memory_cost_kib < 8) {
        fprintf(stderr, "[sad] خطأ: تكلفة ذاكرة أرجون2 يجب أن تكون 8 كيلوبايت على الأقلّ\n");
        return "";
    }
    if (iterations <= 0) {
        fprintf(stderr, "[sad] خطأ: عدد تكرارات أرجون2 يجب أن يكون أكبر من صفر\n");
        return "";
    }
    if (saltlen < 8) {
        fprintf(stderr, "[sad] خطأ: ملح أرجون2 يجب أن يكون 8 بايت على الأقلّ\n");
        return "";
    }

    m_cost = (unsigned int)memory_cost_kib;
    t_cost = (unsigned int)iterations;
    m_prime = (m_cost / 4) * 4;
    if (m_prime < 2 * SAD_ARGON2_SYNC_POINTS * lanes) m_prime = 2 * SAD_ARGON2_SYNC_POINTS * lanes;
    lane_len = m_prime / lanes;
    seg_len = lane_len / SAD_ARGON2_SYNC_POINTS;

    {
        unsigned char buf[4];
        sad_b2b_state s;
        sad_b2b_init(&s, 64);
#define SAD_ARGON2_PUT32(val) \
    buf[0] = (unsigned char)(val); buf[1] = (unsigned char)((val) >> 8); \
    buf[2] = (unsigned char)((val) >> 16); buf[3] = (unsigned char)((val) >> 24); \
    sad_b2b_update(&s, buf, 4)
        SAD_ARGON2_PUT32(lanes);
        SAD_ARGON2_PUT32(32u);
        SAD_ARGON2_PUT32(m_cost); /* RFC 9106 §3.2 H0 uses the raw memory cost, not the rounded-down m' */
        SAD_ARGON2_PUT32(t_cost);
        SAD_ARGON2_PUT32(0x13u);
        SAD_ARGON2_PUT32(2u);
        SAD_ARGON2_PUT32((unsigned int)pwlen);
        sad_b2b_update(&s, (const unsigned char *)password, pwlen);
        SAD_ARGON2_PUT32((unsigned int)saltlen);
        sad_b2b_update(&s, (const unsigned char *)salt, saltlen);
        SAD_ARGON2_PUT32(0u);
        SAD_ARGON2_PUT32(0u);
        sad_b2b_final(&s, h0);
#undef SAD_ARGON2_PUT32
    }

    B = (sad_argon2_block_t *)malloc((size_t)m_prime * sizeof(sad_argon2_block_t));
    if (!B) return "";

    {
        unsigned char seed[72];
        memcpy(seed, h0, 64);
        memset(seed + 64, 0, 8);
        sad_argon2_hprime((unsigned char *)&B[0], SAD_ARGON2_BLOCK_SIZE, seed, 72);
        seed[64] = 1;
        sad_argon2_hprime((unsigned char *)&B[1], SAD_ARGON2_BLOCK_SIZE, seed, 72);
    }

    for (pass = 0; pass < t_cost; ++pass) {
        for (slice = 0; slice < SAD_ARGON2_SYNC_POINTS; ++slice) {
            int data_independent = (pass == 0 && slice < SAD_ARGON2_SYNC_POINTS / 2) ? 1 : 0;
            sad_argon2_block_t addr_block;
            sad_argon2_addr_input_t ai;
            unsigned int start_index = (pass == 0 && slice == 0) ? 2 : 0;
            unsigned int j;
            ai.pass = pass; ai.lane = 0; ai.slice = slice;
            ai.m_prime = m_prime; ai.t_prime = t_cost; ai.type = 2; ai.counter = 0;

            for (j = 0; j < seg_len; ++j) {
                unsigned int pos_in_lane, curr_index, prev_index;
                unsigned long long rand64;
                unsigned int ref_index;
                int with_xor;

                if (data_independent && j % SAD_ARGON2_QWORDS_IN_BLOCK == 0) {
                    ai.counter++;
                    sad_argon2_gen_addr_block(&addr_block, &ai);
                }
                if (j < start_index) continue;

                pos_in_lane = slice * seg_len + j;
                curr_index = pos_in_lane;
                prev_index = (pos_in_lane == 0) ? (lane_len - 1) : (curr_index - 1);

                if (data_independent) rand64 = addr_block.v[j % SAD_ARGON2_QWORDS_IN_BLOCK];
                else rand64 = B[prev_index].v[0];

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
        if (!out) return "";
        for (i = 0; i < 32; ++i) snprintf(out + i * 2, 3, "%02x", tag[i]);
        out[64] = '\0';
        return out;
    }
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
                // X25519 (RFC 7748) + Ed25519 (RFC 8032) + SHA-512 (FIPS 180-4) —
                // third identical-logic copy of sadx_core (mirrors interpreter's
                // CryptoDetail and sad_embedded_runtime.c). Split into two <16KB raw
                // strings to avoid MSVC C2026 (raw-string-literal size limit).
                // Segment A: SHA-512 + field arithmetic + X25519.
                rt_file << R"(
#include <stdint.h>
/* sadx_core (part A): SHA-512 + Curve25519 field arithmetic (gf[16], radix 2^16,
 * no __int128) + X25519 scalar multiplication. Byte-identical in logic to
 * interpreter/src/builtins/builtin_module_crypto.cpp and
 * tools/compiler/runtime/sad_embedded_runtime.c. Verified against FIPS 180-4,
 * RFC 7748 §5.2/§6.1, RFC 8032 §7.1. */
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
)";
                // Segment B: Ed25519 (sign/verify/pubkey) + hex/random helpers + wrappers.
                rt_file << R"(
/* sadx_core (part B): Ed25519 point ops (extended coords), sign (RFC 8032
 * §5.1.6), verify (§5.1.7, incl. non-canonical S>=L rejection and §5.1.3
 * decompression), scalar reduction mod L, and the seven string-in/out builtin
 * wrappers. */
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
static void sadx_random_bytes(unsigned char *buf, size_t len) {
    if (sad_crypto_os_random(buf, len)) return;
    {
        static int seeded = 0;
        size_t i;
        if (!seeded) { srand((unsigned int)time(NULL)); seeded = 1; }
        for (i = 0; i < len; ++i) buf[i] = (unsigned char)(rand() & 0xFF);
    }
}
static int sadx_hex2bin(const char *hex, unsigned char *out, size_t n) {
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
static char *sadx_bin2hex(const unsigned char *b, size_t n) {
    char *out = (char *)malloc(n * 2 + 1);
    size_t i;
    if (!out) return NULL;
    for (i = 0; i < n; ++i) snprintf(out + i * 2, 3, "%02x", b[i]);
    out[n * 2] = '\0';
    return out;
}
const char *sad_security_x25519_keygen_priv(void) {
    unsigned char sk[32]; char *out;
    sadx_random_bytes(sk, 32);
    out = sadx_bin2hex(sk, 32);
    if (!out) { fprintf(stderr, "[sad] error: x25519 keygen allocation failed\n"); return ""; }
    return out;
}
const char *sad_security_x25519_derive_pub(const char *priv) {
    unsigned char sk[32], pk[32]; char *out;
    if (!priv) priv = "";
    if (!sadx_hex2bin(priv, sk, 32)) { fprintf(stderr, "[sad] error: invalid x25519 private key (64 hex chars)\n"); return ""; }
    sadx_x25519_base(pk, sk);
    out = sadx_bin2hex(pk, 32);
    if (!out) { fprintf(stderr, "[sad] error: x25519 derive allocation failed\n"); return ""; }
    return out;
}
const char *sad_security_x25519_exchange(const char *priv, const char *peer_pub) {
    unsigned char sk[32], pp[32], ss[32]; char *out;
    if (!priv) priv = "";
    if (!peer_pub) peer_pub = "";
    if (!sadx_hex2bin(priv, sk, 32) || !sadx_hex2bin(peer_pub, pp, 32)) { fprintf(stderr, "[sad] error: invalid key-exchange input (both keys 64 hex chars)\n"); return ""; }
    sadx_x25519(ss, sk, pp);
    if (sadx_is_all_zero(ss, 32)) { fprintf(stderr, "[sad] error: all-zero shared secret rejected (low-order public key, RFC 7748 6.1)\n"); return ""; }
    out = sadx_bin2hex(ss, 32);
    if (!out) { fprintf(stderr, "[sad] error: key-exchange allocation failed\n"); return ""; }
    return out;
}
const char *sad_security_ed25519_keygen_priv(void) {
    unsigned char sk[32]; char *out;
    sadx_random_bytes(sk, 32);
    out = sadx_bin2hex(sk, 32);
    if (!out) { fprintf(stderr, "[sad] error: ed25519 keygen allocation failed\n"); return ""; }
    return out;
}
const char *sad_security_ed25519_derive_pub(const char *seed_hex) {
    unsigned char seed[32], pk[32]; char *out;
    if (!seed_hex) seed_hex = "";
    if (!sadx_hex2bin(seed_hex, seed, 32)) { fprintf(stderr, "[sad] error: invalid signing seed (64 hex chars)\n"); return ""; }
    sadx_ed25519_pubkey(pk, seed);
    out = sadx_bin2hex(pk, 32);
    if (!out) { fprintf(stderr, "[sad] error: ed25519 derive allocation failed\n"); return ""; }
    return out;
}
const char *sad_security_ed25519_sign(const char *msg, const char *seed_hex) {
    unsigned char seed[32], sig[64]; unsigned char *scratch; size_t mlen; char *out;
    if (!msg) msg = "";
    if (!seed_hex) seed_hex = "";
    if (!sadx_hex2bin(seed_hex, seed, 32)) { fprintf(stderr, "[sad] error: invalid signing private key (64 hex chars)\n"); return ""; }
    mlen = strlen(msg);
    scratch = (unsigned char *)malloc(mlen + 64);
    if (!scratch) { fprintf(stderr, "[sad] error: sign allocation failed\n"); return ""; }
    sadx_ed25519_sign(sig, (const unsigned char *)msg, mlen, seed, scratch);
    free(scratch);
    out = sadx_bin2hex(sig, 64);
    if (!out) { fprintf(stderr, "[sad] error: sign allocation failed\n"); return ""; }
    return out;
}
int sad_security_ed25519_verify(const char *msg, const char *sig_hex, const char *pub_hex) {
    unsigned char sig[64], pk[32]; unsigned char *scratch; size_t mlen; int ok;
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
                // ChaCha20-Poly1305 AEAD (شفّر_موثّق/فك_تشفير_موثّق) — نسخة ثالثة
                // مطابقة حرفيًّا لنظير sad_embedded_runtime.c، مقسّمة في سلسلة خام
                // مستقلّة (<16KB لتفادي حدّ MSVC C2026 لحرفيّات السلاسل).
                rt_file << R"(
#include <stdint.h>
/* ChaCha20-Poly1305 AEAD (RFC 8439) - self-implemented, byte-identical in logic
 * to tools/compiler/runtime/sad_embedded_runtime.c. Poly1305 radix 2^26 (no
 * __int128). Verified against RFC 8439 vectors 2.3.2/2.4.2/2.5.2/2.6.2/2.8.2. */
static unsigned int sad_cc_rotl32(unsigned int x, int n) { return (x << n) | (x >> (32 - n)); }
static unsigned int sad_cc_load32le(const unsigned char *p) {
    return (unsigned int)p[0] | ((unsigned int)p[1] << 8) |
           ((unsigned int)p[2] << 16) | ((unsigned int)p[3] << 24);
}
)";
                // (AR) تقسيم السلسلة الخام: تجاوز حدّ MSVC C2026 (65535 بايت للـ
                //      literal الواحد) بعد نموّ runtime التشفير المضمَّن (ChaCha/Poly/AEAD).
                rt_file << R"(static void sad_cc_store32le(unsigned char *p, unsigned int v) {
    p[0] = (unsigned char)v; p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16); p[3] = (unsigned char)(v >> 24);
}
#define SAD_CC_QR(a, b, c, d)                       \
    a += b; d ^= a; d = sad_cc_rotl32(d, 16);       \
    c += d; b ^= c; b = sad_cc_rotl32(b, 12);       \
    a += b; d ^= a; d = sad_cc_rotl32(d, 8);        \
    c += d; b ^= c; b = sad_cc_rotl32(b, 7);
static void sad_chacha20_block(const unsigned char key[32], unsigned int counter,
                               const unsigned char nonce[12], unsigned char out[64]) {
    unsigned int s[16], x[16];
    int i;
    s[0] = 0x61707865u; s[1] = 0x3320646eu; s[2] = 0x79622d32u; s[3] = 0x6b206574u;
    for (i = 0; i < 8; ++i) s[4 + i] = sad_cc_load32le(key + i * 4);
    s[12] = counter;
    s[13] = sad_cc_load32le(nonce + 0);
    s[14] = sad_cc_load32le(nonce + 4);
    s[15] = sad_cc_load32le(nonce + 8);
    for (i = 0; i < 16; ++i) x[i] = s[i];
    for (i = 0; i < 10; ++i) {
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
static void sad_chacha20_xor(const unsigned char key[32], unsigned int counter,
                             const unsigned char nonce[12], const unsigned char *in,
                             size_t len, unsigned char *out) {
    unsigned char ks[64];
    size_t off = 0;
    while (off < len) {
        size_t i, take = len - off;
        if (take > 64) take = 64;
        sad_chacha20_block(key, counter, nonce, ks);
        for (i = 0; i < take; ++i) out[off + i] = in[off + i] ^ ks[i];
        off += take;
        ++counter;
    }
}
static void sad_poly1305_mac(const unsigned char *msg, size_t len,
                             const unsigned char key[32], unsigned char tag[16]) {
    unsigned int r0, r1, r2, r3, r4, s1, s2, s3, s4;
    unsigned int h0 = 0, h1 = 0, h2 = 0, h3 = 0, h4 = 0;
    unsigned int t0, t1, t2, t3, c, g0, g1, g2, g3, g4, mask;
    unsigned long long d0, d1, d2, d3, d4, f;
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
    while (len > 0) {
        unsigned char block[16];
        size_t i, n = len < 16 ? len : 16;
        unsigned int hibit;
        for (i = 0; i < n; ++i) block[i] = msg[i];
        if (n < 16) {
            block[n] = 1;
            for (i = n + 1; i < 16; ++i) block[i] = 0;
            hibit = 0;
        } else {
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
static void sad_poly1305_keygen(const unsigned char key[32], const unsigned char nonce[12],
                                unsigned char otk[32]) {
    unsigned char blk[64];
    sad_chacha20_block(key, 0, nonce, blk);
    memcpy(otk, blk, 32);
}
static void sad_aead_compute_tag(const unsigned char otk[32],
                                 const unsigned char *aad, size_t aadlen,
                                 const unsigned char *ct, size_t ctlen, unsigned char tag[16]) {
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
static int sad_ct_equal(const unsigned char *a, const unsigned char *b, size_t n) {
    unsigned char d = 0;
    size_t i;
    for (i = 0; i < n; ++i) d |= (unsigned char)(a[i] ^ b[i]);
    return d == 0;
}
static void sad_crypto_random_bytes(unsigned char *buf, size_t len) {
    if (sad_crypto_os_random(buf, len)) return;
    {
        static int seeded = 0;
        size_t i;
        if (!seeded) { srand((unsigned int)time(NULL)); seeded = 1; }
        for (i = 0; i < len; ++i) buf[i] = (unsigned char)(rand() & 0xFF);
    }
}
static void sad_aead_key32(const char *key, unsigned char out[32]) {
    size_t klen = strlen(key);
    if (klen == 32) memcpy(out, key, 32);
    else sad_sha256_raw((const unsigned char *)key, klen, out);
}
const char *sad_security_aead_encrypt(const char *text, const char *key) {
    unsigned char key32[32], nonce[12], tag[16];
    unsigned char *ct;
    size_t tlen, i, envlen;
    char *result;
    if (!text) text = "";
    if (!key || !*key) return text;
    tlen = strlen(text);
    if (tlen > (SIZE_MAX / 2 - 64)) { fprintf(stderr, "[sad] error: plaintext too long for AEAD\n"); return text; }
    sad_aead_key32(key, key32);
    sad_crypto_random_bytes(nonce, 12);
    ct = (unsigned char *)malloc(tlen ? tlen : 1);
    if (!ct) { fprintf(stderr, "[sad] error: AEAD alloc failed\n"); return text; }
    sad_chacha20_xor(key32, 1, nonce, (const unsigned char *)text, tlen, ct);
    {
        unsigned char otk[32];
        unsigned char aad_empty = 0;
        sad_poly1305_keygen(key32, nonce, otk);
        sad_aead_compute_tag(otk, &aad_empty, 0, ct, tlen, tag);
    }
    envlen = 12 + tlen + 16;
    result = (char *)malloc(envlen * 2 + 1);
    if (!result) { free(ct); fprintf(stderr, "[sad] error: AEAD alloc failed\n"); return text; }
    for (i = 0; i < 12; ++i) snprintf(result + i * 2, 3, "%02x", nonce[i]);
    for (i = 0; i < tlen; ++i) snprintf(result + (12 + i) * 2, 3, "%02x", ct[i]);
    for (i = 0; i < 16; ++i) snprintf(result + (12 + tlen + i) * 2, 3, "%02x", tag[i]);
    result[envlen * 2] = '\0';
    free(ct);
    return result;
}
const char *sad_security_aead_decrypt(const char *hex, const char *key) {
    unsigned char key32[32], nonce[12], tag[16], tag2[16];
    unsigned char *raw, *ct;
    size_t hlen, rlen, ctlen, i;
    char *result;
    if (!hex) hex = "";
    if (!key || !*key) { fprintf(stderr, "[sad] error: AEAD decrypt - missing key\n"); exit(1); }
    hlen = strlen(hex);
    if (hlen % 2 != 0 || (hlen / 2) < (12 + 16)) { fprintf(stderr, "[sad] error: AEAD auth failed - tampered envelope or wrong key\n"); exit(1); }
    rlen = hlen / 2;
    raw = (unsigned char *)malloc(rlen);
    if (!raw) { fprintf(stderr, "[sad] error: AEAD alloc failed\n"); exit(1); }
    for (i = 0; i < rlen; ++i) {
        unsigned int byte = 0;
        if (sscanf(hex + i * 2, "%02x", &byte) != 1 || byte > 255) {
            free(raw); fprintf(stderr, "[sad] error: AEAD auth failed - tampered envelope or wrong key\n"); exit(1);
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
        unsigned char aad_empty = 0;
        sad_poly1305_keygen(key32, nonce, otk);
        sad_aead_compute_tag(otk, &aad_empty, 0, ct, ctlen, tag2);
    }
    if (!sad_ct_equal(tag, tag2, 16)) {
        free(raw); fprintf(stderr, "[sad] error: AEAD auth failed - tampered envelope or wrong key\n"); exit(1);
    }
    result = (char *)malloc(ctlen + 1);
    if (!result) { free(raw); fprintf(stderr, "[sad] error: AEAD alloc failed\n"); exit(1); }
    sad_chacha20_xor(key32, 1, nonce, ct, ctlen, (unsigned char *)result);
    result[ctlen] = '\0';
    free(raw);
    return result;
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
