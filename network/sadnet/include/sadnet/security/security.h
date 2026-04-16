// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة الأمان والثقة
// (AR) حماية ضد هجمات Sybil، نظام سمعة، شهادات ثقة
//      إثبات العمل (PoW) للانضمام، تقييم الأقران
// (EN) Security & Trust: Sybil protection, reputation, certificates// المكون: network/sadnet (الأمان والثقة)
// المسار: network/sadnet/include/sadnet/security/security.h// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_SECURITY_SECURITY_H
#define SAD_SADNET_SECURITY_SECURITY_H

#include "sadnet/core/types.h"
#include "sadnet/crypto/crypto.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace sad::net::security
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::NetError;
    using sad::net::NodeId;
    using sad::net::Result;
    using sad::net::crypto::KeyPair;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) إثبات العمل (Proof of Work) — حماية Sybil
    //      يجب أن يُنتج القرين إثبات عمل قبل الانضمام
    // ═════════════════════════════════════════════════════════════════════

    struct ProofOfWork
    {
        NodeId node_id;
        uint64_t nonce = 0;
        uint8_t difficulty = 16; // عدد الأصفار المطوبة (بالبت)
        uint64_t timestamp = 0;
        Buffer hash; // SHA-256(node_id || nonce || timestamp)

        // (AR) إنشاء إثبات عمل (قد يستغرق وقتاً)
        static ProofOfWork generate(const NodeId &id, uint8_t difficulty);

        // (AR) التحقق من صحة الإثبات
        bool verify() const;

        Buffer serialize() const;
        static ProofOfWork deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) سجل سمعة القرين
    // ═════════════════════════════════════════════════════════════════════

    struct ReputationRecord
    {
        NodeId node_id;
        int64_t score = 0;       // سمعة مجمعة
        uint32_t successful = 0; // عمليات ناجحة
        uint32_t failed = 0;     // عمليات فاشلة
        uint64_t first_seen = 0;
        uint64_t last_interaction = 0;

        // (AR) حساب النسبة المئوية للنجاح
        double success_rate() const
        {
            if (successful + failed == 0)
                return 0.5; // محايد
            return (double)successful / (successful + failed);
        }

        // (AR) هل القرين موثوق؟
        bool is_trusted() const
        {
            return score >= 10 && success_rate() >= 0.7;
        }
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) شهادة ثقة — توقيع قرين على آخر
    // ═════════════════════════════════════════════════════════════════════

    struct TrustCertificate
    {
        NodeId issuer;
        NodeId subject;
        int8_t trust_level = 0; // -10 إلى +10
        std::string reason;
        uint64_t issued_at = 0;
        uint64_t expires_at = 0;
        Buffer signature;

        bool is_expired() const
        {
            return expires_at > 0 && now_ms() > expires_at;
        }

        Buffer serialize() const;
        static TrustCertificate deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير الأمان والثقة
    // ═════════════════════════════════════════════════════════════════════

    class SecurityManager
    {
    public:
        SecurityManager(const NodeId &self_id, const crypto::KeyPair &keys,
                        uint8_t pow_difficulty = 16);
        ~SecurityManager();

        // ─── إثبات العمل ───
        ProofOfWork generate_pow();
        bool verify_pow(const ProofOfWork &pow) const;

        // ─── السمعة ───
        void record_success(const NodeId &peer);
        void record_failure(const NodeId &peer);
        ReputationRecord get_reputation(const NodeId &peer) const;
        bool is_trusted(const NodeId &peer) const;
        std::vector<ReputationRecord> get_all_reputations() const;

        // ─── شهادات الثقة ───
        TrustCertificate issue_certificate(const NodeId &subject,
                                           int8_t trust_level,
                                           const std::string &reason,
                                           uint64_t validity_ms = 0);
        bool verify_certificate(const TrustCertificate &cert) const;
        void store_certificate(const TrustCertificate &cert);
        std::vector<TrustCertificate> get_certificates(const NodeId &subject) const;

        // ─── القائمة السوداء ───
        void ban(const NodeId &peer);
        void unban(const NodeId &peer);
        bool is_banned(const NodeId &peer) const;

        // ─── حد المعدل (Rate Limiting) ───
        bool check_rate_limit(const NodeId &peer, uint32_t max_per_minute = 60);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::security

#endif // SAD_SADNET_SECURITY_SECURITY_H
