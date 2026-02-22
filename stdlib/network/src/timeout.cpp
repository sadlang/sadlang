// ===================================================================
// مهلة_ذكية - إدارة المهلات الزمنية الذكية
// timeout.cpp - Smart Timeout Handling
// ===================================================================
// يوفر نظام مهلات ذكي يتكيف مع حالة الشبكة:
// - مهلة تكيفية حسب زمن الاستجابة
// - تراجع أسي (Exponential Backoff)
// - مهلة مختلفة لكل مرحلة (اتصال، إرسال، استقبال)
// ===================================================================

#include <string>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <vector>
#include <functional>

namespace sad {
namespace stdlib {
namespace network {

/// مراحل الطلب
enum class RequestPhase {
    Connect,    // مرحلة الاتصال
    Send,       // مرحلة الإرسال
    Receive,    // مرحلة الاستقبال
    Total,      // المهلة الإجمالية
};

/// إعدادات المهلة لكل مرحلة
struct PhaseTimeout {
    std::chrono::milliseconds connect{5000};   // مهلة الاتصال: 5 ثواني
    std::chrono::milliseconds send{30000};     // مهلة الإرسال: 30 ثانية
    std::chrono::milliseconds receive{30000};  // مهلة الاستقبال: 30 ثانية
    std::chrono::milliseconds total{60000};    // المهلة الإجمالية: 60 ثانية
};

/// سجل زمن الاستجابة لحساب المتوسط
struct LatencyRecord {
    std::chrono::milliseconds duration;
    std::chrono::steady_clock::time_point timestamp;
    bool success;
};

/// نظام المهلة الذكية
class SmartTimeout {
public:
    /// إنشاء مع إعدادات افتراضية
    SmartTimeout() = default;
    
    /// إنشاء مع إعدادات مخصصة
    explicit SmartTimeout(PhaseTimeout timeouts)
        : base_timeouts_(std::move(timeouts)) {}
    
    /// تحديد مهلة الاتصال
    SmartTimeout& مهلة_اتصال(std::chrono::milliseconds ms) {
        base_timeouts_.connect = ms;
        return *this;
    }
    
    /// تحديد مهلة الإرسال
    SmartTimeout& مهلة_إرسال(std::chrono::milliseconds ms) {
        base_timeouts_.send = ms;
        return *this;
    }
    
    /// تحديد مهلة الاستقبال
    SmartTimeout& مهلة_استقبال(std::chrono::milliseconds ms) {
        base_timeouts_.receive = ms;
        return *this;
    }
    
    /// تحديد المهلة الإجمالية
    SmartTimeout& مهلة_إجمالية(std::chrono::milliseconds ms) {
        base_timeouts_.total = ms;
        return *this;
    }
    
    /// تفعيل/تعطيل التكيف
    SmartTimeout& تكيف(bool enable) {
        adaptive_enabled_ = enable;
        return *this;
    }
    
    /// الحصول على المهلة الفعلية لمرحلة معينة
    std::chrono::milliseconds getTimeout(RequestPhase phase) const {
        auto base = getBaseTimeout(phase);
        
        if (!adaptive_enabled_ || latency_history_.empty()) {
            return base;
        }
        
        // حساب المهلة التكيفية
        auto avg_latency = calculateAverageLatency();
        auto adaptive = std::chrono::milliseconds(
            static_cast<int64_t>(avg_latency.count() * adaptive_multiplier_)
        );
        
        // اختر الأكبر بين الحد الأدنى والقيمة التكيفية
        auto min_timeout = std::chrono::milliseconds(
            static_cast<int64_t>(base.count() * 0.5)
        );
        return std::max(min_timeout, std::min(adaptive, base));
    }
    
    /// تسجيل نتيجة طلب (لتحسين التكيف)
    void recordLatency(std::chrono::milliseconds duration, bool success) {
        latency_history_.push_back({
            duration,
            std::chrono::steady_clock::now(),
            success
        });
        
        // الاحتفاظ بآخر 100 سجل فقط
        if (latency_history_.size() > max_history_size_) {
            latency_history_.erase(latency_history_.begin());
        }
    }
    
    /// حساب مهلة التراجع الأسي
    static std::chrono::milliseconds exponentialBackoff(
        int attempt, 
        std::chrono::milliseconds base = std::chrono::milliseconds(1000),
        double multiplier = 2.0,
        std::chrono::milliseconds max_delay = std::chrono::milliseconds(30000))
    {
        auto delay_ms = static_cast<int64_t>(
            base.count() * std::pow(multiplier, attempt)
        );
        return std::min(
            std::chrono::milliseconds(delay_ms),
            max_delay
        );
    }
    
    /// الحصول على متوسط زمن الاستجابة
    std::chrono::milliseconds calculateAverageLatency() const {
        if (latency_history_.empty()) return std::chrono::milliseconds(0);
        
        int64_t total = 0;
        int count = 0;
        for (const auto& record : latency_history_) {
            if (record.success) {
                total += record.duration.count();
                count++;
            }
        }
        
        if (count == 0) return std::chrono::milliseconds(0);
        return std::chrono::milliseconds(total / count);
    }

private:
    /// الحصول على المهلة الأساسية لمرحلة
    std::chrono::milliseconds getBaseTimeout(RequestPhase phase) const {
        switch (phase) {
            case RequestPhase::Connect: return base_timeouts_.connect;
            case RequestPhase::Send:    return base_timeouts_.send;
            case RequestPhase::Receive: return base_timeouts_.receive;
            case RequestPhase::Total:   return base_timeouts_.total;
        }
        return base_timeouts_.total;
    }
    
    PhaseTimeout base_timeouts_;
    bool adaptive_enabled_ = true;
    double adaptive_multiplier_ = 3.0;
    size_t max_history_size_ = 100;
    std::vector<LatencyRecord> latency_history_;
};

/// إنشاء مهلة ذكية
inline SmartTimeout مهلة_ذكية() {
    return SmartTimeout();
}

} // namespace network
} // namespace stdlib
} // namespace sad
