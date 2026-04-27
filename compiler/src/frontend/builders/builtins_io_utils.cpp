// ============================================================================
// sir_builder_builtins_io_utils.cpp
// ============================================================================
// (AR) دوال الملفات والأدوات وفحص الأنواع — مستخرج من sir_builder_builtins_core.cpp (CW-05)
//      اقرأ_ملف/اكتب_ملف/عشوائي/وقت/سكون/خروج/هو_رقم/هو_عشري/لمنطقي/...
// (EN) File I/O, utility, and type-checking builtins — extracted from sir_builder_builtins_core.cpp
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
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
            std::optional<BuildResult> BuiltinBuilder::buildBuiltinIOUtilsCall(
                const std::string &funcName,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // ========================================================================
                // File I/O Functions (8 functions)
                // ========================================================================

                // 1. اقرأ_ملف / read_file
                if (funcName == "اقرأ_ملف" || funcName == "read_file" || funcName == "readFile")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة اقرأ_ملف تتطلب معامل واحد (مسار الملف)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_READ);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 2. اكتب_ملف / write_file
                if (funcName == "اكتب_ملف" || funcName == "write_file" || funcName == "writeFile")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة اكتب_ملف تتطلب معاملين (مسار, محتوى)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_WRITE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 3. أضف_إلى_ملف / append_to_file
                if (funcName == "أضف_إلى_ملف" || funcName == "append_to_file" || funcName == "appendFile")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة أضف_إلى_ملف تتطلب معاملين (مسار, محتوى)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_APPEND);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 4. احذف_ملف / delete_file
                if (funcName == "احذف_ملف" || funcName == "delete_file" || funcName == "deleteFile")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة احذف_ملف تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_DELETE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 5. انسخ_ملف / copy_file
                if (funcName == "انسخ_ملف" || funcName == "copy_file" || funcName == "copyFile")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة انسخ_ملف تتطلب معاملين (مصدر, وجهة)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_COPY);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 6. انقل_ملف / move_file
                if (funcName == "انقل_ملف" || funcName == "move_file" || funcName == "moveFile")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة انقل_ملف تتطلب معاملين (مصدر, وجهة)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_MOVE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 7. أنشئ_مجلد / create_dir
                if (funcName == "أنشئ_مجلد" || funcName == "create_dir" || funcName == "createDir" || funcName == "mkdir")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة أنشئ_مجلد تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_CREATE_DIR);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 8. اسرد_مجلد / list_dir
                if (funcName == "اسرد_مجلد" || funcName == "list_dir" || funcName == "listDir" || funcName == "ls")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة اسرد_مجلد تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_LIST_DIR);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // ========================================================================
                // Utility Functions (4 functions)
                // ========================================================================

                // 1. عشوائي / random
                if (funcName == "عشوائي" || funcName == "random" || funcName == "rand")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::BUILTIN_RANDOM);
                    inst.result = resultOp;
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // 2. نم / sleep
                if (funcName == "نم" || funcName == "sleep" || funcName == "wait")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة نم تتطلب معامل واحد (مدة بالثواني)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_SLEEP);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 3. اخرج / exit
                if (funcName == "اخرج" || funcName == "exit" || funcName == "quit")
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_EXIT);
                    if (!argResults.empty())
                    {
                        inst.operands.push_back(argOperands[0]); // exit code (optional)
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 4. النوع / type_of
                if (funcName == "النوع" || funcName == "type_of" || funcName == "typeof" || funcName == "نوع")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة النوع تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_TYPE_OF);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // ========================================================================
                // Type Checking Functions (4 functions)
                // ========================================================================

                // هو_رقم / هو_رقم_صحيح / is_int
                if (funcName == "هو_رقم" || funcName == "هو_رقم_صحيح" || funcName == "is_int" || funcName == "isInt")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة هو_رقم تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_IS_INTEGER);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // هو_عشري / هو_رقم_عشري / is_float
                if (funcName == "هو_عشري" || funcName == "هو_رقم_عشري" || funcName == "is_float" || funcName == "isFloat")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة هو_عشري تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_IS_FLOAT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // هو_نص / is_string / isString
                if (funcName == "هو_نص" || funcName == "is_string" || funcName == "isString")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة هو_نص تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_IS_STRING);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // هو_مصفوفة / is_array / isArray
                if (funcName == "هو_مصفوفة" || funcName == "is_array" || funcName == "isArray")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة هو_مصفوفة تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_IS_ARRAY);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ========================================================================
                // Additional Conversion Functions
                // ========================================================================

                // لمنطقي / to_bool / منطقي
                if (funcName == "لمنطقي" || funcName == "to_bool" || funcName == "bool" || funcName == "منطقي")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة لمنطقي تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_TO_BOOL);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // ========================================================================
                // Additional IO Functions
                // ========================================================================

                // قراءة_سطر / readLine
                if (funcName == "قراءة_سطر" || funcName == "readLine" || funcName == "read_line")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_READ_LINE);
                    inst.result = resultOp;
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // مسح_الشاشة / clear / cls
                if (funcName == "مسح_الشاشة" || funcName == "clear" || funcName == "cls")
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_CLEAR_SCREEN);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad