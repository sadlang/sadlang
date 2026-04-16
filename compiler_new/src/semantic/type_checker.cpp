// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
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

namespace Sad
{
    namespace Semantic
    {

        using namespace TypeSystem;
        using TT = Lexer::TokenType;

        // ============================================================================
        // TypeCheckError
        // ============================================================================

        std::string TypeCheckError::toEnglishString() const
        {
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

        std::string TypeCheckError::toArabicString() const
        {
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
            : useArabicMessages_(true), debugMode_(false), strictMode_(false), registry_(TypeRegistry::getInstance()), lastInferredType_(nullptr), expectedReturnType_(nullptr)
        {
            // إنشاء بيئة الأنواع العامة / Create global type environment
            currentEnv_ = std::make_shared<TypeEnvironment>();
            scopeStack_.push_back(currentEnv_);

            // تسجيل الدوال المدمجة / Register built-in functions
            currentEnv_->bind("اطبع", registry_.getVoidType());        // print
            currentEnv_->bind("اطبع_سطر", registry_.getVoidType());    // println
            currentEnv_->bind("ادخل", registry_.getStringType());      // input
            currentEnv_->bind("ادخل_رقم", registry_.getIntegerType()); // input_number
            currentEnv_->bind("طول", registry_.getIntegerType());      // length
            currentEnv_->bind("نوع", registry_.getStringType());       // typeof

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

        TypeCheckResult TypeChecker::check(AST::ASTNode *ast)
        {
            currentResult_ = TypeCheckResult();

            if (!ast)
            {
                currentResult_.addError({"", "", "", 0, 0,
                                         "Null AST provided to type checker",
                                         "تم تمرير شجرة AST فارغة لفاحص الأنواع"});
                return currentResult_;
            }

            try
            {
                ast->accept(*this);
            }
            catch (const std::exception &e)
            {
                currentResult_.addError({"", "", "", 0, 0,
                                         std::string("Internal type checker error: ") + e.what(),
                                         std::string("خطأ داخلي في فاحص الأنواع: ") + e.what()});
            }

            if (debugMode_)
            {
                if (useArabicMessages_)
                    std::cerr << currentResult_.getArabicSummary();
                else
                    std::cerr << currentResult_.getSummary();
            }

            return currentResult_;
        }

        void TypeChecker::printSummary() const
        {
            if (useArabicMessages_)
            {
                std::cerr << currentResult_.getArabicSummary();
            }
            else
            {
                std::cerr << currentResult_.getSummary();
            }

            // طباعة الأخطاء / Print errors
            for (const auto &err : currentResult_.errors)
            {
                if (useArabicMessages_)
                {
                    std::cerr << "  ❌ خطأ نوع";
                    if (!err.variableName.empty())
                        std::cerr << " في '" << err.variableName << "'";
                    std::cerr << ": " << err.arabicMessage;
                    if (err.line > 0)
                        std::cerr << " (سطر " << err.line << ")";
                    std::cerr << "\n";
                }
                else
                {
                    std::cerr << "  ❌ Type error";
                    if (!err.variableName.empty())
                        std::cerr << " in '" << err.variableName << "'";
                    std::cerr << ": " << err.message;
                    if (err.line > 0)
                        std::cerr << " (line " << err.line << ")";
                    std::cerr << "\n";
                }
            }

            // طباعة التحذيرات / Print warnings
            for (const auto &warn : currentResult_.warnings)
            {
                std::cerr << "  ⚠️ " << warn << "\n";
            }
        }

        void TypeChecker::reset()
        {
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

        TypePtr TypeChecker::dataTypeToTypePtr(Data::DataType dt) const
        {
            switch (dt)
            {
            case Data::DataType::INTEGER:
                return registry_.getIntegerType();
            case Data::DataType::FLOAT:
                return registry_.getFloatType();
            case Data::DataType::STRING:
                return registry_.getStringType();
            case Data::DataType::BOOLEAN:
                return registry_.getBooleanType();
            case Data::DataType::NONE:
                return registry_.getVoidType();
            case Data::DataType::ARRAY:
                return std::make_shared<ArrayType>(registry_.getAnyType()); // عنصر المصفوفة يُحدد لاحقاً
            case Data::DataType::MAP:
                return std::make_shared<DictionaryType>(registry_.getStringType(), registry_.getAnyType()); // مفتاح/قيمة يُحددان لاحقاً
            case Data::DataType::FUNCTION:
                return std::make_shared<FunctionType>(TypeList{}, registry_.getVoidType()); // توقيع الدالة يُحدد لاحقاً
            case Data::DataType::OBJECT:
                return registry_.getAnyType(); // يُحدد من StructRegistry عند الوصول
            case Data::DataType::ENUM:
                return registry_.getAnyType(); // يُحدد من EnumRegistry عند الوصول
            case Data::DataType::TUPLE:
                return std::make_shared<TupleType>(TypeList{}); // عناصر الصف تُحدد لاحقاً
            case Data::DataType::BYTE:
                return registry_.getIntegerType(); // byte -> int
            case Data::DataType::ERROR:
                return registry_.getUnknownType();
            case Data::DataType::UNKNOWN:
            default:
                return registry_.getUnknownType();
            }
        }

        // ============================================================================
        // التحقق من توافق الأنواع / Check Type Compatibility
        // ============================================================================

        bool TypeChecker::areTypesCompatible(TypePtr expected, TypePtr actual) const
        {
            if (!expected || !actual)
                return true; // null types = no checking

            // أي نوع = Any يتوافق مع الكل / Any type is compatible with everything
            if (expected->isAny() || actual->isAny())
                return true;

            // Unknown = لم يُحدد بعد / Unknown = not yet determined
            if (expected->isUnknown() || actual->isUnknown())
                return true;

            // نفس النوع بالضبط / Exact same type
            if (expected->getKind() == actual->getKind())
                return true;

            // التحقق من التوافق الرقمي / Numeric compatibility
            if (expected->isNumeric() && actual->isNumeric())
                return true;

            // (AR) المصفوفات متوافقة مع الأعداد (التمثيل الداخلي i64 pointer)
            //      ومتوافقة مع أي نوع مصفوفة آخر (بغض النظر عن حجم العناصر)
            // (EN) Arrays are compatible with integers (internal i64 pointer representation)
            //      and with any other array type (regardless of element count)
            if (expected->isArray() || actual->isArray())
                return true;

            // Never يتوافق مع أي نوع / Never is subtype of everything
            if (actual->isNever())
                return true;

            // استخدم isAssignableTo / Use isAssignableTo
            return actual->isAssignableTo(expected.get());
        }

        // ============================================================================
        // استنتاج نوع تعبير / Infer Expression Type
        // ============================================================================

        TypePtr TypeChecker::inferExprType(AST::Expression *expr)
        {
            if (!expr)
                return registry_.getUnknownType();

            lastInferredType_ = nullptr;
            expr->accept(*this);

            TypePtr result = lastInferredType_;
            if (!result)
            {
                // Fallback: استخدم DataType من AST / use DataType from AST
                result = dataTypeToTypePtr(expr->getDataType());
            }

            currentResult_.totalInferred++;
            return result;
        }

        // ============================================================================
        // إدارة النطاقات / Scope Management
        // ============================================================================

        void TypeChecker::enterScope()
        {
            // إنشاء بيئة فرعية مع تمرير الأب / Create child env with parent
            // ملاحظة: لا نستخدم createChild() لأنها لا تمرر الأب بشكل صحيح
            auto childEnv = std::make_shared<TypeEnvironment>(currentEnv_);
            scopeStack_.push_back(childEnv);
            currentEnv_ = childEnv;
        }

        void TypeChecker::exitScope()
        {
            if (scopeStack_.size() > 1)
            {
                scopeStack_.pop_back();
                currentEnv_ = scopeStack_.back();
            }
        }

        void TypeChecker::declareVariable(const std::string &name, TypePtr type)
        {
            currentEnv_->bind(name, type);
        }

        TypePtr TypeChecker::lookupVariable(const std::string &name) const
        {
            auto type = currentEnv_->lookup(name);
            return type ? type : nullptr;
        }

        // ============================================================================
        // مساعدات / Helpers
        // ============================================================================

        std::pair<size_t, size_t> TypeChecker::getLocation(AST::ASTNode *node) const
        {
            if (!node)
                return {0, 0};
            auto &pos = node->position;
            return {pos.line, pos.column};
        }

        void TypeChecker::recordTypeError(const std::string &varName,
                                          const std::string &expected,
                                          const std::string &actual,
                                          AST::ASTNode *node,
                                          const std::string &msg)
        {
            auto [line, col] = getLocation(node);

            TypeCheckError err;
            err.variableName = varName;
            err.expectedType = expected;
            err.actualType = actual;
            err.line = line;
            err.column = col;

            if (msg.empty())
            {
                err.message = "Type mismatch: expected '" + expected + "' but got '" + actual + "'";
                err.arabicMessage = "عدم تطابق الأنواع: متوقع '" + expected + "' لكن وُجد '" + actual + "'";
            }
            else
            {
                err.message = msg;
                err.arabicMessage = msg;
            }

            currentResult_.addError(err);
        }

        // ============================================================================
        // زيارة التعابير / Visit Expressions
        // ============================================================================

        void TypeChecker::visitLiteralExpr(AST::LiteralExpr &expr)
        {
            currentResult_.totalExpressions++;

            switch (expr.token.getType())
            {
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
                // (AR) لاشيء = nullable — متوافق مع أي نوع لاحق عند إعادة الإسناد
                // (EN) null literal = nullable — compatible with any type on reassignment
                lastInferredType_ = registry_.getUnknownType();
                break;
            default:
                lastInferredType_ = registry_.getUnknownType();
                break;
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] Literal: " << (lastInferredType_ ? lastInferredType_->toString() : "null") << "\n";
            }
        }

        void TypeChecker::visitVariableExpr(AST::VariableExpr &expr)
        {
            currentResult_.totalExpressions++;

            auto type = lookupVariable(expr.name);
            if (type)
            {
                lastInferredType_ = type;
            }
            else
            {
                // متغير غير معرّف — ليس خطأ أنواع بل خطأ دلالي
                // Undeclared variable — not a type error, semantic error
                lastInferredType_ = registry_.getUnknownType();

                if (strictMode_)
                {
                    recordTypeError(expr.name, "", "", &expr,
                                    "Undeclared variable '" + expr.name + "'");
                }
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] Variable '" << expr.name << "': "
                          << (lastInferredType_ ? lastInferredType_->toString() : "unknown") << "\n";
            }
        }

        void TypeChecker::visitBinaryExpr(AST::BinaryExpr &expr)
        {
            currentResult_.totalExpressions++;

            TypePtr leftType = inferExprType(expr.left.get());
            TypePtr rightType = inferExprType(expr.right.get());

            switch (expr.op)
            {
            // عمليات حسابية / Arithmetic
            case TT::OP_PLUS:
                // يمكن أن تكون جمع أرقام أو دمج نصوص
                if (leftType && rightType)
                {
                    if (leftType->isString() || rightType->isString())
                    {
                        lastInferredType_ = registry_.getStringType();
                    }
                    else if (leftType->isNumeric() && rightType->isNumeric())
                    {
                        // float يسود / float dominates
                        if (leftType->isFloat() || rightType->isFloat())
                        {
                            lastInferredType_ = registry_.getFloatType();
                        }
                        else
                        {
                            lastInferredType_ = registry_.getIntegerType();
                        }
                    }
                    else
                    {
                        lastInferredType_ = registry_.getUnknownType();
                        if (strictMode_)
                        {
                            recordTypeError("",
                                            "numeric or string",
                                            leftType->toString() + " + " + rightType->toString(),
                                            &expr, "Cannot add incompatible types");
                        }
                    }
                }
                else
                {
                    lastInferredType_ = registry_.getUnknownType();
                }
                break;

            case TT::OP_MINUS:
            case TT::OP_MULTIPLY:
            case TT::OP_DIVIDE:
            case TT::OP_FLOOR_DIVIDE:
            case TT::OP_MODULO:
                // عمليات حسابية تتطلب أرقام / Arithmetic requires numbers
                if (leftType && rightType)
                {
                    if (leftType->isNumeric() && rightType->isNumeric())
                    {
                        if (expr.op == TT::OP_FLOOR_DIVIDE)
                        {
                            // (AR) القسمة الصحيحة // دائماً تنتج عدد صحيح
                            lastInferredType_ = registry_.getIntegerType();
                        }
                        else if (leftType->isFloat() || rightType->isFloat() || expr.op == TT::OP_DIVIDE)
                        {
                            lastInferredType_ = registry_.getFloatType();
                        }
                        else
                        {
                            lastInferredType_ = registry_.getIntegerType();
                        }
                    }
                    else
                    {
                        lastInferredType_ = registry_.getUnknownType();
                        if (strictMode_)
                        {
                            recordTypeError("",
                                            "numeric",
                                            leftType->toString() + " op " + rightType->toString(),
                                            &expr, "Arithmetic operation requires numeric operands");
                        }
                    }
                }
                else
                {
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

        void TypeChecker::visitUnaryExpr(AST::UnaryExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr operandType = inferExprType(expr.operand.get());

            switch (expr.op)
            {
            case TT::OP_MINUS:
                if (operandType && operandType->isNumeric())
                {
                    lastInferredType_ = operandType;
                }
                else
                {
                    lastInferredType_ = registry_.getUnknownType();
                    if (strictMode_)
                    {
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

        void TypeChecker::visitTernaryExpr(AST::TernaryExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr condType = inferExprType(expr.condition.get());
            TypePtr trueType = inferExprType(expr.trueExpr.get());
            TypePtr falseType = inferExprType(expr.falseExpr.get());

            // الشرط يجب أن يكون منطقي / Condition should be boolean
            if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown())
            {
                recordTypeError("", "boolean", condType->toString(), &expr,
                                "Ternary condition should be boolean");
            }

            // إذا تطابق الفرعين / If branches match
            if (trueType && falseType && trueType->getKind() == falseType->getKind())
            {
                lastInferredType_ = trueType;
            }
            else
            {
                // Union type would be ideal, fall back to broader type
                lastInferredType_ = trueType ? trueType : registry_.getUnknownType();
            }
        }

        void TypeChecker::visitAssignExpr(AST::AssignExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr valueType = inferExprType(expr.value.get());

            auto varType = lookupVariable(expr.name);
            if (varType && valueType)
            {
                // ================================================================
                // (AR) [Fix #53] لغة ص ديناميكية — إعادة الإسناد عبر الأنواع مسموحة:
                //      متغير س = 42      →  نوع: رقم
                //      س = "نص الآن"      →  نوع: نص (مسموح — ديناميكية الأنواع)
                //      في الوضع الصارم (strictMode_) فقط نُبلّغ عن عدم التطابق.
                //      بدون هذا: المترجم يرفض إعادة الإسناد ولا يُنشئ ملف تنفيذي.
                // (EN) [Fix #53] Sad is dynamically typed — cross-type reassignment is allowed:
                //      Only report mismatch in strict mode.
                //      Without this: compiler rejects reassignment and fails to compile.
                // ================================================================
                if (strictMode_ && !areTypesCompatible(varType, valueType))
                {
                    recordTypeError(expr.name,
                                    varType->toString(), valueType->toString(),
                                    &expr);
                }

                // (AR) تحديث نوع المتغير في بيئة الأنواع ليعكس النوع الجديد
                // (EN) Update variable type in type environment to reflect new type
                if (valueType && !valueType->isUnknown())
                {
                    currentEnv_->bind(expr.name, valueType);
                }
            }

            lastInferredType_ = valueType;
        }

        void TypeChecker::visitCallExpr(AST::CallExpr &expr)
        {
            currentResult_.totalExpressions++;

            // تحقق من أنواع المعاملات / Check argument types
            for (auto &arg : expr.arguments)
            {
                if (arg)
                    inferExprType(arg.get());
            }

            // استنتج نوع الإرجاع من المُستدعى / Infer return type from callee
            TypePtr calleeType = inferExprType(expr.callee.get());

            // إذا كان CalleeType function type، استخرج نوع الإرجاع
            if (calleeType && calleeType->getKind() == SadTypeKind::Function)
            {
                auto *fnType = static_cast<FunctionType *>(calleeType.get());
                TypePtr retType = fnType->getReturnType();
                // (AR) إذا لم يُحدَّد نوع الإرجاع، نفترض رقم (مثل int في C)
                // (EN) If return type is unspecified, default to integer (like C's implicit int)
                lastInferredType_ = retType ? retType : registry_.getIntegerType();
            }
            else
            {
                lastInferredType_ = calleeType ? calleeType : registry_.getUnknownType();
            }
        }


    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
