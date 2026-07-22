// ============================================================================
// sir_builder_statements_main.cpp - buildStatement implementation
// ============================================================================
#include <string>
#include <set>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
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
            void StatementBuilder::buildStatement(AST::Statement *stmt)
            {
                if (!stmt)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildStatement: processing statement" << std::endl;
#endif

                // (AR) BlockStmt - كتلة من الجمل (statements.h:423)
                // (EN) Block statement
                if (auto blockStmt = dynamic_cast<Sad::AST::BlockStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found BlockStmt with " << blockStmt->statements.size() << " statements" << std::endl;
#endif
                    // (AR) معالجة كل جملة في الكتلة
                    // (EN) Process each statement in the block
                    for (const auto &innerStmt : blockStmt->statements)
                    {
                        buildStatement(innerStmt.get());
                    }
                    return;
                }

                // (AR) FunctionDecl — تعريف دالة متداخلة (declarations.h:19)
                //      عندما يُعرَّف تعريف دالة داخل جسم دالة أخرى:
                //      1) نحلل المتغيرات الحرة (من النطاق الأب) المُستخدمة في الدالة
                //      2) إذا وُجدت متغيرات حرة: نبنيها كإغلاق (closure) مع __env
                //      3) إذا لم تُوجد: نبنيها كدالة مستقلة (السلوك القديم)
                //      هذا يحلّ مشكلة "Undefined register" للدوال المسمّاة الداخلية
                //      التي تلتقط متغيرات من الدالة الأم (مثل: إنشاء_جامع/أضف)
                // (EN) Nested function declaration (declarations.h:19)
                //      1) Analyze free variables (from parent scope) used in function
                //      2) If free vars found: build as closure with __env
                //      3) If none: build as standalone module-level function
                //      This fixes "Undefined register" for named inner functions
                //      that capture variables from the parent (e.g. factory/accumulator pattern)
                if (auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(stmt))
                {
                    // ================================================================
                    // (AR) الخطوة 1: جمع أسماء المعاملات كأسماء مربوطة
                    // (EN) Step 1: Collect parameter names as bound names
                    // ================================================================
                    std::set<std::string> paramNames;
                    for (const auto &param : funcDecl->parameters)
                    {
                        paramNames.insert(param.name);
                    }
                    // (AR) اسم الدالة نفسها مربوط أيضاً (لتجنب اعتبارها حرة عند الاستدعاء الذاتي)
                    // (EN) Function's own name is also bound (avoid treating recursive calls as free)
                    paramNames.insert(funcDecl->name);

                    // ================================================================
                    // (AR) الخطوة 2: جمع المتغيرات الحرة من جسم الدالة
                    // (EN) Step 2: Collect free variables from function body
                    // ================================================================
                    std::set<std::string> freeVars;
                    if (funcDecl->body)
                    {
                        std::set<std::string> boundCopy = paramNames;
                        b_.collectFreeVarsStmt(funcDecl->body.get(), boundCopy, freeVars);
                    }

                    // ================================================================
                    // (AR) الخطوة 3: تصفية — فقط المتغيرات الموجودة في النطاق الحالي (الأب)
                    // (EN) Step 3: Filter — only vars that exist in current (parent) scope
                    // ================================================================
                    std::vector<SIRBuilderContext::CaptureInfo> captures;
                    for (const auto &fv : freeVars)
                    {
                        auto *varPtr = b_.lookupVariable(fv);
                        if (varPtr)
                        {
                            SIRBuilderContext::CaptureInfo ci;
                            ci.varName = fv;
                            ci.registerName = varPtr->registerName;
                            ci.type = varPtr->type;
                            captures.push_back(ci);
                        }
                    }

                    if (captures.empty())
                    {
                        // ================================================================
                        // (AR) لا التقاطات — السلوك القديم: بناء دالة مستقلة
                        // (EN) No captures — old behavior: build standalone function
                        // ================================================================
                        auto savedCtxNested = b_.saveContext();
                        b_.buildFunction(funcDecl);
                        b_.restoreContext(std::move(savedCtxNested));
                    }
                    else
                    {
                        // ================================================================
                        // (AR) وُجدت التقاطات — بناء الدالة كإغلاق (مثل لامدا)
                        //      نُنشئ دالة بمعامل __env مخفي + نحمّل المتغيرات الملتقطة
                        //      ثم نُنشئ CLOSURE_CREATE ونسجّل اسم الدالة كمتغير محلي
                        // (EN) Captures found — build function as closure (like lambda)
                        //      Create function with hidden __env param + load captured vars
                        //      Then create CLOSURE_CREATE and register function name as local var
                        // ================================================================
                        std::string innerFuncName = "__inner_" + funcDecl->name + "_" + std::to_string(b_.nextTempRegister_++);

                        // (AR) تخزين التقاطات الإغلاق للـ codegen
                        // (EN) Store closure captures for codegen
                        b_.closureCaptures_[innerFuncName] = captures;

                        // (AR) بناء معاملات الدالة: المعاملات الصريحة + __env
                        // (EN) Build function params: explicit params + __env
                        auto ftIt = b_.functionTable_.find(funcDecl->name);
                        std::vector<SIRParameter> sirParams;
                        for (size_t i = 0; i < funcDecl->parameters.size(); i++)
                        {
                            const auto &param = funcDecl->parameters[i];
                            SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                            // (AR) استخدام النوع المُستنتج من b_.functionTable_ إن وُجد
                            // (EN) Use inferred type from b_.functionTable_ if available
                            if (paramType == SadTypeKind::Integer &&
                                param.type == Types::SadTypeKind::Unknown &&
                                ftIt != b_.functionTable_.end() &&
                                i < ftIt->second.parameters.size() &&
                                ftIt->second.parameters[i].type != SadTypeKind::Integer)
                            {
                                paramType = ftIt->second.parameters[i].type;
                            }
                            sirParams.push_back(SIRParameter(param.name, paramType));
                        }
                        sirParams.push_back(SIRParameter("__env", SadTypeKind::Integer));

                        // (AR) استنتاج نوع الإرجاع
                        // (EN) Infer return type
                        SadTypeKind returnType;
                        if (funcDecl->returnType == Types::SadTypeKind::Unknown ||
                            funcDecl->returnType == Types::SadTypeKind::Void)
                        {
                            returnType = b_.inferReturnTypeFromBody(funcDecl->body.get(), funcDecl);
                        }
                        else
                        {
                            returnType = b_.astTypeToSIRType(funcDecl->returnType);
                        }

                        // (AR) إنشاء دالة SIR
                        // (EN) Create SIR function
                        auto innerFunc = std::make_shared<SIRFunction>(innerFuncName, returnType);
                        for (const auto &sp : sirParams)
                            innerFunc->addParameter(sp);

                        // (AR) حفظ السياق وتعيين سياق الدالة الجديدة
                        // (EN) Save context and set new function context
                        auto savedCtx = b_.saveContext();
                        b_.currentFunction_ = innerFunc;
                        auto entryBlock = b_.createBasicBlock("lambda_entry");
                        innerFunc->addBasicBlock(entryBlock);
                        b_.currentBlock_ = entryBlock;

                        b_.enterScope();

                        // (AR) تسجيل المعاملات الصريحة كمتغيرات محلية
                        // (EN) Register explicit parameters as local variables
                        for (size_t i = 0; i < funcDecl->parameters.size(); ++i)
                        {
                            std::string paramReg = "%" + funcDecl->parameters[i].name;
                            VariableInfo paramVar;
                            paramVar.name = funcDecl->parameters[i].name;
                            paramVar.type = sirParams[i].type;
                            paramVar.registerName = paramReg;
                            paramVar.isMutable = true;
                            paramVar.isParameter = true;
                            paramVar.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(paramVar);
                        }

                        // (AR) تحميل المتغيرات الملتقطة من بيئة الإغلاق __env
                        // (EN) Load captured variables from closure environment __env
                        for (size_t i = 0; i < captures.size(); i++)
                        {
                            std::string loadReg = b_.newTempRegister();
                            SIRInstruction envLoadInst;
                            envLoadInst.opcode = SIROpcode::ENV_LOAD;
                            envLoadInst.result = SIROperand::Register(loadReg, captures[i].type);
                            envLoadInst.operands.push_back(SIROperand::Register("%__env", SadTypeKind::Integer));
                            envLoadInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i)));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(envLoadInst);

                            std::string allocaName = "%__cap_" + captures[i].varName + "_" + std::to_string(i);
                            SIRInstruction storeInit;
                            storeInit.opcode = SIROpcode::STORE;
                            storeInit.operands.push_back(SIROperand::Register(loadReg, captures[i].type));
                            storeInit.operands.push_back(SIROperand::Register(allocaName, captures[i].type));
                            storeInit.comment = "init captured var from env[" + std::to_string(i) + "]";
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(storeInit);

                            VariableInfo capVar;
                            capVar.name = captures[i].varName;
                            capVar.type = captures[i].type;
                            capVar.registerName = allocaName;
                            capVar.isMutable = true;
                            capVar.scopeLevel = b_.currentScopeLevel_;
                            capVar.isCaptured = true;
                            capVar.captureIndex = static_cast<int>(i);
                            capVar.envRegister = "%__env";
                            b_.addVariable(capVar);
                        }

                        // (AR) بناء جسم الدالة
                        // (EN) Build function body
                        if (funcDecl->body)
                        {
                            buildStatement(funcDecl->body.get());
                        }

                        // (AR) تحديث نوع الإرجاع من تعليمات RET الفعلية
                        // (EN) Update return type from actual RET instructions
                        if (innerFunc)
                        {
                            for (const auto &block : innerFunc->basicBlocks)
                            {
                                for (const auto &inst : block->instructions)
                                {
                                    if (inst.opcode == SIROpcode::RET && !inst.operands.empty())
                                    {
                                        SadTypeKind actualRetType = inst.operands[0].dataType;
                                        if (actualRetType != SadTypeKind::Void &&
                                            actualRetType != SadTypeKind::Unknown &&
                                            actualRetType != SadTypeKind::Integer &&
                                            returnType == SadTypeKind::Integer)
                                        {
                                            returnType = actualRetType;
                                            innerFunc->returnType = returnType;
                                        }
                                        else if (returnType == SadTypeKind::Integer &&
                                                 actualRetType == SadTypeKind::Integer)
                                        {
                                            const std::string &srcReg = inst.operands[0].name;
                                            for (const auto &cap : captures)
                                            {
                                                std::string capAllocaName = "%__cap_" + cap.varName + "_";
                                                if (srcReg.find(capAllocaName) != std::string::npos ||
                                                    srcReg == "%" + cap.varName)
                                                {
                                                    if (cap.type != SadTypeKind::Integer)
                                                    {
                                                        returnType = cap.type;
                                                        innerFunc->returnType = returnType;
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                        break;
                                    }
                                }
                                if (returnType != SadTypeKind::Integer)
                                    break;
                            }
                        }

                        // (AR) إضافة RET_VOID إذا لم يكن هناك return
                        // (EN) Add RET_VOID if no return statement
                        if (b_.currentBlock_ && !b_.currentBlock_->instructions.empty())
                        {
                            auto &lastInst = b_.currentBlock_->instructions.back();
                            if (lastInst.opcode != SIROpcode::RET &&
                                lastInst.opcode != SIROpcode::RET_VOID &&
                                lastInst.opcode != SIROpcode::BR &&
                                lastInst.opcode != SIROpcode::BR_COND)
                            {
                                SIRInstruction retVoid;
                                retVoid.opcode = SIROpcode::RET_VOID;
                                b_.currentBlock_->addInstruction(retVoid);
                            }
                        }

                        b_.exitScope();

                        // (AR) إضافة الدالة للوحدة
                        // (EN) Add function to module
                        if (b_.module_)
                        {
                            b_.module_->addFunction(innerFunc);
                        }

                        // (AR) تسجيل في جدول الدوال
                        // (EN) Register in function table
                        FunctionInfo funcInfo;
                        funcInfo.name = innerFuncName;
                        funcInfo.returnType = returnType;
                        funcInfo.parameters = innerFunc->getParameters();
                        funcInfo.sirFunction = innerFunc;
                        b_.functionTable_[innerFuncName] = funcInfo;

                        // (AR) استعادة السياق السابق
                        // (EN) Restore previous context
                        b_.restoreContext(std::move(savedCtx));

                        // ================================================================
                        // (AR) إنشاء CLOSURE_CREATE مع القيم الملتقطة في السياق الأب
                        // (EN) Create CLOSURE_CREATE with captured values in parent context
                        // ================================================================
                        std::string closureReg = b_.newTempRegister();
                        SIRInstruction closureInst;
                        closureInst.opcode = SIROpcode::CLOSURE_CREATE;
                        closureInst.result = SIROperand::Register(closureReg, SadTypeKind::Function);
                        closureInst.operands.push_back(SIROperand::Function(innerFuncName));
                        for (const auto &cap : captures)
                        {
                            VariableInfo *capVar = b_.lookupVariable(cap.varName);
                            if (capVar)
                            {
                                std::string capLoadReg = b_.newTempRegister();
                                SIRInstruction capLoadInst;
                                capLoadInst.opcode = SIROpcode::LOAD;
                                capLoadInst.result = SIROperand::Register(capLoadReg, capVar->type);
                                capLoadInst.operands.push_back(SIROperand::Register(capVar->registerName, capVar->type));
                                if (b_.currentBlock_)
                                    b_.currentBlock_->addInstruction(capLoadInst);
                                closureInst.operands.push_back(SIROperand::Register(capLoadReg, capVar->type));
                            }
                            else
                            {
                                closureInst.operands.push_back(SIROperand::Register(cap.registerName, cap.type));
                            }
                        }
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(closureInst);
                        // ================================================================
                        // (AR) تسجيل اسم الدالة الأصلي كمتغير محلي يحمل الإغلاق
                        //      هذا يسمح بـ: جامع = إنشاء_جامع() ثم جامع(5)
                        //      حيث "ارجع أضف" يُرجع قيمة الإغلاق المخزّنة في المتغير
                        // (EN) Register original function name as local variable holding closure
                        //      This enables: acc = create_accumulator() then acc(5)
                        //      where "return add" returns the closure value stored in the variable
                        // ================================================================
                        VariableInfo closureVar;
                        closureVar.name = funcDecl->name;
                        closureVar.type = SadTypeKind::Function;
                        closureVar.registerName = closureReg;
                        closureVar.isMutable = false;
                        closureVar.scopeLevel = b_.currentScopeLevel_;
                        closureVar.closureLambdaName = innerFuncName;
                        b_.addVariable(closureVar);

                        // (AR) لا نُسجّل في b_.functionTable_ باسم الدالة الأصلي
                        //      لأن ذلك يتسبب بأن buildCallExpression يُصدر CALL عادي
                        //      بدلاً من CLOSURE_CALL — مما يسبب خطأ linker
                        //      (unresolved external symbol)
                        //      المتغير المُسجّل أعلاه يكفي لتوجيه الاستدعاء إلى CLOSURE_CALL
                        // (EN) Do NOT register in b_.functionTable_ with original name
                        //      because buildCallExpression would emit a direct CALL
                        //      instead of CLOSURE_CALL — causing linker error
                        //      The variable registered above is sufficient for CLOSURE_CALL routing
                    }
                    return;
                }

                // (AR) IfStmt - جملة if (statements.h:104)
                // (EN) If statement
                if (auto ifStmt = dynamic_cast<Sad::AST::IfStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found IfStmt" << std::endl;
#endif
                    buildIfStatement(ifStmt);
                    return;
                }

                // (AR) WhileStmt - حلقة while (statements.h:143)
                // (EN) While loop
                if (auto whileStmt = dynamic_cast<Sad::AST::WhileStmt *>(stmt))
                {
                    buildWhileLoop(whileStmt);
                    return;
                }

                // (AR) ForStmt - حلقة for (statements.h:187)
                // (EN) For loop
                if (auto forStmt = dynamic_cast<Sad::AST::ForStmt *>(stmt))
                {
                    buildForLoop(forStmt);
                    return;
                }

                // (AR) ForRangeStmt - حلقة لكل في (statements.h:228)
                // (EN) For-range loop (foreach)
                if (auto forRangeStmt = dynamic_cast<Sad::AST::ForRangeStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ForRangeStmt" << std::endl;
#endif
                    buildForRangeLoop(forRangeStmt);
                    return;
                }

                // (AR) ReturnStmt - جملة return (statements.h:266)
                // (EN) Return statement
                if (auto returnStmt = dynamic_cast<Sad::AST::ReturnStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ReturnStmt" << std::endl;
#endif
                    buildReturnStatement(returnStmt);
                    return;
                }

                // (AR) BreakStmt - جملة break (statements.h:360)
                // (EN) Break statement
                if (auto breakStmt = dynamic_cast<Sad::AST::BreakStmt *>(stmt))
                {
                    buildBreakStatement(breakStmt);
                    return;
                }

                // (AR) ContinueStmt - جملة continue (statements.h:392)
                // (EN) Continue statement
                if (auto continueStmt = dynamic_cast<Sad::AST::ContinueStmt *>(stmt))
                {
                    buildContinueStatement(continueStmt);
                    return;
                }

                // (AR) AsmBlockStmt — كتلة لهجة التجميع «تجميع … نهاية» (م١ RFC اللهجات)
                // (EN) Assembly dialect block "تجميع … نهاية"
                if (auto asmBlock = dynamic_cast<Sad::AST::AsmBlockStmt *>(stmt))
                {
                    buildAsmBlock(asmBlock);
                    return;
                }

                // (AR) ExprStmt - جملة تعبير (statements.h:38)
                // (EN) Expression statement
                if (auto exprStmt = dynamic_cast<Sad::AST::ExprStmt *>(stmt))
                {
                    // (AR) تنفيذ التعبير فقط (نتجاهل النتيجة)
                    // (EN) Execute expression only (ignore result)
                    if (exprStmt->expression)
                    {
                        b_.buildExpression(exprStmt->expression.get());
                    }
                    return;
                }

                // (AR) VarDeclStmt - تصريح متغير (statements.h:74)
                // (EN) Variable declaration
                if (auto varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found VarDeclStmt: " << varDecl->name << std::endl;
#endif
                    // (AR) اللبنة 3.16: رفض المصفوفة الساكنة داخل دالّة يقع في تعريف
                    //      buildLocalVariable (نقطة تجميع كلّ المسارات) — انظر statement_assign_if.cpp.
                    buildLocalVariable(varDecl);
                    return;
                }

                // ================================================================
                // (AR) MultiVarDeclStmt - تصريح متغيرات متعددة بفاصلة
                //      مثال: متغير اسم = "أحمد"، عمر = 20، وظيفة = "مهندس"
                //      نُفكّك إلى تصريحات VarDeclStmt فردية ونبنيها واحدة تلو الأخرى
                // (EN) Multiple variable declarations separated by comma
                //      Example: var name = "Ahmad", age = 20, job = "Engineer"
                //      We unpack to individual VarDeclStmt and build each one
                // ================================================================
                if (auto multiVarDecl = dynamic_cast<Sad::AST::MultiVarDeclStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found MultiVarDeclStmt with " << multiVarDecl->declarations.size() << " declarations" << std::endl;
#endif
                    for (auto &decl : multiVarDecl->declarations)
                    {
                        if (decl)
                        {
                            buildStatement(decl.get());
                        }
                    }
                    return;
                }

                // (AR) TupleDestructureStmt - تفكيك صف إلى متغيرات: متغير (أ، ب) = صف
                // (EN) Tuple destructuring: var (a, b) = tuple
                if (auto tupleDestr = dynamic_cast<Sad::AST::TupleDestructureStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TupleDestructureStmt with " << tupleDestr->names.size() << " names" << std::endl;
#endif
                    // (AR) بناء تعبير الصف المصدر
                    // (EN) Build source tuple expression
                    auto tupleResult = b_.buildExpression(tupleDestr->initializer.get());

                    // (AR) استخراج كل عنصر وتسجيله كمتغير محلي
                    // (EN) Extract each element and register as local variable
                    for (size_t i = 0; i < tupleDestr->names.size(); ++i)
                    {
                        const std::string &name = tupleDestr->names[i];
                        if (name == "_")
                            continue; // (AR) تجاهل / (EN) Skip placeholder

                        // (AR) استخراج العنصر من الصف باستخدام TUPLE_GET
                        // (EN) Extract element from tuple using TUPLE_GET
                        std::string elemReg = b_.newTempRegister();
                        if (b_.currentBlock_)
                        {
                            SIRInstruction getInst(SIROpcode::TUPLE_GET);
                            getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                            getInst.operands.push_back(SIROperand::Register(tupleResult.registerName, tupleResult.type));
                            getInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i)));
                            getInst.comment = "tuple destructure: " + name + " = tuple[" + std::to_string(i) + "]";
                            b_.currentBlock_->addInstruction(getInst);
                        }

                        // (AR) تسجيل المتغير المحلي
                        // (EN) Register local variable
                        VariableInfo varInfo;
                        varInfo.name = name;
                        varInfo.type = SadTypeKind::Integer;
                        varInfo.registerName = "%" + name;
                        varInfo.isGlobal = false;
                        varInfo.isMutable = !tupleDestr->isConst;
                        varInfo.scopeLevel = b_.currentScopeLevel_;
                        b_.addVariable(varInfo);

                        // (AR) تخزين القيمة المستخرجة في السجل
                        // (EN) Store extracted value in register
                        if (b_.currentBlock_)
                        {
                            SIRInstruction storeInst(SIROpcode::STORE);
                            storeInst.operands.push_back(SIROperand::Register(varInfo.registerName, SadTypeKind::Integer));
                            storeInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                            storeInst.comment = "store destructured: " + name;
                            b_.currentBlock_->addInstruction(storeInst);
                        }
                    }
                    return;
                }

                // (AR) TemplateFunctionDecl - تصريح دالة قالب (declarations.h:470)
                // (EN) Template function declaration
                if (auto templateDecl = dynamic_cast<Sad::AST::TemplateFunctionDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TemplateFunctionDecl: " << templateDecl->name << std::endl;
#endif
                    b_.buildTemplateFunction(templateDecl);
                    return;
                }

                // (AR) TemplateClassDecl - تصريح صنف قالب (declarations.h:520)
                // (EN) Template class declaration — store for later instantiation
                if (auto templateClassDecl = dynamic_cast<Sad::AST::TemplateClassDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TemplateClassDecl: " << templateClassDecl->name
                              << " with " << templateClassDecl->typeParameters.size() << " type params" << std::endl;
#endif

                    // (AR) تسجيل صنف القالب — يُنشأ عند TemplateInstantiation
                    // (EN) Register template class — instantiated at TemplateInstantiation
                    b_.templateClasses_[templateClassDecl->name] = templateClassDecl;

                    // (AR) إنشاء SIRClass فارغ بالاسم الأساسي ليُعرف في الجدول
                    // (EN) Create empty SIRClass with base name so it's known
                    std::string parentClass;
                    if (!templateClassDecl->superclasses.empty())
                    {
                        parentClass = templateClassDecl->superclasses[0];
                    }
                    auto sirClass = std::make_shared<SIRClass>(templateClassDecl->name, parentClass);

                    for (const auto &tp : templateClassDecl->typeParameters)
                    {
                        sirClass->addField(tp.name, SadTypeKind::Pointer);
                    }

                    b_.module_->addClass(sirClass);
                    return;
                }

                // (AR) MatchStmt - جملة match (pattern_nodes.h:MatchStmt)
                // (EN) Match statement (pattern matching)
                if (auto matchStmt = dynamic_cast<Sad::AST::MatchStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found MatchStmt" << std::endl;
#endif
                    buildMatchStatement(matchStmt);
                    return;
                }

                // ========================================================================
                // (AR) DeferStmt - جملة التأجيل (أجّل) — تنظيف مضمون عند خروج الدالة
                // (EN) Defer statement — guaranteed cleanup on function exit
                //
                // (AR) في المترجم: نُسجّل الجملة المؤجلة في قائمة الدالة الحالية.
                //      عند بناء buildReturnStatement، نُصدر كود الـ defer قبل RET.
                //      هذا مشابه لكيفية تنفيذ defer في llvm.org/docs/LangImpl10.html
                // (EN) In the compiler: register deferred statement in current function.
                //      When buildReturnStatement is called, emit defer code before RET.
                //      Similar to how defer is implemented via cleanup blocks.
                // ========================================================================
                if (auto deferStmt = dynamic_cast<Sad::AST::DeferStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found DeferStmt — registering deferred statement" << std::endl;
#endif
                    if (!b_.currentDeferStackReg_.empty())
                    {
                        BuildResult closureResult = buildDeferredClosure(deferStmt->body.get());
                        if (!closureResult.registerName.empty() && b_.currentBlock_)
                        {
                            SIRInstruction appendInst(SIROpcode::BUILTIN_ARRAY_APPEND);
                            appendInst.result = SIROperand::Register(b_.newTempRegister(), SadTypeKind::Void);
                            appendInst.operands.push_back(SIROperand::Register(b_.currentDeferStackReg_, SadTypeKind::Array));
                            appendInst.operands.push_back(SIROperand::Register(closureResult.registerName, SadTypeKind::Function));
                            appendInst.comment = "register deferred closure at runtime";
                            b_.currentBlock_->addInstruction(appendInst);
                        }
                        return;
                    }

                    // (AR) تسجيل الجملة المؤجلة في مكدس الدالة الحالية
                    // (EN) Register deferred statement in current function's stack
                    if (b_.currentFunction_)
                    {
                        b_.deferredStatements_.push_back(deferStmt->body.get());
                    }
                    return;
                }

                // ========================================================================
                // (AR) SwitchStmt - جملة حالة/switch (statements.h:SwitchStmt)
                // (EN) Switch statement
                // ========================================================================
                if (auto switchStmt = dynamic_cast<Sad::AST::SwitchStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found SwitchStmt with " << switchStmt->cases.size() << " cases" << std::endl;
#endif

                    // (AR) بناء تعبير الحالة
                    // (EN) Build switch expression
                    auto exprResult = b_.buildExpression(switchStmt->expression.get());

                    // (AR) إنشاء كتل لكل حالة وكتلة الخروج
                    // (EN) Create blocks for each case and exit block
                    std::string exitLabel = b_.newLabel("switch_exit");
                    auto exitBlock = b_.createBasicBlock(exitLabel);

                    std::string defaultLabel = exitLabel;
                    if (switchStmt->defaultCase)
                    {
                        defaultLabel = b_.newLabel("switch_default");
                    }

                    // (AR) بناء سلسلة من فروع شرطية لكل حالة
                    // (EN) Build chain of conditional branches for each case
                    std::vector<std::pair<std::string, std::shared_ptr<SIRBasicBlock>>> caseBlocks;

                    for (size_t i = 0; i < switchStmt->cases.size(); ++i)
                    {
                        std::string caseLabel = b_.newLabel("switch_case_" + std::to_string(i));
                        auto caseBlock = b_.createBasicBlock(caseLabel);
                        caseBlocks.push_back({caseLabel, caseBlock});
                    }

                    // (AR) بناء شروط المقارنة
                    // (EN) Build comparison conditions
                    for (size_t i = 0; i < switchStmt->cases.size(); ++i)
                    {
                        auto &caseBranch = switchStmt->cases[i];

                        // (AR) بناء تعبير قيمة الحالة
                        // (EN) Build case value expression
                        auto caseVal = b_.buildExpression(caseBranch.value.get());

                        // (AR) مقارنة المساواة
                        // (EN) Equality comparison
                        std::string cmpReg = b_.newTempRegister();
                        SIRInstruction cmpInst = SIRInstruction::Binary(
                            SIROpcode::EQ,
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Register(exprResult.registerName, exprResult.type),
                            SIROperand::Register(caseVal.registerName, caseVal.type));
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(cmpInst);
                        }

                        // (AR) التحديد: إذا تطابقت القيمة، اذهب لكتلة الحالة
                        // (EN) Branch: if matched, go to case block
                        std::string nextCheckLabel;
                        if (i + 1 < switchStmt->cases.size())
                        {
                            nextCheckLabel = b_.newLabel("switch_check_" + std::to_string(i + 1));
                        }
                        else
                        {
                            nextCheckLabel = defaultLabel;
                        }

                        auto nextCheckBlock = b_.createBasicBlock(nextCheckLabel);

                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                                SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                SIROperand::Label(caseBlocks[i].first),
                                SIROperand::Label(nextCheckLabel)));
                        }

                        if (b_.currentFunction_)
                        {
                            b_.currentFunction_->addBasicBlock(nextCheckBlock);
                        }
                        b_.currentBlock_ = nextCheckBlock;
                    }

                    // (AR) ISSUE-054: إن لم توجد أيّ حالة «عندما»، فحلقة المقارنة أعلاه لم تُنفَّذ،
                    //      فبقيت كتلة المدخل بلا تفرّع إلى الافتراضيّ ⇒ جسم الافتراضيّ كودٌ ميت
                    //      (المترجم يبني ولا يُخرِج شيئاً). نُضيف قفزاً صريحاً إلى الافتراضيّ/الخروج.
                    // (EN) ISSUE-054: with no 'when' cases the comparison loop above didn't run,
                    //      so the entry block never branched to default ⇒ the default body is dead
                    //      code (compiler builds but emits nothing). Add an explicit jump.
                    if (switchStmt->cases.empty() && b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(
                            SIRInstruction::Branch(SIROperand::Label(defaultLabel)));
                    }

                    // (AR) بناء أجسام الحالات
                    // (EN) Build case bodies
                    for (size_t i = 0; i < switchStmt->cases.size(); ++i)
                    {
                        if (b_.currentFunction_)
                        {
                            b_.currentFunction_->addBasicBlock(caseBlocks[i].second);
                        }
                        b_.currentBlock_ = caseBlocks[i].second;

                        b_.enterScope();
                        if (switchStmt->cases[i].body)
                        {
                            buildStatement(switchStmt->cases[i].body.get());
                        }
                        b_.exitScope();

                        // (AR) لا fall-through — القفز مباشرة للخروج
                        // (EN) No fall-through — jump directly to exit
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }
                    }

                    // (AR) بناء الحالة الافتراضية
                    // (EN) Build default case
                    if (switchStmt->defaultCase)
                    {
                        auto defaultBlock = b_.createBasicBlock(defaultLabel);
                        if (b_.currentFunction_)
                        {
                            b_.currentFunction_->addBasicBlock(defaultBlock);
                        }
                        b_.currentBlock_ = defaultBlock;

                        b_.enterScope();
                        buildStatement(switchStmt->defaultCase.get());
                        b_.exitScope();

                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }
                    }

                    // (AR) كتلة الخروج
                    // (EN) Exit block
                    if (b_.currentFunction_)
                    {
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    }
                    b_.currentBlock_ = exitBlock;
                    return;
                }

                // === Delegation to sub-functions ===

                // Exception handling (try/catch/finally, raise)
                if (buildStatement_Exceptions(stmt))
                    return;

                // Generator statements (yield, with)
                if (buildStatement_Generators(stmt))
                    return;

                // Type declarations (enum, struct, namespace, class)
                if (buildStatement_Types(stmt))
                    return;

                // Advanced statements (test, atomic, macro, go, select, etc.)
                if (buildStatement_Advanced(stmt))
                    return;

#ifndef NDEBUG
                std::cout << "[DEBUG] Unknown statement type: " << typeid(*stmt).name() << std::endl;
#endif
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
