// ============================================================================
// jit_bridge.cpp - تنفيذ جسر التكامل بين المفسر ومحرك JIT
// JIT Bridge Implementation
// ============================================================================

#include "jit/jit_bridge.h"
#include "jit/jit_engine.h"
#include <iostream>     // للإخراج القياسي / For standard output
#include <iomanip>      // لتنسيق الإخراج / For output formatting
#include <sstream>      // لمعالجة النصوص / For string processing
#include <chrono>       // للوقت / For time
#include <thread>       // للخيوط / For threads

namespace Sad {
namespace JIT {

// ============================================================================
// التنفيذ الداخلي / Internal Implementation
// ============================================================================

struct JITBridge::Impl {
    std::unique_ptr<JITEngine> jit_engine_;        // محرك JIT / JIT engine
    JITBridgeConfig config_;                       // الإعدادات / Configuration
    ExecutionStatistics stats_;                    // الإحصائيات / Statistics
    
    std::unordered_map<std::string, SadFunction> functions_; // الدوال المسجلة / Registered functions
    std::unordered_map<std::string, size_t> call_counts_;    // عدد الاستدعاءات / Call counts
    std::unordered_map<std::string, double> interpreted_times_; // أوقات المفسر / Interpreter times
    std::unordered_map<std::string, double> jit_times_;      // أوقات JIT / JIT times
    
    std::string last_error_;                       // آخر خطأ / Last error
    bool is_initialized_;                          // هل مُهيأ؟ / Is initialized?
    
    // المُنشئ / Constructor
    Impl() : is_initialized_(false) {}
};

// ============================================================================
// المُنشئ / Constructor
// ============================================================================

JITBridge::JITBridge(const JITBridgeConfig& config)
    : pimpl_(std::make_unique<Impl>())
{
    pimpl_->config_ = config;
}

// ============================================================================
// المُدمر / Destructor
// ============================================================================

JITBridge::~JITBridge() {
    shutdown();
}

// ============================================================================
// التهيئة / Initialize
// ============================================================================

bool JITBridge::initialize() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (pimpl_->is_initialized_) {
        return true; // مُهيأ مسبقاً / Already initialized
    }
    
    try {
        // إنشاء محرك JIT / Create JIT engine
        JITConfig jit_config;
        jit_config.optimization_level = 2;              // O2 افتراضياً / O2 by default
        jit_config.enable_cache = true;                 // تفعيل الذاكرة المؤقتة / Enable caching
        jit_config.enable_tiered_compilation = true;    // تفعيل التجميع المُدرّج / Enable tiering
        jit_config.max_cache_size_mb = 256;             // 256 MB للذاكرة المؤقتة / 256 MB for cache
        
        pimpl_->jit_engine_ = std::make_unique<JITEngine>(jit_config);
        
        // تهيئة محرك JIT / Initialize JIT engine
        if (!pimpl_->jit_engine_->initialize()) {
            pimpl_->last_error_ = "Failed to initialize JIT engine / فشل تهيئة محرك JIT";
            return false;
        }
        
        pimpl_->is_initialized_ = true;
        return true;
        
    } catch (const std::exception& e) {
        pimpl_->last_error_ = std::string("Exception during initialization: ") + e.what();
        return false;
    }
}

// ============================================================================
// إيقاف التشغيل / Shutdown
// ============================================================================

void JITBridge::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (!pimpl_->is_initialized_) {
        return;
    }
    
    // إيقاف محرك JIT / Shutdown JIT engine
    if (pimpl_->jit_engine_) {
        pimpl_->jit_engine_->shutdown();
        pimpl_->jit_engine_.reset();
    }
    
    // مسح البيانات / Clear data
    pimpl_->functions_.clear();
    pimpl_->call_counts_.clear();
    pimpl_->interpreted_times_.clear();
    pimpl_->jit_times_.clear();
    
    pimpl_->is_initialized_ = false;
}

// ============================================================================
// هل مُهيأ؟ / Is Initialized?
// ============================================================================

bool JITBridge::isInitialized() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return pimpl_->is_initialized_;
}

// ============================================================================
// تسجيل دالة Sad / Register Sad Function
// ============================================================================

bool JITBridge::registerFunction(const SadFunction& function) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (function.name.empty()) {
        pimpl_->last_error_ = "Function name cannot be empty / اسم الدالة لا يمكن أن يكون فارغاً";
        return false;
    }
    
    // تسجيل الدالة / Register function
    pimpl_->functions_[function.name] = function;
    pimpl_->call_counts_[function.name] = 0;
    
    return true;
}

// ============================================================================
// تسجيل دالة أصلية / Register Native Function
// ============================================================================

bool JITBridge::registerNativeFunction(
    const std::string& name,
    void* function_ptr,
    const std::vector<std::string>& param_types,
    const std::string& return_type
) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (name.empty() || !function_ptr) {
        pimpl_->last_error_ = "Invalid native function / دالة أصلية غير صالحة";
        return false;
    }
    
    SadFunction func;
    func.name = name;
    func.is_native = true;
    func.native_ptr = function_ptr;
    func.return_type = return_type;
    
    // تسجيل الدالة / Register function
    pimpl_->functions_[name] = func;
    pimpl_->call_counts_[name] = 0;
    
    return true;
}

// ============================================================================
// إلغاء تسجيل دالة / Unregister Function
// ============================================================================

bool JITBridge::unregisterFunction(const std::string& function_name) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = pimpl_->functions_.find(function_name);
    if (it == pimpl_->functions_.end()) {
        return false;
    }
    
    // حذف الدالة / Remove function
    pimpl_->functions_.erase(it);
    pimpl_->call_counts_.erase(function_name);
    pimpl_->interpreted_times_.erase(function_name);
    pimpl_->jit_times_.erase(function_name);
    
    // حذف من محرك JIT / Remove from JIT engine
    if (pimpl_->jit_engine_) {
        pimpl_->jit_engine_->removeFunction(function_name);
    }
    
    return true;
}

// ============================================================================
// تنفيذ دالة / Execute Function
// ============================================================================

SadValue JITBridge::executeFunction(
    const std::string& function_name,
    const std::vector<SadValue>& args
) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    // التحقق من التهيئة / Check initialization
    if (!pimpl_->is_initialized_) {
        pimpl_->last_error_ = "Bridge not initialized / الجسر غير مُهيأ";
        return nullptr;
    }
    
    // التحقق من وجود الدالة / Check function exists
    auto it = pimpl_->functions_.find(function_name);
    if (it == pimpl_->functions_.end()) {
        pimpl_->last_error_ = "Function not found: " + function_name;
        return nullptr;
    }
    
    // زيادة عداد الاستدعاءات / Increment call counter
    pimpl_->call_counts_[function_name]++;
    
    // اختيار طريقة التنفيذ / Choose execution method
    bool use_jit = shouldUseJIT(function_name);
    
    SadValue result = nullptr;
    
    try {
        if (use_jit) {
            // تنفيذ بـ JIT / Execute with JIT
            result = executeJIT(function_name, args);
            
            // إذا فشل JIT والرجوع مفعّل / If JIT failed and fallback enabled
            if (std::holds_alternative<std::nullptr_t>(result) && pimpl_->config_.enable_fallback) {
                pimpl_->stats_.fallback_count++;
                result = executeInterpreted(function_name, args);
            }
        } else {
            // تنفيذ بالمفسر / Execute with interpreter
            result = executeInterpreted(function_name, args);
        }
        
    } catch (const std::exception& e) {
        pimpl_->last_error_ = std::string("Execution error: ") + e.what();
        
        // محاولة الرجوع للمفسر / Try fallback to interpreter
        if (use_jit && pimpl_->config_.enable_fallback) {
            try {
                result = executeInterpreted(function_name, args);
            } catch (...) {
                return nullptr;
            }
        }
    }
    
    return result;
}

// ============================================================================
// تنفيذ بالمفسر / Execute with Interpreter
// ============================================================================

SadValue JITBridge::executeInterpreted(
    const std::string& function_name,
    const std::vector<SadValue>& args
) {
    // ملاحظة: لا قفل هنا - المُستدعي مسؤول / Note: No lock - caller responsible
    
    auto start_time = std::chrono::steady_clock::now();
    
    // TODO: استدعاء المفسر الفعلي / Call actual interpreter
    // هذا مثال توضيحي / This is a demonstration
    
    // محاكاة التنفيذ / Simulate execution
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    
    auto end_time = std::chrono::steady_clock::now();
    double time_us = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time
    ).count();
    
    // تسجيل الوقت / Record time
    recordInterpreterCall(function_name, time_us);
    
    // إرجاع قيمة افتراضية / Return default value
    return static_cast<int64_t>(42);
}

// ============================================================================
// تنفيذ بـ JIT / Execute with JIT
// ============================================================================

SadValue JITBridge::executeJIT(
    const std::string& function_name,
    const std::vector<SadValue>& args
) {
    // ملاحظة: لا قفل هنا - المُستدعي مسؤول / Note: No lock - caller responsible
    
    if (!pimpl_->jit_engine_) {
        return nullptr;
    }
    
    auto start_time = std::chrono::steady_clock::now();
    
    // التحقق من التجميع / Check if compiled
    if (!pimpl_->jit_engine_->isFunctionCompiled(function_name)) {
        // تجميع الدالة / Compile function
        auto& func_info = pimpl_->functions_[function_name];
        auto result = pimpl_->jit_engine_->compileFunction(
            function_name,
            func_info.source_code
        );
        
        if (!result.success) {
            pimpl_->last_error_ = "Compilation failed: " + result.error_message;
            return nullptr;
        }
    }
    
    // تحويل المعاملات / Convert arguments
    std::vector<void*> arg_ptrs = convertArgsToPointers(args);
    
    // تنفيذ الدالة المُجمّعة / Execute compiled function
    void* result_ptr = pimpl_->jit_engine_->executeFunction(function_name, arg_ptrs);
    
    auto end_time = std::chrono::steady_clock::now();
    double time_us = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time
    ).count();
    
    // تسجيل الوقت / Record time
    recordJITCall(function_name, time_us);
    
    // تحويل النتيجة / Convert result
    auto& func_info = pimpl_->functions_[function_name];
    return pointerToSadValue(result_ptr, func_info.return_type);
}

// ============================================================================
// هل يجب استخدام JIT؟ / Should Use JIT?
// ============================================================================

bool JITBridge::shouldUseJIT(const std::string& function_name) const {
    // ملاحظة: لا قفل هنا - المُستدعي مسؤول / Note: No lock - caller responsible
    
    // التحقق من وضع التنفيذ / Check execution mode
    switch (pimpl_->config_.execution_mode) {
        case ExecutionMode::INTERPRETER_ONLY:
            return false;
            
        case ExecutionMode::JIT_ONLY:
            return true;
            
        case ExecutionMode::HYBRID:
        case ExecutionMode::ADAPTIVE: {
            // استخدام JIT إذا تجاوز العتبة / Use JIT if exceeds threshold
            auto it = pimpl_->call_counts_.find(function_name);
            if (it != pimpl_->call_counts_.end()) {
                return it->second >= pimpl_->config_.jit_threshold;
            }
            return false;
        }
        
        default:
            return false;
    }
}

// ============================================================================
// هل الدالة ساخنة؟ / Is Function Hot?
// ============================================================================

bool JITBridge::isHotFunction(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (!pimpl_->jit_engine_) {
        return false;
    }
    
    return pimpl_->jit_engine_->isHotFunction(function_name);
}

// ============================================================================
// إجبار التجميع / Force Compile
// ============================================================================

bool JITBridge::forceCompile(const std::string& function_name) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (!pimpl_->is_initialized_ || !pimpl_->jit_engine_) {
        return false;
    }
    
    auto it = pimpl_->functions_.find(function_name);
    if (it == pimpl_->functions_.end()) {
        return false;
    }
    
    // تجميع الدالة / Compile function
    auto result = pimpl_->jit_engine_->compileFunction(
        function_name,
        it->second.source_code
    );
    
    return result.success;
}

// ============================================================================
// تحويل قيمة Sad إلى مؤشر / Convert Sad Value to Pointer
// ============================================================================

void* JITBridge::sadValueToPointer(const SadValue& value) const {
    // تحويل بناءً على النوع / Convert based on type
    if (std::holds_alternative<std::nullptr_t>(value)) {
        return nullptr;
    } else if (std::holds_alternative<bool>(value)) {
        static bool temp;
        temp = std::get<bool>(value);
        return &temp;
    } else if (std::holds_alternative<int64_t>(value)) {
        static int64_t temp;
        temp = std::get<int64_t>(value);
        return &temp;
    } else if (std::holds_alternative<double>(value)) {
        static double temp;
        temp = std::get<double>(value);
        return &temp;
    } else if (std::holds_alternative<std::string>(value)) {
        static std::string temp;
        temp = std::get<std::string>(value);
        return const_cast<char*>(temp.c_str());
    } else if (std::holds_alternative<void*>(value)) {
        return std::get<void*>(value);
    }
    
    return nullptr;
}

// ============================================================================
// تحويل مؤشر إلى قيمة Sad / Convert Pointer to Sad Value
// ============================================================================

SadValue JITBridge::pointerToSadValue(void* ptr, const std::string& type) const {
    if (!ptr) {
        return nullptr;
    }
    
    // تحويل بناءً على النوع / Convert based on type
    if (type == "int" || type == "عدد_صحيح") {
        return *static_cast<int64_t*>(ptr);
    } else if (type == "float" || type == "عدد_عشري") {
        return *static_cast<double*>(ptr);
    } else if (type == "bool" || type == "منطقي") {
        return *static_cast<bool*>(ptr);
    } else if (type == "string" || type == "نص") {
        return std::string(static_cast<char*>(ptr));
    }
    
    return ptr; // إرجاع كمؤشر عام / Return as generic pointer
}

// ============================================================================
// تحويل المعاملات إلى مؤشرات / Convert Args to Pointers
// ============================================================================

std::vector<void*> JITBridge::convertArgsToPointers(const std::vector<SadValue>& args) const {
    std::vector<void*> ptrs;
    ptrs.reserve(args.size());
    
    for (const auto& arg : args) {
        ptrs.push_back(sadValueToPointer(arg));
    }
    
    return ptrs;
}

// ============================================================================
// الحصول على الإحصائيات / Get Statistics
// ============================================================================

const ExecutionStatistics& JITBridge::getStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    // تحديث معامل التسريع / Update speedup factor
    if (pimpl_->stats_.avg_jit_time_us > 0) {
        pimpl_->stats_.speedup_factor = 
            pimpl_->stats_.avg_interpreted_time_us / pimpl_->stats_.avg_jit_time_us;
    }
    
    return pimpl_->stats_;
}

// ============================================================================
// إعادة تعيين الإحصائيات / Reset Statistics
// ============================================================================

void JITBridge::resetStatistics() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    pimpl_->stats_ = ExecutionStatistics();
    pimpl_->call_counts_.clear();
    pimpl_->interpreted_times_.clear();
    pimpl_->jit_times_.clear();
    
    if (pimpl_->jit_engine_) {
        pimpl_->jit_engine_->resetStatistics();
    }
}

// ============================================================================
// طباعة الإحصائيات / Print Statistics
// ============================================================================

void JITBridge::printStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    const ExecutionStatistics& stats = getStatistics();
    
    std::cout << "\n========================================\n";
    std::cout << " JIT Bridge Statistics\n";
    std::cout << " إحصائيات جسر JIT\n";
    std::cout << "========================================\n";
    std::cout << "Interpreted Calls: " << stats.interpreted_calls << "\n";
    std::cout << "JIT Calls:         " << stats.jit_calls << "\n";
    std::cout << "Fallback Count:    " << stats.fallback_count << "\n";
    std::cout << "Avg Interp Time:   " << std::fixed << std::setprecision(2) 
              << stats.avg_interpreted_time_us << " μs\n";
    std::cout << "Avg JIT Time:      " << std::fixed << std::setprecision(2) 
              << stats.avg_jit_time_us << " μs\n";
    std::cout << "Speedup Factor:    " << std::fixed << std::setprecision(2) 
              << stats.speedup_factor << "x\n";
    std::cout << "========================================\n\n";
}

// ============================================================================
// الحصول على الإعدادات / Get Configuration
// ============================================================================

const JITBridgeConfig& JITBridge::getConfig() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return pimpl_->config_;
}

// ============================================================================
// تحديث الإعدادات / Update Configuration
// ============================================================================

void JITBridge::updateConfig(const JITBridgeConfig& new_config) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    pimpl_->config_ = new_config;
}

// ============================================================================
// تغيير وضع التنفيذ / Change Execution Mode
// ============================================================================

void JITBridge::setExecutionMode(ExecutionMode mode) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    pimpl_->config_.execution_mode = mode;
}

// ============================================================================
// الحصول على محرك JIT / Get JIT Engine
// ============================================================================

JITEngine* JITBridge::getJITEngine() {
    return pimpl_->jit_engine_.get();
}

const JITEngine* JITBridge::getJITEngine() const {
    return pimpl_->jit_engine_.get();
}

// ============================================================================
// الحصول على معلومات دالة / Get Function Info
// ============================================================================

const SadFunction* JITBridge::getFunctionInfo(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    auto it = pimpl_->functions_.find(function_name);
    if (it != pimpl_->functions_.end()) {
        return &(it->second);
    }
    
    return nullptr;
}

// ============================================================================
// عدد الدوال المسجلة / Number of Registered Functions
// ============================================================================

size_t JITBridge::getRegisteredFunctionCount() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return pimpl_->functions_.size();
}

// ============================================================================
// الحصول على جميع أسماء الدوال / Get All Function Names
// ============================================================================

std::vector<std::string> JITBridge::getRegisteredFunctionNames() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    std::vector<std::string> names;
    names.reserve(pimpl_->functions_.size());
    
    for (const auto& pair : pimpl_->functions_) {
        names.push_back(pair.first);
    }
    
    return names;
}

// ============================================================================
// الحصول على آخر خطأ / Get Last Error
// ============================================================================

std::string JITBridge::getLastError() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return pimpl_->last_error_;
}

// ============================================================================
// مسح الأخطاء / Clear Errors
// ============================================================================

void JITBridge::clearErrors() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    pimpl_->last_error_.clear();
}

// ============================================================================
// تسجيل استدعاء المفسر / Record Interpreter Call
// ============================================================================

void JITBridge::recordInterpreterCall(const std::string& function_name, double time_us) {
    // ملاحظة: لا قفل هنا - المُستدعي مسؤول / Note: No lock - caller responsible
    
    pimpl_->stats_.interpreted_calls++;
    pimpl_->interpreted_times_[function_name] = time_us;
    
    // حساب المتوسط / Calculate average
    double total = 0.0;
    for (const auto& pair : pimpl_->interpreted_times_) {
        total += pair.second;
    }
    pimpl_->stats_.avg_interpreted_time_us = total / pimpl_->interpreted_times_.size();
}

// ============================================================================
// تسجيل استدعاء JIT / Record JIT Call
// ============================================================================

void JITBridge::recordJITCall(const std::string& function_name, double time_us) {
    // ملاحظة: لا قفل هنا - المُستدعي مسؤول / Note: No lock - caller responsible
    
    pimpl_->stats_.jit_calls++;
    pimpl_->jit_times_[function_name] = time_us;
    
    // حساب المتوسط / Calculate average
    double total = 0.0;
    for (const auto& pair : pimpl_->jit_times_) {
        total += pair.second;
    }
    pimpl_->stats_.avg_jit_time_us = total / pimpl_->jit_times_.size();
}

// ============================================================================
// دوال مساعدة عامة / Global Helper Functions
// ============================================================================

std::string sadValueToString(const SadValue& value) {
    std::ostringstream oss;
    
    if (std::holds_alternative<std::nullptr_t>(value)) {
        oss << "null";
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::get<double>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        oss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<void*>(value)) {
        oss << "<pointer:" << std::get<void*>(value) << ">";
    }
    
    return oss.str();
}

std::string sadValueTypeName(const SadValue& value) {
    if (std::holds_alternative<std::nullptr_t>(value)) return "null";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<int64_t>(value)) return "int";
    if (std::holds_alternative<double>(value)) return "float";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<void*>(value)) return "pointer";
    return "unknown";
}

bool sadValuesEqual(const SadValue& a, const SadValue& b) {
    return a.index() == b.index() && a == b;
}

} // namespace JIT
} // namespace Sad

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
