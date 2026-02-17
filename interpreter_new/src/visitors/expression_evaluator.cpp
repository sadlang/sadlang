/**
 * @file expression_evaluator.cpp
 * @brief (AR) تنفيذ مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "expression_evaluator.h"
#include "statement_executor.h"
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h" // For AwaitExpr
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "exception.h"
#include <cmath>
#include <iostream>
#include <unordered_map>

namespace Sad {
namespace Interpreter {

using namespace Data;
using namespace AST;
using namespace Lexer;

// =========================================================================
// (AR) تحسين النصوص العربية / (EN) Arabic String Optimization
// =========================================================================

bool ExpressionEvaluator::containsArabic(const std::string& str) {
    // فحص إذا كان النص يحتوي أحرف عربية (UTF-8)
    // Arabic Unicode range: U+0600-U+06FF (UTF-8: 0xD8 0x80 - 0xDB 0xBF)
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(str.data());
    size_t len = str.size();
    for (size_t i = 0; i + 1 < len; ++i) {
        if (bytes[i] >= 0xD8 && bytes[i] <= 0xDB) {
            return true;
        }
    }
    return false;
}

const std::string& ExpressionEvaluator::internString(const std::string& str) {
    arabicOptStats_.totalStrings++;
    
    if (containsArabic(str)) {
        arabicOptStats_.arabicStrings++;
    }
    
    auto [it, inserted] = stringPool_.insert(str);
    if (inserted) {
        // نص جديد - تم إضافته للمُجمّع
        arabicOptStats_.pooledStrings++;
    } else {
        // نص مكرر - تم توفير الذاكرة
        arabicOptStats_.poolHits++;
        arabicOptStats_.savedBytes += str.size();
    }
    
    return *it;
}

void ExpressionEvaluator::printArabicOptStats() const {
    std::cout << "\n========================================\n";
    std::cout << "إحصائيات التحسين العربي (المفسّر) / Arabic Optimization Statistics (Interpreter)\n";
    std::cout << "========================================\n\n";
    
    std::cout << "📝 تحسينات النصوص / String Optimizations:\n";
    std::cout << "  • مجموع النصوص / Total strings: " << arabicOptStats_.totalStrings << "\n";
    std::cout << "  • النصوص الفريدة / Unique strings: " << arabicOptStats_.pooledStrings << "\n";
    std::cout << "  • إصابات المُجمّع / Pool hits: " << arabicOptStats_.poolHits << "\n";
    std::cout << "  • النصوص العربية / Arabic strings: " << arabicOptStats_.arabicStrings << "\n";
    std::cout << "  • البايتات الموفرة / Saved bytes: " << arabicOptStats_.savedBytes << "\n";
    
    if (arabicOptStats_.totalStrings > 0) {
        double hitRate = (static_cast<double>(arabicOptStats_.poolHits) / arabicOptStats_.totalStrings) * 100.0;
        std::cout << "  • نسبة الإصابة / Hit rate: " << hitRate << "%\n";
    }
    
    std::cout << "========================================\n\n";
}

// =========================================================================
// (AR) تقييم القيم الحرفية / (EN) Literal Evaluation
// =========================================================================

void ExpressionEvaluator::visitLiteralExpr(LiteralExpr& node) {
    lastResult_ = tokenToValue(node.token);
}

Value ExpressionEvaluator::tokenToValue(const Token& token) {
    switch (token.getType()) {
        case TokenType::NUMBER_INTEGER: {
            std::string value = token.getValue();
            
            // Binary: 0b1010
            if (value.size() > 2 && value[0] == '0' && (value[1] == 'b' || value[1] == 'B')) {
                return Value(static_cast<int>(std::stoll(value.substr(2), nullptr, 2)));
            }
            
            // Octal: 0o17
            if (value.size() > 2 && value[0] == '0' && (value[1] == 'o' || value[1] == 'O')) {
                return Value(static_cast<int>(std::stoll(value.substr(2), nullptr, 8)));
            }
            
            // Hexadecimal: 0xFF
            if (value.size() > 2 && value[0] == '0' && (value[1] == 'x' || value[1] == 'X')) {
                return Value(static_cast<int>(std::stoll(value.substr(2), nullptr, 16)));
            }
            
            // Decimal: 42
            return Value(std::stoi(value));
        }
        
        case TokenType::NUMBER_DOUBLE:
            return Value(std::stod(token.getValue()));
        
        case TokenType::STRING_LITERAL:
            return Value(internString(token.getValue()));
        
        case TokenType::LITERAL_TRUE:
            return Value(true);
        
        case TokenType::LITERAL_FALSE:
            return Value(false);
        
        case TokenType::LITERAL_NULL:
            return Value();  // VOID
        
        default:
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation("<input>", 1, 1),
                "نوع رمز غير مدعوم: " + token.getValue(),
                "Unsupported token type: " + token.getValue()
            );
            return Value(); // Return null
    }
}

// =========================================================================
// (AR) تقييم المتغيرات / (EN) Variable Evaluation
// =========================================================================

void ExpressionEvaluator::visitVariableExpr(VariableExpr& node) {
    // التحقق من وجود المتغير
    // Check if variable exists
    if (!variableManager_.exists(node.name)) {
        // التحقق من وجود صنف بهذا الاسم (للوصول الثابت)
        // Check if class exists with this name (for static access)
        auto* classManager = Data::ClassManager::getInstance();
        ClassType* classType = classManager->getClass(node.name);
        
        if (classType) {
            // إرجاع اسم الصنف كـ string للتعامل معه في MemberExpr/MethodCallExpr
            // Return class name as string to be handled in MemberExpr/MethodCallExpr
            lastResult_ = Value(node.name);
            return;
        }
        
        // متغير غير معرّف
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "متغير غير معرّف: " + node.name,
            "Undefined variable: " + node.name
        );
        lastResult_ = Value(); // Return null
        return;
    }
    
    // (AR) فحص الملكية: هل المتغير صالح للاستخدام؟ / (EN) Ownership check: is variable valid?
    if (ownershipManager_.isEnabled()) {
        auto error = ownershipManager_.useVariable(node.name);
        if (error.has_value()) {
            // (AR) المتغير منقول أو غير صالح / (EN) Variable moved or invalid
            throw SadException(
                error->arabicMessage + " / " + error->message,
                "OwnershipError",
                node.position
            );
        }
    }
    
    lastResult_ = variableManager_.get(node.name);
}

void ExpressionEvaluator::visitBorrowExpr(BorrowExpr& node) {
    // (AR) التحقق من وجود المتغير / (EN) Check variable exists
    if (!variableManager_.exists(node.variableName)) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xB9\xD8\xB1\xD9\x91\xD9\x81: " + node.variableName,
            "Undefined variable: " + node.variableName
        );
        lastResult_ = Value();
        return;
    }
    
    // (AR) إنشاء استعارة في نظام الملكية / (EN) Create borrow in ownership system
    if (ownershipManager_.isEnabled()) {
        auto borrowKind = node.isMutable 
            ? Data::BorrowKind::Mutable 
            : Data::BorrowKind::Shared;
        // (AR) اسم المستعير المؤقت / (EN) Temporary borrower name
        std::string borrowerName = "__borrow_" + node.variableName;
        auto error = ownershipManager_.createBorrow(node.variableName, borrowerName, borrowKind);
        if (error.has_value()) {
            throw SadException(
                error->arabicMessage + " / " + error->message,
                "OwnershipError",
                node.position
            );
        }
    }
    
    // (AR) إرجاع قيمة المتغير (الاستعارة تُرجع نفس القيمة)
    // (EN) Return variable value (borrow returns same value)
    lastResult_ = variableManager_.get(node.variableName);
}

void ExpressionEvaluator::visitThisExpr(ThisExpr& node) {
    // الحصول على this من النطاق الحالي
    // Get 'this' from current scope
    if (variableManager_.exists("هذا")) {
        lastResult_ = variableManager_.get("هذا");
    } else if (variableManager_.exists("this")) {
        lastResult_ = variableManager_.get("this");
    } else {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "(AR) 'هذا' غير متاح في هذا السياق. (EN) 'this' is not available in this context.",
            "'this' keyword used outside of class context"
        );
        lastResult_ = Value();
    }
}

void ExpressionEvaluator::visitSuperExpr(SuperExpr& node) {
    // الحصول على super من النطاق الحالي
    // Get 'super' from current scope
    if (variableManager_.exists("الأساس")) {
        lastResult_ = variableManager_.get("الأساس");
    } else if (variableManager_.exists("super")) {
        lastResult_ = variableManager_.get("super");
    } else {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "(AR) 'الأساس' غير متاح في هذا السياق. (EN) 'super' is not available in this context.",
            "'super' keyword used outside of class context or class without base"
        );
        lastResult_ = Value();
    }
}

// =========================================================================
// (AR) تقييم الإسناد / (EN) Assignment Evaluation
// =========================================================================

void ExpressionEvaluator::visitAssignExpr(AssignExpr& node) {
    // (AR) التحقق إذا كان الطرف الأيمن متغير (للنقل المحتمل)
    // (EN) Check if RHS is a variable (for potential move)
    std::string sourceVarName;
    auto* varExpr = dynamic_cast<VariableExpr*>(node.value.get());
    if (varExpr) {
        sourceVarName = varExpr->name;
    }
    
    // تقييم القيمة اليمنى / Evaluate right-hand side
    node.value->accept(*this);
    Value value = lastResult_;
    
    // (AR) التحقق إذا كان المتغير موجود / (EN) Check if variable exists
    // إذا لم يكن موجوداً، قم بتعريفه (استدلال النوع)
    // If not exists, define it (type inference)
    if (!variableManager_.exists(node.name)) {
        // (AR) المتغير غير موجود - قم بتعريفه تلقائياً
        // (EN) Variable doesn't exist - auto-define it
        variableManager_.define(node.name, value);
        
        // (AR) تسجيل في نظام الملكية / (EN) Register in ownership system
        if (ownershipManager_.isEnabled()) {
            ownershipManager_.declareVariable(node.name);
            
            // (AR) نقل الملكية من المتغير المصدر (إن وُجد ولم يكن قابلاً للنسخ)
            // (EN) Move ownership from source variable (if exists and not copy type)
            if (!sourceVarName.empty() && sourceVarName != node.name) {
                auto moveError = ownershipManager_.moveVariable(sourceVarName);
                if (moveError.has_value()) {
                    throw SadException(
                        moveError->arabicMessage + " / " + moveError->message,
                        "OwnershipError",
                        node.position
                    );
                }
            }
        }
    } else {
        // (AR) فحص الملكية: هل يمكن التعديل؟ / (EN) Ownership check: can mutate?
        if (ownershipManager_.isEnabled()) {
            auto error = ownershipManager_.mutateVariable(node.name);
            if (error.has_value()) {
                throw SadException(
                    error->arabicMessage + " / " + error->message,
                    "OwnershipError",
                    node.position
                );
            }
        }
        
        // إسناد للمتغير الموجود / Assign to existing variable
        variableManager_.assign(node.name, value);
    }
    
    // الإسناد يُرجع القيمة المُسندة / Assignment returns assigned value
    lastResult_ = value;
}

// =========================================================================
// (AR) تقييم التعابير الثنائية / (EN) Binary Expression Evaluation
// =========================================================================

void ExpressionEvaluator::visitBinaryExpr(BinaryExpr& node) {
    // تقييم الطرف الأيسر / Evaluate left operand
    node.left->accept(*this);
    Value left = lastResult_;
    
    // تقييم الطرف الأيمن / Evaluate right operand
    node.right->accept(*this);
    Value right = lastResult_;
    
    // (AR) فحص تحميل العامل الزائد على الكائنات / (EN) Check operator overloading on objects
    if (left.isMap()) {
        auto fields = left.toMap();
        auto classNameIt = fields.find("__class__");
        if (classNameIt != fields.end()) {
            std::string className = classNameIt->second.toString();
            auto* classManager = Data::ClassManager::getInstance();
            Data::ClassType* classType = classManager->getClass(className);
            if (classType) {
                // (AR) تحويل رمز العامل من TokenType إلى نص / (EN) Convert operator token to string
                std::string opSymbol;
                switch (node.op) {
                    case TokenType::OP_PLUS:          opSymbol = "+"; break;
                    case TokenType::OP_MINUS:         opSymbol = "-"; break;
                    case TokenType::OP_MULTIPLY:      opSymbol = "*"; break;
                    case TokenType::OP_DIVIDE:        opSymbol = "/"; break;
                    case TokenType::OP_MODULO:        opSymbol = "%"; break;
                    case TokenType::OP_POWER:         opSymbol = "**"; break;
                    case TokenType::OP_EQUAL:         opSymbol = "=="; break;
                    case TokenType::OP_NOT_EQUAL:     opSymbol = "!="; break;
                    case TokenType::OP_LESS:          opSymbol = "<"; break;
                    case TokenType::OP_LESS_EQUAL:    opSymbol = "<="; break;
                    case TokenType::OP_GREATER:       opSymbol = ">"; break;
                    case TokenType::OP_GREATER_EQUAL: opSymbol = ">="; break;
                    default: break;
                }
                
                if (!opSymbol.empty()) {
                    Data::OperatorOverload* opOverload = classType->findOperator(opSymbol);
                    if (opOverload && opOverload->body) {
                        // (AR) تنفيذ العامل المحمل زائداً / (EN) Execute operator overload
                        lastResult_ = executeOperatorOverload(left, *opOverload, right, node.position);
                        return;
                    }
                }
            }
        }
    }
    
    // تحديد نوع العملية / Determine operation type
    switch (node.op) {
        // (AR) عمليات حسابية / (EN) Arithmetic operations
        case TokenType::OP_PLUS:
        case TokenType::OP_MINUS:
        case TokenType::OP_MULTIPLY:
        case TokenType::OP_DIVIDE:
        case TokenType::OP_MODULO:
        case TokenType::OP_POWER:
            lastResult_ = evaluateArithmeticOp(left, node.op, right, node.position);
            break;
        
        // (AR) عمليات مقارنة / (EN) Comparison operations
        case TokenType::OP_EQUAL:
        case TokenType::OP_NOT_EQUAL:
        case TokenType::OP_LESS:
        case TokenType::OP_LESS_EQUAL:
        case TokenType::OP_GREATER:
        case TokenType::OP_GREATER_EQUAL:
            lastResult_ = evaluateComparisonOp(left, node.op, right, node.position);
            break;
        
        // (AR) عمليات منطقية / (EN) Logical operations
        case TokenType::OP_AND:
        case TokenType::OP_OR:
            lastResult_ = evaluateLogicalOp(left, node.op, right, node.position);
            break;
        
        default:
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "عملية ثنائية غير مدعومة",
                "Unsupported binary operation"
            );
            lastResult_ = Value(); // Return null
    }
}

// =========================================================================
// (AR) العمليات الحسابية / (EN) Arithmetic Operations
// =========================================================================

Value ExpressionEvaluator::evaluateArithmeticOp(const Value& left, TokenType op, const Value& right, const Lexer::Position& pos) {
    // جمع النصوص (string concatenation) / String concatenation
    if (op == TokenType::OP_PLUS && (left.isString() || right.isString())) {
        return Value(left.toString() + right.toString());
    }
    
    // التأكد من أن الطرفين رقميين / Ensure both operands are numeric
    if (!left.isNumeric() || !right.isNumeric()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation("<expression>", pos.line, pos.column),
            "العمليات الحسابية تتطلب قيم رقمية",
            "Arithmetic operations require numeric values"
        );
        return Value(0); // Return default
    }
    
    // تحويل لـ double إذا كان أحدهما double
    // Convert to double if either is double
    bool useDouble = left.isDouble() || right.isDouble();
    
    if (useDouble) {
        double l = left.toDouble();
        double r = right.toDouble();
        
        switch (op) {
            case TokenType::OP_PLUS:     return Value(l + r);
            case TokenType::OP_MINUS:    return Value(l - r);
            case TokenType::OP_MULTIPLY: return Value(l * r);
            case TokenType::OP_DIVIDE:
                if (r == 0.0) {
                    throw DivisionByZeroError(
                        "(AR) لا يمكن القسمة على صفر / (EN) Cannot divide by zero",
                        pos
                    );
                }
                return Value(l / r);
            case TokenType::OP_POWER:    return Value(std::pow(l, r));
            case TokenType::OP_MODULO:   return Value(std::fmod(l, r));
            default: break;
        }
    } else {
        int l = left.toInt();
        int r = right.toInt();
        
        switch (op) {
            case TokenType::OP_PLUS:     return Value(l + r);
            case TokenType::OP_MINUS:    return Value(l - r);
            case TokenType::OP_MULTIPLY: return Value(l * r);
            case TokenType::OP_DIVIDE:
                if (r == 0) {
                    throw DivisionByZeroError(
                        "(AR) لا يمكن القسمة على صفر / (EN) Cannot divide by zero",
                        pos
                    );
                }
                return Value(l / r);
            case TokenType::OP_MODULO:   return Value(l % r);
            case TokenType::OP_POWER:    return Value(static_cast<int>(std::pow(l, r)));
            default: break;
        }
    }
    
    Sad::Errors::ErrorManager::getInstance().reportError(
        Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
        Sad::Errors::SourceLocation("<expression>", pos.line, pos.column),
        "عملية حسابية غير مدعومة",
        "Unsupported arithmetic operation"
    );
    return Value(0); // Return default
}

// =========================================================================
// (AR) عمليات المقارنة / (EN) Comparison Operations
// =========================================================================

Value ExpressionEvaluator::evaluateComparisonOp(const Value& left, TokenType op, const Value& right, const Lexer::Position& pos) {
    // المقارنة تعمل على أي نوعين / Comparison works on any two types
    
    // معالجة خاصة لـ null (VOID) / Special handling for null (VOID)
    // null يمكن مقارنته بأي نوع / null can be compared with any type
    if (left.isVoid() || right.isVoid()) {
        switch (op) {
            case TokenType::OP_EQUAL:
                return Value(left.isVoid() && right.isVoid());
            case TokenType::OP_NOT_EQUAL:
                return Value(!left.isVoid() || !right.isVoid());
            default:
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation("<expression>", pos.line, pos.column),
                    "لا يمكن استخدام عمليات المقارنة (<، >، <=، >=) مع null. استخدم == أو != فقط",
                    "Cannot use comparison operators (<, >, <=, >=) with null. Use == or != only"
                );
                return Value(false);
        }
    }
    
    // مقارنة الأنواع المختلفة / Different types comparison
    if (left.getType() != right.getType()) {
        // فقط == و != مسموح بهما / Only == and != allowed
        if (op == TokenType::OP_EQUAL) return Value(false);
        if (op == TokenType::OP_NOT_EQUAL) return Value(true);
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation("<expression>", pos.line, pos.column),
            "لا يمكن مقارنة أنواع مختلفة",
            "Cannot compare different types"
        );
        return Value(false); // Return default
    }
    
    // مقارنة الأعداد / Numeric comparison
    if (left.isNumeric() && right.isNumeric()) {
        double l = left.toDouble();
        double r = right.toDouble();
        
        switch (op) {
            case TokenType::OP_EQUAL:         return Value(l == r);
            case TokenType::OP_NOT_EQUAL:     return Value(l != r);
            case TokenType::OP_LESS:          return Value(l < r);
            case TokenType::OP_LESS_EQUAL:    return Value(l <= r);
            case TokenType::OP_GREATER:       return Value(l > r);
            case TokenType::OP_GREATER_EQUAL: return Value(l >= r);
            default: break;
        }
    }
    
    // مقارنة النصوص / String comparison
    if (left.isString() && right.isString()) {
        std::string l = left.toString();
        std::string r = right.toString();
        
        switch (op) {
            case TokenType::OP_EQUAL:         return Value(l == r);
            case TokenType::OP_NOT_EQUAL:     return Value(l != r);
            case TokenType::OP_LESS:          return Value(l < r);
            case TokenType::OP_LESS_EQUAL:    return Value(l <= r);
            case TokenType::OP_GREATER:       return Value(l > r);
            case TokenType::OP_GREATER_EQUAL: return Value(l >= r);
            default: break;
        }
    }
    
    // مقارنة Boolean / Boolean comparison
    if (left.isBoolean() && right.isBoolean()) {
        bool l = left.toBool();
        bool r = right.toBool();
        
        switch (op) {
            case TokenType::OP_EQUAL:     return Value(l == r);
            case TokenType::OP_NOT_EQUAL: return Value(l != r);
            default:
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation("<expression>", pos.line, pos.column),
                    "فقط == و != مسموح بهما للقيم المنطقية",
                    "Only == and != allowed for boolean values"
                );
                return Value(false);
        }
    }
    
    Sad::Errors::ErrorManager::getInstance().reportError(
        Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
        Sad::Errors::SourceLocation("<expression>", pos.line, pos.column),
        "عملية مقارنة غير مدعومة",
        "Unsupported comparison operation"
    );
    return Value(false);
}

// =========================================================================
// (AR) العمليات المنطقية / (EN) Logical Operations
// =========================================================================

Value ExpressionEvaluator::evaluateLogicalOp(const Value& left, TokenType op, const Value& right, const Lexer::Position& pos) {
    bool l = left.toBool();
    bool r = right.toBool();
    
    switch (op) {
        case TokenType::OP_AND: return Value(l && r);
        case TokenType::OP_OR:  return Value(l || r);
        default:
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation("<expression>", pos.line, pos.column),
                "عملية منطقية غير مدعومة",
                "Unsupported logical operation"
            );
            return Value(false);
    }
}

// =========================================================================
// (AR) تنفيذ عامل محمل زائداً / (EN) Operator Overload Execution
// =========================================================================

Value ExpressionEvaluator::executeOperatorOverload(const Value& left, Data::OperatorOverload& overload, const Value& right, const Lexer::Position& pos) {
    // (AR) التحقق من عدد المعاملات / (EN) Verify parameter count
    if (overload.parameters.size() != 1) {
        throw RuntimeError(
            "(AR) العامل '" + overload.operatorSymbol + "' يجب أن يقبل معاملاً واحداً بالضبط. "
            "(EN) Operator '" + overload.operatorSymbol + "' must accept exactly one parameter.",
            pos
        );
    }
    
    // (AR) إنشاء نطاق جديد لتنفيذ العامل / (EN) Create new scope for operator execution
    variableManager_.enterScope(Data::ScopeType::FUNCTION, "operator" + overload.operatorSymbol);
    
    // (AR) ربط 'هذا' بالكائن الأيسر / (EN) Bind 'this' to left object
    variableManager_.define("هذا", left);
    
    // (AR) ربط حقول الكائن الأيسر كمتغيرات محلية (محاكاة this.field → field)
    // (EN) Bind left object fields as local variables (simulate this.field → field)
    if (left.isMap()) {
        auto fields = left.toMap();
        for (const auto& [fieldName, fieldValue] : fields) {
            if (fieldName != "__class__") {
                variableManager_.define(fieldName, fieldValue);
            }
        }
    }
    
    // (AR) ربط المعامل الأيمن / (EN) Bind right operand
    variableManager_.define(overload.parameters[0].name, right);
    
    // (AR) تنفيذ جسم العامل / (EN) Execute operator body
    Value returnValue;
    try {
        overload.body->accept(statementExecutor_);
        
        if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
            returnValue = statementExecutor_.getReturnValue();
            statementExecutor_.resetFlowControl();
        }
    } catch (...) {
        variableManager_.exitScope();
        throw;
    }
    
    variableManager_.exitScope();
    return returnValue;
}

// =========================================================================
// (AR) تقييم التعابير الأحادية / (EN) Unary Expression Evaluation
// =========================================================================

void ExpressionEvaluator::visitUnaryExpr(UnaryExpr& node) {
    // تقييم المعامل / Evaluate operand
    node.operand->accept(*this);
    Value operand = lastResult_;
    
    switch (node.op) {
        case TokenType::OP_MINUS:  // -x
            if (operand.isInteger()) {
                lastResult_ = Value(-operand.toInt());
            } else if (operand.isDouble()) {
                lastResult_ = Value(-operand.toDouble());
            } else {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                    Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "السالب يتطلب قيمة رقمية",
                    "Negation requires numeric value"
                );
                lastResult_ = Value(0);
            }
            break;
        
        case TokenType::OP_NOT:    // !flag
            lastResult_ = Value(!operand.toBool());
            break;
        
        case TokenType::OP_PLUS:   // +x (no-op)
            if (!operand.isNumeric()) {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                    Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "الموجب يتطلب قيمة رقمية",
                    "Positive requires numeric value"
                );
                lastResult_ = Value(0);
                break;
            }
            lastResult_ = operand;
            break;
        
        default:
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "عملية أحادية غير مدعومة",
                "Unsupported unary operation"
            );
            lastResult_ = Value(0);
    }
}

// =========================================================================
// (AR) تقييم التعبير الثلاثي الشرطي / (EN) Ternary Expression Evaluation
// =========================================================================

/**
 * @brief (AR) يُقيّم التعبير الثلاثي الشرطي: شرط ? صحيح : خطأ
 *        (EN) Evaluates ternary conditional expression: condition ? true : false
 * 
 * Evaluates the condition, then returns either true_expression or false_expression
 * based on the condition result.
 * 
 * يُقيّم الشرط، ثم يُرجع إما تعبير_صحيح أو تعبير_خطأ بناءً على نتيجة الشرط.
 * 
 * @example
 * x > 0 ? "positive" : "negative"
 * age >= 18 ? "adult" : "minor"
 * العمر >= 18 ؟ "بالغ" : "قاصر"
 */
void ExpressionEvaluator::visitTernaryExpr(TernaryExpr& node) {
    // Evaluate condition / تقييم الشرط
    node.condition->accept(*this);
    Value condition = lastResult_;
    
    // Based on condition, evaluate either true or false branch
    // بناءً على الشرط، قيّم إما الفرع الصحيح أو الخاطئ
    if (condition.toBool()) {
        // Condition is true, evaluate true branch
        // الشرط صحيح، قيّم الفرع الصحيح
        node.trueExpr->accept(*this);
    } else {
        // Condition is false, evaluate false branch
        // الشرط خاطئ، قيّم الفرع الخاطئ
        node.falseExpr->accept(*this);
    }
    
    // lastResult_ already contains the result from the evaluated branch
    // lastResult_ يحتوي بالفعل على النتيجة من الفرع المُقيّم
}

// =========================================================================
// (AR) تقييم المصفوفات / (EN) Array Evaluation
// =========================================================================

void ExpressionEvaluator::visitArrayExpr(ArrayExpr& node) {
    Value::ArrayType arr;
    
    // تقييم كل عنصر / Evaluate each element
    for (auto& elem : node.elements) {
        elem->accept(*this);
        arr.push_back(lastResult_);
    }
    
    lastResult_ = Value(arr);
}

// =========================================================================
// (AR) تقييم القواميس / (EN) Map Evaluation
// =========================================================================

void ExpressionEvaluator::visitMapExpr(MapExpr& node) {
    Value::MapType map;
    
    // تقييم كل زوج مفتاح-قيمة / Evaluate each key-value pair
    for (auto& pair : node.pairs) {
        // تقييم المفتاح / Evaluate key
        pair.key->accept(*this);
        std::string key = lastResult_.toString();
        
        // تقييم القيمة / Evaluate value
        pair.value->accept(*this);
        Value value = lastResult_;
        
        map[key] = value;
    }
    
    lastResult_ = Value(map);
}

// =========================================================================
// (AR) تقييم الفهرسة / (EN) Index Evaluation
// =========================================================================

void ExpressionEvaluator::visitIndexExpr(IndexExpr& node) {
    // تقييم الكائن المفهرس / Evaluate indexed object
    node.object->accept(*this);
    Value obj = lastResult_;
    
    // تقييم الفهرس / Evaluate index
    node.index->accept(*this);
    Value index = lastResult_;
    
    if (obj.isArray()) {
        // فهرسة مصفوفة / Array indexing
        if (!index.isInteger()) {
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "فهرس المصفوفة يجب أن يكون رقم صحيح",
                "Array index must be integer"
            );
            lastResult_ = Value();
            return;
        }
        
        int idx = index.toInt();
        Value::ArrayType arr = obj.toArray();
        
        if (idx < 0 || idx >= static_cast<int>(arr.size())) {
            throw IndexOutOfRangeError(
                "(AR) الفهرس " + std::to_string(idx) + " خارج النطاق (الحجم: " + std::to_string(arr.size()) + ") / " +
                "(EN) Index " + std::to_string(idx) + " out of range (size: " + std::to_string(arr.size()) + ")",
                node.position
            );
        }
        
        lastResult_ = arr[idx];
        
    } else if (obj.isMap()) {
        // فهرسة قاموس / Map indexing
        std::string key = index.toString();
        Value::MapType map = obj.toMap();
        
        if (map.find(key) == map.end()) {
            throw KeyError(
                "(AR) المفتاح '" + key + "' غير موجود / (EN) Key '" + key + "' not found",
                node.position
            );
        }
        
        lastResult_ = map[key];
        
    } else {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "الفهرسة تعمل فقط على المصفوفات والقواميس",
            "Indexing works only on arrays and maps"
        );
        lastResult_ = Value();
    }
}

// =========================================================================
// (AR) تقييم استدعاء الدالة / (EN) Function Call Evaluation
// =========================================================================

void ExpressionEvaluator::visitCallExpr(CallExpr& node) {
    // (AR) الحصول على اسم الدالة / (EN) Get function name
    std::string funcName;
    bool isTemplateInstantiation = false;
    
    // (AR) التحقق - هل callee هو TemplateInstantiation؟
    // (EN) Check - is callee a TemplateInstantiation?
    auto templateInst = dynamic_cast<TemplateInstantiation*>(node.callee.get());
    if (templateInst) {
        // (AR) هذا استدعاء دالة قالب!
        // (EN) This is a template function call!
        isTemplateInstantiation = true;
        funcName = "__template_" + templateInst->templateName;
        
        #ifdef DEBUG
        std::cout << "[Template] استدعاء دالة قالب: " << templateInst->templateName << "<";
        for (size_t i = 0; i < templateInst->typeArguments.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << static_cast<int>(templateInst->typeArguments[i]);
        }
        std::cout << ">" << std::endl;
        #endif
    } else {
        // (AR) التحقق - هل callee هو VariableExpr (اسم دالة) أم شيء آخر؟
        // (EN) Check - is callee a VariableExpr (function name) or something else?
        auto calleeVar = dynamic_cast<VariableExpr*>(node.callee.get());
        if (calleeVar) {
            // (AR) نتحقق أولاً - هل المتغير موجود؟ / (EN) Check first - does variable exist?
            if (variableManager_.exists(calleeVar->name)) {
                // (AR) قد يكون lambda مخزّن في متغير / (EN) May be lambda stored in variable
                Value varValue = variableManager_.get(calleeVar->name);
                if (varValue.isString() && varValue.toString().find("__lambda_") == 0) {
                    // (AR) هذا lambda! / (EN) This is lambda!
                    funcName = varValue.toString();
                } else {
                    // (AR) دالة عادية / (EN) Regular function
                    funcName = calleeVar->name;
                }
            } else {
                // (AR) دالة عادية غير مخزنة في متغير / (EN) Regular function not stored in variable
                funcName = calleeVar->name;
            }
        } else {
            // (AR) ربما دالة lambda inline / (EN) Maybe inline lambda
            // نقيّم الـ callee لنحصل على اسم الدالة
            node.callee->accept(*this);
            Value calleeValue = lastResult_;
            
            if (!calleeValue.isString()) {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                    Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "استدعاء دالة معقد غير مدعوم حالياً",
                    "Complex function calls not supported yet"
                );
                lastResult_ = Value();
                return;
            }
            
            funcName = calleeValue.toString();
        }
    }
    
    // (AR) تقييم المعاملات / (EN) Evaluate arguments
    std::vector<Data::Value> arguments;
    for (const auto& arg : node.arguments) {
        arg->accept(*this);
        arguments.push_back(lastResult_);
    }
    
    // (AR) أولوية: إذا كنا داخل طريقة صنف، نبحث أولاً عن الطريقة في الصنف الحالي
    //       قبل البحث في الدوال العامة (لتجنب تعارض الأسماء مع الدوال المضمنة)
    // (EN) Priority: if inside a class method, search class methods first
    //       before global functions (to avoid name conflicts with built-ins)
    {
        auto* classManager = Data::ClassManager::getInstance();
        if (variableManager_.exists("هذا")) {
            Value thisValue = variableManager_.get("هذا");
            if (thisValue.isMap()) {
                auto thisFields = thisValue.toMap();
                auto classNameIt = thisFields.find("__class__");
                if (classNameIt != thisFields.end()) {
                    std::string thisClassName = classNameIt->second.toString();
                    Data::ClassType* thisClassType = classManager->getClass(thisClassName);
                    if (thisClassType) {
                        Data::ClassMethod* method = thisClassType->findMethod(funcName);
                        // (AR) البحث عن الجسم: أولاً في الطريقة نفسها، ثم في مصدر القالب
                        // (EN) Find body: first in method itself, then in template instance source
                        AST::Statement* methodBody = nullptr;
                        if (method && method->body) {
                            methodBody = method->body.get();
                        } else if (method) {
                            auto* tplSrc = statementExecutor_.getTemplateInstanceSource(thisClassName);
                            if (tplSrc) {
                                for (auto& m : tplSrc->members) {
                                    if (auto* md = dynamic_cast<AST::MethodDecl*>(m.get())) {
                                        if (md->name == funcName && md->body) {
                                            methodBody = md->body.get();
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        if (method && methodBody) {
                            // (AR) وجدنا طريقة في الصنف الحالي - ننفذها بأولوية
                            // (EN) Found method in current class - execute with priority
                            if (arguments.size() != method->parameters.size()) {
                                std::string errMsg = "(AR) عدد معاملات الطريقة '" + funcName + "' غير متطابق. ";
                                errMsg += "توقع " + std::to_string(method->parameters.size()) + " لكن حصل على " + std::to_string(arguments.size()) + ". ";
                                errMsg += "(EN) Argument count mismatch for method '" + funcName + "'.";
                                throw RuntimeError(errMsg, node.position);
                            }
                            
                            // (AR) ندفع نطاق فقط للمعاملات - الحقول موروثة من النطاق الأب
                            // (EN) Push scope only for parameters - fields inherited from parent scope
                            variableManager_.enterScope(Data::ScopeType::FUNCTION, funcName);
                            
                            for (size_t i = 0; i < method->parameters.size(); ++i) {
                                variableManager_.define(method->parameters[i].name, arguments[i]);
                            }
                            
                            Value returnValue;
                            try {
                                methodBody->accept(statementExecutor_);
                                if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
                                    returnValue = statementExecutor_.getReturnValue();
                                    statementExecutor_.resetFlowControl();
                                }
                            } catch (...) {
                                variableManager_.exitScope();
                                throw;
                            }
                            
                            variableManager_.exitScope();
                            lastResult_ = returnValue;
                            return;
                        }
                    }
                }
            }
        }
    }
    
    // (AR) البحث عن الدالة - flexible matching مع default parameters
    // (EN) Find function - flexible matching with default parameters
    auto allOverloads = functionManager_.getFunctionOverloads(funcName);
    std::shared_ptr<Data::FunctionDefinition> func = nullptr;
    
    // (AR) نبحث عن دالة تقبل هذا العدد من المعاملات (مع الافتراضيات)
    // (EN) Search for function that accepts this argument count (with defaults)
    // (AR) الأولوية 1: الدوال المعرفة من المستخدم التي تطابق عدد المعاملات
    // (EN) Priority 1: User-defined functions that match argument count
    for (const auto& candidate : allOverloads) {
        if (!candidate->hasNativeImplementation() && candidate->acceptsArgumentCount(arguments.size())) {
            func = candidate;
            break;
        }
    }
    // (AR) الأولوية 2: الدوال المضمنة (كنسخة احتياطية)
    // (EN) Priority 2: Built-in functions (as fallback)
    if (!func) {
        for (const auto& candidate : allOverloads) {
            if (candidate->hasNativeImplementation()) {
                func = candidate;
                break;
            }
        }
    }
    
    // (AR) إذا لم نجد دالة مناسبة
    // (EN) If no suitable function found
    if (!func) {
        // (AR) محاولة أخيرة - exact match (للدوال بدون defaults)
        // (EN) Last attempt - exact match (for functions without defaults)
        func = functionManager_.getFunction(funcName, arguments.size());
        
        if (!func) {
            // (AR) التحقق إذا كان الاسم هو اسم صنف مسجّل → إنشاء كائن بدون 'جديد'
            // (EN) Check if name is a registered class → create object without 'new' keyword
            // مثال: مكدس م1 = مكدس()  بدلاً من  متغير م1 = جديد مكدس()
            auto* classManager2 = Data::ClassManager::getInstance();
            Data::ClassType* classType = classManager2->getClass(funcName);
            if (classType) {
                // (AR) بناء عقدة NewExpr مؤقتة والنقل إليها
                // (EN) Build a temporary NewExpr node and delegate
                NewExpr tempNewExpr(funcName);
                tempNewExpr.position = node.position;
                // (AR) نقل المعاملات من الـ CallExpr إلى NewExpr
                // (EN) Move arguments from CallExpr to NewExpr
                for (auto& arg : node.arguments) {
                    tempNewExpr.arguments.push_back(std::move(arg));
                }
                visitNewExpr(tempNewExpr);
                // (AR) إعادة المعاملات للـ CallExpr لتجنب مؤشرات معلقة
                // (EN) Move arguments back to CallExpr to avoid dangling pointers
                node.arguments.clear();
                for (auto& arg : tempNewExpr.arguments) {
                    node.arguments.push_back(std::move(arg));
                }
                return;
            }
            
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
                Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "الدالة '" + funcName + "' غير معرفة بعدد معاملات " + std::to_string(arguments.size()),
                "Function '" + funcName + "' not defined with " + std::to_string(arguments.size()) + " parameters"
            );
            lastResult_ = Value();
            return;
        }
    }
    
    // (AR) التحقق من وجود تنفيذ أصلي (دالة مضمنة) / (EN) Check for native implementation (built-in function)
    if (func->hasNativeImplementation()) {
        // (AR) تحويل القيم إلى ValuePtr / (EN) Convert values to ValuePtr
        std::vector<std::shared_ptr<Data::Value>> valuePtrs;
        for (const auto& arg : arguments) {
            valuePtrs.push_back(std::make_shared<Data::Value>(arg));
        }
        
        // (AR) استدعاء التنفيذ الأصلي / (EN) Call native implementation
        auto resultPtr = func->callNative(valuePtrs);
        
        if (resultPtr) {
            lastResult_ = *resultPtr;
        } else {
            lastResult_ = Data::Value();  // void return
        }
        
        return;
    }
    
    // (AR) التحقق من وجود جسم للدالة / (EN) Check if function has body
    if (!func->hasBody()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "الدالة '" + funcName + "' ليس لها جسم",
            "Function '" + funcName + "' has no body"
        );
        lastResult_ = Value();
        return;
    }
    
    // (AR) إنشاء نطاق جديد للدالة / (EN) Create new scope for function
    variableManager_.enterScope(Data::ScopeType::FUNCTION, funcName);
    
    // (AR) الحصول على المعاملات / (EN) Get parameters
    const auto& params = func->getParameters();
    
    // (AR) تقييم القيم الافتراضية للمعاملات المفقودة
    // (EN) Evaluate default values for missing arguments
    
    // (AR) محاولة الحصول على FunctionDecl الأصلي للوصول للـ Parameters
    // (EN) Try to get original FunctionDecl to access Parameters
    auto funcDeclNode = func->getFunctionDecl();
    FunctionDecl* astFuncDecl = nullptr;
    
    if (funcDeclNode) {
        // Reinterpret as FunctionDecl (we know it's the original FunctionDecl node)
        astFuncDecl = reinterpret_cast<FunctionDecl*>(funcDeclNode.get());
    }
    
    while (arguments.size() < params.size()) {
        size_t i = arguments.size();
        const auto& param = params[i];
        
        if (!param.hasDefaultValue) {
            variableManager_.exitScope();
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_WRONG_ARG_COUNT,
                Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "معامل إلزامي مفقود: " + param.name,
                "Required parameter missing: " + param.name
            );
            lastResult_ = Value();
            return;
        }
        
        Data::Value defaultVal;
        
        // (AR) إذا كان لدينا FunctionDecl، نقيّم ExprPtr من parameters
        // (EN) If we have FunctionDecl, evaluate ExprPtr from parameters
        if (astFuncDecl && i < astFuncDecl->parameters.size()) {
            const auto& astParam = astFuncDecl->parameters[i];
            
            if (astParam.defaultValue) {
                // (AR) تقييم التعبير الافتراضي
                // (EN) Evaluate default expression
                astParam.defaultValue->accept(*this);
                defaultVal = lastResult_;
            } else {
                // (AR) لا توجد قيمة افتراضية في AST (خطأ)
                // (EN) No default value in AST (error)
                variableManager_.exitScope();
                throw RuntimeError(
                    "(AR) معامل إلزامي مفقود: " + param.name + 
                    " / (EN) Required parameter missing: " + param.name
                );
            }
        } else {
            // (AR) Fallback: تحويل القيمة الافتراضية من string إلى Value
            // (EN) Fallback: Convert default value from string to Value
            const std::string& defaultStr = param.defaultValue;
            
            // (AR) محاولة تحويل النص إلى قيمة
            // (EN) Try to convert string to value
            if (defaultStr.empty()) {
                defaultVal = Data::Value();  // null
            } else if (defaultStr == "true" || defaultStr == "صحيح") {
                defaultVal = Data::Value(true);
            } else if (defaultStr == "false" || defaultStr == "خطأ") {
                defaultVal = Data::Value(false);
            } else if (defaultStr[0] == '"' || defaultStr[0] == '\'') {
                // String literal
                std::string strVal = defaultStr.substr(1, defaultStr.length() - 2);
                defaultVal = Data::Value(strVal);
            } else {
                // (AR) محاولة تحويل إلى رقم
                // (EN) Try to convert to number
                try {
                    if (defaultStr.find('.') != std::string::npos) {
                        defaultVal = Data::Value(std::stod(defaultStr));
                    } else {
                        defaultVal = Data::Value(std::stoi(defaultStr));
                    }
                } catch (...) {
                    // (AR) فشل التحويل - نستخدم النص كما هو
                    // (EN) Conversion failed - use string as is
                    defaultVal = Data::Value(defaultStr);
                }
            }
        }
        
        arguments.push_back(defaultVal);
    }
    
    // (AR) تعريف المعاملات كمتغيرات محلية / (EN) Define parameters as local variables
    for (size_t i = 0; i < params.size(); ++i) {
        variableManager_.define(params[i].name, arguments[i]);
    }
    
    // (AR) تنفيذ جسم الدالة / (EN) Execute function body
    auto bodyNode = func->getBody();
    
    // (AR) Lambda body هو Expression، دوال عادية لها Statement body
    // (EN) Lambda body is Expression, regular functions have Statement body
    // We check by trying to cast to Statement first (regular functions)
    auto bodyStmt = dynamic_cast<AST::Statement*>(
        reinterpret_cast<AST::ASTNode*>(bodyNode.get())
    );
    
    if (bodyStmt) {
        // (AR) دالة عادية - نستخدم StatementExecutor / (EN) Regular function - use StatementExecutor
        lastResult_ = statementExecutor_.executeFunctionBody(*bodyStmt);
    } else {
        // (AR) هذه دالة Lambda - نقيّم التعبير مباشرةً / (EN) This is Lambda - evaluate expression directly
        auto bodyExpr = reinterpret_cast<AST::Expression*>(
            reinterpret_cast<AST::ASTNode*>(bodyNode.get())
        );
        
        if (!bodyExpr) {
            variableManager_.exitScope();
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
                Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                "جسم الدالة فارغ",
                "Function body is null"
            );
            lastResult_ = Value();
            return;
        }
        
        bodyExpr->accept(*this);
        // lastResult_ already contains the result
    }
    
    // (AR) الخروج من نطاق الدالة / (EN) Exit function scope
    variableManager_.exitScope();
}

// =========================================================================
// (AR) تقييم إنشاء كائن جديد / (EN) New Object Creation Evaluation
// =========================================================================

void ExpressionEvaluator::visitNewExpr(NewExpr& node) {
    #ifdef DEBUG_OOP
    std::cout << "[OOP] تنفيذ تعبير جديد: " << node.className << "\n";
#endif
    
    // (AR) دعم أصناف القوالب: جديد صنف<نوع>(معاملات)
    // (EN) Template class support: new Class<Type>(args)
    std::string effectiveClassName = node.className;
    if (!node.templateArguments.empty()) {
        // (AR) هذا إنشاء صنف قالب - نحتاج إنشاء نسخة ملموسة
        // (EN) This is a template class instantiation - need to create concrete version
        auto* templateDecl = statementExecutor_.getTemplateClass(node.className);
        if (templateDecl) {
            // (AR) إنشاء النسخة الملموسة تحت الاسم الأصلي (بدون تغيير)
            // (EN) Instantiate concrete version under original name (unchanged)
            // لأن المفسر ديناميكي النوع، النسخة واحدة تكفي لكل الأنواع
            statementExecutor_.instantiateTemplateClass(*templateDecl, node.className);
            effectiveClassName = node.className;
        } else {
            // (AR) ربما الصنف مسجّل بالفعل (من تنفيذ سابق)
            // (EN) Perhaps the class is already registered (from previous execution)
            // نتابع بالاسم الأصلي
        }
    }
    
    // الحصول على ClassType من ClassManager
    auto* classManager = Data::ClassManager::getInstance();
    ClassType* classType = classManager->getClass(effectiveClassName);
    
    if (!classType) {
        // (AR) آخر محاولة: هل هو صنف قالب ولم نحدد الأنواع؟
        // (EN) Last attempt: is it a template class without type args?
        auto* templateDecl = statementExecutor_.getTemplateClass(node.className);
        if (templateDecl) {
            statementExecutor_.instantiateTemplateClass(*templateDecl, node.className);
            classType = classManager->getClass(node.className);
        }
        
        if (!classType) {
            std::string errMsg = "(AR) الصنف '" + effectiveClassName + "' غير موجود. ";
            errMsg += "(EN) Class '" + effectiveClassName + "' not found.";
            throw RuntimeError(errMsg, node.position);
        }
    }
    
    // إنشاء كائن كـ MAP مؤقتًا (حتى يتم توسيع نظام Value)
    // Create object as MAP temporarily (until Value system is extended)
    Value::MapType objectFields;
    
    // تهيئة الحقول بقيم افتراضية (بما في ذلك الحقول الموروثة)
    // Initialize fields with default values (including inherited fields)
    std::vector<ClassField> allFields;
    
    // جمع جميع الحقول من السلسلة الهرمية
    // Collect all fields from the class hierarchy
    ClassType* currentClass = classType;
    while (currentClass) {
        // إضافة حقول الصنف الحالي
        for (const auto& field : currentClass->fields) {
            allFields.push_back(field);
        }
        // الانتقال للصنف الأب
        currentClass = currentClass->getBaseClass();
    }
    
    // تهيئة جميع الحقول
    for (const auto& field : allFields) {
        // تخطي الحقول الثابتة - يتم تخزينها في ClassType وليس في الكائن
        // Skip static fields - they are stored in ClassType not in object
        if (field.isStatic) {
            continue;
        }
        
        Value defaultValue;  // null/none by default
        objectFields[field.name] = defaultValue;
    }
    
    // إضافة معلومة اسم الصنف
    objectFields["__class__"] = Value(node.className);
    
    #ifdef DEBUG_OOP
    
    std::cout << "[OOP] تم إنشاء كائن من صنف: " << node.className << "\n";
#endif
    #ifdef DEBUG_OOP
    std::cout << "[OOP] عدد الحقول: " << allFields.size() << " (بما في ذلك الموروثة)\n";
#endif
    
    // استدعاء الباني إذا كان موجودًا
    // (AR) نبحث أولاً في ClassType، ثم في مصدر القالب
    // (EN) First check ClassType, then template instance source
    AST::ConstructorDecl* constructor = nullptr;
    if (classType->constructor) {
        constructor = classType->constructor.get();
    } else {
        // (AR) البحث في مصادر نسخ القوالب
        // (EN) Look up constructor from template instance source
        auto* templateSrc = statementExecutor_.getTemplateInstanceSource(effectiveClassName);
        if (templateSrc) {
            for (auto& member : templateSrc->members) {
                if (auto* ctorDecl = dynamic_cast<AST::ConstructorDecl*>(member.get())) {
                    constructor = ctorDecl;
                    break;
                }
            }
        }
    }
    
    if (constructor) {
        // التحقق من عدد المعاملات
        if (node.arguments.size() != constructor->parameters.size()) {
            std::string errMsg = "(AR) عدد المعاملات غير متطابق. توقع " + 
                std::to_string(constructor->parameters.size()) + " لكن حصل على " + 
                std::to_string(node.arguments.size()) + ". ";
            errMsg += "(EN) Argument count mismatch. Expected " + 
                std::to_string(constructor->parameters.size()) + " but got " + 
                std::to_string(node.arguments.size()) + ".";
            throw RuntimeError(errMsg, node.position);
        }
        
        // تقييم المعاملات
        std::vector<Value> argValues;
        for (auto& arg : node.arguments) {
            arg->accept(*this);
            argValues.push_back(lastResult_);
        }
        
        // إنشاء scope جديد للباني
        variableManager_.enterScope(Data::ScopeType::FUNCTION, "constructor");
        
        // ربط المعاملات بالقيم
        for (size_t i = 0; i < constructor->parameters.size(); ++i) {
            variableManager_.define(constructor->parameters[i].name, argValues[i]);
        }
        
        // إضافة حقول الكائن للـ scope (محاكاة 'this')
        // (AR) ربط 'هذا' بالكائن الحالي / (EN) Bind 'this' to current object
        Value objectValue(objectFields);
        variableManager_.define("هذا", objectValue);
        variableManager_.define("this", objectValue);
        
        for (const auto& [name, value] : objectFields) {
            if (name != "__class__") {
                variableManager_.define(name, value);
            }
        }
        
        // إضافة الحقول الثابتة للـ scope أيضًا
        // Add static fields to scope as well
        for (const auto& field : classType->fields) {
            if (field.isStatic) {
                Value* staticValue = classType->getStaticField(field.name);
                if (staticValue) {
                    variableManager_.define(field.name, *staticValue);
                }
            }
        }
        
        // === استدعاء باني الأب (أساس) إذا كانت هناك معاملات ===
        // === Call base constructor (super) if superArgs exist ===
        if (!constructor->superArgs.empty() && classType->getBaseClass()) {
            ClassType* baseClass = classType->getBaseClass();
            AST::ConstructorDecl* baseCtor = nullptr;
            if (baseClass->constructor) {
                baseCtor = baseClass->constructor.get();
            }
            if (baseCtor) {
                // تقييم معاملات الأساس
                std::vector<Value> superArgValues;
                for (auto& sarg : constructor->superArgs) {
                    sarg->accept(*this);
                    superArgValues.push_back(lastResult_);
                }
                // ربط معاملات باني الأب بالقيم في الـ scope الحالي
                if (superArgValues.size() == baseCtor->parameters.size()) {
                    for (size_t si = 0; si < baseCtor->parameters.size(); ++si) {
                        const auto& pname = baseCtor->parameters[si].name;
                        if (variableManager_.exists(pname)) {
                            variableManager_.assign(pname, superArgValues[si]);
                        } else {
                            variableManager_.define(pname, superArgValues[si]);
                        }
                    }
                    // تنفيذ جسم باني الأب
                    try {
                        baseCtor->body->accept(statementExecutor_);
                    } catch (...) {
                        // تجاهل الأخطاء في باني الأب مبدئياً
                    }
                    // قراءة القيم المحدثة من باني الأب
                    for (const auto& bfield : allFields) {
                        if (!bfield.isStatic) {
                            try {
                                Value bval = variableManager_.get(bfield.name);
                                objectFields[bfield.name] = bval;
                            } catch (...) {}
                        }
                    }
                }
            }
        }

        // تنفيذ جسم الباني
        try {
            constructor->body->accept(statementExecutor_);
            
            // جمع القيم المحدثة من الـ scope (بما في ذلك الحقول الموروثة)
            // Collect updated values from scope (including inherited fields)
            for (const auto& field : allFields) {
                try {
                    Value updatedValue = variableManager_.get(field.name);
                    if (field.isStatic) {
                        // تحديث الحقل الثابت في ClassType الصحيح
                        ClassType* fc = classType;
                        while (fc) {
                            for (const auto& ff : fc->fields) {
                                if (ff.name == field.name && ff.isStatic) {
                                    fc->setStaticField(field.name, updatedValue);
                                    break;
                                }
                            }
                            fc = fc->getBaseClass();
                        }
                    } else {
                        // تحديث حقل الكائن
                        objectFields[field.name] = updatedValue;
                    }
                } catch (...) {
                    // الحقل لم يتم تعيينه في الباني، استخدام القيمة الافتراضية
                }
            }
        } catch (const std::exception&) {
            variableManager_.exitScope();
            throw;
        }
        
        variableManager_.exitScope();
    }
    
    // إرجاع الكائن كـ MAP
    lastResult_ = Value(objectFields);
}

// =========================================================================
// (AR) استدعاء طريقة / (EN) Method Call
// =========================================================================

void ExpressionEvaluator::visitMethodCallExpr(MethodCallExpr& node) {
    #ifdef DEBUG_OOP
    std::cout << "[OOP] استدعاء طريقة: " << node.methodName << "\n";
#endif
    
    // تقييم الكائن
    node.object->accept(*this);
    Value objectValue = lastResult_;
    
    auto* classManager = Data::ClassManager::getInstance();
    std::string className;
    ClassType* classType = nullptr;
    Value::MapType fields;
    bool isStaticCall = false;
    
    // التحقق من الاستدعاء الثابت: ClassName.staticMethod()
    // Check for static call: ClassName.staticMethod()
    if (objectValue.isString()) {
        std::string possibleClassName = objectValue.toString();
        classType = classManager->getClass(possibleClassName);
        
        if (classType) {
            // هذا استدعاء ثابت: ClassName.staticMethod()
            // This is static call: ClassName.staticMethod()
            #ifdef DEBUG_OOP
            std::cout << "[OOP] استدعاء طريقة ثابتة: " << possibleClassName << "." << node.methodName << "\n";
#endif
            className = possibleClassName;
            isStaticCall = true;
        }
    }
    
    // إذا لم يكن استدعاء ثابت، فهو استدعاء عادي على كائن
    // If not static call, it's regular call on object
    if (!isStaticCall) {
        // التحقق من أن القيمة كائن
        if (!objectValue.isMap()) {
            std::string errMsg = "(AR) لا يمكن استدعاء طريقة على قيمة ليست كائن. ";
            errMsg += "(EN) Cannot call method on non-object value.";
            throw RuntimeError(errMsg, node.position);
        }
        
        // الحصول على اسم الصنف من الكائن
        fields = objectValue.toMap();
        auto classNameIt = fields.find("__class__");
        if (classNameIt == fields.end()) {
            throw RuntimeError("(AR) كائن بدون معلومات صنف. (EN) Object without class info.", node.position);
        }
        
        className = classNameIt->second.toString();
        
        // الحصول على ClassType
        classType = classManager->getClass(className);
    }
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.", node.position);
    }
    
    // البحث عن الطريقة (في السلسلة الهرمية)
    ClassMethod* method = classType->findMethod(node.methodName);
    if (!method) {
        std::string errMsg = "(AR) الطريقة '" + node.methodName + "' غير موجودة في الصنف '" + className + "'. ";
        errMsg += "(EN) Method '" + node.methodName + "' not found in class '" + className + "'.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // التحقق من التطابق بين نوع الاستدعاء ونوع الطريقة
    // Verify call type matches method type
    if (isStaticCall && !method->isStatic) {
        std::string errMsg = "(AR) لا يمكن استدعاء طريقة غير ثابتة '" + node.methodName + "' من خلال اسم الصنف. ";
        errMsg += "(EN) Cannot call non-static method '" + node.methodName + "' through class name.";
        throw RuntimeError(errMsg, node.position);
    }
    if (!isStaticCall && method->isStatic) {
        std::string errMsg = "(AR) يجب استدعاء الطريقة الثابتة '" + node.methodName + "' من خلال اسم الصنف. ";
        errMsg += "(EN) Static method '" + node.methodName + "' should be called through class name.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // فحص الوصول (Phase 6.1: Access Modifiers)
    checkMemberAccess(method->visibility, node.methodName, classType);
    
    // التحقق من عدد المعاملات
    if (node.arguments.size() != method->parameters.size()) {
        std::string errMsg = "(AR) عدد المعاملات غير متطابق. توقع " + 
            std::to_string(method->parameters.size()) + " لكن حصل على " + 
            std::to_string(node.arguments.size()) + ". ";
        errMsg += "(EN) Argument count mismatch.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // تقييم المعاملات
    std::vector<Value> argValues;
    for (auto& arg : node.arguments) {
        arg->accept(*this);
        argValues.push_back(lastResult_);
    }
    
    // إنشاء scope جديد للطريقة
    variableManager_.enterScope(Data::ScopeType::FUNCTION, node.methodName);
    
    // ربط المعاملات بالقيم
    for (size_t i = 0; i < method->parameters.size(); ++i) {
        variableManager_.define(method->parameters[i].name, argValues[i]);
    }
    
    // إضافة حقول الكائن للـ scope (محاكاة 'this') - فقط للطرق غير الثابتة
    // Add object fields to scope (simulate 'this') - only for non-static methods
    if (!isStaticCall) {
        // إضافة 'this' reference للكائن الحالي
        // Add 'this' reference to current object
        variableManager_.define("هذا", objectValue);
        variableManager_.define("this", objectValue);
        
        for (const auto& [name, value] : fields) {
            if (name != "__class__") {
                variableManager_.define(name, value);
            }
        }
    }
    
    // إضافة الحقول الثابتة للـ scope (متاحة لكل الطرق)
    // Add static fields to scope (available to all methods)
    for (const auto& field : classType->fields) {
        if (field.isStatic) {
            Value* staticValue = classType->getStaticField(field.name);
            if (staticValue) {
                variableManager_.define(field.name, *staticValue);
            }
        }
    }
    
    // تنفيذ جسم الطريقة
    // (AR) نبحث عن الجسم في ClassType أولاً، ثم في مصدر القالب
    // (EN) Look for body in ClassType first, then in template instance source
    AST::Statement* methodBody = nullptr;
    if (method->body) {
        methodBody = method->body.get();
    } else {
        // (AR) البحث في مصدر القالب
        // (EN) Look up method body from template instance source
        auto* templateSrc = statementExecutor_.getTemplateInstanceSource(className);
        if (templateSrc) {
            for (auto& member : templateSrc->members) {
                if (auto* methodDecl = dynamic_cast<AST::MethodDecl*>(member.get())) {
                    if (methodDecl->name == node.methodName && methodDecl->body) {
                        methodBody = methodDecl->body.get();
                        break;
                    }
                }
            }
        }
    }
    
    Value returnValue;
    try {
        if (methodBody) {
            methodBody->accept(statementExecutor_);
            
            // التحقق من وجود return
            if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
                returnValue = statementExecutor_.getReturnValue();
                statementExecutor_.resetFlowControl();
            }
            
            // جمع القيم المحدثة للحقول (في حالة تم تعديلها)
            // Collect updated field values (if modified)
            // يجب جمع الحقول من السلسلة الهرمية الكاملة
            ClassType* currentClass = classType;
            while (currentClass) {
                for (const auto& field : currentClass->fields) {
                    try {
                        Value updatedValue = variableManager_.get(field.name);
                        if (field.isStatic) {
                            // تحديث الحقل الثابت في ClassType
                            currentClass->setStaticField(field.name, updatedValue);
                        } else if (!isStaticCall) {
                            // تحديث حقل الكائن - فقط للطرق غير الثابتة
                            fields[field.name] = updatedValue;
                        }
                    } catch (...) {
                        // الحقل لم يتم تعديله
                    }
                }
                currentClass = currentClass->getBaseClass();
            }
            
            // تحديث الكائن الأصلي إذا كان متغيراً - فقط للطرق غير الثابتة
            // Update original object if it's a variable - only for non-static methods
            if (!isStaticCall) {
                if (auto* varExpr = dynamic_cast<VariableExpr*>(node.object.get())) {
                    Value modifiedObject(fields);
                    variableManager_.assign(varExpr->name, modifiedObject);
                }
            }
        }
    } catch (const std::exception&) {
        variableManager_.exitScope();
        throw;
    }
    
    variableManager_.exitScope();
    
    #ifdef DEBUG_OOP
    
    std::cout << "[OOP] ✅ تم تنفيذ الطريقة: " << node.methodName << "\n";
#endif
    lastResult_ = returnValue;
}

// =========================================================================
// (AR) تقييم الوصول للعضو / (EN) Member Access Evaluation
// =========================================================================

void ExpressionEvaluator::visitMemberExpr(MemberExpr& node) {
    // تقييم الكائن
    node.object->accept(*this);
    Value objectValue = lastResult_;
    
    auto* classManager = Data::ClassManager::getInstance();
    
    // التحقق من الوصول الثابت: ClassName.staticField
    // Check for static access: ClassName.staticField
    if (objectValue.isString()) {
        std::string possibleClassName = objectValue.toString();
        ClassType* classType = classManager->getClass(possibleClassName);
        
        if (classType) {
            // هذا وصول ثابت: ClassName.staticField
            // This is static access: ClassName.staticField
            #ifdef DEBUG_OOP
            std::cout << "[OOP] الوصول لحقل ثابت: " << possibleClassName << "." << node.member << "\n";
#endif
            
            // البحث عن الحقل
            ClassField* field = classType->findField(node.member);
            if (!field) {
                std::string errMsg = "(AR) الحقل '" + node.member + "' غير موجود في الصنف '" + possibleClassName + "'. ";
                errMsg += "(EN) Field '" + node.member + "' not found in class '" + possibleClassName + "'.";
                throw RuntimeError(errMsg, node.position);
            }
            
            // التحقق من أن الحقل ثابت
            if (!field->isStatic) {
                std::string errMsg = "(AR) لا يمكن الوصول للحقل غير الثابت '" + node.member + "' من خلال اسم الصنف. ";
                errMsg += "(EN) Cannot access non-static field '" + node.member + "' through class name.";
                throw RuntimeError(errMsg, node.position);
            }
            
            // فحص الوصول
            checkMemberAccess(field->visibility, node.member, classType);
            
            // الحصول على قيمة الحقل الثابت
            Value* staticValue = classType->getStaticField(node.member);
            if (!staticValue) {
                std::string errMsg = "(AR) الحقل الثابت '" + node.member + "' غير مهيأ. ";
                errMsg += "(EN) Static field '" + node.member + "' not initialized.";
                throw RuntimeError(errMsg, node.position);
            }
            
            lastResult_ = *staticValue;
            #ifdef DEBUG_OOP
            std::cout << "[OOP] قيمة الحقل الثابت: " << lastResult_.toString() << "\n";
#endif
            return;
        }
    }
    
    // وصول عادي للكائن: object.field
    // Regular object access: object.field
    
    // التحقق من أن القيمة كائن (MAP مؤقتًا)
    if (!objectValue.isMap()) {
        std::string errMsg = "(AR) لا يمكن الوصول لعضو من قيمة ليست كائن. ";
        errMsg += "(EN) Cannot access member of non-object value.";
        throw RuntimeError(errMsg, node.position);
    }
    
    #ifdef DEBUG_OOP
    
    std::cout << "[OOP] الوصول لحقل: " << node.member << "\n";
#endif
    
    // الحصول على MAP
    Value::MapType fields = objectValue.toMap();
    
    // الحصول على اسم الصنف
    auto classNameIt = fields.find("__class__");
    if (classNameIt == fields.end()) {
        throw RuntimeError("(AR) كائن بدون معلومات صنف. (EN) Object without class info.", node.position);
    }
    
    std::string className = classNameIt->second.toString();
    
    // الحصول على ClassType
    ClassType* classType = classManager->getClass(className);
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.", node.position);
    }
    
    // البحث عن الحقل في السلسلة الهرمية
    ClassField* field = classType->findField(node.member);
    
    // البحث عن خاصية (Property) إذا لم يُوجد حقل
    ClassProperty* property = nullptr;
    if (!field) {
        property = classType->findProperty(node.member);
    }
    
    if (!field && !property) {
        std::string errMsg = "(AR) الحقل أو الخاصية '" + node.member + "' غير موجود في الكائن. ";
        errMsg += "(EN) Field or property '" + node.member + "' not found in object.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // إذا كانت خاصية، نفذ الـ getter
    if (property) {
        #ifdef DEBUG_OOP
        std::cout << "[OOP] تنفيذ getter للخاصية: " << node.member << "\n";
#endif
        
        // فحص الوصول
        checkMemberAccess(property->visibility, node.member, classType);
        
        // التحقق من وجود getter
        if (!property->getterBody) {
            std::string errMsg = "(AR) الخاصية '" + node.member + "' للكتابة فقط (لا يوجد getter). ";
            errMsg += "(EN) Property '" + node.member + "' is write-only (no getter).";
            throw RuntimeError(errMsg, node.position);
        }
        
        // تنفيذ getter body في نطاق جديد
        // TODO: Add proper 'this' context support in future
        try {
            property->getterBody->accept(*this);
        } catch (...) {
            // في حالة حدوث خطأ، نعيد القيمة الحالية
        }
        
        #ifdef DEBUG_OOP
        
        std::cout << "[OOP] قيمة الخاصية: " << lastResult_.toString() << "\n";
#endif
        return;
    }
    
    // معالجة الحقل العادي
    // فحص الوصول (Phase 6.1: Access Modifiers)
    checkMemberAccess(field->visibility, node.member, classType);
    
    // البحث عن قيمة الحقل
    auto it = fields.find(node.member);
    if (it == fields.end()) {
        std::string errMsg = "(AR) الحقل '" + node.member + "' غير موجود في الكائن. ";
        errMsg += "(EN) Field '" + node.member + "' not found in object.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // إرجاع قيمة الحقل
    lastResult_ = it->second;
    #ifdef DEBUG_OOP
    std::cout << "[OOP] قيمة الحقل: " << lastResult_.toString() << "\n";
#endif
}

// =========================================================================
// (AR) تعيين قيمة لعضو / (EN) Member Assignment
// =========================================================================

void ExpressionEvaluator::visitMemberAssignExpr(MemberAssignExpr& node) {
    // تقييم الكائن
    node.object->accept(*this);
    Value objectValue = lastResult_;
    
    // التحقق من أن القيمة كائن (MAP مؤقتًا)
    if (!objectValue.isMap()) {
        std::string errMsg = "(AR) لا يمكن تعيين قيمة لعضو من قيمة ليست كائن. ";
        errMsg += "(EN) Cannot assign to member of non-object value.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // تقييم القيمة الجديدة
    node.value->accept(*this);
    Value newValue = lastResult_;
    
    // الحصول على MAP
    Value::MapType fields = objectValue.toMap();
    
    // الحصول على اسم الصنف
    auto classNameIt = fields.find("__class__");
    if (classNameIt == fields.end()) {
        throw RuntimeError("(AR) كائن بدون معلومات صنف. (EN) Object without class info.", node.position);
    }
    
    std::string className = classNameIt->second.toString();
    
    // الحصول على ClassType
    auto* classManager = Data::ClassManager::getInstance();
    ClassType* classType = classManager->getClass(className);
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.", node.position);
    }
    
    // البحث عن الحقل
    ClassField* field = classType->findField(node.member);
    
    // البحث عن خاصية (Property) إذا لم يُوجد حقل
    ClassProperty* property = nullptr;
    if (!field) {
        property = classType->findProperty(node.member);
    }
    
    if (!field && !property) {
        std::string errMsg = "(AR) الحقل أو الخاصية '" + node.member + "' غير موجود في الكائن. ";
        errMsg += "(EN) Field or property '" + node.member + "' not found in object.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // إذا كانت خاصية، نفذ الـ setter
    if (property) {
        #ifdef DEBUG_OOP
        std::cout << "[OOP] تنفيذ setter للخاصية: " << node.member << "\n";
#endif
        
        // فحص الوصول
        checkMemberAccess(property->visibility, node.member, classType);
        
        // التحقق من وجود setter
        if (!property->setterBody) {
            std::string errMsg = "(AR) الخاصية '" + node.member + "' للقراءة فقط (لا يوجد setter). ";
            errMsg += "(EN) Property '" + node.member + "' is read-only (no setter).";
            throw RuntimeError(errMsg, node.position);
        }
        
        // إنشاء نطاق جديد لمعامل setter
        // TODO: Add proper scope and 'this' context support
        try {
            // تنفيذ setter body
            property->setterBody->accept(*this);
        } catch (...) {
            // معالجة الأخطاء
        }
        
        lastResult_ = newValue;
        #ifdef DEBUG_OOP
        std::cout << "[OOP] تم تعيين قيمة الخاصية: " << newValue.toString() << "\n";
#endif
        return;
    }
    
    // معالجة الحقل العادي
    // فحص الوصول (Phase 6.1: Access Modifiers)
    checkMemberAccess(field->visibility, node.member, classType);
    
    // التحقق من وجود الحقل في الكائن
    if (fields.find(node.member) == fields.end()) {
        std::string errMsg = "(AR) الحقل '" + node.member + "' غير موجود في الكائن. ";
        errMsg += "(EN) Field '" + node.member + "' not found in object.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // تحديث قيمة الحقل
    fields[node.member] = newValue;
    
    // حفظ الكائن المعدّل
    // PROBLEM: We need to update the original variable!
    // This is where we hit a limitation - we need to know which variable holds the object
    // For now, we need to handle this differently
    
    // The object came from evaluating node.object, which is likely a VariableExpr
    // We need to update that variable with the modified MAP
    if (auto* varExpr = dynamic_cast<VariableExpr*>(node.object.get())) {
        // Update the variable with the modified object
        Value modifiedObject(fields);
        variableManager_.assign(varExpr->name, modifiedObject);
        lastResult_ = newValue;
    } else {
        // Complex expression - not supported yet
        std::string errMsg = "(AR) تعيين قيمة لحقل في تعبير معقد غير مدعوم حاليًا. ";
        errMsg += "(EN) Assignment to field in complex expression not yet supported.";
        throw RuntimeError(errMsg, node.position);
    }
}

// =========================================================================
// (AR) تقييم تعبير Walrus / (EN) Walrus Expression Evaluation
// =========================================================================

/**
 * @brief (AR) تقييم عامل Walrus - التعيين داخل التعبير
 *        (EN) Evaluate walrus operator - assignment within expression
 * 
 * @param node (AR) عقدة تعبير Walrus / (EN) Walrus expression node
 * 
 * @details
 *   (AR) عامل Walrus (:=) يسمح بالتعيين داخل التعبير ويُرجع القيمة المُعيّنة
 *   (EN) Walrus operator (:=) allows assignment within expression and returns assigned value
 * 
 *   Examples / أمثلة:
 *   - if (n := len(items)) > 10: print(n)
 *   - while (line := file.read()): process(line)
 */
void ExpressionEvaluator::visitWalrusExpr(WalrusExpr& node) {
    // (AR) تقييم القيمة المراد تعيينها / (EN) Evaluate the value to assign
    node.value->accept(*this);
    Value assignedValue = lastResult_;
    
    // (AR) محاولة تعيين القيمة للمتغير
    // (EN) Try to assign value to variable
    try {
        variableManager_.assign(node.variable, assignedValue);
    } catch (...) {
        // If variable doesn't exist, define it / إذا لم يكن المتغير موجوداً، نُعرّفه
        variableManager_.define(node.variable, assignedValue);
    }
    
    // (AR) إرجاع القيمة المُعيّنة (هذا هو سلوك Walrus)
    // (EN) Return the assigned value (this is walrus behavior)
    lastResult_ = assignedValue;
}

// =========================================================================
// (AR) تقييم دالة Lambda / (EN) Lambda Function Evaluation
// =========================================================================

void ExpressionEvaluator::visitLambdaExpr(LambdaExpr& node) {
    // (AR) إنشاء اسم فريد للدالة Lambda / (EN) Generate unique name for lambda function
    static int lambdaCounter = 0;
    std::string lambdaName = "__lambda_" + std::to_string(lambdaCounter++);
    
    // (AR) تحويل المعاملات من AST Parameter إلى FunctionParameter
    // (EN) Convert parameters from AST Parameter to FunctionParameter
    auto dataTypeToString = [](DataType type) -> std::string {
        switch (type) {
            case DataType::INTEGER: return "integer";
            case DataType::FLOAT: return "float";
            case DataType::STRING: return "string";
            case DataType::BOOLEAN: return "boolean";
            case DataType::NONE: return "none";
            case DataType::ARRAY: return "array";
            case DataType::MAP: return "map";
            case DataType::FUNCTION: return "function";
            case DataType::OBJECT: return "object";
            default: return "unknown";
        }
    };
    
    std::vector<Data::FunctionParameter> params;
    for (const auto& param : node.parameters) {
        params.push_back(Data::FunctionParameter(
            param.name,
            dataTypeToString(param.type)
        ));
    }
    
    // (AR) تحويل جسم Lambda إلى ASTNode
    // (EN) Convert lambda body to ASTNode
    // Lambda body is an Expression (ExprPtr), we need to cast it
    std::shared_ptr<Parser::ASTNode> bodyNode(
        reinterpret_cast<Parser::ASTNode*>(node.body.get()),
        [](Parser::ASTNode*) {}  // Empty deleter - AST owns the memory
    );
    
    // (AR) تسجيل Lambda كدالة في FunctionManager
    // (EN) Register lambda as function in FunctionManager
    functionManager_.defineFunction(lambdaName, params, bodyNode);
    
    // (AR) إرجاع اسم الدالة كقيمة String
    // (EN) Return function name as String value
    // Note: في المستقبل، يمكن إضافة نوع FUNCTION إلى Value
    // Note: In future, we can add FUNCTION type to Value
    lastResult_ = Value(lambdaName);
}

// =========================================================================
// (AR) تقييم استيعاب قائمي / (EN) List Comprehension Evaluation
// =========================================================================

void ExpressionEvaluator::visitListComprehensionExpr(ListComprehensionExpr& node) {
    // (AR) تقييم iterable / (EN) Evaluate iterable
    node.iterable->accept(*this);
    Value iterableValue = lastResult_;
    
    if (!iterableValue.isArray()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "الاستيعاب القائمي يتطلب مصفوفة",
            "List comprehension requires an array"
        );
        lastResult_ = Value(std::vector<Value>());
        return;
    }
    
    // (AR) إنشاء مصفوفة النتيجة / (EN) Create result array
    std::vector<Value> result;
    
    // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
    variableManager_.enterScope(Data::ScopeType::BLOCK, "list_comprehension");
    
    // (AR) المرور على كل عنصر / (EN) Iterate over each element
    for (size_t i = 0; i < iterableValue.size(); ++i) {
        Value item = iterableValue[i];
        
        // (AR) تحديث متغير الحلقة / (EN) Update loop variable
        if (i == 0) {
            // أول iteration - نعرّف المتغير
            variableManager_.define(node.variable, item);
        } else {
            // iterations تالية - نحدّث باستخدام assign
            variableManager_.assign(node.variable, item);
        }
        
        // (AR) التحقق من الشرط إن وُجد / (EN) Check condition if exists
        bool includeItem = true;
        if (node.condition) {
            node.condition->accept(*this);
            includeItem = lastResult_.toBool();
        }
        
        // (AR) تقييم التعبير وإضافة النتيجة / (EN) Evaluate element expression and add result
        if (includeItem) {
            node.element->accept(*this);
            result.push_back(lastResult_);
        }
    }
    
    // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
    variableManager_.exitScope();
    
    // (AR) إرجاع المصفوفة الناتجة / (EN) Return result array
    lastResult_ = Value(result);
}

// =========================================================================
// (AR) تقييم استيعاب قاموسي / (EN) Dict Comprehension Evaluation
// =========================================================================

void ExpressionEvaluator::visitDictComprehensionExpr(DictComprehensionExpr& node) {
    // (AR) تقييم iterable / (EN) Evaluate iterable
    node.iterable->accept(*this);
    Value iterableValue = lastResult_;
    
    if (!iterableValue.isArray()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "الاستيعاب القاموسي يتطلب مصفوفة",
            "Dictionary comprehension requires an array"
        );
        lastResult_ = Value(std::unordered_map<std::string, Value>());
        return;
    }
    
    // (AR) إنشاء قاموس النتيجة / (EN) Create result dictionary
    std::unordered_map<std::string, Value> result;
    
    // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
    variableManager_.enterScope(Data::ScopeType::BLOCK, "dict_comprehension");
    
    // (AR) المرور على كل عنصر / (EN) Iterate over each element
    for (size_t i = 0; i < iterableValue.size(); ++i) {
        Value item = iterableValue[i];
        
        // (AR) تحديث متغير الحلقة / (EN) Update loop variable
        if (i == 0) {
            // أول iteration - نعرّف المتغير
            variableManager_.define(node.variable, item);
        } else {
            // iterations تالية - نحدّث باستخدام assign
            variableManager_.assign(node.variable, item);
        }
        
        // (AR) التحقق من الشرط إن وُجد / (EN) Check condition if exists
        bool includeItem = true;
        if (node.condition) {
            node.condition->accept(*this);
            includeItem = lastResult_.toBool();
        }
        
        // (AR) تقييم Key و Value وإضافة النتيجة / (EN) Evaluate key & value expressions and add result
        if (includeItem) {
            // تقييم تعبير المفتاح / Evaluate key expression
            node.key->accept(*this);
            Value keyValue = lastResult_;
            
            // تقييم تعبير القيمة / Evaluate value expression
            node.value->accept(*this);
            Value valueValue = lastResult_;
            
            // تحويل المفتاح إلى نص / Convert key to string
            std::string keyStr = keyValue.toString();
            
            // إضافة إلى القاموس / Add to dictionary
            result[keyStr] = valueValue;
        }
    }
    
    // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
    variableManager_.exitScope();
    
    // (AR) إرجاع القاموس الناتج / (EN) Return result dictionary
    lastResult_ = Value(result);
}

// =========================================================================
// (AR) تقييم استيعاب مجموعة / (EN) Set Comprehension Evaluation
// =========================================================================

void ExpressionEvaluator::visitSetComprehensionExpr(SetComprehensionExpr& node) {
    // (AR) تقييم iterable / (EN) Evaluate iterable
    node.iterable->accept(*this);
    Value iterableValue = lastResult_;
    
    if (!iterableValue.isArray()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation("<input>", 0, 0),
            "الاستيعاب المجموعة يتطلب مصفوفة",
            "Set comprehension requires an array"
        );
        lastResult_ = Value(std::vector<Value>());
        return;
    }
    
    // (AR) إنشاء set النتيجة (نستخدم vector مع فحص التكرار) / (EN) Create result set (using vector with uniqueness check)
    std::vector<Value> result;
    
    // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
    variableManager_.enterScope(Data::ScopeType::BLOCK, "set_comprehension");
    
    // (AR) المرور على كل عنصر / (EN) Iterate over each element
    for (size_t i = 0; i < iterableValue.size(); ++i) {
        Value item = iterableValue[i];
        
        // (AR) تحديث متغير الحلقة / (EN) Update loop variable
        if (i == 0) {
            // أول iteration - نعرّف المتغير
            variableManager_.define(node.variable, item);
        } else {
            // iterations تالية - نحدّث باستخدام assign
            variableManager_.assign(node.variable, item);
        }
        
        // (AR) التحقق من الشرط إن وُجد / (EN) Check condition if exists
        bool includeItem = true;
        if (node.condition) {
            node.condition->accept(*this);
            includeItem = lastResult_.toBool();
        }
        
        // (AR) تقييم Expression وإضافة النتيجة (بدون تكرار) / (EN) Evaluate expression and add result (without duplicates)
        if (includeItem) {
            node.expression->accept(*this);
            Value itemValue = lastResult_;
            
            // (AR) فحص إذا كان العنصر موجود مسبقاً / (EN) Check if item already exists
            bool exists = false;
            for (const auto& existingItem : result) {
                // Simple equality check - compare toString() representations
                // (AR) فحص بسيط للمساواة - مقارنة التمثيلات النصية
                if (existingItem.toString() == itemValue.toString()) {
                    exists = true;
                    break;
                }
            }
            
            // (AR) إضافة العنصر إذا لم يكن موجوداً / (EN) Add item if not exists
            if (!exists) {
                result.push_back(itemValue);
            }
        }
    }
    
    // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
    variableManager_.exitScope();
    
    // (AR) إرجاع المجموعة الناتجة (كمصفوفة) / (EN) Return result set (as array)
    lastResult_ = Value(result);
}

// =========================================================================
// (AR) فحص الوصول للأعضاء / (EN) Member Access Check
// =========================================================================

void ExpressionEvaluator::checkMemberAccess(
    AST::Visibility visibility, 
    const std::string& memberName,
    Data::ClassType* targetClass) 
{
    // PUBLIC: دائماً متاح
    if (visibility == AST::Visibility::PUBLIC) {
        return;
    }
    
    // التحقق من السياق: هل نحن داخل method من الصنف نفسه أو صنف مشتق؟
    // Check context: are we inside a method from the same class or derived class?
    bool insideMethod = variableManager_.exists("هذا") || variableManager_.exists("this");
    
    if (insideMethod) {
        // نحن داخل method، نتحقق من الصنف الحالي
        Value thisValue = variableManager_.exists("هذا") ? 
                         variableManager_.get("هذا") : 
                         variableManager_.get("this");
        
        if (thisValue.isMap()) {
            auto fields = thisValue.toMap();
            auto classNameIt = fields.find("__class__");
            if (classNameIt != fields.end()) {
                std::string currentClassName = classNameIt->second.toString();
                auto* classManager = Data::ClassManager::getInstance();
                Data::ClassType* currentClass = classManager->getClass(currentClassName);
                
                if (currentClass) {
                    // التحقق إذا كان الصنف الحالي هو نفسه أو مشتق من targetClass
                    // Check if current class is same or derived from targetClass
                    Data::ClassType* temp = currentClass;
                    while (temp) {
                        if (temp == targetClass || temp->name == targetClass->name) {
                            // PRIVATE: متاح فقط في نفس الصنف
                            if (visibility == AST::Visibility::PRIVATE && temp == targetClass) {
                                return; // Same class - allow access
                            }
                            // PROTECTED: متاح في نفس الصنف أو الأصناف المشتقة
                            if (visibility == AST::Visibility::PROTECTED) {
                                return; // Same class or derived - allow access
                            }
                        }
                        temp = temp->getBaseClass();
                    }
                }
            }
        }
    }
    
    // الوصول مرفوض
    if (visibility == AST::Visibility::PRIVATE) {
        std::string errMsg = "(AR) لا يمكن الوصول للعضو الخاص '" + memberName + 
                           "' من خارج الصنف '" + targetClass->name + "'. ";
        errMsg += "(EN) Cannot access private member '" + memberName + 
                 "' from outside class '" + targetClass->name + "'.";
        throw RuntimeError(errMsg, Lexer::Position());
    }
    
    if (visibility == AST::Visibility::PROTECTED) {
        std::string errMsg = "(AR) لا يمكن الوصول للعضو المحمي '" + memberName + 
                           "' من خارج الصنف '" + targetClass->name + "' أو الأصناف المشتقة. ";
        errMsg += "(EN) Cannot access protected member '" + memberName + 
                 "' from outside class '" + targetClass->name + "' or derived classes.";
        throw RuntimeError(errMsg, Lexer::Position());
    }
}

// =========================================================================
// (AR) تقييم تعبير Await / (EN) Await Expression Evaluation
// =========================================================================

void ExpressionEvaluator::visitAwaitExpr(AwaitExpr& node) {
    // (AR) تقييم التعبير المُنتظَر / (EN) Evaluate awaited expression
    // (AR) في التنفيذ الحالي، await يعمل بشكل متزامن (synchronous)
    // (EN) In current implementation, await works synchronously
    // TODO Phase 1.4.4: Implement async runtime with Future/Promise
    
    node.expression->accept(*this);
    // (AR) النتيجة موجودة في lastResult_ / (EN) Result is in lastResult_
    // (AR) await يُرجع نفس القيمة حالياً / (EN) await returns same value for now
}

// =========================================================================
// (AR) تقييم تنفيذ القوالب / (EN) Template Instantiation Evaluation
// =========================================================================

void ExpressionEvaluator::visitTemplateInstantiation(TemplateInstantiation& node) {
    // (AR) إنشاء نسخة من القالب بالأنواع المحددة
    // (EN) Instantiate template with specified types
    
    #ifdef DEBUG
    std::cout << "[Template] تنفيذ قالب: " << node.templateName << "<";
    for (size_t i = 0; i < node.typeArguments.size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << static_cast<int>(node.typeArguments[i]);
    }
    std::cout << ">" << std::endl;
    #endif
    
    // (AR) إنشاء مفتاح القالب
    // (EN) Create template key
    std::string templateKey = "__template_" + node.templateName;
    
    // (AR) البحث عن القالب في FunctionManager
    // (EN) Look for template in FunctionManager
    auto templateFunc = functionManager_.getFunction(templateKey, 0);
    
    if (!templateFunc) {
        // (AR) القالب غير موجود
        // (EN) Template not found
        throw Interpreter::RuntimeError(
            "(AR) القالب '" + node.templateName + "' غير معرّف. (EN) Template '" + node.templateName + "' is not defined.",
            node.position
        );
    }
    
    // (AR) للتنفيذ الحالي: نحفظ معلومات القالب في قيمة خاصة
    // (EN) For current implementation: Store template info in special value
    // سنستخدم هذا عند استدعاء الدالة
    // We'll use this when calling the function
    
    // (AR) إنشاء اسم دالة مؤقت للنسخة المُنشأة
    // (EN) Create temporary function name for instantiated version
    std::string instantiatedName = node.templateName;
    
    // (AR) حفظ اسم القالب في lastResult_
    // (EN) Store template name in lastResult_
    lastResult_ = Value(instantiatedName);
}

} // namespace Interpreter
} // namespace Sad
