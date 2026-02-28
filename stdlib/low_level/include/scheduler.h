/*
 * ============================================================================
 * وحدة الجدولة والعمليات - لغة ص
 * Scheduler & Process Management Module - Sad Language
 * ============================================================================
 *
 * @brief (AR) إدارة العمليات والخيوط وجدولة المعالج
 *              دعم تبديل السياق، الأولويات، حالات العملية
 *
 * @brief (EN) Process and thread management, CPU scheduling
 *              Context switching, priorities, process states support
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_SCHEDULER_H
#define SAD_LOW_LEVEL_SCHEDULER_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <functional>

namespace Sad {
namespace LowLevel {

// ============================================================================
// حالات العملية والخيط / Process & Thread States
// ============================================================================

/// @brief (AR) حالة العملية / (EN) Process state
enum class ProcessState : uint8_t {
    CREATED,        // تم إنشاؤها / Created
    READY,          // جاهزة / Ready to run
    RUNNING,        // قيد التنفيذ / Currently running
    BLOCKED,        // محجوبة / Blocked (waiting for I/O, etc.)
    SLEEPING,       // نائمة / Sleeping (timed wait)
    ZOMBIE,         // ميتة (بانتظار الأب) / Dead (waiting for parent)
    TERMINATED      // منتهية / Terminated
};

/// @brief (AR) أولوية الجدولة / (EN) Scheduling priority
enum class Priority : uint8_t {
    IDLE       = 0,     // خاملة / Idle
    LOW        = 1,     // منخفضة / Low
    NORMAL     = 2,     // عادية / Normal
    HIGH       = 3,     // عالية / High
    REALTIME   = 4      // فورية / Realtime
};

// ============================================================================
// سياق المعالج / CPU Context
// ============================================================================

/// @brief (AR) سياق سجلات المعالج (x86-64) / (EN) CPU Register Context (x86-64)
#pragma pack(push, 1)
struct CPUContext {
    // سجلات للأغراض العامة / General purpose registers
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

    // سجلات يدفعها المعالج عند المقاطعة / CPU-pushed on interrupt
    uint64_t rip;           // مؤشر التعليمة / Instruction pointer
    uint64_t cs;            // مقطع الكود / Code segment
    uint64_t rflags;        // أعلام المعالج / CPU flags
    uint64_t rsp;           // مؤشر المكدس / Stack pointer
    uint64_t ss;            // مقطع المكدس / Stack segment

    CPUContext() {
        r15 = r14 = r13 = r12 = r11 = r10 = r9 = r8 = 0;
        rbp = rdi = rsi = rdx = rcx = rbx = rax = 0;
        rip = cs = rflags = rsp = ss = 0;
    }
};

/// @brief (AR) سياق FPU/SSE / (EN) FPU/SSE Context
struct FPUContext {
    uint8_t data[512];      // منطقة FXSAVE / FXSAVE area (512 bytes)
};
#pragma pack(pop)

// ============================================================================
// كتلة تحكم الخيط / Thread Control Block
// ============================================================================

/// @brief (AR) كتلة تحكم الخيط / (EN) Thread Control Block
struct Thread {
    uint64_t id;                    // معرّف الخيط / Thread ID
    uint64_t processId;             // معرّف العملية الأم / Parent process ID
    char     name[64];              // اسم الخيط / Thread name
    ProcessState state;             // الحالة / State
    Priority priority;              // الأولوية / Priority

    CPUContext context;              // سياق المعالج / CPU context
    FPUContext fpuContext;           // سياق FPU / FPU context

    uint64_t kernelStackBase;       // قاعدة مكدس النواة / Kernel stack base
    uint64_t kernelStackSize;       // حجم مكدس النواة / Kernel stack size
    uint64_t userStackBase;         // قاعدة مكدس المستخدم / User stack base
    uint64_t userStackSize;         // حجم مكدس المستخدم / User stack size

    uint64_t sleepUntil;            // وقت الاستيقاظ (ticks) / Wake time (ticks)
    int32_t  exitCode;              // رمز الخروج / Exit code

    // إحصائيات / Statistics
    uint64_t totalTicks;            // مجموع الـ ticks / Total ticks consumed
    uint64_t creationTime;          // وقت الإنشاء / Creation time
    uint32_t contextSwitches;       // عدد تبديلات السياق / Context switch count

    Thread() : id(0), processId(0), state(ProcessState::CREATED),
               priority(Priority::NORMAL), kernelStackBase(0), kernelStackSize(0),
               userStackBase(0), userStackSize(0), sleepUntil(0), exitCode(0),
               totalTicks(0), creationTime(0), contextSwitches(0) {
        name[0] = '\0';
    }
};

// ============================================================================
// كتلة تحكم العملية / Process Control Block
// ============================================================================

/// @brief (AR) كتلة تحكم العملية / (EN) Process Control Block
struct Process {
    uint64_t id;                    // معرّف العملية / Process ID
    uint64_t parentId;              // معرّف الأب / Parent process ID
    char     name[128];             // اسم العملية / Process name
    ProcessState state;             // الحالة / State
    Priority priority;              // الأولوية الافتراضية / Default priority

    uint64_t pageTableRoot;         // جذر جدول الصفحات (CR3) / Page table root (CR3)
    uint64_t heapStart;             // بداية الكومة / Heap start
    uint64_t heapEnd;               // نهاية الكومة / Heap end

    std::vector<uint64_t> threadIds; // معرفات الخيوط / Thread IDs
    int32_t exitCode;               // رمز الخروج / Exit code

    // إحصائيات / Statistics
    uint64_t totalTicks;            // مجموع الـ ticks / Total ticks
    uint64_t creationTime;          // وقت الإنشاء / Creation time
    uint64_t memoryUsage;           // استهلاك الذاكرة / Memory usage

    Process() : id(0), parentId(0), state(ProcessState::CREATED),
                priority(Priority::NORMAL), pageTableRoot(0),
                heapStart(0), heapEnd(0), exitCode(0),
                totalTicks(0), creationTime(0), memoryUsage(0) {
        name[0] = '\0';
    }
};

// ============================================================================
// مدير الجدولة / Scheduler Manager
// ============================================================================

/**
 * @brief (AR) مدير الجدولة - جدولة Round-Robin مع أولويات
 *              إدارة العمليات والخيوط وتبديل السياق
 *
 * @brief (EN) Scheduler Manager - Priority-based Round-Robin scheduling
 *              Process/thread management and context switching
 */
class SchedulerManager {
public:
    /// @brief (AR) الحصول على المثيل الوحيد / (EN) Get singleton instance
    static SchedulerManager& getInstance();

    // ========================================================================
    // التهيئة / Initialization
    // ========================================================================

    /// @brief (AR) تهيئة المجدول / (EN) Initialize scheduler
    void initialize();

    /// @brief (AR) بدء الجدولة / (EN) Start scheduling
    void start();

    /// @brief (AR) إيقاف الجدولة / (EN) Stop scheduling
    void stop();

    // ========================================================================
    // إدارة العمليات / Process Management
    // ========================================================================

    /// @brief (AR) إنشاء عملية / (EN) Create process
    uint64_t createProcess(const char* name, Priority priority = Priority::NORMAL);

    /// @brief (AR) إنهاء عملية / (EN) Terminate process
    void terminateProcess(uint64_t processId, int32_t exitCode = 0);

    /// @brief (AR) الحصول على عملية / (EN) Get process
    Process* getProcess(uint64_t processId);

    /// @brief (AR) الحصول على العملية الحالية / (EN) Get current process
    Process* getCurrentProcess();

    /// @brief (AR) الحصول على جميع العمليات / (EN) Get all processes
    const std::vector<Process>& getProcesses() const { return processes_; }

    // ========================================================================
    // إدارة الخيوط / Thread Management
    // ========================================================================

    /// @brief (AR) إنشاء خيط / (EN) Create thread
    uint64_t createThread(uint64_t processId, const char* name,
                          uint64_t entryPoint, uint64_t arg = 0,
                          Priority priority = Priority::NORMAL,
                          uint64_t stackSize = 65536);

    /// @brief (AR) إنشاء خيط نواة / (EN) Create kernel thread
    uint64_t createKernelThread(const char* name, uint64_t entryPoint,
                                uint64_t arg = 0,
                                Priority priority = Priority::NORMAL);

    /// @brief (AR) إنهاء خيط / (EN) Terminate thread
    void terminateThread(uint64_t threadId, int32_t exitCode = 0);

    /// @brief (AR) الحصول على خيط / (EN) Get thread
    Thread* getThread(uint64_t threadId);

    /// @brief (AR) الحصول على الخيط الحالي / (EN) Get current thread
    Thread* getCurrentThread();

    // ========================================================================
    // تحكم الجدولة / Schedule Control
    // ========================================================================

    /// @brief (AR) تبديل إجباري للسياق / (EN) Force context switch (yield)
    void yield();

    /// @brief (AR) حجب الخيط الحالي / (EN) Block current thread
    void blockCurrentThread();

    /// @brief (AR) إلغاء حجب خيط / (EN) Unblock thread
    void unblockThread(uint64_t threadId);

    /// @brief (AR) تنويم الخيط الحالي / (EN) Sleep current thread (ms)
    void sleep(uint64_t milliseconds);

    /// @brief (AR) تغيير أولوية خيط / (EN) Change thread priority
    void setThreadPriority(uint64_t threadId, Priority priority);

    // ========================================================================
    // مقاطعة المؤقت / Timer Interrupt
    // ========================================================================

    /// @brief (AR) معالج مقاطعة المؤقت (يُستدعى من ISR) / (EN) Timer ISR handler
    void timerTick();

    /// @brief (AR) تعيين الشريحة الزمنية (ms) / (EN) Set time quantum (ms)
    void setTimeQuantum(uint32_t ms);

    /// @brief (AR) الحصول على عداد الـ ticks / (EN) Get tick count
    uint64_t getTickCount() const { return tickCount_; }

    // ========================================================================
    // الإحصائيات / Statistics
    // ========================================================================

    /// @brief (AR) عدد العمليات / (EN) Process count
    uint32_t getProcessCount() const { return static_cast<uint32_t>(processes_.size()); }

    /// @brief (AR) عدد الخيوط / (EN) Thread count
    uint32_t getThreadCount() const { return static_cast<uint32_t>(threads_.size()); }

    /// @brief (AR) عدد تبديلات السياق / (EN) Total context switches
    uint64_t getTotalContextSwitches() const { return totalContextSwitches_; }

    /// @brief (AR) توليد تقرير / (EN) Generate report
    std::string generateReport() const;

private:
    SchedulerManager();
    SchedulerManager(const SchedulerManager&) = delete;
    SchedulerManager& operator=(const SchedulerManager&) = delete;

    /// @brief (AR) اختيار الخيط التالي / (EN) Select next thread to run
    Thread* selectNextThread();

    /// @brief (AR) تبديل السياق / (EN) Perform context switch
    void switchContext(Thread* oldThread, Thread* newThread);

    /// @brief (AR) تهيئة سياق خيط / (EN) Initialize thread context
    void initializeThreadContext(Thread* thread, uint64_t entryPoint, uint64_t arg);

    /// @brief (AR) إيقاظ الخيوط النائمة / (EN) Wake sleeping threads
    void wakeSleepingThreads();

    // البيانات / Data
    std::vector<Process> processes_;
    std::vector<Thread>  threads_;

    uint64_t currentThreadId_;      // الخيط الحالي / Current thread
    uint64_t nextProcessId_;        // معرف العملية التالي / Next process ID
    uint64_t nextThreadId_;         // معرف الخيط التالي / Next thread ID
    uint64_t tickCount_;            // عداد الـ ticks / Tick counter
    uint32_t timeQuantum_;          // الشريحة الزمنية / Time quantum (ticks)
    uint32_t currentQuantumLeft_;   // ما تبقى من الشريحة / Remaining quantum
    uint64_t totalContextSwitches_; // إجمالي التبديلات / Total switches
    bool     isRunning_;            // هل المجدول يعمل / Is scheduler running
    bool     isInitialized_;        // هل مهيأ / Is initialized
};

// ============================================================================
// ثوابت / Constants
// ============================================================================

namespace SchedulerConstants {
    static constexpr uint32_t DEFAULT_QUANTUM_MS        = 10;    // 10ms شريحة / quantum
    static constexpr uint64_t DEFAULT_KERNEL_STACK_SIZE  = 65536; // 64KB
    static constexpr uint64_t DEFAULT_USER_STACK_SIZE    = 2 * 1024 * 1024; // 2MB
    static constexpr uint64_t KERNEL_STACK_BASE          = 0xFFFFFF0000000000ULL;
    static constexpr uint64_t USER_STACK_TOP             = 0x00007FFFFFFFE000ULL;
    static constexpr uint64_t KERNEL_CS                  = 0x08;
    static constexpr uint64_t KERNEL_SS                  = 0x10;
    static constexpr uint64_t USER_CS                    = 0x18 | 3;
    static constexpr uint64_t USER_SS                    = 0x20 | 3;
    static constexpr uint64_t RFLAGS_IF                  = (1 << 9); // مقاطعات مفعلة / IF
    static constexpr uint64_t RFLAGS_DEFAULT             = RFLAGS_IF | (1 << 1); // IF + Reserved
}

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_SCHEDULER_H
