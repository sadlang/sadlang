// بسم الله الرحمن الرحيم
/**
 * @file firebase_module_loader.h
 * @brief Firebase Module Loader for LSP Server (Optional CURL Support)
 * 
 * تحميل وتنزيل الوحدات من Firebase Project (slangserver-f3fd0)
 * Loading and downloading modules from Firebase Project (slangserver-f3fd0)
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <ctime>
#include <nlohmann/json.hpp>

namespace sad {
namespace lsp {

using json = nlohmann::json;
namespace fs = std::filesystem;

struct FirebaseConfig {
    std::string project_id = "slangserver-f3fd0";
    std::string firestore_url = "https://firestore.googleapis.com/v1/projects/slangserver-f3fd0/databases/(default)/documents";
    std::string storage_bucket = "slangserver-f3fd0.appspot.com";
    std::string cache_dir = ".sad/modules_cache";
    int timeout_seconds = 30;
    int max_retries = 3;
};

struct ModuleInfo {
    std::string id;
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    std::string storage_path;
    std::string checksum;
    std::string last_modified;
    bool is_cached = false;
    
    json to_json() const {
        json j;
        j["id"] = id;
        j["name"] = name;
        j["version"] = version;
        j["description"] = description;
        j["author"] = author;
        j["license"] = license;
        j["dependencies"] = dependencies;
        j["storagePath"] = storage_path;
        j["checksum"] = checksum;
        j["lastModified"] = last_modified;
        j["isCached"] = is_cached;
        return j;
    }
};

struct CacheStats {
    size_t total_size = 0;
    int file_count = 0;
    std::string oldest_file;
    std::string newest_file;
    
    json to_json() const {
        json j;
        j["totalSize"] = total_size;
        j["fileCount"] = file_count;
        j["oldestFile"] = oldest_file;
        j["newestFile"] = newest_file;
        return j;
    }
};

class FirebaseModuleLoader {
private:
    FirebaseConfig config_;
    std::vector<ModuleInfo> module_cache_;
    
    bool ensure_cache_dir() {
        try {
            fs::create_directories(config_.cache_dir);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
public:
    FirebaseModuleLoader(const FirebaseConfig& config = FirebaseConfig()) 
        : config_(config) {
        ensure_cache_dir();
    }
    
    json load_available_modules() {
        return json{
            {"status", "pending"},
            {"message", "Firebase module loading requires CURL enabled. Use -DENABLE_FIREBASE_MODULES=ON"},
            {"projectId", config_.project_id}
        };
    }
    
    json search_modules(const std::string& query) {
        return json{
            {"status", "pending"},
            {"message", "Search requires Firebase CURL support"},
            {"query", query}
        };
    }
    
    json get_module(const std::string& module_id) {
        return json{
            {"status", "pending"},
            {"message", "Module loading requires Firebase CURL support"},
            {"moduleId", module_id}
        };
    }
    
    json download_module(const ModuleInfo& module, const std::string& local_path) {
        return json{
            {"status", "pending"},
            {"message", "Download requires Firebase CURL support"},
            {"localPath", local_path}
        };
    }
    
    json load_module_content(const std::string& module_id, const std::string& version = "latest") {
        std::string cache_path = config_.cache_dir + "/" + module_id + "_" + version + ".sad";
        
        if (fs::exists(cache_path)) {
            try {
                std::ifstream file(cache_path);
                std::stringstream buffer;
                buffer << file.rdbuf();
                return json{
                    {"success", true}, 
                    {"content", buffer.str()}, 
                    {"cached", true},
                    {"path", cache_path}
                };
            } catch (const std::exception& e) {
                return json{{"error", std::string("Cache read error: ") + e.what()}};
            }
        }
        
        return json{
            {"status", "pending"},
            {"message", "Module not in cache. Use ENABLE_FIREBASE_MODULES to download."},
            {"moduleId", module_id},
            {"version", version}
        };
    }
    
    json clear_cache() {
        try {
            int count = 0;
            if (fs::exists(config_.cache_dir)) {
                for (const auto& entry : fs::directory_iterator(config_.cache_dir)) {
                    if (fs::is_regular_file(entry)) {
                        fs::remove(entry);
                        count++;
                    }
                }
            }
            return json{{"success", true}, {"filesRemoved", count}};
        } catch (const std::exception& e) {
            return json{{"error", std::string("Clear cache error: ") + e.what()}};
        }
    }
    
    json get_cache_stats() {
        try {
            CacheStats stats;
            
            if (!fs::exists(config_.cache_dir)) {
                return json{{"success", true}, stats.to_json()};
            }
            
            std::string oldest, newest;
            std::time_t oldest_time = std::time_t(LLONG_MAX);
            std::time_t newest_time = 0;
            
            for (const auto& entry : fs::directory_iterator(config_.cache_dir)) {
                if (fs::is_regular_file(entry)) {
                    stats.file_count++;
                    stats.total_size += fs::file_size(entry);
                    
                    auto last_write = fs::last_write_time(entry);
                    auto duration = last_write.time_since_epoch();
                    auto time_t_val = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
                    
                    if (time_t_val < oldest_time) {
                        oldest_time = time_t_val;
                        oldest = entry.path().filename().string();
                    }
                    if (time_t_val > newest_time) {
                        newest_time = time_t_val;
                        newest = entry.path().filename().string();
                    }
                }
            }
            
            stats.oldest_file = oldest;
            stats.newest_file = newest;
            return json{{"success", true}, stats.to_json()};
        } catch (const std::exception& e) {
            return json{{"error", std::string("Stats error: ") + e.what()}};
        }
    }
};

} // namespace lsp
} // namespace sad
