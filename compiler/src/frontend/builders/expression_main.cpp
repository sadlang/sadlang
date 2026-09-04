// ============================================================================
// sir_builder_expressions_dispatch.cpp
// ============================================================================
#include <string>
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include "sad_debug_log.h"
#include "lexer_keywords.h"
#include "error_manager.h"
#include "error_codes.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <limits>
#include <functional>
#include <set>
namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // (AR) تشخيصُ «هذا/الأساس غير متاح» (SEM015) من كتالوجِ الأخطاءِ ومعجمِ
            //      الكلماتِ كليهما — لا نصًّا مكتوبًا هنا. فالتهجئةُ تأتي من معجمِ
            //      مصدرِ الحقيقة كي لا يقرأَ المستعمِلُ كلمةً لا يستطيعُ كتابتَها.
            // (EN) The SEM015 diagnostic comes from the error catalog and the keyword
            //      lexicon alike — never hard-coded, so the reader is never shown a
            //      spelling the language does not accept.
            static std::string thisSuperUnavailableMessage(Lexer::TokenType keywordType)
            {
                const auto *entry = Lexer::KeywordTable::getEntry(keywordType);
                Sad::Errors::RenderContext context;
                context.placeholders = {
                    {"keyword", entry ? entry->primaryWord : std::string()}};
                return Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                    Sad::Errors::ErrorCode::SEM_THIS_SUPER_UNAVAILABLE, context);
            }

            // ============================================================================
            // EXPRESSION BUILDING - بناء التعابير
            // ============================================================================

            // ============================================================================
            // buildExpression - بناء تعبير (موزع للأنواع المختلفة)
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:432
            // التوقيع / Signature: BuildResult buildExpression(AST::ExpressionNode* expr);
            //
            // المعاملات / Parameters:
            // - expr: AST::ExpressionNode* = Sad::AST::Expression* (sir_builder.h:53)
            //
            // الإرجاع / Returns:
            // - BuildResult: sir_builder.h:103 (contains registerName, type)
            //
            // Expression Types (expressions.h):
            // - BinaryExpr (line 40): left, op, right
            // - UnaryExpr (line 78): op, operand
            // - LiteralExpr (line 174): token
            // - VariableExpr (line 206): name
            // - CallExpr (line 276): callee, arguments
            // - MemberExpr (line 353): object, member
            // ============================================================================
            BuildResult ExpressionBuilder::buildExpression(AST::ExpressionNode *expr)
            {
                if (!expr)
                {
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: expr is null!");
#endif
                    return BuildResult();
                }

#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: processing expression of type: " << typeid(*expr).name());
#endif

                // (AR) BinaryExpr - تعبير ثنائي (expressions.h:40)
                // (EN) Binary expression
                if (auto binaryExpr = dynamic_cast<Sad::AST::BinaryExpr *>(expr))
                {
                    return buildBinaryOp(binaryExpr);
                }

                // (AR) UnaryExpr - تعبير أحادي (expressions.h:78)
                // (EN) Unary expression
                if (auto unaryExpr = dynamic_cast<Sad::AST::UnaryExpr *>(expr))
                {
                    return buildUnaryOp(unaryExpr);
                }

                // (AR) LiteralExpr - ثابت حرفي (expressions.h:174)
                // (EN) Literal
                if (auto literalExpr = dynamic_cast<Sad::AST::LiteralExpr *>(expr))
                {
                    // ================================================================
                    // (AR) إصلاح #43: الكلمات المحجوزة المستخدمة كأسماء متغيرات
                    //      في سياق catch: `امسك خطأ` يُسجّل `خطأ` كمتغير.
                    //      لكن عند استخدامه في تعبير، الـ Lexer يقرأه كـ LITERAL_FALSE.
                    //      الحل: إذا كان LITERAL_FALSE/TRUE/NULL وهناك متغير بنفس الاسم
                    //      العربي في النطاق الحالي → استخدمه كمتغير بدلاً من ثابت.
                    //      الأسماء: خطأ → LITERAL_FALSE, صحيح → LITERAL_TRUE, لاشيء → LITERAL_NULL
                    // (EN) Fix #43: Reserved words used as variable names
                    //      In catch context: `catch error` registers `error` as variable.
                    //      But when used in expression, Lexer reads it as LITERAL_FALSE.
                    //      Fix: If LITERAL_FALSE/TRUE/NULL and a variable with the same
                    //      Arabic name exists in scope → use as variable instead of literal.
                    // ================================================================
                    auto tt = literalExpr->token.getType();
                    std::string possibleVarName;
                    if (tt == Lexer::TokenType::LITERAL_FALSE)
                        possibleVarName = "\xd8\xae\xd8\xb7\xd8\xa3"; // خطأ
                    else if (tt == Lexer::TokenType::LITERAL_TRUE)
                        possibleVarName = "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad"; // صحيح
                    else if (tt == Lexer::TokenType::LITERAL_NULL)
                        possibleVarName = "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1"; // لاشيء

                    if (!possibleVarName.empty())
                    {
                        VariableInfo *varInfo = b_.lookupVariable(possibleVarName);
                        if (varInfo)
                        {
                            // (AR) وُجد متغير بهذا الاسم → استخدمه كمتغير
                            // (EN) Found variable with this name → use as variable
                            return BuildResult(varInfo->registerName, varInfo->type);
                        }
                    }

                    return buildLiteral(literalExpr);
                }

                // (AR) VariableExpr - متغير (expressions.h:206)
                // (EN) Variable
                if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr *>(expr))
                {
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found VariableExpr");
#endif
                    return buildVariableAccess(varExpr);
                }

                // (AR) CallExpr - استدعاء دالة (expressions.h:276)
                // (EN) Function call
                if (auto callExpr = dynamic_cast<Sad::AST::CallExpr *>(expr))
                {
                    return b_.buildFunctionCall(callExpr);
                }

                // (AR) AssignExpr - إسناد متغير (expressions.h:247)
                // (EN) Variable assignment
                if (auto assignExpr = dynamic_cast<Sad::AST::AssignExpr *>(expr))
                {
                    b_.buildAssignment(assignExpr);
                    // (AR) بعد الإسناد، نرجع معلومات المتغير المُسند إليه
                    // (EN) After assignment, return the assigned variable info
                    VariableInfo *varInfo = b_.lookupVariable(assignExpr->name);
                    if (varInfo)
                    {
                        return BuildResult(varInfo->registerName, varInfo->type);
                    }
                    return BuildResult();
                }

                // (AR) NewExpr - إنشاء كائن جديد (class_nodes.h:164)
                // (EN) New object creation
                if (auto newExpr = dynamic_cast<Sad::AST::NewExpr *>(expr))
                {
                    return buildNewObject(newExpr);
                }
                if (auto memberExpr = dynamic_cast<Sad::AST::MemberExpr *>(expr))
                {
                    return buildExprMember(memberExpr);
                }

                // (AR) MemberAccessExpr - الوصول لعضو في كائن (class_nodes.h:206)
                // (EN) Member access
                if (auto memberExpr = dynamic_cast<Sad::AST::MemberAccessExpr *>(expr))
                {
                    return buildMemberAccess(memberExpr);
                }

                // (AR) MethodCallExpr - استدعاء طريقة على كائن (class_nodes.h:245)
                // (EN) Method call on object
                if (auto methodCallExpr = dynamic_cast<Sad::AST::MethodCallExpr *>(expr))
                {
                    // (AR) 🔑 العَلَمُ يُشرَّف هنا **قبلَ** التوزيعِ العاديّ. ولو تُرِك
                    //      لَبَنَى المترجِمُ نداءً غيرَ آمنٍ لعقدةٍ مكتوبةٍ آمنةً:
                    //      يعمل على المستقبِلِ الحيِّ ويرفع RUN033 على العدمِ بينما
                    //      يُرجِع المفسّرُ «لاشيء» — تباعُدُ محرّكَين يُصنَع بإضافةِ
                    //      حقلٍ ونسيانِ قارئِه، وهو عينُ «العَلَمُ يلزمه كلُّ مُنتِجيه».
                    // (EN) Honour the flag BEFORE the normal dispatch. Left out, the
                    //      compiler would build an UNSAFE call for a node written safe:
                    //      correct on a live receiver, raising RUN033 on null while the
                    //      interpreter returns «لاشيء» — a two-engine divergence created
                    //      by adding a field and forgetting its reader.
                    if (methodCallExpr->isOptional)
                    {
                        return buildExprOptionalMethodCall(methodCallExpr);
                    }
                    return b_.buildMethodCall(methodCallExpr);
                }

                // ================================================================
                // (AR) 🔑 SEM015 — «هذا»/«الأساس» خارجَ سياقٍ يتيحُهما.
                //
                //      الرمزُ **مُعلَنٌ في مصدرِ الحقيقةِ ولم يُبعَثْ قطُّ**. وكان
                //      الفرعانِ أدناه يسقطانِ إلى `%#self` حين لا يجدانِ الخانة،
                //      فيُبثُّ مرجعٌ إلى سجلٍّ **لا وجودَ له**، فتنهارُ الخلفيّةُ
                //      بـ«مرجع غير معرَّف (Object not found:%#self)» — خطأٌ داخليٌّ
                //      يطلبُ الإبلاغَ عن علّةِ مترجِمٍ، والعلّةُ في البرنامج.
                //      وقِيسَ أنّ الخروجَ كان **صفرًا** مع ثنائيٍّ يُسلَّم.
                //
                //      والشرطُ مزدوجٌ عمدًا: لا خانةَ `self` **ولا اسمَ صنفٍ جارٍ**.
                //      فالسقوطُ إلى `%#self` داخلَ صنفٍ قد يكون مشروعًا (البانيات
                //      تُعرّفه بغيرِ تسجيلِه متغيّرًا)، وخارجَ الصنفِ لا يكون أبدًا.
                // (EN) SEM015 — this/super outside a context that provides them. The
                //      code was declared in the SoT and never emitted: both branches
                //      fell back to %#self, emitting a reference to a register that
                //      does not exist, so the backend crashed with an INTERNAL error
                //      ("Object not found:%#self") — and exited 0 with a binary.
                //      The condition is deliberately double: no self slot AND no
                //      enclosing class, since the fallback can be legitimate inside a
                //      class (constructors define it without registering a variable).
                // ================================================================
                // (AR) ThisExpr - مرجع ذاتي 'هذا' (class_nodes.h:288)
                // (EN) This/self reference
                if (auto thisExpr = dynamic_cast<Sad::AST::ThisExpr *>(expr))
                {
                    // (AR) 'هذا' يشير إلى self في سياق الصنف
                    // (EN) 'this' refers to self in class context
                    VariableInfo *selfInfo = b_.lookupVariable(kSelfParamName);
                    if (selfInfo)
                    {
                        return BuildResult(selfInfo->registerName, selfInfo->type);
                    }
                    if (b_.currentClassName_.empty())
                    {
                        b_.errors_.push_back(
                            thisSuperUnavailableMessage(Lexer::TokenType::KEYWORD_THIS));
                        return BuildResult();
                    }
                    return BuildResult(kSelfRegisterName, SadTypeKind::Integer);
                }

                // (AR) SuperExpr - مرجع الصنف الأب 'الأساس' (class_nodes.h:298)
                // (EN) Super/base class reference
                if (auto superExpr = dynamic_cast<Sad::AST::SuperExpr *>(expr))
                {
                    // (AR) في سياق المترجم، الأساس يشير إلى self (لأن الكائن واحد)
                    // (EN) In compiler context, super refers to self (single object)
                    VariableInfo *selfInfo = b_.lookupVariable(kSelfParamName);
                    if (selfInfo)
                    {
                        return BuildResult(selfInfo->registerName, selfInfo->type);
                    }
                    if (b_.currentClassName_.empty())
                    {
                        b_.errors_.push_back(
                            thisSuperUnavailableMessage(Lexer::TokenType::KEYWORD_SUPER));
                        return BuildResult();
                    }
                    return BuildResult(kSelfRegisterName, SadTypeKind::Integer);
                }
                if (auto ternaryExpr = dynamic_cast<Sad::AST::TernaryExpr *>(expr))
                {
                    return buildExprTernary(ternaryExpr);
                }
                if (auto indexExpr = dynamic_cast<Sad::AST::IndexExpr *>(expr))
                {
                    return buildExprIndex(indexExpr);
                }
                if (auto memberAssignExpr = dynamic_cast<Sad::AST::MemberAssignExpr *>(expr))
                {
                    return buildExprMemberAssign(memberAssignExpr);
                }
                if (auto indexAssignExpr = dynamic_cast<Sad::AST::IndexAssignExpr *>(expr))
                {
                    return buildExprIndexAssign(indexAssignExpr);
                }
                if (auto arrayExpr = dynamic_cast<Sad::AST::ArrayExpr *>(expr))
                {
                    return buildExprArray(arrayExpr);
                }
                if (auto tupleExpr = dynamic_cast<Sad::AST::TupleExpr *>(expr))
                {
                    return buildExprTuple(tupleExpr);
                }
                if (auto mapExpr = dynamic_cast<Sad::AST::MapExpr *>(expr))
                {
                    return buildExprMap(mapExpr);
                }
                if (auto walrusExpr = dynamic_cast<Sad::AST::WalrusExpr *>(expr))
                {
                    return buildExprWalrus(walrusExpr);
                }
                if (auto lambdaExpr = dynamic_cast<Sad::AST::LambdaExpr *>(expr))
                {
                    return buildExprLambda(lambdaExpr);
                }
                if (auto rangeExpr = dynamic_cast<Sad::AST::RangeExpr *>(expr))
                {
                    return buildExprRange(rangeExpr);
                }
                if (auto listCompExpr = dynamic_cast<Sad::AST::ListComprehensionExpr *>(expr))
                {
                    return buildExprListComp(listCompExpr);
                }
                if (auto dictCompExpr = dynamic_cast<Sad::AST::DictComprehensionExpr *>(expr))
                {
                    return buildExprDictComp(dictCompExpr);
                }
                if (auto setCompExpr = dynamic_cast<Sad::AST::SetComprehensionExpr *>(expr))
                {
                    return buildExprSetComp(setCompExpr);
                }
                // ========================================================================
                // (AR) DecoratorExpr - مُزخرف: @decorator
                // (EN) Decorator expression: @decorator
                // ========================================================================
                if (auto decoratorExpr = dynamic_cast<Sad::AST::DecoratorExpr *>(expr))
                {
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found DecoratorExpr: " << decoratorExpr->name);
#endif

                    // (AR) المُزخرف يُحوّل لاستدعاء دالة بالاسم
                    // (EN) Decorator is lowered to a function call by name
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CALL;
                    callInst.result = SIROperand::Register(resultReg, SadTypeKind::Function);
                    callInst.operands.push_back(SIROperand::ConstantString(decoratorExpr->name));

                    // (AR) إضافة الوسائط إن وُجدت
                    // (EN) Add arguments if present
                    if (decoratorExpr->hasArguments)
                    {
                        for (const auto &arg : decoratorExpr->arguments)
                        {
                            auto argResult = buildExpression(arg.get());
                            callInst.operands.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                        }
                    }
                    callInst.comment = "decorator: " + decoratorExpr->name;

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(callInst);
                    }

                    return BuildResult(resultReg, SadTypeKind::Function);
                }

                // ========================================================================
                // (AR) BorrowExpr - استعارة: &متغير أو &mut متغير
                // (EN) Borrow expression: &var or &mut var
                // ========================================================================
                if (auto borrowExpr = dynamic_cast<Sad::AST::BorrowExpr *>(expr))
                {
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found BorrowExpr: " << borrowExpr->variableName);
#endif

                    // (AR) البحث عن المتغير المُستعار
                    // (EN) Look up the borrowed variable
                    VariableInfo *varInfo = b_.lookupVariable(borrowExpr->variableName);
                    std::string sourceReg = varInfo ? varInfo->registerName : "%" + borrowExpr->variableName;
                    SadTypeKind sourceType = varInfo ? varInfo->type : SadTypeKind::Integer;

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction borrowInst;
                    // (AR) استعارة تُترجم إلى أخذ عنوان (ADDR) — SIR لا يحتوي BORROW مباشرة
                    // (EN) Borrow lowered to address-of (ADDR) — SIR has no direct BORROW opcode
                    borrowInst.opcode = SIROpcode::ADDR;
                    borrowInst.result = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    borrowInst.operands.push_back(SIROperand::Register(sourceReg, sourceType));
                    borrowInst.comment = borrowExpr->isMutable ? "borrow_mut" : "borrow";

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(borrowInst);
                    }

                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // ========================================================================
                // (AR) TemplateInstantiation - إنشاء نسخة من قالب: صندوق<رقم>(42)
                // (EN) Template instantiation: Box<int>(42)
                // ========================================================================
                if (auto templateInst = dynamic_cast<Sad::AST::TemplateInstantiation *>(expr))
                {
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found TemplateInstantiation: "
                              << templateInst->templateName);
#endif

                    // (AR) تحويل أنواع القالب إلى SadTypeKind
                    // (EN) Convert template type arguments to SadTypeKind
                    std::vector<SadTypeKind> typeArgs;
                    for (const auto &arg : templateInst->typeArguments)
                    {
                        typeArgs.push_back(b_.astTypeToSIRType(arg));
                    }

                    // (AR) محاولة إنشاء نسخة من القالب
                    // (EN) Attempt to instantiate the template
                    std::string instName = b_.instantiateTemplate(templateInst->templateName, typeArgs);

                    // (AR) استدعاء الدالة/الصنف المُنشأ بدون وسائط (القالب لا يحمل وسائط قيم)
                    // (EN) Call the instantiated function/class with no args (template has no value args)
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction callInst = SIRInstruction::Call(
                        SIROperand::Register(resultReg, SadTypeKind::Integer),
                        SIROperand::ConstantString(instName),
                        {});

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(callInst);
                    }

                    return BuildResult(resultReg, SadTypeKind::Integer);
                }
                if (auto inlineAsm = dynamic_cast<Sad::AST::InlineAsmExpr *>(expr))
                {
                    return buildExprInlineAsm(inlineAsm);
                }

                // ========================================================================
                // (AR) AwaitExpr - تعبير الانتظار (advanced_expr_nodes.h:AwaitExpr)
                // (EN) Await expression — lowered to LLVM coroutine suspend/resume
                // ========================================================================
                if (auto awaitExpr = dynamic_cast<Sad::AST::AwaitExpr *>(expr))
                {
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found AwaitExpr");
#endif

                    // ════════════════════════════════════════════════════════
                    // (AR) 🔑 نوعُ قيمةِ الوعدِ يُشتقُّ من **المُنادَى** لا من نوعِ
                    //      السجلِّ العائد: الكوروتينُ يُرجِعُ مقبضَ إطارٍ دائمًا،
                    //      والقيمةُ في `coro.promise` بعرضِ i64 لا يحمل نوعًا.
                    //      فبلا هذا الاشتقاق يُقرأُ الوعدُ صحيحًا مهما كان: قِيس أنّ
                    //      `ارجع 2.5` تُعطي `2` و`ارجع "سلام"` تُعطي **عنوانًا**.
                    //      (ISSUE-177)
                    // (EN) The promise's value type is derived from the CALLEE, not from
                    //      the returned register's type: a coroutine always returns a frame
                    //      handle and the promise is a bare i64. Without this the promise is
                    //      read as an integer whatever it holds — measured: 2.5 → 2, and a
                    //      string → an address.
                    // ════════════════════════════════════════════════════════
                    SadTypeKind promiseValueType = SadTypeKind::Void;
                    if (auto *innerCall =
                            dynamic_cast<Sad::AST::CallExpr *>(awaitExpr->expression.get()))
                    {
                        if (auto *calleeName =
                                dynamic_cast<Sad::AST::VariableExpr *>(innerCall->callee.get()))
                        {
                            auto entry = b_.functionTable_.find(calleeName->name);
                            if (entry != b_.functionTable_.end() && entry->second.sirFunction &&
                                entry->second.sirFunction->isCoroutine)
                            {
                                promiseValueType = entry->second.sirFunction->coroutineValueType;
                            }
                        }
                    }

                    // (AR) بناء التعبير الداخلي (استدعاء دالة غير متزامنة → يُرجع handle)
                    // (EN) Build inner expression (async function call → returns coroutine handle)
                    auto innerResult = buildExpression(awaitExpr->expression.get());

                    // (AR) ⚠️ والكوروتينُ يسبقُ ممرَّ الهُويّةِ القياسيَّ أدناه: ذاك الممرُّ
                    //      وُضِع لقيمةٍ **ليست وعدًا** أصلًا، ولو سبقَ لَابتلعَ الانتظارَ.
                    // (EN) The coroutine case precedes the scalar identity path below, which
                    //      exists for values that are NOT promises and would swallow the await.
                    if (promiseValueType != SadTypeKind::Void)
                    {
                        std::string coroResultReg = b_.newTempRegister();
                        SIRInstruction coroSuspend;
                        coroSuspend.opcode = SIROpcode::CORO_SUSPEND;
                        coroSuspend.result = SIROperand::Register(coroResultReg, promiseValueType);
                        coroSuspend.operands.push_back(
                            SIROperand::Register(innerResult.registerName, innerResult.type));
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(coroSuspend);
                        }
                        return BuildResult(coroResultReg, promiseValueType);
                    }

                    // ====================================================================
                    // (AR) انتظار قيمة بدائيّة غير-Future: تمريرٌ مطابق للمفسّر الذي يُرجع
                    //      القيمة كما هي ما لم تكن Future (خريطة __future__ أو كائن Future).
                    //      إصدار CORO_SUSPEND على قيمة عدديّة يُسقط backend بـ
                    //      «Do not know how to promote this operator!» في legalization.
                    //      مسار الكوروتين يبقى للأنواع الكائنيّة/المؤشّرات (المقابض).
                    // (EN) Awaiting a non-Future primitive: identity pass-through, matching
                    //      the interpreter which returns the value unchanged unless it is a
                    //      Future. Emitting CORO_SUSPEND on a scalar crashes LLVM
                    //      legalization; the coroutine path stays for object/pointer handles.
                    // ====================================================================
                    switch (innerResult.type)
                    {
                        case SadTypeKind::Integer:
                        case SadTypeKind::Float:
                        case SadTypeKind::Float32:
                        case SadTypeKind::Boolean:
                        case SadTypeKind::String:
                        case SadTypeKind::Char:
                        case SadTypeKind::Int8:
                        case SadTypeKind::Int16:
                        case SadTypeKind::Int32:
                        case SadTypeKind::UInt8:
                        case SadTypeKind::UInt16:
                        case SadTypeKind::UInt32:
                        case SadTypeKind::UInt64:
                            return innerResult;
                        default:
                            break;
                    }

                    // (AR) إصدار تعليمة CORO_SUSPEND: انتظر الكوروتين الداخلي
                    // (EN) Emit CORO_SUSPEND: await the inner coroutine
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction suspendInst;
                    suspendInst.opcode = SIROpcode::CORO_SUSPEND;
                    suspendInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    suspendInst.operands.push_back(SIROperand::Register(innerResult.registerName, innerResult.type));

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(suspendInst);
                    }

                    return BuildResult(resultReg, SadTypeKind::Integer);
                }
                if (auto sizeofExpr = dynamic_cast<Sad::AST::SizeofExpr *>(expr))
                {
                    return buildExprSizeof(sizeofExpr);
                }
                if (auto atomicExpr = dynamic_cast<Sad::AST::AtomicExpr *>(expr))
                {
                    return buildExprAtomic(atomicExpr);
                }
                if (auto optChainExpr = dynamic_cast<Sad::AST::OptionalChainExpr *>(expr))
                {
                    return buildExprOptionalChain(optChainExpr);
                }
                if (auto nullCoalExpr = dynamic_cast<Sad::AST::NullCoalesceExpr *>(expr))
                {
                    return buildExprNullCoalesce(nullCoalExpr);
                }
                if (auto sliceExpr = dynamic_cast<Sad::AST::SliceExpr *>(expr))
                {
                    return buildExprSlice(sliceExpr);
                }

                // ========================================================================
                // (AR) NamedArgExpr - وسيط مسمى: اسم: قيمة
                //      يظهر داخل قائمة وسائط CallExpr
                //      في سياق المترجم، نبني فقط تعبير القيمة (الاسم يُستخدم للمطابقة)
                // (EN) Named argument: name: value
                //      Appears inside CallExpr argument list
                //      In compiler context, we just build the value expression
                // ========================================================================
                if (auto namedArgExpr = dynamic_cast<Sad::AST::NamedArgExpr *>(expr))
                {
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found NamedArgExpr name='" << namedArgExpr->name << "'");
#endif
                    if (namedArgExpr->value)
                    {
                        return buildExpression(namedArgExpr->value.get());
                    }
                    return BuildResult();
                }
                if (auto errorPropExpr = dynamic_cast<Sad::AST::ErrorPropagateExpr *>(expr))
                {
                    return buildExprErrorPropagate(errorPropExpr);
                }

// (AR) تعبير غير معروف - نرجع قيمة افتراضية
// (EN) Unknown expression - return default
#ifndef NDEBUG
                std::cout << "[WARNING] buildExpression: unhandled expression type: "
                          << typeid(*expr).name() << std::endl;
#endif
                return BuildResult();
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
