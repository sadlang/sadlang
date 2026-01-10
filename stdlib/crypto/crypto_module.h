// بسم الله الرحمن الرحيم
/**
 * @file crypto_module.h
 * @brief Cryptography Module - وحدة التشفير
 * 
 * This module provides comprehensive cryptographic functions for Sad Language.
 * هذه الوحدة توفر دوال تشفير شاملة للغة الحزينة
 * 
 * Features / المميزات:
 * - Hash functions (MD5, SHA-1, SHA-256, SHA-512) / دوال التجزئة
 * - Symmetric encryption (AES) / التشفير المتماثل
 * - Asymmetric encryption (RSA) / التشفير غير المتماثل
 * - Digital signatures / التوقيعات الرقمية
 * - Random number generation / توليد الأرقام العشوائية
 * - Password hashing (bcrypt, PBKDF2) / تجزئة كلمات المرور
 * - HMAC authentication / المصادقة HMAC
 * - Base64 encoding/decoding / ترميز Base64
 * - Hex encoding/decoding / ترميز Hex
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <cstdint>
#include <stdexcept>

namespace sad {
namespace stdlib {
namespace crypto {

// ============================================================================
// Type Definitions - تعريفات الأنواع
// ============================================================================

using ByteArray = std::vector<uint8_t>;

/**
 * @brief Hash algorithm types - أنواع خوارزميات التجزئة
 */
enum class HashAlgorithm {
    MD5,        // 128-bit
    SHA1,       // 160-bit
    SHA224,     // 224-bit
    SHA256,     // 256-bit
    SHA384,     // 384-bit
    SHA512      // 512-bit
};

/**
 * @brief Cipher modes - أوضاع التشفير
 */
enum class CipherMode {
    ECB,        // Electronic Codebook
    CBC,        // Cipher Block Chaining
    CTR,        // Counter
    GCM         // Galois/Counter Mode
};

/**
 * @brief Padding modes - أوضاع الحشو
 */
enum class PaddingMode {
    PKCS7,
    ZERO,
    NONE
};

/**
 * @brief Key sizes - أحجام المفاتيح
 */
enum class KeySize {
    AES_128 = 128,
    AES_192 = 192,
    AES_256 = 256,
    RSA_1024 = 1024,
    RSA_2048 = 2048,
    RSA_4096 = 4096
};

// ============================================================================
// Exception Classes - فئات الاستثناءات
// ============================================================================

/**
 * @brief Cryptography exception - استثناء التشفير
 */
class CryptoException : public std::runtime_error {
public:
    explicit CryptoException(const std::string& message)
        : std::runtime_error(message) {}
};

// ============================================================================
// Hash Functions - دوال التجزئة
// ============================================================================

/**
 * @brief Hash class for cryptographic hash functions
 * فئة التجزئة لدوال التجزئة التشفيرية
 */
class Hash {
public:
    /**
     * @brief Compute hash of data
     * حساب تجزئة البيانات
     */
    static ByteArray compute(HashAlgorithm algo, const ByteArray& data);
    static ByteArray compute(HashAlgorithm algo, const std::string& data);
    
    /**
     * @brief Compute hash and return as hex string
     * حساب التجزئة وإرجاعها كنص hex
     */
    static std::string compute_hex(HashAlgorithm algo, const ByteArray& data);
    static std::string compute_hex(HashAlgorithm algo, const std::string& data);
    
    /**
     * @brief Verify hash
     * التحقق من التجزئة
     */
    static bool verify(HashAlgorithm algo, const ByteArray& data, const ByteArray& hash);
    static bool verify(HashAlgorithm algo, const std::string& data, const std::string& hash_hex);
    
    /**
     * @brief Get hash size in bytes
     * الحصول على حجم التجزئة بالبايتات
     */
    static size_t get_size(HashAlgorithm algo);
};

// ============================================================================
// HMAC - Hash-based Message Authentication Code
// ============================================================================

/**
 * @brief HMAC class for message authentication
 * فئة HMAC للمصادقة على الرسائل
 */
class HMAC {
public:
    /**
     * @brief Compute HMAC
     * حساب HMAC
     */
    static ByteArray compute(HashAlgorithm algo, const ByteArray& key, const ByteArray& data);
    static std::string compute_hex(HashAlgorithm algo, const std::string& key, const std::string& data);
    
    /**
     * @brief Verify HMAC
     * التحقق من HMAC
     */
    static bool verify(HashAlgorithm algo, const ByteArray& key, const ByteArray& data, const ByteArray& mac);
    static bool verify(HashAlgorithm algo, const std::string& key, const std::string& data, const std::string& mac_hex);
};

// ============================================================================
// Symmetric Encryption (AES) - التشفير المتماثل
// ============================================================================

/**
 * @brief AES encryption/decryption class
 * فئة تشفير/فك تشفير AES
 */
class AES {
public:
    /**
     * @brief Encrypt data
     * تشفير البيانات
     */
    static ByteArray encrypt(
        const ByteArray& data,
        const ByteArray& key,
        const ByteArray& iv,
        CipherMode mode = CipherMode::CBC,
        PaddingMode padding = PaddingMode::PKCS7
    );
    
    /**
     * @brief Decrypt data
     * فك تشفير البيانات
     */
    static ByteArray decrypt(
        const ByteArray& data,
        const ByteArray& key,
        const ByteArray& iv,
        CipherMode mode = CipherMode::CBC,
        PaddingMode padding = PaddingMode::PKCS7
    );
    
    /**
     * @brief Encrypt string (returns base64)
     * تشفير نص (يرجع base64)
     */
    static std::string encrypt_string(
        const std::string& plaintext,
        const std::string& key,
        const std::string& iv,
        CipherMode mode = CipherMode::CBC
    );
    
    /**
     * @brief Decrypt string (from base64)
     * فك تشفير نص (من base64)
     */
    static std::string decrypt_string(
        const std::string& ciphertext_base64,
        const std::string& key,
        const std::string& iv,
        CipherMode mode = CipherMode::CBC
    );
    
    /**
     * @brief Generate random key
     * توليد مفتاح عشوائي
     */
    static ByteArray generate_key(KeySize size = KeySize::AES_256);
    
    /**
     * @brief Generate random IV
     * توليد IV عشوائي
     */
    static ByteArray generate_iv();
};

// ============================================================================
// RSA Key Pair - زوج مفاتيح RSA
// ============================================================================

/**
 * @brief RSA key pair structure
 * هيكل زوج مفاتيح RSA
 */
struct RSAKeyPair {
    std::string public_key;   // PEM format
    std::string private_key;  // PEM format
};

/**
 * @brief RSA encryption/decryption class
 * فئة تشفير/فك تشفير RSA
 */
class RSA {
public:
    /**
     * @brief Generate RSA key pair
     * توليد زوج مفاتيح RSA
     */
    static RSAKeyPair generate_key_pair(KeySize size = KeySize::RSA_2048);
    
    /**
     * @brief Encrypt with public key
     * التشفير بالمفتاح العام
     */
    static ByteArray encrypt(const ByteArray& data, const std::string& public_key);
    static std::string encrypt_string(const std::string& plaintext, const std::string& public_key);
    
    /**
     * @brief Decrypt with private key
     * فك التشفير بالمفتاح الخاص
     */
    static ByteArray decrypt(const ByteArray& data, const std::string& private_key);
    static std::string decrypt_string(const std::string& ciphertext_base64, const std::string& private_key);
    
    /**
     * @brief Sign data with private key
     * توقيع البيانات بالمفتاح الخاص
     */
    static ByteArray sign(const ByteArray& data, const std::string& private_key, HashAlgorithm algo = HashAlgorithm::SHA256);
    static std::string sign_string(const std::string& data, const std::string& private_key, HashAlgorithm algo = HashAlgorithm::SHA256);
    
    /**
     * @brief Verify signature with public key
     * التحقق من التوقيع بالمفتاح العام
     */
    static bool verify(const ByteArray& data, const ByteArray& signature, const std::string& public_key, HashAlgorithm algo = HashAlgorithm::SHA256);
    static bool verify_string(const std::string& data, const std::string& signature_base64, const std::string& public_key, HashAlgorithm algo = HashAlgorithm::SHA256);
};

// ============================================================================
// Password Hashing - تجزئة كلمات المرور
// ============================================================================

/**
 * @brief Password hashing class
 * فئة تجزئة كلمات المرور
 */
class Password {
public:
    /**
     * @brief Hash password using PBKDF2
     * تجزئة كلمة المرور باستخدام PBKDF2
     */
    static std::string hash_pbkdf2(
        const std::string& password,
        const std::string& salt,
        int iterations = 100000,
        HashAlgorithm algo = HashAlgorithm::SHA256
    );
    
    /**
     * @brief Verify password against PBKDF2 hash
     * التحقق من كلمة المرور مقابل تجزئة PBKDF2
     */
    static bool verify_pbkdf2(
        const std::string& password,
        const std::string& hash,
        const std::string& salt,
        int iterations = 100000,
        HashAlgorithm algo = HashAlgorithm::SHA256
    );
    
    /**
     * @brief Hash password using bcrypt
     * تجزئة كلمة المرور باستخدام bcrypt
     */
    static std::string hash_bcrypt(const std::string& password, int cost = 12);
    
    /**
     * @brief Verify password against bcrypt hash
     * التحقق من كلمة المرور مقابل تجزئة bcrypt
     */
    static bool verify_bcrypt(const std::string& password, const std::string& hash);
    
    /**
     * @brief Generate random salt
     * توليد salt عشوائي
     */
    static std::string generate_salt(size_t length = 32);
};

// ============================================================================
// Random Number Generation - توليد الأرقام العشوائية
// ============================================================================

/**
 * @brief Random number generator class
 * فئة مولد الأرقام العشوائية
 */
class Random {
public:
    /**
     * @brief Generate random bytes
     * توليد بايتات عشوائية
     */
    static ByteArray bytes(size_t length);
    
    /**
     * @brief Generate random integer
     * توليد عدد صحيح عشوائي
     */
    static int32_t int32();
    static int32_t int32(int32_t min, int32_t max);
    
    /**
     * @brief Generate random 64-bit integer
     * توليد عدد صحيح 64-بت عشوائي
     */
    static int64_t int64();
    static int64_t int64(int64_t min, int64_t max);
    
    /**
     * @brief Generate random double [0.0, 1.0)
     * توليد عدد عشري عشوائي
     */
    static double number();
    static double number(double min, double max);
    
    /**
     * @brief Generate random hex string
     * توليد نص hex عشوائي
     */
    static std::string hex(size_t length);
    
    /**
     * @brief Generate random alphanumeric string
     * توليد نص أبجدي رقمي عشوائي
     */
    static std::string alphanumeric(size_t length);
    
    /**
     * @brief Generate UUID v4
     * توليد UUID إصدار 4
     */
    static std::string uuid();
};

// ============================================================================
// Encoding/Decoding - الترميز/فك الترميز
// ============================================================================

/**
 * @brief Base64 encoding/decoding
 * ترميز/فك ترميز Base64
 */
class Base64 {
public:
    static std::string encode(const ByteArray& data);
    static std::string encode(const std::string& data);
    
    static ByteArray decode(const std::string& encoded);
    static std::string decode_string(const std::string& encoded);
    
    static bool is_valid(const std::string& encoded);
};

/**
 * @brief Hexadecimal encoding/decoding
 * ترميز/فك ترميز Hexadecimal
 */
class Hex {
public:
    static std::string encode(const ByteArray& data);
    static std::string encode(const std::string& data);
    
    static ByteArray decode(const std::string& hex);
    static std::string decode_string(const std::string& hex);
    
    static bool is_valid(const std::string& hex);
};

// ============================================================================
// Utility Functions - الدوال المساعدة
// ============================================================================

/**
 * @brief Convert string to byte array
 * تحويل نص إلى مصفوفة بايتات
 */
ByteArray string_to_bytes(const std::string& str);

/**
 * @brief Convert byte array to string
 * تحويل مصفوفة بايتات إلى نص
 */
std::string bytes_to_string(const ByteArray& bytes);

/**
 * @brief Constant-time comparison (timing attack resistant)
 * مقارنة ثابتة الوقت (مقاومة لهجمات التوقيت)
 */
bool constant_time_compare(const ByteArray& a, const ByteArray& b);
bool constant_time_compare(const std::string& a, const std::string& b);

/**
 * @brief Get algorithm name as string
 * الحصول على اسم الخوارزمية كنص
 */
std::string hash_algorithm_to_string(HashAlgorithm algo);
std::string cipher_mode_to_string(CipherMode mode);
std::string key_size_to_string(KeySize size);

/**
 * @brief Secure memory wipe
 * مسح آمن للذاكرة
 */
void secure_wipe(ByteArray& data);
void secure_wipe(std::string& data);

} // namespace crypto
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
