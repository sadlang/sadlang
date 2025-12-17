/*
 * بسم الله الرحمن الرحيم
 * ================================
 * تنفيذ نظام استدعاءات النظام / System Calls Implementation
 * ================================
 */

#include "syscall.h"
#include <cstring>
#include <cerrno>
#include <thread>

#ifdef SAD_PLATFORM_WINDOWS
    #include <direct.h>
    #include <process.h>
#else
    #include <sys/mman.h>
    #include <pwd.h>
#endif

namespace Sad {
namespace SysCall {

// ============================================================================
// Thread-local error storage / تخزين الأخطاء لكل thread
// ============================================================================

thread_local ErrorCode g_last_error = ErrorCode::Success;

void set_last_error(ErrorCode error) {
    g_last_error = error;
}

ErrorCode sys_get_last_error() {
    return g_last_error;
}

// ============================================================================
// Error Conversion / تحويل الأخطاء
// ============================================================================

ErrorCode errno_to_error_code(int err) {
    switch (err) {
        case 0: return ErrorCode::Success;
#ifdef _WIN32
        case ERROR_ACCESS_DENIED: return ErrorCode::PermissionDenied;
        case ERROR_FILE_NOT_FOUND: return ErrorCode::FileNotFound;
        case ERROR_PATH_NOT_FOUND: return ErrorCode::FileNotFound;
        case ERROR_FILE_EXISTS: return ErrorCode::FileExists;
        case ERROR_INVALID_PARAMETER: return ErrorCode::InvalidArgument;
        case ERROR_NOT_ENOUGH_MEMORY: return ErrorCode::OutOfMemory;
        case ERROR_TOO_MANY_OPEN_FILES: return ErrorCode::TooManyOpenFiles;
        case ERROR_DISK_FULL: return ErrorCode::NoSpaceLeft;
        case ERROR_DIRECTORY: return ErrorCode::NotDirectory;
        case ERROR_NOT_SUPPORTED: return ErrorCode::NotSupported;
#else
        case EACCES: return ErrorCode::PermissionDenied;
        case EPERM: return ErrorCode::PermissionDenied;
        case ENOENT: return ErrorCode::FileNotFound;
        case EEXIST: return ErrorCode::FileExists;
        case EINVAL: return ErrorCode::InvalidArgument;
        case ENOMEM: return ErrorCode::OutOfMemory;
        case EMFILE: return ErrorCode::TooManyOpenFiles;
        case ENFILE: return ErrorCode::TooManyOpenFiles;
        case ENOSPC: return ErrorCode::NoSpaceLeft;
        case ENOTDIR: return ErrorCode::NotDirectory;
        case EISDIR: return ErrorCode::IsDirectory;
        case ENOTSUP: return ErrorCode::NotSupported;
#endif
        default: return ErrorCode::UnknownError;
    }
}

std::string sys_error_string(ErrorCode error) {
    switch (error) {
        case ErrorCode::Success: return "Success / نجاح";
        case ErrorCode::PermissionDenied: return "Permission denied / رفض الإذن";
        case ErrorCode::FileNotFound: return "File not found / الملف غير موجود";
        case ErrorCode::FileExists: return "File exists / الملف موجود";
        case ErrorCode::InvalidArgument: return "Invalid argument / معامل غير صحيح";
        case ErrorCode::OutOfMemory: return "Out of memory / نفاذ الذاكرة";
        case ErrorCode::TooManyOpenFiles: return "Too many open files / ملفات مفتوحة كثيرة";
        case ErrorCode::NoSpaceLeft: return "No space left / لا مساحة متبقية";
        case ErrorCode::NotDirectory: return "Not a directory / ليس مجلدًا";
        case ErrorCode::IsDirectory: return "Is a directory / إنه مجلد";
        case ErrorCode::NotSupported: return "Not supported / غير مدعوم";
        default: return "Unknown error / خطأ غير معروف";
    }
}

// ============================================================================
// File I/O Implementation / تنفيذ عمليات الملفات
// ============================================================================

#ifdef SAD_PLATFORM_WINDOWS

FileHandle sys_open(const char* path, OpenMode mode, FilePermissions perms) {
    DWORD access = 0;
    DWORD creation = 0;
    
    // Convert access mode / تحويل وضع الوصول
    if ((static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::ReadWrite)) == 
        static_cast<uint32_t>(OpenMode::ReadWrite)) {
        access = GENERIC_READ | GENERIC_WRITE;
    } else if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::ReadOnly)) {
        access = GENERIC_READ;
    } else if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::WriteOnly)) {
        access = GENERIC_WRITE;
    }
    
    // Convert creation mode / تحويل وضع الإنشاء
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::Create)) {
        if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::Exclusive)) {
            creation = CREATE_NEW;
        } else if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::Truncate)) {
            creation = CREATE_ALWAYS;
        } else {
            creation = OPEN_ALWAYS;
        }
    } else {
        if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::Truncate)) {
            creation = TRUNCATE_EXISTING;
        } else {
            creation = OPEN_EXISTING;
        }
    }
    
    HANDLE handle = CreateFileA(
        path,
        access,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        creation,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    
    if (handle == INVALID_HANDLE_VALUE) {
        set_last_error(errno_to_error_code(GetLastError()));
        return INVALID_FILE;
    }
    
    // Handle append mode / معالجة وضع الإضافة
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::Append)) {
        SetFilePointer(handle, 0, nullptr, FILE_END);
    }
    
    set_last_error(ErrorCode::Success);
    return handle;
}

bool sys_close(FileHandle handle) {
    if (handle == INVALID_FILE) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (CloseHandle(handle)) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return false;
}

int64_t sys_read(FileHandle handle, void* buffer, size_t count) {
    if (handle == INVALID_FILE || !buffer) {
        set_last_error(ErrorCode::InvalidArgument);
        return -1;
    }
    
    DWORD bytes_read = 0;
    if (ReadFile(handle, buffer, static_cast<DWORD>(count), &bytes_read, nullptr)) {
        set_last_error(ErrorCode::Success);
        return bytes_read;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return -1;
}

int64_t sys_write(FileHandle handle, const void* buffer, size_t count) {
    if (handle == INVALID_FILE || !buffer) {
        set_last_error(ErrorCode::InvalidArgument);
        return -1;
    }
    
    DWORD bytes_written = 0;
    if (WriteFile(handle, buffer, static_cast<DWORD>(count), &bytes_written, nullptr)) {
        set_last_error(ErrorCode::Success);
        return bytes_written;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return -1;
}

int64_t sys_seek(FileHandle handle, int64_t offset, SeekMode mode) {
    if (handle == INVALID_FILE) {
        set_last_error(ErrorCode::InvalidArgument);
        return -1;
    }
    
    DWORD method;
    switch (mode) {
        case SeekMode::Set: method = FILE_BEGIN; break;
        case SeekMode::Current: method = FILE_CURRENT; break;
        case SeekMode::End: method = FILE_END; break;
        default:
            set_last_error(ErrorCode::InvalidArgument);
            return -1;
    }
    
    LARGE_INTEGER li;
    li.QuadPart = offset;
    li.LowPart = SetFilePointer(handle, li.LowPart, &li.HighPart, method);
    
    if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        set_last_error(errno_to_error_code(GetLastError()));
        return -1;
    }
    
    set_last_error(ErrorCode::Success);
    return li.QuadPart;
}

bool sys_stat(const char* path, FileInfo* info) {
    if (!path || !info) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    WIN32_FILE_ATTRIBUTE_DATA attrs;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &attrs)) {
        set_last_error(errno_to_error_code(GetLastError()));
        return false;
    }
    
    LARGE_INTEGER size;
    size.LowPart = attrs.nFileSizeLow;
    size.HighPart = attrs.nFileSizeHigh;
    
    info->size = size.QuadPart;
    info->is_file = !(attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    info->is_directory = attrs.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    info->is_symlink = attrs.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT;
    
    // Convert FILETIME to Unix timestamp
    auto filetime_to_unix = [](const FILETIME& ft) -> int64_t {
        LARGE_INTEGER li;
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        return (li.QuadPart - 116444736000000000LL) / 10000000LL;
    };
    
    info->atime = filetime_to_unix(attrs.ftLastAccessTime);
    info->mtime = filetime_to_unix(attrs.ftLastWriteTime);
    info->ctime = filetime_to_unix(attrs.ftCreationTime);
    
    set_last_error(ErrorCode::Success);
    return true;
}

bool sys_fstat(FileHandle handle, FileInfo* info) {
    if (handle == INVALID_FILE || !info) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    BY_HANDLE_FILE_INFORMATION file_info;
    if (!GetFileInformationByHandle(handle, &file_info)) {
        set_last_error(errno_to_error_code(GetLastError()));
        return false;
    }
    
    LARGE_INTEGER size;
    size.LowPart = file_info.nFileSizeLow;
    size.HighPart = file_info.nFileSizeHigh;
    
    info->size = size.QuadPart;
    info->is_file = !(file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
    info->is_directory = file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    info->is_symlink = file_info.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT;
    
    set_last_error(ErrorCode::Success);
    return true;
}

bool sys_unlink(const char* path) {
    if (!path) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (DeleteFileA(path)) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return false;
}

bool sys_mkdir(const char* path, FilePermissions perms) {
    if (!path) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (CreateDirectoryA(path, nullptr)) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return false;
}

bool sys_rmdir(const char* path) {
    if (!path) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (RemoveDirectoryA(path)) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return false;
}

bool sys_rename(const char* old_path, const char* new_path) {
    if (!old_path || !new_path) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (MoveFileA(old_path, new_path)) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return false;
}

#else // Unix/Linux/macOS

FileHandle sys_open(const char* path, OpenMode mode, FilePermissions perms) {
    int flags = 0;
    
    // Convert access mode / تحويل وضع الوصول
    if ((static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::ReadWrite)) == 
        static_cast<uint32_t>(OpenMode::ReadWrite)) {
        flags = O_RDWR;
    } else if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::ReadOnly)) {
        flags = O_RDONLY;
    } else if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::WriteOnly)) {
        flags = O_WRONLY;
    }
    
    // Convert creation flags / تحويل أعلام الإنشاء
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::Create)) {
        flags |= O_CREAT;
    }
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::Truncate)) {
        flags |= O_TRUNC;
    }
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::Append)) {
        flags |= O_APPEND;
    }
    if (static_cast<uint32_t>(mode) & static_cast<uint32_t>(OpenMode::Exclusive)) {
        flags |= O_EXCL;
    }
    
    int fd = ::open(path, flags, static_cast<mode_t>(perms));
    
    if (fd < 0) {
        set_last_error(errno_to_error_code(errno));
        return INVALID_FILE;
    }
    
    set_last_error(ErrorCode::Success);
    return fd;
}

bool sys_close(FileHandle handle) {
    if (handle == INVALID_FILE) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (::close(handle) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(errno));
    return false;
}

int64_t sys_read(FileHandle handle, void* buffer, size_t count) {
    if (handle == INVALID_FILE || !buffer) {
        set_last_error(ErrorCode::InvalidArgument);
        return -1;
    }
    
    ssize_t result = ::read(handle, buffer, count);
    
    if (result < 0) {
        set_last_error(errno_to_error_code(errno));
        return -1;
    }
    
    set_last_error(ErrorCode::Success);
    return result;
}

int64_t sys_write(FileHandle handle, const void* buffer, size_t count) {
    if (handle == INVALID_FILE || !buffer) {
        set_last_error(ErrorCode::InvalidArgument);
        return -1;
    }
    
    ssize_t result = ::write(handle, buffer, count);
    
    if (result < 0) {
        set_last_error(errno_to_error_code(errno));
        return -1;
    }
    
    set_last_error(ErrorCode::Success);
    return result;
}

int64_t sys_seek(FileHandle handle, int64_t offset, SeekMode mode) {
    if (handle == INVALID_FILE) {
        set_last_error(ErrorCode::InvalidArgument);
        return -1;
    }
    
    int whence;
    switch (mode) {
        case SeekMode::Set: whence = SEEK_SET; break;
        case SeekMode::Current: whence = SEEK_CUR; break;
        case SeekMode::End: whence = SEEK_END; break;
        default:
            set_last_error(ErrorCode::InvalidArgument);
            return -1;
    }
    
    off_t result = ::lseek(handle, offset, whence);
    
    if (result < 0) {
        set_last_error(errno_to_error_code(errno));
        return -1;
    }
    
    set_last_error(ErrorCode::Success);
    return result;
}

bool sys_stat(const char* path, FileInfo* info) {
    if (!path || !info) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    struct stat st;
    if (::stat(path, &st) != 0) {
        set_last_error(errno_to_error_code(errno));
        return false;
    }
    
    info->size = st.st_size;
    info->inode = st.st_ino;
    info->mode = st.st_mode;
    info->uid = st.st_uid;
    info->gid = st.st_gid;
    info->atime = st.st_atime;
    info->mtime = st.st_mtime;
    info->ctime = st.st_ctime;
    info->is_file = S_ISREG(st.st_mode);
    info->is_directory = S_ISDIR(st.st_mode);
    info->is_symlink = S_ISLNK(st.st_mode);
    
    set_last_error(ErrorCode::Success);
    return true;
}

bool sys_fstat(FileHandle handle, FileInfo* info) {
    if (handle == INVALID_FILE || !info) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    struct stat st;
    if (::fstat(handle, &st) != 0) {
        set_last_error(errno_to_error_code(errno));
        return false;
    }
    
    info->size = st.st_size;
    info->inode = st.st_ino;
    info->mode = st.st_mode;
    info->uid = st.st_uid;
    info->gid = st.st_gid;
    info->atime = st.st_atime;
    info->mtime = st.st_mtime;
    info->ctime = st.st_ctime;
    info->is_file = S_ISREG(st.st_mode);
    info->is_directory = S_ISDIR(st.st_mode);
    info->is_symlink = S_ISLNK(st.st_mode);
    
    set_last_error(ErrorCode::Success);
    return true;
}

bool sys_unlink(const char* path) {
    if (!path) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (::unlink(path) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(errno));
    return false;
}

bool sys_mkdir(const char* path, FilePermissions perms) {
    if (!path) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (::mkdir(path, static_cast<mode_t>(perms)) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(errno));
    return false;
}

bool sys_rmdir(const char* path) {
    if (!path) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (::rmdir(path) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(errno));
    return false;
}

bool sys_rename(const char* old_path, const char* new_path) {
    if (!old_path || !new_path) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (::rename(old_path, new_path) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(errno));
    return false;
}

#endif // Platform-specific file operations

// ============================================================================
// Memory Operations Implementation / تنفيذ عمليات الذاكرة
// ============================================================================

#ifdef SAD_PLATFORM_WINDOWS

void* sys_mmap(void* addr, size_t length, MemoryProtection prot, 
               MMapFlags flags, FileHandle fd, int64_t offset) {
    DWORD protect = PAGE_NOACCESS;
    
    // Convert protection / تحويل الحماية
    uint32_t prot_val = static_cast<uint32_t>(prot);
    if (prot_val & static_cast<uint32_t>(MemoryProtection::Execute)) {
        if (prot_val & static_cast<uint32_t>(MemoryProtection::Write)) {
            protect = PAGE_EXECUTE_READWRITE;
        } else if (prot_val & static_cast<uint32_t>(MemoryProtection::Read)) {
            protect = PAGE_EXECUTE_READ;
        } else {
            protect = PAGE_EXECUTE;
        }
    } else if (prot_val & static_cast<uint32_t>(MemoryProtection::Write)) {
        protect = PAGE_READWRITE;
    } else if (prot_val & static_cast<uint32_t>(MemoryProtection::Read)) {
        protect = PAGE_READONLY;
    }
    
    HANDLE mapping = nullptr;
    void* result = nullptr;
    
    if (fd != INVALID_FILE) {
        // File mapping / تعيين ملف
        DWORD access = FILE_MAP_READ;
        if (prot_val & static_cast<uint32_t>(MemoryProtection::Write)) {
            access = FILE_MAP_WRITE;
        }
        if (prot_val & static_cast<uint32_t>(MemoryProtection::Execute)) {
            access |= FILE_MAP_EXECUTE;
        }
        
        mapping = CreateFileMapping(fd, nullptr, protect, 0, 0, nullptr);
        if (!mapping) {
            set_last_error(errno_to_error_code(GetLastError()));
            return nullptr;
        }
        
        result = MapViewOfFile(mapping, access, 
                              static_cast<DWORD>(offset >> 32),
                              static_cast<DWORD>(offset & 0xFFFFFFFF),
                              length);
        CloseHandle(mapping);
    } else {
        // Anonymous mapping / تعيين مجهول
        result = VirtualAlloc(addr, length, MEM_COMMIT | MEM_RESERVE, protect);
    }
    
    if (!result) {
        set_last_error(errno_to_error_code(GetLastError()));
        return nullptr;
    }
    
    set_last_error(ErrorCode::Success);
    return result;
}

bool sys_munmap(void* addr, size_t length) {
    if (!addr) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (VirtualFree(addr, 0, MEM_RELEASE)) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return false;
}

bool sys_mprotect(void* addr, size_t length, MemoryProtection prot) {
    if (!addr) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    DWORD protect = PAGE_NOACCESS;
    uint32_t prot_val = static_cast<uint32_t>(prot);
    
    if (prot_val & static_cast<uint32_t>(MemoryProtection::Execute)) {
        if (prot_val & static_cast<uint32_t>(MemoryProtection::Write)) {
            protect = PAGE_EXECUTE_READWRITE;
        } else if (prot_val & static_cast<uint32_t>(MemoryProtection::Read)) {
            protect = PAGE_EXECUTE_READ;
        } else {
            protect = PAGE_EXECUTE;
        }
    } else if (prot_val & static_cast<uint32_t>(MemoryProtection::Write)) {
        protect = PAGE_READWRITE;
    } else if (prot_val & static_cast<uint32_t>(MemoryProtection::Read)) {
        protect = PAGE_READONLY;
    }
    
    DWORD old_protect;
    if (VirtualProtect(addr, length, protect, &old_protect)) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return false;
}

void* sys_brk(void* addr) {
    // Windows doesn't have brk, use VirtualAlloc instead
    set_last_error(ErrorCode::NotSupported);
    return nullptr;
}

void* sys_sbrk(intptr_t increment) {
    // Windows doesn't have sbrk
    set_last_error(ErrorCode::NotSupported);
    return nullptr;
}

#else // Unix/Linux/macOS

void* sys_mmap(void* addr, size_t length, MemoryProtection prot, 
               MMapFlags flags, FileHandle fd, int64_t offset) {
    int posix_prot = PROT_NONE;
    int posix_flags = 0;
    
    // Convert protection / تحويل الحماية
    uint32_t prot_val = static_cast<uint32_t>(prot);
    if (prot_val & static_cast<uint32_t>(MemoryProtection::Read)) {
        posix_prot |= PROT_READ;
    }
    if (prot_val & static_cast<uint32_t>(MemoryProtection::Write)) {
        posix_prot |= PROT_WRITE;
    }
    if (prot_val & static_cast<uint32_t>(MemoryProtection::Execute)) {
        posix_prot |= PROT_EXEC;
    }
    
    // Convert flags / تحويل الأعلام
    uint32_t flags_val = static_cast<uint32_t>(flags);
    if (flags_val & static_cast<uint32_t>(MMapFlags::Shared)) {
        posix_flags |= MAP_SHARED;
    }
    if (flags_val & static_cast<uint32_t>(MMapFlags::Private)) {
        posix_flags |= MAP_PRIVATE;
    }
    if (flags_val & static_cast<uint32_t>(MMapFlags::Fixed)) {
        posix_flags |= MAP_FIXED;
    }
    if (flags_val & static_cast<uint32_t>(MMapFlags::Anonymous)) {
        posix_flags |= MAP_ANONYMOUS;
        fd = -1;
    }
    
    void* result = ::mmap(addr, length, posix_prot, posix_flags, fd, offset);
    
    if (result == MAP_FAILED) {
        set_last_error(errno_to_error_code(errno));
        return nullptr;
    }
    
    set_last_error(ErrorCode::Success);
    return result;
}

bool sys_munmap(void* addr, size_t length) {
    if (!addr) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    if (::munmap(addr, length) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(errno));
    return false;
}

bool sys_mprotect(void* addr, size_t length, MemoryProtection prot) {
    if (!addr) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
    int posix_prot = PROT_NONE;
    uint32_t prot_val = static_cast<uint32_t>(prot);
    
    if (prot_val & static_cast<uint32_t>(MemoryProtection::Read)) {
        posix_prot |= PROT_READ;
    }
    if (prot_val & static_cast<uint32_t>(MemoryProtection::Write)) {
        posix_prot |= PROT_WRITE;
    }
    if (prot_val & static_cast<uint32_t>(MemoryProtection::Execute)) {
        posix_prot |= PROT_EXEC;
    }
    
    if (::mprotect(addr, length, posix_prot) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(errno));
    return false;
}

void* sys_brk(void* addr) {
    void* result = ::brk(addr);
    if (result == (void*)-1) {
        set_last_error(errno_to_error_code(errno));
        return nullptr;
    }
    set_last_error(ErrorCode::Success);
    return result;
}

void* sys_sbrk(intptr_t increment) {
    void* result = ::sbrk(increment);
    if (result == (void*)-1) {
        set_last_error(errno_to_error_code(errno));
        return nullptr;
    }
    set_last_error(ErrorCode::Success);
    return result;
}

#endif // Platform-specific memory operations

// ============================================================================
// Process Operations Implementation / تنفيذ عمليات العمليات
// ============================================================================

#ifdef SAD_PLATFORM_WINDOWS

ProcessId sys_fork() {
    // Windows doesn't support fork
    set_last_error(ErrorCode::NotSupported);
    return -1;
}

int sys_exec(const char* path, const std::vector<std::string>& args,
             const std::vector<std::string>& env) {
    // Build command line / بناء سطر الأوامر
    std::string cmdline = path;
    for (const auto& arg : args) {
        cmdline += " \"" + arg + "\"";
    }
    
    // Build environment block / بناء كتلة البيئة
    std::string env_block;
    if (!env.empty()) {
        for (const auto& e : env) {
            env_block += e + '\0';
        }
        env_block += '\0';
    }
    
    STARTUPINFOA si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};
    
    BOOL success = CreateProcessA(
        path,
        const_cast<char*>(cmdline.c_str()),
        nullptr,
        nullptr,
        FALSE,
        0,
        env_block.empty() ? nullptr : const_cast<char*>(env_block.c_str()),
        nullptr,
        &si,
        &pi
    );
    
    if (!success) {
        set_last_error(errno_to_error_code(GetLastError()));
        return -1;
    }
    
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    
    set_last_error(ErrorCode::Success);
    return 0;
}

ProcessId sys_wait(ProcessId pid, int* status) {
    HANDLE process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!process) {
        set_last_error(errno_to_error_code(GetLastError()));
        return -1;
    }
    
    WaitForSingleObject(process, INFINITE);
    
    if (status) {
        DWORD exit_code;
        GetExitCodeProcess(process, &exit_code);
        *status = static_cast<int>(exit_code);
    }
    
    CloseHandle(process);
    set_last_error(ErrorCode::Success);
    return pid;
}

void sys_exit(int code) {
    ExitProcess(static_cast<UINT>(code));
}

ProcessId sys_getpid() {
    return GetCurrentProcessId();
}

ProcessId sys_getppid() {
    // Windows doesn't have direct getppid
    set_last_error(ErrorCode::NotSupported);
    return -1;
}

bool sys_kill(ProcessId pid, int signal) {
    HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!process) {
        set_last_error(errno_to_error_code(GetLastError()));
        return false;
    }
    
    BOOL success = TerminateProcess(process, static_cast<UINT>(signal));
    CloseHandle(process);
    
    if (success) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(GetLastError()));
    return false;
}

#else // Unix/Linux/macOS

ProcessId sys_fork() {
    pid_t pid = ::fork();
    
    if (pid < 0) {
        set_last_error(errno_to_error_code(errno));
        return -1;
    }
    
    set_last_error(ErrorCode::Success);
    return pid;
}

int sys_exec(const char* path, const std::vector<std::string>& args,
             const std::vector<std::string>& env) {
    // Build argv / بناء argv
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(path));
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    
    // Build envp / بناء envp
    std::vector<char*> envp;
    if (!env.empty()) {
        for (const auto& e : env) {
            envp.push_back(const_cast<char*>(e.c_str()));
        }
        envp.push_back(nullptr);
    }
    
    int result;
    if (envp.empty()) {
        result = ::execv(path, argv.data());
    } else {
        result = ::execve(path, argv.data(), envp.data());
    }
    
    // If we reach here, exec failed
    set_last_error(errno_to_error_code(errno));
    return result;
}

ProcessId sys_wait(ProcessId pid, int* status) {
    pid_t result = ::waitpid(pid, status, 0);
    
    if (result < 0) {
        set_last_error(errno_to_error_code(errno));
        return -1;
    }
    
    set_last_error(ErrorCode::Success);
    return result;
}

void sys_exit(int code) {
    ::exit(code);
}

ProcessId sys_getpid() {
    return ::getpid();
}

ProcessId sys_getppid() {
    return ::getppid();
}

bool sys_kill(ProcessId pid, int signal) {
    if (::kill(pid, signal) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    
    set_last_error(errno_to_error_code(errno));
    return false;
}

#endif // Platform-specific process operations

// ============================================================================
// System Information Implementation / تنفيذ معلومات النظام
// ============================================================================

std::string sys_gethostname() {
    char buffer[256];
    
#ifdef SAD_PLATFORM_WINDOWS
    DWORD size = sizeof(buffer);
    if (GetComputerNameA(buffer, &size)) {
        set_last_error(ErrorCode::Success);
        return std::string(buffer);
    }
#else
    if (::gethostname(buffer, sizeof(buffer)) == 0) {
        set_last_error(ErrorCode::Success);
        return std::string(buffer);
    }
#endif
    
    set_last_error(ErrorCode::UnknownError);
    return "";
}

std::string sys_getusername() {
#ifdef SAD_PLATFORM_WINDOWS
    char buffer[256];
    DWORD size = sizeof(buffer);
    if (GetUserNameA(buffer, &size)) {
        set_last_error(ErrorCode::Success);
        return std::string(buffer);
    }
    set_last_error(errno_to_error_code(GetLastError()));
    return "";
#else
    struct passwd* pw = getpwuid(getuid());
    if (pw) {
        set_last_error(ErrorCode::Success);
        return std::string(pw->pw_name);
    }
    set_last_error(ErrorCode::UnknownError);
    return "";
#endif
}

std::string sys_getcwd() {
    char buffer[4096];
    
#ifdef SAD_PLATFORM_WINDOWS
    if (_getcwd(buffer, sizeof(buffer))) {
        set_last_error(ErrorCode::Success);
        return std::string(buffer);
    }
#else
    if (::getcwd(buffer, sizeof(buffer))) {
        set_last_error(ErrorCode::Success);
        return std::string(buffer);
    }
#endif
    
    set_last_error(errno_to_error_code(errno));
    return "";
}

bool sys_chdir(const char* path) {
    if (!path) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
#ifdef SAD_PLATFORM_WINDOWS
    if (_chdir(path) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
#else
    if (::chdir(path) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
#endif
    
    set_last_error(errno_to_error_code(errno));
    return false;
}

std::string sys_getenv(const char* name) {
    if (!name) {
        set_last_error(ErrorCode::InvalidArgument);
        return "";
    }
    
#ifdef SAD_PLATFORM_WINDOWS
    char buffer[32767]; // Max environment variable size on Windows
    DWORD size = GetEnvironmentVariableA(name, buffer, sizeof(buffer));
    if (size > 0 && size < sizeof(buffer)) {
        set_last_error(ErrorCode::Success);
        return std::string(buffer);
    }
#else
    const char* value = ::getenv(name);
    if (value) {
        set_last_error(ErrorCode::Success);
        return std::string(value);
    }
#endif
    
    set_last_error(ErrorCode::FileNotFound);
    return "";
}

bool sys_setenv(const char* name, const char* value, bool overwrite) {
    if (!name || !value) {
        set_last_error(ErrorCode::InvalidArgument);
        return false;
    }
    
#ifdef SAD_PLATFORM_WINDOWS
    if (SetEnvironmentVariableA(name, value)) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    set_last_error(errno_to_error_code(GetLastError()));
    return false;
#else
    if (::setenv(name, value, overwrite ? 1 : 0) == 0) {
        set_last_error(ErrorCode::Success);
        return true;
    }
    set_last_error(errno_to_error_code(errno));
    return false;
#endif
}

// ============================================================================
// Helper Functions Implementation / تنفيذ الدوال المساعدة
// ============================================================================

std::vector<uint8_t> sys_read_file(const char* path) {
    FileHandle handle = sys_open(path, OpenMode::ReadOnly);
    if (handle == INVALID_FILE) {
        return {};
    }
    
    FileInfo info;
    if (!sys_fstat(handle, &info)) {
        sys_close(handle);
        return {};
    }
    
    std::vector<uint8_t> data(info.size);
    int64_t bytes_read = sys_read(handle, data.data(), info.size);
    sys_close(handle);
    
    if (bytes_read != static_cast<int64_t>(info.size)) {
        return {};
    }
    
    return data;
}

bool sys_write_file(const char* path, const void* data, size_t size) {
    FileHandle handle = sys_open(
        path,
        OpenMode::WriteOnly | OpenMode::Create | OpenMode::Truncate
    );
    
    if (handle == INVALID_FILE) {
        return false;
    }
    
    int64_t bytes_written = sys_write(handle, data, size);
    sys_close(handle);
    
    return bytes_written == static_cast<int64_t>(size);
}

bool sys_file_exists(const char* path) {
    FileInfo info;
    return sys_stat(path, &info);
}

int64_t sys_file_size(const char* path) {
    FileInfo info;
    if (sys_stat(path, &info)) {
        return info.size;
    }
    return -1;
}

FileHandle sys_tmpfile(std::string* path) {
#ifdef SAD_PLATFORM_WINDOWS
    char temp_path[MAX_PATH];
    char temp_file[MAX_PATH];
    
    GetTempPathA(MAX_PATH, temp_path);
    GetTempFileNameA(temp_path, "sad", 0, temp_file);
    
    if (path) {
        *path = temp_file;
    }
    
    return sys_open(
        temp_file,
        OpenMode::ReadWrite | OpenMode::Create | OpenMode::Exclusive
    );
#else
    char temp_template[] = "/tmp/sadXXXXXX";
    int fd = ::mkstemp(temp_template);
    
    if (fd < 0) {
        set_last_error(errno_to_error_code(errno));
        return INVALID_FILE;
    }
    
    if (path) {
        *path = temp_template;
    }
    
    set_last_error(ErrorCode::Success);
    return fd;
#endif
}

} // namespace SysCall
} // namespace Sad
