// بسم الله الرحمن الرحيم
/**
 * @file cache_manager.cpp
 * @brief Build Cache Manager Implementation
 */

#include "../include/cache_manager.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <openssl/sha.h>

namespace sad {
namespace build {

CacheManager::CacheManager(const std::string& cache_dir)
    : cache_dir_(cache_dir) {
    ensureCacheDir();
}

CacheManager::~CacheManager() {
    if (modified_) {
        save();
    }
}

bool CacheManager::load() {
    std::string cache_file = getCacheMetadataPath();
    
    if (!std::filesystem::exists(cache_file)) {
        return false; // Empty cache
    }

    std::ifstream file(cache_file);
    if (!file.is_open()) {
        return false;
    }

    // Simple JSON parsing (for production, use nlohmann/json)
    std::string line;
    CacheEntry current_entry;
    bool in_entry = false;

    while (std::getline(file, line)) {
        if (line.find("\"source_file\":") != std::string::npos) {
            size_t start = line.find('"', line.find(':') + 1) + 1;
            size_t end = line.find('"', start);
            current_entry.source_file = line.substr(start, end - start);
            in_entry = true;
        } else if (line.find("\"source_hash\":") != std::string::npos) {
            size_t start = line.find('"', line.find(':') + 1) + 1;
            size_t end = line.find('"', start);
            current_entry.source_hash = line.substr(start, end - start);
        } else if (line.find("\"object_file\":") != std::string::npos) {
            size_t start = line.find('"', line.find(':') + 1) + 1;
            size_t end = line.find('"', start);
            current_entry.object_file = line.substr(start, end - start);
        } else if (line.find("\"compiler_version\":") != std::string::npos) {
            size_t start = line.find('"', line.find(':') + 1) + 1;
            size_t end = line.find('"', start);
            current_entry.compiler_version = line.substr(start, end - start);
        } else if (line.find("\"optimization\":") != std::string::npos) {
            size_t start = line.find('"', line.find(':') + 1) + 1;
            size_t end = line.find('"', start);
            current_entry.optimization = line.substr(start, end - start);
        } else if (line.find("\"target\":") != std::string::npos) {
            size_t start = line.find('"', line.find(':') + 1) + 1;
            size_t end = line.find('"', start);
            current_entry.target = line.substr(start, end - start);
        } else if (line.find("}") != std::string::npos && in_entry) {
            cache_[current_entry.source_file] = current_entry;
            current_entry = CacheEntry{};
            in_entry = false;
        }
    }

    stats_.total_entries = cache_.size();
    return true;
}

bool CacheManager::save() {
    std::string cache_file = getCacheMetadataPath();
    
    std::ofstream file(cache_file);
    if (!file.is_open()) {
        return false;
    }

    file << "{\n";
    file << "  \"version\": \"1.0.0\",\n";
    file << "  \"entries\": [\n";

    bool first = true;
    for (const auto& [path, entry] : cache_) {
        if (!first) file << ",\n";
        first = false;

        file << "    {\n";
        file << "      \"source_file\": \"" << entry.source_file << "\",\n";
        file << "      \"source_hash\": \"" << entry.source_hash << "\",\n";
        file << "      \"object_file\": \"" << entry.object_file << "\",\n";
        file << "      \"compiler_version\": \"" << entry.compiler_version << "\",\n";
        file << "      \"optimization\": \"" << entry.optimization << "\",\n";
        file << "      \"target\": \"" << entry.target << "\"\n";
        file << "    }";
    }

    file << "\n  ]\n";
    file << "}\n";

    modified_ = false;
    return true;
}

bool CacheManager::isCached(const std::string& source_file,
                            const std::string& current_hash,
                            const std::string& compiler_version,
                            const std::string& optimization,
                            const std::string& target) const {
    auto it = cache_.find(source_file);
    if (it == cache_.end()) {
        return false;
    }

    bool valid = it->second.isValid(current_hash, compiler_version, optimization, target);
    
    if (valid) {
        const_cast<CacheManager*>(this)->recordHit();
    } else {
        const_cast<CacheManager*>(this)->recordMiss();
    }

    return valid;
}

std::optional<std::string> CacheManager::getCachedObjectFile(const std::string& source_file) const {
    auto it = cache_.find(source_file);
    if (it != cache_.end()) {
        return it->second.object_file;
    }
    return std::nullopt;
}

void CacheManager::updateCache(const std::string& source_file,
                               const std::string& source_hash,
                               const std::string& object_file,
                               const std::string& compiler_version,
                               const std::string& optimization,
                               const std::string& target,
                               const std::vector<std::string>& dependencies) {
    CacheEntry entry;
    entry.source_file = source_file;
    entry.source_hash = source_hash;
    entry.object_file = object_file;
    entry.compiler_version = compiler_version;
    entry.optimization = optimization;
    entry.target = target;
    entry.dependencies = dependencies;
    entry.timestamp = std::time(nullptr);

    cache_[source_file] = entry;
    modified_ = true;
    stats_.total_entries = cache_.size();
}

void CacheManager::invalidate(const std::string& source_file) {
    auto it = cache_.find(source_file);
    if (it != cache_.end()) {
        cache_.erase(it);
        modified_ = true;
        stats_.invalidated++;
        stats_.total_entries = cache_.size();
    }
}

void CacheManager::invalidateAll() {
    cache_.clear();
    modified_ = true;
    stats_.invalidated += stats_.total_entries;
    stats_.total_entries = 0;
}

bool CacheManager::clear() {
    cache_.clear();
    modified_ = true;
    stats_ = CacheStats{};

    // Remove cache files
    try {
        std::filesystem::remove_all(cache_dir_);
        ensureCacheDir();
        return true;
    } catch (...) {
        return false;
    }
}

std::string CacheManager::calculateHash(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }

    // Read file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Calculate SHA-256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(content.c_str()), 
           content.length(), hash);

    // Convert to hex string
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') 
           << static_cast<int>(hash[i]);
    }

    return ss.str();
}

bool CacheManager::exists() const {
    return std::filesystem::exists(cache_dir_) && 
           std::filesystem::exists(getCacheMetadataPath());
}

size_t CacheManager::getCacheSize() const {
    return calculateDirectorySize(cache_dir_);
}

int CacheManager::pruneOldEntries(int days_old) {
    int removed = 0;
    std::time_t cutoff = std::time(nullptr) - (days_old * 24 * 60 * 60);

    auto it = cache_.begin();
    while (it != cache_.end()) {
        if (it->second.timestamp < cutoff) {
            it = cache_.erase(it);
            removed++;
        } else {
            ++it;
        }
    }

    if (removed > 0) {
        modified_ = true;
        stats_.total_entries = cache_.size();
    }

    return removed;
}

std::string CacheManager::getCacheMetadataPath() const {
    return cache_dir_ + "/cache.json";
}

std::string CacheManager::getCacheObjectsDir() const {
    return cache_dir_ + "/objects";
}

void CacheManager::ensureCacheDir() {
    std::filesystem::create_directories(cache_dir_);
    std::filesystem::create_directories(getCacheObjectsDir());
}

void CacheManager::recordHit() {
    stats_.hits++;
}

void CacheManager::recordMiss() {
    stats_.misses++;
}

size_t CacheManager::calculateDirectorySize(const std::string& dir_path) {
    size_t size = 0;
    
    if (!std::filesystem::exists(dir_path)) {
        return 0;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
        if (entry.is_regular_file()) {
            size += entry.file_size();
        }
    }

    return size;
}

bool CacheEntry::isValid(const std::string& current_source_hash,
                        const std::string& current_compiler_version,
                        const std::string& current_optimization,
                        const std::string& current_target) const {
    return source_hash == current_source_hash &&
           compiler_version == current_compiler_version &&
           optimization == current_optimization &&
           target == current_target &&
           std::filesystem::exists(object_file);
}

} // namespace build
} // namespace sad
