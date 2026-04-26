/**
 * =============================================================================
 * ملف: llvm_cpu_regs.h
 * الوصف: توليد كود قراءة/كتابة سجلات المعالج عبر LLVM IRBuilder
 * المهمة: م-د06 - إدارة سجلات المعالج (CR0-CR4, MSRs)
 * =============================================================================
 *
 * يدعم:
 *   - سجلات التحكم: CR0, CR2, CR3, CR4
 *   - سجلات MSR: EFER, APIC_BASE, FS_BASE, GS_BASE, ...
 *   - سجل EFLAGS/RFLAGS
 *   - تعليمات خاصة: cpuid, rdtsc, wbinvd
 * =============================================================================
 */

#ifndef SAD_LLVM_CPU_REGS_H
#define SAD_LLVM_CPU_REGS_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <string>

namespace sad {
namespace codegen {

/**
 * أرقام MSR الشائعة
 */
namespace MSR {
    constexpr uint32_t IA32_EFER       = 0xC0000080;
    constexpr uint32_t IA32_STAR       = 0xC0000081;
    constexpr uint32_t IA32_LSTAR      = 0xC0000082;
    constexpr uint32_t IA32_FMASK      = 0xC0000084;
    constexpr uint32_t IA32_FS_BASE    = 0xC0000100;
    constexpr uint32_t IA32_GS_BASE    = 0xC0000101;
    constexpr uint32_t IA32_APIC_BASE  = 0x0000001B;
    constexpr uint32_t IA32_PAT        = 0x00000277;
}

/**
 * مولّد كود سجلات المعالج
 */
class LLVMCpuRegs {
public:
    explicit LLVMCpuRegs(llvm::Module& module);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    سجلات التحكم (Control Registers)
    // ═══════════════════════════════════════════════════════════════════════
    
    /** قراءة CR0: mov rax, cr0 */
    llvm::Value* emitReadCR0(llvm::IRBuilder<>& builder);
    
    /** كتابة CR0: mov cr0, rax */
    void emitWriteCR0(llvm::IRBuilder<>& builder, llvm::Value* value);
    
    /** قراءة CR2 (عنوان خطأ الصفحة): mov rax, cr2 */
    llvm::Value* emitReadCR2(llvm::IRBuilder<>& builder);
    
    /** قراءة CR3 (دليل جداول الصفحات): mov rax, cr3 */
    llvm::Value* emitReadCR3(llvm::IRBuilder<>& builder);
    
    /** كتابة CR3 (تغيير جداول الصفحات): mov cr3, rax */
    void emitWriteCR3(llvm::IRBuilder<>& builder, llvm::Value* value);
    
    /** قراءة CR4: mov rax, cr4 */
    llvm::Value* emitReadCR4(llvm::IRBuilder<>& builder);
    
    /** كتابة CR4: mov cr4, rax */
    void emitWriteCR4(llvm::IRBuilder<>& builder, llvm::Value* value);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    MSR (Model-Specific Registers)
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * قراءة MSR: rdmsr
     * ECX = رقم المسجل → EDX:EAX = القيمة (64-بت)
     */
    llvm::Value* emitReadMSR(llvm::IRBuilder<>& builder, uint32_t msrNumber);
    llvm::Value* emitReadMSR(llvm::IRBuilder<>& builder, llvm::Value* msrNumber);
    
    /**
     * كتابة MSR: wrmsr
     * ECX = رقم المسجل, EDX:EAX = القيمة
     */
    void emitWriteMSR(llvm::IRBuilder<>& builder, uint32_t msrNumber,
                       llvm::Value* value);
    void emitWriteMSR(llvm::IRBuilder<>& builder, llvm::Value* msrNumber,
                       llvm::Value* value);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    RFLAGS
    // ═══════════════════════════════════════════════════════════════════════
    
    /** قراءة RFLAGS: pushfq; pop rax */
    llvm::Value* emitReadFlags(llvm::IRBuilder<>& builder);
    
    /** كتابة RFLAGS: push rax; popfq */
    void emitWriteFlags(llvm::IRBuilder<>& builder, llvm::Value* value);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    تعليمات خاصة
    // ═══════════════════════════════════════════════════════════════════════
    
    /** rdtsc — قراءة عداد الأختام الزمنية */
    llvm::Value* emitRDTSC(llvm::IRBuilder<>& builder);
    
    /** cpuid — الاستعلام عن إمكانيات المعالج */
    llvm::Value* emitCPUID(llvm::IRBuilder<>& builder, llvm::Value* leaf);
    
    /** wbinvd — تنظيف وتعطيل ذاكرة التخزين المؤقت */
    void emitWBINVD(llvm::IRBuilder<>& builder);
    
    /** invlpg — إبطال صفحة في TLB */
    void emitINVLPG(llvm::IRBuilder<>& builder, llvm::Value* address);

private:
    llvm::Module& m_module;
    llvm::LLVMContext& m_ctx;
    
    // مساعد: توليد inline asm لقراءة سجل تحكم
    llvm::Value* emitReadCR(llvm::IRBuilder<>& builder, unsigned crNum);
    // مساعد: توليد inline asm لكتابة سجل تحكم
    void emitWriteCR(llvm::IRBuilder<>& builder, unsigned crNum,
                      llvm::Value* value);
};

} // namespace codegen
} // namespace sad

#endif // SAD_LLVM_CPU_REGS_H
