// ===================================================================
// صNet — تنفيذ AES-256-GCM و CSPRNG
// (AR) تشفير مُصادق (AEAD) ذاتي + مولّد أرقام عشوائية آمن
//      AES: تنفيذ كامل بـ T-tables + AES-NI (تسريع عتادي)
//      GCM: وضع Galois/Counter Mode مع GHASH 4-bit table
//      CSPRNG: BCryptGenRandom (ويندوز) أو /dev/urandom (لينكس)
// (EN) AES-256-GCM AEAD with AES-NI + T-tables fallback + CSPRNG
// المكون: network/sadnet (تشفير AES-GCM)
// المسار: network/sadnet/src/crypto/aes_gcm.cpp
// ===================================================================

#include "sadnet/crypto/crypto.h"
#include <cstring>
#include <stdexcept>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#else
#include <fcntl.h>
#include <unistd.h>
#endif

// (AR) كشف دعم AES-NI و PCLMULQDQ في وقت التشغيل
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#include <intrin.h>
#include <wmmintrin.h> // AES-NI + PCLMULQDQ
#include <smmintrin.h> // SSE4.1
#define SADNET_AESNI_AVAILABLE 1
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <cpuid.h>
#include <wmmintrin.h>
#include <smmintrin.h>
#define SADNET_AESNI_AVAILABLE 1
#else
#define SADNET_AESNI_AVAILABLE 0
#endif

// (AR) على GCC/Clang: تعليمات AES-NI/PCLMUL تتطلّب تفعيل ISA لترجمتها داخل
//      الدوال (وإلا فشل تضمين always_inline: «target specific option mismatch»).
//      نُفعّلها لكامل وحدة الترجمة عبر pragma؛ والإرسال وقت التشغيل (HAS_AESNI)
//      يضمن ألّا تُنفَّذ على معالج لا يدعمها. MSVC لا يحتاج ذلك (يتجاهل الـpragma).
// (EN) On GCC/Clang, AES-NI/PCLMUL intrinsics need the ISA enabled to compile
//      inside functions (else always_inline inlining fails). We enable it for the
//      whole TU via pragma; runtime dispatch (HAS_AESNI) ensures it never executes
//      on unsupported CPUs. MSVC needs no such flag (ignores the pragma).
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#pragma GCC push_options
#pragma GCC target("aes,pclmul,sse4.1,ssse3")
#endif

namespace sad::net::crypto
{

#if SADNET_AESNI_AVAILABLE
    // (AR) كشف دعم AES-NI عبر CPUID
    static bool detect_aesni()
    {
#ifdef _MSC_VER
        int info[4];
        __cpuid(info, 1);
        return (info[2] & (1 << 25)) != 0; // ECX bit 25 = AES-NI
#else
        unsigned int eax, ebx, ecx, edx;
        if (__get_cpuid(1, &eax, &ebx, &ecx, &edx))
            return (ecx & (1 << 25)) != 0;
        return false;
#endif
    }

    // (AR) كشف دعم PCLMULQDQ (تسريع ضرب GF(2^128) لـ GHASH)
    static bool detect_pclmulqdq()
    {
#ifdef _MSC_VER
        int info[4];
        __cpuid(info, 1);
        return (info[2] & (1 << 1)) != 0; // ECX bit 1 = PCLMULQDQ
#else
        unsigned int eax, ebx, ecx, edx;
        if (__get_cpuid(1, &eax, &ebx, &ecx, &edx))
            return (ecx & (1 << 1)) != 0;
        return false;
#endif
    }

    static const bool HAS_AESNI = detect_aesni();
    static const bool HAS_PCLMULQDQ = detect_pclmulqdq();
#else
    static const bool HAS_AESNI = false;
    static const bool HAS_PCLMULQDQ = false;
#endif

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مولّد الأرقام العشوائية الآمن (CSPRNG)
    // ═════════════════════════════════════════════════════════════════════

    bool random_bytes(uint8_t *out, size_t n)
    {
        if (n == 0)
            return true;
#ifdef _WIN32
        NTSTATUS status = BCryptGenRandom(
            NULL, out, (ULONG)n, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        return BCRYPT_SUCCESS(status);
#else
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd < 0)
            return false;
        size_t total = 0;
        while (total < n)
        {
            ssize_t r = read(fd, out + total, n - total);
            if (r <= 0)
            {
                close(fd);
                return false;
            }
            total += static_cast<size_t>(r);
        }
        close(fd);
        return true;
#endif
    }

    Buffer random_buffer(size_t n)
    {
        Buffer buf(n);
        if (!random_bytes(buf.data(), n))
            buf.clear();
        return buf;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تنفيذ AES-256 الأساسي
    //      14 جولة، مفتاح 256 بت، كتلة 128 بت
    // ═════════════════════════════════════════════════════════════════════

    // (AR) جدول الاستبدال S-box
    static const uint8_t SBOX[256] = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

    // (AR) ثوابت الجولات (Rcon)
    static const uint8_t RCON[11] = {
        0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

    // (AR) توسيع مفتاح AES-256: من 32 بايت إلى 240 بايت (15 مفتاح جولة × 16 بايت)
    static void aes256_key_expand(const uint8_t key[32], uint8_t round_keys[240])
    {
        std::memcpy(round_keys, key, 32);

        uint8_t tmp[4];
        int bytes_generated = 32;
        int rcon_index = 1;

        while (bytes_generated < 240)
        {
            // نسخ آخر 4 بايتات
            std::memcpy(tmp, round_keys + bytes_generated - 4, 4);

            if (bytes_generated % 32 == 0)
            {
                // RotWord + SubWord + Rcon
                uint8_t t = tmp[0];
                tmp[0] = SBOX[tmp[1]] ^ RCON[rcon_index++];
                tmp[1] = SBOX[tmp[2]];
                tmp[2] = SBOX[tmp[3]];
                tmp[3] = SBOX[t];
            }
            else if (bytes_generated % 32 == 16)
            {
                // SubWord فقط
                for (int i = 0; i < 4; i++)
                    tmp[i] = SBOX[tmp[i]];
            }

            for (int i = 0; i < 4; i++)
            {
                round_keys[bytes_generated] =
                    round_keys[bytes_generated - 32] ^ tmp[i];
                bytes_generated++;
            }
        }
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) T-tables لتسريع AES-256 — تدمج SubBytes + ShiftRows + MixColumns
    //      في 4 جداول بحث × 256 مدخل × 4 بايت = 4 كيلوبايت
    //      كل مدخل Te_i[a] = عمود MixColumns بعد SubBytes لبايت واحد
    //      تسريع ~3-5× مقارنة بالتنفيذ البايتي الأصلي
    // ═════════════════════════════════════════════════════════════════════

    // (AR) عمليات AES الأساسية على كتلة 16 بايت

    static inline uint8_t gmul2(uint8_t a)
    {
        return (a << 1) ^ ((a >> 7) * 0x1b);
    }

    // (AR) بناء T-table: دمج S-box + MixColumns
    //      Te0[a] = { 2·S(a), S(a), S(a), 3·S(a) }  — العمود 0 من MixColumns
    //      Te1[a] = { 3·S(a), 2·S(a), S(a), S(a) }  — دوران 1
    //      Te2[a] = { S(a), 3·S(a), 2·S(a), S(a) }  — دوران 2
    //      Te3[a] = { S(a), S(a), 3·S(a), 2·S(a) }  — دوران 3
    static uint32_t Te0[256], Te1[256], Te2[256], Te3[256];
    static bool t_tables_initialized = false;

    static void init_t_tables()
    {
        if (t_tables_initialized)
            return;
        for (int i = 0; i < 256; i++)
        {
            uint8_t s = SBOX[i];
            uint8_t s2 = gmul2(s);
            uint8_t s3 = s2 ^ s;
            // (AR) ترتيب البايتات في uint32_t: [byte0, byte1, byte2, byte3]
            //      byte0 = أقل بايت (المخزن أولاً في little-endian)
            Te0[i] = (uint32_t)s2 | ((uint32_t)s << 8) | ((uint32_t)s << 16) | ((uint32_t)s3 << 24);
            Te1[i] = (uint32_t)s3 | ((uint32_t)s2 << 8) | ((uint32_t)s << 16) | ((uint32_t)s << 24);
            Te2[i] = (uint32_t)s | ((uint32_t)s3 << 8) | ((uint32_t)s2 << 16) | ((uint32_t)s << 24);
            Te3[i] = (uint32_t)s | ((uint32_t)s << 8) | ((uint32_t)s3 << 16) | ((uint32_t)s2 << 24);
        }
        t_tables_initialized = true;
    }

    static void add_round_key(uint8_t state[16], const uint8_t *rk)
    {
        for (int i = 0; i < 16; i++)
            state[i] ^= rk[i];
    }

    /// تشفير كتلة واحدة 128 بت بـ AES-256 مع T-tables
    static void aes256_encrypt_block(const uint8_t in[16], uint8_t out[16],
                                     const uint8_t round_keys[240])
    {
        init_t_tables();

        // (AR) تحميل الحالة كـ 4 أعمدة uint32_t (column-major: state[row + 4*col])
        //      العمود c = state[4c..4c+3] → t[c]
        uint32_t t0, t1, t2, t3;
        std::memcpy(&t0, in + 0, 4);
        std::memcpy(&t1, in + 4, 4);
        std::memcpy(&t2, in + 8, 4);
        std::memcpy(&t3, in + 12, 4);

        // (AR) XOR مع مفتاح الجولة 0
        uint32_t rk0, rk1, rk2, rk3;
        std::memcpy(&rk0, round_keys + 0, 4);
        std::memcpy(&rk1, round_keys + 4, 4);
        std::memcpy(&rk2, round_keys + 8, 4);
        std::memcpy(&rk3, round_keys + 12, 4);
        t0 ^= rk0;
        t1 ^= rk1;
        t2 ^= rk2;
        t3 ^= rk3;

        // (AR) الجولات 1-13: SubBytes + ShiftRows + MixColumns مدمجة في T-tables
        //      ShiftRows تُطبّق ضمنياً عبر اختيار البايتات من أعمدة مختلفة
        //      العمود c (بصف s0..s3 بعد ShiftRows):
        //        s0 = بايت 0 من العمود c     = state[4c]
        //        s1 = بايت 1 من العمود (c+1)%4 = state[4*((c+1)%4)+1]
        //        s2 = بايت 2 من العمود (c+2)%4 = state[4*((c+2)%4)+2]
        //        s3 = بايت 3 من العمود (c+3)%4 = state[4*((c+3)%4)+3]
        //      في little-endian: بايت 0 = أقل بايت من uint32_t
        for (int round = 1; round <= 13; round++)
        {
            const uint8_t *rk = round_keys + round * 16;
            std::memcpy(&rk0, rk + 0, 4);
            std::memcpy(&rk1, rk + 4, 4);
            std::memcpy(&rk2, rk + 8, 4);
            std::memcpy(&rk3, rk + 12, 4);

            // (AR) استخراج البايتات من الأعمدة (little-endian):
            //      بايت 0 (صف 0) = t & 0xFF
            //      بايت 1 (صف 1) = (t >> 8) & 0xFF
            //      بايت 2 (صف 2) = (t >> 16) & 0xFF
            //      بايت 3 (صف 3) = (t >> 24) & 0xFF
            uint32_t u0 = Te0[t0 & 0xFF] ^ Te1[(t1 >> 8) & 0xFF] ^
                          Te2[(t2 >> 16) & 0xFF] ^ Te3[(t3 >> 24) & 0xFF] ^ rk0;
            uint32_t u1 = Te0[t1 & 0xFF] ^ Te1[(t2 >> 8) & 0xFF] ^
                          Te2[(t3 >> 16) & 0xFF] ^ Te3[(t0 >> 24) & 0xFF] ^ rk1;
            uint32_t u2 = Te0[t2 & 0xFF] ^ Te1[(t3 >> 8) & 0xFF] ^
                          Te2[(t0 >> 16) & 0xFF] ^ Te3[(t1 >> 24) & 0xFF] ^ rk2;
            uint32_t u3 = Te0[t3 & 0xFF] ^ Te1[(t0 >> 8) & 0xFF] ^
                          Te2[(t1 >> 16) & 0xFF] ^ Te3[(t2 >> 24) & 0xFF] ^ rk3;

            t0 = u0;
            t1 = u1;
            t2 = u2;
            t3 = u3;
        }

        // (AR) الجولة الأخيرة (14): SubBytes + ShiftRows فقط (بدون MixColumns)
        //      نستخدم S-box مباشرة مع تطبيق ShiftRows ضمنياً
        const uint8_t *rk14 = round_keys + 14 * 16;
        out[0] = SBOX[t0 & 0xFF] ^ rk14[0];
        out[1] = SBOX[(t1 >> 8) & 0xFF] ^ rk14[1];
        out[2] = SBOX[(t2 >> 16) & 0xFF] ^ rk14[2];
        out[3] = SBOX[(t3 >> 24) & 0xFF] ^ rk14[3];
        out[4] = SBOX[t1 & 0xFF] ^ rk14[4];
        out[5] = SBOX[(t2 >> 8) & 0xFF] ^ rk14[5];
        out[6] = SBOX[(t3 >> 16) & 0xFF] ^ rk14[6];
        out[7] = SBOX[(t0 >> 24) & 0xFF] ^ rk14[7];
        out[8] = SBOX[t2 & 0xFF] ^ rk14[8];
        out[9] = SBOX[(t3 >> 8) & 0xFF] ^ rk14[9];
        out[10] = SBOX[(t0 >> 16) & 0xFF] ^ rk14[10];
        out[11] = SBOX[(t1 >> 24) & 0xFF] ^ rk14[11];
        out[12] = SBOX[t3 & 0xFF] ^ rk14[12];
        out[13] = SBOX[(t0 >> 8) & 0xFF] ^ rk14[13];
        out[14] = SBOX[(t1 >> 16) & 0xFF] ^ rk14[14];
        out[15] = SBOX[(t2 >> 24) & 0xFF] ^ rk14[15];
    }

#if SADNET_AESNI_AVAILABLE
    // ═════════════════════════════════════════════════════════════════════
    // (AR) AES-NI — تشفير AES-256 عبر تعليمات عتادية
    //      تسريع ~10-50× مقارنة بالتنفيذ البرمجي
    //      يدعم: Intel Westmere+, AMD Bulldozer+
    // ═════════════════════════════════════════════════════════════════════

    // (AR) مساعد لتوسيع المفتاح عبر AES-NI
    static inline __m128i aesni_key_expand_assist(__m128i key, __m128i keygened)
    {
        keygened = _mm_shuffle_epi32(keygened, 0xFF);
        key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
        key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
        key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
        return _mm_xor_si128(key, keygened);
    }

    // (AR) مساعد لتوسيع المفتاح (النصف الثاني من مفتاح 256 بت)
    //      keygened يأتي من _mm_aeskeygenassist_si128(rk[2i], 0) — أي SubWord مطبّق
    //      على الكلمة الأخيرة من مفتاح الجولة الزوجية المحسوبة حديثاً.
    //      نأخذ word 2 (= SubWord(X3)) عبر shuffle 0xAA ونُوزّعه على كل الكلمات.
    static inline __m128i aesni_key_expand_assist2(__m128i key, __m128i keygened)
    {
        // (AR) إصلاح: كان الكود يعيد حساب keygened من key (rk[2i-1]) بدلاً من
        //      استخدام المعامل الممرر (المحسوب من rk[2i] الصحيح).
        //      هذا أدى لمفاتيح جولات فردية خاطئة (SubWord من مفتاح قديم).
        keygened = _mm_shuffle_epi32(keygened, 0xAA);
        __m128i tmp = key;
        tmp = _mm_xor_si128(tmp, _mm_slli_si128(tmp, 4));
        tmp = _mm_xor_si128(tmp, _mm_slli_si128(tmp, 4));
        tmp = _mm_xor_si128(tmp, _mm_slli_si128(tmp, 4));
        return _mm_xor_si128(tmp, keygened);
    }

    // (AR) توسيع مفتاح AES-256 عبر AES-NI — ينتج 15 مفتاح جولة (__m128i)
    static void aesni_key_expand_256(const uint8_t key[32], __m128i rk[15])
    {
        rk[0] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(key));
        rk[1] = _mm_loadu_si128(reinterpret_cast<const __m128i *>(key + 16));

        __m128i tmp;

        // جولة 2: RCON=0x01
        tmp = _mm_aeskeygenassist_si128(rk[1], 0x01);
        rk[2] = aesni_key_expand_assist(rk[0], tmp);
        tmp = _mm_aeskeygenassist_si128(rk[2], 0);
        rk[3] = aesni_key_expand_assist2(rk[1], tmp);

        // جولة 4: RCON=0x02
        tmp = _mm_aeskeygenassist_si128(rk[3], 0x02);
        rk[4] = aesni_key_expand_assist(rk[2], tmp);
        tmp = _mm_aeskeygenassist_si128(rk[4], 0);
        rk[5] = aesni_key_expand_assist2(rk[3], tmp);

        // جولة 6: RCON=0x04
        tmp = _mm_aeskeygenassist_si128(rk[5], 0x04);
        rk[6] = aesni_key_expand_assist(rk[4], tmp);
        tmp = _mm_aeskeygenassist_si128(rk[6], 0);
        rk[7] = aesni_key_expand_assist2(rk[5], tmp);

        // جولة 8: RCON=0x08
        tmp = _mm_aeskeygenassist_si128(rk[7], 0x08);
        rk[8] = aesni_key_expand_assist(rk[6], tmp);
        tmp = _mm_aeskeygenassist_si128(rk[8], 0);
        rk[9] = aesni_key_expand_assist2(rk[7], tmp);

        // جولة 10: RCON=0x10
        tmp = _mm_aeskeygenassist_si128(rk[9], 0x10);
        rk[10] = aesni_key_expand_assist(rk[8], tmp);
        tmp = _mm_aeskeygenassist_si128(rk[10], 0);
        rk[11] = aesni_key_expand_assist2(rk[9], tmp);

        // جولة 12: RCON=0x20
        tmp = _mm_aeskeygenassist_si128(rk[11], 0x20);
        rk[12] = aesni_key_expand_assist(rk[10], tmp);
        tmp = _mm_aeskeygenassist_si128(rk[12], 0);
        rk[13] = aesni_key_expand_assist2(rk[11], tmp);

        // جولة 14: RCON=0x40
        tmp = _mm_aeskeygenassist_si128(rk[13], 0x40);
        rk[14] = aesni_key_expand_assist(rk[12], tmp);
    }

    /// تشفير كتلة 128 بت عبر AES-NI (AES-256, 14 جولة)
    static inline __m128i aesni_encrypt_block(__m128i block, const __m128i rk[15])
    {
        block = _mm_xor_si128(block, rk[0]);
        block = _mm_aesenc_si128(block, rk[1]);
        block = _mm_aesenc_si128(block, rk[2]);
        block = _mm_aesenc_si128(block, rk[3]);
        block = _mm_aesenc_si128(block, rk[4]);
        block = _mm_aesenc_si128(block, rk[5]);
        block = _mm_aesenc_si128(block, rk[6]);
        block = _mm_aesenc_si128(block, rk[7]);
        block = _mm_aesenc_si128(block, rk[8]);
        block = _mm_aesenc_si128(block, rk[9]);
        block = _mm_aesenc_si128(block, rk[10]);
        block = _mm_aesenc_si128(block, rk[11]);
        block = _mm_aesenc_si128(block, rk[12]);
        block = _mm_aesenc_si128(block, rk[13]);
        return _mm_aesenclast_si128(block, rk[14]);
    }

    /// AES-256-CTR عبر AES-NI — يعالج 4 كتل بالتوازي (pipelining)
    // (AR) دالة مساعدة: تعيين عدّاد big-endian
    static inline __m128i make_ctr_block(const uint8_t nonce[12], uint32_t ctr)
    {
        uint8_t blk[16];
        std::memcpy(blk, nonce, 12);
        blk[12] = (uint8_t)(ctr >> 24);
        blk[13] = (uint8_t)(ctr >> 16);
        blk[14] = (uint8_t)(ctr >> 8);
        blk[15] = (uint8_t)(ctr);
        return _mm_loadu_si128(reinterpret_cast<const __m128i *>(blk));
    }

    static void aesni_ctr(const __m128i rk[15],
                          const uint8_t nonce[12],
                          const uint8_t *in, size_t len,
                          uint8_t *out)
    {
        uint32_t ctr = 2; // CTR يبدأ من 2 (J0=1 محجوز للوسم)
        size_t pos = 0;

        // ═══════════════════════════════════════════════════════════════
        // (AR) معالجة 4 كتل بالتوازي (pipelining)
        // AES-NI لديها latency 4 دورات لكن throughput 1 — نستغل ذلك
        // ═══════════════════════════════════════════════════════════════
        while (pos + 64 <= len)
        {
            __m128i c0 = make_ctr_block(nonce, ctr);
            __m128i c1 = make_ctr_block(nonce, ctr + 1);
            __m128i c2 = make_ctr_block(nonce, ctr + 2);
            __m128i c3 = make_ctr_block(nonce, ctr + 3);

            // الجولة الأولى: XOR مع rk[0]
            c0 = _mm_xor_si128(c0, rk[0]);
            c1 = _mm_xor_si128(c1, rk[0]);
            c2 = _mm_xor_si128(c2, rk[0]);
            c3 = _mm_xor_si128(c3, rk[0]);

            // الجولات 1-13: AES rounds بالتوازي
            for (int r = 1; r <= 13; r++)
            {
                c0 = _mm_aesenc_si128(c0, rk[r]);
                c1 = _mm_aesenc_si128(c1, rk[r]);
                c2 = _mm_aesenc_si128(c2, rk[r]);
                c3 = _mm_aesenc_si128(c3, rk[r]);
            }

            // الجولة الأخيرة
            c0 = _mm_aesenclast_si128(c0, rk[14]);
            c1 = _mm_aesenclast_si128(c1, rk[14]);
            c2 = _mm_aesenclast_si128(c2, rk[14]);
            c3 = _mm_aesenclast_si128(c3, rk[14]);

            // XOR مع البيانات المدخلة
            __m128i p0 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + pos));
            __m128i p1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + pos + 16));
            __m128i p2 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + pos + 32));
            __m128i p3 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + pos + 48));

            _mm_storeu_si128(reinterpret_cast<__m128i *>(out + pos), _mm_xor_si128(p0, c0));
            _mm_storeu_si128(reinterpret_cast<__m128i *>(out + pos + 16), _mm_xor_si128(p1, c1));
            _mm_storeu_si128(reinterpret_cast<__m128i *>(out + pos + 32), _mm_xor_si128(p2, c2));
            _mm_storeu_si128(reinterpret_cast<__m128i *>(out + pos + 48), _mm_xor_si128(p3, c3));

            pos += 64;
            ctr += 4;
        }

        // (AR) معالجة الكتل المتبقية واحدة تلو الأخرى
        while (pos < len)
        {
            __m128i counter = make_ctr_block(nonce, ctr);
            __m128i keystream = aesni_encrypt_block(counter, rk);

            size_t chunk = std::min(size_t(16), len - pos);
            if (chunk == 16)
            {
                __m128i plain = _mm_loadu_si128(reinterpret_cast<const __m128i *>(in + pos));
                _mm_storeu_si128(reinterpret_cast<__m128i *>(out + pos),
                                 _mm_xor_si128(plain, keystream));
            }
            else
            {
                uint8_t ks[16];
                _mm_storeu_si128(reinterpret_cast<__m128i *>(ks), keystream);
                for (size_t i = 0; i < chunk; i++)
                    out[pos + i] = in[pos + i] ^ ks[i];
            }

            pos += chunk;
            ctr++;
        }
    }
#endif // SADNET_AESNI_AVAILABLE

    // ═════════════════════════════════════════════════════════════════════
    // (AR) AES-256-CTR — وضع العدّاد (لتشفير الدفق)
    // ═════════════════════════════════════════════════════════════════════

    /// زيادة العدّاد (big-endian) في آخر 4 بايتات
    static void increment_counter(uint8_t counter[16])
    {
        for (int i = 15; i >= 12; i--)
        {
            if (++counter[i] != 0)
                break;
        }
    }

    /// تشفير/فك تشفير بوضع CTR مع round_keys مُمرّرة مسبقاً
    static void aes256_ctr_with_rk(const uint8_t round_keys[240],
                                   const uint8_t nonce[12],
                                   const uint8_t *in, size_t len,
                                   uint8_t *out)
    {
        // بناء العدّاد الأوّلي: nonce (12 بايت) + عدّاد (4 بايت) يبدأ من 2
        // (العدّاد 1 محجوز لحساب وسم المصادقة في GCM)
        uint8_t counter[16];
        std::memcpy(counter, nonce, 12);
        counter[12] = 0;
        counter[13] = 0;
        counter[14] = 0;
        counter[15] = 2;

        uint8_t keystream[16];
        size_t pos = 0;

        while (pos < len)
        {
            aes256_encrypt_block(counter, keystream, round_keys);
            size_t chunk = std::min(size_t(16), len - pos);
            for (size_t i = 0; i < chunk; i++)
                out[pos + i] = in[pos + i] ^ keystream[i];
            pos += chunk;
            increment_counter(counter);
        }
    }

    /// تشفير/فك تشفير بوضع CTR — واجهة متوافقة (تحسب round_keys داخلياً)
    static void aes256_ctr(const uint8_t key[32],
                           const uint8_t nonce[12],
                           const uint8_t *in, size_t len,
                           uint8_t *out)
    {
        uint8_t round_keys[240];
        aes256_key_expand(key, round_keys);
        aes256_ctr_with_rk(round_keys, nonce, in, len, out);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) GHASH مُحسَّن — جدول 4-بت (Shoup's method)
    //      بدلاً من 128 تكراراً بت-بت، نعالج 4 بتات في كل خطوة
    //      = 32 تكراراً فقط مع lookup في جدول 16 مدخلاً مُحسب مسبقاً
    //      الجدول يُبنى من ضرب bit-by-bit المرجعي لضمان التوافق 100%
    // ═════════════════════════════════════════════════════════════════════

    // (AR) سياق GHASH يحمل الجدول المُحسب مسبقاً (16 مدخل × 16 بايت)
    struct GHashCtx
    {
        uint8_t table[16][16]; // table[i] = i * H في GF(2^128)
    };

    // (AR) جدول الاختزال لـ 4 بتات: GHASH_REDUCE[v] = v * R حيث
    //      R = x^128 + x^7 + x^2 + x + 1 = 0xE1 << 56
    //      كل مدخل = قيمة XOR لأعلى بايتين عند إزاحة يمين 4 بتات
    static const uint16_t GHASH_REDUCE[16] = {
        0x0000, 0x01C2, 0x0384, 0x0246, 0x0708, 0x06CA, 0x048C, 0x054E,
        0x0E10, 0x0FD2, 0x0D94, 0x0C56, 0x0918, 0x08DA, 0x0A9C, 0x0B5E};

    /// إزاحة يمين 1 بت في GF(2^128) مع اختزال (مساعدة لبناء الجدول)
    static inline void gf128_shift_right_1(const uint8_t in[16], uint8_t out[16])
    {
        bool carry = in[15] & 1;
        for (int j = 15; j > 0; j--)
            out[j] = (in[j] >> 1) | (in[j - 1] << 7);
        out[0] = in[0] >> 1;
        if (carry)
            out[0] ^= 0xe1; // اختزال x^128 + x^7 + x^2 + x + 1
    }

    // (AR) بناء جدول GHASH بطريقة Shoup المُحسَّنة:
    //      3 عمليات إزاحة + 12 XOR فقط (بدلاً من 15× ضرب كامل 128-بت)
    //      table[0]=0, table[8]=H, table[4]=x*H, table[2]=x²*H, table[1]=x³*H
    //      الباقي بـ XOR: table[i]=table[MSB(i)] ^ table[i-MSB(i)]
    static void ghash_setup(GHashCtx &ctx, const uint8_t H[16])
    {
        // table[0] = 0
        std::memset(ctx.table[0], 0, 16);
        // table[8] = H
        std::memcpy(ctx.table[8], H, 16);

        // table[4] = x * H (إزاحة يمين 1 بت + اختزال)
        gf128_shift_right_1(ctx.table[8], ctx.table[4]);
        // table[2] = x² * H
        gf128_shift_right_1(ctx.table[4], ctx.table[2]);
        // table[1] = x³ * H
        gf128_shift_right_1(ctx.table[2], ctx.table[1]);

        // (AR) الأرقام الزوجية (التي ليست قوى 2) والأرقام الفردية
        //      بناء بـ XOR: table[i] = table[أعلى_بت(i)] ^ table[i - أعلى_بت(i)]
        for (int i = 3; i < 16; i++)
        {
            if (i == 4 || i == 8)
                continue; // مبنية مسبقاً
            // (AR) نجد أعلى بت مُفعّل في i
            int msb = (i >= 8) ? 8 : (i >= 4) ? 4
                                 : (i >= 2)   ? 2
                                              : 1;
            int rest = i ^ msb; // الباقي بعد إزالة أعلى بت
            for (int j = 0; j < 16; j++)
                ctx.table[i][j] = ctx.table[msb][j] ^ ctx.table[rest][j];
        }
    }

    // (AR) ضرب GHASH المُحسَّن: Y = Y * H في GF(2^128)
    //      يعالج 4 بتات في كل خطوة عبر الجدول المُحسب
    //      المعالجة من LSB (بايت 15) إلى MSB (بايت 0)
    //      في كل بايت: nibble سُفلي أولاً ثم nibble عُلوي
    static void ghash_mult_4bit(const GHashCtx &ctx, uint8_t Y[16])
    {
        uint8_t Z[16] = {};

        // (AR) نعالج Y من البايت الأخير إلى الأول
        //      nibble سُفلي (بتات 3..0) أولاً، ثم عُلوي (بتات 7..4)
        //      هذا يتوافق مع ترتيب بناء الجدول (Xi[15] = i)
        for (int i = 15; i >= 0; i--)
        {
            // (AR) nibble سُفلي (بتات 3..0)
            {
                // إزاحة Z يميناً 4 بتات مع اختزال
                uint8_t rem = Z[15] & 0x0F;
                for (int j = 15; j > 0; j--)
                    Z[j] = (Z[j] >> 4) | (Z[j - 1] << 4);
                Z[0] >>= 4;
                // اختزال: XOR مع R * rem
                uint16_t rv = GHASH_REDUCE[rem];
                Z[0] ^= static_cast<uint8_t>(rv >> 8);
                Z[1] ^= static_cast<uint8_t>(rv & 0xFF);
                // XOR مع table[nibble]
                int idx = Y[i] & 0x0F;
                for (int j = 0; j < 16; j++)
                    Z[j] ^= ctx.table[idx][j];
            }

            // (AR) nibble عُلوي (بتات 7..4)
            {
                uint8_t rem = Z[15] & 0x0F;
                for (int j = 15; j > 0; j--)
                    Z[j] = (Z[j] >> 4) | (Z[j - 1] << 4);
                Z[0] >>= 4;
                uint16_t rv = GHASH_REDUCE[rem];
                Z[0] ^= static_cast<uint8_t>(rv >> 8);
                Z[1] ^= static_cast<uint8_t>(rv & 0xFF);
                int idx = (Y[i] >> 4) & 0x0F;
                for (int j = 0; j < 16; j++)
                    Z[j] ^= ctx.table[idx][j];
            }
        }

        std::memcpy(Y, Z, 16);
    }

    /// حساب GHASH على بيانات (مُحسَّن بجدول 4-بت)
    // ═════════════════════════════════════════════════════════════════════
    // (AR) GHASH مُسرَّع بـ PCLMULQDQ (ضرب GF(2^128) عتادياً)
    // ═════════════════════════════════════════════════════════════════════
#if SADNET_AESNI_AVAILABLE
    // (AR) ضرب عنصرين في GF(2^128) عبر CLMUL + اختزال CLMUL
    // (AR) المعامل: x^128 + x^7 + x^2 + x + 1 (معامل GCM القياسي)
    //      بعد reflect_bytes، CLMUL x^i = GCM x^i — نفس المعامل.
    //      الاختزال: c(x) = t1*x^128 + t0 ≡ t0 + t1*(x^7+x^2+x+1) mod p
    //      نضرب t1 (الجزء العلوي) بـ 0x87 ونجمعه مع t0.
    static inline __m128i clmul_reduce(__m128i H, __m128i X)
    {
        // ═══ الخطوة 1: ضرب Karatsuba (256 بت) ═══
        __m128i t0 = _mm_clmulepi64_si128(H, X, 0x00); // lo * lo
        __m128i t1 = _mm_clmulepi64_si128(H, X, 0x11); // hi * hi
        __m128i t2 = _mm_clmulepi64_si128(H, X, 0x01); // cross term 1
        __m128i t3 = _mm_clmulepi64_si128(H, X, 0x10); // cross term 2
        __m128i mid = _mm_xor_si128(t2, t3);

        // دمج الأجزاء الوسطى → النتيجة 256 بت في t1:t0 (t1=علوي، t0=سفلي)
        t0 = _mm_xor_si128(t0, _mm_slli_si128(mid, 8));
        t1 = _mm_xor_si128(t1, _mm_srli_si128(mid, 8));

        // ═══ الخطوة 2: اختزال modulo x^128 + x^7 + x^2 + x + 1 ═══
        // c(x) = t1 * x^128 + t0  ≡  t0 + t1 * 0x87  (mod p)
        // t1 * 0x87: ضرب 128 بت × 8 بت = حتى 136 بت (7 بتات فوق 128)
        __m128i poly = _mm_set_epi64x(0, 0x87);

        // المرحلة 1: t1 * 0x87 عبر CLMUL (نصفين)
        __m128i r_lo = _mm_clmulepi64_si128(t1, poly, 0x00); // t1_lo * 0x87
        __m128i r_hi = _mm_clmulepi64_si128(t1, poly, 0x01); // t1_hi * 0x87

        // الجمع: t1*0x87 = r_lo + r_hi*x^64
        // r_lo يدخل مباشرة في t0
        // r_hi: الـ 64 بت السفلى تدخل في النصف العلوي من t0
        //       الـ 7 بتات العليا (128-134) تفيض
        t0 = _mm_xor_si128(t0, r_lo);
        t0 = _mm_xor_si128(t0, _mm_slli_si128(r_hi, 8));

        // المرحلة 2: اختزال الفائض (7 بتات في أقصاه)
        __m128i overflow = _mm_srli_si128(r_hi, 8);
        __m128i r_ov = _mm_clmulepi64_si128(overflow, poly, 0x00);
        t0 = _mm_xor_si128(t0, r_ov);

        return t0;
    }

    // (AR) عكس بتات كل بايت (GHASH يستخدم ترتيب بت معكوس)
    static inline __m128i reflect_bytes(__m128i x)
    {
        // GCM يستخدم ترتيب بت معكوس داخل كل بايت
        // لكن PCLMULQDQ أيضاً يعمل بترتيب بت معكوس —
        // لذلك نعكس البايتات فقط (byte-swap)
        // GHASH convention: bit 0 = MSB. CLMUL: bit 0 = LSB.
        // نعكس بايتات + بتات كل بايت
        __m128i mask_0f = _mm_set1_epi8(0x0F);
        __m128i lut = _mm_setr_epi8(
            0, 8, 4, 12, 2, 10, 6, 14,
            1, 9, 5, 13, 3, 11, 7, 15);
        __m128i lo = _mm_and_si128(x, mask_0f);
        __m128i hi = _mm_and_si128(_mm_srli_epi16(x, 4), mask_0f);
        return _mm_or_si128(
            _mm_slli_epi16(_mm_shuffle_epi8(lut, lo), 4),
            _mm_shuffle_epi8(lut, hi));
    }

    // (AR) عكس ترتيب البايتات (byte-swap / bswap128)
    static inline __m128i bswap128(__m128i x)
    {
        __m128i mask = _mm_setr_epi8(15, 14, 13, 12, 11, 10, 9, 8,
                                     7, 6, 5, 4, 3, 2, 1, 0);
        return _mm_shuffle_epi8(x, mask);
    }

    // (AR) GHASH المُسرَّع بـ CLMUL
    static void ghash_clmul(const uint8_t H_bytes[16],
                            const uint8_t *data, size_t data_len,
                            const uint8_t *aad, size_t aad_len,
                            uint8_t tag[16])
    {
        // تحضير H بتنسيق CLMUL (عكس البتات)
        __m128i H_raw = _mm_loadu_si128(reinterpret_cast<const __m128i *>(H_bytes));
        __m128i H = reflect_bytes(H_raw);

        __m128i Y = _mm_setzero_si128();

        // (AR) معالجة AAD
        size_t pos = 0;
        while (pos + 16 <= aad_len)
        {
            __m128i block = _mm_loadu_si128(reinterpret_cast<const __m128i *>(aad + pos));
            block = reflect_bytes(block);
            Y = _mm_xor_si128(Y, block);
            Y = clmul_reduce(H, Y);
            pos += 16;
        }
        if (pos < aad_len)
        {
            uint8_t pad[16] = {};
            std::memcpy(pad, aad + pos, aad_len - pos);
            __m128i block = reflect_bytes(_mm_loadu_si128(reinterpret_cast<const __m128i *>(pad)));
            Y = _mm_xor_si128(Y, block);
            Y = clmul_reduce(H, Y);
        }

        // (AR) معالجة النص المُشفّر
        pos = 0;
        while (pos + 16 <= data_len)
        {
            __m128i block = _mm_loadu_si128(reinterpret_cast<const __m128i *>(data + pos));
            block = reflect_bytes(block);
            Y = _mm_xor_si128(Y, block);
            Y = clmul_reduce(H, Y);
            pos += 16;
        }
        if (pos < data_len)
        {
            uint8_t pad[16] = {};
            std::memcpy(pad, data + pos, data_len - pos);
            __m128i block = reflect_bytes(_mm_loadu_si128(reinterpret_cast<const __m128i *>(pad)));
            Y = _mm_xor_si128(Y, block);
            Y = clmul_reduce(H, Y);
        }

        // (AR) كتلة الأطوال
        uint8_t lengths[16] = {};
        uint64_t aad_bits = aad_len * 8;
        uint64_t ct_bits = data_len * 8;
        for (int i = 0; i < 8; i++)
        {
            lengths[i] = static_cast<uint8_t>((aad_bits >> (56 - i * 8)) & 0xFF);
            lengths[8 + i] = static_cast<uint8_t>((ct_bits >> (56 - i * 8)) & 0xFF);
        }
        __m128i len_block = reflect_bytes(_mm_loadu_si128(reinterpret_cast<const __m128i *>(lengths)));
        Y = _mm_xor_si128(Y, len_block);
        Y = clmul_reduce(H, Y);

        // عكس البتات للنتيجة النهائية
        Y = reflect_bytes(Y);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(tag), Y);
    }
#endif // SADNET_AESNI_AVAILABLE

    static void ghash(const uint8_t H[16],
                      const uint8_t *data, size_t data_len,
                      const uint8_t *aad, size_t aad_len,
                      uint8_t tag[16])
    {
        GHashCtx ctx;
        ghash_setup(ctx, H);

        uint8_t Y[16] = {};

        // (AR) معالجة AAD (بيانات المصادقة الإضافية)
        size_t pos = 0;
        while (pos < aad_len)
        {
            size_t chunk = std::min(size_t(16), aad_len - pos);
            for (size_t i = 0; i < chunk; i++)
                Y[i] ^= aad[pos + i];
            ghash_mult_4bit(ctx, Y);
            pos += chunk;
        }

        // (AR) معالجة النص المُشفّر
        pos = 0;
        while (pos < data_len)
        {
            size_t chunk = std::min(size_t(16), data_len - pos);
            for (size_t i = 0; i < chunk; i++)
                Y[i] ^= data[pos + i];
            ghash_mult_4bit(ctx, Y);
            pos += chunk;
        }

        // (AR) كتلة الأطوال: len(AAD) || len(C) بالبتّات (big-endian, 64 بت لكل)
        uint8_t lengths[16] = {};
        uint64_t aad_bits = aad_len * 8;
        uint64_t ct_bits = data_len * 8;
        for (int i = 0; i < 8; i++)
        {
            lengths[i] = static_cast<uint8_t>((aad_bits >> (56 - i * 8)) & 0xFF);
            lengths[8 + i] = static_cast<uint8_t>((ct_bits >> (56 - i * 8)) & 0xFF);
        }
        for (int i = 0; i < 16; i++)
            Y[i] ^= lengths[i];
        ghash_mult_4bit(ctx, Y);
        std::memcpy(tag, Y, 16);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) واجهات AES-256-GCM العامة
    // ═════════════════════════════════════════════════════════════════════

    AEADResult aes_gcm_encrypt(const uint8_t *key,
                               const uint8_t *plaintext, size_t pt_len,
                               const uint8_t *aad, size_t aad_len)
    {
        AEADResult result;

        // 1. توليد nonce عشوائي (12 بايت)
        random_bytes(result.nonce.data(), AES_NONCE_SIZE);

#if SADNET_AESNI_AVAILABLE
        if (HAS_AESNI)
        {
            // (AR) مسار AES-NI المُسرَّع عتادياً
            __m128i rk[15];
            aesni_key_expand_256(key, rk);

            // حساب H = AES_K(0^128)
            uint8_t H[16];
            __m128i zero = _mm_setzero_si128();
            __m128i H_enc = aesni_encrypt_block(zero, rk);
            _mm_storeu_si128(reinterpret_cast<__m128i *>(H), H_enc);

            // التشفير بـ AES-NI-CTR
            result.ciphertext.resize(pt_len);
            aesni_ctr(rk, result.nonce.data(), plaintext, pt_len,
                      result.ciphertext.data());

            // حساب وسم GHASH (عتاديًا إن أمكن)
            uint8_t ghash_tag[16];
            if (HAS_PCLMULQDQ)
                ghash_clmul(H, result.ciphertext.data(), pt_len, aad, aad_len, ghash_tag);
            else
                ghash(H, result.ciphertext.data(), pt_len, aad, aad_len, ghash_tag);

            // تشفير وسم GHASH بالعدّاد الأوّلي J0
            uint8_t J0[16];
            std::memcpy(J0, result.nonce.data(), 12);
            J0[12] = 0;
            J0[13] = 0;
            J0[14] = 0;
            J0[15] = 1;
            __m128i j0_enc = aesni_encrypt_block(
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(J0)), rk);
            uint8_t encrypted_j0[16];
            _mm_storeu_si128(reinterpret_cast<__m128i *>(encrypted_j0), j0_enc);

            for (int i = 0; i < 16; i++)
                ghash_tag[i] ^= encrypted_j0[i];

            std::memcpy(result.tag.data(), ghash_tag, AES_TAG_SIZE);
            return result;
        }
#endif
        // (AR) مسار T-tables البرمجي (fallback)
        // 2. حساب round_keys مرة واحدة — تُستخدم في CTR و GHASH
        uint8_t round_keys[240];
        aes256_key_expand(key, round_keys);

        // 3. حساب H = AES_K(0^128) — المفتاح الفرعي لـ GHASH
        uint8_t zero_block[16] = {};
        uint8_t H[16];
        aes256_encrypt_block(zero_block, H, round_keys);

        // 4. التشفير بـ AES-256-CTR (باستخدام round_keys المُحسبة)
        result.ciphertext.resize(pt_len);
        aes256_ctr_with_rk(round_keys, result.nonce.data(), plaintext, pt_len,
                           result.ciphertext.data());

        // 5. حساب وسم GHASH
        uint8_t ghash_tag[16];
        ghash(H, result.ciphertext.data(), pt_len, aad, aad_len, ghash_tag);

        // 6. تشفير وسم GHASH بالعدّاد الأوّلي (J0: nonce || 0x00000001)
        uint8_t J0[16];
        std::memcpy(J0, result.nonce.data(), 12);
        J0[12] = 0;
        J0[13] = 0;
        J0[14] = 0;
        J0[15] = 1;
        uint8_t encrypted_j0[16];
        aes256_encrypt_block(J0, encrypted_j0, round_keys);

        for (int i = 0; i < 16; i++)
            ghash_tag[i] ^= encrypted_j0[i];

        std::memcpy(result.tag.data(), ghash_tag, AES_TAG_SIZE);
        return result;
    }

    // (AR) نسخة تقبل nonce خارجي — للاختبارات المعيارية (NIST vectors)
    AEADResult aes_gcm_encrypt_with_nonce(const uint8_t *key,
                                          const uint8_t *nonce,
                                          const uint8_t *plaintext, size_t pt_len,
                                          const uint8_t *aad, size_t aad_len)
    {
        AEADResult result;
        std::memcpy(result.nonce.data(), nonce, AES_NONCE_SIZE);

#if SADNET_AESNI_AVAILABLE
        if (HAS_AESNI)
        {
            __m128i rk[15];
            aesni_key_expand_256(key, rk);

            uint8_t H[16];
            __m128i H_enc = aesni_encrypt_block(_mm_setzero_si128(), rk);
            _mm_storeu_si128(reinterpret_cast<__m128i *>(H), H_enc);

            result.ciphertext.resize(pt_len);
            if (pt_len > 0)
                aesni_ctr(rk, nonce, plaintext, pt_len, result.ciphertext.data());

            uint8_t ghash_tag[16];
            if (HAS_PCLMULQDQ)
                ghash_clmul(H, result.ciphertext.data(), pt_len, aad, aad_len, ghash_tag);
            else
                ghash(H, result.ciphertext.data(), pt_len, aad, aad_len, ghash_tag);

            uint8_t J0[16];
            std::memcpy(J0, nonce, 12);
            J0[12] = 0;
            J0[13] = 0;
            J0[14] = 0;
            J0[15] = 1;
            __m128i j0_enc = aesni_encrypt_block(
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(J0)), rk);
            uint8_t encrypted_j0[16];
            _mm_storeu_si128(reinterpret_cast<__m128i *>(encrypted_j0), j0_enc);

            for (int i = 0; i < 16; i++)
                ghash_tag[i] ^= encrypted_j0[i];

            std::memcpy(result.tag.data(), ghash_tag, AES_TAG_SIZE);
            return result;
        }
#endif
        uint8_t round_keys[240];
        aes256_key_expand(key, round_keys);

        uint8_t zero_block[16] = {};
        uint8_t H[16];
        aes256_encrypt_block(zero_block, H, round_keys);

        result.ciphertext.resize(pt_len);
        if (pt_len > 0)
            aes256_ctr_with_rk(round_keys, nonce, plaintext, pt_len,
                               result.ciphertext.data());

        uint8_t ghash_tag[16];
        ghash(H, result.ciphertext.data(), pt_len, aad, aad_len, ghash_tag);

        uint8_t J0[16];
        std::memcpy(J0, nonce, 12);
        J0[12] = 0;
        J0[13] = 0;
        J0[14] = 0;
        J0[15] = 1;
        uint8_t encrypted_j0[16];
        aes256_encrypt_block(J0, encrypted_j0, round_keys);

        for (int i = 0; i < 16; i++)
            ghash_tag[i] ^= encrypted_j0[i];

        std::memcpy(result.tag.data(), ghash_tag, AES_TAG_SIZE);
        return result;
    }

    Buffer aes_gcm_decrypt(const uint8_t *key,
                           const uint8_t *ciphertext, size_t ct_len,
                           const uint8_t *nonce,
                           const uint8_t *tag,
                           const uint8_t *aad, size_t aad_len)
    {
#if SADNET_AESNI_AVAILABLE
        if (HAS_AESNI)
        {
            // (AR) مسار AES-NI المُسرَّع
            __m128i rk[15];
            aesni_key_expand_256(key, rk);

            // حساب H
            uint8_t H[16];
            __m128i H_enc = aesni_encrypt_block(_mm_setzero_si128(), rk);
            _mm_storeu_si128(reinterpret_cast<__m128i *>(H), H_enc);

            // حساب وسم GHASH المتوقع (عتاديًا إن أمكن)
            uint8_t expected_tag[16];
            if (HAS_PCLMULQDQ)
                ghash_clmul(H, ciphertext, ct_len, aad, aad_len, expected_tag);
            else
                ghash(H, ciphertext, ct_len, aad, aad_len, expected_tag);

            uint8_t J0[16];
            std::memcpy(J0, nonce, 12);
            J0[12] = 0;
            J0[13] = 0;
            J0[14] = 0;
            J0[15] = 1;
            __m128i j0_enc = aesni_encrypt_block(
                _mm_loadu_si128(reinterpret_cast<const __m128i *>(J0)), rk);
            uint8_t encrypted_j0[16];
            _mm_storeu_si128(reinterpret_cast<__m128i *>(encrypted_j0), j0_enc);

            for (int i = 0; i < 16; i++)
                expected_tag[i] ^= encrypted_j0[i];

            // مقارنة آمنة زمنياً
            uint8_t diff = 0;
            for (int i = 0; i < 16; i++)
                diff |= expected_tag[i] ^ tag[i];

            if (diff != 0)
                return Buffer();

            // فك التشفير عبر AES-NI-CTR
            Buffer plaintext(ct_len);
            aesni_ctr(rk, nonce, ciphertext, ct_len, plaintext.data());
            return plaintext;
        }
#endif
        // (AR) مسار T-tables البرمجي (fallback)
        // 1. حساب round_keys مرة واحدة
        uint8_t round_keys[240];
        aes256_key_expand(key, round_keys);

        // 2. حساب H
        uint8_t zero_block[16] = {};
        uint8_t H[16];
        aes256_encrypt_block(zero_block, H, round_keys);

        // 3. حساب وسم GHASH المتوقع
        uint8_t expected_tag[16];
        ghash(H, ciphertext, ct_len, aad, aad_len, expected_tag);

        uint8_t J0[16];
        std::memcpy(J0, nonce, 12);
        J0[12] = 0;
        J0[13] = 0;
        J0[14] = 0;
        J0[15] = 1;
        uint8_t encrypted_j0[16];
        aes256_encrypt_block(J0, encrypted_j0, round_keys);

        for (int i = 0; i < 16; i++)
            expected_tag[i] ^= encrypted_j0[i];

        // 4. مقارنة آمنة زمنياً (constant-time) لمنع هجمات التوقيت
        uint8_t diff = 0;
        for (int i = 0; i < 16; i++)
            diff |= expected_tag[i] ^ tag[i];

        if (diff != 0)
        {
            // فشل التحقق — البيانات تم العبث بها!
            return Buffer();
        }

        // 5. فك التشفير (باستخدام round_keys المُحسبة)
        Buffer plaintext(ct_len);
        aes256_ctr_with_rk(round_keys, nonce, ciphertext, ct_len, plaintext.data());
        return plaintext;
    }

} // namespace sad::net::crypto

// (AR) إنهاء تفعيل ISA الخاص بـ AES-NI/PCLMUL (يقابل push_options أعلاه).
// (EN) End AES-NI/PCLMUL ISA scope (matches push_options above).
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#pragma GCC pop_options
#endif
