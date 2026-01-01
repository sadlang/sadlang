// ============================================================================
// jit_engine.h - محرك التجميع الفوري (JIT Engine)
// Just-In-Time Compilation Engine for Sad Language
// ============================================================================
// الغرض: توفير تجميع فوري لكود Sad باستخدام LLVM ORC JIT
// Purpose: Provide JIT compilation for Sad code using LLVM ORC JIT
// ============================================================================

#ifndef SAD_JIT_ENGINE_H
#define SAD_JIT_ENGINE_H

#include <memory>        // لإدارة الذاكرة الذكية / For smart pointers
#include <string>        // للنصوص / For strings
#include <unordered_map> // للخرائط السريعة / For fast maps
#include <vector>        // للقوائم الديناميكية / For dynamic arrays
#include <functional>    // للدوال الوظيفية / For function objects
#include <mutex>         // للتزامن / For thread safety

// ملاحظة: سنستخدم forward declarations لتجنب الاعتماد على LLVM headers
// Note: Using forward declarations to avoid dependency on LLVM headers
// حتى يكتمل تثبيت LLVM / Until LLVM installation completes

namespace Sad {
namespace JIT {

// ============================================================================
// التصريحات المسبقة / Forward Declarations
// ============================================================================

// نوع الدالة المُجمعة / Compiled function type
using CompiledFunction = void*;

// معلومات إحصائية / Statistical information
struct JITStatistics;

// ============================================================================
// JITCompilationResult - نتيجة التجميع / Compilation Result
// ============================================================================

struct JITCompilationResult {
    bool success;                    // هل نجح التجميع؟ / Compilation succeeded?
    CompiledFunction function;       // مؤشر للدالة المجمعة / Pointer to compiled function
    std::string error_message;       // رسالة الخطأ (إن وجدت) / Error message (if any)
    double compilation_time_ms;      // وقت التجميع بالميلي ثانية / Compilation time in ms
    size_t code_size_bytes;          // حجم الكود المولد / Generated code size
    
    // المُنشئ الافتراضي / Default constructor
    JITCompilationResult() 
        : success(false)
        , function(nullptr)
        , compilation_time_ms(0.0)
        , code_size_bytes(0) 
    {}
};

// ============================================================================
// JITConfig - إعدادات محرك JIT / JIT Engine Configuration
// ============================================================================

struct JITConfig {
    // ========================================
    // إعدادات التحسين / Optimization Settings
    // ========================================
    
    int optimization_level;          // مستوى التحسين (0-3) / Optimization level (0-3)
    bool enable_inlining;            // تفعيل دمج الدوال / Enable function inlining
    bool enable_vectorization;       // تفعيل SIMD / Enable SIMD vectorization
    
    // ========================================
    // إعدادات المسار الساخن / Hot Path Settings
    // ========================================
    
    int hot_threshold;               // عدد التنفيذات لاعتبار المسار ساخناً / Execution count for hot path
    bool enable_tiered_compilation;  // تفعيل التجميع المُدرّج / Enable tiered compilation
    
    // ========================================
    // إعدادات الذاكرة المؤقتة / Cache Settings
    // ========================================
    
    bool enable_cache;               // تفعيل التخزين المؤقت / Enable caching
    size_t max_cache_size_mb;        // الحد الأقصى لحجم الذاكرة المؤقتة (MB) / Max cache size (MB)
    bool enable_persistent_cache;    // تفعيل الذاكرة المؤقتة الدائمة / Enable persistent cache
    std::string cache_directory;     // مجلد الذاكرة المؤقتة / Cache directory
    
    // ========================================
    // إعدادات التنقيح / Debug Settings
    // ========================================
    
    bool enable_profiling;           // تفعيل قياس الأداء / Enable profiling
    bool enable_debug_info;          // تفعيل معلومات التنقيح / Enable debug info
    bool print_ir;                   // طباعة LLVM IR / Print LLVM IR
    bool verbose;                    // طباعة تفاصيل إضافية / Print verbose output
    
    // ========================================
    // المُنشئ الافتراضي / Default Constructor
    // ========================================
    
    JITConfig()
        : optimization_level(2)              // O2 افتراضياً / O2 by default
        , enable_inlining(true)              // دمج الدوال مفعّل / Inlining enabled
        , enable_vectorization(true)         // SIMD مفعّل / SIMD enabled
        , hot_threshold(100)                 // 100 تنفيذ / 100 executions
        , enable_tiered_compilation(true)    // تجميع مُدرّج مفعّل / Tiered compilation enabled
        , enable_cache(true)                 // الذاكرة المؤقتة مفعّلة / Cache enabled
        , max_cache_size_mb(256)             // 256 MB / 256 MB
        , enable_persistent_cache(false)     // دائمة معطلة افتراضياً / Persistent cache disabled by default
        , cache_directory(".sad_jit_cache")  // المجلد الافتراضي / Default directory
        , enable_profiling(false)            // قياس الأداء معطل / Profiling disabled
        , enable_debug_info(false)           // معلومات تنقيح معطلة / Debug info disabled
        , print_ir(false)                    // لا تطبع IR / Don't print IR
        , verbose(false)                     // غير مطوّل / Not verbose
    {}
};

// ============================================================================
// JITStatistics - إحصائيات الأداء / Performance Statistics
// ============================================================================

struct JITStatistics {
    // ========================================
    // إحصائيات التجميع / Compilation Statistics
    // ========================================
    
    size_t total_compilations;       // عدد التجميعات الكلي / Total compilations
    size_t successful_compilations;  // عدد التجميعات الناجحة / Successful compilations
    size_t failed_compilations;      // عدد التجميعات الفاشلة / Failed compilations
    double total_compilation_time_ms; // الوقت الكلي للتجميع (ms) / Total compilation time (ms)
    double avg_compilation_time_ms;  // متوسط وقت التجميع (ms) / Average compilation time (ms)
    
    // ========================================
    // إحصائيات التنفيذ / Execution Statistics
    // ========================================
    
    size_t total_executions;         // عدد التنفيذات الكلي / Total executions
    size_t jit_executions;           // عدد التنفيذات المُجمّعة / JIT executions
    size_t interpreted_executions;   // عدد التنفيذات المُفسّرة / Interpreted executions
    
    // ========================================
    // إحصائيات الذاكرة المؤقتة / Cache Statistics
    // ========================================
    
    size_t cache_hits;               // عدد الإصابات في الذاكرة المؤقتة / Cache hits
    size_t cache_misses;             // عدد الإخفاقات في الذاكرة المؤقتة / Cache misses
    size_t cache_size_bytes;         // حجم الذاكرة المؤقتة الحالي (bytes) / Current cache size (bytes)
    
    // ========================================
    // إحصائيات الأداء / Performance Statistics
    // ========================================
    
    double speedup_factor;           // عامل التسريع مقارنة بالمفسر / Speedup factor vs interpreter
    size_t hot_functions_detected;   // عدد الدوال الساخنة المكتشفة / Hot functions detected
    
    // ========================================
    // المُنشئ الافتراضي / Default Constructor
    // ========================================
    
    JITStatistics()
        : total_compilations(0)
        , successful_compilations(0)
        , failed_compilations(0)
        , total_compilation_time_ms(0.0)
        , avg_compilation_time_ms(0.0)
        , total_executions(0)
        , jit_executions(0)
        , interpreted_executions(0)
        , cache_hits(0)
        , cache_misses(0)
        , cache_size_bytes(0)
        , speedup_factor(1.0)
        , hot_functions_detected(0)
    {}
    
    // ========================================
    // حساب نسبة النجاح / Calculate Success Rate
    // ========================================
    
    double getSuccessRate() const {
        if (total_compilations == 0) return 0.0;
        return (static_cast<double>(successful_compilations) / total_compilations) * 100.0;
    }
    
    // ========================================
    // حساب نسبة الإصابة في الذاكرة المؤقتة / Calculate Cache Hit Rate
    // ========================================
    
    double getCacheHitRate() const {
        size_t total_accesses = cache_hits + cache_misses;
        if (total_accesses == 0) return 0.0;
        return (static_cast<double>(cache_hits) / total_accesses) * 100.0;
    }
};

// ============================================================================
// JITEngine - محرك التجميع الفوري / JIT Compilation Engine
// ============================================================================

class JITEngine {
public:
    // ========================================
    // المُنشئ والمُدمر / Constructor & Destructor
    // ========================================
    
    // المُنشئ / Constructor
    // config: إعدادات المحرك / Engine configuration
    explicit JITEngine(const JITConfig& config = JITConfig());
    
    // المُدمر / Destructor
    ~JITEngine();
    
    // منع النسخ / Prevent copying
    JITEngine(const JITEngine&) = delete;
    JITEngine& operator=(const JITEngine&) = delete;
    
    // السماح بالنقل / Allow moving
    JITEngine(JITEngine&&) noexcept;
    JITEngine& operator=(JITEngine&&) noexcept;
    
    // ========================================
    // التهيئة والإغلاق / Initialization & Shutdown
    // ========================================
    
    // تهيئة محرك JIT / Initialize JIT engine
    // target_triple: معمارية الهدف (مثل "x86_64-pc-windows-msvc") / Target architecture
    // returns: true إذا نجحت التهيئة / true if initialization succeeded
    bool initialize(const std::string& target_triple = "");
    
    // إغلاق محرك JIT وتحرير الموارد / Shutdown JIT engine and free resources
    void shutdown();
    
    // هل المحرك مُهيأ؟ / Is engine initialized?
    bool isInitialized() const;
    
    // ========================================
    // التجميع / Compilation
    // ========================================
    
    // تجميع دالة من كود مصدري / Compile function from source code
    // function_name: اسم الدالة / Function name
    // source_code: الكود المصدري / Source code
    // returns: نتيجة التجميع / Compilation result
    JITCompilationResult compileFunction(
        const std::string& function_name,
        const std::string& source_code
    );
    
    // تجميع دالة من LLVM IR / Compile function from LLVM IR
    // function_name: اسم الدالة / Function name
    // ir_code: كود LLVM IR / LLVM IR code
    // returns: نتيجة التجميع / Compilation result
    JITCompilationResult compileFunctionFromIR(
        const std::string& function_name,
        const std::string& ir_code
    );
    
    // ========================================
    // التنفيذ / Execution
    // ========================================
    
    // تنفيذ دالة مُجمّعة / Execute compiled function
    // function_name: اسم الدالة / Function name
    // args: المعاملات / Arguments
    // returns: مؤشر للنتيجة (أو nullptr) / Pointer to result (or nullptr)
    void* executeFunction(
        const std::string& function_name,
        const std::vector<void*>& args = {}
    );
    
    // هل الدالة مُجمّعة؟ / Is function compiled?
    bool isFunctionCompiled(const std::string& function_name) const;
    
    // ========================================
    // الذاكرة المؤقتة / Caching
    // ========================================
    
    // إضافة دالة إلى الذاكرة المؤقتة / Add function to cache
    void cacheFunction(const std::string& function_name, CompiledFunction function);
    
    // البحث في الذاكرة المؤقتة / Lookup in cache
    CompiledFunction getCachedFunction(const std::string& function_name) const;
    
    // حذف من الذاكرة المؤقتة / Remove from cache
    void evictFromCache(const std::string& function_name);
    
    // مسح الذاكرة المؤقتة بالكامل / Clear entire cache
    void clearCache();
    
    // ========================================
    // إدارة المسارات الساخنة / Hot Path Management
    // ========================================
    
    // تسجيل تنفيذ دالة / Record function execution
    void recordExecution(const std::string& function_name);
    
    // هل الدالة ساخنة؟ / Is function hot?
    bool isHotFunction(const std::string& function_name) const;
    
    // الحصول على عدد التنفيذات / Get execution count
    size_t getExecutionCount(const std::string& function_name) const;
    
    // ========================================
    // الإحصائيات / Statistics
    // ========================================
    
    // الحصول على الإحصائيات / Get statistics
    const JITStatistics& getStatistics() const;
    
    // إعادة تعيين الإحصائيات / Reset statistics
    void resetStatistics();
    
    // طباعة الإحصائيات / Print statistics
    void printStatistics() const;
    
    // ========================================
    // الإعدادات / Configuration
    // ========================================
    
    // الحصول على الإعدادات الحالية / Get current configuration
    const JITConfig& getConfig() const;
    
    // تحديث الإعدادات / Update configuration
    void updateConfig(const JITConfig& new_config);
    
    // ========================================
    // معلومات النظام / System Information
    // ========================================
    
    // الحصول على معمارية الهدف / Get target architecture
    std::string getTargetTriple() const;
    
    // الحصول على عدد النوى المتاحة / Get available CPU cores
    unsigned int getAvailableCores() const;
    
    // الحصول على حجم الذاكرة المتاح / Get available memory
    size_t getAvailableMemoryMB() const;

private:
    // ========================================
    // البيانات الداخلية / Internal Data
    // ========================================
    
    class Impl;                      // التنفيذ الداخلي (Pimpl idiom) / Internal implementation
    std::unique_ptr<Impl> pImpl;     // مؤشر ذكي للتنفيذ / Smart pointer to implementation
    
    JITConfig config_;               // إعدادات المحرك / Engine configuration
    JITStatistics statistics_;       // الإحصائيات / Statistics
    
    bool initialized_;               // هل المحرك مُهيأ؟ / Is engine initialized?
    std::string target_triple_;      // معمارية الهدف / Target architecture
    
    mutable std::mutex mutex_;       // قفل للتزامن / Mutex for thread safety
    
    // ========================================
    // الدوال المساعدة الداخلية / Internal Helper Functions
    // ========================================
    
    // تحديث الإحصائيات بعد التجميع / Update statistics after compilation
    void updateCompilationStats(const JITCompilationResult& result);
    
    // تحديث الإحصائيات بعد التنفيذ / Update statistics after execution
    void updateExecutionStats(bool was_jit);
    
    // التحقق من حد الذاكرة المؤقتة / Check cache size limit
    void enforceCacheLimit();
};

} // namespace JIT
} // namespace Sad

#endif // SAD_JIT_ENGINE_H

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
// ملاحظات التنفيذ / Implementation Notes:
// - استخدام Pimpl idiom لإخفاء تفاصيل LLVM
// - Using Pimpl idiom to hide LLVM implementation details
// - Thread-safe باستخدام mutex
// - Thread-safe using mutex
// - يدعم التجميع المُدرّج (tiered compilation)
// - Supports tiered compilation
// - يدعم الذاكرة المؤقتة الذكية
// - Supports intelligent caching
// ============================================================================
