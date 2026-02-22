// ===================================================================
// أحداث_مباشرة - Server-Sent Events (SSE)
// sse.cpp - تنفيذ بروتوكول SSE
// ===================================================================
// يوفر دعم أحداث الخادم المُرسلة (SSE):
// - ارسال أحداث من الخادم للعميل
// - إعادة الاتصال التلقائي
// - معرّفات الأحداث لاستئناف البث
// ===================================================================

#include <string>
#include <functional>
#include <vector>
#include <map>
#include <optional>
#include <sstream>
#include <chrono>

namespace sad {
namespace stdlib {
namespace network {

/// حدث SSE
struct ServerEvent {
    std::string id;         // معرّف الحدث
    std::string event_type; // نوع الحدث
    std::string data;       // البيانات
    int retry_ms = 0;       // مهلة إعادة الاتصال
    
    /// تحويل إلى صيغة SSE
    std::string toSSEFormat() const {
        std::ostringstream oss;
        if (!id.empty()) {
            oss << "id: " << id << "\n";
        }
        if (!event_type.empty()) {
            oss << "event: " << event_type << "\n";
        }
        if (retry_ms > 0) {
            oss << "retry: " << retry_ms << "\n";
        }
        // البيانات قد تكون متعددة الأسطر
        std::istringstream data_stream(data);
        std::string line;
        while (std::getline(data_stream, line)) {
            oss << "data: " << line << "\n";
        }
        oss << "\n"; // سطر فارغ ينهي الحدث
        return oss.str();
    }
};

/// مصدر أحداث الخادم (Server-side)
class EventSource {
public:
    using EventCallback = std::function<void(const ServerEvent&)>;
    using ErrorCallback = std::function<void(const std::string&)>;
    
    /// إنشاء مصدر أحداث
    EventSource() = default;
    
    /// إرسال حدث
    void أرسل(const ServerEvent& event) {
        events_sent_++;
        if (on_send_) {
            on_send_(event);
        }
    }
    
    /// إرسال بيانات بسيطة
    void أرسل_بيانات(const std::string& data) {
        ServerEvent event;
        event.data = data;
        event.id = std::to_string(events_sent_ + 1);
        أرسل(event);
    }
    
    /// إرسال حدث مُسمّى
    void أرسل_حدث(const std::string& type, const std::string& data) {
        ServerEvent event;
        event.event_type = type;
        event.data = data;
        event.id = std::to_string(events_sent_ + 1);
        أرسل(event);
    }
    
    /// تحديد استدعاء الإرسال
    EventSource& عند_إرسال(EventCallback callback) {
        on_send_ = std::move(callback);
        return *this;
    }
    
    /// ترويسات HTTP المطلوبة للـ SSE
    static std::map<std::string, std::string> sseHeaders() {
        return {
            {"Content-Type", "text/event-stream"},
            {"Cache-Control", "no-cache"},
            {"Connection", "keep-alive"},
            {"X-Content-Type-Options", "nosniff"},
        };
    }
    
    /// عدد الأحداث المُرسلة
    size_t eventsSent() const { return events_sent_; }

private:
    EventCallback on_send_;
    size_t events_sent_ = 0;
};

/// عميل أحداث الخادم (Client-side)
class EventListener {
public:
    using MessageCallback = std::function<void(const ServerEvent&)>;
    using OpenCallback = std::function<void()>;
    using ErrorCallback = std::function<void(const std::string&)>;
    
    /// إنشاء مستمع أحداث
    explicit EventListener(const std::string& url)
        : url_(url) {}
    
    /// الاستماع لكل الأحداث
    EventListener& عند_رسالة(MessageCallback callback) {
        on_message_ = std::move(callback);
        return *this;
    }
    
    /// الاستماع لنوع حدث محدد
    EventListener& عند_حدث(const std::string& event_type, MessageCallback callback) {
        event_handlers_[event_type] = std::move(callback);
        return *this;
    }
    
    /// عند فتح الاتصال
    EventListener& عند_اتصال(OpenCallback callback) {
        on_open_ = std::move(callback);
        return *this;
    }
    
    /// عند حدوث خطأ
    EventListener& عند_خطأ(ErrorCallback callback) {
        on_error_ = std::move(callback);
        return *this;
    }
    
    /// تحديد مهلة إعادة الاتصال
    EventListener& مهلة_إعادة_اتصال(std::chrono::milliseconds ms) {
        reconnect_timeout_ = ms;
        return *this;
    }
    
    /// تحديد آخر معرّف (للاستئناف)
    EventListener& آخر_معرّف(const std::string& id) {
        last_event_id_ = id;
        return *this;
    }
    
    /// بدء الاستماع
    void ابدأ() { connected_ = true; }
    
    /// إيقاف الاستماع
    void أوقف() { connected_ = false; }
    
    /// هل متصل؟
    bool متصل() const { return connected_; }
    
    /// معالجة سطر SSE خام
    void processLine(const std::string& line) {
        if (line.empty()) {
            // سطر فارغ = نهاية الحدث
            flushEvent();
            return;
        }
        
        size_t colon = line.find(':');
        if (colon == std::string::npos) return;
        
        std::string field = line.substr(0, colon);
        std::string value = (colon + 2 < line.size()) ? line.substr(colon + 2) : "";
        
        if (field == "data") {
            if (!current_event_.data.empty()) current_event_.data += "\n";
            current_event_.data += value;
        } else if (field == "event") {
            current_event_.event_type = value;
        } else if (field == "id") {
            current_event_.id = value;
            last_event_id_ = value;
        } else if (field == "retry") {
            try {
                current_event_.retry_ms = std::stoi(value);
                reconnect_timeout_ = std::chrono::milliseconds(current_event_.retry_ms);
            } catch (...) {}
        }
    }

private:
    /// إرسال الحدث المُجمّع
    void flushEvent() {
        if (current_event_.data.empty()) return;
        
        // التحقق من معالج مخصص للنوع
        auto it = event_handlers_.find(current_event_.event_type);
        if (it != event_handlers_.end()) {
            it->second(current_event_);
        }
        
        // المعالج العام
        if (on_message_) {
            on_message_(current_event_);
        }
        
        events_received_++;
        current_event_ = ServerEvent{};
    }
    
    std::string url_;
    bool connected_ = false;
    ServerEvent current_event_;
    std::string last_event_id_;
    std::chrono::milliseconds reconnect_timeout_{3000};
    size_t events_received_ = 0;
    
    MessageCallback on_message_;
    OpenCallback on_open_;
    ErrorCallback on_error_;
    std::map<std::string, MessageCallback> event_handlers_;
};

/// إنشاء مصدر أحداث
inline EventSource مصدر_أحداث() {
    return EventSource();
}

/// إنشاء مستمع أحداث
inline EventListener مستمع_أحداث(const std::string& url) {
    return EventListener(url);
}

} // namespace network
} // namespace stdlib
} // namespace sad
