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

#include <string>
#include <vector>
#include <iostream>

using namespace SadTest;
using namespace Sad;
using namespace Sad::AST;
using namespace Sad::Lexer;
using namespace Sad::Parser;

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
                نص("مرحباً")
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
        SAD_ASSERT_EQ(conditional->thenChildren[0]->widgetName, std::string("نص"));

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
                نص("مرحباً")
            وإلا
                نص("سجّل دخولك")
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
        SAD_ASSERT_EQ(conditional->thenChildren[0]->widgetName, std::string("نص"));

        // (AR) فرع else
        SAD_ASSERT_TRUE(conditional->hasElseBranch());
        SAD_ASSERT_EQ(conditional->elseChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(conditional->elseChildren[0]->widgetName, std::string("نص")); });

    // ── (AR) اختبار 3: رسم شرطي متعدد العناصر / (EN) Conditional with multiple widgets ──
    runner.runTest("إذا with multiple children per branch", [&]()
                   {
        std::string code = R"(
واجهة اختبار_متعدد
    @حالة نشط: منطقي = صحيح

    دالة بناء()
        ارجع اعرض عمود
            إذا (نشط)
                نص("الأول")
                نص("الثاني")
                زر("ضغط")
            وإلا
                صورة("path.png")
                نص("بديل")
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
        SAD_ASSERT_EQ(cond->thenChildren[0]->widgetName, std::string("نص"));
        SAD_ASSERT_EQ(cond->thenChildren[1]->widgetName, std::string("نص"));
        SAD_ASSERT_EQ(cond->thenChildren[2]->widgetName, std::string("زر"));

        // (AR) 2 عناصر في else
        SAD_ASSERT_EQ(cond->elseChildren.size(), static_cast<size_t>(2));
        SAD_ASSERT_EQ(cond->elseChildren[0]->widgetName, std::string("صورة"));
        SAD_ASSERT_EQ(cond->elseChildren[1]->widgetName, std::string("نص")); });

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
                    نص("أ و ب")
                وإلا
                    نص("أ فقط")
                نهاية
            وإلا
                نص("لا أ")
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
        SAD_ASSERT_EQ(innerCond->thenChildren[0]->widgetName, std::string("نص"));
        SAD_ASSERT_TRUE(innerCond->hasElseBranch());
        SAD_ASSERT_EQ(innerCond->elseChildren.size(), static_cast<size_t>(1));

        // (AR) else الخارجي
        SAD_ASSERT_TRUE(outerCond->hasElseBranch());
        SAD_ASSERT_EQ(outerCond->elseChildren.size(), static_cast<size_t>(1));
        SAD_ASSERT_EQ(outerCond->elseChildren[0]->widgetName, std::string("نص")); });

    // ── (AR) اختبار 5: شرط بجانب عناصر عادية / (EN) Conditional mixed with normal widgets ──
    runner.runTest("إذا mixed with regular widgets in container", [&]()
                   {
        std::string code = R"(
واجهة اختبار_مزج
    @حالة ظاهر: منطقي = صحيح

    دالة بناء()
        ارجع اعرض عمود
            نص("العنوان")
            إذا (ظاهر)
                نص("محتوى مرئي")
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
        SAD_ASSERT_EQ(rootWidget->children[0]->widgetName, std::string("نص"));

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
                نص(عنصر)
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
        SAD_ASSERT_EQ(loop->bodyChildren[0]->widgetName, std::string("نص")); });

    // ── (AR) اختبار 2: لكل مع معدّلات / (EN) For-each with modifiers ──
    runner.runTest("لكل with widget modifiers", [&]()
                   {
        std::string code = R"(
واجهة اختبار_معدلات
    @حالة أسماء = ["أحمد"، "فاطمة"]

    دالة بناء()
        ارجع اعرض عمود
            لكل اسم في أسماء
                نص(اسم).حجم(16)
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
                نص("رقم")
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
        SAD_ASSERT_EQ(loop->bodyChildren[0]->widgetName, std::string("نص"));
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
                        نص("قيمة")
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
                    نص(ع)
                نهاية
            وإلا
                نص("لا تصفية")
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
        SAD_ASSERT_EQ(cond->elseChildren[0]->widgetName, std::string("نص")); });

    // ── (AR) اختبار 6: لكل مع حاويات متعددة / (EN) For-each mixed with regular widgets ──
    runner.runTest("لكل mixed with regular widgets", [&]()
                   {
        std::string code = R"(
واجهة اختبار_مختلط
    @حالة قائمة = [1، 2]

    دالة بناء()
        ارجع اعرض عمود
            نص("عنوان")
            لكل ع في قائمة
                نص("عنصر")
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
        SAD_ASSERT_EQ(rootWidget->children[0]->widgetName, std::string("نص"));

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

    // ── (AR) اختبار 1: العناصر الأولية الـ 15 تعمل / (EN) All 15 primitives parse ──
    runner.runTest("All 15 primitive widgets parse correctly", [&]()
                   {
        // (AR) جميع العناصر الأولية الـ 15 يجب أن تُحلل بنجاح
        std::vector<std::string> primitives = {
            "عمود", "صف", "رصة", "شبكة",      // تخطيط
            "نص", "صورة", "أيقونة",             // عرض
            "زر", "حقل_نص", "مفتاح", "منزلق",  // تفاعل
            "حاوية", "عرض_تمرير", "قائمة_كسولة", // هيكل
            "فاصل"                               // فراغ
        };
        SAD_ASSERT_EQ(primitives.size(), static_cast<size_t>(15));

        for (const auto& widget : primitives) {
            // (AR) نختبر كل عنصر كعنصر ورقي داخل حاوية
            std::string code = "واجهة ت\n    دالة بناء()\n        ارجع اعرض عمود\n            "
                + widget;

            // (AR) الحاويات تحتاج نهاية إضافية
            bool isContainer = (widget == "عمود" || widget == "صف" || widget == "رصة" ||
                                widget == "شبكة" || widget == "حاوية" ||
                                widget == "عرض_تمرير" || widget == "قائمة_كسولة");

            if (isContainer) {
                code += "\n            نهاية";
            }
            code += "\n        نهاية\n    نهاية\nنهاية";

            bool ok = parseWithoutErrors(code);
            SAD_ASSERT_TRUE(ok);
        } });

    // ── (AR) اختبار 2: الحاويات الـ 7 تقبل أبناء / (EN) 7 containers accept children ──
    runner.runTest("7 containers accept children", [&]()
                   {
        std::vector<std::string> containers = {
            "عمود", "صف", "رصة", "شبكة",
            "حاوية", "عرض_تمرير", "قائمة_كسولة"
        };
        SAD_ASSERT_EQ(containers.size(), static_cast<size_t>(7));

        for (const auto& container : containers) {
            std::string code = "واجهة ت\n    دالة بناء()\n        ارجع اعرض "
                + container + "\n            نص(\"ابن\")\n        نهاية\n    نهاية\nنهاية";

            auto stmts = parseCode(code);
            auto* uiDecl = findUIDecl(stmts);
            SAD_ASSERT_TRUE(uiDecl != nullptr);

            auto* rootWidget = findReturnWidget(findBuildMethod(uiDecl));
            SAD_ASSERT_TRUE(rootWidget != nullptr);
            SAD_ASSERT_EQ(rootWidget->widgetName, container);
            SAD_ASSERT_TRUE(rootWidget->hasChildrenBlock);
            SAD_ASSERT_TRUE(rootWidget->children.size() >= 1);
        } });

    // ── (AR) اختبار 3: العناصر المهملة تصدر تحذير / (EN) Deprecated widgets emit warning ──
    runner.runTest("Deprecated widget emits warning but parses", [&]()
                   {
        // (AR) "بطاقة" مُهمل → يُحلل كـ "حاوية"
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
        // (AR) يجب أن يُحلل بنجاح (مع تحذير في stderr)
        bool ok = parseWithoutErrors(code);
        SAD_ASSERT_TRUE(ok); });

    // ── (AR) اختبار 4: parseWidgetExpressionTyped يُرجع النوع الصحيح / (EN) Typed parse returns correct type ──
    runner.runTest("parseWidgetExpressionTyped returns UIWidgetExprNode", [&]()
                   {
        std::string code = R"(
واجهة ت
    دالة بناء()
        ارجع اعرض عمود
            نص("مرحبا")
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
                نص("صف")
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
            نص("التطبيق").حجم(32)
            إذا (مسجل)
                صف
                    لكل ع في عناصر
                        نص(ع).حجم(16)
                    نهاية
                نهاية
            وإلا
                نص("سجّل أولاً")
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
        SAD_ASSERT_EQ(rootWidget->children[0]->widgetName, std::string("نص"));
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
                نص("مرحباً")
            وإلا
                نص("سجّل دخولك")
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
                نص(عنصر).حجم(16)
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
