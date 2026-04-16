// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// ״µNet ג€” ״×†״° ״§„״×ˆ״¬‡ ״§„״¨״µ„
// (AR) ״¨†״§״¡ ״­„‚״§״× ״¨״µ„״©״ ״×״÷„/ƒ ״·״¨‚״§״× ״§„״×״´״±
//      ״¹‚״¯״© ״×״±״­„״ …״¯״± ״×ˆ״¬‡ …״¬‡ˆ„ ״§„‡ˆ״©
// (EN) Onion routing implementation: circuits, relay, anonymous router
// المكون: network/sadnet (التوجيه البصلي)
// المسار: network/sadnet/src/routing/onion_routing.cpp
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

#include "sadnet/routing/routing.h"
#include <algorithm>
#include <cstring>
#include <map>
#include <mutex>

namespace sad::net::routing
{

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) …״³״§״¹״¯״§״× ״×״±…״²
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    static void w32(uint8_t *p, uint32_t v)
    {
        p[0] = (uint8_t)(v >> 24);
        p[1] = (uint8_t)(v >> 16);
        p[2] = (uint8_t)(v >> 8);
        p[3] = (uint8_t)v;
    }
    static uint32_t r32(const uint8_t *p)
    {
        return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
               ((uint32_t)p[2] << 8) | p[3];
    }

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) OnionCell ג€” ״×״±…״²/ƒ ״×״±…״²
    //      [circuit_id:4][type:1][payload_len:4][payload:N]
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    Buffer OnionCell::serialize() const
    {
        Buffer buf(4 + 1 + 4 + payload.size());
        uint8_t *p = buf.data();
        w32(p, circuit_id);
        p[4] = (uint8_t)type;
        w32(p + 5, (uint32_t)payload.size());
        if (!payload.empty())
            std::memcpy(p + 9, payload.data(), payload.size());
        return buf;
    }

    OnionCell OnionCell::deserialize(const uint8_t *data, size_t len)
    {
        OnionCell cell;
        if (len < 9)
            return cell;
        cell.circuit_id = r32(data);
        cell.type = (CellType)data[4];
        uint32_t pl = r32(data + 5);
        if (9 + pl <= len)
            cell.payload = Buffer(data + 9, pl);
        return cell;
    }

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) Circuit ג€” ״­„‚״© ״¨״µ„״©
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    struct Circuit::Impl
    {
        uint32_t circuit_id;
        std::vector<CircuitHop> hops;
        bool ready = false;

        Impl()
        {
            // …״¹״± ״¹״´ˆ״§״¦
            uint8_t rnd[4];
            crypto::random_bytes(rnd, 4);
            circuit_id = r32(rnd);
        }
    };

    Circuit::Circuit() : impl_(std::make_unique<Impl>()) {}
    Circuit::~Circuit() = default;
    Circuit::Circuit(Circuit &&other) noexcept = default;
    Circuit &Circuit::operator=(Circuit &&other) noexcept = default;

    void Circuit::add_hop(const CircuitHop &hop)
    {
        impl_->hops.push_back(hop);
    }

    Result<Buffer> Circuit::wrap(const Buffer &plaintext) const
    {
        if (impl_->hops.empty())
            return Result<Buffer>::fail(NetError::PROTOCOL_ERROR);

        Buffer data = plaintext;

        // ״×״÷„ …† ״§„‚״²״© ״§„״£״®״±״© ״¥„‰ ״§„״£ˆ„‰
        for (int i = (int)impl_->hops.size() - 1; i >= 0; i--)
        {
            auto &hop = impl_->hops[i];
            // ״×״´״± ״¨…״×״§״­ ״§„‚״²״©
            Buffer aad(4);
            w32(aad.data(), impl_->circuit_id);

            auto encrypted = crypto::aes_gcm_encrypt(hop.forward_key.data(),
                                                     data.data(), data.size(),
                                                     aad.data(), aad.size());

            // [nonce:12][ciphertext:N][tag:16]
            Buffer layer(encrypted.nonce.size() + encrypted.ciphertext.size() +
                         encrypted.tag.size());
            uint8_t *p = layer.data();
            std::memcpy(p, encrypted.nonce.data(), encrypted.nonce.size());
            p += encrypted.nonce.size();
            std::memcpy(p, encrypted.ciphertext.data(), encrypted.ciphertext.size());
            p += encrypted.ciphertext.size();
            std::memcpy(p, encrypted.tag.data(), encrypted.tag.size());

            data = std::move(layer);
        }

        return Result<Buffer>::success(std::move(data));
    }

    Result<Buffer> Circuit::peel_layer(const Buffer &data, uint32_t hop_index) const
    {
        if (hop_index >= impl_->hops.size())
            return Result<Buffer>::fail(NetError::NOT_FOUND);

        auto &hop = impl_->hops[hop_index];
        if (data.size() < AES_NONCE_SIZE + AES_TAG_SIZE)
            return Result<Buffer>::fail(NetError::PROTOCOL_ERROR);

        const uint8_t *base = data.data();
        size_t ct_len = data.size() - AES_NONCE_SIZE - AES_TAG_SIZE;
        const uint8_t *nonce_ptr = base;
        const uint8_t *ct_ptr = base + AES_NONCE_SIZE;
        const uint8_t *tag_ptr = base + AES_NONCE_SIZE + ct_len;

        Buffer aad(4);
        w32(aad.data(), impl_->circuit_id);

        auto plaintext = crypto::aes_gcm_decrypt(hop.forward_key.data(),
                                                 ct_ptr, ct_len,
                                                 nonce_ptr, tag_ptr,
                                                 aad.data(), aad.size());
        if (plaintext.empty())
            return Result<Buffer>::fail(NetError::PROTOCOL_ERROR);

        return Result<Buffer>::success(std::move(plaintext));
    }

    uint32_t Circuit::id() const { return impl_->circuit_id; }
    size_t Circuit::hop_count() const { return impl_->hops.size(); }
    bool Circuit::is_ready() const { return impl_->ready; }
    const CircuitHop &Circuit::hop(size_t index) const { return impl_->hops[index]; }

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) ״¨״§† ״§„״­„‚״§״× ג€” ״×״¨״§״¯„ …״§״×״­ X25519 ״×״¯״±״¬
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    struct CircuitBuilder::Impl
    {
        KeyPair identity;
        CellSender sender;

        Impl(const KeyPair &id, CellSender s) : identity(id), sender(std::move(s)) {}

        // ״¥†״´״§״¡ ‚״²״© ˆ״§״­״¯״© ״¹״¨״± ״×״¨״§״¯„ …״§״×״­
        Result<CircuitHop> create_hop(const NodeId &node_id,
                                      const Endpoint &ep,
                                      uint32_t circuit_id)
        {
            // ״¥†״´״§״¡ ״²ˆ״¬ …״§״×״­ …״₪‚״×
            auto ephemeral = crypto::x25519_generate_keypair();

            // ״¨†״§״¡ ״®„״© CREATE: [circuit_id:4][public_key:32]
            OnionCell cell;
            cell.circuit_id = circuit_id;
            cell.type = CellType::CREATE;
            cell.payload = Buffer(ephemeral.public_key.data(), X25519_KEY_SIZE);

            auto cell_data = cell.serialize();
            auto resp_data = sender(ep, cell_data);
            if (!resp_data.ok())
                return Result<CircuitHop>::fail(resp_data.error);

            // ״×״­„„ CREATED: [public_key:32]
            auto resp_cell = OnionCell::deserialize(resp_data.value.data(),
                                                    resp_data.value.size());
            if (resp_cell.type != CellType::CREATED ||
                resp_cell.payload.size() < X25519_KEY_SIZE)
                return Result<CircuitHop>::fail(NetError::PROTOCOL_ERROR);

            // ״­״³״§״¨ ״§„״³״± ״§„…״´״×״±ƒ
            // (AR) استخراج المفتاح العام من الرد
            X25519Key peer_pub{};
            std::memcpy(peer_pub.data(), resp_cell.payload.data(),
                        std::min(resp_cell.payload.size(), (size_t)X25519_KEY_SIZE));
            auto shared = crypto::x25519_shared_secret(ephemeral.private_key, peer_pub);

            // ״§״´״×‚״§‚ …״§״×״­ ״§„״£…״§… ˆ״§„״®„
            Buffer info_fwd(3);
            info_fwd[0] = 'F';
            info_fwd[1] = 'W';
            info_fwd[2] = 'D';
            auto forward_key = crypto::hkdf_sha256(nullptr, 0,
                                                   shared.data(), shared.size(),
                                                   info_fwd.data(), info_fwd.size(),
                                                   AES_KEY_SIZE);

            Buffer info_bwd(3);
            info_bwd[0] = 'B';
            info_bwd[1] = 'W';
            info_bwd[2] = 'D';
            auto backward_key = crypto::hkdf_sha256(nullptr, 0,
                                                    shared.data(), shared.size(),
                                                    info_bwd.data(), info_bwd.size(),
                                                    AES_KEY_SIZE);

            CircuitHop hop;
            hop.node_id = node_id;
            hop.endpoint = ep;
            hop.shared_secret = Buffer(shared.data(), shared.size());
            hop.forward_key = forward_key;
            hop.backward_key = backward_key;

            return Result<CircuitHop>::success(std::move(hop));
        }
    };

    CircuitBuilder::CircuitBuilder(const KeyPair &identity, CellSender sender)
        : impl_(std::make_unique<Impl>(identity, std::move(sender))) {}

    CircuitBuilder::~CircuitBuilder() = default;

    Result<Circuit> CircuitBuilder::build(
        const std::vector<std::pair<NodeId, Endpoint>> &path)
    {
        Circuit circuit;

        for (size_t i = 0; i < path.size(); i++)
        {
            auto hop_result = impl_->create_hop(path[i].first,
                                                path[i].second,
                                                circuit.id());
            if (!hop_result.ok())
                return Result<Circuit>::fail(hop_result.error);

            circuit.add_hop(hop_result.value);
        }

        return Result<Circuit>::success(std::move(circuit));
    }

    Result<void> CircuitBuilder::extend(Circuit &circuit,
                                        const NodeId &node,
                                        const Endpoint &ep)
    {
        auto hop_result = impl_->create_hop(node, ep, circuit.id());
        if (!hop_result.ok())
            return Result<void>::fail(hop_result.error);

        circuit.add_hop(hop_result.value);
        return Result<void>::success();
    }

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) ״¹‚״¯״© ״§„״×״±״­„ ״§„״¨״µ„״©
    //      ״×״­״×״¸ ״¨״¬״¯ˆ„ ״­„‚״§״× †״´״·״©״ ״×ƒ ״·״¨‚״© ״×״´״± ˆ״×״¹״¯ ״§„״×ˆ״¬‡
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    struct RelayNode::Impl
    {
        KeyPair identity;

        struct CircuitEntry
        {
            uint32_t circuit_id;
            Buffer shared_secret;
            Buffer forward_key;
            Buffer backward_key;
            Endpoint next_hop;
            uint64_t created_at;
        };

        std::map<uint32_t, CircuitEntry> circuits;
        mutable std::mutex mutex;

        Impl(const KeyPair &id) : identity(id) {}
    };

    RelayNode::RelayNode(const KeyPair &identity)
        : impl_(std::make_unique<Impl>(identity)) {}

    RelayNode::~RelayNode() = default;

    Result<void> RelayNode::process_cell(const OnionCell &cell,
                                         const Endpoint &from,
                                         RelayForwarder forwarder)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);

        if (cell.type == CellType::CREATE)
        {
            // ״×״¨״§״¯„ …״§״×״­: ״§״³״×‚״¨״§„ ״§„…״×״§״­ ״§„״¹״§… ˆ״¥״±״³״§„ …״×״§״­†״§
            if (cell.payload.size() < X25519_KEY_SIZE)
                return Result<void>::fail(NetError::PROTOCOL_ERROR);

            // (AR) استخراج المفتاح العام من الخلية
            X25519Key peer_pub{};
            std::memcpy(peer_pub.data(), cell.payload.data(),
                        std::min(cell.payload.size(), (size_t)X25519_KEY_SIZE));
            auto ephemeral = crypto::x25519_generate_keypair();
            auto shared = crypto::x25519_shared_secret(ephemeral.private_key, peer_pub);

            Buffer info_fwd(3);
            info_fwd[0] = 'F';
            info_fwd[1] = 'W';
            info_fwd[2] = 'D';
            auto fwd_key = crypto::hkdf_sha256(nullptr, 0,
                                               shared.data(), shared.size(),
                                               info_fwd.data(), info_fwd.size(),
                                               AES_KEY_SIZE);

            Buffer info_bwd(3);
            info_bwd[0] = 'B';
            info_bwd[1] = 'W';
            info_bwd[2] = 'D';
            auto bwd_key = crypto::hkdf_sha256(nullptr, 0,
                                               shared.data(), shared.size(),
                                               info_bwd.data(), info_bwd.size(),
                                               AES_KEY_SIZE);

            Impl::CircuitEntry entry;
            entry.circuit_id = cell.circuit_id;
            entry.shared_secret = Buffer(shared.data(), shared.size());
            entry.forward_key = fwd_key;
            entry.backward_key = bwd_key;
            entry.created_at = now_ms();
            impl_->circuits[cell.circuit_id] = std::move(entry);

            // ״¥״±״³״§„ CREATED …״¹ …״×״§״­†״§ ״§„״¹״§…
            OnionCell resp;
            resp.circuit_id = cell.circuit_id;
            resp.type = CellType::CREATED;
            resp.payload = Buffer(ephemeral.public_key.data(), X25519_KEY_SIZE);
            forwarder(from, resp.serialize());

            return Result<void>::success();
        }

        if (cell.type == CellType::RELAY || cell.type == CellType::DATA)
        {
            auto it = impl_->circuits.find(cell.circuit_id);
            if (it == impl_->circuits.end())
                return Result<void>::fail(NetError::NOT_FOUND);

            // ƒ ״·״¨‚״© ״§„״×״´״±
            auto &entry = it->second;
            if (cell.payload.size() < AES_NONCE_SIZE + AES_TAG_SIZE)
                return Result<void>::fail(NetError::PROTOCOL_ERROR);

            const uint8_t *rbase = cell.payload.data();
            size_t rct_len = cell.payload.size() - AES_NONCE_SIZE - AES_TAG_SIZE;
            const uint8_t *rnonce = rbase;
            const uint8_t *rct = rbase + AES_NONCE_SIZE;
            const uint8_t *rtag = rbase + AES_NONCE_SIZE + rct_len;

            Buffer aad(4);
            w32(aad.data(), cell.circuit_id);

            auto plaintext = crypto::aes_gcm_decrypt(entry.forward_key.data(),
                                                     rct, rct_len,
                                                     rnonce, rtag,
                                                     aad.data(), aad.size());
            if (plaintext.empty())
                return Result<void>::fail(NetError::PROTOCOL_ERROR);

            // ״¥״¹״§״¯״© ״×ˆ״¬‡ ״¥„‰ ״§„‚״²״© ״§„״×״§„״©
            if (!entry.next_hop.address.empty())
            {
                forwarder(entry.next_hop, plaintext);
            }

            return Result<void>::success();
        }

        if (cell.type == CellType::DESTROY)
        {
            impl_->circuits.erase(cell.circuit_id);
            return Result<void>::success();
        }

        return Result<void>::fail(NetError::PROTOCOL_ERROR);
    }

    size_t RelayNode::active_circuits() const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        return impl_->circuits.size();
    }

    void RelayNode::cleanup_expired()
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        uint64_t now = now_ms();
        for (auto it = impl_->circuits.begin(); it != impl_->circuits.end();)
        {
            if (now - it->second.created_at > CIRCUIT_TIMEOUT)
                it = impl_->circuits.erase(it);
            else
                ++it;
        }
    }

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // (AR) …״¯״± ״§„״×ˆ״¬‡ ״§„…״¬‡ˆ„
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    struct OnionRouter::Impl
    {
        KeyPair identity;
        uint32_t num_hops;

        Impl(const KeyPair &id, uint32_t hops)
            : identity(id), num_hops(hops) {}
    };

    OnionRouter::OnionRouter(const KeyPair &identity, uint32_t num_hops)
        : impl_(std::make_unique<Impl>(identity, num_hops)) {}

    OnionRouter::~OnionRouter() = default;

    Result<Buffer> OnionRouter::send_anonymous(const Endpoint &destination,
                                               const Buffer &data,
                                               CellSender sender)
    {
        // ״­״×״§״¬ ‚״§״¦…״© …† ״§„״¹‚״¯ ״§„…״×״§״­״© „״¨†״§״¡ ״§„…״³״§״±
        //  ״§„״§״³״×״®״¯״§… ״§„״­‚‚״ ״³״×״¯״¹‰ select_path ״£ˆ„״§‹ ״«… create_circuit
        return Result<Buffer>::fail(NetError::NOT_FOUND);
    }

    Result<Circuit> OnionRouter::create_circuit(
        const std::vector<std::pair<NodeId, Endpoint>> &path,
        CellSender sender)
    {
        CircuitBuilder builder(impl_->identity, std::move(sender));
        return builder.build(path);
    }

    Result<Buffer> OnionRouter::send_via_circuit(Circuit &circuit,
                                                 const Buffer &data,
                                                 CellSender sender)
    {
        auto wrapped = circuit.wrap(data);
        if (!wrapped.ok())
            return Result<Buffer>::fail(wrapped.error);

        OnionCell cell;
        cell.circuit_id = circuit.id();
        cell.type = CellType::RELAY;
        cell.payload = wrapped.value;

        auto cell_data = cell.serialize();
        return sender(circuit.hop(0).endpoint, cell_data);
    }

    std::vector<std::pair<NodeId, Endpoint>> OnionRouter::select_path(
        const std::vector<std::pair<NodeId, Endpoint>> &available_nodes) const
    {

        if (available_nodes.size() < impl_->num_hops)
            return {};

        // ״§״®״×״§״± ״¹״´ˆ״§״¦ ״¨״¯ˆ† ״×ƒ״±״§״±
        auto candidates = available_nodes;
        std::vector<std::pair<NodeId, Endpoint>> path;

        for (uint32_t i = 0; i < impl_->num_hops && !candidates.empty(); i++)
        {
            uint8_t rnd[4];
            crypto::random_bytes(rnd, 4);
            uint32_t idx = r32(rnd) % candidates.size();

            path.push_back(candidates[idx]);
            candidates.erase(candidates.begin() + idx);
        }

        return path;
    }

} // namespace sad::net::routing
