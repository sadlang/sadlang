// ============================================================================
// sir_builder_stmt_property.cpp
// ============================================================================
#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include "sad_debug_log.h"
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

            bool StatementBuilder::buildStatement_Property(AST::Statement *stmt)
            {
                // ========================================================================
                // (AR) PropertyDecl - خاصية مع getter/setter (property_nodes.h:PropertyDecl)
                // (EN) Property declaration — lowered to getter/setter functions
                // ========================================================================
                if (auto propDecl = dynamic_cast<Sad::AST::PropertyDecl *>(stmt))
                {
#ifndef NDEBUG
                    SAD_DEBUG_LOG_LINE("[DEBUG] Found PropertyDecl: " << propDecl->name);
#endif

                    SadTypeKind propType = b_.astTypeToSIRType(propDecl->type);
                    const bool hasClassContext = !b_.currentClassName_.empty();
                    const std::string functionPrefix = hasClassContext ? (b_.currentClassName_ + ".") : "";

                    // (AR) بناء دالة getter إن وُجدت
                    // (EN) Build getter function if present
                    if (propDecl->getter)
                    {
                        std::string getterName = functionPrefix + "__get_" + propDecl->name;
                        auto getterFunc = std::make_shared<SIRFunction>(getterName, propType);

                        // (AR) معامل self القياسي
                        // (EN) Standard self parameter
                        getterFunc->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                        auto entryBlock = b_.createBasicBlock("getter_entry");
                        getterFunc->addBasicBlock(entryBlock);

                        auto savedCtxGetter = b_.saveContext();
                        b_.currentFunction_ = getterFunc;
                        b_.currentBlock_ = entryBlock;

                        b_.enterScope();

                        // (AR) تسجيل self + هذا لتمكين هذا.الحقل داخل getter
                        // (EN) Register self + this alias to support this.field inside getter
                        {
                            VariableInfo selfInfo;
                            selfInfo.name = kSelfParamName;
                            selfInfo.type = SadTypeKind::Integer;
                            selfInfo.registerName = kSelfRegisterName;
                            selfInfo.isGlobal = false;
                            selfInfo.isMutable = false;
                            b_.addVariable(selfInfo);

                            VariableInfo thisInfo;
                            thisInfo.name = kThisAliasName;
                            thisInfo.type = SadTypeKind::Integer;
                            thisInfo.registerName = kSelfRegisterName;
                            thisInfo.isGlobal = false;
                            thisInfo.isMutable = false;
                            b_.addVariable(thisInfo);

                            if (hasClassContext)
                            {
                                b_.classInstanceTypes_[kSelfRegisterName] = b_.currentClassName_;
                                b_.classInstanceTypes_[kThisAliasName] = b_.currentClassName_;
                            }
                        }

                        if (propDecl->getter->body)
                        {
                            buildStatement(propDecl->getter->body.get());
                        }
                        b_.exitScope();

                        if (b_.currentBlock_)
                        {
                            bool hasTerminator = false;
                            if (!b_.currentBlock_->instructions.empty())
                            {
                                auto lastOp = b_.currentBlock_->instructions.back().opcode;
                                hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                            }
                            if (!hasTerminator)
                            {
                                SIRInstruction retInst;
                                if (propType == SadTypeKind::Void)
                                {
                                    retInst.opcode = SIROpcode::RET_VOID;
                                }
                                else
                                {
                                    retInst.opcode = SIROpcode::RET;
                                    if (propType == SadTypeKind::String)
                                    {
                                        retInst.operands.push_back(SIROperand::ConstantString(""));
                                    }
                                    else
                                    {
                                        retInst.operands.push_back(SIROperand::ConstantI64(0));
                                    }
                                }
                                b_.currentBlock_->addInstruction(retInst);
                            }
                        }

                        b_.restoreContext(std::move(savedCtxGetter));
                        b_.module_->addFunction(getterFunc);
                    }

                    // (AR) بناء دالة setter إن وُجدت
                    // (EN) Build setter function if present
                    if (propDecl->setter)
                    {
                        std::string setterName = functionPrefix + "__set_" + propDecl->name;
                        auto setterFunc = std::make_shared<SIRFunction>(setterName, SadTypeKind::Void);

                        // (AR) معامل self + معامل القيمة
                        // (EN) self parameter + value parameter
                        setterFunc->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));
                        setterFunc->addParameter(SIRParameter(propDecl->setter->parameterName, propType));

                        auto entryBlock = b_.createBasicBlock("setter_entry");
                        setterFunc->addBasicBlock(entryBlock);

                        auto savedCtxSetter = b_.saveContext();
                        b_.currentFunction_ = setterFunc;
                        b_.currentBlock_ = entryBlock;

                        b_.enterScope();

                        // (AR) تسجيل self + هذا لتمكين هذا.الحقل داخل setter
                        // (EN) Register self + this alias to support this.field inside setter
                        {
                            VariableInfo selfInfo;
                            selfInfo.name = kSelfParamName;
                            selfInfo.type = SadTypeKind::Integer;
                            selfInfo.registerName = kSelfRegisterName;
                            selfInfo.isGlobal = false;
                            selfInfo.isMutable = false;
                            b_.addVariable(selfInfo);

                            VariableInfo thisInfo;
                            thisInfo.name = kThisAliasName;
                            thisInfo.type = SadTypeKind::Integer;
                            thisInfo.registerName = kSelfRegisterName;
                            thisInfo.isGlobal = false;
                            thisInfo.isMutable = false;
                            b_.addVariable(thisInfo);

                            if (hasClassContext)
                            {
                                b_.classInstanceTypes_[kSelfRegisterName] = b_.currentClassName_;
                                b_.classInstanceTypes_[kThisAliasName] = b_.currentClassName_;
                            }
                        }

                        // (AR) تسجيل معامل القيمة في النطاق
                        // (EN) Register value parameter in scope
                        VariableInfo paramInfo;
                        paramInfo.name = propDecl->setter->parameterName;
                        paramInfo.type = propType;
                        paramInfo.registerName = propDecl->setter->parameterName;
                        paramInfo.isGlobal = false;
                        paramInfo.isMutable = true;
                        b_.addVariable(paramInfo);

                        if (propDecl->setter->body)
                        {
                            buildStatement(propDecl->setter->body.get());
                        }
                        b_.exitScope();

                        if (b_.currentBlock_)
                        {
                            bool hasTerminator = false;
                            if (!b_.currentBlock_->instructions.empty())
                            {
                                auto lastOp = b_.currentBlock_->instructions.back().opcode;
                                hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                            }
                            if (!hasTerminator)
                            {
                                b_.currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
                            }
                        }

                        b_.restoreContext(std::move(savedCtxSetter));
                        b_.module_->addFunction(setterFunc);
                    }
                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
