// ============================================================================
// type_checker_expr_visitors.cpp - زوار التعبيرات المتقدمة
// ============================================================================
// (AR) زوار التعبيرات: IndexExpr, MemberExpr, ArrayExpr, MapExpr, NewExpr...
// (EN) Expression visitors: IndexExpr, MemberExpr, ArrayExpr, MapExpr, NewExpr...
// تم استخراج هذا الملف من type_checker.cpp وفقاً لقاعدة CW-05
// ============================================================================
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

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

        void TypeChecker::visitIndexExpr(AST::IndexExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr objType = inferExprType(expr.object.get());
            TypePtr idxType = inferExprType(expr.index.get());

            if (objType && objType->isArray())
            {
                // Array[int] -> element type
                if (strictMode_ && idxType && !idxType->isInteger() && !idxType->isUnknown())
                {
                    recordTypeError("", "integer", idxType->toString(), &expr,
                                    "Array index must be integer");
                }
                // (AR) استرجاع نوع العنصر من نوع المصفوفة / (EN) Retrieve element type from array type
                if (objType->getKind() == SadTypeKind::Array)
                {
                    auto *arrType = static_cast<ArrayType *>(objType.get());
                    TypePtr elemType = arrType->getElementType();
                    lastInferredType_ = elemType ? elemType : registry_.getUnknownType();
                    return;
                }
            }
            // (AR) إذا كان كائن به عملية فهرسة / (EN) Object with subscript operation
            if (objType && (objType->getKind() == SadTypeKind::Map || objType->isString()))
            {
                lastInferredType_ = objType->isString() ? registry_.getStringType() : registry_.getAnyType();
                return;
            }
            lastInferredType_ = registry_.getUnknownType();
        }

        void TypeChecker::visitMemberExpr(AST::MemberExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr objType = inferExprType(expr.object.get());

            // (AR) البحث عن نوع العضو من StructRegistry / (EN) Look up member type from StructRegistry
            if (objType && (objType->getKind() == SadTypeKind::Class))
            {
                std::string className;
                if (auto *newExpr = dynamic_cast<AST::NewExpr *>(expr.object.get()))
                {
                    className = newExpr->className;
                }
                if (!className.empty())
                {
                    auto structType = StructRegistry::instance().findStruct(className);
                    if (structType)
                    {
                        auto field = structType->findField(expr.member);
                        if (field)
                        {
                            lastInferredType_ = field->getType() ? field->getType() : registry_.getUnknownType();
                            return;
                        }
                    }
                }
            }
            // (AR) للنصوص: خصائص مثل الطول / (EN) For strings: properties like length
            if (objType && objType->isString())
            {
                if (expr.member == "الطول" || expr.member == "length")
                {
                    lastInferredType_ = registry_.getIntegerType();
                    return;
                }
            }
            lastInferredType_ = registry_.getUnknownType();
        }

        void TypeChecker::visitMemberAssignExpr(AST::MemberAssignExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr objType = inferExprType(expr.object.get());
            TypePtr valType = inferExprType(expr.value.get());

            // (AR) تحقق من توافق نوع العضو مع القيمة المُسندة / (EN) Check member type compatibility with assigned value
            if (strictMode_ && objType && (objType->getKind() == SadTypeKind::Class))
            {
                std::string className;
                if (auto *newExpr = dynamic_cast<AST::NewExpr *>(expr.object.get()))
                {
                    className = newExpr->className;
                }
                if (!className.empty())
                {
                    auto structType = StructRegistry::instance().findStruct(className);
                    if (structType)
                    {
                        auto field = structType->findField(expr.member);
                        if (field && field->getType() && valType && !valType->isUnknown())
                        {
                            TypePtr fieldType = field->getType();
                            if (!fieldType->isUnknown() && !areTypesCompatible(fieldType, valType))
                            {
                                recordTypeError(expr.member, fieldType->toString(), valType->toString(), &expr,
                                                "Member assignment type mismatch");
                            }
                        }
                    }
                }
            }
            lastInferredType_ = valType;
        }

        void TypeChecker::visitArrayExpr(AST::ArrayExpr &expr)
        {
            currentResult_.totalExpressions++;

            TypePtr elementType = nullptr;
            for (auto &elem : expr.elements)
            {
                TypePtr et = inferExprType(elem.get());
                if (!elementType)
                {
                    elementType = et;
                }
                else if (et && elementType && et->getKind() != elementType->getKind())
                {
                    // مصفوفة مختلطة / Mixed array
                    if (!et->isUnknown() && !elementType->isUnknown())
                    {
                        currentResult_.addWarning(
                            "Mixed array element types: " + elementType->toString() + " and " + et->toString());
                    }
                }
            }

            // (AR) إنشاء نوع المصفوفة الفعلي مع نوع العنصر
            // (EN) Create actual ArrayType with element type
            if (elementType)
            {
                lastInferredType_ = std::make_shared<ArrayType>(
                    elementType,
                    expr.elements.empty() ? std::nullopt : std::optional<size_t>(expr.elements.size()));
            }
            else
            {
                lastInferredType_ = std::make_shared<ArrayType>(registry_.getAnyType());
            }
        }

        void TypeChecker::visitMapExpr(AST::MapExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr keyType = nullptr;
            TypePtr valType = nullptr;
            for (auto &pair : expr.pairs)
            {
                TypePtr kt = inferExprType(pair.key.get());
                TypePtr vt = inferExprType(pair.value.get());
                if (!keyType)
                    keyType = kt;
                if (!valType)
                    valType = vt;
            }
            // (AR) إنشاء نوع القاموس مع نوع المفتاح والقيمة
            lastInferredType_ = std::make_shared<DictionaryType>(
                keyType ? keyType : registry_.getAnyType(),
                valType ? valType : registry_.getAnyType());
        }

        void TypeChecker::visitWalrusExpr(AST::WalrusExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr valType = inferExprType(expr.value.get());
            declareVariable(expr.variable, valType);
            lastInferredType_ = valType;
        }

        void TypeChecker::visitLambdaExpr(AST::LambdaExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();

            // Register parameters and collect param types
            TypeList paramTypes;
            for (auto &param : expr.parameters)
            {
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

        void TypeChecker::visitListComprehensionExpr(AST::ListComprehensionExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();
            TypePtr iterType = inferExprType(expr.iterable.get());
            declareVariable(expr.variable, registry_.getUnknownType());
            TypePtr elemType = inferExprType(expr.element.get());
            if (expr.condition)
                inferExprType(expr.condition.get());
            exitScope();

            // (AR) نوع المصفوفة الناتجة من الاستيعاب
            lastInferredType_ = std::make_shared<ArrayType>(elemType ? elemType : registry_.getAnyType());
        }

        void TypeChecker::visitDictComprehensionExpr(AST::DictComprehensionExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();
            inferExprType(expr.iterable.get());
            declareVariable(expr.variable, registry_.getUnknownType());
            TypePtr keyType = inferExprType(expr.key.get());
            TypePtr valType = inferExprType(expr.value.get());
            if (expr.condition)
                inferExprType(expr.condition.get());
            exitScope();

            // (AR) نوع القاموس الناتج من الاستيعاب
            lastInferredType_ = std::make_shared<DictionaryType>(
                keyType ? keyType : registry_.getAnyType(),
                valType ? valType : registry_.getAnyType());
        }

        void TypeChecker::visitSetComprehensionExpr(AST::SetComprehensionExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();
            inferExprType(expr.iterable.get());
            declareVariable(expr.variable, registry_.getUnknownType());
            TypePtr setElemType = inferExprType(expr.expression.get());
            if (expr.condition)
                inferExprType(expr.condition.get());
            exitScope();

            // (AR) نوع المجموعة الناتجة من الاستيعاب
            lastInferredType_ = std::make_shared<ArrayType>(setElemType ? setElemType : registry_.getAnyType());
        }

        void TypeChecker::visitGeneratorExpr(AST::GeneratorExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();
            inferExprType(expr.iterable.get());
            declareVariable(expr.variable, registry_.getUnknownType());
            inferExprType(expr.element.get());
            if (expr.condition)
                inferExprType(expr.condition.get());
            exitScope();

            lastInferredType_ = registry_.getAnyType();
        }

        void TypeChecker::visitDecoratorExpr(AST::DecoratorExpr &expr)
        {
            currentResult_.totalExpressions++;
            for (auto &arg : expr.arguments)
            {
                if (arg)
                    inferExprType(arg.get());
            }
            lastInferredType_ = registry_.getUnknownType();
        }

        // ============================================================================
        // OOP Expressions / تعابير الكائنات
        // ============================================================================

        void TypeChecker::visitNewExpr(AST::NewExpr &expr)
        {
            currentResult_.totalExpressions++;
            for (auto &arg : expr.arguments)
            {
                if (arg)
                    inferExprType(arg.get());
            }
            // (AR) البحث عن نوع الصنف في StructRegistry
            auto structType = StructRegistry::instance().findStruct(expr.className);
            if (structType)
            {
                // (AR) الصنف موجود — إرجاع نوع Class
                lastInferredType_ = registry_.internPrimitiveType(SadTypeKind::Class);
            }
            else
            {
                // (AR) صنف غير معروف — تحذير
                lastInferredType_ = registry_.getUnknownType();
            }
        }

        void TypeChecker::visitMemberAccessExpr(AST::MemberAccessExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr objType = inferExprType(expr.object.get());

            // (AR) البحث عن نوع الحقل من StructRegistry / (EN) Look up field type from StructRegistry
            if (objType && (objType->getKind() == SadTypeKind::Class))
            {
                // (AR) محاولة تحديد اسم الصنف / (EN) Try to identify class name
                std::string className;
                if (auto *newExpr = dynamic_cast<AST::NewExpr *>(expr.object.get()))
                {
                    className = newExpr->className;
                }
                else if (auto *thisExpr = dynamic_cast<AST::ThisExpr *>(expr.object.get()))
                {
                    (void)thisExpr;
                    className = currentFunction_;
                }
                if (!className.empty())
                {
                    auto structType = StructRegistry::instance().findStruct(className);
                    if (structType)
                    {
                        auto field = structType->findField(expr.memberName);
                        if (field && field->getType())
                        {
                            lastInferredType_ = field->getType();
                            return;
                        }
                    }
                }
            }
            // (AR) خصائص النص / (EN) String properties
            if (objType && objType->isString())
            {
                if (expr.memberName == "الطول" || expr.memberName == "length")
                {
                    lastInferredType_ = registry_.getIntegerType();
                    return;
                }
            }
            lastInferredType_ = registry_.getUnknownType();
        }

        void TypeChecker::visitMethodCallExpr(AST::MethodCallExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr objType = inferExprType(expr.object.get());
            for (auto &arg : expr.arguments)
            {
                if (arg)
                    inferExprType(arg.get());
            }
            // (AR) استنتاج نوع الإرجاع من الطريقة / (EN) Infer return type from method
            // (AR) طرق النص تُرجع أنواعاً معروفة / (EN) String methods return known types
            if (objType && objType->isString())
            {
                const std::string &method = expr.methodName;
                if (method == "الطول" || method == "length")
                {
                    lastInferredType_ = registry_.getIntegerType();
                    return;
                }
                if (method == "يحتوي" || method == "contains" ||
                    method == "يبدأ_بـ" || method == "startsWith" ||
                    method == "ينتهي_بـ" || method == "endsWith")
                {
                    lastInferredType_ = registry_.getBooleanType();
                    return;
                }
                if (method == "قطع" || method == "slice" ||
                    method == "استبدل" || method == "replace" ||
                    method == "حروف_كبيرة" || method == "toUpperCase" ||
                    method == "حروف_صغيرة" || method == "toLowerCase")
                {
                    lastInferredType_ = registry_.getStringType();
                    return;
                }
            }
            // (AR) طرق المصفوفة / (EN) Array methods
            if (objType && objType->isArray())
            {
                const std::string &method = expr.methodName;
                if (method == "الطول" || method == "length" || method == "حجم" || method == "size")
                {
                    lastInferredType_ = registry_.getIntegerType();
                    return;
                }
                if (method == "أضف" || method == "push" || method == "ادفع")
                {
                    lastInferredType_ = registry_.getVoidType();
                    return;
                }
            }
            lastInferredType_ = registry_.getUnknownType();
        }

        void TypeChecker::visitThisExpr(AST::ThisExpr &expr)
        {
            currentResult_.totalExpressions++;
            // (AR) البحث عن نوع الصنف الحالي
            if (!currentFunction_.empty())
            {
                auto classType = StructRegistry::instance().findStruct(currentFunction_);
                if (classType)
                {
                    lastInferredType_ = registry_.internPrimitiveType(SadTypeKind::Class);
                    return;
                }
            }
            lastInferredType_ = registry_.getUnknownType();
        }

        void TypeChecker::visitSuperExpr(AST::SuperExpr &expr)
        {
            currentResult_.totalExpressions++;
            // (AR) super يشير للصنف الأب — يعتمد على السياق
            lastInferredType_ = registry_.getUnknownType();
        }

        void TypeChecker::visitBorrowExpr(AST::BorrowExpr &expr)
        {
            currentResult_.totalExpressions++;
            auto type = lookupVariable(expr.variableName);
            lastInferredType_ = type ? type : registry_.getUnknownType();
        }

        void TypeChecker::visitInlineAsmExpr(AST::InlineAsmExpr &expr)
        {
            currentResult_.totalExpressions++;
            lastInferredType_ = registry_.getUnknownType();
        }

        void TypeChecker::visitRangeExpr(AST::RangeExpr &expr)
        {
            // (AR) تعبير النطاق 1..10 — ينتج مصفوفة أرقام
            // (EN) Range expression 1..10 — produces an array of integers
            currentResult_.totalExpressions++;
            if (expr.start)
                inferExprType(expr.start.get());
            if (expr.end)
                inferExprType(expr.end.get());
            lastInferredType_ = registry_.getUnknownType();
        }

        void TypeChecker::visitOptionalChainExpr(AST::OptionalChainExpr &expr)
        {
            // (AR) وصول آمن ?. — نوع النتيجة هو نوع العضو أو لاشيء
            // (EN) Optional chaining ?. — result is member type or null
            currentResult_.totalExpressions++;
            if (expr.object)
            {
                TypePtr objType = inferExprType(expr.object.get());
                // (AR) البحث عن نوع العضو إذا كان الكائن بنية/صنف
                // (EN) Look up member type if object is a struct/class
                if (objType && objType->getKind() == SadTypeKind::Class)
                {
                    if (auto *newExpr = dynamic_cast<AST::NewExpr *>(expr.object.get()))
                    {
                        auto structType = StructRegistry::instance().findStruct(newExpr->className);
                        if (structType)
                        {
                            auto field = structType->findField(expr.member);
                            if (field && field->getType())
                            {
                                lastInferredType_ = field->getType();
                                return;
                            }
                        }
                    }
                }
            }
            // (AR) لا نعرف النوع — يمكن أن يكون لاشيء
            // (EN) Unknown type — could be null
            lastInferredType_ = registry_.getUnknownType();
        }

        void TypeChecker::visitNullCoalesceExpr(AST::NullCoalesceExpr &expr)
        {
            // (AR) تجميع فارغ ?? — نوع النتيجة هو نوع الأيمن (البديل)
            // (EN) Null coalescing ?? — result type is the right (fallback) type
            currentResult_.totalExpressions++;
            TypePtr leftType = nullptr;
            TypePtr rightType = nullptr;

            if (expr.left)
            {
                leftType = inferExprType(expr.left.get());
            }
            if (expr.right)
            {
                rightType = inferExprType(expr.right.get());
            }

            // (AR) إذا كان النوعان معروفين ومتوافقين، استخدم الأيمن
            // (EN) If both types known and compatible, use right type
            if (rightType)
            {
                lastInferredType_ = rightType;
            }
            else if (leftType)
            {
                lastInferredType_ = leftType;
            }
            else
            {
                lastInferredType_ = registry_.getUnknownType();
            }
        }

        // ============================================================================
        // زيارة العبارات / Visit Statements
        // ============================================================================


    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
