// ============================================================================
// test_jit_profiler.cpp - اختبارات JIT Profiler
// JIT Profiler Tests
// ============================================================================
// الغرض: اختبار وظائف قياس الأداء
// Purpose: Test performance profiling functionality
// ============================================================================

#include "../include/jit_profiler.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

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

void simulateWork(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// ============================================================================
// Tests
// ============================================================================

TEST(profiler_enable_disable) {
    JITProfiler& profiler = JITProfiler::getInstance();
    
    // التحقق من التمكين الافتراضي / Check default enabled state
    assert(profiler.isEnabled() && "Profiler should be enabled by default");
    
    // تعطيل / Disable
    profiler.disable();
    assert(!profiler.isEnabled() && "Profiler should be disabled");
    
    // إعادة تمكين / Re-enable
    profiler.enable();
    assert(profiler.isEnabled() && "Profiler should be enabled again");
    
    std::cout << " ✓ Enable/Disable works correctly";
}

TEST(profiler_reset) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    
    // إضافة إدخال / Add entry
    ProfileEntry entry;
    entry.function_name = "test_function";
    entry.phase = "test_phase";
    entry.duration_us = 100;
    profiler.recordEntry(entry);
    
    // التحقق من وجود الإدخال / Verify entry exists
    auto entries = profiler.getEntries();
    assert(entries.size() > 0 && "Should have entries");
    
    // إعادة تعيين / Reset
    profiler.reset();
    entries = profiler.getEntries();
    assert(entries.size() == 0 && "Should have no entries after reset");
    
    std::cout << " ✓ Reset works correctly";
}

TEST(profiler_record_entry) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    // تسجيل إدخال / Record entry
    ProfileEntry entry;
    entry.function_name = "compile_function";
    entry.phase = "compilation";
    entry.thread_id = 123;
    entry.duration_us = 5000;
    entry.memory_used_bytes = 1024;
    entry.cpu_cycles = 10000;
    entry.metadata["code_size"] = "256";
    
    profiler.recordEntry(entry);
    
    // التحقق / Verify
    auto entries = profiler.getEntries();
    assert(entries.size() > 0 && "Should have at least one entry");
    
    const auto& recorded = entries.back();
    assert(recorded.function_name == "compile_function");
    assert(recorded.phase == "compilation");
    assert(recorded.duration_us == 5000);
    assert(recorded.memory_used_bytes == 1024);
    
    std::cout << " ✓ Entry recording works correctly";
}

TEST(profiler_get_entries_filtered) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    // إضافة إدخالات متعددة / Add multiple entries
    ProfileEntry entry1;
    entry1.function_name = "func1";
    entry1.phase = "compilation";
    entry1.duration_us = 1000;
    profiler.recordEntry(entry1);
    
    ProfileEntry entry2;
    entry2.function_name = "func2";
    entry2.phase = "execution";
    entry2.duration_us = 2000;
    profiler.recordEntry(entry2);
    
    ProfileEntry entry3;
    entry3.function_name = "func3";
    entry3.phase = "compilation";
    entry3.duration_us = 3000;
    profiler.recordEntry(entry3);
    
    // الحصول على جميع الإدخالات / Get all entries
    auto all_entries = profiler.getEntries();
    assert(all_entries.size() == 3 && "Should have 3 entries");
    
    // تصفية حسب المرحلة / Filter by phase
    auto compilation_entries = profiler.getEntries("compilation");
    assert(compilation_entries.size() == 2 && "Should have 2 compilation entries");
    
    auto execution_entries = profiler.getEntries("execution");
    assert(execution_entries.size() == 1 && "Should have 1 execution entry");
    
    std::cout << " ✓ Entry filtering works correctly";
}

TEST(profiler_get_top_entries) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    // إضافة إدخالات بمدد مختلفة / Add entries with different durations
    for (int i = 1; i <= 10; ++i) {
        ProfileEntry entry;
        entry.function_name = "func" + std::to_string(i);
        entry.phase = "test";
        entry.duration_us = i * 1000;
        entry.memory_used_bytes = i * 100;
        entry.cpu_cycles = i * 500;
        profiler.recordEntry(entry);
    }
    
    // الحصول على أعلى 5 حسب المدة / Get top 5 by duration
    auto top_duration = profiler.getTopEntries(5, "duration");
    assert(top_duration.size() == 5 && "Should have 5 entries");
    assert(top_duration[0].duration_us > top_duration[1].duration_us && "Should be sorted");
    assert(top_duration[0].duration_us == 10000 && "Longest duration should be first");
    
    // الحصول على أعلى 5 حسب الذاكرة / Get top 5 by memory
    auto top_memory = profiler.getTopEntries(5, "memory");
    assert(top_memory.size() == 5 && "Should have 5 entries");
    assert(top_memory[0].memory_used_bytes == 1000 && "Largest memory should be first");
    
    std::cout << " ✓ Top entries retrieval works correctly";
}

TEST(profiler_get_summary) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    // إضافة إدخالات / Add entries
    for (int i = 1; i <= 5; ++i) {
        ProfileEntry entry;
        entry.function_name = "func" + std::to_string(i);
        entry.phase = (i % 2 == 0) ? "compilation" : "execution";
        entry.duration_us = i * 1000;
        entry.memory_used_bytes = i * 100;
        profiler.recordEntry(entry);
    }
    
    // الحصول على الملخص / Get summary
    auto summary = profiler.getSummary();
    
    assert(summary.total_entries == 5 && "Should have 5 entries");
    assert(summary.total_duration_us == 15000 && "Total duration should be 15000");
    assert(summary.avg_duration_us == 3000 && "Average duration should be 3000");
    assert(summary.min_duration_us == 1000 && "Min duration should be 1000");
    assert(summary.max_duration_us == 5000 && "Max duration should be 5000");
    assert(summary.total_memory_bytes == 1500 && "Total memory should be 1500");
    
    // التحقق من إحصائيات المراحل / Verify phase statistics
    assert(summary.phase_counts.at("compilation") == 2 && "Should have 2 compilation entries");
    assert(summary.phase_counts.at("execution") == 3 && "Should have 3 execution entries");
    
    std::cout << " ✓ Summary generation works correctly";
}

TEST(profiler_session) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    // بدء جلسة / Start session
    profiler.startSession("test_session");
    
    // إضافة بعض الإدخالات / Add some entries
    ProfileEntry entry;
    entry.function_name = "session_func";
    entry.phase = "test";
    entry.duration_us = 1000;
    profiler.recordEntry(entry);
    
    // إنهاء الجلسة / End session
    profiler.endSession();
    
    // التحقق من وجود إدخالات الجلسة / Verify session entries exist
    auto entries = profiler.getEntries();
    assert(entries.size() >= 3 && "Should have at least 3 entries (start, work, end)");
    
    std::cout << " ✓ Session management works correctly";
}

TEST(profile_scope_basic) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    {
        ProfileScope scope("test_function", "test_phase");
        simulateWork(10); // محاكاة عمل / Simulate work
    }
    
    // التحقق من تسجيل الإدخال / Verify entry was recorded
    auto entries = profiler.getEntries();
    assert(entries.size() > 0 && "Should have recorded entry");
    
    const auto& entry = entries.back();
    assert(entry.function_name == "test_function");
    assert(entry.phase == "test_phase");
    assert(entry.duration_us >= 10000 && "Duration should be at least 10ms");
    
    std::cout << " ✓ ProfileScope basic functionality works";
}

TEST(profile_scope_with_metadata) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    {
        ProfileScope scope("metadata_function", "test_phase");
        scope.addMetadata("key1", "value1");
        scope.addMetadata("key2", "value2");
        simulateWork(5);
    }
    
    // التحقق من البيانات الوصفية / Verify metadata
    auto entries = profiler.getEntries();
    assert(entries.size() > 0 && "Should have recorded entry");
    
    const auto& entry = entries.back();
    assert(entry.metadata.count("key1") > 0 && "Should have key1");
    assert(entry.metadata.at("key1") == "value1");
    assert(entry.metadata.count("key2") > 0 && "Should have key2");
    assert(entry.metadata.at("key2") == "value2");
    
    std::cout << " ✓ ProfileScope metadata works correctly";
}

TEST(profile_scope_macro) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    {
        PROFILE_SCOPE("test_phase");
        simulateWork(5);
    }
    
    // التحقق / Verify
    auto entries = profiler.getEntries();
    assert(entries.size() > 0 && "Should have recorded entry");
    
    const auto& entry = entries.back();
    assert(entry.phase == "test_phase");
    
    std::cout << " ✓ PROFILE_SCOPE macro works correctly";
}

TEST(profiler_export_json) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    // إضافة بعض الإدخالات / Add some entries
    for (int i = 0; i < 3; ++i) {
        ProfileEntry entry;
        entry.function_name = "export_func" + std::to_string(i);
        entry.phase = "export_test";
        entry.duration_us = (i + 1) * 1000;
        profiler.recordEntry(entry);
    }
    
    // تصدير إلى ملف / Export to file
    profiler.exportToFile("test_profile.json");
    
    // التحقق من وجود الملف / Verify file exists
    std::ifstream file("test_profile.json");
    assert(file.good() && "Profile file should exist");
    
    // قراءة المحتوى / Read content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    assert(content.find("\"profile_version\": \"1.0\"") != std::string::npos);
    assert(content.find("export_func0") != std::string::npos);
    
    file.close();
    
    std::cout << " ✓ JSON export works correctly";
}

TEST(profiler_export_chrome_trace) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    // إضافة إدخالات / Add entries
    for (int i = 0; i < 3; ++i) {
        ProfileEntry entry;
        entry.function_name = "chrome_func" + std::to_string(i);
        entry.phase = "chrome_test";
        entry.duration_us = (i + 1) * 1000;
        entry.thread_id = 1;
        profiler.recordEntry(entry);
    }
    
    // تصدير إلى Chrome Trace / Export to Chrome Trace
    profiler.exportToChromeTrace("test_trace.json");
    
    // التحقق من وجود الملف / Verify file exists
    std::ifstream file("test_trace.json");
    assert(file.good() && "Trace file should exist");
    
    // قراءة المحتوى / Read content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    assert(content.find("\"traceEvents\"") != std::string::npos);
    assert(content.find("\"ph\": \"X\"") != std::string::npos);
    assert(content.find("chrome_func0") != std::string::npos);
    
    file.close();
    
    std::cout << " ✓ Chrome Trace export works correctly";
}

TEST(profiler_generate_report) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.enable();
    
    // إضافة إدخالات متنوعة / Add varied entries
    for (int i = 0; i < 10; ++i) {
        ProfileEntry entry;
        entry.function_name = "report_func" + std::to_string(i);
        entry.phase = (i < 5) ? "compilation" : "execution";
        entry.duration_us = (i + 1) * 1000;
        profiler.recordEntry(entry);
    }
    
    // توليد التقرير / Generate report
    std::string report = profiler.generateReport();
    
    // التحقق من المحتوى / Verify content
    assert(report.find("JIT Performance Profile Report") != std::string::npos);
    assert(report.find("Total Entries: 10") != std::string::npos);
    assert(report.find("compilation") != std::string::npos);
    assert(report.find("execution") != std::string::npos);
    assert(report.find("Top 10 Functions") != std::string::npos);
    
    std::cout << " ✓ Report generation works correctly";
}

TEST(profiler_disabled_no_recording) {
    JITProfiler& profiler = JITProfiler::getInstance();
    profiler.reset();
    profiler.disable();
    
    size_t initial_count = profiler.getEntries().size();
    
    // محاولة تسجيل إدخال / Try to record entry
    ProfileEntry entry;
    entry.function_name = "disabled_func";
    entry.phase = "test";
    profiler.recordEntry(entry);
    
    // التحقق من عدم التسجيل / Verify not recorded
    size_t final_count = profiler.getEntries().size();
    assert(final_count == initial_count && "Should not record when disabled");
    
    profiler.enable(); // إعادة التمكين للاختبارات الأخرى / Re-enable for other tests
    
    std::cout << " ✓ Disabled profiler doesn't record";
}

// ============================================================================
// Main
// ============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "JIT Profiler Tests" << std::endl;
    std::cout << "اختبارات JIT Profiler" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // الاختبارات تُنفَّذ تلقائياً عند التهيئة / Tests run automatically on initialization
    
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
