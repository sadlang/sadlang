/**
 * @file interpreter_core.cpp
 * @brief (AR) تنفيذ المفسر الأساسي
 * @brief (EN) Core Interpreter Implementation
 *
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include <string>
#include "interpreter_core.h"
#include "declarations.h"
#include "statements.h"
#include "module_nodes.h"
#include "error_manager.h"
#include "source_location.h"
#include "error_codes.h"
#include "object_instance.h"

// (AR) فاحص الأنواع المتقدم / (EN) Advanced Type Checker
// (AR) Phase 3 (F-01): نُقل من compiler/include/semantic/ إلى shared/semantic/
//      ليصبح طبقة دلالية مشتركة بين المفسر والمترجم.
// (EN) Phase 3 (F-01): moved from compiler/include/semantic/ to shared/semantic/
//      to become a semantic layer shared by interpreter and compiler.
#ifndef __EMSCRIPTEN__
#include "semantic/type_checker.h"
#endif

// (AR) المكتبة القياسية / (EN) Standard Library Manager
#include "stdlib_manager.h"

// (AR) دوال الإدخال والإخراج — لتسجيل callback عامل نص()
// (EN) I/O Functions — for registering نص() operator callback
#include "io/io_functions.h"

// (AR) مدير الأصناف — للوصول إلى عامل نص() من الـ callback
// (EN) Class manager — for accessing نص() operator from callback
#include "class_manager.h"

// (AR) Phase B-step3: محرك GC الموحَّد — يُفعَّل/يُعلَّق حسب gcStrategy
// (EN) Phase B-step3: unified GC engine — toggled per gcStrategy
#include "memory/gc/engine/garbage_collector.h"

#include <iostream>
#include <stdexcept>
#include <filesystem>

namespace Sad
{
    namespace Interpreter
    {

        // =========================================================================
        // (AR) البناء والإعداد / (EN) Construction and Setup
        // =========================================================================

        Interpreter::Interpreter(const InterpreterOptions &options)
            : options_(options)
        {
            initializeComponents();
        }

        void Interpreter::initializeComponents()
        {
            // (AR) إنشاء المديرين بالترتيب الصحيح / (EN) Create managers in correct order
            scopeManager_ = std::make_shared<Data::ScopeManager>();
            variableManager_ = std::make_shared<Data::VariableManager>(*scopeManager_);
            functionManager_ = std::make_shared<Data::FunctionManager>();

            // (AR) إنشاء مدير الملكية / (EN) Create ownership manager
            ownershipManager_ = std::make_shared<Data::OwnershipManager>();
            if (options_.enableOwnership)
            {
                ownershipManager_->enable();
                ownershipManager_->setArabicMessages(options_.ownershipArabicMessages);
                ownershipManager_->setDebugMode(options_.enableDebugMode || options_.ownershipDebugMode);
            }

            // ================================================================
            // (AR) Phase A2 — تطبيق سياسة الذاكرة الموحَّدة (--dev/--prod/--learn)
            // (EN) Phase A2 — apply unified memory policy (--dev/--prod/--learn)
            //
            // (AR) إذا ضبط المستخدم سياسة من سطر الأوامر، نمنحها الأولوية على
            //      enableOwnership: setStrictness يُفعّل/يُعطّل الفحص ويضبط
            //      علم warningsOnly لوضع التعلم.
            // (EN) If a CLI policy was provided, override enableOwnership:
            //      setStrictness enables/disables checks and sets warningsOnly
            //      for learn mode.
            // ================================================================
            if (options_.memoryPolicySet)
            {
                ownershipManager_->setArabicMessages(options_.ownershipArabicMessages);
                ownershipManager_->setDebugMode(options_.enableDebugMode || options_.ownershipDebugMode);
                ownershipManager_->setStrictness(options_.memoryPolicy.ownershipMode);

                // ============================================================
                // (AR) Phase B-step3 — تطبيق سياسة GC على المحرك الموحَّد
                //
                // الدلالات المعتمدة (بحسب docs/معمارية_الذاكرة_الموحدة.md):
                //   --dev   → gcStrategy = MarkAndSweep  → resume() (يعمل)
                //   --prod  → gcStrategy = None          → pause()  (مُعطَّل)
                //   --learn → gcStrategy = MarkAndSweep  → resume() (يعمل، تحذيرات فقط للملكية)
                //
                // (AR) ملاحظة هامة: المفسّر حالياً يُدير حياة الكائنات بـ
                //      shared_ptr<ObjectInstance>، لذلك المحرك لا يتعقَّب
                //      تخصيصات المفسّر مباشرة بعد. هذه الخطوة تُفعّل المحرك
                //      وتجعله جاهزاً لاستقبال تسجيلات لاحقة (B-step4: ربط
                //      ObjectInstance::ctor بـ registerObject والـ dtor بـ
                //      unregisterObject — تغيير غير عكوس على دلالات الذاكرة
                //      ولذلك نُؤجّله إلى مرحلة لها اختبار خاص).
                // (EN) Apply GC policy to the unified engine. The interpreter
                //      still owns object lifetime via shared_ptr; full hooking
                //      of allocations is deferred to B-step4.
                // ============================================================
                auto& gcEngine = Sad::Memory::GC::defaultEngine();
                if (options_.memoryPolicy.gcStrategy == Sad::Memory::GCStrategy::None)
                {
                    gcEngine.pause();
                }
                else
                {
                    gcEngine.resume();
                }

                if (options_.enableDebugMode)
                {
                    auto stats = gcEngine.getStats();
                    std::cout << "(AR) GC engine: paused=" << (stats.paused ? "true" : "false")
                              << " strategy=" << static_cast<int>(options_.memoryPolicy.gcStrategy)
                              << " / (EN) GC engine state applied" << std::endl;
                }

                // ============================================================
                // (AR) Phase B-step4 — ربط ObjectInstance بمحرك GC الموحَّد
                //
                // (AR) نُسجِّل خطّافَين على ObjectInstance يحوّلان كل إنشاء/هدم
                //      كائن إلى استدعاء على defaultEngine(). هذا يجعل
                //      `getStats().objectCount` يعكس فعلياً عدد الكائنات
                //      الحيّة في المفسّر — وهو الفرق الملموس بين --dev
                //      (يتعقَّب) و --prod (paused → return early داخل المحرك).
                //
                // (AR) ملاحظات معمارية مهمّة:
                //   1. shared/types لا تعتمد على shared/memory_gc — التسجيل
                //      هنا (في المفسّر) يُكسر هذه التبعية وقت التشغيل فقط.
                //   2. حياة ObjectInstance لا تزال shared_ptr — الـ hook
                //      إحصائي بحت ولا يحرّر الذاكرة. التحرير الفعلي عبر GC
                //      يحتاج تحويل الإدارة إلى raw ptr وهو عمل مرحلة لاحقة.
                //   3. عند gcStrategy == None نلغي تسجيل الـ hooks تماماً
                //      ليعود السلوك إلى مطابق-الأصل 100% (BF-15: توافق خلفي).
                //   4. التسجيل آمن للخيوط داخل ObjectInstance (mutex قصير +
                //      copy-then-invoke لتجنّب re-entrancy).
                //
                // (EN) Wire ObjectInstance lifecycle to the unified GC engine
                //      via runtime hooks (no compile-time dependency from
                //      shared/types on shared/memory_gc). Stats become real
                //      in --dev/--learn; --prod fully restores legacy behavior.
                // ============================================================
                if (options_.memoryPolicy.gcStrategy == Sad::Memory::GCStrategy::None)
                {
                    Sad::Data::ObjectInstance::clearHooks();
                }
                else
                {
                    Sad::Data::ObjectInstance::setAllocHook(
                        [](Sad::Data::ObjectInstance *obj, size_t size) {
                            // (AR) ترجمة المؤشر إلى void* لتجاوز نظام الأنواع
                            //      (المحرك يتعامل مع void* العام لأي نوع).
                            // (EN) Cast to void* — engine is type-agnostic.
                            Sad::Memory::GC::defaultEngine().registerObject(
                                static_cast<void *>(obj), size);
                        });

                    Sad::Data::ObjectInstance::setFreeHook(
                        [](Sad::Data::ObjectInstance *obj) {
                            Sad::Memory::GC::defaultEngine().unregisterObject(
                                static_cast<void *>(obj));
                        });
                }
            }

            // (AR) إنشاء محلل الوحدات لنظام الاستيراد والتصدير
            // (EN) Create module resolver for import/export system
            moduleResolver_ = std::make_shared<Modules::ModuleResolver>();

            // (AR) إضافة مسار الملف الحالي كمسار بحث للوحدات
            // (EN) Add current file path as module search path
            if (!options_.currentFilePath.empty())
            {
                auto parentDir = std::filesystem::path(options_.currentFilePath).parent_path();
                if (std::filesystem::exists(parentDir))
                {
                    moduleResolver_->addSearchPath(parentDir.string());
                }
            }

            // (AR) إنشاء منفذ العبارات أولاً / (EN) Create statement executor first
            statementExecutor_ = std::make_unique<StatementExecutor>(
                *variableManager_,
                *functionManager_,
                *scopeManager_,
                *ownershipManager_);

            // (AR) ربط محلل الوحدات بمنفذ العبارات
            // (EN) Connect module resolver to statement executor
            statementExecutor_->setModuleResolver(moduleResolver_.get());
            statementExecutor_->setCurrentFilePath(options_.currentFilePath);

            // (AR) ثم إنشاء مقيّم التعابير مع مرجع لمنفذ العبارات / (EN) Then create expression evaluator with statement executor reference
            expressionEvaluator_ = std::make_unique<ExpressionEvaluator>(
                *variableManager_,
                *functionManager_,
                *scopeManager_,
                *statementExecutor_,
                *ownershipManager_);

            // (AR) تسجيل جميع الدوال المضمنة / (EN) Register all built-in functions
            registerBuiltinFunctions(*this);

            // (AR) تسجيل callback لتحويل الكائنات إلى نص عبر عامل نص()
            // (EN) Register callback for object-to-string conversion via نص() operator
            {
                auto *exprEval = expressionEvaluator_.get();
                StdLib::IO::IOFunctions::setObjectToStringCallback(
                    [exprEval](const Data::Value &val) -> std::string
                    {
                        if (!val.isObject() && !val.isObjectLike())
                            return "";
                        std::string className = val.getClassName();
                        if (className.empty())
                            return "";
                        auto *classMgr = Data::ClassManager::getInstance();
                        if (!classMgr)
                            return "";
                        auto *classType = classMgr->getClass(className);
                        if (!classType)
                            return "";
                        auto *opOverload = classType->findOperator("\xd9\x86\xd8\xb5"); // "نص"
                        if (!opOverload || !opOverload->body)
                            return "";
                        try
                        {
                            Lexer::Position pos(0, 0);
                            Data::Value result = exprEval->executeUnaryOperatorOverload(val, *opOverload, pos);
                            return result.toString();
                        }
                        catch (...)
                        {
                            return "";
                        }
                    });
            }

            // (AR) تسجيل دوال المكتبة القياسية (رياضيات، نصوص، مصفوفات، أنواع)
            // (EN) Register standard library functions (math, string, array, type)
            {
                StdLib::StandardLibraryManager stdlibMgr(*functionManager_);
                stdlibMgr.registerAllFunctions();
            }

            if (options_.enableDebugMode)
            {
                std::cout << "(AR) تم تهيئة المفسر / (EN) Interpreter initialized" << std::endl;
            }
        }

        // =========================================================================
        // (AR) تنفيذ البرنامج / (EN) Program Execution
        // =========================================================================

        ExecutionResult Interpreter::execute(const std::vector<std::unique_ptr<AST::Statement>> &program)
        {
            try
            {
                if (options_.enableDebugMode)
                {
                    std::cout << "(AR) بدء تنفيذ البرنامج (" << program.size() << " جملة)"
                              << " / (EN) Starting program execution (" << program.size() << " statements)"
                              << std::endl;
                }

                // (AR) البحث عن الدالة الرئيسية / (EN) Search for main function
                AST::FunctionDecl *mainFunction = nullptr;
                bool hasMainFunction = false;

                // (AR) المرحلة الأولى: تسجيل جميع التصريحات والبحث عن main
                // (EN) Phase 1: Register all declarations and search for main
                for (const auto &stmt : program)
                {
                    // (AR) التحقق إذا كانت هذه دالة رئيسية
                    // (EN) Check if this is a main function
                    if (auto *funcDecl = dynamic_cast<AST::FunctionDecl *>(stmt.get()))
                    {
                        if (funcDecl->isMainFunction)
                        {
                            hasMainFunction = true;
                            mainFunction = funcDecl;

                            if (options_.enableDebugMode)
                            {
                                std::cout << "(AR) تم العثور على الدالة الرئيسية!"
                                          << " / (EN) Found main function!" << std::endl;
                            }
                        }
                    }
                }

                // (AR) التحقق من صحة البرنامج عند وجود دالة رئيسية
                // (EN) Validate program structure when main function exists
                if (hasMainFunction)
                {
                    for (const auto &stmt : program)
                    {
                        // (AR) التحقق من أن الجمل خارج الدوال هي تصريحات فقط وليست جمل تنفيذية
                        // (EN) Check that top-level statements are declarations only, not executable statements
                        bool isDeclaration =
                            dynamic_cast<AST::FunctionDecl *>(stmt.get()) != nullptr ||
                            dynamic_cast<AST::ClassDecl *>(stmt.get()) != nullptr ||
                            dynamic_cast<AST::EnumDecl *>(stmt.get()) != nullptr ||
                            dynamic_cast<AST::TemplateFunctionDecl *>(stmt.get()) != nullptr ||
                            dynamic_cast<AST::TemplateClassDecl *>(stmt.get()) != nullptr ||
                            dynamic_cast<AST::NamespaceDecl *>(stmt.get()) != nullptr;

                        // (AR) السماح بالمتغيرات العامة والاستيراد والتصدير
                        // (EN) Allow global variables, import, and export statements
                        bool isGlobalVar = dynamic_cast<AST::VarDeclStmt *>(stmt.get()) != nullptr;

                        // (AR) السماح بجمل الاستيراد والتصدير على المستوى الأعلى
                        // (EN) Allow import/export statements at top level
                        bool isModuleStmt =
                            dynamic_cast<AST::ImportStmt *>(stmt.get()) != nullptr ||
                            dynamic_cast<AST::FromImportStmt *>(stmt.get()) != nullptr ||
                            dynamic_cast<AST::ExportDecl *>(stmt.get()) != nullptr ||
                            dynamic_cast<AST::ExportStmt *>(stmt.get()) != nullptr;

                        if (!isDeclaration && !isGlobalVar && !isModuleStmt)
                        {
                            // (AR) جملة تنفيذية خارج الدوال - غير مسموح عند وجود main
                            // (EN) Executable statement outside functions - not allowed when main exists

                            // (AR) إنشاء موقع من الجملة (افتراضي إذا لم يكن متاحاً)
                            // (EN) Create location from statement (default if not available)
                            Sad::Errors::SourceLocation location("<input>", 1, 1);

                            Sad::Errors::ErrorManager::getInstance().reportError(
                                Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH, // (AR) استخدام خطأ دلالي عام / (EN) Use general semantic error
                                location,
                                "(AR) خطأ: عند وجود الدالة الرئيسية 'رئيسية'، يجب أن تكون جميع الجمل التنفيذية داخل دوال.\n"
                                "       لا يُسمح بكتابة كود تنفيذي خارج الدوال عندما يحتوي البرنامج على دالة رئيسية.\n"
                                "       الحل: ضع الكود داخل الدالة الرئيسية أو دالة أخرى.",
                                "(EN) Error: When main function 'رئيسية' exists, all executable statements must be inside functions.\n"
                                "       Writing executable code outside functions is not allowed when the program has a main function.\n"
                                "       Solution: Place the code inside the main function or another function.");

                            return ExecutionResult(false, Data::Value(),
                                                   "(AR) خطأ: كود تنفيذي خارج الدوال عند وجود main / "
                                                   "(EN) Error: Executable code outside functions when main exists");
                        }
                    }
                }

                // (AR) تسجيل التصريحات (الدوال، الأصناف، المتغيرات العامة)
                // (EN) Register declarations (functions, classes, global variables)

                // ================================================================
                // (AR) فحص الأنواع المتقدم - قبل التنفيذ
                // (EN) Advanced type check - before execution
                // ================================================================
#if !defined(__EMSCRIPTEN__) && !defined(SAD_PLATFORM_ANDROID)
                if (options_.enableTypeCheck)
                {
                    if (options_.enableDebugMode)
                    {
                        std::cout << "(AR) فحص الأنواع... / (EN) Type checking..." << std::endl;
                    }

                    Sad::Semantic::TypeChecker typeChecker;
                    typeChecker.setArabicMessages(true);
                    typeChecker.setDebugMode(options_.typeCheckDebugMode);
                    typeChecker.setStrictMode(options_.typeCheckStrictMode);

                    // (AR) زيارة كل جملة مباشرة / (EN) Visit each statement directly
                    for (const auto &stmt : program)
                    {
                        if (stmt)
                        {
                            stmt->accept(typeChecker);
                        }
                    }

                    // (AR) الحصول على النتيجة وعرض الأخطاء / (EN) Get result and display errors
                    const auto &result = typeChecker.getResult();

                    if (options_.typeCheckDebugMode || !result.errors.empty())
                    {
                        typeChecker.printSummary();
                    }

                    if (!result.errors.empty())
                    {
                        std::cerr << "\n❌ فحص الأنواع فشل مع " << result.errors.size() << " أخطاء\n";
                        std::cerr << "❌ Type check failed with " << result.errors.size() << " error(s)\n";
                        return ExecutionResult(false, Data::Value(), "Type check failed");
                    }

                    if (options_.enableDebugMode)
                    {
                        std::cout << "(AR) ✓ فحص الأنواع تم / (EN) ✓ Type check completed" << std::endl;
                    }
                }
#endif // !__EMSCRIPTEN__ && !SAD_PLATFORM_ANDROID

                // (AR) مسح أي أخطاء سابقة من مراحل التحليل - نبدأ تنفيذاً نظيفاً
                // (EN) Clear any previous errors from parsing phases - start with clean execution
                Sad::Errors::ErrorManager::getInstance().clear();

                for (const auto &stmt : program)
                {
                    // (AR) تنفيذ التصريحات فقط (تسجيل الدوال، الأصناف، المتغيرات العامة)
                    // (EN) Execute declarations only (register functions, classes, global variables)
                    auto result = executeStatement(*stmt);

                    if (!result.success)
                    {
                        return result; // (AR) فشل التنفيذ / (EN) Execution failed
                    }

                    // (AR) التحقق من وجود أخطاء بعد تنفيذ كل جملة / (EN) Check for errors after each statement
                    if (Sad::Errors::ErrorManager::getInstance().hasErrors())
                    {
                        return ExecutionResult(false, Data::Value(), "Runtime error occurred");
                    }
                }

                // (AR) المرحلة الثانية: تنفيذ الدالة الرئيسية إذا وُجدت
                // (EN) Phase 2: Execute main function if found
                if (hasMainFunction && mainFunction)
                {
                    if (options_.enableDebugMode)
                    {
                        std::cout << "(AR) تنفيذ الدالة الرئيسية..."
                                  << " / (EN) Executing main function..." << std::endl;
                    }

                    // (AR) تنفيذ جسم الدالة الرئيسية باستخدام executeFunctionBodyWithReturnType
                    // (EN) Execute main function body using executeFunctionBodyWithReturnType
                    try
                    {
                        // (AR) إنشاء نطاق جديد للدالة الرئيسية
                        // (EN) Create new scope for main function
                        variableManager_->enterScope(Data::ScopeType::FUNCTION, "main");

                        // (AR) تنفيذ جسم الدالة مع نوع الإرجاع
                        // (EN) Execute function body with return type
                        Data::Value returnValue;
                        if (mainFunction->body)
                        {
                            // (AR) استخدام executeFunctionBodyWithReturnType لتعيين سياق الدالة بشكل صحيح
                            // (EN) Use executeFunctionBodyWithReturnType to properly set function context
                            returnValue = statementExecutor_->executeFunctionBodyWithReturnType(
                                *mainFunction->body,
                                mainFunction->returnType,
                                "رئيسية" // (AR) اسم الدالة بالعربية / (EN) Function name in Arabic
                            );

                            // (AR) إعادة تعيين التحكم في التدفق
                            // (EN) Reset flow control
                            statementExecutor_->resetFlowControl();
                        }
                        else
                        {
                            // (AR) إذا لم يكن هناك جسم، إرجاع 0 كقيمة افتراضية
                            // (EN) If no body, return 0 as default
                            returnValue = Data::Value(0);
                        }

                        // (AR) إزالة النطاق
                        // (EN) Pop scope
                        variableManager_->exitScope();

                        if (options_.enableDebugMode)
                        {
                            std::cout << "(AR) اكتملت الدالة الرئيسية بقيمة إرجاع: "
                                      << returnValue.toString()
                                      << " / (EN) Main function completed with return value: "
                                      << returnValue.toString() << std::endl;
                        }

                        // (AR) طباعة إحصائيات التحسين العربي
                        // (EN) Print Arabic optimization statistics
                        if (options_.enableDebugMode)
                        {
                            // (AR) استخدام ExpressionEvaluator من StatementExecutor (الذي يُنفَّذ فعلاً)
                            // (EN) Use ExpressionEvaluator from StatementExecutor (the one actually executing)
                            if (statementExecutor_->getExpressionEvaluator())
                            {
                                statementExecutor_->getExpressionEvaluator()->printArabicOptStats();
                            }
                        }

                        return ExecutionResult(true, returnValue);
                    }
                    catch (const std::exception &e)
                    {
                        // (AR) التأكد من إزالة النطاق حتى في حالة الخطأ
                        // (EN) Ensure scope is popped even on error
                        try
                        {
                            variableManager_->exitScope();
                        }
                        catch (...)
                        {
                            // (AR) تجاهل أخطاء popScope في حالة الاستثناء
                            // (EN) Ignore popScope errors during exception
                        }

                        return ExecutionResult(false, Data::Value(),
                                               std::string("(AR) خطأ في تنفيذ الدالة الرئيسية: ") + e.what() +
                                                   " / (EN) Error executing main function: " + e.what());
                    }
                }

                // (AR) إذا لم توجد دالة رئيسية، التنفيذ العادي (الوضع القديم)
                // (EN) If no main function, normal execution (legacy mode)
                if (options_.enableDebugMode)
                {
                    std::cout << "(AR) لا توجد دالة رئيسية - الوضع القديم"
                              << " / (EN) No main function - legacy mode" << std::endl;
                }

                // (AR) طباعة إحصائيات التحسين العربي في الوضع القديم أيضاً
                // (EN) Print Arabic optimization statistics in legacy mode too
                if (options_.enableDebugMode)
                {
                    if (statementExecutor_->getExpressionEvaluator())
                    {
                        statementExecutor_->getExpressionEvaluator()->printArabicOptStats();
                    }
                }

                return ExecutionResult(true, Data::Value());
            }
            catch (const ExecutionError &e)
            {
                std::string errorMsg = "(AR) خطأ في التنفيذ: " + std::string(e.what()) +
                                       " / (EN) Execution error: " + std::string(e.what());

                if (options_.enableDebugMode)
                {
                    std::cerr << errorMsg << std::endl;
                }

                return ExecutionResult(false, Data::Value(), errorMsg);
            }
            catch (const RuntimeError &e)
            {
                std::string errorMsg = "(AR) خطأ في وقت التشغيل: " + std::string(e.what()) +
                                       " / (EN) Runtime error: " + std::string(e.what());

                if (options_.enableDebugMode)
                {
                    std::cerr << errorMsg << std::endl;
                }

                return ExecutionResult(false, Data::Value(), errorMsg);
            }
            catch (const std::exception &e)
            {
                std::string errorMsg = "(AR) خطأ غير متوقع: " + std::string(e.what()) +
                                       " / (EN) Unexpected error: " + std::string(e.what());

                if (options_.enableDebugMode)
                {
                    std::cerr << errorMsg << std::endl;
                }

                return ExecutionResult(false, Data::Value(), errorMsg);
            }
        }

        ExecutionResult Interpreter::executeStatement(AST::Statement &statement)
        {
            try
            {
                // (AR) تنفيذ الجملة / (EN) Execute statement
                statement.accept(*statementExecutor_);

                // (AR) التحقق من وجود return / (EN) Check for return
                if (statementExecutor_->getFlowControl() == FlowControl::RETURN)
                {
                    Data::Value returnValue = statementExecutor_->getReturnValue();
                    statementExecutor_->resetFlowControl();
                    return ExecutionResult(true, returnValue);
                }

                // (AR) نجح التنفيذ بدون إرجاع / (EN) Execution succeeded without return
                return ExecutionResult(true);
            }
            catch (const ExecutionError &e)
            {
                return ExecutionResult(false, Data::Value(), std::string(e.what()));
            }
            catch (const RuntimeError &e)
            {
                return ExecutionResult(false, Data::Value(), std::string(e.what()));
            }
            catch (const std::exception &e)
            {
                return ExecutionResult(false, Data::Value(), std::string(e.what()));
            }
        }

        Data::Value Interpreter::evaluateExpression(AST::Expression &expression)
        {
            expression.accept(*expressionEvaluator_);
            return expressionEvaluator_->getResult();
        }

        // =========================================================================
        // (AR) إدارة الحالة / (EN) State Management
        // =========================================================================

        void Interpreter::reset()
        {
            if (options_.enableDebugMode)
            {
                std::cout << "(AR) إعادة تعيين المفسر / (EN) Resetting interpreter" << std::endl;
            }

            // (AR) إعادة إنشاء جميع المكونات / (EN) Recreate all components
            initializeComponents();
        }

        // =========================================================================
        // (AR) استدعاء دالة مستخدم من C++ — الإطار التفاعلي
        // (EN) Call user function from C++ — Reactive Framework
        // =========================================================================

        Data::Value Interpreter::callUserFunction(const std::string &funcName,
                                                  const std::vector<Data::Value> &args)
        {
            // ─── (AR) البحث عن الدالة بالاسم وعدد المعاملات ───
            // ─── (EN) Find function by name and argument count ───
            auto func = functionManager_->getFunction(funcName, args.size());

            if (!func)
            {
                // (AR) بحث مرن — جرّب مع القيم الافتراضية
                // (EN) Flexible search — try with default parameter support
                auto allOverloads = functionManager_->getFunctionOverloads(funcName);
                for (const auto &candidate : allOverloads)
                {
                    if (candidate->hasNativeImplementation())
                    {
                        func = candidate;
                        break;
                    }
                    if (candidate->acceptsArgumentCount(args.size()))
                    {
                        func = candidate;
                        break;
                    }
                }
            }

            if (!func)
            {
                throw std::runtime_error(
                    "(AR) الدالة '" + funcName + "' غير موجودة / "
                                                 "(EN) Function '" +
                    funcName + "' not found");
            }

            // ─── (AR) إذا كانت دالة مضمنة — استدعاء مباشر ───
            // ─── (EN) If built-in — call directly ───
            if (func->hasNativeImplementation())
            {
                std::vector<std::shared_ptr<Data::Value>> valuePtrs;
                for (const auto &arg : args)
                {
                    valuePtrs.push_back(std::make_shared<Data::Value>(arg));
                }
                auto result = func->callNative(valuePtrs);
                return result ? *result : Data::Value();
            }

            // ─── (AR) إذا كانت دالة خارجية (FFI) — نُرجع قيمة فارغة ───
            // ─── (EN) If extern function (FFI) — return empty value ───
            // (AR) الدوال الخارجية مُعدّة للمترجم وليس المفسر
            // (EN) Extern functions are meant for compiler, not interpreter
            if (func->isExtern())
            {
                // (AR) نُرجع 0 كقيمة افتراضية للدوال الخارجية في المفسر
                // (EN) Return 0 as default value for extern functions in interpreter
                return Data::Value(static_cast<int64_t>(0));
            }

            // ─── (AR) التحقق من وجود جسم للدالة ───
            // ─── (EN) Check function has body ───
            if (!func->hasBody())
            {
                throw std::runtime_error(
                    "(AR) الدالة '" + funcName + "' ليس لها جسم / "
                                                 "(EN) Function '" +
                    funcName + "' has no body");
            }

            // ─── (AR) إنشاء نطاق جديد للدالة ───
            // ─── (EN) Create new scope for function ───
            variableManager_->enterScope(Data::ScopeType::FUNCTION, funcName);

            // ─── (AR) تعريف المعاملات كمتغيرات محلية ───
            // ─── (EN) Define parameters as local variables ───
            const auto &params = func->getParameters();
            for (size_t i = 0; i < params.size() && i < args.size(); ++i)
            {
                variableManager_->define(params[i].name, args[i]);
            }

            // ─── (AR) معالجة المعاملات الافتراضية للناقصة ───
            // ─── (EN) Handle default values for missing parameters ───
            if (args.size() < params.size())
            {
                auto funcDeclNode = func->getFunctionDecl();
                AST::FunctionDecl *astFuncDecl = nullptr;
                if (funcDeclNode)
                {
                    astFuncDecl = dynamic_cast<AST::FunctionDecl *>(funcDeclNode.get());
                }

                for (size_t i = args.size(); i < params.size(); ++i)
                {
                    const auto &param = params[i];
                    Data::Value defaultVal;

                    if (astFuncDecl && i < astFuncDecl->parameters.size() &&
                        astFuncDecl->parameters[i].defaultValue)
                    {
                        astFuncDecl->parameters[i].defaultValue->accept(*expressionEvaluator_);
                        defaultVal = expressionEvaluator_->getResult();
                    }
                    else if (param.hasDefaultValue)
                    {
                        const std::string &ds = param.defaultValue;
                        if (ds == "true" || ds == "صحيح")
                            defaultVal = Data::Value(true);
                        else if (ds == "false" || ds == "خطأ")
                            defaultVal = Data::Value(false);
                        else
                        {
                            try
                            {
                                defaultVal = Data::Value(std::stod(ds));
                            }
                            catch (...)
                            {
                                defaultVal = Data::Value(ds);
                            }
                        }
                    }

                    variableManager_->define(param.name, defaultVal);
                }
            }

            // ─── (AR) تنفيذ جسم الدالة ───
            // ─── (EN) Execute function body ───
            Data::Value result;
            try
            {
                auto bodyNode = func->getBody();
                auto bodyStmt = dynamic_cast<AST::Statement *>(bodyNode.get());

                if (bodyStmt)
                {
                    result = statementExecutor_->executeFunctionBody(*bodyStmt);
                }
                else
                {
                    // (AR) جسم لامدا — تعبير
                    // (EN) Lambda body — expression
                    auto bodyExpr = dynamic_cast<AST::Expression *>(bodyNode.get());
                    if (bodyExpr)
                    {
                        bodyExpr->accept(*expressionEvaluator_);
                        result = expressionEvaluator_->getResult();
                    }
                }
            }
            catch (...)
            {
                // (AR) التأكد من إزالة النطاق حتى عند الخطأ
                // (EN) Ensure scope is popped even on error
                variableManager_->exitScope();
                throw;
            }

            // ─── (AR) إزالة نطاق الدالة ───
            // ─── (EN) Pop function scope ───
            variableManager_->exitScope();

            return result;
        }

        // =========================================================================
        // (AR) استدعاء طريقة على كائن — لنظام حالة_عنصر
        // (EN) Call a method on object — for StatefulWidget system
        // =========================================================================

        Data::Value Interpreter::callMethodOnObject(
            const std::shared_ptr<Data::ObjectInstance> &obj,
            const std::string &methodName,
            const std::vector<Data::Value> &args)
        {
            if (!obj)
            {
                throw std::runtime_error(
                    "(AR) لا يمكن استدعاء طريقة على كائن فارغ / "
                    "(EN) Cannot call method on null object");
            }

            // ─── (AR) البحث عن الطريقة في صنف الكائن ───
            // ─── (EN) Find the method in the object's class ───
            auto *method = obj->getMethod(methodName);
            if (!method)
            {
                throw std::runtime_error(
                    "(AR) الطريقة '" + methodName + "' غير موجودة في الصنف '" +
                    obj->getClassName() + "' / "
                                          "(EN) Method '" +
                    methodName + "' not found in class '" +
                    obj->getClassName() + "'");
            }

            if (!method->getBody())
            {
                throw std::runtime_error(
                    "(AR) الطريقة '" + methodName + "' ليس لها جسم / "
                                                    "(EN) Method '" +
                    methodName + "' has no body");
            }

            // ─── (AR) إنشاء نطاق جديد للطريقة ───
            // ─── (EN) Create new scope for method ───
            variableManager_->enterScope(Data::ScopeType::FUNCTION,
                                         obj->getClassName() + "." + methodName);

            // ─── (AR) تعريف هذا (this) في النطاق ───
            // ─── (EN) Define this in scope ───
            Data::Value thisValue(obj);
            variableManager_->define("\xd9\x87\xd8\xb0\xd8\xa7", thisValue); // هذا
            variableManager_->define("this", thisValue);

            // ─── (AR) تعريف حقول الكائن كمتغيرات محلية ───
            // ─── (EN) Define object fields as local variables ───
            auto fieldNames = obj->getFieldNames();
            for (const auto &fieldName : fieldNames)
            {
                auto *fieldVal = obj->getField(fieldName);
                if (fieldVal)
                {
                    variableManager_->define(fieldName, *fieldVal);
                }
            }

            // ─── (AR) تعريف المعاملات ───
            // ─── (EN) Define parameters ───
            const auto &params = method->parameters;
            for (size_t i = 0; i < params.size() && i < args.size(); ++i)
            {
                variableManager_->define(params[i].name, args[i]);
            }

            // ─── (AR) تنفيذ جسم الطريقة ───
            // ─── (EN) Execute method body ───
            Data::Value result;
            try
            {
                auto *bodyNode = method->getBody(); // (AR) يرجع AST::BlockStmt* مباشرة

                if (bodyNode)
                {
                    result = statementExecutor_->executeFunctionBody(*bodyNode);
                }
            }
            catch (...)
            {
                variableManager_->exitScope();
                throw;
            }

            // ─── (AR) نسخ التعديلات على الحقول إلى الكائن ───
            // ─── (EN) Copy field modifications back to the object ───
            for (const auto &fieldName : fieldNames)
            {
                try
                {
                    auto updatedVal = variableManager_->get(fieldName);
                    obj->setField(fieldName, updatedVal);
                }
                catch (...)
                {
                    // (AR) الحقل لم يُعدّل — متوقع
                }
            }

            variableManager_->exitScope();

            return result;
        }

        // =========================================================================
        // (AR) تنفيذ دالة مسجلة في سياق كائن — لمعالجات أحداث الواجهة
        // (EN) Execute registered function in object context — for UI event handlers
        // =========================================================================

        Data::Value Interpreter::callFunctionInObjectContext(
            const std::string &funcName,
            const std::shared_ptr<Data::ObjectInstance> &obj,
            const std::vector<Data::Value> &args)
        {
            if (!obj)
            {
                // (AR) لا يوجد كائن — تنفيذ عادي
                // (EN) No object — regular execution
                return callUserFunction(funcName, args);
            }

            // ─── (AR) البحث عن الدالة بالاسم ───
            // ─── (EN) Find function by name ───
            auto func = functionManager_->getFunction(funcName, args.size());
            if (!func)
            {
                auto allOverloads = functionManager_->getFunctionOverloads(funcName);
                for (const auto &candidate : allOverloads)
                {
                    if (candidate->acceptsArgumentCount(args.size()))
                    {
                        func = candidate;
                        break;
                    }
                }
            }

            if (!func)
            {
                throw std::runtime_error(
                    "(AR) الدالة '" + funcName + "' غير موجودة / "
                                                 "(EN) Function '" +
                    funcName + "' not found");
            }

            if (!func->hasBody())
            {
                throw std::runtime_error(
                    "(AR) الدالة '" + funcName + "' ليس لها جسم / "
                                                 "(EN) Function '" +
                    funcName + "' has no body");
            }

            // ─── (AR) إنشاء نطاق جديد مع سياق الكائن ───
            // ─── (EN) Create new scope with object context ───
            variableManager_->enterScope(Data::ScopeType::FUNCTION, funcName);

            // ─── (AR) تعريف هذا (this) في النطاق ───
            // ─── (EN) Define this in scope ───
            Data::Value thisValue(obj);
            variableManager_->define("\xd9\x87\xd8\xb0\xd8\xa7", thisValue); // هذا
            variableManager_->define("this", thisValue);

            // ─── (AR) تعريف حقول الكائن كمتغيرات محلية ───
            // ─── (EN) Define object fields as local variables ───
            auto fieldNames = obj->getFieldNames();
            for (const auto &fieldName : fieldNames)
            {
                auto *fieldVal = obj->getField(fieldName);
                if (fieldVal)
                {
                    variableManager_->define(fieldName, *fieldVal);
                }
            }

            // ─── (AR) تعريف المعاملات ───
            // ─── (EN) Define parameters ───
            const auto &params = func->getParameters();
            for (size_t i = 0; i < params.size() && i < args.size(); ++i)
            {
                variableManager_->define(params[i].name, args[i]);
            }

            // ─── (AR) تنفيذ جسم الدالة ───
            // ─── (EN) Execute function body ───
            Data::Value result;
            try
            {
                auto bodyNode = func->getBody();
                auto bodyStmt = dynamic_cast<AST::Statement *>(bodyNode.get());

                if (bodyStmt)
                {
                    result = statementExecutor_->executeFunctionBody(*bodyStmt);
                }
                else
                {
                    // (AR) جسم تعبيري (لامدا/سهم)
                    // (EN) Expression body (lambda/arrow)
                    auto bodyExpr = dynamic_cast<AST::Expression *>(bodyNode.get());
                    if (bodyExpr)
                    {
                        bodyExpr->accept(*expressionEvaluator_);
                        result = expressionEvaluator_->getResult();
                    }
                }
            }
            catch (...)
            {
                variableManager_->exitScope();
                throw;
            }

            // ─── (AR) نسخ التعديلات على الحقول إلى الكائن ───
            // ─── (EN) Copy field modifications back to the object ───
            for (const auto &fieldName : fieldNames)
            {
                try
                {
                    auto updatedVal = variableManager_->get(fieldName);
                    obj->setField(fieldName, updatedVal);
                }
                catch (...)
                {
                    // (AR) الحقل لم يُعدّل — متوقع
                }
            }

            variableManager_->exitScope();

            return result;
        }

    } // namespace Interpreter
} // namespace Sad
