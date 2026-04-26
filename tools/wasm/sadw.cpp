/**
 * =============================================================================
 * @file sadw.cpp
 * @brief (AR) أداة ترجمة لغة ص إلى WebAssembly — .ص → .wasm
 * @brief (EN) Sad to WebAssembly Compiler Tool — .ص → .wasm
 * 
 * المهمة: T242 - WASM CLI Tool
 * المرحلة: سبتمبر 2026 — WASM Direct Compilation
 * =============================================================================
 * 
 * 🎯 الاستخدام
 * ═══════════
 * sadw برنامج.ص                    # → برنامج.wasm + برنامج.js + برنامج.html
 * sadw برنامج.ص -o output.wasm    # تحديد اسم الملف
 * sadw برنامج.ص --wat             # توليد WAT (نص) أيضاً
 * sadw برنامج.ص --optimize        # تحسين الكود
 * 
 * =============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

// Sad core
#include "shared/lexer/include/lexer_core.h"
#include "shared/parser/include/parser_core.h"
#include "shared/ast/include/ast_nodes.h"

// WASM generator
#include "compiler/src/targets/wasm_direct.h"
#include "compiler/src/targets/sir_to_wasm.h"

namespace fs = std::filesystem;
using namespace sad::wasm;

// =============================================================================
// ألوان الطرفية
// =============================================================================

#ifdef _WIN32
#include <windows.h>
void enableAnsiColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    SetConsoleOutputCP(CP_UTF8);
}
#else
void enableAnsiColors() {}
#endif

const char* RESET = "\033[0m";
const char* BOLD = "\033[1m";
const char* RED = "\033[31m";
const char* GREEN = "\033[32m";
const char* YELLOW = "\033[33m";
const char* BLUE = "\033[34m";
const char* CYAN = "\033[36m";

// =============================================================================
// خيارات سطر الأوامر
// =============================================================================

struct Options {
    std::string inputFile;
    std::string outputFile;
    bool generateWat = false;
    bool generateHtml = true;
    bool generateJs = true;
    bool optimize = false;
    bool verbose = false;
    bool help = false;
};

void printUsage() {
    std::cout << BOLD << "🔮 sadw — مُترجم لغة ص إلى WebAssembly" << RESET << "\n\n";
    
    std::cout << CYAN << "الاستخدام:" << RESET << "\n";
    std::cout << "  sadw <ملف.ص> [خيارات]\n\n";
    
    std::cout << CYAN << "الخيارات:" << RESET << "\n";
    std::cout << "  -o, --output <ملف>    تحديد اسم الملف الناتج\n";
    std::cout << "  --wat                 توليد ملف WAT (نص قابل للقراءة)\n";
    std::cout << "  --no-html             عدم توليد ملف HTML\n";
    std::cout << "  --no-js               عدم توليد ملف JavaScript\n";
    std::cout << "  -O, --optimize        تحسين الكود\n";
    std::cout << "  -v, --verbose         عرض تفاصيل الترجمة\n";
    std::cout << "  -h, --help            عرض هذه الرسالة\n\n";
    
    std::cout << CYAN << "أمثلة:" << RESET << "\n";
    std::cout << "  sadw مرحبا.ص                    # ترجمة للـ WASM\n";
    std::cout << "  sadw برنامج.ص -o app.wasm      # تحديد الاسم\n";
    std::cout << "  sadw برنامج.ص --wat -O         # مع تحسين و WAT\n\n";
}

Options parseArgs(int argc, char* argv[]) {
    Options opts;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            opts.help = true;
        } else if (arg == "-v" || arg == "--verbose") {
            opts.verbose = true;
        } else if (arg == "-O" || arg == "--optimize") {
            opts.optimize = true;
        } else if (arg == "--wat") {
            opts.generateWat = true;
        } else if (arg == "--no-html") {
            opts.generateHtml = false;
        } else if (arg == "--no-js") {
            opts.generateJs = false;
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            opts.outputFile = argv[++i];
        } else if (arg[0] != '-') {
            opts.inputFile = arg;
        }
    }
    
    return opts;
}

// =============================================================================
// قراءة الملف
// =============================================================================

std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("لا يمكن فتح الملف: " + path);
    }
    
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// =============================================================================
// زائر AST للترجمة
// =============================================================================

class WasmCompilerVisitor {
public:
    WasmCompilerVisitor() : compiler_() {}
    
    /**
     * @brief ترجمة برنامج كامل
     */
    bool compile(const std::string& source, bool verbose = false) {
        verbose_ = verbose;
        
        try {
            // 1. التحليل المعجمي
            if (verbose_) {
                std::cout << BLUE << "  📝 التحليل المعجمي..." << RESET << "\n";
            }
            
            Sad::Lexer::LexerCore lexer(source);
            auto tokens = lexer.tokenize();
            
            if (verbose_) {
                std::cout << "     " << tokens.size() << " رمز\n";
            }
            
            // 2. التحليل النحوي
            if (verbose_) {
                std::cout << BLUE << "  🌳 التحليل النحوي..." << RESET << "\n";
            }
            
            Sad::Parser::ParserCore parser(tokens);
            auto ast = parser.parse();
            
            if (!ast) {
                throw std::runtime_error("فشل التحليل النحوي");
            }
            
            // 3. توليد WASM
            if (verbose_) {
                std::cout << BLUE << "  ⚙️ توليد WASM..." << RESET << "\n";
            }
            
            generateWasm(ast.get());
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << RED << "❌ خطأ: " << e.what() << RESET << "\n";
            return false;
        }
    }
    
    /**
     * @brief حفظ الملفات
     */
    bool save(const std::string& basePath, bool html = true, bool js = true, bool wat = false) {
        // .wasm
        if (!compiler_.saveWasm(basePath + ".wasm")) {
            return false;
        }
        
        // .js
        if (js) {
            std::ofstream jsFile(basePath + ".js");
            if (jsFile) {
                jsFile << compiler_.generateJsGlue();
            }
        }
        
        // .html
        if (html) {
            std::ofstream htmlFile(basePath + ".html");
            if (htmlFile) {
                std::string wasmName = fs::path(basePath).filename().string() + ".wasm";
                htmlFile << compiler_.generateHtml(wasmName);
            }
        }
        
        // .wat (WebAssembly Text Format)
        if (wat) {
            std::string watPath = basePath + ".wat";
            auto binary = compiler_.generate();
            std::ofstream watFile(watPath);
            if (watFile) {
                watFile << "(module\n";
                watFile << "  ;; Generated by sadw — لغة ص WebAssembly Compiler\n";
                watFile << "  ;; Size: " << binary.size() << " bytes\n";
                watFile << "  (memory (export \"memory\") 1)\n";
                
                // (AR) تصدير دالة _start
                // (EN) Export _start function  
                watFile << "  (func $main (export \"_start\")\n";
                watFile << "    ;; WAT disassembly placeholder — use wasm2wat for full output\n";
                watFile << "    nop\n";
                watFile << "  )\n";
                watFile << ")\n";
                watFile << ";; NOTE: For full WAT output, use: wasm2wat " << basePath << ".wasm\n";
            }
        }
        
        return true;
    }
    
    std::vector<uint8_t> getBinary() {
        return compiler_.generate();
    }

private:
    SIRToWasm compiler_;
    bool verbose_ = false;
    
    // جدول الدوال (اسم → فهرس)
    std::map<std::string, uint32_t> functionTable_;
    uint32_t nextFuncIndex_ = 7;  // بعد الدوال المستوردة
    
    // جدول المتغيرات المحلية (اسم → فهرس)
    std::map<std::string, uint32_t> localTable_;
    
    /**
     * @brief توليد WASM من AST
     */
    void generateWasm(Sad::AST::ASTNode* ast) {
        // بدء دالة main
        compiler_.beginFunction("main", {}, {ValType::I32}, true);
        
        // ترجمة العقد
        if (auto* program = dynamic_cast<Sad::AST::ProgramNode*>(ast)) {
            for (const auto& stmt : program->statements) {
                compileStatement(stmt.get());
            }
        }
        
        // إرجاع 0
        compiler_.emitLoadConst(0);
        compiler_.emitReturn();
        compiler_.endFunction();
    }
    
    /**
     * @brief ترجمة جملة
     */
    void compileStatement(Sad::AST::ASTNode* node) {
        if (!node) return;
        
        // Variable Declaration
        if (auto* varDecl = dynamic_cast<Sad::AST::VariableDeclarationNode*>(node)) {
            compileVarDecl(varDecl);
        }
        // Expression Statement
        else if (auto* exprStmt = dynamic_cast<Sad::AST::ExpressionStatement*>(node)) {
            compileExpression(exprStmt->expression.get());
            compiler_.getGenerator()->emit(Op::Drop);  // drop result
        }
        // Function Call (print)
        else if (auto* call = dynamic_cast<Sad::AST::FunctionCallNode*>(node)) {
            compileFunctionCall(call);
            // check if void
        }
        // If Statement
        else if (auto* ifStmt = dynamic_cast<Sad::AST::IfStatementNode*>(node)) {
            compileIfStatement(ifStmt);
        }
        // While Statement
        else if (auto* whileStmt = dynamic_cast<Sad::AST::WhileStatementNode*>(node)) {
            compileWhileStatement(whileStmt);
        }
        // Return Statement
        else if (auto* retStmt = dynamic_cast<Sad::AST::ReturnStatementNode*>(node)) {
            if (retStmt->returnValue) {
                compileExpression(retStmt->returnValue.get());
            } else {
                compiler_.emitLoadConst(0);
            }
            compiler_.emitReturn();
        }
    }
    
    /**
     * @brief ترجمة تعريف متغير
     */
    void compileVarDecl(Sad::AST::VariableDeclarationNode* node) {
        // إضافة متغير محلي
        uint32_t idx = compiler_.addLocal(ValType::I32);
        localTable_[node->name] = idx;
        
        // تعيين القيمة
        if (node->initialValue) {
            compileExpression(node->initialValue.get());
            compiler_.emitStoreLocal(idx);
        }
    }
    
    /**
     * @brief ترجمة تعبير
     */
    void compileExpression(Sad::AST::ASTNode* node) {
        if (!node) {
            compiler_.emitLoadConst(0);
            return;
        }
        
        // Number Literal
        if (auto* num = dynamic_cast<Sad::AST::NumberLiteralNode*>(node)) {
            compiler_.emitLoadConst(static_cast<int64_t>(num->value));
        }
        // Float Literal
        else if (auto* flt = dynamic_cast<Sad::AST::FloatLiteralNode*>(node)) {
            compiler_.emitLoadConstF64(flt->value);
        }
        // String Literal
        else if (auto* str = dynamic_cast<Sad::AST::StringLiteralNode*>(node)) {
            compiler_.emitLoadString(str->value);
        }
        // Boolean Literal
        else if (auto* b = dynamic_cast<Sad::AST::BooleanLiteralNode*>(node)) {
            compiler_.emitLoadConst(b->value ? 1 : 0);
        }
        // Variable
        else if (auto* var = dynamic_cast<Sad::AST::IdentifierNode*>(node)) {
            auto it = localTable_.find(var->name);
            if (it != localTable_.end()) {
                compiler_.emitLoadLocal(it->second);
            } else {
                // متغير غير معروف
                compiler_.emitLoadConst(0);
            }
        }
        // Binary Expression
        else if (auto* bin = dynamic_cast<Sad::AST::BinaryExpressionNode*>(node)) {
            compileBinaryExpr(bin);
        }
        // Function Call
        else if (auto* call = dynamic_cast<Sad::AST::FunctionCallNode*>(node)) {
            compileFunctionCall(call);
        }
        // Unary Expression
        else if (auto* unary = dynamic_cast<Sad::AST::UnaryExpressionNode*>(node)) {
            compileUnaryExpr(unary);
        }
    }
    
    /**
     * @brief ترجمة تعبير ثنائي
     */
    void compileBinaryExpr(Sad::AST::BinaryExpressionNode* node) {
        compileExpression(node->left.get());
        compileExpression(node->right.get());
        
        std::string op = node->op;
        
        // عمليات حسابية
        if (op == "+" || op == "جمع") {
            compiler_.emitAdd();
        } else if (op == "-" || op == "طرح") {
            compiler_.emitSub();
        } else if (op == "*" || op == "ضرب") {
            compiler_.emitMul();
        } else if (op == "/" || op == "قسمة") {
            compiler_.emitDiv();
        } else if (op == "%" || op == "باقي") {
            compiler_.emitMod();
        }
        // مقارنات
        else if (op == "==" || op == "يساوي") {
            compiler_.emitEq();
        } else if (op == "!=" || op == "لا_يساوي") {
            compiler_.emitNe();
        } else if (op == "<" || op == "أصغر") {
            compiler_.emitLt();
        } else if (op == ">" || op == "أكبر") {
            compiler_.emitGt();
        } else if (op == "<=" || op == "أصغر_أو_يساوي") {
            compiler_.emitLe();
        } else if (op == ">=" || op == "أكبر_أو_يساوي") {
            compiler_.emitGe();
        }
        // منطقية
        else if (op == "و" || op == "&&") {
            compiler_.emitAnd();
        } else if (op == "أو" || op == "||") {
            compiler_.emitOr();
        }
    }
    
    /**
     * @brief ترجمة تعبير أحادي
     */
    void compileUnaryExpr(Sad::AST::UnaryExpressionNode* node) {
        compileExpression(node->operand.get());
        
        if (node->op == "!" || node->op == "ليس") {
            compiler_.emitNot();
        } else if (node->op == "-") {
            // نفي: 0 - القيمة
            compiler_.emitLoadConst(0);
            compiler_.emitSub();
        }
    }
    
    /**
     * @brief ترجمة استدعاء دالة
     */
    void compileFunctionCall(Sad::AST::FunctionCallNode* node) {
        std::string name = node->functionName;
        
        // دوال الطباعة المدمجة
        if (name == "اطبع" || name == "اطبع_سطر" || name == "print" || name == "println") {
            if (!node->arguments.empty()) {
                compileExpression(node->arguments[0].get());
                
                // تحقق من نوع الوسيط
                auto* arg = node->arguments[0].get();
                if (dynamic_cast<Sad::AST::StringLiteralNode*>(arg)) {
                    // string: need ptr and len
                    // TODO: proper string handling
                    compiler_.emitCallPrint(true);
                } else {
                    compiler_.emitCallPrint(false);
                }
            }
        }
        // دوال رياضية
        else if (name == "جذر" || name == "sqrt") {
            if (!node->arguments.empty()) {
                compileExpression(node->arguments[0].get());
                compiler_.emitCall(3);  // math_sqrt
            }
        }
        else if (name == "جيب" || name == "sin") {
            if (!node->arguments.empty()) {
                compileExpression(node->arguments[0].get());
                compiler_.emitCall(4);  // math_sin
            }
        }
        else if (name == "جتا" || name == "cos") {
            if (!node->arguments.empty()) {
                compileExpression(node->arguments[0].get());
                compiler_.emitCall(5);  // math_cos
            }
        }
        else if (name == "عشوائي" || name == "random") {
            compiler_.emitCall(6);  // math_random
        }
        // دالة مستخدم
        else {
            auto it = functionTable_.find(name);
            if (it != functionTable_.end()) {
                // تمرير الوسائط
                for (const auto& arg : node->arguments) {
                    compileExpression(arg.get());
                }
                compiler_.emitCall(it->second);
            }
        }
    }
    
    /**
     * @brief ترجمة جملة if
     */
    void compileIfStatement(Sad::AST::IfStatementNode* node) {
        // الشرط
        compileExpression(node->condition.get());
        
        compiler_.emitIf();
        
        // الجسم الرئيسي
        if (node->thenBranch) {
            if (auto* block = dynamic_cast<Sad::AST::BlockNode*>(node->thenBranch.get())) {
                for (const auto& stmt : block->statements) {
                    compileStatement(stmt.get());
                }
            } else {
                compileStatement(node->thenBranch.get());
            }
        }
        
        // else
        if (node->elseBranch) {
            compiler_.emitElse();
            if (auto* block = dynamic_cast<Sad::AST::BlockNode*>(node->elseBranch.get())) {
                for (const auto& stmt : block->statements) {
                    compileStatement(stmt.get());
                }
            } else {
                compileStatement(node->elseBranch.get());
            }
        }
        
        compiler_.emitEnd();
    }
    
    /**
     * @brief ترجمة حلقة while
     */
    void compileWhileStatement(Sad::AST::WhileStatementNode* node) {
        // block للخروج
        compiler_.emitBlock();
        
        // loop للاستمرار
        compiler_.emitLoop();
        
        // الشرط (مقلوب للخروج)
        compileExpression(node->condition.get());
        compiler_.emitNot();
        compiler_.emitBrIf(1);  // break على false
        
        // الجسم
        if (node->body) {
            if (auto* block = dynamic_cast<Sad::AST::BlockNode*>(node->body.get())) {
                for (const auto& stmt : block->statements) {
                    compileStatement(stmt.get());
                }
            } else {
                compileStatement(node->body.get());
            }
        }
        
        // continue
        compiler_.emitBr(0);
        
        compiler_.emitEnd();  // loop end
        compiler_.emitEnd();  // block end
    }
};

// =============================================================================
// main
// =============================================================================

int main(int argc, char* argv[]) {
    enableAnsiColors();
    
    Options opts = parseArgs(argc, argv);
    
    if (opts.help || opts.inputFile.empty()) {
        printUsage();
        return opts.help ? 0 : 1;
    }
    
    // تحديد اسم الملف الناتج
    std::string basePath;
    if (!opts.outputFile.empty()) {
        basePath = opts.outputFile;
        // إزالة الامتداد إذا وجد
        auto dotPos = basePath.rfind('.');
        if (dotPos != std::string::npos) {
            basePath = basePath.substr(0, dotPos);
        }
    } else {
        basePath = fs::path(opts.inputFile).stem().string();
    }
    
    std::cout << BOLD << "🔮 sadw — مُترجم لغة ص إلى WebAssembly" << RESET << "\n\n";
    
    try {
        // قراءة الملف
        std::cout << CYAN << "📂 قراءة: " << RESET << opts.inputFile << "\n";
        std::string source = readFile(opts.inputFile);
        
        // الترجمة
        std::cout << CYAN << "⚙️ الترجمة:" << RESET << "\n";
        
        WasmCompilerVisitor compiler;
        if (!compiler.compile(source, opts.verbose)) {
            return 1;
        }
        
        // الحفظ
        std::cout << CYAN << "💾 الحفظ:" << RESET << "\n";
        
        if (!compiler.save(basePath, opts.generateHtml, opts.generateJs, opts.generateWat)) {
            std::cerr << RED << "❌ فشل حفظ الملفات" << RESET << "\n";
            return 1;
        }
        
        // عرض الملفات المُولدة
        std::cout << GREEN << "✅ تم التوليد بنجاح!" << RESET << "\n\n";
        std::cout << "الملفات المُولدة:\n";
        std::cout << "  📦 " << basePath << ".wasm\n";
        if (opts.generateJs) {
            std::cout << "  📜 " << basePath << ".js\n";
        }
        if (opts.generateHtml) {
            std::cout << "  🌐 " << basePath << ".html\n";
        }
        if (opts.generateWat) {
            std::cout << "  📄 " << basePath << ".wat\n";
        }
        
        std::cout << "\n" << YELLOW << "💡 للتشغيل:" << RESET << "\n";
        std::cout << "  افتح " << basePath << ".html في المتصفح\n";
        std::cout << "  أو: python -m http.server ثم افتح http://localhost:8000/" << basePath << ".html\n";
        
    } catch (const std::exception& e) {
        std::cerr << RED << "❌ خطأ: " << e.what() << RESET << "\n";
        return 1;
    }
    
    return 0;
}
