/**
 * ==========================================================
 * File: actor_type.cpp / ملف: actor_type.cpp
 * Description: Actor Type for Distributed Computing
 * Task: T359 - Sad Distribution
 * ==========================================================
 */

 /**
 * ==========================================================
 * ملف: actor_type.cpp
 * الوصف: نوع Actor الأساسي للحوسبة الموزعة
 * المهمة: T359 - توزيع ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * نموذج Actor هو طريقة للبرمجة المتزامنة والموزعة:
 * - كل Actor هو وحدة مستقلة بحالة خاصة
 * - Actors يتواصلون عبر الرسائل فقط
 * - لا يوجد تشارك بيانات مباشر (thread-safe by design)
 * 
 * 💡 مثال في ص:
 * ```
 * #[ممثل]
 * صنف عداد
 *     متغير قيمة: عدد = 0
 *     
 *     رسالة زِد()
 *         ذاتي.قيمة++
 *     نهاية
 *     
 *     رسالة احصل() -> عدد
 *         ارجع ذاتي.قيمة
 *     نهاية
 * نهاية
 * ```
 */


#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <queue>
#include <mutex>
#include <functional>

namespace Sad {
namespace TypeSystem {

// Message Types
enum class MessageType {
    Normal, Request, Reply, SystemSignal, ErrorNotify, Stop
};

struct ActorMessage {
    MessageType msgType = MessageType::Normal;
    std::string name;
    std::vector<std::string> params;
    std::string sender;
    uint64_t requestId = 0;
    
    ActorMessage() = default;
    ActorMessage(const std::string& n, const std::vector<std::string>& p = {})
        : name(n), params(p) {}
};

// Actor State
enum class ActorState {
    New, Running, Suspended, Failed, Stopped
};

struct ActorStats {
    uint64_t messagesReceived = 0;
    uint64_t messagesProcessed = 0;
    uint64_t messagesFailed = 0;
    uint64_t errors = 0;
    double avgProcessingTimeMs = 0;
};

// Actor Type
using MessageHandler = std::function<std::string(const ActorMessage&)>;

class ActorType {
private:
    std::string id_;
    std::string name_;
    ActorState state_ = ActorState::New;
    ActorStats stats_;
    
    std::map<std::string, MessageHandler> handlers_;
    std::queue<ActorMessage> inbox_;
    std::mutex inboxLock_;
    std::string supervisorId_;
    
public:
    ActorType(const std::string& name = "") : name_(name) {
        id_ = generateId();
    }
    
    virtual ~ActorType() = default;
    
    static std::string generateId() {
        static uint64_t counter = 0;
        return "actor_" + std::to_string(++counter);
    }
    
    void registerHandler(const std::string& msgName, MessageHandler handler) {
        handlers_[msgName] = handler;
    }
    
    void send(const ActorMessage& msg) {
        std::lock_guard<std::mutex> lock(inboxLock_);
        inbox_.push(msg);
        stats_.messagesReceived++;
    }
    
    bool processMessage() {
        ActorMessage msg;
        
        {
            std::lock_guard<std::mutex> lock(inboxLock_);
            if (inbox_.empty()) return false;
            msg = inbox_.front();
            inbox_.pop();
        }
        
        auto it = handlers_.find(msg.name);
        if (it != handlers_.end()) {
            try {
                it->second(msg);
                stats_.messagesProcessed++;
            } catch (...) {
                stats_.messagesFailed++;
                stats_.errors++;
            }
        }
        
        return true;
    }
    
    void start() { state_ = ActorState::Running; }
    void stop() { state_ = ActorState::Stopped; }
    
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    ActorState state() const { return state_; }
    const ActorStats& stats() const { return stats_; }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(inboxLock_));
        return inbox_.empty();
    }
    
    void setSupervisor(const std::string& id) { supervisorId_ = id; }
};

class ActorRef {
private:
    std::string id_;
    std::string address_;
    bool local_ = true;
    
public:
    ActorRef() = default;
    ActorRef(const std::string& id, bool local = true) : id_(id), local_(local) {}
    
    void send(const ActorMessage& msg) const {}
    std::string request(const ActorMessage& msg, int timeoutMs = 5000) const { return ""; }
    void stop() const {
        ActorMessage stopMsg;
        stopMsg.msgType = MessageType::Stop;
        send(stopMsg);
    }
    
    const std::string& id() const { return id_; }
    bool isLocal() const { return local_; }
    const std::string& address() const { return address_; }
    void setAddress(const std::string& addr) { address_ = addr; local_ = false; }
};

struct MessageDef {
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;
    std::string returnType;
    bool synchronous = false;
};

class ActorTypeSystem {
private:
    std::map<std::string, std::vector<MessageDef>> messageDefs_;
    
public:
    void registerType(const std::string& typeName, const std::vector<MessageDef>& messages) {
        messageDefs_[typeName] = messages;
    }
    
    bool hasMessage(const std::string& typeName, const std::string& msgName) const {
        auto it = messageDefs_.find(typeName);
        if (it == messageDefs_.end()) return false;
        for (const auto& msg : it->second) {
            if (msg.name == msgName) return true;
        }
        return false;
    }
    
    const MessageDef* getMessage(const std::string& typeName, const std::string& msgName) const {
        auto it = messageDefs_.find(typeName);
        if (it == messageDefs_.end()) return nullptr;
        for (const auto& msg : it->second) {
            if (msg.name == msgName) return &msg;
        }
        return nullptr;
    }
    
    std::vector<std::string> getMessages(const std::string& typeName) const {
        std::vector<std::string> result;
        auto it = messageDefs_.find(typeName);
        if (it != messageDefs_.end()) {
            for (const auto& msg : it->second) {
                result.push_back(msg.name);
            }
        }
        return result;
    }
};

} // namespace TypeSystem
} // namespace Sad
