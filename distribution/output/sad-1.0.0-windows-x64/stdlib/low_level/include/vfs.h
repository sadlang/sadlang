/*
 * ============================================================================
 * وحدة نظام الملفات الافتراضي (VFS) - لغة ص
 * Virtual Filesystem Module - Sad Language
 * ============================================================================
 *
 * @brief (AR) طبقة تجريد نظام الملفات: عقد، ملفات، مجلدات، عمليات
 *              دعم أنظمة ملفات متعددة قابلة للتسجيل
 *
 * @brief (EN) Filesystem abstraction layer: inodes, files, directories, ops
 *              Multiple registrable filesystems support
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_VFS_H
#define SAD_LOW_LEVEL_VFS_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <functional>

namespace Sad {
namespace LowLevel {

// ============================================================================
// تعدادات / Enumerations
// ============================================================================

/// @brief (AR) نوع العقدة / (EN) Inode type
enum class InodeType : uint8_t {
    FILE,           // ملف عادي / Regular file
    DIRECTORY,      // مجلد / Directory
    SYMLINK,        // رابط رمزي / Symbolic link
    DEVICE_CHAR,    // جهاز حرفي / Character device
    DEVICE_BLOCK,   // جهاز كتلي / Block device
    PIPE,           // أنبوب / Pipe
    SOCKET,         // مقبس / Socket
    MOUNT_POINT     // نقطة تحميل / Mount point
};

/// @brief (AR) أوضاع فتح الملف / (EN) File open modes
enum class OpenMode : uint16_t {
    READ        = 0x0001,   // قراءة / Read
    WRITE       = 0x0002,   // كتابة / Write
    APPEND      = 0x0004,   // إلحاق / Append
    CREATE      = 0x0008,   // إنشاء إذا لم يوجد / Create if not exist
    TRUNCATE    = 0x0010,   // اقتطاع / Truncate
    EXCLUSIVE   = 0x0020,   // حصري / Exclusive
    DIRECTORY   = 0x0040    // فتح كمجلد / Open as directory
};

/// @brief (AR) مرجع البحث / (EN) Seek origin
enum class SeekOrigin : uint8_t {
    BEGIN   = 0,    // من البداية / From beginning
    CURRENT = 1,    // من الموضع الحالي / From current position
    END     = 2     // من النهاية / From end
};

/// @brief (AR) أذونات الملف / (EN) File permissions
struct FilePermissions {
    bool ownerRead : 1;
    bool ownerWrite : 1;
    bool ownerExec : 1;
    bool groupRead : 1;
    bool groupWrite : 1;
    bool groupExec : 1;
    bool otherRead : 1;
    bool otherWrite : 1;
    bool otherExec : 1;

    FilePermissions()
        : ownerRead(true), ownerWrite(true), ownerExec(false),
          groupRead(true), groupWrite(false), groupExec(false),
          otherRead(true), otherWrite(false), otherExec(false) {}

    /// @brief (AR) تحويل إلى وضع Unix / (EN) Convert to Unix mode
    uint16_t toMode() const {
        return (ownerRead  ? 0400 : 0) | (ownerWrite ? 0200 : 0) | (ownerExec  ? 0100 : 0) |
               (groupRead ? 0040 : 0) | (groupWrite ? 0020 : 0) | (groupExec  ? 0010 : 0) |
               (otherRead ? 0004 : 0) | (otherWrite ? 0002 : 0) | (otherExec  ? 0001 : 0);
    }
};

// ============================================================================
// هياكل / Structures
// ============================================================================

/// @brief (AR) معلومات الملف / (EN) File stat information
struct FileStat {
    uint64_t        inodeNumber;    // رقم العقدة / Inode number
    InodeType       type;           // النوع / Type
    FilePermissions permissions;    // الأذونات / Permissions
    uint32_t        linkCount;      // عدد الروابط / Link count
    uint32_t        uid;            // معرّف المالك / Owner UID
    uint32_t        gid;            // معرّف المجموعة / Group GID
    uint64_t        size;           // الحجم / Size
    uint64_t        blockCount;     // عدد الكتل / Block count
    uint32_t        blockSize;      // حجم الكتلة / Block size
    uint64_t        accessTime;     // وقت الوصول / Access time
    uint64_t        modifyTime;     // وقت التعديل / Modify time
    uint64_t        createTime;     // وقت الإنشاء / Create time
    uint16_t        deviceMajor;    // رقم الجهاز الرئيسي / Device major
    uint16_t        deviceMinor;    // رقم الجهاز الفرعي / Device minor

    FileStat() : inodeNumber(0), type(InodeType::FILE), linkCount(1),
                 uid(0), gid(0), size(0), blockCount(0), blockSize(4096),
                 accessTime(0), modifyTime(0), createTime(0),
                 deviceMajor(0), deviceMinor(0) {}
};

/// @brief (AR) مدخل مجلد / (EN) Directory entry
struct DirectoryEntry {
    uint64_t    inodeNumber;
    InodeType   type;
    std::string name;
};

// ============================================================================
// عمليات نظام الملفات / Filesystem Operations
// ============================================================================

/**
 * @brief (AR) واجهة عمليات نظام الملفات (يجب تنفيذها لكل نظام ملفات)
 * @brief (EN) Filesystem operations interface (must implement for each FS)
 */
struct FilesystemOps {
    /// @brief (AR) تحميل نظام الملفات / (EN) Mount filesystem
    std::function<bool(uint64_t deviceId, void* privateData)> mount;

    /// @brief (AR) فصل نظام الملفات / (EN) Unmount filesystem
    std::function<bool(void* privateData)> unmount;

    /// @brief (AR) فتح ملف / (EN) Open file
    std::function<int64_t(const char* path, uint16_t mode, void* privateData)> open;

    /// @brief (AR) إغلاق ملف / (EN) Close file
    std::function<bool(int64_t fd, void* privateData)> close;

    /// @brief (AR) قراءة / (EN) Read
    std::function<int64_t(int64_t fd, void* buffer, uint64_t size, void* privateData)> read;

    /// @brief (AR) كتابة / (EN) Write
    std::function<int64_t(int64_t fd, const void* buffer, uint64_t size, void* privateData)> write;

    /// @brief (AR) بحث / (EN) Seek
    std::function<int64_t(int64_t fd, int64_t offset, uint8_t origin, void* privateData)> seek;

    /// @brief (AR) حالة / (EN) Stat
    std::function<bool(const char* path, FileStat* stat, void* privateData)> stat;

    /// @brief (AR) إنشاء مجلد / (EN) Make directory
    std::function<bool(const char* path, uint16_t mode, void* privateData)> mkdir;

    /// @brief (AR) حذف مجلد / (EN) Remove directory
    std::function<bool(const char* path, void* privateData)> rmdir;

    /// @brief (AR) حذف ملف / (EN) Unlink file
    std::function<bool(const char* path, void* privateData)> unlink;

    /// @brief (AR) إعادة تسمية / (EN) Rename
    std::function<bool(const char* oldPath, const char* newPath, void* privateData)> rename;

    /// @brief (AR) قراءة مجلد / (EN) Read directory
    std::function<std::vector<DirectoryEntry>(const char* path, void* privateData)> readdir;

    /// @brief (AR) اقتطاع ملف / (EN) Truncate file
    std::function<bool(const char* path, uint64_t length, void* privateData)> truncate;
};

// ============================================================================
// نظام ملفات مسجل / Registered Filesystem
// ============================================================================

/// @brief (AR) نظام ملفات مسجل / (EN) Registered filesystem
struct RegisteredFilesystem {
    std::string name;           // اسم (مثل "ext2", "fat32") / Name
    FilesystemOps ops;          // العمليات / Operations
    bool isDefault;             // هل افتراضي / Is default
};

/// @brief (AR) نقطة تحميل / (EN) Mount point
struct MountPoint {
    std::string path;           // مسار التحميل / Mount path
    std::string fsName;         // اسم نظام الملفات / FS name
    uint64_t    deviceId;       // معرّف الجهاز / Device ID
    void*       privateData;    // بيانات خاصة / Private data
    bool        readOnly;       // للقراءة فقط / Read-only
};

/// @brief (AR) واصف ملف مفتوح / (EN) Open file descriptor
struct OpenFile {
    int64_t     fd;             // واصف الملف / File descriptor
    std::string path;           // المسار / Path
    std::string mountPath;      // مسار التحميل / Mount path
    uint16_t    mode;           // وضع الفتح / Open mode
    uint64_t    position;       // موضع القراءة/الكتابة / R/W position
    uint64_t    size;           // حجم الملف / File size
    bool        isOpen;         // هل مفتوح / Is open
};

// ============================================================================
// مدير VFS / VFS Manager
// ============================================================================

/**
 * @brief (AR) مدير نظام الملفات الافتراضي - طبقة التجريد الرئيسية
 * @brief (EN) VFS Manager - Main abstraction layer
 */
class VFSManager {
public:
    /// @brief (AR) الحصول على المثيل الوحيد / (EN) Get singleton instance
    static VFSManager& getInstance();

    // ========================================================================
    // تسجيل أنظمة الملفات / Filesystem Registration
    // ========================================================================

    /// @brief (AR) تسجيل نظام ملفات / (EN) Register filesystem
    bool registerFilesystem(const std::string& name, const FilesystemOps& ops);

    /// @brief (AR) إلغاء تسجيل / (EN) Unregister filesystem
    bool unregisterFilesystem(const std::string& name);

    // ========================================================================
    // التحميل والفصل / Mount/Unmount
    // ========================================================================

    /// @brief (AR) تحميل نظام ملفات / (EN) Mount filesystem
    bool mount(const std::string& devicePath, const std::string& mountPath,
               const std::string& fsType, bool readOnly = false);

    /// @brief (AR) فصل / (EN) Unmount
    bool unmount(const std::string& mountPath);

    // ========================================================================
    // عمليات الملفات / File Operations
    // ========================================================================

    /// @brief (AR) فتح ملف / (EN) Open file
    int64_t open(const std::string& path, uint16_t mode);

    /// @brief (AR) إغلاق ملف / (EN) Close file
    bool close(int64_t fd);

    /// @brief (AR) قراءة / (EN) Read
    int64_t read(int64_t fd, void* buffer, uint64_t size);

    /// @brief (AR) كتابة / (EN) Write
    int64_t write(int64_t fd, const void* buffer, uint64_t size);

    /// @brief (AR) بحث / (EN) Seek
    int64_t seek(int64_t fd, int64_t offset, SeekOrigin origin);

    /// @brief (AR) حالة ملف / (EN) File stat
    bool stat(const std::string& path, FileStat* statBuf);

    // ========================================================================
    // عمليات المجلدات / Directory Operations
    // ========================================================================

    /// @brief (AR) إنشاء مجلد / (EN) Make directory
    bool mkdir(const std::string& path, uint16_t mode = 0755);

    /// @brief (AR) حذف مجلد / (EN) Remove directory
    bool rmdir(const std::string& path);

    /// @brief (AR) قراءة مجلد / (EN) Read directory
    std::vector<DirectoryEntry> readdir(const std::string& path);

    // ========================================================================
    // عمليات أخرى / Other Operations
    // ========================================================================

    /// @brief (AR) حذف ملف / (EN) Delete file
    bool unlink(const std::string& path);

    /// @brief (AR) إعادة تسمية / (EN) Rename
    bool rename(const std::string& oldPath, const std::string& newPath);

    /// @brief (AR) اقتطاع ملف / (EN) Truncate file
    bool truncate(const std::string& path, uint64_t length);

    /// @brief (AR) هل الملف موجود / (EN) Does file exist
    bool exists(const std::string& path);

    // ========================================================================
    // المعلومات / Information
    // ========================================================================

    /// @brief (AR) نقاط التحميل / (EN) Mount points
    const std::vector<MountPoint>& getMountPoints() const { return mountPoints_; }

    /// @brief (AR) الملفات المفتوحة / (EN) Open files
    const std::vector<OpenFile>& getOpenFiles() const { return openFiles_; }

    /// @brief (AR) توليد تقرير / (EN) Generate report
    std::string generateReport() const;

    /// @brief (AR) هل مهيأ / (EN) Is initialized
    bool isInitialized() const { return isInitialized_; }

private:
    VFSManager();
    VFSManager(const VFSManager&) = delete;
    VFSManager& operator=(const VFSManager&) = delete;

    /// @brief (AR) إيجاد نقطة التحميل لمسار / (EN) Find mount point for path
    MountPoint* findMountPoint(const std::string& path);

    /// @brief (AR) إيجاد نظام ملفات مسجل / (EN) Find registered FS
    RegisteredFilesystem* findFilesystem(const std::string& name);

    /// @brief (AR) تحويل مسار مطلق لنسبي / (EN) Convert absolute to relative path
    std::string relativePath(const std::string& path, const std::string& mountPath) const;

    /// @brief (AR) إيجاد واصف ملف / (EN) Find open file by fd
    OpenFile* findOpenFile(int64_t fd);

    std::vector<RegisteredFilesystem> filesystems_;
    std::vector<MountPoint> mountPoints_;
    std::vector<OpenFile>   openFiles_;
    int64_t nextFd_;
    bool isInitialized_;
};

// ============================================================================
// ثوابت / Constants
// ============================================================================

namespace VFSConstants {
    static constexpr int64_t  INVALID_FD    = -1;
    static constexpr uint64_t MAX_PATH_LEN  = 4096;
    static constexpr uint64_t MAX_NAME_LEN  = 255;
    static constexpr uint64_t MAX_OPEN_FILES = 1024;
    static constexpr uint32_t DEFAULT_BLOCK_SIZE = 4096;
}

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_VFS_H
