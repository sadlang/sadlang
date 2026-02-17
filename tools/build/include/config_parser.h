// بسم الله الرحمن الرحيم
/**
 * @file config_parser.h
 * @brief Configuration Parser for sad.toml - محلل إعدادات sad.toml
 * 
 * Parses sad.toml project configuration files and provides access to:
 * - Package metadata (name, version, authors, etc.)
 * - Build configuration (main file, output dir, optimization, etc.)
 * - Dependencies
 * - Custom tasks
 * 
 * @author Sad Language Team
 * @date 2026-01-07
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace sad {
namespace build {

/**
 * @brief Package metadata - معلومات الحزمة
 */
struct PackageInfo {
    std::string name;                   ///< Package name
    std::string version;                ///< Package version (e.g., "1.0.0")
    std::string description;            ///< Package description
    std::vector<std::string> authors;   ///< Package authors
    std::string license;                ///< License (e.g., "MIT")
    std::string homepage;               ///< Homepage URL
    std::string repository;             ///< Repository URL
};

/**
 * @brief Build configuration - إعدادات البناء
 */
struct BuildConfig {
    std::string main;                   ///< Main entry point (e.g., "src/main.s")
    std::string output;                 ///< Output directory (e.g., "build/")
    std::vector<std::string> sources;   ///< Source directories
    std::string optimization;           ///< Optimization level (none/basic/full)
    std::string target;                 ///< Target platform (x86_64/arm64/wasm)
    std::string type;                   ///< Build type (binary/library)
    
    // Compiler flags
    std::vector<std::string> compiler_flags;
    std::vector<std::string> linker_flags;
};

/**
 * @brief Dependency specification - مواصفات التبعية
 */
struct Dependency {
    std::string name;                   ///< Dependency name
    std::string version;                ///< Version requirement (e.g., "^1.0.0")
    std::string source;                 ///< Source (registry/git/path)
    std::optional<std::string> git_url; ///< Git URL (if git source)
    std::optional<std::string> path;    ///< Local path (if path source)
    bool optional = false;              ///< Optional dependency
};

/**
 * @brief Custom task definition - تعريف مهمة مخصصة
 */
struct TaskConfig {
    std::string name;                   ///< Task name
    std::string description;            ///< Task description
    std::string command;                ///< Command to execute
    std::vector<std::string> args;      ///< Command arguments
    std::vector<std::string> depends;   ///< Task dependencies
    bool is_background = false;         ///< Run in background
};

/**
 * @brief Complete project configuration - الإعدادات الكاملة للمشروع
 */
struct ProjectConfig {
    PackageInfo package;                                ///< Package metadata
    BuildConfig build;                                  ///< Build configuration
    std::map<std::string, Dependency> dependencies;     ///< Dependencies
    std::map<std::string, Dependency> dev_dependencies; ///< Dev dependencies
    std::map<std::string, TaskConfig> tasks;            ///< Custom tasks
    
    std::string config_dir;                             ///< Directory containing sad.toml
};

/**
 * @brief Configuration parser class - محلل الإعدادات
 * 
 * Parses sad.toml files and provides access to project configuration.
 * 
 * Example sad.toml:
 * @code
 * [package]
 * name = "my_project"
 * version = "1.0.0"
 * 
 * [build]
 * main = "src/main.s"
 * output = "build/"
 * 
 * [dependencies]
 * http = "^1.0.0"
 * 
 * [tasks.build]
 * command = "sadc"
 * args = ["${main}", "-o", "${output}/app"]
 * @endcode
 */
class ConfigParser {
public:
    /**
     * @brief Constructor
     * @param config_path Path to sad.toml file
     * @throws std::runtime_error if file not found or invalid
     */
    explicit ConfigParser(const std::string& config_path);

    /**
     * @brief Parse the configuration file
     * @return true if successful
     */
    bool parse();

    /**
     * @brief Get complete project configuration
     * @return ProjectConfig structure
     */
    const ProjectConfig& getConfig() const { return config_; }

    /**
     * @brief Get package info
     * @return PackageInfo structure
     */
    const PackageInfo& getPackageInfo() const { return config_.package; }

    /**
     * @brief Get build config
     * @return BuildConfig structure
     */
    const BuildConfig& getBuildConfig() const { return config_.build; }

    /**
     * @brief Get dependencies
     * @return Map of dependency name to Dependency
     */
    const std::map<std::string, Dependency>& getDependencies() const { 
        return config_.dependencies; 
    }

    /**
     * @brief Get dev dependencies
     * @return Map of dev dependency name to Dependency
     */
    const std::map<std::string, Dependency>& getDevDependencies() const { 
        return config_.dev_dependencies; 
    }

    /**
     * @brief Get custom tasks
     * @return Map of task name to TaskConfig
     */
    const std::map<std::string, TaskConfig>& getTasks() const { 
        return config_.tasks; 
    }

    /**
     * @brief Get task by name
     * @param name Task name
     * @return Optional TaskConfig
     */
    std::optional<TaskConfig> getTask(const std::string& name) const;

    /**
     * @brief Expand variables in string - توسيع المتغيرات في النص
     * 
     * Expands variables like ${main}, ${output}, etc.
     * 
     * @param str String with variables
     * @return String with expanded variables
     * 
     * @code
     * std::string result = parser.expandVariables("${output}/app");
     * // result = "build/app"
     * @endcode
     */
    std::string expandVariables(const std::string& str) const;

    /**
     * @brief Validate configuration
     * @return true if valid
     */
    bool validate() const;

    /**
     * @brief Get validation errors
     * @return Vector of error messages
     */
    std::vector<std::string> getValidationErrors() const;

private:
    std::string config_path_;           ///< Path to sad.toml
    ProjectConfig config_;              ///< Parsed configuration
    std::vector<std::string> errors_;   ///< Validation errors
    
    /**
     * @brief Parse [package] section
     */
    bool parsePackage(const void* toml_table);
    
    /**
     * @brief Parse [build] section
     */
    bool parseBuild(const void* toml_table);
    
    /**
     * @brief Parse [dependencies] section
     */
    bool parseDependencies(const void* toml_table, bool is_dev);
    
    /**
     * @brief Parse [tasks] section
     */
    bool parseTasks(const void* toml_table);
    
    /**
     * @brief Parse single task
     */
    TaskConfig parseTask(const std::string& name, const void* toml_table);
    
    /**
     * @brief Get TOML string value
     */
    std::string getTomlString(const void* toml_table, const std::string& key, 
                             const std::string& default_value = "") const;
    
    /**
     * @brief Get TOML array value
     */
    std::vector<std::string> getTomlArray(const void* toml_table, 
                                          const std::string& key) const;
    
    /**
     * @brief Get TOML bool value
     */
    bool getTomlBool(const void* toml_table, const std::string& key, 
                     bool default_value = false) const;
    
    /**
     * @brief Set default values
     */
    void setDefaults();
    
    /**
     * @brief Add validation error
     */
    void addError(const std::string& error);
};

/**
 * @brief Create default sad.toml file
 * @param project_name Project name
 * @param output_path Output path for sad.toml
 * @return true if successful
 */
bool createDefaultConfig(const std::string& project_name, 
                        const std::string& output_path = "sad.toml");

} // namespace build
} // namespace sad
