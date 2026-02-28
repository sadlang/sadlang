/*
 * ============================================================================
 * ناقل PCI/PCIe للغة ص - PCI/PCIe Bus for Sad Language
 * ============================================================================
 *
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يوفر واجهة شاملة للتعامل مع ناقل PCI/PCIe.
 * يشمل اكتشاف الأجهزة، قراءة/كتابة فضاء التهيئة، إدارة BAR،
 * وإدارة المقاطعات (MSI/MSI-X).
 * ضروري لاكتشاف بطاقة الرسومات والتواصل مع جميع أجهزة الحاسوب.
 *
 * English Description:
 * -------------------
 * This file provides a comprehensive PCI/PCIe bus interface.
 * Includes device enumeration, configuration space read/write, BAR management,
 * and interrupt management (MSI/MSI-X).
 * Essential for GPU discovery and communication with all computer devices.
 *
 * المؤلف / Author: Sad Language Team
 * الإصدار / Version: 2.0.0
 * ============================================================================
 */

#ifndef SAD_PCI_H
#define SAD_PCI_H

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace Sad {
namespace LowLevel {

// ============================================================================
// ثوابت PCI / PCI Constants
// ============================================================================

namespace PCIConstants {
    // عناوين منافذ تهيئة PCI / PCI configuration port addresses
    constexpr uint16_t CONFIG_ADDRESS = 0xCF8;
    constexpr uint16_t CONFIG_DATA    = 0xCFC;

    // إزاحات فضاء التهيئة / Configuration space offsets
    constexpr uint8_t VENDOR_ID       = 0x00;
    constexpr uint8_t DEVICE_ID       = 0x02;
    constexpr uint8_t COMMAND         = 0x04;
    constexpr uint8_t STATUS          = 0x06;
    constexpr uint8_t REVISION_ID     = 0x08;
    constexpr uint8_t PROG_IF         = 0x09;
    constexpr uint8_t SUBCLASS        = 0x0A;
    constexpr uint8_t CLASS_CODE      = 0x0B;
    constexpr uint8_t CACHE_LINE_SIZE = 0x0C;
    constexpr uint8_t LATENCY_TIMER   = 0x0D;
    constexpr uint8_t HEADER_TYPE     = 0x0E;
    constexpr uint8_t BIST            = 0x0F;
    constexpr uint8_t BAR0            = 0x10;
    constexpr uint8_t BAR1            = 0x14;
    constexpr uint8_t BAR2            = 0x18;
    constexpr uint8_t BAR3            = 0x1C;
    constexpr uint8_t BAR4            = 0x20;
    constexpr uint8_t BAR5            = 0x24;
    constexpr uint8_t CARDBUS_CIS     = 0x28;
    constexpr uint8_t SUBSYS_VENDOR   = 0x2C;
    constexpr uint8_t SUBSYS_ID       = 0x2E;
    constexpr uint8_t EXPANSION_ROM   = 0x30;
    constexpr uint8_t CAPABILITIES    = 0x34;
    constexpr uint8_t INTERRUPT_LINE  = 0x3C;
    constexpr uint8_t INTERRUPT_PIN   = 0x3D;
    constexpr uint8_t MIN_GRANT       = 0x3E;
    constexpr uint8_t MAX_LATENCY     = 0x3F;

    // بتات سجل الأوامر / Command register bits
    constexpr uint16_t CMD_IO_SPACE       = 1 << 0;
    constexpr uint16_t CMD_MEMORY_SPACE   = 1 << 1;
    constexpr uint16_t CMD_BUS_MASTER     = 1 << 2;
    constexpr uint16_t CMD_SPECIAL_CYCLES = 1 << 3;
    constexpr uint16_t CMD_MWI_ENABLE     = 1 << 4;
    constexpr uint16_t CMD_VGA_PALETTE    = 1 << 5;
    constexpr uint16_t CMD_PARITY_ERROR   = 1 << 6;
    constexpr uint16_t CMD_SERR_ENABLE    = 1 << 8;
    constexpr uint16_t CMD_FAST_B2B       = 1 << 9;
    constexpr uint16_t CMD_INT_DISABLE    = 1 << 10;

    // أصناف الأجهزة الرئيسية / Major device classes
    constexpr uint8_t CLASS_UNCLASSIFIED  = 0x00;
    constexpr uint8_t CLASS_STORAGE       = 0x01;
    constexpr uint8_t CLASS_NETWORK       = 0x02;
    constexpr uint8_t CLASS_DISPLAY       = 0x03;
    constexpr uint8_t CLASS_MULTIMEDIA    = 0x04;
    constexpr uint8_t CLASS_MEMORY        = 0x05;
    constexpr uint8_t CLASS_BRIDGE        = 0x06;
    constexpr uint8_t CLASS_COMMUNICATION = 0x07;
    constexpr uint8_t CLASS_SYSTEM        = 0x08;
    constexpr uint8_t CLASS_INPUT         = 0x09;
    constexpr uint8_t CLASS_DOCKING       = 0x0A;
    constexpr uint8_t CLASS_PROCESSOR     = 0x0B;
    constexpr uint8_t CLASS_SERIAL_BUS    = 0x0C;
    constexpr uint8_t CLASS_WIRELESS      = 0x0D;
    constexpr uint8_t CLASS_ENCRYPTION    = 0x10;
    constexpr uint8_t CLASS_SIGNAL_PROC   = 0x11;

    // أصناف العرض الفرعية / Display subclasses
    constexpr uint8_t DISPLAY_VGA        = 0x00;
    constexpr uint8_t DISPLAY_XGA        = 0x01;
    constexpr uint8_t DISPLAY_3D         = 0x02;
    constexpr uint8_t DISPLAY_OTHER      = 0x80;

    // معرفات شركات GPU المعروفة / Known GPU vendor IDs
    constexpr uint16_t VENDOR_NVIDIA  = 0x10DE;
    constexpr uint16_t VENDOR_AMD     = 0x1002;
    constexpr uint16_t VENDOR_INTEL   = 0x8086;

    // معرف غير صالح / Invalid vendor ID
    constexpr uint16_t VENDOR_INVALID = 0xFFFF;

    // معرفات القدرات / Capability IDs
    constexpr uint8_t CAP_PM             = 0x01;  ///< إدارة الطاقة / Power Management
    constexpr uint8_t CAP_AGP            = 0x02;  ///< AGP
    constexpr uint8_t CAP_VPD            = 0x03;  ///< بيانات المنتج الحيوية / Vital Product Data
    constexpr uint8_t CAP_MSI            = 0x05;  ///< مقاطعات MSI / Message Signaled Interrupts
    constexpr uint8_t CAP_VENDOR         = 0x09;  ///< الشركة المصنعة / Vendor Specific
    constexpr uint8_t CAP_PCIE           = 0x10;  ///< PCI Express
    constexpr uint8_t CAP_MSIX           = 0x11;  ///< MSI-X
}

// ============================================================================
// عنوان PCI / PCI Address
// ============================================================================

struct PCIAddress {
    uint8_t bus;       ///< رقم الناقل (0-255) / Bus number
    uint8_t device;    ///< رقم الجهاز (0-31) / Device number
    uint8_t function;  ///< رقم الدالة (0-7) / Function number

    PCIAddress() : bus(0), device(0), function(0) {}
    PCIAddress(uint8_t b, uint8_t d, uint8_t f) : bus(b), device(d), function(f) {}

    /**
     * إنشاء عنوان تهيئة PCI بصيغة 32-بت
     * Create 32-bit PCI configuration address
     */
    uint32_t toConfigAddress(uint8_t offset) const {
        return (1U << 31)
             | (static_cast<uint32_t>(bus)      << 16)
             | (static_cast<uint32_t>(device)   << 11)
             | (static_cast<uint32_t>(function) << 8)
             | (offset & 0xFC);
    }

    bool operator==(const PCIAddress& other) const {
        return bus == other.bus && device == other.device && function == other.function;
    }

    std::string toString() const;
};

// ============================================================================
// نوع BAR / BAR Type
// ============================================================================

enum class BARType {
    MEMORY_32,      ///< ذاكرة 32-بت / 32-bit Memory
    MEMORY_64,      ///< ذاكرة 64-بت / 64-bit Memory
    IO_SPACE        ///< فضاء I/O / I/O Space
};

/**
 * معلومات سجل العنوان الأساسي (BAR)
 * Base Address Register (BAR) Information
 */
struct BARInfo {
    uint64_t baseAddress;   ///< العنوان الأساسي / Base address
    uint64_t size;          ///< الحجم / Size
    BARType type;           ///< النوع / Type
    bool prefetchable;      ///< قابل للجلب المسبق / Prefetchable
    int barIndex;           ///< رقم BAR / BAR index

    BARInfo()
        : baseAddress(0), size(0), type(BARType::MEMORY_32)
        , prefetchable(false), barIndex(0) {}
};

// ============================================================================
// معلومات جهاز PCI / PCI Device Information
// ============================================================================

struct PCIDeviceInfo {
    PCIAddress address;          ///< العنوان / Address
    uint16_t vendorId;           ///< معرف الشركة / Vendor ID
    uint16_t deviceId;           ///< معرف الجهاز / Device ID
    uint8_t classCode;           ///< رمز الصنف / Class code
    uint8_t subclass;            ///< الصنف الفرعي / Subclass
    uint8_t progIF;              ///< واجهة البرمجة / Programming interface
    uint8_t revisionId;          ///< رقم المراجعة / Revision ID
    uint8_t headerType;          ///< نوع الترويسة / Header type
    uint16_t subsysVendorId;     ///< معرف شركة النظام الفرعي / Subsystem vendor ID
    uint16_t subsysId;           ///< معرف النظام الفرعي / Subsystem ID
    uint8_t interruptLine;       ///< خط المقاطعة / Interrupt line
    uint8_t interruptPin;        ///< دبوس المقاطعة / Interrupt pin
    uint8_t capabilitiesPtr;     ///< مؤشر القدرات / Capabilities pointer
    std::vector<BARInfo> bars;   ///< سجلات BAR / BAR registers

    PCIDeviceInfo()
        : vendorId(0), deviceId(0), classCode(0), subclass(0)
        , progIF(0), revisionId(0), headerType(0)
        , subsysVendorId(0), subsysId(0)
        , interruptLine(0), interruptPin(0), capabilitiesPtr(0) {}

    bool isDisplayController() const { return classCode == PCIConstants::CLASS_DISPLAY; }
    bool isVGACompatible() const { return classCode == PCIConstants::CLASS_DISPLAY && subclass == PCIConstants::DISPLAY_VGA; }
    bool is3DController() const { return classCode == PCIConstants::CLASS_DISPLAY && subclass == PCIConstants::DISPLAY_3D; }
    bool isNvidia() const { return vendorId == PCIConstants::VENDOR_NVIDIA; }
    bool isAMD() const { return vendorId == PCIConstants::VENDOR_AMD; }
    bool isIntel() const { return vendorId == PCIConstants::VENDOR_INTEL; }
    bool isMultiFunction() const { return (headerType & 0x80) != 0; }

    std::string getClassName() const;
    std::string getVendorName() const;
};

// ============================================================================
// بنية MSI / MSI Structure
// ============================================================================

struct MSIConfig {
    uint8_t capOffset;           ///< إزاحة القدرة في فضاء التهيئة / Capability offset
    uint16_t messageControl;     ///< تحكم الرسالة / Message control
    uint32_t messageAddress;     ///< عنوان الرسالة / Message address
    uint32_t messageAddressHigh; ///< العنوان العلوي (64-بت) / High address (64-bit)
    uint16_t messageData;        ///< بيانات الرسالة / Message data
    bool is64Bit;                ///< هل 64-بت؟ / Is 64-bit?
    bool isEnabled;              ///< مفعل؟ / Enabled?

    MSIConfig()
        : capOffset(0), messageControl(0), messageAddress(0)
        , messageAddressHigh(0), messageData(0)
        , is64Bit(false), isEnabled(false) {}
};

// ============================================================================
// مدير ناقل PCI / PCI Bus Manager
// ============================================================================

class PCIManager {
public:
    static PCIManager& getInstance();

    /**
     * مسح واكتشاف جميع أجهزة PCI
     * Scan and enumerate all PCI devices
     */
    void enumerate();

    /**
     * الحصول على قائمة الأجهزة المكتشفة
     * Get list of discovered devices
     */
    const std::vector<PCIDeviceInfo>& getDevices() const { return devices_; }

    /**
     * البحث عن أجهزة بصنف محدد
     * Find devices by class
     *
     * @param classCode رمز الصنف / Class code
     * @param subclass الصنف الفرعي (-1 لأي) / Subclass (-1 for any)
     * @return قائمة الأجهزة المطابقة / Matching devices
     */
    std::vector<PCIDeviceInfo> findByClass(uint8_t classCode, int subclass = -1) const;

    /**
     * البحث عن أجهزة بمعرف الشركة والجهاز
     * Find devices by vendor and device ID
     */
    std::vector<PCIDeviceInfo> findByVendor(uint16_t vendorId, uint16_t deviceId = 0xFFFF) const;

    /**
     * البحث عن وحدات معالجة الرسومات (GPU)
     * Find GPUs
     */
    std::vector<PCIDeviceInfo> findGPUs() const;

    /**
     * الحصول على GPU الأساسي
     * Get primary GPU
     */
    const PCIDeviceInfo* getPrimaryGPU() const;

    // ====================================================================
    // قراءة/كتابة فضاء التهيئة / Configuration Space Read/Write
    // ====================================================================

    static uint8_t  readConfig8(const PCIAddress& addr, uint8_t offset);
    static uint16_t readConfig16(const PCIAddress& addr, uint8_t offset);
    static uint32_t readConfig32(const PCIAddress& addr, uint8_t offset);

    static void writeConfig8(const PCIAddress& addr, uint8_t offset, uint8_t value);
    static void writeConfig16(const PCIAddress& addr, uint8_t offset, uint16_t value);
    static void writeConfig32(const PCIAddress& addr, uint8_t offset, uint32_t value);

    // ====================================================================
    // إدارة الأوامر / Command Management
    // ====================================================================

    /**
     * تفعيل Bus Master لجهاز
     * Enable Bus Master for device
     */
    static void enableBusMaster(const PCIAddress& addr);

    /**
     * تفعيل فضاء الذاكرة لجهاز
     * Enable memory space for device
     */
    static void enableMemorySpace(const PCIAddress& addr);

    /**
     * تفعيل فضاء I/O لجهاز
     * Enable I/O space for device
     */
    static void enableIOSpace(const PCIAddress& addr);

    /**
     * تعطيل المقاطعات القديمة لجهاز
     * Disable legacy interrupts for device
     */
    static void disableLegacyInterrupts(const PCIAddress& addr);

    // ====================================================================
    // إدارة BAR / BAR Management
    // ====================================================================

    /**
     * قراءة معلومات BAR لجهاز
     * Read BAR information for device
     *
     * @param addr عنوان الجهاز / Device address
     * @param barIndex رقم BAR (0-5) / BAR index (0-5)
     * @return معلومات BAR / BAR information
     */
    static BARInfo readBAR(const PCIAddress& addr, int barIndex);

    /**
     * قراءة جميع سجلات BAR لجهاز
     * Read all BAR registers for device
     */
    static std::vector<BARInfo> readAllBARs(const PCIAddress& addr);

    // ====================================================================
    // إدارة MSI / MSI Management
    // ====================================================================

    /**
     * البحث عن قدرة MSI
     * Find MSI capability
     */
    static bool findMSICapability(const PCIAddress& addr, MSIConfig& config);

    /**
     * تهيئة MSI لجهاز
     * Configure MSI for device
     *
     * @param addr عنوان الجهاز / Device address
     * @param vector رقم المتجه / Vector number
     * @param processor معرف المعالج المستهدف / Target processor ID
     * @return true عند النجاح / true on success
     */
    static bool configureMSI(const PCIAddress& addr, uint8_t vector, uint8_t processor);

    /**
     * تفعيل MSI لجهاز
     * Enable MSI for device
     */
    static bool enableMSI(const PCIAddress& addr);

    /**
     * تعطيل MSI لجهاز
     * Disable MSI for device
     */
    static void disableMSI(const PCIAddress& addr);

    // ====================================================================
    // القدرات / Capabilities
    // ====================================================================

    /**
     * البحث عن قدرة في سلسلة القدرات
     * Find capability in capability chain
     *
     * @param addr عنوان الجهاز / Device address
     * @param capId معرف القدرة / Capability ID
     * @return إزاحة القدرة أو 0 إذا لم توجد / Capability offset or 0 if not found
     */
    static uint8_t findCapability(const PCIAddress& addr, uint8_t capId);

    /**
     * فحص دعم PCIe
     * Check PCIe support
     */
    static bool isPCIeDevice(const PCIAddress& addr);

    /**
     * إنشاء تقرير الأجهزة
     * Generate device report
     */
    std::string generateReport() const;

private:
    PCIManager();
    ~PCIManager() = default;
    PCIManager(const PCIManager&) = delete;
    PCIManager& operator=(const PCIManager&) = delete;

    void scanBus(uint8_t bus);
    void scanDevice(uint8_t bus, uint8_t device);
    void scanFunction(uint8_t bus, uint8_t device, uint8_t function);
    PCIDeviceInfo readDeviceInfo(const PCIAddress& addr);

    std::vector<PCIDeviceInfo> devices_;
    bool isEnumerated_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_PCI_H
