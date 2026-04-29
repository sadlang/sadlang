// ============================================================================
// safe_allocator.cpp — تنفيذ SafeAllocator و AllocationTracker
// (AR) منقول من compiler/src/security/compiler_security_arithmetic_mem.cpp
//      مع توحيد فضاء الأسماء وتنظيف الترميز.
// (EN) Moved from compiler/src/security/compiler_security_arithmetic_mem.cpp,
//      namespace unified, encoding cleaned.
// ============================================================================

#include "safe_allocator.h"
#include "bounds_checker.h"

#include <algorithm>
#include <iostream>
#include <new>
#include <stdexcept>

namespace Sad {
namespace Security {

// ----------------------------------------------------------------------------
// (AR) SafeAllocator
// (EN) SafeAllocator
// ----------------------------------------------------------------------------

void* SafeAllocator::safeAlloc(std::size_t size) {
    if (!BoundsChecker::checkAllocationSize(size)) throw std::bad_alloc();

    void* ptr = std::malloc(size);
    if (!ptr) throw std::bad_alloc();

    // (AR) تصفير الذاكرة لمنع تسرب البيانات السابقة
    // (EN) zero-initialize to prevent stale-data leaks
    std::memset(ptr, 0, size);
    return ptr;
}

void* SafeAllocator::safeArrayAlloc(std::size_t count, std::size_t elementSize) {
    std::size_t totalSize = 0;
    if (!BoundsChecker::checkSizeMultiply(count, elementSize, totalSize)) {
        throw std::bad_alloc();
    }
    return safeAlloc(totalSize);
}

void* SafeAllocator::safeRealloc(void* ptr, std::size_t oldSize, std::size_t newSize) {
    if (newSize == 0) {
        std::free(ptr);
        return nullptr;
    }
    if (!BoundsChecker::checkAllocationSize(newSize)) throw std::bad_alloc();

    void* newPtr = std::realloc(ptr, newSize);
    if (!newPtr) throw std::bad_alloc();

    // (AR) تصفير الجزء المضاف فقط
    // (EN) zero-fill the new tail only
    if (newSize > oldSize) {
        std::memset(static_cast<char*>(newPtr) + oldSize, 0, newSize - oldSize);
    }
    return newPtr;
}

void SafeAllocator::safeFree(void* ptr, std::size_t size) {
    if (ptr) {
        // (AR) تصفير قبل التحرير لمنع تسرب المعلومات الحساسة
        // (EN) zero-fill before free to prevent info leak
        std::memset(ptr, 0, size);
        std::free(ptr);
    }
}

// ----------------------------------------------------------------------------
// (AR) AllocationTracker (singleton)
// (EN) AllocationTracker (singleton)
// ----------------------------------------------------------------------------

SafeAllocator::AllocationTracker& SafeAllocator::AllocationTracker::instance() {
    static AllocationTracker tracker;
    return tracker;
}

void* SafeAllocator::AllocationTracker::trackAlloc(std::size_t size,
                                                   const char* file, int line) {
    void* ptr = SafeAllocator::safeAlloc(size);
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_.push_back({ptr, size, file, line, false});
    return ptr;
}

void SafeAllocator::AllocationTracker::trackFree(void* ptr) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& alloc : allocations_) {
        if (alloc.ptr == ptr && !alloc.freed) {
            SafeAllocator::safeFree(ptr, alloc.size);
            alloc.freed = true;
            return;
        }
    }
    throw std::runtime_error("Attempted to free unallocated memory");
}

void SafeAllocator::AllocationTracker::reportLeaks() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& alloc : allocations_) {
        if (!alloc.freed) {
            std::cerr << "Memory leak: " << alloc.size << " bytes at "
                      << (alloc.file ? alloc.file : "<unknown>") << ":" << alloc.line
                      << std::endl;
        }
    }
}

std::size_t SafeAllocator::AllocationTracker::getLeakCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<std::size_t>(
        std::count_if(allocations_.begin(), allocations_.end(),
                      [](const AllocationInfo& a) { return !a.freed; }));
}

}  // namespace Security
}  // namespace Sad
