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
 * @note يتطلب بناء صNet (features/network/sadnet/)
 * @see builtin_registry.cpp — التسجيل المركزي
 */

#include "interpreter_core.h"
#include "builtin_registry.h"
#include "builtins/builtin_context.h"
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
                    discovery = std::make_unique<sad::net::discovery::DiscoveryManager>(id, 7654);
            }
            void ensure_messaging()
            {
                if (!messaging)
                    messaging = std::make_unique<sad::net::messaging::MessagingManager>(id, keys);
            }
            void ensure_storage()
            {
                if (!storage)
                    storage = std::make_unique<sad::net::storage::StorageManager>(id, keys);
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
                    identity = std::make_unique<sad::net::identity::IdentityManager>(keys);
            }
            void ensure_onion()
            {
                if (!onion)
                    onion = std::make_unique<sad::net::routing::OnionRouter>(keys);
            }
        };

        // (AR) خريطة عقد صNet — معرّف رقمي → عقدة
        static std::unordered_map<int64_t, std::unique_ptr<SadNetNode>> g_nodes;
        static std::mutex g_nodes_mtx;
        static std::atomic<int64_t> g_next_node_id{1};

        // (AR) 🔑 سجلُّ بياناتِ الملفّات. تغيّر عقدُ التخزين: `store_file` يُرجعُ
        //      `FileManifest` كاملًا، و`retrieve_file` يطلبُه كاملًا — لا معرّفًا
        //      وحدَه (storage.h:113,122). وسطحُ اللغةِ يُعطي المستخدمَ نصًّا
        //      واحدًا، فيُحفَظُ البيانُ ههنا مفهرسًا بـhex الجذرِ ويُستردُّ به.
        //      وبلا هذا السجلِّ لا يمكنُ لـ`استرجاع_ملف` أن تعملَ البتّة.
        // (EN) Manifest registry: retrieve_file needs the whole manifest, but the
        //      language surface hands the user only the root id string.
        static std::unordered_map<std::string, sad::net::storage::FileManifest> g_manifests;
        static std::mutex g_manifests_mtx;

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
        // (AR) 🔑 «فراغٌ» باسمِه الصادقِ لا «عدم»: هذه القيمةُ وسمُها Void، وهي
        //      عائدُ «بحثٍ بلا مطابقة» (dht_بحث، dns_حل، استرجاع_ملف، cdn_محتوى)
        //      — وهو بنصِّ مصدرِ الحقيقةِ (type.void) فراغٌ لا عدم، وبعُرفِ وحداتِ
        //      الخرائطِ الخمسِ (makeVoidVal). كان الاسمُ makeNull فيوهم قارئَه
        //      أنّ العائدَ يساوي «لاشيء» بالمقارنة — والحقُّ أنّ فراغ == لاشيء
        //      «خطأ»، والفحصُ الصادقُ صِدقيّةُ القيمةِ أو نوع().
        // (EN) Void under its honest name, not "null": this value's tag is Void —
        //      the return of a no-match lookup, which the SoT (type.void) and the
        //      five maps modules (makeVoidVal) both define as void, not null. The
        //      old name makeNull misled readers into expecting `== لاشيء` to hold;
        //      it does not (Void == Null is false). Test truthiness or نوع().
        static ValPtr makeVoidVal() { return std::make_shared<Data::Value>(); }

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

            // (AR) 🔑 كُتبت أذرعُ هذه الوحدةِ كلُّها على التوقيعِ القديمِ
            //      `(const Args&)`، وقد **حُذف** من `FunctionManager` عند هجرةِ
            //      ADR-EM-CPP-1 إلى `BuiltinContext&` (راجع function_manager.h:200).
            //      ولم يحمرَّ شيءٌ لأنّ الملفَّ كلَّه كان خلفَ `#ifdef HAS_SADNET`
            //      لم يصدُقْ قطّ — فالهجرةُ الشاملةُ مرّت عليه ولم تمسَّه، إذ
            //      **لا يُهاجَرُ ما لا يُترجَم**. فلمّا أُحييَ البناءُ ظهرت
            //      ٤٤ عطبًا دفعةً واحدة (C2664 لكلِّ لامبدا وتسجيلِها).
            //      وجُسِر ههنا بنفسِ نسقِ أختِها المهاجَرةِ builtin_module_http.cpp:419.
            // (EN) All arms here were written against the removed `(const Args&)`
            //      signature; the ADR-EM-CPP-1 migration skipped this TU because a
            //      dead #ifdef meant it never compiled. Bridged like the HTTP module.

            // (AR) اختصار لأسماء ثوابت الشبكة اللامركزية
            namespace Bn = Builtins::Names::SadNet;

            // ═════════════════════════════════════════════════════════════════
            // القسم 1: تهيئة العقدة
            // ═════════════════════════════════════════════════════════════════

            // ─── عقدة_جديدة() → رقم (معرّف العقدة) ───────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    (void)args;
                    auto node = std::make_unique<SadNetNode>();
                    int64_t id = g_next_node_id.fetch_add(1);
                    std::lock_guard<std::mutex> lock(g_nodes_mtx);
                    g_nodes[id] = std::move(node);
                    return makeNum(static_cast<double>(id));
                };
                fm.registerBuiltinFunction(std::string(Bn::NODE_NEW), f); // عقدة_جديدة
            }

            // ─── معرّف_العقدة(عقدة) → نص ───────────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
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
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
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
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.empty())
                        return makeStr("");
                    size_t n = static_cast<size_t>(args[0]->toDouble());
                    if (n > 1024)
                        n = 1024; // حد أقصى
                    // (AR) العقدُ الحاليّ: `bool random_bytes(uint8_t*, size_t)`
                    //      يملأُ مخزنًا ويُرجعُ نجاحَه — لا يُرجعُ المتّجه.
                    std::vector<uint8_t> bytes(n);
                    if (n && !sad::net::crypto::random_bytes(bytes.data(), n))
                        return makeStr("");
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
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
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

                    // (AR) `store` يأخذُ `Buffer` مباشرةً لا `DHTValue`؛ والطابعُ
                    //      الزمنيُّ يضعُه المحرّكُ نفسُه عند التخزين.
                    sad::net::Buffer val(val_str);
                    return makeBool(node->dht->store(key, val).ok());
                };
                fm.registerBuiltinFunction(std::string(Bn::DHT_STORE), f); // dht_تخزين
            }

            // ─── dht_بحث(عقدة، مفتاح_نص) → نص|فراغ ──────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.size() < 2)
                        return makeVoidVal();
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeVoidVal();
                    node->ensure_dht();

                    std::string key_str = args[1]->toString();
                    auto key_hash = sad::net::crypto::sha256(
                        reinterpret_cast<const uint8_t *>(key_str.data()), key_str.size());
                    sad::net::NodeId key;
                    std::memcpy(key.data.data(), key_hash.data(), std::min(key_hash.size(), (size_t)32));

                    // (AR) `Result<Buffer>` بحقلَي `ok()`/`value` — لا `success`
                    //      ولا `optional`. راجع core/types.h:263.
                    auto result = node->dht->find_value(key);
                    if (result.ok())
                        return makeStr(std::string(result.value.begin(), result.value.end()));
                    return makeVoidVal();
                };
                fm.registerBuiltinFunction(std::string(Bn::DHT_LOOKUP), f); // dht_بحث
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 4: التخزين الموزع
            // ═════════════════════════════════════════════════════════════════

            // ─── تخزين_ملف(عقدة، اسم، محتوى) → نص (معرّف المحتوى hex) ──────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.size() < 3)
                        return makeStr("");
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeStr("");
                    node->ensure_storage();

                    std::string name = args[1]->toString();
                    std::string content = args[2]->toString();
                    sad::net::Buffer data(content);

                    auto result = node->storage->store_file(name, data);
                    if (result.ok())
                    {
                        std::string root = result.value.root_id.to_hex();
                        {
                            std::lock_guard<std::mutex> lock(g_manifests_mtx);
                            g_manifests[root] = result.value;
                        }
                        return makeStr(root);
                    }
                    return makeStr("");
                };
                fm.registerBuiltinFunction(std::string(Bn::STORE_FILE), f); // تخزين_ملف
            }

            // ─── استرجاع_ملف(عقدة، معرّف_hex) → نص|فراغ ──────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.size() < 2)
                        return makeVoidVal();
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeVoidVal();
                    node->ensure_storage();

                    std::string id_hex = args[1]->toString();
                    sad::net::storage::FileManifest manifest;
                    {
                        std::lock_guard<std::mutex> lock(g_manifests_mtx);
                        auto it = g_manifests.find(id_hex);
                        if (it == g_manifests.end())
                            return makeVoidVal();
                        manifest = it->second;
                    }

                    auto result = node->storage->retrieve_file(manifest);
                    if (result.ok())
                    {
                        return makeStr(std::string(result.value.begin(), result.value.end()));
                    }
                    return makeVoidVal();
                };
                fm.registerBuiltinFunction(std::string(Bn::RETRIEVE_FILE), f); // استرجاع_ملف
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 5: DNS لامركزي
            // ═════════════════════════════════════════════════════════════════

            // ─── dns_تسجيل(عقدة، اسم، عنوان_ip) → منطقي ─────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.size() < 3)
                        return makeBool(false);
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeBool(false);
                    node->ensure_dns();

                    std::string name = args[1]->toString();
                    std::string ip = args[2]->toString();

                    // (AR) العقدُ يطلبُ نوعَ السجلِّ صراحةً: A لعنوانِ IPv4
                    //      (dns.h:89). والقيمةُ الافتراضيّةُ للـttl تكفي.
                    return makeBool(node->dns
                                        ->register_name(name, sad::net::dns::RecordType::A, ip)
                                        .ok());
                };
                fm.registerBuiltinFunction(std::string(Bn::DNS_NAME_REGISTER), f); // dns_تسجيل
            }

            // ─── dns_حل(عقدة، اسم) → نص|فراغ ───────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.size() < 2)
                        return makeVoidVal();
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeVoidVal();
                    node->ensure_dns();

                    std::string name = args[1]->toString();
                    auto records = node->dns->resolve(name);
                    if (!records.empty())
                    {
                        return makeStr(records[0].value);
                    }
                    return makeVoidVal();
                };
                fm.registerBuiltinFunction(std::string(Bn::DNS_RESOLVE), f); // dns_حل
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 6: المراسلة المشفرة
            // ═════════════════════════════════════════════════════════════════

            // ─── رسالة_مشفرة(عقدة، مستقبل_hex، نص_الرسالة) → نص (hex) ────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.size() < 3)
                        return makeStr("");
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeStr("");
                    node->ensure_messaging();

                    std::string recipient_hex = args[1]->toString();
                    std::string message_text = args[2]->toString();

                    sad::net::NodeId recipient = sad::net::NodeId::from_hex(recipient_hex);

                    // (AR) لا `send_message` في العقدِ الحاليّ؛ الإرسالُ النصّيُّ
                    //      المشفَّرُ هو `send_text` (messaging.h:181).
                    auto result = node->messaging->send_text(recipient, message_text);

                    if (result.ok())
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
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
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
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
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
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
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
                fm.registerBuiltinFunction(std::string(Bn::PEER_BAN), f); // حظر_قرين
            }

            // ─── قرين_موثوق(عقدة، قرين_hex) → منطقي ─────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
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
                fm.registerBuiltinFunction(std::string(Bn::PEER_TRUSTED), f); // قرين_موثوق
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 8: الهوية اللامركزية (DID)
            // ═════════════════════════════════════════════════════════════════

            // ─── هوية_جديدة(عقدة) → نص (DID) ─────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.empty())
                        return makeStr("");
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeStr("");
                    node->ensure_identity();

                    // (AR) `create_identity` تُرجعُ `DIDDocument` مباشرةً لا
                    //      `Result<…>`، وحقلُ المعرّفِ اسمُه `did` (identity.h:78,134).
                    auto doc = node->identity->create_identity();
                    return makeStr(doc.did);
                };
                fm.registerBuiltinFunction(std::string(Bn::IDENTITY_NEW), f); // هوية_جديدة
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 9: البث المباشر
            // ═════════════════════════════════════════════════════════════════

            // ─── بث_جديد(عقدة، عنوان) → رقم (معرّف البث) ─────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
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
                fm.registerBuiltinFunction(std::string(Bn::BROADCAST_NEW), f); // بث_جديد
            }

            // ─── بث_إيقاف(عقدة، معرّف_البث) → منطقي ─────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
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
                fm.registerBuiltinFunction(std::string(Bn::BROADCAST_STOP), f); // بث_إيقاف
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 10: CDN لامركزي
            // ═════════════════════════════════════════════════════════════════

            // ─── cdn_نشر(عقدة، اسم، نوع_mime، محتوى) → نص (معرّف hex) ────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.size() < 4)
                        return makeStr("");
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeStr("");
                    node->ensure_cdn();

                    std::string name = args[1]->toString();
                    std::string mime = args[2]->toString();
                    std::string content = args[3]->toString();
                    sad::net::Buffer data(content);

                    auto id = node->cdn->publish_content(name, mime, data);
                    return makeStr(id.to_hex());
                };
                fm.registerBuiltinFunction(std::string(Bn::CDN_PUBLISH), f); // cdn_نشر
            }

            // ─── cdn_محتوى(عقدة، معرّف_hex) → نص|فراغ ───────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.size() < 2)
                        return makeVoidVal();
                    auto *node = getNode(static_cast<int64_t>(args[0]->toDouble()));
                    if (!node)
                        return makeVoidVal();
                    node->ensure_cdn();

                    std::string id_hex = args[1]->toString();
                    sad::net::NodeId cid = sad::net::NodeId::from_hex(id_hex);

                    auto result = node->cdn->request_content(cid);
                    if (result.success)
                    {
                        return makeStr(std::string(result.value.begin(), result.value.end()));
                    }
                    return makeVoidVal();
                };
                fm.registerBuiltinFunction(std::string(Bn::CDN_CONTENT), f); // cdn_محتوى
            }

            // ═════════════════════════════════════════════════════════════════
            // القسم 11: أدوات عامة
            // ═════════════════════════════════════════════════════════════════

            // ─── snet_إصدار() → نص ──────────────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    (void)args;
                    return makeStr("sadnet 1.0.0");
                };
                fm.registerBuiltinFunction(std::string(Bn::NET_VERSION), f); // snet_إصدار
            }

            // ─── تدمير_عقدة(عقدة) → منطقي ──────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> ValPtr
                {
                    const Args &args = ctx.args();
                    (void)args;
                    if (args.empty())
                        return makeBool(false);
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    std::lock_guard<std::mutex> lock(g_nodes_mtx);
                    return makeBool(g_nodes.erase(id) > 0);
                };
                fm.registerBuiltinFunction(std::string(Bn::NODE_DESTROY), f); // تدمير_عقدة
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
