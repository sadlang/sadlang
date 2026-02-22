// ==============================================================================
// profiler.h - نظام تحليل الأداء / Performance Profiler
// ==============================================================================
// الوصف: أدوات قياس وتحليل أداء التطبيق والألعاب
// Description: Tools for measuring and analyzing application/game performance
// ==============================================================================
// المميزات / Features:
//   - قياس وقت التنفيذ / Execution time measurement
//   - تتبع استدعاءات الدوال / Function call tracing
//   - إحصائيات الإطار / Frame statistics
//   - تتبع الذاكرة / Memory tracking
//   - تصدير التقارير / Report export
//   - عرض مرئي / Visual display
// ==============================================================================
// الاستخدام / Usage:
//   // بدء قياس منطقة / Start measuring a scope
//   PROFILE_SCOPE("Update Physics");
//
//   // أو يدوياً / Or manually
//   Profiler::GetInstance().BeginSample("Render");
//   // ... الكود / code ...
//   Profiler::GetInstance().EndSample();
//
//   // عرض الإحصائيات / Display statistics
//   Profiler::GetInstance().PrintReport();
// ==============================================================================

#ifndef SAD_GRAPHICS_PROFILER_H
#define SAD_GRAPHICS_PROFILER_H

#include "types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <stack>
#include <mutex>
#include <sstream>

namespace sad {
namespace graphics {

// ==============================================================================
// الثوابت / Constants
// ==============================================================================

/// أقصى عدد للإطارات المحفوظة / Maximum saved frames
constexpr size_t PROFILER_MAX_FRAMES = 120;

/// أقصى عمق للتتبع / Maximum trace depth
constexpr size_t PROFILER_MAX_DEPTH = 32;

// ==============================================================================
// مؤقت عالي الدقة / High Resolution Timer
// ==============================================================================

/// مؤقت عالي الدقة للقياسات
/// High resolution timer for measurements
class HighResTimer {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    using Duration = std::chrono::duration<Float64, std::micro>; // ميكروثانية
    
    /// الوقت الحالي / Current time
    static TimePoint Now() { return Clock::now(); }
    
    /// الفرق بالميكروثانية / Difference in microseconds
    static Float64 DiffMicroseconds(TimePoint start, TimePoint end) {
        return std::chrono::duration_cast<Duration>(end - start).count();
    }
    
    /// الفرق بالملّي ثانية / Difference in milliseconds
    static Float64 DiffMilliseconds(TimePoint start, TimePoint end) {
        return DiffMicroseconds(start, end) / 1000.0;
    }
};

// ==============================================================================
// عينة الأداء / Profile Sample
// ==============================================================================

/// عينة أداء واحدة
/// Single profile sample
struct ProfileSample {
    std::string name;           // اسم العينة / Sample name
    Float64 startTime = 0.0;    // وقت البدء / Start time
    Float64 endTime = 0.0;      // وقت الانتهاء / End time
    Float64 duration = 0.0;     // المدة بالملّي ثانية / Duration in ms
    Int32 depth = 0;            // العمق في التسلسل / Depth in hierarchy
    Int32 callCount = 1;        // عدد الاستدعاءات / Call count
    
    // المجاميع للإحصائيات / Aggregates for statistics
    Float64 totalTime = 0.0;    // الوقت الإجمالي / Total time
    Float64 minTime = 1e9;      // أقل وقت / Minimum time
    Float64 maxTime = 0.0;      // أعلى وقت / Maximum time
};

// ==============================================================================
// إحصائيات الإطار / Frame Statistics
// ==============================================================================

/// إحصائيات إطار واحد
/// Single frame statistics
struct FrameStats {
    UInt64 frameNumber = 0;         // رقم الإطار / Frame number
    Float64 totalTime = 0.0;        // الوقت الكلي / Total time
    Float64 updateTime = 0.0;       // وقت التحديث / Update time
    Float64 renderTime = 0.0;       // وقت الرسم / Render time
    Float64 physicsTime = 0.0;      // وقت الفيزياء / Physics time
    Float64 audioTime = 0.0;        // وقت الصوت / Audio time
    Int32 drawCalls = 0;            // عدد استدعاءات الرسم / Draw calls
    Int32 triangles = 0;            // عدد المثلثات / Triangle count
    Int32 vertices = 0;             // عدد الرؤوس / Vertex count
    size_t memoryUsed = 0;          // الذاكرة المستخدمة / Memory used
    
    std::vector<ProfileSample> samples;  // العينات / Samples
};

// ==============================================================================
// تتبع الذاكرة / Memory Tracker
// ==============================================================================

/// تتبع استخدام الذاكرة
/// Track memory usage
class MemoryTracker {
public:
    /// تسجيل تخصيص / Record allocation
    void RecordAllocation(size_t size, const std::string& tag = "") {
        m_totalAllocated += size;
        m_currentUsage += size;
        m_allocationCount++;
        
        if (m_currentUsage > m_peakUsage) {
            m_peakUsage = m_currentUsage;
        }
        
        if (!tag.empty()) {
            m_taggedUsage[tag] += size;
        }
    }
    
    /// تسجيل تحرير / Record deallocation
    void RecordDeallocation(size_t size, const std::string& tag = "") {
        if (size <= m_currentUsage) {
            m_currentUsage -= size;
        }
        m_deallocationCount++;
        
        if (!tag.empty() && m_taggedUsage.count(tag)) {
            m_taggedUsage[tag] -= size;
        }
    }
    
    /// إعادة تعيين / Reset
    void Reset() {
        m_totalAllocated = 0;
        m_currentUsage = 0;
        m_peakUsage = 0;
        m_allocationCount = 0;
        m_deallocationCount = 0;
        m_taggedUsage.clear();
    }
    
    // الإحصائيات / Statistics
    size_t GetTotalAllocated() const { return m_totalAllocated; }
    size_t GetCurrentUsage() const { return m_currentUsage; }
    size_t GetPeakUsage() const { return m_peakUsage; }
    UInt64 GetAllocationCount() const { return m_allocationCount; }
    UInt64 GetDeallocationCount() const { return m_deallocationCount; }
    
    /// الحصول على استخدام علامة / Get tagged usage
    size_t GetTaggedUsage(const std::string& tag) const {
        auto it = m_taggedUsage.find(tag);
        return (it != m_taggedUsage.end()) ? it->second : 0;
    }
    
    /// جميع العلامات / All tags
    const std::unordered_map<std::string, size_t>& GetAllTagged() const {
        return m_taggedUsage;
    }

private:
    size_t m_totalAllocated = 0;
    size_t m_currentUsage = 0;
    size_t m_peakUsage = 0;
    UInt64 m_allocationCount = 0;
    UInt64 m_deallocationCount = 0;
    std::unordered_map<std::string, size_t> m_taggedUsage;
};

// ==============================================================================
// المُحلل / Profiler
// ==============================================================================

/// المُحلل الرئيسي للأداء
/// Main Performance Profiler
/// 
/// يقيس ويجمع إحصائيات الأداء للتطبيق
/// يدعم التتبع الهرمي والإحصائيات التراكمية
class Profiler {
public:
    // ==========================================================================
    // Singleton
    // ==========================================================================
    
    /// الحصول على المثيل / Get instance
    static Profiler& GetInstance() {
        static Profiler instance;
        return instance;
    }
    
    // ==========================================================================
    // التحكم / Control
    // ==========================================================================
    
    /// تفعيل/تعطيل التحليل / Enable/disable profiling
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    
    // ==========================================================================
    // أخذ العينات / Sampling
    // ==========================================================================
    
    /// بدء عينة / Begin sample
    void BeginSample(const std::string& name) {
        if (!m_enabled) return;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        ProfileSample sample;
        sample.name = name;
        sample.startTime = GetCurrentTimeMs();
        sample.depth = static_cast<Int32>(m_sampleStack.size());
        
        m_sampleStack.push(m_currentFrame.samples.size());
        m_currentFrame.samples.push_back(sample);
    }
    
    /// إنهاء عينة / End sample
    void EndSample() {
        if (!m_enabled) return;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_sampleStack.empty()) return;
        
        size_t index = m_sampleStack.top();
        m_sampleStack.pop();
        
        if (index < m_currentFrame.samples.size()) {
            auto& sample = m_currentFrame.samples[index];
            sample.endTime = GetCurrentTimeMs();
            sample.duration = sample.endTime - sample.startTime;
            
            // تحديث الإحصائيات التراكمية / Update cumulative stats
            UpdateStats(sample.name, sample.duration);
        }
    }
    
    // ==========================================================================
    // إدارة الإطارات / Frame Management
    // ==========================================================================
    
    /// بدء إطار جديد / Begin new frame
    void BeginFrame() {
        if (!m_enabled) return;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_frameStartTime = HighResTimer::Now();
        m_currentFrame = FrameStats{};
        m_currentFrame.frameNumber = m_frameCount;
        
        // مسح مكدس العينات / Clear sample stack
        while (!m_sampleStack.empty()) m_sampleStack.pop();
    }
    
    /// إنهاء إطار / End frame
    void EndFrame() {
        if (!m_enabled) return;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_currentFrame.totalTime = HighResTimer::DiffMilliseconds(
            m_frameStartTime, HighResTimer::Now()
        );
        
        // حفظ الإطار / Save frame
        m_frameHistory.push_back(m_currentFrame);
        if (m_frameHistory.size() > PROFILER_MAX_FRAMES) {
            m_frameHistory.erase(m_frameHistory.begin());
        }
        
        m_frameCount++;
        
        // تحديث المتوسطات / Update averages
        UpdateAverages();
    }
    
    /// تعيين إحصائيات الرسم / Set render stats
    void SetRenderStats(Int32 drawCalls, Int32 triangles, Int32 vertices) {
        if (!m_enabled) return;
        
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentFrame.drawCalls = drawCalls;
        m_currentFrame.triangles = triangles;
        m_currentFrame.vertices = vertices;
    }
    
    // ==========================================================================
    // الإحصائيات / Statistics
    // ==========================================================================
    
    /// الحصول على إحصائيات الإطار الحالي / Get current frame stats
    const FrameStats& GetCurrentFrameStats() const {
        return m_currentFrame;
    }
    
    /// الحصول على تاريخ الإطارات / Get frame history
    const std::vector<FrameStats>& GetFrameHistory() const {
        return m_frameHistory;
    }
    
    /// معدل FPS / Average FPS
    Float64 GetAverageFPS() const { return m_averageFPS; }
    
    /// أقل FPS / Minimum FPS
    Float64 GetMinFPS() const { return m_minFPS; }
    
    /// أعلى FPS / Maximum FPS
    Float64 GetMaxFPS() const { return m_maxFPS; }
    
    /// متوسط وقت الإطار / Average frame time
    Float64 GetAverageFrameTime() const { return m_averageFrameTime; }
    
    /// رقم الإطار الحالي / Current frame number
    UInt64 GetFrameCount() const { return m_frameCount; }
    
    /// الحصول على إحصائيات عينة / Get sample statistics
    struct SampleStats {
        Float64 totalTime = 0.0;
        Float64 averageTime = 0.0;
        Float64 minTime = 0.0;
        Float64 maxTime = 0.0;
        UInt64 callCount = 0;
    };
    
    SampleStats GetSampleStats(const std::string& name) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_cumulativeStats.find(name);
        if (it != m_cumulativeStats.end()) {
            SampleStats stats;
            stats.totalTime = it->second.totalTime;
            stats.callCount = it->second.callCount;
            stats.averageTime = (stats.callCount > 0) ? 
                stats.totalTime / stats.callCount : 0.0;
            stats.minTime = it->second.minTime;
            stats.maxTime = it->second.maxTime;
            return stats;
        }
        return SampleStats{};
    }
    
    /// جميع أسماء العينات / All sample names
    std::vector<std::string> GetSampleNames() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::vector<std::string> names;
        for (const auto& pair : m_cumulativeStats) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // ==========================================================================
    // تتبع الذاكرة / Memory Tracking
    // ==========================================================================
    
    MemoryTracker& GetMemoryTracker() { return m_memoryTracker; }
    const MemoryTracker& GetMemoryTracker() const { return m_memoryTracker; }
    
    // ==========================================================================
    // التقارير / Reports
    // ==========================================================================
    
    /// طباعة تقرير / Print report
    std::string GenerateReport() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        std::ostringstream ss;
        
        ss << "======================================\n";
        ss << "       تقرير الأداء / Performance Report\n";
        ss << "======================================\n\n";
        
        // إحصائيات عامة / General statistics
        ss << "--- الإحصائيات العامة / General Stats ---\n";
        ss << "عدد الإطارات / Frame Count: " << m_frameCount << "\n";
        ss << "متوسط FPS / Avg FPS: " << m_averageFPS << "\n";
        ss << "أقل FPS / Min FPS: " << m_minFPS << "\n";
        ss << "أعلى FPS / Max FPS: " << m_maxFPS << "\n";
        ss << "متوسط وقت الإطار / Avg Frame Time: " << m_averageFrameTime << " ms\n\n";
        
        // إحصائيات الرسم / Render statistics
        if (!m_frameHistory.empty()) {
            const auto& last = m_frameHistory.back();
            ss << "--- إحصائيات الرسم / Render Stats ---\n";
            ss << "استدعاءات الرسم / Draw Calls: " << last.drawCalls << "\n";
            ss << "المثلثات / Triangles: " << last.triangles << "\n";
            ss << "الرؤوس / Vertices: " << last.vertices << "\n\n";
        }
        
        // إحصائيات العينات / Sample statistics
        ss << "--- إحصائيات العينات / Sample Stats ---\n";
        for (const auto& pair : m_cumulativeStats) {
            Float64 avg = (pair.second.callCount > 0) ? 
                pair.second.totalTime / pair.second.callCount : 0.0;
            
            ss << pair.first << ":\n";
            ss << "  الاستدعاءات / Calls: " << pair.second.callCount << "\n";
            ss << "  الإجمالي / Total: " << pair.second.totalTime << " ms\n";
            ss << "  المتوسط / Avg: " << avg << " ms\n";
            ss << "  الأدنى / Min: " << pair.second.minTime << " ms\n";
            ss << "  الأقصى / Max: " << pair.second.maxTime << " ms\n";
        }
        
        // إحصائيات الذاكرة / Memory statistics
        ss << "\n--- إحصائيات الذاكرة / Memory Stats ---\n";
        ss << "الاستخدام الحالي / Current: " << FormatBytes(m_memoryTracker.GetCurrentUsage()) << "\n";
        ss << "الذروة / Peak: " << FormatBytes(m_memoryTracker.GetPeakUsage()) << "\n";
        ss << "الإجمالي المخصص / Total Alloc: " << FormatBytes(m_memoryTracker.GetTotalAllocated()) << "\n";
        ss << "عدد التخصيصات / Alloc Count: " << m_memoryTracker.GetAllocationCount() << "\n";
        ss << "عدد التحريرات / Dealloc Count: " << m_memoryTracker.GetDeallocationCount() << "\n";
        
        ss << "\n======================================\n";
        
        return ss.str();
    }
    
    /// طباعة التقرير للإخراج القياسي / Print to stdout
    void PrintReport() const {
        printf("%s", GenerateReport().c_str());
    }
    
    // ==========================================================================
    // إعادة التعيين / Reset
    // ==========================================================================
    
    /// مسح جميع البيانات / Clear all data
    void Reset() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_frameHistory.clear();
        m_cumulativeStats.clear();
        m_memoryTracker.Reset();
        
        while (!m_sampleStack.empty()) m_sampleStack.pop();
        
        m_frameCount = 0;
        m_averageFPS = 0.0;
        m_minFPS = 1e9;
        m_maxFPS = 0.0;
        m_averageFrameTime = 0.0;
    }

private:
    Profiler() = default;
    ~Profiler() = default;
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    
    /// الوقت الحالي بالملّي ثانية / Current time in milliseconds
    Float64 GetCurrentTimeMs() const {
        using namespace std::chrono;
        auto now = high_resolution_clock::now();
        auto epoch = now.time_since_epoch();
        return duration_cast<duration<Float64, std::milli>>(epoch).count();
    }
    
    /// تحديث الإحصائيات التراكمية / Update cumulative stats
    void UpdateStats(const std::string& name, Float64 duration) {
        auto& stats = m_cumulativeStats[name];
        stats.name = name;
        stats.callCount++;
        stats.totalTime += duration;
        
        if (duration < stats.minTime) stats.minTime = duration;
        if (duration > stats.maxTime) stats.maxTime = duration;
    }
    
    /// تحديث المتوسطات / Update averages
    void UpdateAverages() {
        if (m_frameHistory.empty()) return;
        
        Float64 totalTime = 0.0;
        Float64 minTime = 1e9;
        Float64 maxTime = 0.0;
        
        for (const auto& frame : m_frameHistory) {
            totalTime += frame.totalTime;
            if (frame.totalTime < minTime) minTime = frame.totalTime;
            if (frame.totalTime > maxTime) maxTime = frame.totalTime;
        }
        
        m_averageFrameTime = totalTime / m_frameHistory.size();
        m_averageFPS = (m_averageFrameTime > 0) ? 1000.0 / m_averageFrameTime : 0.0;
        m_minFPS = (maxTime > 0) ? 1000.0 / maxTime : 0.0;
        m_maxFPS = (minTime > 0) ? 1000.0 / minTime : 0.0;
    }
    
    /// تنسيق البايتات / Format bytes
    static std::string FormatBytes(size_t bytes) {
        const char* suffixes[] = {"B", "KB", "MB", "GB"};
        int s = 0;
        Float64 count = static_cast<Float64>(bytes);
        
        while (count >= 1024 && s < 3) {
            count /= 1024;
            s++;
        }
        
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.2f %s", count, suffixes[s]);
        return buffer;
    }
    
    // البيانات / Data
    bool m_enabled = true;
    mutable std::mutex m_mutex;
    
    // الإطار الحالي / Current frame
    FrameStats m_currentFrame;
    HighResTimer::TimePoint m_frameStartTime;
    std::stack<size_t> m_sampleStack;
    
    // التاريخ / History
    std::vector<FrameStats> m_frameHistory;
    UInt64 m_frameCount = 0;
    
    // الإحصائيات التراكمية / Cumulative statistics
    std::unordered_map<std::string, ProfileSample> m_cumulativeStats;
    
    // المتوسطات / Averages
    Float64 m_averageFPS = 0.0;
    Float64 m_minFPS = 1e9;
    Float64 m_maxFPS = 0.0;
    Float64 m_averageFrameTime = 0.0;
    
    // تتبع الذاكرة / Memory tracker
    MemoryTracker m_memoryTracker;
};

// ==============================================================================
// فئة مساعدة للقياس التلقائي / Auto-scope profiler helper
// ==============================================================================

/// قياس تلقائي عند الدخول والخروج من النطاق
/// Automatic measurement on scope entry/exit
class ScopedProfiler {
public:
    ScopedProfiler(const std::string& name) {
        Profiler::GetInstance().BeginSample(name);
    }
    
    ~ScopedProfiler() {
        Profiler::GetInstance().EndSample();
    }
    
    // منع النسخ / Prevent copying
    ScopedProfiler(const ScopedProfiler&) = delete;
    ScopedProfiler& operator=(const ScopedProfiler&) = delete;
};

// ==============================================================================
// الماكروات / Macros
// ==============================================================================

/// ماكرو لقياس نطاق كامل
/// Macro for measuring entire scope
#define PROFILE_SCOPE(name) \
    sad::graphics::ScopedProfiler _profiler_##__LINE__(name)

/// ماكرو باسم الدالة / Macro with function name
#define PROFILE_FUNCTION() \
    PROFILE_SCOPE(__FUNCTION__)

/// بدء قياس يدوي / Begin manual measurement
#define PROFILE_BEGIN(name) \
    sad::graphics::Profiler::GetInstance().BeginSample(name)

/// إنهاء قياس يدوي / End manual measurement
#define PROFILE_END() \
    sad::graphics::Profiler::GetInstance().EndSample()

/// بدء إطار / Begin frame
#define PROFILE_FRAME_BEGIN() \
    sad::graphics::Profiler::GetInstance().BeginFrame()

/// إنهاء إطار / End frame
#define PROFILE_FRAME_END() \
    sad::graphics::Profiler::GetInstance().EndFrame()

// ==============================================================================
// مُحلل GPU (للتوسع) / GPU Profiler (for extension)
// ==============================================================================

/// واجهة لتحليل GPU / GPU profiler interface
class IGPUProfiler {
public:
    virtual ~IGPUProfiler() = default;
    
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void BeginQuery(const std::string& name) = 0;
    virtual void EndQuery() = 0;
    virtual Float64 GetQueryTime(const std::string& name) const = 0;
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_PROFILER_H
