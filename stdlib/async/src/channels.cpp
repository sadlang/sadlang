// ===================================================================
// قناة_ثنائية - قنوات الاتصال ثنائية الاتجاه
// channels.cpp - Bidirectional Communication Channels
// ===================================================================
// يوفر قنوات اتصال بين المهام المتزامنة:
// - قناة أحادية الاتجاه (mpsc)
// - قناة ثنائية الاتجاه
// - قناة مُخزّنة (buffered)
// ===================================================================

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <memory>
#include <atomic>
#include <chrono>
#include <functional>

namespace sad {
namespace stdlib {
namespace async {

/// حالة القناة
enum class ChannelState {
    Open,    // مفتوحة
    Closed,  // مغلقة
};

/// نتيجة الإرسال/الاستقبال
template<typename T>
struct ChannelResult {
    bool success;
    std::optional<T> value;
    std::string error_ar;
    
    /// إنشاء نتيجة ناجحة
    static ChannelResult ok(T val) {
        return {true, std::move(val), ""};
    }
    
    /// إنشاء نتيجة فارغة (لا قيمة)
    static ChannelResult empty() {
        return {true, std::nullopt, ""};
    }
    
    /// إنشاء نتيجة خطأ
    static ChannelResult error(const std::string& msg) {
        return {false, std::nullopt, msg};
    }
};

/// قناة ثنائية الاتجاه مُخزّنة
template<typename T>
class BidirectionalChannel {
public:
    /// إنشاء قناة بحجم مُخزّن
    explicit BidirectionalChannel(size_t buffer_size = 0)
        : buffer_size_(buffer_size) {}
    
    // === طرف أ ===
    
    /// إرسال من الطرف أ إلى الطرف ب
    bool أرسل_أ(T value) {
        return send(a_to_b_, a_to_b_mutex_, a_to_b_cv_, std::move(value));
    }
    
    /// استقبال في الطرف أ (من الطرف ب)
    ChannelResult<T> استقبل_أ() {
        return receive(b_to_a_, b_to_a_mutex_, b_to_a_cv_);
    }
    
    /// استقبال مع مهلة في الطرف أ
    ChannelResult<T> استقبل_أ_مهلة(std::chrono::milliseconds timeout) {
        return receive_timeout(b_to_a_, b_to_a_mutex_, b_to_a_cv_, timeout);
    }
    
    // === طرف ب ===
    
    /// إرسال من الطرف ب إلى الطرف أ
    bool أرسل_ب(T value) {
        return send(b_to_a_, b_to_a_mutex_, b_to_a_cv_, std::move(value));
    }
    
    /// استقبال في الطرف ب (من الطرف أ)
    ChannelResult<T> استقبل_ب() {
        return receive(a_to_b_, a_to_b_mutex_, a_to_b_cv_);
    }
    
    /// استقبال مع مهلة في الطرف ب
    ChannelResult<T> استقبل_ب_مهلة(std::chrono::milliseconds timeout) {
        return receive_timeout(a_to_b_, a_to_b_mutex_, a_to_b_cv_, timeout);
    }
    
    // === التحكم ===
    
    /// إغلاق القناة
    void أغلق() {
        state_.store(ChannelState::Closed);
        a_to_b_cv_.notify_all();
        b_to_a_cv_.notify_all();
    }
    
    /// هل القناة مفتوحة؟
    bool مفتوحة() const {
        return state_.load() == ChannelState::Open;
    }
    
    /// عدد الرسائل المنتظرة (أ إلى ب)
    size_t حجم_أ() const {
        std::lock_guard<std::mutex> lock(a_to_b_mutex_);
        return a_to_b_.size();
    }
    
    /// عدد الرسائل المنتظرة (ب إلى أ)
    size_t حجم_ب() const {
        std::lock_guard<std::mutex> lock(b_to_a_mutex_);
        return b_to_a_.size();
    }

private:
    /// إرسال رسالة إلى طابور
    bool send(std::queue<T>& queue, std::mutex& mutex, 
              std::condition_variable& cv, T value) {
        if (state_.load() == ChannelState::Closed) return false;
        
        {
            std::unique_lock<std::mutex> lock(mutex);
            
            // إذا كان مُخزّناً، انتظر حتى يتوفر مكان
            if (buffer_size_ > 0) {
                cv.wait(lock, [&] {
                    return queue.size() < buffer_size_ || 
                           state_.load() == ChannelState::Closed;
                });
            }
            
            if (state_.load() == ChannelState::Closed) return false;
            
            queue.push(std::move(value));
        }
        cv.notify_one();
        return true;
    }
    
    /// استقبال رسالة من طابور
    ChannelResult<T> receive(std::queue<T>& queue, std::mutex& mutex,
                              std::condition_variable& cv) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&] {
            return !queue.empty() || state_.load() == ChannelState::Closed;
        });
        
        if (queue.empty()) {
            return ChannelResult<T>::error("القناة مغلقة");
        }
        
        T value = std::move(queue.front());
        queue.pop();
        cv.notify_one(); // إعلام المرسل بتوفر مكان
        return ChannelResult<T>::ok(std::move(value));
    }
    
    /// استقبال مع مهلة
    ChannelResult<T> receive_timeout(std::queue<T>& queue, std::mutex& mutex,
                                      std::condition_variable& cv,
                                      std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        bool got = cv.wait_for(lock, timeout, [&] {
            return !queue.empty() || state_.load() == ChannelState::Closed;
        });
        
        if (!got || queue.empty()) {
            if (state_.load() == ChannelState::Closed) {
                return ChannelResult<T>::error("القناة مغلقة");
            }
            return ChannelResult<T>::error("انتهت المهلة الزمنية");
        }
        
        T value = std::move(queue.front());
        queue.pop();
        cv.notify_one();
        return ChannelResult<T>::ok(std::move(value));
    }
    
    // طوابير الرسائل
    std::queue<T> a_to_b_;
    mutable std::mutex a_to_b_mutex_;
    std::condition_variable a_to_b_cv_;
    
    std::queue<T> b_to_a_;
    mutable std::mutex b_to_a_mutex_;
    std::condition_variable b_to_a_cv_;
    
    size_t buffer_size_;
    std::atomic<ChannelState> state_{ChannelState::Open};
};

/// إنشاء قناة ثنائية
template<typename T>
inline BidirectionalChannel<T> قناة_ثنائية(size_t buffer_size = 0) {
    return BidirectionalChannel<T>(buffer_size);
}

} // namespace async
} // namespace stdlib
} // namespace sad
