/*
 * بسم الله الرحمن الرحيم
 * ================================
 * نظام استدعاءات النظام / System Calls Wrapper
 * ================================
 * 
 * يوفر واجهة موحدة لاستدعاءات النظام عبر المنصات المختلفة
 * Provides unified interface for system calls across platforms
 * 
 * المكونات الأساسية / Core Components:
 * - File I/O operations (open, read, write, close, etc.)
 * - Memory operations (mmap, munmap, mprotect, etc.)
 * - Process operations (fork, exec, wait, etc.)
 * - Platform-specific implementations
 * 
 * @file runtime/syscall/syscall.h
 * @author SadLang Compiler Team
 * @date December 2025
 */

#ifndef SAD_RUNTIME_SYSCALL_H
#define SAD_RUNTIME_SYSCALL_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

// Platform detection / كشف المنصة
#if defined(_WIN32) || defined(_WIN64)
    #define SAD_PLATFORM_WINDOWS
    #include <windows.h>
    #include <io.h>
#else
    #define SAD_PLATFORM_UNIX
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <sys/mman.h>
    #include <sys/wait.h>
#endif

namespace Sad {
namespace SysCall {

// ============================================================================
// Type Definitions / تعريفات الأنواع
// ============================================================================

/**
 * مقبض الملف / File handle
 */
#ifdef SAD_PLATFORM_WINDOWS
    using FileHandle = HANDLE;
    constexpr FileHandle INVALID_FILE = INVALID_HANDLE_VALUE;
#else
    using FileHandle = int;
    constexpr FileHandle INVALID_FILE = -1;
#endif

/**
 * معرف العملية / Process ID
 */
#ifdef SAD_PLATFORM_WINDOWS
    using ProcessId = DWORD;
#else
    using ProcessId = pid_t;
#endif

/**
 * أوضاع فتح الملفات / File open modes
 */
enum class OpenMode : uint32_t {
    ReadOnly    = 0x01,     // قراءة فقط
    WriteOnly   = 0x02,     // كتابة فقط
    ReadWrite   = 0x03,     // قراءة وكتابة
    Create      = 0x04,     // إنشاء إذا غير موجود
    Truncate    = 0x08,     // قص الملف
    Append      = 0x10,     // الإضافة للنهاية
    Exclusive   = 0x20      // فشل إذا موجود
};

/**
 * أذونات الملفات / File permissions
 */
enum class FilePermissions : uint32_t {
    None        = 0000,
    OwnerRead   = 0400,
    OwnerWrite  = 0200,
    OwnerExec   = 0100,
    GroupRead   = 0040,
    GroupWrite  = 0020,
    GroupExec   = 0010,
    OtherRead   = 0004,
    OtherWrite  = 0002,
    OtherExec   = 0001,
    Default     = 0644      // rw-r--r--
};

/**
 * أوضاع الـ seek / Seek modes
 */
enum class SeekMode {
    Set,        // من البداية / From beginning
    Current,    // من الموقع الحالي / From current
    End         // من النهاية / From end
};

/**
 * معلومات الملف / File information
 */
struct FileInfo {
    uint64_t size;          // حجم الملف
    uint64_t inode;         // رقم inode
    uint32_t mode;          // الأذونات
    uint32_t uid;           // معرف المالك
    uint32_t gid;           // معرف المجموعة
    int64_t atime;          // وقت آخر وصول
    int64_t mtime;          // وقت آخر تعديل
    int64_t ctime;          // وقت تغيير البيانات الوصفية
    bool is_file;           // هل ملف؟
    bool is_directory;      // هل مجلد؟
    bool is_symlink;        // هل رابط رمزي؟
};

/**
 * أوضاع الذاكرة / Memory protection modes
 */
enum class MemoryProtection : uint32_t {
    None        = 0x00,
    Read        = 0x01,
    Write       = 0x02,
    Execute     = 0x04,
    ReadWrite   = 0x03,
    ReadExec    = 0x05,
    All         = 0x07
};

/**
 * خيارات mmap / mmap flags
 */
enum class MMapFlags : uint32_t {
    Private     = 0x01,     // خاص بالعملية
    Shared      = 0x02,     // مشترك
    Fixed       = 0x04,     // عنوان ثابت
    Anonymous   = 0x08      // بدون ملف
};

// ============================================================================
// File I/O Operations / عمليات الملفات
// ============================================================================

/**
 * فتح ملف / Open file
 * 
 * @param path مسار الملف
 * @param mode وضع الفتح
 * @param perms الأذونات (للإنشاء)
 * @return مقبض الملف أو INVALID_FILE عند الفشل
 */
FileHandle sys_open(
    const char* path,
    OpenMode mode,
    FilePermissions perms = FilePermissions::Default
);

/**
 * إغلاق ملف / Close file
 */
bool sys_close(FileHandle handle);

/**
 * قراءة من ملف / Read from file
 * 
 * @param handle مقبض الملف
 * @param buffer المخزن المؤقت
 * @param count عدد البايتات للقراءة
 * @return عدد البايتات المقروءة، أو -1 عند الفشل
 */
int64_t sys_read(FileHandle handle, void* buffer, size_t count);

/**
 * كتابة إلى ملف / Write to file
 * 
 * @return عدد البايتات المكتوبة، أو -1 عند الفشل
 */
int64_t sys_write(FileHandle handle, const void* buffer, size_t count);

/**
 * تحريك مؤشر الملف / Seek in file
 * 
 * @param handle مقبض الملف
 * @param offset الإزاحة
 * @param mode وضع الـ seek
 * @return الموقع الجديد، أو -1 عند الفشل
 */
int64_t sys_seek(FileHandle handle, int64_t offset, SeekMode mode);

/**
 * الحصول على معلومات الملف / Get file information
 */
bool sys_stat(const char* path, FileInfo* info);
bool sys_fstat(FileHandle handle, FileInfo* info);

/**
 * حذف ملف / Delete file
 */
bool sys_unlink(const char* path);

/**
 * إنشاء مجلد / Create directory
 */
bool sys_mkdir(const char* path, FilePermissions perms = FilePermissions::Default);

/**
 * حذف مجلد / Remove directory
 */
bool sys_rmdir(const char* path);

/**
 * إعادة تسمية / Rename
 */
bool sys_rename(const char* old_path, const char* new_path);

// ============================================================================
// Memory Operations / عمليات الذاكرة
// ============================================================================

/**
 * تعيين ذاكرة / Map memory
 * 
 * @param addr العنوان المفضل (nullptr للتلقائي)
 * @param length الحجم
 * @param prot الحماية
 * @param flags الخيارات
 * @param fd مقبض الملف (للملفات)
 * @param offset الإزاحة في الملف
 * @return عنوان الذاكرة أو nullptr عند الفشل
 */
void* sys_mmap(
    void* addr,
    size_t length,
    MemoryProtection prot,
    MMapFlags flags,
    FileHandle fd = INVALID_FILE,
    int64_t offset = 0
);

/**
 * إلغاء تعيين الذاكرة / Unmap memory
 */
bool sys_munmap(void* addr, size_t length);

/**
 * تغيير حماية الذاكرة / Change memory protection
 */
bool sys_mprotect(void* addr, size_t length, MemoryProtection prot);

/**
 * تخصيص ذاكرة heap / Allocate heap memory
 */
void* sys_brk(void* addr);
void* sys_sbrk(intptr_t increment);

// ============================================================================
// Process Operations / عمليات العمليات
// ============================================================================

/**
 * إنشاء عملية فرعية / Fork process
 * 
 * @return معرف العملية الفرعية (في الأب)، 0 (في الابن)، -1 (فشل)
 */
ProcessId sys_fork();

/**
 * تنفيذ برنامج / Execute program
 * 
 * @param path مسار البرنامج
 * @param args المعاملات
 * @param env المتغيرات البيئية
 * @return لا يعود عند النجاح، -1 عند الفشل
 */
int sys_exec(
    const char* path,
    const std::vector<std::string>& args,
    const std::vector<std::string>& env = {}
);

/**
 * انتظار عملية فرعية / Wait for child process
 * 
 * @param pid معرف العملية (-1 لأي عملية)
 * @param status حالة الخروج
 * @return معرف العملية التي انتهت، أو -1 عند الفشل
 */
ProcessId sys_wait(ProcessId pid, int* status = nullptr);

/**
 * إنهاء العملية / Exit process
 */
[[noreturn]] void sys_exit(int code);

/**
 * الحصول على معرف العملية / Get process ID
 */
ProcessId sys_getpid();

/**
 * الحصول على معرف العملية الأب / Get parent process ID
 */
ProcessId sys_getppid();

/**
 * إرسال إشارة لعملية / Send signal to process
 */
bool sys_kill(ProcessId pid, int signal);

// ============================================================================
// System Information / معلومات النظام
// ============================================================================

/**
 * الحصول على اسم المضيف / Get hostname
 */
std::string sys_gethostname();

/**
 * الحصول على اسم المستخدم / Get username
 */
std::string sys_getusername();

/**
 * الحصول على المجلد الحالي / Get current directory
 */
std::string sys_getcwd();

/**
 * تغيير المجلد الحالي / Change current directory
 */
bool sys_chdir(const char* path);

/**
 * الحصول على متغير بيئي / Get environment variable
 */
std::string sys_getenv(const char* name);

/**
 * تعيين متغير بيئي / Set environment variable
 */
bool sys_setenv(const char* name, const char* value, bool overwrite = true);

// ============================================================================
// Error Handling / معالجة الأخطاء
// ============================================================================

/**
 * رموز الأخطاء / Error codes
 */
enum class ErrorCode {
    Success,
    PermissionDenied,
    FileNotFound,
    FileExists,
    InvalidArgument,
    OutOfMemory,
    TooManyOpenFiles,
    NoSpaceLeft,
    NotDirectory,
    IsDirectory,
    NotSupported,
    UnknownError
};

/**
 * الحصول على آخر خطأ / Get last error
 */
ErrorCode sys_get_last_error();

/**
 * الحصول على رسالة الخطأ / Get error message
 */
std::string sys_error_string(ErrorCode error);

/**
 * تحويل errno إلى ErrorCode / Convert errno to ErrorCode
 */
ErrorCode errno_to_error_code(int err);

// ============================================================================
// Helper Functions / دوال مساعدة
// ============================================================================

/**
 * قراءة ملف كامل / Read entire file
 */
std::vector<uint8_t> sys_read_file(const char* path);

/**
 * كتابة ملف كامل / Write entire file
 */
bool sys_write_file(const char* path, const void* data, size_t size);

/**
 * التحقق من وجود ملف / Check if file exists
 */
bool sys_file_exists(const char* path);

/**
 * الحصول على حجم ملف / Get file size
 */
int64_t sys_file_size(const char* path);

/**
 * إنشاء ملف مؤقت / Create temporary file
 */
FileHandle sys_tmpfile(std::string* path = nullptr);

// ============================================================================
// Operator Overloads / تحميل المعاملات
// ============================================================================

/**
 * دمج أوضاع الفتح / Combine open modes
 */
inline OpenMode operator|(OpenMode a, OpenMode b) {
    return static_cast<OpenMode>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

inline OpenMode& operator|=(OpenMode& a, OpenMode b) {
    return a = a | b;
}

/**
 * دمج أذونات الملفات / Combine file permissions
 */
inline FilePermissions operator|(FilePermissions a, FilePermissions b) {
    return static_cast<FilePermissions>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

/**
 * دمج حماية الذاكرة / Combine memory protection
 */
inline MemoryProtection operator|(MemoryProtection a, MemoryProtection b) {
    return static_cast<MemoryProtection>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

/**
 * دمج خيارات mmap / Combine mmap flags
 */
inline MMapFlags operator|(MMapFlags a, MMapFlags b) {
    return static_cast<MMapFlags>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

} // namespace SysCall
} // namespace Sad

#endif // SAD_RUNTIME_SYSCALL_H
