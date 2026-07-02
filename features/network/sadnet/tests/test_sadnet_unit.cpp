// ═══════════════════════════════════════════════════════════════════
// صNet — اختبارات وحدة C++ شاملة
// (AR) اختبارات تلقائية لجميع وحدات مكتبة صNet:
//      الأنواع الأساسية (Buffer, NodeId, Endpoint, Result)
//      التشفير (SHA-256, HMAC, HKDF, AES-GCM, X25519)
//      جدول التوجيه DHT (K-Bucket, XOR distance)
//      بروتوكول Double Ratchet (تشفير/فك تشفير جلسة)
// (EN) Unit tests for all sadnet modules
// المكون: features/network/sadnet (اختبارات الوحدة)
// المسار: features/network/sadnet/tests/test_sadnet_unit.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/core/types.h"
#include "sadnet/crypto/crypto.h"
#include "sadnet/dht/dht.h"

#include <cassert>
#include <chrono>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// ─── إطار الاختبارات البسيط ─────────────────────────────────────

namespace test
{

    static int total = 0;
    static int passed = 0;
    static int failed = 0;

    struct TestCase
    {
        std::string name;
        std::function<void()> func;
    };

    static std::vector<TestCase> all_tests;

    // (AR) تسجيل اختبار جديد
    void register_test(const std::string &name, std::function<void()> func)
    {
        all_tests.push_back({name, std::move(func)});
    }

    // (AR) تأكيد شرط — يسجل نجاحاً أو فشلاً
    void check(bool condition, const std::string &msg, const char *file, int line)
    {
        total++;
        if (condition)
        {
            passed++;
        }
        else
        {
            failed++;
            std::cerr << "  [فشل] " << msg
                      << " (" << file << ":" << line << ")" << std::endl;
        }
    }

    // (AR) تشغيل جميع الاختبارات المسجلة
    int run_all()
    {
        for (const auto &tc : all_tests)
        {
            std::cout << "--- " << tc.name << " ---" << std::endl;
            try
            {
                tc.func();
            }
            catch (const std::exception &e)
            {
                failed++;
                total++;
                std::cerr << "  [استثناء] " << e.what() << std::endl;
            }
            catch (...)
            {
                failed++;
                total++;
                std::cerr << "  [استثناء غير معروف]" << std::endl;
            }
        }

        std::cout << "\n═══════════════════════════════════════" << std::endl;
        std::cout << "المجموع: " << total
                  << " | نجح: " << passed
                  << " | فشل: " << failed << std::endl;
        std::cout << "═══════════════════════════════════════" << std::endl;

        return (failed == 0) ? 0 : 1;
    }

} // namespace test

#define CHECK(cond, msg) test::check((cond), (msg), __FILE__, __LINE__)

// ═══════════════════════════════════════════════════════════════════
// القسم 1: اختبارات Buffer
// ═══════════════════════════════════════════════════════════════════

void test_buffer_basic()
{
    using sad::net::Buffer;

    // (AR) إنشاء فارغ
    Buffer empty;
    CHECK(empty.empty(), "Buffer فارغ عند الإنشاء");
    CHECK(empty.size() == 0, "Buffer حجمه 0");

    // (AR) إنشاء بحجم
    Buffer sized(16);
    CHECK(sized.size() == 16, "Buffer بحجم 16");
    CHECK(sized[0] == 0, "Buffer مبدئياً أصفار");

    // (AR) إنشاء من مؤشر وطول
    uint8_t raw[] = {1, 2, 3, 4, 5};
    Buffer from_ptr(raw, 5);
    CHECK(from_ptr.size() == 5, "Buffer من مؤشر بطول 5");
    CHECK(from_ptr[0] == 1, "Buffer[0] == 1");
    CHECK(from_ptr[4] == 5, "Buffer[4] == 5");

    // (AR) إنشاء من نص
    Buffer from_str("hello");
    CHECK(from_str.size() == 5, "Buffer من نص بطول 5");
    CHECK(from_str.to_string() == "hello", "Buffer.to_string() == 'hello'");

    // (AR) إنشاء من vector
    std::vector<uint8_t> vec = {10, 20, 30};
    Buffer from_vec(vec);
    CHECK(from_vec.size() == 3, "Buffer من vector بطول 3");
    CHECK(from_vec[1] == 20, "Buffer[1] == 20");
}

void test_buffer_operations()
{
    using sad::net::Buffer;

    // (AR) push_back
    Buffer buf;
    buf.push_back(0xAA);
    buf.push_back(0xBB);
    CHECK(buf.size() == 2, "push_back يزيد الحجم");
    CHECK(buf[0] == 0xAA, "push_back أول عنصر");
    CHECK(buf[1] == 0xBB, "push_back ثاني عنصر");

    // (AR) append من مؤشر
    uint8_t extra[] = {0xCC, 0xDD};
    buf.append(extra, 2);
    CHECK(buf.size() == 4, "append يزيد الحجم");
    CHECK(buf[3] == 0xDD, "append آخر عنصر");

    // (AR) append من Buffer
    Buffer other("XY");
    buf.append(other);
    CHECK(buf.size() == 6, "append Buffer يزيد الحجم");

    // (AR) resize
    buf.resize(3);
    CHECK(buf.size() == 3, "resize يقلص الحجم");

    // (AR) clear
    buf.clear();
    CHECK(buf.empty(), "clear يفرغ المخزن");

    // (AR) مقارنة
    Buffer a("test");
    Buffer b("test");
    Buffer c("other");
    CHECK(a == b, "مقارنة Buffers متساويين");
    CHECK(!(a == c), "مقارنة Buffers مختلفين");
}

void test_buffer_hex()
{
    using sad::net::Buffer;

    // (AR) تحويل Buffer إلى hex
    uint8_t raw[] = {0xDE, 0xAD, 0xBE, 0xEF};
    Buffer buf(raw, 4);
    std::string hex = buf.to_hex();
    CHECK(hex == "deadbeef", "to_hex() يُنتج deadbeef");

    // (AR) إنشاء Buffer من hex
    Buffer from_hex = Buffer::from_hex("cafebabe");
    CHECK(from_hex.size() == 4, "from_hex بطول 4");
    CHECK(from_hex[0] == 0xCA, "from_hex[0] == 0xCA");
    CHECK(from_hex[3] == 0xBE, "from_hex[3] == 0xBE");

    // (AR) دورة كاملة: Buffer → hex → Buffer
    Buffer original("Hello!");
    Buffer roundtrip = Buffer::from_hex(original.to_hex());
    CHECK(original == roundtrip, "Buffer ↔ hex دورة كاملة متطابقة");
}

void test_buffer_secure_clear()
{
    using sad::net::Buffer;

    // (AR) مسح آمن يصفّر البيانات
    Buffer secret("SuperSecret123");
    CHECK(!secret.empty(), "البيانات السرية موجودة");
    secret.secure_clear();
    CHECK(secret.empty(), "secure_clear يفرغ المخزن");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 2: اختبارات NodeId
// ═══════════════════════════════════════════════════════════════════

void test_nodeid_basic()
{
    using sad::net::NODE_ID_SIZE;
    using sad::net::NodeId;

    // (AR) معرّف فارغ
    NodeId zero;
    CHECK(zero.is_zero(), "NodeId الافتراضي أصفار");

    // (AR) إنشاء من مصفوفة
    std::array<uint8_t, NODE_ID_SIZE> arr{};
    arr[0] = 0xFF;
    arr[31] = 0x01;
    NodeId filled(arr);
    CHECK(!filled.is_zero(), "NodeId ليس فارغاً بعد التعبئة");
    CHECK(filled.data[0] == 0xFF, "NodeId.data[0] == 0xFF");

    // (AR) مقارنة
    NodeId a, b;
    CHECK(a == b, "معرّفان فارغان متساويان");
    a.data[0] = 1;
    CHECK(a != b, "معرّفان مختلفان بعد التعديل");
}

void test_nodeid_hex()
{
    using sad::net::NodeId;

    // (AR) تحويل hex
    NodeId id;
    id.data[0] = 0xAB;
    id.data[1] = 0xCD;
    std::string hex = id.to_hex();
    CHECK(hex.size() == 64, "to_hex طوله 64 حرف");
    CHECK(hex.substr(0, 4) == "abcd", "to_hex يبدأ بـ abcd");

    // (AR) دورة كاملة
    NodeId roundtrip = NodeId::from_hex(hex);
    CHECK(id == roundtrip, "NodeId ↔ hex دورة كاملة");
}

void test_nodeid_xor_distance()
{
    using sad::net::NodeId;

    NodeId a, b;
    a.data[0] = 0xFF;
    b.data[0] = 0x0F;

    NodeId dist = a.xor_distance(b);
    CHECK(dist.data[0] == 0xF0, "XOR distance صحيحة");

    // (AR) XOR مع النفس = صفر
    NodeId self_dist = a.xor_distance(a);
    CHECK(self_dist.is_zero(), "XOR distance مع النفس = 0");
}

void test_nodeid_leading_zeros()
{
    using sad::net::NodeId;

    // (AR) كله أصفار = 256 leading zeros
    NodeId zero;
    CHECK(zero.leading_zero_bits() == 256, "256 بت أصفار قيادية للمعرّف الفارغ");

    // (AR) البت الأعلى مفعّل = 0 leading zeros
    NodeId top;
    top.data[0] = 0x80; // 10000000
    CHECK(top.leading_zero_bits() == 0, "0 أصفار قيادية عندما البت الأعلى مفعّل");

    // (AR) 8 أصفار ثم بت مفعّل
    NodeId mid;
    mid.data[1] = 0x40; // 01000000
    CHECK(mid.leading_zero_bits() == 9, "9 أصفار قيادية");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 3: اختبارات Endpoint
// ═══════════════════════════════════════════════════════════════════

void test_endpoint()
{
    using sad::net::Endpoint;

    // (AR) إنشاء IPv4
    Endpoint ep("192.168.1.1", 8080);
    CHECK(ep.address == "192.168.1.1", "عنوان IPv4 صحيح");
    CHECK(ep.port == 8080, "منفذ صحيح");
    CHECK(!ep.is_ipv6, "ليس IPv6");
    CHECK(ep.to_string() == "192.168.1.1:8080", "to_string IPv4");

    // (AR) إنشاء IPv6
    Endpoint ep6("::1", 443, true);
    CHECK(ep6.is_ipv6, "IPv6 مفعّل");
    CHECK(ep6.to_string() == "[::1]:443", "to_string IPv6 بأقواس مربعة");

    // (AR) مقارنة
    Endpoint same("192.168.1.1", 8080);
    CHECK(ep == same, "Endpoints متساويان");

    Endpoint diff("192.168.1.2", 8080);
    CHECK(!(ep == diff), "Endpoints مختلفان");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 4: اختبارات Result
// ═══════════════════════════════════════════════════════════════════

void test_result()
{
    using sad::net::NetError;
    using sad::net::Result;

    // (AR) نتيجة ناجحة
    auto ok = Result<int>::success(42);
    CHECK(ok.ok(), "Result::success() يُرجع ok");
    CHECK(ok.value == 42, "Result.value == 42");

    // (AR) نتيجة فاشلة
    auto err = Result<int>::fail(NetError::TIMEOUT);
    CHECK(!err.ok(), "Result::fail() لا يُرجع ok");
    CHECK(err.error == NetError::TIMEOUT, "Result.error == TIMEOUT");

    // (AR) تحويل bool
    CHECK(static_cast<bool>(ok), "Result ناجح يحوّل إلى true");
    CHECK(!static_cast<bool>(err), "Result فاشل يحوّل إلى false");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 5: اختبارات SHA-256
// ═══════════════════════════════════════════════════════════════════

void test_sha256_basic()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    // (AR) بصمة النص الفارغ — قيمة معروفة من المعيار
    Hash256 empty_hash = sha256(reinterpret_cast<const uint8_t *>(""), 0);
    Buffer empty_buf(empty_hash.data(), empty_hash.size());
    CHECK(empty_buf.to_hex() ==
              "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
          "SHA-256('') يطابق المعيار");

    // (AR) بصمة "abc" — قيمة معروفة
    Hash256 abc_hash = sha256(std::string("abc"));
    Buffer abc_buf(abc_hash.data(), abc_hash.size());
    CHECK(abc_buf.to_hex() ==
              "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
          "SHA-256('abc') يطابق المعيار");
}

void test_sha256_deterministic()
{
    using namespace sad::net::crypto;

    // (AR) نفس الإدخال = نفس البصمة (حتمية)
    Hash256 h1 = sha256(std::string("مرحبا"));
    Hash256 h2 = sha256(std::string("مرحبا"));
    CHECK(h1 == h2, "SHA-256 حتمية (نفس الإدخال = نفس الناتج)");

    // (AR) إدخال مختلف = بصمة مختلفة
    Hash256 h3 = sha256(std::string("مختلف"));
    CHECK(h1 != h3, "SHA-256 إدخال مختلف = بصمة مختلفة");
}

void test_sha256_buffer_overload()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    // (AR) حساب SHA-256 من Buffer
    Buffer data("hello world");
    Hash256 h1 = sha256(data);

    // (AR) المقارنة مع الحساب من المؤشر
    Hash256 h2 = sha256(data.data(), data.size());
    CHECK(h1 == h2, "SHA-256(Buffer) == SHA-256(ptr, len)");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 6: اختبارات HMAC-SHA256
// ═══════════════════════════════════════════════════════════════════

void test_hmac_sha256()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    uint8_t key[] = "mysecretkey";
    uint8_t data[] = "message to authenticate";

    // (AR) حساب HMAC
    Hash256 mac1 = hmac_sha256(key, sizeof(key) - 1, data, sizeof(data) - 1);
    Hash256 mac2 = hmac_sha256(key, sizeof(key) - 1, data, sizeof(data) - 1);
    CHECK(mac1 == mac2, "HMAC-SHA256 حتمية");

    // (AR) مفتاح مختلف = HMAC مختلف
    uint8_t key2[] = "differentkey";
    Hash256 mac3 = hmac_sha256(key2, sizeof(key2) - 1, data, sizeof(data) - 1);
    CHECK(mac1 != mac3, "HMAC-SHA256 مفتاح مختلف = ناتج مختلف");

    // (AR) بيانات مختلفة = HMAC مختلف
    uint8_t data2[] = "other message";
    Hash256 mac4 = hmac_sha256(key, sizeof(key) - 1, data2, sizeof(data2) - 1);
    CHECK(mac1 != mac4, "HMAC-SHA256 بيانات مختلفة = ناتج مختلف");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 7: اختبارات HKDF-SHA256
// ═══════════════════════════════════════════════════════════════════

void test_hkdf_sha256()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    uint8_t salt[] = "my_salt_data";
    uint8_t ikm[] = "initial_key_material";
    uint8_t info[] = "context_info";

    // (AR) اشتقاق مفتاح بطول 32 بايت
    Buffer key32 = hkdf_sha256(salt, sizeof(salt) - 1,
                               ikm, sizeof(ikm) - 1,
                               info, sizeof(info) - 1,
                               32);
    CHECK(key32.size() == 32, "HKDF أنتج 32 بايت");

    // (AR) اشتقاق مفتاح بطول 64 بايت
    Buffer key64 = hkdf_sha256(salt, sizeof(salt) - 1,
                               ikm, sizeof(ikm) - 1,
                               info, sizeof(info) - 1,
                               64);
    CHECK(key64.size() == 64, "HKDF أنتج 64 بايت");

    // (AR) أول 32 بايت من key64 لا تساوي key32 بالضرورة
    //      (لأن HKDF-Expand يعتمد على عداد مختلف)
    //      لكن نتأكد من أنهما ليسا فارغين
    CHECK(!key32.empty(), "HKDF خرج غير فارغ (32)");
    CHECK(!key64.empty(), "HKDF خرج غير فارغ (64)");

    // (AR) حتمية — نفس الإدخال = نفس الناتج
    Buffer key32b = hkdf_sha256(salt, sizeof(salt) - 1,
                                ikm, sizeof(ikm) - 1,
                                info, sizeof(info) - 1,
                                32);
    CHECK(key32 == key32b, "HKDF حتمية");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 8: اختبارات AES-256-GCM
// ═══════════════════════════════════════════════════════════════════

void test_aes_gcm_encrypt_decrypt()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    // (AR) مفتاح عشوائي 256 بت
    uint8_t key[AES_KEY_SIZE];
    random_bytes(key, AES_KEY_SIZE);

    // (AR) نص أصلي
    std::string plaintext_str = "This is a secret message for testing AES-GCM!";
    const uint8_t *pt = reinterpret_cast<const uint8_t *>(plaintext_str.data());
    size_t pt_len = plaintext_str.size();

    // (AR) تشفير
    AEADResult encrypted = aes_gcm_encrypt(key, pt, pt_len);
    CHECK(!encrypted.ciphertext.empty(), "النص المشفر ليس فارغاً");
    CHECK(encrypted.ciphertext.size() == pt_len, "حجم النص المشفر = حجم الأصلي");

    // (AR) فك التشفير
    Buffer decrypted = aes_gcm_decrypt(key,
                                       encrypted.ciphertext.data(),
                                       encrypted.ciphertext.size(),
                                       encrypted.nonce.data(),
                                       encrypted.tag.data());
    CHECK(decrypted.size() == pt_len, "حجم النص المفكوك = الأصلي");
    CHECK(decrypted.to_string() == plaintext_str,
          "فك التشفير يُرجع النص الأصلي");
}

void test_aes_gcm_with_aad()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    // (AR) تشفير مع بيانات مصادقة إضافية (AAD)
    uint8_t key[AES_KEY_SIZE];
    random_bytes(key, AES_KEY_SIZE);

    std::string pt_str = "authenticated data test";
    std::string aad_str = "metadata header";
    const uint8_t *pt = reinterpret_cast<const uint8_t *>(pt_str.data());
    const uint8_t *aad = reinterpret_cast<const uint8_t *>(aad_str.data());

    AEADResult enc = aes_gcm_encrypt(key, pt, pt_str.size(), aad, aad_str.size());

    // (AR) فك تشفير صحيح مع AAD
    Buffer dec = aes_gcm_decrypt(key,
                                 enc.ciphertext.data(), enc.ciphertext.size(),
                                 enc.nonce.data(), enc.tag.data(),
                                 aad, aad_str.size());
    CHECK(dec.to_string() == pt_str, "فك تشفير مع AAD صحيح");

    // (AR) فك تشفير بـ AAD مختلف يجب أن يفشل
    std::string wrong_aad = "wrong metadata";
    Buffer bad_dec = aes_gcm_decrypt(key,
                                     enc.ciphertext.data(), enc.ciphertext.size(),
                                     enc.nonce.data(), enc.tag.data(),
                                     reinterpret_cast<const uint8_t *>(wrong_aad.data()),
                                     wrong_aad.size());
    CHECK(bad_dec.empty(), "فك تشفير بـ AAD خاطئ يفشل (Buffer فارغ)");
}

void test_aes_gcm_tamper_detection()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    // (AR) التحقق من كشف العبث بالنص المشفر
    uint8_t key[AES_KEY_SIZE];
    random_bytes(key, AES_KEY_SIZE);

    std::string pt_str = "tamper detection test";
    const uint8_t *pt = reinterpret_cast<const uint8_t *>(pt_str.data());

    AEADResult enc = aes_gcm_encrypt(key, pt, pt_str.size());

    // (AR) تعديل بايت في النص المشفر
    Buffer tampered = enc.ciphertext;
    if (!tampered.empty())
    {
        tampered[0] ^= 0xFF; // عكس بايت
    }

    Buffer dec = aes_gcm_decrypt(key,
                                 tampered.data(), tampered.size(),
                                 enc.nonce.data(), enc.tag.data());
    CHECK(dec.empty(), "كشف العبث بالنص المشفر (Buffer فارغ)");

    // (AR) تعديل بايت في الوسم
    auto bad_tag = enc.tag;
    bad_tag[0] ^= 0xFF;
    Buffer dec2 = aes_gcm_decrypt(key,
                                  enc.ciphertext.data(), enc.ciphertext.size(),
                                  enc.nonce.data(), bad_tag.data());
    CHECK(dec2.empty(), "كشف العبث بالوسم (Buffer فارغ)");
}

void test_aes_gcm_wrong_key()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    // (AR) فك التشفير بمفتاح خاطئ يفشل
    uint8_t key1[AES_KEY_SIZE], key2[AES_KEY_SIZE];
    random_bytes(key1, AES_KEY_SIZE);
    random_bytes(key2, AES_KEY_SIZE);

    std::string pt_str = "wrong key test";
    const uint8_t *pt = reinterpret_cast<const uint8_t *>(pt_str.data());

    AEADResult enc = aes_gcm_encrypt(key1, pt, pt_str.size());

    Buffer dec = aes_gcm_decrypt(key2,
                                 enc.ciphertext.data(), enc.ciphertext.size(),
                                 enc.nonce.data(), enc.tag.data());
    CHECK(dec.empty(), "فك التشفير بمفتاح خاطئ يفشل");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 9: اختبارات X25519
// ═══════════════════════════════════════════════════════════════════

void test_x25519_keygen()
{
    using namespace sad::net::crypto;

    // (AR) توليد زوج مفاتيح
    KeyPair kp = x25519_generate_keypair();

    // (AR) المفاتيح ليست أصفاراً
    bool pub_all_zero = true;
    bool priv_all_zero = true;
    for (size_t i = 0; i < 32; i++)
    {
        if (kp.public_key[i] != 0)
            pub_all_zero = false;
        if (kp.private_key[i] != 0)
            priv_all_zero = false;
    }
    CHECK(!pub_all_zero, "المفتاح العام ليس أصفاراً");
    CHECK(!priv_all_zero, "المفتاح الخاص ليس أصفاراً");

    // (AR) مفتاحان مختلفان عند كل توليد
    KeyPair kp2 = x25519_generate_keypair();
    CHECK(kp.public_key != kp2.public_key, "أزواج مفاتيح فريدة");
}

void test_x25519_shared_secret()
{
    using namespace sad::net::crypto;

    // (AR) اتفاق Diffie-Hellman: الطرفان يصلان لنفس السر المشترك
    KeyPair alice = x25519_generate_keypair();
    KeyPair bob = x25519_generate_keypair();

    X25519Key shared_alice = x25519_shared_secret(alice.private_key, bob.public_key);
    X25519Key shared_bob = x25519_shared_secret(bob.private_key, alice.public_key);

    CHECK(shared_alice == shared_bob,
          "Diffie-Hellman: الطرفان يصلان لنفس السر المشترك");

    // (AR) السر المشترك ليس أصفاراً
    bool is_zero = true;
    for (size_t i = 0; i < 32; i++)
    {
        if (shared_alice[i] != 0)
            is_zero = false;
    }
    CHECK(!is_zero, "السر المشترك ليس أصفاراً");
}

void test_x25519_rfc7748_vector()
{
    using namespace sad::net::crypto;

    // (AR) اختبار RFC 7748 Section 6.1 — قيم اختبار معيارية
    // Alice's private key
    uint8_t alice_priv_bytes[32] = {
        0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d,
        0x3c, 0x16, 0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45,
        0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0, 0x99, 0x2a,
        0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a};
    // Bob's public key
    uint8_t bob_pub_bytes[32] = {
        0xde, 0x9e, 0xdb, 0x7d, 0x7b, 0x7d, 0xc1, 0xb4,
        0xd3, 0x5b, 0x61, 0xc2, 0xec, 0xe4, 0x35, 0x37,
        0x3f, 0x83, 0x43, 0xc8, 0x5b, 0x78, 0x67, 0x4d,
        0xad, 0xfc, 0x7e, 0x14, 0x6f, 0x88, 0x2b, 0x4f};
    // Expected shared secret
    uint8_t expected_shared[32] = {
        0x4a, 0x5d, 0x9d, 0x5b, 0xa4, 0xce, 0x2d, 0xe1,
        0x72, 0x8e, 0x3b, 0xf4, 0x80, 0x35, 0x0f, 0x25,
        0xe0, 0x7e, 0x21, 0xc9, 0x47, 0xd1, 0x9e, 0x33,
        0x76, 0xf0, 0x9b, 0x3c, 0x1e, 0x16, 0x17, 0x42};

    X25519Key alice_priv, bob_pub;
    std::memcpy(alice_priv.data(), alice_priv_bytes, 32);
    std::memcpy(bob_pub.data(), bob_pub_bytes, 32);

    X25519Key result = x25519_shared_secret(alice_priv, bob_pub);
    bool match = (std::memcmp(result.data(), expected_shared, 32) == 0);
    CHECK(match, "RFC 7748 test vector: shared secret matches");

    // (AR) اطبع النتيجة للتشخيص إذا فشل
    if (!match)
    {
        fprintf(stderr, "  Expected: ");
        for (int i = 0; i < 32; i++)
            fprintf(stderr, "%02x", expected_shared[i]);
        fprintf(stderr, "\n  Got:      ");
        for (int i = 0; i < 32; i++)
            fprintf(stderr, "%02x", result[i]);
        fprintf(stderr, "\n");
    }
}

// ═══════════════════════════════════════════════════════════════════
// القسم 10: اختبارات CSPRNG (random_bytes)
// ═══════════════════════════════════════════════════════════════════

void test_random_bytes()
{
    using namespace sad::net::crypto;

    // (AR) توليد بايتات عشوائية
    uint8_t buf1[32], buf2[32];
    bool ok1 = random_bytes(buf1, 32);
    bool ok2 = random_bytes(buf2, 32);
    CHECK(ok1, "random_bytes نجح (1)");
    CHECK(ok2, "random_bytes نجح (2)");

    // (AR) استدعاءان مختلفان يُنتجان قيماً مختلفة (احتمال التطابق ~ 2^-256)
    CHECK(std::memcmp(buf1, buf2, 32) != 0,
          "random_bytes يُنتج قيماً مختلفة في كل استدعاء");

    // (AR) Buffer عشوائي
    using sad::net::Buffer;
    Buffer rbuf = random_buffer(16);
    CHECK(rbuf.size() == 16, "random_buffer بطول 16");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 11: اختبارات Double Ratchet
// ═══════════════════════════════════════════════════════════════════

void test_double_ratchet()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    // (AR) إنشاء جلسة Ratchet لأليس وبوب
    KeyPair alice_kp = x25519_generate_keypair();
    KeyPair bob_kp = x25519_generate_keypair();

    // (AR) حساب السر المشترك عبر Diffie-Hellman
    X25519Key shared = x25519_shared_secret(alice_kp.private_key, bob_kp.public_key);

    DoubleRatchetSession alice_session;
    alice_session.init_sender(shared, bob_kp.public_key);

    DoubleRatchetSession bob_session;
    bob_session.init_receiver(shared, bob_kp);

    // (AR) أليس تشفّر رسالة
    std::string msg = "Hello from Alice!";
    auto encrypted = alice_session.encrypt(
        reinterpret_cast<const uint8_t *>(msg.data()), msg.size());

    CHECK(!encrypted.ciphertext.empty(), "Ratchet: النص المشفر ليس فارغاً");

    // (AR) بوب يفك التشفير
    Buffer decrypted = bob_session.decrypt(encrypted);
    CHECK(decrypted.to_string() == msg,
          "Ratchet: فك التشفير يُرجع الرسالة الأصلية");
}

void test_double_ratchet_multiple_messages()
{
    using namespace sad::net;
    using namespace sad::net::crypto;

    KeyPair alice_kp = x25519_generate_keypair();
    KeyPair bob_kp = x25519_generate_keypair();

    // (AR) حساب السر المشترك عبر Diffie-Hellman
    X25519Key shared = x25519_shared_secret(alice_kp.private_key, bob_kp.public_key);

    DoubleRatchetSession alice_session;
    alice_session.init_sender(shared, bob_kp.public_key);

    DoubleRatchetSession bob_session;
    bob_session.init_receiver(shared, bob_kp);

    // (AR) إرسال عدة رسائل
    std::vector<std::string> messages = {
        "Message 1",
        "Message 2 - longer text for testing",
        "Msg 3",
        "رسالة عربية للاختبار"};

    for (const auto &m : messages)
    {
        auto enc = alice_session.encrypt(
            reinterpret_cast<const uint8_t *>(m.data()), m.size());
        Buffer dec = bob_session.decrypt(enc);
        CHECK(dec.to_string() == m,
              "Ratchet: رسالة متعددة '" + m + "' فُكّت بنجاح");
    }
}

// ═══════════════════════════════════════════════════════════════════
// القسم 12: اختبارات DHT Routing Table
// ═══════════════════════════════════════════════════════════════════

void test_dht_routing_table()
{
    using namespace sad::net;
    using namespace sad::net::dht;
    using namespace sad::net::crypto;

    // (AR) إنشاء عقدة محلية
    Hash256 local_hash = sha256(std::string("local_node"));
    NodeId local_id(local_hash);

    RoutingTable table(local_id);

    // (AR) إضافة عقد قريبة
    for (int i = 1; i <= 10; i++)
    {
        Hash256 h = sha256(std::string("node_") + std::to_string(i));
        NodeId nid(h);
        Endpoint ep("192.168.1." + std::to_string(i), 7654);
        NodeInfo info;
        info.id = nid;
        info.endpoint = ep;
        info.is_good = true;
        table.add_or_update(info);
    }

    // (AR) البحث عن أقرب العقد
    Hash256 target_hash = sha256(std::string("target"));
    NodeId target(target_hash);

    auto closest = table.find_closest(target, 5);
    CHECK(closest.size() <= 5, "find_closest يُرجع ≤ 5 عقد");
    CHECK(!closest.empty(), "find_closest يُرجع عقداً");
}

void test_dht_routing_table_remove()
{
    using namespace sad::net;
    using namespace sad::net::dht;
    using namespace sad::net::crypto;

    Hash256 local_hash = sha256(std::string("my_node"));
    NodeId local_id(local_hash);
    RoutingTable table(local_id);

    // (AR) إضافة عقدة ثم إزالتها
    Hash256 h = sha256(std::string("removable_node"));
    NodeId nid(h);
    Endpoint ep("10.0.0.1", 8000);

    NodeInfo info;
    info.id = nid;
    info.endpoint = ep;
    info.is_good = true;
    table.add_or_update(info);

    auto before = table.find_closest(nid, 1);
    CHECK(!before.empty(), "العقدة موجودة قبل الإزالة");

    table.remove(nid);

    auto after = table.find_closest(nid, 1);
    // (AR) بعد الإزالة لن تظهر + لن تكون قريبة
    bool found = false;
    for (const auto &entry : after)
    {
        if (entry.id == nid)
            found = true;
    }
    CHECK(!found, "العقدة لا تظهر بعد الإزالة");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 13: اختبارات أكواد الأخطاء
// ═══════════════════════════════════════════════════════════════════

void test_error_codes()
{
    using sad::net::error_to_arabic;
    using sad::net::error_to_english;
    using sad::net::NetError;

    // (AR) التحقق من أن الأخطاء تُترجم بشكل صحيح
    CHECK(std::strlen(error_to_arabic(NetError::OK)) > 0,
          "error_to_arabic(OK) ليس فارغاً");
    CHECK(std::strlen(error_to_english(NetError::OK)) > 0,
          "error_to_english(OK) ليس فارغاً");
    CHECK(std::strlen(error_to_arabic(NetError::TIMEOUT)) > 0,
          "error_to_arabic(TIMEOUT) ليس فارغاً");
    CHECK(std::strlen(error_to_english(NetError::CRYPTO_ERROR)) > 0,
          "error_to_english(CRYPTO_ERROR) ليس فارغاً");
}

// ═══════════════════════════════════════════════════════════════════
// القسم 14: اختبارات الطابع الزمني
// ═══════════════════════════════════════════════════════════════════

void test_timestamp()
{
    using sad::net::now_ms;

    // (AR) الطابع الزمني معقول (بعد 2020-01-01 بالمللي ثانية)
    uint64_t ts = now_ms();
    CHECK(ts > 1577836800000ULL, "الطابع الزمني بعد 2020");

    // (AR) استدعاءان متتاليان — الثاني >= الأول
    uint64_t ts2 = now_ms();
    CHECK(ts2 >= ts, "الطابع الزمني متزايد");
}

// ═══════════════════════════════════════════════════════════════════
// القسم: اختبارات NIST AES-256-GCM (متجهات رسمية)
// (AR) متجهات اختبار من مواصفات GCM (NIST SP 800-38D)
//      Test Cases 13-16 لـ AES-256
//      تتحقق من صحة التشفير والوسم والفك مع nonce/AAD محددين
// ═══════════════════════════════════════════════════════════════════

// (AR) دالة مساعدة لتحويل hex string إلى مصفوفة بايتات
static std::vector<uint8_t> hex_to_bytes(const std::string &hex)
{
    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2)
    {
        uint8_t hi = (hex[i] >= 'a')   ? (hex[i] - 'a' + 10)
                     : (hex[i] >= 'A') ? (hex[i] - 'A' + 10)
                                       : (hex[i] - '0');
        uint8_t lo = (hex[i + 1] >= 'a')   ? (hex[i + 1] - 'a' + 10)
                     : (hex[i + 1] >= 'A') ? (hex[i + 1] - 'A' + 10)
                                           : (hex[i + 1] - '0');
        out.push_back((hi << 4) | lo);
    }
    return out;
}

// (AR) دالة مساعدة لمقارنة مصفوفتين بايت
static bool bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    for (size_t i = 0; i < len; i++)
        if (a[i] != b[i])
            return false;
    return true;
}

// ─── NIST GCM Test Case 13 ──────────────────────────────────────
// (AR) AES-256-GCM: مفتاح أصفار، IV أصفار، بدون نص، بدون AAD
//      يتحقق فقط من وسم المصادقة (Tag)
void test_nist_gcm_case13()
{
    using namespace sad::net::crypto;
    // Key = 32 bytes of zeros
    uint8_t key[32] = {};
    // IV = 12 bytes of zeros
    uint8_t iv[12] = {};
    // No plaintext, no AAD
    auto result = aes_gcm_encrypt_with_nonce(key, iv, nullptr, 0, nullptr, 0);

    // (AR) النص المُشفّر يجب أن يكون فارغاً
    CHECK(result.ciphertext.empty(), "NIST-13: ciphertext empty");

    // Expected Tag: 530f8afbc74536b9a963b4f1c4cb738b
    auto expected_tag = hex_to_bytes("530f8afbc74536b9a963b4f1c4cb738b");
    CHECK(bytes_equal(result.tag.data(), expected_tag.data(), 16),
          "NIST-13: tag matches");
}

// ─── NIST GCM Test Case 14 ──────────────────────────────────────
// (AR) AES-256-GCM: مفتاح أصفار، IV أصفار، نص 16 بايت أصفار
void test_nist_gcm_case14()
{
    using namespace sad::net::crypto;
    uint8_t key[32] = {};
    uint8_t iv[12] = {};
    uint8_t pt[16] = {};

    auto result = aes_gcm_encrypt_with_nonce(key, iv, pt, 16, nullptr, 0);

    // Expected CT:  cea7403d4d606b6e074ec5d3baf39d18
    auto expected_ct = hex_to_bytes("cea7403d4d606b6e074ec5d3baf39d18");
    CHECK(result.ciphertext.size() == 16, "NIST-14: ciphertext size 16");
    CHECK(bytes_equal(result.ciphertext.data(), expected_ct.data(), 16),
          "NIST-14: ciphertext matches");

    // Expected Tag: d0d1c8a799996bf0265b98b5d48ab919
    auto expected_tag = hex_to_bytes("d0d1c8a799996bf0265b98b5d48ab919");
    CHECK(bytes_equal(result.tag.data(), expected_tag.data(), 16),
          "NIST-14: tag matches");

    // (AR) فك التشفير يجب أن يُعيد النص الأصلي (أصفار)
    auto dec = aes_gcm_decrypt(key, result.ciphertext.data(), result.ciphertext.size(),
                               iv, result.tag.data(), nullptr, 0);
    CHECK(dec.size() == 16, "NIST-14: decrypted size 16");
    CHECK(bytes_equal(dec.data(), pt, 16), "NIST-14: decrypted matches plaintext");
}

// ─── NIST GCM Test Case 15 ──────────────────────────────────────
// (AR) AES-256-GCM: مفتاح غير صفري، نص طويل (60 بايت)، بدون AAD
void test_nist_gcm_case15()
{
    using namespace sad::net::crypto;
    auto key_v = hex_to_bytes("feffe9928665731c6d6a8f9467308308"
                              "feffe9928665731c6d6a8f9467308308");
    auto iv_v = hex_to_bytes("cafebabefacedbaddecaf888");
    auto pt_v = hex_to_bytes("d9313225f88406e5a55909c5aff5269a"
                             "86a7a9531534f7da2e4c303d8a318a72"
                             "1c3c0c95956809532fcf0e2449a6b525"
                             "b16aedf5aa0de657ba637b391aafd255");

    auto result = aes_gcm_encrypt_with_nonce(
        key_v.data(), iv_v.data(),
        pt_v.data(), pt_v.size(),
        nullptr, 0);

    // Expected CT
    auto expected_ct = hex_to_bytes("522dc1f099567d07f47f37a32a84427d"
                                    "643a8cdcbfe5c0c97598a2bd2555d1aa"
                                    "8cb08e48590dbb3da7b08b1056828838"
                                    "c5f61e6393ba7a0abcc9f662898015ad");
    CHECK(result.ciphertext.size() == expected_ct.size(), "NIST-15: ciphertext size");
    CHECK(bytes_equal(result.ciphertext.data(), expected_ct.data(), expected_ct.size()),
          "NIST-15: ciphertext matches");

    // Expected Tag: b094dac5d93471bdec1a502270e3cc6c
    auto expected_tag = hex_to_bytes("b094dac5d93471bdec1a502270e3cc6c");
    CHECK(bytes_equal(result.tag.data(), expected_tag.data(), 16),
          "NIST-15: tag matches");

    // (AR) فك التشفير
    auto dec = aes_gcm_decrypt(key_v.data(),
                               result.ciphertext.data(), result.ciphertext.size(),
                               iv_v.data(), result.tag.data(),
                               nullptr, 0);
    CHECK(dec.size() == pt_v.size(), "NIST-15: decrypted size");
    CHECK(bytes_equal(dec.data(), pt_v.data(), pt_v.size()),
          "NIST-15: decrypted matches plaintext");
}

// ─── NIST GCM Test Case 16 ──────────────────────────────────────
// (AR) AES-256-GCM: مفتاح غير صفري، نص + AAD (بيانات إضافية للمصادقة)
void test_nist_gcm_case16()
{
    using namespace sad::net::crypto;
    auto key_v = hex_to_bytes("feffe9928665731c6d6a8f9467308308"
                              "feffe9928665731c6d6a8f9467308308");
    auto iv_v = hex_to_bytes("cafebabefacedbaddecaf888");
    auto pt_v = hex_to_bytes("d9313225f88406e5a55909c5aff5269a"
                             "86a7a9531534f7da2e4c303d8a318a72"
                             "1c3c0c95956809532fcf0e2449a6b525"
                             "b16aedf5aa0de657ba637b39");
    auto aad_v = hex_to_bytes("feedfacedeadbeeffeedfacedeadbeef"
                              "abaddad2");

    auto result = aes_gcm_encrypt_with_nonce(
        key_v.data(), iv_v.data(),
        pt_v.data(), pt_v.size(),
        aad_v.data(), aad_v.size());

    // Expected CT
    auto expected_ct = hex_to_bytes("522dc1f099567d07f47f37a32a84427d"
                                    "643a8cdcbfe5c0c97598a2bd2555d1aa"
                                    "8cb08e48590dbb3da7b08b1056828838"
                                    "c5f61e6393ba7a0abcc9f662");
    CHECK(result.ciphertext.size() == expected_ct.size(), "NIST-16: ciphertext size");
    CHECK(bytes_equal(result.ciphertext.data(), expected_ct.data(), expected_ct.size()),
          "NIST-16: ciphertext matches");

    // Expected Tag: 76fc6ece0f4e1768cddf8853bb2d551b
    auto expected_tag = hex_to_bytes("76fc6ece0f4e1768cddf8853bb2d551b");
    CHECK(bytes_equal(result.tag.data(), expected_tag.data(), 16),
          "NIST-16: tag matches");

    // (AR) فك التشفير مع AAD
    auto dec = aes_gcm_decrypt(key_v.data(),
                               result.ciphertext.data(), result.ciphertext.size(),
                               iv_v.data(), result.tag.data(),
                               aad_v.data(), aad_v.size());
    CHECK(dec.size() == pt_v.size(), "NIST-16: decrypted size");
    CHECK(bytes_equal(dec.data(), pt_v.data(), pt_v.size()),
          "NIST-16: decrypted matches plaintext");

    // (AR) التحقق من كشف العبث بـ AAD
    auto tampered_aad = aad_v;
    tampered_aad[0] ^= 0x01;
    auto dec_bad = aes_gcm_decrypt(key_v.data(),
                                   result.ciphertext.data(), result.ciphertext.size(),
                                   iv_v.data(), result.tag.data(),
                                   tampered_aad.data(), tampered_aad.size());
    CHECK(dec_bad.empty(), "NIST-16: tampered AAD detected");
}

// ═══════════════════════════════════════════════════════════════════
// القسم: متجهات اختبار Wycheproof — AES-256-GCM (ivSize=96, keySize=256, tagSize=128)
// (AR) مشروع Wycheproof من Google: يختبر حالات حدّية متنوعة لأحجام مختلفة
//      تشمل: بدون نص، بايت واحد، 8/15/16/17/24 بايت، AAD فقط، AAD+نص، حالات خاصة
//      المصدر: https://github.com/C2SP/wycheproof/blob/master/testvectors_v1/aes_gcm_test.json
// ═══════════════════════════════════════════════════════════════════

// (AR) دالة مساعدة: تنفيذ اختبار Wycheproof واحد — تشفير + مطابقة CT و Tag + فك تشفير
static void wycheproof_gcm_test(const char *label, int tcId,
                                const char *key_hex, const char *iv_hex,
                                const char *aad_hex, const char *msg_hex,
                                const char *ct_hex, const char *tag_hex)
{
    using namespace sad::net::crypto;
    auto key_v = hex_to_bytes(key_hex);
    auto iv_v = hex_to_bytes(iv_hex);
    auto aad_v = hex_to_bytes(aad_hex);
    auto pt_v = hex_to_bytes(msg_hex);
    auto expected_ct = hex_to_bytes(ct_hex);
    auto expected_tag = hex_to_bytes(tag_hex);

    auto result = aes_gcm_encrypt_with_nonce(
        key_v.data(), iv_v.data(),
        pt_v.empty() ? nullptr : pt_v.data(), pt_v.size(),
        aad_v.empty() ? nullptr : aad_v.data(), aad_v.size());

    // (AR) التحقق من حجم النص المُشفّر
    std::string id_str = "WP-" + std::to_string(tcId);
    CHECK(result.ciphertext.size() == expected_ct.size(),
          (id_str + ": CT size").c_str());

    // (AR) التحقق من محتوى النص المُشفّر
    if (!expected_ct.empty())
    {
        CHECK(bytes_equal(result.ciphertext.data(), expected_ct.data(), expected_ct.size()),
              (id_str + ": CT matches").c_str());
    }

    // (AR) التحقق من وسم المصادقة (128 بت)
    CHECK(bytes_equal(result.tag.data(), expected_tag.data(), 16),
          (id_str + ": tag matches").c_str());

    // (AR) فك التشفير — التحقق من استرجاع النص الأصلي
    auto dec = aes_gcm_decrypt(key_v.data(),
                               result.ciphertext.data(), result.ciphertext.size(),
                               iv_v.data(), result.tag.data(),
                               aad_v.empty() ? nullptr : aad_v.data(), aad_v.size());
    CHECK(dec.size() == pt_v.size(), (id_str + ": dec size").c_str());
    if (!pt_v.empty())
    {
        CHECK(bytes_equal(dec.data(), pt_v.data(), pt_v.size()),
              (id_str + ": dec matches PT").c_str());
    }
}

// ─── Wycheproof: tcId 93 — بدون نص، بدون AAD ────────────────────
void test_wycheproof_93()
{
    wycheproof_gcm_test("empty msg+aad", 93,
                        "80ba3192c803ce965ea371d5ff073cf0f43b6a2ab576b208426e11409c09b9b0",
                        "4da5bf8dfd5852c1ea12379d",
                        "", "", "",
                        "4771a7c404a472966cea8f73c8bfe17a");
}

// ─── Wycheproof: tcId 94 — بايت واحد ─────────────────────────────
void test_wycheproof_94()
{
    wycheproof_gcm_test("1-byte msg", 94,
                        "cc56b680552eb75008f5484b4cb803fa5063ebd6eab91f6ab6aef4916a766273",
                        "99e23ec48985bccdeeab60f1",
                        "", "2a", "06",
                        "633c1e9703ef744ffffb40edf9d14355");
}

// ─── Wycheproof: tcId 95 — 8 بايت ────────────────────────────────
void test_wycheproof_95()
{
    wycheproof_gcm_test("8-byte msg", 95,
                        "51e4bf2bad92b7aff1a4bc05550ba81df4b96fabf41c12c7b00e60e48db7e152",
                        "4f07afedfdc3b6c2361823d3",
                        "", "be3308f72a2c6aed", "cf332a12fdee800b",
                        "602e8d7c4799d62c140c9bb834876b09");
}

// ─── Wycheproof: tcId 96 — 15 بايت (أقل من كتلة AES بواحد) ──────
void test_wycheproof_96()
{
    wycheproof_gcm_test("15-byte msg", 96,
                        "67119627bd988eda906219e08c0d0d779a07d208ce8a4fe0709af755eeec6dcb",
                        "68ab7fdbf61901dad461d23c",
                        "", "51f8c1f731ea14acdb210a6d973e07",
                        "43fc101bff4b32bfadd3daf57a590e",
                        "ec04aacb7148a8b8be44cb7eaf4efa69");
}

// ─── Wycheproof: tcId 97 — 16 بايت (كتلة AES كاملة) ─────────────
void test_wycheproof_97()
{
    wycheproof_gcm_test("16-byte msg (1 block)", 97,
                        "59d4eafb4de0cfc7d3db99a8f54b15d7b39f0acc8da69763b019c1699f87674a",
                        "2fcb1b38a99e71b84740ad9b",
                        "", "549b365af913f3b081131ccb6b825588",
                        "f58c16690122d75356907fd96b570fca",
                        "28752c20153092818faba2a334640d6e");
}

// ─── Wycheproof: tcId 98 — 17 بايت (أكبر من كتلة AES بواحد) ─────
void test_wycheproof_98()
{
    wycheproof_gcm_test("17-byte msg", 98,
                        "3b2458d8176e1621c0cc24c0c0e24c1e80d72f7ee9149a4b166176629616d011",
                        "45aaa3e5d16d2d42dc03445d",
                        "", "3ff1514b1c503915918f0c0c31094a6e1f",
                        "73a6b6f45f6ccc5131e07f2caa1f2e2f56",
                        "2d7379ec1db5952d4e95d30c340b1b1d");
}

// ─── Wycheproof: tcId 99 — 24 بايت (كتلة ونصف) ──────────────────
void test_wycheproof_99()
{
    wycheproof_gcm_test("24-byte msg", 99,
                        "0212a8de5007ed87b33f1a7090b6114f9e08cefd9607f2c276bdcfdbc5ce9cd7",
                        "e6b1adf2fd58a8762c65f31b",
                        "", "10f1ecf9c60584665d9ae5efe279e7f7377eea6916d2b111",
                        "0843fff52d934fc7a071ea62c0bd351ce85678cde3ea2c9e",
                        "7355fde599006715053813ce696237a8");
}

// ─── Wycheproof: tcId 91 — 10 بايت نص + 8 بايت AAD ──────────────
void test_wycheproof_91()
{
    wycheproof_gcm_test("10B msg + 8B AAD", 91,
                        "92ace3e348cd821092cd921aa3546374299ab46209691bc28b8752d17f123c20",
                        "00112233445566778899aabb",
                        "00000000ffffffff",
                        "00010203040506070809",
                        "e27abdd2d2a53d2f136b",
                        "9a4a2579529301bcfb71c78d4060f52c");
}

// ─── Wycheproof: tcId 92 — AAD فقط بدون نص ───────────────────────
void test_wycheproof_92()
{
    wycheproof_gcm_test("AAD only, no msg", 92,
                        "29d3a44f8723dc640239100c365423a312934ac80239212ac3df3421a2098123",
                        "00112233445566778899aabb",
                        "aabbccddeeff",
                        "", "",
                        "2a7d77fa526b8250cb296078926b5020");
}

// ─── Wycheproof: tcId 100 — 20 بايت نص + 1 بايت AAD ─────────────
void test_wycheproof_100()
{
    wycheproof_gcm_test("20B msg + 1B AAD", 100,
                        "b279f57e19c8f53f2f963f5f2519fdb7c1779be2ca2b3ae8e1128b7d6c627fc4",
                        "98bc2c7438d5cd7665d76f6e",
                        "c0",
                        "fcc515b294408c8645c9183e3f4ecee5127846d1",
                        "eb5500e3825952866d911253f8de860c00831c81",
                        "ecb660e1fb0541ec41e8d68a64141b3a");
}

// ─── Wycheproof: tcId 101 — 20 بايت نص + 16 بايت AAD ────────────
void test_wycheproof_101()
{
    wycheproof_gcm_test("20B msg + 16B AAD", 101,
                        "f32364b1d339d82e4f132d8f4a0ec1ff7e746517fa07ef1a7f422f4e25a48194",
                        "5a86a50a0e8a179c734b996d",
                        "ab2ac7c44c60bdf8228c7884adb20184",
                        "43891bccb522b1e72a6b53cf31c074e9d6c2df8e",
                        "43dda832e942e286da314daa99bef5071d9d2c78",
                        "c3922583476ced575404ddb85dd8cd44");
}

// ─── Wycheproof: tcId 128 — حالة خاصة: Tag = 0xFF×16 ─────────────
void test_wycheproof_128()
{
    wycheproof_gcm_test("special: tag=all-ones", 128,
                        "00112233445566778899aabbccddeeff102132435465768798a9bacbdcedfe0f",
                        "000000000000000000000000",
                        "",
                        "561008fa07a68f5c61285cd013464eaf",
                        "23293e9b07ca7d1b0cae7cc489a973b3",
                        "ffffffffffffffffffffffffffffffff");
}

// ═══════════════════════════════════════════════════════════════════
// القسم: اختبارات Wycheproof — كشف العبث (Modified Tag / Modified CT / Modified AAD)
// (AR) هذه الاختبارات تتحقق أن فك التشفير يرفض النصوص المُعبَث بها.
//      المصدر: Wycheproof tcId 129 (صحيح) + tcId 134-149 (وسم مُعدّل)
//      والقياس المرجعي هو: key=000102...1f, iv=505152...5b, msg=202122...2f
//      CT المتوقع = b2061457c0759fc1749f174ee1ccadfa
//      Tag الصحيح = 9ce8fef6d8ab1bf1bf887232eab590dd
// ═══════════════════════════════════════════════════════════════════

// (AR) دالة مساعدة: تنفيذ اختبار رفض عبث — يجب أن يرجع فارغاً
static void wycheproof_reject_test(const char *label, int tcId,
                                   const char *key_hex, const char *iv_hex,
                                   const char *aad_hex,
                                   const char *ct_hex, const char *tag_hex)
{
    using namespace sad::net::crypto;
    auto key_v = hex_to_bytes(key_hex);
    auto iv_v = hex_to_bytes(iv_hex);
    auto aad_v = hex_to_bytes(aad_hex);
    auto ct_v = hex_to_bytes(ct_hex);
    auto tag_v = hex_to_bytes(tag_hex);

    auto dec = aes_gcm_decrypt(key_v.data(),
                               ct_v.data(), ct_v.size(),
                               iv_v.data(), tag_v.data(),
                               aad_v.empty() ? nullptr : aad_v.data(), aad_v.size());

    std::string id_str = "WP-" + std::to_string(tcId);
    CHECK(dec.empty(), (id_str + ": reject tampered").c_str());
}

// ─── المتجه المرجعي الصحيح (tcId 129) ───────────────────────────
// key  = 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f
// iv   = 505152535455565758595a5b
// aad  = (empty)
// msg  = 202122232425262728292a2b2c2d2e2f
// ct   = b2061457c0759fc1749f174ee1ccadfa
// tag  = 9ce8fef6d8ab1bf1bf887232eab590dd

// المشترك بين جميع اختبارات العبث
#define WP_TAMPER_KEY "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
#define WP_TAMPER_IV "505152535455565758595a5b"
#define WP_TAMPER_CT "b2061457c0759fc1749f174ee1ccadfa"

// ─── tcId 129: المرجع الصحيح — يجب أن يُفك بنجاح ────────────────
void test_wycheproof_129_valid()
{
    wycheproof_gcm_test("reference valid", 129,
                        WP_TAMPER_KEY, WP_TAMPER_IV, "",
                        "202122232425262728292a2b2c2d2e2f",
                        WP_TAMPER_CT,
                        "9ce8fef6d8ab1bf1bf887232eab590dd");
}

// ─── tcId 134: Flipped bit 0 in tag — يجب الرفض ──────────────────
void test_wycheproof_134()
{
    wycheproof_reject_test("flipped tag bit 0", 134,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           WP_TAMPER_CT,
                           "9de8fef6d8ab1bf1bf887232eab590dd");
}

// ─── tcId 135: Flipped bit 32 in tag — يجب الرفض ─────────────────
void test_wycheproof_135()
{
    wycheproof_reject_test("flipped tag bit 32", 135,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           WP_TAMPER_CT,
                           "9ce8fe76d8ab1bf1bf887232eab590dd");
}

// ─── tcId 140: Flipped bit 77 in tag — يجب الرفض ─────────────────
void test_wycheproof_140()
{
    wycheproof_reject_test("flipped tag bit 77", 140,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           WP_TAMPER_CT,
                           "9ce8fef6d8ab1bf13f887232eab590dd");
}

// ─── tcId 147: Flipped bit 126 in tag — يجب الرفض ────────────────
void test_wycheproof_147()
{
    wycheproof_reject_test("flipped tag bit 126", 147,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           WP_TAMPER_CT,
                           "9ce8fef6d8ab1bf1bf887232eab5909d");
}

// ─── tcId 148: Flipped bit 127 in tag — يجب الرفض ────────────────
void test_wycheproof_148()
{
    wycheproof_reject_test("flipped tag bit 127", 148,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           WP_TAMPER_CT,
                           "9ce8fef6d8ab1bf1bf887232eab5905d");
}

// ─── tcId 149: Flipped bits 0+64 in tag — يجب الرفض ──────────────
void test_wycheproof_149()
{
    wycheproof_reject_test("flipped tag bits 0+64", 149,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           WP_TAMPER_CT,
                           "9de9fff7d9aa1af0be897333ebb491dc");
}

// ─── اختبار إضافي: عبث ببايت واحد في CT ─────────────────────────
// (AR) CT الحقيقي b2061457... نقلب البايت الأول → a2061457...
void test_wycheproof_ct_tamper()
{
    wycheproof_reject_test("CT byte 0 flipped", 9001,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           "a2061457c0759fc1749f174ee1ccadfa",  // بايت أول مقلوب
                           "9ce8fef6d8ab1bf1bf887232eab590dd"); // tag الصحيح
}

// ─── اختبار إضافي: عبث ببايت أخير في CT ─────────────────────────
// (AR) CT الحقيقي ...ccadfa → ...ccadfb
void test_wycheproof_ct_tamper_last()
{
    wycheproof_reject_test("CT last byte flipped", 9002,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           "b2061457c0759fc1749f174ee1ccadfb", // بايت أخير مقلوب
                           "9ce8fef6d8ab1bf1bf887232eab590dd");
}

// ─── اختبار إضافي: AAD مُعبث عليه ───────────────────────────────
// (AR) نستخدم متجه WP-91 (يحتوي AAD) ونقلب بايت في AAD
void test_wycheproof_aad_tamper()
{
    using namespace sad::net::crypto;
    auto key_v = hex_to_bytes("92ace3e348cd821092cd921aa3546374299ab46209691bc28b8752d17f123c20");
    auto iv_v = hex_to_bytes("00112233445566778899aabb");
    auto ct_v = hex_to_bytes("e27abdd2d2a53d2f136b");
    auto tag_v = hex_to_bytes("9a4a2579529301bcfb71c78d4060f52c");

    // AAD الأصلي: 00000000ffffffff → نقلب بايت: 01000000ffffffff
    auto aad_tampered = hex_to_bytes("01000000ffffffff");

    auto dec = aes_gcm_decrypt(key_v.data(),
                               ct_v.data(), ct_v.size(),
                               iv_v.data(), tag_v.data(),
                               aad_tampered.data(), aad_tampered.size());

    CHECK(dec.empty(), "WP-91-tampered-AAD: reject");

    // (AR) أيضاً تحقق: AAD بحجم مختلف يُرفض
    auto aad_short = hex_to_bytes("00000000ffffff"); // 7 بايت بدل 8
    auto dec2 = aes_gcm_decrypt(key_v.data(),
                                ct_v.data(), ct_v.size(),
                                iv_v.data(), tag_v.data(),
                                aad_short.data(), aad_short.size());
    CHECK(dec2.empty(), "WP-91-short-AAD: reject");
}

// ─── اختبار: tag مبدّل بالكامل ───────────────────────────────────
void test_wycheproof_tag_zeroed()
{
    wycheproof_reject_test("zeroed tag", 9003,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           WP_TAMPER_CT,
                           "00000000000000000000000000000000");
}

// ─── اختبار: tag مقلوب بالكامل (bitwise NOT) ─────────────────────
void test_wycheproof_tag_inverted()
{
    wycheproof_reject_test("inverted tag", 9004,
                           WP_TAMPER_KEY, WP_TAMPER_IV, "",
                           WP_TAMPER_CT,
                           "63170109275ae40e40778dcd154a6f22"); // ~(9ce8fef6d8ab1bf1bf887232eab590dd)
}

// ═══════════════════════════════════════════════════════════════════
// القسم: اختبارات الأداء (Benchmark)
// (AR) قياس عدد العمليات في الثانية لكل خوارزمية
//      كل اختبار يعمل لمدة ثانية واحدة على الأقل ثم يطبع النتيجة
//      هذه ليست اختبارات صحة — بل قياس أداء (تُسجل دائماً كنجاح)
// ═══════════════════════════════════════════════════════════════════

// (AR) قياس عمليات توليد مفاتيح X25519 في الثانية
void test_bench_x25519_keygen()
{
    using namespace sad::net::crypto;
    using clk = std::chrono::high_resolution_clock;

    int ops = 0;
    auto start = clk::now();
    // (AR) نعمل لمدة ثانية واحدة على الأقل
    while (true)
    {
        auto kp = x25519_generate_keypair();
        (void)kp; // منع التحسين
        ops++;
        if (ops % 100 == 0)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                               clk::now() - start)
                               .count();
            if (elapsed >= 1000)
                break;
        }
    }
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        clk::now() - start)
                        .count();
    double ops_per_sec = (double)ops / ((double)total_ms / 1000.0);
    std::cout << "  X25519 keypair: " << (int)ops_per_sec
              << " ops/sec (" << ops << " in " << total_ms << "ms)" << std::endl;
    CHECK(ops > 0, "Benchmark X25519 keygen completed");
}

// (AR) قياس عمليات تبادل المفاتيح (DH shared secret) في الثانية
void test_bench_x25519_dh()
{
    using namespace sad::net::crypto;
    using clk = std::chrono::high_resolution_clock;

    // (AR) نولّد زوجين ثابتين لقياس DH فقط
    auto alice = x25519_generate_keypair();
    auto bob = x25519_generate_keypair();

    int ops = 0;
    auto start = clk::now();
    while (true)
    {
        auto shared = x25519_shared_secret(alice.private_key, bob.public_key);
        (void)shared;
        ops++;
        if (ops % 100 == 0)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                               clk::now() - start)
                               .count();
            if (elapsed >= 1000)
                break;
        }
    }
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        clk::now() - start)
                        .count();
    double ops_per_sec = (double)ops / ((double)total_ms / 1000.0);
    std::cout << "  X25519 DH: " << (int)ops_per_sec
              << " ops/sec (" << ops << " in " << total_ms << "ms)" << std::endl;
    CHECK(ops > 0, "Benchmark X25519 DH completed");
}

// (AR) قياس إنتاجية SHA-256 بالميجابايت/ثانية
void test_bench_sha256_throughput()
{
    using namespace sad::net::crypto;
    using clk = std::chrono::high_resolution_clock;

    // (AR) كتلة بيانات 4 كيلوبايت
    const size_t block_size = 4096;
    std::vector<uint8_t> data(block_size, 0xAB);

    int ops = 0;
    auto start = clk::now();
    while (true)
    {
        auto h = sha256(data.data(), data.size());
        (void)h;
        ops++;
        if (ops % 500 == 0)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                               clk::now() - start)
                               .count();
            if (elapsed >= 1000)
                break;
        }
    }
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        clk::now() - start)
                        .count();
    double mb_per_sec = ((double)ops * block_size) / (1024.0 * 1024.0) / ((double)total_ms / 1000.0);
    std::cout << "  SHA-256: " << std::fixed << std::setprecision(1) << mb_per_sec
              << " MB/s (" << ops << " × " << block_size << "B in " << total_ms << "ms)" << std::endl;
    CHECK(ops > 0, "Benchmark SHA-256 completed");
}

// (AR) قياس إنتاجية AES-256-GCM بالميجابايت/ثانية
void test_bench_aes_gcm_throughput()
{
    using namespace sad::net::crypto;
    using clk = std::chrono::high_resolution_clock;

    // (AR) مفتاح ثابت + كتلة 4 كيلوبايت
    uint8_t key[32] = {};
    for (int i = 0; i < 32; i++)
        key[i] = (uint8_t)i;

    const size_t block_size = 4096;
    std::vector<uint8_t> data(block_size, 0xCD);

    int ops = 0;
    auto start = clk::now();
    while (true)
    {
        auto enc = aes_gcm_encrypt(key, data.data(), data.size(), nullptr, 0);
        (void)enc;
        ops++;
        if (ops % 500 == 0)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                               clk::now() - start)
                               .count();
            if (elapsed >= 1000)
                break;
        }
    }
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        clk::now() - start)
                        .count();
    double mb_per_sec = ((double)ops * block_size) / (1024.0 * 1024.0) / ((double)total_ms / 1000.0);
    std::cout << "  AES-GCM: " << std::fixed << std::setprecision(1) << mb_per_sec
              << " MB/s (" << ops << " × " << block_size << "B in " << total_ms << "ms)" << std::endl;
    CHECK(ops > 0, "Benchmark AES-GCM completed");
}

// (AR) قياس رسائل Double Ratchet في الثانية (تشفير + فك تشفير)
void test_bench_double_ratchet()
{
    using namespace sad::net::crypto;
    using clk = std::chrono::high_resolution_clock;

    // (AR) إنشاء جلسة مرسل/مستقبل
    auto alice_kp = x25519_generate_keypair();
    auto bob_kp = x25519_generate_keypair();
    auto shared = x25519_shared_secret(alice_kp.private_key, bob_kp.public_key);

    DoubleRatchetSession sender, receiver;
    sender.init_sender(shared, bob_kp.public_key);
    receiver.init_receiver(shared, bob_kp);

    const uint8_t msg[] = "benchmark test message - 32 bytes";
    const size_t msg_len = 33;

    int ops = 0;
    auto start = clk::now();
    while (true)
    {
        auto pkt = sender.encrypt(msg, msg_len);
        auto dec = receiver.decrypt(pkt);
        (void)dec;
        ops++;
        if (ops % 100 == 0)
        {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                               clk::now() - start)
                               .count();
            if (elapsed >= 1000)
                break;
        }
    }
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        clk::now() - start)
                        .count();
    double msgs_per_sec = (double)ops / ((double)total_ms / 1000.0);
    std::cout << "  Double Ratchet: " << (int)msgs_per_sec
              << " msg/sec (" << ops << " in " << total_ms << "ms)" << std::endl;
    CHECK(ops > 0, "Benchmark Double Ratchet completed");
}

// ═══════════════════════════════════════════════════════════════════
// تسجيل جميع الاختبارات
// ═══════════════════════════════════════════════════════════════════

void register_all_tests()
{
    // --- Buffer ---
    test::register_test("Buffer: أساسيات", test_buffer_basic);
    test::register_test("Buffer: عمليات", test_buffer_operations);
    test::register_test("Buffer: تحويل hex", test_buffer_hex);
    test::register_test("Buffer: مسح آمن", test_buffer_secure_clear);

    // --- NodeId ---
    test::register_test("NodeId: أساسيات", test_nodeid_basic);
    test::register_test("NodeId: تحويل hex", test_nodeid_hex);
    test::register_test("NodeId: مسافة XOR", test_nodeid_xor_distance);
    test::register_test("NodeId: أصفار قيادية", test_nodeid_leading_zeros);

    // --- Endpoint ---
    test::register_test("Endpoint: IPv4/IPv6", test_endpoint);

    // --- Result ---
    test::register_test("Result: success/fail", test_result);

    // --- SHA-256 ---
    test::register_test("SHA-256: قيم معيارية", test_sha256_basic);
    test::register_test("SHA-256: حتمية", test_sha256_deterministic);
    test::register_test("SHA-256: Buffer overload", test_sha256_buffer_overload);

    // --- HMAC-SHA256 ---
    test::register_test("HMAC-SHA256: حتمية وتفرد", test_hmac_sha256);

    // --- HKDF-SHA256 ---
    test::register_test("HKDF-SHA256: اشتقاق مفاتيح", test_hkdf_sha256);

    // --- AES-256-GCM ---
    test::register_test("AES-GCM: تشفير/فك تشفير", test_aes_gcm_encrypt_decrypt);
    test::register_test("AES-GCM: مع AAD", test_aes_gcm_with_aad);
    test::register_test("AES-GCM: كشف العبث", test_aes_gcm_tamper_detection);
    test::register_test("AES-GCM: مفتاح خاطئ", test_aes_gcm_wrong_key);

    // --- X25519 ---
    test::register_test("X25519: توليد مفاتيح", test_x25519_keygen);
    test::register_test("X25519: Diffie-Hellman", test_x25519_shared_secret);
    test::register_test("X25519: RFC 7748 vector", test_x25519_rfc7748_vector);

    // --- CSPRNG ---
    test::register_test("CSPRNG: random_bytes", test_random_bytes);

    // --- Double Ratchet ---
    test::register_test("Double Ratchet: جلسة واحدة", test_double_ratchet);
    test::register_test("Double Ratchet: رسائل متعددة", test_double_ratchet_multiple_messages);

    // --- DHT Routing Table ---
    test::register_test("DHT: جدول التوجيه", test_dht_routing_table);
    test::register_test("DHT: إزالة عقدة", test_dht_routing_table_remove);

    // --- Error Codes ---
    test::register_test("أكواد الأخطاء: ترجمة", test_error_codes);

    // --- Timestamp ---
    test::register_test("الطابع الزمني", test_timestamp);

    // --- NIST AES-256-GCM ---
    test::register_test("NIST GCM Case 13: key=0 IV=0 no PT", test_nist_gcm_case13);
    test::register_test("NIST GCM Case 14: key=0 IV=0 PT=0", test_nist_gcm_case14);
    test::register_test("NIST GCM Case 15: 64B PT no AAD", test_nist_gcm_case15);
    test::register_test("NIST GCM Case 16: 60B PT + AAD", test_nist_gcm_case16);

    // --- Wycheproof AES-256-GCM (ivSize=96, keySize=256, tagSize=128) ---
    test::register_test("WP-93: empty msg+aad", test_wycheproof_93);
    test::register_test("WP-94: 1-byte msg", test_wycheproof_94);
    test::register_test("WP-95: 8-byte msg", test_wycheproof_95);
    test::register_test("WP-96: 15-byte msg", test_wycheproof_96);
    test::register_test("WP-97: 16-byte msg (1 block)", test_wycheproof_97);
    test::register_test("WP-98: 17-byte msg", test_wycheproof_98);
    test::register_test("WP-99: 24-byte msg", test_wycheproof_99);
    test::register_test("WP-91: 10B msg + 8B AAD", test_wycheproof_91);
    test::register_test("WP-92: AAD only, no msg", test_wycheproof_92);
    test::register_test("WP-100: 20B msg + 1B AAD", test_wycheproof_100);
    test::register_test("WP-101: 20B msg + 16B AAD", test_wycheproof_101);
    test::register_test("WP-128: special tag=all-ones", test_wycheproof_128);

    // --- Wycheproof: كشف العبث (Modified Tag / CT / AAD) ---
    test::register_test("WP-129: reference valid vector", test_wycheproof_129_valid);
    test::register_test("WP-134: reject flipped tag bit 0", test_wycheproof_134);
    test::register_test("WP-135: reject flipped tag bit 32", test_wycheproof_135);
    test::register_test("WP-140: reject flipped tag bit 77", test_wycheproof_140);
    test::register_test("WP-147: reject flipped tag bit 126", test_wycheproof_147);
    test::register_test("WP-148: reject flipped tag bit 127", test_wycheproof_148);
    test::register_test("WP-149: reject flipped tag bits 0+64", test_wycheproof_149);
    test::register_test("WP-9001: reject CT byte 0 flipped", test_wycheproof_ct_tamper);
    test::register_test("WP-9002: reject CT last byte flipped", test_wycheproof_ct_tamper_last);
    test::register_test("WP-AAD: reject tampered/short AAD", test_wycheproof_aad_tamper);
    test::register_test("WP-9003: reject zeroed tag", test_wycheproof_tag_zeroed);
    test::register_test("WP-9004: reject inverted tag", test_wycheproof_tag_inverted);

    // --- Benchmark ---
    test::register_test("Benchmark: X25519 keypair/s", test_bench_x25519_keygen);
    test::register_test("Benchmark: X25519 DH/s", test_bench_x25519_dh);
    test::register_test("Benchmark: SHA-256 MB/s", test_bench_sha256_throughput);
    test::register_test("Benchmark: AES-GCM MB/s", test_bench_aes_gcm_throughput);
    test::register_test("Benchmark: Double Ratchet msg/s", test_bench_double_ratchet);
}

// ═══════════════════════════════════════════════════════════════════
// نقطة الدخول
// ═══════════════════════════════════════════════════════════════════

int main()
{
    std::cout << "═══════════════════════════════════════" << std::endl;
    std::cout << "  اختبارات وحدة صNet" << std::endl;
    std::cout << "═══════════════════════════════════════" << std::endl;

    register_all_tests();
    return test::run_all();
}
