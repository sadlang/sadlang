// ═══════════════════════════════════════════════════════════════════
// صNet — اكتشاف الأقران: LAN Multicast + PEX + مدير موحد
// (AR) اكتشاف محلي عبر البث المتعدد (multicast)
//      تبادل أقران (PEX) مع ترميز ثنائي
//      مدير موحد يدمج جميع مصادر الاكتشاف
// (EN) Peer discovery: LAN multicast, PEX, unified discovery manager
// المكون: network/sadnet (اكتشاف الأقران)
// المسار: network/sadnet/src/discovery/discovery.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/discovery/discovery.h"
#include "sadnet/crypto/crypto.h"
#include <algorithm>
#include <atomic>
#include <cstring>
#include <map>
#include <mutex>
#include <thread>

namespace sad::net::discovery
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مساعدات الترميز
    // ═════════════════════════════════════════════════════════════════════

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
    static uint16_t r16(const uint8_t *p)
    {
        return ((uint16_t)p[0] << 8) | p[1];
    }
    static uint32_t r32(const uint8_t *p)
    {
        return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
               ((uint32_t)p[2] << 8) | p[3];
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) اكتشاف الشبكة المحلية (LAN Discovery)
    //      بروتوكول الإعلان:
    //      [magic:4][version:2][node_id:32][port:2][service_len:2][service:N]
    // ═════════════════════════════════════════════════════════════════════

    constexpr uint32_t LAN_MAGIC = 0x534E4554; // "SNET"

    struct LANDiscovery::Impl
    {
        NodeId self_id;
        uint16_t service_port;
        std::string service_name;

        UdpSocket socket;
        std::atomic<bool> running{false};
        std::thread announce_thread;
        std::thread listen_thread;

        std::map<NodeId, PeerInfo> peers;
        mutable std::mutex peers_mutex;

        PeerDiscoveredCallback on_discovered;
        PeerLostCallback on_lost;

        Impl(const NodeId &id, uint16_t port) : self_id(id), service_port(port) {}

        Buffer build_announce()
        {
            size_t svc_len = service_name.size();
            Buffer buf(4 + 2 + NODE_ID_SIZE + 2 + 2 + svc_len);
            uint8_t *p = buf.data();

            w32(p, LAN_MAGIC);
            w16(p + 4, 1); // إصدار البروتوكول
            std::memcpy(p + 6, self_id.data.data(), NODE_ID_SIZE);
            w16(p + 6 + NODE_ID_SIZE, service_port);
            w16(p + 8 + NODE_ID_SIZE, (uint16_t)svc_len);
            if (svc_len > 0)
                std::memcpy(p + 10 + NODE_ID_SIZE, service_name.c_str(), svc_len);

            return buf;
        }

        void parse_announce(const uint8_t *data, size_t len, const Endpoint &sender)
        {
            if (len < 4 + 2 + NODE_ID_SIZE + 2 + 2)
                return;

            if (r32(data) != LAN_MAGIC)
                return;
            // uint16_t version = r16(data + 4);

            NodeId id;
            std::memcpy(id.data.data(), data + 6, NODE_ID_SIZE);
            if (id == self_id)
                return; // تجاهل إعلاناتنا

            uint16_t port = r16(data + 6 + NODE_ID_SIZE);
            uint16_t svc_len = r16(data + 8 + NODE_ID_SIZE);

            std::string svc;
            if (svc_len > 0 && 10 + NODE_ID_SIZE + svc_len <= len)
            {
                svc.assign(reinterpret_cast<const char *>(data + 10 + NODE_ID_SIZE), svc_len);
            }

            std::lock_guard<std::mutex> lock(peers_mutex);
            auto it = peers.find(id);
            if (it == peers.end())
            {
                PeerInfo pi;
                pi.id = id;
                pi.endpoint = Endpoint{sender.address, port};
                pi.discovered_at = now_ms();
                pi.last_seen = now_ms();
                pi.service_name = svc;
                peers[id] = pi;
                if (on_discovered)
                    on_discovered(pi);
            }
            else
            {
                it->second.last_seen = now_ms();
                it->second.endpoint = Endpoint{sender.address, port};
            }
        }

        void announce_loop()
        {
            while (running.load())
            {
                auto pkt = build_announce();
                Endpoint mcast{MULTICAST_GROUP, MULTICAST_PORT};
                socket.send_to(mcast, pkt);

                // تنظيف المنتهي
                {
                    std::lock_guard<std::mutex> lock(peers_mutex);
                    for (auto it = peers.begin(); it != peers.end();)
                    {
                        if (it->second.is_expired())
                        {
                            if (on_lost)
                                on_lost(it->first);
                            it = peers.erase(it);
                        }
                        else
                        {
                            ++it;
                        }
                    }
                }

                // انتظار فترة الإعلان
                for (uint32_t elapsed = 0;
                     elapsed < ANNOUNCE_INTERVAL && running.load();
                     elapsed += 500)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
        }

        void listen_loop()
        {
            socket.set_recv_timeout(1000);
            while (running.load())
            {
                auto r = socket.recv_from();
                if (r.ok())
                {
                    parse_announce(r.value.data.data(), r.value.data.size(),
                                   r.value.sender);
                }
            }
        }
    };

    LANDiscovery::LANDiscovery(const NodeId &self_id, uint16_t service_port)
        : impl_(std::make_unique<Impl>(self_id, service_port)) {}

    LANDiscovery::~LANDiscovery() { stop(); }

    void LANDiscovery::start()
    {
        if (impl_->running.load())
            return;

        impl_->socket.bind(MULTICAST_PORT);
        impl_->socket.join_multicast(MULTICAST_GROUP);

        impl_->running = true;
        impl_->announce_thread = std::thread([this]
                                             { impl_->announce_loop(); });
        impl_->listen_thread = std::thread([this]
                                           { impl_->listen_loop(); });
    }

    void LANDiscovery::stop()
    {
        impl_->running = false;
        if (impl_->announce_thread.joinable())
            impl_->announce_thread.join();
        if (impl_->listen_thread.joinable())
            impl_->listen_thread.join();
    }

    bool LANDiscovery::is_running() const { return impl_->running.load(); }

    std::vector<PeerInfo> LANDiscovery::get_peers() const
    {
        std::lock_guard<std::mutex> lock(impl_->peers_mutex);
        std::vector<PeerInfo> result;
        for (const auto &kv : impl_->peers)
            if (!kv.second.is_expired())
                result.push_back(kv.second);
        return result;
    }

    void LANDiscovery::on_peer_discovered(PeerDiscoveredCallback cb)
    {
        impl_->on_discovered = std::move(cb);
    }
    void LANDiscovery::on_peer_lost(PeerLostCallback cb)
    {
        impl_->on_lost = std::move(cb);
    }
    void LANDiscovery::set_service_name(const std::string &name)
    {
        impl_->service_name = name;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تبادل الأقران (PEX)
    //      ترميز ثنائي: [type:1][sender:32][count:2][peers...]
    //      كل قرين: [id:32][ip_len:2][ip:N][port:2]
    // ═════════════════════════════════════════════════════════════════════

    Buffer PEXMessage::serialize() const
    {
        // حساب الحجم
        size_t total = 1 + NODE_ID_SIZE + 2; // type + sender + count
        for (const auto &p : peers)
            total += NODE_ID_SIZE + 2 + p.endpoint.address.size() + 2;

        Buffer buf(total);
        uint8_t *ptr = buf.data();

        *ptr++ = (uint8_t)type;
        std::memcpy(ptr, sender.data.data(), NODE_ID_SIZE);
        ptr += NODE_ID_SIZE;
        w16(ptr, (uint16_t)peers.size());
        ptr += 2;

        for (const auto &p : peers)
        {
            std::memcpy(ptr, p.id.data.data(), NODE_ID_SIZE);
            ptr += NODE_ID_SIZE;
            w16(ptr, (uint16_t)p.endpoint.address.size());
            ptr += 2;
            std::memcpy(ptr, p.endpoint.address.c_str(), p.endpoint.address.size());
            ptr += p.endpoint.address.size();
            w16(ptr, p.endpoint.port);
            ptr += 2;
        }

        return buf;
    }

    PEXMessage PEXMessage::deserialize(const uint8_t *data, size_t len)
    {
        PEXMessage msg;
        if (len < 1 + NODE_ID_SIZE + 2)
            return msg;

        msg.type = (PEXType)data[0];
        std::memcpy(msg.sender.data.data(), data + 1, NODE_ID_SIZE);
        uint16_t count = r16(data + 1 + NODE_ID_SIZE);

        size_t off = 1 + NODE_ID_SIZE + 2;
        for (uint16_t i = 0; i < count && off < len; i++)
        {
            if (off + NODE_ID_SIZE + 2 > len)
                break;

            PeerInfo pi;
            std::memcpy(pi.id.data.data(), data + off, NODE_ID_SIZE);
            off += NODE_ID_SIZE;

            uint16_t ip_len = r16(data + off);
            off += 2;

            if (off + ip_len + 2 > len)
                break;
            pi.endpoint.address.assign(reinterpret_cast<const char *>(data + off), ip_len);
            off += ip_len;

            pi.endpoint.port = r16(data + off);
            off += 2;

            pi.last_seen = now_ms();
            pi.discovered_at = now_ms();
            msg.peers.push_back(pi);
        }

        return msg;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تنفيذ PeerExchange
    // ═════════════════════════════════════════════════════════════════════

    struct PeerExchange::Impl
    {
        NodeId self_id;
        std::map<NodeId, PeerInfo> known_peers;
        mutable std::mutex mutex;
        PeerDiscoveredCallback on_discovered;

        Impl(const NodeId &id) : self_id(id) {}
    };

    PeerExchange::PeerExchange(const NodeId &self_id)
        : impl_(std::make_unique<Impl>(self_id)) {}

    PeerExchange::~PeerExchange() = default;

    void PeerExchange::add_known_peer(const PeerInfo &peer)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->known_peers[peer.id] = peer;
    }

    void PeerExchange::remove_peer(const NodeId &id)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->known_peers.erase(id);
    }

    PEXMessage PeerExchange::build_response(uint32_t max_peers) const
    {
        PEXMessage msg;
        msg.type = PEXType::RESPONSE;
        msg.sender = impl_->self_id;

        std::lock_guard<std::mutex> lock(impl_->mutex);
        uint32_t count = 0;
        for (const auto &kv : impl_->known_peers)
        {
            if (count >= max_peers)
                break;
            if (!kv.second.is_expired())
            {
                msg.peers.push_back(kv.second);
                count++;
            }
        }

        return msg;
    }

    std::vector<PeerInfo> PeerExchange::process_message(const PEXMessage &msg)
    {
        std::vector<PeerInfo> new_peers;
        std::lock_guard<std::mutex> lock(impl_->mutex);

        for (const auto &p : msg.peers)
        {
            if (p.id == impl_->self_id)
                continue;
            auto it = impl_->known_peers.find(p.id);
            if (it == impl_->known_peers.end())
            {
                impl_->known_peers[p.id] = p;
                new_peers.push_back(p);
                if (impl_->on_discovered)
                    impl_->on_discovered(p);
            }
            else
            {
                it->second.last_seen = now_ms();
            }
        }

        return new_peers;
    }

    std::vector<PeerInfo> PeerExchange::get_known_peers() const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        std::vector<PeerInfo> result;
        for (const auto &kv : impl_->known_peers)
            if (!kv.second.is_expired())
                result.push_back(kv.second);
        return result;
    }

    void PeerExchange::cleanup_expired()
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        for (auto it = impl_->known_peers.begin(); it != impl_->known_peers.end();)
        {
            if (it->second.is_expired())
                it = impl_->known_peers.erase(it);
            else
                ++it;
        }
    }

    void PeerExchange::on_peer_discovered(PeerDiscoveredCallback cb)
    {
        impl_->on_discovered = std::move(cb);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير الاكتشاف الموحد — يدمج LAN + PEX
    // ═════════════════════════════════════════════════════════════════════

    struct DiscoveryManager::Impl
    {
        NodeId self_id;
        uint16_t port;

        LANDiscovery lan;
        PeerExchange pex;

        std::vector<Endpoint> dht_seeds;
        Endpoint stun_server;
        bool has_stun = false;

        PeerDiscoveredCallback on_discovered;
        PeerLostCallback on_lost;

        Impl(const NodeId &id, uint16_t p)
            : self_id(id), port(p), lan(id, p), pex(id) {}
    };

    DiscoveryManager::DiscoveryManager(const NodeId &self_id, uint16_t service_port)
        : impl_(std::make_unique<Impl>(self_id, service_port)) {}

    DiscoveryManager::~DiscoveryManager() { stop(); }

    void DiscoveryManager::start()
    {
        // ربط معالجات LAN
        impl_->lan.on_peer_discovered([this](const PeerInfo &p)
                                      {
        impl_->pex.add_known_peer(p);
        if (impl_->on_discovered) impl_->on_discovered(p); });
        impl_->lan.on_peer_lost([this](const NodeId &id)
                                {
        impl_->pex.remove_peer(id);
        if (impl_->on_lost) impl_->on_lost(id); });

        impl_->lan.start();
    }

    void DiscoveryManager::stop()
    {
        impl_->lan.stop();
    }

    void DiscoveryManager::add_dht_seed(const Endpoint &seed)
    {
        impl_->dht_seeds.push_back(seed);
    }

    void DiscoveryManager::set_stun_server(const Endpoint &stun)
    {
        impl_->stun_server = stun;
        impl_->has_stun = true;
    }

    std::vector<PeerInfo> DiscoveryManager::get_all_peers() const
    {
        // دمج أقران LAN و PEX بدون تكرار
        auto lan_peers = impl_->lan.get_peers();
        auto pex_peers = impl_->pex.get_known_peers();

        std::map<NodeId, PeerInfo> merged;
        for (auto &p : lan_peers)
            merged[p.id] = p;
        for (auto &p : pex_peers)
        {
            auto it = merged.find(p.id);
            if (it == merged.end())
                merged[p.id] = p;
            else if (p.last_seen > it->second.last_seen)
                it->second.last_seen = p.last_seen;
        }

        std::vector<PeerInfo> result;
        for (auto &kv : merged)
            result.push_back(kv.second);
        return result;
    }

    void DiscoveryManager::on_peer_discovered(PeerDiscoveredCallback cb)
    {
        impl_->on_discovered = std::move(cb);
    }
    void DiscoveryManager::on_peer_lost(PeerLostCallback cb)
    {
        impl_->on_lost = std::move(cb);
    }

} // namespace sad::net::discovery
