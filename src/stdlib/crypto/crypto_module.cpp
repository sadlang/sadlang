// بسم الله الرحمن الرحيم
/**
 * @file crypto_module.cpp
 * @brief Cryptography Module Implementation - تنفيذ وحدة التشفير
 * 
 * الحمد لله رب العالمين
 */

#include "stdlib/crypto/crypto_module.h"
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/err.h>

namespace sad {
namespace stdlib {
namespace crypto {

// ============================================================================
// Hash Implementation - تنفيذ التجزئة
// ============================================================================

ByteArray Hash::compute(HashAlgorithm algo, const ByteArray& data) {
    const EVP_MD* md = nullptr;
    
    switch (algo) {
        case HashAlgorithm::MD5:
            md = EVP_md5();
            break;
        case HashAlgorithm::SHA1:
            md = EVP_sha1();
            break;
        case HashAlgorithm::SHA224:
            md = EVP_sha224();
            break;
        case HashAlgorithm::SHA256:
            md = EVP_sha256();
            break;
        case HashAlgorithm::SHA384:
            md = EVP_sha384();
            break;
        case HashAlgorithm::SHA512:
            md = EVP_sha512();
            break;
        default:
            throw CryptoException("Unsupported hash algorithm / خوارزمية تجزئة غير مدعومة");
    }
    
    unsigned int hash_len = EVP_MD_size(md);
    ByteArray hash(hash_len);
    
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw CryptoException("Failed to create hash context / فشل إنشاء سياق التجزئة");
    }
    
    EVP_DigestInit_ex(ctx, md, nullptr);
    EVP_DigestUpdate(ctx, data.data(), data.size());
    EVP_DigestFinal_ex(ctx, hash.data(), &hash_len);
    EVP_MD_CTX_free(ctx);
    
    return hash;
}

ByteArray Hash::compute(HashAlgorithm algo, const std::string& data) {
    return compute(algo, string_to_bytes(data));
}

std::string Hash::compute_hex(HashAlgorithm algo, const ByteArray& data) {
    ByteArray hash = compute(algo, data);
    return Hex::encode(hash);
}

std::string Hash::compute_hex(HashAlgorithm algo, const std::string& data) {
    return compute_hex(algo, string_to_bytes(data));
}

bool Hash::verify(HashAlgorithm algo, const ByteArray& data, const ByteArray& hash) {
    ByteArray computed = compute(algo, data);
    return constant_time_compare(computed, hash);
}

bool Hash::verify(HashAlgorithm algo, const std::string& data, const std::string& hash_hex) {
    std::string computed_hex = compute_hex(algo, data);
    return constant_time_compare(computed_hex, hash_hex);
}

size_t Hash::get_size(HashAlgorithm algo) {
    switch (algo) {
        case HashAlgorithm::MD5: return 16;
        case HashAlgorithm::SHA1: return 20;
        case HashAlgorithm::SHA224: return 28;
        case HashAlgorithm::SHA256: return 32;
        case HashAlgorithm::SHA384: return 48;
        case HashAlgorithm::SHA512: return 64;
        default: return 0;
    }
}

// ============================================================================
// HMAC Implementation - تنفيذ HMAC
// ============================================================================

ByteArray HMAC::compute(HashAlgorithm algo, const ByteArray& key, const ByteArray& data) {
    const EVP_MD* md = nullptr;
    
    switch (algo) {
        case HashAlgorithm::MD5: md = EVP_md5(); break;
        case HashAlgorithm::SHA1: md = EVP_sha1(); break;
        case HashAlgorithm::SHA224: md = EVP_sha224(); break;
        case HashAlgorithm::SHA256: md = EVP_sha256(); break;
        case HashAlgorithm::SHA384: md = EVP_sha384(); break;
        case HashAlgorithm::SHA512: md = EVP_sha512(); break;
        default:
            throw CryptoException("Unsupported HMAC algorithm / خوارزمية HMAC غير مدعومة");
    }
    
    unsigned int mac_len = EVP_MD_size(md);
    ByteArray mac(mac_len);
    
    ::HMAC(md, key.data(), key.size(), data.data(), data.size(), mac.data(), &mac_len);
    
    return mac;
}

std::string HMAC::compute_hex(HashAlgorithm algo, const std::string& key, const std::string& data) {
    ByteArray mac = compute(algo, string_to_bytes(key), string_to_bytes(data));
    return Hex::encode(mac);
}

bool HMAC::verify(HashAlgorithm algo, const ByteArray& key, const ByteArray& data, const ByteArray& mac) {
    ByteArray computed = compute(algo, key, data);
    return constant_time_compare(computed, mac);
}

bool HMAC::verify(HashAlgorithm algo, const std::string& key, const std::string& data, const std::string& mac_hex) {
    std::string computed_hex = compute_hex(algo, key, data);
    return constant_time_compare(computed_hex, mac_hex);
}

// ============================================================================
// AES Implementation - تنفيذ AES
// ============================================================================

ByteArray AES::encrypt(const ByteArray& data, const ByteArray& key, const ByteArray& iv,
                       CipherMode mode, PaddingMode padding) {
    const EVP_CIPHER* cipher = nullptr;
    
    // Determine cipher based on key size and mode
    if (mode == CipherMode::CBC) {
        if (key.size() == 16) cipher = EVP_aes_128_cbc();
        else if (key.size() == 24) cipher = EVP_aes_192_cbc();
        else if (key.size() == 32) cipher = EVP_aes_256_cbc();
    } else if (mode == CipherMode::ECB) {
        if (key.size() == 16) cipher = EVP_aes_128_ecb();
        else if (key.size() == 24) cipher = EVP_aes_192_ecb();
        else if (key.size() == 32) cipher = EVP_aes_256_ecb();
    } else if (mode == CipherMode::CTR) {
        if (key.size() == 16) cipher = EVP_aes_128_ctr();
        else if (key.size() == 24) cipher = EVP_aes_192_ctr();
        else if (key.size() == 32) cipher = EVP_aes_256_ctr();
    } else if (mode == CipherMode::GCM) {
        if (key.size() == 16) cipher = EVP_aes_128_gcm();
        else if (key.size() == 24) cipher = EVP_aes_192_gcm();
        else if (key.size() == 32) cipher = EVP_aes_256_gcm();
    }
    
    if (!cipher) {
        throw CryptoException("Invalid key size or cipher mode / حجم مفتاح أو وضع تشفير غير صالح");
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw CryptoException("Failed to create cipher context / فشل إنشاء سياق التشفير");
    }
    
    EVP_EncryptInit_ex(ctx, cipher, nullptr, key.data(), iv.data());
    
    if (padding == PaddingMode::NONE) {
        EVP_CIPHER_CTX_set_padding(ctx, 0);
    }
    
    ByteArray ciphertext(data.size() + EVP_CIPHER_block_size(cipher));
    int len = 0, ciphertext_len = 0;
    
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, data.data(), data.size());
    ciphertext_len = len;
    
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

ByteArray AES::decrypt(const ByteArray& data, const ByteArray& key, const ByteArray& iv,
                       CipherMode mode, PaddingMode padding) {
    const EVP_CIPHER* cipher = nullptr;
    
    if (mode == CipherMode::CBC) {
        if (key.size() == 16) cipher = EVP_aes_128_cbc();
        else if (key.size() == 24) cipher = EVP_aes_192_cbc();
        else if (key.size() == 32) cipher = EVP_aes_256_cbc();
    } else if (mode == CipherMode::ECB) {
        if (key.size() == 16) cipher = EVP_aes_128_ecb();
        else if (key.size() == 24) cipher = EVP_aes_192_ecb();
        else if (key.size() == 32) cipher = EVP_aes_256_ecb();
    } else if (mode == CipherMode::CTR) {
        if (key.size() == 16) cipher = EVP_aes_128_ctr();
        else if (key.size() == 24) cipher = EVP_aes_192_ctr();
        else if (key.size() == 32) cipher = EVP_aes_256_ctr();
    } else if (mode == CipherMode::GCM) {
        if (key.size() == 16) cipher = EVP_aes_128_gcm();
        else if (key.size() == 24) cipher = EVP_aes_192_gcm();
        else if (key.size() == 32) cipher = EVP_aes_256_gcm();
    }
    
    if (!cipher) {
        throw CryptoException("Invalid key size or cipher mode / حجم مفتاح أو وضع تشفير غير صالح");
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw CryptoException("Failed to create cipher context / فشل إنشاء سياق التشفير");
    }
    
    EVP_DecryptInit_ex(ctx, cipher, nullptr, key.data(), iv.data());
    
    if (padding == PaddingMode::NONE) {
        EVP_CIPHER_CTX_set_padding(ctx, 0);
    }
    
    ByteArray plaintext(data.size());
    int len = 0, plaintext_len = 0;
    
    EVP_DecryptUpdate(ctx, plaintext.data(), &len, data.data(), data.size());
    plaintext_len = len;
    
    EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    plaintext_len += len;
    
    EVP_CIPHER_CTX_free(ctx);
    
    plaintext.resize(plaintext_len);
    return plaintext;
}

std::string AES::encrypt_string(const std::string& plaintext, const std::string& key,
                                const std::string& iv, CipherMode mode) {
    ByteArray encrypted = encrypt(
        string_to_bytes(plaintext),
        string_to_bytes(key),
        string_to_bytes(iv),
        mode
    );
    return Base64::encode(encrypted);
}

std::string AES::decrypt_string(const std::string& ciphertext_base64, const std::string& key,
                                const std::string& iv, CipherMode mode) {
    ByteArray encrypted = Base64::decode(ciphertext_base64);
    ByteArray decrypted = decrypt(
        encrypted,
        string_to_bytes(key),
        string_to_bytes(iv),
        mode
    );
    return bytes_to_string(decrypted);
}

ByteArray AES::generate_key(KeySize size) {
    size_t key_len = static_cast<size_t>(size) / 8;
    return Random::bytes(key_len);
}

ByteArray AES::generate_iv() {
    return Random::bytes(16); // AES block size
}

// ============================================================================
// RSA Implementation - تنفيذ RSA
// ============================================================================

RSAKeyPair RSA::generate_key_pair(KeySize size) {
    int bits = static_cast<int>(size);
    
    EVP_PKEY* pkey = EVP_PKEY_new();
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits);
    EVP_PKEY_keygen(ctx, &pkey);
    EVP_PKEY_CTX_free(ctx);
    
    // Export public key
    BIO* pub_bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(pub_bio, pkey);
    
    char* pub_data = nullptr;
    long pub_len = BIO_get_mem_data(pub_bio, &pub_data);
    std::string public_key(pub_data, pub_len);
    BIO_free(pub_bio);
    
    // Export private key
    BIO* priv_bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(priv_bio, pkey, nullptr, nullptr, 0, nullptr, nullptr);
    
    char* priv_data = nullptr;
    long priv_len = BIO_get_mem_data(priv_bio, &priv_data);
    std::string private_key(priv_data, priv_len);
    BIO_free(priv_bio);
    
    EVP_PKEY_free(pkey);
    
    return RSAKeyPair{public_key, private_key};
}

ByteArray RSA::encrypt(const ByteArray& data, const std::string& public_key) {
    BIO* bio = BIO_new_mem_buf(public_key.data(), public_key.size());
    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    
    if (!pkey) {
        throw CryptoException("Failed to load public key / فشل تحميل المفتاح العام");
    }
    
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    EVP_PKEY_encrypt_init(ctx);
    EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    
    size_t outlen;
    EVP_PKEY_encrypt(ctx, nullptr, &outlen, data.data(), data.size());
    
    ByteArray encrypted(outlen);
    EVP_PKEY_encrypt(ctx, encrypted.data(), &outlen, data.data(), data.size());
    
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    
    return encrypted;
}

std::string RSA::encrypt_string(const std::string& plaintext, const std::string& public_key) {
    ByteArray encrypted = encrypt(string_to_bytes(plaintext), public_key);
    return Base64::encode(encrypted);
}

ByteArray RSA::decrypt(const ByteArray& data, const std::string& private_key) {
    BIO* bio = BIO_new_mem_buf(private_key.data(), private_key.size());
    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    
    if (!pkey) {
        throw CryptoException("Failed to load private key / فشل تحميل المفتاح الخاص");
    }
    
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    EVP_PKEY_decrypt_init(ctx);
    EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING);
    
    size_t outlen;
    EVP_PKEY_decrypt(ctx, nullptr, &outlen, data.data(), data.size());
    
    ByteArray decrypted(outlen);
    EVP_PKEY_decrypt(ctx, decrypted.data(), &outlen, data.data(), data.size());
    
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    
    decrypted.resize(outlen);
    return decrypted;
}

std::string RSA::decrypt_string(const std::string& ciphertext_base64, const std::string& private_key) {
    ByteArray encrypted = Base64::decode(ciphertext_base64);
    ByteArray decrypted = decrypt(encrypted, private_key);
    return bytes_to_string(decrypted);
}

// RSA signature using proper OpenSSL API
ByteArray RSA::sign(const ByteArray& data, const std::string& private_key, HashAlgorithm algo) {
    BIO* bio = BIO_new_mem_buf(private_key.data(), private_key.size());
    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    
    if (!pkey) {
        throw CryptoException("Failed to load private key / فشل تحميل المفتاح الخاص");
    }
    
    // Determine the EVP_MD based on algorithm
    const EVP_MD* md = nullptr;
    switch (algo) {
        case HashAlgorithm::SHA256: md = EVP_sha256(); break;
        case HashAlgorithm::SHA512: md = EVP_sha512(); break;
        default: md = EVP_sha256();
    }
    
    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    EVP_DigestSignInit(md_ctx, nullptr, md, nullptr, pkey);
    EVP_DigestSignUpdate(md_ctx, data.data(), data.size());
    
    size_t sig_len;
    EVP_DigestSignFinal(md_ctx, nullptr, &sig_len);
    
    ByteArray signature(sig_len);
    EVP_DigestSignFinal(md_ctx, signature.data(), &sig_len);
    
    EVP_MD_CTX_free(md_ctx);
    EVP_PKEY_free(pkey);
    
    signature.resize(sig_len);
    return signature;
}

std::string RSA::sign_string(const std::string& data, const std::string& private_key, HashAlgorithm algo) {
    ByteArray signature = sign(string_to_bytes(data), private_key, algo);
    return Base64::encode(signature);
}

bool RSA::verify(const ByteArray& data, const ByteArray& signature, const std::string& public_key, HashAlgorithm algo) {
    try {
        BIO* bio = BIO_new_mem_buf(public_key.data(), public_key.size());
        EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);
        
        if (!pkey) {
            throw CryptoException("Failed to load public key / فشل تحميل المفتاح العام");
        }
        
        // Determine the EVP_MD based on algorithm
        const EVP_MD* md = nullptr;
        switch (algo) {
            case HashAlgorithm::SHA256: md = EVP_sha256(); break;
            case HashAlgorithm::SHA512: md = EVP_sha512(); break;
            default: md = EVP_sha256();
        }
        
        EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
        EVP_DigestVerifyInit(md_ctx, nullptr, md, nullptr, pkey);
        EVP_DigestVerifyUpdate(md_ctx, data.data(), data.size());
        
        int result = EVP_DigestVerifyFinal(md_ctx, signature.data(), signature.size());
        
        EVP_MD_CTX_free(md_ctx);
        EVP_PKEY_free(pkey);
        
        return result == 1;
    } catch (...) {
        return false;
    }
}

bool RSA::verify_string(const std::string& data, const std::string& signature_base64,
                       const std::string& public_key, HashAlgorithm algo) {
    ByteArray signature = Base64::decode(signature_base64);
    return verify(string_to_bytes(data), signature, public_key, algo);
}

// ============================================================================
// Password Hashing - تجزئة كلمات المرور
// ============================================================================

std::string Password::hash_pbkdf2(const std::string& password, const std::string& salt,
                                 int iterations, HashAlgorithm algo) {
    const EVP_MD* md = nullptr;
    
    switch (algo) {
        case HashAlgorithm::SHA256: md = EVP_sha256(); break;
        case HashAlgorithm::SHA512: md = EVP_sha512(); break;
        default: md = EVP_sha256();
    }
    
    size_t key_len = Hash::get_size(algo);
    ByteArray hash(key_len);
    
    PKCS5_PBKDF2_HMAC(
        password.c_str(), password.length(),
        reinterpret_cast<const unsigned char*>(salt.c_str()), salt.length(),
        iterations,
        md,
        key_len,
        hash.data()
    );
    
    return Hex::encode(hash);
}

bool Password::verify_pbkdf2(const std::string& password, const std::string& hash,
                             const std::string& salt, int iterations, HashAlgorithm algo) {
    std::string computed = hash_pbkdf2(password, salt, iterations, algo);
    return constant_time_compare(computed, hash);
}

std::string Password::hash_bcrypt(const std::string& password, int cost) {
    // Simplified - production would use actual bcrypt library
    std::string salt = generate_salt(16);
    return hash_pbkdf2(password, salt, 1 << cost, HashAlgorithm::SHA256);
}

bool Password::verify_bcrypt(const std::string& password, const std::string& hash) {
    // Simplified - production would extract salt from hash
    return false; // Placeholder
}

std::string Password::generate_salt(size_t length) {
    return Hex::encode(Random::bytes(length));
}

// ============================================================================
// Random Generation - توليد الأرقام العشوائية
// ============================================================================

ByteArray Random::bytes(size_t length) {
    ByteArray data(length);
    RAND_bytes(data.data(), length);
    return data;
}

int32_t Random::int32() {
    ByteArray data = bytes(4);
    return *reinterpret_cast<int32_t*>(data.data());
}

int32_t Random::int32(int32_t min, int32_t max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int32_t> dis(min, max);
    return dis(gen);
}

int64_t Random::int64() {
    ByteArray data = bytes(8);
    return *reinterpret_cast<int64_t*>(data.data());
}

int64_t Random::int64(int64_t min, int64_t max) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dis(min, max);
    return dis(gen);
}

double Random::number() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    return dis(gen);
}

double Random::number(double min, double max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

std::string Random::hex(size_t length) {
    return Hex::encode(bytes(length));
}

std::string Random::alphanumeric(size_t length) {
    static const char charset[] = 
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string result;
    result.reserve(length);
    
    for (size_t i = 0; i < length; i++) {
        result += charset[int32(0, sizeof(charset) - 2)];
    }
    
    return result;
}

std::string Random::uuid() {
    ByteArray data = bytes(16);
    
    // Set version (4) and variant bits
    data[6] = (data[6] & 0x0F) | 0x40;
    data[8] = (data[8] & 0x3F) | 0x80;
    
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    
    for (size_t i = 0; i < 16; i++) {
        if (i == 4 || i == 6 || i == 8 || i == 10) {
            oss << '-';
        }
        oss << std::setw(2) << static_cast<int>(data[i]);
    }
    
    return oss.str();
}

// ============================================================================
// Base64 Implementation - تنفيذ Base64
// ============================================================================

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string Base64::encode(const ByteArray& data) {
    std::string encoded;
    int val = 0;
    int valb = -6;
    
    for (uint8_t c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    
    if (valb > -6) {
        encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    
    while (encoded.size() % 4) {
        encoded.push_back('=');
    }
    
    return encoded;
}

std::string Base64::encode(const std::string& data) {
    return encode(string_to_bytes(data));
}

ByteArray Base64::decode(const std::string& encoded) {
    ByteArray decoded;
    int val = 0;
    int valb = -8;
    
    for (unsigned char c : encoded) {
        if (c == '=') break;
        
        size_t pos = base64_chars.find(c);
        if (pos == std::string::npos) continue;
        
        val = (val << 6) + pos;
        valb += 6;
        
        if (valb >= 0) {
            decoded.push_back((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    
    return decoded;
}

std::string Base64::decode_string(const std::string& encoded) {
    return bytes_to_string(decode(encoded));
}

bool Base64::is_valid(const std::string& encoded) {
    for (char c : encoded) {
        if (c != '=' && base64_chars.find(c) == std::string::npos) {
            return false;
        }
    }
    return true;
}

// ============================================================================
// Hex Implementation - تنفيذ Hex
// ============================================================================

std::string Hex::encode(const ByteArray& data) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    
    for (uint8_t byte : data) {
        oss << std::setw(2) << static_cast<int>(byte);
    }
    
    return oss.str();
}

std::string Hex::encode(const std::string& data) {
    return encode(string_to_bytes(data));
}

ByteArray Hex::decode(const std::string& hex) {
    ByteArray data;
    
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byte_str = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 16));
        data.push_back(byte);
    }
    
    return data;
}

std::string Hex::decode_string(const std::string& hex) {
    return bytes_to_string(decode(hex));
}

bool Hex::is_valid(const std::string& hex) {
    if (hex.length() % 2 != 0) return false;
    
    for (char c : hex) {
        if (!std::isxdigit(c)) return false;
    }
    
    return true;
}

// ============================================================================
// Utility Functions - الدوال المساعدة
// ============================================================================

ByteArray string_to_bytes(const std::string& str) {
    return ByteArray(str.begin(), str.end());
}

std::string bytes_to_string(const ByteArray& bytes) {
    return std::string(bytes.begin(), bytes.end());
}

bool constant_time_compare(const ByteArray& a, const ByteArray& b) {
    if (a.size() != b.size()) return false;
    
    int result = 0;
    for (size_t i = 0; i < a.size(); i++) {
        result |= a[i] ^ b[i];
    }
    
    return result == 0;
}

bool constant_time_compare(const std::string& a, const std::string& b) {
    return constant_time_compare(string_to_bytes(a), string_to_bytes(b));
}

std::string hash_algorithm_to_string(HashAlgorithm algo) {
    switch (algo) {
        case HashAlgorithm::MD5: return "MD5";
        case HashAlgorithm::SHA1: return "SHA1";
        case HashAlgorithm::SHA224: return "SHA224";
        case HashAlgorithm::SHA256: return "SHA256";
        case HashAlgorithm::SHA384: return "SHA384";
        case HashAlgorithm::SHA512: return "SHA512";
        default: return "Unknown";
    }
}

std::string cipher_mode_to_string(CipherMode mode) {
    switch (mode) {
        case CipherMode::ECB: return "ECB";
        case CipherMode::CBC: return "CBC";
        case CipherMode::CTR: return "CTR";
        case CipherMode::GCM: return "GCM";
        default: return "Unknown";
    }
}

std::string key_size_to_string(KeySize size) {
    return std::to_string(static_cast<int>(size));
}

void secure_wipe(ByteArray& data) {
    volatile uint8_t* p = data.data();
    for (size_t i = 0; i < data.size(); i++) {
        p[i] = 0;
    }
    data.clear();
}

void secure_wipe(std::string& data) {
    volatile char* p = data.data();
    for (size_t i = 0; i < data.size(); i++) {
        p[i] = 0;
    }
    data.clear();
}

} // namespace crypto
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
