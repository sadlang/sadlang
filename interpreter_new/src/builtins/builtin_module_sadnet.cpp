/**
 * @file builtin_module_sadnet.cpp
 * @brief (AR) وحدة الشبكة اللامركزية — ربط مكتبة صNet بمفسر لغة ص
 * @brief (EN) Decentralized network module — SadNet bindings for Sad interpreter
 *
 * @note يتطلب HAS_SADNET (يُعرَّف تلقائياً عند ربط مكتبة sadnet)
 *
 * @details
 * (AR) الأقسام:
 *   1. تهيئة العقدة: إنشاء عقدة + مفتاح + إثبات عمل
 *   2. DHT: تخزين واسترجاع القيم في الجدول الموزع
 *   3. الاكتشاف: البحث عن الأقران عبر LAN + PEX
 *   4. المراسلة: رسائل E2EE (طرف لطرف)
 *   5. التخزين: تخزين واسترجاع الملفات الموزعة
 *   6. DNS: تسجيل وحل أسماء .snet
 *   7. الأمان: سمعة، شهادات ثقة، قوائم سوداء
 *   8. البث: بث مباشر P2P
 *   9. CDN: تخزين مؤقت لامركزي
 *  10. التوجيه: توجيه بصلي مجهول
 *  11. الهوية: DID لامركزي
 *
 * استيراد الوحدة في لغة ص:
 *   استورد شبكة_لامركزية
 *   # أو
 *   من شبكة_لامركزية استورد عقدة_جديدة
 *
 * @note يتطلب بناء صNet (network/sadnet/)
 * @see builtin_registry.cpp — التسجيل المركزي
 */

#include "interpreter_core.h"
#include "value.h"

#ifdef HAS_SADNET

#include <atomic>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

// ═══════════════════════════════════════════════════════════════════════
// (AR) رؤوس صNet
// (EN) SadNet headers
// ═══════════════════════════════════════════════════════════════════════

#include "sadnet/core/types.h"
#include "sadnet/core/transport.h"
#include "sadnet/crypto/crypto.h"
#include "sadnet/dht/dht.h"
#include "sadnet/nat/nat.h"
#include "sadnet/discovery/discovery.h"
#include "sadnet/messaging/messaging.h"
#include "sadnet/routing/routing.h"
#include "sadnet/identity/identity.h"
#include "sadnet/dns/dns.h"
#include "sadnet/storage/storage.h"
#include "sadnet/security/security.h"
#include "sadnet/streaming/streaming.h"
#include "sadnet/cdn/cdn.h"

namespace Sad
{
    namespace Interpreter
    {

        using ValPtr = std::shared_ptr<Data::Value>;
        using Args = std::vector<ValPtr>;

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) حالة العقدة المحلية — تحتفظ بجميع المديرين
        // (EN) Local node state — holds all managers
        // ═══════════════════════════════════════════════════════════════════════

        struct SadNetNode
        {
            sad::net::NodeId id;
            sad::net::crypto::KeyPair keys;

            // (AR) المديرون — يُنشأون عند الحاجة (كسول)
            std::unique_ptr<sad::net::dht::DHTEngine> dht;
            std::unique_ptr<sad::net::discovery::DiscoveryManager> discovery;
            std::unique_ptr<sad::net::messaging::MessagingManager> messaging;
            std::unique_ptr<sad::net::storage::StorageManager> storage;
            std::unique_ptr<sad::net::dns::DNSManager> dns;
            std::unique_ptr<sad::net::security::SecurityManager> security;
            std::unique_ptr<sad::net::streaming::StreamManager> streaming;
            std::unique_ptr<sad::net::cdn::CDNManager> cdn;
            std::unique_ptr<sad::net::identity::IdentityManager> identity;
            std::unique_ptr<sad::net::routing::OnionRouter> onion;

            SadNetNode()
            {
                // (AR) إنشاء هوية فريدة
                keys = sad::net::crypto::x25519_generate_keypair();
                auto hash = sad::net::crypto::sha256(keys.public_key.data(), keys.public_key.size());
                std::memcpy(id.data.data(), hash.data(), std::min(hash.size(), (size_t)32));
            }

            // (AR) تهيئة كسولة لكل مدير
            void ensure_dht()
            {
                if (!dht)
                    dht = std::make_unique<sad::net::dht::DHTEngine>(id, 7654);
            }
            void ensure_discovery()
            {
                if (!discovery)
                    discovery = std::make_unique<sad::net::discovery::DiscoveryManager>(id);
            }
            void ensure_messaging()
            {
                if (!messaging)
                    messaging = std::make_unique<sad::net::messaging::MessagingManager>(id, keys);
            }
            void ensure_storage()
            {
                if (!storage)
                    storage = std::make_unique<sad::net::storage::StorageManager>(id);
            }
            void ensure_dns()
            {
                if (!dns)
                    dns = std::make_unique<sad::net::dns::DNSManager>(id, keys);
            }
            void ensure_security()
            {
                if (!security)
                    security = std::make_unique<sad::net::security::SecurityManager>(id, keys);
            }
            void ensure_streaming()
            {
                if (!streaming)
                    streaming = std::make_unique<sad::net::streaming::StreamManager>(id);
            }
            void ensure_cdn()
            {
                if (!cdn)
                    cdn = std::make_unique<sad::net::cdn::CDNManager>(id);
            }
            void ensure_identity()
            {
                if (!identity)
                    identity = std::make_unique<sad::net::identity::IdentityManager>(id, keys);
            }
            void ensure_onion()
            {
                if (!onion)
                    onion = std::make_unique<sad::net::routing::OnionRouter>(id);
            }
        };

        // (AR) خريطة عقد صNet — معرّف رقمي → عقدة
        static std::unordered_map<int64_t, std::unique_ptr<SadNetNode>> g_nodes;
        static std::mutex g_nodes_mtx;
        static std::atomic<int64_t> g_next_node_id{1};

        static SadNetNode *getNode(int64_t id)
        {
            std::lock_guard<std::mutex> lock(g_nodes_mtx);
            auto it = g_nodes.find(id);
            return it != g_nodes.end() ? it->second.get() : nullptr;
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) دوال مساعدة — تحويل الأنواع
        // ═══════════════════════════════════════════════════════════════════════

        static ValPtr makeNum(double v) { return std::make_shared<Data::Value>(v); }
        static ValPtr makeStr(const std::string &s) { return std::make_shared<Data::Value>(s); }
        static ValPtr makeBool(bool b) { return std::make_shared<Data::Value>(b); }
        static ValPtr makeNull() { return std::make_shared<Data::Value>(); }

        static std::string nodeIdToHex(const sad::net::NodeId &id)
        {
            return id.to_hex();
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) تسجيل جميع دوال صNet في المفسر
        // (EN) Register all SadNet functions in the interpreter
        // ═══════════════════════════════════════════════════════════════════════

        void registerBuiltinsSadNet(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═════════════════════════════════════════════════════════════════
            // القسم 1: تهيئة العقدة
            // ═════════════════════════════════════════════════════════════════

            // ─── عقدة_جديدة() → رقم (معرّف العقدة) ───────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    (void)args;
                    auto node = std::make_unique<SadNetNode>();
                    int64_t id = g_next_node_id.fetch_add(1);
                    std::lock_guard<std::mutex> lock(g_nodes_mtx);
                    g_nodes[id] = std::move(node);
                    return makeNum(static_cast<double>(id));
                };
                fm.registerBuiltinFunction("\xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9", f); // عقدة_جديدة
                fm.registerBuiltinFunction("snet_new_node", f);
            }

            // ─── معرّف_العقدة(عقدة) → نص ───────────────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.empty())
                        return makeStr("");
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeStr("");
                    return makeStr(nodeIdToHex(node->id));
                };
                fm.registerBuiltinFunction("\xd9\x85\xd8\xb9\xd8\xb1\xd9\x91\xd9\x81_\xd8\xa7\xd9\x84\xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9", f); // معرّف_العقدة
                fm.registerBuiltinFunction("snet_node_id", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 2: التشفير
            // ═════════════════════════════════════════════════════════════════

            // ─── تشفير_sha256(نص) → نص (hex) ───────────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.empty())
                        return makeStr("");
                    std::string input = args[0]->toString();
                    auto hash = sad::net::crypto::sha256(
                        reinterpret_cast<const uint8_t *>(input.data()), input.size());
                    // (AR) تحويل لـ hex
                    std::string hex;
                    for (uint8_t b : hash)
                    {
                        char buf[3];
                        std::snprintf(buf, sizeof(buf), "%02x", b);
                        hex += buf;
                    }
                    return makeStr(hex);
                };
                fm.registerBuiltinFunction("\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1_sha256", f); // تشفير_sha256
                fm.registerBuiltinFunction("snet_sha256", f);
            }

            // ─── بايتات_عشوائية(عدد) → نص (hex) ─────────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.empty())
                        return makeStr("");
                    size_t n = static_cast<size_t>(args[0]->toDouble());
                    if (n > 1024)
                        n = 1024; // حد أقصى
                    auto bytes = sad::net::crypto::random_bytes(n);
                    std::string hex;
                    for (uint8_t b : bytes)
                    {
                        char buf[3];
                        std::snprintf(buf, sizeof(buf), "%02x", b);
                        hex += buf;
                    }
                    return makeStr(hex);
                };
                fm.registerBuiltinFunction("\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa\xd8\xa7\xd8\xaa_\xd8\xb9\xd8\xb4\xd9\x88\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa9", f); // بايتات_عشوائية
                fm.registerBuiltinFunction("snet_random_bytes", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 3: DHT — الجدول الموزع
            // ═════════════════════════════════════════════════════════════════

            // ─── dht_تخزين(عقدة، مفتاح_نص، قيمة_نص) → منطقي ─────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 3)
                        return makeBool(false);
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeBool(false);
                    node->ensure_dht();

                    std::string key_str = args[1]->toString();
                    std::string val_str = args[2]->toString();

                    // (AR) حساب مفتاح DHT من SHA-256
                    auto key_hash = sad::net::crypto::sha256(
                        reinterpret_cast<const uint8_t *>(key_str.data()), key_str.size());
                    sad::net::NodeId key;
                    std::memcpy(key.data.data(), key_hash.data(), std::min(key_hash.size(), (size_t)32));

                    sad::net::dht::DHTValue val;
                    val.data.assign(val_str.begin(), val_str.end());
                    val.timestamp = sad::net::now_ms();

                    return makeBool(node->dht->store(key, val));
                };
                fm.registerBuiltinFunction("dht_\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86", f); // dht_تخزين
                fm.registerBuiltinFunction("snet_dht_store", f);
            }

            // ─── dht_بحث(عقدة، مفتاح_نص) → نص|لاشيء ──────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 2)
                        return makeNull();
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeNull();
                    node->ensure_dht();

                    std::string key_str = args[1]->toString();
                    auto key_hash = sad::net::crypto::sha256(
                        reinterpret_cast<const uint8_t *>(key_str.data()), key_str.size());
                    sad::net::NodeId key;
                    std::memcpy(key.data.data(), key_hash.data(), std::min(key_hash.size(), (size_t)32));

                    auto result = node->dht->find_value(key);
                    if (result.success && result.value.has_value())
                    {
                        auto &val = result.value.value();
                        return makeStr(std::string(val.data.begin(), val.data.end()));
                    }
                    return makeNull();
                };
                fm.registerBuiltinFunction("dht_\xd8\xa8\xd8\xad\xd8\xab", f); // dht_بحث
                fm.registerBuiltinFunction("snet_dht_find", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 4: التخزين الموزع
            // ═════════════════════════════════════════════════════════════════

            // ─── تخزين_ملف(عقدة، اسم، محتوى) → نص (معرّف المحتوى hex) ──────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 3)
                        return makeStr("");
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeStr("");
                    node->ensure_storage();

                    std::string name = args[1]->toString();
                    std::string content = args[2]->toString();
                    sad::net::Buffer data(content.begin(), content.end());

                    auto result = node->storage->store_file(name, data);
                    if (result.success)
                    {
                        return makeStr(result.value.to_hex());
                    }
                    return makeStr("");
                };
                fm.registerBuiltinFunction("\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86_\xd9\x85\xd9\x84\xd9\x81", f); // تخزين_ملف
                fm.registerBuiltinFunction("snet_store_file", f);
            }

            // ─── استرجاع_ملف(عقدة، معرّف_hex) → نص|لاشيء ──────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 2)
                        return makeNull();
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeNull();
                    node->ensure_storage();

                    std::string id_hex = args[1]->toString();
                    sad::net::NodeId content_id = sad::net::NodeId::from_hex(id_hex);

                    auto result = node->storage->retrieve_file(content_id);
                    if (result.success)
                    {
                        return makeStr(std::string(result.value.begin(), result.value.end()));
                    }
                    return makeNull();
                };
                fm.registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9_\xd9\x85\xd9\x84\xd9\x81", f); // استرجاع_ملف
                fm.registerBuiltinFunction("snet_retrieve_file", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 5: DNS لامركزي
            // ═════════════════════════════════════════════════════════════════

            // ─── dns_تسجيل(عقدة، اسم، عنوان_ip) → منطقي ─────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 3)
                        return makeBool(false);
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeBool(false);
                    node->ensure_dns();

                    std::string name = args[1]->toString();
                    std::string ip = args[2]->toString();

                    return makeBool(node->dns->register_name(name, ip));
                };
                fm.registerBuiltinFunction("dns_\xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84", f); // dns_تسجيل
                fm.registerBuiltinFunction("snet_dns_register", f);
            }

            // ─── dns_حل(عقدة، اسم) → نص|لاشيء ───────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 2)
                        return makeNull();
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeNull();
                    node->ensure_dns();

                    std::string name = args[1]->toString();
                    auto records = node->dns->resolve(name);
                    if (!records.empty())
                    {
                        return makeStr(records[0].value);
                    }
                    return makeNull();
                };
                fm.registerBuiltinFunction("dns_\xd8\xad\xd9\x84", f); // dns_حل
                fm.registerBuiltinFunction("snet_dns_resolve", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 6: المراسلة المشفرة
            // ═════════════════════════════════════════════════════════════════

            // ─── رسالة_مشفرة(عقدة، مستقبل_hex، نص_الرسالة) → نص (hex) ────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 3)
                        return makeStr("");
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeStr("");
                    node->ensure_messaging();

                    std::string recipient_hex = args[1]->toString();
                    std::string message_text = args[2]->toString();

                    sad::net::NodeId recipient = sad::net::NodeId::from_hex(recipient_hex);

                    auto result = node->messaging->send_message(
                        recipient, sad::net::messaging::MessageType::TEXT,
                        sad::net::Buffer(message_text.begin(), message_text.end()));

                    if (result.success)
                    {
                        return makeStr("ok");
                    }
                    return makeStr("");
                };
                fm.registerBuiltinFunction("\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x85\xd8\xb4\xd9\x81\xd8\xb1\xd8\xa9", f); // رسالة_مشفرة
                fm.registerBuiltinFunction("snet_send_encrypted", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 7: الأمان والثقة
            // ═════════════════════════════════════════════════════════════════

            // ─── إثبات_عمل(عقدة) → منطقي ─────────────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.empty())
                        return makeBool(false);
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeBool(false);
                    node->ensure_security();

                    auto pow = node->security->generate_pow();
                    return makeBool(pow.verify());
                };
                fm.registerBuiltinFunction("\xd8\xa5\xd8\xab\xd8\xa8\xd8\xa7\xd8\xaa_\xd8\xb9\xd9\x85\xd9\x84", f); // إثبات_عمل
                fm.registerBuiltinFunction("snet_proof_of_work", f);
            }

            // ─── سمعة_قرين(عقدة، قرين_hex) → رقم ─────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 2)
                        return makeNum(0);
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeNum(0);
                    node->ensure_security();

                    std::string peer_hex = args[1]->toString();
                    sad::net::NodeId peer = sad::net::NodeId::from_hex(peer_hex);
                    auto rep = node->security->get_reputation(peer);
                    return makeNum(static_cast<double>(rep.score));
                };
                fm.registerBuiltinFunction("\xd8\xb3\xd9\x85\xd8\xb9\xd8\xa9_\xd9\x82\xd8\xb1\xd9\x8a\xd9\x86", f); // سمعة_قرين
                fm.registerBuiltinFunction("snet_peer_reputation", f);
            }

            // ─── حظر_قرين(عقدة، قرين_hex) → منطقي ────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 2)
                        return makeBool(false);
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeBool(false);
                    node->ensure_security();

                    std::string peer_hex = args[1]->toString();
                    sad::net::NodeId peer = sad::net::NodeId::from_hex(peer_hex);
                    node->security->ban(peer);
                    return makeBool(true);
                };
                fm.registerBuiltinFunction("\xd8\xad\xd8\xb8\xd8\xb1_\xd9\x82\xd8\xb1\xd9\x8a\xd9\x86", f); // حظر_قرين
                fm.registerBuiltinFunction("snet_ban_peer", f);
            }

            // ─── قرين_موثوق(عقدة، قرين_hex) → منطقي ─────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 2)
                        return makeBool(false);
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeBool(false);
                    node->ensure_security();

                    std::string peer_hex = args[1]->toString();
                    sad::net::NodeId peer = sad::net::NodeId::from_hex(peer_hex);
                    return makeBool(node->security->is_trusted(peer));
                };
                fm.registerBuiltinFunction("\xd9\x82\xd8\xb1\xd9\x8a\xd9\x86_\xd9\x85\xd9\x88\xd8\xab\xd9\x88\xd9\x82", f); // قرين_موثوق
                fm.registerBuiltinFunction("snet_is_trusted", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 8: الهوية اللامركزية (DID)
            // ═════════════════════════════════════════════════════════════════

            // ─── هوية_جديدة(عقدة) → نص (DID) ─────────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.empty())
                        return makeStr("");
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeStr("");
                    node->ensure_identity();

                    auto result = node->identity->create_identity();
                    if (result.success)
                    {
                        return makeStr(result.value.id);
                    }
                    return makeStr("");
                };
                fm.registerBuiltinFunction("\xd9\x87\xd9\x88\xd9\x8a\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9", f); // هوية_جديدة
                fm.registerBuiltinFunction("snet_create_identity", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 9: البث المباشر
            // ═════════════════════════════════════════════════════════════════

            // ─── بث_جديد(عقدة، عنوان) → رقم (معرّف البث) ─────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 2)
                        return makeNum(-1);
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeNum(-1);
                    node->ensure_streaming();

                    std::string title = args[1]->toString();
                    std::vector<sad::net::streaming::QualityLevel> levels;
                    sad::net::streaming::QualityLevel q;
                    q.bitrate = 2000000;
                    q.width = 1280;
                    q.height = 720;
                    q.fps = 30;
                    q.codec = "H264";
                    levels.push_back(q);

                    uint64_t sid = node->streaming->create_stream(title, levels);
                    return makeNum(static_cast<double>(sid));
                };
                fm.registerBuiltinFunction("\xd8\xa8\xd8\xab_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // بث_جديد
                fm.registerBuiltinFunction("snet_create_stream", f);
            }

            // ─── بث_إيقاف(عقدة، معرّف_البث) → منطقي ─────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 2)
                        return makeBool(false);
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeBool(false);
                    node->ensure_streaming();

                    uint64_t sid = static_cast<uint64_t>(args[1]->toDouble());
                    node->streaming->stop_stream(sid);
                    return makeBool(true);
                };
                fm.registerBuiltinFunction("\xd8\xa8\xd8\xab_\xd8\xa5\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81", f); // بث_إيقاف
                fm.registerBuiltinFunction("snet_stop_stream", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 10: CDN لامركزي
            // ═════════════════════════════════════════════════════════════════

            // ─── cdn_نشر(عقدة، اسم، نوع_mime، محتوى) → نص (معرّف hex) ────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 4)
                        return makeStr("");
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeStr("");
                    node->ensure_cdn();

                    std::string name = args[1]->toString();
                    std::string mime = args[2]->toString();
                    std::string content = args[3]->toString();
                    sad::net::Buffer data(content.begin(), content.end());

                    auto id = node->cdn->publish_content(name, mime, data);
                    return makeStr(id.to_hex());
                };
                fm.registerBuiltinFunction("cdn_\xd9\x86\xd8\xb4\xd8\xb1", f); // cdn_نشر
                fm.registerBuiltinFunction("snet_cdn_publish", f);
            }

            // ─── cdn_محتوى(عقدة، معرّف_hex) → نص|لاشيء ───────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.size() < 2)
                        return makeNull();
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeNull();
                    node->ensure_cdn();

                    std::string id_hex = args[1]->toString();
                    sad::net::NodeId cid = sad::net::NodeId::from_hex(id_hex);

                    auto result = node->cdn->request_content(cid);
                    if (result.success)
                    {
                        return makeStr(std::string(result.value.begin(), result.value.end()));
                    }
                    return makeNull();
                };
                fm.registerBuiltinFunction("cdn_\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", f); // cdn_محتوى
                fm.registerBuiltinFunction("snet_cdn_get", f);
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 11: أدوات عامة
            // ═════════════════════════════════════════════════════════════════

            // ─── snet_إصدار() → نص ──────────────────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    (void)args;
                    return makeStr("sadnet 1.0.0");
                };
                fm.registerBuiltinFunction("snet_\xd8\xa5\xd8\xb5\xd8\xaf\xd8\xa7\xd8\xb1", f); // snet_إصدار
                fm.registerBuiltinFunction("snet_version", f);
            }

            // ─── تدمير_عقدة(عقدة) → منطقي ──────────────────────────────
            {
                auto f = [](const Args &args) -> ValPtr
                {
                    if (args.empty())
                        return makeBool(false);
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    std::lock_guard<std::mutex> lock(g_nodes_mtx);
                    return makeBool(g_nodes.erase(id) > 0);
                };
                fm.registerBuiltinFunction("\xd8\xaa\xd8\xaf\xd9\x85\xd9\x8a\xd8\xb1_\xd8\xb9\xd9\x82\xd8\xaf\xd8\xa9", f); // تدمير_عقدة
                fm.registerBuiltinFunction("snet_destroy_node", f);
            }
        }

    } // namespace Interpreter
} // namespace Sad

#else // !HAS_SADNET

// (AR) نسخة بديلة فارغة — صNet غير مفعّلة
namespace Sad
{
    namespace Interpreter
    {
        void registerBuiltinsSadNet(Interpreter &) {}
    } // namespace Interpreter
} // namespace Sad

#endif // HAS_SADNET
