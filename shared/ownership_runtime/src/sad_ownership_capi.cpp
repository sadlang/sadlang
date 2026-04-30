// ═══════════════════════════════════════════════════════════════════════════════
// (AR) sad_ownership_capi.cpp — تنفيذ طبقة C-ABI لـ DEF-001.
//
// (AR) كل دالة مجرد جسر رفيع نحو Sad::Ownership::Runtime::OwnershipTracker.
//      أمان الخيوط محقَّق داخل المتتبع (mutex). C-ABI لا تضيف أي state.
//
// (EN) Thin bridge from C-ABI to the C++ singleton tracker. Thread-safety
//      lives in the tracker; this file is stateless.
// ═══════════════════════════════════════════════════════════════════════════════

#include "ownership/runtime/sad_ownership_capi.h"
#include "ownership/runtime/ownership_tracker.h"

#include <string>

using ::Sad::Ownership::Runtime::OwnershipTracker;

extern "C" {

void sad_ownership_register(void* ptr, size_t size, const char* allocator) {
    // (AR) نُحوّل NULL إلى نص فارغ للحفاظ على دلالات std::string.
    // (EN) Convert NULL allocator to empty string for std::string semantics.
    OwnershipTracker::instance().registerAllocation(
        ptr, size, allocator ? std::string(allocator) : std::string());
}

int sad_ownership_deregister(void* ptr, const char* deallocator) {
    return OwnershipTracker::instance().registerDeallocation(
        ptr, deallocator ? std::string(deallocator) : std::string()) ? 1 : 0;
}

void sad_ownership_transfer(void* ptr, const char* new_owner) {
    OwnershipTracker::instance().transferOwnership(
        ptr, new_owner ? std::string(new_owner) : std::string());
}

int sad_ownership_is_owned(void* ptr) {
    return OwnershipTracker::instance().isOwned(ptr) ? 1 : 0;
}

int sad_ownership_is_freed(void* ptr) {
    return OwnershipTracker::instance().isFreed(ptr) ? 1 : 0;
}

void sad_ownership_print_leaks(void) {
    OwnershipTracker::instance().printLeakReport();
}

void sad_ownership_clear(void) {
    OwnershipTracker::instance().clear();
}

} // extern "C"
