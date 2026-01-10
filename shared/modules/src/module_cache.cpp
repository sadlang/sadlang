/**
 * @file module_cache.cpp
 * @brief (AR) تنفيذ نظام التخزين المؤقت
 *        (EN) Implementation of caching system
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "module_cache.h"
#include <iostream>

namespace Sad {
namespace Modules {

// =========================================================================
// CacheEntry Implementation
// =========================================================================

bool CacheEntry::isValid() const {
    if (!module) {
        return false;
    }
    
    // التحقق من صلاحية الوحدة
    // Check module validity
    return module->isValid();
}

// =========================================================================
// ModuleCache Implementation
// =========================================================================

ModuleCache& ModuleCache::getInstance() {
    static ModuleCache instance;
    return instance;
}

ModuleCache::ModuleCache() : cache_{}, stats_{0, 0, 0, 0} {
    // لا حاجة لتهيئة خاصة
    // No special initialization needed
}

Module* ModuleCache::get(const std::string& moduleName) {
    auto it = cache_.find(moduleName);
    
    if (it != cache_.end()) {
        // وُجد في الكاش / Found in cache
        if (it->second.isValid()) {
            // صالح / Valid
            updateHit(moduleName);
            return it->second.module.get();
        } else {
            // غير صالح - حذف / Invalid - remove
            cache_.erase(it);
            updateMiss();
            return nullptr;
        }
    }
    
    // غير موجود / Not found
    updateMiss();
    return nullptr;
}

void ModuleCache::put(const std::string& moduleName, std::unique_ptr<Module> module) {
    if (!module) {
        return;
    }
    
    CacheEntry entry;
    entry.module = std::move(module);
    entry.cacheTime = std::chrono::system_clock::now();
    entry.hits = 0;
    
    // إضافة أو استبدال
    // Add or replace
    auto it = cache_.find(moduleName);
    if (it != cache_.end()) {
        // استبدال / Replace
        stats_.evictions++;
    }
    
    cache_[moduleName] = std::move(entry);
    
    // تحديث الذاكرة المستخدمة (تقريبي)
    // Update memory usage (approximate)
    stats_.memoryUsage = cache_.size() * sizeof(CacheEntry);
}

bool ModuleCache::has(const std::string& moduleName) const {
    return cache_.find(moduleName) != cache_.end();
}

bool ModuleCache::isValid(const std::string& moduleName) const {
    auto it = cache_.find(moduleName);
    if (it != cache_.end()) {
        return it->second.isValid();
    }
    return false;
}

void ModuleCache::clear() {
    cache_.clear();
    stats_ = {0, 0, 0, 0};
}

void ModuleCache::clearModule(const std::string& moduleName) {
    auto it = cache_.find(moduleName);
    if (it != cache_.end()) {
        cache_.erase(it);
        stats_.evictions++;
        stats_.memoryUsage = cache_.size() * sizeof(CacheEntry);
    }
}

void ModuleCache::printStats() const {
    std::cout << "\n========================================\n";
    std::cout << "  Module Cache Statistics\n";
    std::cout << "  (إحصائيات الكاش)\n";
    std::cout << "========================================\n";
    std::cout << "Hits (نجاحات): " << stats_.hits << "\n";
    std::cout << "Misses (إخفاقات): " << stats_.misses << "\n";
    std::cout << "Evictions (حذف): " << stats_.evictions << "\n";
    std::cout << "Hit Rate (نسبة النجاح): " 
              << (stats_.hitRate() * 100.0) << "%\n";
    std::cout << "Cached Modules (وحدات مخزنة): " << cache_.size() << "\n";
    std::cout << "Memory Usage (est): " 
              << (stats_.memoryUsage / 1024.0) << " KB\n";
    std::cout << "========================================\n\n";
}

void ModuleCache::updateHit(const std::string& moduleName) {
    stats_.hits++;
    
    auto it = cache_.find(moduleName);
    if (it != cache_.end()) {
        it->second.hits++;
    }
}

void ModuleCache::updateMiss() {
    stats_.misses++;
}

} // namespace Modules
} // namespace Sad
