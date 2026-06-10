// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// tests/sadinfo/unit/cache_state_tests.cpp
//
// (AR) S-007 Part A — اختبارات وحدة لـCacheState (serialize/deserialize +
//      atomic write + read). إطار اختبار محلي بسيط (EXPECT_TRUE/EXPECT_EQ)
//      بنفس نمط hash_tests.cpp للحفاظ على الاتساق.
//
//      التغطية:
//        1) to_json روتين ⇒ JSON صحيح وقابل للقراءة.
//        2) roundtrip: from_json(to_json(state)) == state (للحقول الأساسية).
//        3) write_state ⇒ read_state على ملف مؤقت ⇒ نفس الحالة.
//        4) read_state على ملف غير موجود ⇒ FileNotFound (سلوك طبيعي).
//        5) from_json على JSON معطوب ⇒ ParseError.
//        6) SchemaMismatch عند إصدار غير مدعوم.
//        7) write_state على مسار فارغ ⇒ InvalidPath.
//        8) atomic_write: ملف نهائي يبقى صحيحاً حتى لو وُجد ملف tmp قديم.
// (EN) S-007 Part A — Unit tests for CacheState JSON I/O + atomic write.
// ═══════════════════════════════════════════════════════════════════════════════

#include "cache_state.h"
#include "atomic_write.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;
using namespace sad::sadinfo::cache;

// ─────────────────────────────────────────────────────────────────────────────
// (AR) إطار اختبار خفيف.
// ─────────────────────────────────────────────────────────────────────────────
static int g_pass = 0;
static int g_fail = 0;

#define EXPECT_TRUE(cond, msg)                                          \
    do                                                                  \
    {                                                                   \
        if (cond)                                                       \
        {                                                               \
            ++g_pass;                                                   \
        }                                                               \
        else                                                            \
        {                                                               \
            ++g_fail;                                                   \
            std::cerr << "  ✗ FAIL: " << (msg) << " (line " << __LINE__ \
                      << ")\n";                                         \
        }                                                               \
    } while (0)

#define EXPECT_EQ(a, b, msg)                                          \
    do                                                                \
    {                                                                 \
        if ((a) == (b))                                               \
        {                                                             \
            ++g_pass;                                                 \
        }                                                             \
        else                                                          \
        {                                                             \
            ++g_fail;                                                 \
            std::cerr << "  ✗ FAIL: " << (msg) << " — got=" << (a)    \
                      << " expected=" << (b) << " (line " << __LINE__ \
                      << ")\n";                                       \
        }                                                             \
    } while (0)

// ─────────────────────────────────────────────────────────────────────────────
// (AR) مساعد: مسار مؤقت فريد.
// ─────────────────────────────────────────────────────────────────────────────
static std::string tmp_path(const std::string &name)
{
    fs::path p = fs::temp_directory_path() / "sadinfo_s007_tests";
    fs::create_directories(p);
    p /= name;
    return p.string();
}

// ─────────────────────────────────────────────────────────────────────────────
// (AR) مساعد: بناء حالة نموذجية للاختبارات.
// ─────────────────────────────────────────────────────────────────────────────
static CacheState make_sample_state()
{
    CacheState s;
    s.metadata.schema_version = kSupportedSchemaVersion;
    s.metadata.created_at = "2025-01-01T00:00:00Z";
    s.metadata.last_run_at = "2025-01-15T12:34:56Z";
    s.metadata.total_runs = 42;

    s.versions.sadinfo_version = "2.0.0";
    s.versions.schema_version = "1.0.0";
    s.versions.sad_version = "0.9.0";

    EntityState e1;
    e1.id = "kw_function";
    e1.source_path = "lexer/keywords.yaml";
    e1.fast_hash = 0xDEADBEEFCAFEBABEull;
    e1.full_hash = "abcdef0123456789abcdef0123456789"
                   "abcdef0123456789abcdef0123456789";
    e1.mtime_ns = 1737000000000000000ull;
    e1.status = "valid";
    s.entities.emplace(e1.id, e1);

    EntityState e2;
    e2.id = "builtin_print";
    e2.source_path = "stdlib/io/print.yaml";
    e2.fast_hash = 0x1122334455667788ull;
    e2.full_hash = "";
    e2.mtime_ns = 1737000000000000001ull;
    e2.status = "valid";
    s.entities.emplace(e2.id, e2);

    DependencyEdge ed;
    ed.from = "builtin_print";
    ed.to = "kw_function";
    ed.kind = "uses";
    s.dependency_graph.push_back(ed);

    s.merkle_root = "0123456789abcdef0123456789abcdef"
                    "0123456789abcdef0123456789abcdef";
    s.deletions.push_back("deprecated_kw_old");
    return s;
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 1: to_json produces non-empty valid-looking JSON.
// ─────────────────────────────────────────────────────────────────────────────
static void test_to_json_basic()
{
    std::cout << "[1] test_to_json_basic\n";
    const auto state = make_sample_state();
    const auto txt = to_json(state);
    EXPECT_TRUE(!txt.empty(), "to_json returns non-empty");
    EXPECT_TRUE(txt.find("\"cache_metadata\"") != std::string::npos,
                "contains cache_metadata key");
    EXPECT_TRUE(txt.find("\"entities\"") != std::string::npos,
                "contains entities key");
    EXPECT_TRUE(txt.find("kw_function") != std::string::npos,
                "contains entity id");
    EXPECT_TRUE(txt.find("\"merkle_root\"") != std::string::npos,
                "contains merkle_root");
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 2: roundtrip — serialize then parse yields equivalent state.
// ─────────────────────────────────────────────────────────────────────────────
static void test_roundtrip()
{
    std::cout << "[2] test_roundtrip\n";
    const auto original = make_sample_state();
    const auto txt = to_json(original);

    CacheState parsed;
    const auto rc = from_json(txt, parsed);
    EXPECT_EQ(static_cast<int>(rc), static_cast<int>(StateIoResult::Ok),
              "from_json returns Ok");

    EXPECT_EQ(parsed.metadata.total_runs, original.metadata.total_runs,
              "total_runs preserved");
    EXPECT_TRUE(parsed.metadata.created_at == original.metadata.created_at,
                "created_at preserved");
    EXPECT_TRUE(parsed.versions.sadinfo_version == original.versions.sadinfo_version,
                "sadinfo_version preserved");
    EXPECT_EQ(parsed.entities.size(), original.entities.size(),
              "entities count preserved");

    auto it = parsed.entities.find("kw_function");
    EXPECT_TRUE(it != parsed.entities.end(), "entity kw_function present");
    if (it != parsed.entities.end())
    {
        EXPECT_EQ(it->second.fast_hash, 0xDEADBEEFCAFEBABEull,
                  "fast_hash preserved");
        EXPECT_TRUE(it->second.full_hash.size() == 64, "full_hash length 64");
        EXPECT_TRUE(it->second.status == "valid", "status preserved");
    }

    EXPECT_EQ(parsed.dependency_graph.size(), 1u, "1 dep edge");
    if (!parsed.dependency_graph.empty())
    {
        EXPECT_TRUE(parsed.dependency_graph[0].kind == "uses", "edge kind");
    }
    EXPECT_TRUE(parsed.merkle_root == original.merkle_root, "merkle preserved");
    EXPECT_EQ(parsed.deletions.size(), 1u, "1 deletion preserved");
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 3: write_state → read_state roundtrip on disk.
// ─────────────────────────────────────────────────────────────────────────────
static void test_write_read_disk()
{
    std::cout << "[3] test_write_read_disk\n";
    const auto original = make_sample_state();
    const auto path = tmp_path("state_roundtrip.json");
    std::error_code ec;
    fs::remove(path, ec);

    const auto wrc = write_state(path, original);
    EXPECT_EQ(static_cast<int>(wrc), static_cast<int>(StateIoResult::Ok),
              "write_state Ok");
    EXPECT_TRUE(fs::exists(path), "file exists after write");

    CacheState loaded;
    const auto rrc = read_state(path, loaded);
    EXPECT_EQ(static_cast<int>(rrc), static_cast<int>(StateIoResult::Ok),
              "read_state Ok");
    EXPECT_EQ(loaded.entities.size(), original.entities.size(),
              "entities count after disk roundtrip");
    EXPECT_TRUE(loaded.merkle_root == original.merkle_root,
                "merkle after disk roundtrip");

    fs::remove(path, ec);
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 4: read_state on missing file ⇒ FileNotFound (not an error).
// ─────────────────────────────────────────────────────────────────────────────
static void test_read_missing()
{
    std::cout << "[4] test_read_missing\n";
    const auto path = tmp_path("does_not_exist_xyz.json");
    std::error_code ec;
    fs::remove(path, ec);

    CacheState s;
    const auto rc = read_state(path, s);
    EXPECT_EQ(static_cast<int>(rc),
              static_cast<int>(StateIoResult::FileNotFound),
              "missing file returns FileNotFound");
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 5: from_json on garbage ⇒ ParseError.
// ─────────────────────────────────────────────────────────────────────────────
static void test_parse_garbage()
{
    std::cout << "[5] test_parse_garbage\n";
    CacheState s;
    auto rc = from_json("not json at all {{{", s);
    EXPECT_EQ(static_cast<int>(rc),
              static_cast<int>(StateIoResult::ParseError),
              "garbage returns ParseError");

    rc = from_json("[1, 2, 3]", s); // valid JSON but not an object
    EXPECT_EQ(static_cast<int>(rc),
              static_cast<int>(StateIoResult::ParseError),
              "non-object root returns ParseError");
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 6: SchemaMismatch when version differs.
// ─────────────────────────────────────────────────────────────────────────────
static void test_schema_mismatch()
{
    std::cout << "[6] test_schema_mismatch\n";
    const std::string js =
        "{\"cache_metadata\":{\"schema_version\":\"99.0.0\","
        "\"created_at\":\"2025-12-15\"},"
        "\"entities\":{\"x\":{\"id\":\"x\",\"kind\":\"K\"}}}";
    CacheState s;
    const auto rc = from_json(js, s);
    EXPECT_EQ(static_cast<int>(rc),
              static_cast<int>(StateIoResult::SchemaMismatch),
              "unsupported schema returns SchemaMismatch");
    // (AR) J3/J6 — يجب أن يكون out نظيفاً تماماً عند الفشل (لا تسرّب).
    EXPECT_TRUE(s.metadata.schema_version.empty(),
                "J3: metadata cleared after SchemaMismatch");
    EXPECT_TRUE(s.metadata.created_at.empty(),
                "J3: created_at cleared after SchemaMismatch");
    EXPECT_EQ(s.entities.size(), 0u,
              "J3: entities not populated on SchemaMismatch");
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 7: write_state with empty path ⇒ InvalidPath.
// ─────────────────────────────────────────────────────────────────────────────
static void test_invalid_path()
{
    std::cout << "[7] test_invalid_path\n";
    CacheState s = make_sample_state();
    const auto rc = write_state("", s);
    EXPECT_EQ(static_cast<int>(rc),
              static_cast<int>(StateIoResult::InvalidPath),
              "empty path returns InvalidPath");

    CacheState dummy;
    const auto rc2 = read_state("", dummy);
    EXPECT_EQ(static_cast<int>(rc2),
              static_cast<int>(StateIoResult::InvalidPath),
              "empty read path returns InvalidPath");
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 8: atomic_write — overwrites existing final file safely.
// ─────────────────────────────────────────────────────────────────────────────
static void test_atomic_overwrite()
{
    std::cout << "[8] test_atomic_overwrite\n";
    const auto path = tmp_path("atomic_overwrite.json");
    std::error_code ec;
    fs::remove(path, ec);

    EXPECT_TRUE(write_atomic(path, "first version"),
                "first write_atomic succeeds");

    // (AR) قراءة المحتوى للتأكد.
    {
        std::ifstream f(path, std::ios::binary);
        std::ostringstream ss;
        ss << f.rdbuf();
        EXPECT_TRUE(ss.str() == "first version", "first content correct");
    }

    EXPECT_TRUE(write_atomic(path, "second version - longer"),
                "second write_atomic succeeds (overwrite)");

    {
        std::ifstream f(path, std::ios::binary);
        std::ostringstream ss;
        ss << f.rdbuf();
        EXPECT_TRUE(ss.str() == "second version - longer",
                    "second content correct (atomic replace)");
    }

    fs::remove(path, ec);
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 9: atomic_write with empty path ⇒ false.
// ─────────────────────────────────────────────────────────────────────────────
static void test_atomic_empty_path()
{
    std::cout << "[9] test_atomic_empty_path\n";
    EXPECT_TRUE(!write_atomic("", "content"),
                "atomic_write rejects empty path");
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 10: empty state roundtrip — minimal viable cache.
// ─────────────────────────────────────────────────────────────────────────────
static void test_empty_state_roundtrip()
{
    std::cout << "[10] test_empty_state_roundtrip\n";
    CacheState empty;
    empty.metadata.schema_version = kSupportedSchemaVersion;
    const auto txt = to_json(empty);
    EXPECT_TRUE(!txt.empty(), "empty state serializes");

    CacheState back;
    const auto rc = from_json(txt, back);
    EXPECT_EQ(static_cast<int>(rc), static_cast<int>(StateIoResult::Ok),
              "empty roundtrip Ok");
    EXPECT_EQ(back.entities.size(), 0u, "no entities");
    EXPECT_EQ(back.dependency_graph.size(), 0u, "no edges");
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 11: J8 — map key wins over entity.id field (consistency invariant).
// ─────────────────────────────────────────────────────────────────────────────
static void test_key_wins_over_id_field()
{
    std::cout << "[11] test_key_wins_over_id_field\n";
    // (AR) JSON محرَّر يدوياً: المفتاح "alpha" لكن الحقل id="beta".
    //      السياسة (J8): المفتاح يكسب لمنع تناقض داخلي.
    const std::string js =
        "{\"cache_metadata\":{\"schema_version\":\"" +
        std::string(kSupportedSchemaVersion) +
        "\"},\"entities\":{\"alpha\":{\"id\":\"beta\",\"kind\":\"K\"}}}";
    CacheState s;
    const auto rc = from_json(js, s);
    EXPECT_EQ(static_cast<int>(rc), static_cast<int>(StateIoResult::Ok),
              "parses Ok");
    auto it = s.entities.find("alpha");
    EXPECT_TRUE(it != s.entities.end(), "entry found by map key");
    if (it != s.entities.end())
    {
        EXPECT_TRUE(it->second.id == "alpha",
                    "J8: entity.id rewritten to match map key");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 12: K1 — read_state on a directory returns InvalidPath (not ParseError).
// ─────────────────────────────────────────────────────────────────────────────
static void test_read_state_on_directory()
{
    std::cout << "[12] test_read_state_on_directory\n";
    fs::path dir = fs::temp_directory_path() / "sadinfo_s007_tests" / "dir_as_file";
    std::error_code ec;
    fs::create_directories(dir, ec);
    CacheState s;
    const auto rc = read_state(dir.string(), s);
    EXPECT_EQ(static_cast<int>(rc),
              static_cast<int>(StateIoResult::InvalidPath),
              "K1: directory path returns InvalidPath not ParseError");
    fs::remove_all(dir, ec);
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 13: K2 — get_u64 accepts non-negative signed integers (interop).
// ─────────────────────────────────────────────────────────────────────────────
static void test_signed_integer_field_accepted()
{
    std::cout << "[13] test_signed_integer_field_accepted\n";
    // (AR) أداة خارجية قد تكتب total_runs بدون علامة u — يجب أن نقبله.
    const std::string js =
        "{\"cache_metadata\":{\"schema_version\":\"" +
        std::string(kSupportedSchemaVersion) +
        "\",\"total_runs\":7}}";
    CacheState s;
    const auto rc = from_json(js, s);
    EXPECT_EQ(static_cast<int>(rc), static_cast<int>(StateIoResult::Ok),
              "parses Ok");
    EXPECT_EQ(s.metadata.total_runs, 7u,
              "K2: signed positive integer accepted as u64");
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 14: K2 — get_u64 rejects negative integer (defensive).
// ─────────────────────────────────────────────────────────────────────────────
static void test_negative_integer_field_rejected()
{
    std::cout << "[14] test_negative_integer_field_rejected\n";
    const std::string js =
        "{\"cache_metadata\":{\"schema_version\":\"" +
        std::string(kSupportedSchemaVersion) +
        "\",\"total_runs\":-1}}";
    CacheState s;
    const auto rc = from_json(js, s);
    EXPECT_EQ(static_cast<int>(rc), static_cast<int>(StateIoResult::Ok),
              "still parses (graceful)");
    EXPECT_EQ(s.metadata.total_runs, 0u,
              "K2: negative integer falls back to default (no silent cast)");
}

// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    std::cout << "=== S-007 cache_state tests ===\n";
    test_to_json_basic();
    test_roundtrip();
    test_write_read_disk();
    test_read_missing();
    test_parse_garbage();
    test_schema_mismatch();
    test_invalid_path();
    test_atomic_overwrite();
    test_atomic_empty_path();
    test_empty_state_roundtrip();
    test_key_wins_over_id_field();
    test_read_state_on_directory();
    test_signed_integer_field_accepted();
    test_negative_integer_field_rejected();

    // (AR) K10 — تنظيف نهائي لمجلد tmp لمنع تراكم الملفات بين runs.
    {
        std::error_code ec;
        fs::remove_all(fs::temp_directory_path() / "sadinfo_s007_tests", ec);
    }

    std::cout << "\nResults: " << g_pass << " pass, " << g_fail << " fail\n";
    return (g_fail == 0) ? 0 : 1;
}
