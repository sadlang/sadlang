// ============================================================================
// sir_builder_builtins_strings_arrays.cpp
// ============================================================================
// (AR) دوال النصوص والمصفوفات المضمنة — مستخرج من sir_builder_builtins_core.cpp (CW-05)
//      طول_نص/تقسيم/استبدال/يحتوي/... + إضافة_عنصر/حذف/ترتيب/عكس/...
// (EN) String & array builtin functions — extracted from sir_builder_builtins_core.cpp
// ============================================================================

#include "sir_builder.h"
#include <stdexcept>
#include <iostream>
#include <optional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            std::optional<BuildResult> SIRBuilder::buildBuiltinStringArrayCall(
                const std::string &funcName,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // ========================================================================
                // String Functions (12 functions)
                // ========================================================================

                // 1. طول_نص / string_length
                if (funcName == "طول_نص" || funcName == "string_length" || funcName == "str_length" ||
                    funcName == "نص_طول" || funcName == "نص_الطول")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة طول_نص تتطلب معامل واحد (نص)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_LENGTH);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 1b. رمز_حرف / char_at - get character at index
                if (funcName == "رمز_حرف" || funcName == "char_at" || funcName == "charAt")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة رمز_حرف تتطلب معاملين (نص, فهرس)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_CHAR_AT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]); // string
                    inst.operands.push_back(argOperands[1]); // index
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 2. تحويل_كبير / toUpper
                if (funcName == "تحويل_كبير" || funcName == "toUpper" || funcName == "uppercase" || funcName == "لأكبر")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة تحويل_كبير تتطلب معامل واحد (نص)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_TO_UPPER);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 3. تحويل_صغير / toLower
                if (funcName == "تحويل_صغير" || funcName == "toLower" || funcName == "lowercase" || funcName == "لأصغر")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة تحويل_صغير تتطلب معامل واحد (نص)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_TO_LOWER);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 4. بحث / find
                if (funcName == "بحث" || funcName == "find" || funcName == "indexOf" || funcName == "ابحث")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة بحث تتطلب معاملين (نص, نص للبحث عنه)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_FIND);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 5. استبدل / replace
                if (funcName == "استبدل" || funcName == "replace" || funcName == "بدل")
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[Error] دالة استبدل تتطلب 3 معاملات (نص, قديم, جديد)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_REPLACE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 6. استخراج / substring
                if (funcName == "استخراج" || funcName == "substring" || funcName == "substr" || funcName == "slice")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة استخراج تتطلب على الأقل معاملين (نص, بداية)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SUBSTRING);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]); // string
                    inst.operands.push_back(argOperands[1]); // start
                    if (argResults.size() >= 3)
                    {
                        inst.operands.push_back(argOperands[2]); // end (optional)
                    }
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 7. قص_أطراف / trim
                if (funcName == "قص_أطراف" || funcName == "trim" || funcName == "strip")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة قص_أطراف تتطلب معامل واحد (نص)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_TRIM);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 8. تقسيم / split (returns array)
                if (funcName == "تقسيم" || funcName == "split")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة تقسيم تتطلب معاملين (نص, فاصل)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SPLIT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // 9. دمج / join (array to string)
                if (funcName == "دمج" || funcName == "join")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة دمج تتطلب معاملين (مصفوفة, فاصل)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_JOIN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 10. يبدأ_ب / startsWith
                if (funcName == "يبدأ_ب" || funcName == "startsWith" || funcName == "starts_with")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة يبدأ_ب تتطلب معاملين (نص, بادئة)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_STARTS_WITH);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 11. ينتهي_ب / endsWith
                if (funcName == "ينتهي_ب" || funcName == "endsWith" || funcName == "ends_with")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة ينتهي_ب تتطلب معاملين (نص, لاحقة)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_ENDS_WITH);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 12. يحتوي_على / contains
                if (funcName == "يحتوي_على" || funcName == "contains" || funcName == "includes")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة يحتوي_على تتطلب معاملين (نص, نص للبحث عنه)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_CONTAINS);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ========================================================================
                // Array Functions (10 functions)
                // ========================================================================

                // 1. إضافة_عنصر / append / أضف
                if (funcName == "إضافة_عنصر" || funcName == "append" || funcName == "push" || funcName == "add" || funcName == "أضف")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة إضافة_عنصر تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_APPEND);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 2. إزالة_عنصر / remove / أزل / احذف
                if (funcName == "إزالة_عنصر" || funcName == "remove" || funcName == "delete" || funcName == "pop" || funcName == "أزل" || funcName == "احذف")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة إزالة_عنصر تتطلب معاملين (مصفوفة, فهرس)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REMOVE);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 3. حجم_مصفوفة / array_size / length
                if (funcName == "حجم_مصفوفة" || funcName == "array_size" || funcName == "حجم")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة حجم_مصفوفة تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SIZE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 4. فهرس / indexOf (array)
                if (funcName == "فهرس_مصفوفة" || funcName == "array_indexOf" || funcName == "فهرس_عنصر")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة فهرس تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_INDEX_OF);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 5. يحتوي_عنصر / contains (array)
                if (funcName == "يحتوي_عنصر" || funcName == "array_contains" || funcName == "has")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة يحتوي_عنصر تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_CONTAINS);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 6. قلب / reverse
                if (funcName == "قلب" || funcName == "reverse" || funcName == "اعكس")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة قلب تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REVERSE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // 7. فرز / sort
                if (funcName == "فرز" || funcName == "sort" || funcName == "رتب")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة فرز تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SORT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // 8. أول / first
                if (funcName == "أول" || funcName == "first" || funcName == "العنصر_الأول")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة أول تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_FIRST);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Void);
                }

                // 9. آخر / last
                if (funcName == "آخر" || funcName == "last" || funcName == "العنصر_الأخير")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة آخر تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_LAST);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Void);
                }

                // 10. شريحة / slice
                if (funcName == "شريحة" || funcName == "array_slice" || funcName == "قطع")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة شريحة تتطلب على الأقل معاملين (مصفوفة, بداية)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SLICE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (argResults.size() >= 3)
                    {
                        inst.operands.push_back(argOperands[2]); // end (optional)
                    }
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Array);
                }


                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad