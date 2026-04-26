// ======================================================================
// llvm_kernel_sync.cpp - توليد كود مزامنة النواة / Kernel Sync Codegen
// ======================================================================
// الوصف بالعربية:
//   توليد LLVM IR لبدائيات المزامنة المستخدمة في النواة
//   يتضمن: spinlock, ticket lock, memory barriers
//
// English Description:
//   LLVM IR generation for kernel synchronization primitives
//   Includes: spinlock, ticket lock, memory barriers
// ======================================================================

#include "kernel/kernel_sync.h"
#include "types/atomic_types.h"
#include <string>
#include <sstream>

namespace Sad {
namespace Backend {

/**
 * @brief Generate LLVM IR for kernel sync primitives
 * (AR) توليد LLVM IR لبدائيات مزامنة النواة
 */
class LLVMKernelSyncCodegen {
public:
    /**
     * @brief Generate spinlock struct type / توليد نوع بنية القفل الدوراني
     * 
     * (AR) بنية القفل الدوراني: { i32 locked }
     *      0 = مفتوح، 1 = مقفل
     */
    static std::string generateSpinlockType() {
        return "%spinlock = type { i32 }\n";
    }
    
    /**
     * @brief Generate spinlock init / توليد تهيئة القفل الدوراني
     * 
     * @example ص: دع قفل = قفل_دوراني::جديد()
     */
    static std::string generateSpinlockInit(const std::string& ptrVar) {
        std::ostringstream ir;
        ir << "; تهيئة قفل دوراني — spinlock init\n";
        ir << "%lock_field = getelementptr %spinlock, %spinlock* " << ptrVar 
           << ", i32 0, i32 0\n";
        ir << "store i32 0, i32* %lock_field\n";
        return ir.str();
    }
    
    /**
     * @brief Generate spinlock acquire / توليد قفل (acquire)
     * 
     * (AR) حلقة CAS: حاول تبديل 0→1 حتى ينجح
     * (EN) CAS loop: try to swap 0→1 until success
     * 
     * @example ص: قفل.أقفل()
     * 
     * LLVM IR:
     *   loop:
     *     %old = cmpxchg i32* %lock, i32 0, i32 1 acquire monotonic
     *     %success = extractvalue {i32, i1} %old, 1
     *     br i1 %success, label %acquired, label %spin
     *   spin:
     *     ; pause instruction for CPU hint
     *     call void @llvm.x86.sse2.pause()
     *     ; read lock value (relaxed) to avoid cache line bouncing
     *     %val = load atomic i32, i32* %lock monotonic, align 4
     *     %free = icmp eq i32 %val, 0
     *     br i1 %free, label %loop, label %spin
     */
    static std::string generateSpinlockAcquire(const std::string& ptrVar,
                                                 const std::string& labelPrefix) {
        std::ostringstream ir;
        ir << "; قفل دوراني — spinlock acquire\n";
        ir << "br label %" << labelPrefix << "_loop\n\n";
        
        ir << labelPrefix << "_loop:\n";
        ir << "  %lock_ptr = getelementptr %spinlock, %spinlock* " << ptrVar 
           << ", i32 0, i32 0\n";
        ir << "  %cas_result = cmpxchg i32* %lock_ptr, i32 0, i32 1 acquire monotonic\n";
        ir << "  %success = extractvalue {i32, i1} %cas_result, 1\n";
        ir << "  br i1 %success, label %" << labelPrefix << "_acquired, label %" 
           << labelPrefix << "_spin\n\n";
        
        ir << labelPrefix << "_spin:\n";
        ir << "  call void @llvm.x86.sse2.pause()\n";
        ir << "  %spin_val = load atomic i32, i32* %lock_ptr monotonic, align 4\n";
        ir << "  %is_free = icmp eq i32 %spin_val, 0\n";
        ir << "  br i1 %is_free, label %" << labelPrefix << "_loop, label %" 
           << labelPrefix << "_spin\n\n";
        
        ir << labelPrefix << "_acquired:\n";
        return ir.str();
    }
    
    /**
     * @brief Generate spinlock release / توليد فتح القفل (release)
     * 
     * @example ص: قفل.افتح()
     * LLVM: store atomic i32 0, i32* %lock release, align 4
     */
    static std::string generateSpinlockRelease(const std::string& ptrVar) {
        std::ostringstream ir;
        ir << "; فتح قفل دوراني — spinlock release\n";
        ir << "%lock_field_rel = getelementptr %spinlock, %spinlock* " << ptrVar 
           << ", i32 0, i32 0\n";
        ir << "store atomic i32 0, i32* %lock_field_rel release, align 4\n";
        return ir.str();
    }
    
    /**
     * @brief Generate ticket lock type / توليد نوع قفل التذكرة
     * 
     * (AR) قفل تذكرة عادل: { i32 next_ticket, i32 now_serving }
     */
    static std::string generateTicketLockType() {
        return "%ticket_lock = type { i32, i32 }\n";
    }
    
    /**
     * @brief Generate ticket lock acquire
     * (AR) يأخذ تذكرة ذرياً وينتظر حتى يحين دوره
     */
    static std::string generateTicketLockAcquire(const std::string& ptrVar,
                                                   const std::string& labelPrefix) {
        std::ostringstream ir;
        ir << "; قفل تذكرة — ticket lock acquire\n";
        ir << "%next_ptr = getelementptr %ticket_lock, %ticket_lock* " << ptrVar 
           << ", i32 0, i32 0\n";
        ir << "%my_ticket = atomicrmw add i32* %next_ptr, i32 1 acquire\n";
        ir << "br label %" << labelPrefix << "_wait\n\n";
        
        ir << labelPrefix << "_wait:\n";
        ir << "  %serving_ptr = getelementptr %ticket_lock, %ticket_lock* " << ptrVar 
           << ", i32 0, i32 1\n";
        ir << "  %current = load atomic i32, i32* %serving_ptr acquire, align 4\n";
        ir << "  %my_turn = icmp eq i32 %current, %my_ticket\n";
        ir << "  br i1 %my_turn, label %" << labelPrefix << "_got_lock, label %" 
           << labelPrefix << "_pause\n\n";
        
        ir << labelPrefix << "_pause:\n";
        ir << "  call void @llvm.x86.sse2.pause()\n";
        ir << "  br label %" << labelPrefix << "_wait\n\n";
        
        ir << labelPrefix << "_got_lock:\n";
        return ir.str();
    }
    
    /**
     * @brief Generate ticket lock release
     */
    static std::string generateTicketLockRelease(const std::string& ptrVar) {
        std::ostringstream ir;
        ir << "; فتح قفل تذكرة — ticket lock release\n";
        ir << "%serving = getelementptr %ticket_lock, %ticket_lock* " << ptrVar 
           << ", i32 0, i32 1\n";
        ir << "%cur_serving = load atomic i32, i32* %serving monotonic, align 4\n";
        ir << "%next_serving = add i32 %cur_serving, 1\n";
        ir << "store atomic i32 %next_serving, i32* %serving release, align 4\n";
        return ir.str();
    }
    
    /**
     * @brief Generate memory barrier / توليد حاجز ذاكرة
     * 
     * @example ص: حاجز_ذاكرة(كامل)
     */
    static std::string generateMemoryBarrier(Kernel::MemoryBarrier type) {
        std::ostringstream ir;
        ir << "; حاجز ذاكرة — memory barrier\n";
        
        switch (type) {
            case Kernel::MemoryBarrier::Full:
                ir << "fence seq_cst\n";
                break;
            case Kernel::MemoryBarrier::LoadLoad:
                ir << "fence acquire\n";
                break;
            case Kernel::MemoryBarrier::StoreStore:
                ir << "fence release\n";
                break;
            case Kernel::MemoryBarrier::LoadStore:
                ir << "fence acq_rel\n";
                break;
            case Kernel::MemoryBarrier::Compiler:
                ir << "; compiler barrier only — no instruction emitted\n";
                ir << "fence syncscope(\"singlethread\") seq_cst\n";
                break;
        }
        
        return ir.str();
    }
    
    /**
     * @brief Generate interrupt disable/enable
     * (AR) تعطيل/تفعيل المقاطعات (x86_64)
     */
    static std::string generateInterruptDisable() {
        return "call void asm sideeffect \"cli\", \"\"()\n";
    }
    
    static std::string generateInterruptEnable() {
        return "call void asm sideeffect \"sti\", \"\"()\n";
    }
};

} // namespace Backend
} // namespace Sad
