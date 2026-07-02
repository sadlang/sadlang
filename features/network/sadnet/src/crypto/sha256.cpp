// ===================================================================
// صNet — تنفيذ SHA-256 الكامل حسب FIPS 180-4
// (AR) تنفيذ ذاتي بالكامل بدون مكتبات خارجية.
//      يُستخدم في: هوية العُقد، التحقق من المحتوى، HMAC، HKDF
//      يدعم SHA-NI (Intel SHA Extensions) مع fallback بايتي
// (EN) Complete SHA-256 implementation per FIPS 180-4 with SHA-NI
// المكون: features/network/sadnet (تشفير SHA-256)
// المسار: features/network/sadnet/src/crypto/sha256.cpp
// ===================================================================

#include "sadnet/crypto/crypto.h"
#include <cstring>

// (AR) كشف دعم SHA-NI / AES-NI في وقت التشغيل
#if defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))
#include <intrin.h>
#include <immintrin.h>
#define SADNET_X86_INTRINSICS 1
// (AR) MSVC يقبل تعليمات SHA-NI/SSE دون سمة هدف على مستوى الدالة.
// (EN) MSVC accepts SHA-NI/SSE intrinsics without a per-function target attribute.
#define SADNET_SHANI_TARGET
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <cpuid.h>
#include <immintrin.h>
#define SADNET_X86_INTRINSICS 1
// (AR) GCC/Clang يتطلب سمة هدف لترجمة تعليمات SHA-NI داخل دالة دون -msha عام،
//      وإلا فشل تضمين always_inline («target specific option mismatch»). الإرسال
//      وقت التشغيل عبر HAS_SHA_NI يضمن عدم استدعائها على معالج لا يدعمها.
// (EN) GCC/Clang need a per-function target attribute to compile SHA-NI intrinsics
//      without a global -msha (otherwise always_inline inlining fails). Runtime
//      dispatch via HAS_SHA_NI guarantees it is never called on unsupported CPUs.
#define SADNET_SHANI_TARGET __attribute__((target("sha,sse4.1,ssse3")))
#else
#define SADNET_X86_INTRINSICS 0
#define SADNET_SHANI_TARGET
#endif

namespace sad::net::crypto
{

#if SADNET_X86_INTRINSICS
    // (AR) كشف دعم SHA-NI في وقت التشغيل عبر CPUID
    static bool detect_sha_ni()
    {
#ifdef _MSC_VER
        int info[4];
        __cpuidex(info, 7, 0);
        return (info[1] & (1 << 29)) != 0; // EBX bit 29 = SHA
#else
        unsigned int eax, ebx, ecx, edx;
        if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx))
            return (ebx & (1 << 29)) != 0;
        return false;
#endif
    }

    static const bool HAS_SHA_NI = detect_sha_ni();
#else
    static const bool HAS_SHA_NI = false;
#endif

    // ─────────────────────────────────────────────────────────────────────
    // (AR) ثوابت SHA-256 — أول 32 بت من الجذور التكعيبية لأول 64 عدد أولي
    // ─────────────────────────────────────────────────────────────────────

    static constexpr uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    // ─────────────────────────────────────────────────────────────────────
    // (AR) عمليات بت أساسية
    // ─────────────────────────────────────────────────────────────────────

    static inline uint32_t rotr(uint32_t x, int n)
    {
        return (x >> n) | (x << (32 - n));
    }

    static inline uint32_t Ch(uint32_t x, uint32_t y, uint32_t z)
    {
        return (x & y) ^ (~x & z);
    }

    static inline uint32_t Maj(uint32_t x, uint32_t y, uint32_t z)
    {
        return (x & y) ^ (x & z) ^ (y & z);
    }

    static inline uint32_t Sigma0(uint32_t x)
    {
        return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
    }

    static inline uint32_t Sigma1(uint32_t x)
    {
        return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
    }

    static inline uint32_t sigma0(uint32_t x)
    {
        return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
    }

    static inline uint32_t sigma1(uint32_t x)
    {
        return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
    }

    // ─────────────────────────────────────────────────────────────────────
    // (AR) حالة SHA-256 الداخلية
    // ─────────────────────────────────────────────────────────────────────

    struct SHA256State
    {
        uint32_t h[8];      // حالة التجزئة (8 كلمات × 32 بت)
        uint8_t block[64];  // كتلة العمل (512 بت)
        size_t block_len;   // عدد البايتات في الكتلة الحالية
        uint64_t total_len; // إجمالي البايتات المُعالجة

        SHA256State()
        {
            // القيم الأولية — أول 32 بت من الجذور التربيعية لأول 8 أعداد أولية
            h[0] = 0x6a09e667;
            h[1] = 0xbb67ae85;
            h[2] = 0x3c6ef372;
            h[3] = 0xa54ff53a;
            h[4] = 0x510e527f;
            h[5] = 0x9b05688c;
            h[6] = 0x1f83d9ab;
            h[7] = 0x5be0cd19;
            block_len = 0;
            total_len = 0;
        }
    };

    /// معالجة كتلة واحدة 512 بت (مُحسَّنة — جولات مفكوكة + توسيع مُدمج)
    static void sha256_transform(SHA256State &state)
    {
        uint32_t W[64];

        // (AR) إعداد الـ 16 كلمة الأولى من الكتلة (big-endian)
        //      نستخدم مؤشر لتجنب حساب الفهرس المتكرر
        const uint8_t *blk = state.block;
        for (int i = 0; i < 16; i++, blk += 4)
        {
            W[i] = (uint32_t(blk[0]) << 24) | (uint32_t(blk[1]) << 16) |
                   (uint32_t(blk[2]) << 8) | uint32_t(blk[3]);
        }

        // (AR) توسيع إلى 64 كلمة
        for (int i = 16; i < 64; i++)
            W[i] = sigma1(W[i - 2]) + W[i - 7] + sigma0(W[i - 15]) + W[i - 16];

        // (AR) نسخ الحالة الحالية
        uint32_t a = state.h[0], b = state.h[1], c = state.h[2], d = state.h[3];
        uint32_t e = state.h[4], f = state.h[5], g = state.h[6], hh = state.h[7];

        // (AR) ماكرو الجولة المُحسَّن — يتجنب نقل 7 متغيرات في كل تكرار
        //      بدلاً من ذلك: يُحدّث d و hh مباشرة، وتتبادل الأسماء بين الجولات
#define SHA256_ROUND(a, b, c, d, e, f, g, h, i)                    \
    do                                                             \
    {                                                              \
        uint32_t T1 = (h) + Sigma1(e) + Ch(e, f, g) + K[i] + W[i]; \
        uint32_t T2 = Sigma0(a) + Maj(a, b, c);                    \
        (d) += T1;                                                 \
        (h) = T1 + T2;                                             \
    } while (0)

        // (AR) 64 جولة مفكوكة في مجموعات 8 — كل مجموعة تدور الأسماء
        //      هذا يتجنب 7 عمليات نقل في كل جولة
        SHA256_ROUND(a, b, c, d, e, f, g, hh, 0);
        SHA256_ROUND(hh, a, b, c, d, e, f, g, 1);
        SHA256_ROUND(g, hh, a, b, c, d, e, f, 2);
        SHA256_ROUND(f, g, hh, a, b, c, d, e, 3);
        SHA256_ROUND(e, f, g, hh, a, b, c, d, 4);
        SHA256_ROUND(d, e, f, g, hh, a, b, c, 5);
        SHA256_ROUND(c, d, e, f, g, hh, a, b, 6);
        SHA256_ROUND(b, c, d, e, f, g, hh, a, 7);

        SHA256_ROUND(a, b, c, d, e, f, g, hh, 8);
        SHA256_ROUND(hh, a, b, c, d, e, f, g, 9);
        SHA256_ROUND(g, hh, a, b, c, d, e, f, 10);
        SHA256_ROUND(f, g, hh, a, b, c, d, e, 11);
        SHA256_ROUND(e, f, g, hh, a, b, c, d, 12);
        SHA256_ROUND(d, e, f, g, hh, a, b, c, 13);
        SHA256_ROUND(c, d, e, f, g, hh, a, b, 14);
        SHA256_ROUND(b, c, d, e, f, g, hh, a, 15);

        SHA256_ROUND(a, b, c, d, e, f, g, hh, 16);
        SHA256_ROUND(hh, a, b, c, d, e, f, g, 17);
        SHA256_ROUND(g, hh, a, b, c, d, e, f, 18);
        SHA256_ROUND(f, g, hh, a, b, c, d, e, 19);
        SHA256_ROUND(e, f, g, hh, a, b, c, d, 20);
        SHA256_ROUND(d, e, f, g, hh, a, b, c, 21);
        SHA256_ROUND(c, d, e, f, g, hh, a, b, 22);
        SHA256_ROUND(b, c, d, e, f, g, hh, a, 23);

        SHA256_ROUND(a, b, c, d, e, f, g, hh, 24);
        SHA256_ROUND(hh, a, b, c, d, e, f, g, 25);
        SHA256_ROUND(g, hh, a, b, c, d, e, f, 26);
        SHA256_ROUND(f, g, hh, a, b, c, d, e, 27);
        SHA256_ROUND(e, f, g, hh, a, b, c, d, 28);
        SHA256_ROUND(d, e, f, g, hh, a, b, c, 29);
        SHA256_ROUND(c, d, e, f, g, hh, a, b, 30);
        SHA256_ROUND(b, c, d, e, f, g, hh, a, 31);

        SHA256_ROUND(a, b, c, d, e, f, g, hh, 32);
        SHA256_ROUND(hh, a, b, c, d, e, f, g, 33);
        SHA256_ROUND(g, hh, a, b, c, d, e, f, 34);
        SHA256_ROUND(f, g, hh, a, b, c, d, e, 35);
        SHA256_ROUND(e, f, g, hh, a, b, c, d, 36);
        SHA256_ROUND(d, e, f, g, hh, a, b, c, 37);
        SHA256_ROUND(c, d, e, f, g, hh, a, b, 38);
        SHA256_ROUND(b, c, d, e, f, g, hh, a, 39);

        SHA256_ROUND(a, b, c, d, e, f, g, hh, 40);
        SHA256_ROUND(hh, a, b, c, d, e, f, g, 41);
        SHA256_ROUND(g, hh, a, b, c, d, e, f, 42);
        SHA256_ROUND(f, g, hh, a, b, c, d, e, 43);
        SHA256_ROUND(e, f, g, hh, a, b, c, d, 44);
        SHA256_ROUND(d, e, f, g, hh, a, b, c, 45);
        SHA256_ROUND(c, d, e, f, g, hh, a, b, 46);
        SHA256_ROUND(b, c, d, e, f, g, hh, a, 47);

        SHA256_ROUND(a, b, c, d, e, f, g, hh, 48);
        SHA256_ROUND(hh, a, b, c, d, e, f, g, 49);
        SHA256_ROUND(g, hh, a, b, c, d, e, f, 50);
        SHA256_ROUND(f, g, hh, a, b, c, d, e, 51);
        SHA256_ROUND(e, f, g, hh, a, b, c, d, 52);
        SHA256_ROUND(d, e, f, g, hh, a, b, c, 53);
        SHA256_ROUND(c, d, e, f, g, hh, a, b, 54);
        SHA256_ROUND(b, c, d, e, f, g, hh, a, 55);

        SHA256_ROUND(a, b, c, d, e, f, g, hh, 56);
        SHA256_ROUND(hh, a, b, c, d, e, f, g, 57);
        SHA256_ROUND(g, hh, a, b, c, d, e, f, 58);
        SHA256_ROUND(f, g, hh, a, b, c, d, e, 59);
        SHA256_ROUND(e, f, g, hh, a, b, c, d, 60);
        SHA256_ROUND(d, e, f, g, hh, a, b, c, 61);
        SHA256_ROUND(c, d, e, f, g, hh, a, b, 62);
        SHA256_ROUND(b, c, d, e, f, g, hh, a, 63);

#undef SHA256_ROUND

        // (AR) إضافة إلى الحالة
        state.h[0] += a;
        state.h[1] += b;
        state.h[2] += c;
        state.h[3] += d;
        state.h[4] += e;
        state.h[5] += f;
        state.h[6] += g;
        state.h[7] += hh;
    }

#if SADNET_X86_INTRINSICS
    // ═════════════════════════════════════════════════════════════════════
    // (AR) SHA-NI — تنفيذ SHA-256 عبر تعليمات Intel SHA Extensions
    //      يستخدم: _mm_sha256rnds2_epu32, _mm_sha256msg1_epu32,
    //              _mm_sha256msg2_epu32
    //      تسريع ~5-10× مقارنة بالتنفيذ البرمجي
    // ═════════════════════════════════════════════════════════════════════

    // (AR) ثوابت SHA-256 للتحميل SIMD (4 ثوابت لكل جولة مزدوجة)
    static const __m128i SHA_K[] = {
        _mm_set_epi32(0xe9b5dba5, 0xb5c0fbcf, 0x71374491, 0x428a2f98),
        _mm_set_epi32(0xab1c5ed5, 0x923f82a4, 0x59f111f1, 0x3956c25b),
        _mm_set_epi32(0x550c7dc3, 0x243185be, 0x12835b01, 0xd807aa98),
        _mm_set_epi32(0xc19bf174, 0x9bdc06a7, 0x80deb1fe, 0x72be5d74),
        _mm_set_epi32(0x240ca1cc, 0x0fc19dc6, 0xefbe4786, 0xe49b69c1),
        _mm_set_epi32(0x76f988da, 0x5cb0a9dc, 0x4a7484aa, 0x2de92c6f),
        _mm_set_epi32(0xbf597fc7, 0xb00327c8, 0xa831c66d, 0x983e5152),
        _mm_set_epi32(0x14292967, 0x06ca6351, 0xd5a79147, 0xc6e00bf3),
        _mm_set_epi32(0x53380d13, 0x4d2c6dfc, 0x2e1b2138, 0x27b70a85),
        _mm_set_epi32(0x92722c85, 0x81c2c92e, 0x766a0abb, 0x650a7354),
        _mm_set_epi32(0xc76c51a3, 0xc24b8b70, 0xa81a664b, 0xa2bfe8a1),
        _mm_set_epi32(0x106aa070, 0xf40e3585, 0xd6990624, 0xd192e819),
        _mm_set_epi32(0x34b0bcb5, 0x2748774c, 0x1e376c08, 0x19a4c116),
        _mm_set_epi32(0x682e6ff3, 0x5b9cca4f, 0x4ed8aa4a, 0x391c0cb3),
        _mm_set_epi32(0x8cc70208, 0x84c87814, 0x78a5636f, 0x748f82ee),
        _mm_set_epi32(0xc67178f2, 0xbef9a3f7, 0xa4506ceb, 0x90befffa),
    };

    // (AR) عكس ترتيب البايتات (big-endian to little-endian)
    static const __m128i SHA_SHUF_MASK = _mm_set_epi64x(
        0x0c0d0e0f08090a0bULL, 0x0405060700010203ULL);

    /// معالجة كتلة 512 بت بـ SHA-NI
    SADNET_SHANI_TARGET
    static void sha256_transform_shani(SHA256State &state)
    {
        __m128i STATE0, STATE1;
        __m128i MSG, TMP;
        __m128i MSG0, MSG1, MSG2, MSG3;
        __m128i ABEF_SAVE, CDGH_SAVE;

        // (AR) تحميل الحالة الحالية
        // STATE0 = [A, B, E, F]  STATE1 = [C, D, G, H]
        // لكن _mm_sha256rnds2 يتوقع: STATE0 = [C,D,G,H], STATE1 = [A,B,E,F]
        // والترتيب الداخلي: [3,2,1,0] = [أقل..أعلى]
        TMP = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&state.h[0]));    // A B C D
        STATE1 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&state.h[4])); // E F G H

        // (AR) إعادة ترتيب: نحتاج STATE0=[A,B,E,F], STATE1=[C,D,G,H]
        TMP = _mm_shuffle_epi32(TMP, 0xB1);          // B A D C
        STATE1 = _mm_shuffle_epi32(STATE1, 0x1B);    // H G F E
        STATE0 = _mm_alignr_epi8(TMP, STATE1, 8);    // A B F E → [D,C,B,A] style
        STATE1 = _mm_blend_epi16(STATE1, TMP, 0xF0); // C D G H

        // حفظ الحالة الأولية
        ABEF_SAVE = STATE0;
        CDGH_SAVE = STATE1;

        // (AR) تحميل الرسالة مع عكس البايتات (big-endian → little-endian)
        MSG0 = _mm_shuffle_epi8(_mm_loadu_si128(reinterpret_cast<const __m128i *>(state.block)), SHA_SHUF_MASK);
        MSG1 = _mm_shuffle_epi8(_mm_loadu_si128(reinterpret_cast<const __m128i *>(state.block + 16)), SHA_SHUF_MASK);
        MSG2 = _mm_shuffle_epi8(_mm_loadu_si128(reinterpret_cast<const __m128i *>(state.block + 32)), SHA_SHUF_MASK);
        MSG3 = _mm_shuffle_epi8(_mm_loadu_si128(reinterpret_cast<const __m128i *>(state.block + 48)), SHA_SHUF_MASK);

        // (AR) الجولات 0-3
        MSG = _mm_add_epi32(MSG0, SHA_K[0]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        // الجولات 4-7
        MSG = _mm_add_epi32(MSG1, SHA_K[1]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

        // الجولات 8-11
        MSG = _mm_add_epi32(MSG2, SHA_K[2]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

        // الجولات 12-15
        MSG = _mm_add_epi32(MSG3, SHA_K[3]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
        MSG0 = _mm_add_epi32(MSG0, TMP);
        MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

        // (AR) الجولات 16-19
        MSG = _mm_add_epi32(MSG0, SHA_K[4]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        // الجولات 20-23
        MSG = _mm_add_epi32(MSG1, SHA_K[5]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
        MSG2 = _mm_add_epi32(MSG2, TMP);
        MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

        // الجولات 24-27
        MSG = _mm_add_epi32(MSG2, SHA_K[6]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
        MSG3 = _mm_add_epi32(MSG3, TMP);
        MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

        // الجولات 28-31
        MSG = _mm_add_epi32(MSG3, SHA_K[7]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
        MSG0 = _mm_add_epi32(MSG0, TMP);
        MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

        // الجولات 32-35
        MSG = _mm_add_epi32(MSG0, SHA_K[8]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        // الجولات 36-39
        MSG = _mm_add_epi32(MSG1, SHA_K[9]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
        MSG2 = _mm_add_epi32(MSG2, TMP);
        MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG0 = _mm_sha256msg1_epu32(MSG0, MSG1);

        // الجولات 40-43
        MSG = _mm_add_epi32(MSG2, SHA_K[10]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
        MSG3 = _mm_add_epi32(MSG3, TMP);
        MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG1 = _mm_sha256msg1_epu32(MSG1, MSG2);

        // الجولات 44-47
        MSG = _mm_add_epi32(MSG3, SHA_K[11]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG3, MSG2, 4);
        MSG0 = _mm_add_epi32(MSG0, TMP);
        MSG0 = _mm_sha256msg2_epu32(MSG0, MSG3);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG2 = _mm_sha256msg1_epu32(MSG2, MSG3);

        // الجولات 48-51
        MSG = _mm_add_epi32(MSG0, SHA_K[12]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG0, MSG3, 4);
        MSG1 = _mm_add_epi32(MSG1, TMP);
        MSG1 = _mm_sha256msg2_epu32(MSG1, MSG0);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);
        MSG3 = _mm_sha256msg1_epu32(MSG3, MSG0);

        // الجولات 52-55
        MSG = _mm_add_epi32(MSG1, SHA_K[13]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG1, MSG0, 4);
        MSG2 = _mm_add_epi32(MSG2, TMP);
        MSG2 = _mm_sha256msg2_epu32(MSG2, MSG1);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        // الجولات 56-59
        MSG = _mm_add_epi32(MSG2, SHA_K[14]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        TMP = _mm_alignr_epi8(MSG2, MSG1, 4);
        MSG3 = _mm_add_epi32(MSG3, TMP);
        MSG3 = _mm_sha256msg2_epu32(MSG3, MSG2);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        // الجولات 60-63
        MSG = _mm_add_epi32(MSG3, SHA_K[15]);
        STATE1 = _mm_sha256rnds2_epu32(STATE1, STATE0, MSG);
        MSG = _mm_shuffle_epi32(MSG, 0x0E);
        STATE0 = _mm_sha256rnds2_epu32(STATE0, STATE1, MSG);

        // (AR) إضافة الحالة المحفوظة
        STATE0 = _mm_add_epi32(STATE0, ABEF_SAVE);
        STATE1 = _mm_add_epi32(STATE1, CDGH_SAVE);

        // (AR) استخراج الحالة: STATE0=[A,B,E,F], STATE1=[C,D,G,H] → h[0..7]
        TMP = _mm_shuffle_epi32(STATE0, 0x1B);       // E F B A → need ABCD
        STATE1 = _mm_shuffle_epi32(STATE1, 0xB1);    // D C H G
        STATE0 = _mm_blend_epi16(TMP, STATE1, 0xF0); // A B C D
        STATE1 = _mm_alignr_epi8(STATE1, TMP, 8);    // E F G H

        _mm_storeu_si128(reinterpret_cast<__m128i *>(&state.h[0]), STATE0);
        _mm_storeu_si128(reinterpret_cast<__m128i *>(&state.h[4]), STATE1);
    }
#endif // SADNET_X86_INTRINSICS

    // (AR) دالة التحويل الموحدة — تختار SHA-NI إن دعم المعالج أو البرمجية
    static void sha256_dispatch(SHA256State &state)
    {
#if SADNET_X86_INTRINSICS
        if (HAS_SHA_NI)
        {
            sha256_transform_shani(state);
            return;
        }
#endif
        sha256_transform(state);
    }

    /// إضافة بيانات لحالة SHA-256 (مُحسَّنة — نسخ كتل كاملة بدلاً من بايت-بايت)
    static void sha256_update(SHA256State &state, const uint8_t *data, size_t len)
    {
        size_t offset = 0;

        // (AR) إذا كانت هناك بقايا في الكتلة الحالية، أكملها أولاً
        if (state.block_len > 0)
        {
            size_t needed = 64 - state.block_len;
            size_t copy = (len < needed) ? len : needed;
            std::memcpy(state.block + state.block_len, data, copy);
            state.block_len += copy;
            state.total_len += copy;
            offset += copy;

            if (state.block_len == 64)
            {
                sha256_dispatch(state);
                state.block_len = 0;
            }
        }

        // (AR) معالجة كتل كاملة مباشرة (64 بايت لكل كتلة)
        while (offset + 64 <= len)
        {
            std::memcpy(state.block, data + offset, 64);
            state.total_len += 64;
            sha256_dispatch(state);
            offset += 64;
        }

        // (AR) نسخ الباقي إلى الكتلة
        size_t remaining = len - offset;
        if (remaining > 0)
        {
            std::memcpy(state.block, data + offset, remaining);
            state.block_len = remaining;
            state.total_len += remaining;
        }
    }

    /// إنهاء وإنتاج البصمة النهائية
    static Hash256 sha256_final(SHA256State &state)
    {
        // حفظ الطول بالبتّات (big-endian)
        uint64_t bit_len = state.total_len * 8;

        // إضافة بت 1 ثم أصفار
        state.block[state.block_len++] = 0x80;

        // إذا لم يتبقى مساحة كافية للطول (8 بايت)
        if (state.block_len > 56)
        {
            while (state.block_len < 64)
                state.block[state.block_len++] = 0;
            sha256_dispatch(state);
            state.block_len = 0;
        }

        // ملء بأصفار حتى 56 بايت
        while (state.block_len < 56)
            state.block[state.block_len++] = 0;

        // كتابة الطول بالبتّات (big-endian, 64 بت)
        for (int i = 7; i >= 0; i--)
        {
            state.block[state.block_len++] =
                static_cast<uint8_t>((bit_len >> (i * 8)) & 0xFF);
        }

        sha256_dispatch(state);

        // استخراج البصمة (big-endian)
        Hash256 result;
        for (int i = 0; i < 8; i++)
        {
            result[i * 4] = static_cast<uint8_t>((state.h[i] >> 24) & 0xFF);
            result[i * 4 + 1] = static_cast<uint8_t>((state.h[i] >> 16) & 0xFF);
            result[i * 4 + 2] = static_cast<uint8_t>((state.h[i] >> 8) & 0xFF);
            result[i * 4 + 3] = static_cast<uint8_t>((state.h[i]) & 0xFF);
        }
        return result;
    }

    // ─────────────────────────────────────────────────────────────────────
    // (AR) واجهات SHA-256 العامة
    // ─────────────────────────────────────────────────────────────────────

    Hash256 sha256(const uint8_t *data, size_t len)
    {
        SHA256State state;
        sha256_update(state, data, len);
        return sha256_final(state);
    }

    Hash256 sha256(const Buffer &buf)
    {
        return sha256(buf.data(), buf.size());
    }

    Hash256 sha256(const std::string &str)
    {
        return sha256(reinterpret_cast<const uint8_t *>(str.data()), str.size());
    }

    // ─────────────────────────────────────────────────────────────────────
    // (AR) HMAC-SHA256 حسب RFC 2104
    // ─────────────────────────────────────────────────────────────────────

    Hash256 hmac_sha256(const uint8_t *key, size_t key_len,
                        const uint8_t *data, size_t data_len)
    {
        // إذا كان المفتاح أطول من 64 بايت، نُصغّره بـ SHA-256
        uint8_t padded_key[64];
        std::memset(padded_key, 0, 64);

        if (key_len > 64)
        {
            auto hashed = sha256(key, key_len);
            std::memcpy(padded_key, hashed.data(), 32);
        }
        else
        {
            std::memcpy(padded_key, key, key_len);
        }

        // حساب المفتاح الداخلي والخارجي
        uint8_t i_key_pad[64], o_key_pad[64];
        for (int i = 0; i < 64; i++)
        {
            i_key_pad[i] = padded_key[i] ^ 0x36;
            o_key_pad[i] = padded_key[i] ^ 0x5C;
        }

        // HMAC = SHA256(o_key_pad || SHA256(i_key_pad || data))
        SHA256State inner;
        sha256_update(inner, i_key_pad, 64);
        sha256_update(inner, data, data_len);
        auto inner_hash = sha256_final(inner);

        SHA256State outer;
        sha256_update(outer, o_key_pad, 64);
        sha256_update(outer, inner_hash.data(), 32);
        return sha256_final(outer);
    }

    // ─────────────────────────────────────────────────────────────────────
    // (AR) HKDF-SHA256 حسب RFC 5869
    // ─────────────────────────────────────────────────────────────────────

    Buffer hkdf_sha256(const uint8_t *salt, size_t salt_len,
                       const uint8_t *ikm, size_t ikm_len,
                       const uint8_t *info, size_t info_len,
                       size_t out_len)
    {
        // الخطوة 1: الاستخراج — PRK = HMAC-SHA256(salt, IKM)
        const uint8_t default_salt[32] = {};
        if (salt == nullptr || salt_len == 0)
        {
            salt = default_salt;
            salt_len = 32;
        }
        auto prk = hmac_sha256(salt, salt_len, ikm, ikm_len);

        // الخطوة 2: التوسيع — T(i) = HMAC-SHA256(PRK, T(i-1) || info || i)
        Buffer output;
        output.resize(out_len);

        uint8_t T[32] = {}; // T(0) فارغ
        size_t T_len = 0;
        size_t pos = 0;

        for (uint8_t i = 1; pos < out_len; i++)
        {
            // بناء المدخل: T(i-1) || info || byte(i)
            std::vector<uint8_t> input;
            input.insert(input.end(), T, T + T_len);
            if (info && info_len > 0)
                input.insert(input.end(), info, info + info_len);
            input.push_back(i);

            auto Ti = hmac_sha256(prk.data(), 32,
                                  input.data(), input.size());

            size_t copy_len = std::min(size_t(32), out_len - pos);
            std::memcpy(output.data() + pos, Ti.data(), copy_len);
            std::memcpy(T, Ti.data(), 32);
            T_len = 32;
            pos += copy_len;
        }

        return output;
    }

} // namespace sad::net::crypto
