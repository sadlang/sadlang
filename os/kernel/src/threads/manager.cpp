// ===================================================================
// مدير الخيوط - صدOS
// manager.cpp - Thread Management
// إدارة إنشاء وتدمير الخيوط مع التخزين المحلي للخيط
// المرحلة 52 - مشروع لغة صاد
// ===================================================================

#include <cstdint>
#include <cstddef>
#include <array>

namespace sad::os::kernel::threads {

// === ثوابت الخيوط ===
static constexpr size_t MAX_THREADS          = 8192;        // الحد الأقصى للخيوط
static constexpr size_t DEFAULT_STACK_SIZE   = 65536;       // حجم المكدس الافتراضي (64 كيلوبايت)
static constexpr size_t MAX_TLS_SLOTS        = 128;         // الحد الأقصى لفتحات TLS
static constexpr size_t GUARD_PAGE_SIZE      = 4096;        // صفحة حماية المكدس

// === حالة الخيط ===
enum class ThreadState : uint8_t {
    Created,     // تم إنشاؤه (لم يبدأ بعد)
    Running,     // يعمل حالياً
    Ready,       // جاهز للتشغيل
    Blocked,     // محجوب (ينتظر)
    Suspended,   // معلق
    Terminated,  // انتهى
    Detached     // منفصل (لا يمكن الانضمام إليه)
};

// === سجلات سياق الخيط (x86_64) ===
struct ThreadContext {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip;              // مؤشر التعليمة
    uint64_t rflags;           // سجل الأعلام
    uint64_t cr3;              // جدول الصفحات
    uint64_t fs_base;          // قاعدة TLS
};

// === التخزين المحلي للخيط ===
struct ThreadLocalStorage {
    uint64_t slots[MAX_TLS_SLOTS];  // فتحات التخزين المحلي
    uint32_t used_count;             // عدد الفتحات المستخدمة
    uint32_t thread_id;              // معرف الخيط المالك

    void clear() {
        for (auto& s : slots) s = 0;
        used_count = 0;
    }
};

// === كتلة التحكم بالخيط ===
struct ThreadControlBlock {
    uint32_t tid;              // معرف الخيط
    uint32_t pid;              // معرف العملية الأم
    ThreadState state;
    uint8_t priority;          // الأولوية (0-255)

    ThreadContext context;      // سياق المعالج
    ThreadLocalStorage tls;     // التخزين المحلي

    uint64_t stack_base;        // قاعدة المكدس
    uint64_t stack_size;        // حجم المكدس
    uint64_t entry_point;       // نقطة الدخول
    uint64_t argument;          // وسيطة دالة الخيط

    int32_t exit_code;          // رمز الخروج
    uint32_t join_waiter;       // خيط ينتظر الانضمام (-1 = لا أحد)
    bool is_joinable;           // هل يمكن الانضمام إليه؟
};

// === مدير الخيوط ===
class ThreadManager {
public:
    // تهيئة مدير الخيوط
    bool initialize() {
        thread_count_ = 0;
        next_tid_ = 1;
        return true;
    }

    // إنشاء خيط جديد
    int32_t create_thread(uint32_t pid, uint64_t entry_point,
                          uint64_t argument, uint64_t stack_size = DEFAULT_STACK_SIZE) {
        if (thread_count_ >= MAX_THREADS) return -1;

        auto& tcb = threads_[thread_count_];
        tcb.tid = next_tid_++;
        tcb.pid = pid;
        tcb.state = ThreadState::Created;
        tcb.priority = 128;  // أولوية متوسطة
        tcb.entry_point = entry_point;
        tcb.argument = argument;
        tcb.stack_size = stack_size;
        tcb.exit_code = 0;
        tcb.join_waiter = UINT32_MAX;
        tcb.is_joinable = true;

        // تخصيص المكدس مع صفحة حماية
        tcb.stack_base = allocate_stack(stack_size);
        if (tcb.stack_base == 0) return -1;

        // تهيئة سياق المعالج
        setup_initial_context(tcb);

        // تهيئة التخزين المحلي
        tcb.tls.clear();
        tcb.tls.thread_id = tcb.tid;

        tcb.state = ThreadState::Ready;
        thread_count_++;
        return static_cast<int32_t>(tcb.tid);
    }

    // إنهاء الخيط الحالي
    void exit_thread(uint32_t tid, int32_t exit_code) {
        auto* tcb = find_thread(tid);
        if (!tcb) return;

        tcb->exit_code = exit_code;
        tcb->state = ThreadState::Terminated;

        // إيقاظ الخيط المنتظر (إن وجد)
        if (tcb->join_waiter != UINT32_MAX) {
            auto* waiter = find_thread(tcb->join_waiter);
            if (waiter && waiter->state == ThreadState::Blocked) {
                waiter->state = ThreadState::Ready;
            }
        }

        // تحرير الموارد إذا كان منفصلاً
        if (!tcb->is_joinable) {
            free_stack(tcb->stack_base, tcb->stack_size);
        }
    }

    // انتظار انتهاء خيط (الانضمام)
    int32_t join_thread(uint32_t caller_tid, uint32_t target_tid) {
        auto* target = find_thread(target_tid);
        if (!target || !target->is_joinable) return -1;

        if (target->state == ThreadState::Terminated) {
            int32_t code = target->exit_code;
            cleanup_thread(target_tid);
            return code;
        }

        // حجب الخيط المستدعي حتى ينتهي الهدف
        target->join_waiter = caller_tid;
        auto* caller = find_thread(caller_tid);
        if (caller) caller->state = ThreadState::Blocked;

        return 0; // سيُستأنف لاحقاً
    }

    // فصل الخيط (لا يمكن الانضمام إليه بعد ذلك)
    bool detach_thread(uint32_t tid) {
        auto* tcb = find_thread(tid);
        if (!tcb || !tcb->is_joinable) return false;

        tcb->is_joinable = false;
        tcb->state = ThreadState::Detached;

        // تنظيف فوري إذا كان منتهياً
        if (tcb->state == ThreadState::Terminated) {
            cleanup_thread(tid);
        }
        return true;
    }

    // التنازل عن المعالج طواعية
    void yield(uint32_t tid) {
        auto* tcb = find_thread(tid);
        if (tcb && tcb->state == ThreadState::Running) {
            tcb->state = ThreadState::Ready;
            // المجدول سيختار المهمة التالية
        }
    }

    // تخصيص فتحة تخزين محلي
    int32_t tls_alloc(uint32_t tid) {
        auto* tcb = find_thread(tid);
        if (!tcb || tcb->tls.used_count >= MAX_TLS_SLOTS) return -1;
        return tcb->tls.used_count++;
    }

    // قراءة/كتابة التخزين المحلي
    bool tls_set(uint32_t tid, uint32_t slot, uint64_t value) {
        auto* tcb = find_thread(tid);
        if (!tcb || slot >= tcb->tls.used_count) return false;
        tcb->tls.slots[slot] = value;
        return true;
    }

    uint64_t tls_get(uint32_t tid, uint32_t slot) {
        auto* tcb = find_thread(tid);
        if (!tcb || slot >= tcb->tls.used_count) return 0;
        return tcb->tls.slots[slot];
    }

    uint32_t active_threads() const { return thread_count_; }

private:
    std::array<ThreadControlBlock, MAX_THREADS> threads_;
    uint32_t thread_count_ = 0;
    uint32_t next_tid_ = 1;

    ThreadControlBlock* find_thread(uint32_t tid) {
        for (uint32_t i = 0; i < thread_count_; i++) {
            if (threads_[i].tid == tid) return &threads_[i];
        }
        return nullptr;
    }

    void setup_initial_context(ThreadControlBlock& tcb) {
        auto& ctx = tcb.context;
        ctx = {};
        ctx.rip = tcb.entry_point;
        ctx.rsp = tcb.stack_base + tcb.stack_size - 8; // أعلى المكدس
        ctx.rdi = tcb.argument;                         // الوسيطة الأولى
        ctx.rflags = 0x202;                             // تفعيل المقاطعات
    }

    uint64_t allocate_stack(uint64_t size) {
        // تخصيص مع صفحة حماية في الأسفل
        return 0x00007F0000000000ULL; // عنوان مبسط
    }

    void free_stack(uint64_t base, uint64_t size) { /* تحرير ذاكرة المكدس */ }

    void cleanup_thread(uint32_t tid) {
        auto* tcb = find_thread(tid);
        if (tcb) {
            free_stack(tcb->stack_base, tcb->stack_size);
        }
    }
};

} // namespace sad::os::kernel::threads
