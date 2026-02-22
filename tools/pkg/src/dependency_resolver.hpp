// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: dependency_resolver.hpp
 * الوصف: ملف الترويسة لمحلل التبعيات - Header for Dependency Resolver
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T090
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_PKG_DEPENDENCY_RESOLVER_HPP
#define SAD_PKG_DEPENDENCY_RESOLVER_HPP

#include "semantic_version.hpp"
#include "version_requirement.hpp"

#include <map>
#include <vector>
#include <string>
#include <optional>

namespace sad {
namespace pkg {

// إعلان مسبق للأصناف
class RegistryClient;

/**
 * @struct معلومات_الحزمة_المحلولة
 * @brief معلومات حزمة بعد حل التبعيات
 */
struct ResolvedPackageInfo {
    std::string name;                                    // اسم الحزمة
    SemanticVersion version;                             // الإصدار المحلول
    std::map<std::string, VersionRequirement> dependencies; // التبعيات
    bool from_lockfile = false;                          // هل من ملف القفل؟
};

/**
 * @class محلل_التبعيات
 * @brief يحل تبعيات الحزم ويختار الإصدارات المناسبة
 */
class DependencyResolver {
public:
    /**
     * @brief المنشئ
     */
    DependencyResolver() = default;
    
    /**
     * @brief حل التبعيات
     */
    std::map<std::string, SemanticVersion> resolve(
        const std::map<std::string, VersionRequirement>& direct_deps,
        const std::map<std::string, SemanticVersion>& locked_versions,
        RegistryClient& registry);
    
    /**
     * @brief الحصول على ترتيب التثبيت
     */
    std::vector<std::string> get_installation_order() const;
    
    /**
     * @brief الحصول على شجرة التبعيات
     */
    std::string get_dependency_tree() const;
    
    /**
     * @brief هل يوجد تعارضات؟
     */
    bool has_conflicts() const;
    
    /**
     * @brief الحصول على رسائل التعارضات
     */
    std::vector<std::string> get_conflict_messages() const;
};

} // namespace pkg
} // namespace sad

#endif // SAD_PKG_DEPENDENCY_RESOLVER_HPP
