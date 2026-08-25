// ============================================================================
// sir_builder_builtins_io_utils.cpp
// ============================================================================
// (AR) دوال الملفات والأدوات وفحص الأنواع — مستخرج من sir_builder_builtins_core.cpp (CW-05)
//      اقرأ_ملف/اكتب_ملف/عشوائي/وقت/سكون/خروج/هو_رقم/هو_عشري/لمنطقي/...
// (EN) File I/O, utility, and type-checking builtins — extracted from sir_builder_builtins_core.cpp
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "builders/builtin_arity_check.h"
#include "sir_builder.h"
#include <stdexcept>
#include <iostream>
#include <optional>

#include "builtin_registry.h"
namespace Bn = Sad::Builtins::Names;
namespace Ar = Sad::Builtins::Arity;

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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::READ_FILE, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::WRITE_FILE, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::WRITE_BYTES, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::READ_BYTES, argResults.size()))
                        return BuildResult("", SadTypeKind::Array);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::APPEND_FILE, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::DELETE_FILE, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::COPY_FILE, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::MOVE_FILE, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::MKDIR, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::LIST_DIR, argResults.size()))
                        return BuildResult("", SadTypeKind::Array);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::IS_DIR, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::REMOVE_DIR, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::FILE_EXISTS, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::IS_FILE, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_IS_FILE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // ────────────────────────────────────────────────────────────────
                // 9-د. حجم_ملف / file_size — حجمُ الملفّ بالبايتات.
                //
                // (AR) كانت مُعلَنةً في مصدر الحقيقة (builtins/maps.yaml) ومُنفَّذةً في
                //      المفسّر (builtin_module_maps_utils.cpp) وغائبةً عن المصرّف
                //      وحده — فوحدةُ ص تستعملها تعمل مُفسَّرةً وتُرفَض مُصرَّفةً.
                //      ونبراس يُشحن مصرَّفًا، فالغيابُ حاجزٌ لا نقص.
                //      العقدُ مطابقٌ للمفسّر: عددٌ بالبايتات، و**سالبُ واحدٍ** إن تعذّر
                //      الفتح — قيمةٌ يفحصها المستدعي بدل انهيارٍ أو صفرٍ ملتبسٍ بملفٍّ فارغ.
                // (EN) Declared in the SoT and implemented in the interpreter but absent
                //      from the compiler, so a unit using it ran interpreted and was
                //      rejected compiled. Contract mirrors the interpreter: byte count,
                //      or -1 when the file cannot be opened (distinct from an empty file).
                // ────────────────────────────────────────────────────────────────
                if (funcName == Bn::Maps::FILE_SIZE)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Maps::FILE_SIZE, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_FILE_SIZE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // تعبير_بحث / regex_search — يُرجِع النصَّ المطابق، أو «فراغ» عند عدم
                // المطابقة (نظير المفسّر: match[0].str() أو قيمةُ فراغ).
                if (funcName == Bn::Maps::REGEX_SEARCH)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Maps::REGEX_SEARCH, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_REGEX_SEARCH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    // الرايةُ اختياريّة؛ غيابُها = بلا رايات (حسّاسٌ للحالة).
                    if (argOperands.size() > 2)
                        inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // تعبير_مطابقة / regex_match — مطابقةُ النصّ كاملًا ⇒ منطقيّ.
                if (funcName == Bn::Maps::REGEX)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Maps::REGEX, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_REGEX_MATCH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (argOperands.size() > 2)
                        inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // 10. هل_رابط_رمزي / is_symlink — يفحص المدخلَ نفسه بلا اتّباع الرابط.
                //     (AR) هل_ملف/هل_مجلد يتبعان الرابطَ فيصفان الهدف ⇒ لا يكشفان الرابط.
                if (funcName == Bn::Basics::IS_SYMLINK)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::IS_SYMLINK, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::REAL_PATH, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Basics::ABS_PATH, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerIo::IO_0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Core::TYPE, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_TYPE_OF);
                    inst.result = resultOp;

                    // ══════════════════════════════════════════════════════════════
                    // (AR) 🔑 `نوع()` يسألُ عن نوعٍ **دلاليٍّ**، والمُمرَّرُ إليه نوعُ **خانة**
                    // ══════════════════════════════════════════════════════════════
                    //
                    // (AR) المقيسُ قبلَ السدّ (ISSUE-152): `شخص ك = شخص()` ثمّ `نوع(ك)`
                    //      يُجيبُ **«رقم»** والمرجعُ «كائن» — بـrc=0 في الطرفَين.
                    //      وثلاثةُ أجوبةٍ بحسبِ **صيغةِ التصريحِ** لا المعنى:
                    //
                    //          شخص ك = شخص()      ⇒ نوعُ السِجِلِّ `1` (Integer) ⇒ «رقم»
                    //          متغير ك = شخص()    ⇒ نوعُ السِجِلِّ `22` (Struct) ⇒ «كائن» ✅
                    //          نوع(شخص()) مباشرةً  ⇒ `22` ⇒ «كائن» ✅
                    //
                    //      والعلّةُ أنّ `astTypeToSIRType` تُخفِضُ `Class`/`Struct` إلى **مقبضٍ
                    //      معتِمٍ** (i64) لأنّ لا بنيةَ لهما في SIR — وذلك قرارُ **تمثيلٍ**
                    //      صحيحٌ في موضعِه. والخطأُ أنّ `نوع()` يقرأُ ذلك المقبضَ جوابًا
                    //      دلاليًّا — فيُعلِنُ أنّ الكائنَ رقم.
                    //
                    //      ⚠️ **ولا يُصلَحُ برفعِ `Class`/`Struct` إلى الهويّةِ في جسرِ الأنواع**:
                    //      ذاك **تغييرُ ABI** ينهى عنه التعليقُ صراحةً في `sir_builder_helpers.cpp`
                    //      ويلزمُه قياسٌ مستقلٌّ على المحرّكَين. فالعلاجُ أن يُحمَلَ النوعُ
                    //      الدلاليُّ إلى **المُعامِلِ وحدَه** هنا، فلا يتغيّرُ تمثيلٌ ولا يُمَسُّ
                    //      نوعُ السِجِلِّ الذي يقرأُه سواه.
                    //
                    //      ⚠️ والتعدادُ الجبريُّ مستثنًى عمدًا: `className` يحملُ اسمَ التعدادِ
                    //      أيضًا (لتعملَ `طابق`)، والمرجعُ يُجيبُ عنه «خريطة» لا «كائن» —
                    //      فإدخالُه هنا يُبدِلُ كذبًا بكذب (ISSUE-153).
                    // (EN) `نوع()` asks for a SEMANTIC type; what reaches it is a STORAGE kind.
                    //      astTypeToSIRType lowers Class/Struct to an opaque i64 handle — a correct
                    //      REPRESENTATION decision — and نوع() then reads that handle as the answer,
                    //      declaring an object to be a number. Lifting Class/Struct to identity in
                    //      the type bridge is an ABI change the bridge's own comment forbids without
                    //      independent measurement, so the semantic kind is carried on THIS OPERAND
                    //      only: no representation changes and no other reader is affected. ADTs are
                    //      deliberately excluded — className also holds enum names (so `match` works)
                    //      and the reference answers «map» for them, so including them would swap one
                    //      lie for another (ISSUE-153).
                    // ══════════════════════════════════════════════════════════════
                    SIROperand typeOfOperand = argOperands[0];
                    const std::string &operandClassName = argResults[0].className;
                    // (AR) ⚠️ والشرطُ ضيِّقَ مرّتَينِ بقياسٍ، وكلُّ قيدٍ منهما منعَ انحدارًا:
                    //   ① `Integer` وحدَه: هو المقبضُ الذي ضاعَ فيه المعنى. وما وصلَ
                    //     بنوعِه الصحيحِ (Struct · Future · عنصر_واجهة …) لا يُمَسُّ.
                    //   ② واسمٌ هو **لفظُ نوعٍ سطحيٌّ** (من SoT) ليس صنفًا للمستخدِم.
                    // (AR) 🔴 والمقيس: بلا القيدَينِ كان `متغير م = مستقبل()` ثمّ `نوع(م)`
                    //     يُجيبُ «كائن» والمرجعُ «مستقبل» — بذرةُ `060_future_type_wiring`
                    //     حمراءُ، ولم تكن في الخمسةِ المرجعيّة. أي أنّ سدًّا يرفعُ نوعًا
                    //     دلاليًّا قد يدوسُ نوعًا دلاليًّا أدقَّ منه واصلًا أصلًا.
                    // (EN) Narrowed twice by measurement, each guard preventing a regression:
                    //   1. Integer only — that IS the handle where the meaning was lost; anything
                    //      arriving with its real kind (Struct, Future, Widget, …) is left alone.
                    //   2. a name that is a SURFACE TYPE WORD (per the SoT) is not a user class.
                    //   Measured: without these, `var f = future()` then `نوع(f)` answered «object»
                    //   where the reference says «future» — seed 060_future_type_wiring went red and
                    //   was NOT one of the five baseline failures. A fix that raises a semantic type
                    //   can trample a MORE precise semantic type that was already arriving intact.
                    if (!operandClassName.empty() &&
                        argOperands[0].dataType == SadTypeKind::Integer &&
                        b_.adtEnumTable_.find(operandClassName) == b_.adtEnumTable_.end() &&
                        ::Sad::Types::sadTypeKindFromArabicName(operandClassName) ==
                            Sad::Types::SadTypeKind::Unknown)
                    {
                        typeOfOperand.dataType = SadTypeKind::Class;
                    }
                    inst.operands.push_back(typeOfOperand);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Strings::IS_INT, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Strings::IS_FLOAT, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Strings::IS_STRING, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Strings::IS_ARRAY, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::TypeCtor::TO_BOOL, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
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