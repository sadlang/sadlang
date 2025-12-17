# المرحلة 1: دعم الميزات منخفضة المستوى
# Stage 1: Low-Level Features Support

**المدة المتوقعة / Duration:** 3-4 أشهر / months  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### العربية

هذه المرحلة تهدف لإضافة الميزات الأساسية منخفضة المستوى التي تسمح بالتعامل المباشر مع الذاكرة والعتاد. هذه الميزات ضرورية لبناء kernel وتطوير drivers.

**الهدف الرئيسي:** تمكين لغة ص من الوصول المباشر للذاكرة والعتاد.

### English

This stage aims to add essential low-level features that enable direct memory and hardware manipulation. These features are necessary for kernel development and driver programming.

**Main Goal:** Enable Sad Language to directly access memory and hardware.

---

## 🎯 الأهداف التفصيلية / Detailed Goals

### العربية

1. ✅ إضافة نوع بيانات المؤشرات (Pointers)
2. ✅ دعم عمليات القراءة/الكتابة المباشرة للذاكرة
3. ✅ إضافة عمليات Bitwise كاملة
4. ✅ دعم Memory Mapping
5. ✅ واجهات برمجية للوصول لـ I/O Ports
6. ✅ دعم Interrupts وHandlers
7. ✅ إضافة أنواع بيانات بحجم محدد (int8, int16, int32, int64, etc.)
8. ✅ دعم Volatile Variables
9. ✅ دعم Inline Functions للأداء العالي

### English

1. ✅ Add pointer data type
2. ✅ Support direct memory read/write operations
3. ✅ Add complete bitwise operations
4. ✅ Support memory mapping
5. ✅ Provide APIs for I/O port access
6. ✅ Support interrupts and handlers
7. ✅ Add fixed-size data types (int8, int16, int32, int64, etc.)
8. ✅ Support volatile variables
9. ✅ Support inline functions for high performance

---

## 🏗️ البنية المعمارية / Architecture

### 1. نظام أنواع البيانات الجديد / New Type System

```cpp
// العربية - Arabic Syntax
نوع المؤشر / Pointer Types:
    مؤشر<نوع>       // Pointer to type
    مؤشر<رقم>       // Pointer to number
    مؤشر<نص>        // Pointer to string
    مؤشر<فارغ>      // void pointer

أنواع محددة الحجم / Fixed-Size Types:
    صحيح8           // int8_t
    صحيح16          // int16_t
    صحيح32          // int32_t
    صحيح64          // int64_t
    غير_صحيح8      // uint8_t
    غير_صحيح16     // uint16_t
    غير_صحيح32     // uint32_t
    غير_صحيح64     // uint64_t
```

```cpp
// English - English Syntax
Pointer Types:
    pointer<type>
    pointer<number>
    pointer<string>
    pointer<void>

Fixed-Size Types:
    int8
    int16
    int32
    int64
    uint8
    uint16
    uint32
    uint64
```

### 2. عمليات المؤشرات / Pointer Operations

```sad
# العربية
مؤشر<رقم> عنوان = عنوان_من(متغير) ;    # Get address
رقم قيمة = قيمة_من(عنوان) ;              # Dereference
اكتب_في(عنوان, 42) ;                    # Write to address
رقم محتوى = اقرأ_من(عنوان) ;            # Read from address

# English
pointer<number> addr = address_of(variable) ;
number value = value_of(addr) ;
write_to(addr, 42) ;
number content = read_from(addr) ;
```

### 3. Memory Mapping

```sad
# العربية
مؤشر<فارغ> ذاكرة = خريطة_ذاكرة(0x1000, 4096) ;  # Map 4KB at 0x1000
اكتب_بايت(ذاكرة, 0, 0xFF) ;
رقم بايت = اقرأ_بايت(ذاكرة, 0) ;
الغ_خريطة(ذاكرة, 4096) ;

# English
pointer<void> memory = map_memory(0x1000, 4096) ;
write_byte(memory, 0, 0xFF) ;
number byte = read_byte(memory, 0) ;
unmap_memory(memory, 4096) ;
```

### 4. عمليات Bitwise

```sad
# العربية
رقم نتيجة_و = أ & ب ;          # Bitwise AND
رقم نتيجة_او = أ | ب ;         # Bitwise OR
رقم نتيجة_او_حصري = أ ^ ب ;   # Bitwise XOR
رقم نتيجة_ليس = ~أ ;          # Bitwise NOT
رقم نتيجة_ازاحة_يسار = أ << ب ;   # Left shift
رقم نتيجة_ازاحة_يمين = أ >> ب ;  # Right shift

# English
number result_and = a & b ;
number result_or = a | b ;
number result_xor = a ^ b ;
number result_not = ~a ;
number result_lshift = a << b ;
number result_rshift = a >> b ;
```

### 5. I/O Ports Access

```sad
# العربية
دالة اكتب_منفذ(رقم منفذ, رقم قيمة)
    # Assembly inline
    asm
        mov dx, منفذ
        mov al, قيمة
        out dx, al
    نهاية
نهاية

دالة اقرأ_منفذ(رقم منفذ) -> رقم
    رقم قيمة
    asm
        mov dx, منفذ
        in al, dx
        mov قيمة, al
    نهاية
    إرجاع قيمة
نهاية

# English
دالة write_port(number port, number value)
    asm
        mov dx, port
        mov al, value
        out dx, al
    نهاية
نهاية

دالة read_port(number port) -> number
    number value
    asm
        mov dx, port
        in al, dx
        mov value, al
    نهاية
    إرجاع value
نهاية
```

### 6. Interrupt Handling

```sad
# العربية
دالة متطاير معالج_مقاطعة()
    # Handler code
    اطبع("تم استدعاء معالج المقاطعة!")
نهاية

# تسجيل المعالج
سجل_معالج_مقاطعة(0x21, معالج_مقاطعة) ;

# English
دالة متطاير interrupt_handler()
    print("Interrupt handler called!")
نهاية

register_interrupt_handler(0x21, interrupt_handler) ;
```

---

## 📝 المهام التفصيلية / Detailed Tasks

### 1.1 تطوير نظام أنواع البيانات الجديد (أسبوعان)

#### العربية
- [ ] تصميم هيكل `PointerType` في AST
- [ ] إضافة توكنات جديدة للمؤشرات: `مؤشر`, `pointer`, `<`, `>`
- [ ] تحديث Parser لدعم تصريحات المؤشرات
- [ ] تحديث Type Checker للتحقق من صحة أنواع المؤشرات
- [ ] إضافة أنواع البيانات محددة الحجم (int8, int16, etc.)
- [ ] اختبار شامل لنظام الأنواع الجديد

#### English
- [ ] Design `PointerType` structure in AST
- [ ] Add new tokens for pointers: `مؤشر`, `pointer`, `<`, `>`
- [ ] Update Parser to support pointer declarations
- [ ] Update Type Checker to validate pointer types
- [ ] Add fixed-size data types (int8, int16, etc.)
- [ ] Comprehensive testing of new type system

**الملفات المتأثرة / Affected Files:**
- `include/parser/ast/types.h`
- `include/lexer/token.h`
- `src/parser/parser_core_impl.cpp`
- `src/interpreter/type_checker.cpp`

---

### 1.2 تنفيذ عمليات المؤشرات (أسبوعان)

#### العربية
- [ ] تنفيذ `عنوان_من()` / `address_of()`
- [ ] تنفيذ `قيمة_من()` / `value_of()` (dereference)
- [ ] تنفيذ `اكتب_في()` / `write_to()`
- [ ] تنفيذ `اقرأ_من()` / `read_from()`
- [ ] إضافة pointer arithmetic (+, -, ++, --)
- [ ] تنفيذ array indexing عبر pointers
- [ ] اختبار safety checks (null pointer, bounds checking)

#### English
- [ ] Implement `address_of()`
- [ ] Implement `value_of()` (dereference)
- [ ] Implement `write_to()`
- [ ] Implement `read_from()`
- [ ] Add pointer arithmetic (+, -, ++, --)
- [ ] Implement array indexing via pointers
- [ ] Test safety checks (null pointer, bounds checking)

**الملفات الجديدة / New Files:**
- `src/stdlib/memory/pointer_ops.cpp`
- `include/stdlib/memory/pointer_ops.h`

---

### 1.3 إضافة عمليات Bitwise (أسبوع واحد)

#### العربية
- [ ] إضافة operators: `&`, `|`, `^`, `~`, `<<`, `>>`
- [ ] تحديث Lexer لدعم الرموز الجديدة
- [ ] تحديث Parser لتحليل التعابير Bitwise
- [ ] تنفيذ Evaluator للعمليات Bitwise
- [ ] اختبار جميع العمليات مع أنواع البيانات المختلفة

#### English
- [ ] Add operators: `&`, `|`, `^`, `~`, `<<`, `>>`
- [ ] Update Lexer to support new symbols
- [ ] Update Parser to parse bitwise expressions
- [ ] Implement Evaluator for bitwise operations
- [ ] Test all operations with different data types

**الملفات المتأثرة / Affected Files:**
- `src/lexer/lexer.cpp`
- `src/parser/parser_core_impl.cpp`
- `src/interpreter/visitors/expression_evaluator.cpp`

---

### 1.4 تنفيذ Memory Mapping (أسبوعان)

#### العربية
- [ ] تصميم واجهة `خريطة_ذاكرة()` / `map_memory()`
- [ ] تنفيذ `الغ_خريطة()` / `unmap_memory()`
- [ ] دعم flags مختلفة (read, write, execute)
- [ ] تنفيذ `اكتب_بايت()` / `write_byte()`
- [ ] تنفيذ `اقرأ_بايت()` / `read_byte()`
- [ ] إضافة دعم write/read بأحجام مختلفة (word, dword, qword)
- [ ] اختبار memory mapping على عناوين مختلفة

#### English
- [ ] Design `map_memory()` interface
- [ ] Implement `unmap_memory()`
- [ ] Support different flags (read, write, execute)
- [ ] Implement `write_byte()`
- [ ] Implement `read_byte()`
- [ ] Add support for different sizes (word, dword, qword)
- [ ] Test memory mapping on different addresses

**الملفات الجديدة / New Files:**
- `src/stdlib/memory/memory_mapping.cpp`
- `include/stdlib/memory/memory_mapping.h`

---

### 1.5 إضافة دعم I/O Ports (أسبوعان)

#### العربية
- [ ] تنفيذ `اكتب_منفذ()` / `write_port()`
- [ ] تنفيذ `اقرأ_منفذ()` / `read_port()`
- [ ] دعم أحجام مختلفة (byte, word, dword)
- [ ] إضافة inline assembly support (مبدئي)
- [ ] اختبار على أجهزة وهمية (QEMU)
- [ ] توثيق I/O ports الشائعة (serial, keyboard, etc.)

#### English
- [ ] Implement `write_port()`
- [ ] Implement `read_port()`
- [ ] Support different sizes (byte, word, dword)
- [ ] Add inline assembly support (preliminary)
- [ ] Test on virtual hardware (QEMU)
- [ ] Document common I/O ports (serial, keyboard, etc.)

**الملفات الجديدة / New Files:**
- `src/stdlib/hardware/io_ports.cpp`
- `include/stdlib/hardware/io_ports.h`

---

### 1.6 تنفيذ Interrupt Handling (ثلاثة أسابيع)

#### العربية
- [ ] تصميم نظام تسجيل interrupt handlers
- [ ] تنفيذ `سجل_معالج_مقاطعة()` / `register_interrupt_handler()`
- [ ] تنفيذ `الغ_معالج_مقاطعة()` / `unregister_interrupt_handler()`
- [ ] إضافة دعم `متقلب` / `volatile` keyword
- [ ] تنفيذ Interrupt Descriptor Table (IDT) wrapper
- [ ] اختبار مع timer interrupt (IRQ0)
- [ ] اختبار مع keyboard interrupt (IRQ1)

#### English
- [ ] Design interrupt handler registration system
- [ ] Implement `register_interrupt_handler()`
- [ ] Implement `unregister_interrupt_handler()`
- [ ] Add `volatile` keyword support
- [ ] Implement Interrupt Descriptor Table (IDT) wrapper
- [ ] Test with timer interrupt (IRQ0)
- [ ] Test with keyboard interrupt (IRQ1)

**الملفات الجديدة / New Files:**
- `src/stdlib/hardware/interrupts.cpp`
- `include/stdlib/hardware/interrupts.h`
- `src/stdlib/hardware/idt.cpp`

---

### 1.7 إضافة Volatile Variables (أسبوع واحد)

#### العربية
- [ ] إضافة keyword `متقلب` / `volatile`
- [ ] تحديث Parser لدعم volatile declarations
- [ ] تحديث code generator لمنع optimizations على volatile vars
- [ ] اختبار مع hardware registers
- [ ] توثيق استخدامات volatile

#### English
- [ ] Add `متقلب` / `volatile` keyword
- [ ] Update Parser to support volatile declarations
- [ ] Update code generator to prevent optimizations on volatile vars
- [ ] Test with hardware registers
- [ ] Document volatile use cases

---

### 1.8 تنفيذ Inline Functions (أسبوع واحد)

#### العربية
- [ ] إضافة keyword `مضمّن` / `inline`
- [ ] تحديث Parser لدعم inline functions
- [ ] تحديث code generator لـ inline expansion
- [ ] اختبار الأداء مقارنة بـ regular functions
- [ ] توثيق أفضل ممارسات استخدام inline

#### English
- [ ] Add `مضمّن` / `inline` keyword
- [ ] Update Parser to support inline functions
- [ ] Update code generator for inline expansion
- [ ] Test performance vs regular functions
- [ ] Document inline best practices

---

## 🧪 الاختبارات / Testing

### العربية

#### اختبارات الوحدة / Unit Tests
- اختبار كل عملية مؤشر بشكل منفصل
- اختبار عمليات bitwise مع جميع أنواع البيانات
- اختبار memory mapping مع عناوين مختلفة
- اختبار I/O ports على emulator
- اختبار interrupt handlers

#### اختبارات التكامل / Integration Tests
- برنامج يجمع بين pointers + bitwise + memory mapping
- driver بسيط يستخدم I/O ports + interrupts
- برنامج يكتب ويقرأ من ذاكرة mapped

#### اختبارات الأداء / Performance Tests
- مقارنة سرعة pointer operations مع C
- قياس overhead لـ inline functions
- benchmark لعمليات bitwise

### English

#### Unit Tests
- Test each pointer operation separately
- Test bitwise operations with all data types
- Test memory mapping with different addresses
- Test I/O ports on emulator
- Test interrupt handlers

#### Integration Tests
- Program combining pointers + bitwise + memory mapping
- Simple driver using I/O ports + interrupts
- Program writing and reading from mapped memory

#### Performance Tests
- Compare pointer operation speed with C
- Measure inline function overhead
- Bitwise operation benchmarks

---

## 📊 معايير النجاح / Success Criteria

### العربية

✅ **المرحلة مكتملة عندما:**

1. جميع أنواع البيانات الجديدة تعمل بشكل صحيح
2. عمليات المؤشرات آمنة وسريعة
3. عمليات Bitwise تعطي نفس نتائج C
4. Memory mapping يعمل على QEMU
5. I/O ports يمكنها التحكم بأجهزة وهمية
6. Interrupt handlers تستجيب بشكل صحيح
7. جميع الاختبارات تمر بنجاح (100% pass rate)
8. التوثيق كامل مع أمثلة عملية
9. لا توجد memory leaks أو segmentation faults

### English

✅ **Stage Complete When:**

1. All new data types work correctly
2. Pointer operations are safe and fast
3. Bitwise operations match C results
4. Memory mapping works on QEMU
5. I/O ports can control virtual devices
6. Interrupt handlers respond correctly
7. All tests pass (100% pass rate)
8. Complete documentation with practical examples
9. No memory leaks or segmentation faults

---

## 🔗 الربط مع المراحل الأخرى / Links to Other Stages

- **المرحلة 2 (Compiler):** يعتمد على أنواع البيانات الجديدة
- **المرحلة 3 (System Library):** يستخدم pointer operations
- **المرحلة 4 (Assembly):** يتكامل مع inline assembly
- **المرحلة 6 (Kernel):** أساس لتطوير kernel

---

## 📚 موارد إضافية / Additional Resources

### العربية

- [Intel x86-64 Manual - Volume 1](https://software.intel.com/sites/default/files/managed/39/c5/325462-sdm-vol-1-2abcd-3abcd.pdf)
- [OSDev Wiki - Memory Management](https://wiki.osdev.org/Memory_Management)
- [OSDev Wiki - Interrupts](https://wiki.osdev.org/Interrupts)
- [OSDev Wiki - I/O Ports](https://wiki.osdev.org/I/O_Ports)

### English

- [Intel x86-64 Manual - Volume 1]
- [OSDev Wiki - Memory Management]
- [OSDev Wiki - Interrupts]
- [OSDev Wiki - I/O Ports]

---

**التالي / Next:** [المرحلة 2: مترجم مستقل](02_stage2_compiler.md)
