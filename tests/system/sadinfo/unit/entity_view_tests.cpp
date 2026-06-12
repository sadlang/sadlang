// ===================================================================
// (AR) entity_view_tests.cpp — اختبارات S-002 (T6, T8).
// (EN) entity_view_tests.cpp — S-002 tests (T6, T8).
//
// التغطية:
//   * AC-1: std::array<Field, 32> sparse → fill + clear.
//   * AC-2: Field { name_id; value }.
//   * AC-3: FieldNameTable singleton + thread-safe (نمط أساسي).
//   * AC-4: get بـname_id lookup + linear scan.
//   * AC-5: from_entity/to_entity.
//   * AC-6: sizeof(EntityView) < 1KB (static_assert في .cpp).
//   * Stress (T8): 1000 entities × 50 field access.
//
// قواعد:
//   - استخدام plain main + exit codes (لا GoogleTest — تماشياً مع S-001).
// ===================================================================
#include "entity_view.h"
#include "field_name_table.h"

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

using namespace Sad::SadInfo;

// ─────────────────────────────────────────────────────────────────
// (AR) أدوات assertion بسيطة.
// ─────────────────────────────────────────────────────────────────
static int g_failures = 0;
#define ASSERT_TRUE(expr)                                                  \
    do                                                                     \
    {                                                                      \
        if (!(expr))                                                       \
        {                                                                  \
            std::fprintf(stderr, "FAIL: %s (line %d)\n", #expr, __LINE__); \
            ++g_failures;                                                  \
        }                                                                  \
    } while (0)

#define ASSERT_EQ(a, b)                                                           \
    do                                                                            \
    {                                                                             \
        if (!((a) == (b)))                                                        \
        {                                                                         \
            std::fprintf(stderr, "FAIL: %s == %s (line %d)\n", #a, #b, __LINE__); \
            ++g_failures;                                                         \
        }                                                                         \
    } while (0)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

// ─────────────────────────────────────────────────────────────────
// (AR) T6.1: FieldNameTable — intern + name_of + idempotency.
// ─────────────────────────────────────────────────────────────────
static void test_field_name_table_basic()
{
    auto &t = FieldNameTable::instance();
    const auto id1 = t.intern("ar.name");
    ASSERT_TRUE(id1 != kEmptyFieldNameId);

    const auto id2 = t.intern("ar.name"); // idempotent
    ASSERT_EQ(id1, id2);

    const auto id3 = t.intern("en.name");
    ASSERT_TRUE(id3 != id1);

    ASSERT_EQ(t.name_of(id1), std::string_view{"ar.name"});
    ASSERT_EQ(t.name_of(id3), std::string_view{"en.name"});

    // (AR) sentinel/invalid → فارغ.
    ASSERT_TRUE(t.name_of(kEmptyFieldNameId).empty());

    // (AR) intern("") → 0.
    ASSERT_EQ(t.intern(""), kEmptyFieldNameId);
}

// ─────────────────────────────────────────────────────────────────
// (AR) T6.2: FieldNameTable — concurrent intern (نمط Stress).
// ─────────────────────────────────────────────────────────────────
static void test_field_name_table_concurrent()
{
    auto &t = FieldNameTable::instance();
    constexpr int kThreads = 8;
    constexpr int kPerThread = 200;

    std::vector<std::thread> threads;
    std::atomic<int> non_zero{0};

    for (int ti = 0; ti < kThreads; ++ti)
    {
        threads.emplace_back([&, ti]()
                             {
            for (int i = 0; i < kPerThread; ++i)
            {
                const std::string name = "concurrent.field_" + std::to_string(i);
                const auto id = t.intern(name);
                if (id != kEmptyFieldNameId)
                {
                    ++non_zero;
                }
            } });
    }
    for (auto &th : threads)
    {
        th.join();
    }
    ASSERT_EQ(non_zero.load(), kThreads * kPerThread);
}

// ─────────────────────────────────────────────────────────────────
// (AR) T6.3: EntityView — set/get/contains/size/clear.
// ─────────────────────────────────────────────────────────────────
static void test_entity_view_basic()
{
    EntityView v;
    ASSERT_EQ(v.size(), 0u);
    ASSERT_EQ(v.capacity(), kEntityViewCapacity);
    ASSERT_TRUE(!v.contains("anything"));

    ASSERT_EQ(v.set("ar.name", FieldValue{std::string{"اطبع"}}),
              FieldOpStatus::Ok);
    ASSERT_EQ(v.size(), 1u);
    ASSERT_TRUE(v.contains("ar.name"));

    auto val = v.get("ar.name");
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(std::holds_alternative<std::string>(*val));
    ASSERT_EQ(std::get<std::string>(*val), std::string{"اطبع"});

    // (AR) استبدال — لا يزيد العدّاد.
    ASSERT_EQ(v.set("ar.name", FieldValue{std::string{"اطبع_سطر"}}),
              FieldOpStatus::Ok);
    ASSERT_EQ(v.size(), 1u);
    ASSERT_EQ(std::get<std::string>(*v.get("ar.name")),
              std::string{"اطبع_سطر"});

    // (AR) أنواع متعددة.
    v.set("arity", FieldValue{std::int64_t{2}});
    v.set("is_variadic", FieldValue{true});
    v.set("ratio", FieldValue{0.5});
    ASSERT_EQ(v.size(), 4u);

    ASSERT_EQ(std::get<std::int64_t>(*v.get("arity")), std::int64_t{2});
    ASSERT_EQ(std::get<bool>(*v.get("is_variadic")), true);
    ASSERT_EQ(std::get<double>(*v.get("ratio")), 0.5);

    v.clear();
    ASSERT_EQ(v.size(), 0u);
    ASSERT_TRUE(!v.contains("ar.name"));
}

// ─────────────────────────────────────────────────────────────────
// (AR) T6.4: EntityView — السعة ممتلئة → Full.
// ─────────────────────────────────────────────────────────────────
static void test_entity_view_full()
{
    EntityView v;
    for (std::size_t i = 0; i < kEntityViewCapacity; ++i)
    {
        const std::string n = "f_" + std::to_string(i);
        ASSERT_EQ(v.set(n, FieldValue{static_cast<std::int64_t>(i)}),
                  FieldOpStatus::Ok);
    }
    ASSERT_EQ(v.size(), kEntityViewCapacity);

    // (AR) الإدخال الـ33 → Full.
    const auto st = v.set("overflow", FieldValue{std::int64_t{99}});
    ASSERT_EQ(st, FieldOpStatus::Full);

    // (AR) الاستبدال على اسم موجود — لا يزال Ok حتى عند الامتلاء.
    ASSERT_EQ(v.set("f_0", FieldValue{std::int64_t{100}}),
              FieldOpStatus::Ok);
    ASSERT_EQ(std::get<std::int64_t>(*v.get("f_0")), std::int64_t{100});
}

// ─────────────────────────────────────────────────────────────────
// (AR) T6.5: from_entity / to_entity (Round-trip للحقول الأساسية).
// ─────────────────────────────────────────────────────────────────
static void test_entity_view_bridges()
{
    std::vector<RawYamlFile> files;
    files.push_back(RawYamlFile{"_index.yaml", "id: print_line\n"});
    Entity e(EntityCategory::Builtin,
             std::string{"print_line"},
             std::string{"data/builtins/print_line"},
             std::move(files));

    auto view = EntityView::from_entity(e);
    ASSERT_TRUE(view.contains("category"));
    ASSERT_TRUE(view.contains("entity_id"));
    ASSERT_EQ(std::get<std::string>(*view.get("category")),
              std::string{"builtin"});
    ASSERT_EQ(std::get<std::string>(*view.get("entity_id")),
              std::string{"print_line"});

    view.set("arity", FieldValue{std::int64_t{2}});

    auto rebuilt = view.to_entity(EntityCategory::Builtin,
                                  std::string{"print_line"},
                                  std::string{"data/builtins/print_line"});
    ASSERT_EQ(rebuilt.category(), EntityCategory::Builtin);
    ASSERT_EQ(rebuilt.id(), std::string_view{"print_line"});

    // (AR) ملف synthetic موجود.
    ASSERT_EQ(rebuilt.files().size(), 1u);
    ASSERT_EQ(rebuilt.files()[0].relative_path,
              std::string{"synthetic.yaml"});
    ASSERT_TRUE(rebuilt.files()[0].content.find("category:") !=
                std::string::npos);
}

// ─────────────────────────────────────────────────────────────────
// (AR) T8: Stress — 1000 entities × 50 field access.
// ─────────────────────────────────────────────────────────────────
static void test_stress_1000_x_50()
{
    constexpr int kEntities = 1000;
    constexpr int kFieldsPerEntity = 20; // ≤ 32

    // (AR) أنشئ الأسماء مسبقاً لتجنّب تخصيصات داخل القياس.
    std::vector<std::string> names;
    names.reserve(kFieldsPerEntity);
    for (int i = 0; i < kFieldsPerEntity; ++i)
    {
        names.emplace_back("stress.field_" + std::to_string(i));
    }

    std::vector<EntityView> views;
    views.reserve(kEntities);

    for (int e = 0; e < kEntities; ++e)
    {
        EntityView v;
        for (int f = 0; f < kFieldsPerEntity; ++f)
        {
            v.set(names[f], FieldValue{static_cast<std::int64_t>(e * 100 + f)});
        }
        views.push_back(std::move(v));
    }

    // (AR) 50 وصول للحقل لكل كيان.
    std::int64_t checksum = 0;
    for (int e = 0; e < kEntities; ++e)
    {
        for (int k = 0; k < 50; ++k)
        {
            const auto &n = names[k % kFieldsPerEntity];
            auto v = views[e].get(n);
            ASSERT_TRUE(v.has_value());
            checksum += std::get<std::int64_t>(*v);
        }
    }
    // (AR) نضمن أن المترجم لا يحذف الحلقة.
    ASSERT_TRUE(checksum != 0);
}

// ─────────────────────────────────────────────────────────────────
// (AR) جديد (CR): التأكد أن get/contains لاسم غير موجود لا يلوّث
//      جدول الاستيطان (intern_existing بدل intern).
// (EN) NEW (CR): get/contains for a missing name must NOT pollute the
//      intern table.
// ─────────────────────────────────────────────────────────────────
static void test_read_no_pollution()
{
    auto &t = FieldNameTable::instance();
    const std::size_t before = t.size();

    EntityView v;
    v.set("real.existing", FieldValue{std::int64_t{1}});

    // (AR) أسماء فريدة لم تُستوطَن من قبل — استعلامات قراءة فقط.
    ASSERT_FALSE(v.contains("cr.ghost_field_1"));
    ASSERT_FALSE(v.contains("cr.ghost_field_2"));
    auto opt = v.get("cr.ghost_field_3");
    ASSERT_TRUE(!opt.has_value());

    const std::size_t after = t.size();
    // (AR) واحد فقط أُضيف: "real.existing". الأشباح يجب أن تبقى خارج الجدول.
    // (EN) Only "real.existing" should be added; ghosts must stay out.
    ASSERT_EQ(after, before + 1);
}

// ─────────────────────────────────────────────────────────────────
// (AR) جديد (CR): erase يزيل الحقل ويُحافظ على contiguous prefix.
// (EN) NEW (CR): erase removes the field and preserves contiguous prefix.
// ─────────────────────────────────────────────────────────────────
static void test_erase()
{
    EntityView v;
    v.set("a", FieldValue{std::int64_t{1}});
    v.set("b", FieldValue{std::int64_t{2}});
    v.set("c", FieldValue{std::int64_t{3}});
    ASSERT_EQ(v.size(), static_cast<std::size_t>(3));

    // (AR) احذف الأوسط.
    ASSERT_EQ(v.erase("b"), FieldOpStatus::Ok);
    ASSERT_EQ(v.size(), static_cast<std::size_t>(2));
    ASSERT_FALSE(v.contains("b"));
    ASSERT_TRUE(v.contains("a"));
    ASSERT_TRUE(v.contains("c"));

    // (AR) لا فجوات: أول خانتين فقط مشغولتان.
    const auto &raw = v.raw_fields();
    ASSERT_TRUE(raw[0].name_id != kEmptyFieldNameId);
    ASSERT_TRUE(raw[1].name_id != kEmptyFieldNameId);
    ASSERT_EQ(raw[2].name_id, kEmptyFieldNameId);

    // (AR) حذف اسم غير موجود = NotFound.
    ASSERT_EQ(v.erase("nope"), FieldOpStatus::NotFound);

    // (AR) أفرغ الكل ثم اضبط مرة أخرى — لا يجب أن يبقى أي أثر.
    ASSERT_EQ(v.erase("a"), FieldOpStatus::Ok);
    ASSERT_EQ(v.erase("c"), FieldOpStatus::Ok);
    ASSERT_EQ(v.size(), static_cast<std::size_t>(0));
    ASSERT_EQ(v.set("z", FieldValue{std::int64_t{9}}), FieldOpStatus::Ok);
    ASSERT_EQ(v.size(), static_cast<std::size_t>(1));

    // (AR) Round-2 CR — حالات سلبية (BF-22):
    //  - double-erase لنفس الاسم
    //  - erase("") = NotFound
    //  - erase ثم set ثم erase + التأكد من contiguous prefix
    EntityView w;
    w.set("k1", FieldValue{std::int64_t{1}});
    ASSERT_EQ(w.erase("k1"), FieldOpStatus::Ok);
    ASSERT_EQ(w.erase("k1"), FieldOpStatus::NotFound); // double-erase
    ASSERT_EQ(w.erase(""), FieldOpStatus::NotFound);   // empty name
    ASSERT_EQ(w.size(), static_cast<std::size_t>(0));

    // (AR) contiguous prefix بعد erase الأول.
    EntityView x;
    x.set("p", FieldValue{std::int64_t{1}});
    x.set("q", FieldValue{std::int64_t{2}});
    x.set("r", FieldValue{std::int64_t{3}});
    x.set("s", FieldValue{std::int64_t{4}});
    ASSERT_EQ(x.erase("p"), FieldOpStatus::Ok);
    // (AR) الآن أول 3 خانات يجب أن تكون مشغولة، البقية فارغة.
    const auto &rx = x.raw_fields();
    ASSERT_TRUE(rx[0].name_id != kEmptyFieldNameId);
    ASSERT_TRUE(rx[1].name_id != kEmptyFieldNameId);
    ASSERT_TRUE(rx[2].name_id != kEmptyFieldNameId);
    ASSERT_EQ(rx[3].name_id, kEmptyFieldNameId);
    ASSERT_EQ(x.size(), static_cast<std::size_t>(3));
}

// ─────────────────────────────────────────────────────────────────
int main()
{
    test_field_name_table_basic();
    test_field_name_table_concurrent();
    test_entity_view_basic();
    test_entity_view_full();
    test_entity_view_bridges();
    test_stress_1000_x_50();
    test_read_no_pollution();
    test_erase();

    if (g_failures == 0)
    {
        std::printf("[ENTITY_VIEW] all tests PASSED\n");
        return 0;
    }
    std::fprintf(stderr, "[ENTITY_VIEW] %d FAILURE(S)\n", g_failures);
    return 1;
}
