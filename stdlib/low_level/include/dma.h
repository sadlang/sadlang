/*
 * ============================================================================
 * الوصول المباشر للذاكرة (DMA) للغة ص - DMA for Sad Language
 * ============================================================================
 *
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يوفر نظام الوصول المباشر للذاكرة (DMA).
 * يدعم DMA التقليدي (ISA)، Bus Mastering DMA، و Scatter-Gather DMA.
 * ضروري لنقل البيانات عالي السرعة بين الذاكرة والأجهزة مثل GPU.
 *
 * English Description:
 * -------------------
 * This file provides the Direct Memory Access (DMA) system.
 * Supports legacy (ISA) DMA, Bus Mastering DMA, and Scatter-Gather DMA.
 * Essential for high-speed data transfer between memory and devices like GPU.
 *
 * المؤلف / Author: Sad Language Team
 * الإصدار / Version: 2.0.0
 * ============================================================================
 */

#ifndef SAD_DMA_H
#define SAD_DMA_H

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace Sad {
namespace LowLevel {

// ============================================================================
// ثوابت DMA / DMA Constants
// ============================================================================

namespace DMAConstants {
    constexpr int MAX_CHANNELS = 8;
    constexpr int MAX_SG_ENTRIES = 256;
    constexpr uint32_t MAX_TRANSFER_SIZE_ISA = 65536;
    constexpr uint64_t MAX_TRANSFER_SIZE_BM  = 0xFFFFFFFFULL;

    // منافذ DMA التقليدية (ISA) / Legacy DMA ports
    constexpr uint16_t DMA1_STATUS   = 0x08;
    constexpr uint16_t DMA1_COMMAND  = 0x08;
    constexpr uint16_t DMA1_REQUEST  = 0x09;
    constexpr uint16_t DMA1_MASK     = 0x0A;
    constexpr uint16_t DMA1_MODE     = 0x0B;
    constexpr uint16_t DMA1_CLEAR_FF = 0x0C;
    constexpr uint16_t DMA1_RESET    = 0x0D;

    constexpr uint16_t DMA2_STATUS   = 0xD0;
    constexpr uint16_t DMA2_COMMAND  = 0xD0;
    constexpr uint16_t DMA2_REQUEST  = 0xD2;
    constexpr uint16_t DMA2_MASK     = 0xD4;
    constexpr uint16_t DMA2_MODE     = 0xD6;
    constexpr uint16_t DMA2_CLEAR_FF = 0xD8;
    constexpr uint16_t DMA2_RESET    = 0xDA;
}

// ============================================================================
// اتجاه النقل / Transfer Direction
// ============================================================================

enum class DMADirection {
    DEVICE_TO_MEMORY,   ///< من الجهاز إلى الذاكرة / Device to memory
    MEMORY_TO_DEVICE,   ///< من الذاكرة إلى الجهاز / Memory to device
    BIDIRECTIONAL       ///< ثنائي الاتجاه / Bidirectional
};

// ============================================================================
// وضع النقل / Transfer Mode
// ============================================================================

enum class DMAMode {
    DEMAND,     ///< وضع الطلب / Demand mode
    SINGLE,     ///< نقل فردي / Single transfer
    BLOCK,      ///< نقل كتلة / Block transfer
    CASCADE     ///< تتالي / Cascade
};

// ============================================================================
// حالة النقل / Transfer Status
// ============================================================================

enum class DMAStatus {
    IDLE,           ///< خامل / Idle
    IN_PROGRESS,    ///< جاري النقل / Transfer in progress
    COMPLETED,      ///< مكتمل / Completed
    ERROR,          ///< خطأ / Error
    TIMEOUT         ///< انتهاء المهلة / Timeout
};

// ============================================================================
// إدخال Scatter-Gather / Scatter-Gather Entry
// ============================================================================

#pragma pack(push, 1)
struct SGEntry {
    uint64_t physicalAddress;   ///< العنوان المادي / Physical address
    uint32_t length;            ///< الطول بالبايتات / Length in bytes
    uint32_t flags;             ///< أعلام (آخر إدخال، مقاطعة، ...) / Flags

    SGEntry() : physicalAddress(0), length(0), flags(0) {}
    SGEntry(uint64_t addr, uint32_t len, uint32_t f = 0)
        : physicalAddress(addr), length(len), flags(f) {}
};
#pragma pack(pop)

namespace SGFlags {
    constexpr uint32_t LAST_ENTRY    = 1 << 0;  ///< آخر إدخال في القائمة / Last entry in list
    constexpr uint32_t INTERRUPT     = 1 << 1;  ///< توليد مقاطعة عند الانتهاء / Generate interrupt on completion
    constexpr uint32_t LINK          = 1 << 2;  ///< ربط بقائمة أخرى / Link to another list
}

// ============================================================================
// واصف النقل / Transfer Descriptor
// ============================================================================

struct DMATransfer {
    uint64_t sourceAddress;      ///< عنوان المصدر / Source address
    uint64_t destAddress;        ///< عنوان الوجهة / Destination address
    uint64_t size;               ///< حجم النقل / Transfer size
    DMADirection direction;      ///< اتجاه النقل / Direction
    DMAMode mode;                ///< وضع النقل / Mode
    DMAStatus status;            ///< حالة النقل / Status
    uint32_t transferId;         ///< معرف النقل / Transfer ID
    bool useScatterGather;       ///< استخدام Scatter-Gather / Use Scatter-Gather
    std::vector<SGEntry> sgList; ///< قائمة Scatter-Gather / Scatter-Gather list
    std::function<void(DMAStatus)> callback; ///< دالة الاستدعاء / Callback function

    DMATransfer()
        : sourceAddress(0), destAddress(0), size(0)
        , direction(DMADirection::DEVICE_TO_MEMORY)
        , mode(DMAMode::SINGLE), status(DMAStatus::IDLE)
        , transferId(0), useScatterGather(false) {}
};

// ============================================================================
// مدير DMA / DMA Manager
// ============================================================================

class DMAManager {
public:
    static DMAManager& getInstance();

    /**
     * تهيئة نظام DMA
     * Initialize DMA system
     */
    void initialize();

    // ====================================================================
    // تخصيص الذاكرة المتوافقة مع DMA / DMA-compatible Memory Allocation
    // ====================================================================

    /**
     * تخصيص ذاكرة متوافقة مع DMA (متجاورة مادياً)
     * Allocate DMA-compatible memory (physically contiguous)
     *
     * @param size الحجم بالبايتات / Size in bytes
     * @param alignment المحاذاة المطلوبة / Required alignment
     * @return العنوان المادي أو 0 عند الفشل / Physical address or 0 on failure
     */
    uint64_t allocateBuffer(uint64_t size, uint64_t alignment = 4096);

    /**
     * تحرير ذاكرة DMA
     * Free DMA memory
     *
     * @param physicalAddress العنوان المادي / Physical address
     * @param size الحجم / Size
     */
    void freeBuffer(uint64_t physicalAddress, uint64_t size);

    // ====================================================================
    // عمليات النقل / Transfer Operations
    // ====================================================================

    /**
     * بدء نقل DMA بسيط
     * Start simple DMA transfer
     *
     * @param source عنوان المصدر / Source address
     * @param dest عنوان الوجهة / Destination address
     * @param size الحجم / Size
     * @param direction الاتجاه / Direction
     * @return معرف النقل أو 0 عند الفشل / Transfer ID or 0 on failure
     */
    uint32_t startTransfer(uint64_t source, uint64_t dest, uint64_t size,
                           DMADirection direction);

    /**
     * بدء نقل Scatter-Gather DMA
     * Start Scatter-Gather DMA transfer
     *
     * @param sgList قائمة SG / SG list
     * @param dest عنوان الوجهة / Destination address
     * @param direction الاتجاه / Direction
     * @return معرف النقل / Transfer ID
     */
    uint32_t startScatterGatherTransfer(const std::vector<SGEntry>& sgList,
                                         uint64_t dest,
                                         DMADirection direction);

    /**
     * انتظار اكتمال نقل
     * Wait for transfer completion
     *
     * @param transferId معرف النقل / Transfer ID
     * @param timeoutMs المهلة بالمللي ثانية (0 = لا نهائي) / Timeout in ms (0 = infinite)
     * @return حالة النقل / Transfer status
     */
    DMAStatus waitForTransfer(uint32_t transferId, uint32_t timeoutMs = 0);

    /**
     * إلغاء نقل DMA
     * Cancel DMA transfer
     *
     * @param transferId معرف النقل / Transfer ID
     * @return true عند النجاح / true on success
     */
    bool cancelTransfer(uint32_t transferId);

    /**
     * الحصول على حالة نقل
     * Get transfer status
     */
    DMAStatus getTransferStatus(uint32_t transferId) const;

    /**
     * تسجيل دالة استدعاء لنقل
     * Register callback for transfer
     */
    void setTransferCallback(uint32_t transferId, std::function<void(DMAStatus)> callback);

    // ====================================================================
    // DMA التقليدي (ISA) / Legacy (ISA) DMA
    // ====================================================================

    /**
     * تهيئة قناة DMA تقليدية
     * Initialize legacy DMA channel
     *
     * @param channel رقم القناة (0-7) / Channel number
     * @param address العنوان المادي (أقل من 16MB) / Physical address (below 16MB)
     * @param count العدد / Count
     * @param mode الوضع / Mode
     * @param direction الاتجاه / Direction
     */
    void setupLegacyChannel(uint8_t channel, uint32_t address,
                            uint16_t count, DMAMode mode,
                            DMADirection direction);

    /**
     * تفعيل قناة DMA تقليدية
     * Enable legacy DMA channel
     */
    void enableLegacyChannel(uint8_t channel);

    /**
     * تعطيل قناة DMA تقليدية
     * Disable legacy DMA channel
     */
    void disableLegacyChannel(uint8_t channel);

    // ====================================================================
    // إحصائيات / Statistics
    // ====================================================================

    /**
     * الحصول على عدد عمليات النقل الكلي
     * Get total transfers count
     */
    uint64_t getTotalTransfers() const { return totalTransfers_; }

    /**
     * الحصول على إجمالي البايتات المنقولة
     * Get total bytes transferred
     */
    uint64_t getTotalBytesTransferred() const { return totalBytes_; }

    /**
     * إنشاء تقرير DMA
     * Generate DMA report
     */
    std::string generateReport() const;

private:
    DMAManager();
    ~DMAManager() = default;
    DMAManager(const DMAManager&) = delete;
    DMAManager& operator=(const DMAManager&) = delete;

    uint32_t nextTransferId_;
    std::vector<DMATransfer> activeTransfers_;
    uint64_t totalTransfers_;
    uint64_t totalBytes_;
    bool isInitialized_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_DMA_H
