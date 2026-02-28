/*
 * ============================================================================
 * تنفيذ بروتوكول الإقلاع - لغة ص
 * Boot Protocol Implementation - Sad Language
 * ============================================================================
 */

#include "boot.h"
#include <cstring>
#include <sstream>
#include <iomanip>

namespace Sad {
namespace LowLevel {

// ============================================================================
// المثيل الوحيد / Singleton
// ============================================================================

BootManager& BootManager::getInstance() {
    static BootManager instance;
    return instance;
}

BootManager::BootManager()
    : hasFramebuffer_(false)
    , rsdpAddress_(0)
    , lowerMemoryKB_(0)
    , upperMemoryKB_(0)
    , isInitialized_(false) {
    std::memset(&framebuffer_, 0, sizeof(framebuffer_));
}

// ============================================================================
// تحليل Multiboot2 / Multiboot2 Parsing
// ============================================================================

bool BootManager::parseMultiboot2(uint64_t bootInfoAddress) {
    const Multiboot2BootInfo* info =
        reinterpret_cast<const Multiboot2BootInfo*>(bootInfoAddress);

    if (!info) return false;

    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(info) + 8;
    const uint8_t* end = reinterpret_cast<const uint8_t*>(info) + info->totalSize;

    while (ptr < end) {
        const Multiboot2Tag* tag = reinterpret_cast<const Multiboot2Tag*>(ptr);

        if (tag->type == static_cast<uint32_t>(Multiboot2TagType::END)) {
            break;
        }

        switch (static_cast<Multiboot2TagType>(tag->type)) {

            case Multiboot2TagType::CMDLINE: {
                const Multiboot2CmdlineTag* cmdTag =
                    reinterpret_cast<const Multiboot2CmdlineTag*>(tag);
                commandLine_ = cmdTag->cmdline;
                break;
            }

            case Multiboot2TagType::BOOT_LOADER: {
                const Multiboot2BootloaderTag* blTag =
                    reinterpret_cast<const Multiboot2BootloaderTag*>(tag);
                bootloaderName_ = blTag->name;
                break;
            }

            case Multiboot2TagType::MODULE: {
                const Multiboot2ModuleTag* modTag =
                    reinterpret_cast<const Multiboot2ModuleTag*>(tag);
                BootModule mod;
                mod.startAddress = modTag->modStart;
                mod.endAddress = modTag->modEnd;
                mod.cmdline = modTag->cmdline;
                modules_.push_back(mod);
                break;
            }

            case Multiboot2TagType::BASIC_MEMINFO: {
                const Multiboot2BasicMemInfoTag* memTag =
                    reinterpret_cast<const Multiboot2BasicMemInfoTag*>(tag);
                lowerMemoryKB_ = memTag->memLower;
                upperMemoryKB_ = memTag->memUpper;
                break;
            }

            case Multiboot2TagType::MMAP: {
                const Multiboot2MmapTag* mmapTag =
                    reinterpret_cast<const Multiboot2MmapTag*>(tag);
                uint32_t entrySize = mmapTag->entrySize;
                const uint8_t* entryPtr = reinterpret_cast<const uint8_t*>(mmapTag->entries);
                const uint8_t* entryEnd = reinterpret_cast<const uint8_t*>(tag) + tag->size;

                while (entryPtr < entryEnd) {
                    const Multiboot2MmapEntry* entry =
                        reinterpret_cast<const Multiboot2MmapEntry*>(entryPtr);

                    BootMemoryRegion region;
                    region.baseAddress = entry->addr;
                    region.length = entry->len;
                    region.type = static_cast<MemoryRegionType>(entry->type);
                    memoryMap_.push_back(region);

                    entryPtr += entrySize;
                }
                break;
            }

            case Multiboot2TagType::FRAMEBUFFER: {
                const Multiboot2FramebufferTag* fbTag =
                    reinterpret_cast<const Multiboot2FramebufferTag*>(tag);
                framebuffer_.address = fbTag->addr;
                framebuffer_.width = fbTag->width;
                framebuffer_.height = fbTag->height;
                framebuffer_.pitch = fbTag->pitch;
                framebuffer_.bpp = fbTag->bpp;
                framebuffer_.type = fbTag->fbType;
                hasFramebuffer_ = true;
                break;
            }

            case Multiboot2TagType::ACPI_OLD: {
                const Multiboot2AcpiOldTag* acpiTag =
                    reinterpret_cast<const Multiboot2AcpiOldTag*>(tag);
                rsdpAddress_ = reinterpret_cast<uint64_t>(acpiTag->rsdp);
                break;
            }

            case Multiboot2TagType::ACPI_NEW: {
                const Multiboot2AcpiNewTag* acpiTag =
                    reinterpret_cast<const Multiboot2AcpiNewTag*>(tag);
                rsdpAddress_ = reinterpret_cast<uint64_t>(acpiTag->rsdp);
                break;
            }

            default:
                break;
        }

        // الانتقال للعلامة التالية (محاذاة 8 بايت) / Move to next tag (8-byte aligned)
        uint32_t tagSize = (tag->size + 7) & ~7u;
        ptr += tagSize;
    }

    isInitialized_ = true;
    return true;
}

// ============================================================================
// خريطة الذاكرة / Memory Map
// ============================================================================

uint64_t BootManager::getTotalAvailableMemory() const {
    uint64_t total = 0;
    for (const auto& region : memoryMap_) {
        if (region.type == MemoryRegionType::AVAILABLE) {
            total += region.length;
        }
    }
    return total;
}

uint64_t BootManager::getHighestAvailableAddress() const {
    uint64_t highest = 0;
    for (const auto& region : memoryMap_) {
        if (region.type == MemoryRegionType::AVAILABLE) {
            uint64_t end = region.baseAddress + region.length;
            if (end > highest) highest = end;
        }
    }
    return highest;
}

// ============================================================================
// التقرير / Report
// ============================================================================

std::string BootManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير الإقلاع / Boot Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "الحالة / Status: " << (isInitialized_ ? "مهيأ / Initialized" : "غير مهيأ / Not initialized") << "\n";
    if (!bootloaderName_.empty())
        report << "محمّل الإقلاع / Bootloader: " << bootloaderName_ << "\n";
    if (!commandLine_.empty())
        report << "سطر الأوامر / Command Line: " << commandLine_ << "\n";

    report << "\n--- الذاكرة / Memory ---\n";
    report << "ذاكرة منخفضة / Lower: " << lowerMemoryKB_ << " KB\n";
    report << "ذاكرة عليا / Upper: " << upperMemoryKB_ << " KB\n";
    report << "إجمالي متاح / Total Available: "
           << (getTotalAvailableMemory() / (1024 * 1024)) << " MB\n\n";

    if (!memoryMap_.empty()) {
        report << "--- خريطة الذاكرة / Memory Map ---\n";
        report << std::left
               << std::setw(18) << "العنوان / Addr"
               << std::setw(14) << "الحجم / Size"
               << "النوع / Type\n";
        report << std::string(55, '-') << "\n";

        for (const auto& region : memoryMap_) {
            report << "0x" << std::hex << std::setw(16) << std::setfill('0') << region.baseAddress
                   << std::setfill(' ') << " "
                   << std::dec << std::setw(12) << (region.length / 1024) << " KB  "
                   << region.typeName() << "\n";
        }
    }

    if (hasFramebuffer_) {
        report << "\n--- المخزن المؤقت / Framebuffer ---\n";
        report << "العنوان / Address: 0x" << std::hex << framebuffer_.address << std::dec << "\n";
        report << "الأبعاد / Resolution: " << framebuffer_.width << "x" << framebuffer_.height << "\n";
        report << "عمق اللون / BPP: " << static_cast<int>(framebuffer_.bpp) << "\n";
        report << "Pitch: " << framebuffer_.pitch << " bytes\n";
    }

    if (!modules_.empty()) {
        report << "\n--- الوحدات / Modules ---\n";
        for (const auto& mod : modules_) {
            report << "  0x" << std::hex << mod.startAddress
                   << " - 0x" << mod.endAddress << std::dec
                   << " " << mod.cmdline << "\n";
        }
    }

    if (rsdpAddress_) {
        report << "\nACPI RSDP: 0x" << std::hex << rsdpAddress_ << std::dec << "\n";
    }

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
