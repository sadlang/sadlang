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

#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
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
                if (funcDecl->returnType == Types::SadTypeKind::Optional && funcDecl->sadReturnType)
                {
                    if (auto *opt = dynamic_cast<const Sad::Types::SadOptionalType *>(funcDecl->sadReturnType.get()))
                        if (opt->getInnerType())
                            returnType = astTypeToSIRType(opt->getInnerType()->getKind());
                }

                // (AR) إنشاء دالة SIR جديدة (sir_module.h:235 - SIRFunction constructor)
                // (EN) Create new SIR function
                auto sirFunction = std::make_shared<SIRFunction>(funcDecl->name, returnType);

                // (AR) نقل سمات الدالة [[سمة]] من AST إلى SIR لتُترجم لاحقاً
                //      إلى LLVM function attributes في codegen.
                // (EN) Propagate function attributes [[attr]] from AST to SIR
                //      so they can be lowered to LLVM function attributes in codegen.
                sirFunction->attributes = funcDecl->attributes;
#ifdef SIR_BUILDER_DEBUG
                std::cerr << "[SIR-DBG] buildFunction: '" << funcDecl->name
                          << "' inferred retType=" << static_cast<int>(returnType) << std::endl;
#endif

                // (AR) إذا كانت دالة غير متزامنة، اجعلها كوروتين
                // (EN) If async function, mark as coroutine
                if (funcDecl->is_async)
                {
                    sirFunction->isCoroutine = true;
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
                    if (param.type == Types::SadTypeKind::Optional && param.sadType)
                    {
                        if (auto *opt = dynamic_cast<const Sad::Types::SadOptionalType *>(param.sadType.get()))
                            if (opt->getInnerType())
                                paramType = astTypeToSIRType(opt->getInnerType()->getKind());
                    }

                    // (AR) إذا كان النوع I64 (من UNKNOWN) وfunctionTable_ يحتوي نوعاً مستنتجاً أفضل
                    // (EN) If type is I64 (from UNKNOWN) and functionTable_ has a better inferred type
                    // (AR) إصلاح X04: لا تُبدِّل إلى Boolean (i1) لأن null/لاشيء يُمرَّر كـ i64 sentinel
                    //      تبديل UNKNOWN→Boolean يُسبّب trunc i64→i1 عند الاستدعاء مما يُضيّع قيمة null
                    // (EN) FIX X04: Don't override to Boolean (i1) — null is passed as i64 sentinel
                    //      Overriding UNKNOWN→Boolean causes trunc i64→i1 at call site, corrupting null
                    if (paramType == SadTypeKind::Integer &&
                        param.type == Types::SadTypeKind::Unknown &&
                        ftIt != functionTable_.end() &&
                        i < ftIt->second.parameters.size() &&
                        ftIt->second.parameters[i].type != SadTypeKind::Integer &&
                        ftIt->second.parameters[i].type != SadTypeKind::Boolean)
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
                    sirFunction->addParameter(sirParam);
                }

                // (AR) إذا لم يكن هناك جسم للدالة (builtin or declaration only), لا نبني الجسم
                // (EN) If no function body (builtin or declaration only), don't build body
                if (!funcDecl->body)
                {
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
                    if (param.type == Types::SadTypeKind::Optional && param.sadType)
                    {
                        if (auto *opt = dynamic_cast<const Sad::Types::SadOptionalType *>(param.sadType.get()))
                            if (opt->getInnerType())
                                paramType = astTypeToSIRType(opt->getInnerType()->getKind());
                    }

                    // (AR) نفس المنطق: استخدام النوع المستنتج عندما يكون UNKNOWN
                    // (EN) Same logic: use inferred type when UNKNOWN
                    // (AR) إصلاح X04: نفس الشرط — لا تُبدِّل إلى Boolean
                    // (EN) FIX X04: Same condition — don't override to Boolean
                    if (paramType == SadTypeKind::Integer &&
                        param.type == Types::SadTypeKind::Unknown &&
                        ftIt != functionTable_.end() &&
                        i < ftIt->second.parameters.size() &&
                        ftIt->second.parameters[i].type != SadTypeKind::Integer &&
                        ftIt->second.parameters[i].type != SadTypeKind::Boolean)
                    {
                        paramType = ftIt->second.parameters[i].type;
                    }

                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = paramType;
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

                currentDeferStackReg_ = "%__defer_stack_" + std::to_string(nextLabel_++);
                currentDeferExecutedFlagReg_ = "%__defer_done_" + std::to_string(nextLabel_++);
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

                std::string functionCleanupJmpbufReg = "%__defer_jmpbuf_" + std::to_string(nextLabel_++);
                std::string functionCleanupSetjmpReg = newTempRegister();
                std::string functionCleanupCmpReg = newTempRegister();
                std::string functionBodyLabel = newLabel("function_body");
                std::string functionCleanupLabel = newLabel("function_defer_cleanup");
                auto functionBodyBlock = createBasicBlock(functionBodyLabel);
                auto functionCleanupBlock = createBasicBlock(functionCleanupLabel);
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
                    funcInfo.returnType = returnType;
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
            void SIRBuilder::buildGlobalVariable(AST::VariableDeclNode *varDecl)
            {
                if (!varDecl)
                {
                    return;
                }

                // (AR) تحويل النوع (astTypeToSIRType: sir_builder.h:713)
                // (EN) Convert type
                SadTypeKind varType = astTypeToSIRType(varDecl->type);
                // (AR) إذا كان النوع UNKNOWN (افتراضي Integer)، نستنتجه من المُهيئ
                //      هذا ضروري لمتغيرات الفضاء مثل: متغير PI = 3.14159
                //      حيث لا يُحدد النوع صراحة ويجب استنتاجه من القيمة الحرفية
                // (EN) If type is UNKNOWN (defaults to Integer), infer from initializer
                //      Needed for namespace vars like: var PI = 3.14159
                //      where type is not explicit and must be inferred from literal value
                if (varDecl->type == Types::SadTypeKind::Unknown && varDecl->initializer)
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
                // (AR) إنشاء متغير عام (SIRGlobalVariable constructor: sir_module.h:96)
                // (EN) Create global variable
                auto sirGlobal = std::make_shared<SIRGlobalVariable>(varDecl->name, varType);

                // (AR) معالجة القيمة الأولية إذا كانت ثابتاً حرفياً
                // (EN) Handle initializer if it's a literal constant
                if (varDecl->initializer)
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
                    }
                }

                // (AR) إضافة المتغير العام للوحدة (sir_module.h:591 - addGlobalVariable)
                // (EN) Add global variable to module
                module_->addGlobalVariable(sirGlobal);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
