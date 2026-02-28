/*
 * ============================================================================
 * تنفيذ ACPI - واجهة الطاقة والتهيئة المتقدمة - لغة ص
 * ACPI Implementation - Sad Language
 * ============================================================================
 */

#include "acpi.h"
#include "io_ports.h"
#include <cstring>
#include <sstream>
#include <iomanip>

namespace Sad {
namespace LowLevel {

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

ACPIManager& ACPIManager::getInstance() {
    static ACPIManager instance;
    return instance;
}

ACPIManager::ACPIManager()
    : rsdp_(nullptr)
    , fadt_(nullptr)
    , madt_(nullptr)
    , mcfg_(nullptr)
    , hpetTable_(nullptr)
    , localApicAddress_(0xFEE00000)
    , acpiVersion_(0)
    , isInitialized_(false) {}

// ============================================================================
// التهيئة / Initialization
// ============================================================================

bool ACPIManager::initialize() {
    const RSDP* rsdp = findRSDP();
    if (!rsdp) return false;

    return initializeFromRSDP(reinterpret_cast<uint64_t>(rsdp));
}

bool ACPIManager::initializeFromRSDP(uint64_t rsdpAddress) {
    rsdp_ = reinterpret_cast<const RSDP*>(rsdpAddress);

    // التحقق من التوقيع / Validate signature
    if (std::memcmp(rsdp_->signature, "RSD PTR ", 8) != 0) {
        return false;
    }

    // التحقق من المجموع الفحصي / Validate checksum (first 20 bytes for ACPI 1.0)
    if (!validateChecksum(rsdp_, 20)) {
        return false;
    }

    acpiVersion_ = rsdp_->revision;

    // تحليل الجداول / Parse tables
    if (acpiVersion_ >= 2 && rsdp_->xsdtAddress != 0) {
        // ACPI 2.0+: استخدام XSDT
        const XSDT* xsdt = reinterpret_cast<const XSDT*>(rsdp_->xsdtAddress);
        if (validateChecksum(xsdt, xsdt->header.length)) {
            parseXSDT(xsdt);
        }
    } else if (rsdp_->rsdtAddress != 0) {
        // ACPI 1.0: استخدام RSDT
        const RSDT* rsdt = reinterpret_cast<const RSDT*>(
            static_cast<uint64_t>(rsdp_->rsdtAddress));
        if (validateChecksum(rsdt, rsdt->header.length)) {
            parseRSDT(rsdt);
        }
    } else {
        return false;
    }

    // البحث عن الجداول المهمة / Find important tables
    fadt_ = reinterpret_cast<const FADT*>(findTable(ACPIConstants::SIG_FACP));
    madt_ = reinterpret_cast<const MADT*>(findTable(ACPIConstants::SIG_APIC));
    mcfg_ = reinterpret_cast<const MCFG*>(findTable(ACPIConstants::SIG_MCFG));
    hpetTable_ = reinterpret_cast<const HPETTable*>(findTable(ACPIConstants::SIG_HPET));

    // تحليل MADT / Parse MADT
    if (madt_) {
        parseMADT(madt_);
    }

    // تحليل MCFG / Parse MCFG
    if (mcfg_) {
        parseMCFG(mcfg_);
    }

    isInitialized_ = true;
    return true;
}

// ============================================================================
// البحث عن RSDP / RSDP Search
// ============================================================================

const RSDP* ACPIManager::findRSDP() const {
    // البحث في منطقة EBDA / Search EBDA area
    // الحصول على عنوان EBDA من BDA
    uint16_t ebdaSegment = *reinterpret_cast<const uint16_t*>(0x040E);
    uint64_t ebdaAddr = static_cast<uint64_t>(ebdaSegment) << 4;

    if (ebdaAddr >= ACPIConstants::EBDA_START && ebdaAddr < ACPIConstants::EBDA_END) {
        for (uint64_t addr = ebdaAddr; addr < ebdaAddr + 1024; addr += 16) {
            const RSDP* rsdp = reinterpret_cast<const RSDP*>(addr);
            if (std::memcmp(rsdp->signature, "RSD PTR ", 8) == 0) {
                if (validateChecksum(rsdp, 20)) {
                    return rsdp;
                }
            }
        }
    }

    // البحث في منطقة BIOS ROM / Search BIOS ROM area
    for (uint64_t addr = ACPIConstants::BIOS_ROM_START;
         addr < ACPIConstants::BIOS_ROM_END; addr += 16) {
        const RSDP* rsdp = reinterpret_cast<const RSDP*>(addr);
        if (std::memcmp(rsdp->signature, "RSD PTR ", 8) == 0) {
            if (validateChecksum(rsdp, 20)) {
                return rsdp;
            }
        }
    }

    return nullptr;
}

// ============================================================================
// التحقق / Validation
// ============================================================================

bool ACPIManager::validateChecksum(const void* data, size_t length) const {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    uint8_t sum = 0;
    for (size_t i = 0; i < length; i++) {
        sum += bytes[i];
    }
    return sum == 0;
}

// ============================================================================
// تحليل الجداول / Table Parsing
// ============================================================================

void ACPIManager::parseRSDT(const RSDT* rsdt) {
    uint32_t entries = (rsdt->header.length - sizeof(SDTHeader)) / sizeof(uint32_t);

    for (uint32_t i = 0; i < entries; i++) {
        const SDTHeader* table = reinterpret_cast<const SDTHeader*>(
            static_cast<uint64_t>(rsdt->entries[i]));

        if (table && validateChecksum(table, table->length)) {
            tables_.push_back(table);
        }
    }
}

void ACPIManager::parseXSDT(const XSDT* xsdt) {
    uint32_t entries = (xsdt->header.length - sizeof(SDTHeader)) / sizeof(uint64_t);

    for (uint32_t i = 0; i < entries; i++) {
        const SDTHeader* table = reinterpret_cast<const SDTHeader*>(xsdt->entries[i]);

        if (table && validateChecksum(table, table->length)) {
            tables_.push_back(table);
        }
    }
}

const SDTHeader* ACPIManager::findTable(const char* signature) const {
    for (const auto* table : tables_) {
        if (std::memcmp(table->signature, signature, 4) == 0) {
            return table;
        }
    }
    return nullptr;
}

void ACPIManager::parseMADT(const MADT* madt) {
    localApicAddress_ = madt->localApicAddress;

    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(madt) + sizeof(MADT);
    const uint8_t* end = reinterpret_cast<const uint8_t*>(madt) + madt->header.length;

    while (ptr < end) {
        const MADTEntryHeader* entry = reinterpret_cast<const MADTEntryHeader*>(ptr);
        if (entry->length == 0) break;

        switch (static_cast<MADTEntryType>(entry->type)) {
            case MADTEntryType::LOCAL_APIC: {
                const MADTLocalAPIC* lapic = reinterpret_cast<const MADTLocalAPIC*>(entry);
                MADTLocalAPICInfo info;
                info.processorId = lapic->processorId;
                info.apicId = lapic->apicId;
                info.enabled = (lapic->flags & 1) != 0;
                localApics_.push_back(info);
                break;
            }

            case MADTEntryType::IO_APIC: {
                const MADTIOAPIC* ioapic = reinterpret_cast<const MADTIOAPIC*>(entry);
                MADTIOAPICInfo info;
                info.id = ioapic->ioApicId;
                info.address = ioapic->ioApicAddress;
                info.gsiBase = ioapic->globalSystemInterruptBase;
                ioApics_.push_back(info);
                break;
            }

            case MADTEntryType::INTERRUPT_OVERRIDE: {
                const MADTInterruptOverride* ovr =
                    reinterpret_cast<const MADTInterruptOverride*>(entry);
                IRQOverride info;
                info.source = ovr->source;
                info.gsi = ovr->globalSystemInterrupt;
                info.flags = ovr->flags;
                irqOverrides_.push_back(info);
                break;
            }

            default:
                break;
        }

        ptr += entry->length;
    }
}

void ACPIManager::parseMCFG(const MCFG* mcfg) {
    uint32_t entryCount = (mcfg->header.length - sizeof(SDTHeader) - 8) / sizeof(MCFGEntry);
    const MCFGEntry* entries = mcfg->entries;

    for (uint32_t i = 0; i < entryCount; i++) {
        mcfgEntries_.push_back(entries[i]);
    }
}

// ============================================================================
// إدارة الطاقة / Power Management
// ============================================================================

bool ACPIManager::enableACPI() {
    if (!fadt_) return false;

    // التحقق إذا كان ACPI مفعلاً بالفعل / Check if already enabled
    uint16_t pm1a = IOPorts::inw(static_cast<uint16_t>(fadt_->pm1aControlBlock));
    if (pm1a & 1) return true; // مفعل بالفعل / Already enabled

    // إرسال أمر التفعيل / Send enable command
    if (fadt_->smiCommandPort != 0 && fadt_->acpiEnable != 0) {
        IOPorts::outb(static_cast<uint16_t>(fadt_->smiCommandPort), fadt_->acpiEnable);

        // انتظار التفعيل / Wait for enable
        for (int i = 0; i < 1000; i++) {
            pm1a = IOPorts::inw(static_cast<uint16_t>(fadt_->pm1aControlBlock));
            if (pm1a & 1) return true;
            // تأخير قصير / Short delay
            for (volatile int j = 0; j < 10000; j++) {}
        }
        return false;
    }

    return true;
}

bool ACPIManager::disableACPI() {
    if (!fadt_ || fadt_->smiCommandPort == 0 || fadt_->acpiDisable == 0) return false;

    IOPorts::outb(static_cast<uint16_t>(fadt_->smiCommandPort), fadt_->acpiDisable);
    return true;
}

void ACPIManager::shutdown() {
    if (!fadt_) return;

    // كتابة SLP_TYP | SLP_EN إلى PM1a_CNT / Write SLP_TYP | SLP_EN to PM1a_CNT
    uint16_t pm1aPort = static_cast<uint16_t>(fadt_->pm1aControlBlock);
    IOPorts::outw(pm1aPort, ACPIConstants::PM1_CNT_SLP_TYP_S5 | ACPIConstants::SLP_EN);

    // إذا لم ينجح، حاول PM1b / If failed, try PM1b
    if (fadt_->pm1bControlBlock != 0) {
        uint16_t pm1bPort = static_cast<uint16_t>(fadt_->pm1bControlBlock);
        IOPorts::outw(pm1bPort, ACPIConstants::PM1_CNT_SLP_TYP_S5 | ACPIConstants::SLP_EN);
    }

    // إذا لم ينجح، حاول إعادة الضبط / If failed, try reset
    reboot();
}

void ACPIManager::reboot() {
    if (fadt_ && (fadt_->flags & ACPIConstants::FADT_RESET_REG)) {
        // استخدام سجل إعادة الضبط / Use reset register
        if (fadt_->resetReg.addressSpace == 1) {
            // I/O space
            IOPorts::outb(static_cast<uint16_t>(fadt_->resetReg.address), fadt_->resetValue);
        } else if (fadt_->resetReg.addressSpace == 0) {
            // Memory space
            volatile uint8_t* addr = reinterpret_cast<volatile uint8_t*>(fadt_->resetReg.address);
            *addr = fadt_->resetValue;
        }
    }

    // طريقة بديلة: لوحة مفاتيح 8042 / Fallback: 8042 keyboard controller
    IOPorts::outb(0x64, 0xFE);

    // إذا لم تنجح أيضاً: Triple fault (إعادة تشغيل قسرية)
    // If that also failed: Triple fault (forced reboot)
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("lidt (%%rax)" :: "a"(0));
    __asm__ volatile("int $3");
#endif
}

bool ACPIManager::enterSleepState(SleepState state) {
    if (!fadt_) return false;

    // يحتاج تحليل AML لاستخراج SLP_TYP الصحيح لكل حالة نوم
    // In a real implementation, AML parsing is needed for correct SLP_TYP values
    // هنا نستخدم قيم نموذجية / Here we use typical values

    uint16_t slpTyp = 0;
    switch (state) {
        case SleepState::S0: return true; // يقظة / Already awake
        case SleepState::S1: slpTyp = (1 << 10); break;
        case SleepState::S3: slpTyp = (5 << 10); break;
        case SleepState::S5: slpTyp = (7 << 10); break;
        default: return false;
    }

    uint16_t pm1aPort = static_cast<uint16_t>(fadt_->pm1aControlBlock);
    IOPorts::outw(pm1aPort, slpTyp | ACPIConstants::SLP_EN);

    return true;
}

// ============================================================================
// مؤقت PM / PM Timer
// ============================================================================

uint32_t ACPIManager::readPMTimer() const {
    if (!fadt_) return 0;
    return IOPorts::inl(static_cast<uint16_t>(fadt_->pmTimerBlock));
}

bool ACPIManager::isPMTimer32Bit() const {
    if (!fadt_) return false;
    return (fadt_->flags & (1 << 8)) != 0; // TMR_VAL_EXT bit
}

void ACPIManager::delayMicroseconds(uint32_t us) const {
    if (!fadt_) return;

    uint32_t timerTicks = (us * ACPIConstants::PM_TIMER_FREQ) / 1000000;
    uint32_t mask = isPMTimer32Bit() ? 0xFFFFFFFF : 0x00FFFFFF;

    uint32_t start = readPMTimer() & mask;

    while (true) {
        uint32_t current = readPMTimer() & mask;
        uint32_t elapsed = (current - start) & mask;
        if (elapsed >= timerTicks) break;
    }
}

// ============================================================================
// PCIe ECAM
// ============================================================================

uint64_t ACPIManager::getECAMBase(uint16_t segment, uint8_t bus) const {
    for (const auto& entry : mcfgEntries_) {
        if (entry.pciSegmentGroup == segment &&
            bus >= entry.startBusNumber &&
            bus <= entry.endBusNumber) {
            return entry.baseAddress;
        }
    }
    return 0;
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string ACPIManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير ACPI / ACPI Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "الحالة / Status: " << (isInitialized_ ? "مهيأ / Initialized" : "غير مهيأ / Not initialized") << "\n";
    report << "إصدار ACPI / ACPI Version: " << static_cast<int>(acpiVersion_) << ".0\n";
    report << "عدد الجداول / Table Count: " << tables_.size() << "\n\n";

    if (!tables_.empty()) {
        report << "--- الجداول المكتشفة / Discovered Tables ---\n";
        for (const auto* table : tables_) {
            char sig[5] = {0};
            std::memcpy(sig, table->signature, 4);
            report << "  " << sig << " (حجم / Size: " << table->length << " بايت / bytes)\n";
        }
        report << "\n";
    }

    report << "--- APIC ---\n";
    report << "عنوان Local APIC / Local APIC Address: 0x" << std::hex << localApicAddress_ << std::dec << "\n";
    report << "عدد المعالجات / Processor Count: " << localApics_.size() << "\n";
    for (const auto& lapic : localApics_) {
        report << "  معالج / CPU #" << static_cast<int>(lapic.processorId)
               << " APIC #" << static_cast<int>(lapic.apicId)
               << " " << (lapic.enabled ? "(مفعل / Enabled)" : "(معطل / Disabled)") << "\n";
    }

    report << "عدد I/O APICs: " << ioApics_.size() << "\n";
    for (const auto& ioapic : ioApics_) {
        report << "  I/O APIC #" << static_cast<int>(ioapic.id)
               << " العنوان / Address: 0x" << std::hex << ioapic.address
               << " GSI: " << std::dec << ioapic.gsiBase << "\n";
    }

    if (!irqOverrides_.empty()) {
        report << "تجاوزات IRQ / IRQ Overrides: " << irqOverrides_.size() << "\n";
        for (const auto& ovr : irqOverrides_) {
            report << "  IRQ " << static_cast<int>(ovr.source)
                   << " -> GSI " << ovr.gsi << "\n";
        }
    }

    if (!mcfgEntries_.empty()) {
        report << "\n--- PCIe ECAM ---\n";
        for (const auto& entry : mcfgEntries_) {
            report << "  الشريحة / Segment: " << entry.pciSegmentGroup
                   << " الناقلات / Buses: " << static_cast<int>(entry.startBusNumber)
                   << "-" << static_cast<int>(entry.endBusNumber)
                   << " العنوان / Base: 0x" << std::hex << entry.baseAddress << std::dec << "\n";
        }
    }

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
