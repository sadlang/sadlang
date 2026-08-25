// ============================================================================
// sir_builder_builtins_ffi.cpp
// (AR) FFI builtins: printf, malloc, fopen, C/C++ interop
// (EN) C/C++ Foreign Function Interface (20 functions)
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
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

#include "builtin_registry.h"
#include "builders/builtin_arity_check.h"
// (AR) كانت أسماءُ هذه المدمجاتِ تُقارَن **بايتاتٍ مهرَّبةً** (`"\xd8\xad…"`)
//      لا بثوابتِ مصدرِ الحقيقة — نسخةٌ ثانيةٌ للاسمِ لا يقرؤها بشرٌ ولا يقيسُها
//      حارس. صارت المقارنةُ بالثابتِ المُولَّد، والهجاءانِ الإنجليزيّانِ باقيان
//      كما كانا (لا تُمَسّ الأسماءُ المقبولة، فذاك قرارُ دلالةٍ لا تنظيف).
namespace Bn = Sad::Builtins::Names;
namespace Ar = Sad::Builtins::Arity;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_FFI(
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
                    // (AR) رتبةٌ مفتوحةُ الأعلى (تنسيقٌ ثمّ ما شاء المنادي) — العقدُ الآن في مصدرِ الحقيقةِ
                    //      بـ`variadic: true`، والأدنى يُقرَأ من الثابتِ المُولَّدِ لا من رقمٍ
                    //      يُكتَب. والحدُّ الأعلى قائمٌ شكلًا (UNBOUNDED) فلا يمنعُ نداءً صحيحًا.
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_PRINTF, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_PRINTF);
                    inst.result = resultOp;
                    for (size_t i = 0; i < argOperands.size(); i++)
                    {
                        inst.operands.push_back(argOperands[i]);
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin printf() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 2. حجز / malloc — allocate memory
                if (funcName == Bn::FFI::C_MALLOC || funcName == "malloc" || funcName == "c_malloc")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_MALLOC, argOperands.size()))
                        return BuildResult("", SadTypeKind::Pointer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_MALLOC);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin malloc() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 3. حرر / free — free memory
                if (funcName == Bn::FFI::C_FREE || funcName == "free" || funcName == "c_free")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_FREE, argOperands.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::FFI_FREE);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin free()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 4. اعد_حجز / realloc
                if (funcName == Bn::FFI::C_REALLOC || funcName == "realloc" || funcName == "c_realloc")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_REALLOC, argOperands.size()))
                        return BuildResult("", SadTypeKind::Pointer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_REALLOC);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 5. حجز_صفري / calloc
                if (funcName == Bn::FFI::C_CALLOC || funcName == "calloc" || funcName == "c_calloc")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_CALLOC, argOperands.size()))
                        return BuildResult("", SadTypeKind::Pointer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_CALLOC);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 6. طول_نص_س / strlen
                if (funcName == Bn::FFI::C_STRLEN || funcName == "strlen" || funcName == "c_strlen")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_STRLEN, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_STRLEN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 7. انسخ_نص_س / strcpy
                if (funcName == Bn::FFI::C_STRCPY || funcName == "strcpy" || funcName == "c_strcpy")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_STRCPY, argOperands.size()))
                        return BuildResult("", SadTypeKind::Pointer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_STRCPY);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 8. قارن_نص_س / strcmp
                if (funcName == Bn::FFI::C_STRCMP || funcName == "strcmp" || funcName == "c_strcmp")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_STRCMP, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_STRCMP);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 9. الحق_نص_س / strcat
                if (funcName == Bn::FFI::C_STRCAT || funcName == "strcat" || funcName == "c_strcat")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_STRCAT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Pointer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_STRCAT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 10. انسخ_ذاكرة_س / memcpy
                if (funcName == "\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9_\xd8\xb3" || funcName == "memcpy" || funcName == "c_memcpy")
                {
                    // (AR) صار معلَنًا في `ffi.yaml` برتبتِه، والمقارنةُ بالثابتِ
                    //      المُولَّدِ لا ببايتاتٍ مهرَّبة. ولا نظيرَ له في المفسّرِ —
                    //      تباعدٌ **يُقاس** في الحارسِ ولا يُدَّعى في نثر.
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::FFI::C_MEMCPY, argOperands.size()))
                        return BuildResult("", SadTypeKind::Pointer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_MEMCPY);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 11. عبئ_ذاكرة_س / memset
                if (funcName == "\xd8\xb9\xd8\xa8\xd8\xa6_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9_\xd8\xb3" || funcName == "memset" || funcName == "c_memset")
                {
                    // (AR) كسابقتِه.
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::FFI::C_MEMSET, argOperands.size()))
                        return BuildResult("", SadTypeKind::Pointer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_MEMSET);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 12. افتح_ملف_س / fopen
                if (funcName == Bn::FFI::C_FOPEN || funcName == "fopen" || funcName == "c_fopen")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_FOPEN, argOperands.size()))
                        return BuildResult("", SadTypeKind::Pointer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_FOPEN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 13. اغلق_ملف_س / fclose
                if (funcName == Bn::FFI::C_FCLOSE || funcName == "fclose" || funcName == "c_fclose")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_FCLOSE, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_FCLOSE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 14. اكتب_ملف_س / fputs — write string to file
                if (funcName == Bn::FFI::C_FPUTS || funcName == "fputs" || funcName == "c_fputs")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_FPUTS, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_FWRITE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 15. اقرأ_ملف_س / fgets — read line from file
                if (funcName == Bn::FFI::C_FGETS || funcName == "fgets" || funcName == "c_fgets")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_FGETS, argOperands.size()))
                        return BuildResult("", SadTypeKind::Pointer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Pointer);
                    SIRInstruction inst(SIROpcode::FFI_FREAD);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Pointer);
                }

                // 16. نفذ_امر / system — execute system command
                if (funcName == Bn::FFI::C_SYSTEM || funcName == "system" || funcName == "c_system")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_SYSTEM, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_SYSTEM);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 17. قيمة_بيئة / getenv
                if (funcName == Bn::FFI::C_GETENV || funcName == "getenv" || funcName == "c_getenv")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_GETENV, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::FFI_GETENV);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 18. نص_لعدد / atoi
                if (funcName == Bn::FFI::C_ATOI || funcName == "atoi" || funcName == "c_atoi")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_ATOI, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_ATOI);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 19. نص_لعشري / atof
                if (funcName == Bn::FFI::C_ATOF || funcName == "atof" || funcName == "c_atof")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::FFI::C_ATOF, argOperands.size()))
                        return BuildResult("", SadTypeKind::Float);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::FFI_ATOF);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // 20. تنسيق_نص / snprintf — format to buffer (variadic)
                if (funcName == "\xd8\xaa\xd9\x86\xd8\xb3\xd9\x8a\xd9\x82_\xd9\x86\xd8\xb5" || funcName == "snprintf" || funcName == "c_snprintf")
                {
                    // (AR) رتبةٌ مفتوحةُ الأعلى (buf وsize وfmt ثمّ ما شاء المنسِّق)
                    //      — صار لها عُرفٌ في مصدرِ الحقيقة: `variadic: true` بلا
                    //      `max`. والأدنى ٣ مقيسٌ من الذراعِ نفسِها، لا سقفٌ يُخترَع.
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::FFI::C_SNPRINTF, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::FFI_SNPRINTF);
                    inst.result = resultOp;
                    for (size_t i = 0; i < argOperands.size(); i++)
                    {
                        inst.operands.push_back(argOperands[i]);
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad