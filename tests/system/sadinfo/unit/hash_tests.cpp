// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// tests/sadinfo/unit/hash_tests.cpp
// (AR) S-004 — اختبارات وحدة لاستراتيجية التجزئة الطبقية.
//
//      التغطية:
//        1) sha256_hex على متجهات RFC 6234 (تأكيد صحة SHA-256).
//        2) fast_hash لملفات بأحجام مختلفة (0, 1B, <4KB, =4KB, >4KB).
//        3) full_hash لملفات بأحجام مختلفة (يتجاوز عتبة mmap عند 64KB).
//        4) content_equals: متطابق، مختلف، أحدهما مفقود.
//        5) ملف غير موجود → fast_hash=0, full_hash="".
//
//      كل اختبار يطبع OK/FAIL ويزيد عدّاد. الإرجاع 0 عند نجاح الكل.
//      لا اعتماد على إطار اختبار خارجي — قائم بذاته.
// (EN) S-004 — Unit tests for tiered hash strategy.
// ═══════════════════════════════════════════════════════════════════════════════

#include "hash_strategy.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <vector>

namespace fs = std::filesystem;
using sad::sadinfo::hash_strategy::content_equals;
using sad::sadinfo::hash_strategy::fast_hash;
using sad::sadinfo::hash_strategy::full_hash;
using sad::sadinfo::hash_strategy::kFastHashError;
using sad::sadinfo::hash_strategy::sha256_hex;

// ─────────────────────────────────────────────────────────────────────────────
// (AR) عدّادات وأدوات.
// (EN) Counters and helpers.
// ─────────────────────────────────────────────────────────────────────────────
static int g_pass = 0;
static int g_fail = 0;

#define EXPECT_TRUE(cond, label)                         \
    do                                                   \
    {                                                    \
        if (cond)                                        \
        {                                                \
            std::cout << "  [OK]   " << (label) << "\n"; \
            ++g_pass;                                    \
        }                                                \
        else                                             \
        {                                                \
            std::cout << "  [FAIL] " << (label) << "\n"; \
            ++g_fail;                                    \
        }                                                \
    } while (0)

#define EXPECT_EQ(a, b, label)                                            \
    do                                                                    \
    {                                                                     \
        if ((a) == (b))                                                   \
        {                                                                 \
            std::cout << "  [OK]   " << (label) << "\n";                  \
            ++g_pass;                                                     \
        }                                                                 \
        else                                                              \
        {                                                                 \
            std::cout << "  [FAIL] " << (label) << "\n        got:      " \
                      << (a) << "\n        expected: " << (b) << "\n";    \
            ++g_fail;                                                     \
        }                                                                 \
    } while (0)

// ─────────────────────────────────────────────────────────────────────────────
// (AR) make_tmp_file — يكتب محتوى إلى ملف مؤقت ويرجع المسار.
// (EN) make_tmp_file — write content to a temp file and return its path.
// ─────────────────────────────────────────────────────────────────────────────
static std::string make_tmp_file(const std::string &name, const std::vector<unsigned char> &data)
{
    const fs::path dir = fs::temp_directory_path() / "sadinfo_hash_tests";
    fs::create_directories(dir);
    const fs::path p = dir / name;
    std::ofstream ofs(p, std::ios::binary | std::ios::trunc);
    if (!data.empty())
    {
        ofs.write(reinterpret_cast<const char *>(data.data()),
                  static_cast<std::streamsize>(data.size()));
    }
    ofs.close();
    return p.string();
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) test_sha256_vectors — متجهات RFC 6234.
// (EN) test_sha256_vectors — RFC 6234 vectors.
// ─────────────────────────────────────────────────────────────────────────────
static void test_sha256_vectors()
{
    std::cout << "── SHA-256 RFC 6234 vectors ──────────────────────────\n";

    // (AR) متجه 1: سلسلة فارغة.
    {
        const std::string h = sha256_hex(nullptr, 0);
        EXPECT_EQ(h, std::string("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"),
                  "sha256_hex(\"\")");
    }
    // (AR) متجه 2: "abc".
    {
        const unsigned char d[] = {'a', 'b', 'c'};
        const std::string h = sha256_hex(d, 3);
        EXPECT_EQ(h, std::string("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"),
                  "sha256_hex(\"abc\")");
    }
    // (AR) متجه 3: "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq".
    {
        const std::string s = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
        const std::string h = sha256_hex(reinterpret_cast<const unsigned char *>(s.data()), s.size());
        EXPECT_EQ(h, std::string("248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"),
                  "sha256_hex(56-char vector)");
    }
    // (AR) متجه 4: مليون 'a' (الكلاسيكي).
    {
        std::vector<unsigned char> big(1000000, 'a');
        const std::string h = sha256_hex(big.data(), big.size());
        EXPECT_EQ(h, std::string("cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"),
                  "sha256_hex(1,000,000 x 'a')");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) test_fast_hash_sizes — fast_hash لأحجام متنوعة وثبات النتيجة.
// (EN) test_fast_hash_sizes — fast_hash across sizes; stability.
// ─────────────────────────────────────────────────────────────────────────────
static void test_fast_hash_sizes()
{
    std::cout << "── fast_hash sizes ───────────────────────────────────\n";

    const std::string p_empty = make_tmp_file("empty.bin", {});
    const std::string p_1b = make_tmp_file("one.bin", {0x42});
    std::vector<unsigned char> v3k(3 * 1024, 0xA5);
    const std::string p_3k = make_tmp_file("3k.bin", v3k);
    std::vector<unsigned char> v4k(4 * 1024, 0x5A);
    const std::string p_4k = make_tmp_file("4k.bin", v4k);
    std::vector<unsigned char> v5k(5 * 1024, 0x33);
    // (AR) تغيير آخر بايت لاختبار أن fast_hash لا يلاحظه (يقرأ أول 4KB فقط).
    v5k[5 * 1024 - 1] = 0xFF;
    const std::string p_5k = make_tmp_file("5k.bin", v5k);

    const uint64_t h_empty = fast_hash(p_empty);
    const uint64_t h_1b = fast_hash(p_1b);
    const uint64_t h_3k = fast_hash(p_3k);
    const uint64_t h_4k = fast_hash(p_4k);
    const uint64_t h_5k = fast_hash(p_5k);

    EXPECT_TRUE(h_empty != kFastHashError, "fast_hash(empty) != error");
    EXPECT_TRUE((h_empty >> 32) == 0, "fast_hash(empty) size_part == 0");
    EXPECT_TRUE((h_1b >> 32) == 1, "fast_hash(1B) size_part == 1");
    EXPECT_TRUE((h_3k >> 32) == 3072, "fast_hash(3KB) size_part == 3072");
    EXPECT_TRUE((h_4k >> 32) == 4096, "fast_hash(4KB) size_part == 4096");
    EXPECT_TRUE((h_5k >> 32) == 5120, "fast_hash(5KB) size_part == 5120");
    EXPECT_TRUE(h_3k != h_4k, "fast_hash distinct (3KB vs 4KB)");

    // (AR) إعادة الاستدعاء = نفس النتيجة (deterministic).
    EXPECT_EQ(fast_hash(p_4k), h_4k, "fast_hash deterministic");
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) test_full_hash_paths — يغطي مسار streaming ومسار mmap.
// (EN) test_full_hash_paths — covers streaming and mmap paths.
// ─────────────────────────────────────────────────────────────────────────────
static void test_full_hash_paths()
{
    std::cout << "── full_hash streaming + mmap ────────────────────────\n";

    // (AR) ملف فارغ → معروف.
    const std::string p_empty = make_tmp_file("fh_empty.bin", {});
    EXPECT_EQ(full_hash(p_empty),
              std::string("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"),
              "full_hash(empty file)");

    // (AR) ملف "abc".
    const std::string p_abc = make_tmp_file("fh_abc.bin", {'a', 'b', 'c'});
    EXPECT_EQ(full_hash(p_abc),
              std::string("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"),
              "full_hash(\"abc\")");

    // (AR) ملف 100KB يساوي تجزئته تجزئة buffer مكافئ (مسار mmap).
    std::vector<unsigned char> v100k(100 * 1024);
    for (std::size_t i = 0; i < v100k.size(); ++i)
        v100k[i] = static_cast<unsigned char>(i & 0xff);
    const std::string p_100k = make_tmp_file("fh_100k.bin", v100k);
    const std::string h_file = full_hash(p_100k);
    const std::string h_buf = sha256_hex(v100k.data(), v100k.size());
    EXPECT_EQ(h_file, h_buf, "full_hash(100KB mmap) matches sha256_hex(buffer)");

    // (AR) ملف 5KB → مسار streaming (< 64KB).
    std::vector<unsigned char> v5k(5 * 1024);
    for (std::size_t i = 0; i < v5k.size(); ++i)
        v5k[i] = static_cast<unsigned char>((i * 7) & 0xff);
    const std::string p_5k = make_tmp_file("fh_5k.bin", v5k);
    const std::string h_file2 = full_hash(p_5k);
    const std::string h_buf2 = sha256_hex(v5k.data(), v5k.size());
    EXPECT_EQ(h_file2, h_buf2, "full_hash(5KB stream) matches sha256_hex(buffer)");

    // (AR) ملف غير موجود → سلسلة فارغة.
    EXPECT_EQ(full_hash(std::string("__does_not_exist_xyz__.bin")),
              std::string{}, "full_hash(missing) returns empty");
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) test_content_equals — مسار fast→full الكامل.
// (EN) test_content_equals — fast→full short-circuit path.
// ─────────────────────────────────────────────────────────────────────────────
static void test_content_equals()
{
    std::cout << "── content_equals ────────────────────────────────────\n";

    std::vector<unsigned char> data(10 * 1024);
    for (std::size_t i = 0; i < data.size(); ++i)
        data[i] = static_cast<unsigned char>(i & 0xff);

    const std::string a = make_tmp_file("eq_a.bin", data);
    const std::string b = make_tmp_file("eq_b.bin", data);
    EXPECT_TRUE(content_equals(a, b), "content_equals identical 10KB");

    // (AR) فرق في آخر بايت — fast_hash لن يلاحظه (يقرأ 4KB)، لكن full_hash سيلاحظه.
    auto data2 = data;
    data2.back() ^= 0xFF;
    const std::string c = make_tmp_file("eq_c.bin", data2);
    EXPECT_TRUE(!content_equals(a, c), "content_equals diff in tail (full catches)");

    // (AR) أحجام مختلفة — fast يردّ مباشرة.
    auto data3 = data;
    data3.pop_back();
    const std::string d = make_tmp_file("eq_d.bin", data3);
    EXPECT_TRUE(!content_equals(a, d), "content_equals diff sizes");

    // (AR) ملف مفقود → false.
    EXPECT_TRUE(!content_equals(a, std::string("__missing_zzz__.bin")),
                "content_equals missing returns false");
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) test_missing_file — fast_hash لمسار غير موجود.
// (EN) test_missing_file — fast_hash for nonexistent path.
// ─────────────────────────────────────────────────────────────────────────────
static void test_missing_file()
{
    std::cout << "── missing file handling ─────────────────────────────\n";
    const uint64_t h = fast_hash(std::string("__truly_missing_qqq__.bin"));
    EXPECT_EQ(h, kFastHashError, "fast_hash(missing) == kFastHashError");
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) test_64kb_boundary — تغطية حدود عتبة mmap بدقة (BF-06 / CR F8).
//      نتحقق أن full_hash يُنتج نفس النتيجة كـ sha256_hex المباشر عند:
//        64KB-1 (streaming), 64KB (mmap exact), 64KB+1 (mmap), 128KB (mmap كبير).
//      السبب: بُنية الكود تتفرع بناءً على هذا الحد، أي خطأ off-by-one يفسد التجزئة.
// (EN) test_64kb_boundary — exercise mmap threshold precisely (BF-06 / CR F8).
// ─────────────────────────────────────────────────────────────────────────────
static void test_64kb_boundary()
{
    std::cout << "── 64KB threshold boundary ───────────────────────────\n";

    constexpr std::size_t kT = 64 * 1024;
    const std::size_t sizes[] = {kT - 1, kT, kT + 1, kT * 2};
    const char *labels[] = {"64KB-1 (stream)", "64KB exact (mmap)",
                            "64KB+1 (mmap)", "128KB (mmap)"};

    for (int i = 0; i < 4; ++i)
    {
        std::vector<unsigned char> buf(sizes[i]);
        // (AR) نمط حشو محدد لتمييز كل بايت (يكشف أي خطأ في مدى القراءة).
        for (std::size_t j = 0; j < sizes[i]; ++j)
        {
            buf[j] = static_cast<unsigned char>((j * 31 + 7) & 0xFF);
        }
        const std::string p = make_tmp_file(std::string("boundary_") + std::to_string(i) + ".bin", buf);
        const std::string h_via_file = full_hash(p);
        const std::string h_via_mem = sha256_hex(buf.data(), buf.size());
        EXPECT_EQ(h_via_file, h_via_mem, labels[i]);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) test_same_path_shortcut — تحسين CR F5: مسار == مسار → true بدون قراءة كاملة.
// (EN) test_same_path_shortcut — CR F5 optimization: identical paths shortcut.
// ─────────────────────────────────────────────────────────────────────────────
static void test_same_path_shortcut()
{
    std::cout << "── content_equals same-path shortcut (CR F5) ─────────\n";

    std::vector<unsigned char> data(2048, 0x5A);
    const std::string p = make_tmp_file("same_path.bin", data);

    EXPECT_TRUE(content_equals(p, p), "content_equals(p, p) == true");
    EXPECT_TRUE(!content_equals(std::string("__nope_x__.bin"), std::string("__nope_x__.bin")),
                "content_equals(missing, missing) == false");
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) test_directory_handling — CR G4/G8 (Amelia): تمرير مجلد كمسار يجب
//      أن يُرفض بدلاً من إرجاع digest الفراغ كاذباً.
// (EN) test_directory_handling — directories must be rejected, not silently
//      treated as empty files.
// ─────────────────────────────────────────────────────────────────────────────
static void test_directory_handling()
{
    std::cout << "── directory handling (CR G4/G8) ─────────────────────\n";

    const fs::path dir = fs::temp_directory_path() / "sadinfo_hash_tests" / "as_dir";
    std::error_code ec;
    fs::create_directories(dir, ec);
    EXPECT_TRUE(!ec, "create_directories ok");

    const std::string dpath = dir.string();
    EXPECT_EQ(fast_hash(dpath), kFastHashError, "fast_hash(directory) == kFastHashError");
    EXPECT_TRUE(full_hash(dpath).empty(), "full_hash(directory) == \"\"");

    // (AR) ملف حقيقي بجانب المجلد للتأكد أن المنطق ما زال يعمل.
    std::vector<unsigned char> data(16, 0x77);
    const std::string fpath = make_tmp_file("near_dir.bin", data);
    EXPECT_TRUE(fast_hash(fpath) != kFastHashError, "fast_hash(regular file) ok");
    EXPECT_TRUE(!content_equals(dpath, fpath), "content_equals(dir, file) == false");
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) CR Round-4 — اختبار تزامن: 8 خيوط × 100 تجزئة لنفس الملف الكبير.
//      يثبت سلامة `thread_local` في stream_hash + عدم تداخل buffers بين الخيوط.
//      الفشل المتوقع لو كان stack صغيراً: stack overflow؛ لو bug في مشاركة buffer:
//      نتائج متفاوتة (race condition).
// (EN) CR Round-4 — concurrency test: 8 threads × 100 hashes on same large file.
//      Proves thread_local safety + no buffer sharing across threads.
// ─────────────────────────────────────────────────────────────────────────────
static void test_concurrent_hashing()
{
    std::cout << "\n[test_concurrent_hashing] CR Round-4 — thread_local safety\n";

    // (AR) ملف 256KB → يجبر stream_hash على الدوران عدة مرات لكل خيط.
    std::vector<unsigned char> data(256 * 1024);
    for (std::size_t i = 0; i < data.size(); ++i)
    {
        data[i] = static_cast<unsigned char>(i & 0xff);
    }
    const std::string fpath = make_tmp_file("concurrent.bin", data);

    // (AR) احسب التجزئة المرجعية مرة واحدة (single-threaded baseline).
    const std::string expected = full_hash(fpath);
    EXPECT_TRUE(!expected.empty() && expected.size() == 64,
                "baseline hash computed (64 hex chars)");

    constexpr int kThreads = 8;
    constexpr int kIterPerThread = 100;
    std::atomic<int> mismatch_count{0};
    std::atomic<int> success_count{0};

    std::vector<std::thread> workers;
    workers.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t)
    {
        workers.emplace_back([&]()
                             {
            for (int i = 0; i < kIterPerThread; ++i)
            {
                const std::string h = full_hash(fpath);
                if (h == expected)
                {
                    success_count.fetch_add(1, std::memory_order_relaxed);
                }
                else
                {
                    mismatch_count.fetch_add(1, std::memory_order_relaxed);
                }
            } });
    }
    for (auto &w : workers)
    {
        w.join();
    }

    const int total = kThreads * kIterPerThread;
    EXPECT_EQ(success_count.load(), total, "all threads produced identical hashes");
    EXPECT_EQ(mismatch_count.load(), 0, "zero mismatches under concurrent load");

    // (AR) fast_hash تحت تزامن أيضاً (buffer أصغر — 4KB stack).
    const uint64_t expected_fast = fast_hash(fpath);
    std::atomic<int> fast_mismatch{0};
    std::vector<std::thread> fast_workers;
    for (int t = 0; t < kThreads; ++t)
    {
        fast_workers.emplace_back([&]()
                                  {
            for (int i = 0; i < kIterPerThread; ++i)
            {
                if (fast_hash(fpath) != expected_fast)
                {
                    fast_mismatch.fetch_add(1, std::memory_order_relaxed);
                }
            } });
    }
    for (auto &w : fast_workers)
    {
        w.join();
    }
    EXPECT_EQ(fast_mismatch.load(), 0, "fast_hash stable under concurrent load");

    // (AR) content_equals تحت تزامن.
    std::vector<unsigned char> data2 = data;
    const std::string fpath2 = make_tmp_file("concurrent2.bin", data2);
    std::atomic<int> ce_failures{0};
    std::vector<std::thread> ce_workers;
    for (int t = 0; t < kThreads; ++t)
    {
        ce_workers.emplace_back([&]()
                                {
            for (int i = 0; i < kIterPerThread; ++i)
            {
                if (!content_equals(fpath, fpath2))
                {
                    ce_failures.fetch_add(1, std::memory_order_relaxed);
                }
            } });
    }
    for (auto &w : ce_workers)
    {
        w.join();
    }
    EXPECT_EQ(ce_failures.load(), 0, "content_equals stable under concurrent load");
}

int main()
{
    std::cout << "══════════════════════════════════════════════════════\n";
    std::cout << "  sadinfo hash_strategy unit tests (S-004)\n";
    std::cout << "══════════════════════════════════════════════════════\n";

    test_sha256_vectors();
    test_fast_hash_sizes();
    test_full_hash_paths();
    test_content_equals();
    test_missing_file();
    test_64kb_boundary();
    test_same_path_shortcut();
    test_directory_handling();
    test_concurrent_hashing();

    std::cout << "══════════════════════════════════════════════════════\n";
    std::cout << "  passed: " << g_pass << "    failed: " << g_fail << "\n";
    std::cout << "══════════════════════════════════════════════════════\n";

    // (AR) تنظيف المجلد المؤقت (best-effort).
    std::error_code ec;
    fs::remove_all(fs::temp_directory_path() / "sadinfo_hash_tests", ec);

    return (g_fail == 0) ? 0 : 1;
}
