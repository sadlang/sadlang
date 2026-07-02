// ===================================================================
// صNet — وحدة التشفير: SHA-256, AES-256-GCM, X25519, HKDF
// (AR) تنفيذ ذاتي بالكامل — لا يعتمد على مكتبات خارجية.
//      SHA-256: تنفيذ كامل حسب FIPS 180-4
//      AES-256-GCM: تنفيذ CTR + GHASH
//      X25519: Curve25519 Diffie-Hellman
//      HKDF: اشتقاق مفاتيح حسب RFC 5869
//      CSPRNG: توليد بايتات عشوائية آمنة (OS-backed)
// (EN) Self-contained crypto: SHA-256, AES-256-GCM, X25519, HKDF, CSPRNG
// المكون: features/network/sadnet (التشفير)
// المسار: features/network/sadnet/include/sadnet/crypto/crypto.h
// ===================================================================
#ifndef SAD_SADNET_CRYPTO_CRYPTO_H
#define SAD_SADNET_CRYPTO_CRYPTO_H

#include "sadnet/core/types.h"
#include <array>
#include <cstdint>
#include <memory>
#include <vector>

namespace sad::net::crypto
{

    // ─────────────────────────────────────────────────────────────────────
    // (AR) توليد بايتات عشوائية آمنة (CSPRNG)
    //      يستخدم BCryptGenRandom على ويندوز و /dev/urandom على لينكس
    // ─────────────────────────────────────────────────────────────────────

    /// توليد n بايت عشوائية آمنة تشفيرياً
    bool random_bytes(uint8_t *out, size_t n);

    /// توليد Buffer عشوائي
    Buffer random_buffer(size_t n);

    // ─────────────────────────────────────────────────────────────────────
    // (AR) SHA-256 حسب FIPS 180-4
    // ─────────────────────────────────────────────────────────────────────

    using Hash256 = std::array<uint8_t, SHA256_SIZE>;

    /// حساب بصمة SHA-256 لبيانات
    Hash256 sha256(const uint8_t *data, size_t len);
    Hash256 sha256(const Buffer &buf);
    Hash256 sha256(const std::string &str);

    /// HMAC-SHA256 (RFC 2104)
    Hash256 hmac_sha256(const uint8_t *key, size_t key_len,
                        const uint8_t *data, size_t data_len);

    // ─────────────────────────────────────────────────────────────────────
    // (AR) HKDF — اشتقاق مفاتيح حسب RFC 5869
    // ─────────────────────────────────────────────────────────────────────

    /// استخراج وتوسيع مفتاح باستخدام HKDF-SHA256
    /// salt: ملح (32 بايت، اختياري)
    /// ikm: مادة المفتاح الأولية
    /// info: معلومات السياق
    /// out_len: طول المفتاح المُشتق المطلوب
    Buffer hkdf_sha256(const uint8_t *salt, size_t salt_len,
                       const uint8_t *ikm, size_t ikm_len,
                       const uint8_t *info, size_t info_len,
                       size_t out_len);

    // ─────────────────────────────────────────────────────────────────────
    // (AR) AES-256-GCM — تشفير مُصادق (AEAD)
    // ─────────────────────────────────────────────────────────────────────

    /// نتيجة التشفير: (نص مُشفّر + nonce + وسم مصادقة)
    struct AEADResult
    {
        Buffer ciphertext;
        std::array<uint8_t, AES_NONCE_SIZE> nonce;
        std::array<uint8_t, AES_TAG_SIZE> tag;
    };

    /// تشفير AES-256-GCM مع توليد nonce عشوائي تلقائي
    AEADResult aes_gcm_encrypt(const uint8_t *key,
                               const uint8_t *plaintext, size_t pt_len,
                               const uint8_t *aad = nullptr, size_t aad_len = 0);

    /// تشفير AES-256-GCM مع nonce خارجي (للاختبارات المعيارية)
    AEADResult aes_gcm_encrypt_with_nonce(const uint8_t *key,
                                          const uint8_t *nonce,
                                          const uint8_t *plaintext, size_t pt_len,
                                          const uint8_t *aad = nullptr, size_t aad_len = 0);

    /// فك تشفير AES-256-GCM مع التحقق من وسم المصادقة
    /// يُرجع Buffer فارغاً إذا فشل التحقق (العبث بالبيانات)
    Buffer aes_gcm_decrypt(const uint8_t *key,
                           const uint8_t *ciphertext, size_t ct_len,
                           const uint8_t *nonce,
                           const uint8_t *tag,
                           const uint8_t *aad = nullptr, size_t aad_len = 0);

    // ─────────────────────────────────────────────────────────────────────
    // (AR) X25519 — تبادل مفاتيح Diffie-Hellman على Curve25519
    // ─────────────────────────────────────────────────────────────────────

    using X25519Key = std::array<uint8_t, X25519_KEY_SIZE>;

    /// زوج مفاتيح X25519
    struct KeyPair
    {
        X25519Key public_key;
        X25519Key private_key;
    };

    /// توليد زوج مفاتيح X25519 عشوائي
    KeyPair x25519_generate_keypair();

    /// حساب السر المشترك من المفتاح الخاص ومفتاح الطرف الآخر العام
    X25519Key x25519_shared_secret(const X25519Key &my_private,
                                   const X25519Key &their_public);

    // ─────────────────────────────────────────────────────────────────────
    // (AR) بروتوكول السقّالة المزدوجة (Double Ratchet) — Signal Protocol
    //      يضمن:
    //      1. Forward secrecy: اختراق مفتاح حالي لا يكشف الرسائل السابقة
    //      2. Post-compromise security: تتعافى الأمان تلقائياً
    // ─────────────────────────────────────────────────────────────────────

    /// حالة جلسة Double Ratchet
    class DoubleRatchetSession
    {
    public:
        DoubleRatchetSession();
        ~DoubleRatchetSession();

        /// تهيئة كمرسل (أليس)
        void init_sender(const X25519Key &shared_secret,
                         const X25519Key &receiver_public);

        /// تهيئة كمستقبل (بوب)
        void init_receiver(const X25519Key &shared_secret,
                           const KeyPair &our_keypair);

        /// تشفير رسالة (يتقدم الـ ratchet تلقائياً)
        struct EncryptedPacket
        {
            Buffer ciphertext;
            Buffer header; // يحتوي على المفتاح العام + أرقام السلاسل
        };
        EncryptedPacket encrypt(const uint8_t *plaintext, size_t len);
        EncryptedPacket encrypt(const Buffer &plaintext);

        /// فك تشفير رسالة
        Buffer decrypt(const EncryptedPacket &packet);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::crypto

#endif // SAD_SADNET_CRYPTO_CRYPTO_H
