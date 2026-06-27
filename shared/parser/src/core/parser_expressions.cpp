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
#include "directive_nodes.h"
#include <iostream>
#include <sstream>

namespace Sad
{
    namespace Parser
    {

        using namespace AST;
        using namespace Lexer;
        using TT = TokenType;

        /**
         * @brief (AR) يحلل تعبير - نقطة الدخول.
         *        (EN) Parses expression - entry point.
         */
        ExprPtr ParserCore::parseExpression()
        {
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
        ExprPtr ParserCore::parsePipeline()
        {
            auto expr = parseAssignment();

            while (match(TT::OP_PIPE_ARROW))
            {
                auto pos = previous().getPosition();
                auto right = parseAssignment();

                if (!right)
                {
                    errorBilingual(
                        "خطأ: توقعت تعبير بعد '|>'.",
                        "Error: expected expression after '|>'.");
                    return nullptr;
                }

                // Desugar: if right is a CallExpr, prepend expr as first argument
                // (AR) إزالة السكر: إذا كان الجانب الأيمن استدعاء دالة، أدخل التعبير كأول معامل
                if (auto *call = dynamic_cast<CallExpr *>(right.get()))
                {
                    // a |> f(b, c) → f(a, b, c)
                    ExprList newArgs;
                    newArgs.push_back(std::move(expr));
                    for (auto &arg : call->arguments)
                    {
                        newArgs.push_back(std::move(arg));
                    }
                    expr = std::make_unique<CallExpr>(
                        std::move(call->callee),
                        std::move(newArgs),
                        pos);
                }
                else
                {
                    // a |> f → f(a)
                    ExprList args;
                    args.push_back(std::move(expr));
                    expr = std::make_unique<CallExpr>(
                        std::move(right),
                        std::move(args),
                        pos);
                }
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل تعيين: اسم = قيمة، أو Walrus: اسم := قيمة.
         *        (EN) Parses assignment: name = value, or Walrus: name := value.
         */
        ExprPtr ParserCore::parseAssignment()
        {
            auto expr = parseTernary();

            // (AR) فحص عامل Walrus := (إسناد داخل تعبير) / (EN) Check for Walrus operator := (assignment expression)
            if (match(TT::OP_WALRUS))
            {
                Token walrus = previous();
                auto value = parseAssignment();

                // (AR) Walrus يتطلب متغيراً على اليسار / (EN) Walrus requires variable on left
                if (auto *var = dynamic_cast<VariableExpr *>(expr.get()))
                {
                    return std::make_unique<WalrusExpr>(
                        var->name,
                        std::move(value),
                        walrus.getPosition());
                }

                errorBilingual(
                    "خطأ: عامل Walrus (:=) يتطلب اسم متغير على اليسار",
                    "Error: Walrus operator (:=) requires variable name on left");
            }

            // Check for assignment operator
            // (AR) التحقق من عامل التعيين العادي
            if (match(TT::OP_ASSIGN))
            {
                Token equals = previous();
                auto value = parseAssignment();

                // Left side must be a variable or member access
                // (AR) الجانب الأيسر يجب أن يكون متغيراً أو وصول لعضو
                if (auto *var = dynamic_cast<VariableExpr *>(expr.get()))
                {
                    return std::make_unique<AssignExpr>(
                        var->name,
                        std::move(value),
                        equals.getPosition());
                }

                // Support member access assignment: obj.field = value
                // (AR) دعم تعيين قيمة لعضو الكائن
                if (auto *member = dynamic_cast<MemberExpr *>(expr.get()))
                {
                    // Extract object and member name, then create MemberAssignExpr
                    // Need to clone the object expression since we're consuming expr
                    ExprPtr objectCopy;

                    // We need to transfer ownership properly
                    // Release the MemberExpr and extract its parts
                    std::unique_ptr<MemberExpr> memberPtr(static_cast<MemberExpr *>(expr.release()));

                    return std::make_unique<MemberAssignExpr>(
                        std::move(memberPtr->object),
                        memberPtr->member,
                        std::move(value),
                        equals.getPosition());
                }

                // دعم الإسناد بالفهرس: م[0] = قيمة ، قاموس["مفتاح"] = قيمة
                // (EN) Support index assignment: arr[0] = value, map["key"] = value
                if (auto *indexExpr = dynamic_cast<IndexExpr *>(expr.get()))
                {
                    std::unique_ptr<IndexExpr> indexPtr(static_cast<IndexExpr *>(expr.release()));

                    return std::make_unique<IndexAssignExpr>(
                        std::move(indexPtr->object),
                        std::move(indexPtr->index),
                        std::move(value),
                        equals.getPosition());
                }

                errorBilingual(
                    "خطأ: هدف الإسناد غير صالح - يجب أن يكون معرّفاً أو حقل كائن أو فهرس مصفوفة",
                    "Error: invalid assignment target - must be identifier, object field, or array index");
            }

            // ========================================================================
            // (AR) عمليات الإسناد المركبة: += -= *= /= %=
            // (EN) Compound assignment operators: += -= *= /= %=
            // تحويل نحوي: x += y → x = x + y
            // Desugaring: x += y → x = x + y
            // ========================================================================
            if (check(TT::OP_PLUS_ASSIGN) || check(TT::OP_MINUS_ASSIGN) ||
                check(TT::OP_MULTIPLY_ASSIGN) || check(TT::OP_DIVIDE_ASSIGN) ||
                check(TT::OP_FLOOR_DIVIDE_ASSIGN) || check(TT::OP_MODULO_ASSIGN))
            {

                Token opToken = current_;
                advance(); // consume the compound operator

                auto rhs = parseAssignment();

                // (AR) تحديد العامل الأساسي / (EN) Determine base operator
                TokenType binOp;
                switch (opToken.getType())
                {
                case TT::OP_PLUS_ASSIGN:
                    binOp = TT::OP_PLUS;
                    break;
                case TT::OP_MINUS_ASSIGN:
                    binOp = TT::OP_MINUS;
                    break;
                case TT::OP_MULTIPLY_ASSIGN:
                    binOp = TT::OP_MULTIPLY;
                    break;
                case TT::OP_DIVIDE_ASSIGN:
                    binOp = TT::OP_DIVIDE;
                    break;
                case TT::OP_FLOOR_DIVIDE_ASSIGN:
                    binOp = TT::OP_FLOOR_DIVIDE;
                    break;
                case TT::OP_MODULO_ASSIGN:
                    binOp = TT::OP_MODULO;
                    break;
                default:
                    binOp = TT::OP_PLUS;
                    break;
                }

                // (AR) الجانب الأيسر متغير: x += y → x = x + y
                if (auto *var = dynamic_cast<VariableExpr *>(expr.get()))
                {
                    std::string varName = var->name;
                    auto varCopy = std::make_unique<VariableExpr>(varName, opToken.getPosition());
                    auto binExpr = std::make_unique<BinaryExpr>(
                        std::move(varCopy), binOp, std::move(rhs), opToken.getPosition());
                    return std::make_unique<AssignExpr>(
                        varName, std::move(binExpr), opToken.getPosition());
                }

                // (AR) الجانب الأيسر حقل كائن: obj.field += y
                if (auto *member = dynamic_cast<MemberExpr *>(expr.get()))
                {
                    std::string memberName = member->member;
                    std::unique_ptr<MemberExpr> memberPtr(static_cast<MemberExpr *>(expr.release()));
                    // Build read copy: create new MemberExpr from same object
                    ExprPtr readObj;
                    if (auto *objVar = dynamic_cast<VariableExpr *>(memberPtr->object.get()))
                    {
                        readObj = std::make_unique<VariableExpr>(objVar->name, opToken.getPosition());
                    }
                    else if (dynamic_cast<ThisExpr *>(memberPtr->object.get()))
                    {
                        // (AR) دعم هذا.حقل += قيمة / (EN) Support this.field += value
                        readObj = std::make_unique<ThisExpr>();
                    }
                    else if (auto *objMem = dynamic_cast<MemberExpr *>(memberPtr->object.get()))
                    {
                        // (AR) دعم كائن.حقل1.حقل2 += قيمة / (EN) Support obj.field1.field2 += value
                        if (auto *innerVar = dynamic_cast<VariableExpr *>(objMem->object.get()))
                        {
                            auto innerCopy = std::make_unique<VariableExpr>(innerVar->name, opToken.getPosition());
                            readObj = std::make_unique<MemberExpr>(std::move(innerCopy), objMem->member, opToken.getPosition());
                        }
                        else if (dynamic_cast<ThisExpr *>(objMem->object.get()))
                        {
                            auto innerCopy = std::make_unique<ThisExpr>();
                            readObj = std::make_unique<MemberExpr>(std::move(innerCopy), objMem->member, opToken.getPosition());
                        }
                    }
                    else if (auto *objIdx = dynamic_cast<IndexExpr *>(memberPtr->object.get()))
                    {
                        // ═══════════════════════════════════════════════════════════════
                        // (AR) إصلاح المشكلة 16: دعم مصفوفة[فهرس].حقل += قيمة
                        //      مثال: العمليات[فهرس].أبناء += [قيمة_جديدة]
                        //
                        // (EN) Fix issue 16: Support array[index].field += value
                        //      Example: processes[idx].children += [newValue]
                        // ═══════════════════════════════════════════════════════════════
                        ExprPtr readArrObj;
                        ExprPtr readArrIdx;

                        // Rebuild the array object reference
                        if (auto *arrVar = dynamic_cast<VariableExpr *>(objIdx->object.get()))
                        {
                            readArrObj = std::make_unique<VariableExpr>(arrVar->name, opToken.getPosition());
                        }

                        // Rebuild the index
                        if (auto *idxLit = dynamic_cast<LiteralExpr *>(objIdx->index.get()))
                        {
                            readArrIdx = std::make_unique<LiteralExpr>(idxLit->token);
                        }
                        else if (auto *idxVar = dynamic_cast<VariableExpr *>(objIdx->index.get()))
                        {
                            readArrIdx = std::make_unique<VariableExpr>(idxVar->name, opToken.getPosition());
                        }

                        if (readArrObj && readArrIdx)
                        {
                            readObj = std::make_unique<IndexExpr>(
                                std::move(readArrObj), std::move(readArrIdx), opToken.getPosition());
                        }
                    }
                    if (readObj)
                    {
                        auto readMember = std::make_unique<MemberExpr>(
                            std::move(readObj), memberName, opToken.getPosition());
                        auto binExpr = std::make_unique<BinaryExpr>(
                            std::move(readMember), binOp, std::move(rhs), opToken.getPosition());
                        return std::make_unique<MemberAssignExpr>(
                            std::move(memberPtr->object), memberName, std::move(binExpr), opToken.getPosition());
                    }
                }

                // (AR) الجانب الأيسر عنصر مصفوفة/خريطة: arr[i] += y → arr[i] = arr[i] + y
                // (EN) Left side is array/map index: arr[i] += y → arr[i] = arr[i] + y
                if (auto *indexExpr = dynamic_cast<IndexExpr *>(expr.get()))
                {
                    std::unique_ptr<IndexExpr> indexPtr(static_cast<IndexExpr *>(expr.release()));
                    // Build read copy: reconstruct the index access for the read side
                    ExprPtr readObj;
                    ExprPtr readIdx;
                    if (auto *objVar = dynamic_cast<VariableExpr *>(indexPtr->object.get()))
                    {
                        readObj = std::make_unique<VariableExpr>(objVar->name, opToken.getPosition());
                    }
                    else if (dynamic_cast<ThisExpr *>(indexPtr->object.get()))
                    {
                        // (AR) دعم هذا[فهرس] += قيمة / (EN) Support this[index] += value
                        readObj = std::make_unique<ThisExpr>();
                    }
                    else if (auto *objMem = dynamic_cast<MemberExpr *>(indexPtr->object.get()))
                    {
                        if (auto *innerVar = dynamic_cast<VariableExpr *>(objMem->object.get()))
                        {
                            auto innerCopy = std::make_unique<VariableExpr>(innerVar->name, opToken.getPosition());
                            readObj = std::make_unique<MemberExpr>(std::move(innerCopy), objMem->member, opToken.getPosition());
                        }
                        else if (dynamic_cast<ThisExpr *>(objMem->object.get()))
                        {
                            // (AR) دعم هذا.حقل[فهرس] += قيمة / (EN) Support this.field[index] += value
                            auto innerCopy = std::make_unique<ThisExpr>();
                            readObj = std::make_unique<MemberExpr>(std::move(innerCopy), objMem->member, opToken.getPosition());
                        }
                    }
                    if (auto *idxLit = dynamic_cast<LiteralExpr *>(indexPtr->index.get()))
                    {
                        readIdx = std::make_unique<LiteralExpr>(idxLit->token);
                    }
                    else if (auto *idxVar = dynamic_cast<VariableExpr *>(indexPtr->index.get()))
                    {
                        readIdx = std::make_unique<VariableExpr>(idxVar->name, opToken.getPosition());
                    }
                    if (readObj && readIdx)
                    {
                        auto readIndex = std::make_unique<IndexExpr>(
                            std::move(readObj), std::move(readIdx), opToken.getPosition());
                        auto binExpr = std::make_unique<BinaryExpr>(
                            std::move(readIndex), binOp, std::move(rhs), opToken.getPosition());
                        return std::make_unique<IndexAssignExpr>(
                            std::move(indexPtr->object), std::move(indexPtr->index),
                            std::move(binExpr), opToken.getPosition());
                    }
                }

                errorBilingual(
                    "خطأ: هدف الإسناد المركب غير صالح - يجب أن يكون متغيراً أو حقل كائن أو عنصر مصفوفة",
                    "Error: invalid compound assignment target - must be variable, object field, or array index");
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل التعبير الثلاثي الشرطي: شرط ? صحيح : خطأ.
         *        (EN) Parses ternary conditional: condition ? true : false.
         *
         * Grammar / القواعد:
         *   ternary → null_coalesce ("?" expression ":" ternary)?
         *
         * Right-associative: a ? b : c ? d : e → a ? b : (c ? d : e)
         */
        ExprPtr ParserCore::parseTernary()
        {
            auto expr = parseNullCoalesce();

            // Check for ternary operator ?
            // (AR) التحقق من عامل الثلاثي ؟
            if (match(TT::QUESTION))
            {
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
                    questionToken.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل عامل التجميع الفارغ: تعبير1 ?? تعبير2
         *        (EN) Parses null coalescing: expr1 ?? expr2
         */
        ExprPtr ParserCore::parseNullCoalesce()
        {
            auto expr = parseLogicalOr();

            while (match(TT::QUESTION_QUESTION))
            {
                Token op = previous();
                auto right = parseLogicalOr();
                expr = std::make_unique<NullCoalesceExpr>(
                    std::move(expr),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل عامل OR المنطقي: تعبير1 || تعبير2.
         *        (EN) Parses logical OR: expr1 || expr2.
         */
        ExprPtr ParserCore::parseLogicalOr()
        {
            auto expr = parseLogicalAnd();

            while (match(TT::OP_OR))
            {
                Token op = previous();
                auto right = parseLogicalAnd();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل عامل AND المنطقي: تعبير1 && تعبير2.
         *        (EN) Parses logical AND: expr1 && expr2.
         */
        ExprPtr ParserCore::parseLogicalAnd()
        {
            auto expr = parseBitwiseOr();

            while (match(TT::OP_AND))
            {
                Token op = previous();
                auto right = parseBitwiseOr();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل عامل OR البتّي: | .
         *        (EN) Parses bitwise OR operator: | .
         */
        ExprPtr ParserCore::parseBitwiseOr()
        {
            auto expr = parseBitwiseXor();

            while (match(TT::OP_BITWISE_OR))
            {
                Token op = previous();
                auto right = parseBitwiseXor();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل عامل XOR البتّي: ^.
         *        (EN) Parses bitwise XOR operator: ^.
         */
        ExprPtr ParserCore::parseBitwiseXor()
        {
            auto expr = parseBitwiseAnd();

            while (match(TT::OP_XOR))
            {
                Token op = previous();
                auto right = parseBitwiseAnd();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل عامل AND البتّي: &.
         *        (EN) Parses bitwise AND operator: &.
         */
        ExprPtr ParserCore::parseBitwiseAnd()
        {
            auto expr = parseEquality();

            while (check(TT::OP_BITWISE_AND))
            {
                advance();
                Token op = previous();
                auto right = parseEquality();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    TT::OP_BITWISE_AND,
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل عوامل المساواة: == !=.
         *        (EN) Parses equality operators: == !=.
         */
        ExprPtr ParserCore::parseEquality()
        {
            auto expr = parseComparison();

            while (matchAny({TT::OP_EQUAL, TT::OP_NOT_EQUAL}))
            {
                Token op = previous();
                auto right = parseComparison();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل عوامل المقارنة: < <= > >=.
         *        (EN) Parses comparison operators: < <= > >=.
         */
        ExprPtr ParserCore::parseComparison()
        {
            auto expr = parseRange();

            while (matchAny({TT::OP_LESS, TT::OP_LESS_EQUAL,
                             TT::OP_GREATER, TT::OP_GREATER_EQUAL,
                             TT::KEYWORD_IN}))
            {
                Token op = previous();
                auto right = parseRange();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل تعبير المدى: بداية..نهاية أو بداية.. (مدى مفتوح)
         *        (EN) Parses range expression: start..end or start.. (open-ended range)
         */
        ExprPtr ParserCore::parseRange()
        {
            auto expr = parseTerm();

            if (match(TT::DOT_DOT))
            {
                auto pos = previous().getPosition();
                // (AR) دعم المدى المفتوح: 1.. بدون نهاية (مثل: array[1..])
                // (EN) Support open-ended range: 1.. without end (e.g., array[1..])
                ExprPtr end = nullptr;
                if (!check(TT::BRACKET_RIGHT) && !check(TT::PAREN_RIGHT) &&
                    !checkComma() && !isAtEnd())
                {
                    end = parseTerm();
                }
                expr = std::make_unique<RangeExpr>(std::move(expr), std::move(end), pos);
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل الجمع والطرح: + -.
         *        (EN) Parses addition and subtraction: + -.
         */
        ExprPtr ParserCore::parseTerm()
        {
            auto expr = parseFactor();

            while (matchAny({TT::OP_PLUS, TT::OP_MINUS}))
            {
                Token op = previous();
                auto right = parseFactor();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            // (AR) إزاحة البت: << >>  / (EN) Bit shifts: << >>
            while (matchAny({TT::OP_SHIFT_LEFT, TT::OP_SHIFT_RIGHT}))
            {
                Token op = previous();
                auto right = parseFactor();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل الضرب والقسمة: * / %.
         *        (EN) Parses multiplication and division: * / %.
         */
        ExprPtr ParserCore::parseFactor()
        {
            auto expr = parseUnary();

            while (matchAny({TT::OP_MULTIPLY, TT::OP_DIVIDE, TT::OP_FLOOR_DIVIDE, TT::OP_MODULO}))
            {
                Token op = previous();
                auto right = parseUnary();
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل العوامل الأحادية: - ! ++ --.
         *        (EN) Parses unary operators: - ! ++ --.
         */
        ExprPtr ParserCore::parseUnary()
        {
            // (AR) تحليل عامل الاستعارة & / (EN) Parse borrow operator &
            if (match(TT::OP_BITWISE_AND))
            {
                auto pos = previous().getPosition();
                // (AR) تحليل تعليق العمر الاختياري: &'أ متغير / (EN) Parse optional lifetime: &'أ variable
                std::string lifetimeAnnotation;
                if (check(TT::LIFETIME))
                {
                    lifetimeAnnotation = current_.getValue();
                    advance(); // (AR) استهلك LIFETIME / consume LIFETIME
                }
                // (AR) التحقق من &متغير / &mut (استعارة قابلة للتعديل)
                // (EN) Check for &متغير / &mut (mutable borrow)
                bool isMut = false;
                if (check(TT::KEYWORD_VAR) ||
                    (check(TT::IDENTIFIER) && peek().getValue() == "mut"))
                {
                    isMut = true;
                    advance();
                }
                // (AR) اسم المتغير المُستعار / (EN) Variable name to borrow
                if (!check(TT::IDENTIFIER))
                {
                    error("\xD9\x85\xD8\xAA\xD9\x88\xD9\x82\xD8\xB9 \xD8\xA7\xD8\xB3\xD9\x85 \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA8\xD8\xB9\xD8\xAF & / Expected variable name after &");
                    return std::make_unique<LiteralExpr>(Token(TT::LITERAL_NULL, "null", pos));
                }
                std::string varName = peek().getValue();
                advance();
                return std::make_unique<BorrowExpr>(varName, isMut, pos, lifetimeAnnotation);
            }

            if (matchAny({TT::OP_NOT, TT::OP_MINUS, TT::OP_INCREMENT, TT::OP_DECREMENT, TT::OP_BITWISE_NOT}))
            {
                Token op = previous();
                auto right = parseUnary();
                return std::make_unique<UnaryExpr>(
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return parsePower();
        }

        /**
         * @brief (AR) يحلل عامل الأس: **.
         *        (EN) Parses power operator: **.
         */
        ExprPtr ParserCore::parsePower()
        {
            auto expr = parsePostfix();

            if (match(TT::OP_POWER))
            {
                Token op = previous();
                auto right = parsePower(); // Right associative
                expr = std::make_unique<BinaryExpr>(
                    std::move(expr),
                    op.getType(),
                    std::move(right),
                    op.getPosition());
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل استدعاءات الدوال والوصول: f() obj.member arr[i].
         *        (EN) Parses function calls and access: f() obj.member arr[i].
         */
        ExprPtr ParserCore::parsePostfix()
        {
            auto expr = parsePrimary();

            while (true)
            {
                // ═══════════════════════════════════════════════════════════════
                // (AR) دعم استدعاء الماكرو: اسم!(معاملات)
                //      إذا وجدنا '!' بعد مُعرّف وقبل '('، نعتبره استدعاء ماكرو
                // (EN) Macro call support: name!(args)
                //      If we see '!' after identifier and before '(', treat as macro call
                // ═══════════════════════════════════════════════════════════════
                if (check(TT::OP_NOT) && peekNext().getType() == TT::PAREN_LEFT)
                {
                    // (AR) تحقق أن التعبير السابق هو مُعرّف (اسم الماكرو)
                    // (EN) Verify previous expr is an identifier (macro name)
                    auto *varExpr = dynamic_cast<VariableExpr *>(expr.get());
                    if (varExpr)
                    {
                        advance();             // (AR) استهلاك '!' / (EN) consume '!'
                        match(TT::PAREN_LEFT); // (AR) استهلاك '(' / (EN) consume '('
                        auto args = parseArgumentList();
                        consume(TT::PAREN_RIGHT,
                                "(AR) توقع ')' بعد وسائط الماكرو. (EN) Expected ')' after macro arguments.");
                        expr = std::make_unique<CallExpr>(
                            std::move(expr),
                            std::move(args),
                            previous().getPosition(),
                            true // isMacroCall = true
                        );
                        continue;
                    }
                }

                if (match(TT::PAREN_LEFT))
                {
                    // Function call
                    // (AR) استدعاء دالة
                    auto args = parseArgumentList();
                    consume(TT::PAREN_RIGHT,
                            "(AR) توقع ')' بعد الوسائط. (EN) Expected ')' after arguments.");

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) دعم postfix جديد: اسم_صنف(معاملات) جديد
                    //      الصفة بعد الموصوف — قاعدة اللغة العربية
                    //      إذا تلا ')' كلمة 'جديد'، نحوّل CallExpr إلى NewExpr
                    //      'جديد' اختيارية — بدونها يُعامل كـ CallExpr عادي
                    //      (المفسر والمترجم يكتشفان الأصناف تلقائياً)
                    // (EN) Postfix 'new' support: ClassName(args) جديد
                    //      Adjective after noun — Arabic grammar rule
                    //      If ')' is followed by 'جديد', convert CallExpr to NewExpr
                    //      'جديد' is optional — without it, treated as regular CallExpr
                    //      (interpreter and compiler auto-detect class names)
                    // ═══════════════════════════════════════════════════════════════
                    if (check(TT::KEYWORD_NEW))
                    {
                        advance(); // (AR) استهلاك 'جديد' / (EN) consume 'جديد'
                        // (AR) استخراج اسم الصنف من تعبير الدالة
                        // (EN) Extract class name from callee expression
                        std::string className;
                        if (auto *varExpr = dynamic_cast<VariableExpr *>(expr.get()))
                        {
                            className = varExpr->name;
                        }
                        else
                        {
                            error("(AR) توقع اسم صنف قبل '(معاملات) جديد'. "
                                  "(EN) Expected class name before '(args) جديد'.");
                            className = "?";
                        }
                        auto newExpr = std::make_unique<NewExpr>(className);
                        newExpr->arguments = std::move(args);
                        expr = std::move(newExpr);
                    }
                    else
                    {
                        expr = std::make_unique<CallExpr>(
                            std::move(expr),
                            std::move(args),
                            previous().getPosition());
                    }
                }
                else if (check(TT::BRACE_LEFT))
                {
                    // ═══════════════════════════════════════════════════════════════
                    // (AR) دعم خريطة{...} — خريطة حرفية مكتوبة بنوع
                    //      مثال: خريطة{"مفتاح": "قيمة"} أو خريطة{}
                    //      فقط إذا كان التعبير السابق هو VariableExpr بقيمة نوع
                    // (EN) Support map{...} — typed map literal
                    //      Example: map{"key": "value"} or map{}
                    //      Only if previous expression is a VariableExpr from a type token
                    // ═══════════════════════════════════════════════════════════════
                    if (auto *varExpr = dynamic_cast<VariableExpr *>(expr.get()))
                    {
                        std::string name = varExpr->name;
                        if (name == "خريطة" || name == "مصفوفة" || name == "map")
                        {
                            advance(); // consume '{'
                            auto mapResult = parseMapLiteral();
                            if (mapResult)
                            {
                                expr = std::move(mapResult);
                            }
                            continue;
                        }
                    }
                    break; // Not a typed literal, exit postfix loop
                }
                else if (match(TT::DOT))
                {
                    // Member access or method call
                    // (AR) الوصول لعضو أو استدعاء طريقة
                    // (AR) دعم الكلمات المفتاحية الناعمة كأسماء أعضاء (مثل: كائن.احصل())
                    // (EN) Support soft keywords as member names (e.g., obj.احصل())
                    Token member(TT::IDENTIFIER, "", Lexer::Position());
                    if (check(TT::IDENTIFIER))
                    {
                        member = current_;
                        advance();
                    }
                    else if (check(TT::NUMBER_INTEGER))
                    {
                        // (AR) دعم الوصول الرقمي للصفوف: صف.0، صف.1، صف.2
                        // (EN) Support numeric tuple access: tuple.0, tuple.1, tuple.2
                        member = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                    }
                    else if (check(TT::NUMBER_DOUBLE))
                    {
                        // (AR) معالجة الوصول المتسلسل للصفوف المتداخلة: صف.0.0، صف.1.2
                        // عندما يقرأ المحلل المعجمي "0.0" كرقم عشري واحد، نقسمه إلى وصولين عضويين
                        // مثال: ص.0.0 → ص DOT "0.0"(NUMBER_DOUBLE) → نحولها إلى ص.0 ثم .0
                        // (EN) Handle chained tuple member access: tuple.0.0, tuple.1.2
                        // When lexer reads "0.0" as a single NUMBER_DOUBLE, we split it into two member accesses
                        std::string numStr = current_.getValue();
                        auto numPos = current_.getPosition();
                        advance();

                        // (AR) نبحث عن النقطة العشرية في النص لنقسمه
                        auto dotPos = numStr.find('.');
                        if (dotPos != std::string::npos)
                        {
                            std::string firstPart = numStr.substr(0, dotPos);
                            std::string secondPart = numStr.substr(dotPos + 1);

                            // (AR) الوصول الأول: expr.firstPart
                            member = Token(TT::IDENTIFIER, firstPart, numPos);

                            // (AR) ننشئ عقدة MemberExpr الأولى هنا ونستمر بوصولات إضافية
                            expr = std::make_unique<MemberExpr>(
                                std::move(expr),
                                member.getValue(),
                                member.getPosition());

                            // (AR) الوصول الثاني: .secondPart (مجرد member access عادي)
                            expr = std::make_unique<MemberExpr>(
                                std::move(expr),
                                secondPart,
                                numPos);

                            // (AR) ننتقل مباشرة للحلقة التالية بدون الدخول للتحقق أدناه
                            continue;
                        }
                        else
                        {
                            // (AR) رقم عشري بدون نقطة (لا ينبغي أن يحدث)
                            member = Token(TT::IDENTIFIER, numStr, numPos);
                        }
                    }
                    else if (isTokenUsableAsName(current_.getType()))
                    {
                        member = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                    }
                    else
                    {
                        member = consume(TT::IDENTIFIER,
                                         "(AR) توقع اسم عضو بعد '.'. (EN) Expected member name after '.'.");
                    }

                    // Check if this is a method call: obj.method()
                    if (check(TT::PAREN_LEFT))
                    {
                        match(TT::PAREN_LEFT);
                        auto args = parseArgumentList();
                        consume(TT::PAREN_RIGHT,
                                "(AR) توقع ')' بعد وسائط الطريقة. (EN) Expected ')' after method arguments.");
                        expr = std::make_unique<MethodCallExpr>(
                            std::move(expr),
                            member.getValue());
                        // Add arguments
                        auto *methodCall = static_cast<MethodCallExpr *>(expr.get());
                        methodCall->arguments = std::move(args);
                    }
                    else
                    {
                        // Regular member access: obj.field
                        expr = std::make_unique<MemberExpr>(
                            std::move(expr),
                            member.getValue(),
                            member.getPosition());
                    }
                }
                else if (match(TT::QUESTION_DOT))
                {
                    // (AR) وصول آمن: كائن?.عضو / (EN) Optional chaining: obj?.member
                    Token member(TT::IDENTIFIER, "", Lexer::Position());
                    if (check(TT::IDENTIFIER))
                    {
                        member = current_;
                        advance();
                    }
                    else if (isTokenUsableAsName(current_.getType()))
                    {
                        member = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                    }
                    else
                    {
                        member = consume(TT::IDENTIFIER,
                                         "(AR) توقع اسم عضو بعد '?.'. (EN) Expected member name after '?.'.");
                    }

                    expr = std::make_unique<OptionalChainExpr>(
                        std::move(expr),
                        member.getValue(),
                        member.getPosition());
                }
                else if (match(TT::BRACKET_LEFT))
                {
                    // (AR) الوصول بالفهرس أو الشريحة
                    // (EN) Index access or slice
                    // (AR) صيغ الشريحة: [start:end], [:end], [start:], [start:end:step]
                    // (EN) Slice forms: [start:end], [:end], [start:], [start:end:step]
                    auto pos = previous().getPosition();

                    // (AR) فحص شريحة بدون بداية: [:end]
                    // (EN) Check for slice with no start: [:end]
                    if (check(TT::COLON))
                    {
                        advance(); // consume ':'
                        ExprPtr sliceEnd = nullptr;
                        if (!check(TT::BRACKET_RIGHT))
                        {
                            sliceEnd = parseExpression();
                        }
                        ExprPtr sliceStep = nullptr;
                        if (match(TT::COLON))
                        {
                            sliceStep = parseExpression();
                        }
                        consume(TT::BRACKET_RIGHT,
                                "(AR) توقع ']' بعد الشريحة. (EN) Expected ']' after slice.");
                        expr = std::make_unique<SliceExpr>(
                            std::move(expr), nullptr, std::move(sliceEnd), std::move(sliceStep), pos);
                    }
                    else
                    {
                        // (AR) تحليل التعبير الأول (قد يكون فهرس أو بداية شريحة)
                        // (EN) Parse first expression (could be index or slice start)
                        auto first = parseExpression();

                        if (match(TT::COLON))
                        {
                            // (AR) هذه شريحة: [start:end] أو [start:] أو [start:end:step]
                            // (EN) This is a slice: [start:end] or [start:] or [start:end:step]
                            ExprPtr sliceEnd = nullptr;
                            if (!check(TT::BRACKET_RIGHT) && !check(TT::COLON))
                            {
                                sliceEnd = parseExpression();
                            }
                            ExprPtr sliceStep = nullptr;
                            if (match(TT::COLON))
                            {
                                sliceStep = parseExpression();
                            }
                            consume(TT::BRACKET_RIGHT,
                                    "(AR) توقع ']' بعد الشريحة. (EN) Expected ']' after slice.");
                            expr = std::make_unique<SliceExpr>(
                                std::move(expr), std::move(first), std::move(sliceEnd), std::move(sliceStep), pos);
                        }
                        else if (check(TT::DOT_DOT))
                        {
                            // (AR) شريحة بصيغة النطاق: [1..5] أو [1..]
                            // (EN) Range-based slice: [1..5] or [1..]
                            advance(); // consume '..'
                            ExprPtr sliceEnd = nullptr;
                            if (!check(TT::BRACKET_RIGHT))
                            {
                                sliceEnd = parseExpression();
                            }
                            consume(TT::BRACKET_RIGHT,
                                    "(AR) توقع ']' بعد الشريحة. (EN) Expected ']' after slice.");
                            expr = std::make_unique<SliceExpr>(
                                std::move(expr), std::move(first), std::move(sliceEnd), nullptr, pos);
                        }
                        else
                        {
                            // (AR) تحقق إذا كان التعبير RangeExpr (مثل: arr[1..5] أو arr[1..])
                            // (EN) Check if expression is RangeExpr (e.g., arr[1..5] or arr[1..])
                            // (AR) عندما يستهلك parseExpression النطاق ".." يُرجع RangeExpr
                            // (EN) When parseExpression consumes ".." it returns RangeExpr
                            auto *rangeExpr = dynamic_cast<RangeExpr *>(first.get());
                            if (rangeExpr)
                            {
                                // (AR) تحويل RangeExpr إلى SliceExpr
                                // (EN) Convert RangeExpr to SliceExpr
                                ExprPtr sliceStart = std::move(rangeExpr->start);
                                ExprPtr sliceEnd = std::move(rangeExpr->end);
                                consume(TT::BRACKET_RIGHT,
                                        "(AR) توقع ']' بعد الشريحة. (EN) Expected ']' after slice.");
                                expr = std::make_unique<SliceExpr>(
                                    std::move(expr), std::move(sliceStart), std::move(sliceEnd), nullptr, pos);
                            }
                            else
                            {
                                // (AR) فهرس عادي: [index]
                                // (EN) Regular index: [index]
                                consume(TT::BRACKET_RIGHT,
                                        "(AR) توقع ']' بعد الفهرس. (EN) Expected ']' after index.");
                                expr = std::make_unique<IndexExpr>(
                                    std::move(expr),
                                    std::move(first),
                                    pos);
                            }
                        }
                    }
                }
                // (AR) عامل بعدي «x++»/«x--» — يُربَط فقط إن كان على سطر معاملِه نفسِه. اللغة غير
                //      حسّاسة للأسطر عمومًا، لكن التقاط «++» في بداية سطرٍ تالٍ كلاحقٍ للجملة السابقة
                //      كان يُفسِد بادئ «++س» ويُحدِث تباعدًا بين المحرّكين (ISSUE-043). فحص السطر يفضّ الالتباس.
                // (EN) Postfix «x++»/«x--» — bind only if on the same line as its operand. The language is
                //      newline-insensitive generally, but grabbing a «++» at the start of the next line as a
                //      postfix of the previous statement broke prefix «++س» and diverged the engines
                //      (ISSUE-043). The line check disambiguates.
                else if ((check(TT::OP_INCREMENT) || check(TT::OP_DECREMENT)) &&
                         peek().getPosition().line == previous().getPosition().line)
                {
                    Token op = peek();
                    advance();
                    expr = std::make_unique<UnaryExpr>(
                        op.getType(),
                        std::move(expr),
                        op.getPosition());
                }
                // ─── عامل تأكيد عدم الفراغ: قيمة مؤكَّد / Null assertion: value مؤكَّد (NS-05) ───
                // (AR) لاحقيّ سياقيّ (كلمة): T؟ → T؛ خطأ وقت تشغيل إذا كانت القيمة عدمًا.
                //      سياقيّ كي تبقى «مؤكد»/«مؤكدة» صالحة كأسماء/أعضاء تعداد؛ تُميَّز هنا
                //      فقط حين تَلي تعبيرًا لاحقيًّا (الوصول لعضو «.مؤكد» يُعالَج في فرع DOT أعلاه).
                // (EN) Postfix contextual word: T? → T; runtime error if null. Contextual so
                //      «مؤكد»/«مؤكدة» remain valid as identifiers/enum members; recognized here
                //      only when following a postfix expression (member «.مؤكد» handled in DOT above).
                else if (checkContextual(TT::OP_NULL_ASSERT))
                {
                    Token op = current_;
                    advance(); // (AR) استهلاك «مؤكد»/«مؤكدة» / (EN) consume the word
                    expr = std::make_unique<UnaryExpr>(
                        TT::OP_NULL_ASSERT,
                        std::move(expr),
                        op.getPosition());
                }
                // ─── Optional index access: obj?[key] ───
                // (AR) وصول آمن بالفهرس: كائن?[مفتاح] — يرجع لاشيء إذا كان الكائن null
                // (EN) Optional index: obj?[key] — returns null if object is null
                else if (check(TT::QUESTION) && peekNext().getType() == TT::BRACKET_LEFT)
                {
                    advance(); // consume '?'
                    advance(); // consume '['
                    auto pos = previous().getPosition();
                    auto indexExpr = parseExpression();
                    consume(TT::BRACKET_RIGHT,
                            "(AR) توقع ']' بعد الفهرس. (EN) Expected ']' after index.");
                    // (AR) نحوّله إلى: (كائن != لاشيء) ? كائن[فهرس] : لاشيء
                    // (EN) Convert to: (obj != null) ? obj[index] : null
                    // (AR) للبساطة نستخدم IndexExpr عادي — المفسر يتعامل مع null
                    expr = std::make_unique<IndexExpr>(
                        std::move(expr),
                        std::move(indexExpr),
                        pos);
                }
                else
                {
                    break;
                }
            }

            return expr;
        }

        /**
         * @brief (AR) يحلل التعبيرات الأساسية: أرقام، نصوص، متغيرات.
         *        (EN) Parses primary expressions: numbers, strings, variables.
         */
        ExprPtr ParserCore::parsePrimary()
        {
            // ═══════════════════════════════════════════════════════════════════
            // (AR) تعبير إذا كتعبير (inline if-expression / ternary)
            //      الصيغة: إذا (شرط) قيمة_صح وإلا قيمة_خطأ
            //      أو:    إذا (شرط) ثم قيمة_صح وإلا قيمة_خطأ
            // (EN) If-expression (inline if / ternary):
            //      syntax: if (cond) true_val else false_val
            //      or:     if (cond) then true_val else false_val
            // ═══════════════════════════════════════════════════════════════════
            if (check(TT::KEYWORD_IF))
            {
                auto ifPos = current_.getPosition();
                advance(); // consume 'إذا'

                // (AR) تحليل الشرط (مع أو بدون أقواس)
                ExprPtr condition;
                if (match(TT::PAREN_LEFT))
                {
                    condition = parseExpression();
                    consume(TT::PAREN_RIGHT, "(AR) توقع ')' بعد شرط إذا. (EN) Expected ')' after if condition.");
                }
                else
                {
                    condition = parseLogicalOr(); // parse condition without parens
                }

                // (AR) تخطي 'ثم' الاختياري
                if (matchContextual(TT::KEYWORD_THEN))
                {
                    // consumed optional 'ثم'
                }

                // (AR) القيمة الصحيحة
                auto trueExpr = parseLogicalOr();

                // (AR) توقع 'وإلا'
                if (!match(TT::KEYWORD_ELSE))
                {
                    errorBilingual(
                        "خطأ: توقع 'وإلا' في تعبير إذا.",
                        "Error: expected 'else' in if-expression.");
                    return nullptr;
                }

                // (AR) القيمة الخاطئة
                auto falseExpr = parseLogicalOr();

                return std::make_unique<TernaryExpr>(
                    std::move(condition), std::move(trueExpr), std::move(falseExpr), ifPos);
            }

            // Await expression: await expr (contextual keyword)
            // (AR) تعبير الانتظار (كلمة سياقية — لم تعد محجوزة)
            if (match(TT::KEYWORD_AWAIT) || matchContextual(TT::KEYWORD_AWAIT))
            {
                Token awaitToken = previous();
                auto expr = parseTernary(); // (AR) تحليل تعبير ذو أولوية أعلى / (EN) Parse higher precedence expression
                if (!expr)
                {
                    errorBilingual(
                        "خطأ نحوي: يجب أن يتبع 'انتظر' بتعبير.",
                        "Syntax error: 'await' must be followed by an expression.");
                    return nullptr;
                }
                return std::make_unique<AwaitExpr>(std::move(expr), awaitToken.getPosition());
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) تعبير نشر الخطأ: انشر تعبير (كلمة سياقية)
            //      إذا كانت النتيجة خطأ/عدم → يُرجع القيمة فوراً من الدالة الحالية
            //      إذا كانت النتيجة نجاح/بعض → يُستخرج الحمولة كقيمة التعبير
            // (EN) Error propagation expression: propagate expr (contextual keyword)
            //      If result is error/none → early return from current function
            //      If result is success/some → unwrap payload as expression value
            // ═══════════════════════════════════════════════════════════════════
            if (checkContextual(TT::KEYWORD_PROPAGATE))
            {
                Token propagateToken = current_;
                advance();                  // (AR) استهلاك 'انشر' / (EN) consume 'انشر'
                auto expr = parseTernary(); // (AR) تحليل التعبير الداخلي / (EN) Parse inner expression
                if (!expr)
                {
                    errorBilingual(
                        "خطأ نحوي: يجب أن يتبع 'انشر' بتعبير يُرجع نتيجة أو اختياري.",
                        "Syntax error: 'انشر' (propagate) must be followed by an expression returning Result or Option.");
                    return nullptr;
                }
                return std::make_unique<ErrorPropagateExpr>(std::move(expr), propagateToken.getPosition());
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) 'جديد' لم تعد تُستخدم كبادئة (prefix) لإنشاء الكائنات
            //      القاعدة الجديدة: اسم_صنف(معاملات) جديد (postfix — الصفة بعد الموصوف)
            //      أو: اسم_صنف(معاملات) بدون جديد (اختيارية)
            //      إذا ظهرت 'جديد' في بداية تعبير → تُعامل كمعرّف عادي
            // (EN) 'جديد' is no longer used as prefix for object creation
            //      New rule: ClassName(args) جديد (postfix — adjective after noun in Arabic)
            //      Or: ClassName(args) without جديد (optional)
            //      If 'جديد' appears at start of expression → treated as regular identifier
            // ═══════════════════════════════════════════════════════════════════
            if (match(TT::KEYWORD_NEW))
            {
                return std::make_unique<VariableExpr>(previous().getValue(), previous().getPosition());
            }

            // OOP: this expression for current object
            // (AR) تعبير هذا للكائن الحالي
            if (match(TT::KEYWORD_THIS))
            {
                return std::make_unique<ThisExpr>();
            }

            // OOP: super expression for base class
            // (AR) تعبير الأساس للصنف الأب — فقط إذا تبعه '.' (وصول عضو)
            //      وإلا يُعامل كمعرّف عادي (مثلاً: معامل دالة اسمه "أساس")
            // (EN) Only treat as super-expression if followed by '.' (member access)
            //      Otherwise treat as regular identifier (e.g., parameter named "أساس")
            if (check(TT::KEYWORD_SUPER) && peekNext().getType() == TT::DOT)
            {
                advance(); // consume KEYWORD_SUPER
                return std::make_unique<SuperExpr>();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) تعبيرات التوجيهات @: @حجم(نوع)، @ذري(عملية, ...)، @تجميع("code")
            // (EN) @ directive expressions: @حجم(type), @ذري(op, ...), @تجميع("code")
            // ═══════════════════════════════════════════════════════════════════
            if (check(TT::AT_SIGN))
            {
                auto dirExpr = parseDirectiveExpr();
                if (dirExpr)
                    return dirExpr;
            }

            // Lambda expression (contextual keyword)
            // (AR) تعبير لامدا (كلمة سياقية — لم تعد محجوزة)
            if (match(TT::KEYWORD_LAMBDA) || matchContextual(TT::KEYWORD_LAMBDA))
            {
                return parseLambda();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) دعم الدالة المجهولة والسهمية تمت إزالته.
            //      استخدم لامدا(params) => body بدلاً من ذلك.
            // (EN) Anonymous function and arrow function support removed.
            //      Use lambda(params) => body instead.
            // ═══════════════════════════════════════════════════════════════════

            // Literals
            // (AR) القيم الحرفية

            if (match(TT::LITERAL_TRUE))
            {
                return std::make_unique<LiteralExpr>(previous());
            }

            if (match(TT::LITERAL_FALSE))
            {
                return std::make_unique<LiteralExpr>(previous());
            }

            if (match(TT::LITERAL_NULL))
            {
                return std::make_unique<LiteralExpr>(previous());
            }

            if (match(TT::NUMBER_INTEGER) || match(TT::NUMBER_DOUBLE))
            {
                return std::make_unique<LiteralExpr>(previous());
            }

            // STRING_LITERAL: Regular strings / النصوص العادية
            // (AR) معالجة النص الحرفي العادي
            if (match(TT::STRING_LITERAL))
            {
                return std::make_unique<LiteralExpr>(previous());
            }

            // STRING_RAW: Raw strings r"..." treat as regular string
            // (AR) النصوص الخام r"..." تُعامل كنص عادي (لا معالجة للـ escape)
            // Lexer already processed escapes (or didn't), just use as-is
            // المحلل المعجمي بالفعل عالج (أو لم يعالج) الـ escapes، استخدمها كما هي
            if (match(TT::STRING_RAW))
            {
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
            if (match(TT::STRING_FSTRING))
            {
                auto fstringToken = previous();
                std::string fstring = fstringToken.getValue();

                // (AR) تحليل f-string — يدعم التعبيرات المدمجة {تعبير}
                // (EN) F-string parsing — supports embedded expressions {expr}

                // Check if f-string contains expressions {...}
                // تحقق إذا كان f-string يحتوي على تعبيرات {...}
                if (fstring.find('{') != std::string::npos)
                {
                    // Parse and expand f-string expressions
                    // تحليل وتوسيع تعبيرات f-string
                    return parseFStringExpr(fstring, fstringToken.getPosition());
                }
                else
                {
                    // No expressions, treat as regular string
                    // لا يوجد تعبيرات، عامله كنص عادي
                    Lexer::Token stringToken(TT::STRING_LITERAL, fstring, fstringToken.getPosition());
                    return std::make_unique<LiteralExpr>(stringToken);
                }
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) القيمة المطلقة: |تعبير| → abs(تعبير)
            // (EN) Absolute value: |expr| → abs(expr)
            // ═══════════════════════════════════════════════════════════════════
            if (match(TT::OP_BITWISE_OR))
            {
                auto absPos = previous().getPosition();
                // (AR) نستخدم parseBitwiseXor بدلاً من parseExpression لتجنب استهلاك '|' كعامل OR ثنائي
                // (EN) Use parseBitwiseXor (one level below bitwiseOr) to avoid consuming closing '|' as binary OR
                auto innerExpr = parseBitwiseXor();
                if (!match(TT::OP_BITWISE_OR))
                {
                    errorBilingual(
                        "خطأ: توقع '|' لإغلاق تعبير القيمة المطلقة.",
                        "Error: expected closing '|' for absolute value expression.");
                }
                // (AR) تحويل |x| إلى استدعاء «مطلق(x)» — الدالة المضمَّنة العربيّة العاملة في
                //      المحرّكين (ISSUE-039). الاسم اللاتينيّ «abs» غير مُسجَّل في المفسّر ومكسور
                //      في المترجم (يُرجع 0)، بينما «مطلق» يُرجع القيمة الصحيحة في الاثنين.
                // (EN) Lower |x| to a call to «مطلق(x)» — the working Arabic builtin in both engines
                //      (ISSUE-039). The Latin name «abs» is unregistered in the interpreter and broken
                //      in the compiler (returns 0), whereas «مطلق» returns the correct value in both.
                std::vector<ExprPtr> args;
                args.push_back(std::move(innerExpr));
                return std::make_unique<CallExpr>(
                    std::make_unique<VariableExpr>("\xD9\x85\xD8\xB7\xD9\x84\xD9\x82", absPos),
                    std::move(args));
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) تعبير واجهة تصريحية: اعرض عنصر_واجهة(...).معدّل
            //      يُستخدم داخل ارجع أو إسناد: ارجع اعرض عمود ... نهاية
            //      أو: متغير ع = اعرض نص("مرحبا").حجم(32)
            //      'اعرض' كلمة سياقية — تُعامل كتعبير واجهة فقط إذا تلاها
            //      اسم عنصر واجهة مسجل. وإلا تُعامل كمُعرّف عادي.
            //
            // (EN) Declarative UI expression: اعرض widget(...).modifier
            //      Used inside return or assignment: return اعرض column ... end
            //      'اعرض' is contextual — treated as UI expr only if followed
            //      by a known widget name. Otherwise treated as regular identifier.
            // ═══════════════════════════════════════════════════════════════════
            if (checkContextual(TT::KEYWORD_SHOW) &&
                peekNext().getType() == TT::IDENTIFIER && isKnownWidget(peekNext().getValue()))
            {
                advance(); // (AR) استهلاك 'اعرض' / (EN) consume 'اعرض'
                return parseWidgetExpression();
            }

            // Variable reference or Template Instantiation
            // (AR) مرجع متغير أو تنفيذ قالب
            if (match(TT::IDENTIFIER))
            {
                std::string identName = previous().getValue();
                auto identPos = previous().getPosition();

                // (AR) تحقق إذا كان تنفيذ قالب: اسم<نوع> أو اسم<قيمة>
                // (EN) Check if template instantiation: name<type> or name<const-value>
                // ══════════════════════════════════════════════════════════════════
                // (AR) الحل الجذري (BF-09): نُميّز تخصيص القالب عن عامل المقارنة `<`
                //      عبر فحص مزدوج:
                //        (1) اسم المعرف مُسجَّل في knownTemplateNames_ (أي عُرِّف كقالب)، أو
                //        (2) الرمز بعد `<` نوعٌ مدمج صراحةً (هذا لم يكن متعارضاً سابقاً).
                //      هذا يمنع تفسير `عداد < 5` كتخصيص قالب ويسمح بـ `مكعب<4>` عند
                //      كون `مكعب` قالباً معروفاً.
                // (EN) Root-cause fix: Disambiguate template instantiation from `<`
                //      comparison operator via two-pronged check:
                //        (1) identifier is in knownTemplateNames_ (declared as a template), OR
                //        (2) the token after `<` is an explicit built-in type keyword.
                //      Prevents `counter < 5` from being parsed as template and still
                //      supports `Cube<4>` when `Cube` is a known template.
                // ══════════════════════════════════════════════════════════════════
                bool isKnownTemplate = knownTemplateNames_.count(identName) > 0;
                if (check(TT::OP_LESS) &&
                    (isTypeToken(nextToken_.getType()) || isKnownTemplate))
                {
                    // (AR) حاول تحليل كتنفيذ قالب
                    // (EN) Try to parse as template instantiation
                    auto templateExpr = parseTemplateInstantiation(identName, identPos);
                    if (templateExpr)
                    {
                        return templateExpr;
                    }
                    // (AR) إذا فشل، اعتبره متغير عادي
                    // (EN) If failed, treat as regular variable
                }

                return std::make_unique<VariableExpr>(
                    identName,
                    identPos);
            }

            // Parenthesized expression or multi-param arrow function
            // (AR) تعبير بين أقواس أو دالة سهمية متعددة المعاملات
            if (match(TT::PAREN_LEFT))
            {
                auto leftPos = previous().getPosition();

                // ═══════════════════════════════════════════════════════════════
                // (AR) التحقق من صف فارغ: ()
                // (EN) Check for empty tuple: ()
                // ═══════════════════════════════════════════════════════════════
                if (check(TT::PAREN_RIGHT))
                {
                    advance(); // consume ')'
                    // (AR) صف فارغ () / (EN) Empty tuple ()
                    std::vector<ExprPtr> elements;
                    ExprPtr tupleNode;
                    tupleNode.reset(new TupleExpr(std::move(elements), leftPos));
                    return tupleNode;
                }

                auto expr = parseExpression();

                // Check if expression parsed successfully
                // (AR) تحقق إذا تم تحليل التعبير بنجاح
                if (!expr)
                {
                    errorBilingual(
                        "تعبير غير صحيح بين الأقواس. تأكد من أن التعبير يحتوي على قيم أو عمليات صحيحة.",
                        "Invalid expression in parentheses. Make sure the expression contains valid values or operations.");
                    return nullptr;
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) التحقق من صف: (تعبير، تعبير، ...) أو (تعبير،)
                //      إذا وجدنا فاصلة بعد التعبير الأول فهو صف وليس تجميع
                //      (تعبير) → تجميع عادي
                //      (تعبير،) → صف بعنصر واحد
                //      (تعبير، تعبير) → صف بعنصرين
                // (EN) Check for tuple: (expr, expr, ...) or (expr,)
                //      A comma after first expression means it's a tuple
                // ═══════════════════════════════════════════════════════════════
                if (check(TT::COMMA) || check(TT::ARABIC_COMMA))
                {
                    std::vector<ExprPtr> elements;
                    elements.push_back(std::move(expr));

                    while (check(TT::COMMA) || check(TT::ARABIC_COMMA))
                    {
                        advance(); // consume ','/'،'
                        // (AR) فاصلة أخيرة قبل ')' — صف بعنصر واحد أو فاصلة زائدة
                        // (EN) Trailing comma before ')' — single-element tuple or trailing comma
                        if (check(TT::PAREN_RIGHT))
                            break;
                        auto elem = parseExpression();
                        if (!elem)
                        {
                            errorBilingual(
                                "خطأ نحوي: توقعت تعبيراً بعد الفاصلة في الصف.",
                                "Syntax error: expected expression after comma in tuple.");
                            return nullptr;
                        }
                        elements.push_back(std::move(elem));
                    }

                    consume(TT::PAREN_RIGHT,
                            "(AR) توقع ')' لإغلاق الصف. (EN) Expected ')' to close tuple.");

                    ExprPtr tupleMulti;
                    tupleMulti.reset(new TupleExpr(std::move(elements), leftPos));
                    return tupleMulti;
                }

                consume(TT::PAREN_RIGHT,
                        "(AR) توقع ')' بعد التعبير. تأكد من إغلاق جميع الأقواس بشكل صحيح. (EN) Expected ')' after expression. Make sure all parentheses are properly closed.");

                // (AR) التحقق من (x) => جسم — دالة سهمية بمعامل واحد بين أقواس
                // (EN) Check for (x) => body — single-param arrow function with parens
                if (check(TT::FAT_ARROW))
                {
                    advance(); // consume =>
                    if (auto *varExpr = dynamic_cast<VariableExpr *>(expr.get()))
                    {
                        std::vector<Parameter> params;
                        params.emplace_back(varExpr->name, Types::SadTypeKind::Unknown, nullptr);
                        auto body = parseExpression();
                        return std::make_unique<LambdaExpr>(std::move(params), std::move(body), leftPos);
                    }
                    else
                    {
                        errorBilingual(
                            "خطأ: معامل الدالة السهمية يجب أن يكون اسم متغير.",
                            "Error: arrow function parameter must be an identifier.");
                        return nullptr;
                    }
                }

                return expr;
            }

            // Array literal
            // (AR) مصفوفة حرفية
            if (match(TT::BRACKET_LEFT))
            {
                auto result = parseArrayLiteral();
                if (!result)
                {
                    errorBilingual(
                        "خطأ في تحليل مصفوفة حرفية. تأكد من صيغة المصفوفة: [عنصر1، عنصر2، ...] أو [x for x in list]",
                        "Error parsing array literal. Make sure array syntax is correct: [elem1, elem2, ...] or [x for x in list]");
                    return nullptr;
                }
                return result;
            }

            // Map literal
            // (AR) خريطة حرفية
            if (match(TT::BRACE_LEFT))
            {
                auto result = parseMapLiteral();
                if (!result)
                {
                    errorBilingual(
                        "خطأ في تحليل خريطة حرفية. تأكد من صيغة الخريطة: {مفتاح: قيمة، ...}",
                        "Error parsing map literal. Make sure map syntax is correct: {key: value, ...}");
                    return nullptr;
                }
                return result;
            }

            // Type keywords used as expressions (e.g., نص(...) as function call)
            // (AR) كلمات أنواع مستخدمة كتعبيرات (مثلاً نص(...) كاستدعاء دالة)
            // This allows type names to be used as function names in expression position
            if (isTypeToken(current_.getType()))
            {
                auto tok = current_;
                advance();
                return std::make_unique<VariableExpr>(tok.getValue(), tok.getPosition());
            }

            // (AR) كلمات مفتاحية ناعمة مستخدمة كتعبيرات (مثلاً احصل(...)، نوع، حجم، من، استورد)
            // (EN) Soft keywords used as expressions (e.g., احصل(...), نوع, حجم, من, استورد)
            // This allows keywords like احصل (KEYWORD_GET), نوع (KEYWORD_TYPENAME),
            // من (KEYWORD_FROM), استورد (KEYWORD_IMPORT) etc. to be used as identifiers in expression position
            if (isTokenUsableAsName(current_.getType()))
            {
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
            switch (current.getType())
            {
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
            if (!isAtEnd())
            {
                advance();
            }

            return nullptr;
        }

        /**
         * @brief (AR) يحلل دالة لامدا بالصيغ المدعومة:
         *        1. لامدا(م) => تعبير                      (سهم مع تعبير)
         *        2. لامدا(م) => كتلة_جمل نهاية             (سهم مع كتلة)
         *        3. لامدا(م) جسم... نهاية                   (بلا فاصل — كتلة)
         *        (EN) Parses lambda in supported forms:
         *        1. lambda(m) => expression                  (arrow with expression)
         *        2. lambda(m) => statement_block end          (arrow with block)
         *        3. lambda(m) body... end                     (no separator — block)
         *
         *        (AR) ملاحظة: الأقواس حول المعاملات إلزامية دائماً.
         *        (EN) Note: Parentheses around parameters are always required.
         */
        ExprPtr ParserCore::parseLambda()
        {
            auto lambdaPos = previous().getPosition();
            std::vector<std::string> paramNames;

            // (AR) الأقواس حول المعاملات إلزامية: لامدا(params) أو لامدا()
            // (EN) Parenthesized params required: lambda(params) or lambda()
            if (!check(TT::PAREN_LEFT))
            {
                errorBilingual(
                    "خطأ: يجب استخدام أقواس حول معاملات لامدا. مثال: لامدا(x) => x + 1",
                    "Error: Lambda parameters must be in parentheses. Example: lambda(x) => x + 1");
                return nullptr;
            }

            advance(); // consume '('
            paramNames = parseParameterList();
            consume(TT::PAREN_RIGHT,
                    "(AR) توقع ')' بعد معاملات لامدا. (EN) Expected ')' after lambda parameters.");

            // (AR) تحويل أسماء المعاملات إلى كائنات Parameter
            // (EN) Convert param names to Parameter objects
            std::vector<Parameter> params;
            params.reserve(paramNames.size());
            for (const auto &name : paramNames)
            {
                params.emplace_back(name, Types::SadTypeKind::Unknown, nullptr);
            }

            // ─── Arrow form: لامدا(params) => expression [نهاية] ───
            // (AR) أو لامدا(params) => كتلة_جمل نهاية (عندما يبدأ الجسم بجملة)
            // (EN) Or lambda(params) => statement_block end (when body starts with a statement)
            if (match(TT::FAT_ARROW))
            {
                // (AR) فحص إذا كان الجسم كتلة جمل (يبدأ بكلمة مفتاحية لجملة)
                // (EN) Check if body is a statement block (starts with statement keyword)
                bool isBlockBody = check(TT::KEYWORD_IF) || check(TT::KEYWORD_WHILE) ||
                                   check(TT::KEYWORD_FOR) || check(TT::KEYWORD_RETURN) ||
                                   check(TT::KEYWORD_VAR) || check(TT::KEYWORD_CONST) ||
                                   check(TT::KEYWORD_TRY) || check(TT::KEYWORD_THROW) ||
                                   check(TT::KEYWORD_MATCH) || check(TT::KEYWORD_BREAK) ||
                                   check(TT::KEYWORD_CONTINUE);

                if (isBlockBody)
                {
                    // (AR) تحليل ككتلة حتى 'نهاية'
                    // (EN) Parse as block until 'end'
                    auto body = parseBlockStmt();
                    return std::make_unique<LambdaExpr>(
                        std::move(params), std::move(body), lambdaPos);
                }

                auto body = parseExpression();
                if (!body)
                {
                    errorBilingual(
                        "خطأ: فشل تحليل جسم لامدا بعد '=>'.",
                        "Error: failed to parse lambda body after '=>'.");
                    return nullptr;
                }
                // (AR) استهلاك 'نهاية' الاختيارية بعد تعبير السهم فقط إذا كانت
                //      متبوعة بمحدد إغلاق (مثل ')' أو ',' أو ']')
                //      لتجنب استهلاك 'نهاية' الخاصة بالكتلة الأب (دالة/إذا/بينما/لكل)
                // (EN) Only consume optional 'نهاية' after arrow expression if it's
                //      followed by a closing delimiter (')', ',', ']', ';')
                //      to avoid consuming 'نهاية' belonging to an enclosing block
                if (check(TT::KEYWORD_END))
                {
                    auto nextType = peekNext().getType();
                    if (nextType == TT::PAREN_RIGHT ||
                        nextType == TT::COMMA ||
                        nextType == TT::ARABIC_COMMA ||
                        nextType == TT::BRACKET_RIGHT ||
                        nextType == TT::SEMICOLON ||
                        nextType == TT::ARABIC_SEMICOLON)
                    {
                        advance(); // (AR) استهلاك 'نهاية' — تنتمي للامدا وليس للكتلة الأب
                    }
                }
                return std::make_unique<LambdaExpr>(
                    std::move(params), std::move(body), lambdaPos);
            }

            // ─── Block form (no separator): لامدا(params) جسم... نهاية ───
            auto body = parseBlockStmt();
            return std::make_unique<LambdaExpr>(
                std::move(params), std::move(body), lambdaPos);
        }

        /**
         * @brief (AR) يحلل f-string ويحوله إلى تسلسل نصوص.
         *        (EN) Parses f-string and converts to string concatenation.
         *
         * Converts f"Hello {name}!" to: "Hello " + str(name) + "!"
         * يحول f"مرحبا {الاسم}!" إلى: "مرحبا " + str(الاسم) + "!"
         */
        ExprPtr ParserCore::parseFStringExpr(const std::string &fstring, const Lexer::Position &pos)
        {
            // Build concatenation of string parts and expressions
            // بناء تسلسل من أجزاء النصوص والتعبيرات

            ExprPtr result = nullptr;
            size_t i = 0;
            std::string currentText;

            while (i < fstring.length())
            {
                if (fstring[i] == '{' && (i + 1 < fstring.length() && fstring[i + 1] != '{'))
                {
                    // Found expression start
                    // وجدنا بداية تعبير

                    // Add accumulated text as string literal
                    // أضف النص المتراكم كنص حرفي
                    if (!currentText.empty())
                    {
                        auto textLiteral = std::make_unique<LiteralExpr>(
                            Lexer::Token(TT::STRING_LITERAL, currentText, pos));
                        if (result)
                        {
                            // Concatenate with previous parts
                            // تسلسل مع الأجزاء السابقة
                            result = std::make_unique<BinaryExpr>(
                                std::move(result),
                                TT::OP_PLUS,
                                std::move(textLiteral),
                                pos);
                        }
                        else
                        {
                            result = std::move(textLiteral);
                        }
                        currentText.clear();
                    }

                    // Find closing brace
                    // ابحث عن القوس الإغلاق
                    size_t exprStart = i + 1;
                    size_t braceDepth = 1;
                    i++;
                    while (i < fstring.length() && braceDepth > 0)
                    {
                        if (fstring[i] == '{')
                            braceDepth++;
                        else if (fstring[i] == '}')
                            braceDepth--;
                        if (braceDepth > 0)
                            i++;
                    }

                    if (braceDepth != 0)
                    {
                        errorBilingual(
                            "خطأ: قوس غير مُغلق في f-string. تأكد من إغلاق جميع الأقواس {...}",
                            "Error: unclosed brace in f-string. Make sure all braces {...} are closed.");
                        return nullptr;
                    }

                    // Extract expression text
                    // استخراج نص التعبير
                    std::string exprText = fstring.substr(exprStart, i - exprStart);

                    // (AR) تحليل التعبير الكامل من النص باستخدام محلل فرعي
                    // (EN) Parse full expression from text using a sub-parser
                    ExprPtr exprNode = nullptr;
                    try
                    {
                        Lexer::LexerCore exprLexer(exprText);
                        ParserCore exprParser(exprLexer);
                        exprNode = exprParser.parseExpression();
                    }
                    catch (...)
                    {
                        // (AR) في حالة فشل تحليل التعبير، استخدم كاسم متغير
                        // (EN) If expression parsing fails, fall back to variable name
                        exprNode = std::make_unique<VariableExpr>(exprText, pos);
                    }

                    if (!exprNode)
                    {
                        exprNode = std::make_unique<VariableExpr>(exprText, pos);
                    }

                    // Wrap in نص() call to convert to string
                    // (AR) لف في استدعاء نص() للتحويل إلى نص
                    std::vector<ExprPtr> strArgs;
                    strArgs.push_back(std::move(exprNode));
                    auto strCall = std::make_unique<CallExpr>(
                        std::make_unique<VariableExpr>("\xd9\x86\xd8\xb5", pos),
                        std::move(strArgs));

                    if (result)
                    {
                        // Concatenate with previous parts
                        // تسلسل مع الأجزاء السابقة
                        result = std::make_unique<BinaryExpr>(
                            std::move(result),
                            TT::OP_PLUS,
                            std::move(strCall),
                            pos);
                    }
                    else
                    {
                        result = std::move(strCall);
                    }

                    i++; // Move past closing brace
                }
                else if (fstring[i] == '{' && i + 1 < fstring.length() && fstring[i + 1] == '{')
                {
                    // Escaped brace {{
                    // قوس مُتجاوز {{
                    currentText += '{';
                    i += 2;
                }
                else if (fstring[i] == '}' && i + 1 < fstring.length() && fstring[i + 1] == '}')
                {
                    // Escaped brace }}
                    // قوس مُتجاوز }}
                    currentText += '}';
                    i += 2;
                }
                else
                {
                    // Regular character
                    // حرف عادي
                    currentText += fstring[i];
                    i++;
                }
            }

            // Add any remaining text
            // أضف أي نص متبقي
            if (!currentText.empty())
            {
                auto textLiteral = std::make_unique<LiteralExpr>(
                    Lexer::Token(TT::STRING_LITERAL, currentText, pos));
                if (result)
                {
                    result = std::make_unique<BinaryExpr>(
                        std::move(result),
                        TT::OP_PLUS,
                        std::move(textLiteral),
                        pos);
                }
                else
                {
                    result = std::move(textLiteral);
                }
            }

            return result ? std::move(result) : std::make_unique<LiteralExpr>(Lexer::Token(TT::STRING_LITERAL, "", pos));
        }

        /**
         * @brief (AR) يحلل مُزخرِف (decorator): @decorator أو @decorator(args).
         *        (EN) Parses decorator: @decorator or @decorator(args).
         */
        ExprPtr ParserCore::parseDecorator()
        {
            // Expect @ sign (should already be consumed by caller)
            // (AR) نتوقع @ (يجب أن يكون مُستهلكاً من المُستدعي)
            Token atSign = previous();

            // Expect decorator name
            // (AR) نتوقع اسم المُزخرِف
            if (!check(TT::IDENTIFIER))
            {
                errorBilingual(
                    "خطأ: توقعت اسم المُزخرِف بعد @. مثال: @property أو @staticmethod",
                    "Error: expected decorator name after @. Example: @property or @staticmethod");
                return nullptr;
            }
            Token decoratorName = peek();
            advance();

            // Check for arguments
            // (AR) التحقق من الوسائط
            if (check(TT::PAREN_LEFT))
            {
                // Decorator with arguments: @decorator(arg1, arg2, ...)
                // (AR) مُزخرِف مع وسائط
                advance(); // consume (

                ExprList args;

                // Parse arguments if not empty
                // (AR) تحليل الوسائط إذا لم تكن فارغة
                if (!check(TT::PAREN_RIGHT))
                {
                    args = parseArgumentList();
                    if (args.empty() && !check(TT::PAREN_RIGHT))
                    {
                        errorBilingual(
                            "خطأ: فشل تحليل وسائط المُزخرِف.",
                            "Error: failed to parse decorator arguments.");
                        return nullptr;
                    }
                }

                if (!check(TT::PAREN_RIGHT))
                {
                    errorBilingual(
                        "خطأ: توقعت ')' بعد وسائط المُزخرِف. هل نسيت إغلاق الأقواس؟",
                        "Error: expected ')' after decorator arguments. Did you forget to close the parenthesis?");
                    return nullptr;
                }
                consume(TT::PAREN_RIGHT, "");

                // Create decorator with arguments
                // (AR) إنشاء مُزخرِف مع وسائط
                return std::make_unique<DecoratorExpr>(
                    decoratorName.getValue(),
                    std::move(args),
                    atSign.getPosition());
            }

            // Decorator without arguments: @decorator
            // (AR) مُزخرِف بدون وسائط
            return std::make_unique<DecoratorExpr>(
                decoratorName.getValue(),
                atSign.getPosition());
        }

    } // namespace Parser
} // namespace Sad
