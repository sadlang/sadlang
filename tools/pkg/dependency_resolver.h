// بسم الله الرحمن الرحيم
/**
 * @file dependency_resolver.h
 * @brief Dependency Resolution System - نظام حل التبعيات
 * 
 * يحل التبعيات ويكشف التعارضات
 * Resolves dependencies and detects conflicts
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include "package.h"
#include <set>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <sstream>

namespace sad {
namespace pkg {

// ============================================================================
// Resolved Dependency - تبعية محلولة
// ============================================================================

/**
 * @brief A resolved dependency with specific version - تبعية محلولة مع إصدار محدد
 */
struct ResolvedDependency {
    std::string name;
    Version version;
    std::vector<ResolvedDependency> dependencies;  // Transitive dependencies
    bool is_dev;
    bool is_build;

    ResolvedDependency() : is_dev(false), is_build(false) {}

    ResolvedDependency(const std::string& n, const Version& v, 
                      bool dev = false, bool build = false)
        : name(n), version(v), is_dev(dev), is_build(build) {}

    std::string to_string() const {
        return name + " " + version.to_string();
    }

    bool operator==(const ResolvedDependency& other) const {
        return name == other.name && version == other.version;
    }

    bool operator<(const ResolvedDependency& other) const {
        if (name != other.name) return name < other.name;
        return version < other.version;
    }
};

// ============================================================================
// Dependency Conflict - تعارض التبعيات
// ============================================================================

/**
 * @brief Represents a conflict between dependencies - يمثل تعارضاً بين التبعيات
 */
struct DependencyConflict {
    std::string package_name;
    std::vector<VersionRequirement> requirements;
    std::vector<std::string> required_by;  // Which packages require this

    std::string to_string() const {
        std::stringstream ss;
        ss << "Conflict for package '" << package_name << "':\n";
        for (size_t i = 0; i < requirements.size(); i++) {
            ss << "  - " << required_by[i] << " requires " 
               << requirements[i].to_string() << "\n";
        }
        return ss.str();
    }
};

// ============================================================================
// Package Registry Interface - واجهة سجل الحزم
// ============================================================================

/**
 * @brief Interface for querying package registry - واجهة للاستعلام عن سجل الحزم
 * 
 * يوفر معلومات عن الحزم المتاحة وإصداراتها
 * Provides information about available packages and their versions
 */
class IPackageRegistry {
public:
    virtual ~IPackageRegistry() = default;

    /**
     * @brief Get all available versions of a package - الحصول على جميع الإصدارات المتاحة لحزمة
     */
    virtual std::vector<Version> get_available_versions(const std::string& package_name) = 0;

    /**
     * @brief Get package metadata for specific version - الحصول على بيانات الحزمة لإصدار محدد
     */
    virtual Package get_package_metadata(const std::string& package_name, 
                                        const Version& version) = 0;

    /**
     * @brief Check if package exists - التحقق من وجود الحزمة
     */
    virtual bool package_exists(const std::string& package_name) = 0;
};

// ============================================================================
// Mock Registry (for testing) - سجل وهمي (للاختبار)
// ============================================================================

/**
 * @brief Mock package registry for testing - سجل حزم وهمي للاختبار
 */
class MockPackageRegistry : public IPackageRegistry {
public:
    // Store packages by name and version
    std::unordered_map<std::string, std::map<Version, Package>> packages_;

    void add_package(const Package& pkg) {
        packages_[pkg.name][pkg.version] = pkg;
    }

    std::vector<Version> get_available_versions(const std::string& package_name) override {
        std::vector<Version> versions;
        
        if (packages_.count(package_name)) {
            for (const auto& [version, pkg] : packages_[package_name]) {
                versions.push_back(version);
            }
            
            // Sort versions descending - ترتيب الإصدارات تنازلياً
            std::sort(versions.begin(), versions.end(), std::greater<Version>());
        }
        
        return versions;
    }

    Package get_package_metadata(const std::string& package_name, 
                                 const Version& version) override {
        if (!packages_.count(package_name) || 
            !packages_[package_name].count(version)) {
            throw std::runtime_error("Package not found: " + package_name + 
                                   " " + version.to_string());
        }
        
        return packages_[package_name][version];
    }

    bool package_exists(const std::string& package_name) override {
        return packages_.count(package_name) > 0;
    }
};

// ============================================================================
// Dependency Resolver - محلل التبعيات
// ============================================================================

/**
 * @brief Resolves package dependencies - يحل تبعيات الحزم
 * 
 * يستخدم خوارزمية backtracking لإيجاد مجموعة متوافقة من التبعيات
 * Uses backtracking algorithm to find compatible set of dependencies
 */
class DependencyResolver {
public:
    DependencyResolver(IPackageRegistry& registry) : registry_(registry) {}

    /**
     * @brief Resolve dependencies for a package - حل التبعيات لحزمة
     * 
     * @return Vector of resolved dependencies in installation order
     */
    std::vector<ResolvedDependency> resolve(const Package& root_package, 
                                           bool include_dev = false) {
        resolved_packages_.clear();
        conflicts_.clear();
        resolution_order_.clear();

        // Start resolution - بدء الحل
        try {
            resolve_package(root_package.name, root_package.version, "", false, false);
            
            // Add root package dependencies - إضافة تبعيات الحزمة الجذرية
            for (const auto& dep : root_package.dependencies) {
                resolve_dependency(dep, root_package.name, false, false);
            }

            if (include_dev) {
                for (const auto& dep : root_package.dev_dependencies) {
                    resolve_dependency(dep, root_package.name, true, false);
                }
            }

            for (const auto& dep : root_package.build_dependencies) {
                resolve_dependency(dep, root_package.name, false, true);
            }

        } catch (const std::exception& e) {
            throw std::runtime_error("Dependency resolution failed: " + 
                                   std::string(e.what()));
        }

        // Return in topological order - الإرجاع في ترتيب توبولوجي
        return get_installation_order();
    }

    /**
     * @brief Get any conflicts found during resolution - الحصول على أي تعارضات وجدت أثناء الحل
     */
    const std::vector<DependencyConflict>& get_conflicts() const {
        return conflicts_;
    }

    /**
     * @brief Check if resolution was successful - التحقق من نجاح الحل
     */
    bool has_conflicts() const {
        return !conflicts_.empty();
    }

private:
    IPackageRegistry& registry_;
    
    // Resolved packages: name -> (version, metadata)
    std::unordered_map<std::string, std::pair<Version, Package>> resolved_packages_;
    
    // Conflicts found during resolution
    std::vector<DependencyConflict> conflicts_;
    
    // Resolution order for installation
    std::vector<ResolvedDependency> resolution_order_;

    /**
     * @brief Resolve a single dependency - حل تبعية واحدة
     */
    void resolve_dependency(const Dependency& dep, 
                           const std::string& required_by,
                           bool is_dev, bool is_build) {
        // Check if package exists - التحقق من وجود الحزمة
        if (!registry_.package_exists(dep.name)) {
            throw std::runtime_error("Package not found: " + dep.name);
        }

        // Get available versions - الحصول على الإصدارات المتاحة
        auto available_versions = registry_.get_available_versions(dep.name);
        
        if (available_versions.empty()) {
            throw std::runtime_error("No versions available for package: " + dep.name);
        }

        // Find best matching version - إيجاد أفضل إصدار مطابق
        std::optional<Version> best_version;
        
        for (const auto& version : available_versions) {
            if (dep.requirement.matches(version)) {
                // Check if this version is compatible with already resolved packages
                // التحقق من توافق هذا الإصدار مع الحزم المحلولة
                if (is_version_compatible(dep.name, version)) {
                    best_version = version;
                    break;  // Take the first (highest) matching version
                }
            }
        }

        if (!best_version) {
            // No compatible version found - لم يتم إيجاد إصدار متوافق
            DependencyConflict conflict;
            conflict.package_name = dep.name;
            conflict.requirements.push_back(dep.requirement);
            conflict.required_by.push_back(required_by);
            
            // Add existing requirements - إضافة المتطلبات الموجودة
            if (resolved_packages_.count(dep.name)) {
                // This is a conflict
                conflict.requirements.push_back(
                    VersionRequirement(VersionRequirement::Operator::EXACT,
                                     resolved_packages_[dep.name].first)
                );
                conflict.required_by.push_back("(already resolved)");
            }
            
            conflicts_.push_back(conflict);
            throw std::runtime_error("Cannot find compatible version for: " + dep.name);
        }

        // Resolve this package and its dependencies recursively
        // حل هذه الحزمة وتبعياتها بشكل تكراري
        resolve_package(dep.name, *best_version, required_by, is_dev, is_build);
    }

    /**
     * @brief Resolve a specific package version - حل إصدار محدد من الحزمة
     */
    void resolve_package(const std::string& name, const Version& version,
                        const std::string& required_by,
                        bool is_dev, bool is_build) {
        // Skip if already resolved with same or compatible version
        // تخطي إذا كان محلولاً بالفعل بنفس الإصدار أو إصدار متوافق
        if (resolved_packages_.count(name)) {
            auto& [resolved_version, resolved_pkg] = resolved_packages_[name];
            if (resolved_version == version) {
                return;  // Already resolved with exact version
            }
            
            // Version conflict - تعارض في الإصدار
            DependencyConflict conflict;
            conflict.package_name = name;
            conflict.requirements.push_back(
                VersionRequirement(VersionRequirement::Operator::EXACT, version)
            );
            conflict.required_by.push_back(required_by);
            conflict.requirements.push_back(
                VersionRequirement(VersionRequirement::Operator::EXACT, resolved_version)
            );
            conflict.required_by.push_back("(already resolved)");
            
            conflicts_.push_back(conflict);
            throw std::runtime_error("Version conflict for package: " + name);
        }

        // Get package metadata - الحصول على بيانات الحزمة
        Package pkg = registry_.get_package_metadata(name, version);
        
        // Mark as resolved - تعليم كمحلول
        resolved_packages_[name] = {version, pkg};
        
        // Add to resolution order - إضافة إلى ترتيب الحل
        ResolvedDependency resolved_dep(name, version, is_dev, is_build);
        
        // Recursively resolve dependencies - حل التبعيات بشكل تكراري
        for (const auto& dep : pkg.dependencies) {
            resolve_dependency(dep, name, is_dev, is_build);
            
            // Add as transitive dependency - إضافة كتبعية متعدية
            if (resolved_packages_.count(dep.name)) {
                resolved_dep.dependencies.push_back(
                    ResolvedDependency(dep.name, 
                                     resolved_packages_[dep.name].first,
                                     is_dev, is_build)
                );
            }
        }
        
        resolution_order_.push_back(resolved_dep);
    }

    /**
     * @brief Check if a version is compatible with resolved packages
     * التحقق من توافق إصدار مع الحزم المحلولة
     */
    bool is_version_compatible(const std::string& package_name, 
                               const Version& version) {
        if (!resolved_packages_.count(package_name)) {
            return true;  // Not yet resolved, so compatible
        }

        // Check if versions are compatible
        // التحقق من توافق الإصدارات
        const auto& resolved_version = resolved_packages_[package_name].first;
        return resolved_version == version;
    }

    /**
     * @brief Get dependencies in installation order (topological sort)
     * الحصول على التبعيات في ترتيب التثبيت (ترتيب توبولوجي)
     */
    std::vector<ResolvedDependency> get_installation_order() {
        // Use topological sort to determine installation order
        // استخدام الترتيب التوبولوجي لتحديد ترتيب التثبيت
        
        std::unordered_map<std::string, std::unordered_set<std::string>> dependencies_graph;
        std::unordered_map<std::string, int> in_degree;
        std::unordered_map<std::string, ResolvedDependency> dep_map;

        // Build graph - بناء الرسم البياني
        for (const auto& dep : resolution_order_) {
            dep_map[dep.name] = dep;
            
            if (!in_degree.count(dep.name)) {
                in_degree[dep.name] = 0;
            }

            for (const auto& child_dep : dep.dependencies) {
                dependencies_graph[child_dep.name].insert(dep.name);
                in_degree[dep.name]++;
            }
        }

        // Kahn's algorithm for topological sort - خوارزمية كان للترتيب التوبولوجي
        std::queue<std::string> queue;
        std::vector<ResolvedDependency> sorted;

        // Add all nodes with no dependencies - إضافة كل العقد بدون تبعيات
        for (const auto& [name, degree] : in_degree) {
            if (degree == 0) {
                queue.push(name);
            }
        }

        while (!queue.empty()) {
            std::string current = queue.front();
            queue.pop();
            
            if (dep_map.count(current)) {
                sorted.push_back(dep_map[current]);
            }

            // Reduce in-degree for dependent packages - تقليل الدرجة الداخلية للحزم التابعة
            if (dependencies_graph.count(current)) {
                for (const auto& dependent : dependencies_graph[current]) {
                    in_degree[dependent]--;
                    if (in_degree[dependent] == 0) {
                        queue.push(dependent);
                    }
                }
            }
        }

        // Check for cycles - التحقق من الدورات
        if (sorted.size() != resolution_order_.size()) {
            throw std::runtime_error("Circular dependency detected");
        }

        return sorted;
    }
};

} // namespace pkg
} // namespace sad
