/**
 * @file builtin_registry.cpp
 * @brief (AR) dispatcher - يستدعي الأقسام المقسمة
 */

#include "builtins.h"
#include "interpreter_core.h"

namespace Sad {
namespace Interpreter {

extern void registerBuiltinsPart1(Interpreter& interpreter);
extern void registerBuiltinsPart2(Interpreter& interpreter);
extern void registerBuiltinsPart3(Interpreter& interpreter);
extern void registerBuiltinsPart4(Interpreter& interpreter);
extern void registerBuiltinsPart5(Interpreter& interpreter);
extern void registerBuiltinsPart6(Interpreter& interpreter);
extern void registerBuiltinsPart7(Interpreter& interpreter);
extern void registerBuiltinsPart8(Interpreter& interpreter);
extern void registerBuiltinsPart9(Interpreter& interpreter);
extern void registerBuiltinsPart10(Interpreter& interpreter);
extern void registerBuiltinsPart11(Interpreter& interpreter);
extern void registerBuiltinsPart12(Interpreter& interpreter);
extern void registerBuiltinsPart13(Interpreter& interpreter);
extern void registerBuiltinsPart14(Interpreter& interpreter);
extern void registerBuiltinsPart15(Interpreter& interpreter);
extern void registerBuiltinsPart16(Interpreter& interpreter);
extern void registerBuiltinsPart17(Interpreter& interpreter);
extern void registerBuiltinsPart18(Interpreter& interpreter);

void registerBuiltinFunctions(Interpreter& interpreter) {
    registerBuiltinsPart1(interpreter);
    registerBuiltinsPart2(interpreter);
    registerBuiltinsPart3(interpreter);
    registerBuiltinsPart4(interpreter);
    registerBuiltinsPart5(interpreter);
    registerBuiltinsPart6(interpreter);
    registerBuiltinsPart7(interpreter);
    registerBuiltinsPart8(interpreter);
    registerBuiltinsPart9(interpreter);
    registerBuiltinsPart10(interpreter);
    registerBuiltinsPart11(interpreter);  // UI widget factories (109 widgets)
    registerBuiltinsPart12(interpreter);  // Widget properties, tree ops, text/button/textfield
    registerBuiltinsPart13(interpreter);  // App lifecycle, navigator, state, events, themes
    registerBuiltinsPart14(interpreter);  // v2.0: opacity, shadow, tooltip, constraints, full events
    registerBuiltinsPart15(interpreter);  // v2.1: queries, tree ops, scroll, alignment, shadow, gradient, Flutter helpers
    registerBuiltinsPart16(interpreter);  // v3.0: 50 composite Flutter widgets + inline property support
    registerBuiltinsPart17(interpreter);  // v3.0: 45+ property setters/getters for new Widget fields
    registerBuiltinsPart18(interpreter);  // v4.0: 80 advanced widgets (Material3, Cupertino, Data, Forms, Special)
}

} // namespace Interpreter
} // namespace Sad
