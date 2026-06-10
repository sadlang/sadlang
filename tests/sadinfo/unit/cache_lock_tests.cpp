// ״¨״³… ״§„„‡ ״§„״±״­…† ״§„״±״­…
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// tests/sadinfo/unit/cache_lock_tests.cpp
// (AR) S-007 C+D ג€” ״§״®״×״¨״§״±״§״× CacheLock (16 ״§״®״×״¨״§״±״§‹):
//      T1  acquire on fresh path ג†’ Ok + held
//      T2  double-acquire by SAME object ג†’ AlreadyHeld
//      T3  acquire while held by ANOTHER live object ג†’ AlreadyHeld + out filled
//      T4  release then re-acquire ג†’ Ok
//      T5  inspect on missing file ג†’ IoError
//      T6  inspect on directory ג†’ InvalidPath
//      T7  inspect on garbage JSON ג†’ ParseError
//      T8  force_break with matching info ג†’ Ok
//      T9  force_break with mismatching info ג†’ Mismatch
//      T10 stale lock (PID=0xFFFFFFFE) ג†’ StaleAcquired
//      T11 release auto-called on destructor (RAII)
//      T12 move semantics preserve ownership
//      T13 acquire on empty path ג†’ InvalidPath
//      T14 release without acquire ג†’ NotHeld
//      T15 (L2) release ״¨״¹״¯ ״­״° ״®״§״±״¬ ג†’ Ok (idempotent)
//      T16 (L2) release ״¹†״¯ ״§״³״×״¨״¯״§„ ״®״§״±״¬ ג†’ Mismatch ״¨„״§ ״­״°
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

#include "cache_lock.h"

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>

namespace fs = std::filesystem;
using namespace sad::sadinfo::cache;

// (AR) R26 — اختبار ping-pong stress (سباق stale-break بين عمليتين فعليتين)
//      مؤجَّل لـPart E: يحتاج إما 2-process حقيقي (CreateProcess/fork)
//      أو mock layer للplatform_create_excl/check_owner. هنا يكفي
//      kMaxAttempts=3 + اختبار التحقق من الحدّ في cache_lock.cpp.

// ─────────────────────────────────────────────────────────────────────────
namespace
{
    int g_pass = 0;
    int g_fail = 0;

    template <class T, class U>
    void expect_eq(const T &a, const U &b, const char *msg)
    {
        if (a == b)
        {
            ++g_pass;
        }
        else
        {
            ++g_fail;
            std::cerr << "  FAIL: " << msg << " (got " << static_cast<long long>(a)
                      << " expected " << static_cast<long long>(b) << ")\n";
        }
    }

    void expect_true(bool c, const char *msg)
    {
        if (c)
        {
            ++g_pass;
        }
        else
        {
            ++g_fail;
            std::cerr << "  FAIL: " << msg << "\n";
        }
    }
}

#define EXPECT_EQ(a, b, m) expect_eq((a), (b), (m))
#define EXPECT_TRUE(c, m) expect_true((c), (m))

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
static fs::path tmp_root()
{
    fs::path p = fs::temp_directory_path() / "sadinfo_s007_lock_tests";
    std::error_code ec;
    fs::create_directories(p, ec);
    return p;
}

static std::string fresh_path(const std::string &name)
{
    // (AR) „ƒ„ ״§״®״×״¨״§״± …״³״§״± ״±״¯ „…†״¹ ״×״¯״§״®„ state ״¨† ״§„״§״®״×״¨״§״±״§״×.
    return (tmp_root() / name).string();
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T1: acquire fresh
static void test_acquire_fresh()
{
    std::cout << "[1] test_acquire_fresh\n";
    const auto p = fresh_path("t1.lock");
    std::error_code ec;
    fs::remove(p, ec);

    CacheLock lock;
    LockInfo existing{};
    const auto rc = lock.try_acquire(p, existing);
    EXPECT_EQ(static_cast<int>(rc), static_cast<int>(LockResult::Ok),
              "fresh path ג†’ Ok");
    EXPECT_TRUE(lock.is_held(), "lock object holds");
    EXPECT_TRUE(fs::exists(p), "lock file exists on disk");
    (void)lock.release();
    EXPECT_TRUE(!fs::exists(p), "lock file removed after release");
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T2: double-acquire same object
static void test_double_acquire_same_obj()
{
    std::cout << "[2] test_double_acquire_same_obj\n";
    const auto p = fresh_path("t2.lock");
    std::error_code ec;
    fs::remove(p, ec);

    CacheLock lock;
    LockInfo existing{};
    EXPECT_EQ(static_cast<int>(lock.try_acquire(p, existing)),
              static_cast<int>(LockResult::Ok),
              "first acquire Ok");
    EXPECT_EQ(static_cast<int>(lock.try_acquire(p, existing)),
              static_cast<int>(LockResult::AlreadyHeld),
              "second acquire on same object ג†’ AlreadyHeld");
    (void)lock.release();
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T3: contended acquire (two live holders) ג€” T9 …† ״§„…ˆ״§״µ״©״ simulated -״¹…„״©.
static void test_contended_acquire()
{
    std::cout << "[3] test_contended_acquire\n";
    const auto p = fresh_path("t3.lock");
    std::error_code ec;
    fs::remove(p, ec);

    CacheLock a;
    LockInfo info_a{};
    EXPECT_EQ(static_cast<int>(a.try_acquire(p, info_a)),
              static_cast<int>(LockResult::Ok), "A acquires Ok");

    CacheLock b;
    LockInfo info_b{};
    const auto rc = b.try_acquire(p, info_b);
    EXPECT_EQ(static_cast<int>(rc),
              static_cast<int>(LockResult::AlreadyHeld),
              "B sees AlreadyHeld (same process is alive)");
    EXPECT_TRUE(info_b.pid != 0, "B received existing holder's PID");
    EXPECT_TRUE(!b.is_held(), "B does not hold");
    (void)a.release();
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T4: release + re-acquire
static void test_release_then_reacquire()
{
    std::cout << "[4] test_release_then_reacquire\n";
    const auto p = fresh_path("t4.lock");
    std::error_code ec;
    fs::remove(p, ec);

    CacheLock lock;
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(lock.try_acquire(p, info)),
              static_cast<int>(LockResult::Ok), "first Ok");
    EXPECT_EQ(static_cast<int>(lock.release()),
              static_cast<int>(LockResult::Ok), "release Ok");
    EXPECT_EQ(static_cast<int>(lock.try_acquire(p, info)),
              static_cast<int>(LockResult::Ok), "re-acquire Ok");
    (void)lock.release();
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T5: inspect missing
static void test_inspect_missing()
{
    std::cout << "[5] test_inspect_missing\n";
    const auto p = fresh_path("t5_missing.lock");
    std::error_code ec;
    fs::remove(p, ec);
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(CacheLock::inspect(p, info)),
              static_cast<int>(LockResult::IoError),
              "missing ג†’ IoError");
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T6: inspect on directory
static void test_inspect_directory()
{
    std::cout << "[6] test_inspect_directory\n";
    fs::path dir = tmp_root() / "t6_dir";
    std::error_code ec;
    fs::create_directories(dir, ec);
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(CacheLock::inspect(dir.string(), info)),
              static_cast<int>(LockResult::InvalidPath),
              "directory ג†’ InvalidPath");
    fs::remove_all(dir, ec);
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T7: inspect garbage
static void test_inspect_garbage()
{
    std::cout << "[7] test_inspect_garbage\n";
    const auto p = fresh_path("t7.lock");
    {
        std::ofstream f(p, std::ios::binary);
        f << "not valid json {{{";
    }
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(CacheLock::inspect(p, info)),
              static_cast<int>(LockResult::ParseError),
              "garbage ג†’ ParseError");
    std::error_code ec;
    fs::remove(p, ec);
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T8: force_break matching
static void test_force_break_match()
{
    std::cout << "[8] test_force_break_match\n";
    const auto p = fresh_path("t8.lock");
    std::error_code ec;
    fs::remove(p, ec);

    CacheLock lock;
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(lock.try_acquire(p, info)),
              static_cast<int>(LockResult::Ok), "acquire Ok");

    LockInfo current{};
    EXPECT_EQ(static_cast<int>(CacheLock::inspect(p, current)),
              static_cast<int>(LockResult::Ok), "inspect Ok");

    EXPECT_EQ(static_cast<int>(CacheLock::force_break(p, current)),
              static_cast<int>(LockResult::Ok), "force_break matching ג†’ Ok");
    EXPECT_TRUE(!fs::exists(p), "file removed after force_break");

    // (AR) lock object ״¸† ״£†‡ …״×„ƒ ג€” release ״³ƒ״´ Mismatch (…„ ״÷״± …ˆ״¬ˆ״¯
    //      = read ״§״´„ = †״×״¬״§‡„ ״¨״¯ˆ† ״­״°).
    (void)lock.release();
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T9: force_break mismatch
static void test_force_break_mismatch()
{
    std::cout << "[9] test_force_break_mismatch\n";
    const auto p = fresh_path("t9.lock");
    std::error_code ec;
    fs::remove(p, ec);

    CacheLock lock;
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(lock.try_acquire(p, info)),
              static_cast<int>(LockResult::Ok), "acquire Ok");

    LockInfo wrong{};
    wrong.pid = 999'999u; // (AR) PID …״®״×„ ״¨״§„״×״£ƒ״¯ ״¹†״§
    wrong.start_time_ns = 12345u;
    EXPECT_EQ(static_cast<int>(CacheLock::force_break(p, wrong)),
              static_cast<int>(LockResult::Mismatch),
              "force_break wrong info ג†’ Mismatch (TOCTOU guard)");
    EXPECT_TRUE(fs::exists(p), "lock file untouched");
    (void)lock.release();
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T10: stale lock auto-break (T10 …† ״§„…ˆ״§״µ״©).
//      †״µ†״¹ …„ ‚„ ״¯ˆ״§‹ ״­ˆ PID „״¹…„״© …״×״© ג†’ try_acquire ƒ״×״´ stale
//      ˆ״±״¬״¹ StaleAcquired.
static void test_stale_auto_break()
{
    std::cout << "[10] test_stale_auto_break\n";
    const auto p = fresh_path("t10.lock");
    std::error_code ec;
    fs::remove(p, ec);

    // (AR) L6 ג€” PID ‚״±״¨ …† max u32 „״§ ״®״µ‘״µ „״¹…„״© ״­‚‚״© ״¹„‰ ״£ OS …״¯״¹ˆ…:
    //      Linux: pid_max ״§״×״±״§״¶ 32768-4194304״› Windows: PIDs …״¶״§״¹״§״× 4 ˆ„״§
    //      ״×״×״¬״§ˆ״² ״¹״§״¯״©‹ 100K. 0xFFFFFFFE = 4_294_967_294 ג€” ״¢…† ƒ€"…״× ״¨״§„״×״£ƒ״¯".
    const std::uint64_t dead_pid = 0xFFFFFFFEull;

    // (AR) ƒ״×״§״¨״© ״¯ˆ״© „…„ ״§„‚„ ״¨״µ״÷״© JSON …״×ˆ״§‚״© …״¹ parse_lock.
    {
        std::ofstream f(p, std::ios::binary);
        f << "{\n"
          << "  \"pid\": " << dead_pid << ",\n"
          << "  \"start_time_ns\": 1,\n"
          << "  \"hostname\": \"stale-host\",\n"
          << "  \"acquired_at\": \"2020-01-01T00:00:00Z\",\n"
          << "  \"tool_version\": \"sadinfo 0.0.0\"\n"
          << "}\n";
    }

    CacheLock lock;
    LockInfo existing{};
    const auto rc = lock.try_acquire(p, existing);
    EXPECT_EQ(static_cast<int>(rc),
              static_cast<int>(LockResult::StaleAcquired),
              "dead-PID lock auto-broken and re-acquired");
    EXPECT_TRUE(lock.is_held(), "lock now held by us");
    (void)lock.release();
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T11: destructor releases
static void test_destructor_releases()
{
    std::cout << "[11] test_destructor_releases\n";
    const auto p = fresh_path("t11.lock");
    std::error_code ec;
    fs::remove(p, ec);
    {
        CacheLock lock;
        LockInfo info{};
        EXPECT_EQ(static_cast<int>(lock.try_acquire(p, info)),
                  static_cast<int>(LockResult::Ok), "acquire Ok");
        EXPECT_TRUE(fs::exists(p), "file exists while held");
    }
    EXPECT_TRUE(!fs::exists(p), "file removed by destructor");
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T12: move semantics
static void test_move_semantics()
{
    std::cout << "[12] test_move_semantics\n";
    const auto p = fresh_path("t12.lock");
    std::error_code ec;
    fs::remove(p, ec);

    CacheLock a;
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(a.try_acquire(p, info)),
              static_cast<int>(LockResult::Ok), "a acquires");

    CacheLock b = std::move(a);
    EXPECT_TRUE(!a.is_held(), "moved-from no longer holds");
    EXPECT_TRUE(b.is_held(), "moved-to holds");
    EXPECT_TRUE(fs::exists(p), "file still exists");
    (void)b.release();
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T13: empty path
static void test_empty_path()
{
    std::cout << "[13] test_empty_path\n";
    CacheLock lock;
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(lock.try_acquire("", info)),
              static_cast<int>(LockResult::InvalidPath),
              "empty path ג†’ InvalidPath");
    EXPECT_EQ(static_cast<int>(CacheLock::inspect("", info)),
              static_cast<int>(LockResult::InvalidPath),
              "inspect empty ג†’ InvalidPath");
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T14: release without acquire
static void test_release_without_acquire()
{
    std::cout << "[14] test_release_without_acquire\n";
    CacheLock lock;
    EXPECT_EQ(static_cast<int>(lock.release()),
              static_cast<int>(LockResult::NotHeld),
              "release without acquire ג†’ NotHeld");
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T15 (L2): release ״¨״¹״¯ ״­״° ״®״§״±״¬ „„…„ ג†’ Ok (idempotent)״ „״§ ״­״° „‚„ ״÷״±״¨.
static void test_release_after_external_delete()
{
    std::cout << "[15] test_release_after_external_delete\n";
    const auto p = fresh_path("t15.lock");
    std::error_code ec;
    fs::remove(p, ec);

    CacheLock lock;
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(lock.try_acquire(p, info)),
              static_cast<int>(LockResult::Ok), "acquire Ok");

    // (AR) …״­״§ƒ״§״© ״­״° ״®״§״±״¬ (…״¯״± †״¸״§…״ ״£ˆ ‚„ ״¢״®״± ״×… force_break).
    fs::remove(p, ec);
    EXPECT_TRUE(!fs::exists(p), "file removed externally");

    // (AR) L2 ג€” release ״§„״¢† ״±״¬״¹ Ok ״¨״£…״§† (read_lock_file ג†’ IoError ג†’ cleanup state).
    EXPECT_EQ(static_cast<int>(lock.release()),
              static_cast<int>(LockResult::Ok),
              "release after external delete ג†’ Ok (idempotent)");
    EXPECT_TRUE(!lock.is_held(), "state cleared after release");
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// T16 (L2): release ״¹†״¯…״§ ״³״×״¨״¯„ ״´״®״µ ״¢״®״± ״§„‚„ ״¨‚„ ״÷״±״¨ ג†’ Mismatch״ „״§ ״­״°.
static void test_release_when_lock_replaced()
{
    std::cout << "[16] test_release_when_lock_replaced\n";
    const auto p = fresh_path("t16.lock");
    std::error_code ec;
    fs::remove(p, ec);

    CacheLock lock;
    LockInfo info{};
    EXPECT_EQ(static_cast<int>(lock.try_acquire(p, info)),
              static_cast<int>(LockResult::Ok), "acquire Ok");

    // (AR) …״­״§ƒ״§״© ״§״³״×״¨״¯״§„ ״®״§״±״¬: †ƒ״×״¨ ‚„״§‹ ״¨€PID …״®״×„ ˆ‚ …„†״§.
    fs::remove(p, ec);
    {
        std::ofstream f(p, std::ios::binary);
        f << R"({"pid":12345,"start_time_ns":7,"hostname":"x","acquired_at":"2030-01-01T00:00:00Z","tool_version":"other"})";
    }

    EXPECT_EQ(static_cast<int>(lock.release()),
              static_cast<int>(LockResult::Mismatch),
              "release on replaced lock ג†’ Mismatch (no deletion)");
    EXPECT_TRUE(fs::exists(p), "foreign lock NOT deleted");
    EXPECT_TRUE(!lock.is_held(), "our state cleared anyway");
    fs::remove(p, ec);
}

// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
int main()
{
    std::cout << "=== S-007 C+D cache_lock tests ===\n";
    test_acquire_fresh();
    test_double_acquire_same_obj();
    test_contended_acquire();
    test_release_then_reacquire();
    test_inspect_missing();
    test_inspect_directory();
    test_inspect_garbage();
    test_force_break_match();
    test_force_break_mismatch();
    test_stale_auto_break();
    test_destructor_releases();
    test_move_semantics();
    test_empty_path();
    test_release_without_acquire();
    test_release_after_external_delete();
    test_release_when_lock_replaced();

    // (AR) K10 ג€” ״×†״¸ †‡״§״¦.
    std::error_code ec;
    fs::remove_all(tmp_root(), ec);

    std::cout << "\nResults: " << g_pass << " pass, " << g_fail << " fail\n";
    return g_fail == 0 ? 0 : 1;
}
