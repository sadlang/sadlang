// ===================================================================
// صNet — محرك DHT: البحث التكراري والتخزين وحلقة الاستقبال
// (AR) تنفيذ العمليات الأربعة: PING, STORE, FIND_NODE, FIND_VALUE
//      بحث تكراري (iterative lookup) بعامل α=3
//      تخزين مكرر على أقرب k عُقدة
// (EN) DHT engine: iterative lookup, replicated storage, message loop
// المكون: network/sadnet (محرك DHT)
// المسار: network/sadnet/src/dht/dht_engine.cpp
// ===================================================================

#include "sadnet/dht/dht.h"
#include "sadnet/crypto/crypto.h"
#include <algorithm>
#include <atomic>
#include <cstring>
#include <map>
#include <mutex>
#include <set>
#include <thread>

namespace sad::net::dht
{

    // ═════════════════════════════════════════════════════════════════════
    // (AR) التنفيذ الداخلي لمحرك DHT
    // ═════════════════════════════════════════════════════════════════════

    struct DHTEngine::Impl
    {
        NodeId self_id;
        RoutingTable routing;
        UdpSocket socket;

        // تخزين محلي: key → value
        std::map<NodeId, DHTValue> storage;
        mutable std::mutex storage_mutex;

        // عدّاد المعاملات
        std::atomic<uint32_t> next_tx_id{1};

        // حالة التشغيل
        std::atomic<bool> running{false};
        std::thread recv_thread;

        // معالجات مخصصة
        DHTEngine::RequestHandler custom_handler;

        // طلبات بانتظار الاستجابة: tx_id → callback
        struct PendingRequest
        {
            std::function<void(const RPCMessage &)> callback;
            uint64_t sent_at;
        };
        std::map<uint32_t, PendingRequest> pending;
        mutable std::mutex pending_mutex;

        Impl(const NodeId &id, uint16_t port)
            : self_id(id), routing(id)
        {
            socket.bind(port);
            socket.set_recv_timeout(1000); // 1 ثانية مهلة
        }

        // ─── إرسال رسالة RPC ───

        Result<void> send_message(const Endpoint &dest, const RPCMessage &msg)
        {
            auto buf = msg.serialize();
            auto r = socket.send_to(dest, buf);
            if (r.ok())
                return Result<void>::success();
            return Result<void>::fail(r.error);
        }

        // ─── إرسال وانتظار الاستجابة ───

        RPCMessage send_and_wait(const Endpoint &dest, RPCMessage &msg)
        {
            msg.transaction_id = next_tx_id++;

            RPCMessage response;
            bool got_response = false;
            std::mutex resp_mutex;
            std::condition_variable resp_cv;

            {
                std::lock_guard<std::mutex> lock(pending_mutex);
                pending[msg.transaction_id] = {
                    [&](const RPCMessage &r)
                    {
                        std::lock_guard<std::mutex> lk(resp_mutex);
                        response = r;
                        got_response = true;
                        resp_cv.notify_one();
                    },
                    now_ms()};
            }

            send_message(dest, msg);

            // انتظار الاستجابة
            std::unique_lock<std::mutex> lock(resp_mutex);
            resp_cv.wait_for(lock, std::chrono::milliseconds(RPC_TIMEOUT_MS),
                             [&]
                             { return got_response; });

            {
                std::lock_guard<std::mutex> plk(pending_mutex);
                pending.erase(msg.transaction_id);
            }

            return response;
        }

        // ─── معالجة الرسائل الواردة ───

        void handle_incoming(const RPCMessage &msg, const Endpoint &sender)
        {
            // تحديث جدول التوجيه بالمُرسل
            NodeInfo sender_info;
            sender_info.id = msg.sender_id;
            sender_info.endpoint = sender;
            sender_info.last_seen = now_ms();
            routing.add_or_update(sender_info);

            // تحقق إذا كانت استجابة لطلب معلّق
            {
                std::lock_guard<std::mutex> lock(pending_mutex);
                auto it = pending.find(msg.transaction_id);
                if (it != pending.end())
                {
                    it->second.callback(msg);
                    return;
                }
            }

            // معالجة كطلب وارد
            switch (msg.type)
            {
            case RPCType::PING:
                handle_ping(msg, sender);
                break;
            case RPCType::FIND_NODE:
                handle_find_node(msg, sender);
                break;
            case RPCType::FIND_VALUE:
                handle_find_value(msg, sender);
                break;
            case RPCType::STORE:
                handle_store(msg, sender);
                break;
            default:
                break;
            }

            // معالج مخصص إذا وُجد
            if (custom_handler)
                custom_handler(msg, sender);
        }

        void handle_ping(const RPCMessage &msg, const Endpoint &sender)
        {
            RPCMessage pong;
            pong.type = RPCType::PONG;
            pong.sender_id = self_id;
            pong.transaction_id = msg.transaction_id;
            send_message(sender, pong);
        }

        void handle_find_node(const RPCMessage &msg, const Endpoint &sender)
        {
            RPCMessage resp;
            resp.type = RPCType::FIND_NODE_RESPONSE;
            resp.sender_id = self_id;
            resp.transaction_id = msg.transaction_id;
            resp.nodes = routing.find_closest(msg.target_key);
            send_message(sender, resp);
        }

        void handle_find_value(const RPCMessage &msg, const Endpoint &sender)
        {
            RPCMessage resp;
            resp.sender_id = self_id;
            resp.transaction_id = msg.transaction_id;

            // بحث في التخزين المحلي
            {
                std::lock_guard<std::mutex> lock(storage_mutex);
                auto it = storage.find(msg.target_key);
                if (it != storage.end() && !it->second.is_expired())
                {
                    resp.type = RPCType::FIND_VALUE_RESPONSE;
                    resp.value = it->second.data;
                    send_message(sender, resp);
                    return;
                }
            }

            // القيمة غير موجودة — نُرجع أقرب عُقد
            resp.type = RPCType::FIND_VALUE_RESPONSE;
            resp.nodes = routing.find_closest(msg.target_key);
            send_message(sender, resp);
        }

        void handle_store(const RPCMessage &msg, const Endpoint &sender)
        {
            {
                std::lock_guard<std::mutex> lock(storage_mutex);
                DHTValue val;
                val.data = msg.value;
                val.publisher = msg.sender_id;
                val.stored_at = now_ms();
                storage[msg.target_key] = std::move(val);
            }

            RPCMessage resp;
            resp.type = RPCType::STORE_RESPONSE;
            resp.sender_id = self_id;
            resp.transaction_id = msg.transaction_id;
            resp.value = Buffer(1); // نجاح
            send_message(sender, resp);
        }

        // ─── حلقة الاستقبال ───

        void recv_loop()
        {
            while (running.load())
            {
                auto result = socket.recv_from();
                if (!result.ok())
                    continue; // timeout أو خطأ

                auto &rr = result.value;
                if (rr.data.size() < 37)
                    continue; // حزمة قصيرة جداً

                auto msg = RPCMessage::deserialize(rr.data.data(), rr.data.size());
                handle_incoming(msg, rr.sender);
            }
        }

        // ─── البحث التكراري (Iterative Lookup) ───

        std::vector<NodeInfo> iterative_find_node(const NodeId &target)
        {
            // بدء من أقرب عُقد معروفة
            auto closest = routing.find_closest(target, ALPHA);
            if (closest.empty())
                return {};

            std::set<NodeId> queried;   // عُقد تم سؤالها
            std::set<NodeId> seen;      // عُقد تمت رؤيتها
            std::vector<NodeInfo> best; // أفضل النتائج

            for (const auto &n : closest)
            {
                seen.insert(n.id);
                best.push_back(n);
            }

            bool improved = true;
            while (improved)
            {
                improved = false;

                // اختيار أقرب α عُقدة لم تُسأل بعد
                std::vector<NodeInfo> to_query;
                for (const auto &n : best)
                {
                    if (queried.count(n.id) == 0)
                    {
                        to_query.push_back(n);
                        if (to_query.size() >= ALPHA)
                            break;
                    }
                }

                if (to_query.empty())
                    break;

                for (auto &n : to_query)
                {
                    queried.insert(n.id);

                    RPCMessage req;
                    req.type = RPCType::FIND_NODE;
                    req.sender_id = self_id;
                    req.target_key = target;

                    auto resp = send_and_wait(n.endpoint, req);

                    if (resp.type == RPCType::FIND_NODE_RESPONSE)
                    {
                        for (const auto &found : resp.nodes)
                        {
                            if (seen.count(found.id) == 0 && found.id != self_id)
                            {
                                seen.insert(found.id);
                                best.push_back(found);
                                improved = true;
                            }
                        }
                    }
                }

                // ترتيب حسب المسافة
                std::sort(best.begin(), best.end(),
                          [&target](const NodeInfo &a, const NodeInfo &b)
                          {
                              return a.id.xor_distance(target) <
                                     b.id.xor_distance(target);
                          });

                // تقليم إلى K
                if (best.size() > K_BUCKET_SIZE)
                    best.resize(K_BUCKET_SIZE);
            }

            return best;
        }
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) واجهات DHTEngine العامة
    // ═════════════════════════════════════════════════════════════════════

    DHTEngine::DHTEngine(const NodeId &self_id, uint16_t port)
        : impl_(std::make_unique<Impl>(self_id, port)) {}

    DHTEngine::~DHTEngine()
    {
        stop();
    }

    Result<void> DHTEngine::ping(const Endpoint &target)
    {
        RPCMessage req;
        req.type = RPCType::PING;
        req.sender_id = impl_->self_id;

        auto resp = impl_->send_and_wait(target, req);
        if (resp.type == RPCType::PONG)
            return Result<void>::success();
        return Result<void>::fail(NetError::TIMEOUT);
    }

    Result<void> DHTEngine::store(const NodeId &key, const Buffer &value)
    {
        // إيجاد أقرب k عُقدة للمفتاح
        auto closest = impl_->iterative_find_node(key);

        if (closest.empty())
        {
            // تخزين محلي فقط
            std::lock_guard<std::mutex> lock(impl_->storage_mutex);
            DHTValue val;
            val.data = value;
            val.publisher = impl_->self_id;
            val.stored_at = now_ms();
            impl_->storage[key] = std::move(val);
            return Result<void>::success();
        }

        // إرسال STORE لأقرب k عُقدة
        uint32_t success_count = 0;
        for (const auto &n : closest)
        {
            RPCMessage req;
            req.type = RPCType::STORE;
            req.sender_id = impl_->self_id;
            req.target_key = key;
            req.value = value;

            auto resp = impl_->send_and_wait(n.endpoint, req);
            if (resp.type == RPCType::STORE_RESPONSE && !resp.value.empty())
                success_count++;
        }

        // تخزين محلي أيضاً
        {
            std::lock_guard<std::mutex> lock(impl_->storage_mutex);
            DHTValue val;
            val.data = value;
            val.publisher = impl_->self_id;
            val.stored_at = now_ms();
            impl_->storage[key] = std::move(val);
        }

        if (success_count > 0)
            return Result<void>::success();
        return Result<void>::fail(NetError::TIMEOUT);
    }

    Result<std::vector<NodeInfo>> DHTEngine::find_node(const NodeId &target)
    {
        auto nodes = impl_->iterative_find_node(target);
        if (!nodes.empty())
            return Result<std::vector<NodeInfo>>::success(std::move(nodes));
        return Result<std::vector<NodeInfo>>::fail(NetError::NOT_FOUND);
    }

    Result<Buffer> DHTEngine::find_value(const NodeId &key)
    {
        // بحث محلي أولاً
        {
            std::lock_guard<std::mutex> lock(impl_->storage_mutex);
            auto it = impl_->storage.find(key);
            if (it != impl_->storage.end() && !it->second.is_expired())
                return Result<Buffer>::success(it->second.data);
        }

        // بحث تكراري في الشبكة
        auto closest = impl_->routing.find_closest(key, ALPHA);

        std::set<NodeId> queried;
        for (const auto &n : closest)
        {
            queried.insert(n.id);

            RPCMessage req;
            req.type = RPCType::FIND_VALUE;
            req.sender_id = impl_->self_id;
            req.target_key = key;

            auto resp = impl_->send_and_wait(n.endpoint, req);
            if (resp.type == RPCType::FIND_VALUE_RESPONSE && !resp.value.empty())
            {
                // وجدنا القيمة — تخزينها محلياً أيضاً (caching)
                {
                    std::lock_guard<std::mutex> lock(impl_->storage_mutex);
                    DHTValue val;
                    val.data = resp.value;
                    val.publisher = resp.sender_id;
                    val.stored_at = now_ms();
                    impl_->storage[key] = std::move(val);
                }
                return Result<Buffer>::success(resp.value);
            }

            // لم نجد — أضف العُقد الجديدة وتابع
            for (const auto &found : resp.nodes)
            {
                if (queried.count(found.id) == 0)
                {
                    impl_->routing.add_or_update(found);
                }
            }
        }

        return Result<Buffer>::fail(NetError::NOT_FOUND);
    }

    Result<void> DHTEngine::bootstrap(const Endpoint &seed)
    {
        // إرسال FIND_NODE لأنفسنا عبر العُقدة البذرية
        RPCMessage req;
        req.type = RPCType::FIND_NODE;
        req.sender_id = impl_->self_id;
        req.target_key = impl_->self_id;

        auto resp = impl_->send_and_wait(seed, req);
        if (resp.type != RPCType::FIND_NODE_RESPONSE)
            return Result<void>::fail(NetError::TIMEOUT);

        // إضافة العُقد المُكتشفة
        for (const auto &n : resp.nodes)
            impl_->routing.add_or_update(n);

        // إيجاد المزيد من العُقد القريبة
        impl_->iterative_find_node(impl_->self_id);

        return Result<void>::success();
    }

    void DHTEngine::start()
    {
        if (impl_->running.load())
            return;
        impl_->running = true;
        impl_->recv_thread = std::thread([this]()
                                         { impl_->recv_loop(); });
    }

    void DHTEngine::stop()
    {
        impl_->running = false;
        if (impl_->recv_thread.joinable())
            impl_->recv_thread.join();
    }

    bool DHTEngine::is_running() const
    {
        return impl_->running.load();
    }

    const RoutingTable &DHTEngine::routing_table() const
    {
        return impl_->routing;
    }

    size_t DHTEngine::stored_values_count() const
    {
        std::lock_guard<std::mutex> lock(impl_->storage_mutex);
        return impl_->storage.size();
    }

    void DHTEngine::set_custom_handler(RequestHandler handler)
    {
        impl_->custom_handler = std::move(handler);
    }

} // namespace sad::net::dht
