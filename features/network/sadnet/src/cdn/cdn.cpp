// ═══════════════════════════════════════════════════════════════════
// صNet — CDN لامركزي — التنفيذ
// (AR) تخزين مؤقت LRU مع وزن الشعبية، فهرسة، توزيع تكيفي
// (EN) CDN Manager: popularity-weighted LRU cache, indexing, routing
// المكون: features/network/sadnet (CDN لامركزي)
// المسار: features/network/sadnet/src/cdn/cdn.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/cdn/cdn.h"
#include <algorithm>
#include <cstring>
#include <list>
#include <mutex>
#include <unordered_map>

namespace sad::net::cdn
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ContentEntry — التسلسل
    // ═════════════════════════════════════════════════════════════════════

    static void write_u16(Buffer &out, uint16_t v)
    {
        out.push_back((uint8_t)(v & 0xFF));
        out.push_back((uint8_t)(v >> 8));
    }

    static void write_u32(Buffer &out, uint32_t v)
    {
        for (int i = 0; i < 4; ++i)
            out.push_back((uint8_t)(v >> (i * 8)));
    }

    static void write_u64(Buffer &out, uint64_t v)
    {
        for (int i = 0; i < 8; ++i)
            out.push_back((uint8_t)(v >> (i * 8)));
    }

    static void write_str(Buffer &out, const std::string &s)
    {
        write_u16(out, (uint16_t)s.size());
        out.append(reinterpret_cast<const uint8_t *>(s.data()), s.size());
    }

    static uint16_t read_u16(const uint8_t *&p)
    {
        uint16_t v = (uint16_t)(p[0] | (p[1] << 8));
        p += 2;
        return v;
    }

    static uint32_t read_u32(const uint8_t *&p)
    {
        uint32_t v = 0;
        for (int i = 0; i < 4; ++i)
            v |= ((uint32_t)p[i]) << (i * 8);
        p += 4;
        return v;
    }

    static uint64_t read_u64(const uint8_t *&p)
    {
        uint64_t v = 0;
        for (int i = 0; i < 8; ++i)
            v |= ((uint64_t)p[i]) << (i * 8);
        p += 8;
        return v;
    }

    static std::string read_str(const uint8_t *&p)
    {
        uint16_t len = read_u16(p);
        std::string s((const char *)p, len);
        p += len;
        return s;
    }

    Buffer ContentEntry::serialize() const
    {
        Buffer out;
        out.append(content_id.data.data(), content_id.data.size()); // 32
        write_str(out, name);
        write_str(out, mime_type);
        write_u64(out, size);
        write_u64(out, created_at);
        out.append(origin.data.data(), origin.data.size()); // 32
        write_u16(out, (uint16_t)cache_nodes.size());
        for (auto &node : cache_nodes)
        {
            out.append(node.data.data(), node.data.size());
        }
        write_u32(out, access_count);
        write_u64(out, last_accessed);
        return out;
    }

    ContentEntry ContentEntry::deserialize(const uint8_t *data, size_t len)
    {
        ContentEntry entry;
        if (len < 32 + 2 + 2 + 8 + 8 + 32 + 2 + 4 + 8)
            return entry;
        const uint8_t *p = data;

        std::memcpy(entry.content_id.data.data(), p, 32);
        p += 32;
        entry.name = read_str(p);
        entry.mime_type = read_str(p);
        entry.size = read_u64(p);
        entry.created_at = read_u64(p);
        std::memcpy(entry.origin.data.data(), p, 32);
        p += 32;
        uint16_t node_count = read_u16(p);
        for (uint16_t i = 0; i < node_count && (size_t)(p - data + 32) <= len; ++i)
        {
            NodeId nid;
            std::memcpy(nid.data.data(), p, 32);
            p += 32;
            entry.cache_nodes.push_back(nid);
        }
        entry.access_count = read_u32(p);
        entry.last_accessed = read_u64(p);
        entry.update_popularity();
        return entry;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) طلب المحتوى — التسلسل
    // ═════════════════════════════════════════════════════════════════════

    Buffer ContentRequest::serialize() const
    {
        Buffer out;
        out.append(content_id.data.data(), content_id.data.size()); // 32
        out.append(requester.data.data(), requester.data.size());   // 32
        write_u64(out, timestamp);
        write_u32(out, hop_count);
        return out;
    }

    ContentRequest ContentRequest::deserialize(const uint8_t *data, size_t len)
    {
        ContentRequest req;
        if (len < 32 + 32 + 8 + 4)
            return req;
        const uint8_t *p = data;
        std::memcpy(req.content_id.data.data(), p, 32);
        p += 32;
        std::memcpy(req.requester.data.data(), p, 32);
        p += 32;
        req.timestamp = read_u64(p);
        req.hop_count = read_u32(p);
        return req;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) عنصر التخزين المؤقت
    // ═════════════════════════════════════════════════════════════════════

    struct CacheItem
    {
        ContentId content_id;
        Buffer data;
        ContentEntry entry;
        uint64_t cached_at = 0;
        uint32_t local_hits = 0;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير CDN — التنفيذ الداخلي
    // ═════════════════════════════════════════════════════════════════════

    struct CDNManager::Impl
    {
        NodeId self_id;
        uint64_t max_cache_bytes;
        uint64_t used_bytes = 0;
        mutable std::mutex mtx;

        // (AR) فهرس المحتوى العالمي (معروف لهذه العقدة)
        std::unordered_map<ContentId, ContentEntry> index;

        // (AR) ذاكرة تخزين مؤقت محلية (LRU مع وزن الشعبية)
        // ترتيب الوصول: الأحدث في الأمام
        std::list<ContentId> lru_order;
        std::unordered_map<ContentId, CacheItem> cache;

        // (AR) إحصائيات
        uint32_t requests_served = 0;
        double total_response_ms = 0.0;

        FetchCallback fetch_cb;
        ServeCallback serve_cb;

        Impl(const NodeId &id, uint32_t cache_mb)
            : self_id(id), max_cache_bytes((uint64_t)cache_mb * 1024 * 1024) {}

        // (AR) نقل عنصر لمقدمة LRU (تم الوصول إليه مؤخراً)
        void touch_lru(const ContentId &id)
        {
            lru_order.remove(id);
            lru_order.push_front(id);
        }

        // (AR) حذف الأقل شعبية وقِدماً من المخزن
        void evict_one()
        {
            if (lru_order.empty())
                return;

            // (AR) نبحث عن العنصر ذو أقل شعبية × حداثة
            ContentId worst_id = lru_order.back();
            double worst_score = 1e18;

            for (auto it = lru_order.rbegin(); it != lru_order.rend(); ++it)
            {
                auto cit = cache.find(*it);
                if (cit == cache.end())
                    continue;

                // (AR) النتيجة = الشعبية × (1 + الوصول المحلي)
                double score = cit->second.entry.popularity_score * (1.0 + cit->second.local_hits);
                if (score < worst_score)
                {
                    worst_score = score;
                    worst_id = *it;
                }
            }

            auto cit = cache.find(worst_id);
            if (cit != cache.end())
            {
                used_bytes -= cit->second.data.size();
                cache.erase(cit);
            }
            lru_order.remove(worst_id);
        }
    };

    CDNManager::CDNManager(const NodeId &self_id, uint32_t cache_size_mb)
        : impl_(std::make_unique<Impl>(self_id, cache_size_mb)) {}

    CDNManager::~CDNManager() = default;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) نشر المحتوى
    // ═════════════════════════════════════════════════════════════════════

    ContentId CDNManager::publish_content(const std::string &name,
                                          const std::string &mime_type,
                                          const Buffer &data)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        // (AR) حساب معرف المحتوى من التجزئة
        auto hash = crypto::sha256(data.data(), data.size());
        ContentId id;
        std::memcpy(id.data.data(), hash.data(), 32);

        ContentEntry entry;
        entry.content_id = id;
        entry.name = name;
        entry.mime_type = mime_type;
        entry.size = data.size();
        entry.created_at = now_ms();
        entry.origin = impl_->self_id;
        entry.cache_nodes.push_back(impl_->self_id);
        entry.access_count = 0;
        entry.last_accessed = now_ms();

        impl_->index[id] = entry;

        // (AR) تخزين محلياً
        CacheItem item;
        item.content_id = id;
        item.data = data;
        item.entry = entry;
        item.cached_at = now_ms();

        // (AR) إفساح المجال إذا لزم الأمر
        while (impl_->used_bytes + data.size() > impl_->max_cache_bytes && !impl_->cache.empty())
        {
            impl_->evict_one();
        }

        impl_->used_bytes += data.size();
        impl_->cache[id] = std::move(item);
        impl_->lru_order.push_front(id);

        return id;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) طلب المحتوى
    // ═════════════════════════════════════════════════════════════════════

    Result<Buffer> CDNManager::request_content(const ContentId &id)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        // (AR) البحث في المخزن المحلي أولاً
        auto cit = impl_->cache.find(id);
        if (cit != impl_->cache.end())
        {
            cit->second.local_hits++;
            impl_->touch_lru(id);
            impl_->requests_served++;

            // (AR) تحديث الإحصائيات في الفهرس
            auto idx = impl_->index.find(id);
            if (idx != impl_->index.end())
            {
                idx->second.access_count++;
                idx->second.last_accessed = now_ms();
                idx->second.update_popularity();
            }

            return Result<Buffer>::success(cit->second.data);
        }

        // (AR) غير موجود محلياً — طلب من الشبكة
        if (impl_->fetch_cb)
        {
            auto idx = impl_->index.find(id);
            if (idx != impl_->index.end() && !idx->second.cache_nodes.empty())
            {
                // (AR) طلب من أقرب عقدة تخزين
                impl_->fetch_cb(id, idx->second.cache_nodes[0]);
            }
        }

        return Result<Buffer>::fail(NetError::NOT_FOUND);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) التخزين المؤقت
    // ═════════════════════════════════════════════════════════════════════

    void CDNManager::cache_content(const ContentId &id, const Buffer &data,
                                   const ContentEntry &entry)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        if (impl_->cache.count(id))
        {
            impl_->touch_lru(id);
            return; // موجود بالفعل
        }

        // (AR) إفساح المجال
        while (impl_->used_bytes + data.size() > impl_->max_cache_bytes && !impl_->cache.empty())
        {
            impl_->evict_one();
        }

        CacheItem item;
        item.content_id = id;
        item.data = data;
        item.entry = entry;
        item.cached_at = now_ms();

        impl_->used_bytes += data.size();
        impl_->cache[id] = std::move(item);
        impl_->lru_order.push_front(id);

        // (AR) تحديث الفهرس
        auto idx = impl_->index.find(id);
        if (idx != impl_->index.end())
        {
            // (AR) إضافة هذه العقدة كعقدة تخزين
            auto &nodes = idx->second.cache_nodes;
            if (std::find(nodes.begin(), nodes.end(), impl_->self_id) == nodes.end())
            {
                nodes.push_back(impl_->self_id);
            }
        }
        else
        {
            ContentEntry e = entry;
            e.cache_nodes.push_back(impl_->self_id);
            impl_->index[id] = e;
        }
    }

    bool CDNManager::has_cached(const ContentId &id) const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        return impl_->cache.count(id) > 0;
    }

    void CDNManager::evict_content(const ContentId &id)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        auto it = impl_->cache.find(id);
        if (it != impl_->cache.end())
        {
            impl_->used_bytes -= it->second.data.size();
            impl_->cache.erase(it);
        }
        impl_->lru_order.remove(id);
    }

    void CDNManager::evict_least_popular()
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->evict_one();
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) فهرس المحتوى
    // ═════════════════════════════════════════════════════════════════════

    void CDNManager::register_content(const ContentEntry &entry)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->index[entry.content_id] = entry;
    }

    ContentEntry *CDNManager::find_content(const ContentId &id)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        auto it = impl_->index.find(id);
        if (it == impl_->index.end())
            return nullptr;
        return &it->second;
    }

    std::vector<ContentEntry> CDNManager::search_content(const std::string &query) const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        std::vector<ContentEntry> results;

        for (auto &[_, entry] : impl_->index)
        {
            // (AR) بحث بسيط في الاسم ونوع MIME
            if (entry.name.find(query) != std::string::npos ||
                entry.mime_type.find(query) != std::string::npos)
            {
                results.push_back(entry);
            }
        }

        // (AR) ترتيب حسب الشعبية
        std::sort(results.begin(), results.end(),
                  [](const ContentEntry &a, const ContentEntry &b)
                  {
                      return a.popularity_score > b.popularity_score;
                  });

        return results;
    }

    std::vector<ContentEntry> CDNManager::get_popular(uint32_t limit) const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        std::vector<ContentEntry> results;
        results.reserve(impl_->index.size());

        for (auto &[_, entry] : impl_->index)
        {
            results.push_back(entry);
        }

        std::sort(results.begin(), results.end(),
                  [](const ContentEntry &a, const ContentEntry &b)
                  {
                      return a.popularity_score > b.popularity_score;
                  });

        if (results.size() > limit)
        {
            results.resize(limit);
        }

        return results;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) معالجة الطلبات الواردة
    // ═════════════════════════════════════════════════════════════════════

    void CDNManager::handle_request(const ContentRequest &req)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        // (AR) منع الحلقات
        if (req.hop_count > 10)
            return;

        auto cit = impl_->cache.find(req.content_id);
        if (cit != impl_->cache.end())
        {
            cit->second.local_hits++;
            impl_->touch_lru(req.content_id);
            impl_->requests_served++;

            if (impl_->serve_cb)
            {
                impl_->serve_cb(req.content_id, cit->second.data, req.requester);
            }
        }
        else if (impl_->fetch_cb)
        {
            // (AR) إعادة التوجيه إلى عقدة أخرى
            auto idx = impl_->index.find(req.content_id);
            if (idx != impl_->index.end() && !idx->second.cache_nodes.empty())
            {
                impl_->fetch_cb(req.content_id, idx->second.cache_nodes[0]);
            }
        }
    }

    void CDNManager::on_fetch_needed(FetchCallback cb)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->fetch_cb = std::move(cb);
    }

    void CDNManager::on_serve_content(ServeCallback cb)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->serve_cb = std::move(cb);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) الإحصائيات
    // ═════════════════════════════════════════════════════════════════════

    CacheNodeStats CDNManager::get_stats() const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        CacheNodeStats stats;
        stats.node_id = impl_->self_id;
        stats.total_space_mb = impl_->max_cache_bytes / (1024 * 1024);
        stats.used_space_mb = impl_->used_bytes / (1024 * 1024);
        stats.items_cached = (uint32_t)impl_->cache.size();
        stats.requests_served = impl_->requests_served;
        if (impl_->requests_served > 0)
        {
            stats.avg_response_ms = impl_->total_response_ms / impl_->requests_served;
        }
        return stats;
    }

    uint32_t CDNManager::cached_count() const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        return (uint32_t)impl_->cache.size();
    }

    uint64_t CDNManager::cache_used_bytes() const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        return impl_->used_bytes;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) الصيانة
    // ═════════════════════════════════════════════════════════════════════

    void CDNManager::cleanup_expired()
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        uint64_t now = now_ms();
        std::vector<ContentId> to_remove;

        for (auto &[id, item] : impl_->cache)
        {
            if (now - item.cached_at > CACHE_EXPIRY_MS && item.local_hits == 0)
            {
                to_remove.push_back(id);
            }
        }

        for (auto &id : to_remove)
        {
            auto it = impl_->cache.find(id);
            if (it != impl_->cache.end())
            {
                impl_->used_bytes -= it->second.data.size();
                impl_->cache.erase(it);
            }
            impl_->lru_order.remove(id);
        }
    }

    void CDNManager::rebalance()
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        // (AR) تحديث درجات الشعبية لكل المحتوى
        for (auto &[_, entry] : impl_->index)
        {
            entry.update_popularity();
        }

        // (AR) حذف المحتوى المنخفض الشعبية إذا كان المخزن ممتلئاً (> 90%)
        if (impl_->used_bytes > impl_->max_cache_bytes * 9 / 10)
        {
            while (impl_->used_bytes > impl_->max_cache_bytes * 7 / 10 && !impl_->cache.empty())
            {
                impl_->evict_one();
            }
        }
    }

} // namespace sad::net::cdn
