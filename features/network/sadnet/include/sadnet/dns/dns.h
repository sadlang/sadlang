// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة DNS اللامركزي
// (AR) نظام أسماء نطاقات لامركزي يعتمد على DHT
//      تسجيل أسماء → عناوين، حل أسماء، تحديث، انتهاء صلاحية
//      يدعم نطاق .snet (مثال: myapp.snet)
// (EN) Decentralized DNS: name registration & resolution via DHT
// المكون: features/network/sadnet (DNS اللامركزي)
// المسار: features/network/sadnet/include/sadnet/dns/dns.h
// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_DNS_DNS_H
#define SAD_SADNET_DNS_DNS_H

#include "sadnet/core/types.h"
#include "sadnet/crypto/crypto.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace sad::net::dns
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::Endpoint;
    using sad::net::NetError;
    using sad::net::NodeId;
    using sad::net::Result;
    using sad::net::crypto::KeyPair;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) أنواع السجلات (Record Types)
    // ═════════════════════════════════════════════════════════════════════

    enum class RecordType : uint8_t
    {
        A = 1,     // عنوان IPv4
        AAAA = 2,  // عنوان IPv6
        CNAME = 3, // اسم بديل
        TXT = 4,   // نص حر
        SRV = 5,   // خدمة (عنوان + منفذ)
        DID = 6,   // وثيقة هوية لامركزية
        NODE = 7   // معرف عُقدة صNet
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) سجل DNS
    // ═════════════════════════════════════════════════════════════════════

    struct DNSRecord
    {
        std::string name; // الاسم الكامل (مثال: "myapp.snet")
        RecordType type;
        std::string value; // القيمة (عنوان IP، اسم، نص...)
        Endpoint endpoint; // للخدمات
        NodeId owner;      // مالك السجل
        uint64_t created_at = 0;
        uint64_t expires_at = 0; // 0 = لا ينتهي
        uint32_t ttl = 3600;     // وقت الحياة (بالثواني)
        Buffer signature;        // توقيع المالك

        bool is_expired() const
        {
            return expires_at > 0 && now_ms() > expires_at;
        }

        Buffer serialize() const;
        static DNSRecord deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير DNS اللامركزي
    // ═════════════════════════════════════════════════════════════════════

    using ResolveCallback = std::function<void(const std::vector<DNSRecord> &records)>;

    class DNSManager
    {
    public:
        DNSManager(const NodeId &self_id, const crypto::KeyPair &keys);
        ~DNSManager();

        // ─── التسجيل ───

        // (AR) تسجيل اسم نطاق جديد
        Result<DNSRecord> register_name(const std::string &name,
                                        RecordType type,
                                        const std::string &value,
                                        uint32_t ttl = 3600);

        // (AR) تسجيل خدمة (SRV)
        Result<DNSRecord> register_service(const std::string &name,
                                           const Endpoint &endpoint,
                                           uint32_t ttl = 3600);

        // (AR) تحديث سجل موجود
        Result<void> update_record(const std::string &name,
                                   const std::string &new_value);

        // (AR) حذف سجل
        Result<void> remove_record(const std::string &name);

        // ─── الحل ───

        // (AR) حل اسم إلى سجلات
        std::vector<DNSRecord> resolve(const std::string &name) const;

        // (AR) حل اسم من نوع محدد
        std::vector<DNSRecord> resolve(const std::string &name,
                                       RecordType type) const;

        // ─── التخزين المحلي ───

        // (AR) إضافة سجل إلى الذاكرة المؤقتة
        void cache_record(const DNSRecord &record);

        // (AR) مسح الذاكرة المؤقتة
        void clear_cache();

        // (AR) تنظيف السجلات المنتهية
        void cleanup_expired();

        // (AR) التحقق من صحة سجل
        bool verify_record(const DNSRecord &record) const;

        // (AR) جميع السجلات المحلية
        std::vector<DNSRecord> local_records() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::dns

#endif // SAD_SADNET_DNS_DNS_H
