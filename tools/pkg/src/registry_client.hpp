// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: registry_client.hpp
 * الوصف: ملف الترويسة لعميل المستودع - Header for Registry Client
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T091
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_PKG_REGISTRY_CLIENT_HPP
#define SAD_PKG_REGISTRY_CLIENT_HPP

#include <string>
#include <vector>
#include <map>
#include <optional>

namespace sad {
namespace pkg {

// إعلان مسبق
struct VersionRequirement;

/**
 * @struct نتيجة_البحث
 * @brief نتيجة البحث في المستودع
 */
struct SearchResult {
    std::string name;           // اسم الحزمة
    std::string description;    // الوصف
    std::string latest_version; // أحدث إصدار
    std::string author;         // المؤلف
    int downloads;              // عدد التنزيلات
    std::string updated_at;     // تاريخ آخر تحديث
};

/**
 * @struct معلومات_الحزمة_المستودع
 * @brief معلومات حزمة من المستودع
 */
struct RegistryPackageInfo {
    std::string name;                                       // اسم الحزمة
    std::string description;                                // الوصف
    std::string author;                                     // المؤلف
    std::string repository;                                 // مستودع الكود
    std::string license;                                    // الترخيص
    std::vector<std::string> versions;                      // الإصدارات المتاحة
    std::vector<std::string> keywords;                      // الكلمات المفتاحية
    std::map<std::string, VersionRequirement> dependencies; // التبعيات
    int total_downloads;                                    // إجمالي التنزيلات
    std::string created_at;                                 // تاريخ الإنشاء
    std::string updated_at;                                 // تاريخ التحديث
};

/**
 * @class عميل_المستودع
 * @brief يتواصل مع مستودع الحزم الرسمي
 */
class RegistryClient {
public:
    /**
     * @brief المنشئ
     */
    explicit RegistryClient(const std::string& registry_url = "https://packages.sad-lang.org");
    
    // البحث والاستعلام
    std::vector<SearchResult> search(const std::string& query, int page = 1, int per_page = 20);
    std::optional<RegistryPackageInfo> get_package_info(const std::string& name);
    std::optional<RegistryPackageInfo> get_package_info(const std::string& name, const std::string& version);
    std::vector<std::string> get_versions(const std::string& name);
    
    // التنزيل والنشر
    std::string download_package(const std::string& name, const std::string& version, const std::string& dest_dir);
    bool publish_package(const std::string& tarball_path);
    
    // المصادقة
    bool login(const std::string& username, const std::string& password);
    void logout();
    bool is_logged_in() const;
    
    // الذاكرة المؤقتة
    void clean_cache(int max_age_days = 7);
    
    // إعدادات
    void set_registry_url(const std::string& url);
    std::string get_registry_url() const;
};

} // namespace pkg
} // namespace sad

#endif // SAD_PKG_REGISTRY_CLIENT_HPP
