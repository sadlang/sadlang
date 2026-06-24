/**
 * @file statement_executor_functions_templates.cpp
 * @brief (AR) تنفيذ namespace/operator/template/unsafe/comptime/volatile في StatementExecutor
 * @brief (EN) namespace/operator/template/unsafe/comptime/volatile statement execution
 *
 * (AR) ملف مُستخرج لتقليل حجم statement_executor_functions.cpp وفق CW-05
 */

#include "statement_executor.h"
#include "declarations.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "error_manager.h"
#include "runtime_throw.h"
#include "user_thrown.h"
#include "object_instance.h"
#include "debug_server.h"
#include "class_manager.h"
#include "channel.h"
#include "sad_type_system.h"
#include "profiler_hooks.h" // (AR) خطافات مصحح الأداء / (EN) Profiler hooks
#include <iostream>
#include <sstream>
#include <map>

namespace Sad
{
    namespace Interpreter
    {

        // (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
        // (EN) Helper function to get filename from error manager
        static inline std::string getSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) خطاف التصحيح — يُستدعى قبل تنفيذ كل جملة
        // (EN) Debug hook — called before each statement execution
        // ═══════════════════════════════════════════════════════════════
        static inline void debugHook(const Sad::Lexer::Position &pos)
        {
            auto *dbg = Sad::Debug::DebugServer::getInstance();
            if (dbg && dbg->isConnected())
            {
                dbg->onBeforeStatement(
                    getSourceFilename(),
                    static_cast<int>(pos.line),
                    static_cast<int>(pos.column));
                if (dbg->shouldStop())
                {
                    throw std::runtime_error("تم فصل المصحح / Debugger disconnected");
                }
            }
        }
        void StatementExecutor::visitNamespaceDecl(AST::NamespaceDecl &node)
        {
            // ═══════════════════════════════════════════════════════════════════
            // (AR) تنفيذ فضاء الأسماء — يُنشئ كائن خريطة يحوي جميع الأعضاء المُصدّرة
            //      مما يتيح الوصول عبر: فضاء.دالة() أو فضاء.متغير أو فضاء.صنف
            //
            //      يدعم:
            //      • الدوال        → FunctionDecl  → FunctionRef في الخريطة
            //      • المتغيرات     → VarDeclStmt   → القيمة في الخريطة
            //      • الأصناف       → ClassDecl     → اسم الصنف كنص في الخريطة
            //      • الثوابت       → ConstDeclStmt → القيمة الثابتة في الخريطة
            //      • فضاء متداخل   → NamespaceDecl → خريطة فرعية في الخريطة
            //
            // (EN) Execute namespace — creates a MAP object containing all exported
            //      members, enabling access via: namespace.func(), namespace.var, namespace.Class
            // ═══════════════════════════════════════════════════════════════════

            // (AR) حفظ فضاء الأسماء السابق
            std::string previousNamespace = currentNamespace_;

            if (currentNamespace_.empty())
            {
                currentNamespace_ = node.name;
            }
            else
            {
                currentNamespace_ = currentNamespace_ + "::" + node.name;
            }

            // (AR) تحديد أسماء الأعضاء من عقد AST مباشرة — لا نعتمد على المقارنة
            //      لأن بعض الأسماء (مثل "جمع") قد تكون مسجلة كدوال مدمجة مسبقاً
            // (EN) Identify member names directly from AST nodes — don't rely on comparison
            //      because some names (like "جمع") may already be registered as builtins
            std::vector<std::string> nsFuncNames;
            std::vector<std::string> nsVarNames;
            std::vector<std::string> nsClassNames;
            std::vector<std::string> nsNestedNsNames;

            for (auto &stmt : node.members)
            {
                if (auto *funcDecl = dynamic_cast<AST::FunctionDecl *>(stmt.get()))
                {
                    nsFuncNames.push_back(funcDecl->name);
                }
                else if (auto *varDecl = dynamic_cast<AST::VarDeclStmt *>(stmt.get()))
                {
                    nsVarNames.push_back(varDecl->name);
                }
                else if (auto *classDecl = dynamic_cast<AST::ClassDecl *>(stmt.get()))
                {
                    nsClassNames.push_back(classDecl->name);
                }
                else if (auto *nsDecl = dynamic_cast<AST::NamespaceDecl *>(stmt.get()))
                {
                    nsNestedNsNames.push_back(nsDecl->name);
                }
            }

            // (AR) دخول نطاق جديد لعزل متغيرات هذا الفضاء
            //      نستخدم FUNCTION بدلاً من BLOCK لأن define() لا تتجاوز حدود FUNCTION
            //      وهذا يمنع تعارض الأسماء بين فضاءات متداخلة (مثل: فضاء خارجي وداخلي
            //      كلاهما يحوي متغير بنفس الاسم — كل واحد يحتفظ بقيمته)
            // (EN) Enter a new scope to isolate this namespace's variables.
            //      Using FUNCTION instead of BLOCK because define() doesn't cross FUNCTION
            //      boundaries. This prevents name collisions between nested namespaces
            //      (e.g., outer and inner both having "value" — each keeps its own)
            variableManager_.enterScope(Data::ScopeType::FUNCTION, "namespace:" + node.name);

            // (AR) تنفيذ جميع التصريحات داخل فضاء الأسماء في النطاق الحالي
            //      هذا يسجّل الدوال في FunctionManager، الأصناف في ClassManager،
            //      والفضاءات المتداخلة كمتغيرات (خرائط) في VariableManager
            // (EN) Execute all declarations inside the namespace in current scope
            //      This registers functions in FunctionManager, classes in ClassManager,
            //      and nested namespaces as variables (maps) in VariableManager
            for (auto &stmt : node.members)
            {
                if (stmt)
                {
                    stmt->accept(*this);
                }
                if (shouldStopExecution())
                {
                    break;
                }
            }

            // (AR) بناء خريطة فضاء الأسماء من الأعضاء المحددة مسبقاً
            // (EN) Build namespace map from pre-identified members
            Data::Value::MapType nsMap;

            // (AR) إضافة الدوال — كل دالة تُحوّل إلى FunctionRef Value
            // (EN) Add functions — each function is converted to a FunctionRef Value
            for (const auto &fname : nsFuncNames)
            {
                auto funcRef = std::make_shared<Data::FunctionRef>(
                    fname, fname, Data::FunctionRefKind::USER_DEFINED, -1);
                nsMap[fname] = Data::Value(funcRef);
            }

            // (AR) إضافة المتغيرات — نقرأ قيمها من VariableManager (في النطاق الحالي)
            // (EN) Add variables — read their values from VariableManager (in current scope)
            for (const auto &vname : nsVarNames)
            {
                const Data::Value *val = variableManager_.tryGet(vname);
                if (val)
                {
                    nsMap[vname] = *val;
                }
            }

            // (AR) إضافة الأصناف — كل صنف يُخزّن باسمه كنص حتى يمكن استخدامه مع جديد
            // (EN) Add classes — each class stored with its name as string for use with جديد
            for (const auto &cname : nsClassNames)
            {
                nsMap[cname] = Data::Value(cname);
            }

            // (AR) إضافة الفضاءات المتداخلة — مسجّلة كمتغيرات (خرائط) بعد تنفيذها
            // (EN) Add nested namespaces — registered as variables (maps) after execution
            for (const auto &nsName : nsNestedNsNames)
            {
                const Data::Value *val = variableManager_.tryGet(nsName);
                if (val)
                {
                    nsMap[nsName] = *val;
                }
            }

            // (AR) الخروج من نطاق الفضاء — يحذف جميع المتغيرات المحلية تلقائياً
            //      المتغيرات أصبحت محفوظة في nsMap ولا حاجة لحذفها يدوياً
            // (EN) Exit namespace scope — automatically cleans up all local variables.
            //      Variables are already saved in nsMap, no manual removal needed
            variableManager_.exitScope();

            // (AR) تسجيل فضاء الأسماء ككائن خريطة في النطاق الخارجي
            //      الآن يمكن الوصول عبر: node.name.عضو
            // (EN) Register namespace as a MAP object in the outer scope
            //      Now accessible via: node.name.member
            variableManager_.define(node.name, Data::Value(nsMap));

            // (AR) استعادة فضاء الأسماء السابق
            currentNamespace_ = previousNamespace;
        }

        void StatementExecutor::visitOperatorDecl(AST::OperatorDecl &node)
        {
            // (AR) تسجيل حمل العامل الزائد — يُرمى خطأ إذا ظهر خارج صنف
            // (EN) Register operator overload — throws error if appears outside class context
            // (AR) ملاحظة: التسجيل الفعلي يتم في visitClassDecl عند تحليل أعضاء الصنف.
            // (EN) Note: Actual registration happens in visitClassDecl when parsing class members.
            ::Sad::Errors::throwRuntime(
                ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                node.position,
                {{"type", "context"}, {"operation", node.operatorSymbol}});
        }

        // =========================================================================
        // (AR) تنفيذ القوالب (Templates) / (EN) Templates Implementation
        // =========================================================================

        /**
         * @brief (AR) يُنفذ تصريح دالة قالب
         *        (EN) Executes template function declaration
         *
         * @details
         * يحفظ الدالة القالب في مدير القوالب للاستخدام لاحقاً عند الاستدعاء.
         * Stores the template function in template manager for later instantiation.
         */
        void StatementExecutor::visitTemplateFunctionDecl(AST::TemplateFunctionDecl &node)
        {
            // (AR) حفظ دالة القالب في FunctionManager للاستدعاء لاحقاً
            // (EN) Store template function for later instantiation

            // (AR) حفظ مؤشر AST للتحقق من القيود لاحقاً
            // (EN) Store AST pointer for constraint verification later
            templateFunctions_[node.name] = &node;

#ifdef DEBUG
            std::cout << "[Template] تسجيل دالة قالب: " << node.functionName << std::endl;
            std::cout << "[Template] معاملات الأنواع: ";
            for (const auto &typeParam : node.typeParameters)
            {
                std::cout << typeParam.name << " ";
            }
            std::cout << std::endl;
#endif

            // (AR) Helper function لتحويل DataType إلى string
            // (EN) Helper to convert DataType to string
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
                case Types::SadTypeKind::Tuple:
                    return "tuple";
                case Types::SadTypeKind::Function:
                    return "function";
                case Types::SadTypeKind::Class:
                    return "object";
                case Types::SadTypeKind::Enum:
                    return "enum";
                case Types::SadTypeKind::Byte:
                    return "byte";
                default:
                    return "unknown";
                }
            };

            // (AR) تحويل المعاملات إلى تنسيق FunctionManager
            // (EN) Convert parameters to FunctionManager format
            std::vector<Data::FunctionParameter> params;
            for (const auto &param : node.parameters)
            {
                bool hasDefault = (param.defaultValue != nullptr);
                std::string defaultValStr = "";

                if (hasDefault)
                {
                    defaultValStr = "<from_ast>";
                }

                params.push_back(Data::FunctionParameter(
                    param.name,
                    dataTypeToString(param.type),
                    hasDefault,
                    defaultValStr,
                    param.typeName // (AR) اسم الصنف إذا كان المعامل من نوع OBJECT / (EN) Class name if OBJECT
                    ));
            }

            // (AR) حفظ الدالة القالب - سنستخدم اسم خاص يحتوي على علامة قالب
            // (EN) Save template function - use special name with template marker
            // Format: __template_<name>_<type_params>
            std::string templateKey = "__template_" + node.name;

            std::shared_ptr<AST::ASTNode> bodyNode(
                static_cast<AST::ASTNode *>(node.body.get()),
                [](AST::ASTNode *) {});

            std::shared_ptr<AST::ASTNode> declNode(
                static_cast<AST::ASTNode *>(&node),
                [](AST::ASTNode *) {});

            // (AR) محو نوع المعاملات الجنيسة قبل التسجيل (للمسارين: التخصيص
            //      الصريح `هوية<رقم>(7)` والاستدعاء العاديّ `هوية(7)`). المعامل
            //      المكتوب بمعامِل-نوعيّ (مثل «ت») يقبل أيّ وسيط وقت التشغيل،
            //      فنُفرّغ اسم الصنف كي لا يُطلَق فحص الصنف الصارم
            //      RUN_TYPE_CHECK_FAILED على وسيط بدائيّ — جوهر محو النوع في
            //      مفسّر ديناميكيّ (يطابق حَوْصلة المترجم).
            // (EN) Erase generic parameter type names before registration (for
            //      both paths: explicit `هوية<رقم>(7)` and plain `هوية(7)`). A
            //      param typed by a type-parameter (e.g. «ت») accepts any runtime
            //      argument, so we clear its class name to avoid the strict
            //      RUN_TYPE_CHECK_FAILED check on a primitive — the essence of
            //      type erasure in a dynamic interpreter (matches the compiler's
            //      monomorphization).
            for (auto &p : params)
            {
                for (const auto &tp : node.typeParameters)
                {
                    if (p.typeName == tp.name)
                    {
                        p.typeName.clear();
                        break;
                    }
                }
            }

            functionManager_.defineFunction(templateKey, params, bodyNode, declNode);

            // (AR) حفظ نوع الإرجاع
            // (EN) Save return type
            functionReturnTypes_[templateKey] = node.returnType;

            // ================================================================
            // (AR) تسجيل إضافيّ تحت الاسم الصريح لتمكين الاستدعاء العاديّ بمحو
            //      النوع (type erasure): المفسّر ديناميكيّ الأنواع، فجسم القالب
            //      يُنفَّذ كدالة عاديّة عند `هوية(7)` بلا وسائط نوع صريحة —
            //      مطابقةً للمترجم الذي يُحَوْصِل القالب (monomorphize). لا نطمس
            //      دالة غير-قالبيّة تحمل الاسم نفسه إن وُجدت.
            // (EN) Also register under the plain name so a normal call like
            //      `هوية(7)` (no explicit type args) runs via type erasure. The
            //      interpreter is dynamically typed, so a generic body executes
            //      as an ordinary function — matching the compiler which
            //      monomorphizes templates. Do not shadow an existing
            //      non-template function with the same name.
            // ================================================================
            if (!functionManager_.hasFunction(node.name, static_cast<int>(params.size())))
            {
                functionManager_.defineFunction(node.name, params, bodyNode, declNode);
                functionReturnTypes_[node.name] = node.returnType;
            }
        }

        /**
         * @brief (AR) يُنفذ تصريح صنف قالب
         *        (EN) Executes template class declaration
         *
         * @details
         * يحفظ الصنف القالب في مدير الأصناف للاستخدام لاحقاً عند الإنشاء.
         * Stores the template class in class manager for later instantiation.
         */
        void StatementExecutor::visitTemplateClassDecl(AST::TemplateClassDecl &node)
        {
            // (AR) حفظ صنف القالب للإنشاء لاحقاً
            // (EN) Store template class for later instantiation

#ifdef DEBUG
            std::cout << "[Template] تسجيل صنف قالب: " << node.name << std::endl;
            std::cout << "[Template] معاملات الأنواع: ";
            for (const auto &typeParam : node.typeParameters)
            {
                std::cout << typeParam.name << " ";
            }
            std::cout << std::endl;
#endif

            // (AR) حفظ مؤشر للعقدة الأصلية في خريطة القوالب
            // (EN) Store pointer to original AST node in template map
            templateClasses_[node.name] = &node;
        }

        // ======================================================================
        // (AR) زوار التوجيهات @ — Directive Visitors
        // ======================================================================

        /**
         * @brief (AR) @غير_آمن — ينفذ الجسم في وضع غير آمن (بدون قيود)
         * @brief (EN) @unsafe — executes body in unsafe mode (no restrictions)
         *
         * (AR) في المفسر، هذا ينفذ الجسم مباشرة. القيود الحقيقية تكون في المترجم.
         * (EN) In interpreter, this just executes the body. Real restrictions are in compiler.
         */
        void StatementExecutor::visitUnsafeBlockStmt(AST::UnsafeBlockStmt &node)
        {
            // (AR) تنفيذ جسم الكتلة في نطاق جديد
            // (EN) Execute block body in a new scope
            variableManager_.enterScope(Data::ScopeType::BLOCK);

            for (auto &stmt : node.body)
            {
                if (stmt)
                {
                    stmt->accept(*this);
                }
            }

            variableManager_.exitScope();
        }

        /**
         * @brief (AR) @وقت_الترجمة — ينفذ الجسم فوراً (في المفسر: تنفيذ عادي)
         * @brief (EN) @comptime — executes body immediately (in interpreter: normal execution)
         *
         * (AR) في المفسر: يُنفَّذ كأي كتلة عادية. في المترجم: يُنفَّذ وقت الترجمة.
         * (EN) In interpreter: runs as a normal block. In compiler: evaluated at compile time.
         */
        void StatementExecutor::visitComptimeBlockStmt(AST::ComptimeBlockStmt &node)
        {
            // (AR) نفس سلوك الكتلة العادية في المفسر
            // (EN) Same behavior as regular block in interpreter
            variableManager_.enterScope(Data::ScopeType::BLOCK);

            for (auto &stmt : node.body)
            {
                if (stmt)
                {
                    stmt->accept(*this);
                }
            }

            variableManager_.exitScope();
        }

        /**
         * @brief (AR) @متطاير — يعلن متغيراً مع تعليمات volatile
         * @brief (EN) @volatile — declares a variable with volatile semantics
         *
         * (AR) في المفسر: يُعلن المتغير بشكل عادي. الفائدة الحقيقية في المترجم (يمنع التحسين).
         * (EN) In interpreter: declares variable normally. Real benefit is in compiler (prevents optimization).
         */
        void StatementExecutor::visitVolatileVarDeclStmt(AST::VolatileVarDeclStmt &node)
        {
            // (AR) ننفذ الإعلان الأصلي
            // (EN) Execute the underlying declaration
            if (node.declaration)
            {
                node.declaration->accept(*this);
            }
        }

        // ======================================================================
        // (AR) التحقق من قيود القوالب / (EN) Template Constraint Verification
        // ======================================================================

        void StatementExecutor::verifyTemplateConstraints(
            const std::vector<AST::TypeParameter> &typeParams,
            const std::vector<std::string> &typeArgNames,
            const AST::WhereClause &whereClause,
            const std::string &templateName,
            const Lexer::Position &pos)
        {

            auto *classManager = Data::ClassManager::getInstance();

            // (AR) بناء خريطة اسم_المعامل → اسم_النوع_الفعلي
            // (EN) Build map: param_name → actual_type_name
            std::unordered_map<std::string, std::string> typeMapping;
            for (size_t i = 0; i < typeParams.size() && i < typeArgNames.size(); ++i)
            {
                typeMapping[typeParams[i].name] = typeArgNames[i];
            }

            // (AR) التحقق من قيود المعاملات المباشرة
            // (EN) Check direct parameter constraints
            for (size_t i = 0; i < typeParams.size() && i < typeArgNames.size(); ++i)
            {
                const auto &param = typeParams[i];
                const std::string &argName = typeArgNames[i];

                for (const auto &constraintName : param.constraints)
                {
                    if (constraintName.empty())
                        continue;

                    // (AR) تحقق: هل القيد سمة مسجلة؟
                    // (EN) Check: is the constraint a registered trait?
                    if (!classManager->hasTrait(constraintName))
                    {
                        // (AR) السمة غير مسجلة — لا يمكن التحقق
                        // (EN) Trait not registered — can't verify
                        continue;
                    }

                    // (AR) تحقق: هل النوع الفعلي صنف مسجل؟
                    // (EN) Check: is the actual type a registered class?
                    if (!classManager->hasClass(argName))
                    {
                        // (AR) النوع ليس صنفاً مسجلاً (ربما نوع مدمج) — لا يمكن التحقق
                        // (EN) Type is not a registered class (maybe built-in) — can't verify
                        continue;
                    }

                    // (AR) التحقق: هل الصنف ينفذ السمة المطلوبة؟
                    // (EN) Verify: does the class implement the required trait?
                    if (!classManager->classImplementsTrait(argName, constraintName))
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_CONTRACT_WHERE_FAILED,
                            pos,
                            {{"function", templateName}, {"constraint", constraintName + " on " + argName}});
                    }
                }
            }

            // (AR) التحقق من قيود where clause
            // (EN) Check where clause constraints
            if (!whereClause.empty())
            {
                for (const auto &item : whereClause.items)
                {
                    // (AR) البحث عن النوع الفعلي للمعامل
                    // (EN) Find the actual type for the parameter
                    auto it = typeMapping.find(item.typeName);
                    if (it == typeMapping.end())
                        continue;

                    const std::string &actualType = it->second;

                    for (const auto &constraintName : item.constraints)
                    {
                        if (constraintName.empty())
                            continue;

                        if (!classManager->hasTrait(constraintName))
                            continue;
                        if (!classManager->hasClass(actualType))
                            continue;

                        if (!classManager->classImplementsTrait(actualType, constraintName))
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_CONTRACT_WHERE_FAILED,
                                pos,
                                {{"function", templateName + " (where)"}, {"constraint", constraintName + " on " + actualType}});
                        }
                    }
                }
            }
        }

    } // namespace Interpreter
} // namespace Sad
