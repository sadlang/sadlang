/**
 * @file wasm_ast_visitor.h
 * @brief (AR) زائر AST لتوليد WebAssembly — يحول AST إلى كود WASM
 * @brief (EN) AST Visitor for WebAssembly generation — converts AST to WASM code
 *
 * يربط بين:
 *   - shared/ast/ — شجرة AST
 *   - compiler/src/targets/wasm.cpp — مولد WAT
 *   - compiler/src/backend/wasm_codegen.cpp — مولد LLVM IR لـ WASM
 *
 * @author فريق لغة ص
 * @date 2026-03-06
 */

#ifndef SAD_WASM_AST_VISITOR_H
#define SAD_WASM_AST_VISITOR_H

#include "shared/ast/include/ast_visitor.h"
#include "shared/ast/include/ast_node.h"
#include "shared/ast/include/expressions.h"
#include "shared/ast/include/statements.h"
#include "shared/ast/include/declarations.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <memory>
#include <sstream>

namespace sad
{
    namespace wasm
    {

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نوع قيمة WASM
        // ═══════════════════════════════════════════════════════════════════════════════

        enum class WasmValType
        {
            I32, // عدد صحيح 32-bit
            I64, // عدد صحيح 64-bit
            F32, // عشري 32-bit
            F64, // عشري 64-bit
            Void // لا شيء
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        //  معلومات متغير محلي
        // ═══════════════════════════════════════════════════════════════════════════════

        struct WasmLocal
        {
            std::string name;     // الاسم الأصلي
            uint32_t index;       // فهرس المتغير في WASM
            WasmValType type;     // النوع
            bool isParam = false; // هل هو معامل دالة
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        //  معلومات دالة
        // ═══════════════════════════════════════════════════════════════════════════════

        struct WasmFunc
        {
            std::string name;                 // اسم الدالة
            std::string arabicName;           // الاسم العربي
            std::vector<WasmValType> params;  // أنواع المعاملات
            std::vector<WasmValType> results; // أنواع النتائج
            std::vector<WasmLocal> locals;    // المتغيرات المحلية
            std::vector<std::string> body;    // تعليمات WAT
            bool exported = false;            // هل تُصدَّر
            uint32_t index = 0;               // فهرس الدالة
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        //  وحدة WASM
        // ═══════════════════════════════════════════════════════════════════════════════

        struct WasmModule
        {
            std::string name;                      // اسم الوحدة
            std::vector<WasmFunc> functions;       // الدوال
            std::vector<std::string> imports;      // الاستيرادات
            std::vector<std::string> dataSegments; // قطع البيانات (النصوص)
            uint32_t nextFuncIndex = 0;            // فهرس الدالة التالية
            uint32_t memoryPages = 1;              // صفحات الذاكرة
            uint32_t dataOffset = 0;               // إزاحة البيانات الحالية

            // خريطة أسماء الدوال → فهارسها
            std::unordered_map<std::string, uint32_t> funcIndices;

            // خريطة السلاسل النصية → إزاحاتها
            std::unordered_map<std::string, uint32_t> stringOffsets;
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        //  زائر WASM — يحول AST إلى WAT
        // ═══════════════════════════════════════════════════════════════════════════════

        class WasmASTVisitor : public Sad::AST::BaseASTVisitor
        {
        public:
            WasmASTVisitor();
            ~WasmASTVisitor() override = default;

            /**
             * تجميع برنامج كامل
             * @param program عقدة البرنامج الجذرية
             * @return WAT text
             */
            std::string compile(Sad::AST::ASTNode *program);

            /**
             * الحصول على كود WAT الناتج
             */
            std::string getWatCode() const;

            /**
             * الحصول على أخطاء التجميع
             */
            const std::vector<std::string> &getErrors() const { return errors_; }

            /**
             * هل نجح التجميع
             */
            bool hasErrors() const { return !errors_.empty(); }

            // ═══════════════════════════════════════════════════════════════════════════
            //  زوار التعابير
            // ═══════════════════════════════════════════════════════════════════════════

            void visitBinaryExpr(Sad::AST::BinaryExpr &expr) override;
            void visitUnaryExpr(Sad::AST::UnaryExpr &expr) override;
            void visitLiteralExpr(Sad::AST::LiteralExpr &expr) override;
            void visitVariableExpr(Sad::AST::VariableExpr &expr) override;
            void visitAssignExpr(Sad::AST::AssignExpr &expr) override;
            void visitCallExpr(Sad::AST::CallExpr &expr) override;
            void visitTernaryExpr(Sad::AST::TernaryExpr &expr) override;
            void visitArrayExpr(Sad::AST::ArrayExpr &expr) override;
            void visitIndexExpr(Sad::AST::IndexExpr &expr) override;
            void visitOptionalChainExpr(Sad::AST::OptionalChainExpr &expr) override;
            void visitNullCoalesceExpr(Sad::AST::NullCoalesceExpr &expr) override;

            // ═══════════════════════════════════════════════════════════════════════════
            //  زوار العبارات
            // ═══════════════════════════════════════════════════════════════════════════

            void visitExprStmt(Sad::AST::ExprStmt &stmt) override;
            void visitVarDeclStmt(Sad::AST::VarDeclStmt &stmt) override;
            void visitIfStmt(Sad::AST::IfStmt &stmt) override;
            void visitWhileStmt(Sad::AST::WhileStmt &stmt) override;
            void visitForRangeStmt(Sad::AST::ForRangeStmt &stmt) override;
            void visitReturnStmt(Sad::AST::ReturnStmt &stmt) override;
            void visitBreakStmt(Sad::AST::BreakStmt &stmt) override;
            void visitContinueStmt(Sad::AST::ContinueStmt &stmt) override;
            void visitBlockStmt(Sad::AST::BlockStmt &stmt) override;

            // (AR) جمل التزامن — غير مدعومة في WASM / (EN) Concurrency — not supported in WASM
            void visitGoStmt(Sad::AST::GoStmt &) override
            {
                errors_.push_back("خطأ: جملة 'أطلق' (goroutines) غير مدعومة في هدف WASM");
            }
            void visitSelectCase(Sad::AST::SelectCase &) override
            {
                errors_.push_back("خطأ: جملة 'اختر' (select) غير مدعومة في هدف WASM");
            }
            void visitSelectStmt(Sad::AST::SelectStmt &) override
            {
                errors_.push_back("خطأ: جملة 'اختر' (select) غير مدعومة في هدف WASM");
            }

            // ═══════════════════════════════════════════════════════════════════════════
            //  زوار التصريحات
            // ═══════════════════════════════════════════════════════════════════════════

            void visitFunctionDecl(Sad::AST::FunctionDecl &decl) override;
            void visitImportStmt(Sad::AST::ImportStmt &stmt) override;

        private:
            WasmModule module_;               // الوحدة الناتجة
            WasmFunc *currentFunc_ = nullptr; // الدالة الحالية
            std::vector<std::string> errors_; // أخطاء التجميع

            // خريطة المتغيرات المحلية في الدالة الحالية
            std::unordered_map<std::string, WasmLocal> currentLocals_;
            uint32_t nextLocalIndex_ = 0;

            // مكدس الحلقات (للـ break/continue)
            std::stack<std::pair<uint32_t, uint32_t>> loopStack_; // (break_depth, continue_depth)
            uint32_t blockDepth_ = 0;

            // ═══════════════════════════════════════════════════════════════════════════
            //  دوال مساعدة
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * إصدار تعليمة WAT
             */
            void emit(const std::string &instruction);

            /**
             * إضافة متغير محلي
             */
            uint32_t addLocal(const std::string &name, WasmValType type);

            /**
             * البحث عن متغير محلي
             */
            WasmLocal *findLocal(const std::string &name);

            /**
             * البحث عن دالة
             */
            uint32_t findFunction(const std::string &name);

            /**
             * إضافة نص إلى قطعة البيانات
             * @return إزاحة النص في الذاكرة
             */
            uint32_t addString(const std::string &str);

            /**
             * تحويل نوع ص إلى نوع WASM
             */
            WasmValType sadTypeToWasm(const std::string &type);

            /**
             * تحويل نوع WASM إلى نص WAT
             */
            std::string wasmTypeToWat(WasmValType type);

            /**
             * تسجيل دوال الاستيراد القياسية
             */
            void registerStandardImports();

            /**
             * توليد WAT النهائي
             */
            std::string generateWat();

            /**
             * إضافة خطأ
             */
            void addError(const std::string &msg);
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        //  دوال المكتبة القياسية المستوردة
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * قائمة الدوال المستوردة من JavaScript
         */
        struct WasmStdlibImport
        {
            const char *module;     // وحدة الاستيراد (env)
            const char *wasmName;   // الاسم في WASM
            const char *arabicName; // الاسم العربي
            const char *signature;  // التوقيع في WAT
        };

        /**
         * الدوال القياسية المستوردة
         */
        extern const WasmStdlibImport WASM_STDLIB_IMPORTS[];
        extern const size_t WASM_STDLIB_IMPORTS_COUNT;

    } // namespace wasm
} // namespace sad

#endif // SAD_WASM_AST_VISITOR_H
