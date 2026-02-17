// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: dependency_resolver.h
 * الوصف: محرك حل التبعيات لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يوفر:
 *   ● تحليل قيود الإصدارات (SemVer)
 *   ● حل التبعيات باستخدام خوارزمية PubGrub
 *   ● كشف التعارضات والدورات
 *   ● تخزين مؤقت لنتائج الحل
 *   ● دعم التبعيات الاختيارية والتطويرية
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <functional>
#include <memory>

namespace sad {
namespace registry {

// Forward declaration
class Database;

namespace deps {

// ═══════════════════════════════════════════════════════════════════════════════
// هياكل الإصدارات الدلالية (Semantic Versioning)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct إصدار
 * @brief إصدار دلالي (Major.Minor.Patch-prerelease+build)
 */
struct Version {
    int major = 0;
    int minor = 0;
    int patch = 0;
    std::string prerelease;
    std::string build_metadata;
    
    // Constructors
    Version() = default;
    Version(int maj, int min, int pat) : major(maj), minor(min), patch(pat) {}
    explicit Version(const std::string& str);
    
    // Formatting
    std::string to_string() const;
    
    // Comparison
    bool operator==(const Version& other) const;
    bool operator!=(const Version& other) const { return !(*this == other); }
    bool operator<(const Version& other) const;
    bool operator<=(const Version& other) const { return *this < other || *this == other; }
    bool operator>(const Version& other) const { return other < *this; }
    bool operator>=(const Version& other) const { return !(*this < other); }
    
    // Utilities
    bool is_prerelease() const { return !prerelease.empty(); }
    Version next_major() const { return Version(major + 1, 0, 0); }
    Version next_minor() const { return Version(major, minor + 1, 0); }
    Version next_patch() const { return Version(major, minor, patch + 1); }
};

/**
 * @enum نوع_القيد
 * @brief أنواع قيود الإصدارات
 */
enum class ConstraintType {
    Exact,      // =1.2.3 أو 1.2.3
    Greater,    // >1.2.3
    GreaterEq,  // >=1.2.3
    Less,       // <1.2.3
    LessEq,     // <=1.2.3
    Caret,      // ^1.2.3 (متوافق مع 1.x.x)
    Tilde,      // ~1.2.3 (متوافق مع 1.2.x)
    Wildcard,   // 1.2.* أو *
    Range,      // >=1.0.0 <2.0.0
    Any         // أي إصدار
};

/**
 * @struct قيد_الإصدار
 * @brief قيد على إصدارات مقبولة
 */
struct VersionConstraint {
    ConstraintType type = ConstraintType::Any;
    Version min_version;
    Version max_version;  // للنطاقات
    bool min_inclusive = true;
    bool max_inclusive = false;
    
    // Parse from string
    static VersionConstraint parse(const std::string& str);
    
    // Check if version satisfies constraint
    bool satisfies(const Version& version) const;
    
    // Format
    std::string to_string() const;
    
    // Intersection
    std::optional<VersionConstraint> intersect(const VersionConstraint& other) const;
    
    // Helpers
    bool is_any() const { return type == ConstraintType::Any; }
    bool is_empty() const;
};

// ═══════════════════════════════════════════════════════════════════════════════
// هياكل التبعيات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum نوع_التبعية
 * @brief أنواع التبعيات
 */
enum class DependencyType {
    Runtime,    // تبعية وقت التشغيل
    Dev,        // تبعية التطوير فقط
    Build,      // تبعية البناء
    Optional,   // تبعية اختيارية
    Peer        // تبعية نظيرة (يجب أن يثبتها المستخدم)
};

/**
 * @struct تبعية
 * @brief متطلب تبعية لحزمة
 */
struct Dependency {
    std::string package_name;
    VersionConstraint constraint;
    DependencyType type = DependencyType::Runtime;
    bool optional = false;
    std::string platform_filter;  // windows, linux, macos, all
    std::vector<std::string> features;  // الميزات المطلوبة
};

/**
 * @struct معلومات_الحزمة_للحل
 * @brief معلومات حزمة لازمة لحل التبعيات
 */
struct PackageVersionInfo {
    std::string name;
    Version version;
    std::vector<Dependency> dependencies;
    std::string checksum;
    bool yanked = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
// نتائج الحل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct تعارض
 * @brief تعارض بين تبعيات
 */
struct Conflict {
    std::string package_name;
    std::string reason;
    std::vector<std::string> involved_packages;
    std::vector<VersionConstraint> conflicting_constraints;
};

/**
 * @struct حزمة_محلولة
 * @brief حزمة في نتيجة الحل
 */
struct ResolvedPackage {
    std::string name;
    Version version;
    std::string checksum;
    std::vector<std::string> depends_on;  // أسماء التبعيات المباشرة
    DependencyType type;
    bool is_root = false;
};

/**
 * @struct نتيجة_الحل
 * @brief نتيجة عملية حل التبعيات
 */
struct ResolutionResult {
    bool success = false;
    std::vector<ResolvedPackage> packages;  // مرتبة طبقياً (topologically)
    std::vector<Conflict> conflicts;
    std::string error_message;
    int resolution_time_ms = 0;
    int packages_considered = 0;
    int cache_hits = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
// محرك حل التبعيات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class حالّ_التبعيات
 * @brief محرك حل التبعيات الرئيسي
 *
 * يستخدم خوارزمية مستوحاة من PubGrub (Dart/Pub):
 *   1. بناء شجرة التبعيات
 *   2. اختيار أفضل إصدار متوافق
 *   3. كشف التعارضات والتراجع (backtracking)
 *   4. تخزين مؤقت للنتائج
 */
class DependencyResolver {
public:
    /**
     * @brief مُنشئ
     * @param db قاعدة البيانات لجلب معلومات الحزم
     */
    explicit DependencyResolver(Database& db);
    
    /**
     * @brief حل تبعيات قائمة من الحزم
     * @param root_packages الحزم الجذرية مع قيودها
     * @param include_dev تضمين تبعيات التطوير؟
     * @param platform فلترة حسب المنصة (windows, linux, macos, all)
     */
    ResolutionResult resolve(
        const std::vector<Dependency>& root_packages,
        bool include_dev = false,
        const std::string& platform = "all"
    );
    
    /**
     * @brief حل تبعيات ملف حزمة (sad.toml)
     * @param manifest_path مسار ملف البيان
     */
    ResolutionResult resolve_from_manifest(const std::string& manifest_path);
    
    /**
     * @brief التحقق من توافق إصدار مع القيود
     */
    bool check_compatibility(const std::string& package_name,
                             const Version& version,
                             const std::vector<VersionConstraint>& constraints);
    
    /**
     * @brief جلب الإصدارات المتاحة لحزمة
     */
    std::vector<Version> get_available_versions(const std::string& package_name,
                                                 const VersionConstraint& constraint);
    
    /**
     * @brief كشف الدورات في شجرة التبعيات
     */
    std::vector<std::vector<std::string>> detect_cycles(
        const std::vector<Dependency>& dependencies);
    
    /**
     * @brief تنظيف الذاكرة المؤقتة
     */
    void clear_cache();
    
    // ─────────────────────────────────────────────────────────────────────────
    // خيارات الإعداد
    // ─────────────────────────────────────────────────────────────────────────
    
    /// الحد الأقصى لعمق التبعيات
    void set_max_depth(int depth) { max_depth_ = depth; }
    
    /// الحد الأقصى لعدد الإصدارات للنظر فيها
    void set_max_versions_to_try(int count) { max_versions_to_try_ = count; }
    
    /// تفعيل/تعطيل التخزين المؤقت
    void set_use_cache(bool use) { use_cache_ = use; }
    
    /// تفعيل/تعطيل إصدارات ما قبل الإصدار
    void set_allow_prereleases(bool allow) { allow_prereleases_ = allow; }

private:
    Database& db_;
    
    // Configuration
    int max_depth_ = 100;
    int max_versions_to_try_ = 50;
    bool use_cache_ = true;
    bool allow_prereleases_ = false;
    
    // Cache
    std::map<std::string, std::vector<PackageVersionInfo>> version_cache_;
    
    // Internal state during resolution
    struct State {
        std::map<std::string, Version> selected;
        std::map<std::string, std::vector<VersionConstraint>> constraints;
        std::set<std::string> visited;
        std::vector<std::string> path;
        int depth = 0;
    };
    
    // Internal methods
    bool resolve_recursive(State& state, const Dependency& dep);
    std::optional<Version> select_best_version(const std::string& package,
                                               const std::vector<VersionConstraint>& constraints);
    PackageVersionInfo get_package_info(const std::string& name, const Version& version);
    std::vector<PackageVersionInfo> fetch_versions(const std::string& package_name);
    std::string compute_cache_key(const std::vector<Dependency>& deps);
    std::optional<ResolutionResult> get_cached_result(const std::string& key);
    void cache_result(const std::string& key, const ResolutionResult& result);
};

// ═══════════════════════════════════════════════════════════════════════════════
// دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تحليل سلسلة إصدار
 */
Version parse_version(const std::string& str);

/**
 * @brief تحليل قيد إصدار
 */
VersionConstraint parse_constraint(const std::string& str);

/**
 * @brief ترتيب طبقي للحزم (topological sort)
 */
std::vector<std::string> topological_sort(
    const std::map<std::string, std::vector<std::string>>& graph);

/**
 * @brief تحويل نوع التبعية من نص
 */
DependencyType dependency_type_from_string(const std::string& str);

/**
 * @brief تحويل نوع التبعية إلى نص
 */
std::string dependency_type_to_string(DependencyType type);

} // namespace deps
} // namespace registry
} // namespace sad
