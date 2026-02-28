/*
 * ============================================================================
 *  وحدة التخزين NVMe/AHCI — لغة ص
 *  NVMe/AHCI Storage Module — Sad Language
 * ============================================================================
 *
 * @brief (AR) وحدة إدارة أقراص التخزين عالية السرعة (NVMe) وأقراص SATA (AHCI)
 *             تدعم: اكتشاف الأقراص، قراءة/كتابة القطاعات، معلومات S.M.A.R.T،
 *             إدارة الأقسام، والتعامل مع نظام الملفات البسيط
 * @brief (EN) High-speed storage management module for NVMe and SATA/AHCI drives.
 *             Supports: drive discovery, sector read/write, S.M.A.R.T info,
 *             partition management, and simple filesystem operations
 *
 * @details
 * (AR) هذه الوحدة توفر واجهة برمجية موحدة للتعامل مع أجهزة التخزين على مستوى
 *      منخفض. في بيئة سطح المكتب (Windows/Linux)، تعمل كمحاكاة آمنة.
 *      في بيئة النواة (bare-metal)، تصل مباشرة للأجهزة عبر MMIO وPCI.
 *
 *      الأقسام الرئيسية:
 *        1. اكتشاف الأقراص — مسح ناقل PCI للعثور على متحكمات NVMe/AHCI
 *        2. قراءة/كتابة — نقل البيانات من/إلى القطاعات
 *        3. معلومات القرص — الاسم، الحجم، الحرارة، حالة S.M.A.R.T
 *        4. إدارة الأقسام — قراءة جدول GPT/MBR
 *        5. نظام ملفات بسيط — قراءة FAT32/ext2 الأساسية
 *
 * (EN) This module provides a unified API for low-level storage device interaction.
 *      On desktop (Windows/Linux), it works as a safe simulation.
 *      On bare-metal, it accesses hardware directly via MMIO and PCI.
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_NVME_H
#define SAD_LOW_LEVEL_NVME_H

#include <cstdint>
#include <string>
#include <vector>

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) ثوابت التخزين / (EN) Storage Constants
// ============================================================================
namespace StorageConstants {
    // (AR) حجم القطاع الافتراضي / (EN) Default sector size
    constexpr uint32_t SECTOR_SIZE = 512;
    // (AR) حجم القطاع لأقراص NVMe المتقدمة / (EN) NVMe advanced sector size
    constexpr uint32_t NVME_SECTOR_SIZE = 4096;
    // (AR) أقصى عدد أقراص مدعوم / (EN) Maximum supported drives
    constexpr int MAX_DRIVES = 32;
    // (AR) أقصى عدد أقسام لكل قرص / (EN) Max partitions per drive
    constexpr int MAX_PARTITIONS = 128;
    // (AR) توقيع GPT / (EN) GPT signature
    constexpr uint64_t GPT_SIGNATURE = 0x5452415020494645ULL; // "EFI PART"
    // (AR) توقيع MBR / (EN) MBR boot signature
    constexpr uint16_t MBR_SIGNATURE = 0xAA55;
    // (AR) معرّف فئة PCI لمتحكم التخزين / (EN) PCI class ID for storage
    constexpr uint8_t PCI_CLASS_STORAGE = 0x01;
    // (AR) معرّف فئة فرعية NVMe / (EN) NVMe subclass
    constexpr uint8_t PCI_SUBCLASS_NVME = 0x08;
    // (AR) معرّف فئة فرعية AHCI / (EN) AHCI subclass
    constexpr uint8_t PCI_SUBCLASS_AHCI = 0x06;
}

// ============================================================================
// (AR) أنواع المتحكمات / (EN) Controller Types
// ============================================================================
enum class StorageType : uint8_t {
    NVME  = 0,   // (AR) قرص NVMe عالي السرعة / (EN) High-speed NVMe
    AHCI  = 1,   // (AR) قرص SATA/AHCI / (EN) SATA/AHCI drive
    IDE   = 2,   // (AR) قرص IDE قديم / (EN) Legacy IDE drive
    RAMDISK = 3, // (AR) قرص ذاكرة / (EN) RAM disk
    UNKNOWN = 255
};

// ============================================================================
// (AR) حالة القرص / (EN) Drive State
// ============================================================================
enum class DriveState : uint8_t {
    NOT_PRESENT = 0, // (AR) غير موجود / (EN) Not present
    PRESENT     = 1, // (AR) موجود لكن غير مهيأ / (EN) Present but not initialized
    READY       = 2, // (AR) جاهز للاستخدام / (EN) Ready for use
    ERROR       = 3, // (AR) خطأ في القرص / (EN) Drive error
    BUSY        = 4  // (AR) مشغول / (EN) Busy
};

// ============================================================================
// (AR) نوع القسم / (EN) Partition Type
// ============================================================================
enum class PartitionScheme : uint8_t {
    NONE = 0, // (AR) بدون أقسام / (EN) No partitions
    MBR  = 1, // (AR) جدول أقسام MBR / (EN) MBR partition table
    GPT  = 2  // (AR) جدول أقسام GPT / (EN) GPT partition table
};

// ============================================================================
// (AR) معلومات القرص / (EN) Drive Information
// ============================================================================
struct DriveInfo {
    int id;                     // (AR) معرّف القرص / (EN) Drive ID
    StorageType type;           // (AR) نوع المتحكم / (EN) Controller type
    DriveState state;           // (AR) حالة القرص / (EN) Drive state
    std::string model;          // (AR) اسم الموديل / (EN) Model name
    std::string serial;         // (AR) الرقم التسلسلي / (EN) Serial number
    std::string firmware;       // (AR) إصدار البرنامج الثابت / (EN) Firmware version
    uint64_t totalSectors;      // (AR) إجمالي القطاعات / (EN) Total sectors
    uint32_t sectorSize;        // (AR) حجم القطاع بالبايت / (EN) Sector size in bytes
    uint64_t capacityBytes;     // (AR) السعة بالبايت / (EN) Capacity in bytes
    int temperature;            // (AR) درجة الحرارة (مئوية) / (EN) Temperature (°C)
    PartitionScheme scheme;     // (AR) نظام الأقسام / (EN) Partition scheme
    int pciSlot;                // (AR) منفذ PCI / (EN) PCI slot
};

// ============================================================================
// (AR) معلومات القسم / (EN) Partition Information
// ============================================================================
struct PartitionInfo {
    int index;                  // (AR) رقم القسم / (EN) Partition index
    std::string label;          // (AR) اسم القسم / (EN) Partition label
    std::string typeGUID;       // (AR) معرّف النوع / (EN) Type GUID
    uint64_t startLBA;          // (AR) بداية القسم (LBA) / (EN) Start LBA
    uint64_t endLBA;            // (AR) نهاية القسم (LBA) / (EN) End LBA  
    uint64_t sizeBytes;         // (AR) حجم القسم بالبايت / (EN) Partition size in bytes
    bool bootable;              // (AR) قابل للإقلاع / (EN) Bootable
    bool active;                // (AR) نشط / (EN) Active
};

// ============================================================================
// (AR) معلومات S.M.A.R.T / (EN) S.M.A.R.T Information
// ============================================================================
struct SmartInfo {
    bool supported;             // (AR) مدعوم / (EN) Supported
    bool healthy;               // (AR) القرص سليم / (EN) Drive is healthy
    int temperature;            // (AR) درجة الحرارة / (EN) Temperature
    int powerOnHours;           // (AR) ساعات التشغيل / (EN) Power-on hours
    uint64_t totalBytesWritten; // (AR) إجمالي البايتات المكتوبة / (EN) Total bytes written
    uint64_t totalBytesRead;    // (AR) إجمالي البايتات المقروءة / (EN) Total bytes read
    int percentUsed;            // (AR) نسبة الاستخدام / (EN) Percent used
    int availableSpare;         // (AR) القطاعات الاحتياطية / (EN) Available spare %
    std::string report;         // (AR) تقرير نصي / (EN) Text report
};

// ============================================================================
// (AR) مدير التخزين — نمط المفرد / (EN) Storage Manager — Singleton
// ============================================================================
class StorageManager {
public:
    // (AR) الحصول على المثيل الوحيد / (EN) Get singleton instance
    static StorageManager& getInstance() {
        static StorageManager instance;
        return instance;
    }

    // ════════════════════════════════════════════════════════════════════
    // (AR) 1. اكتشاف الأقراص / (EN) 1. Drive Discovery
    // ════════════════════════════════════════════════════════════════════

    /// (AR) مسح ناقل PCI للعثور على أقراص التخزين
    /// (EN) Scan PCI bus for storage drives
    int scanDrives();

    /// (AR) عدد الأقراص المكتشفة / (EN) Number of discovered drives
    int getDriveCount() const { return static_cast<int>(drives_.size()); }

    /// (AR) الحصول على معلومات قرص / (EN) Get drive info
    DriveInfo getDriveInfo(int driveId) const;

    /// (AR) تقرير شامل عن جميع الأقراص / (EN) Full report of all drives
    std::string generateReport() const;

    // ════════════════════════════════════════════════════════════════════
    // (AR) 2. قراءة وكتابة القطاعات / (EN) 2. Sector Read/Write
    // ════════════════════════════════════════════════════════════════════

    /// (AR) قراءة قطاع واحد / (EN) Read a single sector
    /// @param driveId معرّف القرص / Drive ID
    /// @param lba عنوان القطاع المنطقي / Logical Block Address
    /// @return البيانات كنص سداسي عشري أو سلسلة فارغة عند الفشل
    std::string readSector(int driveId, uint64_t lba) const;

    /// (AR) كتابة قطاع واحد / (EN) Write a single sector
    /// @return 0 نجاح، -1 فشل / 0 success, -1 failure
    int writeSector(int driveId, uint64_t lba, const std::string& data);

    /// (AR) قراءة عدة قطاعات / (EN) Read multiple sectors
    std::string readSectors(int driveId, uint64_t startLBA, int count) const;

    /// (AR) مسح (تصفير) قطاعات / (EN) Zero-fill sectors
    int zeroSectors(int driveId, uint64_t startLBA, int count);

    // ════════════════════════════════════════════════════════════════════
    // (AR) 3. معلومات S.M.A.R.T / (EN) 3. S.M.A.R.T Info
    // ════════════════════════════════════════════════════════════════════

    /// (AR) قراءة بيانات S.M.A.R.T / (EN) Read S.M.A.R.T data
    SmartInfo getSmartInfo(int driveId) const;

    /// (AR) هل القرص بصحة جيدة؟ / (EN) Is drive healthy?
    bool isDriveHealthy(int driveId) const;

    /// (AR) درجة حرارة القرص / (EN) Drive temperature
    int getDriveTemperature(int driveId) const;

    // ════════════════════════════════════════════════════════════════════
    // (AR) 4. إدارة الأقسام / (EN) 4. Partition Management
    // ════════════════════════════════════════════════════════════════════

    /// (AR) قراءة جدول الأقسام / (EN) Read partition table
    int readPartitionTable(int driveId);

    /// (AR) عدد الأقسام / (EN) Number of partitions
    int getPartitionCount(int driveId) const;

    /// (AR) معلومات قسم / (EN) Partition info
    PartitionInfo getPartitionInfo(int driveId, int partIndex) const;

    /// (AR) نظام الأقسام (MBR/GPT) / (EN) Partition scheme
    PartitionScheme getPartitionScheme(int driveId) const;

    /// (AR) إجمالي الحجم بالميغابايت / (EN) Total size in MB
    uint64_t getDriveCapacityMB(int driveId) const;

    // ════════════════════════════════════════════════════════════════════
    // (AR) 5. عمليات متقدمة / (EN) 5. Advanced Operations
    // ════════════════════════════════════════════════════════════════════

    /// (AR) مزامنة ذاكرة التخزين المؤقت / (EN) Flush cache
    int flushCache(int driveId);

    /// (AR) هل القرص NVMe? / (EN) Is drive NVMe?
    bool isNVMe(int driveId) const;

    /// (AR) هل القرص AHCI? / (EN) Is drive AHCI?
    bool isAHCI(int driveId) const;

    /// (AR) إنشاء قرص ذاكرة وهمي / (EN) Create a RAM disk
    int createRamdisk(uint64_t sizeBytes);

    /// (AR) إعادة تعيين الوحدة / (EN) Reset/reinitialize
    void reset();

private:
    StorageManager();
    ~StorageManager() = default;
    StorageManager(const StorageManager&) = delete;
    StorageManager& operator=(const StorageManager&) = delete;

    std::vector<DriveInfo> drives_;
    std::vector<std::vector<PartitionInfo>> partitions_; // (AR) أقسام لكل قرص
    std::vector<std::vector<uint8_t>> ramdisks_;         // (AR) أقراص الذاكرة
    bool scanned_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_NVME_H
