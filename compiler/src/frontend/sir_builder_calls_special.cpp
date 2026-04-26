// ============================================================================
// sir_builder_calls_special.cpp — استدعاءات خاصة: callable object + super constructor
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 2026
// الغرض / Purpose:
//   (AR) مُستخرج من sir_builder_calls.cpp وفق قاعدة CW-05 (≤800 سطر)
//        يحتوي على حالتين خاصتين من buildFunctionCall:
//        1. استدعاء عامل () على كائن — buildCallableObjectInvoke
//        2. استدعاء باني الصنف الأب — buildSuperConstructorCall
//
//   (EN) Extracted from sir_builder_calls.cpp per CW-05 (≤800 lines)
//        Contains two special dispatch cases from buildFunctionCall:
//        1. Operator () call on an object — buildCallableObjectInvoke
//        2. Parent class constructor call — buildSuperConstructorCall
//
// الملفات المرتبطة / Related files:
//   - sir_builder_calls.cpp        — الدالة الرئيسية buildFunctionCall
//   - sir_builder_calls_macro.cpp  — معالجة الماكرو
//   - sir_builder_calls_args.cpp   — بناء قائمة الوسائط
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
            // buildCallableObjectInvoke — استدعاء الكائن القابل للاستدعاء operator()
            // ============================================================================
            // (AR) يُعالج حالة: ض(7) حيث ض كائن من صنف يحتوي عامل الاستدعاء الزائد
            //      يبحث عن __op_call__ في سلسلة الوراثة ويُصدر OBJECT_CALL
            //
            // (EN) Handles: ض(7) where ض is an object with call operator overload
            //      Searches for __op_call__ in inheritance chain and emits OBJECT_CALL
            //
            // الإرجاع / Return:
            //   nullopt = ليس كائناً قابلاً للاستدعاء، يتابع المستدعي
            //   some(result) = تم الاستدعاء، يجب الإرجاع الفوري بهذه النتيجة
            // ============================================================================
            std::optional<BuildResult> SIRBuilder::buildCallableObjectInvoke(
                AST::FunctionCallNode *call, const std::string &funcName)
            {
                // (AR) البحث عن اسم الدالة في جدول نوع كائنات الصنف
                // (EN) Search for function name in class instance types table
                auto classIt = classInstanceTypes_.find(funcName);
                if (classIt == classInstanceTypes_.end())
                    return std::nullopt;

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

                // (AR) لم يوجد عامل الاستدعاء → ليس callable object
                // (EN) Call operator not found → not a callable object
                if (!found)
                    return std::nullopt;

                // ====================================================================
                // (AR) بناء المعاملات للاستدعاء
                // (EN) Build call arguments
                // ====================================================================
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

                // ====================================================================
                // (AR) إصدار OBJECT_CALL — استدعاء العامل على الكائن
                // (EN) Emit OBJECT_CALL — invoke operator on object
                // ====================================================================
                std::string resultReg = newTempRegister();
                auto &opInfo = functionTable_[fullOpName];
                SadTypeKind returnType = opInfo.returnType;

                if (currentBlock_)
                {
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::OBJECT_CALL;
                    callInst.result = SIROperand::Register(resultReg, returnType);

                    // (AR) البحث عن سجل الكائن من جدول المتغيرات
                    // (EN) Look up object register from variable table
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

            // ============================================================================
            // buildSuperConstructorCall — استدعاء باني الصنف الأب
            // ============================================================================
            // (AR) يُعالج استدعاءات: أساس(...) / الأساس(...) / اساس(...) / super(...)
            //      يبحث عن الصنف الأب في الوحدة الحالية ويُصدر CALL لبانيه
            //      يبدأ بإضافة self كأول وسيط ثم يتبعه وسائط المستدعي
            //
            // (EN) Handles: أساس(...) / الأساس(...) / اساس(...) / super(...)
            //      Looks up parent class in current module and emits CALL to its constructor
            //      Prepends self as first argument then caller arguments
            //
            // الإرجاع / Return:
            //   nullopt = ليس استدعاء باني الأب، يتابع المستدعي
            //   some(result) = تم استدعاء الباني، يجب الإرجاع الفوري بهذه النتيجة
            // ============================================================================
            std::optional<BuildResult> SIRBuilder::buildSuperConstructorCall(
                AST::FunctionCallNode *call, const std::string &funcName)
            {
                // (AR) التحقق: هل هو استدعاء باني الأب وهل نحن داخل صنف؟
                // (EN) Check: is it a super constructor call and are we inside a class?
                bool isSuperCall =
                    (funcName == "\xD8\xA3\xD8\xB3\xD8\xA7\xD8\xB3" ||                 // أساس
                     funcName == "\xD8\xA7\xD9\x84\xD8\xA3\xD8\xB3\xD8\xA7\xD8\xB3" || // الأساس
                     funcName == "\xD8\xA7\xD8\xB3\xD8\xA7\xD8\xB3" ||                 // اساس
                     funcName == "\xD8\xA7\xD9\x84\xD8\xA7\xD8\xB3\xD8\xA7\xD8\xB3" || // الاساس
                     funcName == "super") &&
                    !currentClassName_.empty();

                if (!isSuperCall)
                    return std::nullopt;

                // ====================================================================
                // (AR) البحث عن الصنف الأب وإصدار CALL لبانيه
                // (EN) Find parent class and emit CALL to its constructor
                // ====================================================================
                auto sirClass = module_->getClass(currentClassName_);
                if (sirClass && !sirClass->parentClass.empty())
                {
                    // (AR) اسم باني الأب: اسم_الصنف.بناء
                    // (EN) Parent constructor name: ClassName.بناء
                    std::string parentCtorName = sirClass->parentClass + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // بناء

                    // (AR) بناء الوسائط: self أولاً ثم وسائط المستدعي
                    // (EN) Build arguments: self first then caller arguments
                    std::vector<SIROperand> superArgs;
                    superArgs.push_back(SIROperand::Register(kSelfRegisterName, SadTypeKind::Integer));

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

                // (AR) لا يوجد صنف أب — نتجاهل ونُرجع نتيجة فارغة
                // (EN) No parent class — ignore and return empty result
                return BuildResult();
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
