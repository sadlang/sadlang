/**
 * @file async_codegen.hpp
 * @brief (AR) تعريفات مولّد الكود غير المتزامن
 *        (EN) Async code generator declarations
 * 
 * هذا الملف يعرّف الأنواع والدوال المطلوبة لتوليد كود LLVM IR
 * لآلات الحالة غير المتزامنة (async state machines)
 */

#ifndef SAD_BACKEND_ASYNC_CODEGEN_HPP
#define SAD_BACKEND_ASYNC_CODEGEN_HPP

#include <string>
#include <vector>
#include <memory>

// تعريفات أمامية لـ LLVM
// LLVM forward declarations
namespace llvm {
    class Module;
    class LLVMContext;
    template<typename T> class IRBuilder;
    class NoFolder;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  (AR) أنواع النظام المطلوبة لمولّد الكود غير المتزامن
//  (EN) Type system types needed by async code generator
// ═══════════════════════════════════════════════════════════════════════════════

namespace types {

/**
 * @enum TypeKind
 * @brief (AR) أنواع البيانات للنظام الداخلي
 *        (EN) Data type kinds for internal type system
 */
enum class TypeKind {
    Int8,       // i8
    Int16,      // i16
    Int32,      // i32
    Int64,      // i64
    Float32,    // f32
    Float64,    // f64
    Bool,       // منطقي
    Unit,       // وحدة (فراغ)
    Pointer,    // مؤشر
    Struct,     // هيكل
    Array,      // مصفوفة
    Function,   // دالة
    Unknown     // مجهول
};

/**
 * @struct Type
 * @brief (AR) معلومات النوع المجردة
 *        (EN) Abstract type information
 */
struct Type {
    TypeKind kind = TypeKind::Unknown;
    std::string name;
    
    virtual ~Type() = default;
    
    Type() = default;
    Type(TypeKind k) : kind(k) {}
    Type(TypeKind k, const std::string& n) : kind(k), name(n) {}
};

} // namespace types

// ═══════════════════════════════════════════════════════════════════════════════
//  (AR) أنواع التحويل غير المتزامن (middle layer)
//  (EN) Async transformation types (middle layer)
// ═══════════════════════════════════════════════════════════════════════════════

namespace middle {

/**
 * @enum AsyncStateKind
 * @brief (AR) أنواع حالات آلة الحالة غير المتزامنة
 */
enum class AsyncStateKind {
    START,          // بداية
    AWAIT_POINT,    // نقطة انتظار
    BRANCH,         // تفرع
    LOOP,           // حلقة
    END             // نهاية
};

/**
 * @struct CapturedVariable
 * @brief (AR) متغير ملتقط في آلة الحالة
 */
struct CapturedVariable {
    std::string name;
    std::unique_ptr<types::Type> type;
    int defined_in_state = 0;
    
    CapturedVariable() = default;
    CapturedVariable(const std::string& n, types::TypeKind k, int state = 0)
        : name(n), type(std::make_unique<types::Type>(k)), defined_in_state(state) {}
};

/**
 * @struct AsyncState
 * @brief (AR) حالة واحدة في آلة الحالة
 */
struct AsyncState {
    int id = 0;
    std::string name;
    AsyncStateKind kind = AsyncStateKind::START;
    int next_state = -1;
    
    AsyncState() = default;
    AsyncState(int i, const std::string& n, AsyncStateKind k, int next = -1)
        : id(i), name(n), kind(k), next_state(next) {}
};

/**
 * @struct AwaitPoint
 * @brief (AR) نقطة انتظار في آلة الحالة
 */
struct AwaitPoint {
    int state_id = 0;
    std::string future_name;
    
    AwaitPoint() = default;
    AwaitPoint(int sid, const std::string& fname)
        : state_id(sid), future_name(fname) {}
};

/**
 * @struct AsyncStateMachine
 * @brief (AR) آلة الحالة غير المتزامنة الكاملة
 */
struct AsyncStateMachine {
    std::string name;
    std::vector<AsyncState> states;
    std::vector<CapturedVariable> captured_vars;
    std::vector<AwaitPoint> await_points;
    std::unique_ptr<types::Type> output_type;
    
    AsyncStateMachine() : output_type(std::make_unique<types::Type>(types::TypeKind::Unit)) {}
};

} // namespace middle

// ═══════════════════════════════════════════════════════════════════════════════
//  (AR) واجهة مولّد الكود العامة
//  (EN) Public code generator interface
// ═══════════════════════════════════════════════════════════════════════════════

namespace sad::backend {

/**
 * @brief (AR) توليد كود LLVM IR لآلة حالة async
 *        (EN) Generate LLVM IR code for async state machine
 */
void generateAsyncStateMachineCode(
    const middle::AsyncStateMachine& machine,
    llvm::Module& module,
    llvm::LLVMContext& context,
    llvm::IRBuilder<>& builder
);

} // namespace sad::backend

#endif // SAD_BACKEND_ASYNC_CODEGEN_HPP
