// ============================================================================
// jit_hash.cpp - تنفيذ خوارزميات Hash لـ JIT (Hash Algorithms Implementation)
// Fast and Secure Hash Functions for Code Caching - Implementation
// ============================================================================

#include "jit_hash.h"
#include <cstring>       // memcpy
#include <algorithm>     // min
#include <sstream>       // ostringstream
#include <iomanip>       // setw, setfill

namespace Sad {
namespace JIT {

// ============================================================================
// دوال مساعدة عامة / General Helper Functions
// ============================================================================

static inline uint64_t read64le(const void* ptr) {
    uint64_t value;
    std::memcpy(&value, ptr, sizeof(value));
    return value;  // Windows is little-endian
}

static inline uint32_t read32le(const void* ptr) {
    uint32_t value;
    std::memcpy(&value, ptr, sizeof(value));
    return value;
}

static inline uint64_t rotl64(uint64_t x, int r) {
    return (x << r) | (x >> (64 - r));
}

static inline uint32_t rotr32(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

// ============================================================================
// XXHash64 - Implementation
// ============================================================================

Hash64 XXHash64::hash64(const void* data, size_t length, Hash64 seed) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    const uint8_t* const end = p + length;
    Hash64 h64;
    
    if (length >= 32) {
        const uint8_t* const limit = end - 32;
        Hash64 v1 = seed + PRIME64_1 + PRIME64_2;
        Hash64 v2 = seed + PRIME64_2;
        Hash64 v3 = seed;
        Hash64 v4 = seed - PRIME64_1;
        
        do {
            v1 += read64le(p +  0) * PRIME64_2; v1 = rotl64(v1, 31); v1 *= PRIME64_1;
            v2 += read64le(p +  8) * PRIME64_2; v2 = rotl64(v2, 31); v2 *= PRIME64_1;
            v3 += read64le(p + 16) * PRIME64_2; v3 = rotl64(v3, 31); v3 *= PRIME64_1;
            v4 += read64le(p + 24) * PRIME64_2; v4 = rotl64(v4, 31); v4 *= PRIME64_1;
            p += 32;
        } while (p <= limit);
        
        h64 = rotl64(v1, 1) + rotl64(v2, 7) + rotl64(v3, 12) + rotl64(v4, 18);
        
        v1 *= PRIME64_2; v1 = rotl64(v1, 31); v1 *= PRIME64_1; h64 ^= v1; h64 = h64 * PRIME64_1 + PRIME64_4;
        v2 *= PRIME64_2; v2 = rotl64(v2, 31); v2 *= PRIME64_1; h64 ^= v2; h64 = h64 * PRIME64_1 + PRIME64_4;
        v3 *= PRIME64_2; v3 = rotl64(v3, 31); v3 *= PRIME64_1; h64 ^= v3; h64 = h64 * PRIME64_1 + PRIME64_4;
        v4 *= PRIME64_2; v4 = rotl64(v4, 31); v4 *= PRIME64_1; h64 ^= v4; h64 = h64 * PRIME64_1 + PRIME64_4;
    } else {
        h64 = seed + PRIME64_5;
    }
    
    h64 += static_cast<Hash64>(length);
    
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
    
    h64 ^= h64 >> 33;
    h64 *= PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= PRIME64_3;
    h64 ^= h64 >> 32;
    
    return h64;
}

// ============================================================================
// XXHash64::Hasher - Incremental Hashing
// ============================================================================

XXHash64::Hasher::Hasher(Hash64 seed)
    : total_length_(0)
    , seed_(seed)
    , v1_(seed + PRIME64_1 + PRIME64_2)
    , v2_(seed + PRIME64_2)
    , v3_(seed)
    , v4_(seed - PRIME64_1)
    , buffer_size_(0)
{
    buffer_.fill(0);
}

void XXHash64::Hasher::update(const void* data, size_t length) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    total_length_ += length;
    
    if (buffer_size_ + length < 32) {
        std::memcpy(buffer_.data() + buffer_size_, p, length);
        buffer_size_ += length;
        return;
    }
    
    const uint8_t* const end = p + length;
    
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
    
    if (p < end) {
        std::memcpy(buffer_.data(), p, end - p);
        buffer_size_ = end - p;
    }
}

Hash64 XXHash64::Hasher::finalize() const {
    Hash64 h64;
    
    if (total_length_ >= 32) {
        h64 = rotl64(v1_, 1) + rotl64(v2_, 7) + rotl64(v3_, 12) + rotl64(v4_, 18);
        
        Hash64 v1 = v1_ * PRIME64_2; v1 = rotl64(v1, 31); v1 *= PRIME64_1; h64 ^= v1; h64 = h64 * PRIME64_1 + PRIME64_4;
        Hash64 v2 = v2_ * PRIME64_2; v2 = rotl64(v2, 31); v2 *= PRIME64_1; h64 ^= v2; h64 = h64 * PRIME64_1 + PRIME64_4;
        Hash64 v3 = v3_ * PRIME64_2; v3 = rotl64(v3, 31); v3 *= PRIME64_1; h64 ^= v3; h64 = h64 * PRIME64_1 + PRIME64_4;
        Hash64 v4 = v4_ * PRIME64_2; v4 = rotl64(v4, 31); v4 *= PRIME64_1; h64 ^= v4; h64 = h64 * PRIME64_1 + PRIME64_4;
    } else {
        h64 = seed_ + PRIME64_5;
    }
    
    h64 += static_cast<Hash64>(total_length_);
    
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
    v3_ = seed_;
    v4_ = seed_ - PRIME64_1;
    buffer_size_ = 0;
    buffer_.fill(0);
}

// ============================================================================
// CityHash64 - Implementation
// ============================================================================

static constexpr Hash64 k0 = 0xc3a5c85c97cb3127ULL;
static constexpr Hash64 k1 = 0xb492b66fbe98f273ULL;
static constexpr Hash64 k2 = 0x9ae16a3b2f90404fULL;

static inline Hash64 cityRotate(Hash64 val, int shift) {
    return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
}

static inline Hash64 cityShiftMix(Hash64 val) {
    return val ^ (val >> 47);
}

Hash64 CityHash64::fetch64(const uint8_t* p) {
    return read64le(p);
}

Hash64 CityHash64::rotate(Hash64 val, int shift) {
    return cityRotate(val, shift);
}

Hash64 CityHash64::hashLen16(Hash64 u, Hash64 v) {
    constexpr Hash64 kMul = 0x9ddfea08eb382d69ULL;
    Hash64 a = (u ^ v) * kMul;
    a ^= (a >> 47);
    Hash64 b = (v ^ a) * kMul;
    b ^= (b >> 47);
    b *= kMul;
    return b;
}

Hash64 CityHash64::hashLen0to16(const uint8_t* s, size_t len) {
    if (len >= 8) {
        Hash64 mul = k2 + len * 2;
        Hash64 a = read64le(s) + k2;
        Hash64 b = read64le(s + len - 8);
        Hash64 c = cityRotate(b, 37) * mul + a;
        Hash64 d = (cityRotate(a, 25) + b) * mul;
        return hashLen16(c, d);
    }
    
    if (len >= 4) {
        Hash64 mul = k2 + len * 2;
        Hash64 a = read32le(s);
        return hashLen16(len + (a << 3), read32le(s + len - 4));
    }
    
    if (len > 0) {
        uint8_t a = s[0];
        uint8_t b = s[len >> 1];
        uint8_t c = s[len - 1];
        uint32_t y = static_cast<uint32_t>(a) + (static_cast<uint32_t>(b) << 8);
        uint32_t z = len + (static_cast<uint32_t>(c) << 2);
        return cityShiftMix(y * k2 ^ z * k0) * k2;
    }
    
    return k2;
}

Hash64 CityHash64::hashLen17to32(const uint8_t* s, size_t len) {
    Hash64 mul = k2 + len * 2;
    Hash64 a = read64le(s) * k1;
    Hash64 b = read64le(s + 8);
    Hash64 c = read64le(s + len - 8) * mul;
    Hash64 d = read64le(s + len - 16) * k2;
    return hashLen16(
        cityRotate(a + b, 43) + cityRotate(c, 30) + d,
        a + cityRotate(b + k2, 18) + c
    );
}

Hash64 CityHash64::hashLen33to64(const uint8_t* s, size_t len) {
    Hash64 mul = k2 + len * 2;
    Hash64 a = read64le(s) * k2;
    Hash64 b = read64le(s + 8);
    Hash64 c = read64le(s + len - 8) * mul;
    Hash64 d = read64le(s + len - 16) * k2;
    Hash64 y = cityRotate(a + b, 43) + cityRotate(c, 30) + d;
    Hash64 z = hashLen16(y, a + cityRotate(b + k2, 18) + c);
    Hash64 e = read64le(s + 16) * mul;
    Hash64 f = read64le(s + 24);
    Hash64 g = (y + read64le(s + len - 32)) * mul;
    Hash64 h = (z + read64le(s + len - 24)) * mul;
    return hashLen16(
        cityRotate(e + f, 43) + cityRotate(g, 30) + h,
        e + cityRotate(f + a, 18) + g
    );
}

Hash64 CityHash64::hash64(const void* data, size_t len) {
    const uint8_t* s = static_cast<const uint8_t*>(data);
    
    if (len <= 32) {
        if (len <= 16) {
            return hashLen0to16(s, len);
        } else {
            return hashLen17to32(s, len);
        }
    } else if (len <= 64) {
        return hashLen33to64(s, len);
    }
    
    Hash64 x = read64le(s + len - 40);
    Hash64 y = read64le(s + len - 16) + read64le(s + len - 56);
    Hash64 z = hashLen16(read64le(s + len - 48) + len, read64le(s + len - 24));
    
    Hash64 v1 = 0, v2 = 0;
    Hash64 w1 = 0, w2 = 0;
    
    x = x * k1 + read64le(s);
    
    size_t pos = 0;
    size_t end_pos = (len - 1) & ~63ULL;
    
    do {
        x = cityRotate(x + y + v1 + read64le(s + pos + 8), 37) * k1;
        y = cityRotate(y + v2 + read64le(s + pos + 48), 42) * k1;
        x ^= w2;
        y += v1 + read64le(s + pos + 40);
        z = cityRotate(z + w1, 33) * k1;
        
        v1 = read64le(s + pos) * k1;
        v2 = read64le(s + pos + 8) + z;
        v1 = cityRotate(v1 + v2, 37) * k1;
        v2 = cityRotate(v2, 42) * k1;
        
        w1 = read64le(s + pos + 32) + y;
        w2 = read64le(s + pos + 40) * k1;
        w1 = cityRotate(w1 + w2, 37) * k1;
        w2 = cityRotate(w2, 42) * k1;
        
        std::swap(z, x);
        pos += 64;
    } while (pos != end_pos);
    
    return hashLen16(
        hashLen16(v1, w1) + cityShiftMix(y) * k1 + z,
        hashLen16(v2, w2) + x
    );
}

Hash64 CityHash64::hash64WithSeed(const void* data, size_t len, Hash64 seed) {
    return hashLen16(hash64(data, len) - seed, seed);
}

Hash64 CityHash64::hash64WithSeeds(const void* data, size_t len, Hash64 seed0, Hash64 seed1) {
    return hashLen16(hash64(data, len) - seed0, seed1);
}

Hash128 CityHash64::hash128(const void* data, size_t len) {
    const uint8_t* s = static_cast<const uint8_t*>(data);
    if (len < 128) {
        return Hash128{hash64(data, len), hash64(s + (len/2), len - (len/2))};
    }
    Hash64 h1 = hash64(s, len / 2);
    Hash64 h2 = hash64(s + len / 2, len - len / 2);
    return Hash128{h1, h2};
}

Hash128 CityHash64::hash128WithSeed(const void* data, size_t len, Hash128 seed) {
    Hash128 result = hash128(data, len);
    result[0] = hashLen16(result[0], seed[0]);
    result[1] = hashLen16(result[1], seed[1]);
    return result;
}

// ============================================================================
// SHA256 - Implementation
// ============================================================================

const uint32_t SHA256::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline uint32_t SHA256::rotr(uint32_t x, int n) { return rotr32(x, n); }
inline uint32_t SHA256::ch(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ ((~x) & z); }
inline uint32_t SHA256::maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
inline uint32_t SHA256::sigma0(uint32_t x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
inline uint32_t SHA256::sigma1(uint32_t x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }
inline uint32_t SHA256::gamma0(uint32_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
inline uint32_t SHA256::gamma1(uint32_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }

Hash256 SHA256::hash(const void* data, size_t length) {
    Hasher hasher;
    hasher.update(data, length);
    return hasher.finalize();
}

SHA256::Hasher::Hasher() {
    reset();
}

void SHA256::Hasher::reset() {
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

void SHA256::Hasher::update(const void* data, size_t length) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    count_ += length * 8;
    
    if (buffer_size_ > 0) {
        size_t to_copy = std::min<size_t>(64 - buffer_size_, length);
        std::memcpy(buffer_.data() + buffer_size_, p, to_copy);
        buffer_size_ += to_copy;
        p += to_copy;
        length -= to_copy;
        
        if (buffer_size_ == 64) {
            processBlock(buffer_.data());
            buffer_size_ = 0;
        }
    }
    
    while (length >= 64) {
        processBlock(p);
        p += 64;
        length -= 64;
    }
    
    if (length > 0) {
        std::memcpy(buffer_.data(), p, length);
        buffer_size_ = length;
    }
}

void SHA256::Hasher::update(const std::string& str) {
    update(str.data(), str.size());
}

Hash256 SHA256::Hasher::finalize() {
    buffer_[buffer_size_++] = 0x80;
    
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
    
    uint64_t bit_count = count_;
    for (int i = 7; i >= 0; --i) {
        buffer_[56 + (7 - i)] = static_cast<uint8_t>(bit_count >> (i * 8));
    }
    
    processBlock(buffer_.data());
    
    Hash256 result;
    for (size_t i = 0; i < 8; ++i) {
        result[i * 4 + 0] = static_cast<uint8_t>(state_[i] >> 24);
        result[i * 4 + 1] = static_cast<uint8_t>(state_[i] >> 16);
        result[i * 4 + 2] = static_cast<uint8_t>(state_[i] >> 8);
        result[i * 4 + 3] = static_cast<uint8_t>(state_[i]);
    }
    
    return result;
}

void SHA256::Hasher::processBlock(const uint8_t* block) {
    uint32_t w[64];
    
    for (int i = 0; i < 16; ++i) {
        w[i] = (static_cast<uint32_t>(block[i * 4 + 0]) << 24) |
               (static_cast<uint32_t>(block[i * 4 + 1]) << 16) |
               (static_cast<uint32_t>(block[i * 4 + 2]) << 8) |
               (static_cast<uint32_t>(block[i * 4 + 3]));
    }
    
    for (int i = 16; i < 64; ++i) {
        w[i] = gamma1(w[i - 2]) + w[i - 7] + gamma0(w[i - 15]) + w[i - 16];
    }
    
    uint32_t a = state_[0];
    uint32_t b = state_[1];
    uint32_t c = state_[2];
    uint32_t d = state_[3];
    uint32_t e = state_[4];
    uint32_t f = state_[5];
    uint32_t g = state_[6];
    uint32_t h = state_[7];
    
    for (int i = 0; i < 64; ++i) {
        uint32_t t1 = h + sigma1(e) + ch(e, f, g) + K[i] + w[i];
        uint32_t t2 = sigma0(a) + maj(a, b, c);
        
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    
    state_[0] += a;
    state_[1] += b;
    state_[2] += c;
    state_[3] += d;
    state_[4] += e;
    state_[5] += f;
    state_[6] += g;
    state_[7] += h;
}

std::string SHA256::toString(const Hash256& hash) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < hash.size(); ++i) {
        oss << std::setw(2) << static_cast<int>(hash[i]);
    }
    return oss.str();
}

Hash256 SHA256::fromString(const std::string& str) {
    Hash256 hash;
    hash.fill(0);
    
    if (str.length() != 64) {
        return hash;
    }
    
    for (size_t i = 0; i < 32; ++i) {
        std::string byte_str = str.substr(i * 2, 2);
        hash[i] = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
    }
    
    return hash;
}

// ============================================================================
// CodeHasher - Implementation
// ============================================================================

std::string CodeHasher::hashCode(const std::string& source_code) const {
    Hash64 h = 0;
    
    switch (algorithm_) {
        case HashAlgorithm::FNV1A:
            h = FNV1aHash::hash64(source_code);
            break;
            
        case HashAlgorithm::XXH64:
            h = XXHash64::hash64(source_code);
            break;
            
        case HashAlgorithm::CITY64:
            h = CityHash64::hash64(source_code);
            break;
            
        case HashAlgorithm::SHA256:
            return secureHashCode(source_code);
        
        default:
            h = FNV1aHash::hash64(source_code);
            break;
    }
    
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(16) << h;
    return oss.str();
}

std::string CodeHasher::secureHashCode(const std::string& source_code) const {
    Hash256 hash = SHA256::hash(source_code);
    return SHA256::toString(hash);
}

std::string CodeHasher::hashWithMetadata(
    const std::string& source_code,
    const std::string& function_name,
    int optimization_level) const 
{
    std::string combined = source_code + "|" + function_name + "|" + std::to_string(optimization_level);
    return hashCode(combined);
}

} // namespace JIT
} // namespace Sad
