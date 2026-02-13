// ======================================================================
// kernel_sync.h - بدائيات مزامنة النواة / Kernel Synchronization Primitives
// ======================================================================
// الوصف بالعربية:
//   بدائيات المزامنة لنظام النحلة — spinlock, mutex, semaphore
//   مصممة للعمل في وضع النواة بدون مكتبة قياسية
//
// English Description:
//   Kernel synchronization primitives for BeeOS
//   Designed for kernel mode without standard library
// ======================================================================

#ifndef SAD_KERNEL_SYNC_H
#define SAD_KERNEL_SYNC_H

#include <string>
#include <vector>

namespace Sad {
namespace Kernel {

/**
 * @brief Sync primitive types / أنواع بدائيات المزامنة
 */
enum class SyncPrimitive {
    SpinLock,       ///< قفل_دوراني / spinlock — busy-wait lock
    Mutex,          ///< قفل / mutex — mutual exclusion lock
    RwLock,         ///< قفل_قراءة_كتابة / rwlock — reader-writer lock
    Semaphore,      ///< إشارة / semaphore — counting semaphore
    Barrier,        ///< حاجز / barrier — synchronization barrier
    CondVar,        ///< شرط / condvar — condition variable
    Once            ///< مرة / once — one-time initialization
};

/**
 * @brief Spinlock implementation info / معلومات تنفيذ القفل الدوراني
 * 
 * (AR) قفل دوراني على مستوى النواة يستخدم عمليات ذرية
 *      مناسب للأقسام الحرجة القصيرة في النواة
 * (EN) Kernel-level spinlock using atomic operations
 *      Suitable for short critical sections in kernel
 * 
 * @example صيغة ص:
 * - دع قفل = قفل_دوراني::جديد()
 * - قفل.أقفل()
 * - // ... القسم الحرج ...
 * - قفل.افتح()
 * 
 * أو باستخدام RAII:
 * - باستخدام حارس = قفل.احرس() {
 * -     // ... القسم الحرج ...
 * - }
 */
struct SpinLockInfo {
    bool interruptSafe;     ///< Disables interrupts? / يعطل المقاطعات؟
    bool isTicket;          ///< Ticket spinlock? / قفل تذكرة؟ (fair ordering)
    
    SpinLockInfo(bool intSafe = false, bool ticket = false)
        : interruptSafe(intSafe), isTicket(ticket) {}
};

/**
 * @brief Mutex implementation info / معلومات تنفيذ القفل
 * 
 * (AR) قفل يسمح بانتقال الخيط للنوم عند الانتظار
 * (EN) Lock that allows thread to sleep while waiting
 */
struct MutexInfo {
    bool isRecursive;       ///< Recursive mutex? / قفل تكراري؟
    bool hasPriorityInherit;///< Priority inheritance? / وراثة الأولوية؟
    
    MutexInfo(bool recursive = false, bool priority = false)
        : isRecursive(recursive), hasPriorityInherit(priority) {}
};

/**
 * @brief RwLock implementation info / معلومات قفل القراءة/الكتابة
 * 
 * (AR) يسمح بقراءات متعددة متزامنة لكن كتابة واحدة فقط
 * (EN) Allows multiple concurrent reads but only one write
 */
struct RwLockInfo {
    bool writerPriority;    ///< Writer priority? / أولوية الكتابة؟
    
    RwLockInfo(bool wp = true) : writerPriority(wp) {}
};

/**
 * @brief Get Arabic name for sync primitive
 * (AR) الحصول على الاسم العربي لبدائية المزامنة
 */
inline std::string syncPrimitiveArabicName(SyncPrimitive prim) {
    switch (prim) {
        case SyncPrimitive::SpinLock:   return "قفل_دوراني";
        case SyncPrimitive::Mutex:      return "قفل";
        case SyncPrimitive::RwLock:     return "قفل_قراءة_كتابة";
        case SyncPrimitive::Semaphore:  return "إشارة";
        case SyncPrimitive::Barrier:    return "حاجز";
        case SyncPrimitive::CondVar:    return "شرط";
        case SyncPrimitive::Once:       return "مرة";
    }
    return "مجهول";
}

/**
 * @brief Memory barrier types / أنواع حواجز الذاكرة
 * 
 * (AR) حواجز الذاكرة للتحكم في ترتيب العمليات على مستوى المعالج
 * (EN) Memory barriers for controlling operation ordering at CPU level
 */
enum class MemoryBarrier {
    Full,           ///< كامل / full — full memory barrier (mfence)
    LoadLoad,       ///< قراءة_قراءة / load-load barrier
    StoreStore,     ///< كتابة_كتابة / store-store barrier
    LoadStore,      ///< قراءة_كتابة / load-store barrier
    Compiler        ///< مترجم / compiler — compiler-only barrier (no CPU instruction)
};

/**
 * @brief Get x86_64 instruction for memory barrier
 */
inline std::string barrierToX86(MemoryBarrier barrier) {
    switch (barrier) {
        case MemoryBarrier::Full:        return "mfence";
        case MemoryBarrier::LoadLoad:    return "lfence";
        case MemoryBarrier::StoreStore:  return "sfence";
        case MemoryBarrier::LoadStore:   return "mfence";
        case MemoryBarrier::Compiler:    return ""; // compiler barrier only
    }
    return "mfence";
}

} // namespace Kernel
} // namespace Sad

#endif // SAD_KERNEL_SYNC_H
