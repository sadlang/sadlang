const { execSync } = require('child_process');
// const glob = require('glob');
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
    "shared/lexer/src",
    "shared/parser/src",
    "shared/types/src",
    "shared/errors/src",
    "shared/modules/src",
    "shared/utils/src",
    "interpreter_new/src/core",
    "interpreter_new/src/evaluators",
    "interpreter_new/src/managers",
    "interpreter_new/src/oop",
    "interpreter_new/src/statements",
    "interpreter_new/src/builtins",
    "interpreter_new/src/memory",
    "interpreter_new/src/visitors",
    "stdlib/core",
    "stdlib/math",
    "stdlib/string",
    "stdlib/io",
    "stdlib/system",
    "stdlib/filesystem",
    "stdlib/graphics",
    "stdlib/image",
    "graphics/src"
];

let srcs = ["tools/wasm/sad_wasm.cpp"];
for (const f of folders) {
    let newFiles = getFiles(f, ".cpp");
    newFiles = newFiles.filter(file => !file.includes("async_parser.cpp") && !file.includes("await_parser.cpp"));
    srcs = srcs.concat(newFiles);
}

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
    "runtime_new/include",
    "compiler_new/include",
    "graphics/include",
    "stdlib/include",
    "stdlib/core",
    "stdlib/math",
    "stdlib/string",
    "stdlib/io",
    "stdlib/system",
    "stdlib/filesystem",
    "stdlib/graphics",
    "stdlib/image"
];

let incFlags = includes.map(i => `-I ${i}`).join(' ');

const cmd = `emcc -O2 -s WASM=1 -s USE_SDL=2 -s ASYNCIFY=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','UTF8ToString'] -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=['_sad_execute','_malloc','_free'] ` + incFlags + ` -D EMSCRIPTEN -o website/docs/public/sad.js @srcs.rsp`;

fs.writeFileSync('srcs.rsp', srcs.join('\n'));

console.log("Building WASM...");
try {
    process.env.PATH = `C:\\emsdk;C:\\emsdk\\upstream\\emscripten;${process.env.PATH}`;
    execSync(cmd, {stdio: 'inherit'});
    console.log("Success!");
} catch(e) {
    console.log("Failed.");
}
