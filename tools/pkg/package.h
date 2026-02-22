// بسم الله الرحمن الرحيم
/**
 * @file package.h
 * @brief Package Management System - نظام إدارة الحزم
 * 
 * يوفر فئات لإدارة حزم Sad Language
 * Provides classes for managing Sad Language packages
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include "toml_parser.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include <filesystem>
#include <algorithm>
#include <regex>

namespace sad {
namespace pkg {

// ============================================================================
// Version - الإصدار
// ============================================================================

/**
 * @brief Semantic version - إصدار دلالي
 * 
 * يمثل إصداراً دلالياً (major.minor.patch)
 * Represents a semantic version (major.minor.patch)
 */
class Version {
public:
    int major;
    int minor;
    int patch;
    std::string prerelease;  // e.g., "alpha.1"
    std::string build;       // e.g., "20130313144700"

    Version(int maj = 0, int min = 0, int pat = 0, 
            const std::string& pre = "", const std::string& bld = "")
        : major(maj), minor(min), patch(pat), prerelease(pre), build(bld) {}

    /**
     * @brief Parse version from string - تحليل الإصدار من نص
     * 
     * Examples: "1.2.3", "1.0.0-alpha.1", "2.1.0+build.123"
     */
    static Version parse(const std::string& version_str) {
        std::regex version_regex(R"((\d+)\.(\d+)\.(\d+)(?:-([a-zA-Z0-9.]+))?(?:\+([a-zA-Z0-9.]+))?)");
        std::smatch match;

        if (!std::regex_match(version_str, match, version_regex)) {
            throw std::invalid_argument("Invalid version format: " + version_str);
        }

        int major = std::stoi(match[1].str());
        int minor = std::stoi(match[2].str());
        int patch = std::stoi(match[3].str());
        std::string prerelease = match[4].str();
        std::string build = match[5].str();

        return Version(major, minor, patch, prerelease, build);
    }

    /**
     * @brief Convert to string - تحويل إلى نص
     */
    std::string to_string() const {
        std::string result = std::to_string(major) + "." + 
                           std::to_string(minor) + "." + 
                           std::to_string(patch);
        
        if (!prerelease.empty()) {
            result += "-" + prerelease;
        }
        
        if (!build.empty()) {
            result += "+" + build;
        }
        
        return result;
    }

    // Comparison operators - عوامل المقارنة
    bool operator==(const Version& other) const {
        return major == other.major && minor == other.minor && 
               patch == other.patch && prerelease == other.prerelease;
    }

    bool operator!=(const Version& other) const {
        return !(*this == other);
    }

    bool operator<(const Version& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        if (patch != other.patch) return patch < other.patch;
        
        // Prerelease versions have lower precedence
        // الإصدارات التجريبية لها أولوية أقل
        if (prerelease.empty() && !other.prerelease.empty()) return false;
        if (!prerelease.empty() && other.prerelease.empty()) return true;
        
        return prerelease < other.prerelease;
    }

    bool operator>(const Version& other) const {
        return other < *this;
    }

    bool operator<=(const Version& other) const {
        return !(other < *this);
    }

    bool operator>=(const Version& other) const {
        return !(*this < other);
    }
};

// ============================================================================
// Version Requirement - متطلب الإصدار
// ============================================================================

/**
 * @brief Version requirement - متطلب الإصدار
 * 
 * يمثل متطلب إصدار (مثل "^1.2.3" أو ">=2.0.0")
 * Represents a version requirement (like "^1.2.3" or ">=2.0.0")
 */
class VersionRequirement {
public:
    enum class Operator {
        EXACT,      // "1.2.3"
        CARET,      // "^1.2.3" - compatible with 1.x.x
        TILDE,      // "~1.2.3" - compatible with 1.2.x
        GT,         // ">1.2.3"
        GTE,        // ">=1.2.3"
        LT,         // "<1.2.3"
        LTE,        // "<=1.2.3"
        WILDCARD    // "*" or "1.x.x"
    };

    Operator op;
    Version version;

    VersionRequirement(Operator o, const Version& v) : op(o), version(v) {}

    /**
     * @brief Parse requirement from string - تحليل المتطلب من نص
     */
    static VersionRequirement parse(const std::string& req_str) {
        if (req_str == "*") {
            return VersionRequirement(Operator::WILDCARD, Version());
        }

        if (req_str.rfind("^", 0) == 0) {
            return VersionRequirement(Operator::CARET, 
                                    Version::parse(req_str.substr(1)));
        }

        if (req_str.rfind("~", 0) == 0) {
            return VersionRequirement(Operator::TILDE, 
                                    Version::parse(req_str.substr(1)));
        }

        if (req_str.rfind(">=", 0) == 0) {
            return VersionRequirement(Operator::GTE, 
                                    Version::parse(req_str.substr(2)));
        }

        if (req_str.rfind(">", 0) == 0) {
            return VersionRequirement(Operator::GT, 
                                    Version::parse(req_str.substr(1)));
        }

        if (req_str.rfind("<=", 0) == 0) {
            return VersionRequirement(Operator::LTE, 
                                    Version::parse(req_str.substr(2)));
        }

        if (req_str.rfind("<", 0) == 0) {
            return VersionRequirement(Operator::LT, 
                                    Version::parse(req_str.substr(1)));
        }

        // Exact version - إصدار دقيق
        return VersionRequirement(Operator::EXACT, Version::parse(req_str));
    }

    /**
     * @brief Check if version satisfies requirement - التحقق من تطابق الإصدار مع المتطلب
     */
    bool matches(const Version& v) const {
        switch (op) {
            case Operator::EXACT:
                return v == version;
            
            case Operator::CARET:
                // ^1.2.3 means >=1.2.3 and <2.0.0
                if (version.major == 0) {
                    // ^0.2.3 means >=0.2.3 and <0.3.0
                    return v >= version && 
                           v.major == version.major && 
                           v.minor == version.minor;
                }
                return v >= version && v.major == version.major;
            
            case Operator::TILDE:
                // ~1.2.3 means >=1.2.3 and <1.3.0
                return v >= version && 
                       v.major == version.major && 
                       v.minor == version.minor;
            
            case Operator::GT:
                return v > version;
            
            case Operator::GTE:
                return v >= version;
            
            case Operator::LT:
                return v < version;
            
            case Operator::LTE:
                return v <= version;
            
            case Operator::WILDCARD:
                return true;
        }
        
        return false;
    }

    std::string to_string() const {
        switch (op) {
            case Operator::EXACT: return version.to_string();
            case Operator::CARET: return "^" + version.to_string();
            case Operator::TILDE: return "~" + version.to_string();
            case Operator::GT: return ">" + version.to_string();
            case Operator::GTE: return ">=" + version.to_string();
            case Operator::LT: return "<" + version.to_string();
            case Operator::LTE: return "<=" + version.to_string();
            case Operator::WILDCARD: return "*";
        }
        return "";
    }
};

// ============================================================================
// Dependency - التبعية
// ============================================================================

/**
 * @brief Package dependency - تبعية حزمة
 */
struct Dependency {
    std::string name;
    VersionRequirement requirement;
    bool optional;
    std::string registry;  // Custom registry URL

    Dependency(const std::string& n, const VersionRequirement& req, 
              bool opt = false, const std::string& reg = "")
        : name(n), requirement(req), optional(opt), registry(reg) {}

    std::string to_string() const {
        return name + " " + requirement.to_string() + 
               (optional ? " (optional)" : "");
    }
};

// ============================================================================
// Package Metadata - بيانات الحزمة
// ============================================================================

/**
 * @brief Package metadata from sad.toml - بيانات الحزمة من sad.toml
 */
class Package {
public:
    // Basic info - معلومات أساسية
    std::string name;
    Version version;
    std::string description;
    std::vector<std::string> authors;
    std::string license;
    std::string repository;
    std::string homepage;
    std::string readme;
    std::vector<std::string> keywords;
    std::vector<std::string> categories;

    // Dependencies - التبعيات
    std::vector<Dependency> dependencies;
    std::vector<Dependency> dev_dependencies;
    std::vector<Dependency> build_dependencies;

    // Build info - معلومات البناء
    std::string main_file;           // Main entry point
    std::vector<std::string> source_dirs;
    std::vector<std::string> include_dirs;
    std::vector<std::string> exclude_patterns;

    // Package location - موقع الحزمة
    std::filesystem::path package_dir;

    /**
     * @brief Load package from sad.toml file - تحميل الحزمة من ملف sad.toml
     */
    static Package from_file(const std::filesystem::path& toml_path) {
        if (!std::filesystem::exists(toml_path)) {
            throw std::runtime_error("Package file not found: " + toml_path.string());
        }

        auto toml = TomlParser::parse_file(toml_path.string());
        Package pkg;

        pkg.package_dir = toml_path.parent_path();

        // Parse package section - تحليل قسم الحزمة
        if (toml.find("package") == toml.end()) {
            throw std::runtime_error("Missing [package] section in sad.toml");
        }

        const auto& package_section = toml["package"].as_table();

        // Required fields - حقول مطلوبة
        if (!package_section.count("name")) {
            throw std::runtime_error("Missing 'name' in [package]");
        }
        pkg.name = package_section.at("name").as_string();

        if (!package_section.count("version")) {
            throw std::runtime_error("Missing 'version' in [package]");
        }
        pkg.version = Version::parse(package_section.at("version").as_string());

        // Optional fields - حقول اختيارية
        if (package_section.count("description")) {
            pkg.description = package_section.at("description").as_string();
        }

        if (package_section.count("authors")) {
            const auto& authors_arr = package_section.at("authors").as_array();
            for (const auto& author : authors_arr) {
                pkg.authors.push_back(author.as_string());
            }
        }

        if (package_section.count("license")) {
            pkg.license = package_section.at("license").as_string();
        }

        if (package_section.count("repository")) {
            pkg.repository = package_section.at("repository").as_string();
        }

        if (package_section.count("homepage")) {
            pkg.homepage = package_section.at("homepage").as_string();
        }

        if (package_section.count("readme")) {
            pkg.readme = package_section.at("readme").as_string();
        }

        if (package_section.count("keywords")) {
            const auto& keywords_arr = package_section.at("keywords").as_array();
            for (const auto& kw : keywords_arr) {
                pkg.keywords.push_back(kw.as_string());
            }
        }

        if (package_section.count("categories")) {
            const auto& cat_arr = package_section.at("categories").as_array();
            for (const auto& cat : cat_arr) {
                pkg.categories.push_back(cat.as_string());
            }
        }

        // Parse dependencies - تحليل التبعيات
        if (toml.find("dependencies") != toml.end()) {
            const auto& deps = toml["dependencies"].as_table();
            for (const auto& [name, version_str] : deps) {
                auto req = VersionRequirement::parse(version_str.as_string());
                pkg.dependencies.emplace_back(name, req);
            }
        }

        if (toml.find("dev-dependencies") != toml.end()) {
            const auto& dev_deps = toml["dev-dependencies"].as_table();
            for (const auto& [name, version_str] : dev_deps) {
                auto req = VersionRequirement::parse(version_str.as_string());
                pkg.dev_dependencies.emplace_back(name, req);
            }
        }

        if (toml.find("build-dependencies") != toml.end()) {
            const auto& build_deps = toml["build-dependencies"].as_table();
            for (const auto& [name, version_str] : build_deps) {
                auto req = VersionRequirement::parse(version_str.as_string());
                pkg.build_dependencies.emplace_back(name, req);
            }
        }

        // Parse build section - تحليل قسم البناء
        if (toml.find("build") != toml.end()) {
            const auto& build_section = toml["build"].as_table();

            if (build_section.count("main")) {
                pkg.main_file = build_section.at("main").as_string();
            }

            if (build_section.count("source-dirs")) {
                const auto& src_dirs = build_section.at("source-dirs").as_array();
                for (const auto& dir : src_dirs) {
                    pkg.source_dirs.push_back(dir.as_string());
                }
            } else {
                pkg.source_dirs.push_back("src");  // Default
            }

            if (build_section.count("include-dirs")) {
                const auto& inc_dirs = build_section.at("include-dirs").as_array();
                for (const auto& dir : inc_dirs) {
                    pkg.include_dirs.push_back(dir.as_string());
                }
            }

            if (build_section.count("exclude")) {
                const auto& excl_patterns = build_section.at("exclude").as_array();
                for (const auto& pattern : excl_patterns) {
                    pkg.exclude_patterns.push_back(pattern.as_string());
                }
            }
        } else {
            pkg.source_dirs.push_back("src");  // Default
        }

        return pkg;
    }

    /**
     * @brief Save package to sad.toml file - حفظ الحزمة إلى ملف sad.toml
     */
    void save_to_file(const std::filesystem::path& toml_path) const {
        TomlTable toml;

        // Package section - قسم الحزمة
        TomlTable package_section;
        package_section["name"] = name;
        package_section["version"] = version.to_string();
        
        if (!description.empty()) {
            package_section["description"] = description;
        }
        
        if (!authors.empty()) {
            TomlArray authors_arr;
            for (const auto& author : authors) {
                authors_arr.push_back(TomlValue(author));
            }
            package_section["authors"] = authors_arr;
        }
        
        if (!license.empty()) {
            package_section["license"] = license;
        }
        
        if (!repository.empty()) {
            package_section["repository"] = repository;
        }
        
        if (!homepage.empty()) {
            package_section["homepage"] = homepage;
        }
        
        if (!readme.empty()) {
            package_section["readme"] = readme;
        }
        
        if (!keywords.empty()) {
            TomlArray keywords_arr;
            for (const auto& kw : keywords) {
                keywords_arr.push_back(TomlValue(kw));
            }
            package_section["keywords"] = keywords_arr;
        }
        
        if (!categories.empty()) {
            TomlArray cat_arr;
            for (const auto& cat : categories) {
                cat_arr.push_back(TomlValue(cat));
            }
            package_section["categories"] = cat_arr;
        }

        toml["package"] = package_section;

        // Dependencies - التبعيات
        if (!dependencies.empty()) {
            TomlTable deps_section;
            for (const auto& dep : dependencies) {
                deps_section[dep.name] = dep.requirement.to_string();
            }
            toml["dependencies"] = deps_section;
        }

        if (!dev_dependencies.empty()) {
            TomlTable dev_deps_section;
            for (const auto& dep : dev_dependencies) {
                dev_deps_section[dep.name] = dep.requirement.to_string();
            }
            toml["dev-dependencies"] = dev_deps_section;
        }

        if (!build_dependencies.empty()) {
            TomlTable build_deps_section;
            for (const auto& dep : build_dependencies) {
                build_deps_section[dep.name] = dep.requirement.to_string();
            }
            toml["build-dependencies"] = build_deps_section;
        }

        // Build section - قسم البناء
        TomlTable build_section;
        
        if (!main_file.empty()) {
            build_section["main"] = main_file;
        }
        
        if (!source_dirs.empty()) {
            TomlArray src_dirs_arr;
            for (const auto& dir : source_dirs) {
                src_dirs_arr.push_back(TomlValue(dir));
            }
            build_section["source-dirs"] = src_dirs_arr;
        }
        
        if (!include_dirs.empty()) {
            TomlArray inc_dirs_arr;
            for (const auto& dir : include_dirs) {
                inc_dirs_arr.push_back(TomlValue(dir));
            }
            build_section["include-dirs"] = inc_dirs_arr;
        }
        
        if (!exclude_patterns.empty()) {
            TomlArray excl_arr;
            for (const auto& pattern : exclude_patterns) {
                excl_arr.push_back(TomlValue(pattern));
            }
            build_section["exclude"] = excl_arr;
        }

        if (build_section.size() > 0) {
            toml["build"] = build_section;
        }

        // Write to file - الكتابة إلى ملف
        TomlParser::write_file(toml_path.string(), toml);
    }

    /**
     * @brief Get all source files - الحصول على جميع ملفات المصدر
     */
    std::vector<std::filesystem::path> get_source_files() const {
        std::vector<std::filesystem::path> files;

        for (const auto& src_dir : source_dirs) {
            auto full_path = package_dir / src_dir;
            
            if (!std::filesystem::exists(full_path)) {
                continue;
            }

            for (const auto& entry : std::filesystem::recursive_directory_iterator(full_path)) {
                if (!entry.is_regular_file()) continue;

                auto ext = entry.path().extension().string();
                if (ext == ".ص" || ext == u8".ص") {
                    // Check if excluded - التحقق من الاستثناء
                    bool excluded = false;
                    for (const auto& pattern : exclude_patterns) {
                        // Simple pattern matching - مطابقة بسيطة للنمط
                        if (entry.path().string().find(pattern) != std::string::npos) {
                            excluded = true;
                            break;
                        }
                    }

                    if (!excluded) {
                        files.push_back(entry.path());
                    }
                }
            }
        }

        return files;
    }

    /**
     * @brief Validate package metadata - التحقق من صحة بيانات الحزمة
     */
    std::vector<std::string> validate() const {
        std::vector<std::string> errors;

        if (name.empty()) {
            errors.push_back("Package name is required");
        }

        // Package name validation - التحقق من صحة اسم الحزمة
        // Allow Arabic and English package names
        // Arabic names: must contain at least one Arabic letter
        // English names: must start with lowercase letter
        bool has_arabic = false;
        for (unsigned char c : name) {
            if (c >= 0xD8 || c >= 0xC0) { has_arabic = true; break; }
        }
        if (!has_arabic) {
            if (!std::regex_match(name, std::regex("^[a-z][a-z0-9_-]*$"))) {
                errors.push_back("Package name must start with lowercase letter and contain only lowercase letters, numbers, hyphens, and underscores");
            }
        } else {
            // For Arabic names: just check it's not empty and has no spaces
            if (name.find(' ') != std::string::npos) {
                errors.push_back("Package name must not contain spaces");
            }
        }

        if (version.major < 0 || version.minor < 0 || version.patch < 0) {
            errors.push_back("Version numbers must be non-negative");
        }

        if (!main_file.empty()) {
            auto main_path = package_dir / main_file;
            if (!std::filesystem::exists(main_path)) {
                errors.push_back("Main file not found: " + main_file);
            }
        }

        for (const auto& src_dir : source_dirs) {
            auto dir_path = package_dir / src_dir;
            if (!std::filesystem::exists(dir_path)) {
                errors.push_back("Source directory not found: " + src_dir);
            }
        }

        return errors;
    }

    /**
     * @brief Create package manifest string - إنشاء نص بيان الحزمة
     */
    std::string to_manifest_string() const {
        std::stringstream ss;
        ss << "Package: " << name << "\n";
        ss << "Version: " << version.to_string() << "\n";
        
        if (!description.empty()) {
            ss << "Description: " << description << "\n";
        }
        
        if (!authors.empty()) {
            ss << "Authors: ";
            for (size_t i = 0; i < authors.size(); i++) {
                ss << authors[i];
                if (i < authors.size() - 1) ss << ", ";
            }
            ss << "\n";
        }
        
        if (!license.empty()) {
            ss << "License: " << license << "\n";
        }
        
        if (!dependencies.empty()) {
            ss << "Dependencies:\n";
            for (const auto& dep : dependencies) {
                ss << "  - " << dep.to_string() << "\n";
            }
        }
        
        return ss.str();
    }
};

} // namespace pkg
} // namespace sad
