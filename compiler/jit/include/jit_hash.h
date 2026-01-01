// ============================================================================
// jit_hash.h - خوارزميات Hash لـ JIT (Hash Algorithms for JIT)
// Fast and Secure Hash Functions for Code Caching
// ============================================================================
// الغرض: توفير خوارزميات hash سريعة وآمنة
// Purpose: Provide fast and secure hash algorithms
// ============================================================================

#ifndef SAD_JIT_HASH_H
#define SAD_JIT_HASH_H

#include <string>        // للنصوص / For strings
#include <cstdint>       // للأنواع الصحيحة / For integer types
#include <vector>        // للقوائم / For vectors
#include <array>         // للمصفوفات الثابتة / For fixed arrays

namespace Sad {
namespace JIT {

// ============================================================================
// HashAlgorithm - نوع خوارزمية Hash / Hash Algorithm Type
// ============================================================================

enum class HashAlgorithm {
    FNV1A,                           // FNV-1a (سريع جداً) / FNV-1a (very fast)
    XXH64,                           // xxHash64 (سريع ومتوازن) / xxHash64 (fast and balanced)
    CITY64,                          // CityHash64 (جودة عالية) / CityHash64 (high quality)
    SHA256                           // SHA-256 (آمن جداً) / SHA-256 (very secure)
};

// ============================================================================
// Hash Results / نتائج Hash
// ============================================================================

using Hash32 = uint32_t;            // Hash 32-بت / 32-bit hash
using Hash64 = uint64_t;            // Hash 64-بت / 64-bit hash
using Hash128 = std::array<uint64_t, 2>; // Hash 128-بت / 128-bit hash
using Hash256 = std::array<uint8_t, 32>; // Hash 256-بت / 256-bit hash

// ============================================================================
// FNV-1a Hash (سريع جداً، جودة جيدة)
// FNV-1a Hash (very fast, good quality)
// ============================================================================

class FNV1aHash {
public:
    // ثوابت FNV-1a / FNV-1a constants
    static constexpr Hash32 FNV_32_PRIME = 0x01000193;      // FNV prime 32-bit
    static constexpr Hash32 FNV_32_OFFSET = 0x811c9dc5;     // FNV offset 32-bit
    static constexpr Hash64 FNV_64_PRIME = 0x100000001b3;   // FNV prime 64-bit
    static constexpr Hash64 FNV_64_OFFSET = 0xcbf29ce484222325; // FNV offset 64-bit
    
    // ========================================
    // Hash 32-bit
    // ========================================
    
    static Hash32 hash32(const void* data, size_t length) {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        Hash32 hash = FNV_32_OFFSET;
        
        for (size_t i = 0; i < length; ++i) {
            hash ^= bytes[i];                // XOR مع البايت / XOR with byte
            hash *= FNV_32_PRIME;            // ضرب في الثابت / Multiply by prime
        }
        
        return hash;
    }
    
    static Hash32 hash32(const std::string& str) {
        return hash32(str.data(), str.size());
    }
    
    // ========================================
    // Hash 64-bit (موصى به)
    // Hash 64-bit (recommended)
    // ========================================
    
    static Hash64 hash64(const void* data, size_t length) {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        Hash64 hash = FNV_64_OFFSET;
        
        for (size_t i = 0; i < length; ++i) {
            hash ^= bytes[i];                // XOR مع البايت / XOR with byte
            hash *= FNV_64_PRIME;            // ضرب في الثابت / Multiply by prime
        }
        
        return hash;
    }
    
    static Hash64 hash64(const std::string& str) {
        return hash64(str.data(), str.size());
    }
    
    // ========================================
    // Hash تراكمي / Incremental Hash
    // ========================================
    
    class Hasher {
    public:
        Hasher() : hash_(FNV_64_OFFSET) {}
        
        void update(const void* data, size_t length) {
            const uint8_t* bytes = static_cast<const uint8_t*>(data);
            for (size_t i = 0; i < length; ++i) {
                hash_ ^= bytes[i];
                hash_ *= FNV_64_PRIME;
            }
        }
        
        void update(const std::string& str) {
            update(str.data(), str.size());
        }
        
        Hash64 finalize() const {
            return hash_;
        }
        
        void reset() {
            hash_ = FNV_64_OFFSET;
        }
        
    private:
        Hash64 hash_;
    };
};

// ============================================================================
// xxHash64 (أسرع hash عالي الجودة)
// xxHash64 (fastest high-quality hash)
// ============================================================================

class XXHash64 {
public:
    // ثوابت xxHash / xxHash constants
    static constexpr Hash64 PRIME64_1 = 0x9E3779B185EBCA87ULL;
    static constexpr Hash64 PRIME64_2 = 0xC2B2AE3D27D4EB4FULL;
    static constexpr Hash64 PRIME64_3 = 0x165667B19E3779F9ULL;
    static constexpr Hash64 PRIME64_4 = 0x85EBCA77C2B2AE63ULL;
    static constexpr Hash64 PRIME64_5 = 0x27D4EB2F165667C5ULL;
    
    // ========================================
    // Hash رئيسي / Main Hash
    // ========================================
    
    static Hash64 hash64(const void* data, size_t length, Hash64 seed = 0);
    
    static Hash64 hash64(const std::string& str, Hash64 seed = 0) {
        return hash64(str.data(), str.size(), seed);
    }
    
    // ========================================
    // Hash تراكمي / Incremental Hash
    // ========================================
    
    class Hasher {
    public:
        explicit Hasher(Hash64 seed = 0);
        
        void update(const void* data, size_t length);
        void update(const std::string& str) {
            update(str.data(), str.size());
        }
        
        Hash64 finalize() const;
        void reset();
        
    private:
        size_t total_length_;
        Hash64 seed_;
        Hash64 v1_, v2_, v3_, v4_;
        std::array<uint8_t, 32> buffer_;
        size_t buffer_size_;
    };

private:
    // دوال مساعدة / Helper functions
    static inline Hash64 rotl64(Hash64 x, int r) {
        return (x << r) | (x >> (64 - r));
    }
    
    static inline Hash64 round(Hash64 acc, Hash64 input) {
        acc += input * PRIME64_2;
        acc = rotl64(acc, 31);
        acc *= PRIME64_1;
        return acc;
    }
};

// ============================================================================
// CityHash64 (جودة عالية جداً)
// CityHash64 (very high quality)
// ============================================================================

class CityHash64 {
public:
    // ========================================
    // Hash رئيسي / Main Hash
    // ========================================
    
    static Hash64 hash64(const void* data, size_t length);
    
    static Hash64 hash64(const std::string& str) {
        return hash64(str.data(), str.size());
    }
    
    // ========================================
    // Hash مع Seed
    // Hash with Seed
    // ========================================
    
    static Hash64 hash64WithSeed(const void* data, size_t length, Hash64 seed);
    
    static Hash64 hash64WithSeeds(
        const void* data, 
        size_t length, 
        Hash64 seed0, 
        Hash64 seed1
    );
    
    // ========================================
    // Hash 128-bit
    // ========================================
    
    static Hash128 hash128(const void* data, size_t length);
    
    static Hash128 hash128WithSeed(
        const void* data, 
        size_t length, 
        Hash128 seed
    );

private:
    // دوال مساعدة / Helper functions
    static inline Hash64 fetch64(const uint8_t* p);
    static inline Hash64 rotate(Hash64 val, int shift);
    static inline Hash64 hashLen16(Hash64 u, Hash64 v);
    static inline Hash64 hashLen0to16(const uint8_t* s, size_t len);
    static inline Hash64 hashLen17to32(const uint8_t* s, size_t len);
    static inline Hash64 hashLen33to64(const uint8_t* s, size_t len);
};

// ============================================================================
// SHA-256 (أعلى مستوى أمان)
// SHA-256 (highest security level)
// ============================================================================

class SHA256 {
public:
    // ========================================
    // Hash رئيسي / Main Hash
    // ========================================
    
    static Hash256 hash(const void* data, size_t length);
    
    static Hash256 hash(const std::string& str) {
        return hash(str.data(), str.size());
    }
    
    // ========================================
    // Hash تراكمي / Incremental Hash
    // ========================================
    
    class Hasher {
    public:
        Hasher();
        
        void update(const void* data, size_t length);
        void update(const std::string& str);
        
        Hash256 finalize();
        void reset();
        
    private:
        uint32_t state_[8];
        uint64_t count_;
        std::array<uint8_t, 64> buffer_;
        size_t buffer_size_;
        
        void processBlock(const uint8_t* block);
    };
    
    // ========================================
    // تحويل إلى نص / Convert to String
    // ========================================
    
    static std::string toString(const Hash256& hash);
    static Hash256 fromString(const std::string& str);

private:
    // ثوابت SHA-256 / SHA-256 constants
    static const uint32_t K[64];
    
    // دوال مساعدة / Helper functions
    static inline uint32_t rotr(uint32_t x, int n);
    static inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    static inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    static inline uint32_t sigma0(uint32_t x);
    static inline uint32_t sigma1(uint32_t x);
    static inline uint32_t gamma0(uint32_t x);
    static inline uint32_t gamma1(uint32_t x);
};

// ============================================================================
// CodeHasher - أداة Hash موحدة للكود
// CodeHasher - Unified Code Hash Utility
// ============================================================================

class CodeHasher {
public:
    // ========================================
    // المُنشئ / Constructor
    // ========================================
    
    explicit CodeHasher(HashAlgorithm algorithm = HashAlgorithm::XXH64)
        : algorithm_(algorithm)
    {}
    
    // ========================================
    // Hash السريع (موصى به للـ caching)
    // Fast Hash (recommended for caching)
    // ========================================
    
    std::string hashCode(const std::string& source_code) const;
    
    // ========================================
    // Hash الآمن (للتحقق من السلامة)
    // Secure Hash (for integrity verification)
    // ========================================
    
    std::string secureHashCode(const std::string& source_code) const;
    
    // ========================================
    // Hash مع معلومات إضافية
    // Hash with Additional Information
    // ========================================
    
    std::string hashWithMetadata(
        const std::string& source_code,
        const std::string& function_name,
        int optimization_level
    ) const;
    
    // ========================================
    // تغيير الخوارزمية / Change Algorithm
    // ========================================
    
    void setAlgorithm(HashAlgorithm algorithm) {
        algorithm_ = algorithm;
    }
    
    HashAlgorithm getAlgorithm() const {
        return algorithm_;
    }

private:
    HashAlgorithm algorithm_;        // الخوارزمية المستخدمة / Algorithm used
    
    // تحويل hash إلى hex string / Convert hash to hex string
    static std::string toHexString(Hash64 hash);
    static std::string toHexString(const Hash256& hash);
};

// ============================================================================
// دوال مساعدة سريعة / Quick Helper Functions
// ============================================================================

// Hash سريع لـ string / Quick string hash
inline Hash64 quickHash(const std::string& str) {
    return FNV1aHash::hash64(str);
}

// Hash سريع لـ binary data / Quick binary data hash
inline Hash64 quickHash(const void* data, size_t length) {
    return FNV1aHash::hash64(data, length);
}

// Hash آمن / Secure hash
inline std::string secureHash(const std::string& str) {
    Hash256 hash = SHA256::hash(str);
    return SHA256::toString(hash);
}

// دمج hashين / Combine two hashes
inline Hash64 combineHashes(Hash64 h1, Hash64 h2) {
    // طريقة Boost لدمج hashes / Boost's hash combine method
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
}

} // namespace JIT
} // namespace Sad

#endif // SAD_JIT_HASH_H

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
// ملاحظات التنفيذ / Implementation Notes:
// - FNV-1a: الأسرع، مناسب للـ caching / Fastest, suitable for caching
// - xxHash64: متوازن بين السرعة والجودة / Balanced speed and quality
// - CityHash64: جودة عالية جداً / Very high quality
// - SHA-256: الأكثر أماناً / Most secure
// 
// الاستخدام الموصى به / Recommended Usage:
// - للـ JIT caching: FNV-1a أو xxHash64
// - للتحقق من السلامة: SHA-256
// ============================================================================
