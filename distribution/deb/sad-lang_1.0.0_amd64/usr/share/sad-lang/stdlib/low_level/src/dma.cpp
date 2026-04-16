/*
 * ============================================================================
 * تنفيذ الوصول المباشر للذاكرة (DMA) للغة ص
 * DMA Implementation for Sad Language
 * ============================================================================
 */

#include "dma.h"
#include "io_ports.h"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>

namespace Sad {
namespace LowLevel {

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

DMAManager& DMAManager::getInstance() {
    static DMAManager instance;
    return instance;
}

DMAManager::DMAManager()
    : nextTransferId_(1)
    , totalTransfers_(0)
    , totalBytes_(0)
    , isInitialized_(false) {}

// ============================================================================
// التهيئة / Initialization
// ============================================================================

void DMAManager::initialize() {
    if (isInitialized_) return;

    // إعادة ضبط وحدتي DMA / Reset both DMA controllers
    IOPorts::outb(DMAConstants::DMA1_RESET, 0xFF);
    IOPorts::outb(DMAConstants::DMA2_RESET, 0xFF);

    // تقنيع جميع القنوات (تعطيلها) / Mask all channels (disable them)
    IOPorts::outb(DMAConstants::DMA1_MASK, 0x04); // قناع القناة 0 / Mask channel 0
    IOPorts::outb(DMAConstants::DMA1_MASK, 0x05);
    IOPorts::outb(DMAConstants::DMA1_MASK, 0x06);
    IOPorts::outb(DMAConstants::DMA1_MASK, 0x07);

    isInitialized_ = true;
}

// ============================================================================
// تخصيص الذاكرة / Memory Allocation
// ============================================================================

uint64_t DMAManager::allocateBuffer(uint64_t size, uint64_t alignment) {
    // في بيئة نواة حقيقية، نستخدم مخصص الإطارات لتخصيص ذاكرة متجاورة مادياً
    // In a real kernel, we'd use the frame allocator for physically contiguous memory
    // وتتأكد من أنها محاذاة ولا تتجاوز حدود 4GB لتوافق DMA 32-بت
    // Ensuring alignment and staying below 4GB for 32-bit DMA compatibility

    uint64_t pages = (size + 4095) / 4096;

    // محاكاة التخصيص / Simulated allocation
    // في النواة الحقيقية: PageFrameAllocator::getInstance().allocateContiguousFrames(pages)
    static uint64_t dmaPoolBase = 0x200000; // 2MB فما فوق / 2MB and above
    uint64_t address = (dmaPoolBase + alignment - 1) & ~(alignment - 1);
    dmaPoolBase = address + (pages * 4096);

    return address;
}

void DMAManager::freeBuffer(uint64_t physicalAddress, uint64_t size) {
    // في النواة الحقيقية: PageFrameAllocator::getInstance().freeContiguousFrames(...)
    (void)physicalAddress;
    (void)size;
}

// ============================================================================
// عمليات النقل / Transfer Operations
// ============================================================================

uint32_t DMAManager::startTransfer(uint64_t source, uint64_t dest, uint64_t size,
                                    DMADirection direction) {
    DMATransfer transfer;
    transfer.sourceAddress = source;
    transfer.destAddress = dest;
    transfer.size = size;
    transfer.direction = direction;
    transfer.mode = DMAMode::SINGLE;
    transfer.status = DMAStatus::IN_PROGRESS;
    transfer.transferId = nextTransferId_++;
    transfer.useScatterGather = false;

    activeTransfers_.push_back(transfer);
    totalTransfers_++;

    // في نظام حقيقي: برمجة وحدة DMA أو Bus Master الخاص بالجهاز
    // In real system: program DMA controller or device Bus Master

    // محاكاة إكمال فوري / Simulated immediate completion
    for (auto& t : activeTransfers_) {
        if (t.transferId == transfer.transferId) {
            t.status = DMAStatus::COMPLETED;
            totalBytes_ += size;
            if (t.callback) {
                t.callback(DMAStatus::COMPLETED);
            }
            break;
        }
    }

    return transfer.transferId;
}

uint32_t DMAManager::startScatterGatherTransfer(const std::vector<SGEntry>& sgList,
                                                  uint64_t dest,
                                                  DMADirection direction) {
    DMATransfer transfer;
    transfer.destAddress = dest;
    transfer.direction = direction;
    transfer.mode = DMAMode::BLOCK;
    transfer.status = DMAStatus::IN_PROGRESS;
    transfer.transferId = nextTransferId_++;
    transfer.useScatterGather = true;
    transfer.sgList = sgList;

    // حساب الحجم الإجمالي / Calculate total size
    uint64_t totalSize = 0;
    for (const auto& entry : sgList) {
        totalSize += entry.length;
    }
    transfer.size = totalSize;

    activeTransfers_.push_back(transfer);
    totalTransfers_++;

    // محاكاة إكمال / Simulated completion
    for (auto& t : activeTransfers_) {
        if (t.transferId == transfer.transferId) {
            t.status = DMAStatus::COMPLETED;
            totalBytes_ += totalSize;
            if (t.callback) {
                t.callback(DMAStatus::COMPLETED);
            }
            break;
        }
    }

    return transfer.transferId;
}

DMAStatus DMAManager::waitForTransfer(uint32_t transferId, uint32_t timeoutMs) {
    (void)timeoutMs;

    for (const auto& t : activeTransfers_) {
        if (t.transferId == transferId) {
            return t.status;
        }
    }
    return DMAStatus::ERROR;
}

bool DMAManager::cancelTransfer(uint32_t transferId) {
    for (auto& t : activeTransfers_) {
        if (t.transferId == transferId && t.status == DMAStatus::IN_PROGRESS) {
            t.status = DMAStatus::ERROR;
            return true;
        }
    }
    return false;
}

DMAStatus DMAManager::getTransferStatus(uint32_t transferId) const {
    for (const auto& t : activeTransfers_) {
        if (t.transferId == transferId) {
            return t.status;
        }
    }
    return DMAStatus::ERROR;
}

void DMAManager::setTransferCallback(uint32_t transferId, std::function<void(DMAStatus)> callback) {
    for (auto& t : activeTransfers_) {
        if (t.transferId == transferId) {
            t.callback = callback;
            return;
        }
    }
}

// ============================================================================
// DMA التقليدي (ISA) / Legacy (ISA) DMA
// ============================================================================

void DMAManager::setupLegacyChannel(uint8_t channel, uint32_t address,
                                     uint16_t count, DMAMode mode,
                                     DMADirection direction) {
    if (channel >= DMAConstants::MAX_CHANNELS) return;

    // التأكد من أن العنوان أقل من 16MB (حد ISA DMA) / Ensure address below 16MB
    if (address >= 0x1000000) return;

    bool isHighChannel = (channel >= 4);
    uint8_t actualChannel = isHighChannel ? (channel - 4) : channel;

    // حساب عنوان المنفذ / Calculate port addresses
    uint16_t maskPort   = isHighChannel ? DMAConstants::DMA2_MASK : DMAConstants::DMA1_MASK;
    uint16_t modePort   = isHighChannel ? DMAConstants::DMA2_MODE : DMAConstants::DMA1_MODE;
    uint16_t clearPort  = isHighChannel ? DMAConstants::DMA2_CLEAR_FF : DMAConstants::DMA1_CLEAR_FF;

    // منافذ العنوان والعدد / Address and count ports
    static const uint16_t addrPorts[] = {0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC};
    static const uint16_t countPorts[] = {0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE};
    static const uint16_t pagePorts[] = {0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A};

    // 1. تقنيع القناة / Mask the channel
    IOPorts::outb(maskPort, 0x04 | actualChannel);

    // 2. مسح علم flip-flop / Clear flip-flop
    IOPorts::outb(clearPort, 0xFF);

    // 3. تعيين وضع النقل / Set transfer mode
    uint8_t modeValue = actualChannel;
    switch (direction) {
        case DMADirection::DEVICE_TO_MEMORY: modeValue |= 0x04; break;  // كتابة / Write
        case DMADirection::MEMORY_TO_DEVICE: modeValue |= 0x08; break;  // قراءة / Read
        case DMADirection::BIDIRECTIONAL:    modeValue |= 0x00; break;  // تحقق / Verify
    }
    switch (mode) {
        case DMAMode::DEMAND:   modeValue |= 0x00; break;
        case DMAMode::SINGLE:   modeValue |= 0x40; break;
        case DMAMode::BLOCK:    modeValue |= 0x80; break;
        case DMAMode::CASCADE:  modeValue |= 0xC0; break;
    }
    IOPorts::outb(modePort, modeValue);

    // 4. تعيين العنوان / Set address
    uint16_t addrLow;
    if (isHighChannel) {
        addrLow = (address >> 1) & 0xFFFF;  // القنوات العليا تستخدم عناوين كلمات / High channels use word addresses
    } else {
        addrLow = address & 0xFFFF;
    }
    IOPorts::outb(addrPorts[channel], addrLow & 0xFF);
    IOPorts::outb(addrPorts[channel], (addrLow >> 8) & 0xFF);

    // 5. تعيين صفحة الذاكرة / Set page register
    IOPorts::outb(pagePorts[channel], (address >> 16) & 0xFF);

    // 6. تعيين العدد / Set count (count - 1)
    IOPorts::outb(clearPort, 0xFF); // مسح flip-flop مرة أخرى / Clear flip-flop again
    uint16_t actualCount = count - 1;
    IOPorts::outb(countPorts[channel], actualCount & 0xFF);
    IOPorts::outb(countPorts[channel], (actualCount >> 8) & 0xFF);
}

void DMAManager::enableLegacyChannel(uint8_t channel) {
    if (channel >= DMAConstants::MAX_CHANNELS) return;

    bool isHighChannel = (channel >= 4);
    uint8_t actualChannel = isHighChannel ? (channel - 4) : channel;
    uint16_t maskPort = isHighChannel ? DMAConstants::DMA2_MASK : DMAConstants::DMA1_MASK;

    IOPorts::outb(maskPort, actualChannel); // إلغاء التقنيع = تفعيل / Unmask = enable
}

void DMAManager::disableLegacyChannel(uint8_t channel) {
    if (channel >= DMAConstants::MAX_CHANNELS) return;

    bool isHighChannel = (channel >= 4);
    uint8_t actualChannel = isHighChannel ? (channel - 4) : channel;
    uint16_t maskPort = isHighChannel ? DMAConstants::DMA2_MASK : DMAConstants::DMA1_MASK;

    IOPorts::outb(maskPort, 0x04 | actualChannel); // تقنيع = تعطيل / Mask = disable
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string DMAManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير DMA / DMA Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "الحالة / Status: " << (isInitialized_ ? "مهيأ / Initialized" : "غير مهيأ / Not initialized") << "\n";
    report << "إجمالي عمليات النقل / Total Transfers: " << totalTransfers_ << "\n";
    report << "إجمالي البايتات / Total Bytes: " << totalBytes_ << "\n";
    report << "عمليات نشطة / Active Transfers: " << activeTransfers_.size() << "\n\n";

    if (!activeTransfers_.empty()) {
        report << std::left
               << std::setw(8)  << "المعرف"
               << std::setw(18) << "المصدر"
               << std::setw(18) << "الوجهة"
               << std::setw(12) << "الحجم"
               << std::setw(15) << "الحالة"
               << "\n";
        report << std::string(70, '-') << "\n";

        for (const auto& t : activeTransfers_) {
            const char* statusStr;
            switch (t.status) {
                case DMAStatus::IDLE:        statusStr = "خامل / Idle"; break;
                case DMAStatus::IN_PROGRESS: statusStr = "جاري / Working"; break;
                case DMAStatus::COMPLETED:   statusStr = "مكتمل / Done"; break;
                case DMAStatus::ERROR:       statusStr = "خطأ / Error"; break;
                case DMAStatus::TIMEOUT:     statusStr = "مهلة / Timeout"; break;
            }

            report << std::left
                   << std::setw(8)  << t.transferId
                   << "0x" << std::hex << std::setw(16) << t.sourceAddress
                   << "0x" << std::setw(16) << t.destAddress
                   << std::dec << std::setw(12) << t.size
                   << std::setw(15) << statusStr
                   << "\n";
        }
    }

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
