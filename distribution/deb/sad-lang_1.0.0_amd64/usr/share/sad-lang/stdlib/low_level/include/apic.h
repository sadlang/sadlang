/*
 * ============================================================================
 * وحدة APIC المتقدم - لغة ص
 * Advanced Programmable Interrupt Controller Module - Sad Language
 *
 * @brief (AR) واجهة برمجة Local APIC و I/O APIC لنظام تشغيل x86-64
 * @brief (EN) Local APIC and I/O APIC programming interface for x86-64 OS
 *
 * يدعم: تهيئة APIC، إرسال المقاطعات بين المعالجات (IPI)، مؤقت APIC،
 *       جدول إعادة التوجيه في I/O APIC، EOI، أوضاع التسليم المختلفة
 * ============================================================================
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Sad {
namespace LowLevel {

// ============================================================================
// سجلات Local APIC / Local APIC Registers
// ============================================================================

/// إزاحات سجلات Local APIC / Local APIC register offsets (relative to base)
namespace APICRegisters {
    static constexpr uint32_t ID               = 0x020;  // معرّف APIC / APIC ID
    static constexpr uint32_t VERSION           = 0x030;  // الإصدار / Version
    static constexpr uint32_t TPR               = 0x080;  // أولوية المهمة / Task Priority
    static constexpr uint32_t APR               = 0x090;  // أولوية التحكيم / Arbitration Priority
    static constexpr uint32_t PPR               = 0x0A0;  // أولوية المعالج / Processor Priority
    static constexpr uint32_t EOI               = 0x0B0;  // نهاية المقاطعة / End of Interrupt
    static constexpr uint32_t RRD               = 0x0C0;  // وجهة القراءة البعيدة / Remote Read
    static constexpr uint32_t LDR               = 0x0D0;  // الوجهة المنطقية / Logical Destination
    static constexpr uint32_t DFR               = 0x0E0;  // تنسيق الوجهة / Destination Format
    static constexpr uint32_t SVR               = 0x0F0;  // المتجه الزائف / Spurious Interrupt Vector
    static constexpr uint32_t ISR_BASE          = 0x100;  // حالة الخدمة / In-Service (8 regs)
    static constexpr uint32_t TMR_BASE          = 0x180;  // وضع التشغيل / Trigger Mode (8 regs)
    static constexpr uint32_t IRR_BASE          = 0x200;  // طلب المقاطعة / Interrupt Request (8 regs)
    static constexpr uint32_t ESR               = 0x280;  // حالة الخطأ / Error Status
    static constexpr uint32_t LVT_CMCI          = 0x2F0;  // CMCI LVT
    static constexpr uint32_t ICR_LOW           = 0x300;  // أمر المقاطعة (سفلي) / ICR Low
    static constexpr uint32_t ICR_HIGH          = 0x310;  // أمر المقاطعة (علوي) / ICR High
    static constexpr uint32_t LVT_TIMER         = 0x320;  // مؤقت LVT / LVT Timer
    static constexpr uint32_t LVT_THERMAL       = 0x330;  // حراري LVT / LVT Thermal
    static constexpr uint32_t LVT_PERFMON       = 0x340;  // أداء LVT / LVT Performance
    static constexpr uint32_t LVT_LINT0         = 0x350;  // LINT0 LVT
    static constexpr uint32_t LVT_LINT1         = 0x360;  // LINT1 LVT
    static constexpr uint32_t LVT_ERROR         = 0x370;  // خطأ LVT / LVT Error
    static constexpr uint32_t TIMER_ICR         = 0x380;  // العد الأولي للمؤقت / Timer Initial Count
    static constexpr uint32_t TIMER_CCR         = 0x390;  // العد الحالي للمؤقت / Timer Current Count
    static constexpr uint32_t TIMER_DCR         = 0x3E0;  // مقسم المؤقت / Timer Divide Config
}

// ============================================================================
// أنماط وثوابت / Modes and Constants
// ============================================================================

/// وضع تسليم المقاطعة / Interrupt delivery mode
enum class DeliveryMode : uint8_t {
    FIXED       = 0b000,  // ثابت / Fixed vector
    LOWEST      = 0b001,  // أدنى أولوية / Lowest priority
    SMI         = 0b010,  // مقاطعة إدارة النظام / System Management
    NMI         = 0b100,  // مقاطعة غير قابلة للإخفاء / Non-maskable
    INIT        = 0b101,  // تهيئة / INIT
    STARTUP     = 0b110,  // ابدأ / Start-up IPI (SIPI)
    EXT_INT     = 0b111   // خارجية / External
};

/// وضع مقسم المؤقت / Timer divider mode
enum class TimerDivide : uint8_t {
    DIV_1   = 0x0B,  // ÷1
    DIV_2   = 0x00,  // ÷2
    DIV_4   = 0x01,  // ÷4
    DIV_8   = 0x02,  // ÷8
    DIV_16  = 0x03,  // ÷16
    DIV_32  = 0x08,  // ÷32
    DIV_64  = 0x09,  // ÷64
    DIV_128 = 0x0A   // ÷128
};

/// وضع المؤقت / Timer mode
enum class TimerMode : uint8_t {
    ONE_SHOT    = 0,  // مرة واحدة / One-shot
    PERIODIC    = 1,  // دوري / Periodic
    TSC_DEADLINE = 2  // موعد TSC / TSC-Deadline
};

/// وضع استقطاب I/O APIC / I/O APIC polarity
enum class Polarity : uint8_t {
    ACTIVE_HIGH = 0,
    ACTIVE_LOW  = 1
};

/// وضع الحافة / Trigger mode
enum class TriggerMode : uint8_t {
    EDGE  = 0,  // بالحافة / Edge-triggered
    LEVEL = 1   // بالمستوى / Level-triggered
};

// ============================================================================
// بنى I/O APIC / I/O APIC Structures
// ============================================================================

/// سجلات I/O APIC / I/O APIC registers
namespace IOAPICRegisters {
    static constexpr uint32_t IOREGSEL = 0x00;  // اختيار السجل / Register Select
    static constexpr uint32_t IOWIN    = 0x10;  // نافذة البيانات / Data Window
    static constexpr uint32_t ID       = 0x00;  // معرّف / ID register index
    static constexpr uint32_t VER      = 0x01;  // إصدار / Version register index
    static constexpr uint32_t ARB      = 0x02;  // تحكيم / Arbitration register index
    static constexpr uint32_t REDTBL   = 0x10;  // جدول إعادة التوجيه / Redirection table base
}

/// مدخل جدول إعادة التوجيه / Redirection table entry
#pragma pack(push, 1)
struct IOAPICRedirectionEntry {
    uint8_t  vector;           // رقم المتجه / Vector number (0-255)
    DeliveryMode deliveryMode; // وضع التسليم / Delivery mode
    bool     logicalDest;      // وجهة منطقية / Logical destination mode
    bool     pending;          // معلقة / Delivery pending (read-only)
    Polarity polarity;         // الاستقطاب / Polarity
    bool     remoteIRR;        // IRR بعيد / Remote IRR (read-only, level only)
    TriggerMode trigger;       // وضع الحافة / Trigger mode
    bool     masked;           // مخفية / Masked
    uint8_t  destination;      // المعالج الوجهة / Destination APIC ID

    /// تحويل إلى قيمة 64 بت خام / Convert to raw 64-bit value
    uint64_t toRaw() const {
        uint64_t raw = 0;
        raw |= static_cast<uint64_t>(vector);
        raw |= static_cast<uint64_t>(deliveryMode) << 8;
        raw |= static_cast<uint64_t>(logicalDest ? 1 : 0) << 11;
        raw |= static_cast<uint64_t>(polarity) << 13;
        raw |= static_cast<uint64_t>(trigger) << 15;
        raw |= static_cast<uint64_t>(masked ? 1 : 0) << 16;
        raw |= static_cast<uint64_t>(destination) << 56;
        return raw;
    }

    /// تحويل من قيمة 64 بت خام / Convert from raw 64-bit value
    static IOAPICRedirectionEntry fromRaw(uint64_t raw) {
        IOAPICRedirectionEntry e;
        e.vector       = raw & 0xFF;
        e.deliveryMode = static_cast<DeliveryMode>((raw >> 8) & 0x7);
        e.logicalDest  = (raw >> 11) & 1;
        e.pending      = (raw >> 12) & 1;
        e.polarity     = static_cast<Polarity>((raw >> 13) & 1);
        e.remoteIRR    = (raw >> 14) & 1;
        e.trigger      = static_cast<TriggerMode>((raw >> 15) & 1);
        e.masked       = (raw >> 16) & 1;
        e.destination  = (raw >> 56) & 0xFF;
        return e;
    }
};
#pragma pack(pop)

/// معلومات I/O APIC / I/O APIC info
struct IOAPICInfo {
    uint8_t  id;                // معرّف / ID
    uint64_t baseAddress;       // العنوان الأساسي / MMIO base address
    uint32_t gsib;              // بداية المقاطعات العامة / Global System Interrupt Base
    uint8_t  maxRedirections;   // أقصى عدد إعادات التوجيه / Max redirections
    uint8_t  version;           // الإصدار / Version
};

/// معلومات Local APIC / Local APIC Info
struct LocalAPICInfo {
    uint8_t  id;             // معرّف APIC / APIC ID
    uint8_t  version;        // الإصدار / Version
    uint8_t  maxLVT;         // أقصى إدخال LVT / Max LVT entry
    bool     hasEOISuppress; // دعم كبت EOI / EOI broadcast suppression
    uint32_t busFrequency;   // تردد الناقل (هرتز) / Bus frequency (Hz)
};

// ============================================================================
// مدير APIC / APIC Manager
// ============================================================================

/**
 * @class APICManager
 * @brief (AR) مدير المقاطعات المتقدم - Local APIC و I/O APIC
 * @brief (EN) Advanced interrupt manager - Local APIC and I/O APIC
 */
class APICManager {
public:
    /// الحصول على المثيل الوحيد / Get singleton instance
    static APICManager& getInstance();

    // ==== تهيئة / Initialization ====

    /// تهيئة Local APIC / Initialize Local APIC
    bool initLocalAPIC(uint64_t baseAddress = 0xFEE00000);

    /// تهيئة I/O APIC / Initialize I/O APIC
    bool initIOAPIC(uint8_t id, uint64_t baseAddress, uint32_t gsib);

    /// تعطيل 8259 PIC القديم / Disable legacy 8259 PIC
    void disablePIC();

    /// إعادة تعيين عنوان APIC / Reset APIC base (safety for desktop)
    void resetLocalAPICBase() { localAPICBase_ = nullptr; localAPICEnabled_ = false; }

    /// إزالة آخر IO APIC مضاف (للأمان) / Pop last IO APIC (safety)
    void popLastIOAPIC() { if (!ioapics_.empty()) ioapics_.pop_back(); }

    /// هل APIC مدعوم عبر CPUID؟ / Is APIC supported via CPUID?
    bool isAPICSupported() const;

    /// هل x2APIC مدعوم؟ / Is x2APIC supported?
    bool isX2APICSupported() const;

    // ==== Local APIC ====

    /// قراءة سجل APIC / Read APIC register
    uint32_t readLocalAPIC(uint32_t reg) const;

    /// كتابة سجل APIC / Write APIC register
    void writeLocalAPIC(uint32_t reg, uint32_t value);

    /// الحصول على معرّف APIC الحالي / Get current APIC ID
    uint8_t getAPICId() const;

    /// إرسال EOI / Send End of Interrupt
    void sendEOI();

    /// تعيين أولوية المهمة / Set task priority
    void setTaskPriority(uint8_t priority);

    /// الحصول على معلومات APIC المحلي / Get local APIC info
    LocalAPICInfo getLocalAPICInfo() const;

    // ==== مؤقت APIC / APIC Timer ====

    /// تهيئة مؤقت APIC / Initialize APIC timer
    void initTimer(uint8_t vector, TimerMode mode, TimerDivide divide);

    /// بدء المؤقت بعدد أولي / Start timer with initial count
    void startTimer(uint32_t initialCount);

    /// إيقاف المؤقت / Stop timer
    void stopTimer();

    /// الحصول على العداد الحالي / Get current timer count
    uint32_t getTimerCount() const;

    /// معايرة المؤقت (بالهرتز المطلوب) / Calibrate timer for desired Hz
    uint32_t calibrateTimer(uint32_t desiredHz);

    // ==== LVT ====

    /// تهيئة LINT0 (عادة ExtINT) / Configure LINT0
    void configureLINT0(uint8_t vector, DeliveryMode mode, bool masked);

    /// تهيئة LINT1 (عادة NMI) / Configure LINT1
    void configureLINT1(uint8_t vector, DeliveryMode mode, bool masked);

    /// تهيئة مقاطعة الخطأ / Configure error interrupt
    void configureErrorLVT(uint8_t vector);

    // ==== IPI (مقاطعات بين المعالجات) / Inter-Processor Interrupts ====

    /// إرسال IPI إلى معالج محدد / Send IPI to specific processor
    void sendIPI(uint8_t destAPICId, uint8_t vector, DeliveryMode mode);

    /// إرسال IPI إلى كل المعالجات / Send IPI to all processors
    void sendIPIToAll(uint8_t vector, bool includeSelf);

    /// إرسال INIT IPI / Send INIT IPI
    void sendINIT(uint8_t destAPICId);

    /// إرسال SIPI / Send Startup IPI
    void sendSIPI(uint8_t destAPICId, uint8_t pageNumber);

    /// انتظار انتهاء التسليم / Wait for delivery complete
    void waitForDelivery();

    // ==== I/O APIC ====

    /// قراءة سجل I/O APIC / Read I/O APIC register
    uint32_t readIOAPIC(uint32_t index, uint32_t reg) const;

    /// كتابة سجل I/O APIC / Write I/O APIC register
    void writeIOAPIC(uint32_t index, uint32_t reg, uint32_t value);

    /// تعيين إعادة توجيه المقاطعة / Set interrupt redirection
    void setRedirection(uint32_t ioapicIndex, uint8_t irq,
                        const IOAPICRedirectionEntry& entry);

    /// الحصول على إعادة التوجيه / Get redirection entry
    IOAPICRedirectionEntry getRedirection(uint32_t ioapicIndex, uint8_t irq) const;

    /// إخفاء مقاطعة IRQ / Mask IRQ
    void maskIRQ(uint8_t irq);

    /// إظهار مقاطعة IRQ / Unmask IRQ
    void unmaskIRQ(uint8_t irq);

    /// توجيه IRQ إلى متجه ومعالج / Route IRQ to vector and CPU
    void routeIRQ(uint8_t irq, uint8_t vector, uint8_t destAPICId,
                  TriggerMode trigger = TriggerMode::EDGE,
                  Polarity polarity = Polarity::ACTIVE_HIGH);

    /// الحصول على عدد I/O APICs / Get I/O APIC count
    uint32_t getIOAPICCount() const { return static_cast<uint32_t>(ioapics_.size()); }

    /// الحصول على معلومات I/O APIC / Get I/O APIC info
    IOAPICInfo getIOAPICInfo(uint32_t index) const;

    // ==== أدوات / Utilities ====

    /// تحويل IRQ إلى GSI (مع مراعاة التجاوزات) / Convert IRQ to GSI
    uint32_t irqToGSI(uint8_t irq) const;

    /// إيجاد I/O APIC المسؤول عن GSI / Find I/O APIC handling GSI
    int32_t findIOAPICForGSI(uint32_t gsi) const;

    /// تعيين تجاوز مقاطعة (من MADT) / Set interrupt override
    void setInterruptOverride(uint8_t sourceIRQ, uint32_t gsi,
                              Polarity polarity, TriggerMode trigger);

    /// إنشاء تقرير / Generate report
    std::string generateReport() const;

private:
    APICManager() = default;
    APICManager(const APICManager&) = delete;
    APICManager& operator=(const APICManager&) = delete;

    /// عنوان MMIO الأساسي لـ Local APIC / Local APIC MMIO base address
    volatile uint32_t* localAPICBase_ = nullptr;
    uint64_t localAPICPhysBase_ = 0xFEE00000;
    bool localAPICEnabled_ = false;

    /// قائمة I/O APICs / I/O APIC list
    struct IOAPICState {
        IOAPICInfo info;
        volatile uint32_t* mmioBase;
    };
    std::vector<IOAPICState> ioapics_;

    /// تجاوز المقاطعات (من MADT) / Interrupt overrides from MADT
    struct InterruptOverride {
        uint8_t sourceIRQ;
        uint32_t gsi;
        Polarity polarity;
        TriggerMode trigger;
    };
    std::vector<InterruptOverride> overrides_;
};

// ============================================================================
// ثوابت APIC / APIC Constants
// ============================================================================

namespace APICConstants {
    /// عنوان APIC الافتراضي / Default APIC base address
    static constexpr uint64_t DEFAULT_LOCAL_APIC_BASE = 0xFEE00000;

    /// MSR لعنوان APIC / APIC base MSR
    static constexpr uint32_t IA32_APIC_BASE_MSR      = 0x1B;
    static constexpr uint64_t APIC_BASE_MSR_ENABLE    = (1ULL << 11);
    static constexpr uint64_t APIC_BASE_MSR_X2APIC    = (1ULL << 10);
    static constexpr uint64_t APIC_BASE_MSR_BSP       = (1ULL << 8);

    /// بتات SVR / SVR bits
    static constexpr uint32_t SVR_APIC_ENABLE   = (1 << 8);
    static constexpr uint32_t SVR_EOI_SUPPRESS  = (1 << 12);

    /// أرقام متجهات افتراضية / Default vector numbers
    static constexpr uint8_t SPURIOUS_VECTOR   = 0xFF;
    static constexpr uint8_t TIMER_VECTOR      = 0x20;
    static constexpr uint8_t ERROR_VECTOR      = 0xFE;
    static constexpr uint8_t THERMAL_VECTOR    = 0xFD;
    static constexpr uint8_t PERFMON_VECTOR    = 0xFC;

    /// بتات ICR / ICR bits
    static constexpr uint32_t ICR_DELIVERY_PENDING = (1 << 12);
    static constexpr uint32_t ICR_LEVEL_ASSERT     = (1 << 14);
    static constexpr uint32_t ICR_DEST_ALL         = (0x2 << 18);
    static constexpr uint32_t ICR_DEST_ALL_EX_SELF = (0x3 << 18);

    /// بتات LVT / LVT bits
    static constexpr uint32_t LVT_MASKED       = (1 << 16);
    static constexpr uint32_t LVT_TIMER_PERIODIC = (1 << 17);
    static constexpr uint32_t LVT_TIMER_TSC_DL   = (2 << 17);

    /// منافذ 8259 PIC / 8259 PIC ports
    static constexpr uint16_t PIC1_CMD  = 0x20;
    static constexpr uint16_t PIC1_DATA = 0x21;
    static constexpr uint16_t PIC2_CMD  = 0xA0;
    static constexpr uint16_t PIC2_DATA = 0xA1;

    /// تردد معايرة PIT / PIT calibration frequency
    static constexpr uint32_t PIT_FREQUENCY = 1193182;
}

} // namespace LowLevel
} // namespace Sad
