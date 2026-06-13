// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file type_checker.h
 * @brief فاحص الأنواع / Type Checker
 * @author فريق لغة ص / Sad Language Team
 * @date فبراير 2026 / February 2026
 * @version 1.0
 * @phase Phase 5: US3 - نظام الأنواع المتقدم
 *
 * @details
 * (AR) فاحص الأنواع يربط بين شجرة AST ونظام الأنواع المتقدم:
 *      - يزور عقد AST ويستنتج أنواعها
 *      - يتحقق من توافق الأنواع في التعيينات والاستدعاءات
 *      - يدعم المعممات والسمات
 *      - يكتشف أخطاء الأنواع في وقت الترجمة
 *
 * (EN) Type Checker bridges AST tree and advanced type system:
 *      - Visits AST nodes and infers their types
 *      - Checks type compatibility in assignments and calls
 *      - Supports generics and traits
 *      - Detects type errors at compile time
 */

#ifndef SAD_TYPE_CHECKER_H
#define SAD_TYPE_CHECKER_H

#include "ast_visitor.h"
#include "ast_node.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include "types/type.h"
#include "types/type_registry.h"
#include "types/primitive_type.h"
#include "types/type_inferencer.h"
#include "types/arabic_types.h"
#include "types/struct_types.h"
#include "types/enum_types.h"
#include "types/generics.h"
#include "types/trait_system.h"
#include "types/union_types.h"
#include "sad_type_system.h" // (AR) SadTypeKind — محور المدقّق بعد S-TS-P2

#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace Sad
{
    namespace Semantic
    {

        // ============================================================================
        // نتيجة فحص الأنواع / Type Check Result
        // ============================================================================

        /**
         * @struct TypeCheckError
         * @brief (AR) خطأ في فحص الأنواع
         * @brief (EN) Type check error
         */
        struct TypeCheckError
        {
            std::string variableName;
            std::string expectedType;
            std::string actualType;
            size_t line;
            size_t column;
            std::string message;
            std::string arabicMessage;

            std::string toEnglishString() const;
            std::string toArabicString() const;
        };

        /**
         * @struct TypeCheckResult
         * @brief (AR) نتيجة فحص الأنواع للبرنامج
         * @brief (EN) Type check result for the program
         */
        struct TypeCheckResult
        {
            bool success = true;
            size_t totalExpressions = 0;
            size_t totalVariables = 0;
            size_t totalFunctions = 0;
            size_t totalInferred = 0;
            std::vector<TypeCheckError> errors;
            std::vector<std::string> warnings;
            std::vector<std::string> moduleDependencies; // اعتماديات الوحدات المستوردة

            void addError(const TypeCheckError &error)
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
        // فاحص الأنواع / Type Checker
        // ============================================================================

        /**
         * @class TypeChecker
         * @brief (AR) فاحص الأنواع - يزور AST ويتحقق من الأنواع
         * @brief (EN) Type Checker - visits AST and checks types
         */
        class TypeChecker : public AST::BaseASTVisitor
        {
        public:
            // ==================================================================
            // بناء وهدم / Construction & Destruction
            // ==================================================================

            TypeChecker();
            ~TypeChecker() = default;

            // ==================================================================
            // واجهة الفحص الرئيسية / Main Check Interface
            // ==================================================================

            /**
             * @brief (AR) فحص AST كامل
             * @brief (EN) Check complete AST
             */
            TypeCheckResult check(AST::ASTNode *ast);

            /**
             * @brief (AR) الحصول على نتيجة الفحص الحالية
             * @brief (EN) Get the current check result
             */
            const TypeCheckResult &getResult() const { return currentResult_; }

            /**
             * @brief (AR) طباعة ملخص النتائج
             * @brief (EN) Print results summary
             */
            void printSummary() const;

            /**
             * @brief (AR) إعادة تعيين الفاحص
             * @brief (EN) Reset checker
             */
            void reset();

            // ==================================================================
            // إعدادات / Settings
            // ==================================================================

            void setArabicMessages(bool arabic) { useArabicMessages_ = arabic; }
            void setDebugMode(bool debug) { debugMode_ = debug; }
            void setStrictMode(bool strict) { strictMode_ = strict; }

            // ==================================================================
            // زيارة عقد AST / AST Node Visiting
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
            void visitArrayExpr(AST::ArrayExpr &expr) override;
            void visitMapExpr(AST::MapExpr &expr) override;
            void visitWalrusExpr(AST::WalrusExpr &expr) override;
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
            void visitOptionalChainExpr(AST::OptionalChainExpr &expr) override;
            void visitNullCoalesceExpr(AST::NullCoalesceExpr &expr) override;

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
            void visitGoStmt(AST::GoStmt &stmt) override;         // (AR) أطلق — تنفيذ متزامن / (EN) Go statement
            void visitSelectCase(AST::SelectCase &stmt) override; // (AR) حالة اختر / (EN) Select case
            void visitSelectStmt(AST::SelectStmt &stmt) override; // (AR) جملة اختر / (EN) Select statement
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
            // (AR) [Phase 5 — Template Constraints] زوار السمات والتنفيذات
            //      visitTraitDecl: يسجل السمة في userTraits_ (مع superTraits)
            //      visitImplDecl: يسجل أن النوع X ينفذ السمة Y
            //      visitTemplateInstantiation أعلاه: يفرض القيود ضد typeImpls_
            // (EN) [Phase 5 — Template Constraints] trait/impl visitors
            //      Used to enforce `T: Trait1 + Trait2` constraints on template params.
            void visitTraitDecl(AST::TraitDecl &decl) override;
            void visitImplDecl(AST::ImplDecl &decl) override;

        protected:
            // ==================================================================
            // دوال مساعدة / Helper Functions
            // ==================================================================

            /**
             * @brief (AR) تحويل SadTypeKind إلى TypePtr من نظام الأنواع المتقدم (S-TS-P2: المحور SadTypeKind)
             * @brief (EN) Convert SadTypeKind to advanced type system TypePtr (S-TS-P2: SadTypeKind-centric)
             *
             * (AR) قراءات الـAST (DataType) تُجسَّر عبر Types::fromDataType عند نقطة الاستدعاء؛
             *      هذا الجسر يُحذف في S-TS-P2.5a عند ترحيل حقول الـAST إلى SadTypeKind.
             */
            TypeSystem::TypePtr sadKindToTypePtr(Types::SadTypeKind kind) const;

            /**
             * @brief (AR) جسر حدود الـAST: DataType → SadTypeKind → TypePtr (يُحذف في S-TS-P2.5a)
             * @brief (EN) AST-boundary bridge: DataType → SadTypeKind → TypePtr (removed in S-TS-P2.5a)
             */
            TypeSystem::TypePtr dataTypeToTypePtr(Data::DataType dt) const;

            /**
             * @brief (AR) التحقق من توافق نوعين
             * @brief (EN) Check if two types are compatible
             */
            bool areTypesCompatible(TypeSystem::TypePtr expected, TypeSystem::TypePtr actual) const;

            /**
             * @brief (AR) استنتاج نوع تعبير
             * @brief (EN) Infer expression type
             */
            TypeSystem::TypePtr inferExprType(AST::Expression *expr);

            /**
             * @brief (AR) الحصول على موقع من عقدة
             * @brief (EN) Get location from node
             */
            std::pair<size_t, size_t> getLocation(AST::ASTNode *node) const;

            /**
             * @brief (AR) تسجيل خطأ نوع
             * @brief (EN) Record type error
             */
            void recordTypeError(const std::string &varName,
                                 const std::string &expected,
                                 const std::string &actual,
                                 AST::ASTNode *node,
                                 const std::string &msg = "");

            /**
             * @brief (AR) الدخول/الخروج من نطاق
             * @brief (EN) Enter/exit scope
             */
            void enterScope();
            void exitScope();

            /**
             * @brief (AR) تسجيل متغير في النطاق الحالي
             * @brief (EN) Register variable in current scope
             */
            void declareVariable(const std::string &name, TypeSystem::TypePtr type);

            /**
             * @brief (AR) البحث عن نوع متغير
             * @brief (EN) Look up variable type
             */
            TypeSystem::TypePtr lookupVariable(const std::string &name) const;

        private:
            // ==================================================================
            // بيانات خاصة / Private Data
            // ==================================================================

            /// نتيجة الفحص الحالية / Current check result
            TypeCheckResult currentResult_;

            /// استخدام الرسائل العربية / Use Arabic messages
            bool useArabicMessages_ = true;

            /// وضع التنقيح / Debug mode
            bool debugMode_ = false;

            /// وضع صارم / Strict mode (reject any type mismatch)
            bool strictMode_ = false;

            /// سجل الأنواع / Type registry
            TypeSystem::TypeRegistry &registry_;

            /// آخر نوع مُستنتج (للتمرير بين visitor calls)
            /// Last inferred type (for passing between visitor calls)
            TypeSystem::TypePtr lastInferredType_;

            /// بيئة الأنواع الحالية / Current type environment
            std::shared_ptr<TypeSystem::TypeEnvironment> currentEnv_;

            /// مكدس النطاقات / Scope stack
            std::vector<std::shared_ptr<TypeSystem::TypeEnvironment>> scopeStack_;

            /// اسم الدالة الحالية / Current function name
            std::string currentFunction_;

            /// نوع الإرجاع المتوقع للدالة الحالية / Expected return type
            TypeSystem::TypePtr expectedReturnType_;

            // ==================================================================
            // [Phase 5 — Template Constraints] سجلات القوالب والسمات
            // [Phase 5 — Template Constraints] Templates & traits registries
            // ==================================================================
            // (AR) سجل القوالب (دوال + أصناف) لاسترجاعها عند instantiation.
            //      نُخزّن نسخة مبسّطة من المعاملات (الاسم + isConst + القيود)
            //      لأن AST::TypeParameter يحوي ExprPtr (defaultType) غير قابل للنسخ.
            // (EN) Template registry. Stores a simplified param copy (name +
            //      isConst + constraints) since AST::TypeParameter holds a
            //      non-copy-assignable ExprPtr (defaultType).
            struct ParamInfo
            {
                std::string name;
                bool isConst = false;
                std::vector<std::string> constraints;
            };
            struct WhereItemInfo
            {
                std::string typeName;
                std::vector<std::string> constraints;
            };
            struct TemplateInfo
            {
                std::vector<ParamInfo> typeParameters;
                std::vector<WhereItemInfo> whereItems;
                bool isClass = false;

                // ==========================================================
                // (AR) [Phase 5b] أسماء أنواع معاملات الدالة كما كُتبت في
                //      تعريف القالب — تُستخدم للاستدلال النوعي عند نقاط
                //      الاستدعاء. مثال: دالة أكبر(T أ، T ب) → ["T", "T"].
                //      الفهرس i يقابل المعامل i من parameters.
                // (EN) [Phase 5b] Function parameter type-name strings as
                //      written in the template declaration. Used for type
                //      inference at call sites. Example: func max(T a, T b)
                //      → ["T", "T"]. Index i matches parameters[i].
                // ==========================================================
                std::vector<std::string> paramTypeNames;
            };

            /// (AR) قوالب المستخدم بالاسم / (EN) user templates by name
            std::unordered_map<std::string, TemplateInfo> userTemplates_;

            /// (AR) سمات المستخدم بالاسم → سمات أم / (EN) user traits by name → super traits
            std::unordered_map<std::string, std::vector<std::string>> userTraits_;

            /// (AR) خريطة: اسم النوع → مجموعة أسماء السمات التي ينفذها
            /// (EN) Map: type name → set of trait names it implements
            std::unordered_map<std::string, std::unordered_set<std::string>> typeImpls_;

            // ==================================================================
            // (AR) [Phase 5b] توقيعات دوال السمات + تحقق التنفيذات
            // (EN) [Phase 5b] Trait method signatures + impl verification
            // ==================================================================

            /// (AR) توقيع دالة في سمة (لمطابقة التنفيذ).
            /// (EN) Trait method signature (for impl matching).
            struct TraitMethodSig
            {
                std::string name;        // اسم الدالة / Method name
                size_t paramCount = 0;   // عدد المعاملات / Param count (excl. self/this)
                bool hasDefault = false; // له تنفيذ افتراضي؟ / Has default impl?
                // (AR) [Phase 5d] أسماء أنواع المعاملات (قد تكون فارغة لو
                //      المعامل بدون نوع مُصرَّح). تُستخدم لمقارنة توقيعات
                //      التنفيذ مقابل توقيعات السمة.
                // (EN) [Phase 5d] Parameter type names (may be empty if a
                //      param is untyped). Used to match impl signatures
                //      against trait signatures.
                std::vector<std::string> paramTypes;
                // (AR) [Phase 5d] اسم نوع القيمة المُرجعة (قد يكون فارغاً).
                // (EN) [Phase 5d] Return type name (may be empty).
                std::string returnType;
            };

            /// (AR) دوال كل سمة بالاسم / (EN) Methods of each trait by name
            std::unordered_map<std::string, std::vector<TraitMethodSig>> traitMethods_;

            // ==================================================================
            // (AR) أسماء الأصناف المعرفة في الوحدة الحالية. نستخدمها كمرجع
            //      احتياطي عند الاستدلال النوعي بدل الاعتماد الكامل على
            //      StructRegistry الذي قد لا يكون محدثاً في بعض المسارات.
            // (EN) Class names declared in current compilation unit. Used as
            //      a fallback source for inference when StructRegistry is not
            //      fully populated in some semantic paths.
            // ==================================================================
            std::unordered_set<std::string> userClassNames_;

            // ==================================================================
            // (AR) [Phase 5c] تتبّع أسماء أصناف المتغيرات للاستدلال النوعي
            //      مكدس متوازٍ مع scopeStack_ — كل نطاق يحوي خريطة
            //      متغير → اسم الصنف. يُملأ من VarDeclStmt حين تكون
            //      القيمة الابتدائية NewExpr أو CallExpr لاسم صنف معروف.
            // (EN) [Phase 5c] Track variable class names for type inference.
            //      Stack parallel to scopeStack_; each scope holds a map
            //      varName → className. Filled in VarDeclStmt when the
            //      initializer is NewExpr or a CallExpr targeting a known
            //      class name (constructor-call syntax).
            // ==================================================================
            std::vector<std::unordered_map<std::string, std::string>>
                variableClassNames_;

            // ==================================================================
            // [Phase 5] دوال مساعدة للقيود / Constraint helpers
            // ==================================================================
            bool typeSatisfiesTrait(const std::string &typeName,
                                    const std::string &traitName) const;

            bool isKnownTrait(const std::string &traitName) const;

            int enforceParamConstraints(const ParamInfo &param,
                                        const std::string &typeArgName,
                                        const Lexer::Position &pos);

            // ==================================================================
            // (AR) [Phase 5b] مساعدات للاستدلال النوعي وتحقق التنفيذات
            // (EN) [Phase 5b] Helpers for type inference + impl verification
            // ==================================================================

            /**
             * @brief (AR) استنتج وسائط الأنواع لقالب من أنواع وسائط الاستدعاء.
             * @brief (EN) Infer template type arguments from call argument types.
             *
             * @param tmpl   تعريف القالب المرشح / candidate template info
             * @param argTypes  أسماء أنواع الوسائط (بالترتيب) / call arg type names
             * @param[out] inferred  أسماء الأنواع المُستنتَجة (بترتيب typeParameters)
             *                      أو فارغة إن فشل الاستدلال
             * @return true عند النجاح، false عند الفشل/الغموض
             */
            bool inferTemplateTypeArguments(
                const TemplateInfo &tmpl,
                const std::vector<std::string> &argTypeNames,
                std::vector<std::string> &inferred) const;

            /**
             * @brief (AR) تحقق أن impl يوفر جميع دوال السمة المطلوبة.
             * @brief (EN) Verify an impl provides all required trait methods.
             *
             * يجمع الدوال المطلوبة من السمة + سماتها الأم بشكل متعدٍّ،
             * ثم يقارنها بمجموعة الدوال المنفّذة. يُسجّل خطأً لكل دالة
             * مفقودة (إلا إن كان لها تنفيذ افتراضي في السمة).
             */
            void verifyImplCompleteness(const std::string &traitName,
                                        const std::string &targetType,
                                        const std::unordered_map<std::string, TraitMethodSig>
                                            &providedMethods,
                                        const Lexer::Position &pos);

            /**
             * @brief (AR) استخرج اسم نوع من تعبير AST (للاستدلال).
             * @brief (EN) Extract type name from AST expression (for inference).
             */
            std::string extractTypeNameFromExpr(AST::Expression *expr) const;

            /**
             * @brief (AR) [Phase 5d] استخرج اسم النوع من Parameter — يفضّل
             *        typeName (للأصناف) ويرجع تسمية عربية لـ DataType
             *        البدائية. سلسلة فارغة لو النوع غير معروف.
             * @brief (EN) [Phase 5d] Extract type name from a Parameter —
             *        prefers typeName (for classes), falls back to Arabic
             *        name for primitive DataType. Empty if unknown.
             */
            std::string paramTypeNameOf(const AST::Parameter &p) const;

            /**
             * @brief (AR) [Phase 5d / S-TS-P2] حوّل SadTypeKind إلى اسم عربي (محور SadTypeKind).
             * @brief (EN) [Phase 5d / S-TS-P2] Map SadTypeKind to Arabic name (SadTypeKind-centric).
             *
             * (AR) السلاسل المُرجَعة مطابقة للسابق (لا تغيير في رسائل الأخطاء)؛
             *      قراءات الـAST تُجسَّر عبر Types::fromDataType حتى S-TS-P2.5a.
             */
            std::string sadKindArabicName(Types::SadTypeKind t) const;

            /**
             * @brief (AR) جسر حدود الـAST: DataType → SadTypeKind → اسم عربي (يُحذف في S-TS-P2.5a)
             * @brief (EN) AST-boundary bridge: DataType → SadTypeKind → Arabic name (removed in S-TS-P2.5a)
             */
            std::string dataTypeArabicName(Data::DataType t) const;
        };

        // ============================================================================
        // تنفيذ getSummary و getArabicSummary
        // ============================================================================

        inline std::string TypeCheckResult::getSummary() const
        {
            std::ostringstream oss;
            oss << "Type Check Summary:\n";
            oss << "  Status: " << (success ? "PASSED" : "FAILED") << "\n";
            oss << "  Variables checked: " << totalVariables << "\n";
            oss << "  Functions checked: " << totalFunctions << "\n";
            oss << "  Types inferred: " << totalInferred << "\n";
            oss << "  Errors: " << errors.size() << "\n";
            oss << "  Warnings: " << warnings.size() << "\n";
            return oss.str();
        }

        inline std::string TypeCheckResult::getArabicSummary() const
        {
            std::ostringstream oss;
            oss << "ملخص فحص الأنواع:\n";
            oss << "  الحالة: " << (success ? "نجح ✓" : "فشل ✗") << "\n";
            oss << "  المتغيرات المفحوصة: " << totalVariables << "\n";
            oss << "  الدوال المفحوصة: " << totalFunctions << "\n";
            oss << "  الأنواع المُستنتَجة: " << totalInferred << "\n";
            oss << "  الأخطاء: " << errors.size() << "\n";
            oss << "  التحذيرات: " << warnings.size() << "\n";
            return oss.str();
        }

    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_TYPE_CHECKER_H
