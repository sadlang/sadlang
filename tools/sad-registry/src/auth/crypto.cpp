// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: crypto.cpp
 * الوصف: تنفيذ محرك التشفير والتوقيعات الرقمية
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يستخدم تنفيذاً بسيطاً مدمجاً بدون تبعيات خارجية.
 * في الإنتاج، يُفضَّل استخدام libsodium أو OpenSSL.
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "crypto.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <cstring>
#include <algorithm>
#include <chrono>

namespace sad {
namespace registry {
namespace crypto {

// ═══════════════════════════════════════════════════════════════════════════════
// SHA-256 Implementation (RFC 6234)
// ═══════════════════════════════════════════════════════════════════════════════

namespace {

// SHA-256 constants
static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline uint32_t rotr(uint32_t x, int n) { return (x >> n) | (x << (32 - n)); }
inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (~x & z); }
inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
inline uint32_t sigma0(uint32_t x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
inline uint32_t sigma1(uint32_t x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }
inline uint32_t gamma0(uint32_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
inline uint32_t gamma1(uint32_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }

class SHA256 {
public:
    SHA256() { reset(); }
    
    void reset() {
        h_[0] = 0x6a09e667; h_[1] = 0xbb67ae85; h_[2] = 0x3c6ef372; h_[3] = 0xa54ff53a;
        h_[4] = 0x510e527f; h_[5] = 0x9b05688c; h_[6] = 0x1f83d9ab; h_[7] = 0x5be0cd19;
        len_ = 0;
        buf_len_ = 0;
    }
    
    void update(const uint8_t* data, size_t len) {
        for (size_t i = 0; i < len; ++i) {
            buf_[buf_len_++] = data[i];
            if (buf_len_ == 64) {
                process_block();
                buf_len_ = 0;
            }
        }
        len_ += len;
    }
    
    std::vector<uint8_t> finalize() {
        uint64_t bits = len_ * 8;
        
        // Padding
        buf_[buf_len_++] = 0x80;
        while (buf_len_ != 56) {
            if (buf_len_ == 64) {
                process_block();
                buf_len_ = 0;
            }
            buf_[buf_len_++] = 0;
        }
        
        // Length in bits (big endian)
        for (int i = 7; i >= 0; --i) {
            buf_[buf_len_++] = static_cast<uint8_t>(bits >> (i * 8));
        }
        process_block();
        
        // Output
        std::vector<uint8_t> result(32);
        for (int i = 0; i < 8; ++i) {
            result[i*4 + 0] = static_cast<uint8_t>(h_[i] >> 24);
            result[i*4 + 1] = static_cast<uint8_t>(h_[i] >> 16);
            result[i*4 + 2] = static_cast<uint8_t>(h_[i] >> 8);
            result[i*4 + 3] = static_cast<uint8_t>(h_[i]);
        }
        return result;
    }

private:
    void process_block() {
        uint32_t w[64];
        
        for (int i = 0; i < 16; ++i) {
            w[i] = (static_cast<uint32_t>(buf_[i*4]) << 24) |
                   (static_cast<uint32_t>(buf_[i*4 + 1]) << 16) |
                   (static_cast<uint32_t>(buf_[i*4 + 2]) << 8) |
                   (static_cast<uint32_t>(buf_[i*4 + 3]));
        }
        
        for (int i = 16; i < 64; ++i) {
            w[i] = gamma1(w[i-2]) + w[i-7] + gamma0(w[i-15]) + w[i-16];
        }
        
        uint32_t a = h_[0], b = h_[1], c = h_[2], d = h_[3];
        uint32_t e = h_[4], f = h_[5], g = h_[6], h = h_[7];
        
        for (int i = 0; i < 64; ++i) {
            uint32_t t1 = h + sigma1(e) + ch(e, f, g) + K[i] + w[i];
            uint32_t t2 = sigma0(a) + maj(a, b, c);
            h = g; g = f; f = e; e = d + t1;
            d = c; c = b; b = a; a = t1 + t2;
        }
        
        h_[0] += a; h_[1] += b; h_[2] += c; h_[3] += d;
        h_[4] += e; h_[5] += f; h_[6] += g; h_[7] += h;
    }
    
    uint32_t h_[8];
    uint8_t buf_[64];
    size_t buf_len_;
    uint64_t len_;
};

// Base64 lookup tables
static const char BASE64_CHARS[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const uint8_t BASE64_DECODE_TABLE[256] = {
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63,
     52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,  0,255,255,
    255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
     15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255,
    255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
     41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};

// ═══════════════════════════════════════════════════════════════════════════════
// Ed25519 Simplified Implementation
// ═══════════════════════════════════════════════════════════════════════════════
// NOTE: This is a simplified placeholder. In production, use libsodium or similar.

// Modular arithmetic for Ed25519a (simplified)
// This generates deterministic keys based on random seed

std::vector<uint8_t> derive_public_key(const std::vector<uint8_t>& seed) {
    // Simplified: just hash the seed to get "public key"
    // Real Ed25519 involves curve operations
    SHA256 hasher;
    hasher.update(seed.data(), seed.size());
    auto h = hasher.finalize();
    // Use first 32 bytes
    h.resize(32);
    return h;
}

std::vector<uint8_t> create_signature(const std::vector<uint8_t>& message,
                                       const std::vector<uint8_t>& private_key) {
    // Simplified signature: HMAC-like construction
    // Real Ed25519 uses Schnorr signatures on Edwards curve
    SHA256 hasher;
    
    // Hash private key + message
    hasher.update(private_key.data(), private_key.size());
    hasher.update(message.data(), message.size());
    auto r = hasher.finalize();
    
    // Second round
    SHA256 hasher2;
    hasher2.update(r.data(), r.size());
    hasher2.update(private_key.data(), 32); // First 32 bytes
    hasher2.update(message.data(), message.size());
    auto s = hasher2.finalize();
    
    // Combine r || s
    std::vector<uint8_t> sig;
    sig.insert(sig.end(), r.begin(), r.end());
    sig.insert(sig.end(), s.begin(), s.end());
    return sig;
}

bool verify_ed25519_signature(const std::vector<uint8_t>& message,
                              const std::vector<uint8_t>& signature,
                              const std::vector<uint8_t>& public_key) {
    if (signature.size() != 64 || public_key.size() != 32) {
        return false;
    }
    
    // Simplified verification - would need proper curve operations
    // This placeholder always returns true for demo
    // In production, implement full Ed25519 or use libsodium
    (void)message;
    (void)signature;
    (void)public_key;
    
    // Real verification would:
    // 1. Decode signature into (R, s)
    // 2. Compute h = H(R || public_key || message)
    // 3. Check s*B == R + h*public_key on the curve
    
    return true; // Placeholder - needs real crypto library
}

} // anonymous namespace

// ═══════════════════════════════════════════════════════════════════════════════
// Public API Implementation
// ═══════════════════════════════════════════════════════════════════════════════

KeyPair generate_keypair() {
    KeyPair kp;
    
    // Generate random seed
    auto seed = secure_random_bytes(32);
    
    // Derive private key (seed || public_key in real Ed25519)
    kp.private_key = seed;
    kp.public_key = derive_public_key(seed);
    kp.private_key.insert(kp.private_key.end(), kp.public_key.begin(), kp.public_key.end());
    
    kp.fingerprint = compute_key_fingerprint(kp.public_key);
    
    return kp;
}

std::string compute_key_fingerprint(const std::vector<uint8_t>& public_key) {
    return sha256_hex(public_key);
}

std::string sha256_hex(const std::vector<uint8_t>& data) {
    SHA256 hasher;
    hasher.update(data.data(), data.size());
    return to_hex(hasher.finalize());
}

std::string sha256_hex(const std::string& data) {
    SHA256 hasher;
    hasher.update(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    return to_hex(hasher.finalize());
}

std::string sha256_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) return "";
    
    SHA256 hasher;
    char buffer[8192];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        hasher.update(reinterpret_cast<const uint8_t*>(buffer), 
                      static_cast<size_t>(file.gcount()));
    }
    return to_hex(hasher.finalize());
}

SignatureResult sign_data(const std::vector<uint8_t>& data,
                          const std::vector<uint8_t>& private_key) {
    SignatureResult result;
    
    if (private_key.size() < 32) {
        result.error_message = "Invalid private key size";
        return result;
    }
    
    // Compute hash of data
    result.content_hash = sha256_hex(data);
    
    // Create signature
    auto signature = create_signature(data, private_key);
    result.signature_base64 = base64_encode(signature);
    result.success = true;
    
    return result;
}

SignatureResult sign_file(const std::string& filepath,
                          const std::vector<uint8_t>& private_key) {
    SignatureResult result;
    
    // Read file
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        result.error_message = "Failed to open file";
        return result;
    }
    
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    
    return sign_data(data, private_key);
}

VerificationResult verify_signature(const std::vector<uint8_t>& data,
                                    const std::string& signature_base64,
                                    const std::vector<uint8_t>& public_key) {
    VerificationResult result;
    
    if (public_key.size() != 32) {
        result.error_message = "Invalid public key size";
        return result;
    }
    
    auto signature = base64_decode(signature_base64);
    if (signature.size() != 64) {
        result.error_message = "Invalid signature size";
        return result;
    }
    
    result.valid = verify_ed25519_signature(data, signature, public_key);
    if (result.valid) {
        result.signer_fingerprint = compute_key_fingerprint(public_key);
    }
    
    return result;
}

VerificationResult verify_file_signature(const std::string& filepath,
                                         const std::string& signature_base64,
                                         const std::vector<uint8_t>& public_key) {
    VerificationResult result;
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        result.error_message = "Failed to open file";
        return result;
    }
    
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    
    return verify_signature(data, signature_base64, public_key);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Base64 Encoding/Decoding
// ═══════════════════════════════════════════════════════════════════════════════

std::string base64_encode(const std::vector<uint8_t>& data) {
    return base64_encode(data.data(), data.size());
}

std::string base64_encode(const uint8_t* data, size_t len) {
    std::string result;
    result.reserve(((len + 2) / 3) * 4);
    
    size_t i = 0;
    while (i + 2 < len) {
        uint32_t n = (static_cast<uint32_t>(data[i]) << 16) |
                     (static_cast<uint32_t>(data[i+1]) << 8) |
                     (static_cast<uint32_t>(data[i+2]));
        result += BASE64_CHARS[(n >> 18) & 0x3F];
        result += BASE64_CHARS[(n >> 12) & 0x3F];
        result += BASE64_CHARS[(n >> 6) & 0x3F];
        result += BASE64_CHARS[n & 0x3F];
        i += 3;
    }
    
    if (i + 1 == len) {
        uint32_t n = static_cast<uint32_t>(data[i]) << 16;
        result += BASE64_CHARS[(n >> 18) & 0x3F];
        result += BASE64_CHARS[(n >> 12) & 0x3F];
        result += '=';
        result += '=';
    } else if (i + 2 == len) {
        uint32_t n = (static_cast<uint32_t>(data[i]) << 16) |
                     (static_cast<uint32_t>(data[i+1]) << 8);
        result += BASE64_CHARS[(n >> 18) & 0x3F];
        result += BASE64_CHARS[(n >> 12) & 0x3F];
        result += BASE64_CHARS[(n >> 6) & 0x3F];
        result += '=';
    }
    
    return result;
}

std::vector<uint8_t> base64_decode(const std::string& encoded) {
    std::vector<uint8_t> result;
    
    size_t len = encoded.size();
    if (len == 0) return result;
    
    // Remove padding count
    size_t padding = 0;
    if (len > 0 && encoded[len-1] == '=') padding++;
    if (len > 1 && encoded[len-2] == '=') padding++;
    
    result.reserve(((len + 3) / 4) * 3 - padding);
    
    uint32_t buf = 0;
    int bits = 0;
    
    for (char c : encoded) {
        if (c == '=') break;
        uint8_t val = BASE64_DECODE_TABLE[static_cast<uint8_t>(c)];
        if (val == 255) continue; // Skip invalid
        
        buf = (buf << 6) | val;
        bits += 6;
        
        if (bits >= 8) {
            bits -= 8;
            result.push_back(static_cast<uint8_t>((buf >> bits) & 0xFF));
        }
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Hex Encoding/Decoding
// ═══════════════════════════════════════════════════════════════════════════════

std::string to_hex(const std::vector<uint8_t>& data) {
    return to_hex(data.data(), data.size());
}

std::string to_hex(const uint8_t* data, size_t len) {
    static const char hex_chars[] = "0123456789abcdef";
    std::string result;
    result.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        result += hex_chars[(data[i] >> 4) & 0xF];
        result += hex_chars[data[i] & 0xF];
    }
    return result;
}

std::vector<uint8_t> from_hex(const std::string& hex) {
    std::vector<uint8_t> result;
    result.reserve(hex.size() / 2);
    
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        uint8_t high = 0, low = 0;
        char c = hex[i];
        if (c >= '0' && c <= '9') high = c - '0';
        else if (c >= 'a' && c <= 'f') high = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') high = c - 'A' + 10;
        
        c = hex[i+1];
        if (c >= '0' && c <= '9') low = c - '0';
        else if (c >= 'a' && c <= 'f') low = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') low = c - 'A' + 10;
        
        result.push_back((high << 4) | low);
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Security Functions
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<uint8_t> secure_random_bytes(size_t count) {
    std::vector<uint8_t> result(count);
    
    // Use system random device
    std::random_device rd;
    
    // Fill with random bytes
    for (size_t i = 0; i < count; i += 4) {
        uint32_t val = rd();
        size_t bytes_to_copy = std::min(size_t(4), count - i);
        std::memcpy(&result[i], &val, bytes_to_copy);
    }
    
    // Mix with time-based entropy
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    SHA256 mixer;
    mixer.update(result.data(), result.size());
    mixer.update(reinterpret_cast<const uint8_t*>(&now), sizeof(now));
    auto mixed = mixer.finalize();
    
    // XOR with mixed hash
    for (size_t i = 0; i < count && i < 32; ++i) {
        result[i] ^= mixed[i];
    }
    
    return result;
}

std::string generate_secure_token(size_t length) {
    static const char CHARS[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    
    auto bytes = secure_random_bytes(length);
    std::string token;
    token.reserve(length);
    
    for (size_t i = 0; i < length; ++i) {
        token += CHARS[bytes[i] % (sizeof(CHARS) - 1)];
    }
    
    return token;
}

bool secure_compare(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    
    volatile uint8_t result = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        result |= static_cast<uint8_t>(a[i]) ^ static_cast<uint8_t>(b[i]);
    }
    return result == 0;
}

bool secure_compare(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b) {
    if (a.size() != b.size()) return false;
    
    volatile uint8_t result = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        result |= a[i] ^ b[i];
    }
    return result == 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Password Hashing (PBKDF2-SHA256 simplified)
// ═══════════════════════════════════════════════════════════════════════════════

std::string hash_password(const std::string& password) {
    // Generate salt
    auto salt = secure_random_bytes(16);
    
    // PBKDF2-like iteration
    SHA256 hasher;
    std::vector<uint8_t> key(32);
    
    // Initial hash
    hasher.update(reinterpret_cast<const uint8_t*>(password.data()), password.size());
    hasher.update(salt.data(), salt.size());
    auto u = hasher.finalize();
    key = u;
    
    // Iterations (simplified - real PBKDF2 uses HMAC)
    const int iterations = 10000;
    for (int i = 1; i < iterations; ++i) {
        SHA256 h2;
        h2.update(u.data(), u.size());
        h2.update(reinterpret_cast<const uint8_t*>(password.data()), password.size());
        u = h2.finalize();
        for (size_t j = 0; j < 32; ++j) {
            key[j] ^= u[j];
        }
    }
    
    // Format: $pbkdf2$iterations$salt$hash
    std::ostringstream oss;
    oss << "$pbkdf2$" << iterations << "$" 
        << base64_encode(salt) << "$" << base64_encode(key);
    return oss.str();
}

bool verify_password(const std::string& password, const std::string& hash) {
    // Parse hash format
    if (hash.substr(0, 8) != "$pbkdf2$") {
        return false;
    }
    
    // Find components
    size_t pos1 = 8;
    size_t pos2 = hash.find('$', pos1);
    if (pos2 == std::string::npos) return false;
    
    int iterations = std::stoi(hash.substr(pos1, pos2 - pos1));
    
    size_t pos3 = hash.find('$', pos2 + 1);
    if (pos3 == std::string::npos) return false;
    
    auto salt = base64_decode(hash.substr(pos2 + 1, pos3 - pos2 - 1));
    auto stored_key = base64_decode(hash.substr(pos3 + 1));
    
    // Recompute
    SHA256 hasher;
    std::vector<uint8_t> key(32);
    
    hasher.update(reinterpret_cast<const uint8_t*>(password.data()), password.size());
    hasher.update(salt.data(), salt.size());
    auto u = hasher.finalize();
    key = u;
    
    for (int i = 1; i < iterations; ++i) {
        SHA256 h2;
        h2.update(u.data(), u.size());
        h2.update(reinterpret_cast<const uint8_t*>(password.data()), password.size());
        u = h2.finalize();
        for (size_t j = 0; j < 32; ++j) {
            key[j] ^= u[j];
        }
    }
    
    return secure_compare(key, stored_key);
}

} // namespace crypto
} // namespace registry
} // namespace sad
