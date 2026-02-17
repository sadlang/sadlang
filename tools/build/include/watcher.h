// بسم الله الرحمن الرحيم
/**
 * @file watcher.h
 * @brief File System Watcher - مراقب نظام الملفات
 * 
 * Monitors source files for changes and triggers automatic rebuilds.
 * Implements watch mode for development workflow.
 * 
 * Features:
 * - Watch multiple directories
 * - Detect file changes (add/modify/delete)
 * - Debouncing (avoid rapid rebuilds)
 * - Configurable poll interval
 * - Callback system for events
 * 
 * @author Sad Language Team
 * @date 2026-01-07
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <ctime>

namespace sad {
namespace build {

/**
 * @brief File change event type - نوع حدث تغيير الملف
 */
enum class FileEventType {
    ADDED,      ///< File added
    MODIFIED,   ///< File modified
    DELETED     ///< File deleted
};

/**
 * @brief File change event - حدث تغيير ملف
 */
struct FileEvent {
    FileEventType type;         ///< Event type
    std::string file_path;      ///< File path
    std::time_t timestamp;      ///< Event timestamp
    
    /**
     * @brief Get event type as string
     */
    std::string getTypeString() const;
};

/**
 * @brief Watcher callback function
 * 
 * Called when files change.
 * 
 * @param events Vector of file events
 */
using WatcherCallback = std::function<void(const std::vector<FileEvent>&)>;

/**
 * @brief Watcher configuration - إعدادات المراقب
 */
struct WatcherConfig {
    std::chrono::milliseconds poll_interval = std::chrono::milliseconds(500);
                                ///< Poll interval (default: 500ms)
    
    std::chrono::milliseconds debounce_delay = std::chrono::milliseconds(1000);
                                ///< Debounce delay (default: 1000ms)
    
    bool recursive = true;      ///< Watch subdirectories recursively
    bool ignore_hidden = true;  ///< Ignore hidden files (starting with .)
    
    std::vector<std::string> ignore_patterns = {
        "*.o", "*.obj", "*.a", "*.lib", "*.exe",
        "build/", ".git/", ".sad_cache/"
    };                          ///< Patterns to ignore
};

/**
 * @brief File system watcher class - مراقب نظام الملفات
 * 
 * Watches directories for file changes and triggers callbacks.
 * 
 * Example usage:
 * @code
 * Watcher watcher;
 * 
 * // Configure
 * WatcherConfig config;
 * config.poll_interval = std::chrono::milliseconds(1000);
 * watcher.setConfig(config);
 * 
 * // Add directories to watch
 * watcher.addPath("src/");
 * watcher.addPath("lib/");
 * 
 * // Set callback
 * watcher.setCallback([](const std::vector<FileEvent>& events) {
 *     for (const auto& event : events) {
 *         std::cout << "File changed: " << event.file_path << "\n";
 *     }
 *     // Trigger rebuild
 *     build();
 * });
 * 
 * // Start watching (blocks)
 * watcher.start();
 * 
 * // In another thread, stop watching
 * watcher.stop();
 * @endcode
 */
class Watcher {
public:
    /**
     * @brief Constructor
     */
    Watcher();

    /**
     * @brief Destructor - stops watching automatically
     */
    ~Watcher();

    // Disable copy
    Watcher(const Watcher&) = delete;
    Watcher& operator=(const Watcher&) = delete;

    /**
     * @brief Set watcher configuration
     * 
     * @param config Configuration
     */
    void setConfig(const WatcherConfig& config);

    /**
     * @brief Get watcher configuration
     * 
     * @return Current configuration
     */
    const WatcherConfig& getConfig() const { return config_; }

    /**
     * @brief Add path to watch - إضافة مسار للمراقبة
     * 
     * @param path Directory path to watch
     * @return true if added successfully
     */
    bool addPath(const std::string& path);

    /**
     * @brief Remove path from watching
     * 
     * @param path Directory path to remove
     * @return true if removed successfully
     */
    bool removePath(const std::string& path);

    /**
     * @brief Get watched paths
     * 
     * @return Vector of watched directory paths
     */
    std::vector<std::string> getWatchedPaths() const;

    /**
     * @brief Set change callback
     * 
     * @param callback Callback function
     */
    void setCallback(WatcherCallback callback);

    /**
     * @brief Start watching - بدء المراقبة
     * 
     * Starts watching in a background thread.
     * Non-blocking.
     */
    void start();

    /**
     * @brief Start watching (blocking) - بدء المراقبة (محظور)
     * 
     * Blocks until stop() is called.
     */
    void startBlocking();

    /**
     * @brief Stop watching - إيقاف المراقبة
     * 
     * Stops the watcher thread.
     */
    void stop();

    /**
     * @brief Check if watching
     * 
     * @return true if currently watching
     */
    bool isWatching() const { return watching_; }

    /**
     * @brief Get number of watched files
     * 
     * @return Number of files being monitored
     */
    size_t getWatchedFileCount() const { return file_timestamps_.size(); }

private:
    WatcherConfig config_;                      ///< Configuration
    std::vector<std::string> watched_paths_;    ///< Watched directory paths
    WatcherCallback callback_;                  ///< Change callback
    
    std::map<std::string, std::time_t> file_timestamps_;
                                                ///< File path -> last modified time
    
    std::atomic<bool> watching_;                ///< Watching flag
    std::atomic<bool> should_stop_;             ///< Stop flag
    std::unique_ptr<std::thread> watch_thread_; ///< Watch thread
    
    std::chrono::steady_clock::time_point last_event_time_;
                                                ///< Last event time (for debouncing)
    std::vector<FileEvent> pending_events_;     ///< Pending events (debouncing)
    
    /**
     * @brief Main watch loop
     */
    void watchLoop();

    /**
     * @brief Poll for changes
     * 
     * @return Vector of file events
     */
    std::vector<FileEvent> pollChanges();

    /**
     * @brief Scan directory for files
     * 
     * @param dir_path Directory path
     * @param recursive Scan recursively
     * @return Map of file path to last modified time
     */
    std::map<std::string, std::time_t> scanDirectory(const std::string& dir_path, 
                                                      bool recursive);

    /**
     * @brief Check if file should be ignored
     * 
     * @param file_path File path
     * @return true if should be ignored
     */
    bool shouldIgnore(const std::string& file_path) const;

    /**
     * @brief Check if path matches pattern
     * 
     * @param path File path
     * @param pattern Pattern (supports * wildcard)
     * @return true if matches
     */
    static bool matchesPattern(const std::string& path, const std::string& pattern);

    /**
     * @brief Get file last modified time
     * 
     * @param file_path File path
     * @return Last modified time, or 0 if error
     */
    static std::time_t getFileModifiedTime(const std::string& file_path);

    /**
     * @brief Process pending events (debouncing)
     * 
     * @return true if events were processed
     */
    bool processPendingEvents();

    /**
     * @brief Add event to pending events
     * 
     * @param event File event
     */
    void addPendingEvent(const FileEvent& event);

    /**
     * @brief Print file event
     * 
     * @param event File event
     */
    void printEvent(const FileEvent& event) const;
};

} // namespace build
} // namespace sad
