// ============================================================================
// sir_builder_control_flow.cpp
// (AR) Loops: buildWhileLoop, buildForLoop, buildForRangeLoop
// (EN) Loop control flow: while, for, for-range loops
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "sir_constants.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
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
            // (AR) أسماء دوال جمع مفاتيح/قيم الخريطة موحَّدة في sir_constants.h
            //      (kRuntimeMapKeys/kRuntimeMapValues) — مشتركة مع باني الاستيعابات.
            // (EN) Map keys/values collector names are unified in sir_constants.h
            //      (kRuntimeMapKeys/kRuntimeMapValues) — shared with the comprehension builder.

            // ============================================================================
            // buildForRangeLoop - بناء حلقة لكل في (foreach)
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:405
            // التوقيع / Signature: void buildForRangeLoop(AST::ForRangeStmt* forRange);
            //
            // المعاملات / Parameters:
            // - forRange: AST::ForRangeStmt* = Sad::AST::ForRangeStmt* (statements.h:228)
            //
            // ForRangeStmt Members (statements.h:228-235):
            // - variable: std::string (line 230) - اسم متغير الحلقة
            // - valueVar: std::string (line 231) - متغير القيمة للقواميس (اختياري)
            // - iterable: ExprPtr (line 232) - التعبير القابل للتكرار
            // - body: StmtPtr (line 233) - جسم الحلقة
            // ============================================================================
            void StatementBuilder::buildForRangeLoop(AST::ForRangeStmt *forRange)
            {
                if (!forRange)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForRangeLoop: starting, variable='"
                          << forRange->variable << "'" << std::endl;
#endif

                // ========================================================================
                // (AR) مسار خاص: إذا كان التعبير نطاقاً (RangeExpr) نولّد حلقة while بسيطة
                // (EN) Special path: if iterable is RangeExpr, generate simple while loop
                // ========================================================================
                if (auto *rangeExpr = dynamic_cast<Sad::AST::RangeExpr *>(forRange->iterable.get()))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForRangeLoop: detected RangeExpr, using while-style loop" << std::endl;
#endif

                    b_.enterScope();

                    // (AR) بناء بداية ونهاية النطاق
                    auto startResult = b_.buildExpression(rangeExpr->start.get());
                    auto endResult = b_.buildExpression(rangeExpr->end.get());

                    // ================================================================
                    // (AR) [Fix #47] تحديد اتجاه المدى — صعودي أو تنازلي:
                    //      لكل ع من 1 الى 5 → صعودي (LE, +1)
                    //      لكل ع من 5 الى 1 → تنازلي (GE, -1)
                    //      لكل ع من س الى ص → يُحدد في وقت التشغيل
                    //      بدون هذا: لكل ع من 5 الى 1 لا ينفذ الحلقة أبداً
                    //      لأن الشرط 5 <= 1 خطأ من أول تكرار
                    // (EN) [Fix #47] Determine range direction — ascending or descending:
                    //      for i from 1 to 5 → ascending (LE, +1)
                    //      for i from 5 to 1 → descending (GE, -1)
                    //      for i from x to y → determined at runtime
                    //      Without this: for i from 5 to 1 never executes because 5<=1 is false
                    // ================================================================
                    bool isDescending = false;
                    bool isStaticDirection = false;

                    if (startResult.isConstant && endResult.isConstant)
                    {
                        try
                        {
                            int64_t startVal = std::stoll(startResult.constantValue);
                            int64_t endVal = std::stoll(endResult.constantValue);
                            isDescending = (startVal > endVal);
                            isStaticDirection = true;
                        }
                        catch (...)
                        {
                        }
                    }

                    // (AR) إنشاء الكتل الأساسية
                    std::string condL = b_.newLabel("range_cond");
                    std::string bodyL = b_.newLabel("range_body");
                    std::string incL = b_.newLabel("range_inc");
                    std::string exitL = b_.newLabel("range_exit");

                    auto condB = b_.createBasicBlock(condL);
                    auto bodyB = b_.createBasicBlock(bodyL);
                    auto incB = b_.createBasicBlock(incL);
                    auto exitB = b_.createBasicBlock(exitL);

                    // (AR) كتل إضافية للمدى الديناميكي — تحديد الاتجاه في وقت التشغيل
                    // (EN) Extra blocks for dynamic range — determine direction at runtime
                    std::shared_ptr<SIRBasicBlock> stepAscB, stepDescB;
                    std::string stepAscL, stepDescL;
                    if (!isStaticDirection)
                    {
                        stepAscL = b_.newLabel("step_asc");
                        stepDescL = b_.newLabel("step_desc");
                        stepAscB = b_.createBasicBlock(stepAscL);
                        stepDescB = b_.createBasicBlock(stepDescL);
                    }

                    if (b_.currentFunction_)
                    {
                        if (!isStaticDirection)
                        {
                            b_.currentFunction_->addBasicBlock(stepAscB);
                            b_.currentFunction_->addBasicBlock(stepDescB);
                        }
                        b_.currentFunction_->addBasicBlock(condB);
                        b_.currentFunction_->addBasicBlock(bodyB);
                        b_.currentFunction_->addBasicBlock(incB);
                        b_.currentFunction_->addBasicBlock(exitB);
                    }

                    // ========================================================================
                    // (AR) موقعُ العدّادِ يسكنُ فضاءَ أسماءٍ لا يستطيعُ المستخدمُ كتابتَه.
                    //      بلا فصلٍ يتقاسمُ العدّادُ الموقعَ `%<اسم>` مع أيِّ `متغير` بالاسمِ
                    //      نفسِه داخلَ الجسم، فيكتبُ التصريحُ في موقعِ التحكّمِ ويُفسدُ عدَّ
                    //      الدورات: «لكل ي من 1 الى 3» ثمّ «متغير ي = 100» كان يعطي دورةً
                    //      واحدةً (أو حلقةً لا نهائيّةً قبلَ م‑١٠) والمفسّرُ يعطي ثلاثًا.
                    //      ولا تكفي لاحقةٌ من حروفٍ وأرقامٍ (`%ي_5`) لأنّها **تهجئةٌ مشروعةٌ
                    //      لمعرّفِ مستخدم**: `متغير ي_5` كان يُعيدُ التصادمَ عينَه بصورتِه
                    //      الأسوأ — حلقةٌ لا نهائيّة (ولا يُذكَرُ عددُ الأسطرِ ثابتًا: هو أثرُ
                    //      مهلةٍ لا قيمةٌ قابلةٌ لإعادةِ الإنتاج). لذا الفاصلُ المسمّى
                    //      `kSlotNamespaceSeparator` — انظرْ تعليلَه في sir_builder.h.
                    // (EN) The counter slot lives in a namespace the user cannot spell. Without
                    //      separation it shares `%<name>` with any same-named declaration in the
                    //      body, which writes into the control slot and corrupts the iteration
                    //      count. An alphanumeric suffix (`%ي_5`) is NOT enough — it is a legal
                    //      user identifier, and `متغير ي_5` reproduced the collision as an
                    //      infinite loop. Hence `#`, which starts a comment in Sad and therefore
                    //      can never occur inside an identifier.
                    // ========================================================================
                    std::string rangeIdxSuffix = condL.substr(condL.find_last_of('_') + 1);
                    std::string loopVarAlloc = "%" + forRange->variable + kSlotNamespaceSeparator + rangeIdxSuffix;
                    {
                        SIRInstruction allocInst(SIROpcode::ALLOC);
                        allocInst.result = SIROperand::Register(loopVarAlloc, SadTypeKind::Integer);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocInst);
                    }
                    // (AR) دالة مساعدة: تحويل BuildResult إلى SIROperand مع مراعاة الثوابت
                    // (EN) Helper: convert BuildResult to SIROperand, handling constants
                    auto resultToOperand = [](const BuildResult &r) -> SIROperand
                    {
                        if (r.isConstant && !r.constantValue.empty())
                        {
                            if (r.type == SadTypeKind::Integer)
                            {
                                return SIROperand::ConstantI64(std::stoll(r.constantValue));
                            }
                            else if (r.type == SadTypeKind::Float)
                            {
                                return SIROperand::ConstantF64(std::stod(r.constantValue));
                            }
                        }
                        return SIROperand::Register(r.registerName, r.type);
                    };

                    // (AR) تهيئة المتغير بقيمة البداية
                    {
                        SIRInstruction storeInit(SIROpcode::STORE);
                        storeInit.operands.push_back(resultToOperand(startResult));
                        storeInit.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeInit);
                    }

                    // (AR) تخصيص alloca لنهاية النطاق
                    std::string endAlloc = b_.newTempRegister();
                    {
                        SIRInstruction allocEnd(SIROpcode::ALLOC);
                        allocEnd.result = SIROperand::Register(endAlloc, SadTypeKind::Integer);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocEnd);
                    }
                    {
                        SIRInstruction storeEnd(SIROpcode::STORE);
                        storeEnd.operands.push_back(resultToOperand(endResult));
                        storeEnd.operands.push_back(SIROperand::Register(endAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeEnd);
                    }

                    // (AR) تسجيل متغير الحلقة
                    VariableInfo varInfo;
                    varInfo.name = forRange->variable;
                    varInfo.registerName = loopVarAlloc;
                    varInfo.type = SadTypeKind::Integer;
                    varInfo.isMutable = true;
                    // (AR) موقعُه يملكُه شرطُ الحلقةِ لا تصريحٌ داخلَ الجسم — انظر
                    //      `isLoopControl` في sir_builder.h و`priorIsReusableSlot`.
                    // (EN) Owned by the loop condition, not by a body declaration.
                    varInfo.isLoopControl = true;
                    b_.addVariable(varInfo);

                    // ================================================================
                    // (AR) [Fix #47] تخصيص alloca لخطوة المدى وتهيئتها:
                    //      ثابت صعودي: step = 1
                    //      ثابت تنازلي: step = -1
                    //      ديناميكي: فحص start <= end في وقت التشغيل
                    // ================================================================
                    std::string stepAlloc = b_.newTempRegister();
                    {
                        SIRInstruction allocStep(SIROpcode::ALLOC);
                        allocStep.result = SIROperand::Register(stepAlloc, SadTypeKind::Integer);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocStep);
                    }

                    if (isStaticDirection)
                    {
                        // (AR) الاتجاه معروف وقت الترجمة — نخزن الخطوة مباشرة
                        SIRInstruction storeStep(SIROpcode::STORE);
                        storeStep.operands.push_back(SIROperand::ConstantI64(isDescending ? -1 : 1));
                        storeStep.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                        storeStep.comment = isDescending ? "step = -1 (descending)" : "step = 1 (ascending)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeStep);

                        // القفز إلى الشرط
                        SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condL));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(br);
                    }
                    else
                    {
                        // (AR) الاتجاه غير معروف — نفحص start <= end في وقت التشغيل
                        //      ونقفز لكتلة step_asc أو step_desc لتعيين الخطوة
                        // (EN) Unknown direction — check start <= end at runtime
                        //      branch to step_asc or step_desc to set step value
                        std::string loadS = b_.newTempRegister();
                        {
                            SIRInstruction ls(SIROpcode::LOAD);
                            ls.result = SIROperand::Register(loadS, SadTypeKind::Integer);
                            ls.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(ls);
                        }
                        std::string loadE = b_.newTempRegister();
                        {
                            SIRInstruction le(SIROpcode::LOAD);
                            le.result = SIROperand::Register(loadE, SadTypeKind::Integer);
                            le.operands.push_back(SIROperand::Register(endAlloc, SadTypeKind::Integer));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(le);
                        }
                        std::string dirCmp = b_.newTempRegister();
                        {
                            SIRInstruction cmp = SIRInstruction::Binary(
                                SIROpcode::LE, SIROperand::Register(dirCmp, SadTypeKind::Boolean),
                                SIROperand::Register(loadS, SadTypeKind::Integer),
                                SIROperand::Register(loadE, SadTypeKind::Integer));
                            cmp.comment = "start <= end? (direction check)";
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(cmp);
                        }
                        {
                            SIRInstruction brDir = SIRInstruction::BranchCond(
                                SIROperand::Register(dirCmp, SadTypeKind::Boolean),
                                SIROperand::Label(stepAscL), SIROperand::Label(stepDescL));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(brDir);
                        }

                        // -- كتلة step_asc: تخزين step = 1 --
                        b_.currentBlock_ = stepAscB;
                        {
                            SIRInstruction store(SIROpcode::STORE);
                            store.operands.push_back(SIROperand::ConstantI64(1));
                            store.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                            store.comment = "step = 1 (ascending)";
                            b_.currentBlock_->instructions.push_back(store);
                        }
                        {
                            SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condL));
                            b_.currentBlock_->instructions.push_back(br);
                        }

                        // -- كتلة step_desc: تخزين step = -1 --
                        b_.currentBlock_ = stepDescB;
                        {
                            SIRInstruction store(SIROpcode::STORE);
                            store.operands.push_back(SIROperand::ConstantI64(-1));
                            store.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                            store.comment = "step = -1 (descending)";
                            b_.currentBlock_->instructions.push_back(store);
                        }
                        {
                            SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condL));
                            b_.currentBlock_->instructions.push_back(br);
                        }
                    }

                    // ---- كتلة الشرط ----
                    b_.currentBlock_ = condB;
                    std::string loadedVar = b_.newTempRegister();
                    {
                        SIRInstruction loadV(SIROpcode::LOAD);
                        loadV.result = SIROperand::Register(loadedVar, SadTypeKind::Integer);
                        loadV.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(loadV);
                    }
                    std::string loadedEnd = b_.newTempRegister();
                    {
                        SIRInstruction loadE(SIROpcode::LOAD);
                        loadE.result = SIROperand::Register(loadedEnd, SadTypeKind::Integer);
                        loadE.operands.push_back(SIROperand::Register(endAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(loadE);
                    }
                    std::string cmpReg = b_.newTempRegister();
                    {
                        // ================================================================
                        // (AR) [Fix #47] الشرط يعتمد على اتجاه المدى:
                        //      صعودي (ثابت): LE (أصغر أو يساوي)
                        //      تنازلي (ثابت): GE (أكبر أو يساوي)
                        //      ديناميكي: نفحص step > 0 ونختار LE أو GE
                        //      في الحالة الديناميكية نُنشئ كتلتي شرط منفصلتين
                        // (EN) [Fix #47] Condition depends on range direction:
                        //      ascending (static): LE
                        //      descending (static): GE
                        //      dynamic: check step > 0, choose LE or GE
                        // ================================================================
                        if (isStaticDirection)
                        {
                            SIROpcode cmpOp = isDescending ? SIROpcode::GE : SIROpcode::LE;
                            SIRInstruction cmp = SIRInstruction::Binary(
                                cmpOp, SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                SIROperand::Register(loadedVar, SadTypeKind::Integer),
                                SIROperand::Register(loadedEnd, SadTypeKind::Integer));
                            cmp.comment = isDescending ? "i >= end (descending)" : "i <= end (ascending)";
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(cmp);
                        }
                        else
                        {
                            // (AR) ديناميكي: نقرأ step، إذا step > 0 → LE، وإلا → GE
                            // (EN) Dynamic: read step, if step > 0 → LE, else → GE
                            std::string loadStep = b_.newTempRegister();
                            {
                                SIRInstruction ls(SIROpcode::LOAD);
                                ls.result = SIROperand::Register(loadStep, SadTypeKind::Integer);
                                ls.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(ls);
                            }
                            std::string stepPos = b_.newTempRegister();
                            {
                                SIRInstruction sp = SIRInstruction::Binary(
                                    SIROpcode::GT, SIROperand::Register(stepPos, SadTypeKind::Boolean),
                                    SIROperand::Register(loadStep, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(0));
                                sp.comment = "step > 0?";
                                b_.currentBlock_->instructions.push_back(sp);
                            }
                            // (AR) كتل شرط فرعية: le_cond و ge_cond
                            std::string leCL = b_.newLabel("le_cond");
                            std::string geCL = b_.newLabel("ge_cond");
                            std::string mergeCL = b_.newLabel("cond_merge");
                            auto leCB = b_.createBasicBlock(leCL);
                            auto geCB = b_.createBasicBlock(geCL);
                            auto mergeCB = b_.createBasicBlock(mergeCL);
                            if (b_.currentFunction_)
                            {
                                b_.currentFunction_->addBasicBlock(leCB);
                                b_.currentFunction_->addBasicBlock(geCB);
                                b_.currentFunction_->addBasicBlock(mergeCB);
                            }
                            {
                                SIRInstruction br = SIRInstruction::BranchCond(
                                    SIROperand::Register(stepPos, SadTypeKind::Boolean),
                                    SIROperand::Label(leCL), SIROperand::Label(geCL));
                                b_.currentBlock_->instructions.push_back(br);
                            }
                            // -- LE كتلة --
                            b_.currentBlock_ = leCB;
                            std::string leRes = b_.newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::LE, SIROperand::Register(leRes, SadTypeKind::Boolean),
                                    SIROperand::Register(loadedVar, SadTypeKind::Integer),
                                    SIROperand::Register(loadedEnd, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(cmp);
                            }
                            {
                                SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(mergeCL));
                                b_.currentBlock_->instructions.push_back(br);
                            }
                            // -- GE كتلة --
                            b_.currentBlock_ = geCB;
                            std::string geRes = b_.newTempRegister();
                            {
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::GE, SIROperand::Register(geRes, SadTypeKind::Boolean),
                                    SIROperand::Register(loadedVar, SadTypeKind::Integer),
                                    SIROperand::Register(loadedEnd, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(cmp);
                            }
                            {
                                SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(mergeCL));
                                b_.currentBlock_->instructions.push_back(br);
                            }
                            // -- دمج: PHI لاختيار النتيجة الصحيحة --
                            b_.currentBlock_ = mergeCB;
                            {
                                SIRInstruction phi(SIROpcode::PHI);
                                phi.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                                phi.operands.push_back(SIROperand::Register(leRes, SadTypeKind::Boolean));
                                phi.operands.push_back(SIROperand::Label(leCL));
                                phi.operands.push_back(SIROperand::Register(geRes, SadTypeKind::Boolean));
                                phi.operands.push_back(SIROperand::Label(geCL));
                                phi.comment = "range direction PHI";
                                b_.currentBlock_->instructions.push_back(phi);
                            }
                        }
                    }
                    {
                        SIRInstruction brCond = SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(bodyL), SIROperand::Label(exitL));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brCond);
                    }

                    // ---- كتلة الجسم ----
                    // ================================================================
                    // (AR) م-٩: تسجيلُ سياقِ الحلقةِ لـ«توقف»/«استمر». كان هذا المسارُ
                    //      وحدَه بلا تسجيل، فيرفعُ المترجِمُ «جملة 'قف' خارج حلقة»
                    //      على برنامجٍ يُنفّذه المفسِّرُ سليمًا — تباعُدُ محرِّكَين لا
                    //      قيدَ لغة. المساران الآخران في هذا الملفّ (القناة والتكرار)
                    //      يُسجّلانه أصلًا، والكتلتان هنا موجودتان
                    //      (range_inc / range_exit) — فالناقصُ التسجيلُ لا البنية.
                    //      «استمر» ⇒ كتلةُ الزيادة (تزيد ثمّ تفحص الشرط)،
                    //      و«توقف» ⇒ كتلةُ الخروج.
                    // (EN) م-٩: register the loop context for break/continue. This was
                    //      the only path in this file that skipped it, so the compiler
                    //      rejected «توقف» inside «لكل ي من ١ الى ٣» while the
                    //      interpreter ran it fine — an engine divergence, not a rule.
                    //      Both blocks already exist; only the registration was missing.
                    // ================================================================
                    LoopContext rangeLoopCtx;
                    rangeLoopCtx.continueLabel = incL;
                    rangeLoopCtx.breakLabel = exitL;
                    b_.enterLoop(rangeLoopCtx);

                    b_.currentBlock_ = bodyB;
                    if (forRange->body)
                    {
                        buildStatement(forRange->body.get());
                    }

                    b_.exitLoop();
                    {
                        SIRInstruction brInc = SIRInstruction::Branch(SIROperand::Label(incL));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brInc);
                    }

                    // ---- كتلة الزيادة ----
                    b_.currentBlock_ = incB;
                    std::string loadedInc = b_.newTempRegister();
                    {
                        SIRInstruction loadI(SIROpcode::LOAD);
                        loadI.result = SIROperand::Register(loadedInc, SadTypeKind::Integer);
                        loadI.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(loadI);
                    }
                    // ================================================================
                    // (AR) [Fix #47] استخدام step بدل +1 الثابت:
                    //      صعودي → i = i + 1
                    //      تنازلي → i = i + (-1) = i - 1
                    //      ديناميكي → i = i + step (حيث step = 1 أو -1)
                    // (EN) [Fix #47] Use step instead of hardcoded +1:
                    //      ascending → i = i + 1
                    //      descending → i = i + (-1) = i - 1
                    //      dynamic → i = i + step (where step = 1 or -1)
                    // ================================================================
                    std::string newVal = b_.newTempRegister();
                    if (isStaticDirection)
                    {
                        SIRInstruction addInst = SIRInstruction::Binary(
                            SIROpcode::ADD_I64, SIROperand::Register(newVal, SadTypeKind::Integer),
                            SIROperand::Register(loadedInc, SadTypeKind::Integer),
                            SIROperand::ConstantI64(isDescending ? -1 : 1));
                        addInst.comment = isDescending ? "i = i - 1 (descending)" : "i = i + 1 (ascending)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(addInst);
                    }
                    else
                    {
                        // (AR) ديناميكي: نقرأ step ونضيفه
                        std::string loadStep = b_.newTempRegister();
                        {
                            SIRInstruction ls(SIROpcode::LOAD);
                            ls.result = SIROperand::Register(loadStep, SadTypeKind::Integer);
                            ls.operands.push_back(SIROperand::Register(stepAlloc, SadTypeKind::Integer));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(ls);
                        }
                        SIRInstruction addInst = SIRInstruction::Binary(
                            SIROpcode::ADD_I64, SIROperand::Register(newVal, SadTypeKind::Integer),
                            SIROperand::Register(loadedInc, SadTypeKind::Integer),
                            SIROperand::Register(loadStep, SadTypeKind::Integer));
                        addInst.comment = "i = i + step (dynamic direction)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(addInst);
                    }
                    {
                        SIRInstruction storeNew(SIROpcode::STORE);
                        storeNew.operands.push_back(SIROperand::Register(newVal, SadTypeKind::Integer));
                        storeNew.operands.push_back(SIROperand::Register(loopVarAlloc, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeNew);
                    }
                    {
                        SIRInstruction brBack = SIRInstruction::Branch(SIROperand::Label(condL));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brBack);
                    }

                    // ---- كتلة الخروج ----
                    b_.currentBlock_ = exitB;
                    b_.exitScope();
                    return;
                }

                // ========================================================================
                // (AR) الخطوة 1: دخول نطاق جديد للحلقة
                // (EN) Step 1: Enter new scope for loop
                // ========================================================================
                b_.enterScope();

                // ========================================================================
                // (AR) الخطوة 2: بناء التعبير القابل للتكرار
                // (EN) Step 2: Build iterable expression
                // المصدر: ForRangeStmt::iterable (statements.h:232)
                // ========================================================================
                auto iterableResult = b_.buildExpression(forRange->iterable.get());

                if (iterableResult.registerName.empty())
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForRangeLoop: failed to build iterable!" << std::endl;
#endif
                    b_.errors_.push_back("Error: Failed to build iterable expression in for-range");
                    b_.exitScope();
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForRangeLoop: iterable reg='"
                          << iterableResult.registerName << "'" << std::endl;
#endif

                // ========================================================================
                // (AR) مسار الخريطة: المترجم كان يعامل الخريطة كمصفوفة (ARRAY_GET على بنية
                //      الخريطة {count,cap,keys,values,types} ⇒ قمامة). الحلّ: نُكرّر على
                //      مفاتيح الخريطة عبر __sad_map_keys (تُرجع SadArray {len,cap,data})،
                //      ونربط متغيّر القيمة valueVar بالقيم عبر __sad_map_values إن وُجد.
                //      يطابق دلالة المفسّر «لكل مفتاح[، قيمة] في خريطة» (statement_executor).
                // (EN) Map path: the compiler used to treat a map as an array (ARRAY_GET on
                //      the map struct ⇒ garbage). Fix: iterate the map's keys via __sad_map_keys
                //      (returns a SadArray), binding valueVar to values via __sad_map_values
                //      when present — mirrors the interpreter's «for key[, value] in map».
                // ========================================================================
                // (AR) فارغ ما لم نُكرّر خريطة بمتغيّر قيمة / (EN) empty unless iterating a map with a value var
                std::string mapValuesReg;
                // (AR) فتحة alloc لمتغيّر القيمة (تُضبَط أدناه) / (EN) alloc slot for the value var (set below)
                std::string valueVarAllocName;
                // (AR) نوع متغيّر القيمة المحسوم (يُستخدَم في alloc والربط بالجسم) / (EN) resolved value-var type (used at alloc and body bind)
                SadTypeKind valueVarType = SadTypeKind::Integer;
                // (AR) نوع قيمة الخريطة كما تعقّبه بانٍ الحرفيّ (Void = مختلط/مجهول) — يُلتقَط قبل دهس elementType.
                // (EN) Map value type as tracked by the literal builder (Void = heterogeneous/unknown) — captured before elementType is overwritten.
                SadTypeKind mapValueType = SadTypeKind::Void;
                // (AR) قيود موثَّقة (راجعتها Amelia): (1) مصفوفتا المفاتيح/القيم المُعادتان من
                //      __sad_map_* تُخصَّصان ولا تُحرَّران هنا — تسريب متّسق مع بقيّة مجمِّعات
                //      المصفوفات في الخلفيّة، يُترك لإدارة الذاكرة العامّة لا لهذا المسار. (2) لو
                //      وُضِع valueVar على غير خريطة (مصفوفة)، لا يُخصَّص هنا فيصير رمزًا غير معرَّف
                //      وقت البناء — يوازي سلوك المفسّر (يترك valueVar غير معرَّف للمصفوفات).
                // (EN) Documented limitations (reviewed by Amelia): (1) the keys/values arrays
                //      returned by __sad_map_* are allocated and not freed here — a leak consistent
                //      with the backend's other array collectors, left to global memory management.
                //      (2) a valueVar on a non-map (array) is not allocated here, so it surfaces as
                //      an undefined symbol at build time — mirroring the interpreter (which leaves
                //      valueVar undefined for arrays).
                if (iterableResult.type == SadTypeKind::Map)
                {
                    // (AR) بانٍ حرفيّ الخريطة يحفظ نوع القيمة الموحَّد في elementType (Void إن مختلطًا)
                    //      — انظر buildExprMap في expression_collections.cpp. نلتقطه لتصنيف valueVar.
                    // (EN) The map-literal builder stores the homogeneous value type in elementType
                    //      (Void if mixed) — see buildExprMap. Capture it to type valueVar.
                    mapValueType = iterableResult.elementType;

                    std::string keysReg = b_.newTempRegister();
                    {
                        SIRInstruction c(SIROpcode::CALL);
                        c.result = SIROperand::Register(keysReg, SadTypeKind::Array);
                        c.operands.push_back(SIROperand::ConstantString(kRuntimeMapKeys));
                        c.operands.push_back(SIROperand::Register(iterableResult.registerName, iterableResult.type));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(c);
                    }
                    if (!forRange->valueVar.empty())
                    {
                        mapValuesReg = b_.newTempRegister();
                        SIRInstruction c(SIROpcode::CALL);
                        c.result = SIROperand::Register(mapValuesReg, SadTypeKind::Array);
                        c.operands.push_back(SIROperand::ConstantString(kRuntimeMapValues));
                        c.operands.push_back(SIROperand::Register(iterableResult.registerName, iterableResult.type));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(c);
                    }
                    // (AR) استبدل المصدر بمصفوفة المفاتيح — فتعمل آليّة ARRAY_LEN/GET التالية عليها.
                    //      المفاتيح نصّيّة دائمًا: الخريطة تُخزّنها بـ strdup وتحوّل المفاتيح العدديّة
                    //      إلى نصّ (ISSUE-044 في buildExprMap)، فنوع عنصر مصفوفة المفاتيح = String.
                    // (EN) Replace the iterable with the keys array so the following ARRAY_LEN/GET works.
                    //      Keys are always strings: the map strdup's them and converts numeric keys to
                    //      text (ISSUE-044 in buildExprMap), so the keys-array element type is String.
                    iterableResult.registerName = keysReg;
                    iterableResult.type = SadTypeKind::Array;
                    iterableResult.elementType = SadTypeKind::String;
                    iterableResult.className.clear();
                    iterableResult.elementClassName.clear();
                }

                // ========================================================================
                // (AR) مسار خاص: إذا كان التعبير قناة — حلقة استقبال من القناة
                // (EN) Special path: if iterable is a channel — channel receive loop
                //
                // (AR) بنية الحلقة:
                //   [chan_cond]  — فحص وجود بيانات في القناة
                //   [chan_check] — إذا لا بيانات، فحص إغلاق القناة → خروج أو انتظار
                //   [chan_body]  — استقبال + تنفيذ الجسم
                //   [chan_exit]  — بعد انتهاء القناة
                // ========================================================================
                if (iterableResult.className == "__channel__")
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForRangeLoop: detected channel iterable" << std::endl;
#endif

                    // (AR) إنشاء الكتل الأساسية لحلقة القناة
                    // (EN) Create basic blocks for channel loop
                    std::string condLabel = b_.newLabel("chan_cond");
                    std::string checkLabel = b_.newLabel("chan_check_closed");
                    std::string bodyLabel = b_.newLabel("chan_body");
                    std::string exitLabel = b_.newLabel("chan_exit");

                    auto condBlock = b_.createBasicBlock(condLabel);
                    auto checkBlock = b_.createBasicBlock(checkLabel);
                    auto bodyBlock = b_.createBasicBlock(bodyLabel);
                    auto exitBlock = b_.createBasicBlock(exitLabel);

                    if (b_.currentFunction_)
                    {
                        b_.currentFunction_->addBasicBlock(condBlock);
                        b_.currentFunction_->addBasicBlock(checkBlock);
                        b_.currentFunction_->addBasicBlock(bodyBlock);
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    }

                    // (AR) تخصيص متغير الحلقة وتحديد نوعه من b_.channelTypeMap_
                    // (EN) Allocate loop variable and infer its type from b_.channelTypeMap_
                    // (AR) الفاصلُ `#` كما في صيغتَي المدى والمجموعة: يمنعُ تصريحًا داخلَ
                    //      الجسمِ بالاسمِ نفسِه من اختطافِ موقعِ عنصرِ القناة.
                    // (EN) `#` separator as in the range/foreach forms: keeps a same-named body
                    //      declaration from hijacking the channel element slot.
                    std::string loopVarAllocName =
                        "%" + forRange->variable + kSlotNamespaceSeparator +
                        condLabel.substr(condLabel.find_last_of('_') + 1);
                    SadTypeKind chanElemType = SadTypeKind::Integer;
                    {
                        // (AR) البحث عن نوع عنصر القناة من اسم السجل أو اسم المتغير الأصلي
                        // (EN) Look up the channel element type by register or source variable name
                        auto ctIt = b_.channelTypeMap_.find(iterableResult.registerName);
                        if (ctIt != b_.channelTypeMap_.end())
                        {
                            chanElemType = ctIt->second;
                        }
                        else if (auto *iterVar = dynamic_cast<Sad::AST::VariableExpr *>(
                                     forRange->iterable.get()))
                        {
                            ctIt = b_.channelTypeMap_.find(iterVar->name);
                            if (ctIt != b_.channelTypeMap_.end())
                            {
                                chanElemType = ctIt->second;
                            }
                        }
                    }
                    {
                        SIRInstruction allocLoop(SIROpcode::ALLOC);
                        allocLoop.result = SIROperand::Register(loopVarAllocName, chanElemType);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocLoop);
                    }

                    // (AR) تسجيل متغير الحلقة بالنوع المستنتج
                    // (EN) Register the loop variable with the inferred type
                    VariableInfo chanVarInfo;
                    chanVarInfo.name = forRange->variable;
                    chanVarInfo.registerName = loopVarAllocName;
                    chanVarInfo.type = chanElemType;
                    chanVarInfo.isMutable = true;
                    b_.addVariable(chanVarInfo);

                    // (AR) القفز إلى كتلة الشرط
                    // (EN) Jump to condition block
                    {
                        SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(br);
                    }

                    // ---- كتلة الشرط: هل يوجد بيانات؟ ----
                    // ---- Condition: does channel have data? ----
                    b_.currentBlock_ = condBlock;
                    SIROperand chanOp = SIROperand::Register(iterableResult.registerName, iterableResult.type);

                    std::string hasDataReg = b_.newTempRegister();
                    {
                        SIRInstruction hasDataInst(SIROpcode::ASYNC_CHANNEL_HAS_DATA);
                        hasDataInst.result = SIROperand::Register(hasDataReg, SadTypeKind::Integer);
                        hasDataInst.operands.push_back(chanOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(hasDataInst);
                    }

                    // (AR) مقارنة: has_data != 0 → منطقي
                    // (EN) Compare: has_data != 0 → bool
                    std::string hasDataBool = b_.newTempRegister();
                    {
                        SIRInstruction cmp = SIRInstruction::Binary(
                            SIROpcode::NE, SIROperand::Register(hasDataBool, SadTypeKind::Boolean),
                            SIROperand::Register(hasDataReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(0));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(cmp);
                    }

                    // BR_COND: has_data → body, else → check_closed
                    {
                        SIRInstruction brCond = SIRInstruction::BranchCond(
                            SIROperand::Register(hasDataBool, SadTypeKind::Boolean),
                            SIROperand::Label(bodyLabel),
                            SIROperand::Label(checkLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brCond);
                    }

                    // ---- كتلة فحص الإغلاق ----
                    // ---- Check closed block ----
                    b_.currentBlock_ = checkBlock;

                    std::string isClosedReg = b_.newTempRegister();
                    {
                        SIRInstruction isClosedInst(SIROpcode::ASYNC_CHANNEL_IS_CLOSED);
                        isClosedInst.result = SIROperand::Register(isClosedReg, SadTypeKind::Integer);
                        isClosedInst.operands.push_back(chanOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(isClosedInst);
                    }

                    std::string isClosedBool = b_.newTempRegister();
                    {
                        SIRInstruction cmp = SIRInstruction::Binary(
                            SIROpcode::NE, SIROperand::Register(isClosedBool, SadTypeKind::Boolean),
                            SIROperand::Register(isClosedReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(0));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(cmp);
                    }

                    // (AR) إذا مغلقة → خروج، وإلا → عودة للشرط (انتظار)
                    // (EN) If closed → exit, else → back to cond (wait)
                    {
                        SIRInstruction brCond = SIRInstruction::BranchCond(
                            SIROperand::Register(isClosedBool, SadTypeKind::Boolean),
                            SIROperand::Label(exitLabel),
                            SIROperand::Label(condLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(brCond);
                    }

                    // ---- كتلة الجسم: استقبال + تنفيذ ----
                    // ---- Body: receive + execute ----
                    b_.currentBlock_ = bodyBlock;

                    std::string recvReg = b_.newTempRegister();
                    {
                        SIRInstruction recvInst(SIROpcode::ASYNC_CHANNEL_RECV);
                        recvInst.result = SIROperand::Register(recvReg, chanElemType);
                        recvInst.operands.push_back(chanOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(recvInst);
                    }

                    // (AR) تخزين القيمة المُستقبَلة في متغير الحلقة
                    // (EN) Store received value into loop variable
                    {
                        SIRInstruction storeElem(SIROpcode::STORE);
                        storeElem.operands.push_back(SIROperand::Register(recvReg, chanElemType));
                        storeElem.operands.push_back(SIROperand::Register(loopVarAllocName, chanElemType));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeElem);
                    }

                    // (AR) تسجيل سياق الحلقة لدعم توقف/استمر
                    // (EN) Register loop context for break/continue
                    LoopContext chanLoopCtx;
                    chanLoopCtx.continueLabel = condLabel;
                    chanLoopCtx.breakLabel = exitLabel;
                    b_.enterLoop(chanLoopCtx);

                    // (AR) بناء جسم الحلقة
                    // (EN) Build loop body
                    if (forRange->body)
                    {
                        buildStatement(forRange->body.get());
                    }

                    b_.exitLoop();

                    // (AR) العودة إلى الشرط
                    // (EN) Jump back to condition
                    {
                        SIRInstruction br = SIRInstruction::Branch(SIROperand::Label(condLabel));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(br);
                    }

                    // ---- كتلة الخروج ----
                    b_.currentBlock_ = exitBlock;
                    b_.exitScope();

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildForRangeLoop: channel iteration completed" << std::endl;
#endif
                    return;
                }

                // ========================================================================
                // (AR) الخطوة 3: إنشاء الكتل الأساسية (مسار المصفوفة)
                // (EN) Step 3: Create basic blocks (array path)
                // ========================================================================
                std::string condLabel = b_.newLabel("foreach_cond");
                std::string bodyLabel = b_.newLabel("foreach_body");
                std::string incLabel = b_.newLabel("foreach_inc");
                std::string exitLabel = b_.newLabel("foreach_exit");

                auto condBlock = b_.createBasicBlock(condLabel);
                auto bodyBlock = b_.createBasicBlock(bodyLabel);
                auto incBlock = b_.createBasicBlock(incLabel);
                auto exitBlock = b_.createBasicBlock(exitLabel);

                // (AR) إضافة الكتل إلى الدالة الحالية
                // (EN) Add blocks to current function
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(condBlock);
                    b_.currentFunction_->addBasicBlock(bodyBlock);
                    b_.currentFunction_->addBasicBlock(incBlock);
                    b_.currentFunction_->addBasicBlock(exitBlock);
                }

                // ========================================================================
                // (AR) الخطوة 4: إنشاء متغير العداد عبر ALLOC+STORE (نمط غير-SSA آمن)
                // (EN) Step 4: Create index counter via ALLOC+STORE (safe non-SSA pattern)
                // (AR) نستخدم نمط التخصيص على المكدس مثل المتغيرات المحلية لتجنب
                //      مشكلة تعريف السجل الواحد في كتل متعددة (non-SSA counter).
                // (EN) We use stack allocation like local variables to avoid single-def
                //      register being "defined" in multiple blocks (non-SSA counter).
                // ========================================================================
                std::string idxSuffix = condLabel.substr(condLabel.find_last_of('_') + 1);
                std::string indexAllocName = std::string("%") + kSlotNamespaceSeparator + "foreach_idx" + kSlotNamespaceSeparator + idxSuffix;

                // ALLOC the counter slot
                {
                    SIRInstruction allocIdx(SIROpcode::ALLOC);
                    allocIdx.result = SIROperand::Register(indexAllocName, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(allocIdx);
                }

                // STORE 0 into the counter slot (initialize to 0)
                {
                    SIRInstruction storeZero(SIROpcode::STORE);
                    storeZero.operands.push_back(SIROperand::ConstantI64(0));
                    storeZero.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(storeZero);
                }

                // ========================================================================
                // (AR) الخطوة 5: إنشاء متغير الحلقة وتسجيله في النطاق
                // (EN) Step 5: Create loop variable and register in scope
                // المصدر: ForRangeStmt::variable (statements.h:230)
                // ========================================================================
                // (AR) [إصلاح] استخدام اسم فريد لمتغير الحلقة لتجنب تضارب ALLOCAs
                //      عند وجود عدة حلقات لكل بنفس اسم المتغير (مثل ع).
                //      بدون اللاحقة: namedValues["%ع"] يُكتب فوقه عدة مرات في codegen
                //      وعمليات LOAD/STORE داخل الحلقة تقرأ/تكتب في alloca الحلقة الخطأ.
                // (EN) [Fix] Use unique name for loop variable to avoid alloca collision
                //      when multiple foreach loops use the same variable name (e.g. ع).
                //      Without suffix: namedValues["%ع"] gets overwritten in codegen
                //      and LOAD/STORE inside the loop body reads/writes wrong alloca.
                // (AR) الفاصلُ `#` لا `_`: اللاحقةُ الأبجديّةُ الرقميّةُ تهجئةٌ مشروعةٌ لمعرّفِ
                //      مستخدمٍ، و«لكل ي في [1، 2، 3]» ثمّ «متغير ي_5 = 100» كان يجعلُ
                //      التصريحَ يختطفُ موقعَ عنصرِ الحلقةِ فيطبعُ المترجِمُ `100` والمفسّرُ `1`.
                // (EN) `#` not `_`: an alphanumeric suffix is a legal user identifier, and a
                //      body declaration spelled like it hijacked the element slot (compiler
                //      printed `100` where the interpreter printed `1`). `#` starts a comment
                //      in Sad, so it cannot appear in any identifier.
                std::string loopVarAllocName = "%" + forRange->variable + kSlotNamespaceSeparator + idxSuffix;

                // ALLOC the loop variable slot
                // (AR) نستخدم نوع العنصر الفعلي إن كان معروفاً (مثل STRING للنصوص)
                //      هذا يضمن إنشاء alloca ptr للنصوص بدلاً من alloca i64
                //      مما يمنع تحويل ptrtoint عند التخزين ويحافظ على المؤشر كـ ptr
                // (EN) Use actual element type when known (e.g., STRING for text arrays)
                //      This ensures alloca ptr for strings instead of alloca i64
                {
                    SadTypeKind loopVarType = (iterableResult.elementType != SadTypeKind::Void)
                                                  ? iterableResult.elementType
                                                  : SadTypeKind::Integer;
                    SIRInstruction allocLoop(SIROpcode::ALLOC);
                    allocLoop.result = SIROperand::Register(loopVarAllocName, loopVarType);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(allocLoop);
                }

                // (AR) تسجيل متغير الحلقة (sir_builder.h:144 - VariableInfo)
                //      نستخدم نوع العنصر المستنتج إن وجد لتصحيح الطباعة
                // (EN) Register loop variable using b_.addVariable (sir_builder.h:591)
                //      Use inferred element type when available for correct printing
                VariableInfo varInfo;
                varInfo.name = forRange->variable;
                varInfo.registerName = loopVarAllocName;
                varInfo.type = (iterableResult.elementType != SadTypeKind::Void) ? iterableResult.elementType : SadTypeKind::Integer;
                varInfo.isMutable = true;
                if (!iterableResult.elementClassName.empty())
                {
                    varInfo.className = iterableResult.elementClassName;
                }

                b_.addVariable(varInfo);

                if (!varInfo.className.empty())
                {
                    b_.classInstanceTypes_[forRange->variable] = varInfo.className;
                    b_.classInstanceTypes_[loopVarAllocName] = varInfo.className;
                }

                // (AR) متغيّر القيمة للخريطة («لكل مفتاح، قيمة في خريطة»): فتحة alloc + تسجيل
                //      في النطاق. الربط الفعليّ (ARRAY_GET من مصفوفة القيم) في جسم الحلقة أدناه.
                // (EN) Map value variable ("for key, value in map"): alloc slot + register in
                //      scope. The actual bind (ARRAY_GET from the values array) is in the body.
                if (!mapValuesReg.empty())
                {
                    // (AR) نوع القيمة يُشتقّ من تمثيلها المخزَّن الفعليّ في الخريطة (buildExprMap):
                    //      • صحيح/منطقيّ ⇒ يُخزَّن i64 مباشرة ⇒ النوع نفسه.
                    //      • نصّ ⇒ يُخزَّن مؤشّرًا ⇒ String.
                    //      • عشريّ ⇒ يُخزَّن **ببتّاتِه** بوسمِ العشريِّ (م-٠٠١) ⇒ Float.
                    //      • مختلط (Void)/غيرها ⇒ تراجع إلى Integer (قيد موثَّق).
                    // (EN) The value type is derived from its actual stored representation in the map
                    //      (buildExprMap): int/bool are stored as i64 ⇒ same type; string is stored as
                    //      a pointer ⇒ String; float is stored as **raw bits** under the float tag
                    //      (card م-٠٠١) ⇒ Float; mixed (Void)/other ⇒ fall back to Integer (documented).
                    //
                    // (AR) ⚠️ هذا الاشتقاقُ **مرآةٌ لتمثيلِ التخزين**، فأيُّ تغييرٍ في وسمِ
                    //      قيمةٍ يجبُ أن يُقابَلَ هنا وفي `expression_comprehensions.cpp`.
                    //      كان العشريُّ هنا String لأنّه كان يُنصَّصُ عندَ التخزين؛ فلمّا صار
                    //      يُخزَّنُ بتّاتٍ ولم يُحدَّثْ هذا الموضعُ طُبِعت بتّاتُ الـdouble
                    //      بـ`%s` ⇒ SIGSEGV. الوسمُ ليس عقدًا خلفيًّا وحدَه.
                    // (EN) ⚠️ This derivation **mirrors the storage representation**, so any change
                    //      to a value's tag must be matched here and in
                    //      `expression_comprehensions.cpp`. Float was String here because it used to
                    //      be stringified on store; when storage moved to raw bits and this site was
                    //      not updated, the double's bits were printed with `%s` ⇒ SIGSEGV. The tag
                    //      is not a backend-only contract.
                    valueVarType = (mapValueType == SadTypeKind::Integer ||
                                    mapValueType == SadTypeKind::Boolean ||
                                    mapValueType == SadTypeKind::Float ||
                                    mapValueType == SadTypeKind::Any)
                                       ? mapValueType
                                   : (mapValueType == SadTypeKind::String)
                                       ? SadTypeKind::String
                                       : SadTypeKind::Integer;

                    valueVarAllocName = "%" + forRange->valueVar + kSlotNamespaceSeparator + idxSuffix;
                    {
                        SIRInstruction allocVal(SIROpcode::ALLOC);
                        allocVal.result = SIROperand::Register(valueVarAllocName, valueVarType);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocVal);
                    }
                    // (AR) تهيئة ابتدائيّة للفتحة (0/null) اتّساقًا مع فتحة العدّاد — يمنع قراءة
                    //      قمامة إن قرئ المتغيّر قبل أوّل ربط (مثلًا خريطة فارغة). / (EN) Zero/null
                    //      init for the slot (mirrors the counter slot) — prevents reading garbage
                    //      if the var is read before the first bind (e.g. an empty map).
                    {
                        SIRInstruction storeInit(SIROpcode::STORE);
                        storeInit.operands.push_back(SIROperand::ConstantI64(0));
                        storeInit.operands.push_back(SIROperand::Register(valueVarAllocName, valueVarType));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeInit);
                    }
                    VariableInfo valVarInfo;
                    valVarInfo.name = forRange->valueVar;
                    valVarInfo.registerName = valueVarAllocName;
                    valVarInfo.type = valueVarType;
                    valVarInfo.isMutable = true;
                    b_.addVariable(valVarInfo);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForRangeLoop: registered loop var '"
                          << forRange->variable << "' alloc=" << loopVarAllocName << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 6: قفز غير شرطي إلى كتلة الشرط
                // (EN) Step 6: Unconditional jump to condition block
                // ========================================================================
                SIROperand condLabelOp = SIROperand::Label(condLabel);
                SIRInstruction brCondBlockInst = SIRInstruction::Branch(condLabelOp);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brCondBlockInst);
                }

                // ========================================================================
                // (AR) الخطوة 7: بناء الشرط (index < length)
                // (EN) Step 7: Build condition (index < length)
                // ========================================================================
                b_.currentBlock_ = condBlock;

                SIROperand iterOp = SIROperand::Register(iterableResult.registerName, iterableResult.type);

                // LOAD current counter value
                std::string loadedIdxCond = b_.newTempRegister();
                {
                    SIRInstruction loadIdx(SIROpcode::LOAD);
                    loadIdx.result = SIROperand::Register(loadedIdxCond, SadTypeKind::Integer);
                    loadIdx.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(loadIdx);
                }

                // ARRAY_LEN
                std::string lengthReg = b_.newTempRegister();
                SIROperand lengthOp = SIROperand::Register(lengthReg, SadTypeKind::Integer);
                {
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = lengthOp;
                    lenInst.operands.push_back(iterOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(lenInst);
                }

                // LT: loadedIdxCond < length
                std::string condReg = b_.newTempRegister();
                SIROperand condResultOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                {
                    SIRInstruction cmpInst = SIRInstruction::Binary(
                        SIROpcode::LT, condResultOp,
                        SIROperand::Register(loadedIdxCond, SadTypeKind::Integer),
                        lengthOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(cmpInst);
                }

                // BR_COND → body / exit
                SIROperand bodyLabelOp = SIROperand::Label(bodyLabel);
                SIROperand exitLabelOp = SIROperand::Label(exitLabel);
                {
                    SIRInstruction brCondInst = SIRInstruction::BranchCond(
                        condResultOp, bodyLabelOp, exitLabelOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(brCondInst);
                }

                // ========================================================================
                // (AR) الخطوة 8: بناء جسم الحلقة
                // (EN) Step 8: Build loop body
                // ========================================================================
                b_.currentBlock_ = bodyBlock;

                // LOAD current counter value for ARRAY_GET
                std::string loadedIdxBody = b_.newTempRegister();
                {
                    SIRInstruction loadIdxB(SIROpcode::LOAD);
                    loadIdxB.result = SIROperand::Register(loadedIdxBody, SadTypeKind::Integer);
                    loadIdxB.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(loadIdxB);
                }

                // ARRAY_GET: loopVar = iterable[loadedIdx]
                // (AR) نستخدم نوع العنصر المستنتج من المصفوفة إن وجد
                //      هذا يحل مشكلة تكرار النصوص التي كانت تُحمَّل كأرقام
                // (EN) Use inferred element type from array if available
                //      This fixes string iteration being loaded as numbers
                SadTypeKind elemType = SadTypeKind::Integer;
                if (iterableResult.elementType != SadTypeKind::Void)
                {
                    elemType = iterableResult.elementType;
                }
                std::string elemReg = b_.newTempRegister();
                {
                    SIRInstruction loadElem(SIROpcode::ARRAY_GET);
                    loadElem.result = SIROperand::Register(elemReg, elemType);
                    loadElem.operands.push_back(iterOp);
                    loadElem.operands.push_back(SIROperand::Register(loadedIdxBody, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(loadElem);
                }

                // STORE element into loop variable slot
                // (AR) نستخدم نوع العنصر الفعلي للتخزين لتجنب ptrtoint عند تخزين نصوص
                // (EN) Use actual element type for store to avoid ptrtoint when storing strings
                {
                    SIRInstruction storeElem(SIROpcode::STORE);
                    storeElem.operands.push_back(SIROperand::Register(elemReg, elemType));
                    storeElem.operands.push_back(SIROperand::Register(loopVarAllocName, elemType));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(storeElem);
                }

                // (AR) ربط متغيّر القيمة للخريطة: قيمة = مصفوفة_القيم[نفس الفهرس]، ثمّ خزّنها.
                // (EN) Bind the map value variable: value = valuesArray[same index], then store.
                if (!mapValuesReg.empty())
                {
                    // (AR) نستخدم النوع المحسوم valueVarType لتحميل/تخزين القيمة — فتُفسَّر النصوص
                    //      كمؤشّرات (لا ptrtoint) والأعداد كـ i64، مطابقةً لتخزين buildExprMap.
                    // (EN) Use the resolved valueVarType to load/store the value — so strings are
                    //      read as pointers (no ptrtoint) and integers as i64, matching buildExprMap.
                    std::string valElemReg = b_.newTempRegister();
                    {
                        SIRInstruction loadVal(SIROpcode::ARRAY_GET);
                        loadVal.result = SIROperand::Register(valElemReg, valueVarType);
                        loadVal.operands.push_back(SIROperand::Register(mapValuesReg, SadTypeKind::Array));
                        loadVal.operands.push_back(SIROperand::Register(loadedIdxBody, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(loadVal);
                    }
                    {
                        SIRInstruction storeVal(SIROpcode::STORE);
                        storeVal.operands.push_back(SIROperand::Register(valElemReg, valueVarType));
                        storeVal.operands.push_back(SIROperand::Register(valueVarAllocName, valueVarType));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeVal);
                    }
                }

                // ========================================================================
                // (AR) الخطوة 8.5: تسجيل سياق الحلقة لدعم break/continue
                //      continueLabel = كتلة الزيادة (foreach_inc) — continue يزيد العداد ثم يفحص الشرط
                //      breakLabel = كتلة الخروج (foreach_exit) — break يقفز للخروج
                // (EN) Step 8.5: Register loop context for break/continue support
                //      continueLabel = increment block (foreach_inc) — continue increments then checks condition
                //      breakLabel = exit block (foreach_exit) — break jumps to exit
                // ========================================================================
                LoopContext foreachLoopCtx;
                foreachLoopCtx.continueLabel = incLabel;
                foreachLoopCtx.breakLabel = exitLabel;
                b_.enterLoop(foreachLoopCtx);

                // (AR) بناء جسم الحلقة
                // (EN) Build loop body
                if (forRange->body)
                {
                    buildStatement(forRange->body.get());
                }

                // (AR) الخروج من سياق الحلقة (break/continue)
                // (EN) Exit loop context (break/continue)
                b_.exitLoop();

                // (AR) قفز إلى كتلة الزيادة
                // (EN) Jump to increment block
                SIROperand incLabelOp = SIROperand::Label(incLabel);
                {
                    SIRInstruction brIncInst = SIRInstruction::Branch(incLabelOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(brIncInst);
                }

                // ========================================================================
                // (AR) الخطوة 9: بناء الزيادة (index = index + 1)
                // (EN) Step 9: Build increment (index = index + 1)
                // ========================================================================
                b_.currentBlock_ = incBlock;

                // LOAD counter current value
                std::string loadedIdxInc = b_.newTempRegister();
                {
                    SIRInstruction loadIdxI(SIROpcode::LOAD);
                    loadIdxI.result = SIROperand::Register(loadedIdxInc, SadTypeKind::Integer);
                    loadIdxI.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(loadIdxI);
                }

                // ADD: newIdx = loadedIdx + 1
                std::string newIdxReg = b_.newTempRegister();
                {
                    SIRInstruction incInst = SIRInstruction::Binary(
                        SIROpcode::ADD_I64,
                        SIROperand::Register(newIdxReg, SadTypeKind::Integer),
                        SIROperand::Register(loadedIdxInc, SadTypeKind::Integer),
                        SIROperand::ConstantI64(1));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(incInst);
                }

                // STORE newIdx back into counter slot
                {
                    SIRInstruction storeIdx(SIROpcode::STORE);
                    storeIdx.operands.push_back(SIROperand::Register(newIdxReg, SadTypeKind::Integer));
                    storeIdx.operands.push_back(SIROperand::Register(indexAllocName, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(storeIdx);
                }

                // (AR) قفز للعودة إلى الشرط
                // (EN) Jump back to condition
                {
                    SIRInstruction brBackInst = SIRInstruction::Branch(condLabelOp);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(brBackInst);
                }

                // ========================================================================
                // (AR) الخطوة 10: الاستمرار بعد الحلقة
                // (EN) Step 10: Continue after loop
                // ========================================================================
                b_.currentBlock_ = exitBlock;

                // (AR) الخروج من نطاق الحلقة
                // (EN) Exit loop scope
                b_.exitScope();

#ifndef NDEBUG
                std::cout << "[DEBUG] buildForRangeLoop: completed" << std::endl;
#endif
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
