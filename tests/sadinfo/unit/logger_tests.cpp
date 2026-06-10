// ===================================================================
// (AR) logger_tests.cpp — اختبارات وحدة + معيار أداء للـlogger.
// (EN) logger_tests.cpp — Logger unit tests + throughput benchmark.
//
// المتطلب الحرج (AC-L1): ≥ 10,000 رسالة/ثانية على debug build.
//
// مكتبة الاختبار: catch2/gtest غير مفترضة — نستخدم اختبارات بسيطة
// قائمة على main() وأكواد خروج. (تكاملها مع framework يأتي مع
// CMake wiring في story لاحقة).
// ===================================================================
#include "logging.h"

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <vector>

using namespace Sad::SadInfo;

namespace
{

    // ----------------------------------------------------------------------------
    // (AR) ثوابت الاختبار — لا أرقام سحرية متناثرة.
    // (EN) Test constants — no scattered magic numbers.
    // ----------------------------------------------------------------------------
    constexpr int kThroughputN = 20000;        // عدد الرسائل لقياس الإنتاجية
    constexpr double kMinRatePerSec = 10000.0; // AC-L1: الحد الأدنى المطلوب
    constexpr int kSafetyThreads = 4;          // خيوط test_thread_safety
    constexpr int kSafetyPerThread = 1000;     // رسائل لكل خيط
    constexpr int kRaceLogs = 5000;            // رسائل أثناء تبديل المستوى

    int g_failures = 0;

#define CHECK(cond, msg)                                              \
    do                                                                \
    {                                                                 \
        if (!(cond))                                                  \
        {                                                             \
            std::cerr << "[FAIL] " << __FUNCTION__ << ":" << __LINE__ \
                      << " — " << msg << "\n";                        \
            ++g_failures;                                             \
        }                                                             \
    } while (0)

    // ----------------------------------------------------------------------------
    // (AR) تركيب: تيار بديل بدل stderr لكل اختبار.
    // (EN) Fixture: replace sink with a stringstream for inspection.
    // ----------------------------------------------------------------------------
    struct SinkGuard
    {
        std::stringstream ss;
        Level prev_level{Level::INFO};
        SinkGuard()
        {
            prev_level = Logger::instance().level();
            Logger::instance().set_sink(&ss);
        }
        ~SinkGuard()
        {
            Logger::instance().set_sink(nullptr); // back to stderr
            Logger::instance().set_level(prev_level);
        }
    };

    void test_parse_level()
    {
        CHECK(Logger::parse_level("trace") == Level::TRACE, "trace");
        CHECK(Logger::parse_level("debug") == Level::DEBUG, "debug");
        CHECK(Logger::parse_level("info") == Level::INFO, "info");
        CHECK(Logger::parse_level("warn") == Level::WARN, "warn");
        CHECK(Logger::parse_level("warning") == Level::WARN, "warning alias");
        CHECK(Logger::parse_level("error") == Level::ERROR, "error");
        CHECK(Logger::parse_level("err") == Level::ERROR, "err alias");
        CHECK(Logger::parse_level("off") == Level::OFF, "off");
        CHECK(Logger::parse_level("none") == Level::OFF, "none alias");
        CHECK(Logger::parse_level("garbage") == Level::INFO, "fallback");
    }

    void test_level_filter()
    {
        SinkGuard g;
        Logger::instance().set_level(Level::WARN);

        log_debug("test", "should not appear");
        log_info("test", "should not appear");
        log_warn("test", "yes_warn");
        log_error("test", "yes_error");

        std::string out = g.ss.str();
        CHECK(out.find("should not appear") == std::string::npos,
              "تم تسريب رسائل أدنى من المستوى");
        CHECK(out.find("yes_warn") != std::string::npos, "warn مفقود");
        CHECK(out.find("yes_error") != std::string::npos, "error مفقود");

        Logger::instance().set_level(Level::INFO); // restore
    }

    void test_json_format()
    {
        SinkGuard g;
        Logger::instance().set_level(Level::INFO);
        log_info("comp_a", "hello world",
                 {{"count", static_cast<int64_t>(42)},
                  {"ratio", 3.14},
                  {"ok", true},
                  {"name", std::string{"sadlang"}}});
        std::string line = g.ss.str();

        CHECK(!line.empty() && line.back() == '\n', "السطر يجب أن ينتهي بـ\\n");
        CHECK(line.find("\"level\":\"info\"") != std::string::npos, "level field");
        CHECK(line.find("\"component\":\"comp_a\"") != std::string::npos, "component");
        CHECK(line.find("\"msg\":\"hello world\"") != std::string::npos, "msg");
        CHECK(line.find("\"count\":42") != std::string::npos, "int field");
        CHECK(line.find("\"ratio\":") != std::string::npos, "double field");
        CHECK(line.find("\"ok\":true") != std::string::npos, "bool field");
        CHECK(line.find("\"name\":\"sadlang\"") != std::string::npos, "string field");
        CHECK(line.find("\"ts\":\"") != std::string::npos, "timestamp present");
    }

    void test_json_escape()
    {
        SinkGuard g;
        Logger::instance().set_level(Level::INFO);
        log_info("esc", "line1\nline2\"quote\"\ttab\\back");
        std::string line = g.ss.str();
        CHECK(line.find("\\n") != std::string::npos, "newline escaped");
        CHECK(line.find("\\\"") != std::string::npos, "quote escaped");
        CHECK(line.find("\\t") != std::string::npos, "tab escaped");
        CHECK(line.find("\\\\") != std::string::npos, "backslash escaped");
    }

    void test_utf8_passthrough()
    {
        SinkGuard g;
        Logger::instance().set_level(Level::INFO);
        log_info("ar", "مرحبا بلغة ص");
        std::string line = g.ss.str();
        // UTF-8 bytes should appear as-is (no \uXXXX for ≥0x20)
        CHECK(line.find("مرحبا بلغة ص") != std::string::npos,
              "UTF-8 يجب أن يمر كما هو");
    }

    void test_off_level()
    {
        SinkGuard g;
        Logger::instance().set_level(Level::OFF);
        log_error("x", "should_be_silent");
        CHECK(g.ss.str().empty(), "OFF يجب أن يكتم كل شيء");
        Logger::instance().set_level(Level::INFO);
    }

    // (AR) RAII لاستعادة env بشكل مضمون حتى عند فشل CHECK.
    // (EN) RAII to guarantee env restoration even on CHECK failure.
    struct EnvGuard
    {
        const char *name;
        std::string prev;
        bool had_prev;
        EnvGuard(const char *n) : name(n), had_prev(false)
        {
#ifdef _WIN32
            char *buf = nullptr;
            size_t sz = 0;
            if (_dupenv_s(&buf, &sz, n) == 0 && buf)
            {
                prev = buf;
                had_prev = true;
                std::free(buf);
            }
#else
            if (const char *v = std::getenv(n))
            {
                prev = v;
                had_prev = true;
            }
#endif
        }
        ~EnvGuard()
        {
#ifdef _WIN32
            if (had_prev)
                _putenv_s(name, prev.c_str());
            else
                _putenv_s(name, "");
#else
            if (had_prev)
                setenv(name, prev.c_str(), 1);
            else
                unsetenv(name);
#endif
        }
    };

    // (AR) اختبار reload_level_from_env — يضبط env ثم يطلب إعادة قراءته.
    // (EN) Test reload_level_from_env — sets env then asks for re-read.
    void test_env_reload()
    {
        SinkGuard g;
        EnvGuard envg("SADINFO_LOG_LEVEL"); // (AR) ضمان استعادة البيئة حتى عند الفشل
        Logger::instance().set_level(Level::INFO);

#ifdef _WIN32
        _putenv_s("SADINFO_LOG_LEVEL", "error");
#else
        setenv("SADINFO_LOG_LEVEL", "error", 1);
#endif
        Logger::instance().reload_level_from_env();
        CHECK(Logger::instance().level() == Level::ERROR,
              "reload_level_from_env لم يطبّق ERROR");

#ifdef _WIN32
        _putenv_s("SADINFO_LOG_LEVEL", "off");
#else
        setenv("SADINFO_LOG_LEVEL", "off", 1);
#endif
        Logger::instance().reload_level_from_env();
        CHECK(Logger::instance().level() == Level::OFF,
              "reload_level_from_env لم يطبّق OFF");

        // (AR) امسح env داخل النطاق — EnvGuard يستعيد القيمة الأصلية بعد.
#ifdef _WIN32
        _putenv_s("SADINFO_LOG_LEVEL", "");
#else
        unsetenv("SADINFO_LOG_LEVEL");
#endif
        Logger::instance().reload_level_from_env();
        CHECK(Logger::instance().level() == Level::INFO,
              "reload_level_from_env لم يرجع لـINFO عند غياب env");
    }

    // ----------------------------------------------------------------------------
    // (AR) معيار الأداء: ≥ 10k رسالة/ثانية في خيط واحد.
    //      نقيس على stringstream (الذاكرة) و ofstream (ملف) معاً —
    //      الإنتاج الحقيقي يكتب لـstderr/ملف، فقياس الذاكرة وحده
    //      مضلِّل (BF-22). الحد الأدنى يُطبَّق على الأبطأ منهما.
    // (EN) Benchmark: ≥10k msg/sec single-threaded.
    //      Measure both stringstream (memory) AND ofstream (real file) —
    //      production writes to stderr/file, so memory-only is misleading.
    //      The threshold applies to the slower of the two.
    // ----------------------------------------------------------------------------
    void test_throughput_10k()
    {
        // 1) قياس على الذاكرة (stringstream)
        {
            SinkGuard g;
            Logger::instance().set_level(Level::INFO);
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < kThroughputN; ++i)
            {
                log_info("bench", "msg",
                         {{"i", static_cast<int64_t>(i)}});
            }
            auto end = std::chrono::high_resolution_clock::now();
            double secs = std::chrono::duration<double>(end - start).count();
            double rate = kThroughputN / secs;
            std::cout << "  throughput (memory): " << static_cast<int64_t>(rate)
                      << " msg/sec (" << kThroughputN << " msgs in "
                      << secs << "s)\n";
            CHECK(rate >= kMinRatePerSec,
                  "throughput على الذاكرة تحت 10k msg/s");
        }

        // 2) قياس على ملف حقيقي (يشمل تكلفة flush الفعلية)
        {
            // (AR) ملف مؤقت في temp dir — اسم فريد لتجنب التعارض.
            // (EN) Temp file with unique name to avoid clashes.
            std::string tmp_path;
            {
                std::ostringstream ss;
#ifdef _WIN32
                const char *tmp = std::getenv("TEMP");
                if (!tmp)
                    tmp = ".";
                ss << tmp << "\\sadinfo_bench_"
                   << std::chrono::steady_clock::now().time_since_epoch().count()
                   << ".log";
#else
                const char *tmp = std::getenv("TMPDIR");
                if (!tmp)
                    tmp = "/tmp";
                ss << tmp << "/sadinfo_bench_"
                   << std::chrono::steady_clock::now().time_since_epoch().count()
                   << ".log";
#endif
                tmp_path = ss.str();
            }
            std::ofstream fout(tmp_path, std::ios::binary | std::ios::trunc);
            CHECK(fout.is_open(), "تعذّر فتح ملف القياس المؤقت");
            if (!fout.is_open())
                return;

            Level prev = Logger::instance().level();
            Logger::instance().set_sink(&fout);
            Logger::instance().set_level(Level::INFO);

            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < kThroughputN; ++i)
            {
                log_info("bench", "msg",
                         {{"i", static_cast<int64_t>(i)}});
            }
            auto end = std::chrono::high_resolution_clock::now();

            Logger::instance().set_sink(nullptr); // restore stderr
            Logger::instance().set_level(prev);
            fout.close();

            double secs = std::chrono::duration<double>(end - start).count();
            double rate = kThroughputN / secs;
            std::cout << "  throughput (file):   " << static_cast<int64_t>(rate)
                      << " msg/sec (" << kThroughputN << " msgs in "
                      << secs << "s)\n";
            CHECK(rate >= kMinRatePerSec,
                  "throughput على ملف تحت 10k msg/s — flush المتزامن يفرط في التكلفة");

            // (AR) تنظيف الملف المؤقت.
            std::error_code ec;
            std::filesystem::remove(tmp_path, ec);
        }
    }

    void test_thread_safety()
    {
        SinkGuard g;
        Logger::instance().set_level(Level::INFO);

        std::vector<std::thread> ts;
        ts.reserve(kSafetyThreads);
        for (int t = 0; t < kSafetyThreads; ++t)
        {
            ts.emplace_back([t]
                            {
            for (int i = 0; i < kSafetyPerThread; ++i) {
                log_info("th", "x",
                         {{"tid", static_cast<int64_t>(t)},
                          {"i", static_cast<int64_t>(i)}});
            } });
        }
        for (auto &th : ts)
            th.join();

        // (AR) فحص integrity صارم:
        //      1) عدد الأسطر = THREADS * PER
        //      2) كل سطر يبدأ بـ '{' وينتهي بـ '}' قبل '\n' (لا تداخل)
        //      3) كل سطر يحوي "\"tid\":" مرة واحدة بالضبط
        // (EN) Strict integrity check: line count + per-line structural validity.
        std::string out = g.ss.str();
        size_t lines = 0;
        size_t pos = 0;
        bool all_well_formed = true;
        while (pos < out.size())
        {
            size_t nl = out.find('\n', pos);
            if (nl == std::string::npos)
                break;
            std::string_view line(out.data() + pos, nl - pos);
            if (line.empty() || line.front() != '{' || line.back() != '}')
                all_well_formed = false;
            // عدّ تكرار "tid" — يجب أن يكون 1 (لا تداخل سطرين)
            size_t tid_count = 0;
            size_t fp = 0;
            while ((fp = line.find("\"tid\":", fp)) != std::string_view::npos)
            {
                ++tid_count;
                fp += 6;
            }
            if (tid_count != 1)
                all_well_formed = false;
            ++lines;
            pos = nl + 1;
        }
        CHECK(lines == static_cast<size_t>(kSafetyThreads * kSafetyPerThread),
              "بعض الأسطر فُقدت أو تداخلت تحت المنافسة");
        CHECK(all_well_formed,
              "سطر واحد على الأقل تعرّض للتداخل أو فُقدت بنيته");
    }

    // (AR) اختبار عدم وجود data race بين set_level و log في خيوط متوازية.
    //      الإصلاح الحتمي: نُلوّح بين INFO و TRACE (كلاهما يسمح بـINFO)
    //      بدلاً من INFO/WARN — هذا يضمن أن جميع رسائلنا تُكتب،
    //      فيصبح "inspected > 0" حتمياً (ليس flaky).
    // (EN) Race test for set_level vs log on parallel threads.
    //      Deterministic fix: toggle between INFO and TRACE (both allow INFO)
    //      instead of INFO/WARN — guarantees every msg is written so
    //      "inspected > 0" is deterministic (not flaky).
    void test_concurrent_level_change()
    {
        SinkGuard g;
        Logger::instance().set_level(Level::INFO);

        std::atomic<bool> stop{false};
        std::thread toggler([&]
                            {
            while (!stop.load(std::memory_order_relaxed)) {
                Logger::instance().set_level(Level::TRACE);
                Logger::instance().set_level(Level::INFO);
            } });

        for (int i = 0; i < kRaceLogs; ++i)
            log_info("race", "x");

        stop.store(true, std::memory_order_relaxed);
        toggler.join();

        // (AR) أي سطر صدر يجب أن يكون JSON متوازن — التداخل سيكسر هذا.
        //      حماية من السطر الفارغ (nl == pos) لتجنب out[nl-1] خاطئ.
        // (EN) Any emitted line must be balanced JSON.
        //      Guard against empty line (nl == pos) to avoid bogus out[nl-1].
        std::string out = g.ss.str();
        size_t pos = 0;
        bool ok = true;
        size_t inspected = 0;
        while (pos < out.size())
        {
            size_t nl = out.find('\n', pos);
            if (nl == std::string::npos)
                break;
            if (nl == pos)
            {
                ok = false;
                break;
            } // سطر فارغ = تداخل
            if (out[pos] != '{' || out[nl - 1] != '}')
            {
                ok = false;
                break;
            }
            ++inspected;
            pos = nl + 1;
        }
        CHECK(ok, "ظهر سطر مشوّه أثناء race — احتمال تداخل تحت قفل الكتابة");
        CHECK(inspected > 0, "race test لم يُنتج أي سطر — مستوى INFO رُفِض بالكامل");
    }

} // namespace

int main()
{
    std::cout << "=== logger_tests ===\n";
    test_parse_level();
    test_level_filter();
    test_json_format();
    test_json_escape();
    test_utf8_passthrough();
    test_off_level();
    test_env_reload();
    test_throughput_10k();
    test_thread_safety();
    test_concurrent_level_change();

    if (g_failures == 0)
    {
        std::cout << "جميع الاختبارات مرّت ✓\n";
        return 0;
    }
    std::cout << g_failures << " اختبارات فشلت ✗\n";
    return 1;
}
