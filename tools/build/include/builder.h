// بسم الله الرحمن الرحيم
/**
 * @file builder.h
 * @brief Build Engine - محرك البناء
 * 
 * Core build engine that compiles Sad source files and links binaries.
 * Integrates with the Sad compiler and handles:
 * - Source file discovery
 * - Compilation
 * - Linking
 * - Dependency management
 * - Incremental builds
 * 
 * @author Sad Language Team
 * @date 2026-01-07
 */

#pragma once

#include "config_parser.h"
#include "cache_manager.h"
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <optional>

namespace sad {
namespace build {

/**
 * @brief Source file info - معلومات ملف المصدر
 */
struct SourceFile {
    std::string path;               ///< File path
    std::string absolute_path;      ///< Absolute file path
    bool is_cached = false;         ///< File is up-to-date in cache
    std::time_t last_modified;      ///< Last modification time
    std::vector<std::string> dependencies; ///< Dependencies (imports)
};

/**
 * @brief Compilation result - نتيجة الترجمة
 */
struct CompilationResult {
    bool success = false;           ///< Compilation succeeded
    std::string source_file;        ///< Source file compiled
    std::string object_file;        ///< Output object file
    std::string error_message;      ///< Error message if failed
    double compile_time_seconds = 0.0; ///< Compilation time
};

/**
 * @brief Link result - نتيجة الربط
 */
struct LinkResult {
    bool success = false;           ///< Linking succeeded
    std::string output_file;        ///< Output binary
    std::string error_message;      ///< Error message if failed
    double link_time_seconds = 0.0; ///< Link time
};

/**
 * @brief Build statistics - إحصائيات البناء
 */
struct BuildStats {
    int total_files = 0;            ///< Total source files
    int files_compiled = 0;         ///< Files compiled
    int files_cached = 0;           ///< Files from cache
    int files_failed = 0;           ///< Files failed to compile
    double total_time_seconds = 0.0;///< Total build time
    double compile_time = 0.0;      ///< Compilation time
    double link_time = 0.0;         ///< Link time
};

/**
 * @brief Builder class - محرك البناء
 * 
 * Handles compilation and linking of Sad projects.
 * 
 * Build process:
 * 1. Discover source files
 * 2. Check cache for each file
 * 3. Compile changed files
 * 4. Link all object files
 * 5. Update cache
 * 
 * Example usage:
 * @code
 * Builder builder(config, cache_manager);
 * builder.setOptimization("full");
 * builder.setTarget("x86_64");
 * 
 * BuildResult result = builder.build();
 * if (result.success) {
 *     std::cout << "Build successful: " << result.output_path << "\n";
 * }
 * @endcode
 */
class Builder {
public:
    /**
     * @brief Constructor
     * 
     * @param config Project configuration
     * @param cache_manager Cache manager (optional)
     */
    explicit Builder(const ProjectConfig& config, 
                    CacheManager* cache_manager = nullptr);

    /**
     * @brief Set optimization level
     * 
     * @param level Optimization level ("none", "basic", "full")
     */
    void setOptimization(const std::string& level);

    /**
     * @brief Set target platform
     * 
     * @param target Target platform ("x86_64", "arm64", "wasm")
     */
    void setTarget(const std::string& target);

    /**
     * @brief Set verbose output
     * 
     * @param verbose Enable verbose output
     */
    void setVerbose(bool verbose);

    /**
     * @brief Set clean build flag
     * 
     * @param clean Force rebuild of all files
     */
    void setClean(bool clean);

    /**
     * @brief Build the project - بناء المشروع
     * 
     * Complete build process: compile + link
     * 
     * @return BuildResult with status and output path
     */
    BuildResult build();

    /**
     * @brief Compile only (no linking)
     * 
     * @return Vector of CompilationResult for each file
     */
    std::vector<CompilationResult> compile();

    /**
     * @brief Link object files
     * 
     * @param object_files Object files to link
     * @return LinkResult with output path
     */
    LinkResult link(const std::vector<std::string>& object_files);

    /**
     * @brief Get source files
     * 
     * Discovers all source files in configured source directories.
     * 
     * @return Vector of SourceFile
     */
    std::vector<SourceFile> getSourceFiles() const;

    /**
     * @brief Get build statistics
     * 
     * @return BuildStats structure
     */
    const BuildStats& getStats() const { return stats_; }

    /**
     * @brief Check if rebuild is needed
     * 
     * @return true if any source file changed
     */
    bool needsRebuild() const;

private:
    const ProjectConfig& config_;       ///< Project configuration
    CacheManager* cache_manager_;       ///< Cache manager
    
    std::string optimization_ = "none"; ///< Optimization level
    std::string target_ = "x86_64";     ///< Target platform
    bool verbose_ = false;              ///< Verbose output
    bool clean_ = false;                ///< Clean build
    
    BuildStats stats_;                  ///< Build statistics
    
    /**
     * @brief Discover source files recursively
     * 
     * @param directory Directory to search
     * @param files Output vector
     */
    void discoverSourceFiles(const std::string& directory, 
                           std::vector<SourceFile>& files) const;

    /**
     * @brief Compile single file
     * 
     * @param source Source file
     * @return CompilationResult
     */
    CompilationResult compileFile(const SourceFile& source);

    /**
     * @brief Get compiler command
     * 
     * @param source_file Source file path
     * @param output_file Output object file path
     * @return Command string
     */
    std::string getCompilerCommand(const std::string& source_file,
                                   const std::string& output_file) const;

    /**
     * @brief Get linker command
     * 
     * @param object_files Object files to link
     * @param output_file Output binary path
     * @return Command string
     */
    std::string getLinkerCommand(const std::vector<std::string>& object_files,
                                const std::string& output_file) const;

    /**
     * @brief Execute command
     * 
     * @param command Command to execute
     * @param capture_output Capture stdout/stderr
     * @return Exit code
     */
    int executeCommand(const std::string& command, bool capture_output = true);

    /**
     * @brief Get object file path for source file
     * 
     * @param source_file Source file path
     * @return Object file path
     */
    std::string getObjectFilePath(const std::string& source_file) const;

    /**
     * @brief Get output binary path
     * 
     * @return Binary path
     */
    std::string getOutputPath() const;

    /**
     * @brief Check if file needs recompilation
     * 
     * @param source Source file
     * @return true if needs recompilation
     */
    bool needsRecompilation(const SourceFile& source) const;

    /**
     * @brief Print compilation progress
     * 
     * @param current Current file number
     * @param total Total files
     * @param file_name File name
     */
    void printCompileProgress(int current, int total, 
                             const std::string& file_name) const;

    /**
     * @brief Print build error
     * 
     * @param error Error message
     */
    void printError(const std::string& error) const;
};

/**
 * @brief Build result structure
 */
struct BuildResult {
    bool success = false;
    std::string output_path;
    std::string error_message;
    double build_time_seconds = 0.0;
    int files_compiled = 0;
    int files_cached = 0;
    BuildStats stats;
};

} // namespace build
} // namespace sad
