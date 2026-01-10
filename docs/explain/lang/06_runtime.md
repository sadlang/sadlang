# بيئة التشغيل (Runtime)

## ما هي بيئة التشغيل؟

بيئة التشغيل (Runtime) هي مجموعة البرامج والمكتبات التي تعمل خلف الكواليس أثناء تنفيذ برامج لغة ص. تشمل كل ما يحتاجه البرنامج ليعمل بشكل صحيح: إدارة الذاكرة، جامع القمامة، دعم الخيوط المتعددة، التعامل مع الاستثناءات، والربط مع الدوال الأصلية. بينما المترجم ينتج كود الآلة، فإن بيئة التشغيل توفر البنية التحتية التي يعتمد عليها هذا الكود.

يمكن تشبيه بيئة التشغيل بنظام التشغيل المصغر الخاص بلغة ص. تماماً كما يوفر نظام التشغيل خدمات للبرامج، توفر بيئة التشغيل خدمات لكود لغة ص. هذه الخدمات تشمل تخصيص وتحرير الذاكرة تلقائياً، إدارة الكائنات ودورة حياتها، التعامل مع الأخطاء والاستثناءات، والتفاعل مع مكتبات النظام.

## بنية بيئة التشغيل

تتواجد بيئة التشغيل في المجلد `runtime_new/` وتتكون من:

```
runtime_new/
├── include/           # ملفات الرأس
├── src/
│   ├── memory/        # إدارة الذاكرة وجامع القمامة
│   │   ├── allocator.cpp
│   │   └── gc.cpp
│   ├── vm/            # الآلة الافتراضية
│   │   ├── vm.cpp
│   │   ├── vm_gc.cpp
│   │   ├── vm_helpers.cpp
│   │   └── vm_opcodes.cpp
│   ├── thread/        # دعم الخيوط المتعددة
│   ├── exception/     # معالجة الاستثناءات
│   └── ffi/           # الربط مع اللغات الأخرى
```

## إدارة الذاكرة (Memory Management)

### المُخصِّص (Allocator)

المُخصِّص هو المكون المسؤول عن حجز وتحرير الذاكرة. بدلاً من استخدام malloc/free مباشرة، توفر لغة ص مُخصِّصاً خاصاً مُحسَّناً لأنماط استخدام اللغة:

```cpp
// من allocator.cpp
class MemoryAllocator {
public:
    static MemoryAllocator& instance();
    
    bool initialize();
    void shutdown();
    
    // تخصيص ذاكرة
    void* allocate(size_t size);
    
    // تحرير ذاكرة
    void deallocate(void* ptr);
    
    // إعادة تخصيص
    void* reallocate(void* ptr, size_t newSize);
    
private:
    // تجمعات لأحجام مختلفة
    Pool small_pool_;   // للكائنات الصغيرة
    Pool medium_pool_;  // للكائنات المتوسطة
    // الكائنات الكبيرة تُخصَّص مباشرة
};
```

### تجمعات الذاكرة (Memory Pools)

للأداء الأمثل، يستخدم المُخصِّص تجمعات ذاكرة مقسمة حسب الحجم:

- **التجمع الصغير (< 64 بايت):** للأرقام والمؤشرات
- **التجمع المتوسط (64-1024 بايت):** للنصوص القصيرة والكائنات الصغيرة
- **التخصيص الكبير (> 1024 بايت):** للمصفوفات الكبيرة والنصوص الطويلة

هذا النهج يقلل من تجزئة الذاكرة ويسرع عمليات التخصيص والتحرير.

## جامع القمامة (Garbage Collector)

جامع القمامة هو القلب النابض لإدارة الذاكرة في لغة ص. يتتبع جميع الكائنات في الذاكرة ويحرر تلقائياً الكائنات التي لم تعد مستخدمة.

### خوارزمية Mark-and-Sweep

جامع القمامة يستخدم خوارزمية Mark-and-Sweep على ثلاث مراحل:

```cpp
// من gc.cpp
void GarbageCollector::collect() {
    if (collecting_ || !enabled_) return;
    
    collecting_ = true;
    uint64_t start_time = get_microseconds();
    
    // المرحلة 1: التعليم (Mark)
    markPhase();
    
    // المرحلة 2: الكنس (Sweep)
    sweepPhase();
    
    // تحديث الإحصائيات
    stats_.total_collections++;
    stats_.last_collection_time = get_microseconds() - start_time;
    
    collecting_ = false;
}
```

### مرحلة التعليم (Mark Phase)

تبدأ من الجذور (المتغيرات العامة، المكدس) وتتبع جميع المراجع:

```cpp
void GarbageCollector::markPhase() {
    // تعليم الجذور
    for (GCObject* root : roots_) {
        markObject(root);
    }
    
    // معالجة القائمة الرمادية
    while (!gray_list_.empty()) {
        GCObject* obj = gray_list_.back();
        gray_list_.pop_back();
        
        // تعليم الكائنات المُشار إليها
        if (obj->trace_fn) {
            obj->trace_fn();
        }
        
        obj->state = GCObjectState::Black;
    }
}
```

### مرحلة الكنس (Sweep Phase)

تمر على جميع الكائنات وتحرر غير المُعلَّمة:

```cpp
void GarbageCollector::sweepPhase() {
    GCObject** current = &objects_list_;
    
    while (*current) {
        if ((*current)->state == GCObjectState::White) {
            // الكائن غير مُشار إليه - حرره
            GCObject* unreached = *current;
            *current = unreached->next;
            
            // استدعاء المُدمِّر
            if (unreached->finalize_fn) {
                unreached->finalize_fn();
            }
            
            freeObject(unreached);
            stats_.live_objects--;
        } else {
            // إعادة تعيين اللون للدورة التالية
            (*current)->state = GCObjectState::White;
            current = &(*current)->next;
        }
    }
}
```

### إعدادات جامع القمامة

```cpp
struct GCConfig {
    size_t gc_threshold = 1024 * 1024;  // 1 ميجابايت
    size_t gc_growth_factor = 2;         // مضاعفة العتبة بعد كل جمع
    bool gc_stress_mode = false;         // جمع بعد كل تخصيص (للاختبار)
    bool gc_log_enabled = false;         // تسجيل نشاط GC
    
    static GCConfig default_config() {
        return GCConfig();
    }
};
```

### تثبيت الكائنات (Pinning)

بعض الكائنات لا يجب تحريكها (مثل تلك المُمرَّرة لكود أصلي):

```cpp
void GarbageCollector::pin(GCObject* obj) {
    obj->pinned = true;
}

void GarbageCollector::unpin(GCObject* obj) {
    obj->pinned = false;
}
```

## تخصيص الكائنات

عند إنشاء كائن جديد:

```cpp
GCObject* GarbageCollector::allocate(GCObjectType type, size_t size) {
    // التحقق من الحاجة للجمع
    if (enabled_ && should_collect()) {
        collect();
    }
    
    // تخصيص الذاكرة
    size_t total_size = sizeof(GCObject) + size - 1;
    GCObject* obj = static_cast<GCObject*>(
        MemoryAllocator::instance().allocate(total_size)
    );
    
    if (!obj) {
        // محاولة الجمع وإعادة المحاولة
        collect();
        obj = static_cast<GCObject*>(
            MemoryAllocator::instance().allocate(total_size)
        );
    }
    
    // تهيئة الكائن
    obj->type = type;
    obj->state = GCObjectState::White;
    obj->size = size;
    obj->pinned = false;
    
    // إضافة للقائمة
    obj->next = objects_list_;
    objects_list_ = obj;
    
    allocated_bytes_ += total_size;
    stats_.total_objects++;
    stats_.live_objects++;
    
    return obj;
}
```

## دعم الخيوط المتعددة (Threading)

بيئة التشغيل توفر دعماً للخيوط المتعددة مع مزامنة آمنة:

### مزامنة جامع القمامة

```cpp
class GarbageCollector {
private:
    std::mutex gc_mutex_;  // قفل لعمليات GC
    
public:
    void collect() {
        std::lock_guard<std::mutex> lock(gc_mutex_);
        // ... عملية الجمع
    }
};
```

### نقاط التوقف الآمنة (Safepoints)

الخيوط تتوقف عند نقاط آمنة أثناء جمع القمامة:

```cpp
void safepoint() {
    if (gc_requested) {
        // انتظر حتى ينتهي GC
        gc_condition.wait();
    }
}
```

## معالجة الاستثناءات

بيئة التشغيل توفر نظام استثناءات متكامل:

```cpp
struct Exception {
    std::string type;
    std::string message;
    std::string arabic_message;
    std::vector<StackFrame> stack_trace;
};

void Runtime::throwException(const std::string& type, 
                             const std::string& msg,
                             const std::string& msgAr) {
    Exception ex;
    ex.type = type;
    ex.message = msg;
    ex.arabic_message = msgAr;
    ex.stack_trace = captureStackTrace();
    
    // البحث عن معالج
    // ...
}
```

## واجهة الدوال الأجنبية (FFI)

للتواصل مع مكتبات C/C++:

```cpp
// تسجيل دالة أصلية
void Runtime::registerNativeFunction(
    const std::string& name,
    NativeFunction fn
) {
    native_functions_[name] = fn;
}

// استدعاء دالة أصلية من كود Sad
Value Runtime::callNative(const std::string& name,
                          const std::vector<Value>& args) {
    auto it = native_functions_.find(name);
    if (it != native_functions_.end()) {
        return it->second(args);
    }
    throwException("خطأ", "دالة أصلية غير موجودة: " + name);
}
```

## إحصائيات بيئة التشغيل

```cpp
struct RuntimeStats {
    // إحصائيات الذاكرة
    size_t total_allocated;
    size_t current_allocated;
    size_t peak_allocated;
    
    // إحصائيات GC
    uint64_t gc_collections;
    uint64_t gc_total_time;
    uint64_t gc_objects_freed;
    
    // إحصائيات عامة
    uint64_t objects_created;
    uint64_t function_calls;
    uint64_t exceptions_thrown;
};
```

## التهيئة والإغلاق

```cpp
bool Runtime::initialize() {
    // تهيئة المُخصِّص
    if (!MemoryAllocator::instance().initialize()) {
        return false;
    }
    
    // تهيئة جامع القمامة
    GCConfig gc_config;
    if (!GarbageCollector::instance().initialize(&gc_config)) {
        return false;
    }
    
    // تسجيل الدوال المدمجة
    registerBuiltins();
    
    return true;
}

void Runtime::shutdown() {
    // تحرير جميع الموارد
    GarbageCollector::instance().shutdown();
    MemoryAllocator::instance().shutdown();
}
```

## الخلاصة

بيئة التشغيل في لغة ص هي أساس تنفيذ البرامج. توفر إدارة ذاكرة تلقائية عبر جامع قمامة متطور، ودعم للخيوط المتعددة، ومعالجة استثناءات شاملة، وواجهة للتفاعل مع الكود الأصلي. كل هذه المكونات تعمل معاً بشفافية لتجعل البرمجة بلغة ص سهلة وآمنة، حيث لا يحتاج المبرمج للقلق بشأن تسريبات الذاكرة أو المشاكل المتعلقة بإدارة الموارد.
