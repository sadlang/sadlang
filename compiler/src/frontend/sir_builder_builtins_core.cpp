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
                    // (AR) إصلاح: استخدم ARRAY_LEN فقط إذا كان النوع مصفوفة صريحاً
                    //      حقول الأصناف بدون تعليق نوع تُسجّل كـ Integer افتراضياً
                    //      لكن عند استدعاء طول() عليها فهي غالباً نصوص وليست أعداداً
                    //      لذا نستخدم STRING_LEN لكل ما ليس مصفوفة صريحة
                    // (EN) Fix: Use ARRAY_LEN only when type is explicitly Array
                    //      Class fields without type annotations default to Integer
                    //      but when طول() is called on them, they're likely strings not ints
                    //      so use STRING_LEN for everything that isn't explicitly Array
                    SIROpcode opcode = (argResults[0].type == SadTypeKind::Array || argResults[0].type == SadTypeKind::Tuple)
                                           ? SIROpcode::ARRAY_LEN   // (sir_types.h:176)
                                           : SIROpcode::STRING_LEN; // (sir_types.h:181)

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

                    // (AR) تحديد opcode التحويل حسب نوع المعامل:
                    //      Boolean → BOOL_TO_STRING (يُنتج "صحيح"/"خطأ")
                    //      Float   → F64_TO_STRING
                    //      غير ذلك → I64_TO_STRING
                    //      ملاحظة: دون فحص Boolean، يُعامل bool كـ i64 → يُرجع pointer
                    //      كرقم عند استخدام نتيجة دالة تُرجع boolean
                    // (EN) Determine conversion opcode by argument type:
                    //      Boolean → BOOL_TO_STRING (produces "صحيح"/"خطأ")
                    //      Float   → F64_TO_STRING
                    //      else    → I64_TO_STRING
                    //      Without Boolean check, bool treated as i64 → pointer returned
                    //      as number when function returning bool is the argument
                    SIROpcode opcode;
                    if (argResults[0].type == SadTypeKind::Boolean)
                    {
                        opcode = SIROpcode::BOOL_TO_STRING; // (sir_types.h:211)
                    }
                    else if (argResults[0].type == SadTypeKind::Float)
                    {
                        opcode = SIROpcode::F64_TO_STRING; // (sir_types.h:210)
                    }
                    else
                    {
                        opcode = SIROpcode::I64_TO_STRING; // (sir_types.h:209)
                    }

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
                //      مع فحص استيراد الوحدة المطلوبة وفق سلوك المفسر
                // (EN) Dispatch to extracted helper functions — CW-05
                //      With module import checking per interpreter behavior
                // ================================================================

                // (AR) فحص الوحدة المطلوبة للدالة قبل التوزيع
                //      إذا كانت الدالة تتطلب وحدة لم يتم استيرادها، لا تعاملها كمضمنة
                //      يدعم الوحدات المتعددة مفصولة بـ "|" (أي واحدة كافية)
                // (EN) Check required module before dispatch
                //      If function requires an unimported module, don't treat as builtin
                //      Supports multi-module with "|" separator (any one is sufficient)
                {
                    std::string requiredModule = getRequiredModuleForBuiltin(funcName);
                    if (!requiredModule.empty())
                    {
                        bool moduleFound = false;
                        // (AR) تحقق من وحدات متعددة مفصولة بـ "|"
                        // (EN) Check multiple modules separated by "|"
                        size_t start = 0;
                        size_t pos = requiredModule.find('|');
                        while (true)
                        {
                            std::string mod = (pos == std::string::npos)
                                                  ? requiredModule.substr(start)
                                                  : requiredModule.substr(start, pos - start);
                            if (isStdlibModuleImported(mod))
                            {
                                moduleFound = true;
                                break;
                            }
                            if (pos == std::string::npos)
                                break;
                            start = pos + 1;
                            pos = requiredModule.find('|', start);
                        }

                        if (!moduleFound)
                        {
                            // (AR) الوحدة المطلوبة غير مستوردة — لا تعالج كدالة مضمنة
                            //      ستُعالج كدالة مستخدم ← خطأ "دالة غير معرّفة" كما في المفسر
                            // (EN) Required module not imported — don't handle as builtin
                            //      Will be treated as user function → "undefined function" error like interpreter
                            return std::nullopt;
                        }
                    }
                }

                // ================================================================
                // (AR) دالة مدى() / range() — إنشاء مصفوفة أرقام
                //      تتطلب استيراد وحدة "أساسيات" وفق سلوك المفسر
                // (EN) range() function — create integer array
                //      Requires importing "أساسيات" module per interpreter behavior
                // ================================================================
                if (funcName == "مدى" || funcName == "range")
                {
                    // (AR) فحص استيراد وحدة أساسيات — توحيد مع المفسر
                    // (EN) Check أساسيات module import — unify with interpreter
                    if (!isStdlibModuleImported("أساسيات"))
                    {
                        // (AR) لا تعامل كدالة مضمنة — ستُعالج كدالة مستخدم → خطأ واضح
                        // (EN) Don't treat as builtin — will be handled as user function → clear error
                        return std::nullopt;
                    }
                    if (argResults.empty() || argResults.size() > 3)
                    {
                        errors_.push_back("Error: مدى() requires 1-3 arguments (stop) or (start, stop) or (start, stop, step)");
                        return BuildResult();
                    }

                    // (AR) تحديد المعاملات: مدى(stop), مدى(start, stop), مدى(start, stop, step)
                    // (EN) Determine args: range(stop), range(start, stop), range(start, stop, step)
                    SIROperand startOp, endOp, stepOp;
                    if (argResults.size() == 1)
                    {
                        // مدى(stop) → __sad_range(0, stop, 1)
                        startOp = SIROperand::ConstantI64(0);
                        endOp = argOperands[0];
                        stepOp = SIROperand::ConstantI64(1);
                    }
                    else if (argResults.size() == 2)
                    {
                        // مدى(start, stop) → __sad_range(start, stop, 1)
                        startOp = argOperands[0];
                        endOp = argOperands[1];
                        stepOp = SIROperand::ConstantI64(1);
                    }
                    else
                    {
                        // مدى(start, stop, step) → __sad_range(start, stop, step)
                        startOp = argOperands[0];
                        endOp = argOperands[1];
                        stepOp = argOperands[2];
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Array);

                    SIRInstruction callInst(SIROpcode::CALL);
                    callInst.result = resultOp;
                    callInst.operands.push_back(SIROperand::Function("__sad_range"));
                    callInst.operands.push_back(startOp);
                    callInst.operands.push_back(endOp);
                    callInst.operands.push_back(stepOp);

                    if (currentBlock_)
                        currentBlock_->instructions.push_back(callInst);

                    BuildResult result(resultReg, SadTypeKind::Array);
                    result.elementType = SadTypeKind::Integer;
                    return result;
                }

                auto mathResult = buildBuiltinMathCall(funcName, argResults, argOperands);
                if (mathResult)
                    return *mathResult;

                auto strArrResult = buildBuiltinStringArrayCall(funcName, argResults, argOperands);
                if (strArrResult)
                    return *strArrResult;

                auto ioResult = buildBuiltinIOUtilsCall(funcName, argResults, argOperands);
                if (ioResult)
                    return *ioResult;

                // ================================================================
                // (AR) خريطة() — إنشاء خريطة فارغة عبر __sad_map_create
                //      معالجة خريطة() كدالة مضمنة (type constructor) بدلاً من external call
                //      يضمن أن المتغير المُسنَد يحصل على نوع Map الصحيح
                //      مما يُتيح لـ .عيّن() و.احصل() وغيرها العمل عبر buildMapBuiltinMethodCall
                // (EN) خريطة() — create empty map via __sad_map_create
                //      Handle خريطة() as a builtin (type constructor) instead of external call
                //      Ensures the assigned variable gets the correct Map type
                //      allowing .عيّن()/.احصل() and others to work via buildMapBuiltinMethodCall
                // ================================================================
                if (funcName == "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9" || funcName == "map")
                {
                    // (AR) hint = عدد الوسائط إذا مُررت (اختياري)
                    // (EN) hint = number of arguments if provided (optional)
                    int64_t hint = static_cast<int64_t>(argOperands.size());
                    std::string resultReg = newTempRegister();
                    SIRInstruction createInst;
                    createInst.opcode = SIROpcode::CALL;
                    createInst.result = SIROperand::Register(resultReg, SadTypeKind::Map);
                    createInst.operands.push_back(SIROperand::ConstantString("__sad_map_create"));
                    createInst.operands.push_back(SIROperand::ConstantI64(hint));
                    createInst.comment = "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9() \xe2\x86\x92 __sad_map_create";
                    if (currentBlock_)
                        currentBlock_->addInstruction(createInst);
                    return BuildResult(resultReg, SadTypeKind::Map);
                }

                // ================================================================
                // (AR) مصفوفة() — إنشاء مصفوفة فارغة عبر SIROpcode::ARRAY_NEW
                //      معالجة مصفوفة() كدالة مضمنة (type constructor) بدلاً من external call
                //      يضمن أن المتغير المُسنَد يحصل على نوع Array الصحيح
                //      مما يُتيح لـ .أضف() و.طول() وغيرها العمل عبر buildArrayBasicMethodCall
                //      emitArrayNew تُولِّد SadArray مباشرة بدون استدعاء دالة خارجية
                // (EN) مصفوفة() — create empty array via SIROpcode::ARRAY_NEW
                //      Handle مصفوفة() as a builtin (type constructor) instead of external call
                //      Ensures the assigned variable gets the correct Array type
                //      allowing .أضف()/.طول() and others to work via buildArrayBasicMethodCall
                //      emitArrayNew generates SadArray inline without external function call
                // ================================================================
                if (funcName == "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9" || funcName == "array")
                {
                    // (AR) السعة الابتدائية — اختياري (افتراضي 8)
                    // (EN) Initial capacity — optional (default 8)
                    int64_t capacity = 8;
                    if (!argOperands.empty() && argOperands[0].type == SIROperandType::CONSTANT)
                        capacity = argOperands[0].intValue > 0 ? argOperands[0].intValue : 8;
                    std::string resultReg = newTempRegister();
                    SIRInstruction arrInst;
                    arrInst.opcode = SIROpcode::ARRAY_NEW;
                    arrInst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    arrInst.operands.push_back(SIROperand::ConstantI64(capacity));
                    arrInst.comment = "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9() \xe2\x86\x92 ARRAY_NEW";
                    if (currentBlock_)
                        currentBlock_->addInstruction(arrInst);
                    // (AR) تسجيل المتغير بنوع Array لدعم استدعاء .أضف()/.طول() لاحقاً
                    // (EN) Register variable as Array type so method dispatch works
                    return BuildResult(resultReg, SadTypeKind::Array);
                }

                // (AR) ليست دالة مضمنة — المتصل يتابع البحث
                // (EN) Not a core builtin — caller continues lookup
                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad