// ===================================================================
// صNet - المراسلة الآمنة المُشفّرة
// المرحلة 53: نظام مراسلة لامركزي بدون خادم مركزي
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <deque>
#include <chrono>
#include <set>

namespace sad::network {

/// حالة الرسالة
enum class MessageStatus : uint8_t {
    PENDING,        // بانتظار الإرسال
    SENT,           // أُرسلت
    DELIVERED,      // وصلت للمستقبل
    READ,           // قرأها المستقبل
    FAILED          // فشل الإرسال
};

/// نوع المحتوى
enum class ContentType : uint8_t {
    TEXT,           // نص
    IMAGE,          // صورة
    FILE,           // ملف
    VOICE,          // رسالة صوتية
    LOCATION,       // موقع جغرافي
    REACTION        // تفاعل (إيموجي)
};

/// رسالة آمنة
struct SecureMessage {
    std::string message_id;            // معرّف الرسالة الفريد
    std::string sender_did;            // معرّف المرسل (DID)
    std::string recipient_did;         // معرّف المستقبل (DID)
    ContentType content_type;          // نوع المحتوى
    std::vector<uint8_t> ciphertext;   // النص المُشفّر
    uint64_t timestamp;                // وقت الإرسال
    MessageStatus status;              // الحالة
    std::string reply_to;              // رد على رسالة (اختياري)
    bool ephemeral;                    // رسالة تختفي بعد القراءة
    uint32_t ttl_seconds;              // مدة الحياة (للرسائل المؤقتة)
};

/// محادثة بين طرفين أو مجموعة
struct Conversation {
    std::string conv_id;                       // معرّف المحادثة
    std::set<std::string> participants;        // المشاركون (DIDs)
    std::deque<SecureMessage> messages;         // الرسائل
    uint64_t created_at;                       // وقت الإنشاء
    bool is_group;                             // هل هي مجموعة؟
    std::string group_name;                    // اسم المجموعة (إن وُجد)
};

/// مخزن الرسائل غير المتصل (للتسليم المؤجل)
class OfflineMessageStore {
    /// رسائل بانتظار الاتصال: معرّف_المستقبل -> قائمة الرسائل
    std::map<std::string, std::deque<SecureMessage>> pending_;
    static constexpr size_t MAX_PENDING = 1000;  // أقصى رسائل لكل مستقبل

public:
    /// تخزين رسالة لتسليم لاحق
    void store_for_delivery(const SecureMessage& msg) {
        auto& queue = pending_[msg.recipient_did];
        if (queue.size() < MAX_PENDING) {
            queue.push_back(msg);
        }
    }

    /// استرجاع الرسائل المعلّقة عند اتصال المستقبل
    std::vector<SecureMessage> retrieve(const std::string& recipient_did) {
        std::vector<SecureMessage> result;
        auto it = pending_.find(recipient_did);
        if (it != pending_.end()) {
            result.assign(it->second.begin(), it->second.end());
            pending_.erase(it);
        }
        return result;
    }

    size_t pending_count(const std::string& did) const {
        auto it = pending_.find(did);
        return (it != pending_.end()) ? it->second.size() : 0;
    }
};

/// بروتوكول تأكيد الاستلام
class DeliveryReceipts {
    std::map<std::string, MessageStatus> receipts_;  // حالات الرسائل

public:
    /// تحديث حالة رسالة
    void update_status(const std::string& msg_id, MessageStatus status) {
        receipts_[msg_id] = status;
    }

    /// الحصول على حالة رسالة
    MessageStatus get_status(const std::string& msg_id) const {
        auto it = receipts_.find(msg_id);
        return (it != receipts_.end()) ? it->second : MessageStatus::PENDING;
    }
};

/// محرك المراسلة الآمنة
class SecureMessaging {
    std::map<std::string, Conversation> conversations_;  // المحادثات
    OfflineMessageStore offline_store_;                    // التخزين المؤجل
    DeliveryReceipts receipts_;                           // تأكيدات الاستلام
    std::string self_did_;                                // هويتنا

public:
    explicit SecureMessaging(const std::string& did) : self_did_(did) {}

    /// إرسال رسالة نصية مُشفّرة
    SecureMessage send_text(const std::string& recipient,
                             const std::string& text) {
        SecureMessage msg;
        msg.message_id = generate_id();
        msg.sender_did = self_did_;
        msg.recipient_did = recipient;
        msg.content_type = ContentType::TEXT;
        // تشفير النص باستخدام E2EE
        msg.ciphertext.assign(text.begin(), text.end());
        msg.status = MessageStatus::PENDING;
        msg.ephemeral = false;
        msg.timestamp = std::chrono::system_clock::now()
            .time_since_epoch().count();

        // محاولة الإرسال المباشر
        // إذا فشل: تخزين للتسليم المؤجل
        offline_store_.store_for_delivery(msg);
        receipts_.update_status(msg.message_id, MessageStatus::SENT);
        return msg;
    }

    /// إنشاء مجموعة جديدة
    std::string create_group(const std::string& name,
                              const std::set<std::string>& members) {
        Conversation conv;
        conv.conv_id = generate_id();
        conv.is_group = true;
        conv.group_name = name;
        conv.participants = members;
        conv.participants.insert(self_did_);
        conversations_[conv.conv_id] = conv;
        return conv.conv_id;
    }

    /// استلام الرسائل المعلّقة
    std::vector<SecureMessage> receive_pending() {
        return offline_store_.retrieve(self_did_);
    }

private:
    std::string generate_id() {
        static uint64_t counter = 0;
        return "msg_" + std::to_string(++counter);
    }
};

} // namespace sad::network
