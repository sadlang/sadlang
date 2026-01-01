// ============================================================================
// hot_path_detector.h - كاشف المسارات الساخنة
// Hot Path Detector for Adaptive JIT Compilation
// ============================================================================
// الغرض: اكتشاف المسارات الساخنة للتجميع التكيفي
// Purpose: Detect hot paths for adaptive compilation
// ============================================================================

#ifndef SAD_HOT_PATH_DETECTOR_H
#define SAD_HOT_PATH_DETECTOR_H

#include <memory>        // لإدارة الذاكرة الذكية / For smart pointers
#include <string>        // للنصوص / For strings
#include <unordered_map> // للخرائط السريعة / For fast maps
#include <vector>        // للقوائم الديناميكية / For dynamic arrays
#include <chrono>        // للوقت / For time
#include <mutex>         // للتزامن / For thread safety
#include <functional>    // للدوال الوظيفية / For function objects

namespace Sad {
namespace JIT {

// ============================================================================
// HotPathInfo - معلومات المسار الساخن / Hot Path Information
// ============================================================================

struct HotPathInfo {
    // ========================================
    // البيانات الأساسية / Basic Data
    // ========================================
    
    std::string function_name;       // اسم الدالة / Function name
    std::string path_id;             // معرّف المسار الفريد / Unique path identifier
    
    // ========================================
    // إحصائيات التنفيذ / Execution Statistics
    // ========================================
    
    size_t execution_count;          // عدد مرات التنفيذ / Execution count
    size_t total_time_us;            // الوقت الكلي (ميكروثانية) / Total time (microseconds)
    size_t avg_time_us;              // متوسط الوقت (ميكروثانية) / Average time (microseconds)
    
    std::chrono::steady_clock::time_point first_execution; // أول تنفيذ / First execution
    std::chrono::steady_clock::time_point last_execution;  // آخر تنفيذ / Last execution
    
    // ========================================
    // التصنيف / Classification
    // ========================================
    
    bool is_hot;                     // هل المسار ساخن؟ / Is path hot?
    int temperature_level;           // مستوى الحرارة (0-100) / Temperature level (0-100)
    int tier_level;                  // مستوى التجميع المُدرّج / Tiering level
    
    // ========================================
    // معلومات التجميع / Compilation Info
    // ========================================
    
    bool is_compiled;                // هل المسار مُجمّع؟ / Is path compiled?
    int compilation_level;           // مستوى التحسين المستخدم / Optimization level used
    std::chrono::steady_clock::time_point compiled_at; // وقت التجميع / Compilation time
    
    // ========================================
    // المُنشئ / Constructor
    // ========================================
    
    HotPathInfo()
        : execution_count(0)
        , total_time_us(0)
        , avg_time_us(0)
        , first_execution(std::chrono::steady_clock::now())
        , last_execution(std::chrono::steady_clock::now())
        , is_hot(false)
        , temperature_level(0)
        , tier_level(0)
        , is_compiled(false)
        , compilation_level(0)
        , compiled_at(std::chrono::steady_clock::now())
    {}
    
    // ========================================
    // تسجيل تنفيذ / Record Execution
    // ========================================
    
    void recordExecution(size_t time_us) {
        execution_count++;
        total_time_us += time_us;
        avg_time_us = total_time_us / execution_count;
        last_execution = std::chrono::steady_clock::now();
    }
    
    // ========================================
    // حساب درجة الحرارة / Calculate Temperature
    // ========================================
    
    int calculateTemperature() const {
        // درجة الحرارة تعتمد على: عدد التنفيذات، التردد، متوسط الوقت
        // Temperature depends on: execution count, frequency, average time
        
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            now - first_execution
        ).count();
        
        if (duration == 0) duration = 1; // تجنب القسمة على صفر / Avoid division by zero
        
        // معدل التنفيذ (تنفيذات/ثانية) / Execution rate (executions/second)
        double exec_rate = static_cast<double>(execution_count) / duration;
        
        // معامل التردد (0-50) / Frequency factor (0-50)
        int frequency_score = std::min(50, static_cast<int>(exec_rate * 10));
        
        // معامل الوقت (0-50) - كلما كان أطول، كان أهم للتحسين
        // Time factor (0-50) - longer = more important to optimize
        int time_score = std::min(50, static_cast<int>(avg_time_us / 1000));
        
        return frequency_score + time_score;
    }
};

// ============================================================================
// TieringConfig - إعدادات التجميع المُدرّج / Tiering Configuration
// ============================================================================

struct TieringConfig {
    // ========================================
    // عتبات المستويات / Level Thresholds
    // ========================================
    
    size_t tier0_threshold;          // عتبة المستوى 0 (مُفسّر) / Tier 0 threshold (interpreter)
    size_t tier1_threshold;          // عتبة المستوى 1 (O0) / Tier 1 threshold (O0)
    size_t tier2_threshold;          // عتبة المستوى 2 (O1) / Tier 2 threshold (O1)
    size_t tier3_threshold;          // عتبة المستوى 3 (O2) / Tier 3 threshold (O2)
    size_t tier4_threshold;          // عتبة المستوى 4 (O3) / Tier 4 threshold (O3)
    
    // ========================================
    // إعدادات المراقبة / Monitoring Settings
    // ========================================
    
    size_t sampling_interval;        // فترة أخذ العينات (ms) / Sampling interval (ms)
    size_t window_size;              // حجم نافذة المراقبة (تنفيذات) / Monitoring window size (executions)
    
    // ========================================
    // عوامل التسخين / Warmup Factors
    // ========================================
    
    double warmup_multiplier;        // معامل التسخين / Warmup multiplier
    size_t min_warmup_executions;    // الحد الأدنى لتنفيذات التسخين / Min warmup executions
    
    // ========================================
    // المُنشئ الافتراضي / Default Constructor
    // ========================================
    
    TieringConfig()
        : tier0_threshold(0)         // المفسر دائماً / Interpreter always
        , tier1_threshold(10)        // بعد 10 تنفيذات → O0 / After 10 executions → O0
        , tier2_threshold(50)        // بعد 50 تنفيذاً → O1 / After 50 executions → O1
        , tier3_threshold(200)       // بعد 200 تنفيذ → O2 / After 200 executions → O2
        , tier4_threshold(1000)      // بعد 1000 تنفيذ → O3 / After 1000 executions → O3
        , sampling_interval(100)     // 100 ms / 100 ms
        , window_size(100)           // آخر 100 تنفيذ / Last 100 executions
        , warmup_multiplier(1.5)     // 1.5x / 1.5x
        , min_warmup_executions(5)   // على الأقل 5 تنفيذات / At least 5 executions
    {}
};

// ============================================================================
// HotPathDetector - كاشف المسارات الساخنة / Hot Path Detector
// ============================================================================

class HotPathDetector {
public:
    // نوع callback لإشعارات المسارات الساخنة / Callback type for hot path notifications
    using HotPathCallback = std::function<void(const HotPathInfo&)>;
    
    // ========================================
    // المُنشئ والمُدمر / Constructor & Destructor
    // ========================================
    
    // المُنشئ / Constructor
    // config: إعدادات التجميع المُدرّج / Tiering configuration
    explicit HotPathDetector(const TieringConfig& config = TieringConfig());
    
    // المُدمر / Destructor
    ~HotPathDetector();
    
    // منع النسخ / Prevent copying
    HotPathDetector(const HotPathDetector&) = delete;
    HotPathDetector& operator=(const HotPathDetector&) = delete;
    
    // ========================================
    // التسجيل والمراقبة / Recording & Monitoring
    // ========================================
    
    // تسجيل تنفيذ دالة / Record function execution
    // function_name: اسم الدالة / Function name
    // execution_time_us: وقت التنفيذ (ميكروثانية) / Execution time (microseconds)
    void recordExecution(const std::string& function_name, size_t execution_time_us);
    
    // تسجيل تنفيذ مسار محدد / Record specific path execution
    // function_name: اسم الدالة / Function name
    // path_id: معرّف المسار / Path identifier
    // execution_time_us: وقت التنفيذ (ميكروثانية) / Execution time (microseconds)
    void recordPathExecution(
        const std::string& function_name,
        const std::string& path_id,
        size_t execution_time_us
    );
    
    // ========================================
    // الاستعلام / Querying
    // ========================================
    
    // هل الدالة ساخنة؟ / Is function hot?
    bool isHot(const std::string& function_name) const;
    
    // الحصول على معلومات المسار / Get path information
    HotPathInfo* getPathInfo(const std::string& function_name);
    const HotPathInfo* getPathInfo(const std::string& function_name) const;
    
    // الحصول على جميع المسارات الساخنة / Get all hot paths
    std::vector<HotPathInfo> getHotPaths() const;
    
    // الحصول على عدد التنفيذات / Get execution count
    size_t getExecutionCount(const std::string& function_name) const;
    
    // الحصول على مستوى الحرارة / Get temperature level
    int getTemperature(const std::string& function_name) const;
    
    // ========================================
    // التجميع المُدرّج / Tiered Compilation
    // ========================================
    
    // الحصول على المستوى الموصى به / Get recommended tier level
    // function_name: اسم الدالة / Function name
    // returns: مستوى التجميع (0-4) / Compilation level (0-4)
    int getRecommendedTier(const std::string& function_name) const;
    
    // هل يجب ترقية المستوى؟ / Should upgrade tier?
    bool shouldUpgradeTier(const std::string& function_name) const;
    
    // تحديث مستوى التجميع / Update compilation tier
    void updateCompilationTier(const std::string& function_name, int tier_level);
    
    // ========================================
    // الإشعارات / Notifications
    // ========================================
    
    // تسجيل callback للمسارات الساخنة / Register callback for hot paths
    void onHotPath(HotPathCallback callback);
    
    // إلغاء تسجيل callbacks / Unregister callbacks
    void clearCallbacks();
    
    // ========================================
    // الإعدادات / Configuration
    // ========================================
    
    // الحصول على الإعدادات الحالية / Get current configuration
    const TieringConfig& getConfig() const;
    
    // تحديث الإعدادات / Update configuration
    void updateConfig(const TieringConfig& new_config);
    
    // تفعيل/تعطيل التجميع المُدرّج / Enable/disable tiered compilation
    void setTieringEnabled(bool enabled);
    
    // هل التجميع المُدرّج مفعّل؟ / Is tiered compilation enabled?
    bool isTieringEnabled() const;
    
    // ========================================
    // الإحصائيات / Statistics
    // ========================================
    
    // عدد المسارات المُراقبة / Number of tracked paths
    size_t getTrackedPathsCount() const;
    
    // عدد المسارات الساخنة / Number of hot paths
    size_t getHotPathsCount() const;
    
    // معدل اكتشاف المسارات الساخنة / Hot path detection rate
    double getHotPathRate() const;
    
    // طباعة الإحصائيات / Print statistics
    void printStatistics() const;
    
    // إعادة تعيين الإحصائيات / Reset statistics
    void reset();
    
    // ========================================
    // التصدير / Export
    // ========================================
    
    // تصدير إلى JSON / Export to JSON
    std::string toJSON() const;
    
    // حفظ إلى ملف / Save to file
    bool saveToFile(const std::string& file_path) const;
    
    // تحميل من ملف / Load from file
    bool loadFromFile(const std::string& file_path);

private:
    // ========================================
    // البيانات الداخلية / Internal Data
    // ========================================
    
    std::unordered_map<std::string, HotPathInfo> paths_; // معلومات المسارات / Path information
    
    TieringConfig config_;           // إعدادات التجميع المُدرّج / Tiering configuration
    bool tiering_enabled_;           // هل التجميع المُدرّج مفعّل؟ / Is tiering enabled?
    
    std::vector<HotPathCallback> callbacks_; // callbacks للإشعارات / Notification callbacks
    
    mutable std::mutex mutex_;       // قفل للتزامن / Mutex for thread safety
    
    // ========================================
    // الإحصائيات / Statistics
    // ========================================
    
    size_t total_executions_;        // عدد التنفيذات الكلي / Total executions
    size_t hot_detections_;          // عدد اكتشافات المسارات الساخنة / Hot path detections
    
    // ========================================
    // الدوال المساعدة الداخلية / Internal Helper Functions
    // ========================================
    
    // تحديث درجة الحرارة / Update temperature
    void updateTemperature(HotPathInfo& info);
    
    // التحقق من العتبات / Check thresholds
    bool checkThreshold(const HotPathInfo& info) const;
    
    // إشعار المستمعين / Notify listeners
    void notifyHotPath(const HotPathInfo& info);
    
    // حساب المستوى الموصى به / Calculate recommended tier
    int calculateRecommendedTier(const HotPathInfo& info) const;
};

} // namespace JIT
} // namespace Sad

#endif // SAD_HOT_PATH_DETECTOR_H

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
// ملاحظات التنفيذ / Implementation Notes:
// - يدعم التجميع المُدرّج مع 5 مستويات (0-4)
// - Supports tiered compilation with 5 levels (0-4)
// - اكتشاف ذكي للمسارات الساخنة بناءً على التردد والوقت
// - Intelligent hot path detection based on frequency and time
// - نظام callbacks للإشعارات الفورية
// - Callback system for immediate notifications
// - Thread-safe مع تزامن كامل
// - Thread-safe with full synchronization
// - يدعم الحفظ والتحميل من القرص
// - Supports save/load from disk
// ============================================================================
