/*
 * ============================================================================
 * جدول الواصفات العام وقطعة حالة المهمة - GDT & TSS for Sad Language
 * ============================================================================
 *
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يوفر إدارة جدول الواصفات العام (GDT) وقطعة حالة المهمة (TSS).
 * GDT يحدد قطع الذاكرة وامتيازات الوصول في معمارية x86-64.
 * TSS يحتوي على معلومات تبديل المكدس بين حلقات الامتياز.
 *
 * English Description:
 * -------------------
 * This file provides GDT (Global Descriptor Table) and TSS (Task State Segment)
 * management. GDT defines memory segments and access privileges in x86-64.
 * TSS contains stack switching information between privilege rings.
 *
 * المؤلف / Author: Sad Language Team
 * الإصدار / Version: 2.0.0
 * ============================================================================
 */

#ifndef SAD_GDT_H
#define SAD_GDT_H

#include <cstdint>
#include <string>

namespace Sad {
namespace LowLevel {

// ============================================================================
// ثوابت GDT / GDT Constants
// ============================================================================

namespace GDTConstants {
    constexpr int MAX_GDT_ENTRIES = 7;

    // محددات القطع / Segment Selectors
    constexpr uint16_t NULL_SELECTOR      = 0x00;  ///< القطعة الفارغة / Null segment
    constexpr uint16_t KERNEL_CODE_SEL    = 0x08;  ///< كود النواة / Kernel code (Ring 0)
    constexpr uint16_t KERNEL_DATA_SEL    = 0x10;  ///< بيانات النواة / Kernel data (Ring 0)
    constexpr uint16_t USER_CODE_SEL      = 0x18;  ///< كود المستخدم / User code (Ring 3)
    constexpr uint16_t USER_DATA_SEL      = 0x20;  ///< بيانات المستخدم / User data (Ring 3)
    constexpr uint16_t TSS_SELECTOR       = 0x28;  ///< محدد TSS / TSS selector

    // أعلام الوصول / Access Flags
    constexpr uint8_t ACCESS_PRESENT      = 0x80;  ///< القطعة موجودة / Segment present
    constexpr uint8_t ACCESS_RING0        = 0x00;  ///< مستوى امتياز 0 / Privilege level 0
    constexpr uint8_t ACCESS_RING3        = 0x60;  ///< مستوى امتياز 3 / Privilege level 3
    constexpr uint8_t ACCESS_SYSTEM       = 0x00;  ///< قطعة نظام / System segment
    constexpr uint8_t ACCESS_CODE_DATA    = 0x10;  ///< قطعة كود/بيانات / Code/Data segment
    constexpr uint8_t ACCESS_EXECUTABLE   = 0x08;  ///< قابلة للتنفيذ / Executable
    constexpr uint8_t ACCESS_DIRECTION    = 0x04;  ///< الاتجاه (كود: يتوافق) / Direction (code: conforming)
    constexpr uint8_t ACCESS_READ_WRITE   = 0x02;  ///< قراءة/كتابة / Read/Write
    constexpr uint8_t ACCESS_ACCESSED     = 0x01;  ///< تم الوصول / Accessed
    constexpr uint8_t ACCESS_TSS_64       = 0x09;  ///< TSS 64-بت نشط / 64-bit TSS available

    // أعلام GDT / GDT Flags
    constexpr uint8_t FLAG_GRANULARITY    = 0x80;  ///< حبيبية 4KB / 4KB granularity
    constexpr uint8_t FLAG_SIZE_32        = 0x40;  ///< حجم 32-بت / 32-bit size
    constexpr uint8_t FLAG_LONG_MODE      = 0x20;  ///< وضع طويل 64-بت / 64-bit long mode
}

// ============================================================================
// بنية إدخال GDT / GDT Entry Structure
// ============================================================================

#pragma pack(push, 1)

/**
 * إدخال في جدول GDT (8 بايتات)
 * GDT Entry (8 bytes)
 */
struct GDTEntry {
    uint16_t limitLow;      ///< الحد: البتات 0-15 / Limit: bits 0-15
    uint16_t baseLow;       ///< القاعدة: البتات 0-15 / Base: bits 0-15
    uint8_t  baseMiddle;    ///< القاعدة: البتات 16-23 / Base: bits 16-23
    uint8_t  access;        ///< بايت الوصول / Access byte
    uint8_t  flagsLimitHigh;///< الأعلام (4 بتات) + الحد 16-19 / Flags (4 bits) + Limit 16-19
    uint8_t  baseHigh;      ///< القاعدة: البتات 24-31 / Base: bits 24-31

    GDTEntry()
        : limitLow(0), baseLow(0), baseMiddle(0)
        , access(0), flagsLimitHigh(0), baseHigh(0) {}
};

/**
 * إدخال GDT للنظام في الوضع الطويل (16 بايت) - مثل TSS
 * System GDT Entry for long mode (16 bytes) - used for TSS
 */
struct GDTSystemEntry {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t  baseMiddle1;
    uint8_t  access;
    uint8_t  flagsLimitHigh;
    uint8_t  baseMiddle2;
    uint32_t baseHigh;
    uint32_t reserved;

    GDTSystemEntry()
        : limitLow(0), baseLow(0), baseMiddle1(0)
        , access(0), flagsLimitHigh(0), baseMiddle2(0)
        , baseHigh(0), reserved(0) {}
};

/**
 * مؤشر GDT / GDT Pointer
 */
struct GDTPointer {
    uint16_t limit;    ///< حجم GDT - 1 / GDT size - 1
    uint64_t base;     ///< عنوان قاعدة GDT / GDT base address
};

/**
 * قطعة حالة المهمة (TSS) في الوضع الطويل / Task State Segment (Long Mode)
 */
struct TSS {
    uint32_t reserved0;
    uint64_t rsp0;          ///< مكدس الحلقة 0 / Ring 0 stack pointer
    uint64_t rsp1;          ///< مكدس الحلقة 1 / Ring 1 stack pointer
    uint64_t rsp2;          ///< مكدس الحلقة 2 / Ring 2 stack pointer
    uint64_t reserved1;
    uint64_t ist1;          ///< جدول مكدس المقاطعات 1 / Interrupt Stack Table 1
    uint64_t ist2;          ///< جدول مكدس المقاطعات 2 / Interrupt Stack Table 2
    uint64_t ist3;          ///< جدول مكدس المقاطعات 3 / Interrupt Stack Table 3
    uint64_t ist4;          ///< جدول مكدس المقاطعات 4 / Interrupt Stack Table 4
    uint64_t ist5;          ///< جدول مكدس المقاطعات 5 / Interrupt Stack Table 5
    uint64_t ist6;          ///< جدول مكدس المقاطعات 6 / Interrupt Stack Table 6
    uint64_t ist7;          ///< جدول مكدس المقاطعات 7 / Interrupt Stack Table 7
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopbOffset;    ///< إزاحة خريطة منافذ I/O / I/O port bitmap offset

    TSS()
        : reserved0(0)
        , rsp0(0), rsp1(0), rsp2(0)
        , reserved1(0)
        , ist1(0), ist2(0), ist3(0), ist4(0)
        , ist5(0), ist6(0), ist7(0)
        , reserved2(0), reserved3(0)
        , iopbOffset(sizeof(TSS)) {}
};

#pragma pack(pop)

// ============================================================================
// مدير GDT / GDT Manager
// ============================================================================

class GDTManager {
public:
    /**
     * الحصول على المثيل الوحيد
     * Get singleton instance
     */
    static GDTManager& getInstance();

    /**
     * تهيئة GDT مع الإعدادات الافتراضية للوضع الطويل
     * Initialize GDT with default long mode settings
     */
    void initialize();

    /**
     * تعيين إدخال GDT
     * Set GDT entry
     *
     * @param index الفهرس في الجدول / Index in table
     * @param base عنوان القاعدة / Base address
     * @param limit الحد / Limit
     * @param access بايت الوصول / Access byte
     * @param flags الأعلام / Flags
     */
    void setEntry(int index, uint32_t base, uint32_t limit,
                  uint8_t access, uint8_t flags);

    /**
     * تعيين إدخال TSS في GDT
     * Set TSS entry in GDT
     *
     * @param index الفهرس في الجدول / Index in table
     * @param tssAddress عنوان TSS / TSS address
     * @param tssSize حجم TSS / TSS size
     */
    void setTSSEntry(int index, uint64_t tssAddress, uint32_t tssSize);

    /**
     * تحميل GDT إلى المعالج
     * Load GDT into CPU
     */
    void load();

    /**
     * تحميل TSS إلى المعالج
     * Load TSS into CPU
     *
     * @param selector محدد TSS في GDT / TSS selector in GDT
     */
    void loadTSS(uint16_t selector);

    /**
     * تهيئة TSS
     * Initialize TSS
     *
     * @param kernelStackTop قمة مكدس النواة / Kernel stack top
     */
    void initializeTSS(uint64_t kernelStackTop);

    /**
     * تحديث مكدس TSS للحلقة 0
     * Update TSS Ring 0 stack
     *
     * @param stackTop قمة المكدس الجديدة / New stack top
     */
    void updateTSSStack(uint64_t stackTop);

    /**
     * تعيين مكدس المقاطعات (IST)
     * Set Interrupt Stack Table entry
     *
     * @param istIndex رقم IST (1-7) / IST number (1-7)
     * @param stackTop قمة المكدس / Stack top
     */
    void setIST(int istIndex, uint64_t stackTop);

    /**
     * الحصول على إدخال GDT
     * Get GDT entry
     */
    const GDTEntry* getEntry(int index) const;

    /**
     * الحصول على مؤشر TSS
     * Get TSS pointer
     */
    TSS* getTSS() { return &tss_; }

    /**
     * إنشاء تقرير GDT
     * Generate GDT report
     */
    std::string generateReport() const;

private:
    GDTManager();
    ~GDTManager() = default;
    GDTManager(const GDTManager&) = delete;
    GDTManager& operator=(const GDTManager&) = delete;

    GDTEntry entries_[GDTConstants::MAX_GDT_ENTRIES];
    GDTPointer gdtPointer_;
    TSS tss_;
    bool isInitialized_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_GDT_H
