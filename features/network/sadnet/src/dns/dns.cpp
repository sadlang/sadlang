// ═══════════════════════════════════════════════════════════════════
// صNet — تنفيذ DNS اللامركزي
// (AR) تسجيل وحل الأسماء عبر DHT، ذاكرة مؤقتة، توقيع
// (EN) Decentralized DNS implementation: register, resolve, cache
// المكون: features/network/sadnet (DNS اللامركزي)
// المسار: features/network/sadnet/src/dns/dns.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/dns/dns.h"
#include <algorithm>
#include <cstring>
#include <mutex>

namespace sad::net::dns
{

    // مساعدات ترميز
    static void w16(uint8_t *p, uint16_t v)
    {
        p[0] = (uint8_t)(v >> 8);
        p[1] = (uint8_t)v;
    }
    static void w32(uint8_t *p, uint32_t v)
    {
        p[0] = (uint8_t)(v >> 24);
        p[1] = (uint8_t)(v >> 16);
        p[2] = (uint8_t)(v >> 8);
        p[3] = (uint8_t)v;
    }
    static void w64(uint8_t *p, uint64_t v)
    {
        w32(p, (uint32_t)(v >> 32));
        w32(p + 4, (uint32_t)v);
    }
    static uint16_t r16(const uint8_t *p) { return ((uint16_t)p[0] << 8) | p[1]; }
    static uint32_t r32(const uint8_t *p)
    {
        return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
    }
    static uint64_t r64(const uint8_t *p) { return ((uint64_t)r32(p) << 32) | r32(p + 4); }

    static void write_str(Buffer &b, const std::string &s)
    {
        size_t o = b.size();
        b.resize(o + 2 + s.size());
        w16(b.data() + o, (uint16_t)s.size());
        if (!s.empty())
            std::memcpy(b.data() + o + 2, s.c_str(), s.size());
    }
    static std::string read_str(const uint8_t *d, size_t l, size_t &o)
    {
        if (o + 2 > l)
            return "";
        uint16_t n = r16(d + o);
        o += 2;
        if (o + n > l)
            return "";
        std::string s((const char *)(d + o), n);
        o += n;
        return s;
    }
    static void write_buf(Buffer &b, const Buffer &data)
    {
        size_t o = b.size();
        b.resize(o + 2 + data.size());
        w16(b.data() + o, (uint16_t)data.size());
        if (!data.empty())
            std::memcpy(b.data() + o + 2, data.data(), data.size());
    }
    static Buffer read_buf(const uint8_t *d, size_t l, size_t &o)
    {
        if (o + 2 > l)
            return Buffer();
        uint16_t n = r16(d + o);
        o += 2;
        if (o + n > l)
            return Buffer();
        Buffer b(d + o, n);
        o += n;
        return b;
    }

    // ═══════════════════════════════════════════════════════════════════
    // DNSRecord ترميز/فك
    // ═══════════════════════════════════════════════════════════════════

    Buffer DNSRecord::serialize() const
    {
        Buffer buf;
        write_str(buf, name);
        size_t o = buf.size();
        buf.resize(o + 1);
        buf[o] = (uint8_t)type;
        write_str(buf, value);
        write_str(buf, endpoint.address);
        o = buf.size();
        buf.resize(o + 2);
        w16(buf.data() + o, endpoint.port);

        o = buf.size();
        buf.resize(o + NODE_ID_SIZE);
        std::memcpy(buf.data() + o, owner.data.data(), NODE_ID_SIZE);

        o = buf.size();
        buf.resize(o + 8 + 8 + 4);
        w64(buf.data() + o, created_at);
        w64(buf.data() + o + 8, expires_at);
        w32(buf.data() + o + 16, ttl);

        write_buf(buf, signature);
        return buf;
    }

    DNSRecord DNSRecord::deserialize(const uint8_t *data, size_t len)
    {
        DNSRecord rec;
        size_t off = 0;
        rec.name = read_str(data, len, off);
        if (off >= len)
            return rec;
        rec.type = (RecordType)data[off++];
        rec.value = read_str(data, len, off);
        rec.endpoint.address = read_str(data, len, off);
        if (off + 2 > len)
            return rec;
        rec.endpoint.port = r16(data + off);
        off += 2;

        if (off + NODE_ID_SIZE > len)
            return rec;
        std::memcpy(rec.owner.data.data(), data + off, NODE_ID_SIZE);
        off += NODE_ID_SIZE;

        if (off + 20 > len)
            return rec;
        rec.created_at = r64(data + off);
        off += 8;
        rec.expires_at = r64(data + off);
        off += 8;
        rec.ttl = r32(data + off);
        off += 4;

        rec.signature = read_buf(data, len, off);
        return rec;
    }

    // ═══════════════════════════════════════════════════════════════════
    // مدير DNS
    // ═══════════════════════════════════════════════════════════════════

    struct DNSManager::Impl
    {
        NodeId self_id;
        crypto::KeyPair keys;

        // سجلات محلية (نحن مالكوها)
        std::vector<DNSRecord> local;
        // ذاكرة مؤقتة (سجلات من الشبكة)
        std::vector<DNSRecord> cache;
        mutable std::mutex mutex;

        Impl(const NodeId &id, const crypto::KeyPair &k) : self_id(id), keys(k) {}

        Buffer sign_record(const DNSRecord &rec)
        {
            DNSRecord copy = rec;
            copy.signature.clear();
            auto data = copy.serialize();
            auto hash = crypto::hmac_sha256(keys.private_key.data(), keys.private_key.size(),
                                            data.data(), data.size());
            return Buffer(hash.data(), hash.size());
        }
    };

    DNSManager::DNSManager(const NodeId &self_id, const crypto::KeyPair &keys)
        : impl_(std::make_unique<Impl>(self_id, keys)) {}

    DNSManager::~DNSManager() = default;

    Result<DNSRecord> DNSManager::register_name(const std::string &name,
                                                RecordType type,
                                                const std::string &value,
                                                uint32_t ttl)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);

        // تحقق من عدم وجود اسم مكرر
        for (const auto &r : impl_->local)
            if (r.name == name && r.type == type)
                return Result<DNSRecord>::fail(NetError::ADDRESS_IN_USE);

        DNSRecord rec;
        rec.name = name;
        rec.type = type;
        rec.value = value;
        rec.owner = impl_->self_id;
        rec.created_at = now_ms();
        rec.expires_at = ttl > 0 ? now_ms() + (uint64_t)ttl * 1000 : 0;
        rec.ttl = ttl;
        rec.signature = impl_->sign_record(rec);

        impl_->local.push_back(rec);
        return Result<DNSRecord>::success(rec);
    }

    Result<DNSRecord> DNSManager::register_service(const std::string &name,
                                                   const Endpoint &endpoint,
                                                   uint32_t ttl)
    {
        auto result = register_name(name, RecordType::SRV, endpoint.address, ttl);
        if (result.ok())
        {
            // تحديث العنوان في السجل المحلي
            std::lock_guard<std::mutex> lock(impl_->mutex);
            for (auto &r : impl_->local)
            {
                if (r.name == name && r.type == RecordType::SRV)
                {
                    r.endpoint = endpoint;
                    break;
                }
            }
        }
        return result;
    }

    Result<void> DNSManager::update_record(const std::string &name,
                                           const std::string &new_value)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        for (auto &r : impl_->local)
        {
            if (r.name == name)
            {
                r.value = new_value;
                r.signature = impl_->sign_record(r);
                return Result<void>::success();
            }
        }
        return Result<void>::fail(NetError::NOT_FOUND);
    }

    Result<void> DNSManager::remove_record(const std::string &name)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        auto it = std::remove_if(impl_->local.begin(), impl_->local.end(),
                                 [&name](const DNSRecord &r)
                                 { return r.name == name; });
        if (it == impl_->local.end())
            return Result<void>::fail(NetError::NOT_FOUND);
        impl_->local.erase(it, impl_->local.end());
        return Result<void>::success();
    }

    std::vector<DNSRecord> DNSManager::resolve(const std::string &name) const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        std::vector<DNSRecord> results;
        for (const auto &r : impl_->local)
            if (r.name == name && !r.is_expired())
                results.push_back(r);
        for (const auto &r : impl_->cache)
            if (r.name == name && !r.is_expired())
                results.push_back(r);
        return results;
    }

    std::vector<DNSRecord> DNSManager::resolve(const std::string &name,
                                               RecordType type) const
    {
        auto all = resolve(name);
        std::vector<DNSRecord> filtered;
        for (const auto &r : all)
            if (r.type == type)
                filtered.push_back(r);
        return filtered;
    }

    void DNSManager::cache_record(const DNSRecord &record)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->cache.push_back(record);
    }

    void DNSManager::clear_cache()
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->cache.clear();
    }

    void DNSManager::cleanup_expired()
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        auto rm = [](std::vector<DNSRecord> &v)
        {
            v.erase(std::remove_if(v.begin(), v.end(),
                                   [](const DNSRecord &r)
                                   { return r.is_expired(); }),
                    v.end());
        };
        rm(impl_->local);
        rm(impl_->cache);
    }

    bool DNSManager::verify_record(const DNSRecord &record) const
    {
        if (record.is_expired())
            return false;
        if (record.signature.empty())
            return false;
        // التحقق البسيط: إعادة حساب التوقيع (يعمل فقط للسجلات المحلية)
        DNSRecord copy = record;
        copy.signature.clear();
        auto data = copy.serialize();
        auto expected = crypto::hmac_sha256(impl_->keys.private_key.data(), impl_->keys.private_key.size(),
                                            data.data(), data.size());
        Buffer expected_buf(expected.data(), expected.size());
        return expected_buf == record.signature;
    }

    std::vector<DNSRecord> DNSManager::local_records() const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        return impl_->local;
    }

} // namespace sad::net::dns
