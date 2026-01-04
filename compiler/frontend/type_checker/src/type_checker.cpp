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
std::shared_ptr<Type> TypeChecker::checkExpr(AST::Expression* expr) {
    if (!expr) {
        return TypeFactory::getVoidType(); // تعبير null / null expression
    }
    
    // التحقق من التخزين المؤقت أولاً / Check cache first
    auto cachedType = getCachedType(expr);
    if (cachedType) {
        return cachedType; // أرجع النوع المخزن / Return cached type
    }
    
    std::shared_ptr<Type> type;
    
    // استخدام dynamic_cast للتحقق من نوع التعبير / Use dynamic_cast to check expression type
    if (dynamic_cast<AST::LiteralExpr*>(expr)) {
        type = checkLiteral(expr);
    }
    else if (auto varExpr = dynamic_cast<AST::VariableExpr*>(expr)) {
        type = checkVariable(varExpr);
    }
    else if (auto binExpr = dynamic_cast<AST::BinaryExpr*>(expr)) {
        type = checkBinaryOp(binExpr);
    }
    else if (auto unExpr = dynamic_cast<AST::UnaryExpr*>(expr)) {
        type = checkUnaryOp(unExpr);
    }
    else if (auto callExpr = dynamic_cast<AST::CallExpr*>(expr)) {
        type = checkFunctionCall(callExpr);
    }
    else if (auto memExpr = dynamic_cast<AST::MemberExpr*>(expr)) {
        type = checkMemberAccess(memExpr);
    }
    else if (auto idxExpr = dynamic_cast<AST::IndexExpr*>(expr)) {
        type = checkIndexAccess(idxExpr);
    }
    else if (auto lambdaExpr = dynamic_cast<AST::LambdaExpr*>(expr)) {
        type = checkLambda(lambdaExpr);
    }
    else if (auto assignExpr = dynamic_cast<AST::AssignExpr*>(expr)) {
        type = checkAssignment(assignExpr);
    }
    else {
        // استخدام Type Inference للأنواع غير المدعومة / Use Type Inference for unsupported types
        addWarning("استخدام Type Inference لتعبير غير مدعوم", expr);
        type = inference_->inferExpr(expr);
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
bool TypeChecker::checkStmt(AST::Statement* stmt) {
    if (!stmt) {
        return true; // جملة null / null statement
    }
    
    // استخدام dynamic_cast للتحقق من نوع الجملة / Use dynamic_cast to check statement type
    if (auto exprStmt = dynamic_cast<AST::ExprStmt*>(stmt)) {
        return checkExprStmt(exprStmt);
    }
    else if (auto varDecl = dynamic_cast<AST::VarDeclStmt*>(stmt)) {
        return checkVarDecl(varDecl);
    }
    else if (auto ifStmt = dynamic_cast<AST::IfStmt*>(stmt)) {
        return checkIfStmt(ifStmt);
    }
    else if (auto whileStmt = dynamic_cast<AST::WhileStmt*>(stmt)) {
        return checkWhileStmt(whileStmt);
    }
    else if (auto forStmt = dynamic_cast<AST::ForStmt*>(stmt)) {
        return checkForStmt(forStmt);
    }
    else if (auto retStmt = dynamic_cast<AST::ReturnStmt*>(stmt)) {
        return checkReturnStmt(retStmt);
    }
    else if (auto breakStmt = dynamic_cast<AST::BreakStmt*>(stmt)) {
        return checkBreakStmt(breakStmt);
    }
    else if (auto contStmt = dynamic_cast<AST::ContinueStmt*>(stmt)) {
        return checkContinueStmt(contStmt);
    }
    else if (auto blockStmt = dynamic_cast<AST::BlockStmt*>(stmt)) {
        return checkBlock(blockStmt);
    }
    else if (auto funcDecl = dynamic_cast<AST::FunctionDecl*>(stmt)) {
        return checkFunctionDecl(funcDecl);
    }
    else if (auto classDecl = dynamic_cast<AST::ClassDecl*>(stmt)) {
        return checkClassDecl(classDecl);
    }
    else {
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
std::shared_ptr<Type> TypeChecker::checkLiteral(AST::Expression* expr) {
    // استخدام dynamic_cast للتحقق من نوع الحرفي / Use dynamic_cast to check literal type
    if (dynamic_cast<AST::LiteralExpr*>(expr)) {
        auto litExpr = static_cast<AST::LiteralExpr*>(expr);
        auto tokenType = litExpr->token.getType();
        
        if (tokenType == Lexer::TokenType::NUMBER_INTEGER) {
            return TypeFactory::getIntType();
        }
        else if (tokenType == Lexer::TokenType::NUMBER_DOUBLE) {
            return TypeFactory::getFloatType();
        }
        else if (tokenType == Lexer::TokenType::STRING_LITERAL || 
                 tokenType == Lexer::TokenType::STRING_RAW ||
                 tokenType == Lexer::TokenType::STRING_FSTRING) {
            return TypeFactory::getStringType();
        }
        else if (tokenType == Lexer::TokenType::LITERAL_TRUE || 
                 tokenType == Lexer::TokenType::LITERAL_FALSE) {
            return TypeFactory::getBoolType();
        }
    }
    
    return TypeFactory::getVoidType();
}

/**
 * التحقق من متغير / Check variable
 */
std::shared_ptr<Type> TypeChecker::checkVariable(AST::VariableExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    auto varName = expr->name;
    
    // البحث عن المتغير في البيئة / Look up variable in environment
    auto symbol = context_->getEnvironment()->lookupSymbol(varName);
    if (!symbol) {
        auto error = TypeErrorBuilder::error("E001")
            .messageAr("متؾير غير معرَّف: " + varName)
            .messageEn("Undefined variable: " + varName)
            .location(expr->position)
            .addSuggestion("تحقق من اسم المتغير أو صرّح عنه أولاً", expr->position)
            .build();
        reporter_->addError(error);
        hasErrors_ = true;
        return TypeFactory::getVoidType();
    }
    
    // التحقق من أن المتغير مُهيأ / Check if variable is initialized
    if (!symbol->isInitialized()) {
        auto warning = TypeErrorBuilder::warning("W001")
            .messageAr("استخدام متغير قبل تهيئته: " + varName)
            .messageEn("Using uninitialized variable: " + varName)
            .location(expr->position)
            .addSuggestion("هيِّئ المتغير بقيمة قبل استخدامه", expr->position)
            .build();
        reporter_->addWarning(warning.code, warning.messageAr, warning.messageEn, warning.primaryLocation);
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
    auto leftType = checkExpr(expr->left.get());
    auto rightType = checkExpr(expr->right.get());
    
    auto op = expr->op;
    
    // عمليات حسابية: +, -, *, /, % / Arithmetic operations
    if (op == Lexer::TokenType::OP_PLUS || 
        op == Lexer::TokenType::OP_MINUS || 
        op == Lexer::TokenType::OP_MULTIPLY || 
        op == Lexer::TokenType::OP_DIVIDE ||
        op == Lexer::TokenType::OP_MODULO) {
        
        // كلا الطرفين يجب أن يكونا رقميين / Both sides must be numeric
        if (!requireNumericType(leftType, expr->left.get())) {
            auto error = TypeErrorBuilder::error("E002")
                .messageAr("الطرف الأيسر يجب أن يكون رقمياً")
                .messageEn("Left operand must be numeric")
                .location(expr->left->position)
                .expectedType(TypeFactory::getIntType())
                .actualType(leftType)
                .addSuggestion("تحقق من نوع البيانات", expr->left->position)
                .build();
            reporter_->addError(error);
            hasErrors_ = true;
        }
        
        if (!requireNumericType(rightType, expr->right.get())) {
            auto error = TypeErrorBuilder::error("E003")
                .messageAr("الطرف الأيمن يجب أن يكون رقمياً")
                .messageEn("Right operand must be numeric")
                .location(expr->right->position)
                .expectedType(TypeFactory::getIntType())
                .actualType(rightType)
                .addSuggestion("تحقق من نوع البيانات", expr->right->position)
                .build();
            reporter_->addError(error);
            hasErrors_ = true;
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
            auto error = TypeErrorBuilder::error("E004")
                .messageAr("تعارض في الأنواع")
                .messageEn("Type mismatch in comparison")
                .location(expr->position)
                .expectedType(leftType)
                .actualType(rightType)
                .addSuggestion("تحويل أحد الطرفين لنفس النوع", expr->position)
                .build();
            reporter_->addError(error);
            hasErrors_ = true;
        }
        
        // النتيجة دائماً bool / Result is always bool
        return TypeFactory::getBoolType();
    }
    
    // عمليات منطقية: &&, ||, and, or / Logical operations
    if (op == Lexer::TokenType::OP_AND || 
        op == Lexer::TokenType::OP_OR) {
        
        // كلا الطرفين يجب أن يكونا bool / Both sides must be bool
        if (!requireBoolType(leftType, expr->left.get())) {
            auto error = TypeErrorBuilder::error("E005")
                .messageAr("الطرف الأيسر يجب أن يكون منطقياً (bool)")
                .messageEn("Left operand must be boolean")
                .location(expr->left->position)
                .expectedType(TypeFactory::getBoolType())
                .actualType(leftType)
                .build();
            reporter_->addError(error);
            hasErrors_ = true;
        }
        
        if (!requireBoolType(rightType, expr->right.get())) {
            auto error = TypeErrorBuilder::error("E006")
                .messageAr("الطرف الأيمن يجب أن يكون منطقياً (bool)")
                .messageEn("Right operand must be boolean")
                .location(expr->right->position)
                .expectedType(TypeFactory::getBoolType())
                .actualType(rightType)
                .build();
            reporter_->addError(error);
            hasErrors_ = true;
        }
        
        return TypeFactory::getBoolType();
    }
    
    // عملية غير معروفة / Unknown operation
    auto error = TypeErrorBuilder::error("E007")
        .messageAr("عملية ثنائية غير مدعومة")
        .messageEn("Unsupported binary operation")
        .location(expr->position)
        .build();
    reporter_->addError(error);
    hasErrors_ = true;
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
    auto operandType = checkExpr(expr->operand.get());
    
    auto op = expr->op;
    
    // عملية النفي المنطقي: !, not / Logical negation
    if (op == Lexer::TokenType::OP_NOT) {
        
        if (!requireBoolType(operandType, expr->operand.get())) {
            auto error = TypeErrorBuilder::error("E008")
                .messageAr("المعامل يجب أن يكون منطقياً (bool)")
                .messageEn("Operand must be boolean")
                .location(expr->operand->position)
                .expectedType(TypeFactory::getBoolType())
                .actualType(operandType)
                .build();
            reporter_->addError(error);
            hasErrors_ = true;
        }
        
        return TypeFactory::getBoolType();
    }
    
    // عملية النفي السالب: -, + / Numeric negation
    if (op == Lexer::TokenType::OP_MINUS || 
        op == Lexer::TokenType::OP_PLUS) {
        
        if (!requireNumericType(operandType, expr->operand.get())) {
            auto error = TypeErrorBuilder::error("E009")
                .messageAr("المعامل يجب أن يكون رقمياً")
                .messageEn("Operand must be numeric")
                .location(expr->operand->position)
                .expectedType(TypeFactory::getIntType())
                .actualType(operandType)
                .build();
            reporter_->addError(error);
            hasErrors_ = true;
        }
        
        return operandType; // النتيجة نفس نوع المعامل / Result is same as operand type
    }
    
    // عمليات الزيادة/النقصان: ++, -- / Increment/decrement
    if (op == Lexer::TokenType::OP_INCREMENT || 
        op == Lexer::TokenType::OP_DECREMENT) {
        
        if (!requireNumericType(operandType, expr->operand.get())) {
            auto error = TypeErrorBuilder::error("E010")
                .messageAr("المعامل يجب أن يكون رقمياً")
                .messageEn("Operand must be numeric")
                .location(expr->operand->position)
                .expectedType(TypeFactory::getIntType())
                .actualType(operandType)
                .build();
            reporter_->addError(error);
            hasErrors_ = true;
        }
        
        // التحقق من أن المعامل متغير قابل للتعديل / Check operand is mutable variable
        auto varExpr = dynamic_cast<AST::VariableExpr*>(expr->operand.get());
        if (varExpr) {
            auto symbol = context_->getEnvironment()->lookupSymbol(varExpr->name);
            if (symbol && !symbol->isMutable()) {
                auto error = TypeErrorBuilder::error("E011")
                    .messageAr("لا يمكن تعديل متغير ثابت")
                    .messageEn("Cannot modify constant variable")
                    .location(expr->position)
                    .addSuggestion("استخدم متغير قابل للتعديل", expr->position)
                    .build();
                reporter_->addError(error);
                hasErrors_ = true;
            }
        }
        
        return operandType;
    }
    
    // عملية غير معروفة / Unknown operation
    auto error = TypeErrorBuilder::error("E012")
        .messageAr("عملية أحادية غير مدعومة")
        .messageEn("Unsupported unary operation")
        .location(expr->position)
        .build();
    reporter_->addError(error);
    hasErrors_ = true;
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
    auto funcType = checkExpr(expr->callee.get());
    
    // التحقق من أنه نوع دالة / Check it's a function type
    auto functionType = std::dynamic_pointer_cast<FunctionType>(funcType);
    if (!functionType) {
        auto error = TypeErrorBuilder::error("E013")
            .messageAr("التعبير ليس دالة")
            .messageEn("Expression is not callable")
            .location(expr->callee->position)
            .actualType(funcType)
            .addSuggestion("تحقق من أن هذا اسم دالة صحيح", expr->callee->position)
            .build();
        reporter_->addError(error);
        hasErrors_ = true;
        return TypeFactory::getVoidType();
    }
    
    // التحقق من عدد المعاملات / Check argument count
    auto expectedParams = functionType->getParamTypes();
    const auto& actualArgs = expr->arguments;  // استخدام reference لتجنب نسخ unique_ptr
    
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
    auto objectType = checkExpr(expr->object.get());
    
    auto memberName = expr->member;
    
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
    auto containerType = checkExpr(expr->object.get());
    
    // التحقق من نوع الفهرس / Check index type
    auto indexType = checkExpr(expr->index.get());
    
    // إذا كان مصفوفة / If array
    auto arrayType = std::dynamic_pointer_cast<ArrayType>(containerType);
    if (arrayType) {
        // الفهرس يجب أن يكون int / Index must be int
        if (!checkTypeCompatibility(TypeFactory::getIntType(), indexType, expr->index.get())) {
            addError("فهرس المصفوفة يجب أن يكون عدد صحيح", expr->index.get());
        }
        
        return arrayType->getElementType();
    }
    
    // إذا كان قاموس / If dict
    auto dictType = std::dynamic_pointer_cast<DictType>(containerType);
    if (dictType) {
        // الفهرس يجب أن يكون من نوع المفتاح / Index must be key type
        if (!checkTypeCompatibility(dictType->getKeyType(), indexType, expr->index.get())) {
            addError("نوع المفتاح غير متوافق. متوقع: " + dictType->getKeyType()->toString(), expr->index.get());
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
    for (const auto& param : expr->parameters) {
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
    if (expr->body) {
        returnType = checkExpr(expr->body.get());
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
std::shared_ptr<Type> TypeChecker::checkAssignment(AST::AssignExpr* expr) {
    if (!expr) {
        return TypeFactory::getVoidType();
    }
    
    // التحقق من نوع اليمين (القيمة) / Check right side (value) type
    auto valueType = checkExpr(expr->value.get());
    
    // التحقق من نوع اليسار (الهدف) / Check left side (target) type
    // في AssignExpr، الهدف هو اسم المتغير مباشرة
    auto symbol = context_->getEnvironment()->lookupSymbol(expr->name);
    if (!symbol) {
        addError("متغير غير معرّف: " + expr->name, expr);
        return TypeFactory::getVoidType();
    }
    
    auto targetType = symbol->getType();
    
    // التحقق من أن المتغير قابل للتعديل / Check variable is mutable
    if (!symbol->isMutable()) {
        addError("لا يمكن تعديل متغير ثابت: " + expr->name, expr);
    }
    
    // تحديد المتغير كمُهيأ / Mark variable as initialized
    symbol->setInitialized(true);
    
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
    if (!stmt || !stmt->expression) {
        return true;
    }
    
    // التحقق من التعبير / Check expression
    checkExpr(stmt->expression.get());
    
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
    auto varType = dataTypeToType(decl->type);
    
    // التحقق من القيمة الأولية / Check initializer
    std::shared_ptr<Type> initType;
    if (decl->initializer) {
        initType = checkExpr(decl->initializer.get());
        
        // التحقق من توافق الأنواع / Check type compatibility
        if (!checkTypeCompatibility(varType, initType, decl->initializer.get())) {
            addError("تعارض في نوع المتغير. متوقع: " + varType->toString() + "، فعلي: " + initType->toString(), decl);
            return false;
        }
    } else if (decl->isConst) {
        // الثوابت يجب أن تُهيأ / Constants must be initialized
        addError("يجب تهيئة المتغير الثابت: " + decl->name, decl);
        return false;
    }
    
    // إضافة المتغير للنطاق / Add variable to scope
    auto symbol = context_->getEnvironment()->addSymbol(
        decl->name,
        varType,
        decl->isConst ? Symbol::Kind::CONSTANT : Symbol::Kind::VARIABLE
    );
    
    if (!symbol) {
        addError("المتغير معرّف مسبقاً: " + decl->name, decl);
        return false;
    }
    
    // تحديد حالة التهيئة / Set initialization state
    symbol->setInitialized(decl->initializer != nullptr);
    symbol->setMutable(!decl->isConst);
    
    return true;
}

/**
 * التحقق من جملة if / Check if statement
 * 
 * ============================================================================
 * Phase 1.3.5.3: Type Narrowing Integration
 * ============================================================================
 * مصدر التعريف: type_checker.h:252
 * التوقيع الكامل: bool checkIfStmt(AST::IfStmt* stmt);
 * المتغيرات المستخدمة:
 *   - enableTypeNarrowing_: defined at type_checker.h:80
 *   - narrowingAnalyzer_: defined at type_checker.h:79
 *   - context_: defined at type_checker.h:70
 * الدوال المستدعاة:
 *   - checkExpr(): defined at type_checker.h:106
 *   - requireBoolType(): defined at type_checker.h:342
 *   - checkStmt(): defined at type_checker.h:118
 *   - TypeNarrowingAnalyzer::getCurrentContext(): defined at type_narrowing.h:255
 *   - TypeNarrowingAnalyzer::extractGuardFromCondition(): defined at type_narrowing.h:295
 *   - TypeNarrowingContext::pushScope(): defined at type_narrowing.h:73
 *   - TypeNarrowingContext::popScope(): defined at type_narrowing.h:80
 *   - TypeNarrowingContext::applyGuard(): defined at type_narrowing.h:137
 *   - TypeNarrowingContext::applyElseGuard(): defined at type_narrowing.h:148
 * ============================================================================
 */
bool TypeChecker::checkIfStmt(AST::IfStmt* stmt) {
    if (!stmt) {
        return true;
    }
    
    // التحقق من الشرط / Check condition
    auto condType = checkExpr(stmt->condition.get());
    if (!requireBoolType(condType, stmt->condition.get())) {
        addError("شرط if يجب أن يكون منطقياً (bool)", stmt);
    }
    
    // ════════════════════════════════════════════════════════════════════════════
    // Phase 1.3.5.3: Apply Type Narrowing
    // ════════════════════════════════════════════════════════════════════════════
    
    if (enableTypeNarrowing_) {
        // استخراج Type Guard من الشرط / Extract Type Guard from condition
        std::string varName;
        auto guard = narrowingAnalyzer_.extractGuardFromCondition(stmt->condition.get(), varName);
        
        if (guard && !varName.empty()) {
            // توجد حارس → طبّق تضييق / Guard exists → apply narrowing
            auto* narrowCtx = narrowingAnalyzer_.getCurrentContext();
            
            // التحقق من الفرع الإيجابي (then) / Check then branch
            if (stmt->thenBranch) {
                context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "if-then");
                
                // إنشاء نطاق تضييق جديد / Create new narrowing scope
                narrowCtx->pushScope();
                
                // تطبيق الحارس في then branch / Apply guard in then branch
                narrowCtx->applyGuard(varName, guard);
                
                // التحقق من الكود في then / Check code in then
                checkStmt(stmt->thenBranch.get());
                
                // إغلاق نطاق التضييق / Close narrowing scope
                narrowCtx->popScope();
                
                context_->getEnvironment()->popScope();
            }
            
            // التحقق من الفرع السلبي (else) / Check else branch
            if (stmt->elseBranch) {
                context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "if-else");
                
                // إنشاء نطاق تضييق جديد / Create new narrowing scope
                narrowCtx->pushScope();
                
                // تطبيق الحارس المعكوس في else branch / Apply inverted guard in else branch
                narrowCtx->applyElseGuard(varName, guard);
                
                // التحقق من الكود في else / Check code in else
                checkStmt(stmt->elseBranch.get());
                
                // إغلاق نطاق التضييق / Close narrowing scope
                narrowCtx->popScope();
                
                context_->getEnvironment()->popScope();
            }
            
            return true;
        }
    }
    
    // ════════════════════════════════════════════════════════════════════════════
    // No Type Narrowing - Original Behavior
    // ════════════════════════════════════════════════════════════════════════════
    
    // التحقق من الفرع الإيجابي / Check then branch
    if (stmt->thenBranch) {
        context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "if-then");
        checkStmt(stmt->thenBranch.get());
        context_->getEnvironment()->popScope();
    }
    
    // التحقق من الفرع السلبي / Check else branch
    if (stmt->elseBranch) {
        context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "if-else");
        checkStmt(stmt->elseBranch.get());
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
    auto condType = checkExpr(stmt->condition.get());
    if (!requireBoolType(condType, stmt->condition.get())) {
        addError("شرط while يجب أن يكون منطقياً (bool)", stmt);
    }
    
    // التحقق من الجسم / Check body
    if (stmt->body) {
        context_->getEnvironment()->pushScope(Scope::Type::BLOCK, "while");
        checkStmt(stmt->body.get());
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
    if (stmt->initializer) {
        checkStmt(stmt->initializer.get());
    }
    
    // التحقق من الشرط / Check condition
    if (stmt->condition) {
        auto condType = checkExpr(stmt->condition.get());
        if (!requireBoolType(condType, stmt->condition.get())) {
            addError("شرط for يجب أن يكون منطقياً (bool)", stmt);
        }
    }
    
    // التحقق من التحديث / Check increment
    if (stmt->increment) {
        checkExpr(stmt->increment.get());
    }
    
    // التحقق من الجسم / Check body
    if (stmt->body) {
        checkStmt(stmt->body.get());
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
    if (stmt->value) {
        actualReturnType = checkExpr(stmt->value.get());
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
    for (const auto& statement : stmt->statements) {
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
    for (const auto& param : decl->parameters) {
        paramTypes.push_back(dataTypeToType(param.type));
    }
    
    // إنشاء نوع الإرجاع / Create return type
    auto returnType = dataTypeToType(decl->returnType);
    
    // إنشاء نوع الدالة / Create function type
    auto funcType = TypeFactory::createFunctionType(paramTypes, returnType, false);
    
    // إضافة الدالة للنطاق / Add function to scope
    auto symbol = context_->getEnvironment()->addSymbol(
        decl->name,
        funcType,
        Symbol::Kind::FUNCTION
    );
    
    if (!symbol) {
        addError("الدالة معرّفة مسبقاً: " + decl->name, decl);
        return false;
    }
    
    symbol->setInitialized(true);
    
    // دفع نطاق جديد للدالة / Push new scope for function
    context_->getEnvironment()->pushScope(Scope::Type::FUNCTION, decl->name);
    
    // حفظ نوع الإرجاع المتوقع / Save expected return type
    auto prevReturnType = context_->getExpectedReturnType();
    context_->setExpectedReturnType(returnType);
    
    // إضافة المعاملات للنطاق / Add parameters to scope
    for (size_t i = 0; i < decl->parameters.size(); ++i) {
        const auto& param = decl->parameters[i];
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
    if (decl->body) {
        checkStmt(decl->body.get());
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
    auto classType = std::make_shared<ClassType>(decl->name);
    
    // إضافة الأعضاء / Add members
    for (const auto& member : decl->members) {
        // TODO: تحديد نوع العضو
    }
    
    // إضافة الصنف للنطاق / Add class to scope
    auto symbol = context_->getEnvironment()->addSymbol(
        decl->name,
        classType,
        Symbol::Kind::CLASS
    );
    
    if (!symbol) {
        addError("الصنف معرّف مسبقاً: " + decl->name, decl);
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
    // استخدام TypeErrorReporter الجديد / Use new TypeErrorReporter
    Lexer::Position pos = node ? node->position : Lexer::Position{0, 0};
    
    // إنشاء خطأ بسيط / Create simple error
    reporter_->addError("E000", message, message, pos);
    
    hasErrors_ = true;
}

/**
 * إضافة تحذير / Add warning
 */
void TypeChecker::addWarning(const std::string& message, AST::ASTNode* node) {
    // استخدام TypeErrorReporter الجديد / Use new TypeErrorReporter
    Lexer::Position pos = node ? node->position : Lexer::Position{0, 0};
    
    // إنشاء تحذير بسيط / Create simple warning
    reporter_->addWarning("W000", message, message, pos);
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
        case Data::DataType::NONE:
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
