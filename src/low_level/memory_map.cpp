/*
 * ============================================================================
 * تنفيذ إدارة الذاكرة - Memory Management Implementation
 * ============================================================================
 * 
 * هذا الملف يحتوي على التنفيذ الفعلي لنظام إدارة الذاكرة
 * This file contains the actual implementation of memory management system
 * ============================================================================
 */

#include "low_level/memory_map.h"
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cstring>

namespace Sad {
namespace LowLevel {

// ============================================================================
// المنشئ والوصول للمثيل الوحيد / Constructor and Singleton Access
// ============================================================================

MemoryManager::MemoryManager()
    : nextAddress_(0x100000)  // Start at 1MB
    , nextAllocationId_(1)
    , totalAllocated_(0)
    , peakUsage_(0) {
}

MemoryManager& MemoryManager::getInstance() {
    static MemoryManager instance;
    return instance;
}

// ============================================================================
// التخصيص الأساسي / Basic Allocation
// ============================================================================

uint64_t MemoryManager::malloc(size_t size) {
    /*
     * تخصيص ذاكرة ديناميكية
     * Allocate dynamic memory
     * 
     * الخوارزمية / Algorithm:
     * 1. التحقق من صحة الحجم / Validate size
     * 2. البحث عن منطقة حرة / Find free region
     * 3. تخصيص العنوان / Allocate address
     * 4. تسجيل الكتلة / Register block
     * 5. تحديث الإحصائيات / Update statistics
     */
    
    // فحص الحجم / Validate size
    if (size == 0 || size > MemoryConstants::MAX_ALLOCATION) {
        return 0;
    }
    
    // تخصيص العنوان / Allocate address
    uint64_t address = allocateAddress(size);
    if (address == 0) {
        return 0;
    }
    
    // إنشاء كتلة / Create block
    MemoryBlock block(address, size);
    block.allocationId = nextAllocationId_++;
    
    // تسجيل الكتلة / Register block
    allocatedBlocks_[address] = block;
    
    // تحديث الإحصائيات / Update statistics
    totalAllocated_ += size;
    if (totalAllocated_ > peakUsage_) {
        peakUsage_ = totalAllocated_;
    }
    
    return address;
}

uint64_t MemoryManager::alignedMalloc(size_t size, size_t alignment) {
    /*
     * تخصيص ذاكرة مع محاذاة
     * Allocate aligned memory
     * 
     * المحاذاة يجب أن تكون قوة 2
     * Alignment must be a power of 2
     */
    
    // فحص المحاذاة / Validate alignment
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        throw std::runtime_error(
            "خطأ: المحاذاة يجب أن تكون قوة 2\n"
            "Error: Alignment must be a power of 2"
        );
    }
    
    // فحص الحجم / Validate size
    if (size == 0 || size > MemoryConstants::MAX_ALLOCATION) {
        return 0;
    }
    
    // تخصيص مع محاذاة / Allocate with alignment
    uint64_t address = allocateAddress(size, alignment);
    if (address == 0) {
        return 0;
    }
    
    // إنشاء كتلة / Create block
    MemoryBlock block(address, size, alignment);
    block.allocationId = nextAllocationId_++;
    
    // تسجيل / Register
    allocatedBlocks_[address] = block;
    
    // تحديث الإحصائيات / Update statistics
    totalAllocated_ += size;
    if (totalAllocated_ > peakUsage_) {
        peakUsage_ = totalAllocated_;
    }
    
    return address;
}

uint64_t MemoryManager::calloc(size_t count, size_t size) {
    /*
     * تخصيص ذاكرة مع تهيئة بأصفار
     * Allocate and zero-initialize memory
     * 
     * ملاحظة: في نظام حقيقي، سيتم كتابة أصفار فعلية
     * Note: In a real system, actual zeros would be written
     */
    
    // فحص تجاوز السعة / Check overflow
    if (count > 0 && size > MemoryConstants::MAX_ALLOCATION / count) {
        return 0;
    }
    
    size_t totalSize = count * size;
    
    // تخصيص / Allocate
    uint64_t address = malloc(totalSize);
    if (address == 0) {
        return 0;
    }
    
    // في نظام حقيقي: memset(address, 0, totalSize)
    // In real system: memset(address, 0, totalSize)
    
    return address;
}

uint64_t MemoryManager::realloc(uint64_t address, size_t newSize) {
    /*
     * إعادة تخصيص ذاكرة
     * Reallocate memory
     * 
     * الخوارزمية / Algorithm:
     * 1. إذا العنوان NULL، نفس malloc
     * 2. إذا الحجم 0، نفس free
     * 3. وإلا، تخصيص جديد ونسخ البيانات
     */
    
    // حالة خاصة: NULL pointer
    // Special case: NULL pointer
    if (address == 0) {
        return malloc(newSize);
    }
    
    // حالة خاصة: حجم صفر
    // Special case: zero size
    if (newSize == 0) {
        free(address);
        return 0;
    }
    
    // البحث عن الكتلة الحالية / Find current block
    auto it = allocatedBlocks_.find(address);
    if (it == allocatedBlocks_.end()) {
        throw std::runtime_error(
            "خطأ: محاولة إعادة تخصيص عنوان غير مخصص\n"
            "Error: Attempting to realloc unallocated address"
        );
    }
    
    size_t oldSize = it->second.size;
    
    // إذا الحجم الجديد أصغر أو يساوي، نبقي نفس الكتلة
    // If new size is smaller or equal, keep same block
    if (newSize <= oldSize) {
        it->second.size = newSize;
        totalAllocated_ = totalAllocated_ - oldSize + newSize;
        return address;
    }
    
    // تخصيص كتلة جديدة / Allocate new block
    uint64_t newAddress = malloc(newSize);
    if (newAddress == 0) {
        return 0;
    }
    
    // في نظام حقيقي: نسخ البيانات القديمة
    // In real system: copy old data
    // memcpy(newAddress, address, oldSize);
    
    // تحرير الكتلة القديمة / Free old block
    free(address);
    
    return newAddress;
}

bool MemoryManager::free(uint64_t address) {
    /*
     * تحرير ذاكرة مخصصة
     * Free allocated memory
     * 
     * يحذف الكتلة من السجل ويحدث الإحصائيات
     * Removes block from registry and updates statistics
     */
    
    // السماح بـ free(NULL)
    // Allow free(NULL)
    if (address == 0) {
        return true;
    }
    
    // البحث عن الكتلة / Find block
    auto it = allocatedBlocks_.find(address);
    if (it == allocatedBlocks_.end()) {
        throw std::runtime_error(
            "خطأ: محاولة تحرير عنوان غير مخصص\n"
            "Error: Attempting to free unallocated address"
        );
    }
    
    // تحديث الإحصائيات / Update statistics
    totalAllocated_ -= it->second.size;
    
    // حذف الكتلة / Remove block
    allocatedBlocks_.erase(it);
    
    return true;
}

// ============================================================================
// دوال التعيين / Mapping Functions
// ============================================================================

uint64_t MemoryManager::mmap(uint64_t address, size_t size,
                              MemoryProtection protection, MapFlags flags,
                              const std::string& name) {
    /*
     * تعيين منطقة ذاكرة
     * Map memory region
     * 
     * يشبه mmap في Linux/POSIX
     * Similar to mmap in Linux/POSIX
     */
    
    // فحص الحجم / Validate size
    if (size == 0 || size > MemoryConstants::MAX_ALLOCATION) {
        return 0;
    }
    
    // محاذاة الحجم إلى حدود الصفحة
    // Align size to page boundary
    size_t pageSize = MemoryConstants::PAGE_SIZE_4KB;
    size_t alignedSize = ((size + pageSize - 1) / pageSize) * pageSize;
    
    // تحديد العنوان / Determine address
    uint64_t mappedAddress;
    if (address != 0 && static_cast<uint32_t>(flags) & static_cast<uint32_t>(MapFlags::FIXED)) {
        // استخدام العنوان المحدد / Use specified address
        mappedAddress = address;
        
        // فحص التداخل / Check for overlap
        for (const auto& region : regions_) {
            if (region.state != RegionState::FREE) {
                MemoryRegion testRegion(mappedAddress, alignedSize, 
                                       RegionState::MAPPED, protection);
                if (region.overlaps(testRegion)) {
                    return 0;  // Overlap detected
                }
            }
        }
    } else {
        // البحث عن منطقة حرة / Find free region
        mappedAddress = findFreeRegion(alignedSize, pageSize);
        if (mappedAddress == 0) {
            return 0;
        }
    }
    
    // إنشاء المنطقة / Create region
    MemoryRegion region(mappedAddress, alignedSize, RegionState::MAPPED, 
                       protection, name);
    
    // قفل في الذاكرة إذا مطلوب / Lock if requested
    if (static_cast<uint32_t>(flags) & static_cast<uint32_t>(MapFlags::LOCKED)) {
        region.isLocked = true;
    }
    
    // إضافة المنطقة / Add region
    regions_.push_back(region);
    
    // تحديث الإحصائيات / Update statistics
    totalAllocated_ += alignedSize;
    if (totalAllocated_ > peakUsage_) {
        peakUsage_ = totalAllocated_;
    }
    
    return mappedAddress;
}

bool MemoryManager::munmap(uint64_t address, size_t size) {
    /*
     * إلغاء تعيين منطقة ذاكرة
     * Unmap memory region
     * 
     * يشبه munmap في Linux/POSIX
     * Similar to munmap in Linux/POSIX
     */
    
    // البحث عن المنطقة / Find region
    for (auto it = regions_.begin(); it != regions_.end(); ++it) {
        if (it->startAddress == address) {
            // تحديث الإحصائيات / Update statistics
            totalAllocated_ -= it->size;
            
            // حذف المنطقة / Remove region
            regions_.erase(it);
            return true;
        }
    }
    
    return false;
}

bool MemoryManager::mprotect(uint64_t address, size_t size,
                              MemoryProtection protection) {
    /*
     * تغيير حماية منطقة
     * Change region protection
     * 
     * يشبه mprotect في Linux/POSIX
     * Similar to mprotect in Linux/POSIX
     */
    
    // البحث عن المنطقة / Find region
    for (auto& region : regions_) {
        if (region.contains(address)) {
            region.protection = protection;
            return true;
        }
    }
    
    return false;
}

// ============================================================================
// دوال الاستعلام / Query Functions
// ============================================================================

const MemoryBlock* MemoryManager::getBlockInfo(uint64_t address) {
    auto it = allocatedBlocks_.find(address);
    if (it != allocatedBlocks_.end()) {
        return &it->second;
    }
    return nullptr;
}

const MemoryRegion* MemoryManager::getRegionInfo(uint64_t address) {
    for (const auto& region : regions_) {
        if (region.contains(address)) {
            return &region;
        }
    }
    return nullptr;
}

bool MemoryManager::isValidAddress(uint64_t address) {
    if (address == 0) {
        return false;
    }
    
    // فحص في الكتل المخصصة / Check allocated blocks
    if (allocatedBlocks_.find(address) != allocatedBlocks_.end()) {
        return true;
    }
    
    // فحص في المناطق / Check regions
    for (const auto& region : regions_) {
        if (region.contains(address)) {
            return true;
        }
    }
    
    return false;
}

bool MemoryManager::isAllocated(uint64_t address) {
    return allocatedBlocks_.find(address) != allocatedBlocks_.end();
}

size_t MemoryManager::getBlockSize(uint64_t address) {
    auto it = allocatedBlocks_.find(address);
    if (it != allocatedBlocks_.end()) {
        return it->second.size;
    }
    return 0;
}

// ============================================================================
// دوال الإحصائيات / Statistics Functions
// ============================================================================

std::vector<MemoryBlock> MemoryManager::detectLeaks() const {
    /*
     * كشف التسريبات
     * Detect memory leaks
     * 
     * يعيد قائمة بجميع الكتل المخصصة التي لم تُحرر
     * Returns list of all allocated blocks that weren't freed
     */
    
    std::vector<MemoryBlock> leaks;
    for (const auto& pair : allocatedBlocks_) {
        leaks.push_back(pair.second);
    }
    return leaks;
}

std::string MemoryManager::getMemoryReport() const {
    /*
     * إنشاء تقرير شامل عن الذاكرة
     * Generate comprehensive memory report
     */
    
    std::ostringstream oss;
    
    oss << "╔══════════════════════════════════════════════════════════╗\n";
    oss << "║           تقرير الذاكرة - Memory Report                 ║\n";
    oss << "╚══════════════════════════════════════════════════════════╝\n\n";
    
    oss << "الإحصائيات / Statistics:\n";
    oss << "  - إجمالي المخصص / Total Allocated: " << totalAllocated_ << " bytes\n";
    oss << "  - الذروة / Peak Usage: " << peakUsage_ << " bytes\n";
    oss << "  - عدد الكتل / Block Count: " << allocatedBlocks_.size() << "\n";
    oss << "  - عدد المناطق / Region Count: " << regions_.size() << "\n\n";
    
    if (!allocatedBlocks_.empty()) {
        oss << "الكتل المخصصة / Allocated Blocks:\n";
        for (const auto& pair : allocatedBlocks_) {
            const auto& block = pair.second;
            oss << "  - العنوان / Address: 0x" << std::hex << block.address << std::dec;
            oss << ", الحجم / Size: " << block.size << " bytes";
            if (block.isAligned) {
                oss << ", المحاذاة / Aligned: " << block.alignment;
            }
            oss << "\n";
        }
        oss << "\n";
    }
    
    if (!regions_.empty()) {
        oss << "المناطق / Regions:\n";
        for (const auto& region : regions_) {
            oss << "  - " << (region.name.empty() ? "بدون اسم / Unnamed" : region.name) << "\n";
            oss << "    العنوان / Address: 0x" << std::hex << region.startAddress << std::dec;
            oss << " - 0x" << std::hex << region.endAddress << std::dec;
            oss << ", الحجم / Size: " << region.size << " bytes\n";
            oss << "    الحالة / State: ";
            switch (region.state) {
                case RegionState::FREE: oss << "حرة / Free"; break;
                case RegionState::ALLOCATED: oss << "مخصصة / Allocated"; break;
                case RegionState::MAPPED: oss << "معينة / Mapped"; break;
                case RegionState::RESERVED: oss << "محجوزة / Reserved"; break;
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

// ============================================================================
// دوال الصيانة / Maintenance Functions
// ============================================================================

void MemoryManager::clear() {
    /*
     * مسح جميع التخصيصات
     * Clear all allocations
     * 
     * يستخدم للاختبار والتنظيف
     * Used for testing and cleanup
     */
    
    allocatedBlocks_.clear();
    regions_.clear();
    totalAllocated_ = 0;
    nextAddress_ = 0x100000;
    nextAllocationId_ = 1;
}

void MemoryManager::compact() {
    /*
     * ضغط الذاكرة
     * Compact memory
     * 
     * دمج المناطق الحرة المتجاورة
     * Merge adjacent free regions
     */
    
    if (regions_.size() < 2) {
        return;
    }
    
    // ترتيب المناطق حسب العنوان / Sort regions by address
    std::sort(regions_.begin(), regions_.end(),
              [](const MemoryRegion& a, const MemoryRegion& b) {
                  return a.startAddress < b.startAddress;
              });
    
    // دمج المناطق الحرة المتجاورة / Merge adjacent free regions
    for (size_t i = 0; i < regions_.size() - 1; ) {
        if (regions_[i].state == RegionState::FREE &&
            regions_[i + 1].state == RegionState::FREE &&
            regions_[i].endAddress == regions_[i + 1].startAddress) {
            
            // دمج / Merge
            regions_[i].endAddress = regions_[i + 1].endAddress;
            regions_[i].size += regions_[i + 1].size;
            regions_.erase(regions_.begin() + i + 1);
        } else {
            i++;
        }
    }
}

// ============================================================================
// دوال مساعدة خاصة / Private Helper Functions
// ============================================================================

uint64_t MemoryManager::allocateAddress(size_t size, size_t alignment) {
    /*
     * تخصيص عنوان من مساحة العناوين
     * Allocate address from address space
     */
    
    uint64_t address = nextAddress_;
    
    // تطبيق المحاذاة إذا مطلوبة / Apply alignment if needed
    if (alignment > 0) {
        uint64_t remainder = address % alignment;
        if (remainder != 0) {
            address += (alignment - remainder);
        }
    }
    
    // تحديث العنوان التالي / Update next address
    nextAddress_ = address + size;
    
    return address;
}

uint64_t MemoryManager::findFreeRegion(size_t size, size_t alignment) {
    /*
     * البحث عن منطقة حرة مناسبة
     * Find suitable free region
     */
    
    // خوارزمية بسيطة: استخدام nextAddress_
    // Simple algorithm: use nextAddress_
    
    uint64_t address = nextAddress_;
    
    // تطبيق المحاذاة / Apply alignment
    if (alignment > 0) {
        uint64_t remainder = address % alignment;
        if (remainder != 0) {
            address += (alignment - remainder);
        }
    }
    
    // تحديث / Update
    nextAddress_ = address + size;
    
    return address;
}

} // namespace LowLevel
} // namespace Sad
