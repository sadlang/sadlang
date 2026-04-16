/*
 * ============================================================================
 * تنفيذ ناقل PCI/PCIe للغة ص
 * PCI/PCIe Bus Implementation for Sad Language
 * ============================================================================
 */

#include "pci.h"
#include "io_ports.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Sad {
namespace LowLevel {

// ============================================================================
// PCIAddress
// ============================================================================

std::string PCIAddress::toString() const {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(2) << static_cast<int>(bus) << ":"
        << std::setw(2) << static_cast<int>(device) << "."
        << static_cast<int>(function);
    return oss.str();
}

// ============================================================================
// PCIDeviceInfo - أسماء مساعدة / Helper names
// ============================================================================

std::string PCIDeviceInfo::getClassName() const {
    switch (classCode) {
        case PCIConstants::CLASS_UNCLASSIFIED:  return "غير مصنف / Unclassified";
        case PCIConstants::CLASS_STORAGE:       return "وحدة تخزين / Storage Controller";
        case PCIConstants::CLASS_NETWORK:       return "وحدة شبكة / Network Controller";
        case PCIConstants::CLASS_DISPLAY:
            switch (subclass) {
                case PCIConstants::DISPLAY_VGA:   return "وحدة عرض VGA / VGA Display Controller";
                case PCIConstants::DISPLAY_XGA:   return "وحدة عرض XGA / XGA Display Controller";
                case PCIConstants::DISPLAY_3D:    return "وحدة 3D / 3D Controller";
                default:                          return "وحدة عرض / Display Controller";
            }
        case PCIConstants::CLASS_MULTIMEDIA:    return "وسائط متعددة / Multimedia Controller";
        case PCIConstants::CLASS_MEMORY:        return "وحدة ذاكرة / Memory Controller";
        case PCIConstants::CLASS_BRIDGE:        return "جسر / Bridge Device";
        case PCIConstants::CLASS_COMMUNICATION: return "اتصالات / Communication Controller";
        case PCIConstants::CLASS_SYSTEM:        return "نظام / System Peripheral";
        case PCIConstants::CLASS_INPUT:         return "إدخال / Input Device";
        case PCIConstants::CLASS_SERIAL_BUS:    return "ناقل تسلسلي / Serial Bus Controller";
        case PCIConstants::CLASS_WIRELESS:      return "لاسلكي / Wireless Controller";
        case PCIConstants::CLASS_ENCRYPTION:    return "تشفير / Encryption Controller";
        default:                               return "غير معروف / Unknown";
    }
}

std::string PCIDeviceInfo::getVendorName() const {
    switch (vendorId) {
        case PCIConstants::VENDOR_NVIDIA: return "NVIDIA";
        case PCIConstants::VENDOR_AMD:    return "AMD/ATI";
        case PCIConstants::VENDOR_INTEL:  return "Intel";
        case 0x1234:                      return "QEMU/Bochs";
        case 0x15AD:                      return "VMware";
        case 0x1AF4:                      return "VirtIO (Red Hat)";
        case 0x80EE:                      return "VirtualBox (Oracle)";
        case 0x1B36:                      return "QEMU";
        case 0x14E4:                      return "Broadcom";
        case 0x10EC:                      return "Realtek";
        case 0x168C:                      return "Qualcomm/Atheros";
        default:                          return "غير معروف / Unknown";
    }
}

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

PCIManager& PCIManager::getInstance() {
    static PCIManager instance;
    return instance;
}

PCIManager::PCIManager() : isEnumerated_(false) {}

// ============================================================================
// قراءة/كتابة فضاء التهيئة / Configuration Space Read/Write
// ============================================================================

uint32_t PCIManager::readConfig32(const PCIAddress& addr, uint8_t offset) {
    IOPorts::outl(PCIConstants::CONFIG_ADDRESS, addr.toConfigAddress(offset));
    return IOPorts::inl(PCIConstants::CONFIG_DATA);
}

uint16_t PCIManager::readConfig16(const PCIAddress& addr, uint8_t offset) {
    uint32_t value = readConfig32(addr, offset & 0xFC);
    return static_cast<uint16_t>((value >> ((offset & 2) * 8)) & 0xFFFF);
}

uint8_t PCIManager::readConfig8(const PCIAddress& addr, uint8_t offset) {
    uint32_t value = readConfig32(addr, offset & 0xFC);
    return static_cast<uint8_t>((value >> ((offset & 3) * 8)) & 0xFF);
}

void PCIManager::writeConfig32(const PCIAddress& addr, uint8_t offset, uint32_t value) {
    IOPorts::outl(PCIConstants::CONFIG_ADDRESS, addr.toConfigAddress(offset));
    IOPorts::outl(PCIConstants::CONFIG_DATA, value);
}

void PCIManager::writeConfig16(const PCIAddress& addr, uint8_t offset, uint16_t value) {
    uint32_t tmp = readConfig32(addr, offset & 0xFC);
    int shift = (offset & 2) * 8;
    tmp &= ~(0xFFFF << shift);
    tmp |= (static_cast<uint32_t>(value) << shift);
    writeConfig32(addr, offset & 0xFC, tmp);
}

void PCIManager::writeConfig8(const PCIAddress& addr, uint8_t offset, uint8_t value) {
    uint32_t tmp = readConfig32(addr, offset & 0xFC);
    int shift = (offset & 3) * 8;
    tmp &= ~(0xFF << shift);
    tmp |= (static_cast<uint32_t>(value) << shift);
    writeConfig32(addr, offset & 0xFC, tmp);
}

// ============================================================================
// اكتشاف الأجهزة / Device Enumeration
// ============================================================================

void PCIManager::enumerate() {
    devices_.clear();

    // مسح جميع النواقل / Scan all buses
    for (int bus = 0; bus < 256; ++bus) {
        scanBus(static_cast<uint8_t>(bus));
    }

    isEnumerated_ = true;
}

void PCIManager::scanBus(uint8_t bus) {
    for (uint8_t device = 0; device < 32; ++device) {
        scanDevice(bus, device);
    }
}

void PCIManager::scanDevice(uint8_t bus, uint8_t device) {
    PCIAddress addr(bus, device, 0);
    uint16_t vendorId = readConfig16(addr, PCIConstants::VENDOR_ID);
    if (vendorId == PCIConstants::VENDOR_INVALID) return;

    scanFunction(bus, device, 0);

    // فحص الدوال المتعددة / Check multi-function
    uint8_t headerType = readConfig8(addr, PCIConstants::HEADER_TYPE);
    if (headerType & 0x80) {
        for (uint8_t func = 1; func < 8; ++func) {
            PCIAddress funcAddr(bus, device, func);
            uint16_t funcVendor = readConfig16(funcAddr, PCIConstants::VENDOR_ID);
            if (funcVendor != PCIConstants::VENDOR_INVALID) {
                scanFunction(bus, device, func);
            }
        }
    }
}

void PCIManager::scanFunction(uint8_t bus, uint8_t device, uint8_t function) {
    PCIAddress addr(bus, device, function);
    PCIDeviceInfo info = readDeviceInfo(addr);

    // إذا كان الجهاز جسر PCI-to-PCI فمسح الناقل الثانوي
    // If device is PCI-to-PCI bridge, scan secondary bus
    if (info.classCode == PCIConstants::CLASS_BRIDGE && info.subclass == 0x04) {
        uint8_t secondaryBus = readConfig8(addr, 0x19);
        scanBus(secondaryBus);
    }

    devices_.push_back(info);
}

PCIDeviceInfo PCIManager::readDeviceInfo(const PCIAddress& addr) {
    PCIDeviceInfo info;
    info.address = addr;

    info.vendorId      = readConfig16(addr, PCIConstants::VENDOR_ID);
    info.deviceId      = readConfig16(addr, PCIConstants::DEVICE_ID);
    info.classCode     = readConfig8(addr, PCIConstants::CLASS_CODE);
    info.subclass      = readConfig8(addr, PCIConstants::SUBCLASS);
    info.progIF        = readConfig8(addr, PCIConstants::PROG_IF);
    info.revisionId    = readConfig8(addr, PCIConstants::REVISION_ID);
    info.headerType    = readConfig8(addr, PCIConstants::HEADER_TYPE);
    info.subsysVendorId = readConfig16(addr, PCIConstants::SUBSYS_VENDOR);
    info.subsysId      = readConfig16(addr, PCIConstants::SUBSYS_ID);
    info.interruptLine = readConfig8(addr, PCIConstants::INTERRUPT_LINE);
    info.interruptPin  = readConfig8(addr, PCIConstants::INTERRUPT_PIN);

    // قراءة مؤشر القدرات / Read capabilities pointer
    uint16_t status = readConfig16(addr, PCIConstants::STATUS);
    if (status & (1 << 4)) { // بت القدرات / Capabilities bit
        info.capabilitiesPtr = readConfig8(addr, PCIConstants::CAPABILITIES) & 0xFC;
    }

    // قراءة جميع BARs / Read all BARs
    info.bars = readAllBARs(addr);

    return info;
}

// ============================================================================
// إدارة BAR / BAR Management
// ============================================================================

BARInfo PCIManager::readBAR(const PCIAddress& addr, int barIndex) {
    BARInfo info;
    info.barIndex = barIndex;

    uint8_t offset = PCIConstants::BAR0 + (barIndex * 4);
    uint32_t barValue = readConfig32(addr, offset);

    if (barValue == 0) return info; // BAR غير مستخدم / BAR unused

    if (barValue & 0x01) {
        // فضاء I/O / I/O space
        info.type = BARType::IO_SPACE;
        info.baseAddress = barValue & 0xFFFFFFFC;

        // كشف الحجم / Detect size
        writeConfig32(addr, offset, 0xFFFFFFFF);
        uint32_t sizeValue = readConfig32(addr, offset);
        writeConfig32(addr, offset, barValue); // استعادة / Restore

        sizeValue |= 0x03;
        info.size = (~sizeValue) + 1;
    } else {
        // فضاء ذاكرة / Memory space
        uint8_t memType = (barValue >> 1) & 0x03;
        info.prefetchable = (barValue >> 3) & 0x01;

        if (memType == 0x02) {
            // ذاكرة 64-بت / 64-bit memory
            info.type = BARType::MEMORY_64;
            uint32_t barHighValue = readConfig32(addr, offset + 4);
            info.baseAddress = (static_cast<uint64_t>(barHighValue) << 32)
                             | (barValue & 0xFFFFFFF0);

            // كشف الحجم / Detect size
            writeConfig32(addr, offset, 0xFFFFFFFF);
            writeConfig32(addr, offset + 4, 0xFFFFFFFF);
            uint32_t sizeLow = readConfig32(addr, offset);
            uint32_t sizeHigh = readConfig32(addr, offset + 4);
            writeConfig32(addr, offset, barValue);
            writeConfig32(addr, offset + 4, barHighValue);

            uint64_t sizeValue = (static_cast<uint64_t>(sizeHigh) << 32) | sizeLow;
            sizeValue |= 0x0F;
            info.size = (~sizeValue) + 1;
        } else {
            // ذاكرة 32-بت / 32-bit memory
            info.type = BARType::MEMORY_32;
            info.baseAddress = barValue & 0xFFFFFFF0;

            // كشف الحجم / Detect size
            writeConfig32(addr, offset, 0xFFFFFFFF);
            uint32_t sizeValue = readConfig32(addr, offset);
            writeConfig32(addr, offset, barValue);

            sizeValue |= 0x0F;
            info.size = (~sizeValue) + 1;
        }
    }

    return info;
}

std::vector<BARInfo> PCIManager::readAllBARs(const PCIAddress& addr) {
    std::vector<BARInfo> bars;
    int maxBars = 6;

    uint8_t headerType = readConfig8(addr, PCIConstants::HEADER_TYPE) & 0x7F;
    if (headerType == 0x01) maxBars = 2; // جسر PCI-to-PCI / PCI-to-PCI bridge

    for (int i = 0; i < maxBars; ++i) {
        BARInfo bar = readBAR(addr, i);
        if (bar.size > 0) {
            bars.push_back(bar);
            if (bar.type == BARType::MEMORY_64) {
                i++; // BAR 64-بت تستخدم إدخالين / 64-bit BAR uses 2 entries
            }
        }
    }

    return bars;
}

// ============================================================================
// إدارة الأوامر / Command Management
// ============================================================================

void PCIManager::enableBusMaster(const PCIAddress& addr) {
    uint16_t cmd = readConfig16(addr, PCIConstants::COMMAND);
    cmd |= PCIConstants::CMD_BUS_MASTER;
    writeConfig16(addr, PCIConstants::COMMAND, cmd);
}

void PCIManager::enableMemorySpace(const PCIAddress& addr) {
    uint16_t cmd = readConfig16(addr, PCIConstants::COMMAND);
    cmd |= PCIConstants::CMD_MEMORY_SPACE;
    writeConfig16(addr, PCIConstants::COMMAND, cmd);
}

void PCIManager::enableIOSpace(const PCIAddress& addr) {
    uint16_t cmd = readConfig16(addr, PCIConstants::COMMAND);
    cmd |= PCIConstants::CMD_IO_SPACE;
    writeConfig16(addr, PCIConstants::COMMAND, cmd);
}

void PCIManager::disableLegacyInterrupts(const PCIAddress& addr) {
    uint16_t cmd = readConfig16(addr, PCIConstants::COMMAND);
    cmd |= PCIConstants::CMD_INT_DISABLE;
    writeConfig16(addr, PCIConstants::COMMAND, cmd);
}

// ============================================================================
// القدرات / Capabilities
// ============================================================================

uint8_t PCIManager::findCapability(const PCIAddress& addr, uint8_t capId) {
    uint16_t status = readConfig16(addr, PCIConstants::STATUS);
    if (!(status & (1 << 4))) return 0; // لا توجد قدرات / No capabilities

    uint8_t ptr = readConfig8(addr, PCIConstants::CAPABILITIES) & 0xFC;

    while (ptr != 0) {
        uint8_t id = readConfig8(addr, ptr);
        if (id == capId) return ptr;
        ptr = readConfig8(addr, ptr + 1) & 0xFC;
        if (ptr == 0xFF) break; // حماية من الحلقة اللانهائية / Protection from infinite loop
    }

    return 0;
}

bool PCIManager::isPCIeDevice(const PCIAddress& addr) {
    return findCapability(addr, PCIConstants::CAP_PCIE) != 0;
}

// ============================================================================
// إدارة MSI / MSI Management
// ============================================================================

bool PCIManager::findMSICapability(const PCIAddress& addr, MSIConfig& config) {
    uint8_t capOffset = findCapability(addr, PCIConstants::CAP_MSI);
    if (capOffset == 0) return false;

    config.capOffset = capOffset;
    config.messageControl = readConfig16(addr, capOffset + 2);
    config.is64Bit = (config.messageControl >> 7) & 1;
    config.isEnabled = config.messageControl & 1;

    config.messageAddress = readConfig32(addr, capOffset + 4);
    if (config.is64Bit) {
        config.messageAddressHigh = readConfig32(addr, capOffset + 8);
        config.messageData = readConfig16(addr, capOffset + 12);
    } else {
        config.messageAddressHigh = 0;
        config.messageData = readConfig16(addr, capOffset + 8);
    }

    return true;
}

bool PCIManager::configureMSI(const PCIAddress& addr, uint8_t vector, uint8_t processor) {
    MSIConfig config;
    if (!findMSICapability(addr, config)) return false;

    // عنوان MSI: 0xFEE + (processor << 12) / MSI address format
    uint32_t msiAddress = 0xFEE00000 | (static_cast<uint32_t>(processor) << 12);
    writeConfig32(addr, config.capOffset + 4, msiAddress);

    if (config.is64Bit) {
        writeConfig32(addr, config.capOffset + 8, 0);
        writeConfig16(addr, config.capOffset + 12, vector);
    } else {
        writeConfig16(addr, config.capOffset + 8, vector);
    }

    return true;
}

bool PCIManager::enableMSI(const PCIAddress& addr) {
    uint8_t capOffset = findCapability(addr, PCIConstants::CAP_MSI);
    if (capOffset == 0) return false;

    uint16_t msgCtrl = readConfig16(addr, capOffset + 2);
    msgCtrl |= 0x0001; // تفعيل MSI / Enable MSI
    writeConfig16(addr, capOffset + 2, msgCtrl);

    // تعطيل المقاطعات القديمة / Disable legacy interrupts
    disableLegacyInterrupts(addr);

    return true;
}

void PCIManager::disableMSI(const PCIAddress& addr) {
    uint8_t capOffset = findCapability(addr, PCIConstants::CAP_MSI);
    if (capOffset == 0) return;

    uint16_t msgCtrl = readConfig16(addr, capOffset + 2);
    msgCtrl &= ~0x0001; // تعطيل MSI / Disable MSI
    writeConfig16(addr, capOffset + 2, msgCtrl);
}

// ============================================================================
// البحث عن الأجهزة / Device Search
// ============================================================================

std::vector<PCIDeviceInfo> PCIManager::findByClass(uint8_t classCode, int subclass) const {
    std::vector<PCIDeviceInfo> result;
    for (const auto& dev : devices_) {
        if (dev.classCode == classCode) {
            if (subclass < 0 || dev.subclass == static_cast<uint8_t>(subclass)) {
                result.push_back(dev);
            }
        }
    }
    return result;
}

std::vector<PCIDeviceInfo> PCIManager::findByVendor(uint16_t vendorId, uint16_t deviceId) const {
    std::vector<PCIDeviceInfo> result;
    for (const auto& dev : devices_) {
        if (dev.vendorId == vendorId) {
            if (deviceId == 0xFFFF || dev.deviceId == deviceId) {
                result.push_back(dev);
            }
        }
    }
    return result;
}

std::vector<PCIDeviceInfo> PCIManager::findGPUs() const {
    return findByClass(PCIConstants::CLASS_DISPLAY);
}

const PCIDeviceInfo* PCIManager::getPrimaryGPU() const {
    // أولاً: البحث عن VGA / First: look for VGA
    for (const auto& dev : devices_) {
        if (dev.isVGACompatible()) return &dev;
    }
    // ثانياً: أي وحدة عرض / Second: any display controller
    for (const auto& dev : devices_) {
        if (dev.isDisplayController()) return &dev;
    }
    return nullptr;
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string PCIManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(90, '=') << "\n";
    report << "تقرير أجهزة PCI / PCI Devices Report\n";
    report << std::string(90, '=') << "\n\n";
    report << "عدد الأجهزة المكتشفة / Devices Found: " << devices_.size() << "\n\n";

    report << std::left
           << std::setw(10) << "العنوان"
           << std::setw(12) << "Vendor:Dev"
           << std::setw(16) << "الشركة"
           << std::setw(30) << "الصنف / Class"
           << std::setw(6)  << "IRQ"
           << "BARs\n";
    report << std::string(90, '-') << "\n";

    for (const auto& dev : devices_) {
        report << std::left
               << std::setw(10) << dev.address.toString()
               << std::hex << std::setfill('0')
               << std::setw(4) << dev.vendorId << ":"
               << std::setw(4) << dev.deviceId << "  "
               << std::dec << std::setfill(' ')
               << std::setw(16) << dev.getVendorName()
               << std::setw(30) << dev.getClassName()
               << std::setw(6)  << static_cast<int>(dev.interruptLine);

        for (const auto& bar : dev.bars) {
            report << " [BAR" << bar.barIndex << ":0x" << std::hex << bar.baseAddress
                   << " " << std::dec << (bar.size / 1024) << "KB]";
        }
        report << "\n";
    }

    // تفاصيل GPU / GPU details
    auto gpus = findGPUs();
    if (!gpus.empty()) {
        report << "\n" << std::string(90, '-') << "\n";
        report << "وحدات معالجة الرسومات / GPUs:\n";
        report << std::string(90, '-') << "\n";
        for (const auto& gpu : gpus) {
            report << "  " << gpu.getVendorName() << " " << gpu.getClassName() << "\n";
            report << "    العنوان / Address: " << gpu.address.toString() << "\n";
            report << "    المعرف / ID: 0x" << std::hex << gpu.vendorId << ":0x" << gpu.deviceId << std::dec << "\n";
            for (const auto& bar : gpu.bars) {
                report << "    BAR" << bar.barIndex << ": 0x" << std::hex << bar.baseAddress
                       << " (" << std::dec << (bar.size / (1024*1024)) << " MB)"
                       << (bar.prefetchable ? " [قابل للجلب المسبق]" : "") << "\n";
            }
        }
    }

    report << std::string(90, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
