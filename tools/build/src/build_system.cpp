// بسم الله الرحمن الرحيم
/**
 * @file build_system.cpp
 * @brief Build System Implementation
 */

#include "../include/build_system.h"
#include <iostream>
#include <chrono>

namespace sad {
namespace build {

BuildSystem::BuildSystem(const std::string& config_path)
    : config_path_(config_path) {
    
    // Initialize components
    config_ = std::make_unique<ConfigParser>(config_path);
    
    if (!config_->parse()) {
        throw std::runtime_error("Failed to parse configuration: " + config_path);
    }

    cache_ = std::make_unique<CacheManager>(".sad_cache");
    cache_->load();

    builder_ = std::make_unique<Builder>(config_->getConfig(), cache_.get());

    task_runner_ = std::make_unique<TaskRunner>();
    
    // Register tasks from config
    for (const auto& [name, task_config] : config_->getTasks()) {
        Task task;
        task.name = task_config.name;
        task.description = task_config.description;
        task.command = task_config.command;
        task.args = task_config.args;
        task.depends = task_config.depends;
        task.is_background = task_config.is_background;
        
        task_runner_->registerTask(task);
    }

    // Set variables for task runner
    std::map<std::string, std::string> vars;
    vars["main"] = config_->getConfig().build.main;
    vars["output"] = config_->getConfig().build.output;
    vars["name"] = config_->getConfig().package.name;
    vars["version"] = config_->getConfig().package.version;
    task_runner_->setVariables(vars);
}

BuildSystem::~BuildSystem() {
    if (cache_) {
        cache_->save();
    }
}

BuildResult BuildSystem::build(const BuildOptions& options) {
    std::cout << "Building " << config_->getConfig().package.name 
              << " v" << config_->getConfig().package.version << "...\n";

    auto start = std::chrono::high_resolution_clock::now();

    // Configure builder
    builder_->setOptimization(options.release ? "full" : options.optimization);
    builder_->setTarget(options.target);
    builder_->setVerbose(options.verbose);
    builder_->setClean(options.clean);

    // Build
    BuildResult result = builder_->build();
    
    auto end = std::chrono::high_resolution_clock::now();
    result.build_time_seconds = 
        std::chrono::duration<double>(end - start).count();

    last_build_result_ = result;

    printBuildSummary(result);

    return result;
}

int BuildSystem::run(const RunOptions& options) {
    // Ensure project is built
    if (!ensureBuilt()) {
        std::cerr << "Build failed, cannot run\n";
        return 1;
    }

    // Get output path
    std::string output_path = last_build_result_.output_path;
    
    if (options.watch) {
        // Watch mode: rebuild and run on changes
        std::cout << "Watch mode enabled. Press Ctrl+C to stop.\n";
        watch({});
        return 0;
    }

    // Build command with arguments
    std::string cmd = output_path;
    for (const auto& arg : options.args) {
        cmd += " " + arg;
    }

    std::cout << "Running " << config_->getConfig().package.name << "...\n";
    
    return std::system(cmd.c_str());
}

BuildResult BuildSystem::test(const TestOptions& options) {
    std::cout << "Running tests...\n";

    // Check if test task exists
    if (task_runner_->hasTask("test")) {
        TaskExecutionOptions exec_opts;
        exec_opts.verbose = options.verbose;

        TaskResult result = task_runner_->run("test", exec_opts);

        BuildResult build_result;
        build_result.success = result.success;
        build_result.error_message = result.error;
        build_result.build_time_seconds = result.execution_time_seconds;

        return build_result;
    }

    // Default: run tests from tests/ directory
    BuildOptions build_opts;
    build_opts.verbose = options.verbose;

    return build(build_opts);
}

int BuildSystem::runTask(const std::string& task_name) {
    if (!task_runner_->hasTask(task_name)) {
        std::cerr << "Task not found: " << task_name << "\n";
        return 1;
    }

    TaskExecutionOptions options;
    options.verbose = true;

    TaskResult result = task_runner_->run(task_name, options);

    return result.success ? 0 : 1;
}

std::vector<std::string> BuildSystem::listTasks() const {
    std::vector<std::string> task_names;
    for (const auto& task : task_runner_->listTasks()) {
        task_names.push_back(task.name);
    }
    return task_names;
}

bool BuildSystem::clean(bool all) {
    std::cout << "Cleaning build artifacts...\n";

    std::string output_dir = config_->getConfig().build.output;
    
    try {
        if (std::filesystem::exists(output_dir)) {
            std::filesystem::remove_all(output_dir);
            std::cout << "✓ Removed " << output_dir << "\n";
        }

        if (all && cache_) {
            cache_->clear();
            std::cout << "✓ Cleared cache\n";
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "✗ Clean failed: " << e.what() << "\n";
        return false;
    }
}

void BuildSystem::watch(const std::vector<std::string>& paths) {
    std::cout << "Watching for changes...\n";

    auto watcher = std::make_unique<Watcher>();
    
    // Add paths to watch
    std::vector<std::string> watch_paths = paths;
    if (watch_paths.empty()) {
        watch_paths = config_->getConfig().build.sources;
    }

    for (const auto& path : watch_paths) {
        watcher->addPath(path);
        std::cout << "Watching " << path << "\n";
    }

    // Set callback
    watcher->setCallback([this](const std::vector<FileEvent>& events) {
        std::cout << "\n→ Files changed, rebuilding...\n";
        
        BuildOptions opts;
        BuildResult result = this->build(opts);
        
        if (result.success) {
            std::cout << "→ Build successful in " << result.build_time_seconds << "s\n";
        } else {
            std::cerr << "→ Build failed\n";
        }
    });

    // Start watching (blocking)
    watcher->startBlocking();
}

const ProjectConfig& BuildSystem::getConfig() const {
    return config_->getConfig();
}

std::string BuildSystem::getBuildStats() const {
    const auto& stats = builder_->getStats();
    
    std::stringstream ss;
    ss << "Build Statistics:\n";
    ss << "  Total files: " << stats.total_files << "\n";
    ss << "  Compiled: " << stats.files_compiled << "\n";
    ss << "  Cached: " << stats.files_cached << "\n";
    ss << "  Failed: " << stats.files_failed << "\n";
    ss << "  Total time: " << stats.total_time_seconds << "s\n";
    ss << "  Compile time: " << stats.compile_time << "s\n";
    ss << "  Link time: " << stats.link_time << "s\n";

    return ss.str();
}

bool BuildSystem::ensureBuilt() {
    if (!needsRebuild()) {
        return true;
    }

    BuildOptions opts;
    BuildResult result = build(opts);
    return result.success;
}

bool BuildSystem::needsRebuild() const {
    return builder_->needsRebuild();
}

void BuildSystem::printBuildSummary(const BuildResult& result) const {
    if (result.success) {
        std::cout << "\n✓ Build successful!\n";
        std::cout << "  Output: " << result.output_path << "\n";
        std::cout << "  Time: " << result.build_time_seconds << "s\n";
        std::cout << "  Files compiled: " << result.files_compiled << "\n";
        std::cout << "  Files cached: " << result.files_cached << "\n";
    } else {
        std::cerr << "\n✗ Build failed\n";
        if (!result.error_message.empty()) {
            std::cerr << "  Error: " << result.error_message << "\n";
        }
    }
}

void BuildSystem::handleBuildError(const std::string& error) {
    std::cerr << "Build error: " << error << "\n";
}

std::string getBuildSystemVersion() {
    return "1.0.0";
}

void printBanner() {
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "  Sad Build System v" << getBuildSystemVersion() << "\n";
    std::cout << "  نظام بناء لغة ص\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
}

} // namespace build
} // namespace sad
