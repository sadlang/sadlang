/*
 * ============================================================================
 *       تنفيذ وحدة إقلاع UEFI — لغة ص
 *       UEFI Boot Protocol Module Implementation — Sad Language
 * ============================================================================
 *
 * @brief (AR) التنفيذ الكامل لمدير UEFI:
 *              - محاكاة جدول النظام و Boot/Runtime Services للمفسر
 *              - خريطة ذاكرة واقعية مع واصفات EFI
 *              - محاكاة GOP مع أوضاع متعددة
 *              - نظام ملفات افتراضي في الذاكرة
 *              - متغيرات NVRAM في الذاكرة
 *
 * @brief (EN) Full UEFI Manager implementation:
 *              - System table and Boot/Runtime Services simulation
 *              - Realistic memory map with EFI descriptors
 *              - GOP simulation with multiple modes
 *              - In-memory virtual file system
 *              - In-memory NVRAM variables
 *
 * @version 5.0
 * @date يناير 2026 / January 2026
 * ============================================================================
 */

#include "uefi.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Sad {
namespace LowLevel {

// ============================================================================
//                     بناء المثيل الوحيد / Singleton
// ============================================================================

UefiManager& UefiManager::getInstance() {
    static UefiManager instance;
    return instance;
}

UefiManager::UefiManager()
    : initialized_(false)
    , bootServicesExited_(false)
    , imageHandle_(0)
    , systemTablePtr_(0)
    , firmwareVendor_("SadFirmware")
    , firmwareRevision_(0x00050000)   // v5.0
    , uefiMajorRevision_(2)
    , uefiMinorRevision_(10)          // UEFI 2.10
    , memoryMapKey_(0)
    , nextAllocAddress_(0x100000000ULL) // 4GB فما فوق / 4GB and above
    , nextFd_(0)
{
    gopState_.initialized = false;
    gopState_.currentMode = 0;
    gopState_.maxMode = 0;
    gopState_.framebufferBase = 0;
    gopState_.framebufferSize = 0;
}

// ============================================================================
//                           التهيئة / Initialization
// ============================================================================

EFI_STATUS UefiManager::initialize(EFI_HANDLE imageHandle, uint64_t systemTable) {
    if (initialized_) return EfiStatus::ALREADY_STARTED;

    imageHandle_ = imageHandle;
    systemTablePtr_ = systemTable;

    if (systemTable == 0) {
        // وضع المحاكاة: إنشاء بيئة افتراضية
        // Simulation mode: create a virtual environment
        firmwareVendor_ = "SadFirmware (محاكاة / Simulation)";
        firmwareRevision_ = 0x00050000;

        createSimulationMemoryMap();
        createSimulationGOP();
    }
    // في وضع freestanding، يستقبل المؤشرات الحقيقية من البرنامج الثابت
    // In freestanding mode, real pointers come from firmware

    initialized_ = true;
    bootServicesExited_ = false;
    return EfiStatus::SUCCESS;
}

EFI_STATUS UefiManager::exitBootServices(uint64_t mapKey) {
    if (!initialized_) return EfiStatus::NOT_READY;
    if (bootServicesExited_) return EfiStatus::ALREADY_STARTED;

    // التحقق من مفتاح الخريطة
    // Verify map key matches current memory map
    if (mapKey != memoryMapKey_ && memoryMapKey_ != 0) {
        return EfiStatus::INVALID_PARAMETER;
    }

    bootServicesExited_ = true;
    return EfiStatus::SUCCESS;
}

// ============================================================================
//               خدمات تخصيص الذاكرة / Memory Allocation Services
// ============================================================================

EFI_STATUS UefiManager::allocatePages(EfiAllocateType type, EfiMemoryType memoryType,
                                        uint64_t pages, uint64_t& address) {
    if (!initialized_ || bootServicesExited_) return EfiStatus::NOT_READY;
    if (pages == 0) return EfiStatus::INVALID_PARAMETER;

    uint64_t size = pages * 4096;

    switch (type) {
        case EfiAllocateType::AllocateAnyPages:
            address = nextAllocAddress_;
            nextAllocAddress_ += size;
            break;
        case EfiAllocateType::AllocateMaxAddress:
            // محاكاة: تخصيص من أسفل العنوان المحدد
            address = (address >= size) ? (address - size) : 0;
            break;
        case EfiAllocateType::AllocateAddress:
            // محاكاة: استخدم العنوان المحدد مباشرة
            break;
        default:
            return EfiStatus::INVALID_PARAMETER;
    }

    // إضافة واصف للذاكرة المخصصة
    // Add descriptor for allocated memory
    EfiMemoryDescriptor desc;
    desc.type = static_cast<uint32_t>(memoryType);
    desc.physicalStart = address;
    desc.virtualStart = address;
    desc.numberOfPages = pages;
    desc.attribute = EfiMemAttr::WB;
    memoryMap_.push_back(desc);
    memoryMapKey_++;

    return EfiStatus::SUCCESS;
}

EFI_STATUS UefiManager::freePages(uint64_t address, uint64_t pages) {
    if (!initialized_ || bootServicesExited_) return EfiStatus::NOT_READY;
    (void)pages;

    // إزالة الواصف
    for (auto it = memoryMap_.begin(); it != memoryMap_.end(); ++it) {
        if (it->physicalStart == address) {
            memoryMap_.erase(it);
            memoryMapKey_++;
            return EfiStatus::SUCCESS;
        }
    }
    return EfiStatus::NOT_FOUND;
}

EFI_STATUS UefiManager::allocatePool(EfiMemoryType poolType, uint64_t size, uint64_t& buffer) {
    // Pool allocation = تخصيص صفحات بما يكفي للحجم المطلوب
    uint64_t pages = (size + 4095) / 4096;
    return allocatePages(EfiAllocateType::AllocateAnyPages, poolType, pages, buffer);
}

EFI_STATUS UefiManager::freePool(uint64_t buffer) {
    return freePages(buffer, 0);
}

// ============================================================================
//                     خريطة الذاكرة / Memory Map
// ============================================================================

std::vector<EfiMemoryDescriptor> UefiManager::getMemoryMap(uint64_t& mapKey) {
    mapKey = memoryMapKey_;
    return memoryMap_;
}

uint64_t UefiManager::getTotalAvailableMemory() const {
    uint64_t total = 0;
    for (const auto& desc : memoryMap_) {
        auto type = static_cast<EfiMemoryType>(desc.type);
        if (type == EfiMemoryType::ConventionalMemory ||
            type == EfiMemoryType::BootServicesCode ||
            type == EfiMemoryType::BootServicesData ||
            type == EfiMemoryType::LoaderCode ||
            type == EfiMemoryType::LoaderData) {
            total += desc.numberOfPages * 4096;
        }
    }
    return total;
}

std::string UefiManager::getMemoryTypeName(EfiMemoryType type) {
    switch (type) {
        case EfiMemoryType::ReservedMemoryType:     return "محجوزة / Reserved";
        case EfiMemoryType::LoaderCode:             return "كود المحمّل / Loader Code";
        case EfiMemoryType::LoaderData:             return "بيانات المحمّل / Loader Data";
        case EfiMemoryType::BootServicesCode:       return "كود خدمات الإقلاع / BS Code";
        case EfiMemoryType::BootServicesData:       return "بيانات خدمات الإقلاع / BS Data";
        case EfiMemoryType::RuntimeServicesCode:    return "كود وقت التشغيل / RT Code";
        case EfiMemoryType::RuntimeServicesData:    return "بيانات وقت التشغيل / RT Data";
        case EfiMemoryType::ConventionalMemory:     return "ذاكرة عادية / Conventional";
        case EfiMemoryType::UnusableMemory:         return "غير صالحة / Unusable";
        case EfiMemoryType::ACPIReclaimMemory:      return "ACPI قابلة للاسترداد / ACPI Reclaim";
        case EfiMemoryType::ACPIMemoryNVS:          return "ACPI NVS";
        case EfiMemoryType::MemoryMappedIO:         return "MMIO";
        case EfiMemoryType::MemoryMappedIOPortSpace:return "MMIO Ports";
        case EfiMemoryType::PalCode:                return "PAL Code";
        case EfiMemoryType::PersistentMemory:       return "ذاكرة دائمة / Persistent";
        default:                                     return "غير معروف / Unknown";
    }
}

void UefiManager::createSimulationMemoryMap() {
    memoryMap_.clear();

    // خريطة ذاكرة واقعية لنظام UEFI نموذجي
    // Realistic memory map for a typical UEFI system

    // 0x0000_0000 - 0x0000_0FFF: محجوزة (Real Mode IVT + BDA)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::ReservedMemoryType),
        0x00000000, 0x00000000, 1, EfiMemAttr::WB
    });

    // 0x0000_1000 - 0x0009_FFFF: ذاكرة عادية (640KB)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::ConventionalMemory),
        0x00001000, 0x00001000, 159, EfiMemAttr::WB
    });

    // 0x0010_0000 - 0x001F_FFFF: كود المحمّل (1MB)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::LoaderCode),
        0x00100000, 0x00100000, 256, EfiMemAttr::WB
    });

    // 0x0020_0000 - 0x00FF_FFFF: بيانات خدمات الإقلاع (14MB)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::BootServicesData),
        0x00200000, 0x00200000, 3584, EfiMemAttr::WB
    });

    // 0x0100_0000 - 0x3FFF_FFFF: ذاكرة عادية (1GB - 16MB)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::ConventionalMemory),
        0x01000000, 0x01000000, 258048, EfiMemAttr::WB
    });

    // 0x4000_0000 - 0x4003_FFFF: كود وقت التشغيل (256KB)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::RuntimeServicesCode),
        0x40000000, 0x40000000, 64, EfiMemAttr::WB | EfiMemAttr::RUNTIME
    });

    // 0x4004_0000 - 0x400F_FFFF: بيانات وقت التشغيل (768KB)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::RuntimeServicesData),
        0x40040000, 0x40040000, 192, EfiMemAttr::WB | EfiMemAttr::RUNTIME
    });

    // 0x4010_0000 - 0x7FFF_FFFF: ذاكرة عادية (أغلب الـ 2GB)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::ConventionalMemory),
        0x40100000, 0x40100000, 261888, EfiMemAttr::WB
    });

    // 0xFE00_0000 - 0xFEFF_FFFF: MMIO (APIC, HPET, etc.)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::MemoryMappedIO),
        0xFE000000, 0xFE000000, 4096, EfiMemAttr::UC | EfiMemAttr::RUNTIME
    });

    // 0xFED0_0000 - 0xFED0_FFFF: MMIO (HPET)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::MemoryMappedIO),
        0xFED00000, 0xFED00000, 16, EfiMemAttr::UC | EfiMemAttr::RUNTIME
    });

    // 0xFEE0_0000 - 0xFEE0_FFFF: MMIO (Local APIC)
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::MemoryMappedIO),
        0xFEE00000, 0xFEE00000, 16, EfiMemAttr::UC | EfiMemAttr::RUNTIME
    });

    // ACPI Reclaim
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::ACPIReclaimMemory),
        0xBFFE0000, 0xBFFE0000, 32, EfiMemAttr::WB
    });

    // ACPI NVS
    memoryMap_.push_back({
        static_cast<uint32_t>(EfiMemoryType::ACPIMemoryNVS),
        0xBFFC0000, 0xBFFC0000, 32, EfiMemAttr::WB
    });

    memoryMapKey_ = 1;
}

// ============================================================================
//                بروتوكول الرسوميات (GOP) / Graphics Output Protocol
// ============================================================================

void UefiManager::createSimulationGOP() {
    gopState_.modes.clear();

    // أوضاع رسوميات شائعة / Common graphics modes
    // الوضع 0: 640x480 (VGA)
    gopState_.modes.push_back({
        0, 640, 480, UefiPixelFormat::BGRR_8BIT,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 640
    });

    // الوضع 1: 800x600 (SVGA)
    gopState_.modes.push_back({
        0, 800, 600, UefiPixelFormat::BGRR_8BIT,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 800
    });

    // الوضع 2: 1024x768 (XGA)
    gopState_.modes.push_back({
        0, 1024, 768, UefiPixelFormat::BGRR_8BIT,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 1024
    });

    // الوضع 3: 1280x720 (HD)
    gopState_.modes.push_back({
        0, 1280, 720, UefiPixelFormat::BGRR_8BIT,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 1280
    });

    // الوضع 4: 1920x1080 (Full HD)
    gopState_.modes.push_back({
        0, 1920, 1080, UefiPixelFormat::BGRR_8BIT,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 1920
    });

    gopState_.maxMode = static_cast<uint32_t>(gopState_.modes.size());
    gopState_.currentMode = 0;
    gopState_.modeInfo = gopState_.modes[0];

    // حساب عنوان وحجم المخزن المؤقت (محاكاة)
    // Calculate framebuffer base and size (simulation)
    gopState_.framebufferBase = 0xFD000000ULL;  // عنوان PCI MMIO نموذجي
    gopState_.framebufferSize = 1920 * 1080 * 4; // أكبر وضع * 4 بايت/بكسل
    gopState_.initialized = true;
}

EFI_STATUS UefiManager::initializeGOP() {
    if (!initialized_) return EfiStatus::NOT_READY;
    if (gopState_.initialized) return EfiStatus::SUCCESS;

    createSimulationGOP();
    return EfiStatus::SUCCESS;
}

EFI_STATUS UefiManager::setGOPMode(uint32_t modeNumber) {
    if (!gopState_.initialized) return EfiStatus::NOT_READY;
    if (modeNumber >= gopState_.maxMode) return EfiStatus::INVALID_PARAMETER;

    gopState_.currentMode = modeNumber;
    gopState_.modeInfo = gopState_.modes[modeNumber];

    // تحديث حجم المخزن
    uint64_t w = gopState_.modeInfo.horizontalResolution;
    uint64_t h = gopState_.modeInfo.verticalResolution;
    gopState_.framebufferSize = w * h * 4;

    return EfiStatus::SUCCESS;
}

EFI_STATUS UefiManager::queryGOPMode(uint32_t modeNumber, UefiGopModeInfo& info) {
    if (!gopState_.initialized) return EfiStatus::NOT_READY;
    if (modeNumber >= gopState_.maxMode) return EfiStatus::INVALID_PARAMETER;

    info = gopState_.modes[modeNumber];
    return EfiStatus::SUCCESS;
}

EFI_STATUS UefiManager::gopBlt(UefiGopBltPixel* buffer, UefiGopBltOperation operation,
                                 uint32_t srcX, uint32_t srcY,
                                 uint32_t dstX, uint32_t dstY,
                                 uint32_t width, uint32_t height) {
    if (!gopState_.initialized) return EfiStatus::NOT_READY;

    // في المحاكاة، العمليات تُسجل فقط
    // In simulation, operations are logged only
    (void)buffer; (void)operation;
    (void)srcX; (void)srcY; (void)dstX; (void)dstY;
    (void)width; (void)height;

    return EfiStatus::SUCCESS;
}

uint32_t UefiManager::getGOPModeCount() const {
    return gopState_.maxMode;
}

uint32_t UefiManager::getCurrentGOPMode() const {
    return gopState_.currentMode;
}

uint64_t UefiManager::getFramebufferBase() const {
    return gopState_.framebufferBase;
}

uint64_t UefiManager::getFramebufferSize() const {
    return gopState_.framebufferSize;
}

// ============================================================================
//            خدمات وقت التشغيل / Runtime Services
// ============================================================================

EfiTime UefiManager::getTime() {
    EfiTime time = {};

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    struct tm tm_now;

#ifdef _WIN32
    localtime_s(&tm_now, &time_t_now);
#else
    localtime_r(&time_t_now, &tm_now);
#endif

    time.year   = static_cast<uint16_t>(tm_now.tm_year + 1900);
    time.month  = static_cast<uint8_t>(tm_now.tm_mon + 1);
    time.day    = static_cast<uint8_t>(tm_now.tm_mday);
    time.hour   = static_cast<uint8_t>(tm_now.tm_hour);
    time.minute = static_cast<uint8_t>(tm_now.tm_min);
    time.second = static_cast<uint8_t>(tm_now.tm_sec);
    time.nanosecond = 0;
    time.timezone = 0; // UTC
    time.daylight = 0;

    return time;
}

EFI_STATUS UefiManager::setTime(const EfiTime& time) {
    // في المحاكاة: لا يمكن تعيين وقت النظام فعلياً
    // In simulation: cannot actually set system time
    (void)time;
    return EfiStatus::SUCCESS;
}

EFI_STATUS UefiManager::getVariable(const std::string& name, const EFI_GUID& vendorGuid,
                                      std::vector<uint8_t>& data) {
    for (const auto& var : nvramVars_) {
        if (var.name == name &&
            var.guid.data1 == vendorGuid.data1 &&
            var.guid.data2 == vendorGuid.data2) {
            data = var.data;
            return EfiStatus::SUCCESS;
        }
    }
    return EfiStatus::NOT_FOUND;
}

EFI_STATUS UefiManager::setVariable(const std::string& name, const EFI_GUID& vendorGuid,
                                      uint32_t attributes, const std::vector<uint8_t>& data) {
    // البحث عن متغير موجود / Search for existing variable
    for (auto& var : nvramVars_) {
        if (var.name == name &&
            var.guid.data1 == vendorGuid.data1 &&
            var.guid.data2 == vendorGuid.data2) {
            var.data = data;
            var.attributes = attributes;
            return EfiStatus::SUCCESS;
        }
    }

    // إنشاء متغير جديد / Create new variable
    nvramVars_.push_back({name, vendorGuid, attributes, data});
    return EfiStatus::SUCCESS;
}

void UefiManager::resetSystem(int resetType) {
    // في المحاكاة: طباعة رسالة فقط
    // In simulation: just print message
    (void)resetType;
    // في الوضع الأصلي، هذا يستدعي EFI_RUNTIME_SERVICES->ResetSystem()
    // In native mode, this would call EFI_RUNTIME_SERVICES->ResetSystem()
}

// ============================================================================
//                   نظام الملفات / File System
// ============================================================================

int64_t UefiManager::allocateFileDescriptor() {
    // البحث عن واصف حر / Find free descriptor
    for (size_t i = 0; i < openFiles_.size(); i++) {
        if (!openFiles_[i].open) {
            return static_cast<int64_t>(i);
        }
    }
    // إنشاء واصف جديد / Create new descriptor
    openFiles_.push_back({false, "", 0, 0, {}, false});
    return static_cast<int64_t>(openFiles_.size() - 1);
}

int64_t UefiManager::openVolume() {
    if (!initialized_) return -1;

    int64_t fd = allocateFileDescriptor();
    if (fd < 0) return -1;

    openFiles_[fd].open = true;
    openFiles_[fd].name = "/";
    openFiles_[fd].offset = 0;
    openFiles_[fd].size = 0;
    openFiles_[fd].isDirectory = true;
    openFiles_[fd].data.clear();

    return fd;
}

int64_t UefiManager::openFile(int64_t parentFd, const std::string& filename,
                                uint64_t mode, uint64_t attributes) {
    if (!initialized_) return -1;
    if (parentFd < 0 || parentFd >= static_cast<int64_t>(openFiles_.size())) return -1;
    if (!openFiles_[parentFd].open) return -1;

    (void)attributes;

    int64_t fd = allocateFileDescriptor();
    if (fd < 0) return -1;

    openFiles_[fd].open = true;
    openFiles_[fd].name = filename;
    openFiles_[fd].offset = 0;
    openFiles_[fd].size = 0;
    openFiles_[fd].isDirectory = false;
    openFiles_[fd].data.clear();

    if (mode & EfiFileMode::CREATE) {
        // ملف جديد / New file
        openFiles_[fd].size = 0;
    }

    return fd;
}

int64_t UefiManager::readFile(int64_t fd, std::vector<uint8_t>& buffer, uint64_t size) {
    if (fd < 0 || fd >= static_cast<int64_t>(openFiles_.size())) return -1;
    if (!openFiles_[fd].open) return -1;

    uint64_t available = openFiles_[fd].data.size() - openFiles_[fd].offset;
    uint64_t toRead = std::min(size, available);

    buffer.resize(toRead);
    if (toRead > 0) {
        std::copy(openFiles_[fd].data.begin() + openFiles_[fd].offset,
                  openFiles_[fd].data.begin() + openFiles_[fd].offset + toRead,
                  buffer.begin());
        openFiles_[fd].offset += toRead;
    }

    return static_cast<int64_t>(toRead);
}

int64_t UefiManager::writeFile(int64_t fd, const std::vector<uint8_t>& data) {
    if (fd < 0 || fd >= static_cast<int64_t>(openFiles_.size())) return -1;
    if (!openFiles_[fd].open) return -1;

    // إلحاق البيانات / Append data
    openFiles_[fd].data.insert(openFiles_[fd].data.end(), data.begin(), data.end());
    openFiles_[fd].size = openFiles_[fd].data.size();

    return static_cast<int64_t>(data.size());
}

EFI_STATUS UefiManager::closeFile(int64_t fd) {
    if (fd < 0 || fd >= static_cast<int64_t>(openFiles_.size())) return EfiStatus::INVALID_PARAMETER;
    if (!openFiles_[fd].open) return EfiStatus::NOT_FOUND;

    openFiles_[fd].open = false;
    return EfiStatus::SUCCESS;
}

UefiFileInfo UefiManager::getFileInfo(int64_t fd) {
    UefiFileInfo info = {};
    if (fd < 0 || fd >= static_cast<int64_t>(openFiles_.size())) return info;
    if (!openFiles_[fd].open) return info;

    info.fileSize = openFiles_[fd].size;
    info.physicalSize = (openFiles_[fd].size + 4095) & ~4095ULL;
    info.fileName = openFiles_[fd].name;

    if (openFiles_[fd].isDirectory) {
        info.attribute = EfiFileAttr::DIRECTORY;
    } else {
        info.attribute = EfiFileAttr::ARCHIVE;
    }

    return info;
}

// ============================================================================
//                     بروتوكولات / Protocol Services
// ============================================================================

uint64_t UefiManager::locateProtocol(const EFI_GUID& guid) {
    if (!initialized_ || bootServicesExited_) return 0;

    // التحقق من البروتوكولات المعروفة
    // Check known protocols
    if (guid.data1 == EfiProtocol::GOP.data1 && 
        guid.data2 == EfiProtocol::GOP.data2) {
        return gopState_.initialized ? 0xDEAD0001 : 0;
    }

    if (guid.data1 == EfiProtocol::SIMPLE_FS.data1 &&
        guid.data2 == EfiProtocol::SIMPLE_FS.data2) {
        return 0xDEAD0002;
    }

    return 0;
}

// ============================================================================
//                 معلومات النظام / System Information
// ============================================================================

std::string UefiManager::getUefiRevision() const {
    return std::to_string(uefiMajorRevision_) + "." + std::to_string(uefiMinorRevision_);
}

std::string UefiManager::getFirmwareVendor() const {
    return firmwareVendor_;
}

uint32_t UefiManager::getFirmwareRevision() const {
    return firmwareRevision_;
}

// ============================================================================
//                         التقارير / Reports
// ============================================================================

std::string UefiManager::generateReport() const {
    std::ostringstream report;

    report << "╔══════════════════════════════════════════════════════════════╗\n";
    report << "║          تقرير نظام UEFI / UEFI System Report              ║\n";
    report << "╚══════════════════════════════════════════════════════════════╝\n\n";

    if (!initialized_) {
        report << "  الحالة: غير مهيأ / Status: Not initialized\n";
        return report.str();
    }

    // معلومات النظام / System Information
    report << "═══ معلومات النظام / System Info ═══\n";
    report << "  إصدار UEFI / UEFI Revision: " << uefiMajorRevision_ << "." << uefiMinorRevision_ << "\n";
    report << "  البائع / Firmware Vendor:    " << firmwareVendor_ << "\n";
    report << "  إصدار البرنامج الثابت:       0x" << std::hex << std::setfill('0')
           << std::setw(8) << firmwareRevision_ << std::dec << "\n";
    report << "  مقبض الصورة / Image Handle:  0x" << std::hex << imageHandle_ << std::dec << "\n";
    report << "  خدمات الإقلاع / BS Status:   "
           << (bootServicesExited_ ? "منتهية / Exited" : "نشطة / Active") << "\n\n";

    // خريطة الذاكرة / Memory Map
    report << generateMemoryMapReport();

    // GOP
    if (gopState_.initialized) {
        report << "\n" << generateGOPReport();
    }

    // نظام الملفات / File System
    int openCount = 0;
    for (const auto& f : openFiles_) if (f.open) openCount++;
    report << "\n═══ نظام الملفات / File System ═══\n";
    report << "  ملفات مفتوحة / Open Files: " << openCount << "\n";

    // NVRAM
    report << "\n═══ متغيرات NVRAM / NVRAM Variables ═══\n";
    report << "  عدد المتغيرات / Variable Count: " << nvramVars_.size() << "\n";
    for (const auto& var : nvramVars_) {
        report << "    " << var.name << " (" << var.data.size() << " بايت / bytes)\n";
    }

    return report.str();
}

std::string UefiManager::generateMemoryMapReport() const {
    std::ostringstream report;

    report << "═══ خريطة الذاكرة / Memory Map ═══\n";
    report << "  عدد المناطق / Regions: " << memoryMap_.size() << "\n";
    report << "  مفتاح الخريطة / Map Key: " << memoryMapKey_ << "\n\n";

    report << "  العنوان الفيزيائي    | الصفحات  | النوع\n";
    report << "  ─────────────────────┼──────────┼────────────────────────────\n";

    for (const auto& desc : memoryMap_) {
        report << "  0x" << std::hex << std::setfill('0') << std::setw(16)
               << desc.physicalStart << std::dec
               << " | " << std::setw(8) << std::setfill(' ') << desc.numberOfPages
               << " | " << getMemoryTypeName(static_cast<EfiMemoryType>(desc.type))
               << "\n";
    }

    report << "\n  إجمالي الذاكرة المتاحة / Total Available: "
           << (getTotalAvailableMemory() / (1024 * 1024)) << " MB\n";

    return report.str();
}

std::string UefiManager::generateGOPReport() const {
    std::ostringstream report;

    report << "═══ بروتوكول الرسوميات (GOP) / Graphics Output Protocol ═══\n";

    if (!gopState_.initialized) {
        report << "  الحالة: غير مهيأ / Not initialized\n";
        return report.str();
    }

    report << "  الحالة / Status: مهيأ / Initialized\n";
    report << "  الوضع الحالي / Current Mode: " << gopState_.currentMode << "\n";
    report << "  عدد الأوضاع / Total Modes:   " << gopState_.maxMode << "\n";
    report << "  عنوان المخزن / FB Base:       0x" << std::hex << gopState_.framebufferBase
           << std::dec << "\n";
    report << "  حجم المخزن / FB Size:         " << (gopState_.framebufferSize / 1024) << " KB\n\n";

    report << "  الأوضاع المتاحة / Available Modes:\n";
    report << "  رقم | الدقة          | التنسيق       | بكسل/سطر\n";
    report << "  ────┼────────────────┼───────────────┼──────────\n";

    for (uint32_t i = 0; i < gopState_.maxMode; i++) {
        const auto& mode = gopState_.modes[i];
        std::string format;
        switch (mode.pixelFormat) {
            case UefiPixelFormat::RGBR_8BIT: format = "RGB 8bit"; break;
            case UefiPixelFormat::BGRR_8BIT: format = "BGR 8bit"; break;
            case UefiPixelFormat::BIT_MASK:  format = "BitMask";  break;
            case UefiPixelFormat::BLT_ONLY:  format = "BLT Only"; break;
        }

        report << "  " << std::setw(3) << i
               << " | " << std::setw(4) << mode.horizontalResolution
               << "x" << std::setw(4) << std::left << mode.verticalResolution << std::right
               << "      | " << std::setw(13) << format
               << " | " << mode.pixelsPerScanLine << "\n";
    }

    return report.str();
}

} // namespace LowLevel
} // namespace Sad
