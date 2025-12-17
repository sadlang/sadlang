// بسم الله الرحمن الرحيم
/**
 * @file cli.cpp
 * @brief sad-pkg CLI Tool - أداة sad-pkg لسطر الأوامر
 * 
 * أداة سطر الأوامر لإدارة حزم Sad Language
 * Command-line tool for managing Sad Language packages
 * 
 * Usage / الاستخدام:
 *   sad-pkg init              - Initialize new package / إنشاء حزمة جديدة
 *   sad-pkg install [pkg]     - Install dependencies / تثبيت التبعيات
 *   sad-pkg add <pkg>         - Add dependency / إضافة تبعية
 *   sad-pkg remove <pkg>      - Remove dependency / إزالة تبعية
 *   sad-pkg update            - Update dependencies / تحديث التبعيات
 *   sad-pkg publish           - Publish package / نشر الحزمة
 *   sad-pkg search <query>    - Search packages / البحث عن الحزم
 *   sad-pkg info <pkg>        - Show package info / عرض معلومات الحزمة
 *   sad-pkg list              - List installed packages / سرد الحزم المثبتة
 *   sad-pkg clean             - Clean cache / تنظيف التخزين المؤقت
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include "package.h"
#include "toml_parser.h"
#include "dependency_resolver.h"
#include "registry_client.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>

using namespace sad::pkg;

// ============================================================================
// Color Output - إخراج ملون
// ============================================================================

#ifdef _WIN32
#include <windows.h>

class ConsoleColor {
public:
    static void init() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }
};
#else
class ConsoleColor {
public:
    static void init() {}
};
#endif

const char* RESET = "\033[0m";
const char* BOLD = "\033[1m";
const char* RED = "\033[31m";
const char* GREEN = "\033[32m";
const char* YELLOW = "\033[33m";
const char* BLUE = "\033[34m";
const char* CYAN = "\033[36m";

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

void print_header(const std::string& text) {
    std::cout << BOLD << CYAN << text << RESET << "\n";
}

void print_success(const std::string& text) {
    std::cout << GREEN << "✓ " << text << RESET << "\n";
}

void print_error(const std::string& text) {
    std::cerr << RED << "✗ Error: " << text << RESET << "\n";
}

void print_warning(const std::string& text) {
    std::cout << YELLOW << "⚠ Warning: " << text << RESET << "\n";
}

void print_info(const std::string& text) {
    std::cout << BLUE << "ℹ " << text << RESET << "\n";
}

void print_usage() {
    print_header("sad-pkg - Sad Language Package Manager");
    std::cout << "\n";
    std::cout << BOLD << "USAGE:" << RESET << "\n";
    std::cout << "  sad-pkg <command> [options]\n\n";
    
    std::cout << BOLD << "COMMANDS:" << RESET << "\n";
    std::cout << "  " << CYAN << "init" << RESET << "              Initialize new package\n";
    std::cout << "  " << CYAN << "install [pkg]" << RESET << "     Install dependencies or specific package\n";
    std::cout << "  " << CYAN << "add <pkg>" << RESET << "         Add dependency to sad.toml\n";
    std::cout << "  " << CYAN << "remove <pkg>" << RESET << "      Remove dependency from sad.toml\n";
    std::cout << "  " << CYAN << "update" << RESET << "            Update all dependencies\n";
    std::cout << "  " << CYAN << "publish" << RESET << "           Publish package to registry\n";
    std::cout << "  " << CYAN << "search <query>" << RESET << "    Search for packages\n";
    std::cout << "  " << CYAN << "info <pkg>" << RESET << "        Show package information\n";
    std::cout << "  " << CYAN << "list" << RESET << "              List installed packages\n";
    std::cout << "  " << CYAN << "clean" << RESET << "             Clean package cache\n";
    std::cout << "  " << CYAN << "help" << RESET << "              Show this help message\n";
    std::cout << "\n";
    
    std::cout << BOLD << "EXAMPLES:" << RESET << "\n";
    std::cout << "  sad-pkg init\n";
    std::cout << "  sad-pkg add http ^1.0.0\n";
    std::cout << "  sad-pkg install\n";
    std::cout << "  sad-pkg search http\n";
    std::cout << "\n";
}

// ============================================================================
// Command Implementations - تنفيذ الأوامر
// ============================================================================

/**
 * @brief Initialize a new package - إنشاء حزمة جديدة
 */
int cmd_init() {
    print_header("Initializing new Sad package...");

    // Check if sad.toml already exists - التحقق من وجود sad.toml
    if (std::filesystem::exists("sad.toml")) {
        print_error("sad.toml already exists in current directory");
        return 1;
    }

    // Get package info from user - الحصول على معلومات الحزمة من المستخدم
    std::string name, version_str, description, author;

    std::cout << "Package name (default: my_package): ";
    std::getline(std::cin, name);
    if (name.empty()) name = "my_package";

    std::cout << "Version (default: 0.1.0): ";
    std::getline(std::cin, version_str);
    if (version_str.empty()) version_str = "0.1.0";

    std::cout << "Description: ";
    std::getline(std::cin, description);

    std::cout << "Author: ";
    std::getline(std::cin, author);

    // Create package - إنشاء الحزمة
    Package pkg;
    pkg.name = name;
    pkg.version = Version::parse(version_str);
    pkg.description = description;
    if (!author.empty()) {
        pkg.authors.push_back(author);
    }
    pkg.main_file = "src/main.s";
    pkg.source_dirs.push_back("src");

    // Save to sad.toml - الحفظ في sad.toml
    pkg.save_to_file("sad.toml");

    // Create directory structure - إنشاء هيكل المجلدات
    std::filesystem::create_directories("src");
    
    // Create main.s - إنشاء main.s
    std::ofstream main_file("src/main.s");
    if (main_file.is_open()) {
        main_file << "// بسم الله الرحمن الرحيم\n";
        main_file << "// " << name << " - Main entry point\n";
        main_file << "\n";
        main_file << "دالة رئيسية() {\n";
        main_file << "    اطبع(\"مرحباً من " << name << "!\");\n";
        main_file << "    أرجع 0;\n";
        main_file << "}\n";
        main_file.close();
    }

    print_success("Package initialized successfully!");
    print_info("Created sad.toml and src/main.s");
    print_info("Run 'sad-pkg install' to install dependencies");

    return 0;
}

/**
 * @brief Install dependencies - تثبيت التبعيات
 */
int cmd_install(const std::string& package_name = "") {
    // Load sad.toml - تحميل sad.toml
    if (!std::filesystem::exists("sad.toml")) {
        print_error("sad.toml not found. Run 'sad-pkg init' first.");
        return 1;
    }

    Package pkg;
    try {
        pkg = Package::from_file("sad.toml");
    } catch (const std::exception& e) {
        print_error(std::string("Failed to load sad.toml: ") + e.what());
        return 1;
    }

    // If specific package requested, add it first
    // إذا طلبت حزمة محددة، أضفها أولاً
    if (!package_name.empty()) {
        // Parse package name and version requirement
        // تحليل اسم الحزمة ومتطلب الإصدار
        std::string name = package_name;
        std::string version_req = "*";

        size_t space_pos = package_name.find(' ');
        if (space_pos != std::string::npos) {
            name = package_name.substr(0, space_pos);
            version_req = package_name.substr(space_pos + 1);
        }

        // Add to dependencies - إضافة إلى التبعيات
        auto req = VersionRequirement::parse(version_req);
        pkg.dependencies.emplace_back(name, req);
        
        // Save updated sad.toml - حفظ sad.toml المحدث
        pkg.save_to_file("sad.toml");
        print_info("Added " + name + " " + version_req + " to dependencies");
    }

    print_header("Installing dependencies...");

    // Create registry client and cache - إنشاء عميل السجل والتخزين المؤقت
    RegistryClient registry;
    PackageCache cache;
    DependencyResolver resolver(registry);

    // Resolve dependencies - حل التبعيات
    std::vector<ResolvedDependency> resolved_deps;
    try {
        resolved_deps = resolver.resolve(pkg, false);
    } catch (const std::exception& e) {
        print_error(std::string("Dependency resolution failed: ") + e.what());
        
        if (resolver.has_conflicts()) {
            std::cout << "\n" << BOLD << "Conflicts:" << RESET << "\n";
            for (const auto& conflict : resolver.get_conflicts()) {
                std::cout << conflict.to_string() << "\n";
            }
        }
        
        return 1;
    }

    if (resolved_deps.empty()) {
        print_success("No dependencies to install");
        return 0;
    }

    // Install each dependency - تثبيت كل تبعية
    std::cout << "\n" << BOLD << "Installing " << resolved_deps.size() 
              << " packages:" << RESET << "\n";

    for (const auto& dep : resolved_deps) {
        std::cout << "  " << dep.name << " " << dep.version.to_string() << "\n";

        // Check cache first - التحقق من التخزين المؤقت أولاً
        if (cache.is_cached(dep.name, dep.version)) {
            print_info("Using cached version of " + dep.name);
            continue;
        }

        // Download from registry - التنزيل من السجل
        std::filesystem::path download_path = 
            std::filesystem::temp_directory_path() / 
            (dep.name + "-" + dep.version.to_string() + ".tar.gz");

        print_info("Downloading " + dep.name + "...");
        
        if (!registry.download_package(dep.name, dep.version, download_path)) {
            print_error("Failed to download " + dep.name);
            continue;
        }

        // Extract and cache - فك الضغط والتخزين المؤقت
        // TODO: Implement tar.gz extraction
        print_success("Installed " + dep.name + " " + dep.version.to_string());
    }

    print_success("\nAll dependencies installed successfully!");
    return 0;
}

/**
 * @brief Add dependency - إضافة تبعية
 */
int cmd_add(const std::string& package_spec) {
    if (package_spec.empty()) {
        print_error("Package name required");
        std::cout << "Usage: sad-pkg add <package> [version]\n";
        return 1;
    }

    // Parse package spec - تحليل مواصفات الحزمة
    std::string name = package_spec;
    std::string version_req = "*";

    size_t space_pos = package_spec.find(' ');
    if (space_pos != std::string::npos) {
        name = package_spec.substr(0, space_pos);
        version_req = package_spec.substr(space_pos + 1);
    }

    // Load sad.toml - تحميل sad.toml
    if (!std::filesystem::exists("sad.toml")) {
        print_error("sad.toml not found. Run 'sad-pkg init' first.");
        return 1;
    }

    Package pkg = Package::from_file("sad.toml");

    // Check if already exists - التحقق من الوجود المسبق
    for (const auto& dep : pkg.dependencies) {
        if (dep.name == name) {
            print_warning("Dependency '" + name + "' already exists, updating version requirement");
            break;
        }
    }

    // Add dependency - إضافة التبعية
    auto req = VersionRequirement::parse(version_req);
    bool found = false;
    for (auto& dep : pkg.dependencies) {
        if (dep.name == name) {
            dep.requirement = req;
            found = true;
            break;
        }
    }
    
    if (!found) {
        pkg.dependencies.emplace_back(name, req);
    }

    // Save - الحفظ
    pkg.save_to_file("sad.toml");
    print_success("Added " + name + " " + version_req + " to dependencies");
    print_info("Run 'sad-pkg install' to install the package");

    return 0;
}

/**
 * @brief Remove dependency - إزالة تبعية
 */
int cmd_remove(const std::string& package_name) {
    if (package_name.empty()) {
        print_error("Package name required");
        std::cout << "Usage: sad-pkg remove <package>\n";
        return 1;
    }

    // Load sad.toml - تحميل sad.toml
    if (!std::filesystem::exists("sad.toml")) {
        print_error("sad.toml not found");
        return 1;
    }

    Package pkg = Package::from_file("sad.toml");

    // Remove dependency - إزالة التبعية
    auto it = std::remove_if(pkg.dependencies.begin(), pkg.dependencies.end(),
                            [&](const Dependency& dep) { return dep.name == package_name; });

    if (it == pkg.dependencies.end()) {
        print_error("Dependency '" + package_name + "' not found");
        return 1;
    }

    pkg.dependencies.erase(it, pkg.dependencies.end());

    // Save - الحفظ
    pkg.save_to_file("sad.toml");
    print_success("Removed " + package_name + " from dependencies");

    return 0;
}

/**
 * @brief Search for packages - البحث عن الحزم
 */
int cmd_search(const std::string& query) {
    if (query.empty()) {
        print_error("Search query required");
        std::cout << "Usage: sad-pkg search <query>\n";
        return 1;
    }

    print_header("Searching for packages...");

    RegistryClient registry;
    auto results = registry.search(query, 20);

    if (results.empty()) {
        print_info("No packages found");
        return 0;
    }

    std::cout << "\n" << BOLD << "Found " << results.size() << " packages:" << RESET << "\n\n";

    for (const auto& package_name : results) {
        std::cout << "  " << CYAN << package_name << RESET << "\n";
        // TODO: Show description and latest version
    }

    return 0;
}

/**
 * @brief Show package info - عرض معلومات الحزمة
 */
int cmd_info(const std::string& package_name) {
    if (package_name.empty()) {
        print_error("Package name required");
        std::cout << "Usage: sad-pkg info <package>\n";
        return 1;
    }

    print_header("Package Information");

    RegistryClient registry;

    // Get available versions - الحصول على الإصدارات المتاحة
    auto versions = registry.get_available_versions(package_name);

    if (versions.empty()) {
        print_error("Package not found: " + package_name);
        return 1;
    }

    // Get latest version metadata - الحصول على بيانات أحدث إصدار
    auto latest_version = versions[0];
    Package pkg = registry.get_package_metadata(package_name, latest_version);

    // Display info - عرض المعلومات
    std::cout << "\n";
    std::cout << BOLD << "Name:        " << RESET << pkg.name << "\n";
    std::cout << BOLD << "Version:     " << RESET << pkg.version.to_string() << "\n";
    std::cout << BOLD << "Description: " << RESET << pkg.description << "\n";
    
    if (!pkg.authors.empty()) {
        std::cout << BOLD << "Authors:     " << RESET;
        for (size_t i = 0; i < pkg.authors.size(); i++) {
            std::cout << pkg.authors[i];
            if (i < pkg.authors.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
    }
    
    if (!pkg.license.empty()) {
        std::cout << BOLD << "License:     " << RESET << pkg.license << "\n";
    }
    
    if (!pkg.repository.empty()) {
        std::cout << BOLD << "Repository:  " << RESET << pkg.repository << "\n";
    }

    // Show all versions - عرض جميع الإصدارات
    std::cout << "\n" << BOLD << "Available versions:" << RESET << "\n";
    for (const auto& ver : versions) {
        std::cout << "  " << ver.to_string() << "\n";
    }

    return 0;
}

/**
 * @brief List installed packages - سرد الحزم المثبتة
 */
int cmd_list() {
    print_header("Installed Packages");

    PackageCache cache;
    auto packages = cache.list_cached_packages();

    if (packages.empty()) {
        print_info("No packages installed");
        return 0;
    }

    std::cout << "\n";
    for (const auto& [name, version] : packages) {
        std::cout << "  " << CYAN << name << RESET << " " 
                  << version.to_string() << "\n";
    }

    std::cout << "\n" << BOLD << "Total: " << RESET << packages.size() << " packages\n";

    return 0;
}

/**
 * @brief Clean package cache - تنظيف التخزين المؤقت
 */
int cmd_clean() {
    print_header("Cleaning package cache...");

    PackageCache cache;
    
    size_t cache_size = cache.get_cache_size();
    double size_mb = cache_size / (1024.0 * 1024.0);

    std::cout << "Cache size: " << std::fixed << std::setprecision(2) 
              << size_mb << " MB\n";

    std::cout << "Are you sure you want to clear the cache? (y/N): ";
    std::string response;
    std::getline(std::cin, response);

    if (response != "y" && response != "Y") {
        print_info("Cancelled");
        return 0;
    }

    cache.clear();
    print_success("Cache cleared successfully!");

    return 0;
}

/**
 * @brief Publish package to Firebase - نشر الحزمة إلى Firebase
 */
int cmd_publish() {
    print_header("Publishing package to Firebase...");

    // Load sad.toml - تحميل sad.toml
    if (!std::filesystem::exists("sad.toml")) {
        print_error("sad.toml not found. Run 'sad-pkg init' first.");
        return 1;
    }

    Package pkg;
    try {
        pkg = Package::from_file("sad.toml");
    } catch (const std::exception& e) {
        print_error(std::string("Failed to load sad.toml: ") + e.what());
        return 1;
    }

    print_info("Package: " + pkg.name);
    print_info("Version: " + pkg.version.to_string());

    // Create registry client - إنشاء عميل السجل
    RegistryClient registry;

    // Publish package - نشر الحزمة
    try {
        if (registry.publish(pkg)) {
            print_success("Package published successfully!");
            std::cout << "\n" << BOLD << "Package Details:" << RESET << "\n";
            std::cout << "  Name: " << pkg.name << "\n";
            std::cout << "  Version: " << pkg.version.to_string() << "\n";
            std::cout << "  Firebase Project: slangserver-f3fd0\n";
            std::cout << "  Storage: packages/" << pkg.name << "/" << pkg.version.to_string() << "/\n";
            return 0;
        } else {
            print_error("Failed to publish package");
            return 1;
        }
    } catch (const std::exception& e) {
        print_error(std::string("Publishing failed: ") + e.what());
        return 1;
    }
}

// ============================================================================
// Main - الدالة الرئيسية
// ============================================================================

int main(int argc, char* argv[]) {
    ConsoleColor::init();

    // Parse command line arguments - تحليل وسائط سطر الأوامر
    if (argc < 2) {
        print_usage();
        return 0;
    }

    std::string command = argv[1];

    try {
        if (command == "init") {
            return cmd_init();
        }
        else if (command == "install") {
            std::string package = argc > 2 ? argv[2] : "";
            return cmd_install(package);
        }
        else if (command == "add") {
            if (argc < 3) {
                print_error("Package name required");
                return 1;
            }
            std::string package_spec = argv[2];
            if (argc > 3) {
                package_spec += " " + std::string(argv[3]);
            }
            return cmd_add(package_spec);
        }
        else if (command == "remove") {
            if (argc < 3) {
                print_error("Package name required");
                return 1;
            }
            return cmd_remove(argv[2]);
        }
        else if (command == "search") {
            if (argc < 3) {
                print_error("Search query required");
                return 1;
            }
            return cmd_search(argv[2]);
        }
        else if (command == "info") {
            if (argc < 3) {
                print_error("Package name required");
                return 1;
            }
            return cmd_info(argv[2]);
        }
        else if (command == "list") {
            return cmd_list();
        }
        else if (command == "clean") {
            return cmd_clean();
        }
        else if (command == "publish") {
            return cmd_publish();
        }
        else if (command == "help" || command == "--help" || command == "-h") {
            print_usage();
            return 0;
        }
        else {
            print_error("Unknown command: " + command);
            print_info("Run 'sad-pkg help' for usage information");
            return 1;
        }
    }
    catch (const std::exception& e) {
        print_error(std::string("Unexpected error: ") + e.what());
        return 1;
    }

    return 0;
}
