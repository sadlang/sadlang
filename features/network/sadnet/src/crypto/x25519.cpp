// ===================================================================
// صNet — تنفيذ X25519 (Curve25519 Diffie-Hellman) + Double Ratchet
// (AR) تبادل مفاتيح لامركزي على المنحنى البيضاوي Curve25519
//      + بروتوكول السقّالة المزدوجة (Signal Protocol) لتشفير الجلسات
//      تنفيذ ذاتي بالكامل — حساب حقل أولي mod p = 2^255 - 19
// (EN) X25519 key exchange + Double Ratchet session protocol
// المكون: features/network/sadnet (تبادل مفاتيح X25519)
// المسار: features/network/sadnet/src/crypto/x25519.cpp
// ===================================================================

#include "sadnet/crypto/crypto.h"
#include <cstring>
#include <memory>
#include <vector>

// (AR) MSVC لا يدعم __uint128_t. نستخدم بديلاً مُحسَّناً مع _umul128 intrinsic
//      _umul128 تعطي ضرب 64×64→128 في تعليمة واحدة (mul r64)
#ifdef _MSC_VER
#include <intrin.h>
struct uint128_t
{
    uint64_t lo, hi;
    uint128_t() : lo(0), hi(0) {}
    uint128_t(uint64_t v) : lo(v), hi(0) {}
    uint128_t &operator+=(const uint128_t &o)
    {
        uint64_t old = lo;
        lo += o.lo;
        hi += o.hi + (lo < old ? 1 : 0);
        return *this;
    }
    uint128_t operator*(uint64_t v) const
    {
        // this->lo * v مع مراعاة this->hi * v
        uint64_t r_hi;
        uint64_t r_lo = _umul128(lo, v, &r_hi);
        r_hi += hi * v;
        uint128_t r;
        r.lo = r_lo;
        r.hi = r_hi;
        return r;
    }
    explicit operator uint64_t() const { return lo; }
    uint128_t operator>>(int n) const
    {
        if (n == 0)
            return *this;
        uint128_t r;
        if (n < 64)
        {
            r.lo = (lo >> n) | (hi << (64 - n));
            r.hi = hi >> n;
        }
        else
        {
            r.lo = hi >> (n - 64);
            r.hi = 0;
        }
        return r;
    }
    uint128_t operator&(uint64_t mask) const
    {
        uint128_t r;
        r.lo = lo & mask;
        r.hi = 0;
        return r;
    }
    // (AR) ضرب 64×64→128 عبر تعليمة مضمنة واحدة
    static uint128_t mul64(uint64_t a, uint64_t b)
    {
        uint128_t r;
        r.lo = _umul128(a, b, &r.hi);
        return r;
    }
};
#endif

namespace sad::net::crypto
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) حساب حسابي في الحقل GF(2^255 - 19) — حقل Curve25519
    //      جميع العمليات mod p حيث p = 2^255 - 19
    //      نستخدم تمثيل limbs: 5 أطراف × 51 بت (في uint64_t)
    // ═════════════════════════════════════════════════════════════════════

    /// تمثيل عنصر في الحقل بـ 5 أطراف
    struct Fe
    {
        uint64_t v[5] = {};
    };

    /// p = 2^255 - 19
    static const uint64_t P_LIMBS[5] = {
        0x7FFFFFFFFFFFF, // 2^51 - 1 ... - 19 on last limb
        0x7FFFFFFFFFFFF,
        0x7FFFFFFFFFFFF,
        0x7FFFFFFFFFFFF,
        0x7FFFFFFFFFFFF};

    static constexpr uint64_t MASK51 = (1ULL << 51) - 1;

    /// تحويل 32 بايت (little-endian) إلى عنصر حقل
    static Fe fe_from_bytes(const uint8_t b[32])
    {
        Fe r;
        r.v[0] = (uint64_t(b[0])) | (uint64_t(b[1]) << 8) | (uint64_t(b[2]) << 16) | (uint64_t(b[3]) << 24) | (uint64_t(b[4]) << 32) | (uint64_t(b[5]) << 40) | ((uint64_t(b[6]) & 0x07) << 48);
        r.v[0] &= MASK51;

        r.v[1] = (uint64_t(b[6]) >> 3) | (uint64_t(b[7]) << 5) | (uint64_t(b[8]) << 13) | (uint64_t(b[9]) << 21) | (uint64_t(b[10]) << 29) | (uint64_t(b[11]) << 37) | ((uint64_t(b[12]) & 0x3F) << 45);
        r.v[1] &= MASK51;

        r.v[2] = (uint64_t(b[12]) >> 6) | (uint64_t(b[13]) << 2) | (uint64_t(b[14]) << 10) | (uint64_t(b[15]) << 18) | (uint64_t(b[16]) << 26) | (uint64_t(b[17]) << 34) | (uint64_t(b[18]) << 42) | ((uint64_t(b[19]) & 0x01) << 50);
        r.v[2] &= MASK51;

        r.v[3] = (uint64_t(b[19]) >> 1) | (uint64_t(b[20]) << 7) | (uint64_t(b[21]) << 15) | (uint64_t(b[22]) << 23) | (uint64_t(b[23]) << 31) | (uint64_t(b[24]) << 39) | ((uint64_t(b[25]) & 0x0F) << 47);
        r.v[3] &= MASK51;

        r.v[4] = (uint64_t(b[25]) >> 4) | (uint64_t(b[26]) << 4) | (uint64_t(b[27]) << 12) | (uint64_t(b[28]) << 20) | (uint64_t(b[29]) << 28) | (uint64_t(b[30]) << 36) | ((uint64_t(b[31]) & 0x7F) << 44);
        r.v[4] &= MASK51;

        return r;
    }

    /// تحويل عنصر حقل إلى 32 بايت (little-endian)
    /// (AR) تطبيع كامل mod p ثم ترميز الأطراف الخمسة إلى 256 بت
    static void fe_to_bytes(const Fe &f, uint8_t out[32])
    {
        // تطبيع (reduce mod p)
        Fe t = f;
        // عدة مراحل carries لضمان كل limb < 2^51
        for (int round = 0; round < 3; round++)
        {
            uint64_t carry;
            carry = t.v[0] >> 51;
            t.v[0] &= MASK51;
            t.v[1] += carry;
            carry = t.v[1] >> 51;
            t.v[1] &= MASK51;
            t.v[2] += carry;
            carry = t.v[2] >> 51;
            t.v[2] &= MASK51;
            t.v[3] += carry;
            carry = t.v[3] >> 51;
            t.v[3] &= MASK51;
            t.v[4] += carry;
            carry = t.v[4] >> 51;
            t.v[4] &= MASK51;
            t.v[0] += carry * 19;
        }

        // التقليل النهائي: إذا كان >= p، اطرح p
        // نحسب t - p ونرى إذا كانت النتيجة غير سالبة
        uint64_t carry = (t.v[0] + 19) >> 51;
        carry = (t.v[1] + carry) >> 51;
        carry = (t.v[2] + carry) >> 51;
        carry = (t.v[3] + carry) >> 51;
        carry = (t.v[4] + carry) >> 51;
        // carry = 1 يعني t >= p، فنطرح p (نضيف 19 ثم carry)
        t.v[0] += 19 * carry;
        carry = t.v[0] >> 51;
        t.v[0] &= MASK51;
        t.v[1] += carry;
        carry = t.v[1] >> 51;
        t.v[1] &= MASK51;
        t.v[2] += carry;
        carry = t.v[2] >> 51;
        t.v[2] &= MASK51;
        t.v[3] += carry;
        carry = t.v[3] >> 51;
        t.v[3] &= MASK51;
        t.v[4] += carry;
        t.v[4] &= MASK51;

        // (AR) ترميز 5 أطراف × 51 بت إلى 32 بايت little-endian
        //      عمليات shift/OR مباشرة — بدون حلقة bit-by-bit
        //      h = v[0] + v[1]*2^51 + v[2]*2^102 + v[3]*2^153 + v[4]*2^204
        //
        // توزيع الأطراف على البايتات:
        //   limb 0: بت 0..50   → بايت 0..6  (بايت 6 يشترك فيه 3 بت)
        //   limb 1: بت 51..101 → بايت 6..12 (بايت 6: 5 بت، بايت 12: 6 بت)
        //   limb 2: بت 102..152 → بايت 12..19 (بايت 12: 2 بت، بايت 19: 1 بت)
        //   limb 3: بت 153..203 → بايت 19..25 (بايت 19: 7 بت، بايت 25: 4 بت)
        //   limb 4: بت 204..254 → بايت 25..31 (بايت 25: 4 بت)

        // (AR) نبني 4 قيم 64-بت تغطي 256 بت
        // q[0] = بت 0..63:   v[0](51 بت) | v[1] << 51 (13 بت فقط تناسب)
        // q[1] = بت 64..127: v[1] >> 13 (38 بت) | v[2] << 38 (26 بت)
        // q[2] = بت 128..191: v[2] >> 26 (25 بت) | v[3] << 25 (39 بت)
        // q[3] = بت 192..255: v[3] >> 39 (12 بت) | v[4] << 12 (51 بت، فقط 52 مهم)
        uint64_t q0 = t.v[0] | (t.v[1] << 51);
        uint64_t q1 = (t.v[1] >> 13) | (t.v[2] << 38);
        uint64_t q2 = (t.v[2] >> 26) | (t.v[3] << 25);
        uint64_t q3 = (t.v[3] >> 39) | (t.v[4] << 12);

        // (AR) ترميز 4 × uint64_t إلى 32 بايت little-endian
        for (int i = 0; i < 8; i++)
            out[i] = (uint8_t)(q0 >> (8 * i));
        for (int i = 0; i < 8; i++)
            out[8 + i] = (uint8_t)(q1 >> (8 * i));
        for (int i = 0; i < 8; i++)
            out[16 + i] = (uint8_t)(q2 >> (8 * i));
        for (int i = 0; i < 8; i++)
            out[24 + i] = (uint8_t)(q3 >> (8 * i));
    }

    /// جمع في الحقل: a + b mod p
    static Fe fe_add(const Fe &a, const Fe &b)
    {
        Fe r;
        for (int i = 0; i < 5; i++)
            r.v[i] = a.v[i] + b.v[i];
        return r;
    }

    /// طرح في الحقل: a - b mod p (نضيف مضاعف p لمنع underflow)
    static Fe fe_sub(const Fe &a, const Fe &b)
    {
        Fe r;
        // نضيف 2p لضمان عدم السلبية
        static const uint64_t TWO_P[5] = {
            0xFFFFFFFFFFFDA, 0xFFFFFFFFFFFFE,
            0xFFFFFFFFFFFFE, 0xFFFFFFFFFFFFE, 0xFFFFFFFFFFFFE};
        for (int i = 0; i < 5; i++)
            r.v[i] = a.v[i] + TWO_P[i] - b.v[i];
        return r;
    }

    /// ضرب في الحقل: a * b mod p
    /// نستخدم ضرب 128-بت عبر تجزئة إلى 64-بت
    static Fe fe_mul(const Fe &a, const Fe &b)
    {
        // ضرب schoolbook مع 5 أطراف
        // كل حاصل ضرب limb×limb يناسب uint128 (51+51 = 102 بت)
        // نستخدم uint64_t فقط مع حمل
        Fe r;

        // (AR) ضرب مفكوك الحلقات (unrolled) — donna64 المرجعي
        //      نحسب المضاعفات ×19 مسبقاً لتجنب التفرعات
        //      كل مراكم t[k] = Σ a[i]*b[j] حيث (i+j) mod 5 = k
        //      المنتجات مع k >= 5 تُختزل بـ ×19 (لأن 2^255 ≡ 19 mod p)

        uint64_t b1_19 = b.v[1] * 19;
        uint64_t b2_19 = b.v[2] * 19;
        uint64_t b3_19 = b.v[3] * 19;
        uint64_t b4_19 = b.v[4] * 19;

#ifdef _MSC_VER
        // t[0] = a0*b0 + a1*b4*19 + a2*b3*19 + a3*b2*19 + a4*b1*19
        uint128_t t0 = uint128_t::mul64(a.v[0], b.v[0]);
        t0 += uint128_t::mul64(a.v[1], b4_19);
        t0 += uint128_t::mul64(a.v[2], b3_19);
        t0 += uint128_t::mul64(a.v[3], b2_19);
        t0 += uint128_t::mul64(a.v[4], b1_19);

        // t[1] = a0*b1 + a1*b0 + a2*b4*19 + a3*b3*19 + a4*b2*19
        uint128_t t1 = uint128_t::mul64(a.v[0], b.v[1]);
        t1 += uint128_t::mul64(a.v[1], b.v[0]);
        t1 += uint128_t::mul64(a.v[2], b4_19);
        t1 += uint128_t::mul64(a.v[3], b3_19);
        t1 += uint128_t::mul64(a.v[4], b2_19);

        // t[2] = a0*b2 + a1*b1 + a2*b0 + a3*b4*19 + a4*b3*19
        uint128_t t2 = uint128_t::mul64(a.v[0], b.v[2]);
        t2 += uint128_t::mul64(a.v[1], b.v[1]);
        t2 += uint128_t::mul64(a.v[2], b.v[0]);
        t2 += uint128_t::mul64(a.v[3], b4_19);
        t2 += uint128_t::mul64(a.v[4], b3_19);

        // t[3] = a0*b3 + a1*b2 + a2*b1 + a3*b0 + a4*b4*19
        uint128_t t3 = uint128_t::mul64(a.v[0], b.v[3]);
        t3 += uint128_t::mul64(a.v[1], b.v[2]);
        t3 += uint128_t::mul64(a.v[2], b.v[1]);
        t3 += uint128_t::mul64(a.v[3], b.v[0]);
        t3 += uint128_t::mul64(a.v[4], b4_19);

        // t[4] = a0*b4 + a1*b3 + a2*b2 + a3*b1 + a4*b0
        uint128_t t4 = uint128_t::mul64(a.v[0], b.v[4]);
        t4 += uint128_t::mul64(a.v[1], b.v[3]);
        t4 += uint128_t::mul64(a.v[2], b.v[2]);
        t4 += uint128_t::mul64(a.v[3], b.v[1]);
        t4 += uint128_t::mul64(a.v[4], b.v[0]);

        // (AR) سلسلة الحوامل (carry chain)
        uint64_t carry;
        r.v[0] = (uint64_t)t0 & MASK51;
        carry = (uint64_t)(t0 >> 51);
        t1 += uint128_t(carry);
        r.v[1] = (uint64_t)t1 & MASK51;
        carry = (uint64_t)(t1 >> 51);
        t2 += uint128_t(carry);
        r.v[2] = (uint64_t)t2 & MASK51;
        carry = (uint64_t)(t2 >> 51);
        t3 += uint128_t(carry);
        r.v[3] = (uint64_t)t3 & MASK51;
        carry = (uint64_t)(t3 >> 51);
        t4 += uint128_t(carry);
        r.v[4] = (uint64_t)t4 & MASK51;
        carry = (uint64_t)(t4 >> 51);
#else
        // (AR) نفس الخوارزمية مع __uint128_t (GCC/Clang)
        __uint128_t t0 = (__uint128_t)a.v[0] * b.v[0] + (__uint128_t)a.v[1] * b4_19 + (__uint128_t)a.v[2] * b3_19 + (__uint128_t)a.v[3] * b2_19 + (__uint128_t)a.v[4] * b1_19;

        __uint128_t t1 = (__uint128_t)a.v[0] * b.v[1] + (__uint128_t)a.v[1] * b.v[0] + (__uint128_t)a.v[2] * b4_19 + (__uint128_t)a.v[3] * b3_19 + (__uint128_t)a.v[4] * b2_19;

        __uint128_t t2 = (__uint128_t)a.v[0] * b.v[2] + (__uint128_t)a.v[1] * b.v[1] + (__uint128_t)a.v[2] * b.v[0] + (__uint128_t)a.v[3] * b4_19 + (__uint128_t)a.v[4] * b3_19;

        __uint128_t t3 = (__uint128_t)a.v[0] * b.v[3] + (__uint128_t)a.v[1] * b.v[2] + (__uint128_t)a.v[2] * b.v[1] + (__uint128_t)a.v[3] * b.v[0] + (__uint128_t)a.v[4] * b4_19;

        __uint128_t t4 = (__uint128_t)a.v[0] * b.v[4] + (__uint128_t)a.v[1] * b.v[3] + (__uint128_t)a.v[2] * b.v[2] + (__uint128_t)a.v[3] * b.v[1] + (__uint128_t)a.v[4] * b.v[0];

        uint64_t carry;
        r.v[0] = (uint64_t)t0 & MASK51;
        carry = (uint64_t)(t0 >> 51);
        t1 += carry;
        r.v[1] = (uint64_t)t1 & MASK51;
        carry = (uint64_t)(t1 >> 51);
        t2 += carry;
        r.v[2] = (uint64_t)t2 & MASK51;
        carry = (uint64_t)(t2 >> 51);
        t3 += carry;
        r.v[3] = (uint64_t)t3 & MASK51;
        carry = (uint64_t)(t3 >> 51);
        t4 += carry;
        r.v[4] = (uint64_t)t4 & MASK51;
        carry = (uint64_t)(t4 >> 51);
#endif
        // (AR) إضافة carry * 19 مع نشر الحمل كاملاً (مطابق لـ donna64)
        // بدون هذا، الأطراف تنمو وتتسبب في فيضان uint128_t في الضرب اللاحق
        r.v[0] += carry * 19;
        carry = r.v[0] >> 51;
        r.v[0] &= MASK51;
        r.v[1] += carry;

        return r;
    }

    /// تربيع في الحقل: a^2 mod p
    /// (AR) تنفيذ مُحسَّن يستغل تناظر a[i]*a[j] = a[j]*a[i]
    ///      بدلاً من 25 ضرب (5×5)، نحسب 15 فقط (القطر + المثلث العلوي × 2)
    ///      مطابق لنمط donna64/ref10 المرجعي
    static Fe fe_sq(const Fe &a)
    {
        Fe r;
        // (AR) الأطراف d[i] = a[i] * 2 — نستخدمها لتوفير ضرب واحد في كل حاصل متقاطع
        uint64_t d0 = a.v[0] * 2;
        uint64_t d1 = a.v[1] * 2;
        uint64_t d2 = a.v[2] * 2;
        uint64_t d3 = a.v[3] * 2;
        // d4 ليست مطلوبة (لا يوجد a[4]*a[j>4])

        // (AR) الحاصلات المتقاطعة التي تُختزل بـ ×19 (k >= 5)
        uint64_t a3_19 = a.v[3] * 19;
        uint64_t a4_19 = a.v[4] * 19;

#ifdef _MSC_VER
        // (AR) تجميع المراكمات — كل t[k] يمثل مجموع الحاصلات عند الموقع k
        //      الحساب المباشر بدون حلقات لأقصى أداء

        // t[0] = a0*a0 + 2*(a1*a4)*19 + 2*(a2*a3)*19
        uint128_t t0 = uint128_t::mul64(a.v[0], a.v[0]);
        t0 += uint128_t::mul64(d1, a4_19);
        t0 += uint128_t::mul64(d2, a3_19);

        // t[1] = 2*a0*a1 + 2*(a2*a4)*19 + a3*a3*19
        uint128_t t1 = uint128_t::mul64(d0, a.v[1]);
        t1 += uint128_t::mul64(d2, a4_19);
        t1 += uint128_t::mul64(a.v[3], a3_19);

        // t[2] = 2*a0*a2 + a1*a1 + 2*(a3*a4)*19
        uint128_t t2 = uint128_t::mul64(d0, a.v[2]);
        t2 += uint128_t::mul64(a.v[1], a.v[1]);
        t2 += uint128_t::mul64(d3, a4_19);

        // t[3] = 2*a0*a3 + 2*a1*a2 + a4*a4*19
        uint128_t t3 = uint128_t::mul64(d0, a.v[3]);
        t3 += uint128_t::mul64(d1, a.v[2]);
        t3 += uint128_t::mul64(a.v[4], a4_19);

        // t[4] = 2*a0*a4 + 2*a1*a3 + a2*a2
        uint128_t t4 = uint128_t::mul64(d0, a.v[4]);
        t4 += uint128_t::mul64(d1, a.v[3]);
        t4 += uint128_t::mul64(a.v[2], a.v[2]);

        // (AR) سلسلة الحوامل (carry chain) — مطابقة لـ donna64
        uint64_t carry;
        r.v[0] = (uint64_t)t0 & MASK51;
        carry = (uint64_t)(t0 >> 51);
        t1 += uint128_t(carry);
        r.v[1] = (uint64_t)t1 & MASK51;
        carry = (uint64_t)(t1 >> 51);
        t2 += uint128_t(carry);
        r.v[2] = (uint64_t)t2 & MASK51;
        carry = (uint64_t)(t2 >> 51);
        t3 += uint128_t(carry);
        r.v[3] = (uint64_t)t3 & MASK51;
        carry = (uint64_t)(t3 >> 51);
        t4 += uint128_t(carry);
        r.v[4] = (uint64_t)t4 & MASK51;
        carry = (uint64_t)(t4 >> 51);
        // (AR) الحمل النهائي — إعادة ×19 مع نشر كامل
        r.v[0] += carry * 19;
        carry = r.v[0] >> 51;
        r.v[0] &= MASK51;
        r.v[1] += carry;
#else
        // (AR) نفس الخوارزمية مع __uint128_t الأصلية (GCC/Clang)
        __uint128_t t0 = (__uint128_t)a.v[0] * a.v[0] + (__uint128_t)d1 * a4_19 + (__uint128_t)d2 * a3_19;

        __uint128_t t1 = (__uint128_t)d0 * a.v[1] + (__uint128_t)d2 * a4_19 + (__uint128_t)a.v[3] * a3_19;

        __uint128_t t2 = (__uint128_t)d0 * a.v[2] + (__uint128_t)a.v[1] * a.v[1] + (__uint128_t)d3 * a4_19;

        __uint128_t t3 = (__uint128_t)d0 * a.v[3] + (__uint128_t)d1 * a.v[2] + (__uint128_t)a.v[4] * a4_19;

        __uint128_t t4 = (__uint128_t)d0 * a.v[4] + (__uint128_t)d1 * a.v[3] + (__uint128_t)a.v[2] * a.v[2];

        uint64_t carry;
        r.v[0] = (uint64_t)t0 & MASK51;
        carry = (uint64_t)(t0 >> 51);
        t1 += carry;
        r.v[1] = (uint64_t)t1 & MASK51;
        carry = (uint64_t)(t1 >> 51);
        t2 += carry;
        r.v[2] = (uint64_t)t2 & MASK51;
        carry = (uint64_t)(t2 >> 51);
        t3 += carry;
        r.v[3] = (uint64_t)t3 & MASK51;
        carry = (uint64_t)(t3 >> 51);
        t4 += carry;
        r.v[4] = (uint64_t)t4 & MASK51;
        carry = (uint64_t)(t4 >> 51);
        r.v[0] += carry * 19;
        carry = r.v[0] >> 51;
        r.v[0] &= MASK51;
        r.v[1] += carry;
#endif
        return r;
    }

    /// حساب المعكوس: a^(-1) = a^(p-2) mod p
    /// تنفيذ مطابق لسلسلة التربيعات المرجعية (مثل donna / TweetNaCl)
    /// p - 2 = 2^255 - 21
    /// نحسب z^(p-2) عبر سلسلة addition chain دقيقة
    static Fe fe_invert(const Fe &z)
    {
        Fe z2 = fe_sq(z); // z^2
        Fe z9, z11;
        {
            Fe t;
            t = fe_sq(z2);        // z^4
            t = fe_sq(t);         // z^8
            z9 = fe_mul(t, z);    // z^9
            z11 = fe_mul(z9, z2); // z^11
        }
        Fe z_5_0; // z^(2^5 - 1) = z^31
        {
            Fe t = fe_sq(z11);     // z^22
            z_5_0 = fe_mul(t, z9); // z^31
        }
        Fe z_10_0; // z^(2^10 - 1)
        {
            Fe t = z_5_0;
            for (int i = 0; i < 5; i++)
                t = fe_sq(t);          // z^(31 * 2^5) = z^(2^10 - 2^5)
            z_10_0 = fe_mul(t, z_5_0); // z^(2^10 - 1)
        }
        Fe z_20_0; // z^(2^20 - 1)
        {
            Fe t = z_10_0;
            for (int i = 0; i < 10; i++)
                t = fe_sq(t);           // z^(2^20 - 2^10)
            z_20_0 = fe_mul(t, z_10_0); // z^(2^20 - 1)
        }
        Fe z_40_0; // z^(2^40 - 1)
        {
            Fe t = z_20_0;
            for (int i = 0; i < 20; i++)
                t = fe_sq(t);           // z^(2^40 - 2^20)
            z_40_0 = fe_mul(t, z_20_0); // z^(2^40 - 1)
        }
        Fe z_50_0; // z^(2^50 - 1)
        {
            Fe t = z_40_0;
            for (int i = 0; i < 10; i++)
                t = fe_sq(t);           // z^(2^50 - 2^10)
            z_50_0 = fe_mul(t, z_10_0); // z^(2^50 - 1)
        }
        Fe z_100_0; // z^(2^100 - 1)
        {
            Fe t = z_50_0;
            for (int i = 0; i < 50; i++)
                t = fe_sq(t);            // z^(2^100 - 2^50)
            z_100_0 = fe_mul(t, z_50_0); // z^(2^100 - 1)
        }
        Fe t;
        {
            t = z_100_0;
            for (int i = 0; i < 100; i++)
                t = fe_sq(t);       // z^(2^200 - 2^100)
            t = fe_mul(t, z_100_0); // z^(2^200 - 1)
        }
        for (int i = 0; i < 50; i++)
            t = fe_sq(t);      // z^(2^250 - 2^50)
        t = fe_mul(t, z_50_0); // z^(2^250 - 1)
        t = fe_sq(t);          // z^(2^251 - 2)
        t = fe_sq(t);          // z^(2^252 - 4)
        t = fe_sq(t);          // z^(2^253 - 8)
        t = fe_sq(t);          // z^(2^254 - 16)
        t = fe_sq(t);          // z^(2^255 - 32)
        t = fe_mul(t, z11);    // z^(2^255 - 21) = z^(p-2)
        return t;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) سُلّم مونتغمري (Montgomery Ladder) لـ X25519
    //      حساب scalar * base_point على Curve25519
    //      يستخدم إحداثيات X فقط (Montgomery form)
    // ═════════════════════════════════════════════════════════════════════

    /// النقطة الأساسية (base point) لـ Curve25519: u = 9
    static const uint8_t BASE_POINT[32] = {9};

    static void x25519_scalar_mult(uint8_t result[32],
                                   const uint8_t scalar[32],
                                   const uint8_t point[32])
    {
        // تجهيز السكالار (RFC 7748, Section 5)
        uint8_t s[32];
        std::memcpy(s, scalar, 32);
        s[0] &= 248;  // مسح أسفل 3 بتات
        s[31] &= 127; // مسح أعلى بت
        s[31] |= 64;  // تعيين البت 254

        // تجهيز إحداثي u للنقطة
        uint8_t u[32];
        std::memcpy(u, point, 32);
        u[31] &= 127; // مسح أعلى بت

        Fe x_1 = fe_from_bytes(u);
        Fe x_2;
        x_2.v[0] = 1; // x_2 = 1
        Fe z_2;       // z_2 = 0
        Fe x_3 = x_1; // x_3 = u
        Fe z_3;
        z_3.v[0] = 1; // z_3 = 1

        int swap = 0;

        // 255 خطوة في السُلّم
        for (int pos = 254; pos >= 0; pos--)
        {
            int bit = (s[pos / 8] >> (pos % 8)) & 1;

            // مبادلة مشروطة (constant-time)
            int swap_flag = swap ^ bit;
            swap = bit;

            auto cswap = [swap_flag](Fe &a, Fe &b)
            {
                for (int i = 0; i < 5; i++)
                {
                    uint64_t mask = ~((uint64_t)swap_flag - 1);
                    uint64_t t = mask & (a.v[i] ^ b.v[i]);
                    a.v[i] ^= t;
                    b.v[i] ^= t;
                }
            };
            cswap(x_2, x_3);
            cswap(z_2, z_3);

            // خطوة واحدة في السُلّم: التفاضل والتربيع
            Fe A = fe_add(x_2, z_2);
            Fe AA = fe_sq(A);
            Fe B = fe_sub(x_2, z_2);
            Fe BB = fe_sq(B);
            Fe E = fe_sub(AA, BB);
            Fe C = fe_add(x_3, z_3);
            Fe D = fe_sub(x_3, z_3);
            Fe DA = fe_mul(D, A);
            Fe CB = fe_mul(C, B);

            x_3 = fe_sq(fe_add(DA, CB));
            z_3 = fe_mul(x_1, fe_sq(fe_sub(DA, CB)));
            x_2 = fe_mul(AA, BB);

            // a24 = 121665 (ثابت المنحنى: (A-2)/4 حيث A=486662)
            Fe a24;
            a24.v[0] = 121665;
            z_2 = fe_mul(E, fe_add(AA, fe_mul(a24, E)));
        }

        // مبادلة نهائية
        auto cswap_final = [swap](Fe &a, Fe &b)
        {
            for (int i = 0; i < 5; i++)
            {
                uint64_t mask = ~((uint64_t)swap - 1);
                uint64_t t = mask & (a.v[i] ^ b.v[i]);
                a.v[i] ^= t;
                b.v[i] ^= t;
            }
        };
        cswap_final(x_2, x_3);
        cswap_final(z_2, z_3);

        // النتيجة = x_2 / z_2 = x_2 * z_2^(-1)
        Fe z_inv = fe_invert(z_2);
        Fe result_fe = fe_mul(x_2, z_inv);
        fe_to_bytes(result_fe, result);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) واجهات X25519 العامة
    // ═════════════════════════════════════════════════════════════════════

    KeyPair x25519_generate_keypair()
    {
        KeyPair kp;
        // توليد مفتاح خاص عشوائي
        random_bytes(kp.private_key.data(), X25519_KEY_SIZE);
        // حساب المفتاح العام: public = scalar * G
        x25519_scalar_mult(kp.public_key.data(),
                           kp.private_key.data(),
                           BASE_POINT);
        return kp;
    }

    X25519Key x25519_shared_secret(const X25519Key &my_private,
                                   const X25519Key &their_public)
    {
        X25519Key shared;
        x25519_scalar_mult(shared.data(),
                           my_private.data(),
                           their_public.data());
        return shared;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تنفيذ Double Ratchet (Signal Protocol)
    // ═════════════════════════════════════════════════════════════════════

    struct DoubleRatchetSession::Impl
    {
        // مفاتيح السلاسل
        std::array<uint8_t, 32> root_key{};
        std::array<uint8_t, 32> chain_key_send{};
        std::array<uint8_t, 32> chain_key_recv{};

        // زوج مفاتيح DH الحالي
        KeyPair dh_pair;
        X25519Key dh_remote_public{};

        // عدّادات الرسائل
        uint32_t send_counter = 0;
        uint32_t recv_counter = 0;
        uint32_t prev_send_counter = 0;

        bool initialized = false;

        /// اشتقاق مفتاح رسالة من سلسلة المفاتيح
        std::array<uint8_t, 32> derive_message_key(
            std::array<uint8_t, 32> &chain_key)
        {
            // مفتاح الرسالة = HMAC(chain_key, 0x01)
            uint8_t input = 0x01;
            auto mk = hmac_sha256(chain_key.data(), 32, &input, 1);

            // تقدّم سلسلة المفاتيح = HMAC(chain_key, 0x02)
            input = 0x02;
            auto new_ck = hmac_sha256(chain_key.data(), 32, &input, 1);
            chain_key = new_ck;

            return mk;
        }

        /// تدوير DH (Ratchet step)
        void dh_ratchet(const X25519Key &remote_public)
        {
            dh_remote_public = remote_public;

            // حساب السر المشترك
            auto shared = x25519_shared_secret(dh_pair.private_key, dh_remote_public);

            // اشتقاق مفتاح جذر جديد + مفتاح سلسلة استقبال
            auto derived = hkdf_sha256(root_key.data(), 32,
                                       shared.data(), 32,
                                       (const uint8_t *)"ratchet", 7, 64);
            std::memcpy(root_key.data(), derived.data(), 32);
            std::memcpy(chain_key_recv.data(), derived.data() + 32, 32);

            // توليد زوج مفاتيح DH جديد
            prev_send_counter = send_counter;
            send_counter = 0;
            dh_pair = x25519_generate_keypair();

            // اشتقاق مفتاح سلسلة إرسال جديد
            shared = x25519_shared_secret(dh_pair.private_key, dh_remote_public);
            derived = hkdf_sha256(root_key.data(), 32,
                                  shared.data(), 32,
                                  (const uint8_t *)"ratchet", 7, 64);
            std::memcpy(root_key.data(), derived.data(), 32);
            std::memcpy(chain_key_send.data(), derived.data() + 32, 32);
        }
    };

    DoubleRatchetSession::DoubleRatchetSession()
        : impl_(std::make_unique<Impl>()) {}

    DoubleRatchetSession::~DoubleRatchetSession() = default;

    void DoubleRatchetSession::init_sender(const X25519Key &shared_secret,
                                           const X25519Key &receiver_public)
    {
        impl_->dh_pair = x25519_generate_keypair();
        impl_->dh_remote_public = receiver_public;

        // (AR) حساب DH مع المفتاح العام للمستقبل
        auto sk = x25519_shared_secret(impl_->dh_pair.private_key, receiver_public);

        // (AR) اشتقاق المفاتيح بنفس مسار dh_ratchet حتى يتطابق مع المُستقبل
        //      المُستقبل يخزن shared_secret كـ root_key ثم يستدعي dh_ratchet
        //      الذي يستخدم HKDF(root_key=shared_secret, sk, "ratchet", 64)
        auto derived = hkdf_sha256(shared_secret.data(), 32,
                                   sk.data(), 32,
                                   (const uint8_t *)"ratchet", 7, 64);
        std::memcpy(impl_->root_key.data(), derived.data(), 32);
        std::memcpy(impl_->chain_key_send.data(), derived.data() + 32, 32);
        impl_->initialized = true;
    }

    void DoubleRatchetSession::init_receiver(const X25519Key &shared_secret,
                                             const KeyPair &our_keypair)
    {
        impl_->dh_pair = our_keypair;
        std::memcpy(impl_->root_key.data(), shared_secret.data(), 32);
        impl_->initialized = true;
    }

    DoubleRatchetSession::EncryptedPacket
    DoubleRatchetSession::encrypt(const uint8_t *plaintext, size_t len)
    {
        EncryptedPacket pkt;

        // اشتقاق مفتاح الرسالة
        auto mk = impl_->derive_message_key(impl_->chain_key_send);

        // بناء الهيدر: المفتاح العام (32) + رقم السلسلة (4) + رقم الرسالة السابق (4)
        pkt.header.resize(40);
        std::memcpy(pkt.header.data(), impl_->dh_pair.public_key.data(), 32);
        uint32_t pn = impl_->prev_send_counter;
        uint32_t n = impl_->send_counter;
        std::memcpy(pkt.header.data() + 32, &pn, 4);
        std::memcpy(pkt.header.data() + 36, &n, 4);

        // تشفير بـ AES-256-GCM مع الهيدر كـ AAD
        auto result = aes_gcm_encrypt(mk.data(), plaintext, len,
                                      pkt.header.data(), pkt.header.size());

        // تجميع النص المُشفّر: nonce + tag + ciphertext
        pkt.ciphertext.resize(AES_NONCE_SIZE + AES_TAG_SIZE + result.ciphertext.size());
        std::memcpy(pkt.ciphertext.data(), result.nonce.data(), AES_NONCE_SIZE);
        std::memcpy(pkt.ciphertext.data() + AES_NONCE_SIZE,
                    result.tag.data(), AES_TAG_SIZE);
        std::memcpy(pkt.ciphertext.data() + AES_NONCE_SIZE + AES_TAG_SIZE,
                    result.ciphertext.data(), result.ciphertext.size());

        impl_->send_counter++;
        return pkt;
    }

    DoubleRatchetSession::EncryptedPacket
    DoubleRatchetSession::encrypt(const Buffer &plaintext)
    {
        return encrypt(plaintext.data(), plaintext.size());
    }

    Buffer DoubleRatchetSession::decrypt(const EncryptedPacket &packet)
    {
        if (packet.header.size() < 40 ||
            packet.ciphertext.size() < AES_NONCE_SIZE + AES_TAG_SIZE)
        {
            return Buffer();
        }

        // استخراج المفتاح العام من الهيدر
        X25519Key remote_public;
        std::memcpy(remote_public.data(), packet.header.data(), 32);

        // هل تغيّر المفتاح العام للطرف الآخر؟ → تدوير DH
        if (remote_public != impl_->dh_remote_public)
        {
            impl_->dh_ratchet(remote_public);
        }

        // اشتقاق مفتاح الرسالة
        auto mk = impl_->derive_message_key(impl_->chain_key_recv);
        impl_->recv_counter++;

        // استخراج nonce + tag + ciphertext
        const uint8_t *nonce = packet.ciphertext.data();
        const uint8_t *tag = nonce + AES_NONCE_SIZE;
        const uint8_t *ct = tag + AES_TAG_SIZE;
        size_t ct_len = packet.ciphertext.size() - AES_NONCE_SIZE - AES_TAG_SIZE;

        // فك التشفير
        return aes_gcm_decrypt(mk.data(), ct, ct_len, nonce, tag,
                               packet.header.data(), packet.header.size());
    }

} // namespace sad::net::crypto
