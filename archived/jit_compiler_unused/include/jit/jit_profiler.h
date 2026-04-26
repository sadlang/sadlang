// ============================================================================
// jit_profiler.h - أداة قياس أداء JIT (JIT Profiler)
// Performance Profiling Tool for JIT Engine
// ============================================================================
// الغرض: جمع وتحليل بيانات الأداء التفصيلية
// Purpose: Collect and analyze detailed performance data
// ============================================================================

#ifndef SAD_JIT_PROFILER_H
#define SAD_JIT_PROFILER_H

#include <memory>        // لإدارة الذاكرة الذكية / For smart pointers
#include <string>        // للنصوص / For strings
#include <unordered_map> // للخرائط السريعة / For fast maps
#include <vector>        // للقوائم الديناميكية / For dynamic arrays
#include <chrono>        // للوقت / For time
#include <mutex>         // للتزامن / For thread safety
#include <fstream>       // للملفات / For files

namespace Sad {
namespace JIT {

// ============================================================================
// ProfileEntry - إدخال قياس الأداء / Profile Entry
// ============================================================================

struct ProfileEntry {
    // ========================================
    // معلومات التوقيت / Timing Information
    // ========================================
    
    std::chrono::steady_clock::time_point start_time; // وقت البدء / Start time
    std::chrono::steady_clock::time_point end_time;   // وقت النهاية / End time
    size_t duration_us;              // المدة (ميكروثانية) / Duration (microseconds)
    
    // ========================================
    // معلومات السياق / Context Information
    // ========================================
    
    std::string function_name;       // اسم الدالة / Function name
    std::string phase;               // المرحلة (compilation, execution, etc.) / Phase
    int thread_id;                   // معرّف الخيط / Thread ID
    
    // ========================================
    // معلومات الموارد / Resource Information
    // ========================================
    
    size_t memory_used_bytes;        // الذاكرة المستخدمة / Memory used
    size_t cpu_cycles;               // دورات المعالج / CPU cycles
    
    // ========================================
    // البيانات الوصفية / Metadata
    // ========================================
    
    std::unordered_map<std::string, std::string> metadata; // بيانات إضافية / Additional data
    
    // ========================================
    // المُنشئ / Constructor
    // ========================================
    
    ProfileEntry()
        : start_time(std::chrono::steady_clock::now())
        , end_time(std::chrono::steady_clock::now())
        , duration_us(0)
        , thread_id(0)
        , memory_used_bytes(0)
        , cpu_cycles(0)
    {}
};

// ============================================================================
// ProfileSummary - ملخص قياس الأداء / Profile Summary
// ============================================================================

struct ProfileSummary {
    // ========================================
    // إحصائيات التوقيت / Timing Statistics
    // ========================================
    
    size_t total_entries;            // إجمالي الإدخالات / Total entries
    size_t total_duration_us;        // المدة الكلية (ميكروثانية) / Total duration
    size_t avg_duration_us;          // متوسط المدة / Average duration
    size_t min_duration_us;          // أقل مدة / Minimum duration
    size_t max_duration_us;          // أقصى مدة / Maximum duration
    
    // ========================================
    // إحصائيات الموارد / Resource Statistics
    // ========================================
    
    size_t total_memory_bytes;       // الذاكرة الكلية / Total memory
    size_t peak_memory_bytes;        // ذروة الذاكرة / Peak memory
    size_t total_cpu_cycles;         // دورات المعالج الكلية / Total CPU cycles
    
    // ========================================
    // معلومات التوزيع / Distribution Information
    // ========================================
    
    std::unordered_map<std::string, size_t> phase_counts;     // عدد الإدخالات لكل مرحلة / Entries per phase
    std::unordered_map<std::string, size_t> phase_durations;  // المدة لكل مرحلة / Duration per phase
    std::unordered_map<std::string, size_t> function_counts;  // عدد الإدخالات لكل دالة / Entries per function
    
    // ========================================
    // المُنشئ / Constructor
    // ========================================
    
    ProfileSummary()
        : total_entries(0)
        , total_duration_us(0)
        , avg_duration_us(0)
        , min_duration_us(0)
        , max_duration_us(0)
        , total_memory_bytes(0)
        , peak_memory_bytes(0)
        , total_cpu_cycles(0)
    {}
};

// ============================================================================
// JITProfiler - أداة قياس الأداء / JIT Profiler
// ============================================================================

class JITProfiler {
public:
    // ========================================
    // المُنشئ والمُدمر / Constructor & Destructor
    // ========================================
    
    JITProfiler();
    ~JITProfiler();
    
    // منع النسخ / Prevent copying
    JITProfiler(const JITProfiler&) = delete;
    JITProfiler& operator=(const JITProfiler&) = delete;
    
    // ========================================
    // Singleton / نمط المفرد
    // ========================================
    
    static JITProfiler& getInstance();
    
    // ========================================
    // إدارة قياس الأداء / Profiling Management
    // ========================================
    
    // تفعيل قياس الأداء / Enable profiling
    void enable();
    
    // تعطيل قياس الأداء / Disable profiling
    void disable();
    
    // هل قياس الأداء مُفعّل؟ / Is profiling enabled?
    bool isEnabled() const;
    
    // إعادة تعيين / Reset
    void reset();
    
    // ========================================
    // إدارة الجلسات / Session Management
    // ========================================
    
    // بدء جلسة / Start session
    void startSession(const std::string& session_name);
    
    // إنهاء جلسة / End session
    void endSession();
    
    // ========================================
    // تسجيل الأحداث / Event Recording
    // ========================================
    
    // تسجيل إدخال / Record entry
    void recordEntry(const ProfileEntry& entry);
    
    // ========================================
    // التحليل والتقارير / Analysis & Reporting
    // ========================================
    
    // الحصول على الملخص / Get summary
    ProfileSummary getSummary() const;
    
    // الحصول على الإدخالات / Get entries (with optional phase filter)
    std::vector<ProfileEntry> getEntries(const std::string& filter_phase = "") const;
    
    // الحصول على أعلى الإدخالات / Get top entries
    std::vector<ProfileEntry> getTopEntries(size_t count, const std::string& sort_by = "duration") const;
    
    // توليد تقرير / Generate report
    std::string generateReport() const;
    
    // ========================================
    // التصدير / Export
    // ========================================
    
    // تصدير إلى ملف JSON / Export to JSON file
    void exportToFile(const std::string& filename) const;
    
    // تصدير إلى Chrome Trace Format / Export to Chrome Trace Format
    void exportToChromeTrace(const std::string& filename) const;

private:
    // ========================================
    // البيانات الداخلية / Internal Data
    // ========================================
    
    mutable std::mutex mutex_;                    // قفل للتزامن / Mutex for thread safety
    bool enabled_;                                // هل قياس الأداء مُفعّل؟ / Is profiling enabled?
    size_t current_session_id_;                   // معرف الجلسة الحالية / Current session ID
    size_t total_entries_;                        // إجمالي الإدخالات / Total entries
    
    std::vector<ProfileEntry> entries_;                           // قائمة الإدخالات / Entries list
    std::unordered_map<size_t, std::string> session_names_;       // أسماء الجلسات / Session names
    
    // ========================================
    // الدوال المساعدة / Helper Functions
    // ========================================
    
    // الحصول على معرّف الخيط / Get thread ID
    static int getCurrentThreadId();
    
    // تهريب JSON / Escape JSON string
    static std::string escapeJson(const std::string& str);
    
    // تنسيق المدة / Format duration
    static std::string formatDuration(size_t microseconds);
    
    // تنسيق البايتات / Format bytes
    static std::string formatBytes(size_t bytes);
};

// ============================================================================
// RAII Helper لقياس الأداء التلقائي / RAII Helper for Automatic Profiling
// ============================================================================

class ProfileScope {
public:
    ProfileScope(
        JITProfiler& profiler,
        const std::string& function_name,
        const std::string& phase
    )
        : profiler_(profiler)
        , entry_()
    {
        if (profiler_.isEnabled()) {
            entry_.function_name = function_name;
            entry_.phase = phase;
            entry_.start_time = std::chrono::steady_clock::now();
        }
    }
    
    ~ProfileScope() {
        if (profiler_.isEnabled()) {
            entry_.end_time = std::chrono::steady_clock::now();
            entry_.duration_us = std::chrono::duration_cast<std::chrono::microseconds>(
                entry_.end_time - entry_.start_time
            ).count();
            profiler_.recordEntry(entry_);
        }
    }
    
    // منع النسخ / Prevent copying
    ProfileScope(const ProfileScope&) = delete;
    ProfileScope& operator=(const ProfileScope&) = delete;

private:
    JITProfiler& profiler_;          // مرجع للأداة / Reference to profiler
    ProfileEntry entry_;             // الإدخال / Entry
};

// ============================================================================
// Macros للاستخدام السهل / Macros for Easy Usage
// ============================================================================

#define PROFILE_SCOPE(profiler, function, phase) \
    ProfileScope _profile_scope_##__LINE__(profiler, function, phase)

#define PROFILE_FUNCTION(profiler) \
    PROFILE_SCOPE(profiler, __FUNCTION__, "execution")

} // namespace JIT
} // namespace Sad

#endif // SAD_JIT_PROFILER_H
