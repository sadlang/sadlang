// بسم الله الرحمن الرحيم
// ======================================================================
// test_ui_phase0.cpp — اختبارات القبول لقصص المرحلة 0 من SadUI
// ======================================================================
// الوصف:
//   يختبر هذا الملف جميع معايير القبول لقصص Phase 0:
//   - UI-1.1: رسم شرطي (إذا/وإلا) داخل الحاويات
//   - UI-1.2: حلقات رسم (لكل/بينما) داخل الحاويات
//   - UI-1.3: تقليص knownWidgets إلى 15 + إزالة dynamic_cast
//
//   يستخدم ParserCore مباشرة لتحليل كود ص وفحص شجرة AST الناتجة.
//
// English Description:
//   Tests all acceptance criteria for Phase 0 stories:
//   - UI-1.1: Conditional rendering (if/else) inside containers
//   - UI-1.2: Loop rendering (for-each/while) inside containers
//   - UI-1.3: Reduce knownWidgets to 15 + remove dynamic_cast
//
//   Uses ParserCore directly to parse Sad code and inspect the resulting AST.
// ======================================================================

#include "sad_test_framework.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "ui_nodes.h"
#include "declarations.h"
#include "expressions.h"
#include "statements.h"
// (AR) قائمتا الأوّليّات/الحاويات المولَّدتان من language-truth/ui_nodes.yaml —
//      مصدرُ أسماءِ العُقَدِ في هذا الاختبار، فلا اسمَ يدويٌّ يبيت.
// (EN) SoT-generated primitive/container lists — the node names used here.
#include "generated/ui_parser_nodes_generated.h"

#include <string>
#include <unordered_set>
#include <vector>
#include <iostream>

using namespace SadTest;
using namespace Sad;
using namespace Sad::AST;
using namespace Sad::Lexer;
using namespace Sad::Parser;

// (AR) مدخلةُ اسمٍ من قوائمِ X-macro المولَّدة (تُستعمَل داخلَ مُهيِّئاتِ الحاويات).
// (EN) Name entry for the generated X-macro lists.
#define SAD_UI_TEST_NAME_ENTRY(Id, Name) Name,

// (AR) عنصرُ النصّ — الابنُ المستعمَلُ في اختباراتِ الحاويات. يُشتقّ من الرأسِ
//      المولَّدِ (أوّلُ عقدةٍ اسمُها هو الاسمُ القانونيُّ لعنصرِ النصّ) لا من نصٍّ يدويّ.
// (EN) The Text widget's canonical name, taken from the generated header.
static const std::string kTextWidget = SAD_UI_PARSER_NODE_Text;

// (AR) عددا ADR-UI-02 (١٥ أوّليًّا، سبعةٌ منها حاويات) — قرارٌ معماريٌّ يُحرَس
//      صراحةً هنا وفي scripts/codegen/check_ui_props_consistency.py.
// (EN) ADR-UI-02's counts — an architectural decision, asserted explicitly.
static constexpr size_t kAdrPrimitiveCount = 15;
static constexpr size_t kAdrContainerCount = 7;

// =====================================================================
// (AR) دوال مساعدة للاختبار / (EN) Test helper functions
// =====================================================================

/**
 * @brief (AR) يحلل كود ص ويُرجع قائمة الجمل — يفشل الاختبار إذا كان هناك أخطاء
 * @brief (EN) Parses Sad code, returns statement list — fails test if errors exist
 */
static StmtList parseCode(const std::string &code)
{
    LexerCore lexer(code);
    ParserCore parser(lexer);
    auto stmts = parser.parseProgram();
    if (parser.hasErrors())
    {
        parser.printErrors();
    }
    return stmts;
}

/**
 * @brief (AR) يحلل كود ص ويتحقق من عدم وجود أخطاء
 * @brief (EN) Parses Sad code and asserts no errors
 */
static bool parseWithoutErrors(const std::string &code)
{
    // (AR) `ParserCore::hasErrors()` تقرأ مديرَ الأخطاء **المفرَد العامّ** لا حالةَ
    //      المحلّل، فأيُّ اختبارٍ سابقٍ يفشل تحليلُه عمدًا (كاختبارِ رفضِ الاسمِ
    //      المُهمَل) يُلوّث كلَّ نداءٍ بعده فيبدو فاشلًا بلا سبب. نُصفّرُ قبلَ القياس.
    // (EN) hasErrors() reads a global singleton, not per-parser state: a prior
    //      intentional parse failure would poison every later call. Clear first.
    Errors::ErrorManager::getInstance().clear();
    LexerCore lexer(code);
    ParserCore parser(lexer);
    parser.parseProgram();
    return !parser.hasErrors();
}

/**
 * @brief (AR) يستخرج UIDeclarationNode من قائمة الجمل
 * @brief (EN) Extracts UIDeclarationNode from statement list
 */
static UIDeclarationNode *findUIDecl(const StmtList &stmts, const std::string &name = "")
{
    for (const auto &s : stmts)
    {
        auto *uiDecl = dynamic_cast<UIDeclarationNode *>(s.get());
        if (uiDecl && (name.empty() || uiDecl->name == name))
        {
            return uiDecl;
        }
    }
    return nullptr;
}

/**
 * @brief (AR) يستخرج دالة بناء() من مكون واجهة
 * @brief (EN) Extracts the بناء() method from a UI component
 */
static FunctionDecl *findBuildMethod(UIDeclarationNode *uiDecl)
{
    if (!uiDecl)
        return nullptr;
    for (const auto &m : uiDecl->methods)
    {
        auto *func = dynamic_cast<FunctionDecl *>(m.get());
        if (func && (func->name == "بناء" || func->name == "build"))
        {
            return func;
        }
    }
    return nullptr;
}

/**
 * @brief (AR) يستخرج عقدة UIWidgetExprNode من جملة ارجع (return) في دالة
 * @brief (EN) Extracts UIWidgetExprNode from a return statement in a function
 *
 * (AR) يبحث في جسم الدالة عن أول جملة ارجع تحتوي UIWidgetExprNode
 * (EN) Searches function body for first return containing UIWidgetExprNode
 */
static UIWidgetExprNode *findReturnWidget(FunctionDecl *func)
{
    if (!func || !func->body)
        return nullptr;

    // (AR) الجسم عبارة عن BlockStmt يحتوي جمل
    auto *block = dynamic_cast<BlockStmt *>(func->body.get());
    if (!block)
        return nullptr;

    for (const auto &stmt : block->statements)
    {
        auto *retStmt = dynamic_cast<ReturnStmt *>(stmt.get());
        if (retStmt && retStmt->value)
        {
            return dynamic_cast<UIWidgetExprNode *>(retStmt->value.get());
        }
    }
    return nullptr;
}

// =====================================================================
// UI-1.1: اختبارات الرسم الشرطي (إذا/وإلا) / Conditional Rendering Tests
// =====================================================================

void test_ui_conditional_basic()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("UI-1.1: Conditional Rendering (إذا/وإلا)");

    // ── (AR) اختبار 1: رسم شرطي بسيط بدون وإلا / (EN) Simple conditional without else ──
    runner.runTest("Simple إذا inside container", [&]()
                   {
        std::string code = R"(
واجهة اختبار_شرط
    @حالة ظاهر: منطقي = صحيح

    دالة بناء()
        ارجع اعرض عمود
            إذا (ظاهر)
                نص_عنصر("مرحباً")
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* uiDecl = findUIDecl(stmts);
        SAD_ASSERT_TRUE(uiDecl != nullptr);
        SAD_ASSERT_EQ(uiDecl->name, std::string("اختبار_شرط"));

        auto* buildFunc = findBuildMethod(uiDecl);
        SAD_ASSERT_TRUE(buildFunc != nullptr);

        auto* rootWidget = findReturnWidget(buildFunc);
        SAD_ASSERT_TRUE(rootWidget != nullptr);
        SAD_ASSERT_EQ(rootWidget->widgetName, std::string("عمود"));

        // (AR) يجب أن يحتوي الابن الأول على UIConditionalNode
        SAD_ASSERT_TRUE(rootWidget->children.size() >= 1);
        auto* conditional = dynamic_cast<UIConditionalNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(conditional != nullptr);

        // (AR) الفرع الصحيح يحتوي عنصر واحد
        SAD_ASSERT_EQ(conditional->thenChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(conditional->thenChildren[0]->widgetName, kTextWidget);

        // (AR) لا يوجد فرع وإلا
        SAD_ASSERT_TRUE(!conditional->hasElseBranch()); });

    // ── (AR) اختبار 2: رسم شرطي مع وإلا / (EN) Conditional with else ──
    runner.runTest("إذا/وإلا with both branches", [&]()
                   {
        std::string code = R"(
واجهة اختبار_شرط_كامل
    @حالة مسجل: منطقي = خطأ

    دالة بناء()
        ارجع اعرض عمود
            إذا (مسجل)
                نص_عنصر("مرحباً")
            وإلا
                نص_عنصر("سجّل دخولك")
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* uiDecl = findUIDecl(stmts);
        SAD_ASSERT_TRUE(uiDecl != nullptr);

        auto* rootWidget = findReturnWidget(findBuildMethod(uiDecl));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        auto* conditional = dynamic_cast<UIConditionalNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(conditional != nullptr);

        // (AR) فرع then
        SAD_ASSERT_EQ(conditional->thenChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(conditional->thenChildren[0]->widgetName, kTextWidget);

        // (AR) فرع else
        SAD_ASSERT_TRUE(conditional->hasElseBranch());
        SAD_ASSERT_EQ(conditional->elseChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(conditional->elseChildren[0]->widgetName, kTextWidget); });

    // ── (AR) اختبار 3: رسم شرطي متعدد العناصر / (EN) Conditional with multiple widgets ──
    runner.runTest("إذا with multiple children per branch", [&]()
                   {
        std::string code = R"(
واجهة اختبار_متعدد
    @حالة نشط: منطقي = صحيح

    دالة بناء()
        ارجع اعرض عمود
            إذا (نشط)
                نص_عنصر("الأول")
                نص_عنصر("الثاني")
                زر("ضغط")
            وإلا
                صورة("path.png")
                نص_عنصر("بديل")
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        auto* cond = dynamic_cast<UIConditionalNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(cond != nullptr);

        // (AR) 3 عناصر في then
        SAD_ASSERT_EQ(cond->thenChildren.size(), static_cast<size_t>(3));
        SAD_ASSERT_EQ(cond->thenChildren[0]->widgetName, kTextWidget);
        SAD_ASSERT_EQ(cond->thenChildren[1]->widgetName, kTextWidget);
        SAD_ASSERT_EQ(cond->thenChildren[2]->widgetName, std::string("زر"));

        // (AR) 2 عناصر في else
        SAD_ASSERT_EQ(cond->elseChildren.size(), static_cast<size_t>(2));
        SAD_ASSERT_EQ(cond->elseChildren[0]->widgetName, std::string("صورة"));
        SAD_ASSERT_EQ(cond->elseChildren[1]->widgetName, kTextWidget); });

    // ── (AR) اختبار 4: شرط متداخل / (EN) Nested conditional ──
    runner.runTest("Nested إذا inside إذا", [&]()
                   {
        std::string code = R"(
واجهة اختبار_تداخل
    @حالة أ: منطقي = صحيح
    @حالة ب: منطقي = خطأ

    دالة بناء()
        ارجع اعرض عمود
            إذا (أ)
                إذا (ب)
                    نص_عنصر("أ و ب")
                وإلا
                    نص_عنصر("أ فقط")
                نهاية
            وإلا
                نص_عنصر("لا أ")
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        auto* outerCond = dynamic_cast<UIConditionalNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(outerCond != nullptr);

        // (AR) then الخارجي يحتوي شرط داخلي
        SAD_ASSERT_EQ(outerCond->thenChildren.size(), static_cast<size_t>(1));
        auto* innerCond = dynamic_cast<UIConditionalNode*>(outerCond->thenChildren[0].get());
        SAD_ASSERT_TRUE(innerCond != nullptr);

        // (AR) then/else الداخلي
        SAD_ASSERT_EQ(innerCond->thenChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(innerCond->thenChildren[0]->widgetName, kTextWidget);
        SAD_ASSERT_TRUE(innerCond->hasElseBranch());
        SAD_ASSERT_EQ(innerCond->elseChildren.size(), static_cast<size_t>(1));

        // (AR) else الخارجي
        SAD_ASSERT_TRUE(outerCond->hasElseBranch());
        SAD_ASSERT_EQ(outerCond->elseChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(outerCond->elseChildren[0]->widgetName, kTextWidget); });

    // ── (AR) اختبار 5: شرط بجانب عناصر عادية / (EN) Conditional mixed with normal widgets ──
    runner.runTest("إذا mixed with regular widgets in container", [&]()
                   {
        std::string code = R"(
واجهة اختبار_مزج
    @حالة ظاهر: منطقي = صحيح

    دالة بناء()
        ارجع اعرض عمود
            نص_عنصر("العنوان")
            إذا (ظاهر)
                نص_عنصر("محتوى مرئي")
            نهاية
            زر("إجراء")
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);
        SAD_ASSERT_EQ(rootWidget->widgetName, std::string("عمود"));

        // (AR) 3 أبناء: نص، شرط، زر
        SAD_ASSERT_EQ(rootWidget->children.size(), static_cast<size_t>(3));
        SAD_ASSERT_EQ(rootWidget->children[0]->widgetName, kTextWidget);

        auto* cond = dynamic_cast<UIConditionalNode*>(rootWidget->children[1].get());
        SAD_ASSERT_TRUE(cond != nullptr);

        SAD_ASSERT_EQ(rootWidget->children[2]->widgetName, std::string("زر")); });
}

// =====================================================================
// UI-1.2: اختبارات حلقات الرسم (لكل/بينما) / Loop Rendering Tests
// =====================================================================

void test_ui_loop_basic()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("UI-1.2: Loop Rendering (لكل/بينما)");

    // ── (AR) اختبار 1: لكل بسيط / (EN) Simple for-each ──
    runner.runTest("Simple لكل inside container", [&]()
                   {
        std::string code = R"(
واجهة اختبار_حلقة
    @حالة عناصر = ["أ"، "ب"، "ج"]

    دالة بناء()
        ارجع اعرض عمود
            لكل عنصر في عناصر
                نص_عنصر(عنصر)
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* uiDecl = findUIDecl(stmts);
        SAD_ASSERT_TRUE(uiDecl != nullptr);

        auto* rootWidget = findReturnWidget(findBuildMethod(uiDecl));
        SAD_ASSERT_TRUE(rootWidget != nullptr);
        SAD_ASSERT_EQ(rootWidget->widgetName, std::string("عمود"));

        // (AR) الابن الأول عقدة حلقة
        SAD_ASSERT_TRUE(rootWidget->children.size() >= 1);
        auto* loop = dynamic_cast<UILoopNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(loop != nullptr);

        // (AR) نوع الحلقة: لكل
        SAD_ASSERT_TRUE(loop->loopKind == UILoopNode::LoopKind::FOR_EACH);
        SAD_ASSERT_EQ(loop->iteratorName, std::string("عنصر"));

        // (AR) جسم الحلقة يحتوي عنصر واحد
        SAD_ASSERT_EQ(loop->bodyChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(loop->bodyChildren[0]->widgetName, kTextWidget); });

    // ── (AR) اختبار 2: لكل مع معدّلات / (EN) For-each with modifiers ──
    runner.runTest("لكل with widget modifiers", [&]()
                   {
        std::string code = R"(
واجهة اختبار_معدلات
    @حالة أسماء = ["أحمد"، "فاطمة"]

    دالة بناء()
        ارجع اعرض عمود
            لكل اسم في أسماء
                نص_عنصر(اسم).حجم(16)
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        auto* loop = dynamic_cast<UILoopNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(loop != nullptr);
        SAD_ASSERT_TRUE(loop->loopKind == UILoopNode::LoopKind::FOR_EACH);

        // (AR) العنصر الابن يحتوي معدّل
        SAD_ASSERT_EQ(loop->bodyChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_TRUE(loop->bodyChildren[0]->hasModifiers());
        SAD_ASSERT_EQ(loop->bodyChildren[0]->modifiers[0]->name, std::string("حجم")); });

    // ── (AR) اختبار 3: لكل متعدد الأبناء / (EN) For-each with multiple children ──
    runner.runTest("لكل with multiple body children", [&]()
                   {
        std::string code = R"(
واجهة اختبار_متعدد
    @حالة قائمة = [1، 2، 3]

    دالة بناء()
        ارجع اعرض عمود
            لكل ع في قائمة
                نص_عنصر("رقم")
                زر("حذف")
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        auto* loop = dynamic_cast<UILoopNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(loop != nullptr);
        SAD_ASSERT_EQ(loop->bodyChildren.size(), static_cast<size_t>(2));
        SAD_ASSERT_EQ(loop->bodyChildren[0]->widgetName, kTextWidget);
        SAD_ASSERT_EQ(loop->bodyChildren[1]->widgetName, std::string("زر")); });

    // ── (AR) اختبار 4: لكل متداخل / (EN) Nested for-each ──
    runner.runTest("Nested لكل inside لكل", [&]()
                   {
        std::string code = R"(
واجهة اختبار_تداخل_حلقات
    @حالة مصفوفة = [[1، 2]، [3، 4]]

    دالة بناء()
        ارجع اعرض عمود
            لكل صف في مصفوفة
                صف
                    لكل عنصر في صف
                        نص_عنصر("قيمة")
                    نهاية
                نهاية
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        // (AR) الابن الأول: حلقة خارجية
        auto* outerLoop = dynamic_cast<UILoopNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(outerLoop != nullptr);
        SAD_ASSERT_EQ(outerLoop->iteratorName, std::string("صف"));

        // (AR) جسم الحلقة الخارجية يحتوي عنصر صف (container)
        SAD_ASSERT_EQ(outerLoop->bodyChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(outerLoop->bodyChildren[0]->widgetName, std::string("صف"));

        // (AR) داخل الصف: حلقة داخلية
        auto* rowWidget = outerLoop->bodyChildren[0].get();
        SAD_ASSERT_TRUE(rowWidget->children.size() >= 1);
        auto* innerLoop = dynamic_cast<UILoopNode*>(rowWidget->children[0].get());
        SAD_ASSERT_TRUE(innerLoop != nullptr);
        SAD_ASSERT_EQ(innerLoop->iteratorName, std::string("عنصر")); });

    // ── (AR) اختبار 5: لكل مع إذا (شرط + حلقة مختلطة) / (EN) Loop with conditional ──
    runner.runTest("لكل combined with إذا", [&]()
                   {
        std::string code = R"(
واجهة اختبار_مزج
    @حالة عناصر = ["أ"، "ب"]
    @حالة تصفية: منطقي = صحيح

    دالة بناء()
        ارجع اعرض عمود
            إذا (تصفية)
                لكل ع في عناصر
                    نص_عنصر(ع)
                نهاية
            وإلا
                نص_عنصر("لا تصفية")
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        // (AR) الابن شرط
        auto* cond = dynamic_cast<UIConditionalNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(cond != nullptr);

        // (AR) then يحتوي حلقة
        SAD_ASSERT_EQ(cond->thenChildren.size(), static_cast<size_t>(1));
        auto* loop = dynamic_cast<UILoopNode*>(cond->thenChildren[0].get());
        SAD_ASSERT_TRUE(loop != nullptr);
        SAD_ASSERT_EQ(loop->iteratorName, std::string("ع"));

        // (AR) else يحتوي نص
        SAD_ASSERT_TRUE(cond->hasElseBranch());
        SAD_ASSERT_EQ(cond->elseChildren[0]->widgetName, kTextWidget); });

    // ── (AR) اختبار 6: لكل مع حاويات متعددة / (EN) For-each mixed with regular widgets ──
    runner.runTest("لكل mixed with regular widgets", [&]()
                   {
        std::string code = R"(
واجهة اختبار_مختلط
    @حالة قائمة = [1، 2]

    دالة بناء()
        ارجع اعرض عمود
            نص_عنصر("عنوان")
            لكل ع في قائمة
                نص_عنصر("عنصر")
            نهاية
            زر("المزيد")
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        // (AR) 3 أبناء: نص، حلقة، زر
        SAD_ASSERT_EQ(rootWidget->children.size(), static_cast<size_t>(3));
        SAD_ASSERT_EQ(rootWidget->children[0]->widgetName, kTextWidget);

        auto* loop = dynamic_cast<UILoopNode*>(rootWidget->children[1].get());
        SAD_ASSERT_TRUE(loop != nullptr);

        SAD_ASSERT_EQ(rootWidget->children[2]->widgetName, std::string("زر")); });
}

// =====================================================================
// UI-1.3: اختبارات تقليص knownWidgets / Widget Registry Tests
// =====================================================================

void test_ui_widget_registry()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("UI-1.3: knownWidgets & containerWidgets");

    // ── (AR) اختبار 1: كلُّ عنصرٍ أوّليٍّ في مصدرِ الحقيقةِ يُحلَّل / (EN) Every SoT primitive parses ──
    //      القائمتان تُقرآن من الرأسِ المولَّدِ لا من نصٍّ يدويّ، فلا تبيتُ الاختباراتُ
    //      إن أُضيفت عقدةٌ أو أُعيدت تسميتُها في language-truth/ui_nodes.yaml.
    // (EN) Both lists come from the generated header, never a hand-written literal.
    runner.runTest("All SoT primitive widgets parse correctly", [&]()
                   {
        const std::vector<std::string> primitives = {
            SAD_UI_PARSER_PRIMITIVE_LIST(SAD_UI_TEST_NAME_ENTRY)};
        const std::unordered_set<std::string> containerSet = {
            SAD_UI_PARSER_CONTAINER_LIST(SAD_UI_TEST_NAME_ENTRY)};
        // (AR) الرقمُ قرارُ ADR-UI-02 لا اشتقاقٌ من المولّد: مقارنةُ المولَّدِ
        //      بالمولَّدِ حشوٌ لا يفشل أبدًا، فلن تكشف عقدةً سادسةَ عشرةَ تُرفَع
        //      إلى الأوّليّاتِ سهوًا.
        // (EN) ADR-UI-02's count is a decision — comparing the generated list to
        //      itself is a tautology that can never fail.
        SAD_ASSERT_EQ(primitives.size(), static_cast<size_t>(kAdrPrimitiveCount));

        for (const auto& widget : primitives) {
            // (AR) نختبر كل عنصر كعنصر ورقي داخل حاوية
            std::string code = "واجهة ت\n    دالة بناء()\n        ارجع اعرض عمود\n            "
                + widget;

            // (AR) الحاويات تحتاج نهاية إضافية
            if (containerSet.count(widget) > 0) {
                code += "\n            نهاية";
            }
            code += "\n        نهاية\n    نهاية\nنهاية";

            bool ok = parseWithoutErrors(code);
            SAD_ASSERT_TRUE(ok);
        } });

    // ── (AR) اختبار 2: كلُّ حاويةٍ في مصدرِ الحقيقةِ تقبل أبناء / (EN) Every SoT container accepts children ──
    runner.runTest("SoT containers accept children", [&]()
                   {
        const std::vector<std::string> containers = {
            SAD_UI_PARSER_CONTAINER_LIST(SAD_UI_TEST_NAME_ENTRY)};
        SAD_ASSERT_EQ(containers.size(), static_cast<size_t>(kAdrContainerCount));

        for (const auto& container : containers) {
            std::string code = "واجهة ت\n    دالة بناء()\n        ارجع اعرض "
                + container + "\n            " + kTextWidget
                + "(\"ابن\")\n        نهاية\n    نهاية\nنهاية";

            auto stmts = parseCode(code);
            auto* uiDecl = findUIDecl(stmts);
            SAD_ASSERT_TRUE(uiDecl != nullptr);

            auto* rootWidget = findReturnWidget(findBuildMethod(uiDecl));
            SAD_ASSERT_TRUE(rootWidget != nullptr);
            SAD_ASSERT_EQ(rootWidget->widgetName, container);
            SAD_ASSERT_TRUE(rootWidget->hasChildrenBlock);
            SAD_ASSERT_TRUE(rootWidget->children.size() >= 1);
        } });

    // ── (AR) اختبار 3: لا توافقَ خلفيًّا — الاسمُ المُهمَلُ يُرفَض / (EN) No back-compat: legacy name rejected ──
    //      «بطاقة» كان يُحلَّل حاويةً عبر جدولِ الإهمالِ المحذوف. القرارُ المالكيُّ:
    //      الاسمُ القانونيُّ وحدَه يُقبَل ⇒ يجب أن يفشلَ التحليلُ الآن لا أن يحذّر.
    // (EN) The deprecated alias must now fail to parse, not warn.
    runner.runTest("Legacy widget name is rejected (no back-compat)", [&]()
                   {
        std::string code = R"(
واجهة ت
    دالة بناء()
        ارجع اعرض عمود
            بطاقة
            نهاية
        نهاية
    نهاية
نهاية
)";
        bool ok = parseWithoutErrors(code);
        SAD_ASSERT_TRUE(!ok); });

    // ── (AR) اختبار 4: parseWidgetExpressionTyped يُرجع النوع الصحيح / (EN) Typed parse returns correct type ──
    runner.runTest("parseWidgetExpressionTyped returns UIWidgetExprNode", [&]()
                   {
        std::string code = R"(
واجهة ت
    دالة بناء()
        ارجع اعرض عمود
            نص_عنصر("مرحبا")
            زر("ضغط")
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        // (AR) الأبناء مُحللون بـ parseWidgetExpressionTyped — لا يحتاجون dynamic_cast
        for (const auto& child : rootWidget->children) {
            SAD_ASSERT_TRUE(child != nullptr);
            // (AR) كل ابن يجب أن يكون UIWidgetExprNode بالفعل
            SAD_ASSERT_TRUE(!child->widgetName.empty());
        } });
}

// =====================================================================
// اختبارات إضافية: بينما / While loop tests
// =====================================================================

void test_ui_while_loop()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("UI-1.2b: While Loop Rendering (بينما)");

    // ── (AR) اختبار 1: بينما بسيط / (EN) Simple while ──
    runner.runTest("Simple بينما inside container", [&]()
                   {
        std::string code = R"(
واجهة اختبار_بينما
    @حالة عدد: رقم = 0

    دالة بناء()
        ارجع اعرض عمود
            بينما (عدد < 5)
                نص_عنصر("صف")
            نهاية
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* rootWidget = findReturnWidget(findBuildMethod(findUIDecl(stmts)));
        SAD_ASSERT_TRUE(rootWidget != nullptr);

        auto* loop = dynamic_cast<UILoopNode*>(rootWidget->children[0].get());
        SAD_ASSERT_TRUE(loop != nullptr);
        SAD_ASSERT_TRUE(loop->loopKind == UILoopNode::LoopKind::WHILE);
        SAD_ASSERT_TRUE(loop->whileCondition != nullptr);
        SAD_ASSERT_EQ(loop->bodyChildren.size(), static_cast<size_t>(1)); });
}

// =====================================================================
// اختبارات تكامل: كل الميزات معاً / Integration tests
// =====================================================================

void test_ui_phase0_integration()
{
    auto &runner = TestRunner::instance();
    runner.beginGroup("Phase 0 Integration");

    // ── (AR) مكون معقد يجمع كل ميزات Phase 0 ──
    runner.runTest("Complex component: إذا + لكل + containers + modifiers", [&]()
                   {
        std::string code = R"(
واجهة تطبيق_معقد
    @حالة مسجل: منطقي = صحيح
    @حالة عناصر = ["أ"، "ب"، "ج"]

    دالة بناء()
        ارجع اعرض عمود
            نص_عنصر("التطبيق").حجم(32)
            إذا (مسجل)
                صف
                    لكل ع في عناصر
                        نص_عنصر(ع).حجم(16)
                    نهاية
                نهاية
            وإلا
                نص_عنصر("سجّل أولاً")
                زر("تسجيل")
            نهاية
            فاصل
        نهاية
    نهاية
نهاية
)";
        auto stmts = parseCode(code);
        auto* uiDecl = findUIDecl(stmts);
        SAD_ASSERT_TRUE(uiDecl != nullptr);
        SAD_ASSERT_EQ(uiDecl->name, std::string("تطبيق_معقد"));

        // (AR) حالتان
        SAD_ASSERT_EQ(uiDecl->stateDecls.size(), static_cast<size_t>(2));

        auto* rootWidget = findReturnWidget(findBuildMethod(uiDecl));
        SAD_ASSERT_TRUE(rootWidget != nullptr);
        SAD_ASSERT_EQ(rootWidget->widgetName, std::string("عمود"));

        // (AR) 3 أبناء: نص، شرط، فاصل
        SAD_ASSERT_EQ(rootWidget->children.size(), static_cast<size_t>(3));

        // (AR) الأول: نص مع معدّل
        SAD_ASSERT_EQ(rootWidget->children[0]->widgetName, kTextWidget);
        SAD_ASSERT_TRUE(rootWidget->children[0]->hasModifiers());

        // (AR) الثاني: شرط
        auto* cond = dynamic_cast<UIConditionalNode*>(rootWidget->children[1].get());
        SAD_ASSERT_TRUE(cond != nullptr);

        // (AR) then: صف يحتوي لكل
        SAD_ASSERT_EQ(cond->thenChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(cond->thenChildren[0]->widgetName, std::string("صف"));
        auto* row = cond->thenChildren[0].get();
        SAD_ASSERT_TRUE(row->children.size() >= 1);
        auto* innerLoop = dynamic_cast<UILoopNode*>(row->children[0].get());
        SAD_ASSERT_TRUE(innerLoop != nullptr);

        // (AR) else: نص + زر
        SAD_ASSERT_TRUE(cond->hasElseBranch());
        SAD_ASSERT_EQ(cond->elseChildren.size(), static_cast<size_t>(2));

        // (AR) الثالث: فاصل
        SAD_ASSERT_EQ(rootWidget->children[2]->widgetName, std::string("فاصل")); });

    // ── (AR) اختبار عدم وجود أخطاء في تحليل كود مرجعي / (EN) No errors parsing reference code ──
    runner.runTest("Reference code from architecture-sadui.md parses OK", [&]()
                   {
        // (AR) الكود المرجعي من Story UI-1.1
        std::string code1 = R"(
واجهة اختبار
    دالة بناء()
        ارجع اعرض عمود
            إذا (مسجل)
                نص_عنصر("مرحباً")
            وإلا
                نص_عنصر("سجّل دخولك")
            نهاية
        نهاية
    نهاية
نهاية
)";
        SAD_ASSERT_TRUE(parseWithoutErrors(code1));

        // (AR) الكود المرجعي من Story UI-1.2
        std::string code2 = R"(
واجهة قائمة
    @حالة عناصر = ["أ"، "ب"، "ج"]

    دالة بناء()
        ارجع اعرض عمود
            لكل عنصر في عناصر
                نص_عنصر(عنصر).حجم(16)
            نهاية
        نهاية
    نهاية
نهاية
)";
        SAD_ASSERT_TRUE(parseWithoutErrors(code2)); });
}

// =====================================================================
// main — نقطة الدخول
// =====================================================================

int main()
{
    SAD_ENABLE_COLORS();
    std::cout << "\n";
    std::cout << "== Phase 0 UI Parser Acceptance Tests ==\n\n";

    test_ui_conditional_basic();
    test_ui_loop_basic();
    test_ui_widget_registry();
    test_ui_while_loop();
    test_ui_phase0_integration();

    return TestRunner::instance().printSummary();
}
