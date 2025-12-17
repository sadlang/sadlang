# المرحلة 3: المكتبة القياسية للنظام
# Stage 3: System-Level Standard Library

**المدة المتوقعة / Duration:** 3-4 أشهر / months  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### العربية

هذه المرحلة تهدف لتطوير مكتبة قياسية شاملة تدعم البرمجة على مستوى النظام، بما في ذلك إدارة الذاكرة، العمليات، الملفات، الشبكات، والأجهزة. هذه المكتبة ضرورية لبناء kernel وتطبيقات نظام.

**الهدف الرئيسي:** توفير واجهات برمجية قوية وآمنة للتعامل مع موارد النظام.

### English

This stage aims to develop a comprehensive standard library supporting system-level programming, including memory management, processes, files, networking, and hardware. This library is essential for kernel development and system applications.

**Main Goal:** Provide powerful and safe APIs for system resource management.

---

## 🎯 المكونات الرئيسية / Main Components

### العربية

1. ✅ إدارة الذاكرة (Memory Management)
2. ✅ إدارة العمليات (Process Management)
3. ✅ نظام الملفات (File System)
4. ✅ الشبكات (Networking)
5. ✅ إدارة الأجهزة (Device Management)
6. ✅ System Calls Wrapper
7. ✅ مزامنة وخيوط (Synchronization & Threads)
8. ✅ معالجة الإشارات (Signal Handling)
9. ✅ إدارة الوقت (Time Management)

### English

1. ✅ Memory Management
2. ✅ Process Management
3. ✅ File System
4. ✅ Networking
5. ✅ Device Management
6. ✅ System Calls Wrapper
7. ✅ Synchronization & Threads
8. ✅ Signal Handling
9. ✅ Time Management

---

## 📝 المهام التفصيلية / Detailed Tasks

### 3.1 إدارة الذاكرة (Memory Management) - شهر واحد

#### العربية

**واجهات برمجية أساسية:**

```sad
# تخصيص وتحرير الذاكرة
مؤشر<فارغ> خصص_ذاكرة(رقم حجم) ;
خصص_ذاكرة_محاذية(رقم حجم, رقم محاذاة) ;
خصص_ذاكرة_صفرية(رقم عدد, رقم حجم) ;
حرر_ذاكرة(مؤشر<فارغ> عنوان) ;
إعادة_تخصيص(مؤشر<فارغ> عنوان, رقم حجم_جديد) ;

# Paging و Virtual Memory
صنف إدارة_الصفحات {
    دالة خريطة_صفحة(رقم عنوان_فعلي, رقم عنوان_افتراضي, رقم صلاحيات) ;
    دالة الغ_خريطة_صفحة(رقم عنوان_افتراضي) ;
    دالة احصل_عنوان_فعلي(رقم عنوان_افتراضي) -> رقم ;
    دالة تفعيل_جدول_صفحات(مؤشر<فارغ> جدول) ;
}

# Heap Allocator
صنف مُخصص_كومة {
    دالة تهيئة(مؤشر<فارغ> بداية, رقم حجم) ;
    دالة خصص(رقم حجم) -> مؤشر<فارغ> ;
    دالة حرر(مؤشر<فارغ> عنوان) ;
    دالة احصل_حجم_مستخدم() -> رقم ;
    دالة احصل_حجم_حر() -> رقم ;
}
```

#### English

**Core APIs:**

```sad
# Memory allocation and deallocation
pointer<void> allocate_memory(number size) ;
allocate_aligned_memory(number size, number alignment) ;
allocate_zeroed_memory(number count, number size) ;
free_memory(pointer<void> address) ;
reallocate(pointer<void> address, number new_size) ;

# Paging and Virtual Memory
class PageManager {
    function map_page(number physical_addr, number virtual_addr, number permissions) ;
    function unmap_page(number virtual_addr) ;
    function get_physical_address(number virtual_addr) -> number ;
    function enable_page_table(pointer<void> table) ;
}

# Heap Allocator
class HeapAllocator {
    function initialize(pointer<void> start, number size) ;
    function allocate(number size) -> pointer<void> ;
    function free(pointer<void> address) ;
    function get_used_size() -> number ;
    function get_free_size() -> number ;
}
```

**المهام:**
- [ ] تنفيذ buddy allocator أو slab allocator
- [ ] دعم paging (4KB, 2MB, 1GB pages)
- [ ] تنفيذ page fault handler
- [ ] اختبار على QEMU

---

### 3.2 إدارة العمليات (Process Management) - شهر واحد

#### العربية

```sad
# Process Control
صنف عملية {
    رقم معرّف ;
    نص اسم ;
    رقم أولوية ;
    حالة_عملية حالة ;
    
    دالة إنشاء(نص برنامج, مصفوفة معاملات) -> رقم ;
    دالة إنهاء(رقم رمز_خروج) ;
    دالة انتظر() -> رقم ;
    دالة ارسل_إشارة(رقم إشارة) ;
    دالة احصل_معرف() -> رقم ;
}

# Thread Management
صنف خيط {
    رقم معرّف ;
    دالة إنشاء(دالة وظيفة, مؤشر<فارغ> معامل) -> رقم ;
    دالة انضم() ;
    دالة فصل() ;
    دالة اخرج(مؤشر<فارغ> قيمة_إرجاع) ;
}

# Scheduler
صنف جدول_مهام {
    دالة إضافة_عملية(عملية ع) ;
    دالة إزالة_عملية(رقم معرّف) ;
    دالة جدول_تالي() -> عملية ;
    دالة تغيير_أولوية(رقم معرّف, رقم أولوية_جديدة) ;
}
```

#### English

```sad
# Process Control
class Process {
    number id ;
    string name ;
    number priority ;
    ProcessState state ;
    
    function create(string program, array args) -> number ;
    function terminate(number exit_code) ;
    function wait() -> number ;
    function send_signal(number signal) ;
    function get_id() -> number ;
}

# Thread Management
class Thread {
    number id ;
    function create(function func, pointer<void> arg) -> number ;
    function join() ;
    function detach() ;
    function exit(pointer<void> return_value) ;
}

# Scheduler
class Scheduler {
    function add_process(Process p) ;
    function remove_process(number id) ;
    function schedule_next() -> Process ;
    function change_priority(number id, number new_priority) ;
}
```

**المهام:**
- [ ] تنفيذ context switching
- [ ] دعم Round-Robin scheduler
- [ ] تنفيذ priority scheduler
- [ ] دعم multi-threading
- [ ] اختبار مع عمليات متعددة

---

### 3.3 نظام الملفات (File System) - شهر واحد

#### العربية

```sad
# File Operations
صنف ملف {
    دالة فتح(نص مسار, نص وضع) -> رقم ;
    دالة إغلاق(رقم معرّف) ;
    دالة قراءة(رقم معرّف, مؤشر<فارغ> buffer, رقم حجم) -> رقم ;
    دالة كتابة(رقم معرّف, مؤشر<فارغ> buffer, رقم حجم) -> رقم ;
    دالة طلب(رقم معرّف, رقم موضع, رقم من_أين) -> رقم ;
    دالة احصل_حجم(نص مسار) -> رقم ;
    دالة موجود(نص مسار) -> منطقي ;
}

# Directory Operations
صنف مجلد {
    دالة إنشاء(نص مسار) -> منطقي ;
    دالة حذف(نص مسار) -> منطقي ;
    دالة فتح(نص مسار) -> رقم ;
    دالة قراءة(رقم معرّف) -> نص ;
    دالة إغلاق(رقم معرّف) ;
}

# Virtual File System (VFS)
صنف نظام_ملفات_افتراضي {
    دالة تسجيل_نظام_ملفات(نص نوع, مؤشر<فارغ> عمليات) ;
    دالة تركيب(نص جهاز, نص نقطة_تركيب, نص نوع) -> منطقي ;
    دالة فك_تركيب(نص نقطة_تركيب) -> منطقي ;
}
```

#### English

```sad
# File Operations
class File {
    function open(string path, string mode) -> number ;
    function close(number fd) ;
    function read(number fd, pointer<void> buffer, number size) -> number ;
    function write(number fd, pointer<void> buffer, number size) -> number ;
    function seek(number fd, number offset, number whence) -> number ;
    function get_size(string path) -> number ;
    function exists(string path) -> boolean ;
}

# Directory Operations
class Directory {
    function create(string path) -> boolean ;
    function delete(string path) -> boolean ;
    function open(string path) -> number ;
    function read(number fd) -> string ;
    function close(number fd) ;
}

# Virtual File System (VFS)
class VirtualFileSystem {
    function register_filesystem(string type, pointer<void> operations) ;
    function mount(string device, string mountpoint, string type) -> boolean ;
    function unmount(string mountpoint) -> boolean ;
}
```

**المهام:**
- [ ] تنفيذ VFS layer
- [ ] دعم FAT32 filesystem (قراءة/كتابة)
- [ ] دعم Ext2 filesystem (أساسي)
- [ ] تنفيذ file descriptor table
- [ ] اختبار مع disk images

---

### 3.4 الشبكات (Networking) - شهر واحد

#### العربية

```sad
# Socket API
صنف مقبس {
    دالة إنشاء(رقم عائلة, رقم نوع, رقم بروتوكول) -> رقم ;
    دالة ربط(رقم مقبس, نص عنوان, رقم منفذ) -> منطقي ;
    دالة استماع(رقم مقبس, رقم عدد_انتظار) -> منطقي ;
    دالة قبول(رقم مقبس) -> رقم ;
    دالة اتصال(رقم مقبس, نص عنوان, رقم منفذ) -> منطقي ;
    دالة إرسال(رقم مقبس, مؤشر<فارغ> بيانات, رقم حجم) -> رقم ;
    دالة استقبال(رقم مقبس, مؤشر<فارغ> buffer, رقم حجم) -> رقم ;
    دالة إغلاق(رقم مقبس) ;
}

# Network Stack
صنف مكدس_شبكة {
    دالة تهيئة_شبكة() ;
    دالة إرسال_حزمة_IP(مؤشر<فارغ> حزمة, رقم حجم) ;
    دالة استقبال_حزمة_IP() -> مؤشر<فارغ> ;
    دالة تسجيل_محول_شبكة(مؤشر<فارغ> محول) ;
}
```

#### English

```sad
# Socket API
class Socket {
    function create(number family, number type, number protocol) -> number ;
    function bind(number socket, string address, number port) -> boolean ;
    function listen(number socket, number backlog) -> boolean ;
    function accept(number socket) -> number ;
    function connect(number socket, string address, number port) -> boolean ;
    function send(number socket, pointer<void> data, number size) -> number ;
    function receive(number socket, pointer<void> buffer, number size) -> number ;
    function close(number socket) ;
}

# Network Stack
class NetworkStack {
    function initialize_network() ;
    function send_ip_packet(pointer<void> packet, number size) ;
    function receive_ip_packet() -> pointer<void> ;
    function register_network_adapter(pointer<void> adapter) ;
}
```

**المهام:**
- [ ] تنفيذ TCP/IP stack (basic)
- [ ] دعم UDP و TCP
- [ ] تنفيذ ARP protocol
- [ ] دعم ICMP (ping)
- [ ] اختبار مع network emulation

---

### 3.5 إدارة الأجهزة (Device Management) - أسبوعان

#### العربية

```sad
# Device Driver Interface
صنف جهاز {
    نص اسم ;
    رقم نوع ;
    
    دالة تهيئة() -> منطقي ;
    دالة قراءة(مؤشر<فارغ> buffer, رقم حجم) -> رقم ;
    دالة كتابة(مؤشر<فارغ> بيانات, رقم حجم) -> رقم ;
    دالة تحكم(رقم أمر, مؤشر<فارغ> معامل) -> رقم ;
    دالة إغلاق() ;
}

# Device Manager
صنف مدير_أجهزة {
    دالة تسجيل_جهاز(جهاز ج) -> منطقي ;
    دالة إلغاء_تسجيل(نص اسم) -> منطقي ;
    دالة احصل_جهاز(نص اسم) -> جهاز ;
    دالة قائمة_أجهزة() -> مصفوفة ;
}
```

#### English

```sad
# Device Driver Interface
class Device {
    string name ;
    number type ;
    
    function initialize() -> boolean ;
    function read(pointer<void> buffer, number size) -> number ;
    function write(pointer<void> data, number size) -> number ;
    function control(number command, pointer<void> arg) -> number ;
    function close() ;
}

# Device Manager
class DeviceManager {
    function register_device(Device d) -> boolean ;
    function unregister(string name) -> boolean ;
    function get_device(string name) -> Device ;
    function list_devices() -> array ;
}
```

**المهام:**
- [ ] تصميم device driver framework
- [ ] تنفيذ serial port driver
- [ ] تنفيذ keyboard driver
- [ ] تنفيذ disk driver (basic)
- [ ] اختبار على QEMU

---

### 3.6 System Calls Wrapper - أسبوعان

#### العربية

```sad
# Syscall Interface
دالة نداء_نظام(رقم رقم_نداء, مصفوفة معاملات) -> رقم ;

# Common Syscalls
دالة sys_خرج(رقم رمز) ;
دالة sys_قراءة(رقم fd, مؤشر<فارغ> buf, رقم حجم) -> رقم ;
دالة sys_كتابة(رقم fd, مؤشر<فارغ> buf, رقم حجم) -> رقم ;
دالة sys_فتح(نص مسار, رقم صلاحيات) -> رقم ;
دالة sys_إغلاق(رقم fd) -> رقم ;
دالة sys_انتظر(رقم pid) -> رقم ;
دالة sys_تنفيذ(نص برنامج, مصفوفة معاملات) -> رقم ;
```

#### English

```sad
# Syscall Interface
function syscall(number syscall_number, array args) -> number ;

# Common Syscalls
function sys_exit(number code) ;
function sys_read(number fd, pointer<void> buf, number size) -> number ;
function sys_write(number fd, pointer<void> buf, number size) -> number ;
function sys_open(string path, number flags) -> number ;
function sys_close(number fd) -> number ;
function sys_wait(number pid) -> number ;
function sys_exec(string program, array args) -> number ;
```

**المهام:**
- [ ] تنفيذ syscall handler في kernel
- [ ] إنشاء wrapper functions لـ syscalls
- [ ] اختبار syscalls من user space
- [ ] توثيق جميع syscalls

---

### 3.7 مزامنة وخيوط (Synchronization & Threads) - أسبوعان

#### العربية

```sad
# Mutex
صنف قفل {
    دالة تهيئة() ;
    دالة قفل() ;
    دالة فك_قفل() ;
    دالة محاولة_قفل() -> منطقي ;
    دالة تدمير() ;
}

# Semaphore
صنف إشارة {
    دالة تهيئة(رقم قيمة_أولية) ;
    دالة انتظر() ;
    دالة إشارة() ;
    دالة احصل_قيمة() -> رقم ;
}

# Spinlock
صنف قفل_دوران {
    دالة قفل() ;
    دالة فك_قفل() ;
}
```

#### English

```sad
# Mutex
class Mutex {
    function initialize() ;
    function lock() ;
    function unlock() ;
    function try_lock() -> boolean ;
    function destroy() ;
}

# Semaphore
class Semaphore {
    function initialize(number initial_value) ;
    function wait() ;
    function signal() ;
    function get_value() -> number ;
}

# Spinlock
class Spinlock {
    function lock() ;
    function unlock() ;
}
```

**المهام:**
- [ ] تنفيذ mutex
- [ ] تنفيذ semaphore
- [ ] تنفيذ spinlock
- [ ] اختبار مع multiple threads
- [ ] اختبار deadlock detection

---

## 📊 معايير النجاح / Success Criteria

### العربية

✅ **المرحلة مكتملة عندما:**

1. جميع المكونات تعمل بشكل صحيح
2. الواجهات البرمجية موثقة بالكامل
3. اختبارات شاملة لكل مكون
4. أداء مماثل أو أفضل من C standard library
5. أمثلة عملية لكل مكون
6. دعم Linux و bare-metal
7. لا توجد memory leaks
8. thread-safe حيث ضروري

### English

✅ **Stage Complete When:**

1. All components work correctly
2. APIs fully documented
3. Comprehensive tests for each component
4. Performance comparable to or better than C standard library
5. Practical examples for each component
6. Support for Linux and bare-metal
7. No memory leaks
8. Thread-safe where necessary

---

**السابق / Previous:** [المرحلة 2: مترجم مستقل](02_stage2_compiler.md)  
**التالي / Next:** [المرحلة 4: دعم لغة التجميع](04_stage4_assembly.md)
