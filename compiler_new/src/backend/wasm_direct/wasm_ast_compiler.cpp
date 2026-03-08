/**
 * @file wasm_ast_compiler.cpp
 * @brief (AR) تطبيق مترجم AST إلى WASM ثنائي مباشر
 * @brief (EN) AST to direct WASM binary compiler implementation
 *
 * يحوّل شجرة AST من لغة ص إلى ملف .wasm ثنائي
 * بدون أي اعتماد على LLVM أو Emscripten
 */

#include "wasm_ast_compiler.h"
#include "shared/lexer/include/token.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace Sad {
namespace Compiler {
namespace WasmDirect {

using TT = Lexer::TokenType;

// ════════════════════════════════════════════════════════════════════════════════
//  إنشاء وتهيئة
// ════════════════════════════════════════════════════════════════════════════════

WasmASTCompiler::WasmASTCompiler(const WasmCompileOptions& options)
    : options_(options) {}

// ════════════════════════════════════════════════════════════════════════════════
//  ترجمة البرنامج
// ════════════════════════════════════════════════════════════════════════════════

WasmCompileResult WasmASTCompiler::compile(
    const std::vector<std::unique_ptr<AST::Statement>>& statements) {
    
    WasmCompileResult result;
    emitter_ = std::make_unique<WasmEmitter>();
    errors_.clear();
    warnings_.clear();
    functions_.clear();
    stringPool_.clear();
    nextStringOffset_ = 1024;

    // تهيئة الذاكرة
    emitter_->setMemory(options_.initialMemoryPages, options_.maxMemoryPages);
    emitter_->exportMemory("memory");

    // تسجيل الدوال المستوردة القياسية
    setupStandardImports();

    // تسجيل المتغيرات العامة
    setupGlobals();

    // ═══ المرحلة 1: تسجيل جميع الدوال (forward declaration) ═══
    for (const auto& stmt : statements) {
        if (auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(stmt.get())) {
            WasmFuncType funcType;
            for (const auto& param : funcDecl->getParameters()) {
                funcType.params.push_back(sadTypeToWasm(param.type));
            }
            if (funcDecl->getReturnType() != AST::DataType::VOID) {
                funcType.results.push_back(sadTypeToWasm(funcDecl->getReturnType()));
            }
            
            uint32_t funcIdx = emitter_->addFunction(funcType);
            functions_[funcDecl->getName()] = {funcIdx, funcType, false};

            // تصدير الدالة الرئيسية والدوال العامة
            if (funcDecl->getName() == "رئيسية" || 
                funcDecl->getName() == "main" ||
                options_.exportAllFunctions) {
                emitter_->exportFunction(funcDecl->getName(), funcIdx);
            }
        }
    }

    // ═══ المرحلة 2: إنشاء _start (نقطة الدخول) ═══
    // تجمع الكود العام (خارج الدوال) في دالة _start
    WasmFuncType startType;
    // _start لا تأخذ معاملات ولا ترجع شيئاً
    uint32_t startFuncIdx = emitter_->addFunction(startType);
    emitter_->exportFunction("_start", startFuncIdx);
    functions_["_start"] = {startFuncIdx, startType, false};

    // ═══ المرحلة 3: ترجمة أجسام الدوال ═══
    for (const auto& stmt : statements) {
        if (auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(stmt.get())) {
            auto it = functions_.find(funcDecl->getName());
            if (it == functions_.end()) continue;

            currentFunctionName_ = funcDecl->getName();
            WasmCodeBuilder code;
            currentCode_ = &code;
            nextLocalIndex_ = 0;
            scopes_.clear();
            pushScope();

            // تسجيل المعاملات
            for (const auto& param : funcDecl->getParameters()) {
                ValType wt = sadTypeToWasm(param.type);
                declareLocal(param.name, wt, true);
            }

            // ترجمة الجسم
            if (funcDecl->getBody()) {
                funcDecl->getBody()->accept(*this);
            }

            popScope();

            // إذا لم يكن هناك return صريح والدالة ترجع قيمة، أضف قيمة افتراضية
            if (!it->second.type.results.empty()) {
                auto rt = it->second.type.results[0];
                if (rt == ValType::I32) code.i32Const(0);
                else if (rt == ValType::F64) code.f64Const(0.0);
            }

            emitter_->setFunctionBody(it->second.wasmIndex, code.buildBody());
            currentCode_ = nullptr;
        }
    }

    // ═══ المرحلة 4: ترجمة الكود العام (خارج الدوال) إلى _start ═══
    {
        currentFunctionName_ = "_start";
        WasmCodeBuilder code;
        currentCode_ = &code;
        nextLocalIndex_ = 0;
        scopes_.clear();
        pushScope();

        for (const auto& stmt : statements) {
            if (dynamic_cast<AST::FunctionDecl*>(stmt.get())) continue;
            if (dynamic_cast<AST::ImportStmt*>(stmt.get())) continue;
            stmt->accept(*this);
        }

        // استدعاء رئيسية() إذا وجدت
        auto mainIt = functions_.find("رئيسية");
        if (mainIt == functions_.end()) mainIt = functions_.find("main");
        if (mainIt != functions_.end()) {
            code.call(mainIt->second.wasmIndex);
            if (!mainIt->second.type.results.empty()) {
                code.drop();
            }
        }

        popScope();
        emitter_->setFunctionBody(startFuncIdx, code.buildBody());
        currentCode_ = nullptr;
    }

    // ═══ المرحلة 5: بناء الثنائي ═══
    result.wasmBinary = emitter_->emit();
    result.success = errors_.empty();
    result.errors = errors_;
    result.warnings = warnings_;
    result.codeSize = result.wasmBinary.size();
    result.functionCount = emitter_->getFunctionCount();
    result.importCount = emitter_->getImportCount();
    result.jsRuntime = generateJSRuntime();
    result.htmlLoader = generateHTMLLoader(options_.moduleName + ".wasm");

    return result;
}

bool WasmASTCompiler::compileFile(const std::string& inputPath, 
                                   const std::string& outputPath) {
    // قراءة الملف المصدري
    std::ifstream file(inputPath, std::ios::binary);
    if (!file) {
        errors_.push_back("فشل في فتح الملف: " + inputPath);
        return false;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    // تحليل معجمي ونحوي
    Lexer::LexerCore lexer(source);
    Parser::ParserCore parser(lexer);
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        errors_.push_back("أخطاء في التحليل النحوي");
        return false;
    }

    // ترجمة
    auto result = compile(program);
    if (!result.success) return false;

    // كتابة .wasm
    std::ofstream wasmFile(outputPath, std::ios::binary);
    if (!wasmFile) {
        errors_.push_back("فشل في كتابة الملف: " + outputPath);
        return false;
    }
    wasmFile.write(reinterpret_cast<const char*>(result.wasmBinary.data()),
                   result.wasmBinary.size());

    // كتابة ملف JS runtime
    std::string jsPath = outputPath;
    auto dotPos = jsPath.rfind('.');
    if (dotPos != std::string::npos) jsPath = jsPath.substr(0, dotPos);
    jsPath += "_runtime.js";
    std::ofstream jsFile(jsPath);
    if (jsFile) jsFile << result.jsRuntime;

    // كتابة ملف HTML
    std::string htmlPath = jsPath.substr(0, jsPath.rfind('_')) + ".html";
    std::ofstream htmlFile(htmlPath);
    if (htmlFile) htmlFile << result.htmlLoader;

    return true;
}

// ════════════════════════════════════════════════════════════════════════════════
//  التهيئة
// ════════════════════════════════════════════════════════════════════════════════

void WasmASTCompiler::setupStandardImports() {
    // دوال الطباعة
    WasmFuncType printType;
    printType.params = {ValType::I32}; // مؤشر النص
    auto printIdx = emitter_->addImport("env", "__sad_print", printType);
    functions_["اطبع"] = {printIdx, printType, true};
    functions_["print"] = {printIdx, printType, true};

    WasmFuncType printlnType;
    printlnType.params = {ValType::I32};
    auto printlnIdx = emitter_->addImport("env", "__sad_println", printlnType);
    functions_["اطبع_سطر"] = {printlnIdx, printlnType, true};
    functions_["println"] = {printlnIdx, printlnType, true};

    WasmFuncType printIntType;
    printIntType.params = {ValType::I32};
    auto printIntIdx = emitter_->addImport("env", "__sad_print_int", printIntType);
    functions_["__sad_print_int"] = {printIntIdx, printIntType, true};

    WasmFuncType printFloatType;
    printFloatType.params = {ValType::F64};
    auto printFloatIdx = emitter_->addImport("env", "__sad_print_float", printFloatType);
    functions_["__sad_print_float"] = {printFloatIdx, printFloatType, true};

    // دوال الرياضيات
    WasmFuncType sqrtType;
    sqrtType.params = {ValType::F64};
    sqrtType.results = {ValType::F64};
    auto sqrtIdx = emitter_->addImport("env", "__sad_sqrt", sqrtType);
    functions_["جذر"] = {sqrtIdx, sqrtType, true};
    functions_["sqrt"] = {sqrtIdx, sqrtType, true};

    WasmFuncType powType;
    powType.params = {ValType::F64, ValType::F64};
    powType.results = {ValType::F64};
    auto powIdx = emitter_->addImport("env", "__sad_pow", powType);
    functions_["أس"] = {powIdx, powType, true};
    functions_["pow"] = {powIdx, powType, true};

    WasmFuncType randomType;
    randomType.results = {ValType::F64};
    auto randIdx = emitter_->addImport("env", "__sad_random", randomType);
    functions_["عشوائي"] = {randIdx, randomType, true};

    // دوال DOM
    WasmFuncType domQueryType;
    domQueryType.params = {ValType::I32};
    domQueryType.results = {ValType::I32};
    auto domQueryIdx = emitter_->addImport("env", "__sad_dom_query", domQueryType);
    functions_["__sad_dom_query"] = {domQueryIdx, domQueryType, true};

    WasmFuncType domCreateType;
    domCreateType.params = {ValType::I32};
    domCreateType.results = {ValType::I32};
    auto domCreateIdx = emitter_->addImport("env", "__sad_dom_create", domCreateType);
    functions_["__sad_dom_create"] = {domCreateIdx, domCreateType, true};

    WasmFuncType domSetTextType;
    domSetTextType.params = {ValType::I32, ValType::I32};
    auto domSetTextIdx = emitter_->addImport("env", "__sad_dom_set_text", domSetTextType);
    functions_["__sad_dom_set_text"] = {domSetTextIdx, domSetTextType, true};

    WasmFuncType domAppendType;
    domAppendType.params = {ValType::I32, ValType::I32};
    auto domAppendIdx = emitter_->addImport("env", "__sad_dom_append", domAppendType);
    functions_["__sad_dom_append"] = {domAppendIdx, domAppendType, true};

    WasmFuncType domOnClickType;
    domOnClickType.params = {ValType::I32, ValType::I32};
    auto domOnClickIdx = emitter_->addImport("env", "__sad_dom_on_click", domOnClickType);
    functions_["__sad_dom_on_click"] = {domOnClickIdx, domOnClickType, true};

    // دوال الذاكرة
    WasmFuncType mallocType;
    mallocType.params = {ValType::I32};
    mallocType.results = {ValType::I32};
    auto mallocIdx = emitter_->addImport("env", "__sad_malloc", mallocType);
    functions_["__sad_malloc"] = {mallocIdx, mallocType, true};

    WasmFuncType freeType;
    freeType.params = {ValType::I32};
    auto freeIdx = emitter_->addImport("env", "__sad_free", freeType);
    functions_["__sad_free"] = {freeIdx, freeType, true};

    // دوال النصوص
    WasmFuncType strLenType;
    strLenType.params = {ValType::I32};
    strLenType.results = {ValType::I32};
    auto strLenIdx = emitter_->addImport("env", "__sad_str_len", strLenType);
    functions_["الطول"] = {strLenIdx, strLenType, true};
    functions_["طول"] = {strLenIdx, strLenType, true};

    WasmFuncType strConcatType;
    strConcatType.params = {ValType::I32, ValType::I32};
    strConcatType.results = {ValType::I32};
    auto strConcatIdx = emitter_->addImport("env", "__sad_str_concat", strConcatType);
    functions_["__sad_str_concat"] = {strConcatIdx, strConcatType, true};

    // دوال console
    WasmFuncType consoleType;
    consoleType.params = {ValType::I32};
    auto consoleIdx = emitter_->addImport("env", "__sad_console_log", consoleType);
    functions_["__sad_console_log"] = {consoleIdx, consoleType, true};
}

void WasmASTCompiler::setupGlobals() {
    // مؤشر الكومة
    emitter_->addGlobal(ValType::I32, true, 
                        static_cast<int32_t>(options_.initialMemoryPages * 65536 / 2));
    // مؤشر المكدس
    emitter_->addGlobal(ValType::I32, true,
                        static_cast<int32_t>(options_.initialMemoryPages * 65536));
}

uint32_t WasmASTCompiler::addString(const std::string& str) {
    auto it = stringPool_.find(str);
    if (it != stringPool_.end()) return it->second;

    uint32_t offset = nextStringOffset_;
    emitter_->addDataSegment(offset, str);
    nextStringOffset_ = offset + static_cast<uint32_t>(str.size()) + 1;
    // محاذاة على 4 بايت
    nextStringOffset_ = (nextStringOffset_ + 3) & ~3u;
    stringPool_[str] = offset;
    return offset;
}

ValType WasmASTCompiler::sadTypeToWasm(AST::DataType type) {
    switch (type) {
        case AST::DataType::INTEGER:  return ValType::I32;
        case AST::DataType::FLOAT:    return ValType::F64;
        case AST::DataType::BOOLEAN:  return ValType::I32;
        case AST::DataType::STRING:   return ValType::I32; // مؤشر
        default:                      return ValType::I32;
    }
}

void WasmASTCompiler::pushScope() {
    scopes_.emplace_back();
}

void WasmASTCompiler::popScope() {
    if (!scopes_.empty()) scopes_.pop_back();
}

uint32_t WasmASTCompiler::declareLocal(const std::string& name, ValType type, bool isParam) {
    uint32_t idx = nextLocalIndex_++;
    if (!isParam && currentCode_) {
        currentCode_->addLocal(type);
    }
    if (!scopes_.empty()) {
        scopes_.back()[name] = {idx, type, isParam};
    }
    return idx;
}

VarInfo* WasmASTCompiler::lookupVar(const std::string& name) {
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return &found->second;
    }
    return nullptr;
}

void WasmASTCompiler::error(const std::string& msg) {
    errors_.push_back(msg);
}

void WasmASTCompiler::warning(const std::string& msg) {
    warnings_.push_back(msg);
}

// ════════════════════════════════════════════════════════════════════════════════
//  زيارات التعبيرات
// ════════════════════════════════════════════════════════════════════════════════

void WasmASTCompiler::visitBinaryExpr(AST::BinaryExpr& node) {
    if (!currentCode_) return;

    // حالة خاصة: جمع نصوص
    // TODO: دعم ربط النصوص عبر __sad_str_concat

    // ترجمة الطرفين
    node.getLeft().accept(*this);
    node.getRight().accept(*this);

    // العملية
    switch (node.getOp()) {
        // حسابية
        case TT::PLUS:         currentCode_->i32Add(); break;
        case TT::MINUS:        currentCode_->i32Sub(); break;
        case TT::STAR:         currentCode_->i32Mul(); break;
        case TT::SLASH:        currentCode_->i32DivS(); break;
        case TT::PERCENT:      currentCode_->i32RemS(); break;

        // مقارنة
        case TT::EQUAL_EQUAL:  currentCode_->i32Eq(); break;
        case TT::BANG_EQUAL:
        case TT::NOT_EQUAL:    currentCode_->i32Ne(); break;
        case TT::LESS:         currentCode_->i32LtS(); break;
        case TT::GREATER:      currentCode_->i32GtS(); break;
        case TT::LESS_EQUAL:   currentCode_->i32LeS(); break;
        case TT::GREATER_EQUAL: currentCode_->i32GeS(); break;

        // منطقية
        case TT::AND:
        case TT::KEYWORD_AND:  currentCode_->i32And(); break;
        case TT::OR:
        case TT::KEYWORD_OR:   currentCode_->i32Or(); break;

        // بتية
        case TT::AMPERSAND:    currentCode_->i32And(); break;
        case TT::PIPE:         currentCode_->i32Or(); break;
        case TT::CARET:        currentCode_->i32Xor(); break;
        case TT::SHIFT_LEFT:   currentCode_->i32Shl(); break;
        case TT::SHIFT_RIGHT:  currentCode_->i32ShrS(); break;

        default:
            warning("عملية ثنائية غير مدعومة في WASM");
            break;
    }
}

void WasmASTCompiler::visitUnaryExpr(AST::UnaryExpr& node) {
    if (!currentCode_) return;

    node.getOperand().accept(*this);

    switch (node.getOp()) {
        case TT::MINUS:
            // -x = 0 - x
            currentCode_->i32Const(0);
            // نحتاج تبديل الترتيب: نضع 0 أولاً ثم x
            // لكن x على المكدس بالفعل، لذا نستخدم حيلة:
            // swap: local.tee tmp, drop, i32.const 0, local.get tmp, i32.sub
            // بديل أبسط: i32.const -1, i32.mul
            currentCode_->i32Const(-1);
            currentCode_->i32Mul();
            break;
        case TT::BANG:
        case TT::KEYWORD_NOT:
            currentCode_->i32Eqz();
            break;
        default:
            warning("عملية أحادية غير مدعومة في WASM");
            break;
    }
}

void WasmASTCompiler::visitLiteralExpr(AST::LiteralExpr& node) {
    if (!currentCode_) return;

    const auto& token = node.getToken();
    switch (token.getType()) {
        case TT::INTEGER:
        case TT::NUMBER: {
            int32_t val = 0;
            try { val = std::stoi(token.getValue()); }
            catch (...) {}
            currentCode_->i32Const(val);
            break;
        }
        case TT::FLOAT: {
            double val = 0.0;
            try { val = std::stod(token.getValue()); }
            catch (...) {}
            currentCode_->f64Const(val);
            break;
        }
        case TT::STRING: {
            uint32_t offset = addString(token.getValue());
            currentCode_->i32Const(static_cast<int32_t>(offset));
            break;
        }
        case TT::KEYWORD_TRUE:
            currentCode_->i32Const(1);
            break;
        case TT::KEYWORD_FALSE:
            currentCode_->i32Const(0);
            break;
        case TT::KEYWORD_NULL:
            currentCode_->i32Const(0);
            break;
        default:
            currentCode_->i32Const(0);
            break;
    }
}

void WasmASTCompiler::visitVariableExpr(AST::VariableExpr& node) {
    if (!currentCode_) return;

    auto* var = lookupVar(node.getName());
    if (var) {
        currentCode_->localGet(var->localIndex);
    } else {
        warning("متغير غير معرّف: " + node.getName());
        currentCode_->i32Const(0);
    }
}

void WasmASTCompiler::visitAssignExpr(AST::AssignExpr& node) {
    if (!currentCode_) return;

    // ترجمة القيمة
    node.getValue().accept(*this);

    auto* var = lookupVar(node.getName());
    if (var) {
        currentCode_->localTee(var->localIndex);
    } else {
        warning("محاولة إسناد لمتغير غير معرّف: " + node.getName());
    }
}

void WasmASTCompiler::visitCallExpr(AST::CallExpr& node) {
    if (!currentCode_) return;

    // الحصول على اسم الدالة
    std::string funcName;
    if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(&node.getCallee())) {
        funcName = varExpr->getName();
    }

    if (funcName.empty()) {
        warning("استدعاء دالة غير مباشر غير مدعوم بعد في WASM المباشر");
        currentCode_->i32Const(0);
        return;
    }

    // حالة خاصة: اطبع/اطبع_سطر مع نص أو رقم
    bool isPrint = (funcName == "اطبع" || funcName == "print" ||
                    funcName == "اطبع_سطر" || funcName == "println");

    if (isPrint && !node.getArguments().empty()) {
        auto& arg = *node.getArguments()[0];
        
        // إذا كان الوسيط نصاً حرفياً
        if (auto* lit = dynamic_cast<AST::LiteralExpr*>(&arg)) {
            if (lit->getToken().getType() == TT::STRING) {
                uint32_t offset = addString(lit->getToken().getValue());
                currentCode_->i32Const(static_cast<int32_t>(offset));
                auto it = functions_.find(funcName);
                if (it != functions_.end()) {
                    currentCode_->call(it->second.wasmIndex);
                }
                return;
            }
            if (lit->getToken().getType() == TT::INTEGER ||
                lit->getToken().getType() == TT::NUMBER) {
                arg.accept(*this);
                auto it = functions_.find("__sad_print_int");
                if (it != functions_.end()) {
                    currentCode_->call(it->second.wasmIndex);
                }
                return;
            }
            if (lit->getToken().getType() == TT::FLOAT) {
                arg.accept(*this);
                auto it = functions_.find("__sad_print_float");
                if (it != functions_.end()) {
                    currentCode_->call(it->second.wasmIndex);
                }
                return;
            }
        }
    }

    // البحث عن الدالة
    auto it = functions_.find(funcName);
    if (it == functions_.end()) {
        warning("دالة غير معرّفة: " + funcName);
        currentCode_->i32Const(0);
        return;
    }

    // ترجمة الوسائط
    for (const auto& arg : node.getArguments()) {
        arg->accept(*this);
    }

    currentCode_->call(it->second.wasmIndex);
}

void WasmASTCompiler::visitTernaryExpr(AST::TernaryExpr& node) {
    if (!currentCode_) return;

    // condition ? trueExpr : falseExpr
    node.getCondition().accept(*this);
    currentCode_->ifBlock(static_cast<uint8_t>(ValType::I32));
    node.getTrueExpr().accept(*this);
    currentCode_->elseBlock();
    node.getFalseExpr().accept(*this);
    currentCode_->end();
}

void WasmASTCompiler::visitArrayExpr(AST::ArrayExpr& node) {
    if (!currentCode_) return;
    // المصفوفات: تخصيص ذاكرة وتخزين العناصر
    uint32_t count = static_cast<uint32_t>(node.getElements().size());
    uint32_t size = count * 4; // كل عنصر i32
    
    currentCode_->i32Const(static_cast<int32_t>(size + 4)); // +4 للطول
    auto mallocIt = functions_.find("__sad_malloc");
    if (mallocIt != functions_.end()) {
        currentCode_->call(mallocIt->second.wasmIndex);
    }

    // تخزين الطول في البداية
    // النتيجة (العنوان) على المكدس — نحتاج تخزينه في local
    // ثم نستخدمه لتخزين الطول والعناصر
    warning("المصفوفات في WASM مدعومة جزئياً");
}

void WasmASTCompiler::visitIndexExpr(AST::IndexExpr& node) {
    if (!currentCode_) return;
    warning("الوصول بالفهرس في WASM مدعوم جزئياً");
    currentCode_->i32Const(0);
}

void WasmASTCompiler::visitMemberAccessExpr(AST::MemberAccessExpr& node) {
    if (!currentCode_) return;
    warning("الوصول للأعضاء في WASM غير مدعوم بعد");
    currentCode_->i32Const(0);
}

void WasmASTCompiler::visitMethodCallExpr(AST::MethodCallExpr& node) {
    if (!currentCode_) return;
    warning("استدعاء الطرق في WASM غير مدعوم بعد");
    currentCode_->i32Const(0);
}

// ════════════════════════════════════════════════════════════════════════════════
//  زيارات العبارات
// ════════════════════════════════════════════════════════════════════════════════

void WasmASTCompiler::visitExprStmt(AST::ExprStmt& node) {
    if (!currentCode_) return;
    node.getExpression().accept(*this);
    
    // تفريغ أي قيمة متبقية على المكدس
    // (العبارات التعبيرية لا ترجع قيمة)
    // نحتاج drop فقط إذا كان التعبير يُنتج قيمة
    // (الاستدعاءات التي ترجع void لا تحتاج drop)
    auto* callExpr = dynamic_cast<AST::CallExpr*>(&node.getExpression());
    if (callExpr) {
        std::string funcName;
        if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(&callExpr->getCallee())) {
            funcName = varExpr->getName();
        }
        auto it = functions_.find(funcName);
        if (it != functions_.end() && !it->second.type.results.empty()) {
            currentCode_->drop();
        }
    }
}

void WasmASTCompiler::visitVarDeclStmt(AST::VarDeclStmt& node) {
    if (!currentCode_) return;

    ValType wt = sadTypeToWasm(node.getType());
    uint32_t localIdx = declareLocal(node.getName(), wt);

    if (node.getInitializer()) {
        node.getInitializer()->accept(*this);
        currentCode_->localSet(localIdx);
    }
}

void WasmASTCompiler::visitIfStmt(AST::IfStmt& node) {
    if (!currentCode_) return;

    node.getCondition().accept(*this);
    currentCode_->ifBlock();
    
    if (node.getThenBranch()) {
        node.getThenBranch()->accept(*this);
    }
    
    if (node.getElseBranch()) {
        currentCode_->elseBlock();
        node.getElseBranch()->accept(*this);
    }
    
    currentCode_->end();
}

void WasmASTCompiler::visitWhileStmt(AST::WhileStmt& node) {
    if (!currentCode_) return;

    loopDepth_++;
    
    // block $break
    //   loop $continue
    //     condition
    //     i32.eqz
    //     br_if $break
    //     body
    //     br $continue
    //   end
    // end
    currentCode_->block();        // $break (depth 1)
    currentCode_->loop();         // $continue (depth 0)
    
    node.getCondition().accept(*this);
    currentCode_->i32Eqz();
    currentCode_->brIf(1);        // br_if $break
    
    if (node.getBody()) {
        node.getBody()->accept(*this);
    }
    
    currentCode_->br(0);           // br $continue
    currentCode_->end();           // end loop
    currentCode_->end();           // end block
    
    loopDepth_--;
}

void WasmASTCompiler::visitForRangeStmt(AST::ForRangeStmt& node) {
    if (!currentCode_) return;

    loopDepth_++;
    pushScope();

    // تخصيص متغير الحلقة
    uint32_t iterLocal = declareLocal(node.getVariable(), ValType::I32);

    // تهيئة (0 افتراضياً — يمكن تحسينه لاحقاً للمصفوفات)
    currentCode_->i32Const(0);
    currentCode_->localSet(iterLocal);

    // حلقة بسيطة
    currentCode_->block();
    currentCode_->loop();
    
    if (node.getBody()) {
        node.getBody()->accept(*this);
    }
    
    // زيادة المتغير
    currentCode_->localGet(iterLocal);
    currentCode_->i32Const(1);
    currentCode_->i32Add();
    currentCode_->localSet(iterLocal);
    
    currentCode_->br(0);
    currentCode_->end();
    currentCode_->end();

    popScope();
    loopDepth_--;
}

void WasmASTCompiler::visitReturnStmt(AST::ReturnStmt& node) {
    if (!currentCode_) return;

    if (node.getValue()) {
        node.getValue()->accept(*this);
    }
    currentCode_->ret();
}

void WasmASTCompiler::visitBreakStmt(AST::BreakStmt&) {
    if (!currentCode_) return;
    // الخروج من أقرب block (الذي يحيط بـ loop)
    currentCode_->br(1); // $break
}

void WasmASTCompiler::visitContinueStmt(AST::ContinueStmt&) {
    if (!currentCode_) return;
    // العودة لبداية الحلقة
    currentCode_->br(0); // $continue
}

void WasmASTCompiler::visitBlockStmt(AST::BlockStmt& node) {
    if (!currentCode_) return;

    pushScope();
    for (const auto& stmt : node.getStatements()) {
        stmt->accept(*this);
    }
    popScope();
}

// ════════════════════════════════════════════════════════════════════════════════
//  زيارات التصريحات
// ════════════════════════════════════════════════════════════════════════════════

void WasmASTCompiler::visitFunctionDecl(AST::FunctionDecl& node) {
    // الدوال تُعالج في compile() مباشرة
    // هذا الزائر لا يُستدعى أثناء ترجمة _start
}

void WasmASTCompiler::visitImportStmt(AST::ImportStmt& node) {
    // الاستيرادات تُعالج بشكل منفصل
}

// ════════════════════════════════════════════════════════════════════════════════
//  توليد ملفات مساعدة
// ════════════════════════════════════════════════════════════════════════════════

std::string WasmASTCompiler::generateJSRuntime() const {
    std::ostringstream js;
    js << R"(/**
 * وقت التشغيل JS لـ WebAssembly المولّد من لغة ص
 * Sad Language WASM Direct Runtime
 * تم التوليد تلقائياً — لا تعدّل يدوياً
 */

export async function loadSadWasm(wasmPath, options = {}) {
    const memory = new WebAssembly.Memory({
        initial: )" << options_.initialMemoryPages << R"(,
        maximum: )" << options_.maxMemoryPages << R"(
    });

    const encoder = new TextEncoder();
    const decoder = new TextDecoder('utf-8');
    let heapBase = )" << (options_.initialMemoryPages * 65536 / 2) << R"(;

    function readString(ptr) {
        const mem = new Uint8Array(memory.buffer);
        let end = ptr;
        while (mem[end] !== 0 && end < mem.length) end++;
        return decoder.decode(mem.slice(ptr, end));
    }

    function writeString(str) {
        const bytes = encoder.encode(str + '\0');
        const ptr = heapBase;
        new Uint8Array(memory.buffer).set(bytes, ptr);
        heapBase += bytes.length + ((8 - bytes.length % 8) % 8);
        return ptr;
    }

    const outputEl = options.outputElement || null;
    const onOutput = options.onOutput || console.log;

    function appendOutput(text) {
        if (outputEl) outputEl.textContent += text;
        if (onOutput) onOutput(text);
    }

    const importObject = {
        env: {
            memory,
            __sad_print: (ptr) => appendOutput(readString(ptr)),
            __sad_println: (ptr) => appendOutput(readString(ptr) + '\n'),
            __sad_print_int: (n) => appendOutput(String(n)),
            __sad_print_float: (n) => appendOutput(String(n)),
            __sad_sqrt: (x) => Math.sqrt(x),
            __sad_pow: (base, exp) => Math.pow(base, exp),
            __sad_random: () => Math.random(),
            __sad_dom_query: (sPtr) => {
                const el = document.querySelector(readString(sPtr));
                return el ? 1 : 0;
            },
            __sad_dom_create: (tPtr) => {
                document.createElement(readString(tPtr));
                return 1;
            },
            __sad_dom_set_text: (h, tPtr) => {},
            __sad_dom_append: (p, c) => {},
            __sad_dom_on_click: (h, cb) => {},
            __sad_malloc: (size) => {
                const ptr = heapBase;
                heapBase += size + ((8 - size % 8) % 8);
                return ptr;
            },
            __sad_free: (ptr) => {},
            __sad_str_len: (ptr) => {
                const mem = new Uint8Array(memory.buffer);
                let len = 0;
                while (mem[ptr + len] !== 0) len++;
                return len;
            },
            __sad_str_concat: (p1, p2) => {
                return writeString(readString(p1) + readString(p2));
            },
            __sad_console_log: (ptr) => console.log(readString(ptr))
        }
    };

    const response = await fetch(wasmPath);
    const bytes = await response.arrayBuffer();
    const { instance } = await WebAssembly.instantiate(bytes, importObject);

    if (instance.exports._start) {
        instance.exports._start();
    }

    return instance;
}
)";
    return js.str();
}

std::string WasmASTCompiler::generateHTMLLoader(const std::string& wasmFileName) const {
    std::ostringstream html;
    html << R"(<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)" << options_.moduleName << R"( — لغة ص WebAssembly</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'IBM Plex Arabic', 'Segoe UI', Tahoma, sans-serif;
            background: #0d1117; color: #c9d1d9;
            display: flex; flex-direction: column; align-items: center;
            min-height: 100vh; padding: 2rem;
        }
        h1 { color: #58a6ff; margin-bottom: 1rem; }
        .badge { background: #238636; color: white; padding: 0.2rem 0.8rem;
                 border-radius: 1rem; font-size: 0.8rem; margin-bottom: 1rem; }
        #output {
            background: #161b22; border: 1px solid #30363d;
            border-radius: 0.5rem; padding: 1rem; width: 100%;
            max-width: 700px; min-height: 200px; font-family: 'Cascadia Code', monospace;
            white-space: pre-wrap; direction: ltr; text-align: left;
            font-size: 0.95rem; line-height: 1.6;
        }
        .loading { color: #8b949e; font-style: italic; }
        .error { color: #f85149; }
        .footer { margin-top: 2rem; color: #484f58; font-size: 0.8rem; }
    </style>
</head>
<body>
    <h1>🏗️ )" << options_.moduleName << R"(</h1>
    <span class="badge">WebAssembly Direct — لغة ص</span>
    <div id="output"><span class="loading">⏳ جاري التحميل...</span></div>
    <p class="footer">تم التوليد تلقائياً بواسطة مترجم لغة ص المباشر</p>

    <script type="module">
        import { loadSadWasm } from './)" << options_.moduleName << R"(_runtime.js';

        const outputEl = document.getElementById('output');
        outputEl.textContent = '';

        try {
            await loadSadWasm('./)" << wasmFileName << R"(', {
                outputElement: outputEl,
                onOutput: (text) => console.log(text)
            });
        } catch (error) {
            outputEl.innerHTML = '<span class="error">❌ خطأ: ' + error.message + '</span>';
            console.error(error);
        }
    </script>
</body>
</html>
)";
    return html.str();
}

} // namespace WasmDirect
} // namespace Compiler
} // namespace Sad
