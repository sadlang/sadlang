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
#include <cstdio>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildFunctionCall - بناء استدعاء دالة
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:464
            // التوقيع / Signature: BuildResult buildFunctionCall(AST::FunctionCallNode* call);
            //
            // المعاملات / Parameters:
            // - call: AST::FunctionCallNode* = Sad::AST::CallExpr* (sir_builder.h:68)
            //
            // CallExpr Members (expressions.h:276):
            // - callee: ExprPtr (line 278)
            // - arguments: ExprList (line 279)
            //
            // الدوال المضمنة / Builtin Functions (sir_types.h:178-187):
            // - طول() / length: STRING_LEN للنصوص، ARRAY_LEN للمصفوفات
            // - اطبع() / print: BUILTIN_PRINT
            // - إلى_رقم() / to_int: STRING_TO_I64
            // - إلى_نص() / to_string: I64_TO_STRING / F64_TO_STRING
            // ============================================================================
            BuildResult SIRBuilder::buildFunctionCall(AST::FunctionCallNode *call)
            {
                if (!call)
                {
                    return BuildResult();
                }
#ifdef SIR_BUILDER_DEBUG
                std::cerr << "[SIR-DBG] buildFunctionCall entered" << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 1: استخراج اسم الدالة من callee
                // (EN) Step 1: Extract function name from callee
                // المصدر: expressions.h:290 - callee: ExprPtr
                // ========================================================================
                std::string funcName;

                // (AR) callee يمكن أن يكون VariableExpr (اسم دالة مباشر)
                // (EN) callee can be VariableExpr (direct function name)
                // المصدر: expressions.h:210-212 - VariableExpr::name
                if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr *>(call->callee.get()))
                {
                    funcName = varExpr->name;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: function name = '" << funcName << "'" << std::endl;
#endif
                }
                else
                {
                    // (AR) لا ندعم استدعاءات غير مباشرة حالياً
                    // (EN) We don't support indirect calls currently
                    errors_.push_back("Error: Only direct function calls are supported");
                    return BuildResult();
                }

                // ========================================================================
                // (AR) معالجة استدعاء الماكرو: اسم!(وسائط)
                //      إذا كان isMacroCall = true، نبحث عن الماكرو في macros_ ونوسّعه
                //      inline: نبني الوسائط، ننشئ نطاقاً معزولاً، نربط المعاملات
                //      بالوسائط، ونبني جسم الماكرو مباشرة في النطاق الحالي.
                //
                //      الاستراتيجية: تحويل الماكرو إلى دالة SIR مُولَّدة تلقائياً
                //      ثم استدعاؤها — يضمن العزل (hygiene) ويدعم ارجع (return) بشكل طبيعي.
                //
                // (EN) Handle macro call: name!(args)
                //      If isMacroCall = true, find macro in macros_ and expand inline.
                //      Strategy: convert macro to auto-generated SIR function then call it —
                //      ensures hygiene and supports return naturally.
                // ========================================================================
                if (call->isMacroCall)
                {
                    auto macroIt = macros_.find(funcName);
                    if (macroIt == macros_.end())
                    {
                        errors_.push_back("Error: Macro '" + funcName + "' is not defined / الماكرو غير معرّف");
                        return BuildResult();
                    }

                    auto *macroDef = macroIt->second;
#ifndef NDEBUG
                    std::cout << "[DEBUG] Expanding macro '" << funcName << "' with "
                              << call->arguments.size() << " arguments" << std::endl;
#endif

                    // (AR) الخطوة 1: التحقق من عدد الوسائط
                    // (EN) Step 1: Validate argument count
                    size_t requiredParams = macroDef->isVariadic
                                                ? macroDef->params.size() - 1
                                                : macroDef->params.size();

                    if (!macroDef->isVariadic && call->arguments.size() != requiredParams)
                    {
                        errors_.push_back("Error: Macro '" + funcName + "' expects " +
                                          std::to_string(requiredParams) + " arguments, got " +
                                          std::to_string(call->arguments.size()));
                        return BuildResult();
                    }
                    if (macroDef->isVariadic && call->arguments.size() < requiredParams)
                    {
                        errors_.push_back("Error: Macro '" + funcName + "' requires at least " +
                                          std::to_string(requiredParams) + " arguments, got " +
                                          std::to_string(call->arguments.size()));
                        return BuildResult();
                    }

                    // (AR) الخطوة 2: بناء الوسائط في النطاق الحالي (قبل دخول النطاق المعزول)
                    // (EN) Step 2: Build arguments in current scope (before entering isolated scope)
                    std::vector<BuildResult> argResults;
                    for (size_t i = 0; i < call->arguments.size(); ++i)
                    {
                        argResults.push_back(buildExpression(call->arguments[i].get()));
                    }

                    // (AR) الخطوة 3: إنشاء دالة SIR مؤقتة للماكرو
                    //      نولد اسماً فريداً لكل استدعاء: __macro_اسم_N
                    //      نوع الإرجاع I64 (يمكن أن يُرجع الماكرو قيمة عبر ارجع)
                    // (EN) Step 3: Create temporary SIR function for macro
                    //      Generate unique name per call: __macro_name_N
                    //      Return type I64 (macro can return value via return)
                    std::string macroFuncName = "__macro_" + funcName + "_" + std::to_string(nextTempRegister_++);

                    // (AR) استنتاج نوع الإرجاع من جسم الماكرو
                    // (EN) Infer return type from macro body
                    SadTypeKind macroRetType = SadTypeKind::Void;
                    if (macroDef->body && hasReturnWithValue(macroDef->body.get()))
                    {
                        macroRetType = inferReturnTypeFromBody(macroDef->body.get());
                        if (macroRetType == SadTypeKind::Void)
                        {
                            macroRetType = SadTypeKind::Integer; // (AR) افتراضي / (EN) default
                        }
                    }

                    auto macroFunc = std::make_shared<SIRFunction>(macroFuncName, macroRetType);

                    // (AR) الخطوة 4: إضافة المعاملات للدالة المؤقتة
                    // (EN) Step 4: Add parameters to temporary function
                    for (size_t i = 0; i < requiredParams; ++i)
                    {
                        SadTypeKind paramType = (i < argResults.size()) ? argResults[i].type : SadTypeKind::Integer;
                        macroFunc->addParameter(SIRParameter(macroDef->params[i], paramType));
                    }

                    // (AR) المعامل المتغير: مصفوفة واحدة تحتوي الباقي
                    // (EN) Variadic parameter: single array containing the rest
                    if (macroDef->isVariadic)
                    {
                        macroFunc->addParameter(SIRParameter(macroDef->params.back(), SadTypeKind::Struct));
                    }

                    // (AR) الخطوة 5: حفظ السياق الحالي وبناء جسم الماكرو في الدالة المؤقتة
                    // (EN) Step 5: Save current context and build macro body in temp function
                    auto savedCtx = saveContext();

                    currentFunction_ = macroFunc;
                    auto entryBlock = createBasicBlock("macro_entry");
                    macroFunc->addBasicBlock(entryBlock);
                    currentBlock_ = entryBlock;

                    // (AR) دخول نطاق معزول وربط المعاملات
                    // (EN) Enter isolated scope and bind parameters
                    enterScope();

                    for (size_t i = 0; i < requiredParams; ++i)
                    {
                        VariableInfo vi;
                        vi.name = macroDef->params[i];
                        vi.type = (i < argResults.size()) ? argResults[i].type : SadTypeKind::Integer;
                        vi.registerName = "%" + macroDef->params[i];
                        vi.isMutable = true;
                        vi.isParameter = true;
                        vi.scopeLevel = currentScopeLevel_;
                        addVariable(vi);
                    }

                    // (AR) المعامل المتغير
                    // (EN) Variadic parameter
                    if (macroDef->isVariadic && !macroDef->params.empty())
                    {
                        VariableInfo vi;
                        vi.name = macroDef->params.back();
                        vi.type = SadTypeKind::Struct;
                        vi.registerName = "%" + macroDef->params.back();
                        vi.isMutable = true;
                        vi.isParameter = true;
                        vi.scopeLevel = currentScopeLevel_;
                        addVariable(vi);
                    }

                    // (AR) بناء جسم الماكرو
                    // (EN) Build macro body
                    if (macroDef->body)
                    {
                        buildStatement(macroDef->body.get());
                    }

                    // (AR) إضافة ReturnVoid إذا لم يكن هناك terminator
                    // (EN) Add ReturnVoid if no terminator exists
                    if (currentBlock_ && !currentBlock_->getTerminator())
                    {
                        currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
                    }

                    exitScope();

                    // (AR) إضافة الدالة المؤقتة إلى الوحدة
                    // (EN) Add temporary function to module
                    module_->addFunction(macroFunc);

                    // (AR) تسجيل الدالة في جدول الدوال
                    // (EN) Register function in function table
                    FunctionInfo fi;
                    fi.name = macroFuncName;
                    fi.returnType = macroRetType;
                    fi.sirFunction = macroFunc;
                    functionTable_[macroFuncName] = fi;

                    // (AR) الخطوة 6: استعادة السياق السابق
                    // (EN) Step 6: Restore previous context
                    restoreContext(std::move(savedCtx));

                    // (AR) الخطوة 7: إصدار استدعاء الدالة المؤقتة
                    // (EN) Step 7: Emit call to temporary function
                    std::vector<SIROperand> callArgs;
                    for (size_t i = 0; i < requiredParams && i < argResults.size(); ++i)
                    {
                        if (argResults[i].isConstant && !argResults[i].constantValue.empty())
                        {
                            switch (argResults[i].type)
                            {
                            case SadTypeKind::Integer:
                                callArgs.push_back(SIROperand::ConstantI64(std::stoll(argResults[i].constantValue)));
                                break;
                            case SadTypeKind::Float:
                                callArgs.push_back(SIROperand::ConstantF64(std::stod(argResults[i].constantValue)));
                                break;
                            case SadTypeKind::String:
                                callArgs.push_back(SIROperand::ConstantString(argResults[i].constantValue));
                                break;
                            default:
                                callArgs.push_back(SIROperand::Register(argResults[i].registerName, argResults[i].type));
                                break;
                            }
                        }
                        else
                        {
                            callArgs.push_back(SIROperand::Register(argResults[i].registerName, argResults[i].type));
                        }
                    }

                    // (AR) الوسائط المتغيرة: تُمرر كوسائط إضافية
                    // (EN) Variadic arguments: passed as extra arguments
                    if (macroDef->isVariadic)
                    {
                        for (size_t i = requiredParams; i < argResults.size(); ++i)
                        {
                            if (argResults[i].isConstant && !argResults[i].constantValue.empty())
                            {
                                switch (argResults[i].type)
                                {
                                case SadTypeKind::Integer:
                                    callArgs.push_back(SIROperand::ConstantI64(std::stoll(argResults[i].constantValue)));
                                    break;
                                case SadTypeKind::Float:
                                    callArgs.push_back(SIROperand::ConstantF64(std::stod(argResults[i].constantValue)));
                                    break;
                                case SadTypeKind::String:
                                    callArgs.push_back(SIROperand::ConstantString(argResults[i].constantValue));
                                    break;
                                default:
                                    callArgs.push_back(SIROperand::Register(argResults[i].registerName, argResults[i].type));
                                    break;
                                }
                            }
                            else
                            {
                                callArgs.push_back(SIROperand::Register(argResults[i].registerName, argResults[i].type));
                            }
                        }
                    }

                    std::string resultReg = newTempRegister();

                    if (currentBlock_)
                    {
                        SIRInstruction callInst;
                        callInst.opcode = SIROpcode::CALL;
                        callInst.result = SIROperand::Register(resultReg, macroRetType);
                        callInst.operands.push_back(SIROperand::Function(macroFuncName));
                        for (auto &arg : callArgs)
                        {
                            callInst.operands.push_back(arg);
                        }
                        currentBlock_->addInstruction(callInst);
                    }

                    return BuildResult(resultReg, macroRetType);
                }

                // ========================================================================
                // (AR) تحميل عامل الاستدعاء الزائد (): إذا كان الاسم كائناً في classInstanceTypes_
                //      مثال: ض(7) حيث ض كائن من صنف يحتوي 'عامل ()(قيمة)'
                //      يتوافق مع expression_evaluator_calls.cpp:findOperator("()")
                // (EN) Call operator overload (): if name is an object in classInstanceTypes_
                //      Example: ض(7) where ض is an object with 'operator ()(value)'
                //      Matches expression_evaluator_calls.cpp:findOperator("()")
                // ========================================================================
                {
                    auto classIt = classInstanceTypes_.find(funcName);
                    if (classIt != classInstanceTypes_.end())
                    {
                        std::string objClassName = classIt->second;
                        // (AR) البحث عن __op_call__ في سلسلة الوراثة
                        // (EN) Search for __op_call__ in inheritance chain
                        std::string searchClass = objClassName;
                        std::string fullOpName;
                        bool found = false;
                        while (!searchClass.empty())
                        {
                            fullOpName = searchClass + ".__op_call__";
                            auto funcIt = functionTable_.find(fullOpName);
                            if (funcIt != functionTable_.end())
                            {
                                found = true;
                                break;
                            }
                            auto parentClass = module_->getClass(searchClass);
                            if (parentClass && !parentClass->parentClass.empty())
                            {
                                searchClass = parentClass->parentClass;
                            }
                            else
                            {
                                break;
                            }
                        }

                        if (found)
                        {
                            // (AR) بناء المعاملات
                            // (EN) Build arguments
                            std::vector<SIROperand> callArgOps;
                            for (const auto &arg : call->arguments)
                            {
                                BuildResult argResult = buildExpression(arg.get());
                                if (argResult.isConstant && !argResult.constantValue.empty())
                                {
                                    switch (argResult.type)
                                    {
                                    case SadTypeKind::Integer:
                                        callArgOps.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                                        break;
                                    case SadTypeKind::Float:
                                        callArgOps.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                                        break;
                                    case SadTypeKind::String:
                                        callArgOps.push_back(SIROperand::ConstantString(argResult.constantValue));
                                        break;
                                    default:
                                        callArgOps.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                                        break;
                                    }
                                }
                                else
                                {
                                    callArgOps.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                                }
                            }

                            // (AR) إصدار OBJECT_CALL
                            // (EN) Emit OBJECT_CALL
                            std::string resultReg = newTempRegister();
                            auto &opInfo = functionTable_[fullOpName];
                            SadTypeKind returnType = opInfo.returnType;

                            if (currentBlock_)
                            {
                                SIRInstruction callInst;
                                callInst.opcode = SIROpcode::OBJECT_CALL;
                                callInst.result = SIROperand::Register(resultReg, returnType);
                                // (AR) البحث عن سجل الكائن
                                // (EN) Look up object register
                                VariableInfo *varInfo = lookupVariable(funcName);
                                std::string objReg = varInfo ? varInfo->registerName : ("%" + funcName);
                                callInst.operands.push_back(SIROperand::Register(objReg, SadTypeKind::Integer));
                                callInst.operands.push_back(SIROperand::ConstantString("__op_call__"));
                                for (auto &op : callArgOps)
                                {
                                    callInst.operands.push_back(op);
                                }
                                currentBlock_->addInstruction(callInst);
                            }

                            BuildResult result(resultReg, returnType);
                            result.className = objClassName;
                            return result;
                        }
                    }
                }

                // ========================================================================
                // (AR) التعامل مع استدعاء باني الأب: أساس(...) / الأساس(...) / super(...)
                // (EN) Handle super constructor call: أساس(...) / الأساس(...) / super(...)
                // ========================================================================
                if ((funcName == "\xD8\xA3\xD8\xB3\xD8\xA7\xD8\xB3" ||                 // أساس
                     funcName == "\xD8\xA7\xD9\x84\xD8\xA3\xD8\xB3\xD8\xA7\xD8\xB3" || // الأساس
                     funcName == "\xD8\xA7\xD8\xB3\xD8\xA7\xD8\xB3" ||                 // اساس
                     funcName == "\xD8\xA7\xD9\x84\xD8\xA7\xD8\xB3\xD8\xA7\xD8\xB3" || // الاساس
                     funcName == "super") &&
                    !currentClassName_.empty())
                {
                    // (AR) البحث عن الصنف الأب
                    // (EN) Find parent class
                    auto sirClass = module_->getClass(currentClassName_);
                    if (sirClass && !sirClass->parentClass.empty())
                    {
                        std::string parentCtorName = sirClass->parentClass + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // بناء

                        // (AR) بناء المعاملات
                        // (EN) Build arguments
                        std::vector<SIROperand> superArgs;
                        superArgs.push_back(SIROperand::Register(kSelfRegisterName, SadTypeKind::Integer)); // self
                        for (const auto &arg : call->arguments)
                        {
                            BuildResult argResult = buildExpression(arg.get());
                            superArgs.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                        }

                        // (AR) إصدار تعليمة CALL لباني الأب
                        // (EN) Emit CALL instruction for parent constructor
                        std::string superResultReg = newTempRegister();
                        SIRInstruction callInst;
                        callInst.opcode = SIROpcode::CALL;
                        callInst.result = SIROperand::Register(superResultReg, SadTypeKind::Void);
                        callInst.operands.push_back(SIROperand::Register(parentCtorName, SadTypeKind::Void));
                        for (auto &op : superArgs)
                        {
                            callInst.operands.push_back(op);
                        }
                        if (currentBlock_)
                            currentBlock_->addInstruction(callInst);

                        return BuildResult(superResultReg, SadTypeKind::Void);
                    }
                    // (AR) لا يوجد صنف أب - نتجاهل
                    // (EN) No parent class - ignore
                    return BuildResult();
                }

                // ========================================================================
                // (AR) الخطوة 2: بناء المعاملات (arguments)
                // (EN) Step 2: Build arguments
                // المصدر: expressions.h:291 - arguments: ExprList = vector<ExprPtr>
                // ========================================================================
                std::vector<SIROperand> argOperands;
                std::vector<BuildResult> argResults;

                for (const auto &arg : call->arguments)
                {
                    // (AR) بناء كل معامل باستخدام buildExpression (sir_builder.h:440)
                    // (EN) Build each argument using buildExpression
                    BuildResult argResult = buildExpression(arg.get());

                    // (AR) فحص الفشل: فقط إذا لم يكن ثابتاً ولا في سجل
                    // (EN) Check failure: only if neither constant nor in register
                    if (argResult.registerName.empty() && !argResult.isConstant)
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildFunctionCall: failed to build argument" << std::endl;
#endif
                        errors_.push_back("Error: Failed to build function argument");
                        return BuildResult();
                    }

                    argResults.push_back(argResult);

                    // (AR) إنشاء SIROperand للمعامل (sir_types.h:355-362 - Register() أو Constant*)
                    // (EN) Create SIROperand for argument
                    SIROperand argOp;

                    // (AR) إذا كان المعامل ثابتاً، استخدم Constant* factory methods
                    // (EN) If argument is constant, use Constant* factory methods
                    if (argResult.isConstant)
                    {
                        switch (argResult.type)
                        {
                        case SadTypeKind::String:
                            // استخدام ConstantString (sir_types.h:353-361)
                            argOp = SIROperand::ConstantString(argResult.constantValue);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildFunctionCall: added STRING constant='"
                                      << argResult.constantValue << "'" << std::endl;
#endif
                            break;
                        case SadTypeKind::Integer:
                        {
                            int64_t intVal = std::stoll(argResult.constantValue);
                            argOp = SIROperand::ConstantI64(intVal);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildFunctionCall: added I64 constant="
                                      << intVal << std::endl;
#endif
                            break;
                        }
                        case SadTypeKind::Float:
                        {
                            double floatVal = std::stod(argResult.constantValue);
                            argOp = SIROperand::ConstantF64(floatVal);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildFunctionCall: added F64 constant="
                                      << floatVal << std::endl;
#endif
                            break;
                        }
                        case SadTypeKind::Boolean:
                        {
                            bool boolVal = (argResult.constantValue == "true");
                            argOp = SIROperand::ConstantBool(boolVal);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildFunctionCall: added BOOL constant="
                                      << boolVal << std::endl;
#endif
                            break;
                        }
                        default:
                            // للأنواع الأخرى، استخدم Register كافتراضي
                            argOp = SIROperand::Register(argResult.registerName, argResult.type);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildFunctionCall: added register='"
                                      << argResult.registerName << "', type="
                                      << static_cast<int>(argResult.type) << std::endl;
#endif
                            break;
                        }
                    }
                    else
                    {
                        // (AR) للمتغيرات، استخدم Register
                        // (EN) For variables, use Register
                        argOp = SIROperand::Register(argResult.registerName, argResult.type);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildFunctionCall: added register='"
                                  << argResult.registerName << "', type="
                                  << static_cast<int>(argResult.type) << std::endl;
#endif
                    }

                    argOperands.push_back(argOp);
                }

                // ========================================================================
                // (AR) الخطوة 2.3: ملء القيم الافتراضية للمعاملات الناقصة
                //      إذا كان عدد الوسائط أقل من عدد المعاملات، نبحث عن قيم افتراضية
                //      في AST ونبنيها كوسائط إضافية
                // (EN) Step 2.3: Fill default parameter values for missing arguments
                //      If fewer arguments than parameters, look for default values in AST
                //      and build them as additional arguments
                // ========================================================================
                {
                    auto ftIt2 = functionTable_.find(funcName);
                    if (ftIt2 != functionTable_.end() && ftIt2->second.astDecl)
                    {
                        auto *astDecl = ftIt2->second.astDecl;
                        size_t numParams = astDecl->parameters.size();
                        size_t numArgs = argOperands.size();
                        if (numArgs < numParams)
                        {
                            for (size_t i = numArgs; i < numParams; i++)
                            {
                                if (astDecl->parameters[i].defaultValue)
                                {
                                    // (AR) بناء التعبير الافتراضي
                                    // (EN) Build default value expression
                                    auto defResult = buildExpression(astDecl->parameters[i].defaultValue.get());
                                    argResults.push_back(defResult);

                                    SIROperand defOp;
                                    if (defResult.isConstant)
                                    {
                                        switch (defResult.type)
                                        {
                                        case SadTypeKind::String:
                                            defOp = SIROperand::ConstantString(defResult.constantValue);
                                            break;
                                        case SadTypeKind::Integer:
                                            defOp = SIROperand::ConstantI64(std::stoll(defResult.constantValue));
                                            break;
                                        case SadTypeKind::Float:
                                            defOp = SIROperand::ConstantF64(std::stod(defResult.constantValue));
                                            break;
                                        case SadTypeKind::Boolean:
                                            defOp = SIROperand::ConstantBool(defResult.constantValue == "true");
                                            break;
                                        default:
                                            defOp = SIROperand::Register(defResult.registerName, defResult.type);
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        defOp = SIROperand::Register(defResult.registerName, defResult.type);
                                    }
                                    argOperands.push_back(defOp);
                                }
                            }
                        }
                    }
                }

                // ========================================================================
                // (AR) الخطوة 2.5: التحقق من الدوال المضمنة
                // (EN) Step 2.5: Check for builtin functions
                // ========================================================================

                // ========================================================================
                // (AR) إصلاح: تحقق أولاً إذا كانت الدالة معرّفة من المستخدم
                // (EN) Fix: First check if function is user-defined before checking builtins
                // هذا يمنع الدوال المضمنة من التداخل مع دوال المستخدم التي تحمل نفس الاسم
                // This prevents builtins from shadowing user-defined functions with same name
                // (AR) نفحص أيضاً الأسماء المستعارة للامدا — لأن لامدا مُسندة لمتغير (مثل: جمع = لامدا...)
                //       يجب أن تأخذ الأولوية على الدوال المضمنة التي تحمل نفس الاسم
                // (EN) Also check lambda aliases — a lambda assigned to a variable (e.g. جمع = lambda...)
                //       must take priority over builtins with the same name
                // (AR) [إصلاح] فحص المتغيرات أيضاً — لامدا مُسندة لمتغير (مثل: تحقق = لامدا...)
                //       لا تُسجّل في lambdaAliases_ في نظام الإغلاقات الجديد (CLOSURE_CALL)
                //       لكن المتغير يحمل closure، فيجب أن يأخذ الأولوية على الدوال المضمنة
                //       بدون هذا: "تحقق" كـ builtin verify يطغى على لامدا المستخدم بنفس الاسم
                //       نفحص فقط المتغيرات من نوع Function لتجنب طغيان متغير عادي (رقم/نص)
                //       على دالة مضمنة بنفس الاسم (مثلاً: متغير طول = 5 لا يطغى على طول())
                // (EN) [Fix] Also check variables — lambda assigned to variable (e.g. check = lambda...)
                //       is NOT registered in lambdaAliases_ in new closure system (CLOSURE_CALL)
                //       but the variable holds a closure, so it must take priority over builtins
                //       Without this: builtin "تحقق" (verify) shadows user's lambda with same name
                //       Only check Function-typed variables to avoid shadowing builtins with int/string vars
                // ========================================================================
                VariableInfo *closureVarCheck = lookupVariable(funcName);
                bool isClosureVariable = (closureVarCheck != nullptr && closureVarCheck->type == SadTypeKind::Function);
                bool isUserDefinedFunction = (functionTable_.find(funcName) != functionTable_.end()) ||
                                             (lambdaAliases_.find(funcName) != lambdaAliases_.end()) ||
                                             isClosureVariable;

                // (AR) دالة طول() - STRING_LEN للنصوص، ARRAY_LEN للمصفوفات
                // (EN) length() function - STRING_LEN for strings, ARRAY_LEN for arrays

                // ========================================================================
                // (AR) ״§„״×״­‚‚ …† ״§„״¯ˆ״§„ ״§„…״¯…״¬״© (…‚״³…״© ״¥„‰ …„†)
                // (EN) Check builtin functions (split into two files)
                // ========================================================================
                auto builtinResult = buildBuiltinCallCore(funcName, isUserDefinedFunction, argResults, argOperands);
                if (builtinResult.has_value())
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG] buildFunctionCall: handled by BuiltinCore! type="
                              << static_cast<int>(builtinResult.value().type) << std::endl;
#endif
                    return builtinResult.value();
                }
                builtinResult = buildBuiltinCallSystem(funcName, isUserDefinedFunction, argResults, argOperands);
                if (builtinResult.has_value())
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG] buildFunctionCall: handled by BuiltinSystem! type="
                              << static_cast<int>(builtinResult.value().type) << std::endl;
#endif
                    return builtinResult.value();
                }

                // ========================================================================
                // (AR) الخطوة 2.7: حل الأسماء المستعارة للامدا
                // (EN) Step 2.7: Resolve lambda aliases
                // مثال: ف(5) حيث ف = لامدا(س): س + 1 → funcName = "__lambda_0"
                // ========================================================================
                {
                    auto aliasIt = lambdaAliases_.find(funcName);
                    if (aliasIt != lambdaAliases_.end())
                    {
                        std::string realLambdaName = aliasIt->second;
#ifdef SIR_BUILDER_DEBUG
                        std::cerr << "[LAMBDA] Resolved alias '" << funcName
                                  << "' -> '" << realLambdaName << "'" << std::endl;
#endif
                        funcName = realLambdaName;

                        // (AR) إضافة المتغيرات المُلتقطة كمعاملات إضافية مخفية
                        // (EN) Append captured variables as extra hidden arguments
                        auto capIt = closureCaptures_.find(realLambdaName);
                        if (capIt != closureCaptures_.end())
                        {
                            for (const auto &cap : capIt->second)
                            {
                                VariableInfo *capVar = lookupVariable(cap.varName);
                                if (capVar)
                                {
                                    // (AR) تحميل القيمة الحالية للمتغير المُلتقط
                                    // (EN) Load current value of captured variable
                                    std::string loadReg = newTempRegister();
                                    SIRInstruction loadInst;
                                    loadInst.opcode = SIROpcode::LOAD;
                                    loadInst.result = SIROperand::Register(loadReg, capVar->type);
                                    loadInst.operands.push_back(SIROperand::Register(capVar->registerName, capVar->type));
                                    if (currentBlock_)
                                        currentBlock_->addInstruction(loadInst);

                                    argOperands.push_back(SIROperand::Register(loadReg, capVar->type));
                                }
                                else
                                {
                                    // (AR) الرجوع إلى معلومات الالتقاط المخزنة
                                    // (EN) Fallback to stored capture info
                                    argOperands.push_back(SIROperand::Register(cap.registerName, cap.type));
                                }
                            }
#ifdef SIR_BUILDER_DEBUG
                            std::cerr << "[LAMBDA] Injected " << capIt->second.size()
                                      << " captured args for '" << realLambdaName << "'" << std::endl;
#endif
                        }

                        // ================================================================
                        // (AR) إضافة __env = 0 كمعامل أخير (دائماً)
                        //      جميع اللامدات تُعرّف بمعامل __env كمعامل أخير
                        //      حتى بدون التقاطات، يجب تمرير 0 كقيمة __env
                        //      بدون هذا: الدالة تُستدعى بعدد معاملات أقل مما تتوقعه
                        //      مما يؤدي إلى قراءة قيم trash من المكدس
                        // (EN) Append __env = 0 as last argument (always)
                        //      All lambdas are defined with __env as last parameter
                        //      Even without captures, must pass 0 as __env value
                        //      Without this: function called with fewer args than expected
                        //      leading to reading garbage values from stack
                        // ================================================================
                        argOperands.push_back(SIROperand::ConstantI64(0));
                    }
                }

                // ========================================================================
                // (AR) الخطوة 3: البحث عن الدالة والحصول على نوع الإرجاع
                // (EN) Step 3: Look up function and get return type
                // المصدر: sir_builder.h:719 - functionTable_
                // المصدر: sir_builder.h:162-175 - FunctionInfo struct
                // ========================================================================
                SadTypeKind returnType = SadTypeKind::Void; // (AR) افتراضياً void

                auto it = functionTable_.find(funcName);
                if (it != functionTable_.end())
                {
                    // (AR) الدالة موجودة - استخدم نوع الإرجاع (sir_builder.h:165)
                    // (EN) Function found - use return type
                    returnType = it->second.returnType;
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG] buildFunctionCall: found '" << funcName
                              << "' retType=" << static_cast<int>(returnType) << std::endl;
#endif

                    // ================================================================
                    // (AR) استنتاج أنواع المعاملات من موقع الاستدعاء
                    //      إذا كان المعامل I64 (افتراضي/غير محدد) والوسيط STRING/F64/BOOL
                    //      نحدّث نوع المعامل في functionTable_ فقط (ليس في SIRFunction المبنية)
                    //      لأن SIRFunction قد تكون بُنيت بالفعل بأنواع مستنتجة صحيحة
                    // (EN) Infer parameter types from call-site arguments
                    //      If param is I64 (default/unknown) and arg is STRING/F64/BOOL,
                    //      update param type in functionTable_ only (not in built SIRFunction)
                    //      because SIRFunction may already have correct inferred types
                    // ================================================================
                    auto &funcInfo = it->second;
                    for (size_t i = 0; i < argResults.size() && i < funcInfo.parameters.size(); i++)
                    {
                        SadTypeKind argType = argResults[i].type;
                        SadTypeKind &paramType = funcInfo.parameters[i].type;

                        if (paramType == SadTypeKind::Integer && argType == SadTypeKind::String)
                        {
                            // (AR) تحديث إلى STRING — المعامل يستقبل نصوصاً
                            // (EN) Update to STRING — parameter receives strings
                            paramType = SadTypeKind::String;
                        }
                    }

                    // ================================================================
                    // (AR) إصلاح جذري: تحويل الوسائط تلقائياً عند عدم تطابق النوع
                    //      لغة ص ديناميكية النوع — نفس الدالة قد تُستدعى بأنواع مختلفة
                    //      مثال: تحقق("نص", "نص") ثم تحقق(صحيح, صحيح)
                    //      المفسر يتعامل مع هذا تلقائياً عبر Value variant
                    //      المترجم يحتاج تحويلاً صريحاً: BOOL→STRING, I64→STRING
                    // (EN) Radical fix: auto-convert args when type doesn't match param
                    //      Sad language is dynamically typed — same function may be called
                    //      with different types. Interpreter handles this via Value variant.
                    //      Compiler needs explicit conversion: BOOL→STRING, I64→STRING.
                    // ================================================================
                    for (size_t i = 0; i < argOperands.size() && i < funcInfo.parameters.size(); i++)
                    {
                        SadTypeKind paramType = funcInfo.parameters[i].type;
                        SadTypeKind argType = argOperands[i].dataType;

                        if (paramType == SadTypeKind::String && argType != SadTypeKind::String)
                        {
                            // (AR) التحويل يعتمد على ما إذا كانت القيمة ثابتة أم سجل
                            // (EN) Conversion depends on whether the value is constant or register
                            if (argOperands[i].type == SIROperandType::CONSTANT)
                            {
                                // (AR) ثابت: نحوّل مباشرة إلى ConstantString
                                // (EN) Constant: convert directly to ConstantString
                                if (argType == SadTypeKind::Boolean)
                                {
                                    std::string strVal = argOperands[i].boolValue ? "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad" : // "صحيح"
                                                             "\xd8\xae\xd8\xb7\xd8\xa3";                                 // "خطأ"
                                    argOperands[i] = SIROperand::ConstantString(strVal);
                                }
                                else if (argType == SadTypeKind::Integer)
                                {
                                    argOperands[i] = SIROperand::ConstantString(
                                        std::to_string(argOperands[i].intValue));
                                }
                                else if (argType == SadTypeKind::Float)
                                {
                                    // (AR) استخدام snprintf مع %g لإنتاج تنسيق مطابق لما يُنتجه المترجم في وقت التشغيل
                                    //      مثال: 3.14 بدلاً من 3.140000 (الذي يُنتجه std::to_string)
                                    // (EN) Use snprintf with %g to produce format matching runtime output
                                    //      e.g., 3.14 instead of 3.140000 (which std::to_string produces)
                                    char buf[64];
                                    std::snprintf(buf, sizeof(buf), "%g", argOperands[i].floatValue);
                                    argOperands[i] = SIROperand::ConstantString(buf);
                                }
                            }
                            else
                            {
                                // (AR) سجل: نستخدم تعليمة تحويل SIR
                                // (EN) Register: use SIR conversion instruction
                                SIROpcode convOp;
                                if (argType == SadTypeKind::Boolean)
                                {
                                    convOp = SIROpcode::BOOL_TO_STRING;
                                }
                                else if (argType == SadTypeKind::Integer)
                                {
                                    convOp = SIROpcode::I64_TO_STRING;
                                }
                                else if (argType == SadTypeKind::Float)
                                {
                                    convOp = SIROpcode::F64_TO_STRING;
                                }
                                else
                                {
                                    continue; // (AR) لا يمكن تحويل هذا النوع
                                }
                                std::string strReg = newTempRegister();
                                SIRInstruction convInst;
                                convInst.opcode = convOp;
                                convInst.result = SIROperand::Register(strReg, SadTypeKind::String);
                                convInst.operands.push_back(argOperands[i]);
                                if (currentBlock_)
                                    currentBlock_->addInstruction(convInst);
                                argOperands[i] = SIROperand::Register(strReg, SadTypeKind::String);
                            }
                        }
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: function found, returnType="
                              << static_cast<int>(returnType) << std::endl;
#endif
                }
                else
                {
                    // (AR) الدالة غير موجودة - تحقق إذا كانت قالباً
                    // (EN) Function not found - check if it's a template
                    if (templateFunctions_.find(funcName) != templateFunctions_.end())
                    {
                        std::cout << "[Template] Found template function: " << funcName << std::endl;

                        // (AR) استنتاج الأنواع من المعاملات
                        // (EN) Infer types from arguments
                        std::vector<SadTypeKind> inferredTypes;
                        for (const auto &argResult : argResults)
                        {
                            inferredTypes.push_back(argResult.type);
                        }

                        // (AR) إنشاء نسخة من القالب مع الأنواع المستنتجة
                        // (EN) Instantiate template with inferred types
                        std::string instanceName = instantiateTemplate(funcName, inferredTypes);

                        if (instanceName.empty())
                        {
                            std::cerr << "[Template Error] Failed to instantiate template: " << funcName << std::endl;
                            errors_.push_back("Error: Failed to instantiate template '" + funcName + "'");
                            return BuildResult();
                        }

                        // (AR) استخدم اسم النسخة المُنشأة
                        // (EN) Use the instantiated function name
                        funcName = instanceName;

                        // (AR) ابحث عن نوع الإرجاع للنسخة المُنشأة
                        // (EN) Look up return type for instantiated function
                        auto instIt = functionTable_.find(funcName);
                        if (instIt != functionTable_.end())
                        {
                            returnType = instIt->second.returnType;
                        }
                        else
                        {
                            std::cerr << "[Template Error] Instantiated function not in function table: " << funcName << std::endl;
                        }

                        std::cout << "[Template] Using instantiated function: " << funcName << std::endl;
                    }
                    else
                    {
// (AR) قد تكون دالة مدمجة غير مسجلة بعد
// (EN) May be a builtin function not registered yet
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildFunctionCall: function '" << funcName
                                  << "' not found (no template), assuming VOID return" << std::endl;
#endif
                    }
                }

                // ========================================================================
                // (AR) الخطوة 3.5: فحص الاستدعاء عبر بنية إغلاق (Closure)
                //      إذا كان الاسم ليس في functionTable_ ولا lambdaAliases_ ولا templateFunctions_
                //      لكنه متغير معروف → يحمل مؤشر بنية إغلاق (closure struct)
                //      نُصدر CLOSURE_CALL الذي يستخرج fn_ptr و env_ptr من البنية
                //      ويستدعي fn_ptr(args..., env_ptr) — يعمل مع/بدون التقاطات
                // (EN) Step 3.5: Check for closure call via closure struct
                //      If name is not in functionTable_, lambdaAliases_, or templateFunctions_
                //      but IS a known variable → it holds a closure struct pointer
                //      Emit CLOSURE_CALL which extracts fn_ptr + env_ptr and calls fn(args, env)
                // ========================================================================
                if (functionTable_.find(funcName) == functionTable_.end() &&
                    lambdaAliases_.find(funcName) == lambdaAliases_.end() &&
                    templateFunctions_.find(funcName) == templateFunctions_.end())
                {
                    VariableInfo *varInfo = lookupVariable(funcName);
                    if (varInfo)
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildFunctionCall: '" << funcName
                                  << "' is a variable (closure call)" << std::endl;
#endif
                        // (AR) تحميل مؤشر بنية الإغلاق من المتغير
                        // (EN) Load the closure struct pointer from the variable
                        std::string loadReg = newTempRegister();
                        SIRInstruction loadInst;
                        loadInst.opcode = SIROpcode::LOAD;
                        loadInst.result = SIROperand::Register(loadReg, SadTypeKind::Function);
                        loadInst.operands.push_back(SIROperand::Register(varInfo->registerName, SadTypeKind::Function));
                        if (currentBlock_)
                            currentBlock_->addInstruction(loadInst);

                        // (AR) تحديد نوع إرجاع الإغلاق:
                        //      الإغلاقات ليست في functionTable_ لذا returnType قد يكون Void خطأً.
                        //      [إصلاح] نبحث أولاً عن اسم دالة اللامدا المرتبطة بالمتغير
                        //      ونأخذ نوع الإرجاع من functionTable_ — هذا يحافظ على Boolean
                        //      بدلاً من التراجع للافتراضي Integer.
                        //      مثال: تحقق = لامدا(س) => س > 10 → closureLambdaName = "__lambda_0"
                        //             → functionTable_["__lambda_0"].returnType = Boolean
                        //             → CLOSURE_CALL result.dataType = Boolean → يطبع صحيح/خطأ
                        // (EN) Determine closure return type:
                        //      [Fix] First look up the associated lambda function name from VariableInfo
                        //      and get return type from functionTable_ — preserves Boolean type
                        SadTypeKind closureRetType = returnType;
                        if (!varInfo->closureLambdaName.empty())
                        {
                            auto lambdaIt = functionTable_.find(varInfo->closureLambdaName);
                            if (lambdaIt != functionTable_.end())
                            {
                                closureRetType = lambdaIt->second.returnType;
                            }
                        }
                        if (closureRetType == SadTypeKind::Void)
                        {
                            closureRetType = SadTypeKind::Integer;
                        }

                        // (AR) إنشاء تعليمة CLOSURE_CALL
                        //      المعامل الأول = مؤشر بنية الإغلاق
                        //      الباقي = وسائط الاستدعاء الصريحة
                        //      CLOSURE_CALL يستخرج fn_ptr و env_ptr ويستدعي fn(args, env)
                        // (EN) Create CLOSURE_CALL instruction
                        //      First operand = closure struct pointer
                        //      Rest = explicit call arguments
                        //      CLOSURE_CALL extracts fn_ptr + env_ptr and calls fn(args, env)
                        std::string resultReg = newTempRegister();
                        SIRInstruction closureCallInst;
                        closureCallInst.opcode = SIROpcode::CLOSURE_CALL;
                        closureCallInst.result = SIROperand::Register(resultReg, closureRetType);
                        closureCallInst.operands.push_back(SIROperand::Register(loadReg, SadTypeKind::Function));
                        for (const auto &argOp : argOperands)
                        {
                            closureCallInst.operands.push_back(argOp);
                        }
                        // (AR) تمرير اسم دالة اللامدا في comment — يستخدمه emitClosureCall
                        //      للبحث عن التوقيع الحقيقي وتحويل أنواع الوسائط
                        // (EN) Pass lambda function name in comment — used by emitClosureCall
                        //      to look up real signature and convert argument types
                        if (!varInfo->closureLambdaName.empty())
                        {
                            closureCallInst.comment = "lambda:" + varInfo->closureLambdaName;
                        }
                        if (currentBlock_)
                            currentBlock_->addInstruction(closureCallInst);

                        BuildResult closureResult(resultReg, closureRetType);
                        closureResult.isDirectValue = true;
                        return closureResult;
                    }
                }

                // ========================================================================
                // (AR) الخطوة 4: إنشاء سجل النتيجة وتعليمة الاستدعاء
                // (EN) Step 4: Create result register and call instruction
                // المصدر: sir_instruction.h:231-238 - SIRInstruction::Call()
                // المصدر: sir_types.h:390-395 - SIROperand::Function()
                // ========================================================================
                std::string resultReg = newTempRegister();

                // (AR) إنشاء معامل الدالة (sir_types.h:390)
                // (EN) Create function operand
                SIROperand funcOp = SIROperand::Function(funcName);

                // (AR) إنشاء معامل النتيجة (sir_types.h:355)
                // (EN) Create result operand
                SIROperand resultOp = SIROperand::Register(resultReg, returnType);

                // (AR) إنشاء تعليمة الاستدعاء (sir_instruction.h:231)
                // (EN) Create call instruction
                // SIRInstruction::Call(result, function, args)
                SIRInstruction callInst = SIRInstruction::Call(resultOp, funcOp, argOperands);

                // ================================================================
                // (AR) تمرير اسم الصنف المُرجع إلى الباكند عبر حقل comment:
                //      إذا كانت الدالة تُرجع كائناً (لديها returnClassName في functionTable_)،
                //      نُخزّن اسم الصنف في comment بتنسيق "__return_class__:ClassName"
                //      حتى يتمكن emitCall في الباكند من تحديث objectClassMap.
                //      هذا ضروري لأن SIRInstruction لا يحمل metadata للأنواع المركبة.
                // (EN) Pass return class name to backend via comment field:
                //      If function returns an object (has returnClassName in functionTable_),
                //      store class name in comment as "__return_class__:ClassName"
                //      so emitCall in backend can update objectClassMap.
                // ================================================================
                if (it != functionTable_.end() && !it->second.returnClassName.empty())
                {
                    callInst.comment = "__return_class__:" + it->second.returnClassName;
                }

                // ========================================================================
                // (AR) الخطوة 5: إضافة التعليمة إلى الكتلة الحالية
                // (EN) Step 5: Add instruction to current block
                // المصدر: sir_builder.h:712 - currentBlock_
                // ========================================================================
                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(callInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: added CALL instruction to block" << std::endl;
#endif
                }
                else
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: WARNING - no current block!" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 6: إذا كانت الدالة مولّد، أضف GENERATOR_CONSUME لجمع القيم
                // (EN) Step 6: If callee is a generator, add GENERATOR_CONSUME to collect values
                // ========================================================================
                if (it != functionTable_.end() && it->second.isGenerator)
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[GEN] Emitting GENERATOR_CONSUME for generator '" << funcName << "'" << std::endl;
#endif

                    // (AR) CALL أعاد المقبض (PTR) — الآن نستهلكه
                    // (EN) CALL returned the handle (PTR) — now consume it
                    std::string consumeReg = newTempRegister();
                    SIRInstruction consumeInst;
                    consumeInst.opcode = SIROpcode::GENERATOR_CONSUME;
                    consumeInst.result = SIROperand::Register(consumeReg, SadTypeKind::Integer);
                    consumeInst.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Pointer));
                    consumeInst.comment = "consume generator yields";

                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(consumeInst);
                    }

                    return BuildResult(consumeReg, SadTypeKind::Integer);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildFunctionCall: returning result reg='" << resultReg
                          << "', type=" << static_cast<int>(returnType) << std::endl;
#endif

                // ================================================================
                // (AR) تتبع نوع الصنف المُرجع من الدالة:
                //      إذا كانت الدالة مُسجّلة في functionTable_ وتحمل returnClassName،
                //      ننقل هذه المعلومة إلى BuildResult حتى يتمكن buildLocalVariable
                //      من تسجيل المتغير في classInstanceTypes_.
                //      مثال: متغير ن = اصنع_نقطة() → result.className = "نقطة"
                //      هذا يحل مشكلة: الوصول لحقول كائن مُرجع من دالة كان يتعطل (segfault)
                //      لأن المترجم لم يكن يعرف أن المتغير يحمل كائناً
                // (EN) Track return class type from function:
                //      If function is in functionTable_ and has returnClassName,
                //      transfer this info to BuildResult so buildLocalVariable can
                //      register the variable in classInstanceTypes_.
                //      This fixes: accessing fields of object returned from function was crashing
                //      because compiler didn't know the variable holds an object
                // ================================================================
                BuildResult result(resultReg, returnType);
                if (it != functionTable_.end() && !it->second.returnClassName.empty())
                {
                    result.className = it->second.returnClassName;
                    // (AR) سجّل أيضاً في classInstanceTypes_ بالسجل المؤقت
                    //      حتى لو لم يُخزّن في متغير مباشرة
                    // (EN) Also register in classInstanceTypes_ by temp register
                    //      even if not stored in variable directly
                    classInstanceTypes_[resultReg] = it->second.returnClassName;
                }
                return result;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
