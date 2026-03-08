// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4819)
#endif

/**
 * @file type_checker.cpp
 * @brief تنفيذ فاحص الأنواع / Type Checker Implementation
 * @phase Phase 5: US3 - نظام الأنواع المتقدم
 */

#include "semantic/type_checker.h"
#include "token.h"
#include "class_nodes.h"
#include "types/composite_type_classes.h"
#include "types/enum_types.h"
#include "types/struct_types.h"

#include <iostream>
#include <algorithm>
#include <cassert>

namespace Sad {
namespace Semantic {

using namespace TypeSystem;
using TT = Lexer::TokenType;

// ============================================================================
// TypeCheckError
// ============================================================================

std::string TypeCheckError::toEnglishString() const {
    std::ostringstream oss;
    oss << "[Type Error] ";
    if (!variableName.empty())
        oss << "'" << variableName << "': ";
    if (!message.empty())
        oss << message;
    else
        oss << "expected '" << expectedType << "' but got '" << actualType << "'";
    oss << " (line " << line << ", col " << column << ")";
    return oss.str();
}

std::string TypeCheckError::toArabicString() const {
    std::ostringstream oss;
    oss << "[خطأ نوع] ";
    if (!variableName.empty())
        oss << "'" << variableName << "': ";
    if (!arabicMessage.empty())
        oss << arabicMessage;
    else
        oss << "متوقع '" << expectedType << "' لكن وُجد '" << actualType << "'";
    oss << " (سطر " << line << ", عمود " << column << ")";
    return oss.str();
}

// ============================================================================
// TypeChecker - بناء / Construction
// ============================================================================

TypeChecker::TypeChecker()
    : registry_(TypeRegistry::getInstance())
    , lastInferredType_(nullptr)
    , expectedReturnType_(nullptr)
    , useArabicMessages_(true)
    , debugMode_(false)
    , strictMode_(false) {
    // إنشاء بيئة الأنواع العامة / Create global type environment
    currentEnv_ = std::make_shared<TypeEnvironment>();
    scopeStack_.push_back(currentEnv_);
    
    // تسجيل الدوال المدمجة / Register built-in functions
    currentEnv_->bind("اطبع", registry_.getVoidType());        // print
    currentEnv_->bind("اطبع_سطر", registry_.getVoidType());    // println
    currentEnv_->bind("ادخل", registry_.getStringType());       // input
    currentEnv_->bind("ادخل_رقم", registry_.getIntegerType());  // input_number
    currentEnv_->bind("طول", registry_.getIntegerType());        // length
    currentEnv_->bind("نوع", registry_.getStringType());         // typeof
    
    // English aliases
    currentEnv_->bind("print", registry_.getVoidType());
    currentEnv_->bind("println", registry_.getVoidType());
    currentEnv_->bind("input", registry_.getStringType());
    currentEnv_->bind("len", registry_.getIntegerType());
    currentEnv_->bind("type", registry_.getStringType());
}

// ============================================================================
// واجهة الفحص الرئيسية / Main Check Interface
// ============================================================================

TypeCheckResult TypeChecker::check(AST::ASTNode* ast) {
    currentResult_ = TypeCheckResult();
    
    if (!ast) {
        currentResult_.addError({
            "", "", "", 0, 0,
            "Null AST provided to type checker",
            "تم تمرير شجرة AST فارغة لفاحص الأنواع"
        });
        return currentResult_;
    }
    
    try {
        ast->accept(*this);
    } catch (const std::exception& e) {
        currentResult_.addError({
            "", "", "", 0, 0,
            std::string("Internal type checker error: ") + e.what(),
            std::string("خطأ داخلي في فاحص الأنواع: ") + e.what()
        });
    }
    
    if (debugMode_) {
        if (useArabicMessages_)
            std::cerr << currentResult_.getArabicSummary();
        else
            std::cerr << currentResult_.getSummary();
    }
    
    return currentResult_;
}

void TypeChecker::printSummary() const {
    if (useArabicMessages_) {
        std::cerr << currentResult_.getArabicSummary();
    } else {
        std::cerr << currentResult_.getSummary();
    }
    
    // طباعة الأخطاء / Print errors
    for (const auto& err : currentResult_.errors) {
        if (useArabicMessages_) {
            std::cerr << "  ❌ خطأ نوع";
            if (!err.variableName.empty()) std::cerr << " في '" << err.variableName << "'";
            std::cerr << ": " << err.arabicMessage;
            if (err.line > 0) std::cerr << " (سطر " << err.line << ")";
            std::cerr << "\n";
        } else {
            std::cerr << "  ❌ Type error";
            if (!err.variableName.empty()) std::cerr << " in '" << err.variableName << "'";
            std::cerr << ": " << err.message;
            if (err.line > 0) std::cerr << " (line " << err.line << ")";
            std::cerr << "\n";
        }
    }
    
    // طباعة التحذيرات / Print warnings
    for (const auto& warn : currentResult_.warnings) {
        std::cerr << "  ⚠️ " << warn << "\n";
    }
}

void TypeChecker::reset() {
    currentResult_ = TypeCheckResult();
    lastInferredType_ = nullptr;
    expectedReturnType_ = nullptr;
    currentFunction_.clear();
    scopeStack_.clear();
    currentEnv_ = std::make_shared<TypeEnvironment>();
    scopeStack_.push_back(currentEnv_);
}

// ============================================================================
// تحويل DataType إلى TypePtr / Convert DataType to TypePtr
// ============================================================================

TypePtr TypeChecker::dataTypeToTypePtr(Data::DataType dt) const {
    switch (dt) {
        case Data::DataType::INTEGER:   return registry_.getIntegerType();
        case Data::DataType::FLOAT:     return registry_.getFloatType();
        case Data::DataType::STRING:    return registry_.getStringType();
        case Data::DataType::BOOLEAN:   return registry_.getBooleanType();
        case Data::DataType::NONE:      return registry_.getVoidType();
        case Data::DataType::ARRAY:     return std::make_shared<ArrayType>(registry_.getAnyType()); // عنصر المصفوفة يُحدد لاحقاً
        case Data::DataType::MAP:       return std::make_shared<DictionaryType>(registry_.getStringType(), registry_.getAnyType()); // مفتاح/قيمة يُحددان لاحقاً
        case Data::DataType::FUNCTION:  return std::make_shared<FunctionType>(TypeList{}, registry_.getVoidType()); // توقيع الدالة يُحدد لاحقاً
        case Data::DataType::OBJECT:    return registry_.getAnyType(); // يُحدد من StructRegistry عند الوصول
        case Data::DataType::ENUM:      return registry_.getAnyType(); // يُحدد من EnumRegistry عند الوصول
        case Data::DataType::TUPLE:     return std::make_shared<TupleType>(TypeList{}); // عناصر الصف تُحدد لاحقاً
        case Data::DataType::BYTE:      return registry_.getIntegerType(); // byte -> int
        case Data::DataType::ERROR:     return registry_.getUnknownType();
        case Data::DataType::UNKNOWN:
        default:                        return registry_.getUnknownType();
    }
}

// ============================================================================
// التحقق من توافق الأنواع / Check Type Compatibility
// ============================================================================

bool TypeChecker::areTypesCompatible(TypePtr expected, TypePtr actual) const {
    if (!expected || !actual) return true;  // null types = no checking
    
    // أي نوع = Any يتوافق مع الكل / Any type is compatible with everything
    if (expected->isAny() || actual->isAny()) return true;
    
    // Unknown = لم يُحدد بعد / Unknown = not yet determined
    if (expected->isUnknown() || actual->isUnknown()) return true;
    
    // نفس النوع بالضبط / Exact same type
    if (expected->getKind() == actual->getKind()) return true;
    
    // التحقق من التوافق الرقمي / Numeric compatibility
    if (expected->isNumeric() && actual->isNumeric()) return true;
    
    // Never يتوافق مع أي نوع / Never is subtype of everything
    if (actual->isNever()) return true;
    
    // استخدم isAssignableTo / Use isAssignableTo  
    return actual->isAssignableTo(expected.get());
}

// ============================================================================
// استنتاج نوع تعبير / Infer Expression Type
// ============================================================================

TypePtr TypeChecker::inferExprType(AST::Expression* expr) {
    if (!expr) return registry_.getUnknownType();
    
    lastInferredType_ = nullptr;
    expr->accept(*this);
    
    TypePtr result = lastInferredType_;
    if (!result) {
        // Fallback: استخدم DataType من AST / use DataType from AST
        result = dataTypeToTypePtr(expr->getType());
    }
    
    currentResult_.totalInferred++;
    return result;
}

// ============================================================================
// إدارة النطاقات / Scope Management
// ============================================================================

void TypeChecker::enterScope() {
    // إنشاء بيئة فرعية مع تمرير الأب / Create child env with parent
    // ملاحظة: لا نستخدم createChild() لأنها لا تمرر الأب بشكل صحيح
    auto childEnv = std::make_shared<TypeEnvironment>(currentEnv_);
    scopeStack_.push_back(childEnv);
    currentEnv_ = childEnv;
}

void TypeChecker::exitScope() {
    if (scopeStack_.size() > 1) {
        scopeStack_.pop_back();
        currentEnv_ = scopeStack_.back();
    }
}

void TypeChecker::declareVariable(const std::string& name, TypePtr type) {
    currentEnv_->bind(name, type);
}

TypePtr TypeChecker::lookupVariable(const std::string& name) const {
    auto type = currentEnv_->lookup(name);
    return type ? type : nullptr;
}

// ============================================================================
// مساعدات / Helpers
// ============================================================================

std::pair<size_t, size_t> TypeChecker::getLocation(AST::ASTNode* node) const {
    if (!node) return {0, 0};
    auto& pos = node->position;
    return {pos.line, pos.column};
}

void TypeChecker::recordTypeError(const std::string& varName,
                                  const std::string& expected,
                                  const std::string& actual,
                                  AST::ASTNode* node,
                                  const std::string& msg) {
    auto [line, col] = getLocation(node);
    
    TypeCheckError err;
    err.variableName = varName;
    err.expectedType = expected;
    err.actualType = actual;
    err.line = line;
    err.column = col;
    
    if (msg.empty()) {
        err.message = "Type mismatch: expected '" + expected + "' but got '" + actual + "'";
        err.arabicMessage = "عدم تطابق الأنواع: متوقع '" + expected + "' لكن وُجد '" + actual + "'";
    } else {
        err.message = msg;
        err.arabicMessage = msg;
    }
    
    currentResult_.addError(err);
}

// ============================================================================
// زيارة التعابير / Visit Expressions
// ============================================================================

void TypeChecker::visitLiteralExpr(AST::LiteralExpr& expr) {
    currentResult_.totalExpressions++;
    
    switch (expr.token.getType()) {
        case TT::NUMBER_INTEGER:
            lastInferredType_ = registry_.getIntegerType();
            break;
        case TT::NUMBER_DOUBLE:
            lastInferredType_ = registry_.getFloatType();
            break;
        case TT::STRING_LITERAL:
            lastInferredType_ = registry_.getStringType();
            break;
        case TT::LITERAL_TRUE:
        case TT::LITERAL_FALSE:
            lastInferredType_ = registry_.getBooleanType();
            break;
        case TT::LITERAL_NULL:
            lastInferredType_ = registry_.getVoidType();
            break;
        default:
            lastInferredType_ = registry_.getUnknownType();
            break;
    }
    
    if (debugMode_) {
        std::cerr << "  [TC] Literal: " << (lastInferredType_ ? lastInferredType_->toString() : "null") << "\n";
    }
}

void TypeChecker::visitVariableExpr(AST::VariableExpr& expr) {
    currentResult_.totalExpressions++;
    
    auto type = lookupVariable(expr.name);
    if (type) {
        lastInferredType_ = type;
    } else {
        // متغير غير معرّف — ليس خطأ أنواع بل خطأ دلالي
        // Undeclared variable — not a type error, semantic error
        lastInferredType_ = registry_.getUnknownType();
        
        if (strictMode_) {
            recordTypeError(expr.name, "", "", &expr,
                "Undeclared variable '" + expr.name + "'");
        }
    }
    
    if (debugMode_) {
        std::cerr << "  [TC] Variable '" << expr.name << "': "
                  << (lastInferredType_ ? lastInferredType_->toString() : "unknown") << "\n";
    }
}

void TypeChecker::visitBinaryExpr(AST::BinaryExpr& expr) {
    currentResult_.totalExpressions++;
    
    TypePtr leftType = inferExprType(expr.left.get());
    TypePtr rightType = inferExprType(expr.right.get());
    
    switch (expr.op) {
        // عمليات حسابية / Arithmetic
        case TT::OP_PLUS:
            // يمكن أن تكون جمع أرقام أو دمج نصوص
            if (leftType && rightType) {
                if (leftType->isString() || rightType->isString()) {
                    lastInferredType_ = registry_.getStringType();
                } else if (leftType->isNumeric() && rightType->isNumeric()) {
                    // float يسود / float dominates
                    if (leftType->isFloat() || rightType->isFloat()) {
                        lastInferredType_ = registry_.getFloatType();
                    } else {
                        lastInferredType_ = registry_.getIntegerType();
                    }
                } else {
                    lastInferredType_ = registry_.getUnknownType();
                    if (strictMode_) {
                        recordTypeError("", 
                            "numeric or string",
                            leftType->toString() + " + " + rightType->toString(),
                            &expr, "Cannot add incompatible types");
                    }
                }
            } else {
                lastInferredType_ = registry_.getUnknownType();
            }
            break;
            
        case TT::OP_MINUS:
        case TT::OP_MULTIPLY:
        case TT::OP_DIVIDE:
        case TT::OP_MODULO:
            // عمليات حسابية تتطلب أرقام / Arithmetic requires numbers
            if (leftType && rightType) {
                if (leftType->isNumeric() && rightType->isNumeric()) {
                    if (leftType->isFloat() || rightType->isFloat() || expr.op == TT::OP_DIVIDE) {
                        lastInferredType_ = registry_.getFloatType();
                    } else {
                        lastInferredType_ = registry_.getIntegerType();
                    }
                } else {
                    lastInferredType_ = registry_.getUnknownType();
                    if (strictMode_) {
                        recordTypeError("",
                            "numeric",
                            leftType->toString() + " op " + rightType->toString(),
                            &expr, "Arithmetic operation requires numeric operands");
                    }
                }
            } else {
                lastInferredType_ = registry_.getUnknownType();
            }
            break;
            
        // عمليات المقارنة / Comparison
        case TT::OP_EQUAL:
        case TT::OP_NOT_EQUAL:
        case TT::OP_LESS:
        case TT::OP_LESS_EQUAL:
        case TT::OP_GREATER:
        case TT::OP_GREATER_EQUAL:
            lastInferredType_ = registry_.getBooleanType();
            break;
            
        // عمليات منطقية / Logical
        case TT::OP_AND:
        case TT::OP_OR:
            lastInferredType_ = registry_.getBooleanType();
            break;
            
        default:
            lastInferredType_ = registry_.getUnknownType();
            break;
    }
}

void TypeChecker::visitUnaryExpr(AST::UnaryExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr operandType = inferExprType(expr.operand.get());
    
    switch (expr.op) {
        case TT::OP_MINUS:
            if (operandType && operandType->isNumeric()) {
                lastInferredType_ = operandType;
            } else {
                lastInferredType_ = registry_.getUnknownType();
                if (strictMode_) {
                    recordTypeError("", "numeric",
                        operandType ? operandType->toString() : "unknown",
                        &expr, "Unary minus requires numeric operand");
                }
            }
            break;
            
        case TT::OP_NOT:
            lastInferredType_ = registry_.getBooleanType();
            break;
            
        default:
            lastInferredType_ = operandType ? operandType : registry_.getUnknownType();
            break;
    }
}

void TypeChecker::visitTernaryExpr(AST::TernaryExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr condType = inferExprType(expr.condition.get());
    TypePtr trueType = inferExprType(expr.trueExpr.get());
    TypePtr falseType = inferExprType(expr.falseExpr.get());
    
    // الشرط يجب أن يكون منطقي / Condition should be boolean
    if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown()) {
        recordTypeError("", "boolean", condType->toString(), &expr,
            "Ternary condition should be boolean");
    }
    
    // إذا تطابق الفرعين / If branches match
    if (trueType && falseType && trueType->getKind() == falseType->getKind()) {
        lastInferredType_ = trueType;
    } else {
        // Union type would be ideal, fall back to broader type
        lastInferredType_ = trueType ? trueType : registry_.getUnknownType();
    }
}

void TypeChecker::visitAssignExpr(AST::AssignExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr valueType = inferExprType(expr.value.get());
    
    auto varType = lookupVariable(expr.name);
    if (varType && valueType) {
        if (!areTypesCompatible(varType, valueType)) {
            recordTypeError(expr.name,
                varType->toString(), valueType->toString(),
                &expr);
        }
    }
    
    lastInferredType_ = valueType;
}

void TypeChecker::visitCallExpr(AST::CallExpr& expr) {
    currentResult_.totalExpressions++;
    
    // تحقق من أنواع المعاملات / Check argument types
    for (auto& arg : expr.arguments) {
        if (arg) inferExprType(arg.get());
    }
    
    // استنتج نوع الإرجاع من المُستدعى / Infer return type from callee
    TypePtr calleeType = inferExprType(expr.callee.get());
    
    // إذا كان CalleeType function type، استخرج نوع الإرجاع
    if (calleeType && calleeType->getKind() == TypeKind::Function) {
        auto* fnType = static_cast<FunctionType*>(calleeType.get());
        TypePtr retType = fnType->getReturnType();
        // (AR) إذا لم يُحدَّد نوع الإرجاع، نفترض رقم (مثل int في C)
        // (EN) If return type is unspecified, default to integer (like C's implicit int)
        lastInferredType_ = retType ? retType : registry_.getIntegerType();
    } else {
        lastInferredType_ = calleeType ? calleeType : registry_.getUnknownType();
    }
}

void TypeChecker::visitIndexExpr(AST::IndexExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr objType = inferExprType(expr.object.get());
    TypePtr idxType = inferExprType(expr.index.get());
    
    if (objType && objType->isArray()) {
        // Array[int] -> element type
        if (strictMode_ && idxType && !idxType->isInteger() && !idxType->isUnknown()) {
            recordTypeError("", "integer", idxType->toString(), &expr,
                "Array index must be integer");
        }
        // (AR) استرجاع نوع العنصر من نوع المصفوفة / (EN) Retrieve element type from array type
        if (objType->getKind() == TypeKind::Array) {
            auto* arrType = static_cast<ArrayType*>(objType.get());
            TypePtr elemType = arrType->getElementType();
            lastInferredType_ = elemType ? elemType : registry_.getUnknownType();
            return;
        }
    }
    // (AR) إذا كان كائن به عملية فهرسة / (EN) Object with subscript operation
    if (objType && (objType->getKind() == TypeKind::Dictionary || objType->isString())) {
        lastInferredType_ = objType->isString() ? registry_.getStringType() : registry_.getAnyType();
        return;
    }
    lastInferredType_ = registry_.getUnknownType();
}

void TypeChecker::visitMemberExpr(AST::MemberExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr objType = inferExprType(expr.object.get());
    
    // (AR) البحث عن نوع العضو من StructRegistry / (EN) Look up member type from StructRegistry
    if (objType && (objType->getKind() == TypeKind::Class)) {
        std::string className;
        if (auto* newExpr = dynamic_cast<AST::NewExpr*>(expr.object.get())) {
            className = newExpr->className;
        }
        if (!className.empty()) {
            auto structType = StructRegistry::instance().findStruct(className);
            if (structType) {
                auto field = structType->findField(expr.member);
                if (field) {
                    lastInferredType_ = field->getType() ? field->getType() : registry_.getUnknownType();
                    return;
                }
            }
        }
    }
    // (AR) للنصوص: خصائص مثل الطول / (EN) For strings: properties like length
    if (objType && objType->isString()) {
        if (expr.member == "الطول" || expr.member == "length") {
            lastInferredType_ = registry_.getIntegerType();
            return;
        }
    }
    lastInferredType_ = registry_.getUnknownType();
}

void TypeChecker::visitMemberAssignExpr(AST::MemberAssignExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr objType = inferExprType(expr.object.get());
    TypePtr valType = inferExprType(expr.value.get());
    
    // (AR) تحقق من توافق نوع العضو مع القيمة المُسندة / (EN) Check member type compatibility with assigned value
    if (strictMode_ && objType && (objType->getKind() == TypeKind::Class)) {
        std::string className;
        if (auto* newExpr = dynamic_cast<AST::NewExpr*>(expr.object.get())) {
            className = newExpr->className;
        }
        if (!className.empty()) {
            auto structType = StructRegistry::instance().findStruct(className);
            if (structType) {
                auto field = structType->findField(expr.member);
                if (field && field->getType() && valType && !valType->isUnknown()) {
                    TypePtr fieldType = field->getType();
                    if (!fieldType->isUnknown() && !areTypesCompatible(fieldType, valType)) {
                        recordTypeError(expr.member, fieldType->toString(), valType->toString(), &expr,
                            "Member assignment type mismatch");
                    }
                }
            }
        }
    }
    lastInferredType_ = valType;
}

void TypeChecker::visitArrayExpr(AST::ArrayExpr& expr) {
    currentResult_.totalExpressions++;
    
    TypePtr elementType = nullptr;
    for (auto& elem : expr.elements) {
        TypePtr et = inferExprType(elem.get());
        if (!elementType) {
            elementType = et;
        } else if (et && elementType && et->getKind() != elementType->getKind()) {
            // مصفوفة مختلطة / Mixed array
            if (!et->isUnknown() && !elementType->isUnknown()) {
                currentResult_.addWarning(
                    "Mixed array element types: " + elementType->toString() + " and " + et->toString());
            }
        }
    }
    
    // (AR) إنشاء نوع المصفوفة الفعلي مع نوع العنصر
    // (EN) Create actual ArrayType with element type
    if (elementType) {
        lastInferredType_ = std::make_shared<ArrayType>(
            elementType, 
            expr.elements.empty() ? std::nullopt : std::optional<size_t>(expr.elements.size()));
    } else {
        lastInferredType_ = std::make_shared<ArrayType>(registry_.getAnyType());
    }
}

void TypeChecker::visitMapExpr(AST::MapExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr keyType = nullptr;
    TypePtr valType = nullptr;
    for (auto& pair : expr.pairs) {
        TypePtr kt = inferExprType(pair.key.get());
        TypePtr vt = inferExprType(pair.value.get());
        if (!keyType) keyType = kt;
        if (!valType) valType = vt;
    }
    // (AR) إنشاء نوع القاموس مع نوع المفتاح والقيمة
    lastInferredType_ = std::make_shared<DictionaryType>(
        keyType ? keyType : registry_.getAnyType(),
        valType ? valType : registry_.getAnyType());
}

void TypeChecker::visitWalrusExpr(AST::WalrusExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr valType = inferExprType(expr.value.get());
    declareVariable(expr.variable, valType);
    lastInferredType_ = valType;
}

void TypeChecker::visitLambdaExpr(AST::LambdaExpr& expr) {
    currentResult_.totalExpressions++;
    
    enterScope();
    
    // Register parameters and collect param types
    TypeList paramTypes;
    for (auto& param : expr.parameters) {
        TypePtr paramType = dataTypeToTypePtr(param.type);
        declareVariable(param.name, paramType);
        paramTypes.push_back(paramType);
    }
    
    // Infer body type
    TypePtr bodyType = inferExprType(expr.body.get());
    
    exitScope();
    
    // (AR) إنشاء نوع الدالة مع المعاملات ونوع الإرجاع
    lastInferredType_ = std::make_shared<FunctionType>(
        std::move(paramTypes), bodyType ? bodyType : registry_.getVoidType());
}

void TypeChecker::visitListComprehensionExpr(AST::ListComprehensionExpr& expr) {
    currentResult_.totalExpressions++;
    
    enterScope();
    TypePtr iterType = inferExprType(expr.iterable.get());
    declareVariable(expr.variable, registry_.getUnknownType());
    TypePtr elemType = inferExprType(expr.element.get());
    if (expr.condition) inferExprType(expr.condition.get());
    exitScope();
    
    // (AR) نوع المصفوفة الناتجة من الاستيعاب
    lastInferredType_ = std::make_shared<ArrayType>(elemType ? elemType : registry_.getAnyType());
}

void TypeChecker::visitDictComprehensionExpr(AST::DictComprehensionExpr& expr) {
    currentResult_.totalExpressions++;
    
    enterScope();
    inferExprType(expr.iterable.get());
    declareVariable(expr.variable, registry_.getUnknownType());
    TypePtr keyType = inferExprType(expr.key.get());
    TypePtr valType = inferExprType(expr.value.get());
    if (expr.condition) inferExprType(expr.condition.get());
    exitScope();
    
    // (AR) نوع القاموس الناتج من الاستيعاب
    lastInferredType_ = std::make_shared<DictionaryType>(
        keyType ? keyType : registry_.getAnyType(),
        valType ? valType : registry_.getAnyType());
}

void TypeChecker::visitSetComprehensionExpr(AST::SetComprehensionExpr& expr) {
    currentResult_.totalExpressions++;
    
    enterScope();
    inferExprType(expr.iterable.get());
    declareVariable(expr.variable, registry_.getUnknownType());
    TypePtr setElemType = inferExprType(expr.expression.get());
    if (expr.condition) inferExprType(expr.condition.get());
    exitScope();
    
    // (AR) نوع المجموعة الناتجة من الاستيعاب
    lastInferredType_ = std::make_shared<ArrayType>(setElemType ? setElemType : registry_.getAnyType());
}

void TypeChecker::visitGeneratorExpr(AST::GeneratorExpr& expr) {
    currentResult_.totalExpressions++;
    
    enterScope();
    inferExprType(expr.iterable.get());
    declareVariable(expr.variable, registry_.getUnknownType());
    inferExprType(expr.element.get());
    if (expr.condition) inferExprType(expr.condition.get());
    exitScope();
    
    lastInferredType_ = registry_.getAnyType();
}

void TypeChecker::visitDecoratorExpr(AST::DecoratorExpr& expr) {
    currentResult_.totalExpressions++;
    for (auto& arg : expr.arguments) {
        if (arg) inferExprType(arg.get());
    }
    lastInferredType_ = registry_.getUnknownType();
}

// ============================================================================
// OOP Expressions / تعابير الكائنات
// ============================================================================

void TypeChecker::visitNewExpr(AST::NewExpr& expr) {
    currentResult_.totalExpressions++;
    for (auto& arg : expr.arguments) {
        if (arg) inferExprType(arg.get());
    }
    // (AR) البحث عن نوع الصنف في StructRegistry
    auto structType = StructRegistry::instance().findStruct(expr.className);
    if (structType) {
        // (AR) الصنف موجود — إرجاع نوع Class
        lastInferredType_ = registry_.internPrimitiveType(TypeKind::Class);
    } else {
        // (AR) صنف غير معروف — تحذير
        lastInferredType_ = registry_.getUnknownType();
    }
}

void TypeChecker::visitMemberAccessExpr(AST::MemberAccessExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr objType = inferExprType(expr.object.get());
    
    // (AR) البحث عن نوع الحقل من StructRegistry / (EN) Look up field type from StructRegistry
    if (objType && (objType->getKind() == TypeKind::Class)) {
        // (AR) محاولة تحديد اسم الصنف / (EN) Try to identify class name
        std::string className;
        if (auto* newExpr = dynamic_cast<AST::NewExpr*>(expr.object.get())) {
            className = newExpr->className;
        } else if (auto* thisExpr = dynamic_cast<AST::ThisExpr*>(expr.object.get())) {
            className = currentFunction_;
        }
        if (!className.empty()) {
            auto structType = StructRegistry::instance().findStruct(className);
            if (structType) {
                auto field = structType->findField(expr.memberName);
                if (field && field->getType()) {
                    lastInferredType_ = field->getType();
                    return;
                }
            }
        }
    }
    // (AR) خصائص النص / (EN) String properties
    if (objType && objType->isString()) {
        if (expr.memberName == "الطول" || expr.memberName == "length") {
            lastInferredType_ = registry_.getIntegerType();
            return;
        }
    }
    lastInferredType_ = registry_.getUnknownType();
}

void TypeChecker::visitMethodCallExpr(AST::MethodCallExpr& expr) {
    currentResult_.totalExpressions++;
    TypePtr objType = inferExprType(expr.object.get());
    for (auto& arg : expr.arguments) {
        if (arg) inferExprType(arg.get());
    }
    // (AR) استنتاج نوع الإرجاع من الطريقة / (EN) Infer return type from method
    // (AR) طرق النص تُرجع أنواعاً معروفة / (EN) String methods return known types
    if (objType && objType->isString()) {
        const std::string& method = expr.methodName;
        if (method == "الطول" || method == "length") {
            lastInferredType_ = registry_.getIntegerType();
            return;
        }
        if (method == "يحتوي" || method == "contains" ||
            method == "يبدأ_بـ" || method == "startsWith" ||
            method == "ينتهي_بـ" || method == "endsWith") {
            lastInferredType_ = registry_.getBooleanType();
            return;
        }
        if (method == "قطع" || method == "slice" ||
            method == "استبدل" || method == "replace" ||
            method == "حروف_كبيرة" || method == "toUpperCase" ||
            method == "حروف_صغيرة" || method == "toLowerCase") {
            lastInferredType_ = registry_.getStringType();
            return;
        }
    }
    // (AR) طرق المصفوفة / (EN) Array methods
    if (objType && objType->isArray()) {
        const std::string& method = expr.methodName;
        if (method == "الطول" || method == "length" || method == "حجم" || method == "size") {
            lastInferredType_ = registry_.getIntegerType();
            return;
        }
        if (method == "أضف" || method == "push" || method == "ادفع") {
            lastInferredType_ = registry_.getVoidType();
            return;
        }
    }
    lastInferredType_ = registry_.getUnknownType();
}

void TypeChecker::visitThisExpr(AST::ThisExpr& expr) {
    currentResult_.totalExpressions++;
    // (AR) البحث عن نوع الصنف الحالي
    if (!currentFunction_.empty()) {
        auto classType = StructRegistry::instance().findStruct(currentFunction_);
        if (classType) {
            lastInferredType_ = registry_.internPrimitiveType(TypeKind::Class);
            return;
        }
    }
    lastInferredType_ = registry_.getUnknownType();
}

void TypeChecker::visitSuperExpr(AST::SuperExpr& expr) {
    currentResult_.totalExpressions++;
    // (AR) super يشير للصنف الأب — يعتمد على السياق
    lastInferredType_ = registry_.getUnknownType();
}

void TypeChecker::visitBorrowExpr(AST::BorrowExpr& expr) {
    currentResult_.totalExpressions++;
    auto type = lookupVariable(expr.variableName);
    lastInferredType_ = type ? type : registry_.getUnknownType();
}

void TypeChecker::visitInlineAsmExpr(AST::InlineAsmExpr& expr) {
    currentResult_.totalExpressions++;
    lastInferredType_ = registry_.getUnknownType();
}

void TypeChecker::visitRangeExpr(AST::RangeExpr& expr) {
    // (AR) تعبير النطاق 1..10 — ينتج مصفوفة أرقام
    // (EN) Range expression 1..10 — produces an array of integers
    currentResult_.totalExpressions++;
    if (expr.start) inferExprType(expr.start.get());
    if (expr.end) inferExprType(expr.end.get());
    lastInferredType_ = registry_.getUnknownType();
}

// ============================================================================
// زيارة العبارات / Visit Statements
// ============================================================================

void TypeChecker::visitExprStmt(AST::ExprStmt& stmt) {
    if (stmt.expression) {
        inferExprType(stmt.expression.get());
    }
}

void TypeChecker::visitVarDeclStmt(AST::VarDeclStmt& stmt) {
    currentResult_.totalVariables++;
    
    // النوع المُصرّح / Declared type
    TypePtr declaredType = dataTypeToTypePtr(stmt.type);
    
    // النوع المُستنتج من القيمة / Inferred type from initializer
    TypePtr initType = nullptr;
    if (stmt.initializer) {
        initType = inferExprType(stmt.initializer.get());
    }
    
    // تسجيل نوع المتغير
    TypePtr finalType = declaredType;
    
    // إذا كان النوع المُصرّح unknown، استخدم المُستنتج
    if (declaredType && declaredType->isUnknown() && initType && !initType->isUnknown()) {
        finalType = initType;
        currentResult_.totalInferred++;
    }
    
    // التحقق من التوافق / Check compatibility
    if (initType && declaredType && !declaredType->isUnknown() && !initType->isUnknown()) {
        if (!areTypesCompatible(declaredType, initType)) {
            recordTypeError(stmt.name,
                declaredType->toString(), initType->toString(),
                &stmt);
        }
    }
    
    // تسجيل في البيئة / Register in environment
    declareVariable(stmt.name, finalType);
    
    if (debugMode_) {
        std::cerr << "  [TC] VarDecl '" << stmt.name << "': "
                  << (finalType ? finalType->toString() : "unknown")
                  << (stmt.isConst ? " (const)" : "") << "\n";
    }
}

void TypeChecker::visitIfStmt(AST::IfStmt& stmt) {
    // تحقق من نوع الشرط / Check condition type
    if (stmt.condition) {
        TypePtr condType = inferExprType(stmt.condition.get());
        if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown()) {
            recordTypeError("", "boolean", condType->toString(), &stmt,
                "If condition should be boolean");
        }
    }
    
    // فحص الفرعين / Check both branches
    if (stmt.thenBranch) {
        enterScope();
        stmt.thenBranch->accept(*this);
        exitScope();
    }
    if (stmt.elseBranch) {
        enterScope();
        stmt.elseBranch->accept(*this);
        exitScope();
    }
}

void TypeChecker::visitWhileStmt(AST::WhileStmt& stmt) {
    if (stmt.condition) {
        TypePtr condType = inferExprType(stmt.condition.get());
        if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown()) {
            recordTypeError("", "boolean", condType->toString(), &stmt,
                "While condition should be boolean");
        }
    }
    
    if (stmt.body) {
        enterScope();
        stmt.body->accept(*this);
        exitScope();
    }
}

void TypeChecker::visitForStmt(AST::ForStmt& stmt) {
    enterScope();
    if (stmt.initializer) stmt.initializer->accept(*this);
    if (stmt.condition) inferExprType(stmt.condition.get());
    if (stmt.increment) inferExprType(stmt.increment.get());
    if (stmt.body) stmt.body->accept(*this);
    exitScope();
}

void TypeChecker::visitForRangeStmt(AST::ForRangeStmt& stmt) {
    enterScope();
    
    if (stmt.iterable) {
        inferExprType(stmt.iterable.get());
    }
    
    // تسجيل متغير الحلقة / Register loop variable
    declareVariable(stmt.variable, registry_.getUnknownType());
    if (!stmt.valueVar.empty()) {
        declareVariable(stmt.valueVar, registry_.getUnknownType());
    }
    
    if (stmt.body) stmt.body->accept(*this);
    exitScope();
}

void TypeChecker::visitSwitchStmt(AST::SwitchStmt& stmt) {
    TypePtr switchType = nullptr;
    if (stmt.expression) {
        switchType = inferExprType(stmt.expression.get());
    }
    
    for (auto& caseBranch : stmt.cases) {
        if (caseBranch.value) {
            TypePtr caseType = inferExprType(caseBranch.value.get());
            if (switchType && caseType && !areTypesCompatible(switchType, caseType)) {
                recordTypeError("", switchType->toString(), caseType->toString(),
                    caseBranch.value.get(),
                    "Switch case type mismatch");
            }
        }
        if (caseBranch.body) {
            enterScope();
            caseBranch.body->accept(*this);
            exitScope();
        }
    }
    
    if (stmt.defaultCase) {
        enterScope();
        stmt.defaultCase->accept(*this);
        exitScope();
    }
}

void TypeChecker::visitReturnStmt(AST::ReturnStmt& stmt) {
    TypePtr retType = registry_.getVoidType();
    if (stmt.value) {
        retType = inferExprType(stmt.value.get());
    }
    
    // تحقق من توافق نوع الإرجاع / Check return type compatibility
    if (expectedReturnType_ && retType) {
        if (!areTypesCompatible(expectedReturnType_, retType)) {
            recordTypeError(currentFunction_,
                expectedReturnType_->toString(), retType->toString(),
                &stmt,
                "Return type mismatch in function '" + currentFunction_ + "'");
        }
    }
}

void TypeChecker::visitYieldStmt(AST::YieldStmt& stmt) {
    if (stmt.value) {
        inferExprType(stmt.value.get());
    }
}

void TypeChecker::visitBreakStmt(AST::BreakStmt& /*stmt*/) {
    // لا فحص أنواع / No type checking needed
}

void TypeChecker::visitContinueStmt(AST::ContinueStmt& /*stmt*/) {
    // لا فحص أنواع / No type checking needed
}

void TypeChecker::visitBlockStmt(AST::BlockStmt& stmt) {
    enterScope();
    for (auto& s : stmt.statements) {
        if (s) s->accept(*this);
    }
    exitScope();
}

void TypeChecker::visitTryStmt(AST::TryStmt& stmt) {
    if (stmt.tryBlock) {
        enterScope();
        stmt.tryBlock->accept(*this);
        exitScope();
    }
    
    for (auto& clause : stmt.catchClauses) {
        enterScope();
        if (!clause.exceptionVar.empty()) {
            declareVariable(clause.exceptionVar, dataTypeToTypePtr(clause.exceptionType));
        }
        if (clause.body) clause.body->accept(*this);
        exitScope();
    }
    
    if (stmt.finallyBlock) {
        enterScope();
        stmt.finallyBlock->accept(*this);
        exitScope();
    }
}

void TypeChecker::visitRaiseStmt(AST::RaiseStmt& stmt) {
    if (stmt.exception) {
        inferExprType(stmt.exception.get());
    }
}

void TypeChecker::visitWithStmt(AST::WithStmt& stmt) {
    enterScope();
    if (stmt.resource) {
        TypePtr resType = inferExprType(stmt.resource.get());
        if (!stmt.alias.empty()) {
            declareVariable(stmt.alias, resType);
        }
    }
    if (stmt.body) stmt.body->accept(*this);
    exitScope();
}

void TypeChecker::visitClassDeclStmt(AST::ClassDeclStmt& stmt) {
    enterScope();
    
    // تسجيل الحقول / Register fields
    for (auto& field : stmt.fields) {
        if (field) field->accept(*this);
    }
    
    // فحص المُنشئ / Check constructor
    if (stmt.constructor) {
        stmt.constructor->accept(*this);
    }
    
    // فحص المُدمّر / Check destructor
    if (stmt.destructor) {
        stmt.destructor->accept(*this);
    }
    
    // فحص الدوال / Check methods
    for (auto& method : stmt.methods) {
        if (method) method->accept(*this);
    }
    
    exitScope();
}

// ============================================================================
// زيارة التصريحات / Visit Declarations
// ============================================================================

void TypeChecker::visitFunctionDecl(AST::FunctionDecl& decl) {
    currentResult_.totalFunctions++;
    
    std::string prevFunction = currentFunction_;
    TypePtr prevReturnType = expectedReturnType_;
    
    currentFunction_ = decl.name;
    // (AR) إذا كان نوع الإرجاع غير معروف أو NONE
    // (EN) If return type is UNKNOWN or NONE
    if (decl.returnType == Data::DataType::UNKNOWN ||
        decl.returnType == Data::DataType::NONE) {
        if (decl.isExtern) {
            // (AR) الدوال الخارجية بدون نوع إرجاع تُفترض رقم (I64)
            // (EN) Extern functions without return type default to integer
            expectedReturnType_ = registry_.getIntegerType();
        } else {
            expectedReturnType_ = nullptr;
        }
    } else {
        expectedReturnType_ = dataTypeToTypePtr(decl.returnType);
    }
    
    // (AR) تسجيل الدالة بنوع دالة كامل (معاملات + إرجاع)
    TypeList paramTypes;
    for (auto& param : decl.parameters) {
        paramTypes.push_back(dataTypeToTypePtr(param.type));
    }
    auto fnType = std::make_shared<FunctionType>(std::move(paramTypes), expectedReturnType_);
    declareVariable(decl.name, fnType);
    
    enterScope();
    
    // تسجيل المعاملات / Register parameters
    for (auto& param : decl.parameters) {
        TypePtr paramType = dataTypeToTypePtr(param.type);
        declareVariable(param.name, paramType);
    }
    
    // فحص جسم الدالة / Check function body
    if (decl.body) {
        decl.body->accept(*this);
    }
    
    exitScope();
    
    currentFunction_ = prevFunction;
    expectedReturnType_ = prevReturnType;
    
    if (debugMode_) {
        std::cerr << "  [TC] Function '" << decl.name << "' -> "
                  << (expectedReturnType_ ? expectedReturnType_->toString() : "void") << "\n";
    }
}

void TypeChecker::visitClassDecl(AST::ClassDecl& decl) {
    enterScope();
    
    // (AR) التحقق من وجود الصنف في StructRegistry
    bool isKnown = StructRegistry::instance().isStructType(decl.name);
    declareVariable(decl.name, isKnown 
        ? registry_.internPrimitiveType(TypeKind::Class) 
        : registry_.getUnknownType());
    
    // فحص الأعضاء / Check members
    for (auto& member : decl.members) {
        if (member) member->accept(*this);
    }
    
    exitScope();
}

void TypeChecker::visitFieldDecl(AST::FieldDecl& decl) {
    TypePtr fieldType = dataTypeToTypePtr(decl.type);
    declareVariable(decl.name, fieldType);
    
    if (decl.initializer) {
        TypePtr initType = inferExprType(decl.initializer.get());
        if (initType && fieldType && !areTypesCompatible(fieldType, initType)) {
            recordTypeError(decl.name,
                fieldType->toString(), initType->toString(),
                &decl, "Field initializer type mismatch");
        }
    }
}

void TypeChecker::visitMethodDecl(AST::MethodDecl& decl) {
    currentResult_.totalFunctions++;
    
    std::string prevFunction = currentFunction_;
    TypePtr prevReturnType = expectedReturnType_;
    
    currentFunction_ = decl.name;
    // (AR) إذا كان نوع الإرجاع غير معروف أو NONE، لا نفحص نوع return
    // (EN) If return type is UNKNOWN or NONE, skip return type checking
    if (decl.returnType == Data::DataType::UNKNOWN ||
        decl.returnType == Data::DataType::NONE) {
        expectedReturnType_ = nullptr;
    } else {
        expectedReturnType_ = dataTypeToTypePtr(decl.returnType);
    }
    
    enterScope();
    
    for (auto& param : decl.parameters) {
        declareVariable(param.name, dataTypeToTypePtr(param.type));
    }
    
    if (decl.body) decl.body->accept(*this);
    
    exitScope();
    
    currentFunction_ = prevFunction;
    expectedReturnType_ = prevReturnType;
}

void TypeChecker::visitPropertyDecl(AST::PropertyDecl& decl) {
    // PropertyDecl may not have standard fields we can check
    // Just visit children
    (void)decl;
}

void TypeChecker::visitConstructorDecl(AST::ConstructorDecl& decl) {
    currentResult_.totalFunctions++;
    
    enterScope();
    for (auto& param : decl.parameters) {
        declareVariable(param.name, dataTypeToTypePtr(param.type));
    }
    if (decl.body) decl.body->accept(*this);
    exitScope();
}

void TypeChecker::visitDestructorDecl(AST::DestructorDecl& decl) {
    enterScope();
    if (decl.body) decl.body->accept(*this);
    exitScope();
}

void TypeChecker::visitEnumDecl(AST::EnumDecl& decl) {
    // (AR) التحقق من وجود التعداد في EnumRegistry
    bool isKnown = EnumRegistry::instance().isEnumType(decl.name);
    declareVariable(decl.name, isKnown
        ? registry_.internPrimitiveType(TypeKind::Class)  // استخدام Class كبديل لحين يتوفر EnumKind
        : registry_.getUnknownType());
    
    for (auto& member : decl.members) {
        if (member.value) {
            inferExprType(member.value.get());
        }
    }
}

void TypeChecker::visitImportStmt(AST::ImportStmt& stmt) {
    // (AR) تحميل أنواع الوحدة — تسجيل اسم الوحدة كمتغير من نوع Module
    // (EN) Module type loading — register module name as Module-typed variable
    if (!stmt.modulePath.empty()) {
        std::string effectiveName = stmt.getEffectiveName();
        
        // (AR) تسجيل الوحدة كنطاق اسم — كل أعضائها Any حتى يتم ربط الوحدات
        // (EN) Register module as namespace — members are Any until module linking
        declareVariable(effectiveName, registry_.getAnyType());
        
        // (AR) تسجيل مسار الوحدة لمرحلة الربط في SIR
        // (EN) Record module path for SIR linking phase
        currentResult_.moduleDependencies.push_back(stmt.getFullModuleName());
    }
}

void TypeChecker::visitFromImportStmt(AST::FromImportStmt& stmt) {
    // (AR) تسجيل كل رمز مستورد مع نوعه المبدئي
    // (EN) Register each imported symbol with its initial type
    
    // (AR) تسجيل اعتماد الوحدة
    currentResult_.moduleDependencies.push_back(stmt.getFullModuleName());
    
    for (const auto& item : stmt.items) {
        std::string name = item.getEffectiveName();
        // (AR) كل رمز مستورد يبدأ كـ Any — سيُحدَّث عند ربط الوحدات
        declareVariable(name, registry_.getAnyType());
    }
}

void TypeChecker::visitExportStmt(AST::ExportStmt& stmt) {
    if (stmt.declaration) stmt.declaration->accept(*this);
}

void TypeChecker::visitExportDecl(AST::ExportDecl& decl) {
    (void)decl;
}

// ============================================================================
// المعممات / Templates & Generics
// ============================================================================

void TypeChecker::visitTemplateFunctionDecl(AST::TemplateFunctionDecl& decl) {
    currentResult_.totalFunctions++;
    
    enterScope();
    
    // (AR) تسجيل معاملات النوع كمتغيرات نوع معممة
    for (auto& tp : decl.typeParameters) {
        declareVariable(tp.name, registry_.getUnknownType());
    }
    
    // تسجيل معاملات الدالة / Register function parameters
    for (auto& param : decl.parameters) {
        declareVariable(param.name, dataTypeToTypePtr(param.type));
    }
    
    if (decl.body) decl.body->accept(*this);
    
    exitScope();
    
    // تسجيل الدالة المعممة / Register generic function
    declareVariable(decl.name, dataTypeToTypePtr(decl.returnType));
    
    if (debugMode_) {
        std::cerr << "  [TC] TemplateFunction '" << decl.name << "' with "
                  << decl.typeParameters.size() << " type params\n";
    }
}

void TypeChecker::visitTemplateClassDecl(AST::TemplateClassDecl& decl) {
    enterScope();
    
    for (auto& tp : decl.typeParameters) {
        declareVariable(tp.name, registry_.getUnknownType());
    }
    
    for (auto& member : decl.members) {
        if (member) member->accept(*this);
    }
    
    exitScope();
    
    // (AR) تسجيل الصنف المعمم — البحث في StructRegistry
    bool isKnown = StructRegistry::instance().isStructType(decl.name);
    declareVariable(decl.name, isKnown
        ? registry_.internPrimitiveType(TypeKind::Class)
        : registry_.getUnknownType());
}

void TypeChecker::visitTemplateInstantiation(AST::TemplateInstantiation& inst) {
    currentResult_.totalExpressions++;
    
    // (AR) البحث عن القالب والتحقق من عدد معاملات الأنواع
    // (EN) Look up template and validate type argument count
    auto type = lookupVariable(inst.templateName);
    
    if (!type) {
        // (AR) قالب غير معرّف
        // (EN) Template not defined
        TypeCheckError err;
        err.variableName = inst.templateName;
        err.line = inst.position.line;
        err.column = inst.position.column;
        err.message = "Undefined template: '" + inst.templateName + "'";
        err.arabicMessage = "قالب غير معرّف: '" + inst.templateName + "'";
        currentResult_.addError(err);
        lastInferredType_ = registry_.getUnknownType();
    } else {
        // (AR) التحقق من أنه ليس فارغًا من معاملات الأنواع
        // (EN) Ensure type arguments are provided
        if (inst.typeArguments.empty()) {
            TypeCheckError err;
            err.variableName = inst.templateName;
            err.line = inst.position.line;
            err.column = inst.position.column;
            err.message = "Empty type arguments for template: '" + inst.templateName + "'";
            err.arabicMessage = "معاملات أنواع فارغة للقالب: '" + inst.templateName + "'";
            currentResult_.addError(err);
        }
        lastInferredType_ = type;
    }
    
    if (debugMode_) {
        std::cerr << "  [TC] TemplateInstantiation '" << inst.templateName
                  << "' with " << inst.typeArguments.size() << " type args\n";
    }
}

void TypeChecker::visitNamespaceDecl(AST::NamespaceDecl& decl) {
    enterScope();
    for (auto& member : decl.members) {
        if (member) member->accept(*this);
    }
    exitScope();
}

void TypeChecker::visitOperatorDecl(AST::OperatorDecl& decl) {
    currentResult_.totalFunctions++;
    
    enterScope();
    for (auto& param : decl.parameters) {
        declareVariable(param.name, dataTypeToTypePtr(param.type));
    }
    if (decl.body) decl.body->accept(*this);
    exitScope();
}

} // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
