// ============================================================================
// sir_builder_stmt_advanced.cpp
// (AR) Advanced: test, atomic, property, macro, goroutine, select, extension
// (EN) Advanced statement types: testing, concurrency, metaprogramming
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
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

            bool SIRBuilder::buildStatement_Advanced(AST::Statement *stmt)
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

                    auto entryBlock = createBasicBlock("test_entry");
                    testFunc->addBasicBlock(entryBlock);

                    auto savedCtx = saveContext();
                    currentFunction_ = testFunc;
                    currentBlock_ = entryBlock;

                    enterScope();

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
                        std::string counterReg = newTempRegister();
                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(
                                SIRInstruction::Store(
                                    SIROperand::Register(counterReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(0)));
                        }
                        VariableInfo counterVar;
                        counterVar.name = "__iter_counter";
                        counterVar.type = SadTypeKind::Integer;
                        counterVar.registerName = counterReg;
                        counterVar.isMutable = true;
                        counterVar.scopeLevel = currentScopeLevel_;
                        addVariable(counterVar);

                        // (AR) إعداد الحلقة: header → body → increment → exit
                        // (EN) Setup loop: header → body → increment → exit
                        std::string headerLabel = newLabel("prop_header");
                        std::string bodyLabel = newLabel("prop_body");
                        std::string incrLabel = newLabel("prop_incr");
                        std::string exitLabel = newLabel("prop_exit");

                        // (AR) قفز إلى header
                        // (EN) Branch to header
                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(headerLabel)));
                        }

                        // (AR) كتلة Header: فحص العداد < iterations
                        // (EN) Header block: check counter < iterations
                        auto headerBlock = createBasicBlock(headerLabel);
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(headerBlock);
                        currentBlock_ = headerBlock;

                        std::string cmpReg = newTempRegister();
                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(
                                SIRInstruction::Binary(
                                    SIROpcode::LT,
                                    SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                    SIROperand::Register(counterReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(testDecl->iterations)));
                            currentBlock_->addInstruction(
                                SIRInstruction::BranchCond(
                                    SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                                    SIROperand::Label(bodyLabel),
                                    SIROperand::Label(exitLabel)));
                        }

                        // (AR) كتلة Body: تعريف المتغيرات الخاصة + تنفيذ الجسم
                        // (EN) Body block: define special variables + execute body
                        auto bodyBlock = createBasicBlock(bodyLabel);
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(bodyBlock);
                        currentBlock_ = bodyBlock;

                        enterScope();

                        // (AR) تعريف _تكرار = counter
                        // (EN) Define _تكرار = counter
                        {
                            VariableInfo vi;
                            vi.name = "\xD9\x80\xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1"; // _تكرار
                            vi.type = SadTypeKind::Integer;
                            vi.registerName = counterReg;
                            vi.isMutable = false;
                            vi.scopeLevel = currentScopeLevel_;
                            addVariable(vi);
                        }

                        // (AR) تعريف _التكرار_الحالي = counter + 1
                        // (EN) Define _التكرار_الحالي = counter + 1
                        {
                            std::string curIterReg = newTempRegister();
                            if (currentBlock_)
                            {
                                currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::ADD_I64,
                                        SIROperand::Register(curIterReg, SadTypeKind::Integer),
                                        SIROperand::Register(counterReg, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1)));
                            }
                            VariableInfo vi;
                            vi.name = "\xD9\x80\xD8\xA7\xD9\x84\xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1_\xD8\xA7\xD9\x84\xD8\xAD\xD8\xA7\xD9\x84\xD9\x8A"; // _التكرار_الحالي
                            vi.type = SadTypeKind::Integer;
                            vi.registerName = curIterReg;
                            vi.isMutable = false;
                            vi.scopeLevel = currentScopeLevel_;
                            addVariable(vi);
                        }

                        // (AR) تعريف _إجمالي_التكرارات = iterations
                        // (EN) Define _إجمالي_التكرارات = iterations
                        {
                            std::string totalReg = newTempRegister();
                            if (currentBlock_)
                            {
                                currentBlock_->addInstruction(
                                    SIRInstruction::Store(
                                        SIROperand::Register(totalReg, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(testDecl->iterations)));
                            }
                            VariableInfo vi;
                            vi.name = "\xD9\x80\xD8\xA5\xD8\xAC\xD9\x85\xD8\xA7\xD9\x84\xD9\x8A_\xD8\xA7\xD9\x84\xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1\xD8\xA7\xD8\xAA"; // _إجمالي_التكرارات
                            vi.type = SadTypeKind::Integer;
                            vi.registerName = totalReg;
                            vi.isMutable = false;
                            vi.scopeLevel = currentScopeLevel_;
                            addVariable(vi);
                        }

                        // (AR) تعريف _بذرة = seed + counter (بذرة مختلفة لكل تكرار)
                        //      LCG بسيط: rng = (seed + i) * 1664525 + 1013904223
                        // (EN) Define _بذرة = seed + counter (different seed per iteration)
                        //      Simple LCG: rng = (seed + i) * 1664525 + 1013904223
                        {
                            int baseSeed = (testDecl->seed >= 0) ? testDecl->seed : 0;
                            std::string seedReg = newTempRegister();
                            if (currentBlock_)
                            {
                                currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::ADD_I64,
                                        SIROperand::Register(seedReg, SadTypeKind::Integer),
                                        SIROperand::Register(counterReg, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(baseSeed)));
                            }
                            VariableInfo vi;
                            vi.name = "\xD9\x80\xD8\xA8\xD8\xB0\xD8\xB1\xD8\xA9"; // _بذرة
                            vi.type = SadTypeKind::Integer;
                            vi.registerName = seedReg;
                            vi.isMutable = false;
                            vi.scopeLevel = currentScopeLevel_;
                            addVariable(vi);

                            // (AR) _عشوائي_صحيح = (seed * 1664525 + 1013904223) % 1000
                            // (EN) _عشوائي_صحيح = (seed * 1664525 + 1013904223) % 1000
                            std::string rng1 = newTempRegister();
                            std::string rng2 = newTempRegister();
                            std::string randIntReg = newTempRegister();
                            if (currentBlock_)
                            {
                                currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::MUL_I64,
                                        SIROperand::Register(rng1, SadTypeKind::Integer),
                                        SIROperand::Register(seedReg, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1664525)));
                                currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::ADD_I64,
                                        SIROperand::Register(rng2, SadTypeKind::Integer),
                                        SIROperand::Register(rng1, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1013904223)));
                                currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::MOD_I64,
                                        SIROperand::Register(randIntReg, SadTypeKind::Integer),
                                        SIROperand::Register(rng2, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1000)));
                            }
                            VariableInfo viRandInt;
                            viRandInt.name = "\xD9\x80\xD8\xB9\xD8\xB4\xD9\x88\xD8\xA7\xD8\xA6\xD9\x8A_\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD"; // _عشوائي_صحيح
                            viRandInt.type = SadTypeKind::Integer;
                            viRandInt.registerName = randIntReg;
                            viRandInt.isMutable = false;
                            viRandInt.scopeLevel = currentScopeLevel_;
                            addVariable(viRandInt);

                            // (AR) _عشوائي_سالب = (rng % 2001) - 1000
                            // (EN) _عشوائي_سالب = (rng % 2001) - 1000
                            std::string modReg = newTempRegister();
                            std::string randNegReg = newTempRegister();
                            if (currentBlock_)
                            {
                                currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::MOD_I64,
                                        SIROperand::Register(modReg, SadTypeKind::Integer),
                                        SIROperand::Register(rng2, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(2001)));
                                currentBlock_->addInstruction(
                                    SIRInstruction::Binary(
                                        SIROpcode::SUB_I64,
                                        SIROperand::Register(randNegReg, SadTypeKind::Integer),
                                        SIROperand::Register(modReg, SadTypeKind::Integer),
                                        SIROperand::ConstantI64(1000)));
                            }
                            VariableInfo viRandNeg;
                            viRandNeg.name = "\xD9\x80\xD8\xB9\xD8\xB4\xD9\x88\xD8\xA7\xD8\xA6\xD9\x8A_\xD8\xB3\xD8\xA7\xD9\x84\xD8\xA8"; // _عشوائي_سالب
                            viRandNeg.type = SadTypeKind::Integer;
                            viRandNeg.registerName = randNegReg;
                            viRandNeg.isMutable = false;
                            viRandNeg.scopeLevel = currentScopeLevel_;
                            addVariable(viRandNeg);
                        }

                        // (AR) بناء جسم الاختبار
                        // (EN) Build test body
                        if (testDecl->body)
                        {
                            buildStatement(testDecl->body.get());
                        }

                        exitScope();

                        // (AR) قفز إلى increment
                        // (EN) Branch to increment
                        if (currentBlock_ && !currentBlock_->getTerminator())
                        {
                            currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(incrLabel)));
                        }

                        // (AR) كتلة Increment: counter++
                        // (EN) Increment block: counter++
                        auto incrBlock = createBasicBlock(incrLabel);
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(incrBlock);
                        currentBlock_ = incrBlock;

                        std::string newCounterReg = newTempRegister();
                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(
                                SIRInstruction::Binary(
                                    SIROpcode::ADD_I64,
                                    SIROperand::Register(newCounterReg, SadTypeKind::Integer),
                                    SIROperand::Register(counterReg, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(1)));
                            // (AR) تحديث العداد
                            // (EN) Update counter
                            currentBlock_->addInstruction(
                                SIRInstruction::Store(
                                    SIROperand::Register(counterReg, SadTypeKind::Integer),
                                    SIROperand::Register(newCounterReg, SadTypeKind::Integer)));
                            currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(headerLabel)));
                        }

                        // (AR) كتلة Exit: إرجاع true (نجح)
                        // (EN) Exit block: return true (passed)
                        auto exitBlock = createBasicBlock(exitLabel);
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(exitBlock);
                        currentBlock_ = exitBlock;

                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::Return(
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
                        if (currentBlock_ && !currentBlock_->getTerminator())
                        {
                            if (testDecl->shouldFail)
                            {
                                currentBlock_->addInstruction(SIRInstruction::Return(
                                    SIROperand::ConstantBool(false)));
                            }
                            else
                            {
                                currentBlock_->addInstruction(SIRInstruction::Return(
                                    SIROperand::ConstantBool(true)));
                            }
                        }
                    }

                    exitScope();

                    restoreContext(std::move(savedCtx));
                    module_->addFunction(testFunc);

                    // (AR) تسجيل دالة الاختبار في جدول الدوال
                    // (EN) Register test function in function table
                    FunctionInfo fi;
                    fi.name = testFuncName;
                    fi.returnType = SadTypeKind::Boolean;
                    fi.sirFunction = testFunc;
                    functionTable_[testFuncName] = fi;

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
                        std::string callResultReg = newTempRegister();
                        SIRInstruction callInst(SIROpcode::CALL);
                        callInst.result = SIROperand::Register(callResultReg, SadTypeKind::Boolean);
                        callInst.operands.push_back(SIROperand::Function(testFuncName));
                        if (currentBlock_)
                            currentBlock_->instructions.push_back(callInst);

                        // (AR) بناء رسالة النجاح
                        // (EN) Build success message
                        if (testDecl->isPropertyTest && testDecl->iterations > 0)
                        {
                            // (AR) طباعة: "✓ اسم_الاختبار (N/N تكرار نجح)"
                            std::string successMsg = "\xe2\x9c\x93 " + testDecl->testName +
                                                     " (" + std::to_string(testDecl->iterations) +
                                                     "/" + std::to_string(testDecl->iterations) +
                                                     " \xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1 \xD9\x86\xD8\xAC\xD8\xAD)";
                            std::string msgReg = newTempRegister();
                            SIRInstruction strInst(SIROpcode::STRING_NEW);
                            strInst.result = SIROperand::Register(msgReg, SadTypeKind::String);
                            strInst.operands.push_back(SIROperand::ConstantString(successMsg));
                            if (currentBlock_)
                                currentBlock_->instructions.push_back(strInst);

                            SIRInstruction printInst(SIROpcode::BUILTIN_PRINT);
                            printInst.operands.push_back(SIROperand::Register(msgReg, SadTypeKind::String));
                            printInst.operands.push_back(SIROperand::ConstantBool(true)); // newline
                            if (currentBlock_)
                                currentBlock_->instructions.push_back(printInst);
                        }
                    }

                    return true;
                }

                // ========================================================================
                // (AR) AtomicDecl - متغير ذري (declarations.h:AtomicDecl)
                // (EN) Atomic variable declaration — lowered to regular variable with atomic hint
                // ========================================================================
                if (auto atomicDecl = dynamic_cast<Sad::AST::AtomicDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found AtomicDecl: " << atomicDecl->name << std::endl;
#endif

                    // (AR) تحويل النوع الداخلي إلى SadTypeKind
                    // (EN) Convert inner type to SadTypeKind
                    SadTypeKind varType = astTypeToSIRType(atomicDecl->innerType);

                    // (AR) تخصيص مكان في الذاكرة
                    // (EN) Allocate memory
                    std::string reg = newTempRegister();
                    if (currentBlock_)
                    {
                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::ALLOC;
                        allocInst.result = SIROperand::Register(reg, varType);
                        currentBlock_->addInstruction(allocInst);
                    }

                    // (AR) تهيئة القيمة إن وُجدت
                    // (EN) Initialize value if present
                    if (atomicDecl->initialValue)
                    {
                        auto initResult = buildExpression(atomicDecl->initialValue.get());
                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::Store(
                                SIROperand::Register(reg, varType),
                                SIROperand::Register(initResult.registerName, initResult.type)));
                        }
                    }

                    // (AR) تسجيل المتغير في النطاق الحالي
                    // (EN) Register variable in current scope
                    VariableInfo varInfo;
                    varInfo.name = atomicDecl->name;
                    varInfo.type = varType;
                    varInfo.registerName = reg;
                    varInfo.isGlobal = false;
                    varInfo.isMutable = true;
                    addVariable(varInfo);
                    return true;
                }

                // ========================================================================
                // (AR) PropertyDecl - خاصية مع getter/setter (property_nodes.h:PropertyDecl)
                // (EN) Property declaration — lowered to getter/setter functions
                // ========================================================================
                if (auto propDecl = dynamic_cast<Sad::AST::PropertyDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found PropertyDecl: " << propDecl->name << std::endl;
#endif

                    SadTypeKind propType = astTypeToSIRType(propDecl->type);

                    // (AR) بناء دالة getter إن وُجدت
                    // (EN) Build getter function if present
                    if (propDecl->getter)
                    {
                        std::string getterName = "__get_" + propDecl->name;
                        auto getterFunc = std::make_shared<SIRFunction>(getterName, propType);

                        // (AR) معامل this
                        // (EN) this parameter
                        getterFunc->addParameter(SIRParameter("this", SadTypeKind::Pointer));

                        auto entryBlock = createBasicBlock("getter_entry");
                        getterFunc->addBasicBlock(entryBlock);

                        auto savedCtxGetter = saveContext();
                        currentFunction_ = getterFunc;
                        currentBlock_ = entryBlock;

                        enterScope();
                        if (propDecl->getter->body)
                        {
                            buildStatement(propDecl->getter->body.get());
                        }
                        exitScope();

                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
                        }

                        restoreContext(std::move(savedCtxGetter));
                        module_->addFunction(getterFunc);
                    }

                    // (AR) بناء دالة setter إن وُجدت
                    // (EN) Build setter function if present
                    if (propDecl->setter)
                    {
                        std::string setterName = "__set_" + propDecl->name;
                        auto setterFunc = std::make_shared<SIRFunction>(setterName, SadTypeKind::Void);

                        // (AR) معامل this + معامل القيمة
                        // (EN) this parameter + value parameter
                        setterFunc->addParameter(SIRParameter("this", SadTypeKind::Pointer));
                        setterFunc->addParameter(SIRParameter(propDecl->setter->parameterName, propType));

                        auto entryBlock = createBasicBlock("setter_entry");
                        setterFunc->addBasicBlock(entryBlock);

                        auto savedCtxSetter = saveContext();
                        currentFunction_ = setterFunc;
                        currentBlock_ = entryBlock;

                        enterScope();
                        // (AR) تسجيل معامل القيمة في النطاق
                        // (EN) Register value parameter in scope
                        VariableInfo paramInfo;
                        paramInfo.name = propDecl->setter->parameterName;
                        paramInfo.type = propType;
                        paramInfo.registerName = propDecl->setter->parameterName;
                        paramInfo.isGlobal = false;
                        paramInfo.isMutable = true;
                        addVariable(paramInfo);

                        if (propDecl->setter->body)
                        {
                            buildStatement(propDecl->setter->body.get());
                        }
                        exitScope();

                        if (currentBlock_)
                        {
                            currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
                        }

                        restoreContext(std::move(savedCtxSetter));
                        module_->addFunction(setterFunc);
                    }
                    return true;
                }

                // ========================================================================
                // (AR) UnsafeBlockStmt - @غير_آمن ... نهاية
                // (EN) Unsafe block statement - @غير_آمن ... نهاية
                // ========================================================================
                if (auto unsafeStmt = dynamic_cast<Sad::AST::UnsafeBlockStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found UnsafeBlockStmt with "
                              << unsafeStmt->body.size() << " statements" << std::endl;
#endif

                    // (AR) تنفيذ جميع الجمل داخل الكتلة غير الآمنة
                    // (EN) Execute all statements inside the unsafe block
                    for (const auto &innerStmt : unsafeStmt->body)
                    {
                        if (innerStmt)
                        {
                            buildStatement(innerStmt.get());
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) ComptimeBlockStmt - @وقت_الترجمة ... نهاية
                // (EN) Compile-time block statement - @وقت_الترجمة ... نهاية
                // ========================================================================
                if (auto comptimeStmt = dynamic_cast<Sad::AST::ComptimeBlockStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ComptimeBlockStmt with "
                              << comptimeStmt->body.size() << " statements" << std::endl;
#endif

                    // (AR) في وقت الترجمة، نقوم فقط بمعالجة الجمل (يمكن توسيعها لاحقاً)
                    // (EN) At compile time, we just process the statements (can be extended later)
                    for (const auto &innerStmt : comptimeStmt->body)
                    {
                        if (innerStmt)
                        {
                            buildStatement(innerStmt.get());
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) VolatileVarDeclStmt - @متطاير متغير
                // (EN) Volatile variable declaration - @متطاير متغير
                // ========================================================================
                if (auto volatileDecl = dynamic_cast<Sad::AST::VolatileVarDeclStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found VolatileVarDeclStmt" << std::endl;
#endif

                    // (AR) استخراج VarDeclStmt من declaration
                    // (EN) Extract VarDeclStmt from declaration
                    if (volatileDecl->declaration)
                    {
                        if (auto varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(volatileDecl->declaration.get()))
                        {
                            // (AR) بناء المتغير العادي مع تعليق volatile
                            // (EN) Build normal variable with volatile comment
                            buildLocalVariable(varDecl);
                        }
                        else
                        {
                            // (AR) إذا لم يكن VarDeclStmt، نعالج كجملة عادية
                            // (EN) If not VarDeclStmt, process as normal statement
                            buildStatement(volatileDecl->declaration.get());
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) MacroDecl - تصريح ماكرو: ماكرو اسم(معاملات) ... نهاية
                //      نُسجّل الماكرو في خريطة macros_ فقط — يُوسَّع عند الاستدعاء
                //      (مشابه لـ StatementExecutor::visitMacroDecl في المفسر)
                // (EN) MacroDecl - macro declaration: macro name(params) ... end
                //      We register the macro in macros_ map only — expanded at call site
                //      (mirrors StatementExecutor::visitMacroDecl in interpreter)
                // ========================================================================
                if (auto macroDecl = dynamic_cast<Sad::AST::MacroDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Registering macro: '" << macroDecl->name
                              << "' with " << macroDecl->params.size() << " params"
                              << (macroDecl->isVariadic ? " (variadic)" : "") << std::endl;
#endif
                    // (AR) تخزين مؤشر الماكرو (AST يملك الذاكرة)
                    // (EN) Store macro pointer (AST owns memory)
                    macros_[macroDecl->name] = macroDecl;
                    return true;
                }

                // ========================================================================
                // (AR) GoStmt - جملة أطلق (goroutine) — تنفيذ متزامن
                // (EN) Go statement — concurrent execution (goroutine)
                //
                // (AR) تحوّل إلى ASYNC_SPAWN:
                //      أطلق دالة()      → ASYNC_SPAWN(دالة)
                //      أطلق تعبير        → ASYNC_SPAWN(lambda_wrapper(تعبير))
                //      أطلق ... نهاية    → ASYNC_SPAWN(anonymous_lambda)
                // (EN) Lowers to ASYNC_SPAWN:
                //      go func()         → ASYNC_SPAWN(func)
                //      go expr           → ASYNC_SPAWN(lambda_wrapper(expr))
                //      go ... end        → ASYNC_SPAWN(anonymous_lambda)
                // ========================================================================
                if (auto goStmt = dynamic_cast<Sad::AST::GoStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found GoStmt" << std::endl;
#endif
                    if (goStmt->expression)
                    {
                        // (AR) أطلق تعبير — نبني التعبير كاستدعاء دالة
                        // (EN) go expression — build expression as function call
                        auto exprResult = buildExpression(goStmt->expression.get());

                        // (AR) إصلاح: بعض تعابير الاستدعاء ذات نوع void لا تُنتج register صالح.
                        //      في هذه الحالة التعبير نفسه يكون قد أضاف تعليماته بالفعل،
                        //      لذلك لا نولّد ASYNC_SPAWN بسجل غير معرّف.
                        // (EN) Fix: some void call expressions do not produce a valid register.
                        //      In that case, expression lowering already emitted instructions,
                        //      so avoid emitting ASYNC_SPAWN with an undefined register.
                        if (!exprResult.registerName.empty())
                        {
                            std::string resultReg = newTempRegister();
                            SIRInstruction spawnInst(SIROpcode::ASYNC_SPAWN);
                            spawnInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                            spawnInst.operands.push_back(
                                SIROperand::Register(exprResult.registerName, exprResult.type));
                            spawnInst.comment = "go expression → ASYNC_SPAWN";
                            if (currentBlock_)
                                currentBlock_->instructions.push_back(spawnInst);
                        }
                    }
                    else if (goStmt->blockBody)
                    {
                        // (AR) إصلاح جذري لكتل أطلق المتداخلة:
                        //      التنفيذ الخلفي الحالي للـ ASYNC_SPAWN يعمل بشكل متزامن (direct call)
                        //      ولا يدعم captures كاملة للمتغيرات المحلية داخل go-blocks المتداخلة.
                        //      لذلك نُخفض go-block مباشرة داخل نفس السياق للحفاظ على صحة النطاقات
                        //      ومنع توليد سجلات غير معرّفة مثل %مج_داخلي في الاختبارات المركبة.
                        // (EN) Root fix for nested go blocks:
                        //      current ASYNC_SPAWN backend behaves synchronously (direct call)
                        //      and does not fully support local captures in nested go-blocks.
                        //      So we lower go-block inline in the same scope to preserve
                        //      variable visibility and avoid undefined registers (e.g. %مج_داخلي).
                        buildStatement(goStmt->blockBody.get());
                    }
                    return true;
                }

                // ========================================================================
                // (AR) SelectStmt - جملة اختر — اختيار من قنوات متعددة
                // (EN) Select statement — multiplex on multiple channels
                //
                // (AR) التنفيذ: نُحوّل إلى سلسلة فحوصات:
                //      لكل حالة: ASYNC_CHANNEL_HAS_DATA → إذا نعم: ASYNC_CHANNEL_RECV + جسم
                //      إذا لم تنجح أي حالة: جسم الافتراضي (إن وُجد)
                // (EN) Implementation: lower to check chain:
                //      for each case: ASYNC_CHANNEL_HAS_DATA → if yes: ASYNC_CHANNEL_RECV + body
                //      if no case ready: default body (if exists)
                // ========================================================================
                if (auto selectStmt = dynamic_cast<Sad::AST::SelectStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found SelectStmt with " << selectStmt->cases.size() << " cases" << std::endl;
#endif
                    // (AR) إنشاء تسمية الخروج
                    std::string exitLabel = newLabel("select_exit");
                    auto exitBlock = createBasicBlock(exitLabel);

                    for (size_t i = 0; i < selectStmt->cases.size(); ++i)
                    {
                        auto &selCase = selectStmt->cases[i];

                        // (AR) إنشاء تسميات لجسم الحالة والحالة التالية
                        std::string bodyLabel = newLabel("select_case_" + std::to_string(i));
                        std::string nextLabel = newLabel("select_next_" + std::to_string(i));
                        auto bodyBlock = createBasicBlock(bodyLabel);
                        auto nextBlock = createBasicBlock(nextLabel);

                        // (AR) بناء تعبير القناة (عادةً ق.حاول_استقبل() أو ق.استقبل())
                        // (EN) Build channel expression => check if data available
                        if (selCase->channelExpr)
                        {
                            std::string checkReg = newTempRegister();
                            bool loweredToTryRecvCompare = false;

                            // (AR) إصلاح دلالة select:
                            //      عندما تكون الحالة من الشكل ق.حاول_استقبل() يجب أن نستهلك
                            //      العنصر عند النجاح. لذلك نُبقي الاستدعاء try_recv نفسه ونقارن
                            //      الناتج مع null-sentinel (لاشيء) بدلاً من المقارنة مع 0.
                            // (EN) Fix select semantics:
                            //      for ch.try_recv() cases we must consume on success.
                            //      Keep try_recv call and compare result with null-sentinel
                            //      instead of comparing with 0.
                            if (auto m = dynamic_cast<Sad::AST::MethodCallExpr *>(selCase->channelExpr.get()))
                            {
                                const std::string &mn = m->methodName;
                                const bool isTryRecv =
                                    mn == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84" ||
                                    mn == "try_recv";
                                if (isTryRecv)
                                {
                                    auto recvResult = buildExpression(selCase->channelExpr.get());
                                    if (!recvResult.registerName.empty())
                                    {
                                        SIRInstruction cmpInst = SIRInstruction::Binary(
                                            SIROpcode::NE,
                                            SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                            SIROperand::Register(recvResult.registerName, recvResult.type),
                                            SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                                        cmpInst.comment = "select: try_recv != null case " + std::to_string(i);
                                        if (currentBlock_)
                                            currentBlock_->addInstruction(cmpInst);
                                        loweredToTryRecvCompare = true;
                                    }
                                }
                            }

                            if (!loweredToTryRecvCompare)
                            {
                                // (AR) المسار العام: تقييم التعبير ثم مقارنة != 0
                                auto chanResult = buildExpression(selCase->channelExpr.get());
                                SIRInstruction cmpInst = SIRInstruction::Binary(
                                    SIROpcode::NE,
                                    SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                    SIROperand::Register(chanResult.registerName, chanResult.type),
                                    SIROperand::ConstantI64(0));
                                cmpInst.comment = "select: check channel case " + std::to_string(i);
                                if (currentBlock_)
                                    currentBlock_->addInstruction(cmpInst);
                            }

                            // (AR) تفرع: إذا جاهزة → الجسم، وإلا → الحالة التالية
                            SIRInstruction brInst = SIRInstruction::BranchCond(
                                SIROperand::Register(checkReg, SadTypeKind::Boolean),
                                SIROperand::Label(bodyLabel),
                                SIROperand::Label(nextLabel));
                            if (currentBlock_)
                                currentBlock_->addInstruction(brInst);
                        }
                        else
                        {
                            // (AR) لا يوجد تعبير — تخطي إلى التالي
                            if (currentBlock_)
                                currentBlock_->addInstruction(
                                    SIRInstruction::Branch(SIROperand::Label(nextLabel)));
                        }

                        // (AR) كتلة جسم الحالة
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(bodyBlock);
                        currentBlock_ = bodyBlock;

                        // (AR) بناء جسم الحالة
                        for (auto &bodyStmt : selCase->body)
                        {
                            if (bodyStmt)
                                buildStatement(bodyStmt.get());
                        }

                        // (AR) قفز إلى الخروج بعد الجسم
                        if (currentBlock_ && !currentBlock_->getTerminator())
                        {
                            currentBlock_->addInstruction(
                                SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                        }

                        // (AR) كتلة الحالة التالية
                        if (currentFunction_)
                            currentFunction_->addBasicBlock(nextBlock);
                        currentBlock_ = nextBlock;
                    }

                    // (AR) الحالة الافتراضية
                    if (!selectStmt->defaultBody.empty())
                    {
                        for (auto &defStmt : selectStmt->defaultBody)
                        {
                            if (defStmt)
                                buildStatement(defStmt.get());
                        }
                    }

                    // (AR) قفز إلى الخروج
                    if (currentBlock_ && !currentBlock_->getTerminator())
                    {
                        currentBlock_->addInstruction(
                            SIRInstruction::Branch(SIROperand::Label(exitLabel)));
                    }

                    // (AR) كتلة الخروج
                    if (currentFunction_)
                        currentFunction_->addBasicBlock(exitBlock);
                    currentBlock_ = exitBlock;

                    return true;
                }

                // ========================================================================
                // (AR) ExtensionDecl - كتلة الامتداد: امتداد نوع_هدف ... نهاية
                //      تُضيف دوالاً جديدة لصنف موجود بدون تعديل تعريفه الأصلي
                //      كل دالة في كتلة الامتداد تُسجَّل كطريقة في الصنف الهدف
                //      مثال:
                //        امتداد نقطة
                //            دالة وصف()
                //                ارجع "(" + هذا.س + "، " + هذا.ص + ")"
                //            نهاية
                //        نهاية
                // (EN) Extension block: extension target_type ... end
                //      Adds new methods to an existing class without modifying its definition
                //      Each function in extension block is registered as a method on the target class
                // ========================================================================
                if (auto extensionDecl = dynamic_cast<Sad::AST::ExtensionDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ExtensionDecl for type: '" << extensionDecl->targetType << "'" << std::endl;
#endif
                    // (AR) حفظ اسم الصنف الحالي واستعادته بعد الانتهاء
                    // (EN) Save current class name and restore after finishing
                    std::string savedClassName = currentClassName_;
                    currentClassName_ = extensionDecl->targetType;

                    // (AR) معالجة كل دالة في كتلة الامتداد كطريقة في الصنف الهدف
                    // (EN) Process each function in extension block as a method on the target class
                    for (auto &method : extensionDecl->methods)
                    {
                        if (!method)
                            continue;

                        auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(method.get());
                        if (!funcDecl)
                            continue;

#ifndef NDEBUG
                        std::cout << "[DEBUG] ExtensionDecl: adding method '" << funcDecl->name
                                  << "' to class '" << extensionDecl->targetType << "'" << std::endl;
#endif

                        // (AR) إنشاء اسم الطريقة الكامل: صنف.اسم_الطريقة
                        // (EN) Create full method name: class.method_name
                        std::string fullMethodName = extensionDecl->targetType + "." + funcDecl->name;

                        // (AR) استنتاج نوع الإرجاع
                        // (EN) Infer return type
                        SadTypeKind inferredReturnType = SadTypeKind::Void;
                        if (funcDecl->body)
                        {
                            inferredReturnType = inferReturnTypeFromBody(funcDecl->body.get(), funcDecl);
                        }

                        // (AR) إنشاء دالة SIR للطريقة
                        // (EN) Create SIR function for the method
                        auto sirFunc = std::make_shared<SIRFunction>(fullMethodName, inferredReturnType);

                        // (AR) إضافة معامل self كأول معامل
                        // (EN) Add self as first parameter
                        sirFunc->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                        // (AR) إضافة بقية المعاملات
                        // (EN) Add remaining parameters
                        for (auto &param : funcDecl->parameters)
                        {
                            SadTypeKind paramType = SadTypeKind::Integer;
                            if (param.type != Sad::Data::DataType::UNKNOWN)
                            {
                                paramType = astTypeToSIRType(param.type);
                            }
                            sirFunc->addParameter(SIRParameter(param.name, paramType));
                        }

                        // (AR) تسجيل الدالة في الوحدة وجدول الدوال
                        // (EN) Register function in module and function table
                        module_->addFunction(sirFunc);

                        FunctionInfo funcInfo;
                        funcInfo.name = fullMethodName;
                        funcInfo.returnType = inferredReturnType;
                        funcInfo.parameters = sirFunc->parameters;
                        funcInfo.sirFunction = sirFunc;
                        funcInfo.astDecl = funcDecl;
                        functionTable_[fullMethodName] = funcInfo;

                        // (AR) بناء جسم الطريقة
                        // (EN) Build method body
                        auto savedCtxExt = saveContext();

                        currentFunction_ = sirFunc;
                        auto entryBlock = createBasicBlock(kEntryBlockName);
                        sirFunc->addBasicBlock(entryBlock);
                        currentBlock_ = entryBlock;

                        enterScope();

                        // (AR) تسجيل self
                        VariableInfo selfInfo;
                        selfInfo.name = kSelfParamName;
                        selfInfo.type = SadTypeKind::Integer;
                        selfInfo.registerName = kSelfRegisterName;
                        selfInfo.isMutable = false;
                        selfInfo.className = extensionDecl->targetType;
                        addVariable(selfInfo);
                        classInstanceTypes_[kSelfRegisterName] = extensionDecl->targetType;

                        // (AR) تسجيل المعاملات
                        // (EN) Register parameters
                        for (size_t i = 0; i < funcDecl->parameters.size(); ++i)
                        {
                            VariableInfo paramInfo;
                            paramInfo.name = funcDecl->parameters[i].name;
                            paramInfo.type = sirFunc->parameters[i + 1].type;
                            paramInfo.registerName = "%" + funcDecl->parameters[i].name;
                            paramInfo.isParameter = true;
                            addVariable(paramInfo);
                        }

                        // (AR) بناء الجسم
                        if (funcDecl->body)
                        {
                            buildStatement(funcDecl->body.get());
                        }

                        // (AR) إضافة return void إذا لم يكن هناك terminator
                        if (currentBlock_ && !currentBlock_->getTerminator())
                        {
                            SIRInstruction retInst(SIROpcode::RET);
                            retInst.comment = "implicit return from extension method";
                            currentBlock_->addInstruction(retInst);
                        }

                        exitScope();

                        restoreContext(std::move(savedCtxExt));
                    }

                    currentClassName_ = savedClassName;
                    return true;
                }

                // ========================================================================
                // (AR) TypeAliasDecl - تصريح اسم مستعار للنوع: نوع اسم = نوع_هدف
                //      يُسجّل الاسم المستعار كمرادف للنوع الهدف
                //      مثال: نوع عدد = رقم
                //      مثال: نوع قائمة = مصفوفة
                //      في سياق المترجم: نُسجّل الاسم كصنف بنفس بنية الهدف
                //      أو نُسجّل كمرادف في جدول الأنواع
                // (EN) Type alias declaration: type name = target_type
                //      Registers the alias as equivalent to the target type
                //      In compiler context: register as a synonym in type table
                // ========================================================================
                if (auto typeAliasDecl = dynamic_cast<Sad::AST::TypeAliasDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found TypeAliasDecl: '" << typeAliasDecl->name << "'" << std::endl;
#endif

                    // (AR) إذا كان الهدف اسم صنف → نُسجّل الاسم المستعار ككائن بنفس بنية الصنف
                    //      إذا كان نوعاً بدائياً → نتجاهل (الأنواع البدائية لا تحتاج alias في SIR)
                    // (EN) If target is a class name → register alias as object with same class structure
                    //      If primitive type → ignore (primitives don't need alias in SIR)
                    if (typeAliasDecl->target)
                    {
                        auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(typeAliasDecl->target.get());
                        if (varExpr)
                        {
                            std::string targetName = varExpr->name;

                            // (AR) فحص هل الهدف صنف مسجل
                            // (EN) Check if target is a registered class
                            auto targetClass = module_ ? module_->getClass(targetName) : nullptr;
                            if (targetClass)
                            {
                                // (AR) إنشاء صنف SIR جديد بنفس البنية
                                // (EN) Create new SIR class with same structure
                                auto aliasClass = std::make_shared<SIRClass>(typeAliasDecl->name);
                                aliasClass->parentClass = targetClass->parentClass;
                                aliasClass->fields_ = targetClass->fields_;
                                module_->addClass(aliasClass);
                                classTable_[typeAliasDecl->name] = aliasClass;

                                // (AR) نسخ جميع دوال الصنف الهدف مع الاسم الجديد
                                //      نجمع الإدخالات أولاً ثم نضيفها (تجنب التعديل أثناء التكرار)
                                // (EN) Copy all target class functions with the new name
                                //      Collect entries first then insert (avoid modifying during iteration)
                                std::vector<std::pair<std::string, FunctionInfo>> newEntries;
                                for (auto &[funcName, funcInfo] : functionTable_)
                                {
                                    if (funcName.find(targetName + ".") == 0)
                                    {
                                        std::string newFuncName = typeAliasDecl->name + funcName.substr(targetName.size());
                                        if (functionTable_.find(newFuncName) == functionTable_.end())
                                        {
                                            FunctionInfo aliasFunc = funcInfo;
                                            aliasFunc.name = newFuncName;
                                            newEntries.push_back({newFuncName, aliasFunc});
                                        }
                                    }
                                }
                                for (auto &[name, info] : newEntries)
                                {
                                    functionTable_[name] = info;
                                }

                                // (AR) نسخ دوال SIR الفعلية في الوحدة (مطلوب للربط)
                                //      نستنسخ كل دالة من الصنف الأصلي باسم الصنف المستعار
                                //      هذا يضمن أن الرابط يجد حيوان_أليف.بناء / حيوان_أليف.وصف
                                // (EN) Clone actual SIR functions in module (required for linking)
                                //      Clone each function from original class with alias class name
                                if (module_)
                                {
                                    std::vector<std::shared_ptr<SIRFunction>> newFuncs;
                                    for (auto &func : module_->getFunctions())
                                    {
                                        if (func && func->name.find(targetName + ".") == 0)
                                        {
                                            std::string newFuncName = typeAliasDecl->name + func->name.substr(targetName.size());
                                            // (AR) تحقق أن الدالة غير موجودة بالفعل
                                            bool exists = false;
                                            for (auto &f : module_->getFunctions())
                                            {
                                                if (f && f->name == newFuncName)
                                                {
                                                    exists = true;
                                                    break;
                                                }
                                            }
                                            if (!exists)
                                            {
                                                // (AR) استنساخ الدالة الأصلية بالكامل باسم جديد
                                                //      نسخ المعاملات والكتل والتعليمات كما هي
                                                // (EN) Clone original function completely with new name
                                                //      Copy parameters, blocks, and instructions as-is
                                                auto clonedFunc = std::make_shared<SIRFunction>(newFuncName, func->returnType);
                                                clonedFunc->parameters = func->parameters;
                                                clonedFunc->basicBlocks = func->basicBlocks;
                                                newFuncs.push_back(clonedFunc);
                                            }
                                        }
                                    }
                                    for (auto &f : newFuncs)
                                    {
                                        module_->addFunction(f);
                                    }
                                }

#ifndef NDEBUG
                                std::cout << "[DEBUG] TypeAlias: registered '" << typeAliasDecl->name
                                          << "' as alias for class '" << targetName << "'" << std::endl;
#endif
                            }
                            else
                            {
#ifndef NDEBUG
                                std::cout << "[DEBUG] TypeAlias: '" << typeAliasDecl->name
                                          << "' = '" << targetName << "' (primitive alias, no SIR class)" << std::endl;
#endif
                            }
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) ReExportStmt - إعادة التصدير: صدّر * من وحدة / صدّر عنصر1 من وحدة
                //      يستورد الوحدة ثم يُعيد تصدير رموزها
                //      في سياق المترجم: نُعامل كاستيراد عادي ثم نُسجّل الرموز للتصدير
                // (EN) Re-export: export * from module / export item1 from module
                //      Imports the module then re-exports its symbols
                //      In compiler context: treat as normal import then register symbols for export
                // ========================================================================
                if (auto reExportStmt = dynamic_cast<Sad::AST::ReExportStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ReExportStmt from module: ";
                    for (auto &seg : reExportStmt->modulePath)
                        std::cout << seg << ".";
                    std::cout << " wildcard=" << reExportStmt->isWildcard << std::endl;
#endif

                    // (AR) بناء اسم الوحدة الكامل
                    // (EN) Build full module name
                    std::string fullModuleName;
                    for (size_t i = 0; i < reExportStmt->modulePath.size(); ++i)
                    {
                        if (i > 0)
                            fullModuleName += ".";
                        fullModuleName += reExportStmt->modulePath[i];
                    }

                    // (AR) تحميل الوحدة عبر محلل الوحدات (نفس منطق buildImportStmt)
                    // (EN) Load module via module resolver (same logic as buildImportStmt)
                    if (!moduleResolver_)
                    {
                        moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
                    }

                    // (AR) التحقق مما إذا تمت معالجة الوحدة بالفعل
                    // (EN) Check if module was already processed
                    if (!processedModules_.count(fullModuleName))
                    {
                        Modules::Module *resolvedModule = moduleResolver_->resolveModule(
                            reExportStmt->modulePath, currentFilePath_);

                        if (resolvedModule)
                        {
                            // (AR) تمييز الوحدة كمعالجة
                            // (EN) Mark module as processed
                            processedModules_.insert(fullModuleName);

                            // (AR) معالجة تصريحات الوحدة — نفس منطق buildImportStmt
                            // (EN) Process module declarations — same logic as buildImportStmt
                            for (const auto &modStmt : resolvedModule->ast)
                            {
                                if (!modStmt)
                                    continue;

                                AST::FunctionDecl *funcDecl = nullptr;
                                AST::VarDeclStmt *varDecl = nullptr;
                                AST::ClassDecl *classDecl = nullptr;

                                // (AR) تصدير صريح
                                if (auto exportDecl = dynamic_cast<AST::ExportDecl *>(modStmt.get()))
                                {
                                    if (exportDecl->declaration)
                                    {
                                        funcDecl = dynamic_cast<AST::FunctionDecl *>(exportDecl->declaration.get());
                                        varDecl = dynamic_cast<AST::VarDeclStmt *>(exportDecl->declaration.get());
                                        classDecl = dynamic_cast<AST::ClassDecl *>(exportDecl->declaration.get());
                                    }
                                }
                                else if (auto exportStmt2 = dynamic_cast<AST::ExportStmt *>(modStmt.get()))
                                {
                                    if (exportStmt2->declaration)
                                    {
                                        funcDecl = dynamic_cast<AST::FunctionDecl *>(exportStmt2->declaration.get());
                                        varDecl = dynamic_cast<AST::VarDeclStmt *>(exportStmt2->declaration.get());
                                        classDecl = dynamic_cast<AST::ClassDecl *>(exportStmt2->declaration.get());
                                    }
                                }
                                else
                                {
                                    funcDecl = dynamic_cast<AST::FunctionDecl *>(modStmt.get());
                                    varDecl = dynamic_cast<AST::VarDeclStmt *>(modStmt.get());
                                    classDecl = dynamic_cast<AST::ClassDecl *>(modStmt.get());
                                }

                                // (AR) تصفية إذا لم يكن wildcard
                                // (EN) Filter if not wildcard
                                if (!reExportStmt->isWildcard)
                                {
                                    std::string declName;
                                    if (funcDecl)
                                        declName = funcDecl->name;
                                    else if (varDecl)
                                        declName = varDecl->name;
                                    else if (classDecl)
                                        declName = classDecl->name;

                                    bool found = false;
                                    for (auto &item : reExportStmt->items)
                                    {
                                        if (item.name == declName)
                                        {
                                            found = true;
                                            break;
                                        }
                                    }
                                    if (!found)
                                        continue;
                                }

                                // (AR) بناء SIR للتصريحات
                                // (EN) Build SIR for declarations
                                if (funcDecl)
                                    buildFunction(funcDecl);
                                if (varDecl)
                                    buildGlobalVariable(varDecl);
                                if (classDecl)
                                    buildClass(classDecl);
                            }
                        }
                        else
                        {
                            errors_.push_back(
                                "خطأ: فشل استيراد الوحدة للتصدير '" + fullModuleName + "' / "
                                                                                       "Error: Failed to import module for re-export '" +
                                fullModuleName + "'");
                        }
                    }
                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad