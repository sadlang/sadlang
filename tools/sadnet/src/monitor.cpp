// ===================================================================
// صNet - مراقب الشبكة اللامركزية
// المرحلة 53: لوحة مراقبة وتحليل أداء الشبكة
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <chrono>
#include <deque>
#include <iostream>
#include <iomanip>

namespace sad::network {

/// مقاييس العُقدة في لحظة معينة
struct NodeMetrics {
    uint64_t timestamp;                // الطابع الزمني
    uint32_t connected_peers;          // عدد الأقران
    uint64_t bytes_sent;               // البايتات المُرسلة
    uint64_t bytes_received;           // البايتات المُستلمة
    uint32_t active_streams;           // البثوث النشطة
    uint32_t dht_entries;              // مدخلات DHT
    double cpu_usage;                  // استخدام المعالج %
    double memory_mb;                  // الذاكرة المستخدمة MB
    uint32_t open_connections;         // الاتصالات المفتوحة
    double bandwidth_mbps;             // عرض النطاق Mbps
};

/// تنبيه مراقبة
struct Alert {
    enum class Severity : uint8_t {
        INFO,       // معلومات
        WARNING,    // تحذير
        ERROR,      // خطأ
        CRITICAL    // حرج
    };

    Severity severity;             // مستوى الخطورة
    std::string message;           // رسالة التنبيه
    uint64_t timestamp;            // وقت التنبيه
    std::string source;            // مصدر التنبيه
};

/// جامع المقاييس - يأخذ عيّنات دورية
class MetricsCollector {
    std::deque<NodeMetrics> history_;       // سجل المقاييس
    size_t max_history_;                   // أقصى حجم للسجل
    uint32_t sample_interval_ms_;          // فترة أخذ العيّنات

public:
    MetricsCollector(size_t max = 10000, uint32_t interval = 5000)
        : max_history_(max), sample_interval_ms_(interval) {}

    /// تسجيل عيّنة مقاييس جديدة
    void record(const NodeMetrics& metrics) {
        history_.push_back(metrics);
        if (history_.size() > max_history_) {
            history_.pop_front();
        }
    }

    /// حساب متوسط عرض النطاق في آخر n عيّنات
    double average_bandwidth(size_t n = 60) const {
        if (history_.empty()) return 0.0;
        double sum = 0;
        size_t count = std::min(n, history_.size());
        auto it = history_.rbegin();
        for (size_t i = 0; i < count; i++, ++it) {
            sum += it->bandwidth_mbps;
        }
        return sum / count;
    }

    /// الحصول على آخر عيّنة
    const NodeMetrics& latest() const {
        return history_.back();
    }

    size_t sample_count() const { return history_.size(); }
};

/// محرك التنبيهات - كشف المشاكل تلقائياً
class AlertEngine {
    std::deque<Alert> alerts_;             // قائمة التنبيهات
    uint32_t max_peers_threshold_;         // حد أقصى للأقران
    double max_cpu_threshold_;             // حد المعالج %
    double max_memory_mb_;                 // حد الذاكرة MB

public:
    AlertEngine() : max_peers_threshold_(100),
                    max_cpu_threshold_(90.0),
                    max_memory_mb_(512.0) {}

    /// فحص المقاييس وتوليد التنبيهات
    void check(const NodeMetrics& metrics) {
        if (metrics.cpu_usage > max_cpu_threshold_) {
            emit_alert(Alert::Severity::WARNING,
                "استخدام المعالج مرتفع: " +
                std::to_string((int)metrics.cpu_usage) + "%");
        }
        if (metrics.memory_mb > max_memory_mb_) {
            emit_alert(Alert::Severity::WARNING,
                "استخدام الذاكرة مرتفع: " +
                std::to_string((int)metrics.memory_mb) + " MB");
        }
        if (metrics.connected_peers == 0) {
            emit_alert(Alert::Severity::ERROR,
                "لا يوجد أقران متصلون!");
        }
        if (metrics.connected_peers > max_peers_threshold_) {
            emit_alert(Alert::Severity::INFO,
                "عدد الأقران كبير: " +
                std::to_string(metrics.connected_peers));
        }
    }

    const std::deque<Alert>& alerts() const { return alerts_; }

private:
    void emit_alert(Alert::Severity sev, const std::string& msg) {
        Alert alert{sev, msg, 0, "monitor"};
        alerts_.push_back(alert);
        if (alerts_.size() > 500) alerts_.pop_front();
    }
};

/// مراقب الشبكة الرئيسي
class NetworkMonitor {
    MetricsCollector collector_;       // جامع المقاييس
    AlertEngine alerts_;              // محرك التنبيهات
    bool running_;                    // هل المراقب يعمل؟

public:
    NetworkMonitor() : running_(false) {}

    /// بدء المراقبة
    void start() {
        running_ = true;
        // حلقة أخذ العيّنات الدورية
    }

    /// إيقاف المراقبة
    void stop() { running_ = false; }

    /// طباعة ملخص الحالة الحالية
    void print_dashboard() const {
        if (collector_.sample_count() == 0) {
            std::cout << "لا توجد بيانات بعد.\n";
            return;
        }
        const auto& m = collector_.latest();
        std::cout << "╔══════════════════════════════════╗\n";
        std::cout << "║     لوحة مراقبة شبكة صد         ║\n";
        std::cout << "╠══════════════════════════════════╣\n";
        std::cout << "║ الأقران: " << std::setw(6) << m.connected_peers;
        std::cout << "                    ║\n";
        std::cout << "║ النطاق:  " << std::fixed << std::setprecision(1)
                  << m.bandwidth_mbps << " Mbps";
        std::cout << "              ║\n";
        std::cout << "║ DHT:     " << std::setw(6) << m.dht_entries;
        std::cout << " مدخل              ║\n";
        std::cout << "║ المعالج: " << std::setprecision(0) << m.cpu_usage;
        std::cout << "%                     ║\n";
        std::cout << "╚══════════════════════════════════╝\n";

        // عرض التنبيهات الأخيرة
        auto& recent = alerts_.alerts();
        if (!recent.empty()) {
            std::cout << "\nآخر التنبيهات:\n";
            size_t show = std::min((size_t)5, recent.size());
            for (size_t i = recent.size() - show; i < recent.size(); i++) {
                std::cout << "  ⚠ " << recent[i].message << "\n";
            }
        }
    }

    /// تسجيل عيّنة يدوياً (للاختبار)
    void record(const NodeMetrics& m) {
        collector_.record(m);
        alerts_.check(m);
    }
};

} // namespace sad::network
