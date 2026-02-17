/**
 * @file statement_executor.h
 * @brief (AR) منفّذ الجُمل - تنفيذ جميع أنواع الجُمل في لغة ص
 * @brief (EN) Statement Executor - executes all statement types in S Language
 * 
 * (AR) هذا الملف يحتوي على تعريف فئة StatementExecutor التي تستخدم نمط الزائر
 *      لتنفيذ جميع أنواع الجُمل في لغة ص. يتكامل مع ExpressionEvaluator لتقييم
 *      التعابير ضمن الجُمل.
 * 
 * (EN) This file contains the StatementExecutor class definition that uses the
 *      Visitor pattern to execute all statement types in S Language. It integrates
 *      with ExpressionEvaluator to evaluate expressions within statements.
 * 
 * Statement Executor is responsible for:
 * - Executing variable declarations (VarDeclStmt)
 * - Executing expression statements (ExprStmt)
 * - Managing code blocks and scopes (BlockStmt)
 * - Executing control flow (IfStmt, WhileStmt, ForStmt)
 * - Handling function declarations and calls (FuncDeclStmt, CallExpr)
 * - Managing return, break, continue statements
 * - Exception handling (TryStmt, RaiseStmt)
 * 
 * Features:
 * - Inherits from BaseASTVisitor to implement Visitor pattern
 * - Integrates with ExpressionEvaluator for expression evaluation
 * - Manages variable scopes using ScopeManager
 * - Handles control flow with special flow control states
 * - Full support for all statement types
 * - Clear bilingual error messages
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#pragma once

#include "ast_visitor.h"
#include "statements.h"
#include "expressions.h"
#include "value.h"
#include "variable_manager.h"
#include "function_manager.h"
#include "scope_manager.h"
#include "ownership_manager.h"
#include "module_resolver.h"
#include "expression_evaluator.h"
#include <stdexcept>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace Sad {
namespace Interpreter {

/**
 * @brief (AR) حالات التحكم بالتدفق
 * @brief (EN) Flow control states
 */
enum class FlowControl {
    NONE,       ///< (AR) تدفق عادي / (EN) Normal flow
    RETURN,     ///< (AR) إرجاع من دالة / (EN) Return from function
    BREAK,      ///< (AR) كسر حلقة / (EN) Break from loop
    CONTINUE,   ///< (AR) متابعة للتكرار التالي / (EN) Continue to next iteration
    YIELD       ///< (AR) إعطاء قيمة من مولّد / (EN) Yield value from generator
};

/**
 * @brief (AR) استثناء وقت التشغيل
 * @brief (EN) Runtime exception
 */
class ExecutionError : public std::runtime_error {
public:
    ExecutionError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief (AR) منفّذ الجُمل - ينفذ نمط الزائر لتنفيذ AST
 * @brief (EN) Statement Executor - implements Visitor pattern to execute AST
 * 
 * هذا الصنف يجتاز شجرة AST وينفذ كل جملة، مع إدارة النطاقات والتدفق
 * This class traverses the AST and executes each statement, managing scopes and flow
 * 
 * @example مثال الاستخدام / Usage Example:
 * @code{.cpp}
 * Data::VariableManager varMgr;
 * Data::FunctionManager funcMgr;
 * Data::ScopeManager scopeMgr;
 * StatementExecutor executor(varMgr, funcMgr, scopeMgr);
 * 
 * // تنفيذ كتلة كود
 * auto block = std::make_unique<BlockStmt>(...);
 * block->accept(executor);
 * @endcode
 */
class StatementExecutor : public AST::BaseASTVisitor {
public:
    /**
     * @brief (AR) البناء
     * @brief (EN) Constructor
     * 
     * @param varMgr (AR) مرجع لمدير المتغيرات / (EN) Reference to variable manager
     * @param funcMgr (AR) مرجع لمدير الدوال / (EN) Reference to function manager
     * @param scopeMgr (AR) مرجع لمدير النطاقات / (EN) Reference to scope manager
     */
    StatementExecutor(Data::VariableManager& varMgr, 
                     Data::FunctionManager& funcMgr,
                     Data::ScopeManager& scopeMgr,
                     Data::OwnershipManager& ownershipMgr);
    
    /**
     * @brief (AR) الحصول على حالة التحكم بالتدفق الحالية
     * @brief (EN) Get current flow control state
     */
    FlowControl getFlowControl() const { return flowControl_; }
    
    /**
     * @brief (AR) الحصول على مُقيِّم التعابير (للإحصائيات)
     * @brief (EN) Get expression evaluator (for statistics)
     */
    ExpressionEvaluator* getExpressionEvaluator() const { return expressionEvaluator_.get(); }
    
    /**
     * @brief (AR) إعادة تعيين حالة التحكم بالتدفق
     * @brief (EN) Reset flow control state
     */
    void resetFlowControl() { flowControl_ = FlowControl::NONE; }
    
    /**
     * @brief (AR) الحصول على قيمة الإرجاع (للدوال)
     * @brief (EN) Get return value (for functions)
     */
    Data::Value getReturnValue() const { return returnValue_; }
    
    /**
     * @brief (AR) الحصول على قيمة yield (للمولّدات)
     * @brief (EN) Get yield value (for generators)
     */
    Data::Value getYieldValue() const { return yieldValue_; }
    
    /**
     * @brief (AR) تعيين وضع المولّد
     * @brief (EN) Set generator mode
     */
    void setGeneratorMode(bool isGenerator) { inGenerator_ = isGenerator; }
    
    /**
     * @brief (AR) هل نحن داخل مولّد؟
     * @brief (EN) Are we inside a generator?
     */
    bool isInGenerator() const { return inGenerator_; }
    
    // =========================================================================
    // (AR) زيارة الجُمل / (EN) Statement Visitors
    // =========================================================================
    
    /**
     * @brief (AR) زيارة جملة تعبير / (EN) Visit expression statement
     * @details تنفذ تعبير مستقل كجملة / Executes standalone expression as statement
     */
    void visitExprStmt(AST::ExprStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة تعريف متغير / (EN) Visit variable declaration statement
     * @details تعرّف متغير جديد مع قيمة اختيارية / Declares new variable with optional value
     */
    void visitVarDeclStmt(AST::VarDeclStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة إذا / (EN) Visit if statement
     * @details تنفذ فرع الشرط حسب التقييم / Executes conditional branch based on evaluation
     */
    void visitIfStmt(AST::IfStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة بينما / (EN) Visit while statement
     * @details تنفذ حلقة while مع دعم break/continue / Executes while loop with break/continue support
     */
    void visitWhileStmt(AST::WhileStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة لكل / (EN) Visit for statement
     * @details تنفذ حلقة for بأسلوب C / Executes C-style for loop
     */
    void visitForStmt(AST::ForStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة لكل في / (EN) Visit for-range statement
     * @details تنفذ حلقة for-each على مجموعة / Executes for-each loop over collection
     */
    void visitForRangeStmt(AST::ForRangeStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة switch-case / (EN) Visit switch-case statement
     * @details تُقيّم تعبيراً وتنفذ الحالة المطابقة / Evaluates expression and executes matching case
     */
    void visitSwitchStmt(AST::SwitchStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة إرجاع / (EN) Visit return statement
     * @details تُرجع من دالة مع قيمة اختيارية / Returns from function with optional value
     */
    void visitReturnStmt(AST::ReturnStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة كسر / (EN) Visit break statement
     * @details تكسر من حلقة / Breaks from loop
     */
    void visitBreakStmt(AST::BreakStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة متابعة / (EN) Visit continue statement
     * @details تنتقل للتكرار التالي / Continues to next iteration
     */
    void visitContinueStmt(AST::ContinueStmt& node) override;
    
    /**
     * @brief (AR) زيارة كتلة كود / (EN) Visit block statement
     * @details تنفذ كتلة جمل مع نطاق جديد / Executes block of statements with new scope
     */
    void visitBlockStmt(AST::BlockStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة محاولة-التقاط / (EN) Visit try statement
     * @details تعالج الاستثناءات / Handles exceptions
     */
    void visitTryStmt(AST::TryStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة رفع استثناء / (EN) Visit raise statement
     * @details ترفع استثناء / Raises exception
     */
    void visitRaiseStmt(AST::RaiseStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة yield (للمولّدات) / (EN) Visit yield statement (for generators)
     * @details تُنتج قيمة من دالة مولّدة / Yields value from generator function
     */
    void visitYieldStmt(AST::YieldStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة باستخدام (مدير السياق) / (EN) Visit with statement (context manager)
     * @details تستدعي __دخول__() عند الدخول و__خروج__() عند الخروج / Calls __enter__() on entry and __exit__() on exit
     */
    void visitWithStmt(AST::WithStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة مطابقة أنماط / (EN) Visit pattern matching statement
     * @details تطابق قيمة مع أنماط وتنفذ الفرع المناسب / Matches value against patterns and executes matching branch
     */
    void visitMatchStmt(AST::MatchStmt& node) override;
    
    /**
     * @brief (AR) زيارة تصريح دالة / (EN) Visit function declaration
     * @details تسجل الدالة في FunctionManager / Registers function in FunctionManager
     */
    void visitFunctionDecl(AST::FunctionDecl& node) override;
    
    // =========================================================================
    // (AR) زيارة تصريحات OOP / (EN) OOP Declaration Visitors
    // =========================================================================
    
    /**
     * @brief (AR) زيارة تصريح صنف / (EN) Visit class declaration
     * @details تسجل الصنف في ClassManager / Registers class in ClassManager
     */
    void visitClassDecl(AST::ClassDecl& node) override;
    
    /**
     * @brief (AR) زيارة تصريح حقل / (EN) Visit field declaration
     * @details يعالج حقل في صنف / Processes field in class
     */
    void visitFieldDecl(AST::FieldDecl& node) override;
    
    /**
     * @brief (AR) زيارة تصريح طريقة / (EN) Visit method declaration
     * @details يسجل طريقة في صنف / Registers method in class
     */
    void visitMethodDecl(AST::MethodDecl& node) override;
    
    /**
     * @brief (AR) زيارة تصريح خاصية / (EN) Visit property declaration
     * @details يسجل خاصية في صنف / Registers property in class
     */
    void visitPropertyDecl(AST::PropertyDecl& node) override;
    
    /**
     * @brief (AR) زيارة تصريح باني / (EN) Visit constructor declaration
     * @details يسجل باني في صنف / Registers constructor in class
     */
    void visitConstructorDecl(AST::ConstructorDecl& node) override;
    
    /**
     * @brief (AR) زيارة تصريح هادم / (EN) Visit destructor declaration
     * @details يسجل هادم في صنف / Registers destructor in class
     */
    void visitDestructorDecl(AST::DestructorDecl& node) override;
    
    // =========================================================================
    // (AR) زيارة تصريحات القوالب / (EN) Template Declaration Visitors
    // =========================================================================
    
    /**
     * @brief (AR) زيارة تصريح دالة قالب / (EN) Visit template function declaration
     * @details تسجل دالة القالب لإنشائها لاحقاً / Registers template function for later instantiation
     */
    void visitTemplateFunctionDecl(AST::TemplateFunctionDecl& node) override;
    
    /**
     * @brief (AR) زيارة تصريح صنف قالب / (EN) Visit template class declaration
     * @details تسجل صنف القالب لإنشائه لاحقاً / Registers template class for later instantiation
     */
    void visitTemplateClassDecl(AST::TemplateClassDecl& node) override;
    
    /**
     * @brief (AR) زيارة تصريح فضاء أسماء / (EN) Visit namespace declaration
     * @details يسجل فضاء الأسماء ويُنفّذ محتواه / Registers namespace and executes its contents
     */
    void visitNamespaceDecl(AST::NamespaceDecl& node) override;
    
    /**
     * @brief (AR) زيارة تصريح عامل محمّل زائداً / (EN) Visit operator overload declaration
     * @details يسجل تحميل العامل الزائد للصنف / Registers operator overload for class
     */
    void visitOperatorDecl(AST::OperatorDecl& node) override;
    
    // =========================================================================
    // (AR) زيارة جمل الاستيراد والتصدير / (EN) Import/Export Statement Visitors
    // =========================================================================
    
    /**
     * @brief (AR) زيارة جملة استيراد وحدة كاملة / (EN) Visit full module import statement
     * @details تحمّل وحدة كاملة وتسجل رموزها في النطاق الحالي / Loads full module and registers its symbols in current scope
     * @example استورد رياضيات → يحمّل ملف رياضيات.sad ويسجل كل صادراته
     * @example استورد رياضيات كـ ر → يحمّل الوحدة بالاسم المستعار "ر"
     */
    void visitImportStmt(AST::ImportStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة استيراد انتقائي / (EN) Visit selective import statement
     * @details تحمّل رموز محددة من وحدة / Loads specific symbols from a module
     * @example من رياضيات استورد جذر، قوة → يحمّل فقط الدوال جذر وقوة
     * @example من رياضيات استورد * → يحمّل كل الصادرات
     */
    void visitFromImportStmt(AST::FromImportStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة تصدير (الإصدار الجديد) / (EN) Visit export declaration (new version)
     * @details تنفذ التصريح الداخلي وتميّزه كمُصدَّر / Executes inner declaration and marks it as exported
     * @example صدّر دالة حساب() → تسجل الدالة وتميّزها كمُصدَّرة
     */
    void visitExportDecl(AST::ExportDecl& node) override;
    
    /**
     * @brief (AR) زيارة جملة تصدير (الإصدار القديم للتوافق) / (EN) Visit export statement (legacy version)
     * @details تنفذ التصريح الداخلي وتميّزه كمُصدَّر / Executes inner declaration and marks it as exported
     */
    void visitExportStmt(AST::ExportStmt& node) override;
    
    // =========================================================================
    // (AR) إعدادات نظام الوحدات / (EN) Module System Configuration
    // =========================================================================
    
    /**
     * @brief (AR) تعيين محلل الوحدات / (EN) Set module resolver
     * @param resolver (AR) مؤشر لمحلل الوحدات / (EN) Pointer to module resolver
     */
    void setModuleResolver(Modules::ModuleResolver* resolver) { moduleResolver_ = resolver; }
    
    /**
     * @brief (AR) تعيين مسار الملف الحالي / (EN) Set current file path
     * @param path (AR) مسار الملف / (EN) File path
     */
    void setCurrentFilePath(const std::string& path) { currentFilePath_ = path; }
    
private:
    // (AR) المراجع للمديرين / (EN) Manager references
    Data::VariableManager& variableManager_;
    Data::FunctionManager& functionManager_;
    Data::ScopeManager& scopeManager_;
    Data::OwnershipManager& ownershipManager_;
    
    // (AR) مُقيِّم التعابير / (EN) Expression evaluator
    std::unique_ptr<ExpressionEvaluator> expressionEvaluator_;
    
    // (AR) حالة التحكم بالتدفق / (EN) Flow control state
    FlowControl flowControl_;
    
    // (AR) قيمة الإرجاع / (EN) Return value
    Data::Value returnValue_;
    
    // (AR) قيمة yield (للمولّدات) / (EN) Yield value (for generators)
    Data::Value yieldValue_;
    
    // (AR) هل نحن داخل مولّد؟ / (EN) Are we inside a generator?
    bool inGenerator_;
    
    // (AR) عداد مستوى الحلقات (للتحقق من break/continue) / (EN) Loop depth counter
    int loopDepth_;
    
    // (AR) نوع إرجاع الدالة الحالية / (EN) Current function return type
    Data::DataType currentFunctionReturnType_;
    
    // (AR) اسم الدالة الحالية / (EN) Current function name
    std::string currentFunctionName_;
    
    // (AR) خريطة أنواع إرجاع الدوال / (EN) Map of function return types
    std::unordered_map<std::string, Data::DataType> functionReturnTypes_;
    
    // =========================================================================
    // (AR) تخزين القوالب / (EN) Template Storage
    // =========================================================================
    
    // (AR) خريطة دوال القوالب / (EN) Template functions map
    // Key: template function name, Value: pointer to AST node
    std::unordered_map<std::string, AST::TemplateFunctionDecl*> templateFunctions_;
    
    // (AR) خريطة أصناف القوالب / (EN) Template classes map
    // Key: template class name, Value: pointer to AST node
    std::unordered_map<std::string, AST::TemplateClassDecl*> templateClasses_;
    
    // (AR) خريطة مصادر نسخ القوالب - لربط الأصناف الملموسة بقوالبها الأصلية
    // (EN) Template instance sources map - links concrete class names to their template AST nodes
    // Used for runtime lookup of constructor/method bodies (avoids ownership issues with unique_ptr)
    std::unordered_map<std::string, AST::TemplateClassDecl*> templateInstanceSources_;
    
    // (AR) فضاء الأسماء الحالي / (EN) Current namespace
    std::string currentNamespace_;
    
    // =========================================================================
    // (AR) نظام الوحدات (الاستيراد والتصدير) / (EN) Module System (Import/Export)
    // =========================================================================
    
    // (AR) محلل الوحدات - يستخدم للبحث عن الوحدات وتحميلها
    // (EN) Module resolver - used for finding and loading modules
    Modules::ModuleResolver* moduleResolver_ = nullptr;
    
    // (AR) مسار الملف الحالي - يُستخدم لحل المسارات النسبية
    // (EN) Current file path - used for resolving relative paths
    std::string currentFilePath_;
    
    // (AR) الرموز المُصدَّرة من الملف الحالي - تُملأ عند مواجهة جمل صدّر
    // (EN) Exported symbols from current file - populated when export statements are encountered
    std::unordered_set<std::string> exportedSymbols_;
    
    // (AR) الوحدات المُحمَّلة كمتغيرات Map (لنمط: استورد وحدة كـ م → م.دالة)
    // (EN) Loaded modules as Map variables (for: import module as m → m.function)
    // key: اسم الوحدة (أو الاسم المستعار), value: Map من أسماء الرموز إلى قيمها
    std::unordered_map<std::string, Data::Value> loadedModuleNamespaces_;
    
    // (AR) ذاكرة مخبئية لصادرات الوحدات التي تم تنفيذها - لمنع إعادة التنفيذ
    // (EN) Cache for executed module exports - prevents re-execution
    // key: مسار الملف الكامل, value: Map الصادرات
    std::unordered_map<std::string, Data::Value> executedModuleExports_;
    
    /**
     * @brief (AR) تنفيذ AST وحدة مُحمَّلة واستخراج رموزها
     * @brief (EN) Execute a loaded module's AST and extract its symbols
     * 
     * @param module (AR) مؤشر للوحدة المُحمَّلة / (EN) Pointer to loaded module
     * @return (AR) خريطة بأسماء الرموز المُصدَّرة وقيمها / (EN) Map of exported symbol names to their values
     */
    Data::Value executeModuleAndExtractExports(Modules::Module* module);
    
    /**
     * @brief (AR) تقييم تعبير وإرجاع قيمته
     * @brief (EN) Evaluate expression and return its value
     */
    Data::Value evaluateExpression(AST::Expression& expr);
    
    /**
     * @brief (AR) التحقق إذا كنا داخل حلقة
     * @brief (EN) Check if we're inside a loop
     */
    bool isInLoop() const { return loopDepth_ > 0; }

public:
    /**
     * @brief (AR) تنفيذ جسم دالة
     * @brief (EN) Execute function body
     * @param body (AR) جسم الدالة / (EN) Function body
     * @return (AR) قيمة الإرجاع / (EN) Return value
     */
    Data::Value executeFunctionBody(AST::Statement& body);
    
    /**
     * @brief (AR) تنفيذ جسم دالة مع اسم الدالة (للتحقق من نوع الإرجاع)
     * @brief (EN) Execute function body with function name (for return type checking)
     * @param body (AR) جسم الدالة / (EN) Function body
     * @param functionName (AR) اسم الدالة / (EN) Function name
     * @return (AR) قيمة الإرجاع / (EN) Return value
     */
    Data::Value executeFunctionBodyWithFuncName(AST::Statement& body, const std::string& functionName);
    
    /**
     * @brief (AR) تنفيذ جسم دالة مع تتبع نوع الإرجاع
     * @brief (EN) Execute function body with return type tracking
     * @param body (AR) جسم الدالة / (EN) Function body
     * @param returnType (AR) نوع الإرجاع المتوقع / (EN) Expected return type
     * @param functionName (AR) اسم الدالة / (EN) Function name
     * @return (AR) قيمة الإرجاع / (EN) Return value
     */
    Data::Value executeFunctionBodyWithReturnType(
        AST::Statement& body, 
        Data::DataType returnType, 
        const std::string& functionName);
    
    /**
     * @brief (AR) التحقق إذا يجب التوقف عن التنفيذ
     * @brief (EN) Check if we should stop execution
     */
    bool shouldStopExecution() const {
        return flowControl_ != FlowControl::NONE;
    }
    
    /**
     * @brief (AR) الحصول على صنف قالب بالاسم
     * @brief (EN) Get a template class by name
     * @param name (AR) اسم صنف القالب / (EN) Template class name
     * @return (AR) مؤشر لعقدة صنف القالب أو nullptr / (EN) Pointer to template class node or nullptr
     */
    AST::TemplateClassDecl* getTemplateClass(const std::string& name) {
        auto it = templateClasses_.find(name);
        if (it != templateClasses_.end()) return it->second;
        return nullptr;
    }
    
    /**
     * @brief (AR) إنشاء نسخة ملموسة من صنف قالب وتسجيلها
     * @brief (EN) Instantiate a concrete version of a template class and register it
     * @param templateNode (AR) عقدة صنف القالب / (EN) Template class AST node
     * @param className (AR) اسم الصنف الملموس / (EN) Concrete class name
     */
    void instantiateTemplateClass(AST::TemplateClassDecl& templateNode, const std::string& className);
    
    /**
     * @brief (AR) الحصول على مصدر القالب لنسخة ملموسة
     * @brief (EN) Get the template source for a concrete instance
     * @param className (AR) اسم الصنف الملموس / (EN) Concrete class name
     * @return (AR) مؤشر لعقدة صنف القالب أو nullptr / (EN) Pointer to template class node or nullptr
     */
    AST::TemplateClassDecl* getTemplateInstanceSource(const std::string& className) {
        auto it = templateInstanceSources_.find(className);
        if (it != templateInstanceSources_.end()) return it->second;
        return nullptr;
    }
};

} // namespace Interpreter
} // namespace Sad
