// ============================================================================
// jit_profiler.cpp - تنفيذ أداة قياس أداء JIT (JIT Profiler Implementation)
// Performance Profiling Tool for JIT Engine - Implementation
// ============================================================================
// الغرض: تنفيذ جمع وتحليل بيانات الأداء التفصيلية
// Purpose: Implement detailed performance data collection and analysis
// ============================================================================

#include "jit/jit_profiler.h"
#include <algorithm>     // للخوارزميات / For algorithms
#include <numeric>       // للعمليات الرياضية / For numeric operations
#include <sstream>       // لبناء النصوص / For string building
#include <iomanip>       // لتنسيق الإخراج / For output formatting
#include <ctime>         // للوقت / For time
#include <thread>        // للخيوط / For threads

#ifdef _WIN32
#include <windows.h>     // لمعلومات النظام Windows / For Windows system info
#include <psapi.h>       // لمعلومات الذاكرة / For memory info
#pragma comment(lib, "psapi.lib")
// Undefine Windows macros that conflict with std::min/std::max
#undef min
#undef max
#else
#include <sys/resource.h> // لمعلومات الموارد Unix / For Unix resource info
#include <unistd.h>
#endif

namespace Sad {
namespace JIT {

// ============================================================================
// JITProfiler - Implementation
// ============================================================================

JITProfiler::JITProfiler()
    : enabled_(true)
    , current_session_id_(0)
    , total_entries_(0)
{
}

JITProfiler::~JITProfiler() {
    // حفظ البيانات عند التدمير / Save data on destruction
    if (enabled_ && !entries_.empty()) {
        exportToFile("jit_profile_final.json");
    }
}

JITProfiler& JITProfiler::getInstance() {
    static JITProfiler instance;
    return instance;
}

void JITProfiler::enable() {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = true;
}

void JITProfiler::disable() {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = false;
}

bool JITProfiler::isEnabled() const {
    return enabled_;
}

void JITProfiler::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.clear();
    total_entries_ = 0;
    current_session_id_ = 0;
}

void JITProfiler::startSession(const std::string& session_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    current_session_id_++;
    
    // حفظ اسم الجلسة / Save session name
    session_names_[current_session_id_] = session_name;
    
    // تسجيل بداية الجلسة / Record session start
    ProfileEntry entry;
    entry.function_name = "SESSION_START";
    entry.phase = session_name;
    entry.thread_id = getCurrentThreadId();
    entry.start_time = std::chrono::steady_clock::now();
    entry.end_time = entry.start_time;
    entry.duration_us = 0;
    entry.metadata["session_id"] = std::to_string(current_session_id_);
    
    entries_.push_back(entry);
}

void JITProfiler::endSession() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // تسجيل نهاية الجلسة / Record session end
    ProfileEntry entry;
    entry.function_name = "SESSION_END";
    entry.phase = session_names_[current_session_id_];
    entry.thread_id = getCurrentThreadId();
    entry.start_time = std::chrono::steady_clock::now();
    entry.end_time = entry.start_time;
    entry.duration_us = 0;
    entry.metadata["session_id"] = std::to_string(current_session_id_);
    
    entries_.push_back(entry);
}

void JITProfiler::recordEntry(const ProfileEntry& entry) {
    if (!enabled_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.push_back(entry);
    total_entries_++;
}

ProfileSummary JITProfiler::getSummary() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ProfileSummary summary;
    summary.total_entries = entries_.size();
    
    if (entries_.empty()) {
        summary.total_duration_us = 0;
        summary.avg_duration_us = 0;
        summary.min_duration_us = 0;
        summary.max_duration_us = 0;
        summary.total_memory_bytes = 0;
        summary.peak_memory_bytes = 0;
        summary.total_cpu_cycles = 0;
        return summary;
    }
    
    // حساب إحصائيات التوقيت / Calculate timing statistics
    size_t total_duration = 0;
    size_t min_duration = entries_[0].duration_us;
    size_t max_duration = entries_[0].duration_us;
    size_t total_memory = 0;
    size_t peak_memory = 0;
    size_t total_cycles = 0;
    
    for (const auto& entry : entries_) {
        total_duration += entry.duration_us;
        min_duration = std::min(min_duration, entry.duration_us);
        max_duration = std::max(max_duration, entry.duration_us);
        total_memory += entry.memory_used_bytes;
        peak_memory = std::max(peak_memory, entry.memory_used_bytes);
        total_cycles += entry.cpu_cycles;
        
        // إحصائيات المراحل / Phase statistics
        summary.phase_counts[entry.phase]++;
        summary.phase_durations[entry.phase] += entry.duration_us;
        
        // إحصائيات الدوال / Function statistics
        summary.function_counts[entry.function_name]++;
    }
    
    summary.total_duration_us = total_duration;
    summary.avg_duration_us = total_duration / entries_.size();
    summary.min_duration_us = min_duration;
    summary.max_duration_us = max_duration;
    summary.total_memory_bytes = total_memory;
    summary.peak_memory_bytes = peak_memory;
    summary.total_cpu_cycles = total_cycles;
    
    return summary;
}

std::vector<ProfileEntry> JITProfiler::getEntries(const std::string& filter_phase) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (filter_phase.empty()) {
        return entries_;
    }
    
    std::vector<ProfileEntry> filtered;
    for (const auto& entry : entries_) {
        if (entry.phase == filter_phase) {
            filtered.push_back(entry);
        }
    }
    
    return filtered;
}

std::vector<ProfileEntry> JITProfiler::getTopEntries(size_t count, const std::string& sort_by) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<ProfileEntry> sorted = entries_;
    
    // ترتيب حسب المعيار المحدد / Sort by specified criterion
    if (sort_by == "duration") {
        std::sort(sorted.begin(), sorted.end(), [](const ProfileEntry& a, const ProfileEntry& b) {
            return a.duration_us > b.duration_us;
        });
    } else if (sort_by == "memory") {
        std::sort(sorted.begin(), sorted.end(), [](const ProfileEntry& a, const ProfileEntry& b) {
            return a.memory_used_bytes > b.memory_used_bytes;
        });
    } else if (sort_by == "cpu") {
        std::sort(sorted.begin(), sorted.end(), [](const ProfileEntry& a, const ProfileEntry& b) {
            return a.cpu_cycles > b.cpu_cycles;
        });
    }
    
    // إرجاع أعلى N إدخالات / Return top N entries
    if (sorted.size() > count) {
        sorted.resize(count);
    }
    
    return sorted;
}

void JITProfiler::exportToFile(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "{\n";
    file << "  \"profile_version\": \"1.0\",\n";
    file << "  \"total_entries\": " << entries_.size() << ",\n";
    
    // ملخص الأداء / Performance summary
    ProfileSummary summary = getSummary();
    file << "  \"summary\": {\n";
    file << "    \"total_duration_us\": " << summary.total_duration_us << ",\n";
    file << "    \"avg_duration_us\": " << summary.avg_duration_us << ",\n";
    file << "    \"min_duration_us\": " << summary.min_duration_us << ",\n";
    file << "    \"max_duration_us\": " << summary.max_duration_us << ",\n";
    file << "    \"total_memory_bytes\": " << summary.total_memory_bytes << ",\n";
    file << "    \"peak_memory_bytes\": " << summary.peak_memory_bytes << ",\n";
    file << "    \"total_cpu_cycles\": " << summary.total_cpu_cycles << "\n";
    file << "  },\n";
    
    // الإدخالات / Entries
    file << "  \"entries\": [\n";
    for (size_t i = 0; i < entries_.size(); ++i) {
        const auto& entry = entries_[i];
        file << "    {\n";
        file << "      \"function\": \"" << escapeJson(entry.function_name) << "\",\n";
        file << "      \"phase\": \"" << escapeJson(entry.phase) << "\",\n";
        file << "      \"thread_id\": " << entry.thread_id << ",\n";
        file << "      \"duration_us\": " << entry.duration_us << ",\n";
        file << "      \"memory_bytes\": " << entry.memory_used_bytes << ",\n";
        file << "      \"cpu_cycles\": " << entry.cpu_cycles << ",\n";
        
        // البيانات الوصفية / Metadata
        file << "      \"metadata\": {\n";
        size_t metadata_count = 0;
        for (const auto& [key, value] : entry.metadata) {
            file << "        \"" << escapeJson(key) << "\": \"" << escapeJson(value) << "\"";
            if (++metadata_count < entry.metadata.size()) {
                file << ",";
            }
            file << "\n";
        }
        file << "      }\n";
        
        file << "    }";
        if (i < entries_.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "  ]\n";
    file << "}\n";
    
    file.close();
}

void JITProfiler::exportToChromeTrace(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    // تنسيق Chrome Trace Event / Chrome Trace Event format
    // https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/
    
    file << "{\n";
    file << "  \"displayTimeUnit\": \"ms\",\n";
    file << "  \"traceEvents\": [\n";
    
    // حساب الوقت الأساسي / Calculate base time
    auto base_time = std::chrono::steady_clock::now();
    if (!entries_.empty()) {
        base_time = entries_[0].start_time;
    }
    
    for (size_t i = 0; i < entries_.size(); ++i) {
        const auto& entry = entries_[i];
        
        // حساب الوقت النسبي / Calculate relative time
        auto start_offset = std::chrono::duration_cast<std::chrono::microseconds>(
            entry.start_time - base_time).count();
        
        file << "    {\n";
        file << "      \"name\": \"" << escapeJson(entry.function_name) << "\",\n";
        file << "      \"cat\": \"" << escapeJson(entry.phase) << "\",\n";
        file << "      \"ph\": \"X\",\n";  // Complete event
        file << "      \"ts\": " << start_offset << ",\n";
        file << "      \"dur\": " << entry.duration_us << ",\n";
        file << "      \"pid\": 1,\n";
        file << "      \"tid\": " << entry.thread_id << ",\n";
        
        // الأرغيومنتات / Arguments
        file << "      \"args\": {\n";
        file << "        \"memory_bytes\": " << entry.memory_used_bytes << ",\n";
        file << "        \"cpu_cycles\": " << entry.cpu_cycles;
        
        // البيانات الوصفية / Metadata
        for (const auto& [key, value] : entry.metadata) {
            file << ",\n";
            file << "        \"" << escapeJson(key) << "\": \"" << escapeJson(value) << "\"";
        }
        
        file << "\n      }\n";
        file << "    }";
        
        if (i < entries_.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
}

std::string JITProfiler::generateReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream report;
    
    report << "========================================\n";
    report << "JIT Performance Profile Report\n";
    report << "تقرير قياس أداء JIT\n";
    report << "========================================\n\n";
    
    // ملخص عام / General summary
    ProfileSummary summary = getSummary();
    report << "General Statistics / الإحصائيات العامة:\n";
    report << "  Total Entries: " << summary.total_entries << "\n";
    report << "  Total Duration: " << formatDuration(summary.total_duration_us) << "\n";
    report << "  Average Duration: " << formatDuration(summary.avg_duration_us) << "\n";
    report << "  Min Duration: " << formatDuration(summary.min_duration_us) << "\n";
    report << "  Max Duration: " << formatDuration(summary.max_duration_us) << "\n";
    report << "  Total Memory: " << formatBytes(summary.total_memory_bytes) << "\n";
    report << "  Peak Memory: " << formatBytes(summary.peak_memory_bytes) << "\n";
    report << "  Total CPU Cycles: " << summary.total_cpu_cycles << "\n\n";
    
    // إحصائيات المراحل / Phase statistics
    report << "Phase Statistics / إحصائيات المراحل:\n";
    for (const auto& [phase, count] : summary.phase_counts) {
        size_t duration = summary.phase_durations.at(phase);
        double percentage = (summary.total_duration_us > 0) 
            ? (duration * 100.0 / summary.total_duration_us) 
            : 0.0;
        
        report << "  " << phase << ":\n";
        report << "    Count: " << count << "\n";
        report << "    Duration: " << formatDuration(duration);
        report << " (" << std::fixed << std::setprecision(2) << percentage << "%)\n";
    }
    report << "\n";
    
    // أعلى 10 دوال / Top 10 functions
    report << "Top 10 Functions by Duration / أعلى 10 دوال حسب المدة:\n";
    auto top_entries = getTopEntries(10, "duration");
    for (size_t i = 0; i < top_entries.size(); ++i) {
        const auto& entry = top_entries[i];
        report << "  " << (i + 1) << ". " << entry.function_name;
        report << " (" << entry.phase << ")";
        report << ": " << formatDuration(entry.duration_us) << "\n";
    }
    
    return report.str();
}

// ============================================================================
// ProfileScope - Implementation (matches header with JITProfiler& parameter)
// ============================================================================
// Note: ProfileScope is now inline-defined in the header, so we don't need
// implementation here. The previous version used a different signature.
// ============================================================================

// If we need standalone ProfileScope (without profiler ref), we can add it:
// This is kept for backward compatibility with existing code that might use it

// ============================================================================
// Helper Functions - Implementation
// ============================================================================

int JITProfiler::getCurrentThreadId() {
#ifdef _WIN32
    return static_cast<int>(GetCurrentThreadId());
#else
    return static_cast<int>(pthread_self());
#endif
}

std::string JITProfiler::escapeJson(const std::string& str) {
    std::ostringstream escaped;
    for (char c : str) {
        switch (c) {
            case '"':  escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\b': escaped << "\\b";  break;
            case '\f': escaped << "\\f";  break;
            case '\n': escaped << "\\n";  break;
            case '\r': escaped << "\\r";  break;
            case '\t': escaped << "\\t";  break;
            default:
                if (c >= 0 && c < 32) {
                    escaped << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    escaped << c;
                }
        }
    }
    return escaped.str();
}

std::string JITProfiler::formatDuration(size_t microseconds) {
    std::ostringstream formatted;
    
    if (microseconds < 1000) {
        formatted << microseconds << " μs";
    } else if (microseconds < 1000000) {
        formatted << std::fixed << std::setprecision(2) << (microseconds / 1000.0) << " ms";
    } else {
        formatted << std::fixed << std::setprecision(2) << (microseconds / 1000000.0) << " s";
    }
    
    return formatted.str();
}

std::string JITProfiler::formatBytes(size_t bytes) {
    std::ostringstream formatted;
    
    if (bytes < 1024) {
        formatted << bytes << " B";
    } else if (bytes < 1024 * 1024) {
        formatted << std::fixed << std::setprecision(2) << (bytes / 1024.0) << " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        formatted << std::fixed << std::setprecision(2) << (bytes / (1024.0 * 1024.0)) << " MB";
    } else {
        formatted << std::fixed << std::setprecision(2) << (bytes / (1024.0 * 1024.0 * 1024.0)) << " GB";
    }
    
    return formatted.str();
}

} // namespace JIT
} // namespace Sad
