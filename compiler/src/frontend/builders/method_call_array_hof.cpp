// ============================================================================
// sir_builder_method_call_array_hof.cpp
// ============================================================================
// (AR) دوال المصفوفات العالية: خريطة/رشح/اختزل/لكل/أول/آخر/مسطح
// (EN) Array higher-order methods: map/filter/reduce/forEach/first/last/flat
// ============================================================================

#include <string>
#include <optional>
#include "sir_builder.h"
#include "builders/method_call_builder.h"
#include "builtin_registry.h"

namespace TM = Sad::Builtins::Names::TypeMethods;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            std::optional<BuildResult> MethodCallBuilder::buildArrayHigherOrderMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args,
                const std::string &firstClosureLambdaName, SadTypeKind firstClosureRetType)
            {
                // ================================================================
                // (AR) طرق المصفوفات العالية الرتبة مع اللامدا (Higher-order Array Methods)
                //      خريطة (map)، رشح (filter)، اختزل (reduce)، لكل (forEach)
                //      تُنفَّذ عبر تفكيك (desugaring) إلى حلقة SIR:
                //      - ARRAY_LEN لحساب الطول
                //      - ARRAY_NEW لإنشاء مصفوفة النتيجة (خريطة/رشح)
                //      - حلقة: ARRAY_GET + CLOSURE_CALL + (ARRAY_APPEND أو تجميع)
                //      لا حاجة إلى opcodes جديدة — كل الأوليّات موجودة
                // (EN) Higher-order array methods with lambda
                //      map, filter, reduce, forEach
                //      Implemented via desugaring to SIR loop:
                //      - ARRAY_LEN for length
                //      - ARRAY_NEW for result array (map/filter)
                //      - loop: ARRAY_GET + CLOSURE_CALL + (ARRAY_APPEND or accumulate)
                //      No new opcodes needed — all primitives exist
                // ================================================================

                // (AR) خريطة / map — تطبيق اللامدا على كل عنصر وإرجاع مصفوفة جديدة
                // (EN) map — apply lambda to each element and return new array
                if (methodName == TM::Array::MAP)
                {
                    // (AR) استخراج مؤشر الإغلاق من args[1] (الوسيط الأول بعد self)
                    // (EN) Extract closure pointer from args[1] (first arg after self)
                    if (args.size() < 2)
                    {
                        b_.errors_.push_back("Error: \u062e\u0631\u064a\u0637\u0629() requires a lambda argument");
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIROperand closureOp = args[1];

                    // (AR) نوع إرجاع اللامدا — يُؤخذ من تتبّع الخطوة 3
                    // (EN) Lambda return type — taken from Step 3 tracking
                    SadTypeKind lambdaRetType = firstClosureRetType;
                    if (lambdaRetType == SadTypeKind::Void)
                        lambdaRetType = SadTypeKind::Integer;

                    // (AR) الخطوة 1: الحصول على طول المصفوفة المصدر
                    // (EN) Step 1: Get source array length
                    std::string lenReg = b_.newTempRegister();
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                    lenInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(lenInst);

                    // (AR) الخطوة 2: إنشاء مصفوفة النتيجة
                    // (EN) Step 2: Create result array
                    std::string resultArrReg = b_.newTempRegister();
                    SIRInstruction newArrInst;
                    newArrInst.opcode = SIROpcode::ARRAY_NEW;
                    newArrInst.result = SIROperand::Register(resultArrReg, SadTypeKind::Array);
                    newArrInst.operands.push_back(SIROperand::ConstantI64(0));
                    newArrInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    newArrInst.comment = "map result array";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(newArrInst);

                    // (AR) الخطوة 3: تهيئة العداد i = 0
                    // (EN) Step 3: Initialize counter i = 0
                    std::string iVarReg = b_.newTempRegister();
                    SIRInstruction iAllocInst(SIROpcode::ALLOC);
                    iAllocInst.result = SIROperand::Register(iVarReg, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iAllocInst);
                    SIRInstruction iStoreInst(SIROpcode::STORE);
                    iStoreInst.operands.push_back(SIROperand::ConstantI64(0));
                    iStoreInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iStoreInst);

                    // (AR) الخطوة 4: إنشاء الكتل الأساسية للحلقة
                    // (EN) Step 4: Create loop basic blocks
                    std::string condLabel = b_.newLabel("map_cond");
                    std::string bodyLabel = b_.newLabel("map_body");
                    std::string exitLabel = b_.newLabel("map_exit");
                    auto condBlock = b_.createBasicBlock(condLabel);
                    auto bodyBlock = b_.createBasicBlock(bodyLabel);
                    auto exitBlock = b_.createBasicBlock(exitLabel);
                    if (b_.currentFunction_)
                    {
                        b_.currentFunction_->addBasicBlock(condBlock);
                        b_.currentFunction_->addBasicBlock(bodyBlock);
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    }

                    // (AR) قفز إلى كتلة الشرط
                    // (EN) Jump to condition block
                    SIRInstruction brCondInst = SIRInstruction::Branch(SIROperand::Label(condLabel));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(brCondInst);

                    // (AR) الخطوة 5: كتلة الشرط — i < len
                    // (EN) Step 5: Condition block — i < len
                    b_.currentBlock_ = condBlock;
                    std::string iLoadReg = b_.newTempRegister();
                    SIRInstruction iLoadInst(SIROpcode::LOAD);
                    iLoadInst.result = SIROperand::Register(iLoadReg, SadTypeKind::Integer);
                    iLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iLoadInst);
                    std::string cmpReg = b_.newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::LT);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(iLoadReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(cmpInst);
                    SIRInstruction brLoopInst = SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(bodyLabel),
                        SIROperand::Label(exitLabel));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(brLoopInst);

                    // (AR) الخطوة 6: كتلة الجسم — elem = arr[i]; result = closure(elem); resultArr.append(result)
                    // (EN) Step 6: Body block — elem = arr[i]; result = closure(elem); resultArr.append(result)
                    b_.currentBlock_ = bodyBlock;
                    // (AR) تحميل i مرة أخرى في كتلة الجسم
                    // (EN) Reload i in body block
                    std::string iBodyReg = b_.newTempRegister();
                    SIRInstruction iBodyLoadInst(SIROpcode::LOAD);
                    iBodyLoadInst.result = SIROperand::Register(iBodyReg, SadTypeKind::Integer);
                    iBodyLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iBodyLoadInst);
                    // (AR) جلب العنصر arr[i]
                    // (EN) Get element arr[i]
                    std::string elemReg = b_.newTempRegister();
                    SIRInstruction getInst(SIROpcode::ARRAY_GET);
                    getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                    getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    getInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(getInst);
                    // (AR) استدعاء اللامدا على العنصر
                    // (EN) Call lambda on element
                    std::string callResReg = b_.newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CLOSURE_CALL;
                    callInst.result = SIROperand::Register(callResReg, lambdaRetType);
                    callInst.operands.push_back(closureOp);
                    callInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (!firstClosureLambdaName.empty())
                        callInst.comment = "lambda:" + firstClosureLambdaName;
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(callInst);
                    // (AR) إلحاق النتيجة بالمصفوفة الجديدة
                    // (EN) Append result to new array
                    std::string appendReg = b_.newTempRegister();
                    SIRInstruction appendInst(SIROpcode::BUILTIN_ARRAY_APPEND);
                    appendInst.result = SIROperand::Register(appendReg, SadTypeKind::Void);
                    appendInst.operands.push_back(SIROperand::Register(resultArrReg, SadTypeKind::Array));
                    appendInst.operands.push_back(SIROperand::Register(callResReg, lambdaRetType));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(appendInst);
                    // (AR) زيادة العداد i = i + 1
                    // (EN) Increment counter i = i + 1
                    std::string incReg = b_.newTempRegister();
                    SIRInstruction incInst(SIROpcode::ADD_I64);
                    incInst.result = SIROperand::Register(incReg, SadTypeKind::Integer);
                    incInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    incInst.operands.push_back(SIROperand::ConstantI64(1));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(incInst);
                    SIRInstruction iStoreBackInst(SIROpcode::STORE);
                    iStoreBackInst.operands.push_back(SIROperand::Register(incReg, SadTypeKind::Integer));
                    iStoreBackInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iStoreBackInst);
                    // (AR) قفز عائد إلى كتلة الشرط
                    // (EN) Jump back to condition block
                    SIRInstruction brBackInst = SIRInstruction::Branch(SIROperand::Label(condLabel));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(brBackInst);

                    // (AR) الخطوة 7: كتلة الخروج — إرجاع المصفوفة الجديدة
                    // (EN) Step 7: Exit block — return new array
                    b_.currentBlock_ = exitBlock;
                    BuildResult mapResult(resultArrReg, SadTypeKind::Array);
                    mapResult.elementType = lambdaRetType;
                    return mapResult;
                }

                // (AR) رشح / filter — ترشيح عناصر المصفوفة بلامدا شرطيّة
                // (EN) filter — filter array elements with a predicate lambda
                if (methodName == TM::Array::FILTER)
                {
                    if (args.size() < 2)
                    {
                        b_.errors_.push_back("Error: رشح() requires a lambda argument");
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIROperand closureOp = args[1];

                    // (AR) الخطوة 1: طول المصفوفة المصدر
                    // (EN) Step 1: Source array length
                    std::string lenReg = b_.newTempRegister();
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                    lenInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(lenInst);

                    // (AR) الخطوة 2: مصفوفة النتيجة الفارغة
                    // (EN) Step 2: Empty result array
                    std::string resultArrReg = b_.newTempRegister();
                    SIRInstruction newArrInst;
                    newArrInst.opcode = SIROpcode::ARRAY_NEW;
                    newArrInst.result = SIROperand::Register(resultArrReg, SadTypeKind::Array);
                    newArrInst.operands.push_back(SIROperand::ConstantI64(0));
                    newArrInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    newArrInst.comment = "filter result array";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(newArrInst);

                    // (AR) الخطوة 3: تهيئة العداد
                    // (EN) Step 3: Initialize counter
                    std::string iVarReg = b_.newTempRegister();
                    SIRInstruction iAllocInst(SIROpcode::ALLOC);
                    iAllocInst.result = SIROperand::Register(iVarReg, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iAllocInst);
                    SIRInstruction iStoreInst(SIROpcode::STORE);
                    iStoreInst.operands.push_back(SIROperand::ConstantI64(0));
                    iStoreInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iStoreInst);

                    // (AR) الخطوة 4: إنشاء الكتل
                    // (EN) Step 4: Create blocks
                    std::string condLabel = b_.newLabel("filter_cond");
                    std::string bodyLabel = b_.newLabel("filter_body");
                    std::string appendLabel = b_.newLabel("filter_append");
                    std::string nextLabel = b_.newLabel("filter_next");
                    std::string exitLabel = b_.newLabel("filter_exit");
                    auto condBlock = b_.createBasicBlock(condLabel);
                    auto bodyBlock = b_.createBasicBlock(bodyLabel);
                    auto appendBlock = b_.createBasicBlock(appendLabel);
                    auto nextBlock = b_.createBasicBlock(nextLabel);
                    auto exitBlock = b_.createBasicBlock(exitLabel);
                    if (b_.currentFunction_)
                    {
                        b_.currentFunction_->addBasicBlock(condBlock);
                        b_.currentFunction_->addBasicBlock(bodyBlock);
                        b_.currentFunction_->addBasicBlock(appendBlock);
                        b_.currentFunction_->addBasicBlock(nextBlock);
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    }

                    // (AR) قفز إلى الشرط
                    // (EN) Jump to condition
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) الخطوة 5: كتلة الشرط — i < len
                    // (EN) Step 5: Condition block — i < len
                    b_.currentBlock_ = condBlock;
                    std::string iLoadReg = b_.newTempRegister();
                    SIRInstruction iLoadInst(SIROpcode::LOAD);
                    iLoadInst.result = SIROperand::Register(iLoadReg, SadTypeKind::Integer);
                    iLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iLoadInst);
                    std::string cmpReg = b_.newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::LT);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(iLoadReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(cmpInst);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(bodyLabel),
                            SIROperand::Label(exitLabel)));

                    // (AR) الخطوة 6: كتلة الجسم — elem = arr[i]; cond = closure(elem); if cond → append
                    // (EN) Step 6: Body block — elem = arr[i]; cond = closure(elem); if cond → append
                    b_.currentBlock_ = bodyBlock;
                    std::string iBodyReg = b_.newTempRegister();
                    SIRInstruction iBodyLoadInst(SIROpcode::LOAD);
                    iBodyLoadInst.result = SIROperand::Register(iBodyReg, SadTypeKind::Integer);
                    iBodyLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iBodyLoadInst);
                    std::string elemReg = b_.newTempRegister();
                    SIRInstruction getInst(SIROpcode::ARRAY_GET);
                    getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                    getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    getInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(getInst);
                    // (AR) استدعاء لامدا الشرط
                    // (EN) Call predicate lambda
                    std::string predReg = b_.newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CLOSURE_CALL;
                    callInst.result = SIROperand::Register(predReg, SadTypeKind::Boolean);
                    callInst.operands.push_back(closureOp);
                    callInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (!firstClosureLambdaName.empty())
                        callInst.comment = "lambda:" + firstClosureLambdaName;
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(callInst);
                    // (AR) قفز شرطي: إذا صحيح → إلى الإلحاق، وإلا → التالي
                    // (EN) Conditional branch: if true → append, else → next
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(predReg, SadTypeKind::Boolean),
                            SIROperand::Label(appendLabel),
                            SIROperand::Label(nextLabel)));

                    // (AR) كتلة الإلحاق — append element
                    // (EN) Append block
                    b_.currentBlock_ = appendBlock;
                    std::string appendReg = b_.newTempRegister();
                    SIRInstruction appendInst(SIROpcode::BUILTIN_ARRAY_APPEND);
                    appendInst.result = SIROperand::Register(appendReg, SadTypeKind::Void);
                    appendInst.operands.push_back(SIROperand::Register(resultArrReg, SadTypeKind::Array));
                    appendInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(appendInst);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(nextLabel)));

                    // (AR) كتلة التالي — زيادة العداد والقفز إلى الشرط
                    // (EN) Next block — increment and jump to condition
                    b_.currentBlock_ = nextBlock;
                    std::string iNextReg = b_.newTempRegister();
                    SIRInstruction iNextLoadInst(SIROpcode::LOAD);
                    iNextLoadInst.result = SIROperand::Register(iNextReg, SadTypeKind::Integer);
                    iNextLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iNextLoadInst);
                    std::string incReg = b_.newTempRegister();
                    SIRInstruction incInst(SIROpcode::ADD_I64);
                    incInst.result = SIROperand::Register(incReg, SadTypeKind::Integer);
                    incInst.operands.push_back(SIROperand::Register(iNextReg, SadTypeKind::Integer));
                    incInst.operands.push_back(SIROperand::ConstantI64(1));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(incInst);
                    SIRInstruction iStoreBackInst(SIROpcode::STORE);
                    iStoreBackInst.operands.push_back(SIROperand::Register(incReg, SadTypeKind::Integer));
                    iStoreBackInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iStoreBackInst);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) كتلة الخروج
                    // (EN) Exit block
                    b_.currentBlock_ = exitBlock;
                    BuildResult filterResult(resultArrReg, SadTypeKind::Array);
                    filterResult.elementType = SadTypeKind::Integer;
                    return filterResult;
                }

                // (AR) اختزل / reduce — اختزال المصفوفة إلى قيمة واحدة عبر لامدا التجميع
                // (EN) reduce — reduce array to single value via accumulator lambda
                if (methodName == TM::Array::REDUCE)
                {
                    // (AR) اختزل(لامدا، قيمة_ابتدائية) — الوسيط الأول لامدا والثاني القيمة الابتدائية
                    // (EN) reduce(lambda, initial_value) — first arg is lambda, second is initial value
                    if (args.size() < 3)
                    {
                        b_.errors_.push_back("Error: \u0627\u062e\u062a\u0632\u0644() requires a lambda and an initial value");
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIROperand closureOp = args[1]; // (AR) اللامدا
                    SIROperand initOp = args[2];    // (AR) القيمة الابتدائية

                    // (AR) تحديد نوع المجمّع (من القيمة الابتدائية)
                    // (EN) Determine accumulator type (from initial value)
                    SadTypeKind accType = initOp.dataType;
                    if (accType == SadTypeKind::Void)
                        accType = SadTypeKind::Integer;

                    // (AR) الخطوة 1: طول المصفوفة المصدر
                    // (EN) Step 1: Source array length
                    std::string lenReg = b_.newTempRegister();
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                    lenInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(lenInst);

                    // (AR) الخطوة 2: تهيئة المجمّع والعداد
                    // (EN) Step 2: Initialize accumulator and counter
                    std::string accVarReg = b_.newTempRegister();
                    SIRInstruction accAllocInst(SIROpcode::ALLOC);
                    accAllocInst.result = SIROperand::Register(accVarReg, accType);
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(accAllocInst);
                    SIRInstruction accStoreInst(SIROpcode::STORE);
                    accStoreInst.operands.push_back(initOp);
                    accStoreInst.operands.push_back(SIROperand::Register(accVarReg, accType));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(accStoreInst);

                    std::string iVarReg = b_.newTempRegister();
                    SIRInstruction iAllocInst(SIROpcode::ALLOC);
                    iAllocInst.result = SIROperand::Register(iVarReg, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iAllocInst);
                    SIRInstruction iStoreInst(SIROpcode::STORE);
                    iStoreInst.operands.push_back(SIROperand::ConstantI64(0));
                    iStoreInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iStoreInst);

                    // (AR) الخطوة 3: إنشاء الكتل
                    // (EN) Step 3: Create blocks
                    std::string condLabel = b_.newLabel("reduce_cond");
                    std::string bodyLabel = b_.newLabel("reduce_body");
                    std::string exitLabel = b_.newLabel("reduce_exit");
                    auto condBlock = b_.createBasicBlock(condLabel);
                    auto bodyBlock = b_.createBasicBlock(bodyLabel);
                    auto exitBlock = b_.createBasicBlock(exitLabel);
                    if (b_.currentFunction_)
                    {
                        b_.currentFunction_->addBasicBlock(condBlock);
                        b_.currentFunction_->addBasicBlock(bodyBlock);
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    }

                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) الخطوة 4: كتلة الشرط
                    // (EN) Step 4: Condition block
                    b_.currentBlock_ = condBlock;
                    std::string iLoadReg = b_.newTempRegister();
                    SIRInstruction iLoadInst(SIROpcode::LOAD);
                    iLoadInst.result = SIROperand::Register(iLoadReg, SadTypeKind::Integer);
                    iLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iLoadInst);
                    std::string cmpReg = b_.newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::LT);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(iLoadReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(cmpInst);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(bodyLabel),
                            SIROperand::Label(exitLabel)));

                    // (AR) الخطوة 5: كتلة الجسم — acc = closure(acc, elem); i++
                    // (EN) Step 5: Body block — acc = closure(acc, elem); i++
                    b_.currentBlock_ = bodyBlock;
                    std::string iBodyReg = b_.newTempRegister();
                    SIRInstruction iBodyLoadInst(SIROpcode::LOAD);
                    iBodyLoadInst.result = SIROperand::Register(iBodyReg, SadTypeKind::Integer);
                    iBodyLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iBodyLoadInst);
                    // (AR) جلب العنصر
                    // (EN) Get element
                    std::string elemReg = b_.newTempRegister();
                    SIRInstruction getInst(SIROpcode::ARRAY_GET);
                    getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                    getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    getInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(getInst);
                    // (AR) تحميل المجمّع الحالي
                    // (EN) Load current accumulator
                    std::string accLoadReg = b_.newTempRegister();
                    SIRInstruction accLoadInst(SIROpcode::LOAD);
                    accLoadInst.result = SIROperand::Register(accLoadReg, accType);
                    accLoadInst.operands.push_back(SIROperand::Register(accVarReg, accType));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(accLoadInst);
                    // (AR) استدعاء اللامدا(المجمّع، العنصر)
                    // (EN) Call lambda(accumulator, element)
                    std::string callResReg = b_.newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CLOSURE_CALL;
                    callInst.result = SIROperand::Register(callResReg, accType);
                    callInst.operands.push_back(closureOp);
                    callInst.operands.push_back(SIROperand::Register(accLoadReg, accType));
                    callInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (!firstClosureLambdaName.empty())
                        callInst.comment = "lambda:" + firstClosureLambdaName;
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(callInst);
                    // (AR) تخزين المجمّع الجديد
                    // (EN) Store new accumulator
                    SIRInstruction accStoreBackInst(SIROpcode::STORE);
                    accStoreBackInst.operands.push_back(SIROperand::Register(callResReg, accType));
                    accStoreBackInst.operands.push_back(SIROperand::Register(accVarReg, accType));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(accStoreBackInst);
                    // (AR) زيادة العداد
                    // (EN) Increment counter
                    std::string incReg = b_.newTempRegister();
                    SIRInstruction incInst(SIROpcode::ADD_I64);
                    incInst.result = SIROperand::Register(incReg, SadTypeKind::Integer);
                    incInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    incInst.operands.push_back(SIROperand::ConstantI64(1));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(incInst);
                    SIRInstruction iStoreBackInst(SIROpcode::STORE);
                    iStoreBackInst.operands.push_back(SIROperand::Register(incReg, SadTypeKind::Integer));
                    iStoreBackInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iStoreBackInst);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) الخطوة 6: كتلة الخروج — تحميل المجمّع النهائي وإرجاعه
                    // (EN) Step 6: Exit block — load and return final accumulator
                    b_.currentBlock_ = exitBlock;
                    std::string finalAccReg = b_.newTempRegister();
                    SIRInstruction finalAccLoadInst(SIROpcode::LOAD);
                    finalAccLoadInst.result = SIROperand::Register(finalAccReg, accType);
                    finalAccLoadInst.operands.push_back(SIROperand::Register(accVarReg, accType));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(finalAccLoadInst);
                    return BuildResult(finalAccReg, accType);
                }

                // (AR) لكل / forEach — تطبيق اللامدا على كل عنصر دون إرجاع
                // (EN) forEach — apply lambda to each element without returning
                if (methodName == TM::Array::FOR_EACH)
                {
                    if (args.size() < 2)
                    {
                        b_.errors_.push_back("Error: لكل() requires a lambda argument");
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIROperand closureOp = args[1];

                    // (AR) الخطوة 1: الحصول على الطول
                    // (EN) Step 1: Get length
                    std::string lenReg = b_.newTempRegister();
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                    lenInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(lenInst);

                    // (AR) الخطوة 2: تهيئة العداد
                    // (EN) Step 2: Initialize counter
                    std::string iVarReg = b_.newTempRegister();
                    SIRInstruction iAllocInst(SIROpcode::ALLOC);
                    iAllocInst.result = SIROperand::Register(iVarReg, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iAllocInst);
                    SIRInstruction iStoreInst(SIROpcode::STORE);
                    iStoreInst.operands.push_back(SIROperand::ConstantI64(0));
                    iStoreInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iStoreInst);

                    // (AR) الخطوة 3: إنشاء الكتل
                    // (EN) Step 3: Create blocks
                    std::string condLabel = b_.newLabel("foreach_cond");
                    std::string bodyLabel = b_.newLabel("foreach_body");
                    std::string exitLabel = b_.newLabel("foreach_exit");
                    auto condBlock = b_.createBasicBlock(condLabel);
                    auto bodyBlock = b_.createBasicBlock(bodyLabel);
                    auto exitBlock = b_.createBasicBlock(exitLabel);
                    if (b_.currentFunction_)
                    {
                        b_.currentFunction_->addBasicBlock(condBlock);
                        b_.currentFunction_->addBasicBlock(bodyBlock);
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    }

                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) كتلة الشرط
                    // (EN) Condition block
                    b_.currentBlock_ = condBlock;
                    std::string iLoadReg = b_.newTempRegister();
                    SIRInstruction iLoadInst(SIROpcode::LOAD);
                    iLoadInst.result = SIROperand::Register(iLoadReg, SadTypeKind::Integer);
                    iLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iLoadInst);
                    std::string cmpReg = b_.newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::LT);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(iLoadReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(cmpInst);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(bodyLabel),
                            SIROperand::Label(exitLabel)));

                    // (AR) كتلة الجسم — elem = arr[i]; closure(elem); i++
                    // (EN) Body block — elem = arr[i]; closure(elem); i++
                    b_.currentBlock_ = bodyBlock;
                    std::string iBodyReg = b_.newTempRegister();
                    SIRInstruction iBodyLoadInst(SIROpcode::LOAD);
                    iBodyLoadInst.result = SIROperand::Register(iBodyReg, SadTypeKind::Integer);
                    iBodyLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iBodyLoadInst);
                    std::string elemReg = b_.newTempRegister();
                    SIRInstruction getInst(SIROpcode::ARRAY_GET);
                    getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                    getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    getInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(getInst);
                    // (AR) استدعاء اللامدا (مع تجاهل النتيجة)
                    // (EN) Call lambda (ignore result)
                    std::string callResReg = b_.newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CLOSURE_CALL;
                    callInst.result = SIROperand::Register(callResReg, SadTypeKind::Void);
                    callInst.operands.push_back(closureOp);
                    callInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (!firstClosureLambdaName.empty())
                        callInst.comment = "lambda:" + firstClosureLambdaName;
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(callInst);
                    // (AR) زيادة العداد
                    // (EN) Increment counter
                    std::string incReg = b_.newTempRegister();
                    SIRInstruction incInst(SIROpcode::ADD_I64);
                    incInst.result = SIROperand::Register(incReg, SadTypeKind::Integer);
                    incInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    incInst.operands.push_back(SIROperand::ConstantI64(1));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(incInst);
                    SIRInstruction iStoreBackInst(SIROpcode::STORE);
                    iStoreBackInst.operands.push_back(SIROperand::Register(incReg, SadTypeKind::Integer));
                    iStoreBackInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(iStoreBackInst);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) كتلة الخروج
                    // (EN) Exit block
                    b_.currentBlock_ = exitBlock;
                    return BuildResult("", SadTypeKind::Void);
                }

                // ================================================================
                return std::nullopt;
            }

            // === buildStringBuiltinMethodCall ===
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
