// بسم الله الرحمن الرحيم
/**
 * @file generator.cpp
 * @brief Template Generator for Sad Projects
 * 
 * Generates new projects from templates with variable substitution
 * 
 * @author Sad Language Team
 * @date January 2026
 * الحمد لله رب العالمين
 */

#include <string>
#include <map>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;

namespace sad {
namespace templates {

// ============================================================================
// Template Variables
// ============================================================================

/**
 * @brief Template variables for substitution
 */
struct TemplateVars {
    std::string project_name;
    std::string author_name;
    std::string author_email;
    std::string date;
    std::string year;
    
    /**
     * @brief Get all variables as map
     */
    std::map<std::string, std::string> to_map() const {
        return {
            {"PROJECT_NAME", project_name},
            {"AUTHOR_NAME", author_name},
            {"AUTHOR_EMAIL", author_email},
            {"DATE", date},
            {"YEAR", year}
        };
    }
};

// ============================================================================
// Template Generator
// ============================================================================

/**
 * @brief Generates projects from templates
 */
class Generator {
public:
    /**
     * @brief Constructor
     * @param templates_dir Path to templates directory
     */
    Generator(const std::string& templates_dir = "tools/templates")
        : templates_dir_(templates_dir) {}
    
    /**
     * @brief List available templates
     * @return Vector of template names
     */
    std::vector<std::string> list_templates() const {
        std::vector<std::string> templates;
        
        if (!fs::exists(templates_dir_)) {
            return templates;
        }
        
        for (const auto& entry : fs::directory_iterator(templates_dir_)) {
            if (entry.is_directory()) {
                templates.push_back(entry.path().filename().string());
            }
        }
        
        return templates;
    }
    
    /**
     * @brief Generate project from template
     * @param template_name Name of template
     * @param project_name Name of project
     * @param output_dir Output directory
     * @param vars Template variables
     */
    void generate(const std::string& template_name,
                  const std::string& project_name,
                  const std::string& output_dir,
                  const TemplateVars& vars) {
        
        fs::path template_path = fs::path(templates_dir_) / template_name;
        fs::path output_path = fs::path(output_dir) / project_name;
        
        // Check if template exists
        if (!fs::exists(template_path)) {
            throw std::runtime_error("Template '" + template_name + "' not found");
        }
        
        // Check if output directory already exists
        if (fs::exists(output_path)) {
            throw std::runtime_error("Directory '" + output_path.string() + "' already exists");
        }
        
        // Copy template to output directory
        std::cout << "Creating project '" << project_name << "' from template '" 
                  << template_name << "'...\n";
        
        copy_and_substitute(template_path, output_path, vars);
        
        std::cout << "✓ Project created successfully at: " << output_path << "\n";
        std::cout << "\nNext steps:\n";
        std::cout << "  cd " << project_name << "\n";
        std::cout << "  sad-pkg install\n";
        std::cout << "  sad build\n";
        std::cout << "  sad run\n";
    }
    
private:
    std::string templates_dir_;
    
    /**
     * @brief Copy directory and substitute variables
     */
    void copy_and_substitute(const fs::path& src, 
                            const fs::path& dst,
                            const TemplateVars& vars) {
        
        // Create output directory
        fs::create_directories(dst);
        
        // Get variable map
        auto var_map = vars.to_map();
        
        // Copy all files and directories
        for (const auto& entry : fs::recursive_directory_iterator(src)) {
            auto rel_path = fs::relative(entry.path(), src);
            auto dst_path = dst / rel_path;
            
            if (entry.is_directory()) {
                fs::create_directories(dst_path);
            } else if (entry.is_regular_file()) {
                copy_file_with_substitution(entry.path(), dst_path, var_map);
            }
        }
    }
    
    /**
     * @brief Copy file and substitute variables
     */
    void copy_file_with_substitution(const fs::path& src,
                                     const fs::path& dst,
                                     const std::map<std::string, std::string>& vars) {
        
        // Read source file
        std::ifstream src_file(src);
        if (!src_file) {
            throw std::runtime_error("Cannot read file: " + src.string());
        }
        
        std::string content((std::istreambuf_iterator<char>(src_file)),
                           std::istreambuf_iterator<char>());
        src_file.close();
        
        // Substitute variables
        for (const auto& [key, value] : vars) {
            std::string pattern = "{{" + key + "}}";
            size_t pos = 0;
            while ((pos = content.find(pattern, pos)) != std::string::npos) {
                content.replace(pos, pattern.length(), value);
                pos += value.length();
            }
        }
        
        // Write to destination
        fs::create_directories(dst.parent_path());
        std::ofstream dst_file(dst);
        if (!dst_file) {
            throw std::runtime_error("Cannot write file: " + dst.string());
        }
        
        dst_file << content;
        dst_file.close();
        
        std::cout << "  Created: " << dst << "\n";
    }
};

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Get current date as string
 */
std::string get_current_date() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_c);
    
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%B %d, %Y", now_tm);
    return std::string(buffer);
}

/**
 * @brief Get current year as string
 */
std::string get_current_year() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_c);
    
    return std::to_string(1900 + now_tm->tm_year);
}

/**
 * @brief Print template list
 */
void print_templates(const std::vector<std::string>& templates) {
    std::cout << "\nAvailable templates:\n\n";
    
    std::map<std::string, std::string> descriptions = {
        {"cli", "Command-line application"},
        {"web", "Web application (Backend)"},
        {"library", "Reusable library"},
        {"rest-api", "RESTful API with database"},
        {"game", "2D game with Raylib"},
        {"desktop", "Desktop application with GUI"},
        {"blank", "Blank project"}
    };
    
    for (const auto& tmpl : templates) {
        std::cout << "  " << tmpl;
        if (descriptions.count(tmpl)) {
            std::cout << " - " << descriptions[tmpl];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

} // namespace templates
} // namespace sad

// ============================================================================
// Main Entry Point (if compiled as standalone)
// ============================================================================

#ifndef SAD_TEMPLATES_NO_MAIN

int main(int argc, char* argv[]) {
    using namespace sad::templates;
    
    // Parse arguments
    if (argc < 2) {
        std::cout << "sad-template - Project Template Generator\n\n";
        std::cout << "Usage:\n";
        std::cout << "  sad-template list\n";
        std::cout << "  sad-template new <template> <project_name> [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --author <name>     Author name\n";
        std::cout << "  --email <email>     Author email\n";
        std::cout << "  --output <dir>      Output directory (default: .)\n\n";
        std::cout << "Examples:\n";
        std::cout << "  sad-template list\n";
        std::cout << "  sad-template new cli my_cli --author \"John Doe\"\n";
        std::cout << "  sad-template new web my_web_app --email john@example.com\n";
        return 1;
    }
    
    std::string command = argv[1];
    
    try {
        Generator generator;
        
        if (command == "list") {
            auto templates = generator.list_templates();
            print_templates(templates);
            return 0;
        }
        
        if (command == "new") {
            if (argc < 4) {
                std::cerr << "Error: Missing template or project name\n";
                std::cerr << "Usage: sad-template new <template> <project_name>\n";
                return 1;
            }
            
            std::string template_name = argv[2];
            std::string project_name = argv[3];
            
            // Parse options
            TemplateVars vars;
            vars.project_name = project_name;
            vars.author_name = "Developer";
            vars.author_email = "dev@example.com";
            vars.date = get_current_date();
            vars.year = get_current_year();
            
            std::string output_dir = ".";
            
            for (int i = 4; i < argc; i++) {
                std::string arg = argv[i];
                if (arg == "--author" && i + 1 < argc) {
                    vars.author_name = argv[++i];
                } else if (arg == "--email" && i + 1 < argc) {
                    vars.author_email = argv[++i];
                } else if (arg == "--output" && i + 1 < argc) {
                    output_dir = argv[++i];
                }
            }
            
            // Generate project
            generator.generate(template_name, project_name, output_dir, vars);
            
            return 0;
        }
        
        std::cerr << "Error: Unknown command '" << command << "'\n";
        return 1;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}

#endif // SAD_TEMPLATES_NO_MAIN
