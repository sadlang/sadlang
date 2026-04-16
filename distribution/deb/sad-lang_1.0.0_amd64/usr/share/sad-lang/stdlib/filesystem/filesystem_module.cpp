// بسم الله الرحمن الرحيم
/**
 * @file filesystem_module.cpp
 * @brief Implementation of Filesystem Module - تنفيذ وحدة نظام الملفات
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include <string>
#include "filesystem/filesystem_module.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <chrono>
#include <ctime>
#include <random>
#include <algorithm>
#include <regex>
#include <map>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #define getcwd _getcwd
    #define chdir _chdir
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/statvfs.h>
#endif

namespace fs = std::filesystem;

namespace sad {
namespace stdlib {
namespace filesystem {

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

namespace {
    void throw_if_not_exists(const std::string& path, const std::string& operation) {
        if (!fs::exists(path)) {
            throw std::runtime_error(operation + ": Path does not exist: " + path);
        }
    }

    void throw_on_filesystem_error(const std::string& operation, const std::string& path) {
        throw std::runtime_error(operation + " failed for: " + path);
    }
}

// ============================================================================
// File I/O Operations - عمليات قراءة/كتابة الملفات
// ============================================================================

std::string read_file(const std::string& path) {
    throw_if_not_exists(path, "read_file");

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw_on_filesystem_error("Failed to open file", path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> read_lines(const std::string& path) {
    throw_if_not_exists(path, "read_lines");

    std::ifstream file(path);
    if (!file.is_open()) {
        throw_on_filesystem_error("Failed to open file", path);
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::vector<uint8_t> read_bytes(const std::string& path) {
    throw_if_not_exists(path, "read_bytes");

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw_on_filesystem_error("Failed to open file", path);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw_on_filesystem_error("Failed to read file", path);
    }

    return buffer;
}

void write_file(const std::string& path, const std::string& content, bool truncate) {
    std::ios::openmode mode = std::ios::out | std::ios::binary;
    if (truncate) {
        mode |= std::ios::trunc;
    } else {
        mode |= std::ios::app;
    }

    std::ofstream file(path, mode);
    if (!file.is_open()) {
        throw_on_filesystem_error("Failed to open file for writing", path);
    }

    file << content;
    if (file.fail()) {
        throw_on_filesystem_error("Failed to write to file", path);
    }
}

void write_lines(const std::string& path, const std::vector<std::string>& lines) {
    std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        throw_on_filesystem_error("Failed to open file for writing", path);
    }

    for (const auto& line : lines) {
        file << line << '\n';
    }

    if (file.fail()) {
        throw_on_filesystem_error("Failed to write lines to file", path);
    }
}

void write_bytes(const std::string& path, const std::vector<uint8_t>& bytes) {
    std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        throw_on_filesystem_error("Failed to open file for writing", path);
    }

    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    if (file.fail()) {
        throw_on_filesystem_error("Failed to write bytes to file", path);
    }
}

void append_to_file(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::out | std::ios::binary | std::ios::app);
    if (!file.is_open()) {
        throw_on_filesystem_error("Failed to open file for appending", path);
    }

    file << content;
    if (file.fail()) {
        throw_on_filesystem_error("Failed to append to file", path);
    }
}

void copy_file(const std::string& source, const std::string& destination, bool overwrite) {
    throw_if_not_exists(source, "copy_file");

    auto options = overwrite ? fs::copy_options::overwrite_existing : fs::copy_options::none;

    std::error_code ec;
    fs::copy_file(source, destination, options, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to copy file from " + source + " to " + 
                               destination + ": " + ec.message());
    }
}

void move_file(const std::string& source, const std::string& destination) {
    throw_if_not_exists(source, "move_file");

    std::error_code ec;
    fs::rename(source, destination, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to move file from " + source + " to " + 
                               destination + ": " + ec.message());
    }
}

bool delete_file(const std::string& path) {
    if (!fs::exists(path)) {
        return false;
    }

    if (!fs::is_regular_file(path)) {
        throw std::runtime_error("Path is not a regular file: " + path);
    }

    std::error_code ec;
    return fs::remove(path, ec);
}

// ============================================================================
// Directory Operations - عمليات المجلدات
// ============================================================================

bool create_directory(const std::string& path, bool recursive) {
    std::error_code ec;
    
    if (recursive) {
        return fs::create_directories(path, ec);
    } else {
        return fs::create_directory(path, ec);
    }
}

std::vector<std::string> list_directory(const std::string& path) {
    throw_if_not_exists(path, "list_directory");

    if (!fs::is_directory(path)) {
        throw std::runtime_error("Path is not a directory: " + path);
    }

    std::vector<std::string> entries;
    for (const auto& entry : fs::directory_iterator(path)) {
        entries.push_back(entry.path().filename().string());
    }

    return entries;
}

std::vector<std::string> list_directory_full(const std::string& path) {
    throw_if_not_exists(path, "list_directory_full");

    if (!fs::is_directory(path)) {
        throw std::runtime_error("Path is not a directory: " + path);
    }

    std::vector<std::string> entries;
    for (const auto& entry : fs::directory_iterator(path)) {
        entries.push_back(entry.path().string());
    }

    return entries;
}

std::vector<std::string> list_directory_recursive(const std::string& path) {
    throw_if_not_exists(path, "list_directory_recursive");

    if (!fs::is_directory(path)) {
        throw std::runtime_error("Path is not a directory: " + path);
    }

    std::vector<std::string> entries;
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        entries.push_back(entry.path().string());
    }

    return entries;
}

void copy_directory(const std::string& source, const std::string& destination, bool recursive) {
    throw_if_not_exists(source, "copy_directory");

    if (!fs::is_directory(source)) {
        throw std::runtime_error("Source is not a directory: " + source);
    }

    auto options = recursive ? fs::copy_options::recursive : fs::copy_options::none;

    std::error_code ec;
    fs::copy(source, destination, options, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to copy directory from " + source + " to " + 
                               destination + ": " + ec.message());
    }
}

bool remove_directory(const std::string& path, bool recursive) {
    if (!fs::exists(path)) {
        return false;
    }

    if (!fs::is_directory(path)) {
        throw std::runtime_error("Path is not a directory: " + path);
    }

    std::error_code ec;
    
    if (recursive) {
        return fs::remove_all(path, ec) > 0;
    } else {
        return fs::remove(path, ec);
    }
}

std::string get_current_directory() {
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) != nullptr) {
        return std::string(buffer);
    }
    throw std::runtime_error("Failed to get current directory");
}

bool change_directory(const std::string& path) {
    throw_if_not_exists(path, "change_directory");

    if (!fs::is_directory(path)) {
        throw std::runtime_error("Path is not a directory: " + path);
    }

    return chdir(path.c_str()) == 0;
}

// ============================================================================
// Path Manipulation - معالجة المسارات
// ============================================================================

std::string join_path(const std::vector<std::string>& components) {
    if (components.empty()) {
        return "";
    }

    fs::path result = components[0];
    for (size_t i = 1; i < components.size(); i++) {
        result /= components[i];
    }

    return result.string();
}

std::string get_absolute_path(const std::string& path) {
    std::error_code ec;
    auto absolute = fs::absolute(path, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to get absolute path: " + ec.message());
    }

    return absolute.string();
}

std::string get_relative_path(const std::string& path, const std::string& base) {
    std::error_code ec;
    auto relative = fs::relative(path, base, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to get relative path: " + ec.message());
    }

    return relative.string();
}

std::string normalize_path(const std::string& path) {
    fs::path p(path);
    return p.lexically_normal().string();
}

std::string get_filename(const std::string& path) {
    fs::path p(path);
    return p.filename().string();
}

std::string get_extension(const std::string& path) {
    fs::path p(path);
    return p.extension().string();
}

std::string get_stem(const std::string& path) {
    fs::path p(path);
    return p.stem().string();
}

std::string get_parent_path(const std::string& path) {
    fs::path p(path);
    return p.parent_path().string();
}

std::string replace_extension(const std::string& path, const std::string& new_extension) {
    fs::path p(path);
    p.replace_extension(new_extension);
    return p.string();
}

// ============================================================================
// File Information - معلومات الملف
// ============================================================================

bool exists(const std::string& path) {
    return fs::exists(path);
}

bool is_file(const std::string& path) {
    return fs::is_regular_file(path);
}

bool is_directory(const std::string& path) {
    return fs::is_directory(path);
}

bool is_symlink(const std::string& path) {
    return fs::is_symlink(path);
}

uint64_t get_file_size(const std::string& path) {
    throw_if_not_exists(path, "get_file_size");

    std::error_code ec;
    auto size = fs::file_size(path, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to get file size: " + ec.message());
    }

    return static_cast<uint64_t>(size);
}

int64_t get_last_write_time(const std::string& path) {
    throw_if_not_exists(path, "get_last_write_time");

    std::error_code ec;
    auto ftime = fs::last_write_time(path, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to get last write time: " + ec.message());
    }

    // Convert to seconds since epoch
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    
    return std::chrono::duration_cast<std::chrono::seconds>(
        sctp.time_since_epoch()
    ).count();
}

uint64_t get_available_space(const std::string& path) {
    throw_if_not_exists(path, "get_available_space");

    std::error_code ec;
    auto space_info = fs::space(path, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to get disk space: " + ec.message());
    }

    return static_cast<uint64_t>(space_info.available);
}

uint64_t get_total_space(const std::string& path) {
    throw_if_not_exists(path, "get_total_space");

    std::error_code ec;
    auto space_info = fs::space(path, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to get disk space: " + ec.message());
    }

    return static_cast<uint64_t>(space_info.capacity);
}

// ============================================================================
// File Permissions - صلاحيات الملفات
// ============================================================================

bool is_readable(const std::string& path) {
    throw_if_not_exists(path, "is_readable");

    std::error_code ec;
    auto perms = fs::status(path, ec).permissions();
    
    if (ec) {
        return false;
    }

    return (perms & fs::perms::owner_read) != fs::perms::none ||
           (perms & fs::perms::group_read) != fs::perms::none ||
           (perms & fs::perms::others_read) != fs::perms::none;
}

bool is_writable(const std::string& path) {
    throw_if_not_exists(path, "is_writable");

    std::error_code ec;
    auto perms = fs::status(path, ec).permissions();
    
    if (ec) {
        return false;
    }

    return (perms & fs::perms::owner_write) != fs::perms::none ||
           (perms & fs::perms::group_write) != fs::perms::none ||
           (perms & fs::perms::others_write) != fs::perms::none;
}

bool is_executable(const std::string& path) {
    throw_if_not_exists(path, "is_executable");

    std::error_code ec;
    auto perms = fs::status(path, ec).permissions();
    
    if (ec) {
        return false;
    }

    return (perms & fs::perms::owner_exec) != fs::perms::none ||
           (perms & fs::perms::group_exec) != fs::perms::none ||
           (perms & fs::perms::others_exec) != fs::perms::none;
}

void set_permissions(const std::string& path, bool readable, bool writable, bool executable) {
    throw_if_not_exists(path, "set_permissions");

    fs::perms perms = fs::perms::none;

    if (readable) {
        perms |= fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read;
    }

    if (writable) {
        perms |= fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write;
    }

    if (executable) {
        perms |= fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec;
    }

    std::error_code ec;
    fs::permissions(path, perms, fs::perm_options::replace, ec);
    
    if (ec) {
        throw std::runtime_error("Failed to set permissions: " + ec.message());
    }
}

// ============================================================================
// Temporary Files - الملفات المؤقتة
// ============================================================================

std::string get_temp_directory() {
    return fs::temp_directory_path().string();
}

std::string create_temp_file(const std::string& prefix) {
    auto temp_dir = fs::temp_directory_path();
    
    // Generate random suffix
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 999999);
    
    for (int attempts = 0; attempts < 100; attempts++) {
        std::string filename = prefix + std::to_string(dis(gen)) + ".tmp";
        fs::path temp_file = temp_dir / filename;
        
        if (!fs::exists(temp_file)) {
            // Create empty file
            std::ofstream file(temp_file);
            file.close();
            return temp_file.string();
        }
    }
    
    throw std::runtime_error("Failed to create temporary file");
}

std::string create_temp_directory(const std::string& prefix) {
    auto temp_dir = fs::temp_directory_path();
    
    // Generate random suffix
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 999999);
    
    for (int attempts = 0; attempts < 100; attempts++) {
        std::string dirname = prefix + std::to_string(dis(gen));
        fs::path temp_subdir = temp_dir / dirname;
        
        if (!fs::exists(temp_subdir)) {
            std::error_code ec;
            if (fs::create_directory(temp_subdir, ec)) {
                return temp_subdir.string();
            }
        }
    }
    
    throw std::runtime_error("Failed to create temporary directory");
}

// ============================================================================
// File Watching - مراقبة الملفات
// ============================================================================

struct FileWatcher::Impl {
    std::string path;
    bool recursive;
    bool running;
    std::map<std::string, int64_t> file_times;
    std::vector<std::pair<Event, std::string>> events;

    Impl(const std::string& p, bool rec) 
        : path(p), recursive(rec), running(false) {}

    void scan_directory() {
        if (!fs::exists(path)) {
            return;
        }

        std::map<std::string, int64_t> current_times;

        auto scan = [&](const fs::path& entry_path) {
            if (fs::is_regular_file(entry_path)) {
                auto last_write = get_last_write_time(entry_path.string());
                std::string key = entry_path.string();
                current_times[key] = last_write;

                // Check for changes
                if (file_times.count(key)) {
                    if (file_times[key] != last_write) {
                        events.push_back({Event::Modified, key});
                    }
                } else {
                    events.push_back({Event::Created, key});
                }
            }
        };

        if (recursive) {
            for (const auto& entry : fs::recursive_directory_iterator(path)) {
                scan(entry.path());
            }
        } else {
            for (const auto& entry : fs::directory_iterator(path)) {
                scan(entry.path());
            }
        }

        // Check for deleted files
        for (const auto& [file_path, _] : file_times) {
            if (!current_times.count(file_path)) {
                events.push_back({Event::Deleted, file_path});
            }
        }

        file_times = std::move(current_times);
    }
};

FileWatcher::FileWatcher(const std::string& path, bool recursive)
    : impl_(std::make_unique<Impl>(path, recursive)) {
}

FileWatcher::~FileWatcher() {
    stop();
}

void FileWatcher::start() {
    impl_->running = true;
    impl_->scan_directory(); // Initial scan
}

void FileWatcher::stop() {
    impl_->running = false;
}

std::vector<std::pair<FileWatcher::Event, std::string>> FileWatcher::poll_events() {
    if (!impl_->running) {
        return {};
    }

    impl_->scan_directory();
    
    auto events = std::move(impl_->events);
    impl_->events.clear();
    
    return events;
}

// ============================================================================
// Glob Pattern Matching - مطابقة الأنماط
// ============================================================================

namespace {
    bool matches_glob_pattern(const std::string& str, const std::string& pattern) {
        // Convert glob pattern to regex
        std::string regex_pattern;
        regex_pattern.reserve(pattern.size() * 2);

        for (char c : pattern) {
            switch (c) {
                case '*':
                    regex_pattern += ".*";
                    break;
                case '?':
                    regex_pattern += ".";
                    break;
                case '.':
                case '+':
                case '^':
                case '$':
                case '(':
                case ')':
                case '[':
                case ']':
                case '{':
                case '}':
                case '|':
                case '\\':
                    regex_pattern += '\\';
                    regex_pattern += c;
                    break;
                default:
                    regex_pattern += c;
                    break;
            }
        }

        try {
            std::regex re(regex_pattern);
            return std::regex_match(str, re);
        } catch (...) {
            return false;
        }
    }
}

std::vector<std::string> glob(const std::string& pattern, const std::string& base_path) {
    if (!fs::exists(base_path)) {
        return {};
    }

    std::vector<std::string> matches;

    for (const auto& entry : fs::recursive_directory_iterator(base_path)) {
        if (fs::is_regular_file(entry)) {
            std::string filename = entry.path().filename().string();
            if (matches_glob_pattern(filename, pattern)) {
                matches.push_back(entry.path().string());
            }
        }
    }

    return matches;
}

bool matches_pattern(const std::string& path, const std::string& pattern) {
    fs::path p(path);
    std::string filename = p.filename().string();
    return matches_glob_pattern(filename, pattern);
}

} // namespace filesystem
} // namespace stdlib
} // namespace sad
