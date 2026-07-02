// ═══════════════════════════════════════════════════════════════════
// صNet — CDN لامركزي (Decentralized Content Delivery Network)
// (AR) تخزين مؤقت موزع، توجيه حسب الشعبية، نسخ تكيفي
//      عقد تخزين متطوعة، إحصائيات المحتوى
// (EN) Decentralized CDN: distributed caching, popularity routing,
//      adaptive replication, volunteer cache nodes
// المكون: features/network/sadnet (CDN لامركزي)
// المسار: features/network/sadnet/include/sadnet/cdn/cdn.h
// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_CDN_CDN_H
#define SAD_SADNET_CDN_CDN_H

#include "sadnet/core/types.h"
#include "sadnet/crypto/crypto.h"
#include "sadnet/storage/storage.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sad::net::cdn
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::Endpoint;
    using sad::net::NetError;
    using sad::net::NodeId;
    using sad::net::Result;
    using storage::ContentId;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ثوابت CDN
    // ═════════════════════════════════════════════════════════════════════

    constexpr uint32_t DEFAULT_CACHE_SIZE_MB = 256;   // حجم التخزين المؤقت الافتراضي
    constexpr uint32_t MIN_REPLICAS = 3;              // الحد الأدنى من النسخ
    constexpr uint32_t MAX_REPLICAS = 20;             // الحد الأقصى
    constexpr uint64_t POPULARITY_DECAY_MS = 3600000; // تناقص الشعبية كل ساعة
    constexpr uint64_t CACHE_EXPIRY_MS = 86400000;    // انتهاء التخزين بعد يوم

    // ═════════════════════════════════════════════════════════════════════
    // (AR) فهرس المحتوى — معلومات عن محتوى في الشبكة
    // ═════════════════════════════════════════════════════════════════════

    struct ContentEntry
    {
        ContentId content_id;
        std::string name;
        std::string mime_type;
        uint64_t size = 0;
        uint64_t created_at = 0;
        NodeId origin;                   // العقدة الأصلية
        std::vector<NodeId> cache_nodes; // عقد التخزين المؤقت
        uint32_t access_count = 0;       // عدد مرات الوصول
        uint64_t last_accessed = 0;
        double popularity_score = 0.0;

        // (AR) حساب درجة الشعبية مع التناقص الزمني
        void update_popularity()
        {
            uint64_t now = now_ms();
            double age_hours = (double)(now - created_at) / 3600000.0;
            if (age_hours < 0.01)
                age_hours = 0.01;
            popularity_score = (double)access_count / age_hours;
        }

        // (AR) العدد المطلوب من النسخ حسب الشعبية
        uint32_t desired_replicas() const
        {
            if (popularity_score > 100)
                return MAX_REPLICAS;
            if (popularity_score > 50)
                return 15;
            if (popularity_score > 10)
                return 10;
            if (popularity_score > 1)
                return 5;
            return MIN_REPLICAS;
        }

        Buffer serialize() const;
        static ContentEntry deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) إحصائيات عقدة التخزين المؤقت
    // ═════════════════════════════════════════════════════════════════════

    struct CacheNodeStats
    {
        NodeId node_id;
        uint64_t total_space_mb = 0;
        uint64_t used_space_mb = 0;
        uint32_t items_cached = 0;
        uint32_t requests_served = 0;
        double avg_response_ms = 0.0;
        double uptime_ratio = 1.0; // 0.0 - 1.0

        double available_ratio() const
        {
            if (total_space_mb == 0)
                return 0;
            return 1.0 - ((double)used_space_mb / total_space_mb);
        }
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) طلب وصول للمحتوى
    // ═════════════════════════════════════════════════════════════════════

    struct ContentRequest
    {
        ContentId content_id;
        NodeId requester;
        uint64_t timestamp = 0;
        uint32_t hop_count = 0; // عدد القفزات (لمنع الحلقات)

        Buffer serialize() const;
        static ContentRequest deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير CDN
    // ═════════════════════════════════════════════════════════════════════

    class CDNManager
    {
    public:
        CDNManager(const NodeId &self_id, uint32_t cache_size_mb = DEFAULT_CACHE_SIZE_MB);
        ~CDNManager();

        // ─── نشر المحتوى ───
        ContentId publish_content(const std::string &name,
                                  const std::string &mime_type,
                                  const Buffer &data);

        // ─── طلب المحتوى ───
        // (AR) يبحث أولاً في المخزن المحلي، ثم في العقد القريبة
        Result<Buffer> request_content(const ContentId &id);

        // ─── التخزين المؤقت ───
        void cache_content(const ContentId &id, const Buffer &data,
                           const ContentEntry &entry);
        bool has_cached(const ContentId &id) const;
        void evict_content(const ContentId &id);
        void evict_least_popular();

        // ─── فهرس المحتوى ───
        void register_content(const ContentEntry &entry);
        ContentEntry *find_content(const ContentId &id);
        std::vector<ContentEntry> search_content(const std::string &query) const;
        std::vector<ContentEntry> get_popular(uint32_t limit = 20) const;

        // ─── معالجة الطلبات الواردة ───
        void handle_request(const ContentRequest &req);

        // (AR) استدعاء عند الحاجة لإحضار محتوى من الشبكة
        using FetchCallback = std::function<void(const ContentId &, const NodeId &target)>;
        void on_fetch_needed(FetchCallback cb);

        // (AR) استدعاء عند توفر محتوى محلياً (للرد على الطلبات)
        using ServeCallback = std::function<void(const ContentId &, const Buffer &, const NodeId &requester)>;
        void on_serve_content(ServeCallback cb);

        // ─── إحصائيات ───
        CacheNodeStats get_stats() const;
        uint32_t cached_count() const;
        uint64_t cache_used_bytes() const;

        // ─── صيانة ───
        void cleanup_expired();
        void rebalance(); // (AR) إعادة توزيع النسخ حسب الشعبية

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::cdn

#endif // SAD_SADNET_CDN_CDN_H
