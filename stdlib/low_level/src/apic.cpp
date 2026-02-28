/*
 * ============================================================================
 * تنفيذ وحدة APIC المتقدم - لغة ص
 * APIC Module Implementation - Sad Language
 *
 * @brief (AR) تنفيذ Local APIC و I/O APIC لنواة نظام تشغيل x86-64
 * @brief (EN) Local APIC and I/O APIC implementation for x86-64 OS kernel
 * ============================================================================
 */

#include "apic.h"
#include <cstring>
#include <sstream>

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <cpuid.h>
#endif

namespace Sad {
namespace LowLevel {

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

APICManager& APICManager::getInstance() {
    static APICManager instance;
    return instance;
}

// ============================================================================
// أدوات مساعدة / Helper utilities
// ============================================================================

namespace {
    /// قراءة MSR / Read MSR
    inline uint64_t readMSR(uint32_t msr) {
        uint32_t lo, hi;
#ifdef _MSC_VER
        uint64_t val = __readmsr(msr);
        lo = static_cast<uint32_t>(val);
        hi = static_cast<uint32_t>(val >> 32);
#else
        __asm__ volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
#endif
        return (static_cast<uint64_t>(hi) << 32) | lo;
    }

    /// كتابة MSR / Write MSR
    inline void writeMSR(uint32_t msr, uint64_t value) {
        uint32_t lo = static_cast<uint32_t>(value);
        uint32_t hi = static_cast<uint32_t>(value >> 32);
#ifdef _MSC_VER
        __writemsr(msr, value);
#else
        __asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
#endif
    }

    /// إخراج بايت إلى منفذ / Output byte to port
    inline void outb(uint16_t port, uint8_t val) {
#ifdef _MSC_VER
        __outbyte(port, val);
#else
        __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
#endif
    }

    /// إدخال بايت من منفذ / Input byte from port
    inline uint8_t inb(uint16_t port) {
        uint8_t val;
#ifdef _MSC_VER
        val = __inbyte(port);
#else
        __asm__ volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
#endif
        return val;
    }

    /// تأخير بسيط / Simple I/O delay
    inline void ioWait() {
        outb(0x80, 0);
    }
}

// ============================================================================
// تهيئة / Initialization
// ============================================================================

bool APICManager::isAPICSupported() const {
    uint32_t eax, ebx, ecx, edx;
#ifdef _MSC_VER
    int regs[4];
    __cpuid(regs, 1);
    edx = regs[3];
#else
    __cpuid(1, eax, ebx, ecx, edx);
#endif
    return (edx & (1 << 9)) != 0; // CPUID.1:EDX.APIC[bit 9]
}

bool APICManager::isX2APICSupported() const {
    uint32_t eax, ebx, ecx, edx;
#ifdef _MSC_VER
    int regs[4];
    __cpuid(regs, 1);
    ecx = regs[2];
#else
    __cpuid(1, eax, ebx, ecx, edx);
#endif
    return (ecx & (1 << 21)) != 0; // CPUID.1:ECX.x2APIC[bit 21]
}

void APICManager::disablePIC() {
    // إعادة تعيين 8259 PIC مع إخفاء جميع المقاطعات
    // Remap 8259 PIC and mask all interrupts

    // ICW1: بدء التهيئة / Start init sequence
    outb(APICConstants::PIC1_CMD, 0x11);
    ioWait();
    outb(APICConstants::PIC2_CMD, 0x11);
    ioWait();

    // ICW2: إزاحة المتجهات / Vector offsets
    outb(APICConstants::PIC1_DATA, 0x20); // IRQ 0-7 → INT 0x20-0x27
    ioWait();
    outb(APICConstants::PIC2_DATA, 0x28); // IRQ 8-15 → INT 0x28-0x2F
    ioWait();

    // ICW3: ربط PIC / Cascade
    outb(APICConstants::PIC1_DATA, 4);
    ioWait();
    outb(APICConstants::PIC2_DATA, 2);
    ioWait();

    // ICW4: وضع 8086 / 8086 mode
    outb(APICConstants::PIC1_DATA, 0x01);
    ioWait();
    outb(APICConstants::PIC2_DATA, 0x01);
    ioWait();

    // إخفاء جميع المقاطعات / Mask all interrupts
    outb(APICConstants::PIC1_DATA, 0xFF);
    ioWait();
    outb(APICConstants::PIC2_DATA, 0xFF);
    ioWait();
}

bool APICManager::initLocalAPIC(uint64_t baseAddress) {
    if (!isAPICSupported()) return false;

    localAPICPhysBase_ = baseAddress;

    // تعيين العنوان الأساسي / Set base address
    // في نظام تشغيل حقيقي: يجب تعيين الذاكرة عبر صفحات الذاكرة
    // In real OS: must map via page tables
    localAPICBase_ = reinterpret_cast<volatile uint32_t*>(baseAddress);

    // تفعيل APIC عبر MSR / Enable APIC via MSR
    uint64_t apicBase = readMSR(APICConstants::IA32_APIC_BASE_MSR);
    apicBase |= APICConstants::APIC_BASE_MSR_ENABLE;
    apicBase = (apicBase & ~0xFFFULL) | (baseAddress & ~0xFFFULL);
    writeMSR(APICConstants::IA32_APIC_BASE_MSR, apicBase);

    // تفعيل وتعيين المتجه الزائف / Enable and set spurious vector
    uint32_t svr = readLocalAPIC(APICRegisters::SVR);
    svr |= APICConstants::SVR_APIC_ENABLE;
    svr = (svr & ~0xFF) | APICConstants::SPURIOUS_VECTOR;
    writeLocalAPIC(APICRegisters::SVR, svr);

    // تعيين أولوية المهمة إلى صفر (قبول جميع المقاطعات)
    // Set TPR to 0 (accept all interrupts)
    writeLocalAPIC(APICRegisters::TPR, 0);

    // تهيئة DFR إلى flat model / Set DFR to flat model
    writeLocalAPIC(APICRegisters::DFR, 0xFFFFFFFF);

    // تعيين LDR / Set LDR
    uint32_t ldr = readLocalAPIC(APICRegisters::LDR);
    ldr = (ldr & 0x00FFFFFF) | (1 << 24);
    writeLocalAPIC(APICRegisters::LDR, ldr);

    localAPICEnabled_ = true;
    return true;
}

bool APICManager::initIOAPIC(uint8_t id, uint64_t baseAddress, uint32_t gsib) {
    IOAPICState state;
    state.info.id = id;
    state.info.baseAddress = baseAddress;
    state.info.gsib = gsib;

    // تعيين عنوان MMIO / Set MMIO address
    state.mmioBase = reinterpret_cast<volatile uint32_t*>(baseAddress);

    // قراءة الإصدار / Read version
    uint32_t ioapicIndex = static_cast<uint32_t>(ioapics_.size());
    ioapics_.push_back(state);

    uint32_t ver = readIOAPIC(ioapicIndex, IOAPICRegisters::VER);
    ioapics_.back().info.version = ver & 0xFF;
    ioapics_.back().info.maxRedirections = ((ver >> 16) & 0xFF) + 1;

    // إخفاء جميع المقاطعات افتراضياً / Mask all interrupts by default
    for (uint8_t i = 0; i < ioapics_.back().info.maxRedirections; i++) {
        IOAPICRedirectionEntry entry = {};
        entry.vector = 0;
        entry.masked = true;
        entry.deliveryMode = DeliveryMode::FIXED;
        entry.trigger = TriggerMode::EDGE;
        entry.polarity = Polarity::ACTIVE_HIGH;
        entry.logicalDest = false;
        entry.destination = 0;
        setRedirection(ioapicIndex, i, entry);
    }

    return true;
}

// ============================================================================
// Local APIC / قراءة وكتابة السجلات
// ============================================================================

uint32_t APICManager::readLocalAPIC(uint32_t reg) const {
    if (!localAPICBase_) return 0;
    return localAPICBase_[reg / 4];
}

void APICManager::writeLocalAPIC(uint32_t reg, uint32_t value) {
    if (!localAPICBase_) return;
    localAPICBase_[reg / 4] = value;
}

uint8_t APICManager::getAPICId() const {
    return static_cast<uint8_t>(readLocalAPIC(APICRegisters::ID) >> 24);
}

void APICManager::sendEOI() {
    writeLocalAPIC(APICRegisters::EOI, 0);
}

void APICManager::setTaskPriority(uint8_t priority) {
    writeLocalAPIC(APICRegisters::TPR, priority);
}

LocalAPICInfo APICManager::getLocalAPICInfo() const {
    LocalAPICInfo info;
    info.id = getAPICId();
    uint32_t ver = readLocalAPIC(APICRegisters::VERSION);
    info.version = ver & 0xFF;
    info.maxLVT = ((ver >> 16) & 0xFF) + 1;
    info.hasEOISuppress = (ver & (1 << 24)) != 0;
    info.busFrequency = 0; // يحدد بعد المعايرة / Determined after calibration
    return info;
}

// ============================================================================
// مؤقت APIC / APIC Timer
// ============================================================================

void APICManager::initTimer(uint8_t vector, TimerMode mode, TimerDivide divide) {
    // تعيين مقسم المؤقت / Set timer divider
    writeLocalAPIC(APICRegisters::TIMER_DCR, static_cast<uint32_t>(divide));

    // تهيئة LVT Timer / Configure LVT Timer
    uint32_t lvt = static_cast<uint32_t>(vector);
    switch (mode) {
        case TimerMode::PERIODIC:
            lvt |= APICConstants::LVT_TIMER_PERIODIC;
            break;
        case TimerMode::TSC_DEADLINE:
            lvt |= APICConstants::LVT_TIMER_TSC_DL;
            break;
        case TimerMode::ONE_SHOT:
        default:
            break;
    }
    writeLocalAPIC(APICRegisters::LVT_TIMER, lvt);
}

void APICManager::startTimer(uint32_t initialCount) {
    writeLocalAPIC(APICRegisters::TIMER_ICR, initialCount);
}

void APICManager::stopTimer() {
    writeLocalAPIC(APICRegisters::TIMER_ICR, 0);
}

uint32_t APICManager::getTimerCount() const {
    return readLocalAPIC(APICRegisters::TIMER_CCR);
}

uint32_t APICManager::calibrateTimer(uint32_t desiredHz) {
    // المعايرة باستخدام PIT (القناة 2) / Calibrate using PIT (channel 2)
    // 1. تهيئة PIT لعد 10ms / Set PIT for 10ms countdown

    const uint32_t pitDiv = APICConstants::PIT_FREQUENCY / 100; // ~10ms

    // تعيين PIT القناة 2 / Set PIT channel 2
    outb(0x61, (inb(0x61) & 0xFD) | 1); // تفعيل بوابة / Enable gate
    outb(0x43, 0xB0);                      // القناة 2، وضع 0 / Ch2, mode 0
    outb(0x42, pitDiv & 0xFF);
    outb(0x42, (pitDiv >> 8) & 0xFF);

    // 2. بدء عداد APIC بأقصى قيمة / Start APIC counter at max
    writeLocalAPIC(APICRegisters::TIMER_DCR, static_cast<uint32_t>(TimerDivide::DIV_16));
    writeLocalAPIC(APICRegisters::TIMER_ICR, 0xFFFFFFFF);

    // 3. انتظار PIT / Wait for PIT
    while (!(inb(0x61) & 0x20)) {
        // انتظار / Waiting
    }

    // 4. قراءة عداد APIC / Read APIC counter
    uint32_t remaining = readLocalAPIC(APICRegisters::TIMER_CCR);
    uint32_t elapsed = 0xFFFFFFFF - remaining;

    // 5. عدد التكات في 10ms → تكات لكل ثانية → عد أولي للتردد المطلوب
    // Ticks in 10ms → ticks/sec → initial count for desired frequency
    uint32_t ticksPerSec = elapsed * 100;
    uint32_t initialCount = ticksPerSec / desiredHz;

    return initialCount;
}

// ============================================================================
// LVT
// ============================================================================

void APICManager::configureLINT0(uint8_t vector, DeliveryMode mode, bool masked) {
    uint32_t lvt = static_cast<uint32_t>(vector);
    lvt |= (static_cast<uint32_t>(mode) << 8);
    if (masked) lvt |= APICConstants::LVT_MASKED;
    writeLocalAPIC(APICRegisters::LVT_LINT0, lvt);
}

void APICManager::configureLINT1(uint8_t vector, DeliveryMode mode, bool masked) {
    uint32_t lvt = static_cast<uint32_t>(vector);
    lvt |= (static_cast<uint32_t>(mode) << 8);
    if (masked) lvt |= APICConstants::LVT_MASKED;
    writeLocalAPIC(APICRegisters::LVT_LINT1, lvt);
}

void APICManager::configureErrorLVT(uint8_t vector) {
    writeLocalAPIC(APICRegisters::LVT_ERROR, static_cast<uint32_t>(vector));
}

// ============================================================================
// IPI — مقاطعات بين المعالجات / Inter-Processor Interrupts
// ============================================================================

void APICManager::sendIPI(uint8_t destAPICId, uint8_t vector, DeliveryMode mode) {
    waitForDelivery();

    // تعيين ICR العلوي (المعالج الهدف) / Set ICR high (destination)
    writeLocalAPIC(APICRegisters::ICR_HIGH,
                   static_cast<uint32_t>(destAPICId) << 24);

    // تعيين ICR السفلي (المتجه + الوضع + assert + edge)
    // Set ICR low (vector + mode + assert + edge)
    uint32_t icr = static_cast<uint32_t>(vector);
    icr |= (static_cast<uint32_t>(mode) << 8);
    icr |= APICConstants::ICR_LEVEL_ASSERT;
    writeLocalAPIC(APICRegisters::ICR_LOW, icr);
}

void APICManager::sendIPIToAll(uint8_t vector, bool includeSelf) {
    waitForDelivery();

    writeLocalAPIC(APICRegisters::ICR_HIGH, 0);

    uint32_t icr = static_cast<uint32_t>(vector);
    icr |= (static_cast<uint32_t>(DeliveryMode::FIXED) << 8);
    icr |= APICConstants::ICR_LEVEL_ASSERT;
    icr |= includeSelf ? APICConstants::ICR_DEST_ALL
                       : APICConstants::ICR_DEST_ALL_EX_SELF;
    writeLocalAPIC(APICRegisters::ICR_LOW, icr);
}

void APICManager::sendINIT(uint8_t destAPICId) {
    sendIPI(destAPICId, 0, DeliveryMode::INIT);
}

void APICManager::sendSIPI(uint8_t destAPICId, uint8_t pageNumber) {
    waitForDelivery();

    writeLocalAPIC(APICRegisters::ICR_HIGH,
                   static_cast<uint32_t>(destAPICId) << 24);

    uint32_t icr = static_cast<uint32_t>(pageNumber);
    icr |= (static_cast<uint32_t>(DeliveryMode::STARTUP) << 8);
    icr |= APICConstants::ICR_LEVEL_ASSERT;
    writeLocalAPIC(APICRegisters::ICR_LOW, icr);
}

void APICManager::waitForDelivery() {
    // انتظار حتى يكتمل التسليم / Wait until delivery completes
    while (readLocalAPIC(APICRegisters::ICR_LOW) & APICConstants::ICR_DELIVERY_PENDING) {
        // انتظار / Waiting
#ifdef _MSC_VER
        _mm_pause();
#else
        __asm__ volatile("pause");
#endif
    }
}

// ============================================================================
// I/O APIC
// ============================================================================

uint32_t APICManager::readIOAPIC(uint32_t index, uint32_t reg) const {
    if (index >= ioapics_.size()) return 0;
    volatile uint32_t* base = ioapics_[index].mmioBase;
    if (!base) return 0;
    base[IOAPICRegisters::IOREGSEL / 4] = reg;
    return base[IOAPICRegisters::IOWIN / 4];
}

void APICManager::writeIOAPIC(uint32_t index, uint32_t reg, uint32_t value) {
    if (index >= ioapics_.size()) return;
    volatile uint32_t* base = ioapics_[index].mmioBase;
    if (!base) return;
    base[IOAPICRegisters::IOREGSEL / 4] = reg;
    base[IOAPICRegisters::IOWIN / 4] = value;
}

void APICManager::setRedirection(uint32_t ioapicIndex, uint8_t irq,
                                  const IOAPICRedirectionEntry& entry) {
    uint64_t raw = entry.toRaw();
    uint32_t regLow = IOAPICRegisters::REDTBL + irq * 2;
    uint32_t regHigh = regLow + 1;

    writeIOAPIC(ioapicIndex, regLow, static_cast<uint32_t>(raw));
    writeIOAPIC(ioapicIndex, regHigh, static_cast<uint32_t>(raw >> 32));
}

IOAPICRedirectionEntry APICManager::getRedirection(uint32_t ioapicIndex,
                                                     uint8_t irq) const {
    uint32_t regLow = IOAPICRegisters::REDTBL + irq * 2;
    uint32_t regHigh = regLow + 1;

    uint32_t lo = readIOAPIC(ioapicIndex, regLow);
    uint32_t hi = readIOAPIC(ioapicIndex, regHigh);
    uint64_t raw = (static_cast<uint64_t>(hi) << 32) | lo;

    return IOAPICRedirectionEntry::fromRaw(raw);
}

void APICManager::maskIRQ(uint8_t irq) {
    uint32_t gsi = irqToGSI(irq);
    int32_t ioapicIdx = findIOAPICForGSI(gsi);
    if (ioapicIdx < 0) return;

    uint8_t pin = static_cast<uint8_t>(gsi - ioapics_[ioapicIdx].info.gsib);
    auto entry = getRedirection(static_cast<uint32_t>(ioapicIdx), pin);
    entry.masked = true;
    setRedirection(static_cast<uint32_t>(ioapicIdx), pin, entry);
}

void APICManager::unmaskIRQ(uint8_t irq) {
    uint32_t gsi = irqToGSI(irq);
    int32_t ioapicIdx = findIOAPICForGSI(gsi);
    if (ioapicIdx < 0) return;

    uint8_t pin = static_cast<uint8_t>(gsi - ioapics_[ioapicIdx].info.gsib);
    auto entry = getRedirection(static_cast<uint32_t>(ioapicIdx), pin);
    entry.masked = false;
    setRedirection(static_cast<uint32_t>(ioapicIdx), pin, entry);
}

void APICManager::routeIRQ(uint8_t irq, uint8_t vector, uint8_t destAPICId,
                             TriggerMode trigger, Polarity polarity) {
    uint32_t gsi = irqToGSI(irq);
    int32_t ioapicIdx = findIOAPICForGSI(gsi);
    if (ioapicIdx < 0) return;

    uint8_t pin = static_cast<uint8_t>(gsi - ioapics_[ioapicIdx].info.gsib);

    IOAPICRedirectionEntry entry = {};
    entry.vector = vector;
    entry.deliveryMode = DeliveryMode::FIXED;
    entry.logicalDest = false;
    entry.polarity = polarity;
    entry.trigger = trigger;
    entry.masked = false;
    entry.destination = destAPICId;

    setRedirection(static_cast<uint32_t>(ioapicIdx), pin, entry);
}

IOAPICInfo APICManager::getIOAPICInfo(uint32_t index) const {
    if (index >= ioapics_.size()) return {};
    return ioapics_[index].info;
}

// ============================================================================
// أدوات ومساعدات / Utilities
// ============================================================================

uint32_t APICManager::irqToGSI(uint8_t irq) const {
    // التحقق من وجود تجاوز / Check for override
    for (const auto& ovr : overrides_) {
        if (ovr.sourceIRQ == irq) {
            return ovr.gsi;
        }
    }
    // بدون تجاوز: IRQ = GSI / No override: IRQ = GSI
    return static_cast<uint32_t>(irq);
}

int32_t APICManager::findIOAPICForGSI(uint32_t gsi) const {
    for (size_t i = 0; i < ioapics_.size(); i++) {
        const auto& io = ioapics_[i];
        if (gsi >= io.info.gsib &&
            gsi < io.info.gsib + io.info.maxRedirections) {
            return static_cast<int32_t>(i);
        }
    }
    return -1;
}

void APICManager::setInterruptOverride(uint8_t sourceIRQ, uint32_t gsi,
                                        Polarity polarity, TriggerMode trigger) {
    // استبدال إن وجد / Replace if exists
    for (auto& ovr : overrides_) {
        if (ovr.sourceIRQ == sourceIRQ) {
            ovr.gsi = gsi;
            ovr.polarity = polarity;
            ovr.trigger = trigger;
            return;
        }
    }
    overrides_.push_back({sourceIRQ, gsi, polarity, trigger});
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string APICManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير APIC / APIC Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "Local APIC:\n";
    if (localAPICEnabled_) {
        auto info = getLocalAPICInfo();
        report << "  معرّف / ID: " << static_cast<int>(info.id) << "\n";
        report << "  إصدار / Version: 0x" << std::hex << static_cast<int>(info.version) << std::dec << "\n";
        report << "  أقصى LVT: " << static_cast<int>(info.maxLVT) << "\n";
        report << "  عنوان / Base: 0x" << std::hex << localAPICPhysBase_ << std::dec << "\n";
    } else {
        report << "  غير مفعّل / Not enabled\n";
    }

    report << "\nI/O APICs: " << ioapics_.size() << "\n";
    for (size_t i = 0; i < ioapics_.size(); i++) {
        const auto& io = ioapics_[i];
        report << "  [" << i << "] ID=" << static_cast<int>(io.info.id)
               << " عنوان / Base=0x" << std::hex << io.info.baseAddress << std::dec
               << " GSI Base=" << io.info.gsib
               << " Pins=" << static_cast<int>(io.info.maxRedirections)
               << " Ver=0x" << std::hex << static_cast<int>(io.info.version) << std::dec << "\n";
    }

    if (!overrides_.empty()) {
        report << "\nتجاوزات المقاطعات / Interrupt Overrides: " << overrides_.size() << "\n";
        for (const auto& ovr : overrides_) {
            report << "  IRQ " << static_cast<int>(ovr.sourceIRQ)
                   << " → GSI " << ovr.gsi
                   << " (حافة / Edge=" << (ovr.trigger == TriggerMode::EDGE ? "نعم" : "لا")
                   << ")\n";
        }
    }

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
