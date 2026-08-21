/**
 * @file expression_evaluator_members_assign.cpp
 * @brief (AR) تنفيذ تعابير التعيين واللامبدا في ExpressionEvaluator
 * @brief (EN) Assignment and lambda expressions in ExpressionEvaluator
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

#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
#include "null_safety/null_safety_analyzer.h" // (AR) محور الصرامة D6 لحارس SEM045 / (EN) D6 strictness axis for the SEM045 guard
#include "visitors/sem045_report.h"           // (AR) باب إبلاغ SEM045 الواحد / (EN) single SEM045 reporting door
#include "sad_type_utils.h"                   // (AR) kindToArabic لرسالة SEM045 / (EN) kindToArabic for the SEM045 message
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

        void ExpressionEvaluator::visitMemberAssignExpr(MemberAssignExpr &node)
        {
            // ═══════════════════════════════════════════════════════════════════════
            // (AR) تعيين قيمة لحقل في كائن: كائن.حقل = قيمة
            //      يدعم كلاً من نوع OBJECT الحقيقي ونوع MAP القديم
            //
            //      مع OBJECT الحقيقي: التعيين يكون بالمرجع — أي تغيير ينعكس
            //      على جميع المتغيرات التي تشير لنفس الكائن
            //
            // (EN) Assign value to field in object: object.field = value
            //      Supports both real OBJECT type and legacy MAP type
            //
            //      With real OBJECT: assignment is by reference — any change
            //      reflects on all variables pointing to the same object
            // ═══════════════════════════════════════════════════════════════════════

            // (AR) تقييم الكائن
            // (EN) Evaluate the object
            node.object->accept(*this);
            Value objectValue = lastResult_;

            // (AR) تقييم القيمة الجديدة
            // (EN) Evaluate the new value
            node.value->accept(*this);
            Value newValue = lastResult_;

            std::string className;
            ClassType *classType = nullptr;
            Value::MapType fields;
            bool isRealObject = false;
            ObjectInstance *objPtr = nullptr;

            if (objectValue.isObject())
            {
                // (AR) نوع OBJECT الحقيقي
                // (EN) Real OBJECT type
                isRealObject = true;
                objPtr = objectValue.toObject();
                if (objPtr)
                {
                    className = objPtr->getClassName();
                    fields = objPtr->fields;
                }
            }
            else if (objectValue.isMap())
            {
                // (AR) نوع MAP القديم
                // (EN) Legacy MAP type
                fields = objectValue.toMap();
                auto classNameIt = fields.find("__class__");
                if (classNameIt == fields.end())
                {
                    // ═══════════════════════════════════════════════════════════════
                    // (AR) خريطة عادية بدون صنف — وصول مباشر للمفتاح
                    //      مثال: خ.مفتاح = قيمة  حيث خ = {"مفتاح": ...}
                    // (EN) Plain map without class — direct key access
                    //      Example: m.key = val  where m = {"key": ...}
                    // ═══════════════════════════════════════════════════════════════
                    fields[node.member] = newValue;
                    Value modifiedObject(fields);
                    writeBackChain(node.object.get(), modifiedObject);
                    lastResult_ = newValue;
                    return;
                }
                className = classNameIt->second.toString();
            }
            else if (objectValue.isString())
            {
                // ═══════════════════════════════════════════════════════════════
                // (AR) وصول ثابت/ساكن: ClassName.staticField = value
                //      مثال: موظف.عدد_الموظفين = 5 أو موظف.عدد_الموظفين += 1
                // (EN) Static access: ClassName.staticField = value
                // ═══════════════════════════════════════════════════════════════
                std::string possibleClassName = objectValue.toString();
                auto *cm = Data::ClassManager::getInstance();
                ClassType *staticClassType = cm->getClass(possibleClassName);
                if (staticClassType)
                {
                    ClassField *field = staticClassType->findField(node.member);
                    if (field && field->isStatic)
                    {
                        staticClassType->setStaticField(node.member, newValue);
                        lastResult_ = newValue;
                        return;
                    }
                }
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                    node.position,
                    {{"type", objectValue.getTypeName()}, {"operation", ".="}});
            }
            else
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                    node.position,
                    {{"type", objectValue.getTypeName()}, {"operation", ".="}});
            }

            // (AR) الحصول على ClassType
            // (EN) Get ClassType
            auto *classManager = Data::ClassManager::getInstance();
            classType = classManager->getClass(className);

            if (!classType)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_CLASS_NOT_FOUND,
                    node.position,
                    {{"class", className}});
            }

            // (AR) البحث عن الحقل
            // (EN) Find the field
            ClassField *field = classType->findField(node.member);

            // (AR) البحث عن خاصية (Property) إذا لم يُوجد حقل
            // (EN) Search for property if field not found
            ClassProperty *property = nullptr;
            if (!field)
            {
                property = classType->findProperty(node.member);
            }

            if (!field && !property)
            {
                // (AR) لم يُعثر على حقل أو خاصية معلنة — إنشاء حقل ديناميكي
                //      هذا يسمح بـ هذا.حقل = قيمة في الباني بدون إعلان مسبق
                // (EN) No declared field or property found — create dynamic field
                //      This allows this.field = value in constructor without prior declaration
                objPtr->setField(node.member, newValue);
                // (AR) مزامنة النطاق: نستخدم define بدلاً من assign لأن define
                //      لا يتجاوز حدود FUNCTION scope — وبذلك لا تتسرب القيم
                //      من باني كائن جديد داخل عامل إلى نطاق العامل الخارجي
                // (EN) Scope sync: use define instead of assign because define
                //      respects FUNCTION scope boundaries — preventing values
                //      from leaking from a nested constructor into operator scope
                variableManager_.define(node.member, newValue);
                lastResult_ = newValue;
                return;
            }

            // (AR) إذا كانت خاصية، نفذ الـ setter
            // (EN) If it's a property, execute the setter
            if (property)
            {
                checkMemberAccess(property->visibility, node.member, classType);

                if (!property->setterBody)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                        node.position,
                        {{"resource", "property setter '" + node.member + "'"}});
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) تنفيذ setter body في نطاق جديد مع سياق الكائن
                //      نربط 'هذا' بالكائن، ونربط 'قيمة'/'value' بالقيمة الجديدة
                //      وكذلك نربط الحقول للوصول المباشر
                //
                // (EN) Execute setter body in new scope with object context
                //      Bind 'هذا' (this) to object, 'قيمة'/'value' to new value
                //      and bind fields for direct access
                // ═══════════════════════════════════════════════════════════════
                variableManager_.enterScope(Data::ScopeType::FUNCTION, "set_" + node.member);

                // (AR) ربط 'هذا' و 'قيمة' / (EN) Bind 'this' and 'value'
                variableManager_.define("هذا", objectValue);
                variableManager_.define("this", objectValue);
                variableManager_.define("قيمة", newValue);
                variableManager_.define("value", newValue);

                // (AR) ربط اسم معامل setter الفعلي (مثل 'ق' في عيّن(ق))
                //      هذا يجعل اسم المعامل مرئياً داخل جسم setter
                // (EN) Bind the actual setter parameter name (e.g. 'ق' in عيّن(ق))
                //      This makes the parameter name visible inside the setter body
                if (!property->setterParamName.empty())
                {
                    variableManager_.define(property->setterParamName, newValue);
                }

                // (AR) ربط حقول الكائن / (EN) Bind object fields
                if (isRealObject && objPtr)
                {
                    for (const auto &[fname, fval] : objPtr->fields)
                    {
                        variableManager_.define(fname, fval);
                    }
                }
                else
                {
                    auto fields = objectValue.toMap();
                    for (const auto &[fname, fval] : fields)
                    {
                        if (fname != "__class__")
                        {
                            variableManager_.define(fname, fval);
                        }
                    }
                }

                try
                {
                    // (AR) تنفيذ setter body عبر StatementExecutor (لأن الجسم هو BlockStmt)
                    // (EN) Execute setter body via StatementExecutor (because body is BlockStmt)
                    property->setterBody->accept(statementExecutor_);

                    if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                    {
                        statementExecutor_.resetFlowControl();
                    }

                    // (AR) كتابة الحقول المحدثة إلى الكائن
                    // (EN) Write back updated fields to object
                    if (isRealObject && objPtr)
                    {
                        for (auto &[fname, fval] : objPtr->fields)
                        {
                            if (variableManager_.exists(fname))
                            {
                                Value updated = variableManager_.get(fname);
                                objPtr->setField(fname, updated);
                            }
                            // (AR) الحقل غير موجود في النطاق — متوقع عند كتابة الحقول
                            // (EN) Field not in scope — expected during field writeback
                        }
                    }
                }
                catch (...)
                {
                    variableManager_.exitScope();
                    throw;
                }

                variableManager_.exitScope();
                lastResult_ = newValue;
                return;
            }

            // (AR) معالجة الحقل العادي
            // (EN) Handle regular field
            checkMemberAccess(field->visibility, node.member, classType);

            // ═══════════════════════════════════════════════════════════════
            // (AR) SEM045 (RFC عقد الغياب — حقول الأصناف): «فراغ» لا يعبر إلى
            //      حقلٍ مصنَّف («رقم قيمة») — نفس عقد خانات التصريح وإعادة
            //      الإسناد والمعاملات، على محور الصرامة D6 نفسِه. الحقلُ
            //      المجرَّد (declaredKind=Unknown) خانةٌ ديناميّةٌ تقبل الفراغَ
            //      كما تقبله الخانةُ المجرَّدة — لا حراسةَ عليه.
            // (EN) SEM045 (absence-contract RFC — class fields): Void must not
            //      cross into a TYPED field — same D6 contract as the
            //      declaration/reassignment/parameter guards. A bare field
            //      (declaredKind=Unknown) is a dynamic slot and accepts Void
            //      like any bare slot — unguarded on purpose.
            // ═══════════════════════════════════════════════════════════════
            if (newValue.getKind() == Types::SadTypeKind::Void &&
                Sad::Interpreter::Sem045::kindIsGuarded(field->declaredKind))
            {
                // (AR) الإبلاغُ والمسنَدُ عبر البابِ الواحد (sem045_report).
                // (EN) Reporting and predicate through the single door (sem045_report).
                Sad::Interpreter::Sem045::reportVoidCrossing(
                    node.member, Sad::Types::kindToArabic(field->declaredKind),
                    node.position,
                    Sad::NullSafety::strictnessFromOwnershipMode(
                        statementExecutor_.getMemoryPolicy().ownershipMode));
            }

            if (isRealObject && objPtr)
            {
                // ═══════════════════════════════════════════════════════════════
                // (AR) OBJECT حقيقي: نحدّث ObjectInstance مباشرة بالمرجع
                //      لا نحتاج إعادة تعيين المتغير لأن shared_ptr يعمل بالمرجع
                //
                // (EN) Real OBJECT: update ObjectInstance directly by reference
                //      No need to reassign the variable since shared_ptr works by ref
                //
                // (AR) مزامنة متغير النطاق: عند تعيين هذا.حقل = قيمة داخل دالة،
                //      يجب تحديث متغير النطاق أيضاً لأن مرحلة الكتابة الراجعة
                //      في visitMethodCallExpr تقرأ من متغيرات النطاق
                //
                // (EN) Sync scope variable: when assigning this.field = value inside
                //      a method, also update scope variable because the writeback
                //      phase in visitMethodCallExpr reads from scope variables
                // ═══════════════════════════════════════════════════════════════
                objPtr->setField(node.member, newValue);
                // (AR) مزامنة النطاق: نستخدم define بدلاً من assign لأن define
                //      لا يتجاوز حدود FUNCTION scope — وبذلك لا تتسرب القيم
                //      من باني كائن جديد داخل عامل إلى نطاق العامل الخارجي
                // (EN) Scope sync: use define instead of assign because define
                //      respects FUNCTION scope boundaries — preventing values
                //      from leaking from a nested constructor into operator scope
                variableManager_.define(node.member, newValue);
                lastResult_ = newValue;
            }
            else
            {
                // (AR) MAP القديم: نحتاج إعادة بناء وتعيين — ندعم أي عمق
                // (EN) Legacy MAP: need to rebuild and reassign — supports any depth
                fields[node.member] = newValue;
                Value modifiedObject(fields);

                // (AR) كتابة رجوعية تكرارية تدعم أي مستوى تداخل
                // (EN) Recursive writeback supports any nesting depth
                writeBackChain(node.object.get(), modifiedObject);
                lastResult_ = newValue;
            }
        }

        // =========================================================================
        // (AR) تقييم الإسناد بالفهرس / (EN) Index Assignment Evaluation
        // م[0] = 5 ، قاموس["مفتاح"] = قيمة
        // =========================================================================
        void ExpressionEvaluator::visitIndexAssignExpr(IndexAssignExpr &node)
        {
            // ==========================================
            // الخطوة 1: تقييم الكائن (المصفوفة أو القاموس)
            // ==========================================
            node.object->accept(*this);
            Value objectValue = lastResult_;

            // ==========================================
            // الخطوة 2: تقييم الفهرس
            // ==========================================
            node.index->accept(*this);
            Value indexValue = lastResult_;

            // ==========================================
            // الخطوة 3: تقييم القيمة الجديدة
            // ==========================================
            node.value->accept(*this);
            Value newValue = lastResult_;

            // ==========================================
            // الخطوة 4: التعامل مع المصفوفات
            // ==========================================
            if (objectValue.isArray())
            {
                if (!indexValue.isNumeric())
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_ARRAY_INDEX_NOT_NUMBER,
                        node.position,
                        {{"actual", indexValue.getTypeName()}});
                }

                auto arr = objectValue.toArray();
                // (AR) فهرس 64-بت — منع لفّ toInt() الصامت / (EN) 64-bit index — no silent toInt() wrap
                int64_t idx = indexValue.isInteger() ? indexValue.toInt64() : static_cast<int64_t>(indexValue.toDouble());
                int64_t size = static_cast<int64_t>(arr.size());

                // دعم الفهارس السالبة: -1 = آخر عنصر، -2 = ما قبل الأخير...
                if (idx < 0)
                {
                    idx = size + idx;
                }

                if (idx < 0 || idx >= size)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_INDEX_OUT_OF_RANGE,
                        node.position,
                        {{"index", std::to_string(idx)}, {"length", std::to_string(size)}, {"container", objectValue.getTypeName()}});
                }

                arr[idx] = newValue;

                // (AR) كتابة رجوعية تكرارية تدعم أي مستوى تداخل
                // (EN) Recursive writeback supports any nesting depth
                writeBackChain(node.object.get(), Value(arr));

                lastResult_ = newValue;
                return;
            }

            // ==========================================
            // الخطوة 5: التعامل مع القواميس (MAP)
            // ==========================================
            if (objectValue.isMap())
            {
                std::string key = indexValue.toString();
                auto map = objectValue.toMap();
                map[key] = newValue;

                // (AR) كتابة رجوعية تكرارية تدعم أي مستوى تداخل
                // (EN) Recursive writeback supports any nesting depth
                writeBackChain(node.object.get(), Value(map));

                lastResult_ = newValue;
                return;
            }

            // ==========================================
            // الخطوة 5.5: التعامل مع الكائنات (عامل []= المحمل زائداً)
            // ==========================================
            if (objectValue.isObject() || objectValue.isObjectLike())
            {
                std::string className = objectValue.getClassName();
                if (!className.empty())
                {
                    auto *classManager = Data::ClassManager::getInstance();
                    Data::ClassType *classType = classManager->getClass(className);
                    if (classType)
                    {
                        Data::OperatorOverload *opOverload = classType->findOperator("[]=");
                        if (opOverload && opOverload->body)
                        {
                            lastResult_ = executeIndexAssignOperatorOverload(objectValue, *opOverload, indexValue, newValue, node.position);
                            return;
                        }
                    }
                }
            }

            ::Sad::Errors::throwRuntime(
                ::Sad::Errors::ErrorCode::RUN_INDEX_ASSIGN_TYPE_INVALID,
                node.position,
                {{"type", objectValue.getTypeName()}});
        }

        // =========================================================================
        // (AR) تقييم تعبير Walrus / (EN) Walrus Expression Evaluation
        // =========================================================================

        /**
         * @brief (AR) تقييم عامل Walrus - التعيين داخل التعبير
         *        (EN) Evaluate walrus operator - assignment within expression
         *
         * @param node (AR) عقدة تعبير Walrus / (EN) Walrus expression node
         *
         * @details
         *   (AR) عامل Walrus (:=) يسمح بالتعيين داخل التعبير ويُرجع القيمة المُعيّنة
         *   (EN) Walrus operator (:=) allows assignment within expression and returns assigned value
         *
         *   Examples / أمثلة:
         *   - if (n := len(items)) > 10: print(n)
         *   - while (line := file.read()): process(line)
         */
        void ExpressionEvaluator::visitWalrusExpr(WalrusExpr &node)
        {
            // (AR) تقييم القيمة المراد تعيينها / (EN) Evaluate the value to assign
            node.value->accept(*this);
            Value assignedValue = lastResult_;

            // (AR) محاولة تعيين القيمة للمتغير — أو تعريفه إن لم يوجد
            // (EN) Try to assign value to variable — or define it if not found
            // (AR) تحسين أداء: بحث واحد بدلاً من اثنين
            // (EN) Performance: single lookup instead of two
            variableManager_.defineOrAssign(node.variable, assignedValue);

            // (AR) إرجاع القيمة المُعيّنة (هذا هو سلوك Walrus)
            // (EN) Return the assigned value (this is walrus behavior)
            lastResult_ = assignedValue;
        }

        // =========================================================================
        // (AR) تقييم دالة Lambda / (EN) Lambda Function Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitLambdaExpr(LambdaExpr &node)
        {
            // (AR) إنشاء اسم فريد للدالة Lambda / (EN) Generate unique name for lambda function
            static std::atomic<int> lambdaCounter{0};
            std::string lambdaName = "__lambda_" + std::to_string(lambdaCounter.fetch_add(1));

            // (AR) تحويل المعاملات من AST Parameter إلى FunctionParameter
            // (EN) Convert parameters from AST Parameter to FunctionParameter
            auto dataTypeToString = [](Types::SadTypeKind type) -> std::string
            {
                switch (type)
                {
                case Types::SadTypeKind::Integer:
                    return "integer";
                case Types::SadTypeKind::Float:
                    return "float";
                case Types::SadTypeKind::String:
                    return "string";
                case Types::SadTypeKind::Boolean:
                    return "boolean";
                case Types::SadTypeKind::Void:
                    return "none";
                case Types::SadTypeKind::Array:
                    return "array";
                case Types::SadTypeKind::Map:
                    return "map";
                case Types::SadTypeKind::Function:
                    return "function";
                case Types::SadTypeKind::Class:
                    return "object";
                default:
                    return "unknown";
                }
            };

            std::vector<Data::FunctionParameter> params;
            for (const auto &param : node.parameters)
            {
                params.push_back(Data::FunctionParameter(
                    param.name,
                    dataTypeToString(param.type)));
            }

            // (AR) تحويل جسم Lambda إلى ASTNode — upcast آمن
            // (EN) Convert lambda body to ASTNode — safe upcast
            // (AR) إذا كان جسم كتلي (دالة مجهولة)، استخدمه. وإلا استخدم التعبير.
            // (EN) If block body (anonymous function), use it. Otherwise use expression body.
            std::shared_ptr<AST::ASTNode> bodyNode;
            if (node.blockBody)
            {
                bodyNode = std::shared_ptr<AST::ASTNode>(
                    static_cast<AST::ASTNode *>(node.blockBody.get()),
                    [](AST::ASTNode *) {} // Empty deleter - AST owns the memory
                );
            }
            else
            {
                bodyNode = std::shared_ptr<AST::ASTNode>(
                    static_cast<AST::ASTNode *>(node.body.get()),
                    [](AST::ASTNode *) {} // Empty deleter - AST owns the memory
                );
            }

            // (AR) تسجيل Lambda كدالة في FunctionManager
            // (EN) Register lambda as function in FunctionManager
            functionManager_.defineFunction(lambdaName, params, bodyNode);

            // (AR) التقاط المتغيرات من النطاق الحالي للإغلاقات
            // (EN) Capture variables from current scope for closures
            {
                auto func = functionManager_.getFunction(lambdaName, params.size());
                if (func)
                {
                    std::unordered_map<std::string, Data::Value> captures;
                    auto varNames = variableManager_.getVariableNames();
                    for (const auto &vname : varNames)
                    {
                        // (AR) لا نلتقط اسم اللامدا نفسها ولا المعاملات
                        // (EN) Don't capture the lambda name itself or parameter names
                        bool isParam = false;
                        for (const auto &p : params)
                        {
                            if (p.name == vname)
                            {
                                isParam = true;
                                break;
                            }
                        }
                        if (!isParam && vname != lambdaName)
                        {
                            try
                            {
                                captures[vname] = variableManager_.get(vname);
                            }
                            catch (const std::exception &)
                            {
                                // (AR) متوقع: بعض المتغيرات قد لا تكون متاحة للالتقاط
                                // (EN) Expected: some variables may not be accessible for capture
                            }
                        }
                    }
                    func->setCaptures(captures);
                }
            }

            // (AR) إرجاع مرجع الدالة كقيمة من نوع FUNCTION
            // (EN) Return function reference as FUNCTION type value
            auto funcRef = std::make_shared<Data::FunctionRef>(
                lambdaName,                                                                                                      // displayName
                lambdaName,                                                                                                      // registeredName
                Data::FunctionRefKind::LAMBDA,                                                                                   // kind
                ::Sad::Security::SafeArithmetic::assertSafeCast<int>(params.size(), "expression_evaluator_members_assign_size"), // arity
                std::vector<std::string>()                                                                                       // parameterNames (filled below)
            );
            // (AR) ملء أسماء المعاملات / (EN) Fill parameter names
            for (const auto &p : params)
            {
                funcRef->parameterNames.push_back(p.name);
            }
            lastResult_ = Value(std::move(funcRef));
        }

        // =========================================================================
        // (AR) تقييم استيعاب قائمي / (EN) List Comprehension Evaluation
        // =========================================================================

    } // namespace Interpreter
} // namespace Sad
