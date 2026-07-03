/**
 * @file interpreter_core.h
 * @brief (AR) المفسر الأساسي - يدير تنفيذ برامج لغة ص
 * @brief (EN) Core Interpreter - manages execution of S Language programs
 *
 * الوصف بالعربية:
 * =================
 * المفسر الأساسي هو نقطة الدخول الرئيسية لتنفيذ برامج لغة ص.
 * يُنسق بين جميع المكونات:
 * - إدارة المتغيرات (VariableManager)
 * - إدارة الدوال (FunctionManager)
 * - إدارة النطاقات (ScopeManager)
 * - تقييم التعابير (ExpressionEvaluator)
 * - تنفيذ الجُمل (StatementExecutor)
 *
 * المميزات:
 * - واجهة بسيطة: execute(AST)
 * - إدارة تلقائية للموارد
 * - رسائل خطأ واضحة ثنائية اللغة
 * - دعم كامل لجميع ميزات اللغة
 *
 * English Description:
 * ====================
 * The Core Interpreter is the main entry point for executing S Language programs.
 * It coordinates all components:
 * - Variable Management (VariableManager)
 * - Function Management (FunctionManager)
 * - Scope Management (ScopeManager)
 * - Expression Evaluation (ExpressionEvaluator)
 * - Statement Execution (StatementExecutor)
 *
 * Features:
 * - Simple interface: execute(AST)
 * - Automatic resource management
 * - Clear bilingual error messages
 * - Full support for all language features
 *
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#pragma once

#include "ast_node.h"
#include "statements.h"
#include "value.h"
#include "variable_manager.h"
#include "function_manager.h"
#include "scope_manager.h"
#include "ownership_manager.h"
#include "module_resolver.h"
#include "memory/policy/gc_mode.h" // (AR) سياسة الذاكرة الموحَّدة (Phase A2) / (EN) Unified memory policy
#include "../visitors/expression_evaluator.h"
#include "../visitors/statement_executor.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {

        /**
         * @brief (AR) خيارات المفسر
         * @brief (EN) Interpreter options
         */
        struct InterpreterOptions
        {
            bool enableDebugMode = false;        ///< (AR) تفعيل وضع التصحيح / (EN) Enable debug mode
            bool enableStrictMode = false;       ///< (AR) تفعيل الوضع الصارم / (EN) Enable strict mode
            bool printResults = false;           ///< (AR) طباعة النتائج / (EN) Print results
            size_t maxCallDepth = 1000;          ///< (AR) أقصى عمق للاستدعاءات / (EN) Maximum call depth
            bool enableOwnership = false;        ///< (AR) تفعيل نظام الملكية / (EN) Enable ownership system
            bool ownershipArabicMessages = true; ///< (AR) رسائل ملكية عربية / (EN) Arabic ownership messages
            bool ownershipDebugMode = false;     ///< (AR) تتبع الملكية / (EN) Ownership debug trace
            bool enableTypeCheck = false;        ///< (AR) تفعيل فحص الأنواع / (EN) Enable type checking
            bool typeCheckDebugMode = false;     ///< (AR) تنقيح فحص الأنواع / (EN) Type check debug mode
            bool typeCheckStrictMode = false;    ///< (AR) فحص أنواع صارم / (EN) Strict type checking
            bool enableSecurity = false;         ///< (AR) تفعيل نظام الأمان / (EN) Enable security system
            bool securityStrictMode = false;     ///< (AR) وضع الأمان الصارم / (EN) Strict security mode
            bool securityDebugMode = false;      ///< (AR) تنقيح الأمان / (EN) Security debug mode
            std::string currentFilePath;         ///< (AR) مسار الملف الحالي - لحل مسارات الاستيراد / (EN) Current file path - for import resolution
            bool enableHotReload = false;        ///< (AR) تفعيل إعادة التحميل الساخن / (EN) Enable hot reload

            // ========================================================================
            // (AR) سياسة الذاكرة الموحَّدة (Phase A2) — مصدر الحقيقة لـ --dev/--prod/--learn
            // (EN) Unified memory policy (Phase A2) — source of truth for --dev/--prod/--learn
            //
            // (AR) عند تمرير إعدادات سياسة، يقوم InterpreterCore بضبط صرامة
            //      OwnershipManager تلقائياً (يتجاوز enableOwnership أعلاه إذا كانت
            //      OwnershipMode != Disabled).
            // (EN) When provided, InterpreterCore auto-adjusts OwnershipManager
            //      strictness (overrides enableOwnership above if OwnershipMode != Disabled).
            // ========================================================================
            ::Sad::Memory::MemoryModeSettings memoryPolicy{}; ///< (AR) سياسة الذاكرة / (EN) Memory mode policy
            bool memoryPolicySet = false;                     ///< (AR) هل ضُبطت السياسة من CLI؟ / (EN) Was policy explicitly set from CLI?
        };

        /**
         * @brief (AR) نتيجة التنفيذ
         * @brief (EN) Execution result
         */
        struct ExecutionResult
        {
            bool success = false;     ///< (AR) نجح التنفيذ / (EN) Execution succeeded
            Data::Value result;       ///< (AR) النتيجة / (EN) Result value
            std::string errorMessage; ///< (AR) رسالة الخطأ / (EN) Error message

            // (AR) Phase E-3 / Phase 4 — معلومات الخطأ المنظمة لتمريرها لـ ErrorCatalog
            // (EN) Phase E-3 / Phase 4 — structured error info to route via ErrorCatalog
            int errorCode = 0;      ///< (AR) رمز الخطأ (Sad::Errors::ErrorCode) — 0 = غير محدد
            size_t errorLine = 0;   ///< (AR) سطر الخطأ (1-based)
            size_t errorColumn = 0; ///< (AR) عمود الخطأ (1-based)
            std::string errorKind;  ///< (AR) اسم الاستثناء (DivisionByZeroError ...)

            ExecutionResult() = default;

            explicit ExecutionResult(bool s, const Data::Value &r = Data::Value(), const std::string &err = "")
                : success(s), result(r), errorMessage(err) {}
        };

        /**
         * @brief (AR) المفسر الأساسي - ينفذ برامج لغة ص
         * @brief (EN) Core Interpreter - executes S Language programs
         *
         * هذا الصنف يُنسق بين جميع مكونات المفسر لتنفيذ شجرة AST
         * This class coordinates all interpreter components to execute an AST
         *
         * @example مثال الاستخدام / Usage Example:
         * @code{.cpp}
         * using namespace Sad::Interpreter;
         *
         * // إنشاء مفسر
         * Interpreter interpreter;
         *
         * // تنفيذ AST
         * auto result = interpreter.execute(programAST);
         *
         * if (result.success) {
         *     std::cout << "النتيجة: " << result.result.toString() << std::endl;
         * } else {
         *     std::cerr << "خطأ: " << result.errorMessage << std::endl;
         * }
         * @endcode
         */
        class Interpreter
        {
        public:
            /**
             * @brief (AR) البناء - ينشئ مفسر جديد
             * @brief (EN) Constructor - creates new interpreter
             *
             * @param options (AR) خيارات المفسر / (EN) Interpreter options
             */
            explicit Interpreter(const InterpreterOptions &options = InterpreterOptions());

            /**
             * @brief (AR) تنفيذ برنامج من AST
             * @brief (EN) Execute program from AST
             *
             * @param program (AR) برنامج (قائمة جمل) / (EN) Program (list of statements)
             * @return (AR) نتيجة التنفيذ / (EN) Execution result
             */
            ExecutionResult execute(const std::vector<std::unique_ptr<AST::Statement>> &program);

            /**
             * @brief (AR) تنفيذ جملة واحدة
             * @brief (EN) Execute single statement
             *
             * @param statement (AR) الجملة / (EN) Statement to execute
             * @return (AR) نتيجة التنفيذ / (EN) Execution result
             */
            ExecutionResult executeStatement(AST::Statement &statement);

            /**
             * @brief (AR) تقييم تعبير
             * @brief (EN) Evaluate expression
             *
             * @param expression (AR) التعبير / (EN) Expression to evaluate
             * @return (AR) القيمة الناتجة / (EN) Resulting value
             */
            Data::Value evaluateExpression(AST::Expression &expression);

            /**
             * @brief (AR) إعادة تعيين المفسر (مسح جميع المتغيرات والدوال)
             * @brief (EN) Reset interpreter (clear all variables and functions)
             */
            void reset();

            /**
             * @brief (AR) الحصول على مدير المتغيرات
             * @brief (EN) Get variable manager
             */
            Data::VariableManager &getVariableManager() { return *variableManager_; }

            /**
             * @brief (AR) الحصول على مدير الدوال
             * @brief (EN) Get function manager
             */
            Data::FunctionManager &getFunctionManager() { return *functionManager_; }

            /**
             * @brief (AR) الحصول على مدير النطاقات
             * @brief (EN) Get scope manager
             */
            Data::ScopeManager &getScopeManager() { return *scopeManager_; }

            /**
             * @brief (AR) الحصول على مدير الملكية
             * @brief (EN) Get ownership manager
             */
            Data::OwnershipManager &getOwnershipManager() { return *ownershipManager_; }

            /**
             * @brief (AR) الحصول على محلل الوحدات
             * @brief (EN) Get module resolver
             */
            Modules::ModuleResolver &getModuleResolver() { return *moduleResolver_; }

            /**
             * @brief (AR) الحصول على الخيارات
             * @brief (EN) Get options
             */
            const InterpreterOptions &getOptions() const { return options_; }

            /**
             * @brief (AR) تعيين خيارات جديدة
             * @brief (EN) Set new options
             */
            void setOptions(const InterpreterOptions &options) { options_ = options; }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) استدعاء دالة مستخدم من C++ — يُستخدم للإطار التفاعلي
            // (EN) Call a user function from C++ — used for reactive framework
            // ═══════════════════════════════════════════════════════════════════

            /**
             * @brief (AR) استدعاء دالة معرّفة في لغة ص من كود C++
             * @brief (EN) Call a Sad-defined function from C++ code
             *
             * هذه الدالة تُمكّن النظام التفاعلي من استدعاء دوال البناء
             * ومعالجات الأحداث المكتوبة بلغة ص من داخل محرك C++.
             * تدعم إعادة الدخول (re-entrant) — يمكن استدعاؤها من داخل
             * دالة مضمنة تعمل حالياً.
             *
             * This function enables the reactive system to call build functions
             * and event handlers written in Sad from within the C++ engine.
             * Supports re-entrant calls — can be called from within a currently
             * running builtin function.
             *
             * @param funcName (AR) اسم الدالة / (EN) Function name
             * @param args (AR) المعاملات / (EN) Arguments
             * @return (AR) القيمة المُرجعة / (EN) Return value
             */
            Data::Value callUserFunction(const std::string &funcName,
                                         const std::vector<Data::Value> &args = {});

            // ═══════════════════════════════════════════════════════════════════
            // (AR) استدعاء دالة على كائن — يُستخدم لنظام حالة_عنصر (StatefulWidget)
            // (EN) Call a method on an object — used for StatefulWidget system
            // ═══════════════════════════════════════════════════════════════════

            /**
             * @brief (AR) استدعاء طريقة على كائن محدد من كود C++
             * @brief (EN) Call a method on a specific object from C++ code
             *
             * هذه الدالة تُمكّن محرك الواجهات من استدعاء دالة بناء() على
             * كائنات حالة_عنصر. تُنشئ نطاقاً جديداً وتُعرّف هذا (this)
             * ثم تُنفّذ جسم الطريقة وتُرجع النتيجة.
             *
             * This function enables the UI engine to call build() on
             * StatefulWidget objects. Creates a new scope, defines this,
             * executes the method body and returns the result.
             *
             * @param obj (AR) مؤشر الكائن / (EN) Object pointer
             * @param methodName (AR) اسم الطريقة / (EN) Method name
             * @param args (AR) المعاملات / (EN) Arguments
             * @return (AR) القيمة المُرجعة / (EN) Return value
             */
            Data::Value callMethodOnObject(
                Data::ObjectInstance *obj,
                const std::string &methodName,
                const std::vector<Data::Value> &args = {});

            // ═══════════════════════════════════════════════════════════════════
            // (AR) تنفيذ دالة مسجّلة في سياق كائن محدد
            // (EN) Execute a registered function in the context of a specific object
            // ═══════════════════════════════════════════════════════════════════

            /**
             * @brief (AR) تنفيذ دالة (مسجّلة في FunctionManager) في سياق كائن
             * @brief (EN) Execute a registered function in object context
             *
             * (AR) يجمع بين callUserFunction وcallMethodOnObject:
             *      يبحث عن الدالة بالاسم في FunctionManager، ثم يُنشئ نطاقاً
             *      يحتوي على هذا + حقول الكائن، ويُنفذ الدالة، وينسخ
             *      التعديلات على الحقول بعد التنفيذ.
             *      يُستخدم لمعالجات أحداث الواجهة التي تحتاج الوصول لحالة المكون.
             *
             * (EN) Combines callUserFunction and callMethodOnObject:
             *      Finds function by name in FunctionManager, creates a scope
             *      with هذا + object fields, executes, and copies modifications
             *      back. Used for UI event handlers that need component state access.
             *
             * @param funcName اسم الدالة المسجلة
             * @param obj الكائن المالك
             * @param args المعاملات
             * @return القيمة المُرجعة
             */
            Data::Value callFunctionInObjectContext(
                const std::string &funcName,
                Data::ObjectInstance *obj,
                const std::vector<Data::Value> &args = {});

        private:
            // (AR) خيارات المفسر / (EN) Interpreter options
            InterpreterOptions options_;

            // (AR) هل سُجِّل تعداد الألوان المدمَج «ألوان»؟ (مرّة واحدة لكلّ مفسّر).
            // (EN) Was the builtin `ألوان` color enum registered? (once per instance).
            bool builtinColorsRegistered_ = false;

            // (AR) يسجّل تعداد الألوان المدمَج بتحليل بادئته المولَّدة وتنفيذها مرّةً.
            // (EN) Registers the builtin color enum by parsing+executing its prelude once.
            void ensureBuiltinColorsRegistered();

            // (AR) المديرون / (EN) Managers
            std::shared_ptr<Data::ScopeManager> scopeManager_;
            std::shared_ptr<Data::VariableManager> variableManager_;
            std::shared_ptr<Data::FunctionManager> functionManager_;
            std::shared_ptr<Data::OwnershipManager> ownershipManager_;

            // (AR) محلل الوحدات - لحل مسارات الاستيراد والتصدير / (EN) Module resolver - for import/export path resolution
            std::shared_ptr<Modules::ModuleResolver> moduleResolver_;

            // (AR) المنفذون / (EN) Executors
            std::unique_ptr<ExpressionEvaluator> expressionEvaluator_;
            std::unique_ptr<StatementExecutor> statementExecutor_;

            /**
             * @brief (AR) تهيئة المكونات
             * @brief (EN) Initialize components
             */
            void initializeComponents();
        };

        // Forward declaration for built-in function registration
        // (AR) تصريح تقديمي لتسجيل الدوال المضمنة
        // (EN) Forward declaration for built-in function registration
        void registerBuiltinFunctions(Interpreter &interpreter);

    } // namespace Interpreter
} // namespace Sad
