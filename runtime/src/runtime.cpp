/**
 * @file runtime.cpp
 * @brief تطبيق المكتبة الأساسية / Core Runtime Implementation
 * @brief Implementation of memory, Stage 1, array, and system functions
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/runtime.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
#endif

namespace Sad {
namespace Runtime {

// ========================================
// Memory Operations
// ========================================

VM::Value runtime_malloc(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] malloc requires size\n";
        return VM::Value::Null();
    }
    
    size_t size = static_cast<size_t>(vm->toInteger(args[0]));
    void* ptr = std::malloc(size);
    
    if (!ptr) {
        std::cerr << "[Error] malloc failed\n";
        return VM::Value::Null();
    }
    
    // (AR) أرجع العنوان كعدد صحيح
    // (EN) Return address as integer
    return VM::Value::Int(reinterpret_cast<int64_t>(ptr));
}

VM::Value runtime_free(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] free requires pointer\n";
        return VM::Value::Null();
    }
    
    int64_t addr = vm->toInteger(args[0]);
    void* ptr = reinterpret_cast<void*>(addr);
    
    std::free(ptr);
    return VM::Value::Null();
}

VM::Value runtime_realloc(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] realloc requires pointer and size\n";
        return VM::Value::Null();
    }
    
    int64_t addr = vm->toInteger(args[0]);
    size_t size = static_cast<size_t>(vm->toInteger(args[1]));
    
    void* oldPtr = reinterpret_cast<void*>(addr);
    void* newPtr = std::realloc(oldPtr, size);
    
    if (!newPtr) {
        std::cerr << "[Error] realloc failed\n";
        return VM::Value::Null();
    }
    
    return VM::Value::Int(reinterpret_cast<int64_t>(newPtr));
}

VM::Value runtime_memcpy(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 3) {
        std::cerr << "[Error] memcpy requires dest, src, and size\n";
        return VM::Value::Null();
    }
    
    int64_t destAddr = vm->toInteger(args[0]);
    int64_t srcAddr = vm->toInteger(args[1]);
    size_t size = static_cast<size_t>(vm->toInteger(args[2]));
    
    void* dest = reinterpret_cast<void*>(destAddr);
    const void* src = reinterpret_cast<const void*>(srcAddr);
    
    std::memcpy(dest, src, size);
    return VM::Value::Int(destAddr);
}

VM::Value runtime_memset(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 3) {
        std::cerr << "[Error] memset requires pointer, value, and size\n";
        return VM::Value::Null();
    }
    
    int64_t addr = vm->toInteger(args[0]);
    int value = static_cast<int>(vm->toInteger(args[1]));
    size_t size = static_cast<size_t>(vm->toInteger(args[2]));
    
    void* ptr = reinterpret_cast<void*>(addr);
    std::memset(ptr, value, size);
    
    return VM::Value::Int(addr);
}

// ========================================
// Stage 1 Compatibility Functions
// ========================================

VM::Value runtime_mmap(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] mmap requires size\n";
        return VM::Value::Null();
    }
    
    size_t size = static_cast<size_t>(vm->toInteger(args[0]));
    
#ifdef _WIN32
    // (AR) استخدم VirtualAlloc على Windows
    // (EN) Use VirtualAlloc on Windows
    void* ptr = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!ptr) {
        std::cerr << "[Error] mmap (VirtualAlloc) failed\n";
        return VM::Value::Null();
    }
#else
    // (AR) استخدم mmap على Unix
    // (EN) Use mmap on Unix
    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "[Error] mmap failed\n";
        return VM::Value::Null();
    }
#endif
    
    return VM::Value::Int(reinterpret_cast<int64_t>(ptr));
}

VM::Value runtime_munmap(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] munmap requires pointer and size\n";
        return VM::Value::Null();
    }
    
    int64_t addr = vm->toInteger(args[0]);
    size_t size = static_cast<size_t>(vm->toInteger(args[1]));
    
    void* ptr = reinterpret_cast<void*>(addr);
    
#ifdef _WIN32
    if (!VirtualFree(ptr, 0, MEM_RELEASE)) {
        std::cerr << "[Error] munmap (VirtualFree) failed\n";
        return VM::Value::Int(-1);
    }
#else
    if (munmap(ptr, size) != 0) {
        std::cerr << "[Error] munmap failed\n";
        return VM::Value::Int(-1);
    }
#endif
    
    return VM::Value::Int(0);
}

VM::Value runtime_io_read(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 3) {
        std::cerr << "[Error] io_read requires fd, buffer, and size\n";
        return VM::Value::Null();
    }
    
    int fd = static_cast<int>(vm->toInteger(args[0]));
    int64_t bufAddr = vm->toInteger(args[1]);
    size_t size = static_cast<size_t>(vm->toInteger(args[2]));
    
    void* buffer = reinterpret_cast<void*>(bufAddr);
    
#ifdef _WIN32
    HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
    DWORD bytesRead;
    if (!ReadFile(handle, buffer, static_cast<DWORD>(size), &bytesRead, NULL)) {
        return VM::Value::Int(-1);
    }
    return VM::Value::Int(static_cast<int64_t>(bytesRead));
#else
    ssize_t result = read(fd, buffer, size);
    return VM::Value::Int(static_cast<int64_t>(result));
#endif
}

VM::Value runtime_io_write(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 3) {
        std::cerr << "[Error] io_write requires fd, buffer, and size\n";
        return VM::Value::Null();
    }
    
    int fd = static_cast<int>(vm->toInteger(args[0]));
    int64_t bufAddr = vm->toInteger(args[1]);
    size_t size = static_cast<size_t>(vm->toInteger(args[2]));
    
    const void* buffer = reinterpret_cast<const void*>(bufAddr);
    
#ifdef _WIN32
    HANDLE handle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
    DWORD bytesWritten;
    if (!WriteFile(handle, buffer, static_cast<DWORD>(size), &bytesWritten, NULL)) {
        return VM::Value::Int(-1);
    }
    return VM::Value::Int(static_cast<int64_t>(bytesWritten));
#else
    ssize_t result = write(fd, buffer, size);
    return VM::Value::Int(static_cast<int64_t>(result));
#endif
}

VM::Value runtime_int_get(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] int_get requires address\n";
        return VM::Value::Null();
    }
    
    int64_t addr = vm->toInteger(args[0]);
    const int64_t* ptr = reinterpret_cast<const int64_t*>(addr);
    
    return VM::Value::Int(*ptr);
}

VM::Value runtime_int_set(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] int_set requires address and value\n";
        return VM::Value::Null();
    }
    
    int64_t addr = vm->toInteger(args[0]);
    int64_t value = vm->toInteger(args[1]);
    
    int64_t* ptr = reinterpret_cast<int64_t*>(addr);
    *ptr = value;
    
    return VM::Value::Null();
}

// ========================================
// Array Utilities
// ========================================

VM::Value runtime_map(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] map requires array and function\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isArray()) {
        std::cerr << "[Error] map first argument must be array\n";
        return VM::Value::Null();
    }
    
    VM::ArrayObject* arr = args[0].asArray();
    VM::Value func = args[1];
    
    // (AR) أنشئ مصفوفة جديدة للنتائج
    // (EN) Create new array for results
    VM::ArrayObject* result = static_cast<VM::ArrayObject*>(
        vm->allocateObject(VM::OBJ_ARRAY, sizeof(VM::ArrayObject))
    );
    result->elements = new std::vector<VM::Value>();
    result->elements->reserve(arr->elements->size());
    
    // (AR) طبّق الدالة على كل عنصر
    // (EN) Apply function to each element
    for (const auto& element : *arr->elements) {
        VM::Value callArgs[1] = { element };
        VM::Value mappedValue = vm->callValue(func, 1, callArgs);
        result->elements->push_back(mappedValue);
    }
    
    return VM::Value::Array(result);
}

VM::Value runtime_filter(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] filter requires array and predicate\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isArray()) {
        std::cerr << "[Error] filter first argument must be array\n";
        return VM::Value::Null();
    }
    
    VM::ArrayObject* arr = args[0].asArray();
    VM::Value predicate = args[1];
    
    VM::ArrayObject* result = static_cast<VM::ArrayObject*>(
        vm->allocateObject(VM::OBJ_ARRAY, sizeof(VM::ArrayObject))
    );
    result->elements = new std::vector<VM::Value>();
    
    for (const auto& element : *arr->elements) {
        VM::Value callArgs[1] = { element };
        VM::Value testResult = vm->callValue(predicate, 1, callArgs);
        
        if (vm->toBool(testResult)) {
            result->elements->push_back(element);
        }
    }
    
    return VM::Value::Array(result);
}

VM::Value runtime_reduce(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 3) {
        std::cerr << "[Error] reduce requires array, function, and initial value\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isArray()) {
        std::cerr << "[Error] reduce first argument must be array\n";
        return VM::Value::Null();
    }
    
    VM::ArrayObject* arr = args[0].asArray();
    VM::Value func = args[1];
    VM::Value accumulator = args[2];
    
    for (const auto& element : *arr->elements) {
        VM::Value callArgs[2] = { accumulator, element };
        accumulator = vm->callValue(func, 2, callArgs);
    }
    
    return accumulator;
}

VM::Value runtime_sort(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] sort requires array\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isArray()) {
        std::cerr << "[Error] sort argument must be array\n";
        return VM::Value::Null();
    }
    
    VM::ArrayObject* arr = args[0].asArray();
    
    // (AR) أنشئ نسخة من المصفوفة
    // (EN) Create copy of array
    VM::ArrayObject* result = static_cast<VM::ArrayObject*>(
        vm->allocateObject(VM::OBJ_ARRAY, sizeof(VM::ArrayObject))
    );
    result->elements = new std::vector<VM::Value>(*arr->elements);
    
    // (AR) رتّب المصفوفة
    // (EN) Sort array
    std::sort(result->elements->begin(), result->elements->end(), 
              [vm](const VM::Value& a, const VM::Value& b) {
        // (AR) قارن حسب النوع
        // (EN) Compare by type
        if (a.isInt() && b.isInt()) {
            return a.asInt() < b.asInt();
        }
        return vm->toFloat(a) < vm->toFloat(b);
    });
    
    return VM::Value::Array(result);
}

VM::Value runtime_reverse(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] reverse requires array\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isArray()) {
        std::cerr << "[Error] reverse argument must be array\n";
        return VM::Value::Null();
    }
    
    VM::ArrayObject* arr = args[0].asArray();
    
    VM::ArrayObject* result = static_cast<VM::ArrayObject*>(
        vm->allocateObject(VM::OBJ_ARRAY, sizeof(VM::ArrayObject))
    );
    result->elements = new std::vector<VM::Value>(*arr->elements);
    
    std::reverse(result->elements->begin(), result->elements->end());
    
    return VM::Value::Array(result);
}

VM::Value runtime_slice(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 2) {
        std::cerr << "[Error] slice requires array and start index\n";
        return VM::Value::Null();
    }
    
    if (!args[0].isArray()) {
        std::cerr << "[Error] slice first argument must be array\n";
        return VM::Value::Null();
    }
    
    VM::ArrayObject* arr = args[0].asArray();
    int64_t start = vm->toInteger(args[1]);
    int64_t end = (argc >= 3) ? vm->toInteger(args[2]) : arr->elements->size();
    
    // (AR) تطبيع الفهارس
    // (EN) Normalize indices
    if (start < 0) start += arr->elements->size();
    if (end < 0) end += arr->elements->size();
    
    start = std::max<int64_t>(0, std::min<int64_t>(start, arr->elements->size()));
    end = std::max<int64_t>(start, std::min<int64_t>(end, arr->elements->size()));
    
    VM::ArrayObject* result = static_cast<VM::ArrayObject*>(
        vm->allocateObject(VM::OBJ_ARRAY, sizeof(VM::ArrayObject))
    );
    result->elements = new std::vector<VM::Value>(
        arr->elements->begin() + start,
        arr->elements->begin() + end
    );
    
    return VM::Value::Array(result);
}

// ========================================
// System Functions
// ========================================

VM::Value runtime_exit(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    int code = (argc > 0) ? static_cast<int>(vm->toInteger(args[0])) : 0;
    std::exit(code);
    return VM::Value::Null(); // Never reached
}

VM::Value runtime_sleep(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    if (argc < 1) {
        std::cerr << "[Error] sleep requires duration in milliseconds\n";
        return VM::Value::Null();
    }
    
    int64_t ms = vm->toInteger(args[0]);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    
    return VM::Value::Null();
}

VM::Value runtime_time(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    
    return VM::Value::Int(seconds);
}

VM::Value runtime_clock(VM::VirtualMachine* vm, int argc, VM::Value* args) {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    return VM::Value::Int(ms);
}

// ========================================
// Registration Functions
// ========================================

void registerMemoryFunctions(VM::VirtualMachine* vm) {
    vm->registerNative("malloc", runtime_malloc);
    vm->registerNative("free", runtime_free);
    vm->registerNative("realloc", runtime_realloc);
    vm->registerNative("memcpy", runtime_memcpy);
    vm->registerNative("memset", runtime_memset);
}

void registerStage1Functions(VM::VirtualMachine* vm) {
    vm->registerNative("mmap", runtime_mmap);
    vm->registerNative("munmap", runtime_munmap);
    vm->registerNative("io_read", runtime_io_read);
    vm->registerNative("io_write", runtime_io_write);
    vm->registerNative("int_get", runtime_int_get);
    vm->registerNative("int_set", runtime_int_set);
}

void registerArrayFunctions(VM::VirtualMachine* vm) {
    vm->registerNative("map", runtime_map);
    vm->registerNative("filter", runtime_filter);
    vm->registerNative("reduce", runtime_reduce);
    vm->registerNative("sort", runtime_sort);
    vm->registerNative("reverse", runtime_reverse);
    vm->registerNative("slice", runtime_slice);
}

void registerSystemFunctions(VM::VirtualMachine* vm) {
    vm->registerNative("exit", runtime_exit);
    vm->registerNative("sleep", runtime_sleep);
    vm->registerNative("time", runtime_time);
    vm->registerNative("clock", runtime_clock);
}

void registerAllRuntimeFunctions(VM::VirtualMachine* vm) {
    registerIOFunctions(vm);
    registerMemoryFunctions(vm);
    registerStage1Functions(vm);
    registerMathFunctions(vm);
    registerStringFunctions(vm);
    registerArrayFunctions(vm);
    registerSystemFunctions(vm);
}

} // namespace Runtime
} // namespace Sad
