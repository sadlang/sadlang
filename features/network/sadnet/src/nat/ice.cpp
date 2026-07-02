// ═══════════════════════════════════════════════════════════════════
// صNet — وكيل ICE: تجميع مرشحين، فحص اتصال، اختيار المسار
// (AR) تنفيذ RFC 8445 (ICE) — إطار تفاوض الاتصال
//      يجمع مرشحات HOST + SRFLX + RELAY ثم يفحص أزواج الاتصال
// (EN) ICE agent: candidate gathering, connectivity checks, nomination
// المكون: features/network/sadnet (وكيل ICE)
// المسار: features/network/sadnet/src/nat/ice.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/nat/nat.h"
#include "sadnet/crypto/crypto.h"
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#else
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#endif

namespace sad::net::nat
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ICECandidate — حساب الأولوية
    //      حسب RFC 8445 Section 5.1.2.1:
    //      priority = (2^24 × type_preference) + (2^8 × local_preference) + (256 - component_id)
    // ═════════════════════════════════════════════════════════════════════

    uint32_t ICECandidate::compute_priority(CandidateType type, uint16_t local_pref)
    {
        uint32_t type_pref;
        switch (type)
        {
        case CandidateType::HOST:
            type_pref = 126;
            break;
        case CandidateType::SRFLX:
            type_pref = 100;
            break;
        case CandidateType::PRFLX:
            type_pref = 110;
            break;
        case CandidateType::RELAY:
            type_pref = 0;
            break;
        }
        return (type_pref << 24) + ((uint32_t)local_pref << 8) + 255;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) CandidatePair — حساب الأولوية المركبة
    //      حسب RFC 8445 Section 6.1.2.3:
    //      pair_priority = 2^32×min(G,D) + 2×max(G,D) + (G>D ? 1 : 0)
    //      G = controlling priority, D = controlled priority
    // ═════════════════════════════════════════════════════════════════════

    uint64_t CandidatePair::compute_pair_priority(uint32_t controlling_prio,
                                                  uint32_t controlled_prio,
                                                  bool is_controlling)
    {
        uint32_t G = is_controlling ? controlling_prio : controlled_prio;
        uint32_t D = is_controlling ? controlled_prio : controlling_prio;

        uint64_t min_val = (G < D) ? G : D;
        uint64_t max_val = (G > D) ? G : D;

        return (min_val << 32) + 2 * max_val + (G > D ? 1 : 0);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) جمع العناوين المحلية — متعدد المنصات
    // ═════════════════════════════════════════════════════════════════════

    static std::vector<std::string> get_local_addresses()
    {
        std::vector<std::string> addrs;

#ifdef _WIN32
        // Windows: استخدام GetAdaptersAddresses
        ULONG buf_size = 16384;
        auto buf = std::make_unique<uint8_t[]>(buf_size);
        auto *adapter = reinterpret_cast<IP_ADAPTER_ADDRESSES *>(buf.get());

        ULONG flags = GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_MULTICAST;
        if (GetAdaptersAddresses(AF_INET, flags, nullptr, adapter, &buf_size) == NO_ERROR)
        {
            for (auto *a = adapter; a; a = a->Next)
            {
                if (a->OperStatus != IfOperStatusUp)
                    continue;
                for (auto *ua = a->FirstUnicastAddress; ua; ua = ua->Next)
                {
                    auto *sa = reinterpret_cast<sockaddr_in *>(ua->Address.lpSockaddr);
                    if (sa->sin_family == AF_INET)
                    {
                        char ip[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &sa->sin_addr, ip, sizeof(ip));
                        std::string s(ip);
                        if (s != "127.0.0.1")
                            addrs.push_back(s);
                    }
                }
            }
        }
#else
        // Linux/macOS: استخدام getifaddrs
        struct ifaddrs *ifa_list = nullptr;
        if (getifaddrs(&ifa_list) == 0)
        {
            for (auto *ifa = ifa_list; ifa; ifa = ifa->ifa_next)
            {
                if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET)
                    continue;
                if (!(ifa->ifa_flags & IFF_UP))
                    continue;
                auto *sa = reinterpret_cast<sockaddr_in *>(ifa->ifa_addr);
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &sa->sin_addr, ip, sizeof(ip));
                std::string s(ip);
                if (s != "127.0.0.1")
                    addrs.push_back(s);
            }
            freeifaddrs(ifa_list);
        }
#endif

        return addrs;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) التنفيذ الداخلي لوكيل ICE
    // ═════════════════════════════════════════════════════════════════════

    struct ICEAgent::Impl
    {
        bool controlling;
        ICEState current_state = ICEState::NEW;
        ICEStateCallback state_callback;

        std::vector<ICECandidate> local_candidates;
        std::vector<ICECandidate> remote_candidates;
        std::vector<CandidatePair> pairs;

        const CandidatePair *selected = nullptr;

        void set_state(ICEState s)
        {
            current_state = s;
            if (state_callback)
                state_callback(s, selected);
        }

        void form_pairs()
        {
            pairs.clear();
            for (const auto &lc : local_candidates)
            {
                for (const auto &rc : remote_candidates)
                {
                    CandidatePair pair;
                    pair.local = lc;
                    pair.remote = rc;
                    pair.priority = CandidatePair::compute_pair_priority(
                        lc.priority, rc.priority, controlling);
                    pair.state = CandidatePair::State::FROZEN;
                    pairs.push_back(pair);
                }
            }
            // ترتيب تنازلي حسب الأولوية
            std::sort(pairs.begin(), pairs.end(),
                      [](const CandidatePair &a, const CandidatePair &b)
                      {
                          return a.priority > b.priority;
                      });
        }
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) واجهات ICEAgent العامة
    // ═════════════════════════════════════════════════════════════════════

    ICEAgent::ICEAgent(bool controlling)
        : impl_(std::make_unique<Impl>())
    {
        impl_->controlling = controlling;
    }

    ICEAgent::~ICEAgent() = default;

    Result<std::vector<ICECandidate>> ICEAgent::gather_candidates(
        uint16_t local_port,
        const Endpoint *stun_server,
        TurnClient *turn_client)
    {

        impl_->set_state(ICEState::GATHERING);
        impl_->local_candidates.clear();

        uint32_t foundation_counter = 0;

        // ─── 1. مرشحات HOST ───
        auto local_addrs = get_local_addresses();
        for (const auto &ip : local_addrs)
        {
            ICECandidate c;
            c.type = CandidateType::HOST;
            c.address = Endpoint{ip, local_port};
            c.priority = ICECandidate::compute_priority(CandidateType::HOST);
            c.foundation = "h" + std::to_string(++foundation_counter);
            impl_->local_candidates.push_back(c);
        }

        // ─── 2. مرشحات SRFLX (عبر STUN) ───
        if (stun_server)
        {
            UdpSocket stun_sock;
            stun_sock.bind(0); // منفذ عشوائي

            auto stun_result = StunClient::binding_request(stun_sock, *stun_server);
            if (stun_result.success)
            {
                ICECandidate c;
                c.type = CandidateType::SRFLX;
                c.address = stun_result.mapped_address;
                c.priority = ICECandidate::compute_priority(CandidateType::SRFLX);
                c.foundation = "s" + std::to_string(++foundation_counter);
                impl_->local_candidates.push_back(c);
            }
        }

        // ─── 3. مرشحات RELAY (عبر TURN) ───
        if (turn_client && turn_client->is_allocated())
        {
            // لدينا تخصيص TURN نشط — نضيف العنوان المرحّل
            // (التخصيص يتم خارج ICE — هنا نسجل فقط)
            // ملاحظة: في الواقع نحتاج الوصول للعنوان. نتركه للمستدعي
            // لإضافة مرشح RELAY يدوياً إذا أراد
        }

        if (impl_->local_candidates.empty())
        {
            impl_->set_state(ICEState::FAILED);
            return Result<std::vector<ICECandidate>>::fail(NetError::NETWORK_UNREACHABLE);
        }

        return Result<std::vector<ICECandidate>>::success(impl_->local_candidates);
    }

    void ICEAgent::add_remote_candidate(const ICECandidate &candidate)
    {
        impl_->remote_candidates.push_back(candidate);
    }

    Result<void> ICEAgent::start_checks(UdpSocket &sock)
    {
        if (impl_->local_candidates.empty() || impl_->remote_candidates.empty())
            return Result<void>::fail(NetError::NOT_FOUND);

        impl_->form_pairs();
        impl_->set_state(ICEState::CHECKING);

        // فحص الاتصال: إرسال STUN Binding Request لكل زوج
        for (auto &pair : impl_->pairs)
        {
            pair.state = CandidatePair::State::IN_PROGRESS;

            uint8_t tx_id[12];
            crypto::random_bytes(tx_id, 12);
            auto pkt = StunClient::build_binding_request(tx_id);

            sock.set_recv_timeout(2000);
            auto send_r = sock.send_to(pair.remote.address, pkt);
            if (!send_r.ok())
            {
                pair.state = CandidatePair::State::FAILED;
                continue;
            }

            auto recv_r = sock.recv_from();
            if (!recv_r.ok())
            {
                pair.state = CandidatePair::State::FAILED;
                continue;
            }

            auto result = StunClient::parse_binding_response(
                recv_r.value.data.data(), recv_r.value.data.size(), tx_id);

            if (result.success)
            {
                pair.state = CandidatePair::State::SUCCEEDED;
                impl_->selected = &pair;
                impl_->set_state(ICEState::CONNECTED);
                return Result<void>::success();
            }
            else
            {
                pair.state = CandidatePair::State::FAILED;
            }
        }

        // لم ينجح أي زوج
        impl_->set_state(ICEState::FAILED);
        return Result<void>::fail(NetError::CONNECTION_REFUSED);
    }

    const CandidatePair *ICEAgent::selected_pair() const
    {
        return impl_->selected;
    }

    ICEState ICEAgent::state() const
    {
        return impl_->current_state;
    }

    void ICEAgent::on_state_change(ICEStateCallback callback)
    {
        impl_->state_callback = std::move(callback);
    }

    void ICEAgent::cancel()
    {
        impl_->set_state(ICEState::DISCONNECTED);
        impl_->selected = nullptr;
    }

} // namespace sad::net::nat
