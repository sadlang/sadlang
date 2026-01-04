/*
 * ============================================================================
 * Type Checker - مدقق الأنواع
 * Type Checker - Type Checker
 * ============================================================================
 * 
 * الوصف (Description):
 *   المدقق الرئيسي للأنواع - يتحقق من صحة الأنواع في الكود ويكتشف الأخطاء.
 *   يستخدم Type Inference لاستنتاج الأنواع تلقائياً.
 * 
 *   Main type checker - verifies type correctness and detects errors.
 *   Uses Type Inference to infer types automatically.
 * 
 * المكونات الرئيسية (Main Components):
 *   - TypeChecker: المدقق الرئيسي / Main checker
 *   - checkExpr(): التحقق من التعابير / Check expressions
 *   - checkStmt(): التحقق من الجمل / Check statements
 *   - checkDecl(): التحقق من التصريحات / Check declarations
 * 
 * الوظائف (Functions):
 *   1. Type checking: التحقق من توافق الأنواع
 *   2. Error detection: كشف أخطاء الأنواع
 *   3. Type inference integration: دمج Type Inference
 *   4. Symbol resolution: حل الرموز والمتغيرات
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_TYPE_CHECKER_H
#define SAD_TYPE_CHECKER_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "typed_ast.h"
#include "type_context.h"
#include "type_inference.h"
#include "type_errors.h"
#include "../../../../include/parser/ast/ast_node.h"
#include "../../../../include/parser/ast/expressions.h"
#include "../../../../include/parser/ast/statements.h"
#include "../../../../include/parser/ast/declarations.h"
#include "../../../../compiler/type_system/include/type_narrowing.h"

namespace Sad {
namespace TypeChecker {

// ============================================================================
// TypeChecker - المدقق الرئيسي للأنواع
// TypeChecker - Main Type Checker
// ============================================================================

/**
 * مدقق الأنواع الرئيسي
 * Main type checker
 * 
 * يتحقق من صحة الأنواع في جميع أجزاء الكود:
 * - التعابير (expressions)
 * - الجمل (statements)
 * - التصريحات (declarations)
 * - الدوال (functions)
 * - الأصناف (classes)
 */
class TypeChecker {
private:
    std::shared_ptr<TypeContext> context_;       // سياق Type Checking / Type checking context
    std::shared_ptr<TypeInference> inference_;   // محرك Type Inference / Type inference engine
    std::shared_ptr<TypeErrorReporter> reporter_; // مُبلِّغ الأخطاء / Error reporter
    bool hasErrors_;                             // هل توجد أخطاء / Has errors
    std::string currentFile_;                    // الملف الحالي / Current file
    
    // خرائط للتخزين المؤقت / Caching maps
    std::unordered_map<AST::ASTNode*, std::shared_ptr<Type>> typeCache_; // تخزين الأنواع / Type cache
    
    // Phase 1.3.5.3: Type Narrowing Integration / تكامل تضييق الأنواع
    TypeSystem::TypeNarrowingAnalyzer narrowingAnalyzer_; // محلل تضييق الأنواع / Type narrowing analyzer
    bool enableTypeNarrowing_;                            // تفعيل تضييق الأنواع / Enable type narrowing
    
public:
    /**
     * المنشئ / Constructor
     * 
     * @param context سياق Type checking / Type checking context
     */
    explicit TypeChecker(std::shared_ptr<TypeContext> context)
        : context_(context)
        , inference_(std::make_shared<TypeInference>(context))
        , reporter_(std::make_shared<TypeErrorReporter>())
        , hasErrors_(false)
        , currentFile_("")
        , enableTypeNarrowing_(true)  // مفعّل افتراضياً / Enabled by default
    {}
    
    /**
     * التحقق من برنامج كامل / Check complete program
     * 
     * @param program البرنامج (قائمة العبارات) / Program (statement list)
     * @return true إذا لا توجد أخطاء / true if no errors
     */
    bool checkProgram(const std::vector<AST::StmtPtr>& program);
    
    /**
     * التحقق من تعبير / Check expression
     * 
     * @param expr التعبير / Expression
     * @return نوع التعبير / Expression type
     */
    std::shared_ptr<Type> checkExpr(AST::Expression* expr);
    
    /**
     * التحقق من جملة / Check statement
     * 
     * @param stmt الجملة / Statement
     * @return true إذا صحيحة / true if valid
     */
    bool checkStmt(AST::Statement* stmt);
    
    /**
     * التحقق من تصريح متغير / Check variable declaration
     * 
     * @param decl التصريح / Declaration
     * @return true إذا صحيح / true if valid
     */
    bool checkVarDecl(AST::VarDeclStmt* decl);
    
    /**
     * التحقق من تصريح دالة / Check function declaration
     * 
     * @param decl التصريح / Declaration
     * @return true إذا صحيح / true if valid
     */
    bool checkFunctionDecl(AST::FunctionDecl* decl);
    
    /**
     * التحقق من تصريح صنف / Check class declaration
     * 
     * @param decl التصريح / Declaration
     * @return true إذا صحيح / true if valid
     */
    bool checkClassDecl(AST::ClassDecl* decl);
    
    /**
     * هل توجد أخطاء / Has errors
     * 
     * @return true إذا كانت هناك أخطاء / true if errors exist
     */
    bool hasErrors() const {
        return hasErrors_ || reporter_->hasErrors();
    }
    
    /**
     * الحصول على عدد الأخطاء / Get error count
     * 
     * @return عدد الأخطاء / Number of errors
     */
    size_t getErrorCount() const {
        return reporter_->getErrorCount();
    }
    
    /**
     * طباعة الأخطاء / Print errors
     */
    void printErrors() const {
        reporter_->printAllErrors();
    }
    
    /**
     * طباعة التحذيرات / Print warnings
     */
    void printWarnings() const {
        reporter_->printAllWarnings();
    }
    
    /**
     * إعادة تعيين المدقق / Reset checker
     */
    void reset() {
        context_->reset();
        inference_->reset();
        reporter_->clear();
        typeCache_.clear();
        hasErrors_ = false;
        narrowingAnalyzer_.resetContext();
    }
    
    /**
     * تفعيل أو تعطيل Type Narrowing / Enable or disable Type Narrowing
     * 
     * @param enable true لتفعيل / true to enable
     */
    void setTypeNarrowingEnabled(bool enable) {
        enableTypeNarrowing_ = enable;
    }
    
    /**
     * هل Type Narrowing مفعّل / Is Type Narrowing enabled
     * 
     * @return true إذا كان مفعّلاً / true if enabled
     */
    bool isTypeNarrowingEnabled() const {
        return enableTypeNarrowing_;
    }

private:
    // ========================================================================
    // دوال مساعدة للتحقق من التعابير / Helper functions for checking expressions
    // ========================================================================
    
    /**
     * التحقق من حرفي (literal) / Check literal
     */
    std::shared_ptr<Type> checkLiteral(AST::Expression* expr);
    
    /**
     * التحقق من متغير / Check variable
     */
    std::shared_ptr<Type> checkVariable(AST::VariableExpr* expr);
    
    /**
     * التحقق من عملية ثنائية / Check binary operation
     */
    std::shared_ptr<Type> checkBinaryOp(AST::BinaryExpr* expr);
    
    /**
     * التحقق من عملية أحادية / Check unary operation
     */
    std::shared_ptr<Type> checkUnaryOp(AST::UnaryExpr* expr);
    
    /**
     * التحقق من استدعاء دالة / Check function call
     */
    std::shared_ptr<Type> checkFunctionCall(AST::CallExpr* expr);
    
    /**
     * التحقق من الوصول لعضو / Check member access
     */
    std::shared_ptr<Type> checkMemberAccess(AST::MemberExpr* expr);
    
    /**
     * التحقق من الوصول لعنصر / Check array/dict access
     */
    std::shared_ptr<Type> checkIndexAccess(AST::IndexExpr* expr);
    
    /**
     * التحقق من دالة lambda / Check lambda function
     */
    std::shared_ptr<Type> checkLambda(AST::LambdaExpr* expr);
    
    /**
     * التحقق من إسناد / Check assignment
     */
    std::shared_ptr<Type> checkAssignment(AST::AssignExpr* expr);
    
    // ========================================================================
    // دوال مساعدة للتحقق من الجمل / Helper functions for checking statements
    // ========================================================================
    
    /**
     * التحقق من جملة تعبير / Check expression statement
     */
    bool checkExprStmt(AST::ExprStmt* stmt);
    
    /**
     * التحقق من جملة if / Check if statement
     */
    bool checkIfStmt(AST::IfStmt* stmt);
    
    /**
     * التحقق من جملة while / Check while statement
     */
    bool checkWhileStmt(AST::WhileStmt* stmt);
    
    /**
     * التحقق من جملة for / Check for statement
     */
    bool checkForStmt(AST::ForStmt* stmt);
    
    /**
     * التحقق من جملة return / Check return statement
     */
    bool checkReturnStmt(AST::ReturnStmt* stmt);
    
    /**
     * التحقق من جملة break / Check break statement
     */
    bool checkBreakStmt(AST::BreakStmt* stmt);
    
    /**
     * التحقق من جملة continue / Check continue statement
     */
    bool checkContinueStmt(AST::ContinueStmt* stmt);
    
    /**
     * التحقق من كتلة / Check block
     */
    bool checkBlock(AST::BlockStmt* stmt);
    
    // ========================================================================
    // دوال مساعدة عامة / General helper functions
    // ========================================================================
    
    /**
     * التحقق من توافق نوعين / Check type compatibility
     * 
     * @param expected النوع المتوقع / Expected type
     * @param actual النوع الفعلي / Actual type
     * @param node العقدة المصدرية / Source node
     * @return true إذا متوافقان / true if compatible
     */
    bool checkTypeCompatibility(
        std::shared_ptr<Type> expected,
        std::shared_ptr<Type> actual,
        AST::ASTNode* node
    );
    
    /**
     * إضافة خطأ نوع / Add type error
     * 
     * @param message رسالة الخطأ / Error message
     * @param node العقدة المصدرية / Source node
     */
    void addError(const std::string& message, AST::ASTNode* node = nullptr);
    
    /**
     * إضافة تحذير / Add warning
     * 
     * @param message رسالة التحذير / Warning message
     * @param node العقدة المصدرية / Source node
     */
    void addWarning(const std::string& message, AST::ASTNode* node = nullptr);
    
    /**
     * الحصول على نوع من التخزين المؤقت / Get type from cache
     * 
     * @param node العقدة / Node
     * @return النوع أو nullptr / Type or nullptr
     */
    std::shared_ptr<Type> getCachedType(AST::ASTNode* node);
    
    /**
     * حفظ نوع في التخزين المؤقت / Save type to cache
     * 
     * @param node العقدة / Node
     * @param type النوع / Type
     */
    void cacheType(AST::ASTNode* node, std::shared_ptr<Type> type);
    
    /**
     * تحويل DataType إلى Type / Convert DataType to Type
     * 
     * @param dataType نوع البيانات القديم / Old data type
     * @return النوع الجديد / New type
     */
    std::shared_ptr<Type> dataTypeToType(Data::DataType dataType);
    
    /**
     * التحقق من أن النوع boolean / Check if type is boolean
     * 
     * @param type النوع / Type
     * @param node العقدة / Node
     * @return true إذا كان boolean / true if boolean
     */
    bool requireBoolType(std::shared_ptr<Type> type, AST::ASTNode* node);
    
    /**
     * التحقق من أن النوع رقمي / Check if type is numeric
     * 
     * @param type النوع / Type
     * @param node العقدة / Node
     * @return true إذا كان رقمياً / true if numeric
     */
    bool requireNumericType(std::shared_ptr<Type> type, AST::ASTNode* node);
};

} // namespace TypeChecker
} // namespace Sad

#endif // SAD_TYPE_CHECKER_H
