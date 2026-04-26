// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file constraint_cache.cpp
 * @brief تنفيذ ذاكرة تخزين مؤقت للقيود
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *            تنفيذ ذاكرة القيود في لغة ص (Constraint Cache Implementation)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "types/constraint_cache.hpp"
#include "types/constraint_solver.hpp"
#include <algorithm>
#include <fstream>

namespace Sad {
namespace TypeSystem {

// ═══════════════════════════════════════════════════════════════════════════
//                    الذاكرة العامة / Global Cache
// ═══════════════════════════════════════════════════════════════════════════

static std::shared_ptr<ConstraintCache> s_globalCache = nullptr;

ConstraintCache& getGlobalConstraintCache() {
    if (!s_globalCache) {
        s_globalCache = std::make_shared<ConstraintCache>();
    }
    return *s_globalCache;
}

void setGlobalConstraintCache(std::shared_ptr<ConstraintCache> cache) {
    s_globalCache = cache;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    المُنشئ والمُدمر / Constructor & Destructor
// ═══════════════════════════════════════════════════════════════════════════

ConstraintCache::ConstraintCache(const ConstraintCacheConfig& config)
    : config_(config)
    , cache_()
    , mutex_()
    , stats_()
{
    // تحميل من القرص إذا كان مفعلاً
    if (config_.persistToDisk) {
        loadFromDisk();
    }
}

ConstraintCache::~ConstraintCache() {
    // حفظ على القرص إذا كان مفعلاً
    if (config_.persistToDisk) {
        saveToDisk();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    البحث / Lookup
// ═══════════════════════════════════════════════════════════════════════════

std::optional<CacheEntry> ConstraintCache::lookup(const Constraint& constraint) {
    size_t hash = hashConstraint(constraint);
    return lookupByHash(hash);
}

std::optional<CacheEntry> ConstraintCache::lookupByHash(size_t hash) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (config_.enableStatistics) {
        stats_.totalLookups++;
    }
    
    auto it = cache_.find(hash);
    if (it == cache_.end()) {
        if (config_.enableStatistics) {
            stats_.cacheMisses++;
        }
        return std::nullopt;
    }
    
    // التحقق من انتهاء الصلاحية
    if (it->second.isExpired(config_.ttl)) {
        cache_.erase(it);
        if (config_.enableStatistics) {
            stats_.cacheMisses++;
            stats_.evictions++;
            stats_.currentEntries = cache_.size();
        }
        return std::nullopt;
    }
    
    // تحديث الإحصائيات
    it->second.touch();
    
    if (config_.enableStatistics) {
        stats_.cacheHits++;
    }
    
    return it->second;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    التخزين / Storage
// ═══════════════════════════════════════════════════════════════════════════

void ConstraintCache::storeSuccess(const Constraint& constraint,
                                    std::shared_ptr<Substitution> substitution) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    checkAndEvict();
    
    size_t hash = hashConstraint(constraint);
    
    CacheEntry entry;
    entry.success = true;
    entry.substitution = substitution;
    entry.constraintHash = hash;
    
    cache_[hash] = entry;
    
    if (config_.enableStatistics) {
        stats_.currentEntries = cache_.size();
        stats_.memoryUsedBytes += estimateEntrySize(entry);
    }
}

void ConstraintCache::storeFailure(const Constraint& constraint,
                                    const std::string& errorMessage) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    checkAndEvict();
    
    size_t hash = hashConstraint(constraint);
    
    CacheEntry entry;
    entry.success = false;
    entry.errorMessage = errorMessage;
    entry.constraintHash = hash;
    
    cache_[hash] = entry;
    
    if (config_.enableStatistics) {
        stats_.currentEntries = cache_.size();
        stats_.memoryUsedBytes += estimateEntrySize(entry);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    إدارة الذاكرة / Memory Management
// ═══════════════════════════════════════════════════════════════════════════

void ConstraintCache::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
    
    if (config_.enableStatistics) {
        stats_.currentEntries = 0;
        stats_.memoryUsedBytes = 0;
    }
}

void ConstraintCache::evictExpired() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = cache_.begin();
    while (it != cache_.end()) {
        if (it->second.isExpired(config_.ttl)) {
            if (config_.enableStatistics) {
                stats_.evictions++;
                stats_.memoryUsedBytes -= estimateEntrySize(it->second);
            }
            it = cache_.erase(it);
        } else {
            ++it;
        }
    }
    
    if (config_.enableStatistics) {
        stats_.currentEntries = cache_.size();
    }
}

void ConstraintCache::evictLRU(size_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (cache_.empty() || count == 0) return;
    
    // جمع الإدخالات مع عدد الاستخدام
    std::vector<std::pair<size_t, size_t>> entries; // (hash, hitCount)
    for (const auto& [hash, entry] : cache_) {
        entries.emplace_back(hash, entry.hitCount);
    }
    
    // ترتيب حسب الأقل استخداماً
    std::sort(entries.begin(), entries.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    // إزالة count إدخالات
    size_t toRemove = std::min(count, entries.size());
    for (size_t i = 0; i < toRemove; ++i) {
        auto it = cache_.find(entries[i].first);
        if (it != cache_.end()) {
            if (config_.enableStatistics) {
                stats_.evictions++;
                stats_.memoryUsedBytes -= estimateEntrySize(it->second);
            }
            cache_.erase(it);
        }
    }
    
    if (config_.enableStatistics) {
        stats_.currentEntries = cache_.size();
    }
}

void ConstraintCache::shrinkToFit() {
    // إزالة المنتهية الصلاحية أولاً
    evictExpired();
    
    // إذا لا زال هناك إدخالات كثيرة، إزالة الأقل استخداماً
    if (cache_.size() > config_.maxEntries) {
        evictLRU(cache_.size() - config_.maxEntries);
    }
}

void ConstraintCache::checkAndEvict() {
    // التحقق من الحد الأقصى للإدخالات
    if (cache_.size() >= config_.maxEntries) {
        // إزالة 10% من الإدخالات الأقل استخداماً
        evictLRU(config_.maxEntries / 10);
    }
    
    // التحقق من حد الذاكرة
    size_t maxBytes = config_.maxMemoryMB * 1024 * 1024;
    if (stats_.memoryUsedBytes >= maxBytes) {
        evictLRU(config_.maxEntries / 10);
    }
}

size_t ConstraintCache::estimateEntrySize(const CacheEntry& entry) const {
    size_t size = sizeof(CacheEntry);
    size += entry.errorMessage.capacity();
    // تقدير حجم الاستبدال
    if (entry.substitution) {
        size += 256; // تقدير تقريبي
    }
    return size;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    التجزئة / Hashing
// ═══════════════════════════════════════════════════════════════════════════

size_t ConstraintCache::hashConstraint(const Constraint& constraint) {
    std::hash<std::string> stringHasher;
    size_t hash = 0;
    
    // دمج التجزئة باستخدام خوارزمية مشابهة لـ boost::hash_combine
    auto hashCombine = [&hash](size_t value) {
        hash ^= value + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    };
    
    // تجزئة نوع القيد
    hashCombine(static_cast<size_t>(constraint.getKind()));
    
    // تجزئة الأنواع المعنية
    if (constraint.getType1()) {
        hashCombine(stringHasher(constraint.getType1()->toString()));
    }
    
    if (constraint.getType2()) {
        hashCombine(stringHasher(constraint.getType2()->toString()));
    }
    
    return hash;
}

size_t ConstraintCache::hashConstraintSet(const std::vector<Constraint>& constraints) {
    size_t hash = 0;
    
    auto hashCombine = [&hash](size_t value) {
        hash ^= value + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    };
    
    for (const auto& constraint : constraints) {
        hashCombine(hashConstraint(constraint));
    }
    
    return hash;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الإحصائيات / Statistics
// ═══════════════════════════════════════════════════════════════════════════

CacheStatistics ConstraintCache::getStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return stats_;
}

void ConstraintCache::resetStatistics() {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_ = CacheStatistics();
    stats_.currentEntries = cache_.size();
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الحفظ والتحميل / Persistence
// ═══════════════════════════════════════════════════════════════════════════

bool ConstraintCache::saveToDisk() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string filePath = config_.cacheDir + "/constraint_cache.bin";
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // كتابة عدد الإدخالات
    size_t count = cache_.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    // كتابة كل إدخال
    for (const auto& [hash, entry] : cache_) {
        file.write(reinterpret_cast<const char*>(&hash), sizeof(hash));
        file.write(reinterpret_cast<const char*>(&entry.success), sizeof(entry.success));
        
        size_t msgLen = entry.errorMessage.size();
        file.write(reinterpret_cast<const char*>(&msgLen), sizeof(msgLen));
        file.write(entry.errorMessage.data(), msgLen);
        
        file.write(reinterpret_cast<const char*>(&entry.hitCount), sizeof(entry.hitCount));
    }
    
    return file.good();
}

bool ConstraintCache::loadFromDisk() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string filePath = config_.cacheDir + "/constraint_cache.bin";
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // قراءة عدد الإدخالات
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    // (AR) حماية من ملفات الكاش التالفة أو الخبيثة
    // (EN) Guard against corrupted or malicious cache files
    constexpr size_t MAX_CACHE_ENTRIES = 10'000'000;  // 10M max
    constexpr size_t MAX_MSG_LENGTH = 10'000;         // 10KB max per message
    
    if (!file.good() || count > MAX_CACHE_ENTRIES) {
        return false;
    }
    
    // قراءة كل إدخال
    for (size_t i = 0; i < count; ++i) {
        size_t hash;
        CacheEntry entry;
        
        file.read(reinterpret_cast<char*>(&hash), sizeof(hash));
        file.read(reinterpret_cast<char*>(&entry.success), sizeof(entry.success));
        
        size_t msgLen;
        file.read(reinterpret_cast<char*>(&msgLen), sizeof(msgLen));
        
        // (AR) التحقق من طول الرسالة وحالة الملف
        // (EN) Validate message length and file state
        if (!file.good() || msgLen > MAX_MSG_LENGTH) {
            return false;
        }
        
        entry.errorMessage.resize(msgLen);
        file.read(entry.errorMessage.data(), msgLen);
        
        file.read(reinterpret_cast<char*>(&entry.hitCount), sizeof(entry.hitCount));
        
        if (!file.good()) {
            return false;
        }
        
        entry.constraintHash = hash;
        entry.timestamp = std::chrono::steady_clock::now();
        
        cache_[hash] = entry;
    }
    
    if (config_.enableStatistics) {
        stats_.currentEntries = cache_.size();
    }
    
    return file.good();
}

} // namespace TypeSystem
} // namespace Sad
