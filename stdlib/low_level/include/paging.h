/*
 * ============================================================================
 * نظام الترحيل للغة ص - Paging System for Sad Language
 * ============================================================================
 *
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يوفر نظام الترحيل (الذاكرة الافتراضية) لمعمارية x86-64.
 * يدعم 4 مستويات من جداول الصفحات (PML4 → PDPT → PD → PT)
 * مع صفحات بأحجام 4KB و 2MB و 1GB.
 * ضروري لإدارة الذاكرة الافتراضية وحماية الذاكرة وعزل العمليات.
 *
 * English Description:
 * -------------------
 * This file provides the paging (virtual memory) system for x86-64.
 * Supports 4-level page tables (PML4 → PDPT → PD → PT)
 * with 4KB, 2MB, and 1GB page sizes.
 * Essential for virtual memory management, memory protection, and process isolation.
 *
 * المؤلف / Author: Sad Language Team
 * الإصدار / Version: 2.0.0
 * ============================================================================
 */

#ifndef SAD_PAGING_H
#define SAD_PAGING_H

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace Sad {
namespace LowLevel {

// ============================================================================
// ثوابت الترحيل / Paging Constants
// ============================================================================

namespace PagingConstants {
    constexpr uint64_t PAGE_SIZE_4K  = 4096ULL;
    constexpr uint64_t PAGE_SIZE_2M  = 2 * 1024 * 1024ULL;
    constexpr uint64_t PAGE_SIZE_1G  = 1024 * 1024 * 1024ULL;

    constexpr int ENTRIES_PER_TABLE = 512;
    constexpr int PML4_SHIFT   = 39;
    constexpr int PDPT_SHIFT   = 30;
    constexpr int PD_SHIFT     = 21;
    constexpr int PT_SHIFT     = 12;
    constexpr uint64_t INDEX_MASK = 0x1FF;

    constexpr uint64_t PHYS_ADDR_MASK = 0x000FFFFFFFFFF000ULL;
    constexpr uint64_t CANONICAL_HIGH = 0xFFFF800000000000ULL;
    constexpr uint64_t CANONICAL_LOW  = 0x00007FFFFFFFFFFFULL;
}

// ============================================================================
// أعلام إدخالات جدول الصفحات / Page Table Entry Flags
// ============================================================================

namespace PageFlags {
    constexpr uint64_t PRESENT       = 1ULL << 0;   ///< الصفحة موجودة / Page present
    constexpr uint64_t WRITABLE      = 1ULL << 1;   ///< قابلة للكتابة / Writable
    constexpr uint64_t USER          = 1ULL << 2;   ///< وصول المستخدم / User accessible
    constexpr uint64_t WRITE_THROUGH = 1ULL << 3;   ///< كتابة مباشرة / Write-through caching
    constexpr uint64_t CACHE_DISABLE = 1ULL << 4;   ///< تعطيل الذاكرة المخبئية / Cache disable
    constexpr uint64_t ACCESSED      = 1ULL << 5;   ///< تم الوصول / Accessed
    constexpr uint64_t DIRTY         = 1ULL << 6;   ///< معدلة / Dirty
    constexpr uint64_t HUGE_PAGE     = 1ULL << 7;   ///< صفحة كبيرة (2MB/1GB) / Huge page
    constexpr uint64_t GLOBAL        = 1ULL << 8;   ///< صفحة عامة / Global page
    constexpr uint64_t NO_EXECUTE    = 1ULL << 63;  ///< عدم التنفيذ / No execute (NX)

    // مجموعات شائعة / Common combinations
    constexpr uint64_t KERNEL_PAGE   = PRESENT | WRITABLE;
    constexpr uint64_t KERNEL_CODE   = PRESENT | NO_EXECUTE;
    constexpr uint64_t KERNEL_RODATA = PRESENT | NO_EXECUTE;
    constexpr uint64_t USER_PAGE     = PRESENT | WRITABLE | USER;
    constexpr uint64_t USER_CODE     = PRESENT | USER;
    constexpr uint64_t MMIO_PAGE     = PRESENT | WRITABLE | CACHE_DISABLE | WRITE_THROUGH | NO_EXECUTE;
}

// ============================================================================
// حجم الصفحة / Page Size Enum
// ============================================================================

enum class PageSize {
    SIZE_4K,    ///< 4 كيلوبايت / 4 Kilobytes
    SIZE_2M,    ///< 2 ميغابايت / 2 Megabytes
    SIZE_1G     ///< 1 غيغابايت / 1 Gigabyte
};

// ============================================================================
// بنية إدخال جدول الصفحات / Page Table Entry
// ============================================================================

struct PageTableEntry {
    uint64_t value;

    PageTableEntry() : value(0) {}
    explicit PageTableEntry(uint64_t v) : value(v) {}

    bool isPresent() const      { return value & PageFlags::PRESENT; }
    bool isWritable() const     { return value & PageFlags::WRITABLE; }
    bool isUserAccessible() const { return value & PageFlags::USER; }
    bool isHugePage() const     { return value & PageFlags::HUGE_PAGE; }
    bool isGlobal() const       { return value & PageFlags::GLOBAL; }
    bool isAccessed() const     { return value & PageFlags::ACCESSED; }
    bool isDirty() const        { return value & PageFlags::DIRTY; }
    bool isNoExecute() const    { return value & PageFlags::NO_EXECUTE; }

    uint64_t getPhysicalAddress() const {
        return value & PagingConstants::PHYS_ADDR_MASK;
    }

    void setPhysicalAddress(uint64_t addr) {
        value = (value & ~PagingConstants::PHYS_ADDR_MASK) | (addr & PagingConstants::PHYS_ADDR_MASK);
    }

    void setFlags(uint64_t flags) {
        value |= flags;
    }

    void clearFlags(uint64_t flags) {
        value &= ~flags;
    }

    void clear() { value = 0; }
};

// ============================================================================
// جدول الصفحات / Page Table
// ============================================================================

struct alignas(4096) PageTable {
    PageTableEntry entries[PagingConstants::ENTRIES_PER_TABLE];

    void clear() {
        for (int i = 0; i < PagingConstants::ENTRIES_PER_TABLE; ++i) {
            entries[i].clear();
        }
    }
};

// ============================================================================
// معلومات التحويل / Translation Info
// ============================================================================

struct TranslationResult {
    bool valid;                  ///< هل التحويل ناجح؟ / Is translation valid?
    uint64_t physicalAddress;    ///< العنوان المادي / Physical address
    uint64_t pageBase;           ///< قاعدة الصفحة / Page base
    PageSize pageSize;           ///< حجم الصفحة / Page size
    uint64_t flags;              ///< أعلام الصفحة / Page flags
    int level;                   ///< مستوى الجدول / Table level where mapping was found

    TranslationResult()
        : valid(false), physicalAddress(0), pageBase(0)
        , pageSize(PageSize::SIZE_4K), flags(0), level(0) {}
};

// ============================================================================
// مخصص إطارات الصفحات / Page Frame Allocator
// ============================================================================

class PageFrameAllocator {
public:
    static PageFrameAllocator& getInstance();

    /**
     * تهيئة المخصص بنطاق ذاكرة مادية
     * Initialize allocator with physical memory range
     *
     * @param startAddress عنوان البداية (محاذاة 4KB) / Start address (4KB aligned)
     * @param totalPages إجمالي الصفحات المتاحة / Total available pages
     */
    void initialize(uint64_t startAddress, uint64_t totalPages);

    /**
     * تخصيص إطار صفحة واحد (4KB)
     * Allocate single page frame (4KB)
     *
     * @return العنوان المادي أو 0 عند الفشل / Physical address or 0 on failure
     */
    uint64_t allocateFrame();

    /**
     * تخصيص عدة إطارات متتالية
     * Allocate contiguous frames
     *
     * @param count عدد الإطارات / Number of frames
     * @return العنوان المادي للأول أو 0 عند الفشل / Physical address of first or 0 on failure
     */
    uint64_t allocateContiguousFrames(uint64_t count);

    /**
     * تحرير إطار صفحة
     * Free page frame
     *
     * @param physicalAddress العنوان المادي / Physical address
     */
    void freeFrame(uint64_t physicalAddress);

    /**
     * تحرير عدة إطارات متتالية
     * Free contiguous frames
     */
    void freeContiguousFrames(uint64_t physicalAddress, uint64_t count);

    /**
     * الحصول على عدد الإطارات الحرة
     * Get free frame count
     */
    uint64_t getFreeFrameCount() const { return freeFrames_; }

    /**
     * الحصول على إجمالي الإطارات
     * Get total frame count
     */
    uint64_t getTotalFrameCount() const { return totalFrames_; }

    /**
     * الحصول على الإطارات المستخدمة
     * Get used frame count
     */
    uint64_t getUsedFrameCount() const { return totalFrames_ - freeFrames_; }

    /**
     * حجز نطاق ذاكرة (منع التخصيص فيه)
     * Reserve memory range (prevent allocation)
     */
    void reserveRange(uint64_t startAddress, uint64_t sizeInBytes);

    /**
     * إلغاء حجز نطاق ذاكرة
     * Unreserve memory range
     */
    void unreserveRange(uint64_t startAddress, uint64_t sizeInBytes);

private:
    PageFrameAllocator();
    ~PageFrameAllocator() = default;
    PageFrameAllocator(const PageFrameAllocator&) = delete;
    PageFrameAllocator& operator=(const PageFrameAllocator&) = delete;

    void setBit(uint64_t frameIndex);
    void clearBit(uint64_t frameIndex);
    bool testBit(uint64_t frameIndex) const;
    uint64_t findFirstFree() const;
    uint64_t findContiguousFree(uint64_t count) const;

    uint64_t* bitmap_;
    uint64_t bitmapSize_;
    uint64_t totalFrames_;
    uint64_t freeFrames_;
    uint64_t startAddress_;
    bool isInitialized_;
};

// ============================================================================
// مدير الترحيل / Paging Manager
// ============================================================================

class PagingManager {
public:
    static PagingManager& getInstance();

    /**
     * تهيئة نظام الترحيل
     * Initialize paging system
     */
    void initialize();

    /**
     * تحويل عنوان افتراضي إلى مادي
     * Translate virtual to physical address
     *
     * @param virtualAddress العنوان الافتراضي / Virtual address
     * @return نتيجة التحويل / Translation result
     */
    TranslationResult translate(uint64_t virtualAddress) const;

    /**
     * تعيين صفحة (ربط عنوان افتراضي بمادي)
     * Map a page (bind virtual to physical address)
     *
     * @param virtualAddress العنوان الافتراضي / Virtual address
     * @param physicalAddress العنوان المادي / Physical address
     * @param flags أعلام الصفحة / Page flags
     * @param pageSize حجم الصفحة / Page size
     * @return true عند النجاح / true on success
     */
    bool mapPage(uint64_t virtualAddress, uint64_t physicalAddress,
                 uint64_t flags, PageSize pageSize = PageSize::SIZE_4K);

    /**
     * إلغاء تعيين صفحة
     * Unmap a page
     *
     * @param virtualAddress العنوان الافتراضي / Virtual address
     * @return true عند النجاح / true on success
     */
    bool unmapPage(uint64_t virtualAddress);

    /**
     * تعيين نطاق من الصفحات
     * Map a range of pages
     *
     * @param virtualStart بداية النطاق الافتراضي / Virtual range start
     * @param physicalStart بداية النطاق المادي / Physical range start
     * @param size الحجم بالبايتات / Size in bytes
     * @param flags أعلام الصفحة / Page flags
     * @param pageSize حجم الصفحة / Page size
     * @return true عند النجاح / true on success
     */
    bool mapRange(uint64_t virtualStart, uint64_t physicalStart,
                  uint64_t size, uint64_t flags,
                  PageSize pageSize = PageSize::SIZE_4K);

    /**
     * إلغاء تعيين نطاق من الصفحات
     * Unmap a range of pages
     */
    bool unmapRange(uint64_t virtualStart, uint64_t size);

    /**
     * تعيين MMIO (ذاكرة الأجهزة)
     * Map MMIO (device memory)
     *
     * @param physicalAddress العنوان المادي للجهاز / Device physical address
     * @param size الحجم / Size
     * @return العنوان الافتراضي أو 0 عند الفشل / Virtual address or 0 on failure
     */
    uint64_t mapMMIO(uint64_t physicalAddress, uint64_t size);

    /**
     * إلغاء تعيين MMIO
     * Unmap MMIO
     */
    void unmapMMIO(uint64_t virtualAddress, uint64_t size);

    /**
     * تغيير أعلام صفحة
     * Change page flags
     *
     * @param virtualAddress العنوان الافتراضي / Virtual address
     * @param flags الأعلام الجديدة / New flags
     * @return true عند النجاح / true on success
     */
    bool changeFlags(uint64_t virtualAddress, uint64_t flags);

    /**
     * إنشاء فضاء عناوين جديد (للعمليات)
     * Create new address space (for processes)
     *
     * @return العنوان المادي لجدول PML4 الجديد / Physical address of new PML4 table
     */
    uint64_t createAddressSpace();

    /**
     * تبديل فضاء العناوين
     * Switch address space
     *
     * @param pml4PhysAddr العنوان المادي لجدول PML4 / PML4 physical address
     */
    void switchAddressSpace(uint64_t pml4PhysAddr);

    /**
     * تدمير فضاء عناوين
     * Destroy address space
     *
     * @param pml4PhysAddr العنوان المادي لجدول PML4 / PML4 physical address
     */
    void destroyAddressSpace(uint64_t pml4PhysAddr);

    /**
     * الحصول على العنوان المادي لجدول PML4 الحالي
     * Get current PML4 table physical address
     */
    uint64_t getCurrentPML4() const { return currentPML4_; }

    /**
     * إبطال إدخال TLB لعنوان محدد
     * Invalidate TLB entry for specific address
     */
    void invalidatePage(uint64_t virtualAddress);

    /**
     * إبطال جميع إدخالات TLB
     * Flush entire TLB
     */
    void flushTLB();

    /**
     * إنشاء تقرير الترحيل
     * Generate paging report
     */
    std::string generateReport() const;

private:
    PagingManager();
    ~PagingManager() = default;
    PagingManager(const PagingManager&) = delete;
    PagingManager& operator=(const PagingManager&) = delete;

    PageTable* getOrCreateTable(PageTable* parent, int index, uint64_t flags);
    void freeTable(PageTable* table, int level);

    static int pml4Index(uint64_t addr)  { return (addr >> PagingConstants::PML4_SHIFT) & PagingConstants::INDEX_MASK; }
    static int pdptIndex(uint64_t addr)  { return (addr >> PagingConstants::PDPT_SHIFT) & PagingConstants::INDEX_MASK; }
    static int pdIndex(uint64_t addr)    { return (addr >> PagingConstants::PD_SHIFT)   & PagingConstants::INDEX_MASK; }
    static int ptIndex(uint64_t addr)    { return (addr >> PagingConstants::PT_SHIFT)   & PagingConstants::INDEX_MASK; }

    static uint64_t alignDown(uint64_t addr, uint64_t alignment) {
        return addr & ~(alignment - 1);
    }
    static uint64_t alignUp(uint64_t addr, uint64_t alignment) {
        return (addr + alignment - 1) & ~(alignment - 1);
    }

    uint64_t currentPML4_;
    uint64_t nextMmioVirtual_;
    uint64_t mappedPageCount_;
    bool isInitialized_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_PAGING_H
