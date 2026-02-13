// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: package_manager.hpp
 * الوصف: ملف الترويسة لنواة مدير الحزم
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <memory>

namespace sad {
namespace pkg {

// الإعلان المسبق
class RegistryClient;
class DependencyResolver;
class SemanticVersion;
class VersionRequirement;
struct PackageInfo;

/**
 * @class PackageManager
 * @brief مدير حزم لغة ص
 */
class PackageManager {
public:
    explicit PackageManager(const std::string& project_dir = ".");
    
    // عمليات المشروع
    bool init_project(const std::string& name, const std::string& name_ar = "");
    bool load_project();
    
    // عمليات الحزم
    bool install_all();
    bool install_package(const std::string& package_name, 
                        const std::string& version_req = "*",
                        bool save_to_config = true);
    bool remove_package(const std::string& package_name);
    bool update_all();
    bool update_package(const std::string& package_name);
    
    // التخزين المؤقت
    bool clean_cache();
    size_t get_cache_size();
    
    // البحث والمعلومات
    std::vector<PackageInfo> search(const std::string& query);
    std::optional<PackageInfo> get_package_info(const std::string& package_name);
    std::vector<std::pair<std::string, SemanticVersion>> list_installed();

private:
    std::string project_dir_;
    std::string cache_dir_;
    std::string deps_dir_;
    PackageInfo current_package_;
    std::map<std::string, SemanticVersion> locked_versions_;
    std::unique_ptr<RegistryClient> registry_client_;
    std::unique_ptr<DependencyResolver> dependency_resolver_;
    
    std::string get_global_cache_dir();
    std::string find_config_file();
    PackageInfo load_package_info(const std::string& path);
    void save_config_file();
    void load_lock_file();
    void save_lock_file(const std::map<std::string, SemanticVersion>& versions);
    std::map<std::string, SemanticVersion> resolve_dependencies(
        const std::map<std::string, VersionRequirement>& deps);
    bool install_package_internal(const std::string& name, const SemanticVersion& version);
    
    void log_info(const std::string& msg);
    void log_success(const std::string& msg);
    void log_warning(const std::string& msg);
    void log_error(const std::string& msg);
};

} // namespace pkg
} // namespace sad
