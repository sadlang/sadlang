"""
wasm_build.py - Build sad.wasm using emcc (avoids PowerShell Arabic encoding issues)
Usage: python wasm_build.py [--opt 0|1|2]
"""
import subprocess, sys, os, shutil, time

R = os.path.dirname(os.path.abspath(__file__))
BUILD = os.path.join(R, "build_wasm")
os.makedirs(BUILD, exist_ok=True)

OUT_JS   = os.path.join(BUILD, "sad.js")
WEB_PUB  = os.path.join(R, "website", "docs", "public")
OPT      = sys.argv[2] if len(sys.argv) > 2 else (sys.argv[1] if len(sys.argv) > 1 else "1")

# Add emsdk to PATH
os.environ["PATH"] = r"C:\emsdk;C:\emsdk\upstream\emscripten;" + os.environ.get("PATH", "")

def S(*parts):
    return os.path.join(R, *parts)

SRCS = [
    # utils
    S("shared","utils","src","string_utils.cpp"),
    # lexer
    S("shared","lexer","src","token.cpp"),
    S("shared","lexer","src","lexer_core.cpp"),
    S("shared","lexer","src","lexer_keywords.cpp"),
    # parser
    S("shared","parser","src","parser_core.cpp"),
    S("shared","parser","src","parser_core_main.cpp"),
    S("shared","parser","src","parser_declarations.cpp"),
    S("shared","parser","src","parser_statements.cpp"),
    S("shared","parser","src","parser_expressions.cpp"),
    S("shared","parser","src","parser_advanced.cpp"),
    S("shared","parser","src","parser_core_helpers.cpp"),
    S("shared","parser","src","parser_core_oop.cpp"),
    S("shared","parser","src","parser_modules.cpp"),
    # AST
    S("shared","ast","src","ast_node.cpp"),
    S("shared","ast","src","ast_printer.cpp"),
    S("shared","ast","src","expressions.cpp"),
    S("shared","ast","src","statements.cpp"),
    S("shared","ast","src","declarations.cpp"),
    S("shared","ast","src","property_nodes.cpp"),
    S("shared","ast","src","module_nodes.cpp"),
    S("shared","ast","src","type_nodes.cpp"),
    S("shared","ast","src","class_nodes.cpp"),
    # types
    S("shared","types","src","value.cpp"),
    S("shared","types","src","generator.cpp"),
    S("shared","types","src","class_type.cpp"),
    S("shared","types","src","object_instance.cpp"),
    # errors
    S("shared","errors","src","error_codes.cpp"),
    S("shared","errors","src","diagnostic.cpp"),
    S("shared","errors","src","error_manager.cpp"),
    S("shared","errors","src","smart_errors.cpp"),
    S("shared","errors","src","smart_suggestions.cpp"),
    S("shared","errors","src","smart_type_explanations.cpp"),
    S("shared","errors","src","smart_multi_error.cpp"),
    S("shared","errors","src","smart_fix_suggestions.cpp"),
    S("shared","errors","src","smart_teacher_mode.cpp"),
    S("shared","errors","src","smart_error_recovery.cpp"),
    S("shared","errors","src","smart_cascade_prevention.cpp"),
    # modules
    S("shared","modules","src","module.cpp"),
    S("shared","modules","src","search_paths.cpp"),
    S("shared","modules","src","module_cache.cpp"),
    S("shared","modules","src","dependency_graph.cpp"),
    S("shared","modules","src","module_loader.cpp"),
    S("shared","modules","src","module_resolver.cpp"),
    S("shared","modules","src","symbol_resolver.cpp"),
    S("shared","modules","src","module_validator.cpp"),
    # interpreter core
    S("interpreter_new","src","core","interpreter_core.cpp"),
    S("interpreter_new","src","visitors","expression_evaluator_core.cpp"),
    S("interpreter_new","src","visitors","expression_evaluator_calls.cpp"),
    S("interpreter_new","src","visitors","expression_evaluator_oop.cpp"),
    S("interpreter_new","src","visitors","expression_evaluator_members.cpp"),
    S("interpreter_new","src","visitors","statement_executor.cpp"),
    S("interpreter_new","src","visitors","statement_executor_oop.cpp"),
    S("interpreter_new","src","visitors","statement_executor_modules.cpp"),
    S("interpreter_new","src","exception.cpp"),
    # managers
    S("interpreter_new","src","managers","variable_manager.cpp"),
    S("interpreter_new","src","managers","function_manager.cpp"),
    S("interpreter_new","src","managers","scope_manager.cpp"),
    S("interpreter_new","src","managers","ownership_manager.cpp"),
    S("interpreter_new","src","managers","class_manager.cpp"),
    S("interpreter_new","src","managers","object_manager.cpp"),
    # builtins — WASM-safe stub file replaces all 38 platform parts
    S("interpreter_new","src","builtins","builtin_registry.cpp"),
    S("tools","wasm","wasm_builtins.cpp"),
    # stdlib (core only, no graphics/http/db/network)
    S("stdlib","core","builtins.cpp"),
    S("stdlib","core","stdlib_manager.cpp"),
    S("stdlib","core","type_functions.cpp"),
    S("stdlib","core","array_functions.cpp"),
    S("stdlib","core","other_functions.cpp"),
    S("stdlib","string","string_functions.cpp"),
    S("stdlib","math","math_functions.cpp"),
    S("stdlib","math","advanced_math.cpp"),
    S("stdlib","io","io_functions.cpp"),
    # WASM entry point
    S("tools","wasm","sad_wasm.cpp"),
]

# Report missing files
missing = [f for f in SRCS if not os.path.isfile(f)]
if missing:
    print(f"WARNING: {len(missing)} missing files:")
    for f in missing:
        print(f"  MISSING: {f}")

INCS = [
    f"-I{R}",
    f"-I{S('shared','utils','include')}",
    f"-I{S('shared','lexer','include')}",
    f"-I{S('shared','parser','include')}",
    f"-I{S('shared','ast','include')}",
    f"-I{S('shared','types','include')}",
    f"-I{S('shared','errors','include')}",
    f"-I{S('shared','modules','include')}",
    f"-I{S('interpreter_new','include')}",
    f"-I{S('interpreter_new','include','core')}",
    f"-I{S('interpreter_new','include','visitors')}",
    f"-I{S('interpreter_new','include','managers')}",
    f"-I{S('compiler_new','include')}",
    # stdlib: top-level so "io/io_functions.h", "graphics/sad_*.h" etc. resolve
    f"-I{S('stdlib')}",
    f"-I{S('stdlib','core')}",
    f"-I{S('stdlib','string')}",
    f"-I{S('stdlib','math')}",
    f"-I{S('stdlib','io')}",
    f"-I{S('stdlib','system')}",
    f"-I{S('stdlib','filesystem')}",
    f"-I{S('stdlib','graphics')}",
    f"-I{S('stdlib','mobile')}",
    f"-I{S('stdlib','network','include')}",
    f"-I{S('stdlib','low_level','include')}",
    f"-I{S('stdlib','http')}",
    f"-I{S('stdlib','database')}",
    f"-I{S('stdlib','crypto')}",
    f"-I{S('stdlib','json')}",
    f"-I{S('stdlib','xml')}",
    f"-I{S('stdlib','image')}",
]

DEFS = [
    "-DSAD_WASM_BUILD=1",
    "-DHAD_GRAPHICS=0",
    "-DSAD_NO_FILESYSTEM=1",
    "-DSAD_NO_NETWORK=1",
    "-DSAD_NO_SDL2=1",
]

FLAGS = [
    f"-O{OPT}",
    "-std=c++17",
    "-fexceptions",
    "-sWASM=1",
    "-sMODULARIZE=1",
    "-sEXPORT_NAME=SadWasm",
    '-sEXPORTED_FUNCTIONS=["_sad_execute","_sad_version","_malloc","_free"]',
    '-sEXPORTED_RUNTIME_METHODS=["cwrap","ccall","UTF8ToString","stringToUTF8","lengthBytesUTF8"]',
    "-sALLOW_MEMORY_GROWTH=1",
    "-sMAXIMUM_MEMORY=268435456",
    "-sSTACK_SIZE=5MB",
    "-sFORCE_FILESYSTEM=0",
    "--no-entry",
    "-o", OUT_JS,
]

# On Windows, emcc is a .bat file — must use shell=True or explicit path
# Use a response file to avoid Windows 8192-char command line limit
emcc_bat = r"C:\emsdk\upstream\emscripten\emcc.bat"
emcc_cmd = emcc_bat if os.path.isfile(emcc_bat) else "emcc"

# Write all args to a response file — use forward slashes (emcc is Python under the hood)
args_all = DEFS + INCS + SRCS + FLAGS
rsp_file = os.path.join(BUILD, "sad_wasm.rsp")
with open(rsp_file, "w", encoding="utf-8") as f:
    for arg in args_all:
        arg_fwd = arg.replace("\\", "/")
        # Quote args with spaces
        if " " in arg_fwd:
            f.write(f'"{arg_fwd}"\n')
        else:
            f.write(f'{arg_fwd}\n')

rsp_fwd = rsp_file.replace("\\", "/")
cmd_rsp = f'"{emcc_cmd}" @{rsp_fwd}'

print(f"\nBuilding sad.wasm (O{OPT}) ...")
print(f"  emcc    : {emcc_cmd}")
print(f"  Sources : {len(SRCS)} files")
print(f"  Rsp file: {rsp_file}")
print(f"  Output  : {OUT_JS}\n")

t0 = time.time()
result = subprocess.run(cmd_rsp, cwd=R, shell=True)
elapsed = int(time.time() - t0)

if result.returncode == 0:
    js_kb   = os.path.getsize(OUT_JS)               // 1024
    wasm_kb = os.path.getsize(OUT_JS.replace('.js','.wasm')) // 1024
    print(f"\nSUCCESS in {elapsed}s")
    print(f"  sad.js   = {js_kb} KB")
    print(f"  sad.wasm = {wasm_kb} KB")
    os.makedirs(WEB_PUB, exist_ok=True)
    shutil.copy(OUT_JS,                        os.path.join(WEB_PUB, "sad.js"))
    shutil.copy(OUT_JS.replace('.js','.wasm'), os.path.join(WEB_PUB, "sad.wasm"))
    print(f"  Copied to: {WEB_PUB}")
    print(f"  Next: cd website && npx vitepress build .")
else:
    print(f"\nBUILD FAILED (exit={result.returncode})")
    sys.exit(result.returncode)
