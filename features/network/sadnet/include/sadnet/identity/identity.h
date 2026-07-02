// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة الهوية اللامركزية (DID)
// (AR) هويات لامركزية مستقلة (Self-Sovereign Identity)
//      وثائق DID، أوراق اعتماد قابلة للتحقق (Verifiable Credentials)
//      تخزين في DHT، لا تعتمد على سلطة مركزية
// (EN) Decentralized Identity: DID documents, Verifiable Credentials
// المكون: features/network/sadnet (الهوية اللامركزية DID)
// المسار: features/network/sadnet/include/sadnet/identity/identity.h
// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_IDENTITY_IDENTITY_H
#define SAD_SADNET_IDENTITY_IDENTITY_H

#include "sadnet/core/types.h"
#include "sadnet/crypto/crypto.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace sad::net::identity
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::Endpoint;
    using sad::net::NetError;
    using sad::net::NodeId;
    using sad::net::Result;
    using sad::net::crypto::KeyPair;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) طريقة التحقق — نوع المفتاح المستخدم
    // ═════════════════════════════════════════════════════════════════════

    enum class KeyType : uint8_t
    {
        X25519 = 1, // X25519 (تبادل مفاتيح)
        ED25519 = 2 // Ed25519 (توقيع — مستقبلاً)
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) طريقة تحقق (Verification Method)
    // ═════════════════════════════════════════════════════════════════════

    struct VerificationMethod
    {
        std::string id; // مثال: "did:snet:abc123#key-1"
        KeyType type;
        Buffer public_key;
        uint64_t created_at = 0;
        uint64_t expires_at = 0; // 0 = لا ينتهي

        bool is_expired() const
        {
            return expires_at > 0 && now_ms() > expires_at;
        }
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) خدمة مرتبطة بالهوية
    // ═════════════════════════════════════════════════════════════════════

    struct ServiceEndpoint
    {
        std::string id;   // مثال: "did:snet:abc123#messaging"
        std::string type; // مثال: "SNetMessaging"
        Endpoint address;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) وثيقة DID — مستوحاة من W3C DID Core
    //      did:snet:<node_id_hex>
    // ═════════════════════════════════════════════════════════════════════

    struct DIDDocument
    {
        std::string did; // "did:snet:..."
        NodeId node_id;
        std::vector<VerificationMethod> auth_methods;
        std::vector<VerificationMethod> key_agreement;
        std::vector<ServiceEndpoint> services;
        uint64_t created = 0;
        uint64_t updated = 0;
        uint32_t version = 1;

        // (AR) ترميز وتحليل ثنائي
        Buffer serialize() const;
        static DIDDocument deserialize(const uint8_t *data, size_t len);

        // (AR) إنشاء DID URI من معرف العُقدة
        static std::string make_did(const NodeId &id);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ورقة اعتماد قابلة للتحقق (Verifiable Credential)
    // ═════════════════════════════════════════════════════════════════════

    struct Credential
    {
        std::string id;
        std::string type;                          // نوع الاعتماد (مثلاً "عضوية", "مهارة")
        std::string issuer_did;                    // DID المُصدر
        std::string subject_did;                   // DID الموضوع
        std::map<std::string, std::string> claims; // المطالبات (key → value)
        uint64_t issued_at = 0;
        uint64_t expires_at = 0;
        Buffer signature; // توقيع المُصدر

        Buffer serialize() const;
        static Credential deserialize(const uint8_t *data, size_t len);

        bool is_expired() const
        {
            return expires_at > 0 && now_ms() > expires_at;
        }
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير الهوية — إنشاء، تحديث، حل، والتحقق من الهويات
    // ═════════════════════════════════════════════════════════════════════

    class IdentityManager
    {
    public:
        IdentityManager(const KeyPair &identity_keys);
        ~IdentityManager();

        // ─── إدارة DID المحلية ───

        // (AR) إنشاء هوية جديدة
        DIDDocument create_identity();

        // (AR) الحصول على وثيقة DID المحلية
        const DIDDocument &local_document() const;

        // (AR) إضافة خدمة
        void add_service(const std::string &type, const Endpoint &address);

        // (AR) إضافة مفتاح تحقق
        void add_verification_method(const Buffer &public_key,
                                     KeyType type = KeyType::X25519);

        // (AR) تدوير المفتاح الرئيسي (key rotation)
        Result<void> rotate_key(const KeyPair &new_keys);

        // ─── حل وتحقق ───

        // (AR) تسجيل وثيقة DID بعيدة (محلول من DHT)
        void register_remote(const DIDDocument &doc);

        // (AR) حل DID إلى وثيقة
        const DIDDocument *resolve(const std::string &did) const;
        const DIDDocument *resolve(const NodeId &id) const;

        // (AR) التحقق من صحة وثيقة DID
        bool verify_document(const DIDDocument &doc) const;

        // ─── أوراق الاعتماد ───

        // (AR) إصدار ورقة اعتماد
        Credential issue_credential(const std::string &subject_did,
                                    const std::string &type,
                                    const std::map<std::string, std::string> &claims,
                                    uint64_t validity_ms = 0);

        // (AR) التحقق من ورقة اعتماد
        bool verify_credential(const Credential &cred) const;

        // (AR) تخزين ورقة اعتماد مستقبلة
        void store_credential(const Credential &cred);

        // (AR) استرجاع أوراق الاعتماد
        std::vector<Credential> get_credentials(const std::string &subject_did = "") const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::identity

#endif // SAD_SADNET_IDENTITY_IDENTITY_H
