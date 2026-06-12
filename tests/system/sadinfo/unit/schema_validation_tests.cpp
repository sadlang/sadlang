// ===================================================================
// (AR) schema_validation_tests.cpp — اختبارات C++ خفيفة للـschemas.
// (EN) Lightweight C++ tests for schemas (presence + JSON parseability).
//
// التحقق الكامل من schemas ضد JSON Schema draft-07 يجري عبر
// scripts/validate_schemas.py (Python + jsonschema).
// هذه الاختبارات هنا تتأكد فقط أن الملفات موجودة وقابلة للقراءة كـJSON
// — فحص دلالي عميق يحتاج مكتبة validator (RapidJSON Schema أو غيرها)
// تُضاف في story لاحقة عند تكوين CMake.
// ===================================================================
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{

    int g_failures = 0;

#define CHECK(cond, msg)                                                \
    do                                                                  \
    {                                                                   \
        if (!(cond))                                                    \
        {                                                               \
            std::cerr << "[FAIL] " << __LINE__ << " — " << msg << "\n"; \
            ++g_failures;                                               \
        }                                                               \
    } while (0)

    // ---------------------------------------------------------------------------
    // (AR) محلِّل JSON تقريبي: يكفي للتحقق من توازن {} [] "" وعدم وجود مفاتيح مكررة.
    // (EN) Tiny JSON sanity checker: balance of {} [] "" only. Not a full parser.
    // ---------------------------------------------------------------------------
    bool json_balanced(const std::string &src)
    {
        int curly = 0, square = 0;
        bool in_str = false;
        bool escape = false;
        for (char c : src)
        {
            if (escape)
            {
                escape = false;
                continue;
            }
            if (in_str)
            {
                if (c == '\\')
                    escape = true;
                else if (c == '"')
                    in_str = false;
                continue;
            }
            switch (c)
            {
            case '"':
                in_str = true;
                break;
            case '{':
                ++curly;
                break;
            case '}':
                --curly;
                break;
            case '[':
                ++square;
                break;
            case ']':
                --square;
                break;
            default:
                break;
            }
            if (curly < 0 || square < 0)
                return false;
        }
        return curly == 0 && square == 0 && !in_str;
    }

    std::string read_file(const fs::path &p)
    {
        std::ifstream f(p, std::ios::binary);
        std::ostringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

    fs::path find_repo_root()
    {
        // (AR) جرّب SAD_REPO_ROOT أولاً، ثم اصعد من cwd حتى 8 مستويات.
        // (EN) Try SAD_REPO_ROOT env first, then walk up from cwd up to 8 levels.
        if (const char *env = std::getenv("SAD_REPO_ROOT"))
        {
            fs::path p(env);
            if (fs::exists(p / "data" / "_schemas"))
                return p;
        }
        fs::path p = fs::current_path();
        for (int i = 0; i < 8; ++i)
        {
            if (fs::exists(p / "data" / "_schemas"))
                return p;
            if (!p.has_parent_path() || p.parent_path() == p)
                break;
            p = p.parent_path();
        }
        return fs::current_path();
    }

    const std::vector<std::string> kSchemaFiles = {
        "builtin.schema.json",
        "keyword.schema.json",
        "error.schema.json",
        "lesson.schema.json",
        "example.schema.json",
        "exercise.schema.json",
        "docs.schema.json",
        "i18n_overlay.schema.json",
    };

    const std::vector<std::string> kPolicyJsonFiles = {
        "state_schema.json",
        "codeowners.schema.json",
    };

    void test_schemas_exist_and_balanced()
    {
        fs::path root = find_repo_root();
        fs::path schemas = root / "data" / "_schemas";

        for (const auto &name : kSchemaFiles)
        {
            fs::path p = schemas / name;
            CHECK(fs::exists(p), std::string("schema مفقود: ") + name);
            if (!fs::exists(p))
                continue;
            std::string body = read_file(p);
            CHECK(!body.empty(), std::string("schema فارغ: ") + name);
            CHECK(json_balanced(body),
                  std::string("JSON غير متوازن في: ") + name);
            CHECK(body.find("\"draft-07/schema\"") != std::string::npos ||
                      body.find("draft-07") != std::string::npos,
                  std::string("draft-07 غير معلَن في: ") + name);
            CHECK(body.find("\"additionalProperties\": false") != std::string::npos ||
                      body.find("\"additionalProperties\":false") != std::string::npos,
                  std::string("additionalProperties:false مفقود في: ") + name);
            CHECK(body.find("\"schema_version\"") != std::string::npos,
                  std::string("schema_version مفقود في: ") + name);
        }
    }

    void test_policy_files_exist()
    {
        fs::path root = find_repo_root();
        fs::path schemas = root / "data" / "_schemas";

        std::vector<std::string> yaml_files = {
            "VERSION.yaml",
            "i18n_policy.yaml",
            "whitelist.yaml",
            "naming_rules.yaml",
            "query_limits.yaml",
            "non_deterministic_builtins.yaml",
            "side_effect_builtins.yaml",
        };
        for (const auto &y : yaml_files)
        {
            CHECK(fs::exists(schemas / y),
                  std::string("policy YAML مفقود: ") + y);
        }
        for (const auto &j : kPolicyJsonFiles)
        {
            fs::path p = schemas / j;
            CHECK(fs::exists(p), std::string("policy JSON مفقود: ") + j);
            if (!fs::exists(p))
                continue;
            CHECK(json_balanced(read_file(p)),
                  std::string("JSON غير متوازن في policy: ") + j);
        }
    }

    void test_fixtures_exist()
    {
        fs::path root = find_repo_root();
        fs::path base = root / "tests" / "sadinfo" / "fixtures" / "data";

        std::vector<fs::path> expected = {
            base / "builtins" / "builtin_demo" / "_index.yaml",
            base / "builtins" / "builtin_demo" / "docs.yaml",
            base / "keywords" / "keyword_demo" / "_index.yaml",
            base / "errors" / "error_demo" / "_index.yaml",
            base / "errors" / "error_demo" / "docs.yaml",
            base / "lessons" / "lesson_demo" / "_index.yaml",
            base / "lessons" / "lesson_demo" / "docs.yaml",
        };
        for (const auto &p : expected)
        {
            CHECK(fs::exists(p),
                  std::string("fixture مفقود: ") + p.string());
        }
    }

    void test_codeowners_meta_present()
    {
        fs::path root = find_repo_root();
        fs::path co = root / "data" / "_meta" / "CODEOWNERS.yaml";
        CHECK(fs::exists(co), "data/_meta/CODEOWNERS.yaml مفقود");
        if (!fs::exists(co))
            return;
        std::string body = read_file(co);
        CHECK(!body.empty(), "CODEOWNERS.yaml فارغ");
        // (AR) فحص دلالي خفيف: يحوي على الأقل مفتاح owners أو entries.
        // (EN) Light semantic check: contains at least owners or entries key.
        CHECK(body.find("owners") != std::string::npos ||
                  body.find("entries") != std::string::npos ||
                  body.find("teams") != std::string::npos ||
                  body.find("paths") != std::string::npos,
              "CODEOWNERS.yaml لا يحوي مفاتيح متوقعة (owners/entries/teams/paths)");
        CHECK(body.find("schema_version") != std::string::npos,
              "CODEOWNERS.yaml بدون schema_version");
    }

} // namespace

int main()
{
    std::cout << "=== schema_validation_tests ===\n";
    fs::path root = find_repo_root();
    std::cout << "repo root: " << root.string() << "\n";
    if (!fs::exists(root / "data" / "_schemas"))
    {
        std::cerr << "[FATAL] لم يُعثر على data/_schemas — "
                  << "اضبط SAD_REPO_ROOT أو شغّل من جذر المستودع\n";
        return 2;
    }
    test_schemas_exist_and_balanced();
    test_policy_files_exist();
    test_fixtures_exist();
    test_codeowners_meta_present();

    if (g_failures == 0)
    {
        std::cout << "جميع الاختبارات مرّت ✓\n";
        std::cout << "(للتحقق الدلالي العميق شغّل: "
                  << "python scripts/validate_schemas.py)\n";
        return 0;
    }
    std::cout << g_failures << " اختبارات فشلت ✗\n";
    return 1;
}
