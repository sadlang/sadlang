// ============================================================================
// test_jit_hash.cpp - اختبارات JIT Hash Algorithms
// JIT Hash Algorithms Tests
// ============================================================================
// الغرض: اختبار خوارزميات Hash المختلفة
// Purpose: Test various hash algorithms
// ============================================================================

#include "../include/jit_hash.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

using namespace Sad::JIT;

// ============================================================================
// Test Framework
// ============================================================================

int test_count = 0;
int passed_count = 0;

#define TEST(name) \
    void test_##name(); \
    struct Test_##name { \
        Test_##name() { \
            test_count++; \
            std::cout << "Running test: " << #name << "..."; \
            try { \
                test_##name(); \
                passed_count++; \
                std::cout << " PASSED" << std::endl; \
            } catch (const std::exception& e) { \
                std::cout << " FAILED: " << e.what() << std::endl; \
            } catch (...) { \
                std::cout << " FAILED: Unknown exception" << std::endl; \
            } \
        } \
    } test_instance_##name; \
    void test_##name()

// ============================================================================
// FNV-1a Tests
// ============================================================================

TEST(fnv1a_hash32_basic) {
    std::string test = "hello world";
    Hash32 hash = FNV1aHash::hash32(test);
    
    assert(hash != 0 && "Hash should not be zero");
    
    // التحقق من الاتساق / Verify consistency
    Hash32 hash2 = FNV1aHash::hash32(test);
    assert(hash == hash2 && "Same input should produce same hash");
    
    std::cout << " ✓ FNV-1a 32-bit basic test passes";
}

TEST(fnv1a_hash64_basic) {
    std::string test = "hello world";
    Hash64 hash = FNV1aHash::hash64(test);
    
    assert(hash != 0 && "Hash should not be zero");
    
    // التحقق من الاتساق / Verify consistency
    Hash64 hash2 = FNV1aHash::hash64(test);
    assert(hash == hash2 && "Same input should produce same hash");
    
    std::cout << " ✓ FNV-1a 64-bit basic test passes";
}

TEST(fnv1a_different_inputs) {
    Hash64 hash1 = FNV1aHash::hash64("input1");
    Hash64 hash2 = FNV1aHash::hash64("input2");
    
    assert(hash1 != hash2 && "Different inputs should produce different hashes");
    
    std::cout << " ✓ FNV-1a handles different inputs";
}

TEST(fnv1a_empty_string) {
    Hash64 hash = FNV1aHash::hash64("");
    
    // Hash فارغ يجب أن يعطي القيمة الأولية / Empty hash should give initial value
    assert(hash == FNV1aHash::FNV_64_OFFSET && "Empty string should give offset value");
    
    std::cout << " ✓ FNV-1a handles empty string";
}

TEST(fnv1a_incremental) {
    std::string full = "hello world";
    std::string part1 = "hello ";
    std::string part2 = "world";
    
    // Hash كامل / Full hash
    Hash64 full_hash = FNV1aHash::hash64(full);
    
    // Hash تراكمي / Incremental hash
    FNV1aHash::Hasher hasher;
    hasher.update(part1);
    hasher.update(part2);
    Hash64 incremental_hash = hasher.finalize();
    
    assert(full_hash == incremental_hash && "Incremental hash should match full hash");
    
    std::cout << " ✓ FNV-1a incremental hashing works";
}

TEST(fnv1a_hasher_reset) {
    FNV1aHash::Hasher hasher;
    
    hasher.update("test");
    Hash64 hash1 = hasher.finalize();
    
    hasher.reset();
    hasher.update("test");
    Hash64 hash2 = hasher.finalize();
    
    assert(hash1 == hash2 && "Reset should give same results");
    
    std::cout << " ✓ FNV-1a hasher reset works";
}

// ============================================================================
// xxHash64 Tests
// ============================================================================

TEST(xxhash64_basic) {
    std::string test = "hello world";
    Hash64 hash = XXHash64::hash64(test);
    
    assert(hash != 0 && "Hash should not be zero");
    
    // التحقق من الاتساق / Verify consistency
    Hash64 hash2 = XXHash64::hash64(test);
    assert(hash == hash2 && "Same input should produce same hash");
    
    std::cout << " ✓ xxHash64 basic test passes";
}

TEST(xxhash64_with_seed) {
    std::string test = "test data";
    
    Hash64 hash1 = XXHash64::hash64(test, 0);
    Hash64 hash2 = XXHash64::hash64(test, 123);
    
    assert(hash1 != hash2 && "Different seeds should produce different hashes");
    
    std::cout << " ✓ xxHash64 with seed works";
}

TEST(xxhash64_different_inputs) {
    Hash64 hash1 = XXHash64::hash64("input1");
    Hash64 hash2 = XXHash64::hash64("input2");
    
    assert(hash1 != hash2 && "Different inputs should produce different hashes");
    
    std::cout << " ✓ xxHash64 handles different inputs";
}

TEST(xxhash64_empty_string) {
    Hash64 hash = XXHash64::hash64("");
    
    assert(hash != 0 && "Empty string hash should not be zero");
    
    std::cout << " ✓ xxHash64 handles empty string";
}

TEST(xxhash64_short_strings) {
    // اختبار سلاسل قصيرة / Test short strings
    Hash64 hash1 = XXHash64::hash64("a");
    Hash64 hash2 = XXHash64::hash64("ab");
    Hash64 hash3 = XXHash64::hash64("abc");
    
    assert(hash1 != hash2 && hash2 != hash3 && hash1 != hash3);
    
    std::cout << " ✓ xxHash64 handles short strings";
}

TEST(xxhash64_long_strings) {
    // اختبار سلاسل طويلة / Test long strings
    std::string long_str(1000, 'a');
    Hash64 hash = XXHash64::hash64(long_str);
    
    assert(hash != 0 && "Long string hash should not be zero");
    
    std::cout << " ✓ xxHash64 handles long strings";
}

TEST(xxhash64_incremental) {
    std::string full = "hello world test data";
    std::string part1 = "hello ";
    std::string part2 = "world ";
    std::string part3 = "test data";
    
    // Hash كامل / Full hash
    Hash64 full_hash = XXHash64::hash64(full);
    
    // Hash تراكمي / Incremental hash
    XXHash64::Hasher hasher;
    hasher.update(part1);
    hasher.update(part2);
    hasher.update(part3);
    Hash64 incremental_hash = hasher.finalize();
    
    assert(full_hash == incremental_hash && "Incremental hash should match full hash");
    
    std::cout << " ✓ xxHash64 incremental hashing works";
}

TEST(xxhash64_hasher_reset) {
    XXHash64::Hasher hasher;
    
    hasher.update("test");
    Hash64 hash1 = hasher.finalize();
    
    hasher.reset();
    hasher.update("test");
    Hash64 hash2 = hasher.finalize();
    
    assert(hash1 == hash2 && "Reset should give same results");
    
    std::cout << " ✓ xxHash64 hasher reset works";
}

// ============================================================================
// CityHash64 Tests
// ============================================================================

TEST(cityhash64_basic) {
    std::string test = "hello world";
    Hash64 hash = CityHash64::hash64(test);
    
    assert(hash != 0 && "Hash should not be zero");
    
    // التحقق من الاتساق / Verify consistency
    Hash64 hash2 = CityHash64::hash64(test);
    assert(hash == hash2 && "Same input should produce same hash");
    
    std::cout << " ✓ CityHash64 basic test passes";
}

TEST(cityhash64_different_inputs) {
    Hash64 hash1 = CityHash64::hash64("input1");
    Hash64 hash2 = CityHash64::hash64("input2");
    
    assert(hash1 != hash2 && "Different inputs should produce different hashes");
    
    std::cout << " ✓ CityHash64 handles different inputs";
}

TEST(cityhash64_empty_string) {
    Hash64 hash = CityHash64::hash64("");
    
    assert(hash != 0 && "Empty string hash should not be zero");
    
    std::cout << " ✓ CityHash64 handles empty string";
}

TEST(cityhash64_various_lengths) {
    // اختبار أطوال مختلفة / Test various lengths
    std::vector<std::string> test_strings = {
        "a",                                    // Length 1
        "ab",                                   // Length 2
        "abcd",                                 // Length 4
        "abcdefgh",                             // Length 8
        "abcdefghijklmnop",                     // Length 16
        "abcdefghijklmnopqrstuvwxyz123456",    // Length 32
        std::string(64, 'x'),                   // Length 64
        std::string(128, 'y')                   // Length 128
    };
    
    std::vector<Hash64> hashes;
    for (const auto& str : test_strings) {
        Hash64 hash = CityHash64::hash64(str);
        hashes.push_back(hash);
    }
    
    // التحقق من أن كل hash فريد / Verify all hashes are unique
    for (size_t i = 0; i < hashes.size(); ++i) {
        for (size_t j = i + 1; j < hashes.size(); ++j) {
            assert(hashes[i] != hashes[j] && "Different lengths should produce different hashes");
        }
    }
    
    std::cout << " ✓ CityHash64 handles various lengths";
}

// ============================================================================
// SHA-256 Tests
// ============================================================================

TEST(sha256_basic) {
    std::string test = "hello world";
    Hash256 hash = SHA256Hash::hash256(test);
    
    // التحقق من أن Hash ليس كلياً أصفار / Verify hash is not all zeros
    bool all_zeros = true;
    for (uint8_t byte : hash) {
        if (byte != 0) {
            all_zeros = false;
            break;
        }
    }
    assert(!all_zeros && "Hash should not be all zeros");
    
    // التحقق من الاتساق / Verify consistency
    Hash256 hash2 = SHA256Hash::hash256(test);
    assert(hash == hash2 && "Same input should produce same hash");
    
    std::cout << " ✓ SHA-256 basic test passes";
}

TEST(sha256_known_vector) {
    // اختبار معروف: hash فارغ / Known test: empty hash
    Hash256 hash = SHA256Hash::hash256("");
    
    // SHA-256 للسلسلة الفارغة / SHA-256 of empty string
    // e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    Hash256 expected = {
        0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
        0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
        0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
        0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
    };
    
    assert(hash == expected && "Empty string hash should match known value");
    
    std::cout << " ✓ SHA-256 known vector test passes";
}

TEST(sha256_different_inputs) {
    Hash256 hash1 = SHA256Hash::hash256("input1");
    Hash256 hash2 = SHA256Hash::hash256("input2");
    
    assert(hash1 != hash2 && "Different inputs should produce different hashes");
    
    std::cout << " ✓ SHA-256 handles different inputs";
}

TEST(sha256_incremental) {
    std::string full = "hello world";
    std::string part1 = "hello ";
    std::string part2 = "world";
    
    // Hash كامل / Full hash
    Hash256 full_hash = SHA256Hash::hash256(full);
    
    // Hash تراكمي / Incremental hash
    SHA256Hash::Hasher hasher;
    hasher.update(part1.data(), part1.size());
    hasher.update(part2.data(), part2.size());
    Hash256 incremental_hash = hasher.finalize();
    
    assert(full_hash == incremental_hash && "Incremental hash should match full hash");
    
    std::cout << " ✓ SHA-256 incremental hashing works";
}

TEST(sha256_hasher_reset) {
    SHA256Hash::Hasher hasher;
    
    hasher.update("test", 4);
    Hash256 hash1 = hasher.finalize();
    
    hasher.reset();
    hasher.update("test", 4);
    Hash256 hash2 = hasher.finalize();
    
    assert(hash1 == hash2 && "Reset should give same results");
    
    std::cout << " ✓ SHA-256 hasher reset works";
}

// ============================================================================
// CodeHasher Tests
// ============================================================================

TEST(code_hasher_fnv1a) {
    CodeHasher hasher(HashAlgorithm::FNV1A);
    
    assert(hasher.getAlgorithm() == HashAlgorithm::FNV1A);
    assert(hasher.getAlgorithmName() == "FNV-1a");
    
    std::string code = "function test() { return 42; }";
    Hash64 hash = hasher.hash(code);
    
    assert(hash != 0 && "Hash should not be zero");
    
    std::cout << " ✓ CodeHasher with FNV-1a works";
}

TEST(code_hasher_xxhash64) {
    CodeHasher hasher(HashAlgorithm::XXH64);
    
    assert(hasher.getAlgorithm() == HashAlgorithm::XXH64);
    assert(hasher.getAlgorithmName() == "xxHash64");
    
    std::string code = "function test() { return 42; }";
    Hash64 hash = hasher.hash(code);
    
    assert(hash != 0 && "Hash should not be zero");
    
    std::cout << " ✓ CodeHasher with xxHash64 works";
}

TEST(code_hasher_cityhash64) {
    CodeHasher hasher(HashAlgorithm::CITY64);
    
    assert(hasher.getAlgorithm() == HashAlgorithm::CITY64);
    assert(hasher.getAlgorithmName() == "CityHash64");
    
    std::string code = "function test() { return 42; }";
    Hash64 hash = hasher.hash(code);
    
    assert(hash != 0 && "Hash should not be zero");
    
    std::cout << " ✓ CodeHasher with CityHash64 works";
}

TEST(code_hasher_sha256) {
    CodeHasher hasher(HashAlgorithm::SHA256);
    
    assert(hasher.getAlgorithm() == HashAlgorithm::SHA256);
    assert(hasher.getAlgorithmName() == "SHA-256");
    
    std::string code = "function test() { return 42; }";
    Hash64 hash = hasher.hash(code);
    
    assert(hash != 0 && "Hash should not be zero");
    
    std::cout << " ✓ CodeHasher with SHA-256 works";
}

TEST(code_hasher_to_hex) {
    CodeHasher hasher(HashAlgorithm::FNV1A);
    
    std::string code = "test";
    std::string hex = hasher.hashToHex(code);
    
    assert(hex.length() == 16 && "Hex string should be 16 characters");
    
    // التحقق من أن كل حرف هو hex / Verify all characters are hex
    for (char c : hex) {
        assert(((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')) && "Should be valid hex");
    }
    
    std::cout << " ✓ CodeHasher hex conversion works";
}

TEST(code_hasher_consistency) {
    std::string code = "function test() { return 42; }";
    
    CodeHasher hasher1(HashAlgorithm::FNV1A);
    CodeHasher hasher2(HashAlgorithm::FNV1A);
    
    Hash64 hash1 = hasher1.hash(code);
    Hash64 hash2 = hasher2.hash(code);
    
    assert(hash1 == hash2 && "Same algorithm should produce same hash");
    
    std::cout << " ✓ CodeHasher consistency works";
}

// ============================================================================
// Helper Functions Tests
// ============================================================================

TEST(hash_to_string_64) {
    Hash64 hash = 0x123456789ABCDEF0ULL;
    std::string str = hashToString(hash);
    
    assert(str == "123456789abcdef0" && "Should convert to hex string");
    
    std::cout << " ✓ Hash64 to string works";
}

TEST(hash_to_string_256) {
    Hash256 hash;
    for (size_t i = 0; i < 32; ++i) {
        hash[i] = static_cast<uint8_t>(i);
    }
    
    std::string str = hashToString(hash);
    assert(str.length() == 64 && "Should be 64 hex characters");
    
    std::cout << " ✓ Hash256 to string works";
}

// ============================================================================
// Performance Comparison Tests
// ============================================================================

TEST(hash_performance_comparison) {
    std::string code(1000, 'x'); // 1KB of data
    
    // FNV-1a
    auto start = std::chrono::high_resolution_clock::now();
    Hash64 fnv = FNV1aHash::hash64(code);
    auto end = std::chrono::high_resolution_clock::now();
    auto fnv_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // xxHash64
    start = std::chrono::high_resolution_clock::now();
    Hash64 xxh = XXHash64::hash64(code);
    end = std::chrono::high_resolution_clock::now();
    auto xxh_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // CityHash64
    start = std::chrono::high_resolution_clock::now();
    Hash64 city = CityHash64::hash64(code);
    end = std::chrono::high_resolution_clock::now();
    auto city_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    // SHA-256
    start = std::chrono::high_resolution_clock::now();
    Hash256 sha = SHA256Hash::hash256(code);
    end = std::chrono::high_resolution_clock::now();
    auto sha_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    
    std::cout << " ✓ Performance: FNV=" << fnv_time << "ns, XXH=" << xxh_time 
              << "ns, City=" << city_time << "ns, SHA=" << sha_time << "ns";
    
    // جميع الخوارزميات يجب أن تعمل / All algorithms should work
    assert(fnv != 0 && xxh != 0 && city != 0);
}

TEST(hash_collision_resistance) {
    // اختبار بسيط لمقاومة التصادم / Simple collision resistance test
    std::vector<Hash64> hashes;
    
    for (int i = 0; i < 1000; ++i) {
        std::string input = "test_" + std::to_string(i);
        Hash64 hash = XXHash64::hash64(input);
        hashes.push_back(hash);
    }
    
    // التحقق من عدم وجود تكرارات / Verify no duplicates
    std::sort(hashes.begin(), hashes.end());
    auto it = std::adjacent_find(hashes.begin(), hashes.end());
    
    assert(it == hashes.end() && "Should have no collisions in 1000 inputs");
    
    std::cout << " ✓ Hash collision resistance test passes";
}

// ============================================================================
// Main
// ============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "JIT Hash Algorithms Tests" << std::endl;
    std::cout << "اختبارات خوارزميات Hash" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // الاختبارات تُنفَّذ تلقائياً / Tests run automatically
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Test Results / نتائج الاختبارات" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total tests: " << test_count << std::endl;
    std::cout << "Passed: " << passed_count << std::endl;
    std::cout << "Failed: " << (test_count - passed_count) << std::endl;
    std::cout << "Success rate: " << (passed_count * 100 / test_count) << "%" << std::endl;
    
    return (test_count == passed_count) ? 0 : 1;
}
