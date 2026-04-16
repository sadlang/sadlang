/**
 * @file wasm_ast_compiler.h
 * @brief (AR) مترجم AST إلى WASM ثنائي مباشر — بدون LLVM أو Emscripten
 * @brief (EN) AST to direct WASM binary compiler — no LLVM or Emscripten
 *
 * يحوّل شجرة AST من لغة ص إلى ملف .wasm ثنائي مباشرة
 * يستخدم WasmEmitter لتوليد الثنائي النهائي
 */

#pragma once

#include "wasm_emitter.h"
#include "shared/ast/include/ast_visitor.h"
#include "shared/ast/include/ast_node.h"
#include "shared/ast/include/expressions.h"
#include "shared/ast/include/statements.h"
#include "shared/ast/include/declarations.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stack>

namespace Sad
{
    namespace Compiler
    {
        namespace WasmDirect
        {

            /**
             * خيارات ترجمة WASM المباشرة
             */
            struct WasmCompileOptions
            {
                uint32_t initialMemoryPages = 16; // 1MB
                uint32_t maxMemoryPages = 256;    // 16MB
                uint32_t stackSize = 65536;       // 64KB
                bool exportAllFunctions = false;  // تصدير جميع الدوال
                bool generateSourceMap = false;   // خريطة المصدر
                bool enableSIMD = false;          // SIMD 128
                bool debugMode = false;           // وضع التصحيح
                std::string moduleName = "sad_module";
            };

            /**
             * نتيجة الترجمة
             */
            struct WasmCompileResult
            {
                bool success = false;
                std::vector<uint8_t> wasmBinary;   // الثنائي الناتج
                std::string jsRuntime;             // كود JS المساعد
                std::string htmlLoader;            // صفحة HTML للاختبار
                std::vector<std::string> errors;   // أخطاء الترجمة
                std::vector<std::string> warnings; // تحذيرات
                size_t codeSize = 0;               // حجم الكود
                uint32_t functionCount = 0;        // عدد الدوال
                uint32_t importCount = 0;          // عدد الاستيرادات
            };

            /**
             * معلومات متغير في النطاق
             */
            struct VarInfo
            {
                uint32_t localIndex; // فهرس المتغير المحلي
                ValType type;        // نوع القيمة
                bool isParam;        // هل هو معامل
            };

            /**
             * معلومات دالة مُسجّلة
             */
            struct FuncInfo
            {
                uint32_t wasmIndex; // فهرس الدالة في WASM
                WasmFuncType type;  // نوع الدالة
                bool isImport;      // هل هي مستوردة
            };

            /**
             * مترجم AST → WASM ثنائي مباشر
             */
            class WasmASTCompiler : public AST::BaseASTVisitor
            {
            public:
                explicit WasmASTCompiler(const WasmCompileOptions &options = {});
                ~WasmASTCompiler() override = default;

                /**
                 * ترجمة برنامج كامل
                 * @param statements عبارات البرنامج
                 * @return نتيجة الترجمة
                 */
                WasmCompileResult compile(const std::vector<std::unique_ptr<AST::Statement>> &statements);

                /**
                 * ترجمة ملف .ص إلى .wasm
                 * @param inputPath مسار الملف المصدري
                 * @param outputPath مسار ملف الخرج
                 * @return true إذا نجحت الترجمة
                 */
                bool compileFile(const std::string &inputPath, const std::string &outputPath);

            protected:
                // ═══ الزيارات: التعبيرات ═══
                void visitBinaryExpr(AST::BinaryExpr &node) override;
                void visitUnaryExpr(AST::UnaryExpr &node) override;
                void visitLiteralExpr(AST::LiteralExpr &node) override;
                void visitVariableExpr(AST::VariableExpr &node) override;
                void visitAssignExpr(AST::AssignExpr &node) override;
                void visitCallExpr(AST::CallExpr &node) override;
                void visitTernaryExpr(AST::TernaryExpr &node) override;
                void visitArrayExpr(AST::ArrayExpr &node) override;
                void visitIndexExpr(AST::IndexExpr &node) override;
                void visitMemberAccessExpr(AST::MemberAccessExpr &node) override;
                void visitMethodCallExpr(AST::MethodCallExpr &node) override;
                void visitOptionalChainExpr(AST::OptionalChainExpr &node) override;
                void visitNullCoalesceExpr(AST::NullCoalesceExpr &node) override;

                // ═══ الزيارات: العبارات ═══
                void visitExprStmt(AST::ExprStmt &node) override;
                void visitVarDeclStmt(AST::VarDeclStmt &node) override;
                void visitIfStmt(AST::IfStmt &node) override;
                void visitWhileStmt(AST::WhileStmt &node) override;
                void visitForRangeStmt(AST::ForRangeStmt &node) override;
                void visitReturnStmt(AST::ReturnStmt &node) override;
                void visitBreakStmt(AST::BreakStmt &node) override;
                void visitContinueStmt(AST::ContinueStmt &node) override;
                void visitBlockStmt(AST::BlockStmt &node) override;

                // (AR) جمل التزامن — غير مدعومة في WASM / (EN) Concurrency — not supported in WASM
                void visitGoStmt(AST::GoStmt &) override
                {
                    errors_.push_back("خطأ: جملة 'أطلق' (goroutines) غير مدعومة في هدف WASM");
                }
                void visitSelectCase(AST::SelectCase &) override
                {
                    errors_.push_back("خطأ: جملة 'اختر' (select) غير مدعومة في هدف WASM");
                }
                void visitSelectStmt(AST::SelectStmt &) override
                {
                    errors_.push_back("خطأ: جملة 'اختر' (select) غير مدعومة في هدف WASM");
                }

                // ═══ الزيارات: التصريحات ═══
                void visitFunctionDecl(AST::FunctionDecl &node) override;
                void visitImportStmt(AST::ImportStmt &node) override;

            private:
                WasmCompileOptions options_;
                std::unique_ptr<WasmEmitter> emitter_;
                WasmCodeBuilder *currentCode_ = nullptr;

                // إدارة الدوال
                std::unordered_map<std::string, FuncInfo> functions_;
                std::string currentFunctionName_;

                // إدارة المتغيرات (مكدس النطاقات)
                std::vector<std::unordered_map<std::string, VarInfo>> scopes_;
                uint32_t nextLocalIndex_ = 0;

                // عمق الحلقات (لـ break/continue)
                uint32_t loopDepth_ = 0;

                // إدارة النصوص الثابتة
                std::unordered_map<std::string, uint32_t> stringPool_;
                uint32_t nextStringOffset_ = 1024; // بعد الثوابت

                // أخطاء وتحذيرات
                std::vector<std::string> errors_;
                std::vector<std::string> warnings_;

                // ─── مساعدات ───
                void setupStandardImports();
                void setupGlobals();
                uint32_t addString(const std::string &str);
                ValType sadTypeToWasm(AST::DataType type);

                void pushScope();
                void popScope();
                uint32_t declareLocal(const std::string &name, ValType type, bool isParam = false);
                VarInfo *lookupVar(const std::string &name);

                void error(const std::string &msg);
                void warning(const std::string &msg);

                // توليد ملفات مساعدة
                std::string generateJSRuntime() const;
                std::string generateHTMLLoader(const std::string &wasmFileName) const;
            };

        } // namespace WasmDirect
    } // namespace Compiler
} // namespace Sad
