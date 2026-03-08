/*
 * ============================================================================
 *       وحدة إقلاع UEFI — لغة ص
 *       UEFI Boot Protocol Module — Sad Language
 * ============================================================================
 *
 * @brief (AR) دعم كامل لبروتوكول إقلاع UEFI:
 *              - جدول النظام (System Table) مع جميع خدمات الإقلاع والتشغيل
 *              - خدمات الإقلاع (Boot Services): تخصيص ذاكرة، بروتوكولات، أحداث
 *              - خدمات وقت التشغيل (Runtime Services): وقت، متغيرات NVRAM
 *              - بروتوكول الرسوميات (GOP): فيديو UEFI الأصلي
 *              - بروتوكول نظام الملفات البسيط (Simple File System Protocol)
 *              - تحليل خريطة الذاكرة وإنهاء خدمات الإقلاع (ExitBootServices)
 *              
 *              يعمل في وضعين:
 *              1. محاكاة (simulation) — للمفسر على نظام تشغيل عادي
 *              2. أصلي (native) — للمترجم sadc في وضع --freestanding
 *
 * @brief (EN) Complete UEFI boot protocol support:
 *              - System Table with full Boot/Runtime Services
 *              - Boot Services: memory allocation, protocols, events
 *              - Runtime Services: time, NVRAM variables
 *              - Graphics Output Protocol (GOP): native UEFI video
 *              - Simple File System Protocol
 *              - Memory map parsing and ExitBootServices
 *
 * @version 5.0
 * @date يناير 2026 / January 2026
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_UEFI_H
#define SAD_LOW_LEVEL_UEFI_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <functional>

namespace Sad {
namespace LowLevel {

// ============================================================================
// أنواع UEFI الأساسية / UEFI Base Types
// ============================================================================

/// @brief (AR) حالة UEFI / (EN) UEFI Status code
///   القيم الموجبة = نجاح، السالبة = خطأ
///   Positive = success, negative = error
using EFI_STATUS  = int64_t;
using EFI_HANDLE  = uint64_t;
struct EFI_GUID   { uint32_t data1; uint16_t data2; uint16_t data3; uint8_t data4[8]; };

/// @brief (AR) رموز حالة UEFI / (EN) UEFI Status codes
namespace EfiStatus {
    static constexpr EFI_STATUS SUCCESS             = 0;
    static constexpr EFI_STATUS LOAD_ERROR           = 1;
    static constexpr EFI_STATUS INVALID_PARAMETER    = 2;
    static constexpr EFI_STATUS UNSUPPORTED          = 3;
    static constexpr EFI_STATUS BAD_BUFFER_SIZE      = 4;
    static constexpr EFI_STATUS BUFFER_TOO_SMALL     = 5;
    static constexpr EFI_STATUS NOT_READY            = 6;
    static constexpr EFI_STATUS DEVICE_ERROR         = 7;
    static constexpr EFI_STATUS WRITE_PROTECTED      = 8;
    static constexpr EFI_STATUS OUT_OF_RESOURCES     = 9;
    static constexpr EFI_STATUS NOT_FOUND            = 14;
    static constexpr EFI_STATUS ACCESS_DENIED        = 15;
    static constexpr EFI_STATUS ALREADY_STARTED      = 20;
}

// ============================================================================
// أنواع تخصيص الذاكرة / Memory Allocation Types
// ============================================================================

/// @brief (AR) نوع تخصيص الذاكرة / (EN) EFI Memory allocation type
enum class EfiAllocateType : uint32_t {
    AllocateAnyPages   = 0,   // أي صفحة متاحة / Any available page
    AllocateMaxAddress = 1,   // تحت عنوان معين / Below specific address
    AllocateAddress    = 2    // عنوان محدد / Specific address
};

/// @brief (AR) نوع ذاكرة UEFI / (EN) EFI Memory type
enum class EfiMemoryType : uint32_t {
    ReservedMemoryType     = 0,   // محجوزة
    LoaderCode             = 1,   // كود المحمّل
    LoaderData             = 2,   // بيانات المحمّل
    BootServicesCode       = 3,   // كود خدمات الإقلاع
    BootServicesData       = 4,   // بيانات خدمات الإقلاع
    RuntimeServicesCode    = 5,   // كود خدمات التشغيل
    RuntimeServicesData    = 6,   // بيانات خدمات التشغيل
    ConventionalMemory     = 7,   // ذاكرة عادية (متاحة)
    UnusableMemory         = 8,   // غير صالحة
    ACPIReclaimMemory      = 9,   // ACPI قابلة للاسترداد
    ACPIMemoryNVS          = 10,  // ACPI NVS
    MemoryMappedIO         = 11,  // MMIO
    MemoryMappedIOPortSpace = 12, // MMIO Ports
    PalCode                = 13,  // PAL
    PersistentMemory       = 14,  // ذاكرة دائمة
    MaxMemoryType          = 15
};

/// @brief (AR) واصف خريطة ذاكرة UEFI / (EN) EFI Memory Descriptor
#pragma pack(push, 1)
struct EfiMemoryDescriptor {
    uint32_t type;                  // EfiMemoryType
    uint64_t physicalStart;         // عنوان فيزيائي / Physical address
    uint64_t virtualStart;          // عنوان افتراضي / Virtual address
    uint64_t numberOfPages;         // عدد الصفحات (4KB لكل صفحة) / Number of 4KB pages
    uint64_t attribute;             // سمات الذاكرة / Memory attributes
};
#pragma pack(pop)

/// @brief (AR) سمات الذاكرة / (EN) Memory attributes
namespace EfiMemAttr {
    static constexpr uint64_t UC   = 0x0000000000000001ULL;  // غير مخبأة / Uncacheable
    static constexpr uint64_t WC   = 0x0000000000000002ULL;  // Write Combining
    static constexpr uint64_t WT   = 0x0000000000000004ULL;  // Write Through
    static constexpr uint64_t WB   = 0x0000000000000008ULL;  // Write Back
    static constexpr uint64_t UCE  = 0x0000000000000010ULL;  // UC exported
    static constexpr uint64_t WP   = 0x0000000000001000ULL;  // محمية من الكتابة / Write Protected
    static constexpr uint64_t RP   = 0x0000000000002000ULL;  // محمية من القراءة / Read Protected
    static constexpr uint64_t XP   = 0x0000000000004000ULL;  // محمية من التنفيذ / eXecute Protected
    static constexpr uint64_t NV   = 0x0000000000008000ULL;  // غير متطايرة / Non-Volatile
    static constexpr uint64_t RUNTIME = 0x8000000000000000ULL; // وقت التشغيل / Runtime
}

// ============================================================================
// الوقت والتاريخ / Time and Date
// ============================================================================

/// @brief (AR) هيكل الوقت في UEFI / (EN) UEFI Time structure
struct EfiTime {
    uint16_t year;       // السنة 1900-9999 / Year 1900-9999
    uint8_t  month;      // الشهر 1-12 / Month 1-12
    uint8_t  day;        // اليوم 1-31 / Day 1-31
    uint8_t  hour;       // الساعة 0-23 / Hour 0-23
    uint8_t  minute;     // الدقيقة 0-59 / Minute 0-59
    uint8_t  second;     // الثانية 0-59 / Second 0-59
    uint8_t  pad1;
    uint32_t nanosecond; // نانوثانية 0-999999999 / Nanosecond
    int16_t  timezone;   // المنطقة الزمنية / Timezone (-1440 to 1440)
    uint8_t  daylight;   // التوقيت الصيفي / Daylight savings
    uint8_t  pad2;
};

// ============================================================================
// بروتوكولات UEFI المعروفة / Known UEFI Protocol GUIDs
// ============================================================================

namespace EfiProtocol {
    /// @brief (AR) معرّف بروتوكول الرسوميات / (EN) GOP GUID
    ///   {9042A9DE-23DC-4A38-96FB-7ADED080516A}
    static constexpr EFI_GUID GOP = {
        0x9042A9DE, 0x23DC, 0x4A38,
        {0x96, 0xFB, 0x7A, 0xDE, 0xD0, 0x80, 0x51, 0x6A}
    };

    /// @brief (AR) معرّف نظام الملفات البسيط / (EN) Simple File System GUID
    ///   {964E5B22-6459-11D2-8E39-00A0C969723B}
    static constexpr EFI_GUID SIMPLE_FS = {
        0x964E5B22, 0x6459, 0x11D2,
        {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}
    };

    /// @brief (AR) معرّف بروتوكول المعلومات المحملة / (EN) Loaded Image Protocol GUID
    ///   {5B1B31A1-9562-11D2-8E3F-00A0C969723B}
    static constexpr EFI_GUID LOADED_IMAGE = {
        0x5B1B31A1, 0x9562, 0x11D2,
        {0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}
    };

    /// @brief (AR) معرّف بروتوكول مسار الجهاز / (EN) Device Path Protocol GUID
    static constexpr EFI_GUID DEVICE_PATH = {
        0x09576E91, 0x6D3F, 0x11D2,
        {0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B}
    };
}

// ============================================================================
// بروتوكول الرسوميات (GOP) / Graphics Output Protocol
// ============================================================================

/// @brief (AR) تنسيق بكسل GOP / (EN) GOP Pixel format
enum class UefiPixelFormat : uint32_t {
    RGBR_8BIT = 0,    // أحمر-أخضر-أزرق (8 بت لكل لون) / RGB 8bpp
    BGRR_8BIT = 1,    // أزرق-أخضر-أحمر (8 بت لكل لون) / BGR 8bpp  
    BIT_MASK  = 2,    // قناع بت مخصص / Custom bit mask
    BLT_ONLY  = 3     // نقل كتل فقط / Block transfer only
};

/// @brief (AR) معلومات وضع GOP / (EN) GOP Mode information
struct UefiGopModeInfo {
    uint32_t version;                   // إصدار المعلومات / Info version
    uint32_t horizontalResolution;      // العرض بالبكسل / Width in pixels
    uint32_t verticalResolution;        // الارتفاع بالبكسل / Height in pixels
    UefiPixelFormat pixelFormat;        // تنسيق البكسل / Pixel format
    uint32_t redMask;                   // قناع الأحمر / Red mask
    uint32_t greenMask;                 // قناع الأخضر / Green mask
    uint32_t blueMask;                  // قناع الأزرق / Blue mask
    uint32_t reservedMask;              // قناع محجوز / Reserved mask
    uint32_t pixelsPerScanLine;         // بكسل لكل سطر مسح / Pixels per scan line
};

/// @brief (AR) بكسل BLT (نقل كتل) / (EN) BLT Pixel
struct UefiGopBltPixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
};

/// @brief (AR) عمليات BLT / (EN) BLT operations
enum class UefiGopBltOperation : uint32_t {
    VideoFill     = 0,    // ملء منطقة بلون / Fill region with color
    VideoToBuffer = 1,    // شاشة إلى ذاكرة / Screen to buffer
    BufferToVideo = 2,    // ذاكرة إلى شاشة / Buffer to screen
    VideoToVideo  = 3     // شاشة إلى شاشة / Screen to screen
};

/// @brief (AR) حالة بروتوكول GOP / (EN) GOP Protocol state
struct UefiGopState {
    bool     initialized;               // مهيأ / Initialized
    uint32_t currentMode;               // الوضع الحالي / Current mode
    uint32_t maxMode;                   // عدد الأوضاع / Number of modes
    uint64_t framebufferBase;           // عنوان المخزن المؤقت / Framebuffer base address
    uint64_t framebufferSize;           // حجم المخزن / Framebuffer size
    UefiGopModeInfo modeInfo;           // معلومات الوضع الحالي / Current mode info
    std::vector<UefiGopModeInfo> modes; // جميع الأوضاع المتاحة / All available modes
};

// ============================================================================
// نظام الملفات / File System
// ============================================================================

/// @brief (AR) سمات الملف / (EN) File attributes
namespace EfiFileAttr {
    static constexpr uint64_t READ_ONLY  = 0x0000000000000001ULL;
    static constexpr uint64_t HIDDEN     = 0x0000000000000002ULL;
    static constexpr uint64_t SYSTEM     = 0x0000000000000004ULL;
    static constexpr uint64_t DIRECTORY  = 0x0000000000000010ULL;
    static constexpr uint64_t ARCHIVE    = 0x0000000000000020ULL;
}

/// @brief (AR) أوضاع فتح الملف / (EN) File open modes
namespace EfiFileMode {
    static constexpr uint64_t READ   = 0x0000000000000001ULL;
    static constexpr uint64_t WRITE  = 0x0000000000000002ULL;
    static constexpr uint64_t CREATE = 0x8000000000000000ULL;
}

/// @brief (AR) معلومات ملف UEFI / (EN) UEFI File info
struct UefiFileInfo {
    uint64_t    size;           // حجم الهيكل / Structure size
    uint64_t    fileSize;       // حجم الملف / File size
    uint64_t    physicalSize;   // الحجم الفيزيائي / Physical size
    EfiTime     createTime;     // وقت الإنشاء / Creation time
    EfiTime     lastAccessTime; // آخر وصول / Last access time
    EfiTime     modificationTime; // آخر تعديل / Last modification time
    uint64_t    attribute;      // السمات / Attributes
    std::string fileName;       // اسم الملف / File name
};

// ============================================================================
// متغيرات NVRAM / NVRAM Variables
// ============================================================================

/// @brief (AR) سمات متغير NVRAM / (EN) NVRAM Variable attributes
namespace EfiVarAttr {
    static constexpr uint32_t NON_VOLATILE                       = 0x00000001;
    static constexpr uint32_t BOOTSERVICE_ACCESS                 = 0x00000002;
    static constexpr uint32_t RUNTIME_ACCESS                     = 0x00000004;
    static constexpr uint32_t HARDWARE_ERROR_RECORD              = 0x00000008;
    static constexpr uint32_t AUTHENTICATED_WRITE_ACCESS         = 0x00000010;
    static constexpr uint32_t TIME_BASED_AUTHENTICATED_WRITE_ACCESS = 0x00000020;
    static constexpr uint32_t APPEND_WRITE                       = 0x00000040;
}

// ============================================================================
// مدير UEFI / UEFI Manager
// ============================================================================

/**
 * @brief (AR) مدير بروتوكول UEFI الشامل
 *        يوفر واجهة كاملة لجميع خدمات UEFI:
 *        - تهيئة ومحاكاة جدول النظام
 *        - خدمات الإقلاع (تخصيص ذاكرة، بروتوكولات، أحداث)
 *        - خدمات وقت التشغيل (وقت، متغيرات NVRAM، إعادة تشغيل)
 *        - بروتوكول الرسوميات GOP (الفيديو)
 *        - بروتوكول نظام الملفات
 *        - تحليل خريطة الذاكرة وإنهاء خدمات الإقلاع
 *
 * @brief (EN) Comprehensive UEFI Protocol Manager
 *        Provides a full interface to all UEFI services:
 *        - System table initialization and simulation
 *        - Boot Services (memory allocation, protocols, events)
 *        - Runtime Services (time, NVRAM variables, reset)
 *        - GOP (Graphics Output Protocol) for video
 *        - Simple File System Protocol
 *        - Memory map parsing and ExitBootServices
 */
class UefiManager {
public:
    /// @brief (AR) الحصول على المثيل الوحيد / (EN) Get singleton instance
    static UefiManager& getInstance();

    // ========================================================================
    //                    التهيئة / Initialization
    // ========================================================================

    /**
     * @brief (AR) تهيئة بيئة UEFI
     *        في وضع المحاكاة: ينشئ جدول نظام افتراضي
     *        في وضع freestanding: يستقبل المؤشرات من البرنامج الثابت
     *
     * @brief (EN) Initialize UEFI environment
     *        Simulation mode: creates a default system table
     *        Freestanding mode: receives pointers from firmware
     *
     * @param imageHandle (AR) مقبض الصورة / (EN) Image handle (0 for simulation)
     * @param systemTable (AR) مؤشر جدول النظام / (EN) System table pointer (0 for simulation)
     * @return (AR) حالة النجاح / (EN) Success status
     */
    EFI_STATUS initialize(EFI_HANDLE imageHandle = 0, uint64_t systemTable = 0);

    /**
     * @brief (AR) إنهاء خدمات الإقلاع — نقطة اللاعودة!
     *        بعد استدعاء هذه الدالة، لا يمكن استخدام خدمات الإقلاع مجدداً.
     *        يجب استدعاؤها قبل تحويل النواة إلى التحكم الكامل.
     *
     * @brief (EN) Exit Boot Services — point of no return!
     *        After calling this, Boot Services become unavailable.
     *        Must be called before kernel takes full control.
     *
     * @param mapKey (AR) مفتاح خريطة الذاكرة / (EN) Memory map key
     * @return (AR) حالة UEFI / (EN) EFI_STATUS
     */
    EFI_STATUS exitBootServices(uint64_t mapKey);

    /// @brief (AR) هل تم تهيئة UEFI / (EN) Is UEFI initialized
    bool isInitialized() const { return initialized_; }

    /// @brief (AR) هل تم إنهاء خدمات الإقلاع / (EN) Are boot services exited
    bool bootServicesExited() const { return bootServicesExited_; }

    // ========================================================================
    //         خدمات تخصيص الذاكرة / Memory Allocation Services
    // ========================================================================

    /**
     * @brief (AR) تخصيص صفحات ذاكرة
     *        كل صفحة = 4096 بايت
     *
     * @brief (EN) Allocate memory pages
     *        Each page = 4096 bytes
     *
     * @param type نوع التخصيص / Allocation type
     * @param memoryType نوع الذاكرة / Memory type
     * @param pages عدد الصفحات / Number of pages
     * @param address عنوان الذاكرة المخصصة (إخراج) / Allocated address (output)
     * @return حالة UEFI / EFI_STATUS
     */
    EFI_STATUS allocatePages(EfiAllocateType type, EfiMemoryType memoryType,
                              uint64_t pages, uint64_t& address);

    /**
     * @brief (AR) تحرير صفحات ذاكرة / (EN) Free memory pages
     * @param address العنوان / Address
     * @param pages عدد الصفحات / Number of pages
     */
    EFI_STATUS freePages(uint64_t address, uint64_t pages);

    /**
     * @brief (AR) تخصيص كتلة ذاكرة / (EN) Allocate pool memory
     *        مشابه لـ malloc ولكن مع نوع ذاكرة محدد
     *
     * @param poolType نوع الذاكرة / Memory type
     * @param size الحجم بالبايت / Size in bytes
     * @param buffer مؤشر الذاكرة المخصصة (إخراج) / Allocated buffer (output)
     */
    EFI_STATUS allocatePool(EfiMemoryType poolType, uint64_t size, uint64_t& buffer);

    /**
     * @brief (AR) تحرير كتلة ذاكرة / (EN) Free pool memory
     * @param buffer المؤشر / Buffer pointer
     */
    EFI_STATUS freePool(uint64_t buffer);

    // ========================================================================
    //             خريطة الذاكرة / Memory Map
    // ========================================================================

    /**
     * @brief (AR) الحصول على خريطة ذاكرة UEFI
     *        تُرجع قائمة واصفات الذاكرة مع مفتاح الخريطة
     *        (المفتاح مطلوب لـ ExitBootServices)
     *
     * @brief (EN) Get UEFI memory map
     *        Returns list of memory descriptors with map key
     *        (key is required for ExitBootServices)
     *
     * @param mapKey مفتاح الخريطة (إخراج) / Map key (output)
     * @return قائمة واصفات الذاكرة / List of memory descriptors
     */
    std::vector<EfiMemoryDescriptor> getMemoryMap(uint64_t& mapKey);

    /**
     * @brief (AR) إجمالي الذاكرة المتاحة / (EN) Total available memory
     * @return الحجم بالبايت / Size in bytes
     */
    uint64_t getTotalAvailableMemory() const;

    /**
     * @brief (AR) الحصول على نوع منطقة الذاكرة كنص عربي
     * @brief (EN) Get memory type name in Arabic
     */
    static std::string getMemoryTypeName(EfiMemoryType type);

    // ========================================================================
    //          بروتوكول الرسوميات (GOP) / Graphics Output Protocol
    // ========================================================================

    /**
     * @brief (AR) تهيئة بروتوكول الرسوميات GOP
     *        يبحث عن GOP handle ويحمّل معلومات الأوضاع المتاحة
     *
     * @brief (EN) Initialize GOP
     *        Locates GOP handle and loads available mode information
     *
     * @return حالة UEFI / EFI_STATUS
     */
    EFI_STATUS initializeGOP();

    /**
     * @brief (AR) تعيين وضع الرسوميات / (EN) Set graphics mode
     * @param modeNumber رقم الوضع / Mode number
     * @return حالة UEFI / EFI_STATUS
     */
    EFI_STATUS setGOPMode(uint32_t modeNumber);

    /**
     * @brief (AR) الحصول على معلومات وضع معين / (EN) Query specific mode info
     * @param modeNumber رقم الوضع / Mode number
     * @param info معلومات الوضع (إخراج) / Mode info (output)
     */
    EFI_STATUS queryGOPMode(uint32_t modeNumber, UefiGopModeInfo& info);

    /**
     * @brief (AR) عملية نقل كتل (BLT) / (EN) Block Transfer (BLT) operation
     * @param buffer مخزن البكسل / Pixel buffer (null for fill)
     * @param operation نوع العملية / Operation type
     * @param srcX, srcY إحداثيات المصدر / Source coordinates
     * @param dstX, dstY إحداثيات الوجهة / Destination coordinates
     * @param width, height الأبعاد / Dimensions
     */
    EFI_STATUS gopBlt(UefiGopBltPixel* buffer, UefiGopBltOperation operation,
                       uint32_t srcX, uint32_t srcY,
                       uint32_t dstX, uint32_t dstY,
                       uint32_t width, uint32_t height);

    /// @brief (AR) الحصول على عدد أوضاع GOP / (EN) Get number of GOP modes
    uint32_t getGOPModeCount() const;

    /// @brief (AR) الحصول على الوضع الحالي / (EN) Get current mode number
    uint32_t getCurrentGOPMode() const;

    /// @brief (AR) الحصول على عنوان المخزن المؤقت / (EN) Get framebuffer base
    uint64_t getFramebufferBase() const;

    /// @brief (AR) الحصول على حجم المخزن / (EN) Get framebuffer size
    uint64_t getFramebufferSize() const;

    /// @brief (AR) الحصول على حالة GOP / (EN) Get GOP state
    const UefiGopState& getGOPState() const { return gopState_; }

    // ========================================================================
    //        خدمات وقت التشغيل / Runtime Services
    // ========================================================================

    /**
     * @brief (AR) الحصول على الوقت الحالي / (EN) Get current time
     * @return الوقت في هيكل EfiTime / Time in EfiTime structure
     */
    EfiTime getTime();

    /**
     * @brief (AR) تعيين الوقت / (EN) Set time
     * @param time الوقت الجديد / New time
     * @return حالة UEFI / EFI_STATUS
     */
    EFI_STATUS setTime(const EfiTime& time);

    /**
     * @brief (AR) قراءة متغير NVRAM / (EN) Read NVRAM variable
     * @param name اسم المتغير / Variable name
     * @param vendorGuid معرّف البائع / Vendor GUID
     * @param data البيانات (إخراج) / Data (output)
     * @return حالة UEFI / EFI_STATUS
     */
    EFI_STATUS getVariable(const std::string& name, const EFI_GUID& vendorGuid,
                            std::vector<uint8_t>& data);

    /**
     * @brief (AR) كتابة متغير NVRAM / (EN) Write NVRAM variable
     * @param name اسم المتغير / Variable name
     * @param vendorGuid معرّف البائع / Vendor GUID
     * @param attributes سمات المتغير / Variable attributes
     * @param data البيانات / Data
     */
    EFI_STATUS setVariable(const std::string& name, const EFI_GUID& vendorGuid,
                            uint32_t attributes, const std::vector<uint8_t>& data);

    /**
     * @brief (AR) إعادة تشغيل/إيقاف النظام / (EN) Reset/shutdown system
     * @param resetType نوع إعادة التشغيل (0=بارد, 1=دافئ, 2=إيقاف)
     *        Reset type (0=cold, 1=warm, 2=shutdown)
     */
    void resetSystem(int resetType);

    // ========================================================================
    //        نظام الملفات / File System Protocol
    // ========================================================================

    /**
     * @brief (AR) فتح وحدة تخزين / (EN) Open volume
     *        يبحث عن بروتوكول نظام الملفات البسيط ويفتح المجلد الجذر
     *
     * @return واصف الملف الجذر (< 0 = خطأ) / Root file descriptor (-1 = error)
     */
    int64_t openVolume();

    /**
     * @brief (AR) فتح ملف / (EN) Open file
     * @param parentFd واصف المجلد الأب / Parent directory descriptor
     * @param filename اسم الملف / File name
     * @param mode وضع الفتح / Open mode (EfiFileMode)
     * @param attributes السمات (عند الإنشاء) / Attributes (on creation)
     * @return واصف الملف / File descriptor (-1 = error)
     */
    int64_t openFile(int64_t parentFd, const std::string& filename,
                      uint64_t mode = EfiFileMode::READ,
                      uint64_t attributes = 0);

    /**
     * @brief (AR) قراءة ملف / (EN) Read file
     * @param fd واصف الملف / File descriptor
     * @param buffer مخزن البيانات / Data buffer
     * @param size الحجم المطلوب / Requested size
     * @return عدد البايتات المقروءة / Bytes read (-1 = error)
     */
    int64_t readFile(int64_t fd, std::vector<uint8_t>& buffer, uint64_t size);

    /**
     * @brief (AR) كتابة ملف / (EN) Write file
     * @param fd واصف الملف / File descriptor
     * @param data البيانات / Data
     * @return عدد البايتات المكتوبة / Bytes written (-1 = error)
     */
    int64_t writeFile(int64_t fd, const std::vector<uint8_t>& data);

    /**
     * @brief (AR) إغلاق ملف / (EN) Close file
     * @param fd واصف الملف / File descriptor
     */
    EFI_STATUS closeFile(int64_t fd);

    /**
     * @brief (AR) الحصول على معلومات ملف / (EN) Get file info
     * @param fd واصف الملف / File descriptor
     * @return معلومات الملف / File info
     */
    UefiFileInfo getFileInfo(int64_t fd);

    // ========================================================================
    //         بروتوكولات / Protocol Services
    // ========================================================================

    /**
     * @brief (AR) البحث عن بروتوكول / (EN) Locate protocol
     * @param guid معرّف البروتوكول / Protocol GUID
     * @return مقبض البروتوكول / Protocol handle (0 = not found)
     */
    uint64_t locateProtocol(const EFI_GUID& guid);

    // ========================================================================
    //         معلومات النظام / System Information
    // ========================================================================

    /// @brief (AR) الحصول على إصدار UEFI / (EN) Get UEFI revision
    std::string getUefiRevision() const;

    /// @brief (AR) اسم البائع / (EN) Firmware vendor name
    std::string getFirmwareVendor() const;

    /// @brief (AR) إصدار البرنامج الثابت / (EN) Firmware revision
    uint32_t getFirmwareRevision() const;

    // ========================================================================
    //         التقارير / Reports
    // ========================================================================

    /**
     * @brief (AR) توليد تقرير شامل
     *        يشمل: معلومات النظام، خريطة الذاكرة، GOP، الملفات
     *
     * @brief (EN) Generate comprehensive report
     *        Includes: system info, memory map, GOP, files
     *
     * @return التقرير كنص / Report as string
     */
    std::string generateReport() const;

    /**
     * @brief (AR) تقرير خريطة الذاكرة / (EN) Memory map report
     */
    std::string generateMemoryMapReport() const;

    /**
     * @brief (AR) تقرير GOP / (EN) GOP report
     */
    std::string generateGOPReport() const;

private:
    UefiManager();
    UefiManager(const UefiManager&) = delete;
    UefiManager& operator=(const UefiManager&) = delete;

    // الحالة / State
    bool initialized_;
    bool bootServicesExited_;
    EFI_HANDLE imageHandle_;
    uint64_t systemTablePtr_;

    // معلومات النظام / System info
    std::string firmwareVendor_;
    uint32_t firmwareRevision_;
    uint16_t uefiMajorRevision_;
    uint16_t uefiMinorRevision_;

    // خريطة الذاكرة / Memory map
    std::vector<EfiMemoryDescriptor> memoryMap_;
    uint64_t memoryMapKey_;
    uint64_t nextAllocAddress_;

    // GOP
    UefiGopState gopState_;

    // نظام الملفات / File system
    struct FileEntry {
        bool open;
        std::string name;
        uint64_t offset;
        uint64_t size;
        std::vector<uint8_t> data;  // محاكاة / Simulation data
        bool isDirectory;
    };
    std::vector<FileEntry> openFiles_;
    int64_t nextFd_;

    // NVRAM
    struct NvramVar {
        std::string name;
        EFI_GUID guid;
        uint32_t attributes;
        std::vector<uint8_t> data;
    };
    std::vector<NvramVar> nvramVars_;

    // مساعدون / Helpers
    void createSimulationMemoryMap();
    void createSimulationGOP();
    int64_t allocateFileDescriptor();
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_UEFI_H
