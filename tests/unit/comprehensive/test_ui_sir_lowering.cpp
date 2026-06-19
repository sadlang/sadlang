// بسم الله الرحمن الرحيم
// ============================================================================
// test_ui_sir_lowering.cpp
// (AR) اختبار خفض مكوّن «واجهة» إلى SIR (STORY-UI-W15-04, AC-1/AC-4)
// (EN) UI component (واجهة) → SIR lowering test (STORY-UI-W15-04, AC-1/AC-4)
// ============================================================================
//
// (AR) يثبت أن SIRBuilder يخفض كل عقدة UIDeclarationNode على المستوى الأعلى إلى
//      ClassDecl مكافئ (Phase 0) فتعالجها مراحل بناء الأصناف القائمة، فيظهر في
//      وحدة SIR صنفٌ بحقول @حالة ودواله، مع احترام الوراثة «يرث».
// (EN) Verifies SIRBuilder lowers each top-level UIDeclarationNode to an
//      equivalent ClassDecl (Phase 0), so the existing class-building phases
//      emit a SIR class carrying the @state fields and methods, while
//      preserving `يرث` inheritance.
//
// (AR) يُربط بـsad_core (يضمّ المحلّل + sad_frontend) — لا interpreter_test_stubs.
// (EN) Linked against sad_core (parser + sad_frontend) — NOT the stubs.
// ============================================================================

#include <iostream>
#include <memory>
#include <string>

#include "lexer_core.h"
#include "parser_core.h"
#include "../../../compiler/include/frontend/sir_builder.h"
#include "../../../compiler/include/frontend/sir_module.h"

namespace {

int g_failures = 0;
int g_checks = 0;

void check(bool cond, const std::string &label)
{
    ++g_checks;
    if (cond)
    {
        std::cout << "  [PASS] " << label << "\n";
    }
    else
    {
        ++g_failures;
        std::cout << "  [FAIL] " << label << "\n";
    }
}

// (AR) مصدر يحوي مكوّنين: «عداد» (حقل @حالة + دالة) و«عداد_مزدوج يرث عداد».
// (EN) Source with two components: counter (@state + method) and a child that
//      inherits via يرث.
const char *kSource =
    "\xD9\x88\xD8\xA7\xD8\xAC\xD9\x87\xD8\xA9 \xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF\n"        // واجهة عداد
    "    @\xD8\xAD\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB9\xD8\xAF\xD8\xAF = 0\n"               // @حالة عدد = 0
    "    \xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB2\xD9\x90\xD8\xAF()\n"                  // دالة زِد()
    "        \xD9\x87\xD8\xB0\xD8\xA7.\xD8\xB9\xD8\xAF\xD8\xAF = \xD9\x87\xD8\xB0\xD8\xA7.\xD8\xB9\xD8\xAF\xD8\xAF + 1\n" // هذا.عدد = هذا.عدد + 1
    "    \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"                                     // نهاية
    "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"                                         // نهاية
    "\xD9\x88\xD8\xA7\xD8\xAC\xD9\x87\xD8\xA9 \xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF_\xD9\x85\xD8\xB2\xD8\xAF\xD9\x88\xD8\xAC \xD9\x8A\xD8\xB1\xD8\xAB \xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF\n" // واجهة عداد_مزدوج يرث عداد
    "    \xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB2\xD9\x90\xD8\xAF_\xD9\x85\xD8\xB1\xD8\xAA\xD9\x8A\xD9\x86()\n" // دالة زِد_مرتين()
    "        \xD9\x87\xD8\xB0\xD8\xA7.\xD8\xB2\xD9\x90\xD8\xAF()\n"                       // هذا.زِد()
    "    \xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n"                                     // نهاية
    "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9\n";                                        // نهاية

// (AR) أسماء عربية تُستخدم في التحقّق (UTF-8 صريح لتفادي مشاكل الترميز).
const std::string kCounter   = "\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF";                       // عداد
const std::string kField     = "\xD8\xB9\xD8\xAF\xD8\xAF";                               // عدد
// (AR) المحلّل يجرّد التشكيل من المُعرّفات، فتُخزَّن «زِد» باسم «زد».
// (EN) The lexer strips tashkeel from identifiers, so «زِد» is stored as «زد».
const std::string kMethod    = "\xD8\xB2\xD8\xAF";                                       // زد (زِد بلا تشكيل)
const std::string kChild     = "\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF_\xD9\x85\xD8\xB2\xD8\xAF\xD9\x88\xD8\xAC"; // عداد_مزدوج
const std::string kChildMeth = "\xD8\xB2\xD8\xAF_\xD9\x85\xD8\xB1\xD8\xAA\xD9\x8A\xD9\x86";                 // زد_مرتين

} // namespace

int main()
{
    std::cout << "=== UI -> SIR lowering test (STORY-UI-W15-04) ===\n";

    // (1) Lex + parse
    Sad::Lexer::LexerCore lexer(kSource);
    Sad::Parser::ParserCore parser(lexer);
    Sad::AST::StmtList program = parser.parseProgram();

    check(!parser.hasErrors(), "parser produced no errors");
    check(!program.empty(), "program AST is non-empty");

    // (2) Build SIR
    Sad::Compiler::SIR::SIRBuilder builder;
    auto module = builder.buildModule(&program);

    check(module != nullptr, "buildModule returned a module (no unsupported-node failure) [AC-1]");
    if (!module)
    {
        std::cout << "\nRESULT: FAILED (no module)\n";
        return 1;
    }

    // (3) AC-4: واجهة عداد lowered to a SIR class with its @state field + method
    auto counter = module->getClass(kCounter);
    check(counter != nullptr, "UI component 'عداد' lowered to a SIR class [AC-4]");
    if (counter)
    {
        check(counter->getField(kField) != nullptr, "@state field 'عدد' present on lowered class");
        // (AR) الطُرق تُخزَّن باسم مؤهَّل «الصنف.الطريقة» (اصطلاح الأصناف القائم).
        // (EN) Methods are keyed by the qualified name «Class.method» (existing class convention).
        check(counter->getMethod(kCounter + "." + kMethod) != nullptr, "method 'عداد.زد' present on lowered class");
    }

    // (4) Inheritance يرث preserved on the child component
    auto child = module->getClass(kChild);
    check(child != nullptr, "child UI component 'عداد_مزدوج' lowered to a SIR class");
    if (child)
    {
        check(child->getParentClass() == kCounter, "child preserves 'يرث عداد' as parent class");
        check(child->getMethod(kChild + "." + kChildMeth) != nullptr, "child method 'عداد_مزدوج.زد_مرتين' present");
    }

    std::cout << "\nChecks: " << (g_checks - g_failures) << "/" << g_checks << " passed\n";
    if (g_failures == 0)
    {
        std::cout << "RESULT: PASSED\n";
        return 0;
    }
    std::cout << "RESULT: FAILED (" << g_failures << " checks)\n";
    return 1;
}
