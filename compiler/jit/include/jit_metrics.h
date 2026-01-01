// ============================================================================
// jit_metrics.h - مقاييس أداء JIT (JIT Metrics)
// Performance Metrics Collection for JIT Engine
// ============================================================================
// الغرض: جمع وتتبع مقاييس الأداء في الوقت الفعلي
// Purpose: Collect and track real-time performance metrics
// ============================================================================

#ifndef SAD_JIT_METRICS_H
#define SAD_JIT_METRICS_H

#include <atomic>        // للعمليات الذرية / For atomic operations
#include <chrono>        // للوقت / For time
#include <memory>        // لإدارة الذاكرة الذكية / For smart pointers
#include <string>        // للنصوص / For strings
#include <unordered_map> // للخرائط السريعة / For fast maps
#include <mutex>         // للتزامن / For thread safety

namespace Sad {
namespace JIT {

// ============================================================================
// MetricType - نوع المقياس / Metric Type
// ============================================================================

enum class MetricType {
    COUNTER,                         // عداد (يزيد فقط) / Counter (increases only)
    GAUGE,                           // قياس (يمكن أن يزيد أو ينقص) / Gauge (can increase/decrease)
    HISTOGRAM,                       // رسم بياني (توزيع القيم) / Histogram (value distribution)
    TIMER                            // مؤقت (قياس المدة) / Timer (duration measurement)
};

// ============================================================================
// Metric - مقياس واحد / Single Metric
// ============================================================================

class Metric {
public:
    // ========================================
    // المُنشئ / Constructor
    // ========================================
    
    Metric(const std::string& name, MetricType type, const std::string& description = "")
        : name_(name)
        , type_(type)
        , description_(description)
        , value_(0)
        , count_(0)
        , sum_(0)
        , min_(std::numeric_limits<double>::max())
        , max_(std::numeric_limits<double>::min())
    {}
    
    // ========================================
    // العمليات / Operations
    // ========================================
    
    // زيادة (للعداد) / Increment (for counter)
    void increment(double delta = 1.0) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ += delta;
    }
    
    // تعيين (للقياس) / Set (for gauge)
    void set(double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = value;
    }
    
    // تسجيل (للرسم البياني والمؤقت) / Record (for histogram/timer)
    void record(double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        count_++;
        sum_ += value;
        
        if (value < min_) min_ = value;
        if (value > max_) max_ = value;
        
        // تحديث الرسم البياني / Update histogram
        if (type_ == MetricType::HISTOGRAM || type_ == MetricType::TIMER) {
            updateHistogram(value);
        }
    }
    
    // ========================================
    // الوصول / Access
    // ========================================
    
    // الحصول على القيمة الحالية / Get current value
    double getValue() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return value_;
    }
    
    // الحصول على العدد / Get count
    size_t getCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_;
    }
    
    // الحصول على المتوسط / Get average
    double getAverage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_ > 0 ? sum_ / count_ : 0.0;
    }
    
    // الحصول على الحد الأدنى / Get minimum
    double getMin() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_ > 0 ? min_ : 0.0;
    }
    
    // الحصول على الحد الأقصى / Get maximum
    double getMax() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return count_ > 0 ? max_ : 0.0;
    }
    
    // الحصول على المجموع / Get sum
    double getSum() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sum_;
    }
    
    // ========================================
    // المعلومات / Information
    // ========================================
    
    const std::string& getName() const { return name_; }
    MetricType getType() const { return type_; }
    const std::string& getDescription() const { return description_; }
    
    // ========================================
    // الإدارة / Management
    // ========================================
    
    // إعادة تعيين / Reset
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = 0.0;
        count_ = 0;
        sum_ = 0;
        min_ = std::numeric_limits<double>::max();
        max_ = std::numeric_limits<double>::min();
        histogram_.clear();
    }
    
    // ========================================
    // التصدير / Export
    // ========================================
    
    // تحويل إلى نص / Convert to string
    std::string toString() const;
    
    // تحويل إلى JSON / Convert to JSON
    std::string toJSON() const;

private:
    // ========================================
    // البيانات / Data
    // ========================================
    
    std::string name_;               // اسم المقياس / Metric name
    MetricType type_;                // نوع المقياس / Metric type
    std::string description_;        // الوصف / Description
    
    double value_;                   // القيمة الحالية / Current value
    
    // إحصائيات (محمية بـ mutex) / Statistics (protected by mutex)
    mutable std::mutex mutex_;
    size_t count_;                   // عدد القياسات / Number of measurements
    double sum_;                     // المجموع / Sum
    double min_;                     // الحد الأدنى / Minimum
    double max_;                     // الحد الأقصى / Maximum
    
    // الرسم البياني (للتوزيع) / Histogram (for distribution)
    std::unordered_map<int, size_t> histogram_; // bucket → count
    
    // ========================================
    // الدوال المساعدة / Helper Functions
    // ========================================
    
    // تحديث الرسم البياني / Update histogram
    void updateHistogram(double value);
};

// ============================================================================
// MetricsCollector - جامع المقاييس / Metrics Collector
// ============================================================================

class MetricsCollector {
public:
    // ========================================
    // المُنشئ والمُدمر / Constructor & Destructor
    // ========================================
    
    MetricsCollector();
    ~MetricsCollector();
    
    // منع النسخ / Prevent copying
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;
    
    // ========================================
    // تسجيل المقاييس / Register Metrics
    // ========================================
    
    // تسجيل عداد / Register counter
    void registerCounter(
        const std::string& name,
        const std::string& description = ""
    );
    
    // تسجيل قياس / Register gauge
    void registerGauge(
        const std::string& name,
        const std::string& description = ""
    );
    
    // تسجيل رسم بياني / Register histogram
    void registerHistogram(
        const std::string& name,
        const std::string& description = ""
    );
    
    // تسجيل مؤقت / Register timer
    void registerTimer(
        const std::string& name,
        const std::string& description = ""
    );
    
    // ========================================
    // تحديث المقاييس / Update Metrics
    // ========================================
    
    // زيادة عداد / Increment counter
    void incrementCounter(const std::string& name, double delta = 1.0);
    
    // تعيين قياس / Set gauge
    void setGauge(const std::string& name, double value);
    
    // تسجيل قيمة / Record value
    void recordValue(const std::string& name, double value);
    
    // تسجيل مدة / Record duration
    void recordDuration(const std::string& name, std::chrono::microseconds duration);
    
    // ========================================
    // الوصول للمقاييس / Access Metrics
    // ========================================
    
    // الحصول على مقياس / Get metric
    const Metric* getMetric(const std::string& name) const;
    
    // الحصول على جميع المقاييس / Get all metrics
    std::vector<const Metric*> getAllMetrics() const;
    
    // الحصول على المقاييس حسب النوع / Get metrics by type
    std::vector<const Metric*> getMetricsByType(MetricType type) const;
    
    // ========================================
    // المقاييس المُعرّفة مسبقاً / Predefined Metrics
    // ========================================
    
    // مقاييس التجميع / Compilation metrics
    void recordCompilationTime(std::chrono::microseconds duration);
    void recordCompilationSuccess();
    void recordCompilationFailure();
    
    // مقاييس الذاكرة المؤقتة / Cache metrics
    void recordCacheHit();
    void recordCacheMiss();
    void recordCacheEviction();
    
    // مقاييس التنفيذ / Execution metrics
    void recordExecutionTime(std::chrono::microseconds duration);
    void recordJITExecution();
    void recordInterpretedExecution();
    
    // مقاييس المسارات الساخنة / Hot path metrics
    void recordHotPathDetected(const std::string& function_name);
    void recordFunctionExecution(const std::string& function_name);
    
    // ========================================
    // التقارير / Reporting
    // ========================================
    
    // طباعة ملخص / Print summary
    void printSummary() const;
    
    // تصدير إلى JSON / Export to JSON
    std::string toJSON() const;
    
    // حفظ إلى ملف / Save to file
    bool saveToFile(const std::string& filename) const;
    
    // ========================================
    // الإدارة / Management
    // ========================================
    
    // إعادة تعيين جميع المقاييس / Reset all metrics
    void resetAll();
    
    // مسح جميع المقاييس / Clear all metrics
    void clearAll();
    
    // الحصول على عدد المقاييس / Get metric count
    size_t getMetricCount() const;

private:
    // ========================================
    // البيانات الداخلية / Internal Data
    // ========================================
    
    mutable std::mutex mutex_;       // قفل للتزامن / Mutex for thread safety
    
    std::unordered_map<std::string, std::unique_ptr<Metric>> metrics_; // خريطة المقاييس / Metrics map
    
    std::chrono::steady_clock::time_point start_time_; // وقت البدء / Start time
    
    // ========================================
    // الدوال المساعدة / Helper Functions
    // ========================================
    
    // الحصول أو إنشاء مقياس / Get or create metric
    Metric* getOrCreateMetric(const std::string& name, MetricType type);
    
    // التحقق من وجود مقياس / Check if metric exists
    bool hasMetric(const std::string& name) const;
};

// ============================================================================
// RAII Timer للقياس التلقائي / RAII Timer for Automatic Measurement
// ============================================================================

class MetricTimer {
public:
    MetricTimer(MetricsCollector& collector, const std::string& metric_name)
        : collector_(collector)
        , metric_name_(metric_name)
        , start_time_(std::chrono::steady_clock::now())
    {}
    
    ~MetricTimer() {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time_
        );
        collector_.recordDuration(metric_name_, duration);
    }
    
    // منع النسخ / Prevent copying
    MetricTimer(const MetricTimer&) = delete;
    MetricTimer& operator=(const MetricTimer&) = delete;

private:
    MetricsCollector& collector_;
    std::string metric_name_;
    std::chrono::steady_clock::time_point start_time_;
};

// ============================================================================
// Macros للاستخدام السهل / Macros for Easy Usage
// ============================================================================

#define METRIC_TIMER(collector, name) \
    MetricTimer _metric_timer_##__LINE__(collector, name)

#define METRIC_TIME_FUNCTION(collector) \
    METRIC_TIMER(collector, std::string(__FUNCTION__) + "_time")

} // namespace JIT
} // namespace Sad

#endif // SAD_JIT_METRICS_H

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
// ملاحظات التنفيذ / Implementation Notes:
// - استخدام std::atomic للعمليات الذرية / Using std::atomic for atomic ops
// - Thread-safe باستخدام mutex / Thread-safe using mutex
// - دعم مقاييس متعددة الأنواع / Supports multiple metric types
// - قياس تلقائي باستخدام RAII / Automatic measurement using RAII
// ============================================================================
