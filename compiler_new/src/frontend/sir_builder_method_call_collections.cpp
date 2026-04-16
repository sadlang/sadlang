// ============================================================================
// sir_builder_method_call_collections.cpp
// ============================================================================
// (AR) طرق المجموعات المضمنة — مستخرج من sir_builder_method_call.cpp (CW-05)
//      buildArrayBasicMethodCall       - أضف/حجم/أزل/فارغة/يحتوي/رتب/عكس/...
//      buildArrayHigherOrderMethodCall - خريطة/رشح/اختزل/لكل
//      buildStringBuiltinMethodCall    - قسم/استبدل/يبدأ_بـ/ينتهي_بـ/قص/...
//      buildMapBuiltinMethodCall       - مفاتيح/قيم/حجم/فارغة/يحتوي
// (EN) Collection method call builders — extracted from sir_builder_method_call.cpp
// ============================================================================

#include <string>
#include <optional>
#include "sir_builder.h"

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // === buildArrayBasicMethodCall ===
            std::optional<BuildResult> SIRBuilder::buildArrayBasicMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args)
            {
                if (methodName == "أضف" || methodName == "اضف" ||
                    methodName == "push" || methodName == "append")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_APPEND);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Void);
                    // (AR) ??????? ?????: ????????? ??????: ?????? ???????
                    // (EN) First operand: array, Second: element to add
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                    {
                        inst.operands.push_back(args[1]); // (AR) ?????? (args[0] ?? self)
                    }
                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(inst);
                    }
                    return BuildResult(resultReg, SadTypeKind::Void);
                }

                // (AR) ??? / size - ?????? ??? ??? ???????? ?? ??? ????
                // (EN) size / length - get array size or string length
                // (AR) ? ????? ??? ???????? ?????: ??? ??? ?????? ???? ? BUILTIN_STRING_LENGTH
                // (EN) ? Disambiguate: if object is string ? BUILTIN_STRING_LENGTH
                if (methodName == "حجم" || methodName == "طول" ||
                    methodName == "size" || methodName == "length" || methodName == "len")
                {
                    // (AR) ??????: ?? ?????? ??? ? ?????? BUILTIN_STRING_LENGTH
                    // (EN) Check: is object a string? ? use BUILTIN_STRING_LENGTH
                    if (objResult.type == SadTypeKind::String)
                    {
                        std::string resultReg = newTempRegister();
                        SIRInstruction inst(SIROpcode::BUILTIN_STRING_LENGTH);
                        inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                        inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                        if (currentBlock_)
                            currentBlock_->instructions.push_back(inst);
                        return BuildResult(resultReg, SadTypeKind::Integer);
                    }
                    // (AR) ???? ???: ?????? ? ARRAY_LEN
                    // (EN) Otherwise: array ? ARRAY_LEN
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::ARRAY_LEN);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ??? / pop - ????? ??? ????
                // (EN) pop / remove - remove last element
                if (methodName == "أزل" || methodName == "ازل" ||
                    methodName == "pop" || methodName == "remove_last")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REMOVE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer); // (AR) ?????? ???????
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ????? / empty - ?????? ??? ???? ???????? ?????
                // (EN) empty / is_empty - check if array is empty
                // (AR) ?????? ARRAY_LEN ?????? ?? 0
                // (EN) Use ARRAY_LEN and compare with 0
                if (methodName == "فارغة" || methodName == "فارغ" ||
                    methodName == "empty" || methodName == "is_empty")
                {
                    // (AR) ?????? 1: ?????? ??? ?????
                    // (EN) Step 1: Get size
                    std::string sizeReg = newTempRegister();
                    SIRInstruction sizeInst(SIROpcode::ARRAY_LEN);
                    sizeInst.result = SIROperand::Register(sizeReg, SadTypeKind::Integer);
                    sizeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(sizeInst);

                    // (AR) ?????? 2: ?????? size == 0
                    // (EN) Step 2: Compare size == 0
                    std::string resultReg = newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::EQ);
                    cmpInst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(sizeReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(0));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(cmpInst);

                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // ================================================================
                // (AR) Fix #48: ??? ????????? ??????? ????????
                //      ??? ????? ???? ?????? ?? LLVM codegen (??? opcodes) ???
                //      ?? ??? ?????? ?? SIR builder � ??? ???? crash ??? ?????????
                //      ??? ????????? ???? ??? CALL ????? ???? ???? runtime ??????
                // (EN) Fix #48: Additional builtin array methods
                //      These methods had opcodes & LLVM codegen support but were not
                //      wired in SIR builder � causing ACCESS_VIOLATION crashes
                // ================================================================

                // (AR) ????? / contains � ??? ???? ???? ?? ???????? ?? ?? ???? ?? ????
                // (EN) contains — check if element exists in array or substring in string
                // (AR) — تمييز: إذا كان الكائن نصاً → BUILTIN_STRING_CONTAINS
                // (EN) — Disambiguate: if object is string → BUILTIN_STRING_CONTAINS
                if (methodName == "يحتوي" || methodName == "contains" ||
                    methodName == "includes")
                {
                    // (AR) ??????: ?? ?????? ??? ? ?????? BUILTIN_STRING_CONTAINS
                    // (EN) Check: is object a string? ? use BUILTIN_STRING_CONTAINS
                    if (objResult.type == SadTypeKind::String)
                    {
                        std::string resultReg = newTempRegister();
                        SIRInstruction inst(SIROpcode::BUILTIN_STRING_CONTAINS);
                        inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                        inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                        if (args.size() > 1)
                            inst.operands.push_back(args[1]);
                        if (currentBlock_)
                            currentBlock_->instructions.push_back(inst);
                        return BuildResult(resultReg, SadTypeKind::Boolean);
                    }
                    // (AR) ?? ?????? ?????? ? ?????? __sad_map_has
                    // (EN) Is object a map? ? use __sad_map_has
                    if (objResult.type == SadTypeKind::Map)
                    {
                        std::string resultReg = newTempRegister();
                        SIRInstruction inst;
                        inst.opcode = SIROpcode::CALL;
                        inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                        inst.operands.push_back(SIROperand::ConstantString("__sad_map_has"));
                        inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                        if (args.size() > 1)
                            inst.operands.push_back(args[1]);
                        inst.comment = "map has key (contains)";
                        if (currentBlock_)
                            currentBlock_->addInstruction(inst);
                        return BuildResult(resultReg, SadTypeKind::Boolean);
                    }
                    // (AR) ???? ???: ?????? ? BUILTIN_ARRAY_CONTAINS
                    // (EN) Otherwise: array ? BUILTIN_ARRAY_CONTAINS
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_CONTAINS);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                    {
                        inst.operands.push_back(args[1]); // (AR) ?????? ??????? ????? ???
                    }
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) ??? / sort � ????? ???????? ????????
                // (EN) sort � sort array in ascending order
                if (methodName == "رتب" || methodName == "sort")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SORT);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // (AR) ??? / ???? / ??? / reverse � ??? ????? ????? ????????
                // (EN) reverse � reverse array elements in-place
                if (methodName == "اعكس" || methodName == "عكس" ||
                    methodName == "عكّس" || methodName == "reverse")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REVERSE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // (AR) ???? / indexOf � ????? ???? ??? ???? ?????
                // (EN) indexOf � find index of first occurrence (-1 if not found)
                if (methodName == "فهرس" || methodName == "indexOf" ||
                    methodName == "index_of")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_INDEX_OF);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                    {
                        inst.operands.push_back(args[1]); // (AR) ?????? ??????? ????? ???
                    }
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ??? / first � ?????? ??? ??? ???? ?? ????????
                // (EN) first � get first element of the array
                if (methodName == "أول" || methodName == "اول" ||
                    methodName == "first")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_FIRST);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ??? / last � ?????? ??? ??? ???? ?? ????????
                // (EN) last � get last element of the array
                if (methodName == "آخر" || methodName == "اخر" ||
                    methodName == "last")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_LAST);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ????? / slice � ??????? ??? ?? ????????
                // (EN) slice � extract a sub-array from start to end
                if (methodName == "شريحة" || methodName == "slice")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SLICE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    // (AR) ??????? ??????: ????? ???????? ??????: ????? ???????
                    // (EN) Second arg: start index, Third: end index
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (args.size() > 2)
                        inst.operands.push_back(args[2]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // (AR) ????_???? / pop � ????? ??? ???? (????? ?? ???)
                // (EN) pop � remove and return last element (alias for ???)
                if (methodName == "احذف_اخير" || methodName == "احذف_آخر" ||
                    methodName == "pop_back")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REMOVE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ????? / ????? � ????? ????? (???? ????? ?? ??? ???????)
                // (EN) length property (aliased)
                if (methodName == "الطول" || methodName == "الحجم")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::ARRAY_LEN);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                return std::nullopt;
            }

            // === buildArrayHigherOrderMethodCall ===
            std::optional<BuildResult> SIRBuilder::buildArrayHigherOrderMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args,
                const std::string &firstClosureLambdaName, SadTypeKind firstClosureRetType)
            {
                // ================================================================
                // (AR) ??? ????????? ????? ??????? ?? ??????? (Higher-order Array Methods)
                //      ????? (map)? ??? (filter)? ????? (reduce)? ??? (forEach)
                //      ??????? ??? ????? (desugaring) ??? ???? SIR:
                //      - ARRAY_LEN ?????? ?????
                //      - ARRAY_NEW ?????? ?????? ????? (?????/???)
                //      - ????: ARRAY_GET + CLOSURE_CALL + (ARRAY_APPEND ?? ?????)
                //      ?? ????? opcodes ????? � ?? ??? ?????
                // (EN) Higher-order array methods with lambda
                //      map, filter, reduce, forEach
                //      Implemented via desugaring to SIR loop:
                //      - ARRAY_LEN for length
                //      - ARRAY_NEW for result array (map/filter)
                //      - loop: ARRAY_GET + CLOSURE_CALL + (ARRAY_APPEND or accumulate)
                //      No new opcodes needed � all primitives exist
                // ================================================================

                // (AR) ????? / map � ????? ????? ??? ?? ???? ?????? ?????? ?????
                // (EN) map � apply lambda to each element and return new array
                if (methodName == "خريطة" || methodName == "map")
                {
                    // (AR) ??????? ???? ??????? ?? args[1] (?????? ????? ??? self)
                    // (EN) Extract closure pointer from args[1] (first arg after self)
                    if (args.size() < 2)
                    {
                        errors_.push_back("Error: \u062e\u0631\u064a\u0637\u0629() requires a lambda argument");
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIROperand closureOp = args[1];

                    // (AR) ??? ????? ??????? � ????? ?? ???? ?????? 3
                    // (EN) Lambda return type � taken from Step 3 tracking
                    SadTypeKind lambdaRetType = firstClosureRetType;
                    if (lambdaRetType == SadTypeKind::Void)
                        lambdaRetType = SadTypeKind::Integer;

                    // (AR) ?????? 1: ?????? ??? ??? ???????? ??????
                    // (EN) Step 1: Get source array length
                    std::string lenReg = newTempRegister();
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                    lenInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->addInstruction(lenInst);

                    // (AR) ?????? 2: ????? ?????? ???????
                    // (EN) Step 2: Create result array
                    std::string resultArrReg = newTempRegister();
                    SIRInstruction newArrInst;
                    newArrInst.opcode = SIROpcode::ARRAY_NEW;
                    newArrInst.result = SIROperand::Register(resultArrReg, SadTypeKind::Array);
                    newArrInst.operands.push_back(SIROperand::ConstantI64(0));
                    newArrInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    newArrInst.comment = "map result array";
                    if (currentBlock_)
                        currentBlock_->addInstruction(newArrInst);

                    // (AR) ?????? 3: ????? ?????? i = 0
                    // (EN) Step 3: Initialize counter i = 0
                    std::string iVarReg = newTempRegister();
                    SIRInstruction iAllocInst(SIROpcode::ALLOC);
                    iAllocInst.result = SIROperand::Register(iVarReg, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->addInstruction(iAllocInst);
                    SIRInstruction iStoreInst(SIROpcode::STORE);
                    iStoreInst.operands.push_back(SIROperand::ConstantI64(0));
                    iStoreInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iStoreInst);

                    // (AR) ?????? 4: ????? ????? ???????? ??????
                    // (EN) Step 4: Create loop basic blocks
                    std::string condLabel = newLabel("map_cond");
                    std::string bodyLabel = newLabel("map_body");
                    std::string exitLabel = newLabel("map_exit");
                    auto condBlock = createBasicBlock(condLabel);
                    auto bodyBlock = createBasicBlock(bodyLabel);
                    auto exitBlock = createBasicBlock(exitLabel);
                    if (currentFunction_)
                    {
                        currentFunction_->addBasicBlock(condBlock);
                        currentFunction_->addBasicBlock(bodyBlock);
                        currentFunction_->addBasicBlock(exitBlock);
                    }

                    // (AR) ??? ??? ???? ?????
                    // (EN) Jump to condition block
                    SIRInstruction brCondInst = SIRInstruction::Branch(SIROperand::Label(condLabel));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(brCondInst);

                    // (AR) ?????? 5: ???? ????? � i < len
                    // (EN) Step 5: Condition block � i < len
                    currentBlock_ = condBlock;
                    std::string iLoadReg = newTempRegister();
                    SIRInstruction iLoadInst(SIROpcode::LOAD);
                    iLoadInst.result = SIROperand::Register(iLoadReg, SadTypeKind::Integer);
                    iLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iLoadInst);
                    std::string cmpReg = newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::LT);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(iLoadReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(cmpInst);
                    SIRInstruction brLoopInst = SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(bodyLabel),
                        SIROperand::Label(exitLabel));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(brLoopInst);

                    // (AR) ?????? 6: ???? ????? � elem = arr[i]; result = closure(elem); resultArr.append(result)
                    // (EN) Step 6: Body block � elem = arr[i]; result = closure(elem); resultArr.append(result)
                    currentBlock_ = bodyBlock;
                    // (AR) ????? i ??? ???? ?? ???? ?????
                    // (EN) Reload i in body block
                    std::string iBodyReg = newTempRegister();
                    SIRInstruction iBodyLoadInst(SIROpcode::LOAD);
                    iBodyLoadInst.result = SIROperand::Register(iBodyReg, SadTypeKind::Integer);
                    iBodyLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iBodyLoadInst);
                    // (AR) ??? ?????? arr[i]
                    // (EN) Get element arr[i]
                    std::string elemReg = newTempRegister();
                    SIRInstruction getInst(SIROpcode::ARRAY_GET);
                    getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                    getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    getInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(getInst);
                    // (AR) ??????? ??????? ??? ??????
                    // (EN) Call lambda on element
                    std::string callResReg = newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CLOSURE_CALL;
                    callInst.result = SIROperand::Register(callResReg, lambdaRetType);
                    callInst.operands.push_back(closureOp);
                    callInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (!firstClosureLambdaName.empty())
                        callInst.comment = "lambda:" + firstClosureLambdaName;
                    if (currentBlock_)
                        currentBlock_->addInstruction(callInst);
                    // (AR) ????? ??????? ??? ???????? ???????
                    // (EN) Append result to new array
                    std::string appendReg = newTempRegister();
                    SIRInstruction appendInst(SIROpcode::BUILTIN_ARRAY_APPEND);
                    appendInst.result = SIROperand::Register(appendReg, SadTypeKind::Void);
                    appendInst.operands.push_back(SIROperand::Register(resultArrReg, SadTypeKind::Array));
                    appendInst.operands.push_back(SIROperand::Register(callResReg, lambdaRetType));
                    if (currentBlock_)
                        currentBlock_->addInstruction(appendInst);
                    // (AR) ????? ?????? i = i + 1
                    // (EN) Increment counter i = i + 1
                    std::string incReg = newTempRegister();
                    SIRInstruction incInst(SIROpcode::ADD_I64);
                    incInst.result = SIROperand::Register(incReg, SadTypeKind::Integer);
                    incInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    incInst.operands.push_back(SIROperand::ConstantI64(1));
                    if (currentBlock_)
                        currentBlock_->addInstruction(incInst);
                    SIRInstruction iStoreBackInst(SIROpcode::STORE);
                    iStoreBackInst.operands.push_back(SIROperand::Register(incReg, SadTypeKind::Integer));
                    iStoreBackInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iStoreBackInst);
                    // (AR) ??? ???? ??? ???? ?????
                    // (EN) Jump back to condition block
                    SIRInstruction brBackInst = SIRInstruction::Branch(SIROperand::Label(condLabel));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(brBackInst);

                    // (AR) ?????? 7: ???? ?????? � ????? ???????? ???????
                    // (EN) Step 7: Exit block � return new array
                    currentBlock_ = exitBlock;
                    BuildResult mapResult(resultArrReg, SadTypeKind::Array);
                    mapResult.elementType = lambdaRetType;
                    return mapResult;
                }

                // (AR) ??? / filter � ????? ????? ???????? ?????? ?????
                // (EN) filter � filter array elements with a predicate lambda
                if (methodName == "رشح" || methodName == "filter")
                {
                    if (args.size() < 2)
                    {
                        errors_.push_back("Error: رشح() requires a lambda argument");
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIROperand closureOp = args[1];

                    // (AR) ?????? 1: ??? ???????? ??????
                    // (EN) Step 1: Source array length
                    std::string lenReg = newTempRegister();
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                    lenInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->addInstruction(lenInst);

                    // (AR) ?????? 2: ?????? ??????? ?????
                    // (EN) Step 2: Empty result array
                    std::string resultArrReg = newTempRegister();
                    SIRInstruction newArrInst;
                    newArrInst.opcode = SIROpcode::ARRAY_NEW;
                    newArrInst.result = SIROperand::Register(resultArrReg, SadTypeKind::Array);
                    newArrInst.operands.push_back(SIROperand::ConstantI64(0));
                    newArrInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    newArrInst.comment = "filter result array";
                    if (currentBlock_)
                        currentBlock_->addInstruction(newArrInst);

                    // (AR) ?????? 3: ????? ??????
                    // (EN) Step 3: Initialize counter
                    std::string iVarReg = newTempRegister();
                    SIRInstruction iAllocInst(SIROpcode::ALLOC);
                    iAllocInst.result = SIROperand::Register(iVarReg, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->addInstruction(iAllocInst);
                    SIRInstruction iStoreInst(SIROpcode::STORE);
                    iStoreInst.operands.push_back(SIROperand::ConstantI64(0));
                    iStoreInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iStoreInst);

                    // (AR) ?????? 4: ????? ?????
                    // (EN) Step 4: Create blocks
                    std::string condLabel = newLabel("filter_cond");
                    std::string bodyLabel = newLabel("filter_body");
                    std::string appendLabel = newLabel("filter_append");
                    std::string nextLabel = newLabel("filter_next");
                    std::string exitLabel = newLabel("filter_exit");
                    auto condBlock = createBasicBlock(condLabel);
                    auto bodyBlock = createBasicBlock(bodyLabel);
                    auto appendBlock = createBasicBlock(appendLabel);
                    auto nextBlock = createBasicBlock(nextLabel);
                    auto exitBlock = createBasicBlock(exitLabel);
                    if (currentFunction_)
                    {
                        currentFunction_->addBasicBlock(condBlock);
                        currentFunction_->addBasicBlock(bodyBlock);
                        currentFunction_->addBasicBlock(appendBlock);
                        currentFunction_->addBasicBlock(nextBlock);
                        currentFunction_->addBasicBlock(exitBlock);
                    }

                    // (AR) ??? ??? ?????
                    // (EN) Jump to condition
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) ?????? 5: ???? ????? � i < len
                    // (EN) Step 5: Condition block � i < len
                    currentBlock_ = condBlock;
                    std::string iLoadReg = newTempRegister();
                    SIRInstruction iLoadInst(SIROpcode::LOAD);
                    iLoadInst.result = SIROperand::Register(iLoadReg, SadTypeKind::Integer);
                    iLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iLoadInst);
                    std::string cmpReg = newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::LT);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(iLoadReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(cmpInst);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(bodyLabel),
                            SIROperand::Label(exitLabel)));

                    // (AR) ?????? 6: ???? ????? � elem = arr[i]; cond = closure(elem); if cond ? append
                    // (EN) Step 6: Body block � elem = arr[i]; cond = closure(elem); if cond ? append
                    currentBlock_ = bodyBlock;
                    std::string iBodyReg = newTempRegister();
                    SIRInstruction iBodyLoadInst(SIROpcode::LOAD);
                    iBodyLoadInst.result = SIROperand::Register(iBodyReg, SadTypeKind::Integer);
                    iBodyLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iBodyLoadInst);
                    std::string elemReg = newTempRegister();
                    SIRInstruction getInst(SIROpcode::ARRAY_GET);
                    getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                    getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    getInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(getInst);
                    // (AR) ??????? ????? ?????
                    // (EN) Call predicate lambda
                    std::string predReg = newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CLOSURE_CALL;
                    callInst.result = SIROperand::Register(predReg, SadTypeKind::Boolean);
                    callInst.operands.push_back(closureOp);
                    callInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (!firstClosureLambdaName.empty())
                        callInst.comment = "lambda:" + firstClosureLambdaName;
                    if (currentBlock_)
                        currentBlock_->addInstruction(callInst);
                    // (AR) ??? ????: ??? ???? ? ??? ??????? ???? ? ??????
                    // (EN) Conditional branch: if true ? append, else ? next
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(predReg, SadTypeKind::Boolean),
                            SIROperand::Label(appendLabel),
                            SIROperand::Label(nextLabel)));

                    // (AR) ???? ??????? � append element
                    // (EN) Append block
                    currentBlock_ = appendBlock;
                    std::string appendReg = newTempRegister();
                    SIRInstruction appendInst(SIROpcode::BUILTIN_ARRAY_APPEND);
                    appendInst.result = SIROperand::Register(appendReg, SadTypeKind::Void);
                    appendInst.operands.push_back(SIROperand::Register(resultArrReg, SadTypeKind::Array));
                    appendInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(appendInst);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(nextLabel)));

                    // (AR) ???? ?????? � ????? ?????? ?????? ?????
                    // (EN) Next block � increment and jump to condition
                    currentBlock_ = nextBlock;
                    std::string iNextReg = newTempRegister();
                    SIRInstruction iNextLoadInst(SIROpcode::LOAD);
                    iNextLoadInst.result = SIROperand::Register(iNextReg, SadTypeKind::Integer);
                    iNextLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iNextLoadInst);
                    std::string incReg = newTempRegister();
                    SIRInstruction incInst(SIROpcode::ADD_I64);
                    incInst.result = SIROperand::Register(incReg, SadTypeKind::Integer);
                    incInst.operands.push_back(SIROperand::Register(iNextReg, SadTypeKind::Integer));
                    incInst.operands.push_back(SIROperand::ConstantI64(1));
                    if (currentBlock_)
                        currentBlock_->addInstruction(incInst);
                    SIRInstruction iStoreBackInst(SIROpcode::STORE);
                    iStoreBackInst.operands.push_back(SIROperand::Register(incReg, SadTypeKind::Integer));
                    iStoreBackInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iStoreBackInst);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) ???? ??????
                    // (EN) Exit block
                    currentBlock_ = exitBlock;
                    BuildResult filterResult(resultArrReg, SadTypeKind::Array);
                    filterResult.elementType = SadTypeKind::Integer;
                    return filterResult;
                }

                // (AR) ????? / reduce � ????? ???????? ??? ???? ????? ??? ????? ???????
                // (EN) reduce � reduce array to single value via accumulator lambda
                if (methodName == "اختزل" || methodName == "reduce")
                {
                    // (AR) ?????(?????, ????_????????) � ??????? ????? ????? ??????? ?????? ??????????
                    // (EN) reduce(lambda, initial_value) � first arg is lambda, second is initial value
                    if (args.size() < 3)
                    {
                        errors_.push_back("Error: \u0627\u062e\u062a\u0632\u0644() requires a lambda and an initial value");
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIROperand closureOp = args[1]; // (AR) ???????
                    SIROperand initOp = args[2];    // (AR) ?????? ??????????

                    // (AR) ????? ??? ???????? (?? ?????? ??????????)
                    // (EN) Determine accumulator type (from initial value)
                    SadTypeKind accType = initOp.dataType;
                    if (accType == SadTypeKind::Void)
                        accType = SadTypeKind::Integer;

                    // (AR) ?????? 1: ??? ???????? ??????
                    // (EN) Step 1: Source array length
                    std::string lenReg = newTempRegister();
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                    lenInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->addInstruction(lenInst);

                    // (AR) ?????? 2: ????? ???????? ???????
                    // (EN) Step 2: Initialize accumulator and counter
                    std::string accVarReg = newTempRegister();
                    SIRInstruction accAllocInst(SIROpcode::ALLOC);
                    accAllocInst.result = SIROperand::Register(accVarReg, accType);
                    if (currentBlock_)
                        currentBlock_->addInstruction(accAllocInst);
                    SIRInstruction accStoreInst(SIROpcode::STORE);
                    accStoreInst.operands.push_back(initOp);
                    accStoreInst.operands.push_back(SIROperand::Register(accVarReg, accType));
                    if (currentBlock_)
                        currentBlock_->addInstruction(accStoreInst);

                    std::string iVarReg = newTempRegister();
                    SIRInstruction iAllocInst(SIROpcode::ALLOC);
                    iAllocInst.result = SIROperand::Register(iVarReg, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->addInstruction(iAllocInst);
                    SIRInstruction iStoreInst(SIROpcode::STORE);
                    iStoreInst.operands.push_back(SIROperand::ConstantI64(0));
                    iStoreInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iStoreInst);

                    // (AR) ?????? 3: ????? ?????
                    // (EN) Step 3: Create blocks
                    std::string condLabel = newLabel("reduce_cond");
                    std::string bodyLabel = newLabel("reduce_body");
                    std::string exitLabel = newLabel("reduce_exit");
                    auto condBlock = createBasicBlock(condLabel);
                    auto bodyBlock = createBasicBlock(bodyLabel);
                    auto exitBlock = createBasicBlock(exitLabel);
                    if (currentFunction_)
                    {
                        currentFunction_->addBasicBlock(condBlock);
                        currentFunction_->addBasicBlock(bodyBlock);
                        currentFunction_->addBasicBlock(exitBlock);
                    }

                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) ?????? 4: ???? ?????
                    // (EN) Step 4: Condition block
                    currentBlock_ = condBlock;
                    std::string iLoadReg = newTempRegister();
                    SIRInstruction iLoadInst(SIROpcode::LOAD);
                    iLoadInst.result = SIROperand::Register(iLoadReg, SadTypeKind::Integer);
                    iLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iLoadInst);
                    std::string cmpReg = newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::LT);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(iLoadReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(cmpInst);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(bodyLabel),
                            SIROperand::Label(exitLabel)));

                    // (AR) ?????? 5: ???? ????? � acc = closure(acc, elem); i++
                    // (EN) Step 5: Body block � acc = closure(acc, elem); i++
                    currentBlock_ = bodyBlock;
                    std::string iBodyReg = newTempRegister();
                    SIRInstruction iBodyLoadInst(SIROpcode::LOAD);
                    iBodyLoadInst.result = SIROperand::Register(iBodyReg, SadTypeKind::Integer);
                    iBodyLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iBodyLoadInst);
                    // (AR) ??? ??????
                    // (EN) Get element
                    std::string elemReg = newTempRegister();
                    SIRInstruction getInst(SIROpcode::ARRAY_GET);
                    getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                    getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    getInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(getInst);
                    // (AR) ????? ???????? ??????
                    // (EN) Load current accumulator
                    std::string accLoadReg = newTempRegister();
                    SIRInstruction accLoadInst(SIROpcode::LOAD);
                    accLoadInst.result = SIROperand::Register(accLoadReg, accType);
                    accLoadInst.operands.push_back(SIROperand::Register(accVarReg, accType));
                    if (currentBlock_)
                        currentBlock_->addInstruction(accLoadInst);
                    // (AR) ??????? ???????(??????, ????)
                    // (EN) Call lambda(accumulator, element)
                    std::string callResReg = newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CLOSURE_CALL;
                    callInst.result = SIROperand::Register(callResReg, accType);
                    callInst.operands.push_back(closureOp);
                    callInst.operands.push_back(SIROperand::Register(accLoadReg, accType));
                    callInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (!firstClosureLambdaName.empty())
                        callInst.comment = "lambda:" + firstClosureLambdaName;
                    if (currentBlock_)
                        currentBlock_->addInstruction(callInst);
                    // (AR) ????? ???????? ??????
                    // (EN) Store new accumulator
                    SIRInstruction accStoreBackInst(SIROpcode::STORE);
                    accStoreBackInst.operands.push_back(SIROperand::Register(callResReg, accType));
                    accStoreBackInst.operands.push_back(SIROperand::Register(accVarReg, accType));
                    if (currentBlock_)
                        currentBlock_->addInstruction(accStoreBackInst);
                    // (AR) ????? ??????
                    // (EN) Increment counter
                    std::string incReg = newTempRegister();
                    SIRInstruction incInst(SIROpcode::ADD_I64);
                    incInst.result = SIROperand::Register(incReg, SadTypeKind::Integer);
                    incInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    incInst.operands.push_back(SIROperand::ConstantI64(1));
                    if (currentBlock_)
                        currentBlock_->addInstruction(incInst);
                    SIRInstruction iStoreBackInst(SIROpcode::STORE);
                    iStoreBackInst.operands.push_back(SIROperand::Register(incReg, SadTypeKind::Integer));
                    iStoreBackInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iStoreBackInst);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) ?????? 6: ???? ?????? � ????? ?????? ???????? ???????
                    // (EN) Step 6: Exit block � load and return final accumulator
                    currentBlock_ = exitBlock;
                    std::string finalAccReg = newTempRegister();
                    SIRInstruction finalAccLoadInst(SIROpcode::LOAD);
                    finalAccLoadInst.result = SIROperand::Register(finalAccReg, accType);
                    finalAccLoadInst.operands.push_back(SIROperand::Register(accVarReg, accType));
                    if (currentBlock_)
                        currentBlock_->addInstruction(finalAccLoadInst);
                    return BuildResult(finalAccReg, accType);
                }

                // (AR) ??? / forEach � ????? ????? ??? ?? ???? ???? ?????
                // (EN) forEach � apply lambda to each element without returning
                if (methodName == "لكل" || methodName == "forEach" || methodName == "for_each")
                {
                    if (args.size() < 2)
                    {
                        errors_.push_back("Error: لكل() requires a lambda argument");
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIROperand closureOp = args[1];

                    // (AR) ?????? 1: ?????? ??? ?????
                    // (EN) Step 1: Get length
                    std::string lenReg = newTempRegister();
                    SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                    lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                    lenInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (currentBlock_)
                        currentBlock_->addInstruction(lenInst);

                    // (AR) ?????? 2: ????? ??????
                    // (EN) Step 2: Initialize counter
                    std::string iVarReg = newTempRegister();
                    SIRInstruction iAllocInst(SIROpcode::ALLOC);
                    iAllocInst.result = SIROperand::Register(iVarReg, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->addInstruction(iAllocInst);
                    SIRInstruction iStoreInst(SIROpcode::STORE);
                    iStoreInst.operands.push_back(SIROperand::ConstantI64(0));
                    iStoreInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iStoreInst);

                    // (AR) ?????? 3: ????? ?????
                    // (EN) Step 3: Create blocks
                    std::string condLabel = newLabel("foreach_cond");
                    std::string bodyLabel = newLabel("foreach_body");
                    std::string exitLabel = newLabel("foreach_exit");
                    auto condBlock = createBasicBlock(condLabel);
                    auto bodyBlock = createBasicBlock(bodyLabel);
                    auto exitBlock = createBasicBlock(exitLabel);
                    if (currentFunction_)
                    {
                        currentFunction_->addBasicBlock(condBlock);
                        currentFunction_->addBasicBlock(bodyBlock);
                        currentFunction_->addBasicBlock(exitBlock);
                    }

                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) ???? ?????
                    // (EN) Condition block
                    currentBlock_ = condBlock;
                    std::string iLoadReg = newTempRegister();
                    SIRInstruction iLoadInst(SIROpcode::LOAD);
                    iLoadInst.result = SIROperand::Register(iLoadReg, SadTypeKind::Integer);
                    iLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iLoadInst);
                    std::string cmpReg = newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::LT);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(iLoadReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(cmpInst);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                            SIROperand::Label(bodyLabel),
                            SIROperand::Label(exitLabel)));

                    // (AR) ???? ????? � elem = arr[i]; closure(elem); i++
                    // (EN) Body block � elem = arr[i]; closure(elem); i++
                    currentBlock_ = bodyBlock;
                    std::string iBodyReg = newTempRegister();
                    SIRInstruction iBodyLoadInst(SIROpcode::LOAD);
                    iBodyLoadInst.result = SIROperand::Register(iBodyReg, SadTypeKind::Integer);
                    iBodyLoadInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iBodyLoadInst);
                    std::string elemReg = newTempRegister();
                    SIRInstruction getInst(SIROpcode::ARRAY_GET);
                    getInst.result = SIROperand::Register(elemReg, SadTypeKind::Integer);
                    getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    getInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(getInst);
                    // (AR) ??????? ??????? (???? ??????? ???????)
                    // (EN) Call lambda (ignore result)
                    std::string callResReg = newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CLOSURE_CALL;
                    callInst.result = SIROperand::Register(callResReg, SadTypeKind::Void);
                    callInst.operands.push_back(closureOp);
                    callInst.operands.push_back(SIROperand::Register(elemReg, SadTypeKind::Integer));
                    if (!firstClosureLambdaName.empty())
                        callInst.comment = "lambda:" + firstClosureLambdaName;
                    if (currentBlock_)
                        currentBlock_->addInstruction(callInst);
                    // (AR) ????? ??????
                    // (EN) Increment counter
                    std::string incReg = newTempRegister();
                    SIRInstruction incInst(SIROpcode::ADD_I64);
                    incInst.result = SIROperand::Register(incReg, SadTypeKind::Integer);
                    incInst.operands.push_back(SIROperand::Register(iBodyReg, SadTypeKind::Integer));
                    incInst.operands.push_back(SIROperand::ConstantI64(1));
                    if (currentBlock_)
                        currentBlock_->addInstruction(incInst);
                    SIRInstruction iStoreBackInst(SIROpcode::STORE);
                    iStoreBackInst.operands.push_back(SIROperand::Register(incReg, SadTypeKind::Integer));
                    iStoreBackInst.operands.push_back(SIROperand::Register(iVarReg, SadTypeKind::Integer));
                    if (currentBlock_)
                        currentBlock_->addInstruction(iStoreBackInst);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) ???? ??????
                    // (EN) Exit block
                    currentBlock_ = exitBlock;
                    return BuildResult("", SadTypeKind::Void);
                }

                // ================================================================
                return std::nullopt;
            }

            // === buildStringBuiltinMethodCall ===
            std::optional<BuildResult> SIRBuilder::buildStringBuiltinMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args)
            {
                if (methodName == "قسم" || methodName == "تقسيم" ||
                    methodName == "قسّم" || methodName == "split")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SPLIT);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    BuildResult result(resultReg, SadTypeKind::Array);
                    result.elementType = SadTypeKind::String;
                    return result;
                }

                // (AR) ?????? / replace � ??????? ?? ????
                // (EN) replace � replace substring
                if (methodName == "استبدل" || methodName == "replace")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_REPLACE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]); // (AR) ???? ??????
                    if (args.size() > 2)
                        inst.operands.push_back(args[2]); // (AR) ???? ??????
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // (AR) ????_?? / starts_with � ??? ????? ????
                // (EN) starts_with � check if string starts with prefix
                if (methodName == "يبدأ_بـ" || methodName == "يبدا_ب" ||
                    methodName == "starts_with")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_STARTS_WITH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) ?????_?? / ends_with � ??? ????? ????
                // (EN) ends_with � check if string ends with suffix
                if (methodName == "ينتهي_بـ" || methodName == "ينتهي_ب" ||
                    methodName == "ends_with")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_ENDS_WITH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) ?? / trim � ????? ???????? ?? ???????
                // (EN) trim � remove whitespace from both ends
                if (methodName == "قص" || methodName == "قص_أطراف" ||
                    methodName == "trim")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_TRIM);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // (AR) ??? / substring � ??????? ??? ?? ????
                // (EN) substring � extract part of string
                if (methodName == "جزء" || methodName == "استخراج" ||
                    methodName == "substring" || methodName == "substr")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SUBSTRING);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]); // (AR) ???? ???????
                    if (args.size() > 2)
                        inst.operands.push_back(args[2]); // (AR) ????? (???????)
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                return std::nullopt;
            }

            // === buildMapBuiltinMethodCall ===
            std::optional<BuildResult> SIRBuilder::buildMapBuiltinMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args)
            {
                if (objResult.type != SadTypeKind::Map)
                    return std::nullopt;

                // (AR) ?????? / keys � ????? ?????? ?????? ???????
                // (EN) keys � return array of map keys
                if (methodName == "مفاتيح" || methodName == "keys")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_keys"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.comment = "map keys";
                    if (currentBlock_)
                        currentBlock_->addInstruction(inst);
                    BuildResult res(resultReg, SadTypeKind::Array);
                    res.elementType = SadTypeKind::String;
                    return res;
                }

                // (AR) ??? / values � ????? ?????? ??? ???????
                // (EN) values � return array of map values
                if (methodName == "قيم" || methodName == "values")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_values"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.comment = "map values";
                    if (currentBlock_)
                        currentBlock_->addInstruction(inst);
                    BuildResult res(resultReg, SadTypeKind::Array);
                    res.elementType = objResult.elementType;
                    return res;
                }

                // (AR) ??? / size � ??? ????? ???????
                // (EN) size � number of map entries
                if (methodName == "حجم" || methodName == "size")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_size"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.comment = "map size";
                    if (currentBlock_)
                        currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ????? / empty � ?? ??????? ?????
                // (EN) empty � is map empty
                if (methodName == "فارغة" || methodName == "empty" || methodName == "isEmpty")
                {
                    // (AR) ??? ? ?? ?????? == 0 ??? SIR EQ
                    std::string sizeReg = newTempRegister();
                    SIRInstruction sizeInst;
                    sizeInst.opcode = SIROpcode::CALL;
                    sizeInst.result = SIROperand::Register(sizeReg, SadTypeKind::Integer);
                    sizeInst.operands.push_back(SIROperand::ConstantString("__sad_map_size"));
                    sizeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    sizeInst.comment = "map size for empty check";
                    if (currentBlock_)
                        currentBlock_->addInstruction(sizeInst);

                    std::string resultReg = newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::EQ);
                    cmpInst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(sizeReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(0));
                    if (currentBlock_)
                        currentBlock_->addInstruction(cmpInst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) ????? / contains / has � ??? ???? ?????
                // (EN) contains / has � check if key exists
                if (methodName == "يحتوي" || methodName == "contains" || methodName == "has")
                {
                    std::string resultReg = newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_has"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    inst.comment = "map has key";
                    if (currentBlock_)
                        currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
