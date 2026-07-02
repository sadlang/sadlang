// ===================================================================
// صNet — تنفيذ جدول التوجيه وأدلّة DHT (K-Bucket)
// (AR) جدول توجيه Kademlia بـ 256 دلو، LRU eviction
// (EN) Kademlia routing table: 256 K-Buckets with LRU eviction
// المكون: features/network/sadnet (جدول التوجيه DHT)
// المسار: features/network/sadnet/src/dht/routing_table.cpp
// ===================================================================

#include "sadnet/dht/dht.h"
#include "sadnet/crypto/crypto.h"
#include <algorithm>
#include <cstring>

namespace sad::net::dht
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تنفيذ KBucket
    // ═════════════════════════════════════════════════════════════════════

    bool KBucket::add_or_update(const NodeInfo &node)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        // البحث عن عُقدة موجودة بنفس المعرّف
        for (auto &n : nodes_)
        {
            if (n.id == node.id)
            {
                // تحديث المعلومات ونقلها للنهاية (الأحدث)
                n.last_seen = node.last_seen > 0 ? node.last_seen : now_ms();
                n.endpoint = node.endpoint;
                n.fail_count = 0;
                n.is_good = true;

                // نقل للنهاية (LRU: الأحدث في الخلف)
                auto updated = n;
                nodes_.erase(std::find_if(nodes_.begin(), nodes_.end(),
                                          [&](const NodeInfo &x)
                                          { return x.id == node.id; }));
                nodes_.push_back(updated);
                return true;
            }
        }

        // إذا لم تكن موجودة والدلو غير ممتلئ
        if (nodes_.size() < K_BUCKET_SIZE)
        {
            NodeInfo new_node = node;
            if (new_node.last_seen == 0)
                new_node.last_seen = now_ms();
            new_node.is_good = true;
            new_node.fail_count = 0;
            nodes_.push_back(new_node);
            return true;
        }

        // الدلو ممتلئ — تحقق من أقدم عُقدة (المقدمة)
        // في Kademlia الحقيقي: نرسل PING للأقدم، إذا لم تستجب نستبدلها
        // هنا: نستبدل إذا كانت الأقدم "سيئة"
        if (!nodes_.front().is_good || nodes_.front().fail_count > 2)
        {
            nodes_.erase(nodes_.begin());
            NodeInfo new_node = node;
            if (new_node.last_seen == 0)
                new_node.last_seen = now_ms();
            new_node.is_good = true;
            nodes_.push_back(new_node);
            return true;
        }

        return false; // الدلو ممتلئ بعُقد جيدة
    }

    bool KBucket::remove(const NodeId &id)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find_if(nodes_.begin(), nodes_.end(),
                               [&](const NodeInfo &n)
                               { return n.id == id; });
        if (it != nodes_.end())
        {
            nodes_.erase(it);
            return true;
        }
        return false;
    }

    const NodeInfo *KBucket::find(const NodeId &id) const
    {
        // ملاحظة: لا نأخذ القفل هنا لأن النتيجة مؤقتة
        // المُستدعي يجب أن يتعامل مع thread-safety بشكل أوسع
        for (const auto &n : nodes_)
        {
            if (n.id == id)
                return &n;
        }
        return nullptr;
    }

    std::vector<NodeInfo> KBucket::get_all() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return nodes_;
    }

    void KBucket::evict_stale(uint64_t max_age_ms)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        uint64_t now = now_ms();
        nodes_.erase(
            std::remove_if(nodes_.begin(), nodes_.end(),
                           [&](const NodeInfo &n)
                           {
                               return (now - n.last_seen) > max_age_ms || n.fail_count > 5;
                           }),
            nodes_.end());
    }

    size_t KBucket::size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return nodes_.size();
    }

    bool KBucket::is_full() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return nodes_.size() >= K_BUCKET_SIZE;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تنفيذ RoutingTable
    // ═════════════════════════════════════════════════════════════════════

    RoutingTable::RoutingTable(const NodeId &self) : self_id_(self) {}

    size_t RoutingTable::bucket_index(const NodeId &target) const
    {
        NodeId dist = self_id_.xor_distance(target);
        // فهرس الدلو = موقع أول بت غير صفري
        int lzb = dist.leading_zero_bits();
        if (lzb >= (int)NUM_BUCKETS)
            return NUM_BUCKETS - 1;
        return (size_t)lzb;
    }

    bool RoutingTable::add_or_update(const NodeInfo &node)
    {
        // لا نضيف أنفسنا
        if (node.id == self_id_)
            return false;
        size_t idx = bucket_index(node.id);
        return buckets_[idx].add_or_update(node);
    }

    std::vector<NodeInfo> RoutingTable::find_closest(const NodeId &target,
                                                     size_t count) const
    {
        // جمع كل العُقد من جميع الأدلّة
        std::vector<NodeInfo> all;
        for (const auto &bucket : buckets_)
        {
            auto nodes = bucket.get_all();
            all.insert(all.end(), nodes.begin(), nodes.end());
        }

        // ترتيب حسب مسافة XOR من الهدف
        std::sort(all.begin(), all.end(),
                  [&target](const NodeInfo &a, const NodeInfo &b)
                  {
                      return a.id.xor_distance(target) < b.id.xor_distance(target);
                  });

        // قص إلى العدد المطلوب
        if (all.size() > count)
            all.resize(count);

        return all;
    }

    bool RoutingTable::remove(const NodeId &id)
    {
        size_t idx = bucket_index(id);
        return buckets_[idx].remove(id);
    }

    size_t RoutingTable::total_nodes() const
    {
        size_t total = 0;
        for (const auto &b : buckets_)
            total += b.size();
        return total;
    }

    void RoutingTable::cleanup(uint64_t max_age_ms)
    {
        for (auto &b : buckets_)
            b.evict_stale(max_age_ms);
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) تسلسل/فك تسلسل رسائل RPC
    //      تنسيق ثنائي بسيط:
    //      [type:1][tx_id:4][sender_id:32][payload...]
    // ═════════════════════════════════════════════════════════════════════

    Buffer RPCMessage::serialize() const
    {
        Buffer buf;

        // الهيدر: type(1) + tx_id(4) + sender_id(32) = 37 بايت
        buf.resize(37);
        buf[0] = static_cast<uint8_t>(type);
        std::memcpy(buf.data() + 1, &transaction_id, 4);
        std::memcpy(buf.data() + 5, sender_id.data.data(), NODE_ID_SIZE);

        switch (type)
        {
        case RPCType::PING:
        case RPCType::PONG:
            // بدون حمولة إضافية
            break;

        case RPCType::FIND_NODE:
        case RPCType::FIND_VALUE:
        {
            // target_key (32 بايت)
            size_t old_size = buf.size();
            buf.resize(old_size + NODE_ID_SIZE);
            std::memcpy(buf.data() + old_size, target_key.data.data(), NODE_ID_SIZE);
            break;
        }

        case RPCType::STORE:
        {
            // target_key(32) + value_len(4) + value
            size_t old_size = buf.size();
            uint32_t vlen = static_cast<uint32_t>(value.size());
            buf.resize(old_size + NODE_ID_SIZE + 4 + value.size());
            std::memcpy(buf.data() + old_size, target_key.data.data(), NODE_ID_SIZE);
            std::memcpy(buf.data() + old_size + NODE_ID_SIZE, &vlen, 4);
            std::memcpy(buf.data() + old_size + NODE_ID_SIZE + 4,
                        value.data(), value.size());
            break;
        }

        case RPCType::FIND_NODE_RESPONSE:
        {
            // nodes_count(2) + [id(32) + addr_len(1) + addr + port(2)] per node
            size_t old_size = buf.size();
            uint16_t count = static_cast<uint16_t>(nodes.size());
            buf.resize(old_size + 2); // عدد العُقد
            std::memcpy(buf.data() + old_size, &count, 2);

            for (const auto &n : nodes)
            {
                size_t pos = buf.size();
                uint8_t addr_len = static_cast<uint8_t>(n.endpoint.address.size());
                buf.resize(pos + NODE_ID_SIZE + 1 + addr_len + 2);
                std::memcpy(buf.data() + pos, n.id.data.data(), NODE_ID_SIZE);
                buf[pos + NODE_ID_SIZE] = addr_len;
                std::memcpy(buf.data() + pos + NODE_ID_SIZE + 1,
                            n.endpoint.address.data(), addr_len);
                std::memcpy(buf.data() + pos + NODE_ID_SIZE + 1 + addr_len,
                            &n.endpoint.port, 2);
            }
            break;
        }

        case RPCType::FIND_VALUE_RESPONSE:
        {
            // found(1) + if found: value_len(4) + value
            //          + if not: nodes (same as FIND_NODE_RESPONSE format)
            size_t old_size = buf.size();
            if (!value.empty())
            {
                uint32_t vlen = static_cast<uint32_t>(value.size());
                buf.resize(old_size + 1 + 4 + value.size());
                buf[old_size] = 1; // found
                std::memcpy(buf.data() + old_size + 1, &vlen, 4);
                std::memcpy(buf.data() + old_size + 5,
                            value.data(), value.size());
            }
            else
            {
                buf.resize(old_size + 1);
                buf[old_size] = 0; // not found — nodes follow
                // سيتم إضافة العُقد بنفس تنسيق FIND_NODE_RESPONSE
            }
            break;
        }

        case RPCType::STORE_RESPONSE:
            // success flag (1 بايت)
            buf.resize(buf.size() + 1);
            buf[buf.size() - 1] = value.empty() ? 0 : 1;
            break;
        }

        return buf;
    }

    RPCMessage RPCMessage::deserialize(const uint8_t *data, size_t len)
    {
        RPCMessage msg;
        if (len < 37)
            return msg; // هيدر غير مكتمل

        msg.type = static_cast<RPCType>(data[0]);
        std::memcpy(&msg.transaction_id, data + 1, 4);
        std::memcpy(msg.sender_id.data.data(), data + 5, NODE_ID_SIZE);

        const uint8_t *payload = data + 37;
        size_t payload_len = len - 37;

        switch (msg.type)
        {
        case RPCType::PING:
        case RPCType::PONG:
            break;

        case RPCType::FIND_NODE:
        case RPCType::FIND_VALUE:
            if (payload_len >= NODE_ID_SIZE)
            {
                std::memcpy(msg.target_key.data.data(), payload, NODE_ID_SIZE);
            }
            break;

        case RPCType::STORE:
            if (payload_len >= NODE_ID_SIZE + 4)
            {
                std::memcpy(msg.target_key.data.data(), payload, NODE_ID_SIZE);
                uint32_t vlen;
                std::memcpy(&vlen, payload + NODE_ID_SIZE, 4);
                if (payload_len >= NODE_ID_SIZE + 4 + vlen)
                {
                    msg.value = Buffer(payload + NODE_ID_SIZE + 4, vlen);
                }
            }
            break;

        case RPCType::FIND_NODE_RESPONSE:
            if (payload_len >= 2)
            {
                uint16_t count;
                std::memcpy(&count, payload, 2);
                size_t pos = 2;
                for (uint16_t i = 0; i < count && pos < payload_len; i++)
                {
                    if (pos + NODE_ID_SIZE + 1 > payload_len)
                        break;
                    NodeInfo n;
                    std::memcpy(n.id.data.data(), payload + pos, NODE_ID_SIZE);
                    pos += NODE_ID_SIZE;
                    uint8_t addr_len = payload[pos++];
                    if (pos + addr_len + 2 > payload_len)
                        break;
                    n.endpoint.address = std::string(
                        (const char *)(payload + pos), addr_len);
                    pos += addr_len;
                    std::memcpy(&n.endpoint.port, payload + pos, 2);
                    pos += 2;
                    msg.nodes.push_back(n);
                }
            }
            break;

        case RPCType::FIND_VALUE_RESPONSE:
            if (payload_len >= 1)
            {
                if (payload[0] == 1 && payload_len >= 5)
                {
                    uint32_t vlen;
                    std::memcpy(&vlen, payload + 1, 4);
                    if (payload_len >= 5 + vlen)
                    {
                        msg.value = Buffer(payload + 5, vlen);
                    }
                }
                // إذا payload[0] == 0، العُقد تُوزّع بنفس تنسيق FIND_NODE_RESPONSE
            }
            break;

        case RPCType::STORE_RESPONSE:
            if (payload_len >= 1)
            {
                // 1 = نجاح، 0 = فشل
                if (payload[0] == 1)
                    msg.value = Buffer(1);
            }
            break;
        }

        return msg;
    }

} // namespace sad::net::dht
