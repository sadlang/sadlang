// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file null_safety_analyzer.cpp
 * @brief تنفيذ محلّل أمان null المشترك / Shared Null-Safety Analyzer impl
 * @phase NS-03 — تحليل التدفّق (smart narrowing) فوق NS-04
 *
 * (AR) NS-02 (ADR-NS-001 D6/D8):
 *      - المحلّل يأخذ **صرامة مجرّدة** (Ignore/Warn/Fatal)؛ المابِع
 *        strictnessFromOwnershipMode() يترجم سياسة الذاكرة (محور منفصل، D6).
 *      - يرصد حالة P9 **ساكنًا**: إسناد `لاشيء` (عدم) **حرفيًّا** لمتغير من نوع
 *        صريح غير اختياري. هذه هي نقطة الحقيقة الوحيدة التي يستدعيها المحرّكان (D8).
 *      - D10 (قرار الفريق): «ساكن للحرفيّ + حارس زمنيّ» — الحرفيّ هنا، والتعابير
 *        غير الحرفيّة (`= دالة_ترجع_عدم()`) يبقى لها حارس زمنيّ في المفسّر.
 *
 * (AR) NS-04 (ADR-NS-001 D11):
 *      - يهبط المحلّل في **التعابير** (لا الجُمل فقط) ويرصد الوصول الخام
 *        (`متغير.عضو` عبر MemberExpr، و`متغير.طريقة()` عبر MethodCallExpr) حين
 *        يكون الكائن متغيّرًا مُعلَنًا اختياريًّا `T؟`. الوصول الآمن `؟.`
 *        (OptionalChainExpr) **مستثنًى**. لا تضييق بعد (NS-03 لاحقًا).
 *      - D11 (قرار الفريق، 2026-06-20): NS-04 يتبع **جدول الصرامة كاملًا فورًا**
 *        (Fatal في --prod) دون انتظار تضييق NS-03.
 *      - تتبّع المتغيرات الاختيارية بمكدّس نطاقات يمنع التسرّب بين الدوال.
 *
 * (AR) NS-03 (ADR-NS-001 D1-D5): تحليل تدفّق محلّيّ بسيط (smart narrowing) فوق
 *      رصد NS-04. يضيّق `T؟→T` للمتغيّرات المحلّية بعد فحص `!= لاشيء`/`== لاشيء`
 *      فلا يُرصَد الوصول الخام داخل الحرس. سليم-متحفّظ (D5): عند الشكّ لا يُضيِّق.
 *      - D1: محلّيّات فقط (لا حقول/إغلاقات).
 *      - D2: إعادة الإسناد تُبطل التضييق (التحوّر).
 *      - D4: التضييق العكسي يشترط خروجًا حتميًّا (`إذا (س==لاشيء){ارجع}` يضيّق بعده).
 *      - D5: متحفّظ — false-negative في prod غير مقبول، فالشكّ = إبقاء التحذير.
 *
 * (EN) NS-02: abstract strictness + static P9 relocation. NS-04: flags raw access
 *      on optionals (safe `?.` excluded), D11 full strictness. NS-03: local flow
 *      narrowing (T?→T) after `!= null`/`== null` guards; sound-conservative (D5).
 */

#include "null_safety/null_safety_analyzer.h"
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"

namespace Sad
{
    namespace NullSafety
    {
        // ====================================================================
        // (AR) أداة: هل المُهيِّئ هو الحرفيّ `لاشيء` (عدم)؟ (رصد ساكن، D10)
        // (EN) Helper: is the initializer the literal `null`? (static, D10)
        // ====================================================================
        static bool isNullLiteral(const AST::Expression *expr)
        {
            if (!expr)
                return false;
            if (auto *lit = dynamic_cast<const AST::LiteralExpr *>(expr))
                return lit->token.getType() == Lexer::TokenType::LITERAL_NULL;
            return false;
        }

        // ====================================================================
        // (AR) أدوات تحليل التدفّق (NS-03) / Flow-analysis helpers (NS-03)
        // ====================================================================

        // (AR) إن كان أحد طرفَي المقارنة متغيّرًا والآخر `لاشيء` حرفيًّا، يعيد اسمه.
        // (EN) If one side is a variable and the other is literal null, return its name.
        static bool varComparedToNull(AST::Expression *l, AST::Expression *r,
                                      std::string &nameOut)
        {
            auto *vl = dynamic_cast<AST::VariableExpr *>(l);
            if (vl && isNullLiteral(r))
            {
                nameOut = vl->name;
                return true;
            }
            auto *vr = dynamic_cast<AST::VariableExpr *>(r);
            if (vr && isNullLiteral(l))
            {
                nameOut = vr->name;
                return true;
            }
            return false;
        }

        // (AR) يجمع المتغيّرات التي يُثبِت الشرطُ أنها **غير عدم** عند تحقّقه (then):
        //      `س != لاشيء`، و`أ != لاشيء وَ ب != لاشيء` (تفكيك &&). D5: متحفّظ.
        // (EN) Collects vars the condition proves non-null when TRUE (then-branch),
        //      including `&&` decomposition. Sound-conservative (D5).
        static void collectNonNullWhenTrue(AST::Expression *cond,
                                           std::set<std::string> &out)
        {
            auto *bin = dynamic_cast<AST::BinaryExpr *>(cond);
            if (!bin)
                return;
            std::string name;
            if (bin->op == Lexer::TokenType::OP_NOT_EQUAL &&
                varComparedToNull(bin->left.get(), bin->right.get(), name))
                out.insert(name);
            else if (bin->op == Lexer::TokenType::OP_AND)
            {
                // (AR) في then يتحقّق الطرفان معًا → نضيّق من كليهما.
                collectNonNullWhenTrue(bin->left.get(), out);
                collectNonNullWhenTrue(bin->right.get(), out);
            }
        }

        // (AR) يجمع المتغيّرات التي يُثبِت الشرطُ أنها **غير عدم** عند **بطلانه** (else):
        //      `س == لاشيء` (في else يكون س غير عدم). نظير عكسيّ لـcollectNonNullWhenTrue.
        // (EN) Collects vars proven non-null when the condition is FALSE (else-branch):
        //      `س == لاشيء` → in else, س is non-null.
        static void collectNonNullWhenFalse(AST::Expression *cond,
                                            std::set<std::string> &out)
        {
            auto *bin = dynamic_cast<AST::BinaryExpr *>(cond);
            if (!bin)
                return;
            std::string name;
            if (bin->op == Lexer::TokenType::OP_EQUAL &&
                varComparedToNull(bin->left.get(), bin->right.get(), name))
                out.insert(name);
        }

        // (AR) هل تنتهي الجملة حتمًا بخروج (ارجع/ارمِ/اكسر/استمر)؟ (D4)
        //      كتلة: آخر جملة تخرج. إذا: كلا الفرعين يخرجان. متحفّظ (D5): الشكّ = لا.
        // (EN) Does the statement always exit (return/raise/break/continue)? (D4)
        static bool stmtAlwaysExits(AST::Statement *stmt)
        {
            if (!stmt)
                return false;
            if (dynamic_cast<AST::ReturnStmt *>(stmt) ||
                dynamic_cast<AST::RaiseStmt *>(stmt) ||
                dynamic_cast<AST::BreakStmt *>(stmt) ||
                dynamic_cast<AST::ContinueStmt *>(stmt))
                return true;
            if (auto *blk = dynamic_cast<AST::BlockStmt *>(stmt))
                return !blk->statements.empty() &&
                       stmtAlwaysExits(blk->statements.back().get());
            if (auto *iff = dynamic_cast<AST::IfStmt *>(stmt))
                return iff->elseBranch && stmtAlwaysExits(iff->thenBranch.get()) &&
                       stmtAlwaysExits(iff->elseBranch.get());
            return false;
        }

        // ====================================================================
        // (AR) فحص تصريح متغير واحد لحالة P9 / Check one var decl for P9
        // ====================================================================
        void NullSafetyAnalyzer::checkVarDecl(AST::VarDeclStmt &decl,
                                              NullSafetyResult &result)
        {
            // (AR) نفس حُرّاس P9 الأصليّة: نوع صريح (≠ مجهول) غير اختياري/أيّ/عدم/فراغ،
            //      ومُهيِّئ = حرفيّ `لاشيء`. الحارس الخارجي يحفظ `متغير س = لاشيء`
            //      (المُستنتَج) من أي إنذار كاذب — لا انحدار.
            // (EN) Same P9 guards: explicit non-optional type + literal null initializer.
            if (!decl.sadType)
                return;
            if (!isNullLiteral(decl.initializer.get()))
                return;
            if (decl.type == Types::SadTypeKind::Unknown ||
                decl.type == Types::SadTypeKind::Optional ||
                decl.type == Types::SadTypeKind::Any ||
                decl.type == Types::SadTypeKind::Null ||
                decl.type == Types::SadTypeKind::Void)
                return;

            // (AR) صرامة Ignore (نظير --gc): لا تشخيص إطلاقًا.
            // (EN) Ignore strictness (≈ --gc): no diagnostic at all.
            if (strictness_ == Strictness::Ignore)
                return;

            NullSafetyDiagnostic d;
            d.kind = NullSafetyErrorKind::NullAssignedToNonOptional;
            d.line = decl.position.line;
            d.column = decl.position.column;
            d.symbol = decl.name;
            d.fatal = (strictness_ == Strictness::Fatal);

            // (AR) نُبقي صيغة الرسالة مطابِقةً لفرض P9 القديم (توافق المخرجات).
            // (EN) Keep message identical to the legacy P9 enforcement (output parity).
            d.messageAr =
                "تحذير: إسناد 'لاشيء' (عدم) لمتغير '" + decl.name +
                "' من نوع غير اختياري '" + decl.sadType->arabicName() +
                "'. اجعله اختياريًّا: '" + decl.sadType->arabicName() + "؟'";
            d.messageEn =
                "Assigning 'null' to non-optional variable '" + decl.name +
                "' of type '" + decl.sadType->englishName() + "'. Make it optional: 'T?'";

            result.addDiagnostic(d);
        }

        // ====================================================================
        // (AR) NS-04: رصد وصول خام على متغيّر اختياريّ / Flag raw optional access
        // ====================================================================
        void NullSafetyAnalyzer::checkOptionalAccess(AST::Expression *object,
                                                     const std::string &member,
                                                     bool isMethodCall,
                                                     const Lexer::Position &pos,
                                                     NullSafetyResult &result)
        {
            // (AR) نرصد فقط حين يكون الكائن **متغيّرًا** مُعلَنًا اختياريًّا.
            //      (سلسلة `أ.ب.ج` تُرصَد على جزئها الأساسيّ عند العَودة في object.)
            // (EN) Flag only when the object is a *variable* known to be optional.
            auto *var = dynamic_cast<AST::VariableExpr *>(object);
            if (!var || !isOptionalVar(var->name))
                return;

            // (AR) NS-03: إن كان المتغيّر مُضيَّقًا (مُثبَتًا غير عدم) عند هذه النقطة،
            //      فالوصول آمن — لا تحذير. (smart cast بعد `!= لاشيء` ونحوه.)
            // (EN) NS-03: if proven non-null at this flow point, access is safe.
            if (isNarrowed(var->name))
                return;

            // (AR) صرامة Ignore (نظير --gc): لا تشخيص.
            if (strictness_ == Strictness::Ignore)
                return;

            NullSafetyDiagnostic d;
            d.kind = NullSafetyErrorKind::UnsafeAccessOnOptional;
            // (AR) بعض العقد (MethodCallExpr) لا تحمل موقعًا — نرجع لموقع المتغيّر.
            // (EN) Some nodes (MethodCallExpr) carry no position — fall back to the var's.
            d.line = pos.line > 0 ? pos.line : var->position.line;
            d.column = pos.line > 0 ? pos.column : var->position.column;
            d.symbol = var->name;
            d.fatal = (strictness_ == Strictness::Fatal); // D11: جدول الصرامة كاملًا
            const std::string access = isMethodCall ? (member + "()") : member;
            d.messageAr =
                "تحذير: وصول غير آمن '." + access + "' على متغيّر اختياريّ '" +
                var->name + "' قد يكون 'عدم'. استعمل الوصول الآمن '" + var->name +
                "؟." + member + "' أو تحقّق أوّلًا: 'إذا (" + var->name + " != لاشيء)'.";
            d.messageEn =
                "Unsafe access '." + access + "' on optional variable '" + var->name +
                "' which may be null. Use safe access '" + var->name + "?." + member +
                "' or guard first: 'if (" + var->name + " != null)'.";
            result.addDiagnostic(d);
        }

        // ====================================================================
        // (AR) ماشي التعابير (NS-04): يهبط في كلّ تعبير لرصد الوصول الخام.
        //      سليم-متحفّظ (D5): العقد غير المعروفة تُتخطّى (رصد أقلّ لا خطأ).
        // (EN) Expression walker (NS-04): descends into every expression.
        // ====================================================================
        void NullSafetyAnalyzer::analyzeExpr(AST::Expression *expr,
                                             NullSafetyResult &result)
        {
            if (!expr)
                return;

            // ── وصول لحقل: كائن.حقل ──────────────────────────────────────────
            if (auto *me = dynamic_cast<AST::MemberExpr *>(expr))
            {
                checkOptionalAccess(me->object.get(), me->member, /*method*/ false,
                                    me->position, result);
                analyzeExpr(me->object.get(), result);
                return;
            }
            // (AR) مسار OOP البديل (MemberAccessExpr.memberName) — للاكتمال.
            if (auto *ma = dynamic_cast<AST::MemberAccessExpr *>(expr))
            {
                checkOptionalAccess(ma->object.get(), ma->memberName, /*method*/ false,
                                    ma->position, result);
                analyzeExpr(ma->object.get(), result);
                return;
            }
            // ── استدعاء طريقة: كائن.طريقة(وسائط) ─────────────────────────────
            if (auto *mc = dynamic_cast<AST::MethodCallExpr *>(expr))
            {
                checkOptionalAccess(mc->object.get(), mc->methodName, /*method*/ true,
                                    mc->position, result);
                analyzeExpr(mc->object.get(), result);
                for (auto &a : mc->arguments)
                    analyzeExpr(a.get(), result);
                return;
            }
            // ── وصول آمن `؟.` (OptionalChainExpr): **لا يُرصَد** — نهبط فقط ──
            if (auto *oc = dynamic_cast<AST::OptionalChainExpr *>(expr))
            {
                analyzeExpr(oc->object.get(), result);
                return;
            }

            // ── العقد المركّبة: نهبط في أبنائها ───────────────────────────────
            if (auto *bin = dynamic_cast<AST::BinaryExpr *>(expr))
            {
                analyzeExpr(bin->left.get(), result);
                analyzeExpr(bin->right.get(), result);
                return;
            }
            if (auto *un = dynamic_cast<AST::UnaryExpr *>(expr))
            {
                analyzeExpr(un->operand.get(), result);
                return;
            }
            if (auto *tern = dynamic_cast<AST::TernaryExpr *>(expr))
            {
                analyzeExpr(tern->condition.get(), result);
                analyzeExpr(tern->trueExpr.get(), result);
                analyzeExpr(tern->falseExpr.get(), result);
                return;
            }
            if (auto *nc = dynamic_cast<AST::NullCoalesceExpr *>(expr))
            {
                analyzeExpr(nc->left.get(), result);
                analyzeExpr(nc->right.get(), result);
                return;
            }
            if (auto *call = dynamic_cast<AST::CallExpr *>(expr))
            {
                analyzeExpr(call->callee.get(), result);
                for (auto &a : call->arguments)
                    analyzeExpr(a.get(), result);
                return;
            }
            if (auto *idx = dynamic_cast<AST::IndexExpr *>(expr))
            {
                analyzeExpr(idx->object.get(), result);
                analyzeExpr(idx->index.get(), result);
                return;
            }
            if (auto *asn = dynamic_cast<AST::AssignExpr *>(expr))
            {
                analyzeExpr(asn->value.get(), result);
                narrowed_.erase(asn->name); // D2: التحوّر يُبطل التضييق
                return;
            }
            if (auto *masn = dynamic_cast<AST::MemberAssignExpr *>(expr))
            {
                // (AR) إسناد لحقل كائن اختياريّ: الكائن أيضًا قد يكون عدمًا.
                checkOptionalAccess(masn->object.get(), masn->member, /*method*/ false,
                                    masn->position, result);
                analyzeExpr(masn->object.get(), result);
                analyzeExpr(masn->value.get(), result);
                return;
            }
            if (auto *iasn = dynamic_cast<AST::IndexAssignExpr *>(expr))
            {
                analyzeExpr(iasn->object.get(), result);
                analyzeExpr(iasn->index.get(), result);
                analyzeExpr(iasn->value.get(), result);
                return;
            }
            if (auto *arr = dynamic_cast<AST::ArrayExpr *>(expr))
            {
                for (auto &e : arr->elements)
                    analyzeExpr(e.get(), result);
                return;
            }
            if (auto *map = dynamic_cast<AST::MapExpr *>(expr))
            {
                for (auto &p : map->pairs)
                {
                    analyzeExpr(p.key.get(), result);
                    analyzeExpr(p.value.get(), result);
                }
                return;
            }
            if (auto *wal = dynamic_cast<AST::WalrusExpr *>(expr))
            {
                analyzeExpr(wal->value.get(), result);
                narrowed_.erase(wal->variable); // D2: التحوّر يُبطل التضييق
                return;
            }

            // (AR) تعابير ورقيّة (متغيّر/حرفيّ/هذا/...) أو غير مدعومة: تُتخطّى (D5).
            // (EN) Leaf or unsupported expressions: skipped (sound-conservative).
        }

        // ====================================================================
        // (AR) ماشٍ متدرّج: يهبط في كلّ الجُمل الحاملة للكتل (D5: سليم-متحفّظ)
        // (EN) Recursive walker descending into all block-bearing statements
        // ====================================================================
        void NullSafetyAnalyzer::analyzeStmt(AST::Statement *stmt,
                                             NullSafetyResult &result)
        {
            if (!stmt)
                return;

            // ── تصريح متغير: نقطة الفحص ──────────────────────────────────────
            if (auto *vd = dynamic_cast<AST::VarDeclStmt *>(stmt))
            {
                checkVarDecl(*vd, result);
                // (NS-04) سجّل المتغيّر إن كان نوعه اختياريًّا `T؟`.
                if (vd->type == Types::SadTypeKind::Optional)
                    declareOptional(vd->name);
                analyzeExpr(vd->initializer.get(), result);
                // (NS-03) تصريح جديد يُلغي أيّ تضييق سابق للاسم نفسه؛ ثمّ يُضيّق إن
                //         كان المُهيِّئ حرفيًّا غير عدم (مُثبَت غير null عند هذه النقطة).
                narrowed_.erase(vd->name);
                if (auto *lit = dynamic_cast<AST::LiteralExpr *>(vd->initializer.get()))
                    if (lit->token.getType() != Lexer::TokenType::LITERAL_NULL)
                        narrowed_.insert(vd->name);
                return;
            }

            // ── تصريح متعدّد: يُفكَّك إلى تصريحات فردية ──────────────────────
            if (auto *mv = dynamic_cast<AST::MultiVarDeclStmt *>(stmt))
            {
                for (auto &d : mv->declarations)
                    analyzeStmt(d.get(), result);
                return;
            }

            // ── تعبير-جملة: نقطة هبوط NS-04 في التعابير ──────────────────────
            if (auto *es = dynamic_cast<AST::ExprStmt *>(stmt))
            {
                analyzeExpr(es->expression.get(), result);
                return;
            }

            // ── إرجاع / إعطاء: تحمل تعابير ────────────────────────────────────
            if (auto *rt = dynamic_cast<AST::ReturnStmt *>(stmt))
            {
                analyzeExpr(rt->value.get(), result);
                return;
            }
            if (auto *yd = dynamic_cast<AST::YieldStmt *>(stmt))
            {
                analyzeExpr(yd->value.get(), result);
                return;
            }

            // ── كتلة: نطاق جديد (يمنع تسرّب الاختياريّات) ─────────────────────
            if (auto *blk = dynamic_cast<AST::BlockStmt *>(stmt))
            {
                pushScope();
                for (auto &s : blk->statements)
                    analyzeStmt(s.get(), result);
                popScope();
                return;
            }

            // ── إذا/وإلا: تحليل التدفّق (NS-03) ───────────────────────────────
            if (auto *iff = dynamic_cast<AST::IfStmt *>(stmt))
            {
                analyzeExpr(iff->condition.get(), result);
                const std::set<std::string> saved = narrowed_;

                // (AR) فرع then: الشرط متحقّق → نضيّق ما يُثبِته (`!= لاشيء`).
                std::set<std::string> thenNarrow;
                collectNonNullWhenTrue(iff->condition.get(), thenNarrow);
                for (const auto &n : thenNarrow)
                    narrowed_.insert(n);
                analyzeStmt(iff->thenBranch.get(), result);
                const bool thenExits = stmtAlwaysExits(iff->thenBranch.get());
                narrowed_ = saved;

                // (AR) فرع else: الشرط باطل → نضيّق ما يُثبِته نفيُه (`== لاشيء`).
                std::set<std::string> elseNarrow;
                collectNonNullWhenFalse(iff->condition.get(), elseNarrow);
                for (const auto &n : elseNarrow)
                    narrowed_.insert(n);
                analyzeStmt(iff->elseBranch.get(), result);
                const bool elseExits =
                    iff->elseBranch && stmtAlwaysExits(iff->elseBranch.get());
                narrowed_ = saved;

                // (AR) D4 — تضييق عكسيّ بعد الجملة عند خروج فرعٍ حتميّ:
                //      then يخرج ⇒ بعدها يسري تضييق else؛ else يخرج ⇒ يسري تضييق then.
                if (thenExits)
                    for (const auto &n : elseNarrow)
                        narrowed_.insert(n);
                if (elseExits)
                    for (const auto &n : thenNarrow)
                        narrowed_.insert(n);
                return;
            }

            // ── بينما / لكل / لكل-في ──────────────────────────────────────────
            if (auto *wh = dynamic_cast<AST::WhileStmt *>(stmt))
            {
                analyzeExpr(wh->condition.get(), result);
                analyzeStmt(wh->body.get(), result);
                return;
            }
            if (auto *fr = dynamic_cast<AST::ForStmt *>(stmt))
            {
                pushScope(); // (AR) نطاق متغيّر التهيئة
                analyzeStmt(fr->initializer.get(), result);
                analyzeExpr(fr->condition.get(), result);
                analyzeExpr(fr->increment.get(), result);
                analyzeStmt(fr->body.get(), result);
                popScope();
                return;
            }
            if (auto *frr = dynamic_cast<AST::ForRangeStmt *>(stmt))
            {
                analyzeExpr(frr->iterable.get(), result);
                analyzeStmt(frr->body.get(), result);
                return;
            }

            // ── باستخدام / أجّل / أطلق ────────────────────────────────────────
            if (auto *wt = dynamic_cast<AST::WithStmt *>(stmt))
            {
                analyzeStmt(wt->body.get(), result);
                return;
            }
            if (auto *df = dynamic_cast<AST::DeferStmt *>(stmt))
            {
                analyzeStmt(df->body.get(), result);
                return;
            }
            if (auto *go = dynamic_cast<AST::GoStmt *>(stmt))
            {
                analyzeStmt(go->blockBody.get(), result);
                return;
            }

            // ── حاول/امسك/أخيرًا ──────────────────────────────────────────────
            if (auto *tr = dynamic_cast<AST::TryStmt *>(stmt))
            {
                analyzeStmt(tr->tryBlock.get(), result);
                for (auto &cc : tr->catchClauses)
                    analyzeStmt(cc.body.get(), result);
                analyzeStmt(tr->finallyBlock.get(), result);
                return;
            }

            // ── اختر/حالة ─────────────────────────────────────────────────────
            if (auto *sw = dynamic_cast<AST::SwitchStmt *>(stmt))
            {
                for (auto &cb : sw->cases)
                    analyzeStmt(cb.body.get(), result);
                analyzeStmt(sw->defaultCase.get(), result);
                return;
            }

            // ── دالة / صنف / طريقة: نطاق + معاملات اختيارية ───────────────────
            if (auto *fn = dynamic_cast<AST::FunctionDecl *>(stmt))
            {
                pushScope();
                const std::set<std::string> savedNarrow = narrowed_;
                narrowed_.clear(); // (NS-03) تدفّق مستقلّ لكلّ دالّة (لا تسرّب)
                for (const auto &p : fn->parameters)
                    if (p.type == Types::SadTypeKind::Optional)
                        declareOptional(p.name);
                analyzeStmt(fn->body.get(), result);
                narrowed_ = savedNarrow;
                popScope();
                return;
            }
            if (auto *md = dynamic_cast<AST::MethodDecl *>(stmt))
            {
                pushScope();
                const std::set<std::string> savedNarrow = narrowed_;
                narrowed_.clear(); // (NS-03) تدفّق مستقلّ لكلّ طريقة
                for (const auto &p : md->parameters)
                    if (p.type == Types::SadTypeKind::Optional)
                        declareOptional(p.name);
                analyzeStmt(md->body.get(), result);
                narrowed_ = savedNarrow;
                popScope();
                return;
            }
            if (auto *cd = dynamic_cast<AST::ClassDecl *>(stmt))
            {
                for (auto &m : cd->members)
                    analyzeStmt(m.get(), result);
                return;
            }
            if (auto *cds = dynamic_cast<AST::ClassDeclStmt *>(stmt))
            {
                for (auto &m : cds->methods)
                    analyzeStmt(m.get(), result);
                return;
            }

            // (AR) جُمل غير حاملة للكتل (ExprStmt/Return/Break/...) لا تصريح فيها — تُتخطّى.
            // (EN) Non-block-bearing statements carry no declarations — skipped (D5).
        }

        // ====================================================================
        // (AR) النقطة العامة / Public entry
        // ====================================================================
        NullSafetyResult NullSafetyAnalyzer::analyze(
            const std::vector<std::unique_ptr<AST::Statement>> &program)
        {
            NullSafetyResult result;
            (void)useArabicMessages_; // (AR) الرسالتان تُبنيان دائمًا؛ المحرّك يختار.

            optionalScopes_.clear();
            narrowed_.clear(); // (NS-03) لا تضييق في البداية
            pushScope(); // (AR) النطاق العامّ (NS-04)
            for (const auto &stmt : program)
                analyzeStmt(stmt.get(), result);
            popScope();

            return result;
        }

    } // namespace NullSafety
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
