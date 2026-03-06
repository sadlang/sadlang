const { execSync } = require('child_process');
const fs = require('fs');
const path = require('path');

function getFiles(dir, match) {
    let results = [];
    if (!fs.existsSync(dir)) return results;
    fs.readdirSync(dir).forEach(file => {
        const full = path.join(dir, file);
        if (fs.statSync(full).isDirectory()) {
            results = results.concat(getFiles(full, match));
        } else if (full.endsWith(match)) {
            results.push(full.replace(/\\/g, '/'));
        }
    });
    return results;
}

const folders = [
    "shared",
    "interpreter_new/src",
    "stdlib",
    "graphics/src"
];

let allCppFiles = [];
for (const f of folders) {
    allCppFiles = allCppFiles.concat(getFiles(f, ".cpp")).concat(getFiles(f, ".c"));
}

const txt = fs.readFileSync('cmake/sources.cmake', 'utf8') + '\n' + fs.readFileSync('graphics/CMakeLists.txt', 'utf8');
let allowedNames = new Set([...txt.matchAll(/([a-zA-Z0-9_]+\.cpp)/g)].map(m => m[1]));

let srcs = ['tools/wasm/sad_wasm.cpp', 'graphics/third_party/glad/src/glad.c', ...allCppFiles.filter(f => {
    let name = f.split('/').pop();
    if (!allowedNames.has(name)) return false;
    if (f.includes('low_level/src')) return false;
    if (name.match(/^builtin_registry_part(19|20|21|22|23|24|25|26|27|28|35|36|37|38)(_.*)?\.cpp$/)) return false;
    return true;
}).map(x => x.replace('expression_evaluator_calls.cpp', 'expression_evaluator_calls2.cpp'))];


const includes = [
    ".",
    "shared/lexer/include",
    "shared/parser/include",
    "shared/ast/include",
    "shared/types/include",
    "shared/errors/include",
    "shared/modules/include",
    "shared/utils/include",
    "interpreter_new/include",
    "interpreter_new/include/core",
    "interpreter_new/include/managers",
    "interpreter_new/include/oop",
    "interpreter_new/include/evaluators",
    "interpreter_new/include/statements",
    "interpreter_new/include/builtins",
    "interpreter_new/include/memory",
    "interpreter_new/include/visitors",
    "interpreter_new/include/debug",
    "graphics/include",
    "graphics/third_party",
    "graphics/third_party/glad/include",
    "stdlib",
    "stdlib/include",
    "stdlib/core",
    "stdlib/math",
    "stdlib/string",
    "stdlib/io",
    "stdlib/system",
    "stdlib/filesystem",
    "stdlib/graphics",
    "stdlib/image",
    "stdlib/low_level/include",
    "stdlib/network/include",
    "stdlib/crypto",
    "stdlib/database",
    "stdlib/http",
    "stdlib/json",
    "stdlib/xml"
];

let incFlags = includes.map(i => `-I ${i}`).join(' ');

console.log("Sources to build:", srcs.length);
fs.writeFileSync('srcs.rsp', srcs.join('\n'));

const cmd = `emcc -O2 -s WASM=1 -s DISABLE_EXCEPTION_CATCHING=0 -s USE_SDL=2 -s ASYNCIFY=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','UTF8ToString'] -s ALLOW_MEMORY_GROWTH=1 -s MODULARIZE=1 -s EXPORT_NAME="SadWasm" -s EXPORTED_FUNCTIONS=['_sad_execute','_malloc','_free'] ` + incFlags + ` -D EMSCRIPTEN -o website/docs/public/sad.js @srcs.rsp`;

console.log("Building WASM...");
try {
    process.env.PATH = `C:\\emsdk;C:\\emsdk\\upstream\\emscripten;${process.env.PATH}`;
    execSync(cmd, {stdio: 'inherit'});
    console.log("Success!");
} catch(e) {
    if (e.stdout) console.log(e.stdout.toString());
    if (e.stderr) console.error(e.stderr.toString());
    console.log("Failed.");
}
