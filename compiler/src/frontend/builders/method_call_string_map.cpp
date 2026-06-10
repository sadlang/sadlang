// ============================================================================
// sir_builder_method_call_string_map.cpp
// ============================================================================
// (AR) طرق النصوص والخرائط المضمنة
// (EN) String and map built-in methods
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
            std::optional<BuildResult> MethodCallBuilder::buildStringBuiltinMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args)
            {
                if (methodName == TM::String::SPLIT)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SPLIT);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    BuildResult result(resultReg, SadTypeKind::Array);
                    result.elementType = SadTypeKind::String;
                    return result;
                }

                // (AR) ?????? / replace � ??????? ?? ????
                // (EN) replace � replace substring
                if (methodName == TM::String::REPLACE)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_REPLACE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]); // (AR) ???? ??????
                    if (args.size() > 2)
                        inst.operands.push_back(args[2]); // (AR) ???? ??????
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // (AR) ????_?? / starts_with � ??? ????? ????
                // (EN) starts_with � check if string starts with prefix
                if (methodName == TM::String::STARTS_WITH)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_STARTS_WITH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) ?????_?? / ends_with � ??? ????? ????
                // (EN) ends_with � check if string ends with suffix
                if (methodName == TM::String::ENDS_WITH)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_ENDS_WITH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) ?? / trim � ????? ???????? ?? ???????
                // (EN) trim � remove whitespace from both ends
                if (methodName == TM::String::TRIM)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_TRIM);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // (AR) ??? / substring � ??????? ??? ?? ????
                // (EN) substring � extract part of string
                if (methodName == TM::String::SUBSTRING)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SUBSTRING);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]); // (AR) ???? ???????
                    if (args.size() > 2)
                        inst.operands.push_back(args[2]); // (AR) ????? (???????)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                return std::nullopt;
            }

            // === buildMapBuiltinMethodCall ===
            std::optional<BuildResult> MethodCallBuilder::buildMapBuiltinMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args)
            {
                if (objResult.type != SadTypeKind::Map)
                    return std::nullopt;

                // ================================================================
                // (AR) احصل / get — قراءة قيمة من الخريطة بالمفتاح
                //      يُرجع التمثيل النصي دائماً عبر __sad_map_get (ذكي: يحوّل الأرقام لنص)
                //      args: [0]=self(map), [1]=key(string)
                // (EN) get — read value from map by key
                //      Always returns string representation via __sad_map_get (smart: converts ints to string)
                // ================================================================
                if (methodName == TM::Map::GET)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_get"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    inst.comment = "map get";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // ================================================================
                // (AR) عيّن / set — تعيين قيمة في الخريطة
                //      يستخدم __sad_map_set_typed مع نوع القيمة (0=نص, 1=رقم, 2=عشري, 3=منطقي)
                //      args: [0]=self(map), [1]=key(string), [2]=value
                // (EN) set — set value in map by key
                //      Uses __sad_map_set_typed with type tag
                // ================================================================
                if (methodName == TM::Map::SET)
                {
                    if (args.size() < 3)
                        return BuildResult("", SadTypeKind::Integer);

                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    std::string resultReg = b_.newTempRegister();
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_set_typed"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.operands.push_back(args[1]); // (AR) المفتاح / key
                    inst.operands.push_back(args[2]); // (AR) القيمة / value

                    // (AR) تحديد نوع القيمة: 0=نص, 1=رقم, 2=عشري, 3=منطقي
                    // (EN) Determine value type tag
                    int typeTag = 0; // (AR) افتراضي: نص
                    if (args[2].dataType == SadTypeKind::Integer)
                        typeTag = 1;
                    else if (args[2].dataType == SadTypeKind::Float)
                        typeTag = 2;
                    else if (args[2].dataType == SadTypeKind::Boolean)
                        typeTag = 3;
                    inst.operands.push_back(SIROperand::ConstantI64(typeTag));
                    inst.comment = "map set typed";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ================================================================
                // (AR) احذف / delete — حذف عنصر من الخريطة بالمفتاح
                //      args: [0]=self(map), [1]=key(string)
                // (EN) delete — remove entry from map by key
                // ================================================================
                if (methodName == TM::Map::DELETE)
                {
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    std::string resultReg = b_.newTempRegister();
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_delete"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    inst.comment = "map delete";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) مفاتيح / keys — إرجاع مصفوفة مفاتيح الخريطة
                // (EN) keys — return array of map keys
                if (methodName == TM::Map::KEYS)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_keys"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.comment = "map keys";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    BuildResult res(resultReg, SadTypeKind::Array);
                    res.elementType = SadTypeKind::String;
                    return res;
                }

                // (AR) ??? / values � ????? ?????? ??? ???????
                // (EN) values � return array of map values
                if (methodName == TM::Map::VALUES)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_values"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.comment = "map values";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    BuildResult res(resultReg, SadTypeKind::Array);
                    res.elementType = objResult.elementType;
                    return res;
                }

                // (AR) ??? / size � ??? ????? ???????
                // (EN) size � number of map entries
                if (methodName == TM::Map::SIZE)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_size"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.comment = "map size";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ????? / empty � ?? ??????? ?????
                // (EN) empty � is map empty
                if (methodName == TM::Map::IS_EMPTY)
                {
                    // (AR) ??? ? ?? ?????? == 0 ??? SIR EQ
                    std::string sizeReg = b_.newTempRegister();
                    SIRInstruction sizeInst;
                    sizeInst.opcode = SIROpcode::CALL;
                    sizeInst.result = SIROperand::Register(sizeReg, SadTypeKind::Integer);
                    sizeInst.operands.push_back(SIROperand::ConstantString("__sad_map_size"));
                    sizeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    sizeInst.comment = "map size for empty check";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(sizeInst);

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::EQ);
                    cmpInst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(sizeReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(0));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(cmpInst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) ????? / contains / has � ??? ???? ?????
                // (EN) contains / has � check if key exists
                if (methodName == TM::Map::CONTAINS)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_has"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    inst.comment = "map has key";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
