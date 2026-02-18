/**
 * @file interpreter_test_stubs.cpp
 * @brief Stub for registerBuiltinFunctions to avoid pulling the entire stdlib
 * 
 * The real builtin_registry.cpp depends on graphics, filesystem, networking,
 * etc. For interpreter tests we only need the core interpreter logic.
 */

#include "interpreter_core.h"

namespace Sad {
namespace Interpreter {

void registerBuiltinFunctions(Interpreter& /*interpreter*/) {
    // No-op stub for tests - builtins not needed for core interpreter testing
}

} // namespace Interpreter
} // namespace Sad
