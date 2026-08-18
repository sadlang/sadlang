// ============================================================================
// sir_builder.cpp - بناء SIR من AST / SIR Builder from AST
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 2.0 (إعادة كتابة كاملة من الصفر)
//
// ملاحظة هامة / Important Note:
// هذا الملف مُعاد كتابته بالكامل من الصفر بما يتوافق 100% مع:
// This file is completely rewritten from scratch to comply 100% with:
// - STRICT_CODING_RULES.md
// - sir_builder.h
// - sir_types.h
// - sir_instruction.h
// - sir_module.h
// - AST headers (ast_node.h, expressions.h, statements.h, declarations.h)
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "ui_nodes.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h" // (AR) VolatileVarDeclStmt (@متطاير، اللبنة 3.14)
#include "sad_ui/generated/color_table_generated.h" // (AR) تعداد الألوان المدمَج (SoT)
#include "sad_event_layout_generated.h" // (② rfcs#46) تخطيط «حدث» المولَّد (SAD_EVENT_FIELDS/SAD_EVENT_STRUCT_NAME) — SoT صنف الحدث المضمَّن
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // (AR) اسم مدمج البيانات المضمَّنة (اللبنة 3.14) — ثابت مسمّى واحد يشاركه
            //      الكشف والتخطّي (تفاديًا لانحراف سلسلتين نصّيّتين مباشرتين).
            static const std::string kByteBlobBuiltin = "\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa\xd8\xa7\xd8\xaa"; // «بايتات»

            // ============================================================================
            // المنشئ / Constructor
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:287
            // التوقيع / Signature: SIRBuilder();
            //
            // المتغيرات المُهيّئة / Initialized variables:
            // - module_: sir_builder.h:582 (std::shared_ptr<SIRModule>)
            // - currentFunction_: sir_builder.h:583 (std::shared_ptr<SIRFunction>)
            // - currentBlock_: sir_builder.h:584 (SIRBasicBlock*)
            // - currentScopeLevel_: sir_builder.h:599 (int)
            // - registerCounter_: sir_builder.h:600 (int)
            // - labelCounter_: sir_builder.h:601 (int)
            // ============================================================================
            SIRBuilder::SIRBuilder()
            {
                // (AR) جميع الحقول مُهيّأة في SIRBuilderContext (base class) بقيم افتراضية
                // (EN) All fields are default-initialized in SIRBuilderContext (base class)
                // (AR) تهيئة بنّاء استدعاءات الطرق (Phase 6 Step 2)
                // (EN) Initialize method-call builder (Phase 6 Step 2)
                methodCalls_ = std::make_unique<MethodCallBuilder>(*this);
                // (AR) تهيئة بنّاء الدوال المدمجة (Phase 6 Step 3)
                // (EN) Initialize builtin function builder (Phase 6 Step 3)
                builtins_ = std::make_unique<BuiltinBuilder>(*this);
                // (AR) تهيئة بنّاء استدعاءات الدوال (Phase 6 Step 4)
                // (EN) Initialize function call builder (Phase 6 Step 4)
                calls_ = std::make_unique<CallBuilder>(*this);
                // (AR) تهيئة بنّاء الأصناف والسمات (Phase 6 Step 5)
                // (EN) Initialize class/trait builder (Phase 6 Step 5)
                classes_ = std::make_unique<ClassBuilder>(*this);
                // (AR) تهيئة بنّاء العبارات (Phase 6 Step 6)
                // (EN) Initialize statement builder (Phase 6 Step 6)
                statements_ = std::make_unique<StatementBuilder>(*this);
                // (AR) تهيئة بنّاء التعابير (Phase 6 Step 7)
                // (EN) Initialize expression builder (Phase 6 Step 7)
                expressions_ = std::make_unique<ExpressionBuilder>(*this);
                // (AR) تهيئة بنّاء القوالب (Phase 6 Step 8 - الأخيرة)
                // (EN) Initialize template builder (Phase 6 Step 8 - final)
                templates_ = std::make_unique<TemplateBuilder>(*this);

                // (AR) تسجيل تعداد الألوان المدمَج «ألوان» (مولَّد من ui_colors.yaml):
                //   نملأ ثوابت الأعضاء النصّيّة مباشرةً فيصير «ألوان.أحمر» ⇐ «أحمر»
                //   (يستهلكها وصولُ العضو بعد إصلاح م0). المفسّر يسجّله عبر بادئته،
                //   فالنتيجة متكافئة. مصدرٌ واحد ⇒ لا تباعد.
                // (EN) Register the builtin `ألوان` color enum (generated from SoT):
                //   pre-populate the string member constants so `ألوان.أحمر` → «أحمر»
                //   (consumed by member access after the m0 fix). Parity with the
                //   interpreter, which registers it via its prelude.
#define X(qualified, value) enumStringConstants_[qualified] = value;
                SAD_UI_COLOR_MEMBERS(X)
#undef X
            }

            // ============================================================================
            // buildModule - بناء وحدة SIR كاملة من AST
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:329
            // التوقيع / Signature:
            //   std::shared_ptr<SIRModule> buildModule(AST::ProgramNode* program);
            //
            // المعاملات / Parameters:
            // - program: AST::ProgramNode* = Sad::AST::StmtList* (sir_builder.h:51)
            //            وهو alias لـ std::vector<StmtPtr> (ast_node.h:170)
            //
            // الإرجاع / Returns:
            // - std::shared_ptr<SIRModule>: وحدة SIR كاملة أو nullptr عند الخطأ
            //
            // الدوال المستدعاة / Called functions:
            // - std::make_shared<SIRModule>: standard library
            // - buildFunction: sir_builder.h:348
            // - buildGlobalVariable: sir_builder.h:356
            // - buildClass: sir_builder.h:364
            //
            // AST Node Types Used:
            // - Sad::AST::FunctionDecl: declarations.h:19
            // - Sad::AST::VarDeclStmt: statements.h:74
            // - Sad::AST::ClassDecl: declarations.h:122
            // ============================================================================
            // ============================================================================
            // (AR) خفض عقد الواجهة (واجهة) إلى أصناف SIR — STORY-UI-W15-04
            // (EN) Lowering UI declaration nodes (واجهة) to SIR classes — STORY-UI-W15-04
            // ============================================================================
            // (AR) عقدة `واجهة` تُعامَل كصنف خاص بالواجهة: تصريحات @حالة تصبح حقولًا،
            //      ودوال المكوّن (بما فيها `بناء()`) تصبح طُرقًا. نُعيد بناء `ClassDecl`
            //      مكافئة لتمرّ عبر مسار بناء الأصناف القائم (المراحل 1.3/1.35/2A)،
            //      تماشيًا مع منطق المفسّر في `statement_executor_oop_struct_test.cpp:503`
            //      (إعادة استخدام آلية الأصناف القائمة — CW-19/CW-20).
            // (EN) A `واجهة` node is treated as a UI-specific class: @state declarations
            //      become fields and component methods (including `بناء()`) become methods.
            //      We synthesize an equivalent `ClassDecl` so it flows through the existing
            //      class-building phases (1.3/1.35/2A), mirroring the interpreter logic.
            namespace
            {
                // (AR) خريطة اسم النوع → نوع الحقل (مطابِقة لاصطلاح expression_lowlevel.cpp:232)
                // (EN) Type-name → field type (matches expression_lowlevel.cpp:232 convention)
                Sad::Types::SadTypeKind uiTypeNameToKind(const std::string &typeName)
                {
                    if (typeName == "\xD8\xB1\xD9\x82\xD9\x85" /*رقم*/ ||
                        typeName == "\xD8\xB9\xD8\xAF\xD8\xAF" /*عدد*/ ||
                        typeName == "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD" /*صحيح*/ ||
                        typeName == "i64" || typeName == "int" || typeName == "integer")
                        return Sad::Types::SadTypeKind::Integer;
                    if (typeName == "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A" /*عشري*/ ||
                        typeName == "\xD9\x85\xD8\xB6\xD8\xA7\xD8\xB9\xD9\x81" /*مضاعف*/ ||
                        typeName == "\xD8\xAD\xD9\x82\xD9\x8A\xD9\x82\xD9\x8A" /*حقيقي*/ ||
                        typeName == "f64" || typeName == "float" || typeName == "double")
                        return Sad::Types::SadTypeKind::Float;
                    if (typeName == "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A" /*منطقي*/ ||
                        typeName == "bool" || typeName == "boolean")
                        return Sad::Types::SadTypeKind::Boolean;
                    if (typeName == "\xD9\x86\xD8\xB5" /*نص*/ ||
                        typeName == "string" || typeName == "str")
                        return Sad::Types::SadTypeKind::String;
                    if (typeName == "\xD9\x85\xD8\xB5\xD9\x81\xD9\x88\xD9\x81\xD8\xA9" /*مصفوفة*/ ||
                        typeName == "array")
                        return Sad::Types::SadTypeKind::Array;
                    // (AR) بدون نوع صريح → غير معروف (يُستنتج من المُهيّئ لاحقًا)
                    // (EN) No explicit type → Unknown (inferred from initializer later)
                    return Sad::Types::SadTypeKind::Unknown;
                }

                // (AR) تركيب ClassDecl مكافئة لعقدة واجهة (تستهلك حقول العقدة بالنقل)
                // (EN) Synthesize an equivalent ClassDecl from a UI node (consumes node fields by move)
                std::unique_ptr<Sad::AST::ClassDecl> synthesizeClassFromUI(Sad::AST::UIDeclarationNode &ui)
                {
                    Sad::AST::StmtList members;

                    // (AR) تصريحات @حالة → حقول الصنف
                    // (EN) @state declarations → class fields
                    for (auto &sd : ui.stateDecls)
                    {
                        if (!sd)
                            continue;
                        Sad::Types::SadTypeKind ft = uiTypeNameToKind(sd->typeName);
                        members.push_back(std::make_unique<Sad::AST::FieldDecl>(
                            sd->name, ft, std::move(sd->initializer),
                            Sad::AST::AccessModifier::PUBLIC, /*isStatic*/ false, sd->position));
                    }

                    // (AR) دوال المكوّن (FunctionDecl) → طُرق الصنف (MethodDecl)
                    // (EN) Component methods (FunctionDecl) → class methods (MethodDecl)
                    for (auto &m : ui.methods)
                    {
                        auto *fn = dynamic_cast<Sad::AST::FunctionDecl *>(m.get());
                        if (!fn)
                            continue;
                        members.push_back(std::make_unique<Sad::AST::MethodDecl>(
                            fn->name, std::move(fn->parameters), fn->returnType,
                            std::move(fn->body), Sad::AST::AccessModifier::PUBLIC,
                            /*isStatic*/ false, /*isVirtual*/ false, /*isOverride*/ false,
                            /*isAbstract*/ false, fn->position, fn->is_async));
                    }

                    // (AR) باني الصنف ذو الأب الواحد (الوراثة `يرث` → superclass)
                    // (EN) Single-base ClassDecl constructor (`يرث` inheritance → superclass)
                    return std::make_unique<Sad::AST::ClassDecl>(
                        ui.name, ui.parentName, std::move(members), ui.isExported, ui.position);
                }
            } // namespace

            std::shared_ptr<SIRModule> SIRBuilder::buildModule(AST::ProgramNode *program)
            {
                // (AR) امسح قناتَي التشخيص عند دخول كلّ وحدة: مستهلكٌ يعيد استخدام الباني (LSP/
                //   متعدّد الوحدات) لا يرث أخطاء/تحذيرات وحدةٍ سابقة (M1). السائق يُنشئ بانيًا
                //   لكلّ ملفّ فلا يتأثّر، لكنّ المسح يجعل السلوك صحيحًا لأيّ مستهلك.
                // (EN) Clear both diagnostic channels on each module entry so a builder reused
                //   across modules (LSP/multi-unit) never inherits a prior module's diagnostics.
                errors_.clear();
                warnings_.clear();
                // (AR) (اللبنة 3.17، رصد Amelia H1) امسح مجموعتَي رموز @رمز المُصدَّرة أيضًا:
                //      باقٍ حيّ عبر الوحدات في بانٍ مُعاد استخدامه (LSP) يُطلق SEM024 كاذبًا
                //      على رمزٍ شرعيّ في وحدةٍ تالية (وحدتان مستقلّتان قد تُصدِّران الاسم نفسه).
                // (EN) (Brick 3.17, Amelia H1) Also clear the @رمز exported-symbol sets: state
                //      surviving across modules in a reused builder (LSP) would raise a false
                //      SEM024 on a legitimate symbol in a later module (independent units may
                //      export the same name).
                exportedFunctionLinkNames_.clear();
                exportedVarLinkNames_.clear();
                // (AR) امسح أشجار الوحدات المستوردة المجموعة للاستنتاج: بانٍ مُعاد
                //      استخدامه (LSP) يحمل مؤشّرات وحدةٍ سابقة قد تُبطَل خبيئتها.
                // (EN) Clear collected imported module ASTs for inference: a reused
                //      builder (LSP) would hold a prior module's possibly-stale pointers.
                importedModuleBodies_.clear();
                preRegisteredImportNames_.clear();
                pendingGlobalTypes_.clear();
                // (AR) وخريطةُ الأسماء المستعارة وأثرُها في النطاق العامّ: «كـ» ربطُ اسمٍ
                //      خاصٌّ بالملفّ الذي كتبه، وهي تُملأ بالإضافة لا بالإسناد؛ فبانٍ
                //      مُعاد استخدامه يصرّف ملفَّين كان يحمل استعارةَ الأوّل إلى الثاني
                //      — تسرّبُ ISSUE-088 نفسُه على مستوى الملفّ لا الوحدة.
                // (EN) …and the alias map with its global-scope residue: «as» is a binding
                //      private to the file that wrote it, and the map is appended to rather
                //      than assigned; a reused builder compiling two files would carry the
                //      first file's alias into the second — the very ISSUE-088 leak, at
                //      file level rather than module level.
                importAliases_.clear();
                aliasSeededGlobals_.clear();
                // (AR) وفضاءاتُ الوحدات معها للسبب عينِه (ISSUE-090).
                // (EN) …and the module namespaces with them, for the same reason (ISSUE-090).
                moduleNamespaces_.clear();

                if (!program)
                {
                    errors_.push_back("Error: Null program AST");
                    return nullptr;
                }

                // (AR) إنشاء وحدة SIR جديدة (sir_module.h:501 - SIRModule constructor)
                // (EN) Create new SIR module
                module_ = std::make_shared<SIRModule>("main");

                // ═══════════════════════════════════════════════════════════════════
                // (② rfcs#46) البنية المضمَّنة «حدث»: نُسجّلها كصنفٍ في الوحدة قبل أيّ
                //   معالجة، فتشتقّ حقولها حصريًّا من مصدر الحقيقة (SAD_EVENT_FIELDS
                //   المولَّد من types.yaml) — لا أسماءٍ حرفيّة هنا. أثرها مزدوج:
                //     (١) الواجهة الأماميّة: معالِج ص `دالة(حدث ح) { ح.دلتا_س … }` يُنمَّط
                //         معامله كصنف «حدث» فيُحلّ الوصولُ العضويّ ويُتحقّق نوعيًّا؛
                //     (٢) الخلفيّة: preprocessClasses يُوجِد classStructTypes["حدث"]
                //         دائمًا، فيبني thunkُ الحدث بنيةً بهذا التخطيط بالضبط (اللبنة 4).
                //   بترتيب SAD_EVENT_FIELDS نفسه ⇒ فهارس الحقول تطابق الجسر بايتيًّا.
                // (EN) Built-in «حدث» struct registered as a module class up-front, its
                //   fields derived solely from the generated SoT (SAD_EVENT_FIELDS). This
                //   both types event-handler params (`دالة(حدث ح)`) so member access
                //   resolves, and guarantees classStructTypes["حدث"] for the backend thunk.
                {
                    auto eventClass = std::make_shared<SIRClass>(
                        std::string(::Sad::Types::EventLayout::SAD_EVENT_STRUCT_NAME));
                    for (const auto &f : ::Sad::Types::EventLayout::SAD_EVENT_FIELDS)
                    {
                        eventClass->addField(std::string(f.nameUtf8), f.kind);
                    }
                    module_->addClass(eventClass);
                }

                // ===========================================================
                // Phase 0: Lower UI components (واجهة) to equivalent ClassDecl
                // so the existing class-building phases (1.3/1.35/2A) handle
                // them. Mirrors the interpreter's visitUIDeclaration (CW-19/20).
                // ===========================================================
                for (auto &uiStmt : *program)
                {
                    if (!uiStmt)
                        continue;
                    if (auto *uiNode = dynamic_cast<Sad::AST::UIDeclarationNode *>(uiStmt.get()))
                    {
                        uiStmt = synthesizeClassFromUI(*uiNode);
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] Phase0: lowered UI component '"
                                  << static_cast<Sad::AST::ClassDecl *>(uiStmt.get())->name
                                  << "' to ClassDecl" << std::endl;
#endif
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 0.5 (ISSUE-058): مسح مسبق لأنواع إسناد الحقول
                //      يجمع نوع القيمة الحرفيّة المُسنَدة لكلّ حقل (كائن.حقل = حرفيّ) عبر
                //      البرنامج. إن اتّفقت الإسنادات على نوعٍ بدائيّ واحد يُستنتَج للحقل غير
                //      المُصرَّح بلا مُهيّئ (بدل Pointer الافتراضيّ الذي ينهار مع عدد). عند
                //      التضارب يُوسَم Unknown فيُبقى الافتراضيّ (آمن للنصوص/الكائنات).
                // (EN) Phase 0.5 (ISSUE-058): pre-scan field-assignment types. Collects the
                //      literal value type assigned to each field (obj.field = literal) across
                //      the program. If assignments agree on one primitive it is inferred for an
                //      untyped/uninitialized field (instead of the crashing Pointer default). On
                //      conflict it is marked Unknown, keeping the default (safe for str/objects).
                // ═══════════════════════════════════════════════════════════════════
                {
                    // (AR) خريطة متغيّر→صنف (من «متغير v = صنف(...)») لتأهيل مفتاح الحقل
                    //      بالصنف ومنع تلوّث حقلٍ باسمٍ مشترك عبر أصناف مختلفة.
                    // (EN) var→class map (from «var v = Class(...)») to qualify the field key by
                    //      class, preventing cross-class pollution of same-named fields.
                    std::unordered_map<std::string, std::string> varClass;

                    auto litType = [](Sad::AST::Expression *e) -> SadTypeKind {
                        if (auto *lit = dynamic_cast<Sad::AST::LiteralExpr *>(e))
                        {
                            switch (lit->token.getType())
                            {
                            case Sad::Lexer::TokenType::NUMBER_INTEGER: return SadTypeKind::Integer;
                            case Sad::Lexer::TokenType::NUMBER_DOUBLE:  return SadTypeKind::Float;
                            case Sad::Lexer::TokenType::STRING_LITERAL: return SadTypeKind::String;
                            case Sad::Lexer::TokenType::LITERAL_TRUE:
                            case Sad::Lexer::TokenType::LITERAL_FALSE:  return SadTypeKind::Boolean;
                            default: break;
                            }
                        }
                        return SadTypeKind::Unknown;
                    };
                    // (AR) المفتاح «صنف.حقل»؛ لا نُسجّل إلّا حين نعرف الصنف المُضيف.
                    // (EN) Key "class.field"; only record when the host class is known.
                    auto record = [this](const std::string &cls, const std::string &field, SadTypeKind t) {
                        if (t == SadTypeKind::Unknown || cls.empty())
                            return;
                        std::string key = cls + "." + field;
                        auto it = inferredFieldTypes_.find(key);
                        if (it == inferredFieldTypes_.end())
                            inferredFieldTypes_[key] = t;
                        else if (it->second != t)
                            it->second = SadTypeKind::Unknown; // (AR) تضارب ⇒ لا تستنتج
                    };
                    // (AR) استنتاج صنف مُستقبِل الإسناد: «هذا»⇒الصنف الحاليّ، متغيّرٌ معروف⇒صنفه.
                    // (EN) Resolve the assignment target's class: «this»⇒current class, known var⇒its class.
                    auto resolveClass = [&](Sad::AST::Expression *obj, const std::string &cur) -> std::string {
                        if (dynamic_cast<Sad::AST::ThisExpr *>(obj))
                            return cur;
                        if (auto *v = dynamic_cast<Sad::AST::VariableExpr *>(obj))
                        {
                            auto it = varClass.find(v->name);
                            if (it != varClass.end())
                                return it->second;
                        }
                        return "";
                    };
                    std::function<void(Sad::AST::Statement *, const std::string &)> scan;
                    scan = [&](Sad::AST::Statement *s, const std::string &cur) {
                        if (!s)
                            return;
                        // (AR) «متغير v = صنف(...)» أو «صنف(...) جديد» ⇒ سجّل v→صنف
                        if (auto *vd = dynamic_cast<Sad::AST::VarDeclStmt *>(s))
                        {
                            if (vd->initializer)
                            {
                                std::string cn;
                                if (auto *ne = dynamic_cast<Sad::AST::NewExpr *>(vd->initializer.get()))
                                    cn = ne->className;
                                else if (auto *ce = dynamic_cast<Sad::AST::CallExpr *>(vd->initializer.get()))
                                    if (auto *cv = dynamic_cast<Sad::AST::VariableExpr *>(ce->callee.get()))
                                        cn = cv->name;
                                if (!cn.empty())
                                    varClass[vd->name] = cn;
                            }
                            return;
                        }
                        if (auto *es = dynamic_cast<Sad::AST::ExprStmt *>(s))
                        {
                            if (auto *ma = dynamic_cast<Sad::AST::MemberAssignExpr *>(es->expression.get()))
                                record(resolveClass(ma->object.get(), cur), ma->member, litType(ma->value.get()));
                            return;
                        }
                        if (auto *b = dynamic_cast<Sad::AST::BlockStmt *>(s))
                            for (auto &c : b->statements) scan(c.get(), cur);
                        else if (auto *iff = dynamic_cast<Sad::AST::IfStmt *>(s))
                        { scan(iff->thenBranch.get(), cur); scan(iff->elseBranch.get(), cur); }
                        else if (auto *w = dynamic_cast<Sad::AST::WhileStmt *>(s))
                            scan(w->body.get(), cur);
                        else if (auto *f = dynamic_cast<Sad::AST::ForStmt *>(s))
                            scan(f->body.get(), cur);
                        else if (auto *fr = dynamic_cast<Sad::AST::ForRangeStmt *>(s))
                            scan(fr->body.get(), cur);
                        else if (auto *fn = dynamic_cast<Sad::AST::FunctionDecl *>(s))
                            scan(fn->body.get(), cur);
                        else if (auto *cl = dynamic_cast<Sad::AST::ClassDecl *>(s))
                            for (auto &m : cl->members)
                            {
                                if (auto *md = dynamic_cast<Sad::AST::MethodDecl *>(m.get()))
                                    scan(md->body.get(), cl->name);
                                else if (auto *cd = dynamic_cast<Sad::AST::ConstructorDecl *>(m.get()))
                                    scan(cd->body.get(), cl->name);
                            }
                        else if (auto *tr = dynamic_cast<Sad::AST::TryStmt *>(s))
                        {
                            scan(tr->tryBlock.get(), cur);
                            for (auto &cc : tr->catchClauses) scan(cc.body.get(), cur);
                            scan(tr->finallyBlock.get(), cur);
                        }
                        else if (auto *sw = dynamic_cast<Sad::AST::SwitchStmt *>(s))
                        {
                            for (auto &cb : sw->cases) scan(cb.body.get(), cur);
                            scan(sw->defaultCase.get(), cur);
                        }
                        else if (auto *mt = dynamic_cast<Sad::AST::MatchStmt *>(s))
                        {
                            for (auto &cc : mt->cases)
                                for (auto &bs : cc.body) scan(bs.get(), cur);
                        }
                        else if (auto *go = dynamic_cast<Sad::AST::GoStmt *>(s))
                            scan(go->blockBody.get(), cur);
                    };
                    for (const auto &stmt : *program)
                        scan(stmt.get(), "");
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة الأولى: تسجيل توقيعات جميع الدوال مسبقاً
                // (EN) Phase 1: Pre-register all function signatures for forward references
                // ═══════════════════════════════════════════════════════════════════
                preRegisterFunctionSignatures(program);

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 1.3: تسجيل توقيعات دوال الأصناف مسبقاً (خاصة الساكنة)
                //      حتى تتوفر في functionTable_ قبل المرحلة 1.7 (استنتاج الأنواع)
                //      بدون هذا: استدعاءات مثل سجل.سجّل("أول") لا تُحدّث أنواع المعاملات
                // (EN) Phase 1.3: Pre-register class method signatures (especially static)
                //      So they're available in functionTable_ before Phase 1.7 (type inference)
                //      Without this: calls like Class.method("arg") don't update param types
                // ═══════════════════════════════════════════════════════════════════
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    auto *classDecl = dynamic_cast<Sad::AST::ClassDecl *>(stmt.get());
                    if (!classDecl)
                        continue;

                    for (const auto &member : classDecl->members)
                    {
                        if (!member)
                            continue;

                        auto *methodDecl = dynamic_cast<Sad::AST::MethodDecl *>(member.get());
                        if (!methodDecl)
                            continue;

                        if (methodDecl->isAbstract)
                            continue;

                        std::string fullMethodName = classDecl->name + "." + methodDecl->name;

                        // (AR) تخطي إذا سبق التسجيل
                        // (EN) Skip if already registered
                        if (functionTable_.count(fullMethodName) > 0)
                            continue;

                        FunctionInfo methodInfo;
                        methodInfo.name = fullMethodName;
                        methodInfo.returnType = astTypeToSIRType(methodDecl->returnType);

                        // (AR) الدوال غير الساكنة تأخذ self كمعامل أول
                        // (EN) Non-static methods take self as first parameter
                        if (!methodDecl->isStatic)
                        {
                            methodInfo.parameters.push_back(SIRParameter(kSelfParamName, SadTypeKind::Integer));
                        }

                        for (const auto &param : methodDecl->parameters)
                        {
                            SadTypeKind paramType = astTypeToSIRType(param.type);
                            methodInfo.parameters.push_back(SIRParameter(param.name, paramType));
                        }

                        methodInfo.sirFunction = nullptr;
                        functionTable_[fullMethodName] = methodInfo;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] Phase1.3: registered method '" << fullMethodName
                                  << "' static=" << methodDecl->isStatic
                                  << " params=" << methodInfo.parameters.size() << std::endl;
#endif
                    }
                }

                // ═══════════════════════════════════════════════════════════════════════
                // (AR) المرحلة 1.35: تسجيل توقيعات البُناة مبكراً (قبل Phase 1.7/1.8)
                //      بدون هذا: Phase 1.7 لا يستطيع تحديث أنواع معاملات الباني
                //      و Phase 1.8 لا يستطيع استنتاج أنواع الحقول من وسائط الباني
                //      مما يُبقي الحقول كـ Pointer والمعاملات كـ Integer
                // (EN) Phase 1.35: Pre-register constructor signatures (before Phase 1.7/1.8)
                //      Without this: Phase 1.7 can't update constructor param types
                //      and Phase 1.8 can't infer field types from constructor arguments
                //      leaving fields as Pointer and params as Integer
                // ═══════════════════════════════════════════════════════════════════════
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    auto *classDecl = dynamic_cast<Sad::AST::ClassDecl *>(stmt.get());
                    if (!classDecl)
                        continue;

                    for (const auto &member : classDecl->members)
                    {
                        if (!member)
                            continue;

                        auto *ctorDecl = dynamic_cast<Sad::AST::ConstructorDecl *>(member.get());
                        if (!ctorDecl)
                            continue;

                        std::string fullCtorName = classDecl->name + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني

                        // (AR) تخطي إذا سبق التسجيل
                        // (EN) Skip if already registered
                        if (functionTable_.count(fullCtorName) > 0)
                            continue;

                        FunctionInfo ctorInfo;
                        ctorInfo.name = fullCtorName;
                        ctorInfo.returnType = SadTypeKind::Void;

                        // (AR) المعامل الأول دائماً self
                        // (EN) First parameter is always self
                        ctorInfo.parameters.push_back(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                        for (const auto &param : ctorDecl->parameters)
                        {
                            SadTypeKind paramType = astTypeToSIRType(param.type);
                            ctorInfo.parameters.push_back(SIRParameter(param.name, paramType));
                        }

                        ctorInfo.sirFunction = nullptr;
                        functionTable_[fullCtorName] = ctorInfo;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] Phase1.35: registered ctor '" << fullCtorName
                                  << "' params=" << ctorInfo.parameters.size() << std::endl;
#endif
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 1.5: تسجيل المتغيرات العامة مسبقاً في نطاق عام ثابت
                // (EN) Phase 1.5: Pre-register global variables in a persistent global scope
                // ═══════════════════════════════════════════════════════════════════
                // (AR) هذا النطاق يبقى طوال بناء الوحدة حتى تستطيع الدوال الوصول للمتغيرات العامة
                // (EN) This scope persists throughout module building so functions can access globals
                enterScope(); // (AR) النطاق العام - سيبقى حتى نهاية buildModule

                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    // (AR) استخراج VarDeclStmt مباشرة أو من داخل ExportDecl/ExportStmt
                    // (EN) Extract VarDeclStmt directly or from inside ExportDecl/ExportStmt
                    Sad::AST::VarDeclStmt *varDecl = nullptr;

                    varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(stmt.get());

                    if (!varDecl)
                    {
                        if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                        {
                            if (exportDecl->declaration)
                            {
                                varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(exportDecl->declaration.get());
                            }
                        }
                    }
                    if (!varDecl)
                    {
                        if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                        {
                            if (exportStmt->declaration)
                            {
                                varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(exportStmt->declaration.get());
                            }
                        }
                    }
                    // (AR) اللبنة 3.14: لصيقة «متطاير» (isVolatile) تُضبَط مباشرةً على
                    //      VarDeclStmt في parseVarDecl، فلا حاجة لفكّ غلاف — تُقرأ أدناه.

                    if (varDecl)
                    {
                        // (AR) تسجيل المتغير العام في النطاق العام — عبر سلطةِ الخانةِ
                        //      كي يفكَّ `T؟`؛ وإلّا اختلف هذا النطاقُ عن SIRGlobalVariable.
                        // (EN) Register the global in the global scope — through the storage
                        //      authority so `T?` is unwrapped; otherwise this scope and
                        //      SIRGlobalVariable disagree about one slot.
                        SadTypeKind varType = resolveDeclaredStorageKind(varDecl->type,
                                                                         varDecl->sadType.get(),
                                                                         astTypeToSIRType(varDecl->type));

                        // ================================================================
                        // (AR) استنتاج النوع من القيمة الحرفية عندما يكون النوع غير معروف:
                        //      متغير س = 3.14 → النوع UNKNOWN → يجب أن يكون F64 لا I64
                        //      بدون هذا: العشري يُخزن كـ i64 (بتات خام) ويُطبع بشكل خاطئ
                        // (EN) Infer type from literal initializer when type is UNKNOWN:
                        //      var x = 3.14 → type UNKNOWN → should be F64 not I64
                        //      Without this: double stored as i64 (raw bits) and printed incorrectly
                        // ================================================================
                        if (varDecl->type == Types::SadTypeKind::Unknown && varDecl->initializer)
                        {
                            // (AR) استنتاج النوع من أي تعبير — ليس فقط الحرفيات
                            //      نستخدم دالة مساعدة تمشي في شجرة التعبير بشكل عميق
                            //      لاكتشاف أي وجود لـ NUMBER_DOUBLE أو متغير Float
                            // (EN) Infer type from any expression — not just literals
                            //      Use helper that walks expression tree to detect float presence
                            varType = inferExprType(varDecl->initializer.get());
                        }

                        // (AR) بابُ ISSUE-138 نفسُه: بلا نوعٍ وبلا تهيئةٍ ⇒ خانةٌ ديناميّة.
                        //      وغيابُه هنا وحدَه — مع وجودِه في المسارِ المحلّيّ — كان
                        //      يجعل العامَّ `i64` ونوعَه المُعلَنَ `Any` ⇒ انهيارُ مترجِمٍ
                        //      داخليٌّ في «متغير ك» ثمّ «اطبع_سطر(ك)» بلا سواهما.
                        // (EN) Same ISSUE-138 door. Its absence HERE alone, while the local
                        //      path had it, allocated an i64 global while declaring its kind
                        //      Any ⇒ internal compiler error on the two-line program.
                        varType = resolveBareSlotStorageKind(
                            varDecl->type, varDecl->initializer != nullptr, varType);

                        VariableInfo globalVarInfo;
                        globalVarInfo.name = varDecl->name;
                        globalVarInfo.type = varType;
                        // (AR) [طبقة طبيعي64 — الخطوة ٧/٥] النوع السطحيّ المُصرَّح صراحةً (طبيعي64/بايت)
                        //      — Unknown إن كان مُستنتَجًا. نظيرُ statement_assign_if.cpp:350 للمتغيّر
                        //      المحلّيّ. بدونه: عامّ طبيعي64 مُستعمَل في %،//،مقارنة **داخل دالّة** يُرى
                        //      Integer (resolveSurfaceType يقرأ declaredSurfaceType=Unknown) ⇒ المترجم
                        //      موقَّع بينما المفسّر (getDeclaredType يجد نوع العامّ) لا-موقَّع ⇒ انفراج
                        //      (رصده أميليا). نُخزّن الخام varDecl->type لا المُستنتَج varType.
                        // (EN) [طبيعي64 layer — Step 7/5] Explicitly-declared surface type (طبيعي64/Byte)
                        //      — Unknown when inferred. Sibling of statement_assign_if.cpp:350 for locals.
                        //      Without it: a طبيعي64 global used in %,//,comparison INSIDE a function is
                        //      seen as Integer (resolveSurfaceType reads declaredSurfaceType=Unknown) ⇒ the
                        //      compiler is signed while the interpreter (getDeclaredType finds the global's
                        //      type) is unsigned ⇒ divergence (found by Amelia). Store the raw
                        //      varDecl->type, not the inferred varType.
                        globalVarInfo.declaredSurfaceType = varDecl->type;
                        globalVarInfo.registerName = "%" + varDecl->name;
                        globalVarInfo.isGlobal = true;
                        globalVarInfo.isMutable = !varDecl->isConst;
                        globalVarInfo.scopeLevel = 0;

                        // (AR) استنتاج نوع عنصر المصفوفة من ArrayExpr لدعم foreach
                        //      مثال: متغير الفواكه = ["تفاح"، "موز"] → elementType = STRING
                        //      هذا يُمكّن ARRAY_GET من استخدام النوع الصحيح عند التكرار
                        // (EN) Infer array element type from ArrayExpr for foreach support
                        if (varDecl->initializer)
                        {
                            if (auto *arrayExpr = dynamic_cast<Sad::AST::ArrayExpr *>(varDecl->initializer.get()))
                            {
                                if (!arrayExpr->elements.empty())
                                {
                                    if (auto *litElem = dynamic_cast<Sad::AST::LiteralExpr *>(arrayExpr->elements[0].get()))
                                    {
                                        auto elemTokenType = litElem->token.getType();
                                        if (elemTokenType == Lexer::TokenType::STRING_LITERAL)
                                        {
                                            globalVarInfo.elementType = SadTypeKind::String;
                                        }
                                        else if (elemTokenType == Lexer::TokenType::NUMBER_DOUBLE)
                                        {
                                            globalVarInfo.elementType = SadTypeKind::Float;
                                        }
                                        else if (elemTokenType == Lexer::TokenType::LITERAL_TRUE ||
                                                 elemTokenType == Lexer::TokenType::LITERAL_FALSE)
                                        {
                                            globalVarInfo.elementType = SadTypeKind::Boolean;
                                        }
                                    }
                                }
                            }
                        }

                        addVariable(globalVarInfo);

                        // (AR) تتبع نوع الصنف مسبقاً للمتغيرات العامة المهيأة بـ جديد صنف()
                        // (EN) Pre-track class type for global variables initialized with new ClassName()
                        if (varDecl->initializer)
                        {
                            if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(varDecl->initializer.get()))
                            {
                                classInstanceTypes_[varDecl->name] = newExpr->className;
                            }
                        }

                        // (AR) إضافة المتغير العام أيضاً لوحدة SIR حتى ينشئ LLVM CodeGen متغيرات عامة حقيقية
                        // (EN) Also add global variable to SIR module so LLVM CodeGen creates real LLVM globals
                        auto sirGlobal = std::make_shared<SIRGlobalVariable>(varDecl->name, varType);
                        sirGlobal->isConstant = varDecl->isConst;
                        // (AR) اللبنة 3.14: سمات التخزين الساكن من التوجيهات
                        sirGlobal->linkName = varDecl->linkSymbol;   // @رمز("اسم") — رمز مُصدَّر
                        // (AR) (اللبنة 3.17، رصد Amelia H2) تقاطع رمز المتغيّر المُصدَّر مع
                        //      رموز الدوالّ المُصدَّرة: متغيّر يشارك دالّةً الاسمَ يجعل LLVM
                        //      يُعيد تسمية أحدهما (@اسم.1) فيَضيع عقد الـABI صامتًا ⇒ SEM024.
                        //      نتصادم مع مجموعة الدوالّ فقط (تصادم متغيّر/متغيّر يبقى لحارسه
                        //      القائم SEM022 خلفيًّا بلا تغيير)، ونسجّل الرمز في مجموعة
                        //      المتغيّرات ليكشفه مسار الدالّة إن عولجت لاحقًا (استقلال الترتيب).
                        // (EN) (Brick 3.17, Amelia H2) Cross-check the exported variable symbol
                        //      against exported function symbols: a variable sharing a function's
                        //      name makes LLVM rename one (@name.1), silently losing the ABI
                        //      contract ⇒ SEM024. Collide only with the function set (var/var stays
                        //      on its existing SEM022 backend guard); record in the variable set so
                        //      a later-processed function detects it (order-independent).
                        if (!varDecl->linkSymbol.empty())
                        {
                            if (exportedFunctionLinkNames_.count(varDecl->linkSymbol) != 0)
                            {
                                reportDuplicateExportSymbol(varDecl->linkSymbol);
                            }
                            exportedVarLinkNames_.insert(varDecl->linkSymbol);
                        }
                        sirGlobal->isVolatile = varDecl->isVolatile; // @متطاير
                        // (AR) اللبنة 3.16: مصفوفة تخزين ساكن مصفَّرة ⇒ [N x i8] zeroinitializer
                        //      في .bss. لا مُهيّئ (فلا تمرّ بمسار __sad_main). N موجب (SEM023).
                        if (varDecl->isStaticArray)
                        {
                            sirGlobal->isZeroArray = true;
                            sirGlobal->zeroArrayCount = varDecl->staticArrayCount;
                            // (AR) حجم صفر يُرفَض في المحلّل (SEM023، عند parseVarDecl) — الحجم
                            //      حرفيّ دائمًا فلا حاجة لتكرار الإبلاغ هنا (رصد أميليا 3.16).
                        }

                        // (AR) اللبنة 3.14: مُهيّئ بايتات(...) ⇒ كتلة بايتات ثابتة في
                        //      .rodata برمز مملوك (لجداول النواة: أرشيف/خرائط). يُصدَر
                        //      ConstantDataArray في الخلفيّة. القيَم أعداد حرفيّة 0–255.
                        bool handledByteBlob = false;
                        if (varDecl->initializer)
                        {
                            if (auto *callExpr = dynamic_cast<Sad::AST::CallExpr *>(varDecl->initializer.get()))
                            {
                                auto *callee = dynamic_cast<Sad::AST::VariableExpr *>(callExpr->callee.get());
                                if (callee && callee->name == kByteBlobBuiltin)
                                {
                                    handledByteBlob = true;
                                    sirGlobal->isByteBlob = true;
                                    // (AR) C2: قائمة فارغة بايتات() ⇒ كتلة بلا معنى، ترفض
                                    if (callExpr->arguments.empty())
                                        errors_.push_back("Error: بايتات(...) لا يقبل قائمة فارغة — مرّر بايتًا واحدًا على الأقلّ (0–255)");
                                    for (const auto &arg : callExpr->arguments)
                                    {
                                        auto *lit = dynamic_cast<Sad::AST::LiteralExpr *>(arg.get());
                                        if (!lit || lit->token.getType() != Lexer::TokenType::NUMBER_INTEGER)
                                        {
                                            errors_.push_back("Error: بايتات(...) يقبل أعدادًا صحيحة حرفيّة فقط (0–255)");
                                            break;
                                        }
                                        std::string bv = lit->token.getValue();
                                        int64_t bn = 0;
                                        try
                                        {
                                            if (bv.size() > 2 && bv[0] == '0' && (bv[1] == 'x' || bv[1] == 'X'))
                                                bn = static_cast<int64_t>(std::stoull(bv, nullptr, 16));
                                            else if (bv.size() > 2 && bv[0] == '0' && (bv[1] == 'o' || bv[1] == 'O'))
                                                bn = static_cast<int64_t>(std::stoull(bv.substr(2), nullptr, 8));
                                            else if (bv.size() > 2 && bv[0] == '0' && (bv[1] == 'b' || bv[1] == 'B'))
                                                bn = static_cast<int64_t>(std::stoull(bv.substr(2), nullptr, 2));
                                            else
                                                bn = std::stoll(bv);
                                        }
                                        catch (...)
                                        {
                                            errors_.push_back("Error: بايتات: تعذّر تحليل العدد '" + bv + "'");
                                            break;
                                        }
                                        if (bn < 0 || bn > 255)
                                        {
                                            errors_.push_back("Error: بايتات: قيمة خارج المدى [0,255]: " + bv);
                                            break;
                                        }
                                        sirGlobal->byteData.push_back(static_cast<uint8_t>(bn));
                                    }
                                }
                            }
                        }

                        // (AR) استخراج القيمة الأولية إذا كانت ثابتاً حرفياً
                        // (EN) Extract initial value if it's a literal constant
                        if (!handledByteBlob && varDecl->initializer)
                        {
                            if (auto *litExpr = dynamic_cast<Sad::AST::LiteralExpr *>(varDecl->initializer.get()))
                            {
                                const auto &token = litExpr->token;
                                std::string value = token.getValue();
                                Lexer::TokenType tokenType = token.getType();

                                if (tokenType == Lexer::TokenType::NUMBER_INTEGER)
                                {
                                    // (AR) تحويل الأعداد الست عشرية/الثمانية/الثنائية إلى عشرية
                                    // (EN) Normalize hex/octal/binary literals to decimal
                                    if (value.size() > 2 && value[0] == '0')
                                    {
                                        char prefix = value[1];
                                        if (prefix == 'x' || prefix == 'X')
                                        {
                                            value = std::to_string(static_cast<int64_t>(std::stoull(value, nullptr, 16)));
                                        }
                                        else if (prefix == 'o' || prefix == 'O')
                                        {
                                            value = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 8)));
                                        }
                                        else if (prefix == 'b' || prefix == 'B')
                                        {
                                            value = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 2)));
                                        }
                                    }
                                    sirGlobal->initialValue = value;
                                    // (AR) رفعُ عَلَمِ الوجود: المستهلِكون (اختيارُ المُهيِّئ، وحكمُ
                                    //      `constant`، وكبتُ الـSTORE) يحكمون بالعَلَمِ لا بفراغِ
                                    //      النصّ — وهذه المرحلةُ مُنتِجٌ لهم كـbuildGlobalVariable.
                                    // (EN) Raise the presence flag: consumers judge by the flag,
                                    //      not by emptiness; this phase is a producer for them too.
                                    sirGlobal->hasInitialValue = true;
                                    // ═══════════════════════════════════════════════
                                    // (AR) 🔑 وصنفُ القيمةِ معه — والدرسُ المُعاد: **العَلَمُ
                                    //      يلزمه كلُّ مُنتِجيه**. مُلِئ `initialValueKind` أوّلًا في
                                    //      `buildGlobalVariable` وحدَها، وهي مسارُ عوامِّ **فضاءِ
                                    //      الأسماء**؛ أمّا عامُّ المستوى الأعلى — وهو المقصودُ —
                                    //      فيُبنى هنا. فبُنِي المصرِّفُ نظيفًا واجتاز المراجعةَ
                                    //      و**بقي `ثابت أي س = 5` يطبع «لاشيء»**: رقعةٌ صحيحةٌ في
                                    //      مُنتِجٍ لا يمرّ به الكود.
                                    //      ⇒ ولا يُكتشف بقراءةِ المعمار: الدالّةُ اسمُها
                                    //      `buildGlobalVariable` فيبدو أنّها **هي** بابُ العوامّ.
                                    //      الحكمُ من المُخرَج: `@"..." = internal global %SadDyn
                                    //      zeroinitializer` بعد الرقعةِ يقول إنّ الرقعةَ لم تُنفَّذ.
                                    // (EN) The value kind with it — and the recurring lesson: a
                                    //      flag needs ALL of its producers. initialValueKind was
                                    //      first filled in buildGlobalVariable alone, which is the
                                    //      NAMESPACE-global path; top-level globals are built
                                    //      here. The compiler built clean, passed review, and
                                    //      `ثابت أي س = 5` still printed «لاشيء» — a correct patch
                                    //      in a producer the code never enters. Not visible from
                                    //      the architecture (the function is *named*
                                    //      buildGlobalVariable); only the emitted IR shows it.
                                    // ═══════════════════════════════════════════════
                                    sirGlobal->initialValueKind = SadTypeKind::Integer;
                                }
                                else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE)
                                {
                                    sirGlobal->initialValue = value;
                                    sirGlobal->hasInitialValue = true;
                                    sirGlobal->initialValueKind = SadTypeKind::Float;
                                }
                                else if (tokenType == Lexer::TokenType::LITERAL_TRUE)
                                {
                                    sirGlobal->initialValue = "1";
                                    sirGlobal->hasInitialValue = true;
                                    sirGlobal->initialValueKind = SadTypeKind::Boolean;
                                }
                                else if (tokenType == Lexer::TokenType::LITERAL_FALSE)
                                {
                                    sirGlobal->initialValue = "0";
                                    sirGlobal->hasInitialValue = true;
                                    sirGlobal->initialValueKind = SadTypeKind::Boolean;
                                }
                            }
                        }

                        module_->addGlobalVariable(sirGlobal);
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 1.7: استنتاج أنواع المعاملات من مواقع الاستدعاء
                // (EN) Phase 1.7: Infer parameter types from call sites
                // ═══════════════════════════════════════════════════════════════════
                // (AR) قبل بناء أجسام الدوال، نمسح كل استدعاءات الدوال في البرنامج
                //      لتحديث أنواع المعاملات غير المحددة (UNKNOWN → I64) إلى أنواعها الحقيقية
                //      بناءً على الوسائط الفعلية المُمررة في مواقع الاستدعاء
                // (EN) Before building function bodies, scan all function calls in the program
                //      to update untyped parameter types (UNKNOWN → I64) to their actual types
                //      based on the real arguments passed at call sites

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 1.65 (ISSUE-076/084): تسجيل مبكرٌ لأسماء حقول تعدادات ADT في
                //      adtEnumTable_ قبل Phase 1.7 (بناء التعدادات الكامل في المرحلة 2أ، متأخّرًا).
                //      يُمكّن inferExprType من تمييز الوصول المباشر `X.حقل` كحمولةٍ ديناميّة (Any)
                //      فيُرقَّى معامِلُ دالةٍ تتلقّاه ⇒ يُصلح `ط(ش.نق)`. المرحلة 2أ ستستبدله بالكامل.
                // (EN) Phase 1.65 (ISSUE-076/084): pre-register ADT enum field names into
                //      adtEnumTable_ before Phase 1.7 (the full enum build is in Phase 2أ, later).
                //      Lets inferExprType recognize direct access `X.f` as a dynamic (Any) payload,
                //      widening a function param that receives it ⇒ fixes `ط(ش.نق)`. Phase 2أ later
                //      overwrites this entry with the complete build.
                {
                    auto preRegisterEnum = [&](Sad::AST::EnumDecl *enumDecl)
                    {
                        // (AR) فقط تعدادات ADT (ذات حقول) — التعداد البسيط (ثوابت) لا يُسجَّل في
                        //      adtEnumTable_ (وإلّا عُومِل خطأً كـADT ⇒ انهيار مطابقة الحالة الوحدة).
                        // (EN) Only ADT enums (with fields) — a simple enum (constants) must not go
                        //      into adtEnumTable_ (else it is mistaken for an ADT ⇒ unit-variant
                        //      match crash).
                        if (!enumDecl || !enumDecl->isADT() || adtEnumTable_.count(enumDecl->name))
                            return;
                        ADTEnumInfo adtInfo;
                        adtInfo.name = enumDecl->name;
                        adtInfo.structName = enumDecl->name;
                        for (size_t i = 0; i < enumDecl->members.size(); ++i)
                        {
                            ADTVariantInfo variant;
                            variant.name = enumDecl->members[i].name;
                            variant.tag = static_cast<int64_t>(i);
                            variant.fields = enumDecl->members[i].fields;
                            variant.fieldTypes.assign(variant.fields.size(), SadTypeKind::Unknown);
                            adtInfo.variants.push_back(std::move(variant));
                        }
                        adtEnumTable_[enumDecl->name] = std::move(adtInfo);
                    };
                    for (const auto &stmt : *program)
                    {
                        if (!stmt)
                            continue;
                        if (auto *ed = dynamic_cast<Sad::AST::EnumDecl *>(stmt.get()))
                            preRegisterEnum(ed);
                        else if (auto *exp = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                            preRegisterEnum(dynamic_cast<Sad::AST::EnumDecl *>(exp->declaration.get()));
                        else if (auto *exps = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                            preRegisterEnum(dynamic_cast<Sad::AST::EnumDecl *>(exps->declaration.get()));
                    }
                }

                // (AR) قبل الطور 1.7: سجّل تواقيع دوالّ الوحدات المستوردة انتقائيًّا في
                //      functionTable_ كي يستنتج inferParamTypesFromCallSites أنواع معاملاتها
                //      غير المصرَّحة من مواقع النداء في هذه الوحدة (وإلّا بقيت Integer فطُبع
                //      الوسيط النصّيّ مشوَّهًا عبر حدّ الوحدة). الطور 2 يبنيها بالنوع المصحَّح.
                // (EN) Before Phase 1.7: register selectively-imported module function
                //      signatures in functionTable_ so inferParamTypesFromCallSites infers
                //      their undeclared param types from this module's call sites (otherwise
                //      they stay Integer and a string arg prints garbled across the module
                //      boundary). Phase 2 builds them with the corrected type.
                preRegisterImportedSignatures(program);

                inferParamTypesFromCallSites(program);

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 1.8: استنتاج أنواع حقول الأصناف من وسائط جديد() مبكراً
                //      نمسح كل تعبيرات NewExpr في البرنامج ونُطابق وسائطها مع حقول الصنف
                //      عبر paramToFieldMap_ — هذا يسمح لـ inferReturnTypeFromBody بمعرفة
                //      أن حقل "اسم" هو STRING قبل بناء أجسام الدوال
                // (EN) Phase 1.8: Early field type inference from NewExpr arguments
                //      Scan all NewExpr in program and match arguments with class fields
                //      via paramToFieldMap_ — this allows inferReturnTypeFromBody to know
                //      that field "اسم" is STRING before building function bodies
                // ═══════════════════════════════════════════════════════════════════
                {
                    // (AR) دالة تعاودية لمسح تعبيرات NewExpr في الجمل
                    // (EN) Recursive function to scan NewExpr in statements
                    std::function<void(const Sad::AST::Expression *)> scanExprForNewExpr;
                    std::function<void(const Sad::AST::Statement *)> scanStmtForNewExpr;

                    scanExprForNewExpr = [&](const Sad::AST::Expression *expr)
                    {
                        if (!expr)
                            return;
                        if (auto *newExpr = dynamic_cast<const Sad::AST::NewExpr *>(expr))
                        {
                            auto sirClass = module_->getClass(newExpr->className);
                            if (sirClass && !sirClass->paramToFieldMap_.empty())
                            {
                                // (AR) البحث عن الباني للحصول على أسماء المعاملات
                                // (EN) Find constructor to get parameter names
                                std::string ctorName = newExpr->className + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                                auto ctorFunc = module_->getFunction(ctorName);
                                if (ctorFunc)
                                {
                                    const auto &params = ctorFunc->getParameters();
                                    // params[0] = self, params[1..N] = user params
                                    for (size_t i = 1; i < params.size() && (i - 1) < newExpr->arguments.size(); i++)
                                    {
                                        const std::string &paramName = params[i].name;
                                        auto fieldIt = sirClass->paramToFieldMap_.find(paramName);
                                        if (fieldIt != sirClass->paramToFieldMap_.end())
                                        {
                                            const std::string &fieldName = fieldIt->second;
                                            auto currentType = sirClass->fields_.find(fieldName);
                                            if (currentType != sirClass->fields_.end() &&
                                                currentType->second == SadTypeKind::Pointer)
                                            {
                                                // (AR) استنتاج نوع الوسيط من AST
                                                // (EN) Infer argument type from AST
                                                const auto &arg = newExpr->arguments[i - 1];
                                                SadTypeKind argType = SadTypeKind::Pointer;
                                                if (auto *lit = dynamic_cast<const Sad::AST::LiteralExpr *>(arg.get()))
                                                {
                                                    auto tt = lit->token.getType();
                                                    if (tt == Lexer::TokenType::STRING_LITERAL)
                                                        argType = SadTypeKind::String;
                                                    else if (tt == Lexer::TokenType::NUMBER_INTEGER)
                                                        argType = SadTypeKind::Integer;
                                                    else if (tt == Lexer::TokenType::NUMBER_DOUBLE)
                                                        argType = SadTypeKind::Float;
                                                    else if (tt == Lexer::TokenType::LITERAL_TRUE || tt == Lexer::TokenType::LITERAL_FALSE)
                                                        argType = SadTypeKind::Boolean;
                                                }
                                                else if (dynamic_cast<const Sad::AST::ArrayExpr *>(arg.get()))
                                                {
                                                    argType = SadTypeKind::Array;
                                                }
                                                else if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(arg.get()))
                                                {
                                                    // (AR) متغير — نبحث عن نوعه في الجدول
                                                    // (EN) Variable — look up its type
                                                    auto *varInfo = lookupVariable(varExpr->name);
                                                    if (varInfo && varInfo->type != SadTypeKind::Void)
                                                        argType = varInfo->type;
                                                }

                                                if (argType != SadTypeKind::Pointer && argType != SadTypeKind::Void)
                                                {
                                                    sirClass->fields_[fieldName] = argType;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        // (AR) مسح التعبيرات الفرعية
                        if (auto *call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                        {
                            scanExprForNewExpr(call->callee.get());
                            for (const auto &arg : call->arguments)
                                scanExprForNewExpr(arg.get());
                        }
                        else if (auto *bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                        {
                            scanExprForNewExpr(bin->left.get());
                            scanExprForNewExpr(bin->right.get());
                        }
                        else if (auto *assign = dynamic_cast<const Sad::AST::AssignExpr *>(expr))
                        {
                            scanExprForNewExpr(assign->value.get());
                        }
                        // (AR) [ISSUE-050b] التعمّق في نداء الطريقة/الوصول للعضو كي نبلغ
                        //      بناءً متداخلًا مثل «م(د()).ق()» (كان المسح لا يدخلها فلا
                        //      يُحلَّل تركيب «م» إطلاقًا).
                        // (EN) [ISSUE-050b] Recurse into method-call/member access so a nested
                        //      construction like `م(د()).ق()` is reached (the scan previously
                        //      skipped these, so `م`'s construction was never analyzed).
                        else if (auto *mc = dynamic_cast<const Sad::AST::MethodCallExpr *>(expr))
                        {
                            scanExprForNewExpr(mc->object.get());
                            for (const auto &arg : mc->arguments)
                                scanExprForNewExpr(arg.get());
                        }
                        else if (auto *me = dynamic_cast<const Sad::AST::MemberExpr *>(expr))
                        {
                            scanExprForNewExpr(me->object.get());
                        }
                        else if (auto *mae = dynamic_cast<const Sad::AST::MemberAccessExpr *>(expr))
                        {
                            scanExprForNewExpr(mae->object.get());
                        }
                    };

                    scanStmtForNewExpr = [&](const Sad::AST::Statement *stmt)
                    {
                        if (!stmt)
                            return;
                        if (auto *varDecl = dynamic_cast<const Sad::AST::VarDeclStmt *>(stmt))
                        {
                            if (varDecl->initializer)
                                scanExprForNewExpr(varDecl->initializer.get());
                        }
                        else if (auto *exprStmt = dynamic_cast<const Sad::AST::ExprStmt *>(stmt))
                        {
                            if (exprStmt->expression)
                                scanExprForNewExpr(exprStmt->expression.get());
                        }
                        else if (auto *block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                        {
                            for (const auto &s : block->statements)
                                scanStmtForNewExpr(s.get());
                        }
                        else if (auto *ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                        {
                            scanStmtForNewExpr(ifStmt->thenBranch.get());
                            if (ifStmt->elseBranch)
                                scanStmtForNewExpr(ifStmt->elseBranch.get());
                        }
                        else if (auto *whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                        {
                            scanStmtForNewExpr(whileStmt->body.get());
                        }
                        else if (auto *forStmt = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                        {
                            scanStmtForNewExpr(forStmt->body.get());
                        }
                    };

                    for (const auto &stmt : *program)
                    {
                        if (stmt)
                            scanStmtForNewExpr(stmt.get());
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة الثانية: بناء التصريحات وجمع الجمل التنفيذية
                // (EN) Phase 2: Build declarations and collect executable statements
                // ═══════════════════════════════════════════════════════════════════
                // (AR) جمع الجمل التنفيذية في المستوى الأعلى لتضمينها في __sad_main
                // (EN) Collect top-level executable statements for __sad_main wrapper
                std::vector<Sad::AST::Statement *> topLevelStatements;
                bool hasMainFunction = false; // (AR) هل توجد دالة "رئيسية" أو "main"؟

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 2-قبل: تصريحاتُ المستوى الأعلى (`ثابت`/`متغير`) تصير
                //      **متغيّراتٍ عامّة** حين توجد `رئيسية` صريحة.
                //
                //      العلّة: حين توجد `رئيسية`، كانت جملُ المستوى الأعلى تُجمَع في
                //      `topLevelStatements` ثمّ **تُهمَل** — لأنّ `__sad_main` لا يُنشَأ
                //      إلّا عند غيابها. فالثابتُ لا يُهيَّأ أبدًا: النصُّ يُطبَع «void»
                //      (مخرَجٌ خاطئٌ صامت — أخطرُ من الانهيار) والمصفوفةُ تنهار بـsegfault.
                //      والمفسّر سليمٌ في الحالتين ⇒ انحرافُ تكافؤ.
                //
                //      الإصلاح على مرحلتين: نُصرّح بها **هنا** — قبل بناء أيّ دالّة —
                //      كي تُحَلّ مراجعُها داخل `رئيسية` وغيرِها إلى الرمز العامّ لا إلى
                //      محلّيٍّ مفقود؛ ثمّ تُهيَّأ في `__sad_main` الذي يُنادى
                //      في مستهلّ `رئيسية` (انظر المرحلة الثالثة أدناه).
                //
                //      حين لا توجد `رئيسية` يبقى المسارُ القديم كما هو: الجملُ كلُّها
                //      داخل `__sad_main` بنطاقه الخاصّ — وهو يعمل، فلا نمسّه.
                // (EN) Phase 2-pre: top-level `const`/`var` declarations become module
                //      GLOBALS when an explicit main exists. Previously they were
                //      collected and then dropped (the `__sad_main` wrapper is only
                //      built when main is absent), so the constant was never
                //      initialised: strings printed "void" (a silent wrong answer) and
                //      arrays segfaulted. The interpreter is correct in both ⇒ parity
                //      divergence. Declared here, before any function is built, so
                //      references inside main resolve to the global symbol; initialised
                //      by `__sad_main`, called at main's entry (Phase 3).
                //      With no main the old path is untouched.
                // ═══════════════════════════════════════════════════════════════════
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;
                    if (auto *fd = dynamic_cast<Sad::AST::FunctionDecl *>(stmt.get()))
                    {
                        if (fd->name == "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9" ||
                            fd->name == "main")
                        {
                            hasMainFunction = true;
                            break;
                        }
                    }
                }
                const bool globalsFromTopLevel = hasMainFunction && !moduleMode_;
                if (globalsFromTopLevel)
                {
                    for (const auto &stmt : *program)
                    {
                        if (!stmt)
                            continue;
                        if (auto *varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(stmt.get()))
                        {
                            buildGlobalVariable(varDecl);
                        }
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 2أ: بناء الأصناف والتعدادات والسمات أولاً
                //      هذا ضروري حتى تتوفر معلومات الحقول والبانيات
                //      قبل بناء الدوال التي قد تحتاج inferReturnTypeFromBody
                // (EN) Phase 2A: Build classes, enums, traits FIRST
                //      This ensures field info and constructors are available
                //      before building functions that may need inferReturnTypeFromBody
                // ═══════════════════════════════════════════════════════════════════
                // ═══════════════════════════════════════════════════════════════════════
                // (AR) [ISSUE-049] المرحلة 2A-0: سجّل البُنى (struct) قبل بناء أجسام طرق
                //      الأصناف. وإلا فإنّ بناء بنية داخل طريقة صنف «ن()» لا يجد نوعها
                //      (getClass=null أثناء بناء الطريقة في Phase 2A) فيُصدَر نداء دالة غير
                //      معرَّف @"ن" ⇒ فشل ربط. (البناء في دالة حرّة/المستوى الأعلى كان يعمل
                //      لأنه يجري بعد تسجيل البنية في Phase 2 لاحقًا.) نبنيها هنا مرّة واحدة،
                //      ونتخطّاها في حلقة Phase 2 اللاحقة لتفادي التسجيل المزدوج.
                // (EN) [ISSUE-049] Phase 2A-0: register structs BEFORE building class method
                //      bodies. Otherwise constructing a struct inside a method (ن()) can't
                //      resolve its type (getClass=null during Phase 2A method build) and an
                //      undefined function call @"ن" is emitted ⇒ link failure. (Free-function
                //      and top-level construction worked because they run after the struct is
                //      registered in the later Phase 2.) Build once here, skip in Phase 2.
                // ═══════════════════════════════════════════════════════════════════════
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;
                    Sad::AST::StructDecl *structDecl = dynamic_cast<Sad::AST::StructDecl *>(stmt.get());
                    // (AR) [ISSUE-051] بنية مُصدَّرة «صدّر بنية ن»: فُكّ ExportDecl/ExportStmt لتُسجَّل
                    //      هنا (مبكرًا) أيضًا، وإلّا تُسجَّل متأخّرًا فيعود عرض 049 (نداء @"ن" غير معرَّف
                    //      عند بنائها داخل تابع صنف ⇒ فشل ربط).
                    // (EN) [ISSUE-051] Exported struct «export struct ن»: unwrap ExportDecl/ExportStmt
                    //      so it is pre-registered here too; otherwise it registers late and ISSUE-049
                    //      reappears (undefined @"ن" when constructed inside a class method ⇒ link fail).
                    if (!structDecl)
                    {
                        if (auto ed = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                            structDecl = ed->declaration ? dynamic_cast<Sad::AST::StructDecl *>(ed->declaration.get()) : nullptr;
                        else if (auto es = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                            structDecl = es->declaration ? dynamic_cast<Sad::AST::StructDecl *>(es->declaration.get()) : nullptr;
                    }
                    if (structDecl)
                    {
                        buildStatement(structDecl);
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] Phase2A-0: pre-registered struct '"
                                  << structDecl->name << "' before class method bodies" << std::endl;
#endif
                    }
                }

                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    if (auto classDecl = dynamic_cast<Sad::AST::ClassDecl *>(stmt.get()))
                    {
                        buildClass(classDecl);
                        continue;
                    }

                    if (auto enumDecl = dynamic_cast<Sad::AST::EnumDecl *>(stmt.get()))
                    {
                        buildStatement(enumDecl);
                        continue;
                    }

                    if (auto traitDecl = dynamic_cast<Sad::AST::TraitDecl *>(stmt.get()))
                    {
                        buildTrait(traitDecl);
                        continue;
                    }

                    if (auto implDecl = dynamic_cast<Sad::AST::ImplDecl *>(stmt.get()))
                    {
                        buildImpl(implDecl);
                        continue;
                    }

                    // (AR) [ISSUE-051] تعداد مُصدَّر «صدّر تعداد ل»: فُكّ ExportDecl/ExportStmt وابنِه
                    //      مبكرًا كنظيره العاريّ (تسجيل ثوابت التعداد قبل أجسام التوابع)؛ يُتخطّى لاحقًا.
                    // (EN) [ISSUE-051] Exported enum «export enum ل»: unwrap ExportDecl/ExportStmt and
                    //      build it early like its bare counterpart (register enum constants before
                    //      method bodies); skipped later in Phase 2C.
                    {
                        Sad::AST::Statement *inner = nullptr;
                        if (auto ed = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                            inner = ed->declaration.get();
                        else if (auto es = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                            inner = es->declaration.get();
                        if (inner)
                        {
                            if (auto enumDecl = dynamic_cast<Sad::AST::EnumDecl *>(inner))
                            {
                                buildStatement(enumDecl);
                                continue;
                            }
                        }
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 2ب: مسح NewExpr لتحديث أنواع حقول الأصناف
                //      الآن بعد بناء الأصناف، نمسح جميع تعبيرات جديد() في البرنامج
                //      ونستنتج أنواع الحقول من الوسائط الفعلية
                //      كذلك نُحدّث أنواع معاملات البانيات في functionTable_
                // (EN) Phase 2B: Scan NewExpr to update class field types
                //      Now that classes are built, scan all NewExpr in program
                //      and infer field types from actual arguments
                //      Also update constructor param types in functionTable_
                // ═══════════════════════════════════════════════════════════════════
                {
                    std::function<void(const Sad::AST::Expression *)> scanExprForNewExpr;
                    std::function<void(const Sad::AST::Statement *)> scanStmtForNewExpr;

                    scanExprForNewExpr = [&](const Sad::AST::Expression *expr)
                    {
                        if (!expr)
                            return;
                        if (auto *newExpr = dynamic_cast<const Sad::AST::NewExpr *>(expr))
                        {
                            auto sirClass = module_->getClass(newExpr->className);
                            if (sirClass && !sirClass->paramToFieldMap_.empty())
                            {
                                std::string ctorName = newExpr->className + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                                auto ctorFunc = module_->getFunction(ctorName);
                                auto ctorTableIt = functionTable_.find(ctorName);
                                if (ctorFunc)
                                {
                                    const auto &params = ctorFunc->getParameters();
                                    for (size_t i = 1; i < params.size() && (i - 1) < newExpr->arguments.size(); i++)
                                    {
                                        const std::string &paramName = params[i].name;
                                        auto fieldIt = sirClass->paramToFieldMap_.find(paramName);
                                        if (fieldIt != sirClass->paramToFieldMap_.end())
                                        {
                                            const std::string &fieldName = fieldIt->second;
                                            auto currentType = sirClass->fields_.find(fieldName);
                                            if (currentType != sirClass->fields_.end() &&
                                                currentType->second == SadTypeKind::Pointer)
                                            {
                                                const auto &arg = newExpr->arguments[i - 1];
                                                SadTypeKind argType = SadTypeKind::Pointer;
                                                if (auto *lit = dynamic_cast<const Sad::AST::LiteralExpr *>(arg.get()))
                                                {
                                                    auto tt = lit->token.getType();
                                                    if (tt == Lexer::TokenType::STRING_LITERAL)
                                                        argType = SadTypeKind::String;
                                                    else if (tt == Lexer::TokenType::NUMBER_INTEGER)
                                                        argType = SadTypeKind::Integer;
                                                    else if (tt == Lexer::TokenType::NUMBER_DOUBLE)
                                                        argType = SadTypeKind::Float;
                                                    else if (tt == Lexer::TokenType::LITERAL_TRUE || tt == Lexer::TokenType::LITERAL_FALSE)
                                                        argType = SadTypeKind::Boolean;
                                                }
                                                else if (dynamic_cast<const Sad::AST::ArrayExpr *>(arg.get()))
                                                {
                                                    argType = SadTypeKind::Array;
                                                }

                                                if (argType != SadTypeKind::Pointer && argType != SadTypeKind::Void)
                                                {
                                                    sirClass->fields_[fieldName] = argType;
                                                    // (AR) تحديث أيضاً functionTable_ للباني
                                                    if (ctorTableIt != functionTable_.end() && i < ctorTableIt->second.parameters.size())
                                                    {
                                                        if (ctorTableIt->second.parameters[i].type == SadTypeKind::Integer)
                                                            ctorTableIt->second.parameters[i].type = argType;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (auto *call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                        {
                            // (EN) Fix: Handle constructor calls via CallExpr (without "new" keyword)
                            //      In Sad, ClassName("arg") is parsed as CallExpr not NewExpr
                            //      Without this: field types don't get updated from implicit ctor args
                            if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(call->callee.get()))
                            {
                                auto callSirClass = module_->getClass(varExpr->name);
                                if (callSirClass && (!callSirClass->paramToFieldMap_.empty() ||
                                                     !callSirClass->fieldFromParamMember_.empty()))
                                {
                                    std::string ctorCallName = varExpr->name + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                                    // (AR) نبحث في functionTable_ (مسجّل في Phase 1.35) بدلاً من module_
                                    //      لأن الباني لم يُضف إلى module_ بعد (يحدث في Phase 2)
                                    // (EN) Look up in functionTable_ (registered in Phase 1.35) instead of module_
                                    //      because constructor isn't added to module_ until Phase 2
                                    auto ctorIt = functionTable_.find(ctorCallName);
                                    if (ctorIt != functionTable_.end())
                                    {
                                        const auto &ctorParams = ctorIt->second.parameters;

                                        // (AR) [ISSUE-050b] حلّ «هذا.حقل = معامِل.عضو» متعدّيًا: إن كان وسيط
                                        //      الموضع بنية/صنفًا معروفًا، نأخذ نوع الحقل من نوع عضو ذلك الصنف
                                        //      (المعامل كان غير مُنوَّع وقت بناء الباني فبقي الحقل Pointer⇒%s).
                                        // (EN) [ISSUE-050b] Resolve `this.field = param.member` transitively: if
                                        //      the positional arg is a known struct/class, take the field type
                                        //      from that class's member type (the param was untyped at ctor-build
                                        //      time, so the field stayed Pointer ⇒ %s on read).
                                        for (const auto &fpm : callSirClass->fieldFromParamMember_)
                                        {
                                            const std::string &fName = fpm.first;
                                            const std::string &pName = fpm.second.first;
                                            const std::string &mName = fpm.second.second;
                                            auto fTypeIt = callSirClass->fields_.find(fName);
                                            if (fTypeIt == callSirClass->fields_.end() ||
                                                fTypeIt->second != SadTypeKind::Pointer)
                                                continue;
                                            for (size_t ci = 1; ci < ctorParams.size() && (ci - 1) < call->arguments.size(); ci++)
                                            {
                                                if (ctorParams[ci].name != pName)
                                                    continue;
                                                const auto &cArg = call->arguments[ci - 1];
                                                std::string argClassName;
                                                if (auto *ne = dynamic_cast<const Sad::AST::NewExpr *>(cArg.get()))
                                                    argClassName = ne->className;
                                                else if (auto *ce = dynamic_cast<const Sad::AST::CallExpr *>(cArg.get()))
                                                {
                                                    if (auto *ve = dynamic_cast<const Sad::AST::VariableExpr *>(ce->callee.get()))
                                                        argClassName = ve->name;
                                                }
                                                if (argClassName.empty())
                                                    continue;
                                                auto argClass = module_->getClass(argClassName);
                                                if (!argClass)
                                                    continue;
                                                auto mIt = argClass->fields_.find(mName);
                                                if (mIt != argClass->fields_.end() &&
                                                    mIt->second != SadTypeKind::Pointer &&
                                                    mIt->second != SadTypeKind::Void)
                                                {
                                                    callSirClass->fields_[fName] = mIt->second;
                                                }
                                            }
                                        }

                                        for (size_t ci = 1; ci < ctorParams.size() && (ci - 1) < call->arguments.size(); ci++)
                                        {
                                            const std::string &cpName = ctorParams[ci].name;
                                            auto cfIt = callSirClass->paramToFieldMap_.find(cpName);
                                            if (cfIt != callSirClass->paramToFieldMap_.end())
                                            {
                                                const std::string &cfName = cfIt->second;
                                                auto cfType = callSirClass->fields_.find(cfName);
                                                if (cfType != callSirClass->fields_.end() &&
                                                    cfType->second == SadTypeKind::Pointer)
                                                {
                                                    const auto &cArg = call->arguments[ci - 1];
                                                    SadTypeKind cArgType = SadTypeKind::Pointer;
                                                    if (auto *lit = dynamic_cast<const Sad::AST::LiteralExpr *>(cArg.get()))
                                                    {
                                                        auto tt = lit->token.getType();
                                                        if (tt == Lexer::TokenType::STRING_LITERAL)
                                                            cArgType = SadTypeKind::String;
                                                        else if (tt == Lexer::TokenType::NUMBER_INTEGER)
                                                            cArgType = SadTypeKind::Integer;
                                                        else if (tt == Lexer::TokenType::NUMBER_DOUBLE)
                                                            cArgType = SadTypeKind::Float;
                                                        else if (tt == Lexer::TokenType::LITERAL_TRUE || tt == Lexer::TokenType::LITERAL_FALSE)
                                                            cArgType = SadTypeKind::Boolean;
                                                    }
                                                    else if (dynamic_cast<const Sad::AST::ArrayExpr *>(cArg.get()))
                                                    {
                                                        cArgType = SadTypeKind::Array;
                                                    }
                                                    else if (auto *varArg = dynamic_cast<const Sad::AST::VariableExpr *>(cArg.get()))
                                                    {
                                                        auto *varInfo = lookupVariable(varArg->name);
                                                        if (varInfo && varInfo->type != SadTypeKind::Void)
                                                            cArgType = varInfo->type;
                                                    }
                                                    if (cArgType != SadTypeKind::Pointer && cArgType != SadTypeKind::Void)
                                                    {
                                                        callSirClass->fields_[cfName] = cArgType;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            scanExprForNewExpr(call->callee.get());
                            for (const auto &arg : call->arguments)
                                scanExprForNewExpr(arg.get());
                        }
                        else if (auto *bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                        {
                            scanExprForNewExpr(bin->left.get());
                            scanExprForNewExpr(bin->right.get());
                        }
                        else if (auto *assign = dynamic_cast<const Sad::AST::AssignExpr *>(expr))
                        {
                            scanExprForNewExpr(assign->value.get());
                        }
                        // (AR) [ISSUE-050b] التعمّق في نداء الطريقة/الوصول للعضو كي نبلغ
                        //      بناءً متداخلًا مثل «م(د()).ق()» (كان المسح لا يدخلها فلا
                        //      يُحلَّل تركيب «م» إطلاقًا).
                        // (EN) [ISSUE-050b] Recurse into method-call/member access so a nested
                        //      construction like `م(د()).ق()` is reached (the scan previously
                        //      skipped these, so `م`'s construction was never analyzed).
                        else if (auto *mc = dynamic_cast<const Sad::AST::MethodCallExpr *>(expr))
                        {
                            scanExprForNewExpr(mc->object.get());
                            for (const auto &arg : mc->arguments)
                                scanExprForNewExpr(arg.get());
                        }
                        else if (auto *me = dynamic_cast<const Sad::AST::MemberExpr *>(expr))
                        {
                            scanExprForNewExpr(me->object.get());
                        }
                        else if (auto *mae = dynamic_cast<const Sad::AST::MemberAccessExpr *>(expr))
                        {
                            scanExprForNewExpr(mae->object.get());
                        }
                    };

                    scanStmtForNewExpr = [&](const Sad::AST::Statement *stmt)
                    {
                        if (!stmt)
                            return;
                        if (auto *varDecl = dynamic_cast<const Sad::AST::VarDeclStmt *>(stmt))
                        {
                            if (varDecl->initializer)
                                scanExprForNewExpr(varDecl->initializer.get());
                        }
                        else if (auto *exprStmt = dynamic_cast<const Sad::AST::ExprStmt *>(stmt))
                        {
                            if (exprStmt->expression)
                                scanExprForNewExpr(exprStmt->expression.get());
                        }
                        else if (auto *block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                        {
                            for (const auto &s : block->statements)
                                scanStmtForNewExpr(s.get());
                        }
                        else if (auto *ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                        {
                            scanStmtForNewExpr(ifStmt->thenBranch.get());
                            if (ifStmt->elseBranch)
                                scanStmtForNewExpr(ifStmt->elseBranch.get());
                        }
                        else if (auto *whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                        {
                            scanStmtForNewExpr(whileStmt->body.get());
                        }
                        else if (auto *forStmt = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                        {
                            scanStmtForNewExpr(forStmt->body.get());
                        }
                    };

                    for (const auto &stmt : *program)
                    {
                        if (stmt)
                            scanStmtForNewExpr(stmt.get());
                    }
                }

                // ═══════════════════════════════════════════════════════════════════════
                // (AR) المرحلة 2B2: نشر أنواع الحقول الموروثة من الأب إلى الابن
                //      المشكلة: عند نسخ حقول الأب في Phase 2A، قد تكون أنواعها لم تُحدَّث بعد
                //      (ستُحدَّث لاحقاً في Phase 2B). الحل: بعد Phase 2B، ننشر أنواع الحقول
                //      المُحدَّثة من الأب إلى جميع الأبناء عبر سلسلة الوراثة.
                // (EN) Phase 2B2: Propagate inherited field types from parent to child classes
                //      Problem: when copying parent fields in Phase 2A, their types may not be
                //      updated yet (Phase 2B updates them later). Fix: after Phase 2B, propagate
                //      updated field types from parent to all children down the inheritance chain.
                // ═══════════════════════════════════════════════════════════════════════
                {
                    const auto &allClasses = module_->getClasses();
                    // (AR) نمرر عدة مرات لضمان النشر في سلاسل الوراثة العميقة
                    //      (قاعدة → وسط → فرع): المرة الأولى تنشر من قاعدة لوسط،
                    //      المرة الثانية تنشر من وسط لفرع
                    // (EN) Multiple passes to handle deep chains (base->middle->child):
                    //      Pass 1 propagates base->middle, pass 2 propagates middle->child
                    for (size_t pass = 0; pass < allClasses.size(); pass++)
                    {
                        bool anyUpdated = false;
                        for (const auto &sirClass : allClasses)
                        {
                            if (sirClass->parentClass.empty())
                                continue;

                            auto parentSirClass = module_->getClass(sirClass->parentClass);
                            if (!parentSirClass)
                                continue;

                            // (AR) لكل حقل موروث: إذا كان الأب يملك نوعاً أدق، نحدّث الابن
                            // (EN) For each inherited field: if parent has more specific type, update child
                            for (const auto &fieldName : sirClass->fieldOrder_)
                            {
                                auto parentFieldIt = parentSirClass->fields_.find(fieldName);
                                if (parentFieldIt == parentSirClass->fields_.end())
                                    continue; // (AR) ليس حقلاً موروثاً / (EN) not an inherited field

                                auto childFieldIt = sirClass->fields_.find(fieldName);
                                if (childFieldIt == sirClass->fields_.end())
                                    continue;

                                // (AR) إذا الأب أدق (ليس Pointer) والابن لا يزال Pointer → حدّث
                                // (EN) If parent is more specific (not Pointer) and child is still Pointer -> update
                                if (parentFieldIt->second != SadTypeKind::Pointer &&
                                    childFieldIt->second == SadTypeKind::Pointer)
                                {
                                    sirClass->fields_[fieldName] = parentFieldIt->second;
                                    anyUpdated = true;
#ifndef NDEBUG
                                    std::cout << "[DEBUG] Phase2B2: propagated field '"
                                              << fieldName << "' type from parent '"
                                              << sirClass->parentClass << "' to child '"
                                              << sirClass->name << "'" << std::endl;
#endif
                                }
                            }
                        }
                        if (!anyUpdated)
                            break; // (AR) لا تحديثات — توقف مبكراً / (EN) No updates — early exit
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة 2ج: بناء الدوال والقوالب والصادرات + جمع الجمل التنفيذية
                // (EN) Phase 2C: Build functions, templates, exports + collect executable stmts
                // ═══════════════════════════════════════════════════════════════════

                // program هو StmtList = std::vector<StmtPtr> (ast_node.h:170)
                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    // (AR) الأصناف والتعدادات والسمات تم بناؤها في المرحلة 2أ — نتخطاها
                    // (EN) Classes, enums, traits were built in Phase 2A — skip them
                    if (dynamic_cast<Sad::AST::ClassDecl *>(stmt.get()))
                        continue;
                    if (dynamic_cast<Sad::AST::EnumDecl *>(stmt.get()))
                        continue;
                    if (dynamic_cast<Sad::AST::TraitDecl *>(stmt.get()))
                        continue;
                    if (dynamic_cast<Sad::AST::ImplDecl *>(stmt.get()))
                        continue;

                    // (AR) فضاء الأسماء: يُبنى في المرحلة 2 لأن أعضاءه (دوال/متغيرات/أصناف)
                    //      تعريفات على مستوى الوحدة وليست جملاً تنفيذية
                    // (EN) Namespace: build in Phase 2 because its members (functions/vars/classes)
                    //      are module-level declarations, not executable statements
                    if (auto nsDecl = dynamic_cast<Sad::AST::NamespaceDecl *>(stmt.get()))
                    {
                        buildStatement(nsDecl);
                        continue;
                    }

                    // (AR) دالة؟ (declarations.h:19 - class FunctionDecl)
                    // (EN) Function declaration?
                    if (auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(stmt.get()))
                    {
                        buildFunction(funcDecl);
                        // (AR) التحقق هل هي دالة رئيسية
                        if (funcDecl->name == "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9" || funcDecl->name == "main")
                        {
                            hasMainFunction = true;
                        }
                        continue;
                    }

                    // (AR) دالة قالب؟ (declarations.h:470 - class TemplateFunctionDecl)
                    // (EN) Template function declaration?
                    if (auto templateDecl = dynamic_cast<Sad::AST::TemplateFunctionDecl *>(stmt.get()))
                    {
                        buildTemplateFunction(templateDecl);
                        continue;
                    }

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) نظام الاستيراد والتصدير في المترجم
                    // (EN) Import/Export system in compiler
                    // ═══════════════════════════════════════════════════════════════

                    // (AR) تصدير (الإصدار الجديد): صدّر دالة|صنف|متغير ...
                    // (EN) Export declaration (new version): export function|class|var ...
                    if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                    {
                        if (exportDecl->declaration)
                        {
                            auto *innerStmt = exportDecl->declaration.get();
                            if (auto innerFunc = dynamic_cast<Sad::AST::FunctionDecl *>(innerStmt))
                            {
                                buildFunction(innerFunc);
                                // (AR) تعليم الدالة كمُصدّرة → ExternalLinkage في وضع الوحدة
                                // (EN) Mark function as exported → ExternalLinkage in module mode
                                if (currentFunction_)
                                {
                                    currentFunction_->isExported = true;
                                }
                                // (AR) البحث عن الدالة بالاسم في حال currentFunction_ تغيرت
                                auto exportedFn = module_->getFunction(innerFunc->name);
                                if (exportedFn)
                                {
                                    exportedFn->isExported = true;
                                }
                                if (innerFunc->name == "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9" || innerFunc->name == "main")
                                {
                                    hasMainFunction = true;
                                }
                            }
                            else if (auto innerClass = dynamic_cast<Sad::AST::ClassDecl *>(innerStmt))
                            {
                                buildClass(innerClass);
                            }
                            // (AR) [ISSUE-051] بنية/تعداد مُصدَّران: سُجِّلا مبكرًا كنوعَين (البنية في
                            //      المرحلة 2A-0، التعداد في 2A) بفكّ ExportDecl/ExportStmt. نتخطّاهما
                            //      هنا (لا buildStatement ولا دفع إلى else) تفاديًا للتسجيل المزدوج.
                            //      كانا قبل الإصلاح يسقطان في else فيُدفعان كجملة تنفيذ بلا تسجيل نوع
                            //      ⇒ `ن().ق` يفشل و`ل.ب`=0.
                            // (EN) [ISSUE-051] Exported struct/enum were registered early as TYPES
                            //      (struct in Phase 2A-0, enum in 2A) by unwrapping ExportDecl/
                            //      ExportStmt. Skip them here (no buildStatement, no push to else) to
                            //      avoid double registration. Before the fix they fell into else and
                            //      were pushed as runtime statements without type registration ⇒
                            //      `ن().ق` failed and `ل.ب`=0.
                            else if (dynamic_cast<Sad::AST::StructDecl *>(innerStmt) ||
                                     dynamic_cast<Sad::AST::EnumDecl *>(innerStmt))
                            {
                                // (AR) لا شيء — سُجِّلا مبكرًا / (EN) nothing — registered early
                            }
                            else
                            {
                                // (AR) أي تصدير آخر (متغير، إلخ) → جملة تنفيذية
                                topLevelStatements.push_back(stmt.get());
                            }
                        }
                        continue;
                    }

                    // (AR) تصدير (الإصدار القديم): صدّر ...
                    // (EN) Export statement (legacy): export ...
                    if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                    {
                        if (exportStmt->declaration)
                        {
                            auto *innerStmt = exportStmt->declaration.get();

                            if (auto innerFunc = dynamic_cast<Sad::AST::FunctionDecl *>(innerStmt))
                            {
                                buildFunction(innerFunc);
                                // (AR) تعليم الدالة كمُصدّرة
                                // (EN) Mark function as exported
                                auto exportedFn = module_->getFunction(innerFunc->name);
                                if (exportedFn)
                                {
                                    exportedFn->isExported = true;
                                }
                            }
                            else if (auto innerClass = dynamic_cast<Sad::AST::ClassDecl *>(innerStmt))
                            {
                                buildClass(innerClass);
                            }
                            // (AR) [ISSUE-051] بنية/تعداد مُصدَّران: سُجِّلا مبكرًا كنوعَين (البنية في
                            //      المرحلة 2A-0، التعداد في 2A) بفكّ ExportDecl/ExportStmt. نتخطّاهما
                            //      هنا (لا buildStatement ولا دفع إلى else) تفاديًا للتسجيل المزدوج.
                            //      كانا قبل الإصلاح يسقطان في else فيُدفعان كجملة تنفيذ بلا تسجيل نوع
                            //      ⇒ `ن().ق` يفشل و`ل.ب`=0.
                            // (EN) [ISSUE-051] Exported struct/enum were registered early as TYPES
                            //      (struct in Phase 2A-0, enum in 2A) by unwrapping ExportDecl/
                            //      ExportStmt. Skip them here (no buildStatement, no push to else) to
                            //      avoid double registration. Before the fix they fell into else and
                            //      were pushed as runtime statements without type registration ⇒
                            //      `ن().ق` failed and `ل.ب`=0.
                            else if (dynamic_cast<Sad::AST::StructDecl *>(innerStmt) ||
                                     dynamic_cast<Sad::AST::EnumDecl *>(innerStmt))
                            {
                                // (AR) لا شيء — سُجِّلا مبكرًا / (EN) nothing — registered early
                            }
                            else
                            {
                                topLevelStatements.push_back(stmt.get());
                            }
                        }
                        continue;
                    }

                    // (AR) استيراد انتقائي: من وحدة استورد ...
                    // (EN) Selective import: from module import ...
                    if (auto fromImport = dynamic_cast<Sad::AST::FromImportStmt *>(stmt.get()))
                    {
                        buildFromImportStmt(fromImport);
                        continue;
                    }

                    // (AR) استيراد كامل: استورد وحدة
                    // (EN) Full import: import module
                    if (auto importStmt = dynamic_cast<Sad::AST::ImportStmt *>(stmt.get()))
                    {
                        buildImportStmt(importStmt);
                        continue;
                    }

                    // (AR) إعادة التصدير: صدّر * من وحدة / صدّر عنصر1 من وحدة
                    // (EN) Re-export: export * from module / export item1 from module
                    if (auto reExportStmt = dynamic_cast<Sad::AST::ReExportStmt *>(stmt.get()))
                    {
                        buildStatement(reExportStmt);
                        continue;
                    }

                    // (AR) كتلة الامتداد: امتداد صنف ... نهاية
                    //      تُعالج في المرحلة 2 لأن الدوال المُضافة يجب أن تكون متاحة لجميع الدوال
                    // (EN) Extension block: extension class ... end
                    //      Processed in Phase 2 so added methods are available to all functions
                    if (auto extensionDecl = dynamic_cast<Sad::AST::ExtensionDecl *>(stmt.get()))
                    {
                        buildStatement(extensionDecl);
                        continue;
                    }

                    // (AR) اسم مستعار للنوع: نوع اسم = نوع_هدف
                    //      يُعالج في المرحلة 2 لأن الأسماء يجب أن تكون متاحة مبكراً
                    // (EN) Type alias: type name = target_type
                    //      Processed in Phase 2 so aliases are available early
                    if (auto typeAliasDecl = dynamic_cast<Sad::AST::TypeAliasDecl *>(stmt.get()))
                    {
                        buildStatement(typeAliasDecl);
                        continue;
                    }

                    // (AR) بنية: بنية اسم ... نهاية
                    //      يُعالج في المرحلة 2 مثل الأصناف
                    // (EN) Struct: struct name ... end
                    //      Processed in Phase 2 like classes
                    if (dynamic_cast<Sad::AST::StructDecl *>(stmt.get()))
                    {
                        // (AR) [ISSUE-049] البنية بُنِيت سلفًا في المرحلة 2A-0؛ نتخطّاها هنا
                        //      تفاديًا للتسجيل المزدوج (addClass مرّتين).
                        // (EN) [ISSUE-049] Struct already built in Phase 2A-0; skip here to
                        //      avoid double registration (addClass twice).
                        continue;
                    }

                    // (AR) اللبنة 3.14: تصريح «= بايتات(...)» عامّ مُعرَّف بالكامل كـ
                    //      global .rodata (Phase 1.5)؛ لا يُدفَع كجملة تنفيذيّة تفاديًا
                    //      لمعالجة «بايتات» كنداء دالّة غير معرّفة في __sad_main.
                    //      قيود موثَّقة (خارج نطاق نهلة): (C1) بايتات مع «صدّر» تُعالَج
                    //      مرّتين — نهلة تستعمل @رمز لا صدّر؛ (C3) بايتات كتعبير داخل
                    //      دالّة يُعطي «دالّة غير معرّفة»؛ (C4) قراءة الكتلة بالاسم مباشرةً
                    //      (لا عبر عنوان_رمز) مكسورة النوع — نهلة تقرأ بـعنوان_رمز+memread.
                    if (auto *vd = dynamic_cast<Sad::AST::VarDeclStmt *>(stmt.get()))
                    {
                        // (AR) اللبنة 3.16: مصفوفة ساكنة .bss عُرِّفت كـglobal أعلاه؛ لا تُدفَع
                        //      كجملة تنفيذيّة في __sad_main (نظير بايتات) — لا مُهيّئ لها.
                        if (vd->isStaticArray)
                            continue;
                        if (vd->initializer)
                        {
                            if (auto *ce = dynamic_cast<Sad::AST::CallExpr *>(vd->initializer.get()))
                            {
                                auto *cv = dynamic_cast<Sad::AST::VariableExpr *>(ce->callee.get());
                                if (cv && cv->name == kByteBlobBuiltin)
                                    continue;
                            }
                        }
                    }

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) جملة تنفيذية في المستوى الأعلى (طباعة، متغير، شرط، حلقة...)
                    // (EN) Top-level executable statement (print, var, if, while...)
                    // ═══════════════════════════════════════════════════════════════
                    topLevelStatements.push_back(stmt.get());
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) المرحلة الثالثة: إنشاء دالة __sad_main لتغليف الكود التنفيذي
                // (EN) Phase 3: Create __sad_main to wrap top-level executable code
                // ═══════════════════════════════════════════════════════════════════
                // (AR) إذا توجد جمل تنفيذية ولا توجد دالة رئيسية صريحة
                //      وليس في وضع الوحدة (--module)
                // (EN) If there are executable statements and no explicit main function
                //      and not in module mode (--module)
                // (AR) توجد `رئيسية` وجملٌ في المستوى الأعلى ⇒ ابنِ `__sad_main`
                //      يحمل تهيئةَ العوامّ المُصرَّح بها في المرحلة 2-قبل، وانْدُهْ به في
                //      مستهلّ `رئيسية` — قبل أوّل قراءةٍ لأيٍّ منها.
                //      بلا نطاقٍ جديد (`enterScope`) عمدًا: التصريحاتُ تُحَلّ إلى النطاق
                //      العامّ فتخزّن في الرمز العامّ بدل محلّيٍّ يُهمَل عند العودة.
                // (EN) Main exists and there is top-level code ⇒ build
                //      `__sad_main` holding the initialisation of the globals
                //      declared in Phase 2-pre, and call it at main's entry, before any
                //      of them can be read. Deliberately no new scope: the declarations
                //      resolve to the global scope and store into the global symbol
                //      instead of a local that dies on return.
                if (!topLevelStatements.empty() && globalsFromTopLevel)
                {
                    auto initFunc = std::make_shared<SIRFunction>("__sad_main",
                                                                  SadTypeKind::Void);
                    auto prevFunction = currentFunction_;
                    auto prevBlock = currentBlock_;
                    currentFunction_ = initFunc;

                    auto entryBlock = createBasicBlock(kEntryBlockName);
                    initFunc->addBasicBlock(entryBlock);
                    currentBlock_ = entryBlock;

                    for (auto *stmtNode : topLevelStatements)
                    {
                        if (stmtNode)
                        {
                            buildStatement(stmtNode);
                        }
                    }

                    if (currentBlock_)
                    {
                        bool hasTerminator = false;
                        if (!currentBlock_->instructions.empty())
                        {
                            const auto &lastInst = currentBlock_->instructions.back();
                            hasTerminator = (lastInst.opcode == SIROpcode::RET ||
                                             lastInst.opcode == SIROpcode::RET_VOID);
                        }
                        if (!hasTerminator)
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET_VOID;
                            currentBlock_->addInstruction(retInst);
                        }
                    }

                    module_->addFunction(initFunc);
                    {
                        FunctionInfo funcInfo;
                        funcInfo.name = "__sad_main";
                        funcInfo.returnType = SadTypeKind::Void;
                        funcInfo.sirFunction = initFunc;
                        functionTable_["__sad_main"] = funcInfo;
                    }

                    currentFunction_ = prevFunction;
                    currentBlock_ = prevBlock;

                    // (AR) احقن النداء في **مقدّمة** كتلة دخول `رئيسية` — لا في مؤخّرتها:
                    //      التهيئةُ يجب أن تسبق أوّل قراءة، وإلّا عاد العيبُ نفسه مؤجَّلًا.
                    // (EN) Inject the call at the FRONT of main's entry block — not the
                    //      back: initialisation must precede the first read.
                    auto mainFn = module_->getFunction(
                        "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9");
                    if (!mainFn)
                    {
                        mainFn = module_->getFunction("main");
                    }
                    if (mainFn && !mainFn->basicBlocks.empty())
                    {
                        auto &mainEntry = mainFn->basicBlocks.front();
                        if (mainEntry)
                        {
                            SIRInstruction callInst(SIROpcode::CALL);
                            callInst.operands.push_back(
                                SIROperand::Function("__sad_main"));
                            callInst.comment = "تهيئة عوامّ المستوى الأعلى قبل جسم رئيسية";
                            mainEntry->instructions.insert(mainEntry->instructions.begin(),
                                                           callInst);
                        }
                    }
                }

                if (!topLevelStatements.empty() && !hasMainFunction && !moduleMode_)
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildModule: creating __sad_main with "
                              << topLevelStatements.size() << " top-level statements" << std::endl;
#endif

                    // (AR) إنشاء دالة __sad_main من نوع void بدون معاملات
                    // (EN) Create __sad_main function: void __sad_main()
                    auto sadMainFunc = std::make_shared<SIRFunction>("__sad_main", SadTypeKind::Void);

                    // (AR) حفظ الحالة الحالية
                    // (EN) Save current state
                    auto prevFunction = currentFunction_;
                    auto prevBlock = currentBlock_;

                    currentFunction_ = sadMainFunc;

                    // (AR) إنشاء نطاق جديد للدالة الرئيسية
                    // (EN) Create new scope for main function
                    enterScope();

                    // (AR) إنشاء basic block للدخول
                    // (EN) Create entry basic block
                    auto entryBlock = createBasicBlock(kEntryBlockName);
                    sadMainFunc->addBasicBlock(entryBlock);
                    currentBlock_ = entryBlock;

                    // (AR) بناء جميع الجمل التنفيذية
                    // (EN) Build all executable statements
                    for (auto *stmtNode : topLevelStatements)
                    {
                        if (stmtNode)
                        {
                            buildStatement(stmtNode);
                        }
                    }

                    // (AR) الخروج من النطاق
                    // (EN) Exit scope
                    exitScope();

                    // (AR) التأكد من وجود terminator
                    // (EN) Ensure terminator exists
                    if (currentBlock_)
                    {
                        bool hasTerminator = false;
                        if (!currentBlock_->instructions.empty())
                        {
                            const auto &lastInst = currentBlock_->instructions.back();
                            hasTerminator = (lastInst.opcode == SIROpcode::RET ||
                                             lastInst.opcode == SIROpcode::RET_VOID);
                        }
                        if (!hasTerminator)
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET_VOID;
                            currentBlock_->addInstruction(retInst);
                        }
                    }

                    // (AR) إضافة الدالة للوحدة
                    // (EN) Add function to module
                    module_->addFunction(sadMainFunc);

                    // (AR) تسجيل في جدول الدوال
                    // (EN) Register in function table
                    {
                        FunctionInfo funcInfo;
                        funcInfo.name = "__sad_main";
                        funcInfo.returnType = SadTypeKind::Void;
                        funcInfo.sirFunction = sadMainFunc;
                        functionTable_["__sad_main"] = funcInfo;
                    }

                    // (AR) استعادة الحالة السابقة
                    // (EN) Restore previous state
                    currentFunction_ = prevFunction;
                    currentBlock_ = prevBlock;
                }

                // (AR) الخروج من النطاق العام الذي أنشأناه في المرحلة 1.5
                // (EN) Exit the global scope we created in Phase 1.5
                exitScope();

                return module_;
            }


            // ════════════════════════════════════════════════════════════════════
            // (AR) تسجيل تواقيع دوالّ قائمةِ عباراتٍ مسبقًا — تمريرةٌ واحدة يشترك فيها
            //      ملفُّ الدخول والوحداتُ المستوردة.
            //
            //      لماذا دالّةٌ لا حلقةٌ مكرَّرة: كانت هذه التمريرةُ محصورةً في مسار ملفّ
            //      الدخول، فبُنيت أجسامُ الوحدات المستوردة بترتيب السطور وحده. فنداءٌ
            //      أماميّ داخل وحدةٍ مستوردة — «أ» تنادي «ب» المعرَّفةَ بعدها — يفشل
            //      بـ«استدعاء دالة غير معرّفة»، بينما تُصرَّف الوحدةُ نفسُها بلا خطأٍ
            //      حين تكون ملفَّ الدخول. تباعدٌ في **مسار البناء** لا في الكود المكتوب،
            //      وهو ما جعل «الوحدة تُصرَّف وحدها ولا تُصرَّف حين تُستورَد»
            //      (sadlang/s-programming-language#333). ولأنّ العلّة تكرارُ منطقٍ في
            //      مسارَين، فالعلاجُ توحيدُ المنطق لا نسخُه ثالثةً.
            // (EN) Pre-register the function signatures of a statement list — one pass
            //      shared by the entry file and by imported modules. This pass used to
            //      exist only on the entry path, so imported module bodies were built in
            //      source order and a forward call inside an imported module failed with
            //      "undefined function" — while the very same file compiled cleanly as an
            //      entry file (#333). The cure is one shared pass, not a third copy.
            // ════════════════════════════════════════════════════════════════════
            void SIRBuilder::preRegisterFunctionSignatures(Sad::AST::StmtList *program)
            {
                if (!program)
                    return;

                for (const auto &stmt : *program)
                {
                    if (!stmt)
                        continue;

                    AST::FunctionDecl *funcDecl = nullptr;
                    // (AR) دالة عادية
                    if (auto fd = dynamic_cast<Sad::AST::FunctionDecl *>(stmt.get()))
                    {
                        funcDecl = fd;
                    }
                    // (AR) دالة مُصدَّرة (الإصدار الجديد)
                    else if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                    {
                        if (exportDecl->declaration)
                        {
                            funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(exportDecl->declaration.get());
                        }
                    }
                    // (AR) دالة مُصدَّرة (الإصدار القديم)
                    else if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                    {
                        if (exportStmt->declaration)
                        {
                            funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(exportStmt->declaration.get());
                        }
                    }

                    if (funcDecl)
                    {
                        // ────────────────────────────────────────────────────────
                        // (AR) توقيعٌ مُسجَّلٌ سلفًا لا يُدهَس.
                        //
                        //      حين تُستدعى هذه التمريرةُ للوحدة المستوردة (الطور 2)
                        //      يكون الطورُ 1.7 قد استنتج أنواعَ معاملاتها من مواقع
                        //      النداء — ومنها **نوعُ عنصر المصفوفة**. وإعادةُ الكتابة
                        //      بأنواعٍ خام تمحو ذلك الاستنتاج، فيعود متغيّرُ «لكل»
                        //      عددًا. والتسجيلُ هنا غرضُه سدُّ الغياب لا التصحيح،
                        //      فالموجودُ أدقُّ من الخام دائمًا.
                        //
                        //      ولا يضرّ التخطّي ملفَّ الدخول: جدولُه فارغٌ عند الطور 1.
                        // (EN) Never clobber an existing signature: by the time this
                        //      pass runs for an imported module (Phase 2), Phase 1.7 has
                        //      already inferred its parameter types — including array
                        //      ELEMENT types — from call sites. Overwriting with raw
                        //      declared types erases that and the `لكل` variable reverts
                        //      to an integer. This pass fills gaps; it never corrects.
                        //      Harmless for the entry file, whose table is empty at Phase 1.
                        // ────────────────────────────────────────────────────────
                        if (functionTable_.find(funcDecl->name) != functionTable_.end())
                            continue;

                        // (AR) تسجيل توقيع الدالة في الجدول
                        // (EN) Register function signature in function table
                        FunctionInfo funcInfo;
                        funcInfo.name = funcDecl->name;
                        // (AR) §9 الجذر1: استنتج نوع الإرجاع من الجسم عند غياب نوعٍ صريح —
                        //      نظير الدوال المتداخلة (statement_main.cpp:159-167) كي لا تبقى
                        //      دوالُّ المستوى الأعلى Void فتُهمَل قيمتها عند تمريرها وسيطًا.
                        if ((funcDecl->returnType == Types::SadTypeKind::Unknown ||
                             funcDecl->returnType == Types::SadTypeKind::Void) &&
                            funcDecl->body)
                        {
                            funcInfo.returnType =
                                inferReturnTypeFromBody(funcDecl->body.get(), funcDecl);
                        }
                        else if (funcDecl->returnType == Types::SadTypeKind::Unknown &&
                                 !funcDecl->body)
                        {
                            // (AR) دالّة بلا جسم (خارجي/FFI) بلا نوع إرجاع صريح ⇒ Void —
                            //      كان astTypeToSIRType(Unknown) يعيد Integer فيتباعد
                            //      functionTable_ ‏(i64) عن SIRFunction ‏(Void) ويُصدر
                            //      موضعُ النداء call i64 على declare void ‏(UB إن
                            //      استُهلكت القيمة). نوع الإرجاع الصريح يسلك else أدناه.
                            // (EN) Bodyless (extern/FFI) function with no explicit
                            //      return type ⇒ Void — astTypeToSIRType(Unknown)
                            //      returned Integer, splitting functionTable_ (i64)
                            //      from SIRFunction (Void) and emitting call i64 on a
                            //      declare void (UB if the value is consumed).
                            funcInfo.returnType = SadTypeKind::Void;
                        }
                        else
                        {
                            funcInfo.returnType = astTypeToSIRType(funcDecl->returnType);
                        }
                        for (const auto &param : funcDecl->parameters)
                        {
                            SadTypeKind paramType = astTypeToSIRType(param.type);
                            funcInfo.parameters.push_back(SIRParameter(param.name, paramType));
                        }
                        // ═══════════════════════════════════════════════════════════
                        // (AR) 🔑 صنفُ الإرجاعِ المُصرَّحُ معروفٌ **هنا**، وكان يُهمَل (ISSUE-140)
                        // ═══════════════════════════════════════════════════════════
                        //
                        // (AR) المقيس: `دالة شخص حر()` ثمّ `نص(حر().اسم)` يطبعُ **مؤشّرًا
                        //      خامًا** (`140696668676234`) بـrc=0 والمرجعُ «علي». والضابطُ أنّ الحقلَ
                        //      **العدديّ** عبرَ المسارِ نفسِه سليم ⇒ فالإزاحةُ صحيحةٌ والمفقودُ
                        //      **نوعُ الحقل**: يُقرأُ `Integer` فيُنسِّقُ `نص()` المؤشّرَ عددًا.
                        //
                        //      وموضِعُ الفجوةِ هنا: `buildExprMember` يستنبِطُ نوعَ الحقلِ من
                        //      `objResult.className`، وموضِعُ النداءِ يملأُ ذلك من
                        //      `functionTable_[fn].returnClassName` — وهذا الحقلُ لم يكن يُكتبُ
                        //      من التصريحِ قطُّ، بل من **استنتاجِ جملةِ `ارجع`** وحدَها.
                        //
                        //      🔑 والمعلومةُ كانت حاضرةً هنا منذُ البدءِ ومُهمَلةً: التصريحُ
                        //      يحملُ `returnType=Class` و`returnTypeName=شخص` (مقيس). وملؤها هنا
                        //      **مستقلٌّ عن ترتيبِ الأطوار**: التسجيلُ المُسبَقُ يسبقُ بناءَ أيِّ
                        //      جسمٍ، فلا يتوقّفُ الجوابُ على أيُّهما يُبنى أوّلًا — وهو عينُ ما
                        //      يُخفِقُ في الدالّةِ بلا نوعِ إرجاعٍ مُصرَّح (يبقى مفتوحًا).
                        //
                        //      ⚠️ ولا يُكتبُ إلّا لـ`Class`/`Struct`: اسمُ النوعِ موجودٌ لـ`نص` و`رقم`
                        //      أيضًا، وكتابتُه `returnClassName` تجعلُ موضِعَ النداءِ يُسجِّلُ النتيجةَ
                        //      كائنًا في `classInstanceTypes_` — فيُبحَثُ عن حقولٍ في «صنفٍ» اسمُه `نص`.
                        // (EN) The DECLARED return class is known right here and was ignored.
                        //      Measured: `func Person free()` then `str(free().name)` prints a raw
                        //      pointer with rc=0 where the reference prints the name — and the same
                        //      path with a NUMERIC field is correct, so the offset is right and what
                        //      is missing is the field TYPE: read as Integer, str() formats the
                        //      pointer as a number. buildExprMember derives the field type from
                        //      objResult.className, which the call site fills from
                        //      functionTable_[fn].returnClassName — a field only ever written by
                        //      inference over the `return` statement, never from the declaration.
                        //      Filling it here is phase-order independent: pre-registration runs
                        //      before any body is built, so the answer no longer depends on which
                        //      is built first — which is exactly what still fails for a function
                        //      with no declared return type (stays open).
                        //      Only Class/Struct: a type name also exists for `string` and `int`,
                        //      and writing those into returnClassName would make the call site
                        //      register the result as an object and look up fields in a «class»
                        //      named `string`.
                        // ═══════════════════════════════════════════════════════════
                        if ((funcDecl->returnType == Types::SadTypeKind::Class ||
                             funcDecl->returnType == Types::SadTypeKind::Struct) &&
                            !funcDecl->returnTypeName.empty())
                        {
                            funcInfo.returnClassName = funcDecl->returnTypeName;
                        }
                        // (AR) مؤشر الدالة سيُحدَّث لاحقاً في buildFunction
                        // (EN) sirFunction pointer will be updated later in buildFunction
                        funcInfo.sirFunction = nullptr;
                        // (AR) حفظ مرجع AST لمعالجة القيم الافتراضية عند الاستدعاء
                        // (EN) Save AST reference for default parameter values at call sites
                        funcInfo.astDecl = funcDecl;
                        functionTable_[funcDecl->name] = funcInfo;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] Phase1: registered func '" << funcDecl->name
                                  << "' retType=" << static_cast<int>(funcInfo.returnType)
                                  << " AST-retType=" << static_cast<int>(funcDecl->returnType) << std::endl;
#endif
                    }

                    // ─────────────────────────────────────────────────────────────
                    // (AR) تسجيل دوال الامتداد مسبقاً: امتداد نوع_هدف ... نهاية
                    //      تُسجّل كـ "نوع_هدف.اسم_الدالة" في جدول الدوال
                    //      هذا يضمن أن الدوال المُعرّفة قبل كتلة الامتداد يمكنها الوصول إليها
                    // (EN) Pre-register extension methods: extension target_type ... end
                    //      Registered as "target_type.method_name" in function table
                    // ─────────────────────────────────────────────────────────────
                    if (auto extensionDecl = dynamic_cast<Sad::AST::ExtensionDecl *>(stmt.get()))
                    {
                        for (auto &method : extensionDecl->methods)
                        {
                            if (!method)
                                continue;
                            auto extFunc = dynamic_cast<Sad::AST::FunctionDecl *>(method.get());
                            if (!extFunc)
                                continue;

                            std::string fullName = extensionDecl->targetType + "." + extFunc->name;
                            FunctionInfo extInfo;
                            extInfo.name = fullName;
                            extInfo.returnType = astTypeToSIRType(extFunc->returnType);
                            // (AR) المعامل الأول هو self
                            extInfo.parameters.push_back(SIRParameter(kSelfParamName, SadTypeKind::Integer));
                            for (const auto &param : extFunc->parameters)
                            {
                                SadTypeKind paramType = astTypeToSIRType(param.type);
                                extInfo.parameters.push_back(SIRParameter(param.name, paramType));
                            }
                            extInfo.sirFunction = nullptr;
                            extInfo.astDecl = extFunc;
                            functionTable_[fullName] = extInfo;
#ifdef SIR_BUILDER_DEBUG
                            std::cerr << "[SIR-DBG] Phase1: registered extension method '" << fullName << "'" << std::endl;
#endif
                        }
                    }
                }
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
