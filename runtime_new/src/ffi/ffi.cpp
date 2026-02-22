/*
 * بسم الله الرحمن الرحيم
 * ================================
 * تطبيق نظام FFI / FFI System Implementation
 * ================================
 * 
 * @file runtime/ffi/ffi.cpp
 * @author SadLang Compiler Team
 * @date December 2025
 */

#include "ffi/ffi.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cstdarg>
#include <cstring>

// Platform-specific includes / التضمينات الخاصة بالمنصة
#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
    #include <sys/mman.h>  // For mmap/munmap
    #include <unistd.h>    // For sysconf
#endif

namespace Sad {
namespace FFI {

// ============================================================================
// Library Implementation / تطبيق المكتبة
// ============================================================================

Library::Library(const std::string& path)
    : path_(path)
    , handle_(nullptr)
    , loaded_(false)
{
}

Library::~Library() {
    close();
}

bool Library::load() {
    if (loaded_) {
        return true; // Already loaded / محملة مسبقاً
    }
    
#ifdef _WIN32
    // Windows implementation / تطبيق Windows
    handle_ = LoadLibraryA(path_.c_str());
    if (!handle_) {
        DWORD error = GetLastError();
        std::cerr << "Failed to load library: " << path_ 
                  << " (Error: " << error << ")\n";
        return false;
    }
#else
    // Unix/Linux/macOS implementation / تطبيق Unix
    handle_ = dlopen(path_.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle_) {
        const char* error = dlerror();
        std::cerr << "Failed to load library: " << path_
                  << " (" << (error ? error : "Unknown error") << ")\n";
        return false;
    }
#endif
    
    loaded_ = true;
    return true;
}

void Library::close() {
    if (!loaded_ || !handle_) {
        return;
    }
    
#ifdef _WIN32
    FreeLibrary(static_cast<HMODULE>(handle_));
#else
    dlclose(handle_);
#endif
    
    handle_ = nullptr;
    loaded_ = false;
    functions_.clear();
}

void* Library::get_symbol(const std::string& name) const {
    if (!loaded_ || !handle_) {
        return nullptr;
    }
    
#ifdef _WIN32
    return reinterpret_cast<void*>(
        GetProcAddress(static_cast<HMODULE>(handle_), name.c_str())
    );
#else
    // Clear any existing error / مسح أي خطأ موجود
    dlerror();
    
    void* symbol = dlsym(handle_, name.c_str());
    
    // Check for error / التحقق من الخطأ
    const char* error = dlerror();
    if (error) {
        std::cerr << "Failed to get symbol '" << name << "': " << error << "\n";
        return nullptr;
    }
    
    return symbol;
#endif
}

std::shared_ptr<Function> Library::get_function(
    const std::string& name,
    const ABI::FunctionSignature& signature)
{
    // التحقق من الذاكرة المؤقتة / Check cache
    auto it = functions_.find(name);
    if (it != functions_.end()) {
        return it->second;
    }
    
    // البحث عن الرمز / Lookup symbol
    void* symbol = get_symbol(name);
    if (!symbol) {
        return nullptr;
    }
    
    // إنشاء غلاف الدالة / Create function wrapper
    auto func = std::make_shared<Function>(
        symbol,
        signature,
        shared_from_this()
    );
    
    // تخزين في الذاكرة المؤقتة / Cache it
    functions_[name] = func;
    
    return func;
}

// ============================================================================
// Function Implementation / تطبيق الدالة
// ============================================================================

Function::Function(
    void* ptr,
    const ABI::FunctionSignature& signature,
    std::shared_ptr<Library> lib)
    : ptr_(ptr)
    , signature_(signature)
    , library_(lib)
{
    abi_ = std::make_unique<ABI::CABI>();
    marshaller_ = std::make_unique<ABI::TypeMarshaller>();
}

void* Function::call(const std::vector<void*>& args) {
    if (!ptr_) {
        std::cerr << "Error: Null function pointer\n";
        return nullptr;
    }
    
    // تحويل المعاملات من Sad إلى C / Convert args from Sad to C
    std::vector<void*> c_args;
    c_args.reserve(args.size());
    
    for (size_t i = 0; i < args.size() && i < signature_.param_types.size(); ++i) {
        void* c_arg = marshaller_->sad_to_c(args[i], signature_.param_types[i]);
        c_args.push_back(c_arg);
    }
    
    // إعداد الاستدعاء / Prepare call
    ABI::CABI::CallInfo call_info = abi_->prepare_c_call(
        ptr_,
        signature_,
        args
    );
    
    // تنفيذ الاستدعاء / Execute call
    void* c_result = abi_->execute_call(call_info);
    
    // تحويل النتيجة من C إلى Sad / Convert result from C to Sad
    void* sad_result = nullptr;
    if (signature_.return_type.sad_type != ABI::SadType::Void) {
        sad_result = marshaller_->c_to_sad(c_result, signature_.return_type);
    }
    
    // تنظيف / Cleanup
    abi_->cleanup_call(call_info);
    
    return sad_result;
}

void* Function::call_variadic(
    const std::vector<void*>& args,
    const std::vector<ABI::TypeInfo>& variadic_types)
{
    if (!ptr_) {
        std::cerr << "Error: Null function pointer\n";
        return nullptr;
    }
    
    if (!signature_.is_variadic) {
        std::cerr << "Error: Function is not variadic\n";
        return nullptr;
    }
    
    // إعداد الاستدعاء المتغير / Prepare variadic call
    ABI::CABI::CallInfo call_info = abi_->prepare_variadic_call(
        ptr_,
        signature_,
        args,
        variadic_types
    );
    
    // تنفيذ / Execute
    void* c_result = abi_->execute_call(call_info);
    
    // تحويل النتيجة / Convert result
    void* sad_result = nullptr;
    if (signature_.return_type.sad_type != ABI::SadType::Void) {
        sad_result = marshaller_->c_to_sad(c_result, signature_.return_type);
    }
    
    // تنظيف / Cleanup
    abi_->cleanup_call(call_info);
    
    return sad_result;
}

// ============================================================================
// Callback Implementation / تطبيق Callback
// ============================================================================

Callback::Callback(
    const ABI::FunctionSignature& signature,
    SadFunction sad_func)
    : signature_(signature)
    , sad_function_(sad_func)
    , c_wrapper_(nullptr)
    , wrapper_memory_(nullptr)
    , wrapper_size_(0)
{
    c_wrapper_ = create_c_wrapper();
}

Callback::~Callback() {
    destroy_c_wrapper();
}

void* Callback::create_c_wrapper() {
    // في التطبيق الحقيقي، نحتاج إنشاء wrapper ديناميكي
    // In real implementation, we need to create dynamic wrapper
    // يستخدم assembly code أو libffi
    // Uses assembly code or libffi
    
    // Full wrapper generation implementation / تنفيذ كامل لتوليد الغلاف
    
    // استراتيجية التنفيذ / Implementation strategy:
    // 1. نخصص صفحة قابلة للتنفيذ / Allocate executable page
    // 2. نكتب trampoline code / Write trampoline code
    // 3. الـ trampoline يستدعي sad_function_ / Trampoline calls sad_function_
    
#ifdef _WIN32
    // Windows: استخدام VirtualAlloc مع PAGE_EXECUTE_READWRITE
    // Allocate executable memory
    SIZE_T size = 1024; // كافي لـ trampoline / Enough for trampoline
    void* exec_mem = VirtualAlloc(
        nullptr,
        size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_EXECUTE_READWRITE
    );
    
    if (!exec_mem) {
        std::cerr << "Failed to allocate executable memory\n";
        return nullptr;
    }
    
    // كتابة trampoline code / Write trampoline code
    // هذا مثال مبسط - في التطبيق الحقيقي نحتاج assembly كامل
    // This is simplified - real implementation needs full assembly
    
    unsigned char* code = static_cast<unsigned char*>(exec_mem);
    size_t offset = 0;
    
#ifdef _M_X64
    // x64 trampoline:
    // mov rax, <address of trampoline_helper>
    // jmp rax
    
    // mov rax, immediate (48 B8 + 8 bytes address)
    code[offset++] = 0x48;
    code[offset++] = 0xB8;
    
    // عنوان دالة المساعد / Helper function address
    uintptr_t helper_addr = reinterpret_cast<uintptr_t>(&trampoline_helper);
    std::memcpy(&code[offset], &helper_addr, sizeof(uintptr_t));
    offset += sizeof(uintptr_t);
    
    // jmp rax (FF E0)
    code[offset++] = 0xFF;
    code[offset++] = 0xE0;
    
#else // _M_IX86
    // x86 trampoline:
    // push <this callback pointer>
    // jmp <trampoline_helper>
    
    // push immediate (68 + 4 bytes)
    code[offset++] = 0x68;
    uintptr_t this_ptr = reinterpret_cast<uintptr_t>(this);
    std::memcpy(&code[offset], &this_ptr, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    // jmp relative (E9 + 4 bytes offset)
    code[offset++] = 0xE9;
    uintptr_t helper_addr = reinterpret_cast<uintptr_t>(&trampoline_helper);
    int32_t rel_offset = helper_addr - (reinterpret_cast<uintptr_t>(&code[offset]) + 4);
    std::memcpy(&code[offset], &rel_offset, sizeof(int32_t));
    offset += sizeof(int32_t);
#endif
    
    // حفظ المؤشر للتنظيف لاحقاً / Save pointer for cleanup
    wrapper_memory_ = exec_mem;
    wrapper_size_ = size;
    
    return exec_mem;
    
#else // Unix/Linux/macOS
    // Unix: استخدام mmap مع PROT_EXEC
    size_t size = 1024;
    void* exec_mem = mmap(
        nullptr,
        size,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );
    
    if (exec_mem == MAP_FAILED) {
        std::cerr << "Failed to allocate executable memory\n";
        return nullptr;
    }
    
    unsigned char* code = static_cast<unsigned char*>(exec_mem);
    size_t offset = 0;
    
#ifdef __x86_64__
    // x64 Unix trampoline (similar to Windows but different calling convention)
    // mov rax, <trampoline_helper>
    // jmp rax
    
    code[offset++] = 0x48;
    code[offset++] = 0xB8;
    
    uintptr_t helper_addr = reinterpret_cast<uintptr_t>(&trampoline_helper);
    std::memcpy(&code[offset], &helper_addr, sizeof(uintptr_t));
    offset += sizeof(uintptr_t);
    
    code[offset++] = 0xFF;
    code[offset++] = 0xE0;
    
#elif defined(__i386__)
    // x86 Unix trampoline
    code[offset++] = 0x68; // push immediate
    uintptr_t this_ptr = reinterpret_cast<uintptr_t>(this);
    std::memcpy(&code[offset], &this_ptr, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    
    code[offset++] = 0xE9; // jmp relative
    uintptr_t helper_addr = reinterpret_cast<uintptr_t>(&trampoline_helper);
    int32_t rel_offset = helper_addr - (reinterpret_cast<uintptr_t>(&code[offset]) + 4);
    std::memcpy(&code[offset], &rel_offset, sizeof(int32_t));
    offset += sizeof(int32_t);
#endif
    
    wrapper_memory_ = exec_mem;
    wrapper_size_ = size;
    
    return exec_mem;
#endif
}

void Callback::destroy_c_wrapper() {
    if (c_wrapper_) {
        // تحرير الذاكرة / Free memory
        // Cleanup implementation / تنفيذ التنظيف
        
#ifdef _WIN32
        // Windows: استخدام VirtualFree
        if (wrapper_memory_) {
            BOOL result = VirtualFree(
                wrapper_memory_,
                0,
                MEM_RELEASE
            );
            
            if (!result) {
                std::cerr << "Warning: Failed to free executable memory\n";
            }
            
            wrapper_memory_ = nullptr;
            wrapper_size_ = 0;
        }
#else
        // Unix/Linux/macOS: استخدام munmap
        if (wrapper_memory_ && wrapper_size_ > 0) {
            int result = munmap(wrapper_memory_, wrapper_size_);
            
            if (result != 0) {
                std::cerr << "Warning: Failed to unmap executable memory\n";
            }
            
            wrapper_memory_ = nullptr;
            wrapper_size_ = 0;
        }
#endif
        
        c_wrapper_ = nullptr;
    }
}

void* Callback::trampoline_helper(void* callback_ptr, ...) {
    // دالة مساعدة لاستدعاء callback من C
    // Helper function to call callback from C
    
    if (!callback_ptr) {
        return nullptr;
    }
    
    Callback* callback = static_cast<Callback*>(callback_ptr);
    
    // جمع المعاملات من va_list / Collect args from va_list
    std::vector<void*> args;
    
    // ملاحظة: هذا تطبيق مبسط / Note: This is simplified implementation
    // في التطبيق الحقيقي، نحتاج تحليل التوقيع لجمع المعاملات بشكل صحيح
    // In real implementation, we need to parse signature to collect args correctly
    
    va_list va_args;
    va_start(va_args, callback_ptr);
    
    // جمع المعاملات حسب التوقيع / Collect args based on signature
    for (const auto& param_type : callback->signature_.param_types) {
        void* arg = nullptr;
        
        // تحويل حسب النوع / Convert based on type
        switch (param_type.sad_type) {
            case ABI::SadType::Int32:
            case ABI::SadType::UInt32:
                arg = reinterpret_cast<void*>(static_cast<intptr_t>(va_arg(va_args, int)));
                break;
                
            case ABI::SadType::Int64:
            case ABI::SadType::UInt64:
                arg = reinterpret_cast<void*>(va_arg(va_args, int64_t));
                break;
                
            case ABI::SadType::Float32:
                // float promoted to double in varargs
                arg = reinterpret_cast<void*>(static_cast<intptr_t>(va_arg(va_args, double)));
                break;
                
            case ABI::SadType::Float64: {
                double dval = va_arg(va_args, double);
                // (AR) لا يمكن تحويل double إلى void* مباشرة — نستخدم memcpy
                // (EN) Can't reinterpret_cast double to void* — use memcpy
                void* tmp = nullptr;
                static_assert(sizeof(double) <= sizeof(void*), "double must fit in void*");
                std::memcpy(&tmp, &dval, sizeof(double));
                arg = tmp;
                break;
            }
                
            case ABI::SadType::Pointer:
            case ABI::SadType::String:
                arg = va_arg(va_args, void*);
                break;
                
            default:
                arg = va_arg(va_args, void*);
                break;
        }
        
        args.push_back(arg);
    }
    
    va_end(va_args);
    
    // استدعاء الدالة الأصلية / Call original function
    void* result = callback->sad_function_(args);
    
    return result;
}

// ============================================================================
// FFIManager Implementation / تطبيق FFIManager
// ============================================================================

FFIManager::FFIManager()
    : debug_mode_(false)
{
    abi_ = std::make_unique<ABI::CABI>();
    marshaller_ = std::make_unique<ABI::TypeMarshaller>();
    stats_ = Stats();
    
    initialize_default_paths();
    
    log_debug("FFIManager initialized");
}

FFIManager::~FFIManager() {
    close_all_libraries();
    
    if (debug_mode_) {
        std::cout << "FFIManager Statistics:\n"
                  << "  Libraries loaded: " << stats_.libraries_loaded << "\n"
                  << "  Functions loaded: " << stats_.functions_loaded << "\n"
                  << "  Callbacks registered: " << stats_.callbacks_registered << "\n"
                  << "  Total calls: " << stats_.total_calls << "\n"
                  << "  Failed calls: " << stats_.failed_calls << "\n";
    }
}

std::shared_ptr<Library> FFIManager::load_library(const std::string& path) {
    // التحقق من المكتبة المحملة / Check if already loaded
    auto it = libraries_.find(path);
    if (it != libraries_.end()) {
        return it->second;
    }
    
    // البحث عن المكتبة / Find library
    std::string full_path = find_library(path);
    if (full_path.empty()) {
        set_error(ErrorCode::LibraryNotFound, 
                 "Library not found: " + path,
                 path);
        return nullptr;
    }
    
    // إنشاء وتحميل المكتبة / Create and load library
    auto library = std::make_shared<Library>(full_path);
    if (!library->load()) {
        set_error(ErrorCode::LibraryLoadFailed,
                 "Failed to load library: " + full_path,
                 full_path);
        return nullptr;
    }
    
    // تخزين المكتبة / Store library
    libraries_[path] = library;
    libraries_[full_path] = library; // Store both paths
    
    stats_.libraries_loaded++;
    
    log_debug("Loaded library: " + full_path);
    
    return library;
}

void FFIManager::close_library(const std::string& path) {
    auto it = libraries_.find(path);
    if (it != libraries_.end()) {
        it->second->close();
        libraries_.erase(it);
        
        log_debug("Closed library: " + path);
    }
}

std::shared_ptr<Library> FFIManager::get_library(const std::string& path) const {
    auto it = libraries_.find(path);
    return (it != libraries_.end()) ? it->second : nullptr;
}

void FFIManager::close_all_libraries() {
    for (auto& pair : libraries_) {
        pair.second->close();
    }
    libraries_.clear();
    
    log_debug("Closed all libraries");
}

std::shared_ptr<Function> FFIManager::load_function(
    const std::string& library_path,
    const std::string& function_name,
    const ABI::FunctionSignature& signature)
{
    // تحميل المكتبة / Load library
    auto library = load_library(library_path);
    if (!library) {
        return nullptr;
    }
    
    // الحصول على الدالة / Get function
    auto function = library->get_function(function_name, signature);
    if (!function) {
        set_error(ErrorCode::SymbolNotFound,
                 "Symbol not found: " + function_name,
                 library_path,
                 function_name);
        return nullptr;
    }
    
    stats_.functions_loaded++;
    
    log_debug("Loaded function: " + function_name + " from " + library_path);
    
    return function;
}

std::shared_ptr<Function> FFIManager::get_function(
    const std::string& library_path,
    const std::string& function_name) const
{
    auto library = get_library(library_path);
    if (!library) {
        return nullptr;
    }
    
    // البحث في الذاكرة المؤقتة / Search in cache
    // Function cache lookup implementation / تنفيذ البحث في ذاكرة الدوال المؤقتة
    
    // بناء مفتاح الذاكرة المؤقتة / Build cache key
    std::string cache_key = library_path + "::" + function_name;
    
    // البحث في الذاكرة المؤقتة العامة / Search in global function cache
    // نستخدم ذاكرة المكتبة المؤقتة / Use library's cache
    auto it = library->functions_.find(function_name);
    if (it != library->functions_.end()) {
        // وجدناها في الذاكرة المؤقتة / Found in cache
        return it->second;
    }
    
    // غير موجودة في الذاكرة المؤقتة / Not in cache
    // المستدعي يجب أن يستخدم load_function() لتحميلها
    // Caller should use load_function() to load it
    return nullptr;
}

std::shared_ptr<Callback> FFIManager::register_callback(
    const std::string& name,
    const ABI::FunctionSignature& signature,
    Callback::SadFunction sad_func)
{
    // إنشاء callback / Create callback
    auto callback = std::make_shared<Callback>(signature, sad_func);
    
    // تسجيل / Register
    callbacks_[name] = callback;
    
    stats_.callbacks_registered++;
    
    log_debug("Registered callback: " + name);
    
    return callback;
}

void FFIManager::unregister_callback(const std::string& name) {
    auto it = callbacks_.find(name);
    if (it != callbacks_.end()) {
        callbacks_.erase(it);
        log_debug("Unregistered callback: " + name);
    }
}

std::shared_ptr<Callback> FFIManager::get_callback(const std::string& name) const {
    auto it = callbacks_.find(name);
    return (it != callbacks_.end()) ? it->second : nullptr;
}

void* FFIManager::call_external(
    const std::string& library_path,
    const std::string& function_name,
    const ABI::FunctionSignature& signature,
    const std::vector<void*>& args)
{
    // تحميل الدالة / Load function
    auto function = load_function(library_path, function_name, signature);
    if (!function) {
        stats_.failed_calls++;
        return nullptr;
    }
    
    // استدعاء / Call
    stats_.total_calls++;
    
    void* result = function->call(args);
    if (!result && signature.return_type.sad_type != ABI::SadType::Void) {
        set_error(ErrorCode::CallFailed,
                 "Function call failed: " + function_name,
                 library_path,
                 function_name);
        stats_.failed_calls++;
        return nullptr;
    }
    
    return result;
}

void* FFIManager::call_symbol(
    void* symbol_ptr,
    const ABI::FunctionSignature& signature,
    const std::vector<void*>& args)
{
    if (!symbol_ptr) {
        set_error(ErrorCode::SymbolNotFound, "Null symbol pointer");
        stats_.failed_calls++;
        return nullptr;
    }
    
    // إنشاء function wrapper / Create function wrapper
    Function func(symbol_ptr, signature);
    
    // استدعاء / Call
    stats_.total_calls++;
    
    void* result = func.call(args);
    
    return result;
}

void FFIManager::add_search_path(const std::string& path) {
    // التحقق من عدم وجوده مسبقاً / Check if not already present
    auto it = std::find(search_paths_.begin(), search_paths_.end(), path);
    if (it == search_paths_.end()) {
        search_paths_.push_back(path);
        log_debug("Added search path: " + path);
    }
}

void FFIManager::remove_search_path(const std::string& path) {
    auto it = std::find(search_paths_.begin(), search_paths_.end(), path);
    if (it != search_paths_.end()) {
        search_paths_.erase(it);
        log_debug("Removed search path: " + path);
    }
}

std::string FFIManager::find_library(const std::string& name) const {
    namespace fs = std::filesystem;
    
    // إذا كان مساراً كاملاً / If absolute path
    if (fs::path(name).is_absolute() && fs::exists(name)) {
        return name;
    }
    
    // البحث في المسارات / Search in paths
    for (const auto& search_path : search_paths_) {
        fs::path full_path = fs::path(search_path) / name;
        if (fs::exists(full_path)) {
            return full_path.string();
        }
    }
    
    // البحث في المسار الحالي / Search in current directory
    if (fs::exists(name)) {
        return fs::absolute(name).string();
    }
    
    // لم يتم العثور / Not found
    return "";
}

void FFIManager::reset_stats() {
    stats_ = Stats();
    log_debug("Statistics reset");
}

void FFIManager::set_error(
    ErrorCode code,
    const std::string& message,
    const std::string& library_path,
    const std::string& symbol_name)
{
    last_error_.code = code;
    last_error_.message = message;
    last_error_.library_path = library_path;
    last_error_.symbol_name = symbol_name;
    
    if (debug_mode_) {
        std::cerr << "FFI Error [" << static_cast<int>(code) << "]: "
                  << message << "\n";
        if (!library_path.empty()) {
            std::cerr << "  Library: " << library_path << "\n";
        }
        if (!symbol_name.empty()) {
            std::cerr << "  Symbol: " << symbol_name << "\n";
        }
    }
}

void FFIManager::log_debug(const std::string& message) {
    if (debug_mode_) {
        std::cout << "[FFI] " << message << "\n";
    }
}

void FFIManager::initialize_default_paths() {
    // إضافة مسارات البحث الافتراضية / Add default search paths
    
#ifdef _WIN32
    // Windows paths
    add_search_path("C:\\Windows\\System32");
    add_search_path(".");
#else
    // Unix/Linux/macOS paths
    add_search_path("/usr/lib");
    add_search_path("/usr/local/lib");
    add_search_path("/lib");
    add_search_path(".");
    
    #ifdef __APPLE__
        add_search_path("/usr/lib/system");
    #endif
#endif
}

// ============================================================================
// Convenience Functions Implementation / تطبيق دوال الراحة
// ============================================================================

void* ffi_call_once(
    const std::string& library_path,
    const std::string& function_name,
    const ABI::FunctionSignature& signature,
    const std::vector<void*>& args)
{
    static FFIManager manager;
    return manager.call_external(library_path, function_name, signature, args);
}

std::shared_ptr<Library> load_std_library(const std::string& name) {
    static FFIManager manager;
    
    std::string lib_name = name;
    
#ifdef _WIN32
    // Windows: name.dll
    if (lib_name.find(".dll") == std::string::npos) {
        lib_name += ".dll";
    }
#elif defined(__APPLE__)
    // macOS: libname.dylib
    if (lib_name.find(".dylib") == std::string::npos) {
        if (lib_name.find("lib") != 0) {
            lib_name = "lib" + lib_name;
        }
        lib_name += ".dylib";
    }
#else
    // Linux: libname.so
    if (lib_name.find(".so") == std::string::npos) {
        if (lib_name.find("lib") != 0) {
            lib_name = "lib" + lib_name;
        }
        lib_name += ".so";
    }
#endif
    
    return manager.load_library(lib_name);
}

ABI::FunctionSignature create_signature(
    const std::string& name,
    ABI::SadType return_type,
    const std::vector<ABI::SadType>& param_types,
    bool is_variadic)
{
    ABI::CABI abi;
    
    ABI::FunctionSignature sig;
    sig.name = name;
    sig.is_variadic = is_variadic;
    sig.is_extern = true;
    sig.convention = abi.get_default_convention();
    sig.return_type = abi.get_type_info(return_type);
    
    for (auto param_type : param_types) {
        sig.param_types.push_back(abi.get_type_info(param_type));
    }
    
    return sig;
}

} // namespace FFI
} // namespace Sad
