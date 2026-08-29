// ============================================================================
// sir_builder_expr_nullsafety.cpp
// ============================================================================
// Null safety, slice, and error propagation expression builders
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "error_catalog.h" // (AR) getTemplate(code)->id — الرمزُ من الكتالوج لا حرفًا
#include "error_manager.h" // (AR) reportFromCatalog + buildBilingualMessage
#include "sad_debug_log.h"

#include <iostream>
#include <limits>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildExprOptionalChain
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprOptionalChain(AST::OptionalChainExpr *optChainExpr)
            {
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found OptionalChainExpr for member '"
                          << optChainExpr->member << "'");
#endif

                // (AR) بناء تعبير الكائن
                // (EN) Build object expression
                auto objResult = buildExpression(optChainExpr->object.get());

                // (AR) [ISSUE-064] الكائن معروف «لاشيء» وقت الترجمة (نوعه Null): قصر
                //      الدائرة مباشرةً إلى حارس العدم دون توليد فرع الوصول. فرعُ الوصول
                //      يُصدر LOAD عضوٍ يتطلّب تخطيط صنف، وهو غير موجود لقيمةٍ فارغة ⇒
                //      كان يُنتج «No class mapping» رغم صحّة المخرج وقت التشغيل.
                // (EN) [ISSUE-064] The object is statically «لاشيء» (type Null): short-circuit
                //      straight to the null sentinel without emitting the access branch. The
                //      access branch emits a member LOAD that needs a class layout, which a
                //      null value lacks ⇒ it produced «No class mapping» despite a correct
                //      runtime result.
                if (objResult.type == SadTypeKind::Null)
                {
                    std::string nullReg = b_.newTempRegister();
                    if (b_.currentBlock_)
                    {
                        SIRInstruction moveInst(SIROpcode::MOVE);
                        moveInst.result = SIROperand::Register(nullReg, SadTypeKind::Integer);
                        moveInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                        b_.currentBlock_->addInstruction(moveInst);
                    }
                    return BuildResult(nullReg, SadTypeKind::Integer);
                }

                // (AR) م2 ISSUE-076: استنتِج نوع العضو من صنف الكائن (مرآة buildExprMember)
                //      وعمّم المسار الموسوم %SadDyn (نوع Any) ليشمل الأعضاء القياسيّة كلّها:
                //      نصّ (م1) + عدد/عشريّ/منطقيّ (م2). القيمة تُحمَّل بنوعها الصحيح (double/i1/i64)
                //      ثمّ يُعلّبها emitPhi عبر toDyn، فتطبعها dynToString وتقارنها dynCompare وتحسبها
                //      dynBinOp مطابقةً للمفسّر (طباعة/==/ضمّ/حساب) وتُرجِع «لاشيء» على الكائن الفارغ.
                //      قبل م2 كان العضو العشريّ/المنطقيّ يُثبَّت i64 فيتضارب نوعُ LOAD (double/i1) مع
                //      PHI‏ i64 ⇒ فشل verifyModule (لا تنفيذيّ). الافتراض Unknown (لا Integer): التعليب
                //      يُفعَّل فقط عند استنتاجٍ موجَبٍ لنوعٍ قياسيّ، فالأعضاء الكائنيّة/المصفوفيّة تبقى
                //      على المسار الموروث (i64) بلا انحدار.
                // (EN) م2 ISSUE-076: infer the member type from the object's class (mirrors
                //      buildExprMember) and generalize the tagged %SadDyn (Any) path to ALL scalar
                //      members: string (م1) + int/float/bool (م2). The value is loaded with its
                //      correct type (double/i1/i64), then emitPhi boxes it via toDyn, so dynToString/
                //      dynCompare/dynBinOp render/compare/compute it exactly like the interpreter
                //      (print/==/concat/arith) and return «لاشيء» on a null object. Before م2 a
                //      float/bool member was pinned to i64, so the LOAD type (double/i1) clashed with
                //      the i64 PHI ⇒ verifyModule failure (no executable). Default is Unknown (not
                //      Integer): boxing engages only on a positive scalar inference, so object/array
                //      members keep the legacy i64 path (no regression).
                SadTypeKind memberType = SadTypeKind::Unknown;
                if (!objResult.className.empty() && b_.module_)
                {
                    auto sirClass = b_.module_->getClass(objResult.className);
                    if (sirClass)
                    {
                        auto fieldIt = sirClass->fields_.find(optChainExpr->member);
                        if (fieldIt != sirClass->fields_.end())
                            memberType = fieldIt->second;
                    }
                }
                const bool useDyn = (memberType == SadTypeKind::String ||
                                     memberType == SadTypeKind::Integer ||
                                     memberType == SadTypeKind::Float ||
                                     memberType == SadTypeKind::Boolean);
                const SadTypeKind accessType = useDyn ? memberType : SadTypeKind::Integer;
                const SadTypeKind nullIncomingType = useDyn ? SadTypeKind::Null : SadTypeKind::Integer;
                const SadTypeKind resultType = useDyn ? SadTypeKind::Any : SadTypeKind::Integer;

                // (AR) إنشاء الكتل: فحص null → وصول العضو / null
                // (EN) Create blocks: null check → member access / null
                std::string accessLabel = b_.newLabel("optchain_access");
                std::string nullLabel = b_.newLabel("optchain_null");
                std::string mergeLabel = b_.newLabel("optchain_merge");

                auto accessBlock = b_.createBasicBlock(accessLabel);
                auto nullBlock = b_.createBasicBlock(nullLabel);
                auto mergeBlock = b_.createBasicBlock(mergeLabel);

                // (AR) [ISSUE-064] فحص: هل الكائن ليس «لاشيء»؟ «لاشيء» يُمثَّل بـ
                //      kSadNullSentinel (≠ 0) لا بصفر، فمقارنته بـ0 كانت تجعل
                //      الكائن الفارغ «موجودًا» خطأً ⇒ يُحمَّل العضو من الحارس كمؤشّر.
                //      الصحيح: قصر الدائرة حين يساوي الكائن الحارس.
                // (EN) [ISSUE-064] Check: is object non-null? «لاشيء» is the sentinel
                //      kSadNullSentinel (≠ 0), not zero — comparing to 0 made a null
                //      object wrongly «present» ⇒ the member was loaded from the
                //      sentinel as a pointer. Correct: short-circuit when object == sentinel.
                std::string cmpReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction cmpInst(SIROpcode::NE);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                    b_.currentBlock_->addInstruction(cmpInst);

                    b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(accessLabel),
                        SIROperand::Label(nullLabel)));
                }

                // (AR) فرع الوصول: الكائن موجود → LOAD العضو
                // (EN) Access branch: object exists → LOAD member
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(accessBlock);
                b_.currentBlock_ = accessBlock;
                std::string memberReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction loadInst;
                    loadInst.opcode = SIROpcode::LOAD;
                    loadInst.result = SIROperand::Register(memberReg, accessType);
                    loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    loadInst.operands.push_back(SIROperand::ConstantString(optChainExpr->member));
                    loadInst.comment = "optional chain member: " + optChainExpr->member;
                    b_.currentBlock_->addInstruction(loadInst);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                // (AR) [ISSUE-064] فرع null: إرجاع حارس العدم (kSadNullSentinel)
                //      لا الصفر، كي يطبعه مسار الطباعة «لاشيء» موافقًا للمفسّر.
                // (EN) [ISSUE-064] Null branch: return the null sentinel
                //      (kSadNullSentinel), not 0, so the print path renders «لاشيء»
                //      matching the interpreter.
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(nullBlock);
                b_.currentBlock_ = nullBlock;
                std::string nullReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(nullReg, SadTypeKind::Integer);
                    moveInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                    b_.currentBlock_->addInstruction(moveInst);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                // (AR) كتلة الدمج مع PHI
                // (EN) Merge block with PHI
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(mergeBlock);
                b_.currentBlock_ = mergeBlock;
                std::string phiReg = b_.newTempRegister();
                SIRInstruction phiInst = SIRInstruction::Phi(
                    SIROperand::Register(phiReg, resultType),
                    {{SIROperand::Register(memberReg, accessType), SIROperand::Label(accessLabel)},
                     {SIROperand::Register(nullReg, nullIncomingType), SIROperand::Label(nullLabel)}});
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(phiInst);

                return BuildResult(phiReg, resultType);
            }

            // ============================================================================
            // (AR) أينتهي هذا المستقبِلُ إلى قراءةٍ بلا أثرٍ جانبيّ؟ — تعاوديًّا.
            //      يُقبَل: متغيّرٌ · «هذا» · عضوٌ **قاعدتُه مقبولةٌ هي الأخرى**.
            //      ويُرفَض كلُّ ما عداه، ومنه النداءُ والإسنادُ والفهرسة —
            //      ⚠️ والفهرسةُ مرفوضةٌ **تحفّظًا**: `س[i]` قراءةٌ بلا أثرٍ ظاهرٍ،
            //      لكنّ `i` نفسَه قد يكون نداءً، ولا تُبنى هنا شجرةُ آثارٍ كاملة.
            //      والتحفّظُ يُنتِج رفضًا مُشخَّصًا؛ وضدُّه يُنتِج أثرًا مضاعَفًا
            //      **لا يُرى في مخرَجٍ صحيح**. والأوّلُ يُصلَح بسطرٍ من الكاتب.
            // (EN) Does this receiver bottom out in a side-effect-free read? Accepts a
            //      variable, «this», or a member whose BASE is itself acceptable.
            //      Indexing is refused conservatively: the index expression could
            //      itself be a call, and no full effect analysis is built here. Being
            //      conservative costs a diagnosed rejection; the converse costs a
            //      duplicated side effect that no correct output reveals.
            // ============================================================================
            static bool isSideEffectFreeReceiver(const Sad::AST::Expr *receiver)
            {
                while (receiver != nullptr)
                {
                    if (dynamic_cast<const Sad::AST::VariableExpr *>(receiver) != nullptr ||
                        dynamic_cast<const Sad::AST::ThisExpr *>(receiver) != nullptr)
                    {
                        return true;
                    }
                    if (const auto *member = dynamic_cast<const Sad::AST::MemberExpr *>(receiver))
                    {
                        receiver = member->object.get();
                        continue;
                    }
                    if (const auto *access =
                            dynamic_cast<const Sad::AST::MemberAccessExpr *>(receiver))
                    {
                        receiver = access->object.get();
                        continue;
                    }
                    return false;
                }
                return false;
            }

            // ============================================================================
            // buildExprOptionalMethodCall
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprOptionalMethodCall(AST::MethodCallExpr *methodCallExpr)
            {
                if (!methodCallExpr || !methodCallExpr->object)
                    return BuildResult();

                // (AR) المستقبِلُ البسيطُ وحدَه: قراءةُ متغيّرٍ أو «هذا» أو عضوٍ منهما —
                //      لا أثرَ جانبيَّ لها فتُعاد مرّتَين بلا ضرر. وما عداه يُشخَّص:
                //      نداءٌ مستقبِلًا يقع مرّتَين، وذاك عطبٌ لا يُرى في مخرَجٍ صحيح —
                //      والسكوتُ عنه أسوأُ من رفضِه.
                // (EN) Only a side-effect-free receiver may be re-read: variable, this, or
                //      a member of them. Anything else is diagnosed, not silently doubled.
                // ════════════════════════════════════════════════════════════
                // (AR) 🔑 **الحارسُ يتعاود على القاعدةِ — ولا يفحص العقدةَ الطرفيّة.**
                //
                //      كُتِب أوّلًا فحصًا لصنفِ العقدةِ وحدَها، فكان يقبل أيَّ
                //      `MemberExpr` **ولو كانت قاعدتُه نداءً**. وقِيس أثرُ ذلك:
                //
                //        دالة أنشئ()          ⇒ تطبع «نُودي» وتُرجِع كائنًا
                //        أنشئ().اسم؟.طول()   ⇒ مفسّر: نُودي مرّةً · مترجَم: **مرّتَين**
                //        أنشئ().اسم.طول()    ⇒ مرّةً في الاثنين (ضابط)
                //
                //      والجوابُ `3` **صحيحٌ في الحالتَين** — وهو ما يجعل العطبَ
                //      غيرَ مرئيّ. أي أنّ الحارسَ كان يُعلِن عقدًا («عضوٌ منهما»)
                //      ولا يُنفِّذه، ويترك يقع بالضبطِ ما وُضِع ليمنعه.
                //
                //      🔑 والدرسُ: **حارسٌ يفحص صنفَ العقدةِ يقيس شكلَ التعبيرِ لا
                //      أثرَه.** والسؤالُ ليس «أهذا عضو؟» بل «أينتهي هذا التعبيرُ
                //      إلى قراءةٍ بلا أثرٍ جانبيّ؟» — وذاك سؤالٌ تعاوديٌّ بطبعِه.
                // (EN) The guard RECURSES on the base; it does not test the terminal
                //      node. Written first as a node-kind test, it accepted any
                //      MemberExpr even when its base was a CALL. Measured: the
                //      compiler evaluated the receiver TWICE (two «نُودي» lines) while
                //      the interpreter evaluated it once — with an identical, correct
                //      answer hiding it. A node-kind test measures the SHAPE of an
                //      expression, not its EFFECT; the real question ("does this end
                //      in a side-effect-free read?") is recursive by nature.
                // ════════════════════════════════════════════════════════════
                const Sad::AST::Expr *receiver = methodCallExpr->object.get();
                const bool receiverIsSimple = isSideEffectFreeReceiver(receiver);
                if (!receiverIsSimple)
                {
                    // ════════════════════════════════════════════════════════
                    // (AR) 🔑 **تشخيصٌ بلا رمزٍ لا يُحرَس** — النمطُ الثلاثيُّ نفسُه
                    //      المشروحُ عند SEM042 في `statement_types.cpp`:
                    //        ① `reportFromCatalog` يُخرِج السطرَ المرمَّزَ الذي يقرؤه
                    //           الكاتبُ و**تُثبِّته البذرةُ** بـ`@expect_compile_error`.
                    //        ② الرمزُ يُؤخَذ من الكتالوجِ (`getTemplate(code)->id`) لا
                    //           يُكتَب حرفًا — وإلّا انجرف عن مصدرِ الحقيقةِ صامتًا.
                    //        ③ `b_.errors_` هو ما يجعل رمزَ خروجِ البناءِ غيرَ صفريّ.
                    //      وتركُ أحدِها يُنتِج إمّا رسالةً لا تُثبَّت، أو رفضًا لا يُوقِف
                    //      البناء. وكانت هذه الرسالةُ **سلسلةً عربيّةً مباشرةً** بلا
                    //      رمزٍ، فكان الحدُّ الذي تُعلنه **غيرَ محروسٍ ببذرةٍ واحدة**:
                    //      حدٌّ مذكورٌ في تعليقٍ ولا يقيسه شيءٌ يُنقَض عند أوّلِ تعديل.
                    // (EN) A diagnostic without a code cannot be guarded — the same
                    //      three-part pattern documented at SEM042: report through the
                    //      catalog (the coded line seeds pin on), take the id FROM the
                    //      catalog (never a literal), and push into the builder's error
                    //      bucket (what makes the build exit nonzero). This message used
                    //      to be a bare Arabic literal, so the limit it declares was
                    //      guarded by no seed at all.
                    // ════════════════════════════════════════════════════════
                    Sad::Errors::RenderContext receiverErrorContext;
                    receiverErrorContext.placeholders = {
                        {"method", methodCallExpr->methodName}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                        Sad::Errors::ErrorCode::SEM_OPTIONAL_CALL_RECEIVER_NOT_SIMPLE,
                        Sad::Errors::SourceLocation("", methodCallExpr->position.line,
                                                    methodCallExpr->position.column),
                        receiverErrorContext);
                    const auto *receiverErrorTemplate =
                        Sad::Errors::ErrorCatalog::instance().getTemplate(
                            Sad::Errors::ErrorCode::SEM_OPTIONAL_CALL_RECEIVER_NOT_SIMPLE);
                    std::string receiverErrorText;
                    if (receiverErrorTemplate)
                    {
                        receiverErrorText = "[" + receiverErrorTemplate->id + "] ";
                    }
                    receiverErrorText +=
                        Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                            Sad::Errors::ErrorCode::SEM_OPTIONAL_CALL_RECEIVER_NOT_SIMPLE,
                            receiverErrorContext);
                    b_.errors_.push_back(receiverErrorText);
                    return BuildResult();
                }

                auto objResult = buildExpression(methodCallExpr->object.get());

                // (AR) مستقبِلٌ معروفٌ «لاشيء» ساكنًا ⇒ عدمٌ بلا توليدِ فرعٍ أصلًا
                //      (نظيرُ ISSUE-064 في `buildExprOptionalChain` حرفًا بحرف).
                // (EN) Statically-null receiver ⇒ null, with no branch emitted at all.
                if (objResult.type == SadTypeKind::Null)
                {
                    std::string nullOnly = b_.newTempRegister();
                    if (b_.currentBlock_)
                    {
                        SIRInstruction moveInst(SIROpcode::MOVE);
                        moveInst.result = SIROperand::Register(nullOnly, SadTypeKind::Integer);
                        moveInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                        b_.currentBlock_->addInstruction(moveInst);
                    }
                    return BuildResult(nullOnly, SadTypeKind::Integer);
                }

                std::string callLabel = b_.newLabel("optcall_invoke");
                std::string nullLabel = b_.newLabel("optcall_null");
                std::string mergeLabel = b_.newLabel("optcall_merge");

                auto callBlock = b_.createBasicBlock(callLabel);
                auto nullBlock = b_.createBasicBlock(nullLabel);
                auto mergeBlock = b_.createBasicBlock(mergeLabel);

                // (AR) الفحصُ على الحارسِ لا على الصفر — نظيرُ ISSUE-064.
                // (EN) Test against the sentinel, not zero — mirrors ISSUE-064.
                std::string cmpReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction cmpInst(SIROpcode::NE);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(
                        SIROperand::Register(objResult.registerName, objResult.type));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                    b_.currentBlock_->addInstruction(cmpInst);
                    b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(callLabel),
                        SIROperand::Label(nullLabel)));
                }

                // (AR) فرعُ النداء: المستقبِلُ حيٌّ ⇒ نداءُ الطريقةِ **كما هو** بلا
                //      تعديلِ دلالة. وهو شرطُ ألّا يصير الوصولُ الآمنُ لهجةً ثانية.
                // (EN) Live receiver ⇒ the ordinary method call, unchanged.
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(callBlock);
                b_.currentBlock_ = callBlock;
                auto callResult = b_.buildMethodCall(methodCallExpr);
                const SadTypeKind callType =
                    callResult.registerName.empty() ? SadTypeKind::Integer : callResult.type;
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                // (AR) ⚠️ الكتلةُ الحاليّةُ قد تكون تبدّلت داخلَ بناءِ النداء (فروعٌ
                //      داخليّة)، فيُؤخَذ اسمُ الوارِدِ من الحالةِ لا من المُفترَض.
                // (EN) The current block may change inside the call build, so the PHI's
                //      incoming label is read from state rather than assumed.
                const std::string callIncomingLabel =
                    b_.currentBlock_ ? b_.currentBlock_->name : callLabel;

                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(nullBlock);
                b_.currentBlock_ = nullBlock;
                std::string nullReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(nullReg, SadTypeKind::Integer);
                    moveInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                    b_.currentBlock_->addInstruction(moveInst);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(mergeBlock);
                b_.currentBlock_ = mergeBlock;

                // (AR) النتيجةُ **موسومةٌ خارجَ النطاق** (`Any`) لا مثبَّتةً بنوعِ الطريقة:
                //      الفرعانِ يحملان نوعَين مختلفَين (نتيجةُ الطريقةِ · العدم)، وتثبيتُهما
                //      على نوعٍ واحدٍ هو عينُ ما تُصلحه هذه الحملة — العقدُ (أ) في موضعِه.
                // (EN) The result is OUT-OF-BAND tagged (Any), not pinned to the method's
                //      return type: the two edges carry different types, and pinning them
                //      to one is the very defect this campaign closes.
                std::string phiReg = b_.newTempRegister();
                SIRInstruction phiInst = SIRInstruction::Phi(
                    SIROperand::Register(phiReg, SadTypeKind::Any),
                    {{SIROperand::Register(callResult.registerName, callType),
                      SIROperand::Label(callIncomingLabel)},
                     {SIROperand::Register(nullReg, SadTypeKind::Null),
                      SIROperand::Label(nullLabel)}});
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(phiInst);

                return BuildResult(phiReg, SadTypeKind::Any);
            }

            // ============================================================================
            // buildExprNullCoalesce
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprNullCoalesce(AST::NullCoalesceExpr *nullCoalExpr)
            {
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found NullCoalesceExpr");
#endif

                // (AR) بناء التعبير الأيسر
                // (EN) Build left expression
                auto leftResult = buildExpression(nullCoalExpr->left.get());

                // (AR) [ISSUE-046] القيمة المنطقيّة لا تكون «لاشيء» أبدًا. ومقارنة سجلّ
                //      منطقيّ (i1) بحارس العدم (i64 = 0x8000…0001) تبتر الحارس إلى بِتّه
                //      الأدنى (=1) فيُحسَب «صحيح» مساويًا للحارس خطأً ⇒ يُعاد الأيمن.
                //      الحلّ: نُرجِع الأيسر مباشرةً لأيّ معامل منطقيّ (غير قابل للعدم).
                // (EN) A boolean is never null. Comparing an i1 register to the i64 null
                //      sentinel (0x8000…0001) truncates the sentinel to its low bit (=1),
                //      so `true` is wrongly seen as the sentinel and the right operand is
                //      returned. Short-circuit to the left value for any boolean operand.
                //
                // 🔑 (AR) [م‑ب · العقد (أ)] هذا الشرطُ **يبقى، ويصير أدقَّ ممّا كان**.
                //      كان يقرأ «منطقيٌّ» فيشمل المنطقيَّ العدميَّ سهوًا؛ فبعد العقد (أ)
                //      يُخزَّن `منطقي؟` في `Any` لا في `Boolean` (sirNullableStorageKind)،
                //      فصار `Boolean` هنا يعني **غيرَ العدميِّ حصرًا** — وهو الذي لا يكون
                //      «لاشيء» أبدًا، فقصرُ الدائرةِ له صوابٌ لا رقعة. وحذفُه — كما كانت
                //      الخطّةُ تفترض — يُعيدُ بترَ الحارسِ على المنطقيِّ غيرِ العدميّ.
                //      والعدميُّ لا يمرّ من هنا: نوعُه `Any` فيسلك مسارَ المقارنةِ الكامل.
                // 🔑 (EN) [م‑ب · contract (a)] This test STAYS, and becomes more precise than
                //      it was. It used to read «boolean» and so swept up nullable booleans by
                //      accident; under contract (a) `bool?` is stored as `Any`, not `Boolean`
                //      (sirNullableStorageKind), so `Boolean` here now means the NON-nullable
                //      kind exclusively — the one that genuinely is never null, for which the
                //      short-circuit is correct rather than a patch. Deleting it, as the plan
                //      assumed, would reinstate sentinel truncation for non-nullable booleans.
                //      Nullable booleans no longer reach this line: their kind is `Any`, so
                //      they take the full comparison path.
                if (leftResult.type == SadTypeKind::Boolean)
                {
                    return leftResult;
                }

                // (AR) إنشاء الكتل: فحص null → يسار / يمين
                // (EN) Create blocks: null check → left / right
                std::string leftLabel = b_.newLabel("nc_left");
                std::string rightLabel = b_.newLabel("nc_right");
                std::string mergeLabel = b_.newLabel("nc_merge");

                auto leftBlock = b_.createBasicBlock(leftLabel);
                auto rightBlock = b_.createBasicBlock(rightLabel);
                auto mergeBlock = b_.createBasicBlock(mergeLabel);

                // (AR) فحص: هل اليسار != لاشيء (null-sentinel)
                //      اللغة تمثل لاشيء بقيمة sentinel خاصة وليست 0، لذلك
                //      المقارنة مع 0 تكسر ?? عندما يكون اليسار هو لاشيء فعلاً.
                // (EN) Check: is left != null sentinel
                //      The language represents null with a dedicated sentinel, not 0,
                //      so comparing against 0 breaks ?? for real null values.
                std::string cmpReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction cmpInst(SIROpcode::NE);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                    b_.currentBlock_->addInstruction(cmpInst);

                    b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(leftLabel),
                        SIROperand::Label(rightLabel)));
                }

                // (AR) فرع اليسار: القيمة موجودة
                // (EN) Left branch: value exists
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(leftBlock);
                b_.currentBlock_ = leftBlock;
                // (AR) دائماً MOVE محلي في nc_left لضمان صلاحية leftReg في PHI
                // (EN) Always emit local MOVE in nc_left so PHI has a locally-defined value
                std::string leftReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(leftReg, leftResult.type);
                    if (leftResult.isConstant)
                    {
                        switch (leftResult.type)
                        {
                        case SadTypeKind::String:
                            moveInst.operands.push_back(SIROperand::ConstantString(leftResult.constantValue));
                            break;
                        case SadTypeKind::Float:
                            moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(leftResult.constantValue)));
                            break;
                        default:
                            try
                            {
                                moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(leftResult.constantValue)));
                            }
                            catch (const std::exception &)
                            {
#ifndef NDEBUG
                                std::cerr << "[SIR] تحذير: فشل تحويل قيمة إلى I64، القيمة الافتراضية 0\n";
#endif
                                moveInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                            break;
                        }
                    }
                    else
                    {
                        // (AR) سجل: أُضيف كمعامل مصدر
                        // (EN) Register: add as source operand
                        moveInst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                    }
                    b_.currentBlock_->addInstruction(moveInst);
                }

                // (AR) فرع اليمين: القيمة البديلة
                // (EN) Right branch: fallback value
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(rightBlock);
                b_.currentBlock_ = rightBlock;
                auto rightResult = buildExpression(nullCoalExpr->right.get());
                // (AR) قد يُولّد بناءُ الأيمن كتلًا فرعيّة (مصفوفة/خريطة/كائن يُصدر
                //      فحوص حدود بكتلٍ مثل set.bc.ok)، فتصير الكتلة الحاليّة هي سلفَ
                //      الدمج الفعليّ لا rightBlock الأصليّة. نلتقطها لنُصدر منها القفزَ
                //      إلى الدمج ولنُسجّلها واردًا في PHI — وإلّا فشل «PHI node entries
                //      do not match predecessors» في «لاشيء ؟؟ [1،2]».
                // (EN) Building the right may spawn sub-blocks (array/map/object emit
                //      bounds checks with blocks like set.bc.ok), so the current block —
                //      not the original rightBlock — is the true predecessor of merge.
                //      Capture it to branch to merge from it and record it as the PHI
                //      incoming; otherwise «PHI node entries do not match predecessors»
                //      fires for «لاشيء ؟؟ [1،2]».
                auto rightEndBlock = b_.currentBlock_ ? b_.currentBlock_ : rightBlock;
                SadTypeKind resultType = leftResult.type;
                // (AR) [ISSUE-046] حين يكون الأيسر «لاشيء» حرفيًّا (Null) فالنتيجة تأتي
                //      من الأيمن دائمًا؛ نعتمد نوع الأيمن كي لا يُعامَل بديلٌ منطقيّ/عشريّ
                //      كـi64 فيُفسَد عند MOVE (مثلاً «true» تفشل stoll ⇒ 0).
                // (EN) When the left is the literal null, the result always comes from the
                //      right, so adopt the right's type — otherwise a boolean/float fallback
                //      is mis-typed as i64 and corrupted at MOVE (e.g. "true" fails stoll→0).
                if (resultType == SadTypeKind::Void || resultType == SadTypeKind::Unknown ||
                    resultType == SadTypeKind::Null)
                {
                    resultType = rightResult.type;
                }
                // (AR) عند مزج nullable عددي مع بديل نصي، نعتمد النص كنوع موحَّد
                //      للحفاظ على سلوك التنفيذ الديناميكي المتوقع في اللغة.
                // (EN) For numeric-nullable with string fallback, prefer string as unified type
                //      to preserve expected dynamic-language behavior.
                if (rightResult.type == SadTypeKind::String && resultType != SadTypeKind::String)
                {
                    resultType = SadTypeKind::String;
                }

                // (AR) توحيد فرع اليسار إلى resultType عند الحاجة قبل القفز إلى الدمج.
                //      الأيسر هنا «لاشيء/فراغ» (فرعٌ ميت: النتيجة تأتي من الأيمن حين
                //      يتباين النوعان)، لكن عقدة PHI في الخفض إلى LLVM تشترط تطابق نوع
                //      كلّ وارد مع نوعها الموحَّد. Null يُخفَض إلى i64 بينما Boolean→i1
                //      وFloat→double، فمرور سجلّ الأيسر (i64) دون توحيد يُنتج
                //      «Module verification failed». نُدرِج تحويلًا صريحًا لكلّ نوعٍ.
                // (EN) Normalize the left branch to resultType before jumping to merge.
                //      The left here is null/void (a dead branch: the result comes from the
                //      right when the two types diverge), yet the LLVM PHI requires every
                //      incoming value's type to match its unified type. Null lowers to i64
                //      while Boolean→i1 and Float→double, so feeding the raw i64 left
                //      register unconverted yields «Module verification failed». Emit an
                //      explicit conversion per target type.
                if (leftResult.type != resultType && b_.currentBlock_)
                {
                    b_.currentBlock_ = leftBlock;

                    if (resultType == SadTypeKind::String)
                    {
                        std::string leftCastReg = b_.newTempRegister();
                        SIROpcode castOpcode = (leftResult.type == SadTypeKind::Float)
                                                   ? SIROpcode::F64_TO_STRING
                                                   : SIROpcode::I64_TO_STRING;
                        SIRInstruction castInst(castOpcode);
                        castInst.result = SIROperand::Register(leftCastReg, SadTypeKind::String);
                        castInst.operands.push_back(SIROperand::Register(leftReg, leftResult.type));
                        b_.currentBlock_->addInstruction(castInst);
                        leftReg = leftCastReg;
                    }
                    else if (resultType == SadTypeKind::Boolean)
                    {
                        // (AR) i64 (حارس العدم) → i1 عبر I64_TO_BOOL كي يطابق نوع PHI.
                        // (EN) i64 (null sentinel) → i1 via I64_TO_BOOL to match the PHI type.
                        std::string leftCastReg = b_.newTempRegister();
                        SIRInstruction castInst(SIROpcode::I64_TO_BOOL);
                        castInst.result = SIROperand::Register(leftCastReg, SadTypeKind::Boolean);
                        castInst.operands.push_back(SIROperand::Register(leftReg, leftResult.type));
                        b_.currentBlock_->addInstruction(castInst);
                        leftReg = leftCastReg;
                    }
                    else if (resultType == SadTypeKind::Float && leftResult.type != SadTypeKind::Float)
                    {
                        // (AR) i64 → double عبر I64_TO_F64 كي يطابق نوع PHI العشريّ.
                        // (EN) i64 → double via I64_TO_F64 to match the float PHI type.
                        std::string leftCastReg = b_.newTempRegister();
                        SIRInstruction castInst(SIROpcode::I64_TO_F64);
                        castInst.result = SIROperand::Register(leftCastReg, SadTypeKind::Float);
                        castInst.operands.push_back(SIROperand::Register(leftReg, leftResult.type));
                        b_.currentBlock_->addInstruction(castInst);
                        leftReg = leftCastReg;
                    }
                }
                if (leftBlock)
                {
                    b_.currentBlock_ = leftBlock;
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                b_.currentBlock_ = rightEndBlock;
                std::string rightReg = rightResult.registerName;
                if (rightResult.isConstant && b_.currentBlock_)
                {
                    rightReg = b_.newTempRegister();
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    // (AR) 🔴 عطبٌ مقيسٌ قبل م‑ب: حين تكون النتيجةُ `Any` (خانةٌ موسومةٌ
                    //      خارجَ النطاق) كان الثابتُ الأيمنُ يسقط إلى فرعِ `default` فيمرّ
                    //      «true» على `std::stoll` ⇒ يرمي ⇒ **صفر**. قِيس على
                    //      `أي س = لاشيء؛ اطبع_سطر(نص(س ؟؟ صحيح))`: المفسّرُ «صحيح»
                    //      والمترجَمُ «0». و`Any` ليست نوعَ الثابتِ بل **وعاؤه**، فيُبنى
                    //      الثابتُ بنوعِه هو وتتكفّلُ `toDyn` بتعليبِه عند PHI.
                    // (EN) 🔴 Defect measured before م‑ب: when the result is `Any` (an
                    //      out-of-band-tagged slot) the right constant fell through to
                    //      `default`, so "true" hit `std::stoll`, threw, and yielded **zero**.
                    //      Measured on `أي س = لاشيء; print(str(س ?? true))`: interpreter
                    //      «صحيح», compiled «0». `Any` is not the constant's type but its
                    //      CONTAINER, so the constant is built with its own kind and `toDyn`
                    //      boxes it at the PHI.
                    const SadTypeKind constantKind =
                        (resultType == SadTypeKind::Any) ? rightResult.type : resultType;
                    moveInst.result = SIROperand::Register(rightReg, constantKind);
                    switch (constantKind)
                    {
                    case SadTypeKind::String:
                        moveInst.operands.push_back(SIROperand::ConstantString(rightResult.constantValue));
                        break;
                    case SadTypeKind::Float:
                        moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(rightResult.constantValue)));
                        break;
                    case SadTypeKind::Boolean:
                        // (AR) [ISSUE-046] بديل منطقيّ: نُصدر ConstantBool لا I64 (stoll تفشل على «true»)
                        // (EN) [ISSUE-046] boolean fallback: emit ConstantBool, not I64 (stoll fails on "true")
                        moveInst.operands.push_back(SIROperand::ConstantBool(
                            rightResult.constantValue == "true" || rightResult.constantValue == "1"));
                        break;
                    default:
                        try
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(rightResult.constantValue)));
                        }
                        catch (const std::exception &)
                        {
#ifndef NDEBUG
                            std::cerr << "[SIR] تحذير: فشل تحويل قيمة إلى I64، القيمة الافتراضية 0\n";
#endif
                            moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                        break;
                    }
                    b_.currentBlock_->addInstruction(moveInst);
                }
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));

                // (AR) كتلة الدمج مع PHI
                // (EN) Merge block with PHI
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(mergeBlock);
                b_.currentBlock_ = mergeBlock;
                std::string phiReg = b_.newTempRegister();
                // (AR) الوارد الأيمن من الكتلة الحاليّة الفعليّة (rightEndBlock) لا
                //      rightLabel الأصليّة، احترازًا من الكتل الفرعيّة التي يولّدها الأيمن.
                // (EN) Right incoming comes from the actual current block (rightEndBlock),
                //      not the original rightLabel, to account for right-spawned sub-blocks.
                std::string rightPredLabel = rightEndBlock ? rightEndBlock->name : rightLabel;
                SIRInstruction phiInst = SIRInstruction::Phi(
                    SIROperand::Register(phiReg, resultType),
                    {{SIROperand::Register(leftReg, resultType), SIROperand::Label(leftLabel)},
                     {SIROperand::Register(rightReg, resultType), SIROperand::Label(rightPredLabel)}});
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(phiInst);

                return BuildResult(phiReg, resultType);
            }

            // ============================================================================
            // buildExprSlice — بناء تعبير الشريحة [:] على المصفوفات
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprSlice(AST::SliceExpr *sliceExpr)
            {
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found SliceExpr");
#endif

                // (AR) بناء تعبير المصفوفة الأصلية
                // (EN) Build source array expression
                auto objResult = buildExpression(sliceExpr->object.get());

                // (AR) بناء معامل البداية (أو 0 افتراضياً)
                // (EN) Build start operand (or 0 as default)
                SIROperand startOp = SIROperand::ConstantI64(0);
                if (sliceExpr->start)
                {
                    auto startResult = buildExpression(sliceExpr->start.get());
                    if (startResult.isConstant)
                    {
                        try
                        {
                            startOp = SIROperand::ConstantI64(std::stoll(startResult.constantValue));
                        }
                        catch (...)
                        {
                            startOp = SIROperand::ConstantI64(0);
                        }
                    }
                    else if (!startResult.registerName.empty())
                    {
                        startOp = SIROperand::Register(startResult.registerName, startResult.type);
                    }
                }

                // (AR) [ISSUE-063] بناء معامل النهاية. الافتراض حين لا نهاية = حارس
                //      INT64_MIN لا -1: الخلفية (emitBuiltinArraySlice) تفسّر INT64_MIN
                //      «حتى آخر المصفوفة» بينما -1 فهرسٌ سالب صريح ⇒ srcLen-1 فيقتطع
                //      الذيل لعنصرٍ واحد (`[1..]` كان يُرجع [20] بدل [20, 30]).
                // (EN) [ISSUE-063] Build end operand. Default when no end = the sentinel
                //      INT64_MIN, not -1: the backend (emitBuiltinArraySlice) reads
                //      INT64_MIN as «to end of array» whereas -1 is an explicit negative
                //      index ⇒ srcLen-1, truncating the tail to one element (`[1..]`
                //      wrongly returned [20] instead of [20, 30]).
                SIROperand endOp = SIROperand::ConstantI64(std::numeric_limits<int64_t>::min());
                if (sliceExpr->end)
                {
                    auto endResult = buildExpression(sliceExpr->end.get());
                    if (endResult.isConstant)
                    {
                        try
                        {
                            endOp = SIROperand::ConstantI64(std::stoll(endResult.constantValue));
                        }
                        catch (...)
                        {
                            endOp = SIROperand::ConstantI64(std::numeric_limits<int64_t>::min());
                        }
                    }
                    else if (!endResult.registerName.empty())
                    {
                        endOp = SIROperand::Register(endResult.registerName, endResult.type);
                    }
                }

                // (AR) إنشاء تعليمة BUILTIN_ARRAY_SLICE: (array, start, end)
                // (EN) Create BUILTIN_ARRAY_SLICE instruction: (array, start, end)
                std::string resultReg = b_.newTempRegister();
                // (AR) نوعُ الشريحةِ نوعُ مستقبِلِها — والحُجّةُ كاملةٌ عند المُرجَعِ أدناه.
                // (EN) The slice's type is its receiver's; the full rationale is at the return.
                const SadTypeKind sliceKind = (objResult.type == SadTypeKind::String)
                                                  ? SadTypeKind::String
                                                  : SadTypeKind::Array;
                SIRInstruction sliceInst(SIROpcode::BUILTIN_ARRAY_SLICE);
                sliceInst.result = SIROperand::Register(resultReg, sliceKind);
                // (AR) المعامل الأول: المصفوفة المصدر
                if (objResult.isConstant)
                    sliceInst.operands.push_back(SIROperand::ConstantString(objResult.constantValue));
                else
                    sliceInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                // (AR) المعامل الثاني: بداية الشريحة
                sliceInst.operands.push_back(startOp);
                // (AR) المعامل الثالث: نهاية الشريحة
                sliceInst.operands.push_back(endOp);

                // (AR) المعامل الرابع: الخطوة (اختياري)
                if (sliceExpr->step)
                {
                    auto stepResult = buildExpression(sliceExpr->step.get());
                    if (stepResult.isConstant)
                    {
                        try
                        {
                            sliceInst.operands.push_back(SIROperand::ConstantI64(std::stoll(stepResult.constantValue)));
                        }
                        catch (...)
                        {
                            sliceInst.operands.push_back(SIROperand::ConstantI64(1));
                        }
                    }
                    else if (!stepResult.registerName.empty())
                    {
                        sliceInst.operands.push_back(SIROperand::Register(stepResult.registerName, stepResult.type));
                    }
                }

                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(sliceInst);

#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExprSlice -> " << resultReg);
#endif
                // (AR) ننشر نوعَ العنصر من المصدر: شريحةُ مصفوفةٍ موسومة (Any) تنسخ مؤشّرات
                //      الصناديق من مصدرٍ واحدٍ ⇒ النتيجة موسومةٌ متّسقة تُفكّ صحيحًا (لا اختلاطَ
                //      تعليبٍ كالدمج). المصدرُ غير الموسوم يُبقي النوعَ كما كان.
                // (EN) Propagate the element type from the source: slicing a boxed (Any) array
                //      copies box pointers from a single source ⇒ a consistently-boxed result
                //      that unboxes correctly (no boxing mismatch like concat). A non-boxed
                //      source keeps its type.
                // ════════════════════════════════════════════════════════════
                // (AR) 🔑 والشريحةُ **تُوسَمُ بنوعِ مستقبِلِها لا بنوعٍ واحدٍ مفترَض**.
                //      كان النوعُ `Array` دائمًا مهما كان المستقبِل، فشريحةُ نصٍّ
                //      تعبرُ إلى ذراعِ المصفوفةِ فيُطبَّعُ `char*` بوصفِه `SadArray*`
                //      وتُقرأُ قمامةٌ حقلًا حقلًا. وقِيس على المترجّمِ وحدَه:
                //        `ن[1:4]` · `ن[2:]` · `ن[:3]` · `ن[-3:]` ⇒ انهيارٌ segfault
                //        `"مرحبا بالعالم"[0:5]`             ⇒ يطبع `[]` جوابًا مختلَقًا
                //      والمفسّرُ يُجيبُ الأربعةَ صحيحًا. أي وجهان لعلّةٍ واحدة:
                //      انهيارٌ حين تقعُ القمامةُ على طولٍ ضخم، وكذبةٌ صامتةٌ حين تقعُ
                //      على صفر — والصامتةُ أسوأُ.
                //
                // (AR) ولا يُصنَعُ لها أوپكودٌ جديد: `BUILTIN_ARRAY_SLICE` يحملُ نوعَ
                //      معاملِه الأوّلِ، فالخلفيّةُ تُوزِّعُ عليه إلى ذراعِ النصِّ التي
                //      تُعيدُ استعمالَ قصِّ `جزء` نفسِه — فلا نسخةَ ثالثةً من قاعدةِ
                //      القصّ، ولا توسيعَ لمصدرِ حقيقةِ الأوپكودات.
                // (EN) The slice is typed by its RECEIVER, not by a single assumed type.
                //      It was always Array, so a string slice reached the array arm and a
                //      char* was normalized as a SadArray* — segfault, or a fabricated []
                //      when the garbage happened to read as length zero. No new opcode:
                //      the backend dispatches on the first operand's type and reuses the
                //      existing جزء clamping, so the clamp rule keeps exactly two copies.
                // ════════════════════════════════════════════════════════════
                if (objResult.type == SadTypeKind::String)
                    return BuildResult(resultReg, SadTypeKind::String);

                BuildResult sliceResult(resultReg, SadTypeKind::Array);
                sliceResult.elementType = objResult.elementType;
                return sliceResult;
            }

            // ============================================================================
            // buildExprErrorPropagate
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprErrorPropagate(AST::ErrorPropagateExpr *errorPropExpr)
            {
                // (AR) تنفيذ أولي آمن: نبني التعبير الداخلي كما هو ونُرجع نتيجته.
                //      هذا يحافظ على تماسك الـ SIR ويمنع فشل الربط عند وجود "انشر".
                // (EN) Safe baseline implementation: build and return the inner expression.
                //      This keeps SIR generation consistent and fixes linker failures.
                if (!errorPropExpr || !errorPropExpr->inner)
                {
                    return BuildResult("", SadTypeKind::Unknown);
                }

                return buildExpression(errorPropExpr->inner.get());
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
