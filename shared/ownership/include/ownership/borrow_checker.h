// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file borrow_checker.h
 * @brief فاحص الاستعارة / Borrow Checker
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 *
 * @details
 * (AR) فاحص الاستعارة يحلل AST ويتحقق من قواعد الملكية:
 *      - لا استخدام بعد النقل
 *      - لا استعارتين متغيرتين في نفس الوقت
 *      - لا خلط بين الاستعارة المتغيرة والثابتة
 *      - لا مراجع معلقة (dangling references)
 *
 * (EN) Borrow Checker analyzes AST and verifies ownership rules:
 *      - No use after move
 *      - No two mutable borrows at once
 *      - No mixing mutable and immutable borrows
 *      - No dangling references
 *
 * @see ownership_tracker.h
 * @see lifetime_analyzer.h
 */

#ifndef SAD_BORROW_CHECKER_H
#define SAD_BORROW_CHECKER_H

#include "ownership/ownership_tracker.h"
#include "ast_visitor.h"
#include "ast_node.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>

namespace Sad
{
    namespace Semantic
    {

        // Forward declarations
        struct BorrowCheckResult;

        // ============================================================================
        // نتيجة فحص الاستعارة / Borrow Check Result
        // ============================================================================

        /**
         * @struct BorrowCheckResult
         * @brief (AR) نتيجة فحص الاستعارة للبرنامج
         * @brief (EN) Borrow check result for the program
         */
        struct BorrowCheckResult
        {
            bool success;                       ///< (AR) هل نجح الفحص؟ / (EN) Did check pass?
            std::vector<OwnershipError> errors; ///< (AR) قائمة الأخطاء / (EN) List of errors
            std::vector<std::string> warnings;  ///< (AR) قائمة التحذيرات / (EN) List of warnings
            size_t totalVariables;              ///< (AR) عدد المتغيرات المفحوصة / (EN) Variables checked
            size_t totalBorrows;                ///< (AR) عدد الاستعارات المفحوصة / (EN) Borrows checked
            size_t totalMoves;                  ///< (AR) عدد عمليات النقل / (EN) Moves checked

            BorrowCheckResult()
                : success(true), totalVariables(0), totalBorrows(0), totalMoves(0) {}

            void addError(const OwnershipError &error)
            {
                errors.push_back(error);
                success = false;
            }

            void addWarning(const std::string &warning)
            {
                warnings.push_back(warning);
            }

            std::string getSummary() const;
            std::string getArabicSummary() const;
        };

        // ============================================================================
        // فاحص الاستعارة / Borrow Checker
        // ============================================================================

        /**
         * @class BorrowChecker
         * @brief (AR) فاحص الاستعارة الرئيسي
         * @brief (EN) Main borrow checker
         *
         * @details
         * (AR) يمشي على AST ويتحقق من صحة جميع عمليات الملكية:
         *      - يتتبع تعريف المتغيرات
         *      - يتتبع نقل الملكية
         *      - يتتبع الاستعارات
         *      - يتحقق من القواعد عند كل عملية
         *
         * (EN) Walks AST and validates all ownership operations:
         *      - Tracks variable definitions
         *      - Tracks ownership transfers
         *      - Tracks borrows
         *      - Validates rules at each operation
         *
         * @example
         * @code{.cpp}
         * BorrowChecker checker;
         * auto result = checker.check(ast);
         * if (!result.success) {
         *     for (const auto& err : result.errors) {
         *         std::cerr << err.toArabicString() << std::endl;
         *     }
         * }
         * @endcode
         */
        class BorrowChecker : public AST::ASTVisitor
        {
        public:
            // ==================================================================
            // بناء وهدم / Construction & Destruction
            // ==================================================================

            BorrowChecker();
            ~BorrowChecker() override;

            // ==================================================================
            // واجهة الفحص الرئيسية / Main Check Interface
            // ==================================================================

            /**
             * @brief (AR) فحص AST كامل
             * @brief (EN) Check complete AST
             *
             * @param ast (AST::ASTNode*) — (AR) جذر AST / (EN) AST root
             * @return (BorrowCheckResult) — (AR) نتيجة الفحص / (EN) Check result
             */
            BorrowCheckResult check(AST::ASTNode *ast);

            /**
             * @brief (AR) فحص دالة واحدة
             * @brief (EN) Check single function
             *
             * @param func (AST::FunctionDecl*) — (AR) تصريح الدالة / (EN) Function declaration
             * @return (BorrowCheckResult) — (AR) نتيجة الفحص / (EN) Check result
             */
            BorrowCheckResult checkFunction(AST::FunctionDecl *func);

            /**
             * @brief (AR) إعادة تعيين الفاحص
             * @brief (EN) Reset checker
             */
            void reset();

            // ==================================================================
            // إعدادات / Settings
            // ==================================================================

            /**
             * @brief (AR) تعيين وضع الرسائل (عربي/إنجليزي)
             * @brief (EN) Set message mode (Arabic/English)
             */
            void setArabicMessages(bool arabic) { useArabicMessages_ = arabic; }

            /**
             * @brief (AR) تعيين وضع التنقيح
             * @brief (EN) Set debug mode
             */
            void setDebugMode(bool debug) { debugMode_ = debug; }

            /**
             * @brief (AR) تعيين وضع NLL (Non-Lexical Lifetimes)
             * @brief (EN) Set NLL mode
             */
            void setNLLMode(bool nll) { nllMode_ = nll; }

            // ==================================================================
            // زيارة عقد AST / AST Node Visiting (ASTVisitor interface)
            // ==================================================================

            // Expressions / التعابير
            void visitBinaryExpr(AST::BinaryExpr &expr) override;
            void visitUnaryExpr(AST::UnaryExpr &expr) override;
            void visitTernaryExpr(AST::TernaryExpr &expr) override;
            void visitLiteralExpr(AST::LiteralExpr &expr) override;
            void visitVariableExpr(AST::VariableExpr &expr) override;
            void visitAssignExpr(AST::AssignExpr &expr) override;
            void visitCallExpr(AST::CallExpr &expr) override;
            void visitIndexExpr(AST::IndexExpr &expr) override;
            void visitMemberExpr(AST::MemberExpr &expr) override;
            void visitMemberAssignExpr(AST::MemberAssignExpr &expr) override;
            void visitIndexAssignExpr(AST::IndexAssignExpr &expr) override;
            void visitArrayExpr(AST::ArrayExpr &expr) override;
            void visitMapExpr(AST::MapExpr &expr) override;
            void visitWalrusExpr(AST::WalrusExpr &expr) override;
            void visitAwaitExpr(AST::AwaitExpr &expr) override;
            void visitLambdaExpr(AST::LambdaExpr &expr) override;
            void visitListComprehensionExpr(AST::ListComprehensionExpr &expr) override;
            void visitDictComprehensionExpr(AST::DictComprehensionExpr &expr) override;
            void visitSetComprehensionExpr(AST::SetComprehensionExpr &expr) override;
            void visitGeneratorExpr(AST::GeneratorExpr &expr) override;
            void visitDecoratorExpr(AST::DecoratorExpr &expr) override;
            void visitNewExpr(AST::NewExpr &expr) override;
            void visitMemberAccessExpr(AST::MemberAccessExpr &expr) override;
            void visitMethodCallExpr(AST::MethodCallExpr &expr) override;
            void visitThisExpr(AST::ThisExpr &expr) override;
            void visitSuperExpr(AST::SuperExpr &expr) override;
            void visitBorrowExpr(AST::BorrowExpr &expr) override;
            void visitInlineAsmExpr(AST::InlineAsmExpr &expr) override;
            void visitRangeExpr(AST::RangeExpr &expr) override;
            void visitSliceExpr(AST::SliceExpr &expr) override {}
            void visitOptionalChainExpr(AST::OptionalChainExpr &expr) override;
            void visitNullCoalesceExpr(AST::NullCoalesceExpr &expr) override;
            void visitErrorPropagateExpr(AST::ErrorPropagateExpr &expr) override;
            void visitTupleExpr(AST::TupleExpr &expr) override;

            // (AR) دوال زائر واجهة المستخدم — غير مستخدمة في فاحص الاستعارة
            // (EN) UI visitor methods — not used in borrow checker
            void visitUIDeclaration(AST::UIDeclarationNode &) override {}
            void visitUIWidgetExpr(AST::UIWidgetExprNode &) override {}
            void visitUIModifier(AST::UIModifierNode &) override {}
            void visitUIEventHandler(AST::UIEventHandlerNode &) override {}

            // Statements / العبارات
            void visitExprStmt(AST::ExprStmt &stmt) override;
            void visitVarDeclStmt(AST::VarDeclStmt &stmt) override;
            void visitIfStmt(AST::IfStmt &stmt) override;
            void visitWhileStmt(AST::WhileStmt &stmt) override;
            void visitForStmt(AST::ForStmt &stmt) override;
            void visitForRangeStmt(AST::ForRangeStmt &stmt) override;
            void visitSwitchStmt(AST::SwitchStmt &stmt) override;
            void visitReturnStmt(AST::ReturnStmt &stmt) override;
            void visitYieldStmt(AST::YieldStmt &stmt) override;
            void visitBreakStmt(AST::BreakStmt &stmt) override;
            void visitContinueStmt(AST::ContinueStmt &stmt) override;
            void visitBlockStmt(AST::BlockStmt &stmt) override;
            void visitTryStmt(AST::TryStmt &stmt) override;
            void visitRaiseStmt(AST::RaiseStmt &stmt) override;
            void visitWithStmt(AST::WithStmt &stmt) override;
            void visitMatchStmt(AST::MatchStmt &stmt) override;
            void visitDeferStmt(AST::DeferStmt &stmt) override;
            void visitGoStmt(AST::GoStmt &stmt) override;
            void visitSelectCase(AST::SelectCase &stmt) override;
            void visitSelectStmt(AST::SelectStmt &stmt) override;
            void visitClassDeclStmt(AST::ClassDeclStmt &stmt) override;

            // Declarations / التصريحات
            void visitFunctionDecl(AST::FunctionDecl &decl) override;
            void visitClassDecl(AST::ClassDecl &decl) override;
            void visitFieldDecl(AST::FieldDecl &decl) override;
            void visitMethodDecl(AST::MethodDecl &decl) override;
            void visitPropertyDecl(AST::PropertyDecl &decl) override;
            void visitConstructorDecl(AST::ConstructorDecl &decl) override;
            void visitDestructorDecl(AST::DestructorDecl &decl) override;
            void visitEnumDecl(AST::EnumDecl &decl) override;
            void visitImportStmt(AST::ImportStmt &stmt) override;
            void visitFromImportStmt(AST::FromImportStmt &stmt) override;
            void visitExportStmt(AST::ExportStmt &stmt) override;
            void visitExportDecl(AST::ExportDecl &decl) override;
            void visitReExportStmt(AST::ReExportStmt &stmt) override;
            void visitTemplateFunctionDecl(AST::TemplateFunctionDecl &decl) override;
            void visitTemplateClassDecl(AST::TemplateClassDecl &decl) override;
            void visitTemplateInstantiation(AST::TemplateInstantiation &inst) override;
            void visitNamespaceDecl(AST::NamespaceDecl &decl) override;
            void visitOperatorDecl(AST::OperatorDecl &decl) override;
            void visitTraitDecl(AST::TraitDecl &decl) override;
            void visitImplDecl(AST::ImplDecl &decl) override;
            void visitStructDecl(AST::StructDecl &decl) override;
            void visitTestDecl(AST::TestDecl &decl) override;
            void visitExtensionDecl(AST::ExtensionDecl &decl) override;
            void visitMacroDecl(AST::MacroDecl &decl) override;
            void visitTypeAliasDecl(AST::TypeAliasDecl &decl) override;
            void visitTupleDestructureStmt(AST::TupleDestructureStmt &stmt) override;

            // Low-level visitor stubs (required by ASTVisitor)
            void visitUnsafeBlockStmt(AST::UnsafeBlockStmt &stmt) override {}
            void visitComptimeBlockStmt(AST::ComptimeBlockStmt &stmt) override {}
            void visitSizeofExpr(AST::SizeofExpr &expr) override {}
            void visitAtomicExpr(AST::AtomicExpr &expr) override {}
            void visitVolatileVarDeclStmt(AST::VolatileVarDeclStmt &stmt) override {}

        protected:
            // ==================================================================
            // دوال مساعدة / Helper Functions
            // ==================================================================

            /**
             * @brief (AR) تحليل تعبير للملكية
             * @brief (EN) Analyze expression for ownership
             */
            void analyzeExpression(AST::Expression *expr, bool isMoveContext);

            /**
             * @brief (AR) تحليل تعبير الإسناد
             * @brief (EN) Analyze assignment expression
             */
            void analyzeAssignment(AST::AssignExpr *assign);

            /**
             * @brief (AR) تحليل استدعاء دالة
             * @brief (EN) Analyze function call
             */
            void analyzeFunctionCall(AST::CallExpr *call);

            /**
             * @brief (AR) التحقق من أن النوع قابل للنسخ
             * @brief (EN) Check if type is Copy
             */
            bool isCopyType(const std::string &typeName) const;

            /**
             * @brief (AR) تحويل نوع البيانات إلى نص
             * @brief (EN) Convert DataType enum to string name
             */
            std::string dataTypeToString(Types::SadTypeKind type) const;

            /**
             * @brief (AR) الحصول على موقع المصدر من عقدة
             * @brief (EN) Get source location from node
             */
            SourceLocation getLocation(AST::ASTNode *node) const;

            /**
             * @brief (AR) تسجيل خطأ
             * @brief (EN) Record error
             */
            void recordError(const OwnershipError &error);

            /**
             * @brief (AR) تسجيل تحذير
             * @brief (EN) Record warning
             */
            void recordWarning(const std::string &warning);

        private:
            // ==================================================================
            // بيانات خاصة / Private Data
            // ==================================================================

            /// (AR) متتبع الملكية / (EN) Ownership tracker
            std::unique_ptr<OwnershipTracker> tracker_;

            /// (AR) نتيجة الفحص الحالية / (EN) Current check result
            BorrowCheckResult currentResult_;

            /// (AR) استخدام الرسائل العربية / (EN) Use Arabic messages
            bool useArabicMessages_;

            /// (AR) وضع التنقيح / (EN) Debug mode
            bool debugMode_;

            /// (AR) وضع NLL / (EN) NLL mode
            bool nllMode_;

            /// (AR) اسم الملف الحالي / (EN) Current file name
            std::string currentFile_;

            /// (AR) سياق الدالة الحالية / (EN) Current function context
            std::string currentFunction_;

            /// (AR) مجموعة الأنواع القابلة للنسخ / (EN) Set of Copy types
            std::unordered_set<std::string> copyTypes_;

            // ==================================================================
            // تهيئة الأنواع / Type Initialization
            // ==================================================================

            void initializeCopyTypes();
        };

        // ============================================================================
        // تنفيذ BorrowCheckResult::getSummary
        // ============================================================================

        inline std::string BorrowCheckResult::getSummary() const
        {
            std::ostringstream oss;
            oss << "Borrow Check Summary:\n";
            oss << "  Status: " << (success ? "PASSED" : "FAILED") << "\n";
            oss << "  Variables checked: " << totalVariables << "\n";
            oss << "  Borrows checked: " << totalBorrows << "\n";
            oss << "  Moves checked: " << totalMoves << "\n";
            oss << "  Errors: " << errors.size() << "\n";
            oss << "  Warnings: " << warnings.size() << "\n";
            return oss.str();
        }

        inline std::string BorrowCheckResult::getArabicSummary() const
        {
            std::ostringstream oss;
            oss << "ملخص فحص الاستعارة:\n";
            oss << "  الحالة: " << (success ? "نجح ✓" : "فشل ✗") << "\n";
            oss << "  المتغيرات المفحوصة: " << totalVariables << "\n";
            oss << "  الاستعارات المفحوصة: " << totalBorrows << "\n";
            oss << "  عمليات النقل: " << totalMoves << "\n";
            oss << "  الأخطاء: " << errors.size() << "\n";
            oss << "  التحذيرات: " << warnings.size() << "\n";
            return oss.str();
        }

    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_BORROW_CHECKER_H
