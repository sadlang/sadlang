// ============================================================================
// sir_builder_control_match.cpp
// (AR) Pattern matching: buildMatchStatement
// (EN) Pattern matching: match/when statement compilation
// ============================================================================

#include <string>
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
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            void SIRBuilder::buildMatchStatement(Sad::AST::MatchStmt *matchStmt)
            {
                if (!matchStmt)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: starting with "
                          << matchStmt->cases.size() << " cases" << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 1: تقييم القيمة المُطابقة
                // (EN) Step 1: Evaluate the match value
                // ========================================================================
                auto matchResult = buildExpression(matchStmt->value.get());

                if (matchResult.registerName.empty() && !matchResult.isConstant)
                {
                    errors_.push_back("Error: Failed to build match expression");
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: match value reg="
                          << matchResult.registerName << ", isConst=" << matchResult.isConstant << std::endl;
#endif

                // (AR) إذا كانت القيمة ثابتة، نحتاج تحميلها في سجل
                // (EN) If value is constant, need to load it into a register
                std::string matchValueReg = matchResult.registerName;
                SadTypeKind matchValueType = matchResult.type;

                if (matchResult.isConstant)
                {
                    matchValueReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(matchValueReg, matchResult.type);
                    SIROperand constOp;
                    if (matchResult.type == SadTypeKind::String)
                    {
                        constOp = SIROperand::ConstantString(matchResult.constantValue);
                    }
                    else if (matchResult.type == SadTypeKind::Float)
                    {
                        constOp = SIROperand::ConstantF64(std::stod(matchResult.constantValue));
                    }
                    else if (matchResult.type == SadTypeKind::Boolean)
                    {
                        constOp = SIROperand::ConstantBool(matchResult.constantValue == "true");
                    }
                    else
                    {
                        constOp = SIROperand::ConstantI64(std::stoll(matchResult.constantValue));
                    }
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = resultOp;
                    moveInst.operands = {constOp};
                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(moveInst);
                    }
                }
                else if (!matchResult.registerName.empty() && matchResult.registerName[0] == '%')
                {
                    // (AR) القيمة في عنوان alloca (متغير)، نحتاج لتحميلها
                    // (EN) Value is in alloca address (variable), need to LOAD it
                    std::string loadedReg = newTempRegister();
                    SIRInstruction loadInst;
                    loadInst.opcode = SIROpcode::LOAD;
                    loadInst.result = SIROperand::Register(loadedReg, matchValueType);
                    loadInst.operands.push_back(SIROperand::Register(matchValueReg, matchValueType));
                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(loadInst);
                    }
                    matchValueReg = loadedReg;
                }

                // ========================================================================
                // (AR) الخطوة 2: إنشاء كتلة النهاية
                // (EN) Step 2: Create merge block
                // ========================================================================
                std::string mergeLabel = newLabel("match.end");
                auto mergeBlock = createBasicBlock(mergeLabel);
                if (currentFunction_)
                {
                    currentFunction_->addBasicBlock(mergeBlock);
                }

                // ========================================================================
                // (AR) الخطوة 3: إنشاء كتل لكل case
                // (EN) Step 3: Create blocks for each case
                // ========================================================================
                struct CaseBlockInfo
                {
                    std::string testLabel;
                    std::string bodyLabel;
                    std::string guardLabel;
                    std::shared_ptr<SIRBasicBlock> testBlock;
                    std::shared_ptr<SIRBasicBlock> bodyBlock;
                    std::shared_ptr<SIRBasicBlock> guardBlock;
                };

                // (AR) لكل case: قائمة بالحقول المؤجل استخراجها (تستخدم MatchDeferredField من الهيدر)
                // (EN) For each case: list of fields to extract later (uses MatchDeferredField from header)
                std::vector<std::vector<MatchDeferredField>> deferredADTExtractions(matchStmt->cases.size());

                std::vector<CaseBlockInfo> caseBlocks;

                for (size_t i = 0; i < matchStmt->cases.size(); ++i)
                {
                    CaseBlockInfo info;
                    info.testLabel = newLabel("match.case" + std::to_string(i) + ".test");
                    info.bodyLabel = newLabel("match.case" + std::to_string(i) + ".body");
                    info.testBlock = createBasicBlock(info.testLabel);
                    info.bodyBlock = createBasicBlock(info.bodyLabel);

                    if (currentFunction_)
                    {
                        currentFunction_->addBasicBlock(info.testBlock);
                        currentFunction_->addBasicBlock(info.bodyBlock);
                    }

                    // (AR) كتلة guard إن وُجد
                    // (EN) Guard block if exists
                    if (matchStmt->cases[i].guard)
                    {
                        info.guardLabel = newLabel("match.case" + std::to_string(i) + ".guard");
                        info.guardBlock = createBasicBlock(info.guardLabel);
                        if (currentFunction_)
                        {
                            currentFunction_->addBasicBlock(info.guardBlock);
                        }
                    }

                    caseBlocks.push_back(std::move(info));
                }

                // ========================================================================
                // (AR) الخطوة 4: القفز للـ case الأول
                // (EN) Step 4: Jump to first case
                // ========================================================================
                if (!caseBlocks.empty())
                {
                    SIROperand firstLabel = SIROperand::Label(caseBlocks[0].testLabel);
                    SIRInstruction brFirst = SIRInstruction::Branch(firstLabel);
                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(brFirst);
                    }
                }
                else
                {
                    // (AR) لا يوجد cases، القفز مباشرة للنهاية
                    // (EN) No cases, jump directly to merge
                    SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
                    SIRInstruction brMerge = SIRInstruction::Branch(mergeLabelOp);
                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(brMerge);
                    }
                    currentBlock_ = mergeBlock;
                    return;
                }

                // ========================================================================
                // (AR) الخطوة 5: توليد كود كل case
                // (EN) Step 5: Generate code for each case
                // ========================================================================
                for (size_t i = 0; i < matchStmt->cases.size(); ++i)
                {
                    const auto &caseClause = matchStmt->cases[i];
                    auto &info = caseBlocks[i];

                    // (AR) تحديد الـ case التالي للقفز عند الفشل
                    // (EN) Determine next case to jump to on failure
                    std::string nextLabel = (i + 1 < caseBlocks.size())
                                                ? caseBlocks[i + 1].testLabel
                                                : mergeLabel;

                    // === كتلة الاختبار / Test block ===
                    currentBlock_ = info.testBlock;

                    std::string condReg;

                    if (caseClause.pattern)
                    {
                        // (AR) توليد شرط النمط عبر دالة مساعدة — CW-05
                        // (EN) Generate pattern condition via helper — CW-05
                        condReg = buildMatchPatternCondition(
                            caseClause.pattern.get(),
                            matchValueReg,
                            matchValueType,
                            i,
                            deferredADTExtractions[i]);
                    } // end if (caseClause.pattern)

                    // === التفريع / Branching ===
                    if (caseClause.guard && !info.guardLabel.empty())
                    {
                        // (AR) القفز إلى كتلة guard عند نجاح الاختبار
                        // (EN) Jump to guard block on test success
                        SIROperand condOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                        SIROperand guardLabelOp = SIROperand::Label(info.guardLabel);
                        SIROperand nextLabelOp = SIROperand::Label(nextLabel);
                        SIRInstruction brCond = SIRInstruction::BranchCond(condOp, guardLabelOp, nextLabelOp);
                        currentBlock_->instructions.push_back(brCond);

                        // === كتلة guard / Guard block ===
                        currentBlock_ = info.guardBlock;
                        enterScope();

                        // (AR) ربط المتغيرات من النمط قبل تقييم الشرط
                        // (EN) Bind pattern variables before evaluating guard
                        if (auto *varPat = dynamic_cast<const Sad::AST::VariablePattern *>(caseClause.pattern.get()))
                        {
                            VariableInfo guardVar;
                            guardVar.name = varPat->name;
                            guardVar.type = matchValueType;
                            guardVar.registerName = matchValueReg;
                            guardVar.isGlobal = false;
                            guardVar.isMutable = false;
                            guardVar.scopeLevel = currentScopeLevel_;
                            addVariable(guardVar);
                        }

                        auto guardResult = buildExpression(caseClause.guard.get());

                        std::string guardCondReg = guardResult.registerName;
                        if (guardResult.isConstant)
                        {
                            guardCondReg = newTempRegister();
                            SIROperand gResultOp = SIROperand::Register(guardCondReg, guardResult.type);
                            SIROperand gConstOp;
                            if (guardResult.type == SadTypeKind::Boolean)
                            {
                                gConstOp = SIROperand::ConstantBool(guardResult.constantValue == "true");
                            }
                            else if (guardResult.type == SadTypeKind::Integer)
                            {
                                gConstOp = SIROperand::ConstantI64(std::stoll(guardResult.constantValue));
                            }
                            else
                            {
                                gConstOp = SIROperand::ConstantI64(guardResult.constantValue == "true" ? 1 : 0);
                            }
                            SIRInstruction moveG(SIROpcode::MOVE);
                            moveG.result = gResultOp;
                            moveG.operands = {gConstOp};
                            currentBlock_->instructions.push_back(moveG);
                        }

                        SIROperand guardCondOp = SIROperand::Register(guardCondReg, SadTypeKind::Boolean);
                        SIROperand bodyLabelOp = SIROperand::Label(info.bodyLabel);
                        SIROperand nextLabelOp2 = SIROperand::Label(nextLabel);
                        SIRInstruction brGuard = SIRInstruction::BranchCond(guardCondOp, bodyLabelOp, nextLabelOp2);
                        currentBlock_->instructions.push_back(brGuard);

                        exitScope();
                    }
                    else
                    {
                        // (AR) بدون guard: القفز مباشرة إلى الجسم أو التالي
                        // (EN) No guard: jump directly to body or next
                        SIROperand condOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                        SIROperand bodyLabelOp = SIROperand::Label(info.bodyLabel);
                        SIROperand nextLabelOp = SIROperand::Label(nextLabel);
                        SIRInstruction brCond = SIRInstruction::BranchCond(condOp, bodyLabelOp, nextLabelOp);
                        currentBlock_->instructions.push_back(brCond);
                    }

                    // === كتلة الجسم / Body block ===
                    currentBlock_ = info.bodyBlock;
                    enterScope();

                    // (AR) ربط المتغيرات من النمط في الجسم
                    // (EN) Bind pattern variables in body scope
                    if (auto *varPat = dynamic_cast<const Sad::AST::VariablePattern *>(caseClause.pattern.get()))
                    {
                        VariableInfo bodyVar;
                        bodyVar.name = varPat->name;
                        bodyVar.type = matchValueType;
                        bodyVar.registerName = matchValueReg;
                        bodyVar.isGlobal = false;
                        bodyVar.isMutable = false;
                        bodyVar.scopeLevel = currentScopeLevel_;
                        addVariable(bodyVar);
                    }

                    // (AR) === استخراج حقول ADT المؤجلة ===
                    //      هذه الحقول تم تأجيل استخراجها من كتلة الاختبار إلى هنا
                    //      لأن كتلة الجسم تنفَّذ فقط عندما يتطابق المميّز (tag)
                    //      مما يضمن أن الحقول صالحة وموجودة في البنية
                    // (EN) === Deferred ADT field extraction ===
                    //      These fields were deferred from testBlock to here
                    //      because bodyBlock executes only when tag matches
                    //      ensuring fields are valid and present in the struct
                    if (!deferredADTExtractions[i].empty())
                    {
                        for (const auto &deferred : deferredADTExtractions[i])
                        {
                            std::string fieldReg = newTempRegister();
                            SIRInstruction getPayload(SIROpcode::ENUM_GET_PAYLOAD);
                            getPayload.result = SIROperand::Register(fieldReg, SadTypeKind::Integer);
                            getPayload.operands.push_back(
                                SIROperand::Register(matchValueReg, matchValueType));
                            getPayload.operands.push_back(
                                SIROperand::ConstantI64(static_cast<int64_t>(deferred.fieldIndex)));
                            // (AR) المعامل [2]: اسم التعداد للبحث عن البنية عبر حدود الدوال
                            // (EN) Operand [2]: enum name for struct lookup across function boundaries
                            getPayload.operands.push_back(
                                SIROperand::ConstantString(deferred.enumName));
                            getPayload.comment = "Deferred extract: field " +
                                                 std::to_string(deferred.fieldIndex) + " (" + deferred.fieldName +
                                                 ") → " + deferred.varName;
                            if (currentBlock_)
                                currentBlock_->addInstruction(getPayload);

                            // (AR) ربط المتغير بقيمة الحقل المستخرج
                            // (EN) Bind variable to extracted field value
                            VariableInfo fieldVarInfo;
                            fieldVarInfo.name = deferred.varName;
                            fieldVarInfo.type = SadTypeKind::Integer;
                            fieldVarInfo.registerName = fieldReg;
                            fieldVarInfo.isGlobal = false;
                            fieldVarInfo.isMutable = false;
                            fieldVarInfo.scopeLevel = currentScopeLevel_;
                            addVariable(fieldVarInfo);
                        }
                    }

                    // (AR) تنفيذ جسم الحالة
                    // (EN) Execute case body
                    for (auto &bodyStmt : caseClause.body)
                    {
                        buildStatement(bodyStmt.get());
                    }

                    exitScope();

                    // (AR) قفز غير مشروط إلى كتلة النهاية
                    // (EN) Unconditional jump to merge block
                    SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
                    SIRInstruction brMerge = SIRInstruction::Branch(mergeLabelOp);
                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(brMerge);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: generated case " << i << std::endl;
#endif
                } // end for each case

                // ========================================================================
                // (AR) الخطوة 5.5: فحص شمولية المطابقة (Exhaustiveness Check)
                //      إذا كانت القيمة المُطابقة من نوع تعداد جبري (ADT)
                //      نتحقق أن جميع حالات التعداد مغطاة أو يوجد نمط شامل (_/افتراضي)
                //      هذا يمنع السلوك غير المعرف عندما لا يتطابق أي نمط
                // (EN) Step 5.5: Exhaustiveness Check
                //      If match value is an ADT enum type, verify all variants are covered
                //      or a wildcard/default pattern exists. Prevents undefined behavior.
                // ========================================================================
                {
                    bool hasWildcard = false;
                    bool hasVariablePattern = false;
                    std::set<std::string> coveredVariants;
                    std::string adtEnumName;

                    for (size_t i = 0; i < matchStmt->cases.size(); ++i)
                    {
                        const auto &caseClause = matchStmt->cases[i];
                        if (!caseClause.pattern)
                            continue;

                        if (dynamic_cast<const Sad::AST::WildcardPattern *>(caseClause.pattern.get()))
                        {
                            hasWildcard = true;
                            break;
                        }
                        if (dynamic_cast<const Sad::AST::VariablePattern *>(caseClause.pattern.get()))
                        {
                            hasVariablePattern = true;
                            break;
                        }
                        if (auto *enumPat = dynamic_cast<const Sad::AST::EnumVariantPattern *>(caseClause.pattern.get()))
                        {
                            adtEnumName = enumPat->enumName;
                            coveredVariants.insert(enumPat->variantName);
                        }
                    }

                    // (AR) إذا كان تعداد ADT ولا يوجد نمط شامل — نتحقق من التغطية
                    // (EN) If ADT enum and no wildcard — check coverage
                    if (!hasWildcard && !hasVariablePattern && !adtEnumName.empty())
                    {
                        auto adtIt = adtEnumTable_.find(adtEnumName);
                        if (adtIt != adtEnumTable_.end())
                        {
                            const ADTEnumInfo &adtInfo = adtIt->second;
                            std::vector<std::string> missingVariants;

                            for (const auto &variant : adtInfo.variants)
                            {
                                if (coveredVariants.find(variant.name) == coveredVariants.end())
                                {
                                    missingVariants.push_back(variant.name);
                                }
                            }

                            if (!missingVariants.empty())
                            {
                                // (AR) تحذير: حالات غير مغطاة — قد يحدث سلوك غير متوقع
                                // (EN) Warning: uncovered variants — may cause unexpected behavior
                                std::string warning = "Warning: Non-exhaustive match on enum '" +
                                                      adtEnumName + "'. Missing variants: ";
                                for (size_t m = 0; m < missingVariants.size(); ++m)
                                {
                                    if (m > 0)
                                        warning += ", ";
                                    warning += adtEnumName + "." + missingVariants[m];
                                }
                                errors_.push_back(warning);

#ifndef NDEBUG
                                std::cout << "[WARNING] " << warning << std::endl;
#endif
                            }
                        }
                    }
                }

                // ========================================================================
                // (AR) الخطوة 6: الاستمرار بعد match
                // (EN) Step 6: Continue after match statement
                // ========================================================================
                currentBlock_ = mergeBlock;
#ifndef NDEBUG
                std::cout << "[DEBUG] buildMatchStatement: completed" << std::endl;
#endif
            }

            // ============================================================================
            // buildWhileLoop - بناء حلقة while
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:393
            // التوقيع / Signature: void buildWhileLoop(AST::WhileStmt* whileLoop);
            //
            // المعاملات / Parameters:
            // - whileLoop: AST::WhileStmt* = Sad::AST::WhileStmt* (sir_builder.h:393)
            //
            // WhileStmt Members (statements.h:143-175):
            // - condition: ExprPtr (line 149)
            // - body: StmtPtr (line 150)
            //
            // المتغيرات المستخدمة / Used variables:
            // - currentBlock_: sir_builder.h:582 (shared_ptr<SIRBasicBlock>)
            //
            // الدوال المستدعاة / Called functions:
            // - buildExpression: sir_builder.h:432
            // - buildStatement: sir_builder.h:372
            // - createBasicBlock: sir_builder.h:501
            // - newLabel: sir_builder.h:520
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad