// ===================================================================
// (AR) loader_tests.cpp — اختبارات وحدة Loader (S-001 T9)
// (EN) loader_tests.cpp — Loader unit tests (S-001 T9)
//
// النمط: plain main() + exit codes (لا GoogleTest) — يطابق
//        sadinfo_logger_tests و sadinfo_schemas_tests.
// التغطية:
//   * is_valid_utf8_no_bom: ASCII, UTF-8 صحيح، BOM، تسلسلات خاطئة، overlong
//   * entity_id_is_nfc: ASCII فقط
//   * Loader::scan على mini_data: 4 كيانات بدون أخطاء + 1 lesson MissingRequiredFile
//   * Loader::scan على bad_data: InvalidYaml + UnknownFile + InvalidEncoding + MissingRequiredFile
//   * LoaderOptions::stop_on_first_error
//   * المسار غير موجود → خطأ واحد
// ===================================================================
#include "entity.h"
#include "entity_error.h"
#include "loader.h"
#include "logging.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;
using namespace Sad::SadInfo;

namespace
{
    int g_fails = 0;

#define CHECK(cond, msg)                                              \
    do                                                                \
    {                                                                 \
        if (!(cond))                                                  \
        {                                                             \
            std::cerr << "[FAIL] " << __FUNCTION__ << ":" << __LINE__ \
                      << " — " << (msg) << "\n";                      \
            ++g_fails;                                                \
        }                                                             \
    } while (0)

    // (AR) جذر الـfixtures يأتي من متغير البيئة SAD_REPO_ROOT لتجنب
    //      الاعتماد على PWD أثناء ctest.
    std::string repo_root()
    {
        const char *r = std::getenv("SAD_REPO_ROOT");
        return r ? std::string(r) : std::string(".");
    }

    std::string mini_path() { return repo_root() + "/tests/sadinfo/fixtures/mini_data"; }
    std::string bad_path() { return repo_root() + "/tests/sadinfo/fixtures/bad_data"; }
    std::string validated_path() { return repo_root() + "/tests/sadinfo/fixtures/validated_data"; }

    int count_errors(const std::vector<EntityOrError> &v)
    {
        int n = 0;
        for (const auto &e : v)
            if (std::holds_alternative<EntityError>(e))
                ++n;
        return n;
    }

    int count_entities(const std::vector<EntityOrError> &v)
    {
        int n = 0;
        for (const auto &e : v)
            if (std::holds_alternative<Entity>(e))
                ++n;
        return n;
    }

    bool has_error_kind(const std::vector<EntityOrError> &v, EntityErrorKind k)
    {
        for (const auto &e : v)
        {
            if (std::holds_alternative<EntityError>(e) &&
                std::get<EntityError>(e).kind == k)
                return true;
        }
        return false;
    }

    // ──────────────────────────────────────────────────────────────
    // اختبارات is_valid_utf8_no_bom
    // ──────────────────────────────────────────────────────────────
    void test_utf8_ascii()
    {
        CHECK(is_valid_utf8_no_bom("hello"), "ASCII");
        CHECK(is_valid_utf8_no_bom(""), "empty");
    }

    void test_utf8_valid()
    {
        // (AR) "مرحبا" بـUTF-8
        const std::string ar = "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7";
        CHECK(is_valid_utf8_no_bom(ar), "Arabic UTF-8");
    }

    void test_utf8_bom_rejected()
    {
        std::string s = "\xEF\xBB\xBFhello";
        CHECK(!is_valid_utf8_no_bom(s), "BOM must be rejected");
    }

    void test_utf8_invalid_seq()
    {
        // (AR) بايت متابعة بدون بداية
        CHECK(!is_valid_utf8_no_bom(std::string("\x80", 1)), "stray continuation");
        // (AR) متسلسلة 2-byte مقطوعة
        CHECK(!is_valid_utf8_no_bom(std::string("\xC2", 1)), "truncated 2-byte");
        // (AR) overlong لـASCII
        CHECK(!is_valid_utf8_no_bom(std::string("\xC0\x80", 2)), "overlong null");
        // (AR) surrogate
        CHECK(!is_valid_utf8_no_bom(std::string("\xED\xA0\x80", 3)), "surrogate U+D800");
    }

    // ──────────────────────────────────────────────────────────────
    // اختبارات entity_id_is_nfc
    // ──────────────────────────────────────────────────────────────
    void test_id_nfc()
    {
        CHECK(entity_id_is_nfc("print_line"), "ASCII id");
        CHECK(!entity_id_is_nfc("اطبع"), "non-ASCII id rejected");
    }

    // ──────────────────────────────────────────────────────────────
    // اختبارات Loader::scan
    // ──────────────────────────────────────────────────────────────
    void test_scan_mini_data_happy()
    {
        Loader ld;
        auto v = ld.scan(mini_path());
        // (AR) 3 كيانات ناجحة: builtin_print_line, keyword_if, error_e001
        //      + 1 خطأ MissingRequiredFile لـlesson_intro
        CHECK(count_entities(v) == 3, "3 valid entities expected");
        CHECK(count_errors(v) == 1, "1 missing-index error expected");
        CHECK(has_error_kind(v, EntityErrorKind::MissingRequiredFile),
              "MissingRequiredFile kind present");
    }

    void test_scan_entity_files()
    {
        Loader ld;
        auto v = ld.scan(mini_path());
        const Entity *print_line = nullptr;
        for (const auto &e : v)
        {
            if (std::holds_alternative<Entity>(e))
            {
                const auto &en = std::get<Entity>(e);
                if (en.id() == "builtin_print_line")
                    print_line = &en;
            }
        }
        CHECK(print_line != nullptr, "builtin_print_line entity found");
        if (!print_line)
            return;
        CHECK(print_line->category() == EntityCategory::Builtin, "category");
        // (AR) ملفات: _index.yaml + examples/basic.yaml + i18n/ar.yaml
        CHECK(print_line->files().size() == 3, "3 files collected");
        CHECK(print_line->find_file("_index.yaml") != nullptr, "_index present");
        CHECK(print_line->find_file("examples/basic.yaml") != nullptr, "example present");
        CHECK(print_line->find_file("i18n/ar.yaml") != nullptr, "i18n present");
        CHECK(print_line->find_file("nope.yaml") == nullptr, "missing returns nullptr");
    }

    void test_scan_bad_data_errors()
    {
        Loader ld;
        auto v = ld.scan(bad_path());
        // (AR) متوقَّع 4 أخطاء: invalid yaml (إن كان yaml-cpp موجوداً)،
        //      unknown file، missing index، BOM (InvalidEncoding).
        //      نتسامح: على الأقل 3 أخطاء + الأنواع المتوقعة.
        CHECK(count_errors(v) >= 3, "at least 3 errors in bad_data");
        CHECK(has_error_kind(v, EntityErrorKind::UnknownFile),
              "UnknownFile (notes.txt)");
        CHECK(has_error_kind(v, EntityErrorKind::MissingRequiredFile),
              "MissingRequiredFile (no _index)");
        CHECK(has_error_kind(v, EntityErrorKind::InvalidEncoding),
              "InvalidEncoding (BOM)");
        // (AR) InvalidYaml يعتمد على yaml-cpp — لا نشترطه
    }

    void test_stop_on_first_error()
    {
        Loader ld;
        LoaderOptions opts;
        opts.stop_on_first_error = true;
        auto v = ld.scan(bad_path(), opts);
        // (AR) عند أول خطأ يجب أن يُرجع النتائج فوراً.
        //      النتيجة الأخيرة يجب أن تكون خطأ.
        CHECK(!v.empty(), "at least one entry returned");
        if (v.empty())
            return;
        CHECK(std::holds_alternative<EntityError>(v.back()),
              "stop_on_first_error: last entry is error");
        CHECK(count_errors(v) == 1, "exactly one error captured");
    }

    void test_missing_root()
    {
        Loader ld;
        auto v = ld.scan(repo_root() + "/no_such_directory_xyz");
        CHECK(v.size() == 1, "single error for missing root");
        CHECK(std::holds_alternative<EntityError>(v.front()), "root error returned");
    }

    // ─────────────────────────────────────────────────────────────
    // (AR) اختبارات CR-1: تغطية إصلاحات المراجعة
    // ─────────────────────────────────────────────────────────────

    // (AR) إعادة استخدام نفس الـLoader لجذور مختلفة (R-1): يجب ألا تتسرّب
    //      whitelist بين الاستدعاءات.
    void test_scan_reuse_loader()
    {
        Loader ld;
        auto v1 = ld.scan(mini_path());
        CHECK(count_entities(v1) == 3, "first scan: 3 entities");

        // (AR) جذر غير موجود بينهما — يجب ألا يتأثر scan الثالث.
        auto v2 = ld.scan(repo_root() + "/no_such_directory_xyz");
        CHECK(v2.size() == 1, "reused loader: missing root reports 1 error");

        auto v3 = ld.scan(mini_path());
        CHECK(count_entities(v3) == 3, "third scan still 3 entities");
        CHECK(count_errors(v3) == 1, "third scan still 1 error (lesson_intro)");
    }

    // (AR) R-4: حد 16 MiB — ننشئ ملف تجريبياً > 16 MiB ونتأكد
    //      أن الـLoader يرفضه بـInvalidEncoding.
    void test_scan_file_size_limit()
    {
        const fs::path tmp = fs::path(repo_root()) /
                             "tests/sadinfo/fixtures/_tmp_oversized";
        const fs::path ent = tmp / "builtins" / "builtin_huge";
        std::error_code ec;
        fs::remove_all(tmp, ec);
        fs::create_directories(ent, ec);
        CHECK(!ec, "create tmp dir");

        // (AR) نكتب 17 MiB من أصفار ASCII صالحة UTF-8.
        {
            std::ofstream out(ent / "_index.yaml", std::ios::binary);
            const std::string chunk(1024 * 1024, '#'); // 1 MiB sharp
            for (int i = 0; i < 17; ++i)
                out << chunk;
        }

        Loader ld;
        auto v = ld.scan(tmp.string());
        CHECK(has_error_kind(v, EntityErrorKind::InvalidEncoding),
              "oversized file rejected as InvalidEncoding");

        fs::remove_all(tmp, ec);
    }

#ifndef _WIN32
    // (AR) R-2: symlinks — نتخطّى الاختبار على Windows لأن إنشاء
    //      symlinks يتطلّب صلاحيات خاصة (developer mode).
    void test_scan_rejects_symlinks()
    {
        const fs::path tmp = fs::path(repo_root()) /
                             "tests/sadinfo/fixtures/_tmp_symlink";
        const fs::path ent = tmp / "builtins" / "builtin_sym";
        std::error_code ec;
        fs::remove_all(tmp, ec);
        fs::create_directories(ent, ec);

        {
            std::ofstream out(ent / "_index.yaml");
            out << "id: builtin_sym\n";
        }
        // (AR) أنشئ ملف خارج الكيان ثم symlink إليه داخله.
        const fs::path outside = tmp / "outside.yaml";
        {
            std::ofstream o(outside);
            o << "secret\n";
        }
        fs::create_symlink(outside, ent / "docs.yaml", ec);
        if (ec)
        {
            fs::remove_all(tmp);
            return;
        } // لا صلاحية — تخطّى

        Loader ld;
        auto v = ld.scan(tmp.string());
        CHECK(has_error_kind(v, EntityErrorKind::UnknownFile),
              "symlink rejected as UnknownFile");

        fs::remove_all(tmp, ec);
    }
#endif

    // ──────────────────────────────────────────────────────────────
    // اختبارات to_string
    // ──────────────────────────────────────────────────────────────
    void test_to_string()
    {
        CHECK(to_string(EntityCategory::Builtin) == "builtin", "builtin");
        CHECK(to_string(EntityCategory::Keyword) == "keyword", "keyword");
        CHECK(to_string(EntityCategory::Error) == "error", "error");
        CHECK(to_string(EntityCategory::Lesson) == "lesson", "lesson");
        CHECK(to_string(EntityErrorKind::InvalidYaml) == "InvalidYaml",
              "InvalidYaml string");
    }

    // ──────────────────────────────────────────────────────────────
    // (AR) T7: تكامل Loader + Tier1Validator على validated_data
    //       يتحقق من أن SchemaViolation يُطلق على الكيانات المكسورة
    //       وأن الكيانات الصالحة تمر بدون أخطاء، عبر الفئات الأربع.
    //
    //   bucket المتوقع:
    //     builtins/ : 1 صالح (demo) + 3 مكسور (missing/mismatch/version)
    //     keywords/ : 2 صالح (demo, dup)
    //     errors/   : 1 صالح (demo) + 1 InvalidEnum (bad_sev)
    //     lessons/  : 1 صالح (demo) + 1 UnsupportedSchema (unsupported)
    //   المجموع: 5 ناجح + 5 SchemaViolation = 10 كيان.
    //
    //   ملاحظة (Tier1 design): DuplicateId غير قابل للتفعيل من القرص
    //   لأن Entity::id() مشتق من اسم المجلد، واسم المجلد فريد في FS.
    //   يبقى الفحص مفيداً للسيناريوهات البرمجية التي تنشئ Entity يدوياً.
    // ──────────────────────────────────────────────────────────────
    void test_scan_validated_data_schema_violations()
    {
        Loader loader;
        const auto results = loader.scan(validated_path());

        const int ents = count_entities(results);
        const int errs = count_errors(results);

        CHECK(ents == 5, "validated_data: 5 entities مرّت بنجاح");
        CHECK(errs == 5, "validated_data: 5 SchemaViolation");

        int schema_violations = 0;
        bool seen_missing_field = false;
        bool seen_id_mismatch = false;
        bool seen_unsupported_schema = false;
        bool seen_invalid_enum = false;

        for (const auto &r : results)
        {
            if (!std::holds_alternative<EntityError>(r))
                continue;
            const auto &e = std::get<EntityError>(r);
            if (e.kind == EntityErrorKind::SchemaViolation)
                ++schema_violations;

            if (e.message.find("missing_field") != std::string::npos)
                seen_missing_field = true;
            if (e.message.find("id_mismatch") != std::string::npos)
                seen_id_mismatch = true;
            if (e.message.find("unsupported_schema") != std::string::npos)
                seen_unsupported_schema = true;
            if (e.message.find("invalid_enum") != std::string::npos)
                seen_invalid_enum = true;
        }
        CHECK(schema_violations == 5,
              "كل الأخطاء من نوع SchemaViolation");
        CHECK(seen_missing_field, "Tier1: missing_field رُصد");
        CHECK(seen_id_mismatch, "Tier1: id_mismatch رُصد");
        CHECK(seen_unsupported_schema, "Tier1: unsupported_schema رُصد");
        CHECK(seen_invalid_enum, "Tier1: invalid_enum رُصد");
    }

} // namespace

int main()
{
    // (AR) قمع Logger أثناء الاختبارات لتنظيف الخرج.
    std::ostringstream sink;
    Logger::instance().set_sink(&sink);
    Logger::instance().set_level(Level::OFF);

    test_utf8_ascii();
    test_utf8_valid();
    test_utf8_bom_rejected();
    test_utf8_invalid_seq();
    test_id_nfc();
    test_to_string();
    test_scan_mini_data_happy();
    test_scan_entity_files();
    test_scan_bad_data_errors();
    test_stop_on_first_error();
    test_missing_root();
    test_scan_reuse_loader();
    test_scan_file_size_limit();
#ifndef _WIN32
    test_scan_rejects_symlinks();
#endif
    test_scan_validated_data_schema_violations();

    if (g_fails == 0)
    {
        std::cout << "[OK] all loader tests passed\n";
        return 0;
    }
    std::cerr << "[FAIL] " << g_fails << " loader checks failed\n";
    return 1;
}
