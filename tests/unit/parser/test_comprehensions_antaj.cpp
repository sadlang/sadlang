/**
 * @file test_comprehensions_antaj.cpp
 * @brief (AR) اختبارات وحدة للمحلّل النحويّ: صياغة الاستيعابات بترتيب «أنتج» العربيّ (RFC 25 م1ب).
 *        (EN) Parser unit tests for the Arabic «أنتج» comprehension order (RFC 25 م1ب).
 *
 * (AR) يتحقّق من أنّ المحلّل يبني عقد AST الصحيحة (List/Set/Dict ComprehensionExpr) بالترتيب
 *      الجديد «[لكل س في مصدر [إذا شرط] أنتج ناتج]»، ويميّز القاموس عن المجموعة عبر «:» بعد
 *      الناتج، ويرفض الترتيب البايثونيّ القديم وكلّ الصيغ الناقصة. طبقة الوحدة (لا تنفيذ).
 * (EN) Verifies the parser builds the right AST nodes in the new order, disambiguates dict vs
 *      set via a trailing ':', and rejects the old python order and malformed forms. Unit layer.
 */

#include "sad_test.h"
#include "parser_core.h"
#include "lexer_core.h"
#include "expressions.h"
#include "statements.h"
#include "ast_node.h"
#include "error_manager.h"

#include <string>

using namespace Sad;
using namespace Sad::AST;

// ─────────────────────────────────────────────────────────────────────────────
// أدوات مساعدة / Helpers
// ─────────────────────────────────────────────────────────────────────────────

// (AR) يحلّل نصًّا ويعيد قائمة الجمل (تملك العقد — تبقى حيّة طوال الاختبار).
static AST::StmtList parseProg(const std::string &code)
{
    Errors::ErrorManager::getInstance().clear();
    Lexer::LexerCore lexer(code);
    Parser::ParserCore parser(lexer);
    return parser.parseProgram();
}

// (AR) يستخرج تعبير المبدئ من «متغير ن = <تعبير>» أو تعبير جملة عاديّة.
static Expression *initOf(AST::StmtList &prog)
{
    if (prog.empty())
        return nullptr;
    if (auto *vd = dynamic_cast<VarDeclStmt *>(prog[0].get()))
        return vd->initializer.get();
    if (auto *es = dynamic_cast<ExprStmt *>(prog[0].get()))
        return es->expression.get();
    return nullptr;
}

static ListComprehensionExpr *asList(AST::StmtList &p) { return dynamic_cast<ListComprehensionExpr *>(initOf(p)); }
static SetComprehensionExpr *asSet(AST::StmtList &p) { return dynamic_cast<SetComprehensionExpr *>(initOf(p)); }
static DictComprehensionExpr *asDict(AST::StmtList &p) { return dynamic_cast<DictComprehensionExpr *>(initOf(p)); }
static ArrayExpr *asArray(AST::StmtList &p) { return dynamic_cast<ArrayExpr *>(initOf(p)); }
static MapExpr *asMap(AST::StmtList &p) { return dynamic_cast<MapExpr *>(initOf(p)); }

static const std::string V = "\xd8\xb3"; // «س» بترميز UTF-8
static const std::string V2 = "\xd9\x86"; // «ن»

// ═════════════════════════════════════════════════════════════════════════════
// (1) استيعاب القائمة — الترتيب الجديد يبني ListComprehensionExpr صحيحًا
// ═════════════════════════════════════════════════════════════════════════════

TEST(ListCompParse, identity)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3] أنتج س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_EQ(lc->variable, V);
    ASSERT_NOT_NULL(lc->element.get());
    ASSERT_NOT_NULL(lc->iterable.get());
    ASSERT_NULL(lc->condition.get());
}

TEST(ListCompParse, map_double)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3] أنتج س * 2]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_EQ(lc->variable, V);
    ASSERT_NULL(lc->condition.get());
}

TEST(ListCompParse, map_square)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3، 4] أنتج س * س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(lc->element.get());
}

TEST(ListCompParse, with_condition_gt)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3، 4] إذا س > 2 أنتج س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(lc->condition.get());
}

TEST(ListCompParse, with_condition_even)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3، 4] إذا س % 2 == 0 أنتج س * 10]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(lc->condition.get());
    ASSERT_NOT_NULL(lc->element.get());
}

TEST(ListCompParse, condition_then_map)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3] إذا س < 3 أنتج س + 1]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(lc->condition.get());
}

TEST(ListCompParse, var_name_preserved)
{
    auto p = parseProg("متغير ن = [لكل عنصر في [1، 2] أنتج عنصر]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_EQ(lc->variable, std::string("\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1")); // «عنصر»
}

TEST(ListCompParse, empty_source)
{
    auto p = parseProg("متغير ن = [لكل س في [] أنتج س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
}

TEST(ListCompParse, single_element_source)
{
    auto p = parseProg("متغير ن = [لكل س في [7] أنتج س * 3]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
}

TEST(ListCompParse, nested_array_output)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3] أنتج [س، س * س]]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(lc->element.get());
}

TEST(ListCompParse, iterable_is_variable)
{
    auto p = parseProg("متغير ق = [1، 2، 3]\nمتغير ن = [لكل س في ق أنتج س]");
    // البيان الثاني
    ASSERT_TRUE(p.size() >= 2);
    auto *vd = dynamic_cast<VarDeclStmt *>(p[1].get());
    ASSERT_NOT_NULL(vd);
    auto *lc = dynamic_cast<ListComprehensionExpr *>(vd->initializer.get());
    ASSERT_NOT_NULL(lc);
}

TEST(ListCompParse, output_uses_var_twice)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3] أنتج س * س + س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
}

TEST(ListCompParse, condition_compound_and)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3، 4، 5] إذا س > 1 و س < 5 أنتج س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(lc->condition.get());
}

TEST(ListCompParse, output_subtraction)
{
    auto p = parseProg("متغير ن = [لكل س في [5، 6، 7] أنتج س - 1]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
}

TEST(ListCompParse, output_modulo)
{
    auto p = parseProg("متغير ن = [لكل س في [10، 11، 12] أنتج س % 3]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
}

// ═════════════════════════════════════════════════════════════════════════════
// (2) استيعاب المجموعة — «{ ... أنتج ناتج }» (لا «:») يبني SetComprehensionExpr
// ═════════════════════════════════════════════════════════════════════════════

TEST(SetCompParse, identity)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 2، 3] أنتج س}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_EQ(sc->variable, V);
    ASSERT_NOT_NULL(sc->expression.get());
    ASSERT_NULL(sc->condition.get());
}

TEST(SetCompParse, map_output)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] أنتج س * 10}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(sc->expression.get());
}

TEST(SetCompParse, with_condition)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3، 4] إذا س > 2 أنتج س}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(sc->condition.get());
}

TEST(SetCompParse, condition_and_map)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3، 4] إذا س % 2 == 1 أنتج س * س}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(sc->condition.get());
    ASSERT_NOT_NULL(sc->expression.get());
}

TEST(SetCompParse, not_a_dict)
{
    // لا «:» بعد الناتج ⇒ مجموعة لا قاموس
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س + 1}");
    ASSERT_NOT_NULL(asSet(p));
    ASSERT_NULL(asDict(p));
}

TEST(SetCompParse, var_preserved)
{
    auto p = parseProg("متغير ن = {لكل ك في [1، 2] أنتج ك}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_EQ(sc->variable, std::string("\xd9\x83")); // «ك»
}

TEST(SetCompParse, single_source)
{
    auto p = parseProg("متغير ن = {لكل س في [9] أنتج س}");
    ASSERT_NOT_NULL(asSet(p));
}

TEST(SetCompParse, all_duplicates)
{
    auto p = parseProg("متغير ن = {لكل س في [5، 5، 5] أنتج س}");
    ASSERT_NOT_NULL(asSet(p));
}

TEST(SetCompParse, output_expression_present)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] أنتج س * 2 + 1}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(sc->expression.get());
}

TEST(SetCompParse, condition_ne)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] إذا س != 2 أنتج س}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(sc->condition.get());
}

// ═════════════════════════════════════════════════════════════════════════════
// (3) استيعاب القاموس — «{ ... أنتج مفتاح: قيمة }» يبني DictComprehensionExpr
// ═════════════════════════════════════════════════════════════════════════════

TEST(DictCompParse, basic_kv)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] أنتج س: س * س}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_EQ(dc->variable, V);
    ASSERT_NOT_NULL(dc->key.get());
    ASSERT_NOT_NULL(dc->value.get());
    ASSERT_NULL(dc->condition.get());
}

TEST(DictCompParse, string_key)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] أنتج نص(س): س * 10}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->key.get());
    ASSERT_NOT_NULL(dc->value.get());
}

TEST(DictCompParse, with_condition)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] إذا س > 1 أنتج س: س}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->condition.get());
}

TEST(DictCompParse, condition_and_kv)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3، 4] إذا س % 2 == 0 أنتج نص(س): س * 100}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->condition.get());
    ASSERT_NOT_NULL(dc->key.get());
    ASSERT_NOT_NULL(dc->value.get());
}

TEST(DictCompParse, key_value_distinct)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س + 1: س - 1}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->key.get());
    ASSERT_NOT_NULL(dc->value.get());
}

TEST(DictCompParse, not_a_set)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س: س}");
    ASSERT_NOT_NULL(asDict(p));
    ASSERT_NULL(asSet(p));
}

TEST(DictCompParse, single_var_only)
{
    // الترتيب الجديد: متغيّر حلقة مفرد
    auto p = parseProg("متغير ن = {لكل م في [1، 2، 3] أنتج م: م * 2}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_EQ(dc->variable, std::string("\xd9\x85")); // «م»
}

TEST(DictCompParse, value_uses_var)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] أنتج س: س * س + 1}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->value.get());
}

TEST(DictCompParse, condition_lt)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3، 4] إذا س < 3 أنتج نص(س): س}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->condition.get());
}

TEST(DictCompParse, single_pair_source)
{
    auto p = parseProg("متغير ن = {لكل س في [7] أنتج نص(س): س}");
    ASSERT_NOT_NULL(asDict(p));
}

// ═════════════════════════════════════════════════════════════════════════════
// (4) التمييز: «:» بعد الناتج ⇒ قاموس، غيابها ⇒ مجموعة
// ═════════════════════════════════════════════════════════════════════════════

TEST(Disambiguate, colon_makes_dict)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س: س}");
    ASSERT_NOT_NULL(asDict(p));
    ASSERT_NULL(asSet(p));
}

TEST(Disambiguate, no_colon_makes_set)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س}");
    ASSERT_NOT_NULL(asSet(p));
    ASSERT_NULL(asDict(p));
}

TEST(Disambiguate, colon_with_condition_dict)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] إذا س > 0 أنتج س: س * 2}");
    ASSERT_NOT_NULL(asDict(p));
}

TEST(Disambiguate, no_colon_with_condition_set)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] إذا س > 0 أنتج س * 2}");
    ASSERT_NOT_NULL(asSet(p));
}

// ═════════════════════════════════════════════════════════════════════════════
// (5) العاديّ لا يُساء تحليله كاستيعاب (مصفوفة/خريطة)
// ═════════════════════════════════════════════════════════════════════════════

TEST(RegularNotComp, plain_array)
{
    auto p = parseProg("متغير ن = [1، 2، 3]");
    ASSERT_NOT_NULL(asArray(p));
    ASSERT_NULL(asList(p));
}

TEST(RegularNotComp, empty_array)
{
    auto p = parseProg("متغير ن = []");
    ASSERT_NOT_NULL(asArray(p));
    ASSERT_NULL(asList(p));
}

TEST(RegularNotComp, plain_map)
{
    auto p = parseProg("متغير ن = {\"أ\": 1، \"ب\": 2}");
    ASSERT_NOT_NULL(asMap(p));
    ASSERT_NULL(asDict(p));
}

TEST(RegularNotComp, single_element_array)
{
    auto p = parseProg("متغير ن = [42]");
    ASSERT_NOT_NULL(asArray(p));
    ASSERT_NULL(asList(p));
}

TEST(RegularNotComp, array_of_expressions)
{
    auto p = parseProg("متغير ن = [1 + 1، 2 * 2، 3]");
    ASSERT_NOT_NULL(asArray(p));
    ASSERT_NULL(asList(p));
}

TEST(RegularNotComp, map_with_assign_syntax)
{
    auto p = parseProg("متغير ن = {\"أ\" = 1}");
    ASSERT_NOT_NULL(asMap(p));
    ASSERT_NULL(asDict(p));
}

// ═════════════════════════════════════════════════════════════════════════════
// (6) رفض الترتيب القديم والصيغ الناقصة (لا يُبنى استيعاب)
// ═════════════════════════════════════════════════════════════════════════════

TEST(NegativeReject, list_old_python_order)
{
    auto p = parseProg("متغير ن = [س * 2 لكل س في [1، 2، 3]]");
    ASSERT_NULL(asList(p)); // الترتيب القديم لم يعد يُبنى كاستيعاب
}

TEST(NegativeReject, list_no_yield)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3]]");
    ASSERT_NULL(asList(p));
}

TEST(NegativeReject, list_no_in)
{
    auto p = parseProg("متغير ن = [لكل س [1، 2، 3] أنتج س]");
    ASSERT_NULL(asList(p));
}

TEST(NegativeReject, list_no_var)
{
    auto p = parseProg("متغير ن = [لكل في [1، 2، 3] أنتج س]");
    ASSERT_NULL(asList(p));
}

TEST(NegativeReject, set_old_python_order)
{
    auto p = parseProg("متغير ن = {س * 2 لكل س في [1، 2، 3]}");
    ASSERT_NULL(asSet(p));
}

TEST(NegativeReject, set_no_yield)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] س}");
    ASSERT_NULL(asSet(p));
}

TEST(NegativeReject, dict_old_python_order)
{
    auto p = parseProg("متغير ن = {نص(س): س لكل س في [1، 2، 3]}");
    ASSERT_NULL(asDict(p));
}

TEST(NegativeReject, dict_no_yield)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] نص(س): س}");
    ASSERT_NULL(asDict(p));
}

TEST(NegativeReject, list_yield_no_output)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3] أنتج]");
    ASSERT_NULL(asList(p));
}

TEST(NegativeReject, set_no_in)
{
    auto p = parseProg("متغير ن = {لكل س [1، 2، 3] أنتج س}");
    ASSERT_NULL(asSet(p));
}

TEST(NegativeReject, dict_no_colon)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] أنتج نص(س) س}");
    // بلا «:» بعد الناتج: قد يُفسَّر كمجموعة أو يفشل — لكنّه ليس قاموسًا صحيحًا بمفتاح:قيمة
    ASSERT_NULL(asDict(p));
}

// ═════════════════════════════════════════════════════════════════════════════
// (7) حالات حدّيّة إضافيّة على مستوى AST
// ═════════════════════════════════════════════════════════════════════════════

TEST(EdgeCases, list_getType_is_array)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2] أنتج س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(lc->getType().get());
}

TEST(EdgeCases, set_getType_is_array)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(sc->getType().get());
}

TEST(EdgeCases, dict_getType_is_map)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س: س}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->getType().get());
}

TEST(EdgeCases, list_condition_null_without_if)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2] أنتج س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NULL(lc->condition.get());
}

TEST(EdgeCases, set_condition_null_without_if)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NULL(sc->condition.get());
}

TEST(EdgeCases, dict_condition_null_without_if)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س: س}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NULL(dc->condition.get());
}

TEST(EdgeCases, list_condition_present_with_if)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2] إذا س > 0 أنتج س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(lc->condition.get());
}

TEST(EdgeCases, list_large_source)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3، 4، 5، 6، 7، 8] أنتج س * 2]");
    ASSERT_NOT_NULL(asList(p));
}

TEST(EdgeCases, set_large_source)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3، 4، 5، 6، 7، 8] أنتج س % 3}");
    ASSERT_NOT_NULL(asSet(p));
}

TEST(EdgeCases, dict_large_source)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3، 4، 5] أنتج نص(س): س * س}");
    ASSERT_NOT_NULL(asDict(p));
}

TEST(EdgeCases, list_zero_in_source)
{
    auto p = parseProg("متغير ن = [لكل س في [0، 1، 2] أنتج س]");
    ASSERT_NOT_NULL(asList(p));
}

TEST(EdgeCases, list_negation_output)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3] أنتج 0 - س]");
    ASSERT_NOT_NULL(asList(p));
}

TEST(EdgeCases, set_condition_ge)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3، 4] إذا س >= 3 أنتج س}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(sc->condition.get());
}

TEST(EdgeCases, dict_condition_ne)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3] إذا س != 2 أنتج نص(س): س}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->condition.get());
}

// ═════════════════════════════════════════════════════════════════════════════
// (8) ثبات المتغيّر ونوع العقدة عبر تنويعات المصدر
// ═════════════════════════════════════════════════════════════════════════════

TEST(Structure, list_variable_ascii_name)
{
    auto p = parseProg("متغير ن = [لكل i في [1، 2] أنتج i]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_EQ(lc->variable, std::string("i"));
}

TEST(Structure, set_variable_ascii_name)
{
    auto p = parseProg("متغير ن = {لكل x في [1، 2] أنتج x}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_EQ(sc->variable, std::string("x"));
}

TEST(Structure, dict_variable_ascii_name)
{
    auto p = parseProg("متغير ن = {لكل k في [1، 2] أنتج k: k}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_EQ(dc->variable, std::string("k"));
}

TEST(Structure, list_element_not_null_all_forms)
{
    auto p = parseProg("متغير ن = [لكل س في [1] أنتج س + س * س - 1]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(lc->element.get());
    ASSERT_NOT_NULL(lc->iterable.get());
}

TEST(Structure, dict_key_and_value_not_null)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج نص(س): س + 1}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->key.get());
    ASSERT_NOT_NULL(dc->value.get());
    ASSERT_NOT_NULL(dc->iterable.get());
}

TEST(Structure, set_expression_and_iterable_not_null)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س * 3}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(sc->expression.get());
    ASSERT_NOT_NULL(sc->iterable.get());
}

// ═════════════════════════════════════════════════════════════════════════════
// (9) بنية تعبير الناتج — يجب أن يُبنى فعلًا لا يُسقَط (مراجعة Amelia #4)
//     التأكيد على «not-null» وحده يمرّ لو أسقط المحلّل العمليّة وخزّن المعرّف فقط؛
//     هنا نتحقّق أنّ ناتج الخريطة عقدة ثنائيّة (BinaryExpr) لا معرّفًا مجرّدًا.
// ═════════════════════════════════════════════════════════════════════════════

TEST(OutputStructure, list_map_output_is_binary)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2] أنتج س * 2]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(dynamic_cast<BinaryExpr *>(lc->element.get()));
}

TEST(OutputStructure, list_identity_output_not_binary)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2] أنتج س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NULL(dynamic_cast<BinaryExpr *>(lc->element.get())); // معرّف مجرّد لا عمليّة
}

TEST(OutputStructure, set_map_output_is_binary)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س * 10}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(dynamic_cast<BinaryExpr *>(sc->expression.get()));
}

TEST(OutputStructure, set_modulo_output_is_binary)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3، 4] أنتج س % 3}");
    auto *sc = asSet(p);
    ASSERT_NOT_NULL(sc);
    ASSERT_NOT_NULL(dynamic_cast<BinaryExpr *>(sc->expression.get()));
}

TEST(OutputStructure, dict_value_is_binary_key_not)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2] أنتج س: س * س}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dynamic_cast<BinaryExpr *>(dc->value.get()));
    ASSERT_NULL(dynamic_cast<BinaryExpr *>(dc->key.get())); // المفتاح «س» معرّف مجرّد
}

TEST(OutputStructure, list_condition_is_binary)
{
    auto p = parseProg("متغير ن = [لكل س في [1، 2، 3] إذا س > 2 أنتج س]");
    auto *lc = asList(p);
    ASSERT_NOT_NULL(lc);
    ASSERT_NOT_NULL(dynamic_cast<BinaryExpr *>(lc->condition.get()));
}

TEST(OutputStructure, dict_folding_key_is_binary)
{
    auto p = parseProg("متغير ن = {لكل س في [1، 2، 3، 4] أنتج نص(س % 2): س}");
    auto *dc = asDict(p);
    ASSERT_NOT_NULL(dc);
    ASSERT_NOT_NULL(dc->key.get());
    ASSERT_NOT_NULL(dc->value.get());
}

// ═════════════════════════════════════════════════════════════════════════════
// (10) البانِي (main)
// ═════════════════════════════════════════════════════════════════════════════

int main(int argc, char **argv)
{
    return SAD_RUN_TESTS();
}
