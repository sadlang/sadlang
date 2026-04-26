/**
 * =============================================================================
 * ملف: llvm_kernel_sync.h
 * الوصف: توليد بدائيات المزامنة للنواة عبر LLVM IRBuilder
 * المهمة: م-د04 - دمج LLVMKernelSyncCodegen في خط الأنابيب
 * =============================================================================
 *
 * يُولّد:
 *   - قفل دوراني (spinlock) مع CAS loop + pause hint
 *   - قفل تذكرة (ticket lock) مع fetch_add
 *   - حواجز ذاكرة (memory fences) بكل الأنواع
 *   - عمليات ذرية (atomic operations)
 * =============================================================================
 */

#ifndef SAD_LLVM_KERNEL_SYNC_H
#define SAD_LLVM_KERNEL_SYNC_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <string>

namespace sad {
namespace codegen {

/**
 * مولّد بدائيات المزامنة عبر LLVM IRBuilder
 */
class LLVMKernelSync {
public:
    explicit LLVMKernelSync(llvm::Module& module);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    القفل الدوراني (Spinlock)
    // ═══════════════════════════════════════════════════════════════════════
    
    /** توليد نوع القفل الدوراني: { i32 } */
    llvm::StructType* getSpinlockType();
    
    /** توليد spinlock_init: store 0 */
    void emitSpinlockInit(llvm::IRBuilder<>& builder, llvm::Value* lockPtr);
    
    /**
     * توليد spinlock_acquire: CAS loop مع pause hint
     * قفل_دوراني.اقفل()
     */
    void emitSpinlockAcquire(llvm::IRBuilder<>& builder,
                              llvm::Value* lockPtr,
                              llvm::Function* parentFunc);
    
    /**
     * توليد spinlock_release: store 0 مع release ordering
     * قفل_دوراني.حرر()
     */
    void emitSpinlockRelease(llvm::IRBuilder<>& builder, llvm::Value* lockPtr);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    قفل التذكرة (Ticket Lock)
    // ═══════════════════════════════════════════════════════════════════════
    
    /** نوع قفل التذكرة: { i32 next_ticket, i32 now_serving } */
    llvm::StructType* getTicketLockType();
    
    /** تهيئة: store {0, 0} */
    void emitTicketLockInit(llvm::IRBuilder<>& builder, llvm::Value* lockPtr);
    
    /** قفل: fetch_add(next_ticket) ثم انتظار حتى now_serving يطابق */
    void emitTicketLockAcquire(llvm::IRBuilder<>& builder,
                                llvm::Value* lockPtr,
                                llvm::Function* parentFunc);
    
    /** تحرير: fetch_add(now_serving, 1) */
    void emitTicketLockRelease(llvm::IRBuilder<>& builder, llvm::Value* lockPtr);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    حواجز الذاكرة
    // ═══════════════════════════════════════════════════════════════════════
    
    /** حاجز_ذاكرة(متسلسل) → fence seq_cst */
    void emitFenceSeqCst(llvm::IRBuilder<>& builder);
    
    /** حاجز_ذاكرة(اكتساب) → fence acquire */
    void emitFenceAcquire(llvm::IRBuilder<>& builder);
    
    /** حاجز_ذاكرة(تحرير) → fence release */
    void emitFenceRelease(llvm::IRBuilder<>& builder);
    
    /** حاجز_ذاكرة(اكتساب_تحرير) → fence acq_rel */
    void emitFenceAcqRel(llvm::IRBuilder<>& builder);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    عمليات ذرية
    // ═══════════════════════════════════════════════════════════════════════
    
    /** قارن_وبدّل(ptr, expected, desired) → cmpxchg */
    llvm::Value* emitCompareAndSwap(llvm::IRBuilder<>& builder,
                                     llvm::Value* ptr,
                                     llvm::Value* expected,
                                     llvm::Value* desired);
    
    /** جلب_وأضف(ptr, val) → atomicrmw add */
    llvm::Value* emitFetchAdd(llvm::IRBuilder<>& builder,
                               llvm::Value* ptr,
                               llvm::Value* val);
    
    /** تحميل_ذري(ptr) → load atomic */
    llvm::Value* emitAtomicLoad(llvm::IRBuilder<>& builder,
                                 llvm::Value* ptr);
    
    /** تخزين_ذري(ptr, val) → store atomic */
    void emitAtomicStore(llvm::IRBuilder<>& builder,
                          llvm::Value* ptr,
                          llvm::Value* val);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    تعطيل/تفعيل المقاطعات
    // ═══════════════════════════════════════════════════════════════════════
    
    /** عطّل_المقاطعات() → cli */
    void emitDisableInterrupts(llvm::IRBuilder<>& builder);
    
    /** فعّل_المقاطعات() → sti */
    void emitEnableInterrupts(llvm::IRBuilder<>& builder);

private:
    llvm::Module& m_module;
    llvm::LLVMContext& m_ctx;
    
    llvm::StructType* m_spinlockType = nullptr;
    llvm::StructType* m_ticketLockType = nullptr;
    
    // مُعرّف فريد لتسمية الكتل
    unsigned m_labelCounter = 0;
    std::string nextLabel(const std::string& prefix);
};

} // namespace codegen
} // namespace sad

#endif // SAD_LLVM_KERNEL_SYNC_H
