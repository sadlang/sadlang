// ============================================================================
// sir_builder_stmt_concurrency.cpp
// ============================================================================
#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include <iostream>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            bool StatementBuilder::buildStatement_Go(AST::Statement *stmt)
            {
                // ========================================================================
                if (auto goStmt = dynamic_cast<Sad::AST::GoStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found GoStmt" << std::endl;
#endif
                    if (goStmt->expression)
                    {
                        // (AR) مساعد مشترك لمساري الإطلاق: حل الاسم المستعار «كـ»
                        //      (نمط call_main) + كشف تظليل متغير الإغلاق — كان
                        //      مكررا حرفيا في المسارين (مراجعة Amelia م11)
                        // (EN) Shared helper for both spawn paths: resolve the
                        //      «كـ» import alias (call_main's pattern) + detect
                        //      closure-variable shadowing — was duplicated
                        //      verbatim (Amelia review #11)
                        auto resolveSpawnTarget =
                            [&](const std::string &sourceName, std::string &targetName,
                                VariableInfo *&shadowVar, bool &shadowedByClosure)
                        {
                            targetName = sourceName;
                            if (!b_.importAliases_.empty() &&
                                b_.functionTable_.find(targetName) ==
                                    b_.functionTable_.end())
                            {
                                auto aliasIt = b_.importAliases_.find(targetName);
                                if (aliasIt != b_.importAliases_.end() &&
                                    b_.functionTable_.find(aliasIt->second) !=
                                        b_.functionTable_.end())
                                {
                                    targetName = aliasIt->second;
                                }
                            }
                            shadowVar = b_.lookupVariable(sourceName);
                            shadowedByClosure =
                                (shadowVar != nullptr &&
                                 shadowVar->type == SadTypeKind::Function);
                        };

                        // (AR) ع-16: «أطلق دالة()» لنداء مسمى بلا معاملات يصدر
                        //      ASYNC_SPAWN بمعامل SIROperand::Function — فيطلق
                        //      الخلفي خيط OS حقيقيا عبر ثانك سليم الـABI (انظر
                        //      emitAsyncSpawn) بدل التنفيذ المتزامن الذي كان
                        //      يحجب البرنامج على النداءات الحاجبة (ابدأ_الاستماع).
                        //      القيد مقصود: نداء بمعاملات أو لامدا/تعبير مركب
                        //      يبقى متزامنا (أدناه) لأن تمرير الوسائط إلى خيط
                        //      يحتاج تعليب التقاط لم يبن بعد — قيد موثق في تقرير
                        //      sad-libs (ع-16) لا التفاف صامت.
                        //      «ارمي» داخل الخيط المطلق مأمونة في الوضع المستضاف:
                        //      حالة الاستثناء (مكدس المعالجات والحمولة) صارت
                        //      thread-local هناك (exception_ops.cpp) — والوضع
                        //      الحر يبقيها عامة (لا TLS في ring-0) حيث لا خيوط
                        //      أصلا.
                        // (EN) ع-16: «go f()» for a zero-argument named call emits
                        //      ASYNC_SPAWN with a Function operand — the backend
                        //      spawns a real OS thread via an ABI-safe thunk (see
                        //      emitAsyncSpawn) instead of the synchronous inline
                        //      execution that deadlocked on blocking callees.
                        //      Calls with arguments, lambdas, and compound
                        //      expressions intentionally stay synchronous below
                        //      (argument capture boxing for threads is not built
                        //      yet) — a documented limit, not a silent detour.
                        // (AR) بوابة الوضع الحر: مسارات الخيوط الحقيقية كلها تعتمد
                        //      زمن تشغيل مستضافا (sad_rt_thread_spawn/malloc/free)
                        //      غير موجود في ring-0 — الوضع الحر يبقى على التنفيذ
                        //      المتزامن القديم بكامله.
                        // (EN) Freestanding gate: every real-thread path depends on
                        //      hosted runtime symbols (sad_rt_thread_spawn/malloc/
                        //      free) absent in ring-0 — freestanding keeps the old
                        //      fully synchronous lowering.
                        if (auto *callExpr =
                                dynamic_cast<Sad::AST::CallExpr *>(goStmt->expression.get());
                            callExpr && !b_.freestandingMode_)
                        {
                            if (callExpr->arguments.empty() && !callExpr->isMacroCall &&
                                b_.currentBlock_)
                            {
                                if (auto *calleeName = dynamic_cast<Sad::AST::VariableExpr *>(
                                        callExpr->callee.get()))
                                {
                                    std::string targetName;
                                    VariableInfo *shadowVar = nullptr;
                                    bool shadowedByClosure = false;
                                    resolveSpawnTarget(calleeName->name, targetName,
                                                       shadowVar, shadowedByClosure);

                                    // (AR) متغير إغلاق بلا معاملات: يُطلق خيطا عبر
                                    //      مدخل الإغلاق في زمن التشغيل. تحميل قيمة
                                    //      الإغلاق أولا بتعليمة LOAD من registerName
                                    //      (نمط call_main حرفا) — تمرير اسم المتغير
                                    //      المصدري خاما كان يخفق حله في الخلفية
                                    //      (المفاتيح بسابقة %) أو يمرر عنوان الخانة.
                                    // (EN) Zero-arg closure variable: spawn through
                                    //      the runtime closure entry. LOAD the closure
                                    //      value from the variable's registerName
                                    //      first (call_main's exact pattern) — passing
                                    //      the raw source name failed backend
                                    //      resolution (keys carry the % prefix) or
                                    //      leaked the slot address instead of the
                                    //      value.
                                    if (shadowedByClosure && b_.currentBlock_)
                                    {
                                        std::string loadReg = b_.newTempRegister();
                                        SIRInstruction loadInst;
                                        loadInst.opcode = SIROpcode::LOAD;
                                        loadInst.result = SIROperand::Register(
                                            loadReg, SadTypeKind::Function);
                                        loadInst.operands.push_back(SIROperand::Register(
                                            shadowVar->registerName, SadTypeKind::Function));
                                        b_.currentBlock_->addInstruction(loadInst);

                                        std::string resultReg = b_.newTempRegister();
                                        SIRInstruction spawnInst(SIROpcode::ASYNC_SPAWN);
                                        spawnInst.result = SIROperand::Register(
                                            resultReg, SadTypeKind::Integer);
                                        spawnInst.operands.push_back(SIROperand::Register(
                                            loadReg, SadTypeKind::Function));
                                        b_.currentBlock_->instructions.push_back(spawnInst);
                                        return true;
                                    }

                                    auto ftIt = b_.functionTable_.find(targetName);
                                    // (AR) المولدات مستثناة: نداؤها يمر بآلية
                                    //      GENERATOR_CONSUME الخاصة لا بالرمز الخام
                                    // (EN) Generators excluded: their calls go through
                                    //      the GENERATOR_CONSUME machinery, not the
                                    //      raw symbol
                                    if (!shadowedByClosure &&
                                        ftIt != b_.functionTable_.end() &&
                                        ftIt->second.parameters.empty() &&
                                        !ftIt->second.isGenerator)
                                    {
                                        std::string resultReg = b_.newTempRegister();
                                        SIRInstruction spawnInst(SIROpcode::ASYNC_SPAWN);
                                        spawnInst.result = SIROperand::Register(
                                            resultReg, SadTypeKind::Integer);
                                        spawnInst.operands.push_back(
                                            SIROperand::Function(ftIt->second.name));
                                        b_.currentBlock_->instructions.push_back(spawnInst);
                                        return true;
                                    }
                                }
                            }

                            // (AR) ع-16 (تعليب الوسائط): نداء مسمى **بمعاملات** —
                            //      يؤهل للخيط الحقيقي إذا طابق العدد عدد معاملات
                            //      الدالة وكانت أنواعها كلها بسيطة (رقم/عشري/منطقي/
                            //      نص/مؤشر): تقيم الوسائط هنا في خيط المطلق ثم
                            //      تعلب في حزمة كومة يفكها ثانك الخلفية داخل
                            //      الخيط. «أي» (%SadDyn) والافتراضية الناقصة تسقط
                            //      للمسار المتزامن (قيد موثق).
                            // (EN) ع-16 (argument boxing): a named call WITH
                            //      arguments qualifies for a real thread when the
                            //      count matches and every parameter type is
                            //      simple (int/float/bool/string/pointer): the
                            //      arguments evaluate HERE on the spawner's thread
                            //      and are boxed into a heap pack the backend
                            //      thunk unpacks inside the thread. Any-typed
                            //      params and defaulted-arity calls fall back to
                            //      the synchronous path (documented limit).
                            if (!callExpr->arguments.empty() && !callExpr->isMacroCall &&
                                b_.currentBlock_)
                            {
                                if (auto *calleeName = dynamic_cast<Sad::AST::VariableExpr *>(
                                        callExpr->callee.get()))
                                {
                                    std::string targetName;
                                    VariableInfo *shadowVar = nullptr;
                                    bool shadowedByClosure = false;
                                    resolveSpawnTarget(calleeName->name, targetName,
                                                       shadowVar, shadowedByClosure);

                                    auto ftIt = b_.functionTable_.find(targetName);
                                    bool eligible =
                                        !shadowedByClosure &&
                                        ftIt != b_.functionTable_.end() &&
                                        !ftIt->second.isGenerator &&
                                        ftIt->second.parameters.size() ==
                                            callExpr->arguments.size();
                                    if (eligible)
                                    {
                                        // (AR) شرط الأنواع المصرحة: معامل بنوع
                                        //      افتراضي (غير مصرح) يسقط للمسار
                                        //      المتزامن — مسار الإطلاق يتجاوز
                                        //      عقافات نشر الأنواع في النداء
                                        //      العادي (channelTypeMap وتوسيع
                                        //      النداء الأمامي)، فتعليب معامل
                                        //      افتراضي النوع يفقد نصيته (قيس:
                                        //      099 قناة نصية عبر دالة مطلقة
                                        //      طبعت مؤشرات أرقاما). فارغة
                                        //      paramDefaulted = لا معلومة =
                                        //      تحفظ: لا خيط.
                                        // (EN) Explicit-types requirement: a
                                        //      defaulted (undeclared) param type
                                        //      falls back to the synchronous
                                        //      path — the spawn path bypasses
                                        //      the regular call's type
                                        //      propagation hooks (channelTypeMap
                                        //      and forward-call widening), so
                                        //      boxing a defaulted param loses
                                        //      its stringness (measured: test
                                        //      099 printed pointers as
                                        //      numbers). Empty paramDefaulted =
                                        //      no info = conservative: no
                                        //      thread.
                                        const auto &defaulted = ftIt->second.paramDefaulted;
                                        if (defaulted.size() != ftIt->second.parameters.size())
                                        {
                                            eligible = false;
                                        }
                                        else
                                        {
                                            for (size_t pi = 0;
                                                 pi < ftIt->second.parameters.size(); ++pi)
                                            {
                                                const auto &param = ftIt->second.parameters[pi];
                                                if (defaulted[pi] ||
                                                    (param.type != SadTypeKind::Integer &&
                                                     param.type != SadTypeKind::Float &&
                                                     param.type != SadTypeKind::Boolean &&
                                                     param.type != SadTypeKind::String &&
                                                     param.type != SadTypeKind::Pointer))
                                                {
                                                    eligible = false;
                                                    break;
                                                }
                                            }
                                        }
                                    }

                                    if (eligible)
                                    {
                                        SIRInstruction spawnInst(SIROpcode::ASYNC_SPAWN);
                                        spawnInst.result = SIROperand::Register(
                                            b_.newTempRegister(), SadTypeKind::Integer);
                                        spawnInst.operands.push_back(
                                            SIROperand::Function(ftIt->second.name));
                                        for (auto &argExpr : callExpr->arguments)
                                        {
                                            BuildResult argResult =
                                                b_.buildExpression(argExpr.get());
                                            if (argResult.isConstant &&
                                                argResult.type == SadTypeKind::String)
                                            {
                                                spawnInst.operands.push_back(
                                                    SIROperand::ConstantString(
                                                        argResult.constantValue));
                                            }
                                            else if (argResult.isConstant &&
                                                     argResult.type == SadTypeKind::Integer)
                                            {
                                                // (AR) حارس التفكيك: قيمة غير قابلة
                                                //      للتفكيك/طافحة تسقط إلى السجل
                                                //      بدل إسقاط المترجم باستثناء
                                                // (EN) Parse guard: an unparsable or
                                                //      overflowing literal falls back
                                                //      to the register instead of
                                                //      crashing the compiler
                                                try
                                                {
                                                    spawnInst.operands.push_back(
                                                        SIROperand::ConstantI64(std::stoll(
                                                            argResult.constantValue)));
                                                }
                                                catch (...)
                                                {
                                                    spawnInst.operands.push_back(
                                                        SIROperand::Register(
                                                            argResult.registerName,
                                                            argResult.type));
                                                }
                                            }
                                            else if (argResult.isConstant &&
                                                     argResult.type == SadTypeKind::Float)
                                            {
                                                try
                                                {
                                                    spawnInst.operands.push_back(
                                                        SIROperand::ConstantF64(std::stod(
                                                            argResult.constantValue)));
                                                }
                                                catch (...)
                                                {
                                                    spawnInst.operands.push_back(
                                                        SIROperand::Register(
                                                            argResult.registerName,
                                                            argResult.type));
                                                }
                                            }
                                            else if (argResult.isConstant &&
                                                     argResult.type == SadTypeKind::Boolean)
                                            {
                                                spawnInst.operands.push_back(
                                                    SIROperand::ConstantBool(
                                                        argResult.constantValue == "true"));
                                            }
                                            else
                                            {
                                                spawnInst.operands.push_back(
                                                    SIROperand::Register(
                                                        argResult.registerName,
                                                        argResult.type));
                                            }
                                        }
                                        b_.currentBlock_->instructions.push_back(spawnInst);
                                        return true;
                                    }
                                }
                            }

                            // (AR) ع-16: لامدا حرفية بلا معاملات — تبنى بنية
                            //      الإغلاق هنا ثم تطلق عبر مدخل الإغلاق في زمن
                            //      التشغيل. لامدا بمعاملات تبقى متزامنة.
                            // (EN) ع-16: a zero-parameter lambda literal — the
                            //      closure struct builds here and spawns through
                            //      the runtime closure entry. Parameterized
                            //      lambdas stay synchronous.
                        }
                        else if (auto *lambdaExpr = dynamic_cast<Sad::AST::LambdaExpr *>(
                                     goStmt->expression.get()))
                        {
                            if (lambdaExpr->parameters.empty() && b_.currentBlock_)
                            {
                                BuildResult closureResult =
                                    b_.buildExpression(goStmt->expression.get());
                                if (!closureResult.registerName.empty())
                                {
                                    SIRInstruction spawnInst(SIROpcode::ASYNC_SPAWN);
                                    spawnInst.result = SIROperand::Register(
                                        b_.newTempRegister(), SadTypeKind::Integer);
                                    spawnInst.operands.push_back(SIROperand::Register(
                                        closureResult.registerName, SadTypeKind::Function));
                                    b_.currentBlock_->instructions.push_back(spawnInst);
                                    return true;
                                }
                            }
                        }

                        // (AR) المسار المتبقي: تنفيذ متزامن (التعليل التاريخي أدناه)
                        //      إصلاح جذري (0xC0000005): لا نُصدر ASYNC_SPAWN على *سجل نتيجة*
                        //      الاستدعاء — فنتيجة نداء void تُحلّ إلى مؤشّر null، فيولّد الخلفيّ
                        //      sad_rt_thread_spawn(null, null) الذي يُطلق خيط OS حقيقيًّا بدالة
                        //      بداية null/قمامة ⇒ انهيار وصول غير حتميّ.
                        // (EN) Remaining path: synchronous execution (historical rationale):
                        //      root fix (0xC0000005): never emit ASYNC_SPAWN on the call's
                        //      *result* register — a void call's result resolves to a null
                        //      pointer → sad_rt_thread_spawn(null, null) → thread with a
                        //      null/garbage start routine → non-deterministic AV.
                        b_.buildExpression(goStmt->expression.get());
                    }
                    else if (goStmt->blockBody)
                    {
                        // (AR) إصلاح جذري لكتل أطلق المتداخلة:
                        //      التنفيذ الخلفي الحالي للـ ASYNC_SPAWN يعمل بشكل متزامن (direct call)
                        //      ولا يدعم captures كاملة للمتغيرات المحلية داخل go-blocks المتداخلة.
                        //      لذلك نُخفض go-block مباشرة داخل نفس السياق للحفاظ على صحة النطاقات
                        //      ومنع توليد سجلات غير معرّفة مثل %مج_داخلي في الاختبارات المركبة.
                        // (EN) Root fix for nested go blocks:
                        //      current ASYNC_SPAWN backend behaves synchronously (direct call)
                        //      and does not fully support local captures in nested go-blocks.
                        //      So we lower go-block inline in the same scope to preserve
                        //      variable visibility and avoid undefined registers (e.g. %مج_داخلي).
                        buildStatement(goStmt->blockBody.get());
                    }
                    return true;
                }

                return false;
            }

            bool StatementBuilder::buildStatement_Select(AST::Statement *stmt)
            {

                // ========================================================================
                // (AR) SelectStmt - جملة اختر — اختيار من قنوات متعددة
                // (EN) Select statement — multiplex on multiple channels
                //
                // (AR) التنفيذ: نُحوّل إلى سلسلة فحوصات:
                //      لكل حالة: ASYNC_CHANNEL_HAS_DATA → إذا نعم: ASYNC_CHANNEL_RECV + جسم
                //      إذا لم تنجح أي حالة: جسم الافتراضي (إن وُجد)
                // (EN) Implementation: lower to check chain:
                //      for each case: ASYNC_CHANNEL_HAS_DATA → if yes: ASYNC_CHANNEL_RECV + body
                //      if no case ready: default body (if exists)
                // ========================================================================
                if (auto selectStmt = dynamic_cast<Sad::AST::SelectStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found SelectStmt with " << selectStmt->cases.size() << " cases" << std::endl;
#endif
                    // (AR) إنشاء تسمية الخروج
                    std::string exitLabel = b_.newLabel("select_exit");
                    auto exitBlock = b_.createBasicBlock(exitLabel);

                    for (size_t i = 0; i < selectStmt->cases.size(); ++i)
                    {
                        auto &selCase = selectStmt->cases[i];

                        // (AR) إنشاء تسميات لجسم الحالة والحالة التالية
                        std::string bodyLabel = b_.newLabel("select_case_" + std::to_string(i));
                        std::string nextLabel = b_.newLabel("select_next_" + std::to_string(i));
                        auto bodyBlock = b_.createBasicBlock(bodyLabel);
                        auto nextBlock = b_.createBasicBlock(nextLabel);

                        // (AR) بناء تعبير القناة (عادةً ق.حاول_استقبل() أو ق.استقبل())
                        // (EN) Build channel expression => check if data available
                        if (selCase->channelExpr)
                        {
                            std::string checkReg = b_.newTempRegister();
                            bool loweredToTryRecvCompare = false;

                            // (AR) إصلاح دلالة select:
                            //      عندما تكون الحالة من الشكل ق.حاول_استقبل() يجب أن نستهلك
                            //      العنصر عند النجاح. لذلك نُبقي الاستدعاء try_recv نفسه ونقارن
                            //      الناتج مع null-sentinel (لاشيء) بدلاً من المقارنة مع 0.
                            // (EN) Fix select semantics:
                            //      for ch.try_recv() cases we must consume on success.
                            //      Keep try_recv call and compare result with null-sentinel
                            //      instead of comparing with 0.
                            if (auto m = dynamic_cast<Sad::AST::MethodCallExpr *>(selCase->channelExpr.get()))
                            {
                                const std::string &mn = m->methodName;
                                const bool isTryRecv =
                                    mn == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84" ||
                                    mn == "try_recv";
                                if (isTryRecv)
                                {
                                    auto recvResult = b_.buildExpression(selCase->channelExpr.get());
                                    if (!recvResult.registerName.empty())
                                    {
                                        SIRInstruction cmpInst = SIRInstruction::Binary(
                                            SIROpcode::NE,
                                            SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                            SIROperand::Register(recvResult.registerName, recvResult.type),
                                            SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                                        cmpInst.comment = "select: try_recv != null case " + std::to_string(i);
                                        if (b_.currentBlock_)
                                            b_.currentBlock_->addInstruction(cmpInst);
                                        loweredToTryRecvCompare = true;
                                    }
                                }
                            }

                            if (!loweredToTryRecvCompare)
                            {
                                // (AR) المسار العام: تقييم التعبير ثم مقارنة != 0
                                auto chanResult = b_.buildExpression(selCase->channelExpr.get());
                                SIRInstruction cmpInst = SIRInstruction::Binary(
                                    SIROpcode::NE,
                                    SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                    SIROperand::Register(chanResult.registerName, chanResult.type),
                                    SIROperand::ConstantI64(0));
                                cmpInst.comment = "select: check channel case " + std::to_string(i);
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(cmpInst);
                            }

                            // (AR) تفرع: إذا جاهزة → الجسم، وإلا → الحالة التالية
                            SIRInstruction brInst = SIRInstruction::BranchCond(
                                SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                SIROperand::Label(bodyLabel),
                                SIROperand::Label(nextLabel));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(brInst);
                        }
                        else
                        {
                            // (AR) لا يوجد تعبير — تخطي إلى التالي
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Branch(SIROperand::Label(nextLabel)));
                        }

                        // (AR) كتلة جسم الحالة
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(bodyBlock);
                        b_.currentBlock_ = bodyBlock;

                        // (AR) بناء جسم الحالة
                        for (auto &bodyStmt : selCase->body)
                        {
                            if (bodyStmt)
                                buildStatement(bodyStmt.get());
                        }

                        // (AR) قفز إلى الخروج بعد الجسم
                        if (b_.currentBlock_ && !b_.currentBlock_->getTerminator())
                        {
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }

                        // (AR) كتلة الحالة التالية
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(nextBlock);
                        b_.currentBlock_ = nextBlock;
                    }

                    // (AR) الحالة الافتراضية
                    if (!selectStmt->defaultBody.empty())
                    {
                        for (auto &defStmt : selectStmt->defaultBody)
                        {
                            if (defStmt)
                                buildStatement(defStmt.get());
                        }
                    }

                    // (AR) قفز إلى الخروج
                    if (b_.currentBlock_ && !b_.currentBlock_->getTerminator())
                    {
                        b_.currentBlock_->addInstruction(
                            SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                    }

                    // (AR) كتلة الخروج
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    b_.currentBlock_ = exitBlock;

                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
