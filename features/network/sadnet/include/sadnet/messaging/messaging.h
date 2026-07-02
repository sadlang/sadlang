// ═══════════════════════════════════════════════════════════════════
// صNet — وحدة الرسائل المشفرة طرف-لطرف (E2EE)
// (AR) نظام رسائل آمن يستخدم Double Ratchet لتشفير E2EE
//      يدعم: محادثات فردية، مجموعات، رسائل غير متصلة، إيصالات
// (EN) End-to-End Encrypted messaging with Double Ratchet// المكون: features/network/sadnet (الرسائل المشفرة E2EE)
// المسار: features/network/sadnet/include/sadnet/messaging/messaging.h// ═══════════════════════════════════════════════════════════════════

#ifndef SAD_SADNET_MESSAGING_MESSAGING_H
#define SAD_SADNET_MESSAGING_MESSAGING_H

#include "sadnet/core/types.h"
#include "sadnet/crypto/crypto.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace sad::net::messaging
{

    // (AR) استيراد الأنواع المطلوبة من النطاق الأساسي
    using sad::net::Buffer;
    using sad::net::NetError;
    using sad::net::NodeId;
    using sad::net::Result;
    using sad::net::crypto::KeyPair;
    using sad::net::crypto::X25519Key;

    // ═════════════════════════════════════════════════════════════════════
    // (AR) أنواع الرسائل
    // ═════════════════════════════════════════════════════════════════════

    enum class MessageType : uint8_t
    {
        TEXT = 1,           // رسالة نصية
        BINARY = 2,         // بيانات ثنائية
        KEY_EXCHANGE = 3,   // تبادل مفاتيح أولي
        ACK = 4,            // إيصال استلام
        READ = 5,           // إيصال قراءة
        GROUP_INVITE = 6,   // دعوة لمجموعة
        GROUP_MESSAGE = 7,  // رسالة مجموعة
        GROUP_KEY_DIST = 8, // توزيع مفتاح المجموعة
        PING = 9,           // فحص حياة
        PONG = 10           // استجابة فحص
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) رسالة مشفرة
    // ═════════════════════════════════════════════════════════════════════

    struct Message
    {
        uint64_t id = 0; // معرف فريد
        MessageType type = MessageType::TEXT;
        NodeId sender;
        NodeId recipient;
        uint64_t timestamp = 0;
        Buffer payload;   // الحمولة (مشفرة أو غير مشفرة حسب السياق)
        Buffer signature; // توقيع (اختياري)

        // (AR) ترميز البيانات الوصفية (بدون الحمولة — تُستخدم كـ AAD)
        Buffer metadata_bytes() const;

        Buffer serialize() const;
        static Message deserialize(const uint8_t *data, size_t len);
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) جلسة محادثة بين قرينين (E2EE)
    //      تستخدم Double Ratchet من وحدة crypto
    // ═════════════════════════════════════════════════════════════════════

    class ChatSession
    {
    public:
        // (AR) إنشاء جلسة كمُبادر (initiator)
        static ChatSession create_initiator(const NodeId &self,
                                            const NodeId &peer,
                                            const KeyPair &self_keys,
                                            const Buffer &peer_public_key);

        // (AR) إنشاء جلسة كمُستقبل (responder)
        static ChatSession create_responder(const NodeId &self,
                                            const NodeId &peer,
                                            const KeyPair &self_keys,
                                            const Buffer &peer_public_key);

        ChatSession();
        ~ChatSession();
        ChatSession(ChatSession &&other) noexcept;
        ChatSession &operator=(ChatSession &&other) noexcept;

        // (AR) تشفير رسالة
        Result<Message> encrypt_message(const Buffer &plaintext,
                                        MessageType type = MessageType::TEXT);

        // (AR) فك تشفير رسالة واردة
        Result<Buffer> decrypt_message(const Message &msg);

        // (AR) هل الجلسة مُهيّأة؟
        bool is_established() const;

        // (AR) معرف النظير
        const NodeId &peer_id() const;

        // (AR) عدد الرسائل المرسلة/المستقبلة
        uint64_t messages_sent() const;
        uint64_t messages_received() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مجموعة مشفرة
    //      تستخدم مفتاح مجموعة مشترك + إعادة توزيع عند تغيير الأعضاء
    // ═════════════════════════════════════════════════════════════════════

    struct GroupInfo
    {
        NodeId group_id;
        std::string name;
        NodeId creator;
        std::vector<NodeId> members;
        uint64_t created_at = 0;
        uint32_t epoch = 0; // رقم الحقبة (يزداد عند تغيير الأعضاء)
    };

    class GroupSession
    {
    public:
        GroupSession(const NodeId &self_id, const GroupInfo &info);
        ~GroupSession();
        GroupSession(GroupSession &&other) noexcept;
        GroupSession &operator=(GroupSession &&other) noexcept;

        // (AR) تشفير رسالة للمجموعة
        Result<Message> encrypt_group_message(const Buffer &plaintext);

        // (AR) فك تشفير رسالة مجموعة
        Result<Buffer> decrypt_group_message(const Message &msg);

        // (AR) إضافة عضو (يُجدد مفتاح المجموعة)
        void add_member(const NodeId &member);

        // (AR) إزالة عضو (يُجدد مفتاح المجموعة)
        void remove_member(const NodeId &member);

        // (AR) الحصول على رسائل توزيع المفتاح الجديد لكل عضو
        std::vector<Message> get_key_distribution_messages(
            const std::function<ChatSession *(const NodeId &)> &get_session);

        const GroupInfo &info() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير الرسائل — واجهة عالية المستوى
    // ═════════════════════════════════════════════════════════════════════

    using MessageCallback = std::function<void(const Message &msg, const Buffer &plaintext)>;
    using AckCallback = std::function<void(uint64_t msg_id)>;

    class MessagingManager
    {
    public:
        MessagingManager(const NodeId &self_id, const KeyPair &identity_keys);
        ~MessagingManager();

        // (AR) إنشاء جلسة محادثة مع قرين
        Result<void> create_session(const NodeId &peer,
                                    const Buffer &peer_public_key,
                                    bool is_initiator = true);

        // (AR) إرسال رسالة نصية
        Result<Message> send_text(const NodeId &to, const std::string &text);

        // (AR) إرسال بيانات ثنائية
        Result<Message> send_binary(const NodeId &to, const Buffer &data);

        // (AR) معالجة رسالة واردة (فك تشفير + إرسال إيصال)
        Result<Buffer> process_incoming(const Message &msg);

        // (AR) إنشاء مجموعة
        Result<GroupInfo> create_group(const std::string &name,
                                       const std::vector<NodeId> &members);

        // (AR) إرسال رسالة مجموعة
        Result<Message> send_group_text(const NodeId &group_id, const std::string &text);

        // (AR) معالجات الأحداث
        void on_message(MessageCallback callback);
        void on_ack(AckCallback callback);

        // (AR) الحصول على جلسة قرين (للاستخدام الداخلي)
        ChatSession *get_session(const NodeId &peer);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace sad::net::messaging

#endif // SAD_SADNET_MESSAGING_MESSAGING_H
