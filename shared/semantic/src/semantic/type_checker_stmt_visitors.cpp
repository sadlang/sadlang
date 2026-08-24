// ============================================================================
// type_checker_stmt_visitors.cpp - زوار الجمل والتعريفات
// ============================================================================
// (AR) زوار الجمل: ExprStmt, VarDeclStmt, IfStmt, WhileStmt, ForStmt...
//      زوار التعريفات: ClassDecl, FunctionDecl, EnumDecl, ImportStmt...
// (EN) Statement visitors: ExprStmt, VarDeclStmt, IfStmt, WhileStmt, ForStmt...
//      Declaration visitors: ClassDecl, FunctionDecl, EnumDecl, ImportStmt...
// تم استخراج هذا الملف من type_checker.cpp وفقاً لقاعدة CW-05
// ============================================================================
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

#include "semantic/type_checker.h"
#include "token.h"
#include "class_nodes.h"
#include "pattern_nodes.h"    // (AR) [أ-م٢] MatchStmt / CaseClause / ConstructorPattern / أنماط
#include "types/composite_type_classes.h"
// (AR) SEM045 (D8): قاعدة «الفراغ اليقيني» المشتركة — لا نسخة ثالثة في الفاحص
#include "null_safety/null_safety_analyzer.h"
#include "types/enum_types.h"
#include "types/struct_types.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <functional> // (AR) [أ-م٢] std::function لتصريح المتغيّرات المربوطة عوديًّا

namespace Sad
{
    namespace Semantic
    {

        using namespace TypeSystem;
        using TT = Lexer::TokenType;

        void TypeChecker::visitExprStmt(AST::ExprStmt &stmt)
        {
            if (stmt.expression)
            {
                inferExprType(stmt.expression.get());
            }
        }

        void TypeChecker::visitVarDeclStmt(AST::VarDeclStmt &stmt)
        {
            currentResult_.totalVariables++;

            // النوع المُصرّح / Declared type
            TypePtr declaredType = dataTypeToTypePtr(stmt.type);

            // النوع المُستنتج من القيمة / Inferred type from initializer
            TypePtr initType = nullptr;
            if (stmt.initializer)
            {
                initType = inferExprType(stmt.initializer.get());
            }

            // تسجيل نوع المتغير
            TypePtr finalType = declaredType;

            // إذا كان النوع المُصرّح unknown، استخدم المُستنتج
            if (declaredType && declaredType->isUnknown() && initType && !initType->isUnknown())
            {
                finalType = initType;
                currentResult_.totalInferred++;
            }

            // التحقق من التوافق / Check compatibility
            // (AR) ملاحظة (S-TS-P9): فرض أمان null في طبقة المفسّر (statement_executor) عبر isNull.
            if (initType && declaredType && !declaredType->isUnknown() && !initType->isUnknown())
            {
                if (!areTypesCompatible(declaredType, initType))
                {
                    recordTypeError(stmt.name,
                                    declaredType->toString(), initType->toString(),
                                    &stmt);
                }
            }

            // تسجيل في البيئة / Register in environment
            declareVariable(stmt.name, finalType);

            // ============================================================
            // (AR) [Phase 5c] إن أمكن، استخرج اسم صنف المُهيِّئ وسجّله
            //      في النطاق الحالي للمُتغير. هذا يُمكِّن الاستدلال
            //      النوعي لاحقاً عند تمرير هذا المتغير لقالب.
            //   حالات مدعومة:
            //     1) NewExpr: متغير ن = نقطة(3،4) جديد        → "نقطة"
            //     2) CallExpr لاسم صنف معروف: متغير ن = نقطة(3،4) → "نقطة"
            //     3) typeName في الأنواع المعممة الجديدة (sadType)
            // (EN) [Phase 5c] If possible, extract the initializer's class
            //      name and record it in the current scope for this var.
            //      This enables later type inference when the var is
            //      passed to a template.
            //   Supported cases:
            //     1) NewExpr  → ne->className
            //     2) CallExpr targeting a known class name (constructor)
            //     3) typeName from the new unified type system (sadType)
            // ============================================================
            if (!variableClassNames_.empty() && stmt.initializer)
            {
                std::string className;
                AST::Expression *init = stmt.initializer.get();

                if (auto *ne = dynamic_cast<AST::NewExpr *>(init))
                {
                    className = ne->className;
                }
                else if (auto *call = dynamic_cast<AST::CallExpr *>(init))
                {
                    if (auto *ve = dynamic_cast<AST::VariableExpr *>(call->callee.get()))
                    {
                        // (AR) باني صنف معروف؟
                        // (EN) Known class constructor?
                        if (StructRegistry::instance().isStructType(ve->name) ||
                            userClassNames_.count(ve->name))
                            className = ve->name;
                    }
                }

                if (!className.empty())
                {
                    variableClassNames_.back()[stmt.name] = className;
                }
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] VarDecl '" << stmt.name << "': "
                          << (finalType ? finalType->toString() : "unknown")
                          << (stmt.isConst ? " (const)" : "") << "\n";
            }
        }

        void TypeChecker::visitIfStmt(AST::IfStmt &stmt)
        {
            // تحقق من نوع الشرط / Check condition type
            if (stmt.condition)
            {
                TypePtr condType = inferExprType(stmt.condition.get());
                if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown())
                {
                    recordTypeError("", "boolean", condType->toString(), &stmt,
                                    "If condition should be boolean");
                }
            }

            // فحص الفرعين / Check both branches
            if (stmt.thenBranch)
            {
                enterScope();
                stmt.thenBranch->accept(*this);
                exitScope();
            }
            if (stmt.elseBranch)
            {
                enterScope();
                stmt.elseBranch->accept(*this);
                exitScope();
            }
        }

        void TypeChecker::visitWhileStmt(AST::WhileStmt &stmt)
        {
            if (stmt.condition)
            {
                TypePtr condType = inferExprType(stmt.condition.get());
                if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown())
                {
                    recordTypeError("", "boolean", condType->toString(), &stmt,
                                    "While condition should be boolean");
                }
            }

            if (stmt.body)
            {
                enterScope();
                stmt.body->accept(*this);
                exitScope();
            }
        }

        void TypeChecker::visitForStmt(AST::ForStmt &stmt)
        {
            enterScope();
            if (stmt.initializer)
                stmt.initializer->accept(*this);
            if (stmt.condition)
                inferExprType(stmt.condition.get());
            if (stmt.increment)
                inferExprType(stmt.increment.get());
            if (stmt.body)
                stmt.body->accept(*this);
            exitScope();
        }

        void TypeChecker::visitForRangeStmt(AST::ForRangeStmt &stmt)
        {
            enterScope();

            if (stmt.iterable)
            {
                inferExprType(stmt.iterable.get());
            }

            // تسجيل متغير الحلقة / Register loop variable
            declareVariable(stmt.variable, registry_.getUnknownType());
            if (!stmt.valueVar.empty())
            {
                declareVariable(stmt.valueVar, registry_.getUnknownType());
            }

            if (stmt.body)
                stmt.body->accept(*this);
            exitScope();
        }

        void TypeChecker::visitSwitchStmt(AST::SwitchStmt &stmt)
        {
            TypePtr switchType = nullptr;
            if (stmt.expression)
            {
                switchType = inferExprType(stmt.expression.get());
            }

            for (auto &caseBranch : stmt.cases)
            {
                if (caseBranch.value)
                {
                    TypePtr caseType = inferExprType(caseBranch.value.get());
                    if (switchType && caseType && !areTypesCompatible(switchType, caseType))
                    {
                        recordTypeError("", switchType->toString(), caseType->toString(),
                                        caseBranch.value.get(),
                                        "Switch case type mismatch");
                    }
                }
                if (caseBranch.body)
                {
                    enterScope();
                    caseBranch.body->accept(*this);
                    exitScope();
                }
            }

            if (stmt.defaultCase)
            {
                enterScope();
                stmt.defaultCase->accept(*this);
                exitScope();
            }
        }

        // ============================================================================
        // (AR) [أ-م٢] مطابقة الأنماط «طابق» — الحاجز ٢ (الأنواع الجبريّة الموسومة)
        // (EN) [A-M2] «طابق» pattern match — barrier 2 (tagged algebraic data types)
        //
        // (AR) ثلاث مهامّ دلاليّة:
        //   ١) حسم هويّة نمط الباني غير المؤهَّل «عندما عدد(ق)»: ربطه بمعامل التعداد
        //      الصحيح عبر السجلّ (variantOwners_) دون تأهيلٍ صريح.
        //   ٢) فحص عدد حمولة كلّ نمط باني مقابل fieldTypes (SEM_WRONG_ARG_COUNT).
        //   ٣) استنفاد المطابقة: إن لم تُغطَّ كلّ معاملات التعداد ولا يوجد فرعٌ شامل
        //      («افتراضي» ⇒ WildcardPattern، أو «عندما _»، أو نمط متغيّر بلا حارس)
        //      يُصدَر SEM_NON_EXHAUSTIVE_MATCH.
        // (EN) Three semantic duties: (1) resolve unqualified constructor-pattern identity
        //   via the registry; (2) check each constructor pattern's payload arity vs
        //   fieldTypes; (3) enforce exhaustiveness, emitting SEM_NON_EXHAUSTIVE_MATCH.
        // ============================================================================
        void TypeChecker::visitMatchStmt(AST::MatchStmt &stmt)
        {
            // (AR) استنتاج نوع المُطابَق عليه (أفضل جهد؛ الهويّة تُحسم من المعاملات). / best-effort
            if (stmt.value)
            {
                inferExprType(stmt.value.get());
            }

            // (AR) المرحلة ١: حسم الهويّة + فحص العدد + جمع التغطية.
            // (EN) Pass 1: identity resolution + arity checks + coverage collection.
            std::string matchedEnum;                 // (AR) التعداد المُطابَق / (EN) matched enum
            std::unordered_set<std::string> covered; // (AR) المعاملات المُغطّاة / (EN) covered variants
            bool hasCatchAll = false;                // (AR) فرعٌ شامل موجود؟ / (EN) catch-all present?
            bool enumPinnedByQualified = false;      // (AR) ثُبّت التعداد بنمط مؤهَّل؟ / pinned by qualified pattern?
            bool ambiguousUnresolved = false;        // (AR) التباسٌ لم يُحسَم / unresolved ambiguity

            // (AR) دالّة فحص عدد الحمولة المشتركة بين النمطين (نُعيد استعمال SEM_WRONG_ARG_COUNT).
            // (EN) Shared payload-arity check for both pattern kinds (reuse SEM_WRONG_ARG_COUNT).
            auto checkPayloadArity =
                [&](const std::string &variantName, const EnumVariantInfo &variant, size_t found)
            {
                const size_t expected = variant.fieldTypes.size();
                if (found != expected)
                {
                    reportCatalogError(
                        Errors::ErrorCode::SEM_WRONG_ARG_COUNT,
                        {{"name", variantName},
                         {"expected", std::to_string(expected)},
                         {"found", std::to_string(found)}},
                        &stmt);
                }
            };

            for (auto &clause : stmt.cases)
            {
                AST::Pattern *pat = clause.pattern.get();
                if (!pat)
                    continue;

                // (AR) نمط باني مؤهَّل: «شكل.مربع(ع)» — التعداد صريح فيه (🔴-١ من مراجعة أميليا).
                // (EN) Qualified variant pattern «شكل.مربع(ع)» — enum name is explicit (Amelia 🔴-1).
                if (auto *ev = dynamic_cast<AST::EnumVariantPattern *>(pat))
                {
                    // (AR) الاسم المؤهَّل مرجعٌ حاسمٌ للهويّة (يتقدّم على الاستدلال من المعامل).
                    // (EN) The qualified enum name authoritatively pins identity.
                    if (!ev->enumName.empty())
                    {
                        matchedEnum = ev->enumName;
                        enumPinnedByQualified = true;
                    }
                    std::string owningEnum;
                    bool ambiguous = false;
                    const EnumVariantInfo *variant =
                        lookupVariant(ev->variantName, owningEnum, ambiguous);
                    if (variant)
                    {
                        if (matchedEnum.empty())
                            matchedEnum = owningEnum;
                        checkPayloadArity(ev->variantName, *variant, ev->fieldPatterns.size());
                    }
                    covered.insert(ev->variantName);
                }
                else if (auto *ctor = dynamic_cast<AST::ConstructorPattern *>(pat))
                {
                    std::string owningEnum;
                    bool ambiguous = false;
                    const EnumVariantInfo *variant =
                        lookupVariant(ctor->variantName, owningEnum, ambiguous);
                    if (variant)
                    {
                        // (AR) الهويّة: أوّل معامل معروف يثبّت تعداد المطابقة (ما لم يثبّته مؤهَّل).
                        // (EN) Identity: first known variant fixes the enum (unless a qualified pattern pinned it).
                        if (matchedEnum.empty())
                            matchedEnum = owningEnum;
                        // (AR) التباسٌ غير محسومٍ بمؤهَّل ⇒ لا نُشغّل الاستنفاد (🔴-٣ من مراجعة أميليا).
                        // (EN) Ambiguity not pinned by a qualified pattern ⇒ skip exhaustiveness (Amelia 🔴-3).
                        if (ambiguous && !enumPinnedByQualified)
                            ambiguousUnresolved = true;
                        checkPayloadArity(ctor->variantName, *variant, ctor->fieldPatterns.size());
                        covered.insert(ctor->variantName);
                    }
                    // (AR) معامل غير معروف: خارج نطاق أ-م٢ (لا خطأ هنا). / unknown: out of A-M2 scope
                }
                else if (dynamic_cast<AST::WildcardPattern *>(pat))
                {
                    // (AR) «افتراضي» أو «عندما _» — يلتقط كلّ ما تبقّى.
                    // (EN) «افتراضي» or «عندما _» — catches everything else.
                    hasCatchAll = true;
                }
                else if (auto *var = dynamic_cast<AST::VariablePattern *>(pat))
                {
                    // (AR) اسمٌ عارٍ قد يكون معاملًا وحدويًّا («فراغ») لا رابطَ متغيّرٍ شاملًا
                    //      (🟠-٤): إن طابق معاملًا وحدويًّا معروفًا (بلا حمولة) لتعداد المطابقة،
                    //      نعدّه تغطيةً لا فرعًا شاملًا؛ وإلّا فهو رابطٌ شاملٌ ما لم يقيّده حارس.
                    // (EN) A bare name may be a UNIT variant («فراغ»), not a catch-all binding
                    //      (🟠-4): if it matches a known unit variant (no payload) of the matched
                    //      enum, count it as coverage; otherwise it is a catch-all unless guarded.
                    // (AR) نعُدّ الاسم العاري تغطيةَ معاملٍ وحدويٍّ فقط إذا كانت هويّة التعداد
                    //      مثبَّتةً سلفًا (بنمط بانٍ/مؤهَّل) وتطابقه — كي لا يثبّت متغيّرٌ رابطٌ عابرٌ
                    //      تعدادًا أجنبيًّا فيُطلق استنفادًا كاذبًا (انحدار كشفته أميليا).
                    // (EN) Count a bare name as unit-variant coverage ONLY when the enum identity is
                    //      already pinned (by a ctor/qualified pattern) and matches — so a stray binding
                    //      whose name coincides with a foreign unit variant cannot pin it and trigger a
                    //      false non-exhaustive error (regression caught by Amelia).
                    std::string owningEnum;
                    bool ambiguous = false;
                    const EnumVariantInfo *unitVariant =
                        matchedEnum.empty() ? nullptr : lookupVariant(var->name, owningEnum, ambiguous);
                    const bool isUnitVariantOfMatch =
                        unitVariant && unitVariant->fieldTypes.empty() && owningEnum == matchedEnum;
                    if (isUnitVariantOfMatch)
                    {
                        covered.insert(var->name);
                    }
                    else if (!clause.guard)
                    {
                        hasCatchAll = true;
                    }
                }
                else if (dynamic_cast<AST::BindingPattern *>(pat))
                {
                    // (AR) نمط ربط عارٍ يربط كلّ قيمة ⇒ شامل، ما لم يقيّده حارس.
                    // (EN) A bare binding pattern binds every value ⇒ catch-all, unless guarded.
                    if (!clause.guard)
                        hasCatchAll = true;
                }
            }

            // (AR) دالّة تصريح المتغيّرات المربوطة في نمط (كي تُفحص الأجسام سليمةً).
            // (EN) Declare pattern-bound variables (so bodies type-check cleanly).
            std::function<void(AST::Pattern *)> declareBindings =
                [&](AST::Pattern *pat)
            {
                if (!pat)
                    return;
                if (auto *v = dynamic_cast<AST::VariablePattern *>(pat))
                {
                    declareVariable(v->name, registry_.getUnknownType());
                }
                else if (auto *b = dynamic_cast<AST::BindingPattern *>(pat))
                {
                    declareVariable(b->name, registry_.getUnknownType());
                    declareBindings(b->pattern.get());
                }
                else if (auto *c = dynamic_cast<AST::ConstructorPattern *>(pat))
                {
                    for (auto &fp : c->fieldPatterns)
                        declareBindings(fp.get());
                }
                else if (auto *ev = dynamic_cast<AST::EnumVariantPattern *>(pat))
                {
                    for (auto &fp : ev->fieldPatterns)
                        declareBindings(fp.get());
                }
            };

            // (AR) المرحلة ٢: زيارة أجسام الفروع في نطاقات معزولة مع المتغيّرات المربوطة.
            // (EN) Pass 2: visit branch bodies in isolated scopes with bound variables.
            for (auto &clause : stmt.cases)
            {
                enterScope();
                declareBindings(clause.pattern.get());
                if (clause.guard)
                    inferExprType(clause.guard.get());
                for (auto &s : clause.body)
                {
                    if (s)
                        s->accept(*this);
                }
                exitScope();
            }

            // (AR) المرحلة ٣: استنفاد المطابقة — فقط لتعدادٍ محسومٍ، بلا فرعٍ شامل، وبلا التباسٍ عالق.
            // (EN) Pass 3: exhaustiveness — only for a resolved enum, no catch-all, no unresolved ambiguity.
            if (!matchedEnum.empty() && !hasCatchAll && !ambiguousUnresolved)
            {
                auto enumIt = enumVariants_.find(matchedEnum);
                if (enumIt != enumVariants_.end())
                {
                    std::vector<std::string> missing;
                    for (const auto &v : enumIt->second)
                    {
                        if (covered.find(v.variantName) == covered.end())
                            missing.push_back(v.variantName);
                    }
                    if (!missing.empty())
                    {
                        std::string missingStr;
                        for (size_t i = 0; i < missing.size(); ++i)
                        {
                            if (i > 0)
                                missingStr += "، ";
                            missingStr += missing[i];
                        }
                        reportCatalogError(
                            Errors::ErrorCode::SEM_NON_EXHAUSTIVE_MATCH,
                            {{"enum", matchedEnum}, {"missing", missingStr}},
                            &stmt);
                    }
                }
            }
        }

        void TypeChecker::visitReturnStmt(AST::ReturnStmt &stmt)
        {
            TypePtr retType = registry_.getVoidType();
            if (stmt.value)
            {
                retType = inferExprType(stmt.value.get());
            }

            // تحقق من توافق نوع الإرجاع / Check return type compatibility
            if (expectedReturnType_ && retType)
            {
                if (!areTypesCompatible(expectedReturnType_, retType))
                {
                    recordTypeError(currentFunction_,
                                    expectedReturnType_->toString(), retType->toString(),
                                    &stmt,
                                    "Return type mismatch in function '" + currentFunction_ + "'");
                }
            }
        }

        void TypeChecker::visitYieldStmt(AST::YieldStmt &stmt)
        {
            if (stmt.value)
            {
                inferExprType(stmt.value.get());
            }
        }

        void TypeChecker::visitBreakStmt(AST::BreakStmt & /*stmt*/)
        {
            // لا فحص أنواع / No type checking needed
        }

        void TypeChecker::visitContinueStmt(AST::ContinueStmt & /*stmt*/)
        {
            // لا فحص أنواع / No type checking needed
        }

        void TypeChecker::visitBlockStmt(AST::BlockStmt &stmt)
        {
            enterScope();
            for (auto &s : stmt.statements)
            {
                if (s)
                    s->accept(*this);
            }
            exitScope();
        }

        void TypeChecker::visitTryStmt(AST::TryStmt &stmt)
        {
            if (stmt.tryBlock)
            {
                enterScope();
                stmt.tryBlock->accept(*this);
                exitScope();
            }

            for (auto &clause : stmt.catchClauses)
            {
                enterScope();
                if (!clause.exceptionVar.empty())
                {
                    declareVariable(clause.exceptionVar, dataTypeToTypePtr(clause.exceptionType));
                }
                if (clause.body)
                    clause.body->accept(*this);
                exitScope();
            }

            if (stmt.finallyBlock)
            {
                enterScope();
                stmt.finallyBlock->accept(*this);
                exitScope();
            }
        }

        void TypeChecker::visitRaiseStmt(AST::RaiseStmt &stmt)
        {
            if (stmt.exception)
            {
                inferExprType(stmt.exception.get());
            }
        }

        void TypeChecker::visitWithStmt(AST::WithStmt &stmt)
        {
            enterScope();
            if (stmt.resource)
            {
                TypePtr resType = inferExprType(stmt.resource.get());
                if (!stmt.alias.empty())
                {
                    declareVariable(stmt.alias, resType);
                }
            }
            if (stmt.body)
                stmt.body->accept(*this);
            exitScope();
        }

        // ============================================================================
        // (AR) جمل التزامن — أطلق / اختر / (EN) Concurrency statements — go / select
        // ============================================================================

        void TypeChecker::visitGoStmt(AST::GoStmt &stmt)
        {
            // (AR) فحص التعبير المُطلَق (استدعاء دالة أو لامدا)
            // (EN) Check the launched expression (function call or lambda)
            if (stmt.expression)
            {
                inferExprType(stmt.expression.get());
            }
            // (AR) فحص كتلة الكود إذا كان "أطلق ... نهاية"
            // (EN) Check block body if "go ... end"
            if (stmt.blockBody)
            {
                enterScope();
                stmt.blockBody->accept(*this);
                exitScope();
            }
        }

        void TypeChecker::visitSelectCase(AST::SelectCase &stmt)
        {
            // (AR) فحص تعبير القناة
            // (EN) Check channel expression
            if (stmt.channelExpr)
            {
                inferExprType(stmt.channelExpr.get());
            }
            // (AR) فحص جسم الحالة
            // (EN) Check case body
            enterScope();
            if (!stmt.bindVar.empty())
            {
                declareVariable(stmt.bindVar, registry_.getUnknownType());
            }
            for (auto &s : stmt.body)
            {
                if (s)
                    s->accept(*this);
            }
            exitScope();
        }

        void TypeChecker::visitSelectStmt(AST::SelectStmt &stmt)
        {
            // (AR) فحص كل حالة اختيار
            // (EN) Check each select case
            for (auto &c : stmt.cases)
            {
                if (c)
                    c->accept(*this);
            }
            // (AR) فحص الجسم الافتراضي
            // (EN) Check default body
            if (!stmt.defaultBody.empty())
            {
                enterScope();
                for (auto &s : stmt.defaultBody)
                {
                    if (s)
                        s->accept(*this);
                }
                exitScope();
            }
        }

        void TypeChecker::visitClassDeclStmt(AST::ClassDeclStmt &stmt)
        {
            // (AR) سجّل اسم الصنف محلياً ليُستخدم في الاستدلال النوعي.
            // (EN) Register class name locally for type inference fallback.
            userClassNames_.insert(stmt.name);

            // (AR) SEM045 (حقول الأصناف): دوّن تصنيفَ الحقول وحدّد الصنفَ الجاري
            //      فحصُه — يقرؤهما حارسُ «الفراغُ لا يعبر إلى حقلٍ مصنَّف» في
            //      visitMemberAssignExpr. الاستعادةُ بعد الأعضاء تحفظ التداخل.
            // (EN) SEM045 (class fields): record field kinds and the class under
            //      check — read by the member-assign Void guard. Restored after
            //      the members so nesting stays correct.
            std::string prevCheckedClass = currentCheckedClassName_;
            currentCheckedClassName_ = stmt.name;
            for (auto &field : stmt.fields)
            {
                if (field)
                    classFieldKinds_[stmt.name][field->name] = field->type;
            }
            // (AR) الحقولُ الموروثة تدخل الجدولَ أيضًا (insert لا يطمس تظليلَ
            //      الابن) — وإلّا أفلت `هذا.حقل_موروث = فراغية()` من الحارس
            //      الساكن (رصدُ مراجعة الجودة). الأبُ غيرُ المُصرَّح بعدُ حدٌّ
            //      طبيعيّ: جدولُه فارغٌ فلا شيءَ يُدمَج.
            // (EN) Inherited fields enter the table too (insert never clobbers a
            //      child override) — else `this.inheritedField = void()` escapes
            //      the static guard (quality review). A base declared later is a
            //      natural limit: its table is empty, nothing merges.
            for (const auto &baseName : stmt.baseClasses)
            {
                auto baseIt = classFieldKinds_.find(baseName);
                if (baseIt != classFieldKinds_.end())
                {
                    classFieldKinds_[stmt.name].insert(baseIt->second.begin(),
                                                       baseIt->second.end());
                }
            }

            enterScope();

            // تسجيل الحقول / Register fields
            for (auto &field : stmt.fields)
            {
                if (field)
                    field->accept(*this);
            }

            // فحص المُنشئ / Check constructor
            if (stmt.constructor)
            {
                stmt.constructor->accept(*this);
            }

            // فحص المُدمّر / Check destructor
            if (stmt.destructor)
            {
                stmt.destructor->accept(*this);
            }

            // فحص الدوال / Check methods
            for (auto &method : stmt.methods)
            {
                if (method)
                    method->accept(*this);
            }

            exitScope();
            currentCheckedClassName_ = prevCheckedClass;
        }

        // ============================================================================
        // زيارة التصريحات / Visit Declarations
        // ============================================================================

        void TypeChecker::visitFunctionDecl(AST::FunctionDecl &decl)
        {
            currentResult_.totalFunctions++;

            std::string prevFunction = currentFunction_;
            TypePtr prevReturnType = expectedReturnType_;

            currentFunction_ = decl.name;
            // (AR) إذا كان نوع الإرجاع غير معروف أو فراغ — محور SadTypeKind (S-TS-P2)
            // (EN) If return type is Unknown or Void — SadTypeKind-centric (S-TS-P2)
            if (decl.returnType == Types::SadTypeKind::Unknown ||
                decl.returnType == Types::SadTypeKind::Void)
            {
                if (decl.isExtern)
                {
                    // (AR) الدوال الخارجية بدون نوع إرجاع تُفترض رقم (I64)
                    // (EN) Extern functions without return type default to integer
                    expectedReturnType_ = registry_.getIntegerType();
                }
                else if (!decl.is_async && !decl.isGenerator && !decl.isNoReturn &&
                         Sad::NullSafety::NullSafetyAnalyzer::bodyCertainlyReturnsNothing(
                             decl.body.get()))
                {
                    // (AR) SEM045 (D8): دالةٌ جسدُها بلا `ارجع` بقيمةٍ ولا `ارمِ` ولا
                    //      `أنتج` نوعُ إرجاعها «فراغ» يقينًا — لا «رقم» الضمنيُّ أدناه.
                    //      كان الافتراضُ يجعل `نص س = لا_شيء()` يُرفَض بتشخيصٍ كاذبِ
                    //      التعليل («وُجد 'رقم'») ويجعل `رقم س = لا_شيء()` يمرُّ صامتًا
                    //      (النوعُ المختلَقُ يطابق الخانة) — قِيس كلاهما. القاعدةُ نفسُها
                    //      المشتركةُ مع محلّل أمان null، لا نسخة ثالثة.
                    // (EN) SEM045 (D8): a certainly-void body types as Void, not the
                    //      implicit-int fallback below. The fabricated «رقم» both
                    //      falsely rejected `نص س = لا_شيء()` and silently passed
                    //      `رقم س = لا_شيء()` (measured). Same shared rule as the
                    //      null-safety analyzer — no third copy.
                    expectedReturnType_ = registry_.getVoidType();
                }
                else
                {
                    expectedReturnType_ = nullptr;
                }
            }
            else
            {
                expectedReturnType_ = dataTypeToTypePtr(decl.returnType);
            }

            // (AR) تسجيل الدالة بنوع دالة كامل (معاملات + إرجاع)
            TypeList paramTypes;
            for (auto &param : decl.parameters)
            {
                paramTypes.push_back(dataTypeToTypePtr(param.type));
            }
            auto fnType = std::make_shared<FunctionType>(std::move(paramTypes), expectedReturnType_);
            declareVariable(decl.name, fnType);

            enterScope();

            // تسجيل المعاملات / Register parameters
            for (auto &param : decl.parameters)
            {
                TypePtr paramType = dataTypeToTypePtr(param.type);
                declareVariable(param.name, paramType);
            }

            // فحص جسم الدالة / Check function body
            if (decl.body)
            {
                decl.body->accept(*this);
            }

            exitScope();

            currentFunction_ = prevFunction;
            expectedReturnType_ = prevReturnType;

            if (debugMode_)
            {
                std::cerr << "  [TC] Function '" << decl.name << "' -> "
                          << (expectedReturnType_ ? expectedReturnType_->toString() : "void") << "\n";
            }
        }

        void TypeChecker::visitClassDecl(AST::ClassDecl &decl)
        {
            // (AR) سجّل اسم الصنف محلياً ليُستخدم في الاستدلال النوعي.
            // (EN) Register class name locally for type inference fallback.
            userClassNames_.insert(decl.name);

            enterScope();

            // (AR) التحقق من وجود الصنف في StructRegistry
            bool isKnown = StructRegistry::instance().isStructType(decl.name);
            declareVariable(decl.name, isKnown
                                           ? registry_.internPrimitiveType(SadTypeKind::Class)
                                           : registry_.getUnknownType());

            // (AR) SEM045 (حقول الأصناف): الشقيقُ الثاني لتصريحِ الصنف — يُدوَّن هنا
            //      أيضًا وإلّا بقيت حقولُ هذا الشكلِ خارجَ الحارس (درسُ الأشقاء).
            // (EN) SEM045 (class fields): the second class-declaration shape —
            //      record here too or its fields stay outside the guard.
            std::string prevCheckedClass = currentCheckedClassName_;
            currentCheckedClassName_ = decl.name;
            for (auto &member : decl.members)
            {
                if (auto *fieldDecl = dynamic_cast<AST::FieldDecl *>(member.get()))
                {
                    classFieldKinds_[decl.name][fieldDecl->name] = fieldDecl->type;
                }
            }
            // (AR) الحقولُ الموروثة — نظيرُ الشكل الأوّل (insert يحفظ تظليلَ الابن).
            // (EN) Inherited fields — twin of the first shape (insert keeps overrides).
            for (const auto &baseName : decl.superclasses)
            {
                auto baseIt = classFieldKinds_.find(baseName);
                if (baseIt != classFieldKinds_.end())
                {
                    classFieldKinds_[decl.name].insert(baseIt->second.begin(),
                                                       baseIt->second.end());
                }
            }

            // فحص الأعضاء / Check members
            for (auto &member : decl.members)
            {
                if (member)
                    member->accept(*this);
            }

            exitScope();
            currentCheckedClassName_ = prevCheckedClass;
        }

        void TypeChecker::visitFieldDecl(AST::FieldDecl &decl)
        {
            TypePtr fieldType = dataTypeToTypePtr(decl.type);
            declareVariable(decl.name, fieldType);

            if (decl.initializer)
            {
                TypePtr initType = inferExprType(decl.initializer.get());
                if (initType && fieldType && !areTypesCompatible(fieldType, initType))
                {
                    recordTypeError(decl.name,
                                    fieldType->toString(), initType->toString(),
                                    &decl, "Field initializer type mismatch");
                }
            }
        }

        void TypeChecker::visitMethodDecl(AST::MethodDecl &decl)
        {
            currentResult_.totalFunctions++;

            std::string prevFunction = currentFunction_;
            TypePtr prevReturnType = expectedReturnType_;

            currentFunction_ = decl.name;
            // (AR) إذا كان نوع الإرجاع غير معروف أو فراغ، لا نفحص نوع return — محور SadTypeKind (S-TS-P2)
            // (EN) If return type is Unknown or Void, skip return type checking — SadTypeKind-centric (S-TS-P2)
            if (decl.returnType == Types::SadTypeKind::Unknown ||
                decl.returnType == Types::SadTypeKind::Void)
            {
                expectedReturnType_ = nullptr;
            }
            else
            {
                expectedReturnType_ = dataTypeToTypePtr(decl.returnType);
            }

            enterScope();

            for (auto &param : decl.parameters)
            {
                declareVariable(param.name, dataTypeToTypePtr(param.type));
            }

            if (decl.body)
                decl.body->accept(*this);

            exitScope();

            currentFunction_ = prevFunction;
            expectedReturnType_ = prevReturnType;
        }

        void TypeChecker::visitPropertyDecl(AST::PropertyDecl &decl)
        {
            // PropertyDecl may not have standard fields we can check
            // Just visit children
            (void)decl;
        }

        void TypeChecker::visitConstructorDecl(AST::ConstructorDecl &decl)
        {
            currentResult_.totalFunctions++;

            enterScope();
            for (auto &param : decl.parameters)
            {
                declareVariable(param.name, dataTypeToTypePtr(param.type));
            }
            if (decl.body)
                decl.body->accept(*this);
            exitScope();
        }

        void TypeChecker::visitDestructorDecl(AST::DestructorDecl &decl)
        {
            enterScope();
            if (decl.body)
                decl.body->accept(*this);
            exitScope();
        }

        void TypeChecker::visitEnumDecl(AST::EnumDecl &decl)
        {
            // (AR) التحقق من وجود التعداد في EnumRegistry
            bool isKnown = EnumRegistry::instance().isEnumType(decl.name);
            declareVariable(decl.name, isKnown
                                           ? registry_.internPrimitiveType(SadTypeKind::Class) // استخدام Class كبديل لحين يتوفر EnumKind
                                           : registry_.getUnknownType());

            // (AR) [أ-م٢] تسجيل معاملات التعداد بحمولة (ADT) في السجلّ الدلاليّ.
            //      يُمكّن حسم هويّة نمط الباني «عندما عدد(ق)» وتعبير البناء «عدد(٥)»،
            //      وفحص عدد/أنواع الحمولة، وفحص استنفاد المطابقة. (الترتيب مصدر حقيقة
            //      لاستنفاد المطابقة، لذا نحفظ المعاملات بترتيب التصريح.)
            // (EN) [A-M2] Register tagged-enum (ADT) variants into the semantic registry.
            //      Enables constructor-pattern «عندما عدد(ق)» and variant-expr «عدد(5)»
            //      identity resolution, payload arity/type checks, and match exhaustiveness.
            auto &variantList = enumVariants_[decl.name];
            for (auto &member : decl.members)
            {
                EnumVariantInfo info;
                info.variantName = member.name;
                info.fieldTypes = member.fieldTypes; // (AR) نسخة موازية للحمولة / (EN) parallel payload copy
                variantList.push_back(std::move(info));
                variantOwners_[member.name].push_back(decl.name);
            }

            for (auto &member : decl.members)
            {
                if (member.value)
                {
                    inferExprType(member.value.get());
                }
            }
        }

        void TypeChecker::visitImportStmt(AST::ImportStmt &stmt)
        {
            // (AR) تحميل أنواع الوحدة — تسجيل اسم الوحدة كمتغير من نوع Module
            // (EN) Module type loading — register module name as Module-typed variable
            if (!stmt.modulePath.empty())
            {
                std::string effectiveName = stmt.getEffectiveName();

                // (AR) تسجيل الوحدة كنطاق اسم — كل أعضائها Any حتى يتم ربط الوحدات
                // (EN) Register module as namespace — members are Any until module linking
                declareVariable(effectiveName, registry_.getAnyType());

                // (AR) تسجيل مسار الوحدة لمرحلة الربط في SIR
                // (EN) Record module path for SIR linking phase
                currentResult_.moduleDependencies.push_back(stmt.getFullModuleName());
            }
        }

        void TypeChecker::visitFromImportStmt(AST::FromImportStmt &stmt)
        {
            // (AR) تسجيل كل رمز مستورد مع نوعه المبدئي
            // (EN) Register each imported symbol with its initial type

            // (AR) تسجيل اعتماد الوحدة
            currentResult_.moduleDependencies.push_back(stmt.getFullModuleName());

            for (const auto &item : stmt.items)
            {
                std::string name = item.getEffectiveName();
                // (AR) كل رمز مستورد يبدأ كـ Any — سيُحدَّث عند ربط الوحدات
                declareVariable(name, registry_.getAnyType());
            }
        }

        void TypeChecker::visitExportStmt(AST::ExportStmt &stmt)
        {
            if (stmt.declaration)
                stmt.declaration->accept(*this);
        }

        void TypeChecker::visitExportDecl(AST::ExportDecl &decl)
        {
            (void)decl;
        }

        void TypeChecker::visitReExportStmt(AST::ReExportStmt &stmt)
        {
            (void)stmt;
        }

        // ============================================================================
        // المعممات / Templates & Generics
        // ============================================================================

        void TypeChecker::visitTemplateFunctionDecl(AST::TemplateFunctionDecl &decl)
        {
            currentResult_.totalFunctions++;

            // (AR) [Phase 8] حفظ سياق الإرجاع وتهيئة نوع الإرجاع المتوقع
            //      للسماح بفحص جمل ارجع داخل جسم القالب.
            // (EN) [Phase 8] Save return context and set expected return type
            //      to enable checking of return statements inside template body.
            std::string prevFunction = currentFunction_;
            TypePtr prevReturnType = expectedReturnType_;
            currentFunction_ = decl.name;
            // (AR) محور SadTypeKind (S-TS-P2): Unknown/Void/Class → لا فحص للإرجاع
            // (EN) SadTypeKind-centric (S-TS-P2): Unknown/Void/Class → skip return checking
            if (decl.returnType == Types::SadTypeKind::Unknown ||
                decl.returnType == Types::SadTypeKind::Void ||
                decl.returnType == Types::SadTypeKind::Class)
            {
                // (AR) نوع غير معروف أو معامل نوع T → لا فحص للإرجاع
                // (EN) Unknown type or type param T → skip return checking
                expectedReturnType_ = nullptr;
            }
            else
            {
                // (AR) نوع إرجاع محدد (رقم/نص/عشري/منطقي) → فعّل الفحص
                // (EN) Concrete return type (int/str/float/bool) → enable check
                expectedReturnType_ = dataTypeToTypePtr(decl.returnType);
            }

            // (AR) [Phase 5] تسجيل القالب مع معاملاته (تشمل القيود) و whereClause
            //      لاسترجاعها لاحقاً عند instantiation وفرض القيود.
            // (EN) [Phase 5] Register template with its parameters (incl. constraints)
            //      and whereClause for later constraint enforcement at instantiation.
            {
                TemplateInfo info;
                info.typeParameters.reserve(decl.typeParameters.size());
                for (const auto &p : decl.typeParameters)
                {
                    ParamInfo pi;
                    pi.name = p.name;
                    pi.isConst = p.isConst;
                    pi.constraints = p.constraints;
                    info.typeParameters.push_back(std::move(pi));
                }
                info.whereItems.reserve(decl.whereClause.items.size());
                for (const auto &it : decl.whereClause.items)
                {
                    WhereItemInfo wi;
                    wi.typeName = it.typeName;
                    wi.constraints = it.constraints;
                    info.whereItems.push_back(std::move(wi));
                }
                // (AR) [Phase 5b] احفظ أسماء أنواع المعاملات بحرفيتها كما
                //      كُتبت — للاستدلال عند الاستدعاء (مثلاً "T" أو "نقطة").
                // (EN) [Phase 5b] Save parameter type names verbatim — used
                //      for inference at call sites (e.g., "T" or "نقطة").
                info.paramTypeNames.reserve(decl.parameters.size());
                for (const auto &p : decl.parameters)
                {
                    info.paramTypeNames.push_back(p.typeName);
                }
                info.isClass = false;
                userTemplates_[decl.name] = std::move(info);
            }

            enterScope();

            // (AR) تسجيل معاملات النوع كمتغيرات نوع معممة
            for (auto &tp : decl.typeParameters)
            {
                declareVariable(tp.name, registry_.getUnknownType());
            }

            // تسجيل معاملات الدالة / Register function parameters
            for (auto &param : decl.parameters)
            {
                declareVariable(param.name, dataTypeToTypePtr(param.type));
            }

            if (decl.body)
                decl.body->accept(*this);

            exitScope();

            // تسجيل الدالة المعممة / Register generic function
            // (AR) SEM045 (انحدار t001 وثقب المراجعة العدائية): الخانةُ الخامُ تكون Void
            //      لغيابِ التصريحِ كما لتصريحِ «فراغ» سواءً، وتسجيلُها كما هي جعل نداءَ
            //      قالبٍ يُرجع قيمةً («هوية(5)») يُختم فراغًا فتطلق قاعدةُ D8 الحسابيةُ
            //      كذبًا — وتحويلُ الفرعِ المستهلِكِ Void→Unknown جملةً أضاع D8 عن
            //      القالبِ الفراغيِّ حقًّا (قِيس: خطأٌ صامتٌ في --إنتاج). الحقيقةُ تُدوَّن
            //      في المنبع: «الفراغُ اليقينيُّ» (bodyCertainlyReturnsNothing المشتركة)
            //      يبقى Void فيصيبه D8، وما يُرجع قيمةً بلا تصريحٍ يُدوَّن مجهولًا.
            // (EN) SEM045 (regression t001 + adversarial-review hole): the raw slot is
            //      Void both for an omitted return type and an explicit «فراغ»; recording
            //      it verbatim stamped value-returning template calls Void (false D8),
            //      while converting Void→Unknown at the consumer lost D8 for genuinely
            //      void templates (measured: silent wrong result under --إنتاج). Record
            //      the truth at the source: a certainly-void body (shared
            //      bodyCertainlyReturnsNothing) stays Void so D8 still fires; an
            //      undeclared value-returning body registers as Unknown.
            if (decl.returnType == Types::SadTypeKind::Unknown ||
                decl.returnType == Types::SadTypeKind::Void)
            {
                declareVariable(decl.name,
                                Sad::NullSafety::NullSafetyAnalyzer::bodyCertainlyReturnsNothing(
                                    decl.body.get())
                                    ? registry_.getVoidType()
                                    : registry_.getUnknownType());
            }
            else
            {
                declareVariable(decl.name, dataTypeToTypePtr(decl.returnType));
            }

            // (AR) [Phase 8] استعادة سياق الإرجاع السابق
            // (EN) [Phase 8] Restore previous return context
            currentFunction_ = prevFunction;
            expectedReturnType_ = prevReturnType;

            if (debugMode_)
            {
                std::cerr << "  [TC] TemplateFunction '" << decl.name << "' with "
                          << decl.typeParameters.size() << " type params\n";
            }
        }

        void TypeChecker::visitTemplateClassDecl(AST::TemplateClassDecl &decl)
        {
            // (AR) [Phase 5] تسجيل القالب الصنفي
            // (EN) [Phase 5] Register template class
            {
                TemplateInfo info;
                info.typeParameters.reserve(decl.typeParameters.size());
                for (const auto &p : decl.typeParameters)
                {
                    ParamInfo pi;
                    pi.name = p.name;
                    pi.isConst = p.isConst;
                    pi.constraints = p.constraints;
                    info.typeParameters.push_back(std::move(pi));
                }
                info.whereItems.reserve(decl.whereClause.items.size());
                for (const auto &it : decl.whereClause.items)
                {
                    WhereItemInfo wi;
                    wi.typeName = it.typeName;
                    wi.constraints = it.constraints;
                    info.whereItems.push_back(std::move(wi));
                }
                info.isClass = true;
                userTemplates_[decl.name] = std::move(info);
            }

            enterScope();

            for (auto &tp : decl.typeParameters)
            {
                declareVariable(tp.name, registry_.getUnknownType());
            }

            for (auto &member : decl.members)
            {
                if (member)
                    member->accept(*this);
            }

            exitScope();

            // (AR) تسجيل الصنف المعمم — البحث في StructRegistry
            bool isKnown = StructRegistry::instance().isStructType(decl.name);
            declareVariable(decl.name, isKnown
                                           ? registry_.internPrimitiveType(SadTypeKind::Class)
                                           : registry_.getUnknownType());
        }

        void TypeChecker::visitTemplateInstantiation(AST::TemplateInstantiation &inst)
        {
            currentResult_.totalExpressions++;

            // (AR) البحث عن القالب والتحقق من عدد معاملات الأنواع
            // (EN) Look up template and validate type argument count
            auto type = lookupVariable(inst.templateName);

            if (!type)
            {
                // (AR) قالب غير معرّف
                // (EN) Template not defined
                TypeCheckError err;
                err.variableName = inst.templateName;
                err.line = inst.position.line;
                err.column = inst.position.column;
                err.message = "Undefined template: '" + inst.templateName + "'";
                err.arabicMessage = "قالب غير معرّف: '" + inst.templateName + "'";
                currentResult_.addError(err);
                lastInferredType_ = registry_.getUnknownType();
            }
            else
            {
                // (AR) التحقق من أنه ليس فارغًا من معاملات الأنواع والثوابت كليهما
                // (EN) Ensure at least one of type/const arguments is provided
                if (inst.typeArguments.empty() && inst.constArguments.empty())
                {
                    TypeCheckError err;
                    err.variableName = inst.templateName;
                    err.line = inst.position.line;
                    err.column = inst.position.column;
                    err.message = "Empty type/const arguments for template: '" + inst.templateName + "'";
                    err.arabicMessage = "معاملات أنواع/ثوابت فارغة للقالب: '" + inst.templateName + "'";
                    currentResult_.addError(err);
                }
                lastInferredType_ = type;
            }

            // ==================================================================
            // (AR) [Phase 5 — Template Constraints] فرض القيود على الوسائط
            //      لكل معامل نوعي في تعريف القالب، نطابقه مع وسيطة
            //      typeArgumentNames في موقعها، ونفحص كل قيد ضد typeImpls_.
            // (EN) [Phase 5 — Template Constraints] Enforce constraints on args
            //      For each type parameter in template decl, match it to the
            //      typeArgumentName at its position, and check each constraint
            //      against typeImpls_.
            // ==================================================================
            auto tmplIt = userTemplates_.find(inst.templateName);
            if (tmplIt != userTemplates_.end())
            {
                const auto &tmpl = tmplIt->second;

                // (AR) نجمع المعاملات النوعية فقط (نتجاهل isConst للقيود).
                // (EN) Collect type-only parameters (skip const-generic params).
                std::vector<const ParamInfo *> typeParams;
                for (const auto &p : tmpl.typeParameters)
                {
                    if (!p.isConst)
                        typeParams.push_back(&p);
                }

                // (AR) فحص أن العدد متطابق (إن وُجدت أسماء وسائط).
                // (EN) Check arity (when argument names provided).
                if (!inst.typeArgumentNames.empty() &&
                    inst.typeArgumentNames.size() != typeParams.size())
                {
                    TypeCheckError err;
                    err.variableName = inst.templateName;
                    err.line = inst.position.line;
                    err.column = inst.position.column;
                    err.message = "Template '" + inst.templateName + "' expects " +
                                  std::to_string(typeParams.size()) + " type arg(s), got " +
                                  std::to_string(inst.typeArgumentNames.size());
                    err.arabicMessage = "القالب '" + inst.templateName + "' يتوقع " +
                                        std::to_string(typeParams.size()) +
                                        " وسائط نوعية، تلقّى " +
                                        std::to_string(inst.typeArgumentNames.size());
                    currentResult_.addError(err);
                }

                // (AR) فرض قيود كل معامل ضد وسيطته.
                // (EN) Enforce each parameter's constraints against its argument.
                size_t n = std::min(typeParams.size(), inst.typeArgumentNames.size());
                for (size_t i = 0; i < n; ++i)
                {
                    enforceParamConstraints(*typeParams[i],
                                            inst.typeArgumentNames[i],
                                            inst.position);
                }

                // (AR) فرض جملة حيث (where) — قيود إضافية بأسماء معاملات النوع.
                // (EN) Enforce where clause — extra constraints by type-param name.
                for (const auto &item : tmpl.whereItems)
                {
                    int paramIdx = -1;
                    for (size_t k = 0; k < typeParams.size(); ++k)
                    {
                        if (typeParams[k]->name == item.typeName)
                        {
                            paramIdx = static_cast<int>(k);
                            break;
                        }
                    }
                    if (paramIdx < 0 || paramIdx >= static_cast<int>(inst.typeArgumentNames.size()))
                        continue;
                    const std::string &argName = inst.typeArgumentNames[paramIdx];
                    ParamInfo synthetic;
                    synthetic.name = item.typeName;
                    synthetic.isConst = false;
                    synthetic.constraints = item.constraints;
                    enforceParamConstraints(synthetic, argName, inst.position);
                }
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] TemplateInstantiation '" << inst.templateName
                          << "' with " << inst.typeArguments.size() << " type args\n";
            }
        }

        // ====================================================================
        // (AR) [Phase 5 — Template Constraints] زوار السمات والتنفيذات
        // (EN) [Phase 5 — Template Constraints] Trait/Impl visitors
        // ====================================================================
        void TypeChecker::visitTraitDecl(AST::TraitDecl &decl)
        {
            // (AR) سجّل السمة + سماتها الأم في userTraits_.
            //      كذلك نسجلها في TraitRegistry العالمي ليتعرف عليها بقية النظام.
            // (EN) Register trait + its super traits in userTraits_. Also register
            //      it in the global TraitRegistry so the rest of the system sees it.
            userTraits_[decl.name] = decl.superTraits;

            // (AR) [Phase 5b] استخراج توقيعات الدوال (الاسم + عدد المعاملات)
            //      وتسجيلها للتحقق من اكتمال أي impl لاحقاً.
            // (EN) [Phase 5b] Extract method signatures (name + param count)
            //      and store them for later impl-completeness verification.
            std::vector<TraitMethodSig> sigs;
            sigs.reserve(decl.methods.size());
            for (const auto &m : decl.methods)
            {
                TraitMethodSig s;
                s.name = m.name;
                s.paramCount = m.params.size();
                s.hasDefault = (m.defaultImpl != nullptr);
                // (AR) [Phase 5d] استخراج أسماء أنواع المعاملات + النوع المُرجع.
                // (EN) [Phase 5d] Extract param type names + return type.
                s.paramTypes.reserve(m.params.size());
                for (const auto &p : m.params)
                    s.paramTypes.push_back(paramTypeNameOf(p));
                // (AR) [Phase 5e] استخدم returnTypeName (اسم الصنف) إن وُجد،
                //      وإلا استخدم اسم النوع الأساسي.
                // (EN) [Phase 5e] Use returnTypeName (class name) if present,
                //      else fall back to data type name.
                s.returnType = m.returnTypeName.empty()
                                   ? dataTypeArabicName(m.returnType)
                                   : m.returnTypeName;
                sigs.push_back(std::move(s));
            }
            traitMethods_[decl.name] = std::move(sigs);

            auto &reg = TypeSystem::TraitRegistry::instance();
            auto traitObj = reg.findTrait(decl.name);
            if (!traitObj)
            {
                traitObj = std::make_shared<TypeSystem::Trait>(decl.name, decl.name);
                reg.registerTrait(traitObj);
            }
            // (AR) [Phase 5b] انشر دوال السمة داخل كائن Trait العالمي حتى
            //      تتمكن طبقات لاحقة (codegen، dispatch) من البحث عنها.
            // (EN) [Phase 5b] Publish trait methods into the global Trait so
            //      later layers (codegen, dispatch) can look them up.
            //      We use UnknownType for params/return — semantic info only.
            for (const auto &m : decl.methods)
            {
                if (traitObj->findMethod(m.name))
                    continue;
                TypeSystem::TraitMethod tm;
                tm.arabicName = m.name;
                tm.englishName = m.name;
                tm.paramTypes.reserve(m.params.size());
                for (size_t i = 0; i < m.params.size(); ++i)
                    tm.paramTypes.push_back(registry_.getUnknownType());
                tm.returnType = registry_.getUnknownType();
                tm.hasDefaultImpl = (m.defaultImpl != nullptr);
                tm.isStatic = false;
                tm.requiresSelf = true;
                tm.mutatesSelf = false;
                traitObj->addMethod(tm);
            }

            // (AR) لا نزور body — السمات تُعالَج بنيوياً فقط هنا.
            // (EN) We don't visit the body — traits are handled structurally only.
            if (debugMode_)
            {
                std::cerr << "  [TC] TraitDecl '" << decl.name << "' with "
                          << decl.superTraits.size() << " super trait(s), "
                          << decl.methods.size() << " method(s)\n";
            }
        }

        void TypeChecker::visitImplDecl(AST::ImplDecl &decl)
        {
            // (AR) سجّل أن النوع targetType ينفذ السمة traitName.
            //      إن كان traitName فارغًا فهذا inherent impl — لا قيود على القوالب.
            // (EN) Record that targetType implements traitName. If traitName is
            //      empty this is an inherent impl — no template constraint info.
            if (!decl.traitName.empty())
            {
                typeImpls_[decl.targetType].insert(decl.traitName);

                // (AR) أضف ضمنياً السمات الأم — لأن A: B تعني تنفيذ B يتطلب A.
                //      هنا نتعامل مع الاتجاه: تنفيذ السمة يضمّن سماتها الأم.
                // (EN) Implicitly add super traits — implementing a trait implies
                //      its super traits are also satisfied (transitively).
                std::vector<std::string> stack;
                stack.push_back(decl.traitName);
                std::unordered_set<std::string> seen{decl.traitName};
                while (!stack.empty())
                {
                    std::string cur = stack.back();
                    stack.pop_back();
                    auto it = userTraits_.find(cur);
                    if (it == userTraits_.end())
                        continue;
                    for (const auto &sup : it->second)
                    {
                        if (seen.insert(sup).second)
                        {
                            typeImpls_[decl.targetType].insert(sup);
                            stack.push_back(sup);
                        }
                    }
                }

                // ============================================================
                // (AR) [Phase 5b] استخراج دوال التنفيذ والتحقق من اكتمالها
                //      مقابل دوال السمة المطلوبة (بما فيها السمات الأم).
                // (EN) [Phase 5b] Extract impl methods and verify completeness
                //      against trait's required methods (incl. super traits).
                // ============================================================
                std::unordered_map<std::string, TraitMethodSig> provided;
                for (const auto &m : decl.methods)
                {
                    if (!m)
                        continue;
                    auto buildSig = [this](const std::string &name,
                                           const std::vector<AST::Parameter> &params,
                                           Types::SadTypeKind ret,
                                           const std::string &retTypeName = "")
                    {
                        TraitMethodSig s;
                        s.name = name;
                        s.paramCount = params.size();
                        s.hasDefault = false;
                        s.paramTypes.reserve(params.size());
                        for (const auto &p : params)
                            s.paramTypes.push_back(paramTypeNameOf(p));
                        // (AR) [Phase 5e] استخدم retTypeName (اسم الصنف) إن وُجد
                        // (EN) [Phase 5e] Use retTypeName (class name) if present
                        s.returnType = retTypeName.empty() ? dataTypeArabicName(ret) : retTypeName;
                        return s;
                    };
                    if (auto *md = dynamic_cast<AST::MethodDecl *>(m.get()))
                    {
                        provided[md->name] = buildSig(md->name, md->parameters, md->returnType, md->returnTypeName);
                    }
                    else if (auto *fd = dynamic_cast<AST::FunctionDecl *>(m.get()))
                    {
                        provided[fd->name] = buildSig(fd->name, fd->parameters, fd->returnType, fd->returnTypeName);
                    }
                }
                verifyImplCompleteness(decl.traitName, decl.targetType,
                                       provided, decl.position);
            }

            // (AR) زر دوال التنفيذ لفحص أنواعها.
            // (EN) Visit implementation methods for type checking.
            for (auto &m : decl.methods)
            {
                if (m)
                    m->accept(*this);
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] ImplDecl '" << decl.traitName << "' for '"
                          << decl.targetType << "'\n";
            }
        }

        // ====================================================================
        // (AR) دوال مساعدة لفرض القيود / Helper functions for enforcement
        // ====================================================================
        bool TypeChecker::isKnownTrait(const std::string &traitName) const
        {
            if (userTraits_.count(traitName))
                return true;
            return TypeSystem::TraitRegistry::instance().findTrait(traitName) != nullptr;
        }

        bool TypeChecker::typeSatisfiesTrait(const std::string &typeName,
                                             const std::string &traitName) const
        {
            // (AR) فحص مباشر: هل سُجِّل impl صريح؟
            // (EN) Direct check: explicit impl registered?
            auto it = typeImpls_.find(typeName);
            if (it != typeImpls_.end() && it->second.count(traitName))
                return true;

            // (AR) قواعد ضمنية للأنواع البدائية:
            //      - الأنواع العددية (رقم/عشري) تحقق قابل_للمقارنة + قابل_للترتيب
            //        + قابل_للتجزئة + قابل_للنسخ + قابل_للاستنساخ + قابل_للعرض + قابل_للتنقيح
            //      - النص يحقق قابل_للمقارنة + قابل_للتجزئة + قابل_للعرض + قابل_للاستنساخ + قابل_للتنقيح
            //      - منطقي يحقق كل ما يحققه رقم
            // (EN) Implicit rules for primitive types — numbers/strings auto-satisfy
            //      the standard comparison/hash/display traits.
            static const std::unordered_set<std::string> numericPrims{
                "رقم", "عشري", "منطقي", "int", "float", "bool", "double"};
            static const std::unordered_set<std::string> stringPrims{
                "نص", "string"};
            static const std::unordered_set<std::string> numericTraits{
                "قابل_للمقارنة", "قابل_للترتيب", "قابل_للتجزئة",
                "قابل_للنسخ", "قابل_للاستنساخ", "قابل_للعرض", "قابل_للتنقيح",
                "Eq", "Ord", "PartialEq", "PartialOrd", "Hash",
                "Copy", "Clone", "Display", "Debug"};
            static const std::unordered_set<std::string> stringTraits{
                "قابل_للمقارنة", "قابل_للتجزئة", "قابل_للعرض",
                "قابل_للاستنساخ", "قابل_للتنقيح",
                "Eq", "PartialEq", "Hash", "Display", "Clone", "Debug"};

            if (numericPrims.count(typeName) && numericTraits.count(traitName))
                return true;
            if (stringPrims.count(typeName) && stringTraits.count(traitName))
                return true;

            return false;
        }

        int TypeChecker::enforceParamConstraints(const ParamInfo &param,
                                                 const std::string &typeArgName,
                                                 const Lexer::Position &pos)
        {
            // (AR) لا قيود؟ لا فحص.
            // (EN) No constraints? skip.
            if (param.constraints.empty())
                return 0;

            int errCount = 0;
            for (const auto &c : param.constraints)
            {
                if (c.empty())
                    continue;
                // (AR) تحذير (لا خطأ) إذا كانت السمة غير معروفة — قد تكون من وحدة لم تُحلّل.
                // (EN) Warning (not error) if trait unknown — may come from un-analyzed module.
                if (!isKnownTrait(c))
                {
                    // (AR) سمة غير معروفة — تحذير لا خطأ.
                    // (EN) Unknown trait — warning, not error.
                    currentResult_.addWarning(
                        "سمة غير معروفة '" + c + "' في قيد على معامل القالب '" +
                        param.name + "' (سطر " + std::to_string(pos.line) + ")");
                    continue;
                }
                if (!typeSatisfiesTrait(typeArgName, c))
                {
                    TypeCheckError err;
                    err.variableName = param.name;
                    err.line = pos.line;
                    err.column = pos.column;
                    err.message = "Type '" + typeArgName + "' does not satisfy constraint '" +
                                  c + "' on template parameter '" + param.name + "'";
                    err.arabicMessage = "النوع '" + typeArgName + "' لا يحقق القيد '" +
                                        c + "' على معامل القالب '" + param.name + "'";
                    currentResult_.addError(err);
                    ++errCount;
                }
            }
            return errCount;
        }

        void TypeChecker::visitNamespaceDecl(AST::NamespaceDecl &decl)
        {
            enterScope();
            for (auto &member : decl.members)
            {
                if (member)
                    member->accept(*this);
            }
            exitScope();
        }

        void TypeChecker::visitOperatorDecl(AST::OperatorDecl &decl)
        {
            currentResult_.totalFunctions++;

            enterScope();
            for (auto &param : decl.parameters)
            {
                declareVariable(param.name, dataTypeToTypePtr(param.type));
            }
            if (decl.body)
                decl.body->accept(*this);
            exitScope();
        }

        // ====================================================================
        // (AR) [Phase 5b] دوال مساعدة لتحقق التنفيذات + الاستدلال النوعي
        // (EN) [Phase 5b] Helpers for impl verification + type inference
        // ====================================================================

        void TypeChecker::verifyImplCompleteness(
            const std::string &traitName,
            const std::string &targetType,
            const std::unordered_map<std::string, TraitMethodSig> &providedMethods,
            const Lexer::Position &pos)
        {
            // (AR) اجمع جميع الدوال المطلوبة من السمة + سماتها الأم بشكل
            //      متعدٍّ. الدوال ذات التنفيذ الافتراضي اختيارية.
            // (EN) Collect all required methods from trait + super traits
            //      transitively. Methods with default impl are optional.
            std::unordered_map<std::string, TraitMethodSig> required;
            std::vector<std::string> stack{traitName};
            std::unordered_set<std::string> seen{traitName};
            while (!stack.empty())
            {
                std::string cur = stack.back();
                stack.pop_back();
                auto mit = traitMethods_.find(cur);
                if (mit != traitMethods_.end())
                {
                    for (const auto &sig : mit->second)
                    {
                        // الأقدم يفوز — لا نستبدل توقيعاً موجوداً
                        if (!required.count(sig.name))
                            required[sig.name] = sig;
                    }
                }
                auto sit = userTraits_.find(cur);
                if (sit != userTraits_.end())
                {
                    for (const auto &sup : sit->second)
                    {
                        if (seen.insert(sup).second)
                            stack.push_back(sup);
                    }
                }
            }

            // (AR) إن لم نعرف عن السمة شيئاً (مثلاً سمة مستوردة)، تخطّ.
            // (EN) If we know nothing about the trait, skip silently.
            if (required.empty() && !traitMethods_.count(traitName))
                return;

            // (AR) لكل دالة مطلوبة بدون تنفيذ افتراضي:
            //      تحقق من وجودها وتطابق عدد المعاملات (تساهلاً نسمح
            //      بفرق ±1 لاحتساب this/self).
            // (EN) For each required method without default impl:
            //      verify presence and param-count match (tolerating ±1
            //      for self/this).
            for (const auto &kv : required)
            {
                const TraitMethodSig &sig = kv.second;
                if (sig.hasDefault)
                    continue;
                auto it = providedMethods.find(sig.name);
                if (it == providedMethods.end())
                {
                    TypeCheckError err;
                    err.variableName = sig.name;
                    err.line = pos.line;
                    err.column = pos.column;
                    err.message = "[Trait='" + traitName + "'][Method='" + sig.name +
                                  "'] Impl for type '" + targetType +
                                  "' is missing required method";
                    err.arabicMessage = "[السمة='" + traitName + "'][الدالة='" + sig.name +
                                        "'] تنفيذ النوع '" + targetType +
                                        "' يفتقد للدالة المطلوبة";
                    currentResult_.addError(err);
                    continue;
                }
                size_t provided = it->second.paramCount;
                // (AR) قبول تطابق دقيق أو فرق 1 (this/self).
                // (EN) Accept exact match or ±1 (self/this).
                if (provided != sig.paramCount &&
                    provided + 1 != sig.paramCount &&
                    sig.paramCount + 1 != provided)
                {
                    currentResult_.addWarning(
                        "[السمة='" + traitName + "'][الدالة='" + sig.name +
                        "'] اختلاف عدد المعاملات على النوع '" + targetType +
                        "': متوقع " + std::to_string(sig.paramCount) +
                        " ، المُنفَّذ " + std::to_string(provided));
                    continue;
                }

                // ============================================================
                // (AR) [Phase 5d] مقارنة أنواع المعاملات والقيمة المُرجعة.
                //      نُحاذي القوائم مع احتساب الفرق المحتمل لمعامل
                //      this/self (الذي قد يظهر أو لا يظهر صراحةً).
                // (EN) [Phase 5d] Compare param types + return type. Align
                //      lists accounting for the possible self/this offset.
                // ============================================================
                const auto &expectedTypes = sig.paramTypes;
                const auto &actualTypes = it->second.paramTypes;
                size_t expSkip = 0, actSkip = 0;
                if (sig.paramCount > provided)
                    expSkip = sig.paramCount - provided; // غالباً 1 (self في السمة)
                else if (provided > sig.paramCount)
                    actSkip = provided - sig.paramCount; // غالباً 1 (this في التنفيذ)

                size_t toCompare = std::min(expectedTypes.size() - expSkip,
                                            actualTypes.size() - actSkip);
                for (size_t i = 0; i < toCompare; ++i)
                {
                    const std::string &exp = expectedTypes[expSkip + i];
                    const std::string &act = actualTypes[actSkip + i];
                    // (AR) إن كان أحدهما فارغاً (نوع غير معروف) لا نشكو.
                    // (EN) If either side is empty (unknown), skip.
                    if (exp.empty() || act.empty())
                        continue;
                    if (exp != act)
                    {
                        TypeCheckError err;
                        err.variableName = sig.name;
                        err.line = pos.line;
                        err.column = pos.column;
                        err.message = "Type mismatch in impl method '" + sig.name +
                                      "' parameter " + std::to_string(i + 1) +
                                      ": expected '" + exp + "', got '" + act + "'";
                        err.arabicMessage = "[السمة='" + traitName + "'][الدالة='" +
                                            sig.name + "'] اختلاف نوع المعامل " +
                                            std::to_string(i + 1) +
                                            " على النوع '" + targetType +
                                            "': المتوقع '" + exp +
                                            "'، المُنفَّذ '" + act + "'";
                        currentResult_.addError(err);
                    }
                }

                // (AR) مقارنة نوع الإرجاع (إن كان معروفاً للجهتين).
                // (EN) Compare return type (if known on both sides).
                if (!sig.returnType.empty() && !it->second.returnType.empty() &&
                    sig.returnType != it->second.returnType)
                {
                    TypeCheckError err;
                    err.variableName = sig.name;
                    err.line = pos.line;
                    err.column = pos.column;
                    err.message = "Return type mismatch in impl method '" + sig.name +
                                  "': expected '" + sig.returnType + "', got '" +
                                  it->second.returnType + "'";
                    err.arabicMessage = "[السمة='" + traitName + "'][الدالة='" +
                                        sig.name + "'] اختلاف نوع القيمة المُرجعة على النوع '" +
                                        targetType + "': المتوقع '" +
                                        sig.returnType + "'، المُنفَّذ '" +
                                        it->second.returnType + "'";
                    currentResult_.addError(err);
                }
            }
        }

        // ====================================================================
        // (AR) [Phase 5d] مساعدات استخراج أسماء الأنواع
        // (EN) [Phase 5d] Type-name extraction helpers
        // ====================================================================

        std::string TypeChecker::sadKindArabicName(Types::SadTypeKind t) const
        {
            // (AR) [S-TS-P2] المحور SadTypeKind. السلاسل مطابقة للسابق تمامًا
            //      (لا تغيير في رسائل الأخطاء/التشخيص). Class تُرجِع "" لأن اسم الصنف
            //      يأتي من typeName، وVoid/Null تُرجِع "لاشيء" (التمييز الدلالي في S-TS-P9).
            using K = Types::SadTypeKind;
            switch (t)
            {
            case K::Integer:
                return "رقم";
            case K::Float:
                return "عشري";
            case K::String:
                return "نص";
            case K::Boolean:
                return "منطقي";
            case K::Byte:
                return "بايت";
            case K::Array:
                return "مصفوفة";
            case K::Map:
                return "خريطة";
            case K::Tuple:
                return "ثنائي";
            case K::Function:
                return "دالة";
            case K::Enum:
                return "تعداد";
            case K::Void:
            case K::Null:
                return "لاشيء";
            case K::Class:
                return ""; // اسم الصنف يأتي من typeName
            case K::Unknown:
                return "";
            case K::Error:
                return "خطأ";
            default:
                return "";
            }
        }

        // (AR) جسر حدود الـAST (S-TS-P2): DataType→SadTypeKind→اسم عربي. يُحذف في S-TS-P2.5a.
        // (EN) AST-boundary bridge (S-TS-P2): DataType→SadTypeKind→Arabic name. Removed in S-TS-P2.5a.
        std::string TypeChecker::dataTypeArabicName(Types::SadTypeKind t) const
        {
            return sadKindArabicName(t);
        }

        std::string TypeChecker::paramTypeNameOf(const AST::Parameter &p) const
        {
            // (AR) اسم الصنف صريح في typeName له الأولوية.
            // (EN) Explicit class name in typeName takes priority.
            if (!p.typeName.empty())
                return p.typeName;
            // (AR) قراءة AST (DataType) مُجسَّرة إلى SadTypeKind — S-TS-P2 (تُزال في P2.5a)
            return sadKindArabicName(p.type);
        }

        std::string TypeChecker::extractTypeNameFromExpr(AST::Expression *expr) const
        {
            if (!expr)
                return std::string();

            // (AR) حرفية رقمية → "رقم".
            // (EN) Numeric literal → "رقم".
            if (auto *lit = dynamic_cast<AST::LiteralExpr *>(expr))
            {
                using TT = Lexer::TokenType;
                switch (lit->token.getType())
                {
                case TT::NUMBER_INTEGER:
                    return "رقم";
                case TT::NUMBER_DOUBLE:
                    return "عشري";
                case TT::STRING_LITERAL:
                case TT::STRING_RAW:
                case TT::STRING_FSTRING:
                    return "نص";
                case TT::LITERAL_TRUE:
                case TT::LITERAL_FALSE:
                    return "منطقي";
                default:
                    return std::string();
                }
            }

            // (AR) مُعرّف → ابحث عن نوعه في النطاق.
            // (EN) Identifier → look up its type in scope.
            if (auto *var = dynamic_cast<AST::VariableExpr *>(expr))
            {
                auto t = lookupVariable(var->name);
                if (t)
                {
                    // أنواع بدائية → نُرجع اسماً عربياً.
                    if (t->getKind() == TypeSystem::SadTypeKind::Integer)
                        return "رقم";
                    if (t->getKind() == TypeSystem::SadTypeKind::Float)
                        return "عشري";
                    if (t->getKind() == TypeSystem::SadTypeKind::String)
                        return "نص";
                    if (t->getKind() == TypeSystem::SadTypeKind::Boolean)
                        return "منطقي";
                    // ====================================================
                    // (AR) [Phase 5c] صنف مُسجَّل: اقرأ اسمه من
                    //      variableClassNames_ (أحدث نطاق يُغطّيه).
                    // (EN) [Phase 5c] Registered class: read its name
                    //      from variableClassNames_ (innermost scope).
                    // ====================================================
                    if (t->getKind() == TypeSystem::SadTypeKind::Class)
                    {
                        for (auto it = variableClassNames_.rbegin();
                             it != variableClassNames_.rend(); ++it)
                        {
                            auto found = it->find(var->name);
                            if (found != it->end())
                                return found->second;
                        }
                    }
                }
                // (AR) [Phase 5c] فحص احتياطي: حتى لو فشل lookupVariable
                //      (مثلاً معامل دالة بدون نوع مُصرَّح)، جرّب السجل.
                // (EN) [Phase 5c] Fallback: even if lookupVariable
                //      fails (e.g. typeless function param), try the
                //      class-name registry.
                for (auto it = variableClassNames_.rbegin();
                     it != variableClassNames_.rend(); ++it)
                {
                    auto found = it->find(var->name);
                    if (found != it->end())
                        return found->second;
                }
            }

            // (AR) إنشاء جديد (NewExpr) → اسم الصنف.
            // (EN) New expression → class name.
            if (auto *ne = dynamic_cast<AST::NewExpr *>(expr))
            {
                return ne->className;
            }

            // (AR) استدعاء يشبه الباني: اسم صنف(وسائط) → اسم الصنف.
            // (EN) Constructor-like call: ClassName(args) → class name.
            if (auto *call = dynamic_cast<AST::CallExpr *>(expr))
            {
                if (auto *ve = dynamic_cast<AST::VariableExpr *>(call->callee.get()))
                {
                    if (StructRegistry::instance().isStructType(ve->name) ||
                        userClassNames_.count(ve->name))
                    {
                        return ve->name;
                    }
                }
            }

            return std::string();
        }

        bool TypeChecker::inferTemplateTypeArguments(
            const TemplateInfo &tmpl,
            const std::vector<std::string> &argTypeNames,
            std::vector<std::string> &inferred) const
        {
            inferred.clear();

            // (AR) اجمع المعاملات النوعية فقط (تجاهل isConst).
            // (EN) Collect type-only parameters (skip const-generic).
            std::vector<const ParamInfo *> typeParams;
            for (const auto &p : tmpl.typeParameters)
            {
                if (!p.isConst)
                    typeParams.push_back(&p);
            }

            if (typeParams.empty())
                return false;

            // (AR) لكل معامل نوعي، ابحث في paramTypeNames عن أول موضع
            //      يستخدم اسمه، واستنتج النوع من نوع وسيطة الاستدعاء عند
            //      ذلك الموضع. اعتبر الاستدلال غامضاً إن وُجد موضعان
            //      بنوعين مختلفين.
            // (EN) For each type parameter, find the first slot in
            //      paramTypeNames using its name, and infer the type from
            //      the call argument at that slot. Consider inference
            //      ambiguous if two slots disagree.
            for (const auto *tp : typeParams)
            {
                std::string chosen;
                for (size_t i = 0; i < tmpl.paramTypeNames.size() &&
                                   i < argTypeNames.size();
                     ++i)
                {
                    if (tmpl.paramTypeNames[i] != tp->name)
                        continue;
                    const std::string &argT = argTypeNames[i];
                    if (argT.empty())
                        continue;
                    if (chosen.empty())
                    {
                        chosen = argT;
                    }
                    else if (chosen != argT)
                    {
                        // غموض → فشل
                        inferred.clear();
                        return false;
                    }
                }
                if (chosen.empty())
                {
                    // لم نستطع الاستدلال على هذا المعامل
                    inferred.clear();
                    return false;
                }
                inferred.push_back(chosen);
            }

            return true;
        }

    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
