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
#include "builtin_registry.h"
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

            // (AR) اختصار لأسماء ثوابت الشبكة اللامركزية
            namespace Bn = Builtins::Names::SadNet;

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
                fm.registerBuiltinFunction(std::string(Bn::NEW_NODE), f); // عقدة_جديدة
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
                fm.registerBuiltinFunction(std::string(Bn::NODE_ID), f); // معرّف_العقدة
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
                fm.registerBuiltinFunction(std::string(Bn::SHA256), f); // تشفير_sha256
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
                fm.registerBuiltinFunction(std::string(Bn::RANDOM_BYTES), f); // بايتات_عشوائية
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
                fm.registerBuiltinFunction(std::string(Bn::STORE_FILE), f); // تخزين_ملف
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
                fm.registerBuiltinFunction(std::string(Bn::RETRIEVE_FILE), f); // استرجاع_ملف
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
                fm.registerBuiltinFunction(std::string(Bn::ENCRYPTED_MSG), f); // رسالة_مشفرة
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
                fm.registerBuiltinFunction(std::string(Bn::PROOF_OF_WORK), f); // إثبات_عمل
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
                fm.registerBuiltinFunction(std::string(Bn::PEER_REPUTATION), f); // سمعة_قرين
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
                fm.registerBuiltinFunction(std::string(Bn::BLOCK_PEER), f); // حظر_قرين
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
                fm.registerBuiltinFunction(std::string(Bn::TRUSTED_PEER), f); // قرين_موثوق
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
                fm.registerBuiltinFunction(std::string(Bn::NEW_IDENTITY), f); // هوية_جديدة
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
                fm.registerBuiltinFunction(std::string(Bn::NEW_BROADCAST), f); // بث_جديد
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
                fm.registerBuiltinFunction(std::string(Bn::STOP_BROADCAST), f); // بث_إيقاف
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
                fm.registerBuiltinFunction(std::string(Bn::DESTROY_NODE), f); // تدمير_عقدة
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
