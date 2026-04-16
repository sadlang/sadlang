/**
 * @file wasm_ast_compiler_stmts.cpp
 * @brief (AR) زيارة الجمل وتوليد JS/HTML في مترجم WASM
 * @brief (EN) Statement visitors and JS/HTML generation for WASM compiler
 * تم استخراج هذا الملف من wasm_ast_compiler.cpp وفقاً لقاعدة CW-05
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

void WasmASTCompiler::visitExprStmt(AST::ExprStmt& node) {
    if (!currentCode_) return;
    node.getExpression().accept(*this);
    
    // ״×״±״÷ ״£ ‚…״© …״×״¨‚״© ״¹„‰ ״§„…ƒ״¯״³
    // (״§„״¹״¨״§״±״§״× ״§„״×״¹״¨״±״© „״§ ״×״±״¬״¹ ‚…״©)
    // †״­״×״§״¬ drop ‚״· ״¥״°״§ ƒ״§† ״§„״×״¹״¨״± †״×״¬ ‚…״©
    // (״§„״§״³״×״¯״¹״§״¡״§״× ״§„״× ״×״±״¬״¹ void „״§ ״×״­״×״§״¬ drop)
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

    // ״×״®״µ״µ …״×״÷״± ״§„״­„‚״©
    uint32_t iterLocal = declareLocal(node.getVariable(), ValType::I32);

    // ״×‡״¦״© (0 ״§״×״±״§״¶״§‹ ג€” …ƒ† ״×״­״³†‡ „״§״­‚״§‹ „„…״µˆ״§״×)
    currentCode_->i32Const(0);
    currentCode_->localSet(iterLocal);

    // ״­„‚״© ״¨״³״·״©
    currentCode_->block();
    currentCode_->loop();
    
    if (node.getBody()) {
        node.getBody()->accept(*this);
    }
    
    // ״²״§״¯״© ״§„…״×״÷״±
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
    // ״§„״®״±ˆ״¬ …† ״£‚״±״¨ block (״§„״° ״­״· ״¨€ loop)
    currentCode_->br(1); // $break
}

void WasmASTCompiler::visitContinueStmt(AST::ContinueStmt&) {
    if (!currentCode_) return;
    // ״§„״¹ˆ״¯״© „״¨״¯״§״© ״§„״­„‚״©
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

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//  ״²״§״±״§״× ״§„״×״µ״±״­״§״×
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

void WasmASTCompiler::visitFunctionDecl(AST::FunctionDecl& node) {
    // ״§„״¯ˆ״§„ ״×״¹״§„״¬  compile() …״¨״§״´״±״©
    // ‡״°״§ ״§„״²״§״¦״± „״§ ״³״×״¯״¹‰ ״£״«†״§״¡ ״×״±״¬…״© _start
}

void WasmASTCompiler::visitImportStmt(AST::ImportStmt& node) {
    // ״§„״§״³״×״±״§״¯״§״× ״×״¹״§„״¬ ״¨״´ƒ„ …†״µ„
}

// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
//  ״×ˆ„״¯ …„״§״× …״³״§״¹״¯״©
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

std::string WasmASTCompiler::generateJSRuntime() const {
    std::ostringstream js;
    js << R"(/**
 * ˆ‚״× ״§„״×״´״÷„ JS „€ WebAssembly ״§„…ˆ„‘״¯ …† „״÷״© ״µ
 * Sad Language WASM Direct Runtime
 * ״×… ״§„״×ˆ„״¯ ״×„‚״§״¦״§‹ ג€” „״§ ״×״¹״¯‘„ ״¯ˆ״§‹
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
    <title>)" << options_.moduleName << R"( ג€” „״÷״© ״µ WebAssembly</title>
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
    <h1>נ—ן¸ )" << options_.moduleName << R"(</h1>
    <span class="badge">WebAssembly Direct ג€” „״÷״© ״µ</span>
    <div id="output"><span class="loading">ג³ ״¬״§״± ״§„״×״­…„...</span></div>
    <p class="footer">״×… ״§„״×ˆ„״¯ ״×„‚״§״¦״§‹ ״¨ˆ״§״³״·״© …״×״±״¬… „״÷״© ״µ ״§„…״¨״§״´״±</p>

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
            outputEl.innerHTML = '<span class="error">ג ״®״·״£: ' + error.message + '</span>';
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

