// ============================================================================
// test_jit_metrics.cpp - اختبارات JIT Metrics
// JIT Metrics Tests
// ============================================================================
// الغرض: اختبار وظائف جمع المقاييس
// Purpose: Test metrics collection functionality
// ============================================================================

#include "../include/jit_metrics.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>
#include <cmath>

using namespace Sad::JIT;

// ============================================================================
// Test Framework
// ============================================================================

int test_count = 0;
int passed_count = 0;

#define TEST(name) \
    void test_##name(); \
    struct Test_##name { \
        Test_##name() { \
            test_count++; \
            std::cout << "Running test: " << #name << "..."; \
            try { \
                test_##name(); \
                passed_count++; \
                std::cout << " PASSED" << std::endl; \
            } catch (const std::exception& e) { \
                std::cout << " FAILED: " << e.what() << std::endl; \
            } catch (...) { \
                std::cout << " FAILED: Unknown exception" << std::endl; \
            } \
        } \
    } test_instance_##name; \
    void test_##name()

// ============================================================================
// Helper Functions
// ============================================================================

bool approxEqual(double a, double b, double epsilon = 0.001) {
    return std::abs(a - b) < epsilon;
}

// ============================================================================
// Metric Tests
// ============================================================================

TEST(metric_counter_basic) {
    Metric counter("test_counter", MetricType::COUNTER, "Test counter");
    
    assert(counter.getName() == "test_counter");
    assert(counter.getType() == MetricType::COUNTER);
    assert(counter.getValue() == 0.0);
    
    counter.increment();
    assert(counter.getValue() == 1.0);
    
    counter.increment(5.0);
    assert(counter.getValue() == 6.0);
    
    std::cout << " ✓ Counter basic operations work";
}

TEST(metric_gauge_basic) {
    Metric gauge("test_gauge", MetricType::GAUGE, "Test gauge");
    
    assert(gauge.getValue() == 0.0);
    
    gauge.set(100.0);
    assert(gauge.getValue() == 100.0);
    
    gauge.set(50.0);
    assert(gauge.getValue() == 50.0);
    
    std::cout << " ✓ Gauge basic operations work";
}

TEST(metric_histogram_basic) {
    Metric histogram("test_histogram", MetricType::HISTOGRAM, "Test histogram");
    
    histogram.record(10.0);
    histogram.record(20.0);
    histogram.record(30.0);
    
    assert(histogram.getCount() == 3);
    assert(approxEqual(histogram.getSum(), 60.0));
    assert(approxEqual(histogram.getAverage(), 20.0));
    assert(approxEqual(histogram.getMin(), 10.0));
    assert(approxEqual(histogram.getMax(), 30.0));
    
    std::cout << " ✓ Histogram basic operations work";
}

TEST(metric_timer_basic) {
    Metric timer("test_timer", MetricType::TIMER, "Test timer");
    
    timer.record(1000.0);  // 1ms
    timer.record(2000.0);  // 2ms
    timer.record(3000.0);  // 3ms
    
    assert(timer.getCount() == 3);
    assert(approxEqual(timer.getSum(), 6000.0));
    assert(approxEqual(timer.getAverage(), 2000.0));
    assert(approxEqual(timer.getMin(), 1000.0));
    assert(approxEqual(timer.getMax(), 3000.0));
    
    std::cout << " ✓ Timer basic operations work";
}

TEST(metric_reset) {
    Metric counter("reset_counter", MetricType::COUNTER);
    
    counter.increment(10);
    assert(counter.getValue() == 10.0);
    
    counter.reset();
    assert(counter.getValue() == 0.0);
    
    std::cout << " ✓ Metric reset works correctly";
}

TEST(metric_to_string) {
    Metric counter("string_counter", MetricType::COUNTER, "Test description");
    counter.increment(42);
    
    std::string str = counter.toString();
    assert(str.find("string_counter") != std::string::npos);
    assert(str.find("Counter") != std::string::npos);
    assert(str.find("42") != std::string::npos);
    
    std::cout << " ✓ Metric toString works correctly";
}

// ============================================================================
// MetricsCollector Tests
// ============================================================================

TEST(collector_enable_disable) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    
    assert(collector.isEnabled() && "Collector should be enabled by default");
    
    collector.disable();
    assert(!collector.isEnabled());
    
    collector.enable();
    assert(collector.isEnabled());
    
    std::cout << " ✓ Collector enable/disable works";
}

TEST(collector_register_metric) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    
    collector.registerMetric("custom.counter", MetricType::COUNTER, "Custom counter");
    
    auto metric = collector.getMetric("custom.counter");
    assert(metric != nullptr && "Metric should exist");
    assert(metric->getName() == "custom.counter");
    assert(metric->getType() == MetricType::COUNTER);
    
    std::cout << " ✓ Metric registration works";
}

TEST(collector_increment_counter) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    
    collector.registerMetric("test.counter", MetricType::COUNTER);
    
    collector.incrementCounter("test.counter", 1.0);
    collector.incrementCounter("test.counter", 5.0);
    
    auto metric = collector.getMetric("test.counter");
    assert(metric != nullptr);
    assert(metric->getValue() == 6.0);
    
    std::cout << " ✓ Counter increment works";
}

TEST(collector_set_gauge) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    
    collector.registerMetric("test.gauge", MetricType::GAUGE);
    
    collector.setGauge("test.gauge", 100.0);
    auto metric = collector.getMetric("test.gauge");
    assert(metric != nullptr);
    assert(metric->getValue() == 100.0);
    
    collector.setGauge("test.gauge", 200.0);
    assert(metric->getValue() == 200.0);
    
    std::cout << " ✓ Gauge set works";
}

TEST(collector_record_histogram) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    
    collector.registerMetric("test.histogram", MetricType::HISTOGRAM);
    
    collector.recordHistogram("test.histogram", 10.0);
    collector.recordHistogram("test.histogram", 20.0);
    collector.recordHistogram("test.histogram", 30.0);
    
    auto metric = collector.getMetric("test.histogram");
    assert(metric != nullptr);
    assert(metric->getCount() == 3);
    assert(approxEqual(metric->getAverage(), 20.0));
    
    std::cout << " ✓ Histogram record works";
}

TEST(collector_record_timer) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    
    collector.registerMetric("test.timer", MetricType::TIMER);
    
    collector.recordTimer("test.timer", 1000.0);
    collector.recordTimer("test.timer", 2000.0);
    
    auto metric = collector.getMetric("test.timer");
    assert(metric != nullptr);
    assert(metric->getCount() == 2);
    assert(approxEqual(metric->getSum(), 3000.0));
    
    std::cout << " ✓ Timer record works";
}

TEST(collector_get_all_metrics) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    
    collector.registerMetric("metric1", MetricType::COUNTER);
    collector.registerMetric("metric2", MetricType::GAUGE);
    collector.registerMetric("metric3", MetricType::HISTOGRAM);
    
    auto metrics = collector.getAllMetrics();
    assert(metrics.size() >= 3 && "Should have at least 3 custom metrics");
    
    std::cout << " ✓ Get all metrics works";
}

TEST(collector_reset) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    
    collector.registerMetric("reset.counter", MetricType::COUNTER);
    collector.incrementCounter("reset.counter", 10);
    
    auto metric = collector.getMetric("reset.counter");
    assert(metric->getValue() == 10.0);
    
    collector.reset();
    assert(metric->getValue() == 0.0 && "Metric should be reset");
    
    std::cout << " ✓ Collector reset works";
}

TEST(collector_reset_single_metric) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    
    collector.registerMetric("counter1", MetricType::COUNTER);
    collector.registerMetric("counter2", MetricType::COUNTER);
    
    collector.incrementCounter("counter1", 10);
    collector.incrementCounter("counter2", 20);
    
    collector.resetMetric("counter1");
    
    assert(collector.getMetric("counter1")->getValue() == 0.0);
    assert(collector.getMetric("counter2")->getValue() == 20.0);
    
    std::cout << " ✓ Single metric reset works";
}

TEST(collector_disabled_no_recording) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.registerMetric("disabled.counter", MetricType::COUNTER);
    collector.disable();
    
    collector.incrementCounter("disabled.counter", 10);
    
    auto metric = collector.getMetric("disabled.counter");
    assert(metric->getValue() == 0.0 && "Should not record when disabled");
    
    collector.enable();
    std::cout << " ✓ Disabled collector doesn't record";
}

TEST(collector_predefined_metrics) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    
    // التحقق من وجود المقاييس المحددة مسبقاً / Check predefined metrics exist
    auto compilation_total = collector.getMetric("jit.compilation.total");
    assert(compilation_total != nullptr && "Compilation total should exist");
    
    auto cache_hits = collector.getMetric("jit.cache.hits");
    assert(cache_hits != nullptr && "Cache hits should exist");
    
    auto execution_time = collector.getMetric("jit.execution.time");
    assert(execution_time != nullptr && "Execution time should exist");
    
    auto memory_allocated = collector.getMetric("jit.memory.allocated");
    assert(memory_allocated != nullptr && "Memory allocated should exist");
    
    std::cout << " ✓ Predefined metrics exist";
}

TEST(collector_export_json) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    
    collector.registerMetric("export.counter", MetricType::COUNTER);
    collector.incrementCounter("export.counter", 42);
    
    collector.exportToFile("test_metrics.json");
    
    std::ifstream file("test_metrics.json");
    assert(file.good() && "Metrics file should exist");
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    assert(content.find("\"metrics_version\": \"1.0\"") != std::string::npos);
    assert(content.find("export.counter") != std::string::npos);
    
    file.close();
    
    std::cout << " ✓ JSON export works";
}

TEST(collector_generate_report) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    
    collector.registerMetric("report.counter", MetricType::COUNTER);
    collector.registerMetric("report.gauge", MetricType::GAUGE);
    collector.registerMetric("report.timer", MetricType::TIMER);
    
    collector.incrementCounter("report.counter", 10);
    collector.setGauge("report.gauge", 50.0);
    collector.recordTimer("report.timer", 1000.0);
    
    std::string report = collector.generateReport();
    
    assert(report.find("JIT Metrics Report") != std::string::npos);
    assert(report.find("report.counter") != std::string::npos);
    assert(report.find("report.gauge") != std::string::npos);
    assert(report.find("report.timer") != std::string::npos);
    
    std::cout << " ✓ Report generation works";
}

// ============================================================================
// MetricTimer Tests
// ============================================================================

TEST(metric_timer_raii) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    collector.registerMetric("raii.timer", MetricType::TIMER);
    
    {
        MetricTimer timer("raii.timer");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    auto metric = collector.getMetric("raii.timer");
    assert(metric != nullptr);
    assert(metric->getCount() == 1);
    assert(metric->getAverage() >= 10000.0); // At least 10ms in microseconds
    
    std::cout << " ✓ MetricTimer RAII works";
}

TEST(metric_timer_manual_stop) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    collector.registerMetric("manual.timer", MetricType::TIMER);
    
    MetricTimer timer("manual.timer");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    timer.stop();
    
    auto metric = collector.getMetric("manual.timer");
    assert(metric != nullptr);
    assert(metric->getCount() == 1);
    
    std::cout << " ✓ MetricTimer manual stop works";
}

TEST(metric_timer_elapsed) {
    MetricTimer timer("elapsed.timer");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    double elapsed = timer.elapsed();
    assert(elapsed >= 10000.0 && "Elapsed time should be at least 10ms");
    
    std::cout << " ✓ MetricTimer elapsed works";
}

TEST(metric_timer_macro) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    collector.registerMetric("macro.timer", MetricType::TIMER);
    
    {
        METRIC_TIMER("macro.timer");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
    auto metric = collector.getMetric("macro.timer");
    assert(metric != nullptr);
    assert(metric->getCount() == 1);
    
    std::cout << " ✓ METRIC_TIMER macro works";
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(integration_jit_workflow) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    
    // محاكاة سير عمل JIT / Simulate JIT workflow
    
    // 1. Compilation
    collector.incrementCounter("jit.compilation.total");
    {
        METRIC_TIMER("jit.compilation.time");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    collector.incrementCounter("jit.compilation.success");
    collector.recordHistogram("jit.compilation.code_size", 1024.0);
    
    // 2. Cache
    collector.incrementCounter("jit.cache.misses");
    collector.setGauge("jit.cache.size", 1.0);
    
    // 3. Execution
    collector.incrementCounter("jit.execution.total");
    {
        METRIC_TIMER("jit.execution.time");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    
    // 4. Memory
    collector.setGauge("jit.memory.allocated", 4096.0);
    
    // التحقق / Verify
    assert(collector.getMetric("jit.compilation.total")->getValue() == 1.0);
    assert(collector.getMetric("jit.compilation.success")->getValue() == 1.0);
    assert(collector.getMetric("jit.cache.misses")->getValue() == 1.0);
    assert(collector.getMetric("jit.execution.total")->getValue() == 1.0);
    assert(collector.getMetric("jit.memory.allocated")->getValue() == 4096.0);
    
    auto compilation_time = collector.getMetric("jit.compilation.time");
    assert(compilation_time->getCount() == 1);
    
    auto execution_time = collector.getMetric("jit.execution.time");
    assert(execution_time->getCount() == 1);
    
    std::cout << " ✓ JIT workflow integration works";
}

TEST(integration_concurrent_updates) {
    MetricsCollector& collector = MetricsCollector::getInstance();
    collector.reset();
    collector.enable();
    collector.registerMetric("concurrent.counter", MetricType::COUNTER);
    
    // تحديثات متزامنة / Concurrent updates
    std::thread t1([&]() {
        for (int i = 0; i < 100; ++i) {
            collector.incrementCounter("concurrent.counter");
        }
    });
    
    std::thread t2([&]() {
        for (int i = 0; i < 100; ++i) {
            collector.incrementCounter("concurrent.counter");
        }
    });
    
    t1.join();
    t2.join();
    
    auto metric = collector.getMetric("concurrent.counter");
    assert(metric->getValue() == 200.0 && "Should handle concurrent updates");
    
    std::cout << " ✓ Concurrent updates work";
}

// ============================================================================
// Main
// ============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "JIT Metrics Tests" << std::endl;
    std::cout << "اختبارات JIT Metrics" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // الاختبارات تُنفَّذ تلقائياً / Tests run automatically
    
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Test Results / نتائج الاختبارات" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total tests: " << test_count << std::endl;
    std::cout << "Passed: " << passed_count << std::endl;
    std::cout << "Failed: " << (test_count - passed_count) << std::endl;
    std::cout << "Success rate: " << (passed_count * 100 / test_count) << "%" << std::endl;
    
    return (test_count == passed_count) ? 0 : 1;
}
