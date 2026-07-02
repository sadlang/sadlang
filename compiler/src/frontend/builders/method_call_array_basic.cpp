// ============================================================================
// sir_builder_method_call_array_basic.cpp
// ============================================================================
// (AR) طرق المصفوفات الأساسية: أضف/حجم/أزل/فارغة/يحتوي/رتب/عكس/شريحة
// (EN) Array basic methods: push/size/remove/empty/contains/sort/reverse/slice
// ============================================================================

#include <string>
#include <optional>
#include "sir_builder.h"
#include "builders/method_call_builder.h"
// (AR) ثوابت أسماء طرق الأنواع المُولَّدة
#include "builtin_registry.h"

namespace TM = Sad::Builtins::Names::TypeMethods;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            std::optional<BuildResult> MethodCallBuilder::buildArrayBasicMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args)
            {
                if (methodName == TM::Array::PUSH)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_APPEND);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Void);
                    // (AR) ??????? ?????: ????????? ??????: ?????? ???????
                    // (EN) First operand: array, Second: element to add
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                    {
                        inst.operands.push_back(args[1]); // (AR) العنصر (args[0] هو self)
                    }
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(inst);
                    }
                    return BuildResult(resultReg, SadTypeKind::Void);
                }

                // (AR) ??? / size - ?????? ??? ??? ???????? ?? ??? ????
                // (EN) size / length - get array size or string length
                // (AR) ? ????? ??? ???????? ?????: ??? ??? ?????? ???? ? BUILTIN_STRING_LENGTH
                // (EN) ? Disambiguate: if object is string ? BUILTIN_STRING_LENGTH
                if (methodName == TM::Array::LENGTH)
                {
                    // (AR) ??????: ?? ?????? ??? ? ?????? BUILTIN_STRING_LENGTH
                    // (EN) Check: is object a string? ? use BUILTIN_STRING_LENGTH
                    if (objResult.type == SadTypeKind::String)
                    {
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction inst(SIROpcode::BUILTIN_STRING_LENGTH);
                        inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                        inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(inst);
                        return BuildResult(resultReg, SadTypeKind::Integer);
                    }
                    // (AR) ???? ???: ?????? ? ARRAY_LEN
                    // (EN) Otherwise: array ? ARRAY_LEN
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ARRAY_LEN);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ??? / pop - ????? ??? ????
                // (EN) pop / remove - remove last element
                if (methodName == TM::Array::REMOVE)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REMOVE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer); // (AR) ?????? ???????
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ????? / empty - ?????? ??? ???? ???????? ?????
                // (EN) empty / is_empty - check if array is empty
                // (AR) ?????? ARRAY_LEN ?????? ?? 0
                // (EN) Use ARRAY_LEN and compare with 0
                if (methodName == TM::Array::IS_EMPTY)
                {
                    // (AR) ?????? 1: ?????? ??? ?????
                    // (EN) Step 1: Get size
                    std::string sizeReg = b_.newTempRegister();
                    SIRInstruction sizeInst(SIROpcode::ARRAY_LEN);
                    sizeInst.result = SIROperand::Register(sizeReg, SadTypeKind::Integer);
                    sizeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(sizeInst);

                    // (AR) ?????? 2: ?????? size == 0
                    // (EN) Step 2: Compare size == 0
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::EQ);
                    cmpInst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(sizeReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(0));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(cmpInst);

                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // ================================================================
                // (AR) Fix #48: ??? ????????? ??????? ????????
                //      ??? ????? ???? ?????? ?? LLVM codegen (??? opcodes) ???
                //      ?? ??? ?????? ?? SIR builder — ??? ???? crash ??? ?????????
                //      ??? ????????? ???? ??? CALL ????? ???? ???? runtime ??????
                // (EN) Fix #48: Additional builtin array methods
                //      These methods had opcodes & LLVM codegen support but were not
                //      wired in SIR builder — causing ACCESS_VIOLATION crashes
                // ================================================================

                // (AR) ????? / contains — ??? ???? ???? ?? ???????? ?? ?? ???? ?? ????
                // (EN) contains — check if element exists in array or substring in string
                // (AR) — تمييز: إذا كان الكائن نصاً → BUILTIN_STRING_CONTAINS
                // (EN) — Disambiguate: if object is string → BUILTIN_STRING_CONTAINS
                if (methodName == TM::Array::CONTAINS)
                {
                    // (AR) ??????: ?? ?????? ??? ? ?????? BUILTIN_STRING_CONTAINS
                    // (EN) Check: is object a string? ? use BUILTIN_STRING_CONTAINS
                    if (objResult.type == SadTypeKind::String)
                    {
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction inst(SIROpcode::BUILTIN_STRING_CONTAINS);
                        inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                        inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                        if (args.size() > 1)
                            inst.operands.push_back(args[1]);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(inst);
                        return BuildResult(resultReg, SadTypeKind::Boolean);
                    }
                    // (AR) ?? ?????? ?????? ? ?????? __sad_map_has
                    // (EN) Is object a map? ? use __sad_map_has
                    if (objResult.type == SadTypeKind::Map)
                    {
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction inst;
                        inst.opcode = SIROpcode::CALL;
                        inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                        inst.operands.push_back(SIROperand::ConstantString("__sad_map_has"));
                        inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                        if (args.size() > 1)
                            inst.operands.push_back(args[1]);
                        inst.comment = "map has key (contains)";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(inst);
                        return BuildResult(resultReg, SadTypeKind::Boolean);
                    }
                    // (AR) ???? ???: ?????? ? BUILTIN_ARRAY_CONTAINS
                    // (EN) Otherwise: array ? BUILTIN_ARRAY_CONTAINS
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_CONTAINS);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                    {
                        inst.operands.push_back(args[1]); // (AR) العنصر (args[0] هو self)
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) رتب / sort — فرز المصفوفة تصاعدياً
                // (EN) sort — sort array in ascending order
                // (AR) [Fix #099] تمرير نوع عنصر المصفوفة (elementType) كمعامل ثانٍ
                //      بدونه: الـ codegen يفترض مقارن أعداد (i64 cmp) لجميع المصفوفات
                //      مما يجعل ترتيب مصفوفات النصوص غير محدد (يقارن عناوين ذاكرة)
                //      الحل: نفس نمط الدالة المدمجة فرز() في sir_builder_builtins_strings_arrays.cpp
                // (EN) [Fix #099] Pass array elementType as 2nd operand
                //      Without it: codegen defaults to i64 comparator for all arrays
                //      causing non-deterministic sort order for string arrays (compares pointers)
                //      Fix: same pattern as builtin sort() in sir_builder_builtins_strings_arrays.cpp
                if (methodName == TM::Array::SORT)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SORT);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    // (AR) تمرير نوع العنصر صراحةً ليختار الـ codegen المقارن الصحيح
                    //      String → __sad_str_cmp (strcmp)، Integer → __sad_i64_cmp
                    // (EN) Pass element type explicitly so codegen picks the right comparator
                    //      String → __sad_str_cmp (strcmp), Integer → __sad_i64_cmp
                    inst.operands.push_back(SIROperand::ConstantI64(
                        static_cast<int64_t>(objResult.elementType)));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    BuildResult result(resultReg, SadTypeKind::Array);
                    result.elementType = objResult.elementType;
                    return result;
                }

                // (AR) ??? / ???? / ??? / reverse — ??? ????? ????? ????????
                // (EN) reverse — reverse array elements in-place
                if (methodName == TM::Array::REVERSE)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REVERSE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // (AR) ???? / indexOf — ????? ???? ??? ???? ?????
                // (EN) indexOf — find index of first occurrence (-1 if not found)
                if (methodName == TM::Array::INDEX_OF)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_INDEX_OF);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                    {
                        inst.operands.push_back(args[1]); // (AR) العنصر (args[0] هو self)
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ??? / first — ?????? ??? ??? ???? ?? ????????
                // (EN) first — get first element of the array
                if (methodName == TM::Array::FIRST)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_FIRST);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ??? / last — ?????? ??? ??? ???? ?? ????????
                // (EN) last — get last element of the array
                if (methodName == TM::Array::LAST)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_LAST);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ????? / slice — ??????? ??? ?? ????????
                // (EN) slice — extract a sub-array from start to end
                if (methodName == TM::Array::SLICE)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SLICE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    // (AR) ??????? ??????: ????? ???????? ??????: ????? ???????
                    // (EN) Second arg: start index, Third: end index
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (args.size() > 2)
                        inst.operands.push_back(args[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // (AR) ????_???? / pop — ????? ??? ???? (????? ?? ???)
                // (EN) pop — remove and return last element (alias for ???)
                if (methodName == TM::Array::POP)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REMOVE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ????? / ????? — ????? ????? (???? ????? ?? ??? ???????)
                // (EN) length property (aliased)
                if (methodName == TM::Array::LENGTH)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ARRAY_LEN);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                return std::nullopt;
            }

            // === buildArrayHigherOrderMethodCall ===
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
