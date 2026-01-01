// ============================================================================
// jit_hash.cpp - تنفيذ خوارزميات Hash لـ JIT (Hash Algorithms Implementation)
// Fast and Secure Hash Functions for Code Caching - Implementation
// ============================================================================
// الغرض: تنفيذ خوارزميات hash سريعة وآمنة
// Purpose: Implement fast and secure hash algorithms
// ============================================================================

#include "jit_hash.h"
#include <cstring>       // لنسخ الذاكرة / For memory copy
#include <algorithm>     // للخوارزميات / For algorithms

namespace Sad {
namespace JIT {

// ============================================================================
// XXHash64 - Implementation
// ============================================================================

// دالة مساعدة: دوران لليسار / Helper: rotate left
static inline Hash64 rotl64(Hash64 x, int r) {
    return (x << r) | (x >> (64 - r));
}

// دالة مساعدة: قراءة 64-bit little-endian / Helper: read 64-bit little-endian
static inline Hash64 read64le(const void* ptr) {
    Hash64 value;
    std::memcpy(&value, ptr, sizeof(value));
    
    // تحويل إلى little-endian إذا لزم الأمر / Convert to little-endian if needed
#ifdef _WIN32
    // Windows is little-endian
    return value;
#else
    // Check endianness and convert if needed
    uint32_t test = 1;
    if (*(uint8_t*)&test == 1) {
        // Little-endian
        return value;
    } else {
        // Big-endian, need to swap
        return ((value & 0x00000000000000FFULL) << 56) |
               ((value & 0x000000000000FF00ULL) << 40) |
               ((value & 0x0000000000FF0000ULL) << 24) |
               ((value & 0x00000000FF000000ULL) <<  8) |
               ((value & 0x000000FF00000000ULL) >>  8) |
               ((value & 0x0000FF0000000000ULL) >> 24) |
               ((value & 0x00FF000000000000ULL) >> 40) |
               ((value & 0xFF00000000000000ULL) >> 56);
    }
#endif
}

// دالة مساعدة: قراءة 32-bit little-endian / Helper: read 32-bit little-endian
static inline Hash32 read32le(const void* ptr) {
    Hash32 value;
    std::memcpy(&value, ptr, sizeof(value));
    
#ifdef _WIN32
    return value;
#else
    uint32_t test = 1;
    if (*(uint8_t*)&test == 1) {
        return value;
    } else {
        return ((value & 0x000000FF) << 24) |
               ((value & 0x0000FF00) <<  8) |
               ((value & 0x00FF0000) >>  8) |
               ((value & 0xFF000000) >> 24);
    }
#endif
}

Hash64 XXHash64::hash64(const void* data, size_t length, Hash64 seed) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    const uint8_t* const end = p + length;
    Hash64 h64;
    
    // خوارزمية xxHash64 الرئيسية / Main xxHash64 algorithm
    
    if (length >= 32) {
        // معالجة الكتل الكبيرة / Process large blocks
        const uint8_t* const limit = end - 32;
        Hash64 v1 = seed + PRIME64_1 + PRIME64_2;
        Hash64 v2 = seed + PRIME64_2;
        Hash64 v3 = seed + 0;
        Hash64 v4 = seed - PRIME64_1;
        
        do {
            v1 += read64le(p +  0) * PRIME64_2; v1 = rotl64(v1, 31); v1 *= PRIME64_1;
            v2 += read64le(p +  8) * PRIME64_2; v2 = rotl64(v2, 31); v2 *= PRIME64_1;
            v3 += read64le(p + 16) * PRIME64_2; v3 = rotl64(v3, 31); v3 *= PRIME64_1;
            v4 += read64le(p + 24) * PRIME64_2; v4 = rotl64(v4, 31); v4 *= PRIME64_1;
            p += 32;
        } while (p <= limit);
        
        h64 = rotl64(v1, 1) + rotl64(v2, 7) + rotl64(v3, 12) + rotl64(v4, 18);
        
        // دمج النتائج / Merge results
        v1 *= PRIME64_2; v1 = rotl64(v1, 31); v1 *= PRIME64_1; h64 ^= v1; h64 = h64 * PRIME64_1 + PRIME64_4;
        v2 *= PRIME64_2; v2 = rotl64(v2, 31); v2 *= PRIME64_1; h64 ^= v2; h64 = h64 * PRIME64_1 + PRIME64_4;
        v3 *= PRIME64_2; v3 = rotl64(v3, 31); v3 *= PRIME64_1; h64 ^= v3; h64 = h64 * PRIME64_1 + PRIME64_4;
        v4 *= PRIME64_2; v4 = rotl64(v4, 31); v4 *= PRIME64_1; h64 ^= v4; h64 = h64 * PRIME64_1 + PRIME64_4;
    } else {
        h64 = seed + PRIME64_5;
    }
    
    h64 += static_cast<Hash64>(length);
    
    // معالجة البايتات المتبقية / Process remaining bytes
    while (p + 8 <= end) {
        Hash64 k1 = read64le(p);
        k1 *= PRIME64_2; k1 = rotl64(k1, 31); k1 *= PRIME64_1;
        h64 ^= k1;
        h64 = rotl64(h64, 27) * PRIME64_1 + PRIME64_4;
        p += 8;
    }
    
    if (p + 4 <= end) {
        h64 ^= static_cast<Hash64>(read32le(p)) * PRIME64_1;
        h64 = rotl64(h64, 23) * PRIME64_2 + PRIME64_3;
        p += 4;
    }
    
    while (p < end) {
        h64 ^= (*p) * PRIME64_5;
        h64 = rotl64(h64, 11) * PRIME64_1;
        ++p;
    }
    
    // النهاية / Finalization
    h64 ^= h64 >> 33;
    h64 *= PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= PRIME64_3;
    h64 ^= h64 >> 32;
    
    return h64;
}

XXHash64::Hasher::Hasher(Hash64 seed)
    : total_length_(0)
    , seed_(seed)
    , v1_(seed + PRIME64_1 + PRIME64_2)
    , v2_(seed + PRIME64_2)
    , v3_(seed + 0)
    , v4_(seed - PRIME64_1)
    , buffer_size_(0)
{
    buffer_.fill(0);
}

void XXHash64::Hasher::update(const void* data, size_t length) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    total_length_ += length;
    
    // ملء المخزن المؤقت / Fill buffer
    if (buffer_size_ + length < 32) {
        std::memcpy(buffer_.data() + buffer_size_, p, length);
        buffer_size_ += length;
        return;
    }
    
    const uint8_t* const end = p + length;
    
    // معالجة البايتات في المخزن المؤقت / Process buffered bytes
    if (buffer_size_ > 0) {
        size_t to_copy = 32 - buffer_size_;
        std::memcpy(buffer_.data() + buffer_size_, p, to_copy);
        
        const uint8_t* buf = buffer_.data();
        v1_ += read64le(buf +  0) * PRIME64_2; v1_ = rotl64(v1_, 31); v1_ *= PRIME64_1;
        v2_ += read64le(buf +  8) * PRIME64_2; v2_ = rotl64(v2_, 31); v2_ *= PRIME64_1;
        v3_ += read64le(buf + 16) * PRIME64_2; v3_ = rotl64(v3_, 31); v3_ *= PRIME64_1;
        v4_ += read64le(buf + 24) * PRIME64_2; v4_ = rotl64(v4_, 31); v4_ *= PRIME64_1;
        
        p += to_copy;
        buffer_size_ = 0;
    }
    
    // معالجة الكتل الكاملة / Process full blocks
    if (p + 32 <= end) {
        const uint8_t* const limit = end - 32;
        
        do {
            v1_ += read64le(p +  0) * PRIME64_2; v1_ = rotl64(v1_, 31); v1_ *= PRIME64_1;
            v2_ += read64le(p +  8) * PRIME64_2; v2_ = rotl64(v2_, 31); v2_ *= PRIME64_1;
            v3_ += read64le(p + 16) * PRIME64_2; v3_ = rotl64(v3_, 31); v3_ *= PRIME64_1;
            v4_ += read64le(p + 24) * PRIME64_2; v4_ = rotl64(v4_, 31); v4_ *= PRIME64_1;
            p += 32;
        } while (p <= limit);
    }
    
    // حفظ البايتات المتبقية / Save remaining bytes
    if (p < end) {
        std::memcpy(buffer_.data(), p, end - p);
        buffer_size_ = end - p;
    }
}

Hash64 XXHash64::Hasher::finalize() const {
    Hash64 h64;
    
    if (total_length_ >= 32) {
        h64 = rotl64(v1_, 1) + rotl64(v2_, 7) + rotl64(v3_, 12) + rotl64(v4_, 18);
        
        // دمج النتائج / Merge results
        Hash64 v1 = v1_ * PRIME64_2; v1 = rotl64(v1, 31); v1 *= PRIME64_1; h64 ^= v1; h64 = h64 * PRIME64_1 + PRIME64_4;
        Hash64 v2 = v2_ * PRIME64_2; v2 = rotl64(v2, 31); v2 *= PRIME64_1; h64 ^= v2; h64 = h64 * PRIME64_1 + PRIME64_4;
        Hash64 v3 = v3_ * PRIME64_2; v3 = rotl64(v3, 31); v3 *= PRIME64_1; h64 ^= v3; h64 = h64 * PRIME64_1 + PRIME64_4;
        Hash64 v4 = v4_ * PRIME64_2; v4 = rotl64(v4, 31); v4 *= PRIME64_1; h64 ^= v4; h64 = h64 * PRIME64_1 + PRIME64_4;
    } else {
        h64 = seed_ + PRIME64_5;
    }
    
    h64 += static_cast<Hash64>(total_length_);
    
    // معالجة البايتات المتبقية / Process remaining bytes
    const uint8_t* p = buffer_.data();
    const uint8_t* const end = p + buffer_size_;
    
    while (p + 8 <= end) {
        Hash64 k1 = read64le(p);
        k1 *= PRIME64_2; k1 = rotl64(k1, 31); k1 *= PRIME64_1;
        h64 ^= k1;
        h64 = rotl64(h64, 27) * PRIME64_1 + PRIME64_4;
        p += 8;
    }
    
    if (p + 4 <= end) {
        h64 ^= static_cast<Hash64>(read32le(p)) * PRIME64_1;
        h64 = rotl64(h64, 23) * PRIME64_2 + PRIME64_3;
        p += 4;
    }
    
    while (p < end) {
        h64 ^= (*p) * PRIME64_5;
        h64 = rotl64(h64, 11) * PRIME64_1;
        ++p;
    }
    
    // النهاية / Finalization
    h64 ^= h64 >> 33;
    h64 *= PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= PRIME64_3;
    h64 ^= h64 >> 32;
    
    return h64;
}

void XXHash64::Hasher::reset() {
    total_length_ = 0;
    v1_ = seed_ + PRIME64_1 + PRIME64_2;
    v2_ = seed_ + PRIME64_2;
    v3_ = seed_ + 0;
    v4_ = seed_ - PRIME64_1;
    buffer_size_ = 0;
    buffer_.fill(0);
}

// ============================================================================
// CityHash64 - Implementation
// ============================================================================

// ثوابت CityHash / CityHash constants
static constexpr Hash64 k0 = 0xc3a5c85c97cb3127ULL;
static constexpr Hash64 k1 = 0xb492b66fbe98f273ULL;
static constexpr Hash64 k2 = 0x9ae16a3b2f90404fULL;

// دوال مساعدة CityHash / CityHash helper functions
static inline Hash64 cityHash64Shift(Hash64 val, int shift) {
    return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
}

static inline Hash64 cityHash64ShiftMix(Hash64 val) {
    return val ^ (val >> 47);
}

static inline Hash64 cityHash64Len16(Hash64 u, Hash64 v, Hash64 mul) {
    Hash64 a = (u ^ v) * mul;
    a ^= (a >> 47);
    Hash64 b = (v ^ a) * mul;
    b ^= (b >> 47);
    b *= mul;
    return b;
}

Hash64 CityHash64::hash64(const void* data, size_t length) {
    const uint8_t* s = static_cast<const uint8_t*>(data);
    
    if (length <= 32) {
        if (length <= 16) {
            return hashLen0to16(s, length);
        } else {
            return hashLen17to32(s, length);
        }
    } else if (length <= 64) {
        return hashLen33to64(s, length);
    }
    
    // خوارزمية CityHash للسلاسل الطويلة / CityHash for long strings
    Hash64 x = read64le(s + length - 40);
    Hash64 y = read64le(s + length - 16) + read64le(s + length - 56);
    Hash64 z = cityHash64Len16(read64le(s + length - 48) + length, read64le(s + length - 24), k1);
    
    Hash64 v1 = 0, v2 = 0;
    Hash64 w1 = 0, w2 = 0;
    
    x = x * k1 + read64le(s);
    size_t len = (length - 1) & ~63ULL;
    const uint8_t* end = s + len;
    s += (length - 1 - len) & ~7ULL;
    
    do {
        x = cityHash64Shift(x + y + v1 + read64le(s + 8), 37) * k1;
        y = cityHash64Shift(y + v2 + read64le(s + 48), 42) * k1;
        x ^= w2;
        y += v1 + read64le(s + 40);
        z = cityHash64Shift(z + w1, 33) * k1;
        
        v1 = read64le(s) * k1;
        v2 = read64le(s + 8) + z;
        v1 = cityHash64Shift(v1 + v2, 37) * k1;
        v2 = cityHash64Shift(v2, 42) * k1;
        
        w1 = read64le(s + 32) + y;
        w2 = read64le(s + 40) * k1;
        w1 = cityHash64Shift(w1 + w2, 37) * k1;
        w2 = cityHash64Shift(w2, 42) * k1;
        
        std::swap(z, x);
        s += 64;
    } while (s != end);
    
    return cityHash64Len16(
        cityHash64Len16(v1, w1, k1) + cityHash64ShiftMix(y) * k1 + z,
        cityHash64Len16(v2, w2, k1) + x,
        k1
    );
}

Hash64 CityHash64::hashLen0to16(const uint8_t* s, size_t length) {
    if (length >= 8) {
        Hash64 mul = k2 + length * 2;
        Hash64 a = read64le(s) + k2;
        Hash64 b = read64le(s + length - 8);
        Hash64 c = cityHash64Shift(b, 37) * mul + a;
        Hash64 d = (cityHash64Shift(a, 25) + b) * mul;
        return cityHash64Len16(c, d, mul);
    }
    
    if (length >= 4) {
        Hash64 mul = k2 + length * 2;
        Hash64 a = read32le(s);
        return cityHash64Len16(length + (a << 3), read32le(s + length - 4), mul);
    }
    
    if (length > 0) {
        uint8_t a = s[0];
        uint8_t b = s[length >> 1];
        uint8_t c = s[length - 1];
        uint32_t y = static_cast<uint32_t>(a) + (static_cast<uint32_t>(b) << 8);
        uint32_t z = length + (static_cast<uint32_t>(c) << 2);
        return cityHash64ShiftMix(y * k2 ^ z * k0) * k2;
    }
    
    return k2;
}

Hash64 CityHash64::hashLen17to32(const uint8_t* s, size_t length) {
    Hash64 mul = k2 + length * 2;
    Hash64 a = read64le(s) * k1;
    Hash64 b = read64le(s + 8);
    Hash64 c = read64le(s + length - 8) * mul;
    Hash64 d = read64le(s + length - 16) * k2;
    return cityHash64Len16(
        cityHash64Shift(a + b, 43) + cityHash64Shift(c, 30) + d,
        a + cityHash64Shift(b + k2, 18) + c,
        mul
    );
}

Hash64 CityHash64::hashLen33to64(const uint8_t* s, size_t length) {
    Hash64 mul = k2 + length * 2;
    Hash64 a = read64le(s) * k2;
    Hash64 b = read64le(s + 8);
    Hash64 c = read64le(s + length - 8) * mul;
    Hash64 d = read64le(s + length - 16) * k2;
    Hash64 y = cityHash64Shift(a + b, 43) + cityHash64Shift(c, 30) + d;
    Hash64 z = cityHash64Len16(y, a + cityHash64Shift(b + k2, 18) + c, mul);
    Hash64 e = read64le(s + 16) * mul;
    Hash64 f = read64le(s + 24);
    Hash64 g = (y + read64le(s + length - 32)) * mul;
    Hash64 h = (z + read64le(s + length - 24)) * mul;
    return cityHash64Len16(
        cityHash64Shift(e + f, 43) + cityHash64Shift(g, 30) + h,
        e + cityHash64Shift(f + a, 18) + g,
        mul
    );
}

// ============================================================================
// SHA256Hash - Implementation (Basic)
// ============================================================================

// ثوابت SHA-256 / SHA-256 constants
static constexpr Hash32 K256[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static inline Hash32 rotr32(Hash32 x, int n) {
    return (x >> n) | (x << (32 - n));
}

Hash256 SHA256Hash::hash256(const void* data, size_t length) {
    SHA256Hash::Hasher hasher;
    hasher.update(data, length);
    return hasher.finalize();
}

SHA256Hash::Hasher::Hasher() {
    reset();
}

void SHA256Hash::Hasher::reset() {
    // القيم الأولية SHA-256 / SHA-256 initial values
    state_[0] = 0x6a09e667;
    state_[1] = 0xbb67ae85;
    state_[2] = 0x3c6ef372;
    state_[3] = 0xa54ff53a;
    state_[4] = 0x510e527f;
    state_[5] = 0x9b05688c;
    state_[6] = 0x1f83d9ab;
    state_[7] = 0x5be0cd19;
    
    count_ = 0;
    buffer_size_ = 0;
    buffer_.fill(0);
}

void SHA256Hash::Hasher::update(const void* data, size_t length) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    count_ += length;
    
    // معالجة البايتات في المخزن المؤقت / Process buffered bytes
    if (buffer_size_ > 0) {
        size_t to_copy = std::min(64 - buffer_size_, length);
        std::memcpy(buffer_.data() + buffer_size_, p, to_copy);
        buffer_size_ += to_copy;
        p += to_copy;
        length -= to_copy;
        
        if (buffer_size_ == 64) {
            processBlock(buffer_.data());
            buffer_size_ = 0;
        }
    }
    
    // معالجة الكتل الكاملة / Process full blocks
    while (length >= 64) {
        processBlock(p);
        p += 64;
        length -= 64;
    }
    
    // حفظ البايتات المتبقية / Save remaining bytes
    if (length > 0) {
        std::memcpy(buffer_.data(), p, length);
        buffer_size_ = length;
    }
}

Hash256 SHA256Hash::Hasher::finalize() {
    // إضافة البت النهائي / Add final bit
    buffer_[buffer_size_++] = 0x80;
    
    // إضافة padding / Add padding
    if (buffer_size_ > 56) {
        while (buffer_size_ < 64) {
            buffer_[buffer_size_++] = 0;
        }
        processBlock(buffer_.data());
        buffer_size_ = 0;
    }
    
    while (buffer_size_ < 56) {
        buffer_[buffer_size_++] = 0;
    }
    
    // إضافة الطول / Add length
    uint64_t bit_count = count_ * 8;
    for (int i = 7; i >= 0; --i) {
        buffer_[56 + i] = static_cast<uint8_t>(bit_count >> (56 - i * 8));
    }
    
    processBlock(buffer_.data());
    
    // تحويل النتيجة / Convert result
    Hash256 result;
    for (size_t i = 0; i < 8; ++i) {
        result[i * 4 + 0] = static_cast<uint8_t>(state_[i] >> 24);
        result[i * 4 + 1] = static_cast<uint8_t>(state_[i] >> 16);
        result[i * 4 + 2] = static_cast<uint8_t>(state_[i] >>  8);
        result[i * 4 + 3] = static_cast<uint8_t>(state_[i] >>  0);
    }
    
    return result;
}

void SHA256Hash::Hasher::processBlock(const uint8_t* block) {
    Hash32 w[64];
    
    // تحضير جدول الرسائل / Prepare message schedule
    for (int i = 0; i < 16; ++i) {
        w[i] = (static_cast<Hash32>(block[i * 4 + 0]) << 24) |
               (static_cast<Hash32>(block[i * 4 + 1]) << 16) |
               (static_cast<Hash32>(block[i * 4 + 2]) <<  8) |
               (static_cast<Hash32>(block[i * 4 + 3]) <<  0);
    }
    
    for (int i = 16; i < 64; ++i) {
        Hash32 s0 = rotr32(w[i - 15], 7) ^ rotr32(w[i - 15], 18) ^ (w[i - 15] >> 3);
        Hash32 s1 = rotr32(w[i - 2], 17) ^ rotr32(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    
    // تهيئة المتغيرات / Initialize variables
    Hash32 a = state_[0];
    Hash32 b = state_[1];
    Hash32 c = state_[2];
    Hash32 d = state_[3];
    Hash32 e = state_[4];
    Hash32 f = state_[5];
    Hash32 g = state_[6];
    Hash32 h = state_[7];
    
    // الجولات الرئيسية / Main rounds
    for (int i = 0; i < 64; ++i) {
        Hash32 S1 = rotr32(e, 6) ^ rotr32(e, 11) ^ rotr32(e, 25);
        Hash32 ch = (e & f) ^ ((~e) & g);
        Hash32 temp1 = h + S1 + ch + K256[i] + w[i];
        Hash32 S0 = rotr32(a, 2) ^ rotr32(a, 13) ^ rotr32(a, 22);
        Hash32 maj = (a & b) ^ (a & c) ^ (b & c);
        Hash32 temp2 = S0 + maj;
        
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }
    
    // تحديث الحالة / Update state
    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
    state_[4] += e;
    state_[5] += f;
    state_[6] += g;
    state_[7] += h;
}

// ============================================================================
// CodeHasher - Implementation
// ============================================================================

CodeHasher::CodeHasher(HashAlgorithm algorithm)
    : algorithm_(algorithm)
{
}

Hash64 CodeHasher::hash(const std::string& code) const {
    switch (algorithm_) {
        case HashAlgorithm::FNV1A:
            return FNV1aHash::hash64(code);
            
        case HashAlgorithm::XXH64:
            return XXHash64::hash64(code);
            
        case HashAlgorithm::CITY64:
            return CityHash64::hash64(code);
            
        case HashAlgorithm::SHA256: {
            auto hash256 = SHA256Hash::hash256(code);
            // استخدام أول 64 بت / Use first 64 bits
            Hash64 result = 0;
            for (int i = 0; i < 8; ++i) {
                result = (result << 8) | hash256[i];
            }
            return result;
        }
        
        default:
            return FNV1aHash::hash64(code);
    }
}

std::string CodeHasher::hashToHex(const std::string& code) const {
    Hash64 h = hash(code);
    
    char hex[17];
    snprintf(hex, sizeof(hex), "%016llx", static_cast<unsigned long long>(h));
    
    return std::string(hex);
}

HashAlgorithm CodeHasher::getAlgorithm() const {
    return algorithm_;
}

std::string CodeHasher::getAlgorithmName() const {
    switch (algorithm_) {
        case HashAlgorithm::FNV1A:  return "FNV-1a";
        case HashAlgorithm::XXH64:  return "xxHash64";
        case HashAlgorithm::CITY64: return "CityHash64";
        case HashAlgorithm::SHA256: return "SHA-256";
        default:                    return "Unknown";
    }
}

// ============================================================================
// Helper Functions
// ============================================================================

std::string hashToString(Hash64 hash) {
    char hex[17];
    snprintf(hex, sizeof(hex), "%016llx", static_cast<unsigned long long>(hash));
    return std::string(hex);
}

std::string hashToString(const Hash256& hash) {
    char hex[65];
    for (size_t i = 0; i < 32; ++i) {
        snprintf(hex + i * 2, 3, "%02x", hash[i]);
    }
    return std::string(hex);
}

} // namespace JIT
} // namespace Sad
