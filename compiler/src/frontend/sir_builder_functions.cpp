// ============================================================================
// sir_builder_functions.cpp - بناء الدوال والمتغيرات العامة
// ============================================================================
// (AR) هذا الملف يحتوي على:
//      - buildFunction: بناء دالة SIR كاملة من عقدة AST
//      - buildGlobalVariable: بناء متغير عام في وحدة SIR
// (EN) This file contains:
//      - buildFunction: Build a complete SIR function from AST node
//      - buildGlobalVariable: Build a global variable in SIR module
// ============================================================================
// تم استخراج هذا الملف من sir_builder_module.cpp وفقاً لقاعدة CW-05
// (حد أقصى 800 سطر لكل ملف)
// ============================================================================

#include <limits>
#include <sstream>
#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "statements.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
#include "error_manager.h"    // (AR) (3.17) buildBilingualMessage لبلاغ SEM024 من الكتالوج
#include "error_catalog.h"    // (AR) RenderContext (حاملُ placeholders)
#include "error_codes.h"      // (AR) ErrorCode::SEM_DUP_EXPORT_SYMBOL

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // ============================================================================
            // (AR) مسح ساكن: هل يحتاج جسم الدالّة آلة التنظيف (مكدّس التأجيل + setjmp)؟
            //      هذه الآلة تخدم جمل «أجّل» حصرًا (تشغيل المؤجَّلات عند انفلات استثناء).
            //      توليدها بلا شرط لكلّ دالّة كان يكلّف تخصيصين ديناميّين + ‏setjmp لكلّ
            //      نداء، ويجرّ رموز libc‏ (_setjmp/longjmp/malloc) في --freestanding.
            //      المولّدات (اسحب) وكتل «أطلق» ومطابقة الأنماط تُعامَل تحفّظيًّا
            //      كمحتاجة للآلة حفاظًا على سلوكها القائم.
            // (EN) Static scan: does the function body need the defer-cleanup machinery
            //      (defer stack + setjmp frame)? It only serves «أجّل» statements.
            //      Unconditional emission cost 2 heap allocs + setjmp per call and
            //      pulled libc symbols under --freestanding. Generators (yield),
            //      go-blocks, and match statements are conservatively kept.
            // ============================================================================
            static bool stmtNeedsDeferMachinery(const ::Sad::AST::Statement *stmt)
            {
                if (!stmt)
                    return false;
                if (dynamic_cast<const ::Sad::AST::DeferStmt *>(stmt))
                    return true;
                // (AR) تحفّظ: المولّدات/الإطلاق تُبقي الآلة (تستعمل زمنَ التشغيل فعلًا)
                if (dynamic_cast<const ::Sad::AST::YieldStmt *>(stmt))
                    return true;
                // (AR) المطابقة لا تحتاج منظومةَ أجّل بذاتها؛ تحتاجها فقط إن استعمل أحدُ أذرعها
                //      «أجّل»/إطلاقًا/محاولةً. نتعمّق في أجساد الأذرع بدل الإبقاء القسريّ — يفكّ
                //      ارتباطَ التعداد المُطابَق عن _setjmp/malloc (الغائبَين عن المعدن والبثّ
                //      السياديّ الأصليّ). يؤثّر في الوضع الحرّ فقط (المستضاف/المفسّر بلا مساس، سطر 577).
                if (auto *ms = dynamic_cast<const ::Sad::AST::MatchStmt *>(stmt))
                {
                    for (const auto &cc : ms->cases)
                        for (const auto &s : cc.body)
                            if (stmtNeedsDeferMachinery(s.get()))
                                return true;
                    return false;
                }
                if (auto *goStmt = dynamic_cast<const ::Sad::AST::GoStmt *>(stmt))
                {
                    (void)goStmt;
                    return true;
                }
                if (auto *blk = dynamic_cast<const ::Sad::AST::BlockStmt *>(stmt))
                {
                    for (const auto &s : blk->statements)
                        if (stmtNeedsDeferMachinery(s.get()))
                            return true;
                    return false;
                }
                if (auto *ifs = dynamic_cast<const ::Sad::AST::IfStmt *>(stmt))
                    return stmtNeedsDeferMachinery(ifs->thenBranch.get()) ||
                           stmtNeedsDeferMachinery(ifs->elseBranch.get());
                if (auto *whs = dynamic_cast<const ::Sad::AST::WhileStmt *>(stmt))
                    return stmtNeedsDeferMachinery(whs->body.get());
                if (auto *fst = dynamic_cast<const ::Sad::AST::ForStmt *>(stmt))
                    return stmtNeedsDeferMachinery(fst->initializer.get()) ||
                           stmtNeedsDeferMachinery(fst->body.get());
                if (auto *frs = dynamic_cast<const ::Sad::AST::ForRangeStmt *>(stmt))
                    return stmtNeedsDeferMachinery(frs->body.get());
                if (auto *wst = dynamic_cast<const ::Sad::AST::WithStmt *>(stmt))
                    return stmtNeedsDeferMachinery(wst->body.get());
                // (AR) كتل التوجيهات @غير_آمن و@زمن_ترجمة تحمل جسمًا يُبنى فعليًّا؛
                //      «أجّل» داخلها يجب أن يُبقي الآلة وإلّا انحدرت الدلالة صامتًا.
                // (EN) Directive blocks (unsafe/comptime) carry a real built body;
                //      «أجّل» inside must keep the machinery.
                if (auto *usb = dynamic_cast<const ::Sad::AST::UnsafeBlockStmt *>(stmt))
                {
                    for (const auto &s : usb->body)
                        if (stmtNeedsDeferMachinery(s.get()))
                            return true;
                    return false;
                }
                if (auto *cmb = dynamic_cast<const ::Sad::AST::ComptimeBlockStmt *>(stmt))
                {
                    for (const auto &s : cmb->body)
                        if (stmtNeedsDeferMachinery(s.get()))
                            return true;
                    return false;
                }
                if (auto *tst = dynamic_cast<const ::Sad::AST::TryStmt *>(stmt))
                {
                    if (stmtNeedsDeferMachinery(tst->tryBlock.get()))
                        return true;
                    for (const auto &cc : tst->catchClauses)
                        if (stmtNeedsDeferMachinery(cc.body.get()))
                            return true;
                    return stmtNeedsDeferMachinery(tst->finallyBlock.get());
                }
                if (auto *sws = dynamic_cast<const ::Sad::AST::SwitchStmt *>(stmt))
                {
                    for (const auto &cb : sws->cases)
                        if (stmtNeedsDeferMachinery(cb.body.get()))
                            return true;
                    return stmtNeedsDeferMachinery(sws->defaultCase.get());
                }
                if (auto *sel = dynamic_cast<const ::Sad::AST::SelectStmt *>(stmt))
                {
                    for (const auto &sc : sel->cases)
                    {
                        if (!sc)
                            continue;
                        for (const auto &s : sc->body)
                            if (stmtNeedsDeferMachinery(s.get()))
                                return true;
                    }
                    for (const auto &s : sel->defaultBody)
                        if (stmtNeedsDeferMachinery(s.get()))
                            return true;
                    return false;
                }
                return false;
            }

            // ============================================================================
            // buildFunction - بناء دالة كاملة
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:348
            // التوقيع / Signature: void buildFunction(AST::FunctionDeclNode* funcDecl);
            //
            // المعاملات / Parameters:
            // - funcDecl: AST::FunctionDeclNode* = Sad::AST::FunctionDecl* (sir_builder.h:58)
            //
            // FunctionDecl Members (declarations.h:19-64):
            // - name: std::string (line 43)
            // - parameters: std::vector<Parameter> (line 44)
            // - returnType: Types::SadTypeKind (line 45)
            // - body: StmtPtr (line 46)
            // - isBuiltin: bool (line 47)
            //
            // Parameter Structure (declarations.h:24-41):
            // - name: std::string
            // - type: Types::SadTypeKind
            // - defaultValue: ExprPtr (optional)
            //
            // الدوال المستدعاة / Called functions:
            // - std::make_shared<SIRFunction>: standard library
            // - SIRFunction::addParameter: sir_module.h:289
            // - astTypeToSIRType: sir_builder.h:713
            // - createBasicBlock: sir_builder.h:501
            // - SIRFunction::addBasicBlock: sir_module.h:321
            // - buildStatement: sir_builder.h:372
            // - module_->addFunction: sir_module.h:569
            // ============================================================================

            // (AR) (اللبنة 3.17) بلاغ SEM024 من كتالوج الأخطاء (مصدر الحقيقة الوحيد — لا
            //      نصّ يدويّ)، نظير نمط الواجهة الخلفيّة (buildBilingualMessage) لكن يُدفَع
            //      إلى errors_ ليُفشِل البناء عبر hasErrors() (البلاغ المطبوع لا يكفي وحده
            //      لأنّ السائق يبوّب على hasErrors). التمرير: symbol = اسم الرمز المُصدَّر.
            // (EN) (Brick 3.17) SEM024 diagnostic from the error catalog (single SoT — no
            //      hand-written text), same pattern as the backend (buildBilingualMessage),
            //      but pushed to errors_ so hasErrors() fails the build (a printed message
            //      alone is insufficient — the driver gates on hasErrors).
            void SIRBuilder::reportDuplicateExportSymbol(const std::string &symbol)
            {
                Sad::Errors::RenderContext ctx;
                ctx.placeholders = {{"symbol", symbol}};
                errors_.push_back(
                    Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                        Sad::Errors::ErrorCode::SEM_DUP_EXPORT_SYMBOL, ctx));
            }

            void SIRBuilder::buildFunction(AST::FunctionDeclNode *funcDecl)
            {
                if (!funcDecl)
                {
                    return;
                }

                // (AR) تحويل نوع الإرجاع من DataType إلى SadTypeKind
                // (EN) Convert return type from DataType to SadTypeKind
                // astTypeToSIRType: sir_builder.h:713
                SadTypeKind returnType;

                // ================================================================
                // (AR) المرحلة 1.75 — تسجيل أنواع الأصناف للمعاملات قبل استنتاج نوع الإرجاع
                //      هذا يسمح لـ inferReturnTypeFromBody بمعرفة أن ط_أ.اسم هو حقل STRING
                //      عندما يكون ط_أ معاملاً يُمرر إليه كائن من صنف طالب
                // (EN) Phase 1.75 — Register class types for parameters before return type inference
                //      This allows inferReturnTypeFromBody to know that ط_أ.اسم is a STRING field
                //      when ط_أ is a parameter that receives an object of class طالب
                // ================================================================
                std::vector<std::string> tempRegisteredParams; // (AR) للتنظيف لاحقاً
                {
                    auto pctIt = paramClassTypes_.find(funcDecl->name);
                    if (pctIt != paramClassTypes_.end())
                    {
                        for (const auto &[paramName, className] : pctIt->second)
                        {
                            // (AR) نتحقق من عدم وجود تسجيل سابق لتجنب الكتابة فوقه
                            // (EN) Check for existing registration to avoid overwriting
                            if (classInstanceTypes_.find(paramName) == classInstanceTypes_.end())
                            {
                                classInstanceTypes_[paramName] = className;
                                tempRegisteredParams.push_back(paramName);
                            }
                        }
                    }
                }

                // (AR) إذا كان نوع الإرجاع غير محدد (UNKNOWN/NONE)، نستنتجه من جسم الدالة
                // (EN) If return type is unspecified (UNKNOWN/NONE), infer it from function body
                if (funcDecl->returnType == Types::SadTypeKind::Unknown ||
                    funcDecl->returnType == Types::SadTypeKind::Void)
                {
                    // (AR) استنتاج النوع من جسم الدالة مع معلومات المعاملات
                    // (EN) Infer type from function body with parameter information
                    returnType = inferReturnTypeFromBody(funcDecl->body.get(), funcDecl);
                }
                else
                {
                    returnType = astTypeToSIRType(funcDecl->returnType);
                }

                // (AR) نوع إرجاع اختياريّ T؟: استعمل النوع الداخليّ T للإرجاع (NS-06)
                // (EN) Optional return type T?: use inner type T for the return (NS-06)
                returnType = resolveDeclaredStorageKind(funcDecl->returnType,
                                                        funcDecl->sadReturnType.get(),
                                                        returnType);

                // (AR) إنشاء دالة SIR جديدة (sir_module.h:235 - SIRFunction constructor)
                // (EN) Create new SIR function
                auto sirFunction = std::make_shared<SIRFunction>(funcDecl->name, returnType);

                // (AR) نقل سمات الدالة [[سمة]] من AST إلى SIR لتُترجم لاحقاً
                //      إلى LLVM function attributes في codegen.
                // (EN) Propagate function attributes [[attr]] from AST to SIR
                //      so they can be lowered to LLVM function attributes in codegen.
                sirFunction->attributes = funcDecl->attributes;
                // (AR) اللبنة 3.15: مُعدِّل «دالة لا_ترجع» ⇒ سمة LLVM NoReturn في codegen.
                // (EN) Brick 3.15: 'دالة لا_ترجع' modifier ⇒ LLVM NoReturn attribute in codegen.
                sirFunction->isNoReturn = funcDecl->isNoReturn;
                // (AR) [RFC #53 F2-ج] راية الدالّة الخارجيّة (FFI) + اسم صنف العائد: تفعّل
                //      الخلفيّةُ بهما تمريرَ/إرجاعَ بنية @تمثيل_سي **بالقيمة** حسب تصنيف ABI
                //      للهدف (على حدّ FFI حصرًا؛ نداءات ص↔ص تُبقي البنى مؤشِّرات كائنيّة).
                //      اسمُ صنف العائد يأتي من التصريح (returnTypeName) لا من الاستنتاج، إذ
                //      أُسقِط النوعُ إلى Integer في astTypeToSIRType فلا يبقى إلا الاسم.
                // (EN) [RFC #53 F2-ج] extern (FFI) flag + return struct class name: the backend
                //      uses them to pass/return a @تمثيل_سي struct BY VALUE per the target ABI
                //      (at the FFI boundary only; ص↔ص calls keep structs as object pointers).
                //      The return class name comes from the declaration (returnTypeName) — the
                //      Class kind is lowered to Integer in astTypeToSIRType, so only the name survives.
                sirFunction->isExtern = funcDecl->isExtern;
                if (funcDecl->returnType == Types::SadTypeKind::Class &&
                    !funcDecl->returnTypeName.empty())
                {
                    sirFunction->returnClassName = funcDecl->returnTypeName;
                }
#ifdef SIR_BUILDER_DEBUG
                std::cerr << "[SIR-DBG] buildFunction: '" << funcDecl->name
                          << "' inferred retType=" << static_cast<int>(returnType) << std::endl;
#endif

                // (AR) إذا كانت دالة غير متزامنة، اجعلها كوروتين
                // (EN) If async function, mark as coroutine
                if (funcDecl->is_async)
                {
                    sirFunction->isCoroutine = true;
                    // (AR) يُحفَظُ **قبل** الدهس بسطر: هذا هو الموضعُ الوحيدُ الذي
                    //      يُعرَف فيه النوعُ المستنتَجُ من جسمِ الدالّة.
                    // (EN) Saved one line BEFORE it is overwritten — the only place the
                    //      type inferred from the body is still known.
                    sirFunction->coroutineValueType = returnType;
                    // (AR) الكوروتين يُرجع مؤشراً (handle) بدلاً من القيمة مباشرة
                    // (EN) Coroutine returns a pointer (handle) instead of direct value
                    sirFunction->returnType = SadTypeKind::Pointer;
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[CORO] Function '" << funcDecl->name << "' marked as coroutine" << std::endl;
#endif
                }

                // (AR) إذا كانت دالة مولّد، اجعلها كوروتين أيضاً
                // (EN) If generator function, also mark as coroutine
                if (funcDecl->isGenerator)
                {
                    sirFunction->isCoroutine = true;
                    sirFunction->isGenerator = true;
                    sirFunction->coroutineValueType = returnType;
                    // (AR) المولّد يُرجع مؤشراً (handle) — المستهلك يجمع القيم
                    // (EN) Generator returns a pointer (handle) — consumer collects values
                    sirFunction->returnType = SadTypeKind::Pointer;
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[GEN] Function '" << funcDecl->name << "' marked as generator" << std::endl;
#endif
                }

                // (AR) تعيين اسم الربط الخارجي (FFI) إذا كان محدداً
                // (EN) Set FFI link name if specified
                if (!funcDecl->linkName.empty())
                {
                    sirFunction->linkName = funcDecl->linkName;

                    // (AR) (اللبنة 3.17) حارس تصادم @رمز على الدوالّ المُعرَّفة: رمزان
                    //      مُصدَّران باسم رابط واحد يُدمَجهما الخلفيّة صامتًا (الثاني
                    //      يُفقَد ⇒ كودٌ خاطئ). نرفض التكرار بخطأ قاتل (SEM024). يُطبَّق على
                    //      المُعرَّفة فقط (الخارجيّة بلا جسم = استيراد، لا تتصادم). نتقاطع
                    //      أيضًا مع رموز المتغيّرات المُصدَّرة: دالّة تشارك متغيّرًا الاسمَ
                    //      تجعل LLVM يُعيد تسمية أحدهما (@اسم.1) فيَضيع عقد الـABI (رصد
                    //      Amelia H2). التقاطع مستقلٌّ عن ترتيب معالجة الدوالّ/المتغيّرات.
                    // (EN) (Brick 3.17) @رمز collision guard for defined functions: two
                    //      exports under one link symbol are silently merged by the backend
                    //      (second lost ⇒ wrong code). Reject duplicates (SEM024). Applies to
                    //      defined functions only (extern = import, no collision). Also
                    //      cross-checks variable export symbols: a function sharing a
                    //      variable's name makes LLVM rename one (@name.1), losing the ABI
                    //      contract (Amelia H2). Cross-check is order-independent.
                    if (!funcDecl->isExtern)
                    {
                        const bool dupFunc =
                            !exportedFunctionLinkNames_.insert(funcDecl->linkName).second;
                        const bool dupVar =
                            exportedVarLinkNames_.count(funcDecl->linkName) != 0;
                        if (dupFunc || dupVar)
                        {
                            reportDuplicateExportSymbol(funcDecl->linkName);
                        }
                    }
                }

                // (AR) إضافة المعاملات (declarations.h:44 - parameters: vector<Parameter>)
                // (EN) Add parameters
                // (AR) نستخدم الأنواع المستنتجة من functionTable_ (المرحلة 1.7)
                //      بدلاً من الأنواع الخام من AST عندما يكون النوع UNKNOWN
                // (EN) Use inferred types from functionTable_ (Phase 1.7)
                //      instead of raw AST types when type is UNKNOWN
                auto ftIt = functionTable_.find(funcDecl->name);
                for (size_t i = 0; i < funcDecl->parameters.size(); i++)
                {
                    const auto &param = funcDecl->parameters[i];
                    SadTypeKind paramType = astTypeToSIRType(param.type);

                    // (AR) معامل اختياريّ T؟: استعمل النوع الداخليّ T للتخزين (NS-06 موجة 3)
                    // (EN) Optional parameter T?: use inner type T for storage (NS-06 wave 3)
                    // (AR) الموضعُ الأحرجُ: معامِلُ `منطقي؟` كان يُخفَّض `i1` فيتطابق
                    //      وسيطُ `لاشيء` ووسيطُ `صحيح` بتًّا ببتّ.
                    // (EN) The critical site: a `bool?` parameter lowered to `i1`, making the
                    //      `null` and `true` arguments bit-for-bit identical.
                    paramType = resolveDeclaredStorageKind(param.type, param.sadType.get(), paramType);

                    // (AR) إذا كان النوع I64 (من UNKNOWN) وfunctionTable_ يحتوي نوعاً مستنتجاً أفضل
                    // (EN) If type is I64 (from UNKNOWN) and functionTable_ has a better inferred type
                    // (AR) إصلاح X04: لا تُبدِّل إلى Boolean (i1) لأن null/لاشيء يُمرَّر كـ i64 sentinel
                    //      تبديل UNKNOWN→Boolean يُسبّب trunc i64→i1 عند الاستدعاء مما يُضيّع قيمة null
                    // (EN) FIX X04: Don't override to Boolean (i1) — null is passed as i64 sentinel
                    //      Overriding UNKNOWN→Boolean causes trunc i64→i1 at call site, corrupting null
                    // (AR) دَين مراجعة #258 (RUN001 كاذبة): المصرَّح «رقم» يقبل العشريّ في المفسّر
                    //      (المرجع)، والمرحلة 1.7 ترقّيه في functionTable_ إلى Float/Any من مواقع
                    //      الاستدعاء — لكنّ شرط Unknown هنا كان يرفض الترقية فيبقى المعامل i64
                    //      ويقتطع fptosi الوسيطَ العشريّ عند الحدود (اقسم(7.0، 0.5) ⇒ مقام 0 ⇒
                    //      RUN001 كاذبة). نقبل الترقية العدديّة (Float/Any حصرًا) للمصرَّح «رقم»
                    //      أيضًا — عدا خارجي/@رمز (ABI مثبَّت لا يُعاد تشكيله من مواقع الاستدعاء).
                    // (EN) #258 review debt (false RUN001): a declared «رقم» accepts floats in the
                    //      interpreter (reference), and Phase 1.7 widens it to Float/Any in
                    //      functionTable_ from call sites — but the Unknown gate here rejected the
                    //      widening, leaving the param i64 so fptosi truncated float args at the
                    //      boundary (divide(7.0, 0.5) ⇒ divisor 0 ⇒ false RUN001). Accept the
                    //      numeric widening (Float/Any only) for declared «رقم» too — except
                    //      extern/@رمز functions (fixed ABI, never reshaped from call sites).
                    if (paramType == SadTypeKind::Integer &&
                        ftIt != functionTable_.end() &&
                        i < ftIt->second.parameters.size() &&
                        ftIt->second.parameters[i].type != SadTypeKind::Integer &&
                        ftIt->second.parameters[i].type != SadTypeKind::Boolean &&
                        (param.type == Types::SadTypeKind::Unknown ||
                         (param.type == Types::SadTypeKind::Integer &&
                          !funcDecl->isExtern && funcDecl->linkName.empty() &&
                          (ftIt->second.parameters[i].type == SadTypeKind::Float ||
                           ftIt->second.parameters[i].type == SadTypeKind::Any))))
                    {
                        paramType = ftIt->second.parameters[i].type;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[SIR-DBG] buildFunction '" << funcDecl->name
                                  << "' param[" << i << "] '" << param.name
                                  << "': UNKNOWN->I64 overridden to "
                                  << static_cast<int>(paramType) << std::endl;
#endif
                    }

                    SIRParameter sirParam(param.name, paramType);
                    // (AR) معامل مصرَّح بصنفٍ مسجَّل («حدث ح» أو صنف مستخدم): انقل اسم
                    //      الصنف إلى SIR كي تبذره الخلفيّة في objectClassMap — التصريح
                    //      أوثق من تخمين الصنف بالاسم، ومعالِج الحدث لا مواقعَ استدعاء له.
                    // (EN) Param declared with a registered class («حدث ح» or a user
                    //      class): carry the class name into SIR so the backend seeds
                    //      objectClassMap — the declaration beats name-guessing, and an
                    //      event handler has no call sites to infer from.
                    if (!param.typeName.empty() && module_ && module_->getClass(param.typeName))
                    {
                        sirParam.className = param.typeName;
                    }
                    sirFunction->addParameter(sirParam);
                }

                // (AR) إذا لم يكن هناك جسم للدالة (builtin or declaration only), لا نبني الجسم
                // (EN) If no function body (builtin or declaration only), don't build body
                if (!funcDecl->body)
                {
                    // (AR) مزامنة جدول الدوالّ مع نوع إرجاع SIRFunction الحقيقيّ —
                    //      الخروج المبكّر هنا كان يتخطّى تسجيل النوع أدناه فيبقى
                    //      مدخلُ الطور الأوّل (Integer الافتراضيّ) ويتباعد عن
                    //      الإعلان المُصدَر (declare void) في مواضع النداء.
                    // (EN) Sync the function table with the real SIRFunction return
                    //      type — this early return used to skip the registration
                    //      below, leaving phase-1's default (Integer) diverging from
                    //      the emitted declaration at call sites.
                    auto tableIt = functionTable_.find(funcDecl->name);
                    if (tableIt != functionTable_.end())
                    {
                        // (AR) 🔑 `sirFunction->returnType` لا `returnType`: التعليقُ أعلاه
                        //      يَعِدُ بمزامنةِ النوعِ **الحقيقيّ**، وكان يُسنِدُ المستنتَجَ —
                        //      أي عينُ العطبِ المُصلَحِ في التسجيلِ الكاملِ أسفلَ الملفّ،
                        //      باقيًا في توأمِه. ودالّةٌ `غير_متزامن` بلا جسمٍ يُعادُ ضبطُ
                        //      نوعِها إلى `Pointer` قبلَ هذا السطرِ بكثير.
                        // (EN) The comment above promises the REAL type but assigned the
                        //      inferred one — the very defect fixed in the full registration
                        //      below, left standing in its twin. A bodiless async function has
                        //      its type rewritten to Pointer long before this line.
                        tableIt->second.returnType = sirFunction->returnType;
                        tableIt->second.sirFunction = sirFunction;
                    }
                    module_->addFunction(sirFunction);
                    return;
                }

                // (AR) تعيين الدالة الحالية
                // (EN) Set current function
                currentFunction_ = sirFunction;

                // (AR) إنشاء نطاق جديد للدالة
                // (EN) Create new scope for function
                enterScope();

                // (AR) تسجيل معاملات الدالة في النطاق
                // (EN) Register function parameters in scope
                // (AR) نستخدم الأنواع المستنتجة (مطابقة لما تم إضافته للدالة أعلاه)
                // (EN) Use inferred types (matching what was added to function above)
                for (size_t i = 0; i < funcDecl->parameters.size(); i++)
                {
                    const auto &param = funcDecl->parameters[i];
                    SadTypeKind paramType = astTypeToSIRType(param.type);

                    // (AR) معامل اختياريّ T؟: استعمل النوع الداخليّ T للتخزين (NS-06 موجة 3)
                    // (EN) Optional parameter T?: use inner type T for storage (NS-06 wave 3)
                    // (AR) الموضعُ الأحرجُ: معامِلُ `منطقي؟` كان يُخفَّض `i1` فيتطابق
                    //      وسيطُ `لاشيء` ووسيطُ `صحيح` بتًّا ببتّ.
                    // (EN) The critical site: a `bool?` parameter lowered to `i1`, making the
                    //      `null` and `true` arguments bit-for-bit identical.
                    paramType = resolveDeclaredStorageKind(param.type, param.sadType.get(), paramType);

                    // (AR) نفس المنطق: استخدام النوع المستنتج عندما يكون UNKNOWN
                    // (EN) Same logic: use inferred type when UNKNOWN
                    // (AR) إصلاح X04: نفس الشرط — لا تُبدِّل إلى Boolean
                    // (EN) FIX X04: Same condition — don't override to Boolean
                    // (AR) دَين #258: نفس الشرط — قبول الترقية العدديّة للمصرَّح «رقم» (انظر أعلاه)
                    // (EN) #258 debt: same condition — accept numeric widening for declared «رقم» (see above)
                    if (paramType == SadTypeKind::Integer &&
                        ftIt != functionTable_.end() &&
                        i < ftIt->second.parameters.size() &&
                        ftIt->second.parameters[i].type != SadTypeKind::Integer &&
                        ftIt->second.parameters[i].type != SadTypeKind::Boolean &&
                        (param.type == Types::SadTypeKind::Unknown ||
                         (param.type == Types::SadTypeKind::Integer &&
                          !funcDecl->isExtern && funcDecl->linkName.empty() &&
                          (ftIt->second.parameters[i].type == SadTypeKind::Float ||
                           ftIt->second.parameters[i].type == SadTypeKind::Any))))
                    {
                        paramType = ftIt->second.parameters[i].type;
                    }

                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = paramType;
                    // (AR) [طبقة طبيعي64 — الخطوة ٥] النوع السطحيّ المُصرَّح للمعامل (طبيعي64/بايت…)
                    //      من تعليق الـAST مباشرةً — لا paramType الذي قد يُرقَّى بالاستنتاج أعلاه.
                    //      يُطابق تسجيل المفسّر لنوع المعامل، فتتّفق إشارة المقارنة على المسارين.
                    // (EN) [طبيعي64 layer — Step 5] The parameter's explicitly-declared surface type
                    //      from the AST annotation directly — not paramType which may be inference-
                    //      promoted above. Mirrors the interpreter's param declared-type registration
                    //      so the comparison signedness agrees on both tracks.
                    paramInfo.declaredSurfaceType = param.type;
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    // (AR) معاملات الدوال قابلة للتعديل — تمثل نسخة محلية من القيمة الممررة
                    //      مثال: دالة حلقة(ع) → ع = ع - 1 داخل بينما يجب أن يعمل
                    //      بدون هذا: buildAssignment يتجاهل التعيين بصمت → حلقة لانهائية
                    // (EN) Function parameters are mutable — they represent a local copy of the passed value
                    //      Example: func loop(x) → x = x - 1 inside while must work
                    //      Without this: buildAssignment silently skips assignment → infinite loop
                    paramInfo.isMutable = true;
                    paramInfo.isParameter = true;
                    paramInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(scopeStack_.size(), "sir_builder_functions_size");

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) نقل نوع عنصر المصفوفة من FunctionInfo إلى VariableInfo
                    //      Phase 1.7 حدّد elementType من فحص ArrayExpr في مواقع الاستدعاء
                    //      بدون هذا: forEach على معامل مصفوفة نصوص يُعامل العناصر كأرقام
                    // (EN) Propagate array element type from FunctionInfo to VariableInfo
                    //      Phase 1.7 inferred elementType from ArrayExpr at call sites
                    //      Without this: forEach over string array param treats elements as integers
                    // ═══════════════════════════════════════════════════════════════
                    if (ftIt != functionTable_.end() && i < ftIt->second.parameters.size())
                    {
                        if (ftIt->second.parameters[i].elementType != SadTypeKind::Void)
                        {
                            paramInfo.elementType = ftIt->second.parameters[i].elementType;
                        }
                    }

                    // (AR) [أ-م٥] معامل مصرَّح بنوع تعداد جبريّ («عقدة ع»): سجّل صنفه = اسم
                    //      التعداد كي يحمل className عبر buildExpression، فتعرف «طابق» نوع
                    //      القيمة المُطابَقة وتميّز المعاملَ الوحدويّ العاري «عندما فراغ:» (فحص
                    //      وسم) عن الرباط الشامل. الأصناف المُسجَّلة لها مسارها (module_->getClass)
                    //      فنقصر هذا على تعدادات adtEnumTable_ حصرًا كيلا نُظلّل مسارها.
                    // (EN) [A-M5] Param declared with an ADT enum type («Node n»): record its
                    //      class = the enum name so className flows via buildExpression, letting
                    //      «match» know the matched value's type and distinguish a bare unit variant
                    //      «when None:» (tag check) from a catch-all. Registered classes have their
                    //      own path (module_->getClass); restrict this to adtEnumTable_ enums only.
                    if (!param.typeName.empty() && adtEnumTable_.count(param.typeName))
                    {
                        classInstanceTypes_[param.name] = param.typeName;
                    }

                    addVariable(paramInfo);
                }

                // (AR) إنشاء basic block للدخول (createBasicBlock: sir_builder.h:501)
                // (EN) Create entry basic block
                auto entryBlock = createBasicBlock(kEntryBlockName);

                // (AR) إضافة block للدالة (sir_module.h:321 - addBasicBlock)
                // (EN) Add block to function
                sirFunction->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;

                // (AR) تنظيف مكدس التأجيل (defer) السابق وبدء مكدس جديد لهذه الدالة
                // (EN) Clear previous defer stack and start fresh for this function
                auto savedDeferredStatements = std::move(deferredStatements_);
                deferredStatements_.clear();
                auto savedDeferStackReg = currentDeferStackReg_;
                auto savedDeferExecutedFlagReg = currentDeferExecutedFlagReg_;
                bool savedCleanupHandlerState = currentFunctionCleanupHandlerActive_;

                // (AR) الآلة تُبنى فقط عند الحاجة الفعليّة (انظر stmtNeedsDeferMachinery).
                //      عند غيابها: يبقى currentDeferStackReg_ فارغًا، والذيل الموجود
                //      أصلًا (المحروس بـ !currentDeferStackReg_.empty()) يتصرّف صحيحًا،
                //      ويُبنى الجسم مباشرة في كتلة الدخول بلا برولوج.
                // (EN) Machinery is emitted only when actually needed. When absent,
                //      currentDeferStackReg_ stays empty and the existing guarded
                //      epilogue paths behave correctly; the body builds straight
                //      into the entry block with no prologue.
                // (AR) قصر الإسقاط على الوضع الحرّ: هناك فقط تكلّف الآلة رموز libc
                //      (_setjmp/malloc) غير المتوفّرة على المعدن. في الوضع المستضاف
                //      نُبقيها دائمًا (سلوك مطابق لـdev بلا انحدار) — إسقاطها هناك كان
                //      يغيّر مسار بناء قيمة الإرجاع الحسّاس (المحروس بـ
                //      currentDeferStackReg_) فيُحدث تباينًا مترجم/مفسّر على بعض المنصّات.
                // (EN) Restrict the skip to freestanding: only there does the machinery
                //      pull libc symbols (_setjmp/malloc) absent on bare metal. In hosted
                //      mode always keep it (dev-identical, no regression) — skipping it
                //      altered the return-value build path (guarded by currentDeferStackReg_)
                //      causing compiler/interpreter divergence on some platforms.
                const bool needsDeferMachinery =
                    freestandingMode_ ? stmtNeedsDeferMachinery(funcDecl->body.get()) : true;

                std::shared_ptr<SIRBasicBlock> functionCleanupBlock;

                if (needsDeferMachinery)
                {
                currentDeferStackReg_ = std::string("%") + kSlotNamespaceSeparator + "defer_stack" + kSlotNamespaceSeparator + std::to_string(nextLabel_++);
                currentDeferExecutedFlagReg_ = std::string("%") + kSlotNamespaceSeparator + "defer_done" + kSlotNamespaceSeparator + std::to_string(nextLabel_++);
                currentFunctionCleanupHandlerActive_ = true;

                {
                    SIRInstruction newArrInst;
                    newArrInst.opcode = SIROpcode::ARRAY_NEW;
                    newArrInst.result = SIROperand::Register(currentDeferStackReg_, SadTypeKind::Array);
                    newArrInst.operands.push_back(SIROperand::ConstantI64(8));
                    newArrInst.operands.push_back(SIROperand::ConstantI64(0));
                    newArrInst.comment = "runtime defer stack for function";
                    currentBlock_->addInstruction(newArrInst);
                }

                {
                    SIRInstruction allocDoneInst;
                    allocDoneInst.opcode = SIROpcode::ALLOC;
                    allocDoneInst.result = SIROperand::Register(currentDeferExecutedFlagReg_, SadTypeKind::Integer);
                    allocDoneInst.comment = "alloca defer executed flag";
                    currentBlock_->addInstruction(allocDoneInst);

                    SIRInstruction initDoneInst;
                    initDoneInst.opcode = SIROpcode::STORE;
                    initDoneInst.operands.push_back(SIROperand::ConstantI64(0));
                    initDoneInst.operands.push_back(SIROperand::Register(currentDeferExecutedFlagReg_, SadTypeKind::Integer));
                    initDoneInst.comment = "init defer executed flag = 0";
                    currentBlock_->addInstruction(initDoneInst);
                }

                std::string functionCleanupJmpbufReg = std::string("%") + kSlotNamespaceSeparator + "defer_jmpbuf" + kSlotNamespaceSeparator + std::to_string(nextLabel_++);
                std::string functionCleanupSetjmpReg = newTempRegister();
                std::string functionCleanupCmpReg = newTempRegister();
                std::string functionBodyLabel = newLabel("function_body");
                std::string functionCleanupLabel = newLabel("function_defer_cleanup");
                auto functionBodyBlock = createBasicBlock(functionBodyLabel);
                functionCleanupBlock = createBasicBlock(functionCleanupLabel);
                currentFunction_->addBasicBlock(functionBodyBlock);
                currentFunction_->addBasicBlock(functionCleanupBlock);

                {
                    SIRInstruction allocJmpbufInst;
                    allocJmpbufInst.opcode = SIROpcode::CALL;
                    allocJmpbufInst.result = SIROperand::Register(functionCleanupJmpbufReg, SadTypeKind::Pointer);
                    allocJmpbufInst.operands.push_back(SIROperand::Function("__sad_alloc_jmpbuf"));
                    allocJmpbufInst.comment = "allocate function-level defer cleanup jmpbuf";
                    currentBlock_->addInstruction(allocJmpbufInst);

                    SIRInstruction pushHandlerInst;
                    pushHandlerInst.opcode = SIROpcode::CALL;
                    pushHandlerInst.operands.push_back(SIROperand::Function("__sad_push_handler"));
                    pushHandlerInst.operands.push_back(SIROperand::Register(functionCleanupJmpbufReg, SadTypeKind::Pointer));
                    pushHandlerInst.comment = "push function-level defer cleanup handler";
                    currentBlock_->addInstruction(pushHandlerInst);

                    SIRInstruction setjmpInst;
                    setjmpInst.opcode = SIROpcode::CALL;
                    setjmpInst.result = SIROperand::Register(functionCleanupSetjmpReg, SadTypeKind::Integer);
                    setjmpInst.operands.push_back(SIROperand::Function("__sad_setjmp"));
                    setjmpInst.operands.push_back(SIROperand::Register(functionCleanupJmpbufReg, SadTypeKind::Pointer));
                    setjmpInst.comment = "setjmp for function-level defer cleanup";
                    currentBlock_->addInstruction(setjmpInst);

                    SIRInstruction cmpInst = SIRInstruction::Binary(
                        SIROpcode::NE,
                        SIROperand::Register(functionCleanupCmpReg, SadTypeKind::Boolean),
                        SIROperand::Register(functionCleanupSetjmpReg, SadTypeKind::Integer),
                        SIROperand::ConstantI64(0));
                    cmpInst.comment = "function-level defer cleanup: did exception escape?";
                    currentBlock_->addInstruction(cmpInst);

                    currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(functionCleanupCmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(functionCleanupLabel),
                        SIROperand::Label(functionBodyLabel)));
                }

                currentBlock_ = functionBodyBlock;
                }
                else
                {
                    // (AR) لا «أجّل» في الجسم — لا مكدّس تأجيل ولا إطار setjmp:
                    //      صفرُ تخصيصات وصفرُ رموز libc لهذه الدالّة.
                    // (EN) No defer in body — no defer stack, no setjmp frame:
                    //      zero allocations and zero libc symbols for this function.
                    currentDeferStackReg_.clear();
                    currentDeferExecutedFlagReg_.clear();
                    currentFunctionCleanupHandlerActive_ = false;
                }

                // (AR) بناء جسم الدالة (declarations.h:46 - body: StmtPtr)
                // (EN) Build function body

                // ================================================================
                // (AR) العقود البرمجية: بناء شروط يتطلب (preconditions)
                //      إذا فشل الشرط، نرفع استثناء عبر __sad_raise يمكن التقاطه بـ حاول/امسك
                //      مثال: يتطلب ب != 0 → إذا (!(ب != 0)) { __sad_raise("خطأ", "فشل...") }
                // (EN) Design by Contract: build preconditions (requires)
                //      If condition fails, raise exception via __sad_raise (catchable by try/catch)
                //      Example: requires b != 0 → if (!(b != 0)) { __sad_raise("خطأ", "فشل...") }
                // ================================================================
                if (!funcDecl->preconditions.empty())
                {
                    for (const auto &precond : funcDecl->preconditions)
                    {
                        if (!precond)
                            continue;
                        auto condResult = buildExpression(precond.get());
                        if (condResult.registerName.empty())
                            continue;

                        // (AR) إنشاء كتل: fail (فشل) + pass (نجاح)
                        std::string failLabel = newLabel("precond_fail");
                        std::string passLabel = newLabel("precond_pass");
                        auto failBlock = createBasicBlock(failLabel);
                        auto passBlock = createBasicBlock(passLabel);
                        if (currentFunction_)
                        {
                            currentFunction_->addBasicBlock(failBlock);
                            currentFunction_->addBasicBlock(passBlock);
                        }

                        // (AR) القفز: إذا الشرط صحيح → pass، وإلا → fail
                        SIRInstruction brInst = SIRInstruction::BranchCond(
                            SIROperand::Register(condResult.registerName, condResult.type),
                            SIROperand::Label(passLabel),
                            SIROperand::Label(failLabel));
                        if (currentBlock_)
                            currentBlock_->instructions.push_back(brInst);

                        // (AR) كتلة الفشل: رفع استثناء عبر __sad_raise (يمكن التقاطه بـ حاول/امسك)
                        // (EN) Fail block: raise exception via __sad_raise (catchable by try/catch)
                        currentBlock_ = failBlock;
                        {
                            // (AR) [Fix BF-04] استبدال exit(1) بـ __sad_raise لجعل العقود قابلة للالتقاط
                            //      الرسالة تحتوي "فشل العقد" للتوافق مع سلوك المفسر
                            // (EN) [Fix BF-04] Replace exit(1) with __sad_raise for catchable contracts
                            //      Message contains "فشل العقد" for interpreter parity
                            std::string errMsg = "\xd9\x81\xd8\xb4\xd9\x84 \xd8\xa7\xd9\x84\xd8\xb9\xd9\x82\xd8\xaf \xd9\x81\xd9\x8a \xd8\xa7\xd9\x84\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 " + funcDecl->name;
                            SIRInstruction raiseInst;
                            raiseInst.opcode = SIROpcode::CALL;
                            raiseInst.operands.push_back(SIROperand::Function("__sad_raise"));
                            raiseInst.operands.push_back(SIROperand::ConstantString("\xd8\xae\xd8\xb7\xd8\xa3")); // "خطأ"
                            raiseInst.operands.push_back(SIROperand::ConstantString(errMsg));
                            raiseInst.comment = "precondition failure — raise catchable exception";
                            currentBlock_->addInstruction(raiseInst);

                            // (AR) كتلة ميتة بعد raise (longjmp لا يعود)
                            // (EN) Dead block after raise (longjmp never returns)
                            std::string deadLabel = newLabel("precond_dead");
                            auto deadBlock = createBasicBlock(deadLabel);
                            if (currentFunction_)
                                currentFunction_->addBasicBlock(deadBlock);
                            currentBlock_ = deadBlock;
                        }

                        // (AR) كتلة النجاح: المتابعة
                        currentBlock_ = passBlock;
                    }
                }

                if (funcDecl->body)
                {
                    // buildStatement: sir_builder.h:372
                    buildStatement(funcDecl->body.get());
                }

                auto bodyContinuationBlock = currentBlock_;

                // (AR) كتلة التنظيف تُبنى فقط مع الآلة (لا آلة ⇒ لا كتلة تنظيف)
                // (EN) Cleanup block only exists when the machinery was emitted
                if (needsDeferMachinery && functionCleanupBlock)
                {
                currentBlock_ = functionCleanupBlock;
                emitPopFunctionCleanupHandler();
                emitRunDeferredClosures();
                {
                    SIRInstruction rethrowInst;
                    rethrowInst.opcode = SIROpcode::CALL;
                    rethrowInst.operands.push_back(SIROperand::Function("__sad_raise_current"));
                    rethrowInst.comment = "run defer then rethrow escaping function exception";
                    currentBlock_->addInstruction(rethrowInst);
                }
                {
                    std::string deadLabel = newLabel("function_cleanup_dead");
                    auto deadBlock = createBasicBlock(deadLabel);
                    currentFunction_->addBasicBlock(deadBlock);
                    currentBlock_ = deadBlock;
                }
                }

                currentBlock_ = bodyContinuationBlock;

                // ================================================================
                // (AR) تنفيذ الجمل المؤجلة المتبقية (لنهاية الدالة الطبيعية بدون return)
                //      مهم: نتحقق أولاً أن الكتلة الحالية لا تحتوي على terminator
                //      إذا كان جسم الدالة ينتهي بـ return، فإن buildReturnStatement
                //      قد نفّذ الجمل المؤجلة بالفعل وأضاف RET. إضافة كود آخر بعد
                //      RET يتسبب في خطأ LLVM: "Terminator in middle of basic block"
                // (EN) Execute remaining deferred statements (for normal function end without return)
                //      Important: first check that current block doesn't have a terminator.
                //      If function body ends with return, buildReturnStatement already
                //      executed deferred statements and emitted RET. Adding code after
                //      RET causes LLVM error: "Terminator in middle of basic block"
                // ================================================================
                bool blockAlreadyTerminated = false;
                if (currentBlock_ && !currentBlock_->instructions.empty())
                {
                    auto lastOp = currentBlock_->instructions.back().opcode;
                    blockAlreadyTerminated = (lastOp == SIROpcode::RET ||
                                              lastOp == SIROpcode::RET_VOID ||
                                              lastOp == SIROpcode::BR ||
                                              lastOp == SIROpcode::BR_COND ||
                                              lastOp == SIROpcode::CORO_RETURN);
                }

                if (!blockAlreadyTerminated && !currentDeferStackReg_.empty())
                {
                    emitRunDeferredClosures();
                    emitPopFunctionCleanupHandler();
                }

                if (!blockAlreadyTerminated && currentDeferStackReg_.empty() && !deferredStatements_.empty())
                {
                    for (auto it = deferredStatements_.rbegin(); it != deferredStatements_.rend(); ++it)
                    {
                        if (*it)
                        {
                            buildStatement(*it);
                        }
                    }
                }

                // (AR) استعادة مكدس التأجيل السابق (للدوال المتداخلة)
                // (EN) Restore previous defer stack (for nested functions)
                deferredStatements_ = std::move(savedDeferredStatements);
                currentDeferStackReg_ = savedDeferStackReg;
                currentDeferExecutedFlagReg_ = savedDeferExecutedFlagReg;
                currentFunctionCleanupHandlerActive_ = savedCleanupHandlerState;

                // (AR) الخروج من نطاق الدالة
                // (EN) Exit function scope
                exitScope();

                // (AR) تنظيف تسجيلات classInstanceTypes_ المؤقتة للمعاملات (المرحلة 1.75)
                //      نُزيل فقط ما أضفناه مؤقتاً — التسجيلات الأصلية (من جديد ClassName) تبقى
                // (EN) Clean up temporary classInstanceTypes_ registrations for params (Phase 1.75)
                //      Only remove what we temporarily added — original registrations (from new ClassName) stay
                for (const auto &pName : tempRegisteredParams)
                {
                    classInstanceTypes_.erase(pName);
                }

                // (AR) التأكد من وجود terminator في نهاية الدالة
                // (EN) Ensure function has a terminator at the end
                // (AR) الكوروتينات لا تحتاج terminator إضافي - الخاتمة تتكفل بذلك
                // (EN) Coroutines don't need extra terminators - epilogue handles it
                if (!sirFunction->isCoroutine)
                {
                    // إذا كانت الدالة void ولا يوجد return صريح، نضيف RET_VOID
                    // If function is void and has no explicit return, add RET_VOID
                    if (currentBlock_ && !currentBlock_->instructions.empty())
                    {
                        const auto &lastInst = currentBlock_->instructions.back();
                        if (lastInst.opcode != SIROpcode::RET && lastInst.opcode != SIROpcode::RET_VOID && lastInst.opcode != SIROpcode::CORO_RETURN)
                        {
                            // (AR) لا يوجد return - نضيف واحداً
                            // (EN) No return - add one
                            if (returnType == SadTypeKind::Void)
                            {
                                SIRInstruction retInst;
                                retInst.opcode = SIROpcode::RET_VOID;
                                currentBlock_->addInstruction(retInst);
                            }
                            else
                            {
                                // (AR) للدوال غير void، نضيف return بقيمة افتراضية
                                // (EN) For non-void functions, add return with default value
                                SIRInstruction retInst;
                                retInst.opcode = SIROpcode::RET;
                                if (returnType == SadTypeKind::String)
                                {
                                    retInst.operands.push_back(SIROperand::ConstantString(""));
                                }
                                else
                                {
                                    retInst.operands.push_back(SIROperand::ConstantI64(0));
                                }
                                currentBlock_->addInstruction(retInst);
                            }
                        }
                    }
                    else if (currentBlock_ && currentBlock_->instructions.empty())
                    {
                        // (AR) الدالة فارغة - نضيف return
                        // (EN) Empty function - add return
                        if (returnType == SadTypeKind::Void)
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET_VOID;
                            currentBlock_->addInstruction(retInst);
                        }
                        else
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET;
                            if (returnType == SadTypeKind::String)
                            {
                                retInst.operands.push_back(SIROperand::ConstantString(""));
                            }
                            else
                            {
                                retInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                            currentBlock_->addInstruction(retInst);
                        }
                    }
                }

                // (AR) إضافة الدالة للوحدة (sir_module.h:569 - addFunction)
                // (EN) Add function to module
                module_->addFunction(sirFunction);

                // ═══════════════════════════════════════════════════════════════════
                // (AR) تسجيل/تحديث الدالة في جدول الدوال للبحث عنها عند الاستدعاء
                // (EN) Register/update function in functionTable_ for call resolution
                // ═══════════════════════════════════════════════════════════════════
                {
                    FunctionInfo funcInfo;
                    funcInfo.name = funcDecl->name;
                    // ════════════════════════════════════════════════════════════
                    // (AR) 🔑 نوعُ الإرجاعِ يُقرأ من **الدالّةِ نفسِها** لا من المتغيّرِ
                    //      المستنتَج: `returnType` استُنتِج من `ارجع` في الجسم، ثمّ
                    //      أُعيدت كتابةُ `sirFunction->returnType` إلى `Pointer` أعلاه
                    //      للكوروتين والمولّد. فحقيقةٌ واحدةٌ بقارئَين يفترقان.
                    //
                    //      🔴 والثمنُ مقيس: `دالة غير_متزامن حساب()` مع `ارجع 42`
                    //      تُسجَّلُ هنا Integer بينما الدالّةُ تُرجِعُ مقبضَ إطارٍ
                    //      (`ptr`). ثمّ `انتظر` — وفيها ممرُّ هُويّةٍ للقيمِ القياسيّةِ
                    //      وُضِع عمدًا لأنّ CORO_SUSPEND على عدديٍّ يُسقِط legalization —
                    //      يرى «Integer» فيمرِّرُ المقبضَ كما هو. فتُطبَع **عنوانًا**
                    //      (قِيس `2142012098544` بدل `42`) بلا خطأٍ ولا تحذير.
                    //      وسدُّ ذلك في `انتظر` وحدَه يترك الاختلافَ قائمًا لكلِّ قارئٍ
                    //      آخرَ لهذا الجدول — فالسدُّ عند مصدرِ الحقيقة.
                    // (EN) Read the return type from the FUNCTION, not from the inferred
                    //      local: `returnType` comes from the body's `ارجع`, but
                    //      `sirFunction->returnType` was rewritten to Pointer above for
                    //      coroutines/generators — one fact, two readers that disagree.
                    //      Measured cost: an async function registers as Integer while it
                    //      actually returns a frame handle, so `انتظر`'s scalar identity
                    //      path (added because CORO_SUSPEND on a scalar crashes LLVM
                    //      legalization) passes the handle straight through and prints an
                    //      address instead of the value, with no diagnostic.
                    // ════════════════════════════════════════════════════════════
                    funcInfo.returnType = sirFunction->returnType;
                    funcInfo.parameters = sirFunction->getParameters();
                    funcInfo.sirFunction = sirFunction;
                    funcInfo.isGenerator = sirFunction->isGenerator;
                    // (AR) حفظ مرجع AST لمعالجة القيم الافتراضية عند الاستدعاء
                    // (EN) Save AST reference for default parameter values at call sites
                    funcInfo.astDecl = funcDecl;

                    // ================================================================
                    // (AR) [Fix #52] تتبع اسم اللامدا المُرجعة:
                    //      إذا كانت الدالة تُرجع إغلاقاً (CLOSURE_CREATE)، نستخرج
                    //      اسم اللامدا من تعليمة RET الأخيرة ونحفظه في returnLambdaName.
                    //      هذا يسمح بتتبع نوع الإرجاع الصحيح عند استدعاء الإغلاق
                    //      عبر متغير (مثل: ل = صانع() ثم ل() )
                    // (EN) [Fix #52] Track returned lambda name:
                    //      If function returns a closure (CLOSURE_CREATE), extract
                    //      the lambda name from the last RET instruction and save
                    //      in returnLambdaName. Enables correct return type tracking
                    //      when calling the closure via a variable.
                    // ================================================================
                    if (returnType == SadTypeKind::Function)
                    {
                        // ================================================================
                        // (AR) [Fix #52-v2] تتبع LOAD→STORE→CLOSURE_CREATE:
                        //      نمط SIR الشائع: CLOSURE_CREATE %c → STORE %c, %var
                        //                       → LOAD %tmp, %var → RET %tmp
                        //      الإصلاح القديم كان يبحث فقط عن CLOSURE_CREATE.result == retReg
                        //      لكن retReg = %tmp (من LOAD) وليس نتيجة CLOSURE_CREATE مباشرةً.
                        //
                        //      الخوارزمية الجديدة:
                        //      1. عند RET %retReg:
                        //         a. إذا وجد CLOSURE_CREATE.result == retReg → استخدمه مباشرة
                        //         b. إذا وجد LOAD.result == retReg → تتبع عبر LOAD إلى %var
                        //            → ابحث عن STORE %closure_reg, %var
                        //            → ابحث عن CLOSURE_CREATE.result == %closure_reg
                        //
                        // (EN) [Fix #52-v2] Trace through LOAD→STORE→CLOSURE_CREATE chain:
                        //      Common SIR pattern: CLOSURE_CREATE %c → STORE %c, %var
                        //                          → LOAD %tmp, %var → RET %tmp
                        //      Old fix only matched CLOSURE_CREATE.result == retReg directly.
                        //      New algorithm traces: retReg → LOAD source → STORE source →
                        //      CLOSURE_CREATE result.
                        // ================================================================

                        // (AR) بناء جداول بحث سريع: LOAD و STORE
                        // (EN) Build fast lookup tables: LOAD and STORE
                        // loadSrc[%tmp] = %var   (من: LOAD %tmp = LOAD %var)
                        // storeVal[%var] = %cls   (من: STORE %cls, %var)
                        // closureCreate[%cls] = lambdaName
                        std::unordered_map<std::string, std::string> loadSrc;    // load_result → loaded_var
                        std::unordered_map<std::string, std::string> storeVal;   // stored_to → stored_value
                        std::unordered_map<std::string, std::string> closureMap; // closure_reg → lambda_name

                        for (const auto &b2 : sirFunction->basicBlocks)
                        {
                            for (const auto &i2 : b2->instructions)
                            {
                                if (i2.opcode == SIROpcode::LOAD &&
                                    i2.result.has_value() && !i2.operands.empty())
                                {
                                    loadSrc[i2.result->name] = i2.operands[0].name;
                                }
                                else if (i2.opcode == SIROpcode::STORE && i2.operands.size() >= 2)
                                {
                                    // (AR) operands[1] = الوجهة، operands[0] = المصدر
                                    // (EN) operands[1] = destination, operands[0] = source
                                    storeVal[i2.operands[1].name] = i2.operands[0].name;
                                }
                                else if (i2.opcode == SIROpcode::CLOSURE_CREATE &&
                                         i2.result.has_value() && !i2.operands.empty())
                                {
                                    closureMap[i2.result->name] = i2.operands[0].name;
                                }
                            }
                        }

                        // (AR) البحث عن اسم اللامدا من خلال RET
                        // (EN) Search for lambda name via RET
                        for (const auto &block : sirFunction->basicBlocks)
                        {
                            for (const auto &inst : block->instructions)
                            {
                                if (inst.opcode == SIROpcode::RET && !inst.operands.empty())
                                {
                                    std::string retReg = inst.operands[0].name;

                                    // (AR) الحالة أ: CLOSURE_CREATE مباشر
                                    // (EN) Case A: direct CLOSURE_CREATE match
                                    auto it_c = closureMap.find(retReg);
                                    if (it_c != closureMap.end())
                                    {
                                        funcInfo.returnLambdaName = it_c->second;
                                        break;
                                    }

                                    // (AR) الحالة ب: RET %tmp ← LOAD %tmp, %var ← STORE %closure, %var
                                    // (EN) Case B: RET %tmp ← LOAD %tmp, %var ← STORE %closure, %var
                                    auto it_l = loadSrc.find(retReg);
                                    if (it_l != loadSrc.end())
                                    {
                                        const std::string &loadedVar = it_l->second;
                                        auto it_s = storeVal.find(loadedVar);
                                        if (it_s != storeVal.end())
                                        {
                                            const std::string &storedReg = it_s->second;
                                            auto it_c2 = closureMap.find(storedReg);
                                            if (it_c2 != closureMap.end())
                                            {
                                                funcInfo.returnLambdaName = it_c2->second;
                                                break;
                                            }
                                        }
                                    }

                                    break; // (AR) نفحص أول RET فقط / (EN) check first RET only
                                }
                            }
                            if (!funcInfo.returnLambdaName.empty())
                                break;
                        }
                    }

                    // (AR) [وسم زمن-التشغيل] حفظُ نوع عنصر المصفوفة المُرجعة: كتبه
                    //      buildReturnStatement في إدخال الجدول أثناء بناء الجسم (فوق)،
                    //      وهذا الإسنادُ الجديد كان يطمسه (funcInfo لا يحمله). ننقله من
                    //      الإدخال السابق قبل الاستبدال، فيبقى متاحًا لموقع الاستدعاء.
                    // (EN) [runtime tags] preserve the returned array element type:
                    //      buildReturnStatement wrote it into the table entry during body
                    //      build (above), and this fresh assignment would clobber it (funcInfo
                    //      doesn't carry it). Copy it from the prior entry before overwriting
                    //      so it stays available at the call site.
                    {
                        auto prevFtIt = functionTable_.find(funcDecl->name);
                        if (prevFtIt != functionTable_.end() &&
                            funcInfo.returnElementType == SadTypeKind::Void)
                        {
                            funcInfo.returnElementType = prevFtIt->second.returnElementType;
                        }
                    }

                    // ═════════════════════════════════════════════════════════════
                    // (AR) 🔑 وصنفُ الإرجاعِ أخو سابقِه حرفًا — وكان يُطمَسُ معه (ISSUE-140)
                    // ═════════════════════════════════════════════════════════════
                    //
                    // (AR) `returnClassName` يُكتَبُ في موضِعَين لا يملكُ أحدُهما `funcInfo`
                    //      هذه: تسجيلُ الطورِ الأوّلِ (من نوعِ الإرجاعِ المُصرَّح) و`buildReturnStatement`
                    //      (من استنتاجِ جملةِ `ارجع` أثناءَ بناءِ الجسم). وكلاهما يكتبُ
                    //      في **إدخالِ الجدول**، ثمّ يأتي هذا الإسنادُ فيستبدِلُ الإدخالَ كلَّه
                    //      بـ`funcInfo` — فيُمحى الاثنان معًا.
                    //
                    //      🔑 **والمقيسُ أنّ الطمسَ وحدَه كان يُبطِلُ النوعَ المُصرَّح.**
                    //      مُلئَ الحقلُ في الطورِ الأوّلِ فبقيَ العطبُ كما هو، ولم يزُلْ إلّا
                    //      بإضافةِ الحفظِ هنا — فالطمسُ مُثبَتٌ بفرقِ قياسٍ لا بقراءةِ الكود.
                    //
                    //      ⚠️ **ولا يُدَّعى أنّ هذا يُفسِّرُ حالةَ الإرجاعِ غيرِ المُصرَّح.** قُِيسَ
                    //      بمِجسَّينِ متتابعَينِ أنّ `ارجع شخص()` يُبنى **قبلَ** موضِعِ النداء
                    //      (فلا مشكلةَ ترتيب) وأنّ `valueResult.className` **فارغٌ أصلًا**
                    //      عندَه — أي أنّ الاستنتاجَ لم ينجحْ فيُمحى، بل لم ينجحْ قطّ. وذاك
                    //      قيدٌ مستقلٌّ يبقى مفتوحًا (ISSUE-140/الشطرُ غيرُ المُصرَّح).
                    //
                    //      ⚠️ والتعليقُ أعلاه كان يصفُ هذا الفخَّ بعينِه لحقلٍ أخَ، ولم يمتدَّ
                    //      إلى هذا الحقل — فإصلاحُ حقلٍ واحدٍ من حقولٍ يطمِسُها إسنادٌ
                    //      واحدٌ يتركُ العطبَ حيًّا في أخواتِه.
                    // (EN) MEASURED: the clobber alone defeated the DECLARED return class — filling
                    //      it at phase 1 changed nothing until this preservation was added, so the
                    //      clobber is proven by a measurement difference, not by reading the code.
                    //      ⚠️ No claim is made that this explains the UNDECLARED case: two ordered
                    //      probes measured that `return Person()` is built BEFORE the call site (so
                    //      ordering is fine) and that valueResult.className is EMPTY there — the
                    //      inference never succeeded, so it was never erased. That is a separate
                    //      open constraint. The comment above described this same trap for one
                    //      field and did not generalise: fixing one clobbered field of several
                    //      leaves the defect alive in its siblings.
                    // ═════════════════════════════════════════════════════════════
                    {
                        auto prevFtIt = functionTable_.find(funcDecl->name);
                        if (prevFtIt != functionTable_.end() &&
                            funcInfo.returnClassName.empty())
                        {
                            funcInfo.returnClassName = prevFtIt->second.returnClassName;
                        }
                    }
                    functionTable_[funcDecl->name] = funcInfo;
                }

                // (AR) إعادة تعيين الدالة الحالية
                // (EN) Reset current function
                currentFunction_ = nullptr;
                currentBlock_ = nullptr;
            }

            // ============================================================================
            // buildGlobalVariable - بناء متغير عام
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:356
            // التوقيع / Signature: void buildGlobalVariable(AST::VariableDeclNode* varDecl);
            //
            // المعاملات / Parameters:
            // - varDecl: AST::VariableDeclNode* = Sad::AST::VarDeclStmt* (sir_builder.h:59)
            //
            // VarDeclStmt Members (statements.h:74-100):
            // - name: std::string (line 76)
            // - type: Types::SadTypeKind (line 77)
            // - initializer: ExprPtr (line 78)
            // - isConst: bool (line 79)
            //
            // الدوال المستدعاة / Called functions:
            // - astTypeToSIRType: sir_builder.h:713
            // - module_->addGlobalVariable: sir_module.h:591
            // ============================================================================
            // ================================================================
            // (AR) طيُّ مُهيِّئٍ حرفيٍّ مركّبٍ لثابتٍ عامّ.
            //      يُعيد `true` مع النصِّ الجاهزِ للانبعاث، و`false` إن لم يكن التعبيرُ
            //      حرفيًّا بحتًا (متغيّر، نداء، ...) فيبقى القرارُ للمنادي.
            //      المدعوم: أحاديُّ `+`/`-`، وثنائيُّ `+ - * /` على حرفيّاتٍ عدديّة،
            //      ووصلُ `+` على حرفيّاتٍ نصّيّة. والقسمةُ على صفرٍ **لا تُطوى**:
            //      تُترَك للمسارِ العامِّ بدل أن نُثبِّت في العامِّ قيمةً مختلَقة.
            // (EN) Fold a composite literal initializer for a module-level constant.
            //      True + emit-ready text, or false when the expression is not purely
            //      literal. Supported: unary +/-, binary + - * / over numeric literals,
            //      and + over string literals. Division by zero is left UNFOLDED rather
            //      than inventing a value for the global.
            // ================================================================
            namespace
            {
                struct FoldedLiteral
                {
                    bool isString = false;
                    bool isFloat = false;
                    long long integerValue = 0;
                    double floatValue = 0.0;
                    std::string stringValue;
                };

                // (AR) حسابٌ صحيحٌ مُدقَّقُ الفيض — مكتوبٌ يدويًّا لأنّ MSVC لا يعرف
                //      `__builtin_*_overflow`. والفيضُ **لا يُطوى** بقيمةٍ ملفوفةٍ بل
                //      يُترَك للمسارِ العامّ: قيمةٌ ملفوفةٌ في العامِّ تخالف المفسّرَ
                //      صامتةً، وهو أسوأُ من ألّا نطوي.
                // (EN) Hand-written overflow-checked integer arithmetic — MSVC has no
                //      __builtin_*_overflow. An overflowing expression is left UNFOLDED
                //      rather than folded to a wrapped value that would silently disagree
                //      with the interpreter.
                constexpr long long kFoldMax = (std::numeric_limits<long long>::max)();
                constexpr long long kFoldMin = (std::numeric_limits<long long>::min)();

                bool addChecked(long long a, long long b, long long &out)
                {
                    if ((b > 0 && a > kFoldMax - b) || (b < 0 && a < kFoldMin - b))
                        return false;
                    out = a + b;
                    return true;
                }

                bool subChecked(long long a, long long b, long long &out)
                {
                    if ((b < 0 && a > kFoldMax + b) || (b > 0 && a < kFoldMin + b))
                        return false;
                    out = a - b;
                    return true;
                }

                bool mulChecked(long long a, long long b, long long &out)
                {
                    if (a == 0 || b == 0) { out = 0; return true; }
                    if (a == -1 && b == kFoldMin) return false;
                    if (b == -1 && a == kFoldMin) return false;
                    const bool overflows = a > 0
                        ? (b > 0 ? a > kFoldMax / b : b < kFoldMin / a)
                        : (b > 0 ? a < kFoldMin / b : a < kFoldMax / b);
                    if (overflows)
                        return false;
                    out = a * b;
                    return true;
                }

                bool foldLiteral(Sad::AST::Expression *expr, FoldedLiteral &out);

                bool foldLiteralNode(Sad::AST::LiteralExpr *lit, FoldedLiteral &out)
                {
                    const auto &token = lit->token;
                    std::string value = token.getValue();
                    switch (token.getType())
                    {
                    case Lexer::TokenType::NUMBER_INTEGER:
                    {
                        int base = 10;
                        std::string digits = value;
                        if (value.size() > 2 && value[0] == '0')
                        {
                            const char prefix = value[1];
                            if (prefix == 'x' || prefix == 'X') { base = 16; }
                            else if (prefix == 'o' || prefix == 'O') { base = 8; digits = value.substr(2); }
                            else if (prefix == 'b' || prefix == 'B') { base = 2; digits = value.substr(2); }
                        }
                        try { out.integerValue = static_cast<long long>(std::stoull(digits, nullptr, base)); }
                        catch (...) { return false; }
                        out.isFloat = false;
                        out.isString = false;
                        return true;
                    }
                    case Lexer::TokenType::NUMBER_DOUBLE:
                        try { out.floatValue = std::stod(value); }
                        catch (...) { return false; }
                        out.isFloat = true;
                        out.isString = false;
                        return true;
                    case Lexer::TokenType::LITERAL_TRUE:
                        out.integerValue = 1; out.isFloat = false; out.isString = false; return true;
                    case Lexer::TokenType::LITERAL_FALSE:
                        out.integerValue = 0; out.isFloat = false; out.isString = false; return true;
                    case Lexer::TokenType::STRING_LITERAL:
                        out.stringValue = value; out.isString = true; return true;
                    default:
                        return false;
                    }
                }

                bool foldLiteral(Sad::AST::Expression *expr, FoldedLiteral &out)
                {
                    if (!expr)
                        return false;
                    if (auto *lit = dynamic_cast<Sad::AST::LiteralExpr *>(expr))
                        return foldLiteralNode(lit, out);
                    if (auto *un = dynamic_cast<Sad::AST::UnaryExpr *>(expr))
                    {
                        if (un->op != Lexer::TokenType::OP_PLUS && un->op != Lexer::TokenType::OP_MINUS)
                            return false;
                        if (!foldLiteral(un->operand.get(), out) || out.isString)
                            return false;
                        if (un->op == Lexer::TokenType::OP_MINUS)
                        {
                            if (out.isFloat) out.floatValue = -out.floatValue;
                            else out.integerValue = -out.integerValue;
                        }
                        return true;
                    }
                    if (auto *bin = dynamic_cast<Sad::AST::BinaryExpr *>(expr))
                    {
                        FoldedLiteral lhs, rhs;
                        if (!foldLiteral(bin->left.get(), lhs) || !foldLiteral(bin->right.get(), rhs))
                            return false;
                        if (lhs.isString || rhs.isString)
                        {
                            if (!lhs.isString || !rhs.isString || bin->op != Lexer::TokenType::OP_PLUS)
                                return false;
                            out.isString = true;
                            out.stringValue = lhs.stringValue + rhs.stringValue;
                            return true;
                        }
                        const bool asFloat = lhs.isFloat || rhs.isFloat;
                        const double a = lhs.isFloat ? lhs.floatValue : static_cast<double>(lhs.integerValue);
                        const double c = rhs.isFloat ? rhs.floatValue : static_cast<double>(rhs.integerValue);
                        out.isString = false;
                        out.isFloat = asFloat;
                        // (AR) الحسابُ الصحيحُ يُجرى **بمُدقِّقاتِ الفيض** لا خامًّا: فيضُ
                        //      الصحيحِ المُوقَّعِ سلوكٌ غيرُ معرَّفٍ في C++، وقسمةُ
                        //      `LLONG_MIN / -1` تُطلِق فخَّ العتادِ فينهار **المصرِّفُ
                        //      نفسُه** بـEXCEPTION_INT_OVERFLOW بلا تشخيصٍ واحدٍ ولا
                        //      ملفَّ ناتج — قِيس على `ثابت س = -9223372036854775808 / -1`.
                        //      والفائضُ لا يُطوى بقيمةٍ ملفوفةٍ بل **لا يُطوى أصلًا**:
                        //      يُترَك للمسارِ العامِّ فيقرّره كما يقرّر أيَّ تعبيرٍ آخر،
                        //      بدل أن نُثبِّت في العامِّ عددًا لا يوافق ما يعطيه المفسّر.
                        // (EN) Integer arithmetic goes through OVERFLOW-CHECKED builtins:
                        //      signed overflow is UB, and LLONG_MIN / -1 raises the hardware
                        //      trap, crashing the COMPILER itself with EXCEPTION_INT_OVERFLOW
                        //      — no diagnostic, no output file (measured). An overflowing
                        //      expression is left UNFOLDED rather than folded to a wrapped
                        //      value that would not match the interpreter.
                        switch (bin->op)
                        {
                        case Lexer::TokenType::OP_PLUS:
                            if (asFloat) { out.floatValue = a + c; }
                            else if (!addChecked(lhs.integerValue, rhs.integerValue,
                                                 out.integerValue)) { return false; }
                            return true;
                        case Lexer::TokenType::OP_MINUS:
                            if (asFloat) { out.floatValue = a - c; }
                            else if (!subChecked(lhs.integerValue, rhs.integerValue,
                                                 out.integerValue)) { return false; }
                            return true;
                        case Lexer::TokenType::OP_MULTIPLY:
                            if (asFloat) { out.floatValue = a * c; }
                            else if (!mulChecked(lhs.integerValue, rhs.integerValue,
                                                 out.integerValue)) { return false; }
                            return true;
                        case Lexer::TokenType::OP_DIVIDE:
                            if (asFloat)
                            {
                                if (c == 0.0) return false;
                                out.floatValue = a / c;
                            }
                            else
                            {
                                if (rhs.integerValue == 0)
                                    return false;
                                // (AR) الحالةُ الوحيدةُ التي تُفلِت من فحصِ الصفر.
                                if (rhs.integerValue == -1 &&
                                    lhs.integerValue == (std::numeric_limits<long long>::min)())
                                    return false;
                                out.integerValue = lhs.integerValue / rhs.integerValue;
                            }
                            return true;
                        default:
                            return false;
                        }
                    }
                    return false;
                }

                std::string formatFoldedDouble(double value)
                {
                    std::ostringstream oss;
                    oss.precision(17);
                    oss << value;
                    std::string text = oss.str();
                    if (text.find('.') == std::string::npos &&
                        text.find('e') == std::string::npos &&
                        text.find('E') == std::string::npos &&
                        text.find("inf") == std::string::npos &&
                        text.find("nan") == std::string::npos)
                    {
                        text += ".0";
                    }
                    return text;
                }
            } // namespace

            bool SIRBuilder::tryFoldLiteralInitializer(Sad::AST::Expression *expr, std::string &out,
                                                       SadTypeKind &outKind)
            {
                // (AR) الحرفيّةُ المفردةُ تُترَك للمسارِ القديمِ عمدًا: هو يحمل تطبيعَ
                //      الأساساتِ ودلالاتٍ مستقرّةً منذ زمن، فلا يُستبدَل بلا داعٍ.
                //      الطيُّ هنا للصيغِ المركّبةِ التي كانت تُسقَط بصمت.
                // (EN) A lone literal is deliberately left to the legacy path (it carries
                //      base normalization and long-settled semantics). This folder exists
                //      for the composite forms that used to be silently dropped.
                if (!expr || dynamic_cast<Sad::AST::LiteralExpr *>(expr))
                    return false;
                FoldedLiteral folded;
                if (!foldLiteral(expr, folded))
                    return false;
                if (folded.isString)
                {
                    out = folded.stringValue;
                    outKind = SadTypeKind::String;
                }
                else if (folded.isFloat)
                {
                    out = formatFoldedDouble(folded.floatValue);
                    outKind = SadTypeKind::Float;
                }
                else
                {
                    out = std::to_string(folded.integerValue);
                    outKind = SadTypeKind::Integer;
                }
                return true;
            }

            void SIRBuilder::buildGlobalVariable(AST::VariableDeclNode *varDecl)
            {
                if (!varDecl)
                {
                    return;
                }

                // (AR) تحويل النوع — عبر سلطةِ الخانةِ كي يفكَّ `T؟` هنا أيضًا؛ كان هذا
                //      المسارُ يستدعي astTypeToSIRType مباشرةً فيسقط العدميُّ إلى الافتراضيّ.
                // (EN) Convert the type through the storage authority so `T?` is unwrapped here
                //      too; this path called astTypeToSIRType directly, so nullables fell
                //      through to the default arm.
                SadTypeKind varType = resolveDeclaredStorageKind(varDecl->type,
                                                                 varDecl->sadType.get(),
                                                                 astTypeToSIRType(varDecl->type));
                // (AR) إذا كان النوع UNKNOWN (افتراضي Integer)، نستنتجه من المُهيئ
                //      هذا ضروري لمتغيرات الفضاء مثل: متغير PI = 3.14159
                //      حيث لا يُحدد النوع صراحة ويجب استنتاجه من القيمة الحرفية
                // (EN) If type is UNKNOWN (defaults to Integer), infer from initializer
                //      Needed for namespace vars like: var PI = 3.14159
                //      where type is not explicit and must be inferred from literal value
                // (AR) الطيُّ يسبق الاستنتاج: نوعُ الثابتِ يُستنتَج من **قيمتِه المطويّة**
                //      لا من كونِ المُهيِّئِ حرفيّةً مفردةً. وإلّا استُنتِج `ثابت س = -2.5`
                //      عددًا صحيحًا فمرّت قيمتُه على `stoll` فصارت ‎-2.0، و`"أ" + "ب"`
                //      عددًا صحيحًا فرُمي التحويلُ فبقيت `(null)` — كلاهما مقيسٌ.
                // (EN) Fold before inferring: a constant's type comes from its FOLDED value,
                //      not from whether the initializer is a lone literal. Otherwise
                //      `ثابت س = -2.5` inferred Integer, went through stoll and became -2.0,
                //      and `"أ" + "ب"` inferred Integer, threw on conversion, and stayed
                //      "(null)" — both measured.
                std::string foldedValue;
                SadTypeKind foldedKind = SadTypeKind::Unknown;
                const bool didFold =
                    varDecl->initializer &&
                    tryFoldLiteralInitializer(varDecl->initializer.get(), foldedValue, foldedKind);
                if (varDecl->type == Types::SadTypeKind::Unknown && didFold)
                {
                    varType = foldedKind;
                }
                else if (varDecl->type == Types::SadTypeKind::Unknown && varDecl->initializer)
                {
                    if (auto *litExpr = dynamic_cast<Sad::AST::LiteralExpr *>(varDecl->initializer.get()))
                    {
                        Lexer::TokenType tokType = litExpr->token.getType();
                        if (tokType == Lexer::TokenType::NUMBER_DOUBLE)
                            varType = SadTypeKind::Float;
                        else if (tokType == Lexer::TokenType::STRING_LITERAL)
                            varType = SadTypeKind::String;
                        else if (tokType == Lexer::TokenType::LITERAL_TRUE ||
                                 tokType == Lexer::TokenType::LITERAL_FALSE)
                            varType = SadTypeKind::Boolean;
                    }
                }

                // (AR) بابُ ISSUE-138 نفسُه — المسارُ الثالثُ لنوعِ خانةِ التصريح.
                // (EN) The same ISSUE-138 door — the third declaration-kind path.
                varType = resolveBareSlotStorageKind(
                    varDecl->type, varDecl->initializer != nullptr, varType);

                // (AR) إنشاء متغير عام (SIRGlobalVariable constructor: sir_module.h:96)
                // (EN) Create global variable
                auto sirGlobal = std::make_shared<SIRGlobalVariable>(varDecl->name, varType);

                // (AR) معالجة القيمة الأولية إذا كانت ثابتاً حرفياً
                // (EN) Handle initializer if it's a literal constant
                if (varDecl->initializer)
                {
                    // (AR) 🔴 كلُّ صيغةٍ لا يفهمها هذا الفرعُ كانت **تُسقَط بصمت**: يبقى
                    //      العامُّ `zeroinitializer` فيُقرأ صفرًا (أو `(null)` للنصّ)
                    //      برمزِ خروجٍ ٠ وبلا تشخيصٍ واحد. وأبسطُ صيغةٍ تقع فيها
                    //      `ثابت س = -1` — سالبٌ = `UnaryExpr` لا `LiteralExpr` —
                    //      وكذلك `1 + 2` و`"أ" + "ب"`. قِيس الثلاثةُ فأعطت 0 و0 و(null)
                    //      بينما يعطي المفسّرُ ‎-1 و3 و«أب». فطُوِيت الصيغُ الحرفيّةُ
                    //      المركّبةُ هنا بدل إسقاطِها.
                    // (EN) 🔴 Any form this branch did not understand was SILENTLY dropped:
                    //      the global stayed zeroinitializer and read back as 0 (or "(null)"
                    //      for a string) with exit 0 and no diagnostic. The simplest case is
                    //      `ثابت س = -1` — a UnaryExpr, not a LiteralExpr — and likewise
                    //      `1 + 2` and `"أ" + "ب"`. All three measured 0/0/(null) compiled
                    //      against -1/3/«أب» interpreted. Composite literal forms are now
                    //      folded here instead of dropped.
                    if (didFold)
                    {
                        sirGlobal->initialValue = foldedValue;
                        sirGlobal->hasInitialValue = true;
                        sirGlobal->initialValueKind = foldedKind;
                    }
                    else if (auto *litExpr = dynamic_cast<Sad::AST::LiteralExpr *>(varDecl->initializer.get()))
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
                        }
                        else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE)
                        {
                            sirGlobal->initialValue = value;
                        }
                        else if (tokenType == Lexer::TokenType::LITERAL_TRUE)
                        {
                            sirGlobal->initialValue = "1";
                        }
                        else if (tokenType == Lexer::TokenType::LITERAL_FALSE)
                        {
                            sirGlobal->initialValue = "0";
                        }
                        else if (tokenType == Lexer::TokenType::STRING_LITERAL)
                        {
                            sirGlobal->initialValue = value;
                        }

                        // (AR) يُرفَع العَلَمُ للأذرعِ المُسنِدةِ وحدَها — لا لكلِّ حرفيّة؛ فالحرفيّةُ
                        //      التي لا ذراعَ لها (مثل «لاشيء») تبقى بلا قيمةٍ أوليّةٍ كما كانت.
                        //      ورفعُه لازمٌ للنصِّ الفارغ `""` خاصّةً: نصُّه فارغٌ ووجودُه حقيقيّ،
                        //      ولو اشتُقّ الوجودُ من النصِّ لَعُدَّ غيابًا فصُفِّر مؤشّرُه فطُبع «void».
                        // (EN) The flag is raised only for the arms that actually assigned — not for
                        //      every literal; a literal with no arm (e.g. «لاشيء») stays value-less as
                        //      before. It matters most for the empty string `""`: its text is empty but
                        //      its presence is real; deriving presence from the text zeroed its pointer
                        //      and printed "void".
                        if (tokenType == Lexer::TokenType::NUMBER_INTEGER ||
                            tokenType == Lexer::TokenType::NUMBER_DOUBLE ||
                            tokenType == Lexer::TokenType::LITERAL_TRUE ||
                            tokenType == Lexer::TokenType::LITERAL_FALSE ||
                            tokenType == Lexer::TokenType::STRING_LITERAL)
                        {
                            sirGlobal->hasInitialValue = true;
                        }

                        // (AR) ونوعُ القيمةِ يُسجَّل من **رمزِ الحرفيّة** لا من النوعِ
                        //      المُصرَّح: هما يفترقان في `أي` وحدَها اليومَ، والأذرعُ
                        //      أعلاه سوّت «صحيح» و«1» في النصِّ فلم يبقَ ما يفرّقهما.
                        // (EN) Record the value kind from the LITERAL token, not the
                        //      declared type: they diverge for `أي`, and the arms above
                        //      collapse «صحيح» and 1 into the same text.
                        switch (tokenType)
                        {
                        case Lexer::TokenType::NUMBER_INTEGER:
                            sirGlobal->initialValueKind = SadTypeKind::Integer;
                            break;
                        case Lexer::TokenType::NUMBER_DOUBLE:
                            sirGlobal->initialValueKind = SadTypeKind::Float;
                            break;
                        case Lexer::TokenType::LITERAL_TRUE:
                        case Lexer::TokenType::LITERAL_FALSE:
                            sirGlobal->initialValueKind = SadTypeKind::Boolean;
                            break;
                        case Lexer::TokenType::STRING_LITERAL:
                            sirGlobal->initialValueKind = SadTypeKind::String;
                            break;
                        default:
                            break;
                        }
                    }
                }

                // (AR) إضافة المتغير العام للوحدة (sir_module.h:591 - addGlobalVariable)
                // (EN) Add global variable to module
                module_->addGlobalVariable(sirGlobal);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
