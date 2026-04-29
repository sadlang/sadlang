/**
 * @file expression_evaluator.cpp
 * @brief (AR) تنفيذ مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Implementation
 *
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include <string>
#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h" // (AR) لضمان تعريف ASTNode الكامل عند استدعاء accept / (EN) Ensure full ASTNode definition for accept() calls
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h" // For AwaitExpr
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "exception.h"
#include "async_runtime.h" // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>

#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;

        // =========================================================================
        // (AR) تقييم الوصول للعضو / (EN) Member Access Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitMemberExpr(MemberExpr &node)
        {
            // تقييم الكائن
            node.object->accept(*this);
            Value objectValue = lastResult_;

            auto *classManager = Data::ClassManager::getInstance();

            // التحقق من الوصول الثابت: ClassName.staticField
            // Check for static access: ClassName.staticField
            if (objectValue.isString())
            {
                std::string possibleClassName = objectValue.toString();
                ClassType *classType = classManager->getClass(possibleClassName);

                if (classType)
                {
// هذا وصول ثابت: ClassName.staticField
// This is static access: ClassName.staticField
#ifdef DEBUG_OOP
                    std::cout << "[OOP] الوصول لحقل ثابت: " << possibleClassName << "." << node.member << "\n";
#endif

                    // البحث عن الحقل
                    ClassField *field = classType->findField(node.member);
                    if (!field)
                    {
                        std::string errMsg = "(AR) الحقل '" + node.member + "' غير موجود في الصنف '" + possibleClassName + "'. ";
                        errMsg += "(EN) Field '" + node.member + "' not found in class '" + possibleClassName + "'.";
                        throw RuntimeError(errMsg, node.position);
                    }

                    // التحقق من أن الحقل ثابت
                    if (!field->isStatic)
                    {
                        std::string errMsg = "(AR) لا يمكن الوصول للحقل غير الثابت '" + node.member + "' من خلال اسم الصنف. ";
                        errMsg += "(EN) Cannot access non-static field '" + node.member + "' through class name.";
                        throw RuntimeError(errMsg, node.position);
                    }

                    // فحص الوصول
                    checkMemberAccess(field->visibility, node.member, classType);

                    // الحصول على قيمة الحقل الثابت
                    Value *staticValue = classType->getStaticField(node.member);
                    if (!staticValue)
                    {
                        std::string errMsg = "(AR) الحقل الثابت '" + node.member + "' غير مهيأ. ";
                        errMsg += "(EN) Static field '" + node.member + "' not initialized.";
                        throw RuntimeError(errMsg, node.position);
                    }

                    lastResult_ = *staticValue;
#ifdef DEBUG_OOP
                    std::cout << "[OOP] قيمة الحقل الثابت: " << lastResult_.toString() << "\n";
#endif
                    return;
                }
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) وصول عادي للكائن: كائن.حقل
            //      يدعم كلاً من نوع OBJECT الحقيقي ونوع MAP القديم
            //
            // (EN) Regular object access: object.field
            //      Supports both real OBJECT type and legacy MAP type
            // ═══════════════════════════════════════════════════════════════════

            std::string className;
            ClassType *classType = nullptr;
            Value::MapType fields;

            if (objectValue.isObject())
            {
                // (AR) نوع OBJECT الحقيقي — الحصول على البيانات من ObjectInstance
                // (EN) Real OBJECT type — get data from ObjectInstance
                auto objPtr = objectValue.toObject();
                if (objPtr)
                {
                    className = objPtr->getClassName();
                    classType = classManager->getClass(className);
                    fields = objPtr->fields;
                    fields["__class__"] = Value(className);
                }
            }
            else if (objectValue.isMap())
            {
                // (AR) نوع MAP — يدعم التعدادات والخرائط العامة والكائنات القديمة
                // (EN) MAP type — supports enums, generic maps, and legacy objects
                fields = objectValue.toMap();
                auto classNameIt = fields.find("__class__");
                if (classNameIt == fields.end())
                {
                    // (AR) خريطة بدون صنف — وصول مباشر للمفتاح (تعداد أو خريطة عامة)
                    // (EN) Map without class — direct key access (enum or generic map)
                    auto it = fields.find(node.member);
                    if (it != fields.end())
                    {
                        lastResult_ = it->second;
                        return;
                    }
                    std::string errMsg = "(AR) المفتاح '" + node.member + "' غير موجود في الخريطة. ";
                    errMsg += "(EN) Key '" + node.member + "' not found in map.";
                    throw RuntimeError(errMsg, node.position);
                }
                className = classNameIt->second.toString();
                classType = classManager->getClass(className);
            }
            else if (objectValue.isTuple())
            {
                // (AR) وصول لعنصر في صف عبر فهرس رقمي: صف.0، صف.1، ...
                // (EN) Tuple element access via numeric index: tuple.0, tuple.1, ...
                const auto &tupleElements = objectValue.toTupleRef();

                // (AR) التحقق من الدوال المدمجة للصفوف
                // (EN) Check built-in tuple methods
                if (node.member == "حجم" || node.member == "طول")
                {
                    // (AR) إرجاع حجم الصف كقيمة — سيُستدعى كدالة لاحقاً إذا أُضيف ()
                    lastResult_ = Value(::Sad::Security::SafeArithmetic::assertSafeCast<int>(tupleElements.size(), "expression_evaluator_members_size"));
                    return;
                }

                // (AR) محاولة تحليل اسم العضو كفهرس رقمي
                // (EN) Try parsing member name as numeric index
                try
                {
                    size_t idx = std::stoul(node.member);
                    if (idx >= tupleElements.size())
                    {
                        std::string errMsg = "(AR) فهرس الصف خارج النطاق: " + std::to_string(idx) +
                                             " (الحجم: " + std::to_string(tupleElements.size()) + "). ";
                        errMsg += "(EN) Tuple index out of range: " + std::to_string(idx) +
                                  " (size: " + std::to_string(tupleElements.size()) + ").";
                        throw RuntimeError(errMsg, node.position);
                    }
                    lastResult_ = tupleElements[idx];
                    return;
                }
                catch (const std::invalid_argument &)
                {
                    std::string errMsg = "(AR) العضو '" + node.member + "' غير صالح للصف. استخدم فهرس رقمي مثل .0 أو .1. ";
                    errMsg += "(EN) Member '" + node.member + "' is not valid for tuple. Use numeric index like .0 or .1.";
                    throw RuntimeError(errMsg, node.position);
                }
                catch (const std::out_of_range &)
                {
                    std::string errMsg = "(AR) فهرس الصف كبير جداً: " + node.member + ". ";
                    errMsg += "(EN) Tuple index too large: " + node.member + ".";
                    throw RuntimeError(errMsg, node.position);
                }
            }
            else
            {
                std::string errMsg = "(AR) لا يمكن الوصول لعضو من قيمة ليست كائن. ";
                errMsg += "(EN) Cannot access member of non-object value.";
                throw RuntimeError(errMsg, node.position);
            }

            if (!classType)
            {
                // (AR) دعم WidgetBuilder: قراءة الحقول الديناميكية مباشرة
                if (objectValue.isObject())
                {
                    auto objPtr = objectValue.toObject();
                    if (objPtr)
                    {
                        auto it = objPtr->fields.find(node.member);
                        if (it != objPtr->fields.end())
                        {
                            lastResult_ = it->second;
                            return;
                        }
                    }
                }
                throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.", node.position);
            }

            // البحث عن الحقل في السلسلة الهرمية
            ClassField *field = classType->findField(node.member);

            // البحث عن خاصية (Property) إذا لم يُوجد حقل
            ClassProperty *property = nullptr;
            if (!field)
            {
                property = classType->findProperty(node.member);
            }

            if (!field && !property)
            {
                // (AR) لم يُعثر على حقل أو خاصية معلنة — جرب قراءة حقل ديناميكي
                // (EN) No declared field or property — try reading dynamic field
                auto it = fields.find(node.member);
                if (it != fields.end())
                {
                    lastResult_ = it->second;
                    return;
                }
                std::string errMsg = "(AR) الحقل أو الخاصية '" + node.member + "' غير موجود في الكائن. ";
                errMsg += "(EN) Field or property '" + node.member + "' not found in object.";
                throw RuntimeError(errMsg, node.position);
            }

            // إذا كانت خاصية، نفذ الـ getter
            if (property)
            {
#ifdef DEBUG_OOP
                std::cout << "[OOP] تنفيذ getter للخاصية: " << node.member << "\n";
#endif

                // فحص الوصول
                checkMemberAccess(property->visibility, node.member, classType);

                // التحقق من وجود getter
                if (!property->getterBody)
                {
                    std::string errMsg = "(AR) الخاصية '" + node.member + "' للكتابة فقط (لا يوجد getter). ";
                    errMsg += "(EN) Property '" + node.member + "' is write-only (no getter).";
                    throw RuntimeError(errMsg, node.position);
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) تنفيذ getter body في نطاق جديد مع سياق الكائن
                //      نربط 'هذا' بالكائن ونضيف الحقول للنطاق
                //      حتى يستطيع الـ getter الوصول لبيانات الكائن
                //
                // (EN) Execute getter body in new scope with object context
                //      Bind 'هذا' (this) to object and add fields to scope
                //      so getter can access object data
                // ═══════════════════════════════════════════════════════════════
                variableManager_.enterScope(Data::ScopeType::FUNCTION, "get_" + node.member);

                // (AR) ربط 'هذا' بالكائن / (EN) Bind 'this' to object
                variableManager_.define("هذا", objectValue);
                variableManager_.define("this", objectValue);

                // (AR) ربط حقول الكائن كمتغيرات محلية / (EN) Bind object fields as local variables
                for (const auto &[fname, fval] : fields)
                {
                    if (fname != "__class__")
                    {
                        variableManager_.define(fname, fval);
                    }
                }

                try
                {
                    // (AR) تنفيذ getter body عبر StatementExecutor (لأن الجسم هو BlockStmt)
                    // (EN) Execute getter body via StatementExecutor (because body is BlockStmt)
                    property->getterBody->accept(statementExecutor_);

                    if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                    {
                        lastResult_ = statementExecutor_.getReturnValue();
                        statementExecutor_.resetFlowControl();
                    }
                }
                catch (...)
                {
                    variableManager_.exitScope();
                    throw;
                }

                variableManager_.exitScope();

#ifdef DEBUG_OOP

                std::cout << "[OOP] قيمة الخاصية: " << lastResult_.toString() << "\n";
#endif
                return;
            }

            // معالجة الحقل العادي
            // فحص الوصول (Phase 6.1: Access Modifiers)
            checkMemberAccess(field->visibility, node.member, classType);

            // البحث عن قيمة الحقل
            auto it = fields.find(node.member);
            if (it == fields.end())
            {
                std::string errMsg = "(AR) الحقل '" + node.member + "' غير موجود في الكائن. ";
                errMsg += "(EN) Field '" + node.member + "' not found in object.";
                throw RuntimeError(errMsg, node.position);
            }

            // إرجاع قيمة الحقل
            lastResult_ = it->second;
#ifdef DEBUG_OOP
            std::cout << "[OOP] قيمة الحقل: " << lastResult_.toString() << "\n";
#endif
        }

        // =========================================================================
        // (AR) الوصول لعضو — عقدة MemberAccessExpr من class_nodes.h
        // (EN) Member access — MemberAccessExpr node from class_nodes.h
        // =========================================================================

        void ExpressionEvaluator::visitMemberAccessExpr(MemberAccessExpr &node)
        {
            // (AR) تقييم الكائن / (EN) Evaluate the object
            node.object->accept(*this);
            Value objectValue = lastResult_;

            auto *classManager = Data::ClassManager::getInstance();

            // (AR) التحقق من الوصول الثابت: ClassName.staticField
            // (EN) Check for static access: ClassName.staticField
            if (objectValue.isString())
            {
                std::string possibleClassName = objectValue.toString();
                ClassType *classType = classManager->getClass(possibleClassName);

                if (classType)
                {
#ifdef DEBUG_OOP
                    std::cout << "[OOP] الوصول لحقل ثابت: " << possibleClassName << "." << node.memberName << "\n";
#endif

                    ClassField *field = classType->findField(node.memberName);
                    if (!field)
                    {
                        std::string errMsg = "(AR) الحقل '" + node.memberName + "' غير موجود في الصنف '" + possibleClassName + "'. ";
                        errMsg += "(EN) Field '" + node.memberName + "' not found in class '" + possibleClassName + "'.";
                        throw RuntimeError(errMsg, node.position);
                    }

                    if (!field->isStatic)
                    {
                        std::string errMsg = "(AR) لا يمكن الوصول للحقل غير الثابت '" + node.memberName + "' من خلال اسم الصنف. ";
                        errMsg += "(EN) Cannot access non-static field '" + node.memberName + "' through class name.";
                        throw RuntimeError(errMsg, node.position);
                    }

                    checkMemberAccess(field->visibility, node.memberName, classType);

                    Value *staticValue = classType->getStaticField(node.memberName);
                    if (!staticValue)
                    {
                        std::string errMsg = "(AR) الحقل الثابت '" + node.memberName + "' غير مهيأ. ";
                        errMsg += "(EN) Static field '" + node.memberName + "' not initialized.";
                        throw RuntimeError(errMsg, node.position);
                    }

                    lastResult_ = *staticValue;
                    return;
                }
            }

            // (AR) وصول عادي للكائن / (EN) Regular object access
            std::string className;
            ClassType *classType = nullptr;
            Value::MapType fields;

            if (objectValue.isObject())
            {
                auto objPtr = objectValue.toObject();
                if (objPtr)
                {
                    className = objPtr->getClassName();
                    classType = classManager->getClass(className);
                    fields = objPtr->fields;
                    fields["__class__"] = Value(className);
                }
            }
            else if (objectValue.isMap())
            {
                fields = objectValue.toMap();
                auto classNameIt = fields.find("__class__");
                if (classNameIt == fields.end())
                {
                    auto it = fields.find(node.memberName);
                    if (it != fields.end())
                    {
                        lastResult_ = it->second;
                        return;
                    }
                    std::string errMsg = "(AR) المفتاح '" + node.memberName + "' غير موجود في الخريطة. ";
                    errMsg += "(EN) Key '" + node.memberName + "' not found in map.";
                    throw RuntimeError(errMsg, node.position);
                }
                className = classNameIt->second.toString();
                classType = classManager->getClass(className);
            }
            else if (objectValue.isTuple())
            {
                // (AR) وصول لعنصر في صف عبر فهرس رقمي: صف.0، صف.1، ...
                // (EN) Tuple element access via numeric index: tuple.0, tuple.1, ...
                const auto &tupleElements = objectValue.toTupleRef();

                // (AR) التحقق من الدوال المدمجة للصفوف
                // (EN) Check built-in tuple methods
                if (node.memberName == "حجم" || node.memberName == "طول")
                {
                    lastResult_ = Value(::Sad::Security::SafeArithmetic::assertSafeCast<int>(tupleElements.size(), "expression_evaluator_members_size"));
                    return;
                }

                // (AR) محاولة تحليل اسم العضو كفهرس رقمي
                // (EN) Try parsing member name as numeric index
                try
                {
                    size_t idx = std::stoul(node.memberName);
                    if (idx >= tupleElements.size())
                    {
                        std::string errMsg = "(AR) فهرس الصف خارج النطاق: " + std::to_string(idx) +
                                             " (الحجم: " + std::to_string(tupleElements.size()) + "). ";
                        errMsg += "(EN) Tuple index out of range: " + std::to_string(idx) +
                                  " (size: " + std::to_string(tupleElements.size()) + ").";
                        throw RuntimeError(errMsg, node.position);
                    }
                    lastResult_ = tupleElements[idx];
                    return;
                }
                catch (const std::invalid_argument &)
                {
                    std::string errMsg = "(AR) العضو '" + node.memberName + "' غير صالح للصف. استخدم فهرس رقمي مثل .0 أو .1. ";
                    errMsg += "(EN) Member '" + node.memberName + "' is not valid for tuple. Use numeric index like .0 or .1.";
                    throw RuntimeError(errMsg, node.position);
                }
                catch (const std::out_of_range &)
                {
                    std::string errMsg = "(AR) فهرس الصف كبير جداً: " + node.memberName + ". ";
                    errMsg += "(EN) Tuple index too large: " + node.memberName + ".";
                    throw RuntimeError(errMsg, node.position);
                }
            }
            else
            {
                std::string errMsg = "(AR) لا يمكن الوصول لعضو من قيمة ليست كائن. ";
                errMsg += "(EN) Cannot access member of non-object value.";
                throw RuntimeError(errMsg, node.position);
            }

            if (!classType)
            {
                // (AR) دعم WidgetBuilder: قراءة الحقول الديناميكية مباشرة
                if (objectValue.isObject())
                {
                    auto objPtr = objectValue.toObject();
                    if (objPtr)
                    {
                        auto it = objPtr->fields.find(node.memberName);
                        if (it != objPtr->fields.end())
                        {
                            lastResult_ = it->second;
                            return;
                        }
                    }
                }
                throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.", node.position);
            }

            // (AR) البحث عن الحقل في السلسلة الهرمية / (EN) Search field in hierarchy
            ClassField *field = classType->findField(node.memberName);

            // (AR) البحث عن خاصية (Property) / (EN) Look for property
            ClassProperty *property = nullptr;
            if (!field)
            {
                property = classType->findProperty(node.memberName);
            }

            if (!field && !property)
            {
                auto it = fields.find(node.memberName);
                if (it != fields.end())
                {
                    lastResult_ = it->second;
                    return;
                }
                std::string errMsg = "(AR) الحقل أو الخاصية '" + node.memberName + "' غير موجود في الكائن. ";
                errMsg += "(EN) Field or property '" + node.memberName + "' not found in object.";
                throw RuntimeError(errMsg, node.position);
            }

            // (AR) إذا كانت خاصية، نفذ الـ getter / (EN) If property, execute getter
            if (property)
            {
                checkMemberAccess(property->visibility, node.memberName, classType);

                if (!property->getterBody)
                {
                    std::string errMsg = "(AR) الخاصية '" + node.memberName + "' للكتابة فقط (لا يوجد getter). ";
                    errMsg += "(EN) Property '" + node.memberName + "' is write-only (no getter).";
                    throw RuntimeError(errMsg, node.position);
                }

                variableManager_.enterScope(Data::ScopeType::FUNCTION, "get_" + node.memberName);
                variableManager_.define("هذا", objectValue);
                variableManager_.define("this", objectValue);

                for (const auto &[fname, fval] : fields)
                {
                    if (fname != "__class__")
                    {
                        variableManager_.define(fname, fval);
                    }
                }

                try
                {
                    property->getterBody->accept(statementExecutor_);

                    if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                    {
                        lastResult_ = statementExecutor_.getReturnValue();
                        statementExecutor_.resetFlowControl();
                    }
                }
                catch (...)
                {
                    variableManager_.exitScope();
                    throw;
                }

                variableManager_.exitScope();
                return;
            }

            // (AR) معالجة الحقل العادي / (EN) Handle regular field
            checkMemberAccess(field->visibility, node.memberName, classType);

            auto it = fields.find(node.memberName);
            if (it == fields.end())
            {
                std::string errMsg = "(AR) الحقل '" + node.memberName + "' غير موجود في الكائن. ";
                errMsg += "(EN) Field '" + node.memberName + "' not found in object.";
                throw RuntimeError(errMsg, node.position);
            }

            lastResult_ = it->second;
        }

        // =========================================================================
        // (AR) كتابة القيمة رجوعاً عبر سلسلة التعبيرات المتداخلة
        // (EN) Recursive writeback for deep nested assignment chains
        // a.b.c = val → read a.b, set c in it, write back a.b, then a
        // arr[0][1] = val → read arr[0], set [1] in it, write back arr[0], then arr
        // =========================================================================
        void ExpressionEvaluator::writeBackChain(AST::Expression *expr, const Data::Value &value)
        {
            if (!expr)
                return;

            // (AR) الحالة الأساسية: الوصول لمتغير جذري ← نعيّنه مباشرة
            // (EN) Base case: root variable → assign directly
            if (auto *varExpr = dynamic_cast<VariableExpr *>(expr))
            {
                variableManager_.assign(varExpr->name, value);
                return;
            }

            // (AR) الحالة 2: parent.member ← نقرأ الأصل، نحدّث الحقل، ثم نكتب رجوعاً
            // (EN) Case 2: parent.member → read parent, update field, write back recursively
            if (auto *memberExpr = dynamic_cast<MemberExpr *>(expr))
            {
                memberExpr->object->accept(*this);
                Value parentValue = lastResult_;

                if (parentValue.isMap())
                {
                    // (AR) تعديل مباشر عبر shared_ptr — بدون نسخ!
                    // (EN) Direct mutation via shared_ptr — no copying!
                    parentValue.toMapMut()[memberExpr->member] = value;
                }
                else if (parentValue.isObject())
                {
                    auto parentObj = parentValue.toObject();
                    if (parentObj)
                    {
                        parentObj->setField(memberExpr->member, value);
                        // (AR) مزامنة النطاق: تحديث متغير الـ scope أيضاً لمنع
                        //      الكتابة الراجعة في نهاية الباني من إعادة القيم القديمة
                        // (EN) Sync scope: also update scope variable to prevent
                        //      constructor writeback from restoring old values
                        try
                        {
                            variableManager_.assign(memberExpr->member, value);
                        }
                        catch (...)
                        {
                            // (AR) المتغير قد لا يكون في النطاق — مقبول
                            // (EN) Variable might not be in scope — acceptable
                        }
                    }
                }
                return;
            }

            // (AR) الحالة 3: container[index] ← تعديل مباشر عبر shared_ptr
            // (EN) Case 3: container[index] → direct mutation via shared_ptr (O(1) per level)
            if (auto *indexExpr = dynamic_cast<IndexExpr *>(expr))
            {
                indexExpr->object->accept(*this);
                Value containerValue = lastResult_;

                indexExpr->index->accept(*this);
                Value idxValue = lastResult_;

                if (containerValue.isArray())
                {
                    // (AR) تعديل مباشر — المصفوفة مخزّنة كـ shared_ptr
                    // (EN) Direct mutation — array is stored as shared_ptr
                    auto &arr = containerValue.toArrayMut();
                    int idx = idxValue.toInt();
                    if (idx < 0)
                        idx = ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_members_size") + idx;
                    if (idx >= 0 && idx < ::Sad::Security::SafeArithmetic::assertSafeCast<int>(arr.size(), "expression_evaluator_members_size"))
                    {
                        arr[idx] = value;
                    }
                }
                else if (containerValue.isMap())
                {
                    // (AR) تعديل مباشر على الخريطة
                    containerValue.toMapMut()[idxValue.toString()] = value;
                }
                return;
            }
        }

        // =========================================================================
        // (AR) تعيين قيمة لعضو / (EN) Member Assignment
        // =========================================================================


    } // namespace Interpreter
} // namespace Sad

