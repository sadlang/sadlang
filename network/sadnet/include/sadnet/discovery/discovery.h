// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة اكتشاف الأقران
// (AR) اكتشاف الأقران في الشبكة المحلية (LAN) عبر البث المتعدد
//      واكتشاف عبر DHT (bootstrap) وتبادل الأقران (PEX)
// (EN) Peer discovery: LAN multicast, DHT bootstrap, PEX// المكون: network/sadnet (اكتشاف الأقران)
// المسار: network/sadnet/include/sadnet/discovery/discovery.h// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_DISCOVERY_DISCOVERY_H
#define SAD_SADNET_DISCOVERY_DISCOVERY_H

#include "sadnet/core/types.h"
#include "sadnet/core/transport.h"
#include "sadnet/dht/dht.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sad::net::discovery
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::Endpoint;
    using sad::net::NetError;
    using sad::net::NodeId;
    using sad::net::Result;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ثوابت الاكتشاف
    // ═════════════════════════════════════════════════════════════════════

    constexpr const char *MULTICAST_GROUP = "239.255.42.99";
    constexpr uint16_t MULTICAST_PORT = 7655;
    constexpr uint32_t ANNOUNCE_INTERVAL = 30000; // 30 ثانية
    constexpr uint32_t PEER_EXPIRY = 120000;      // دقيقتان

    // ═════════════════════════════════════════════════════════════════════
    // (AR) معلومات القرين المكتشف
    // ═════════════════════════════════════════════════════════════════════

    struct PeerInfo
    {
        NodeId id;
        Endpoint endpoint;
        uint64_t discovered_at = 0;
        uint64_t last_seen = 0;
        std::string service_name; // اسم الخدمة (اختياري)
        uint16_t protocol_version = 1;

        bool is_expired() const
        {
            return now_ms() - last_seen > PEER_EXPIRY;
        }
    };

    // (AR) معالج اكتشاف قرين جديد
    using PeerDiscoveredCallback = std::function<void(const PeerInfo &peer)>;
    using PeerLostCallback = std::function<void(const NodeId &id)>;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) اكتشاف الشبكة المحلية (LAN Discovery)
    //      عبر البث المتعدد (Multicast) على مجموعة 239.255.42.99:7655
    // ═════════════════════════════════════════════════════════════════════

    class LANDiscovery
    {
    public:
        LANDiscovery(const NodeId &self_id, uint16_t service_port);
        ~LANDiscovery();

        // (AR) بدء الإعلان والاستماع
        void start();
        void stop();
        bool is_running() const;

        // (AR) الحصول على قائمة الأقران المكتشفين
        std::vector<PeerInfo> get_peers() const;

        // (AR) معالجات الأحداث
        void on_peer_discovered(PeerDiscoveredCallback callback);
        void on_peer_lost(PeerLostCallback callback);

        // (AR) تعيين اسم الخدمة (يُضمّن في الإعلانات)
        void set_service_name(const std::string &name);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تبادل الأقران (Peer Exchange — PEX)
    //      مشاركة قوائم الأقران المعروفين مع الأقران الآخرين
    // ═════════════════════════════════════════════════════════════════════

    // أنواع رسائل PEX
    enum class PEXType : uint8_t
    {
        REQUEST = 1,  // طلب قائمة أقران
        RESPONSE = 2, // استجابة بقائمة أقران
        ANNOUNCE = 3  // إعلان تلقائي
    };

    struct PEXMessage
    {
        PEXType type;
        NodeId sender;
        std::vector<PeerInfo> peers;

        Buffer serialize() const;
        static PEXMessage deserialize(const uint8_t *data, size_t len);
    };

    class PeerExchange
    {
    public:
        PeerExchange(const NodeId &self_id);
        ~PeerExchange();

        // (AR) إضافة قرين معروف
        void add_known_peer(const PeerInfo &peer);

        // (AR) إزالة قرين
        void remove_peer(const NodeId &id);

        // (AR) بناء رسالة استجابة PEX (يُرسل عدداً محدوداً من الأقران)
        PEXMessage build_response(uint32_t max_peers = 20) const;

        // (AR) معالجة رسالة PEX واردة
        std::vector<PeerInfo> process_message(const PEXMessage &msg);

        // (AR) الأقران المعروفون
        std::vector<PeerInfo> get_known_peers() const;

        // (AR) تنظيف الأقران المنتهية صلاحيتهم
        void cleanup_expired();

        void on_peer_discovered(PeerDiscoveredCallback callback);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير الاكتشاف الموحد — يدمج جميع آليات الاكتشاف
    //      LAN + DHT + PEX في واجهة واحدة
    // ═════════════════════════════════════════════════════════════════════

    class DiscoveryManager
    {
    public:
        DiscoveryManager(const NodeId &self_id, uint16_t service_port);
        ~DiscoveryManager();

        // (AR) بدء جميع آليات الاكتشاف
        void start();
        void stop();

        // (AR) إضافة عقدة بذرية DHT
        void add_dht_seed(const Endpoint &seed);

        // (AR) إضافة خادم STUN لاكتشاف العنوان الخارجي
        void set_stun_server(const Endpoint &stun);

        // (AR) الحصول على جميع الأقران المكتشفين
        std::vector<PeerInfo> get_all_peers() const;

        // (AR) معالجات الأحداث
        void on_peer_discovered(PeerDiscoveredCallback callback);
        void on_peer_lost(PeerLostCallback callback);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::discovery

#endif // SAD_SADNET_DISCOVERY_DISCOVERY_H
