// ============================================================================
// jit_metrics_new.cpp - تنفيذ مقاييس أداء JIT (JIT Metrics Implementation)
// Performance Metrics Collection for JIT Engine - Implementation
// Rewritten to match jit_metrics.h header
// ============================================================================

#include "jit_metrics.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>

namespace Sad {
namespace JIT {

// ============================================================================
// Metric - Implementation
// Note: Most methods are inline in header
// ============================================================================

std::string Metric::toString() const {
    std::ostringstream oss;
    
    oss << name_ << " [" << typeToString(type_) << "]: ";
    
    switch (type_) {
        case MetricType::COUNTER:
        case MetricType::GAUGE:
            oss << getValue();
            break;
            
        case MetricType::HISTOGRAM:
        case MetricType::TIMER: {
            oss << "count=" << getCount() << ", ";
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

std::string Metric::toJSON() const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"name\": \"" << name_ << "\",\n";
    json << "  \"type\": \"" << typeToString(type_) << "\",\n";
    json << "  \"description\": \"" << description_ << "\",\n";
    
    switch (type_) {
        case MetricType::COUNTER:
        case MetricType::GAUGE:
            json << "  \"value\": " << getValue() << "\n";
            break;
            
        case MetricType::HISTOGRAM:
        case MetricType::TIMER:
            json << "  \"count\": " << getCount() << ",\n";
            json << "  \"sum\": " << getSum() << ",\n";
            json << "  \"average\": " << getAverage() << ",\n";
            json << "  \"min\": " << getMin() << ",\n";
            json << "  \"max\": " << getMax() << "\n";
            break;
    }
    
    json << "}";
    return json.str();
}

void Metric::updateHistogram(double value) {
    // Exponential bucket ranges: [0-1), [1-10), [10-100), ...
    int bucket = 0;
    if (value >= 10000) bucket = 5;
    else if (value >= 1000) bucket = 4;
    else if (value >= 100) bucket = 3;
    else if (value >= 10) bucket = 2;
    else if (value >= 1) bucket = 1;
    
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
    : start_time_(std::chrono::steady_clock::now())
{
}

MetricsCollector::~MetricsCollector() {
}

void MetricsCollector::registerCounter(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (metrics_.find(name) == metrics_.end()) {
        metrics_[name] = std::make_unique<Metric>(name, MetricType::COUNTER, description);
    }
}

void MetricsCollector::registerGauge(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (metrics_.find(name) == metrics_.end()) {
        metrics_[name] = std::make_unique<Metric>(name, MetricType::GAUGE, description);
    }
}

void MetricsCollector::registerHistogram(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (metrics_.find(name) == metrics_.end()) {
        metrics_[name] = std::make_unique<Metric>(name, MetricType::HISTOGRAM, description);
    }
}

void MetricsCollector::registerTimer(const std::string& name, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (metrics_.find(name) == metrics_.end()) {
        metrics_[name] = std::make_unique<Metric>(name, MetricType::TIMER, description);
    }
}

void MetricsCollector::incrementCounter(const std::string& name, double delta) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metrics_.find(name);
    if (it != metrics_.end() && it->second->getType() == MetricType::COUNTER) {
        it->second->increment(delta);
    }
}

void MetricsCollector::setGauge(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metrics_.find(name);
    if (it != metrics_.end() && it->second->getType() == MetricType::GAUGE) {
        it->second->set(value);
    }
}

void MetricsCollector::recordValue(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metrics_.find(name);
    if (it != metrics_.end()) {
        it->second->record(value);
    }
}

void MetricsCollector::recordDuration(const std::string& name, std::chrono::microseconds duration) {
    recordValue(name, static_cast<double>(duration.count()));
}

const Metric* MetricsCollector::getMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metrics_.find(name);
    return (it != metrics_.end()) ? it->second.get() : nullptr;
}

std::vector<const Metric*> MetricsCollector::getAllMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<const Metric*> result;
    result.reserve(metrics_.size());
    for (const auto& [name, metric] : metrics_) {
        result.push_back(metric.get());
    }
    return result;
}

std::vector<const Metric*> MetricsCollector::getMetricsByType(MetricType type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<const Metric*> result;
    for (const auto& [name, metric] : metrics_) {
        if (metric->getType() == type) {
            result.push_back(metric.get());
        }
    }
    return result;
}

// Predefined metrics helpers
void MetricsCollector::recordCompilationTime(std::chrono::microseconds duration) {
    auto* metric = getOrCreateMetric("compilation_time", MetricType::TIMER);
    if (metric) metric->record(static_cast<double>(duration.count()));
}

void MetricsCollector::recordCompilationSuccess() {
    auto* metric = getOrCreateMetric("compilation_success", MetricType::COUNTER);
    if (metric) metric->increment();
}

void MetricsCollector::recordCompilationFailure() {
    auto* metric = getOrCreateMetric("compilation_failure", MetricType::COUNTER);
    if (metric) metric->increment();
}

void MetricsCollector::recordCacheHit() {
    auto* metric = getOrCreateMetric("cache_hit", MetricType::COUNTER);
    if (metric) metric->increment();
}

void MetricsCollector::recordCacheMiss() {
    auto* metric = getOrCreateMetric("cache_miss", MetricType::COUNTER);
    if (metric) metric->increment();
}

void MetricsCollector::recordCacheEviction() {
    auto* metric = getOrCreateMetric("cache_eviction", MetricType::COUNTER);
    if (metric) metric->increment();
}

void MetricsCollector::recordExecutionTime(std::chrono::microseconds duration) {
    auto* metric = getOrCreateMetric("execution_time", MetricType::TIMER);
    if (metric) metric->record(static_cast<double>(duration.count()));
}

void MetricsCollector::recordJITExecution() {
    auto* metric = getOrCreateMetric("jit_execution", MetricType::COUNTER);
    if (metric) metric->increment();
}

void MetricsCollector::recordInterpretedExecution() {
    auto* metric = getOrCreateMetric("interpreted_execution", MetricType::COUNTER);
    if (metric) metric->increment();
}

void MetricsCollector::recordHotPathDetected(const std::string& function_name) {
    auto* metric = getOrCreateMetric("hot_path_detected", MetricType::COUNTER);
    if (metric) metric->increment();
    // Could also track per-function hot paths if needed
}

void MetricsCollector::recordFunctionExecution(const std::string& function_name) {
    std::string metric_name = "function_" + function_name + "_executions";
    auto* metric = getOrCreateMetric(metric_name, MetricType::COUNTER);
    if (metric) metric->increment();
}

void MetricsCollector::printSummary() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "=== JIT Metrics Summary ===" << std::endl;
    std::cout << "Total metrics: " << metrics_.size() << std::endl;
    for (const auto& [name, metric] : metrics_) {
        std::cout << "  " << metric->toString() << std::endl;
    }
    std::cout << "===========================" << std::endl;
}

std::string MetricsCollector::toJSON() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"total_metrics\": " << metrics_.size() << ",\n";
    json << "  \"metrics\": {\n";
    
    size_t count = 0;
    for (const auto& [name, metric] : metrics_) {
        json << "    \"" << name << "\": " << metric->toJSON();
        if (++count < metrics_.size()) json << ",";
        json << "\n";
    }
    
    json << "  }\n";
    json << "}";
    return json.str();
}

bool MetricsCollector::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << toJSON();
    file.close();
    return true;
}

void MetricsCollector::resetAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [name, metric] : metrics_) {
        metric->reset();
    }
}

void MetricsCollector::clearAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.clear();
}

size_t MetricsCollector::getMetricCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_.size();
}

Metric* MetricsCollector::getOrCreateMetric(const std::string& name, MetricType type) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = metrics_.find(name);
    if (it == metrics_.end()) {
        metrics_[name] = std::make_unique<Metric>(name, type);
        return metrics_[name].get();
    }
    return it->second.get();
}

bool MetricsCollector::hasMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_.find(name) != metrics_.end();
}

} // namespace JIT
} // namespace Sad
