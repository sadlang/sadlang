// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: tests/sadinfo/test_keyword_transforms.cpp
// (AR) اختبارات وحدة لـ keyword_transforms (دوال نقية، بدون I/O).
//      يغطي: deriveStableId، isDeprecated، parseFilter، applyFilters،
//      groupByCategory، processEntry، getAllProcessed.
//      نتبع نمط المشروع: assert + exit-code، بدون gtest.
// (EN) Unit tests for keyword_transforms (pure functions, no I/O).
//      Follows project style: assert + exit-code, no gtest dependency.
// ═══════════════════════════════════════════════════════════════════════════════

#include "keyword_transforms.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace Sad::Info;

// ───────────────────────────────────────────────────────────────────────────────
// (AR) عدّاد اختبارات بسيط / (EN) Simple test counter
// ───────────────────────────────────────────────────────────────────────────────
static int g_pass = 0;
static int g_fail = 0;

#define EXPECT_TRUE(cond, msg)                                                \
    do                                                                        \
    {                                                                         \
        if (cond)                                                             \
        {                                                                     \
            ++g_pass;                                                         \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            ++g_fail;                                                         \
            std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__             \
                      << " — " << msg << "\n";                                \
        }                                                                     \
    } while (0)

#define EXPECT_EQ(a, b, msg)                                                  \
    do                                                                        \
    {                                                                         \
        auto _a = (a);                                                        \
        auto _b = (b);                                                        \
        if (_a == _b)                                                         \
        {                                                                     \
            ++g_pass;                                                         \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            ++g_fail;                                                         \
            std::cerr << "[FAIL] " << __FILE__ << ":" << __LINE__             \
                      << " — " << msg << "  expected '" << _b                 \
                      << "' got '" << _a << "'\n";                            \
        }                                                                     \
    } while (0)

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) 1) اختبارات isDeprecated
// ═══════════════════════════════════════════════════════════════════════════════
static void test_isDeprecated()
{
    EXPECT_TRUE(isDeprecated("give_deprecated"), "give_deprecated → true");
    EXPECT_TRUE(isDeprecated("foo_DEPRECATED"), "case-insensitive: _DEPRECATED → true");
    EXPECT_TRUE(!isDeprecated("function"), "function → false");
    EXPECT_TRUE(!isDeprecated("deprecated"), "exact 'deprecated' (no underscore) → false");
    EXPECT_TRUE(!isDeprecated(""), "empty → false");
    EXPECT_TRUE(!isDeprecated("dep"), "shorter than suffix → false");
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) 2) اختبارات deriveStableId
// ═══════════════════════════════════════════════════════════════════════════════
static void test_deriveStableId()
{
    using Cat  = Sad::Lexer::Generated::KeywordCategory;
    using TT   = Sad::Lexer::TokenType;

    // Reserved
    EXPECT_EQ(deriveStableId(Cat::RESERVED, "function", TT::KEYWORD_FUNCTION),
              std::string("keyword.function"), "RESERVED function");
    EXPECT_EQ(deriveStableId(Cat::RESERVED, "if", TT::KEYWORD_IF),
              std::string("keyword.if"), "RESERVED if");

    // Operator
    EXPECT_EQ(deriveStableId(Cat::OPERATOR, "and", TT::OP_AND),
              std::string("operator.and"), "OPERATOR and");

    // Contextual + suffix stripping
    EXPECT_EQ(deriveStableId(Cat::CONTEXTUAL, "give_deprecated", TT::IDENTIFIER),
              std::string("contextual.give"), "CONTEXTUAL strips _deprecated");
    EXPECT_EQ(deriveStableId(Cat::CONTEXTUAL, "async", TT::IDENTIFIER),
              std::string("contextual.async"), "CONTEXTUAL async");

    // Builtin types
    EXPECT_EQ(deriveStableId(Cat::BUILTIN_TYPE, "integer", TT::IDENTIFIER),
              std::string("type.integer"), "BUILTIN_TYPE integer");

    // LITERAL_* special-case → "literal.*"
    EXPECT_EQ(deriveStableId(Cat::RESERVED, "true", TT::LITERAL_TRUE),
              std::string("literal.true"), "LITERAL_TRUE → literal.true");
    EXPECT_EQ(deriveStableId(Cat::RESERVED, "false", TT::LITERAL_FALSE),
              std::string("literal.false"), "LITERAL_FALSE → literal.false");
    EXPECT_EQ(deriveStableId(Cat::RESERVED, "null", TT::LITERAL_NULL),
              std::string("literal.null"), "LITERAL_NULL → literal.null");

    // Spaces/dashes → snake_case
    EXPECT_EQ(deriveStableId(Cat::CONTEXTUAL, "End With", TT::IDENTIFIER),
              std::string("contextual.end_with"), "spaces → underscore");
    EXPECT_EQ(deriveStableId(Cat::CONTEXTUAL, "ui-decl", TT::IDENTIFIER),
              std::string("contextual.ui_decl"), "dash → underscore");
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) 3) اختبارات parseFilter
// ═══════════════════════════════════════════════════════════════════════════════
static void test_parseFilter()
{
    ParsedFilter pf;
    std::string err;

    // Valid: category=reserved
    EXPECT_TRUE(parseFilter("category=reserved", pf, err), "category=reserved valid");
    EXPECT_EQ(pf.key, std::string("category"), "key=category");
    EXPECT_EQ(pf.value, std::string("reserved"), "value=reserved");
    EXPECT_TRUE(pf.hasValue, "hasValue=true");

    // Valid: has-aliases (no value)
    EXPECT_TRUE(parseFilter("has-aliases", pf, err), "has-aliases valid without value");
    EXPECT_EQ(pf.key, std::string("has-aliases"), "key=has-aliases");
    EXPECT_TRUE(!pf.hasValue, "hasValue=false");

    // Invalid: empty
    EXPECT_TRUE(!parseFilter("", pf, err), "empty rejected");

    // Invalid: unknown key
    EXPECT_TRUE(!parseFilter("badkey", pf, err), "unknown key rejected");
    EXPECT_TRUE(err.find("badkey") != std::string::npos, "error mentions badkey");

    // Invalid: category without value
    EXPECT_TRUE(!parseFilter("category", pf, err), "category without = rejected");
    EXPECT_TRUE(err.find("category") != std::string::npos, "error mentions category");

    // Invalid: role without value
    EXPECT_TRUE(!parseFilter("role", pf, err), "role without = rejected");

    // Invalid: token-type without value
    EXPECT_TRUE(!parseFilter("token-type", pf, err), "token-type without = rejected");
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) 4) اختبارات getAllProcessed + إحصاءات الفئات
// ═══════════════════════════════════════════════════════════════════════════════
static void test_getAllProcessed()
{
    auto all = getAllProcessed();
    EXPECT_EQ(all.size(), static_cast<size_t>(91), "totalCount = 91");

    // عد كل فئة
    std::map<std::string, int> catCount;
    int deprecatedCount = 0;
    for (const auto &e : all)
    {
        catCount[e.category]++;
        if (e.deprecated) ++deprecatedCount;
    }
    EXPECT_EQ(catCount["reserved"], 40, "reserved = 40");
    EXPECT_EQ(catCount["operator"], 3, "operator = 3");
    EXPECT_EQ(catCount["contextual"], 39, "contextual = 39");
    EXPECT_EQ(catCount["builtin_type"], 9, "builtin_type = 9");
    EXPECT_EQ(deprecatedCount, 1, "exactly 1 deprecated");

    // التأكد من تفرّد الـ id
    std::set<std::string> ids;
    for (const auto &e : all)
        ids.insert(e.id);
    EXPECT_EQ(ids.size(), all.size(), "all ids unique");

    // التأكد من حضور deprecated.give كـ contextual.give
    auto it = std::find_if(all.begin(), all.end(), [](const ProcessedKeyword &p)
                           { return p.deprecated; });
    EXPECT_TRUE(it != all.end(), "find deprecated entry");
    if (it != all.end())
    {
        EXPECT_EQ(it->id, std::string("contextual.give"), "deprecated id = contextual.give");
        EXPECT_EQ(it->category, std::string("contextual"), "deprecated cat = contextual");
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) 5) اختبارات applyFilters
// ═══════════════════════════════════════════════════════════════════════════════
static void test_applyFilters()
{
    auto all = getAllProcessed();

    // (AR) بدون فلاتر، استبعاد deprecated
    {
        auto out = applyFilters(all, {}, /*excludeDeprecated=*/true);
        EXPECT_EQ(out.size(), static_cast<size_t>(90), "no filter, exclude deprecated → 90");
    }

    // (AR) بدون فلاتر، تضمين deprecated
    {
        auto out = applyFilters(all, {}, /*excludeDeprecated=*/false);
        EXPECT_EQ(out.size(), static_cast<size_t>(91), "no filter, include deprecated → 91");
    }

    // (AR) فلتر category=reserved
    {
        ParsedFilter f{"category", "reserved", true};
        auto out = applyFilters(all, {f}, true);
        EXPECT_EQ(out.size(), static_cast<size_t>(40), "category=reserved → 40");
    }

    // (AR) فلتر category=contextual بدون deprecated
    {
        ParsedFilter f{"category", "contextual", true};
        auto out = applyFilters(all, {f}, true);
        EXPECT_EQ(out.size(), static_cast<size_t>(38), "contextual w/o deprecated → 38");
    }

    // (AR) فلتر category=contextual مع deprecated
    {
        ParsedFilter f{"category", "contextual", true};
        auto out = applyFilters(all, {f}, false);
        EXPECT_EQ(out.size(), static_cast<size_t>(39), "contextual w/ deprecated → 39");
    }

    // (AR) فلتر category=foo (لا تطابق)
    {
        ParsedFilter f{"category", "foo", true};
        auto out = applyFilters(all, {f}, true);
        EXPECT_EQ(out.size(), static_cast<size_t>(0), "category=foo → 0");
    }

    // (AR) فلتر role=block_opener
    {
        ParsedFilter f{"role", "block_opener", true};
        auto out = applyFilters(all, {f}, true);
        // (AR) لا نفترض رقماً محدداً — فقط أن النتيجة > 0 وكلها لها الدور
        EXPECT_TRUE(out.size() > 0, "role=block_opener > 0");
        for (const auto &e : out)
        {
            EXPECT_TRUE(std::find(e.roles.begin(), e.roles.end(), "block_opener") != e.roles.end(),
                        "every entry has block_opener role");
        }
    }

    // (AR) AND منطقي: category=reserved + role=block_opener
    {
        ParsedFilter f1{"category", "reserved", true};
        ParsedFilter f2{"role", "block_opener", true};
        auto out = applyFilters(all, {f1, f2}, true);
        for (const auto &e : out)
        {
            EXPECT_EQ(e.category, std::string("reserved"), "AND: category=reserved");
            EXPECT_TRUE(std::find(e.roles.begin(), e.roles.end(), "block_opener") != e.roles.end(),
                        "AND: role=block_opener");
        }
    }

    // (AR) has-aliases (بدون قيمة)
    {
        ParsedFilter f{"has-aliases", "", false};
        auto out = applyFilters(all, {f}, true);
        for (const auto &e : out)
            EXPECT_TRUE(!e.aliases.empty(), "has-aliases: aliases not empty");
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) 6) اختبارات groupByCategory
// ═══════════════════════════════════════════════════════════════════════════════
static void test_groupByCategory()
{
    auto all = getAllProcessed();
    auto g   = groupByCategory(all);
    EXPECT_EQ(g.reserved.size(),    static_cast<size_t>(40), "grouped reserved=40");
    EXPECT_EQ(g.op.size(),          static_cast<size_t>(3),  "grouped operator=3");
    EXPECT_EQ(g.contextual.size(),  static_cast<size_t>(39), "grouped contextual=39");
    EXPECT_EQ(g.builtinType.size(), static_cast<size_t>(9),  "grouped builtin_type=9");

    // (AR) المجموع = 91
    EXPECT_EQ(g.reserved.size() + g.op.size() + g.contextual.size() + g.builtinType.size(),
              static_cast<size_t>(91), "grouped total = 91");
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) 7) اختبارات Idempotency: نداءان متتاليان يُنتجان نفس البيانات
// ═══════════════════════════════════════════════════════════════════════════════
static void test_idempotency()
{
    auto a = getAllProcessed();
    auto b = getAllProcessed();
    EXPECT_EQ(a.size(), b.size(), "idempotent size");
    for (size_t i = 0; i < a.size() && i < b.size(); ++i)
    {
        EXPECT_EQ(a[i].id, b[i].id, "idempotent id at position");
        EXPECT_EQ(a[i].word, b[i].word, "idempotent word at position");
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) المدخل الرئيسي / (EN) Main entry
// ═══════════════════════════════════════════════════════════════════════════════
int main()
{
    std::cout << "═══════════════════════════════════════════════════════════════\n";
    std::cout << " اختبارات وحدة: keyword_transforms (Story 1.2)\n";
    std::cout << " Unit tests: keyword_transforms (Story 1.2)\n";
    std::cout << "═══════════════════════════════════════════════════════════════\n";

    test_isDeprecated();
    test_deriveStableId();
    test_parseFilter();
    test_getAllProcessed();
    test_applyFilters();
    test_groupByCategory();
    test_idempotency();

    std::cout << "\n───────────────────────────────────────────────────────────────\n";
    std::cout << "  نتائج / Results: " << g_pass << " pass, " << g_fail << " fail\n";
    std::cout << "───────────────────────────────────────────────────────────────\n";

    return g_fail == 0 ? 0 : 1;
}
