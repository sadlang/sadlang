/*
 * ============================================================================
 * وحدة بروتوكول الإقلاع - لغة ص
 * Boot Protocol Module - Sad Language
 * ============================================================================
 *
 * @brief (AR) دعم بروتوكولات الإقلاع: Multiboot2 و UEFI
 *              تحليل معلومات الإقلاع، خريطة الذاكرة، سطر أوامر النواة
 *
 * @brief (EN) Boot protocol support: Multiboot2 and UEFI
 *              Boot info parsing, memory map, kernel command line
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_BOOT_H
#define SAD_LOW_LEVEL_BOOT_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace Sad {
namespace LowLevel {

// ============================================================================
// Multiboot2 Header Tags / علامات ترويسة Multiboot2
// ============================================================================

#pragma pack(push, 1)

/// @brief (AR) ترويسة Multiboot2 / (EN) Multiboot2 Header
struct Multiboot2Header {
    uint32_t magic;             // 0xE85250D6
    uint32_t architecture;      // 0 = i386 protected mode
    uint32_t headerLength;
    uint32_t checksum;          // -(magic + arch + length)
};

/// @brief (AR) علامة ترويسة / (EN) Header tag
struct Multiboot2HeaderTag {
    uint16_t type;
    uint16_t flags;
    uint32_t size;
};

// ============================================================================
// Multiboot2 Boot Information / معلومات إقلاع Multiboot2
// ============================================================================

/// @brief (AR) ترويسة معلومات الإقلاع / (EN) Boot info header
struct Multiboot2BootInfo {
    uint32_t totalSize;
    uint32_t reserved;
};

/// @brief (AR) علامة معلومات / (EN) Info tag
struct Multiboot2Tag {
    uint32_t type;
    uint32_t size;
};

/// @brief (AR) أنواع العلامات / (EN) Tag types
enum class Multiboot2TagType : uint32_t {
    END             = 0,
    CMDLINE         = 1,
    BOOT_LOADER     = 2,
    MODULE          = 3,
    BASIC_MEMINFO   = 4,
    BOOTDEV         = 5,
    MMAP            = 6,
    VBE             = 7,
    FRAMEBUFFER     = 8,
    ELF_SECTIONS    = 9,
    APM             = 10,
    EFI32           = 11,
    EFI64           = 12,
    SMBIOS          = 13,
    ACPI_OLD        = 14,
    ACPI_NEW        = 15,
    NETWORK         = 16,
    EFI_MMAP        = 17,
    EFI_BS          = 18,
    EFI32_IH        = 19,
    EFI64_IH        = 20,
    LOAD_BASE_ADDR  = 21
};

/// @brief (AR) سطر أوامر النواة / (EN) Command line tag
struct Multiboot2CmdlineTag {
    uint32_t type;  // = 1
    uint32_t size;
    char     cmdline[];
};

/// @brief (AR) اسم محمّل الإقلاع / (EN) Boot loader name tag
struct Multiboot2BootloaderTag {
    uint32_t type;  // = 2
    uint32_t size;
    char     name[];
};

/// @brief (AR) معلومات الذاكرة الأساسية / (EN) Basic memory info tag
struct Multiboot2BasicMemInfoTag {
    uint32_t type;  // = 4
    uint32_t size;
    uint32_t memLower;  // KB من 0 إلى 640K / KB from 0 to 640KB
    uint32_t memUpper;  // KB من 1M فما فوق / KB from 1MB upward
};

/// @brief (AR) مدخل خريطة الذاكرة / (EN) Memory map entry
struct Multiboot2MmapEntry {
    uint64_t addr;
    uint64_t len;
    uint32_t type;      // 1=متاح, 2=محجوز, 3=ACPI, 4=محفوظ, 5=معيب
    uint32_t reserved;
};

/// @brief (AR) نوع منطقة الذاكرة / (EN) Memory region type
enum class MemoryRegionType : uint32_t {
    AVAILABLE       = 1,    // متاحة / Available
    RESERVED        = 2,    // محجوزة / Reserved
    ACPI_RECLAIMABLE = 3,   // ACPI قابلة للاسترداد / ACPI reclaimable
    ACPI_NVS        = 4,    // ACPI NVS
    BAD_MEMORY      = 5     // ذاكرة معيبة / Bad memory
};

/// @brief (AR) علامة خريطة الذاكرة / (EN) Memory map tag
struct Multiboot2MmapTag {
    uint32_t type;          // = 6
    uint32_t size;
    uint32_t entrySize;
    uint32_t entryVersion;
    Multiboot2MmapEntry entries[];
};

/// @brief (AR) علامة المخزن المؤقت / (EN) Framebuffer tag
struct Multiboot2FramebufferTag {
    uint32_t type;          // = 8
    uint32_t size;
    uint64_t addr;          // عنوان المخزن / FB address
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t  bpp;
    uint8_t  fbType;        // 0=indexed, 1=RGB, 2=EGA text
    uint16_t reserved;
    // بيانات إضافية حسب النوع / Additional data depending on type
};

/// @brief (AR) علامة RSDP القديم / (EN) Old ACPI RSDP tag
struct Multiboot2AcpiOldTag {
    uint32_t type;  // = 14
    uint32_t size;
    uint8_t  rsdp[];
};

/// @brief (AR) علامة RSDP الجديدة / (EN) New ACPI RSDP tag
struct Multiboot2AcpiNewTag {
    uint32_t type;  // = 15
    uint32_t size;
    uint8_t  rsdp[];
};

/// @brief (AR) علامة وحدة / (EN) Module tag
struct Multiboot2ModuleTag {
    uint32_t type;      // = 3
    uint32_t size;
    uint32_t modStart;  // بداية الوحدة / Module start address
    uint32_t modEnd;    // نهاية الوحدة / Module end address
    char     cmdline[];
};

#pragma pack(pop)

// ============================================================================
// بيانات محللة / Parsed Boot Data
// ============================================================================

/// @brief (AR) منطقة ذاكرة محللة / (EN) Parsed memory region
struct BootMemoryRegion {
    uint64_t baseAddress;
    uint64_t length;
    MemoryRegionType type;

    std::string typeName() const {
        switch (type) {
            case MemoryRegionType::AVAILABLE:        return "متاحة / Available";
            case MemoryRegionType::RESERVED:         return "محجوزة / Reserved";
            case MemoryRegionType::ACPI_RECLAIMABLE: return "ACPI قابلة للاسترداد";
            case MemoryRegionType::ACPI_NVS:         return "ACPI NVS";
            case MemoryRegionType::BAD_MEMORY:       return "معيبة / Bad";
            default:                                  return "غير معروفة / Unknown";
        }
    }
};

/// @brief (AR) معلومات وحدة محملة / (EN) Loaded module info
struct BootModule {
    uint64_t startAddress;
    uint64_t endAddress;
    std::string cmdline;
};

/// @brief (AR) معلومات المخزن من الإقلاع / (EN) Boot framebuffer info
struct BootFramebuffer {
    uint64_t address;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t  bpp;
    uint8_t  type;
};

// ============================================================================
// مدير الإقلاع / Boot Manager
// ============================================================================

/**
 * @brief (AR) مدير معلومات الإقلاع - تحليل Multiboot2 و UEFI
 * @brief (EN) Boot Info Manager - Multiboot2 and UEFI parsing
 */
class BootManager {
public:
    /// @brief (AR) الحصول على المثيل الوحيد / (EN) Get singleton instance
    static BootManager& getInstance();

    // ========================================================================
    // التهيئة / Initialization
    // ========================================================================

    /// @brief (AR) تحليل معلومات Multiboot2 / (EN) Parse Multiboot2 boot info
    bool parseMultiboot2(uint64_t bootInfoAddress);

    // ========================================================================
    // خريطة الذاكرة / Memory Map
    // ========================================================================

    /// @brief (AR) الحصول على خريطة الذاكرة / (EN) Get memory map
    const std::vector<BootMemoryRegion>& getMemoryMap() const { return memoryMap_; }

    /// @brief (AR) إجمالي الذاكرة المتاحة / (EN) Total available memory (bytes)
    uint64_t getTotalAvailableMemory() const;

    /// @brief (AR) أعلى عنوان متاح / (EN) Highest available address
    uint64_t getHighestAvailableAddress() const;

    // ========================================================================
    // معلومات الإقلاع / Boot Information
    // ========================================================================

    /// @brief (AR) سطر أوامر النواة / (EN) Kernel command line
    const std::string& getCommandLine() const { return commandLine_; }

    /// @brief (AR) اسم محمّل الإقلاع / (EN) Bootloader name
    const std::string& getBootloaderName() const { return bootloaderName_; }

    /// @brief (AR) الوحدات المحملة / (EN) Loaded modules
    const std::vector<BootModule>& getModules() const { return modules_; }

    /// @brief (AR) معلومات المخزن / (EN) Framebuffer info
    const BootFramebuffer& getFramebuffer() const { return framebuffer_; }

    /// @brief (AR) هل يوجد مخزن مؤقت / (EN) Has framebuffer
    bool hasFramebuffer() const { return hasFramebuffer_; }

    /// @brief (AR) عنوان RSDP / (EN) RSDP address
    uint64_t getRSDPAddress() const { return rsdpAddress_; }

    /// @brief (AR) هل يوجد RSDP / (EN) Has RSDP
    bool hasRSDP() const { return rsdpAddress_ != 0; }

    // ========================================================================
    // الذاكرة الأساسية / Basic Memory
    // ========================================================================

    /// @brief (AR) ذاكرة منخفضة (KB) / (EN) Lower memory (KB)
    uint32_t getLowerMemoryKB() const { return lowerMemoryKB_; }

    /// @brief (AR) ذاكرة عليا (KB) / (EN) Upper memory (KB)
    uint32_t getUpperMemoryKB() const { return upperMemoryKB_; }

    // ========================================================================
    // التقرير / Report
    // ========================================================================

    /// @brief (AR) توليد تقرير / (EN) Generate report
    std::string generateReport() const;

    /// @brief (AR) هل مهيأ / (EN) Is initialized
    bool isInitialized() const { return isInitialized_; }

private:
    BootManager();
    BootManager(const BootManager&) = delete;
    BootManager& operator=(const BootManager&) = delete;

    // البيانات / Data
    std::vector<BootMemoryRegion> memoryMap_;
    std::vector<BootModule>       modules_;
    std::string commandLine_;
    std::string bootloaderName_;
    BootFramebuffer framebuffer_;
    bool     hasFramebuffer_;
    uint64_t rsdpAddress_;
    uint32_t lowerMemoryKB_;
    uint32_t upperMemoryKB_;
    bool     isInitialized_;
};

// ============================================================================
// ثوابت / Constants
// ============================================================================

namespace BootConstants {
    static constexpr uint32_t MULTIBOOT2_MAGIC        = 0xE85250D6;
    static constexpr uint32_t MULTIBOOT2_BOOTLOADER_MAGIC = 0x36D76289;
    static constexpr uint32_t MULTIBOOT2_ARCH_I386    = 0;
}

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_BOOT_H
