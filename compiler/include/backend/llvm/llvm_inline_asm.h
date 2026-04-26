/**
 * =============================================================================
 * ملف: llvm_inline_asm.h
 * الوصف: توليد تعليمات التجميع المُضمّنة عبر LLVM IRBuilder
 * المهمة: م-د03 - تحويل InlineAsmCodeGenerator
 * =============================================================================
 *
 * يحوّل تعليمات التجميع العربية إلى inline asm حقيقي:
 *   تجميع! { "cli" }           → call void asm sideeffect "cli", ""()
 *   تجميع! { "hlt" }           → call void asm sideeffect "hlt", ""()
 *   توقف!                       → call void asm sideeffect "hlt", ""()
 * =============================================================================
 */

#ifndef SAD_LLVM_INLINE_ASM_H
#define SAD_LLVM_INLINE_ASM_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/InlineAsm.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace sad {
namespace codegen {

/**
 * معلومات معامل التجميع
 */
struct AsmOperand {
    std::string constraint;      // قيد LLVM (مثل "=r", "r", "m")
    llvm::Value* value = nullptr; // القيمة المرتبطة
    llvm::Type* type = nullptr;  // نوع المعامل
    bool isOutput = false;       // هل هو مخرج؟
};

/**
 * وصف تعليمة تجميع عربية مُترجمة
 */
struct ArabicAsmMapping {
    std::string x86_64;          // تعليمة x86_64
    std::string aarch64;         // تعليمة ARM64 (اختياري)
    bool hasSideEffects;         // تأثيرات جانبية
    bool isVolatile;             // متطاير
};

/**
 * مُولّد تعليمات التجميع المُضمّنة عبر LLVM API
 */
class LLVMInlineAsm {
public:
    explicit LLVMInlineAsm(llvm::Module& module);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    تعليمات التجميع الأساسية
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * توليد تجميع عام
     * تجميع! { "تعليمة" : مخرجات : مدخلات : متلفات }
     */
    llvm::CallInst* emitInlineAsm(
        llvm::IRBuilder<>& builder,
        const std::string& asmStr,
        const std::vector<AsmOperand>& operands,
        bool hasSideEffects = true,
        bool isAlignStack = false);
    
    /**
     * توليد تعليمة بسيطة بدون معاملات
     * تجميع! { "cli" }
     */
    llvm::CallInst* emitSimpleAsm(
        llvm::IRBuilder<>& builder,
        const std::string& asmStr,
        bool hasSideEffects = true);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    تعليمات عربية مُعرّفة مسبقاً
    // ═══════════════════════════════════════════════════════════════════════
    
    /** توقف! → hlt */
    llvm::CallInst* emitHalt(llvm::IRBuilder<>& builder);
    
    /** عطّل_المقاطعات! → cli */
    llvm::CallInst* emitDisableInterrupts(llvm::IRBuilder<>& builder);
    
    /** فعّل_المقاطعات! → sti */
    llvm::CallInst* emitEnableInterrupts(llvm::IRBuilder<>& builder);
    
    /** لا_عملية! → nop */
    llvm::CallInst* emitNop(llvm::IRBuilder<>& builder);
    
    /** انتظر_مقاطعة! → pause (x86) */
    llvm::CallInst* emitPause(llvm::IRBuilder<>& builder);
    
    /** أبطل_tlb!(عنوان) → invlpg [addr] */
    llvm::CallInst* emitInvalidateTLB(llvm::IRBuilder<>& builder,
                                       llvm::Value* address);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    دعم المعمارية
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * ترجمة تعليمة عربية إلى assembly حقيقي
     * توقف → hlt (x86_64)
     */
    std::string translateArabicMnemonic(const std::string& arabic) const;
    
    /**
     * فلترة حسب المعمارية
     * #[معمارية("x86_64")] تجميع! { ... }
     */
    void setTargetArch(const std::string& arch) { m_targetArch = arch; }
    const std::string& getTargetArch() const { return m_targetArch; }

private:
    llvm::Module& m_module;
    llvm::LLVMContext& m_ctx;
    std::string m_targetArch = "x86_64";
    
    // خريطة التعليمات العربية ← المعمارية المحددة
    static const std::unordered_map<std::string, ArabicAsmMapping> s_arabicMnemonics;
    
    // بناء سلسلة القيود من المعاملات
    std::string buildConstraintString(const std::vector<AsmOperand>& operands) const;
    
    // بناء نوع الدالة من المعاملات
    llvm::FunctionType* buildAsmFuncType(const std::vector<AsmOperand>& operands) const;
};

} // namespace codegen
} // namespace sad

#endif // SAD_LLVM_INLINE_ASM_H
