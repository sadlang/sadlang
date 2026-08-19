// ======================================================================
// package_manager.h - نظام حزم لغة ص / Sad Language Package Manager
// ======================================================================
// الوصف بالعربية:
//   مدير حزم يدعم: تثبيت/حذف الحزم، حل التبعيات، مستودعات
//   يعمل مع ملف تبعيات.ص ومجلد حزم/
//
// English Description:
//   Package manager supporting: install/remove, dependency resolution,
//   repositories. Works with تبعيات.ص and حزم/ directory.
// ======================================================================

#ifndef SAD_PACKAGE_MANAGER_H
#define SAD_PACKAGE_MANAGER_H

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <optional>

namespace Sad {
namespace Packages {

/**
 * @brief Package version / إصدار الحزمة
 */
struct Version {
    int major;      ///< رئيسي / major
    int minor;      ///< فرعي / minor
    int patch;      ///< تصحيح / patch
    
    Version(int ma = 0, int mi = 0, int pa = 0)
        : major(ma), minor(mi), patch(pa) {}
    
    std::string toString() const {
        return std::to_string(major) + "." + 
               std::to_string(minor) + "." + 
               std::to_string(patch);
    }
    
    bool operator>=(const Version& other) const {
        if (major != other.major) return major > other.major;
        if (minor != other.minor) return minor > other.minor;
        return patch >= other.patch;
    }
    
    bool operator<(const Version& other) const {
        return !(*this >= other);
    }
    
    /**
     * @brief Parse version string / تحليل نص الإصدار
     */
    static Version parse(const std::string& str) {
        Version v;
        sscanf(str.c_str(), "%d.%d.%d", &v.major, &v.minor, &v.patch);
        return v;
    }
};

/**
 * @brief Package info / معلومات الحزمة
 */
struct PackageInfo {
    std::string name;           ///< اسم الحزمة / package name
    std::string arabicName;     ///< الاسم العربي / Arabic name
    std::string description;    ///< الوصف / description
    Version version;            ///< الإصدار / version
    std::string author;         ///< المؤلف / author
    std::string license;        ///< الرخصة / license
    std::string repository;     ///< المستودع / repository URL
    std::vector<std::string> keywords; ///< كلمات مفتاحية / keywords
    
    struct DepRequirement {
        std::string name;       ///< اسم التبعية / dependency name
        std::string versionReq; ///< قيد الإصدار / version requirement (e.g., ">=1.0.0")
    };
    std::vector<DepRequirement> dependencies;   ///< التبعيات / dependencies
    
    PackageInfo() = default;
    PackageInfo(const std::string& n, const Version& v)
        : name(n), version(v) {}
};

/**
 * @brief Package registry / سجل الحزم
 * 
 * (AR) سجل حزم مركزي للبحث والتحميل
 * (EN) Central package registry for searching and downloading
 * 
 * @example أوامر:
 * - ص حزم بحث "شبكة"
 * - ص حزم ثبّت مكتبة_الشبكة
 * - ص حزم حدّث
 * - ص حزم قائمة
 */
class PackageRegistry {
public:
    PackageRegistry() = default;
    
    /**
     * @brief Search for packages / البحث عن حزم
     */
    std::vector<PackageInfo> search(const std::string& query) const {
        std::vector<PackageInfo> results;
        for (const auto& [name, pkg] : packages_) {
            if (name.find(query) != std::string::npos ||
                pkg.arabicName.find(query) != std::string::npos ||
                pkg.description.find(query) != std::string::npos) {
                results.push_back(pkg);
            }
        }
        // (AR) 🔑 ترتيبٌ بالاسم: `packages_` مُهشَّرةٌ فترتيبُ النتائجِ ترتيبُ
        //      سَلّةٍ يختلفُ بالمنصّة — وهذه قائمةٌ تُعرَضُ على المستخدم. (ISSUE-182)
        // (EN) Sort by name: packages_ is hashed so the result order is a
        //      platform-dependent bucket order, and this list is shown to the user.
        std::sort(results.begin(), results.end(),
                  [](const PackageInfo &left, const PackageInfo &right) {
                      return left.name < right.name;
                  });
        return results;
    }
    
    /**
     * @brief Get package info / معلومات الحزمة
     */
    std::optional<PackageInfo> getPackage(const std::string& name) const {
        auto it = packages_.find(name);
        if (it != packages_.end()) return it->second;
        return std::nullopt;
    }
    
    /**
     * @brief Register package / تسجيل حزمة
     */
    void registerPackage(const PackageInfo& pkg) {
        packages_[pkg.name] = pkg;
    }
    
private:
    std::unordered_map<std::string, PackageInfo> packages_;
};

/**
 * @brief Package manager / مدير الحزم
 */
class PackageManager {
public:
    PackageManager(const std::string& projectRoot)
        : projectRoot_(projectRoot) {}
    
    /**
     * @brief Install a package / تثبيت حزمة
     */
    bool install(const std::string& name, const std::string& versionReq = "");
    
    /**
     * @brief Remove a package / إزالة حزمة
     */
    bool remove(const std::string& name);
    
    /**
     * @brief Update all packages / تحديث جميع الحزم
     */
    bool updateAll();
    
    /**
     * @brief List installed packages / قائمة الحزم المثبتة
     */
    std::vector<PackageInfo> listInstalled() const { return installed_; }
    
    /**
     * @brief Resolve dependencies / حل التبعيات
     * (AR) يحل شجرة التبعيات ويكشف التعارضات
     * (EN) Resolve dependency tree and detect conflicts
     */
    bool resolveDependencies();
    
    /**
     * @brief Get packages directory / مسار مجلد الحزم
     */
    std::string getPackagesDir() const { return projectRoot_ + "/حزم"; }
    
    /**
     * @brief Get lock file path / مسار ملف القفل
     */
    std::string getLockFilePath() const { return projectRoot_ + "/تبعيات.قفل"; }
    
private:
    std::string projectRoot_;
    std::vector<PackageInfo> installed_;
    PackageRegistry registry_;
};

} // namespace Packages
} // namespace Sad

#endif // SAD_PACKAGE_MANAGER_H
