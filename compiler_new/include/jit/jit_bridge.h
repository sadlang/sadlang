// ============================================================================
// jit_bridge.h - جسر التكامل بين المفسر ومحرك JIT
// JIT Bridge - Integration between Interpreter and JIT Engine
// ============================================================================
// الغرض: توفير واجهة سهلة للتكامل بين المفسر ومحرك JIT
// Purpose: Provide easy interface for integration between interpreter and JIT
// ============================================================================

#ifndef SAD_JIT_BRIDGE_H
#define SAD_JIT_BRIDGE_H

#include "jit_engine.h"
#include <memory>        // لإدارة الذاكرة الذكية / For smart pointers
#include <string>        // للنصوص / For strings
#include <vector>        // للقوائم الديناميكية / For dynamic arrays
#include <variant>       // للأنواع المتعددة / For multiple types
#include <functional>    // للدوال الوظيفية / For function objects
#include <unordered_map> // للخرائط السريعة / For fast maps

namespace Sad {
namespace JIT {

// ============================================================================
// SadValue - قيمة Sad (يمكن أن تكون أي نوع) / Sad Value (can be any type)
// ============================================================================

using SadValue = std::variant<
    std::nullptr_t,      // قيمة null / null value
    bool,                // قيمة منطقية / boolean value
    int64_t,             // عدد صحيح / integer
    double,              // عدد عشري / floating point
    std::string,         // نص / string
    void*                // مؤشر عام / generic pointer
>;

// ============================================================================
// SadFunction - معلومات دالة Sad / Sad Function Information
// ============================================================================

struct SadFunction {
    std::string name;              // اسم الدالة / Function name
    std::string source_code;       // الكود المصدري / Source code
    std::vector<std::string> parameters; // المعاملات / Parameters
    std::string return_type;       // نوع الإرجاع / Return type
    bool is_native;                // هل دالة أصلية؟ / Is native function?
    void* native_ptr;              // مؤشر للدالة الأصلية / Native function pointer
    
    // المُنشئ / Constructor
    SadFunction()
        : is_native(false)
        , native_ptr(nullptr)
    {}
};

// ============================================================================
// ExecutionMode - وضع التنفيذ / Execution Mode
// ============================================================================

enum class ExecutionMode {
    INTERPRETER_ONLY,    // المفسر فقط / Interpreter only
    JIT_ONLY,           // JIT فقط / JIT only
    HYBRID,             // هجين (تلقائي) / Hybrid (automatic)
    ADAPTIVE            // تكيفي (بناءً على الأداء) / Adaptive (performance-based)
};

// ============================================================================
// JITBridgeConfig - إعدادات الجسر / Bridge Configuration
// ============================================================================

struct JITBridgeConfig {
    ExecutionMode execution_mode;           // وضع التنفيذ / Execution mode
    size_t jit_threshold;                   // عتبة التحول لـ JIT / JIT threshold
    bool enable_profiling;                  // تفعيل قياس الأداء / Enable profiling
    bool enable_fallback;                   // تفعيل الرجوع للمفسر / Enable fallback to interpreter
    bool cache_interpreted_results;         // تخزين نتائج المفسر / Cache interpreted results
    size_t max_inline_size;                 // أقصى حجم للـ inline / Max inline size
    
    // المُنشئ الافتراضي / Default Constructor
    JITBridgeConfig()
        : execution_mode(ExecutionMode::HYBRID)
        , jit_threshold(10)
        , enable_profiling(true)
        , enable_fallback(true)
        , cache_interpreted_results(true)
        , max_inline_size(50)
    {}
};

// ============================================================================
// ExecutionStatistics - إحصائيات التنفيذ / Execution Statistics
// ============================================================================

struct ExecutionStatistics {
    size_t interpreted_calls;               // عدد استدعاءات المفسر / Interpreter calls
    size_t jit_calls;                       // عدد استدعاءات JIT / JIT calls
    size_t fallback_count;                  // عدد مرات الرجوع للمفسر / Fallback count
    double avg_interpreted_time_us;         // متوسط وقت المفسر / Avg interpreter time
    double avg_jit_time_us;                 // متوسط وقت JIT / Avg JIT time
    double speedup_factor;                  // معامل التسريع / Speedup factor
    
    // المُنشئ / Constructor
    ExecutionStatistics()
        : interpreted_calls(0)
        , jit_calls(0)
        , fallback_count(0)
        , avg_interpreted_time_us(0.0)
        , avg_jit_time_us(0.0)
        , speedup_factor(1.0)
    {}
};

// ============================================================================
// JITBridge - الجسر الرئيسي / Main Bridge
// ============================================================================

class JITBridge {
public:
    // ========================================
    // المُنشئ والمُدمر / Constructor & Destructor
    // ========================================
    
    // المُنشئ / Constructor
    JITBridge(const JITBridgeConfig& config = JITBridgeConfig());
    
    // المُدمر / Destructor
    ~JITBridge();
    
    // منع النسخ والإسناد / Prevent copying and assignment
    JITBridge(const JITBridge&) = delete;
    JITBridge& operator=(const JITBridge&) = delete;
    
    // ========================================
    // التهيئة / Initialization
    // ========================================
    
    // تهيئة الجسر / Initialize bridge
    bool initialize();
    
    // إيقاف التشغيل / Shutdown
    void shutdown();
    
    // هل مُهيأ؟ / Is initialized?
    bool isInitialized() const;
    
    // ========================================
    // تسجيل الدوال / Function Registration
    // ========================================
    
    // تسجيل دالة Sad / Register Sad function
    bool registerFunction(const SadFunction& function);
    
    // تسجيل دالة أصلية / Register native function
    bool registerNativeFunction(
        const std::string& name,
        void* function_ptr,
        const std::vector<std::string>& param_types,
        const std::string& return_type
    );
    
    // إلغاء تسجيل دالة / Unregister function
    bool unregisterFunction(const std::string& function_name);
    
    // ========================================
    // التنفيذ / Execution
    // ========================================
    
    // تنفيذ دالة / Execute function
    SadValue executeFunction(
        const std::string& function_name,
        const std::vector<SadValue>& args
    );
    
    // تنفيذ بالمفسر / Execute with interpreter
    SadValue executeInterpreted(
        const std::string& function_name,
        const std::vector<SadValue>& args
    );
    
    // تنفيذ بـ JIT / Execute with JIT
    SadValue executeJIT(
        const std::string& function_name,
        const std::vector<SadValue>& args
    );
    
    // ========================================
    // اتخاذ القرار / Decision Making
    // ========================================
    
    // هل يجب استخدام JIT؟ / Should use JIT?
    bool shouldUseJIT(const std::string& function_name) const;
    
    // هل الدالة ساخنة؟ / Is function hot?
    bool isHotFunction(const std::string& function_name) const;
    
    // إجبار التجميع / Force compilation
    bool forceCompile(const std::string& function_name);
    
    // ========================================
    // التحويل بين الأنواع / Type Conversion
    // ========================================
    
    // تحويل قيمة Sad إلى مؤشر void / Convert Sad value to void pointer
    void* sadValueToPointer(const SadValue& value) const;
    
    // تحويل مؤشر void إلى قيمة Sad / Convert void pointer to Sad value
    SadValue pointerToSadValue(void* ptr, const std::string& type) const;
    
    // تحويل vector من قيم Sad إلى مؤشرات / Convert vector of Sad values to pointers
    std::vector<void*> convertArgsToPointers(const std::vector<SadValue>& args) const;
    
    // ========================================
    // الإحصائيات / Statistics
    // ========================================
    
    // الحصول على الإحصائيات / Get statistics
    const ExecutionStatistics& getStatistics() const;
    
    // إعادة تعيين الإحصائيات / Reset statistics
    void resetStatistics();
    
    // طباعة الإحصائيات / Print statistics
    void printStatistics() const;
    
    // ========================================
    // الإعدادات / Configuration
    // ========================================
    
    // الحصول على الإعدادات / Get configuration
    const JITBridgeConfig& getConfig() const;
    
    // تحديث الإعدادات / Update configuration
    void updateConfig(const JITBridgeConfig& new_config);
    
    // تغيير وضع التنفيذ / Change execution mode
    void setExecutionMode(ExecutionMode mode);
    
    // ========================================
    // الوصول إلى المحرك / Engine Access
    // ========================================
    
    // الحصول على محرك JIT / Get JIT engine
    JITEngine* getJITEngine();
    const JITEngine* getJITEngine() const;
    
    // ========================================
    // معلومات الدوال / Function Information
    // ========================================
    
    // الحصول على معلومات دالة / Get function info
    const SadFunction* getFunctionInfo(const std::string& function_name) const;
    
    // عدد الدوال المسجلة / Number of registered functions
    size_t getRegisteredFunctionCount() const;
    
    // الحصول على جميع أسماء الدوال / Get all function names
    std::vector<std::string> getRegisteredFunctionNames() const;
    
    // ========================================
    // معالجة الأخطاء / Error Handling
    // ========================================
    
    // الحصول على آخر خطأ / Get last error
    std::string getLastError() const;
    
    // مسح الأخطاء / Clear errors
    void clearErrors();
    
private:
    // ========================================
    // التنفيذ الداخلي (Pimpl) / Internal Implementation
    // ========================================
    
    struct Impl;                             // التنفيذ الداخلي / Internal implementation
    std::unique_ptr<Impl> pimpl_;           // مؤشر للتنفيذ الداخلي / Pointer to implementation
    
    mutable std::mutex mutex_;              // قفل للتزامن / Mutex for thread safety
    
    // ========================================
    // الدوال المساعدة / Helper Functions
    // ========================================
    
    // تسجيل استدعاء المفسر / Record interpreter call
    void recordInterpreterCall(const std::string& function_name, double time_us);
    
    // تسجيل استدعاء JIT / Record JIT call
    void recordJITCall(const std::string& function_name, double time_us);
    
    // تحديث الإحصائيات / Update statistics
    void updateStatistics();
    
    // التحقق من الأخطاء / Check for errors
    bool checkError(const std::string& context);
};

// ============================================================================
// دوال مساعدة عامة / Global Helper Functions
// ============================================================================

// طباعة قيمة Sad / Print Sad value
std::string sadValueToString(const SadValue& value);

// الحصول على نوع قيمة Sad / Get Sad value type
std::string sadValueTypeName(const SadValue& value);

// مقارنة قيمتين Sad / Compare two Sad values
bool sadValuesEqual(const SadValue& a, const SadValue& b);

} // namespace JIT
} // namespace Sad

#endif // SAD_JIT_BRIDGE_H

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
