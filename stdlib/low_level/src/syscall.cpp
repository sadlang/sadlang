/*
 * ============================================================================
 * تنفيذ وحدة استدعاءات النظام - لغة ص
 * Syscall Module Implementation - Sad Language
 *
 * @brief (AR) تنفيذ SYSCALL/SYSRET وجدول استدعاءات النظام لنظام x86-64
 * @brief (EN) SYSCALL/SYSRET and syscall table implementation for x86-64 OS
 * ============================================================================
 */

#include "syscall.h"
#include <cstring>
#include <sstream>
#include <iomanip>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace Sad {
namespace LowLevel {

// ============================================================================
// أدوات مساعدة / Helper utilities
// ============================================================================

namespace {
    /// قراءة MSR / Read MSR
    inline uint64_t readMSR(uint32_t msr) {
        uint32_t lo = 0, hi = 0;
#ifdef _MSC_VER
        uint64_t val = __readmsr(msr);
        lo = static_cast<uint32_t>(val);
        hi = static_cast<uint32_t>(val >> 32);
#elif defined(__x86_64__) || defined(__i386__)
        __asm__ volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
#else
        (void)msr;
#endif
        return (static_cast<uint64_t>(hi) << 32) | lo;
    }

    /// كتابة MSR / Write MSR
    inline void writeMSR(uint32_t msr, uint64_t value) {
        uint32_t lo = static_cast<uint32_t>(value);
        uint32_t hi = static_cast<uint32_t>(value >> 32);
#ifdef _MSC_VER
        __writemsr(msr, value);
#elif defined(__x86_64__) || defined(__i386__)
        __asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
#else
        (void)msr; (void)lo; (void)hi;
#endif
    }
}

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

SyscallManager& SyscallManager::getInstance() {
    static SyscallManager instance;
    return instance;
}

SyscallManager::SyscallManager() {
    std::memset(table_, 0, sizeof(table_));
    std::memset(&stats_, 0, sizeof(stats_));
}

// ============================================================================
// التهيئة / Initialization
// ============================================================================

bool SyscallManager::init(uint16_t kernelCS, uint16_t kernelSS,
                           uint16_t userCS, uint16_t userSS) {
    kernelCS_ = kernelCS;
    kernelSS_ = kernelSS;
    userCS_ = userCS;
    userSS_ = userSS;

    // 1. تفعيل SCE (System Call Extensions) في EFER MSR
    //    Enable SCE in EFER MSR
    uint64_t efer = readMSR(SyscallConstants::MSR_EFER);
    efer |= SyscallConstants::EFER_SCE;
    writeMSR(SyscallConstants::MSR_EFER, efer);

    // 2. تعيين STAR MSR (محددات المقاطع)
    //    Set STAR MSR (segment selectors)
    //    STAR[63:48] = SYSRET CS/SS base (user)
    //    STAR[47:32] = SYSCALL CS/SS base (kernel)
    //    SYSCALL: loads CS = STAR[47:32], SS = STAR[47:32] + 8
    //    SYSRET:  loads CS = STAR[63:48] + 16, SS = STAR[63:48] + 8
    uint64_t star = 0;
    star |= static_cast<uint64_t>(kernelCS) << 32;
    star |= static_cast<uint64_t>(userCS) << 48;
    writeMSR(SyscallConstants::MSR_STAR, star);

    // 3. تعيين قناع RFLAGS الافتراضي / Set default RFLAGS mask
    //    يخفي IF (المقاطعات أثناء الانتقال إلى النواة)
    //    Masks IF (interrupts during kernel transition)
    writeMSR(SyscallConstants::MSR_SFMASK, SyscallConstants::DEFAULT_SFMASK);

    isInitialized_ = true;
    return true;
}

void SyscallManager::setSyscallEntry(void* entryPoint) {
    // تعيين LSTAR (عنوان معالج SYSCALL في الوضع الطويل)
    // Set LSTAR (SYSCALL handler address in long mode)
    writeMSR(SyscallConstants::MSR_LSTAR,
             reinterpret_cast<uint64_t>(entryPoint));
}

void SyscallManager::setFlagsMask(uint64_t mask) {
    writeMSR(SyscallConstants::MSR_SFMASK, mask);
}

// ============================================================================
// تسجيل الاستدعاءات / Registering Syscalls
// ============================================================================

bool SyscallManager::registerSyscall(SyscallNumber number, SyscallHandler handler,
                                      const char* name, uint8_t argCount,
                                      bool requiresRoot) {
    uint64_t idx = static_cast<uint64_t>(number);
    if (idx >= MAX_SYSCALLS || !handler) return false;

    table_[idx].handler = handler;
    table_[idx].name = name;
    table_[idx].argCount = argCount;
    table_[idx].requiresRoot = requiresRoot;
    table_[idx].isRegistered = true;

    return true;
}

bool SyscallManager::unregisterSyscall(SyscallNumber number) {
    uint64_t idx = static_cast<uint64_t>(number);
    if (idx >= MAX_SYSCALLS) return false;

    table_[idx].handler = nullptr;
    table_[idx].name = nullptr;
    table_[idx].argCount = 0;
    table_[idx].requiresRoot = false;
    table_[idx].isRegistered = false;

    return true;
}

bool SyscallManager::isSyscallRegistered(SyscallNumber number) const {
    uint64_t idx = static_cast<uint64_t>(number);
    if (idx >= MAX_SYSCALLS) return false;
    return table_[idx].isRegistered;
}

// ============================================================================
// التوزيع / Dispatching
// ============================================================================

int64_t SyscallManager::dispatch(SyscallContext* ctx) {
    if (!ctx) return SyscallConstants::ERR_FAULT;

    uint64_t num = ctx->rax;
    stats_.totalCalls++;

    // التحقق من النطاق / Range check
    if (num >= MAX_SYSCALLS || !table_[num].isRegistered) {
        stats_.unknownCalls++;
        return SyscallConstants::ERR_NOSYS;
    }

    stats_.perSyscall[num]++;

    // فحص الصلاحيات / Permission check
    if (table_[num].requiresRoot && permChecker_) {
        // في نظام حقيقي: الحصول على ProcessId من السياق الحالي
        // In real OS: get ProcessId from current context
        uint64_t pid = 0;
        if (!permChecker_(pid, static_cast<SyscallNumber>(num))) {
            stats_.failedCalls++;
            return SyscallConstants::ERR_PERM;
        }
    }

    // استدعاء المعالج / Call handler
    int64_t result = table_[num].handler(ctx);

    if (result < 0) {
        stats_.failedCalls++;
    }

    return result;
}

// ============================================================================
// الاستدعاءات الافتراضية / Default Syscalls
// ============================================================================

namespace {
    // معالجات الاستدعاءات الافتراضية / Default syscall handlers

    int64_t sysExit(SyscallContext* ctx) {
        // في نظام حقيقي: إنهاء العملية الحالية / In real OS: terminate current process
        (void)ctx;
        return SyscallConstants::ERR_SUCCESS;
    }

    int64_t sysGetPid(SyscallContext* /*ctx*/) {
        // في نظام حقيقي: إرجاع PID الحالي / In real OS: return current PID
        return 1; // نواة / Kernel (PID 1)
    }

    int64_t sysGetPPid(SyscallContext* /*ctx*/) {
        return 0; // النواة ليس لها أب / Kernel has no parent
    }

    int64_t sysYield(SyscallContext* /*ctx*/) {
        // في نظام حقيقي: تبديل السياق / In real OS: context switch
        return SyscallConstants::ERR_SUCCESS;
    }

    int64_t sysSleep(SyscallContext* ctx) {
        // في نظام حقيقي: نوم العملية / In real OS: sleep process
        (void)ctx->rdi; // المدة بالميلي ثانية / Duration in ms
        return SyscallConstants::ERR_SUCCESS;
    }

    int64_t sysWrite(SyscallContext* ctx) {
        // في نظام حقيقي: كتابة إلى واصف الملف / In real OS: write to fd
        int64_t fd = static_cast<int64_t>(ctx->rdi);
        (void)ctx->rsi; // المخزن المؤقت / Buffer
        uint64_t count = ctx->rdx;
        (void)fd;
        return static_cast<int64_t>(count); // إرجاع عدد البايتات / Return bytes written
    }

    int64_t sysRead(SyscallContext* ctx) {
        // في نظام حقيقي: قراءة من واصف الملف / In real OS: read from fd
        (void)ctx;
        return 0;
    }

    int64_t sysOpen(SyscallContext* ctx) {
        // في نظام حقيقي: فتح ملف / In real OS: open file
        (void)ctx;
        return 3; // أول واصف ملف متاح / First available fd
    }

    int64_t sysClose(SyscallContext* ctx) {
        (void)ctx;
        return SyscallConstants::ERR_SUCCESS;
    }

    int64_t sysMmap(SyscallContext* ctx) {
        // في نظام حقيقي: تعيين ذاكرة / In real OS: map memory
        (void)ctx;
        return SyscallConstants::ERR_NOMEM;
    }

    int64_t sysMunmap(SyscallContext* ctx) {
        (void)ctx;
        return SyscallConstants::ERR_SUCCESS;
    }

    int64_t sysGetTime(SyscallContext* /*ctx*/) {
        // في نظام حقيقي: إرجاع الوقت الحالي / In real OS: return current time
        return 0;
    }
}

void SyscallManager::registerDefaultSyscalls() {
    // إدارة العمليات / Process management
    registerSyscall(SyscallNumber::EXIT,    sysExit,   "خروج/exit",    1);
    registerSyscall(SyscallNumber::GETPID,  sysGetPid, "معرّف/getpid", 0);
    registerSyscall(SyscallNumber::GETPPID, sysGetPPid,"أب/getppid",   0);
    registerSyscall(SyscallNumber::YIELD,   sysYield,  "تنازل/yield",  0);
    registerSyscall(SyscallNumber::SLEEP,   sysSleep,  "نوم/sleep",    1);

    // إدارة الملفات / File management
    registerSyscall(SyscallNumber::OPEN,    sysOpen,   "فتح/open",     3);
    registerSyscall(SyscallNumber::CLOSE,   sysClose,  "إغلاق/close",  1);
    registerSyscall(SyscallNumber::READ,    sysRead,   "قراءة/read",   3);
    registerSyscall(SyscallNumber::WRITE,   sysWrite,  "كتابة/write",  3);

    // إدارة الذاكرة / Memory management
    registerSyscall(SyscallNumber::MMAP,    sysMmap,   "تعيين/mmap",   6, true);
    registerSyscall(SyscallNumber::MUNMAP,  sysMunmap, "إلغاء/munmap", 2);

    // التوقيت / Timing
    registerSyscall(SyscallNumber::GETTIME, sysGetTime,"وقت/gettime",  1);
}

// ============================================================================
// الإحصائيات / Statistics
// ============================================================================

void SyscallManager::resetStats() {
    std::memset(&stats_, 0, sizeof(stats_));
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string SyscallManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير استدعاءات النظام / Syscall Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "الحالة / Status: " << (isInitialized_ ? "مهيأ / Initialized" : "غير مهيأ / Not initialized") << "\n";

    if (isInitialized_) {
        report << "CS النواة / Kernel CS: 0x" << std::hex << kernelCS_ << std::dec << "\n";
        report << "SS النواة / Kernel SS: 0x" << std::hex << kernelSS_ << std::dec << "\n";
        report << "CS المستخدم / User CS: 0x" << std::hex << userCS_ << std::dec << "\n";
        report << "SS المستخدم / User SS: 0x" << std::hex << userSS_ << std::dec << "\n";
    }

    // عدد الاستدعاءات المسجلة / Registered syscalls count
    uint32_t registered = 0;
    for (uint32_t i = 0; i < MAX_SYSCALLS; i++) {
        if (table_[i].isRegistered) registered++;
    }
    report << "\nاستدعاءات مسجلة / Registered: " << registered << "/" << MAX_SYSCALLS << "\n\n";

    // قائمة الاستدعاءات / Syscall list
    report << "جدول الاستدعاءات / Syscall Table:\n";
    report << std::setw(6) << "رقم" << " | "
           << std::setw(20) << "اسم / Name" << " | "
           << std::setw(6) << "معاملات" << " | "
           << std::setw(8) << "جذر" << " | "
           << "استدعاءات / Calls\n";
    report << std::string(65, '-') << "\n";

    for (uint32_t i = 0; i < MAX_SYSCALLS; i++) {
        if (!table_[i].isRegistered) continue;
        report << std::setw(6) << i << " | "
               << std::setw(20) << (table_[i].name ? table_[i].name : "---") << " | "
               << std::setw(6) << static_cast<int>(table_[i].argCount) << " | "
               << std::setw(8) << (table_[i].requiresRoot ? "نعم" : "لا") << " | "
               << stats_.perSyscall[i] << "\n";
    }

    // الإحصائيات / Statistics
    report << "\nإحصائيات / Statistics:\n";
    report << "  إجمالي / Total:    " << stats_.totalCalls << "\n";
    report << "  فاشلة / Failed:    " << stats_.failedCalls << "\n";
    report << "  مجهولة / Unknown:  " << stats_.unknownCalls << "\n";

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
