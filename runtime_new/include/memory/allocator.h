/*
 * بسم الله الرحمن الرحيم
 * ================================
 * نظام تخصيص الذاكرة / Memory Allocator System
 * ================================
 * 
 * نظام متقدم لتخصيص وإدارة الذاكرة للغة Sad
 * Advanced memory allocation and management system for Sad language
 * 
 * الميزات / Features:
 * - Memory pools for different object sizes
 * - Fast allocation/deallocation (O(1))
 * - Memory alignment support
 * - Fragmentation prevention
 * - Memory leak detection (debug mode)
 * - Thread-safe operations
 * 
 * @file runtime/memory/allocator.h
 * @author SadLang Compiler Team
 * @date December 2025
 */

#ifndef SAD_RUNTIME_ALLOCATOR_H
#define SAD_RUNTIME_ALLOCATOR_H

#include <cstdint>
#include <cstddef>
#include <atomic>
#include <mutex>
#include <vector>
#include <unordered_map>

namespace Sad {
namespace Memory {

// ============================================================================
// Constants & Configuration / الثوابت والإعدادات
// ============================================================================

/**
 * حجم الصفحة الافتراضي / Default page size
 */
constexpr size_t DEFAULT_PAGE_SIZE = 4096;

/**
 * الحد الأقصى لحجم التجمع / Maximum pool size
 */
constexpr size_t MAX_POOL_SIZE = 1024 * 1024 * 16; // 16 MB

/**
 * أحجام الكتل المحددة مسبقًا / Predefined block sizes
 */
constexpr size_t SMALL_BLOCK_SIZE = 16;      // 16 bytes
constexpr size_t MEDIUM_BLOCK_SIZE = 64;     // 64 bytes
constexpr size_t LARGE_BLOCK_SIZE = 256;     // 256 bytes
constexpr size_t XLARGE_BLOCK_SIZE = 1024;   // 1 KB

/**
 * محاذاة الذاكرة / Memory alignment
 */
constexpr size_t DEFAULT_ALIGNMENT = 8;      // 8 bytes (64-bit)
constexpr size_t CACHE_LINE_SIZE = 64;       // CPU cache line

// ============================================================================
// Type Definitions / تعريفات الأنواع
// ============================================================================

/**
 * معرف الكتلة / Block ID
 */
using BlockId = uint64_t;

/**
 * معلومات الكتلة / Block metadata
 */
struct BlockHeader {
    size_t size;            // حجم الكتلة (بدون الرأس)
    BlockId id;             // معرف فريد
    uint32_t flags;         // أعلام الحالة
    uint32_t magic;         // رقم سحري للتحقق
    BlockHeader* next;      // الكتلة التالية في القائمة الحرة
    
    // معلومات التصحيح / Debug info
#ifdef SAD_DEBUG_MEMORY
    const char* file;       // ملف التخصيص
    int line;               // سطر التخصيص
    uint64_t timestamp;     // وقت التخصيص
#endif
};

/**
 * أعلام الكتلة / Block flags
 */
enum class BlockFlags : uint32_t {
    None        = 0x00,
    Allocated   = 0x01,     // مخصصة
    Pinned      = 0x02,     // مثبتة (لا يمكن نقلها)
    Marked      = 0x04,     // معلمة للـ GC
    Large       = 0x08,     // كتلة كبيرة
    Pool        = 0x10      // من تجمع
};

/**
 * الرقم السحري للتحقق / Magic number for validation
 */
constexpr uint32_t BLOCK_MAGIC = 0xDEADBEEF;

// ============================================================================
// Memory Pool / تجمع الذاكرة
// ============================================================================

/**
 * تجمع ذاكرة لحجم كتلة محدد
 * Memory pool for specific block size
 */
class MemoryPool {
public:
    /**
     * إنشاء تجمع جديد / Create new pool
     * 
     * @param block_size حجم الكتلة
     * @param initial_blocks عدد الكتل الأولية
     */
    MemoryPool(size_t block_size, size_t initial_blocks = 64);
    
    /**
     * تدمير التجمع / Destroy pool
     */
    ~MemoryPool();
    
    // منع النسخ / Prevent copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    /**
     * تخصيص كتلة / Allocate block
     * 
     * @return مؤشر للذاكرة المخصصة، أو nullptr عند الفشل
     */
    void* allocate();
    
    /**
     * تحرير كتلة / Free block
     * 
     * @param ptr مؤشر الكتلة
     * @return true عند النجاح
     */
    bool deallocate(void* ptr);
    
    /**
     * التحقق من امتلاك الكتلة / Check if owns block
     */
    bool owns(void* ptr) const;
    
    /**
     * توسيع التجمع / Expand pool
     * 
     * @param additional_blocks عدد الكتل الإضافية
     * @return true عند النجاح
     */
    bool expand(size_t additional_blocks);
    
    /**
     * الحصول على الإحصائيات / Get statistics
     */
    size_t get_block_size() const { return block_size_; }
    size_t get_total_blocks() const { return total_blocks_; }
    size_t get_free_blocks() const { return free_blocks_; }
    size_t get_allocated_blocks() const { return total_blocks_ - free_blocks_; }
    size_t get_total_memory() const { return total_memory_; }
    
private:
    /**
     * إضافة صفحة جديدة / Add new page
     */
    bool add_page(size_t num_blocks);
    
    size_t block_size_;         // حجم الكتلة
    size_t total_blocks_;       // إجمالي الكتل
    size_t free_blocks_;        // الكتل الحرة
    size_t total_memory_;       // إجمالي الذاكرة المستخدمة
    
    BlockHeader* free_list_;    // قائمة الكتل الحرة
    std::vector<void*> pages_;  // صفحات الذاكرة
    
    mutable std::mutex mutex_;          // قفل للحماية من التزامن
};

// ============================================================================
// Memory Allocator / مخصص الذاكرة
// ============================================================================

/**
 * النظام الرئيسي لتخصيص الذاكرة
 * Main memory allocation system
 */
class MemoryAllocator {
public:
    /**
     * الحصول على النسخة الوحيدة / Get singleton instance
     */
    static MemoryAllocator& instance();
    
    /**
     * تهيئة المخصص / Initialize allocator
     * 
     * @param config إعدادات الذاكرة
     * @return true عند النجاح
     */
    bool initialize(const struct AllocatorConfig* config = nullptr);
    
    /**
     * إيقاف المخصص / Shutdown allocator
     */
    void shutdown();
    
    /**
     * تخصيص ذاكرة / Allocate memory
     * 
     * @param size الحجم المطلوب
     * @param alignment المحاذاة المطلوبة
     * @return مؤشر الذاكرة أو nullptr
     */
    void* allocate(size_t size, size_t alignment = DEFAULT_ALIGNMENT);
    
    /**
     * تحرير ذاكرة / Free memory
     * 
     * @param ptr مؤشر الذاكرة
     * @return true عند النجاح
     */
    bool deallocate(void* ptr);
    
    /**
     * إعادة تخصيص ذاكرة / Reallocate memory
     * 
     * @param ptr المؤشر القديم
     * @param new_size الحجم الجديد
     * @return المؤشر الجديد أو nullptr
     */
    void* reallocate(void* ptr, size_t new_size);
    
    /**
     * الحصول على حجم الكتلة / Get block size
     */
    size_t get_block_size(void* ptr) const;
    
    /**
     * تثبيت كتلة / Pin block (prevent GC)
     */
    bool pin(void* ptr);
    
    /**
     * إلغاء تثبيت كتلة / Unpin block
     */
    bool unpin(void* ptr);
    
    /**
     * التحقق من صحة الكتلة / Validate block
     */
    bool is_valid_block(void* ptr) const;
    
    /**
     * الحصول على رأس الكتلة / Get block header
     */
    BlockHeader* get_header(void* ptr) const;
    
    /**
     * إحصائيات الذاكرة / Memory statistics
     */
    struct Statistics {
        size_t total_allocated;     // إجمالي الذاكرة المخصصة
        size_t total_freed;         // إجمالي الذاكرة المحررة
        size_t current_usage;       // الاستخدام الحالي
        size_t peak_usage;          // ذروة الاستخدام
        size_t allocation_count;    // عدد التخصيصات
        size_t deallocation_count;  // عدد التحريرات
        size_t pool_allocations;    // تخصيصات من التجمعات
        size_t large_allocations;   // تخصيصات كبيرة
    };
    
    /**
     * الحصول على الإحصائيات / Get statistics
     */
    Statistics get_statistics() const;
    
    /**
     * إعادة تعيين الإحصائيات / Reset statistics
     */
    void reset_statistics();
    
    /**
     * طباعة الإحصائيات / Print statistics
     */
    void print_statistics() const;
    
#ifdef SAD_DEBUG_MEMORY
    /**
     * كشف تسريبات الذاكرة / Detect memory leaks
     */
    void detect_leaks();
    
    /**
     * تسجيل التخصيص / Log allocation
     */
    void log_allocation(void* ptr, size_t size, const char* file, int line);
#endif

private:
    /**
     * مُنشئ خاص (Singleton) / Private constructor
     */
    MemoryAllocator();
    
    /**
     * مُدمر / Destructor
     */
    ~MemoryAllocator();
    
    // منع النسخ / Prevent copying
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;
    
    /**
     * اختيار تجمع مناسب / Select appropriate pool
     */
    MemoryPool* select_pool(size_t size);
    
    /**
     * تخصيص كتلة كبيرة / Allocate large block
     */
    void* allocate_large(size_t size, size_t alignment);
    
    /**
     * تحرير كتلة كبيرة / Free large block
     */
    bool deallocate_large(void* ptr);
    
    /**
     * محاذاة الحجم / Align size
     */
    static size_t align_size(size_t size, size_t alignment);
    
    /**
     * توليد معرف فريد / Generate unique ID
     */
    BlockId generate_id();
    
    bool initialized_;                              // حالة التهيئة
    std::atomic<BlockId> next_id_;                  // المعرف التالي
    
    // تجمعات الذاكرة / Memory pools
    MemoryPool* small_pool_;     // 16 bytes
    MemoryPool* medium_pool_;    // 64 bytes
    MemoryPool* large_pool_;     // 256 bytes
    MemoryPool* xlarge_pool_;    // 1 KB
    
    // الكتل الكبيرة / Large blocks
    std::unordered_map<void*, BlockHeader*> large_blocks_;
    mutable std::mutex large_blocks_mutex_;
    
    // الإحصائيات / Statistics
    mutable std::mutex stats_mutex_;
    Statistics stats_;
    
#ifdef SAD_DEBUG_MEMORY
    // تتبع التسريبات / Leak tracking
    struct AllocationInfo {
        size_t size;
        const char* file;
        int line;
        uint64_t timestamp;
    };
    std::unordered_map<void*, AllocationInfo> allocations_;
    mutable std::mutex allocations_mutex_;
#endif
};

// ============================================================================
// Allocator Configuration / إعدادات المخصص
// ============================================================================

/**
 * إعدادات المخصص / Allocator configuration
 */
struct AllocatorConfig {
    size_t small_pool_blocks;   // عدد كتل التجمع الصغير
    size_t medium_pool_blocks;  // عدد كتل التجمع المتوسط
    size_t large_pool_blocks;   // عدد كتل التجمع الكبير
    size_t xlarge_pool_blocks;  // عدد كتل التجمع الكبير جدًا
    bool enable_leak_detection; // تفعيل كشف التسريبات
    bool enable_statistics;     // تفعيل الإحصائيات
    
    /**
     * الإعدادات الافتراضية / Default configuration
     */
    static AllocatorConfig default_config() {
        return AllocatorConfig{
            128,    // small_pool_blocks
            64,     // medium_pool_blocks
            32,     // large_pool_blocks
            16,     // xlarge_pool_blocks
            true,   // enable_leak_detection (في وضع التصحيح)
            true    // enable_statistics
        };
    }
};

// ============================================================================
// Helper Macros / ماكروهات مساعدة
// ============================================================================

#ifdef SAD_DEBUG_MEMORY
    #define SAD_ALLOCATE(size) \
        Sad::Memory::MemoryAllocator::instance().allocate(size)
    
    #define SAD_FREE(ptr) \
        Sad::Memory::MemoryAllocator::instance().deallocate(ptr)
#else
    #define SAD_ALLOCATE(size) \
        Sad::Memory::MemoryAllocator::instance().allocate(size)
    
    #define SAD_FREE(ptr) \
        Sad::Memory::MemoryAllocator::instance().deallocate(ptr)
#endif

/**
 * تخصيص مع نوع / Allocate with type
 */
#define SAD_NEW(type) \
    new (SAD_ALLOCATE(sizeof(type))) type

/**
 * تخصيص مصفوفة / Allocate array
 */
#define SAD_NEW_ARRAY(type, count) \
    new (SAD_ALLOCATE(sizeof(type) * (count))) type[count]

/**
 * تحرير مع نوع / Free with type
 */
#define SAD_DELETE(ptr, type) \
    do { \
        if (ptr) { \
            (ptr)->~type(); \
            SAD_FREE(ptr); \
        } \
    } while(0)

/**
 * تحرير مصفوفة / Free array
 */
#define SAD_DELETE_ARRAY(ptr, type, count) \
    do { \
        if (ptr) { \
            for (size_t i = 0; i < (count); ++i) { \
                (ptr)[i].~type(); \
            } \
            SAD_FREE(ptr); \
        } \
    } while(0)

} // namespace Memory
} // namespace Sad

#endif // SAD_RUNTIME_ALLOCATOR_H
