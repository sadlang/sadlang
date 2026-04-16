/*
 * ============================================================================
 * تنفيذ وحدة HPET - لغة ص
 * HPET Module Implementation - Sad Language
 *
 * @brief (AR) تنفيذ المؤقت عالي الدقة HPET لنظام تشغيل x86-64
 * @brief (EN) HPET implementation for x86-64 OS kernel
 * ============================================================================
 */

#include "hpet.h"
#include <sstream>
#include <iomanip>

namespace Sad {
namespace LowLevel {

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

HPETManager& HPETManager::getInstance() {
    static HPETManager instance;
    return instance;
}

// ============================================================================
// قراءة/كتابة السجلات / Register Read/Write
// ============================================================================

uint64_t HPETManager::readReg(uint64_t offset) const {
    if (!mmioBase_) return 0;
    return mmioBase_[offset / 8];
}

void HPETManager::writeReg(uint64_t offset, uint64_t value) {
    if (!mmioBase_) return;
    mmioBase_[offset / 8] = value;
}

// ============================================================================
// التهيئة / Initialization
// ============================================================================

bool HPETManager::init(uint64_t baseAddress) {
    physBase_ = baseAddress;

    // تعيين عنوان MMIO / Map MMIO address
    // في نظام حقيقي: يتم عبر صفحات الذاكرة / In real OS: via page tables
    mmioBase_ = reinterpret_cast<volatile uint64_t*>(baseAddress);

    // قراءة القدرات / Read capabilities
    uint64_t caps = readReg(HPETRegisters::GCAP_ID);

    // استخراج المعلومات / Extract info
    period_ = static_cast<uint32_t>(caps >> 32);
    if (period_ == 0) return false; // HPET غير صالح / Invalid HPET

    frequency_ = 1000000000000000ULL / period_; // فيمتوثانية → هرتز / fs → Hz
    if (frequency_ < HPETConstants::MIN_FREQUENCY) return false;

    timerCount_ = ((caps >> 8) & 0x1F) + 1;
    is64Bit_ = (caps & (1ULL << 13)) != 0;

    // إيقاف العداد أثناء التهيئة / Stop counter during init
    uint64_t conf = readReg(HPETRegisters::GEN_CONF);
    conf &= ~HPETConstants::GEN_CONF_ENABLE;
    writeReg(HPETRegisters::GEN_CONF, conf);

    // إعادة تعيين العداد / Reset counter
    writeReg(HPETRegisters::MAIN_COUNTER, 0);

    // إخفاء جميع المقارنات / Mask all timers
    for (uint8_t i = 0; i < timerCount_; i++) {
        uint64_t timerConf = readReg(HPETRegisters::TIMER_CONF(i));
        timerConf &= ~HPETConstants::TN_INT_ENB_CNF;
        writeReg(HPETRegisters::TIMER_CONF(i), timerConf);
    }

    // مسح حالة المقاطعات / Clear interrupt status
    writeReg(HPETRegisters::GEN_INT_STS, 0xFFFFFFFF);

    isInitialized_ = true;
    isRunning_ = false;
    return true;
}

HPETInfo HPETManager::getInfo() const {
    HPETInfo info;
    info.caps.revisionId = 0;
    info.caps.numTimers = timerCount_ - 1;
    info.caps.is64Bit = is64Bit_;
    info.caps.legacyReplace = false;
    info.caps.vendorId = 0;
    info.caps.period = period_;
    info.baseAddress = physBase_;
    info.frequency = frequency_;
    info.timerCount = timerCount_;
    info.isRunning = isRunning_;

    if (mmioBase_) {
        uint64_t caps = readReg(HPETRegisters::GCAP_ID);
        info.caps.revisionId = caps & 0xFF;
        info.caps.vendorId = (caps >> 16) & 0xFFFF;
        info.caps.legacyReplace = (caps & (1ULL << 15)) != 0;
    }

    return info;
}

// ============================================================================
// العداد الرئيسي / Main Counter
// ============================================================================

void HPETManager::enable() {
    if (!mmioBase_) return;
    uint64_t conf = readReg(HPETRegisters::GEN_CONF);
    conf |= HPETConstants::GEN_CONF_ENABLE;
    writeReg(HPETRegisters::GEN_CONF, conf);
    isRunning_ = true;
}

void HPETManager::disable() {
    if (!mmioBase_) return;
    uint64_t conf = readReg(HPETRegisters::GEN_CONF);
    conf &= ~HPETConstants::GEN_CONF_ENABLE;
    writeReg(HPETRegisters::GEN_CONF, conf);
    isRunning_ = false;
}

void HPETManager::resetCounter() {
    bool wasRunning = isRunning_;
    if (wasRunning) disable();
    writeReg(HPETRegisters::MAIN_COUNTER, 0);
    if (wasRunning) enable();
}

uint64_t HPETManager::readCounter() const {
    if (!mmioBase_) return 0;

    if (is64Bit_) {
        return readReg(HPETRegisters::MAIN_COUNTER);
    }

    // قراءة 32 بت بأمان لمنع التفاف / Safe 32-bit read to handle wraparound
    uint32_t lo1, hi1, hi2;
    do {
        hi1 = static_cast<uint32_t>(readReg(HPETRegisters::MAIN_COUNTER) >> 32);
        lo1 = static_cast<uint32_t>(readReg(HPETRegisters::MAIN_COUNTER));
        hi2 = static_cast<uint32_t>(readReg(HPETRegisters::MAIN_COUNTER) >> 32);
    } while (hi1 != hi2);

    return (static_cast<uint64_t>(hi1) << 32) | lo1;
}

// ============================================================================
// التأخير والتوقيت / Delay and Timing
// ============================================================================

void HPETManager::delayNanoseconds(uint64_t ns) const {
    if (!isRunning_ || !mmioBase_ || period_ == 0) return;

    uint64_t targetTicks = nanosecondsToTicks(ns);
    uint64_t start = readCounter();

    while ((readCounter() - start) < targetTicks) {
        // انتظار نشط / Active wait
#ifdef _MSC_VER
        _mm_pause();
#elif defined(__x86_64__) || defined(__i386__)
        __asm__ volatile("pause");
#elif defined(__aarch64__) || defined(__arm__)
        __asm__ volatile("yield");
#else
        // no-op on other architectures
#endif
    }
}

void HPETManager::delayMicroseconds(uint64_t us) const {
    delayNanoseconds(us * 1000);
}

void HPETManager::delayMilliseconds(uint64_t ms) const {
    delayNanoseconds(ms * 1000000);
}

uint64_t HPETManager::elapsedNanoseconds(uint64_t startCount) const {
    uint64_t current = readCounter();
    uint64_t elapsed = current - startCount;
    return ticksToNanoseconds(elapsed);
}

uint64_t HPETManager::ticksToNanoseconds(uint64_t ticks) const {
    if (period_ == 0) return 0;
    // ticks * period (fs) / 1,000,000 (fs/ns)
    return (ticks * period_) / HPETConstants::FEMTOSECONDS_PER_NS;
}

uint64_t HPETManager::nanosecondsToTicks(uint64_t ns) const {
    if (period_ == 0) return 0;
    // ns * 1,000,000 (fs/ns) / period (fs)
    return (ns * HPETConstants::FEMTOSECONDS_PER_NS) / period_;
}

// ============================================================================
// المقارنات / Comparators
// ============================================================================

bool HPETManager::configureOneShotTimer(uint8_t timer, uint8_t irq,
                                         uint64_t delayNs, uint8_t /*vector*/) {
    if (timer >= timerCount_ || !mmioBase_) return false;

    // إيقاف المقارن أولاً / Stop timer first
    stopTimer(timer);

    // قراءة التهيئة / Read config
    uint64_t conf = readReg(HPETRegisters::TIMER_CONF(timer));

    // التحقق من دعم التوجيه / Check route capability
    uint32_t routeCap = static_cast<uint32_t>(
        (conf & HPETConstants::TN_INT_ROUTE_CAP_MASK) >> HPETConstants::TN_INT_ROUTE_CAP_SHIFT);
    if (!(routeCap & (1 << irq))) return false;

    // تهيئة المقارن / Configure timer
    conf &= ~HPETConstants::TN_TYPE_CNF;                       // ليس دوري / Not periodic
    conf &= ~HPETConstants::TN_INT_ROUTE_MASK;
    conf |= (static_cast<uint64_t>(irq) << HPETConstants::TN_INT_ROUTE_SHIFT);
    conf |= HPETConstants::TN_INT_ENB_CNF;                     // تفعيل المقاطعة / Enable interrupt
    conf &= ~HPETConstants::TN_INT_TYPE_CNF;                   // حافة / Edge triggered

    writeReg(HPETRegisters::TIMER_CONF(timer), conf);

    // تعيين قيمة المقارن / Set comparator value
    uint64_t targetTicks = nanosecondsToTicks(delayNs);
    uint64_t currentCount = readCounter();
    writeReg(HPETRegisters::TIMER_CMP(timer), currentCount + targetTicks);

    return true;
}

bool HPETManager::configurePeriodicTimer(uint8_t timer, uint8_t irq,
                                           uint64_t periodNs, uint8_t /*vector*/) {
    if (timer >= timerCount_ || !mmioBase_) return false;

    // إيقاف المقارن / Stop timer
    stopTimer(timer);

    // قراءة التهيئة / Read config
    uint64_t conf = readReg(HPETRegisters::TIMER_CONF(timer));

    // التحقق من دعم الدوري / Check periodic capable
    if (!(conf & HPETConstants::TN_PER_INT_CAP)) return false;

    // التحقق من دعم التوجيه / Check route capability
    uint32_t routeCap = static_cast<uint32_t>(
        (conf & HPETConstants::TN_INT_ROUTE_CAP_MASK) >> HPETConstants::TN_INT_ROUTE_CAP_SHIFT);
    if (!(routeCap & (1 << irq))) return false;

    // تهيئة المقارن كدوري / Configure periodic
    conf |= HPETConstants::TN_TYPE_CNF;                        // دوري / Periodic
    conf |= HPETConstants::TN_VAL_SET_CNF;                     // تعيين القيمة / Value set
    conf &= ~HPETConstants::TN_INT_ROUTE_MASK;
    conf |= (static_cast<uint64_t>(irq) << HPETConstants::TN_INT_ROUTE_SHIFT);
    conf |= HPETConstants::TN_INT_ENB_CNF;                     // تفعيل المقاطعة / Enable interrupt
    conf &= ~HPETConstants::TN_INT_TYPE_CNF;                   // حافة / Edge triggered

    writeReg(HPETRegisters::TIMER_CONF(timer), conf);

    // تعيين الفترة / Set period
    uint64_t periodTicks = nanosecondsToTicks(periodNs);
    writeReg(HPETRegisters::TIMER_CMP(timer), periodTicks);

    return true;
}

void HPETManager::stopTimer(uint8_t timer) {
    if (timer >= timerCount_ || !mmioBase_) return;

    uint64_t conf = readReg(HPETRegisters::TIMER_CONF(timer));
    conf &= ~HPETConstants::TN_INT_ENB_CNF;
    writeReg(HPETRegisters::TIMER_CONF(timer), conf);
}

HPETTimerConfig HPETManager::getTimerConfig(uint8_t timer) const {
    HPETTimerConfig config = {};
    if (timer >= timerCount_ || !mmioBase_) return config;

    uint64_t conf = readReg(HPETRegisters::TIMER_CONF(timer));

    config.index = timer;
    config.enabled = (conf & HPETConstants::TN_INT_ENB_CNF) != 0;
    config.periodic = (conf & HPETConstants::TN_TYPE_CNF) != 0;
    config.is64Bit = (conf & HPETConstants::TN_SIZE_CAP) != 0;
    config.fsbEnabled = (conf & HPETConstants::TN_FSB_EN_CNF) != 0;
    config.irqRoute = static_cast<uint8_t>(
        (conf & HPETConstants::TN_INT_ROUTE_MASK) >> HPETConstants::TN_INT_ROUTE_SHIFT);
    config.routingCapBits = static_cast<uint32_t>(
        (conf & HPETConstants::TN_INT_ROUTE_CAP_MASK) >> HPETConstants::TN_INT_ROUTE_CAP_SHIFT);
    config.comparatorValue = readReg(HPETRegisters::TIMER_CMP(timer));

    return config;
}

void HPETManager::clearInterruptStatus(uint8_t timer) {
    if (timer >= timerCount_ || !mmioBase_) return;
    writeReg(HPETRegisters::GEN_INT_STS, 1ULL << timer);
}

bool HPETManager::isInterruptPending(uint8_t timer) const {
    if (timer >= timerCount_ || !mmioBase_) return false;
    return (readReg(HPETRegisters::GEN_INT_STS) & (1ULL << timer)) != 0;
}

// ============================================================================
// الوضع القديم / Legacy Replacement
// ============================================================================

void HPETManager::enableLegacyReplacement() {
    if (!mmioBase_) return;
    uint64_t conf = readReg(HPETRegisters::GEN_CONF);
    conf |= HPETConstants::GEN_CONF_LEGACY;
    writeReg(HPETRegisters::GEN_CONF, conf);
}

void HPETManager::disableLegacyReplacement() {
    if (!mmioBase_) return;
    uint64_t conf = readReg(HPETRegisters::GEN_CONF);
    conf &= ~HPETConstants::GEN_CONF_LEGACY;
    writeReg(HPETRegisters::GEN_CONF, conf);
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string HPETManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير HPET / HPET Report\n";
    report << std::string(70, '=') << "\n\n";

    if (!isInitialized_) {
        report << "HPET غير مهيأ / HPET not initialized\n";
        report << std::string(70, '=') << "\n\n";
        return report.str();
    }

    auto info = getInfo();

    report << "العنوان الأساسي / Base Address: 0x" << std::hex << info.baseAddress << std::dec << "\n";
    report << "المراجعة / Revision: " << static_cast<int>(info.caps.revisionId) << "\n";
    report << "المصنّع / Vendor ID: 0x" << std::hex << info.caps.vendorId << std::dec << "\n";
    report << "التردد / Frequency: " << info.frequency << " Hz";
    if (info.frequency >= 1000000) {
        report << " (" << (info.frequency / 1000000) << " MHz)";
    }
    report << "\n";
    report << "الفترة / Period: " << period_ << " fs\n";
    report << "عداد 64 بت / 64-bit Counter: " << (is64Bit_ ? "نعم / Yes" : "لا / No") << "\n";
    report << "بديل قديم / Legacy Replace: " << (info.caps.legacyReplace ? "مدعوم / Supported" : "غير مدعوم / Not supported") << "\n";
    report << "الحالة / Status: " << (isRunning_ ? "يعمل / Running" : "متوقف / Stopped") << "\n";

    if (isRunning_) {
        report << "العداد الحالي / Current Counter: " << readCounter() << "\n";
    }

    report << "\nالمقارنات / Timers: " << static_cast<int>(timerCount_) << "\n";
    for (uint8_t i = 0; i < timerCount_; i++) {
        auto tc = getTimerConfig(i);
        report << "  [" << static_cast<int>(i) << "] ";
        report << (tc.enabled ? "مفعّل / Enabled" : "معطّل / Disabled");
        report << " | " << (tc.periodic ? "دوري / Periodic" : "مرة / OneShot");
        report << " | " << (tc.is64Bit ? "64-bit" : "32-bit");
        report << " | IRQ=" << static_cast<int>(tc.irqRoute);
        report << " | RouteCap=0x" << std::hex << tc.routingCapBits << std::dec;
        report << "\n";
    }

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
