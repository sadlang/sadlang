// بسم الله الرحمن الرحيم
/**
 * @file cache_manager.h
 * @brief Build Cache Manager - مدير التخزين المؤقت للبناء
 * 
 * Manages build cache to speed up incremental builds by:
 * - Tracking source file hashes
 * - Storing compilation results
 * - Detecting file changes
 * - Invalidating outdated cache entries
 * 
 * Cache structure:
 * .sad_cache/
 * ├── cache.json          (metadata)
 * └── objects/            (compiled object files)
 *     ├── abc123.o
 *     └── def456.o
 * 
 * @author Sad Language Team
 * @date 2026-01-07
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <optional>

namespace sad {
namespace build {

/**
 * @brief Cache entry - مدخل التخزين المؤقت
 */
struct CacheEntry {
    std::string source_file;            ///< Source file path
    std::string source_hash;            ///< SHA-256 hash of source
    std::time_t timestamp;              ///< Last build timestamp
    std::string object_file;            ///< Cached object file path
    std::string compiler_version;       ///< Compiler version used
    std::string optimization;           ///< Optimization level used
    std::string target;                 ///< Target platform used
    std::vector<std::string> dependencies; ///< File dependencies
    
    /**
     * @brief Check if entry is valid
     * 
     * @param current_source_hash Current hash of source file
     * @param current_compiler_version Current compiler version
     * @param current_optimization Current optimization level
     * @param current_target Current target platform
     * @return true if cache entry is still valid
     */
    bool isValid(const std::string& current_source_hash,
                const std::string& current_compiler_version,
                const std::string& current_optimization,
                const std::string& current_target) const;
};

/**
 * @brief Cache statistics - إحصائيات التخزين المؤقت
 */
struct CacheStats {
    int total_entries = 0;              ///< Total cache entries
    int hits = 0;                       ///< Cache hits
    int misses = 0;                     ///< Cache misses
    int invalidated = 0;                ///< Invalidated entries
    size_t total_size_bytes = 0;        ///< Total cache size
    
    /**
     * @brief Get hit rate
     * @return Hit rate (0.0 - 1.0)
     */
    double getHitRate() const {
        int total = hits + misses;
        return total > 0 ? static_cast<double>(hits) / total : 0.0;
    }
};

/**
 * @brief Cache manager class - مدير التخزين المؤقت
 * 
 * Manages build cache for faster incremental builds.
 * 
 * Benefits:
 * - 5-10x faster rebuilds
 * - Only recompile changed files
 * - Track dependencies
 * - Detect configuration changes
 * 
 * Example usage:
 * @code
 * CacheManager cache(".sad_cache/");
 * cache.load();
 * 
 * std::string hash = cache.calculateHash("src/main.s");
 * if (cache.isCached("src/main.s", hash, "1.0.0", "full", "x86_64")) {
 *     std::string obj = cache.getCachedObjectFile("src/main.s");
 *     // Use cached object file
 * } else {
 *     // Compile and update cache
 *     compile("src/main.s", "build/main.o");
 *     cache.updateCache("src/main.s", hash, "build/main.o", 
 *                      "1.0.0", "full", "x86_64");
 * }
 * 
 * cache.save();
 * @endcode
 */
class CacheManager {
public:
    /**
     * @brief Constructor
     * 
     * @param cache_dir Cache directory (default: ".sad_cache/")
     */
    explicit CacheManager(const std::string& cache_dir = ".sad_cache/");

    /**
     * @brief Destructor - saves cache automatically
     */
    ~CacheManager();

    /**
     * @brief Load cache from disk
     * 
     * @return true if successful (false = empty cache)
     */
    bool load();

    /**
     * @brief Save cache to disk
     * 
     * @return true if successful
     */
    bool save();

    /**
     * @brief Check if file is cached and up-to-date
     * 
     * @param source_file Source file path
     * @param current_hash Current file hash
     * @param compiler_version Compiler version
     * @param optimization Optimization level
     * @param target Target platform
     * @return true if cached and valid
     */
    bool isCached(const std::string& source_file,
                 const std::string& current_hash,
                 const std::string& compiler_version,
                 const std::string& optimization,
                 const std::string& target) const;

    /**
     * @brief Get cached object file path
     * 
     * @param source_file Source file path
     * @return Optional object file path
     */
    std::optional<std::string> getCachedObjectFile(const std::string& source_file) const;

    /**
     * @brief Update cache with new compilation result
     * 
     * @param source_file Source file path
     * @param source_hash File hash
     * @param object_file Object file path
     * @param compiler_version Compiler version
     * @param optimization Optimization level
     * @param target Target platform
     * @param dependencies File dependencies
     */
    void updateCache(const std::string& source_file,
                    const std::string& source_hash,
                    const std::string& object_file,
                    const std::string& compiler_version,
                    const std::string& optimization,
                    const std::string& target,
                    const std::vector<std::string>& dependencies = {});

    /**
     * @brief Invalidate cache entry
     * 
     * @param source_file Source file path
     */
    void invalidate(const std::string& source_file);

    /**
     * @brief Invalidate all cache entries
     */
    void invalidateAll();

    /**
     * @brief Clear cache (remove all entries and files)
     * 
     * @return true if successful
     */
    bool clear();

    /**
     * @brief Calculate hash for file - حساب التجزئة للملف
     * 
     * Uses SHA-256 to hash file contents.
     * 
     * @param file_path File path
     * @return SHA-256 hash string
     */
    static std::string calculateHash(const std::string& file_path);

    /**
     * @brief Get cache statistics
     * 
     * @return CacheStats structure
     */
    const CacheStats& getStats() const { return stats_; }

    /**
     * @brief Get cache directory
     * 
     * @return Cache directory path
     */
    const std::string& getCacheDir() const { return cache_dir_; }

    /**
     * @brief Check if cache exists
     * 
     * @return true if cache directory and metadata exist
     */
    bool exists() const;

    /**
     * @brief Get cache size in bytes
     * 
     * @return Total size of cached files
     */
    size_t getCacheSize() const;

    /**
     * @brief Prune old cache entries
     * 
     * Removes entries older than specified days.
     * 
     * @param days_old Remove entries older than this
     * @return Number of entries removed
     */
    int pruneOldEntries(int days_old = 30);

private:
    std::string cache_dir_;                         ///< Cache directory
    std::map<std::string, CacheEntry> cache_;       ///< Cache entries
    CacheStats stats_;                              ///< Cache statistics
    bool modified_ = false;                         ///< Cache modified flag
    
    /**
     * @brief Get cache metadata file path
     * 
     * @return Path to cache.json
     */
    std::string getCacheMetadataPath() const;

    /**
     * @brief Get cache objects directory
     * 
     * @return Path to objects/
     */
    std::string getCacheObjectsDir() const;

    /**
     * @brief Parse cache JSON
     * 
     * @param json_content JSON string
     * @return true if successful
     */
    bool parseCache(const std::string& json_content);

    /**
     * @brief Serialize cache to JSON
     * 
     * @return JSON string
     */
    std::string serializeCache() const;

    /**
     * @brief Ensure cache directory exists
     */
    void ensureCacheDir();

    /**
     * @brief Record cache hit
     */
    void recordHit();

    /**
     * @brief Record cache miss
     */
    void recordMiss();

    /**
     * @brief Calculate directory size
     * 
     * @param dir_path Directory path
     * @return Size in bytes
     */
    static size_t calculateDirectorySize(const std::string& dir_path);
};

} // namespace build
} // namespace sad
