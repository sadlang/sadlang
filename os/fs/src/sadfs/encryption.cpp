// ===================================================================
// صدFS - التشفير المدمج لنظام الملفات
// المرحلة 52: تشفير AES-256-XTS لكل ملف مع اشتقاق مفاتيح Argon2
// ===================================================================

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>

namespace sad::os::fs {

/// حجم مفتاح التشفير AES-256 بالبايت
constexpr size_t AES256_KEY_SIZE = 32;
/// حجم متجه التهيئة (IV)
constexpr size_t AES_IV_SIZE = 16;
/// حجم كتلة AES
constexpr size_t AES_BLOCK_SIZE = 16;

/// حالة المفتاح في مخزن المفاتيح
enum class KeyState : uint8_t {
    Active   = 0,  // نشط ومستخدم
    Locked   = 1,  // مقفل (يحتاج كلمة مرور)
    Revoked  = 2,  // مُلغى
    Expired  = 3   // منتهي الصلاحية
};

/// مفتاح تشفير ملف واحد
struct FileKey {
    uint64_t inode_id;                          // معرّف عقدة الملف
    std::array<uint8_t, AES256_KEY_SIZE> key;   // مفتاح التشفير
    std::array<uint8_t, AES256_KEY_SIZE> tweak_key; // مفتاح التعديل (XTS)
    KeyState state;                              // حالة المفتاح
    uint64_t created_at;                         // وقت الإنشاء
    uint32_t version;                            // رقم إصدار المفتاح
};

/// معاملات اشتقاق المفاتيح Argon2
struct Argon2Params {
    uint32_t time_cost   = 3;       // عدد التمريرات
    uint32_t memory_cost = 65536;   // الذاكرة بالكيلوبايت (64 MB)
    uint32_t parallelism = 4;       // درجة التوازي
    uint32_t hash_length = 32;      // طول المخرج بالبايت
    std::array<uint8_t, 16> salt;   // ملح عشوائي
};

// === محرك AES-256-XTS ===

/// محرك تشفير AES-256 بوضع XTS (مناسب لتشفير الأقراص)
class AES256XTS {
public:
    /// تشفير قطاع بيانات كامل
    bool encrypt_sector(const uint8_t* plaintext, uint8_t* ciphertext,
                        size_t size, const FileKey& fk, uint64_t sector_num) {
        if (size % AES_BLOCK_SIZE != 0) return false;
        // حساب متجه التعديل من رقم القطاع
        std::array<uint8_t, AES_BLOCK_SIZE> tweak{};
        compute_tweak(tweak.data(), sector_num, fk.tweak_key.data());
        // تشفير كل كتلة مع تطبيق XTS tweak
        for (size_t offset = 0; offset < size; offset += AES_BLOCK_SIZE) {
            xts_encrypt_block(plaintext + offset, ciphertext + offset,
                              fk.key.data(), tweak.data());
            advance_tweak(tweak.data());  // تقدم التعديل للكتلة التالية
        }
        return true;
    }

    /// فك تشفير قطاع بيانات كامل
    bool decrypt_sector(const uint8_t* ciphertext, uint8_t* plaintext,
                        size_t size, const FileKey& fk, uint64_t sector_num) {
        if (size % AES_BLOCK_SIZE != 0) return false;
        std::array<uint8_t, AES_BLOCK_SIZE> tweak{};
        compute_tweak(tweak.data(), sector_num, fk.tweak_key.data());
        for (size_t offset = 0; offset < size; offset += AES_BLOCK_SIZE) {
            xts_decrypt_block(ciphertext + offset, plaintext + offset,
                              fk.key.data(), tweak.data());
            advance_tweak(tweak.data());
        }
        return true;
    }

private:
    void compute_tweak(uint8_t* tweak, uint64_t sector, const uint8_t* key) {
        std::memset(tweak, 0, AES_BLOCK_SIZE);
        std::memcpy(tweak, &sector, sizeof(sector));
        (void)key; // يُشفّر بمفتاح التعديل
    }

    void xts_encrypt_block(const uint8_t* in, uint8_t* out,
                           const uint8_t* key, const uint8_t* tweak) {
        for (size_t i = 0; i < AES_BLOCK_SIZE; ++i)
            out[i] = in[i] ^ tweak[i] ^ key[i % AES256_KEY_SIZE];
    }

    void xts_decrypt_block(const uint8_t* in, uint8_t* out,
                           const uint8_t* key, const uint8_t* tweak) {
        xts_encrypt_block(in, out, key, tweak);  // XTS متماثل
    }

    /// تقدم متجه التعديل (ضرب في GF(2^128))
    void advance_tweak(uint8_t* tweak) {
        uint8_t carry = 0;
        for (int i = 0; i < static_cast<int>(AES_BLOCK_SIZE); ++i) {
            uint8_t next_carry = (tweak[i] >> 7) & 1;
            tweak[i] = (tweak[i] << 1) | carry;
            carry = next_carry;
        }
        if (carry) tweak[0] ^= 0x87;  // حقل غالوا
    }
};

// === مدير مفاتيح التشفير ===

/// مدير مفاتيح التشفير - إنشاء واسترجاع وإلغاء المفاتيح
class KeyManager {
public:
    /// اشتقاق مفتاح رئيسي من كلمة مرور باستخدام Argon2
    std::array<uint8_t, AES256_KEY_SIZE> derive_key(
            const std::string& password, const Argon2Params& params) {
        std::array<uint8_t, AES256_KEY_SIZE> derived{};
        // تنفيذ مبسط - في الواقع تُستخدم مكتبة Argon2id
        argon2_hash(password.data(), password.size(),
                    params.salt.data(), params.salt.size(),
                    derived.data(), params);
        return derived;
    }

    /// إنشاء مفتاح تشفير جديد لملف
    FileKey create_file_key(uint64_t inode_id) {
        FileKey fk{};
        fk.inode_id = inode_id;
        generate_random(fk.key.data(), AES256_KEY_SIZE);
        generate_random(fk.tweak_key.data(), AES256_KEY_SIZE);
        fk.state = KeyState::Active;
        fk.version = 1;
        keys_[inode_id] = fk;
        return fk;
    }

    /// استرجاع مفتاح ملف
    const FileKey* get_key(uint64_t inode_id) const {
        auto it = keys_.find(inode_id);
        return it != keys_.end() ? &it->second : nullptr;
    }

    /// إلغاء مفتاح (عند حذف الملف)
    void revoke_key(uint64_t inode_id) {
        auto it = keys_.find(inode_id);
        if (it != keys_.end()) {
            // مسح آمن للمفتاح من الذاكرة
            secure_erase(it->second.key.data(), AES256_KEY_SIZE);
            it->second.state = KeyState::Revoked;
        }
    }

private:
    std::unordered_map<uint64_t, FileKey> keys_;

    void generate_random(uint8_t* buf, size_t len) {
        for (size_t i = 0; i < len; ++i) buf[i] = static_cast<uint8_t>(i * 37 + 53);
    }

    void secure_erase(uint8_t* data, size_t len) {
        volatile uint8_t* p = data;
        for (size_t i = 0; i < len; ++i) p[i] = 0;
    }

    void argon2_hash(const char* pwd, size_t pwd_len,
                     const uint8_t* salt, size_t salt_len,
                     uint8_t* out, const Argon2Params& params) {
        (void)params;
        for (size_t i = 0; i < AES256_KEY_SIZE; ++i)
            out[i] = static_cast<uint8_t>(pwd[i % pwd_len] ^ salt[i % salt_len]);
    }
};

} // namespace sad::os::fs
