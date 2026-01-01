// ============================================================================
// jit_metrics.cpp - تنفيذ مقاييس أداء JIT (JIT Metrics Implementation)
// Performance Metrics Collection for JIT Engine - Implementation
// ============================================================================
// الغرض: تنفيذ جمع وتتبع مقاييس الأداء في الوقت الفعلي
// Purpose: Implement real-time performance metrics collection and tracking
// ============================================================================

#include "jit_metrics.h"
#include <algorithm>     // للخوارزميات / For algorithms
#include <cmath>         // للعمليات الرياضية / For math operations
#include <sstream>       // لبناء النصوص / For string building
#include <iomanip>       // لتنسيق الإخراج / For output formatting
#include <fstream>       // للملفات / For files

namespace Sad {
namespace JIT {

// ============================================================================
// Metric - Implementation
// ============================================================================

std::string Metric::getName() const {
    return name_;
}

MetricType Metric::getType() const {
    return type_;
}

std::string Metric::getDescription() const {
    return description_;
}

void Metric::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    value_ = 0.0;
    count_ = 0;
    sum_ = 0.0;
    min_ = std::numeric_limits<double>::max();
    max_ = std::numeric_limits<double>::min();
    histogram_.clear();
}

std::string Metric::toString() const {
    std::ostringstream oss;
    
    oss << name_ << " [" << typeToString(type_) << "]: ";
    
    switch (type_) {
        case MetricType::COUNTER:
            oss << getValue();
            break;
            
        case MetricType::GAUGE:
            oss << getValue();
            break;
            
        case MetricType::HISTOGRAM:
        case MetricType::TIMER: {
            std::lock_guard<std::mutex> lock(mutex_);
            oss << "count=" << count_ << ", ";
            oss << "avg=" << std::fixed << std::setprecision(2) << getAverage() << ", ";
            oss << "min=" << getMin() << ", ";
            oss << "max=" << getMax();
            break;
        }
    }
    
    if (!description_.empty()) {
        oss << " (" << description_ << ")";
    }
    
    return oss.str();
}

void Metric::updateHistogram(double value) {
    // تحديد الفئة / Determine bucket
    // استخدام نطاقات أسية / Use exponential ranges
    // [0-1), [1-10), [10-100), [100-1000), [1000-10000), [10000+)
    
    int bucket = 0;
    if (value >= 10000) {
        bucket = 5;
    } else if (value >= 1000) {
        bucket = 4;
    } else if (value >= 100) {
        bucket = 3;
    } else if (value >= 10) {
        bucket = 2;
    } else if (value >= 1) {
        bucket = 1;
    }
    
    histogram_[bucket]++;
}

std::string Metric::typeToString(MetricType type) {
    switch (type) {
        case MetricType::COUNTER:   return "Counter";
        case MetricType::GAUGE:     return "Gauge";
        case MetricType::HISTOGRAM: return "Histogram";
        case MetricType::TIMER:     return "Timer";
        default:                    return "Unknown";
    }
}

// ============================================================================
// MetricsCollector - Implementation
// ============================================================================

MetricsCollector::MetricsCollector()
    : enabled_(true)
{
    // تهيئة المقاييس المحددة مسبقاً / Initialize predefined metrics
    initializePredefinedMetrics();
}

MetricsCollector::~MetricsCollector() {
    // حفظ التقرير النهائي / Save final report
    if (enabled_ && !metrics_.empty()) {
        exportToFile("jit_metrics_final.json");
    }
}

MetricsCollector& MetricsCollector::getInstance() {
    static MetricsCollector instance;
    return instance;
}

void MetricsCollector::enable() {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = true;
}

void MetricsCollector::disable() {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = false;
}

bool MetricsCollector::isEnabled() const {
    return enabled_;
}

void MetricsCollector::registerMetric(const std::string& name, 
                                     MetricType type, 
                                     const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // التحقق من عدم وجود المقياس / Check metric doesn't exist
    if (metrics_.find(name) != metrics_.end()) {
        return;
    }
    
    // إنشاء المقياس الجديد / Create new metric
    metrics_[name] = std::make_shared<Metric>(name, type, description);
}

std::shared_ptr<Metric> MetricsCollector::getMetric(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = metrics_.find(name);
    if (it != metrics_.end()) {
        return it->second;
    }
    
    return nullptr;
}

void MetricsCollector::incrementCounter(const std::string& name, double delta) {
    if (!enabled_) return;
    
    auto metric = getMetric(name);
    if (metric && metric->getType() == MetricType::COUNTER) {
        metric->increment(delta);
    }
}

void MetricsCollector::setGauge(const std::string& name, double value) {
    if (!enabled_) return;
    
    auto metric = getMetric(name);
    if (metric && metric->getType() == MetricType::GAUGE) {
        metric->set(value);
    }
}

void MetricsCollector::recordHistogram(const std::string& name, double value) {
    if (!enabled_) return;
    
    auto metric = getMetric(name);
    if (metric && metric->getType() == MetricType::HISTOGRAM) {
        metric->record(value);
    }
}

void MetricsCollector::recordTimer(const std::string& name, double duration_us) {
    if (!enabled_) return;
    
    auto metric = getMetric(name);
    if (metric && metric->getType() == MetricType::TIMER) {
        metric->record(duration_us);
    }
}

std::vector<std::shared_ptr<Metric>> MetricsCollector::getAllMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::shared_ptr<Metric>> result;
    result.reserve(metrics_.size());
    
    for (const auto& [name, metric] : metrics_) {
        result.push_back(metric);
    }
    
    return result;
}

void MetricsCollector::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& [name, metric] : metrics_) {
        metric->reset();
    }
}

void MetricsCollector::resetMetric(const std::string& name) {
    auto metric = getMetric(name);
    if (metric) {
        metric->reset();
    }
}

void MetricsCollector::exportToFile(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\n";
    file << "  \"metrics_version\": \"1.0\",\n";
    file << "  \"total_metrics\": " << metrics_.size() << ",\n";
    file << "  \"timestamp\": " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() << ",\n";
    
    file << "  \"metrics\": {\n";
    
    size_t count = 0;
    for (const auto& [name, metric] : metrics_) {
        file << "    \"" << name << "\": {\n";
        file << "      \"type\": \"" << Metric::typeToString(metric->getType()) << "\",\n";
        file << "      \"description\": \"" << metric->getDescription() << "\",\n";
        
        switch (metric->getType()) {
            case MetricType::COUNTER:
                file << "      \"value\": " << metric->getValue() << "\n";
                break;
                
            case MetricType::GAUGE:
                file << "      \"value\": " << metric->getValue() << "\n";
                break;
                
            case MetricType::HISTOGRAM:
            case MetricType::TIMER:
                file << "      \"count\": " << metric->getCount() << ",\n";
                file << "      \"sum\": " << metric->getSum() << ",\n";
                file << "      \"average\": " << metric->getAverage() << ",\n";
                file << "      \"min\": " << metric->getMin() << ",\n";
                file << "      \"max\": " << metric->getMax() << "\n";
                break;
        }
        
        file << "    }";
        if (++count < metrics_.size()) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  }\n";
    file << "}\n";
    
    file.close();
}

std::string MetricsCollector::generateReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream report;
    
    report << "========================================\n";
    report << "JIT Metrics Report\n";
    report << "تقرير مقاييس JIT\n";
    report << "========================================\n\n";
    
    // تصنيف المقاييس حسب النوع / Categorize metrics by type
    std::vector<std::shared_ptr<Metric>> counters;
    std::vector<std::shared_ptr<Metric>> gauges;
    std::vector<std::shared_ptr<Metric>> histograms;
    std::vector<std::shared_ptr<Metric>> timers;
    
    for (const auto& [name, metric] : metrics_) {
        switch (metric->getType()) {
            case MetricType::COUNTER:   counters.push_back(metric); break;
            case MetricType::GAUGE:     gauges.push_back(metric); break;
            case MetricType::HISTOGRAM: histograms.push_back(metric); break;
            case MetricType::TIMER:     timers.push_back(metric); break;
        }
    }
    
    // العدادات / Counters
    if (!counters.empty()) {
        report << "Counters / العدادات:\n";
        for (const auto& metric : counters) {
            report << "  " << metric->getName() << ": " << metric->getValue();
            if (!metric->getDescription().empty()) {
                report << " (" << metric->getDescription() << ")";
            }
            report << "\n";
        }
        report << "\n";
    }
    
    // القياسات / Gauges
    if (!gauges.empty()) {
        report << "Gauges / القياسات:\n";
        for (const auto& metric : gauges) {
            report << "  " << metric->getName() << ": " << metric->getValue();
            if (!metric->getDescription().empty()) {
                report << " (" << metric->getDescription() << ")";
            }
            report << "\n";
        }
        report << "\n";
    }
    
    // الرسوم البيانية / Histograms
    if (!histograms.empty()) {
        report << "Histograms / الرسوم البيانية:\n";
        for (const auto& metric : histograms) {
            report << "  " << metric->getName() << ":\n";
            report << "    Count: " << metric->getCount() << "\n";
            report << "    Average: " << std::fixed << std::setprecision(2) << metric->getAverage() << "\n";
            report << "    Min: " << metric->getMin() << "\n";
            report << "    Max: " << metric->getMax() << "\n";
        }
        report << "\n";
    }
    
    // المؤقتات / Timers
    if (!timers.empty()) {
        report << "Timers / المؤقتات:\n";
        for (const auto& metric : timers) {
            report << "  " << metric->getName() << ":\n";
            report << "    Count: " << metric->getCount() << "\n";
            report << "    Average: " << formatDuration(metric->getAverage()) << "\n";
            report << "    Min: " << formatDuration(metric->getMin()) << "\n";
            report << "    Max: " << formatDuration(metric->getMax()) << "\n";
            report << "    Total: " << formatDuration(metric->getSum()) << "\n";
        }
        report << "\n";
    }
    
    return report.str();
}

void MetricsCollector::initializePredefinedMetrics() {
    // مقاييس التجميع / Compilation metrics
    registerMetric("jit.compilation.total", MetricType::COUNTER, "Total compilations");
    registerMetric("jit.compilation.success", MetricType::COUNTER, "Successful compilations");
    registerMetric("jit.compilation.failed", MetricType::COUNTER, "Failed compilations");
    registerMetric("jit.compilation.time", MetricType::TIMER, "Compilation time");
    registerMetric("jit.compilation.code_size", MetricType::HISTOGRAM, "Compiled code size");
    
    // مقاييس التخزين المؤقت / Cache metrics
    registerMetric("jit.cache.hits", MetricType::COUNTER, "Cache hits");
    registerMetric("jit.cache.misses", MetricType::COUNTER, "Cache misses");
    registerMetric("jit.cache.evictions", MetricType::COUNTER, "Cache evictions");
    registerMetric("jit.cache.size", MetricType::GAUGE, "Current cache size");
    registerMetric("jit.cache.usage", MetricType::GAUGE, "Cache usage percentage");
    
    // مقاييس التنفيذ / Execution metrics
    registerMetric("jit.execution.total", MetricType::COUNTER, "Total executions");
    registerMetric("jit.execution.time", MetricType::TIMER, "Execution time");
    registerMetric("jit.execution.hotpath", MetricType::COUNTER, "Hot path executions");
    
    // مقاييس الذاكرة / Memory metrics
    registerMetric("jit.memory.allocated", MetricType::GAUGE, "Allocated memory");
    registerMetric("jit.memory.peak", MetricType::GAUGE, "Peak memory usage");
    registerMetric("jit.memory.code", MetricType::GAUGE, "Code memory");
    registerMetric("jit.memory.data", MetricType::GAUGE, "Data memory");
    
    // مقاييس الأداء / Performance metrics
    registerMetric("jit.performance.speedup", MetricType::HISTOGRAM, "JIT speedup vs interpreter");
    registerMetric("jit.performance.throughput", MetricType::GAUGE, "Instructions per second");
}

std::string MetricsCollector::formatDuration(double microseconds) {
    std::ostringstream formatted;
    
    if (microseconds < 1000) {
        formatted << std::fixed << std::setprecision(2) << microseconds << " μs";
    } else if (microseconds < 1000000) {
        formatted << std::fixed << std::setprecision(2) << (microseconds / 1000.0) << " ms";
    } else {
        formatted << std::fixed << std::setprecision(2) << (microseconds / 1000000.0) << " s";
    }
    
    return formatted.str();
}

// ============================================================================
// MetricTimer - Implementation
// ============================================================================

MetricTimer::MetricTimer(const std::string& metric_name)
    : metric_name_(metric_name)
    , start_time_(std::chrono::steady_clock::now())
{
}

MetricTimer::~MetricTimer() {
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time_).count();
    
    MetricsCollector::getInstance().recordTimer(metric_name_, static_cast<double>(duration));
}

void MetricTimer::stop() {
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time_).count();
    
    MetricsCollector::getInstance().recordTimer(metric_name_, static_cast<double>(duration));
    
    // إعادة تعيين وقت البدء لتجنب التسجيل المزدوج / Reset start time to avoid double recording
    start_time_ = end_time;
}

double MetricTimer::elapsed() const {
    auto current_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        current_time - start_time_).count();
    return static_cast<double>(duration);
}

} // namespace JIT
} // namespace Sad
