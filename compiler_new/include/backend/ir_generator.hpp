/**
 * @file ir_generator.hpp
 * @brief (AR) تعريفات أمامية لمولّد IR
 *        (EN) IR Generator forward declarations
 * 
 * هذا الملف يوفر تعريفات أمامية لمولّد LLVM IR الرئيسي
 */

#ifndef SAD_BACKEND_IR_GENERATOR_HPP
#define SAD_BACKEND_IR_GENERATOR_HPP

// تعريفات أمامية - يتم استخدام المولّد الرئيسي من llvm_codegen.h
// Forward declarations - main generator is from llvm_codegen.h

namespace llvm {
    class Module;
    class LLVMContext;
    class Function;
    class BasicBlock;
    class Value;
    class Type;
    // (AR) ملاحظة: لا نصرّح عن IRBuilder أمامياً لأنه قالب معقد
    //      يتم تضمينه مباشرة من <llvm/IR/IRBuilder.h> عند الحاجة
    // (EN) Note: Don't forward-declare IRBuilder as it's a complex template
    //      Include it directly from <llvm/IR/IRBuilder.h> when needed
}

#endif // SAD_BACKEND_IR_GENERATOR_HPP
