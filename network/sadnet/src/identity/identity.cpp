// ═══════════════════════════════════════════════════════════════════
// صNet — تنفيذ الهوية اللامركزية (DID)
// (AR) وثائق DID، أوراق اعتماد، مدير هوية
//      ترميز ثنائي للتخزين في DHT
// (EN) DID implementation: documents, credentials, identity manager
// المكون: network/sadnet (الهوية اللامركزية DID)
// المسار: network/sadnet/src/identity/identity.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/identity/identity.h"
#include <algorithm>
#include <cstring>
#include <mutex>

namespace sad::net::identity
{

    // استخدام دوال التشفير المطلوبة
    using crypto::hmac_sha256;
    using crypto::sha256;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مساعدات ترميز
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
    static void w64(uint8_t *p, uint64_t v)
    {
        w32(p, (uint32_t)(v >> 32));
        w32(p + 4, (uint32_t)v);
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
    static uint64_t r64(const uint8_t *p)
    {
        return ((uint64_t)r32(p) << 32) | r32(p + 4);
    }

    // ═══════════════════════════════════════════════════════════════════
    // (AR) DIDDocument — ترميز/فك ترميز
    //      [version:4][node_id:32][created:8][updated:8]
    //      [auth_count:2][auth_methods...][ka_count:2][key_agreement...]
    //      [svc_count:2][services...]
    // ═══════════════════════════════════════════════════════════════════

    std::string DIDDocument::make_did(const NodeId &id)
    {
        return "did:snet:" + id.to_hex().substr(0, 32);
    }

    // مساعد: كتابة سلسلة نصية بطول
    static void write_str(Buffer &buf, const std::string &s)
    {
        size_t off = buf.size();
        buf.resize(off + 2 + s.size());
        w16(buf.data() + off, (uint16_t)s.size());
        if (!s.empty())
            std::memcpy(buf.data() + off + 2, s.c_str(), s.size());
    }

    // مساعد: كتابة مخزن بطول
    static void write_buf(Buffer &buf, const Buffer &data)
    {
        size_t off = buf.size();
        buf.resize(off + 2 + data.size());
        w16(buf.data() + off, (uint16_t)data.size());
        if (!data.empty())
            std::memcpy(buf.data() + off + 2, data.data(), data.size());
    }

    Buffer DIDDocument::serialize() const
    {
        Buffer buf;
        buf.resize(4 + NODE_ID_SIZE + 8 + 8);
        uint8_t *p = buf.data();

        w32(p, version);
        std::memcpy(p + 4, node_id.data.data(), NODE_ID_SIZE);
        w64(p + 4 + NODE_ID_SIZE, created);
        w64(p + 12 + NODE_ID_SIZE, updated);

        // طرق المصادقة
        size_t off = buf.size();
        buf.resize(off + 2);
        w16(buf.data() + off, (uint16_t)auth_methods.size());
        for (const auto &vm : auth_methods)
        {
            write_str(buf, vm.id);
            off = buf.size();
            buf.resize(off + 1 + 8 + 8);
            buf[off] = (uint8_t)vm.type;
            w64(buf.data() + off + 1, vm.created_at);
            w64(buf.data() + off + 9, vm.expires_at);
            write_buf(buf, vm.public_key);
        }

        // طرق اتفاق المفاتيح
        off = buf.size();
        buf.resize(off + 2);
        w16(buf.data() + off, (uint16_t)key_agreement.size());
        for (const auto &vm : key_agreement)
        {
            write_str(buf, vm.id);
            off = buf.size();
            buf.resize(off + 1 + 8 + 8);
            buf[off] = (uint8_t)vm.type;
            w64(buf.data() + off + 1, vm.created_at);
            w64(buf.data() + off + 9, vm.expires_at);
            write_buf(buf, vm.public_key);
        }

        // الخدمات
        off = buf.size();
        buf.resize(off + 2);
        w16(buf.data() + off, (uint16_t)services.size());
        for (const auto &svc : services)
        {
            write_str(buf, svc.id);
            write_str(buf, svc.type);
            write_str(buf, svc.address.address);
            off = buf.size();
            buf.resize(off + 2);
            w16(buf.data() + off, svc.address.port);
        }

        return buf;
    }

    // مساعد: قراءة سلسلة نصية
    static std::string read_str(const uint8_t *data, size_t len, size_t &off)
    {
        if (off + 2 > len)
            return "";
        uint16_t slen = r16(data + off);
        off += 2;
        if (off + slen > len)
            return "";
        std::string s(reinterpret_cast<const char *>(data + off), slen);
        off += slen;
        return s;
    }

    // مساعد: قراءة مخزن
    static Buffer read_buf(const uint8_t *data, size_t len, size_t &off)
    {
        if (off + 2 > len)
            return Buffer();
        uint16_t blen = r16(data + off);
        off += 2;
        if (off + blen > len)
            return Buffer();
        Buffer b(data + off, blen);
        off += blen;
        return b;
    }

    DIDDocument DIDDocument::deserialize(const uint8_t *data, size_t len)
    {
        DIDDocument doc;
        if (len < 4 + NODE_ID_SIZE + 16)
            return doc;

        doc.version = r32(data);
        std::memcpy(doc.node_id.data.data(), data + 4, NODE_ID_SIZE);
        doc.created = r64(data + 4 + NODE_ID_SIZE);
        doc.updated = r64(data + 12 + NODE_ID_SIZE);
        doc.did = make_did(doc.node_id);

        size_t off = 4 + NODE_ID_SIZE + 16;

        // طرق المصادقة
        if (off + 2 > len)
            return doc;
        uint16_t auth_count = r16(data + off);
        off += 2;
        for (uint16_t i = 0; i < auth_count; i++)
        {
            VerificationMethod vm;
            vm.id = read_str(data, len, off);
            if (off + 17 > len)
                break;
            vm.type = (KeyType)data[off];
            off++;
            vm.created_at = r64(data + off);
            off += 8;
            vm.expires_at = r64(data + off);
            off += 8;
            vm.public_key = read_buf(data, len, off);
            doc.auth_methods.push_back(vm);
        }

        // طرق اتفاق المفاتيح
        if (off + 2 > len)
            return doc;
        uint16_t ka_count = r16(data + off);
        off += 2;
        for (uint16_t i = 0; i < ka_count; i++)
        {
            VerificationMethod vm;
            vm.id = read_str(data, len, off);
            if (off + 17 > len)
                break;
            vm.type = (KeyType)data[off];
            off++;
            vm.created_at = r64(data + off);
            off += 8;
            vm.expires_at = r64(data + off);
            off += 8;
            vm.public_key = read_buf(data, len, off);
            doc.key_agreement.push_back(vm);
        }

        // الخدمات
        if (off + 2 > len)
            return doc;
        uint16_t svc_count = r16(data + off);
        off += 2;
        for (uint16_t i = 0; i < svc_count; i++)
        {
            ServiceEndpoint svc;
            svc.id = read_str(data, len, off);
            svc.type = read_str(data, len, off);
            svc.address.address = read_str(data, len, off);
            if (off + 2 > len)
                break;
            svc.address.port = r16(data + off);
            off += 2;
            doc.services.push_back(svc);
        }

        return doc;
    }

    // ═══════════════════════════════════════════════════════════════════
    // (AR) Credential — ترميز/فك ترميز
    // ═══════════════════════════════════════════════════════════════════

    Buffer Credential::serialize() const
    {
        Buffer buf;
        write_str(buf, id);
        write_str(buf, type);
        write_str(buf, issuer_did);
        write_str(buf, subject_did);

        // المطالبات
        size_t off = buf.size();
        buf.resize(off + 2);
        w16(buf.data() + off, (uint16_t)claims.size());
        for (const auto &kv : claims)
        {
            write_str(buf, kv.first);
            write_str(buf, kv.second);
        }

        off = buf.size();
        buf.resize(off + 16);
        w64(buf.data() + off, issued_at);
        w64(buf.data() + off + 8, expires_at);

        write_buf(buf, signature);
        return buf;
    }

    Credential Credential::deserialize(const uint8_t *data, size_t len)
    {
        Credential cred;
        size_t off = 0;
        cred.id = read_str(data, len, off);
        cred.type = read_str(data, len, off);
        cred.issuer_did = read_str(data, len, off);
        cred.subject_did = read_str(data, len, off);

        if (off + 2 > len)
            return cred;
        uint16_t claim_count = r16(data + off);
        off += 2;
        for (uint16_t i = 0; i < claim_count; i++)
        {
            auto k = read_str(data, len, off);
            auto v = read_str(data, len, off);
            cred.claims[k] = v;
        }

        if (off + 16 > len)
            return cred;
        cred.issued_at = r64(data + off);
        off += 8;
        cred.expires_at = r64(data + off);
        off += 8;

        cred.signature = read_buf(data, len, off);
        return cred;
    }

    // ═══════════════════════════════════════════════════════════════════
    // (AR) مدير الهوية
    // ═══════════════════════════════════════════════════════════════════

    struct IdentityManager::Impl
    {
        KeyPair keys;
        DIDDocument local_doc;
        std::map<NodeId, DIDDocument> remote_docs;
        std::vector<Credential> credentials;
        mutable std::mutex mutex;

        Impl(const KeyPair &k) : keys(k) {}
    };

    IdentityManager::IdentityManager(const KeyPair &identity_keys)
        : impl_(std::make_unique<Impl>(identity_keys)) {}

    IdentityManager::~IdentityManager() = default;

    DIDDocument IdentityManager::create_identity()
    {
        auto hash = sha256(impl_->keys.public_key.data(), impl_->keys.public_key.size());
        NodeId id;
        std::memcpy(id.data.data(), hash.data(), NODE_ID_SIZE);

        DIDDocument doc;
        doc.did = DIDDocument::make_did(id);
        doc.node_id = id;
        doc.created = now_ms();
        doc.updated = now_ms();
        doc.version = 1;

        // إضافة مفتاح المصادقة
        VerificationMethod auth;
        auth.id = doc.did + "#key-1";
        auth.type = KeyType::X25519;
        auth.public_key = Buffer(impl_->keys.public_key.data(), impl_->keys.public_key.size());
        auth.created_at = now_ms();
        doc.auth_methods.push_back(auth);

        // إضافة مفتاح اتفاق المفاتيح
        VerificationMethod ka;
        ka.id = doc.did + "#ka-1";
        ka.type = KeyType::X25519;
        ka.public_key = Buffer(impl_->keys.public_key.data(), impl_->keys.public_key.size());
        ka.created_at = now_ms();
        doc.key_agreement.push_back(ka);

        impl_->local_doc = doc;
        return doc;
    }

    const DIDDocument &IdentityManager::local_document() const
    {
        return impl_->local_doc;
    }

    void IdentityManager::add_service(const std::string &type, const Endpoint &address)
    {
        ServiceEndpoint svc;
        svc.id = impl_->local_doc.did + "#svc-" + std::to_string(impl_->local_doc.services.size() + 1);
        svc.type = type;
        svc.address = address;
        impl_->local_doc.services.push_back(svc);
        impl_->local_doc.updated = now_ms();
        impl_->local_doc.version++;
    }

    void IdentityManager::add_verification_method(const Buffer &public_key,
                                                  KeyType type)
    {
        VerificationMethod vm;
        vm.id = impl_->local_doc.did + "#key-" +
                std::to_string(impl_->local_doc.auth_methods.size() + 1);
        vm.type = type;
        vm.public_key = public_key;
        vm.created_at = now_ms();
        impl_->local_doc.auth_methods.push_back(vm);
        impl_->local_doc.updated = now_ms();
        impl_->local_doc.version++;
    }

    Result<void> IdentityManager::rotate_key(const KeyPair &new_keys)
    {
        impl_->keys = new_keys;

        // تحديث المفتاح الأول
        if (!impl_->local_doc.auth_methods.empty())
        {
            impl_->local_doc.auth_methods[0].public_key = Buffer(new_keys.public_key.data(), new_keys.public_key.size());
        }
        if (!impl_->local_doc.key_agreement.empty())
        {
            impl_->local_doc.key_agreement[0].public_key = Buffer(new_keys.public_key.data(), new_keys.public_key.size());
        }

        impl_->local_doc.updated = now_ms();
        impl_->local_doc.version++;
        return Result<void>::success();
    }

    void IdentityManager::register_remote(const DIDDocument &doc)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->remote_docs[doc.node_id] = doc;
    }

    const DIDDocument *IdentityManager::resolve(const std::string &did) const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        if (impl_->local_doc.did == did)
            return &impl_->local_doc;
        for (const auto &kv : impl_->remote_docs)
            if (kv.second.did == did)
                return &kv.second;
        return nullptr;
    }

    const DIDDocument *IdentityManager::resolve(const NodeId &id) const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        if (impl_->local_doc.node_id == id)
            return &impl_->local_doc;
        auto it = impl_->remote_docs.find(id);
        if (it != impl_->remote_docs.end())
            return &it->second;
        return nullptr;
    }

    bool IdentityManager::verify_document(const DIDDocument &doc) const
    {
        // التحقق الأساسي: هل DID يطابق node_id
        auto expected_did = DIDDocument::make_did(doc.node_id);
        if (doc.did != expected_did)
            return false;

        // التحقق من وجود مفاتيح صالحة
        for (const auto &vm : doc.auth_methods)
            if (!vm.is_expired() && !vm.public_key.empty())
                return true;

        return false;
    }

    Credential IdentityManager::issue_credential(const std::string &subject_did,
                                                 const std::string &type,
                                                 const std::map<std::string, std::string> &claims,
                                                 uint64_t validity_ms)
    {
        Credential cred;
        // معرف فريد من التوقيت
        uint8_t rnd[8];
        crypto::random_bytes(rnd, 8);
        cred.id = "cred:" + Buffer(rnd, 8).to_hex();
        cred.type = type;
        cred.issuer_did = impl_->local_doc.did;
        cred.subject_did = subject_did;
        cred.claims = claims;
        cred.issued_at = now_ms();
        cred.expires_at = validity_ms > 0 ? now_ms() + validity_ms : 0;

        // توقيع الاعتماد: HMAC-SHA256 للمحتوى
        auto serialized = cred.serialize();
        auto sig_hash = crypto::hmac_sha256(impl_->keys.private_key.data(), impl_->keys.private_key.size(),
                                            serialized.data(), serialized.size());
        cred.signature = Buffer(sig_hash.data(), sig_hash.size());

        return cred;
    }

    bool IdentityManager::verify_credential(const Credential &cred) const
    {
        if (cred.is_expired())
            return false;

        // حل هوية المُصدر
        auto *issuer_doc = resolve(cred.issuer_did);
        if (!issuer_doc)
            return false;

        // التحقق من التوقيع
        Credential copy = cred;
        copy.signature.clear();
        auto serialized = copy.serialize();

        // التحقق باستخدام المفتاح العام للمُصدر
        for (const auto &vm : issuer_doc->auth_methods)
        {
            if (vm.is_expired())
                continue;
            auto expected_sig = crypto::hmac_sha256(vm.public_key.data(), vm.public_key.size(),
                                                    serialized.data(), serialized.size());
            // ملاحظة: هذا HMAC وليس توقيع حقيقي (Ed25519 سيُضاف لاحقاً)
            // للإصدار الحالي نقبل التحقق المحلي فقط
            Buffer expected_buf(expected_sig.data(), expected_sig.size());
            if (expected_buf == cred.signature)
                return true;
        }

        return false;
    }

    void IdentityManager::store_credential(const Credential &cred)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->credentials.push_back(cred);
    }

    std::vector<Credential> IdentityManager::get_credentials(
        const std::string &subject_did) const
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        if (subject_did.empty())
            return impl_->credentials;

        std::vector<Credential> result;
        for (const auto &c : impl_->credentials)
            if (c.subject_did == subject_did)
                result.push_back(c);
        return result;
    }

} // namespace sad::net::identity
