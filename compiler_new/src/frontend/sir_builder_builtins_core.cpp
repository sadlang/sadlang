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

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// sir_builder_builtins_core.cpp
// (AR) ״§„״¯ˆ״§„ ״§„…״¯…״¬״© ״§„״£״³״§״³״©: ״×״­ˆ„ ״§„״£†ˆ״§״¹״ ״§„״·״¨״§״¹״©״ ״§„״±״§״¶״§״×״ ״§„†״µˆ״µ״ ״§„…״µˆ״§״×״ ״§„…„״§״×
// (EN) Core builtins: type conversion, print, math, string, array, file operations
// ============================================================================

std::optional<BuildResult> SIRBuilder::buildBuiltinCallCore(
    const std::string& funcName,
    bool isUserDefinedFunction,
    std::vector<BuildResult>& argResults,
    std::vector<SIROperand>& argOperands) {
    // (AR) إذا كانت الدالة معرّفة من المستخدم، لا تعامل كدالة مضمنة
    // (EN) If function is user-defined, skip all builtins
    if (isUserDefinedFunction) return std::nullopt;
    
    if (funcName == "طول" || funcName == "length") {
        if (argResults.size() != 1) {
            errors_.push_back("Error: طول() requires exactly 1 argument");
            return BuildResult();
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        // (AR) تحديد نوع العملية بناءً على نوع المعامل
        // (EN) Determine operation based on argument type
        SIROpcode opcode = (argResults[0].type == SIRType::STRING) 
            ? SIROpcode::STRING_LEN    // (sir_types.h:181)
            : SIROpcode::ARRAY_LEN;    // (sir_types.h:176)
        
        SIRInstruction lenInst(opcode);
        lenInst.result = resultOp;
        lenInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(lenInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin طول() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة لرقم() - STRING_TO_I64
    // (EN) to_int() function - STRING_TO_I64
    // الأسماء المدعومة: لرقم, حول_رقم, to_int, int, إلى_رقم
    if (funcName == "لرقم" || funcName == "حول_رقم" || funcName == "to_int" || funcName == "int" || funcName == "إلى_رقم") {
        if (argResults.size() != 1) {
            errors_.push_back("Error: لرقم() requires exactly 1 argument");
            return BuildResult();
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        SIRInstruction convInst(SIROpcode::STRING_TO_I64);  // (sir_types.h:187)
        convInst.result = resultOp;
        convInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(convInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin لرقم() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة لعشري() - STRING_TO_F64
    // (EN) to_float() function
    // الأسماء المدعومة: لعشري, to_float, float
    if (funcName == "لعشري" || funcName == "to_float" || funcName == "float") {
        if (argResults.size() != 1) {
            errors_.push_back("Error: لعشري() requires exactly 1 argument");
            return BuildResult();
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction convInst(SIROpcode::STRING_TO_F64);  // (AR) تحويل نص إلى عشري (EN) Convert string to float
        convInst.result = resultOp;
        convInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(convInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin لعشري() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة لنص() - I64_TO_STRING أو F64_TO_STRING
    // (EN) to_string() function
    // الأسماء المدعومة: لنص, نص, to_string, str, string, إلى_نص
    if (funcName == "لنص" || funcName == "نص" || funcName == "to_string" || 
        funcName == "str" || funcName == "string" || funcName == "إلى_نص") {
        if (argResults.size() != 1) {
            errors_.push_back("Error: لنص() requires exactly 1 argument");
            return BuildResult();
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        
        SIROpcode opcode = (argResults[0].type == SIRType::F64) 
            ? SIROpcode::F64_TO_STRING  // (sir_types.h:210)
            : SIROpcode::I64_TO_STRING; // (sir_types.h:209)
        
        SIRInstruction convInst(opcode);
        convInst.result = resultOp;
        convInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(convInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin لنص() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // (AR) دالة اطبع() - BUILTIN_PRINT
    // (EN) print() function
    // الأسماء المدعومة: اطبع, print, إطبع
    if (funcName == "اطبع" || funcName == "print" || funcName == "إطبع") {
        SIRInstruction printInst(SIROpcode::BUILTIN_PRINT);  // (sir_types.h:221)
        printInst.operands = argOperands;
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(printInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin اطبع()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);  // (AR) لا قيمة إرجاع
    }
    
    // (AR) دالة اطبع_سطر() - BUILTIN_PRINTLN
    // (EN) println() function - print with newline
    // الأسماء المدعومة: اطبع_سطر, println, طبع_سطر
    if (funcName == "اطبع_سطر" || funcName == "println" || funcName == "طبع_سطر") {
        // طباعة المعاملات أولاً
        SIRInstruction printInst(SIROpcode::BUILTIN_PRINT);
        printInst.operands = argOperands;
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(printInst);
        }
        
        // ثم إضافة سطر جديد
        SIRInstruction newlineInst(SIROpcode::BUILTIN_PRINT);
        newlineInst.operands.push_back(SIROperand::ConstantString("\n"));
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(newlineInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin اطبع_سطر()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // (AR) دالة اقرأ() - BUILTIN_READ
    // (EN) input() function
    // الأسماء المدعومة: اقرأ, input
    if (funcName == "اقرأ" || funcName == "input") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        
        SIRInstruction readInst(SIROpcode::BUILTIN_READ);  // (sir_types.h:222)
        readInst.result = resultOp;
        readInst.operands = argOperands;
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(readInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin اقرأ() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // ========================================================================
    // (AR) دوال رياضية - Math Functions
    // (EN) Math functions
    // ========================================================================
    
    // (AR) دالة جذر() - sqrt
    // (EN) sqrt() function - Square root
    // الأسماء المدعومة: جذر, sqrt, الجذر_التربيعي
    if (funcName == "جذر" || funcName == "sqrt" || funcName == "الجذر_التربيعي") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة جذر تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction sqrtInst(SIROpcode::BUILTIN_SQRT);
        sqrtInst.result = resultOp;
        sqrtInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(sqrtInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin جذر() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة أس() - power
    // (EN) power() function - Base^Exponent
    // الأسماء المدعومة: أس, power, pow
    if (funcName == "أس" || funcName == "power" || funcName == "pow") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة أس تتطلب معاملين (الأساس والأس)" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction powInst(SIROpcode::BUILTIN_POW);
        powInst.result = resultOp;
        powInst.operands.push_back(argOperands[0]); // base
        powInst.operands.push_back(argOperands[1]); // exponent
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(powInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin أس() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة مطلق() - abs
    // (EN) abs() function - Absolute value
    // الأسماء المدعومة: مطلق, abs, القيمة_المطلقة, absolute
    if (funcName == "مطلق" || funcName == "abs" || funcName == "القيمة_المطلقة" || funcName == "absolute") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة مطلق تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIRType resultType = argResults[0].type; // preserve type (I64 or F64)
        SIROperand resultOp = SIROperand::Register(resultReg, resultType);
        
        SIRInstruction absInst(SIROpcode::BUILTIN_ABS);
        absInst.result = resultOp;
        absInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(absInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin مطلق() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, resultType);
    }
    
    // (AR) دالة تقريب() - round
    // (EN) round() function
    // الأسماء المدعومة: تقريب, round
    if (funcName == "تقريب" || funcName == "round") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة تقريب تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        SIRInstruction roundInst(SIROpcode::BUILTIN_ROUND);
        roundInst.result = resultOp;
        roundInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(roundInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin تقريب() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة أرضية() - floor
    // (EN) floor() function
    // الأسماء المدعومة: أرضية, floor
    if (funcName == "أرضية" || funcName == "floor") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة أرضية تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        SIRInstruction floorInst(SIROpcode::BUILTIN_FLOOR);
        floorInst.result = resultOp;
        floorInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(floorInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin أرضية() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة سقف() - ceil
    // (EN) ceil() function
    // الأسماء المدعومة: سقف, ceil, ceiling
    if (funcName == "سقف" || funcName == "ceil" || funcName == "ceiling") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة سقف تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        
        SIRInstruction ceilInst(SIROpcode::BUILTIN_CEIL);
        ceilInst.result = resultOp;
        ceilInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(ceilInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin سقف() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) دالة مربع() - square
    // (EN) square() function - x^2
    // الأسماء المدعومة: مربع, square
    if (funcName == "مربع" || funcName == "square") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة مربع تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction sqInst(SIROpcode::BUILTIN_POW);
        sqInst.result = resultOp;
        sqInst.operands.push_back(argOperands[0]);
        sqInst.operands.push_back(SIROperand::ConstantF64(2.0));
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(sqInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin مربع() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة جيب() - sin
    // (EN) sin() function - sine
    // الأسماء المدعومة: جيب, sin, sine
    if (funcName == "جيب" || funcName == "sin" || funcName == "sine") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة جيب تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction sinInst(SIROpcode::BUILTIN_SIN);
        sinInst.result = resultOp;
        sinInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(sinInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin جيب() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة جيب_تمام() - cos
    // (EN) cos() function - cosine
    // الأسماء المدعومة: جيب_تمام, cos, cosine
    if (funcName == "جيب_تمام" || funcName == "cos" || funcName == "cosine") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة جيب_تمام تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction cosInst(SIROpcode::BUILTIN_COS);
        cosInst.result = resultOp;
        cosInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(cosInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin جيب_تمام() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // (AR) دالة ظل() - tan
    // (EN) tan() function - tangent
    // الأسماء المدعومة: ظل, tan, tangent
    if (funcName == "ظل" || funcName == "tan" || funcName == "tangent") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة ظل تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        
        SIRInstruction tanInst(SIROpcode::BUILTIN_TAN);
        tanInst.result = resultOp;
        tanInst.operands.push_back(argOperands[0]);
        
        if (currentBlock_) {
            currentBlock_->instructions.push_back(tanInst);
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: builtin ظل() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // ========================================================================
    // String Functions (12 functions)
    // ========================================================================
    
    // 1. طول_نص / string_length
    if (funcName == "طول_نص" || funcName == "string_length" || funcName == "str_length" || 
        funcName == "نص_طول" || funcName == "نص_الطول") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة طول_نص تتطلب معامل واحد (نص)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_LENGTH);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 1b. رمز_حرف / char_at - get character at index
    if (funcName == "رمز_حرف" || funcName == "char_at" || funcName == "charAt") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة رمز_حرف تتطلب معاملين (نص, فهرس)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_CHAR_AT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);  // string
        inst.operands.push_back(argOperands[1]);  // index
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 2. تحويل_كبير / toUpper
    if (funcName == "تحويل_كبير" || funcName == "toUpper" || funcName == "uppercase" || funcName == "لأكبر") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة تحويل_كبير تتطلب معامل واحد (نص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_TO_UPPER);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 3. تحويل_صغير / toLower
    if (funcName == "تحويل_صغير" || funcName == "toLower" || funcName == "lowercase" || funcName == "لأصغر") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة تحويل_صغير تتطلب معامل واحد (نص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_TO_LOWER);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 4. بحث / find
    if (funcName == "بحث" || funcName == "find" || funcName == "indexOf" || funcName == "ابحث") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة بحث تتطلب معاملين (نص, نص للبحث عنه)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_FIND);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 5. استبدل / replace
    if (funcName == "استبدل" || funcName == "replace" || funcName == "بدل") {
        if (argResults.size() < 3) {
            std::cerr << "[Error] دالة استبدل تتطلب 3 معاملات (نص, قديم, جديد)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_REPLACE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 6. استخراج / substring
    if (funcName == "استخراج" || funcName == "substring" || funcName == "substr" || funcName == "slice") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة استخراج تتطلب على الأقل معاملين (نص, بداية)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_SUBSTRING);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]); // string
        inst.operands.push_back(argOperands[1]); // start
        if (argResults.size() >= 3) {
            inst.operands.push_back(argOperands[2]); // end (optional)
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 7. قص_أطراف / trim
    if (funcName == "قص_أطراف" || funcName == "trim" || funcName == "strip") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة قص_أطراف تتطلب معامل واحد (نص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_TRIM);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 8. تقسيم / split (returns array)
    if (funcName == "تقسيم" || funcName == "split") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة تقسيم تتطلب معاملين (نص, فاصل)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_SPLIT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::ARRAY);
    }
    
    // 9. دمج / join (array to string)
    if (funcName == "دمج" || funcName == "join") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة دمج تتطلب معاملين (مصفوفة, فاصل)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_JOIN);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 10. يبدأ_ب / startsWith
    if (funcName == "يبدأ_ب" || funcName == "startsWith" || funcName == "starts_with") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة يبدأ_ب تتطلب معاملين (نص, بادئة)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_STARTS_WITH);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 11. ينتهي_ب / endsWith
    if (funcName == "ينتهي_ب" || funcName == "endsWith" || funcName == "ends_with") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة ينتهي_ب تتطلب معاملين (نص, لاحقة)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_ENDS_WITH);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 12. يحتوي_على / contains
    if (funcName == "يحتوي_على" || funcName == "contains" || funcName == "includes") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة يحتوي_على تتطلب معاملين (نص, نص للبحث عنه)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_STRING_CONTAINS);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // ========================================================================
    // Array Functions (10 functions)
    // ========================================================================
    
    // 1. إضافة_عنصر / append / أضف
    if (funcName == "إضافة_عنصر" || funcName == "append" || funcName == "push" || funcName == "add" || funcName == "أضف") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة إضافة_عنصر تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_APPEND);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 2. إزالة_عنصر / remove / أزل
    if (funcName == "إزالة_عنصر" || funcName == "remove" || funcName == "delete" || funcName == "pop" || funcName == "أزل") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة إزالة_عنصر تتطلب معاملين (مصفوفة, فهرس)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REMOVE);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 3. حجم_مصفوفة / array_size / length
    if (funcName == "حجم_مصفوفة" || funcName == "array_size" || funcName == "حجم") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة حجم_مصفوفة تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SIZE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 4. فهرس / indexOf (array)
    if (funcName == "فهرس_مصفوفة" || funcName == "array_indexOf" || funcName == "فهرس_عنصر") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة فهرس تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_INDEX_OF);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 5. يحتوي_عنصر / contains (array)
    if (funcName == "يحتوي_عنصر" || funcName == "array_contains" || funcName == "has") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة يحتوي_عنصر تتطلب معاملين (مصفوفة, عنصر)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_CONTAINS);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 6. قلب / reverse
    if (funcName == "قلب" || funcName == "reverse" || funcName == "اعكس") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة قلب تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REVERSE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::ARRAY);
    }
    
    // 7. فرز / sort
    if (funcName == "فرز" || funcName == "sort" || funcName == "رتب") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة فرز تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SORT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::ARRAY);
    }
    
    // 8. أول / first
    if (funcName == "أول" || funcName == "first" || funcName == "العنصر_الأول") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة أول تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::VOID);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_FIRST);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::VOID);
    }
    
    // 9. آخر / last
    if (funcName == "آخر" || funcName == "last" || funcName == "العنصر_الأخير") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة آخر تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::VOID);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_LAST);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::VOID);
    }
    
    // 10. شريحة / slice
    if (funcName == "شريحة" || funcName == "array_slice" || funcName == "قطع") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة شريحة تتطلب على الأقل معاملين (مصفوفة, بداية)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_SLICE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (argResults.size() >= 3) {
            inst.operands.push_back(argOperands[2]); // end (optional)
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::ARRAY);
    }

    // ========================================================================
    // File I/O Functions (8 functions)
    // ========================================================================
    
    // 1. اقرأ_ملف / read_file
    if (funcName == "اقرأ_ملف" || funcName == "read_file" || funcName == "readFile") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة اقرأ_ملف تتطلب معامل واحد (مسار الملف)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_READ);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 2. اكتب_ملف / write_file
    if (funcName == "اكتب_ملف" || funcName == "write_file" || funcName == "writeFile") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة اكتب_ملف تتطلب معاملين (مسار, محتوى)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_WRITE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 3. أضف_إلى_ملف / append_to_file
    if (funcName == "أضف_إلى_ملف" || funcName == "append_to_file" || funcName == "appendFile") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة أضف_إلى_ملف تتطلب معاملين (مسار, محتوى)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_APPEND);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 4. احذف_ملف / delete_file
    if (funcName == "احذف_ملف" || funcName == "delete_file" || funcName == "deleteFile") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة احذف_ملف تتطلب معامل واحد (مسار)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_DELETE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 5. انسخ_ملف / copy_file
    if (funcName == "انسخ_ملف" || funcName == "copy_file" || funcName == "copyFile") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة انسخ_ملف تتطلب معاملين (مصدر, وجهة)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_COPY);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 6. انقل_ملف / move_file
    if (funcName == "انقل_ملف" || funcName == "move_file" || funcName == "moveFile") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة انقل_ملف تتطلب معاملين (مصدر, وجهة)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_MOVE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 7. أنشئ_مجلد / create_dir
    if (funcName == "أنشئ_مجلد" || funcName == "create_dir" || funcName == "createDir" || funcName == "mkdir") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة أنشئ_مجلد تتطلب معامل واحد (مسار)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_CREATE_DIR);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 8. اسرد_مجلد / list_dir
    if (funcName == "اسرد_مجلد" || funcName == "list_dir" || funcName == "listDir" || funcName == "ls") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة اسرد_مجلد تتطلب معامل واحد (مسار)" << std::endl;
            return BuildResult("", SIRType::ARRAY);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::ARRAY);
        SIRInstruction inst(SIROpcode::BUILTIN_FILE_LIST_DIR);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::ARRAY);
    }
    
    // ========================================================================
    // Utility Functions (4 functions)
    // ========================================================================
    
    // 1. عشوائي / random
    if (funcName == "عشوائي" || funcName == "random" || funcName == "rand") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        SIRInstruction inst(SIROpcode::BUILTIN_RANDOM);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // 2. نم / sleep
    if (funcName == "نم" || funcName == "sleep" || funcName == "wait") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة نم تتطلب معامل واحد (مدة بالثواني)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SLEEP);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 3. اخرج / exit
    if (funcName == "اخرج" || funcName == "exit" || funcName == "quit") {
        SIRInstruction inst(SIROpcode::BUILTIN_EXIT);
        if (!argResults.empty()) {
            inst.operands.push_back(argOperands[0]); // exit code (optional)
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 4. النوع / type_of
    if (funcName == "النوع" || funcName == "type_of" || funcName == "typeof" || funcName == "نوع") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة النوع تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_TYPE_OF);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }

    // ========================================================================
    // Math Functions - أكبر/أصغر/جمع (3 functions)
    // ========================================================================
    
    // أكبر / max - الأكبر من قيمتين
    if (funcName == "أكبر" || funcName == "max" || funcName == "maximum") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة أكبر تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_MAX);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // أصغر / min - الأصغر من قيمتين
    if (funcName == "أصغر" || funcName == "min" || funcName == "minimum") {
        if (argResults.size() < 2) {
            std::cerr << "[Error] دالة أصغر تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_MIN);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // جمع / sum - مجموع عناصر مصفوفة
    if (funcName == "جمع" || funcName == "sum") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة جمع تتطلب معامل واحد (مصفوفة)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_SUM);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }

    // ========================================================================
    // Type Checking Functions (4 functions)
    // ========================================================================
    
    // هو_رقم / هو_رقم_صحيح / is_int
    if (funcName == "هو_رقم" || funcName == "هو_رقم_صحيح" || funcName == "is_int" || funcName == "isInt") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة هو_رقم تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_IS_INTEGER);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // هو_عشري / هو_رقم_عشري / is_float
    if (funcName == "هو_عشري" || funcName == "هو_رقم_عشري" || funcName == "is_float" || funcName == "isFloat") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة هو_عشري تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_IS_FLOAT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // هو_نص / is_string / isString
    if (funcName == "هو_نص" || funcName == "is_string" || funcName == "isString") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة هو_نص تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_IS_STRING);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // هو_مصفوفة / is_array / isArray
    if (funcName == "هو_مصفوفة" || funcName == "is_array" || funcName == "isArray") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة هو_مصفوفة تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_IS_ARRAY);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }

    // ========================================================================
    // Additional Conversion Functions
    // ========================================================================
    
    // لمنطقي / to_bool
    if (funcName == "لمنطقي" || funcName == "to_bool" || funcName == "bool") {
        if (argResults.empty()) {
            std::cerr << "[Error] دالة لمنطقي تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_TO_BOOL);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }

    // ========================================================================
    // Additional IO Functions
    // ========================================================================
    
    // قراءة_سطر / readLine
    if (funcName == "قراءة_سطر" || funcName == "readLine" || funcName == "read_line") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_READ_LINE);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // مسح_الشاشة / clear / cls
    if (funcName == "مسح_الشاشة" || funcName == "clear" || funcName == "cls") {
        SIRInstruction inst(SIROpcode::BUILTIN_CLEAR_SCREEN);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ========================================================================
    // (AR) دوال برمجة أنظمة التشغيل — OS Development Builtin Functions
    // (EN) OS Development Builtin Functions
    // ========================================================================
    // (AR) هذا القسم يضيف الدوال المضمنة اللازمة لبرمجة أنظمة التشغيل:
    //      - منفذ_اكتب/منفذ_اقرأ: التعامل مع منافذ الإدخال/الإخراج (I/O ports)
    //      - ذاكرة_اكتب/ذاكرة_اقرأ: الوصول المباشر للذاكرة (raw memory access)
    //      - مقاطعة: إطلاق مقاطعة برمجية (software interrupt)
    //      - توقف: إيقاف المعالج (halt CPU)
    //      - تعطيل_مقاطعات/تفعيل_مقاطعات: التحكم بالمقاطعات (CLI/STI)
    //      - شاشة_اكتب/شاشة_امسح: التعامل مع ذاكرة VGA
    //      - انسخ_ذاكرة/املأ_ذاكرة: عمليات الذاكرة الجماعية
    // ========================================================================
    
    // ──────────────────────────────────────────────
    // (AR) منفذ_اكتب(منفذ، قيمة) — كتابة بايت على منفذ I/O
    // (EN) port_write(port, value) — write byte to I/O port (outb)
    // (AR) يستخدم في: التحكم بالعتاد مثل VGA، لوحة المفاتيح، القرص
    // ──────────────────────────────────────────────
    // Not a core builtin
    return std::nullopt;
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
