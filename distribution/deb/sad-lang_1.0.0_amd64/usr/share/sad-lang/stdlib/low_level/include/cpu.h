/*
 * ============================================================================
 * وحدة المعالج المركزي للغة ص - CPU Module for Sad Language
 * ============================================================================
 *
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يوفر واجهة شاملة للتعامل مع المعالج المركزي (CPU).
 * يشمل كشف ميزات المعالج عبر CPUID، قراءة/كتابة سجلات التحكم (CR0-CR4)،
 * سجلات النموذج المحدد (MSR)، وتعليمات خاصة بالمعالج.
 *
 * English Description:
 * -------------------
 * This file provides a comprehensive interface for CPU interaction.
 * Includes feature detection via CPUID, control register (CR0-CR4) access,
 * Model Specific Registers (MSR), and special CPU instructions.
 *
 * المؤلف / Author: Sad Language Team
 * الإصدار / Version: 2.0.0
 * ============================================================================
 */

#ifndef SAD_CPU_H
#define SAD_CPU_H

#include <cstdint>
#include <string>
#include <vector>

namespace Sad {
namespace LowLevel {

// ============================================================================
// ثوابت سجلات التحكم / Control Register Constants
// ============================================================================

namespace CR0Flags {
    constexpr uint64_t PE = 1ULL << 0;   ///< وضع الحماية / Protection Enable
    constexpr uint64_t MP = 1ULL << 1;   ///< مراقبة المعالج المساعد / Monitor Coprocessor
    constexpr uint64_t EM = 1ULL << 2;   ///< محاكاة FPU / FPU Emulation
    constexpr uint64_t TS = 1ULL << 3;   ///< تبديل المهمة / Task Switched
    constexpr uint64_t ET = 1ULL << 4;   ///< نوع الامتداد / Extension Type
    constexpr uint64_t NE = 1ULL << 5;   ///< خطأ رقمي / Numeric Error
    constexpr uint64_t WP = 1ULL << 16;  ///< حماية الكتابة / Write Protect
    constexpr uint64_t AM = 1ULL << 18;  ///< فحص المحاذاة / Alignment Mask
    constexpr uint64_t NW = 1ULL << 29;  ///< بدون كتابة خلفية / Not Write-through
    constexpr uint64_t CD = 1ULL << 30;  ///< تعطيل الذاكرة المخبئية / Cache Disable
    constexpr uint64_t PG = 1ULL << 31;  ///< تفعيل الترحيل / Paging Enable
}

namespace CR4Flags {
    constexpr uint64_t VME   = 1ULL << 0;   ///< امتدادات الوضع الافتراضي / Virtual 8086 Mode Extensions
    constexpr uint64_t PVI   = 1ULL << 1;   ///< مقاطعات افتراضية / Protected Virtual Interrupts
    constexpr uint64_t TSD   = 1ULL << 2;   ///< تعطيل الختم الزمني / Time Stamp Disable
    constexpr uint64_t DE    = 1ULL << 3;   ///< امتدادات التنقيح / Debugging Extensions
    constexpr uint64_t PSE   = 1ULL << 4;   ///< امتداد حجم الصفحة / Page Size Extension
    constexpr uint64_t PAE   = 1ULL << 5;   ///< امتداد العنوان المادي / Physical Address Extension
    constexpr uint64_t MCE   = 1ULL << 6;   ///< تفعيل فحص الآلة / Machine Check Enable
    constexpr uint64_t PGE   = 1ULL << 7;   ///< تفعيل الصفحة العامة / Page Global Enable
    constexpr uint64_t PCE   = 1ULL << 8;   ///< تفعيل عداد الأداء / Performance Counter Enable
    constexpr uint64_t OSFXSR    = 1ULL << 9;   ///< دعم FXSAVE/FXRSTOR
    constexpr uint64_t OSXMMEXCPT = 1ULL << 10;  ///< دعم استثناءات SIMD
    constexpr uint64_t UMIP  = 1ULL << 11;  ///< حماية تعليمات وضع المستخدم / User-Mode Instruction Prevention
    constexpr uint64_t FSGSBASE = 1ULL << 16; ///< تفعيل FSGSBASE
    constexpr uint64_t OSXSAVE  = 1ULL << 18; ///< تفعيل XSAVE
    constexpr uint64_t SMEP  = 1ULL << 20;  ///< حماية التنفيذ من وضع المشرف / Supervisor Mode Exec Prevention
    constexpr uint64_t SMAP  = 1ULL << 21;  ///< حماية الوصول من وضع المشرف / Supervisor Mode Access Prevention
}

// ============================================================================
// ثوابت EFER MSR / EFER MSR Constants
// ============================================================================

namespace EFERFlags {
    constexpr uint64_t SCE  = 1ULL << 0;   ///< تفعيل استدعاء النظام / System Call Enable
    constexpr uint64_t LME  = 1ULL << 8;   ///< تفعيل الوضع الطويل / Long Mode Enable
    constexpr uint64_t LMA  = 1ULL << 10;  ///< الوضع الطويل نشط / Long Mode Active
    constexpr uint64_t NXE  = 1ULL << 11;  ///< تفعيل عدم التنفيذ / No-Execute Enable
}

namespace MSRAddresses {
    constexpr uint32_t IA32_EFER        = 0xC0000080;  ///< سجل EFER
    constexpr uint32_t IA32_STAR        = 0xC0000081;  ///< محددات النجمة / Star selectors
    constexpr uint32_t IA32_LSTAR       = 0xC0000082;  ///< عنوان SYSCALL في الوضع الطويل / Long mode SYSCALL target
    constexpr uint32_t IA32_CSTAR       = 0xC0000083;  ///< عنوان SYSCALL في وضع التوافق / Compat mode SYSCALL target
    constexpr uint32_t IA32_SFMASK      = 0xC0000084;  ///< قناع RFLAGS لـ SYSCALL
    constexpr uint32_t IA32_FS_BASE     = 0xC0000100;  ///< قاعدة FS
    constexpr uint32_t IA32_GS_BASE     = 0xC0000101;  ///< قاعدة GS
    constexpr uint32_t IA32_KERNEL_GS_BASE = 0xC0000102; ///< قاعدة GS للنواة / Kernel GS Base
    constexpr uint32_t IA32_APIC_BASE   = 0x0000001B;  ///< قاعدة APIC
    constexpr uint32_t IA32_PAT         = 0x00000277;  ///< جدول سمات الصفحة / Page Attribute Table
    constexpr uint32_t IA32_MTRR_DEF_TYPE = 0x000002FF; ///< نوع MTRR الافتراضي
}

// ============================================================================
// بنية معلومات CPUID / CPUID Information Structure
// ============================================================================

struct CPUIDResult {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
};

// ============================================================================
// ميزات المعالج / CPU Features
// ============================================================================

struct CPUFeatures {
    // ميزات أساسية من CPUID.01H:EDX
    bool fpu;        ///< وحدة النقطة العائمة / Floating Point Unit
    bool vme;        ///< امتدادات الوضع الافتراضي / Virtual 8086 Mode Extensions
    bool de;         ///< امتدادات التنقيح / Debugging Extensions
    bool pse;        ///< امتداد حجم الصفحة / Page Size Extension
    bool tsc;        ///< عداد الختم الزمني / Time Stamp Counter
    bool msr;        ///< سجلات النموذج المحدد / Model Specific Registers
    bool pae;        ///< امتداد العنوان المادي / Physical Address Extension
    bool mce;        ///< فحص آلة / Machine Check Exception
    bool cx8;        ///< تعليمة CMPXCHG8B
    bool apic;       ///< APIC مدمج / On-chip APIC
    bool sep;        ///< SYSENTER/SYSEXIT
    bool mtrr;       ///< سجلات نطاق نوع الذاكرة / Memory Type Range Registers
    bool pge;        ///< صفحة عامة / Page Global Enable
    bool mca;        ///< معمارية فحص الآلة / Machine Check Architecture
    bool cmov;       ///< نقل شرطي / Conditional Move
    bool pat;        ///< جدول سمات الصفحة / Page Attribute Table
    bool pse36;      ///< امتداد حجم صفحة 36-بت / 36-bit Page Size Extension
    bool mmx;        ///< امتدادات الوسائط المتعددة / MultiMedia Extensions
    bool fxsr;       ///< FXSAVE/FXRSTOR
    bool sse;        ///< امتدادات SIMD المتدفقة / Streaming SIMD Extensions
    bool sse2;       ///< SSE2

    // ميزات أساسية من CPUID.01H:ECX
    bool sse3;       ///< SSE3
    bool pclmulqdq;  ///< ضرب بدون حمل / Carry-less Multiplication
    bool ssse3;      ///< SSSE3 التكميلية / Supplemental SSE3
    bool fma;        ///< الضرب والجمع المدمج / Fused Multiply-Add
    bool cx16;       ///< CMPXCHG16B
    bool sse41;      ///< SSE4.1
    bool sse42;      ///< SSE4.2
    bool x2apic;     ///< x2APIC
    bool popcnt;     ///< عد البتات المعينة / Population Count
    bool aes;        ///< تعليمات AES
    bool xsave;      ///< XSAVE/XRSTOR
    bool avx;        ///< امتدادات المتجهات المتقدمة / Advanced Vector Extensions
    bool f16c;       ///< تحويلات النقطة العائمة 16-بت / 16-bit FP Conversions
    bool rdrand;     ///< أرقام عشوائية من العتاد / Hardware Random Numbers
    bool hypervisor; ///< يعمل داخل آلة افتراضية / Running in hypervisor

    // ميزات موسعة من CPUID.80000001H:EDX
    bool syscall;    ///< SYSCALL/SYSRET
    bool nx;         ///< عدم التنفيذ / No-Execute
    bool gbPages;    ///< صفحات 1GB / 1GB Pages
    bool rdtscp;     ///< RDTSCP
    bool longMode;   ///< الوضع الطويل (64-بت) / Long Mode (64-bit)

    // ميزات موسعة من CPUID.07H
    bool avx2;       ///< AVX2
    bool bmi1;       ///< تعليمات التلاعب بالبتات 1 / Bit Manipulation Instructions 1
    bool bmi2;       ///< تعليمات التلاعب بالبتات 2 / Bit Manipulation Instructions 2
    bool avx512f;    ///< AVX-512 الأساسية / AVX-512 Foundation
    bool rdseed;     ///< RDSEED
    bool smap;       ///< حماية الوصول من وضع المشرف / Supervisor Mode Access Prevention
    bool smep;       ///< حماية التنفيذ من وضع المشرف / Supervisor Mode Execution Prevention

    CPUFeatures();
};

// ============================================================================
// معلومات المعالج / CPU Information
// ============================================================================

struct CPUInfo {
    std::string vendorId;        ///< معرف الشركة المصنعة / Vendor ID
    std::string brandString;     ///< اسم المعالج / Brand String
    uint32_t family;             ///< العائلة / Family
    uint32_t model;              ///< النموذج / Model
    uint32_t stepping;           ///< الخطوة / Stepping
    uint32_t maxBasicLeaf;       ///< أعلى ورقة CPUID أساسية / Maximum basic CPUID leaf
    uint32_t maxExtendedLeaf;    ///< أعلى ورقة CPUID موسعة / Maximum extended CPUID leaf
    uint32_t logicalProcessors;  ///< عدد المعالجات المنطقية / Logical processors count
    uint32_t physicalCores;      ///< عدد النوى المادية / Physical cores count
    uint32_t cacheLineSize;      ///< حجم خط الذاكرة المخبئية / Cache line size in bytes
    CPUFeatures features;        ///< ميزات المعالج / CPU features
};

// ============================================================================
// مدير المعالج المركزي / CPU Manager
// ============================================================================

class CPUManager {
public:
    /**
     * الحصول على المثيل الوحيد
     * Get singleton instance
     */
    static CPUManager& getInstance();

    /**
     * تهيئة وكشف معلومات المعالج
     * Initialize and detect CPU information
     */
    void initialize();

    /**
     * الحصول على معلومات المعالج
     * Get CPU information
     */
    const CPUInfo& getInfo() const { return info_; }

    /**
     * الحصول على ميزات المعالج
     * Get CPU features
     */
    const CPUFeatures& getFeatures() const { return info_.features; }

    // ========================================================================
    // تعليمة CPUID / CPUID Instruction
    // ========================================================================

    /**
     * تنفيذ تعليمة CPUID
     * Execute CPUID instruction
     *
     * @param leaf رقم الورقة / Leaf number
     * @param subleaf رقم الورقة الفرعية / Sub-leaf number
     * @return نتيجة CPUID / CPUID result
     */
    static CPUIDResult cpuid(uint32_t leaf, uint32_t subleaf = 0);

    // ========================================================================
    // سجلات التحكم / Control Registers
    // ========================================================================

    static uint64_t readCR0();
    static void writeCR0(uint64_t value);
    static uint64_t readCR2();
    static uint64_t readCR3();
    static void writeCR3(uint64_t value);
    static uint64_t readCR4();
    static void writeCR4(uint64_t value);

    // ========================================================================
    // سجلات النموذج المحدد / Model Specific Registers
    // ========================================================================

    /**
     * قراءة سجل MSR
     * Read MSR register
     *
     * @param msr عنوان السجل / Register address
     * @return القيمة / Value
     */
    static uint64_t readMSR(uint32_t msr);

    /**
     * كتابة سجل MSR
     * Write MSR register
     *
     * @param msr عنوان السجل / Register address
     * @param value القيمة / Value
     */
    static void writeMSR(uint32_t msr, uint64_t value);

    // ========================================================================
    // تعليمات خاصة / Special Instructions
    // ========================================================================

    /**
     * إيقاف المعالج حتى المقاطعة التالية
     * Halt CPU until next interrupt
     */
    static void halt();

    /**
     * تعطيل المقاطعات
     * Disable interrupts
     */
    static void disableInterrupts();

    /**
     * تفعيل المقاطعات
     * Enable interrupts
     */
    static void enableInterrupts();

    /**
     * قراءة عداد الختم الزمني
     * Read Time Stamp Counter
     *
     * @return قيمة عداد TSC / TSC counter value
     */
    static uint64_t readTSC();

    /**
     * إبطال صفحة TLB
     * Invalidate TLB page
     *
     * @param virtualAddress العنوان الافتراضي / Virtual address
     */
    static void invlpg(uint64_t virtualAddress);

    /**
     * إبطال جميع صفحات TLB
     * Flush entire TLB
     */
    static void flushTLB();

    /**
     * تبديل مكدس النواة
     * Switch kernel stack
     *
     * @param newStack عنوان قمة المكدس الجديد / New stack top address
     */
    static void switchStack(uint64_t newStack);

    /**
     * إنشاء تقرير المعالج
     * Generate CPU report
     *
     * @return تقرير نصي / Text report
     */
    std::string generateReport() const;

private:
    CPUManager();
    ~CPUManager() = default;
    CPUManager(const CPUManager&) = delete;
    CPUManager& operator=(const CPUManager&) = delete;

    void detectVendor();
    void detectBrand();
    void detectFamily();
    void detectFeatures();
    void detectTopology();

    CPUInfo info_;
    bool isInitialized_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_CPU_H
