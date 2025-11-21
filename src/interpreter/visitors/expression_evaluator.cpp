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
#include <cmath>

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
        case TokenType::NUMBER_INTEGER:
            return Value(std::stoi(token.getValue()));
        
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
            throw RuntimeError(
                "(AR) نوع رمز غير مدعوم: " + token.getValue() + 
                " / (EN) Unsupported token type: " + token.getValue()
            );
    }
}

// =========================================================================
// (AR) تقييم المتغيرات / (EN) Variable Evaluation
// =========================================================================

void ExpressionEvaluator::visitVariableExpr(VariableExpr& node) {
    if (!variableManager_.exists(node.name)) {
        throw RuntimeError(
            "(AR) متغير غير معرّف: " + node.name + 
            " / (EN) Undefined variable: " + node.name
        );
    }
    lastResult_ = variableManager_.get(node.name);
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
            lastResult_ = evaluateArithmeticOp(left, node.op, right);
            break;
        
        // (AR) عمليات مقارنة / (EN) Comparison operations
        case TokenType::OP_EQUAL:
        case TokenType::OP_NOT_EQUAL:
        case TokenType::OP_LESS:
        case TokenType::OP_LESS_EQUAL:
        case TokenType::OP_GREATER:
        case TokenType::OP_GREATER_EQUAL:
            lastResult_ = evaluateComparisonOp(left, node.op, right);
            break;
        
        // (AR) عمليات منطقية / (EN) Logical operations
        case TokenType::OP_AND:
        case TokenType::OP_OR:
            lastResult_ = evaluateLogicalOp(left, node.op, right);
            break;
        
        default:
            throw RuntimeError(
                "(AR) عملية ثنائية غير مدعومة / (EN) Unsupported binary operation"
            );
    }
}

// =========================================================================
// (AR) العمليات الحسابية / (EN) Arithmetic Operations
// =========================================================================

Value ExpressionEvaluator::evaluateArithmeticOp(const Value& left, TokenType op, const Value& right) {
    // جمع النصوص (string concatenation) / String concatenation
    if (op == TokenType::OP_PLUS && (left.isString() || right.isString())) {
        return Value(left.toString() + right.toString());
    }
    
    // التأكد من أن الطرفين رقميين / Ensure both operands are numeric
    if (!left.isNumeric() || !right.isNumeric()) {
        throw RuntimeError(
            "(AR) العمليات الحسابية تتطلب قيم رقمية / (EN) Arithmetic operations require numeric values"
        );
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
                    throw RuntimeError("(AR) قسمة على صفر / (EN) Division by zero");
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
                    throw RuntimeError("(AR) قسمة على صفر / (EN) Division by zero");
                }
                return Value(l / r);
            case TokenType::OP_MODULO:   return Value(l % r);
            case TokenType::OP_POWER:    return Value(static_cast<int>(std::pow(l, r)));
            default: break;
        }
    }
    
    throw RuntimeError("(AR) عملية حسابية غير مدعومة / (EN) Unsupported arithmetic operation");
}

// =========================================================================
// (AR) عمليات المقارنة / (EN) Comparison Operations
// =========================================================================

Value ExpressionEvaluator::evaluateComparisonOp(const Value& left, TokenType op, const Value& right) {
    // المقارنة تعمل على أي نوعين / Comparison works on any two types
    
    // مقارنة الأنواع المختلفة / Different types comparison
    if (left.getType() != right.getType()) {
        // فقط == و != مسموح بهما / Only == and != allowed
        if (op == TokenType::OP_EQUAL) return Value(false);
        if (op == TokenType::OP_NOT_EQUAL) return Value(true);
        throw RuntimeError(
            "(AR) لا يمكن مقارنة أنواع مختلفة / (EN) Cannot compare different types"
        );
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
                throw RuntimeError(
                    "(AR) فقط == و != مسموح بهما للقيم المنطقية / "
                    "(EN) Only == and != allowed for boolean values"
                );
        }
    }
    
    throw RuntimeError("(AR) عملية مقارنة غير مدعومة / (EN) Unsupported comparison operation");
}

// =========================================================================
// (AR) العمليات المنطقية / (EN) Logical Operations
// =========================================================================

Value ExpressionEvaluator::evaluateLogicalOp(const Value& left, TokenType op, const Value& right) {
    bool l = left.toBool();
    bool r = right.toBool();
    
    switch (op) {
        case TokenType::OP_AND: return Value(l && r);
        case TokenType::OP_OR:  return Value(l || r);
        default:
            throw RuntimeError("(AR) عملية منطقية غير مدعومة / (EN) Unsupported logical operation");
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
                throw RuntimeError(
                    "(AR) السالب يتطلب قيمة رقمية / (EN) Negation requires numeric value"
                );
            }
            break;
        
        case TokenType::OP_NOT:    // !flag
            lastResult_ = Value(!operand.toBool());
            break;
        
        case TokenType::OP_PLUS:   // +x (no-op)
            if (!operand.isNumeric()) {
                throw RuntimeError(
                    "(AR) الموجب يتطلب قيمة رقمية / (EN) Positive requires numeric value"
                );
            }
            lastResult_ = operand;
            break;
        
        default:
            throw RuntimeError(
                "(AR) عملية أحادية غير مدعومة / (EN) Unsupported unary operation"
            );
    }
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
            throw RuntimeError(
                "(AR) فهرس المصفوفة يجب أن يكون رقم صحيح / "
                "(EN) Array index must be integer"
            );
        }
        
        int idx = index.toInt();
        Value::ArrayType arr = obj.toArray();
        
        if (idx < 0 || idx >= static_cast<int>(arr.size())) {
            throw RuntimeError(
                "(AR) فهرس خارج النطاق / (EN) Index out of bounds"
            );
        }
        
        lastResult_ = arr[idx];
        
    } else if (obj.isMap()) {
        // فهرسة قاموس / Map indexing
        std::string key = index.toString();
        Value::MapType map = obj.toMap();
        
        if (map.find(key) == map.end()) {
            throw RuntimeError(
                "(AR) المفتاح غير موجود: " + key + 
                " / (EN) Key not found: " + key
            );
        }
        
        lastResult_ = map[key];
        
    } else {
        throw RuntimeError(
            "(AR) الفهرسة تعمل فقط على المصفوفات والقواميس / "
            "(EN) Indexing works only on arrays and maps"
        );
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
            throw RuntimeError(
                "(AR) استدعاء دالة معقد غير مدعوم حالياً / "
                "(EN) Complex function calls not supported yet"
            );
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
            throw RuntimeError(
                "(AR) الدالة '" + funcName + "' غير معرفة بعدد معاملات " + 
                std::to_string(arguments.size()) + 
                " / (EN) Function '" + funcName + "' not defined with " + 
                std::to_string(arguments.size()) + " parameters"
            );
        }
    }
    
    // (AR) التحقق من وجود جسم للدالة / (EN) Check if function has body
    if (!func->hasBody()) {
        throw RuntimeError(
            "(AR) الدالة '" + funcName + "' ليس لها جسم / "
            "(EN) Function '" + funcName + "' has no body"
        );
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
            throw RuntimeError(
                "(AR) معامل إلزامي مفقود: " + param.name + 
                " / (EN) Required parameter missing: " + param.name
            );
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
            throw RuntimeError(
                "(AR) جسم الدالة فارغ / "
                "(EN) Function body is null"
            );
        }
        
        bodyExpr->accept(*this);
        // lastResult_ already contains the result
    }
    
    // (AR) الخروج من نطاق الدالة / (EN) Exit function scope
    scopeManager_.popScope();
}

// =========================================================================
// (AR) تقييم الوصول للعضو / (EN) Member Access Evaluation
// =========================================================================

void ExpressionEvaluator::visitMemberExpr(MemberExpr& node) {
    // الوصول للأعضاء سيتم تنفيذه مع OOP
    // Member access will be implemented with OOP
    throw RuntimeError(
        "(AR) الوصول للأعضاء سيتم دعمه لاحقاً / "
        "(EN) Member access will be supported later"
    );
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
        throw RuntimeError(
            "(AR) الاستيعاب القائمي يتطلب مصفوفة / "
            "(EN) List comprehension requires an array"
        );
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

} // namespace Interpreter
} // namespace Sad
