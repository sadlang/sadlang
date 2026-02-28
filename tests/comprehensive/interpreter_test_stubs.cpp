/**
 * @file interpreter_test_stubs.cpp
 * @brief Stub for registerBuiltinFunctions to avoid pulling the entire stdlib
 * 
 * The real builtin_registry.cpp depends on graphics, filesystem, networking,
 * etc. For interpreter tests we only need the core interpreter logic.
 * 
 * (AR) ملف بدائل لتجنب ربط المكتبة القياسية الكاملة في اختبارات المفسر
 * (EN) Stub file to avoid linking the full standard library in interpreter tests
 */

#include "interpreter_core.h"
#include "stdlib_manager.h"
#include "builtin_module_registry.h"

namespace Sad {
namespace Interpreter {

void registerBuiltinFunctions(Interpreter& /*interpreter*/) {
    // No-op stub for tests - builtins not needed for core interpreter testing
    // (AR) بديل فارغ — الدوال المضمنة غير مطلوبة لاختبار نواة المفسر
}

} // namespace Interpreter
} // namespace Sad

// Stubs for StandardLibraryManager (real impl in stdlib_manager.cpp)
namespace Sad {
namespace StdLib {

StandardLibraryManager::StandardLibraryManager(Sad::Data::FunctionManager& fm)
    : functionManager_(fm) {}

StandardLibraryManager::~StandardLibraryManager() {}

bool StandardLibraryManager::registerAllFunctions() { 
    // (AR) نُرجع true بدون تسجيل أي دالة — المفسر لا يحتاجها لاختبار النواة
    // (EN) Return true without registering anything — interpreter doesn't need stdlib for core tests
    return true; 
}

bool StandardLibraryManager::registerPhase1_IOFunctions() { return true; }
bool StandardLibraryManager::registerPhase2_StringFunctions() { return true; }
bool StandardLibraryManager::registerPhase3_ArrayFunctions() { return true; }
bool StandardLibraryManager::registerPhase4_MathFunctions() { return true; }
bool StandardLibraryManager::registerPhase5_TypeFunctions() { return true; }

std::string StandardLibraryManager::getRegistrationStatus() const { 
    return "stub — no functions registered"; 
}

} // namespace StdLib
} // namespace Sad

// Stubs for BuiltinModuleRegistry::loadModule (real impl in builtin_registry.cpp)
namespace Sad {
namespace Interpreter {

bool BuiltinModuleRegistry::loadModule(const std::string& /*name*/) { return false; }

} // namespace Interpreter
} // namespace Sad
