// ============================================================================
// sir_builder.cpp - بناء SIR من AST / SIR Builder from AST
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 2.0 (إعادة كتابة كاملة من الصفر)
//
// ملاحظة هامة / Important Note:
// هذا الملف مُعاد كتابته بالكامل من الصفر بما يتوافق 100% مع:
// This file is completely rewritten from scratch to comply 100% with:
// - STRICT_CODING_RULES.md
// - sir_builder.h
// - sir_types.h
// - sir_instruction.h
// - sir_module.h
// - AST headers (ast_node.h, expressions.h, statements.h, declarations.h)
// ============================================================================

#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
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

            // ============================================================================
            // sir_builder_builtins_core.cpp
            // (AR) ״§„״¯ˆ״§„ ״§„…״¯…״¬״© ״§„״£״³״§״³״©: ״×״­ˆ„ ״§„״£†ˆ״§״¹״ ״§„״·״¨״§״¹״©״ ״§„״±״§״¶״§״×״ ״§„†״µˆ״µ״ ״§„…״µˆ״§״×״ ״§„…„״§״×
            // (EN) Core builtins: type conversion, print, math, string, array, file operations
            // ============================================================================

            std::optional<BuildResult> SIRBuilder::buildBuiltinCallCore(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // (AR) إذا كانت الدالة معرّفة من المستخدم، لا تعامل كدالة مضمنة
                // (EN) If function is user-defined, skip all builtins
                if (isUserDefinedFunction)
                    return std::nullopt;

                if (funcName == "طول" || funcName == "length")
                {
                    if (argResults.size() != 1)
                    {
                        errors_.push_back("Error: طول() requires exactly 1 argument");
                        return BuildResult();
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);

                    // (AR) تحديد نوع العملية بناءً على نوع المعامل
                    // (EN) Determine operation based on argument type
                    SIROpcode opcode = (argResults[0].type == SadTypeKind::String)
                                           ? SIROpcode::STRING_LEN // (sir_types.h:181)
                                           : SIROpcode::ARRAY_LEN; // (sir_types.h:176)

                    SIRInstruction lenInst(opcode);
                    lenInst.result = resultOp;
                    lenInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(lenInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin طول() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة لرقم() - STRING_TO_I64
                // (EN) to_int() function - STRING_TO_I64
                // الأسماء المدعومة: لرقم, حول_رقم, to_int, int, إلى_رقم, رقم
                if (funcName == "لرقم" || funcName == "حول_رقم" || funcName == "to_int" || funcName == "int" || funcName == "إلى_رقم" || funcName == "رقم")
                {
                    if (argResults.size() != 1)
                    {
                        errors_.push_back("Error: لرقم() requires exactly 1 argument");
                        return BuildResult();
                    }

                    if (argResults[0].type == SadTypeKind::Integer)
                    {
                        return argResults[0];
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);

                    SIROpcode convOpcode = SIROpcode::STRING_TO_I64;
                    if (argResults[0].type == SadTypeKind::Float)
                    {
                        convOpcode = SIROpcode::F64_TO_I64;
                    }
                    else if (argResults[0].type == SadTypeKind::Boolean)
                    {
                        convOpcode = SIROpcode::BOOL_TO_I64;
                    }

                    SIRInstruction convInst(convOpcode); // (sir_types.h:187)
                    convInst.result = resultOp;
                    convInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(convInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin لرقم() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة لعشري() - STRING_TO_F64
                // (EN) to_float() function
                // الأسماء المدعومة: لعشري, to_float, float, عشري
                if (funcName == "لعشري" || funcName == "to_float" || funcName == "float" || funcName == "عشري")
                {
                    if (argResults.size() != 1)
                    {
                        errors_.push_back("Error: لعشري() requires exactly 1 argument");
                        return BuildResult();
                    }

                    if (argResults[0].type == SadTypeKind::Float)
                    {
                        return argResults[0];
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIROpcode convOpcode = SIROpcode::STRING_TO_F64;
                    if (argResults[0].type == SadTypeKind::Integer || argResults[0].type == SadTypeKind::Boolean)
                    {
                        convOpcode = SIROpcode::I64_TO_F64;
                    }

                    SIRInstruction convInst(convOpcode); // (AR) تحويل إلى عشري بحسب نوع الوسيط (EN) Convert to float based on source type
                    convInst.result = resultOp;
                    convInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(convInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin لعشري() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة لنص() - I64_TO_STRING أو F64_TO_STRING
                // (EN) to_string() function
                // الأسماء المدعومة: لنص, نص, to_string, str, string, إلى_نص
                if (funcName == "لنص" || funcName == "نص" || funcName == "to_string" ||
                    funcName == "str" || funcName == "string" || funcName == "إلى_نص")
                {
                    if (argResults.size() != 1)
                    {
                        errors_.push_back("Error: لنص() requires exactly 1 argument");
                        return BuildResult();
                    }

                    // (AR) إذا كان المعامل نصاً بالفعل — لا حاجة للتحويل، أرجعه مباشرة
                    //      يحدث مع f-strings: ص"مرحبا {الاسم}" → str(الاسم) حيث الاسم نص
                    //      بدون هذا: I64_TO_STRING يحوّل مؤشر النص كرقم → يطبع عنوان الذاكرة
                    // (EN) If argument is already a String — no conversion needed, return as-is
                    //      Happens with f-strings: f"hello {name}" → str(name) where name is string
                    //      Without this: I64_TO_STRING converts string pointer as number → prints address
                    if (argResults[0].type == SadTypeKind::String)
                    {
                        return argResults[0];
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);

                    SIROpcode opcode = (argResults[0].type == SadTypeKind::Float)
                                           ? SIROpcode::F64_TO_STRING  // (sir_types.h:210)
                                           : SIROpcode::I64_TO_STRING; // (sir_types.h:209)

                    SIRInstruction convInst(opcode);
                    convInst.result = resultOp;
                    convInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(convInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin لنص() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // (AR) دالة اطبع() - BUILTIN_PRINT
                // (EN) print() function
                // الأسماء المدعومة: اطبع, print, إطبع
                if (funcName == "اطبع" || funcName == "print" || funcName == "إطبع")
                {
                    // ====================================================================
                    // (AR) تحويل تلقائي: إذا كان المعامل كائناً ولديه __op_tostring__، استدعها
                    // (EN) Auto-convert: if arg is object with __op_tostring__, call it first
                    // يتوافق مع: expression_evaluator_calls.cpp findOperator("نص")
                    // ====================================================================
                    std::vector<SIROperand> resolvedOps;
                    for (size_t i = 0; i < argResults.size(); ++i)
                    {
                        if (!argResults[i].className.empty())
                        {
                            // (AR) البحث في سلسلة الوراثة عن __op_tostring__
                            // (EN) Search inheritance chain for __op_tostring__
                            std::string searchClass = argResults[i].className;
                            std::string tostrName;
                            bool foundToStr = false;
                            while (!searchClass.empty())
                            {
                                tostrName = searchClass + ".__op_tostring__";
                                if (functionTable_.find(tostrName) != functionTable_.end())
                                {
                                    foundToStr = true;
                                    break;
                                }
                                auto parentClass = module_->getClass(searchClass);
                                if (parentClass && !parentClass->parentClass.empty())
                                {
                                    searchClass = parentClass->parentClass;
                                }
                                else
                                {
                                    break;
                                }
                            }
                            if (foundToStr)
                            {
                                std::string strReg = newTempRegister();
                                SIRInstruction callInst;
                                callInst.opcode = SIROpcode::OBJECT_CALL;
                                callInst.result = SIROperand::Register(strReg, SadTypeKind::String);
                                callInst.operands.push_back(argOperands[i]);
                                callInst.operands.push_back(SIROperand::ConstantString("__op_tostring__"));
                                if (currentBlock_)
                                    currentBlock_->addInstruction(callInst);
                                resolvedOps.push_back(SIROperand::Register(strReg, SadTypeKind::String));
                                continue;
                            }
                        }
                        resolvedOps.push_back(argOperands[i]);
                    }

                    SIRInstruction printInst(SIROpcode::BUILTIN_PRINT); // (sir_types.h:221)
                    printInst.operands = resolvedOps;

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(printInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin اطبع()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void); // (AR) لا قيمة إرجاع
                }

                // (AR) دالة اطبع_سطر() - BUILTIN_PRINTLN
                // (EN) println() function - print with newline
                // الأسماء المدعومة: اطبع_سطر, println, طبع_سطر
                if (funcName == "اطبع_سطر" || funcName == "println" || funcName == "طبع_سطر")
                {
                    // ====================================================================
                    // (AR) تحويل تلقائي: إذا كان المعامل كائناً ولديه __op_tostring__، استدعها
                    // (EN) Auto-convert: if arg is object with __op_tostring__, call it first
                    // ====================================================================
                    std::vector<SIROperand> resolvedOps;
                    for (size_t i = 0; i < argResults.size(); ++i)
                    {
                        if (!argResults[i].className.empty())
                        {
                            // (AR) البحث في سلسلة الوراثة عن __op_tostring__
                            // (EN) Search inheritance chain for __op_tostring__
                            std::string searchClass = argResults[i].className;
                            std::string tostrName;
                            bool foundToStr = false;
                            while (!searchClass.empty())
                            {
                                tostrName = searchClass + ".__op_tostring__";
                                if (functionTable_.find(tostrName) != functionTable_.end())
                                {
                                    foundToStr = true;
                                    break;
                                }
                                auto parentClass = module_->getClass(searchClass);
                                if (parentClass && !parentClass->parentClass.empty())
                                {
                                    searchClass = parentClass->parentClass;
                                }
                                else
                                {
                                    break;
                                }
                            }
                            if (foundToStr)
                            {
                                std::string strReg = newTempRegister();
                                SIRInstruction callInst;
                                callInst.opcode = SIROpcode::OBJECT_CALL;
                                callInst.result = SIROperand::Register(strReg, SadTypeKind::String);
                                callInst.operands.push_back(argOperands[i]);
                                callInst.operands.push_back(SIROperand::ConstantString("__op_tostring__"));
                                if (currentBlock_)
                                    currentBlock_->addInstruction(callInst);
                                resolvedOps.push_back(SIROperand::Register(strReg, SadTypeKind::String));
                                continue;
                            }
                        }
                        resolvedOps.push_back(argOperands[i]);
                    }

                    // طباعة المعاملات أولاً
                    SIRInstruction printInst(SIROpcode::BUILTIN_PRINT);
                    printInst.operands = resolvedOps;

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(printInst);
                    }

                    // ثم إضافة سطر جديد
                    SIRInstruction newlineInst(SIROpcode::BUILTIN_PRINT);
                    newlineInst.operands.push_back(SIROperand::ConstantString("\n"));

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(newlineInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin اطبع_سطر()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // (AR) دالة اقرأ() - BUILTIN_READ
                // (EN) input() function
                // الأسماء المدعومة: اقرأ, input
                if (funcName == "اقرأ" || funcName == "input")
                {
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::String);

                    SIRInstruction readInst(SIROpcode::BUILTIN_READ); // (sir_types.h:222)
                    readInst.result = resultOp;
                    readInst.operands = argOperands;

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(readInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin اقرأ() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::String);
                }


                // ================================================================
                // (AR) توزيع لدوال مساعدة مستخرجة — CW-05
                // (EN) Dispatch to extracted helper functions — CW-05
                // ================================================================

                auto mathResult = buildBuiltinMathCall(funcName, argResults, argOperands);
                if (mathResult) return *mathResult;

                auto strArrResult = buildBuiltinStringArrayCall(funcName, argResults, argOperands);
                if (strArrResult) return *strArrResult;

                auto ioResult = buildBuiltinIOUtilsCall(funcName, argResults, argOperands);
                if (ioResult) return *ioResult;

                // (AR) ليست دالة مضمنة — المتصل يتابع البحث
                // (EN) Not a core builtin — caller continues lookup
                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad