// ===================================================================
// (AR) validator_tier1_tests.cpp — اختبارات Tier1Validator (T3 schema-driven).
// (EN) validator_tier1_tests.cpp — Tier1Validator unit tests (T3 schema-driven).
//
// مرجع: S-003 T6 (تغطية 8 error kinds + valid case).
//
// تغطية:
//   1. valid case (builtin كامل) → لا أخطاء.
//   2. MissingIndexFile.
//   3. InvalidYamlStructure (scalar root + parse error).
//   4. MissingField (حقول مطلوبة مفقودة).
//   5. WrongType (نوع غير صحيح).
//   6. IdMismatch (folder != id suffix).
//   7. WrongType على id pattern (uppercase).
//   8. UnsupportedSchema (schema_version=99).
//   9. InvalidEnum (kind غير صحيح / category غير مسموح).
//  10. DuplicateId + tracker clear.
//
// قواعد:
//   - BF-12: كل فحص اختبار يفشل قبل الإصلاح.
//   - CW-23: DI صريح للtracker + validate_with_schema للعزل عن registry.
// ===================================================================
#include "tier1_schema.h"
#include "validator.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

using namespace Sad::SadInfo;

// ──────────────────────────────────────────────────────────────────
// ASSERT helpers
// ──────────────────────────────────────────────────────────────────
static int g_failures = 0;

#define ASSERT_TRUE(cond, msg)                                \
    do                                                        \
    {                                                         \
        if (!(cond))                                          \
        {                                                     \
            std::fprintf(stderr, "FAIL [%s:%d] %s — !(%s)\n", \
                         __FILE__, __LINE__, msg, #cond);     \
            ++g_failures;                                     \
        }                                                     \
    } while (0)

#define ASSERT_EQ(a, b, msg)                                       \
    do                                                             \
    {                                                              \
        if (!((a) == (b)))                                         \
        {                                                          \
            std::fprintf(stderr, "FAIL [%s:%d] %s — (%s)!=(%s)\n", \
                         __FILE__, __LINE__, msg, #a, #b);         \
            ++g_failures;                                          \
        }                                                          \
    } while (0)

// ──────────────────────────────────────────────────────────────────
// (AR) بناء Entity للاختبار.
// ──────────────────────────────────────────────────────────────────
static Entity make_entity(EntityCategory cat,
                          const std::string &id,
                          const std::string &folder,
                          const std::string &index_yaml,
                          bool include_index = true)
{
    std::vector<RawYamlFile> files;
    if (include_index)
        files.push_back(RawYamlFile{"_index.yaml", index_yaml});
    return Entity(cat, id, folder, std::move(files));
}

static bool has_kind(const std::vector<ValidationError> &errs, Tier1ErrorKind k)
{
    for (const auto &e : errs)
        if (e.kind == k)
            return true;
    return false;
}

static int count_kind(const std::vector<ValidationError> &errs, Tier1ErrorKind k)
{
    int n = 0;
    for (const auto &e : errs)
        if (e.kind == k)
            ++n;
    return n;
}

// ──────────────────────────────────────────────────────────────────
// (AR) Schema يدوي للاختبار — مستقل عن ملفات data/_schemas.
//      يطابق tier1.builtin.yaml.
// ──────────────────────────────────────────────────────────────────
static Tier1Schema make_builtin_schema()
{
    Tier1Schema s;
    s.category = EntityCategory::Builtin;
    s.schema_versions_supported = {1};
    s.id_kind_prefix = "builtin";
    s.id_pattern = "^builtin\\.[a-z][a-z0-9_]*$";
    s.kind_required = "builtin";
    s.required_fields = {
        {"schema_version", Tier1FieldType::Int},
        {"id", Tier1FieldType::String},
        {"kind", Tier1FieldType::String},
        {"name", Tier1FieldType::String},
        {"category", Tier1FieldType::String},
        {"since", Tier1FieldType::String},
        {"signature", Tier1FieldType::Map},
        {"owners", Tier1FieldType::Sequence},
    };
    s.allowed_categories = {
        "io", "math", "string", "array", "map", "type",
        "concurrency", "system", "meta", "debug",
        "network", "graphics", "crypto"};
    return s;
}

// (AR) YAML صالح كامل لـbuiltin.
static const std::string kValidBuiltinYaml =
    "schema_version: 1\n"
    "id: builtin.demo\n"
    "kind: builtin\n"
    "name: تجريبي\n"
    "category: meta\n"
    "since: \"1.0.0\"\n"
    "signature:\n"
    "  params: []\n"
    "  returns: void\n"
    "owners:\n"
    "  - \"@sadlang-core\"\n";

// ──────────────────────────────────────────────────────────────────
// 1) valid case
// ──────────────────────────────────────────────────────────────────
static void test_valid_case()
{
    const auto schema = make_builtin_schema();
    Entity e = make_entity(EntityCategory::Builtin, "builtin.demo",
                           "data/builtins/demo", kValidBuiltinYaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    if (!errs.empty())
    {
        std::fprintf(stderr, "valid case unexpected errors:\n");
        for (const auto &er : errs)
        {
            std::fprintf(stderr, "  - kind=%d msg=%s\n",
                         static_cast<int>(er.kind), er.message.c_str());
        }
    }
    ASSERT_EQ(errs.size(), 0u, "valid case: no errors");
}

// ──────────────────────────────────────────────────────────────────
// 2) MissingIndexFile
// ──────────────────────────────────────────────────────────────────
static void test_missing_index()
{
    const auto schema = make_builtin_schema();
    Entity e = make_entity(EntityCategory::Builtin, "builtin.x",
                           "data/builtins/x", "", false);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::MissingIndexFile),
                "MissingIndexFile detected");
}

// ──────────────────────────────────────────────────────────────────
// 3) InvalidYamlStructure
// ──────────────────────────────────────────────────────────────────
static void test_invalid_yaml_scalar_root()
{
    const auto schema = make_builtin_schema();
    Entity e = make_entity(EntityCategory::Builtin, "builtin.x",
                           "data/builtins/x", "just a string\n");
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::InvalidYamlStructure),
                "scalar root → InvalidYamlStructure");
}

static void test_invalid_yaml_parse_error()
{
    const auto schema = make_builtin_schema();
    Entity e = make_entity(EntityCategory::Builtin, "builtin.x",
                           "data/builtins/x", "id: [unclosed\n");
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::InvalidYamlStructure),
                "parse error → InvalidYamlStructure");
}

// ──────────────────────────────────────────────────────────────────
// 4) MissingField
// ──────────────────────────────────────────────────────────────────
static void test_missing_required_fields()
{
    const auto schema = make_builtin_schema();
    // (AR) فقط id + schema_version — جميع الباقي مفقود.
    const std::string yaml =
        "schema_version: 1\n"
        "id: builtin.x\n";
    Entity e = make_entity(EntityCategory::Builtin, "builtin.x",
                           "data/builtins/x", yaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::MissingField),
                "missing fields → MissingField");
    // (AR) المتوقع: kind, name, category, since, signature, owners = 6.
    ASSERT_TRUE(count_kind(errs, Tier1ErrorKind::MissingField) >= 6,
                "at least 6 MissingField errors expected");
}

// ──────────────────────────────────────────────────────────────────
// 5) WrongType
// ──────────────────────────────────────────────────────────────────
static void test_schema_version_wrong_type()
{
    const auto schema = make_builtin_schema();
    const std::string yaml =
        "schema_version: \"not-a-number\"\n"
        "id: builtin.x\n"
        "kind: builtin\n"
        "name: x\n"
        "category: meta\n"
        "since: \"1.0.0\"\n"
        "signature: {}\n"
        "owners: []\n";
    Entity e = make_entity(EntityCategory::Builtin, "builtin.x",
                           "data/builtins/x", yaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::WrongType),
                "non-int schema_version → WrongType");
}

static void test_owners_wrong_type()
{
    const auto schema = make_builtin_schema();
    const std::string yaml =
        "schema_version: 1\n"
        "id: builtin.x\n"
        "kind: builtin\n"
        "name: x\n"
        "category: meta\n"
        "since: \"1.0.0\"\n"
        "signature: {}\n"
        "owners: \"single-string\"\n"; // (AR) المتوقع sequence
    Entity e = make_entity(EntityCategory::Builtin, "builtin.x",
                           "data/builtins/x", yaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::WrongType),
                "scalar owners → WrongType");
}

// ──────────────────────────────────────────────────────────────────
// 6) IdMismatch (folder != id suffix)
// ──────────────────────────────────────────────────────────────────
static void test_id_mismatch_folder()
{
    const auto schema = make_builtin_schema();
    Entity e = make_entity(EntityCategory::Builtin, "builtin.demo",
                           "data/builtins/different_folder", kValidBuiltinYaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::IdMismatch),
                "folder != id suffix → IdMismatch");
}

// ──────────────────────────────────────────────────────────────────
// 7) id pattern violation → WrongType
// ──────────────────────────────────────────────────────────────────
static void test_id_invalid_pattern()
{
    const auto schema = make_builtin_schema();
    // (AR) id بحرف كبير — يخالف ^builtin\\.[a-z][a-z0-9_]*$.
    const std::string yaml =
        "schema_version: 1\n"
        "id: builtin.DEMO\n"
        "kind: builtin\n"
        "name: x\n"
        "category: meta\n"
        "since: \"1.0.0\"\n"
        "signature: {}\n"
        "owners: []\n";
    Entity e = make_entity(EntityCategory::Builtin, "builtin.DEMO",
                           "data/builtins/DEMO", yaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::WrongType),
                "uppercase in id → WrongType");
}

// ──────────────────────────────────────────────────────────────────
// 8) UnsupportedSchema
// ──────────────────────────────────────────────────────────────────
static void test_unsupported_schema_version()
{
    const auto schema = make_builtin_schema();
    const std::string yaml =
        "schema_version: 99\n"
        "id: builtin.demo\n"
        "kind: builtin\n"
        "name: x\n"
        "category: meta\n"
        "since: \"1.0.0\"\n"
        "signature: {}\n"
        "owners: []\n";
    Entity e = make_entity(EntityCategory::Builtin, "builtin.demo",
                           "data/builtins/demo", yaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::UnsupportedSchema),
                "schema_version=99 → UnsupportedSchema");
}

// ──────────────────────────────────────────────────────────────────
// 9) InvalidEnum
// ──────────────────────────────────────────────────────────────────
static void test_invalid_kind()
{
    const auto schema = make_builtin_schema();
    const std::string yaml =
        "schema_version: 1\n"
        "id: builtin.demo\n"
        "kind: keyword\n" // (AR) خاطئ — المتوقع builtin.
        "name: x\n"
        "category: meta\n"
        "since: \"1.0.0\"\n"
        "signature: {}\n"
        "owners: []\n";
    Entity e = make_entity(EntityCategory::Builtin, "builtin.demo",
                           "data/builtins/demo", yaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::InvalidEnum),
                "kind=keyword for builtin → InvalidEnum");
}

static void test_invalid_category()
{
    const auto schema = make_builtin_schema();
    const std::string yaml =
        "schema_version: 1\n"
        "id: builtin.demo\n"
        "kind: builtin\n"
        "name: x\n"
        "category: not_allowed_category\n" // (AR) خارج allowed_categories.
        "since: \"1.0.0\"\n"
        "signature: {}\n"
        "owners: []\n";
    Entity e = make_entity(EntityCategory::Builtin, "builtin.demo",
                           "data/builtins/demo", yaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate_with_schema(e, schema, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::InvalidEnum),
                "invalid category → InvalidEnum");
}

// ──────────────────────────────────────────────────────────────────
// 10) DuplicateId + tracker
// ──────────────────────────────────────────────────────────────────
static void test_duplicate_id()
{
    const auto schema = make_builtin_schema();
    DuplicateIdTracker tr;
    Entity e1 = make_entity(EntityCategory::Builtin, "builtin.demo",
                            "data/builtins/demo", kValidBuiltinYaml);
    auto errs1 = Tier1Validator::validate_with_schema(e1, schema, tr);
    ASSERT_EQ(errs1.size(), 0u, "first entity: no errors");
    Entity e2 = make_entity(EntityCategory::Builtin, "builtin.demo",
                            "data/builtins/demo", kValidBuiltinYaml);
    auto errs2 = Tier1Validator::validate_with_schema(e2, schema, tr);
    ASSERT_TRUE(has_kind(errs2, Tier1ErrorKind::DuplicateId),
                "second occurrence → DuplicateId");
}

static void test_duplicate_tracker_clear()
{
    DuplicateIdTracker tr;
    ASSERT_TRUE(!tr.register_or_check("a"), "first register returns false");
    ASSERT_TRUE(tr.register_or_check("a"), "second register returns true");
    ASSERT_EQ(tr.size(), 1u, "tracker size = 1");
    tr.clear();
    ASSERT_EQ(tr.size(), 0u, "after clear size = 0");
    ASSERT_TRUE(!tr.register_or_check("a"), "after clear, 'a' is fresh");
}

// ──────────────────────────────────────────────────────────────────
// 11) Registry: unloaded category → UnsupportedSchema (مع validate العام)
// ──────────────────────────────────────────────────────────────────
static void test_unloaded_registry()
{
    // (AR) لم نُحمِّل registry — يجب أن يُعيد UnsupportedSchema.
    Entity e = make_entity(EntityCategory::Builtin, "builtin.demo",
                           "data/builtins/demo", kValidBuiltinYaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate(e, tr);
    ASSERT_TRUE(has_kind(errs, Tier1ErrorKind::UnsupportedSchema),
                "no schema loaded → UnsupportedSchema");
}

// ──────────────────────────────────────────────────────────────────
// 12) Registry load + use end-to-end
// ──────────────────────────────────────────────────────────────────
static void test_registry_load_and_validate()
{
    const char *root = std::getenv("SAD_REPO_ROOT");
    if (root == nullptr)
    {
        std::fprintf(stderr,
                     "SKIP test_registry_load_and_validate: "
                     "SAD_REPO_ROOT not set\n");
        return;
    }
    const std::string dir = std::string(root) + "/data/_schemas";
    auto &reg = Tier1SchemaRegistry::instance();
    const bool ok = reg.load_from_directory(dir);
    if (!ok)
    {
        std::fprintf(stderr,
                     "FAIL: registry load_from_directory(%s) returned false\n",
                     dir.c_str());
        ++g_failures;
        return;
    }
    ASSERT_TRUE(reg.is_loaded(), "all 4 schemas loaded");
    ASSERT_TRUE(reg.get(EntityCategory::Builtin) != nullptr,
                "builtin schema present");
    ASSERT_TRUE(reg.get(EntityCategory::Keyword) != nullptr,
                "keyword schema present");
    ASSERT_TRUE(reg.get(EntityCategory::Error) != nullptr,
                "error schema present");
    ASSERT_TRUE(reg.get(EntityCategory::Lesson) != nullptr,
                "lesson schema present");

    // (AR) الآن validate العام يجب أن ينجح للـvalid case.
    Entity e = make_entity(EntityCategory::Builtin, "builtin.demo",
                           "data/builtins/demo", kValidBuiltinYaml);
    DuplicateIdTracker tr;
    auto errs = Tier1Validator::validate(e, tr);
    if (!errs.empty())
    {
        std::fprintf(stderr, "registry-driven valid case errors:\n");
        for (const auto &er : errs)
        {
            std::fprintf(stderr, "  - kind=%d msg=%s\n",
                         static_cast<int>(er.kind), er.message.c_str());
        }
    }
    ASSERT_EQ(errs.size(), 0u,
              "registry-driven valid case: no errors");
}

// ──────────────────────────────────────────────────────────────────
// main
// ──────────────────────────────────────────────────────────────────
int main()
{
    // (AR) ملاحظة: test_unloaded_registry يجب أن يأتي قبل
    //      test_registry_load_and_validate لضمان حالة "غير محمَّل".
    test_unloaded_registry();

    test_valid_case();
    test_missing_index();
    test_invalid_yaml_scalar_root();
    test_invalid_yaml_parse_error();
    test_missing_required_fields();
    test_schema_version_wrong_type();
    test_owners_wrong_type();
    test_id_mismatch_folder();
    test_id_invalid_pattern();
    test_unsupported_schema_version();
    test_invalid_kind();
    test_invalid_category();
    test_duplicate_id();
    test_duplicate_tracker_clear();

    test_registry_load_and_validate();

    if (g_failures == 0)
    {
        std::printf("[validator_tier1_tests] ALL PASSED\n");
        return 0;
    }
    std::fprintf(stderr, "[validator_tier1_tests] %d FAILURES\n", g_failures);
    return 1;
}
