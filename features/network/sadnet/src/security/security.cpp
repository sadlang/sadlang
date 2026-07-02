// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة الأمان والثقة — التنفيذ
// (AR) إثبات العمل، نظام السمعة، شهادات الثقة، القوائم السوداء
//      حد المعدل (Rate Limiting)، حماية Sybil
// (EN) PoW, reputation, trust certificates, banning, rate limiting
// المكون: features/network/sadnet (الأمان والثقة)
// المسار: features/network/sadnet/src/security/security.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/security/security.h"
#include <algorithm>
#include <cstring>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

namespace sad::net::security
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) إثبات العمل — البحث عن nonce بحيث SHA-256 يبدأ بأصفار كافية
    // ═════════════════════════════════════════════════════════════════════

    // (AR) فحص أن أول difficulty بتات هي أصفار
    static bool check_leading_zeros(const Buffer &hash, uint8_t difficulty)
    {
        uint8_t full_bytes = difficulty / 8;
        uint8_t remaining_bits = difficulty % 8;

        if (hash.size() < full_bytes + (remaining_bits > 0 ? 1 : 0))
            return false;

        for (uint8_t i = 0; i < full_bytes; ++i)
        {
            if (hash[i] != 0)
                return false;
        }

        if (remaining_bits > 0)
        {
            uint8_t mask = (uint8_t)(0xFF << (8 - remaining_bits));
            if ((hash[full_bytes] & mask) != 0)
                return false;
        }

        return true;
    }

    // (AR) حساب هاش الإثبات: SHA-256(node_id || nonce || timestamp)
    static Buffer compute_pow_hash(const NodeId &id, uint64_t nonce, uint64_t timestamp)
    {
        Buffer data;
        data.append(id.data.data(), id.data.size());
        for (int i = 0; i < 8; ++i)
            data.push_back((uint8_t)(nonce >> (i * 8)));
        for (int i = 0; i < 8; ++i)
            data.push_back((uint8_t)(timestamp >> (i * 8)));
        auto h = crypto::sha256(data.data(), data.size());
        return Buffer(h.data(), h.size());
    }

    ProofOfWork ProofOfWork::generate(const NodeId &id, uint8_t difficulty)
    {
        ProofOfWork pow;
        pow.node_id = id;
        pow.difficulty = difficulty;
        pow.timestamp = now_ms();
        pow.nonce = 0;

        // (AR) البحث التسلسلي عن nonce صالح
        while (true)
        {
            Buffer h = compute_pow_hash(id, pow.nonce, pow.timestamp);
            if (check_leading_zeros(h, difficulty))
            {
                pow.hash = std::move(h);
                break;
            }
            ++pow.nonce;
        }

        return pow;
    }

    bool ProofOfWork::verify() const
    {
        Buffer h = compute_pow_hash(node_id, nonce, timestamp);
        if (!check_leading_zeros(h, difficulty))
            return false;

        // (AR) مطابقة الهاش المخزن إذا لم يكن فارغاً
        if (!hash.empty())
        {
            if (h.size() != hash.size())
                return false;
            return std::memcmp(h.data(), hash.data(), h.size()) == 0;
        }

        return true;
    }

    Buffer ProofOfWork::serialize() const
    {
        Buffer out;
        // node_id (32) + nonce (8) + difficulty (1) + timestamp (8) + hash_len (2) + hash
        out.append(node_id.data.data(), node_id.data.size());
        for (int i = 0; i < 8; ++i)
            out.push_back((uint8_t)(nonce >> (i * 8)));
        out.push_back(difficulty);
        for (int i = 0; i < 8; ++i)
            out.push_back((uint8_t)(timestamp >> (i * 8)));
        uint16_t hlen = (uint16_t)hash.size();
        out.push_back((uint8_t)(hlen & 0xFF));
        out.push_back((uint8_t)(hlen >> 8));
        out.insert(out.end(), hash.begin(), hash.end());
        return out;
    }

    ProofOfWork ProofOfWork::deserialize(const uint8_t *data, size_t len)
    {
        ProofOfWork pow;
        if (len < 32 + 8 + 1 + 8 + 2)
            return pow;
        size_t off = 0;

        std::memcpy(pow.node_id.data.data(), data + off, 32);
        off += 32;
        pow.nonce = 0;
        for (int i = 0; i < 8; ++i)
            pow.nonce |= ((uint64_t)data[off++]) << (i * 8);
        pow.difficulty = data[off++];
        pow.timestamp = 0;
        for (int i = 0; i < 8; ++i)
            pow.timestamp |= ((uint64_t)data[off++]) << (i * 8);
        uint16_t hlen = (uint16_t)(data[off] | (data[off + 1] << 8));
        off += 2;
        if (off + hlen <= len)
        {
            pow.hash = Buffer(data + off, hlen);
        }

        return pow;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) شهادة الثقة — التسلسل
    // ═════════════════════════════════════════════════════════════════════

    Buffer TrustCertificate::serialize() const
    {
        Buffer out;
        out.append(issuer.data.data(), issuer.data.size());   // 32
        out.append(subject.data.data(), subject.data.size()); // 32
        out.push_back((uint8_t)trust_level);                  // 1
        uint16_t rlen = (uint16_t)reason.size();
        out.push_back((uint8_t)(rlen & 0xFF));
        out.push_back((uint8_t)(rlen >> 8));
        out.append(reinterpret_cast<const uint8_t *>(reason.data()), reason.size());
        for (int i = 0; i < 8; ++i)
            out.push_back((uint8_t)(issued_at >> (i * 8)));
        for (int i = 0; i < 8; ++i)
            out.push_back((uint8_t)(expires_at >> (i * 8)));
        uint16_t slen = (uint16_t)signature.size();
        out.push_back((uint8_t)(slen & 0xFF));
        out.push_back((uint8_t)(slen >> 8));
        out.insert(out.end(), signature.begin(), signature.end());
        return out;
    }

    TrustCertificate TrustCertificate::deserialize(const uint8_t *data, size_t len)
    {
        TrustCertificate cert;
        if (len < 32 + 32 + 1 + 2 + 8 + 8 + 2)
            return cert;
        size_t off = 0;

        std::memcpy(cert.issuer.data.data(), data + off, 32);
        off += 32;
        std::memcpy(cert.subject.data.data(), data + off, 32);
        off += 32;
        cert.trust_level = (int8_t)data[off++];
        uint16_t rlen = (uint16_t)(data[off] | (data[off + 1] << 8));
        off += 2;
        if (off + rlen > len)
            return cert;
        cert.reason.assign((const char *)data + off, rlen);
        off += rlen;
        cert.issued_at = 0;
        for (int i = 0; i < 8; ++i)
            cert.issued_at |= ((uint64_t)data[off++]) << (i * 8);
        cert.expires_at = 0;
        for (int i = 0; i < 8; ++i)
            cert.expires_at |= ((uint64_t)data[off++]) << (i * 8);
        uint16_t slen = (uint16_t)(data[off] | (data[off + 1] << 8));
        off += 2;
        if (off + slen <= len)
        {
            cert.signature = Buffer(data + off, slen);
        }

        return cert;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير الأمان — التنفيذ الداخلي
    // ═════════════════════════════════════════════════════════════════════

    struct RateLimitEntry
    {
        std::vector<uint64_t> timestamps;
    };

    struct SecurityManager::Impl
    {
        NodeId self_id;
        crypto::KeyPair keys;
        uint8_t pow_difficulty;
        mutable std::mutex mtx;

        // (AR) سجلات السمعة
        std::unordered_map<NodeId, ReputationRecord> reputations;

        // (AR) شهادات الثقة (المفتاح = subject)
        std::unordered_map<NodeId, std::vector<TrustCertificate>> certificates;

        // (AR) القائمة السوداء
        std::unordered_set<NodeId> banned;

        // (AR) حد المعدل
        std::unordered_map<NodeId, RateLimitEntry> rate_limits;

        Impl(const NodeId &id, const crypto::KeyPair &k, uint8_t diff)
            : self_id(id), keys(k), pow_difficulty(diff) {}

        // (AR) الحصول على أو إنشاء سجل سمعة
        ReputationRecord &get_or_create(const NodeId &peer)
        {
            auto it = reputations.find(peer);
            if (it == reputations.end())
            {
                ReputationRecord rec;
                rec.node_id = peer;
                rec.first_seen = now_ms();
                rec.last_interaction = now_ms();
                auto [inserted, _] = reputations.emplace(peer, rec);
                return inserted->second;
            }
            return it->second;
        }

        // (AR) توقيع بيانات الشهادة (HMAC-SHA256 بالمفتاح الخاص)
        Buffer sign_cert_data(const TrustCertificate &cert) const
        {
            Buffer data;
            data.append(cert.issuer.data.data(), cert.issuer.data.size());
            data.append(cert.subject.data.data(), cert.subject.data.size());
            data.push_back((uint8_t)cert.trust_level);
            data.append(reinterpret_cast<const uint8_t *>(cert.reason.data()), cert.reason.size());
            for (int i = 0; i < 8; ++i)
                data.push_back((uint8_t)(cert.issued_at >> (i * 8)));
            for (int i = 0; i < 8; ++i)
                data.push_back((uint8_t)(cert.expires_at >> (i * 8)));
            auto h = crypto::hmac_sha256(keys.private_key.data(), keys.private_key.size(),
                                         data.data(), data.size());
            return Buffer(h.data(), h.size());
        }
    };

    SecurityManager::SecurityManager(const NodeId &self_id,
                                     const crypto::KeyPair &keys,
                                     uint8_t pow_difficulty)
        : impl_(std::make_unique<Impl>(self_id, keys, pow_difficulty)) {}

    SecurityManager::~SecurityManager() = default;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) إثبات العمل
    // ═════════════════════════════════════════════════════════════════════

    ProofOfWork SecurityManager::generate_pow()
    {
        return ProofOfWork::generate(impl_->self_id, impl_->pow_difficulty);
    }

    bool SecurityManager::verify_pow(const ProofOfWork &pow) const
    {
        // (AR) التحقق من الصعوبة والزمن
        if (pow.difficulty < impl_->pow_difficulty)
            return false;

        // (AR) رفض إثباتات قديمة جداً (أكثر من ساعة)
        uint64_t now = now_ms();
        if (pow.timestamp > now)
            return false; // مستقبل
        if (now - pow.timestamp > 3600000)
            return false; // أكثر من ساعة

        return pow.verify();
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) السمعة
    // ═════════════════════════════════════════════════════════════════════

    void SecurityManager::record_success(const NodeId &peer)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        auto &rec = impl_->get_or_create(peer);
        ++rec.successful;
        rec.score += 2; // مكافأة
        rec.last_interaction = now_ms();
    }

    void SecurityManager::record_failure(const NodeId &peer)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        auto &rec = impl_->get_or_create(peer);
        ++rec.failed;
        rec.score -= 3; // عقوبة أشد من المكافأة
        rec.last_interaction = now_ms();
    }

    ReputationRecord SecurityManager::get_reputation(const NodeId &peer) const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        auto it = impl_->reputations.find(peer);
        if (it == impl_->reputations.end())
        {
            ReputationRecord empty;
            empty.node_id = peer;
            return empty;
        }
        return it->second;
    }

    bool SecurityManager::is_trusted(const NodeId &peer) const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        // (AR) المحظور ليس موثوقاً أبداً
        if (impl_->banned.count(peer))
            return false;

        auto it = impl_->reputations.find(peer);
        if (it == impl_->reputations.end())
            return false;
        return it->second.is_trusted();
    }

    std::vector<ReputationRecord> SecurityManager::get_all_reputations() const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        std::vector<ReputationRecord> result;
        result.reserve(impl_->reputations.size());
        for (auto &[_, rec] : impl_->reputations)
        {
            result.push_back(rec);
        }
        return result;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) شهادات الثقة
    // ═════════════════════════════════════════════════════════════════════

    TrustCertificate SecurityManager::issue_certificate(const NodeId &subject,
                                                        int8_t trust_level,
                                                        const std::string &reason,
                                                        uint64_t validity_ms)
    {
        TrustCertificate cert;
        cert.issuer = impl_->self_id;
        cert.subject = subject;
        cert.trust_level = std::max((int8_t)-10, std::min((int8_t)10, trust_level));
        cert.reason = reason;
        cert.issued_at = now_ms();
        cert.expires_at = validity_ms > 0 ? cert.issued_at + validity_ms : 0;

        // (AR) التوقيع
        cert.signature = impl_->sign_cert_data(cert);

        // (AR) تخزين تلقائي
        store_certificate(cert);

        return cert;
    }

    bool SecurityManager::verify_certificate(const TrustCertificate &cert) const
    {
        // (AR) التحقق من الانتهاء
        if (cert.is_expired())
            return false;
        if (cert.signature.empty())
            return false;

        // (AR) يمكننا التحقق فقط إذا كنا المُصدِر
        // لأننا نستخدم HMAC (مفتاح متماثل)
        if (cert.issuer == impl_->self_id)
        {
            Buffer expected = impl_->sign_cert_data(cert);
            if (expected.size() != cert.signature.size())
                return false;
            return std::memcmp(expected.data(), cert.signature.data(), expected.size()) == 0;
        }

        // (AR) لشهادات الآخرين: نقبلها إذا كانت غير منتهية
        return true;
    }

    void SecurityManager::store_certificate(const TrustCertificate &cert)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->certificates[cert.subject].push_back(cert);

        // (AR) تحديث السمعة بناءً على الشهادة
        auto &rec = impl_->get_or_create(cert.subject);
        rec.score += cert.trust_level;
    }

    std::vector<TrustCertificate> SecurityManager::get_certificates(const NodeId &subject) const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        auto it = impl_->certificates.find(subject);
        if (it == impl_->certificates.end())
            return {};

        // (AR) استبعاد المنتهية
        std::vector<TrustCertificate> result;
        for (auto &cert : it->second)
        {
            if (!cert.is_expired())
            {
                result.push_back(cert);
            }
        }
        return result;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) القائمة السوداء
    // ═════════════════════════════════════════════════════════════════════

    void SecurityManager::ban(const NodeId &peer)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->banned.insert(peer);

        // (AR) تصفير السمعة
        auto it = impl_->reputations.find(peer);
        if (it != impl_->reputations.end())
        {
            it->second.score = -100;
        }
    }

    void SecurityManager::unban(const NodeId &peer)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        impl_->banned.erase(peer);
    }

    bool SecurityManager::is_banned(const NodeId &peer) const
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);
        return impl_->banned.count(peer) > 0;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) حد المعدل — نافذة منزلقة لدقيقة واحدة
    // ═════════════════════════════════════════════════════════════════════

    bool SecurityManager::check_rate_limit(const NodeId &peer, uint32_t max_per_minute)
    {
        std::lock_guard<std::mutex> lock(impl_->mtx);

        uint64_t now = now_ms();
        uint64_t window = 60000; // دقيقة واحدة

        auto &entry = impl_->rate_limits[peer];

        // (AR) إزالة الطوابع الزمنية القديمة
        entry.timestamps.erase(
            std::remove_if(entry.timestamps.begin(), entry.timestamps.end(),
                           [now, window](uint64_t ts)
                           { return ts + window < now; }),
            entry.timestamps.end());

        if (entry.timestamps.size() >= max_per_minute)
        {
            return false; // تجاوز الحد
        }

        entry.timestamps.push_back(now);
        return true;
    }

} // namespace sad::net::security
