// ============================================================================
// sir_builder_calls_macro.cpp — معالجة توسيع استدعاء الماكرو
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 2026
// الغرض / Purpose:
//   (AR) مُستخرج من sir_builder_calls.cpp وفق قاعدة CW-05 (≤800 سطر)
//        يحتوي على دالة واحدة مسؤولة عن توسيع استدعاءات الماكرو من الشكل:
//        اسم!(وسائط)
//   (EN) Extracted from sir_builder_calls.cpp per CW-05 (≤800 lines)
//        Contains a single function responsible for macro call expansion:
//        name!(arguments)
//
// المسؤولية / Responsibility:
//   - SIRBuilder::buildMacroCallExpansion — مسار inline + مسار الدالة المؤقتة
//
// الملفات المرتبطة / Related files:
//   - sir_builder_calls.cpp       — الدالة الرئيسية buildFunctionCall
//   - sir_builder_calls_objects.cpp — callable object + super constructor
//   - sir_builder_calls_args.cpp    — بناء قائمة الوسائط
//   - compiler/include/frontend/sir_builder.h — التصريحات
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
            // buildMacroCallExpansion — توسيع استدعاء الماكرو
            // ============================================================================
            // (AR) مسؤولية: معالجة استدعاء من شكل اسم!(وسائط)
            //      مسار 1 (inline): ماكرو بدون ارجع وبدون variadic — يُوسّع مباشرة في المكان
            //      مسار 2 (temp func): ماكرو متغير أو يحتوي ارجع — يُحوَّل لدالة SIR مؤقتة
            //
            // (EN) Responsibility: handle macro call of form name!(args)
            //      Path 1 (inline): non-return non-variadic macro — expands directly in place
            //      Path 2 (temp func): variadic or return macro — converted to temporary SIR function
            //
            // الإرجاع / Return:
            //   nullopt = ليس استدعاء ماكرو، يتابع buildFunctionCall المعالجة
            //   some(result) = كان ماكرو، يجب الإرجاع الفوري بهذه النتيجة
            // ============================================================================
            std::optional<BuildResult> SIRBuilder::buildMacroCallExpansion(
                AST::FunctionCallNode *call, const std::string &funcName)
            {
                // (AR) إذا لم يكن استدعاء ماكرو → لا شيء يُعالج، يتابع المستدعي
                // (EN) If not a macro call → nothing to handle, let caller continue
                if (!call->isMacroCall)
                    return std::nullopt;

                // ====================================================================
                // (AR) البحث عن تعريف الماكرو في جدول الماكروز
                // (EN) Look up macro definition in macro table
                // ====================================================================
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

                // ====================================================================
                // (AR) الخطوة 1: التحقق من عدد الوسائط
                // (EN) Step 1: Validate argument count
                // ====================================================================
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

                // ====================================================================
                // (AR) مسار التوسيع المباشر (inline expansion)
                //      للماكروز بدون ارجع وبدون variadic فقط
                //      يُوسّع الجسم مباشرة في المكان مع ربط أسماء المعاملات
                //      بعناوين المتغيرات الأصلية — يدعم النسخ للخلف (copy-back)
                // (EN) Inline expansion path
                //      For macros without return and without variadic only
                //      Expands body directly in-place, binding parameter names
                //      to original variable allocas — supports copy-back automatically
                // ====================================================================
                bool useInlineExpansion = !macroDef->isVariadic && macroDef->body &&
                                          !hasReturnWithValue(macroDef->body.get());

                if (useInlineExpansion)
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Macro '" << funcName << "' using inline expansion (copy-back)" << std::endl;
#endif
                    enterScope();

                    for (size_t i = 0; i < requiredParams; ++i)
                    {
                        auto *argExpr = call->arguments[i].get();
                        {
                            auto *identExpr = dynamic_cast<Sad::AST::VariableExpr *>(argExpr);
                            if (identExpr)
                            {
                                // (AR) الوسيط identifier — نربط اسم المعامل بعنوان المتغير الأصلي
                                // (EN) Argument is identifier — bind param name to original variable alloca
                                auto *origVar = lookupVariable(identExpr->name);
                                if (origVar)
                                {
                                    VariableInfo vi;
                                    vi.name = macroDef->params[i];
                                    vi.type = origVar->type;
                                    vi.registerName = origVar->registerName;
                                    vi.isMutable = true;
                                    vi.isParameter = false;
                                    vi.scopeLevel = currentScopeLevel_;
                                    vi.elementType = origVar->elementType;
                                    addVariable(vi);
                                    continue;
                                }
                            }
                        }

                        // (AR) الوسيط تعبير أو متغير غير موجود — ننشئ alloca محلي
                        // (EN) Argument is expression or unknown variable — create local alloca
                        auto argResult = buildExpression(argExpr);

                        std::string allocName = "%" + macroDef->params[i] + "_inline";
                        SIRInstruction allocInst(SIROpcode::ALLOC);
                        allocInst.result = SIROperand::Register(allocName, argResult.type);
                        if (currentBlock_)
                            currentBlock_->instructions.push_back(allocInst);

                        // (AR) تخزين القيمة في alloca
                        // (EN) Store value in alloca
                        SIRInstruction storeInst(SIROpcode::STORE);
                        if (argResult.isConstant && !argResult.constantValue.empty())
                        {
                            if (argResult.type == SadTypeKind::Integer)
                                storeInst.operands.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                            else if (argResult.type == SadTypeKind::String)
                                storeInst.operands.push_back(SIROperand::ConstantString(argResult.constantValue));
                            else if (argResult.type == SadTypeKind::Float)
                                storeInst.operands.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                            else
                                storeInst.operands.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                        }
                        else
                        {
                            storeInst.operands.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                        }
                        storeInst.operands.push_back(SIROperand::Register(allocName, argResult.type));
                        if (currentBlock_)
                            currentBlock_->instructions.push_back(storeInst);

                        VariableInfo vi;
                        vi.name = macroDef->params[i];
                        vi.type = argResult.type;
                        vi.registerName = allocName;
                        vi.isMutable = true;
                        vi.isParameter = false;
                        vi.scopeLevel = currentScopeLevel_;
                        addVariable(vi);
                    }

                    // (AR) بناء جسم الماكرو مباشرة في الدالة الحالية
                    // (EN) Build macro body directly in current function
                    buildStatement(macroDef->body.get());

                    exitScope();

                    return BuildResult("", SadTypeKind::Void);
                }

                // ====================================================================
                // (AR) مسار الدالة المؤقتة — للماكروز المتغيرة أو التي تحتوي ارجع
                // (EN) Temporary function path — for variadic macros or those with return
                // ====================================================================

                // (AR) الخطوة 2: بناء الوسائط في النطاق الحالي (قبل النطاق المعزول)
                // (EN) Step 2: Build arguments in current scope (before isolated scope)
                std::vector<BuildResult> argResults;
                for (size_t i = 0; i < call->arguments.size(); ++i)
                {
                    argResults.push_back(buildExpression(call->arguments[i].get()));
                }

                // (AR) الخطوة 3: إنشاء دالة SIR مؤقتة للماكرو باسم فريد __macro_اسم_N
                // (EN) Step 3: Create temporary SIR function with unique name __macro_name_N
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
                    macroFunc->addParameter(SIRParameter(macroDef->params.back(), SadTypeKind::Array));
                }

                // (AR) الخطوة 5: حفظ السياق وبناء جسم الماكرو في الدالة المؤقتة
                // (EN) Step 5: Save context and build macro body in temporary function
                auto savedCtx = saveContext();

                currentFunction_ = macroFunc;
                auto entryBlock = createBasicBlock("macro_entry");
                macroFunc->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;

                enterScope();

                // (AR) ربط المعاملات العادية
                // (EN) Bind regular parameters
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

                // (AR) المعامل المتغير — نحفظ نوع العنصر من أول وسيط متغير
                // (EN) Variadic parameter — save element type from first variadic arg
                if (macroDef->isVariadic && !macroDef->params.empty())
                {
                    VariableInfo vi;
                    vi.name = macroDef->params.back();
                    vi.type = SadTypeKind::Array;
                    vi.registerName = "%" + macroDef->params.back();
                    vi.isMutable = true;
                    vi.isParameter = true;
                    vi.scopeLevel = currentScopeLevel_;
                    if (argResults.size() > requiredParams)
                    {
                        vi.elementType = argResults[requiredParams].type;
                    }
                    addVariable(vi);
                }

                // (AR) بناء جسم الماكرو
                // (EN) Build macro body
                if (macroDef->body)
                {
                    buildStatement(macroDef->body.get());
                }

                // (AR) إضافة ReturnVoid إذا لم يكن هناك terminator
                // (EN) Add ReturnVoid if no terminator
                if (currentBlock_ && !currentBlock_->getTerminator())
                {
                    currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
                }

                exitScope();

                // (AR) إضافة الدالة المؤقتة إلى الوحدة وتسجيلها في جدول الدوال
                // (EN) Add temporary function to module and register in function table
                module_->addFunction(macroFunc);

                FunctionInfo fi;
                fi.name = macroFuncName;
                fi.returnType = macroRetType;
                fi.sirFunction = macroFunc;
                functionTable_[macroFuncName] = fi;

                // (AR) الخطوة 6: استعادة السياق السابق
                // (EN) Step 6: Restore previous context
                restoreContext(std::move(savedCtx));

                // ====================================================================
                // (AR) الخطوة 7: إصدار استدعاء الدالة المؤقتة
                // (EN) Step 7: Emit call to temporary function
                // ====================================================================
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

                // (AR) الوسائط المتغيرة: تُجمع في مصفوفة SadArray ثم تُمرر كوسيط واحد
                // (EN) Variadic arguments: collected into SadArray then passed as single argument
                if (macroDef->isVariadic)
                {
                    size_t variadicCount = argResults.size() - requiredParams;

                    // (AR) إنشاء مصفوفة جديدة بحجم الوسائط المتغيرة
                    // (EN) Create new array with variadic count
                    std::string arrReg = newTempRegister();
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ARRAY_NEW;
                    allocInst.result = SIROperand::Register(arrReg, SadTypeKind::Array);
                    allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(variadicCount)));
                    allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(variadicCount)));
                    allocInst.comment = "variadic args array [" + std::to_string(variadicCount) + "]";
                    if (currentBlock_)
                        currentBlock_->addInstruction(allocInst);

                    // (AR) تخزين كل وسيط في المصفوفة
                    // (EN) Store each variadic arg in array
                    for (size_t i = requiredParams; i < argResults.size(); ++i)
                    {
                        auto &ar = argResults[i];

                        // (AR) تجسيد الثوابت قبل التخزين
                        // (EN) Materialize constants before storing
                        if (ar.isConstant && ar.type != SadTypeKind::Array && currentBlock_)
                        {
                            std::string reg = newTempRegister();
                            SIRInstruction moveInst(SIROpcode::MOVE);
                            moveInst.result = SIROperand::Register(reg, ar.type);
                            if (ar.type == SadTypeKind::String)
                                moveInst.operands.push_back(SIROperand::ConstantString(ar.constantValue));
                            else if (ar.type == SadTypeKind::Float)
                                moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(ar.constantValue)));
                            else
                            {
                                try
                                {
                                    moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(ar.constantValue)));
                                }
                                catch (...)
                                {
                                    moveInst.operands.push_back(SIROperand::ConstantI64(0));
                                }
                            }
                            currentBlock_->addInstruction(moveInst);
                            ar.registerName = reg;
                            ar.isConstant = false;
                        }

                        SIRInstruction storeInst;
                        storeInst.opcode = SIROpcode::ARRAY_SET;
                        storeInst.operands.push_back(SIROperand::Register(arrReg, SadTypeKind::Array));
                        storeInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i - requiredParams)));
                        storeInst.operands.push_back(SIROperand::Register(ar.registerName, ar.type));
                        storeInst.comment = "variadic[" + std::to_string(i - requiredParams) + "]";
                        if (currentBlock_)
                            currentBlock_->addInstruction(storeInst);
                    }

                    // (AR) تمرير المصفوفة كوسيط واحد
                    // (EN) Pass array as single argument
                    callArgs.push_back(SIROperand::Register(arrReg, SadTypeKind::Array));
                }

                // (AR) إصدار تعليمة CALL للدالة المؤقتة
                // (EN) Emit CALL instruction for temporary function
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
                else
                {
                    errors_.push_back("Error: currentBlock_ is null during macro call emission");
                }

                return BuildResult(resultReg, macroRetType);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
