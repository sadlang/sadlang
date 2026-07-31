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

#include "builtin_registry.h"
namespace Bn = Sad::Builtins::Names;

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
                if (funcName == Bn::Basics::READ_FILE)
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
                if (funcName == Bn::Basics::WRITE_FILE)
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

                // 2ب. اكتب_بايتات / write_bytes — كتابة بايتات خام (fwrite، تكتب الصفريّة)
                if (funcName == Bn::Basics::WRITE_BYTES)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة اكتب_بايتات تتطلب معاملين (مسار, مصفوفة بايتات)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_WRITE_BYTES);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]); // (AR) المسار (نصّ)
                    inst.operands.push_back(argOperands[1]); // (AR) مصفوفة البايتات (SadArray*)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 2ج. اقرأ_بايتات / read_bytes — قراءة بايتات خام إلى مصفوفة أعداد
                if (funcName == Bn::Basics::READ_BYTES)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة اقرأ_بايتات تتطلب معامل واحد (مسار الملف)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_READ_BYTES);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // 3. أضف_إلى_ملف / append_to_file
                if (funcName == Bn::Basics::APPEND_FILE)
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
                if (funcName == Bn::Basics::DELETE_FILE)
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
                if (funcName == Bn::Basics::COPY_FILE)
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
                if (funcName == Bn::Basics::MOVE_FILE)
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
                if (funcName == Bn::Basics::MKDIR)
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
                if (funcName == Bn::Basics::LIST_DIR)
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

                // 9. هل_مجلد / is_dir — فحص إن كان المسار مجلدًا (stat)
                if (funcName == Bn::Basics::IS_DIR)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة هل_مجلد تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_IS_DIR);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 9-أ. احذف_مجلد / remove_dir — يحذف مجلّدًا فارغًا (نظير rmdir).
                if (funcName == Bn::Basics::REMOVE_DIR)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة احذف_مجلد تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_REMOVE_DIR);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 9-ب. هل_موجود / exists — أيُّ مدخلٍ موجود (ملفًّا كان أو مجلّدًا).
                if (funcName == Bn::Basics::FILE_EXISTS)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة هل_موجود تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_EXISTS);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 9-ج. هل_ملف / is_file — ملفٌّ عاديّ (يتبع الرابطَ فيصف هدفه، كنظيره في المفسّر).
                if (funcName == Bn::Basics::IS_FILE)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة هل_ملف تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_IS_FILE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 10. هل_رابط_رمزي / is_symlink — يفحص المدخلَ نفسه بلا اتّباع الرابط.
                //     (AR) هل_ملف/هل_مجلد يتبعان الرابطَ فيصفان الهدف ⇒ لا يكشفان الرابط.
                if (funcName == Bn::Basics::IS_SYMLINK)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة هل_رابط_رمزي تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_IS_SYMLINK);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 11. المسار_الحقيقي / real_path — يحلّ الروابط ويطبّع «..».
                //     (AR) أساسُ فرضِ احتواء المسارات: التطبيعُ النصّيّ وحده يُخترَق برابطٍ
                //     رمزيّ يشير خارج الجذر. يُرجع نصًّا، أو عدمًا إن تعذّر الحلّ.
                if (funcName == Bn::Basics::REAL_PATH)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة المسار_الحقيقي تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_REAL_PATH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 12. المسار_المطلق / abs_path — تطبيعٌ نصّيّ بلا حلِّ الروابط.
                //     (AR) يعمل على مسارٍ غير موجود (خلافًا للحقيقيّ) ⇒ لا يصلح وحده
                //     لفرض الاحتواء الأمنيّ: رابطٌ رمزيّ يخترق التطبيعَ النصّيّ.
                if (funcName == Bn::Basics::ABS_PATH)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة المسار_المطلق تتطلب معامل واحد (مسار)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_ABS_PATH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // ========================================================================
                // Utility Functions (4 functions)
                // ========================================================================

                // 1. عشوائي / random
                if (funcName == Bn::Math::RANDOM)
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
                if (funcName == Bn::CompilerIo::IO_0)
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
                if (funcName == Bn::Basics::EXIT_ALT)
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
                if (funcName == Bn::Core::TYPE)
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
                if (funcName == Bn::Strings::IS_INT)
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
                if (funcName == Bn::Strings::IS_FLOAT)
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
                if (funcName == Bn::Strings::IS_STRING)
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
                if (funcName == Bn::Strings::IS_ARRAY)
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
                if (funcName == Bn::TypeCtor::TO_BOOL)
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
                if (funcName == Bn::CompilerIo::IO_1)
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
                if (funcName == Bn::CompilerIo::IO_2)
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