/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: binding_compiler.cpp
 * المسار: features/graphics/reactive/binding_compiler.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * @brief (AR) تنفيذ مُجمّع الربطات — تحليل لامدات AST وتحويلها لدوال C++
 * @brief (EN) Binding compiler implementation — AST lambda analysis and C++ conversion
 *
 * التدفق:
 * ───────
 *   compile(LambdaExpr*)
 *     │
 *     ├─ 1. فحص: اللامدا لها معامل واحد فقط
 *     ├─ 2. فحص: اللامدا لها جسم تعبيري (body) وليس كتلي (blockBody)
 *     ├─ 3. analyze(body, paramName) → BindingExprNode (IR مصغّر)
 *     ├─ 4. فحص التعقيد: nodeCount <= maxNodeCount && depth <= maxDepth
 *     └─ 5. compileNode(IR) → CompiledTransform (C++ lambda)
 *
 * evaluateNode() تعمل تكرارياً:
 *   - LiteralInteger → BindingValue(intVal)
 *   - ParamRef → input (قيمة المعامل)
 *   - BinaryAdd → left + right (حسابي أو دمج نصوص)
 *   - ToStringBuiltin → arg.toString()
 *   - ...إلخ
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "binding_compiler.h"

// (AR) نشمل ملفات AST هنا فقط (في cpp وليس في header)
//      لتقليل التبعيات في ملف الـ header
// (EN) Include AST files only here (in cpp, not in header)
//      to reduce dependencies in the header file
#include "expressions.h"
#include "statements.h"
#include "token.h"

#include <sstream>
#include <cmath>
#include <cassert>

namespace sad
{
    namespace ui
    {

        // ═══════════════════════════════════════════════════════════════════
        // BindingValue — تنفيذ الدوال المساعدة
        // ═══════════════════════════════════════════════════════════════════

        std::string BindingValue::toString() const
        {
            switch (kind)
            {
            case Kind::Null:
                return "لاشيء";
            case Kind::Integer:
                return std::to_string(asInteger());
            case Kind::Double:
            {
                // (AR) إزالة الأصفار اللاحقة غير الضرورية
                // (EN) Remove trailing zeros
                std::ostringstream oss;
                oss << asDouble();
                return oss.str();
            }
            case Kind::String:
                return asString();
            case Kind::Boolean:
                return asBoolean() ? "صحيح" : "خطأ";
            }
            return "";
        }

        double BindingValue::toDouble() const
        {
            switch (kind)
            {
            case Kind::Integer:
                return static_cast<double>(asInteger());
            case Kind::Double:
                return asDouble();
            case Kind::Boolean:
                return asBoolean() ? 1.0 : 0.0;
            default:
                return 0.0;
            }
        }

        bool BindingValue::operator==(const BindingValue &other) const
        {
            if (kind != other.kind)
                return false;

            switch (kind)
            {
            case Kind::Null:
                return true;
            case Kind::Integer:
                return asInteger() == other.asInteger();
            case Kind::Double:
                return asDouble() == other.asDouble();
            case Kind::String:
                return asString() == other.asString();
            case Kind::Boolean:
                return asBoolean() == other.asBoolean();
            }
            return false;
        }

        // ═══════════════════════════════════════════════════════════════════
        // BindingExprNode — مُنشئات مساعدة
        // ═══════════════════════════════════════════════════════════════════

        std::unique_ptr<BindingExprNode> BindingExprNode::makeLiteralInt(int64_t v)
        {
            auto node = std::make_unique<BindingExprNode>();
            node->type = Type::LiteralInteger;
            node->intVal = v;
            return node;
        }

        std::unique_ptr<BindingExprNode> BindingExprNode::makeLiteralDouble(double v)
        {
            auto node = std::make_unique<BindingExprNode>();
            node->type = Type::LiteralDouble;
            node->doubleVal = v;
            return node;
        }

        std::unique_ptr<BindingExprNode> BindingExprNode::makeLiteralString(const std::string &v)
        {
            auto node = std::make_unique<BindingExprNode>();
            node->type = Type::LiteralString;
            node->strVal = v;
            return node;
        }

        std::unique_ptr<BindingExprNode> BindingExprNode::makeLiteralBool(bool v)
        {
            auto node = std::make_unique<BindingExprNode>();
            node->type = Type::LiteralBoolean;
            node->boolVal = v;
            return node;
        }

        std::unique_ptr<BindingExprNode> BindingExprNode::makeParamRef(const std::string &name)
        {
            auto node = std::make_unique<BindingExprNode>();
            node->type = Type::ParamRef;
            node->paramName = name;
            return node;
        }

        std::unique_ptr<BindingExprNode> BindingExprNode::makeBinary(
            Type op, std::unique_ptr<BindingExprNode> l, std::unique_ptr<BindingExprNode> r)
        {
            auto node = std::make_unique<BindingExprNode>();
            node->type = op;
            node->left = std::move(l);
            node->right = std::move(r);
            return node;
        }

        std::unique_ptr<BindingExprNode> BindingExprNode::makeUnary(
            Type op, std::unique_ptr<BindingExprNode> operand)
        {
            auto node = std::make_unique<BindingExprNode>();
            node->type = op;
            node->left = std::move(operand);
            return node;
        }

        std::unique_ptr<BindingExprNode> BindingExprNode::makeBuiltinConvert(
            Type conv, std::unique_ptr<BindingExprNode> arg)
        {
            auto node = std::make_unique<BindingExprNode>();
            node->type = conv;
            node->left = std::move(arg);
            return node;
        }

        size_t BindingExprNode::countNodes() const
        {
            size_t count = 1;
            if (left)
                count += left->countNodes();
            if (right)
                count += right->countNodes();
            return count;
        }

        // ═══════════════════════════════════════════════════════════════════
        // BindingCompiler::compile — الواجهة الرئيسية
        // ═══════════════════════════════════════════════════════════════════

        CompilationResult BindingCompiler::compile(const Sad::AST::LambdaExpr *lambda)
        {
            stats_.totalAttempts++;

            // ─── فحص 1: اللامدا غير فارغة ───
            if (!lambda)
            {
                stats_.failureCount++;
                return CompilationResult::failure("لامدا فارغة (nullptr)");
            }

            // ─── فحص 2: معامل واحد فقط ───
            // (AR) BindingCompiler يدعم فقط لامدات بمعامل واحد (قيمة الإشارة)
            //      لامدات بدون معاملات أو أكثر من معامل → مُفسَّرة
            if (lambda->parameters.size() != 1)
            {
                stats_.failureCount++;
                return CompilationResult::failure(
                    "لامدا بمعاملات غير مدعومة: " +
                    std::to_string(lambda->parameters.size()) + " معامل (المطلوب: 1)");
            }

            const std::string &paramName = lambda->parameters[0].name;

            // ─── فحص 3: جسم تعبيري (body) وليس كتلي (blockBody) ───
            // (AR) لامدا(ق) ارجع تعبير نهاية → body هو التعبير
            //      لامدا(ق) ... كتلة أوامر ... نهاية → blockBody
            // (EN) Lambda with expression body uses body field
            //      Lambda with block body uses blockBody field
            const Sad::AST::Expression *bodyExpr = nullptr;

            if (lambda->body)
            {
                // (AR) جسم تعبيري مباشر
                bodyExpr = lambda->body.get();
            }
            else if (lambda->blockBody)
            {
                // (AR) جسم كتلي — نتحقق إذا كان كتلة واحدة فيها ارجع فقط
                //      أي: لامدا(ق) ارجع "النص: " + نص(ق) نهاية
                auto blockStmt = dynamic_cast<const Sad::AST::BlockStmt *>(lambda->blockBody.get());
                if (blockStmt && blockStmt->statements.size() == 1)
                {
                    auto returnStmt = dynamic_cast<const Sad::AST::ReturnStmt *>(
                        blockStmt->statements[0].get());
                    if (returnStmt && returnStmt->value)
                    {
                        bodyExpr = returnStmt->value.get();
                    }
                }

                if (!bodyExpr)
                {
                    stats_.failureCount++;
                    return CompilationResult::failure(
                        "لامدا بجسم كتلي معقد (أكثر من return واحد)");
                }
            }
            else
            {
                stats_.failureCount++;
                return CompilationResult::failure("لامدا بدون جسم");
            }

            // ─── الخطوة 3: تحليل التعبير → IR مصغّر ───
            auto irNode = analyze(bodyExpr, paramName);
            if (!irNode)
            {
                stats_.failureCount++;
                return CompilationResult::failure("تعبير غير مدعوم في اللامدا");
            }

            // ─── الخطوة 4: فحص التعقيد ───
            size_t nodeCount = irNode->countNodes();
            if (nodeCount > maxNodeCount_)
            {
                stats_.tooComplexCount++;
                stats_.failureCount++;
                return CompilationResult::failure(
                    "لامدا معقدة جداً: " + std::to_string(nodeCount) +
                    " عقدة (الحد: " + std::to_string(maxNodeCount_) + ")");
            }

            if (!checkDepth(irNode.get()))
            {
                stats_.tooComplexCount++;
                stats_.failureCount++;
                return CompilationResult::failure(
                    "لامدا عميقة جداً (أكثر من " + std::to_string(maxDepth_) + " مستوى)");
            }

            // ─── الخطوة 5: فحص النقاء ───
            bool pure = isPureExpression(irNode.get());

            // ─── الخطوة 6: التجميع → C++ function ───
            auto transform = compileNode(irNode.get());
            if (!transform)
            {
                stats_.failureCount++;
                return CompilationResult::failure("فشل التجميع الداخلي");
            }

            stats_.successCount++;
            return CompilationResult::success(std::move(transform), nodeCount, pure);
        }

        // ═══════════════════════════════════════════════════════════════════
        // analyze — تحويل AST → IR مصغّر
        // ═══════════════════════════════════════════════════════════════════

        std::unique_ptr<BindingExprNode> BindingCompiler::analyze(
            const Sad::AST::Expression *expr, const std::string &paramName)
        {
            if (!expr)
                return nullptr;

            // ─── LiteralExpr (رقم صحيح/عشري/نص/منطقي) ───
            if (auto litExpr = dynamic_cast<const Sad::AST::LiteralExpr *>(expr))
            {
                return analyzeLiteral(litExpr);
            }

            // ─── VariableExpr (معرّف/متغير) ───
            if (auto varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(expr))
            {
                return analyzeVariable(varExpr, paramName);
            }

            // ─── BinaryExpr ───
            if (auto binExpr = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
            {
                return analyzeBinary(binExpr, paramName);
            }

            // ─── UnaryExpr ───
            if (auto unaryExpr = dynamic_cast<const Sad::AST::UnaryExpr *>(expr))
            {
                return analyzeUnary(unaryExpr, paramName);
            }

            // ─── CallExpr (لدوال التحويل المضمنة فقط) ───
            if (auto callExpr = dynamic_cast<const Sad::AST::CallExpr *>(expr))
            {
                return analyzeFunctionCall(callExpr, paramName);
            }

            // ─── أي شيء آخر → غير مدعوم ───
            return nullptr;
        }

        // ═══════════════════════════════════════════════════════════════════
        // محللات AST الداخلية
        // ═══════════════════════════════════════════════════════════════════

        std::unique_ptr<BindingExprNode> BindingCompiler::analyzeBinary(
            const Sad::AST::BinaryExpr *expr, const std::string &paramName)
        {
            if (!expr || !expr->left || !expr->right)
                return nullptr;

            auto left = analyze(expr->left.get(), paramName);
            auto right = analyze(expr->right.get(), paramName);

            if (!left || !right)
                return nullptr;

            // (AR) تحويل TokenType إلى نوع IR مصغّر
            // (EN) Convert TokenType to mini-IR type
            using TT = Sad::Lexer::TokenType;
            BindingExprNode::Type irOp;

            switch (expr->op)
            {
            case TT::OP_PLUS:
                irOp = BindingExprNode::Type::BinaryAdd;
                break;
            case TT::OP_MINUS:
                irOp = BindingExprNode::Type::BinarySub;
                break;
            case TT::OP_MULTIPLY:
                irOp = BindingExprNode::Type::BinaryMul;
                break;
            case TT::OP_DIVIDE:
                irOp = BindingExprNode::Type::BinaryDiv;
                break;
            case TT::OP_MODULO:
                irOp = BindingExprNode::Type::BinaryMod;
                break;
            case TT::OP_GREATER:
                irOp = BindingExprNode::Type::BinaryGt;
                break;
            case TT::OP_LESS:
                irOp = BindingExprNode::Type::BinaryLt;
                break;
            case TT::OP_GREATER_EQUAL:
                irOp = BindingExprNode::Type::BinaryGte;
                break;
            case TT::OP_LESS_EQUAL:
                irOp = BindingExprNode::Type::BinaryLte;
                break;
            case TT::OP_EQUAL:
                irOp = BindingExprNode::Type::BinaryEq;
                break;
            case TT::OP_NOT_EQUAL:
                irOp = BindingExprNode::Type::BinaryNeq;
                break;
            case TT::OP_AND:
                irOp = BindingExprNode::Type::BinaryAnd;
                break;
            case TT::OP_OR:
                irOp = BindingExprNode::Type::BinaryOr;
                break;
            default:
                return nullptr; // (AR) عامل غير مدعوم
            }

            return BindingExprNode::makeBinary(irOp, std::move(left), std::move(right));
        }

        std::unique_ptr<BindingExprNode> BindingCompiler::analyzeUnary(
            const Sad::AST::UnaryExpr *expr, const std::string &paramName)
        {
            if (!expr || !expr->operand)
                return nullptr;

            auto operand = analyze(expr->operand.get(), paramName);
            if (!operand)
                return nullptr;

            using TT = Sad::Lexer::TokenType;
            BindingExprNode::Type irOp;

            switch (expr->op)
            {
            case TT::OP_MINUS:
                irOp = BindingExprNode::Type::UnaryNeg;
                break;
            case TT::OP_NOT:
                irOp = BindingExprNode::Type::UnaryNot;
                break;
            default:
                return nullptr;
            }

            return BindingExprNode::makeUnary(irOp, std::move(operand));
        }

        std::unique_ptr<BindingExprNode> BindingCompiler::analyzeVariable(
            const Sad::AST::VariableExpr *expr, const std::string &paramName)
        {
            if (!expr)
                return nullptr;

            // (AR) إذا كان المعرّف هو معامل اللامدا → ParamRef
            // (EN) If the identifier is the lambda parameter → ParamRef
            if (expr->name == paramName)
            {
                return BindingExprNode::makeParamRef(paramName);
            }

            // (AR) أي معرّف آخر = متغير خارجي (closure) → غير مدعوم
            //      لأن BindingCompiler لا يستطيع الوصول لمتغيرات المفسر
            // (EN) Any other identifier = external variable (closure) → unsupported
            return nullptr;
        }

        std::unique_ptr<BindingExprNode> BindingCompiler::analyzeLiteral(
            const Sad::AST::LiteralExpr *expr)
        {
            if (!expr)
                return nullptr;

            using TT = Sad::Lexer::TokenType;
            const auto &token = expr->token;

            switch (token.getType())
            {
            case TT::NUMBER_INTEGER:
            {
                try
                {
                    int64_t val = std::stoll(token.getValue());
                    return BindingExprNode::makeLiteralInt(val);
                }
                catch (...)
                {
                    return nullptr;
                }
            }

            case TT::NUMBER_DOUBLE:
            {
                try
                {
                    double val = std::stod(token.getValue());
                    return BindingExprNode::makeLiteralDouble(val);
                }
                catch (...)
                {
                    return nullptr;
                }
            }

            case TT::STRING_LITERAL:
                return BindingExprNode::makeLiteralString(token.getValue());

            case TT::LITERAL_TRUE:
                return BindingExprNode::makeLiteralBool(true);

            case TT::LITERAL_FALSE:
                return BindingExprNode::makeLiteralBool(false);

            default:
                return nullptr;
            }
        }

        std::unique_ptr<BindingExprNode> BindingCompiler::analyzeFunctionCall(
            const Sad::AST::CallExpr *expr, const std::string &paramName)
        {
            if (!expr)
                return nullptr;

            // (AR) نستخرج اسم الدالة المُستدعاة
            // (EN) Extract the called function name
            std::string funcName;
            if (auto varCallee = dynamic_cast<const Sad::AST::VariableExpr *>(expr->callee.get()))
            {
                funcName = varCallee->name;
            }
            else
            {
                // (AR) استدعاء غير مباشر (مثل: دالة().شيء()) → غير مدعوم
                return nullptr;
            }

            // (AR) فحص الدوال المضمنة لتحويل الأنواع
            // (EN) Check built-in type conversion functions

            // (AR) يجب أن تكون الدالة بمعامل واحد فقط
            if (expr->arguments.size() != 1)
                return nullptr;

            auto arg = analyze(expr->arguments[0].get(), paramName);
            if (!arg)
                return nullptr;

            // (AR) مطابقة أسماء دوال التحويل المضمنة (عربي + إنجليزي)
            // (EN) Match built-in type conversion function names (Arabic + English)

            // نص / str / string / toString
            if (funcName == "\xD9\x86\xD8\xB5" || funcName == "str" ||
                funcName == "string" || funcName == "toString")
            {
                return BindingExprNode::makeBuiltinConvert(
                    BindingExprNode::Type::ToStringBuiltin, std::move(arg));
            }

            // رقم / int / integer / toInt
            if (funcName == "\xD8\xB1\xD9\x82\xD9\x85" || funcName == "int" ||
                funcName == "integer" || funcName == "toInt")
            {
                return BindingExprNode::makeBuiltinConvert(
                    BindingExprNode::Type::ToIntBuiltin, std::move(arg));
            }

            // عشري / float / double / toFloat
            if (funcName == "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A" || funcName == "float" ||
                funcName == "double" || funcName == "toFloat")
            {
                return BindingExprNode::makeBuiltinConvert(
                    BindingExprNode::Type::ToDoubleBuiltin, std::move(arg));
            }

            // منطقي / bool / boolean / toBool
            if (funcName == "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A" || funcName == "bool" ||
                funcName == "boolean" || funcName == "toBool")
            {
                return BindingExprNode::makeBuiltinConvert(
                    BindingExprNode::Type::ToBoolBuiltin, std::move(arg));
            }

            // (AR) أي دالة أخرى → غير مدعومة (تبقى مُفسَّرة)
            return nullptr;
        }

        // ═══════════════════════════════════════════════════════════════════
        // فحص التعقيد
        // ═══════════════════════════════════════════════════════════════════

        bool BindingCompiler::checkDepth(const BindingExprNode *node, size_t currentDepth) const
        {
            if (!node)
                return true;

            if (currentDepth > maxDepth_)
                return false;

            bool leftOk = checkDepth(node->left.get(), currentDepth + 1);
            bool rightOk = checkDepth(node->right.get(), currentDepth + 1);

            return leftOk && rightOk;
        }

        bool BindingCompiler::isPureExpression(const BindingExprNode *node) const
        {
            if (!node)
                return true;

            // (AR) العقد الحرفية نقية دائماً
            switch (node->type)
            {
            case BindingExprNode::Type::LiteralInteger:
            case BindingExprNode::Type::LiteralDouble:
            case BindingExprNode::Type::LiteralString:
            case BindingExprNode::Type::LiteralBoolean:
            case BindingExprNode::Type::ParamRef:
                return true;
            default:
                break;
            }

            // (AR) العمليات الثنائية/الأحادية/التحويل نقية إذا أبناؤها نقية
            bool leftPure = isPureExpression(node->left.get());
            bool rightPure = isPureExpression(node->right.get());

            return leftPure && rightPure;
        }

        // ═══════════════════════════════════════════════════════════════════
        // compileNode — تحويل IR مصغّر → C++ function
        // ═══════════════════════════════════════════════════════════════════

        CompiledTransform BindingCompiler::compileNode(const BindingExprNode *node)
        {
            if (!node)
                return nullptr;

            // (AR) ننشئ C++ lambda تلتقط عقدة IR بنسخة خام (raw pointer)
            //      لأن العقدة تعيش طالما CompilationResult حي
            //      لكن هذا غير آمن — بدلاً من ذلك، ننشئ closure تلتقط القيم مباشرة
            //
            //      الحل: بناء الدالة تكرارياً — كل مستوى ينشئ lambda محلية
            //      والنتيجة النهائية هي lambda واحدة تحسب كل شيء
            // (EN) We build the function recursively — each level creates a local lambda
            //      The final result is a single lambda that computes everything

            // (AR) نستخدم evaluateNode الساكنة مع نسخة من العقدة
            //      نحتاج نسخ الشجرة لأن العقدة الأصلية ستُحرر
            // (EN) We use static evaluateNode with a copy of the node
            //      We need to clone the tree because the original will be freed

            // (AR) بناء نسخة عميقة من شجرة IR المصغّرة
            //      لأن اللامدا المُرجعة قد تعيش أطول من BindingExprNode الأصلية
            // (EN) Build a deep copy of the mini-IR tree
            //      because the returned lambda may outlive the original BindingExprNode
            auto cloneNode = [](const BindingExprNode *src,
                                auto &cloneFn) -> std::shared_ptr<BindingExprNode>
            {
                if (!src)
                    return nullptr;

                auto copy = std::make_shared<BindingExprNode>();
                copy->type = src->type;
                copy->intVal = src->intVal;
                copy->doubleVal = src->doubleVal;
                copy->strVal = src->strVal;
                copy->boolVal = src->boolVal;
                copy->paramName = src->paramName;

                if (src->left)
                {
                    auto leftClone = cloneFn(src->left.get(), cloneFn);
                    copy->left = std::make_unique<BindingExprNode>();
                    *copy->left = std::move(*leftClone);
                }

                if (src->right)
                {
                    auto rightClone = cloneFn(src->right.get(), cloneFn);
                    copy->right = std::make_unique<BindingExprNode>();
                    *copy->right = std::move(*rightClone);
                }

                return copy;
            };

            auto clonedTree = cloneNode(node, cloneNode);

            // (AR) اللامدا المُرجعة تلتقط الشجرة المنسوخة بـ shared_ptr
            //      وتستدعي evaluateNode عند كل تنفيذ
            // (EN) The returned lambda captures the cloned tree via shared_ptr
            //      and calls evaluateNode on each execution
            return [tree = std::move(clonedTree)](const BindingValue &input) -> BindingValue
            {
                return evaluateNode(tree.get(), input);
            };
        }

        // ═══════════════════════════════════════════════════════════════════
        // evaluateNode — تقييم عقدة IR مع قيمة مُدخلة
        // ═══════════════════════════════════════════════════════════════════

        BindingValue BindingCompiler::evaluateNode(const BindingExprNode *node,
                                                   const BindingValue &input)
        {
            if (!node)
                return BindingValue();

            switch (node->type)
            {
            // ─── القيم الحرفية ───
            case BindingExprNode::Type::LiteralInteger:
                return BindingValue(node->intVal);

            case BindingExprNode::Type::LiteralDouble:
                return BindingValue(node->doubleVal);

            case BindingExprNode::Type::LiteralString:
                return BindingValue(node->strVal);

            case BindingExprNode::Type::LiteralBoolean:
                return BindingValue(node->boolVal);

            // ─── مرجع المعامل ───
            case BindingExprNode::Type::ParamRef:
                return input;

            // ─── العمليات الثنائية ───
            case BindingExprNode::Type::BinaryAdd:
            {
                auto leftVal = evaluateNode(node->left.get(), input);
                auto rightVal = evaluateNode(node->right.get(), input);

                // (AR) دمج نصوص: إذا أحدهما نص → concat
                if (leftVal.isString() || rightVal.isString())
                {
                    return BindingValue(leftVal.toString() + rightVal.toString());
                }

                // (AR) جمع عشري: إذا أحدهما عشري → double
                if (leftVal.isDouble() || rightVal.isDouble())
                {
                    return BindingValue(leftVal.toDouble() + rightVal.toDouble());
                }

                // (AR) جمع أعداد صحيحة
                if (leftVal.isInteger() && rightVal.isInteger())
                {
                    return BindingValue(leftVal.asInteger() + rightVal.asInteger());
                }

                return BindingValue();
            }

            case BindingExprNode::Type::BinarySub:
            {
                auto leftVal = evaluateNode(node->left.get(), input);
                auto rightVal = evaluateNode(node->right.get(), input);

                if (leftVal.isDouble() || rightVal.isDouble())
                    return BindingValue(leftVal.toDouble() - rightVal.toDouble());

                if (leftVal.isInteger() && rightVal.isInteger())
                    return BindingValue(leftVal.asInteger() - rightVal.asInteger());

                return BindingValue();
            }

            case BindingExprNode::Type::BinaryMul:
            {
                auto leftVal = evaluateNode(node->left.get(), input);
                auto rightVal = evaluateNode(node->right.get(), input);

                if (leftVal.isDouble() || rightVal.isDouble())
                    return BindingValue(leftVal.toDouble() * rightVal.toDouble());

                if (leftVal.isInteger() && rightVal.isInteger())
                    return BindingValue(leftVal.asInteger() * rightVal.asInteger());

                return BindingValue();
            }

            case BindingExprNode::Type::BinaryDiv:
            {
                auto leftVal = evaluateNode(node->left.get(), input);
                auto rightVal = evaluateNode(node->right.get(), input);

                // (AR) فحص القسمة على صفر
                // (EN) Division by zero check
                double rightD = rightVal.toDouble();
                if (rightD == 0.0)
                    return BindingValue(0.0);

                return BindingValue(leftVal.toDouble() / rightD);
            }

            case BindingExprNode::Type::BinaryMod:
            {
                auto leftVal = evaluateNode(node->left.get(), input);
                auto rightVal = evaluateNode(node->right.get(), input);

                if (leftVal.isInteger() && rightVal.isInteger())
                {
                    int64_t r = rightVal.asInteger();
                    if (r == 0)
                        return BindingValue(static_cast<int64_t>(0));
                    return BindingValue(leftVal.asInteger() % r);
                }

                double r = rightVal.toDouble();
                if (r == 0.0)
                    return BindingValue(0.0);
                return BindingValue(std::fmod(leftVal.toDouble(), r));
            }

            // ─── المقارنات ───
            case BindingExprNode::Type::BinaryGt:
            {
                auto l = evaluateNode(node->left.get(), input);
                auto r = evaluateNode(node->right.get(), input);
                return BindingValue(l.toDouble() > r.toDouble());
            }

            case BindingExprNode::Type::BinaryLt:
            {
                auto l = evaluateNode(node->left.get(), input);
                auto r = evaluateNode(node->right.get(), input);
                return BindingValue(l.toDouble() < r.toDouble());
            }

            case BindingExprNode::Type::BinaryGte:
            {
                auto l = evaluateNode(node->left.get(), input);
                auto r = evaluateNode(node->right.get(), input);
                return BindingValue(l.toDouble() >= r.toDouble());
            }

            case BindingExprNode::Type::BinaryLte:
            {
                auto l = evaluateNode(node->left.get(), input);
                auto r = evaluateNode(node->right.get(), input);
                return BindingValue(l.toDouble() <= r.toDouble());
            }

            case BindingExprNode::Type::BinaryEq:
            {
                auto l = evaluateNode(node->left.get(), input);
                auto r = evaluateNode(node->right.get(), input);
                return BindingValue(l == r);
            }

            case BindingExprNode::Type::BinaryNeq:
            {
                auto l = evaluateNode(node->left.get(), input);
                auto r = evaluateNode(node->right.get(), input);
                return BindingValue(l != r);
            }

            case BindingExprNode::Type::BinaryAnd:
            {
                auto l = evaluateNode(node->left.get(), input);
                if (l.isBoolean() && !l.asBoolean())
                    return BindingValue(false); // (AR) اختصار: إذا اليسار خطأ → خطأ
                auto r = evaluateNode(node->right.get(), input);
                return BindingValue(l.asBoolean() && r.asBoolean());
            }

            case BindingExprNode::Type::BinaryOr:
            {
                auto l = evaluateNode(node->left.get(), input);
                if (l.isBoolean() && l.asBoolean())
                    return BindingValue(true); // (AR) اختصار: إذا اليسار صحيح → صحيح
                auto r = evaluateNode(node->right.get(), input);
                return BindingValue(l.asBoolean() || r.asBoolean());
            }

            // ─── العمليات الأحادية ───
            case BindingExprNode::Type::UnaryNeg:
            {
                auto val = evaluateNode(node->left.get(), input);
                if (val.isInteger())
                    return BindingValue(-val.asInteger());
                if (val.isDouble())
                    return BindingValue(-val.toDouble());
                return BindingValue();
            }

            case BindingExprNode::Type::UnaryNot:
            {
                auto val = evaluateNode(node->left.get(), input);
                if (val.isBoolean())
                    return BindingValue(!val.asBoolean());
                return BindingValue(false);
            }

            // ─── تحويل الأنواع المضمن ───
            case BindingExprNode::Type::ToStringBuiltin:
            {
                auto val = evaluateNode(node->left.get(), input);
                return BindingValue(val.toString());
            }

            case BindingExprNode::Type::ToIntBuiltin:
            {
                auto val = evaluateNode(node->left.get(), input);
                if (val.isInteger())
                    return val;
                if (val.isDouble())
                    return BindingValue(static_cast<int64_t>(val.asDouble()));
                if (val.isString())
                {
                    try
                    {
                        // (AR) cast صريح إلى int64_t: std::stoll تُرجع long long،
                        //      وعلى Linux (LP64) int64_t=long فيصير الاستدعاء غامضًا
                        //      بين بناة BindingValue. على Windows int64_t=long long
                        //      فلا يظهر الغموض — لذا الإصلاح ضروري لبناء GCC/Clang.
                        // (EN) Explicit int64_t cast: std::stoll returns long long;
                        //      on Linux (LP64) int64_t=long, making the call ambiguous
                        //      across BindingValue ctors (not on Windows LLP64).
                        return BindingValue(static_cast<int64_t>(std::stoll(val.asString())));
                    }
                    catch (...)
                    {
                        return BindingValue(static_cast<int64_t>(0));
                    }
                }
                if (val.isBoolean())
                    return BindingValue(val.asBoolean() ? static_cast<int64_t>(1) : static_cast<int64_t>(0));
                return BindingValue(static_cast<int64_t>(0));
            }

            case BindingExprNode::Type::ToDoubleBuiltin:
            {
                auto val = evaluateNode(node->left.get(), input);
                if (val.isDouble())
                    return val;
                if (val.isInteger())
                    return BindingValue(static_cast<double>(val.asInteger()));
                if (val.isString())
                {
                    try
                    {
                        return BindingValue(std::stod(val.asString()));
                    }
                    catch (...)
                    {
                        return BindingValue(0.0);
                    }
                }
                return BindingValue(0.0);
            }

            case BindingExprNode::Type::ToBoolBuiltin:
            {
                auto val = evaluateNode(node->left.get(), input);
                if (val.isBoolean())
                    return val;
                if (val.isInteger())
                    return BindingValue(val.asInteger() != 0);
                if (val.isDouble())
                    return BindingValue(val.asDouble() != 0.0);
                if (val.isString())
                    return BindingValue(!val.asString().empty());
                return BindingValue(false);
            }
            }

            return BindingValue();
        }

    } // namespace ui
} // namespace sad
