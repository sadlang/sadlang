/**
 * @file expression_evaluator.cpp
 * @brief (AR) تنفيذ مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "../../../include/interpreter/visitors/expression_evaluator.h"
#include "../../../include/interpreter/visitors/statement_executor.h"
#include "../../../include/parser/ast/statements.h"
#include "../../../include/parser/ast/declarations.h"
#include "../../../include/parser/ast/class_nodes.h"
#include "../../../include/data/managers/class_manager.h"
#include "../../../include/data/types/object_instance.h"
#include "../../../include/errors/error_manager.h"
#include <cmath>
#include <iostream>

namespace Sad {
namespace Interpreter {

using namespace Data;
using namespace AST;
using namespace Lexer;

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
            return Value(token.getValue());
        
        case TokenType::LITERAL_TRUE:
            return Value(true);
        
        case TokenType::LITERAL_FALSE:
            return Value(false);
        
        case TokenType::LITERAL_NULL:
            return Value();  // VOID
        
        default:
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
                Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "متغير غير معرّف: " + node.name,
            "Undefined variable: " + node.name
        );
        lastResult_ = Value(); // Return null
        return;
    }
    lastResult_ = variableManager_.get(node.name);
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
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
    // تقييم القيمة اليمنى / Evaluate right-hand side
    node.value->accept(*this);
    Value value = lastResult_;
    
    // إسناد للمتغير / Assign to variable
    variableManager_.assign(node.name, value);
    
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
                Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
                    Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
                    Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
                Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
                Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
                Sad::Errors::SourceLocation("<runtime>", 0, 0),
                "استدعاء دالة معقد غير مدعوم حالياً",
                "Complex function calls not supported yet"
            );
            lastResult_ = Value();
            return;
        }
        
        funcName = calleeValue.toString();
    }
    
    // (AR) تقييم المعاملات / (EN) Evaluate arguments
    std::vector<Data::Value> arguments;
    for (const auto& arg : node.arguments) {
        arg->accept(*this);
        arguments.push_back(lastResult_);
    }
    
    // (AR) البحث عن الدالة - flexible matching مع default parameters
    // (EN) Find function - flexible matching with default parameters
    auto allOverloads = functionManager_.getFunctionOverloads(funcName);
    std::shared_ptr<Data::FunctionDefinition> func = nullptr;
    
    // (AR) نبحث عن دالة تقبل هذا العدد من المعاملات (مع الافتراضيات)
    // (EN) Search for function that accepts this argument count (with defaults)
    for (const auto& candidate : allOverloads) {
        // (AR) الدوال المضمنة يمكن أن تقبل أي عدد من المعاملات
        // (EN) Built-in functions can accept any number of arguments
        if (candidate->hasNativeImplementation()) {
            func = candidate;
            break;
        }
        
        if (candidate->acceptsArgumentCount(arguments.size())) {
            func = candidate;
            break;
        }
    }
    
    // (AR) إذا لم نجد دالة مناسبة
    // (EN) If no suitable function found
    if (!func) {
        // (AR) محاولة أخيرة - exact match (للدوال بدون defaults)
        // (EN) Last attempt - exact match (for functions without defaults)
        func = functionManager_.getFunction(funcName, arguments.size());
        
        if (!func) {
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
                Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الدالة '" + funcName + "' ليس لها جسم",
            "Function '" + funcName + "' has no body"
        );
        lastResult_ = Value();
        return;
    }
    
    // (AR) إنشاء نطاق جديد للدالة / (EN) Create new scope for function
    scopeManager_.pushScope(Data::ScopeType::FUNCTION, funcName);
    
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
            scopeManager_.popScope();
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_WRONG_ARG_COUNT,
                Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
                scopeManager_.popScope();
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
            scopeManager_.popScope();
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
                Sad::Errors::SourceLocation("<runtime>", 0, 0),
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
    scopeManager_.popScope();
}

// =========================================================================
// (AR) تقييم إنشاء كائن جديد / (EN) New Object Creation Evaluation
// =========================================================================

void ExpressionEvaluator::visitNewExpr(NewExpr& node) {
    std::cout << "[OOP] تنفيذ تعبير جديد: " << node.className << "\n";
    
    // الحصول على ClassType من ClassManager
    auto* classManager = Data::ClassManager::getInstance();
    ClassType* classType = classManager->getClass(node.className);
    
    if (!classType) {
        std::string errMsg = "(AR) الصنف '" + node.className + "' غير موجود. ";
        errMsg += "(EN) Class '" + node.className + "' not found.";
        throw RuntimeError(errMsg);
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
    
    std::cout << "[OOP] تم إنشاء كائن من صنف: " << node.className << "\n";
    std::cout << "[OOP] عدد الحقول: " << allFields.size() << " (بما في ذلك الموروثة)\n";
    
    // استدعاء الباني إذا كان موجودًا
    if (classType->constructor) {
        auto& constructor = classType->constructor;
        
        // التحقق من عدد المعاملات
        if (node.arguments.size() != constructor->parameters.size()) {
            std::string errMsg = "(AR) عدد المعاملات غير متطابق. توقع " + 
                std::to_string(constructor->parameters.size()) + " لكن حصل على " + 
                std::to_string(node.arguments.size()) + ". ";
            errMsg += "(EN) Argument count mismatch. Expected " + 
                std::to_string(constructor->parameters.size()) + " but got " + 
                std::to_string(node.arguments.size()) + ".";
            throw RuntimeError(errMsg);
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
        // هذا حل مؤقت - يجب استخدام 'this' بشكل صحيح
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
        
        // تنفيذ جسم الباني
        try {
            constructor->body->accept(statementExecutor_);
            
            // جمع القيم المحدثة من الـ scope
            for (const auto& field : classType->fields) {
                try {
                    Value updatedValue = variableManager_.get(field.name);
                    if (field.isStatic) {
                        // تحديث الحقل الثابت في ClassType
                        classType->setStaticField(field.name, updatedValue);
                    } else {
                        // تحديث حقل الكائن
                        objectFields[field.name] = updatedValue;
                    }
                } catch (...) {
                    // الحقل لم يتم تعيينه في الباني، استخدام القيمة الافتراضية
                }
            }
        } catch (const std::exception& e) {
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
    std::cout << "[OOP] استدعاء طريقة: " << node.methodName << "\n";
    
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
            std::cout << "[OOP] استدعاء طريقة ثابتة: " << possibleClassName << "." << node.methodName << "\n";
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
            throw RuntimeError(errMsg);
        }
        
        // الحصول على اسم الصنف من الكائن
        fields = objectValue.toMap();
        auto classNameIt = fields.find("__class__");
        if (classNameIt == fields.end()) {
            throw RuntimeError("(AR) كائن بدون معلومات صنف. (EN) Object without class info.");
        }
        
        className = classNameIt->second.toString();
        
        // الحصول على ClassType
        classType = classManager->getClass(className);
    }
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.");
    }
    
    // البحث عن الطريقة (في السلسلة الهرمية)
    ClassMethod* method = classType->findMethod(node.methodName);
    if (!method) {
        std::string errMsg = "(AR) الطريقة '" + node.methodName + "' غير موجودة في الصنف '" + className + "'. ";
        errMsg += "(EN) Method '" + node.methodName + "' not found in class '" + className + "'.";
        throw RuntimeError(errMsg);
    }
    
    // التحقق من التطابق بين نوع الاستدعاء ونوع الطريقة
    // Verify call type matches method type
    if (isStaticCall && !method->isStatic) {
        std::string errMsg = "(AR) لا يمكن استدعاء طريقة غير ثابتة '" + node.methodName + "' من خلال اسم الصنف. ";
        errMsg += "(EN) Cannot call non-static method '" + node.methodName + "' through class name.";
        throw RuntimeError(errMsg);
    }
    if (!isStaticCall && method->isStatic) {
        std::string errMsg = "(AR) يجب استدعاء الطريقة الثابتة '" + node.methodName + "' من خلال اسم الصنف. ";
        errMsg += "(EN) Static method '" + node.methodName + "' should be called through class name.";
        throw RuntimeError(errMsg);
    }
    
    // فحص الوصول (Phase 6.1: Access Modifiers)
    checkMemberAccess(method->visibility, node.methodName, classType);
    
    // التحقق من عدد المعاملات
    if (node.arguments.size() != method->parameters.size()) {
        std::string errMsg = "(AR) عدد المعاملات غير متطابق. توقع " + 
            std::to_string(method->parameters.size()) + " لكن حصل على " + 
            std::to_string(node.arguments.size()) + ". ";
        errMsg += "(EN) Argument count mismatch.";
        throw RuntimeError(errMsg);
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
    Value returnValue;
    try {
        if (method->body) {
            method->body->accept(statementExecutor_);
            
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
    } catch (const std::exception& e) {
        variableManager_.exitScope();
        throw;
    }
    
    variableManager_.exitScope();
    
    std::cout << "[OOP] ✅ تم تنفيذ الطريقة: " << node.methodName << "\n";
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
            std::cout << "[OOP] الوصول لحقل ثابت: " << possibleClassName << "." << node.member << "\n";
            
            // البحث عن الحقل
            ClassField* field = classType->findField(node.member);
            if (!field) {
                std::string errMsg = "(AR) الحقل '" + node.member + "' غير موجود في الصنف '" + possibleClassName + "'. ";
                errMsg += "(EN) Field '" + node.member + "' not found in class '" + possibleClassName + "'.";
                throw RuntimeError(errMsg);
            }
            
            // التحقق من أن الحقل ثابت
            if (!field->isStatic) {
                std::string errMsg = "(AR) لا يمكن الوصول للحقل غير الثابت '" + node.member + "' من خلال اسم الصنف. ";
                errMsg += "(EN) Cannot access non-static field '" + node.member + "' through class name.";
                throw RuntimeError(errMsg);
            }
            
            // فحص الوصول
            checkMemberAccess(field->visibility, node.member, classType);
            
            // الحصول على قيمة الحقل الثابت
            Value* staticValue = classType->getStaticField(node.member);
            if (!staticValue) {
                std::string errMsg = "(AR) الحقل الثابت '" + node.member + "' غير مهيأ. ";
                errMsg += "(EN) Static field '" + node.member + "' not initialized.";
                throw RuntimeError(errMsg);
            }
            
            lastResult_ = *staticValue;
            std::cout << "[OOP] قيمة الحقل الثابت: " << lastResult_.toString() << "\n";
            return;
        }
    }
    
    // وصول عادي للكائن: object.field
    // Regular object access: object.field
    
    // التحقق من أن القيمة كائن (MAP مؤقتًا)
    if (!objectValue.isMap()) {
        std::string errMsg = "(AR) لا يمكن الوصول لعضو من قيمة ليست كائن. ";
        errMsg += "(EN) Cannot access member of non-object value.";
        throw RuntimeError(errMsg);
    }
    
    std::cout << "[OOP] الوصول لحقل: " << node.member << "\n";
    
    // الحصول على MAP
    Value::MapType fields = objectValue.toMap();
    
    // الحصول على اسم الصنف
    auto classNameIt = fields.find("__class__");
    if (classNameIt == fields.end()) {
        throw RuntimeError("(AR) كائن بدون معلومات صنف. (EN) Object without class info.");
    }
    
    std::string className = classNameIt->second.toString();
    
    // الحصول على ClassType
    ClassType* classType = classManager->getClass(className);
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.");
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
        throw RuntimeError(errMsg);
    }
    
    // إذا كانت خاصية، نفذ الـ getter
    if (property) {
        std::cout << "[OOP] تنفيذ getter للخاصية: " << node.member << "\n";
        
        // فحص الوصول
        checkMemberAccess(property->visibility, node.member, classType);
        
        // التحقق من وجود getter
        if (!property->getterBody) {
            std::string errMsg = "(AR) الخاصية '" + node.member + "' للكتابة فقط (لا يوجد getter). ";
            errMsg += "(EN) Property '" + node.member + "' is write-only (no getter).";
            throw RuntimeError(errMsg);
        }
        
        // تنفيذ getter body في نطاق جديد
        // TODO: Add proper 'this' context support in future
        try {
            property->getterBody->accept(*this);
        } catch (...) {
            // في حالة حدوث خطأ، نعيد القيمة الحالية
        }
        
        std::cout << "[OOP] قيمة الخاصية: " << lastResult_.toString() << "\n";
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
        throw RuntimeError(errMsg);
    }
    
    // إرجاع قيمة الحقل
    lastResult_ = it->second;
    std::cout << "[OOP] قيمة الحقل: " << lastResult_.toString() << "\n";
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
        throw RuntimeError(errMsg);
    }
    
    // تقييم القيمة الجديدة
    node.value->accept(*this);
    Value newValue = lastResult_;
    
    // الحصول على MAP
    Value::MapType fields = objectValue.toMap();
    
    // الحصول على اسم الصنف
    auto classNameIt = fields.find("__class__");
    if (classNameIt == fields.end()) {
        throw RuntimeError("(AR) كائن بدون معلومات صنف. (EN) Object without class info.");
    }
    
    std::string className = classNameIt->second.toString();
    
    // الحصول على ClassType
    auto* classManager = Data::ClassManager::getInstance();
    ClassType* classType = classManager->getClass(className);
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.");
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
        throw RuntimeError(errMsg);
    }
    
    // إذا كانت خاصية، نفذ الـ setter
    if (property) {
        std::cout << "[OOP] تنفيذ setter للخاصية: " << node.member << "\n";
        
        // فحص الوصول
        checkMemberAccess(property->visibility, node.member, classType);
        
        // التحقق من وجود setter
        if (!property->setterBody) {
            std::string errMsg = "(AR) الخاصية '" + node.member + "' للقراءة فقط (لا يوجد setter). ";
            errMsg += "(EN) Property '" + node.member + "' is read-only (no setter).";
            throw RuntimeError(errMsg);
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
        std::cout << "[OOP] تم تعيين قيمة الخاصية: " << newValue.toString() << "\n";
        return;
    }
    
    // معالجة الحقل العادي
    // فحص الوصول (Phase 6.1: Access Modifiers)
    checkMemberAccess(field->visibility, node.member, classType);
    
    // التحقق من وجود الحقل في الكائن
    if (fields.find(node.member) == fields.end()) {
        std::string errMsg = "(AR) الحقل '" + node.member + "' غير موجود في الكائن. ";
        errMsg += "(EN) Field '" + node.member + "' not found in object.";
        throw RuntimeError(errMsg);
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
        throw RuntimeError(errMsg);
    }
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
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الاستيعاب القائمي يتطلب مصفوفة",
            "List comprehension requires an array"
        );
        lastResult_ = Value(std::vector<Value>());
        return;
    }
    
    // (AR) إنشاء مصفوفة النتيجة / (EN) Create result array
    std::vector<Value> result;
    
    // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
    scopeManager_.pushScope(Data::ScopeType::BLOCK, "list_comprehension");
    
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
    scopeManager_.popScope();
    
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
        throw RuntimeError(
            "(AR) الاستيعاب القاموسي يتطلب مصفوفة / "
            "(EN) Dict comprehension requires an array"
        );
    }
    
    // (AR) إنشاء قاموس النتيجة / (EN) Create result map
    std::unordered_map<std::string, Value> result;
    
    // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
    scopeManager_.pushScope(Data::ScopeType::BLOCK, "dict_comprehension");
    
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
        
        // (AR) تقييم key و value وإضافة للنتيجة / (EN) Evaluate key and value and add to result
        if (includeItem) {
            // تقييم المفتاح
            node.key->accept(*this);
            std::string keyStr = lastResult_.toString();
            
            // تقييم القيمة
            node.value->accept(*this);
            Value valueResult = lastResult_;
            
            // إضافة إلى القاموس
            result[keyStr] = valueResult;
        }
    }
    
    // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
    scopeManager_.popScope();
    
    // (AR) إرجاع القاموس الناتج / (EN) Return result map
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
        throw RuntimeError(errMsg);
    }
    
    if (visibility == AST::Visibility::PROTECTED) {
        std::string errMsg = "(AR) لا يمكن الوصول للعضو المحمي '" + memberName + 
                           "' من خارج الصنف '" + targetClass->name + "' أو الأصناف المشتقة. ";
        errMsg += "(EN) Cannot access protected member '" + memberName + 
                 "' from outside class '" + targetClass->name + "' or derived classes.";
        throw RuntimeError(errMsg);
    }
}

} // namespace Interpreter
} // namespace Sad
