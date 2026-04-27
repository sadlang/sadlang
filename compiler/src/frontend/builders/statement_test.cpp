// ============================================================================
// sir_builder_stmt_test.cpp
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

            bool StatementBuilder::buildStatement_Test(AST::Statement *stmt)
            {
                // ========================================================================
                // (AR) TestDecl - إعلان اختبار (declarations.h:TestDecl)
                //      يدعم نوعين:
                //      1. اختبار عادي: اختبر("اسم") ... نهاية → دالة __test_<name>
                //      2. اختبار خصائص: اختبر("اسم") تكرارات N بذرة S ... نهاية
                //         → دالة __test_<name> مع حلقة تكرار ومتغيرات خاصة:
                //           _تكرار، _التكرار_الحالي، _إجمالي_التكرارات، _بذرة
                //           _عشوائي، _عشوائي_صحيح، _عشوائي_سالب
                // (EN) Test declaration — supports two types:
                //      1. Normal test: lowered to __test_<name> function
                //      2. Property test: __test_<name> with iteration loop and special variables
                // ========================================================================
                if (auto testDecl = dynamic_cast<Sad::AST::TestDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TestDecl: " << testDecl->testName
                              << (testDecl->isPropertyTest ? " (property test, iterations=" + std::to_string(testDecl->iterations) + ")" : "")
                              << std::endl;
#endif

                    // (AR) بناء دالة اختبار: __test_<name>
                    // (EN) Build test function: __test_<name>
                    std::string testFuncName = "__test_" + testDecl->testName;
                    auto testFunc = std::make_shared<SIRFunction>(testFuncName, SadTypeKind::Boolean);

                    auto entryBlock = b_.createBasicBlock("test_entry");
                    testFunc->addBasicBlock(entryBlock);

                    auto savedCtx = b_.saveContext();
                    b_.currentFunction_ = testFunc;
                    b_.currentBlock_ = entryBlock;

                    b_.enterScope();

                    // ================================================================
                    // (AR) اختبار الخصائص (Property Testing): تكرار الجسم N مرة
                    //      مع تعريف متغيرات خاصة في كل تكرار
                    //      يتوافق مع StatementExecutor::visitTestDecl في المفسر
                    // (EN) Property Testing: repeat body N times
                    //      with special iteration variables defined per iteration
                    //      mirrors StatementExecutor::visitTestDecl in interpreter
                    // ================================================================
                    if (testDecl->isPropertyTest && testDecl->iterations > 0)
                    {
                        // (AR) متغير العداد: _iter_counter = 0
                        // (EN) Counter variable: _iter_counter = 0
                        std::string counterReg = b_.newTempRegister();
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::Store(
                                    SIROperand::ConstantI64(0),
                                    SIROperand::Register(counterReg, SadTypeKind::Integer)));
                        }
                        VariableInfo counterVar;
                        counterVar.name = "__iter_counter";
                        counterVar.type = SadTypeKind::Integer;
                        counterVar.registerName = counterReg;
                        counterVar.isMutable = true;
                        counterVar.scopeLevel = b_.currentScopeLevel_;
                        b_.addVariable(counterVar);

                        // (AR) إعداد الحلقة: header → body → increment → exit
                        // (EN) Setup loop: header → body → increment → exit
                        std::string headerLabel = b_.newLabel("prop_header");
                        std::string bodyLabel = b_.newLabel("prop_body");
                        std::string incrLabel = b_.newLabel("prop_incr");
                        std::string exitLabel = b_.newLabel("prop_exit");

                        // (AR) قفز إلى header
                        // (EN) Branch to header
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(headerLabel)));
                        }

                        // (AR) كتلة Header: فحص العداد < iterations
                        // (EN) Header block: check counter < iterations
                        auto headerBlock = b_.createBasicBlock(headerLabel);
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(headerBlock);
                        b_.currentBlock_ = headerBlock;

                        std::string cmpReg = b_.newTempRegister();
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::Binary(
                                    SIROpcode::LT,
                                    SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                    SIROperand::Register(counterReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(testDecl->iterations)));
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::BranchCond(
                                    SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                    SIROperand::Label(bodyLabel),
                                    SIROperand::Label(exitLabel)));
                        }

                        // (AR) كتلة Body: تعريف المتغيرات الخاصة + تنفيذ الجسم
                        // (EN) Body block: define special variables + execute body
                        auto bodyBlock = b_.createBasicBlock(bodyLabel);
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(bodyBlock);
                        b_.currentBlock_ = bodyBlock;

                        b_.enterScope();

                        // (AR) تعريف _تكرار = counter
                        // (EN) Define _تكرار = counter
                        {
                            VariableInfo vi;
                            vi.name = "_\xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1"; // _تكرار
                            vi.type = SadTypeKind::Integer;
                            vi.registerName = counterReg;
                            vi.isMutable = false;
                            vi.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(vi);
                        }

                        // (AR) تعريف _التكرار_الحالي = counter + 1
                        // (EN) Define _التكرار_الحالي = counter + 1
                        {
                            std::string curIterReg = b_.newTempRegister();
                            if (b_.currentBlock_)
                            {
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::ADD_I64,
                                        SIROperand::Register(curIterReg, SadTypeKind::Integer),
                                        SIROperand::Register(counterReg, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1)));
                            }
                            VariableInfo vi;
                            vi.name = "_\xD8\xA7\xD9\x84\xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1_\xD8\xA7\xD9\x84\xD8\xAD\xD8\xA7\xD9\x84\xD9\x8A"; // _التكرار_الحالي
                            vi.type = SadTypeKind::Integer;
                            vi.registerName = curIterReg;
                            vi.isMutable = false;
                            vi.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(vi);
                        }

                        // (AR) تعريف _إجمالي_التكرارات = iterations
                        // (EN) Define _إجمالي_التكرارات = iterations
                        {
                            std::string totalReg = b_.newTempRegister();
                            if (b_.currentBlock_)
                            {
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Store(
                                        SIROperand::ConstantI64(testDecl->iterations),
                                        SIROperand::Register(totalReg, SadTypeKind::Integer)));
                            }
                            VariableInfo vi;
                            vi.name = "_\xD8\xA5\xD8\xAC\xD9\x85\xD8\xA7\xD9\x84\xD9\x8A_\xD8\xA7\xD9\x84\xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1\xD8\xA7\xD8\xAA"; // _إجمالي_التكرارات
                            vi.type = SadTypeKind::Integer;
                            vi.registerName = totalReg;
                            vi.isMutable = false;
                            vi.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(vi);
                        }

                        // (AR) تعريف _بذرة = seed + counter (بذرة مختلفة لكل تكرار)
                        //      LCG بسيط: rng = (seed + i) * 1664525 + 1013904223
                        // (EN) Define _بذرة = seed + counter (different seed per iteration)
                        //      Simple LCG: rng = (seed + i) * 1664525 + 1013904223
                        {
                            int baseSeed = (testDecl->seed >= 0) ? testDecl->seed : 0;
                            std::string seedReg = b_.newTempRegister();
                            if (b_.currentBlock_)
                            {
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::ADD_I64,
                                        SIROperand::Register(seedReg, SadTypeKind::Integer),
                                        SIROperand::Register(counterReg, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(baseSeed)));
                            }
                            VariableInfo vi;
                            vi.name = "_\xD8\xA8\xD8\xB0\xD8\xB1\xD8\xA9"; // _بذرة
                            vi.type = SadTypeKind::Integer;
                            vi.registerName = seedReg;
                            vi.isMutable = false;
                            vi.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(vi);

                            // (AR) _عشوائي_صحيح = (seed * 1664525 + 1013904223) % 1000
                            // (EN) _عشوائي_صحيح = (seed * 1664525 + 1013904223) % 1000
                            std::string rng1 = b_.newTempRegister();
                            std::string rng2 = b_.newTempRegister();
                            std::string randIntReg = b_.newTempRegister();
                            if (b_.currentBlock_)
                            {
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::MUL_I64,
                                        SIROperand::Register(rng1, SadTypeKind::Integer),
                                        SIROperand::Register(seedReg, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1664525)));
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::ADD_I64,
                                        SIROperand::Register(rng2, SadTypeKind::Integer),
                                        SIROperand::Register(rng1, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1013904223)));
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::MOD_I64,
                                        SIROperand::Register(randIntReg, SadTypeKind::Integer),
                                        SIROperand::Register(rng2, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1000)));
                            }
                            VariableInfo viRandInt;
                            viRandInt.name = "_\xD8\xB9\xD8\xB4\xD9\x88\xD8\xA7\xD8\xA6\xD9\x8A_\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD"; // _عشوائي_صحيح
                            viRandInt.type = SadTypeKind::Integer;
                            viRandInt.registerName = randIntReg;
                            viRandInt.isMutable = false;
                            viRandInt.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(viRandInt);

                            // (AR) _عشوائي_سالب = (rng % 2001) - 1000
                            // (EN) _عشوائي_سالب = (rng % 2001) - 1000
                            std::string modReg = b_.newTempRegister();
                            std::string randNegReg = b_.newTempRegister();
                            if (b_.currentBlock_)
                            {
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::MOD_I64,
                                        SIROperand::Register(modReg, SadTypeKind::Integer),
                                        SIROperand::Register(rng2, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(2001)));
                                b_.currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::SUB_I64,
                                        SIROperand::Register(randNegReg, SadTypeKind::Integer),
                                        SIROperand::Register(modReg, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1000)));
                            }
                            VariableInfo viRandNeg;
                            viRandNeg.name = "_\xD8\xB9\xD8\xB4\xD9\x88\xD8\xA7\xD8\xA6\xD9\x8A_\xD8\xB3\xD8\xA7\xD9\x84\xD8\xA8"; // _عشوائي_سالب
                            viRandNeg.type = SadTypeKind::Integer;
                            viRandNeg.registerName = randNegReg;
                            viRandNeg.isMutable = false;
                            viRandNeg.scopeLevel = b_.currentScopeLevel_;
                            b_.addVariable(viRandNeg);
                        }

                        // (AR) بناء جسم الاختبار
                        // (EN) Build test body
                        if (testDecl->body)
                        {
                            buildStatement(testDecl->body.get());
                        }

                        b_.exitScope();

                        // (AR) قفز إلى increment
                        // (EN) Branch to increment
                        if (b_.currentBlock_ && !b_.currentBlock_->getTerminator())
                        {
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(incrLabel)));
                        }

                        // (AR) كتلة Increment: counter++
                        // (EN) Increment block: counter++
                        auto incrBlock = b_.createBasicBlock(incrLabel);
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(incrBlock);
                        b_.currentBlock_ = incrBlock;

                        std::string newCounterReg = b_.newTempRegister();
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::Binary(
                                    SIROpcode::ADD_I64,
                                    SIROperand::Register(newCounterReg, SadTypeKind::Integer),
                                    SIROperand::Register(counterReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(1)));
                            // (AR) تحديث العداد
                            // (EN) Update counter
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::Store(
                                    SIROperand::Register(newCounterReg, SadTypeKind::Integer),
                                    SIROperand::Register(counterReg, SadTypeKind::Integer)));
                            b_.currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(headerLabel)));
                        }

                        // (AR) كتلة Exit: إرجاع true (نجح)
                        // (EN) Exit block: return true (passed)
                        auto exitBlock = b_.createBasicBlock(exitLabel);
                        if (b_.currentFunction_)
                            b_.currentFunction_->addBasicBlock(exitBlock);
                        b_.currentBlock_ = exitBlock;

                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::Return(
                                SIROperand::ConstantBool(true)));
                        }
                    }
                    else
                    {
                        // ================================================================
                        // (AR) اختبار عادي — جسم واحد بدون حلقة
                        // (EN) Normal test — single body without loop
                        // ================================================================
                        if (testDecl->body)
                        {
                            buildStatement(testDecl->body.get());
                        }

                        // (AR) إرجاع true/false حسب shouldFail
                        // (EN) Return true/false based on shouldFail
                        if (b_.currentBlock_ && !b_.currentBlock_->getTerminator())
                        {
                            if (testDecl->shouldFail)
                            {
                                b_.currentBlock_->addInstruction(SIRInstruction::Return(
                                    SIROperand::ConstantBool(false)));
                            }
                            else
                            {
                                b_.currentBlock_->addInstruction(SIRInstruction::Return(
                                    SIROperand::ConstantBool(true)));
                            }
                        }
                    }

                    b_.exitScope();

                    b_.restoreContext(std::move(savedCtx));
                    b_.module_->addFunction(testFunc);

                    // (AR) تسجيل دالة الاختبار في جدول الدوال
                    // (EN) Register test function in function table
                    FunctionInfo fi;
                    fi.name = testFuncName;
                    fi.returnType = SadTypeKind::Boolean;
                    fi.sirFunction = testFunc;
                    b_.functionTable_[testFuncName] = fi;

                    // ================================================================
                    // (AR) حقن استدعاء دالة الاختبار في تدفق main + طباعة النتيجة
                    //      يتوافق مع سلوك المفسر: visitTestDecl يُنفذ فوراً ويطبع
                    //      رسالة "✓ اسم_الاختبار (N/N تكرار نجح)" عند النجاح
                    // (EN) Inject test call into main flow + print result
                    //      Matches interpreter: visitTestDecl executes immediately and prints
                    //      "✓ test_name (N/N iterations passed)" on success
                    // ================================================================
                    {
                        // (AR) استدعاء دالة الاختبار
                        // (EN) Call the test function
                        std::string callResultReg = b_.newTempRegister();
                        SIRInstruction callInst(SIROpcode::CALL);
                        callInst.result = SIROperand::Register(callResultReg, SadTypeKind::Boolean);
                        callInst.operands.push_back(SIROperand::Function(testFuncName));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(callInst);

                        // (AR) بناء رسالة النجاح
                        // (EN) Build success message
                        if (testDecl->isPropertyTest && testDecl->iterations > 0)
                        {
                            // (AR) طباعة: "✓ اسم_الاختبار (N/N تكرار نجح)"
                            std::string successMsg = "\xe2\x9c\x93 " + testDecl->testName +
                                                     " (" + std::to_string(testDecl->iterations) +
                                                     "/" + std::to_string(testDecl->iterations) +
                                                     " \xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1 \xD9\x86\xD8\xAC\xD8\xAD)";
                            std::string msgReg = b_.newTempRegister();
                            SIRInstruction strInst(SIROpcode::STRING_NEW);
                            strInst.result = SIROperand::Register(msgReg, SadTypeKind::String);
                            strInst.operands.push_back(SIROperand::ConstantString(successMsg));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(strInst);

                            SIRInstruction printInst(SIROpcode::BUILTIN_PRINT);
                            printInst.operands.push_back(SIROperand::Register(msgReg, SadTypeKind::String));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(printInst);

                            // (AR) طباعة سطر جديد
                            // (EN) Print newline
                            SIRInstruction nlInst(SIROpcode::BUILTIN_PRINT);
                            nlInst.operands.push_back(SIROperand::ConstantString("\n"));
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(nlInst);
                        }
                    }

                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
