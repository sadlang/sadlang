/**
 * @file profiler.h
 * @brief محلل الأداء (Profiler) للغة ص
 * @brief (EN) Performance profiler for Sad language
 * 
 * @author عمر
 * @date أكتوبر 2026
 */

#ifndef SAD_PROFILER_H
#define SAD_PROFILER_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;map&gt;
#include &lt;memory&gt;
#include &lt;chrono&gt;
#include &lt;stack&gt;
#include &lt;mutex&gt;
#include &lt;atomic&gt;

namespace Sad {
namespace Tools {

//==============================================================================
// أنواع التوقيت
//==============================================================================

using TimePoint = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::nanoseconds;

/**
 * @brief سجل استدعاء دالة
 */
struct CallRecord {
    std::string functionName;
    std::string filename;
    int line;
    TimePoint startTime;
    TimePoint endTime;
    Duration duration;
    uint64_t callCount;
    uint64_t memoryAllocated;
    uint64_t memoryFreed;
    std::vector&lt;std::string&gt; callers;      // من استدعاها
    std::vector&lt;std::string&gt; callees;      // من استدعت
    
    CallRecord() : line(0), callCount(0), memoryAllocated(0), memoryFreed(0) {}
    
    double durationMs() const {
        return std::chrono::duration&lt;double, std::milli&gt;(duration).count();
    }
    
    double averageMs() const {
        return callCount &gt; 0 ? durationMs() / callCount : 0;
    }
};

/**
 * @brief إطار المكدس للتتبع
 */
struct ProfileFrame {
    std::string functionName;
    TimePoint enterTime;
    uint64_t memoryStart;
    bool isBuiltin;
};

/**
 * @brief خط ساخن (Hot Line)
 */
struct HotLine {
    std::string filename;
    int line;
    uint64_t hitCount;
    Duration totalTime;
    
    double averageTimeNs() const {
        return hitCount &gt; 0 ? (double)totalTime.count() / hitCount : 0;
    }
};

//==============================================================================
// محلل الأداء
//==============================================================================

/**
 * @brief محلل الأداء الرئيسي
 */
class Profiler {
public:
    static Profiler&amp; instance();
    
    //==========================================================================
    // التحكم
    //==========================================================================
    
    /**
     * @brief بدء التحليل
     */
    void start();
    
    /**
     * @brief إيقاف التحليل
     */
    void stop();
    
    /**
     * @brief هل التحليل نشط؟
     */
    bool isActive() const { return active_.load(); }
    
    /**
     * @brief إعادة تعيين البيانات
     */
    void reset();
    
    //==========================================================================
    // تسجيل الأحداث
    //==========================================================================
    
    /**
     * @brief تسجيل دخول دالة
     */
    void enterFunction(const std::string&amp; name, const std::string&amp; file, int line);
    
    /**
     * @brief تسجيل خروج من دالة
     */
    void exitFunction(const std::string&amp; name);
    
    /**
     * @brief تسجيل سطر
     */
    void hitLine(const std::string&amp; file, int line);
    
    /**
     * @brief تسجيل تخصيص ذاكرة
     */
    void recordAllocation(size_t bytes);
    
    /**
     * @brief تسجيل تحرير ذاكرة
     */
    void recordDeallocation(size_t bytes);
    
    //==========================================================================
    // التقارير
    //==========================================================================
    
    /**
     * @brief الحصول على سجلات الدوال
     */
    std::vector&lt;CallRecord&gt; getFunctionRecords() const;
    
    /**
     * @brief الحصول على الخطوط الساخنة
     */
    std::vector&lt;HotLine&gt; getHotLines(int limit = 20) const;
    
    /**
     * @brief الحصول على شجرة الاستدعاءات
     */
    std::string getCallTree() const;
    
    /**
     * @brief الحصول على ملخص الأداء
     */
    std::string getSummary() const;
    
    /**
     * @brief توليد تقرير HTML
     */
    std::string generateHtmlReport() const;
    
    /**
     * @brief توليد تقرير flame graph (SVG)
     */
    std::string generateFlameGraph() const;
    
    /**
     * @brief حفظ التقرير إلى ملف
     */
    bool saveReport(const std::string&amp; path, const std::string&amp; format = "html");

private:
    Profiler() = default;
    
    std::atomic&lt;bool&gt; active_{false};
    mutable std::mutex mutex_;
    
    // بيانات التحليل
    std::map&lt;std::string, CallRecord&gt; functions_;
    std::map&lt;std::pair&lt;std::string, int&gt;, HotLine&gt; lines_;
    std::stack&lt;ProfileFrame&gt; callStack_;
    
    // إحصائيات عامة
    TimePoint startTime_;
    TimePoint endTime_;
    uint64_t totalAllocs_ = 0;
    uint64_t totalFrees_ = 0;
    uint64_t peakMemory_ = 0;
    uint64_t currentMemory_ = 0;
};

//==============================================================================
// مساعد RAII للتحليل
//==============================================================================

/**
 * @brief RAII scope timer
 */
class ProfileScope {
public:
    ProfileScope(const std::string&amp; name, const std::string&amp; file = "", int line = 0) 
        : name_(name) {
        Profiler::instance().enterFunction(name, file, line);
    }
    
    ~ProfileScope() {
        Profiler::instance().exitFunction(name_);
    }
    
private:
    std::string name_;
};

//==============================================================================
// ماكرو للتحليل
//==============================================================================

#ifdef SAD_ENABLE_PROFILING
    #define SAD_PROFILE_FUNCTION() \
        Sad::Tools::ProfileScope _profScope##__LINE__(__FUNCTION__, __FILE__, __LINE__)
    
    #define SAD_PROFILE_SCOPE(name) \
        Sad::Tools::ProfileScope _profScope##__LINE__(name, __FILE__, __LINE__)
    
    #define SAD_PROFILE_LINE() \
        Sad::Tools::Profiler::instance().hitLine(__FILE__, __LINE__)
#else
    #define SAD_PROFILE_FUNCTION()
    #define SAD_PROFILE_SCOPE(name)
    #define SAD_PROFILE_LINE()
#endif

//==============================================================================
// مولد Flame Graph
//==============================================================================

/**
 * @brief مولد رسم Flame Graph
 */
class FlameGraphGenerator {
public:
    /**
     * @brief إضافة عينة
     */
    void addSample(const std::vector&lt;std::string&gt;&amp; stack, uint64_t count = 1);
    
    /**
     * @brief توليد SVG
     */
    std::string generateSVG(int width = 1200, int height = 600) const;
    
    /**
     * @brief توليد تنسيق collapsed (للأدوات الخارجية)
     */
    std::string generateCollapsed() const;

private:
    struct StackNode {
        std::string name;
        uint64_t count = 0;
        std::map&lt;std::string, StackNode&gt; children;
    };
    
    StackNode root_;
};

//==============================================================================
// دوال عربية
//==============================================================================

namespace Arabic {

/**
 * @brief بدء تحليل الأداء
 */
inline void ابدأ_التحليل() {
    Profiler::instance().start();
}

/**
 * @brief أوقف تحليل الأداء
 */
inline void أوقف_التحليل() {
    Profiler::instance().stop();
}

/**
 * @brief احصل على ملخص الأداء
 */
inline std::string ملخص_الأداء() {
    return Profiler::instance().getSummary();
}

/**
 * @brief احفظ تقرير الأداء
 */
inline bool احفظ_التقرير(const std::string&amp; path) {
    return Profiler::instance().saveReport(path);
}

/**
 * @brief الدوال الأبطأ
 */
inline std::vector&lt;CallRecord&gt; الدوال_الأبطأ(int limit = 10) {
    auto records = Profiler::instance().getFunctionRecords();
    std::sort(records.begin(), records.end(), 
        [](const CallRecord&amp; a, const CallRecord&amp; b) {
            return a.durationMs() &gt; b.durationMs();
        });
    if (records.size() &gt; (size_t)limit) {
        records.resize(limit);
    }
    return records;
}

/**
 * @brief الدوال الأكثر استدعاءً
 */
inline std::vector&lt;CallRecord&gt; الدوال_الأكثر_استدعاءً(int limit = 10) {
    auto records = Profiler::instance().getFunctionRecords();
    std::sort(records.begin(), records.end(),
        [](const CallRecord&amp; a, const CallRecord&amp; b) {
            return a.callCount &gt; b.callCount;
        });
    if (records.size() &gt; (size_t)limit) {
        records.resize(limit);
    }
    return records;
}

} // namespace Arabic

} // namespace Tools
} // namespace Sad

#endif // SAD_PROFILER_H
