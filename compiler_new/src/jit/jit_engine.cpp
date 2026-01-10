// ============================================================================
// jit_engine.cpp - تنفيذ محرك التجميع الفوري (JIT)
// JIT Engine Implementation
// ============================================================================

#include "jit/jit_engine.h"
#include "jit/jit_cache.h"
#include "jit/hot_path_detector.h"
#include <algorithm>    // لدوال الخوارزميات / For algorithms
#include <sstream>      // لمعالجة النصوص / For string processing
#include <iomanip>      // لتنسيق الإخراج / For output formatting
#include <iostream>     // للإخراج القياسي / For standard output
#include <cstring>      // لدوال C للنصوص / For C string functions

// تضمين LLVM Backend (Phase 1.2.1) / Include LLVM Backend (Phase 1.2.1)
// المصدر: compiler/backend/llvm/include/ / Source: compiler/backend/llvm/include/
#ifdef ENABLE_LLVM_BACKEND
#include "backend/llvm/llvm_generator.h"
#include "backend/llvm/llvm_context.h"
#endif

namespace Sad {
namespace JIT {

// ============================================================================
// Pimpl Implementation - التنفيذ الداخلي
// ============================================================================

// هيكل داخلي لإخفاء تفاصيل LLVM / Internal structure to hide LLVM details
struct JITEngine::Impl {
#ifdef ENABLE_LLVM_BACKEND
    // ============================================================================
    // LLVM Components (Phase 1.2.1) / مكونات LLVM
    // المصدر: compiler/backend/llvm/include/ / Source: compiler/backend/llvm/include/
    // ============================================================================
    
    // مدير سياق LLVM / LLVM context manager
    // المصدر: llvm_context.h / Source: llvm_context.h
    std::unique_ptr<LLVMContextManager> llvm_context_mgr_;
    
    // مولد LLVM IR / LLVM IR generator
    // المصدر: llvm_generator.h / Source: llvm_generator.h
    std::unique_ptr<LLVMGenerator> llvm_generator_;
    
    // خريطة الدوال المُجمّعة / Compiled functions map
    // key: اسم الدالة / function name
    // value: مؤشر الدالة المُجمّعة / compiled function pointer
    std::unordered_map<std::string, void*> compiled_functions_;
#endif
    
    // Sad Components
    std::unique_ptr<JITCache> cache_;              // الذاكرة المؤقتة / Cache
    std::unique_ptr<HotPathDetector> detector_;    // كاشف المسارات الساخنة / Hot path detector
    JITConfig config_;                             // الإعدادات / Configuration
    JITStatistics stats_;                          // الإحصائيات / Statistics
    
    // معلومات النظام / System Information
    std::string target_triple_;                    // معمارية الهدف / Target architecture
    std::string cpu_name_;                         // اسم المعالج / CPU name
    std::string cpu_features_;                     // خصائص المعالج / CPU features
    
    // حالة الحياة / Lifetime State
    bool is_initialized_;                          // هل تم التهيئة؟ / Is initialized?
    std::chrono::steady_clock::time_point init_time_; // وقت التهيئة / Initialization time
    
    // المُنشئ / Constructor
    Impl() 
        : is_initialized_(false)
    {}
};

// ============================================================================
// المُنشئ / Constructor
// ============================================================================

JITEngine::JITEngine(const JITConfig& config)
    : pimpl_(std::make_unique<Impl>())  // إنشاء التنفيذ الداخلي / Create implementation
    , mutex_()                           // تهيئة mutex / Initialize mutex
{
    pimpl_->config_ = config;           // حفظ الإعدادات / Store configuration
    
    // إنشاء الذاكرة المؤقتة / Create cache
    if (config.enable_cache) {
        pimpl_->cache_ = std::make_unique<JITCache>(
            config.max_cache_size_mb,
            CacheEvictionPolicy::LRU
        );
    }
    
    // إنشاء كاشف المسارات الساخنة / Create hot path detector
    if (config.enable_tiered_compilation) {
        TieringConfig tier_config;
        tier_config.tier1_threshold = 10;    // O0
        tier_config.tier2_threshold = 50;    // O1
        tier_config.tier3_threshold = 200;   // O2
        tier_config.tier4_threshold = 1000;  // O3 + PGO
        
        pimpl_->detector_ = std::make_unique<HotPathDetector>(tier_config);
        
        // تسجيل callback للمسارات الساخنة / Register callback for hot paths
        pimpl_->detector_->onHotPath([this](const HotPathInfo& info) {
            handleHotPath(info);
        });
    }
}

// ============================================================================
// المُدمر / Destructor
// ============================================================================

JITEngine::~JITEngine() {
    // تنظيف الموارد / Clean up resources
    shutdown();
}

// ============================================================================
// التهيئة / Initialize
// ============================================================================

bool JITEngine::initialize(const std::string& target_triple) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (pimpl_->is_initialized_) {
        return true; // مُهيأ مسبقاً / Already initialized
    }
    
    try {
        // حفظ معلومات النظام / Store system information
        pimpl_->target_triple_ = target_triple.empty() ? detectNativeTarget() : target_triple;
        pimpl_->cpu_name_ = detectCPUName();
        pimpl_->cpu_features_ = detectCPUFeatures();
        
#ifdef ENABLE_LLVM_BACKEND
        // ============================================================================
        // تهيئة LLVM ORC JIT (Phase 1.2.1) / Initialize LLVM ORC JIT
        // ============================================================================
        
        // 1. إنشاء مدير سياق LLVM / Create LLVM context manager
        // المصدر: llvm_context.h:LLVMContextManager / Source: llvm_context.h:LLVMContextManager
        pimpl_->llvm_context_mgr_ = std::make_unique<LLVMContextManager>();
        
        // 2. تهيئة مدير السياق / Initialize context manager
        // المصدر: llvm_context.h:initialize() / Source: llvm_context.h:initialize()
        if (!pimpl_->llvm_context_mgr_->initialize(pimpl_->target_triple_)) {
            std::cerr << "❌ فشل تهيئة LLVM Context Manager\n";
            std::cerr << "❌ Failed to initialize LLVM Context Manager\n";
            std::cerr << "Error: " << pimpl_->llvm_context_mgr_->getLastError() << "\n";
            return false;
        }
        
        // 3. إنشاء مولد LLVM IR / Create LLVM IR generator
        // المصدر: llvm_generator.h:LLVMGenerator / Source: llvm_generator.h:LLVMGenerator
        pimpl_->llvm_generator_ = std::make_unique<LLVMGenerator>();
        
        // 4. تهيئة المولد مع السياق / Initialize generator with context
        // المصدر: llvm_generator.h:initialize() / Source: llvm_generator.h:initialize()
        if (!pimpl_->llvm_generator_->initialize(
            pimpl_->llvm_context_mgr_->getContext(),
            "sad_jit_module"
        )) {
            std::cerr << "❌ فشل تهيئة LLVM Generator\n";
            std::cerr << "❌ Failed to initialize LLVM Generator\n";
            std::cerr << "Error: " << pimpl_->llvm_generator_->getLastError() << "\n";
            return false;
        }
        
        std::cout << "✅ LLVM Backend initialized successfully\n";
        std::cout << "   Target: " << pimpl_->target_triple_ << "\n";
        std::cout << "   CPU: " << pimpl_->cpu_name_ << "\n";
#else
        // TODO: تهيئة LLVM ORC JIT / Initialize LLVM ORC JIT
        // - إنشاء ExecutionSession
        // - إنشاء ObjectLinkingLayer
        // - إنشاء IRCompileLayer
        // - تسجيل الدوال المضمنة (built-in functions)
        
        std::cout << "⚠️  LLVM Backend disabled - using simulation mode\n";
#endif
        
        // وضع علامة التهيئة / Mark as initialized
        pimpl_->is_initialized_ = true;
        pimpl_->init_time_ = std::chrono::steady_clock::now();
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "خطأ في تهيئة JIT Engine: " << e.what() << "\n";
        std::cerr << "JIT Engine initialization error: " << e.what() << "\n";
        return false;
    }
}

// ============================================================================
// إيقاف التشغيل / Shutdown
// ============================================================================

void JITEngine::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (!pimpl_->is_initialized_) {
        return; // غير مُهيأ / Not initialized
    }
    
    // TODO: تنظيف موارد LLVM / Clean up LLVM resources
    
    // مسح الذاكرة المؤقتة / Clear cache
    if (pimpl_->cache_) {
        pimpl_->cache_->clear();
    }
    
    // إعادة تعيين الحالة / Reset state
    pimpl_->is_initialized_ = false;
}

// ============================================================================
// تجميع دالة / Compile Function
// ============================================================================

JITCompilationResult JITEngine::compileFunction(
    const std::string& function_name,
    const std::string& source_code
) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    JITCompilationResult result;
    result.function_name = function_name;
    result.success = false;
    
    // التحقق من التهيئة / Check initialization
    if (!pimpl_->is_initialized_) {
        result.error_message = "JIT Engine not initialized / محرك JIT غير مُهيأ";
        return result;
    }
    
    // بدء قياس الوقت / Start timing
    auto start_time = std::chrono::steady_clock::now();
    
    try {
        // التحقق من الذاكرة المؤقتة / Check cache
        if (pimpl_->cache_) {
            const CacheEntry* cached = pimpl_->cache_->get(function_name);
            if (cached && cached->compiled_code) {
                // وُجد في الذاكرة المؤقتة! / Found in cache!
                result.success = true;
                result.compiled_function = cached->compiled_code;
                result.code_size_bytes = cached->code_size_bytes;
                result.optimization_level = cached->optimization_level;
                result.was_cached = true;
                
                // تحديث الإحصائيات / Update statistics
                pimpl_->stats_.cache_hits++;
                
                return result;
            }
            
            pimpl_->stats_.cache_misses++; // إخفاق الذاكرة المؤقتة / Cache miss
        }
        
        // تحديد مستوى التحسين / Determine optimization level
        int opt_level = determineOptimizationLevel(function_name);
        result.optimization_level = opt_level;
        
#ifdef ENABLE_LLVM_BACKEND
        // ============================================================================
        // التجميع الفعلي باستخدام LLVM (Phase 1.2.1) / Actual compilation using LLVM
        // ============================================================================
        
        // 1. توليد LLVM IR من الكود المصدري / Generate LLVM IR from source code
        // المصدر: llvm_generator.h:generateFromSource() / Source: llvm_generator.h:generateFromSource()
        auto module = pimpl_->llvm_generator_->generateFromSource(source_code);
        
        // 2. التحقق من نجاح التوليد / Verify generation succeeded
        if (!module) {
            result.error_message = "Failed to generate LLVM IR: " + 
                                  pimpl_->llvm_generator_->getLastError();
            pimpl_->stats_.failed_compilations++;
            return result;
        }
        
        // 3. التحقق من صحة الوحدة / Verify module correctness
        // المصدر: llvm_generator.h:verify() / Source: llvm_generator.h:verify()
        std::string verify_error;
        if (!pimpl_->llvm_generator_->verify(&verify_error)) {
            result.error_message = "LLVM IR verification failed: " + verify_error;
            pimpl_->stats_.failed_compilations++;
            return result;
        }
        
        // 4. تطبيق التحسينات / Apply optimizations
        // المصدر: llvm_generator.h:optimize() / Source: llvm_generator.h:optimize()
        if (opt_level > 0) {
            pimpl_->llvm_generator_->optimize(opt_level);
        }
        
        // 5. إنشاء وحدة آمنة للخيوط / Create thread-safe module
        // المصدر: llvm_context.h:createThreadSafeModule() / Source: llvm_context.h:createThreadSafeModule()
        // ملاحظة: نحتاج نقل ملكية module إلى ThreadSafeModule
        // Note: Need to transfer module ownership to ThreadSafeModule
        
        // استخدام getContext و createThreadSafeModule بشكل صحيح
        // Use getContext and createThreadSafeModule correctly
        auto& ts_context = pimpl_->llvm_context_mgr_->getThreadSafeContext();
        
        // نقل ملكية module إلى ThreadSafeModule
        // Transfer module ownership to ThreadSafeModule
        llvm::orc::ThreadSafeModule ts_module(
            std::move(module),
            ts_context
        );
        
        // 6. إضافة الوحدة إلى JIT / Add module to JIT
        // المصدر: llvm_context.h:addModule() / Source: llvm_context.h:addModule()
        if (!pimpl_->llvm_context_mgr_->addModule(std::move(ts_module))) {
            result.error_message = "Failed to add module to JIT: " + 
                                  pimpl_->llvm_context_mgr_->getLastError();
            pimpl_->stats_.failed_compilations++;
            return result;
        }
        
        // 7. البحث عن الدالة المُجمّعة / Lookup compiled function
        // المصدر: llvm_context.h:lookupFunction() / Source: llvm_context.h:lookupFunction()
        void* compiled_ptr = pimpl_->llvm_context_mgr_->lookupFunction(function_name);
        
        if (!compiled_ptr) {
            result.error_message = "Failed to find function after compilation: " + function_name;
            pimpl_->stats_.failed_compilations++;
            return result;
        }
        
        // 8. حفظ مؤشر الدالة / Store function pointer
        // المصدر: pimpl_->compiled_functions_ / Source: pimpl_->compiled_functions_
        pimpl_->compiled_functions_[function_name] = compiled_ptr;
        
        // 9. تحديث النتيجة / Update result
        result.success = true;
        result.compiled_function = compiled_ptr;
        result.code_size_bytes = source_code.size() * 2; // تقدير / Estimate
        result.was_cached = false;
        
#else
        // TODO: التجميع الفعلي باستخدام LLVM / Actual compilation using LLVM
        // 1. تحويل source_code إلى LLVM IR
        // 2. تطبيق التحسينات بناءً على opt_level
        // 3. تجميع IR إلى كود آلة
        // 4. ربط الكود
        // 5. الحصول على مؤشر الدالة
        
        // للتوضيح - سنحاكي التجميع / For demonstration - simulate compilation
        void* compiled_ptr = simulateCompilation(function_name, source_code, opt_level);
        
        if (compiled_ptr) {
            result.success = true;
            result.compiled_function = compiled_ptr;
            result.code_size_bytes = source_code.size() * 2; // تقدير / Estimate
            result.was_cached = false;
#endif
            
            // إضافة إلى الذاكرة المؤقتة / Add to cache
            if (pimpl_->cache_) {
                CacheEntry entry;
                entry.function_name = function_name;
#ifdef ENABLE_LLVM_BACKEND
                entry.compiled_code = compiled_ptr;
#else
                entry.compiled_code = compiled_ptr;
#endif
                entry.code_size_bytes = result.code_size_bytes;
                entry.optimization_level = opt_level;
                entry.source_hash = calculateHash(source_code);
                
                pimpl_->cache_->put(entry);
            }
            
            // تحديث الإحصائيات / Update statistics
            pimpl_->stats_.total_compilations++;
            pimpl_->stats_.successful_compilations++;
            
#ifndef ENABLE_LLVM_BACKEND
        } else {
            result.error_message = "Compilation failed / فشل التجميع";
        }
#endif
        
    } catch (const std::exception& e) {
        result.error_message = std::string("Exception: ") + e.what();
        pimpl_->stats_.failed_compilations++;
    }
    
    // حساب وقت التجميع / Calculate compilation time
    auto end_time = std::chrono::steady_clock::now();
    result.compilation_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time
    ).count();
    
    pimpl_->stats_.total_compilation_time_ms += result.compilation_time_ms;
    
    return result;
}

// ============================================================================
// تجميع وحدة / Compile Module
// ============================================================================

JITCompilationResult JITEngine::compileModule(
    const std::string& module_name,
    const std::string& source_code
) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    JITCompilationResult result;
    result.function_name = module_name;
    result.success = false;
    
    // TODO: تجميع وحدة كاملة / Compile entire module
    // مشابه لـ compileFunction لكن مع معالجة متعددة للدوال / Similar to compileFunction but handles multiple functions
    
    return result;
}

// ============================================================================
// تنفيذ دالة / Execute Function
// ============================================================================

void* JITEngine::executeFunction(
    const std::string& function_name,
    const std::vector<void*>& args
) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    // التحقق من التهيئة / Check initialization
    if (!pimpl_->is_initialized_) {
        return nullptr;
    }
    
    // بدء قياس الوقت / Start timing
    auto start_time = std::chrono::steady_clock::now();
    
    void* result = nullptr;
    
    try {
        // الحصول على الدالة المُجمّعة / Get compiled function
        void* func_ptr = getFunctionPointer(function_name);
        
        if (!func_ptr) {
            std::cerr << "الدالة غير موجودة: " << function_name << "\n";
            std::cerr << "Function not found: " << function_name << "\n";
            return nullptr;
        }
        
        // TODO: استدعاء الدالة المُجمّعة / Call compiled function
        // result = invokeCompiledFunction(func_ptr, args);
        
        // تحديث الإحصائيات / Update statistics
        pimpl_->stats_.total_executions++;
        
        // تسجيل التنفيذ في الكاشف / Record execution in detector
        if (pimpl_->detector_) {
            auto end_time = std::chrono::steady_clock::now();
            size_t exec_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
                end_time - start_time
            ).count();
            
            pimpl_->detector_->recordExecution(function_name, exec_time_us);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "خطأ في التنفيذ: " << e.what() << "\n";
        std::cerr << "Execution error: " << e.what() << "\n";
    }
    
    return result;
}

// ============================================================================
// الحصول على مؤشر دالة / Get Function Pointer
// ============================================================================

void* JITEngine::getFunctionPointer(const std::string& function_name) {
    // ملاحظة: لا قفل هنا - يُفترض أن المُستدعي قد قفل / Note: No lock here - caller assumed to have locked
    
#ifdef ENABLE_LLVM_BACKEND
    // ============================================================================
    // البحث في الدوال المُجمّعة (Phase 1.2.1) / Search in compiled functions
    // ============================================================================
    
    // 1. البحث في خريطة الدوال المُجمّعة / Search in compiled functions map
    // المصدر: pimpl_->compiled_functions_ / Source: pimpl_->compiled_functions_
    auto it = pimpl_->compiled_functions_.find(function_name);
    if (it != pimpl_->compiled_functions_.end()) {
        return it->second; // وُجدت الدالة! / Function found!
    }
    
    // 2. محاولة البحث في LLVM JIT / Try to lookup in LLVM JIT
    // المصدر: llvm_context.h:lookupFunction() / Source: llvm_context.h:lookupFunction()
    if (pimpl_->llvm_context_mgr_ && pimpl_->llvm_context_mgr_->isInitialized()) {
        void* func_ptr = pimpl_->llvm_context_mgr_->lookupFunction(function_name);
        if (func_ptr) {
            // حفظ في الخريطة للوصول السريع / Cache in map for fast access
            pimpl_->compiled_functions_[function_name] = func_ptr;
            return func_ptr;
        }
    }
#endif
    
    // البحث في الذاكرة المؤقتة / Search in cache
    if (pimpl_->cache_) {
        const CacheEntry* entry = pimpl_->cache_->get(function_name);
        if (entry) {
            return entry->compiled_code;
        }
    }
    
    // TODO: البحث في LLVM execution session / Search in LLVM execution session
    
    return nullptr;
}

// ============================================================================
// هل الدالة مُجمّعة؟ / Is Function Compiled?
// ============================================================================

bool JITEngine::isFunctionCompiled(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    // التحقق من الذاكرة المؤقتة / Check cache
    if (pimpl_->cache_) {
        return pimpl_->cache_->contains(function_name);
    }
    
    // TODO: التحقق من LLVM execution session / Check LLVM execution session
    
    return false;
}

// ============================================================================
// حذف دالة مُجمّعة / Remove Compiled Function
// ============================================================================

bool JITEngine::removeFunction(const std::string& function_name) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    bool removed = false;
    
    // حذف من الذاكرة المؤقتة / Remove from cache
    if (pimpl_->cache_) {
        removed = pimpl_->cache_->remove(function_name);
    }
    
    // TODO: حذف من LLVM execution session / Remove from LLVM execution session
    
    return removed;
}

// ============================================================================
// مسح الذاكرة المؤقتة / Clear Cache
// ============================================================================

void JITEngine::clearCache() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (pimpl_->cache_) {
        pimpl_->cache_->clear();
    }
}

// ============================================================================
// تخزين دالة في الذاكرة المؤقتة / Cache Function
// ============================================================================

void JITEngine::cacheFunction(const std::string& function_name, void* function_ptr) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (!pimpl_->cache_ || !function_ptr) {
        return;
    }
    
    CacheEntry entry;
    entry.function_name = function_name;
    entry.compiled_code = function_ptr;
    entry.code_size_bytes = 0; // غير معروف / Unknown
    entry.optimization_level = pimpl_->config_.optimization_level;
    
    pimpl_->cache_->put(entry);
}

// ============================================================================
// هل الدالة ساخنة؟ / Is Function Hot?
// ============================================================================

bool JITEngine::isHotFunction(const std::string& function_name) const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    if (!pimpl_->detector_) {
        return false;
    }
    
    return pimpl_->detector_->isHot(function_name);
}

// ============================================================================
// الحصول على الإحصائيات / Get Statistics
// ============================================================================

const JITStatistics& JITEngine::getStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    // تحديث إحصائيات الذاكرة المؤقتة / Update cache statistics
    if (pimpl_->cache_) {
        pimpl_->stats_.cache_size_bytes = pimpl_->cache_->getCurrentSize();
        pimpl_->stats_.cached_functions = pimpl_->cache_->getCount();
        pimpl_->stats_.cache_hit_rate_percent = pimpl_->cache_->getHitRate();
    }
    
    // تحديث إحصائيات المسارات الساخنة / Update hot path statistics
    if (pimpl_->detector_) {
        pimpl_->stats_.hot_functions = pimpl_->detector_->getHotPathsCount();
    }
    
    // حساب متوسط وقت التجميع / Calculate average compilation time
    if (pimpl_->stats_.total_compilations > 0) {
        pimpl_->stats_.avg_compilation_time_ms = 
            pimpl_->stats_.total_compilation_time_ms / pimpl_->stats_.total_compilations;
    }
    
    return pimpl_->stats_;
}

// ============================================================================
// إعادة تعيين الإحصائيات / Reset Statistics
// ============================================================================

void JITEngine::resetStatistics() {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    pimpl_->stats_ = JITStatistics(); // إعادة تعيين / Reset
    
    // إعادة تعيين إحصائيات الذاكرة المؤقتة / Reset cache statistics
    if (pimpl_->cache_) {
        pimpl_->cache_->resetStatistics();
    }
    
    // إعادة تعيين كاشف المسارات الساخنة / Reset hot path detector
    if (pimpl_->detector_) {
        pimpl_->detector_->reset();
    }
}

// ============================================================================
// طباعة الإحصائيات / Print Statistics
// ============================================================================

void JITEngine::printStatistics() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    const JITStatistics& stats = getStatistics();
    
    std::cout << "\n========================================\n";
    std::cout << " JIT Engine Statistics\n";
    std::cout << " إحصائيات محرك JIT\n";
    std::cout << "========================================\n";
    std::cout << "Compilations:      " << stats.total_compilations << "\n";
    std::cout << "  Successful:      " << stats.successful_compilations << "\n";
    std::cout << "  Failed:          " << stats.failed_compilations << "\n";
    std::cout << "Executions:        " << stats.total_executions << "\n";
    std::cout << "Cache Hits:        " << stats.cache_hits << "\n";
    std::cout << "Cache Misses:      " << stats.cache_misses << "\n";
    std::cout << "Cache Hit Rate:    " << std::fixed << std::setprecision(2) 
              << stats.cache_hit_rate_percent << "%\n";
    std::cout << "Cached Functions:  " << stats.cached_functions << "\n";
    std::cout << "Hot Functions:     " << stats.hot_functions << "\n";
    std::cout << "Avg Compile Time:  " << stats.avg_compilation_time_ms << " ms\n";
    std::cout << "========================================\n\n";
}

// ============================================================================
// تصدير الإحصائيات إلى JSON / Export Statistics to JSON
// ============================================================================

std::string JITEngine::statisticsToJSON() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    
    const JITStatistics& stats = getStatistics();
    
    std::ostringstream json;
    json << "{\n";
    json << "  \"total_compilations\": " << stats.total_compilations << ",\n";
    json << "  \"successful_compilations\": " << stats.successful_compilations << ",\n";
    json << "  \"failed_compilations\": " << stats.failed_compilations << ",\n";
    json << "  \"total_executions\": " << stats.total_executions << ",\n";
    json << "  \"cache_hits\": " << stats.cache_hits << ",\n";
    json << "  \"cache_misses\": " << stats.cache_misses << ",\n";
    json << "  \"cache_hit_rate_percent\": " << stats.cache_hit_rate_percent << ",\n";
    json << "  \"cache_size_bytes\": " << stats.cache_size_bytes << ",\n";
    json << "  \"cached_functions\": " << stats.cached_functions << ",\n";
    json << "  \"hot_functions\": " << stats.hot_functions << ",\n";
    json << "  \"avg_compilation_time_ms\": " << stats.avg_compilation_time_ms << ",\n";
    json << "  \"total_compilation_time_ms\": " << stats.total_compilation_time_ms << ",\n";
    json << "  \"speedup_factor\": " << stats.speedup_factor << "\n";
    json << "}";
    
    return json.str();
}

// ============================================================================
// الحصول على الإعدادات / Get Configuration
// ============================================================================

const JITConfig& JITEngine::getConfig() const {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    return pimpl_->config_;
}

// ============================================================================
// تحديث الإعدادات / Update Configuration
// ============================================================================

void JITEngine::updateConfig(const JITConfig& new_config) {
    std::lock_guard<std::mutex> lock(mutex_); // قفل للتزامن / Lock for thread safety
    pimpl_->config_ = new_config;
    
    // تطبيق الإعدادات الجديدة / Apply new configuration
    if (pimpl_->detector_) {
        pimpl_->detector_->setTieringEnabled(new_config.enable_tiered_compilation);
    }
}

// ============================================================================
// الدوال المساعدة الداخلية / Internal Helper Functions
// ============================================================================

// معالجة مسار ساخن / Handle Hot Path
void JITEngine::handleHotPath(const HotPathInfo& info) {
    // ملاحظة: يُستدعى من callback - احذر من deadlock / Note: Called from callback - beware of deadlock
    
    std::cout << "🔥 مسار ساخن مُكتشف: " << info.function_name << "\n";
    std::cout << "🔥 Hot path detected: " << info.function_name << "\n";
    std::cout << "   التنفيذات: " << info.execution_count << "\n";
    std::cout << "   Executions: " << info.execution_count << "\n";
    std::cout << "   المستوى الموصى به: Tier " << info.tier_level << "\n";
    std::cout << "   Recommended tier: Tier " << info.tier_level << "\n";
    
    // TODO: إعادة التجميع بمستوى أعلى / Recompile with higher tier
}

// اكتشاف معمارية النظام / Detect Native Target
std::string JITEngine::detectNativeTarget() const {
    // TODO: استخدام LLVM للاكتشاف التلقائي / Use LLVM for automatic detection
    
    #if defined(_WIN64) || defined(__x86_64__)
        return "x86_64-pc-windows-msvc";
    #elif defined(_WIN32)
        return "i386-pc-windows-msvc";
    #else
        return "unknown-unknown-unknown";
    #endif
}

// اكتشاف اسم المعالج / Detect CPU Name
std::string JITEngine::detectCPUName() const {
    // TODO: استخدام LLVM للاكتشاف / Use LLVM for detection
    return "generic"; // افتراضي / Default
}

// اكتشاف خصائص المعالج / Detect CPU Features
std::string JITEngine::detectCPUFeatures() const {
    // TODO: استخدام LLVM للاكتشاف / Use LLVM for detection
    return "+sse,+sse2,+sse3,+ssse3,+sse4.1,+sse4.2"; // افتراضي / Default
}

// تحديد مستوى التحسين / Determine Optimization Level
int JITEngine::determineOptimizationLevel(const std::string& function_name) const {
    // ملاحظة: يُفترض أن mutex مقفول من المُستدعي / Note: Assumes mutex locked by caller
    
    if (!pimpl_->detector_ || !pimpl_->config_.enable_tiered_compilation) {
        return pimpl_->config_.optimization_level; // استخدام المستوى الثابت / Use fixed level
    }
    
    // الحصول على المستوى الموصى به / Get recommended tier
    int tier = pimpl_->detector_->getRecommendedTier(function_name);
    
    return tier;
}

// حساب hash للكود / Calculate Code Hash
std::string JITEngine::calculateHash(const std::string& source_code) const {
    // استخدام FNV-1a hash (سريع جداً ومناسب للـ caching)
    // Using FNV-1a hash (very fast and suitable for caching)
    
    // ثوابت FNV-1a / FNV-1a constants
    const uint64_t FNV_64_PRIME = 0x100000001b3ULL;
    const uint64_t FNV_64_OFFSET = 0xcbf29ce484222325ULL;
    
    // حساب hash / Calculate hash
    uint64_t hash = FNV_64_OFFSET;
    for (char c : source_code) {
        hash ^= static_cast<uint8_t>(c); // XOR مع البايت / XOR with byte
        hash *= FNV_64_PRIME;            // ضرب في الثابت / Multiply by prime
    }
    
    // تحويل إلى hex string / Convert to hex string
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << hash;
    
    return oss.str();
}

// محاكاة التجميع (للتطوير) / Simulate Compilation (For Development)
void* JITEngine::simulateCompilation(
    const std::string& function_name,
    const std::string& source_code,
    int optimization_level
) const {
    // ملاحظة: هذه دالة مؤقتة للتطوير / Note: Temporary function for development
    // سيتم استبدالها بالتجميع الحقيقي عند توفر LLVM / Will be replaced with real compilation when LLVM available
    
    // محاكاة تأخير التجميع / Simulate compilation delay
    std::this_thread::sleep_for(std::chrono::microseconds(100 * (optimization_level + 1)));
    
    // إرجاع مؤشر وهمي (لن يُستخدم للتنفيذ الفعلي) / Return dummy pointer (won't be used for actual execution)
    return reinterpret_cast<void*>(static_cast<uintptr_t>(0xDEADBEEF));
}

} // namespace JIT
} // namespace Sad

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
