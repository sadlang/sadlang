// ===================================================================
// صNet - شبكة توزيع المحتوى اللامركزية (صCDN)
// المرحلة 53: CDN بدون خوادم مركزية
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <set>
#include <algorithm>
#include <chrono>

namespace sad::network {

/// سياسة التخزين المؤقت
enum class CachePolicy : uint8_t {
    LRU,            // الأقل استخداماً مؤخراً
    LFU,            // الأقل استخداماً إجمالاً
    TTL_BASED,      // بناءً على مدة الصلاحية
    POPULARITY      // بناءً على الشعبية
};

/// معلومات محتوى مُخزّن مؤقتاً
struct CachedContent {
    std::string content_id;            // معرّف المحتوى
    std::vector<uint8_t> data;         // البيانات
    uint64_t size;                     // الحجم بالبايت
    uint64_t cached_at;                // وقت التخزين
    uint64_t last_accessed;            // آخر وصول
    uint64_t access_count;             // عدد مرات الوصول
    uint32_t ttl_seconds;              // مدة الصلاحية
    std::string origin_node;           // العُقدة المصدر
};

/// إحصائيات عُقدة CDN
struct CDNNodeStats {
    uint64_t total_served;             // إجمالي البايتات المُقدّمة
    uint64_t cache_hits;               // إصابات الذاكرة المؤقتة
    uint64_t cache_misses;             // أخطاء الذاكرة المؤقتة
    uint64_t bandwidth_used;           // عرض النطاق المُستخدم
    double hit_ratio;                  // نسبة الإصابة
    uint32_t active_connections;       // الاتصالات النشطة
};

/// ذاكرة مؤقتة ذكية مع سياسة إخلاء
class SmartCache {
    std::map<std::string, CachedContent> cache_;   // المحتوى المُخزّن
    CachePolicy policy_;                           // سياسة الإخلاء
    uint64_t max_size_;                            // الحد الأقصى
    uint64_t current_size_;                        // الحجم الحالي

public:
    SmartCache(uint64_t max_bytes, CachePolicy policy = CachePolicy::LRU)
        : policy_(policy), max_size_(max_bytes), current_size_(0) {}

    /// تخزين محتوى في الذاكرة المؤقتة
    bool put(const std::string& id, const std::vector<uint8_t>& data,
             uint32_t ttl = 3600) {
        while (current_size_ + data.size() > max_size_) {
            if (!evict_one()) return false;
        }
        CachedContent content;
        content.content_id = id;
        content.data = data;
        content.size = data.size();
        content.ttl_seconds = ttl;
        content.access_count = 0;
        auto now = std::chrono::system_clock::now()
            .time_since_epoch().count();
        content.cached_at = now;
        content.last_accessed = now;
        cache_[id] = std::move(content);
        current_size_ += data.size();
        return true;
    }

    /// استرجاع محتوى من الذاكرة المؤقتة
    const std::vector<uint8_t>* get(const std::string& id) {
        auto it = cache_.find(id);
        if (it == cache_.end()) return nullptr;
        it->second.access_count++;
        it->second.last_accessed = std::chrono::system_clock::now()
            .time_since_epoch().count();
        return &it->second.data;
    }

    bool contains(const std::string& id) const {
        return cache_.count(id) > 0;
    }

    double usage_ratio() const {
        return (max_size_ > 0) ? (double)current_size_ / max_size_ : 0.0;
    }

private:
    /// إخلاء عنصر واحد حسب السياسة
    bool evict_one() {
        if (cache_.empty()) return false;
        auto victim = cache_.begin();
        if (policy_ == CachePolicy::LRU) {
            for (auto it = cache_.begin(); it != cache_.end(); ++it) {
                if (it->second.last_accessed < victim->second.last_accessed) {
                    victim = it;
                }
            }
        } else if (policy_ == CachePolicy::LFU) {
            for (auto it = cache_.begin(); it != cache_.end(); ++it) {
                if (it->second.access_count < victim->second.access_count) {
                    victim = it;
                }
            }
        }
        current_size_ -= victim->second.size;
        cache_.erase(victim);
        return true;
    }
};

/// عُقدة CDN لامركزية
class CDNNode {
    SmartCache cache_;                 // الذاكرة المؤقتة
    CDNNodeStats stats_;               // الإحصائيات
    std::string node_id_;              // معرّف العُقدة
    std::set<std::string> nearby_;     // العُقد القريبة جغرافياً

public:
    CDNNode(const std::string& id, uint64_t cache_size)
        : cache_(cache_size), node_id_(id) {
        stats_ = {};
    }

    /// معالجة طلب محتوى
    std::vector<uint8_t> serve(const std::string& content_id) {
        // البحث في الذاكرة المؤقتة أولاً
        auto* cached = cache_.get(content_id);
        if (cached) {
            stats_.cache_hits++;
            stats_.total_served += cached->size();
            return *cached;
        }
        stats_.cache_misses++;
        // طلب من العُقد القريبة أو المصدر
        return fetch_from_network(content_id);
    }

    /// تحديث الإحصائيات
    void update_stats() {
        uint64_t total = stats_.cache_hits + stats_.cache_misses;
        stats_.hit_ratio = (total > 0)
            ? (double)stats_.cache_hits / total : 0.0;
    }

    const CDNNodeStats& stats() const { return stats_; }

private:
    std::vector<uint8_t> fetch_from_network(const std::string& content_id) {
        // جلب المحتوى من الشبكة وتخزينه محلياً
        std::vector<uint8_t> data;
        // البحث في العُقد القريبة ثم DHT
        if (!data.empty()) {
            cache_.put(content_id, data);
        }
        return data;
    }
};

} // namespace sad::network
