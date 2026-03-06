/**
 * @file wasm_builtins.cpp
 * @brief WASM-safe reimplementation of all 38 builtin registry parts.
 *
 * Only uses functions confirmed to exist in stdlib/core/builtins.h:
 *   print, println, input, length, append, remove,
 *   upper, lower, split, join,
 *   abs, max, min, sum, sqrt,
 *   type_of, to_int, to_float, to_string, range
 *
 * Platform-specific parts (SDL2, low-level OS, USB, game engine) are stubs.
 */

#include "builtins.h"
#include "interpreter_core.h"

namespace Sad {
namespace Interpreter {

using Data::Value;
using VV = std::vector<std::shared_ptr<Value>>;

// ─── Part 1: I/O + arrays ────────────────────────────────────────────────────
void registerBuiltinsPart1(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    fm.registerBuiltinFunction("اطبع",
        [](const VV& a) { return StdLib::BuiltinFunctions::print(a); });
    fm.registerBuiltinFunction("إطبع",
        [](const VV& a) { return StdLib::BuiltinFunctions::print(a); });
    fm.registerBuiltinFunction("print",
        [](const VV& a) { return StdLib::BuiltinFunctions::print(a); });

    fm.registerBuiltinFunction("اطبع_سطر",
        [](const VV& a) { return StdLib::BuiltinFunctions::println(a); });
    fm.registerBuiltinFunction("println",
        [](const VV& a) { return StdLib::BuiltinFunctions::println(a); });

    fm.registerBuiltinFunction("اقرأ",
        [](const VV& a) { return StdLib::BuiltinFunctions::input(a); });
    fm.registerBuiltinFunction("input",
        [](const VV& a) { return StdLib::BuiltinFunctions::input(a); });

    fm.registerBuiltinFunction("الطول",
        [](const VV& a) { return StdLib::BuiltinFunctions::length(a); });
    fm.registerBuiltinFunction("طول",
        [](const VV& a) { return StdLib::BuiltinFunctions::length(a); });
    fm.registerBuiltinFunction("length",
        [](const VV& a) { return StdLib::BuiltinFunctions::length(a); });
    fm.registerBuiltinFunction("len",
        [](const VV& a) { return StdLib::BuiltinFunctions::length(a); });

    fm.registerBuiltinFunction("أضف",
        [](const VV& a) { return StdLib::BuiltinFunctions::append(a); });
    fm.registerBuiltinFunction("push",
        [](const VV& a) { return StdLib::BuiltinFunctions::append(a); });
    fm.registerBuiltinFunction("append",
        [](const VV& a) { return StdLib::BuiltinFunctions::append(a); });

    fm.registerBuiltinFunction("احذف",
        [](const VV& a) { return StdLib::BuiltinFunctions::remove(a); });
    fm.registerBuiltinFunction("remove",
        [](const VV& a) { return StdLib::BuiltinFunctions::remove(a); });
}

// ─── Part 2: Type conversion ──────────────────────────────────────────────────
void registerBuiltinsPart2(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    fm.registerBuiltinFunction("رقم",
        [](const VV& a) { return StdLib::BuiltinFunctions::to_int(a); });
    fm.registerBuiltinFunction("int",
        [](const VV& a) { return StdLib::BuiltinFunctions::to_int(a); });

    fm.registerBuiltinFunction("عشري",
        [](const VV& a) { return StdLib::BuiltinFunctions::to_float(a); });
    fm.registerBuiltinFunction("float",
        [](const VV& a) { return StdLib::BuiltinFunctions::to_float(a); });

    fm.registerBuiltinFunction("نص",
        [](const VV& a) { return StdLib::BuiltinFunctions::to_string(a); });
    fm.registerBuiltinFunction("str",
        [](const VV& a) { return StdLib::BuiltinFunctions::to_string(a); });

    fm.registerBuiltinFunction("نوع",
        [](const VV& a) { return StdLib::BuiltinFunctions::type_of(a); });
    fm.registerBuiltinFunction("type",
        [](const VV& a) { return StdLib::BuiltinFunctions::type_of(a); });
    fm.registerBuiltinFunction("type_of",
        [](const VV& a) { return StdLib::BuiltinFunctions::type_of(a); });
}

// ─── Parts 3-5: Graphics stubs (SDL2 — disabled in WASM) ─────────────────────
void registerBuiltinsPart3(Interpreter&) {}
void registerBuiltinsPart4(Interpreter&) {}
void registerBuiltinsPart5(Interpreter&) {}

// ─── Part 6: Math ─────────────────────────────────────────────────────────────
void registerBuiltinsPart6(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    fm.registerBuiltinFunction("جذر",
        [](const VV& a) { return StdLib::BuiltinFunctions::sqrt(a); });
    fm.registerBuiltinFunction("sqrt",
        [](const VV& a) { return StdLib::BuiltinFunctions::sqrt(a); });

    fm.registerBuiltinFunction("مطلق",
        [](const VV& a) { return StdLib::BuiltinFunctions::abs(a); });
    fm.registerBuiltinFunction("abs",
        [](const VV& a) { return StdLib::BuiltinFunctions::abs(a); });

    fm.registerBuiltinFunction("أكبر",
        [](const VV& a) { return StdLib::BuiltinFunctions::max(a); });
    fm.registerBuiltinFunction("max",
        [](const VV& a) { return StdLib::BuiltinFunctions::max(a); });

    fm.registerBuiltinFunction("أصغر",
        [](const VV& a) { return StdLib::BuiltinFunctions::min(a); });
    fm.registerBuiltinFunction("min",
        [](const VV& a) { return StdLib::BuiltinFunctions::min(a); });

    fm.registerBuiltinFunction("مجموع",
        [](const VV& a) { return StdLib::BuiltinFunctions::sum(a); });
    fm.registerBuiltinFunction("sum",
        [](const VV& a) { return StdLib::BuiltinFunctions::sum(a); });
}

// ─── Part 7: UI stubs (disabled in WASM) ──────────────────────────────────────
void registerBuiltinsPart7(Interpreter&) {}

// ─── Part 8: String functions ─────────────────────────────────────────────────
void registerBuiltinsPart8(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    fm.registerBuiltinFunction("قسّم",
        [](const VV& a) { return StdLib::BuiltinFunctions::split(a); });
    fm.registerBuiltinFunction("split",
        [](const VV& a) { return StdLib::BuiltinFunctions::split(a); });

    fm.registerBuiltinFunction("اربط",
        [](const VV& a) { return StdLib::BuiltinFunctions::join(a); });
    fm.registerBuiltinFunction("join",
        [](const VV& a) { return StdLib::BuiltinFunctions::join(a); });

    fm.registerBuiltinFunction("كبير",
        [](const VV& a) { return StdLib::BuiltinFunctions::upper(a); });
    fm.registerBuiltinFunction("upper",
        [](const VV& a) { return StdLib::BuiltinFunctions::upper(a); });

    fm.registerBuiltinFunction("صغير",
        [](const VV& a) { return StdLib::BuiltinFunctions::lower(a); });
    fm.registerBuiltinFunction("lower",
        [](const VV& a) { return StdLib::BuiltinFunctions::lower(a); });
}

// ─── Part 9: More stubs ────────────────────────────────────────────────────────
void registerBuiltinsPart9(Interpreter&) {}

// ─── Part 10: Range ────────────────────────────────────────────────────────────
void registerBuiltinsPart10(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    fm.registerBuiltinFunction("نطاق",
        [](const VV& a) { return StdLib::BuiltinFunctions::range(a); });
    fm.registerBuiltinFunction("range",
        [](const VV& a) { return StdLib::BuiltinFunctions::range(a); });
}

// ─── Parts 11-38: Stubs (UI, OS, USB, game engine — disabled in WASM) ─────────
void registerBuiltinsPart11(Interpreter&) {}
void registerBuiltinsPart12(Interpreter&) {}
void registerBuiltinsPart13(Interpreter&) {}
void registerBuiltinsPart14(Interpreter&) {}
void registerBuiltinsPart15(Interpreter&) {}
void registerBuiltinsPart16(Interpreter&) {}
void registerBuiltinsPart17(Interpreter&) {}
void registerBuiltinsPart18(Interpreter&) {}
void registerBuiltinsPart19(Interpreter&) {}
void registerBuiltinsPart20(Interpreter&) {}
void registerBuiltinsPart21(Interpreter&) {}
void registerBuiltinsPart22(Interpreter&) {}
void registerBuiltinsPart23(Interpreter&) {}
void registerBuiltinsPart24(Interpreter&) {}
void registerBuiltinsPart25(Interpreter&) {}
void registerBuiltinsPart26(Interpreter&) {}
void registerBuiltinsPart27(Interpreter&) {}
void registerBuiltinsPart28(Interpreter&) {}
void registerBuiltinsPart29(Interpreter&) {}
void registerBuiltinsPart30(Interpreter&) {}
void registerBuiltinsPart31(Interpreter&) {}
void registerBuiltinsPart32(Interpreter&) {}
void registerBuiltinsPart33(Interpreter&) {}
void registerBuiltinsPart34(Interpreter&) {}
void registerBuiltinsPart35(Interpreter&) {}
void registerBuiltinsPart36(Interpreter&) {}
void registerBuiltinsPart37(Interpreter&) {}
void registerBuiltinsPart38(Interpreter&) {}

} // namespace Interpreter
} // namespace Sad
