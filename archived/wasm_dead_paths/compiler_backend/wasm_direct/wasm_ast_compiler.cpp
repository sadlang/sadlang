/**
 * @file wasm_ast_compiler.cpp
 * @brief (AR) ״×״·״¨‚ …״×״±״¬… AST ״¥„‰ WASM ״«†״§״¦ …״¨״§״´״±
 * @brief (EN) AST to direct WASM binary compiler implementation
 *
 * ״­ˆ‘„ ״´״¬״±״© AST …† „״÷״© ״µ ״¥„‰ …„ .wasm ״«†״§״¦
 * ״¨״¯ˆ† ״£ ״§״¹״×…״§״¯ ״¹„‰ LLVM ״£ˆ Emscripten
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

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//  ״¥†״´״§״¡ ˆ״×‡״¦״©
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

WasmASTCompiler::WasmASTCompiler(const WasmCompileOptions& options)
    : options_(options) {}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//  ״×״±״¬…״© ״§„״¨״±†״§…״¬
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

WasmCompileResult WasmASTCompiler::compile(
    const std::vector<std::unique_ptr<AST::Statement>>& statements) {
    
    WasmCompileResult result;
    emitter_ = std::make_unique<WasmEmitter>();
    errors_.clear();
    warnings_.clear();
    functions_.clear();
    stringPool_.clear();
    nextStringOffset_ = 1024;

    // ״×‡״¦״© ״§„״°״§ƒ״±״©
    emitter_->setMemory(options_.initialMemoryPages, options_.maxMemoryPages);
    emitter_->exportMemory("memory");

    // ״×״³״¬„ ״§„״¯ˆ״§„ ״§„…״³״×ˆ״±״¯״© ״§„‚״§״³״©
    setupStandardImports();

    // ״×״³״¬„ ״§„…״×״÷״±״§״× ״§„״¹״§…״©
    setupGlobals();

    // ג•ג•ג• ״§„…״±״­„״© 1: ״×״³״¬„ ״¬…״¹ ״§„״¯ˆ״§„ (forward declaration) ג•ג•ג•
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

            // ״×״µ״¯״± ״§„״¯״§„״© ״§„״±״¦״³״© ˆ״§„״¯ˆ״§„ ״§„״¹״§…״©
            if (funcDecl->getName() == "״±״¦״³״©" || 
                funcDecl->getName() == "main" ||
                options_.exportAllFunctions) {
                emitter_->exportFunction(funcDecl->getName(), funcIdx);
            }
        }
    }

    // ג•ג•ג• ״§„…״±״­„״© 2: ״¥†״´״§״¡ _start (†‚״·״© ״§„״¯״®ˆ„) ג•ג•ג•
    // ״×״¬…״¹ ״§„ƒˆ״¯ ״§„״¹״§… (״®״§״±״¬ ״§„״¯ˆ״§„)  ״¯״§„״© _start
    WasmFuncType startType;
    // _start „״§ ״×״£״®״° …״¹״§…„״§״× ˆ„״§ ״×״±״¬״¹ ״´״¦״§‹
    uint32_t startFuncIdx = emitter_->addFunction(startType);
    emitter_->exportFunction("_start", startFuncIdx);
    functions_["_start"] = {startFuncIdx, startType, false};

    // ג•ג•ג• ״§„…״±״­„״© 3: ״×״±״¬…״© ״£״¬״³״§… ״§„״¯ˆ״§„ ג•ג•ג•
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

            // ״×״³״¬„ ״§„…״¹״§…„״§״×
            for (const auto& param : funcDecl->getParameters()) {
                ValType wt = sadTypeToWasm(param.type);
                declareLocal(param.name, wt, true);
            }

            // ״×״±״¬…״© ״§„״¬״³…
            if (funcDecl->getBody()) {
                funcDecl->getBody()->accept(*this);
            }

            popScope();

            // ״¥״°״§ „… ƒ† ‡†״§ƒ return ״µ״±״­ ˆ״§„״¯״§„״© ״×״±״¬״¹ ‚…״©״ ״£״¶ ‚…״© ״§״×״±״§״¶״©
            if (!it->second.type.results.empty()) {
                auto rt = it->second.type.results[0];
                if (rt == ValType::I32) code.i32Const(0);
                else if (rt == ValType::F64) code.f64Const(0.0);
            }

            emitter_->setFunctionBody(it->second.wasmIndex, code.buildBody());
            currentCode_ = nullptr;
        }
    }

    // ג•ג•ג• ״§„…״±״­„״© 4: ״×״±״¬…״© ״§„ƒˆ״¯ ״§„״¹״§… (״®״§״±״¬ ״§„״¯ˆ״§„) ״¥„‰ _start ג•ג•ג•
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

        // ״§״³״×״¯״¹״§״¡ ״±״¦״³״©() ״¥״°״§ ˆ״¬״¯״×
        auto mainIt = functions_.find("״±״¦״³״©");
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

    // ג•ג•ג• ״§„…״±״­„״© 5: ״¨†״§״¡ ״§„״«†״§״¦ ג•ג•ג•
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
    // ‚״±״§״¡״© ״§„…„ ״§„…״µ״¯״±
    std::ifstream file(inputPath, std::ios::binary);
    if (!file) {
        errors_.push_back("״´„  ״×״­ ״§„…„: " + inputPath);
        return false;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();

    // ״×״­„„ …״¹״¬… ˆ†״­ˆ
    Lexer::LexerCore lexer(source);
    Parser::ParserCore parser(lexer);
    auto program = parser.parseProgram();
    
    if (parser.hasErrors()) {
        errors_.push_back("״£״®״·״§״¡  ״§„״×״­„„ ״§„†״­ˆ");
        return false;
    }

    // ״×״±״¬…״©
    auto result = compile(program);
    if (!result.success) return false;

    // ƒ״×״§״¨״© .wasm
    std::ofstream wasmFile(outputPath, std::ios::binary);
    if (!wasmFile) {
        errors_.push_back("״´„  ƒ״×״§״¨״© ״§„…„: " + outputPath);
        return false;
    }
    wasmFile.write(reinterpret_cast<const char*>(result.wasmBinary.data()),
                   result.wasmBinary.size());

    // ƒ״×״§״¨״© …„ JS runtime
    std::string jsPath = outputPath;
    auto dotPos = jsPath.rfind('.');
    if (dotPos != std::string::npos) jsPath = jsPath.substr(0, dotPos);
    jsPath += "_runtime.js";
    std::ofstream jsFile(jsPath);
    if (jsFile) jsFile << result.jsRuntime;

    // ƒ״×״§״¨״© …„ HTML
    std::string htmlPath = jsPath.substr(0, jsPath.rfind('_')) + ".html";
    std::ofstream htmlFile(htmlPath);
    if (htmlFile) htmlFile << result.htmlLoader;

    return true;
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//  ״§„״×‡״¦״©
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void WasmASTCompiler::setupStandardImports() {
    // ״¯ˆ״§„ ״§„״·״¨״§״¹״©
    WasmFuncType printType;
    printType.params = {ValType::I32}; // …״₪״´״± ״§„†״µ
    auto printIdx = emitter_->addImport("env", "__sad_print", printType);
    functions_["״§״·״¨״¹"] = {printIdx, printType, true};
    functions_["print"] = {printIdx, printType, true};

    WasmFuncType printlnType;
    printlnType.params = {ValType::I32};
    auto printlnIdx = emitter_->addImport("env", "__sad_println", printlnType);
    functions_["״§״·״¨״¹_״³״·״±"] = {printlnIdx, printlnType, true};
    functions_["println"] = {printlnIdx, printlnType, true};

    WasmFuncType printIntType;
    printIntType.params = {ValType::I32};
    auto printIntIdx = emitter_->addImport("env", "__sad_print_int", printIntType);
    functions_["__sad_print_int"] = {printIntIdx, printIntType, true};

    WasmFuncType printFloatType;
    printFloatType.params = {ValType::F64};
    auto printFloatIdx = emitter_->addImport("env", "__sad_print_float", printFloatType);
    functions_["__sad_print_float"] = {printFloatIdx, printFloatType, true};

    // ״¯ˆ״§„ ״§„״±״§״¶״§״×
    WasmFuncType sqrtType;
    sqrtType.params = {ValType::F64};
    sqrtType.results = {ValType::F64};
    auto sqrtIdx = emitter_->addImport("env", "__sad_sqrt", sqrtType);
    functions_["״¬״°״±"] = {sqrtIdx, sqrtType, true};
    functions_["sqrt"] = {sqrtIdx, sqrtType, true};

    WasmFuncType powType;
    powType.params = {ValType::F64, ValType::F64};
    powType.results = {ValType::F64};
    auto powIdx = emitter_->addImport("env", "__sad_pow", powType);
    functions_["״£״³"] = {powIdx, powType, true};
    functions_["pow"] = {powIdx, powType, true};

    WasmFuncType randomType;
    randomType.results = {ValType::F64};
    auto randIdx = emitter_->addImport("env", "__sad_random", randomType);
    functions_["״¹״´ˆ״§״¦"] = {randIdx, randomType, true};

    // ״¯ˆ״§„ DOM
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

    // ״¯ˆ״§„ ״§„״°״§ƒ״±״©
    WasmFuncType mallocType;
    mallocType.params = {ValType::I32};
    mallocType.results = {ValType::I32};
    auto mallocIdx = emitter_->addImport("env", "__sad_malloc", mallocType);
    functions_["__sad_malloc"] = {mallocIdx, mallocType, true};

    WasmFuncType freeType;
    freeType.params = {ValType::I32};
    auto freeIdx = emitter_->addImport("env", "__sad_free", freeType);
    functions_["__sad_free"] = {freeIdx, freeType, true};

    // ״¯ˆ״§„ ״§„†״µˆ״µ
    WasmFuncType strLenType;
    strLenType.params = {ValType::I32};
    strLenType.results = {ValType::I32};
    auto strLenIdx = emitter_->addImport("env", "__sad_str_len", strLenType);
    functions_["״§„״·ˆ„"] = {strLenIdx, strLenType, true};
    functions_["״·ˆ„"] = {strLenIdx, strLenType, true};

    WasmFuncType strConcatType;
    strConcatType.params = {ValType::I32, ValType::I32};
    strConcatType.results = {ValType::I32};
    auto strConcatIdx = emitter_->addImport("env", "__sad_str_concat", strConcatType);
    functions_["__sad_str_concat"] = {strConcatIdx, strConcatType, true};

    // ״¯ˆ״§„ console
    WasmFuncType consoleType;
    consoleType.params = {ValType::I32};
    auto consoleIdx = emitter_->addImport("env", "__sad_console_log", consoleType);
    functions_["__sad_console_log"] = {consoleIdx, consoleType, true};
}

void WasmASTCompiler::setupGlobals() {
    // …״₪״´״± ״§„ƒˆ…״©
    emitter_->addGlobal(ValType::I32, true, 
                        static_cast<int32_t>(options_.initialMemoryPages * 65536 / 2));
    // …״₪״´״± ״§„…ƒ״¯״³
    emitter_->addGlobal(ValType::I32, true,
                        static_cast<int32_t>(options_.initialMemoryPages * 65536));
}

uint32_t WasmASTCompiler::addString(const std::string& str) {
    auto it = stringPool_.find(str);
    if (it != stringPool_.end()) return it->second;

    uint32_t offset = nextStringOffset_;
    emitter_->addDataSegment(offset, str);
    nextStringOffset_ = offset + static_cast<uint32_t>(str.size()) + 1;
    // …״­״§״°״§״© ״¹„‰ 4 ״¨״§״×
    nextStringOffset_ = (nextStringOffset_ + 3) & ~3u;
    stringPool_[str] = offset;
    return offset;
}

ValType WasmASTCompiler::sadTypeToWasm(AST::DataType type) {
    switch (type) {
        case AST::DataType::INTEGER:  return ValType::I32;
        case AST::DataType::FLOAT:    return ValType::F64;
        case AST::DataType::BOOLEAN:  return ValType::I32;
        case AST::DataType::STRING:   return ValType::I32; // …״₪״´״±
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

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//  ״²״§״±״§״× ״§„״×״¹״¨״±״§״×
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void WasmASTCompiler::visitBinaryExpr(AST::BinaryExpr& node) {
    if (!currentCode_) return;

    // ״­״§„״© ״®״§״µ״©: ״¬…״¹ †״µˆ״µ ״¹״¨״± __sad_str_concat
    if (node.getOp() == TT::PLUS) {
        // (AR) ״¥״°״§ ƒ״§† ״£״­״¯ ״§„״·״±† †״µ״§‹״ †״³״×״®״¯… ״¯״§„״© ״±״¨״· ״§„†״µˆ״µ
        // (EN) If either operand is a string literal, use string concat function
        bool leftIsString = dynamic_cast<AST::StringLiteral*>(&node.getLeft()) != nullptr;
        bool rightIsString = dynamic_cast<AST::StringLiteral*>(&node.getRight()) != nullptr;
        if (leftIsString || rightIsString) {
            node.getLeft().accept(*this);
            node.getRight().accept(*this);
            auto it = functions_.find("__sad_str_concat");
            if (it != functions_.end()) {
                currentCode_->call(it->second.wasmIndex);
            } else {
                warning("״¯״§„״© __sad_str_concat ״÷״± …״³״×ˆ״±״¯״© ג€” ״±״¨״· ״§„†״µˆ״µ „† ״¹…„");
            }
            return;
        }
    }

    // ״×״±״¬…״© ״§„״·״±†
    node.getLeft().accept(*this);
    node.getRight().accept(*this);

    // ״§„״¹…„״©
    switch (node.getOp()) {
        // ״­״³״§״¨״©
        case TT::PLUS:         currentCode_->i32Add(); break;
        case TT::MINUS:        currentCode_->i32Sub(); break;
        case TT::STAR:         currentCode_->i32Mul(); break;
        case TT::SLASH:        currentCode_->i32DivS(); break;
        case TT::PERCENT:      currentCode_->i32RemS(); break;

        // …‚״§״±†״©
        case TT::EQUAL_EQUAL:  currentCode_->i32Eq(); break;
        case TT::BANG_EQUAL:
        case TT::NOT_EQUAL:    currentCode_->i32Ne(); break;
        case TT::LESS:         currentCode_->i32LtS(); break;
        case TT::GREATER:      currentCode_->i32GtS(); break;
        case TT::LESS_EQUAL:   currentCode_->i32LeS(); break;
        case TT::GREATER_EQUAL: currentCode_->i32GeS(); break;

        // …†״·‚״©
        case TT::AND:
        case TT::KEYWORD_AND:  currentCode_->i32And(); break;
        case TT::OR:
        case TT::KEYWORD_OR:   currentCode_->i32Or(); break;

        // ״¨״×״©
        case TT::AMPERSAND:    currentCode_->i32And(); break;
        case TT::PIPE:         currentCode_->i32Or(); break;
        case TT::CARET:        currentCode_->i32Xor(); break;
        case TT::SHIFT_LEFT:   currentCode_->i32Shl(); break;
        case TT::SHIFT_RIGHT:  currentCode_->i32ShrS(); break;

        default:
            warning("״¹…„״© ״«†״§״¦״© ״÷״± …״¯״¹ˆ…״©  WASM");
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
            // †״­״×״§״¬ ״×״¨״¯„ ״§„״×״±״×״¨: †״¶״¹ 0 ״£ˆ„״§‹ ״«… x
            // „ƒ† x ״¹„‰ ״§„…ƒ״¯״³ ״¨״§„״¹„״ „״°״§ †״³״×״®״¯… ״­„״©:
            // swap: local.tee tmp, drop, i32.const 0, local.get tmp, i32.sub
            // ״¨״¯„ ״£״¨״³״·: i32.const -1, i32.mul
            currentCode_->i32Const(-1);
            currentCode_->i32Mul();
            break;
        case TT::BANG:
        case TT::KEYWORD_NOT:
            currentCode_->i32Eqz();
            break;
        default:
            warning("״¹…„״© ״£״­״§״¯״© ״÷״± …״¯״¹ˆ…״©  WASM");
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
        warning("…״×״÷״± ״÷״± …״¹״±‘: " + node.getName());
        currentCode_->i32Const(0);
    }
}

void WasmASTCompiler::visitAssignExpr(AST::AssignExpr& node) {
    if (!currentCode_) return;

    // ״×״±״¬…״© ״§„‚…״©
    node.getValue().accept(*this);

    auto* var = lookupVar(node.getName());
    if (var) {
        currentCode_->localTee(var->localIndex);
    } else {
        warning("…״­״§ˆ„״© ״¥״³†״§״¯ „…״×״÷״± ״÷״± …״¹״±‘: " + node.getName());
    }
}

void WasmASTCompiler::visitCallExpr(AST::CallExpr& node) {
    if (!currentCode_) return;

    // ״§„״­״µˆ„ ״¹„‰ ״§״³… ״§„״¯״§„״©
    std::string funcName;
    if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(&node.getCallee())) {
        funcName = varExpr->getName();
    }

    if (funcName.empty()) {
        warning("״§״³״×״¯״¹״§״¡ ״¯״§„״© ״÷״± …״¨״§״´״± ״÷״± …״¯״¹ˆ… ״¨״¹״¯  WASM ״§„…״¨״§״´״±");
        currentCode_->i32Const(0);
        return;
    }

    // ״­״§„״© ״®״§״µ״©: ״§״·״¨״¹/״§״·״¨״¹_״³״·״± …״¹ †״µ ״£ˆ ״±‚…
    bool isPrint = (funcName == "״§״·״¨״¹" || funcName == "print" ||
                    funcName == "״§״·״¨״¹_״³״·״±" || funcName == "println");

    if (isPrint && !node.getArguments().empty()) {
        auto& arg = *node.getArguments()[0];
        
        // ״¥״°״§ ƒ״§† ״§„ˆ״³״· †״µ״§‹ ״­״±״§‹
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

    // ״§„״¨״­״« ״¹† ״§„״¯״§„״©
    auto it = functions_.find(funcName);
    if (it == functions_.end()) {
        warning("״¯״§„״© ״÷״± …״¹״±‘״©: " + funcName);
        currentCode_->i32Const(0);
        return;
    }

    // ״×״±״¬…״© ״§„ˆ״³״§״¦״·
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
    // ״§„…״µˆ״§״×: ״×״®״µ״µ ״°״§ƒ״±״© ˆ״×״®״²† ״§„״¹†״§״µ״±
    uint32_t count = static_cast<uint32_t>(node.getElements().size());
    uint32_t size = count * 4; // ƒ„ ״¹†״µ״± i32
    
    currentCode_->i32Const(static_cast<int32_t>(size + 4)); // +4 „„״·ˆ„
    auto mallocIt = functions_.find("__sad_malloc");
    if (mallocIt != functions_.end()) {
        currentCode_->call(mallocIt->second.wasmIndex);
    }

    // ״×״®״²† ״§„״·ˆ„  ״§„״¨״¯״§״©
    // ״§„†״×״¬״© (״§„״¹†ˆ״§†) ״¹„‰ ״§„…ƒ״¯״³ ג€” †״­״×״§״¬ ״×״®״²†‡  local
    // ״«… †״³״×״®״¯…‡ „״×״®״²† ״§„״·ˆ„ ˆ״§„״¹†״§״µ״±
    warning("״§„…״µˆ״§״×  WASM …״¯״¹ˆ…״© ״¬״²״¦״§‹");
}

void WasmASTCompiler::visitIndexExpr(AST::IndexExpr& node) {
    if (!currentCode_) return;
    warning("״§„ˆ״µˆ„ ״¨״§„‡״±״³  WASM …״¯״¹ˆ… ״¬״²״¦״§‹");
    currentCode_->i32Const(0);
}

void WasmASTCompiler::visitMemberAccessExpr(AST::MemberAccessExpr& node) {
    if (!currentCode_) return;
    warning("״§„ˆ״µˆ„ „„״£״¹״¶״§״¡  WASM ״÷״± …״¯״¹ˆ… ״¨״¹״¯");
    currentCode_->i32Const(0);
}

void WasmASTCompiler::visitMethodCallExpr(AST::MethodCallExpr& node) {
    if (!currentCode_) return;
    warning("״§״³״×״¯״¹״§״¡ ״§„״·״±‚  WASM ״÷״± …״¯״¹ˆ… ״¨״¹״¯");
    currentCode_->i32Const(0);
}

void WasmASTCompiler::visitOptionalChainExpr(AST::OptionalChainExpr& node) {
    if (!currentCode_) return;
    // (AR) ״§„ˆ״µˆ„ ״§„״§״®״×״§״± ?.  ג€” ״¥״°״§ ƒ״§† ״§„ƒ״§״¦† null ״±״¬״¹ 0 („״§״´״¡)
    // (EN) Optional chain ?. ג€” if object is null, return 0 (null)
    node.getObject().accept(*this);
    // ״×ƒ״±״§״± ״§„‚…״© „„״­״µ + ״§„״§״³״×״®״¯״§…
    // ״¥״°״§ ƒ״§†״× ״§„‚…״© 0 (null) ג†’ ״£״±״¬״¹ 0״ ˆ״¥„״§ ״£״±״¬״¹ 0 (״§„ˆ״µˆ„ „„״¹״¶ˆ ״÷״± …״¯״¹ˆ… ״¨״¹״¯)
    warning("״§„ˆ״µˆ„ ״§„״§״®״×״§״± ?.  WASM …״¯״¹ˆ… ״¬״²״¦״§‹");
    currentCode_->ifBlock(static_cast<uint8_t>(ValType::I32));
    currentCode_->i32Const(0); // ״§„ˆ״µˆ„ „„״¹״¶ˆ ג€” ״­״×״§״¬ ״¯״¹… ƒ״§״¦†״§״×
    currentCode_->elseBlock();
    currentCode_->i32Const(0); // null
    currentCode_->end();
}

void WasmASTCompiler::visitNullCoalesceExpr(AST::NullCoalesceExpr& node) {
    if (!currentCode_) return;
    // (AR) ״¹״§…„ ״§„״§†״¯…״§״¬ ״§„״µ״± ?? ג€” ״¥״°״§ ƒ״§† ״§„״³״§״± null/0 ״³״×״®״¯… ״§„…†
    // (EN) Null coalesce ?? ג€” if left is null/0, use right
    node.getLeft().accept(*this);
    currentCode_->ifBlock(static_cast<uint8_t>(ValType::I32));
    node.getLeft().accept(*this);  // ‚…״© ״§„״³״§״± (״÷״± null)
    currentCode_->elseBlock();
    node.getRight().accept(*this); // ‚…״© ״¨״¯„״©
    currentCode_->end();
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//  ״²״§״±״§״× ״§„״¹״¨״§״±״§״×
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•


} // namespace WasmDirect
} // namespace Compiler
} // namespace Sad

