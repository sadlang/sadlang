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
}

} // namespace Interpreter
} // namespace Sad
