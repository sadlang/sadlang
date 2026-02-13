// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file constraint_cache.hpp
 * @brief ذاكرة تخزين مؤقت للقيود - تحسين أداء حل القيود
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *              ذاكرة القيود في لغة ص (Constraint Cache)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على نظام تخزين مؤقت لنتائج حل القيود.
 * يتجنب إعادة حل القيود المتكررة أو المشابهة.
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                    استراتيجية التخزين المؤقت
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * 1. التجزئة (Hashing):
 *    - كل قيد يُحسب له تجزئة فريدة
 *    - القيود المتطابقة لها نفس التجزئة
 * 
 * 2. التخزين (Caching):
 *    - تخزين نتائج الحل الناجحة
 *    - تخزين معلومات الفشل لتجنب المحاولات المتكررة
 * 
 * 3. الإبطال (Invalidation):
 *    - إبطال الذاكرة عند تغير السياق
 *    - استخدام TTL للإدخالات القديمة
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_TYPES_CONSTRAINT_CACHE_HPP
#define SAD_TYPES_CONSTRAINT_CACHE_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <optional>

namespace Sad {
namespace TypeSystem {

// التصريحات المسبقة
class Constraint;
class Substitution;
// TypePtr معرّف في type.h كـ using alias

// ═══════════════════════════════════════════════════════════════════════════
//                    إعدادات الذاكرة المؤقتة / Cache Configuration
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct ConstraintCacheConfig
 * @brief إعدادات ذاكرة القيود
 */
struct ConstraintCacheConfig {
    size_t maxEntries = 10000;          // الحد الأقصى للإدخالات
    size_t maxMemoryMB = 100;           // الحد الأقصى للذاكرة (ميجابايت)
    std::chrono::seconds ttl{3600};     // مدة الصلاحية (ساعة)
    bool enableStatistics = true;       // تفعيل الإحصائيات
    bool persistToDisk = false;         // حفظ على القرص
    std::string cacheDir = ".sad_cache"; // مجلد التخزين
    
    ConstraintCacheConfig() = default;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    إدخال الذاكرة المؤقتة / Cache Entry
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct CacheEntry
 * @brief إدخال في ذاكرة القيود
 */
struct CacheEntry {
    bool success = false;                           // هل الحل نجح؟
    std::shared_ptr<Substitution> substitution;     // الاستبدال الناتج
    std::string errorMessage;                       // رسالة الخطأ (إن فشل)
    std::chrono::steady_clock::time_point timestamp; // وقت الإنشاء
    size_t hitCount = 0;                            // عدد مرات الاستخدام
    size_t constraintHash = 0;                      // تجزئة القيد
    
    CacheEntry() : timestamp(std::chrono::steady_clock::now()) {}
    
    bool isExpired(std::chrono::seconds ttl) const {
        auto now = std::chrono::steady_clock::now();
        return now - timestamp > ttl;
    }
    
    void touch() {
        hitCount++;
        timestamp = std::chrono::steady_clock::now();
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    إحصائيات الذاكرة / Cache Statistics
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct CacheStatistics
 * @brief إحصائيات استخدام الذاكرة المؤقتة
 */
struct CacheStatistics {
    size_t totalLookups = 0;        // إجمالي عمليات البحث
    size_t cacheHits = 0;           // عدد مرات الإصابة
    size_t cacheMisses = 0;         // عدد مرات الإخفاق
    size_t evictions = 0;           // عدد مرات الإزالة
    size_t currentEntries = 0;      // الإدخالات الحالية
    size_t memoryUsedBytes = 0;     // الذاكرة المستخدمة
    
    double hitRate() const {
        if (totalLookups == 0) return 0.0;
        return static_cast<double>(cacheHits) / totalLookups * 100.0;
    }
    
    std::string toArabicString() const {
        std::string result;
        result += "إحصائيات ذاكرة القيود:\n";
        result += "  - إجمالي البحث: " + std::to_string(totalLookups) + "\n";
        result += "  - الإصابات: " + std::to_string(cacheHits) + "\n";
        result += "  - الإخفاقات: " + std::to_string(cacheMisses) + "\n";
        result += "  - نسبة الإصابة: " + std::to_string(hitRate()) + "%\n";
        result += "  - الإدخالات: " + std::to_string(currentEntries) + "\n";
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    ذاكرة القيود / Constraint Cache
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class ConstraintCache
 * @brief ذاكرة تخزين مؤقت لنتائج حل القيود
 */
class ConstraintCache {
public:
    explicit ConstraintCache(const ConstraintCacheConfig& config = ConstraintCacheConfig());
    ~ConstraintCache();
    
    // ══════════════════════════════════════════════════════════════════════
    //                    البحث والتخزين / Lookup & Store
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief البحث عن نتيجة مخزنة لقيد
     * @param constraint القيد للبحث عنه
     * @return الإدخال المخزن أو nullopt
     */
    std::optional<CacheEntry> lookup(const Constraint& constraint);
    
    /**
     * @brief البحث بالتجزئة مباشرة
     */
    std::optional<CacheEntry> lookupByHash(size_t hash);
    
    /**
     * @brief تخزين نتيجة حل ناجحة
     */
    void storeSuccess(const Constraint& constraint, 
                      std::shared_ptr<Substitution> substitution);
    
    /**
     * @brief تخزين نتيجة حل فاشلة
     */
    void storeFailure(const Constraint& constraint, 
                      const std::string& errorMessage);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    إدارة الذاكرة / Memory Management
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief مسح كل الذاكرة المؤقتة
     */
    void clear();
    
    /**
     * @brief إزالة الإدخالات المنتهية الصلاحية
     */
    void evictExpired();
    
    /**
     * @brief إزالة الإدخالات الأقل استخداماً
     */
    void evictLRU(size_t count);
    
    /**
     * @brief تقليص حجم الذاكرة
     */
    void shrinkToFit();
    
    // ══════════════════════════════════════════════════════════════════════
    //                    التجزئة / Hashing
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief حساب تجزئة قيد
     */
    static size_t hashConstraint(const Constraint& constraint);
    
    /**
     * @brief حساب تجزئة مجموعة قيود
     */
    static size_t hashConstraintSet(const std::vector<Constraint>& constraints);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الإحصائيات / Statistics
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief الحصول على الإحصائيات
     */
    CacheStatistics getStatistics() const;
    
    /**
     * @brief إعادة تعيين الإحصائيات
     */
    void resetStatistics();
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الحفظ والتحميل / Persistence
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief حفظ الذاكرة على القرص
     */
    bool saveToDisk() const;
    
    /**
     * @brief تحميل الذاكرة من القرص
     */
    bool loadFromDisk();
    
    // ══════════════════════════════════════════════════════════════════════
    //                    المعلومات / Information
    // ══════════════════════════════════════════════════════════════════════
    
    size_t size() const { return cache_.size(); }
    bool isEmpty() const { return cache_.empty(); }
    const ConstraintCacheConfig& getConfig() const { return config_; }

private:
    ConstraintCacheConfig config_;
    std::unordered_map<size_t, CacheEntry> cache_;
    mutable std::mutex mutex_;
    CacheStatistics stats_;
    
    /**
     * @brief التحقق من الحاجة للإزالة
     */
    void checkAndEvict();
    
    /**
     * @brief تقدير حجم الإدخال بالبايت
     */
    size_t estimateEntrySize(const CacheEntry& entry) const;
};

// ═══════════════════════════════════════════════════════════════════════════
//                    ذاكرة عامة مشتركة / Global Shared Cache
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief الحصول على ذاكرة القيود العامة
 */
ConstraintCache& getGlobalConstraintCache();

/**
 * @brief تعيين ذاكرة القيود العامة
 */
void setGlobalConstraintCache(std::shared_ptr<ConstraintCache> cache);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_CONSTRAINT_CACHE_HPP
