const fs = require('fs');
let code = fs.readFileSync('build_wasm.ps1', 'utf8');

// The file has some corrupted right quotes and unclosed constructs due to Windows-1252 to UTF-8 roundtrip.
// Let's just fix the specific syntax errors identified by powershell.
// 1. Missing string terminator at line 256
// 2. Missing '}' in statement block
code = code.replace(/”/g, '"');
code = code.replace(/“/g, '"');

// Strip out any weird characters that might be breaking powershell parsing
// Specifically we will just rewrite the `if` and `else` blocks at the end to be clean.
// Actually, let me just remove all non-ascii chars inside strings if it's too broken, but that's hard.
fs.writeFileSync('build_wasm.ps1', code, 'utf8');
console.log("Fixed quotes");
