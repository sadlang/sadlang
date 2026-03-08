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

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// buildLiteral - بناء ثابت حرفي
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:492
// التوقيع / Signature: BuildResult buildLiteral(AST::LiteralNode* literal);
//
// المعاملات / Parameters:
// - literal: AST::LiteralNode* = Sad::AST::LiteralExpr* (sir_builder.h:71)
//
// LiteralExpr Members (expressions.h:174):
// - token: Lexer::Token (line 175)
//
// Token Members (lexer/token.h):
// - type: TokenType
// - value: std::string
// - getType(): TokenType
// - getValue(): std::string
//
// TokenType Values (lexer/token.h):
// - INTEGER, FLOAT, STRING, TRUE, FALSE, NONE
//
// الإرجاع / Returns:
// - BuildResult with constant value
// ============================================================================
BuildResult SIRBuilder::buildLiteral(AST::LiteralNode* literal) {
    if (!literal) {
        return BuildResult();
    }
    
    // (AR) الحصول على Token (expressions.h:175 - token member)
    // (EN) Get token
    const auto& token = literal->token;
    Lexer::TokenType tokenType = token.getType();
    std::string value = token.getValue();
    
    // (AR) إنشاء سجل مؤقت للنتيجة
    // (EN) Create temporary register for result
    std::string resultReg = newTempRegister();
    
    // (AR) تحديد النوع بناءً على نوع Token
    // (EN) Determine type based on token type
    SIRType sirType = SIRType::I64; // default
    
    // (AR) معالجة الأنواع المختلفة
    // (EN) Handle different types
    // أسماء TokenType من token.h:198-202
    // TokenType names from token.h:198-202
    BuildResult result;
    result.registerName = resultReg;
    result.isConstant = true;
    result.constantValue = value;
    
    if (tokenType == Lexer::TokenType::NUMBER_INTEGER) {
        result.type = SIRType::I64;
        // (AR) تحويل الأعداد الست عشرية/الثمانية/الثنائية إلى عشرية
        //      لأن std::stoll() الافتراضي يستخدم أساس 10 فقط
        // (EN) Normalize hex/octal/binary literals to decimal strings
        //      because downstream std::stoll() uses base 10 by default
        if (value.size() > 2 && value[0] == '0') {
            char prefix = value[1];
            if (prefix == 'x' || prefix == 'X') {
                // Hex: 0x3F8 → "1016", 0xFFFF800000000000 → "-140737488355328"
                result.constantValue = std::to_string(static_cast<int64_t>(std::stoull(value, nullptr, 16)));
            } else if (prefix == 'o' || prefix == 'O') {
                // Octal: 0o755 → "493"
                result.constantValue = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 8)));
            } else if (prefix == 'b' || prefix == 'B') {
                // Binary: 0b1010 → "10"
                result.constantValue = std::to_string(static_cast<int64_t>(std::stoull(value.substr(2), nullptr, 2)));
            }
        }
    }
    else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE) {
        result.type = SIRType::F64;
    }
    else if (tokenType == Lexer::TokenType::STRING_LITERAL) {
        result.type = SIRType::STRING;
    }
    else if (tokenType == Lexer::TokenType::LITERAL_TRUE || tokenType == Lexer::TokenType::LITERAL_FALSE) {
        result.type = SIRType::BOOL;
        result.constantValue = (tokenType == Lexer::TokenType::LITERAL_TRUE) ? "true" : "false";
    }
    else if (tokenType == Lexer::TokenType::LITERAL_NULL) {
        // (AR) لاشيء / null — نوع PTR بقيمة 0 (مؤشر فارغ)
        // (EN) null literal — PTR type with value 0 (null pointer)
        result.type = SIRType::PTR;
        result.constantValue = "0";
        result.isConstant = true;
    }
    else {
        result.type = SIRType::I64;
    }
    
    return result;
}

// ============================================================================
// buildVariableAccess - بناء وصول لمتغير
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:480
// التوقيع / Signature: BuildResult buildVariableAccess(AST::VariableNode* var);
//
// المعاملات / Parameters:
// - var: AST::VariableNode* = Sad::AST::VariableExpr* (sir_builder.h:70)
//
// VariableExpr Members (expressions.h:206):
// - name: std::string (line 208)
//
// الدوال المستدعاة / Called functions:
// - lookupVariable: sir_builder.h:597 (returns VariableInfo*)
//
// الإرجاع / Returns:
// - BuildResult with variable register and type
// ============================================================================
BuildResult SIRBuilder::buildVariableAccess(AST::VariableNode* var) {
    if (!var) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildVariableAccess: var is null!" << std::endl;
        #endif
        return BuildResult();
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildVariableAccess: looking up variable '" << var->name << "'" << std::endl;
    #endif
    
    // (AR) البحث عن المتغير في النطاقات (expressions.h:208 - name member)
    // (EN) Lookup variable in scopes
    VariableInfo* varInfo = lookupVariable(var->name);
    
    if (!varInfo) {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildVariableAccess: variable NOT FOUND!" << std::endl;
        #endif
        errors_.push_back("Error: Undefined variable '" + var->name + "'");
        return BuildResult();
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildVariableAccess: found variable, registerName='" 
              << varInfo->registerName << "', type=" << static_cast<int>(varInfo->type) << std::endl;
    #endif
    
    // (AR) إرجاع معلومات المتغير (sir_builder.h:139 - VariableInfo struct)
    // (EN) Return variable information
    // VariableInfo members: registerName (line 143), type (line 142)
    return BuildResult(varInfo->registerName, varInfo->type);
}

// ============================================================================
// buildBinaryOp - بناء عملية ثنائية
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:448
// التوقيع / Signature: BuildResult buildBinaryOp(AST::BinaryOpNode* binOp);
//
// المعاملات / Parameters:
// - binOp: AST::BinaryOpNode* = Sad::AST::BinaryExpr* (sir_builder.h:66)
//
// BinaryExpr Members (expressions.h:40-44):
// - left: ExprPtr (line 42)
// - op: Lexer::TokenType (line 43)
// - right: ExprPtr (line 44)
//
// TokenType للعمليات (token.h:205-229):
// - OP_PLUS (205), OP_MINUS (206), OP_MULTIPLY (207), OP_DIVIDE (208)
// - OP_MODULO (209), OP_POWER (210)
// - OP_EQUAL (219), OP_NOT_EQUAL (220), OP_LESS (221), OP_LESS_EQUAL (222)
// - OP_GREATER (223), OP_GREATER_EQUAL (224)
// - OP_AND (227), OP_OR (228)
//
// SIROpcode (sir_types.h:106-130):
// - ADD_I64/ADD_F64, SUB_I64/SUB_F64, MUL_I64/MUL_F64, DIV_I64/DIV_F64
// - MOD_I64, EQ, NE, LT, LE, GT, GE, AND, OR
//
// SIRInstruction::Binary (sir_instruction.h:100-107):
// - الاستخدام: SIRInstruction::Binary(opcode, result, left, right)
//
// SIROperand::Register (sir_types.h:306-312):
// - الاستخدام: SIROperand::Register(name, type)
//
// الإرجاع / Returns:
// - BuildResult (sir_builder.h:103-132): registerName, type
// ============================================================================
BuildResult SIRBuilder::buildBinaryOp(AST::BinaryOpNode* binOp) {
    if (!binOp) {
        return BuildResult();
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildBinaryOp: بدء بناء عملية ثنائية" << std::endl;
    #endif
    
    // (AR) بناء المعامل الأيسر (expressions.h:42 - left: ExprPtr)
    // (EN) Build left operand
    auto leftResult = buildExpression(binOp->left.get());
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildBinaryOp: leftResult.registerName='" << leftResult.registerName 
              << "', type=" << static_cast<int>(leftResult.type) 
              << ", isConstant=" << leftResult.isConstant << std::endl;
    #endif
    
    // (AR) بناء المعامل الأيمن (expressions.h:44 - right: ExprPtr)
    // (EN) Build right operand
    auto rightResult = buildExpression(binOp->right.get());
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildBinaryOp: rightResult.registerName='" << rightResult.registerName 
              << "', type=" << static_cast<int>(rightResult.type) 
              << ", isConstant=" << rightResult.isConstant << std::endl;
    #endif
    
    // ================================================================
    // (AR) تحميل المعاملات الزائد: إذا كان المعامل الأيسر كائن، استدعاء دالة العامل
    // (EN) Operator overloading: if left operand is an object, call operator function
    // ================================================================
    std::string leftClassName = leftResult.className;
    if (leftClassName.empty() && !leftResult.registerName.empty()) {
        auto it = classInstanceTypes_.find(leftResult.registerName);
        if (it != classInstanceTypes_.end()) {
            leftClassName = it->second;
        }
    }
    
    if (!leftClassName.empty()) {
        // (AR) تحويل رمز العامل إلى اسم دالة
        // (EN) Convert operator symbol to function name
        std::string opSafeName;
        switch (binOp->op) {
            case Lexer::TokenType::OP_PLUS:          opSafeName = "__op_add__"; break;
            case Lexer::TokenType::OP_MINUS:         opSafeName = "__op_sub__"; break;
            case Lexer::TokenType::OP_MULTIPLY:      opSafeName = "__op_mul__"; break;
            case Lexer::TokenType::OP_DIVIDE:        opSafeName = "__op_div__"; break;
            case Lexer::TokenType::OP_MODULO:        opSafeName = "__op_mod__"; break;
            case Lexer::TokenType::OP_POWER:         opSafeName = "__op_pow__"; break;
            case Lexer::TokenType::OP_EQUAL:         opSafeName = "__op_eq__"; break;
            case Lexer::TokenType::OP_NOT_EQUAL:     opSafeName = "__op_ne__"; break;
            case Lexer::TokenType::OP_LESS:          opSafeName = "__op_lt__"; break;
            case Lexer::TokenType::OP_LESS_EQUAL:    opSafeName = "__op_le__"; break;
            case Lexer::TokenType::OP_GREATER:       opSafeName = "__op_gt__"; break;
            case Lexer::TokenType::OP_GREATER_EQUAL: opSafeName = "__op_ge__"; break;
            default:
                // (AR) عامل غير مدعوم للتحميل الزائد — لن يُبحّث عن operator overload
                // (EN) Unsupported operator for overloading — no overload lookup
                break;
        }
        
        if (!opSafeName.empty()) {
            // (AR) البحث عن دالة العامل في الصنف أو الأصناف الأب
            // (EN) Look for operator function in the class or parent classes
            std::string searchClass = leftClassName;
            std::string fullOpName;
            bool found = false;
            while (!searchClass.empty()) {
                fullOpName = searchClass + "." + opSafeName;
                auto funcIt = functionTable_.find(fullOpName);
                if (funcIt != functionTable_.end()) {
                    found = true;
                    break;
                }
                // (AR) البحث في الأب
                // (EN) Search in parent
                auto parentClass = module_->getClass(searchClass);
                if (parentClass && !parentClass->parentClass.empty()) {
                    searchClass = parentClass->parentClass;
                } else {
                    break;
                }
            }
            
            if (found) {
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: dispatching to operator overload '" 
                          << fullOpName << "'" << std::endl;
                #endif
                
                std::string resultReg = newTempRegister();
                auto& opInfo = functionTable_[fullOpName];
                SIRType returnType = opInfo.returnType;
                
                if (currentBlock_) {
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::OBJECT_CALL;
                    callInst.result = SIROperand::Register(resultReg, returnType);
                    callInst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                    callInst.operands.push_back(SIROperand::ConstantString(opSafeName));
                    // (AR) إضافة المعامل الأيمن
                    // (EN) Add right operand
                    if (rightResult.isConstant && !rightResult.constantValue.empty()) {
                        switch (rightResult.type) {
                            case SIRType::I64:
                                callInst.operands.push_back(SIROperand::ConstantI64(std::stoll(rightResult.constantValue)));
                                break;
                            case SIRType::F64:
                                callInst.operands.push_back(SIROperand::ConstantF64(std::stod(rightResult.constantValue)));
                                break;
                            case SIRType::STRING:
                                callInst.operands.push_back(SIROperand::ConstantString(rightResult.constantValue));
                                break;
                            default:
                                callInst.operands.push_back(SIROperand::Register(rightResult.registerName, rightResult.type));
                                break;
                        }
                    } else {
                        callInst.operands.push_back(SIROperand::Register(rightResult.registerName, rightResult.type));
                    }
                    currentBlock_->addInstruction(callInst);
                }
                
                BuildResult result(resultReg, returnType);
                result.className = leftClassName;
                return result;
            }
        }
    }
    
    // (AR) إنشاء سجل للنتيجة (sir_builder.h:511 - newTempRegister)
    // (EN) Create result register
    std::string resultReg = newTempRegister();
    
    // (AR) تحديد نوع النتيجة - إذا كان أحد المعاملين عشري، النتيجة عشرية
    // (EN) Determine result type - if either operand is float, result is float
    // (AR) إذا كان أحد المعاملين نصي، النتيجة نصية (دمج نصوص)
    // (EN) If either operand is string, result is string (concatenation)
    SIRType resultType = leftResult.type;
    bool isStringOp = (leftResult.type == SIRType::STRING || rightResult.type == SIRType::STRING);
    
    if (isStringOp) {
        resultType = SIRType::STRING;
    } else if (leftResult.type == SIRType::F64 || rightResult.type == SIRType::F64) {
        resultType = SIRType::F64;
    }
    
    // (AR) تحديد رمز العملية (SIROpcode) بناءً على TokenType (token.h:205-229)
    // (EN) Determine SIROpcode based on TokenType
    SIROpcode opcode;
    bool isComparison = false;  // (AR) عمليات المقارنة تُرجع BOOL
    
    // (AR) العملية من expressions.h:43 - op: Lexer::TokenType
    // (EN) Operation from expressions.h:43
    switch (binOp->op) {
        // ========== العمليات الحسابية (token.h:205-210) ==========
        case Lexer::TokenType::OP_PLUS:
            // (AR) إذا كانت العملية على نصوص: STRING_CONCAT (sir_types.h:182)
            // (EN) If operation on strings: STRING_CONCAT
            if (isStringOp) {
                opcode = SIROpcode::STRING_CONCAT;
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: عملية دمج نصوص (+)" << std::endl;
                #endif
            } else {
                // (AR) جمع: ADD_I64 للأعداد الصحيحة، ADD_F64 للعشرية
                opcode = (resultType == SIRType::F64) ? SIROpcode::ADD_F64 : SIROpcode::ADD_I64;
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: عملية جمع (+)" << std::endl;
                #endif
            }
            break;
            
        case Lexer::TokenType::OP_MINUS:
            // (AR) طرح: SUB_I64 للأعداد الصحيحة، SUB_F64 للعشرية
            opcode = (resultType == SIRType::F64) ? SIROpcode::SUB_F64 : SIROpcode::SUB_I64;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية طرح (-)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_MULTIPLY:
            // (AR) ضرب: MUL_I64 للأعداد الصحيحة، MUL_F64 للعشرية
            opcode = (resultType == SIRType::F64) ? SIROpcode::MUL_F64 : SIROpcode::MUL_I64;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية ضرب (*)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_DIVIDE:
            // (AR) قسمة: DIV_I64 للأعداد الصحيحة، DIV_F64 للعشرية
            opcode = (resultType == SIRType::F64) ? SIROpcode::DIV_F64 : SIROpcode::DIV_I64;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية قسمة (/)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_MODULO:
            // (AR) باقي القسمة: MOD_I64 (لا يوجد للعشري)
            opcode = SIROpcode::MOD_I64;
            resultType = SIRType::I64;  // (AR) باقي القسمة دائماً عدد صحيح
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية باقي القسمة (%)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_POWER:
            // (AR) الأس: BUILTIN_POW (sir_types.h:223)
            opcode = SIROpcode::BUILTIN_POW;
            resultType = SIRType::F64;  // (AR) نتيجة الأس عادةً عشرية
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية الأس (**)" << std::endl;
            #endif
            break;
        
        // ========== عمليات المقارنة (token.h:219-224) ==========
        case Lexer::TokenType::OP_EQUAL:
            // (AR) يساوي: EQ للأرقام، STRING_CMP للنصوص (sir_types.h:125, 183)
            // (EN) Equal: EQ for numbers, STRING_CMP for strings
            if (isStringOp) {
                opcode = SIROpcode::STRING_CMP;
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: عملية مقارنة نصوص (==)" << std::endl;
                #endif
            } else {
                opcode = SIROpcode::EQ;
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: عملية يساوي (==)" << std::endl;
                #endif
            }
            isComparison = true;
            break;
            
        case Lexer::TokenType::OP_NOT_EQUAL:
            // (AR) لا يساوي: NE للأرقام، STRING_CMP مع NOT للنصوص
            // (EN) Not equal: NE for numbers, STRING_CMP with NOT for strings
            if (isStringOp) {
                // (AR) سيتم معالجة النفي لاحقاً
                opcode = SIROpcode::STRING_CMP;
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: عملية عدم تساوي نصوص (!=)" << std::endl;
                #endif
            } else {
                opcode = SIROpcode::NE;
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: عملية لا يساوي (!=)" << std::endl;
                #endif
            }
            isComparison = true;
            break;
            
        case Lexer::TokenType::OP_LESS:
            // (AR) أصغر من: LT (sir_types.h:127)
            opcode = SIROpcode::LT;
            isComparison = true;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية أصغر من (<)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_LESS_EQUAL:
            // (AR) أصغر أو يساوي: LE (sir_types.h:128)
            opcode = SIROpcode::LE;
            isComparison = true;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية أصغر أو يساوي (<=)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_GREATER:
            // (AR) أكبر من: GT (sir_types.h:129)
            opcode = SIROpcode::GT;
            isComparison = true;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية أكبر من (>)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_GREATER_EQUAL:
            // (AR) أكبر أو يساوي: GE (sir_types.h:130)
            opcode = SIROpcode::GE;
            isComparison = true;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية أكبر أو يساوي (>=)" << std::endl;
            #endif
            break;
        
        // ========== العمليات المنطقية (token.h:227-228) ==========
        case Lexer::TokenType::OP_AND:
            // (AR) AND المنطقي: AND (sir_types.h:118)
            opcode = SIROpcode::AND;
            isComparison = true;  // (AR) النتيجة منطقية
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية AND (&&)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_OR:
            // (AR) OR المنطقي: OR (sir_types.h:119)
            opcode = SIROpcode::OR;
            isComparison = true;  // (AR) النتيجة منطقية
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية OR (||)" << std::endl;
            #endif
            break;

        // ========== العمليات البتية (token.h) ==========
        case Lexer::TokenType::OP_XOR:
            // (AR) XOR بتّي: Xor (sir_opcodes.h)
            opcode = SIROpcode::XOR;
            resultType = SIRType::I64;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية XOR بتي (^)" << std::endl;
            #endif
            break;

        case Lexer::TokenType::OP_BITWISE_AND:
            // (AR) AND بتّي: AND
            opcode = SIROpcode::AND;
            resultType = SIRType::I64;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية AND بتي (&)" << std::endl;
            #endif
            break;

        case Lexer::TokenType::OP_BITWISE_OR:
            // (AR) OR بتّي: OR
            opcode = SIROpcode::OR;
            resultType = SIRType::I64;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية OR بتي (|)" << std::endl;
            #endif
            break;

        case Lexer::TokenType::OP_SHIFT_LEFT:
            // (AR) إزاحة يسار: Shl
            opcode = SIROpcode::SHL;
            resultType = SIRType::I64;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية إزاحة يسار (<<)" << std::endl;
            #endif
            break;

        case Lexer::TokenType::OP_SHIFT_RIGHT:
            // (AR) إزاحة يمين: Shr
            opcode = SIROpcode::SHR;
            resultType = SIRType::I64;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية إزاحة يمين (>>)" << std::endl;
            #endif
            break;
            
        default:
            // (AR) عملية غير مدعومة
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildBinaryOp: عملية غير مدعومة: " 
                      << static_cast<int>(binOp->op) << std::endl;
            #endif
            errors_.push_back("عملية ثنائية غير مدعومة / Unsupported binary operation");
            return BuildResult(resultReg, resultType);
    }
    
    // (AR) نوع النتيجة للمقارنات هو BOOL
    // (EN) Result type for comparisons is BOOL
    if (isComparison) {
        resultType = SIRType::BOOL;
    }
    
    // (AR) إنشاء معاملات SIR (sir_types.h:306-312 - SIROperand::Register)
    // (EN) Create SIR operands
    SIROperand leftOp, rightOp, resultOp;
    
    // (AR) المعامل الأيسر
    if (leftResult.isConstant) {
        // (AR) قيمة ثابتة - استخدم ConstantI64/ConstantF64/ConstantString/ConstantBool
        if (leftResult.type == SIRType::STRING) {
            leftOp = SIROperand::ConstantString(leftResult.constantValue);
        } else if (leftResult.type == SIRType::F64) {
            leftOp = SIROperand::ConstantF64(std::stod(leftResult.constantValue));
        } else if (leftResult.type == SIRType::BOOL) {
            leftOp = SIROperand::ConstantBool(leftResult.constantValue == "true");
        } else if (leftResult.type == SIRType::PTR) {
            // (AR) لاشيء/null → مؤشر فارغ
            leftOp = SIROperand::ConstantI64(0);
            leftOp.dataType = SIRType::PTR;
        } else {
            leftOp = SIROperand::ConstantI64(std::stoll(leftResult.constantValue));
        }
    } else {
        // (AR) سجل - استخدم Register
        leftOp = SIROperand::Register(leftResult.registerName, leftResult.type);
    }
    
    // (AR) المعامل الأيمن
    if (rightResult.isConstant) {
        if (rightResult.type == SIRType::STRING) {
            rightOp = SIROperand::ConstantString(rightResult.constantValue);
        } else if (rightResult.type == SIRType::F64) {
            rightOp = SIROperand::ConstantF64(std::stod(rightResult.constantValue));
        } else if (rightResult.type == SIRType::BOOL) {
            rightOp = SIROperand::ConstantBool(rightResult.constantValue == "true");
        } else if (rightResult.type == SIRType::PTR) {
            // (AR) لاشيء/null → مؤشر فارغ
            rightOp = SIROperand::ConstantI64(0);
            rightOp.dataType = SIRType::PTR;
        } else {
            rightOp = SIROperand::ConstantI64(std::stoll(rightResult.constantValue));
        }
    } else {
        rightOp = SIROperand::Register(rightResult.registerName, rightResult.type);
    }
    
    // (AR) سجل النتيجة
    resultOp = SIROperand::Register(resultReg, resultType);
    
    // (AR) تحويل i64 إلى f64 إذا كانت العملية عشرية ولكن أحد المعاملين صحيح
    // (EN) Convert i64 to f64 if operation is float but one operand is integer
    if (resultType == SIRType::F64) {
        if (leftOp.dataType == SIRType::I64 && !leftResult.isConstant) {
            std::string convReg = newTempRegister();
            SIRInstruction convInst;
            convInst.opcode = SIROpcode::I64_TO_F64;
            convInst.result = SIROperand::Register(convReg, SIRType::F64);
            convInst.operands.push_back(leftOp);
            if (currentBlock_) currentBlock_->addInstruction(convInst);
            leftOp = SIROperand::Register(convReg, SIRType::F64);
        } else if (leftResult.isConstant && leftResult.type == SIRType::I64) {
            double val = std::stod(leftResult.constantValue);
            leftOp = SIROperand::ConstantF64(val);
        }
        if (rightOp.dataType == SIRType::I64 && !rightResult.isConstant) {
            std::string convReg = newTempRegister();
            SIRInstruction convInst;
            convInst.opcode = SIROpcode::I64_TO_F64;
            convInst.result = SIROperand::Register(convReg, SIRType::F64);
            convInst.operands.push_back(rightOp);
            if (currentBlock_) currentBlock_->addInstruction(convInst);
            rightOp = SIROperand::Register(convReg, SIRType::F64);
        } else if (rightResult.isConstant && rightResult.type == SIRType::I64) {
            double val = std::stod(rightResult.constantValue);
            rightOp = SIROperand::ConstantF64(val);
        }
    }
    
    // (AR) إنشاء تعليمة SIR (sir_instruction.h:100-107 - SIRInstruction::Binary)
    // (EN) Create SIR instruction
    SIRInstruction inst = SIRInstruction::Binary(opcode, resultOp, leftOp, rightOp);
    
    // (AR) إضافة التعليمة للكتلة الحالية
    // (EN) Add instruction to current block
    if (currentBlock_) {
        currentBlock_->addInstruction(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildBinaryOp: تمت إضافة التعليمة للكتلة الحالية" << std::endl;
        #endif
    } else {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildBinaryOp: تحذير - لا توجد كتلة حالية!" << std::endl;
        #endif
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildBinaryOp: النتيجة في سجل " << resultReg 
              << " بنوع " << static_cast<int>(resultType) << std::endl;
    #endif
    
    return BuildResult(resultReg, resultType);
}

// ============================================================================
// buildUnaryOp - بناء عملية أحادية
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:456
// التوقيع / Signature: BuildResult buildUnaryOp(AST::UnaryOpNode* unaryOp);
//
// المعاملات / Parameters:
// - unaryOp: AST::UnaryOpNode* = Sad::AST::UnaryExpr* (sir_builder.h:67)
//
// UnaryExpr Members (expressions.h:78-81):
// - op: Lexer::TokenType (line 80)
// - operand: ExprPtr (line 81)
//
// TokenType للعمليات الأحادية (token.h):
// - OP_MINUS (206): السالب
// - OP_NOT (229): النفي المنطقي
//
// SIROpcode (sir_types.h):
// - NEG (115): السالب
// - NOT (121): النفي
//
// SIRInstruction::Unary (sir_instruction.h:114-120):
// - الاستخدام: SIRInstruction::Unary(opcode, result, operand)
// ============================================================================
BuildResult SIRBuilder::buildUnaryOp(AST::UnaryOpNode* unaryOp) {
    if (!unaryOp) {
        return BuildResult();
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildUnaryOp: بدء بناء عملية أحادية" << std::endl;
    #endif
    
    // (AR) بناء المعامل (expressions.h:81 - operand: ExprPtr)
    // (EN) Build operand
    auto operandResult = buildExpression(unaryOp->operand.get());
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildUnaryOp: operandResult.registerName='" << operandResult.registerName 
              << "', type=" << static_cast<int>(operandResult.type) << std::endl;
    #endif
    
    // (AR) إنشاء سجل للنتيجة (sir_builder.h:511 - newTempRegister)
    // (EN) Create result register
    std::string resultReg = newTempRegister();
    
    // (AR) تحديد نوع النتيجة
    // (EN) Determine result type
    SIRType resultType = operandResult.type;
    
    // (AR) تحديد رمز العملية (SIROpcode) بناءً على TokenType
    // (EN) Determine SIROpcode based on TokenType
    SIROpcode opcode;
    
    // (AR) العملية من expressions.h:80 - op: Lexer::TokenType
    switch (unaryOp->op) {
        case Lexer::TokenType::OP_MINUS:
            // (AR) السالب: NEG (sir_types.h:115)
            opcode = SIROpcode::NEG;
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildUnaryOp: عملية السالب (-)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_NOT:
            // (AR) النفي المنطقي: NOT (sir_types.h:121)
            opcode = SIROpcode::NOT;
            resultType = SIRType::BOOL;  // (AR) النفي يُرجع قيمة منطقية
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildUnaryOp: عملية النفي (!)" << std::endl;
            #endif
            break;
            
        case Lexer::TokenType::OP_BITWISE_NOT:
            // (AR) النفي البتّي: NOT (sir_types.h:134)
            opcode = SIROpcode::NOT;
            // (AR) النتيجة بنفس نوع المعامل (عدد صحيح)
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildUnaryOp: عملية النفي البتّي (~)" << std::endl;
            #endif
            break;
            
        default:
            // (AR) عملية غير مدعومة
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildUnaryOp: عملية غير مدعومة: " 
                      << static_cast<int>(unaryOp->op) << std::endl;
            #endif
            errors_.push_back("عملية أحادية غير مدعومة / Unsupported unary operation");
            return BuildResult(resultReg, resultType);
    }
    
    // (AR) إنشاء معاملات SIR
    // (EN) Create SIR operands
    SIROperand operandOp, resultOp;
    
    // (AR) المعامل
    if (operandResult.isConstant) {
        if (operandResult.type == SIRType::F64) {
            operandOp = SIROperand::ConstantF64(std::stod(operandResult.constantValue));
        } else if (operandResult.type == SIRType::BOOL) {
            operandOp = SIROperand::ConstantBool(operandResult.constantValue == "true" || operandResult.constantValue == "صحيح");
        } else {
            operandOp = SIROperand::ConstantI64(std::stoll(operandResult.constantValue));
        }
    } else {
        operandOp = SIROperand::Register(operandResult.registerName, operandResult.type);
    }
    
    // (AR) سجل النتيجة
    resultOp = SIROperand::Register(resultReg, resultType);
    
    // (AR) إنشاء تعليمة SIR (sir_instruction.h:114-120 - SIRInstruction::Unary)
    // (EN) Create SIR instruction
    SIRInstruction inst = SIRInstruction::Unary(opcode, resultOp, operandOp);
    
    // (AR) إضافة التعليمة للكتلة الحالية
    // (EN) Add instruction to current block
    if (currentBlock_) {
        currentBlock_->addInstruction(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildUnaryOp: تمت إضافة التعليمة للكتلة الحالية" << std::endl;
        #endif
    } else {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildUnaryOp: تحذير - لا توجد كتلة حالية!" << std::endl;
        #endif
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildUnaryOp: النتيجة في سجل " << resultReg << std::endl;
    #endif
    
    return BuildResult(resultReg, resultType);
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
