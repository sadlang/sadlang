// ======================================================================
// expression_evaluator_ui.cpp — تقييم تعابير عناصر الواجهات التصريحية
// ======================================================================
// الوصف بالعربية:
//   ينفذ visitUIWidgetExpr الذي يحوّل UIWidgetExprNode إلى Value.
//   يُنشئ WidgetBuilder يغلّف IRNode مباشرة — بدون خرائط وسيطة.
//   WidgetBuilder يدعم سلسلة المعدّلات (modifier chain).
//
// English Description:
//   Implements visitUIWidgetExpr which converts UIWidgetExprNode to Value.
//   Creates WidgetBuilder wrapping IRNode directly — no intermediate maps.
//   WidgetBuilder supports modifier chain.
//
// المسار:
//   UIWidgetExprNode → WidgetBuilder(IRNode) → UIBridge يستخرج IRNode مباشرة
// ======================================================================

#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ui_nodes.h"
#include "object_instance.h"
#include "class_manager.h"
#include "ui_bridge.h"
#include "widget_builder.h"
#include <atomic>
#include <iostream>

namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;

        // =====================================================================
        // visitUIWidgetExpr — تقييم تعبير عنصر واجهة
        // =====================================================================
        //
        // (AR) يحوّل UIWidgetExprNode (عقدة AST) إلى WidgetBuilder يغلّف IRNode:
        //
        //      UIWidgetExprNode{widgetName="عمود", modifiers=[...], children=[...]}
        //         ↓
        //      WidgetBuilder{IRNode{Column, خصائص=..., أحداث=..., أبناء=[...]}}
        //
        //      المعدّلات تُطبّق مباشرة على IRNode (setProperty / addEvent)
        //      الأبناء يُضافون كـ IRNode أيضاً (addChild)
        //
        // (EN) Converts UIWidgetExprNode (AST node) to WidgetBuilder wrapping IRNode:
        //      Modifiers applied directly to IRNode (setProperty / addEvent)
        //      Children added as IRNode (addChild)
        // =====================================================================
        void ExpressionEvaluator::visitUIWidgetExpr(UIWidgetExprNode &node)
        {
            // ─────────────────────────────────────────────────────────────────
            // (AR) تحويل اسم العنصر إلى UINodeType ثم إنشاء WidgetBuilder
            // (EN) Convert widget name to UINodeType then create WidgetBuilder
            // ─────────────────────────────────────────────────────────────────
            // (AR) نحاول التحويل عبر arabicNameToNodeType
            // (EN) Try arabicNameToNodeType
            sad::ui::UINodeType nodeType = sad::ui::UINodeType::Container; // (AR) افتراضي
            {
                auto result = sad::ui::arabicNameToNodeType(node.widgetName);
                if (result.has_value())
                {
                    nodeType = result.value();
                }
            }

            // (AR) B-step5b: WidgetBuilder خام مُدار بـGC
            auto *builder = new WidgetBuilder(nodeType);

            // ─────────────────────────────────────────────────────────────────
            // (AR) معالجة الوسائط الموضعية — تُعيّن كخصائص على IRNode
            // (EN) Process positional arguments — set as properties on IRNode
            // ─────────────────────────────────────────────────────────────────
            if (!node.arguments.empty())
            {
                for (size_t i = 0; i < node.arguments.size(); i++)
                {
                    node.arguments[i]->accept(*this);
                    Value argVal = lastResult_;

                    if (i == 0)
                    {
                        // (AR) الوسيط الأول → خاصية حسب نوع العنصر
                        // (EN) First arg → property based on widget type
                        if (node.widgetName == "\xd9\x86\xd8\xb5" || node.widgetName == "Text")
                        {
                            builder->setIRPropertyFromValue("\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", argVal); // محتوى
                        }
                        else if (node.widgetName == "\xd8\xb2\xd8\xb1" || node.widgetName == "Button")
                        {
                            builder->setIRPropertyFromValue("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", argVal); // عنوان
                        }
                        else if (node.widgetName == "\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9" || node.widgetName == "Image")
                        {
                            builder->setIRPropertyFromValue("\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1", argVal); // مصدر
                        }
                        else if (node.widgetName == "\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5" || node.widgetName == "TextField")
                        {
                            builder->setIRPropertyFromValue("\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad", argVal); // تلميح
                        }
                        else
                        {
                            builder->setIRPropertyFromValue("_\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb7_0", argVal); // _وسيط_0
                        }
                    }
                    else
                    {
                        builder->setIRPropertyFromValue("_\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb7_" + std::to_string(i), argVal);
                    }
                }
            }

            // (AR) معالجة الوسائط المسماة — تُعيّن كخصائص مسمّاة على IRNode
            // (EN) Process named arguments — set as named properties on IRNode
            for (auto &[name, expr] : node.namedArgs)
            {
                expr->accept(*this);
                // (AR) إذا كانت القيمة WidgetBuilder → إضافة كابن مسمّى
                // (EN) If value is WidgetBuilder → add as named child
                if (lastResult_.isObject())
                {
                    auto *obj = lastResult_.toObject();
                    if (obj && isWidgetBuilder(obj))
                    {
                        auto childBuilder = static_cast<WidgetBuilder *>(obj);
                        builder->addChild(childBuilder->getIRNode());
                        // (AR) أيضاً نعيّن خاصية بالاسم للربط
                        builder->setIRProperty(name, "widget_" + std::to_string(childBuilder->objectId));
                        continue;
                    }
                }
                builder->setIRPropertyFromValue(name, lastResult_);
            }

            // ─────────────────────────────────────────────────────────────────
            // (AR) معالجة سلسلة المعدّلات (Modifier Chain)
            //      كل معدّل إما خاصية بصرية → setProperty على IRNode
            //      أو معالج حدث → addEvent على IRNode + registerHandler في UIBridge
            // (EN) Process modifier chain
            //      Visual property → setProperty on IRNode
            //      Event handler → addEvent on IRNode + registerHandler in UIBridge
            // ─────────────────────────────────────────────────────────────────

            // (AR) استخراج كائن المالك (هذا) من النطاق — لربط الأحداث بالمكون
            // (EN) Extract owner (هذا) from scope — to bind events to component
            Data::ObjectInstance *ownerObj = nullptr;
            try
            {
                auto thisVal = variableManager_.get("\xd9\x87\xd8\xb0\xd8\xa7"); // هذا
                if (thisVal.isObject())
                {
                    ownerObj = thisVal.toObject();
                }
            }
            catch (...)
            {
                // (AR) لا يوجد هذا في النطاق
            }

            // (AR) حفظ المالك على WidgetBuilder لتسجيل الأحداث لاحقاً
            // (EN) Store owner on WidgetBuilder for later event registration
            if (ownerObj)
            {
                builder->fields["__owner"] = Value(
                    static_cast<Data::ObjectInstance *>(ownerObj));
            }

            for (auto &modifier : node.modifiers)
            {
                if (modifier->isEvent && modifier->eventHandler)
                {
                    // ─── معالج حدث ───────────────────────────────────
                    // (AR) تسجيل المعالج في UIBridge والحصول على معرّف
                    //      ثم إضافة الحدث مباشرة على IRNode
                    // (EN) Register handler in UIBridge and get ID
                    //      then add event directly on IRNode
                    auto *bridge = UIBridge::active();
                    if (bridge)
                    {
                        auto &handler = *modifier->eventHandler;
                        std::string handlerId;

                        switch (handler.kind)
                        {
                        case UIEventKind::ARROW:
                        {
                            // (AR) سهم: .عند_النقر => تعبير
                            if (handler.arrowExpr)
                            {
                                static std::atomic<int> arrowCounter{0};
                                std::string arrowName = "__ui_arrow_" + std::to_string(arrowCounter.fetch_add(1));

                                auto bodyNode = std::shared_ptr<AST::ASTNode>(
                                    static_cast<AST::ASTNode *>(handler.arrowExpr.get()),
                                    [](AST::ASTNode *) {});

                                std::vector<Data::FunctionParameter> params;
                                functionManager_.defineFunction(arrowName, params, bodyNode);

                                // (AR) التقاط المتغيرات من النطاق الحالي (closure)
                                {
                                    auto func = functionManager_.getFunction(arrowName, 0);
                                    if (func)
                                    {
                                        std::unordered_map<std::string, Data::Value> captures;
                                        auto varNames = variableManager_.getVariableNames();
                                        for (const auto &vname : varNames)
                                        {
                                            if (vname != arrowName)
                                            {
                                                try
                                                {
                                                    captures[vname] = variableManager_.get(vname);
                                                }
                                                catch (...)
                                                {
                                                }
                                            }
                                        }
                                        func->setCaptures(captures);
                                    }
                                }

                                auto funcRef = std::make_shared<Data::FunctionRef>(
                                    arrowName, arrowName,
                                    Data::FunctionRefKind::LAMBDA, 0);
                                handlerId = bridge->registerHandler(Value(std::move(funcRef)), ownerObj);
                            }
                            break;
                        }
                        case UIEventKind::BLOCK:
                        {
                            // (AR) كتلة: .عند_النقر ... نهاية
                            if (handler.blockBody)
                            {
                                static std::atomic<int> blockCounter{0};
                                std::string blockName = "__ui_block_" + std::to_string(blockCounter.fetch_add(1));

                                auto bodyNode = std::shared_ptr<AST::ASTNode>(
                                    static_cast<AST::ASTNode *>(handler.blockBody.get()),
                                    [](AST::ASTNode *) {});

                                std::vector<Data::FunctionParameter> params;
                                functionManager_.defineFunction(blockName, params, bodyNode);

                                {
                                    auto func = functionManager_.getFunction(blockName, 0);
                                    if (func)
                                    {
                                        std::unordered_map<std::string, Data::Value> captures;
                                        auto varNames = variableManager_.getVariableNames();
                                        for (const auto &vname : varNames)
                                        {
                                            if (vname != blockName)
                                            {
                                                try
                                                {
                                                    captures[vname] = variableManager_.get(vname);
                                                }
                                                catch (...)
                                                {
                                                }
                                            }
                                        }
                                        func->setCaptures(captures);
                                    }
                                }

                                auto funcRef = std::make_shared<Data::FunctionRef>(
                                    blockName, blockName,
                                    Data::FunctionRefKind::LAMBDA, 0);
                                handlerId = bridge->registerHandler(Value(std::move(funcRef)), ownerObj);
                            }
                            break;
                        }
                        case UIEventKind::LAMBDA:
                        {
                            // (AR) لامدا صريحة: .عند_التغيير(لامدا(ق) ...)
                            if (handler.lambdaExpr)
                            {
                                handler.lambdaExpr->accept(*this);
                                handlerId = bridge->registerHandler(lastResult_, ownerObj);
                            }
                            break;
                        }
                        }

                        if (!handlerId.empty())
                        {
                            // (AR) إضافة الحدث مباشرة على IRNode
                            // (EN) Add event directly on IRNode
                            builder->addIREvent(modifier->name, handlerId);
                        }
                    }
                }
                else
                {
                    // ─── معدّل خاصية بصرية ────────────────────────────
                    // (AR) تقييم وسائط المعدّل وتعيينها مباشرة على IRNode
                    // (EN) Evaluate modifier arguments and set directly on IRNode
                    if (modifier->arguments.size() == 1)
                    {
                        modifier->arguments[0]->accept(*this);
                        Value modVal = lastResult_;

                        // (AR) إذا كانت القيمة WidgetBuilder → إضافة كابن
                        // (EN) If value is WidgetBuilder → add as child
                        if (modVal.isObject())
                        {
                            auto *obj = modVal.toObject();
                            if (obj && isWidgetBuilder(obj))
                            {
                                auto childBuilder = static_cast<WidgetBuilder *>(obj);
                                builder->addChild(childBuilder->getIRNode());
                                // (AR) أيضاً نحفظ المعدّل كخاصية للإشارة
                                builder->setIRProperty(modifier->name,
                                                       "widget_" + std::to_string(childBuilder->objectId));
                                continue;
                            }
                        }

                        builder->setIRPropertyFromValue(modifier->name, modVal);
                    }
                    else if (modifier->arguments.size() > 1)
                    {
                        // (AR) معدّل بعدة وسائط: تُخزن كمصفوفة نصية مفصولة بفواصل
                        // (EN) Multi-arg modifier: stored as comma-separated values
                        std::string combinedValue;
                        for (size_t mi = 0; mi < modifier->arguments.size(); mi++)
                        {
                            modifier->arguments[mi]->accept(*this);
                            if (mi > 0)
                                combinedValue += ",";
                            combinedValue += lastResult_.toString();
                        }
                        builder->setIRProperty(modifier->name, combinedValue);
                    }
                    else
                    {
                        // (AR) معدّل بدون وسائط: .عريض() → true
                        // (EN) No-arg modifier: .عريض() → true
                        builder->setIRProperty(modifier->name, true);
                    }
                }
            }

            // ─────────────────────────────────────────────────────────────────
            // (AR) معالجة الأبناء (Children) — تقييم تكراري
            //      كل ابن يُقيَّم، وإذا كان WidgetBuilder يُستخرج IRNode
            //      وإلا يُنشأ IRNode نصي من قيمته
            // (EN) Process children — recursive evaluation
            //      Each child evaluated; if WidgetBuilder, extract IRNode
            //      Otherwise create text IRNode from its value
            // ─────────────────────────────────────────────────────────────────
            if (!node.children.empty())
            {
                for (auto &child : node.children)
                {
                    child->accept(*this);
                    Value childVal = lastResult_;

                    if (childVal.isObject())
                    {
                        auto *obj = childVal.toObject();
                        if (obj && isWidgetBuilder(obj))
                        {
                            // (AR) ابن WidgetBuilder → إضافة IRNode مباشرة
                            auto childBuilder = static_cast<WidgetBuilder *>(obj);
                            builder->addChild(childBuilder->getIRNode());
                        }
                        else if (obj)
                        {
                            // (AR) كائن عادي (مكون واجهة مع بناء()) → يُحوّل لاحقاً في UIBridge
                            //      نضعه كابن عبر إنشاء IRNode مؤقت بخاصية _object
                            // (EN) Regular object (UI component with بناء()) → converted later
                            auto customNode = sad::ui::IRNode::create(sad::ui::UINodeType::CustomWidget);
                            customNode->setProperty("_class", obj->getClassName());
                            customNode->setId("obj_" + std::to_string(obj->objectId));
                            builder->addChild(customNode);
                            // (AR) نحفظ مرجع الكائن في حقل على WidgetBuilder
                            builder->fields["_child_obj_" + std::to_string(obj->objectId)] = childVal;
                        }
                    }
                    else if (!childVal.isVoid())
                    {
                        // (AR) قيمة بسيطة (نص/رقم) → تُحوَّل لعنصر نصي
                        // (EN) Simple value (string/number) → converted to text node
                        auto textNode = sad::ui::IRNode::create(sad::ui::UINodeType::Text);
                        textNode->setProperty("\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", childVal.toString()); // محتوى
                        builder->addChild(textNode);
                    }
                }
            }

            // ─────────────────────────────────────────────────────────────────
            // (AR) إرجاع WidgetBuilder كنتيجة (Value::OBJECT)
            // (EN) Return WidgetBuilder as result (Value::OBJECT)
            // ─────────────────────────────────────────────────────────────────
            lastResult_ = Value(static_cast<Data::ObjectInstance *>(builder));
        }

    } // namespace Interpreter
} // namespace Sad
