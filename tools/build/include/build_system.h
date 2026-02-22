// بسم الله الرحمن الرحيم
/**
 * @file build_system.h
 * @brief Build System for Sad Language Projects - نظام بناء مشاريع لغة ص
 * 
 * Build system for managing Sad Language project builds, similar to cargo build (Rust),
 * go build (Go), and npm scripts (Node.js).
 * 
 * @author Sad Language Team
 * @date 2026-01-07
 */

#pragma once

#include "config_parser.h"
#include "task_runner.h"
#include "builder.h"
#include "cache_manager.h"
#include <string>
#include <memory>
#include <vector>
#include <chrono>

namespace sad {
namespace build {

/**
 * @brief Build options - خيارات البناء
 */
struct BuildOptions {
    std::string target = "x86_64";      ///< Target platform (x86_64, arm64, wasm)
    std::string optimization = "none";  ///< Optimization level (none, basic, full)
    bool clean = false;                 ///< Clean build (rebuild everything)
    bool release = false;               ///< Release build (optimized)
    bool verbose = false;               ///< Verbose output
    bool use_cache = true;              ///< Use build cache
};

/**
 * @brief Build result - نتيجة البناء
 */
struct BuildResult {
    bool success = false;               ///< Build succeeded
    std::string output_path;            ///< Output binary path
    std::string error_message;          ///< Error message if failed
    double build_time_seconds = 0.0;    ///< Build time in seconds
    int files_compiled = 0;             ///< Number of files compiled
    int files_cached = 0;               ///< Number of files from cache
};

/**
 * @brief Run options - خيارات التشغيل
 */
struct RunOptions {
    std::vector<std::string> args;      ///< Program arguments
    bool watch = false;                 ///< Watch mode (auto-rebuild)
    std::string working_dir;            ///< Working directory
};

/**
 * @brief Test options - خيارات الاختبار
 */
struct TestOptions {
    std::string test_name;              ///< Specific test name (empty = all)
    bool coverage = false;              ///< Generate coverage report
    bool verbose = false;               ///< Verbose test output
};

/**
 * @brief Main build system class - الكلاس الرئيسي لنظام البناء
 * 
 * This is the main interface for the Sad build system. It orchestrates:
 * - Reading sad.toml configuration
 * - Building projects
 * - Running projects
 * - Testing
 * - Custom tasks
 * - Caching for faster builds
 * - Watch mode for auto-rebuild
 * 
 * Example usage:
 * @code
 * BuildSystem bs("sad.toml");
 * BuildResult result = bs.build();
 * if (result.success) {
 *     bs.run();
 * }
 * @endcode
 */
class BuildSystem {
public:
    /**
     * @brief Initialize build system
     * @param config_path Path to sad.toml configuration file
     * @throws std::runtime_error if config file not found
     */
    explicit BuildSystem(const std::string& config_path = "sad.toml");
    
    /**
     * @brief Destructor
     */
    ~BuildSystem();

    // Disable copy (use move semantics if needed)
    BuildSystem(const BuildSystem&) = delete;
    BuildSystem& operator=(const BuildSystem&) = delete;

    /**
     * @brief Build the project - بناء المشروع
     * 
     * Compiles all source files and links the final binary.
     * Uses caching to speed up incremental builds.
     * 
     * @param options Build options
     * @return BuildResult with success status and details
     * 
     * @code
     * BuildOptions opts;
     * opts.release = true;
     * opts.verbose = true;
     * BuildResult result = bs.build(opts);
     * @endcode
     */
    BuildResult build(const BuildOptions& options = BuildOptions{});

    /**
     * @brief Run the project - تشغيل المشروع
     * 
     * Runs the compiled binary. Will build first if needed.
     * 
     * @param options Run options (args, watch mode, etc.)
     * @return Exit code from the program
     * 
     * @code
     * RunOptions opts;
     * opts.args = {"--help"};
     * int exit_code = bs.run(opts);
     * @endcode
     */
    int run(const RunOptions& options = RunOptions{});

    /**
     * @brief Run tests - تشغيل الاختبارات
     * 
     * Runs project tests defined in sad.toml.
     * 
     * @param options Test options (name, coverage, etc.)
     * @return BuildResult with test results
     * 
     * @code
     * TestOptions opts;
     * opts.coverage = true;
     * BuildResult result = bs.test(opts);
     * @endcode
     */
    BuildResult test(const TestOptions& options = TestOptions{});

    /**
     * @brief Run custom task - تشغيل مهمة مخصصة
     * 
     * Runs a custom task defined in sad.toml [tasks] section.
     * 
     * @param task_name Name of the task to run
     * @return Exit code from the task
     * 
     * @code
     * int exit_code = bs.runTask("deploy");
     * @endcode
     */
    int runTask(const std::string& task_name);

    /**
     * @brief List all available tasks - سرد جميع المهام المتاحة
     * 
     * @return Vector of task names
     */
    std::vector<std::string> listTasks() const;

    /**
     * @brief Clean build artifacts - تنظيف مخرجات البناء
     * 
     * Removes build directory and cache.
     * 
     * @param all If true, also remove cache
     * @return true if successful
     * 
     * @code
     * bs.clean(true);  // Clean everything
     * @endcode
     */
    bool clean(bool all = false);

    /**
     * @brief Watch for changes - مراقبة التغييرات
     * 
     * Watches source files for changes and rebuilds automatically.
     * Blocks until interrupted (Ctrl+C).
     * 
     * @param paths Paths to watch (default: sources from sad.toml)
     * 
     * @code
     * bs.watch({"src/", "lib/"});
     * @endcode
     */
    void watch(const std::vector<std::string>& paths = {});

    /**
     * @brief Get project configuration
     * @return Reference to project config
     */
    const ProjectConfig& getConfig() const;

    /**
     * @brief Get build statistics
     * @return Build statistics string
     */
    std::string getBuildStats() const;

private:
    std::unique_ptr<ConfigParser> config_;      ///< Configuration parser
    std::unique_ptr<TaskRunner> task_runner_;   ///< Task executor
    std::unique_ptr<Builder> builder_;          ///< Build engine
    std::unique_ptr<CacheManager> cache_;       ///< Cache manager
    
    std::string config_path_;                   ///< Path to sad.toml
    BuildResult last_build_result_;             ///< Last build result
    
    /**
     * @brief Ensure project is built before running
     * @return true if build succeeded
     */
    bool ensureBuilt();
    
    /**
     * @brief Check if rebuild is needed
     * @return true if sources changed
     */
    bool needsRebuild() const;
    
    /**
     * @brief Print build summary
     */
    void printBuildSummary(const BuildResult& result) const;
    
    /**
     * @brief Handle build error
     */
    void handleBuildError(const std::string& error);
};

/**
 * @brief Get build system version
 * @return Version string (e.g., "1.0.0")
 */
std::string getBuildSystemVersion();

/**
 * @brief Print build system banner
 */
void printBanner();

} // namespace build
} // namespace sad
