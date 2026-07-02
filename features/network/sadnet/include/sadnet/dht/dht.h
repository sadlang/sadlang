// ===================================================================
// صNet — جدول التجزئة الموزع (DHT) المبني على Kademlia
// (AR) واجهة DHT: جدول توجيه، تخزين/استرجاع، بروتوكول RPC
//      - معرّفات 256 بت (SHA-256 من المفتاح العام)
//      - 256 دلو k (k=20) لجدول التوجيه
//      - بروتوكول: PING, FIND_NODE, FIND_VALUE, STORE
// (EN) Kademlia DHT: routing table, store/lookup, RPC protocol
// المكون: features/network/sadnet (جدول التجزئة الموزع DHT)
// المسار: features/network/sadnet/include/sadnet/dht/dht.h
// ===================================================================
#ifndef SAD_SADNET_DHT_DHT_H
#define SAD_SADNET_DHT_DHT_H

#include "sadnet/core/types.h"
#include "sadnet/core/transport.h"
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace sad::net::dht
{

    // ─────────────────────────────────────────────────────────────────────
    // (AR) ثوابت Kademlia
    // ─────────────────────────────────────────────────────────────────────

    /// عدد العُقد القصوى في كل دلو
    constexpr size_t K_BUCKET_SIZE = 20;

    /// عدد أقرب عُقد يُطلب في FIND_NODE
    constexpr size_t ALPHA = 3;

    /// عدد الأدلّة (يساوي حجم المعرّف بالبتّات)
    constexpr size_t NUM_BUCKETS = NODE_ID_SIZE * 8;

    /// مهلة الانتظار لاستجابة RPC (مللي ثانية)
    constexpr uint32_t RPC_TIMEOUT_MS = 5000;

    /// مدة صلاحية القيم المُخزّنة (ثانية)
    constexpr uint32_t DEFAULT_VALUE_TTL = 3600;

    // ─────────────────────────────────────────────────────────────────────
    // (AR) معلومات عُقدة في الشبكة
    // ─────────────────────────────────────────────────────────────────────

    struct NodeInfo
    {
        NodeId id;               // معرّف العُقدة (256 بت)
        Endpoint endpoint;       // عنوان الشبكة
        uint64_t last_seen = 0;  // آخر وقت اتصال ناجح
        uint32_t fail_count = 0; // عدد مرات الفشل المتتالية
        bool is_good = false;    // هل العُقدة "جيدة" (استجابت مؤخراً)

        bool operator==(const NodeInfo &o) const { return id == o.id; }
    };

    // ─────────────────────────────────────────────────────────────────────
    // (AR) قيمة مُخزّنة في DHT
    // ─────────────────────────────────────────────────────────────────────

    struct DHTValue
    {
        Buffer data;                      // البيانات المُخزّنة
        NodeId publisher;                 // العُقدة التي نشرت القيمة
        uint64_t stored_at = 0;           // وقت التخزين
        uint32_t ttl = DEFAULT_VALUE_TTL; // مدة الصلاحية بالثواني

        bool is_expired() const
        {
            return (now_ms() - stored_at) / 1000 > ttl;
        }
    };

    // ─────────────────────────────────────────────────────────────────────
    // (AR) دلو k (K-Bucket) — يحتوي على أقرب العُقد في نطاق XOR معيّن
    //      ترتيب: الأقدم أولاً (LRU: least-recently-seen في المقدمة)
    // ─────────────────────────────────────────────────────────────────────

    class KBucket
    {
        std::vector<NodeInfo> nodes_;
        mutable std::mutex mutex_;

    public:
        /// إضافة أو تحديث عُقدة في الدلو
        /// يُرجع true إذا تمت الإضافة أو التحديث
        bool add_or_update(const NodeInfo &node);

        /// إزالة عُقدة بمعرّفها
        bool remove(const NodeId &id);

        /// إيجاد عُقدة بمعرّفها
        const NodeInfo *find(const NodeId &id) const;

        /// الحصول على جميع العُقد (نسخة)
        std::vector<NodeInfo> get_all() const;

        /// إزالة العُقد القديمة/الميتة
        void evict_stale(uint64_t max_age_ms);

        /// عدد العُقد
        size_t size() const;

        /// هل الدلو ممتلئ؟
        bool is_full() const;
    };

    // ─────────────────────────────────────────────────────────────────────
    // (AR) جدول التوجيه — 256 دلو k مرتبة حسب مسافة XOR
    // ─────────────────────────────────────────────────────────────────────

    class RoutingTable
    {
        NodeId self_id_;
        std::array<KBucket, NUM_BUCKETS> buckets_;

    public:
        explicit RoutingTable(const NodeId &self);

        /// تحديد فهرس الدلو المناسب لمعرّف
        size_t bucket_index(const NodeId &target) const;

        /// إضافة أو تحديث عُقدة
        bool add_or_update(const NodeInfo &node);

        /// إيجاد أقرب k عُقدة لمعرّف مُعطى
        std::vector<NodeInfo> find_closest(const NodeId &target,
                                           size_t count = K_BUCKET_SIZE) const;

        /// إزالة عُقدة
        bool remove(const NodeId &id);

        /// عدد العُقد الإجمالي
        size_t total_nodes() const;

        /// تنظيف العُقد القديمة
        void cleanup(uint64_t max_age_ms = 900000); // 15 دقيقة

        const NodeId &self_id() const { return self_id_; }
    };

    // ─────────────────────────────────────────────────────────────────────
    // (AR) أنواع رسائل RPC
    // ─────────────────────────────────────────────────────────────────────

    enum class RPCType : uint8_t
    {
        PING = 0x01,
        PONG = 0x02,
        FIND_NODE = 0x03,
        FIND_NODE_RESPONSE = 0x04,
        FIND_VALUE = 0x05,
        FIND_VALUE_RESPONSE = 0x06,
        STORE = 0x07,
        STORE_RESPONSE = 0x08
    };

    /// رسالة RPC
    struct RPCMessage
    {
        RPCType type;
        NodeId sender_id;
        uint32_t transaction_id = 0;

        // حمولة متعددة الأغراض
        NodeId target_key;           // لـ FIND_NODE / FIND_VALUE
        Buffer value;                // لـ STORE / FIND_VALUE_RESPONSE
        std::vector<NodeInfo> nodes; // لـ FIND_NODE_RESPONSE

        /// تسلسل إلى حزمة ثنائية
        Buffer serialize() const;

        /// فك تسلسل من حزمة ثنائية
        static RPCMessage deserialize(const uint8_t *data, size_t len);
    };

    // ─────────────────────────────────────────────────────────────────────
    // (AR) محرك DHT الرئيسي — يُدير جدول التوجيه والتخزين والبحث
    // ─────────────────────────────────────────────────────────────────────

    class DHTEngine
    {
    public:
        /// إنشاء محرك DHT مع معرّف العُقدة ومنفذ الاستماع
        DHTEngine(const NodeId &self_id, uint16_t port);
        ~DHTEngine();

        // ─── عمليات DHT الأساسية الأربعة ───

        /// PING: التحقق من أن عُقدة حيّة
        Result<void> ping(const Endpoint &target);

        /// STORE: تخزين قيمة في الشبكة (يُخزّن على أقرب k عُقدة للمفتاح)
        Result<void> store(const NodeId &key, const Buffer &value);

        /// FIND_NODE: إيجاد أقرب k عُقدة لمعرّف مُعطى
        Result<std::vector<NodeInfo>> find_node(const NodeId &target);

        /// FIND_VALUE: البحث عن قيمة مُخزّنة بمفتاحها
        Result<Buffer> find_value(const NodeId &key);

        // ─── إدارة الشبكة ───

        /// الانضمام للشبكة عبر عُقدة بذرية (bootstrap)
        Result<void> bootstrap(const Endpoint &seed);

        /// بدء الاستماع للطلبات الواردة (حلقة الاستقبال)
        void start();

        /// إيقاف المحرك
        void stop();

        /// هل المحرك يعمل؟
        bool is_running() const;

        /// الحصول على جدول التوجيه (للقراءة فقط)
        const RoutingTable &routing_table() const;

        /// عدد القيم المُخزّنة محلياً
        size_t stored_values_count() const;

        /// تعيين معالج للطلبات الواردة (اختياري — للتوسيع)
        using RequestHandler = std::function<void(const RPCMessage &, const Endpoint &)>;
        void set_custom_handler(RequestHandler handler);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::dht

#endif // SAD_SADNET_DHT_DHT_H
