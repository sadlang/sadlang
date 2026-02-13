// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: package_manager.cpp
 * الوصف: نواة مدير الحزم للغة ص - Sad Package Manager Core
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T089
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على النواة الأساسية لمدير حزم لغة ص، وهو النظام المسؤول عن:
 * 
 * ١. إدارة الحزم:
 *    - تحميل وتثبيت الحزم من المستودع المركزي
 *    - حل التبعيات وضمان التوافق بين الإصدارات
 *    - إزالة الحزم مع إدارة التبعيات المرتبطة
 *    - تحديث الحزم للإصدارات الأحدث
 * 
 * ٢. إدارة المشاريع:
 *    - قراءة وكتابة ملف ص.toml (ملف الإعداد العربي)
 *    - إدارة ملف القفل ص.قفل للإصدارات الثابتة
 *    - التحقق من سلامة الحزم بعد التثبيت
 * 
 * ٣. التخزين المؤقت:
 *    - تخزين الحزم المحملة محلياً لتسريع التثبيت
 *    - إدارة مساحة التخزين المؤقت وتنظيفها
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * مثال على الاستخدام:
 * 
 *   مدير_حزم مدير;
 *   مدير.حمّل_إعدادات("ص.toml");
 *   مدير.ثبّت_حزمة("شبكة", "1.0.0");
 *   مدير.حدّث_الكل();
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0.0
 * 
 * الحمد لله رب العالمين
 */

#include "package_manager.hpp"
#include "../dependency_resolver.h"
#include "../registry_client.h"
#include "../toml_parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace sad {
namespace pkg {

// ═══════════════════════════════════════════════════════════════════════════════
// ثوابت النظام - System Constants
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * اسم ملف الإعداد الافتراضي
 * يمكن استخدام "sad.toml" أو "ص.toml" (الإصدار العربي)
 */
constexpr const char* CONFIG_FILE_NAME = "sad.toml";
constexpr const char* CONFIG_FILE_NAME_AR = "ص.toml";

/**
 * اسم ملف القفل
 * يُستخدم لتثبيت إصدارات التبعيات
 */
constexpr const char* LOCK_FILE_NAME = "sad.lock";
constexpr const char* LOCK_FILE_NAME_AR = "ص.قفل";

/**
 * مجلد التخزين المؤقت الافتراضي
 */
constexpr const char* CACHE_DIR = ".sad_cache";

/**
 * مجلد التبعيات
 */
constexpr const char* DEPS_DIR = "deps";

/**
 * رابط المستودع المركزي الافتراضي
 */
constexpr const char* DEFAULT_REGISTRY = "https://packages.sad-lang.org";


// ═══════════════════════════════════════════════════════════════════════════════
// صنف الإصدار الدلالي - Semantic Version Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class الإصدار_الدلالي
 * @brief يمثل إصداراً دلالياً (Semantic Versioning)
 * 
 * يتبع معيار SemVer 2.0.0:
 * - الإصدار الرئيسي (Major): تغييرات غير متوافقة
 * - الإصدار الثانوي (Minor): ميزات جديدة متوافقة
 * - الإصدار التصحيحي (Patch): إصلاحات متوافقة
 * 
 * مثال: 1.2.3 = رئيسي.ثانوي.تصحيحي
 */
class SemanticVersion {
public:
    int major = 0;      // الإصدار الرئيسي
    int minor = 0;      // الإصدار الثانوي
    int patch = 0;      // الإصدار التصحيحي
    std::string prerelease;  // معرف ما قبل الإصدار (مثل: alpha, beta)
    std::string build;       // معلومات البناء
    
    /**
     * @brief تحليل نص الإصدار
     * @param version_str نص الإصدار (مثل: "1.2.3-beta+build123")
     * @return الإصدار المحلل
     */
    static SemanticVersion parse(const std::string& version_str) {
        SemanticVersion version;
        std::string str = version_str;
        
        // إزالة البادئة 'v' إن وجدت
        if (!str.empty() && (str[0] == 'v' || str[0] == 'V')) {
            str = str.substr(1);
        }
        
        // تحليل معلومات البناء
        size_t build_pos = str.find('+');
        if (build_pos != std::string::npos) {
            version.build = str.substr(build_pos + 1);
            str = str.substr(0, build_pos);
        }
        
        // تحليل معرف ما قبل الإصدار
        size_t prerelease_pos = str.find('-');
        if (prerelease_pos != std::string::npos) {
            version.prerelease = str.substr(prerelease_pos + 1);
            str = str.substr(0, prerelease_pos);
        }
        
        // تحليل الأرقام الرئيسية
        std::istringstream iss(str);
        char dot;
        iss >> version.major;
        if (iss.peek() == '.') {
            iss >> dot >> version.minor;
            if (iss.peek() == '.') {
                iss >> dot >> version.patch;
            }
        }
        
        return version;
    }
    
    /**
     * @brief تحويل الإصدار إلى نص
     */
    std::string to_string() const {
        std::ostringstream oss;
        oss << major << "." << minor << "." << patch;
        if (!prerelease.empty()) {
            oss << "-" << prerelease;
        }
        if (!build.empty()) {
            oss << "+" << build;
        }
        return oss.str();
    }
    
    /**
     * @brief مقارنة إصدارين
     * @return -1 إذا كان هذا الإصدار أقل، 0 إذا متساوي، 1 إذا أكبر
     */
    int compare(const SemanticVersion& other) const {
        // مقارنة الإصدار الرئيسي
        if (major != other.major) return (major < other.major) ? -1 : 1;
        // مقارنة الإصدار الثانوي
        if (minor != other.minor) return (minor < other.minor) ? -1 : 1;
        // مقارنة الإصدار التصحيحي
        if (patch != other.patch) return (patch < other.patch) ? -1 : 1;
        
        // إصدار ما قبل النشر أقل من الإصدار النهائي
        if (prerelease.empty() && !other.prerelease.empty()) return 1;
        if (!prerelease.empty() && other.prerelease.empty()) return -1;
        if (prerelease != other.prerelease) {
            return (prerelease < other.prerelease) ? -1 : 1;
        }
        
        return 0;
    }
    
    // عوامل المقارنة
    bool operator<(const SemanticVersion& other) const { return compare(other) < 0; }
    bool operator>(const SemanticVersion& other) const { return compare(other) > 0; }
    bool operator<=(const SemanticVersion& other) const { return compare(other) <= 0; }
    bool operator>=(const SemanticVersion& other) const { return compare(other) >= 0; }
    bool operator==(const SemanticVersion& other) const { return compare(other) == 0; }
    bool operator!=(const SemanticVersion& other) const { return compare(other) != 0; }
};


// ═══════════════════════════════════════════════════════════════════════════════
// صنف متطلب الإصدار - Version Requirement Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class متطلب_الإصدار
 * @brief يمثل متطلب إصدار بصيغ مختلفة
 * 
 * الصيغ المدعومة:
 * - "1.2.3"      : إصدار محدد
 * - "^1.2.3"     : متوافق مع الإصدار الرئيسي (1.x.x)
 * - "~1.2.3"     : متوافق مع الإصدار الثانوي (1.2.x)
 * - ">=1.2.3"    : أكبر من أو يساوي
 * - ">1.2.3"     : أكبر من
 * - "<=1.2.3"    : أصغر من أو يساوي
 * - "<1.2.3"     : أصغر من
 * - "1.2.3 - 2.0.0" : نطاق
 * - "*"          : أي إصدار
 */
class VersionRequirement {
public:
    enum class Type {
        EXACT,          // إصدار محدد
        CARET,          // متوافق مع الرئيسي (^)
        TILDE,          // متوافق مع الثانوي (~)
        GREATER_THAN,   // أكبر من (>)
        GREATER_EQUAL,  // أكبر من أو يساوي (>=)
        LESS_THAN,      // أصغر من (<)
        LESS_EQUAL,     // أصغر من أو يساوي (<=)
        RANGE,          // نطاق
        ANY             // أي إصدار (*)
    };
    
    Type type = Type::ANY;
    SemanticVersion min_version;
    SemanticVersion max_version;  // يُستخدم للنطاق فقط
    
    /**
     * @brief تحليل نص المتطلب
     */
    static VersionRequirement parse(const std::string& req_str) {
        VersionRequirement req;
        std::string str = req_str;
        
        // إزالة المسافات
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
        
        if (str.empty() || str == "*") {
            req.type = Type::ANY;
            return req;
        }
        
        // التحقق من النطاق
        size_t range_pos = str.find('-');
        if (range_pos != std::string::npos && range_pos > 0 && range_pos < str.length() - 1) {
            // التحقق من أنها ليست جزء من الإصدار
            if (str[range_pos - 1] != '.' && str[range_pos + 1] != '.') {
                req.type = Type::RANGE;
                req.min_version = SemanticVersion::parse(str.substr(0, range_pos));
                req.max_version = SemanticVersion::parse(str.substr(range_pos + 1));
                return req;
            }
        }
        
        // التحقق من البادئات
        if (str[0] == '^') {
            req.type = Type::CARET;
            req.min_version = SemanticVersion::parse(str.substr(1));
        } else if (str[0] == '~') {
            req.type = Type::TILDE;
            req.min_version = SemanticVersion::parse(str.substr(1));
        } else if (str.substr(0, 2) == ">=") {
            req.type = Type::GREATER_EQUAL;
            req.min_version = SemanticVersion::parse(str.substr(2));
        } else if (str.substr(0, 2) == "<=") {
            req.type = Type::LESS_EQUAL;
            req.min_version = SemanticVersion::parse(str.substr(2));
        } else if (str[0] == '>') {
            req.type = Type::GREATER_THAN;
            req.min_version = SemanticVersion::parse(str.substr(1));
        } else if (str[0] == '<') {
            req.type = Type::LESS_THAN;
            req.min_version = SemanticVersion::parse(str.substr(1));
        } else {
            req.type = Type::EXACT;
            req.min_version = SemanticVersion::parse(str);
        }
        
        return req;
    }
    
    /**
     * @brief التحقق من توافق إصدار مع المتطلب
     */
    bool satisfies(const SemanticVersion& version) const {
        switch (type) {
            case Type::ANY:
                return true;
                
            case Type::EXACT:
                return version == min_version;
                
            case Type::CARET:
                // متوافق مع الإصدار الرئيسي
                if (min_version.major == 0) {
                    // للإصدارات 0.x.y، يجب تطابق الثانوي أيضاً
                    return version.major == 0 && 
                           version.minor == min_version.minor &&
                           version >= min_version;
                }
                return version.major == min_version.major && version >= min_version;
                
            case Type::TILDE:
                // متوافق مع الإصدار الثانوي
                return version.major == min_version.major &&
                       version.minor == min_version.minor &&
                       version >= min_version;
                
            case Type::GREATER_THAN:
                return version > min_version;
                
            case Type::GREATER_EQUAL:
                return version >= min_version;
                
            case Type::LESS_THAN:
                return version < min_version;
                
            case Type::LESS_EQUAL:
                return version <= min_version;
                
            case Type::RANGE:
                return version >= min_version && version <= max_version;
        }
        return false;
    }
};


// ═══════════════════════════════════════════════════════════════════════════════
// صنف معلومات الحزمة - Package Info Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class معلومات_الحزمة
 * @brief يحتوي على جميع معلومات حزمة معينة
 */
struct PackageInfo {
    std::string name;                                          // اسم الحزمة
    std::string name_ar;                                       // الاسم العربي
    SemanticVersion version;                                   // الإصدار
    std::string description;                                   // الوصف
    std::string description_ar;                                // الوصف العربي
    std::vector<std::string> authors;                          // المؤلفون
    std::string license;                                       // الرخصة
    std::string repository;                                    // رابط المستودع
    std::string homepage;                                      // الصفحة الرئيسية
    std::string documentation;                                 // رابط التوثيق
    std::vector<std::string> keywords;                         // الكلمات المفتاحية
    std::vector<std::string> categories;                       // الفئات
    std::map<std::string, VersionRequirement> dependencies;    // التبعيات
    std::map<std::string, VersionRequirement> dev_dependencies; // تبعيات التطوير
    std::string main_file;                                     // الملف الرئيسي
    std::vector<std::string> source_dirs;                      // مجلدات المصدر
    std::string readme;                                        // ملف README
    bool private_package = false;                              // حزمة خاصة؟
    
    /**
     * @brief تحويل المعلومات إلى نص TOML
     */
    std::string to_toml() const {
        std::ostringstream oss;
        
        oss << "# بسم الله الرحمن الرحيم\n";
        oss << "# ملف إعداد حزمة لغة ص\n\n";
        
        oss << "[حزمة]\n";
        oss << "الاسم = \"" << name << "\"\n";
        if (!name_ar.empty()) {
            oss << "الاسم_العربي = \"" << name_ar << "\"\n";
        }
        oss << "الإصدار = \"" << version.to_string() << "\"\n";
        if (!description.empty()) {
            oss << "الوصف = \"" << description << "\"\n";
        }
        if (!description_ar.empty()) {
            oss << "الوصف_العربي = \"" << description_ar << "\"\n";
        }
        if (!authors.empty()) {
            oss << "المؤلفون = [";
            for (size_t i = 0; i < authors.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << authors[i] << "\"";
            }
            oss << "]\n";
        }
        if (!license.empty()) {
            oss << "الرخصة = \"" << license << "\"\n";
        }
        if (!repository.empty()) {
            oss << "المستودع = \"" << repository << "\"\n";
        }
        if (!main_file.empty()) {
            oss << "الملف_الرئيسي = \"" << main_file << "\"\n";
        }
        
        if (!dependencies.empty()) {
            oss << "\n[تبعيات]\n";
            for (const auto& [name, req] : dependencies) {
                oss << name << " = \"" << req.min_version.to_string() << "\"\n";
            }
        }
        
        if (!dev_dependencies.empty()) {
            oss << "\n[تبعيات_تطوير]\n";
            for (const auto& [name, req] : dev_dependencies) {
                oss << name << " = \"" << req.min_version.to_string() << "\"\n";
            }
        }
        
        return oss.str();
    }
};


// ═══════════════════════════════════════════════════════════════════════════════
// صنف مدير الحزم - Package Manager Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class مدير_الحزم
 * @brief الصنف الرئيسي لإدارة حزم لغة ص
 * 
 * يوفر واجهة موحدة لجميع عمليات إدارة الحزم:
 * - تثبيت وإزالة وتحديث الحزم
 * - حل التبعيات
 * - إدارة التخزين المؤقت
 */
class PackageManager {
public:
    /**
     * @brief المنشئ
     * @param project_dir مجلد المشروع (المجلد الحالي افتراضياً)
     */
    explicit PackageManager(const std::string& project_dir = ".")
        : project_dir_(std::filesystem::absolute(project_dir).string()) {
        
        // تحديد مسارات العمل
        cache_dir_ = get_global_cache_dir();
        deps_dir_ = project_dir_ + "/" + DEPS_DIR;
        
        // تهيئة عميل المستودع
        registry_client_ = std::make_unique<RegistryClient>(DEFAULT_REGISTRY);
        
        // تهيئة محلل التبعيات
        dependency_resolver_ = std::make_unique<DependencyResolver>();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // عمليات المشروع - Project Operations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تهيئة مشروع جديد
     * @param name اسم المشروع
     * @param name_ar الاسم العربي (اختياري)
     * @return true إذا نجحت العملية
     * 
     * ينشئ هيكل المشروع الأساسي:
     * - ص.toml أو sad.toml
     * - مجلد src/
     * - ملف src/main.s
     */
    bool init_project(const std::string& name, const std::string& name_ar = "") {
        log_info("جاري تهيئة مشروع جديد: " + name);
        
        // التحقق من عدم وجود ملف إعداد
        std::string config_path = find_config_file();
        if (!config_path.empty()) {
            log_error("ملف الإعداد موجود بالفعل: " + config_path);
            return false;
        }
        
        // إنشاء معلومات الحزمة
        PackageInfo info;
        info.name = name;
        info.name_ar = name_ar;
        info.version = SemanticVersion::parse("0.1.0");
        info.main_file = "src/main.s";
        info.source_dirs.push_back("src");
        
        // حفظ ملف الإعداد
        std::string config_content = info.to_toml();
        std::ofstream config_file(project_dir_ + "/" + CONFIG_FILE_NAME_AR);
        if (!config_file.is_open()) {
            config_file.open(project_dir_ + "/" + CONFIG_FILE_NAME);
        }
        if (!config_file.is_open()) {
            log_error("فشل إنشاء ملف الإعداد");
            return false;
        }
        config_file << config_content;
        config_file.close();
        
        // إنشاء مجلد المصدر
        std::filesystem::create_directories(project_dir_ + "/src");
        
        // إنشاء الملف الرئيسي
        std::ofstream main_file(project_dir_ + "/src/main.s");
        if (main_file.is_open()) {
            main_file << "// بسم الله الرحمن الرحيم\n";
            main_file << "// " << name << " - نقطة الدخول الرئيسية\n\n";
            main_file << "دالة رئيسية()\n";
            main_file << "    اطبع(\"مرحباً من " << (name_ar.empty() ? name : name_ar) << "!\\n\")\n";
            main_file << "    إرجاع 0\n";
            main_file << "نهاية\n";
            main_file.close();
        }
        
        log_success("تم تهيئة المشروع بنجاح!");
        log_info("أنشئ: " + std::string(CONFIG_FILE_NAME_AR) + " و src/main.s");
        
        return true;
    }
    
    /**
     * @brief تحميل إعدادات المشروع
     * @return true إذا تم التحميل بنجاح
     */
    bool load_project() {
        std::string config_path = find_config_file();
        if (config_path.empty()) {
            log_error("لم يُعثر على ملف الإعداد. استخدم 'ص-حزم init' لإنشاء مشروع جديد.");
            return false;
        }
        
        // تحميل ملف الإعداد
        current_package_ = load_package_info(config_path);
        if (current_package_.name.empty()) {
            log_error("فشل تحميل ملف الإعداد: " + config_path);
            return false;
        }
        
        // تحميل ملف القفل إن وجد
        load_lock_file();
        
        return true;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // عمليات الحزم - Package Operations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تثبيت جميع التبعيات
     * @return true إذا نجح التثبيت
     * 
     * يقرأ ملف الإعداد ويثبت جميع التبعيات المحددة
     */
    bool install_all() {
        if (!load_project()) {
            return false;
        }
        
        log_info("جاري تثبيت التبعيات...");
        
        // حل التبعيات
        auto resolved = resolve_dependencies(current_package_.dependencies);
        if (resolved.empty() && !current_package_.dependencies.empty()) {
            log_error("فشل حل التبعيات");
            return false;
        }
        
        // تثبيت كل حزمة
        for (const auto& [name, version] : resolved) {
            if (!install_package_internal(name, version)) {
                log_warning("فشل تثبيت: " + name);
            }
        }
        
        // حفظ ملف القفل
        save_lock_file(resolved);
        
        log_success("تم تثبيت التبعيات بنجاح!");
        return true;
    }
    
    /**
     * @brief تثبيت حزمة محددة
     * @param package_name اسم الحزمة
     * @param version_req متطلب الإصدار (اختياري، "*" افتراضياً)
     * @param save_to_config حفظ في ملف الإعداد
     * @return true إذا نجح التثبيت
     */
    bool install_package(const std::string& package_name, 
                        const std::string& version_req = "*",
                        bool save_to_config = true) {
        if (!load_project()) {
            return false;
        }
        
        log_info("جاري تثبيت: " + package_name + " (" + version_req + ")");
        
        // البحث عن الإصدار المناسب
        auto req = VersionRequirement::parse(version_req);
        auto available = registry_client_->get_versions(package_name);
        
        SemanticVersion best_version;
        bool found = false;
        
        for (const auto& ver_str : available) {
            auto ver = SemanticVersion::parse(ver_str);
            if (req.satisfies(ver) && (!found || ver > best_version)) {
                best_version = ver;
                found = true;
            }
        }
        
        if (!found) {
            log_error("لم يُعثر على إصدار مناسب للحزمة: " + package_name);
            return false;
        }
        
        // تثبيت الحزمة
        if (!install_package_internal(package_name, best_version)) {
            return false;
        }
        
        // حفظ في ملف الإعداد
        if (save_to_config) {
            current_package_.dependencies[package_name] = req;
            save_config_file();
        }
        
        log_success("تم تثبيت " + package_name + "@" + best_version.to_string());
        return true;
    }
    
    /**
     * @brief إزالة حزمة
     * @param package_name اسم الحزمة
     * @return true إذا نجحت الإزالة
     */
    bool remove_package(const std::string& package_name) {
        if (!load_project()) {
            return false;
        }
        
        log_info("جاري إزالة: " + package_name);
        
        // التحقق من وجود الحزمة
        if (current_package_.dependencies.find(package_name) == current_package_.dependencies.end()) {
            log_error("الحزمة غير موجودة في التبعيات: " + package_name);
            return false;
        }
        
        // إزالة مجلد الحزمة
        std::string pkg_dir = deps_dir_ + "/" + package_name;
        if (std::filesystem::exists(pkg_dir)) {
            std::filesystem::remove_all(pkg_dir);
        }
        
        // إزالة من الإعدادات
        current_package_.dependencies.erase(package_name);
        save_config_file();
        
        // تحديث ملف القفل
        if (locked_versions_.find(package_name) != locked_versions_.end()) {
            locked_versions_.erase(package_name);
            save_lock_file(locked_versions_);
        }
        
        log_success("تم إزالة " + package_name);
        return true;
    }
    
    /**
     * @brief تحديث جميع الحزم
     * @return true إذا نجح التحديث
     */
    bool update_all() {
        if (!load_project()) {
            return false;
        }
        
        log_info("جاري تحديث الحزم...");
        
        // مسح ملف القفل لإعادة حل التبعيات
        locked_versions_.clear();
        
        // إعادة التثبيت
        return install_all();
    }
    
    /**
     * @brief تحديث حزمة محددة
     * @param package_name اسم الحزمة
     * @return true إذا نجح التحديث
     */
    bool update_package(const std::string& package_name) {
        if (!load_project()) {
            return false;
        }
        
        // إزالة الإصدار المقفل
        locked_versions_.erase(package_name);
        
        // إعادة التثبيت
        auto it = current_package_.dependencies.find(package_name);
        if (it == current_package_.dependencies.end()) {
            log_error("الحزمة غير موجودة: " + package_name);
            return false;
        }
        
        return install_package(package_name, it->second.min_version.to_string(), false);
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // عمليات التخزين المؤقت - Cache Operations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تنظيف التخزين المؤقت
     * @return true إذا نجح التنظيف
     */
    bool clean_cache() {
        log_info("جاري تنظيف التخزين المؤقت...");
        
        if (std::filesystem::exists(cache_dir_)) {
            std::filesystem::remove_all(cache_dir_);
            std::filesystem::create_directories(cache_dir_);
        }
        
        log_success("تم تنظيف التخزين المؤقت");
        return true;
    }
    
    /**
     * @brief عرض حجم التخزين المؤقت
     * @return الحجم بالبايت
     */
    size_t get_cache_size() {
        size_t total = 0;
        if (std::filesystem::exists(cache_dir_)) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(cache_dir_)) {
                if (entry.is_regular_file()) {
                    total += entry.file_size();
                }
            }
        }
        return total;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // عمليات البحث والمعلومات - Search and Info Operations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * @brief البحث عن حزم
     * @param query نص البحث
     * @return قائمة النتائج
     */
    std::vector<PackageInfo> search(const std::string& query) {
        log_info("جاري البحث عن: " + query);
        return registry_client_->search(query);
    }
    
    /**
     * @brief الحصول على معلومات حزمة
     * @param package_name اسم الحزمة
     * @return معلومات الحزمة
     */
    std::optional<PackageInfo> get_package_info(const std::string& package_name) {
        return registry_client_->get_package_info(package_name);
    }
    
    /**
     * @brief سرد الحزم المثبتة
     * @return قائمة الحزم المثبتة
     */
    std::vector<std::pair<std::string, SemanticVersion>> list_installed() {
        std::vector<std::pair<std::string, SemanticVersion>> result;
        
        if (!std::filesystem::exists(deps_dir_)) {
            return result;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(deps_dir_)) {
            if (entry.is_directory()) {
                std::string pkg_name = entry.path().filename().string();
                std::string config_path = entry.path().string() + "/" + CONFIG_FILE_NAME;
                
                if (std::filesystem::exists(config_path)) {
                    auto info = load_package_info(config_path);
                    result.push_back({pkg_name, info.version});
                }
            }
        }
        
        return result;
    }

private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة - Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string project_dir_;                                   // مجلد المشروع
    std::string cache_dir_;                                     // مجلد التخزين المؤقت
    std::string deps_dir_;                                      // مجلد التبعيات
    PackageInfo current_package_;                               // معلومات الحزمة الحالية
    std::map<std::string, SemanticVersion> locked_versions_;    // الإصدارات المقفلة
    std::unique_ptr<RegistryClient> registry_client_;           // عميل المستودع
    std::unique_ptr<DependencyResolver> dependency_resolver_;   // محلل التبعيات
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال الخاصة - Private Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * @brief الحصول على مجلد التخزين المؤقت العام
     */
    std::string get_global_cache_dir() {
        #ifdef _WIN32
            const char* home = std::getenv("USERPROFILE");
        #else
            const char* home = std::getenv("HOME");
        #endif
        
        if (home) {
            return std::string(home) + "/.sad/cache";
        }
        return ".sad_cache";
    }
    
    /**
     * @brief البحث عن ملف الإعداد
     */
    std::string find_config_file() {
        // محاولة الملف العربي أولاً
        std::string ar_path = project_dir_ + "/" + CONFIG_FILE_NAME_AR;
        if (std::filesystem::exists(ar_path)) {
            return ar_path;
        }
        
        // ثم الملف الإنجليزي
        std::string en_path = project_dir_ + "/" + CONFIG_FILE_NAME;
        if (std::filesystem::exists(en_path)) {
            return en_path;
        }
        
        return "";
    }
    
    /**
     * @brief تحميل معلومات حزمة من ملف
     */
    PackageInfo load_package_info(const std::string& path) {
        PackageInfo info;
        // TODO: تنفيذ محلل TOML كامل
        // حالياً نستخدم تحليلاً بسيطاً
        
        std::ifstream file(path);
        if (!file.is_open()) return info;
        
        std::string line;
        std::string current_section;
        
        while (std::getline(file, line)) {
            // تخطي السطور الفارغة والتعليقات
            if (line.empty() || line[0] == '#') continue;
            
            // التحقق من بداية قسم
            if (line[0] == '[') {
                size_t end = line.find(']');
                if (end != std::string::npos) {
                    current_section = line.substr(1, end - 1);
                }
                continue;
            }
            
            // تحليل القيم
            size_t eq_pos = line.find('=');
            if (eq_pos != std::string::npos) {
                std::string key = line.substr(0, eq_pos);
                std::string value = line.substr(eq_pos + 1);
                
                // إزالة المسافات والاقتباسات
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t\""));
                value.erase(value.find_last_not_of(" \t\"") + 1);
                
                // معالجة حسب القسم
                if (current_section == "حزمة" || current_section == "package") {
                    if (key == "الاسم" || key == "name") info.name = value;
                    else if (key == "الاسم_العربي") info.name_ar = value;
                    else if (key == "الإصدار" || key == "version") info.version = SemanticVersion::parse(value);
                    else if (key == "الوصف" || key == "description") info.description = value;
                    else if (key == "الرخصة" || key == "license") info.license = value;
                    else if (key == "الملف_الرئيسي" || key == "main") info.main_file = value;
                } else if (current_section == "تبعيات" || current_section == "dependencies") {
                    info.dependencies[key] = VersionRequirement::parse(value);
                } else if (current_section == "تبعيات_تطوير" || current_section == "dev-dependencies") {
                    info.dev_dependencies[key] = VersionRequirement::parse(value);
                }
            }
        }
        
        return info;
    }
    
    /**
     * @brief حفظ ملف الإعداد
     */
    void save_config_file() {
        std::string path = find_config_file();
        if (path.empty()) {
            path = project_dir_ + "/" + CONFIG_FILE_NAME_AR;
        }
        
        std::ofstream file(path);
        if (file.is_open()) {
            file << current_package_.to_toml();
        }
    }
    
    /**
     * @brief تحميل ملف القفل
     */
    void load_lock_file() {
        std::string lock_path = project_dir_ + "/" + LOCK_FILE_NAME_AR;
        if (!std::filesystem::exists(lock_path)) {
            lock_path = project_dir_ + "/" + LOCK_FILE_NAME;
        }
        
        if (!std::filesystem::exists(lock_path)) return;
        
        std::ifstream file(lock_path);
        std::string line;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            size_t eq_pos = line.find('=');
            if (eq_pos != std::string::npos) {
                std::string name = line.substr(0, eq_pos);
                std::string version = line.substr(eq_pos + 1);
                
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                version.erase(0, version.find_first_not_of(" \t\""));
                version.erase(version.find_last_not_of(" \t\"") + 1);
                
                locked_versions_[name] = SemanticVersion::parse(version);
            }
        }
    }
    
    /**
     * @brief حفظ ملف القفل
     */
    void save_lock_file(const std::map<std::string, SemanticVersion>& versions) {
        std::string lock_path = project_dir_ + "/" + LOCK_FILE_NAME_AR;
        
        std::ofstream file(lock_path);
        if (file.is_open()) {
            file << "# ملف القفل - لا تعدله يدوياً\n";
            file << "# تم التوليد بواسطة ص-حزم\n\n";
            
            for (const auto& [name, version] : versions) {
                file << name << " = \"" << version.to_string() << "\"\n";
            }
        }
        
        locked_versions_ = versions;
    }
    
    /**
     * @brief حل التبعيات
     */
    std::map<std::string, SemanticVersion> resolve_dependencies(
        const std::map<std::string, VersionRequirement>& deps) {
        
        return dependency_resolver_->resolve(deps, locked_versions_, *registry_client_);
    }
    
    /**
     * @brief تثبيت حزمة داخلياً
     */
    bool install_package_internal(const std::string& name, const SemanticVersion& version) {
        std::string pkg_dir = deps_dir_ + "/" + name;
        
        // التحقق من وجود الحزمة في التخزين المؤقت
        std::string cache_path = cache_dir_ + "/" + name + "/" + version.to_string();
        
        if (!std::filesystem::exists(cache_path)) {
            // تحميل الحزمة
            if (!registry_client_->download(name, version.to_string(), cache_path)) {
                return false;
            }
        }
        
        // نسخ إلى مجلد التبعيات
        std::filesystem::create_directories(deps_dir_);
        if (std::filesystem::exists(pkg_dir)) {
            std::filesystem::remove_all(pkg_dir);
        }
        
        std::filesystem::copy(cache_path, pkg_dir, 
                             std::filesystem::copy_options::recursive);
        
        return true;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال التسجيل - Logging Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    void log_info(const std::string& msg) {
        std::cout << "\033[34mℹ\033[0m " << msg << std::endl;
    }
    
    void log_success(const std::string& msg) {
        std::cout << "\033[32m✓\033[0m " << msg << std::endl;
    }
    
    void log_warning(const std::string& msg) {
        std::cout << "\033[33m⚠\033[0m " << msg << std::endl;
    }
    
    void log_error(const std::string& msg) {
        std::cerr << "\033[31m✗\033[0m " << msg << std::endl;
    }
};

} // namespace pkg
} // namespace sad
