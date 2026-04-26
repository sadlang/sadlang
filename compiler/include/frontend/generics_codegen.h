// ============================================================================
// generics_codegen.h — واجهة توليد كود المعممات / Generics Codegen Interface
// ============================================================================
// @brief (AR) واجهة تمريرة إنشاء المعممات (Monomorphization Pass)
// @brief (EN) Interface for the generic instantiation (monomorphization) pass
//
// الاستخدام / Usage:
//   #include "generics_codegen.h"
//   SIRModule module = builder.buildModule(ast);
//   runGenericInstantiationPass(module, builder);  // قبل LLVM CodeGen
//   LLVMCodeGen codegen;
//   codegen.generate(module);
// ============================================================================

#pragma once

#include "sir_module.h"
#include "sir_builder.h"

namespace Sad {
namespace Compiler {
namespace SIR {

/// @brief (AR) تشغيل تمريرة إنشاء المعممات — تفحص CALL instructions وتُنشئ نسخاً محددة
/// @brief (EN) Run the generic instantiation pass — scans CALL instructions and creates specializations
/// @param module الوحدة التي تحتوي على SIR
/// @param builder الباني الذي يحتوي على القوالب المسجلة
/// @return true إذا تم إنشاء نسخ جديدة
bool runGenericInstantiationPass(SIRModule& module, SIRBuilder& builder);

} // namespace SIR
} // namespace Compiler
} // namespace Sad
