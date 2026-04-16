/**
 * @file file_watcher.h
 * @brief (AR) مراقب ملفات أصلي متعدد المنصات — لنظام إعادة التحميل الحي
 * @brief (EN) Native cross-platform file watcher — for Hot Reload system
 * 
 * يراقب ملفات .ص للتغييرات ويُنشئ إشعارات عند التعديل.
 * يستخدم واجهات أصلية لكل منصة:
 * - Windows: ReadDirectoryChangesW
 * - Linux: inotify
 * - macOS: kqueue/FSEvents
 * 
 * @author فريق لغة ص
 * @date July 2025
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <filesystem>

namespace Sad {
namespace HotReload {

/**
 * @brief (AR) نوع تغيير الملف
 * @brief (EN) File change type
 */
enum class FileChangeType {
    Modified,   ///< (AR) تم تعديل الملف / (EN) File was modified
    Created,    ///< (AR) تم إنشاء الملف / (EN) File was created
    Deleted     ///< (AR) تم حذف الملف / (EN) File was deleted
};

/**
 * @brief (AR) معلومات تغيير ملف
 * @brief (EN) File change info
 */
struct FileChange {
    std::string filePath;       ///< (AR) مسار الملف / (EN) File path
    FileChangeType type;        ///< (AR) نوع التغيير / (EN) Change type
    std::chrono::steady_clock::time_point timestamp; ///< (AR) وقت التغيير / (EN) Change time
};

/**
 * @brief (AR) دالة الاستدعاء عند تغيير ملف
 * @brief (EN) Callback when file changes
 */
using FileChangeCallback = std::function<void(const FileChange&)>;

/**
 * @brief (AR) مراقب ملفات أصلي متعدد المنصات
 * @brief (EN) Native cross-platform file watcher
 * 
 * يراقب مجلداً أو ملفات محددة للتغييرات ويستدعي callback عند اكتشاف تعديل.
 * يعمل في خيط منفصل لعدم حجب حلقة الأحداث الرئيسية.
 * يدعم debouncing لتجنب الإشعارات المتكررة عند الحفظ.
 */
class FileWatcher {
public:
    /**
     * @brief (AR) إنشاء مراقب ملفات
     * @brief (EN) Create file watcher
     * 
     * @param debounceMs (AR) فترة انتظار قبل الإشعار (مللي ثانية) / (EN) Debounce period (ms)
     */
    explicit FileWatcher(int debounceMs = 300);
    
    ~FileWatcher();

    // (AR) منع النسخ / (EN) Non-copyable
    FileWatcher(const FileWatcher&) = delete;
    FileWatcher& operator=(const FileWatcher&) = delete;

    /**
     * @brief (AR) إضافة ملف للمراقبة
     * @brief (EN) Add file to watch
     * 
     * @param filePath (AR) مسار الملف / (EN) File path
     */
    void watchFile(const std::string& filePath);

    /**
     * @brief (AR) إضافة مجلد للمراقبة (ملفات .ص فقط)
     * @brief (EN) Add directory to watch (.ص files only)
     * 
     * @param dirPath (AR) مسار المجلد / (EN) Directory path
     * @param recursive (AR) مراقبة المجلدات الفرعية / (EN) Watch subdirectories
     */
    void watchDirectory(const std::string& dirPath, bool recursive = true);

    /**
     * @brief (AR) تعيين دالة الاستدعاء عند تغيير ملف
     * @brief (EN) Set callback for file changes
     */
    void setCallback(FileChangeCallback callback);

    /**
     * @brief (AR) بدء المراقبة في خيط منفصل
     * @brief (EN) Start watching in background thread
     */
    void start();

    /**
     * @brief (AR) إيقاف المراقبة
     * @brief (EN) Stop watching
     */
    void stop();

    /**
     * @brief (AR) هل المراقبة نشطة؟
     * @brief (EN) Is watching active?
     */
    bool isRunning() const { return running_.load(); }

    /**
     * @brief (AR) فحص يدوي — لاستخدامه في حلقة أحداث موجودة
     * @brief (EN) Manual poll — for use in existing event loop
     * 
     * يفحص التغييرات دون خيط منفصل. يُستدعى من حلقة الأحداث الرئيسية.
     * يعيد true إذا تم اكتشاف تغيير وتنفيذ الاستدعاء.
     * 
     * @return (AR) true إذا تغيّر ملف / (EN) true if file changed
     */
    bool poll();

private:
    // (AR) الملفات المراقبة مع آخر وقت تعديل / (EN) Watched files with last modification time
    struct WatchedFile {
        std::string path;
        std::filesystem::file_time_type lastModified;
        bool exists;
    };

    std::vector<WatchedFile> watchedFiles_;
    std::vector<std::string> watchedDirs_;
    FileChangeCallback callback_;
    
    std::atomic<bool> running_{false};
    std::thread watchThread_;
    std::mutex mutex_;
    
    int debounceMs_;
    std::chrono::steady_clock::time_point lastNotification_;

    /**
     * @brief (AR) حلقة المراقبة الأصلية (تعمل في خيط منفصل)
     * @brief (EN) Native watch loop (runs in background thread)
     */
    void watchLoop();

    /**
     * @brief (AR) مراقبة أصلية لنظام Windows
     * @brief (EN) Native Windows file monitoring
     */
    void watchLoopWindows();

    /**
     * @brief (AR) مراقبة أصلية لنظام Linux
     * @brief (EN) Native Linux file monitoring (inotify)
     */
    void watchLoopLinux();

    /**
     * @brief (AR) مراقبة بالفحص الدوري (احتياطي)
     * @brief (EN) Polling-based monitoring (fallback)
     */
    void watchLoopPolling();

    /**
     * @brief (AR) فحص تغيير ملف واحد
     * @brief (EN) Check single file for changes
     */
    bool checkFileChanged(WatchedFile& file);

    /**
     * @brief (AR) هل الملف من نوع .ص؟
     * @brief (EN) Is the file a .ص file?
     */
    static bool isSadFile(const std::string& path);

    /**
     * @brief (AR) إخطار بتغيير (مع debounce)
     * @brief (EN) Notify change (with debounce)
     */
    void notifyChange(const FileChange& change);

    /**
     * @brief (AR) مسح المجلدات للعثور على ملفات .ص
     * @brief (EN) Scan directories to find .ص files
     */
    void scanDirectories();
};

} // namespace HotReload
} // namespace Sad
