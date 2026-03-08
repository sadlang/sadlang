// ============================================================================
// sad_jit.h — محرك الترجمة الفورية (JIT) لغة ص
// JIT Compilation Engine for Sad Language
// ============================================================================
//
// الوصف (AR):
//   محرك JIT يدمج الآلة الافتراضية (VM) مع المترجم الأصلي (sadc/LLVM).
//   يأخذ بايت كود VM ويحوّله إلى كود أصلي عبر LLVM ORC JIT لتحسين الأداء.
//
//   المسار: AST → VM Bytecode → SIR → LLVM IR → Native Code → التنفيذ المباشر
//
//   يوفّر:
//   - ترجمة فورية للدوال "الساخنة" (Hot Functions) عند استدعائها عدة مرات
//   - عتبة ترجمة قابلة للضبط (عدد الاستدعاءات قبل الترجمة)
//   - ذاكرة مؤقتة للكود المُترجم (Code Cache)
//   - عداد استدعاءات لكل دالة (Profiling Counters)
//   - دعم التبديل بين التفسير والتنفيذ الأصلي (Mixed Mode)
//
// المعمارية:
//   ┌─────────────┐    ┌──────────────┐    ┌──────────────┐
//   │  VM Bytecode │───→│  JIT Engine   │───→│  Native Code │
//   │  (43+ ops)   │    │  (هذا الملف)   │    │  (LLVM)      │
//   └─────────────┘    └──────────────┘    └──────────────┘
//         │                   │                    │
//         │                   ▼                    │
//         │            ┌──────────────┐            │
//         └───────────→│  البروفايلر   │←───────────┘
//                      │  (Profiler)   │
//                      └──────────────┘
//
// Description (EN):
//   JIT engine that bridges the VM bytecode with LLVM's ORC JIT compiler.
//   Takes VM bytecode and compiles hot functions to native code at runtime.
//
// ============================================================================

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <chrono>

// ============================================================================
// تضمين VM / VM Includes
// ============================================================================
#include "sad_vm_opcodes.h"
#include "sad_vm_chunk.h"

// ============================================================================
// شرط LLVM — JIT يعمل فقط مع LLVM / LLVM conditional
// ============================================================================
#ifdef HAS_LLVM
    // سيتم تضمين رؤوس LLVM في ملف التنفيذ فقط
    // LLVM headers included only in implementation file
#endif

namespace sad {
namespace jit {

// ============================================================================
// مستوى التحسين / Optimization Level
// ============================================================================
/// مستويات تحسين JIT — تؤثر على سرعة الترجمة مقابل جودة الكود
/// JIT optimization levels — tradeoff between compilation speed and code quality
enum class مستوى_التحسين : uint8_t {
    بدون   = 0,    // O0 — أسرع ترجمة، للتصحيح / Fastest compilation, for debugging
    أساسي  = 1,    // O1 — تحسينات بسيطة / Basic optimizations
    متوسط  = 2,    // O2 — توازن (افتراضي) / Balanced (default)
    أقصى   = 3     // O3 — أقصى تحسين / Maximum optimization
};

// ============================================================================
// إحصائيات JIT / JIT Statistics
// ============================================================================
/// إحصائيات الأداء لمحرك JIT — للمراقبة والتحليل
/// JIT performance statistics — for monitoring and profiling
struct إحصائيات_JIT {
    uint64_t عدد_الدوال_المترجمة = 0;          // Functions compiled to native
    uint64_t عدد_الاستدعاءات_الأصلية = 0;       // Native call count
    uint64_t عدد_الاستدعاءات_المفسرة = 0;       // Interpreted call count
    uint64_t حجم_الكود_المولد = 0;              // Generated code size (bytes)
    double   زمن_الترجمة_الإجمالي = 0.0;        // Total compilation time (ms)
    double   زمن_التنفيذ_الأصلي = 0.0;          // Total native execution time (ms)
    double   زمن_التنفيذ_المفسر = 0.0;          // Total interpreted execution time (ms)
    uint64_t عدد_مرات_إلغاء_الترجمة = 0;        // Deoptimization count
    
    /// نسبة التسريع — كم مرة أسرع من التفسير
    /// Speedup ratio — how many times faster than interpretation
    double نسبة_التسريع() const {
        if (زمن_التنفيذ_الأصلي <= 0.0) return 0.0;
        return زمن_التنفيذ_المفسر / زمن_التنفيذ_الأصلي;
    }
};

// ============================================================================
// معلومات الدالة المُترجمة / Compiled Function Info
// ============================================================================
/// معلومات دالة تم ترجمتها JIT — للتخزين في ذاكرة الكود المؤقتة
/// Info about a JIT-compiled function — stored in code cache
struct دالة_مترجمة {
    std::string الاسم;                          // Function name
    void* مؤشر_الكود = nullptr;                 // Pointer to native code
    size_t حجم_الكود = 0;                       // Code size in bytes
    uint64_t عدد_الاستدعاءات = 0;               // Call count (profiling)
    uint64_t عداد_التنفيذ = 0;                   // Execution counter
    مستوى_التحسين مستوى = مستوى_التحسين::متوسط; // Optimization level used
    bool صالحة = false;                         // Is the compiled code valid?
    
    /// الزمن الكلي للترجمة (مللي ثانية)
    /// Total compilation time (milliseconds)
    double زمن_الترجمة = 0.0;
    
    /// طابع زمني لآخر استخدام — لإدارة الذاكرة المؤقتة
    /// Last-use timestamp — for cache eviction
    std::chrono::steady_clock::time_point آخر_استخدام;
};

// ============================================================================
// إعدادات JIT / JIT Configuration
// ============================================================================
/// إعدادات محرك JIT القابلة للضبط
/// Configurable JIT engine settings
struct إعدادات_JIT {
    /// عتبة الترجمة: عدد استدعاءات الدالة قبل ترجمتها JIT
    /// Compilation threshold: function call count before JIT compilation
    uint32_t عتبة_الترجمة = 100;
    
    /// مستوى التحسين الافتراضي
    /// Default optimization level
    مستوى_التحسين المستوى = مستوى_التحسين::متوسط;
    
    /// الحد الأقصى لحجم ذاكرة الكود المؤقتة (ميغابايت)
    /// Maximum code cache size (MB)
    uint32_t حجم_الذاكرة_الأقصى_MB = 64;
    
    /// تفعيل الطباعة التشخيصية
    /// Enable diagnostic output
    bool التتبع = false;
    
    /// تفعيل التحسين التكيفي (إعادة ترجمة بمستوى أعلى)
    /// Enable adaptive optimization (recompile at higher level)
    bool التحسين_التكيفي = true;
    
    /// عتبة التحسين التكيفي (استدعاءات إضافية بعد أول ترجمة)
    /// Adaptive optimization threshold
    uint32_t عتبة_التحسين_التكيفي = 1000;
};

// ============================================================================
// محرك JIT الرئيسي / Main JIT Engine
// ============================================================================
/// محرك الترجمة الفورية — يدمج VM مع LLVM لتنفيذ أصلي
/// JIT Compilation Engine — bridges VM with LLVM for native execution
///
/// الاستخدام / Usage:
///   // إنشاء المحرك مع الإعدادات الافتراضية
///   محرك_JIT jit;
///   
///   // تسجيل دالة من البايت كود
///   jit.سجّل_دالة("جمع", بايت_كود_الدالة);
///   
///   // التنفيذ — آلي (يختار تفسير أو أصلي)
///   auto نتيجة = jit.نفّذ_دالة("جمع", {قيمة1, قيمة2});
///
class محرك_JIT {
public:
    // ========================================================================
    // البناء والتدمير / Construction & Destruction
    // ========================================================================
    
    /// إنشاء محرك JIT بإعدادات مخصصة
    /// Create JIT engine with custom settings
    explicit محرك_JIT(const إعدادات_JIT& إعدادات = إعدادات_JIT{});
    
    /// التدمير — تحرير جميع الموارد
    /// Destructor — release all resources
    ~محرك_JIT();
    
    // منع النسخ / Prevent copying
    محرك_JIT(const محرك_JIT&) = delete;
    محرك_JIT& operator=(const محرك_JIT&) = delete;
    
    // ========================================================================
    // التهيئة / Initialization
    // ========================================================================
    
    /// تهيئة محرك LLVM ORC JIT
    /// Initialize LLVM ORC JIT engine
    /// @return true إذا نجحت التهيئة / true if initialization succeeded
    bool هيّئ();
    
    /// هل المحرك جاهز للعمل؟
    /// Is the engine ready?
    bool جاهز() const { return جاهز_; }
    
    // ========================================================================
    // تسجيل وترجمة الدوال / Function Registration & Compilation
    // ========================================================================
    
    /// تسجيل دالة بايت كود للمراقبة
    /// Register a bytecode function for monitoring
    /// @param الاسم اسم الدالة / Function name
    /// @param الكود بايت كود الدالة / Function bytecode
    /// @param عدد_المعاملات عدد معاملات الدالة / Parameter count
    void سجّل_دالة(const std::string& الاسم,
                    const vm::وحدة_بايت_كود& الكود,
                    uint32_t عدد_المعاملات = 0);
    
    /// ترجمة دالة فوراً (بدون انتظار العتبة)
    /// Compile a function immediately (bypass threshold)
    /// @param الاسم اسم الدالة المسجلة / Registered function name
    /// @param المستوى مستوى التحسين / Optimization level
    /// @return true إذا نجحت الترجمة / true if compilation succeeded
    bool ترجم_فوراً(const std::string& الاسم,
                    مستوى_التحسين المستوى = مستوى_التحسين::متوسط);
    
    /// ترجمة جميع الدوال المسجلة فوراً
    /// Compile all registered functions immediately
    /// @param المستوى مستوى التحسين / Optimization level
    /// @return عدد الدوال المُترجمة بنجاح / Number of successfully compiled functions
    uint32_t ترجم_الكل(مستوى_التحسين المستوى = مستوى_التحسين::متوسط);
    
    // ========================================================================
    // التنفيذ / Execution
    // ========================================================================
    
    /// تنفيذ دالة (اختيار تلقائي: تفسير أو أصلي)
    /// Execute a function (automatic: interpreted or native)
    /// @param الاسم اسم الدالة / Function name
    /// @param المعاملات قيم المعاملات / Argument values
    /// @return قيمة الإرجاع / Return value
    int64_t نفّذ(const std::string& الاسم,
                const std::vector<int64_t>& المعاملات = {});
    
    /// هل الدالة مُترجمة JIT؟
    /// Is the function JIT-compiled?
    bool مترجمة(const std::string& الاسم) const;
    
    /// زيادة عداد استدعاء دالة — يُستدعى من المُفسّر
    /// Increment function call counter — called from interpreter
    /// @return true إذا تجاوز العداد العتبة (يجب الترجمة)
    /// @return true if counter exceeds threshold (should compile)
    bool سجّل_استدعاء(const std::string& الاسم);
    
    // ========================================================================
    // الإحصائيات والتشخيص / Statistics & Diagnostics
    // ========================================================================
    
    /// الحصول على إحصائيات JIT
    /// Get JIT statistics
    const إحصائيات_JIT& الإحصائيات() const { return الإحصائيات_; }
    
    /// إعادة تعيين الإحصائيات
    /// Reset statistics
    void أعد_تعيين_الإحصائيات();
    
    /// طباعة تقرير أداء شامل
    /// Print comprehensive performance report
    void اطبع_التقرير() const;
    
    /// تفريغ ذاكرة الكود المؤقتة
    /// Clear code cache
    void فرّغ_الذاكرة();
    
    /// الحصول على الإعدادات الحالية
    /// Get current settings
    const إعدادات_JIT& الإعدادات() const { return الإعدادات_; }
    
    /// تغيير إعدادات JIT (يؤثر على الترجمات المستقبلية فقط)
    /// Change JIT settings (affects future compilations only)
    void عيّن_إعدادات(const إعدادات_JIT& إعدادات) { الإعدادات_ = إعدادات; }

private:
    // ========================================================================
    // البيانات الخاصة / Private Data
    // ========================================================================
    
    إعدادات_JIT الإعدادات_;                              // الإعدادات / Settings
    إحصائيات_JIT الإحصائيات_;                            // الإحصائيات / Statistics
    bool جاهز_ = false;                                  // حالة الجاهزية / Ready state
    
    /// ذاكرة الكود المؤقتة: اسم الدالة → معلومات الدالة المترجمة
    /// Code cache: function name → compiled function info
    std::unordered_map<std::string, دالة_مترجمة> ذاكرة_الكود_;
    
    /// بايت كود الدوال المسجلة: اسم → بايت كود
    /// Registered function bytecodes: name → bytecode
    std::unordered_map<std::string, vm::وحدة_بايت_كود> بايت_كود_الدوال_;
    
    /// عداد الاستدعاءات لكل دالة
    /// Call counter per function
    std::unordered_map<std::string, uint64_t> عدادات_;
    
    /// القفل للوصول المتزامن (Thread Safety)
    /// Mutex for thread-safe access
    mutable std::mutex القفل_;
    
    // ========================================================================
    // الدوال الخاصة / Private Functions
    // ========================================================================
    
    /// تحويل بايت كود VM إلى تمثيل LLVM IR
    /// Convert VM bytecode to LLVM IR
    bool حوّل_إلى_IR(const std::string& الاسم,
                     const vm::وحدة_بايت_كود& الكود,
                     مستوى_التحسين المستوى);
    
    /// تحسين LLVM IR
    /// Optimize LLVM IR
    bool حسّن_IR(const std::string& الاسم, مستوى_التحسين المستوى);
    
    /// ترجمة LLVM IR إلى كود أصلي
    /// Compile LLVM IR to native code
    void* ولّد_كود_أصلي(const std::string& الاسم);
    
    /// فحص ما إذا كان يجب ترجمة الدالة
    /// Check if function should be compiled
    bool يجب_الترجمة(const std::string& الاسم) const;
    
    /// إدارة سعة ذاكرة الكود المؤقتة
    /// Manage code cache capacity
    void أدر_الذاكرة();
    
    /// إبطال دالة مترجمة (deoptimization)
    /// Invalidate a compiled function
    void أبطل_الترجمة(const std::string& الاسم);

#ifdef HAS_LLVM
    /// بيانات LLVM الداخلية (مُعرّف أمامي)
    /// Internal LLVM data (forward-declared)
    struct بيانات_LLVM;
    std::unique_ptr<بيانات_LLVM> llvm_;
#endif
};

} // namespace jit
} // namespace sad
