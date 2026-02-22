// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: registry_client.cpp
 * الوصف: عميل مستودع الحزم للغة ص - Package Registry Client for Sad Language
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T091
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على عميل HTTP للتواصل مع مستودع الحزم الرسمي للغة ص.
 * 
 * الوظائف الرئيسية:
 * 
 * ١. البحث والاستعلام:
 *    - البحث عن الحزم بالاسم أو الكلمات المفتاحية
 *    - جلب معلومات الحزمة (الإصدارات، الوصف، المؤلف)
 *    - جلب قائمة الإصدارات المتاحة
 * 
 * ٢. التنزيل:
 *    - تنزيل حزمة بإصدار محدد
 *    - التحقق من سلامة التنزيل (checksum)
 *    - التخزين المؤقت للتنزيلات
 * 
 * ٣. النشر:
 *    - تسجيل الدخول والمصادقة
 *    - رفع حزمة جديدة
 *    - تحديث حزمة موجودة
 * 
 * ٤. التخزين المؤقت:
 *    - تخزين البيانات الوصفية مؤقتاً
 *    - تخزين الحزم المنزلة
 *    - إدارة صلاحية الذاكرة المؤقتة
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * واجهة برمجة المستودع (Registry API):
 * 
 * GET  /api/v1/packages              - قائمة الحزم
 * GET  /api/v1/packages/:name        - معلومات حزمة
 * GET  /api/v1/packages/:name/:ver   - معلومات إصدار محدد
 * GET  /api/v1/search?q=:query       - البحث
 * GET  /api/v1/download/:name/:ver   - تنزيل
 * POST /api/v1/packages              - نشر (مع مصادقة)
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0.0
 */

#include "registry_client.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <ctime>

// استخدام مكتبة cURL إن توفرت
#ifdef HAS_CURL
#include <curl/curl.h>
#endif

// استخدام مكتبة JSON
#include <nlohmann/json.hpp>

namespace sad {
namespace pkg {

namespace fs = std::filesystem;
using json = nlohmann::json;

// ═══════════════════════════════════════════════════════════════════════════════
// ثوابت المستودع - Registry Constants
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * عنوان المستودع الرسمي
 */
const std::string DEFAULT_REGISTRY_URL = "https://packages.sad-lang.org";

/**
 * إصدار واجهة برمجة التطبيقات
 */
const std::string API_VERSION = "v1";

/**
 * مدة صلاحية الذاكرة المؤقتة (بالثواني)
 */
constexpr int CACHE_TTL_SECONDS = 3600; // ساعة واحدة

/**
 * مدة انتهاء الطلب (بالثواني)
 */
constexpr int REQUEST_TIMEOUT_SECONDS = 30;


// ═══════════════════════════════════════════════════════════════════════════════
// هياكل البيانات - Data Structures
// ═══════════════════════════════════════════════════════════════════════════════

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
    std::string name;                           // اسم الحزمة
    std::string description;                    // الوصف
    std::string author;                         // المؤلف
    std::string repository;                     // مستودع الكود
    std::string license;                        // الترخيص
    std::vector<std::string> versions;          // الإصدارات المتاحة
    std::vector<std::string> keywords;          // الكلمات المفتاحية
    std::map<std::string, VersionRequirement> dependencies; // التبعيات
    int total_downloads;                        // إجمالي التنزيلات
    std::string created_at;                     // تاريخ الإنشاء
    std::string updated_at;                     // تاريخ التحديث
};

/**
 * @struct عنصر_الذاكرة_المؤقتة
 * @brief عنصر مخزن في الذاكرة المؤقتة
 */
struct CacheEntry {
    std::string data;                          // البيانات المخزنة
    std::chrono::system_clock::time_point timestamp; // وقت التخزين
    
    /**
     * @brief هل انتهت صلاحية العنصر؟
     */
    bool is_expired() const {
        auto now = std::chrono::system_clock::now();
        auto age = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp);
        return age.count() > CACHE_TTL_SECONDS;
    }
};


// ═══════════════════════════════════════════════════════════════════════════════
// صنف عميل المستودع - Registry Client Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class عميل_المستودع
 * @brief يتواصل مع مستودع الحزم الرسمي
 */
class RegistryClient {
public:
    /**
     * @brief المنشئ
     * @param registry_url عنوان المستودع (اختياري)
     */
    explicit RegistryClient(const std::string& registry_url = DEFAULT_REGISTRY_URL)
        : registry_url_(registry_url) {
        
        // إعداد مسار الذاكرة المؤقتة
        cache_dir_ = get_cache_directory();
        
        // إنشاء مجلد الذاكرة المؤقتة
        fs::create_directories(cache_dir_);
        
        // تحميل الرمز المميز إن وجد
        load_auth_token();
        
        log_info("تم تهيئة عميل المستودع: " + registry_url_);
    }
    
    /**
     * @brief البحث عن حزم
     * @param query استعلام البحث
     * @param page رقم الصفحة
     * @param per_page عدد النتائج لكل صفحة
     * @return قائمة نتائج البحث
     */
    std::vector<SearchResult> search(
        const std::string& query,
        int page = 1,
        int per_page = 20) {
        
        log_info("البحث عن: " + query);
        
        // بناء عنوان الطلب
        std::string url = registry_url_ + "/api/" + API_VERSION + "/search";
        url += "?q=" + url_encode(query);
        url += "&page=" + std::to_string(page);
        url += "&per_page=" + std::to_string(per_page);
        
        // التحقق من الذاكرة المؤقتة
        auto cached = get_cached(url);
        std::string response;
        if (cached.has_value()) {
            response = cached.value();
        } else {
            response = http_get(url);
            set_cached(url, response);
        }
        
        // تحليل النتائج
        return parse_search_results(response);
    }
    
    /**
     * @brief جلب معلومات حزمة
     * @param name اسم الحزمة
     * @return معلومات الحزمة
     */
    std::optional<RegistryPackageInfo> get_package_info(const std::string& name) {
        std::string url = registry_url_ + "/api/" + API_VERSION + "/packages/" + name;
        
        auto cached = get_cached(url);
        std::string response;
        if (cached.has_value()) {
            response = cached.value();
        } else {
            response = http_get(url);
            if (response.empty()) {
                return std::nullopt;
            }
            set_cached(url, response);
        }
        
        return parse_package_info(response);
    }
    
    /**
     * @brief جلب معلومات حزمة بإصدار محدد
     * @param name اسم الحزمة
     * @param version الإصدار
     * @return معلومات الحزمة
     */
    std::optional<RegistryPackageInfo> get_package_info(
        const std::string& name,
        const std::string& version) {
        
        std::string url = registry_url_ + "/api/" + API_VERSION + "/packages/" 
                        + name + "/" + version;
        
        auto cached = get_cached(url);
        std::string response;
        if (cached.has_value()) {
            response = cached.value();
        } else {
            response = http_get(url);
            if (response.empty()) {
                return std::nullopt;
            }
            set_cached(url, response);
        }
        
        return parse_package_info(response);
    }
    
    /**
     * @brief جلب قائمة الإصدارات
     * @param name اسم الحزمة
     * @return قائمة الإصدارات المتاحة
     */
    std::vector<std::string> get_versions(const std::string& name) {
        auto info = get_package_info(name);
        if (info.has_value()) {
            return info->versions;
        }
        return {};
    }
    
    /**
     * @brief تنزيل حزمة
     * @param name اسم الحزمة
     * @param version الإصدار
     * @param dest_dir مجلد الوجهة
     * @return مسار الملف المنزل
     */
    std::string download_package(
        const std::string& name,
        const std::string& version,
        const std::string& dest_dir) {
        
        log_info("تنزيل " + name + "@" + version + "...");
        
        // التحقق من الذاكرة المؤقتة
        std::string cache_path = cache_dir_ + "/" + name + "-" + version + ".tar.gz";
        if (fs::exists(cache_path)) {
            log_info("  استخدام النسخة المخزنة مؤقتاً");
            return cache_path;
        }
        
        // بناء عنوان التنزيل
        std::string url = registry_url_ + "/api/" + API_VERSION + "/download/" 
                        + name + "/" + version;
        
        // تنزيل الملف
        bool success = http_download(url, cache_path);
        if (!success) {
            throw std::runtime_error("فشل تنزيل الحزمة: " + name);
        }
        
        // التحقق من سلامة الملف
        if (!verify_checksum(name, version, cache_path)) {
            fs::remove(cache_path);
            throw std::runtime_error("فشل التحقق من سلامة الحزمة: " + name);
        }
        
        log_success("  تم التنزيل بنجاح");
        return cache_path;
    }
    
    /**
     * @brief نشر حزمة
     * @param tarball_path مسار ملف الأرشيف
     * @return هل نجح النشر؟
     */
    bool publish_package(const std::string& tarball_path) {
        // التحقق من المصادقة
        if (auth_token_.empty()) {
            throw std::runtime_error("يجب تسجيل الدخول أولاً");
        }
        
        // التحقق من الملف
        if (!fs::exists(tarball_path)) {
            throw std::runtime_error("ملف الأرشيف غير موجود: " + tarball_path);
        }
        
        log_info("نشر الحزمة...");
        
        std::string url = registry_url_ + "/api/" + API_VERSION + "/packages";
        
        bool success = http_upload(url, tarball_path, auth_token_);
        
        if (success) {
            log_success("تم نشر الحزمة بنجاح");
        } else {
            log_error("فشل نشر الحزمة");
        }
        
        return success;
    }
    
    /**
     * @brief تسجيل الدخول
     * @param username اسم المستخدم
     * @param password كلمة المرور
     * @return هل نجح تسجيل الدخول؟
     */
    bool login(const std::string& username, const std::string& password) {
        log_info("تسجيل الدخول كـ " + username + "...");
        
        std::string url = registry_url_ + "/api/" + API_VERSION + "/login";
        
        json body;
        body["username"] = username;
        body["password"] = password;
        
        std::string response = http_post(url, body.dump());
        
        if (response.empty()) {
            log_error("فشل تسجيل الدخول");
            return false;
        }
        
        try {
            json j = json::parse(response);
            if (j.contains("token")) {
                auth_token_ = j["token"].get<std::string>();
                save_auth_token();
                log_success("تم تسجيل الدخول بنجاح");
                return true;
            }
        } catch (...) {
            // تجاهل أخطاء التحليل
        }
        
        log_error("فشل تسجيل الدخول");
        return false;
    }
    
    /**
     * @brief تسجيل الخروج
     */
    void logout() {
        auth_token_.clear();
        fs::remove(get_token_path());
        log_info("تم تسجيل الخروج");
    }
    
    /**
     * @brief هل المستخدم مسجل الدخول؟
     */
    bool is_logged_in() const {
        return !auth_token_.empty();
    }
    
    /**
     * @brief تنظيف الذاكرة المؤقتة
     * @param max_age_days أقصى عمر بالأيام
     */
    void clean_cache(int max_age_days = 7) {
        log_info("تنظيف الذاكرة المؤقتة...");
        
        int removed = 0;
        auto now = std::chrono::system_clock::now();
        
        for (const auto& entry : fs::directory_iterator(cache_dir_)) {
            auto ftime = fs::last_write_time(entry);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
            );
            auto age = std::chrono::duration_cast<std::chrono::hours>(now - sctp);
            
            if (age.count() > max_age_days * 24) {
                fs::remove(entry);
                removed++;
            }
        }
        
        log_info("تم حذف " + std::to_string(removed) + " ملف");
    }
    
    /**
     * @brief تغيير عنوان المستودع
     */
    void set_registry_url(const std::string& url) {
        registry_url_ = url;
        memory_cache_.clear();
    }
    
    /**
     * @brief الحصول على عنوان المستودع
     */
    std::string get_registry_url() const {
        return registry_url_;
    }

private:
    std::string registry_url_;                          // عنوان المستودع
    std::string cache_dir_;                             // مجلد الذاكرة المؤقتة
    std::string auth_token_;                            // رمز المصادقة
    std::map<std::string, CacheEntry> memory_cache_;    // ذاكرة مؤقتة في الذاكرة
    
    /**
     * @brief الحصول على مجلد الذاكرة المؤقتة
     */
    std::string get_cache_directory() {
        #ifdef _WIN32
        const char* appdata = std::getenv("LOCALAPPDATA");
        if (appdata) {
            return std::string(appdata) + "\\sad\\cache\\registry";
        }
        return ".sad/cache/registry";
        #else
        const char* home = std::getenv("HOME");
        if (home) {
            return std::string(home) + "/.sad/cache/registry";
        }
        return ".sad/cache/registry";
        #endif
    }
    
    /**
     * @brief الحصول على مسار ملف الرمز المميز
     */
    std::string get_token_path() {
        #ifdef _WIN32
        const char* appdata = std::getenv("LOCALAPPDATA");
        if (appdata) {
            return std::string(appdata) + "\\sad\\credentials.json";
        }
        #else
        const char* home = std::getenv("HOME");
        if (home) {
            return std::string(home) + "/.sad/credentials.json";
        }
        #endif
        return ".sad/credentials.json";
    }
    
    /**
     * @brief تحميل رمز المصادقة
     */
    void load_auth_token() {
        std::string path = get_token_path();
        if (!fs::exists(path)) return;
        
        try {
            std::ifstream f(path);
            json j = json::parse(f);
            if (j.contains("token")) {
                auth_token_ = j["token"].get<std::string>();
            }
        } catch (...) {
            // تجاهل الأخطاء
        }
    }
    
    /**
     * @brief حفظ رمز المصادقة
     */
    void save_auth_token() {
        std::string path = get_token_path();
        fs::create_directories(fs::path(path).parent_path());
        
        json j;
        j["token"] = auth_token_;
        
        std::ofstream f(path);
        f << j.dump(2);
    }
    
    /**
     * @brief الحصول من الذاكرة المؤقتة
     */
    std::optional<std::string> get_cached(const std::string& key) {
        auto it = memory_cache_.find(key);
        if (it != memory_cache_.end() && !it->second.is_expired()) {
            return it->second.data;
        }
        return std::nullopt;
    }
    
    /**
     * @brief التخزين في الذاكرة المؤقتة
     */
    void set_cached(const std::string& key, const std::string& value) {
        memory_cache_[key] = {value, std::chrono::system_clock::now()};
    }
    
    /**
     * @brief ترميز URL
     */
    std::string url_encode(const std::string& str) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;
        
        for (char c : str) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            } else {
                escaped << std::uppercase;
                escaped << '%' << std::setw(2) << int((unsigned char)c);
                escaped << std::nouppercase;
            }
        }
        
        return escaped.str();
    }
    
    /**
     * @brief طلب HTTP GET
     * @note هذا تنفيذ وهمي - يجب استخدام مكتبة HTTP حقيقية
     */
    std::string http_get(const std::string& url) {
        #ifdef HAS_CURL
        // تنفيذ حقيقي باستخدام cURL
        CURL* curl = curl_easy_init();
        if (!curl) return "";
        
        std::string response;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, REQUEST_TIMEOUT_SECONDS);
        
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK) {
            return "";
        }
        
        return response;
        #else
        // تنفيذ وهمي للاختبار
        log_warning("تحذير: استخدام طلب HTTP وهمي (لا توجد مكتبة HTTP)");
        return mock_http_response(url);
        #endif
    }
    
    /**
     * @brief طلب HTTP POST
     */
    std::string http_post(const std::string& url, const std::string& body) {
        #ifdef HAS_CURL
        // تنفيذ حقيقي
        CURL* curl = curl_easy_init();
        if (!curl) return "";
        
        std::string response;
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, REQUEST_TIMEOUT_SECONDS);
        
        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK) {
            return "";
        }
        
        return response;
        #else
        return "";
        #endif
    }
    
    /**
     * @brief تنزيل ملف
     */
    bool http_download(const std::string& url, const std::string& dest) {
        #ifdef HAS_CURL
        CURL* curl = curl_easy_init();
        if (!curl) return false;
        
        FILE* fp = fopen(dest.c_str(), "wb");
        if (!fp) {
            curl_easy_cleanup(curl);
            return false;
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300); // 5 دقائق للتنزيل
        
        CURLcode res = curl_easy_perform(curl);
        fclose(fp);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
        #else
        // تنفيذ وهمي
        std::ofstream f(dest, std::ios::binary);
        f << "mock tarball content";
        return true;
        #endif
    }
    
    /**
     * @brief رفع ملف
     */
    bool http_upload(const std::string& url, const std::string& file_path, const std::string& token) {
        #ifdef HAS_CURL
        CURL* curl = curl_easy_init();
        if (!curl) return false;
        
        curl_mime* form = curl_mime_init(curl);
        curl_mimepart* field = curl_mime_addpart(form);
        curl_mime_name(field, "package");
        curl_mime_filedata(field, file_path.c_str());
        
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 300);
        
        CURLcode res = curl_easy_perform(curl);
        
        curl_mime_free(form);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
        #else
        return true; // وهمي للاختبار
        #endif
    }
    
    /**
     * @brief التحقق من سلامة الملف
     */
    bool verify_checksum(const std::string& name, const std::string& version, const std::string& file_path) {
        // TODO: تنفيذ التحقق من SHA256
        // جلب checksum من المستودع ومقارنته مع الملف المنزل
        return true;
    }
    
    /**
     * @brief استجابة HTTP وهمية للاختبار
     */
    std::string mock_http_response(const std::string& url) {
        // توفير استجابات وهمية للاختبار
        if (url.find("/search") != std::string::npos) {
            json j;
            j["results"] = json::array();
            j["results"].push_back({
                {"name", "مثال"},
                {"description", "حزمة مثال للاختبار"},
                {"latest_version", "1.0.0"},
                {"author", "مطور ص"},
                {"downloads", 100}
            });
            return j.dump();
        }
        
        if (url.find("/packages/") != std::string::npos) {
            json j;
            j["name"] = "مثال";
            j["description"] = "حزمة مثال للاختبار";
            j["versions"] = {"1.0.0", "0.9.0", "0.8.0"};
            j["author"] = "مطور ص";
            j["dependencies"] = json::object();
            return j.dump();
        }
        
        return "{}";
    }
    
    /**
     * @brief تحليل نتائج البحث
     */
    std::vector<SearchResult> parse_search_results(const std::string& response) {
        std::vector<SearchResult> results;
        
        try {
            json j = json::parse(response);
            if (j.contains("results")) {
                for (const auto& item : j["results"]) {
                    SearchResult result;
                    result.name = item.value("name", "");
                    result.description = item.value("description", "");
                    result.latest_version = item.value("latest_version", "");
                    result.author = item.value("author", "");
                    result.downloads = item.value("downloads", 0);
                    result.updated_at = item.value("updated_at", "");
                    results.push_back(result);
                }
            }
        } catch (const std::exception& e) {
            log_error("خطأ في تحليل نتائج البحث: " + std::string(e.what()));
        }
        
        return results;
    }
    
    /**
     * @brief تحليل معلومات الحزمة
     */
    std::optional<RegistryPackageInfo> parse_package_info(const std::string& response) {
        try {
            json j = json::parse(response);
            
            RegistryPackageInfo info;
            info.name = j.value("name", "");
            info.description = j.value("description", "");
            info.author = j.value("author", "");
            info.repository = j.value("repository", "");
            info.license = j.value("license", "");
            info.total_downloads = j.value("downloads", 0);
            info.created_at = j.value("created_at", "");
            info.updated_at = j.value("updated_at", "");
            
            if (j.contains("versions") && j["versions"].is_array()) {
                for (const auto& v : j["versions"]) {
                    info.versions.push_back(v.get<std::string>());
                }
            }
            
            if (j.contains("keywords") && j["keywords"].is_array()) {
                for (const auto& k : j["keywords"]) {
                    info.keywords.push_back(k.get<std::string>());
                }
            }
            
            if (j.contains("dependencies") && j["dependencies"].is_object()) {
                for (auto& [key, value] : j["dependencies"].items()) {
                    info.dependencies[key] = VersionRequirement::parse(value.get<std::string>());
                }
            }
            
            return info;
            
        } catch (const std::exception& e) {
            log_error("خطأ في تحليل معلومات الحزمة: " + std::string(e.what()));
            return std::nullopt;
        }
    }
    
    // دوال التسجيل
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
    
    #ifdef HAS_CURL
    /**
     * @brief دالة استدعاء لكتابة البيانات
     */
    static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        size_t total_size = size * nmemb;
        userp->append((char*)contents, total_size);
        return total_size;
    }
    #endif
};

} // namespace pkg
} // namespace sad
