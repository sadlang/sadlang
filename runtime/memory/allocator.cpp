/*
 * بسم الله الرحمن الرحيم
 * ================================
 * تنفيذ نظام تخصيص الذاكرة / Memory Allocator Implementation
 * ================================
 */

#include "allocator.h"
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <algorithm>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif

namespace Sad {
namespace Memory {

// ============================================================================
// Helper Functions / دوال مساعدة
// ============================================================================

/**
 * محاذاة العنوان / Align address
 */
static inline void* align_pointer(void* ptr, size_t alignment) {
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
    return reinterpret_cast<void*>(aligned);
}

/**
 * التحقق من محاذاة العنوان / Check if address is aligned
 */
static inline bool is_aligned(void* ptr, size_t alignment) {
    return (reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0;
}

/**
 * الحصول على الوقت الحالي / Get current timestamp
 */
static inline uint64_t get_timestamp() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()
    ).count();
}

// ============================================================================
// Memory Pool Implementation / تنفيذ تجمع الذاكرة
// ============================================================================

MemoryPool::MemoryPool(size_t block_size, size_t initial_blocks)
    : block_size_(block_size)
    , total_blocks_(0)
    , free_blocks_(0)
    , total_memory_(0)
    , free_list_(nullptr)
{
    // محاذاة حجم الكتلة / Align block size
    block_size_ = std::max(block_size_, sizeof(BlockHeader));
    block_size_ = (block_size_ + DEFAULT_ALIGNMENT - 1) & ~(DEFAULT_ALIGNMENT - 1);
    
    // إضافة الصفحة الأولية / Add initial page
    if (initial_blocks > 0) {
        add_page(initial_blocks);
    }
}

MemoryPool::~MemoryPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // تحرير جميع الصفحات / Free all pages
    for (void* page : pages_) {
#ifdef _WIN32
        VirtualFree(page, 0, MEM_RELEASE);
#else
        munmap(page, total_memory_ / pages_.size());
#endif
    }
}

void* MemoryPool::allocate() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // إذا لم تكن هناك كتل حرة، قم بالتوسيع
    if (!free_list_) {
        if (!expand(64)) {
            return nullptr;
        }
    }
    
    // أخذ كتلة من القائمة الحرة
    BlockHeader* block = free_list_;
    free_list_ = block->next;
    free_blocks_--;
    
    // تعيين البيانات الوصفية
    block->flags = static_cast<uint32_t>(BlockFlags::Allocated) | 
                   static_cast<uint32_t>(BlockFlags::Pool);
    block->next = nullptr;
    
    // إرجاع المؤشر بعد الرأس
    return reinterpret_cast<uint8_t*>(block) + sizeof(BlockHeader);
}

bool MemoryPool::deallocate(void* ptr) {
    if (!ptr || !owns(ptr)) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // الحصول على الرأس
    BlockHeader* block = reinterpret_cast<BlockHeader*>(
        reinterpret_cast<uint8_t*>(ptr) - sizeof(BlockHeader)
    );
    
    // التحقق من الرقم السحري
    if (block->magic != BLOCK_MAGIC) {
        return false;
    }
    
    // إضافة للقائمة الحرة
    block->flags = static_cast<uint32_t>(BlockFlags::None);
    block->next = free_list_;
    free_list_ = block;
    free_blocks_++;
    
    return true;
}

bool MemoryPool::owns(void* ptr) const {
    if (!ptr) return false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    
    for (void* page : pages_) {
        uintptr_t page_start = reinterpret_cast<uintptr_t>(page);
        uintptr_t page_end = page_start + (block_size_ * (total_blocks_ / pages_.size()));
        
        if (addr >= page_start && addr < page_end) {
            return true;
        }
    }
    
    return false;
}

bool MemoryPool::expand(size_t additional_blocks) {
    // التحقق من الحد الأقصى
    if (total_memory_ + (block_size_ * additional_blocks) > MAX_POOL_SIZE) {
        return false;
    }
    
    return add_page(additional_blocks);
}

bool MemoryPool::add_page(size_t num_blocks) {
    size_t page_size = block_size_ * num_blocks;
    
    // تخصيص صفحة جديدة
    void* page = nullptr;
    
#ifdef _WIN32
    page = VirtualAlloc(nullptr, page_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
    page = mmap(nullptr, page_size, PROT_READ | PROT_WRITE, 
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (page == MAP_FAILED) {
        page = nullptr;
    }
#endif
    
    if (!page) {
        return false;
    }
    
    // إضافة الكتل للقائمة الحرة
    uint8_t* current = reinterpret_cast<uint8_t*>(page);
    
    for (size_t i = 0; i < num_blocks; ++i) {
        BlockHeader* block = reinterpret_cast<BlockHeader*>(current);
        block->size = block_size_ - sizeof(BlockHeader);
        block->id = 0;
        block->flags = static_cast<uint32_t>(BlockFlags::None);
        block->magic = BLOCK_MAGIC;
        block->next = free_list_;
        
        free_list_ = block;
        current += block_size_;
    }
    
    pages_.push_back(page);
    total_blocks_ += num_blocks;
    free_blocks_ += num_blocks;
    total_memory_ += page_size;
    
    return true;
}

// ============================================================================
// Memory Allocator Implementation / تنفيذ المخصص الرئيسي
// ============================================================================

MemoryAllocator::MemoryAllocator()
    : initialized_(false)
    , next_id_(1)
    , small_pool_(nullptr)
    , medium_pool_(nullptr)
    , large_pool_(nullptr)
    , xlarge_pool_(nullptr)
{
    std::memset(&stats_, 0, sizeof(stats_));
}

MemoryAllocator::~MemoryAllocator() {
    shutdown();
}

MemoryAllocator& MemoryAllocator::instance() {
    static MemoryAllocator allocator;
    return allocator;
}

bool MemoryAllocator::initialize(const AllocatorConfig* config) {
    if (initialized_) {
        return true;
    }
    
    AllocatorConfig cfg = config ? *config : AllocatorConfig::default_config();
    
    try {
        // إنشاء التجمعات / Create pools
        small_pool_ = new MemoryPool(SMALL_BLOCK_SIZE, cfg.small_pool_blocks);
        medium_pool_ = new MemoryPool(MEDIUM_BLOCK_SIZE, cfg.medium_pool_blocks);
        large_pool_ = new MemoryPool(LARGE_BLOCK_SIZE, cfg.large_pool_blocks);
        xlarge_pool_ = new MemoryPool(XLARGE_BLOCK_SIZE, cfg.xlarge_pool_blocks);
        
        initialized_ = true;
        return true;
    }
    catch (...) {
        shutdown();
        return false;
    }
}

void MemoryAllocator::shutdown() {
    if (!initialized_) {
        return;
    }
    
#ifdef SAD_DEBUG_MEMORY
    detect_leaks();
#endif
    
    // حذف التجمعات / Delete pools
    delete small_pool_;
    delete medium_pool_;
    delete large_pool_;
    delete xlarge_pool_;
    
    small_pool_ = nullptr;
    medium_pool_ = nullptr;
    large_pool_ = nullptr;
    xlarge_pool_ = nullptr;
    
    // تحرير الكتل الكبيرة / Free large blocks
    {
        std::lock_guard<std::mutex> lock(large_blocks_mutex_);
        for (auto& pair : large_blocks_) {
            std::free(pair.second);
        }
        large_blocks_.clear();
    }
    
    initialized_ = false;
}

void* MemoryAllocator::allocate(size_t size, size_t alignment) {
    if (!initialized_) {
        initialize();
    }
    
    if (size == 0) {
        return nullptr;
    }
    
    // محاذاة الحجم / Align size
    size = align_size(size, alignment);
    
    void* ptr = nullptr;
    
    // اختيار التجمع المناسب / Select appropriate pool
    if (size <= SMALL_BLOCK_SIZE) {
        ptr = small_pool_->allocate();
        if (ptr) {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            stats_.pool_allocations++;
        }
    }
    else if (size <= MEDIUM_BLOCK_SIZE) {
        ptr = medium_pool_->allocate();
        if (ptr) {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            stats_.pool_allocations++;
        }
    }
    else if (size <= LARGE_BLOCK_SIZE) {
        ptr = large_pool_->allocate();
        if (ptr) {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            stats_.pool_allocations++;
        }
    }
    else if (size <= XLARGE_BLOCK_SIZE) {
        ptr = xlarge_pool_->allocate();
        if (ptr) {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            stats_.pool_allocations++;
        }
    }
    else {
        // كتلة كبيرة جدًا / Very large block
        ptr = allocate_large(size, alignment);
        if (ptr) {
            std::lock_guard<std::mutex> lock(stats_mutex_);
            stats_.large_allocations++;
        }
    }
    
    if (ptr) {
        // تحديث الإحصائيات / Update statistics
        std::lock_guard<std::mutex> lock(stats_mutex_);
        stats_.total_allocated += size;
        stats_.current_usage += size;
        stats_.allocation_count++;
        
        if (stats_.current_usage > stats_.peak_usage) {
            stats_.peak_usage = stats_.current_usage;
        }
        
        // تعيين المعرف / Set ID
        BlockHeader* header = get_header(ptr);
        if (header) {
            header->id = generate_id();
        }
    }
    
    return ptr;
}

bool MemoryAllocator::deallocate(void* ptr) {
    if (!ptr || !initialized_) {
        return false;
    }
    
    bool success = false;
    size_t block_size = get_block_size(ptr);
    
    // محاولة التحرير من التجمعات / Try pools first
    if (small_pool_->owns(ptr)) {
        success = small_pool_->deallocate(ptr);
    }
    else if (medium_pool_->owns(ptr)) {
        success = medium_pool_->deallocate(ptr);
    }
    else if (large_pool_->owns(ptr)) {
        success = large_pool_->deallocate(ptr);
    }
    else if (xlarge_pool_->owns(ptr)) {
        success = xlarge_pool_->deallocate(ptr);
    }
    else {
        // كتلة كبيرة / Large block
        success = deallocate_large(ptr);
    }
    
    if (success) {
        // تحديث الإحصائيات / Update statistics
        std::lock_guard<std::mutex> lock(stats_mutex_);
        stats_.total_freed += block_size;
        stats_.current_usage -= block_size;
        stats_.deallocation_count++;
    }
    
    return success;
}

void* MemoryAllocator::reallocate(void* ptr, size_t new_size) {
    if (!ptr) {
        return allocate(new_size);
    }
    
    if (new_size == 0) {
        deallocate(ptr);
        return nullptr;
    }
    
    size_t old_size = get_block_size(ptr);
    
    // إذا كان الحجم الجديد مناسب / If new size fits
    if (new_size <= old_size) {
        return ptr;
    }
    
    // تخصيص كتلة جديدة ونسخ البيانات / Allocate new block and copy
    void* new_ptr = allocate(new_size);
    if (!new_ptr) {
        return nullptr;
    }
    
    std::memcpy(new_ptr, ptr, old_size);
    deallocate(ptr);
    
    return new_ptr;
}

size_t MemoryAllocator::get_block_size(void* ptr) const {
    if (!ptr) {
        return 0;
    }
    
    BlockHeader* header = get_header(ptr);
    return header ? header->size : 0;
}

bool MemoryAllocator::pin(void* ptr) {
    if (!ptr) {
        return false;
    }
    
    BlockHeader* header = get_header(ptr);
    if (!header) {
        return false;
    }
    
    header->flags |= static_cast<uint32_t>(BlockFlags::Pinned);
    return true;
}

bool MemoryAllocator::unpin(void* ptr) {
    if (!ptr) {
        return false;
    }
    
    BlockHeader* header = get_header(ptr);
    if (!header) {
        return false;
    }
    
    header->flags &= ~static_cast<uint32_t>(BlockFlags::Pinned);
    return true;
}

bool MemoryAllocator::is_valid_block(void* ptr) const {
    if (!ptr) {
        return false;
    }
    
    BlockHeader* header = get_header(ptr);
    return header && header->magic == BLOCK_MAGIC;
}

BlockHeader* MemoryAllocator::get_header(void* ptr) const {
    if (!ptr) {
        return nullptr;
    }
    
    BlockHeader* header = reinterpret_cast<BlockHeader*>(
        reinterpret_cast<uint8_t*>(ptr) - sizeof(BlockHeader)
    );
    
    // التحقق من الرقم السحري / Validate magic number
    if (header->magic != BLOCK_MAGIC) {
        return nullptr;
    }
    
    return header;
}

MemoryAllocator::Statistics MemoryAllocator::get_statistics() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return stats_;
}

void MemoryAllocator::reset_statistics() {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    
    // الاحتفاظ بالاستخدام الحالي فقط / Keep only current usage
    size_t current = stats_.current_usage;
    std::memset(&stats_, 0, sizeof(stats_));
    stats_.current_usage = current;
}

void MemoryAllocator::print_statistics() const {
    Statistics stats = get_statistics();
    
    std::cout << "=== إحصائيات الذاكرة / Memory Statistics ===" << std::endl;
    std::cout << "إجمالي المخصص / Total Allocated: " 
              << stats.total_allocated << " bytes" << std::endl;
    std::cout << "إجمالي المحرر / Total Freed: " 
              << stats.total_freed << " bytes" << std::endl;
    std::cout << "الاستخدام الحالي / Current Usage: " 
              << stats.current_usage << " bytes" << std::endl;
    std::cout << "ذروة الاستخدام / Peak Usage: " 
              << stats.peak_usage << " bytes" << std::endl;
    std::cout << "عدد التخصيصات / Allocations: " 
              << stats.allocation_count << std::endl;
    std::cout << "عدد التحريرات / Deallocations: " 
              << stats.deallocation_count << std::endl;
    std::cout << "تخصيصات التجمعات / Pool Allocations: " 
              << stats.pool_allocations << std::endl;
    std::cout << "تخصيصات كبيرة / Large Allocations: " 
              << stats.large_allocations << std::endl;
}

#ifdef SAD_DEBUG_MEMORY
void MemoryAllocator::detect_leaks() {
    std::lock_guard<std::mutex> lock(allocations_mutex_);
    
    if (allocations_.empty()) {
        std::cout << "✅ لا توجد تسريبات ذاكرة / No memory leaks detected" << std::endl;
        return;
    }
    
    std::cout << "⚠️ تم اكتشاف " << allocations_.size() 
              << " تسريب ذاكرة / memory leak(s) detected:" << std::endl;
    
    for (const auto& pair : allocations_) {
        const AllocationInfo& info = pair.second;
        std::cout << "  - " << info.size << " bytes at " << pair.first
                  << " (allocated in " << info.file << ":" << info.line << ")"
                  << std::endl;
    }
}

void MemoryAllocator::log_allocation(void* ptr, size_t size, 
                                     const char* file, int line) {
    std::lock_guard<std::mutex> lock(allocations_mutex_);
    
    AllocationInfo info;
    info.size = size;
    info.file = file;
    info.line = line;
    info.timestamp = get_timestamp();
    
    allocations_[ptr] = info;
}
#endif

// Private Methods / الدوال الخاصة

MemoryPool* MemoryAllocator::select_pool(size_t size) {
    if (size <= SMALL_BLOCK_SIZE) return small_pool_;
    if (size <= MEDIUM_BLOCK_SIZE) return medium_pool_;
    if (size <= LARGE_BLOCK_SIZE) return large_pool_;
    if (size <= XLARGE_BLOCK_SIZE) return xlarge_pool_;
    return nullptr;
}

void* MemoryAllocator::allocate_large(size_t size, size_t alignment) {
    // تخصيص ذاكرة كبيرة / Allocate large memory
    size_t total_size = sizeof(BlockHeader) + size + alignment;
    
    void* raw = std::malloc(total_size);
    if (!raw) {
        return nullptr;
    }
    
    // محاذاة المؤشر / Align pointer
    uint8_t* aligned = reinterpret_cast<uint8_t*>(
        align_pointer(reinterpret_cast<uint8_t*>(raw) + sizeof(BlockHeader), alignment)
    );
    
    // إنشاء الرأس / Create header
    BlockHeader* header = reinterpret_cast<BlockHeader*>(aligned - sizeof(BlockHeader));
    header->size = size;
    header->id = generate_id();
    header->flags = static_cast<uint32_t>(BlockFlags::Allocated) | 
                    static_cast<uint32_t>(BlockFlags::Large);
    header->magic = BLOCK_MAGIC;
    header->next = nullptr;
    
    // حفظ في الخريطة / Save in map
    {
        std::lock_guard<std::mutex> lock(large_blocks_mutex_);
        large_blocks_[aligned] = header;
    }
    
    return aligned;
}

bool MemoryAllocator::deallocate_large(void* ptr) {
    std::lock_guard<std::mutex> lock(large_blocks_mutex_);
    
    auto it = large_blocks_.find(ptr);
    if (it == large_blocks_.end()) {
        return false;
    }
    
    BlockHeader* header = it->second;
    
    // التحقق من الرقم السحري / Validate magic
    if (header->magic != BLOCK_MAGIC) {
        return false;
    }
    
    // تحرير الذاكرة / Free memory
    std::free(header);
    large_blocks_.erase(it);
    
    return true;
}

size_t MemoryAllocator::align_size(size_t size, size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}

BlockId MemoryAllocator::generate_id() {
    return next_id_.fetch_add(1, std::memory_order_relaxed);
}

} // namespace Memory
} // namespace Sad
