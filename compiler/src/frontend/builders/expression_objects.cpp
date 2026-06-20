// ============================================================================
// sir_builder_calls_objects.cpp - ???? ?????? ???????? (?????? ????? ??????? ?????)
// ============================================================================
// ?????? / Author: Sad Compiler Team
// ????? / Description:
//   ??? ????? ????? ?????? ???? SIR ???????? ?????????:
//   - buildNewObject: ????? ???? ???? (???? ???_?????(...))
//   - buildMemberAccess: ?????? ????? ?????? (????.???)
//   - b_.buildMethodCall: ??????? ????? ??? ???? (????.?????(...))
//
//   ?? ??? ??? ?????? ?? sir_builder_calls.cpp ???? ????? ??:
//   - b_.buildFunctionCall: ??????? ???? ????? ?????? ??????
// ============================================================================

#include <string>
#include <cstdio>
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // ============================================================================
            // buildNewObject - ???? ????? ????? ???? ????
            // ============================================================================
            // ???? ??????? / Source: class_nodes.h:164
            // ??????? / Signature: BuildResult buildNewObject(AST::NewExpr* newExpr);
            //
            // ????????? / Parameters:
            // - newExpr: AST::NewExpr* (class_nodes.h:164)
            //
            // NewExpr Members:
            // - className: std::string (line 167)
            // - arguments: std::vector<std::unique_ptr<Expr>> (line 168)
            // ============================================================================
            BuildResult ExpressionBuilder::buildNewObject(AST::NewExpr *newExpr)
            {
                if (!newExpr)
                {
                    return BuildResult();
                }

                // (AR) ?????? 1: ????? ?? ????? ?? ??????
                // (EN) Step 1: Find class in module
                auto sirClass = b_.module_->getClass(newExpr->className);
                if (!sirClass)
                {
                    b_.errors_.push_back("Class not found: " + newExpr->className);
                    return BuildResult();
                }

                // (AR) ?????? 2: ????? ????? ??????
                // (EN) Step 2: Allocate memory for object
                std::string objReg = b_.newTempRegister();

                if (b_.currentBlock_)
                {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register(objReg, SadTypeKind::Integer);
                    // (AR) ????? ??? ????? ?? metadata
                    // (EN) Add class name as metadata
                    allocInst.operands.push_back(SIROperand::ConstantString(newExpr->className));
                    b_.currentBlock_->addInstruction(allocInst);
                }

                // (AR) ?????? 3: ??????? ???? ?????? (constructor) ?? ????
                // (EN) Step 3: Call constructor if exists
                std::string constructorName = newExpr->className + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // ????
                auto constructor = sirClass->getMethod(constructorName);

                if (constructor || !newExpr->arguments.empty())
                {
                    // (AR) ???? ??????? ??????
                    // (EN) Build constructor arguments
                    std::vector<SIROperand> args;

                    // (AR) ??????? ????? ?? ?????? ???? (self)
                    // (EN) First argument is the object itself (self)
                    args.push_back(SIROperand::Register(objReg, SadTypeKind::Integer));

                    // (AR) أسماء أصناف الوسائط (لتتبّع أنواع الحقول الكائنيّة)
                    // (EN) Argument class names (to track object-typed field classes)
                    std::vector<std::string> argClassNames;
                    argClassNames.push_back(""); // self

                    // (AR) ???? ?????????
                    // (EN) Rest of arguments
                    for (const auto &arg : newExpr->arguments)
                    {
                        auto argResult = buildExpression(arg.get());
                        argClassNames.push_back(argResult.className);
                        if (argResult.isConstant && !argResult.constantValue.empty())
                        {
                            // ????? ?????? ??? ?????
                            switch (argResult.type)
                            {
                            case SadTypeKind::Integer:
                                args.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                                break;
                            case SadTypeKind::Float:
                                args.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                                break;
                            case SadTypeKind::String:
                                args.push_back(SIROperand::ConstantString(argResult.constantValue));
                                break;
                            default:
                                args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                                break;
                            }
                        }
                        else
                        {
                            args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                        }
                    }

                    // (AR) ????? ?????? ??????? ??????
                    // (EN) Create constructor call instruction
                    if (b_.currentBlock_)
                    {
                        SIRInstruction callInst;
                        callInst.opcode = SIROpcode::CALL;
                        callInst.result = SIROperand::Register(b_.newTempRegister(), SadTypeKind::Void);
                        callInst.operands.push_back(SIROperand::Function(constructorName));
                        for (const auto &arg : args)
                        {
                            callInst.operands.push_back(arg);
                        }
                        b_.currentBlock_->addInstruction(callInst);
                    }

                    // ???????????????????????????????????????????????????????????????
                    // (AR) ??????? ????? ?????? ?? ????? ??????? ???????
                    // (EN) Infer field types from actual argument types at call site
                    // ???????????????????????????????????????????????????????????????
                    if (constructor)
                    {
                        const auto &params = constructor->getParameters();
                        // params[0] = self, params[1..N] = user params
                        // args[0] = self, args[1..N] = user args

                        // (AR) ???? ?????: ???_??????? ? ???_??????
                        // (EN) Build map: paramName ? argType
                        std::unordered_map<std::string, SadTypeKind> paramTypes;
                        std::unordered_map<std::string, std::string> paramClassNames;
                        for (size_t i = 1; i < params.size() && i < args.size(); i++)
                        {
                            paramTypes[params[i].name] = args[i].dataType;
                            if (i < argClassNames.size() && !argClassNames[i].empty())
                                paramClassNames[params[i].name] = argClassNames[i];
                        }

                        // (AR) ????? ???? ????? ??????
                        // (EN) Update current class fields
                        if (!sirClass->paramToFieldMap_.empty())
                        {
                            for (auto &[paramName, argType] : paramTypes)
                            {
                                auto fieldIt = sirClass->paramToFieldMap_.find(paramName);
                                if (fieldIt != sirClass->paramToFieldMap_.end())
                                {
                                    const std::string &fieldName = fieldIt->second;
                                    // (AR) سجّل اسم صنف الحقل الكائنيّ إن وُجد (لتمكين الوصول المتسلسل)
                                    // (EN) Record object field's class name if any (enables chained access)
                                    auto pcnIt = paramClassNames.find(paramName);
                                    if (pcnIt != paramClassNames.end())
                                    {
                                        sirClass->fieldClassNames_[fieldName] = pcnIt->second;
                                    }
                                    auto currentType = sirClass->fields_.find(fieldName);
                                    if (currentType != sirClass->fields_.end() &&
                                        currentType->second == SadTypeKind::Pointer &&
                                        argType != SadTypeKind::Pointer && argType != SadTypeKind::Void)
                                    {
                                        sirClass->fields_[fieldName] = argType;
#ifndef NDEBUG
                                        std::cout << "[DEBUG] buildNewObject: inferred field '" << fieldName
                                                  << "' type=" << static_cast<int>(argType)
                                                  << " from arg '" << paramName << "'" << std::endl;
#endif
                                    }
                                }
                            }
                        }

                        // ???????????????????????????????????????????????????????????????
                        // (AR) ??? ????? ??????? ??? ????? ??????? (super constructor chain)
                        // (EN) Propagate arg types through inheritance chain (super ctor chain)
                        // ???????????????????????????????????????????????????????????????
                        auto currentClass = sirClass;
                        auto currentParamTypes = paramTypes;

                        while (currentClass && !currentClass->parentClass.empty() &&
                               !currentClass->superParamMapping_.empty())
                        {
                            auto parentSirClass = b_.module_->getClass(currentClass->parentClass);
                            if (!parentSirClass)
                                break;

                            // (AR) ?????? ??? ??????? ???? ????
                            // (EN) Get parent constructor params
                            std::string parentCtorName = currentClass->parentClass + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1";
                            auto parentCtor = parentSirClass->getMethod(parentCtorName);
                            if (!parentCtor)
                                break;

                            const auto &parentParams = parentCtor->getParameters();

                            // (AR) ???? ????? ????? ??????? ???? ?? superParamMapping
                            // (EN) Build parent param types from superParamMapping
                            std::unordered_map<std::string, SadTypeKind> parentParamTypes;
                            for (auto &[superIdx, childParamName] : currentClass->superParamMapping_)
                            {
                                int parentIdx = superIdx + 1; // +1 (skip self)
                                if (parentIdx < Sad::Security::SafeArithmetic::assertSafeCast<int>(parentParams.size(), "expression_objects_size"))
                                {
                                    auto it = currentParamTypes.find(childParamName);
                                    if (it != currentParamTypes.end())
                                    {
                                        parentParamTypes[parentParams[parentIdx].name] = it->second;
                                    }
                                }
                            }

                            // (AR) ????? ???? ???? ???????? paramToFieldMap
                            // (EN) Update parent fields using paramToFieldMap
                            for (auto &[parentParamName, inferredType] : parentParamTypes)
                            {
                                auto fieldIt = parentSirClass->paramToFieldMap_.find(parentParamName);
                                if (fieldIt != parentSirClass->paramToFieldMap_.end())
                                {
                                    const std::string &fieldName = fieldIt->second;
                                    auto currentFldType = parentSirClass->fields_.find(fieldName);
                                    if (currentFldType != parentSirClass->fields_.end() &&
                                        currentFldType->second == SadTypeKind::Pointer &&
                                        inferredType != SadTypeKind::Pointer && inferredType != SadTypeKind::Void)
                                    {
                                        parentSirClass->fields_[fieldName] = inferredType;
                                        // (AR) ????? ????? ?? ??????? (?????? ????????)
                                        // (EN) Also update in children (inherited fields)
                                        if (sirClass->fields_.count(fieldName))
                                        {
                                            sirClass->fields_[fieldName] = inferredType;
                                        }
                                        if (currentClass->fields_.count(fieldName))
                                        {
                                            currentClass->fields_[fieldName] = inferredType;
                                        }
#ifndef NDEBUG
                                        std::cout << "[DEBUG] buildNewObject: propagated field '" << fieldName
                                                  << "' type=" << static_cast<int>(inferredType)
                                                  << " to parent '" << parentSirClass->name << "'" << std::endl;
#endif
                                    }
                                }
                            }

                            // (AR) ????? ???? ??????
                            // (EN) Move to next parent
                            currentClass = parentSirClass;
                            currentParamTypes = parentParamTypes;
                        }
                    }
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildNewObject: object created in register '" << objReg << "'" << std::endl;
#endif

                // (AR) ???? ??? ?????? ???? ????? ????????? ??????
                // (EN) Track object type for operator overloading support
                b_.classInstanceTypes_[objReg] = newExpr->className;

                // (AR) ????? ???? ?????? ?? ??? ????? � ??? STRUCT ???? I64
                // (EN) Return pointer to object with class name � STRUCT type not I64
                BuildResult result(objReg, SadTypeKind::Struct);
                result.className = newExpr->className;
                return result;
            }

            // ============================================================================
            // buildMemberAccess - ???? ????? ?????? ???? ?? ????
            // ============================================================================
            // ???? ??????? / Source: class_nodes.h:206
            // ??????? / Signature: BuildResult buildMemberAccess(AST::MemberAccessExpr* memberExpr);
            //
            // ????????? / Parameters:
            // - memberExpr: AST::MemberAccessExpr* (class_nodes.h:206)
            //
            // MemberAccessExpr Members:
            // - object: std::unique_ptr<Expr> (line 209)
            // - memberName: std::string (line 210)
            // ============================================================================
            BuildResult ExpressionBuilder::buildMemberAccess(AST::MemberAccessExpr *memberExpr)
            {
                if (!memberExpr)
                {
                    return BuildResult();
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMemberAccess: accessing member '"
                          << memberExpr->memberName << "'" << std::endl;
#endif

                // ================================================================
                // (AR) ??? ????: ?????? ???? ???? ??? ??? ?????
                //      ????: ????.?????? � "????" ??? ??? ???? ?????
                //      ???? ?? b_.staticFields_ ?? "????.??????" ?????? ???
                // (EN) Early check: static field access via class name
                //      Example: Counter.value � "Counter" is class name, not variable
                //      Look up "Counter.value" in b_.staticFields_ as global variable
                // ================================================================
                if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(memberExpr->object.get()))
                {
                    // ================================================================
                    // (AR) فحص مبكر: وصول عضو فضاء أسماء (namespace.member)
                    //      مثال: رياضيات.PI → تحميل المتغير العام رياضيات::PI
                    // (EN) Early check: namespace member access (namespace.member)
                    //      Example: math.PI → load global math::PI
                    // ================================================================
                    auto nsIt = b_.namespaceMembers_.find(varExpr->name);
                    if (nsIt != b_.namespaceMembers_.end())
                    {
                        auto memIt = nsIt->second.find(memberExpr->memberName);
                        if (memIt != nsIt->second.end())
                        {
                            const auto &nsInfo = memIt->second;
                            if (nsInfo.kind == "var")
                            {
                                // (AR) متغير فضاء → تحميل من المتغير العام
                                // (EN) Namespace variable → load from global
                                std::string loadReg = b_.newTempRegister();
                                if (b_.currentBlock_)
                                {
                                    SIRInstruction loadInst(SIROpcode::LOAD);
                                    loadInst.result = SIROperand::Register(loadReg, nsInfo.type);
                                    loadInst.operands.push_back(
                                        SIROperand::Global(nsInfo.sirName, nsInfo.type));
                                    loadInst.comment = "Load namespace member: " + nsInfo.sirName;
                                    b_.currentBlock_->addInstruction(loadInst);
                                }
                                return BuildResult(loadReg, nsInfo.type);
                            }
                            else if (nsInfo.kind == "func")
                            {
                                // (AR) دالة فضاء → نُرجع مرجع الدالة بالاسم الكامل
                                //      الاستدعاء الفعلي يتم في b_.buildMethodCall أو buildCallExpr
                                // (EN) Namespace function → return function reference with full name
                                //      Actual call happens in b_.buildMethodCall or buildCallExpr
                                return BuildResult(nsInfo.sirName, SadTypeKind::Function);
                            }
                        }
                    }

                    std::string staticFieldName = varExpr->name + "." + memberExpr->memberName;
                    auto sfIt = b_.staticFields_.find(staticFieldName);
                    if (sfIt != b_.staticFields_.end())
                    {
                        // (AR) ??? ???? � ????? ?? ????? ???
                        // (EN) Static field � load from global variable
                        std::string loadReg = b_.newTempRegister();
                        if (b_.currentBlock_)
                        {
                            SIRInstruction loadInst(SIROpcode::LOAD);
                            loadInst.result = SIROperand::Register(loadReg, sfIt->second);
                            loadInst.operands.push_back(
                                SIROperand::Global(staticFieldName, sfIt->second));
                            loadInst.comment = "Load static field: " + staticFieldName;
                            b_.currentBlock_->addInstruction(loadInst);
                        }
                        return BuildResult(loadReg, sfIt->second);
                    }
                }

                // (AR) ?????? 1: ???? ????? ??????
                // (EN) Step 1: Build object expression
                auto objResult = buildExpression(memberExpr->object.get());

                // ================================================================
                // (AR) ?????? 1.25: ??? ??? ??? ?????? ????? ?????? (Unit variant)
                //      ?? ????? ????. ????? ??????? ???.???? (???? ?????):
                //      - "???" ??? ??????? ??????? � ?? ??? ???????
                //      - buildExpression("???") ????? ????? ????? ?? ??? ?????
                //      - ??? ???? ?? ????? ?????? "???.????" ?????? ??? ?????
                //      ???? ???: ????? ? = ???.???? ?????? 0 ????? ?? ??????? ??????
                // (EN) Step 1.25: Check if access is to a Unit variant of an ADT enum.
                //      When ???.???? (without parens) is used:
                //      - "???" isn't a real variable � it's the enum type name
                //      - buildExpression("???") returns empty/invalid result
                //      - So look up the full name "???.????" as a direct global variable
                //      Without this: var s = Shape.Point stores 0 instead of correct tag
                // ================================================================
                {
                    // (AR) ??????? ??? ?????? ?? VariableExpr
                    // (EN) Extract object name from VariableExpr
                    std::string objName;
                    if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(memberExpr->object.get()))
                    {
                        objName = varExpr->name;
                    }

                    if (!objName.empty())
                    {
                        std::string fullName = objName + "." + memberExpr->memberName;

                        // (AR) ?????: ??? ??? ??? ????? ?????? ??????? ?????? (Unit variant ???)
                        // (EN) First: check if full name is a registered variable (global unit variant)
                        auto *varInfo = b_.lookupVariable(fullName);
                        if (varInfo)
                        {
                            // (AR) ????? ??????? ?????? � ????? ?????
                            //      ????: ???.???? ????? ?? global constant i64 2
                            // (EN) Found variable directly � load its value
                            //      Example: Shape.Point registered as global constant i64 2
                            if (varInfo->isGlobal && b_.currentBlock_)
                            {
                                std::string loadReg = b_.newTempRegister();
                                SIRInstruction loadInst(SIROpcode::LOAD);
                                loadInst.result = SIROperand::Register(loadReg, varInfo->type);
                                loadInst.operands.push_back(
                                    SIROperand::Global(fullName, varInfo->type));
                                loadInst.comment = "Load unit variant: " + fullName;
                                b_.currentBlock_->addInstruction(loadInst);
                                return BuildResult(loadReg, varInfo->type);
                            }
                            return BuildResult(varInfo->registerName, varInfo->type);
                        }
                    }
                }

                // (AR) ?????? 1.5: ??? ??? ??? ?????? ???? ????? ???? (ADT)
                //      ??? ??? ?????? ?????? ?? objectClassMap_ ???? ADT
                //      ?????? ENUM_GET_PAYLOAD ????? ?? LOAD ??????
                //      ????: ?.???_????? ??? ? = ???.?????(5)
                // (EN) Step 1.5: Check if object is an ADT enum value
                //      If object is registered in objectClassMap_ as ADT type
                //      use ENUM_GET_PAYLOAD instead of regular LOAD
                //      Example: s.radius where s = Shape.Circle(5)
                if (!objResult.className.empty())
                {
                    auto adtIt = b_.adtEnumTable_.find(objResult.className);
                    if (adtIt != b_.adtEnumTable_.end())
                    {
                        // (AR) ??? ???? ADT � ???? ?? ????? ??????
                        // (EN) This is an ADT object � look up field by name
                        const ADTEnumInfo &adtInfo = adtIt->second;
                        int fieldIdx = adtInfo.findFieldIndex(memberExpr->memberName);

                        if (fieldIdx >= 0)
                        {
                            // (AR) ????? ????? � ??????? ??? ENUM_GET_PAYLOAD
                            // (EN) Found the field � extract via ENUM_GET_PAYLOAD
                            std::string resultReg = b_.newTempRegister();

                            if (b_.currentBlock_)
                            {
                                SIRInstruction getPayload(SIROpcode::ENUM_GET_PAYLOAD);
                                getPayload.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                                getPayload.operands.push_back(
                                    SIROperand::Register(objResult.registerName, objResult.type));
                                getPayload.operands.push_back(
                                    SIROperand::ConstantI64(static_cast<int64_t>(fieldIdx)));
                                getPayload.comment = "ADT field access: " + objResult.className +
                                                     "." + memberExpr->memberName + " (index=" + std::to_string(fieldIdx) + ")";
                                b_.currentBlock_->addInstruction(getPayload);
                            }

#ifndef NDEBUG
                            std::cout << "[DEBUG] buildMemberAccess: ADT field '"
                                      << memberExpr->memberName << "' at index " << fieldIdx
                                      << " in ADT '" << objResult.className << "'" << std::endl;
#endif

                            BuildResult result(resultReg, SadTypeKind::Integer);
                            result.className = objResult.className;
                            result.isFieldAccess = true;
                            return result;
                        }
                        // (AR) ????? ??? ????? ?? ADT � ????? ?? ?????? ??????
                        // (EN) Field not found in ADT � fall through to regular path
                    }
                }

                // (AR) ?????? 2: ????? ?????? ?????? ????? (?????? ?????? ???????)
                // (EN) Step 2: Create member access instruction (regular path for classes)
                std::string resultReg = b_.newTempRegister();

                // (AR) ?????? ??????? ??? ????? ?? module
                // (EN) Try to infer member type from module
                SadTypeKind memberType = SadTypeKind::Integer; // (AR) ???????
                if (!objResult.className.empty() && b_.module_)
                {
                    auto sirClass = b_.module_->getClass(objResult.className);
                    if (sirClass)
                    {
                        auto fieldIt = sirClass->fields_.find(memberExpr->memberName);
                        if (fieldIt != sirClass->fields_.end())
                        {
                            // (AR) ??? ARRAY ????? ????? � ???? ??????? ???? I64
                            // (EN) Only ARRAY needs change � other types stay I64
                            if (fieldIt->second == SadTypeKind::Array)
                            {
                                memberType = SadTypeKind::Array;
                            }
                        }
                    }
                }

                if (b_.currentBlock_)
                {
                    SIRInstruction loadInst;
                    loadInst.opcode = SIROpcode::LOAD;
                    loadInst.result = SIROperand::Register(resultReg, memberType);

                    // (AR) ??????? ?????: ??????
                    // (EN) First operand: object
                    loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));

                    // (AR) ??????? ??????: ??? ????? (?? offset ?? ???)
                    // (EN) Second operand: member name (as offset or name)
                    loadInst.operands.push_back(SIROperand::ConstantString(memberExpr->memberName));

                    b_.currentBlock_->addInstruction(loadInst);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMemberAccess: result in register '" << resultReg
                          << "' with type " << sirTypeToString(memberType) << std::endl;
#endif

                BuildResult result(resultReg, memberType);
                result.className = objResult.className;
                result.isFieldAccess = true;
                return result;
            }
            // (AR) تم نقل b_.buildMethodCall إلى sir_builder_method_call.cpp (CW-05)
            // (EN) b_.buildMethodCall moved to sir_builder_method_call.cpp (CW-05)
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
