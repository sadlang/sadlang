# 🧠 قواعد إدارة الذاكرة / Memory Management Rules

**التاريخ:** 2 ديسمبر 2025  
**الإصدار:** 1.0 - Stage 1  
**الحالة:** مرجع لنظام إدارة الذاكرة منخفض المستوى

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **نظام إدارة الذاكرة منخفض المستوى** للغة "ص"، بما في ذلك تخصيص الذاكرة الديناميكي (malloc/free)، تعيين الذاكرة (mmap/munmap)، حماية الذاكرة (mprotect)، والكشف عن تسريبات الذاكرة.

### (EN) Description
This file documents the **low-level memory management system** لكل Sad language, including dynamic memory allocation (malloc/free), memory mapping (mmap/munmap), memory protection (mprotect), and leak detection.

---

## 📦 الاعتماديات / Dependencies

- ✅ `include/low_level/memory_map.h` - MemoryManager class
- ✅ `include/low_level/pointer_type.h` - Pointer system integration
- ✅ `docs/system_build_plan/01_stage1_low_level.md` - Memory features spec
- ✅ `src/low_level/memory_manager.cpp` - Implementation

---

## 📋 قائمة العمليات / Operations List

| # | العملية (AR) | Operation (EN) | النوع | الحالة |
|---|------------|---------------|------|-------|
| 1 | `malloc` | Allocate Memory | تخصيص | ✅ Core |
| 2 | `free` | Free Memory | تحرير | ✅ Core |
| 3 | `calloc` | Allocate + Zero | تخصيص بصفر | ✅ Core |
| 4 | `realloc` | Reallocate | إعادة تخصيص | ✅ Core |
| 5 | `mmap` | Memory Map | تعيين ذاكرة | 🚧 Advanced |
| 6 | `munmap` | Unmap Memory | إلغاء تعيين | 🚧 Advanced |
| 7 | `mprotect` | Memory Protection | حماية ذاكرة | 🚧 Advanced |
| 8 | `detect_leaks` | Leak Detection | كشف التسريب | 🔧 Debug |

---

## 1️⃣ malloc - تخصيص الذاكرة / Memory Allocation

### Syntax / البناء النحوي (EBNF)

```ebnf
malloc_call ::= 'خصص_ذاكرة' '(' size_expr ')'
size_expr   ::= expression  # Returns integer (bytes)
```

**الشرح بالعربية:**
- `malloc_call`: استدعاء malloc يأخذ تعبير يحسب حجم الذاكرة المطلوب بالبايت
- يعيد مؤشر مؤشر<any> للذاكرة المخصصة، أو null عند الفشل

### Semantics / الدلالات

- **Allocation / التخصيص:** يخصص كتلة ذاكرة بحجم محدد من الـ heap
- **Return / الإرجاع:** يعيد `مؤشر<any>` للذاكرة المخصصة، أو `null` عند الفشل
- **Lifetime / العمر:** الذاكرة تبقى حتى يتم تحريرها صراحة بـ `free`
- **Initialization / التهيئة:** الذاكرة **غير مهيأة** (تحتوي على قيم عشوائية)

### Examples / الأمثلة

```s
# مثال بسيط / Simple example
مؤشر<رقم> ptr = خصص_ذاكرة(8)  # 8 bytes = 1 integer (64-bit)
إذا (ptr == null)
    اطبع("فشل تخصيص الذاكرة!")
    إرجاع -1
نهاية

# استخدام المؤشر / Use the pointer
اكتب_في(ptr, 42)
اطبع(قيمة_من(ptr))  # 42

# تحرير الذاكرة / Free memory
حرر(ptr)

# تخصيص مصفوفة / Allocate array
مؤشر<رقم> arr = خصص_ذاكرة(40)  # 40 bytes = 5 integers
لكل ع في مدى(5)
    اكتب_في(عنوان_من(arr, ع * 8), ع * 10)
نهاية

# طباعة المصفوفة / Print array
لكل ع في مدى(5)
    اطبع(ن"arr[{ع}] = {قيمة_من(عنوان_من(arr, ع * 8))}")
نهاية

حرر(arr)

# تخصيص بنية / Allocate structure
مؤشر<any> person = خصص_ذاكرة(32)  # Name (16) + Age (8) + ID (8)
اكتب_في(person, "أحمد")
اكتب_في(عنوان_من(person, 16), 25)
اكتب_في(عنوان_من(person, 24), 12345)

حرر(person)
```

### C++ API

```cpp
# include/low_level/memory_map.h
class MemoryManager {
public:
    # Allocate memory block
    void* خصص_ذاكرة(size_t size);
    
    # Get allocation info
    AllocationInfo getAllocationInfo(void* ptr);
    
    # Track allocations لكل leak detection
    void trackAllocation(void* ptr, size_t size, 
                        const std::string& source_location);
};

struct AllocationInfo {
    void* address;
    size_t size;
    bool is_freed;
    std::string source_location;
    std::chrono::time_point<std::chrono::system_clock> alloc_time;
};
```

---

## 2️⃣ free - تحرير الذاكرة / Free Memory

### Syntax / البناء النحوي (EBNF)

```ebnf
free_call ::= 'حرر' '(' pointer_expr ')'
pointer_expr ::= expression  # Returns pointer
```

### Semantics / الدلالات

- **Deallocation / إلغاء التخصيص:** يحرر الذاكرة المخصصة مسبقاً بـ malloc/calloc/realloc
- **Safety / الأمان:** تحرير نفس المؤشر مرتين يسبب **double-free error**
- **Null Safety / أمان null:** استدعاء `حرر(null)` آمن (لا يفعل شيئاً)
- **Dangling Pointers / المؤشرات المعلقة:** بعد free، المؤشر يصبح **معلقاً** (يشير لذاكرة محررة)

### Examples / الأمثلة

```s
# مثال صحيح / Correct example
مؤشر<رقم> p = خصص_ذاكرة(8)
اكتب_في(p, 100)
حرر(p)
# p الآن مؤشر معلق - لا تستخدمه!

# خطأ: double-free / Error: double-free
مؤشر<رقم> p2 = خصص_ذاكرة(8)
حرر(p2)
حرر(p2)  # ❌ خطأ: تحرير مرتين!

# آمن: حرر(null) / Safe: حرر(null)
مؤشر<رقم> p3 = null
حرر(p3)  # ✅ آمن - لا يفعل شيئاً

# نمط آمن / Safe pattern
مؤشر<رقم> p4 = خصص_ذاكرة(16)
إذا (p4 != null)
    اكتب_في(p4, 200)
    حرر(p4)
    p4 = null  # ✅ ضع null لتجنب dangling pointer
نهاية
```

### Best Practices / أفضل الممارسات

```s
# 1. دائماً تحقق من null بعد malloc / Always check لكل null after malloc
مؤشر<أي> data = خصص_ذاكرة(1024)
إذا (data == null)
    # معالجة الخطأ / Handle error
    إرجاع -1
نهاية

# 2. حرر كل ذاكرة مخصصة / Free all allocated memory
مؤشر<رقم> p = خصص_ذاكرة(8)
# ... استخدام ...
حرر(p)
p = null  # منع dangling pointer

# 3. استخدم calloc للتهيئة بصفر / Use calloc لكل zero initialization
مؤشر<رقم> arr = خصص_وصفر(10, 8)  # 10 integers, each 8 bytes
# الذاكرة كلها أصفار الآن / Memory is all zeros now

# 4. استخدم realloc لتغيير الحجم / Use realloc to resize
مؤشر<رقم> buf = خصص_ذاكرة(40)  # 5 integers
buf = أعد_تخصيص(buf, 80)         # Resize to 10 integers
```

---

## 3️⃣ calloc - تخصيص مع التصفير / Allocate and Zero

### Syntax / البناء النحوي (EBNF)

```ebnf
calloc_call ::= 'خصص_وصفر' '(' count_expr ',' size_expr ')'
count_expr  ::= expression  # Number of elements
size_expr   ::= expression  # Size of each element in bytes
```

### Semantics / الدلالات

- **Zero Initialization / التصفير:** يخصص ذاكرة ويصفرها (كل البايتات = 0)
- **Total Size / الحجم الكلي:** `count * size` بايت
- **Return / الإرجاع:** `مؤشر<any>` أو `null` عند الفشل
- **Use Case / حالة الاستخدام:** مفيد للمصفوفات والبنيات التي تحتاج تهيئة

### Examples / الأمثلة

```s
# مصفوفة أرقام مصفرة / Zeroed integer array
مؤشر<رقم> numbers = خصص_وصفر(100, 8)  # 100 integers, 8 bytes each
# كل العناصر = 0 الآن / All elements are 0 now

لكل ع في مدى(10)
    اطبع(قيمة_من(عنوان_من(numbers, ع * 8)))  # يطبع: 0
نهاية

حرر(numbers)

# مقارنة malloc vs calloc / Compare malloc vs calloc
مؤشر<رقم> p1 = خصص_ذاكرة(8)   # قيمة عشوائية / Random value
مؤشر<رقم> p2 = خصص_وصفر(1, 8) # صفر / Zero

اطبع(ن"malloc: {قيمة_من(p1)}")  # قد يطبع أي شيء
اطبع(ن"calloc: {قيمة_من(p2)}")  # يطبع: 0

حرر(p1)
حرر(p2)
```

---

## 4️⃣ realloc - إعادة التخصيص / Reallocate Memory

### Syntax / البناء النحوي (EBNF)

```ebnf
realloc_call ::= 'أعد_تخصيص' '(' pointer_expr ',' new_size_expr ')'
pointer_expr ::= expression  # Existing pointer
new_size_expr ::= expression # New size in bytes
```

### Semantics / الدلالات

- **Resize / تغيير الحجم:** يغير حجم كتلة ذاكرة موجودة
- **Copy / النسخ:** قد ينسخ البيانات لموقع جديد إذا لزم الأمر
- **Return / الإرجاع:** مؤشر جديد (قد يكون نفس القديم أو مختلف)
- **Old Pointer / المؤشر القديم:** يصبح **invalid** بعد realloc

### Examples / الأمثلة

```s
# توسيع مصفوفة / Expand array
مؤشر<رقم> arr = خصص_ذاكرة(40)  # 5 integers
لكل ع في مدى(5)
    اكتب_في(عنوان_من(arr, ع * 8), ع)
نهاية

# توسيع إلى 10 عناصر / Expand to 10 elements
arr = أعد_تخصيص(arr, 80)
إذا (arr == null)
    اطبع("فشل realloc!")
    إرجاع -1
نهاية

# إضافة عناصر جديدة / Add new elements
لكل ع في مدى(5, 10)
    اكتب_في(عنوان_من(arr, ع * 8), ع * 2)
نهاية

حرر(arr)

# تقليص الحجم / Shrink size
مؤشر<رقم> buf = خصص_ذاكرة(1000)
# ... استخدام جزء فقط ...
buf = أعد_تخصيص(buf, 100)  # تقليص إلى 100 بايت
حرر(buf)
```

---

## 5️⃣ mmap - تعيين الذاكرة / Memory Mapping

### Syntax / البناء النحوي (EBNF)

```ebnf
mmap_call ::= 'عين_ذاكرة' '(' size_expr ',' flags_expr ')'
flags_expr ::= 'خاص' | 'مشترك' | 'مجهول'
             | flags_expr '|' flags_expr
```

### Semantics / الدلالات

- **Mapping / التعيين:** يعين منطقة ذاكرة كبيرة من الـ kernel
- **Flags / الرايات:**
  - `خاص`: تعيين خاص (تغييرات غير مرئية للعمليات الأخرى)
  - `مشترك`: تعيين مشترك (مرئي للعمليات الأخرى)
  - `مجهول`: بدون ملف (ذاكرة فقط)
- **Use Case / حالة الاستخدام:** تخصيصات كبيرة، memory-mapped files

### Examples / الأمثلة

```s
# تعيين 1MB ذاكرة / Map 1MB memory
مؤشر<أي> region = عين_ذاكرة(1048576, خاص | مجهول)
إذا (region == null)
    اطبع("فشل mmap!")
    إرجاع -1
نهاية

# استخدام الذاكرة / Use the memory
اكتب_في(region, 12345)

# إلغاء التعيين / Unmap
ألغ_تعيين(region, 1048576)

# تعيين مشترك / Shared mapping
مؤشر<أي> shared = عين_ذاكرة(4096, مشترك | مجهول)
# يمكن مشاركتها مع عمليات أخرى / Can be shared with other processes
ألغ_تعيين(shared, 4096)
```

---

## 6️⃣ mprotect - حماية الذاكرة / Memory Protection

### Syntax / البناء النحوي (EBNF)

```ebnf
mprotect_call ::= 'احم_ذاكرة' '(' pointer_expr ',' size_expr ',' prot_flags ')'
prot_flags    ::= 'قراءة' | 'كتابة' | 'تنفيذ' | 'بدون'
                | prot_flags '|' prot_flags
```

### Semantics / الدلالات

- **Protection / الحماية:** يغير أذونات الوصول لمنطقة ذاكرة
- **Flags / الرايات:**
  - `قراءة`: قراءة مسموحة
  - `كتابة`: كتابة مسموحة
  - `تنفيذ`: تنفيذ كود مسموح
  - `بدون`: لا يوجد وصول (محمية تماماً)

### Examples / الأمثلة

```s
# حماية منطقة للقراءة فقط / Protect region as read-only
مؤشر<أي> data = عين_ذاكرة(4096, خاص | مجهول)
اكتب_في(data, 42)  # ✅ ناجح

# تحويل إلى قراءة فقط / Convert to read-only
احم_ذاكرة(data, 4096, قراءة)
# اكتب_في(data, 100)  # ❌ سيسبب segmentation fault

# إعادة تمكين الكتابة / Re-enable write
احم_ذاكرة(data, 4096, قراءة | كتابة)
اكتب_في(data, 100)  # ✅ ناجح الآن

ألغ_تعيين(data, 4096)
```

---

## 7️⃣ Memory Leak Detection (كشف تسريبات الذاكرة)

### Syntax / البناء النحوي

```ebnf
leak_check ::= 'detect_leaks' '(' ')'
leak_report ::= 'memory_report' '(' ')'
```

### Examples / الأمثلة

```s
# مثال مع تسريب / Example with leak
دالة test_leak()
    مؤشر<رقم> p1 = خصص_ذاكرة(100)
    مؤشر<رقم> p2 = خصص_ذاكرة(200)
    
    حرر(p1)
    # نسينا حرر(p2) - تسريب! / Forgot حرر(p2) - leak!
نهاية

test_leak()

# كشف التسريبات / Detect leaks
اكشف_التسريبات()
# Output: "تسريب ذاكرة: 200 bytes في السطر 3"

# تقرير كامل / Full report
تقرير_الذاكرة()
# Output:
# ===== تقرير الذاكرة =====
# مخصص: 200 bytes
# محرر: 100 bytes
# تسريبات: 200 bytes (1 allocation)
```

### C++ Integration

```cpp
# include/low_level/memory_map.h
class MemoryManager {
public:
    # Leak detection
    std::vector<AllocationInfo> detectLeaks();
    
    # Memory statistics
    MemoryStats getStats() const;
    
    # Enable/disable tracking
    void enableTracking(bool enable);
};

struct MemoryStats {
    size_t total_allocated;
    size_t total_freed;
    size_t current_usage;
    size_t peak_usage;
    size_t allocation_count;
    size_t free_count;
};
```

---

## 🧪 Testing Examples / أمثلة الاختبار

```s
# اختبار كامل / Complete test
دالة test_memory()
    # Test 1: malloc/free
    مؤشر<رقم> p1 = خصص_ذاكرة(8)
    assert(p1 != null, "malloc failed")
    اكتب_في(p1, 42)
    assert(قيمة_من(p1) == 42, "write/read failed")
    حرر(p1)
    
    # Test 2: calloc
    مؤشر<رقم> p2 = خصص_وصفر(5, 8)
    assert(قيمة_من(p2) == 0, "calloc should zero memory")
    حرر(p2)
    
    # Test 3: realloc
    مؤشر<رقم> p3 = خصص_ذاكرة(40)
    p3 = أعد_تخصيص(p3, 80)
    assert(p3 != null, "realloc failed")
    حرر(p3)
    
    # Test 4: mmap/munmap
    مؤشر<أي> region = عين_ذاكرة(4096, خاص | مجهول)
    assert(region != null, "mmap failed")
    ألغ_تعيين(region, 4096)
    
    اطبع("✅ جميع الاختبارات نجحت!")
نهاية

test_memory()
```

---

## 📊 Memory Regions Table / جدول مناطق الذاكرة

| المنطقة (AR) | Region (EN) | الاستخدام | الحجم النموذجي |
|------------|-----------|---------|---------------|
| Stack | Stack | متغيرات محلية | ~8 MB |
| Heap | Heap | malloc/free | ديناميكي |
| Mapped | Memory-mapped | mmap | حسب الطلب |
| Static | Static data | ثوابت عامة | ثابت |
| Code | Text segment | كود البرنامج | ثابت |

---

## ⚠️ Common Errors / أخطاء شائعة

```s
# ❌ 1. Double-free
مؤشر<رقم> p = خصص_ذاكرة(8)
حرر(p)
حرر(p)  # خطأ!

# ❌ 2. Use-after-free
مؤشر<رقم> p = خصص_ذاكرة(8)
حرر(p)
اكتب_في(p, 42)  # خطأ! الذاكرة محررة

# ❌ 3. Memory leak
دالة leak()
    مؤشر<رقم> p = خصص_ذاكرة(1000)
    # نسيت حرر(p)
نهاية

# ❌ 4. Buffer overflow
مؤشر<رقم> arr = خصص_ذاكرة(40)  # 5 integers
اكتب_في(عنوان_من(arr, 80), 42)  # خطأ! خارج الحدود

# ✅ الحل الصحيح / Correct solution
مؤشر<رقم> arr = خصص_ذاكرة(40)
لكل ع في مدى(5)
    إذا (ع * 8 < 40)
        اكتب_في(عنوان_من(arr, ع * 8), ع)
    نهاية
نهاية
حرر(arr)
```

---

## 🔧 Implementation Notes

**Files:**
- `include/low_level/memory_map.h` - MemoryManager class definition
- `src/low_level/memory_manager.cpp` - Implementation
- `include/stdlib/builtin_memory.h` - Built-in memory functions
- `tests/low_level/test_memory.s` - Test suite

**Key Features:**
- Allocation tracking لكل leak detection
- Double-free protection
- Memory statistics and reporting
- Integration with pointer system
- Debug mode with extra checks

---

**آخر تحديث:** 2 ديسمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 11 KB
