// ============================================================================
// sir_builder_builtins_ffi.cpp
// (AR) FFI builtins: printf, malloc, fopen, C/C++ interop
// (EN) C/C++ Foreign Function Interface (20 functions)
// ============================================================================

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
#include <optional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_FFI(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ========================================================================
                // (AR) التكامل مع C/C++ — FFI Functions (20 دالة)
                // (EN) C/C++ Foreign Function Interface — 20 functions
                // ========================================================================

                // 1. طباعة_تنسيق / printf — formatted print (variadic)
                if (funcName == "\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9_\xd8\xaa\xd9\x86\xd8\xb3\xd9\x8a\xd9\x82" || funcName == "printf" || funcName == "c_printf")
                {
                    if (argOperands.empty())
                    {
                        std::cerr << "[ERROR] printf requires at least 1 argument (format string)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_PRINTF);
                    inst.result = resultOp;
                    for (size_t i = 0; i < argOperands.size(); i++)
                    {
                        inst.operands.push_back(argOperands[i]);
                    }
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin printf() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 2. حجز / malloc — allocate memory
                if (funcName == "\xd8\xad\xd8\xac\xd8\xb2" || funcName == "malloc" || funcName == "c_malloc")
                {
                    if (argOperands.size() != 1)
                    {
                        std::cerr << "[ERROR] malloc requires 1 argument (size)" << std::endl;
                        return BuildResult("", SadTypeKind::Pointer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_MALLOC);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin malloc() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 3. حرر / free — free memory
                if (funcName == "\xd8\xad\xd8\xb1\xd8\xb1" || funcName == "free" || funcName == "c_free")
                {
                    if (argOperands.size() != 1)
                    {
                        std::cerr << "[ERROR] free requires 1 argument (pointer)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::FFI_FREE);
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin free()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 4. اعد_حجز / realloc
                if (funcName == "\xd8\xa7\xd8\xb9\xd8\xaf_\xd8\xad\xd8\xac\xd8\xb2" || funcName == "realloc" || funcName == "c_realloc")
                {
                    if (argOperands.size() != 2)
                    {
                        std::cerr << "[ERROR] realloc requires 2 arguments (ptr, size)" << std::endl;
                        return BuildResult("", SadTypeKind::Pointer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_REALLOC);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 5. حجز_صفري / calloc
                if (funcName == "\xd8\xad\xd8\xac\xd8\xb2_\xd8\xb5\xd9\x81\xd8\xb1\xd9\x8a" || funcName == "calloc" || funcName == "c_calloc")
                {
                    if (argOperands.size() != 2)
                    {
                        std::cerr << "[ERROR] calloc requires 2 arguments (count, size)" << std::endl;
                        return BuildResult("", SadTypeKind::Pointer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_CALLOC);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 6. طول_نص_س / strlen
                if (funcName == "\xd8\xb7\xd9\x88\xd9\x84_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strlen" || funcName == "c_strlen")
                {
                    if (argOperands.size() != 1)
                    {
                        std::cerr << "[ERROR] strlen requires 1 argument (string)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_STRLEN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 7. انسخ_نص_س / strcpy
                if (funcName == "\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strcpy" || funcName == "c_strcpy")
                {
                    if (argOperands.size() != 2)
                    {
                        std::cerr << "[ERROR] strcpy requires 2 arguments (dest, src)" << std::endl;
                        return BuildResult("", SadTypeKind::Pointer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_STRCPY);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 8. قارن_نص_س / strcmp
                if (funcName == "\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strcmp" || funcName == "c_strcmp")
                {
                    if (argOperands.size() != 2)
                    {
                        std::cerr << "[ERROR] strcmp requires 2 arguments (s1, s2)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_STRCMP);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 9. الحق_نص_س / strcat
                if (funcName == "\xd8\xa7\xd9\x84\xd8\xad\xd9\x82_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strcat" || funcName == "c_strcat")
                {
                    if (argOperands.size() != 2)
                    {
                        std::cerr << "[ERROR] strcat requires 2 arguments (dest, src)" << std::endl;
                        return BuildResult("", SadTypeKind::Pointer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_STRCAT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 10. انسخ_ذاكرة_س / memcpy
                if (funcName == "\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9_\xd8\xb3" || funcName == "memcpy" || funcName == "c_memcpy")
                {
                    if (argOperands.size() != 3)
                    {
                        std::cerr << "[ERROR] memcpy requires 3 arguments (dest, src, size)" << std::endl;
                        return BuildResult("", SadTypeKind::Pointer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_MEMCPY);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 11. عبئ_ذاكرة_س / memset
                if (funcName == "\xd8\xb9\xd8\xa8\xd8\xa6_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9_\xd8\xb3" || funcName == "memset" || funcName == "c_memset")
                {
                    if (argOperands.size() != 3)
                    {
                        std::cerr << "[ERROR] memset requires 3 arguments (ptr, value, size)" << std::endl;
                        return BuildResult("", SadTypeKind::Pointer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_MEMSET);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 12. افتح_ملف_س / fopen
                if (funcName == "\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fopen" || funcName == "c_fopen")
                {
                    if (argOperands.size() != 2)
                    {
                        std::cerr << "[ERROR] fopen requires 2 arguments (filename, mode)" << std::endl;
                        return BuildResult("", SadTypeKind::Pointer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_FOPEN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 13. اغلق_ملف_س / fclose
                if (funcName == "\xd8\xa7\xd8\xba\xd9\x84\xd9\x82_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fclose" || funcName == "c_fclose")
                {
                    if (argOperands.size() != 1)
                    {
                        std::cerr << "[ERROR] fclose requires 1 argument (file pointer)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_FCLOSE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 14. اكتب_ملف_س / fputs — write string to file
                if (funcName == "\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fputs" || funcName == "c_fputs")
                {
                    if (argOperands.size() != 2)
                    {
                        std::cerr << "[ERROR] fputs requires 2 arguments (string, file)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_FWRITE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 15. اقرأ_ملف_س / fgets — read line from file
                if (funcName == "\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fgets" || funcName == "c_fgets")
                {
                    if (argOperands.size() != 3)
                    {
                        std::cerr << "[ERROR] fgets requires 3 arguments (buffer, size, file)" << std::endl;
                        return BuildResult("", SadTypeKind::Pointer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_FREAD);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 16. نفذ_امر / system — execute system command
                if (funcName == "\xd9\x86\xd9\x81\xd8\xb0_\xd8\xa7\xd9\x85\xd8\xb1" || funcName == "system" || funcName == "c_system")
                {
                    if (argOperands.size() != 1)
                    {
                        std::cerr << "[ERROR] system requires 1 argument (command)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_SYSTEM);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 17. قيمة_بيئة / getenv
                if (funcName == "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9_\xd8\xa8\xd9\x8a\xd8\xa6\xd8\xa9" || funcName == "getenv" || funcName == "c_getenv")
                {
                    if (argOperands.size() != 1)
                    {
                        std::cerr << "[ERROR] getenv requires 1 argument (name)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::FFI_GETENV);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 18. نص_لعدد / atoi
                if (funcName == "\xd9\x86\xd8\xb5_\xd9\x84\xd8\xb9\xd8\xaf\xd8\xaf" || funcName == "atoi" || funcName == "c_atoi")
                {
                    if (argOperands.size() != 1)
                    {
                        std::cerr << "[ERROR] atoi requires 1 argument (string)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_ATOI);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 19. نص_لعشري / atof
                if (funcName == "\xd9\x86\xd8\xb5_\xd9\x84\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a" || funcName == "atof" || funcName == "c_atof")
                {
                    if (argOperands.size() != 1)
                    {
                        std::cerr << "[ERROR] atof requires 1 argument (string)" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::FFI_ATOF);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // 20. تنسيق_نص / snprintf — format to buffer (variadic)
                if (funcName == "\xd8\xaa\xd9\x86\xd8\xb3\xd9\x8a\xd9\x82_\xd9\x86\xd8\xb5" || funcName == "snprintf" || funcName == "c_snprintf")
                {
                    if (argOperands.size() < 3)
                    {
                        std::cerr << "[ERROR] snprintf requires at least 3 arguments (buf, size, fmt)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_SNPRINTF);
                    inst.result = resultOp;
                    for (size_t i = 0; i < argOperands.size(); i++)
                    {
                        inst.operands.push_back(argOperands[i]);
                    }
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad