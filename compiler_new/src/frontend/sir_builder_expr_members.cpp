// ============================================================================
// sir_builder_expr_members.cpp
// ============================================================================
// (AR) دوال بناء تعابير الأعضاء والإسناد للأعضاء في الأصناف
//      مستخرجة من sir_builder_expressions_dispatch.cpp
// (EN) Member access and member assignment expression builders
//      Extracted from sir_builder_expressions_dispatch.cpp
// ============================================================================
#include "sir_builder.h"
#include "class_nodes.h"

#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildExprMember — بناء تعبير MemberExpr (وصول للعضو في كائن)
            // ============================================================================
            BuildResult SIRBuilder::buildExprMember(AST::MemberExpr *memberExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found MemberExpr for member '"
                          << memberExpr->member << "'" << std::endl;
#endif

                // (AR) الخطوة 1: بناء تعبير الكائن
                // (EN) Step 1: Build object expression
                auto objResult = buildExpression(memberExpr->object.get());

                // ================================================================
                // (AR) الخطوة 1.25: فحص وصول لحالة واحدية (Unit variant) في تعداد جبري
                // (EN) Step 1.25: Check if accessing Unit variant of ADT enum
                // ================================================================
                {
                    std::string objName;
                    if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(memberExpr->object.get()))
                    {
                        objName = varExpr->name;
                    }
                    if (!objName.empty())
                    {
                        std::string fullName = objName + "." + memberExpr->member;

                        // (AR) الخطوة 1.25أ: فحص ما إذا كان unit variant في تعداد جبري (ADT)
                        // (EN) Step 1.25a: Check if this is a unit variant in an ADT enum
                        auto funcIt = functionTable_.find(fullName);
                        if (funcIt != functionTable_.end() && currentBlock_)
                        {
                            std::string callReg = newTempRegister();
                            SIRInstruction callInst(SIROpcode::CALL);
                            callInst.result = SIROperand::Register(callReg, SadTypeKind::Integer);
                            callInst.operands.push_back(
                                SIROperand::Function(funcIt->second.name));
                            callInst.comment = "Call unit variant constructor: " + fullName;
                            currentBlock_->addInstruction(callInst);

                            classInstanceTypes_[callReg] = objName;

                            BuildResult result(callReg, SadTypeKind::Integer);
                            result.className = objName;
                            return result;
                        }

                        // (AR) الخطوة 1.25ب: تحميل ثابت عام (تعداد بسيط أو C-style enum)
                        // (EN) Step 1.25b: Load global constant (simple enum or C-style enum)
                        auto *varInfo = lookupVariable(fullName);
                        if (varInfo)
                        {
                            if (varInfo->isGlobal && currentBlock_)
                            {
                                std::string loadReg = newTempRegister();
                                SIRInstruction loadInst(SIROpcode::LOAD);
                                loadInst.result = SIROperand::Register(loadReg, varInfo->type);
                                loadInst.operands.push_back(
                                    SIROperand::Global(fullName, varInfo->type));
                                loadInst.comment = "Load unit variant: " + fullName;
                                currentBlock_->addInstruction(loadInst);
                                return BuildResult(loadReg, varInfo->type);
                            }
                            return BuildResult(varInfo->registerName, varInfo->type);
                        }
                    }
                }

                // (AR) استنتاج نوع العضو من module (الأصناف المسجلة)
                // (EN) Infer member type from module (registered classes)
                SadTypeKind memberType = SadTypeKind::Integer;
                std::string className = objResult.className;
                if (className.empty() && dynamic_cast<Sad::AST::ThisExpr *>(memberExpr->object.get()))
                {
                    className = currentClassName_;
                }
                if (!className.empty() && module_)
                {
                    auto sirClass = module_->getClass(className);
                    if (sirClass)
                    {
                        auto fieldIt = sirClass->fields_.find(memberExpr->member);
                        if (fieldIt != sirClass->fields_.end())
                        {
                            if (fieldIt->second == SadTypeKind::Array)
                            {
                                memberType = SadTypeKind::Array;
                            }
                            else if (fieldIt->second == SadTypeKind::String)
                            {
                                memberType = SadTypeKind::String;
                            }
                        }
                    }
                }

                // (AR) الخطوة 2: إنشاء تعليمة الوصول للعضو
                // (EN) Step 2: Create member access instruction
                std::string resultReg = newTempRegister();

                if (currentBlock_)
                {
                    SIRInstruction loadInst;
                    loadInst.opcode = SIROpcode::LOAD;
                    loadInst.result = SIROperand::Register(resultReg, memberType);
                    loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    loadInst.operands.push_back(SIROperand::ConstantString(memberExpr->member));
                    currentBlock_->addInstruction(loadInst);
                }

                BuildResult memberResult(resultReg, memberType);
                memberResult.isFieldAccess = true;
                return memberResult;
            }

            // ============================================================================
            // buildExprMemberAssign — بناء تعبير MemberAssignExpr (إسناد لعضو في كائن)
            // ============================================================================
            BuildResult SIRBuilder::buildExprMemberAssign(AST::MemberAssignExpr *memberAssignExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found MemberAssignExpr" << std::endl;
#endif

                // (AR) فحص مبكر: تعيين حقل ساكن عبر اسم الصنف
                // (EN) Early check: static field assignment via class name
                if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(memberAssignExpr->object.get()))
                {
                    std::string staticFieldName = varExpr->name + "." + memberAssignExpr->member;
                    auto sfIt = staticFields_.find(staticFieldName);
                    if (sfIt != staticFields_.end())
                    {
                        auto valResult = buildExpression(memberAssignExpr->value.get());

                        if (valResult.isConstant && currentBlock_)
                        {
                            std::string reg = newTempRegister();
                            SIRInstruction moveInst(SIROpcode::MOVE);
                            moveInst.result = SIROperand::Register(reg, valResult.type);
                            if (valResult.type == SadTypeKind::Float)
                                moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                            else
                            {
                                try
                                {
                                    moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                                }
                                catch (...)
                                {
                                    moveInst.operands.push_back(SIROperand::ConstantI64(0));
                                }
                            }
                            currentBlock_->addInstruction(moveInst);
                            valResult.registerName = reg;
                            valResult.isConstant = false;
                        }

                        if (currentBlock_)
                        {
                            SIRInstruction storeInst(SIROpcode::STORE);
                            storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                            storeInst.operands.push_back(SIROperand::Global(staticFieldName, sfIt->second));
                            storeInst.comment = "Store static field: " + staticFieldName;
                            currentBlock_->addInstruction(storeInst);
                        }
                        return BuildResult(valResult.registerName, valResult.type);
                    }
                }

                // (AR) بناء تعبير الكائن والقيمة
                // (EN) Build object expression and value
                auto objResult = buildExpression(memberAssignExpr->object.get());
                auto valResult = buildExpression(memberAssignExpr->value.get());

                // (AR) تجسيد القيمة إذا كانت ثابتة
                // (EN) Materialize value if constant
                if (valResult.isConstant && currentBlock_)
                {
                    std::string reg = newTempRegister();
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(reg, valResult.type);
                    if (valResult.type == SadTypeKind::String)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantString(valResult.constantValue));
                    }
                    else if (valResult.type == SadTypeKind::Float)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                    }
                    else if (valResult.type == SadTypeKind::Boolean)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantBool(valResult.constantValue == "true" || valResult.constantValue == "1"));
                    }
                    else
                    {
                        try
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                        }
                        catch (const std::exception &)
                        {
#ifndef NDEBUG
                            std::cerr << "[SIR] تحذير: فشل تحويل '" << valResult.constantValue << "' إلى I64، القيمة الافتراضية 0\n";
#endif
                            moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                    }
                    currentBlock_->addInstruction(moveInst);
                    valResult.registerName = reg;
                    valResult.isConstant = false;
                }

                // (AR) تعليمة STORE لتخزين القيمة في العضو
                // (EN) STORE instruction to store value in member
                SIRInstruction storeInst;
                storeInst.opcode = SIROpcode::STORE;
                storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                storeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                storeInst.operands.push_back(SIROperand::ConstantString(memberAssignExpr->member));
                storeInst.comment = "member assign: " + memberAssignExpr->member;

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(storeInst);
                }

                return BuildResult(valResult.registerName, valResult.type);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
