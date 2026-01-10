/**
 * @file module_cache.h
 * @brief (AR) نظام التخزين المؤقت للوحدات
 *        (EN) Module caching system
 * 
 * @author Sad Language Development Team
 * @date December 2025
 * @version 2.0.0 - Phase 2
 */

#ifndef SAD_MODULE_CACHE_H
#define SAD_MODULE_CACHE_H

#include "module.h"
#include <map>
#include <memory>
#include <chrono>

namespace Sad {
namespace Modules {

/**
 * @struct CacheEntry
 * @brief (AR) إدخال في الكاش / (EN) Cache entry
 */
struct CacheEntry {
    std::unique_ptr<Module> module;                     ///< (AR) الوحدة المُخزنة / (EN) Cached module
    std::chrono::system_clock::time_point cacheTime;    ///< (AR) وقت التخزين / (EN) Cache time
    size_t hits;                                        ///< (AR) عدد الاستخدامات / (EN) Hit count
    
    /**
     * @brief (AR) هل الإدخال صالح؟ / (EN) Is entry valid?
     */
    bool isValid() const;
};

/**
 * @class ModuleCache
 * @brief (AR) مدير التخزين المؤقت للوحدات / (EN) Module cache manager
 * 
 * @details
 * (AR) Singleton يدير الكاش للوحدات المُحلَّلة لتسريع التحميل
 * (EN) Singleton managing cache for parsed modules to speed up loading
 */
class ModuleCache {
public:
    /**
     * @brief (AR) الحصول على Instance / (EN) Get instance
     */
    static ModuleCache& getInstance();
    
    // Delete copy
    ModuleCache(const ModuleCache&) = delete;
    ModuleCache& operator=(const ModuleCache&) = delete;
    
    /**
     * @brief (AR) الحصول على وحدة من الكاش / (EN) Get module from cache
     * @param moduleName (AR) اسم الوحدة / (EN) Module name
     * @return Module* أو nullptr
     */
    Module* get(const std::string& moduleName);
    
    /**
     * @brief (AR) إضافة وحدة للكاش / (EN) Add module to cache
     */
    void put(const std::string& moduleName, std::unique_ptr<Module> module);
    
    /**
     * @brief (AR) تحقق من وجود الوحدة / (EN) Check if module exists
     */
    bool has(const std::string& moduleName) const;
    
    /**
     * @brief (AR) تحقق من صحة الكاش / (EN) Check if cache is valid
     */
    bool isValid(const std::string& moduleName) const;
    
    /**
     * @brief (AR) مسح الكاش / (EN) Clear cache
     */
    void clear();
    
    /**
     * @brief (AR) مسح وحدة معينة / (EN) Clear specific module
     */
    void clearModule(const std::string& moduleName);
    
    /**
     * @brief (AR) إحصائيات الكاش / (EN) Cache statistics
     */
    struct Stats {
        size_t hits;              ///< (AR) النجاحات / (EN) Cache hits
        size_t misses;            ///< (AR) الإخفاقات / (EN) Cache misses
        size_t evictions;         ///< (AR) الحذف / (EN) Evictions
        size_t memoryUsage;       ///< (AR) الذاكرة المستخدمة / (EN) Memory usage
        
        double hitRate() const {
            if (hits + misses == 0) return 0.0;
            return static_cast<double>(hits) / (hits + misses);
        }
    };
    
    Stats getStats() const { return stats_; }
    
    /**
     * @brief (AR) طباعة الإحصائيات / (EN) Print statistics
     */
    void printStats() const;

private:
    ModuleCache();
    
    std::map<std::string, CacheEntry> cache_;  ///< (AR) الكاش / (EN) Cache storage
    Stats stats_;                              ///< (AR) الإحصائيات / (EN) Statistics
    
    void updateHit(const std::string& moduleName);
    void updateMiss();
};

} // namespace Modules
} // namespace Sad

#endif // SAD_MODULE_CACHE_H
