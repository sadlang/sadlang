// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: crypto.h
 * الوصف: محرك التشفير والتوقيعات الرقمية لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يوفر:
 *   ● توليد مفاتيح Ed25519
 *   ● التوقيع الرقمي للحزم
 *   ● التحقق من التوقيعات
 *   ● حساب البصمات (SHA256)
 *   ● توليد الأرقام العشوائية الآمنة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

namespace sad {
namespace registry {
namespace crypto {

// ═══════════════════════════════════════════════════════════════════════════════
// الثوابت
// ═══════════════════════════════════════════════════════════════════════════════

constexpr size_t ED25519_PUBLIC_KEY_SIZE = 32;
constexpr size_t ED25519_PRIVATE_KEY_SIZE = 64;
constexpr size_t ED25519_SIGNATURE_SIZE = 64;
constexpr size_t SHA256_DIGEST_SIZE = 32;

// ═══════════════════════════════════════════════════════════════════════════════
// هياكل البيانات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct زوج_المفاتيح
 * @brief زوج مفتاح عام/خاص Ed25519
 */
struct KeyPair {
    std::vector<uint8_t> public_key;   // 32 bytes
    std::vector<uint8_t> private_key;  // 64 bytes
    std::string fingerprint;            // SHA256 hex
};

/**
 * @struct نتيجة_التوقيع
 * @brief نتيجة عملية التوقيع
 */
struct SignatureResult {
    bool success = false;
    std::string signature_base64;
    std::string content_hash;          // SHA256 hex
    std::string error_message;
};

/**
 * @struct نتيجة_التحقق
 * @brief نتيجة التحقق من التوقيع
 */
struct VerificationResult {
    bool valid = false;
    std::string signer_fingerprint;
    std::string error_message;
    bool key_expired = false;
    bool key_revoked = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// دوال التشفير
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief توليد زوج مفاتيح Ed25519 جديد
 */
KeyPair generate_keypair();

/**
 * @brief حساب بصمة SHA256 لمفتاح عام
 */
std::string compute_key_fingerprint(const std::vector<uint8_t>& public_key);

/**
 * @brief حساب SHA256 لبيانات
 */
std::string sha256_hex(const std::vector<uint8_t>& data);
std::string sha256_hex(const std::string& data);

/**
 * @brief حساب SHA256 لملف
 */
std::string sha256_file(const std::string& filepath);

/**
 * @brief توقيع بيانات باستخدام مفتاح خاص Ed25519
 * @param data البيانات للتوقيع
 * @param private_key المفتاح الخاص (64 bytes)
 * @return نتيجة التوقيع
 */
SignatureResult sign_data(const std::vector<uint8_t>& data,
                          const std::vector<uint8_t>& private_key);

/**
 * @brief توقيع ملف
 */
SignatureResult sign_file(const std::string& filepath,
                          const std::vector<uint8_t>& private_key);

/**
 * @brief التحقق من توقيع
 * @param data البيانات الأصلية
 * @param signature التوقيع (Base64)
 * @param public_key المفتاح العام (32 bytes)
 */
VerificationResult verify_signature(const std::vector<uint8_t>& data,
                                    const std::string& signature_base64,
                                    const std::vector<uint8_t>& public_key);

/**
 * @brief التحقق من توقيع ملف
 */
VerificationResult verify_file_signature(const std::string& filepath,
                                         const std::string& signature_base64,
                                         const std::vector<uint8_t>& public_key);

// ═══════════════════════════════════════════════════════════════════════════════
// دوال الترميز/فك الترميز
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تشفير Base64
 */
std::string base64_encode(const std::vector<uint8_t>& data);
std::string base64_encode(const uint8_t* data, size_t len);

/**
 * @brief فك تشفير Base64
 */
std::vector<uint8_t> base64_decode(const std::string& encoded);

/**
 * @brief تحويل إلى hex
 */
std::string to_hex(const std::vector<uint8_t>& data);
std::string to_hex(const uint8_t* data, size_t len);

/**
 * @brief تحويل من hex
 */
std::vector<uint8_t> from_hex(const std::string& hex);

// ═══════════════════════════════════════════════════════════════════════════════
// دوال الأمان
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief توليد بايتات عشوائية آمنة
 */
std::vector<uint8_t> secure_random_bytes(size_t count);

/**
 * @brief توليد رمز عشوائي آمن
 */
std::string generate_secure_token(size_t length = 32);

/**
 * @brief مقارنة آمنة للوقت الثابت
 */
bool secure_compare(const std::string& a, const std::string& b);
bool secure_compare(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);

// ═══════════════════════════════════════════════════════════════════════════════
// دوال كلمات المرور
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تجزئة كلمة مرور (Argon2id أو PBKDF2)
 */
std::string hash_password(const std::string& password);

/**
 * @brief التحقق من كلمة مرور
 */
bool verify_password(const std::string& password, const std::string& hash);

} // namespace crypto
} // namespace registry
} // namespace sad
