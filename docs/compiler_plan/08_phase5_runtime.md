# المرحلة 5: Runtime & ABI
# Phase 5: Runtime System & ABI

**المدة / Duration:** 4-6 أسابيع / weeks  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### (AR) العربية

تطوير Runtime system كامل:
- C ABI integration
- Foreign Function Interface (FFI)
- System calls wrapper
- Memory management & GC
- Thread support
- Exception handling

---

## 📝 المهام / Tasks

### 1. C ABI Integration (أسبوعان)

#### (AR) العربية

**C Calling Convention:**

```cpp
// runtime/abi/c_abi.h

# Sad → C function calls
extern "C" {
    # يمكن استدعاء دوال C من لغة ص
    int printf(const char* format, ...);
    void* malloc(size_t size);
    void free(void* ptr);
}

# C → Sad function calls
# يمكن استدعاء دوال لغة ص من C
extern "C" {
    int64_t sad_add(int64_t a, int64_t b);
    void sad_print(const char* msg);
}
```

**Type Marshalling:**
```cpp
// Sad Type → C Type mapping
رقم       → int64_t
نص        → const char*
منطقي    → bool (int)
مصفوفة<T> → struct { size_t len; T* data; }
مؤشر<T>   → T*
```

**مثال:**
```s
# في لغة ص
خارجي دالة printf(نص format, ...) -> رقم

دالة main()
    printf("Hello from Sad!\n")
نهاية

# يُترجم إلى:
extern "C" int printf(const char*, ...);

extern "C" void main() {
    printf("Hello from Sad!\n");
}
```

**المهام:**
- [ ] Calling convention implementation
- [ ] Type marshalling
- [ ] Variadic functions support
- [ ] Struct layout compatibility
- [ ] اختبارات C ABI (50+ tests)

---

### 2. Foreign Function Interface (أسبوعان)

#### (AR) العربية

**FFI API:**

```cpp
// runtime/ffi/ffi.h
class FFI {
public:
    # تحميل مكتبة ديناميكية
    دالة load_library(نص path) -> مؤشر<Library>
    
    # البحث عن دالة
    دالة lookup_function(مؤشر<Library> lib, نص name) -> مؤشر<فارغ>
    
    # استدعاء دالة
    دالة call_function(مؤشر<فارغ> func, مصفوفة<Value> args) -> Value
    
    # تسجيل callback
    دالة register_callback(نص name, مؤشر<Function> func) -> فارغ
نهاية
```

**استخدام FFI في لغة ص:**

```s
# تحميل مكتبة
مؤشر<Library> lib = ffi_load("libm.so")

# الحصول على دالة
مؤشر<دالة> sqrt_func = ffi_get_function(lib, "sqrt")

# استدعاء الدالة
رقم result = ffi_call(sqrt_func, [16.0])
اطبع(result)  # 4.0

# أو باستخدام خارجي:
خارجي دالة sqrt(رقم x) -> رقم من "libm.so"

دالة main()
    اطبع(sqrt(16.0))
نهاية
```

**libffi Integration:**
```cpp
// استخدام libffi للاستدعاءات الديناميكية
#include <ffi.h>

دالة ffi_call_dynamic(void* func_ptr, 
                      مصفوفة<Type> arg_types,
                      مصفوفة<void*> args,
                      Type return_type) -> void*
    ffi_cif cif
    ffi_type** arg_type_array = convert_types(arg_types)
    ffi_type* ret_type = convert_type(return_type)
    
    ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arg_types.size(),
                 ret_type, arg_type_array)
    
    void* result
    ffi_call(&cif, FFI_FN(func_ptr), &result, args.data())
    
    إرجاع result
نهاية
```

**المهام:**
- [ ] Dynamic library loading (dlopen/LoadLibrary)
- [ ] Symbol resolution
- [ ] Type conversion layer
- [ ] libffi integration
- [ ] Callback support
- [ ] اختبارات FFI (40+ tests)

---

### 3. System Calls Wrapper (أسبوع واحد)

#### (AR) العربية

**System Call Interface:**

```cpp
// runtime/syscall/syscall.h

# File I/O
دالة sys_open(نص path, رقم flags) -> رقم
دالة sys_close(رقم fd) -> رقم
دالة sys_read(رقم fd, مؤشر<بايت> buf, رقم count) -> رقم
دالة sys_write(رقم fd, مؤشر<بايت> buf, رقم count) -> رقم

# Memory
دالة sys_mmap(رقم addr, رقم length, رقم prot, رقم flags) -> مؤشر<فارغ>
دالة sys_munmap(مؤشر<فارغ> addr, رقم length) -> رقم
دالة sys_mprotect(مؤشر<فارغ> addr, رقم len, رقم prot) -> رقم

# Process
دالة sys_fork() -> رقم
دالة sys_exec(نص path, مصفوفة<نص> args) -> رقم
دالة sys_wait(رقم pid) -> رقم
دالة sys_exit(رقم code) -> فارغ

# I/O Ports (Stage 1)
دالة sys_io_read(رقم port) -> رقم
دالة sys_io_write(رقم port, رقم value) -> فارغ
```

**Platform-specific:**
```cpp
#ifdef _WIN32
    # Windows syscalls
    #include <windows.h>
#elif __linux__
    # Linux syscalls
    #include <unistd.h>
    #include <sys/syscall.h>
#elif __APPLE__
    # macOS syscalls
    #include <unistd.h>
#endif
```

**المهام:**
- [ ] Platform detection
- [ ] Linux syscalls
- [ ] Windows API wrapper
- [ ] macOS syscalls
- [ ] Stage 1 I/O ports (mock/real)
- [ ] اختبارات Syscall (30+ tests)

---

### 4. Memory Management & GC (أسبوعان)

#### (AR) العربية

**Memory Allocator:**

```cpp
// runtime/memory/allocator.h
class MemoryAllocator {
private:
    # Heap segments
    مصفوفة<Segment> segments
    
    # Free lists
    قاموس<رقم، مصفوفة<Block>> free_lists
    
public:
    دالة allocate(رقم size) -> مؤشر<فارغ>
    دالة deallocate(مؤشر<فارغ> ptr) -> فارغ
    دالة reallocate(مؤشر<فارغ> ptr, رقم new_size) -> مؤشر<فارغ>
    
    # Statistics
    دالة get_total_allocated() -> رقم
    دالة get_peak_usage() -> رقم
نهاية
```

**Garbage Collector (Mark & Sweep):**

```cpp
// runtime/memory/gc.h
class GarbageCollector {
private:
    # Object graph
    مصفوفة<مؤشر<Object>> objects
    
    # Roots
    مصفوفة<مؤشر<Object>> roots
    
public:
    # GC cycle
    دالة collect() -> فارغ
    
    # Mark phase
    دالة mark() -> فارغ
    دالة mark_object(مؤشر<Object> obj) -> فارغ
    
    # Sweep phase
    دالة sweep() -> فارغ
    
    # Object registration
    دالة register_object(مؤشر<Object> obj) -> فارغ
    دالة register_root(مؤشر<Object> obj) -> فارغ
    
    # Statistics
    دالة get_stats() -> GCStats
نهاية

class Object {
    منطقي marked
    رقم size
    مصفوفة<مؤشر<Object>> references
نهاية
```

**GC Strategies:**
1. **Reference Counting** (بسيط، سريع)
2. **Mark & Sweep** (كامل، بطيء)
3. **Generational GC** (متقدم، فعال)
4. **Incremental GC** (لا يوقف البرنامج)

**المهام:**
- [ ] Memory allocator
- [ ] Mark & Sweep GC
- [ ] GC tuning parameters
- [ ] Memory leak detection
- [ ] Performance profiling
- [ ] اختبارات GC (50+ tests)

---

### 5. Thread Support (أسبوع واحد)

#### (AR) العربية

**Threading API:**

```cpp
// runtime/thread/thread.h

# Thread management
دالة thread_create(مؤشر<دالة> func, مؤشر<فارغ> arg) -> مؤشر<Thread>
دالة thread_join(مؤشر<Thread> thread) -> رقم
دالة thread_detach(مؤشر<Thread> thread) -> فارغ

# Mutex
دالة mutex_create() -> مؤشر<Mutex>
دالة mutex_lock(مؤشر<Mutex> mutex) -> فارغ
دالة mutex_unlock(مؤشر<Mutex> mutex) -> فارغ
دالة mutex_destroy(مؤشر<Mutex> mutex) -> فارغ

# Condition variables
دالة cond_create() -> مؤشر<Condition>
دالة cond_wait(مؤشر<Condition> cond, مؤشر<Mutex> mutex) -> فارغ
دالة cond_signal(مؤشر<Condition> cond) -> فارغ
دالة cond_broadcast(مؤشر<Condition> cond) -> فارغ
```

**استخدام في لغة ص:**
```s
# مثال: producer-consumer
مؤشر<Mutex> mutex = mutex_create()
مؤشر<Condition> cond = cond_create()
مصفوفة<رقم> queue = []

دالة producer()
    لكل ع في مدى(10)
        mutex_lock(mutex)
        queue.push(ع)
        cond_signal(cond)
        mutex_unlock(mutex)
    نهاية
نهاية

دالة consumer()
    بينما (صحيح)
        mutex_lock(mutex)
        بينما (queue.empty())
            cond_wait(cond, mutex)
        نهاية
        رقم item = queue.pop()
        mutex_unlock(mutex)
        اطبع(item)
    نهاية
نهاية
```

**المهام:**
- [ ] Thread creation (pthreads/Windows threads)
- [ ] Mutex implementation
- [ ] Condition variables
- [ ] Thread-local storage
- [ ] اختبارات Threading (30+ tests)

---

## 📊 معايير النجاح / Success Criteria

✅ **المرحلة مكتملة عندما:**
- [ ] C ABI يعمل 100%
- [ ] FFI يستدعي مكتبات C
- [ ] Syscalls تعمل على جميع المنصات
- [ ] GC لا يسرّب ذاكرة
- [ ] Thread support stable
- [ ] Test coverage > 85%

---

**السابق / Previous:** [المرحلة 4: LLVM Backend](07_phase4_llvm.md)  
**التالي / Next:** [المرحلة 6: التحسينات](09_phase6_optimizations.md)
