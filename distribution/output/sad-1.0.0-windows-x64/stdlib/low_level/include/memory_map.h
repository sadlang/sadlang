/*
 * ============================================================================
 * إدارة الذاكرة للغة ص - Memory Management for Sad Language
 * ============================================================================
 * 
 * الوصف بالعربية:
 * ---------------
 * هذا الملف يحتوي على نظام إدارة الذاكرة الديناميكية.
 * يوفر وظائف malloc, free, mmap, munmap لتخصيص وإدارة الذاكرة.
 * ضروري لبرمجة أنظمة التشغيل والتطبيقات منخفضة المستوى.
 * 
 * المميزات:
 * - تخصيص ديناميكي للذاكرة (malloc/free)
 * - تعيين الذاكرة (mmap/munmap)
 * - إدارة المناطق (regions)
 * - الحماية والأمان (protection & safety)
 * - تتبع التسريبات (leak detection)
 * 
 * English Description:
 * -------------------
 * This file contains the dynamic memory management system.
 * Provides malloc, free, mmap, munmap functions for memory allocation
 * and management. Essential for OS programming and low-level applications.
 * 
 * Features:
 * - Dynamic memory allocation (malloc/free)
 * - Memory mapping (mmap/munmap)
 * - Region management
 * - Protection & safety
 * - Leak detection
 * 
 * المؤلف / Author: Sad Language Team
 * التاريخ / Date: December 2025
 * الإصدار / Version: 1.0.0
 * ============================================================================
 */

#ifndef SAD_MEMORY_MAP_H
#define SAD_MEMORY_MAP_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Sad {
namespace LowLevel {

// ============================================================================
// ثوابت الذاكرة / Memory Constants
// ============================================================================

namespace MemoryConstants {
    // أحجام الصفحات / Page sizes
    constexpr size_t PAGE_SIZE_4KB = 4096;
    constexpr size_t PAGE_SIZE_2MB = 2 * 1024 * 1024;
    constexpr size_t PAGE_SIZE_1GB = 1024 * 1024 * 1024;
    
    // محاذاة / Alignment
    constexpr size_t ALIGNMENT_8 = 8;
    constexpr size_t ALIGNMENT_16 = 16;
    constexpr size_t ALIGNMENT_32 = 32;
    constexpr size_t ALIGNMENT_64 = 64;
    
    // حدود الذاكرة / Memory limits
    constexpr size_t MAX_ALLOCATION = 1ULL << 40;  // 1 TB
    constexpr size_t MIN_ALLOCATION = 1;
    
    // عناوين خاصة / Special addresses
    constexpr uint64_t NULL_ADDRESS = 0;
    constexpr uint64_t INVALID_ADDRESS = 0xFFFFFFFFFFFFFFFFULL;
}

// ============================================================================
// حماية الذاكرة / Memory Protection
// ============================================================================

/**
 * أعلام حماية الذاكرة
 * Memory protection flags
 */
enum class MemoryProtection : uint32_t {
    NONE = 0,           ///< لا حماية / No protection
    READ = 1 << 0,      ///< قراءة / Read
    WRITE = 1 << 1,     ///< كتابة / Write
    EXECUTE = 1 << 2,   ///< تنفيذ / Execute
    
    // مجموعات شائعة / Common combinations
    READ_WRITE = READ | WRITE,
    READ_EXECUTE = READ | EXECUTE,
    READ_WRITE_EXECUTE = READ | WRITE | EXECUTE
};

// عامل OR للأعلام / OR operator for flags
inline MemoryProtection operator|(MemoryProtection a, MemoryProtection b) {
    return static_cast<MemoryProtection>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

inline MemoryProtection operator&(MemoryProtection a, MemoryProtection b) {
    return static_cast<MemoryProtection>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
    );
}

inline bool hasProtection(MemoryProtection flags, MemoryProtection check) {
    return (flags & check) == check;
}

// ============================================================================
// أعلام التعيين / Mapping Flags
// ============================================================================

/**
 * أعلام لوظيفة mmap
 * Flags for mmap function
 */
enum class MapFlags : uint32_t {
    SHARED = 1 << 0,       ///< تعيين مشترك / Shared mapping
    PRIVATE = 1 << 1,      ///< تعيين خاص / Private mapping
    FIXED = 1 << 2,        ///< عنوان ثابت / Fixed address
    ANONYMOUS = 1 << 3,    ///< بدون ملف / Anonymous (no file)
    POPULATE = 1 << 4,     ///< ملء مسبق / Pre-populate
    LOCKED = 1 << 5        ///< مقفل في الذاكرة / Locked in memory
};

inline MapFlags operator|(MapFlags a, MapFlags b) {
    return static_cast<MapFlags>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

// ============================================================================
// حالة المنطقة / Region State
// ============================================================================

/**
 * حالة منطقة الذاكرة
 * Memory region state
 */
enum class RegionState {
    FREE,       ///< حرة / Free
    ALLOCATED,  ///< مخصصة / Allocated
    MAPPED,     ///< معينة / Mapped
    RESERVED    ///< محجوزة / Reserved
};

// ============================================================================
// معلومات المنطقة / Region Information
// ============================================================================

/**
 * معلومات عن منطقة ذاكرة
 * Information about a memory region
 */
struct MemoryRegion {
    uint64_t startAddress;           ///< العنوان الابتدائي / Start address
    uint64_t endAddress;             ///< العنوان النهائي / End address
    size_t size;                     ///< الحجم / Size
    RegionState state;               ///< الحالة / State
    MemoryProtection protection;     ///< الحماية / Protection
    std::string name;                ///< الاسم / Name
    bool isLocked;                   ///< هل مقفلة؟ / Is locked?
    
    // المنشئ الافتراضي / Default constructor
    MemoryRegion()
        : startAddress(0)
        , endAddress(0)
        , size(0)
        , state(RegionState::FREE)
        , protection(MemoryProtection::NONE)
        , name("")
        , isLocked(false) {}
    
    // المنشئ الكامل / Full constructor
    MemoryRegion(uint64_t start, size_t sz, RegionState st, 
                 MemoryProtection prot, const std::string& nm = "")
        : startAddress(start)
        , endAddress(start + sz)
        , size(sz)
        , state(st)
        , protection(prot)
        , name(nm)
        , isLocked(false) {}
    
    /**
     * فحص إذا كانت المنطقة تحتوي على عنوان معين
     * Check if region contains a specific address
     */
    bool contains(uint64_t address) const {
        return address >= startAddress && address < endAddress;
    }
    
    /**
     * فحص إذا كانت المنطقة تتداخل مع منطقة أخرى
     * Check if region overlaps with another region
     */
    bool overlaps(const MemoryRegion& other) const {
        return !(endAddress <= other.startAddress || 
                 startAddress >= other.endAddress);
    }
};

// ============================================================================
// كتلة الذاكرة / Memory Block
// ============================================================================

/**
 * معلومات عن كتلة ذاكرة مخصصة
 * Information about an allocated memory block
 */
struct MemoryBlock {
    uint64_t address;        ///< العنوان / Address
    size_t size;             ///< الحجم / Size
    size_t alignment;        ///< المحاذاة / Alignment
    bool isAligned;          ///< هل محاذاة؟ / Is aligned?
    uint64_t allocationId;   ///< معرف التخصيص / Allocation ID
    
    // المنشئ الافتراضي / Default constructor
    MemoryBlock()
        : address(0)
        , size(0)
        , alignment(0)
        , isAligned(false)
        , allocationId(0) {}
    
    // المنشئ الكامل / Full constructor
    MemoryBlock(uint64_t addr, size_t sz, size_t align = 0)
        : address(addr)
        , size(sz)
        , alignment(align)
        , isAligned(align > 0)
        , allocationId(0) {}
};

// ============================================================================
// مدير الذاكرة / Memory Manager
// ============================================================================

/**
 * مدير الذاكرة - يدير التخصيص والتحرير والتعيين
 * Memory Manager - manages allocation, deallocation, and mapping
 */
class MemoryManager {
public:
    /**
     * الحصول على المثيل الوحيد (Singleton)
     * Get singleton instance
     */
    static MemoryManager& getInstance();
    
    // ========================================================================
    // دوال التخصيص الأساسية / Basic Allocation Functions
    // ========================================================================
    
    /**
     * تخصيص ذاكرة ديناميكية (مثل malloc)
     * Allocate dynamic memory (like malloc)
     * 
     * @param size الحجم بالبايتات / Size in bytes
     * @return عنوان الذاكرة المخصصة أو 0 عند الفشل / Address or 0 on failure
     */
    uint64_t malloc(size_t size);
    
    /**
     * تخصيص ذاكرة مع المحاذاة (aligned malloc)
     * Allocate aligned memory
     * 
     * @param size الحجم بالبايتات / Size in bytes
     * @param alignment المحاذاة المطلوبة / Required alignment
     * @return عنوان الذاكرة المخصصة أو 0 عند الفشل / Address or 0 on failure
     */
    uint64_t alignedMalloc(size_t size, size_t alignment);
    
    /**
     * تخصيص ذاكرة مع تهيئة بأصفار (مثل calloc)
     * Allocate and zero-initialize memory (like calloc)
     * 
     * @param count عدد العناصر / Number of elements
     * @param size حجم كل عنصر / Size of each element
     * @return عنوان الذاكرة المخصصة أو 0 عند الفشل / Address or 0 on failure
     */
    uint64_t calloc(size_t count, size_t size);
    
    /**
     * إعادة تخصيص ذاكرة (مثل realloc)
     * Reallocate memory (like realloc)
     * 
     * @param address العنوان الحالي / Current address
     * @param newSize الحجم الجديد / New size
     * @return العنوان الجديد أو 0 عند الفشل / New address or 0 on failure
     */
    uint64_t realloc(uint64_t address, size_t newSize);
    
    /**
     * تحرير ذاكرة مخصصة (مثل free)
     * Free allocated memory (like free)
     * 
     * @param address عنوان الذاكرة / Memory address
     * @return true إذا نجح التحرير / true if successful
     */
    bool free(uint64_t address);
    
    // ========================================================================
    // دوال التعيين (mmap/munmap) / Mapping Functions
    // ========================================================================
    
    /**
     * تعيين منطقة ذاكرة (مثل mmap)
     * Map memory region (like mmap)
     * 
     * @param address العنوان المفضل (0 للاختيار التلقائي) / Preferred address (0 for auto)
     * @param size الحجم / Size
     * @param protection الحماية / Protection flags
     * @param flags أعلام التعيين / Mapping flags
     * @param name اسم اختياري / Optional name
     * @return عنوان المنطقة المعينة أو 0 عند الفشل / Mapped address or 0 on failure
     */
    uint64_t mmap(uint64_t address, size_t size, MemoryProtection protection,
                  MapFlags flags, const std::string& name = "");
    
    /**
     * إلغاء تعيين منطقة ذاكرة (مثل munmap)
     * Unmap memory region (like munmap)
     * 
     * @param address عنوان المنطقة / Region address
     * @param size الحجم / Size
     * @return true إذا نجح الإلغاء / true if successful
     */
    bool munmap(uint64_t address, size_t size);
    
    /**
     * تغيير حماية منطقة ذاكرة (مثل mprotect)
     * Change memory region protection (like mprotect)
     * 
     * @param address عنوان المنطقة / Region address
     * @param size الحجم / Size
     * @param protection الحماية الجديدة / New protection
     * @return true إذا نجح التغيير / true if successful
     */
    bool mprotect(uint64_t address, size_t size, MemoryProtection protection);
    
    // ========================================================================
    // دوال الاستعلام / Query Functions
    // ========================================================================
    
    /**
     * الحصول على معلومات كتلة ذاكرة
     * Get memory block information
     */
    const MemoryBlock* getBlockInfo(uint64_t address);
    
    /**
     * الحصول على معلومات منطقة ذاكرة
     * Get memory region information
     */
    const MemoryRegion* getRegionInfo(uint64_t address);
    
    /**
     * فحص صحة عنوان ذاكرة
     * Validate memory address
     */
    bool isValidAddress(uint64_t address);
    
    /**
     * فحص إذا كان العنوان مخصصاً
     * Check if address is allocated
     */
    bool isAllocated(uint64_t address);
    
    /**
     * الحصول على حجم كتلة مخصصة
     * Get size of allocated block
     */
    size_t getBlockSize(uint64_t address);
    
    // ========================================================================
    // دوال الإحصائيات / Statistics Functions
    // ========================================================================
    
    /**
     * الحصول على إجمالي الذاكرة المخصصة
     * Get total allocated memory
     */
    size_t getTotalAllocated() const { return totalAllocated_; }
    
    /**
     * الحصول على عدد الكتل المخصصة
     * Get number of allocated blocks
     */
    size_t getAllocatedBlockCount() const { return allocatedBlocks_.size(); }
    
    /**
     * الحصول على عدد المناطق
     * Get number of regions
     */
    size_t getRegionCount() const { return regions_.size(); }
    
    /**
     * الحصول على ذروة استخدام الذاكرة
     * Get peak memory usage
     */
    size_t getPeakUsage() const { return peakUsage_; }
    
    /**
     * كشف التسريبات
     * Detect memory leaks
     */
    std::vector<MemoryBlock> detectLeaks() const;
    
    /**
     * طباعة تقرير الذاكرة
     * Print memory report
     */
    std::string getMemoryReport() const;
    
    // ========================================================================
    // دوال الصيانة / Maintenance Functions
    // ========================================================================
    
    /**
     * مسح جميع التخصيصات (للاختبار)
     * Clear all allocations (for testing)
     */
    void clear();
    
    /**
     * ضغط الذاكرة (دمج الكتل الحرة)
     * Compact memory (merge free blocks)
     */
    void compact();
    
private:
    // المنشئ الخاص (Singleton)
    // Private constructor (Singleton)
    MemoryManager();
    ~MemoryManager() = default;
    
    // منع النسخ / Prevent copying
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    // دوال مساعدة خاصة / Private helper functions
    uint64_t allocateAddress(size_t size, size_t alignment = 0);
    bool deallocateAddress(uint64_t address);
    uint64_t findFreeRegion(size_t size, size_t alignment = 0);
    void updateStatistics();
    
    // البيانات الداخلية / Internal data
    std::unordered_map<uint64_t, MemoryBlock> allocatedBlocks_;
    std::vector<MemoryRegion> regions_;
    uint64_t nextAddress_;
    uint64_t nextAllocationId_;
    size_t totalAllocated_;
    size_t peakUsage_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_MEMORY_MAP_H
