/*
 * ============================================================================
 * وحدة استدعاءات النظام (Syscall) - لغة ص
 * System Call Module - Sad Language
 *
 * @brief (AR) واجهة SYSCALL/SYSRET لنظام تشغيل x86-64 (Long Mode)
 * @brief (EN) SYSCALL/SYSRET interface for x86-64 OS (Long Mode)
 *
 * يدعم: تهيئة MSRs (STAR, LSTAR, SFMASK)، جدول استدعاءات النظام،
 *       معالج استدعاء النظام، انتقال المستخدم-النواة، تمرير المعاملات
 * ============================================================================
 */

#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace Sad {
namespace LowLevel {

// ============================================================================
// أرقام استدعاءات النظام / System Call Numbers
// ============================================================================

enum class SyscallNumber : uint64_t {
    // إدارة العمليات / Process management
    EXIT            = 0,    // إنهاء / Exit process
    FORK            = 1,    // تفرع / Fork process
    EXEC            = 2,    // تنفيذ / Execute program
    WAIT            = 3,    // انتظار / Wait for child
    GETPID          = 4,    // معرّف العملية / Get process ID
    GETPPID         = 5,    // معرّف الأب / Get parent PID
    YIELD           = 6,    // تنازل / Yield CPU
    SLEEP           = 7,    // نوم / Sleep (ms)

    // إدارة الملفات / File management
    OPEN            = 10,   // فتح / Open file
    CLOSE           = 11,   // إغلاق / Close file
    READ            = 12,   // قراءة / Read from fd
    WRITE           = 13,   // كتابة / Write to fd
    SEEK            = 14,   // تنقل / Seek in file
    STAT            = 15,   // حالة / Get file status
    UNLINK          = 16,   // حذف / Delete file
    MKDIR           = 17,   // إنشاء مجلد / Create directory
    RMDIR           = 18,   // حذف مجلد / Remove directory
    RENAME          = 19,   // إعادة تسمية / Rename
    READDIR         = 20,   // قراءة مجلد / Read directory
    TRUNCATE        = 21,   // اقتطاع / Truncate file
    DUP             = 22,   // تكرار واصف / Duplicate fd
    PIPE            = 23,   // أنبوب / Create pipe
    IOCTL           = 24,   // تحكم إدخال/إخراج / IO control

    // إدارة الذاكرة / Memory management
    MMAP            = 30,   // تعيين ذاكرة / Map memory
    MUNMAP          = 31,   // إلغاء تعيين / Unmap memory
    MPROTECT        = 32,   // حماية ذاكرة / Protect memory
    BRK             = 33,   // تغيير حد الكومة / Change heap break
    SBRK            = 34,   // تغيير حد الكومة / Change break (delta)

    // إدارة الإشارات / Signal management
    SIGNAL          = 40,   // تعيين معالج إشارة / Set signal handler
    KILL            = 41,   // إرسال إشارة / Send signal
    SIGRETURN       = 42,   // عودة من إشارة / Return from signal

    // التوقيت / Timing
    GETTIME         = 50,   // الوقت الحالي / Get current time
    NANOSLEEP       = 51,   // نوم دقيق / Precise sleep

    // المقابس / Sockets
    SOCKET          = 60,   // إنشاء مقبس / Create socket
    BIND            = 61,   // ربط / Bind socket
    LISTEN          = 62,   // استماع / Listen
    ACCEPT          = 63,   // قبول / Accept connection
    CONNECT         = 64,   // اتصال / Connect
    SEND            = 65,   // إرسال / Send data
    RECV            = 66,   // استقبال / Receive data

    // متقدمة / Advanced
    CLONE           = 100,  // نسخ خيط / Clone thread/process
    FUTEX           = 101,  // قفل سريع / Fast userspace mutex

    MAX_SYSCALL     = 256   // الحد الأقصى / Maximum
};

// ============================================================================
// سياق الاستدعاء / Syscall Context
// ============================================================================

/// سياق السجلات عند استدعاء النظام / Register context at syscall entry
#pragma pack(push, 1)
struct SyscallContext {
    // معاملات الاستدعاء (ABI System V) / Syscall arguments (System V ABI)
    uint64_t rdi;       // المعامل الأول / Argument 1
    uint64_t rsi;       // المعامل الثاني / Argument 2
    uint64_t rdx;       // المعامل الثالث / Argument 3
    uint64_t r10;       // المعامل الرابع (بدل rcx) / Argument 4 (instead of rcx)
    uint64_t r8;        // المعامل الخامس / Argument 5
    uint64_t r9;        // المعامل السادس / Argument 6

    // رقم الاستدعاء / Syscall number
    uint64_t rax;       // رقم الاستدعاء / Syscall number (input); return value (output)

    // سجلات محفوظة / Preserved registers
    uint64_t rbx;
    uint64_t rbp;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;

    // عنوان العودة والأعلام / Return address and flags
    uint64_t rcx;       // RIP المحفوظ بواسطة SYSCALL / RIP saved by SYSCALL
    uint64_t r11;       // RFLAGS المحفوظة بواسطة SYSCALL / RFLAGS saved by SYSCALL

    // مؤشر المكدس المستخدم / User stack pointer
    uint64_t userRSP;
};
#pragma pack(pop)

// ============================================================================
// نوع معالج الاستدعاء / Syscall Handler Type
// ============================================================================

/// دالة معالج استدعاء النظام / Syscall handler function
/// تأخذ السياق وترجع القيمة في rax / Takes context, return via rax
using SyscallHandler = int64_t(*)(SyscallContext* ctx);

/// معلومات استدعاء نظام مسجل / Registered syscall info
struct SyscallEntry {
    SyscallHandler handler;        // المعالج / Handler function
    const char*    name;           // الاسم (عربي/إنجليزي) / Name
    uint8_t        argCount;       // عدد المعاملات / Argument count
    bool           requiresRoot;   // يتطلب صلاحيات / Requires root privileges
    bool           isRegistered;   // مسجّل / Is registered
};

/// إحصائيات استدعاء النظام / Syscall statistics
struct SyscallStats {
    uint64_t totalCalls;     // إجمالي الاستدعاءات / Total calls
    uint64_t failedCalls;    // الاستدعاءات الفاشلة / Failed calls
    uint64_t unknownCalls;   // استدعاءات غير معروفة / Unknown syscall calls
    uint64_t perSyscall[256]; // لكل استدعاء / Per-syscall counts
};

// ============================================================================
// مدير استدعاءات النظام / Syscall Manager
// ============================================================================

/**
 * @class SyscallManager
 * @brief (AR) مدير استدعاءات النظام SYSCALL/SYSRET في x86-64
 * @brief (EN) x86-64 SYSCALL/SYSRET system call manager
 */
class SyscallManager {
public:
    /// الحصول على المثيل الوحيد / Get singleton instance
    static SyscallManager& getInstance();

    // ==== تهيئة / Initialization ====

    /// تهيئة آلية SYSCALL/SYSRET / Initialize SYSCALL/SYSRET mechanism
    bool init(uint16_t kernelCS, uint16_t kernelSS,
              uint16_t userCS, uint16_t userSS);

    /// هل المنظومة مهيأة؟ / Is system initialized?
    bool isInitialized() const { return isInitialized_; }

    /// تعيين معالج الاستدعاء الرئيسي (LSTAR) / Set main syscall handler (LSTAR)
    void setSyscallEntry(void* entryPoint);

    /// تعيين قناع الأعلام (SFMASK) / Set flags mask (SFMASK)
    void setFlagsMask(uint64_t mask);

    // ==== تسجيل الاستدعاءات / Registering Syscalls ====

    /// تسجيل معالج استدعاء نظام / Register syscall handler
    bool registerSyscall(SyscallNumber number, SyscallHandler handler,
                         const char* name, uint8_t argCount,
                         bool requiresRoot = false);

    /// إلغاء تسجيل استدعاء / Unregister syscall
    bool unregisterSyscall(SyscallNumber number);

    /// هل الاستدعاء مسجّل؟ / Is syscall registered?
    bool isSyscallRegistered(SyscallNumber number) const;

    // ==== توزيع الاستدعاءات / Dispatching ====

    /// توزيع استدعاء النظام / Dispatch syscall
    int64_t dispatch(SyscallContext* ctx);

    /// الحصول على جدول الاستدعاءات / Get syscall table
    const SyscallEntry* getSyscallTable() const { return table_; }

    // ==== تسجيل الاستدعاءات الافتراضية / Register Default Syscalls ====

    /// تسجيل مجموعة الاستدعاءات الأساسية / Register basic syscall set
    void registerDefaultSyscalls();

    // ==== الإحصائيات / Statistics ====

    /// الحصول على إحصائيات / Get statistics
    SyscallStats getStats() const { return stats_; }

    /// إعادة تعيين الإحصائيات / Reset statistics
    void resetStats();

    // ==== أمان / Security ====

    /// تعيين فحص الصلاحيات / Set permission checker
    using PermissionChecker = bool(*)(uint64_t processId, SyscallNumber number);
    void setPermissionChecker(PermissionChecker checker) { permChecker_ = checker; }

    // ==== التقرير / Report ====

    /// إنشاء تقرير / Generate report
    std::string generateReport() const;

private:
    SyscallManager();
    SyscallManager(const SyscallManager&) = delete;
    SyscallManager& operator=(const SyscallManager&) = delete;

    /// جدول استدعاءات النظام / Syscall table
    static constexpr uint32_t MAX_SYSCALLS = 256;
    SyscallEntry table_[MAX_SYSCALLS];

    /// الإحصائيات / Statistics
    SyscallStats stats_;

    /// فحص الصلاحيات / Permission checker
    PermissionChecker permChecker_ = nullptr;

    bool isInitialized_ = false;
    uint16_t kernelCS_ = 0;
    uint16_t kernelSS_ = 0;
    uint16_t userCS_ = 0;
    uint16_t userSS_ = 0;
};

// ============================================================================
// ثوابت SYSCALL / Syscall Constants
// ============================================================================

namespace SyscallConstants {
    /// MSRs لتهيئة SYSCALL/SYSRET / MSRs for SYSCALL/SYSRET setup
    static constexpr uint32_t MSR_STAR   = 0xC0000081;  // محددات المقاطع / Segment selectors
    static constexpr uint32_t MSR_LSTAR  = 0xC0000082;  // نقطة دخول SYSCALL (64-bit) / Entry point
    static constexpr uint32_t MSR_CSTAR  = 0xC0000083;  // نقطة دخول (وضع التوافق) / Compat mode entry
    static constexpr uint32_t MSR_SFMASK = 0xC0000084;  // قناع RFLAGS / RFLAGS mask

    /// MSR لتفعيل SCE / EFER MSR for SCE enable
    static constexpr uint32_t MSR_EFER  = 0xC0000080;
    static constexpr uint64_t EFER_SCE  = (1ULL << 0);  // تفعيل SYSCALL/SYSRET / Enable SCE

    /// قناع RFLAGS الافتراضي / Default RFLAGS mask
    /// يخفي IF (المقاطعات) و TF (التتبع) و DF (الاتجاه)
    /// Masks: IF (interrupts), TF (trap), DF (direction)
    static constexpr uint64_t DEFAULT_SFMASK = (1 << 9) | (1 << 8) | (1 << 10);

    /// أكواد الخطأ / Error codes
    static constexpr int64_t ERR_SUCCESS     =  0;
    static constexpr int64_t ERR_NOSYS       = -1;   // استدعاء غير موجود / No such syscall
    static constexpr int64_t ERR_PERM        = -2;   // لا صلاحية / Permission denied
    static constexpr int64_t ERR_INVAL       = -3;   // معامل غير صالح / Invalid argument
    static constexpr int64_t ERR_FAULT       = -4;   // خطأ عنوان / Bad address
    static constexpr int64_t ERR_NOMEM       = -5;   // نفاد الذاكرة / Out of memory
    static constexpr int64_t ERR_NOENT       = -6;   // غير موجود / No such entry
    static constexpr int64_t ERR_BUSY        = -7;   // مشغول / Resource busy
    static constexpr int64_t ERR_IO          = -8;   // خطأ إدخال/إخراج / I/O error
    static constexpr int64_t ERR_BADF        = -9;   // واصف ملف خاطئ / Bad file descriptor
    static constexpr int64_t ERR_AGAIN       = -10;  // حاول لاحقاً / Try again
    static constexpr int64_t ERR_NOCHILD     = -11;  // لا أبناء / No child processes
    static constexpr int64_t ERR_EXIST       = -12;  // موجود مسبقاً / Already exists
    static constexpr int64_t ERR_NOTDIR      = -13;  // ليس مجلداً / Not a directory
    static constexpr int64_t ERR_ISDIR       = -14;  // هو مجلد / Is a directory
    static constexpr int64_t ERR_PIPE        = -15;  // أنبوب مكسور / Broken pipe
}

// ============================================================================
// ماكرو الاستدعاء من المستخدم / User-space Syscall Macro
// ============================================================================

/// استدعاء النظام من وضع المستخدم (inline assembly)
/// User-space syscall invocation (inline assembly)
#ifndef _MSC_VER
// GCC/Clang: دعم كامل للأسمبلي المضمّن / Full inline asm support
#define SAD_SYSCALL0(num) ({ \
    int64_t ret; \
    __asm__ volatile("syscall" : "=a"(ret) : "a"(num) : "rcx", "r11", "memory"); \
    ret; \
})

#define SAD_SYSCALL1(num, a1) ({ \
    int64_t ret; \
    __asm__ volatile("syscall" : "=a"(ret) : "a"(num), "D"(a1) \
                     : "rcx", "r11", "memory"); \
    ret; \
})

#define SAD_SYSCALL2(num, a1, a2) ({ \
    int64_t ret; \
    __asm__ volatile("syscall" : "=a"(ret) : "a"(num), "D"(a1), "S"(a2) \
                     : "rcx", "r11", "memory"); \
    ret; \
})

#define SAD_SYSCALL3(num, a1, a2, a3) ({ \
    int64_t ret; \
    register uint64_t r10 __asm__("r10") = (uint64_t)(a3); \
    __asm__ volatile("syscall" : "=a"(ret) \
                     : "a"(num), "D"(a1), "S"(a2), "d"(0), "r"(r10) \
                     : "rcx", "r11", "memory"); \
    ret; \
})

#define SAD_SYSCALL4(num, a1, a2, a3, a4) ({ \
    int64_t ret; \
    register uint64_t r10 __asm__("r10") = (uint64_t)(a3); \
    register uint64_t r8  __asm__("r8")  = (uint64_t)(a4); \
    __asm__ volatile("syscall" : "=a"(ret) \
                     : "a"(num), "D"(a1), "S"(a2), "d"(0), "r"(r10), "r"(r8) \
                     : "rcx", "r11", "memory"); \
    ret; \
})
#endif

} // namespace LowLevel
} // namespace Sad
