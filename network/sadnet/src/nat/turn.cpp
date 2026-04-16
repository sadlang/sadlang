// ═══════════════════════════════════════════════════════════════════
// صNet — عميل TURN: تخصيص، إذن، ربط قناة، ترحيل بيانات
// (AR) تنفيذ RFC 5766 (TURN) — ترحيل البيانات عبر خادم وسيط
//      يُستخدم عندما يفشل STUN (NAT متماثل مثلاً)
// (EN) TURN client: allocate, create permission, channel bind, relay
// المكون: network/sadnet (عميل TURN)
// المسار: network/sadnet/src/nat/turn.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/nat/nat.h"
#include "sadnet/crypto/crypto.h"
#include <cstring>
#include <map>
#include <mutex>

namespace sad::net::nat
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مساعدات ترميز البايتات
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

    // (AR) ترميز عنوان XOR — نفس خوارزمية STUN
    static void encode_xor_address(uint8_t *out, const Endpoint &ep)
    {
        out[0] = 0;    // padding
        out[1] = 0x01; // IPv4

        // تحليل العنوان IP
        uint32_t ip = 0;
        int a, b, c, d;
        if (sscanf(ep.address.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) == 4)
        {
            ip = ((uint32_t)a << 24) | ((uint32_t)b << 16) |
                 ((uint32_t)c << 8) | (uint32_t)d;
        }

        w16(out + 2, ep.port ^ (uint16_t)(STUN_MAGIC_COOKIE >> 16));
        w32(out + 4, ip ^ STUN_MAGIC_COOKIE);
    }

    // (AR) فك ترميز عنوان XOR
    static Endpoint decode_xor_address(const uint8_t *data)
    {
        Endpoint ep;
        ep.port = r16(data + 2) ^ (uint16_t)(STUN_MAGIC_COOKIE >> 16);
        uint32_t xor_ip = r32(data + 4) ^ STUN_MAGIC_COOKIE;
        ep.address = std::to_string((xor_ip >> 24) & 0xFF) + "." +
                     std::to_string((xor_ip >> 16) & 0xFF) + "." +
                     std::to_string((xor_ip >> 8) & 0xFF) + "." +
                     std::to_string(xor_ip & 0xFF);
        return ep;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) التنفيذ الداخلي لعميل TURN
    // ═════════════════════════════════════════════════════════════════════

    struct TurnClient::Impl
    {
        UdpSocket &socket;
        Endpoint server;
        TurnAllocation allocation;

        // قنوات مربوطة: channel_number → peer
        std::map<uint16_t, Endpoint> channels;
        uint16_t next_channel = 0x4000; // القنوات تبدأ من 0x4000
        std::mutex mutex;

        Impl(UdpSocket &s, const Endpoint &srv) : socket(s), server(srv) {}

        // إنشاء رأس STUN/TURN
        Buffer build_request(uint16_t type, const Buffer &body)
        {
            uint8_t tx_id[12];
            crypto::random_bytes(tx_id, 12);

            Buffer pkt(20 + body.size());
            uint8_t *p = pkt.data();
            w16(p, type);
            w16(p + 2, (uint16_t)body.size());
            w32(p + 4, STUN_MAGIC_COOKIE);
            std::memcpy(p + 8, tx_id, 12);
            if (!body.empty())
                std::memcpy(p + 20, body.data(), body.size());
            return pkt;
        }

        // إرسال واستقبال استجابة
        Buffer send_and_recv(const Buffer &pkt, uint32_t timeout_ms = 3000)
        {
            socket.set_recv_timeout(timeout_ms);
            socket.send_to(server, pkt);
            auto r = socket.recv_from();
            if (r.ok())
                return r.value.data;
            return Buffer();
        }

        // البحث عن سمة في استجابة STUN/TURN
        const uint8_t *find_attribute(const uint8_t *data, size_t len,
                                      uint16_t attr_type, uint16_t &attr_len)
        {
            if (len < 20)
                return nullptr;
            uint16_t msg_len = r16(data + 2);
            size_t off = 20;
            while (off + 4 <= 20 + msg_len && off + 4 <= len)
            {
                uint16_t at = r16(data + off);
                attr_len = r16(data + off + 2);
                if (at == attr_type && off + 4 + attr_len <= len)
                    return data + off + 4;
                off += 4 + attr_len;
                if (attr_len % 4 != 0)
                    off += 4 - (attr_len % 4);
            }
            return nullptr;
        }
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) واجهات TurnClient العامة
    // ═════════════════════════════════════════════════════════════════════

    TurnClient::TurnClient(UdpSocket &sock, const Endpoint &turn_server)
        : impl_(std::make_unique<Impl>(sock, turn_server)) {}

    TurnClient::~TurnClient() = default;

    Result<TurnAllocation> TurnClient::allocate()
    {
        // سمة REQUESTED-TRANSPORT: UDP (17)
        Buffer body(8);
        uint8_t *bp = body.data();
        w16(bp, TURN_ATTR_REQUESTED_TRANSPORT);
        w16(bp + 2, 4);
        bp[4] = 17; // UDP
        bp[5] = bp[6] = bp[7] = 0;

        auto pkt = impl_->build_request(TURN_ALLOCATE_REQUEST, body);
        auto resp = impl_->send_and_recv(pkt);

        if (resp.empty())
            return Result<TurnAllocation>::fail(NetError::TIMEOUT);

        // تحقق نوع الاستجابة
        uint16_t resp_type = r16(resp.data());
        if (resp_type != TURN_ALLOCATE_RESPONSE)
            return Result<TurnAllocation>::fail(NetError::PROTOCOL_ERROR);

        // البحث عن XOR-RELAYED-ADDRESS
        uint16_t attr_len = 0;
        auto relay_attr = impl_->find_attribute(resp.data(), resp.size(),
                                                TURN_ATTR_XOR_RELAY_ADDR, attr_len);
        if (!relay_attr || attr_len < 8)
            return Result<TurnAllocation>::fail(NetError::PROTOCOL_ERROR);

        TurnAllocation alloc;
        alloc.relay_address = decode_xor_address(relay_attr);
        alloc.valid = true;

        // البحث عن LIFETIME
        auto life_attr = impl_->find_attribute(resp.data(), resp.size(),
                                               TURN_ATTR_LIFETIME, attr_len);
        if (life_attr && attr_len >= 4)
            alloc.lifetime = r32(life_attr);

        alloc.expires_at = now_ms() + (uint64_t)alloc.lifetime * 1000;
        impl_->allocation = alloc;

        return Result<TurnAllocation>::success(std::move(alloc));
    }

    Result<void> TurnClient::refresh(uint32_t lifetime)
    {
        Buffer body(8);
        uint8_t *bp = body.data();
        w16(bp, TURN_ATTR_LIFETIME);
        w16(bp + 2, 4);
        w32(bp + 4, lifetime);

        auto pkt = impl_->build_request(TURN_REFRESH_REQUEST, body);
        auto resp = impl_->send_and_recv(pkt);

        if (resp.empty())
            return Result<void>::fail(NetError::TIMEOUT);

        uint16_t resp_type = r16(resp.data());
        if (resp_type != TURN_REFRESH_RESPONSE)
            return Result<void>::fail(NetError::PROTOCOL_ERROR);

        impl_->allocation.lifetime = lifetime;
        impl_->allocation.expires_at = now_ms() + (uint64_t)lifetime * 1000;
        return Result<void>::success();
    }

    Result<void> TurnClient::create_permission(const Endpoint &peer)
    {
        Buffer body(12);
        uint8_t *bp = body.data();
        w16(bp, TURN_ATTR_XOR_PEER_ADDR);
        w16(bp + 2, 8);
        encode_xor_address(bp + 4, peer);

        auto pkt = impl_->build_request(TURN_CREATE_PERM_REQ, body);
        auto resp = impl_->send_and_recv(pkt);

        if (resp.empty())
            return Result<void>::fail(NetError::TIMEOUT);

        uint16_t resp_type = r16(resp.data());
        if (resp_type != TURN_CREATE_PERM_RESP)
            return Result<void>::fail(NetError::PROTOCOL_ERROR);

        return Result<void>::success();
    }

    Result<uint16_t> TurnClient::channel_bind(const Endpoint &peer)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        uint16_t ch = impl_->next_channel++;

        // سمة CHANNEL-NUMBER (4 بايت: [رقم:2][padding:2])
        // + سمة XOR-PEER-ADDRESS (12 بايت: [type:2][len:2][addr:8])
        Buffer body(16);
        uint8_t *bp = body.data();

        // CHANNEL-NUMBER
        w16(bp, TURN_ATTR_CHANNEL_NUMBER);
        w16(bp + 2, 4);
        w16(bp + 4, ch);
        w16(bp + 6, 0); // RFFU

        // XOR-PEER-ADDRESS
        w16(bp + 8, TURN_ATTR_XOR_PEER_ADDR);
        w16(bp + 10, 8);
        encode_xor_address(bp + 12, peer);
        // ملاحظة: encode_xor_address يكتب 8 بايتات ابتداءً من bp+12
        // لكننا خصصنا 16 بايت فقط — نحتاج 20 بايت إجمالياً
        // إصلاح: توسيع المخزن

        // إعادة تخصيص بالحجم الصحيح
        body.resize(20);
        bp = body.data();
        w16(bp, TURN_ATTR_CHANNEL_NUMBER);
        w16(bp + 2, 4);
        w16(bp + 4, ch);
        w16(bp + 6, 0);
        w16(bp + 8, TURN_ATTR_XOR_PEER_ADDR);
        w16(bp + 10, 8);
        encode_xor_address(bp + 12, peer);

        auto pkt = impl_->build_request(TURN_CHANNEL_BIND_REQ, body);
        auto resp = impl_->send_and_recv(pkt);

        if (resp.empty())
            return Result<uint16_t>::fail(NetError::TIMEOUT);

        uint16_t resp_type = r16(resp.data());
        if (resp_type != TURN_CHANNEL_BIND_RESP)
            return Result<uint16_t>::fail(NetError::PROTOCOL_ERROR);

        impl_->channels[ch] = peer;
        return Result<uint16_t>::success(ch);
    }

    Result<void> TurnClient::send_via_channel(uint16_t channel, const Buffer &data)
    {
        // بيانات القناة: [رقم:2][طول:2][بيانات:n]
        Buffer pkt(4 + data.size());
        w16(pkt.data(), channel);
        w16(pkt.data() + 2, (uint16_t)data.size());
        std::memcpy(pkt.data() + 4, data.data(), data.size());

        auto r = impl_->socket.send_to(impl_->server, pkt);
        if (r.ok())
            return Result<void>::success();
        return Result<void>::fail(r.error);
    }

    Result<void> TurnClient::send_indication(const Endpoint &peer, const Buffer &data)
    {
        // سمة XOR-PEER-ADDRESS + سمة DATA
        Buffer body(12 + 4 + data.size());
        uint8_t *bp = body.data();

        w16(bp, TURN_ATTR_XOR_PEER_ADDR);
        w16(bp + 2, 8);
        encode_xor_address(bp + 4, peer);

        w16(bp + 12, TURN_ATTR_DATA);
        w16(bp + 14, (uint16_t)data.size());
        std::memcpy(bp + 16, data.data(), data.size());

        auto pkt = impl_->build_request(TURN_SEND_INDICATION, body);
        auto r = impl_->socket.send_to(impl_->server, pkt);
        if (r.ok())
            return Result<void>::success();
        return Result<void>::fail(r.error);
    }

    bool TurnClient::is_allocated() const
    {
        return impl_->allocation.valid &&
               now_ms() < impl_->allocation.expires_at;
    }

} // namespace sad::net::nat
