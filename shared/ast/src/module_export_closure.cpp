/**
 * @file module_export_closure.cpp
 * @brief (AR) تنفيذُ إغلاقِ صادراتِ الوحدة — نسخةٌ واحدةٌ للمحرّكين
 * @brief (EN) Module export closure implementation — one copy for both engines
 */

#include "module_export_closure.h"

#include "advanced_expr_nodes.h"
#include "class_nodes.h"
#include "declarations.h"
#include "directive_nodes.h"
#include "expressions.h"
#include "module_nodes.h"
#include "pattern_nodes.h"
#include "statements.h"

#include <unordered_map>

namespace Sad
{
    namespace AST
    {
            // (AR) تصريحٌ مسبق: الجوّالان متعاودان.
            // (EN) Forward declaration: the two walkers are mutually recursive.
            void collectReferencedNamesFromExpr(const Sad::AST::Expression *expr,
                                                std::set<std::string> &out);

            // (AR) أنماطُ المطابقةِ تُسمّي تعداداتٍ قد تكون خاصّةً بالوحدة، والمتغايرُ
            //      يتداخل (`حالة نتيجة::نجاح(خطأ_خاص::رمز)`) فلا يكفي فحصُ المستوى
            //      الأوّل: النزولُ متعاودٌ وإلّا حُجِب التعدادُ الداخليّ.
            // (EN) Patterns name enums that may be module-private, and variants nest, so a
            //      flat first-level check is not enough — recurse or the inner enum is hidden.
            void collectReferencedNamesFromPattern(const Sad::AST::Pattern *pattern,
                                                   std::set<std::string> &out)
            {
                using namespace Sad::AST;
                if (!pattern)
                    return;
                if (auto *enumPat = dynamic_cast<const EnumVariantPattern *>(pattern))
                {
                    out.insert(enumPat->enumName);
                    for (const auto &fieldPattern : enumPat->fieldPatterns)
                        collectReferencedNamesFromPattern(fieldPattern.get(), out);
                }
            }

                // (AR) يجمع كلَّ معرّفٍ حرٍّ في شجرةٍ — نظيرُ collectFreeVars لكنّه لا يشترط
                //      lookupVariable، وذاك يُسقِط أسماءَ الدوالِّ فلا يصلح لهذا الإغلاق.
                // (EN) Collects every free identifier in a subtree. Sibling of collectFreeVars,
                //      but without its lookupVariable requirement, which drops function names
                //      and therefore cannot compute this closure.
                void collectReferencedNames(const Sad::AST::Statement *stmt,
                                            std::set<std::string> &out)
                {
                    using namespace Sad::AST;
                    if (!stmt)
                        return;

                    if (auto *block = dynamic_cast<const BlockStmt *>(stmt))
                    {
                        for (const auto &s : block->statements)
                            collectReferencedNames(s.get(), out);
                        return;
                    }
                    // (AR) تصريحُ دالّةٍ داخلَ شجرة: طريقةُ صنفٍ أو دالّةٌ متداخلة. بلا هذا
                    //      الفرعِ تُهمَل أجسامُ طرقِ الصنفِ المُصدَّرِ فلا يُبلَغ مساعِدُها
                    //      الخاصُّ ⇒ يُحجَب ⇒ «دالّة غير معرّفة».
                    // (EN) A function declaration inside a subtree: a class method or a nested
                    //      function. Without this arm an exported class's method bodies were
                    //      skipped, so their private helper was never reached and got hidden.
                    if (auto *fnDecl = dynamic_cast<const FunctionDecl *>(stmt))
                    {
                        collectReferencedNames(fnDecl->body.get(), out);
                        return;
                    }
                    // (AR) طريقةُ الصنفِ عقدةٌ مستقلّةٌ (`MethodDecl`) لا `FunctionDecl`.
                    // (EN) A class method is its own node (MethodDecl), not a FunctionDecl.
                    if (auto *methodDecl = dynamic_cast<const MethodDecl *>(stmt))
                    {
                        collectReferencedNames(methodDecl->body.get(), out);
                        return;
                    }
                    if (auto *exprStmt = dynamic_cast<const ExprStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(exprStmt->expression.get(), out);
                        return;
                    }
                    if (auto *varDecl = dynamic_cast<const VarDeclStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(varDecl->initializer.get(), out);
                        return;
                    }
                    if (auto *ifStmt = dynamic_cast<const IfStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(ifStmt->condition.get(), out);
                        collectReferencedNames(ifStmt->thenBranch.get(), out);
                        collectReferencedNames(ifStmt->elseBranch.get(), out);
                        return;
                    }
                    if (auto *whileStmt = dynamic_cast<const WhileStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(whileStmt->condition.get(), out);
                        collectReferencedNames(whileStmt->body.get(), out);
                        return;
                    }
                    if (auto *forRange = dynamic_cast<const ForRangeStmt *>(stmt))
                    {
                        collectReferencedNames(forRange->body.get(), out);
                        return;
                    }
                    // (AR) `لكل س في مجموعة` — الشكلُ الأشيع في المكتبات؛ كان مفقودًا
                    //      فالمساعِدُ المُنادى داخلَ الحلقة لا يُبلَغ ⇒ «دالّة غير معرّفة».
                    // (EN) `for x in collection` — the commonest form in libraries; missing it
                    //      meant a helper called inside the loop was never reached.
                    if (auto *forStmt = dynamic_cast<const ForStmt *>(stmt))
                    {
                        collectReferencedNames(forStmt->initializer.get(), out);
                        collectReferencedNamesFromExpr(forStmt->condition.get(), out);
                        collectReferencedNamesFromExpr(forStmt->increment.get(), out);
                        collectReferencedNames(forStmt->body.get(), out);
                        return;
                    }
                    if (auto *ret = dynamic_cast<const ReturnStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(ret->value.get(), out);
                        return;
                    }
                    if (auto *sw = dynamic_cast<const SwitchStmt *>(stmt))
                    {
                        for (const auto &c : sw->cases)
                            collectReferencedNames(c.body.get(), out);
                        collectReferencedNames(sw->defaultCase.get(), out);
                        return;
                    }
                    if (auto *tryStmt = dynamic_cast<const TryStmt *>(stmt))
                    {
                        collectReferencedNames(tryStmt->tryBlock.get(), out);
                        for (const auto &c : tryStmt->catchClauses)
                            collectReferencedNames(c.body.get(), out);
                        collectReferencedNames(tryStmt->finallyBlock.get(), out);
                        return;
                    }
                    // (AR) ز.٤١: `ارمي` عقدةُ `RaiseStmt`، وكانت مفقودةً هنا. وهذا هو
                    //      الشكلُ الذي تُبنى به رسائلُ الخطأ في كلِّ مكتبةٍ تقريبًا:
                    //      `ارمي وسم_الخطا + "..." + نص(الموضع)`. فثابتُ الوحدةِ الذي
                    //      لا يُذكَر إلّا داخلَ `ارمي` لا يُبلَغ ⇒ يُحجَب عن البناء ⇒
                    //      `Undefined variable 'وسم_الخطا'` مصرَّفًا بينما المفسّرُ يعمل.
                    // (EN) `raise` is a RaiseStmt node and was missing here. It is the
                    //      shape nearly every library builds its error messages with, so a
                    //      module constant mentioned only inside a raise was never reached
                    //      and got hidden ⇒ "Undefined variable" in the compiler while the
                    //      interpreter worked.
                    if (auto *raise = dynamic_cast<const RaiseStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(raise->exception.get(), out);
                        return;
                    }
                    // (AR) `طابق` — القيمةُ المُختبَرةُ وحارسُ كلِّ ذراعٍ وجسمُها.
                    // (EN) `match` — the scrutinee plus each arm's guard and body.
                    if (auto *match = dynamic_cast<const MatchStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(match->value.get(), out);
                        for (const auto &c : match->cases)
                        {
                            // (AR) النمطُ نفسُه يُسمّي رموزًا: `EnumVariantPattern` باسمِ
                            //      تعدادٍ خاصٍّ بالوحدة. إهمالُه يعيد عيبَ ز.٤١ حرفيًّا في
                            //      العقدةِ التي أُضيفت لعلاجه.
                            // (EN) The pattern itself names symbols (an EnumVariantPattern's
                            //      module-private enum). Skipping it reproduces ز.٤١ exactly.
                            collectReferencedNamesFromPattern(c.pattern.get(), out);
                            collectReferencedNamesFromExpr(c.guard.get(), out);
                            for (const auto &s : c.body)
                                collectReferencedNames(s.get(), out);
                        }
                        return;
                    }
                    if (auto *deferStmt = dynamic_cast<const DeferStmt *>(stmt))
                    {
                        collectReferencedNames(deferStmt->body.get(), out);
                        return;
                    }
                    if (auto *withStmt = dynamic_cast<const WithStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(withStmt->resource.get(), out);
                        collectReferencedNames(withStmt->body.get(), out);
                        return;
                    }
                    if (auto *yield = dynamic_cast<const YieldStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(yield->value.get(), out);
                        return;
                    }
                    if (auto *multiVar = dynamic_cast<const MultiVarDeclStmt *>(stmt))
                    {
                        for (const auto &d : multiVar->declarations)
                            collectReferencedNames(d.get(), out);
                        return;
                    }
                    if (auto *tupleDecl = dynamic_cast<const TupleDestructureStmt *>(stmt))
                    {
                        collectReferencedNamesFromExpr(tupleDecl->initializer.get(), out);
                        return;
                    }
                    if (auto *unsafeBlock = dynamic_cast<const UnsafeBlockStmt *>(stmt))
                    {
                        for (const auto &s : unsafeBlock->body)
                            collectReferencedNames(s.get(), out);
                        return;
                    }
                    if (auto *comptimeBlock = dynamic_cast<const ComptimeBlockStmt *>(stmt))
                    {
                        for (const auto &s : comptimeBlock->body)
                            collectReferencedNames(s.get(), out);
                        return;
                    }
                    // (AR) ⚠️ السقوطُ من هنا صامتٌ بطبعه: أيُّ عقدةِ جملةٍ جديدةٍ لا ذراعَ
                    //      لها تقطع الانتشارَ فيُحجَب رمزٌ خاصٌّ بلا تشخيص. فمَن أضاف
                    //      عقدةَ جملةٍ تحمل تعبيرًا أو جسمًا فعليه ذراعٌ هنا. وز.٤١ نفسُه
                    //      كان هذا السقوطَ بعينِه على `RaiseStmt`.
                    // (EN) Falling through here is silent by nature: any new statement node
                    //      without an arm cuts propagation and hides a private symbol with no
                    //      diagnostic. Whoever adds a statement node carrying an expression or
                    //      a body owes it an arm here — ز.٤١ was exactly this fall-through on
                    //      RaiseStmt.
                }

                void collectReferencedNamesFromExpr(const Sad::AST::Expression *expr,
                                            std::set<std::string> &out)
                {
                    using namespace Sad::AST;
                    if (!expr)
                        return;

                    // (AR) كلُّ معرّفٍ حرٍّ يُجمَع، لا المُستدعَى وحدَه: المساعِدُ قد يُمرَّر
                    //      مرجعًا بلا نداء (مصفوفة.رشح(مساعد))، والثابتُ يُشار إليه اسمًا.
                    // (EN) Every free identifier is collected, not just callees: a helper may
                    //      be passed by reference without a call, and constants appear by name.
                    if (auto *var = dynamic_cast<const VariableExpr *>(expr))
                    {
                        out.insert(var->name);
                        return;
                    }
                    if (auto *call = dynamic_cast<const CallExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(call->callee.get(), out);
                        for (const auto &a : call->arguments)
                            collectReferencedNamesFromExpr(a.get(), out);
                        return;
                    }
                    if (auto *mcall = dynamic_cast<const MethodCallExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(mcall->object.get(), out);
                        for (const auto &a : mcall->arguments)
                            collectReferencedNamesFromExpr(a.get(), out);
                        return;
                    }
                    if (auto *bin = dynamic_cast<const BinaryExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(bin->left.get(), out);
                        collectReferencedNamesFromExpr(bin->right.get(), out);
                        return;
                    }
                    if (auto *un = dynamic_cast<const UnaryExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(un->operand.get(), out);
                        return;
                    }
                    if (auto *arr = dynamic_cast<const ArrayExpr *>(expr))
                    {
                        for (const auto &e : arr->elements)
                            collectReferencedNamesFromExpr(e.get(), out);
                        return;
                    }
                    if (auto *idx = dynamic_cast<const IndexExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(idx->object.get(), out);
                        collectReferencedNamesFromExpr(idx->index.get(), out);
                        return;
                    }
                    if (auto *assign = dynamic_cast<const AssignExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(assign->value.get(), out);
                        return;
                    }
                    if (auto *ternary = dynamic_cast<const TernaryExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(ternary->condition.get(), out);
                        collectReferencedNamesFromExpr(ternary->trueExpr.get(), out);
                        collectReferencedNamesFromExpr(ternary->falseExpr.get(), out);
                        return;
                    }
                    if (auto *mapExpr = dynamic_cast<const MapExpr *>(expr))
                    {
                        for (const auto &pair : mapExpr->pairs)
                        {
                            collectReferencedNamesFromExpr(pair.key.get(), out);
                            collectReferencedNamesFromExpr(pair.value.get(), out);
                        }
                        return;
                    }
                    // (AR) اللامدا بجسمٍ تعبيريٍّ أو كتليّ — المساعِدُ المُنادى داخلها يُبلَغ
                    // (EN) Lambda with an expression or block body — a helper called inside is reached
                    if (auto *lambda = dynamic_cast<const LambdaExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(lambda->body.get(), out);
                        collectReferencedNames(lambda->blockBody.get(), out);
                        return;
                    }
                    if (auto *comp = dynamic_cast<const ListComprehensionExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(comp->element.get(), out);
                        collectReferencedNamesFromExpr(comp->iterable.get(), out);
                        collectReferencedNamesFromExpr(comp->condition.get(), out);
                        return;
                    }
                    // (AR) الوصولُ إلى عضو: `ثابت_خاص.حقل` — الكائنُ معرّفٌ حرٌّ يُبلَغ منه
                    //      الثابتُ/الصنفُ الخاصّ. شكلان للعقدة في الشجرة.
                    // (EN) Member access: `private_const.field` — the object is a free
                    //      identifier reaching a private constant/class. Two node shapes.
                    if (auto *member = dynamic_cast<const MemberExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(member->object.get(), out);
                        return;
                    }
                    if (auto *memberAccess = dynamic_cast<const MemberAccessExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(memberAccess->object.get(), out);
                        return;
                    }
                    // (AR) `جديد صنف_خاص(...)`: اسمُ الصنفِ لا يظهر معرّفًا حرًّا بل حقلًا
                    //      نصّيًّا، فلولا إدراجُه صراحةً حُجِب الصنفُ ⇒ «صنف غير معرّف».
                    // (EN) `new PrivateClass(...)`: the class name is a string field, not a
                    //      free identifier; without inserting it explicitly the class is
                    //      hidden ⇒ "undefined class".
                    if (auto *newExpr = dynamic_cast<const NewExpr *>(expr))
                    {
                        out.insert(newExpr->className);
                        // (AR) وأسماءُ وسائطِ القالبِ نصوصٌ كذلك: `جديد حاوية<صنف_خاص>()`.
                        // (EN) Template argument names are strings too.
                        for (const auto &t : newExpr->templateArgumentNames)
                            out.insert(t);
                        for (const auto &a : newExpr->arguments)
                            collectReferencedNamesFromExpr(a.get(), out);
                        return;
                    }
                    // (AR) الوسيطُ المسمّى `اسم: قيمة` عقدةٌ مستقلّةٌ تُدفَع في `arguments`،
                    //      فذراعا النداءِ تمرّان عليها ولا تجدان لها ذراعًا ⇒ تُهمَل القيمةُ
                    //      بالكامل ⇒ ثابتُ وحدةٍ لا يُذكَر إلّا وسيطًا مسمّى يُحجَب. وهو
                    //      عيبُ ز.٤١ حرفيًّا في أشيعِ أشكالِ النداءِ بعد الموضعيّ.
                    // (EN) A named argument `name: value` is its own node pushed into
                    //      `arguments`; the call arms walk into it and find no arm, so the
                    //      value is dropped entirely — ز.٤١ again, in the second commonest
                    //      call shape.
                    if (auto *namedArg = dynamic_cast<const NamedArgExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(namedArg->value.get(), out);
                        return;
                    }
                    // (AR) `ثابت_خاص?.حقل` — نظيرُ `MemberExpr` تمامًا.
                    // (EN) `private_const?.field` — the exact sibling of MemberExpr.
                    if (auto *optChain = dynamic_cast<const OptionalChainExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(optChain->object.get(), out);
                        return;
                    }
                    if (auto *tuple = dynamic_cast<const TupleExpr *>(expr))
                    {
                        for (const auto &e : tuple->elements)
                            collectReferencedNamesFromExpr(e.get(), out);
                        return;
                    }
                    if (auto *slice = dynamic_cast<const SliceExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(slice->object.get(), out);
                        collectReferencedNamesFromExpr(slice->start.get(), out);
                        collectReferencedNamesFromExpr(slice->end.get(), out);
                        collectReferencedNamesFromExpr(slice->step.get(), out);
                        return;
                    }
                    if (auto *range = dynamic_cast<const RangeExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(range->start.get(), out);
                        collectReferencedNamesFromExpr(range->end.get(), out);
                        return;
                    }
                    if (auto *await = dynamic_cast<const AwaitExpr *>(expr))
                    {
                        collectReferencedNamesFromExpr(await->expression.get(), out);
                        return;
                    }
                    // (AR) ⚠️ السقوطُ من هنا صامتٌ كنظيرِه في جوّالِ الجُمَل — عقدةُ تعبيرٍ
                    //      بلا ذراعٍ تقطع الانتشارَ فيُحجَب رمزٌ خاصٌّ بلا تشخيص.
                    // (EN) ⚠️ Falling through here is silent, like its statement-walker
                    //      sibling: an expression node without an arm cuts propagation and
                    //      hides a private symbol with no diagnostic.
                }

        // (AR) سجلُّ تصريحٍ علويّ: ما يمكن أن يبلغ منه اسمٌ آخر. الدالّةُ جسمٌ،
        //      والثابتُ مُهيّئٌ، والصنفُ قائمةُ أعضاءٍ (أجسامُ طرقِه).
        // (EN) A top-level declaration record: everything through which another name
        //      can be reached — a function's body, a constant's initializer, a class's
        //      member list (its method bodies).
        namespace
        {
            struct DeclReach
            {
                const Statement *body = nullptr;
                const Expression *initializer = nullptr;
                const StmtList *classMembers = nullptr;
            };
        } // namespace

        std::set<std::string> computeExportedPrivateClosure(
            const std::vector<std::unique_ptr<Statement>> &moduleAst,
            const std::set<std::string> &requestedSymbols,
            bool isWildcard)
        {
            // (AR) فهرسةُ تصريحات الوحدة العلويّة: الاسم ⇒ (ما يُبلَغ منه، أمُصدَّرٌ هو؟).
            //      تشمل الدوالَّ والثوابتَ والأصنافَ والبنى — فالخاصُّ من أيِّ نوعٍ يخضع
            //      للتصفية نفسِها.
            // (EN) Index the module's top-level declarations: name ⇒ (what it reaches,
            //      exported?). Functions, constants, classes and structs alike.
            std::unordered_map<std::string, DeclReach> privateDecls;
            std::unordered_map<std::string, DeclReach> exportedDecls;

            auto indexDecl = [&](const Statement *decl, bool wrappedExport)
            {
                std::string name;
                DeclReach reach;
                // (AR) «صدّر تصريح» تُلَفُّ بعقدةِ `ExportDecl` (يفكّها المنادي أدناه)،
                //      والرايةُ `isExported` تُقرأ احتياطًا لمن يبني العقدةَ مباشرةً بلا
                //      لفّ — فلا يُفهرَس مُصدَّرٌ خاصًّا في أيِّ الحالين.
                // (EN) «صدّر <decl>» is wrapped in an ExportDecl (unwrapped by the caller
                //      below); the declaration's isExported flag is read as a fallback for
                //      nodes built directly without a wrapper, so an export is never
                //      indexed as private in either shape.
                bool exported = wrappedExport;

                // (AR) التصريحُ غيرُ الدالّة يحمل جسمًا أيضًا: طرقُ الصنفِ المُصدَّرِ
                //      ومُهيّئُ الثابتِ يبلغان المساعِدَ الخاصَّ بلا شكّ. وتسجيلُهما
                //      بجسمٍ صفريٍّ كان يقطع الانتشارَ عندهما ⇒ `صدّر صنف عدّاد`
                //      تنادي `داخلي` فتُحجَب `داخلي` ⇒ «دالّة غير معرّفة».
                // (EN) Non-function declarations carry bodies too: an exported class's
                //      methods and a constant's initializer do reach the private helper.
                //      Indexing them with a null body cut propagation there.
                if (auto *fn = dynamic_cast<const FunctionDecl *>(decl))
                {
                    name = fn->name;
                    reach.body = fn->body.get();
                    exported = exported || fn->isExported;
                }
                else if (auto *vd = dynamic_cast<const VarDeclStmt *>(decl))
                {
                    name = vd->name;
                    reach.initializer = vd->initializer.get();
                }
                else if (auto *cd = dynamic_cast<const ClassDecl *>(decl))
                {
                    name = cd->name;
                    reach.classMembers = &cd->members;
                    exported = exported || cd->isExported;
                }
                else if (auto *sd = dynamic_cast<const StructDecl *>(decl))
                {
                    name = sd->name;
                }
                else
                {
                    return;
                }

                if (exported)
                    exportedDecls[name] = reach;
                else
                    privateDecls[name] = reach;
            };

            for (const auto &stmt : moduleAst)
            {
                if (!stmt)
                    continue;

                if (auto *ed = dynamic_cast<ExportDecl *>(stmt.get()))
                {
                    if (ed->declaration)
                        indexDecl(ed->declaration.get(), true);
                }
                else if (auto *es = dynamic_cast<ExportStmt *>(stmt.get()))
                {
                    if (es->declaration)
                        indexDecl(es->declaration.get(), true);
                }
                else
                {
                    indexDecl(stmt.get(), false);
                }
            }

            // (AR) بذرةُ الانتشار: الرموزُ المطلوبة (أو كلُّ المُصدَّر عند الاستيراد الشامل)
            // (EN) Propagation seed: requested symbols (or every export on a wildcard import)
            std::vector<DeclReach> worklist;
            std::set<std::string> reachablePrivate;
            std::set<std::string> visited;

            auto enqueue = [&](const std::string &name)
            {
                if (!visited.insert(name).second)
                    return;
                auto expIt = exportedDecls.find(name);
                if (expIt != exportedDecls.end())
                {
                    worklist.push_back(expIt->second);
                    return;
                }
                auto privIt = privateDecls.find(name);
                if (privIt != privateDecls.end())
                {
                    reachablePrivate.insert(name);
                    worklist.push_back(privIt->second);
                }
            };

            if (isWildcard)
            {
                for (const auto &entry : exportedDecls)
                    enqueue(entry.first);
            }
            else
            {
                for (const auto &name : requestedSymbols)
                    enqueue(name);
            }

            // (AR) انتشارٌ حتى الثبات: كلُّ اسمٍ حرٍّ في تصريحٍ مشمولٍ يطابق خاصًّا يُضاف،
            //      والمطابقُ لمُصدَّرٍ يُتابَع كي تُبلَغ مساعِداتُه الخاصّة. و`visited`
            //      يقطع الدورةَ (دالّتان تتناديان) فلا حلقةَ لا نهائيّة.
            // (EN) Propagate to fixpoint: every free name in an included declaration that
            //      matches a private one is added; a name matching an export is followed too
            //      so its own private helpers are reached. `visited` breaks cycles (mutually
            //      recursive functions), so the loop always terminates.
            while (!worklist.empty())
            {
                const DeclReach reach = worklist.back();
                worklist.pop_back();

                std::set<std::string> referencedNames;
                collectReferencedNames(reach.body, referencedNames);
                collectReferencedNamesFromExpr(reach.initializer, referencedNames);
                if (reach.classMembers)
                {
                    for (const auto &member : *reach.classMembers)
                        collectReferencedNames(member.get(), referencedNames);
                }
                for (const auto &name : referencedNames)
                    enqueue(name);
            }

            return reachablePrivate;
        }
    } // namespace AST
} // namespace Sad
