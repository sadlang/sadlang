/**
 * =============================================================================
 * ملف: llvm_entry_point.h
 * الوصف: توليد نقطة دخول النواة (_start) باستخدام LLVM IRBuilder
 * المهمة: م-د01 - تحويل EntryPointCodeGenerator
 * =============================================================================
 *
 * يُولّد دالة _start التي:
 *   1. تُصفّر قسم BSS
 *   2. تُهيّئ المكدس
 *   3. تستدعي دالة النواة الرئيسية
 *   4. تدخل حلقة halt إذا رجعت الدالة
 *
 * يُعلَّم بـ naked (بدون prologue/epilogue من المترجم)
 * =============================================================================
 */

#ifndef SAD_LLVM_ENTRY_POINT_H
#define SAD_LLVM_ENTRY_POINT_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <string>

namespace sad {
namespace codegen {

/**
 * إعدادات نقطة الدخول
 */
struct EntryPointConfig {
    std::string kernelFunctionName = "kernel_main";  // اسم دالة النواة
    std::string entryPointName = "_start";           // اسم نقطة الدخول
    uint64_t stackTop = 0x80000;                     // قمة المكدس (512KB افتراضي)
    bool clearBSS = true;                            // تصفير BSS
    bool initStack = true;                           // تهيئة المكدس
    bool isNaked = true;                             // بدون prologue/epilogue
    bool haltOnReturn = true;                        // halt إذا رجعت الدالة
};

/**
 * مُولّد نقطة دخول النواة
 */
class LLVMEntryPoint {
public:
    explicit LLVMEntryPoint(llvm::Module& module);
    
    /**
     * توليد _start مع الإعدادات المحددة
     * @return مؤشر لدالة _start
     */
    llvm::Function* generate(const EntryPointConfig& config = {});
    
    /**
     * توليد _start من سمة عربية
     * يُحلل: #[نقطة_دخول] أو #[نقطة_دخول(مكدس = 0x100000)]
     */
    llvm::Function* generateFromAttribute(
        const std::string& kernelFunc,
        uint64_t stackSize = 0);

private:
    llvm::Module& m_module;
    llvm::LLVMContext& m_ctx;
    
    // توليد أجزاء _start
    void emitBSSClear(llvm::IRBuilder<>& builder);
    void emitStackInit(llvm::IRBuilder<>& builder, uint64_t stackTop);
    void emitKernelCall(llvm::IRBuilder<>& builder, const std::string& funcName);
    void emitHaltLoop(llvm::IRBuilder<>& builder);
    
    // إعلان رموز BSS الخارجية
    llvm::GlobalVariable* getOrCreateExternSymbol(const std::string& name, llvm::Type* type);
};

} // namespace codegen
} // namespace sad

#endif // SAD_LLVM_ENTRY_POINT_H
