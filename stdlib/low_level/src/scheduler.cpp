/*
 * ============================================================================
 * تنفيذ الجدولة والعمليات - لغة ص
 * Scheduler & Process Implementation - Sad Language
 * ============================================================================
 */

#include "scheduler.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Sad {
namespace LowLevel {

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

SchedulerManager& SchedulerManager::getInstance() {
    static SchedulerManager instance;
    return instance;
}

SchedulerManager::SchedulerManager()
    : currentThreadId_(0)
    , nextProcessId_(1)
    , nextThreadId_(1)
    , tickCount_(0)
    , timeQuantum_(SchedulerConstants::DEFAULT_QUANTUM_MS)
    , currentQuantumLeft_(0)
    , totalContextSwitches_(0)
    , isRunning_(false)
    , isInitialized_(false) {}

// ============================================================================
// التهيئة / Initialization
// ============================================================================

void SchedulerManager::initialize() {
    if (isInitialized_) return;

    // إنشاء عملية النواة (PID 0) / Create kernel process (PID 0)
    Process kernelProcess;
    kernelProcess.id = 0;
    kernelProcess.parentId = 0;
    std::strncpy(kernelProcess.name, "نواة / Kernel", sizeof(kernelProcess.name) - 1);
    kernelProcess.state = ProcessState::RUNNING;
    kernelProcess.priority = Priority::HIGH;
    kernelProcess.pageTableRoot = 0; // سيُحدَّث لاحقاً / Updated later
    processes_.push_back(kernelProcess);

    // إنشاء الخيط الرئيسي / Create main thread
    Thread mainThread;
    mainThread.id = 0;
    mainThread.processId = 0;
    std::strncpy(mainThread.name, "رئيسي / Main", sizeof(mainThread.name) - 1);
    mainThread.state = ProcessState::RUNNING;
    mainThread.priority = Priority::HIGH;
    mainThread.creationTime = 0;
    threads_.push_back(mainThread);

    currentThreadId_ = 0;
    currentQuantumLeft_ = timeQuantum_;
    isInitialized_ = true;
}

void SchedulerManager::start() {
    isRunning_ = true;
}

void SchedulerManager::stop() {
    isRunning_ = false;
}

// ============================================================================
// إدارة العمليات / Process Management
// ============================================================================

uint64_t SchedulerManager::createProcess(const char* name, Priority priority) {
    Process process;
    process.id = nextProcessId_++;
    process.parentId = currentThreadId_ > 0 ?
        getThread(currentThreadId_)->processId : 0;
    if (name) {
        std::strncpy(process.name, name, sizeof(process.name) - 1);
        process.name[sizeof(process.name) - 1] = '\0';
    }
    process.state = ProcessState::CREATED;
    process.priority = priority;
    process.creationTime = tickCount_;
    processes_.push_back(process);

    return process.id;
}

void SchedulerManager::terminateProcess(uint64_t processId, int32_t exitCode) {
    Process* process = getProcess(processId);
    if (!process) return;

    // إنهاء جميع خيوط العملية / Terminate all threads
    for (auto& thread : threads_) {
        if (thread.processId == processId && thread.state != ProcessState::TERMINATED) {
            thread.state = ProcessState::TERMINATED;
            thread.exitCode = exitCode;
        }
    }

    process->state = ProcessState::TERMINATED;
    process->exitCode = exitCode;
}

Process* SchedulerManager::getProcess(uint64_t processId) {
    for (auto& process : processes_) {
        if (process.id == processId) {
            return &process;
        }
    }
    return nullptr;
}

Process* SchedulerManager::getCurrentProcess() {
    Thread* currentThread = getCurrentThread();
    if (!currentThread) return nullptr;
    return getProcess(currentThread->processId);
}

// ============================================================================
// إدارة الخيوط / Thread Management
// ============================================================================

uint64_t SchedulerManager::createThread(uint64_t processId, const char* name,
                                         uint64_t entryPoint, uint64_t arg,
                                         Priority priority, uint64_t stackSize) {
    Process* process = getProcess(processId);
    if (!process) return 0;

    Thread thread;
    thread.id = nextThreadId_++;
    thread.processId = processId;
    if (name) {
        std::strncpy(thread.name, name, sizeof(thread.name) - 1);
        thread.name[sizeof(thread.name) - 1] = '\0';
    }
    thread.state = ProcessState::READY;
    thread.priority = priority;
    thread.creationTime = tickCount_;

    // تخصيص مكدس المستخدم / Allocate user stack
    thread.userStackSize = stackSize;
    // في نظام حقيقي: تخصيص صفحات / In real OS: allocate pages
    static uint64_t stackPool = SchedulerConstants::USER_STACK_TOP;
    stackPool -= stackSize;
    thread.userStackBase = stackPool;

    // تخصيص مكدس النواة / Allocate kernel stack
    thread.kernelStackSize = SchedulerConstants::DEFAULT_KERNEL_STACK_SIZE;
    static uint64_t kernelStackPool = SchedulerConstants::KERNEL_STACK_BASE;
    kernelStackPool -= thread.kernelStackSize;
    thread.kernelStackBase = kernelStackPool;

    // تهيئة السياق / Initialize context
    initializeThreadContext(&thread, entryPoint, arg);

    process->threadIds.push_back(thread.id);
    threads_.push_back(thread);

    return thread.id;
}

uint64_t SchedulerManager::createKernelThread(const char* name, uint64_t entryPoint,
                                                uint64_t arg, Priority priority) {
    Thread thread;
    thread.id = nextThreadId_++;
    thread.processId = 0; // عملية النواة / Kernel process
    if (name) {
        std::strncpy(thread.name, name, sizeof(thread.name) - 1);
        thread.name[sizeof(thread.name) - 1] = '\0';
    }
    thread.state = ProcessState::READY;
    thread.priority = priority;
    thread.creationTime = tickCount_;

    // مكدس نواة فقط / Kernel stack only
    thread.kernelStackSize = SchedulerConstants::DEFAULT_KERNEL_STACK_SIZE;
    static uint64_t kernelStackPool = SchedulerConstants::KERNEL_STACK_BASE + 0x100000;
    kernelStackPool -= thread.kernelStackSize;
    thread.kernelStackBase = kernelStackPool;

    thread.userStackBase = 0;
    thread.userStackSize = 0;

    // تهيئة سياق النواة / Initialize kernel context
    thread.context.rip = entryPoint;
    thread.context.rdi = arg;
    thread.context.cs = SchedulerConstants::KERNEL_CS;
    thread.context.ss = SchedulerConstants::KERNEL_SS;
    thread.context.rflags = SchedulerConstants::RFLAGS_DEFAULT;
    thread.context.rsp = thread.kernelStackBase + thread.kernelStackSize - 8;
    thread.context.rbp = thread.context.rsp;

    threads_.push_back(thread);

    Process* kernel = getProcess(0);
    if (kernel) {
        kernel->threadIds.push_back(thread.id);
    }

    return thread.id;
}

void SchedulerManager::terminateThread(uint64_t threadId, int32_t exitCode) {
    Thread* thread = getThread(threadId);
    if (!thread) return;

    thread->state = ProcessState::TERMINATED;
    thread->exitCode = exitCode;

    // إذا كان الخيط الحالي، اختر التالي / If current thread, select next
    if (threadId == currentThreadId_) {
        yield();
    }
}

Thread* SchedulerManager::getThread(uint64_t threadId) {
    for (auto& thread : threads_) {
        if (thread.id == threadId) {
            return &thread;
        }
    }
    return nullptr;
}

Thread* SchedulerManager::getCurrentThread() {
    return getThread(currentThreadId_);
}

// ============================================================================
// تحكم الجدولة / Schedule Control
// ============================================================================

void SchedulerManager::yield() {
    if (!isRunning_) return;

    Thread* current = getCurrentThread();
    Thread* next = selectNextThread();

    if (next && next != current) {
        if (current && current->state == ProcessState::RUNNING) {
            current->state = ProcessState::READY;
        }
        switchContext(current, next);
    }
}

void SchedulerManager::blockCurrentThread() {
    Thread* current = getCurrentThread();
    if (!current) return;

    current->state = ProcessState::BLOCKED;
    yield();
}

void SchedulerManager::unblockThread(uint64_t threadId) {
    Thread* thread = getThread(threadId);
    if (thread && thread->state == ProcessState::BLOCKED) {
        thread->state = ProcessState::READY;
    }
}

void SchedulerManager::sleep(uint64_t milliseconds) {
    Thread* current = getCurrentThread();
    if (!current) return;

    current->state = ProcessState::SLEEPING;
    current->sleepUntil = tickCount_ + milliseconds;
    yield();
}

void SchedulerManager::setThreadPriority(uint64_t threadId, Priority priority) {
    Thread* thread = getThread(threadId);
    if (thread) {
        thread->priority = priority;
    }
}

// ============================================================================
// مقاطعة المؤقت / Timer Interrupt
// ============================================================================

void SchedulerManager::timerTick() {
    tickCount_++;

    // إيقاظ الخيوط النائمة / Wake sleeping threads
    wakeSleepingThreads();

    if (!isRunning_) return;

    Thread* current = getCurrentThread();
    if (current) {
        current->totalTicks++;
    }

    // هل انتهت الشريحة الزمنية / Has quantum expired?
    if (currentQuantumLeft_ > 0) {
        currentQuantumLeft_--;
    }

    if (currentQuantumLeft_ == 0) {
        // الشريحة انتهت: تبديل / Quantum expired: switch
        yield();
    }
}

void SchedulerManager::setTimeQuantum(uint32_t ms) {
    timeQuantum_ = ms;
}

// ============================================================================
// الخوارزميات الداخلية / Internal Algorithms
// ============================================================================

Thread* SchedulerManager::selectNextThread() {
    // جدولة Round-Robin مع أولويات / Priority Round-Robin
    Thread* best = nullptr;
    uint8_t bestPriority = 0;

    // البحث من بعد الخيط الحالي (round-robin) / Search from after current (round-robin)
    size_t count = threads_.size();
    size_t startIdx = 0;

    // إيجاد فهرس الخيط الحالي / Find current thread index
    for (size_t i = 0; i < count; i++) {
        if (threads_[i].id == currentThreadId_) {
            startIdx = i + 1;
            break;
        }
    }

    // البحث بطريقة round-robin / Round-robin search
    for (size_t i = 0; i < count; i++) {
        size_t idx = (startIdx + i) % count;
        Thread& t = threads_[idx];

        if (t.state != ProcessState::READY) continue;

        uint8_t p = static_cast<uint8_t>(t.priority);
        if (!best || p > bestPriority) {
            best = &t;
            bestPriority = p;
        }
    }

    return best;
}

void SchedulerManager::switchContext(Thread* oldThread, Thread* newThread) {
    if (!newThread) return;

    // حفظ السياق القديم / Save old context
    // في نظام حقيقي: حفظ السجلات عبر Assembly
    // In real OS: save registers via assembly

    if (oldThread) {
        oldThread->contextSwitches++;
        // حفظ سياق FPU / Save FPU context
#if defined(__GNUC__) || defined(__clang__)
        // __asm__ volatile("fxsave %0" : "=m"(oldThread->fpuContext));
#endif
    }

    // تحميل السياق الجديد / Load new context
    newThread->state = ProcessState::RUNNING;
    currentThreadId_ = newThread->id;
    currentQuantumLeft_ = timeQuantum_;
    totalContextSwitches_++;
    newThread->contextSwitches++;

    // تبديل CR3 إذا عملية مختلفة / Switch CR3 if different process
    if (oldThread && oldThread->processId != newThread->processId) {
        Process* newProcess = getProcess(newThread->processId);
        if (newProcess && newProcess->pageTableRoot) {
#if defined(__GNUC__) || defined(__clang__)
            __asm__ volatile("mov %0, %%cr3" :: "r"(newProcess->pageTableRoot));
#endif
        }
    }

    // استعادة سياق FPU / Restore FPU context
#if defined(__GNUC__) || defined(__clang__)
    // __asm__ volatile("fxrstor %0" :: "m"(newThread->fpuContext));
#endif

    // في نظام حقيقي: iretq لاستعادة السياق الكامل
    // In real OS: iretq to restore full context
}

void SchedulerManager::initializeThreadContext(Thread* thread, uint64_t entryPoint,
                                                uint64_t arg) {
    // تهيئة السياق لخيط مستخدم / Initialize user thread context
    thread->context.rip = entryPoint;
    thread->context.rdi = arg;           // الوسيط الأول (System V ABI) / First argument
    thread->context.cs = SchedulerConstants::USER_CS;
    thread->context.ss = SchedulerConstants::USER_SS;
    thread->context.rflags = SchedulerConstants::RFLAGS_DEFAULT;
    thread->context.rsp = thread->userStackBase + thread->userStackSize - 8;
    thread->context.rbp = thread->context.rsp;

    // تصفير باقي السجلات / Zero remaining registers
    thread->context.rax = 0;
    thread->context.rbx = 0;
    thread->context.rcx = 0;
    thread->context.rdx = 0;
    thread->context.rsi = 0;
    thread->context.r8 = 0;
    thread->context.r9 = 0;
    thread->context.r10 = 0;
    thread->context.r11 = 0;
    thread->context.r12 = 0;
    thread->context.r13 = 0;
    thread->context.r14 = 0;
    thread->context.r15 = 0;
}

void SchedulerManager::wakeSleepingThreads() {
    for (auto& thread : threads_) {
        if (thread.state == ProcessState::SLEEPING &&
            tickCount_ >= thread.sleepUntil) {
            thread.state = ProcessState::READY;
        }
    }
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string SchedulerManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير المجدول / Scheduler Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "الحالة / Status: " << (isRunning_ ? "يعمل / Running" : "متوقف / Stopped") << "\n";
    report << "عداد Ticks: " << tickCount_ << "\n";
    report << "شريحة زمنية / Quantum: " << timeQuantum_ << " ms\n";
    report << "تبديلات السياق / Switches: " << totalContextSwitches_ << "\n";
    report << "عمليات / Processes: " << processes_.size() << "\n";
    report << "خيوط / Threads: " << threads_.size() << "\n\n";

    // جدول العمليات / Process table
    report << "--- العمليات / Processes ---\n";
    report << std::left
           << std::setw(6)  << "PID"
           << std::setw(30) << "الاسم / Name"
           << std::setw(12) << "الحالة / State"
           << std::setw(10) << "خيوط"
           << "\n";
    report << std::string(60, '-') << "\n";

    for (const auto& p : processes_) {
        const char* stateStr;
        switch (p.state) {
            case ProcessState::CREATED:    stateStr = "جديدة"; break;
            case ProcessState::READY:      stateStr = "جاهزة"; break;
            case ProcessState::RUNNING:    stateStr = "تعمل"; break;
            case ProcessState::BLOCKED:    stateStr = "محجوبة"; break;
            case ProcessState::SLEEPING:   stateStr = "نائمة"; break;
            case ProcessState::ZOMBIE:     stateStr = "ميتة"; break;
            case ProcessState::TERMINATED: stateStr = "منتهية"; break;
        }
        report << std::left
               << std::setw(6) << p.id
               << std::setw(30) << p.name
               << std::setw(12) << stateStr
               << std::setw(10) << p.threadIds.size()
               << "\n";
    }

    // جدول الخيوط / Thread table
    report << "\n--- الخيوط / Threads ---\n";
    report << std::left
           << std::setw(6)  << "TID"
           << std::setw(6)  << "PID"
           << std::setw(20) << "الاسم / Name"
           << std::setw(12) << "الحالة"
           << std::setw(10) << "الأولوية"
           << std::setw(10) << "Ticks"
           << "\n";
    report << std::string(65, '-') << "\n";

    for (const auto& t : threads_) {
        const char* stateStr;
        switch (t.state) {
            case ProcessState::CREATED:    stateStr = "جديد"; break;
            case ProcessState::READY:      stateStr = "جاهز"; break;
            case ProcessState::RUNNING:    stateStr = "يعمل"; break;
            case ProcessState::BLOCKED:    stateStr = "محجوب"; break;
            case ProcessState::SLEEPING:   stateStr = "نائم"; break;
            case ProcessState::ZOMBIE:     stateStr = "ميت"; break;
            case ProcessState::TERMINATED: stateStr = "منتهي"; break;
        }
        const char* prioStr;
        switch (t.priority) {
            case Priority::IDLE:     prioStr = "خامل"; break;
            case Priority::LOW:      prioStr = "منخفض"; break;
            case Priority::NORMAL:   prioStr = "عادي"; break;
            case Priority::HIGH:     prioStr = "عالي"; break;
            case Priority::REALTIME: prioStr = "فوري"; break;
        }
        report << std::left
               << std::setw(6) << t.id
               << std::setw(6) << t.processId
               << std::setw(20) << t.name
               << std::setw(12) << stateStr
               << std::setw(10) << prioStr
               << std::setw(10) << t.totalTicks
               << "\n";
    }

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
