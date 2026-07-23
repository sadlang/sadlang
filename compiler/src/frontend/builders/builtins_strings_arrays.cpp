// ============================================================================
// sir_builder_builtins_strings_arrays.cpp
// ============================================================================
// (AR) دوال النصوص والمصفوفات المضمنة — مستخرج من sir_builder_builtins_core.cpp (CW-05)
//      طول_نص/تقسيم/استبدال/يحتوي/... + إضافة_عنصر/حذف/ترتيب/عكس/...
// (EN) String & array builtin functions — extracted from sir_builder_builtins_core.cpp
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "sir_builder.h"
#include "builtin_registry.h"
#include "error_manager.h" // (AR) buildBilingualMessage من كتالوج الأخطاء (م-2) / (EN) catalog bilingual messages (M-2)
#include "error_catalog.h" // (AR) RenderContext (حاملُ placeholders)
#include <stdexcept>
#include <iostream>
#include <optional>
#include <limits>

// (AR) اختصار لأسماء الدوال المركزية
namespace Bs = Sad::Builtins::Names::Strings;
namespace Ba = Sad::Builtins::Names::Arrays;
namespace Bmp = Sad::Builtins::Names::Maps;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            std::optional<BuildResult> BuiltinBuilder::buildBuiltinStringArrayCall(
                const std::string &funcName,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // ========================================================================
                // String Functions (12 functions)
                // ========================================================================

                // 1. طول_نص / string_length
                if (funcName == Bs::STR_LENGTH)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة طول_نص تتطلب معامل واحد (نص)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_LENGTH);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 1b. رمز_حرف / char_at - get character at index
                if (funcName == Bs::CHAR_CODE)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة رمز_حرف تتطلب معاملين (نص, فهرس)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_CHAR_AT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]); // string
                    inst.operands.push_back(argOperands[1]); // index
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 2. تحويل_كبير / toUpper
                if (funcName == Bs::TO_UPPER)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة تحويل_كبير تتطلب معامل واحد (نص)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_TO_UPPER);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 3. تحويل_صغير / toLower
                if (funcName == Bs::TO_LOWER)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة تحويل_صغير تتطلب معامل واحد (نص)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_TO_LOWER);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 4. بحث / find
                if (funcName == Bs::FIND)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة بحث تتطلب معاملين (نص, نص للبحث عنه)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_FIND);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 5. استبدل / replace
                if (funcName == Bs::REPLACE)
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[Error] دالة استبدل تتطلب 3 معاملات (نص, قديم, جديد)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_REPLACE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 6. استخراج / substring
                if (funcName == Bs::SUBSTRING)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة استخراج تتطلب على الأقل معاملين (نص, بداية)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SUBSTRING);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]); // string
                    inst.operands.push_back(argOperands[1]); // start
                    if (argResults.size() >= 3)
                    {
                        inst.operands.push_back(argOperands[2]); // end (optional)
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 7. قص_أطراف / trim
                if (funcName == Bs::TRIM)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة قص_أطراف تتطلب معامل واحد (نص)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_TRIM);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 8. تقسيم / split (returns array of strings)
                if (funcName == Bs::SPLIT)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة تقسيم تتطلب معاملين (نص, فاصل)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SPLIT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    // (AR) الوسيط الثالث الاختياريّ maxSplits — يُمرَّر إن وُجد ليطابق
                    //      المفسّر (StringFunctions::split يدعم حدًّا أقصى للتقسيمات).
                    // (EN) Optional 3rd arg maxSplits — forwarded when present for
                    //      parity with the interpreter's split(str, delim, maxSplits).
                    if (argOperands.size() >= 3)
                        inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    // (AR) تقسيم ترجع مصفوفة نصوص — تعيين elementType=String
                    //      ضروري لحلقة لكل: بدونه يُحمّل العنصر كـ i64 بدلاً من ptr
                    //      مما يطبع عناوين خام بدلاً من النصوص الفعلية
                    // (EN) Split returns array of strings — set elementType=String
                    //      Required for forEach: without it, elements load as i64 not ptr
                    BuildResult splitResult(resultReg, SadTypeKind::Array);
                    splitResult.elementType = SadTypeKind::String;
                    return splitResult;
                }

                // 9. دمج / join (array to string)
                if (funcName == Bs::JOIN)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة دمج تتطلب معاملين (مصفوفة, فاصل)" << std::endl;
                        return BuildResult("", SadTypeKind::String);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_JOIN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // 10. يبدأ_ب / startsWith
                if (funcName == Bs::STARTS_WITH)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة يبدأ_ب تتطلب معاملين (نص, بادئة)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_STARTS_WITH);
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

                // 11. ينتهي_ب / endsWith
                if (funcName == Bs::ENDS_WITH)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة ينتهي_ب تتطلب معاملين (نص, لاحقة)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_ENDS_WITH);
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

                // 12. يحتوي_على / contains
                if (funcName == Bs::CONTAINS)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة يحتوي_على تتطلب معاملين (نص, نص للبحث عنه)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_CONTAINS);
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

                // ========================================================================
                // Array Functions (10 functions)
                // ========================================================================

                // 1. إضافة_عنصر / append / أضف
                if (funcName == Ba::ADD)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة إضافة_عنصر تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_APPEND);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 2. إزالة_عنصر / remove / أزل / احذف
                if (funcName == Ba::REMOVE)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة إزالة_عنصر تتطلب معاملين (مصفوفة, فهرس)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REMOVE);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 3. حجم_مصفوفة / array_size / length
                if (funcName == Ba::SIZE)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة حجم_مصفوفة تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SIZE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 4. فهرس / indexOf (array)
                if (funcName == Ba::INDEX_OF)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة فهرس تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_INDEX_OF);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 5. يحتوي_عنصر / contains (array)
                if (funcName == Ba::ARRAY_CONTAINS)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة يحتوي_عنصر تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_CONTAINS);
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

                // 6. قلب / reverse
                if (funcName == Ba::REVERSE)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة قلب تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REVERSE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // 7. فرز / sort
                if (funcName == Ba::SORT)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة رتب تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SORT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    // (AR) تمرير نوع عنصر المصفوفة صراحةً إلى backend
                    //      حتى لا يُفترض الفرز الرقمي لجميع المصفوفات.
                    inst.operands.push_back(SIROperand::ConstantI64(
                        static_cast<int64_t>(argResults[0].elementType)));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    BuildResult result(resultReg, SadTypeKind::Array);
                    result.elementType = argResults[0].elementType;
                    return result;
                }

                // 8. أول / first
                if (funcName == Ba::FIRST)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة الأول تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_FIRST);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Void);
                }

                // 9. آخر / last
                if (funcName == Ba::LAST)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة الأخير تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_LAST);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Void);
                }

                // 10. شريحة / slice
                if (funcName == Ba::SLICE)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة شريحة تتطلب على الأقل معاملين (مصفوفة, بداية)" << std::endl;
                        return BuildResult("", SadTypeKind::Array);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);
                    SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SLICE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (argResults.size() >= 3)
                    {
                        inst.operands.push_back(argOperands[2]); // end (optional)
                    }
                    else
                    {
                        // (AR) [ISSUE-063] النهاية المحذوفة = حارس INT64_MIN «حتى آخر
                        //      المصفوفة»، لا حذفٌ: الخلفية تشترط 3 معاملات (وإلا خطأ).
                        // (EN) [ISSUE-063] Omitted end = INT64_MIN sentinel «to end of
                        //      array», not dropped: the backend requires 3 operands.
                        inst.operands.push_back(SIROperand::ConstantI64(std::numeric_limits<int64_t>::min()));
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // (AR) زاوج(أ، ب) — اقتران مصفوفتين أزواجًا (Maps::ZIP بعد إعادة
                //      التسمية من «ضم»): النتيجة مصفوفة أزواج بطول min(|أ|، |ب|)،
                //      كلّ زوج مصفوفة داخليّة من عنصرين. تُخفَّض إلى ARRAY_ZIP
                //      وتُولَّد حلقةً في الخلفيّة (بنية SadArray المتداخلة).
                // (EN) zip(a, b) — pair two arrays (Maps::ZIP, renamed from «ضم»):
                //      result is an array of two-element pair arrays of length
                //      min(|a|, |b|). Lowered to ARRAY_ZIP, emitted as a loop in
                //      the backend over the nested SadArray layout.
                if (funcName == Bmp::ZIP)
                {
                    // (AR) Amelia (م-2): رفض ترجمة صريح — الإرجاع الصامت كان يُنتج
                    //      BuildResult فارغًا ⇒ segfault عند الاستهلاك. الرسالة كتالوجيّة
                    //      (SEM005) عبر errors_ فيفشل البناء عبر hasErrors().
                    // (EN) Amelia (M-2): explicit compile rejection — the silent return
                    //      produced an empty BuildResult ⇒ segfault on consumption.
                    //      Catalog message (SEM005) pushed to errors_ fails the build.
                    if (argResults.size() < 2)
                    {
                        Sad::Errors::RenderContext ectx;
                        ectx.placeholders = {{"name", std::string(Bmp::ZIP)},
                                             {"expected", "2"},
                                             {"found", std::to_string(argResults.size())}};
                        b_.errors_.push_back(
                            Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                Sad::Errors::ErrorCode::SEM_WRONG_ARG_COUNT, ectx));
                        return BuildResult("", SadTypeKind::Array);
                    }
                    // (AR) حارس نوع بزمن الترجمة: معاملٌ معلومُ النوع سكونيًّا وليس مصفوفةً
                    //      (عدد/عشريّ/منطقيّ/نصّ) ⇒ SEM002 — زاوج(5، [1، 2]) كانت تنهار
                    //      segfault مترجَمةً بينما يرفضها المفسّر بخطأ نظيف. عند الجهل
                    //      السكونيّ (Any/Unknown/سجلّ عابر) لا حارس زمنيّ ممكن: خانات
                    //      المصفوفات مؤشّرات i64 خام بلا وسم نوعيّ (دَين موثَّق م-3).
                    // (EN) Compile-time type guard: a statically known non-array operand
                    //      (int/float/bool/string) ⇒ SEM002 — compiled زاوج(5, [1,2])
                    //      segfaulted while the interpreter rejects it cleanly. When the
                    //      type is statically unknown (Any/Unknown), no runtime guard is
                    //      possible: array slots are raw untagged i64 pointers (M-3 debt).
                    for (size_t zi = 0; zi < 2; ++zi)
                    {
                        const SadTypeKind zk = argResults[zi].type;
                        if (zk == SadTypeKind::Integer || zk == SadTypeKind::Float ||
                            zk == SadTypeKind::Boolean || zk == SadTypeKind::String)
                        {
                            Sad::Errors::RenderContext ectx;
                            ectx.placeholders = {{"expected", sirTypeToString(SadTypeKind::Array)},
                                                 {"found", sirTypeToString(zk)}};
                            b_.errors_.push_back(
                                Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                    Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH, ectx));
                            return BuildResult("", SadTypeKind::Array);
                        }
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::ARRAY_ZIP);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    BuildResult zr(resultReg, SadTypeKind::Array);
                    zr.elementType = SadTypeKind::Array;
                    return zr;
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad