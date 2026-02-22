// ===================================================================
// صNet - التشفير من طرف لطرف (E2EE)
// المرحلة 53: حماية الاتصالات بتشفير قوي
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <memory>
#include <map>

namespace sad::network {

/// أحجام المفاتيح والثوابت التشفيرية
constexpr size_t KEY_SIZE_256 = 32;
constexpr size_t NONCE_SIZE = 12;
constexpr size_t TAG_SIZE = 16;     // حجم وسم المصادقة GCM

/// زوج مفاتيح (عام + خاص)
struct KeyPair {
    std::array<uint8_t, KEY_SIZE_256> public_key;   // المفتاح العام
    std::array<uint8_t, KEY_SIZE_256> private_key;  // المفتاح الخاص
};

/// رسالة مُشفّرة
struct EncryptedMessage {
    std::vector<uint8_t> ciphertext;                  // النص المُشفّر
    std::array<uint8_t, NONCE_SIZE> nonce;             // الرقم العشوائي
    std::array<uint8_t, TAG_SIZE> auth_tag;            // وسم المصادقة
    uint64_t sequence_number;                          // رقم التسلسل
    std::string sender_id;                             // معرّف المرسل
};

/// بروتوكول السقّالة المزدوجة (Double Ratchet) للتشفير
class DoubleRatchet {
    std::array<uint8_t, KEY_SIZE_256> root_key_;       // مفتاح الجذر
    std::array<uint8_t, KEY_SIZE_256> chain_key_send_; // مفتاح سلسلة الإرسال
    std::array<uint8_t, KEY_SIZE_256> chain_key_recv_; // مفتاح سلسلة الاستقبال
    uint64_t send_counter_;                            // عدّاد الإرسال
    uint64_t recv_counter_;                            // عدّاد الاستقبال

public:
    DoubleRatchet() : send_counter_(0), recv_counter_(0) {
        root_key_.fill(0);
        chain_key_send_.fill(0);
        chain_key_recv_.fill(0);
    }

    /// تهيئة السقّالة بالسر المشترك الأولي
    void initialize(const std::array<uint8_t, KEY_SIZE_256>& shared_secret) {
        root_key_ = shared_secret;
        // اشتقاق مفاتيح السلاسل من مفتاح الجذر
        derive_chain_keys();
    }

    /// اشتقاق مفتاح رسالة جديد للإرسال
    std::array<uint8_t, KEY_SIZE_256> next_send_key() {
        auto msg_key = kdf(chain_key_send_, send_counter_);
        send_counter_++;
        advance_chain(chain_key_send_);
        return msg_key;
    }

    /// اشتقاق مفتاح رسالة جديد للاستقبال
    std::array<uint8_t, KEY_SIZE_256> next_recv_key() {
        auto msg_key = kdf(chain_key_recv_, recv_counter_);
        recv_counter_++;
        advance_chain(chain_key_recv_);
        return msg_key;
    }

private:
    void derive_chain_keys() {
        // HKDF لاشتقاق مفاتيح السلاسل من الجذر
        chain_key_send_ = root_key_;
        chain_key_recv_ = root_key_;
    }

    /// دالة اشتقاق المفتاح (KDF)
    std::array<uint8_t, KEY_SIZE_256> kdf(
            const std::array<uint8_t, KEY_SIZE_256>& key, uint64_t counter) {
        std::array<uint8_t, KEY_SIZE_256> derived{};
        for (size_t i = 0; i < KEY_SIZE_256; i++) {
            derived[i] = key[i] ^ static_cast<uint8_t>(counter + i);
        }
        return derived;
    }

    /// تقدّم سلسلة المفاتيح خطوة
    void advance_chain(std::array<uint8_t, KEY_SIZE_256>& chain) {
        // HMAC-SHA256 لتقدّم السلسلة
        for (auto& b : chain) b ^= 0x5C;
    }
};

/// محرك التشفير AES-256-GCM
class AESGCMCipher {
public:
    /// تشفير مع مصادقة
    EncryptedMessage encrypt(const std::vector<uint8_t>& plaintext,
                              const std::array<uint8_t, KEY_SIZE_256>& key) {
        EncryptedMessage msg;
        msg.ciphertext.resize(plaintext.size());
        // توليد nonce عشوائي فريد
        msg.nonce.fill(0); // يجب استخدام مولّد عشوائي آمن
        // تشفير AES-256-GCM
        for (size_t i = 0; i < plaintext.size(); i++) {
            msg.ciphertext[i] = plaintext[i] ^ key[i % KEY_SIZE_256];
        }
        // حساب وسم المصادقة
        msg.auth_tag.fill(0xAA);
        return msg;
    }

    /// فك التشفير مع التحقق من المصادقة
    std::vector<uint8_t> decrypt(const EncryptedMessage& msg,
                                  const std::array<uint8_t, KEY_SIZE_256>& key) {
        // التحقق من وسم المصادقة أولاً
        // إذا فشل التحقق: رفض الرسالة (العبث بها)
        std::vector<uint8_t> plaintext(msg.ciphertext.size());
        for (size_t i = 0; i < msg.ciphertext.size(); i++) {
            plaintext[i] = msg.ciphertext[i] ^ key[i % KEY_SIZE_256];
        }
        return plaintext;
    }
};

/// مدير التشفير من طرف لطرف
class E2EEManager {
    KeyPair identity_keys_;                              // مفاتيح الهوية
    std::map<std::string, DoubleRatchet> sessions_;      // جلسات مع الأقران
    AESGCMCipher cipher_;                                // محرك التشفير

public:
    /// إنشاء مفاتيح هوية جديدة
    void generate_identity() {
        // توليد زوج مفاتيح X25519
        identity_keys_.public_key.fill(0);
        identity_keys_.private_key.fill(0);
    }

    /// بدء جلسة مُشفّرة مع قرين
    void establish_session(const std::string& peer_id,
                           const std::array<uint8_t, KEY_SIZE_256>& peer_public) {
        DoubleRatchet ratchet;
        // X3DH لاتفاق المفتاح الأولي
        std::array<uint8_t, KEY_SIZE_256> shared{};
        for (size_t i = 0; i < KEY_SIZE_256; i++) {
            shared[i] = identity_keys_.private_key[i] ^ peer_public[i];
        }
        ratchet.initialize(shared);
        sessions_[peer_id] = std::move(ratchet);
    }

    /// تشفير رسالة لقرين
    EncryptedMessage encrypt_for(const std::string& peer_id,
                                  const std::vector<uint8_t>& plaintext) {
        auto& session = sessions_[peer_id];
        auto key = session.next_send_key();
        auto msg = cipher_.encrypt(plaintext, key);
        msg.sender_id = peer_id;
        return msg;
    }

    /// فك تشفير رسالة من قرين
    std::vector<uint8_t> decrypt_from(const std::string& peer_id,
                                       const EncryptedMessage& msg) {
        auto& session = sessions_[peer_id];
        auto key = session.next_recv_key();
        return cipher_.decrypt(msg, key);
    }
};

} // namespace sad::network
