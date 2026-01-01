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
        , min_duration_us(std::numeric_limits<size_t>::max())
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
    // إدارة قياس الأداء / Profiling Management
    // ========================================
    
    // بدء قياس أداء / Start profiling
    void startProfiling();
    
    // إيقاف قياس أداء / Stop profiling
    void stopProfiling();
    
    // هل قياس الأداء مُفعّل؟ / Is profiling enabled?
    bool isProfilingEnabled() const;
    
    // ========================================
    // تسجيل الأحداث / Event Recording
    // ========================================
    
    // بدء حدث / Start event
    // returns: معرّف الحدث / Event ID
    size_t startEvent(
        const std::string& function_name,
        const std::string& phase
    );
    
    // إنهاء حدث / End event
    void endEvent(size_t event_id);
    
    // تسجيل حدث فوري / Record instant event
    void recordEvent(
        const std::string& function_name,
        const std::string& phase,
        size_t duration_us
    );
    
    // ========================================
    // البيانات الوصفية / Metadata
    // ========================================
    
    // إضافة بيانات وصفية لحدث / Add metadata to event
    void addMetadata(
        size_t event_id,
        const std::string& key,
        const std::string& value
    );
    
    // ========================================
    // التحليل والتقارير / Analysis & Reporting
    // ========================================
    
    // الحصول على الملخص / Get summary
    ProfileSummary getSummary() const;
    
    // الحصول على إدخالات المرحلة / Get phase entries
    std::vector<ProfileEntry> getPhaseEntries(const std::string& phase) const;
    
    // الحصول على إدخالات الدالة / Get function entries
    std::vector<ProfileEntry> getFunctionEntries(const std::string& function_name) const;
    
    // ========================================
    // التصدير / Export
    // ========================================
    
    // تصدير إلى JSON / Export to JSON
    std::string toJSON() const;
    
    // حفظ إلى ملف / Save to file
    bool saveToFile(const std::string& filename) const;
    
    // تصدير إلى Chrome Trace Format / Export to Chrome Trace Format
    std::string toChromeTrace() const;
    
    // ========================================
    // الإحصائيات / Statistics
    // ========================================
    
    // الحصول على أبطأ الدوال / Get slowest functions
    std::vector<std::pair<std::string, size_t>> getSlowestFunctions(size_t count = 10) const;
    
    // الحصول على أكثر الدوال استدعاءً / Get most called functions
    std::vector<std::pair<std::string, size_t>> getMostCalledFunctions(size_t count = 10) const;
    
    // الحصول على استهلاك الذاكرة / Get memory usage
    std::unordered_map<std::string, size_t> getMemoryUsageByFunction() const;
    
    // ========================================
    // الإدارة / Management
    // ========================================
    
    // مسح البيانات / Clear data
    void clear();
    
    // إعادة تعيين الإحصائيات / Reset statistics
    void reset();
    
    // الحصول على عدد الإدخالات / Get entry count
    size_t getEntryCount() const;

private:
    // ========================================
    // البيانات الداخلية / Internal Data
    // ========================================
    
    struct Impl;                     // التنفيذ الداخلي (Pimpl idiom) / Internal implementation
    std::unique_ptr<Impl> pimpl_;    // مؤشر ذكي للتنفيذ / Smart pointer to implementation
    
    mutable std::mutex mutex_;       // قفل للتزامن / Mutex for thread safety
    bool enabled_;                   // هل قياس الأداء مُفعّل؟ / Is profiling enabled?
    
    std::vector<ProfileEntry> entries_; // قائمة الإدخالات / Entries list
    std::unordered_map<size_t, ProfileEntry> active_events_; // الأحداث النشطة / Active events
    
    size_t next_event_id_;           // معرّف الحدث التالي / Next event ID
    
    // ========================================
    // الدوال المساعدة / Helper Functions
    // ========================================
    
    // حساب الملخص الداخلي / Calculate internal summary
    void calculateSummary(ProfileSummary& summary) const;
    
    // الحصول على معرّف الخيط / Get thread ID
    int getCurrentThreadId() const;
    
    // الحصول على استخدام الذاكرة / Get memory usage
    size_t getCurrentMemoryUsage() const;
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
        , event_id_(0)
    {
        if (profiler_.isProfilingEnabled()) {
            event_id_ = profiler_.startEvent(function_name, phase);
        }
    }
    
    ~ProfileScope() {
        if (event_id_ > 0) {
            profiler_.endEvent(event_id_);
        }
    }
    
    // منع النسخ / Prevent copying
    ProfileScope(const ProfileScope&) = delete;
    ProfileScope& operator=(const ProfileScope&) = delete;

private:
    JITProfiler& profiler_;          // مرجع للأداة / Reference to profiler
    size_t event_id_;                // معرّف الحدث / Event ID
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

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
// ملاحظات التنفيذ / Implementation Notes:
// - استخدام RAII للقياس التلقائي / Using RAII for automatic profiling
// - Thread-safe باستخدام mutex / Thread-safe using mutex
// - دعم تصدير Chrome Trace Format / Supports Chrome Trace Format export
// - معلومات تفصيلية عن الأداء / Detailed performance information
// ============================================================================
