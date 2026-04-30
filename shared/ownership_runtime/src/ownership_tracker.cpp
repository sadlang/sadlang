// ═══════════════════════════════════════════════════════════════════════════════
// (AR) ownership_tracker.cpp — تنفيذ متتبع الملكية الموحَّد (DEF-001).
//
// (AR) السلوك مطابق للنسخة الأصلية في compiler/src/backend/ffi_wrapper.cpp.
//      الفارق الوحيد: قفل خيوط داخلي حول allocations_.
//
// (EN) Implementation of the unified runtime ownership tracker. Behaviour
//      identical to the original; only addition is internal mutex locking.
// ═══════════════════════════════════════════════════════════════════════════════

#include "ownership/runtime/ownership_tracker.h"

#include <iostream>

namespace Sad {
namespace Ownership {
namespace Runtime {

OwnershipTracker& OwnershipTracker::instance() {
    static OwnershipTracker tracker;
    return tracker;
}

void OwnershipTracker::registerAllocation(void* ptr, std::size_t size,
                                          const std::string& allocator,
                                          const std::string& file, int line) {
    if (ptr == nullptr) return;

    std::lock_guard<std::mutex> lock(allocations_mutex_);

    AllocationInfo info;
    info.ptr       = ptr;
    info.size      = size;
    info.allocator = allocator;
    info.file      = file;
    info.line      = line;
    info.policy    = OwnershipPolicy::OWNED;
    info.isFreed   = false;

    allocations_[ptr] = info;
}

bool OwnershipTracker::registerDeallocation(void* ptr, const std::string& /*deallocator*/) {
    if (ptr == nullptr) return true;

    std::lock_guard<std::mutex> lock(allocations_mutex_);

    auto it = allocations_.find(ptr);
    if (it == allocations_.end()) {
        // (AR) تحرير مؤشر غير مسجَّل — قد يكون double-free أو خارج التتبع.
        // (EN) Freeing unregistered pointer — possibly double-free or untracked.
        std::cerr << "(AR) تحذير: تحرير مؤشر غير مسجل\n";
        return false;
    }

    if (it->second.isFreed) {
        // (AR) تحرير مزدوج صريح. / (EN) Explicit double-free.
        std::cerr << "(AR) خطأ: تحرير مزدوج!\n";
        return false;
    }

    it->second.isFreed = true;
    return true;
}

void OwnershipTracker::transferOwnership(void* ptr, const std::string& /*newOwner*/) {
    std::lock_guard<std::mutex> lock(allocations_mutex_);
    auto it = allocations_.find(ptr);
    if (it != allocations_.end()) {
        it->second.policy = OwnershipPolicy::TRANSFERRED;
    }
}

bool OwnershipTracker::isOwned(void* ptr) const {
    std::lock_guard<std::mutex> lock(allocations_mutex_);
    auto it = allocations_.find(ptr);
    if (it != allocations_.end()) {
        return it->second.policy == OwnershipPolicy::OWNED && !it->second.isFreed;
    }
    return false;
}

bool OwnershipTracker::isFreed(void* ptr) const {
    std::lock_guard<std::mutex> lock(allocations_mutex_);
    auto it = allocations_.find(ptr);
    if (it != allocations_.end()) {
        return it->second.isFreed;
    }
    return false;
}

std::optional<AllocationInfo>
OwnershipTracker::getAllocationInfo(void* ptr) const {
    std::lock_guard<std::mutex> lock(allocations_mutex_);
    auto it = allocations_.find(ptr);
    if (it != allocations_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<AllocationInfo>
OwnershipTracker::getActiveAllocations() const {
    std::lock_guard<std::mutex> lock(allocations_mutex_);
    std::vector<AllocationInfo> active;
    for (const auto& [ptr, info] : allocations_) {
        if (!info.isFreed && info.policy == OwnershipPolicy::OWNED) {
            active.push_back(info);
        }
    }
    return active;
}

void OwnershipTracker::printLeakReport() const {
    // (AR) ننسخ النتائج تحت القفل ثم نطبع خارجه (تجنّب re-entrancy عبر I/O).
    // (EN) Snapshot under lock, print outside (avoid I/O re-entrancy risk).
    std::vector<AllocationInfo> active = getActiveAllocations();

    if (active.empty()) {
        std::cout << "(AR) لا يوجد تسرب ذاكرة\n";
        std::cout << "(EN) No memory leaks detected\n";
        return;
    }

    std::cout << "\n=== تقرير تسرب الذاكرة / Memory Leak Report ===\n\n";

    std::size_t totalBytes = 0;
    for (const auto& info : active) {
        std::cout << "تسرب / Leak:\n";
        std::cout << "  الحجم / Size: " << info.size << " bytes\n";
        std::cout << "  المخصص / Allocator: " << info.allocator << "\n";
        if (!info.file.empty()) {
            std::cout << "  الملف / File: " << info.file << ":" << info.line << "\n";
        }
        std::cout << "\n";
        totalBytes += info.size;
    }

    std::cout << "إجمالي / Total: " << active.size()
              << " تسربات leaks, " << totalBytes << " bytes\n";
}

void OwnershipTracker::clear() {
    std::lock_guard<std::mutex> lock(allocations_mutex_);
    allocations_.clear();
}

} // namespace Runtime
} // namespace Ownership
} // namespace Sad
