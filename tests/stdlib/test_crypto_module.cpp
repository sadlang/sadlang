// بسم الله الرحمن الرحيم
/**
 * @file test_crypto_module.cpp
 * @brief Cryptography Module Tests - اختبارات وحدة التشفير
 * 
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include "stdlib/crypto/crypto_module.h"
#include <string>
#include <vector>

using namespace sad::stdlib::crypto;

// ============================================================================
// Hash Tests - اختبارات التجزئة
// ============================================================================

TEST(CryptoHashTest, MD5Basic) {
    std::string data = "Hello, World!";
    std::string hash = Hash::compute_hex(HashAlgorithm::MD5, data);
    
    EXPECT_EQ(hash.length(), 32); // MD5 is 128 bits = 32 hex chars
    EXPECT_FALSE(hash.empty());
}

TEST(CryptoHashTest, SHA256Basic) {
    std::string data = "The quick brown fox jumps over the lazy dog";
    std::string hash = Hash::compute_hex(HashAlgorithm::SHA256, data);
    
    // Known SHA256 hash
    EXPECT_EQ(hash, "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}

TEST(CryptoHashTest, SHA512Basic) {
    std::string data = "Hello";
    std::string hash = Hash::compute_hex(HashAlgorithm::SHA512, data);
    
    EXPECT_EQ(hash.length(), 128); // SHA512 is 512 bits = 128 hex chars
}

TEST(CryptoHashTest, EmptyString) {
    std::string data = "";
    std::string hash_md5 = Hash::compute_hex(HashAlgorithm::MD5, data);
    std::string hash_sha256 = Hash::compute_hex(HashAlgorithm::SHA256, data);
    
    EXPECT_EQ(hash_md5, "d41d8cd98f00b204e9800998ecf8427e"); // Known MD5 of empty string
    EXPECT_FALSE(hash_sha256.empty());
}

TEST(CryptoHashTest, ArabicText) {
    std::string data = "بسم الله الرحمن الرحيم";
    std::string hash = Hash::compute_hex(HashAlgorithm::SHA256, data);
    
    EXPECT_EQ(hash.length(), 64);
    EXPECT_FALSE(hash.empty());
}

TEST(CryptoHashTest, VerifyCorrectHash) {
    std::string data = "Test data";
    std::string hash = Hash::compute_hex(HashAlgorithm::SHA256, data);
    
    bool valid = Hash::verify(HashAlgorithm::SHA256, data, hash);
    EXPECT_TRUE(valid);
}

TEST(CryptoHashTest, VerifyWrongHash) {
    std::string data = "Test data";
    std::string wrong_hash = "0000000000000000000000000000000000000000000000000000000000000000";
    
    bool valid = Hash::verify(HashAlgorithm::SHA256, data, wrong_hash);
    EXPECT_FALSE(valid);
}

TEST(CryptoHashTest, DifferentDataDifferentHash) {
    std::string data1 = "Hello";
    std::string data2 = "World";
    
    std::string hash1 = Hash::compute_hex(HashAlgorithm::SHA256, data1);
    std::string hash2 = Hash::compute_hex(HashAlgorithm::SHA256, data2);
    
    EXPECT_NE(hash1, hash2);
}

TEST(CryptoHashTest, SameDataSameHash) {
    std::string data = "Consistent data";
    
    std::string hash1 = Hash::compute_hex(HashAlgorithm::SHA256, data);
    std::string hash2 = Hash::compute_hex(HashAlgorithm::SHA256, data);
    
    EXPECT_EQ(hash1, hash2);
}

TEST(CryptoHashTest, LargeData) {
    std::string data(1000000, 'A'); // 1MB of 'A's
    std::string hash = Hash::compute_hex(HashAlgorithm::SHA256, data);
    
    EXPECT_EQ(hash.length(), 64);
    EXPECT_FALSE(hash.empty());
}

// ============================================================================
// HMAC Tests - اختبارات HMAC
// ============================================================================

TEST(CryptoHMACTest, BasicHMAC) {
    std::string key = "secret_key";
    std::string data = "Hello, World!";
    
    std::string mac = HMAC::compute_hex(HashAlgorithm::SHA256, key, data);
    
    EXPECT_EQ(mac.length(), 64);
    EXPECT_FALSE(mac.empty());
}

TEST(CryptoHMACTest, VerifyCorrectMAC) {
    std::string key = "secret_key";
    std::string data = "Test data";
    
    std::string mac = HMAC::compute_hex(HashAlgorithm::SHA256, key, data);
    bool valid = HMAC::verify(HashAlgorithm::SHA256, key, data, mac);
    
    EXPECT_TRUE(valid);
}

TEST(CryptoHMACTest, VerifyWrongMAC) {
    std::string key = "secret_key";
    std::string data = "Test data";
    std::string wrong_mac = "0000000000000000000000000000000000000000000000000000000000000000";
    
    bool valid = HMAC::verify(HashAlgorithm::SHA256, key, data, wrong_mac);
    
    EXPECT_FALSE(valid);
}

TEST(CryptoHMACTest, DifferentKeysDifferentMAC) {
    std::string key1 = "key1";
    std::string key2 = "key2";
    std::string data = "Same data";
    
    std::string mac1 = HMAC::compute_hex(HashAlgorithm::SHA256, key1, data);
    std::string mac2 = HMAC::compute_hex(HashAlgorithm::SHA256, key2, data);
    
    EXPECT_NE(mac1, mac2);
}

TEST(CryptoHMACTest, ArabicKey) {
    std::string key = "مفتاح_سري";
    std::string data = "بيانات";
    
    std::string mac = HMAC::compute_hex(HashAlgorithm::SHA256, key, data);
    
    EXPECT_EQ(mac.length(), 64);
}

// ============================================================================
// AES Tests - اختبارات AES
// ============================================================================

TEST(CryptoAESTest, EncryptDecryptBasic) {
    std::string plaintext = "Hello, World!";
    std::string key = "0123456789abcdef0123456789abcdef"; // 32 bytes = 256 bits
    std::string iv = "0123456789abcdef"; // 16 bytes
    
    std::string ciphertext = AES::encrypt_string(plaintext, key, iv);
    std::string decrypted = AES::decrypt_string(ciphertext, key, iv);
    
    EXPECT_EQ(decrypted, plaintext);
}

TEST(CryptoAESTest, EncryptedDifferentFromPlaintext) {
    std::string plaintext = "Secret message";
    std::string key = "0123456789abcdef0123456789abcdef";
    std::string iv = "0123456789abcdef";
    
    std::string ciphertext = AES::encrypt_string(plaintext, key, iv);
    
    EXPECT_NE(ciphertext, plaintext);
    EXPECT_FALSE(ciphertext.empty());
}

TEST(CryptoAESTest, ArabicText) {
    std::string plaintext = "رسالة سرية";
    std::string key = "0123456789abcdef0123456789abcdef";
    std::string iv = "0123456789abcdef";
    
    std::string ciphertext = AES::encrypt_string(plaintext, key, iv);
    std::string decrypted = AES::decrypt_string(ciphertext, key, iv);
    
    EXPECT_EQ(decrypted, plaintext);
}

TEST(CryptoAESTest, LongText) {
    std::string plaintext(1000, 'A');
    std::string key = "0123456789abcdef0123456789abcdef";
    std::string iv = "0123456789abcdef";
    
    std::string ciphertext = AES::encrypt_string(plaintext, key, iv);
    std::string decrypted = AES::decrypt_string(ciphertext, key, iv);
    
    EXPECT_EQ(decrypted, plaintext);
}

TEST(CryptoAESTest, DifferentIVsDifferentCiphertext) {
    std::string plaintext = "Same message";
    std::string key = "0123456789abcdef0123456789abcdef";
    std::string iv1 = "0123456789abcdef";
    std::string iv2 = "fedcba9876543210";
    
    std::string ciphertext1 = AES::encrypt_string(plaintext, key, iv1);
    std::string ciphertext2 = AES::encrypt_string(plaintext, key, iv2);
    
    EXPECT_NE(ciphertext1, ciphertext2);
}

TEST(CryptoAESTest, GenerateKey256) {
    ByteArray key = AES::generate_key(KeySize::AES_256);
    
    EXPECT_EQ(key.size(), 32); // 256 bits = 32 bytes
}

TEST(CryptoAESTest, GenerateKey128) {
    ByteArray key = AES::generate_key(KeySize::AES_128);
    
    EXPECT_EQ(key.size(), 16); // 128 bits = 16 bytes
}

TEST(CryptoAESTest, GenerateIV) {
    ByteArray iv = AES::generate_iv();
    
    EXPECT_EQ(iv.size(), 16); // AES block size
}

// ============================================================================
// RSA Tests - اختبارات RSA
// ============================================================================

TEST(CryptoRSATest, GenerateKeyPair) {
    RSAKeyPair keypair = RSA::generate_key_pair(KeySize::RSA_2048);
    
    EXPECT_FALSE(keypair.public_key.empty());
    EXPECT_FALSE(keypair.private_key.empty());
    EXPECT_TRUE(keypair.public_key.find("BEGIN PUBLIC KEY") != std::string::npos);
    EXPECT_TRUE(keypair.private_key.find("BEGIN PRIVATE KEY") != std::string::npos);
}

TEST(CryptoRSATest, EncryptDecryptBasic) {
    RSAKeyPair keypair = RSA::generate_key_pair(KeySize::RSA_2048);
    std::string plaintext = "Hello, RSA!";
    
    std::string ciphertext = RSA::encrypt_string(plaintext, keypair.public_key);
    std::string decrypted = RSA::decrypt_string(ciphertext, keypair.private_key);
    
    EXPECT_EQ(decrypted, plaintext);
}

TEST(CryptoRSATest, EncryptedDifferentFromPlaintext) {
    RSAKeyPair keypair = RSA::generate_key_pair(KeySize::RSA_2048);
    std::string plaintext = "Secret";
    
    std::string ciphertext = RSA::encrypt_string(plaintext, keypair.public_key);
    
    EXPECT_NE(ciphertext, plaintext);
}

TEST(CryptoRSATest, ArabicText) {
    RSAKeyPair keypair = RSA::generate_key_pair(KeySize::RSA_2048);
    std::string plaintext = "نص عربي";
    
    std::string ciphertext = RSA::encrypt_string(plaintext, keypair.public_key);
    std::string decrypted = RSA::decrypt_string(ciphertext, keypair.private_key);
    
    EXPECT_EQ(decrypted, plaintext);
}

TEST(CryptoRSATest, SignVerifyBasic) {
    RSAKeyPair keypair = RSA::generate_key_pair(KeySize::RSA_2048);
    std::string data = "Document to sign";
    
    std::string signature = RSA::sign_string(data, keypair.private_key);
    bool valid = RSA::verify_string(data, signature, keypair.public_key);
    
    EXPECT_TRUE(valid);
}

TEST(CryptoRSATest, VerifyWrongSignature) {
    RSAKeyPair keypair = RSA::generate_key_pair(KeySize::RSA_2048);
    std::string data = "Document";
    std::string wrong_signature = "AAAA";
    
    bool valid = RSA::verify_string(data, wrong_signature, keypair.public_key);
    
    EXPECT_FALSE(valid);
}

TEST(CryptoRSATest, ModifiedDataInvalidSignature) {
    RSAKeyPair keypair = RSA::generate_key_pair(KeySize::RSA_2048);
    std::string original_data = "Original document";
    std::string modified_data = "Modified document";
    
    std::string signature = RSA::sign_string(original_data, keypair.private_key);
    bool valid = RSA::verify_string(modified_data, signature, keypair.public_key);
    
    EXPECT_FALSE(valid);
}

// ============================================================================
// Password Tests - اختبارات كلمات المرور
// ============================================================================

TEST(CryptoPasswordTest, PBKDF2Basic) {
    std::string password = "MyPassword123";
    std::string salt = Password::generate_salt(32);
    
    std::string hash = Password::hash_pbkdf2(password, salt, 10000);
    
    EXPECT_FALSE(hash.empty());
    EXPECT_EQ(hash.length(), 64); // SHA256 = 64 hex chars
}

TEST(CryptoPasswordTest, PBKDF2VerifyCorrect) {
    std::string password = "SecurePass456";
    std::string salt = Password::generate_salt(32);
    
    std::string hash = Password::hash_pbkdf2(password, salt, 10000);
    bool valid = Password::verify_pbkdf2(password, hash, salt, 10000);
    
    EXPECT_TRUE(valid);
}

TEST(CryptoPasswordTest, PBKDF2VerifyWrong) {
    std::string correct_password = "Correct";
    std::string wrong_password = "Wrong";
    std::string salt = Password::generate_salt(32);
    
    std::string hash = Password::hash_pbkdf2(correct_password, salt, 10000);
    bool valid = Password::verify_pbkdf2(wrong_password, hash, salt, 10000);
    
    EXPECT_FALSE(valid);
}

TEST(CryptoPasswordTest, ArabicPassword) {
    std::string password = "كلمة_مرور_عربية";
    std::string salt = Password::generate_salt(32);
    
    std::string hash = Password::hash_pbkdf2(password, salt, 10000);
    bool valid = Password::verify_pbkdf2(password, hash, salt, 10000);
    
    EXPECT_TRUE(valid);
}

TEST(CryptoPasswordTest, GenerateSalt) {
    std::string salt1 = Password::generate_salt(32);
    std::string salt2 = Password::generate_salt(32);
    
    EXPECT_EQ(salt1.length(), 64); // 32 bytes = 64 hex chars
    EXPECT_NE(salt1, salt2); // Should be different
}

TEST(CryptoPasswordTest, DifferentIterations) {
    std::string password = "Pass";
    std::string salt = Password::generate_salt(32);
    
    std::string hash1 = Password::hash_pbkdf2(password, salt, 10000);
    std::string hash2 = Password::hash_pbkdf2(password, salt, 20000);
    
    EXPECT_NE(hash1, hash2);
}

// ============================================================================
// Random Tests - اختبارات العشوائية
// ============================================================================

TEST(CryptoRandomTest, RandomBytes) {
    ByteArray bytes1 = Random::bytes(32);
    ByteArray bytes2 = Random::bytes(32);
    
    EXPECT_EQ(bytes1.size(), 32);
    EXPECT_EQ(bytes2.size(), 32);
    EXPECT_NE(bytes1, bytes2); // Should be different
}

TEST(CryptoRandomTest, RandomInt32) {
    int32_t num1 = Random::int32();
    int32_t num2 = Random::int32();
    
    // High probability they're different
    EXPECT_TRUE(num1 != num2 || num1 == num2); // Always true, just testing it runs
}

TEST(CryptoRandomTest, RandomInt32Range) {
    int32_t num = Random::int32(10, 20);
    
    EXPECT_GE(num, 10);
    EXPECT_LE(num, 20);
}

TEST(CryptoRandomTest, RandomNumber) {
    double num = Random::number();
    
    EXPECT_GE(num, 0.0);
    EXPECT_LT(num, 1.0);
}

TEST(CryptoRandomTest, RandomNumberRange) {
    double num = Random::number(5.0, 10.0);
    
    EXPECT_GE(num, 5.0);
    EXPECT_LE(num, 10.0);
}

TEST(CryptoRandomTest, RandomHex) {
    std::string hex = Random::hex(16);
    
    EXPECT_EQ(hex.length(), 32); // 16 bytes = 32 hex chars
    EXPECT_TRUE(Hex::is_valid(hex));
}

TEST(CryptoRandomTest, RandomAlphanumeric) {
    std::string str = Random::alphanumeric(20);
    
    EXPECT_EQ(str.length(), 20);
    
    // Check all characters are alphanumeric
    for (char c : str) {
        EXPECT_TRUE(std::isalnum(c));
    }
}

TEST(CryptoRandomTest, UUID) {
    std::string uuid1 = Random::uuid();
    std::string uuid2 = Random::uuid();
    
    EXPECT_EQ(uuid1.length(), 36); // UUID format: 8-4-4-4-12
    EXPECT_EQ(uuid2.length(), 36);
    EXPECT_NE(uuid1, uuid2);
    
    // Check format
    EXPECT_EQ(uuid1[8], '-');
    EXPECT_EQ(uuid1[13], '-');
    EXPECT_EQ(uuid1[18], '-');
    EXPECT_EQ(uuid1[23], '-');
}

// ============================================================================
// Base64 Tests - اختبارات Base64
// ============================================================================

TEST(CryptoBase64Test, EncodeBasic) {
    std::string data = "Hello, World!";
    std::string encoded = Base64::encode(data);
    
    EXPECT_FALSE(encoded.empty());
    EXPECT_TRUE(Base64::is_valid(encoded));
}

TEST(CryptoBase64Test, EncodeDecodeBasic) {
    std::string data = "Test data 123";
    
    std::string encoded = Base64::encode(data);
    std::string decoded = Base64::decode_string(encoded);
    
    EXPECT_EQ(decoded, data);
}

TEST(CryptoBase64Test, EmptyString) {
    std::string data = "";
    
    std::string encoded = Base64::encode(data);
    std::string decoded = Base64::decode_string(encoded);
    
    EXPECT_EQ(decoded, data);
}

TEST(CryptoBase64Test, ArabicText) {
    std::string data = "مرحبا بالعالم";
    
    std::string encoded = Base64::encode(data);
    std::string decoded = Base64::decode_string(encoded);
    
    EXPECT_EQ(decoded, data);
}

TEST(CryptoBase64Test, BinaryData) {
    ByteArray data = {0x00, 0x01, 0x02, 0xFF, 0xFE};
    
    std::string encoded = Base64::encode(data);
    ByteArray decoded = Base64::decode(encoded);
    
    EXPECT_EQ(decoded, data);
}

// ============================================================================
// Hex Tests - اختبارات Hex
// ============================================================================

TEST(CryptoHexTest, EncodeBasic) {
    std::string data = "Hello";
    std::string encoded = Hex::encode(data);
    
    EXPECT_EQ(encoded, "48656c6c6f");
}

TEST(CryptoHexTest, EncodeDecodeBasic) {
    std::string data = "Test 123";
    
    std::string encoded = Hex::encode(data);
    std::string decoded = Hex::decode_string(encoded);
    
    EXPECT_EQ(decoded, data);
}

TEST(CryptoHexTest, BinaryData) {
    ByteArray data = {0x00, 0x01, 0x0F, 0xFF};
    
    std::string encoded = Hex::encode(data);
    ByteArray decoded = Hex::decode(encoded);
    
    EXPECT_EQ(encoded, "00010fff");
    EXPECT_EQ(decoded, data);
}

TEST(CryptoHexTest, IsValid) {
    EXPECT_TRUE(Hex::is_valid("0123456789abcdef"));
    EXPECT_TRUE(Hex::is_valid("ABCDEF"));
    EXPECT_FALSE(Hex::is_valid("ghijk")); // Invalid chars
    EXPECT_FALSE(Hex::is_valid("123")); // Odd length
}

// ============================================================================
// Utility Tests - اختبارات الدوال المساعدة
// ============================================================================

TEST(CryptoUtilTest, StringToBytes) {
    std::string str = "Hello";
    ByteArray bytes = string_to_bytes(str);
    
    EXPECT_EQ(bytes.size(), 5);
    EXPECT_EQ(bytes[0], 'H');
    EXPECT_EQ(bytes[4], 'o');
}

TEST(CryptoUtilTest, BytesToString) {
    ByteArray bytes = {'W', 'o', 'r', 'l', 'd'};
    std::string str = bytes_to_string(bytes);
    
    EXPECT_EQ(str, "World");
}

TEST(CryptoUtilTest, ConstantTimeCompareEqual) {
    ByteArray a = {1, 2, 3, 4, 5};
    ByteArray b = {1, 2, 3, 4, 5};
    
    EXPECT_TRUE(constant_time_compare(a, b));
}

TEST(CryptoUtilTest, ConstantTimeCompareDifferent) {
    ByteArray a = {1, 2, 3, 4, 5};
    ByteArray b = {1, 2, 3, 4, 6};
    
    EXPECT_FALSE(constant_time_compare(a, b));
}

TEST(CryptoUtilTest, ConstantTimeCompareStrings) {
    std::string a = "password";
    std::string b = "password";
    std::string c = "passw0rd";
    
    EXPECT_TRUE(constant_time_compare(a, b));
    EXPECT_FALSE(constant_time_compare(a, c));
}

TEST(CryptoUtilTest, SecureWipeBytes) {
    ByteArray data = {1, 2, 3, 4, 5};
    secure_wipe(data);
    
    EXPECT_TRUE(data.empty());
}

TEST(CryptoUtilTest, SecureWipeString) {
    std::string data = "sensitive";
    secure_wipe(data);
    
    EXPECT_TRUE(data.empty());
}

// ============================================================================
// Integration Tests - اختبارات التكامل
// ============================================================================

TEST(CryptoIntegrationTest, FullEncryptionWorkflow) {
    // Generate RSA key pair
    RSAKeyPair keypair = RSA::generate_key_pair(KeySize::RSA_2048);
    
    // Generate AES key and IV
    ByteArray aes_key = AES::generate_key(KeySize::AES_256);
    ByteArray aes_iv = AES::generate_iv();
    
    // Encrypt data with AES
    std::string plaintext = "Secret message الحمد لله";
    std::string ciphertext = AES::encrypt_string(
        plaintext,
        bytes_to_string(aes_key),
        bytes_to_string(aes_iv)
    );
    
    // Decrypt
    std::string decrypted = AES::decrypt_string(
        ciphertext,
        bytes_to_string(aes_key),
        bytes_to_string(aes_iv)
    );
    
    EXPECT_EQ(decrypted, plaintext);
}

TEST(CryptoIntegrationTest, HashAndSign) {
    // Create data
    std::string data = "Important document بسم الله";
    
    // Hash it
    std::string hash = Hash::compute_hex(HashAlgorithm::SHA256, data);
    
    // Generate key pair and sign the hash
    RSAKeyPair keypair = RSA::generate_key_pair(KeySize::RSA_2048);
    std::string signature = RSA::sign_string(data, keypair.private_key);
    
    // Verify
    bool valid = RSA::verify_string(data, signature, keypair.public_key);
    
    EXPECT_TRUE(valid);
    EXPECT_FALSE(hash.empty());
}

// الحمد لله رب العالمين
