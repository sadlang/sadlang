// ═══════════════════════════════════════════════════════════════════
// صNet — تنفيذ الرسائل المشفرة طرف-لطرف
// (AR) جلسة محادثة E2EE (Double Ratchet)، مجموعات مشفرة، مدير رسائل
// (EN) E2EE messaging: chat sessions, group sessions, messaging manager
// المكون: features/network/sadnet (الرسائل المشفرة E2EE)
// المسار: features/network/sadnet/src/messaging/messaging.cpp
// ═══════════════════════════════════════════════════════════════════

#include "sadnet/messaging/messaging.h"
#include <algorithm>
#include <cstring>
#include <map>
#include <mutex>

namespace sad::net::messaging
{

    // استخدام دوال وأنواع التشفير المطلوبة
    using crypto::aes_gcm_decrypt;
    using crypto::aes_gcm_encrypt;
    using crypto::DoubleRatchetSession;
    using crypto::hmac_sha256;
    using crypto::sha256;
    using crypto::x25519_shared_secret;

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

    // ═════════════════════════════════════════════════════════════════════
    // (AR) ترميز/فك ترميز الرسائل
    //      [id:8][type:1][sender:32][recipient:32][timestamp:8]
    //      [payload_len:4][payload:N][sig_len:2][sig:M]
    // ═════════════════════════════════════════════════════════════════════

    Buffer Message::metadata_bytes() const
    {
        Buffer buf(8 + 1 + NODE_ID_SIZE * 2 + 8);
        uint8_t *p = buf.data();
        w64(p, id);
        p[8] = (uint8_t)type;
        std::memcpy(p + 9, sender.data.data(), NODE_ID_SIZE);
        std::memcpy(p + 9 + NODE_ID_SIZE, recipient.data.data(), NODE_ID_SIZE);
        w64(p + 9 + NODE_ID_SIZE * 2, timestamp);
        return buf;
    }

    Buffer Message::serialize() const
    {
        size_t total = 8 + 1 + NODE_ID_SIZE * 2 + 8 + 4 + payload.size() + 2 + signature.size();
        Buffer buf(total);
        uint8_t *p = buf.data();

        w64(p, id);
        p += 8;
        *p++ = (uint8_t)type;
        std::memcpy(p, sender.data.data(), NODE_ID_SIZE);
        p += NODE_ID_SIZE;
        std::memcpy(p, recipient.data.data(), NODE_ID_SIZE);
        p += NODE_ID_SIZE;
        w64(p, timestamp);
        p += 8;
        w32(p, (uint32_t)payload.size());
        p += 4;
        std::memcpy(p, payload.data(), payload.size());
        p += payload.size();
        w16(p, (uint16_t)signature.size());
        p += 2;
        if (!signature.empty())
            std::memcpy(p, signature.data(), signature.size());

        return buf;
    }

    Message Message::deserialize(const uint8_t *data, size_t len)
    {
        Message msg;
        size_t min_size = 8 + 1 + NODE_ID_SIZE * 2 + 8 + 4 + 2;
        if (len < min_size)
            return msg;

        const uint8_t *p = data;
        msg.id = r64(p);
        p += 8;
        msg.type = (MessageType)*p++;
        std::memcpy(msg.sender.data.data(), p, NODE_ID_SIZE);
        p += NODE_ID_SIZE;
        std::memcpy(msg.recipient.data.data(), p, NODE_ID_SIZE);
        p += NODE_ID_SIZE;
        msg.timestamp = r64(p);
        p += 8;

        uint32_t pl = r32(p);
        p += 4;
        if (p + pl + 2 > data + len)
            return msg;
        msg.payload = Buffer(p, pl);
        p += pl;

        uint16_t sl = r16(p);
        p += 2;
        if (p + sl <= data + len)
            msg.signature = Buffer(p, sl);

        return msg;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) جلسة المحادثة — E2EE باستخدام Double Ratchet
    // ═════════════════════════════════════════════════════════════════════

    struct ChatSession::Impl
    {
        NodeId self_id;
        NodeId peer;
        DoubleRatchetSession ratchet;
        bool established = false;
        uint64_t msg_counter = 0;
        uint64_t msgs_sent = 0;
        uint64_t msgs_received = 0;
    };

    ChatSession::ChatSession() : impl_(std::make_unique<Impl>()) {}
    ChatSession::~ChatSession() = default;
    ChatSession::ChatSession(ChatSession &&o) noexcept = default;
    ChatSession &ChatSession::operator=(ChatSession &&o) noexcept = default;

    ChatSession ChatSession::create_initiator(const NodeId &self,
                                              const NodeId &peer,
                                              const KeyPair &self_keys,
                                              const Buffer &peer_public_key)
    {
        ChatSession session;
        session.impl_->self_id = self;
        session.impl_->peer = peer;
        // (AR) حساب السر المشترك من مفتاحنا الخاص ومفتاح النظير العام
        X25519Key peer_pub{};
        std::memcpy(peer_pub.data(), peer_public_key.data(),
                    std::min(peer_public_key.size(), (size_t)X25519_KEY_SIZE));
        auto shared = x25519_shared_secret(self_keys.private_key, peer_pub);
        session.impl_->ratchet.init_sender(shared, peer_pub);
        session.impl_->established = true;
        return session;
    }

    ChatSession ChatSession::create_responder(const NodeId &self,
                                              const NodeId &peer,
                                              const KeyPair &self_keys,
                                              const Buffer &peer_public_key)
    {
        ChatSession session;
        session.impl_->self_id = self;
        session.impl_->peer = peer;
        // (AR) حساب السر المشترك ثم تهيئة كمستقبل مع زوج مفاتيحنا
        X25519Key peer_pub{};
        std::memcpy(peer_pub.data(), peer_public_key.data(),
                    std::min(peer_public_key.size(), (size_t)X25519_KEY_SIZE));
        auto shared = x25519_shared_secret(self_keys.private_key, peer_pub);
        session.impl_->ratchet.init_receiver(shared, self_keys);
        session.impl_->established = true;
        return session;
    }

    Result<Message> ChatSession::encrypt_message(const Buffer &plaintext,
                                                 MessageType type)
    {
        if (!impl_->established)
            return Result<Message>::fail(NetError::PROTOCOL_ERROR);

        Message msg;
        msg.id = ++impl_->msg_counter;
        msg.type = type;
        msg.sender = impl_->self_id;
        msg.recipient = impl_->peer;
        msg.timestamp = now_ms();

        // البيانات الوصفية كـ AAD
        auto aad = msg.metadata_bytes();
        // (AR) تشفير: الرسالة الأصلية مضافاً إليها AAD
        Buffer combined;
        combined.append(aad.data(), aad.size());
        combined.append(plaintext.data(), plaintext.size());
        auto encrypted = impl_->ratchet.encrypt(combined);
        // تجميع الحمولة من الهيدر والنص المشفر
        msg.payload.append(encrypted.header.data(), encrypted.header.size());
        msg.payload.append(encrypted.ciphertext.data(), encrypted.ciphertext.size());

        impl_->msgs_sent++;
        return Result<Message>::success(std::move(msg));
    }

    Result<Buffer> ChatSession::decrypt_message(const Message &msg)
    {
        if (!impl_->established)
            return Result<Buffer>::fail(NetError::PROTOCOL_ERROR);

        auto aad = msg.metadata_bytes();
        // (AR) فك الترميز: نعيد بناء EncryptedPacket من payload
        DoubleRatchetSession::EncryptedPacket pkt;
        if (msg.payload.size() >= 40)
        {
            pkt.header = Buffer(msg.payload.data(), 40);
            pkt.ciphertext = Buffer(msg.payload.data() + 40, msg.payload.size() - 40);
        }
        auto plaintext = impl_->ratchet.decrypt(pkt);

        if (plaintext.empty())
            return Result<Buffer>::fail(NetError::PROTOCOL_ERROR);

        impl_->msgs_received++;
        return Result<Buffer>::success(std::move(plaintext));
    }

    bool ChatSession::is_established() const { return impl_->established; }
    const NodeId &ChatSession::peer_id() const { return impl_->peer; }
    uint64_t ChatSession::messages_sent() const { return impl_->msgs_sent; }
    uint64_t ChatSession::messages_received() const { return impl_->msgs_received; }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) جلسة المجموعة — تشفير بمفتاح مشترك
    // ═════════════════════════════════════════════════════════════════════

    struct GroupSession::Impl
    {
        NodeId self_id;
        GroupInfo group_info;
        Buffer group_key; // مفتاح المجموعة الحالي (AES-256)
        uint64_t msg_counter = 0;

        void rotate_key()
        {
            // اشتقاق مفتاح جديد من المفتاح الحالي + الحقبة
            Buffer epoch_buf(4);
            w32(epoch_buf.data(), group_info.epoch);
            auto hash = hmac_sha256(group_key.data(), group_key.size(),
                                    epoch_buf.data(), epoch_buf.size());
            group_key = Buffer(hash.data(), hash.size());
            // قص إلى 32 بايت (AES-256)
            group_key.resize(AES_KEY_SIZE);
        }
    };

    GroupSession::GroupSession(const NodeId &self_id, const GroupInfo &info)
        : impl_(std::make_unique<Impl>())
    {
        impl_->self_id = self_id;
        impl_->group_info = info;
        // إنشاء مفتاح المجموعة الأولي من معرف المجموعة
        auto hash = sha256(info.group_id.data.data(), NODE_ID_SIZE);
        impl_->group_key = Buffer(hash.data(), hash.size());
        impl_->group_key.resize(AES_KEY_SIZE);
    }

    GroupSession::~GroupSession() = default;
    GroupSession::GroupSession(GroupSession &&o) noexcept = default;
    GroupSession &GroupSession::operator=(GroupSession &&o) noexcept = default;

    Result<Message> GroupSession::encrypt_group_message(const Buffer &plaintext)
    {
        Message msg;
        msg.id = ++impl_->msg_counter;
        msg.type = MessageType::GROUP_MESSAGE;
        msg.sender = impl_->self_id;
        msg.recipient = impl_->group_info.group_id;
        msg.timestamp = now_ms();

        auto aad = msg.metadata_bytes();
        auto encrypted = aes_gcm_encrypt(impl_->group_key.data(),
                                         plaintext.data(), plaintext.size(),
                                         aad.data(), aad.size());

        // [nonce:12][ciphertext:N][tag:16]
        Buffer payload(encrypted.nonce.size() + encrypted.ciphertext.size() + encrypted.tag.size());
        uint8_t *p = payload.data();
        std::memcpy(p, encrypted.nonce.data(), encrypted.nonce.size());
        p += encrypted.nonce.size();
        std::memcpy(p, encrypted.ciphertext.data(), encrypted.ciphertext.size());
        p += encrypted.ciphertext.size();
        std::memcpy(p, encrypted.tag.data(), encrypted.tag.size());

        msg.payload = std::move(payload);
        return Result<Message>::success(std::move(msg));
    }

    Result<Buffer> GroupSession::decrypt_group_message(const Message &msg)
    {
        if (msg.payload.size() < AES_NONCE_SIZE + AES_TAG_SIZE)
            return Result<Buffer>::fail(NetError::PROTOCOL_ERROR);

        auto aad = msg.metadata_bytes();

        Buffer nonce(msg.payload.data(), AES_NONCE_SIZE);
        size_t ct_len = msg.payload.size() - AES_NONCE_SIZE - AES_TAG_SIZE;
        Buffer ciphertext(msg.payload.data() + AES_NONCE_SIZE, ct_len);
        Buffer tag(msg.payload.data() + AES_NONCE_SIZE + ct_len, AES_TAG_SIZE);

        auto plaintext = aes_gcm_decrypt(impl_->group_key.data(),
                                         ciphertext.data(), ct_len,
                                         nonce.data(), tag.data(),
                                         aad.data(), aad.size());
        if (plaintext.empty())
            return Result<Buffer>::fail(NetError::PROTOCOL_ERROR);

        return Result<Buffer>::success(std::move(plaintext));
    }

    void GroupSession::add_member(const NodeId &member)
    {
        impl_->group_info.members.push_back(member);
        impl_->group_info.epoch++;
        impl_->rotate_key();
    }

    void GroupSession::remove_member(const NodeId &member)
    {
        auto &members = impl_->group_info.members;
        members.erase(std::remove_if(members.begin(), members.end(),
                                     [&member](const NodeId &m)
                                     { return m == member; }),
                      members.end());
        impl_->group_info.epoch++;
        impl_->rotate_key();
    }

    std::vector<Message> GroupSession::get_key_distribution_messages(
        const std::function<ChatSession *(const NodeId &)> &get_session)
    {
        std::vector<Message> msgs;

        for (const auto &member : impl_->group_info.members)
        {
            if (member == impl_->self_id)
                continue;

            auto *session = get_session(member);
            if (!session)
                continue;

            // تشفير مفتاح المجموعة الجديد عبر الجلسة الفردية
            auto result = session->encrypt_message(impl_->group_key,
                                                   MessageType::GROUP_KEY_DIST);
            if (result.ok())
                msgs.push_back(result.value);
        }

        return msgs;
    }

    const GroupInfo &GroupSession::info() const { return impl_->group_info; }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) مدير الرسائل — واجهة عالية المستوى
    // ═════════════════════════════════════════════════════════════════════

    struct MessagingManager::Impl
    {
        NodeId self_id;
        KeyPair identity_keys;

        std::map<NodeId, ChatSession> sessions;
        std::map<NodeId, GroupSession> groups;
        mutable std::mutex mutex;

        uint64_t msg_id_counter = 0;

        MessageCallback msg_callback;
        AckCallback ack_callback;

        Impl(const NodeId &id, const KeyPair &keys)
            : self_id(id), identity_keys(keys) {}
    };

    MessagingManager::MessagingManager(const NodeId &self_id,
                                       const KeyPair &identity_keys)
        : impl_(std::make_unique<Impl>(self_id, identity_keys)) {}

    MessagingManager::~MessagingManager() = default;

    Result<void> MessagingManager::create_session(const NodeId &peer,
                                                  const Buffer &peer_public_key,
                                                  bool is_initiator)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);

        if (is_initiator)
        {
            impl_->sessions.emplace(peer,
                                    ChatSession::create_initiator(impl_->self_id, peer,
                                                                  impl_->identity_keys, peer_public_key));
        }
        else
        {
            impl_->sessions.emplace(peer,
                                    ChatSession::create_responder(impl_->self_id, peer,
                                                                  impl_->identity_keys, peer_public_key));
        }

        return Result<void>::success();
    }

    Result<Message> MessagingManager::send_text(const NodeId &to,
                                                const std::string &text)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        auto it = impl_->sessions.find(to);
        if (it == impl_->sessions.end())
            return Result<Message>::fail(NetError::NOT_FOUND);

        Buffer plaintext(reinterpret_cast<const uint8_t *>(text.data()), text.size());
        return it->second.encrypt_message(plaintext, MessageType::TEXT);
    }

    Result<Message> MessagingManager::send_binary(const NodeId &to,
                                                  const Buffer &data)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        auto it = impl_->sessions.find(to);
        if (it == impl_->sessions.end())
            return Result<Message>::fail(NetError::NOT_FOUND);

        return it->second.encrypt_message(data, MessageType::BINARY);
    }

    Result<Buffer> MessagingManager::process_incoming(const Message &msg)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);

        if (msg.type == MessageType::ACK || msg.type == MessageType::READ)
        {
            if (impl_->ack_callback)
                impl_->ack_callback(msg.id);
            return Result<Buffer>::success(Buffer());
        }

        if (msg.type == MessageType::GROUP_MESSAGE)
        {
            auto git = impl_->groups.find(msg.recipient);
            if (git == impl_->groups.end())
                return Result<Buffer>::fail(NetError::NOT_FOUND);

            auto plaintext = git->second.decrypt_group_message(msg);
            if (plaintext.ok() && impl_->msg_callback)
                impl_->msg_callback(msg, plaintext.value);
            return plaintext;
        }

        auto it = impl_->sessions.find(msg.sender);
        if (it == impl_->sessions.end())
            return Result<Buffer>::fail(NetError::NOT_FOUND);

        auto plaintext = it->second.decrypt_message(msg);
        if (plaintext.ok() && impl_->msg_callback)
            impl_->msg_callback(msg, plaintext.value);
        return plaintext;
    }

    Result<GroupInfo> MessagingManager::create_group(const std::string &name,
                                                     const std::vector<NodeId> &members)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);

        GroupInfo info;
        // معرف المجموعة = SHA-256 من الاسم + الوقت
        Buffer seed;
        seed.append(reinterpret_cast<const uint8_t *>(name.data()), name.size());
        uint64_t t = now_ms();
        for (int i = 7; i >= 0; i--)
            seed.push_back((uint8_t)(t >> (i * 8)));
        auto hash = sha256(seed);
        std::memcpy(info.group_id.data.data(), hash.data(), NODE_ID_SIZE);

        info.name = name;
        info.creator = impl_->self_id;
        info.members = members;
        info.members.push_back(impl_->self_id);
        info.created_at = now_ms();
        info.epoch = 0;

        impl_->groups.emplace(info.group_id,
                              GroupSession(impl_->self_id, info));

        return Result<GroupInfo>::success(std::move(info));
    }

    Result<Message> MessagingManager::send_group_text(const NodeId &group_id,
                                                      const std::string &text)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        auto it = impl_->groups.find(group_id);
        if (it == impl_->groups.end())
            return Result<Message>::fail(NetError::NOT_FOUND);

        Buffer plaintext(reinterpret_cast<const uint8_t *>(text.data()), text.size());
        return it->second.encrypt_group_message(plaintext);
    }

    void MessagingManager::on_message(MessageCallback cb)
    {
        impl_->msg_callback = std::move(cb);
    }
    void MessagingManager::on_ack(AckCallback cb)
    {
        impl_->ack_callback = std::move(cb);
    }

    ChatSession *MessagingManager::get_session(const NodeId &peer)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        auto it = impl_->sessions.find(peer);
        if (it != impl_->sessions.end())
            return &it->second;
        return nullptr;
    }

} // namespace sad::net::messaging
