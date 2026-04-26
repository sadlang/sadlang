// ============================================================================
// sir_builder_assign_if.cpp — إسناد، متغيرات محلية، جملة إذا
// (AR) buildAssignment, buildLocalVariable, buildIfStatement
// (EN) Assignment, local variable declaration, if statement SIR generation
// تم استخراج هذا الملف من sir_builder_control_branch.cpp وفقاً لقاعدة CW-05
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

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            void SIRBuilder::buildAssignment(AST::AssignExpr *assignment)
            {
                if (!assignment)
                {
                    return;
                }

                // (AR) ״§„״¨״­״« ״¹† ״§„…״×״÷״± (AssignExpr::name: std::string, line 249)
                // (EN) Lookup variable
                VariableInfo *varInfo = lookupVariable(assignment->name);
                if (!varInfo)
                {
                    // (AR) …״×״÷״± ״÷״± …״¹״±
                    // (EN) Undefined variable
                    errors_.push_back("Undefined variable: " + assignment->name);
                    return;
                }

                // (AR) ״§„״×״­‚‚ …† ״¥…ƒ״§†״© ״§„״×״¹״¯„ (VariableInfo::isMutable, sir_builder.h:145)
                // (EN) Check if mutable
                if (!varInfo->isMutable)
                {
                    // (AR) …״×״÷״± ״«״§״¨״× „״§ …ƒ† ״×״¹״¯„‡
                    // (EN) Constant variable cannot be modified
                    errors_.push_back("Cannot assign to const variable: " + assignment->name);
                    return;
                }

                // (AR) ״¨†״§״¡ ‚…״© ״§„״×״¹״¨״± (AssignExpr::value: ExprPtr, line 250)
                // (EN) Build value expression
                auto valueResult = buildExpression(assignment->value.get());

                // (AR) ״×ˆ„״¯ ״×״¹„…״© STORE „״¥״³†״§״¯ ״§„‚…״©
                // (EN) Generate STORE instruction to assign value
                if (currentBlock_ && !valueResult.registerName.empty())
                {
                    SIRInstruction storeInst;
                    storeInst.opcode = SIROpcode::STORE;

                    // (AR) ״§„…״¹״§…„ ״§„״£ˆ„: ״§„‚…״© ״§„…״±״§״¯ ״×״®״²†‡״§
                    // (EN) First operand: value to store
                    SIROperand valueOp;
                    if (valueResult.isConstant && valueResult.type == SadTypeKind::Function)
                    {
                        // (AR) ״¥״¹״§״¯״© ״×״¹† „״§…״¯״§ „…״×״÷״±: ״§״³״×״®״¯… SIROperand::Function
                        //      ״¨״¯„״§‹ …† CONSTANT „״£† resolveOperand ״¯״¹… FUNCTION …״¨״§״´״±״©
                        // (EN) Lambda reassignment: use SIROperand::Function
                        //      instead of CONSTANT since resolveOperand handles FUNCTION directly
                        valueOp = SIROperand::Function(valueResult.constantValue);
                    }
                    else if (valueResult.isConstant)
                    {
                        valueOp.type = SIROperandType::CONSTANT;
                        valueOp.dataType = valueResult.type;
                        valueOp.name = valueResult.constantValue;
                        if (valueResult.type == SadTypeKind::Integer)
                        {
                            try
                            {
                                valueOp.intValue = std::stoll(valueResult.constantValue);
                            }
                            catch (const std::exception &)
                            {
                                valueOp.intValue = 0;
                            }
                        }
                        else if (valueResult.type == SadTypeKind::Float)
                        {
                            try
                            {
                                valueOp.floatValue = std::stod(valueResult.constantValue);
                            }
                            catch (const std::exception &)
                            {
                                valueOp.floatValue = 0.0;
                            }
                        }
                        // ================================================================
                        // (AR) [Fix #49] …״¹״§„״¬ Boolean ג€” ״¨״¯ˆ† ‡״°״§״ ״µ״­״­/״®״·״£ ״®״²†״§† ״¯״§״¦…״§‹ ƒ€ false
                        //      „״£† intValue ״¨‚‰ 0 (״§„״§״×״±״§״¶) ˆ„״§ ״¹‘† …† constantValue
                        //      constantValue ‡ "true" ״£ˆ "false" (…† buildLiteral)
                        //      resolveOperand ״³״×״®״¯… intValue != 0 „״×״­״¯״¯ i1 true/false
                        // (EN) [Fix #49] Boolean handler ג€” without this, true/false always stored as false
                        //      because intValue stays 0 (default) and is not set from constantValue
                        //      resolveOperand uses intValue != 0 to determine i1 true/false
                        // ================================================================
                        else if (valueResult.type == SadTypeKind::Boolean)
                        {
                            valueOp.intValue = (valueResult.constantValue == "true") ? 1 : 0;
                        }
                        // (AR) [Fix #47] „״§״´״¡/null ג€” ״¹״¯״¯ ״µ״­״­ ״¨‚…״© 0 (״×…״«„ …ˆ״­״¯)
                        // (EN) [Fix #47] null ג€” integer with value 0 (unified representation)
                        else if (valueResult.type == SadTypeKind::Pointer)
                        {
                            valueOp.intValue = 0;
                        }
                    }
                    else
                    {
                        valueOp.type = SIROperandType::REGISTER;
                        valueOp.name = valueResult.registerName;
                        valueOp.dataType = valueResult.type;
                    }
                    storeInst.operands.push_back(valueOp);

                    // (AR) ״§„…״¹״§…„ ״§„״«״§†: ״§„…״₪״´״± (alloca) „„…״×״÷״±
                    // (EN) Second operand: variable pointer (alloca)
                    SIROperand ptrOp;
                    ptrOp.type = SIROperandType::REGISTER;
                    ptrOp.name = varInfo->registerName;
                    ptrOp.dataType = varInfo->type;
                    storeInst.operands.push_back(ptrOp);

                    currentBlock_->instructions.push_back(storeInst);

                    // ================================================================
                    // (AR) [Fix #51] ״¥״°״§ ƒ״§† ״§„…״×״÷״± …„״×‚״·״§‹  ״¥״÷„״§‚ ג€” ״£״µ״¯״± ENV_STORE
                    //      „ƒ״×״§״¨״© ״§„‚…״© ״§„״¬״¯״¯״© ״¥„‰ …״µˆ״© env[captureIndex]
                    //      †״³״×״®״¯… †״³ valueOp (״§„‚…״© ״§„…״³†״¯״©) …״¨״§״´״±״© ג€” „״§ ״­״§״¬״© „€ LOAD
                    //      „״£† ״§„‚…״© …״×״§״­״© ״¨״§„״¹„  registerName ״£ˆ ƒ״«״§״¨״×
                    // (EN) [Fix #51] If variable is captured in closure ג€” emit ENV_STORE
                    //      to write new value to env[captureIndex]
                    //      Use valueOp directly ג€” no need for LOAD since value is already available
                    // ================================================================
                    if (varInfo->isCaptured && varInfo->captureIndex >= 0)
                    {
                        SIRInstruction envStoreInst;
                        envStoreInst.opcode = SIROpcode::ENV_STORE;
                        envStoreInst.operands.push_back(valueOp);
                        envStoreInst.operands.push_back(SIROperand::Register(varInfo->envRegister, SadTypeKind::Integer));
                        envStoreInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(varInfo->captureIndex)));
                        envStoreInst.comment = "closure env_store: " + varInfo->name + " -> env[" + std::to_string(varInfo->captureIndex) + "]";
                        currentBlock_->addInstruction(envStoreInst);
                    }

                    // ================================================================
                    // (AR) [Fix #52] ״×״­״¯״« †ˆ״¹ ״§„…״×״÷״± ״¹†״¯ ״¥״¹״§״¯״© ״§„״¥״³†״§״¯ ״¨†ˆ״¹ …״®״×„:
                    //      „״÷״© ״µ ״¯†״§…ƒ״© ג€” …ƒ† ״¥״¹״§״¯״© ״¥״³†״§״¯ …״×״÷״± ״¨†ˆ״¹ ״¢״®״±:
                    //      …״×״÷״± ״³ = „״§״´״¡  ג†’  †ˆ״¹: Integer/Pointer
                    //      ״³ = 42           ג†’  †ˆ״¹: Integer
                    //      ״³ = "†״µ"         ג†’  †ˆ״¹: **״¬״¨ ״£† ״µ״¨״­ String**
                    //      ״¨״¯ˆ† ‡״°״§: varInfo->type ״¨‚‰ Integer״ ˆ״§„״·״¨״§״¹״© ״×״·״¨״¹ ״¹†ˆ״§† ״§„…״₪״´״±
                    //      ƒ״±‚… ״¨״¯„״§‹ …† …״­״×ˆ‰ ״§„†״µ.
                    //      ‡״°״§ ״§„״×״­״¯״« ״¨„‘״÷ buildExpression ˆ emitBuiltinPrint ״¨״§„†ˆ״¹ ״§„״µ״­״­.
                    // (EN) [Fix #52] Update variable type on cross-type reassignment:
                    //      Sad is dynamically typed ג€” variables can be reassigned to different types.
                    //      Without this: varInfo->type stays Integer, print outputs pointer address
                    //      as number instead of string content.
                    // ================================================================
                    if (valueResult.type != SadTypeKind::Void &&
                        valueResult.type != SadTypeKind::Unknown &&
                        valueResult.type != varInfo->type)
                    {
                        varInfo->type = valueResult.type;
                    }
                }

                // (AR) ״×״×״¨״¹ †ˆ״¹ ״§„״µ† ״¹†״¯ ״¥״¹״§״¯״© ״§„״×״¹† ״¨€ ״¬״¯״¯()
                //      …״«״§„: _†״¸״§… = ״¬״¯״¯ †״¸״§…_…„״§״×() ג† ״¬״¨ ״±״¨״· _†״¸״§… ״¨״§„״µ† †״¸״§…_…„״§״×
                //      ״¨״¯ˆ† ‡״°״§״ ״§״³״×״¯״¹״§״¡״§״× _†״¸״§….‡״¦() ״×ˆ„‘״¯ ".‡״¦" ״¨״¯„״§‹ …† "†״¸״§…_…„״§״×.‡״¦"
                // (EN) Track class type on reassignment with new()
                //      e.g., _†״¸״§… = ״¬״¯״¯ †״¸״§…_…„״§״×() ג†’ must associate _†״¸״§… with †״¸״§…_…„״§״×
                //      Without this, _†״¸״§….‡״¦() generates ".‡״¦" instead of "†״¸״§…_…„״§״×.‡״¦"
                if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(assignment->value.get()))
                {
                    classInstanceTypes_[assignment->name] = newExpr->className;
                }

                // (AR) [†״¸״§… ״§„״¥״÷„״§‚״§״× ״§„״¬״¯״¯] „… †״¹״¯ †״³״¬‘„ lambdaAliases_ ״¹†״¯ ״¥״¹״§״¯״© ״§„״×״¹†
                //      ״¬…״¹ ״§״³״×״¯״¹״§״¡״§״× ״§„„״§…״¯״§ ״×…״± ״¹״¨״± CLOSURE_CALL
                // (EN) [New closure system] No longer update lambdaAliases_ on reassignment
                //      All lambda calls go through CLOSURE_CALL
            }

            // ============================================================================
            // buildLocalVariable - ״¨†״§״¡ ״×״µ״±״­ …״×״÷״± …״­„
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:433
            // ״§„״×ˆ‚״¹ / Signature: void buildLocalVariable(AST::VarDeclStmt* varDecl);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - varDecl: AST::VarDeclStmt* (statements.h:74)
            //
            // VarDeclStmt Members (statements.h:74-100):
            // - name: std::string (line 76)
            // - type: Data::DataType (line 77)
            // - initializer: ExprPtr (line 78)
            // - isConst: bool (line 79)
            // ============================================================================
            void SIRBuilder::buildLocalVariable(AST::VarDeclStmt *varDecl)
            {
                if (!varDecl)
                {
                    return;
                }

#ifdef SIR_BUILDER_DEBUG
                std::cerr << "[SIR-DBG] buildLocalVariable: name='" << varDecl->name
                          << "' type=" << static_cast<int>(varDecl->type) << std::endl;
#endif

                // ================================================================
                // (AR) ״×״®״· ״§„״«ˆ״§״¨״× ״§„״¹״§…״© ״§„״× „״¯‡״§ ‚…״© ״£ˆ„״© ״­״±״©:
                //      ״§„…״×״÷״± …״³״¬‘„ …״³״¨‚״§‹ ƒ…״×״÷״± ״¹״§… ״«״§״¨״×  ״§„…״±״­„״© 1.5 …״¹ ‚…״© ״£ˆ„״©.
                //      emitGlobalVariables †״´״¦ `@name = internal constant i64 42`.
                //      „״§ †״­״×״§״¬ ALLOC ״£ˆ STORE ג€” ״§„‚…״© …ˆ״¬ˆ״¯״©  ״§„…״×״÷״± ״§„״¹״§….
                //      ״¨״¯ˆ† ‡״°״§: STORE ״¥„‰ constant = ACCESS_VIOLATION/crash.
                // (EN) Skip const globals that already have a literal initializer:
                //      The variable is pre-registered as a constant global in Phase 1.5 with initialValue.
                //      emitGlobalVariables creates `@name = internal constant i64 42`.
                //      No ALLOC or STORE needed ג€” value is already in the global.
                //      Without this: STORE to constant = ACCESS_VIOLATION/crash.
                // ================================================================
                if (module_ && varDecl->isConst)
                {
                    auto sirGlobal = module_->getGlobalVariable(varDecl->name);
                    if (sirGlobal && sirGlobal->isConstant && !sirGlobal->initialValue.empty())
                    {
                        // (AR) ״§„״«״§״¨״× ״§„״¹״§… …‡״£ ״¨״§„״¹„ ג€” „״§ ״­״§״¬״© „ƒˆ״¯ ״¥״¶״§
                        // (EN) Const global already initialized ג€” no code needed
                        return;
                    }
                }

                // (AR) ״×״­ˆ„ ״§„†ˆ״¹ (VarDeclStmt::type: Data::DataType, line 77)
                // (EN) Convert type
                SadTypeKind varType = astTypeToSIRType(varDecl->type);
                bool needsTypeInference = (varDecl->type == Data::DataType::UNKNOWN);

                // (AR) ״¥†״´״§״¡ …״¹„ˆ…״§״× ״§„…״×״÷״± (sir_builder.h:139 - VariableInfo)
                // (EN) Create variable info
                VariableInfo varInfo;
                varInfo.name = varDecl->name; // line 76
                varInfo.type = varType;
                varInfo.registerName = "%" + varDecl->name;
                varInfo.isGlobal = false;
                varInfo.isMutable = !varDecl->isConst; // line 79
                varInfo.scopeLevel = currentScopeLevel_;

                // (AR) …״¹״§„״¬״© ״§„‚…״© ״§„״£ˆ„״© (VarDeclStmt::initializer: ExprPtr, line 78)
                // (EN) Handle initializer - process first for type inference
                BuildResult initResult;
                bool hasInitializer = varDecl->initializer && currentBlock_;

                if (hasInitializer)
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG]   building initializer, expr type="
                              << typeid(*varDecl->initializer).name() << std::endl;
#endif
                    initResult = buildExpression(varDecl->initializer.get());
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG]   initResult: reg='" << initResult.registerName
                              << "' type=" << static_cast<int>(initResult.type)
                              << " isConst=" << initResult.isConstant
                              << " constVal='" << initResult.constantValue << "'" << std::endl;
#endif

                    // (AR) ״§״³״×†״×״§״¬ ״§„†ˆ״¹ …† ״§„״×״¹״¨״± ״¥״°״§ ƒ״§† ״§„†ˆ״¹ ״÷״± …״¹״±ˆ
                    // (EN) Infer type from expression if type is unknown
                    if (needsTypeInference)
                    {
                        varType = initResult.type;
                        varInfo.type = varType;

                        // ================================================================
                        // (AR) [Fix #44] ״×״­״¯״« ״§„…״×״÷״± ״§„״¹״§…  SIRModule ״¹†״¯…״§ ״×״÷״± ״§„†ˆ״¹:
                        //      Phase 1.5 ״×״³״¬‘„ ״§„…״×״÷״±״§״× ״§„״¹״§…״© …״¨ƒ״±״§‹ ‚״¨„ ״×†״° buildExpression.
                        //      ״¥״°״§ ƒ״§†״× ״§„‚…״© ״§„״£ˆ„״© ״×״¹״¨״±״§‹ …״¹‚״¯״§‹ (BinaryExpr, CallExpr...)
                        //      ‚״¯ ״®״·״¦ Phase 1.5  ״§״³״×†״×״§״¬ ״§„†ˆ״¹ (…״«„״§‹: Integer ״¨״¯„״§‹ …† Float).
                        //      ‡†״§  Phase 2/3״ ״¨״¹״¯ buildExpression ״§„״¹„״ „״¯†״§ ״§„†ˆ״¹ ״§„״¯‚‚.
                        //      †״­״¯‘״« SIRGlobalVariable ˆ״§„†״·״§‚ „״×״·״§״¨‚״§ …״¹ ״§„†ˆ״¹ ״§„״­‚‚.
                        //      ״¨״¯ˆ† ‡״°״§: …״×״÷״± ״¹״´״± ״®״²†  alloca i64 ג†’ fptosi ג†’ ‚״·״¹ ״¹״´״±!
                        //
                        //      …״«״§„: …״×״÷״± …״¬ = ״¹1 + ״¹2 ״­״« ״¹1=3.14, ״¹2=2.71
                        //      Phase 1.5 ״£†״´״£ SIRGlobalVariable(…״¬, Integer) [‚״¨„ ״§„״¥״µ„״§״­]
                        //      ״§„״¢† ״¨״¹״¯ inferExprType: SIRGlobalVariable(…״¬, Float)
                        //      …״¹ ‡״°״§ ״§„״£…״§† ״§„״¥״¶״§: ״­״×‰ „ˆ ״£״®״·״£ inferExprType״
                        //      buildExpression ״¹״·†״§ ״§„†ˆ״¹ ״§„״¯‚‚ ˆ†״­״¯‘״« ‡†״§.
                        // (EN) [Fix #44] Update SIRGlobalVariable when type changes:
                        //      Phase 1.5 pre-registers globals before buildExpression runs.
                        //      For complex initializers (BinaryExpr, CallExpr...),
                        //      Phase 1.5 may infer wrong type (e.g. Integer instead of Float).
                        //      Here in Phase 2/3, after real buildExpression, we have exact type.
                        //      Update SIRGlobalVariable and scope to match the real type.
                        //      Without this: float stored in i64 alloca ג†’ fptosi ג†’ truncation!
                        // ================================================================
                        if (module_)
                        {
                            auto sirGlobal = module_->getGlobalVariable(varDecl->name);
                            if (sirGlobal && sirGlobal->type != varType)
                            {
                                sirGlobal->type = varType;
                            }
                        }
                        // (AR) ״×״­״¯״« VariableInfo ״§„…״³״¬„ …״³״¨‚״§‹  ״§„†״·״§‚ ״§„״¹״§… ״£״¶״§‹
                        // (EN) Also update pre-registered VariableInfo in global scope
                        VariableInfo *existingVar = lookupVariable(varDecl->name);
                        if (existingVar && existingVar->type != varType)
                        {
                            existingVar->type = varType;
                        }
                    }

                    // (AR) †‚„ †ˆ״¹ ״¹†״µ״± ״§„…״µˆ״© ״¥„‰ VariableInfo „״¯״¹… foreach
                    // (EN) Propagate array element type to VariableInfo for foreach support
                    if (initResult.elementType != SadTypeKind::Void)
                    {
                        varInfo.elementType = initResult.elementType;
                    }
                    if (!initResult.elementClassName.empty())
                    {
                        varInfo.elementClassName = initResult.elementClassName;
                    }

                    // (AR) ״×״×״¨״¹ ״§״³… ״¯״§„״© ״§„„״§…״¯״§ ״§„…״±״×״¨״·״© („״×״­״¯״¯ †ˆ״¹ ״§„״¥״±״¬״§״¹  CLOSURE_CALL)
                    // (EN) Track associated lambda function name (for CLOSURE_CALL return type)
                    if (!initResult.closureLambdaName.empty())
                    {
                        varInfo.closureLambdaName = initResult.closureLambdaName;
                    }

                    // (AR) ״×״×״¨״¹ †ˆ״¹ ״§„״µ† ״¥״°״§ ƒ״§† ״§„״×״¹״¨״± ״¬״¯״¯ ClassName()
                    // (EN) Track class type if expression is new ClassName()
                    if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(varDecl->initializer.get()))
                    {
                        classInstanceTypes_[varDecl->name] = newExpr->className;
                    }
                    // ================================================================
                    // (AR) ״×״×״¨״¹ †ˆ״¹ ״§„״µ† …† ״£ …״µ״¯״± ״¢״®״± (״§״³״×״¯״¹״§״¡ ״¯״§„״©״ ״¹״§…„ …״­…‘„״ ״§„״®):
                    //      ״¥״°״§ ƒ״§† ״§„״×״¹״¨״± ״§„…״¨״¯״¦ ״­…„ className (…״«„״§‹ …† buildFunctionCall
                    //      ״£ˆ buildBinaryOp …״¹ operator overloading)״ †״³״¬‘„ ״§„…״×״÷״±
                    //       classInstanceTypes_ ״­״×‰ ״×…ƒ† ״§„…״×״±״¬… …† …״¹״±״© †ˆ״¹ ״§„״µ†
                    //      ״¹†״¯ ״§„ˆ״µˆ„ „״­‚ˆ„ ״§„ƒ״§״¦† „״§״­‚״§‹.
                    //      …״«״§„: …״×״÷״± ״¬ = ״£ + ״¨ ג†’ ״¥״°״§ ״¹״§…„ + ״±״¬״¹ ƒ״§״¦† †‚״·״© ג†’ ״¬.״³ ״¹…„
                    //      …״«״§„: …״×״÷״± † = ״§״µ†״¹_†‚״·״©() ג†’ †.״³ ״¹…„
                    //      ״¨״¯ˆ† ‡״°״§ ״§„״¥״µ„״§״­: ״§„ˆ״µˆ„ „״­‚ˆ„ ƒ״§״¦† …״±״¬״¹ …† ״¯״§„״© ״£ˆ ״¹״§…„ ״×״¹״·„
                    // (EN) Track class type from any other source (function call, operator, etc):
                    //      If the initializer expression has className (e.g. from buildFunctionCall
                    //      or buildBinaryOp with operator overloading), register the variable
                    //      in classInstanceTypes_ so the compiler knows the class type when
                    //      accessing fields later.
                    //      Without this: accessing fields of object from function/operator crashes
                    // ================================================================
                    else if (!initResult.className.empty())
                    {
                        classInstanceTypes_[varDecl->name] = initResult.className;
                    }
                }

                // (AR) ״×ˆ„״¯ ״×״¹„…״© ALLOC „״×״®״µ״µ ״§„״°״§ƒ״±״©
                // (EN) Generate ALLOC instruction for memory allocation
                if (currentBlock_)
                {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register(varInfo.registerName, varType);
                    currentBlock_->addInstruction(allocInst);
                }

                // (AR) ״×ˆ„״¯ ״×״¹„…״© STORE „״¥״³†״§״¯ ״§„‚…״© ״§„״£ˆ„״©
                // (EN) Generate STORE instruction to assign initial value
                if (hasInitializer)
                {
                    // (AR) ״×״­‚‚: ‡„ ״§„‚…״© ״«״§״¨״×״© …ƒ† ״§״³״×״®״¯״§…‡״§ …״¨״§״´״±״©״
                    // (EN) Check: is the value a usable constant?
                    bool useConstant = initResult.isConstant && (initResult.type == SadTypeKind::String ||
                                                                 !initResult.constantValue.empty());

                    // (AR) ״×״®״· STORE ״¥״°״§ „… ״×ƒ† ״§„‚…״© ״«״§״¨״×״© ˆ„״§  ״³״¬„ ״µ״§„״­
                    //      ‡״°״§ ״­״¯״« ״¹†״¯ ״´„ buildExpression (…״«„״§‹: …״×״÷״± ״÷״± …״¹״±‘)
                    // (EN) Skip STORE if value is neither a usable constant nor in a valid register.
                    //      This happens when buildExpression fails (e.g., undefined variable).
                    if (!useConstant && initResult.registerName.empty())
                    {
                        // Just register the variable without initialization
                        addVariable(varInfo);
                        return;
                    }

                    SIRInstruction storeInst;
                    storeInst.opcode = SIROpcode::STORE;

                    // (AR) ״§„…״¹״§…„ ״§„״£ˆ„: ״§„‚…״© ״§„…״±״§״¯ ״×״®״²†‡״§
                    // (EN) First operand: value to store
                    if (useConstant)
                    {
                        // (AR) ״§„‚…״© ״«״§״¨״×״© - ״×״­ˆ„‡״§ „״«״§״¨״× SIR
                        // (EN) Value is constant - convert to SIR constant
                        switch (initResult.type)
                        {
                        case SadTypeKind::Integer:
                            storeInst.operands.push_back(SIROperand::ConstantI64(std::stoll(initResult.constantValue)));
                            break;
                        case SadTypeKind::Float:
                            storeInst.operands.push_back(SIROperand::ConstantF64(std::stod(initResult.constantValue)));
                            break;
                        case SadTypeKind::Boolean:
                            storeInst.operands.push_back(SIROperand::ConstantBool(initResult.constantValue == "true" || initResult.constantValue == "1"));
                            break;
                        case SadTypeKind::String:
                            storeInst.operands.push_back(SIROperand::ConstantString(initResult.constantValue));
                            break;
                        case SadTypeKind::Function:
                            // (AR) ״×״®״²† …״₪״´״± ״¯״§„״© („״§…״¯״§ ״£ˆ ״¯״§„״© ״¹״§״¯״©)
                            //      †״³״×״®״¯… SIROperand::Function „״×״¬†״¨ ״®״·״£ "Undefined register"
                            //      „״£† ״§„„״§…״¯״§ „״§ ״×†״×״¬ ״×״¹„…״© SIR ״×״¹״±‘ ״³״¬„״§‹ ג€” ‚״· SIRFunction
                            // (EN) Store function pointer (lambda or regular function)
                            //      Use SIROperand::Function to avoid "Undefined register" error
                            //      since lambda doesn't emit a SIR instruction that defines a register
                            storeInst.operands.push_back(SIROperand::Function(initResult.constantValue));
                            break;
                        default:
                            storeInst.operands.push_back(SIROperand::Register(initResult.registerName, initResult.type));
                            break;
                        }
                    }
                    else
                    {
                        // (AR) ״§„‚…״©  ״³״¬„
                        // (EN) Value is in register
                        storeInst.operands.push_back(SIROperand::Register(initResult.registerName, initResult.type));
                    }

                    // (AR) ״§„…״¹״§…„ ״§„״«״§†: ״¹†ˆ״§† ״§„…״×״÷״±
                    // (EN) Second operand: variable address
                    storeInst.operands.push_back(SIROperand::Register(varInfo.registerName, varType));

                    currentBlock_->addInstruction(storeInst);
                }

                // (AR) ״¥״¶״§״© ״§„…״×״÷״± „„†״·״§‚ (sir_builder.h:591 - addVariable)
                // (EN) Add variable to scope
                addVariable(varInfo);

                // (AR) [†״¸״§… ״§„״¥״÷„״§‚״§״× ״§„״¬״¯״¯] „… †״¹״¯ †״³״¬‘„ lambdaAliases_ ‡†״§
                //      ״¬…״¹ ״§״³״×״¯״¹״§״¡״§״× ״§„„״§…״¯״§ ״×…״± ״¹״¨״± CLOSURE_CALL (״§„״®״·ˆ״© 3.5)
                //      ״¨״¯„״§‹ …† ״§„״§״³״×״¯״¹״§״¡ ״§„…״¨״§״´״± ״¹״¨״± ״§„״§״³… ״§„…״³״×״¹״§״± (״§„״®״·ˆ״© 2.7)
                // (EN) [New closure system] No longer register lambdaAliases_ here
                //      All lambda calls go through CLOSURE_CALL (Step 3.5)
                //      instead of direct call via alias (Step 2.7)
            }

            // ============================================================================
            // buildIfStatement - ״¨†״§״¡ ״¬…„״© if
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:385
            // ״§„״×ˆ‚״¹ / Signature: void buildIfStatement(AST::IfStmt* ifStmt);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - ifStmt: AST::IfStmt* = Sad::AST::IfStmt* (sir_builder.h:385)
            //
            // IfStmt Members (statements.h:104-135):
            // - condition: ExprPtr (line 107)
            // - thenBranch: StmtPtr (line 108)
            // - elseBranch: StmtPtr (line 109) - optional, can be nullptr
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - currentBlock_: sir_builder.h:582 (shared_ptr<SIRBasicBlock>)
            //
            // ״§„״¯ˆ״§„ ״§„…״³״×״¯״¹״§״© / Called functions:
            // - buildExpression: sir_builder.h:432
            // - buildStatement: sir_builder.h:372
            // - createBasicBlock: sir_builder.h:501
            // - newLabel: sir_builder.h:520
            // ============================================================================
            void SIRBuilder::buildIfStatement(AST::IfStmt *ifStmt)
            {
                if (!ifStmt)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: starting" << std::endl;
#endif

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 1: ״¥†״´״§״¡ ״§„ƒ״×„ ״§„״£״³״§״³״©
                // (EN) Step 1: Create basic blocks
                // ״§„…״µ״¯״±: sir_builder.h:501 - createBasicBlock()
                // ״§„…״µ״¯״±: sir_builder.h:520 - newLabel()
                // ========================================================================
                std::string thenLabel = newLabel("then");
                std::string mergeLabel = newLabel("merge");

                // (AR) ƒ״×„״© else ‚״· ״¥״°״§ ƒ״§† ‡†״§ƒ ״±״¹ else
                // (EN) Else block only if there's an else branch
                std::string elseLabel = ifStmt->elseBranch ? newLabel("else") : mergeLabel;

                auto thenBlock = createBasicBlock(thenLabel);
                auto elseBlock = ifStmt->elseBranch ? createBasicBlock(elseLabel) : nullptr;
                auto mergeBlock = createBasicBlock(mergeLabel);

                // (AR) ״¥״¶״§״© ״§„ƒ״×„ ״¥„‰ ״§„״¯״§„״© ״§„״­״§„״©
                // (EN) Add blocks to current function
                if (currentFunction_)
                {
                    currentFunction_->addBasicBlock(thenBlock);
                    if (elseBlock)
                        currentFunction_->addBasicBlock(elseBlock);
                    currentFunction_->addBasicBlock(mergeBlock);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: created blocks then=" << thenLabel
                          << ", else=" << elseLabel << ", merge=" << mergeLabel << std::endl;
#endif

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 2: ״¨†״§״¡ ״§„״´״±״·
                // (EN) Step 2: Build condition expression
                // ״§„…״µ״¯״±: IfStmt::condition (statements.h:107)
                // ========================================================================
                auto condResult = buildExpression(ifStmt->condition.get());

                if (condResult.registerName.empty())
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildIfStatement: condition build failed!" << std::endl;
#endif
                    errors_.push_back("Error: Failed to build if condition");
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: condition reg=" << condResult.registerName << std::endl;
#endif

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 2.5: ״×״­ˆ„ ״×„‚״§״¦ „€ __op_tobool__ ״¥״°״§ ƒ״§† ״§„״´״±״· ƒ״§״¦†״§‹
                //      …״«״§„: ״¥״°״§ (ƒ״§״¦†) ג†’ ״¥״°״§ (__op_tobool__(ƒ״§״¦†))
                //      ״×ˆ״§‚ …״¹: expression_evaluator_calls.cpp findOperator("…†״·‚")
                // (EN) Step 2.5: Auto-convert __op_tobool__ if condition is an object
                //      Example: if (object) ג†’ if (__op_tobool__(object))
                //      Matches: interpreter's findOperator("…†״·‚")
                // ========================================================================
                {
                    std::string condClassName = condResult.className;
                    if (condClassName.empty() && !condResult.registerName.empty())
                    {
                        auto it = classInstanceTypes_.find(condResult.registerName);
                        if (it != classInstanceTypes_.end())
                            condClassName = it->second;
                    }
                    if (!condClassName.empty())
                    {
                        // (AR) ״¨״­״«  ״³„״³„״© ״§„ˆ״±״§״«״© ״¹† __op_tobool__
                        // (EN) Search inheritance chain for __op_tobool__
                        std::string searchClass = condClassName;
                        std::string toboolName;
                        bool foundToBool = false;
                        while (!searchClass.empty())
                        {
                            toboolName = searchClass + ".__op_tobool__";
                            if (functionTable_.find(toboolName) != functionTable_.end())
                            {
                                foundToBool = true;
                                break;
                            }
                            auto classInfo = module_->getClass(searchClass);
                            if (classInfo && !classInfo->parentClass.empty())
                                searchClass = classInfo->parentClass;
                            else
                                break;
                        }
                        if (foundToBool)
                        {
                            std::string boolReg = newTempRegister();
                            SIRInstruction callInst;
                            callInst.opcode = SIROpcode::OBJECT_CALL;
                            callInst.result = SIROperand::Register(boolReg, SadTypeKind::Boolean);
                            callInst.operands.push_back(SIROperand::Register(condResult.registerName, condResult.type));
                            callInst.operands.push_back(SIROperand::ConstantString("__op_tobool__"));
                            if (currentBlock_)
                                currentBlock_->addInstruction(callInst);
                            condResult = BuildResult(boolReg, SadTypeKind::Boolean);
                        }
                    }
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 3: ״×ˆ„״¯ ״×״¹„…״© ״§„‚״² ״§„״´״±״·
                // (EN) Step 3: Generate conditional branch instruction
                // ״§„…״µ״¯״±: sir_instruction.h:190-197 - SIRInstruction::BranchCond()
                // ״§„…״µ״¯״±: sir_types.h:366-372 - SIROperand::Label()
                // ========================================================================
                // (AR) ״¥״°״§ ƒ״§† ״§„״´״±״· ״«״§״¨״×״§‹ …†״·‚״§‹ (״µ״­״­/״®״·״£)״ †״³״×״®״¯… ConstantBool ״¨״¯„״§‹ …† Register
                //      „״×״¬†״¨ ״×ˆ„״¯ ״³״¬„ ״÷״± …״¹״±‘  LLVM IR
                // (EN) If condition is a boolean constant (true/false), use ConstantBool instead of Register
                //      to avoid generating an undefined register in LLVM IR
                SIROperand condOp;
                if (condResult.isConstant && condResult.type == SadTypeKind::Boolean)
                {
                    condOp = SIROperand::ConstantBool(condResult.constantValue == "true" || condResult.constantValue == "1");
                }
                else if (condResult.isConstant && condResult.type == SadTypeKind::Integer)
                {
                    condOp = SIROperand::ConstantI64(std::stoll(condResult.constantValue));
                }
                else
                {
                    condOp = SIROperand::Register(condResult.registerName, condResult.type);
                }
                SIROperand thenLabelOp = SIROperand::Label(thenLabel);
                SIROperand elseLabelOp = SIROperand::Label(elseLabel);

                SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, thenLabelOp, elseLabelOp);

                if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brCondInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildIfStatement: added BR_COND to current block" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 4: ״¨†״§״¡ ״±״¹ then
                // (EN) Step 4: Build then branch
                // ״§„…״µ״¯״±: IfStmt::thenBranch (statements.h:108)
                // ========================================================================
                currentBlock_ = thenBlock;
                if (ifStmt->thenBranch)
                {
                    buildStatement(ifStmt->thenBranch.get());
                }

                // (AR) ‚״² ״÷״± ״´״±״· ״¥„‰ merge (sir_instruction.h:178-183)
                // (EN) Unconditional jump to merge
                // (AR) „״§ †״¶ ״§„‚״² ״¥״°״§ ƒ״§† ״§„״±״¹ ‚״¯ ״§†״×‡‰ ״¨€ RET ״£ˆ BR ״£ˆ BR_COND
                // (EN) Don't add branch if the block already ends with RET or BR or BR_COND
                SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
                SIRInstruction brMergeInst = SIRInstruction::Branch(mergeLabelOp);

                if (currentBlock_ && !currentBlock_->instructions.empty())
                {
                    const auto &lastInst = currentBlock_->instructions.back();
                    bool hasTerminator = (lastInst.opcode == SIROpcode::RET ||
                                          lastInst.opcode == SIROpcode::RET_VOID ||
                                          lastInst.opcode == SIROpcode::BR ||
                                          lastInst.opcode == SIROpcode::BR_COND);
                    if (!hasTerminator)
                    {
                        currentBlock_->instructions.push_back(brMergeInst);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildIfStatement: added BR to merge from then" << std::endl;
#endif
                    }
                    else
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildIfStatement: then block already has terminator, skipping BR" << std::endl;
#endif
                    }
                }
                else if (currentBlock_)
                {
                    currentBlock_->instructions.push_back(brMergeInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildIfStatement: added BR to merge from then (empty block)" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 5: ״¨†״§״¡ ״±״¹ else ״¥״°״§ ˆ״¬״¯
                // (EN) Step 5: Build else branch if exists
                // ״§„…״µ״¯״±: IfStmt::elseBranch (statements.h:109)
                // ========================================================================
                if (ifStmt->elseBranch && elseBlock)
                {
                    currentBlock_ = elseBlock;
                    buildStatement(ifStmt->elseBranch.get());

                    // (AR) ‚״² ״÷״± ״´״±״· ״¥„‰ merge
                    // (EN) Unconditional jump to merge
                    // (AR) „״§ †״¶ ״§„‚״² ״¥״°״§ ƒ״§† ״§„״±״¹ ‚״¯ ״§†״×‡‰ ״¨€ RET ״£ˆ BR ״£ˆ BR_COND
                    // (EN) Don't add branch if the block already ends with RET or BR or BR_COND
                    if (currentBlock_ && !currentBlock_->instructions.empty())
                    {
                        const auto &lastInst = currentBlock_->instructions.back();
                        bool hasTerminator = (lastInst.opcode == SIROpcode::RET ||
                                              lastInst.opcode == SIROpcode::RET_VOID ||
                                              lastInst.opcode == SIROpcode::BR ||
                                              lastInst.opcode == SIROpcode::BR_COND);
                        if (!hasTerminator)
                        {
                            currentBlock_->instructions.push_back(brMergeInst);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildIfStatement: added BR to merge from else" << std::endl;
#endif
                        }
                        else
                        {
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildIfStatement: else block already has terminator, skipping BR" << std::endl;
#endif
                        }
                    }
                    else if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(brMergeInst);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildIfStatement: added BR to merge from else (empty block)" << std::endl;
#endif
                    }
                }

                // ========================================================================
                // (AR) ״§„״®״·ˆ״© 6: ״§„״§״³״×…״±״§״± ״¨״¹״¯ if
                // (EN) Step 6: Continue after if statement
                // ========================================================================
                currentBlock_ = mergeBlock;
#ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: completed, now at merge block" << std::endl;
#endif
            }

            // ============================================================================
            // buildMatchStatement - ״¨†״§״¡ ״¬…„״© match (Pattern Matching)
            // ============================================================================
            // (AR) ״×״­ˆ„ ״¬…„״© match ״¥„‰ ״³„״³„״© …† BR_COND/BR ״¨״§״³״×״®״¯״§… SIR ״§„…ˆ״¬ˆ״¯
            // (EN) Lower match statement to chain of BR_COND/BR using existing SIR
            //
            // (AR) ״§„״§״³״×״±״§״×״¬״©:
            // „ƒ„ case:
            //   1. ƒ״×„״© ״§״®״×״¨״§״±: …‚״§״±†״© ״§„†…״· …״¹ ״§„‚…״©
            //   2. ƒ״×„״© guard (״¥† ˆ״¬״¯): ״×‚… ״§„״´״±״· ״§„״¥״¶״§
            //   3. ƒ״×„״© ״§„״¬״³…: ״×†״° ״§„ƒˆ״¯
            //   4. ‚״² ״¥„‰ ƒ״×„״© ״§„†‡״§״©
            //
            // (EN) Strategy:
            // For each case:
            //   1. Test block: compare pattern with value
            //   2. Guard block (if exists): evaluate guard condition
            //   3. Body block: execute code
            //   4. Jump to merge block
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
