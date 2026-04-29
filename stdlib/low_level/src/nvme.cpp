/*
 * ============================================================================
 *  وحدة التخزين NVMe/AHCI — التنفيذ
 *  NVMe/AHCI Storage Module — Implementation
 * ============================================================================
 *
 * @brief (AR) تنفيذ وحدة إدارة أقراص التخزين. تعمل كمحاكاة آمنة على سطح المكتب
 *             حيث لا يمكن الوصول للأجهزة الحقيقية من مساحة المستخدم.
 *             أقراص الذاكرة (RAMDISK) تدعم القراءة والكتابة الفعلية في الذاكرة.
 * @brief (EN) Storage management module implementation. Runs as safe simulation
 *             on desktop since real hardware can't be accessed from userspace.
 *             RAM disks support actual in-memory read/write operations.
 * ============================================================================
 */

#include "nvme.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) دوال مساعدة محلية / (EN) Local helper functions
// ============================================================================
namespace {

/// (AR) تحويل حرف سداسي عشري إلى قيمة رقمية (نصف بايت)
/// (EN) Convert a hex character to its numeric nibble value
inline uint8_t hexCharToNibble(char c) {
    if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<uint8_t>(c - 'a' + 10);
    if (c >= 'A' && c <= 'F') return static_cast<uint8_t>(c - 'A' + 10);
    return 0;
}

/// (AR) العثور على فهرس القرص الذاكري ضمن مصفوفة ramdisks_
///      يعمل بعدّ أقراص RAMDISK التي تسبق القرص المطلوب
/// (EN) Find the ramdisk buffer index within ramdisks_ vector
///      Works by counting RAMDISK-type drives before the given drive ID
int findRamdiskIndex(const std::vector<DriveInfo>& drives, int driveId) {
    int ramdiskIdx = 0;
    for (int i = 0; i < driveId; ++i) {
        if (i < Sad::Security::SafeArithmetic::assertSafeCast<int>(drives.size(), "nvme_size") &&
            drives[i].type == StorageType::RAMDISK) {
            ++ramdiskIdx;
        }
    }
    return ramdiskIdx;
}

} // anonymous namespace

// ============================================================================
// (AR) الباني الخاص — تهيئة أولية
// (EN) Private constructor — initial setup
// ============================================================================
StorageManager::StorageManager()
    : scanned_(false) {
    // (AR) لا يتم المسح تلقائياً — يجب استدعاء scanDrives() يدوياً
    // (EN) No automatic scan — scanDrives() must be called manually
}

// ============================================================================
// (AR) 1. اكتشاف الأقراص / (EN) 1. Drive Discovery
// ============================================================================

/// (AR) مسح ناقل PCI للعثور على أقراص التخزين (محاكاة)
///      ينشئ قرص NVMe وقرص AHCI وهميين للاختبار
/// (EN) Scan PCI bus for storage drives (simulation)
///      Creates one simulated NVMe drive and one AHCI drive for testing
int StorageManager::scanDrives() {
    // (AR) إعادة تعيين أي بيانات سابقة / (EN) Reset any previous data
    drives_.clear();
    partitions_.clear();
    // (AR) لا نمسح ramdisks_ لأنها قد تحوي بيانات مخصصة من المستخدم
    // (EN) Don't clear ramdisks_ as they may hold user-allocated data

    // ────────────────────────────────────────────────────────────────
    // (AR) قرص NVMe وهمي — محاكاة Samsung 990 PRO
    // (EN) Simulated NVMe drive — simulating Samsung 990 PRO
    // ────────────────────────────────────────────────────────────────
    DriveInfo nvmeDrive;
    nvmeDrive.id            = 0;
    nvmeDrive.type          = StorageType::NVME;
    nvmeDrive.state         = DriveState::READY;
    nvmeDrive.model         = "Samsung 990 PRO 1TB (simulated)";
    nvmeDrive.serial        = "S6Z2NF0W800001X";
    nvmeDrive.firmware      = "4B2QJXE7";
    nvmeDrive.sectorSize    = StorageConstants::NVME_SECTOR_SIZE;
    nvmeDrive.totalSectors  = 244190646;  // (AR) ~1 تيرابايت / (EN) ~1 TB
    nvmeDrive.capacityBytes = static_cast<uint64_t>(nvmeDrive.totalSectors)
                              * nvmeDrive.sectorSize;
    nvmeDrive.temperature   = 35;
    nvmeDrive.scheme        = PartitionScheme::GPT;
    nvmeDrive.pciSlot       = 1;
    drives_.push_back(nvmeDrive);

    // ────────────────────────────────────────────────────────────────
    // (AR) قرص AHCI وهمي — محاكاة WD Blue
    // (EN) Simulated AHCI drive — simulating WD Blue
    // ────────────────────────────────────────────────────────────────
    DriveInfo ahciDrive;
    ahciDrive.id            = 1;
    ahciDrive.type          = StorageType::AHCI;
    ahciDrive.state         = DriveState::READY;
    ahciDrive.model         = "WD Blue SA510 500GB (simulated)";
    ahciDrive.serial        = "WD-WCC6Y0VP3R5A";
    ahciDrive.firmware      = "52022100";
    ahciDrive.sectorSize    = StorageConstants::SECTOR_SIZE;
    ahciDrive.totalSectors  = 976773168;  // (AR) ~500 غيغابايت / (EN) ~500 GB
    ahciDrive.capacityBytes = static_cast<uint64_t>(ahciDrive.totalSectors)
                              * ahciDrive.sectorSize;
    ahciDrive.temperature   = 32;
    ahciDrive.scheme        = PartitionScheme::MBR;
    ahciDrive.pciSlot       = 2;
    drives_.push_back(ahciDrive);

    // (AR) إنشاء مصفوفات أقسام فارغة لكل قرص
    // (EN) Create empty partition arrays for each drive
    partitions_.resize(drives_.size());

    scanned_ = true;

    // (AR) إرجاع عدد الأقراص المكتشفة / (EN) Return number of discovered drives
    return Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size");
}

/// (AR) الحصول على معلومات قرص بمعرّفه
/// (EN) Get drive info by its ID
DriveInfo StorageManager::getDriveInfo(int driveId) const {
    // (AR) التحقق من صلاحية المعرّف / (EN) Validate ID
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        DriveInfo empty;
        empty.id            = -1;
        empty.type          = StorageType::UNKNOWN;
        empty.state         = DriveState::NOT_PRESENT;
        empty.model         = "";
        empty.serial        = "";
        empty.firmware      = "";
        empty.totalSectors  = 0;
        empty.sectorSize    = 0;
        empty.capacityBytes = 0;
        empty.temperature   = 0;
        empty.scheme        = PartitionScheme::NONE;
        empty.pciSlot       = -1;
        return empty;
    }
    return drives_[driveId];
}

/// (AR) توليد تقرير شامل عن جميع الأقراص بتنسيق نصي
/// (EN) Generate a comprehensive formatted report of all drives
std::string StorageManager::generateReport() const {
    std::ostringstream oss;

    oss << "======================================================\n";
    oss << "       Storage Drive Report / تقرير أقراص التخزين\n";
    oss << "======================================================\n\n";

    if (drives_.empty()) {
        oss << "  (AR) لا توجد أقراص مكتشفة. استدعِ scanDrives() أولاً.\n";
        oss << "  (EN) No drives discovered. Call scanDrives() first.\n";
        return oss.str();
    }

    oss << "  إجمالي الأقراص / Total drives: " << drives_.size() << "\n\n";

    for (const auto& drive : drives_) {
        oss << "  -------------------------------------------------------\n";
        oss << "  قرص / Drive #" << drive.id << "\n";
        oss << "  -------------------------------------------------------\n";

        // (AR) نوع المتحكم / (EN) Controller type
        oss << "  النوع / Type:        ";
        switch (drive.type) {
            case StorageType::NVME:    oss << "NVMe";    break;
            case StorageType::AHCI:    oss << "AHCI";    break;
            case StorageType::IDE:     oss << "IDE";     break;
            case StorageType::RAMDISK: oss << "RAMDISK"; break;
            default:                   oss << "UNKNOWN"; break;
        }
        oss << "\n";

        // (AR) حالة القرص / (EN) Drive state
        oss << "  الحالة / State:      ";
        switch (drive.state) {
            case DriveState::NOT_PRESENT: oss << "غير موجود / Not Present"; break;
            case DriveState::PRESENT:     oss << "موجود / Present";         break;
            case DriveState::READY:       oss << "جاهز / Ready";            break;
            case DriveState::ERROR:       oss << "خطأ / Error";             break;
            case DriveState::BUSY:        oss << "مشغول / Busy";            break;
        }
        oss << "\n";

        oss << "  الموديل / Model:     " << drive.model    << "\n";
        oss << "  الرقم / Serial:      " << drive.serial   << "\n";
        oss << "  البرنامج / Firmware:  " << drive.firmware << "\n";

        // (AR) حساب السعة بالغيغابايت / (EN) Compute capacity in GB
        double capacityGB = static_cast<double>(drive.capacityBytes)
                            / (1024.0 * 1024.0 * 1024.0);
        oss << std::fixed << std::setprecision(2);
        oss << "  السعة / Capacity:    " << capacityGB << " GB"
            << " (" << drive.totalSectors << " قطاع / sectors)\n";
        oss << "  حجم القطاع / Sector: " << drive.sectorSize << " بايت / bytes\n";
        oss << "  الحرارة / Temp:      " << drive.temperature << " °C\n";
        oss << "  PCI Slot:            " << drive.pciSlot << "\n";

        // (AR) عرض عدد الأقسام إن وجدت / (EN) Show partition count if available
        if (drive.id >= 0 &&
            drive.id < Sad::Security::SafeArithmetic::assertSafeCast<int>(partitions_.size(), "nvme_size")) {
            oss << "  الأقسام / Partitions: "
                << partitions_[drive.id].size() << "\n";
        }

        oss << "  -------------------------------------------------------\n\n";
    }

    return oss.str();
}

// ============================================================================
// (AR) 2. قراءة وكتابة القطاعات / (EN) 2. Sector Read/Write
// ============================================================================

/// (AR) قراءة قطاع واحد — تعمل فقط لأقراص الذاكرة (RAMDISK)
///      الأقراص الوهمية غير الذاكرية ترجع سلسلة فارغة
/// (EN) Read a single sector — only works for RAMDISK drives
///      Non-ramdisk simulated drives return an empty string
std::string StorageManager::readSector(int driveId, uint64_t lba) const {
    // (AR) التحقق من صلاحية المعرّف / (EN) Validate drive ID
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return "";
    }

    const DriveInfo& drive = drives_[driveId];

    // (AR) أقراص الذاكرة — قراءة حقيقية من المخزن المؤقت
    // (EN) RAM disks — actual read from the buffer
    if (drive.type == StorageType::RAMDISK) {
        // (AR) حساب فهرس القرص الذاكري / (EN) Calculate ramdisk index
        int ramdiskIdx = findRamdiskIndex(drives_, driveId);
        if (ramdiskIdx < 0 ||
            ramdiskIdx >= Sad::Security::SafeArithmetic::assertSafeCast<int>(ramdisks_.size(), "nvme_size")) {
            return "";
        }

        const auto& disk = ramdisks_[ramdiskIdx];
        uint64_t offset  = lba * drive.sectorSize;
        uint64_t end     = offset + drive.sectorSize;

        // (AR) التحقق من الحدود / (EN) Bounds check
        if (end > disk.size()) {
            return "";
        }

        // (AR) تحويل البيانات إلى نص سداسي عشري / (EN) Convert data to hex string
        std::ostringstream hexOut;
        hexOut << std::hex << std::setfill('0');
        for (uint64_t i = offset; i < end; ++i) {
            hexOut << std::setw(2) << static_cast<int>(disk[i]);
        }
        return hexOut.str();
    }

    // (AR) الأقراص غير الذاكرية — محاكاة فقط، لا بيانات حقيقية
    // (EN) Non-ramdisk drives — simulation only, no real data
    return "";
}

/// (AR) كتابة قطاع واحد — تعمل فقط لأقراص الذاكرة (RAMDISK)
/// (EN) Write a single sector — only works for RAMDISK drives
int StorageManager::writeSector(int driveId, uint64_t lba,
                                const std::string& data) {
    // (AR) التحقق من صلاحية المعرّف / (EN) Validate drive ID
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return -1;
    }

    const DriveInfo& drive = drives_[driveId];

    // (AR) الكتابة مدعومة فقط لأقراص الذاكرة
    // (EN) Write only supported for RAM disks
    if (drive.type != StorageType::RAMDISK) {
        return -1;
    }

    int ramdiskIdx = findRamdiskIndex(drives_, driveId);
    if (ramdiskIdx < 0 ||
        ramdiskIdx >= Sad::Security::SafeArithmetic::assertSafeCast<int>(ramdisks_.size(), "nvme_size")) {
        return -1;
    }

    auto& disk      = ramdisks_[ramdiskIdx];
    uint64_t offset = lba * drive.sectorSize;
    uint64_t end    = offset + drive.sectorSize;

    // (AR) التحقق من الحدود / (EN) Bounds check
    if (end > disk.size()) {
        return -1;
    }

    // (AR) تصفير القطاع أولاً ثم كتابة البيانات
    // (EN) Zero the sector first, then write data
    //      إذا كان النص أقصر من حجم القطاع، يبقى الباقي أصفاراً
    //      If data is shorter than sector size, remainder stays zero
    std::memset(&disk[offset], 0, drive.sectorSize);

    size_t bytesToWrite = std::min(
        static_cast<size_t>(data.size() / 2),
        static_cast<size_t>(drive.sectorSize)
    );

    for (size_t i = 0; i < bytesToWrite; ++i) {
        // (AR) تحويل حرفين سداسيين إلى بايت واحد
        // (EN) Convert two hex chars to a single byte
        uint8_t high = hexCharToNibble(data[i * 2]);
        uint8_t low  = hexCharToNibble(data[i * 2 + 1]);
        disk[offset + i] = static_cast<uint8_t>((high << 4) | low);
    }

    return 0;  // (AR) نجاح / (EN) Success
}

/// (AR) قراءة عدة قطاعات متتالية
/// (EN) Read multiple consecutive sectors
std::string StorageManager::readSectors(int driveId, uint64_t startLBA,
                                        int count) const {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size") ||
        count <= 0) {
        return "";
    }

    std::string result;
    for (int i = 0; i < count; ++i) {
        std::string sectorData = readSector(driveId, startLBA + i);
        if (sectorData.empty()) {
            // (AR) توقف عند أول قطاع فاشل / (EN) Stop at first failed sector
            break;
        }
        result += sectorData;
    }
    return result;
}

/// (AR) مسح (تصفير) قطاعات — تعمل فقط لأقراص الذاكرة
/// (EN) Zero-fill sectors — only works for RAM disks
int StorageManager::zeroSectors(int driveId, uint64_t startLBA, int count) {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size") ||
        count <= 0) {
        return -1;
    }

    const DriveInfo& drive = drives_[driveId];

    // (AR) التصفير مدعوم فقط لأقراص الذاكرة
    // (EN) Zero-fill only supported for RAM disks
    if (drive.type != StorageType::RAMDISK) {
        return -1;
    }

    int ramdiskIdx = findRamdiskIndex(drives_, driveId);
    if (ramdiskIdx < 0 ||
        ramdiskIdx >= Sad::Security::SafeArithmetic::assertSafeCast<int>(ramdisks_.size(), "nvme_size")) {
        return -1;
    }

    auto& disk = ramdisks_[ramdiskIdx];

    for (int i = 0; i < count; ++i) {
        uint64_t sectorOffset = (startLBA + i) * drive.sectorSize;
        uint64_t sectorEnd    = sectorOffset + drive.sectorSize;
        if (sectorEnd > disk.size()) {
            return -1;
        }
        std::memset(&disk[sectorOffset], 0, drive.sectorSize);
    }

    return 0;  // (AR) نجاح / (EN) Success
}

// ============================================================================
// (AR) 3. معلومات S.M.A.R.T / (EN) 3. S.M.A.R.T Info
// ============================================================================

/// (AR) قراءة بيانات S.M.A.R.T المحاكاة لقرص معين
/// (EN) Read simulated S.M.A.R.T data for a given drive
SmartInfo StorageManager::getSmartInfo(int driveId) const {
    SmartInfo info;

    // (AR) قرص غير صالح / (EN) Invalid drive
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        info.supported         = false;
        info.healthy           = false;
        info.temperature       = 0;
        info.powerOnHours      = 0;
        info.totalBytesWritten = 0;
        info.totalBytesRead    = 0;
        info.percentUsed       = 0;
        info.availableSpare    = 0;
        info.report            = "خطأ: قرص غير صالح / Error: Invalid drive";
        return info;
    }

    const DriveInfo& drive = drives_[driveId];

    // (AR) أقراص الذاكرة لا تدعم S.M.A.R.T
    // (EN) RAM disks don't support S.M.A.R.T
    if (drive.type == StorageType::RAMDISK) {
        info.supported         = false;
        info.healthy           = true;
        info.temperature       = 0;
        info.powerOnHours      = 0;
        info.totalBytesWritten = 0;
        info.totalBytesRead    = 0;
        info.percentUsed       = 0;
        info.availableSpare    = 100;
        info.report            = "قرص ذاكرة — S.M.A.R.T غير مدعوم / "
                                 "RAM disk — S.M.A.R.T not supported";
        return info;
    }

    // (AR) قيم محاكاة واقعية / (EN) Realistic simulated values
    info.supported = true;
    info.healthy   = true;

    if (drive.type == StorageType::NVME) {
        // (AR) قيم NVMe نموذجية / (EN) Typical NVMe values
        info.temperature       = 35;
        info.powerOnHours      = 8760;     // (AR) سنة واحدة / (EN) One year
        info.totalBytesWritten = 15ULL * 1024 * 1024 * 1024 * 1024; // 15 TB
        info.totalBytesRead    = 25ULL * 1024 * 1024 * 1024 * 1024; // 25 TB
        info.percentUsed       = 3;        // (AR) 3% مستخدم / (EN) 3% used
        info.availableSpare    = 100;      // (AR) كامل / (EN) Full spare
    } else {
        // (AR) قيم AHCI/SATA نموذجية / (EN) Typical AHCI/SATA values
        info.temperature       = 32;
        info.powerOnHours      = 4380;     // (AR) ستة أشهر / (EN) Six months
        info.totalBytesWritten = 5ULL * 1024 * 1024 * 1024 * 1024;  // 5 TB
        info.totalBytesRead    = 12ULL * 1024 * 1024 * 1024 * 1024; // 12 TB
        info.percentUsed       = 1;
        info.availableSpare    = 100;
    }

    // (AR) بناء تقرير نصي / (EN) Build text report
    std::ostringstream oss;
    oss << "=== تقرير S.M.A.R.T / S.M.A.R.T Report ===\n";
    oss << "القرص / Drive:         " << drive.model << "\n";
    oss << "الحالة / Status:       "
        << (info.healthy ? "سليم / Healthy" : "تحذير / Warning") << "\n";
    oss << "الحرارة / Temp:        " << info.temperature << " °C\n";
    oss << "ساعات التشغيل / Hours: " << info.powerOnHours << "\n";
    oss << "نسبة الاستهلاك / Used: " << info.percentUsed << "%\n";
    oss << "الاحتياطي / Spare:     " << info.availableSpare << "%\n";

    double tbWritten = static_cast<double>(info.totalBytesWritten)
                       / (1024.0 * 1024.0 * 1024.0 * 1024.0);
    double tbRead    = static_cast<double>(info.totalBytesRead)
                       / (1024.0 * 1024.0 * 1024.0 * 1024.0);
    oss << std::fixed << std::setprecision(1);
    oss << "المكتوب / Written:     " << tbWritten << " TB\n";
    oss << "المقروء / Read:        " << tbRead << " TB\n";

    info.report = oss.str();
    return info;
}

/// (AR) هل القرص بصحة جيدة؟ / (EN) Is drive healthy?
bool StorageManager::isDriveHealthy(int driveId) const {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return false;
    }
    // (AR) أقراص الذاكرة دائماً سليمة / (EN) RAM disks are always healthy
    if (drives_[driveId].type == StorageType::RAMDISK) {
        return true;
    }
    // (AR) في المحاكاة، جميع الأقراص سليمة / (EN) In simulation, all drives healthy
    return true;
}

/// (AR) درجة حرارة القرص / (EN) Drive temperature
int StorageManager::getDriveTemperature(int driveId) const {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return -1;
    }
    return drives_[driveId].temperature;
}

// ============================================================================
// (AR) 4. إدارة الأقسام / (EN) 4. Partition Management
// ============================================================================

/// (AR) قراءة جدول الأقسام — محاكاة قسمين على القرص الأول
/// (EN) Read partition table — simulates 2 partitions on the first drive
int StorageManager::readPartitionTable(int driveId) {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return -1;
    }

    // (AR) توسيع مصفوفة الأقسام إن لزم، ثم تنظيف
    // (EN) Expand partitions array if needed, then clear
    if (driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(partitions_.size(), "nvme_size")) {
        partitions_.resize(driveId + 1);
    }
    partitions_[driveId].clear();

    const DriveInfo& drive = drives_[driveId];

    // (AR) أقراص الذاكرة ليس لها أقسام / (EN) RAM disks have no partitions
    if (drive.type == StorageType::RAMDISK) {
        return 0;
    }

    if (drive.id == 0) {
        // ────────────────────────────────────────────────────────────
        // (AR) القرص الأول (NVMe) — قسمان محاكيان بنظام GPT
        // (EN) First drive (NVMe) — 2 simulated GPT partitions
        // ────────────────────────────────────────────────────────────

        // (AR) القسم 1: قسم نظام EFI / (EN) Partition 1: EFI System Partition
        PartitionInfo efi;
        efi.index     = 0;
        efi.label     = "EFI System";
        efi.typeGUID  = "C12A7328-F81F-11D2-BA4B-00A0C93EC93B";
        efi.startLBA  = 2048;
        efi.endLBA    = 1050623;     // (AR) ~512 ميغابايت / (EN) ~512 MB
        efi.sizeBytes = (efi.endLBA - efi.startLBA + 1) * drive.sectorSize;
        efi.bootable  = true;
        efi.active    = true;
        partitions_[driveId].push_back(efi);

        // (AR) القسم 2: قسم البيانات الأساسي
        // (EN) Partition 2: Basic Data Partition
        PartitionInfo dataPartition;
        dataPartition.index     = 1;
        dataPartition.label     = "Primary Data";
        dataPartition.typeGUID  = "EBD0A0A2-B9E5-4433-87C0-68B6B72699C7";
        dataPartition.startLBA  = 1050624;
        dataPartition.endLBA    = drive.totalSectors - 1;
        dataPartition.sizeBytes = (dataPartition.endLBA - dataPartition.startLBA + 1)
                                  * drive.sectorSize;
        dataPartition.bootable  = false;
        dataPartition.active    = true;
        partitions_[driveId].push_back(dataPartition);

    } else if (drive.id == 1) {
        // ────────────────────────────────────────────────────────────
        // (AR) القرص الثاني (AHCI) — قسم واحد MBR
        // (EN) Second drive (AHCI) — 1 MBR partition
        // ────────────────────────────────────────────────────────────

        PartitionInfo primary;
        primary.index     = 0;
        primary.label     = "NTFS Primary";
        primary.typeGUID  = "07";  // (AR) رمز نوع MBR لـ NTFS / (EN) MBR type code for NTFS
        primary.startLBA  = 2048;
        primary.endLBA    = drive.totalSectors - 1;
        primary.sizeBytes = (primary.endLBA - primary.startLBA + 1)
                            * drive.sectorSize;
        primary.bootable  = true;
        primary.active    = true;
        partitions_[driveId].push_back(primary);
    }

    // (AR) إرجاع عدد الأقسام المقروءة / (EN) Return number of partitions read
    return Sad::Security::SafeArithmetic::assertSafeCast<int>(partitions_[driveId].size(), "nvme_size");
}

/// (AR) عدد الأقسام لقرص معين / (EN) Number of partitions for a given drive
int StorageManager::getPartitionCount(int driveId) const {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size") ||
        driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(partitions_.size(), "nvme_size")) {
        return 0;
    }
    return Sad::Security::SafeArithmetic::assertSafeCast<int>(partitions_[driveId].size(), "nvme_size");
}

/// (AR) معلومات قسم معين / (EN) Specific partition info
PartitionInfo StorageManager::getPartitionInfo(int driveId,
                                               int partIndex) const {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size") ||
        driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(partitions_.size(), "nvme_size") ||
        partIndex < 0 ||
        partIndex >= Sad::Security::SafeArithmetic::assertSafeCast<int>(partitions_[driveId].size(), "nvme_size")) {
        PartitionInfo empty;
        empty.index     = -1;
        empty.label     = "";
        empty.typeGUID  = "";
        empty.startLBA  = 0;
        empty.endLBA    = 0;
        empty.sizeBytes = 0;
        empty.bootable  = false;
        empty.active    = false;
        return empty;
    }
    return partitions_[driveId][partIndex];
}

/// (AR) نظام الأقسام (MBR/GPT) / (EN) Partition scheme (MBR/GPT)
PartitionScheme StorageManager::getPartitionScheme(int driveId) const {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return PartitionScheme::NONE;
    }
    return drives_[driveId].scheme;
}

/// (AR) إجمالي سعة القرص بالميغابايت / (EN) Total drive capacity in MB
uint64_t StorageManager::getDriveCapacityMB(int driveId) const {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return 0;
    }
    return drives_[driveId].capacityBytes / (1024ULL * 1024ULL);
}

// ============================================================================
// (AR) 5. عمليات متقدمة / (EN) 5. Advanced Operations
// ============================================================================

/// (AR) مزامنة ذاكرة التخزين المؤقت — عملية وهمية في المحاكاة
/// (EN) Flush cache — no-op in simulation
int StorageManager::flushCache(int driveId) {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return -1;
    }
    // (AR) في المحاكاة لا يوجد ذاكرة مؤقتة للمزامنة
    // (EN) In simulation there's no cache to flush
    return 0;
}

/// (AR) هل القرص من نوع NVMe؟ / (EN) Is drive NVMe?
bool StorageManager::isNVMe(int driveId) const {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return false;
    }
    return drives_[driveId].type == StorageType::NVME;
}

/// (AR) هل القرص من نوع AHCI؟ / (EN) Is drive AHCI?
bool StorageManager::isAHCI(int driveId) const {
    if (driveId < 0 || driveId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size")) {
        return false;
    }
    return drives_[driveId].type == StorageType::AHCI;
}

/// (AR) إنشاء قرص ذاكرة (RAMDISK) بحجم محدد بالبايت
///      يُخصص مخزن مؤقت في الذاكرة ويسمح بالقراءة والكتابة عليه
/// (EN) Create a RAM disk of specified size in bytes
///      Allocates an in-memory buffer and allows read/write operations on it
int StorageManager::createRamdisk(uint64_t sizeBytes) {
    // (AR) الحد الأدنى: قطاع واحد / (EN) Minimum: one sector
    if (sizeBytes < StorageConstants::SECTOR_SIZE) {
        sizeBytes = StorageConstants::SECTOR_SIZE;
    }

    // (AR) محاذاة الحجم لحدود القطاع / (EN) Align size to sector boundary
    uint32_t sectorSize = StorageConstants::SECTOR_SIZE;
    if (sizeBytes % sectorSize != 0) {
        sizeBytes = ((sizeBytes / sectorSize) + 1) * sectorSize;
    }

    // (AR) تخصيص المخزن المؤقت مملوء بأصفار
    // (EN) Allocate zero-filled buffer
    std::vector<uint8_t> buffer(static_cast<size_t>(sizeBytes), 0);
    ramdisks_.push_back(std::move(buffer));

    // (AR) إنشاء معلومات القرص / (EN) Create drive info entry
    int newId = Sad::Security::SafeArithmetic::assertSafeCast<int>(drives_.size(), "nvme_size");

    DriveInfo ramdisk;
    ramdisk.id            = newId;
    ramdisk.type          = StorageType::RAMDISK;
    ramdisk.state         = DriveState::READY;
    ramdisk.model         = "SAD RAMDISK";
    ramdisk.serial        = "RAMDISK-" + std::to_string(ramdisks_.size());
    ramdisk.firmware      = "1.0";
    ramdisk.sectorSize    = sectorSize;
    ramdisk.totalSectors  = sizeBytes / sectorSize;
    ramdisk.capacityBytes = sizeBytes;
    ramdisk.temperature   = 0;  // (AR) لا حرارة لقرص الذاكرة / (EN) No temp for ramdisk
    ramdisk.scheme        = PartitionScheme::NONE;
    ramdisk.pciSlot       = -1; // (AR) ليس على ناقل PCI / (EN) Not on PCI bus
    drives_.push_back(ramdisk);

    // (AR) توسيع مصفوفة الأقسام / (EN) Expand partitions array
    partitions_.resize(drives_.size());

    // (AR) إرجاع معرّف القرص الجديد / (EN) Return the new drive ID
    return newId;
}

/// (AR) إعادة تعيين الوحدة — مسح جميع الأقراص والأقسام وأقراص الذاكرة
/// (EN) Reset module — clear all drives, partitions, and ramdisk buffers
void StorageManager::reset() {
    drives_.clear();
    partitions_.clear();
    ramdisks_.clear();
    scanned_ = false;
}

} // namespace LowLevel
} // namespace Sad
