/*
 * ============================================================================
 * Type Inference - Implementation / تنفيذ استنتاج الأنواع
 * Type Inference - Implementation
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include "../include/type_inference.h"
#include "../../../../include/lexer/token.h"
#include <sstream>
#include <algorithm>

namespace Sad {
namespace TypeChecker {

// ============================================================================
// Static Variables / المتغيرات الساكنة
// ============================================================================

int TypeVariable::nextId_ = 0; // تهيئة العداد / Initialize counter

// ============================================================================
// TypeSubstitution Implementation / تنفيذ TypeSubstitution
// ============================================================================

/**
 * تطبيق الاستبدال على نوع / Apply substitution to type
 */
std::shared_ptr<Type> TypeSubstitution::apply(std::shared_ptr<Type> type) const {
    if (!type) {
        return nullptr; // نوع null / null type
    }
    
    // إذا كان متغير نوع / If type variable
    auto typeVar = std::dynamic_pointer_cast<TypeVariable>(type);
    if (typeVar) {
        // إذا كان مربوطاً بالفعل، أرجع النوع المربوط / If already bound, return bound type
        if (typeVar->isBound()) {
            return typeVar->resolve();
        }
        
        // إذا كان لدينا استبدال له / If we have substitution for it
        auto subst = get(typeVar->getId());
        if (subst) {
            return apply(subst); // تطبيق الاستبدال بشكل متكرر / Apply recursively
        }
        
        return type; // لا يوجد استبدال / No substitution
    }
    
    // إذا كان نوع مركب (Array, Dict, Function...) / If compound type
    auto arrayType = std::dynamic_pointer_cast<ArrayType>(type);
    if (arrayType) {
        auto newElemType = apply(arrayType->getElementType());
        return TypeFactory::createArrayType(newElemType, arrayType->getSize());
    }
    
    auto dictType = std::dynamic_pointer_cast<DictType>(type);
    if (dictType) {
        auto newKeyType = apply(dictType->getKeyType());
        auto newValueType = apply(dictType->getValueType());
        return TypeFactory::createDictType(newKeyType, newValueType);
    }
    
    auto funcType = std::dynamic_pointer_cast<FunctionType>(type);
    if (funcType) {
        std::vector<std::shared_ptr<Type>> newParamTypes;
        for (auto& paramType : funcType->getParamTypes()) {
            newParamTypes.push_back(apply(paramType));
        }
        auto newReturnType = apply(funcType->getReturnType());
        return TypeFactory::createFunctionType(newParamTypes, newReturnType, funcType->isVariadic());
    }
    
    // أنواع أخرى لا تحتاج استبدال / Other types don't need substitution
    return type;
}

/**
 * دمج استبدالين / Compose two substitutions
 */
TypeSubstitution TypeSubstitution::compose(const TypeSubstitution& other) const {
    TypeSubstitution result;
    
    // إضافة استبدالات الأول بعد تطبيق الثاني / Add first substitutions after applying second
    for (const auto& [varId, type] : substitutions_) {
        result.add(varId, other.apply(type));
    }
    
    // إضافة استبدالات الثاني / Add second substitutions
    for (const auto& [varId, type] : other.substitutions_) {
        if (!result.has(varId)) {
            result.add(varId, type);
        }
    }
    
    return result;
}

/**
 * تحويل لنص / Convert to string
 */
std::string TypeSubstitution::toString() const {
    if (substitutions_.empty()) {
        return "[]"; // استبدال فارغ / Empty substitution
    }
    
    std::stringstream ss;
    ss << "[";
    
    bool first = true;
    for (const auto& [varId, type] : substitutions_) {
        if (!first) ss << ", ";
        first = false;
        
        ss << "α" << varId << " → " << type->toString();
    }
    
    ss << "]";
    return ss.str();
}

// ============================================================================
// TypeInference Implementation / تنفيذ TypeInference
// ============================================================================

/**
 * استنتاج نوع تعبير / Infer expression type
 */
std::shared_ptr<Type> TypeInference::inferExpr(AST::Expression* expr) {
    if (!expr) {
        return TypeFactory::getVoidType(); // تعبير null / null expression
    }
    
    // حسب نوع التعبير / Based on expression type using dynamic_cast
    
    // Check for literals
    if (auto* litExpr = dynamic_cast<AST::LiteralExpr*>(expr)) {
        return inferLiteral(expr);
    }
    
    // Binary expressions
    if (auto* binExpr = dynamic_cast<AST::BinaryExpr*>(expr)) {
        return inferBinaryOp(binExpr);
    }
    
    // Unary expressions
    if (auto* unaryExpr = dynamic_cast<AST::UnaryExpr*>(expr)) {
        return inferUnaryOp(unaryExpr);
    }
    
    // Variable references
    if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(expr)) {
        return inferVariable(varExpr);
    }
    
    // Function calls
    if (auto* callExpr = dynamic_cast<AST::CallExpr*>(expr)) {
        return inferFunctionCall(callExpr);
    }
    
    // Member access
    if (auto* memberExpr = dynamic_cast<AST::MemberExpr*>(expr)) {
        return inferMemberAccess(memberExpr);
    }
    
    // Array/index access
    if (auto* indexExpr = dynamic_cast<AST::IndexExpr*>(expr)) {
        return inferArrayAccess(indexExpr);
    }
    
    // Lambda expressions
    if (auto* lambdaExpr = dynamic_cast<AST::LambdaExpr*>(expr)) {
        return inferLambda(lambdaExpr);
    }
    
    // نوع غير معروف / Unknown type
    context_->addWarning("نوع تعبير غير مدعوم في Type Inference");
    return freshTypeVariable();
}

/**
 * استنتاج نوع حرفي / Infer literal type
 */
std::shared_ptr<Type> TypeInference::inferLiteral(AST::Expression* expr) {
    // الحرفيات لها أنواع مباشرة / Literals have direct types
    auto* litExpr = dynamic_cast<AST::LiteralExpr*>(expr);
    if (!litExpr) {
        return TypeFactory::getVoidType();
    }
    
    // Check token type
    auto tokenType = litExpr->token.getType();
    if (tokenType == Sad::Lexer::TokenType::NUMBER_INTEGER) {
        return TypeFactory::getIntType();
    } else if (tokenType == Sad::Lexer::TokenType::NUMBER_DOUBLE) {
        return TypeFactory::getFloatType();
    } else if (tokenType == Sad::Lexer::TokenType::STRING_LITERAL ||
               tokenType == Sad::Lexer::TokenType::STRING_RAW ||
               tokenType == Sad::Lexer::TokenType::STRING_FSTRING) {
        return TypeFactory::getStringType();
    } else if (tokenType == Sad::Lexer::TokenType::LITERAL_TRUE ||
               tokenType == Sad::Lexer::TokenType::LITERAL_FALSE) {
        return TypeFactory::getBoolType();
    } else {
        return TypeFactory::getVoidType();
    }
}

/**
 * استنتاج نوع عملية ثنائية / Infer binary operation type
 */
std::shared_ptr<Type> TypeInference::inferBinaryOp(AST::BinaryExpr* expr) {
    if (!expr) {
        return freshTypeVariable();
    }
    
    // استنتاج نوع اليمين واليسار / Infer left and right types
    auto leftType = inferExpr(expr->left.get());
    auto rightType = inferExpr(expr->right.get());
    
    // حسب نوع العملية / Based on operation type
    auto op = Lexer::Token::typeToString(expr->op);
    
    // عمليات حسابية: +, -, *, /, % / Arithmetic operations
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        // كلا الطرفين يجب أن يكونا رقميين / Both sides must be numeric
        auto numericType = TypeFactory::getIntType(); // افتراضي int / Default int
        
        // إنشاء قيود / Create constraints
        constraints_.push_back(Constraint(
            Constraint::Kind::EQUALITY,
            leftType,
            numericType,
            expr
        ));
        
        constraints_.push_back(Constraint(
            Constraint::Kind::EQUALITY,
            rightType,
            numericType,
            expr
        ));
        
        // النتيجة نفس النوع / Result is same type
        return numericType;
    }
    
    // عمليات مقارنة: ==, !=, <, >, <=, >= / Comparison operations
    if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
        // الطرفان يجب أن يكونا من نفس النوع / Both sides must be same type
        constraints_.push_back(Constraint(
            Constraint::Kind::EQUALITY,
            leftType,
            rightType,
            expr
        ));
        
        // النتيجة bool / Result is bool
        return TypeFactory::getBoolType();
    }
    
    // عمليات منطقية: &&, ||, and, or / Logical operations
    if (op == "&&" || op == "||" || op == "and" || op == "or") {
        auto boolType = TypeFactory::getBoolType();
        
        // كلا الطرفين يجب أن يكونا bool / Both sides must be bool
        constraints_.push_back(Constraint(
            Constraint::Kind::EQUALITY,
            leftType,
            boolType,
            expr
        ));
        
        constraints_.push_back(Constraint(
            Constraint::Kind::EQUALITY,
            rightType,
            boolType,
            expr
        ));
        
        return boolType;
    }
    
    // عملية غير معروفة / Unknown operation
    context_->addWarning("عملية ثنائية غير معروفة: " + op);
    return freshTypeVariable();
}

/**
 * استنتاج نوع عملية أحادية / Infer unary operation type
 */
std::shared_ptr<Type> TypeInference::inferUnaryOp(AST::UnaryExpr* expr) {
    if (!expr) {
        return freshTypeVariable();
    }
    
    // استنتاج نوع المعامل / Infer operand type
    auto operandType = inferExpr(expr->operand.get());
    
    auto op = Lexer::Token::typeToString(expr->op);
    
    // عملية النفي المنطقي: !, not / Logical negation
    if (op == "!" || op == "not") {
        auto boolType = TypeFactory::getBoolType();
        
        // المعامل يجب أن يكون bool / Operand must be bool
        constraints_.push_back(Constraint(
            Constraint::Kind::EQUALITY,
            operandType,
            boolType,
            expr
        ));
        
        return boolType;
    }
    
    // عملية النفي السالب: -, + / Numeric negation
    if (op == "-" || op == "+") {
        auto numericType = TypeFactory::getIntType();
        
        // المعامل يجب أن يكون رقمياً / Operand must be numeric
        constraints_.push_back(Constraint(
            Constraint::Kind::EQUALITY,
            operandType,
            numericType,
            expr
        ));
        
        return numericType;
    }
    
    // عمليات الزيادة/النقصان: ++, -- / Increment/decrement
    if (op == "++" || op == "--") {
        auto intType = TypeFactory::getIntType();
        
        // المعامل يجب أن يكون int / Operand must be int
        constraints_.push_back(Constraint(
            Constraint::Kind::EQUALITY,
            operandType,
            intType,
            expr
        ));
        
        return intType;
    }
    
    // عملية غير معروفة / Unknown operation
    context_->addWarning("عملية أحادية غير معروفة: " + op);
    return operandType; // أرجع نوع المعامل / Return operand type
}

/**
 * استنتاج نوع متغير / Infer variable type
 */
std::shared_ptr<Type> TypeInference::inferVariable(AST::VariableExpr* expr) {
    if (!expr) {
        return freshTypeVariable();
    }
    
    auto varName = expr->name;
    
    // البحث عن المتغير في البيئة / Look up variable in environment
    auto symbol = context_->getEnvironment()->lookupSymbol(varName);
    if (symbol) {
        return symbol->getType(); // أرجع نوع المتغير / Return variable type
    }
    
    // متغير غير معرّف / Undefined variable
    context_->addError("متغير غير معرّف: " + varName);
    return freshTypeVariable(); // أنشئ متغير نوع جديد / Create fresh type variable
}

/**
 * استنتاج نوع استدعاء دالة / Infer function call type
 */
std::shared_ptr<Type> TypeInference::inferFunctionCall(AST::CallExpr* expr) {
    if (!expr) {
        return freshTypeVariable();
    }
    
    // استنتاج نوع الدالة / Infer function type
    auto funcType = inferExpr(expr->callee.get());
    
    // استنتاج أنواع المعاملات / Infer argument types
    std::vector<std::shared_ptr<Type>> argTypes;
    for (auto& arg : expr->arguments) {
        argTypes.push_back(inferExpr(arg.get()));
    }
    
    // إنشاء متغير نوع للنتيجة / Create type variable for result
    auto returnType = freshTypeVariable();
    
    // إنشاء نوع دالة متوقع / Create expected function type
    auto expectedFuncType = TypeFactory::createFunctionType(argTypes, returnType, false);
    
    // إضافة قيد: نوع الدالة = نوع الدالة المتوقع / Add constraint: function type = expected function type
    constraints_.push_back(Constraint(
        Constraint::Kind::EQUALITY,
        funcType,
        expectedFuncType,
        expr
    ));
    
    return returnType; // أرجع نوع النتيجة / Return result type
}

/**
 * استنتاج نوع الوصول لعضو / Infer member access type
 */
std::shared_ptr<Type> TypeInference::inferMemberAccess(AST::MemberExpr* expr) {
    if (!expr) {
        return freshTypeVariable();
    }
    
    // استنتاج نوع الكائن / Infer object type
    auto objectType = inferExpr(expr->object.get());
    
    auto memberName = expr->member;
    
    // إذا كان الكائن من نوع ClassType / If object is ClassType
    auto classType = std::dynamic_pointer_cast<ClassType>(objectType);
    if (classType) {
        // البحث عن العضو / Look for member
        auto memberType = classType->getMemberType(memberName);
        if (memberType) {
            return memberType; // أرجع نوع العضو / Return member type
        }
        
        // عضو غير موجود / Member not found
        context_->addError("العضو '" + memberName + "' غير موجود في الصنف");
    }
    
    // نوع غير معروف / Unknown type
    auto resultType = freshTypeVariable();
    
    // إضافة قيد للوصول لعضو / Add member access constraint
    auto constraint = Constraint(
        Constraint::Kind::MEMBER_ACCESS,
        objectType,
        resultType,
        expr
    );
    constraint.setMemberName(memberName);
    constraints_.push_back(constraint);
    
    return resultType;
}

/**
 * استنتاج نوع الوصول لعنصر مصفوفة / Infer array access type
 */
std::shared_ptr<Type> TypeInference::inferArrayAccess(AST::IndexExpr* expr) {
    if (!expr) {
        return freshTypeVariable();
    }
    
    // استنتاج نوع المصفوفة / Infer array type
    auto arrayType = inferExpr(expr->object.get());
    
    // استنتاج نوع الفهرس / Infer index type
    auto indexType = inferExpr(expr->index.get());
    
    // الفهرس يجب أن يكون int / Index must be int
    constraints_.push_back(Constraint(
        Constraint::Kind::EQUALITY,
        indexType,
        TypeFactory::getIntType(),
        expr
    ));
    
    // إذا كان المصفوفة من نوع ArrayType / If array is ArrayType
    auto arrType = std::dynamic_pointer_cast<ArrayType>(arrayType);
    if (arrType) {
        return arrType->getElementType(); // أرجع نوع العنصر / Return element type
    }
    
    // إذا كان قاموس DictType / If dict is DictType
    auto dictType = std::dynamic_pointer_cast<DictType>(arrayType);
    if (dictType) {
        // الفهرس يجب أن يكون من نوع المفتاح / Index must be key type
        constraints_.push_back(Constraint(
            Constraint::Kind::EQUALITY,
            indexType,
            dictType->getKeyType(),
            expr
        ));
        
        return dictType->getValueType(); // أرجع نوع القيمة / Return value type
    }
    
    // نوع غير معروف - أنشئ متغير نوع / Unknown type - create type variable
    auto elementType = freshTypeVariable();
    
    // إضافة قيد: المصفوفة يجب أن تكون Array[elementType] / Add constraint
    auto expectedArrayType = TypeFactory::createArrayType(elementType);
    constraints_.push_back(Constraint(
        Constraint::Kind::EQUALITY,
        arrayType,
        expectedArrayType,
        expr
    ));
    
    return elementType;
}

/**
 * استنتاج نوع دالة lambda / Infer lambda type
 */
std::shared_ptr<Type> TypeInference::inferLambda(AST::LambdaExpr* expr) {
    if (!expr) {
        return freshTypeVariable();
    }
    
    // إنشاء متغيرات أنواع للمعاملات / Create type variables for parameters
    std::vector<std::shared_ptr<Type>> paramTypes;
    for (const auto& param : expr->parameters) {
        auto paramType = freshTypeVariable();
        paramTypes.push_back(paramType);
        
        // إضافة المعامل للبيئة / Add parameter to environment
        context_->getEnvironment()->addSymbol(
            param.name,
            paramType,
            Symbol::Kind::PARAMETER
        );
    }
    
    // دفع نطاق جديد للدالة / Push new scope for function
    context_->getEnvironment()->pushScope(Scope::Type::FUNCTION, "lambda");
    
    // استنتاج نوع جسم الدالة / Infer body type
    std::shared_ptr<Type> returnType;
    if (expr->body) {
        returnType = inferExpr(expr->body.get());
    } else {
        returnType = TypeFactory::getVoidType();
    }
    
    // سحب النطاق / Pop scope
    context_->getEnvironment()->popScope();
    
    // إنشاء نوع الدالة / Create function type
    return TypeFactory::createFunctionType(paramTypes, returnType, false);
}

/**
 * إنشاء متغير نوع جديد / Create fresh type variable
 */
std::shared_ptr<TypeVariable> TypeInference::freshTypeVariable() {
    freshVarCounter_++;
    return std::make_shared<TypeVariable>();
}

/**
 * حل القيود / Solve constraints
 */
bool TypeInference::solveConstraints() {
    // حل كل قيد / Solve each constraint
    for (auto& constraint : constraints_) {
        bool success = false;
        
        switch (constraint.getKind()) {
            case Constraint::Kind::EQUALITY:
                // توحيد الأنواع / Unify types
                success = unify(constraint.getType1(), constraint.getType2());
                break;
                
            case Constraint::Kind::SUBTYPE:
                // التحقق من النوع الفرعي / Check subtype
                success = constraint.getType1()->canCastTo(constraint.getType2().get());
                break;
                
            case Constraint::Kind::MEMBER_ACCESS:
            case Constraint::Kind::FUNCTION_CALL:
                // هذه القيود تُحل بطرق خاصة / These constraints solved specially
                success = true;
                break;
        }
        
        if (!success) {
            // فشل حل القيد / Failed to solve constraint
            context_->addError(
                "فشل حل القيد: " + constraint.toString()
            );
            return false;
        }
    }
    
    return true; // نجح حل جميع القيود / Successfully solved all constraints
}

/**
 * توحيد نوعين / Unify two types
 */
bool TypeInference::unify(std::shared_ptr<Type> type1, std::shared_ptr<Type> type2) {
    if (!type1 || !type2) {
        return false; // نوع null / null type
    }
    
    // تطبيق الاستبدالات الحالية / Apply current substitutions
    type1 = substitute(type1);
    type2 = substitute(type2);
    
    // إذا كانا متساويين / If equal
    if (type1->equals(type2.get())) {
        return true;
    }
    
    // إذا كان أحدهما متغير نوع / If one is type variable
    auto var1 = std::dynamic_pointer_cast<TypeVariable>(type1);
    auto var2 = std::dynamic_pointer_cast<TypeVariable>(type2);
    
    if (var1) {
        // التحقق من occurs check / Check occurs check
        if (occursIn(var1, type2)) {
            context_->addError("تعريف دائري في الأنواع");
            return false;
        }
        
        // ربط المتغير بالنوع / Bind variable to type
        var1->bind(type2);
        substitution_.add(var1->getId(), type2);
        return true;
    }
    
    if (var2) {
        // التحقق من occurs check / Check occurs check
        if (occursIn(var2, type1)) {
            context_->addError("تعريف دائري في الأنواع");
            return false;
        }
        
        // ربط المتغير بالنوع / Bind variable to type
        var2->bind(type1);
        substitution_.add(var2->getId(), type1);
        return true;
    }
    
    // إذا كانا أنواع مركبة / If compound types
    auto array1 = std::dynamic_pointer_cast<ArrayType>(type1);
    auto array2 = std::dynamic_pointer_cast<ArrayType>(type2);
    if (array1 && array2) {
        return unify(array1->getElementType(), array2->getElementType());
    }
    
    auto dict1 = std::dynamic_pointer_cast<DictType>(type1);
    auto dict2 = std::dynamic_pointer_cast<DictType>(type2);
    if (dict1 && dict2) {
        return unify(dict1->getKeyType(), dict2->getKeyType()) &&
               unify(dict1->getValueType(), dict2->getValueType());
    }
    
    auto func1 = std::dynamic_pointer_cast<FunctionType>(type1);
    auto func2 = std::dynamic_pointer_cast<FunctionType>(type2);
    if (func1 && func2) {
        // التحقق من عدد المعاملات / Check parameter count
        if (func1->getParamTypes().size() != func2->getParamTypes().size()) {
            return false;
        }
        
        // توحيد أنواع المعاملات / Unify parameter types
        for (size_t i = 0; i < func1->getParamTypes().size(); ++i) {
            if (!unify(func1->getParamTypes()[i], func2->getParamTypes()[i])) {
                return false;
            }
        }
        
        // توحيد نوع النتيجة / Unify return type
        return unify(func1->getReturnType(), func2->getReturnType());
    }
    
    // أنواع غير متوافقة / Incompatible types
    context_->addError(
        "تعارض في الأنواع: " + type1->toString() + " ≠ " + type2->toString()
    );
    return false;
}

/**
 * تطبيق الاستبدالات / Apply substitutions
 */
std::shared_ptr<Type> TypeInference::substitute(std::shared_ptr<Type> type) {
    return substitution_.apply(type);
}

/**
 * التحقق من occurs check / Check occurs check
 */
bool TypeInference::occursIn(std::shared_ptr<TypeVariable> var, std::shared_ptr<Type> type) {
    if (!var || !type) {
        return false;
    }
    
    // إذا كان النوع هو المتغير نفسه / If type is the variable itself
    auto typeVar = std::dynamic_pointer_cast<TypeVariable>(type);
    if (typeVar && typeVar->getId() == var->getId()) {
        return true;
    }
    
    // إذا كان نوع مركب، تحقق بشكل متكرر / If compound type, check recursively
    auto arrayType = std::dynamic_pointer_cast<ArrayType>(type);
    if (arrayType) {
        return occursIn(var, arrayType->getElementType());
    }
    
    auto dictType = std::dynamic_pointer_cast<DictType>(type);
    if (dictType) {
        return occursIn(var, dictType->getKeyType()) ||
               occursIn(var, dictType->getValueType());
    }
    
    auto funcType = std::dynamic_pointer_cast<FunctionType>(type);
    if (funcType) {
        for (auto& paramType : funcType->getParamTypes()) {
            if (occursIn(var, paramType)) {
                return true;
            }
        }
        return occursIn(var, funcType->getReturnType());
    }
    
    return false; // لا يحدث / Does not occur
}

} // namespace TypeChecker
} // namespace Sad
