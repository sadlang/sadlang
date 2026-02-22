// بسم الله الرحمن الرحيم
/**
 * @file config_parser.cpp
 * @brief Configuration Parser Implementation
 */

#include "../include/config_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <regex>

namespace sad {
namespace build {

// Simple TOML parser (basic implementation)
// For production, use toml++ library
class SimpleTomlParser {
public:
    struct Value {
        enum Type { STRING, ARRAY, TABLE } type;
        std::string string_value;
        std::vector<std::string> array_value;
        std::map<std::string, Value> table_value;
    };

    bool parse(const std::string& content) {
        std::istringstream stream(content);
        std::string line;
        std::string current_section;

        while (std::getline(stream, line)) {
            // Remove comments
            size_t comment_pos = line.find('#');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }

            // Trim whitespace
            line = trim(line);
            if (line.empty()) continue;

            // Section header [section]
            if (line[0] == '[' && line.back() == ']') {
                current_section = line.substr(1, line.length() - 2);
                data_[current_section] = Value{Value::TABLE, "", {}, {}};
                continue;
            }

            // Key = value
            size_t eq_pos = line.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = trim(line.substr(0, eq_pos));
                std::string value_str = trim(line.substr(eq_pos + 1));

                Value value = parseValue(value_str);
                
                if (current_section.empty()) {
                    data_[key] = value;
                } else {
                    data_[current_section].table_value[key] = value;
                }
            }
        }

        return true;
    }

    std::string getString(const std::string& section, const std::string& key, 
                         const std::string& default_value = "") const {
        auto sec_it = data_.find(section);
        if (sec_it == data_.end()) return default_value;

        auto key_it = sec_it->second.table_value.find(key);
        if (key_it == sec_it->second.table_value.end()) return default_value;

        return key_it->second.string_value;
    }

    std::vector<std::string> getArray(const std::string& section, const std::string& key) const {
        auto sec_it = data_.find(section);
        if (sec_it == data_.end()) return {};

        auto key_it = sec_it->second.table_value.find(key);
        if (key_it == sec_it->second.table_value.end()) return {};

        return key_it->second.array_value;
    }

    bool hasSection(const std::string& section) const {
        return data_.find(section) != data_.end();
    }

    std::map<std::string, Value> getSection(const std::string& section) const {
        auto it = data_.find(section);
        if (it != data_.end()) {
            return it->second.table_value;
        }
        return {};
    }

private:
    std::map<std::string, Value> data_;

    Value parseValue(const std::string& str) {
        std::string trimmed = trim(str);

        // Array [...]
        if (trimmed[0] == '[' && trimmed.back() == ']') {
            Value val{Value::ARRAY, "", {}, {}};
            std::string arr_content = trimmed.substr(1, trimmed.length() - 2);
            
            std::istringstream stream(arr_content);
            std::string item;
            while (std::getline(stream, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    // Remove quotes
                    if (item[0] == '"' && item.back() == '"') {
                        item = item.substr(1, item.length() - 2);
                    }
                    val.array_value.push_back(item);
                }
            }
            return val;
        }

        // String "..."
        if (trimmed[0] == '"' && trimmed.back() == '"') {
            Value val{Value::STRING, trimmed.substr(1, trimmed.length() - 2), {}, {}};
            return val;
        }

        // Plain string
        Value val{Value::STRING, trimmed, {}, {}};
        return val;
    }

    std::string trim(const std::string& str) const {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
};

ConfigParser::ConfigParser(const std::string& config_path)
    : config_path_(config_path) {
    config_.config_dir = std::filesystem::path(config_path).parent_path().string();
    if (config_.config_dir.empty()) {
        config_.config_dir = ".";
    }
}

bool ConfigParser::parse() {
    // Read file
    std::ifstream file(config_path_);
    if (!file.is_open()) {
        addError("Failed to open config file: " + config_path_);
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Parse TOML
    SimpleTomlParser parser;
    if (!parser.parse(content)) {
        addError("Failed to parse TOML file");
        return false;
    }

    // Parse sections
    if (parser.hasSection("package")) {
        config_.package.name = parser.getString("package", "name", "unnamed");
        config_.package.version = parser.getString("package", "version", "0.1.0");
        config_.package.description = parser.getString("package", "description", "");
        config_.package.license = parser.getString("package", "license", "");
        config_.package.homepage = parser.getString("package", "homepage", "");
        config_.package.repository = parser.getString("package", "repository", "");
        config_.package.authors = parser.getArray("package", "authors");
    }

    if (parser.hasSection("build")) {
        config_.build.main = parser.getString("build", "main", "src/main.s");
        config_.build.output = parser.getString("build", "output", "build/");
        config_.build.optimization = parser.getString("build", "optimization", "none");
        config_.build.target = parser.getString("build", "target", "x86_64");
        config_.build.type = parser.getString("build", "type", "binary");
        config_.build.sources = parser.getArray("build", "sources");
        
        if (config_.build.sources.empty()) {
            config_.build.sources.push_back("src/");
        }
    }

    if (parser.hasSection("dependencies")) {
        auto deps = parser.getSection("dependencies");
        for (const auto& [name, value] : deps) {
            Dependency dep;
            dep.name = name;
            dep.version = value.string_value;
            dep.source = "registry";
            config_.dependencies[name] = dep;
        }
    }

    if (parser.hasSection("dev-dependencies")) {
        auto deps = parser.getSection("dev-dependencies");
        for (const auto& [name, value] : deps) {
            Dependency dep;
            dep.name = name;
            dep.version = value.string_value;
            dep.source = "registry";
            config_.dev_dependencies[name] = dep;
        }
    }

    // Parse tasks
    for (const auto& section_name : {"tasks.build", "tasks.run", "tasks.test", "tasks.clean"}) {
        if (parser.hasSection(section_name)) {
            std::string task_name = std::string(section_name).substr(6); // Remove "tasks."
            TaskConfig task;
            task.name = task_name;
            task.description = parser.getString(section_name, "description", "");
            task.command = parser.getString(section_name, "command", "");
            task.args = parser.getArray(section_name, "args");
            task.depends = parser.getArray(section_name, "depends");
            
            config_.tasks[task_name] = task;
        }
    }

    setDefaults();
    return validate();
}

std::optional<TaskConfig> ConfigParser::getTask(const std::string& name) const {
    auto it = config_.tasks.find(name);
    if (it != config_.tasks.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::string ConfigParser::expandVariables(const std::string& str) const {
    std::string result = str;
    
    // Replace ${variable} with actual values
    std::regex var_regex(R"(\$\{([^}]+)\})");
    std::smatch match;
    
    while (std::regex_search(result, match, var_regex)) {
        std::string var_name = match[1].str();
        std::string replacement;
        
        if (var_name == "main") {
            replacement = config_.build.main;
        } else if (var_name == "output") {
            replacement = config_.build.output;
        } else if (var_name == "name") {
            replacement = config_.package.name;
        } else if (var_name == "version") {
            replacement = config_.package.version;
        }
        
        result = match.prefix().str() + replacement + match.suffix().str();
    }
    
    return result;
}

bool ConfigParser::validate() const {
    if (config_.package.name.empty()) {
        return false;
    }
    
    if (config_.build.main.empty()) {
        return false;
    }
    
    return true;
}

std::vector<std::string> ConfigParser::getValidationErrors() const {
    return errors_;
}

void ConfigParser::setDefaults() {
    if (config_.build.output.empty()) {
        config_.build.output = "build/";
    }
    
    if (config_.build.optimization.empty()) {
        config_.build.optimization = "none";
    }
    
    if (config_.build.target.empty()) {
        config_.build.target = "x86_64";
    }
}

void ConfigParser::addError(const std::string& error) {
    errors_.push_back(error);
    std::cerr << "Error: " << error << "\n";
}

bool createDefaultConfig(const std::string& project_name, const std::string& output_path) {
    std::ofstream file(output_path);
    if (!file.is_open()) {
        return false;
    }

    file << "# بسم الله الرحمن الرحيم\n";
    file << "[package]\n";
    file << "name = \"" << project_name << "\"\n";
    file << "version = \"0.1.0\"\n";
    file << "description = \"A Sad Language project\"\n";
    file << "authors = []\n";
    file << "\n";
    file << "[build]\n";
    file << "main = \"src/main.s\"\n";
    file << "output = \"build/\"\n";
    file << "optimization = \"none\"\n";
    file << "target = \"x86_64\"\n";
    file << "\n";
    file << "[dependencies]\n";
    file << "\n";
    file << "[dev-dependencies]\n";
    file << "\n";
    file << "[tasks.build]\n";
    file << "command = \"sadc\"\n";
    file << "args = [\"${main}\", \"-o\", \"${output}/app\"]\n";
    file << "description = \"Build the project\"\n";
    file << "\n";
    file << "[tasks.run]\n";
    file << "command = \"${output}/app\"\n";
    file << "description = \"Run the application\"\n";
    file << "depends = [\"build\"]\n";

    return true;
}

} // namespace build
} // namespace sad
