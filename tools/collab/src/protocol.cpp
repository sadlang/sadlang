/**
 * ==========================================================
 * ملف: protocol.cpp
 * الوصف: بروتوكول التعاون المباشر
 * المهمة: T306 - شراكة ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🤝 ما هو بروتوكول التعاون؟
 * -------------------------
 * قواعد تتبعها الأجهزة للتواصل مع بعضها أثناء
 * العمل المشترك على نفس الكود.
 * 
 * 📝 مثال بسيط:
 * -------------
 * 1. أحمد يفتح جلسة: "ص شراكة ابدأ"
 * 2. سارة تنضم: "ص شراكة انضم ABC123"
 * 3. كلاهما يرى تعديلات الآخر فوراً!
 * 
 * 🔄 نوع الرسائل:
 * ---------------
 * - JOIN: انضمام لجلسة
 * - LEAVE: مغادرة جلسة
 * - EDIT: تعديل في الكود
 * - CURSOR: موقع المؤشر
 * - CHAT: رسالة نصية
 * - VOICE: بيانات صوتية
 */

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <sstream>
#include <cstring>
#include <chrono>
#include <random>

namespace sad {
namespace collab {

// ==========================================================
// 📌 أنواع الرسائل
// ==========================================================

/**
 * 📬 نوع الرسالة
 */
enum class MessageType : uint8_t {
    // إدارة الجلسة
    JOIN_REQUEST    = 0x01,     // طلب انضمام
    JOIN_RESPONSE   = 0x02,     // رد على الانضمام
    LEAVE           = 0x03,     // مغادرة
    HEARTBEAT       = 0x04,     // نبض القلب (أنا موجود)
    
    // تحرير الكود
    TEXT_INSERT     = 0x10,     // إدراج نص
    TEXT_DELETE     = 0x11,     // حذف نص
    TEXT_REPLACE    = 0x12,     // استبدال نص
    TEXT_SYNC       = 0x13,     // مزامنة كاملة
    
    // المؤشر
    CURSOR_MOVE     = 0x20,     // تحريك المؤشر
    SELECTION       = 0x21,     // تحديد نص
    
    // التواصل
    CHAT_MESSAGE    = 0x30,     // رسالة دردشة
    VOICE_DATA      = 0x31,     // بيانات صوتية
    
    // الاختبارات
    TEST_REQUEST    = 0x40,     // طلب تشغيل اختبار
    TEST_RESULT     = 0x41,     // نتيجة اختبار
    
    // النظام
    ERROR           = 0xF0,     // خطأ
    ACK             = 0xF1      // تأكيد استلام
};

/**
 * اسم نوع الرسالة
 */
const char* messageTypeName(MessageType type) {
    switch (type) {
        case MessageType::JOIN_REQUEST: return "طلب انضمام";
        case MessageType::JOIN_RESPONSE: return "رد انضمام";
        case MessageType::LEAVE: return "مغادرة";
        case MessageType::HEARTBEAT: return "نبض";
        case MessageType::TEXT_INSERT: return "إدراج";
        case MessageType::TEXT_DELETE: return "حذف";
        case MessageType::TEXT_REPLACE: return "استبدال";
        case MessageType::TEXT_SYNC: return "مزامنة";
        case MessageType::CURSOR_MOVE: return "تحريك مؤشر";
        case MessageType::SELECTION: return "تحديد";
        case MessageType::CHAT_MESSAGE: return "دردشة";
        case MessageType::VOICE_DATA: return "صوت";
        case MessageType::TEST_REQUEST: return "طلب اختبار";
        case MessageType::TEST_RESULT: return "نتيجة اختبار";
        case MessageType::ERROR: return "خطأ";
        case MessageType::ACK: return "تأكيد";
        default: return "غير معروف";
    }
}

// ==========================================================
// 📌 معرّفات
// ==========================================================

/**
 * 🆔 معرّف الجلسة
 */
struct SessionId {
    uint8_t bytes[6];  // 6 bytes = 12 hex chars
    
    /**
     * توليد معرّف عشوائي
     */
    static SessionId generate() {
        SessionId id;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        
        for (int i = 0; i < 6; ++i) {
            id.bytes[i] = static_cast<uint8_t>(dis(gen));
        }
        return id;
    }
    
    /**
     * تحويل لـ string
     */
    std::string toString() const {
        const char* hex = "0123456789ABCDEF";
        std::string result;
        for (int i = 0; i < 6; ++i) {
            result += hex[bytes[i] >> 4];
            result += hex[bytes[i] & 0x0F];
        }
        return result;
    }
    
    /**
     * من string
     */
    static SessionId fromString(const std::string& str) {
        SessionId id;
        memset(id.bytes, 0, 6);
        
        for (size_t i = 0; i < str.length() && i/2 < 6; i += 2) {
            uint8_t high = 0, low = 0;
            if (str[i] >= '0' && str[i] <= '9') high = str[i] - '0';
            else if (str[i] >= 'A' && str[i] <= 'F') high = str[i] - 'A' + 10;
            else if (str[i] >= 'a' && str[i] <= 'f') high = str[i] - 'a' + 10;
            
            if (i+1 < str.length()) {
                if (str[i+1] >= '0' && str[i+1] <= '9') low = str[i+1] - '0';
                else if (str[i+1] >= 'A' && str[i+1] <= 'F') low = str[i+1] - 'A' + 10;
                else if (str[i+1] >= 'a' && str[i+1] <= 'f') low = str[i+1] - 'a' + 10;
            }
            
            id.bytes[i/2] = (high << 4) | low;
        }
        return id;
    }
    
    bool operator==(const SessionId& other) const {
        return memcmp(bytes, other.bytes, 6) == 0;
    }
};

/**
 * 🆔 معرّف المستخدم
 */
struct UserId {
    uint32_t id;
    
    static UserId generate() {
        UserId uid;
        std::random_device rd;
        uid.id = rd();
        return uid;
    }
    
    bool operator==(const UserId& other) const {
        return id == other.id;
    }
    
    bool operator<(const UserId& other) const {
        return id < other.id;
    }
};

// ==========================================================
// 📌 رأس الرسالة
// ==========================================================

/**
 * 📋 رأس الرسالة
 */
struct MessageHeader {
    uint8_t magic[4] = {'S', 'C', 'L', 'B'};  // Sad CoLlaB
    uint8_t version = 1;
    MessageType type;
    uint32_t length;         // طول البيانات
    uint32_t sequence;       // رقم تسلسلي
    UserId sender;           // المُرسل
    uint64_t timestamp;      // الوقت
    
    /**
     * الحجم الثابت للرأس
     */
    static constexpr size_t SIZE = 4 + 1 + 1 + 4 + 4 + 4 + 8; // 26 bytes
    
    /**
     * تحويل لـ bytes
     */
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> data(SIZE);
        size_t offset = 0;
        
        // Magic
        memcpy(&data[offset], magic, 4); offset += 4;
        
        // Version
        data[offset++] = version;
        
        // Type
        data[offset++] = static_cast<uint8_t>(type);
        
        // Length (little-endian)
        data[offset++] = length & 0xFF;
        data[offset++] = (length >> 8) & 0xFF;
        data[offset++] = (length >> 16) & 0xFF;
        data[offset++] = (length >> 24) & 0xFF;
        
        // Sequence
        data[offset++] = sequence & 0xFF;
        data[offset++] = (sequence >> 8) & 0xFF;
        data[offset++] = (sequence >> 16) & 0xFF;
        data[offset++] = (sequence >> 24) & 0xFF;
        
        // Sender
        data[offset++] = sender.id & 0xFF;
        data[offset++] = (sender.id >> 8) & 0xFF;
        data[offset++] = (sender.id >> 16) & 0xFF;
        data[offset++] = (sender.id >> 24) & 0xFF;
        
        // Timestamp
        for (int i = 0; i < 8; ++i) {
            data[offset++] = (timestamp >> (i * 8)) & 0xFF;
        }
        
        return data;
    }
    
    /**
     * من bytes
     */
    static MessageHeader fromBytes(const uint8_t* data) {
        MessageHeader header;
        size_t offset = 0;
        
        memcpy(header.magic, &data[offset], 4); offset += 4;
        header.version = data[offset++];
        header.type = static_cast<MessageType>(data[offset++]);
        
        header.length = data[offset] | (data[offset+1] << 8) |
                       (data[offset+2] << 16) | (data[offset+3] << 24);
        offset += 4;
        
        header.sequence = data[offset] | (data[offset+1] << 8) |
                         (data[offset+2] << 16) | (data[offset+3] << 24);
        offset += 4;
        
        header.sender.id = data[offset] | (data[offset+1] << 8) |
                          (data[offset+2] << 16) | (data[offset+3] << 24);
        offset += 4;
        
        header.timestamp = 0;
        for (int i = 0; i < 8; ++i) {
            header.timestamp |= static_cast<uint64_t>(data[offset++]) << (i * 8);
        }
        
        return header;
    }
    
    /**
     * التحقق من الصحة
     */
    bool isValid() const {
        return magic[0] == 'S' && magic[1] == 'C' &&
               magic[2] == 'L' && magic[3] == 'B' &&
               version == 1;
    }
};

// ==========================================================
// 📌 حمولات الرسائل
// ==========================================================

/**
 * 🚪 طلب انضمام
 */
struct JoinRequest {
    SessionId sessionId;
    std::string username;
    std::string color;      // لون المؤشر
    
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> data;
        
        // Session ID
        data.insert(data.end(), sessionId.bytes, sessionId.bytes + 6);
        
        // Username (length-prefixed)
        uint16_t len = static_cast<uint16_t>(username.length());
        data.push_back(len & 0xFF);
        data.push_back((len >> 8) & 0xFF);
        data.insert(data.end(), username.begin(), username.end());
        
        // Color
        len = static_cast<uint16_t>(color.length());
        data.push_back(len & 0xFF);
        data.push_back((len >> 8) & 0xFF);
        data.insert(data.end(), color.begin(), color.end());
        
        return data;
    }
};

/**
 * ✅ رد الانضمام
 */
struct JoinResponse {
    bool accepted;
    std::string reason;
    std::string documentContent;    // المحتوى الحالي
    std::vector<UserId> participants;
    
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> data;
        
        data.push_back(accepted ? 1 : 0);
        
        uint16_t len = static_cast<uint16_t>(reason.length());
        data.push_back(len & 0xFF);
        data.push_back((len >> 8) & 0xFF);
        data.insert(data.end(), reason.begin(), reason.end());
        
        // Document content
        uint32_t docLen = static_cast<uint32_t>(documentContent.length());
        data.push_back(docLen & 0xFF);
        data.push_back((docLen >> 8) & 0xFF);
        data.push_back((docLen >> 16) & 0xFF);
        data.push_back((docLen >> 24) & 0xFF);
        data.insert(data.end(), documentContent.begin(), documentContent.end());
        
        // Participants
        data.push_back(static_cast<uint8_t>(participants.size()));
        for (const auto& p : participants) {
            data.push_back(p.id & 0xFF);
            data.push_back((p.id >> 8) & 0xFF);
            data.push_back((p.id >> 16) & 0xFF);
            data.push_back((p.id >> 24) & 0xFF);
        }
        
        return data;
    }
};

/**
 * 📝 عملية تحرير
 */
struct EditOperation {
    uint32_t position;      // موقع البداية
    uint32_t deleteCount;   // عدد الأحرف للحذف
    std::string insertText; // النص للإدراج
    uint64_t logicalClock;  // ساعة منطقية (CRDT)
    
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> data;
        
        // Position
        data.push_back(position & 0xFF);
        data.push_back((position >> 8) & 0xFF);
        data.push_back((position >> 16) & 0xFF);
        data.push_back((position >> 24) & 0xFF);
        
        // Delete count
        data.push_back(deleteCount & 0xFF);
        data.push_back((deleteCount >> 8) & 0xFF);
        data.push_back((deleteCount >> 16) & 0xFF);
        data.push_back((deleteCount >> 24) & 0xFF);
        
        // Insert text
        uint32_t len = static_cast<uint32_t>(insertText.length());
        data.push_back(len & 0xFF);
        data.push_back((len >> 8) & 0xFF);
        data.push_back((len >> 16) & 0xFF);
        data.push_back((len >> 24) & 0xFF);
        data.insert(data.end(), insertText.begin(), insertText.end());
        
        // Logical clock
        for (int i = 0; i < 8; ++i) {
            data.push_back((logicalClock >> (i * 8)) & 0xFF);
        }
        
        return data;
    }
};

/**
 * 🖱️ موقع المؤشر
 */
struct CursorPosition {
    uint32_t line;
    uint32_t column;
    std::string filename;
    
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> data;
        
        data.push_back(line & 0xFF);
        data.push_back((line >> 8) & 0xFF);
        data.push_back((line >> 16) & 0xFF);
        data.push_back((line >> 24) & 0xFF);
        
        data.push_back(column & 0xFF);
        data.push_back((column >> 8) & 0xFF);
        data.push_back((column >> 16) & 0xFF);
        data.push_back((column >> 24) & 0xFF);
        
        uint16_t len = static_cast<uint16_t>(filename.length());
        data.push_back(len & 0xFF);
        data.push_back((len >> 8) & 0xFF);
        data.insert(data.end(), filename.begin(), filename.end());
        
        return data;
    }
};

/**
 * 💬 رسالة دردشة
 */
struct ChatMessage {
    std::string content;
    bool isCode;            // هل هي كود؟
    
    std::vector<uint8_t> toBytes() const {
        std::vector<uint8_t> data;
        
        data.push_back(isCode ? 1 : 0);
        
        uint32_t len = static_cast<uint32_t>(content.length());
        data.push_back(len & 0xFF);
        data.push_back((len >> 8) & 0xFF);
        data.push_back((len >> 16) & 0xFF);
        data.push_back((len >> 24) & 0xFF);
        data.insert(data.end(), content.begin(), content.end());
        
        return data;
    }
};

// ==========================================================
// 📌 رسالة كاملة
// ==========================================================

/**
 * 📨 رسالة بروتوكول
 */
class Message {
private:
    MessageHeader header_;
    std::vector<uint8_t> payload_;
    
public:
    /**
     * إنشاء رسالة
     */
    Message(MessageType type, UserId sender) {
        header_.type = type;
        header_.sender = sender;
        header_.sequence = nextSequence();
        header_.timestamp = currentTimestamp();
    }
    
    /**
     * تعيين الحمولة
     */
    void setPayload(const std::vector<uint8_t>& payload) {
        payload_ = payload;
        header_.length = static_cast<uint32_t>(payload.size());
    }
    
    /**
     * الحصول على الرأس
     */
    const MessageHeader& header() const { return header_; }
    
    /**
     * الحصول على الحمولة
     */
    const std::vector<uint8_t>& payload() const { return payload_; }
    
    /**
     * تحويل لـ bytes
     */
    std::vector<uint8_t> toBytes() const {
        auto headerBytes = header_.toBytes();
        std::vector<uint8_t> result;
        result.reserve(headerBytes.size() + payload_.size());
        result.insert(result.end(), headerBytes.begin(), headerBytes.end());
        result.insert(result.end(), payload_.begin(), payload_.end());
        return result;
    }
    
    /**
     * من bytes
     */
    static Message fromBytes(const uint8_t* data, size_t length) {
        MessageHeader header = MessageHeader::fromBytes(data);
        Message msg(header.type, header.sender);
        msg.header_ = header;
        
        if (length > MessageHeader::SIZE) {
            msg.payload_.assign(
                data + MessageHeader::SIZE,
                data + length
            );
        }
        
        return msg;
    }
    
    // ═══════════════════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════════════════
    
    /**
     * إنشاء رسالة طلب انضمام
     */
    static Message createJoinRequest(UserId sender, const SessionId& session,
                                      const std::string& username,
                                      const std::string& color = "#00FF00") {
        Message msg(MessageType::JOIN_REQUEST, sender);
        
        JoinRequest req;
        req.sessionId = session;
        req.username = username;
        req.color = color;
        
        msg.setPayload(req.toBytes());
        return msg;
    }
    
    /**
     * إنشاء رسالة تحرير
     */
    static Message createEdit(UserId sender, uint32_t pos,
                               uint32_t deleteCount, const std::string& text,
                               uint64_t clock) {
        MessageType type = MessageType::TEXT_REPLACE;
        if (deleteCount == 0) type = MessageType::TEXT_INSERT;
        else if (text.empty()) type = MessageType::TEXT_DELETE;
        
        Message msg(type, sender);
        
        EditOperation op;
        op.position = pos;
        op.deleteCount = deleteCount;
        op.insertText = text;
        op.logicalClock = clock;
        
        msg.setPayload(op.toBytes());
        return msg;
    }
    
    /**
     * إنشاء رسالة مؤشر
     */
    static Message createCursor(UserId sender, uint32_t line, uint32_t col,
                                 const std::string& filename) {
        Message msg(MessageType::CURSOR_MOVE, sender);
        
        CursorPosition pos;
        pos.line = line;
        pos.column = col;
        pos.filename = filename;
        
        msg.setPayload(pos.toBytes());
        return msg;
    }
    
    /**
     * إنشاء رسالة دردشة
     */
    static Message createChat(UserId sender, const std::string& content,
                               bool isCode = false) {
        Message msg(MessageType::CHAT_MESSAGE, sender);
        
        ChatMessage chat;
        chat.content = content;
        chat.isCode = isCode;
        
        msg.setPayload(chat.toBytes());
        return msg;
    }
    
private:
    static uint32_t nextSequence() {
        static uint32_t seq = 0;
        return ++seq;
    }
    
    static uint64_t currentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto epoch = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
    }
};

// ==========================================================
// 📌 معالج البروتوكول
// ==========================================================

/**
 * 🔧 معالج الرسائل
 */
class ProtocolHandler {
public:
    /**
     * معالجة رسالة واردة
     */
    virtual void onJoinRequest(const Message& msg, const JoinRequest& req) {}
    virtual void onJoinResponse(const Message& msg, const JoinResponse& resp) {}
    virtual void onLeave(const Message& msg) {}
    virtual void onEdit(const Message& msg, const EditOperation& op) {}
    virtual void onCursor(const Message& msg, const CursorPosition& pos) {}
    virtual void onChat(const Message& msg, const ChatMessage& chat) {}
    virtual void onError(const Message& msg, const std::string& error) {}
    
    /**
     * تحليل ومعالجة
     */
    void handle(const Message& msg) {
        switch (msg.header().type) {
            case MessageType::JOIN_REQUEST:
                // Parse and call onJoinRequest
                break;
            case MessageType::JOIN_RESPONSE:
                // Parse and call onJoinResponse
                break;
            case MessageType::LEAVE:
                onLeave(msg);
                break;
            case MessageType::TEXT_INSERT:
            case MessageType::TEXT_DELETE:
            case MessageType::TEXT_REPLACE:
                // Parse and call onEdit
                break;
            case MessageType::CURSOR_MOVE:
                // Parse and call onCursor
                break;
            case MessageType::CHAT_MESSAGE:
                // Parse and call onChat
                break;
            case MessageType::ERROR:
                // Parse and call onError
                break;
            default:
                break;
        }
    }
};

} // namespace collab
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * توليد معرّف جلسة
 */
char* sad_collab_generate_session_id(void) {
    auto id = sad::collab::SessionId::generate();
    std::string str = id.toString();
    
    char* result = new char[str.size() + 1];
    std::strcpy(result, str.c_str());
    return result;
}

/**
 * توليد معرّف مستخدم
 */
uint32_t sad_collab_generate_user_id(void) {
    return sad::collab::UserId::generate().id;
}

/**
 * إنشاء رسالة انضمام
 */
void* sad_collab_create_join_message(uint32_t userId, const char* sessionId,
                                      const char* username) {
    sad::collab::UserId uid;
    uid.id = userId;
    
    auto msg = new sad::collab::Message(
        sad::collab::Message::createJoinRequest(
            uid,
            sad::collab::SessionId::fromString(sessionId),
            username
        )
    );
    return msg;
}

/**
 * تحويل الرسالة لـ bytes
 */
size_t sad_collab_message_to_bytes(void* message, uint8_t* buffer, size_t bufferSize) {
    auto* msg = static_cast<sad::collab::Message*>(message);
    auto bytes = msg->toBytes();
    
    if (bytes.size() <= bufferSize) {
        memcpy(buffer, bytes.data(), bytes.size());
        return bytes.size();
    }
    return 0;
}

/**
 * تحرير رسالة
 */
void sad_collab_free_message(void* message) {
    delete static_cast<sad::collab::Message*>(message);
}

/**
 * تحرير نص
 */
void sad_collab_free_string(char* str) {
    delete[] str;
}

} // extern "C"
