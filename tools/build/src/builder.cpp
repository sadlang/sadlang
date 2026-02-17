// بسم الله الرحمن الرحيم
/**
 * @file builder.cpp
 * @brief Build Engine Implementation
 */

#include "../include/builder.h"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <sstream>

namespace sad {
namespace build {

Builder::Builder(const ProjectConfig& config, CacheManager* cache_manager)
    : config_(config), cache_manager_(cache_manager) {
}

void Builder::setOptimization(const std::string& level) {
    optimization_ = level;
}

void Builder::setTarget(const std::string& target) {
    target_ = target;
}

void Builder::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void Builder::setClean(bool clean) {
    clean_ = clean;
}

BuildResult Builder::build() {
    BuildResult result;
    auto start = std::chrono::high_resolution_clock::now();

    // Create output directory
    std::filesystem::create_directories(config_.build.output);

    // Get source files
    std::vector<SourceFile> sources = getSourceFiles();
    stats_.total_files = sources.size();

    if (sources.empty()) {
        result.success = false;
        result.error_message = "No source files found";
        return result;
    }

    // Compile all sources
    std::vector<CompilationResult> compile_results = compile();
    
    // Check for compilation errors
    for (const auto& comp_result : compile_results) {
        if (!comp_result.success) {
            result.success = false;
            result.error_message = comp_result.error_message;
            result.stats = stats_;
            return result;
        }
    }

    // Collect object files
    std::vector<std::string> object_files;
    for (const auto& comp_result : compile_results) {
        object_files.push_back(comp_result.object_file);
    }

    // Link
    LinkResult link_result = link(object_files);
    
    auto end = std::chrono::high_resolution_clock::now();
    stats_.total_time_seconds = 
        std::chrono::duration<double>(end - start).count();

    result.success = link_result.success;
    result.output_path = link_result.output_file;
    result.error_message = link_result.error_message;
    result.build_time_seconds = stats_.total_time_seconds;
    result.files_compiled = stats_.files_compiled;
    result.files_cached = stats_.files_cached;
    result.stats = stats_;

    return result;
}

std::vector<CompilationResult> Builder::compile() {
    std::vector<CompilationResult> results;
    
    std::vector<SourceFile> sources = getSourceFiles();
    int current = 0;

    for (auto& source : sources) {
        current++;
        
        // Check cache
        if (cache_manager_ && !clean_) {
            std::string hash = CacheManager::calculateHash(source.absolute_path);
            
            if (cache_manager_->isCached(source.absolute_path, hash, 
                                        "1.0.0", optimization_, target_)) {
                auto cached_obj = cache_manager_->getCachedObjectFile(source.absolute_path);
                if (cached_obj && std::filesystem::exists(*cached_obj)) {
                    CompilationResult result;
                    result.success = true;
                    result.source_file = source.absolute_path;
                    result.object_file = *cached_obj;
                    
                    results.push_back(result);
                    stats_.files_cached++;
                    
                    if (verbose_) {
                        std::cout << "  [" << current << "/" << sources.size() 
                                  << "] Using cached: " << source.path << "\n";
                    }
                    continue;
                }
            }
        }

        // Compile
        printCompileProgress(current, sources.size(), source.path);
        
        CompilationResult result = compileFile(source);
        results.push_back(result);
        
        if (result.success) {
            stats_.files_compiled++;
            stats_.compile_time += result.compile_time_seconds;
            
            // Update cache
            if (cache_manager_) {
                std::string hash = CacheManager::calculateHash(source.absolute_path);
                cache_manager_->updateCache(source.absolute_path, hash, 
                                          result.object_file, "1.0.0", 
                                          optimization_, target_);
            }
        } else {
            stats_.files_failed++;
            printError(result.error_message);
            break;
        }
    }

    return results;
}

LinkResult Builder::link(const std::vector<std::string>& object_files) {
    LinkResult result;
    
    if (object_files.empty()) {
        result.success = false;
        result.error_message = "No object files to link";
        return result;
    }

    std::string output_path = getOutputPath();
    
    if (verbose_) {
        std::cout << "Linking " << output_path << "...\n";
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::string link_cmd = getLinkerCommand(object_files, output_path);
    int exit_code = executeCommand(link_cmd, !verbose_);

    auto end = std::chrono::high_resolution_clock::now();
    result.link_time_seconds = 
        std::chrono::duration<double>(end - start).count();

    stats_.link_time = result.link_time_seconds;

    result.success = (exit_code == 0);
    result.output_file = output_path;

    if (!result.success) {
        result.error_message = "Linking failed";
    }

    return result;
}

std::vector<SourceFile> Builder::getSourceFiles() const {
    std::vector<SourceFile> files;

    for (const auto& source_dir : config_.build.sources) {
        if (std::filesystem::exists(source_dir)) {
            discoverSourceFiles(source_dir, files);
        }
    }

    // Add main file if not already included
    if (std::filesystem::exists(config_.build.main)) {
        bool found = false;
        for (const auto& f : files) {
            if (f.path == config_.build.main) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            SourceFile main_file;
            main_file.path = config_.build.main;
            main_file.absolute_path = std::filesystem::absolute(config_.build.main).string();
            main_file.last_modified = std::filesystem::last_write_time(main_file.absolute_path).time_since_epoch().count();
            files.push_back(main_file);
        }
    }

    return files;
}

bool Builder::needsRebuild() const {
    auto sources = getSourceFiles();
    
    for (const auto& source : sources) {
        if (needsRecompilation(source)) {
            return true;
        }
    }

    return false;
}

void Builder::discoverSourceFiles(const std::string& directory, 
                                  std::vector<SourceFile>& files) const {
    if (!std::filesystem::exists(directory)) {
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            if (ext == ".s" || ext == ".sad") {
                SourceFile file;
                file.path = entry.path().string();
                file.absolute_path = std::filesystem::absolute(entry.path()).string();
                file.last_modified = entry.last_write_time().time_since_epoch().count();
                files.push_back(file);
            }
        }
    }
}

CompilationResult Builder::compileFile(const SourceFile& source) {
    CompilationResult result;
    result.source_file = source.absolute_path;
    result.object_file = getObjectFilePath(source.absolute_path);

    auto start = std::chrono::high_resolution_clock::now();

    std::string compile_cmd = getCompilerCommand(source.absolute_path, result.object_file);
    int exit_code = executeCommand(compile_cmd, !verbose_);

    auto end = std::chrono::high_resolution_clock::now();
    result.compile_time_seconds = 
        std::chrono::duration<double>(end - start).count();

    result.success = (exit_code == 0);

    if (!result.success) {
        result.error_message = "Compilation failed for " + source.path;
    }

    return result;
}

std::string Builder::getCompilerCommand(const std::string& source_file,
                                       const std::string& output_file) const {
    std::stringstream cmd;
    
    // Basic compiler command
    cmd << "sadc";
    
    // Optimization
    if (optimization_ == "full") {
        cmd << " -O3";
    } else if (optimization_ == "basic") {
        cmd << " -O1";
    }
    
    // Target
    if (target_ != "x86_64") {
        cmd << " --target=" << target_;
    }
    
    // Input and output
    cmd << " \"" << source_file << "\"";
    cmd << " -o \"" << output_file << "\"";
    
    // Additional flags
    for (const auto& flag : config_.build.compiler_flags) {
        cmd << " " << flag;
    }

    return cmd.str();
}

std::string Builder::getLinkerCommand(const std::vector<std::string>& object_files,
                                     const std::string& output_file) const {
    std::stringstream cmd;
    
    cmd << "sadc";
    
    // Object files
    for (const auto& obj : object_files) {
        cmd << " \"" << obj << "\"";
    }
    
    // Output
    cmd << " -o \"" << output_file << "\"";
    
    // Linker flags
    for (const auto& flag : config_.build.linker_flags) {
        cmd << " " << flag;
    }

    return cmd.str();
}

int Builder::executeCommand(const std::string& command, bool capture_output) {
    if (verbose_ && !capture_output) {
        std::cout << "  $ " << command << "\n";
    }
    
    return std::system(command.c_str());
}

std::string Builder::getObjectFilePath(const std::string& source_file) const {
    std::filesystem::path source_path(source_file);
    std::string obj_name = source_path.stem().string() + ".o";
    
    std::filesystem::path output_path(config_.build.output);
    output_path /= "objects";
    std::filesystem::create_directories(output_path);
    
    output_path /= obj_name;
    return output_path.string();
}

std::string Builder::getOutputPath() const {
    std::filesystem::path output_path(config_.build.output);
    output_path /= config_.package.name;
    
#ifdef _WIN32
    output_path += ".exe";
#endif

    return output_path.string();
}

bool Builder::needsRecompilation(const SourceFile& source) const {
    if (clean_) {
        return true;
    }

    std::string obj_path = getObjectFilePath(source.absolute_path);
    
    if (!std::filesystem::exists(obj_path)) {
        return true;
    }

    // Check if source is newer than object
    auto source_time = std::filesystem::last_write_time(source.absolute_path);
    auto obj_time = std::filesystem::last_write_time(obj_path);

    return source_time > obj_time;
}

void Builder::printCompileProgress(int current, int total, 
                                   const std::string& file_name) const {
    std::cout << "  [" << current << "/" << total << "] Compiling " << file_name << "\n";
}

void Builder::printError(const std::string& error) const {
    std::cerr << "✗ Error: " << error << "\n";
}

} // namespace build
} // namespace sad
