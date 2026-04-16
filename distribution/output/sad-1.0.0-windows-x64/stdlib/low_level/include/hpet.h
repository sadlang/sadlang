/*
 * ============================================================================
 * وحدة مؤقت HPET عالي الدقة - لغة ص
 * High Precision Event Timer Module - Sad Language
 *
 * @brief (AR) واجهة برمجة HPET لتوقيت دقيق في نظام تشغيل x86-64
 * @brief (EN) HPET programming interface for precise timing in x86-64 OS
 *
 * يدعم: تهيئة HPET من جداول ACPI، المؤقت الرئيسي، المقارنات الدورية
 *       والمرة الواحدة، تأخيرات نانوثانية، قياس الوقت المنقضي
 * ============================================================================
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Sad {
namespace LowLevel {

// ============================================================================
// سجلات HPET / HPET Registers
// ============================================================================

namespace HPETRegisters {
    static constexpr uint64_t GCAP_ID           = 0x000;  // القدرات والمعرّف / Capabilities & ID
    static constexpr uint64_t GEN_CONF          = 0x010;  // التهيئة العامة / General Configuration
    static constexpr uint64_t GEN_INT_STS       = 0x020;  // حالة المقاطعة العامة / General Interrupt Status
    static constexpr uint64_t MAIN_COUNTER      = 0x0F0;  // العداد الرئيسي / Main Counter Value

    // المقارنات / Comparators (N = 0, 1, 2, ...)
    static constexpr uint64_t TIMER_CONF(uint8_t n) {
        return 0x100 + 0x20 * n;
    }
    static constexpr uint64_t TIMER_CMP(uint8_t n) {
        return 0x108 + 0x20 * n;
    }
    static constexpr uint64_t TIMER_FSB(uint8_t n) {
        return 0x110 + 0x20 * n;
    }
}

// ============================================================================
// بنى HPET / HPET Structures
// ============================================================================

/// بتات سجل القدرات / Capabilities register bits
#pragma pack(push, 1)
struct HPETCapabilities {
    uint8_t  revisionId;       // معرّف المراجعة / Revision ID
    uint8_t  numTimers;        // عدد المقارنات - 1 / Number of timers - 1
    bool     is64Bit;          // عداد 64 بت / 64-bit counter
    bool     legacyReplace;    // بديل القديم / Legacy replacement capable
    uint16_t vendorId;         // معرّف المصنّع / Vendor ID
    uint32_t period;           // فترة العد (فيمتوثانية) / Counter period in femtoseconds

    /// حساب التردد بالهرتز / Calculate frequency in Hz
    uint64_t frequencyHz() const {
        if (period == 0) return 0;
        return 1000000000000000ULL / period; // fs → Hz
    }
};
#pragma pack(pop)

/// تهيئة مقارن / Timer configuration
struct HPETTimerConfig {
    uint8_t  index;            // رقم المقارن / Timer index
    bool     enabled;          // مفعّل / Enabled
    bool     periodic;         // دوري / Periodic mode
    bool     is64Bit;          // عداد 64 بت / 64-bit capable
    bool     fsbEnabled;       // FSB Interrupt Delivery
    uint8_t  irqRoute;         // توجيه IRQ / IRQ routing
    uint32_t routingCapBits;   // بتات قدرة التوجيه / Route capability bits
    uint64_t comparatorValue;  // قيمة المقارن / Comparator value
};

/// معلومات HPET / HPET information
struct HPETInfo {
    HPETCapabilities caps;     // القدرات / Capabilities
    uint64_t baseAddress;      // العنوان الأساسي MMIO / MMIO base address
    uint64_t frequency;        // التردد (هرتز) / Frequency in Hz
    uint8_t  timerCount;       // عدد المقارنات / Number of timers
    bool     isRunning;        // هل يعمل / Is running
};

// ============================================================================
// مدير HPET / HPET Manager
// ============================================================================

/**
 * @class HPETManager
 * @brief (AR) مدير المؤقت عالي الدقة HPET
 * @brief (EN) High Precision Event Timer manager
 */
class HPETManager {
public:
    /// الحصول على المثيل الوحيد / Get singleton instance
    static HPETManager& getInstance();

    // ==== تهيئة / Initialization ====

    /// تهيئة HPET من عنوان ACPI / Initialize HPET from ACPI base address
    bool init(uint64_t baseAddress);

    /// هل HPET مهيأ؟ / Is HPET initialized?
    bool isInitialized() const { return isInitialized_; }

    /// الحصول على معلومات HPET / Get HPET info
    HPETInfo getInfo() const;

    // ==== العداد الرئيسي / Main Counter ====

    /// تفعيل العداد الرئيسي / Enable main counter
    void enable();

    /// تعطيل العداد الرئيسي / Disable main counter
    void disable();

    /// إعادة تعيين العداد / Reset counter to zero
    void resetCounter();

    /// قراءة العداد الرئيسي / Read main counter value
    uint64_t readCounter() const;

    /// الحصول على تردد HPET (هرتز) / Get HPET frequency in Hz
    uint64_t getFrequency() const { return frequency_; }

    /// الحصول على فترة الحساب (فيمتوثانية) / Get counter period in femtoseconds
    uint32_t getPeriod() const { return period_; }

    // ==== التأخير والتوقيت / Delay and Timing ====

    /// تأخير بالنانوثانية / Delay in nanoseconds
    void delayNanoseconds(uint64_t ns) const;

    /// تأخير بالمايكروثانية / Delay in microseconds
    void delayMicroseconds(uint64_t us) const;

    /// تأخير بالميلي ثانية / Delay in milliseconds
    void delayMilliseconds(uint64_t ms) const;

    /// قياس الوقت المنقضي بالنانوثانية / Measure elapsed nanoseconds
    uint64_t elapsedNanoseconds(uint64_t startCount) const;

    /// تحويل عدات إلى نانوثانية / Convert ticks to nanoseconds
    uint64_t ticksToNanoseconds(uint64_t ticks) const;

    /// تحويل نانوثانية إلى عدات / Convert nanoseconds to ticks
    uint64_t nanosecondsToTicks(uint64_t ns) const;

    // ==== المقارنات / Comparators ====

    /// الحصول على عدد المقارنات / Get number of timers
    uint8_t getTimerCount() const { return timerCount_; }

    /// تهيئة مقارن لمقاطعة مرة واحدة / Configure one-shot timer interrupt
    bool configureOneShotTimer(uint8_t timer, uint8_t irq,
                                uint64_t delayNs, uint8_t vector);

    /// تهيئة مقارن لمقاطعة دورية / Configure periodic timer interrupt
    bool configurePeriodicTimer(uint8_t timer, uint8_t irq,
                                 uint64_t periodNs, uint8_t vector);

    /// إيقاف مقارن / Stop a timer
    void stopTimer(uint8_t timer);

    /// الحصول على تهيئة مقارن / Get timer configuration
    HPETTimerConfig getTimerConfig(uint8_t timer) const;

    /// مسح حالة المقاطعة / Clear interrupt status
    void clearInterruptStatus(uint8_t timer);

    /// هل المقاطعة معلقة؟ / Is interrupt pending?
    bool isInterruptPending(uint8_t timer) const;

    // ==== الوضع القديم / Legacy Replacement ====

    /// تفعيل بديل IRQ0/IRQ8 القديم / Enable legacy IRQ0/IRQ8 replacement
    void enableLegacyReplacement();

    /// تعطيل بديل القديم / Disable legacy replacement
    void disableLegacyReplacement();

    // ==== التقرير / Report ====

    /// إنشاء تقرير / Generate report
    std::string generateReport() const;

private:
    HPETManager() = default;
    HPETManager(const HPETManager&) = delete;
    HPETManager& operator=(const HPETManager&) = delete;

    /// قراءة سجل HPET / Read HPET register
    uint64_t readReg(uint64_t offset) const;

    /// كتابة سجل HPET / Write HPET register
    void writeReg(uint64_t offset, uint64_t value);

    volatile uint64_t* mmioBase_ = nullptr;
    uint64_t physBase_ = 0;
    uint32_t period_ = 0;        // فترة العد (فيمتوثانية) / Period in femtoseconds
    uint64_t frequency_ = 0;     // التردد (هرتز) / Frequency in Hz
    uint8_t  timerCount_ = 0;    // عدد المقارنات / Number of timers
    bool     is64Bit_ = false;   // عداد 64 بت / 64-bit counter
    bool     isInitialized_ = false;
    bool     isRunning_ = false;
};

// ============================================================================
// ثوابت HPET / HPET Constants
// ============================================================================

namespace HPETConstants {
    /// بتات التهيئة العامة / General configuration bits
    static constexpr uint64_t GEN_CONF_ENABLE    = (1ULL << 0);  // تفعيل العداد / Enable counter
    static constexpr uint64_t GEN_CONF_LEGACY    = (1ULL << 1);  // بديل قديم / Legacy replacement

    /// بتات تهيئة المقارن / Timer configuration bits
    static constexpr uint64_t TN_INT_TYPE_CNF     = (1ULL << 1);  // نوع المقاطعة مستوى / Int type (level)
    static constexpr uint64_t TN_INT_ENB_CNF      = (1ULL << 2);  // تفعيل المقاطعة / Int enable
    static constexpr uint64_t TN_TYPE_CNF         = (1ULL << 3);  // دوري / Periodic
    static constexpr uint64_t TN_PER_INT_CAP      = (1ULL << 4);  // قدرة دورية / Periodic capable
    static constexpr uint64_t TN_SIZE_CAP         = (1ULL << 5);  // عداد 64 بت / 64-bit capable
    static constexpr uint64_t TN_VAL_SET_CNF      = (1ULL << 6);  // تعيين القيمة / Value set
    static constexpr uint64_t TN_32MODE_CNF       = (1ULL << 8);  // وضع 32 بت إجباري / Force 32-bit
    static constexpr uint64_t TN_FSB_EN_CNF       = (1ULL << 14); // تفعيل FSB / FSB enable
    static constexpr uint64_t TN_FSB_INT_DEL_CAP  = (1ULL << 15); // قدرة FSB / FSB capable

    /// قناع توجيه IRQ / IRQ route mask
    static constexpr uint64_t TN_INT_ROUTE_MASK   = (0x1FULL << 9);
    static constexpr int      TN_INT_ROUTE_SHIFT  = 9;

    /// قدرة التوجيه / Route capability
    static constexpr uint64_t TN_INT_ROUTE_CAP_MASK  = 0xFFFFFFFF00000000ULL;
    static constexpr int      TN_INT_ROUTE_CAP_SHIFT = 32;

    /// ثوابت التحويل / Conversion constants
    static constexpr uint64_t FEMTOSECONDS_PER_NS = 1000000ULL;
    static constexpr uint64_t FEMTOSECONDS_PER_US = 1000000000ULL;
    static constexpr uint64_t FEMTOSECONDS_PER_MS = 1000000000000ULL;

    /// الحد الأدنى للتردد / Minimum HPET frequency (10 MHz)
    static constexpr uint64_t MIN_FREQUENCY = 10000000ULL;
}

} // namespace LowLevel
} // namespace Sad
