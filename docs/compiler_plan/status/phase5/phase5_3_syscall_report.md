# Phase 5.3: System Calls Wrapper - تقرير الإنجاز
## بسم الله الرحمن الرحيم

**تاريخ الإنجاز**: 2025-12-22  
**الحالة**: ✅ مكتمل بنجاح  
**الوقت المستغرق**: جلسة عمل واحدة

---

## 📊 إحصائيات المشروع / Project Statistics

### الأسطر المكتوبة / Lines of Code
```
syscall.h:      404 سطر
syscall.cpp:  1,154 سطر
━━━━━━━━━━━━━━━━━━━━━
المجموع:     1,558 سطر
```

### المكونات / Components
- **File I/O Operations**: 12 دالة أساسية
- **Memory Operations**: 5 دوال للذاكرة
- **Process Operations**: 7 عمليات للعمليات
- **System Information**: 6 دوال معلومات
- **Error Handling**: نظام أخطاء كامل
- **Helper Functions**: 5 دوال مساعدة

---

## 🎯 الميزات الرئيسية / Key Features

### 1. File I/O Operations (عمليات الملفات) ✅

#### الدوال الأساسية:
```cpp
FileHandle sys_open(const char* path, OpenMode mode, FilePermissions perms);
bool sys_close(FileHandle handle);
int64_t sys_read(FileHandle handle, void* buffer, size_t count);
int64_t sys_write(FileHandle handle, const void* buffer, size_t count);
int64_t sys_seek(FileHandle handle, int64_t offset, SeekMode mode);
```

#### دوال البيانات الوصفية:
```cpp
bool sys_stat(const char* path, FileInfo* info);
bool sys_fstat(FileHandle handle, FileInfo* info);
bool sys_unlink(const char* path);
bool sys_mkdir(const char* path, FilePermissions perms);
bool sys_rmdir(const char* path);
bool sys_rename(const char* old_path, const char* new_path);
```

#### المنصات المدعومة:
- ✅ **Windows**: CreateFile, ReadFile, WriteFile, CloseHandle
- ✅ **Linux**: open, read, write, close (POSIX)
- ✅ **macOS**: open, read, write, close (POSIX)

#### أوضاع الفتح:
```cpp
enum class OpenMode {
    ReadOnly,    // قراءة فقط
    WriteOnly,   // كتابة فقط
    ReadWrite,   // قراءة وكتابة
    Create,      // إنشاء إذا غير موجود
    Truncate,    // قص الملف
    Append,      // الإضافة للنهاية
    Exclusive    // فشل إذا موجود
};
```

---

### 2. Memory Operations (عمليات الذاكرة) ✅

#### التعيين والإلغاء:
```cpp
void* sys_mmap(void* addr, size_t length, MemoryProtection prot, 
               MMapFlags flags, FileHandle fd, int64_t offset);
bool sys_munmap(void* addr, size_t length);
bool sys_mprotect(void* addr, size_t length, MemoryProtection prot);
```

#### Heap Management:
```cpp
void* sys_brk(void* addr);         // Linux/macOS only
void* sys_sbrk(intptr_t increment); // Linux/macOS only
```

#### المنصات المدعومة:
- ✅ **Windows**: VirtualAlloc, VirtualFree, VirtualProtect
- ✅ **Linux**: mmap, munmap, mprotect, brk, sbrk
- ✅ **macOS**: mmap, munmap, mprotect, brk, sbrk

#### أوضاع الحماية:
```cpp
enum class MemoryProtection {
    None,       // بدون وصول
    Read,       // قراءة
    Write,      // كتابة
    Execute,    // تنفيذ
    ReadWrite,  // قراءة وكتابة
    ReadExec,   // قراءة وتنفيذ
    All         // كل الأذونات
};
```

---

### 3. Process Operations (عمليات العمليات) ✅

#### إنشاء العمليات:
```cpp
ProcessId sys_fork();  // Linux/macOS only
int sys_exec(const char* path, 
             const std::vector<std::string>& args,
             const std::vector<std::string>& env);
ProcessId sys_wait(ProcessId pid, int* status);
void sys_exit(int code);
```

#### معلومات العمليات:
```cpp
ProcessId sys_getpid();   // معرف العملية الحالية
ProcessId sys_getppid();  // معرف العملية الأب
bool sys_kill(ProcessId pid, int signal);
```

#### المنصات المدعومة:
- ✅ **Windows**: CreateProcess, WaitForSingleObject, TerminateProcess
- ✅ **Linux**: fork, execve, waitpid, kill
- ✅ **macOS**: fork, execve, waitpid, kill

**ملاحظة**: `sys_fork()` غير مدعوم على Windows (يعيد خطأ `NotSupported`)

---

### 4. System Information (معلومات النظام) ✅

#### دوال المعلومات:
```cpp
std::string sys_gethostname();    // اسم المضيف
std::string sys_getusername();    // اسم المستخدم
std::string sys_getcwd();         // المجلد الحالي
bool sys_chdir(const char* path); // تغيير المجلد

std::string sys_getenv(const char* name);
bool sys_setenv(const char* name, const char* value, bool overwrite);
```

#### التنفيذ:
- ✅ **Windows**: GetComputerName, GetUserName, _getcwd, _chdir
- ✅ **Linux/macOS**: gethostname, getpwuid, getcwd, chdir

---

### 5. Error Handling System (نظام معالجة الأخطاء) ✅

#### رموز الأخطاء:
```cpp
enum class ErrorCode {
    Success,              // نجاح
    PermissionDenied,     // رفض الإذن
    FileNotFound,         // الملف غير موجود
    FileExists,           // الملف موجود
    InvalidArgument,      // معامل غير صحيح
    OutOfMemory,          // نفاذ الذاكرة
    TooManyOpenFiles,     // ملفات مفتوحة كثيرة
    NoSpaceLeft,          // لا مساحة متبقية
    NotDirectory,         // ليس مجلدًا
    IsDirectory,          // إنه مجلد
    NotSupported,         // غير مدعوم
    UnknownError          // خطأ غير معروف
};
```

#### دوال الأخطاء:
```cpp
ErrorCode sys_get_last_error();
std::string sys_error_string(ErrorCode error);
ErrorCode errno_to_error_code(int err);
```

#### الميزات:
- ✅ **Thread-safe**: استخدام `thread_local` لتخزين الأخطاء
- ✅ **Cross-platform**: تحويل تلقائي من errno/GetLastError
- ✅ **Bilingual**: رسائل خطأ بالعربية والإنجليزية

---

### 6. Helper Functions (دوال مساعدة) ✅

```cpp
// قراءة/كتابة ملف كامل
std::vector<uint8_t> sys_read_file(const char* path);
bool sys_write_file(const char* path, const void* data, size_t size);

// فحص الملفات
bool sys_file_exists(const char* path);
int64_t sys_file_size(const char* path);

// ملف مؤقت
FileHandle sys_tmpfile(std::string* path = nullptr);
```

---

## 🏗️ البنية المعمارية / Architecture

### Platform Abstraction Layer
```
┌─────────────────────────────────┐
│   SadLang Runtime API           │
│   (Cross-platform Interface)    │
└─────────────────────────────────┘
            │
    ┌───────┴───────┐
    │               │
    ▼               ▼
┌─────────┐   ┌─────────┐
│ Windows │   │  Unix   │
│  APIs   │   │  APIs   │
└─────────┘   └─────────┘
```

### File Structure
```
runtime/syscall/
├── syscall.h     (404 lines)  - واجهة برمجية عامة
└── syscall.cpp   (1,154 lines) - تنفيذ متعدد المنصات
```

### Compilation Model
```cpp
#ifdef SAD_PLATFORM_WINDOWS
    // Windows implementation
    #include <windows.h>
#else
    // Unix/Linux/macOS implementation
    #include <unistd.h>
    #include <sys/mman.h>
#endif
```

---

## 🧪 أمثلة الاستخدام / Usage Examples

### مثال 1: قراءة وكتابة ملف
```cpp
using namespace Sad::SysCall;

// فتح ملف للقراءة
FileHandle file = sys_open("input.txt", OpenMode::ReadOnly);
if (file == INVALID_FILE) {
    std::cerr << "خطأ: " << sys_error_string(sys_get_last_error()) << std::endl;
    return 1;
}

// قراءة البيانات
char buffer[1024];
int64_t bytes_read = sys_read(file, buffer, sizeof(buffer));

// إغلاق الملف
sys_close(file);

// كتابة إلى ملف جديد
FileHandle out = sys_open("output.txt", 
                          OpenMode::WriteOnly | OpenMode::Create | OpenMode::Truncate,
                          FilePermissions::Default);
sys_write(out, buffer, bytes_read);
sys_close(out);
```

### مثال 2: تعيين الذاكرة
```cpp
// تخصيص 1MB من الذاكرة
void* memory = sys_mmap(
    nullptr,                    // عنوان تلقائي
    1024 * 1024,                // 1MB
    MemoryProtection::ReadWrite, // قراءة وكتابة
    MMapFlags::Private | MMapFlags::Anonymous,
    INVALID_FILE,
    0
);

if (!memory) {
    std::cerr << "فشل تخصيص الذاكرة" << std::endl;
    return 1;
}

// استخدام الذاكرة...

// تحرير الذاكرة
sys_munmap(memory, 1024 * 1024);
```

### مثال 3: إنشاء عملية (Linux/macOS)
```cpp
#ifndef SAD_PLATFORM_WINDOWS

ProcessId pid = sys_fork();

if (pid == 0) {
    // العملية الابن
    std::vector<std::string> args = {"-l", "-a"};
    sys_exec("/bin/ls", args);
} else if (pid > 0) {
    // العملية الأب
    int status;
    sys_wait(pid, &status);
    std::cout << "انتهت العملية الفرعية بحالة: " << status << std::endl;
}

#endif
```

### مثال 4: الحصول على معلومات النظام
```cpp
std::cout << "اسم المضيف: " << sys_gethostname() << std::endl;
std::cout << "المستخدم: " << sys_getusername() << std::endl;
std::cout << "المجلد: " << sys_getcwd() << std::endl;

std::string home = sys_getenv("HOME");
if (!home.empty()) {
    std::cout << "المجلد الرئيسي: " << home << std::endl;
}
```

---

## ⚙️ التكامل مع CMake / CMake Integration

### إضافة للـ CMakeLists.txt:
```cmake
# System Calls Library
add_library(sad_syscall STATIC
    runtime/syscall/syscall.h
    runtime/syscall/syscall.cpp
)

target_include_directories(sad_syscall PUBLIC
    ${CMAKE_SOURCE_DIR}/runtime
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(sad_syscall PRIVATE)
else()
    target_link_libraries(sad_syscall PRIVATE pthread)
endif()

# Link to main runtime
target_link_libraries(sad_runtime PUBLIC sad_syscall)
```

---

## 🔒 الأمان والموثوقية / Security & Reliability

### ميزات الأمان:
1. **Parameter Validation**: فحص جميع المعاملات قبل الاستخدام
2. **NULL Pointer Checks**: التحقق من المؤشرات الفارغة
3. **Bounds Checking**: فحص الحدود للمخازن المؤقتة
4. **Thread Safety**: استخدام `thread_local` للأخطاء

### معالجة الأخطاء:
```cpp
// كل دالة تتحقق من المعاملات
if (handle == INVALID_FILE || !buffer) {
    set_last_error(ErrorCode::InvalidArgument);
    return -1;
}

// تحويل أخطاء النظام تلقائيًا
if (result < 0) {
    set_last_error(errno_to_error_code(errno));
    return -1;
}
```

### RAII Pattern:
```cpp
class FileGuard {
    FileHandle handle_;
public:
    FileGuard(const char* path, OpenMode mode)
        : handle_(sys_open(path, mode)) {}
    
    ~FileGuard() {
        if (handle_ != INVALID_FILE) {
            sys_close(handle_);
        }
    }
    
    FileHandle get() const { return handle_; }
};
```

---

## 📈 الأداء / Performance

### تحسينات الأداء:
1. **Zero-copy Operations**: استخدام مباشر للـ syscalls
2. **Minimal Overhead**: طبقة رقيقة فوق APIs النظام
3. **Platform-specific Optimizations**: تحسينات خاصة بكل منصة

### القياسات المتوقعة:
```
Operation          Time        Overhead
────────────────────────────────────────
sys_open           ~50μs       <5%
sys_read (1KB)     ~2μs        <2%
sys_write (1KB)    ~3μs        <2%
sys_mmap           ~100μs      <5%
sys_fork           ~500μs      <5%
```

---

## ✅ اختبارات مقترحة / Suggested Tests

### 1. File I/O Tests
```cpp
TEST(SysCall, FileOperations) {
    // فتح، قراءة، كتابة، إغلاق
    FileHandle file = sys_open("test.txt", 
                               OpenMode::WriteOnly | OpenMode::Create);
    ASSERT_NE(file, INVALID_FILE);
    
    const char* data = "Hello, World!";
    int64_t written = sys_write(file, data, strlen(data));
    EXPECT_EQ(written, strlen(data));
    
    sys_close(file);
}
```

### 2. Memory Tests
```cpp
TEST(SysCall, MemoryMapping) {
    void* mem = sys_mmap(nullptr, 4096, 
                         MemoryProtection::ReadWrite,
                         MMapFlags::Private | MMapFlags::Anonymous);
    ASSERT_NE(mem, nullptr);
    
    // كتابة وقراءة
    int* data = static_cast<int*>(mem);
    *data = 42;
    EXPECT_EQ(*data, 42);
    
    sys_munmap(mem, 4096);
}
```

### 3. Process Tests (Linux/macOS only)
```cpp
TEST(SysCall, ProcessFork) {
#ifndef SAD_PLATFORM_WINDOWS
    ProcessId pid = sys_fork();
    
    if (pid == 0) {
        // Child process
        sys_exit(123);
    } else {
        // Parent process
        int status;
        sys_wait(pid, &status);
        EXPECT_EQ(WEXITSTATUS(status), 123);
    }
#endif
}
```

---

## 🎯 الإنجازات / Achievements

### ✅ المكتمل:
1. **File I/O System** - 12 دالة، Windows/Linux/macOS
2. **Memory Management** - 5 دوال، mmap/VirtualAlloc
3. **Process Control** - 7 عمليات، fork/exec/wait
4. **System Information** - 6 دوال معلومات
5. **Error Handling** - نظام أخطاء متكامل
6. **Helper Functions** - 5 دوال مساعدة
7. **Cross-platform Support** - Windows, Linux, macOS
8. **Bilingual Documentation** - عربي وإنجليزي

### 📊 الإحصائيات النهائية:
```
syscall.h:      404 سطر
syscall.cpp:  1,154 سطر
━━━━━━━━━━━━━━━━━━━━━
المجموع:     1,558 سطر

Total Phase 5:  5,160 سطر (3,602 + 1,558)
```

---

## 🚀 الخطوات التالية / Next Steps

### Phase 5.4: Memory Management & GC (القادم)
- **Allocator System**: مخصص ذاكرة مخصص
- **Garbage Collector**: Mark & Sweep GC
- **Memory Pools**: تجميع الذاكرة
- **Leak Detection**: كشف التسريبات

### Phase 5.5: Thread Support
- **Thread Management**: إدارة الخيوط
- **Mutex & Locks**: أقفال التزامن
- **Condition Variables**: متغيرات الشرط
- **Thread-local Storage**: تخزين محلي للخيوط

### Phase 5.6: Exception Handling
- **Exception System**: نظام الاستثناءات
- **Stack Unwinding**: فك الـ stack
- **Try/Catch/Finally**: معالجة الاستثناءات

---

## 📝 الخاتمة / Conclusion

**الحمد لله**، تم إنجاز **Phase 5.3: System Calls Wrapper** بنجاح! 🎉

**الإنجازات**:
- ✅ **1,558 سطر** من كود الإنتاج عالي الجودة
- ✅ دعم **3 منصات** (Windows, Linux, macOS)
- ✅ **35 دالة** syscall
- ✅ نظام أخطاء **thread-safe**
- ✅ توثيق **ثنائي اللغة** (عربي/إنجليزي)

**التقدم الكلي للمرحلة 5**:
```
Phase 5.1: C ABI         ████████████████████ 100% (2,485 lines)
Phase 5.2: FFI System    ████████████████████ 100% (1,117 lines)
Phase 5.3: System Calls  ████████████████████ 100% (1,558 lines) ✨
Phase 5.4: Memory & GC   ░░░░░░░░░░░░░░░░░░░░   0%
Phase 5.5: Threads       ░░░░░░░░░░░░░░░░░░░░   0%
Phase 5.6: Exceptions    ░░░░░░░░░░░░░░░░░░░░   0%
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Overall Phase 5          ████████████░░░░░░░░  60% 🚀
```

**استعن بالله للمرحلة القادمة!** 💪

---

**التاريخ**: 2025-12-22  
**الكاتب**: SadLang Compiler Team  
**الحالة**: Phase 5.3 Complete ✅
