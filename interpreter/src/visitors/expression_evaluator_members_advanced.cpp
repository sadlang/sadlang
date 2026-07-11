/**
 * @file expression_evaluator_members_advanced.cpp
 * @brief (AR) تنفيذ التعابير المتقدمة (comprehension/await/template/tuple) في ExpressionEvaluator
 * @brief (EN) Advanced expressions (comprehension/await/template/tuple) in ExpressionEvaluator
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
#include "runtime_throw.h"
#include "user_thrown.h"
#include "runtime_throw.h"
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

        void ExpressionEvaluator::visitListComprehensionExpr(ListComprehensionExpr &node)
        {
            // (AR) تقييم iterable / (EN) Evaluate iterable
            node.iterable->accept(*this);
            Value iterableValue = lastResult_;

            // (AR) يقبل الاستيعاب مصفوفةً أو خريطةً. على الخريطة يُربَط متغيّر الحلقة بالمفتاح،
            //      ومتغيّر القيمة (إن وُجد فكّ زوج «لكل مفتاح، قيمة») بالقيمة — يطابق حلقة «لكل».
            // (EN) A comprehension accepts an array or a map. Over a map the loop variable binds to
            //      the key and the value variable (if pair-unpacking «for key, value») to the value —
            //      mirroring the «for» loop.
            const bool isMapIter = iterableValue.isMap();
            if (!iterableValue.isArray() && !isMapIter)
            {
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"kind", "قائمي/list"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_COMPREHENSION_NEEDS_ARRAY, Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                };
                lastResult_ = Value(std::vector<Value>());
                return;
            }

            // (AR) إنشاء مصفوفة النتيجة / (EN) Create result array
            std::vector<Value> result;

            // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
            variableManager_.enterScope(Data::ScopeType::BLOCK, "list_comprehension");

            if (isMapIter)
            {
                // (AR) تكرار مدخلات الخريطة (المفتاح[، القيمة]) / (EN) Iterate map entries (key[, value])
                const auto &map = iterableValue.toMapRef();
                bool firstEntry = true;
                for (const auto &[key, val] : map)
                {
                    Value keyValue(key);
                    if (firstEntry)
                    {
                        variableManager_.define(node.variable, keyValue);
                        if (!node.valueVariable.empty())
                            variableManager_.define(node.valueVariable, val);
                        firstEntry = false;
                    }
                    else
                    {
                        variableManager_.assign(node.variable, keyValue);
                        if (!node.valueVariable.empty())
                            variableManager_.assign(node.valueVariable, val);
                    }

                    bool includeItem = true;
                    if (node.condition)
                    {
                        node.condition->accept(*this);
                        includeItem = lastResult_.toBool();
                    }
                    if (includeItem)
                    {
                        node.element->accept(*this);
                        result.push_back(lastResult_);
                    }
                }
            }
            else
            {
                // (AR) المرور على كل عنصر / (EN) Iterate over each element
                for (size_t i = 0; i < iterableValue.size(); ++i)
                {
                    Value item = iterableValue[i];

                    // (AR) تحديث متغير الحلقة / (EN) Update loop variable
                    if (i == 0)
                    {
                        // أول iteration - نعرّف المتغير
                        variableManager_.define(node.variable, item);
                    }
                    else
                    {
                        // iterations تالية - نحدّث باستخدام assign
                        variableManager_.assign(node.variable, item);
                    }

                    // (AR) التحقق من الشرط إن وُجد / (EN) Check condition if exists
                    bool includeItem = true;
                    if (node.condition)
                    {
                        node.condition->accept(*this);
                        includeItem = lastResult_.toBool();
                    }

                    // (AR) تقييم التعبير وإضافة النتيجة / (EN) Evaluate element expression and add result
                    if (includeItem)
                    {
                        node.element->accept(*this);
                        result.push_back(lastResult_);
                    }
                }
            }

            // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
            variableManager_.exitScope();

            // (AR) إرجاع المصفوفة الناتجة / (EN) Return result array
            lastResult_ = Value(result);
        }

        // =========================================================================
        // (AR) تقييم استيعاب قاموسي / (EN) Dict Comprehension Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitDictComprehensionExpr(DictComprehensionExpr &node)
        {
            // (AR) تقييم iterable / (EN) Evaluate iterable
            node.iterable->accept(*this);
            Value iterableValue = lastResult_;

            // (AR) يقبل مصفوفةً أو خريطةً — انظر التعليق في استيعاب القائمة. / (EN) Accepts an array or map — see the list-comprehension note.
            const bool isMapIter = iterableValue.isMap();
            if (!iterableValue.isArray() && !isMapIter)
            {
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"kind", "قاموسي/dict"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_COMPREHENSION_NEEDS_ARRAY, Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)), _rc);
                };
                lastResult_ = Value(std::unordered_map<std::string, Value>());
                return;
            }

            // (AR) إنشاء قاموس النتيجة / (EN) Create result dictionary
            std::unordered_map<std::string, Value> result;

            // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
            variableManager_.enterScope(Data::ScopeType::BLOCK, "dict_comprehension");

            // (AR) يقيّم مفتاح+قيمة القاموس بعد فحص الشرط، ويضيف الزوج. / (EN) Evaluate the dict key+value after the condition, add the pair.
            auto emitDictPair = [&]()
            {
                node.key->accept(*this);
                Value keyValue = lastResult_;
                node.value->accept(*this);
                Value valueValue = lastResult_;
                result[keyValue.toString()] = valueValue;
            };

            if (isMapIter)
            {
                // (AR) تكرار مدخلات الخريطة (المفتاح[، القيمة]) / (EN) Iterate map entries (key[, value])
                const auto &map = iterableValue.toMapRef();
                bool firstEntry = true;
                for (const auto &[key, val] : map)
                {
                    Value keyValue(key);
                    if (firstEntry)
                    {
                        variableManager_.define(node.variable, keyValue);
                        if (!node.valueVariable.empty())
                            variableManager_.define(node.valueVariable, val);
                        firstEntry = false;
                    }
                    else
                    {
                        variableManager_.assign(node.variable, keyValue);
                        if (!node.valueVariable.empty())
                            variableManager_.assign(node.valueVariable, val);
                    }

                    bool includeItem = true;
                    if (node.condition)
                    {
                        node.condition->accept(*this);
                        includeItem = lastResult_.toBool();
                    }
                    if (includeItem)
                        emitDictPair();
                }
            }
            else
            {
                // (AR) المرور على كل عنصر / (EN) Iterate over each element
                for (size_t i = 0; i < iterableValue.size(); ++i)
                {
                    Value item = iterableValue[i];

                    // (AR) تحديث متغير الحلقة / (EN) Update loop variable
                    if (i == 0)
                    {
                        // أول iteration - نعرّف المتغير
                        variableManager_.define(node.variable, item);
                    }
                    else
                    {
                        // iterations تالية - نحدّث باستخدام assign
                        variableManager_.assign(node.variable, item);
                    }

                    // (AR) التحقق من الشرط إن وُجد / (EN) Check condition if exists
                    bool includeItem = true;
                    if (node.condition)
                    {
                        node.condition->accept(*this);
                        includeItem = lastResult_.toBool();
                    }

                    // (AR) تقييم Key و Value وإضافة النتيجة / (EN) Evaluate key & value expressions and add result
                    if (includeItem)
                        emitDictPair();
                }
            }

            // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
            variableManager_.exitScope();

            // (AR) إرجاع القاموس الناتج / (EN) Return result dictionary
            lastResult_ = Value(result);
        }

        // =========================================================================
        // (AR) تقييم استيعاب مجموعة / (EN) Set Comprehension Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitSetComprehensionExpr(SetComprehensionExpr &node)
        {
            // (AR) تقييم iterable / (EN) Evaluate iterable
            node.iterable->accept(*this);
            Value iterableValue = lastResult_;

            // (AR) يقبل مصفوفةً أو خريطةً — انظر التعليق في استيعاب القائمة. / (EN) Accepts an array or map — see the list-comprehension note.
            const bool isMapIter = iterableValue.isMap();
            if (!iterableValue.isArray() && !isMapIter)
            {
                {
                    Sad::Errors::RenderContext _rc;
                    _rc.placeholders = {{"kind", "مجموعة/set"}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(::Sad::Errors::ErrorCode::RUN_COMPREHENSION_NEEDS_ARRAY, Sad::Errors::SourceLocation("<input>", 0, 0), _rc);
                };
                lastResult_ = Value(std::vector<Value>());
                return;
            }

            // (AR) إنشاء set النتيجة (نستخدم vector مع فحص التكرار) / (EN) Create result set (using vector with uniqueness check)
            std::vector<Value> result;

            // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
            variableManager_.enterScope(Data::ScopeType::BLOCK, "set_comprehension");

            // (AR) يقيّم عنصر المجموعة ويضيفه إن لم يكن مكرّرًا. / (EN) Evaluate the set element and add it if not a duplicate.
            auto emitSetElement = [&]()
            {
                node.expression->accept(*this);
                Value itemValue = lastResult_;
                for (const auto &existingItem : result)
                {
                    // (AR) فحص بسيط للمساواة - مقارنة التمثيلات النصية / (EN) Simple equality via toString()
                    if (existingItem.toString() == itemValue.toString())
                        return;
                }
                result.push_back(itemValue);
            };

            if (isMapIter)
            {
                // (AR) تكرار مدخلات الخريطة (المفتاح[، القيمة]) / (EN) Iterate map entries (key[, value])
                const auto &map = iterableValue.toMapRef();
                bool firstEntry = true;
                for (const auto &[key, val] : map)
                {
                    Value keyValue(key);
                    if (firstEntry)
                    {
                        variableManager_.define(node.variable, keyValue);
                        if (!node.valueVariable.empty())
                            variableManager_.define(node.valueVariable, val);
                        firstEntry = false;
                    }
                    else
                    {
                        variableManager_.assign(node.variable, keyValue);
                        if (!node.valueVariable.empty())
                            variableManager_.assign(node.valueVariable, val);
                    }

                    bool includeItem = true;
                    if (node.condition)
                    {
                        node.condition->accept(*this);
                        includeItem = lastResult_.toBool();
                    }
                    if (includeItem)
                        emitSetElement();
                }
            }
            else
            {
                // (AR) المرور على كل عنصر / (EN) Iterate over each element
                for (size_t i = 0; i < iterableValue.size(); ++i)
                {
                    Value item = iterableValue[i];

                    // (AR) تحديث متغير الحلقة / (EN) Update loop variable
                    if (i == 0)
                    {
                        // أول iteration - نعرّف المتغير
                        variableManager_.define(node.variable, item);
                    }
                    else
                    {
                        // iterations تالية - نحدّث باستخدام assign
                        variableManager_.assign(node.variable, item);
                    }

                    // (AR) التحقق من الشرط إن وُجد / (EN) Check condition if exists
                    bool includeItem = true;
                    if (node.condition)
                    {
                        node.condition->accept(*this);
                        includeItem = lastResult_.toBool();
                    }

                    // (AR) تقييم Expression وإضافة النتيجة (بدون تكرار) / (EN) Evaluate expression and add result (without duplicates)
                    if (includeItem)
                        emitSetElement();
                }
            }

            // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
            variableManager_.exitScope();

            // (AR) إرجاع المجموعة الناتجة (كمصفوفة) / (EN) Return result set (as array)
            lastResult_ = Value(result);
        }

        // =========================================================================
        // (AR) فحص الوصول للأعضاء / (EN) Member Access Check
        // =========================================================================

        void ExpressionEvaluator::checkMemberAccess(
            AST::Visibility visibility,
            const std::string &memberName,
            Data::ClassType *targetClass)
        {
            // PUBLIC: دائماً متاح
            if (visibility == AST::Visibility::PUBLIC)
            {
                return;
            }

            // التحقق من السياق: هل نحن داخل method من الصنف نفسه أو صنف مشتق؟
            // Check context: are we inside a method from the same class or derived class?
            bool insideMethod = variableManager_.exists("هذا") || variableManager_.exists("this");

            if (insideMethod)
            {
                // (AR) نحن داخل طريقة — نتحقق من الصنف الحالي
                //      يدعم نوع OBJECT الحقيقي ونوع MAP القديم
                // (EN) We're inside a method — check current class
                //      Supports real OBJECT type and legacy MAP type
                Value thisValue = variableManager_.exists("هذا") ? variableManager_.get("هذا") : variableManager_.get("this");

                std::string currentClassName;
                if (thisValue.isObject())
                {
                    currentClassName = thisValue.getClassName();
                }
                else if (thisValue.isMap())
                {
                    auto fields = thisValue.toMap();
                    auto classNameIt = fields.find("__class__");
                    if (classNameIt != fields.end())
                    {
                        currentClassName = classNameIt->second.toString();
                    }
                }

                if (!currentClassName.empty())
                {
                    auto *classManager = Data::ClassManager::getInstance();
                    Data::ClassType *currentClass = classManager->getClass(currentClassName);

                    if (currentClass)
                    {
                        // (AR) التحقق إذا كان الصنف الحالي هو نفسه أو مشتق من targetClass
                        // (EN) Check if current class is same or derived from targetClass
                        Data::ClassType *temp = currentClass;
                        while (temp)
                        {
                            if (temp == targetClass || temp->name == targetClass->name)
                            {
                                // PRIVATE: متاح فقط في نفس الصنف
                                if (visibility == AST::Visibility::PRIVATE && temp == targetClass)
                                {
                                    return; // Same class - allow access
                                }
                                // PROTECTED: متاح في نفس الصنف أو الأصناف المشتقة
                                if (visibility == AST::Visibility::PROTECTED)
                                {
                                    return; // Same class or derived - allow access
                                }
                            }
                            temp = temp->getBaseClass();
                        }
                    }
                }
            }

            // الوصول مرفوض
            if (visibility == AST::Visibility::PRIVATE)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                    Lexer::Position(),
                    {{"resource", "private member '" + memberName + "' of class '" + targetClass->name + "'"}});
            }

            if (visibility == AST::Visibility::PROTECTED)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                    Lexer::Position(),
                    {{"resource", "protected member '" + memberName + "' of class '" + targetClass->name + "'"}});
            }
        }

        // =========================================================================
        // (AR) تقييم تعبير Await / (EN) Await Expression Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitAwaitExpr(AwaitExpr &node)
        {
            // (AR) تقييم التعبير المُنتظَر / (EN) Evaluate awaited expression
            node.expression->accept(*this);

            // (AR) التحقق إذا كانت النتيجة Future خريطة (map-based Future)
            // (EN) Check if result is a map-based Future
            if (lastResult_.isMap())
            {
                auto mapVal = lastResult_.toMap();
                auto futureIt = mapVal.find("__future__");
                if (futureIt != mapVal.end())
                {
                    // (AR) هذا Future مبني على خريطة
                    // (EN) This is a map-based Future

                    // (AR) التحقق إذا كان مرفوضاً
                    // (EN) Check if rejected
                    auto rejIt = mapVal.find("__rejected__");
                    if (rejIt != mapVal.end() && rejIt->second.isBoolean() && rejIt->second.toBool())
                    {
                        auto errIt = mapVal.find("__error__");
                        std::string errMsg = (errIt != mapVal.end()) ? errIt->second.toString() : "Unknown async error";
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_ASYNC_TASK_ERROR,
                            node.position,
                            {{"reason", errMsg}});
                    }

                    // (AR) إرجاع القيمة المحلولة
                    // (EN) Return resolved value
                    auto valIt = mapVal.find("__value__");
                    if (valIt != mapVal.end())
                    {
                        lastResult_ = valIt->second;
                        return;
                    }
                }
            }

            // (AR) التحقق إذا كانت النتيجة Future ككائن حقيقي (Object-based Future)
            // (EN) Check if result is an Object-based Future
            if (lastResult_.isObject())
            {
                auto objPtr = lastResult_.toObject();
                if (objPtr)
                {
                    auto futureIt = objPtr->fields.find("__future__");
                    if (futureIt != objPtr->fields.end())
                    {
                        auto valIt = objPtr->fields.find("__value__");
                        if (valIt != objPtr->fields.end())
                        {
                            lastResult_ = valIt->second;
                            return;
                        }
                    }
                }
            }

            // (AR) إذا لم يكن Future، نرجع القيمة كما هي (متوافق مع الإصدارات السابقة)
            // (EN) If not a Future, return value as-is (backward compatible)
        }

        // =========================================================================
        // (AR) تقييم تنفيذ القوالب / (EN) Template Instantiation Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitTemplateInstantiation(TemplateInstantiation &node)
        {
            // (AR) إنشاء نسخة من القالب بالأنواع المحددة
            // (EN) Instantiate template with specified types

#ifdef DEBUG
            std::cout << "[Template] تنفيذ قالب: " << node.templateName << "<";
            for (size_t i = 0; i < node.typeArguments.size(); i++)
            {
                if (i > 0)
                    std::cout << ", ";
                std::cout << static_cast<int>(node.typeArguments[i]);
            }
            std::cout << ">" << std::endl;
#endif

            // (AR) إنشاء مفتاح القالب
            // (EN) Create template key
            std::string templateKey = "__template_" + node.templateName;

            // (AR) البحث عن القالب في FunctionManager
            // (EN) Look for template in FunctionManager
            auto templateFunc = functionManager_.getFunction(templateKey, 0);

            if (!templateFunc)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_FUNCTION_NOT_FOUND,
                    node.position,
                    {{"function", "template:" + node.templateName}});
            }

            // (AR) التحقق من قيود القالب
            // (EN) Verify template constraints
            if (!node.typeArgumentNames.empty())
            {
                auto *templateDecl = statementExecutor_.getTemplateFunction(node.templateName);
                if (templateDecl && !templateDecl->typeParameters.empty())
                {
                    statementExecutor_.verifyTemplateConstraints(
                        templateDecl->typeParameters,
                        node.typeArgumentNames,
                        templateDecl->whereClause,
                        node.templateName,
                        node.position);
                }
            }

            // (AR) للتنفيذ الحالي: نحفظ معلومات القالب في قيمة خاصة
            // (EN) For current implementation: Store template info in special value
            // سنستخدم هذا عند استدعاء الدالة
            // We'll use this when calling the function

            // (AR) إنشاء اسم دالة مؤقت للنسخة المُنشأة
            // (EN) Create temporary function name for instantiated version
            std::string instantiatedName = node.templateName;

            // (AR) حفظ اسم القالب في lastResult_
            // (EN) Store template name in lastResult_
            lastResult_ = Value(instantiatedName);
        }

        // =========================================================================
        // (AR) زوّار التعابير المفقودة / (EN) Missing expression visitors
        // =========================================================================

        void ExpressionEvaluator::visitGeneratorExpr(GeneratorExpr &node)
        {
            // (AR) تنفيذ تعبير المولّد كمصفوفة كسولة (ينتج كل العناصر مباشرةً)
            // (EN) Execute generator expression as lazy array (produces all elements eagerly)
            // مثال: (س * 2 لكل س في [1, 2, 3] إذا س > 1)
            // Example: (x * 2 for x in [1, 2, 3] if x > 1)

            // (AR) تقييم التعبير القابل للتكرار
            // (EN) Evaluate the iterable expression
            node.iterable->accept(*this);
            Value iterableValue = lastResult_;

            std::vector<Data::Value> results;

            if (iterableValue.isArray())
            {
                // (AR) التكرار على المصفوفة
                // (EN) Iterate over array
                auto arr = iterableValue.toArray();

                // (AR) ندخل نطاقاً جديداً للمولّد
                // (EN) Enter new scope for generator
                variableManager_.enterScope(Data::ScopeType::BLOCK, "__generator__");

                for (const auto &item : arr)
                {
                    // (AR) تعريف أو تحديث متغير الحلقة — بحث واحد
                    // (EN) Define or update loop variable — single lookup
                    variableManager_.defineOrAssign(node.variable, item);

                    // (AR) التحقق من الشرط (إن وُجد)
                    // (EN) Check condition (if exists)
                    if (node.condition)
                    {
                        node.condition->accept(*this);
                        Value condValue = lastResult_;
                        if (!condValue.toBool())
                        {
                            continue; // (AR) تخطي العنصر / (EN) Skip element
                        }
                    }

                    // (AR) تقييم تعبير العنصر
                    // (EN) Evaluate element expression
                    node.element->accept(*this);
                    results.push_back(lastResult_);
                }

                variableManager_.exitScope();
            }
            else if (iterableValue.isMap())
            {
                // (AR) التكرار على خريطة (مفاتيح)
                // (EN) Iterate over map (keys)
                auto mapVal = iterableValue.toMap();

                variableManager_.enterScope(Data::ScopeType::BLOCK, "__generator__");

                for (const auto &[key, val] : mapVal)
                {
                    // (AR) تحسين أداء: بحث واحد / (EN) Performance: single lookup
                    variableManager_.defineOrAssign(node.variable, Data::Value(key));

                    if (node.condition)
                    {
                        node.condition->accept(*this);
                        if (!lastResult_.toBool())
                            continue;
                    }

                    node.element->accept(*this);
                    results.push_back(lastResult_);
                }

                variableManager_.exitScope();
            }
            else
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_GENERATOR_TYPE_INVALID,
                    node.position,
                    {{"type", "non-iterable"}});
            }

            // (AR) إرجاع المصفوفة الناتجة
            // (EN) Return the resulting array
            lastResult_ = Data::Value(results);
        }

        void ExpressionEvaluator::visitDecoratorExpr(DecoratorExpr &node)
        {
            // (AR) تقييم المزخرف كتعبير — يُرجع دالة المزخرف كقيمة قابلة للاستدعاء
            // (EN) Evaluate decorator as expression — returns decorator function as callable value
            auto func = functionManager_.getFunction(node.name, 1);
            if (!func)
            {
                // (AR) محاولة بدون عدد معاملات محدد / (EN) Try without specific param count
                auto overloads = functionManager_.getFunctionOverloads(node.name);
                if (!overloads.empty())
                {
                    func = overloads[0];
                }
            }
            if (func)
            {
                // (AR) المزخرف دالة — نرجعها كاسم الدالة (نص)
                // (EN) Decorator is a function — return it as function name string
                lastResult_ = Value(node.name);
            }
            else
            {
                // (AR) محاولة البحث عن المزخرف كمتغير (مثلاً: lambda)
                // (EN) Try looking up decorator as variable (e.g. lambda)
                try
                {
                    Value decoratorVal = variableManager_.get(node.name);
                    lastResult_ = decoratorVal;
                }
                catch (...)
                {
                    // (AR) إذا لم نجد الدالة أو المتغير، نرجع الاسم كنص (سلوك احتياطي)
                    // (EN) If not found as function or variable, fall back to name string
                    lastResult_ = Value(node.name);
                }
            }
        }

        void ExpressionEvaluator::visitInlineAsmExpr(InlineAsmExpr &node)
        {
            // (AR) تعبير التجميع المضمن غير مدعوم في المفسّر — متاح في المترجم فقط
            // (EN) Inline assembly is not supported in interpreter — available in compiler only
            ::Sad::Errors::throwRuntime(
                ::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                node.position,
                {{"resource", "inline assembly (interpreter unsupported; use sadc)"}});
        }

        /**
         * @brief (AR) تقييم تعبير المدى — ينشئ مصفوفة من بداية..نهاية (شامل الطرفين)
         * @brief (EN) Evaluate range expression — creates array from start..end (inclusive both ends)
         */
        void ExpressionEvaluator::visitRangeExpr(AST::RangeExpr &node)
        {
            node.start->accept(*this);
            // (AR) حدود المدى بدقّة 64-بت — كان toInt() يقتطع فيباعد المفسّر عن المترجم
            // (EN) Range bounds at 64-bit precision — toInt() truncated, diverging from the compiler
            int64_t startVal = lastResult_.toInt64();

            node.end->accept(*this);
            int64_t endVal = lastResult_.toInt64();

            Value::ArrayType arr;
            // (AR) حساب الاتّساع بلا إشارة — الفرق الموقَّع يطفح (UB) عند حدود متباعدة جدًّا
            // (EN) Unsigned span math — the signed difference overflows (UB) for far-apart bounds
            if (startVal <= endVal)
            {
                arr.reserve(static_cast<size_t>(
                    static_cast<uint64_t>(endVal) - static_cast<uint64_t>(startVal) + 1));
                for (int64_t i = startVal; i <= endVal; ++i)
                {
                    arr.push_back(Value(i));
                }
            }
            else
            {
                arr.reserve(static_cast<size_t>(
                    static_cast<uint64_t>(startVal) - static_cast<uint64_t>(endVal) + 1));
                for (int64_t i = startVal; i >= endVal; --i)
                {
                    arr.push_back(Value(i));
                }
            }

            lastResult_ = Value(std::move(arr));
        }

        /**
         * @brief (AR) تقييم تعبير الوصول الآمن — object?.member
         * @brief (EN) Evaluate optional chaining expression
         *
         * إذا كان الكائن لاشيء، يرجع لاشيء مباشرة بدون خطأ.
         * If object is null, returns null directly without error.
         */
        void ExpressionEvaluator::visitOptionalChainExpr(AST::OptionalChainExpr &node)
        {
            // تقييم الكائن أولاً
            node.object->accept(*this);

            // (AR) قِصر الدائرة الوحيد للوصول الآمن: إذا كان الكائن عدمًا (null) نُرجع
            //      عدمًا دون خطأ — S-TS-P1 (isNull لا isVoid). وما عدا ذلك يجب أن يتصرّف
            //      «?.» تمامًا كـ«.» (وراثة، getters، حقول، خرائط) عبر المنطق المشترك.
            // (EN) Optional access only short-circuits on a null object; otherwise it
            //      must behave exactly like '.', so we delegate to the shared resolver.
            if (lastResult_.isNull())
            {
                lastResult_ = Value::makeNull();
                return;
            }

            // (AR) فوّض الحلّ إلى نفس مسار «.» — يصلح الخطأ السابق الذي كان يُرجع «لاشيء»
            //      للحقول الموروثة/الـgetters لأنّه اكتفى بـ hasField الساذج.
            // (EN) Delegate to the same path as '.', fixing the prior bug where
            //      inherited/getter members wrongly returned null via naive hasField.
            Value objectValue = lastResult_;
            resolveInstanceMember(objectValue, node.member, node.position);
        }

        /**
         * @brief (AR) تقييم تعبير التجميع الفارغ — left ?? right
         * @brief (EN) Evaluate null coalescing expression
         *
         * إذا كان اليسار لاشيء، يرجع اليمين. وإلا يرجع اليسار.
         * If left is null, returns right. Otherwise returns left.
         */
        void ExpressionEvaluator::visitNullCoalesceExpr(AST::NullCoalesceExpr &node)
        {
            // تقييم الطرف الأيسر أولاً
            node.left->accept(*this);

            // (AR) إذا لم يكن عدم (null)، أرجعه — S-TS-P1 (isNull لا isVoid)
            if (!lastResult_.isNull())
            {
                return; // lastResult_ يحمل القيمة بالفعل
            }

            // إذا كان عدم، قيّم الطرف الأيمن
            node.right->accept(*this);
            // lastResult_ يحمل الآن قيمة الطرف الأيمن
        }

        // ═══════════════════════════════════════════════════════════════════
        // (AR) تقييم تعبير نشر الخطأ — انشر
        //
        //      الدلالات:
        //      1. تقييم التعبير الداخلي
        //      2. إذا كانت النتيجة تعداد جبري (ADT):
        //         أ. إذا __عضو__ == "نجاح" أو "بعض" ← استخرج أول حقل كقيمة التعبير
        //         ب. إذا __عضو__ == "خطأ" أو "عدم" ← أعد القيمة كاملة من الدالة فوراً
        //      3. إذا لم تكن تعداد جبري ← خطأ وقت التشغيل
        //
        //      لتحقيق الإرجاع المبكر، نرسل إشارة RETURN عبر StatementExecutor
        //      لأن ExpressionEvaluator لا يملك flowControl_ مباشرة
        //
        // (EN) Evaluate error propagation expression — propagate
        //      Semantics: evaluate inner, check if Result/Option ADT,
        //      unwrap on success/some, early-return on error/none
        // ═══════════════════════════════════════════════════════════════════
        void ExpressionEvaluator::visitErrorPropagateExpr(AST::ErrorPropagateExpr &node)
        {
            // ─── الخطوة 1: تقييم التعبير الداخلي ───
            node.inner->accept(*this);
            Data::Value result = lastResult_;

            // ─── الخطوة 2: التحقق من أن النتيجة تعداد جبري (ADT) ───
            if (!result.isMap())
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                    node.position,
                    {{"type", result.getTypeName()}, {"operation", "انشر"}});
            }

            auto mapVal = result.toMap();

            // (AR) التحقق من وجود علامة التعداد الجبري
            auto adtIt = mapVal.find("__جبري__");
            if (adtIt == mapVal.end() || !adtIt->second.isBoolean() || !adtIt->second.toBool())
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                    node.position,
                    {{"type", "non-ADT map"}, {"operation", "انشر"}});
            }

            // ─── الخطوة 3: استخراج اسم العضو ───
            auto memberIt = mapVal.find("__عضو__");
            if (memberIt == mapVal.end())
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_PROPERTY_NOT_FOUND,
                    node.position,
                    {{"member", "__عضو__"}, {"class", "ADT"}});
            }
            std::string memberName = memberIt->second.toString();

            // ─── الخطوة 4: معالجة حسب نوع العضو ───

            // (AR) حالة النجاح: نجاح أو بعض → استخراج الحمولة
            if (memberName == "نجاح" || memberName == "بعض")
            {
                // (AR) استخراج أول حقل من الحمولة
                auto fieldsIt = mapVal.find("__حقول__");
                if (fieldsIt != mapVal.end() && fieldsIt->second.isArray())
                {
                    auto fields = fieldsIt->second.toArray();
                    if (!fields.empty())
                    {
                        lastResult_ = fields[0];
                        return;
                    }
                }
                // (AR) إذا لم توجد حقول ← ارجع فراغ (unit variant نجاح بدون قيمة)
                lastResult_ = Data::Value();
                return;
            }

            // (AR) حالة الخطأ: خطأ أو عدم → إرجاع مبكر من الدالة الحالية
            if (memberName == "خطأ" || memberName == "عدم")
            {
                // (AR) نستخدم نفس آلية ارجع (return) — عبر StatementExecutor
                //      نرمي استثناء خاص يلتقطه StatementExecutor
                //      القيمة المُرجعة هي القيمة الأصلية كاملة (نتيجة.خطأ(...) أو اختياري.عدم)
                statementExecutor_.setReturnValueDirect(result);
                return;
            }

            // (AR) عضو غير معروف — ليس من أنواع نتيجة/اختياري القياسية
            //      نحاول معاملته كقيمة نجاح بشكل مرن
            // (EN) Unknown member — not standard Result/Option variant
            //      Try treating as success value permissively
            auto fieldsIt = mapVal.find("__حقول__");
            if (fieldsIt != mapVal.end() && fieldsIt->second.isArray())
            {
                auto fields = fieldsIt->second.toArray();
                if (!fields.empty())
                {
                    lastResult_ = fields[0];
                    return;
                }
            }
            lastResult_ = result;
        }

        // ══════════════════════════════════════════════════════════════════════
        // (AR) تقييم تعبير الصف — (قيمة1، قيمة2، ...)
        //      يُقيّم كل عنصر ثم يُنشئ قيمة TUPLE جديدة
        // (EN) Evaluate tuple expression — (value1, value2, ...)
        //      Evaluates each element then creates a new TUPLE value
        // ══════════════════════════════════════════════════════════════════════

        void ExpressionEvaluator::visitTupleExpr(AST::TupleExpr &node)
        {
            Data::Value::TupleType elements;
            elements.reserve(node.elements.size());

            for (auto &elem : node.elements)
            {
                elem->accept(*this);
                elements.push_back(lastResult_);
            }

            lastResult_ = Data::Value(Data::Value::TupleTag{}, std::move(elements));
        }

    } // namespace Interpreter
} // namespace Sad
