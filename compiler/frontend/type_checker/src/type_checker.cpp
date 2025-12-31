/*
 * ============================================================================
 * Type Checker - Implementation / تنفيذ مدقق الأنواع
 * Type Checker - Implementation
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#include "../include/type_checker.h"
#include <sstream>

namespace Sad {
namespace TypeChecker {

// ============================================================================
// Main Type Checking Functions / الدوال الرئيسية للتحقق من الأنواع
// ============================================================================

/**
 * التحقق من برنامج كامل / Check complete program
 */
bool TypeChecker::checkProgram(const std::vector<AST::StmtPtr>& program) {
    // إعادة تعيين الحالة / Reset state
    reset();
    
    // إنشاء النطاق العام / Create global scope
    context_->getEnvironment()->pushScope(Scope::Type::GLOBAL, "global");
    
    // التحقق من كل جملة / Check each statement
    for (const auto& stmt : program) {
        if (!checkStmt(stmt.get())) {
            hasErrors_ = true;
        }
    }
    
    // سحب النطاق العام / Pop global scope
    context_->getEnvironment()->popScope();
    
    return !hasErrors();
}

/**
 * التحقق من تعبير / Check expression
 */
std::shared_ptr<Type> TypeChecker::checkExpr(AST::Expr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType(); // تعبير null / null expression
    }
    
    // التحقق من التخزين المؤقت أولاً / Check cache first
    auto cachedType = getCachedType(expr);
    if (cachedType) {
        return cachedType; // أرجع النوع المخزن / Return cached type
    }
    
    std::shared_ptr<Type> type;
    
    // حسب نوع التعبير / Based on expression type
    switch (expr->getKind()) {
        case AST::ASTNode::Kind::IntegerLiteral:
        case AST::ASTNode::Kind::FloatLiteral:
        case AST::ASTNode::Kind::StringLiteral:
        case AST::ASTNode::Kind::BoolLiteral:
            type = checkLiteral(expr);
            break;
            
        case AST::ASTNode::Kind::Identifier:
            type = checkVariable(static_cast<AST::IdentifierExpr*>(expr));
            break;
            
        case AST::ASTNode::Kind::BinaryExpr:
            type = checkBinaryOp(static_cast<AST::BinaryExpr*>(expr));
            break;
            
        case AST::ASTNode::Kind::UnaryExpr:
            type = checkUnaryOp(static_cast<AST::UnaryExpr*>(expr));
            break;
            
        case AST::ASTNode::Kind::CallExpr:
            type = checkFunctionCall(static_cast<AST::CallExpr*>(expr));
            break;
            
        case AST::ASTNode::Kind::MemberExpr:
            type = checkMemberAccess(static_cast<AST::MemberExpr*>(expr));
            break;
            
        case AST::ASTNode::Kind::IndexExpr:
            type = checkIndexAccess(static_cast<AST::IndexExpr*>(expr));
            break;
            
        case AST::ASTNode::Kind::LambdaExpr:
            type = checkLambda(static_cast<AST::LambdaExpr*>(expr));
            break;
            
        case AST::ASTNode::Kind::AssignmentExpr:
            type = checkAssignment(static_cast<AST::AssignmentExpr*>(expr));
            break;
            
        default:
            // استخدام Type Inference للأنواع غير المدعومة / Use Type Inference for unsupported types
            addWarning("استخدام Type Inference لتعبير غير مدعوم", expr);
            type = inference_->inferExpr(expr);
            break;
    }
    
    // حفظ النوع في التخزين المؤقت / Cache the type
    if (type) {
        cacheType(expr, type);
    }
    
    return type;
}

/**
 * التحقق من جملة / Check statement
 */
bool TypeChecker::checkStmt(AST::Stmt* stmt) {
    if (!stmt) {
        return true; // جملة null / null statement
    }
    
    // حسب نوع الجملة / Based on statement type
    switch (stmt->getKind()) {
        case AST::ASTNode::Kind::ExprStmt:
            return checkExprStmt(static_cast<AST::ExprStmt*>(stmt));
            
        case AST::ASTNode::Kind::VarDeclStmt:
            return checkVarDecl(static_cast<AST::VarDeclStmt*>(stmt));
            
        case AST::ASTNode::Kind::IfStmt:
            return checkIfStmt(static_cast<AST::IfStmt*>(stmt));
            
        case AST::ASTNode::Kind::WhileStmt:
            return checkWhileStmt(static_cast<AST::WhileStmt*>(stmt));
            
        case AST::ASTNode::Kind::ForStmt:
            return checkForStmt(static_cast<AST::ForStmt*>(stmt));
            
        case AST::ASTNode::Kind::ReturnStmt:
            return checkReturnStmt(static_cast<AST::ReturnStmt*>(stmt));
            
        case AST::ASTNode::Kind::BreakStmt:
            return checkBreakStmt(static_cast<AST::BreakStmt*>(stmt));
            
        case AST::ASTNode::Kind::ContinueStmt:
            return checkContinueStmt(static_cast<AST::ContinueStmt*>(stmt));
            
        case AST::ASTNode::Kind::BlockStmt:
            return checkBlock(static_cast<AST::BlockStmt*>(stmt));
            
        case AST::ASTNode::Kind::FunctionDecl:
            return checkFunctionDecl(static_cast<AST::FunctionDecl*>(stmt));
            
        case AST::ASTNode::Kind::ClassDecl:
            return checkClassDecl(static_cast<AST::ClassDecl*>(stmt));
            
        default:
            addWarning("نوع جملة غير مدعوم في Type Checker", stmt);
            return true;
    }
}

// ============================================================================
// Expression Checking Functions / دوال التحقق من التعابير
// ============================================================================

/**
 * التحقق من حرفي / Check literal
 */
std::shared_ptr<Type> TypeChecker::checkLiteral(AST::Expr* expr) {
    // الحرفيات لها أنواع مباشرة / Literals have direct types
    switch (expr->getKind()) {
        case AST::ASTNode::Kind::IntegerLiteral:
            return TypeFactory::getIntType();
            
        case AST::ASTNode::Kind::FloatLiteral:
            return TypeFactory::getFloatType();
            
        case AST::ASTNode::Kind::StringLiteral:
            return TypeFactory::getStringType();
            
        case AST::ASTNode::Kind::BoolLiteral:
            return TypeFactory::getBoolType();
            
        default:
            return TypeFactory::getVoidType();
    }
}

/**
 * التحقق من متغير / Check variable
 */
std::shared_ptr<Type> TypeChecker::checkVariable(AST::IdentifierExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    auto varName = expr->getName();
    
    // البحث عن المتغير في البيئة / Look up variable in environment
    auto symbol = context_->getEnvironment()->lookupSymbol(varName);
    if (!symbol) {
        addError("متغير غير معرّف: " + varName, expr);
        return TypeFactory::getVoidType();
    }
    
    // التحقق من أن المتغير مُهيأ / Check if variable is initialized
    if (!symbol->isInitialized()) {
        addWarning("استخدام متغير قبل تهيئته: " + varName, expr);
    }
    
    return symbol->getType();
}

/**
 * التحقق من عملية ثنائية / Check binary operation
 */
std::shared_ptr<Type> TypeChecker::checkBinaryOp(AST::BinaryExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    // التحقق من نوع اليمين واليسار / Check left and right types
    auto leftType = checkExpr(expr->getLeft());
    auto rightType = checkExpr(expr->getRight());
    
    auto op = expr->getOperator();
    
    // عمليات حسابية: +, -, *, /, % / Arithmetic operations
    if (op == Lexer::TokenType::OP_PLUS || 
        op == Lexer::TokenType::OP_MINUS || 
        op == Lexer::TokenType::OP_MULTIPLY || 
        op == Lexer::TokenType::OP_DIVIDE ||
        op == Lexer::TokenType::OP_MODULO) {
        
        // كلا الطرفين يجب أن يكونا رقميين / Both sides must be numeric
        if (!requireNumericType(leftType, expr->getLeft())) {
            addError("الطرف الأيسر يجب أن يكون رقمياً", expr);
        }
        
        if (!requireNumericType(rightType, expr->getRight())) {
            addError("الطرف الأيمن يجب أن يكون رقمياً", expr);
        }
        
        // إذا كان أحدهما float، النتيجة float / If either is float, result is float
        if (leftType->getKind() == Type::Kind::FLOAT || 
            rightType->getKind() == Type::Kind::FLOAT) {
            return TypeFactory::getFloatType();
        }
        
        return TypeFactory::getIntType();
    }
    
    // عمليات مقارنة: ==, !=, <, >, <=, >= / Comparison operations
    if (op == Lexer::TokenType::OP_EQUAL || 
        op == Lexer::TokenType::OP_NOT_EQUAL ||
        op == Lexer::TokenType::OP_LESS || 
        op == Lexer::TokenType::OP_GREATER ||
        op == Lexer::TokenType::OP_LESS_EQUAL || 
        op == Lexer::TokenType::OP_GREATER_EQUAL) {
        
        // الطرفان يجب أن يكونا متوافقين / Both sides must be compatible
        if (!checkTypeCompatibility(leftType, rightType, expr)) {
            addError("تعارض في الأنواع: " + leftType->toString() + " ≠ " + rightType->toString(), expr);
        }
        
        // النتيجة دائماً bool / Result is always bool
        return TypeFactory::getBoolType();
    }
    
    // عمليات منطقية: &&, ||, and, or / Logical operations
    if (op == Lexer::TokenType::KW_AND || 
        op == Lexer::TokenType::KW_OR) {
        
        // كلا الطرفين يجب أن يكونا bool / Both sides must be bool
        if (!requireBoolType(leftType, expr->getLeft())) {
            addError("الطرف الأيسر يجب أن يكون منطقياً (bool)", expr);
        }
        
        if (!requireBoolType(rightType, expr->getRight())) {
            addError("الطرف الأيمن يجب أن يكون منطقياً (bool)", expr);
        }
        
        return TypeFactory::getBoolType();
    }
    
    // عملية غير معروفة / Unknown operation
    addError("عملية ثنائية غير مدعومة", expr);
    return TypeFactory::getVoidType();
}

/**
 * التحقق من عملية أحادية / Check unary operation
 */
std::shared_ptr<Type> TypeChecker::checkUnaryOp(AST::UnaryExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    // التحقق من نوع المعامل / Check operand type
    auto operandType = checkExpr(expr->getOperand());
    
    auto op = expr->getOperator();
    
    // عملية النفي المنطقي: !, not / Logical negation
    if (op == Lexer::TokenType::OP_NOT || 
        op == Lexer::TokenType::KW_NOT) {
        
        if (!requireBoolType(operandType, expr->getOperand())) {
            addError("المعامل يجب أن يكون منطقياً (bool)", expr);
        }
        
        return TypeFactory::getBoolType();
    }
    
    // عملية النفي السالب: -, + / Numeric negation
    if (op == Lexer::TokenType::OP_MINUS || 
        op == Lexer::TokenType::OP_PLUS) {
        
        if (!requireNumericType(operandType, expr->getOperand())) {
            addError("المعامل يجب أن يكون رقمياً", expr);
        }
        
        return operandType; // النتيجة نفس نوع المعامل / Result is same as operand type
    }
    
    // عمليات الزيادة/النقصان: ++, -- / Increment/decrement
    if (op == Lexer::TokenType::OP_INCREMENT || 
        op == Lexer::TokenType::OP_DECREMENT) {
        
        if (!requireNumericType(operandType, expr->getOperand())) {
            addError("المعامل يجب أن يكون رقمياً", expr);
        }
        
        // التحقق من أن المعامل متغير قابل للتعديل / Check operand is mutable variable
        auto varExpr = dynamic_cast<AST::IdentifierExpr*>(expr->getOperand());
        if (varExpr) {
            auto symbol = context_->getEnvironment()->lookupSymbol(varExpr->getName());
            if (symbol && !symbol->isMutable()) {
                addError("لا يمكن تعديل متغير ثابت", expr);
            }
        }
        
        return operandType;
    }
    
    // عملية غير معروفة / Unknown operation
    addError("عملية أحادية غير مدعومة", expr);
    return TypeFactory::getVoidType();
}

/**
 * التحقق من استدعاء دالة / Check function call
 */
std::shared_ptr<Type> TypeChecker::checkFunctionCall(AST::CallExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    // التحقق من نوع الدالة / Check function type
    auto funcType = checkExpr(expr->getCallee());
    
    // التحقق من أنه نوع دالة / Check it's a function type
    auto functionType = std::dynamic_pointer_cast<FunctionType>(funcType);
    if (!functionType) {
        addError("التعبير ليس دالة", expr->getCallee());
        return TypeFactory::getVoidType();
    }
    
    // التحقق من عدد المعاملات / Check argument count
    auto expectedParams = functionType->getParamTypes();
    auto actualArgs = expr->getArguments();
    
    if (!functionType->isVariadic() && actualArgs.size() != expectedParams.size()) {
        std::stringstream ss;
        ss << "عدد المعاملات غير صحيح. متوقع: " << expectedParams.size()
           << "، فعلي: " << actualArgs.size();
        addError(ss.str(), expr);
    }
    
    // التحقق من نوع كل معامل / Check each argument type
    size_t checkCount = std::min(actualArgs.size(), expectedParams.size());
    for (size_t i = 0; i < checkCount; ++i) {
        auto argType = checkExpr(actualArgs[i].get());
        auto expectedType = expectedParams[i];
        
        if (!checkTypeCompatibility(expectedType, argType, actualArgs[i].get())) {
            std::stringstream ss;
            ss << "تعارض في نوع المعامل " << (i + 1)
               << ". متوقع: " << expectedType->toString()
               << "، فعلي: " << argType->toString();
            addError(ss.str(), actualArgs[i].get());
        }
    }
    
    return functionType->getReturnType();
}

/**
 * التحقق من الوصول لعضو / Check member access
 */
std::shared_ptr<Type> TypeChecker::checkMemberAccess(AST::MemberExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    // التحقق من نوع الكائن / Check object type
    auto objectType = checkExpr(expr->getObject());
    
    auto memberName = expr->getMemberName();
    
    // إذا كان الكائن من نوع ClassType / If object is ClassType
    auto classType = std::dynamic_pointer_cast<ClassType>(objectType);
    if (classType) {
        // البحث عن العضو / Look for member
        auto memberType = classType->getMemberType(memberName);
        if (memberType) {
            return memberType;
        }
        
        // عضو غير موجود / Member not found
        addError("العضو '" + memberName + "' غير موجود في الصنف", expr);
        return TypeFactory::getVoidType();
    }
    
    // نوع غير مدعوم للوصول للأعضاء / Unsupported type for member access
    addError("لا يمكن الوصول للأعضاء في نوع: " + objectType->toString(), expr);
    return TypeFactory::getVoidType();
}

/**
 * التحقق من الوصول لعنصر / Check array/dict access
 */
std::shared_ptr<Type> TypeChecker::checkIndexAccess(AST::IndexExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    // التحقق من نوع المصفوفة/القاموس / Check array/dict type
    auto containerType = checkExpr(expr->getObject());
    
    // التحقق من نوع الفهرس / Check index type
    auto indexType = checkExpr(expr->getIndex());
    
    // إذا كان مصفوفة / If array
    auto arrayType = std::dynamic_pointer_cast<ArrayType>(containerType);
    if (arrayType) {
        // الفهرس يجب أن يكون int / Index must be int
        if (!checkTypeCompatibility(TypeFactory::getIntType(), indexType, expr->getIndex())) {
            addError("فهرس المصفوفة يجب أن يكون عدد صحيح", expr->getIndex());
        }
        
        return arrayType->getElementType();
    }
    
    // إذا كان قاموس / If dict
    auto dictType = std::dynamic_pointer_cast<DictType>(containerType);
    if (dictType) {
        // الفهرس يجب أن يكون من نوع المفتاح / Index must be key type
        if (!checkTypeCompatibility(dictType->getKeyType(), indexType, expr->getIndex())) {
            addError("نوع المفتاح غير متوافق. متوقع: " + dictType->getKeyType()->toString(), expr->getIndex());
        }
        
        return dictType->getValueType();
    }
    
    // نوع غير مدعوم للفهرسة / Unsupported type for indexing
    addError("لا يمكن فهرسة نوع: " + containerType->toString(), expr);
    return TypeFactory::getVoidType();
}

/**
 * التحقق من دالة lambda / Check lambda function
 */
std::shared_ptr<Type> TypeChecker::checkLambda(AST::LambdaExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    // إنشاء أنواع للمعاملات / Create types for parameters
    std::vector<std::shared_ptr<Type>> paramTypes;
    
    // دفع نطاق جديد / Push new scope
    context_->getEnvironment()->pushScope(Scope::Type::FUNCTION, "lambda");
    
    // إضافة المعاملات للنطاق / Add parameters to scope
    for (const auto& param : expr->getParameters()) {
        auto paramType = dataTypeToType(param.type);
        paramTypes.push_back(paramType);
        
        auto symbol = context_->getEnvironment()->addSymbol(
            param.name,
            paramType,
            Symbol::Kind::PARAMETER
        );
        
        if (symbol) {
            symbol->setInitialized(true); // المعاملات مهيأة دائماً / Parameters always initialized
        }
    }
    
    // التحقق من جسم الدالة / Check function body
    std::shared_ptr<Type> returnType;
    if (expr->getBody()) {
        returnType = checkExpr(expr->getBody());
    } else {
        returnType = TypeFactory::getVoidType();
    }
    
    // سحب النطاق / Pop scope
    context_->getEnvironment()->popScope();
    
    // إنشاء نوع الدالة / Create function type
    return TypeFactory::createFunctionType(paramTypes, returnType, false);
}

/**
 * التحقق من إسناد / Check assignment
 */
std::shared_ptr<Type> TypeChecker::checkAssignment(AST::AssignmentExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    // التحقق من نوع اليمين (القيمة) / Check right side (value) type
    auto valueType = checkExpr(expr->getValue());
    
    // التحقق من نوع اليسار (الهدف) / Check left side (target) type
    auto targetType = checkExpr(expr->getTarget());
    
    // التحقق من أن الهدف متغير / Check target is variable
    auto varExpr = dynamic_cast<AST::IdentifierExpr*>(expr->getTarget());
    if (varExpr) {
        auto symbol = context_->getEnvironment()->lookupSymbol(varExpr->getName());
        if (symbol) {
            // التحقق من أن المتغير قابل للتعديل / Check variable is mutable
            if (!symbol->isMutable()) {
                addError("لا يمكن تعديل متغير ثابت: " + varExpr->getName(), expr);
            }
            
            // تحديد المتغير كمُهيأ / Mark variable as initialized
            symbol->setInitialized(true);
        }
    }
    
    // التحقق من توافق الأنواع / Check type compatibility
    if (!checkTypeCompatibility(targetType, valueType, expr)) {
        addError("تعارض في الأنواع: لا يمكن إسناد " + valueType->toString() + " إلى " + targetType->toString(), expr);
    }
    
    return targetType;
}

// ============================================================================
// Statement Checking Functions / دوال التحقق من الجمل
// ============================================================================

/**
 * التحقق من جملة تعبير / Check expression statement
 */
bool TypeChecker::checkExprStmt(AST::ExprStmt* stmt) {
    if (!stmt || !stmt->getExpression()) {
        return true;
    }
    
    // التحقق من التعبير / Check expression
    checkExpr(stmt->getExpression());
    
    return true;
}

/**
 * التحقق من تصريح متغير / Check variable declaration
 */
bool TypeChecker::checkVarDecl(AST::VarDeclStmt* decl) {
    if (!decl) {
        return true;
    }
    
    // تحويل نوع البيانات / Convert data type
    auto varType = dataTypeToType(decl->getType());
    
    // التحقق من القيمة الأولية / Check initializer
    std::shared_ptr<Type> initType;
    if (decl->getInitializer()) {
        initType = checkExpr(decl->getInitializer());
        
        // التحقق من توافق الأنواع / Check type compatibility
        if (!checkTypeCompatibility(varType, initType, decl->getInitializer())) {
            addError("تعارض في نوع المتغير. متوقع: " + varType->toString() + "، فعلي: " + initType->toString(), decl);
            return false;
        }
    } else if (decl->isConst()) {
        // الثوابت يجب أن تُهيأ / Constants must be initialized
        addError("يجب تهيئة المتغير الثابت: " + decl->getName(), decl);
        return false;
    }
    
    // إضافة المتغير للنطاق / Add variable to scope
    auto symbol = context_->getEnvironment()->addSymbol(
        decl->getName(),
        varType,
        decl->isConst() ? Symbol::Kind::CONSTANT : Symbol::Kind::VARIABLE
    );
    
    if (!symbol) {
        addError("المتغير معرّف مسبقاً: " + decl->getName(), decl);
        return false;
    }
    
    // تحديد حالة التهيئة / Set initialization state
    symbol->setInitialized(decl->getInitializer() != nullptr);
    symbol->setMutable(!decl->isConst());
    
    return true;
}

/**
 * التحقق من جملة if / Check if statement
 */
bool TypeChecker::checkIfStmt(AST::IfStmt* stmt) {
    if (!stmt) {
        return true;
    }
    
    // التحقق من الشرط / Check condition
    auto condType = checkExpr(stmt->getCondition());
    if (!requireBoolType(condType, stmt->getCondition())) {
        addError("شرط if يجب أن يكون منطقياً (bool)", stmt);
    }
    
    // التحقق من الفرع الإيجابي / Check then branch
    if (stmt->getThenBranch()) {
        context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "if-then");
        checkStmt(stmt->getThenBranch());
        context_->getEnvironment()->popScope();
    }
    
    // التحقق من الفرع السلبي / Check else branch
    if (stmt->getElseBranch()) {
        context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "if-else");
        checkStmt(stmt->getElseBranch());
        context_->getEnvironment()->popScope();
    }
    
    return true;
}

/**
 * التحقق من جملة while / Check while statement
 */
bool TypeChecker::checkWhileStmt(AST::WhileStmt* stmt) {
    if (!stmt) {
        return true;
    }
    
    // التحقق من الشرط / Check condition
    auto condType = checkExpr(stmt->getCondition());
    if (!requireBoolType(condType, stmt->getCondition())) {
        addError("شرط while يجب أن يكون منطقياً (bool)", stmt);
    }
    
    // التحقق من الجسم / Check body
    if (stmt->getBody()) {
        context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "while");
        checkStmt(stmt->getBody());
        context_->getEnvironment()->popScope();
    }
    
    return true;
}

/**
 * التحقق من جملة for / Check for statement
 */
bool TypeChecker::checkForStmt(AST::ForStmt* stmt) {
    if (!stmt) {
        return true;
    }
    
    // دفع نطاق جديد للحلقة / Push new scope for loop
    context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "for");
    
    // التحقق من التهيئة / Check initializer
    if (stmt->getInitializer()) {
        checkStmt(stmt->getInitializer());
    }
    
    // التحقق من الشرط / Check condition
    if (stmt->getCondition()) {
        auto condType = checkExpr(stmt->getCondition());
        if (!requireBoolType(condType, stmt->getCondition())) {
            addError("شرط for يجب أن يكون منطقياً (bool)", stmt);
        }
    }
    
    // التحقق من التحديث / Check increment
    if (stmt->getIncrement()) {
        checkExpr(stmt->getIncrement());
    }
    
    // التحقق من الجسم / Check body
    if (stmt->getBody()) {
        checkStmt(stmt->getBody());
    }
    
    // سحب النطاق / Pop scope
    context_->getEnvironment()->popScope();
    
    return true;
}

/**
 * التحقق من جملة return / Check return statement
 */
bool TypeChecker::checkReturnStmt(AST::ReturnStmt* stmt) {
    if (!stmt) {
        return true;
    }
    
    // الحصول على نوع الإرجاع المتوقع / Get expected return type
    auto expectedReturnType = context_->getExpectedReturnType();
    if (!expectedReturnType) {
        addError("جملة return خارج دالة", stmt);
        return false;
    }
    
    // التحقق من قيمة الإرجاع / Check return value
    std::shared_ptr<Type> actualReturnType;
    if (stmt->getValue()) {
        actualReturnType = checkExpr(stmt->getValue());
    } else {
        actualReturnType = TypeFactory::getVoidType();
    }
    
    // التحقق من توافق الأنواع / Check type compatibility
    if (!checkTypeCompatibility(expectedReturnType, actualReturnType, stmt)) {
        addError("تعارض في نوع الإرجاع. متوقع: " + expectedReturnType->toString() + "، فعلي: " + actualReturnType->toString(), stmt);
        return false;
    }
    
    return true;
}

/**
 * التحقق من جملة break / Check break statement
 */
bool TypeChecker::checkBreakStmt(AST::BreakStmt* stmt) {
    // TODO: التحقق من أننا داخل حلقة / Check we're inside a loop
    return true;
}

/**
 * التحقق من جملة continue / Check continue statement
 */
bool TypeChecker::checkContinueStmt(AST::ContinueStmt* stmt) {
    // TODO: التحقق من أننا داخل حلقة / Check we're inside a loop
    return true;
}

/**
 * التحقق من كتلة / Check block
 */
bool TypeChecker::checkBlock(AST::BlockStmt* stmt) {
    if (!stmt) {
        return true;
    }
    
    // دفع نطاق جديد / Push new scope
    context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "block");
    
    // التحقق من كل جملة / Check each statement
    for (const auto& statement : stmt->getStatements()) {
        checkStmt(statement.get());
    }
    
    // سحب النطاق / Pop scope
    context_->getEnvironment()->popScope();
    
    return true;
}

/**
 * التحقق من تصريح دالة / Check function declaration
 */
bool TypeChecker::checkFunctionDecl(AST::FunctionDecl* decl) {
    if (!decl) {
        return true;
    }
    
    // إنشاء أنواع المعاملات / Create parameter types
    std::vector<std::shared_ptr<Type>> paramTypes;
    for (const auto& param : decl->getParameters()) {
        paramTypes.push_back(dataTypeToType(param.type));
    }
    
    // إنشاء نوع الإرجاع / Create return type
    auto returnType = dataTypeToType(decl->getReturnType());
    
    // إنشاء نوع الدالة / Create function type
    auto funcType = TypeFactory::createFunctionType(paramTypes, returnType, false);
    
    // إضافة الدالة للنطاق / Add function to scope
    auto symbol = context_->getEnvironment()->addSymbol(
        decl->getName(),
        funcType,
        Symbol::Kind::FUNCTION
    );
    
    if (!symbol) {
        addError("الدالة معرّفة مسبقاً: " + decl->getName(), decl);
        return false;
    }
    
    symbol->setInitialized(true);
    
    // دفع نطاق جديد للدالة / Push new scope for function
    context_->getEnvironment()->pushScope(Scope::Type::FUNCTION, decl->getName());
    
    // حفظ نوع الإرجاع المتوقع / Save expected return type
    auto prevReturnType = context_->getExpectedReturnType();
    context_->setExpectedReturnType(returnType);
    
    // إضافة المعاملات للنطاق / Add parameters to scope
    for (size_t i = 0; i < decl->getParameters().size(); ++i) {
        const auto& param = decl->getParameters()[i];
        auto paramSymbol = context_->getEnvironment()->addSymbol(
            param.name,
            paramTypes[i],
            Symbol::Kind::PARAMETER
        );
        
        if (paramSymbol) {
            paramSymbol->setInitialized(true);
        }
    }
    
    // التحقق من جسم الدالة / Check function body
    if (decl->getBody()) {
        checkStmt(decl->getBody());
    }
    
    // استرجاع نوع الإرجاع المتوقع / Restore expected return type
    context_->setExpectedReturnType(prevReturnType);
    
    // سحب النطاق / Pop scope
    context_->getEnvironment()->popScope();
    
    return true;
}

/**
 * التحقق من تصريح صنف / Check class declaration
 */
bool TypeChecker::checkClassDecl(AST::ClassDecl* decl) {
    if (!decl) {
        return true;
    }
    
    // إنشاء نوع الصنف / Create class type
    auto classType = std::make_shared<ClassType>(decl->getName());
    
    // إضافة الأعضاء / Add members
    for (const auto& member : decl->getMembers()) {
        auto memberType = dataTypeToType(member.type);
        classType->addMember(member.name, memberType);
    }
    
    // إضافة الصنف للنطاق / Add class to scope
    auto symbol = context_->getEnvironment()->addSymbol(
        decl->getName(),
        classType,
        Symbol::Kind::CLASS
    );
    
    if (!symbol) {
        addError("الصنف معرّف مسبقاً: " + decl->getName(), decl);
        return false;
    }
    
    symbol->setInitialized(true);
    
    // TODO: التحقق من الدوال الأعضاء / Check member functions
    
    return true;
}

// ============================================================================
// Helper Functions / الدوال المساعدة
// ============================================================================

/**
 * التحقق من توافق نوعين / Check type compatibility
 */
bool TypeChecker::checkTypeCompatibility(
    std::shared_ptr<Type> expected,
    std::shared_ptr<Type> actual,
    AST::ASTNode* node
) {
    if (!expected || !actual) {
        return false;
    }
    
    // إذا كانا متساويين / If equal
    if (expected->equals(actual.get())) {
        return true;
    }
    
    // التحقق من إمكانية التحويل / Check if castable
    if (actual->canCastTo(expected.get())) {
        return true;
    }
    
    return false;
}

/**
 * إضافة خطأ / Add error
 */
void TypeChecker::addError(const std::string& message, AST::ASTNode* node) {
    std::string fullMessage = "خطأ في الأنواع: " + message;
    
    if (node) {
        fullMessage += " [السطر " + std::to_string(node->position.line) + "]";
    }
    
    context_->addError(fullMessage);
    hasErrors_ = true;
}

/**
 * إضافة تحذير / Add warning
 */
void TypeChecker::addWarning(const std::string& message, AST::ASTNode* node) {
    std::string fullMessage = "تحذير: " + message;
    
    if (node) {
        fullMessage += " [السطر " + std::to_string(node->position.line) + "]";
    }
    
    context_->addWarning(fullMessage);
}

/**
 * الحصول من التخزين المؤقت / Get from cache
 */
std::shared_ptr<Type> TypeChecker::getCachedType(AST::ASTNode* node) {
    auto it = typeCache_.find(node);
    if (it != typeCache_.end()) {
        return it->second;
    }
    return nullptr;
}

/**
 * الحفظ في التخزين المؤقت / Save to cache
 */
void TypeChecker::cacheType(AST::ASTNode* node, std::shared_ptr<Type> type) {
    typeCache_[node] = type;
}

/**
 * تحويل DataType إلى Type / Convert DataType to Type
 */
std::shared_ptr<Type> TypeChecker::dataTypeToType(Data::DataType dataType) {
    switch (dataType) {
        case Data::DataType::INTEGER:
            return TypeFactory::getIntType();
        case Data::DataType::FLOAT:
            return TypeFactory::getFloatType();
        case Data::DataType::STRING:
            return TypeFactory::getStringType();
        case Data::DataType::BOOLEAN:
            return TypeFactory::getBoolType();
        case Data::DataType::VOID:
            return TypeFactory::getVoidType();
        default:
            return TypeFactory::getVoidType();
    }
}

/**
 * التحقق من نوع bool / Check bool type
 */
bool TypeChecker::requireBoolType(std::shared_ptr<Type> type, AST::ASTNode* node) {
    if (!type) {
        return false;
    }
    
    return type->getKind() == Type::Kind::BOOL;
}

/**
 * التحقق من نوع رقمي / Check numeric type
 */
bool TypeChecker::requireNumericType(std::shared_ptr<Type> type, AST::ASTNode* node) {
    if (!type) {
        return false;
    }
    
    auto kind = type->getKind();
    return kind == Type::Kind::INT || kind == Type::Kind::FLOAT;
}

} // namespace TypeChecker
} // namespace Sad
