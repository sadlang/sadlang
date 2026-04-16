/**
 * @file file_watcher.cpp
 * @brief (AR) تنفيذ مراقب الملفات الأصلي متعدد المنصات
 * @brief (EN) Native cross-platform file watcher implementation
 * 
 * @author فريق لغة ص
 * @date July 2025
 */

#include "file_watcher.h"
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <sys/inotify.h>
#include <unistd.h>
#include <poll.h>
#elif defined(__APPLE__)
#include <sys/event.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace Sad {
namespace HotReload {

// ═══════════════════════════════════════════════════════════════════════════════
// البناء والهدم / Constructor and Destructor
// ═══════════════════════════════════════════════════════════════════════════════

FileWatcher::FileWatcher(int debounceMs)
    : debounceMs_(debounceMs)
    , lastNotification_(std::chrono::steady_clock::now()) {
}

FileWatcher::~FileWatcher() {
    stop();
}

// ═══════════════════════════════════════════════════════════════════════════════
// واجهة عامة / Public Interface
// ═══════════════════════════════════════════════════════════════════════════════

void FileWatcher::watchFile(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    namespace fs = std::filesystem;
    WatchedFile wf;
    wf.path = fs::absolute(filePath).string();
    wf.exists = fs::exists(wf.path);
    if (wf.exists) {
        wf.lastModified = fs::last_write_time(wf.path);
    }
    
    // (AR) تجنب المراقبة المكررة / (EN) Avoid duplicate watches
    for (const auto& existing : watchedFiles_) {
        if (existing.path == wf.path) return;
    }
    
    watchedFiles_.push_back(std::move(wf));
}

void FileWatcher::watchDirectory(const std::string& dirPath, bool recursive) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    namespace fs = std::filesystem;
    std::string absPath = fs::absolute(dirPath).string();
    
    // (AR) تجنب المراقبة المكررة / (EN) Avoid duplicate watches
    for (const auto& existing : watchedDirs_) {
        if (existing == absPath) return;
    }
    
    watchedDirs_.push_back(absPath);
    
    // (AR) مسح المجلد للعثور على ملفات .ص / (EN) Scan directory for .ص files
    if (fs::exists(absPath)) {
        auto iterator = recursive
            ? fs::recursive_directory_iterator(absPath)
            : fs::recursive_directory_iterator(absPath);  // will be limited below
        
        if (recursive) {
            for (const auto& entry : fs::recursive_directory_iterator(absPath)) {
                if (entry.is_regular_file() && isSadFile(entry.path().string())) {
                    WatchedFile wf;
                    wf.path = entry.path().string();
                    wf.exists = true;
                    wf.lastModified = entry.last_write_time();
                    
                    bool duplicate = false;
                    for (const auto& existing : watchedFiles_) {
                        if (existing.path == wf.path) { duplicate = true; break; }
                    }
                    if (!duplicate) {
                        watchedFiles_.push_back(std::move(wf));
                    }
                }
            }
        } else {
            for (const auto& entry : fs::directory_iterator(absPath)) {
                if (entry.is_regular_file() && isSadFile(entry.path().string())) {
                    WatchedFile wf;
                    wf.path = entry.path().string();
                    wf.exists = true;
                    wf.lastModified = entry.last_write_time();
                    
                    bool duplicate = false;
                    for (const auto& existing : watchedFiles_) {
                        if (existing.path == wf.path) { duplicate = true; break; }
                    }
                    if (!duplicate) {
                        watchedFiles_.push_back(std::move(wf));
                    }
                }
            }
        }
    }
}

void FileWatcher::setCallback(FileChangeCallback callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    callback_ = std::move(callback);
}

void FileWatcher::start() {
    if (running_.load()) return;
    
    running_.store(true);
    watchThread_ = std::thread(&FileWatcher::watchLoop, this);
}

void FileWatcher::stop() {
    running_.store(false);
    if (watchThread_.joinable()) {
        watchThread_.join();
    }
}

bool FileWatcher::poll() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    bool changed = false;
    for (auto& wf : watchedFiles_) {
        if (checkFileChanged(wf)) {
            changed = true;
        }
    }
    return changed;
}

// ═══════════════════════════════════════════════════════════════════════════════
// التنفيذ الداخلي / Internal Implementation
// ═══════════════════════════════════════════════════════════════════════════════

bool FileWatcher::isSadFile(const std::string& path) {
    namespace fs = std::filesystem;
    auto ext = fs::path(path).extension().string();
    // (AR) امتداد .ص (UTF-8: D8 B5) أو .sad
    return ext == ".sad" || ext == ".\xD8\xB5" || 
           // (AR) تحقق إضافي — بعض الأنظمة تعيد الامتداد بصيغة مختلفة
           path.size() > 4 && (path.substr(path.size() - 4) == ".\xD8\xB5" ||
                               path.substr(path.size() - 4) == ".sad");
}

bool FileWatcher::checkFileChanged(WatchedFile& file) {
    namespace fs = std::filesystem;
    
    bool currentlyExists = fs::exists(file.path);
    
    if (!file.exists && currentlyExists) {
        // (AR) ملف جديد / (EN) New file
        file.exists = true;
        file.lastModified = fs::last_write_time(file.path);
        notifyChange({file.path, FileChangeType::Created, std::chrono::steady_clock::now()});
        return true;
    }
    
    if (file.exists && !currentlyExists) {
        // (AR) ملف محذوف / (EN) Deleted file
        file.exists = false;
        notifyChange({file.path, FileChangeType::Deleted, std::chrono::steady_clock::now()});
        return true;
    }
    
    if (currentlyExists) {
        auto currentMod = fs::last_write_time(file.path);
        if (currentMod != file.lastModified) {
            file.lastModified = currentMod;
            notifyChange({file.path, FileChangeType::Modified, std::chrono::steady_clock::now()});
            return true;
        }
    }
    
    return false;
}

void FileWatcher::notifyChange(const FileChange& change) {
    // (AR) تطبيق debounce — تجاهل التغييرات المتكررة خلال فترة قصيرة
    // (EN) Apply debounce — ignore rapid successive changes
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - lastNotification_).count();
    
    if (elapsed < debounceMs_) {
        return;  // (AR) تجاهل — لم تمر فترة كافية / (EN) Skip — debounce period not elapsed
    }
    
    lastNotification_ = now;
    
    if (callback_) {
        callback_(change);
    }
}

void FileWatcher::scanDirectories() {
    namespace fs = std::filesystem;
    
    for (const auto& dir : watchedDirs_) {
        if (!fs::exists(dir)) continue;
        
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file() && isSadFile(entry.path().string())) {
                WatchedFile wf;
                wf.path = entry.path().string();
                wf.exists = true;
                wf.lastModified = entry.last_write_time();
                
                bool duplicate = false;
                for (const auto& existing : watchedFiles_) {
                    if (existing.path == wf.path) { duplicate = true; break; }
                }
                if (!duplicate) {
                    watchedFiles_.push_back(std::move(wf));
                }
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// حلقة المراقبة الرئيسية / Main Watch Loop
// ═══════════════════════════════════════════════════════════════════════════════

void FileWatcher::watchLoop() {
#ifdef _WIN32
    watchLoopWindows();
#elif defined(__linux__)
    watchLoopLinux();
#else
    // (AR) macOS و غيرها — استخدام الفحص الدوري كاحتياطي
    // (EN) macOS and others — use polling as fallback
    watchLoopPolling();
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// Windows: ReadDirectoryChangesW
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef _WIN32
void FileWatcher::watchLoopWindows() {
    namespace fs = std::filesystem;
    
    // (AR) جمع المجلدات الفريدة من الملفات المراقبة
    // (EN) Collect unique directories from watched files
    std::vector<std::string> dirs;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& wf : watchedFiles_) {
            auto dir = fs::path(wf.path).parent_path().string();
            if (std::find(dirs.begin(), dirs.end(), dir) == dirs.end()) {
                dirs.push_back(dir);
            }
        }
        for (const auto& d : watchedDirs_) {
            if (std::find(dirs.begin(), dirs.end(), d) == dirs.end()) {
                dirs.push_back(d);
            }
        }
    }
    
    if (dirs.empty()) {
        // (AR) لا مجلدات للمراقبة — الانتقال للفحص الدوري
        watchLoopPolling();
        return;
    }
    
    // (AR) فتح مقبض للمجلد الأول (يمكن توسيعه لمراقبة عدة مجلدات)
    // (EN) Open handle to first directory (expandable to multiple)
    std::wstring wDir(dirs[0].begin(), dirs[0].end());
    HANDLE dirHandle = CreateFileW(
        wDir.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        nullptr
    );
    
    if (dirHandle == INVALID_HANDLE_VALUE) {
        // (AR) فشل فتح المجلد — الانتقال للفحص الدوري
        watchLoopPolling();
        return;
    }
    
    // (AR) إنشاء حدث للإلغاء
    HANDLE cancelEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    
    alignas(DWORD) char buffer[4096];
    OVERLAPPED overlapped = {};
    overlapped.hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    
    while (running_.load()) {
        BOOL success = ReadDirectoryChangesW(
            dirHandle,
            buffer,
            sizeof(buffer),
            TRUE,  // (AR) مراقبة المجلدات الفرعية / (EN) Watch subtree
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
            nullptr,
            &overlapped,
            nullptr
        );
        
        if (!success) break;
        
        // (AR) انتظار التغيير أو إشارة الإيقاف
        // (EN) Wait for change or stop signal
        HANDLE handles[] = { overlapped.hEvent, cancelEvent };
        DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, 500);
        
        if (waitResult == WAIT_OBJECT_0) {
            // (AR) تم اكتشاف تغيير
            DWORD bytesReturned = 0;
            GetOverlappedResult(dirHandle, &overlapped, &bytesReturned, FALSE);
            
            if (bytesReturned > 0) {
                auto* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
                while (info) {
                    std::wstring wName(info->FileName, info->FileNameLength / sizeof(WCHAR));
                    std::string fileName(wName.begin(), wName.end());
                    std::string fullPath = dirs[0] + "\\" + fileName;
                    
                    if (isSadFile(fullPath)) {
                        std::lock_guard<std::mutex> lock(mutex_);
                        // (AR) تحديث آخر وقت تعديل + إشعار
                        for (auto& wf : watchedFiles_) {
                            if (wf.path == fullPath || 
                                fs::path(wf.path).filename() == fs::path(fullPath).filename()) {
                                checkFileChanged(wf);
                                break;
                            }
                        }
                    }
                    
                    if (info->NextEntryOffset == 0) break;
                    info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                        reinterpret_cast<char*>(info) + info->NextEntryOffset);
                }
            }
            
            ResetEvent(overlapped.hEvent);
        }
        else if (waitResult == WAIT_OBJECT_0 + 1) {
            // (AR) إشارة إيقاف / (EN) Stop signal
            break;
        }
        // WAIT_TIMEOUT — استمر في الحلقة
    }
    
    CloseHandle(overlapped.hEvent);
    CloseHandle(cancelEvent);
    CloseHandle(dirHandle);
}
#else
void FileWatcher::watchLoopWindows() {
    watchLoopPolling();
}
#endif

// ═══════════════════════════════════════════════════════════════════════════════
// Linux: inotify
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef __linux__
void FileWatcher::watchLoopLinux() {
    namespace fs = std::filesystem;
    
    int inotifyFd = inotify_init1(IN_NONBLOCK);
    if (inotifyFd < 0) {
        watchLoopPolling();
        return;
    }
    
    // (AR) مراقبة المجلدات المطلوبة
    std::unordered_map<int, std::string> watchDescriptors;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // (AR) جمع المجلدات الفريدة
        std::vector<std::string> dirs;
        for (const auto& wf : watchedFiles_) {
            auto dir = fs::path(wf.path).parent_path().string();
            if (std::find(dirs.begin(), dirs.end(), dir) == dirs.end()) {
                dirs.push_back(dir);
            }
        }
        for (const auto& d : watchedDirs_) {
            if (std::find(dirs.begin(), dirs.end(), d) == dirs.end()) {
                dirs.push_back(d);
            }
        }
        
        for (const auto& dir : dirs) {
            int wd = inotify_add_watch(inotifyFd, dir.c_str(),
                IN_MODIFY | IN_CREATE | IN_DELETE | IN_CLOSE_WRITE);
            if (wd >= 0) {
                watchDescriptors[wd] = dir;
            }
        }
    }
    
    char buffer[4096];
    
    while (running_.load()) {
        struct pollfd pfd = { inotifyFd, POLLIN, 0 };
        int ret = ::poll(&pfd, 1, 500);  // (AR) انتظار 500ms / (EN) Wait 500ms
        
        if (ret > 0 && (pfd.revents & POLLIN)) {
            ssize_t len = read(inotifyFd, buffer, sizeof(buffer));
            if (len > 0) {
                char* ptr = buffer;
                while (ptr < buffer + len) {
                    auto* event = reinterpret_cast<struct inotify_event*>(ptr);
                    
                    if (event->len > 0) {
                        std::string filename(event->name);
                        auto it = watchDescriptors.find(event->wd);
                        if (it != watchDescriptors.end()) {
                            std::string fullPath = it->second + "/" + filename;
                            
                            if (isSadFile(fullPath)) {
                                std::lock_guard<std::mutex> lock(mutex_);
                                for (auto& wf : watchedFiles_) {
                                    checkFileChanged(wf);
                                }
                            }
                        }
                    }
                    
                    ptr += sizeof(struct inotify_event) + event->len;
                }
            }
        }
    }
    
    // (AR) تنظيف / (EN) Cleanup
    for (const auto& [wd, dir] : watchDescriptors) {
        inotify_rm_watch(inotifyFd, wd);
    }
    close(inotifyFd);
}
#else
void FileWatcher::watchLoopLinux() {
    watchLoopPolling();
}
#endif

// ═══════════════════════════════════════════════════════════════════════════════
// الفحص الدوري (احتياطي لجميع المنصات) / Polling Fallback
// ═══════════════════════════════════════════════════════════════════════════════

void FileWatcher::watchLoopPolling() {
    while (running_.load()) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            for (auto& wf : watchedFiles_) {
                checkFileChanged(wf);
            }
        }
        
        // (AR) انتظار قبل الفحص التالي / (EN) Wait before next check
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

} // namespace HotReload
} // namespace Sad
