// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// tests/sadinfo/unit/cache_manager_tests.cpp
// (AR) S-007 Part E — اختبارات CacheManager (12 اختباراً):
//      M1  info() على مجلد غير موجود → free + state_exists=false
//      M2  info() على مجلد فارغ → free + state_exists=false
//      M3  info() بعد write_state → state_exists=true + entity_count صحيح
//      M4  info() مع قفل حيّ → lock_status="held" + lock_holder موجود
//      M5  info() مع قفل ميت (PID=0xFFFFFFFE) → lock_status="stale"
//      M6  info() مع JSON قفل تالف → lock_status="corrupted"
//      M7  clear() على مجلد فارغ → Ok
//      M8  clear() بعد write_state → Ok + الملف محذوف فعلاً
//      M9  clear() مع قفل حيّ → LockHeld + الملف باقٍ
//      M10 clear() مع قفل ميت → Ok (يكسر الميت ويحذف)
//      M11 clear() على مسار فارغ → InvalidPath
//      M12 read_state/write_state round-trip عبر facade
// ═══════════════════════════════════════════════════════════════════════════════

#include "cache_manager.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;
using namespace Sad::Info;
namespace cache_ns = sad::sadinfo::cache;

// ─── عداد بسيط ───────────────────────────────────────────────────────────
namespace
{
    int g_pass = 0;
    int g_fail = 0;

    void check(bool cond, const char *msg)
    {
        if (cond)
            ++g_pass;
        else
        {
            ++g_fail;
            std::cerr << "  FAIL: " << msg << "\n";
        }
    }
} // namespace

#define EXPECT(cond, msg) check((cond), (msg))

// ─── helpers ──────────────────────────────────────────────────────────────
static fs::path tmp_root()
{
    auto p = fs::temp_directory_path() / "sadinfo_s007_mgr_tests";
    std::error_code ec;
    fs::create_directories(p, ec);
    return p;
}

static std::string fresh_dir(const std::string &name)
{
    auto p = tmp_root() / name;
    std::error_code ec;
    fs::remove_all(p, ec);
    fs::create_directories(p, ec);
    return p.string();
}

// (AR) كتابة ملف قفل يدوياً لمحاكاة سيناريوهات لا تنشأ طبيعياً.
static void write_raw_lock(const std::string &path, const std::string &json_text)
{
    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    ofs << json_text;
}

// (AR) بناء حالة صغيرة صالحة للكتابة.
static cache_ns::CacheState make_small_state()
{
    cache_ns::CacheState s;
    s.metadata.schema_version = cache_ns::kSupportedSchemaVersion;
    s.metadata.created_at = "2026-01-01T00:00:00Z";
    s.metadata.last_run_at = "2026-01-01T00:00:00Z";
    s.metadata.total_runs = 1;
    s.versions.sad_version = "0.1.0";
    s.versions.sadinfo_version = "0.1.0";
    s.versions.schema_version = cache_ns::kSupportedSchemaVersion;
    cache_ns::EntityState e;
    e.id = "dummy";
    e.source_path = "dummy.yaml";
    e.fast_hash = 0xCAFEBABEull;
    e.full_hash = "";
    e.mtime_ns = 1;
    e.status = "valid";
    s.entities["dummy"] = e;
    return s;
}

// ═════════════════════════════════════════════════════════════════════════
// M1: info على مجلد غير موجود
// ═════════════════════════════════════════════════════════════════════════
static void test_info_missing_dir()
{
    std::cout << "[M1] info_missing_dir\n";
    CacheManager mgr((tmp_root() / "does_not_exist_xyz").string());
    const auto r = mgr.info();
    EXPECT(!r.state_exists, "state_exists should be false");
    EXPECT(r.state_size_bytes == 0, "size should be 0");
    EXPECT(r.lock_status == "free", "lock_status=free");
    EXPECT(!r.lock_holder.has_value(), "no holder");
}

// ═════════════════════════════════════════════════════════════════════════
// M2: info على مجلد فارغ
// ═════════════════════════════════════════════════════════════════════════
static void test_info_empty_dir()
{
    std::cout << "[M2] info_empty_dir\n";
    CacheManager mgr(fresh_dir("m2"));
    const auto r = mgr.info();
    EXPECT(!r.state_exists, "no state");
    EXPECT(r.lock_status == "free", "free");
}

// ═════════════════════════════════════════════════════════════════════════
// M3: info بعد write_state
// ═════════════════════════════════════════════════════════════════════════
static void test_info_after_write()
{
    std::cout << "[M3] info_after_write\n";
    CacheManager mgr(fresh_dir("m3"));
    auto st = make_small_state();
    EXPECT(mgr.write_state(st) == cache_ns::StateIoResult::Ok, "write Ok");
    const auto r = mgr.info();
    EXPECT(r.state_exists, "state_exists true");
    EXPECT(r.state_size_bytes > 0, "size>0");
    EXPECT(r.entity_count == 1, "entity_count=1");
    EXPECT(r.lock_status == "free", "lock free");
}

// ═════════════════════════════════════════════════════════════════════════
// M4: info مع قفل حيّ
// ═════════════════════════════════════════════════════════════════════════
static void test_info_with_live_lock()
{
    std::cout << "[M4] info_with_live_lock\n";
    auto dir = fresh_dir("m4");
    CacheManager mgr(dir);
    cache_ns::CacheLock lock;
    cache_ns::LockInfo holder;
    EXPECT(mgr.acquire(lock, holder) == cache_ns::LockResult::Ok, "acquire Ok");
    const auto r = mgr.info();
    EXPECT(r.lock_status == "held", "status=held");
    EXPECT(r.lock_holder.has_value(), "holder present");
    // lock تُحرَّر تلقائياً (RAII)
}

// ═════════════════════════════════════════════════════════════════════════
// M5: info مع قفل ميت يدوي
// ═════════════════════════════════════════════════════════════════════════
static void test_info_with_stale_lock()
{
    std::cout << "[M5] info_with_stale_lock\n";
    auto dir = fresh_dir("m5");
    CacheManager mgr(dir);
    // (AR) PID مستحيل = 0xFFFFFFFE — يحاكي عملية ميتة.
    const std::string stale_json =
        R"({"version":1,"pid":4294967294,"hostname":"x","start_time_ns":1,)"
        R"("created_ns":1,"tool_version":"t","platform":"win","schema":"1.0.0"})";
    write_raw_lock(mgr.lock_path(), stale_json);
    const auto r = mgr.info();
    EXPECT(r.lock_status == "stale", "status=stale");
    EXPECT(r.lock_holder.has_value(), "holder present");
}

// ═════════════════════════════════════════════════════════════════════════
// M6: info مع JSON تالف
// ═════════════════════════════════════════════════════════════════════════
static void test_info_corrupted_lock()
{
    std::cout << "[M6] info_corrupted_lock\n";
    auto dir = fresh_dir("m6");
    CacheManager mgr(dir);
    write_raw_lock(mgr.lock_path(), "{{not json at all");
    const auto r = mgr.info();
    EXPECT(r.lock_status == "corrupted", "status=corrupted");
}

// ═════════════════════════════════════════════════════════════════════════
// M7: clear على مجلد فارغ
// ═════════════════════════════════════════════════════════════════════════
static void test_clear_empty()
{
    std::cout << "[M7] clear_empty\n";
    CacheManager mgr(fresh_dir("m7"));
    EXPECT(mgr.clear() == ClearResult::Ok, "Ok on empty");
}

// ═════════════════════════════════════════════════════════════════════════
// M8: clear بعد write_state
// ═════════════════════════════════════════════════════════════════════════
static void test_clear_after_write()
{
    std::cout << "[M8] clear_after_write\n";
    CacheManager mgr(fresh_dir("m8"));
    auto st = make_small_state();
    EXPECT(mgr.write_state(st) == cache_ns::StateIoResult::Ok, "write");
    EXPECT(fs::exists(mgr.state_path()), "exists before clear");
    EXPECT(mgr.clear() == ClearResult::Ok, "clear Ok");
    EXPECT(!fs::exists(mgr.state_path()), "deleted after clear");
}

// ═════════════════════════════════════════════════════════════════════════
// M9: clear مع قفل حيّ من معالج آخر
// ═════════════════════════════════════════════════════════════════════════
static void test_clear_with_live_lock()
{
    std::cout << "[M9] clear_with_live_lock\n";
    auto dir = fresh_dir("m9");
    CacheManager mgr(dir);
    auto st = make_small_state();
    EXPECT(mgr.write_state(st) == cache_ns::StateIoResult::Ok, "write");
    cache_ns::CacheLock holder_lock;
    cache_ns::LockInfo holder;
    EXPECT(mgr.acquire(holder_lock, holder) == cache_ns::LockResult::Ok, "acquire");
    // (AR) الآن clear() يحاول أخذ قفل ثانٍ → AlreadyHeld → LockHeld
    EXPECT(mgr.clear() == ClearResult::LockHeld, "clear=LockHeld");
    EXPECT(fs::exists(mgr.state_path()), "state still exists");
}

// ═════════════════════════════════════════════════════════════════════════
// M10: clear مع قفل ميت — try_acquire يكسره ويعيد StaleAcquired
// ═════════════════════════════════════════════════════════════════════════
static void test_clear_with_stale_lock()
{
    std::cout << "[M10] clear_with_stale_lock\n";
    auto dir = fresh_dir("m10");
    CacheManager mgr(dir);
    auto st = make_small_state();
    EXPECT(mgr.write_state(st) == cache_ns::StateIoResult::Ok, "write");
    const std::string stale_json =
        R"({"version":1,"pid":4294967294,"hostname":"x","start_time_ns":1,)"
        R"("created_ns":1,"tool_version":"t","platform":"win","schema":"1.0.0"})";
    write_raw_lock(mgr.lock_path(), stale_json);
    EXPECT(mgr.clear() == ClearResult::Ok, "clear breaks stale → Ok");
    EXPECT(!fs::exists(mgr.state_path()), "state deleted");
}

// ═════════════════════════════════════════════════════════════════════════
// M11: clear على مسار فارغ
// ═════════════════════════════════════════════════════════════════════════
static void test_clear_invalid_path()
{
    std::cout << "[M11] clear_invalid_path\n";
    CacheManager mgr("");
    EXPECT(mgr.clear() == ClearResult::InvalidPath, "InvalidPath");
}

// ═════════════════════════════════════════════════════════════════════════
// M12: round-trip read/write
// ═════════════════════════════════════════════════════════════════════════
static void test_read_write_roundtrip()
{
    std::cout << "[M12] read_write_roundtrip\n";
    CacheManager mgr(fresh_dir("m12"));
    auto st = make_small_state();
    EXPECT(mgr.write_state(st) == cache_ns::StateIoResult::Ok, "write");
    cache_ns::CacheState got;
    EXPECT(mgr.read_state(got) == cache_ns::StateIoResult::Ok, "read");
    EXPECT(got.entities.size() == 1, "1 entity round-tripped");
    EXPECT(got.entities.count("dummy") == 1, "dummy key present");
}

// ─── main ─────────────────────────────────────────────────────────────────
int main()
{
    std::cout << "═══ CacheManager Tests (S-007 Part E) ═══\n";
    test_info_missing_dir();
    test_info_empty_dir();
    test_info_after_write();
    test_info_with_live_lock();
    test_info_with_stale_lock();
    test_info_corrupted_lock();
    test_clear_empty();
    test_clear_after_write();
    test_clear_with_live_lock();
    test_clear_with_stale_lock();
    test_clear_invalid_path();
    test_read_write_roundtrip();

    std::cout << "\n─────────────────────────────────────\n"
              << "PASS: " << g_pass << "  FAIL: " << g_fail << "\n";
    return g_fail == 0 ? 0 : 1;
}
