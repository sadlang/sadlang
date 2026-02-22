// بسم الله الرحمن الرحيم
/**
 * @file watcher.cpp
 * @brief File System Watcher Implementation
 */

#include "../include/watcher.h"
#include <iostream>
#include <thread>
#include <filesystem>
#include <algorithm>

namespace sad {
namespace build {

std::string FileEvent::getTypeString() const {
    switch (type) {
        case FileEventType::ADDED:    return "Added";
        case FileEventType::MODIFIED: return "Modified";
        case FileEventType::DELETED:  return "Deleted";
        default:                      return "Unknown";
    }
}

Watcher::Watcher()
    : watching_(false), should_stop_(false) {
}

Watcher::~Watcher() {
    stop();
}

void Watcher::setConfig(const WatcherConfig& config) {
    config_ = config;
}

bool Watcher::addPath(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Warning: Path does not exist: " << path << "\n";
        return false;
    }

    watched_paths_.push_back(path);
    return true;
}

bool Watcher::removePath(const std::string& path) {
    auto it = std::find(watched_paths_.begin(), watched_paths_.end(), path);
    if (it != watched_paths_.end()) {
        watched_paths_.erase(it);
        return true;
    }
    return false;
}

std::vector<std::string> Watcher::getWatchedPaths() const {
    return watched_paths_;
}

void Watcher::setCallback(WatcherCallback callback) {
    callback_ = callback;
}

void Watcher::start() {
    if (watching_) {
        return;
    }

    should_stop_ = false;
    watching_ = true;

    // Initialize file timestamps
    for (const auto& path : watched_paths_) {
        auto files = scanDirectory(path, config_.recursive);
        for (const auto& [file_path, timestamp] : files) {
            file_timestamps_[file_path] = timestamp;
        }
    }

    // Start watch thread
    watch_thread_ = std::make_unique<std::thread>([this]() {
        watchLoop();
    });
}

void Watcher::startBlocking() {
    if (watching_) {
        return;
    }

    should_stop_ = false;
    watching_ = true;

    // Initialize file timestamps
    for (const auto& path : watched_paths_) {
        auto files = scanDirectory(path, config_.recursive);
        for (const auto& [file_path, timestamp] : files) {
            file_timestamps_[file_path] = timestamp;
        }
    }

    // Run watch loop in current thread
    watchLoop();
}

void Watcher::stop() {
    if (!watching_) {
        return;
    }

    should_stop_ = true;

    if (watch_thread_ && watch_thread_->joinable()) {
        watch_thread_->join();
    }

    watching_ = false;
}

void Watcher::watchLoop() {
    while (!should_stop_) {
        // Poll for changes
        std::vector<FileEvent> events = pollChanges();

        if (!events.empty()) {
            // Add to pending events
            for (const auto& event : events) {
                addPendingEvent(event);
            }

            last_event_time_ = std::chrono::steady_clock::now();
        }

        // Process pending events if debounce delay has passed
        processPendingEvents();

        // Sleep
        std::this_thread::sleep_for(config_.poll_interval);
    }
}

std::vector<FileEvent> Watcher::pollChanges() {
    std::vector<FileEvent> events;

    // Scan all watched paths
    std::map<std::string, std::time_t> current_files;
    for (const auto& path : watched_paths_) {
        auto files = scanDirectory(path, config_.recursive);
        current_files.insert(files.begin(), files.end());
    }

    // Check for added or modified files
    for (const auto& [file_path, timestamp] : current_files) {
        if (shouldIgnore(file_path)) {
            continue;
        }

        auto it = file_timestamps_.find(file_path);
        if (it == file_timestamps_.end()) {
            // New file
            FileEvent event;
            event.type = FileEventType::ADDED;
            event.file_path = file_path;
            event.timestamp = timestamp;
            events.push_back(event);

            file_timestamps_[file_path] = timestamp;
        } else if (it->second != timestamp) {
            // Modified file
            FileEvent event;
            event.type = FileEventType::MODIFIED;
            event.file_path = file_path;
            event.timestamp = timestamp;
            events.push_back(event);

            file_timestamps_[file_path] = timestamp;
        }
    }

    // Check for deleted files
    auto it = file_timestamps_.begin();
    while (it != file_timestamps_.end()) {
        if (current_files.find(it->first) == current_files.end()) {
            FileEvent event;
            event.type = FileEventType::DELETED;
            event.file_path = it->first;
            event.timestamp = std::time(nullptr);
            events.push_back(event);

            it = file_timestamps_.erase(it);
        } else {
            ++it;
        }
    }

    return events;
}

std::map<std::string, std::time_t> Watcher::scanDirectory(const std::string& dir_path, 
                                                           bool recursive) {
    std::map<std::string, std::time_t> files;

    if (!std::filesystem::exists(dir_path)) {
        return files;
    }

    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
                if (entry.is_regular_file()) {
                    std::string path = entry.path().string();
                    std::time_t mtime = getFileModifiedTime(path);
                    files[path] = mtime;
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
                if (entry.is_regular_file()) {
                    std::string path = entry.path().string();
                    std::time_t mtime = getFileModifiedTime(path);
                    files[path] = mtime;
                }
            }
        }
    } catch (...) {
        // Ignore errors
    }

    return files;
}

bool Watcher::shouldIgnore(const std::string& file_path) const {
    // Check ignore hidden
    if (config_.ignore_hidden) {
        std::filesystem::path path(file_path);
        if (!path.filename().empty() && path.filename().string()[0] == '.') {
            return true;
        }
    }

    // Check ignore patterns
    for (const auto& pattern : config_.ignore_patterns) {
        if (matchesPattern(file_path, pattern)) {
            return true;
        }
    }

    return false;
}

bool Watcher::matchesPattern(const std::string& path, const std::string& pattern) {
    // Simple wildcard matching
    if (pattern.find('*') != std::string::npos) {
        // Pattern with wildcard
        size_t star_pos = pattern.find('*');
        std::string prefix = pattern.substr(0, star_pos);
        std::string suffix = pattern.substr(star_pos + 1);

        if (!prefix.empty() && path.find(prefix) == std::string::npos) {
            return false;
        }

        if (!suffix.empty() && path.find(suffix) == std::string::npos) {
            return false;
        }

        return true;
    }

    // Exact match or substring
    return path.find(pattern) != std::string::npos;
}

std::time_t Watcher::getFileModifiedTime(const std::string& file_path) {
    try {
        auto ftime = std::filesystem::last_write_time(file_path);
        return std::chrono::duration_cast<std::chrono::seconds>(
            ftime.time_since_epoch()).count();
    } catch (...) {
        return 0;
    }
}

bool Watcher::processPendingEvents() {
    if (pending_events_.empty()) {
        return false;
    }

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - last_event_time_);

    if (elapsed >= config_.debounce_delay) {
        // Trigger callback
        if (callback_) {
            callback_(pending_events_);
        }

        pending_events_.clear();
        return true;
    }

    return false;
}

void Watcher::addPendingEvent(const FileEvent& event) {
    pending_events_.push_back(event);
    printEvent(event);
}

void Watcher::printEvent(const FileEvent& event) const {
    std::cout << "→ File " << event.getTypeString() << ": " << event.file_path << "\n";
}

} // namespace build
} // namespace sad
