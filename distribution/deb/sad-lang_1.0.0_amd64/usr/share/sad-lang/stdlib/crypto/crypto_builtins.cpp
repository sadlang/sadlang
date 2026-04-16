// بسم الله الرحمن الرحيم
/**
 * @file crypto_builtins.cpp
 * @brief Cryptography Builtin Functions - دوال التشفير المدمجة
 * 
 * الحمد لله رب العالمين
 */

#include "crypto/crypto_module.h"
#include "data/types/value.h"
#include <memory>

using namespace sad;
using namespace sad::stdlib::crypto;
using namespace Sad::Data;

namespace {

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

HashAlgorithm string_to_hash_algo(const std::string& str) {
    if (str == "md5" || str == "MD5") return HashAlgorithm::MD5;
    if (str == "sha1" || str == "SHA1") return HashAlgorithm::SHA1;
    if (str == "sha224" || str == "SHA224") return HashAlgorithm::SHA224;
    if (str == "sha256" || str == "SHA256") return HashAlgorithm::SHA256;
    if (str == "sha384" || str == "SHA384") return HashAlgorithm::SHA384;
    if (str == "sha512" || str == "SHA512") return HashAlgorithm::SHA512;
    return HashAlgorithm::SHA256; // Default
}

CipherMode string_to_cipher_mode(const std::string& str) {
    if (str == "ecb" || str == "ECB") return CipherMode::ECB;
    if (str == "cbc" || str == "CBC") return CipherMode::CBC;
    if (str == "ctr" || str == "CTR") return CipherMode::CTR;
    if (str == "gcm" || str == "GCM") return CipherMode::GCM;
    return CipherMode::CBC; // Default
}

// ============================================================================
// Hash Functions - دوال التجزئة
// ============================================================================

Value builtin_hash(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("hash requires algorithm and data / hash يتطلب خوارزمية وبيانات");
    }
    
    std::string algo_str = args[0].as_string();
    std::string data = args[1].as_string();
    
    HashAlgorithm algo = string_to_hash_algo(algo_str);
    std::string hash_hex = Hash::compute_hex(algo, data);
    
    return Value(hash_hex);
}

Value builtin_md5(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("md5 requires data / md5 يتطلب بيانات");
    }
    
    std::string data = args[0].as_string();
    std::string hash = Hash::compute_hex(HashAlgorithm::MD5, data);
    
    return Value(hash);
}

Value builtin_sha1(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("sha1 requires data / sha1 يتطلب بيانات");
    }
    
    std::string data = args[0].as_string();
    std::string hash = Hash::compute_hex(HashAlgorithm::SHA1, data);
    
    return Value(hash);
}

Value builtin_sha256(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("sha256 requires data / sha256 يتطلب بيانات");
    }
    
    std::string data = args[0].as_string();
    std::string hash = Hash::compute_hex(HashAlgorithm::SHA256, data);
    
    return Value(hash);
}

Value builtin_sha512(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("sha512 requires data / sha512 يتطلب بيانات");
    }
    
    std::string data = args[0].as_string();
    std::string hash = Hash::compute_hex(HashAlgorithm::SHA512, data);
    
    return Value(hash);
}

Value builtin_hash_verify(const std::vector<Value>& args) {
    if (args.size() < 3) {
        throw std::runtime_error("hash_verify requires algorithm, data, and hash / hash_verify يتطلب خوارزمية وبيانات وتجزئة");
    }
    
    std::string algo_str = args[0].as_string();
    std::string data = args[1].as_string();
    std::string hash = args[2].as_string();
    
    HashAlgorithm algo = string_to_hash_algo(algo_str);
    bool valid = Hash::verify(algo, data, hash);
    
    return Value(valid);
}

// ============================================================================
// HMAC Functions - دوال HMAC
// ============================================================================

Value builtin_hmac(const std::vector<Value>& args) {
    if (args.size() < 3) {
        throw std::runtime_error("hmac requires algorithm, key, and data / hmac يتطلب خوارزمية ومفتاح وبيانات");
    }
    
    std::string algo_str = args[0].as_string();
    std::string key = args[1].as_string();
    std::string data = args[2].as_string();
    
    HashAlgorithm algo = string_to_hash_algo(algo_str);
    std::string mac = HMAC::compute_hex(algo, key, data);
    
    return Value(mac);
}

Value builtin_hmac_verify(const std::vector<Value>& args) {
    if (args.size() < 4) {
        throw std::runtime_error("hmac_verify requires algorithm, key, data, and mac / hmac_verify يتطلب خوارزمية ومفتاح وبيانات وmac");
    }
    
    std::string algo_str = args[0].as_string();
    std::string key = args[1].as_string();
    std::string data = args[2].as_string();
    std::string mac = args[3].as_string();
    
    HashAlgorithm algo = string_to_hash_algo(algo_str);
    bool valid = HMAC::verify(algo, key, data, mac);
    
    return Value(valid);
}

// ============================================================================
// AES Functions - دوال AES
// ============================================================================

Value builtin_aes_encrypt(const std::vector<Value>& args) {
    if (args.size() < 3) {
        throw std::runtime_error("aes_encrypt requires data, key, and iv / aes_encrypt يتطلب بيانات ومفتاح وiv");
    }
    
    std::string plaintext = args[0].as_string();
    std::string key = args[1].as_string();
    std::string iv = args[2].as_string();
    
    CipherMode mode = CipherMode::CBC;
    if (args.size() >= 4) {
        mode = string_to_cipher_mode(args[3].as_string());
    }
    
    std::string ciphertext = AES::encrypt_string(plaintext, key, iv, mode);
    
    return Value(ciphertext);
}

Value builtin_aes_decrypt(const std::vector<Value>& args) {
    if (args.size() < 3) {
        throw std::runtime_error("aes_decrypt requires data, key, and iv / aes_decrypt يتطلب بيانات ومفتاح وiv");
    }
    
    std::string ciphertext = args[0].as_string();
    std::string key = args[1].as_string();
    std::string iv = args[2].as_string();
    
    CipherMode mode = CipherMode::CBC;
    if (args.size() >= 4) {
        mode = string_to_cipher_mode(args[3].as_string());
    }
    
    std::string plaintext = AES::decrypt_string(ciphertext, key, iv, mode);
    
    return Value(plaintext);
}

Value builtin_aes_generate_key(const std::vector<Value>& args) {
    KeySize size = KeySize::AES_256;
    
    if (!args.empty()) {
        int bits = static_cast<int>(args[0].as_number());
        if (bits == 128) size = KeySize::AES_128;
        else if (bits == 192) size = KeySize::AES_192;
        else if (bits == 256) size = KeySize::AES_256;
    }
    
    ByteArray key = AES::generate_key(size);
    std::string key_hex = Hex::encode(key);
    
    return Value(key_hex);
}

Value builtin_aes_generate_iv(const std::vector<Value>& args) {
    ByteArray iv = AES::generate_iv();
    std::string iv_hex = Hex::encode(iv);
    
    return Value(iv_hex);
}

// ============================================================================
// RSA Functions - دوال RSA
// ============================================================================

Value builtin_rsa_generate_keypair(const std::vector<Value>& args) {
    KeySize size = KeySize::RSA_2048;
    
    if (!args.empty()) {
        int bits = static_cast<int>(args[0].as_number());
        if (bits == 1024) size = KeySize::RSA_1024;
        else if (bits == 2048) size = KeySize::RSA_2048;
        else if (bits == 4096) size = KeySize::RSA_4096;
    }
    
    RSAKeyPair keypair = RSA::generate_key_pair(size);
    
    // Create object with public and private keys
    auto obj = std::make_shared<std::map<std::string, Value>>();
    (*obj)["public"] = Value(keypair.public_key);
    (*obj)["private"] = Value(keypair.private_key);
    (*obj)["مفتاح_عام"] = Value(keypair.public_key);
    (*obj)["مفتاح_خاص"] = Value(keypair.private_key);
    
    return Value(obj);
}

Value builtin_rsa_encrypt(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("rsa_encrypt requires data and public key / rsa_encrypt يتطلب بيانات ومفتاح عام");
    }
    
    std::string plaintext = args[0].as_string();
    std::string public_key = args[1].as_string();
    
    std::string ciphertext = RSA::encrypt_string(plaintext, public_key);
    
    return Value(ciphertext);
}

Value builtin_rsa_decrypt(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("rsa_decrypt requires data and private key / rsa_decrypt يتطلب بيانات ومفتاح خاص");
    }
    
    std::string ciphertext = args[0].as_string();
    std::string private_key = args[1].as_string();
    
    std::string plaintext = RSA::decrypt_string(ciphertext, private_key);
    
    return Value(plaintext);
}

Value builtin_rsa_sign(const std::vector<Value>& args) {
    if (args.size() < 2) {
        throw std::runtime_error("rsa_sign requires data and private key / rsa_sign يتطلب بيانات ومفتاح خاص");
    }
    
    std::string data = args[0].as_string();
    std::string private_key = args[1].as_string();
    
    HashAlgorithm algo = HashAlgorithm::SHA256;
    if (args.size() >= 3) {
        algo = string_to_hash_algo(args[2].as_string());
    }
    
    std::string signature = RSA::sign_string(data, private_key, algo);
    
    return Value(signature);
}

Value builtin_rsa_verify(const std::vector<Value>& args) {
    if (args.size() < 3) {
        throw std::runtime_error("rsa_verify requires data, signature, and public key / rsa_verify يتطلب بيانات وتوقيع ومفتاح عام");
    }
    
    std::string data = args[0].as_string();
    std::string signature = args[1].as_string();
    std::string public_key = args[2].as_string();
    
    HashAlgorithm algo = HashAlgorithm::SHA256;
    if (args.size() >= 4) {
        algo = string_to_hash_algo(args[3].as_string());
    }
    
    bool valid = RSA::verify_string(data, signature, public_key, algo);
    
    return Value(valid);
}

// ============================================================================
// Password Functions - دوال كلمات المرور
// ============================================================================

Value builtin_password_hash(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("password_hash requires password / password_hash يتطلب كلمة مرور");
    }
    
    std::string password = args[0].as_string();
    
    // Generate salt
    std::string salt = Password::generate_salt(32);
    
    // Hash with PBKDF2
    std::string hash = Password::hash_pbkdf2(password, salt, 100000, HashAlgorithm::SHA256);
    
    // Return object with hash and salt
    auto obj = std::make_shared<std::map<std::string, Value>>();
    (*obj)["hash"] = Value(hash);
    (*obj)["salt"] = Value(salt);
    (*obj)["تجزئة"] = Value(hash);
    (*obj)["ملح"] = Value(salt);
    
    return Value(obj);
}

Value builtin_password_verify(const std::vector<Value>& args) {
    if (args.size() < 3) {
        throw std::runtime_error("password_verify requires password, hash, and salt / password_verify يتطلب كلمة مرور وتجزئة وملح");
    }
    
    std::string password = args[0].as_string();
    std::string hash = args[1].as_string();
    std::string salt = args[2].as_string();
    
    bool valid = Password::verify_pbkdf2(password, hash, salt, 100000, HashAlgorithm::SHA256);
    
    return Value(valid);
}

// ============================================================================
// Random Functions - دوال عشوائية
// ============================================================================

Value builtin_random_bytes(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("random_bytes requires length / random_bytes يتطلب طول");
    }
    
    size_t length = static_cast<size_t>(args[0].as_number());
    ByteArray bytes = Random::bytes(length);
    std::string hex = Hex::encode(bytes);
    
    return Value(hex);
}

Value builtin_random_int(const std::vector<Value>& args) {
    if (args.size() >= 2) {
        int32_t min = static_cast<int32_t>(args[0].as_number());
        int32_t max = static_cast<int32_t>(args[1].as_number());
        return Value(static_cast<double>(Random::int32(min, max)));
    }
    
    return Value(static_cast<double>(Random::int32()));
}

Value builtin_random_number(const std::vector<Value>& args) {
    if (args.size() >= 2) {
        double min = args[0].as_number();
        double max = args[1].as_number();
        return Value(Random::number(min, max));
    }
    
    return Value(Random::number());
}

Value builtin_random_hex(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("random_hex requires length / random_hex يتطلب طول");
    }
    
    size_t length = static_cast<size_t>(args[0].as_number());
    std::string hex = Random::hex(length);
    
    return Value(hex);
}

Value builtin_random_string(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("random_string requires length / random_string يتطلب طول");
    }
    
    size_t length = static_cast<size_t>(args[0].as_number());
    std::string str = Random::alphanumeric(length);
    
    return Value(str);
}

Value builtin_uuid(const std::vector<Value>& args) {
    std::string uuid = Random::uuid();
    return Value(uuid);
}

// ============================================================================
// Encoding Functions - دوال الترميز
// ============================================================================

Value builtin_base64_encode(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("base64_encode requires data / base64_encode يتطلب بيانات");
    }
    
    std::string data = args[0].as_string();
    std::string encoded = Base64::encode(data);
    
    return Value(encoded);
}

Value builtin_base64_decode(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("base64_decode requires data / base64_decode يتطلب بيانات");
    }
    
    std::string encoded = args[0].as_string();
    std::string decoded = Base64::decode_string(encoded);
    
    return Value(decoded);
}

Value builtin_hex_encode(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("hex_encode requires data / hex_encode يتطلب بيانات");
    }
    
    std::string data = args[0].as_string();
    std::string encoded = Hex::encode(data);
    
    return Value(encoded);
}

Value builtin_hex_decode(const std::vector<Value>& args) {
    if (args.empty()) {
        throw std::runtime_error("hex_decode requires data / hex_decode يتطلب بيانات");
    }
    
    std::string encoded = args[0].as_string();
    std::string decoded = Hex::decode_string(encoded);
    
    return Value(decoded);
}

} // anonymous namespace

// ============================================================================
// Registration - التسجيل
// ============================================================================

namespace sad {
namespace stdlib {
namespace crypto {

void register_crypto_builtins() {
    // Hash functions / دوال التجزئة
    BuiltinRegistry::register_builtin("hash", builtin_hash);
    BuiltinRegistry::register_builtin("تجزئة", builtin_hash);
    
    BuiltinRegistry::register_builtin("md5", builtin_md5);
    
    BuiltinRegistry::register_builtin("sha1", builtin_sha1);
    
    BuiltinRegistry::register_builtin("sha256", builtin_sha256);
    
    BuiltinRegistry::register_builtin("sha512", builtin_sha512);
    
    BuiltinRegistry::register_builtin("hash_verify", builtin_hash_verify);
    BuiltinRegistry::register_builtin("تحقق_من_تجزئة", builtin_hash_verify);
    
    // HMAC functions / دوال HMAC
    BuiltinRegistry::register_builtin("hmac", builtin_hmac);
    
    BuiltinRegistry::register_builtin("hmac_verify", builtin_hmac_verify);
    BuiltinRegistry::register_builtin("تحقق_من_hmac", builtin_hmac_verify);
    
    // AES functions / دوال AES
    BuiltinRegistry::register_builtin("aes_encrypt", builtin_aes_encrypt);
    BuiltinRegistry::register_builtin("شفر_aes", builtin_aes_encrypt);
    
    BuiltinRegistry::register_builtin("aes_decrypt", builtin_aes_decrypt);
    BuiltinRegistry::register_builtin("فك_تشفير_aes", builtin_aes_decrypt);
    
    BuiltinRegistry::register_builtin("aes_generate_key", builtin_aes_generate_key);
    BuiltinRegistry::register_builtin("ولد_مفتاح_aes", builtin_aes_generate_key);
    
    BuiltinRegistry::register_builtin("aes_generate_iv", builtin_aes_generate_iv);
    BuiltinRegistry::register_builtin("ولد_iv_aes", builtin_aes_generate_iv);
    
    // RSA functions / دوال RSA
    BuiltinRegistry::register_builtin("rsa_generate_keypair", builtin_rsa_generate_keypair);
    BuiltinRegistry::register_builtin("ولد_مفاتيح_rsa", builtin_rsa_generate_keypair);
    
    BuiltinRegistry::register_builtin("rsa_encrypt", builtin_rsa_encrypt);
    BuiltinRegistry::register_builtin("شفر_rsa", builtin_rsa_encrypt);
    
    BuiltinRegistry::register_builtin("rsa_decrypt", builtin_rsa_decrypt);
    BuiltinRegistry::register_builtin("فك_تشفير_rsa", builtin_rsa_decrypt);
    
    BuiltinRegistry::register_builtin("rsa_sign", builtin_rsa_sign);
    BuiltinRegistry::register_builtin("وقع_rsa", builtin_rsa_sign);
    
    BuiltinRegistry::register_builtin("rsa_verify", builtin_rsa_verify);
    BuiltinRegistry::register_builtin("تحقق_من_توقيع_rsa", builtin_rsa_verify);
    
    // Password functions / دوال كلمات المرور
    BuiltinRegistry::register_builtin("password_hash", builtin_password_hash);
    BuiltinRegistry::register_builtin("جزئ_كلمة_مرور", builtin_password_hash);
    
    BuiltinRegistry::register_builtin("password_verify", builtin_password_verify);
    BuiltinRegistry::register_builtin("تحقق_من_كلمة_مرور", builtin_password_verify);
    
    // Random functions / دوال عشوائية
    BuiltinRegistry::register_builtin("random_bytes", builtin_random_bytes);
    BuiltinRegistry::register_builtin("بايتات_عشوائية", builtin_random_bytes);
    
    BuiltinRegistry::register_builtin("random_int", builtin_random_int);
    BuiltinRegistry::register_builtin("عدد_صحيح_عشوائي", builtin_random_int);
    
    BuiltinRegistry::register_builtin("random_number", builtin_random_number);
    BuiltinRegistry::register_builtin("رقم_عشوائي", builtin_random_number);
    
    BuiltinRegistry::register_builtin("random_hex", builtin_random_hex);
    BuiltinRegistry::register_builtin("hex_عشوائي", builtin_random_hex);
    
    BuiltinRegistry::register_builtin("random_string", builtin_random_string);
    BuiltinRegistry::register_builtin("نص_عشوائي", builtin_random_string);
    
    BuiltinRegistry::register_builtin("uuid", builtin_uuid);
    
    // Encoding functions / دوال الترميز
    BuiltinRegistry::register_builtin("base64_encode", builtin_base64_encode);
    BuiltinRegistry::register_builtin("رمز_base64", builtin_base64_encode);
    
    BuiltinRegistry::register_builtin("base64_decode", builtin_base64_decode);
    BuiltinRegistry::register_builtin("فك_ترميز_base64", builtin_base64_decode);
    
    BuiltinRegistry::register_builtin("hex_encode", builtin_hex_encode);
    BuiltinRegistry::register_builtin("رمز_hex", builtin_hex_encode);
    
    BuiltinRegistry::register_builtin("hex_decode", builtin_hex_decode);
    BuiltinRegistry::register_builtin("فك_ترميز_hex", builtin_hex_decode);
}

} // namespace crypto
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
