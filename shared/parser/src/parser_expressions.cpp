/**
 * @file parser_core.cpp
 * @brief (AR) تنفيذ المحلل النحوي الأساسي - تحويل الرموز إلى شجرة AST.
 *        (EN) Parser core implementation - converting tokens to AST.
 * 
 * (AR) يحتوي هذا الملف على تنفيذات جميع دوال المحلل النحوي.
 *      يستخدم تقنية Recursive Descent Parsing مع معالجة الأخطاء والتعافي منها.
 * 
 * (EN) This file contains implementations of all parser functions.
 *      Uses Recursive Descent Parsing with error handling and recovery.
 * 
 * @author Sad Language Development Team
 * @date 2025-11-11
 * @license MIT License
 */

#include "parser_core.h"
#include "advanced_expr_nodes.h"
#include "class_manager.h"
#include <iostream>
#include <sstream>

namespace Sad {
namespace Parser {

using namespace AST;
using namespace Lexer;
using TT = TokenType;

/**
 * @brief (AR) يحلل تعبير - نقطة الدخول.
 *        (EN) Parses expression - entry point.
 */
ExprPtr ParserCore::parseExpression() {
    return parsePipeline();
}

/**
 * @brief (AR) يحلل عامل الأنبوب |> مع إزالة السكر النحوي.
 *        (EN) Parses pipeline operator |> with desugaring.
 *
 * Desugaring rules:
 *   a |> f       →  f(a)
 *   a |> f(b, c) →  f(a, b, c)
 *   a |> f |> g  →  g(f(a))
 */
ExprPtr ParserCore::parsePipeline() {
    auto expr = parseAssignment();
    
    while (match(TT::OP_PIPE_ARROW)) {
        auto pos = previous().getPosition();
        auto right = parseAssignment();
        
        if (!right) {
            errorBilingual(
                "خطأ: توقعت تعبير بعد '|>'.",
                "Error: expected expression after '|>'."
            );
            return nullptr;
        }
        
        // Desugar: if right is a CallExpr, prepend expr as first argument
        // (AR) إزالة السكر: إذا كان الجانب الأيمن استدعاء دالة، أدخل التعبير كأول معامل
        if (auto* call = dynamic_cast<CallExpr*>(right.get())) {
            // a |> f(b, c) → f(a, b, c)
            ExprList newArgs;
            newArgs.push_back(std::move(expr));
            for (auto& arg : call->arguments) {
                newArgs.push_back(std::move(arg));
            }
            expr = std::make_unique<CallExpr>(
                std::move(call->callee),
                std::move(newArgs),
                pos
            );
        } else {
            // a |> f → f(a)
            ExprList args;
            args.push_back(std::move(expr));
            expr = std::make_unique<CallExpr>(
                std::move(right),
                std::move(args),
                pos
            );
        }
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل تعيين: اسم = قيمة، أو Walrus: اسم := قيمة.
 *        (EN) Parses assignment: name = value, or Walrus: name := value.
 */
ExprPtr ParserCore::parseAssignment() {
    auto expr = parseTernary();
    
    // (AR) فحص عامل Walrus := (إسناد داخل تعبير) / (EN) Check for Walrus operator := (assignment expression)
    if (match(TT::OP_WALRUS)) {
        Token walrus = previous();
        auto value = parseAssignment();
        
        // (AR) Walrus يتطلب متغيراً على اليسار / (EN) Walrus requires variable on left
        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<WalrusExpr>(
                var->name,
                std::move(value),
                walrus.getPosition()
            );
        }
        
        errorBilingual(
            "خطأ: عامل Walrus (:=) يتطلب اسم متغير على اليسار",
            "Error: Walrus operator (:=) requires variable name on left"
        );
    }
    
    // Check for assignment operator
    // (AR) التحقق من عامل التعيين العادي
    if (match(TT::OP_ASSIGN)) {
        Token equals = previous();
        auto value = parseAssignment();
        
        // Left side must be a variable or member access
        // (AR) الجانب الأيسر يجب أن يكون متغيراً أو وصول لعضو
        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(
                var->name,
                std::move(value),
                equals.getPosition()
            );
        }
        
        // Support member access assignment: obj.field = value
        // (AR) دعم تعيين قيمة لعضو الكائن
        if (auto* member = dynamic_cast<MemberExpr*>(expr.get())) {
            // Extract object and member name, then create MemberAssignExpr
            // Need to clone the object expression since we're consuming expr
            ExprPtr objectCopy;
            
            // We need to transfer ownership properly
            // Release the MemberExpr and extract its parts
            std::unique_ptr<MemberExpr> memberPtr(static_cast<MemberExpr*>(expr.release()));
            
            return std::make_unique<MemberAssignExpr>(
                std::move(memberPtr->object),
                memberPtr->member,
                std::move(value),
                equals.getPosition()
            );
        }
        
        // دعم الإسناد بالفهرس: م[0] = قيمة ، قاموس["مفتاح"] = قيمة
        // (EN) Support index assignment: arr[0] = value, map["key"] = value
        if (auto* indexExpr = dynamic_cast<IndexExpr*>(expr.get())) {
            std::unique_ptr<IndexExpr> indexPtr(static_cast<IndexExpr*>(expr.release()));
            
            return std::make_unique<IndexAssignExpr>(
                std::move(indexPtr->object),
                std::move(indexPtr->index),
                std::move(value),
                equals.getPosition()
            );
        }
        
        errorBilingual(
            "خطأ: هدف الإسناد غير صالح - يجب أن يكون معرّفاً أو حقل كائن أو فهرس مصفوفة",
            "Error: invalid assignment target - must be identifier, object field, or array index"
        );
    }
    
    // ========================================================================
    // (AR) عمليات الإسناد المركبة: += -= *= /=
    // (EN) Compound assignment operators: += -= *= /=
    // تحويل نحوي: x += y → x = x + y
    // Desugaring: x += y → x = x + y
    // ========================================================================
    if (check(TT::OP_PLUS_ASSIGN) || check(TT::OP_MINUS_ASSIGN) ||
        check(TT::OP_MULTIPLY_ASSIGN) || check(TT::OP_DIVIDE_ASSIGN)) {
        
        Token opToken = current_;
        advance(); // consume the compound operator
        
        auto rhs = parseAssignment();
        
        // (AR) تحديد العامل الأساسي / (EN) Determine base operator
        TokenType binOp;
        switch (opToken.getType()) {
            case TT::OP_PLUS_ASSIGN:     binOp = TT::OP_PLUS; break;
            case TT::OP_MINUS_ASSIGN:    binOp = TT::OP_MINUS; break;
            case TT::OP_MULTIPLY_ASSIGN: binOp = TT::OP_MULTIPLY; break;
            case TT::OP_DIVIDE_ASSIGN:   binOp = TT::OP_DIVIDE; break;
            default:                     binOp = TT::OP_PLUS; break;
        }
        
        // (AR) الجانب الأيسر متغير: x += y → x = x + y
        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            std::string varName = var->name;
            auto varCopy = std::make_unique<VariableExpr>(varName, opToken.getPosition());
            auto binExpr = std::make_unique<BinaryExpr>(
                std::move(varCopy), binOp, std::move(rhs), opToken.getPosition()
            );
            return std::make_unique<AssignExpr>(
                varName, std::move(binExpr), opToken.getPosition()
            );
        }
        
        // (AR) الجانب الأيسر حقل كائن: obj.field += y
        if (auto* member = dynamic_cast<MemberExpr*>(expr.get())) {
            std::string memberName = member->member;
            std::unique_ptr<MemberExpr> memberPtr(static_cast<MemberExpr*>(expr.release()));
            // Build read copy: create new MemberExpr from same object
            ExprPtr readObj;
            if (auto* objVar = dynamic_cast<VariableExpr*>(memberPtr->object.get())) {
                readObj = std::make_unique<VariableExpr>(objVar->name, opToken.getPosition());
            } else if (dynamic_cast<ThisExpr*>(memberPtr->object.get())) {
                // (AR) دعم هذا.حقل += قيمة / (EN) Support this.field += value
                readObj = std::make_unique<ThisExpr>();
            } else if (auto* objMem = dynamic_cast<MemberExpr*>(memberPtr->object.get())) {
                // (AR) دعم كائن.حقل1.حقل2 += قيمة / (EN) Support obj.field1.field2 += value
                if (auto* innerVar = dynamic_cast<VariableExpr*>(objMem->object.get())) {
                    auto innerCopy = std::make_unique<VariableExpr>(innerVar->name, opToken.getPosition());
                    readObj = std::make_unique<MemberExpr>(std::move(innerCopy), objMem->member, opToken.getPosition());
                } else if (dynamic_cast<ThisExpr*>(objMem->object.get())) {
                    auto innerCopy = std::make_unique<ThisExpr>();
                    readObj = std::make_unique<MemberExpr>(std::move(innerCopy), objMem->member, opToken.getPosition());
                }
            }
            if (readObj) {
                auto readMember = std::make_unique<MemberExpr>(
                    std::move(readObj), memberName, opToken.getPosition()
                );
                auto binExpr = std::make_unique<BinaryExpr>(
                    std::move(readMember), binOp, std::move(rhs), opToken.getPosition()
                );
                return std::make_unique<MemberAssignExpr>(
                    std::move(memberPtr->object), memberName, std::move(binExpr), opToken.getPosition()
                );
            }
        }
        
        // (AR) الجانب الأيسر عنصر مصفوفة/خريطة: arr[i] += y → arr[i] = arr[i] + y
        // (EN) Left side is array/map index: arr[i] += y → arr[i] = arr[i] + y
        if (auto* indexExpr = dynamic_cast<IndexExpr*>(expr.get())) {
            std::unique_ptr<IndexExpr> indexPtr(static_cast<IndexExpr*>(expr.release()));
            // Build read copy: reconstruct the index access for the read side
            ExprPtr readObj;
            ExprPtr readIdx;
            if (auto* objVar = dynamic_cast<VariableExpr*>(indexPtr->object.get())) {
                readObj = std::make_unique<VariableExpr>(objVar->name, opToken.getPosition());
            } else if (auto* objMem = dynamic_cast<MemberExpr*>(indexPtr->object.get())) {
                if (auto* innerVar = dynamic_cast<VariableExpr*>(objMem->object.get())) {
                    auto innerCopy = std::make_unique<VariableExpr>(innerVar->name, opToken.getPosition());
                    readObj = std::make_unique<MemberExpr>(std::move(innerCopy), objMem->member, opToken.getPosition());
                }
            }
            if (auto* idxLit = dynamic_cast<LiteralExpr*>(indexPtr->index.get())) {
                readIdx = std::make_unique<LiteralExpr>(idxLit->token);
            } else if (auto* idxVar = dynamic_cast<VariableExpr*>(indexPtr->index.get())) {
                readIdx = std::make_unique<VariableExpr>(idxVar->name, opToken.getPosition());
            }
            if (readObj && readIdx) {
                auto readIndex = std::make_unique<IndexExpr>(
                    std::move(readObj), std::move(readIdx), opToken.getPosition()
                );
                auto binExpr = std::make_unique<BinaryExpr>(
                    std::move(readIndex), binOp, std::move(rhs), opToken.getPosition()
                );
                return std::make_unique<IndexAssignExpr>(
                    std::move(indexPtr->object), std::move(indexPtr->index),
                    std::move(binExpr), opToken.getPosition()
                );
            }
        }
        
        errorBilingual(
            "خطأ: هدف الإسناد المركب غير صالح - يجب أن يكون متغيراً أو حقل كائن أو عنصر مصفوفة",
            "Error: invalid compound assignment target - must be variable, object field, or array index"
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل التعبير الثلاثي الشرطي: شرط ? صحيح : خطأ.
 *        (EN) Parses ternary conditional: condition ? true : false.
 * 
 * Grammar / القواعد:
 *   ternary → logical_or ("?" expression ":" ternary)?
 * 
 * Right-associative: a ? b : c ? d : e → a ? b : (c ? d : e)
 */
ExprPtr ParserCore::parseTernary() {
    auto expr = parseLogicalOr();
    
    // Check for ternary operator ?
    // (AR) التحقق من عامل الثلاثي ؟
    if (match(TT::QUESTION)) {
        Token questionToken = previous();
        // Parse true branch
        // (AR) تحليل الفرع الصحيح
        auto trueExpr = parseExpression();
        
        // Expect colon
        // (AR) توقع نقطتان
        consume(TT::COLON,
            "(AR) توقع ':' في التعبير الثلاثي. (EN) Expected ':' in ternary expression.");
        
        // Parse false branch (recursive for right-associativity)
        // (AR) تحليل الفرع الخاطئ (تكراري لتحقيق الربط الأيمن)
        auto falseExpr = parseTernary();
        
        // Create ternary expression node
        // (AR) إنشاء عقدة التعبير الثلاثي
        return std::make_unique<TernaryExpr>(
            std::move(expr),
            std::move(trueExpr),
            std::move(falseExpr),
            questionToken.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل عامل OR المنطقي: تعبير1 || تعبير2.
 *        (EN) Parses logical OR: expr1 || expr2.
 */
ExprPtr ParserCore::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match(TT::OP_OR)) {
        Token op = previous();
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل عامل AND المنطقي: تعبير1 && تعبير2.
 *        (EN) Parses logical AND: expr1 && expr2.
 */
ExprPtr ParserCore::parseLogicalAnd() {
    auto expr = parseBitwiseOr();
    
    while (match(TT::OP_AND)) {
        Token op = previous();
        auto right = parseBitwiseOr();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل عامل OR البتّي: | .
 *        (EN) Parses bitwise OR operator: | .
 */
ExprPtr ParserCore::parseBitwiseOr() {
    auto expr = parseBitwiseXor();

    while (match(TT::OP_BITWISE_OR)) {
        Token op = previous();
        auto right = parseBitwiseXor();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }

    return expr;
}

/**
 * @brief (AR) يحلل عامل XOR البتّي: ^.
 *        (EN) Parses bitwise XOR operator: ^.
 */
ExprPtr ParserCore::parseBitwiseXor() {
    auto expr = parseBitwiseAnd();

    while (match(TT::OP_XOR)) {
        Token op = previous();
        auto right = parseBitwiseAnd();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }

    return expr;
}

/**
 * @brief (AR) يحلل عامل AND البتّي: &.
 *        (EN) Parses bitwise AND operator: &.
 */
ExprPtr ParserCore::parseBitwiseAnd() {
    auto expr = parseEquality();

    while (check(TT::OP_BITWISE_AND)) {
        advance();
        Token op = previous();
        auto right = parseEquality();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            TT::OP_BITWISE_AND,
            std::move(right),
            op.getPosition()
        );
    }

    return expr;
}

/**
 * @brief (AR) يحلل عوامل المساواة: == !=.
 *        (EN) Parses equality operators: == !=.
 */
ExprPtr ParserCore::parseEquality() {
    auto expr = parseComparison();
    
    while (matchAny({TT::OP_EQUAL, TT::OP_NOT_EQUAL})) {
        Token op = previous();
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل عوامل المقارنة: < <= > >=.
 *        (EN) Parses comparison operators: < <= > >=.
 */
ExprPtr ParserCore::parseComparison() {
    auto expr = parseRange();
    
    while (matchAny({TT::OP_LESS, TT::OP_LESS_EQUAL, 
                     TT::OP_GREATER, TT::OP_GREATER_EQUAL,
                     TT::KEYWORD_IN})) {
        Token op = previous();
        auto right = parseRange();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل تعبير المدى: بداية..نهاية
 *        (EN) Parses range expression: start..end
 */
ExprPtr ParserCore::parseRange() {
    auto expr = parseTerm();
    
    if (match(TT::DOT_DOT)) {
        auto pos = previous().getPosition();
        auto end = parseTerm();
        expr = std::make_unique<RangeExpr>(std::move(expr), std::move(end), pos);
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل الجمع والطرح: + -.
 *        (EN) Parses addition and subtraction: + -.
 */
ExprPtr ParserCore::parseTerm() {
    auto expr = parseFactor();
    
    while (matchAny({TT::OP_PLUS, TT::OP_MINUS})) {
        Token op = previous();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }

    // (AR) إزاحة البت: << >>  / (EN) Bit shifts: << >>
    while (matchAny({TT::OP_SHIFT_LEFT, TT::OP_SHIFT_RIGHT})) {
        Token op = previous();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل الضرب والقسمة: * / %.
 *        (EN) Parses multiplication and division: * / %.
 */
ExprPtr ParserCore::parseFactor() {
    auto expr = parseUnary();
    
    while (matchAny({TT::OP_MULTIPLY, TT::OP_DIVIDE, TT::OP_MODULO})) {
        Token op = previous();
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل العوامل الأحادية: - ! ++ --.
 *        (EN) Parses unary operators: - ! ++ --.
 */
ExprPtr ParserCore::parseUnary() {
    // (AR) تحليل عامل الاستعارة & / (EN) Parse borrow operator &
    if (match(TT::AMPERSAND)) {
        auto pos = previous().getPosition();
        // (AR) التحقق من &متغير / &mut (استعارة قابلة للتعديل)
        // (EN) Check for &متغير / &mut (mutable borrow)
        bool isMut = false;
        if (check(TT::KEYWORD_VAR) || 
            (check(TT::IDENTIFIER) && peek().getValue() == "mut")) {
            isMut = true;
            advance();
        }
        // (AR) اسم المتغير المُستعار / (EN) Variable name to borrow
        if (!check(TT::IDENTIFIER)) {
            error("\xD9\x85\xD8\xAA\xD9\x88\xD9\x82\xD8\xB9 \xD8\xA7\xD8\xB3\xD9\x85 \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA8\xD8\xB9\xD8\xAF & / Expected variable name after &");
            return std::make_unique<LiteralExpr>(Token(TT::LITERAL_NULL, "null", pos));
        }
        std::string varName = peek().getValue();
        advance();
        return std::make_unique<BorrowExpr>(varName, isMut, pos);
    }
    
    if (matchAny({TT::OP_NOT, TT::OP_MINUS, TT::OP_INCREMENT, TT::OP_DECREMENT, TT::OP_BITWISE_NOT})) {
        Token op = previous();
        auto right = parseUnary();
        return std::make_unique<UnaryExpr>(
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return parsePower();
}

/**
 * @brief (AR) يحلل عامل الأس: **.
 *        (EN) Parses power operator: **.
 */
ExprPtr ParserCore::parsePower() {
    auto expr = parsePostfix();
    
    if (match(TT::OP_POWER)) {
        Token op = previous();
        auto right = parsePower();  // Right associative
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل استدعاءات الدوال والوصول: f() obj.member arr[i].
 *        (EN) Parses function calls and access: f() obj.member arr[i].
 */
ExprPtr ParserCore::parsePostfix() {
    auto expr = parsePrimary();
    
    while (true) {
        if (match(TT::PAREN_LEFT)) {
            // Function call
            // (AR) استدعاء دالة
            auto args = parseArgumentList();
            consume(TT::PAREN_RIGHT, 
                "(AR) توقع ')' بعد الوسائط. (EN) Expected ')' after arguments.");
            expr = std::make_unique<CallExpr>(
                std::move(expr),
                std::move(args),
                previous().getPosition()
            );
        }
        else if (match(TT::DOT)) {
            // Member access or method call
            // (AR) الوصول لعضو أو استدعاء طريقة
            Token member = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم عضو بعد '.'. (EN) Expected member name after '.'.");
            
            // Check if this is a method call: obj.method()
            if (check(TT::PAREN_LEFT)) {
                match(TT::PAREN_LEFT);
                auto args = parseArgumentList();
                consume(TT::PAREN_RIGHT, 
                    "(AR) توقع ')' بعد وسائط الطريقة. (EN) Expected ')' after method arguments.");
                expr = std::make_unique<MethodCallExpr>(
                    std::move(expr),
                    member.getValue()
                );
                // Add arguments
                auto* methodCall = static_cast<MethodCallExpr*>(expr.get());
                methodCall->arguments = std::move(args);
            } else {
                // Regular member access: obj.field
                expr = std::make_unique<MemberExpr>(
                    std::move(expr),
                    member.getValue(),
                    member.getPosition()
                );
            }
        }
        else if (match(TT::BRACKET_LEFT)) {
            // Index access
            // (AR) الوصول بالفهرس
            auto index = parseExpression();
            consume(TT::BRACKET_RIGHT, 
                "(AR) توقع ']' بعد الفهرس. (EN) Expected ']' after index.");
            expr = std::make_unique<IndexExpr>(
                std::move(expr),
                std::move(index),
                previous().getPosition()
            );
        }
        else {
            break;
        }
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل التعبيرات الأساسية: أرقام، نصوص، متغيرات.
 *        (EN) Parses primary expressions: numbers, strings, variables.
 */
ExprPtr ParserCore::parsePrimary() {
    // Await expression: await expr
    // (AR) تعبير الانتظار: انتظر تعبير
    if (match(TT::KEYWORD_AWAIT)) {
        Token awaitToken = previous();
        auto expr = parseTernary();  // (AR) تحليل تعبير ذو أولوية أعلى / (EN) Parse higher precedence expression
        if (!expr) {
            errorBilingual(
                "خطأ نحوي: يجب أن يتبع 'انتظر' بتعبير.",
                "Syntax error: 'await' must be followed by an expression."
            );
            return nullptr;
        }
        return std::make_unique<AwaitExpr>(std::move(expr), awaitToken.getPosition());
    }
    
    // OOP: new expression for object instantiation
    // (AR) تعبير جديد لإنشاء كائن
    if (match(TT::KEYWORD_NEW)) {
        return parseNewExpr();
    }
    
    // OOP: this expression for current object
    // (AR) تعبير هذا للكائن الحالي
    if (match(TT::KEYWORD_THIS)) {
        return std::make_unique<ThisExpr>();
    }
    
    // OOP: super expression for base class
    // (AR) تعبير الأساس للصنف الأب
    if (match(TT::KEYWORD_SUPER)) {
        return std::make_unique<SuperExpr>();
    }
    
    // Lambda expression: lambda x: x + 1
    // (AR) تعبير لامدا
    if (match(TT::KEYWORD_LAMBDA)) {
        return parseLambda();
    }
    
    // Arrow function: (x, y) => x + y  OR  x => x * 2
    // (AR) دالة سهمية: (x, y) => x + y  أو  x => x * 2
    if (isArrowFunction()) {
        return parseArrowFunction();
    }
    
    // Literals
    // (AR) القيم الحرفية
    
    if (match(TT::LITERAL_TRUE)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    if (match(TT::LITERAL_FALSE)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    if (match(TT::LITERAL_NULL)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    if (match(TT::NUMBER_INTEGER) || match(TT::NUMBER_DOUBLE)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    // STRING_LITERAL: Regular strings / النصوص العادية
    // (AR) معالجة النص الحرفي العادي
    if (match(TT::STRING_LITERAL)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    // STRING_RAW: Raw strings r"..." treat as regular string
    // (AR) النصوص الخام r"..." تُعامل كنص عادي (لا معالجة للـ escape)
    // Lexer already processed escapes (or didn't), just use as-is
    // المحلل المعجمي بالفعل عالج (أو لم يعالج) الـ escapes، استخدمها كما هي
    if (match(TT::STRING_RAW)) {
        auto rawToken = previous();
        // Convert STRING_RAW token to STRING_LITERAL for interpreter
        // تحويل رمز STRING_RAW إلى STRING_LITERAL للمفسر
        Lexer::Token stringToken(TT::STRING_LITERAL, rawToken.getValue(), rawToken.getPosition());
        return std::make_unique<LiteralExpr>(stringToken);
    }
    
    // STRING_FSTRING: F-strings f"{expr}" - convert to string concatenation
    // (AR) النصوص المنسقة f"{expr}" - تحويل إلى تسلسل نصوص
    // This is a simplified implementation - full implementation would parse expressions
    // هذا تنفيذ مبسط - التنفيذ الكامل يحلل التعبيرات داخل الأقواس
    if (match(TT::STRING_FSTRING)) {
        auto fstringToken = previous();
        std::string fstring = fstringToken.getValue();
        
        // (AR) تحليل f-string — يدعم التعبيرات المدمجة {تعبير}
        // (EN) F-string parsing — supports embedded expressions {expr}
        
        // Check if f-string contains expressions {...}
        // تحقق إذا كان f-string يحتوي على تعبيرات {...}
        if (fstring.find('{') != std::string::npos) {
            // Parse and expand f-string expressions
            // تحليل وتوسيع تعبيرات f-string
            return parseFStringExpr(fstring, fstringToken.getPosition());
        } else {
            // No expressions, treat as regular string
            // لا يوجد تعبيرات، عامله كنص عادي
            Lexer::Token stringToken(TT::STRING_LITERAL, fstring, fstringToken.getPosition());
            return std::make_unique<LiteralExpr>(stringToken);
        }
    }
    
    // Variable reference or Template Instantiation
    // (AR) مرجع متغير أو تنفيذ قالب
    if (match(TT::IDENTIFIER)) {
        std::string identName = previous().getValue();
        auto identPos = previous().getPosition();
        
        // (AR) تحقق إذا كان تنفيذ قالب: اسم<نوع>
        // (EN) Check if template instantiation: name<type>
        // (AR) نتحقق أن الرمز بعد < هو نوع مدمج فقط (رقم، نص، إلخ)
        //      لتجنب التعارض مع عامل المقارنة <
        // (EN) Only attempt template parsing if the token after < is a
        //      built-in type keyword, to avoid conflict with < operator
        if (check(TT::OP_LESS) && isTypeToken(nextToken_.getType())) {
            // (AR) حاول تحليل كتنفيذ قالب
            // (EN) Try to parse as template instantiation
            auto templateExpr = parseTemplateInstantiation(identName, identPos);
            if (templateExpr) {
                return templateExpr;
            }
            // (AR) إذا فشل، اعتبره متغير عادي
            // (EN) If failed, treat as regular variable
        }
        
        return std::make_unique<VariableExpr>(
            identName,
            identPos
        );
    }
    
    // Parenthesized expression or multi-param arrow function
    // (AR) تعبير بين أقواس أو دالة سهمية متعددة المعاملات
    if (match(TT::PAREN_LEFT)) {
        auto leftPos = previous().getPosition();
        
        // (AR) التحقق من (معرّف، ...) => جسم — دالة سهمية متعددة المعاملات
        // (EN) Check for (identifier, ...) => body — multi-param arrow function
        // When IDENTIFIER is followed by COMMA, it can't be a valid expression (comma isn't a binary op)
        if (check(TT::IDENTIFIER) && (peekNext().getType() == TT::COMMA || peekNext().getType() == TT::ARABIC_COMMA)) {
            // (AR) هذا نمط دالة سهمية متعددة المعاملات
            // (EN) This is a multi-param arrow function pattern
            std::vector<Parameter> params;
            do {
                Token paramName = consume(TT::IDENTIFIER,
                    "(AR) توقعت اسم معامل في الدالة السهمية. (EN) Expected parameter name in arrow function.");
                params.emplace_back(paramName.getValue(), Data::DataType::UNKNOWN, nullptr);
            } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));
            
            consume(TT::PAREN_RIGHT,
                "(AR) توقع ')' بعد معاملات الدالة السهمية. (EN) Expected ')' after arrow function parameters.");
            consume(TT::FAT_ARROW,
                "(AR) توقع '=>' بعد معاملات الدالة السهمية. (EN) Expected '=>' after arrow function parameters.");
            
            auto body = parseExpression();
            return std::make_unique<LambdaExpr>(std::move(params), std::move(body), leftPos);
        }
        
        auto expr = parseExpression();
        
        // Check if expression parsed successfully
        // (AR) تحقق إذا تم تحليل التعبير بنجاح
        if (!expr) {
            errorBilingual(
                "تعبير غير صحيح بين الأقواس. تأكد من أن التعبير يحتوي على قيم أو عمليات صحيحة.",
                "Invalid expression in parentheses. Make sure the expression contains valid values or operations."
            );
            return nullptr;
        }
        
        consume(TT::PAREN_RIGHT, 
            "(AR) توقع ')' بعد التعبير. تأكد من إغلاق جميع الأقواس بشكل صحيح. (EN) Expected ')' after expression. Make sure all parentheses are properly closed.");
        
        // (AR) التحقق من (x) => جسم — دالة سهمية بمعامل واحد بين أقواس
        // (EN) Check for (x) => body — single-param arrow function with parens
        if (check(TT::FAT_ARROW)) {
            advance(); // consume =>
            if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
                std::vector<Parameter> params;
                params.emplace_back(varExpr->name, Data::DataType::UNKNOWN, nullptr);
                auto body = parseExpression();
                return std::make_unique<LambdaExpr>(std::move(params), std::move(body), leftPos);
            } else {
                errorBilingual(
                    "خطأ: معامل الدالة السهمية يجب أن يكون اسم متغير.",
                    "Error: arrow function parameter must be an identifier."
                );
                return nullptr;
            }
        }
        
        return expr;
    }
    
    // Array literal
    // (AR) مصفوفة حرفية
    if (match(TT::BRACKET_LEFT)) {
        auto result = parseArrayLiteral();
        if (!result) {
            errorBilingual(
                "خطأ في تحليل مصفوفة حرفية. تأكد من صيغة المصفوفة: [عنصر1، عنصر2، ...] أو [x for x in list]",
                "Error parsing array literal. Make sure array syntax is correct: [elem1, elem2, ...] or [x for x in list]"
            );
            return nullptr;
        }
        return result;
    }
    
    // Map literal
    // (AR) خريطة حرفية
    if (match(TT::BRACE_LEFT)) {
        auto result = parseMapLiteral();
        if (!result) {
            errorBilingual(
                "خطأ في تحليل خريطة حرفية. تأكد من صيغة الخريطة: {مفتاح: قيمة، ...}",
                "Error parsing map literal. Make sure map syntax is correct: {key: value, ...}"
            );
            return nullptr;
        }
        return result;
    }
    
    // Type keywords used as expressions (e.g., نص(...) as function call)
    // (AR) كلمات أنواع مستخدمة كتعبيرات (مثلاً نص(...) كاستدعاء دالة)
    // This allows type names to be used as function names in expression position
    if (isTypeToken(current_.getType())) {
        auto tok = current_;
        advance();
        return std::make_unique<VariableExpr>(tok.getValue(), tok.getPosition());
    }

    // Provide more specific error message based on what we found
    // (AR) قدم رسالة خطأ أكثر تحديداً بناءً على ما وجدنا
    Token current = peek();
    std::string errorMsg_ar = "خطأ نحوي: توقعت تعبيراً (رقم، نص، معرّف، إلخ).";
    std::string errorMsg_en = "Syntax error: expected expression (number, string, identifier, etc.).";
    
    // Provide specific guidance based on the token type
    // (AR) قدم توجيهات محددة بناءً على نوع الرمز
    switch (current.getType()) {
        case TT::END_OF_FILE:
            errorMsg_ar = "خطأ: نهاية ملف غير متوقعة - تعبير غير مكتمل.";
            errorMsg_en = "Error: unexpected end of file - incomplete expression.";
            break;
        case TT::SEMICOLON:
            errorMsg_ar = "خطأ: فاصلة منقوطة بدون تعبير سابق. هل نسيت التعبير قبلها؟";
            errorMsg_en = "Error: semicolon without preceding expression. Did you forget the expression?";
            break;
        case TT::KEYWORD_END:
            errorMsg_ar = "خطأ: توقعت تعبيراً قبل كلمة 'نهاية'.";
            errorMsg_en = "Error: expected expression before 'end' keyword.";
            break;
        case TT::BRACE_RIGHT:
            errorMsg_ar = "خطأ: قوس معقوف إغلاق بدون تعبير. هل نسيت العنصر الأخير؟";
            errorMsg_en = "Error: closing brace without expression. Did you forget the last element?";
            break;
        case TT::BRACKET_RIGHT:
            errorMsg_ar = "خطأ: قوس مربع إغلاق بدون عنصر. هل نسيت العنصر الأخير؟";
            errorMsg_en = "Error: closing bracket without element. Did you forget the last element?";
            break;
        case TT::PAREN_RIGHT:
            errorMsg_ar = "خطأ: قوس إغلاق بدون تعبير. هل نسيت الوسيط أو التعبير؟";
            errorMsg_en = "Error: closing parenthesis without expression. Did you forget the argument or expression?";
            break;
        case TT::COMMA:
            errorMsg_ar = "خطأ: فاصلة بدون عنصر سابق. هل نسيت عنصراً أو معامل؟";
            errorMsg_en = "Error: comma without preceding element. Did you forget an element or argument?";
            break;
        default:
            errorMsg_ar = "خطأ نحوي: رمز غير متوقع '" + current.getValue() + "' في موضع تعبير.";
            errorMsg_en = "Syntax error: unexpected token '" + current.getValue() + "' in expression position.";
            break;
    }
    
    errorBilingual(errorMsg_ar, errorMsg_en);
    
    // (AR) استهلاك الرمز غير الصالح لمنع الحلقة اللانهائية
    // (EN) Consume the invalid token to prevent infinite loop
    // CRITICAL: Must advance to prevent infinite parsing loop
    if (!isAtEnd()) {
        advance();
    }
    
    return nullptr;
}

/**
 * @brief (AR) يحلل دالة لامدا: lambda x: x + 1.
 *        (EN) Parses lambda function: lambda x: x + 1.
 */
ExprPtr ParserCore::parseLambda() {
    // Parse parameters
    // (AR) تحليل المعاملات
    std::vector<std::string> paramNames;
    if (!check(TT::COLON)) {
        paramNames = parseParameterList();
        if (paramNames.empty() && !check(TT::COLON)) {
            errorBilingual(
                "خطأ: فشل تحليل قائمة معاملات lambda أو توقعت ':'.",
                "Error: failed to parse lambda parameter list or expected ':'."
            );
            return nullptr;
        }
    }
    
    // Check for colon separator
    // (AR) التحقق من ':' الفاصل
    if (!check(TT::COLON)) {
        errorBilingual(
            "خطأ: توقعت ':' بعد معاملات lambda. الصيغة: (x: x * 2) أو (x, y: x + y)",
            "Error: expected ':' after lambda parameters. Format: (x: x * 2) or (x, y: x + y)"
        );
        return nullptr;
    }
    consume(TT::COLON, "");
    
    // Parse body expression
    // (AR) تحليل تعبير الجسم
    auto body = parseExpression();
    if (!body) {
        errorBilingual(
            "خطأ: فشل تحليل جسم lambda - تعبير غير صحيح.",
            "Error: failed to parse lambda body - invalid expression."
        );
        return nullptr;
    }
    
    // Convert param names to Parameter objects
    // (AR) تحويل أسماء المعاملات إلى كائنات Parameter
    std::vector<Parameter> params;
    params.reserve(paramNames.size());
    for (const auto& name : paramNames) {
        params.emplace_back(name, Data::DataType::UNKNOWN, nullptr);
    }
    
    // Create lambda expression node
    // (AR) إنشاء عقدة تعبير Lambda
    return std::make_unique<LambdaExpr>(
        std::move(params),
        std::move(body),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل f-string ويحوله إلى تسلسل نصوص.
 *        (EN) Parses f-string and converts to string concatenation.
 * 
 * Converts f"Hello {name}!" to: "Hello " + str(name) + "!"
 * يحول f"مرحبا {الاسم}!" إلى: "مرحبا " + str(الاسم) + "!"
 */
ExprPtr ParserCore::parseFStringExpr(const std::string& fstring, const Lexer::Position& pos) {
    // Build concatenation of string parts and expressions
    // بناء تسلسل من أجزاء النصوص والتعبيرات
    
    ExprPtr result = nullptr;
    size_t i = 0;
    std::string currentText;
    
    while (i < fstring.length()) {
        if (fstring[i] == '{' && (i + 1 < fstring.length() && fstring[i + 1] != '{')) {
            // Found expression start
            // وجدنا بداية تعبير
            
            // Add accumulated text as string literal
            // أضف النص المتراكم كنص حرفي
            if (!currentText.empty()) {
                auto textLiteral = std::make_unique<LiteralExpr>(
                    Lexer::Token(TT::STRING_LITERAL, currentText, pos)
                );
                if (result) {
                    // Concatenate with previous parts
                    // تسلسل مع الأجزاء السابقة
                    result = std::make_unique<BinaryExpr>(
                        std::move(result),
                        TT::OP_PLUS,
                        std::move(textLiteral),
                        pos
                    );
                } else {
                    result = std::move(textLiteral);
                }
                currentText.clear();
            }
            
            // Find closing brace
            // ابحث عن القوس الإغلاق
            size_t exprStart = i + 1;
            size_t braceDepth = 1;
            i++;
            while (i < fstring.length() && braceDepth > 0) {
                if (fstring[i] == '{') braceDepth++;
                else if (fstring[i] == '}') braceDepth--;
                if (braceDepth > 0) i++;
            }
            
            if (braceDepth != 0) {
                errorBilingual(
                    "خطأ: قوس غير مُغلق في f-string. تأكد من إغلاق جميع الأقواس {...}",
                    "Error: unclosed brace in f-string. Make sure all braces {...} are closed."
                );
                return nullptr;
            }
            
            // Extract expression text
            // استخراج نص التعبير
            std::string exprText = fstring.substr(exprStart, i - exprStart);
            
            // (AR) تحليل التعبير الكامل من النص باستخدام محلل فرعي
            // (EN) Parse full expression from text using a sub-parser
            ExprPtr exprNode = nullptr;
            try {
                Lexer::LexerCore exprLexer(exprText);
                ParserCore exprParser(exprLexer);
                exprNode = exprParser.parseExpression();
            } catch (...) {
                // (AR) في حالة فشل تحليل التعبير، استخدم كاسم متغير
                // (EN) If expression parsing fails, fall back to variable name
                exprNode = std::make_unique<VariableExpr>(exprText, pos);
            }
            
            if (!exprNode) {
                exprNode = std::make_unique<VariableExpr>(exprText, pos);
            }
            
            // Wrap in str() call to convert to string
            // لف في استدعاء str() للتحويل إلى نص
            std::vector<ExprPtr> strArgs;
            strArgs.push_back(std::move(exprNode));
            auto strCall = std::make_unique<CallExpr>(
                std::make_unique<VariableExpr>("str", pos),
                std::move(strArgs)
            );
            
            if (result) {
                // Concatenate with previous parts
                // تسلسل مع الأجزاء السابقة
                result = std::make_unique<BinaryExpr>(
                    std::move(result),
                    TT::OP_PLUS,
                    std::move(strCall),
                    pos
                );
            } else {
                result = std::move(strCall);
            }
            
            i++; // Move past closing brace
        }
        else if (fstring[i] == '{' && i + 1 < fstring.length() && fstring[i + 1] == '{') {
            // Escaped brace {{
            // قوس مُتجاوز {{
            currentText += '{';
            i += 2;
        }
        else if (fstring[i] == '}' && i + 1 < fstring.length() && fstring[i + 1] == '}') {
            // Escaped brace }}
            // قوس مُتجاوز }}
            currentText += '}';
            i += 2;
        }
        else {
            // Regular character
            // حرف عادي
            currentText += fstring[i];
            i++;
        }
    }
    
    // Add any remaining text
    // أضف أي نص متبقي
    if (!currentText.empty()) {
        auto textLiteral = std::make_unique<LiteralExpr>(
            Lexer::Token(TT::STRING_LITERAL, currentText, pos)
        );
        if (result) {
            result = std::make_unique<BinaryExpr>(
                std::move(result),
                TT::OP_PLUS,
                std::move(textLiteral),
                pos
            );
        } else {
            result = std::move(textLiteral);
        }
    }
    
    return result ? std::move(result) : std::make_unique<LiteralExpr>(
        Lexer::Token(TT::STRING_LITERAL, "", pos)
    );
}

/**
 * @brief (AR) يحلل مُزخرِف (decorator): @decorator أو @decorator(args).
 *        (EN) Parses decorator: @decorator or @decorator(args).
 */
ExprPtr ParserCore::parseDecorator() {
    // Expect @ sign (should already be consumed by caller)
    // (AR) نتوقع @ (يجب أن يكون مُستهلكاً من المُستدعي)
    Token atSign = previous();
    
    // Expect decorator name
    // (AR) نتوقع اسم المُزخرِف
    if (!check(TT::IDENTIFIER)) {
        errorBilingual(
            "خطأ: توقعت اسم المُزخرِف بعد @. مثال: @property أو @staticmethod",
            "Error: expected decorator name after @. Example: @property or @staticmethod"
        );
        return nullptr;
    }
    Token decoratorName = peek();
    advance();
    
    // Check for arguments
    // (AR) التحقق من الوسائط
    if (check(TT::PAREN_LEFT)) {
        // Decorator with arguments: @decorator(arg1, arg2, ...)
        // (AR) مُزخرِف مع وسائط
        advance(); // consume (
        
        ExprList args;
        
        // Parse arguments if not empty
        // (AR) تحليل الوسائط إذا لم تكن فارغة
        if (!check(TT::PAREN_RIGHT)) {
            args = parseArgumentList();
            if (args.empty() && !check(TT::PAREN_RIGHT)) {
                errorBilingual(
                    "خطأ: فشل تحليل وسائط المُزخرِف.",
                    "Error: failed to parse decorator arguments."
                );
                return nullptr;
            }
        }
        
        if (!check(TT::PAREN_RIGHT)) {
            errorBilingual(
                "خطأ: توقعت ')' بعد وسائط المُزخرِف. هل نسيت إغلاق الأقواس؟",
                "Error: expected ')' after decorator arguments. Did you forget to close the parenthesis?"
            );
            return nullptr;
        }
        consume(TT::PAREN_RIGHT, "");
        
        // Create decorator with arguments
        // (AR) إنشاء مُزخرِف مع وسائط
        return std::make_unique<DecoratorExpr>(
            decoratorName.getValue(),
            std::move(args),
            atSign.getPosition()
        );
    }
    
    // Decorator without arguments: @decorator
    // (AR) مُزخرِف بدون وسائط
    return std::make_unique<DecoratorExpr>(
        decoratorName.getValue(),
        atSign.getPosition()
    );
}


} // namespace Parser
} // namespace Sad
