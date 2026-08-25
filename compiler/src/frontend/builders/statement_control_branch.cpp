// ============================================================================
// sir_builder_control_branch.cpp
// (AR) Branches: return, break, continue, assignment, localvar, if
// (EN) Branch control flow: return, break, continue, assignment, local variables, if/else
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include "lexer_keywords.h" // (AR) تهجئةُ الكلماتِ من معجمِ SoT / (EN) SoT keyword spellings
#include "error_manager.h"
#include "error_codes.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // (AR) تشخيصُ «تحكّمٌ خارجَ حلقة» (SEM013) من كتالوجِ الأخطاء ومعجمِ الكلمات
            //      كليهما — لا نصًّا مكتوبًا هنا. كانت الرسالتان سلسلتَين خامَّتَين تسمّيان
            //      «قف» و«أكمل»، وهما تهجئتان لا تعرفهما اللغة (SoT: «توقف» و«استمر»)،
            //      فيقرأ المستعمِلُ كلمةً لا يستطيع كتابتَها، ويختلف نصُّ المحرّكَين.
            // (EN) The "control outside loop" diagnostic (SEM013) comes from the error
            //      catalog and the keyword lexicon alike — never hard-coded here. The two
            //      messages used to be raw strings naming «قف» and «أكمل», spellings the
            //      language does not know (SoT: «توقف» and «استمر»), so the reader saw a
            //      word they cannot write and the two engines disagreed on the text.
            static std::string controlOutsideLoopMessage(Lexer::TokenType keywordType)
            {
                const auto *entry = Lexer::KeywordTable::getEntry(keywordType);
                Sad::Errors::RenderContext context;
                context.placeholders = {
                    {"keyword", entry ? entry->primaryWord : std::string()}};
                return Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                    Sad::Errors::ErrorCode::SEM_CONTROL_OUTSIDE_LOOP, context);
            }

            // ============================================================================
            // buildReturnStatement - بناء جملة return
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:409
            // التوقيع / Signature: void buildReturnStatement(AST::ReturnStmt* retStmt);
            //
            // المعاملات / Parameters:
            // - retStmt: AST::ReturnStmt* = Sad::AST::ReturnStmt* (sir_builder.h:409)
            //
            // ReturnStmt Members (statements.h:266):
            // - value: ExprPtr (line 268) - optional, can be nullptr
            //
            // الدوال المستدعاة / Called functions:
            // - b_.buildExpression: sir_builder.h:432
            // ============================================================================
            void StatementBuilder::buildReturnStatement(AST::ReturnStmt *retStmt)
            {
                if (!retStmt)
                {
                    return;
                }

                // (AR) الحاجز ٧: العودة تخرج من كلّ «حاول» مُحيطة في الدالّة ⇒ خفّض
                //      __sad_try_active بعددها، وإلّا تسرّب العدّادُ بعد موت الإطار فيقفز
                //      أوّلُ حارس قسمة/صفر لاحق لـjmpbuf ميّت (0xC0000005). يُبعَث قبل RET.
                // (EN) Barrier 7: a return leaves every enclosing «try» in the function ⇒
                //      decrement __sad_try_active by that count, else the counter leaks after
                //      the frame dies and the next division-by-zero guard longjmps into a dead
                //      jmpbuf (0xC0000005). Emitted before the RET.
                // (AR) والمعالِجُ يُنبَذ مع العدّاد — عدّادان لا واحد.
                //
                //      «حاول» تدفع شيئَين: `__sad_try_active` (يقول: ثمّة حاولٌ نشط)
                //      و`__sad_handler_count` مع jmpbuf في مكدّسِ المعالِجات (يقول: **أين**
                //      يُقفَز). والخروجُ المبكّر كان يخفّض الأوّلَ ولا ينبذ الثاني، فيبقى
                //      في المكدّسِ مؤشّرٌ إلى jmpbuf إطارٍ **مات**. ثمّ يرمي المستدعي فيقرأ
                //      الرميُ الخانةَ العلياً — وهي الميّتةُ — فيقفز إلى إطارٍ لا وجودَ له.
                //
                //      وشرطُ ظهورِه ثلاثيٌّ فبدا نادرًا: دالّةٌ ترجع من **داخلِ** «حاول»،
                //      تُنادى من **داخلِ** «حاول» أخرى، ثمّ يُرمى بعدها. وهو حرفًا شكلُ
                //      `حلل_عدد` في مكتبة جيسون: تُرجع الزوجَ من داخلِ حارسِ المدى،
                //      فيسقط اختبارُ مسارِ الأخطاء (١٤٦) بلا أن يمسّ الخطأُ العددَ أصلًا.
                //
                //      والتعليقُ فوق `__sad_try_exit` يصف هذا العطبَ بعينِه («يقفز أوّلُ
                //      حارسٍ لاحقٍ إلى jmpbuf ميّت») ثمّ يعالج نصفَه: العدّادَ دون المكدّس.
                // (EN) The handler is popped with the counter — there are two counters, not one.
                //
                //      A «try» pushes two things: `__sad_try_active` (there is a live try) and
                //      `__sad_handler_count` plus a jmpbuf on the handler stack (**where** to
                //      jump). The early exit decremented the first and never popped the second,
                //      leaving on the stack a pointer to a **dead** frame's jmpbuf. The caller
                //      then throws, the raise reads the top slot — the dead one — and jumps into
                //      a frame that no longer exists.
                //
                //      Three conditions must coincide, which is why it looked rare: a function
                //      returning from **inside** a try, called from **inside** another try, with
                //      a throw afterwards. That is exactly the shape of `parse_number` in the
                //      JSON library, which returns its pair from inside the range guard — so the
                //      error-path test (146) died without the error ever touching a number.
                //
                //      The comment above `__sad_try_exit` describes this very defect ("the next
                //      guard longjmps into a dead jmpbuf") and then fixes half of it: the
                //      counter without the stack.
                // (AR) والعدّادان يُنبَذان معًا وبعدَ بناءِ التعبير — لا هذا قبلَه وذاك بعدَه.
                //
                //      كان `__sad_try_exit` يُبعَث هنا، قبلَ بناءِ تعبيرِ الإرجاع، بينما
                //      نُقِل نبذُ المعالِجِ إلى ما بعدَه بالحجّةِ المشروحةِ أدناه. والحجّةُ
                //      نفسُها تشملُ العدّادَ: `ارجع أ / ب` داخلَ «حاول» — القسمةُ في
                //      التعبيرِ نفسِه قد تهلع، وحارسُ الهلعِ يقرأ `__sad_try_active`
                //      ليعرفَ أثمّة التقاطٌ ممكن. فتصفيرُه قبلَ بناءِ التعبيرِ يجعل الهلعَ
                //      الجوهريَّ (RUN001) داخلَ التعبيرِ غيرَ قابلٍ للالتقاط. مقيسًا:
                //      المفسّرُ يلتقطُ ويعودُ ٠، والمصرَّفُ يموتُ بـRUN001 وخروجٍ ١؛
                //      ونقلُ القسمةِ إلى متغيّرٍ ثمّ إرجاعُه يجعلُه يلتقط — أي أنّ
                //      مكدّسَ المعالِجاتِ سليمٌ والعلّةُ في هذا العدّادِ وحدَه.
                // (EN) Both counters are popped together, after the expression is built.
                //
                //      `__sad_try_exit` used to be emitted here, before the return expression,
                //      while the handler pop was moved after it for the reason documented below.
                //      That same reason covers the counter: in `return a / b` inside a try, the
                //      division may panic and the panic guard reads `__sad_try_active` to decide
                //      whether a catch is reachable. Clearing it first makes an intrinsic panic
                //      (RUN001) inside the expression uncatchable — measured: the interpreter
                //      catches and returns 0, the compiled program dies with RUN001 and exit 1.
                //
                // (AR) والنبذُ **بعد** بناءِ تعبيرِ الإرجاع لا قبلَه.
                //
                //      `ارجع طبقة3(س)` داخلَ «حاول»: النداءُ نفسُه قد يرمي، وهو ما تحرسه
                //      هذه «الحاول» بعينِها. فنبذُ المعالِجِ قبلَ بناءِ التعبيرِ يعني أنّ
                //      الرميَ يجد فوقَ المكدّسِ معالِجَ الطبقةِ **الأعلى**، فتُتخطّى كتلُ
                //      `امسك` الوسطى بأسرِها. مقيسًا: سلسلةُ ثلاثِ طبقاتٍ طبعت الطبقةَ
                //      الأولى وحدَها.
                //
                //      ولذلك يُبعَث النبذُ عند نقطتَين تُدرِكان أنّ القيمةَ استقرّت:
                //      بعد البناءِ المسبَق (مسارُ «أجّل»)، أو قبيلَ RET في المسارِ العاديّ.
                //      و`handlerPopsEmitted` يمنع بعثَه مرّتَين.
                // (EN) The pop goes **after** the return expression is built, not before.
                //
                //      `return layer3(x)` inside a try: the call itself may throw, and this very
                //      try is what guards it. Popping the handler before building the expression
                //      means the raise finds the **outer** frame's handler on top, so every
                //      intermediate `catch` is skipped. Measured: a three-layer chain printed
                //      only the outermost layer.
                //
                //      So the pop is emitted at the two points that know the value has settled:
                //      after the pre-build (the defer path), or just before RET on the normal
                //      path. `handlerPopsEmitted` keeps it from being emitted twice.
                bool handlerPopsEmitted = false;
                const int handlerPopCount = b_.currentTryDepth_;
                auto emitTryHandlerPops = [&]() {
                    if (handlerPopsEmitted)
                        return;
                    handlerPopsEmitted = true;
                    for (int i = 0; i < handlerPopCount && b_.currentBlock_; ++i)
                    {
                        SIRInstruction tryExit;
                        tryExit.opcode = SIROpcode::CALL;
                        tryExit.operands.push_back(SIROperand::Function("__sad_try_exit"));
                        tryExit.comment = "barrier7: exit active try (return)";
                        b_.currentBlock_->addInstruction(tryExit);
                    }
                    for (int i = 0; i < handlerPopCount && b_.currentBlock_; ++i)
                    {
                        SIRInstruction popInst;
                        popInst.opcode = SIROpcode::CALL;
                        popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                        popInst.comment = "barrier7: pop the exited try's handler (return)";
                        b_.currentBlock_->addInstruction(popInst);
                    }
                };

                // ================================================================
                // (AR) FIX X06: بناء تعبير الإرجاع قبل defer
                //      المشكلة: emitRunDeferredClosures() يغير b_.currentBlock_ الى defer_skip،
                //      وعند بناء التعبير بعده، namedValues تشير الى alloca من نطاق مختلف.
                //      مثال: في match، متغير "ضريبة" لكل case له alloca منفصل. عند بناء
                //      ارجع "..." + ضريبة في defer_skip، يجد المترجم آخر alloca لـ ضريبة
                //      (من آخر case) بدلاً من alloca الـ case الحالي، فتكون القيمة 0.
                //      الحل: بناء التعبير في body block + تخزينه في alloca فريد لضمان SSA.
                // (EN) FIX X06: Build return expression BEFORE defer
                //      Problem: emitRunDeferredClosures() changes b_.currentBlock_ to defer_skip,
                //      and building the expression after it uses wrong alloca from another scope.
                //      In match: each case has its own "var" alloca. Building "..." + var
                //      in defer_skip finds the LAST case's alloca (value=0) not current case's.
                //      Solution: Build in current body block + store in unique temp alloca.
                // ================================================================
                std::string prebuiltRetReg;
                SadTypeKind prebuiltRetType = SadTypeKind::Void;
                // (AR) [وسم زمن-التشغيل] نوعُ عنصر المصفوفة للقيمة المُبنيّة مسبقًا — يُحفَظ
                //      بمعزلٍ عن prebuiltRetType (النوع فقط) لئلّا يضيع Any/نوعُ العنصر عبر
                //      alloca/store/load في مسار defer، فيبقى متاحًا لتتبّع إرجاع المصفوفة.
                // (EN) [runtime tags] element type of the prebuilt return value — tracked
                //      separately from prebuiltRetType (type only) so Any/element type isn't
                //      lost through the alloca/store/load of the defer path, keeping it
                //      available for array-return tracking.
                SadTypeKind prebuiltRetElementType = SadTypeKind::Void;
                // (AR) 🔑 واسمُ صنفِ القيمةِ المُرجَعةِ يعبُرُ الـalloca/store/load كأخيهِ فوقه
                //      (ISSUE-140 الشطرُ غيرُ المُصرَّح). فالمسارُ أدناهُ يُعيدُ بناءَ
                //      `valueResult` يدويًّا من حقولٍ محفوظةٍ معدودة، فما لم يُحفَظ
                //      يُمحى صامتًا — وكان `دالة حر() ارجع شخص()` يفقدُ «شخص»
                //      فيُقرأُ حقلُهُ النصّيُّ عددًا فيُطبَعُ المؤشِّرُ خامًا بـrc=0.
                //      ⚠️ والمسارُ يُسلَكُ وإن لم يكتبِ المستعمِلُ `أجّل` — مقيسٌ
                //      (`currentDeferStackReg_` غيرُ فارغٍ في دالّةٍ عاديّة): فلا يُظَنُّ
                //      أنّهُ حافّةٌ يندرُ بلوغُها.
                // (EN) The returned value's class name must survive the alloca/store/load
                //      like its sibling above (ISSUE-140, undeclared half). The path below
                //      rebuilds `valueResult` by hand from a fixed set of saved fields, so
                //      anything not saved is silently erased — `fn f() return Person()` lost
                //      «Person» and its string field was then read as an integer, printing a
                //      raw pointer at rc=0. NOTE: this path is taken even with no user
                //      `defer` — measured; it is not a rare edge.
                std::string prebuiltRetClassName;
                bool hasPrebuiltRet = false;
                // (AR) ISSUE-056: علامة أنّ تعبير الإرجاع نداءٌ يُرجع فراغاً وبُني مسبقاً
                //      لأثره الجانبيّ فقط — يمنع المسار العاديّ من إعادة بنائه (تباعد مزدوج).
                // (EN) ISSUE-056: flag that the return expr is a void-returning call already
                //      built for its side effect only — prevents the normal path rebuilding it.
                bool voidSideEffectPrebuilt = false;

                if (retStmt->value && !b_.currentDeferStackReg_.empty() && b_.finallyStack_.empty() &&
                    (!b_.currentFunction_ || !b_.currentFunction_->isCoroutine))
                {
                    BuildResult preResult = b_.buildExpression(retStmt->value.get());
                    if (!preResult.registerName.empty() && !preResult.isConstant)
                    {
                        // (AR) تخزين القيمة في alloca مؤقت قبل defer لضمان SSA correctness
                        // (EN) Store value in temp alloca before defer for SSA correctness
                        std::string tempAllocaReg = b_.newTempRegister();
                        SadTypeKind storeType = preResult.type;

                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::ALLOC;
                        allocInst.result = SIROperand::Register(tempAllocaReg, storeType);
                        allocInst.comment = "temp alloca for return value (FIX X06: before defer)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(allocInst);

                        SIRInstruction storeInst;
                        storeInst.opcode = SIROpcode::STORE;
                        storeInst.operands.push_back(SIROperand::Register(preResult.registerName, storeType));
                        storeInst.operands.push_back(SIROperand::Register(tempAllocaReg, storeType));
                        storeInst.comment = "store return value before defer cleanup (FIX X06)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(storeInst);

                        prebuiltRetReg = tempAllocaReg;
                        prebuiltRetType = storeType;
                        prebuiltRetElementType = preResult.elementType;
                        prebuiltRetClassName = preResult.className;
                        hasPrebuiltRet = true;
                    }
                    else if (preResult.registerName.empty() && !preResult.isConstant)
                    {
                        // (AR) ISSUE-056: التعبير نداءٌ يُرجع فراغاً — نُفِّذ أثره الجانبيّ (طباعة)
                        //      مرّة واحدة هنا، ولا قيمة تُرجَع. نعلّمه لئلا يُعاد بناؤه في المسار
                        //      العاديّ (كان المفسّر يطبع مرّة والمترجم مرّتين).
                        // (EN) ISSUE-056: void-returning call — its side effect (print) already
                        //      ran once here and there is no value to return. Flag it so the
                        //      normal path does NOT rebuild it (interp printed once, compiler twice).
                        voidSideEffectPrebuilt = true;
                    }
                }

                // (AR) القيمةُ بُنيت (أو لا قيمةَ أصلًا) ⇒ خرجنا من «حاول» فعلًا: انبذ معالِجَها
                //      قبل «أجّل» و RET.
                // (EN) The value is built (or there is none) ⇒ the try really is being left:
                //      pop its handler before defers and RET.
                if (hasPrebuiltRet || voidSideEffectPrebuilt || !retStmt->value)
                {
                    emitTryHandlerPops();
                }

                // ================================================================
                // (AR) تنفيذ الجمل المؤجلة (أجّل/defer) قبل RET بترتيب LIFO
                // (EN) Execute deferred statements (defer) before RET in LIFO order
                // ================================================================
                if (!b_.currentDeferStackReg_.empty())
                {
                    emitRunDeferredClosures();
                }
                else if (!b_.deferredStatements_.empty())
                {
                    for (auto it = b_.deferredStatements_.rbegin(); it != b_.deferredStatements_.rend(); ++it)
                    {
                        if (*it)
                            buildStatement(*it);
                    }
                }

                // ================================================================
                // (AR) فحص سياق finally: إذا كنا داخل try/catch مع أخيراً
                //      لا نُصدر RET مباشرة — نحفظ القيمة ونقفز لـ finally لضمان تنفيذه
                //      الأنواع المدعومة: 0=فراغ، 1=رقم/منطقي، 2=نص/مؤشر، 3=عشري
                //      هذا الفحص يجب أن يكون بعد defer وقبل كل شيء آخر
                // (EN) Check finally context: if inside try/catch with finally block
                //      Don't emit direct RET — save value and branch to finally to guarantee execution
                //      Supported types: 0=void, 1=integer/bool, 2=string/ptr, 3=float
                //      This check must come after defer processing and before everything else
                // ================================================================
                if (!b_.finallyStack_.empty())
                {
                    auto &ctx = b_.finallyStack_.back();

                    if (retStmt->value)
                    {
                        // (AR) بناء قيمة الإرجاع
                        // (EN) Build return value expression
                        BuildResult valResult = b_.buildExpression(retStmt->value.get());

                        // (AR) إذا كانت القيمة في alloca (متغير محلي) → نحتاج LOAD أولاً
                        // (EN) If value is in alloca (local variable) → need LOAD first
                        std::string actualReg = valResult.registerName;
                        SadTypeKind actualType = valResult.type;

                        if (!valResult.isConstant && !actualReg.empty() &&
                            !valResult.isFieldAccess && !valResult.isDirectValue &&
                            actualType != SadTypeKind::String && actualType != SadTypeKind::Function &&
                            valResult.className.empty())
                        {
                            std::string vn = actualReg;
                            if (!vn.empty() && vn[0] == '%')
                                vn = vn.substr(1);
                            if (b_.lookupVariable(vn))
                            {
                                std::string loadedReg = b_.newTempRegister();
                                SIRInstruction loadInst;
                                loadInst.opcode = SIROpcode::LOAD;
                                loadInst.result = SIROperand::Register(loadedReg, actualType);
                                loadInst.operands.push_back(SIROperand::Register(actualReg, actualType));
                                loadInst.comment = "load variable for finally return";
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(loadInst);
                                actualReg = loadedReg;
                            }
                        }

                        // (AR) تحديد typeCode وتخزين القيمة في السجل المناسب
                        // (EN) Determine typeCode and store value in appropriate register
                        int typeCode = 0;
                        if (actualType == SadTypeKind::Integer || actualType == SadTypeKind::Boolean)
                        {
                            typeCode = 1;
                            SIRInstruction stI;
                            stI.opcode = SIROpcode::STORE;
                            if (valResult.isConstant && !valResult.constantValue.empty())
                            {
                                if (actualType == SadTypeKind::Integer)
                                    stI.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                                else
                                    stI.operands.push_back(SIROperand::ConstantBool(valResult.constantValue == "true"));
                            }
                            else
                                stI.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stI.operands.push_back(SIROperand::Register(ctx.retValI64Reg, SadTypeKind::Integer));
                            stI.comment = "store integer finally return value";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stI);
                        }
                        else if (actualType == SadTypeKind::String || actualType == SadTypeKind::Pointer)
                        {
                            typeCode = 2;
                            SIRInstruction stP;
                            stP.opcode = SIROpcode::STORE;
                            if (valResult.isConstant && actualType == SadTypeKind::String)
                                stP.operands.push_back(SIROperand::ConstantString(valResult.constantValue));
                            else
                                stP.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stP.operands.push_back(SIROperand::Register(ctx.retValPtrReg, SadTypeKind::String));
                            stP.comment = "store string/ptr finally return value";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stP);
                        }
                        else if (actualType == SadTypeKind::Float)
                        {
                            typeCode = 3;
                            SIRInstruction stF;
                            stF.opcode = SIROpcode::STORE;
                            if (valResult.isConstant)
                                stF.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                            else
                                stF.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stF.operands.push_back(SIROperand::Register(ctx.retValI64Reg, SadTypeKind::Float));
                            stF.comment = "store float finally return value";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stF);
                        }
                        else if (!valResult.registerName.empty())
                        {
                            // (AR) نوع غير معروف — نعاملهم كرقم
                            // (EN) Unknown type — treat as integer
                            typeCode = 1;
                            SIRInstruction stU;
                            stU.opcode = SIROpcode::STORE;
                            stU.operands.push_back(SIROperand::Register(actualReg, actualType));
                            stU.operands.push_back(SIROperand::Register(ctx.retValI64Reg, SadTypeKind::Integer));
                            stU.comment = "store unknown-type finally return value as i64";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stU);
                        }

                        // (AR) تخزين typeCode في سجل النوع
                        // (EN) Store typeCode in type register
                        {
                            SIRInstruction stTC;
                            stTC.opcode = SIROpcode::STORE;
                            stTC.operands.push_back(SIROperand::ConstantI64(typeCode));
                            stTC.operands.push_back(SIROperand::Register(ctx.retTypeReg, SadTypeKind::Integer));
                            stTC.comment = "store finally return type code = " + std::to_string(typeCode);
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(stTC);
                        }
                    }
                    // (AR) ارجع بدون قيمة → type = 0 (void) — مبدئياً 0 من التهيئة
                    // (EN) Return without value → type = 0 (void) — already 0 from initialization

                    // (AR) تعيين علامة has_return = 1 لإعلام finally بوجود ارجع منتظر
                    // (EN) Set has_return = 1 to notify finally that a return is pending
                    {
                        SIRInstruction stHR;
                        stHR.opcode = SIROpcode::STORE;
                        stHR.operands.push_back(SIROperand::ConstantI64(1));
                        stHR.operands.push_back(SIROperand::Register(ctx.hasReturnReg, SadTypeKind::Integer));
                        stHR.comment = "set finally has_return = 1 (ارجع intercepted by finally)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(stHR);
                    }

                    // (AR) القفز إلى كتلة finally بدلاً من إصدار RET مباشر
                    // (EN) Branch to finally block instead of emitting direct RET
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(ctx.finallyLabel)));

                    return; // (AR) لا نُصدر RET — finally سيُصدره بعد تنفيذه
                }

                // (AR) إذا كنا داخل كوروتين، نستخدم CORO_RETURN بدلاً من RET
                // (EN) Inside a coroutine, use CORO_RETURN instead of RET
                if (b_.currentFunction_ && b_.currentFunction_->isCoroutine && retStmt->value)
                {
                    emitPopFunctionCleanupHandler();
                    BuildResult valueResult = b_.buildExpression(retStmt->value.get());
                    SIRInstruction coroRet;
                    coroRet.opcode = SIROpcode::CORO_RETURN;
                    if (valueResult.isConstant && !valueResult.constantValue.empty())
                    {
                        switch (valueResult.type)
                        {
                        case SadTypeKind::Integer:
                            coroRet.operands.push_back(SIROperand::ConstantI64(std::stoll(valueResult.constantValue)));
                            break;
                        case SadTypeKind::Float:
                            coroRet.operands.push_back(SIROperand::ConstantF64(std::stod(valueResult.constantValue)));
                            break;
                        case SadTypeKind::String:
                            coroRet.operands.push_back(SIROperand::ConstantString(valueResult.constantValue));
                            break;
                        default:
                            coroRet.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
                        }
                    }
                    else
                    {
                        coroRet.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(coroRet);
                    return;
                }

                // (AR) ReturnStmt::value: ExprPtr (statements.h:268)
                // (EN) Build return instruction
                // (AR) ISSUE-056: تخطَّ المسار العاديّ إن كان التعبير فراغاً بُني مسبقاً
                //      (وإلّا أُعيد بناؤه فتَكرّر أثره الجانبيّ) — يُعامَل كإرجاع فارغ.
                // (EN) ISSUE-056: skip the normal path when a void expr was already prebuilt
                //      (else it re-runs its side effect) — treat as a bare void return.
                if (retStmt->value && !voidSideEffectPrebuilt)
                {
                    // (AR) بناء تعبير القيمة المُرجعة
                    // (EN) Build return value expression
                    // (AR) FIX X06: استخدام القيمة المُحسوبة مسبقاً إذا توفرت
                    // (EN) FIX X06: use prebuilt return value if available
                    BuildResult valueResult;
                    if (hasPrebuiltRet)
                    {
                        // (AR) تحميل القيمة من الـ alloca المؤقت (محسوب في body block قبل defer)
                        // (EN) Load value from temp alloca (computed in body block before defer)
                        std::string loadedRetReg = b_.newTempRegister();
                        SIRInstruction loadRetInst;
                        loadRetInst.opcode = SIROpcode::LOAD;
                        loadRetInst.result = SIROperand::Register(loadedRetReg, prebuiltRetType);
                        loadRetInst.operands.push_back(SIROperand::Register(prebuiltRetReg, prebuiltRetType));
                        loadRetInst.comment = "load prebuilt return value (FIX X06)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(loadRetInst);
                        valueResult.registerName = loadedRetReg;
                        valueResult.type = prebuiltRetType;
                        valueResult.elementType = prebuiltRetElementType;
                        valueResult.className = prebuiltRetClassName;
                        valueResult.isDirectValue = true;
                    }
                    else
                    {
                        valueResult = b_.buildExpression(retStmt->value.get());
                    }

                    // ================================================================
                    // (AR) تتبع نوع الصنف المُرجع:
                    //      إذا كانت القيمة المُرجعة كائن (لديها className)، نُسجّل ذلك في
                    //      b_.functionTable_ حتى يتمكن b_.buildFunctionCall لاحقاً من معرفة أن
                    //      هذه الدالة تُرجع كائناً من صنف معيّن.
                    //      هذا ضروري لتتبع نوع الكائن عبر استدعاءات الدوال.
                    //      مثال: دالة اصنع_نقطة() → ارجع جديد نقطة(1,2) → returnClassName = "نقطة"
                    //      بدون هذا: متغير ن = اصنع_نقطة() → ن.س يتعطل لأن المترجم لا يعرف أن ن كائن
                    // (EN) Track return class type:
                    //      If the returned value is an object (has className), record it in
                    //      b_.functionTable_ so b_.buildFunctionCall can later know this function
                    //      returns an object of a specific class.
                    //      Without this: var p = makePoint() → p.x crashes because compiler
                    //      doesn't know p is an object.
                    // ================================================================
                    if (!valueResult.className.empty() && b_.currentFunction_)
                    {
                        auto ftIt = b_.functionTable_.find(b_.currentFunction_->name);
                        if (ftIt != b_.functionTable_.end())
                        {
                            ftIt->second.returnClassName = valueResult.className;
                        }
                    }
                    // (AR) [وسم زمن-التشغيل] تتبّعُ نوع عنصر المصفوفة المُرجعة عبر حدّ
                    //      الدالّة: إن أرجعت مصفوفةً مختلطةً (elementType=Any) سجّلناه في
                    //      functionTable_ ليضعه موقعُ الاستدعاء على نتيجته، فتُقرأ الفهرسةُ
                    //      اللاحقةُ موسومةً لا عدديًّا (مؤشّرٌ خام). نوسّع: أيُّ إرجاعٍ Any ⇒
                    //      الدالّةُ Any (لا نُنزِل Any مُسجَّلًا إلى محدَّدٍ من إرجاعٍ آخر).
                    //      نظيرُ كتابةِ returnClassName أعلاه لكن لعنصر المصفوفة.
                    // (EN) [runtime tags] track the returned array element type across the
                    //      function boundary: if it returns a mixed array (elementType=Any)
                    //      record it in functionTable_ so the call site stamps it on the
                    //      result and a later index reads the slot tagged, not as an integer
                    //      (raw pointer). Widen: any Any return ⇒ the function is Any (don't
                    //      downgrade a recorded Any from another return). Sibling of the
                    //      returnClassName writeback above but for the array element.
                    if (b_.currentFunction_ &&
                        valueResult.elementType != SadTypeKind::Void)
                    {
                        auto feIt = b_.functionTable_.find(b_.currentFunction_->name);
                        if (feIt != b_.functionTable_.end())
                        {
                            SadTypeKind &rt = feIt->second.returnElementType;
                            // (AR) توسيعٌ عند التنافر: أوّلُ إرجاعٍ ⇒ نوعُه؛ إرجاعٌ لاحقٌ
                            //      يخالف المُسجَّل (محدَّدان مختلفان، أو أحدهما Any) ⇒ Any
                            //      (لا نُبقي «آخِرَ إرجاعٍ يفوز»: كان يُسجّل نوعَ فرعٍ واحدٍ
                            //      فيُقرأ فرعُ العدد نصًّا ⇒ انهيار). صار التوسيعُ آمنًا بعد
                            //      إصلاح قراءة Any عند tags=null (حقل homogKind): الفرعُ
                            //      المتجانسُ يصف نفسَه زمنَ التشغيل. Any مُسجَّل يبقى Any.
                            // (EN) Widen on disagreement: the first return ⇒ its type; a later
                            //      return that differs from what's recorded (two different
                            //      concrete types, or either is Any) ⇒ Any (not "last return
                            //      wins", which recorded one branch's type so the OTHER branch's
                            //      array was misread — e.g. an int array read as a string ⇒
                            //      crash). Widening is safe now that the Any read at tags==null
                            //      is fixed (homogKind field): a homogeneous branch self-
                            //      describes at runtime. A recorded Any stays Any.
                            if (rt == SadTypeKind::Void)
                                rt = valueResult.elementType;
                            else if (rt != SadTypeKind::Any &&
                                     rt != valueResult.elementType)
                                rt = SadTypeKind::Any;
                        }
                    }

                    // (AR) أيضاً: إذا كان التعبير المُرجع هو جديد ClassName() مباشرة
                    // (EN) Also: if the return expression is directly new ClassName()
                    if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(retStmt->value.get()))
                    {
                        if (b_.currentFunction_)
                        {
                            auto ftIt = b_.functionTable_.find(b_.currentFunction_->name);
                            if (ftIt != b_.functionTable_.end() && ftIt->second.returnClassName.empty())
                            {
                                ftIt->second.returnClassName = newExpr->className;
                            }
                        }
                    }
                    // (AR) وأيضاً: إذا كان المتغير المُرجع مُسجّل في b_.classInstanceTypes_
                    // (EN) Also: if returned variable is tracked in b_.classInstanceTypes_
                    if (valueResult.className.empty() && b_.currentFunction_)
                    {
                        // (AR) التحقق من b_.classInstanceTypes_ بالاسم بدون %
                        // (EN) Check b_.classInstanceTypes_ by name without %
                        std::string varName = valueResult.registerName;
                        if (!varName.empty() && varName[0] == '%')
                        {
                            varName = varName.substr(1);
                        }
                        auto ciIt = b_.classInstanceTypes_.find(varName);
                        if (ciIt != b_.classInstanceTypes_.end())
                        {
                            auto ftIt = b_.functionTable_.find(b_.currentFunction_->name);
                            if (ftIt != b_.functionTable_.end() && ftIt->second.returnClassName.empty())
                            {
                                ftIt->second.returnClassName = ciIt->second;
                            }
                        }
                    }

                    // (AR) الشبكةُ الأخيرة: مسارٌ لم يمرّ بالبناءِ المسبَق (مُعادٌ بلا «أجّل»).
                    // (EN) Last net: a path that did not go through the pre-build (no defers).
                    emitTryHandlerPops();

                    // (AR) توليد تعليمة RET مع القيمة
                    // (EN) Generate RET instruction with value
                    SIRInstruction retInst;
                    retInst.opcode = SIROpcode::RET;

                    // (AR) إذا كانت القيمة ثابتة، أرجعها مباشرة (يجب أن يكون هذا قبل فحص %)
                    // (EN) If value is constant, return it directly (must check before % check)
                    if (valueResult.isConstant && !valueResult.constantValue.empty())
                    {
                        // (AR) القيمة ثابتة
                        // (EN) Value is constant
                        switch (valueResult.type)
                        {
                        case SadTypeKind::Integer:
                            retInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valueResult.constantValue)));
                            break;
                        case SadTypeKind::Float:
                            retInst.operands.push_back(SIROperand::ConstantF64(std::stod(valueResult.constantValue)));
                            break;
                        case SadTypeKind::Boolean:
                            retInst.operands.push_back(SIROperand::ConstantBool(valueResult.constantValue == "true"));
                            break;
                        case SadTypeKind::String:
                            retInst.operands.push_back(SIROperand::ConstantString(valueResult.constantValue));
                            break;
                        // ================================================================
                        // (AR) إرجاع بنية إغلاق (Closure)
                        //      نظام الإغلاقات الجديد: buildLambdaExpr يُنشئ CLOSURE_CREATE
                        //      ويُرجع سجلاً حقيقياً (isConstant=false)
                        //      لن نصل هنا (حالة Function مع isConstant=true) إلا إذا
                        //      كانت دالة عادية مُمررة كقيمة — نُرجعها كـ Function operand
                        // (EN) Return closure struct
                        //      New closure system: buildLambdaExpr creates CLOSURE_CREATE
                        //      and returns real register (isConstant=false)
                        //      We only reach here (Function with isConstant=true) for
                        //      regular functions passed as values — return as Function operand
                        // ================================================================
                        case SadTypeKind::Function:
                        {
                            retInst.operands.push_back(SIROperand::Function(valueResult.constantValue));
                            break;
                        }
                        default:
                            retInst.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));
                        }
                    }
                    // ================================================================
                    // (AR) فحص: هل القيمة في alloca فعلي (متغير محلي أو معامل دالة)؟
                    //      إذا كان اسم السجل يطابق متغيراً معروفاً → LOAD مطلوب
                    //      وإلا → السجل قيمة مباشرة (من عملية حسابية، استدعاء، إلخ)
                    // (EN) Check: is the value in an actual alloca (local var or param)?
                    //      If register name matches a known variable → LOAD needed
                    //      Otherwise → register is a direct value (from binary op, call, etc)
                    // ================================================================
                    else if (!valueResult.registerName.empty() && valueResult.registerName[0] == '%' && !valueResult.isFieldAccess && !valueResult.isDirectValue && valueResult.type != SadTypeKind::String && valueResult.type != SadTypeKind::Function && valueResult.className.empty())
                    {
                        // (AR) تحقق: هل السجل يشير إلى متغير محلي (alloca)؟
                        //      المتغيرات المحلية: %variableName (غير رقمية)
                        //      السجلات المؤقتة: %N (رقمية) — لا تحتاج LOAD
                        // (EN) Check: does register refer to a local variable (alloca)?
                        //      Local variables: %variableName (non-numeric)
                        //      Temp registers: %N (numeric) — no LOAD needed
                        std::string varName = valueResult.registerName.substr(1); // إزالة %
                        VariableInfo *maybeVar = b_.lookupVariable(varName);
                        bool isAllocaVar = maybeVar != nullptr;

                        // (AR) تحقق إضافي: إذا لم يكن متغيراً لكنه بادئة inlining
                        //      مثل %_inl0_variable → نحاول البحث بالاسم بعد البادئة
                        // (EN) Extra check: if not a variable but has inlining prefix
                        //      like %_inl0_variable → try looking up after prefix
                        if (!isAllocaVar && varName.size() > 4 && varName.substr(0, 4) == "_inl")
                        {
                            size_t underscorePos = varName.find('_', 4);
                            if (underscorePos != std::string::npos && underscorePos + 1 < varName.size())
                            {
                                std::string originalName = varName.substr(underscorePos + 1);
                                VariableInfo *inlinedVar = b_.lookupVariable(originalName);
                                isAllocaVar = inlinedVar != nullptr;
                            }
                        }

                        if (isAllocaVar)
                        {
                            // (AR) القيمة في عنوان alloca محلي — نحتاج لتحميلها
                            // (EN) Value is in local alloca address — need to load it
                            std::string loadedReg = b_.newTempRegister();

                            SIRInstruction loadInst;
                            loadInst.opcode = SIROpcode::LOAD;
                            loadInst.result = SIROperand::Register(loadedReg, valueResult.type);
                            loadInst.operands.push_back(SIROperand::Register(valueResult.registerName, valueResult.type));

                            if (b_.currentBlock_)
                            {
                                b_.currentBlock_->addInstruction(loadInst);
                            }

                            SIROperand retOperand = SIROperand::Register(loadedReg, valueResult.type);
                            retInst.operands.push_back(retOperand);
                        }
                        else
                        {
                            // (AR) سجل مؤقت (قيمة مباشرة) — لا نحتاج LOAD
                            // (EN) Temp register (direct value) — no LOAD needed
                            SIROperand retOperand = SIROperand::Register(valueResult.registerName, valueResult.type);
                            retInst.operands.push_back(retOperand);
                        }
                    }
                    else
                    {
                        // (AR) قيمة في سجل مؤقت
                        // (EN) Value in temporary register
                        SIROperand retOperand = SIROperand::Register(valueResult.registerName, valueResult.type);
                        retInst.operands.push_back(retOperand);
                    }

                    // ════════════════════════════════════════════════════════════
                    // (AR) 🔑 موافقةُ القيمةِ المُرجَعةِ للنوعِ المصرَّح. كان يُصرَّح عائدُ
                    //      الدالّةِ بنوعٍ ويُسلَّم إليه سجلٌّ بنوعٍ آخرَ بلا تحويل، فيقع
                    //      عطبان من جذرٍ واحد:
                    //        `دالة عشري` تُرجِع `ق // 2` ⇒ يُطبَع `2` لا `2.0`؛
                    //        `دالة رقم`  تُرجِع `ق / 2.0` ⇒ يُطبَع 4612811918334230528،
                    //      وهي **بتّاتُ** الـdouble تُقرَأ عددًا صحيحًا: لا اقتطاعٌ ولا
                    //      تقريبٌ ولا خطأ — رقمٌ لا معنى له يُطبَع بثقة.
                    //      والموضعُ مقصود: بعدَ اكتمالِ معامِلِ RET (وقد حُمِّل من alloca إن
                    //      لزم) وقبلَ إضافتِه، فالمساراتُ الثلاثةُ — ثابتٌ ومحمَّلٌ ومؤقّتٌ —
                    //      تمرّ بهذه النقطةِ وحدَها. ولو وُضِعت قبلَ التفريعِ لَحُوِّل
                    //      **عنوانُ** alloca بدل محتواه.
                    //      والثابتُ يُطوى في مكانه: تحويلُ ثابتٍ تعليمةٌ لا داعيَ لها،
                    //      وبعضُ المسارات لا تقبل معامِلًا ثابتًا للتحويل أصلًا.
                    // (EN) Coerce the returned value to the DECLARED return type. The SIR
                    //      function declared one type while a register of another was handed
                    //      to it with no conversion, producing two defects from one root:
                    //        a `عشري` (float) function returning `x // 2` printed `2`, not `2.0`;
                    //        a `رقم` (int) function returning `x / 2.0` printed
                    //        4612811918334230528 — the double's BITS read as an integer: not a
                    //        truncation, not a rounding, not an error — a meaningless number
                    //        printed with confidence.
                    //      The placement is deliberate: after the RET operand is final (loaded
                    //      from its alloca where needed) and before it is added, so all three
                    //      paths — constant, loaded, temporary — pass through this one point.
                    //      Placed before the branching, it would have converted the alloca
                    //      ADDRESS instead of its contents.
                    //      Constants are folded in place: converting a constant is a needless
                    //      instruction, and some paths do not accept a constant operand for it.
                    // ════════════════════════════════════════════════════════════
                    if (b_.currentFunction_ && retInst.operands.size() == 1)
                    {
                        const SadTypeKind declaredKind = b_.currentFunction_->returnType;
                        SIROperand &retValue = retInst.operands[0];
                        // (AR) و`Any` مع التصريحِ `عشري`: `//` على صحيحَينِ نوعُها Any عمدًا
                        //      (كِنْهُ نتيجتِها حقيقةُ زمنِ تشغيل — `INT64_MIN // -1` يفيض
                        //      فيُرقّى عشريًّا)، فكانت تُسلَّم i64 خامًا لدالّةٍ موقَّعةٍ f64
                        //      فتُطبَع `2` حيث يطبع المفسّرُ `2.0`. و`I64_TO_F64` يفكّ الوسمَ
                        //      زمنَ التشغيل — نفسُ ما تفعله المدمجاتُ لوسيطٍ ديناميّ — فيخرج
                        //      فرعا `//` كلاهما عشريَّين صحيحَين.
                        //      والعكسُ (`Any` مع `رقم`) متروكٌ عمدًا: الحمولةُ الموسومةُ قد
                        //      تكون نصًّا أو مصفوفةً، وقصرُها على i64 يُفسِدها.
                        // (EN) `Any` under a declared float: `//` on two ints is deliberately
                        //      typed Any (its result KIND is a runtime fact — INT64_MIN // -1
                        //      overflows and is promoted to float), so a raw i64 was handed to
                        //      an f64-signed function and printed `2` where the interpreter
                        //      printed `2.0`. I64_TO_F64 decodes the tag at runtime — the same
                        //      mechanism the builtins use for a dynamic argument — so both
                        //      branches of `//` come out correctly as floats.
                        //      The converse (`Any` under a declared int) is deliberately left
                        //      alone: a tagged payload may be a string or an array, and forcing
                        //      it into an i64 would corrupt it.
                        const bool needWiden = (declaredKind == SadTypeKind::Float &&
                                                (retValue.dataType == SadTypeKind::Integer ||
                                                 retValue.dataType == SadTypeKind::Any));
                        // (AR) 🔑 و«بايت» مع عائدٍ عشريٍّ كان يقعُ خارجَ هذا الحارسِ
                        //      فتُسلَّمُ بتّاتُ الـdouble خامًّا لدالّةٍ موقَّعةٍ i64:
                        //      `دالة بايت ص() ارجع 300.0` كانت تطبع 4643985272004935680
                        //      مترجَمةً — وهي بتّاتُ 300.0 مقروءةً صحيحًا، أي العطبُ
                        //      نفسُه الذي تصفُه هذه الكتلةُ أعلاه وتدّعي سدَّه. والسببُ
                        //      أنّ الحارسَ سُمّي بنوعٍ واحدٍ (`Integer`) لا بالفعلِ
                        //      (تسليمُ عشريٍّ لخانةٍ صحيحة)، فكلُّ نوعٍ صحيحٍ آخرَ
                        //      يسقطُ منه. ثمّ يقتطعُ قناعُ «بايت» أدناه الناتجَ الصحيح.
                        // (EN) Byte under a float return fell outside this guard, handing
                        //      a double's raw BITS to an i64-signed function: a `بايت`
                        //      function returning 300.0 printed 4643985272004935680 — the
                        //      very defect this block claims to seal. The guard was named
                        //      after one type (Integer) rather than after the act (handing
                        //      a float to an integer slot), so every other integer kind
                        //      fell through. The Byte mask below then truncates the result.
                        const bool needNarrow = ((declaredKind == SadTypeKind::Integer ||
                                                  declaredKind == SadTypeKind::Byte) &&
                                                 retValue.dataType == SadTypeKind::Float);
                        if (needWiden || needNarrow)
                        {
                            if (retValue.type == SIROperandType::CONSTANT)
                            {
                                retValue = needWiden
                                               ? SIROperand::ConstantF64(static_cast<double>(retValue.intValue))
                                               : SIROperand::ConstantI64(static_cast<int64_t>(retValue.floatValue));
                            }
                            else if (b_.currentBlock_)
                            {
                                std::string convReg = b_.newTempRegister();
                                SIRInstruction convInst(needWiden ? SIROpcode::I64_TO_F64
                                                                  : SIROpcode::F64_TO_I64);
                                convInst.result = SIROperand::Register(convReg, declaredKind);
                                convInst.operands.push_back(retValue);
                                convInst.comment = "coerce return value to declared type";
                                b_.currentBlock_->addInstruction(convInst);
                                retValue = SIROperand::Register(convReg, declaredKind);
                            }
                        }

                        // ════════════════════════════════════════════════════════════
                        // (AR) 🔑 «بايت» عائدًا — المعبَرُ الثالثُ الذي كان يكذبُ العقد.
                        //      مصدرُ الحقيقةِ يُعلن «بايت» u8 مدىً 0–255، والاقتطاعُ كان
                        //      مسدودًا عند التصريحِ وإعادةِ الإسنادِ وحدَهما، فكانت
                        //      `دالة بايت ص() ارجع 300` تُخرِج 300 في المحرّكَين معًا —
                        //      لا تباعُدَ بينهما بل عقدٌ غيرُ مصونٍ في كليهما.
                        //      والقناعُ نفسُ قناعِ إعادةِ الإسناد (`AND 0xFF` للسجلّ،
                        //      وطيٌّ للثابت، و`TRUNCATE_U8` للموسوم) — فيبقى معنى النوعِ
                        //      واحدًا مهما كان المعبَر.
                        // (AR) و`Any` تُفرَد بـTRUNCATE_U8 لا بـAND: حمولةُ `/` و`//`
                        //      الموسومةُ قد تكون عشريّةً زمنَ التشغيل، وقناعٌ غيرُ مشروطٍ
                        //      يُفسِد بتّاتِها. TRUNCATE_U8 يقنّع **إن كان الوسمُ صحيحًا
                        //      فقط** — وهو نفسُ ما استُعمل في مسارِ إعادةِ الإسناد.
                        // (EN) Byte as a return value — the third crossing that broke the
                        //      contract. The SoT declares Byte as u8 (0-255), yet truncation
                        //      was sealed only at declaration and reassignment, so a `بايت`
                        //      function returning 300 emitted 300 in BOTH engines — not a
                        //      divergence between them but a contract unhonoured by both.
                        //      Same masks as the reassignment path: AND 0xFF for a register,
                        //      constant folding for a constant, TRUNCATE_U8 for a tagged Any
                        //      (an unconditional mask would corrupt a float payload's bits).
                        // ════════════════════════════════════════════════════════════
                        if (declaredKind == SadTypeKind::Byte)
                        {
                            const bool intLike = (retValue.dataType == SadTypeKind::Integer ||
                                                  retValue.dataType == SadTypeKind::Byte ||
                                                  retValue.dataType == SadTypeKind::UInt64);
                            if (intLike && retValue.type == SIROperandType::CONSTANT)
                            {
                                retValue = SIROperand::ConstantI64(
                                    static_cast<int64_t>(static_cast<uint64_t>(retValue.intValue) & 0xFFULL));
                            }
                            else if (intLike && b_.currentBlock_)
                            {
                                std::string maskedReg = b_.newTempRegister();
                                SIRInstruction andInst(SIROpcode::AND);
                                andInst.result = SIROperand::Register(maskedReg, SadTypeKind::Integer);
                                andInst.operands.push_back(retValue);
                                andInst.operands.push_back(SIROperand::ConstantI64(0xFF));
                                andInst.comment = "truncate returned value to declared بايت (u8)";
                                b_.currentBlock_->addInstruction(andInst);
                                retValue = SIROperand::Register(maskedReg, SadTypeKind::Integer);
                            }
                            else if (retValue.dataType == SadTypeKind::Any &&
                                     retValue.type != SIROperandType::CONSTANT &&
                                     b_.currentBlock_)
                            {
                                std::string truncReg = b_.newTempRegister();
                                SIRInstruction truncInst(SIROpcode::TRUNCATE_U8);
                                truncInst.result = SIROperand::Register(truncReg, SadTypeKind::Any);
                                truncInst.operands.push_back(retValue);
                                truncInst.comment = "truncate tagged return to declared بايت (u8)";
                                b_.currentBlock_->addInstruction(truncInst);
                                retValue = SIROperand::Register(truncReg, SadTypeKind::Any);
                            }
                        }
                    }

                    if (b_.currentBlock_)
                    {
                        if (b_.finallyStack_.empty())
                        {
                            emitPopFunctionCleanupHandler();
                        }
                        b_.currentBlock_->addInstruction(retInst);
                    }
                }
                else
                {
                    // (AR) توليد تعليمة RET_VOID
                    // (EN) Generate RET_VOID instruction
                    SIRInstruction retInst;
                    retInst.opcode = SIROpcode::RET_VOID;
                    if (b_.currentBlock_)
                    {
                        if (b_.finallyStack_.empty())
                        {
                            emitPopFunctionCleanupHandler();
                        }
                        b_.currentBlock_->addInstruction(retInst);
                    }
                }
            }

            // ============================================================================
            // buildBreakStatement - بناء جملة break
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:417
            // التوقيع / Signature: void buildBreakStatement(AST::BreakStmt* breakStmt);
            //
            // BreakStmt (statements.h:360):
            // - لا توجد أعضاء إضافية
            // - No additional members
            // ============================================================================
            void StatementBuilder::buildBreakStatement(AST::BreakStmt *breakStmt)
            {
                if (!breakStmt)
                {
                    return;
                }

                // ========================================================================
                // (AR) جملة break: القفز إلى نهاية الحلقة الحالية
                //      نستخدم مكدس الحلقات (b_.loopStack_) للحصول على تسمية كتلة الخروج
                //      ثم نولّد تعليمة قفز غير شرطي (BR) إلى تلك الكتلة
                //
                // (EN) break statement: Jump to the end of current loop
                //      We use the loop stack (b_.loopStack_) to get the exit block label
                //      Then generate an unconditional branch (BR) to that block
                // ========================================================================
                LoopContext *loop = b_.getCurrentLoop();
                if (!loop)
                {
                    b_.errors_.push_back(controlOutsideLoopMessage(Lexer::TokenType::KEYWORD_BREAK));
                    return;
                }

                // (AR) الحاجز ٧: «قف» يخرج من «حاول» الواقعة بين الحلقة ونقطة الكسر ⇒
                //      خفّض __sad_try_active بعددها. / (EN) Barrier 7: break leaves the «try»s
                //      between the loop and the break point ⇒ decrement __sad_try_active.
                for (int i = loop->tryDepthAtEntry; i < b_.currentTryDepth_ && b_.currentBlock_; ++i)
                {
                    SIRInstruction tryExit;
                    tryExit.opcode = SIROpcode::CALL;
                    tryExit.operands.push_back(SIROperand::Function("__sad_try_exit"));
                    tryExit.comment = "barrier7: exit active try (break)";
                    b_.currentBlock_->addInstruction(tryExit);

                    SIRInstruction popInst;
                    popInst.opcode = SIROpcode::CALL;
                    popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                    popInst.comment = "barrier7: pop the exited try's handler (break)";
                    b_.currentBlock_->addInstruction(popInst);
                }

                // (AR) توليد قفز غير شرطي إلى كتلة خروج الحلقة
                // (EN) Generate unconditional branch to loop exit block
                SIROperand exitLabel = SIROperand::Label(loop->breakLabel);
                SIRInstruction brInst = SIRInstruction::Branch(exitLabel);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brInst);
                }

                // (AR) إنشاء كتلة جديدة للكود بعد break (كود ميت)
                //      هذا ضروري لأن LLVM يتطلب أن كل كتلة تنتهي بمُنهِي واحد فقط
                // (EN) Create new block for code after break (dead code)
                //      Required because LLVM needs each block to end with exactly one terminator
                std::string afterBreakLabel = b_.newLabel("after_break");
                auto afterBreakBlock = b_.createBasicBlock(afterBreakLabel);
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(afterBreakBlock);
                }
                b_.currentBlock_ = afterBreakBlock;
            }

            // ============================================================================
            // buildContinueStatement - بناء جملة continue
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:425
            // التوقيع / Signature: void buildContinueStatement(AST::ContinueStmt* continueStmt);
            //
            // ContinueStmt (statements.h:392):
            // - لا توجد أعضاء إضافية
            // - No additional members
            // ============================================================================
            void StatementBuilder::buildContinueStatement(AST::ContinueStmt *continueStmt)
            {
                if (!continueStmt)
                {
                    return;
                }

                // ========================================================================
                // (AR) جملة continue: القفز إلى بداية تكرار الحلقة التالي
                //      - في حلقة while: نقفز إلى كتلة الشرط (while_cond)
                //      - في حلقة for: نقفز إلى كتلة الزيادة (for_inc)
                //        ثم الزيادة ستقفز بدورها للشرط
                //      نستخدم continueLabel من مكدس الحلقات
                //
                // (EN) continue statement: Jump to next loop iteration
                //      - In while loop: jump to condition block (while_cond)
                //      - In for loop: jump to increment block (for_inc)
                //        then increment jumps to condition
                //      We use continueLabel from the loop stack
                // ========================================================================
                LoopContext *loop = b_.getCurrentLoop();
                if (!loop)
                {
                    b_.errors_.push_back(controlOutsideLoopMessage(Lexer::TokenType::KEYWORD_CONTINUE));
                    return;
                }

                // (AR) الحاجز ٧: «أكمل» يخرج من «حاول» الواقعة بين الحلقة ونقطة الاستمرار ⇒
                //      خفّض __sad_try_active بعددها. / (EN) Barrier 7: continue leaves the «try»s
                //      between the loop and the continue point ⇒ decrement __sad_try_active.
                for (int i = loop->tryDepthAtEntry; i < b_.currentTryDepth_ && b_.currentBlock_; ++i)
                {
                    SIRInstruction tryExit;
                    tryExit.opcode = SIROpcode::CALL;
                    tryExit.operands.push_back(SIROperand::Function("__sad_try_exit"));
                    tryExit.comment = "barrier7: exit active try (continue)";
                    b_.currentBlock_->addInstruction(tryExit);

                    SIRInstruction popInst;
                    popInst.opcode = SIROpcode::CALL;
                    popInst.operands.push_back(SIROperand::Function("__sad_pop_handler"));
                    popInst.comment = "barrier7: pop the exited try's handler (continue)";
                    b_.currentBlock_->addInstruction(popInst);
                }

                // (AR) توليد قفز غير شرطي إلى كتلة استمرار الحلقة
                // (EN) Generate unconditional branch to loop continue block
                SIROperand continueLabel = SIROperand::Label(loop->continueLabel);
                SIRInstruction brInst = SIRInstruction::Branch(continueLabel);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brInst);
                }

                // (AR) إنشاء كتلة جديدة للكود بعد continue (كود ميت)
                //      هذا ضروري لأن LLVM يتطلب أن كل كتلة تنتهي بمُنهِي واحد فقط
                // (EN) Create new block for code after continue (dead code)
                //      Required because LLVM needs each block to end with exactly one terminator
                std::string afterContinueLabel = b_.newLabel("after_continue");
                auto afterContinueBlock = b_.createBasicBlock(afterContinueLabel);
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(afterContinueBlock);
                }
                b_.currentBlock_ = afterContinueBlock;
            }

            // ============================================================================
            // buildAssignment - بناء إسناد متغير
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:429
            // التوقيع / Signature: void buildAssignment(AST::AssignExpr* assignment);
            //
            // المعاملات / Parameters:
            // - assignment: AST::AssignExpr* = Sad::AST::AssignExpr* (sir_builder.h:429)
            //
            // AssignExpr Members (expressions.h:247):
            // - name: std::string (line 249)
            // - value: ExprPtr (line 250)
            //
            // الدوال المستدعاة / Called functions:
            // - b_.buildExpression: sir_builder.h:432
            // - b_.lookupVariable: sir_builder.h:597
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad

