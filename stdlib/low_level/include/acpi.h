/*
 * ============================================================================
 * وحدة ACPI - واجهة الطاقة والتهيئة المتقدمة - لغة ص
 * ACPI Module - Advanced Configuration and Power Interface - Sad Language
 * ============================================================================
 *
 * @brief (AR) تحليل جداول ACPI وإدارة الطاقة واكتشاف الأجهزة
 *              دعم RSDP و RSDT و XSDT و MADT و FADT و HPET و MCFG
 *
 * @brief (EN) ACPI table parsing, power management, device discovery
 *              RSDP, RSDT, XSDT, MADT, FADT, HPET, MCFG support
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_ACPI_H
#define SAD_LOW_LEVEL_ACPI_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace Sad {
namespace LowLevel {

// ============================================================================
// هياكل ACPI الأساسية / Core ACPI Structures
// ============================================================================

/// @brief (AR) RSDP - مؤشر وصف نظام الجذر / (EN) Root System Description Pointer
#pragma pack(push, 1)
struct RSDP {
    char     signature[8];      // "RSD PTR "
    uint8_t  checksum;
    char     oemId[6];
    uint8_t  revision;          // 0 = ACPI 1.0, 2 = ACPI 2.0+
    uint32_t rsdtAddress;       // عنوان RSDT (32-بت) / RSDT address (32-bit)

    // حقول ACPI 2.0+ / ACPI 2.0+ fields
    uint32_t length;
    uint64_t xsdtAddress;       // عنوان XSDT (64-بت) / XSDT address (64-bit)
    uint8_t  extendedChecksum;
    uint8_t  reserved[3];
};

/// @brief (AR) ترويسة جدول SDT / (EN) SDT Header
struct SDTHeader {
    char     signature[4];
    uint32_t length;
    uint8_t  revision;
    uint8_t  checksum;
    char     oemId[6];
    char     oemTableId[8];
    uint32_t oemRevision;
    uint32_t creatorId;
    uint32_t creatorRevision;
};

/// @brief (AR) جدول RSDT / (EN) RSDT Table
struct RSDT {
    SDTHeader header;
    uint32_t entries[];         // مصفوفة مؤشرات 32-بت / Array of 32-bit pointers
};

/// @brief (AR) جدول XSDT / (EN) XSDT Table
struct XSDT {
    SDTHeader header;
    uint64_t entries[];         // مصفوفة مؤشرات 64-بت / Array of 64-bit pointers
};

// ============================================================================
// MADT - جدول وصف المقاطعات المتعدد / Multiple APIC Description Table
// ============================================================================

/// @brief (AR) ترويسة MADT / (EN) MADT Header
struct MADT {
    SDTHeader header;
    uint32_t localApicAddress;  // عنوان Local APIC / Local APIC address
    uint32_t flags;             // إذا البت 0 = يوجد 8259 PIC / Bit 0 = dual 8259 PICs installed
};

/// @brief (AR) أنواع مدخلات MADT / (EN) MADT Entry Types
enum class MADTEntryType : uint8_t {
    LOCAL_APIC              = 0,
    IO_APIC                 = 1,
    INTERRUPT_OVERRIDE      = 2,
    NMI_SOURCE              = 3,
    LOCAL_APIC_NMI          = 4,
    LOCAL_APIC_OVERRIDE     = 5,
    IO_SAPIC                = 6,
    LOCAL_SAPIC             = 7,
    PLATFORM_INTERRUPT      = 8,
    LOCAL_X2APIC            = 9,
    LOCAL_X2APIC_NMI        = 10,
    GIC                     = 11,
    GIC_DISTRIBUTOR         = 12
};

/// @brief (AR) ترويسة مدخل MADT / (EN) MADT Entry Header
struct MADTEntryHeader {
    uint8_t type;
    uint8_t length;
};

/// @brief (AR) مدخل Local APIC / (EN) Local APIC Entry
struct MADTLocalAPIC {
    MADTEntryHeader header;
    uint8_t  processorId;       // معرّف المعالج / Processor ID
    uint8_t  apicId;            // معرّف APIC / APIC ID
    uint32_t flags;             // البت 0 = مفعل / Bit 0 = enabled
};

/// @brief (AR) مدخل I/O APIC / (EN) I/O APIC Entry
struct MADTIOAPIC {
    MADTEntryHeader header;
    uint8_t  ioApicId;          // معرّف I/O APIC / I/O APIC ID
    uint8_t  reserved;
    uint32_t ioApicAddress;     // عنوان I/O APIC / I/O APIC address
    uint32_t globalSystemInterruptBase; // قاعدة GSI / GSI base
};

/// @brief (AR) تجاوز مقاطعة / (EN) Interrupt Source Override
struct MADTInterruptOverride {
    MADTEntryHeader header;
    uint8_t  bus;               // دائماً 0 (ISA) / Always 0 (ISA)
    uint8_t  source;            // مصدر IRQ / IRQ source
    uint32_t globalSystemInterrupt;
    uint16_t flags;             // أعلام القطبية والتفعيل / Polarity/Trigger flags
};

/// @brief (AR) NMI المحلي / (EN) Local APIC NMI
struct MADTLocalAPICNMI {
    MADTEntryHeader header;
    uint8_t  processorId;       // 0xFF = جميع المعالجات / 0xFF = all processors
    uint16_t flags;
    uint8_t  lintNumber;        // LINT# (0 أو 1) / LINT# (0 or 1)
};

// ============================================================================
// FADT - جدول وصف ACPI الثابت / Fixed ACPI Description Table
// ============================================================================

/// @brief (AR) بنية العنوان العام / (EN) Generic Address Structure
struct GenericAddress {
    uint8_t  addressSpace;      // 0=ذاكرة, 1=IO / 0=Memory, 1=IO
    uint8_t  bitWidth;
    uint8_t  bitOffset;
    uint8_t  accessSize;        // 0=غير محدد, 1=بايت, 2=كلمة, 3=مزدوج, 4=رباعي
    uint64_t address;
};

/// @brief (AR) جدول FADT / (EN) FADT Table
struct FADT {
    SDTHeader header;
    uint32_t firmwareCtrl;      // عنوان FACS / FACS address
    uint32_t dsdt;              // عنوان DSDT / DSDT address
    uint8_t  reserved0;
    uint8_t  preferredPMProfile;
    uint16_t sciInterrupt;      // مقاطعة SCI / SCI Interrupt
    uint32_t smiCommandPort;    // منفذ أمر SMI / SMI Command port
    uint8_t  acpiEnable;
    uint8_t  acpiDisable;
    uint8_t  s4BiosReq;
    uint8_t  pStateControl;
    uint32_t pm1aEventBlock;
    uint32_t pm1bEventBlock;
    uint32_t pm1aControlBlock;
    uint32_t pm1bControlBlock;
    uint32_t pm2ControlBlock;
    uint32_t pmTimerBlock;
    uint32_t gpe0Block;
    uint32_t gpe1Block;
    uint8_t  pm1EventLength;
    uint8_t  pm1ControlLength;
    uint8_t  pm2ControlLength;
    uint8_t  pmTimerLength;
    uint8_t  gpe0Length;
    uint8_t  gpe1Length;
    uint8_t  gpe1Base;
    uint8_t  cStateControl;
    uint16_t worstC2Latency;
    uint16_t worstC3Latency;
    uint16_t flushSize;
    uint16_t flushStride;
    uint8_t  dutyOffset;
    uint8_t  dutyWidth;
    uint8_t  dayAlarm;
    uint8_t  monthAlarm;
    uint8_t  century;
    uint16_t bootArchitectureFlags; // أعلام معمارية الإقلاع / Boot architecture flags
    uint8_t  reserved1;
    uint32_t flags;

    // حقول إعادة الضبط / Reset fields
    GenericAddress resetReg;
    uint8_t  resetValue;
    uint16_t armBootArchFlags;
    uint8_t  fadtMinorVersion;

    // عناوين 64-بت / 64-bit addresses
    uint64_t xFirmwareControl;
    uint64_t xDsdt;
    GenericAddress xPm1aEventBlock;
    GenericAddress xPm1bEventBlock;
    GenericAddress xPm1aControlBlock;
    GenericAddress xPm1bControlBlock;
    GenericAddress xPm2ControlBlock;
    GenericAddress xPmTimerBlock;
    GenericAddress xGpe0Block;
    GenericAddress xGpe1Block;
    GenericAddress sleepControlReg;
    GenericAddress sleepStatusReg;
    uint64_t hypervisorVendorId;
};

// ============================================================================
// MCFG - جدول تهيئة الذاكرة لـ PCIe / PCIe Memory Configuration Table
// ============================================================================

/// @brief (AR) مدخل MCFG / (EN) MCFG Entry
struct MCFGEntry {
    uint64_t baseAddress;       // عنوان ECAM الأساسي / ECAM base address
    uint16_t pciSegmentGroup;   // مجموعة شريحة PCI / PCI segment group
    uint8_t  startBusNumber;    // رقم أول ناقل / Start bus number
    uint8_t  endBusNumber;      // رقم آخر ناقل / End bus number
    uint32_t reserved;
};

/// @brief (AR) جدول MCFG / (EN) MCFG Table
struct MCFG {
    SDTHeader header;
    uint64_t reserved;
    MCFGEntry entries[];        // مدخلات ECAM
};

// ============================================================================
// HPET - جدول مؤقت الأحداث عالي الدقة / High Precision Event Timer Table
// ============================================================================

/// @brief (AR) جدول HPET في ACPI / (EN) HPET ACPI Table
struct HPETTable {
    SDTHeader header;
    uint8_t  hardwareRevId;
    uint8_t  comparatorCount : 5;
    uint8_t  counterSize : 1;
    uint8_t  reserved0 : 1;
    uint8_t  legacyReplacement : 1;
    uint16_t pciVendorId;
    GenericAddress address;
    uint8_t  hpetNumber;
    uint16_t minimumTick;
    uint8_t  pageProtection;
};
#pragma pack(pop)

// ============================================================================
// بيانات MADT المحللة / Parsed MADT Data
// ============================================================================

/// @brief (AR) معلومات APIC محلي من MADT / (EN) Local APIC Info from MADT
struct MADTLocalAPICInfo {
    uint8_t processorId;
    uint8_t apicId;
    bool    enabled;
};

/// @brief (AR) معلومات I/O APIC من MADT / (EN) I/O APIC Info from MADT
struct MADTIOAPICInfo {
    uint8_t  id;
    uint32_t address;
    uint32_t gsiBase;
};

/// @brief (AR) تجاوز IRQ / (EN) IRQ Override
struct IRQOverride {
    uint8_t  source;
    uint32_t gsi;
    uint16_t flags;
};

// ============================================================================
// حالات الطاقة / Power States
// ============================================================================

/// @brief (AR) حالة النوم / (EN) Sleep State
enum class SleepState : uint8_t {
    S0 = 0,     // يقظة / Working
    S1 = 1,     // نوم خفيف / Light sleep (CPU stops)
    S2 = 2,     // نوم أعمق / Deeper sleep
    S3 = 3,     // تعليق للذاكرة / Suspend to RAM
    S4 = 4,     // السبات / Hibernate (Suspend to disk)
    S5 = 5      // إيقاف ناعم / Soft off
};

// ============================================================================
// مدير ACPI / ACPI Manager
// ============================================================================

/**
 * @brief (AR) مدير ACPI - تحليل الجداول واكتشاف الأجهزة وإدارة الطاقة
 * @brief (EN) ACPI Manager - Table parsing, device discovery, power management
 */
class ACPIManager {
public:
    /// @brief (AR) الحصول على المثيل الوحيد / (EN) Get singleton instance
    static ACPIManager& getInstance();

    // ========================================================================
    // التهيئة / Initialization
    // ========================================================================

    /// @brief (AR) تهيئة ACPI (البحث عن RSDP وتحليل الجداول)
    /// @brief (EN) Initialize ACPI (find RSDP and parse tables)
    bool initialize();

    /// @brief (AR) تهيئة من RSDP معروف / (EN) Initialize from known RSDP address
    bool initializeFromRSDP(uint64_t rsdpAddress);

    // ========================================================================
    // البحث عن الجداول / Table Lookup
    // ========================================================================

    /// @brief (AR) البحث عن جدول بالتوقيع / (EN) Find table by signature
    const SDTHeader* findTable(const char* signature) const;

    /// @brief (AR) الحصول على FADT / (EN) Get FADT
    const FADT* getFADT() const { return fadt_; }

    /// @brief (AR) الحصول على MADT / (EN) Get MADT
    const MADT* getMADT() const { return madt_; }

    /// @brief (AR) الحصول على MCFG / (EN) Get MCFG
    const MCFG* getMCFG() const { return mcfg_; }

    /// @brief (AR) الحصول على HPET / (EN) Get HPET table
    const HPETTable* getHPETTable() const { return hpetTable_; }

    // ========================================================================
    // معلومات APIC / APIC Information
    // ========================================================================

    /// @brief (AR) الحصول على Local APICs / (EN) Get Local APICs
    const std::vector<MADTLocalAPICInfo>& getLocalAPICs() const { return localApics_; }

    /// @brief (AR) الحصول على I/O APICs / (EN) Get I/O APICs
    const std::vector<MADTIOAPICInfo>& getIOAPICs() const { return ioApics_; }

    /// @brief (AR) الحصول على تجاوزات IRQ / (EN) Get IRQ overrides
    const std::vector<IRQOverride>& getIRQOverrides() const { return irqOverrides_; }

    /// @brief (AR) عنوان Local APIC / (EN) Local APIC address
    uint32_t getLocalAPICAddress() const { return localApicAddress_; }

    /// @brief (AR) عدد المعالجات / (EN) Processor count
    uint32_t getProcessorCount() const { return static_cast<uint32_t>(localApics_.size()); }

    // ========================================================================
    // إدارة الطاقة / Power Management
    // ========================================================================

    /// @brief (AR) تفعيل ACPI / (EN) Enable ACPI
    bool enableACPI();

    /// @brief (AR) تعطيل ACPI / (EN) Disable ACPI
    bool disableACPI();

    /// @brief (AR) إيقاف التشغيل / (EN) Shutdown (S5)
    void shutdown();

    /// @brief (AR) إعادة التشغيل / (EN) Reboot
    void reboot();

    /// @brief (AR) الدخول في حالة نوم / (EN) Enter sleep state
    bool enterSleepState(SleepState state);

    // ========================================================================
    // مؤقت ACPI / ACPI Timer
    // ========================================================================

    /// @brief (AR) قراءة مؤقت PM / (EN) Read PM timer
    uint32_t readPMTimer() const;

    /// @brief (AR) هل المؤقت 32-بت / (EN) Is timer 32-bit
    bool isPMTimer32Bit() const;

    /// @brief (AR) تأخير بالميكروثانية / (EN) Delay in microseconds
    void delayMicroseconds(uint32_t us) const;

    // ========================================================================
    // PCIe ECAM / تهيئة PCIe
    // ========================================================================

    /// @brief (AR) الحصول على عنوان ECAM الأساسي / (EN) Get ECAM base for bus
    uint64_t getECAMBase(uint16_t segment, uint8_t bus) const;

    /// @brief (AR) الحصول على مدخلات MCFG / (EN) Get MCFG entries
    const std::vector<MCFGEntry>& getMCFGEntries() const { return mcfgEntries_; }

    // ========================================================================
    // معلومات / Information
    // ========================================================================

    /// @brief (AR) إصدار ACPI / (EN) ACPI version
    uint8_t getACPIVersion() const { return acpiVersion_; }

    /// @brief (AR) هل ACPI مهيأ / (EN) Is initialized
    bool isInitialized() const { return isInitialized_; }

    /// @brief (AR) توليد تقرير / (EN) Generate report
    std::string generateReport() const;

private:
    ACPIManager();
    ACPIManager(const ACPIManager&) = delete;
    ACPIManager& operator=(const ACPIManager&) = delete;

    /// @brief (AR) البحث عن RSDP في الذاكرة / (EN) Search for RSDP in memory
    const RSDP* findRSDP() const;

    /// @brief (AR) التحقق من المجموع الفحصي / (EN) Validate checksum
    bool validateChecksum(const void* data, size_t length) const;

    /// @brief (AR) تحليل RSDT / (EN) Parse RSDT
    void parseRSDT(const RSDT* rsdt);

    /// @brief (AR) تحليل XSDT / (EN) Parse XSDT
    void parseXSDT(const XSDT* xsdt);

    /// @brief (AR) تحليل MADT / (EN) Parse MADT
    void parseMADT(const MADT* madt);

    /// @brief (AR) تحليل MCFG / (EN) Parse MCFG
    void parseMCFG(const MCFG* mcfg);

    // المؤشرات المحفوظة / Cached pointers
    const RSDP*      rsdp_;
    const FADT*      fadt_;
    const MADT*      madt_;
    const MCFG*      mcfg_;
    const HPETTable* hpetTable_;

    // قائمة جميع الجداول / All table list
    std::vector<const SDTHeader*> tables_;

    // بيانات MADT محللة / Parsed MADT data
    std::vector<MADTLocalAPICInfo> localApics_;
    std::vector<MADTIOAPICInfo>    ioApics_;
    std::vector<IRQOverride>   irqOverrides_;
    uint32_t localApicAddress_;

    // بيانات MCFG محللة / Parsed MCFG data
    std::vector<MCFGEntry> mcfgEntries_;

    // الحالة / State
    uint8_t acpiVersion_;
    bool    isInitialized_;
};

// ============================================================================
// ثوابت ACPI / ACPI Constants
// ============================================================================

namespace ACPIConstants {
    // توقيعات الجداول / Table signatures
    static constexpr const char* SIG_FACP = "FACP";    // FADT
    static constexpr const char* SIG_APIC = "APIC";    // MADT
    static constexpr const char* SIG_HPET = "HPET";
    static constexpr const char* SIG_MCFG = "MCFG";
    static constexpr const char* SIG_DSDT = "DSDT";
    static constexpr const char* SIG_SSDT = "SSDT";
    static constexpr const char* SIG_BGRT = "BGRT";
    static constexpr const char* SIG_WAET = "WAET";

    // مناطق البحث عن RSDP / RSDP search areas
    static constexpr uint64_t EBDA_START      = 0x00080000;
    static constexpr uint64_t EBDA_END        = 0x0009FFFF;
    static constexpr uint64_t BIOS_ROM_START  = 0x000E0000;
    static constexpr uint64_t BIOS_ROM_END    = 0x000FFFFF;

    // أعلام معمارية الإقلاع / Boot architecture flags
    static constexpr uint16_t LEGACY_DEVICES       = 0x0001;
    static constexpr uint16_t KEYBOARD_8042        = 0x0002;
    static constexpr uint16_t VGA_NOT_PRESENT      = 0x0004;
    static constexpr uint16_t MSI_NOT_SUPPORTED    = 0x0008;
    static constexpr uint16_t PCIE_ASPM_CONTROLS   = 0x0010;

    // أعلام FADT / FADT flags
    static constexpr uint32_t FADT_WBINVD          = (1 << 0);
    static constexpr uint32_t FADT_SLEEP_BUTTON    = (1 << 5);
    static constexpr uint32_t FADT_RESET_REG       = (1 << 10);
    static constexpr uint32_t FADT_HW_REDUCED      = (1 << 20);

    // قيم SLP_TYP للنوم / Sleep type values (typical)
    static constexpr uint16_t SLP_EN  = (1 << 13);
    static constexpr uint16_t PM1_CNT_SLP_TYP_S5 = (7 << 10); // نموذجي لـ S5

    // تردد مؤقت PM / PM Timer frequency
    static constexpr uint32_t PM_TIMER_FREQ = 3579545;  // 3.579545 MHz
}

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_ACPI_H
