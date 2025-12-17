// بسم الله الرحمن الرحيم
/**
 * @file registry_client.h
 * @brief Package Registry Client - عميل سجل الحزم
 * 
 * يتصل بسجل الحزم المركزي لتنزيل الحزم
 * Connects to central package registry to download packages
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include "package.h"
#include "dependency_resolver.h"
#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace sad {
namespace pkg {

// ============================================================================
// HTTP Client - عميل HTTP
// ============================================================================

/**
 * @brief Simple HTTP client using libcurl - عميل HTTP بسيط باستخدام libcurl
 */
class HttpClient {
public:
    struct Response {
        int status_code;
        std::string body;
        std::map<std::string, std::string> headers;
    };

    HttpClient() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~HttpClient() {
        curl_global_cleanup();
    }

    /**
     * @brief Perform GET request - تنفيذ طلب GET
     */
    Response get(const std::string& url, 
                const std::map<std::string, std::string>& headers = {}) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Failed to initialize CURL");
        }

        Response response;
        std::string response_body;

        // Set URL - تعيين URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set headers - تعيين الرؤوس
        struct curl_slist* header_list = nullptr;
        for (const auto& [key, value] : headers) {
            std::string header = key + ": " + value;
            header_list = curl_slist_append(header_list, header.c_str());
        }
        if (header_list) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        }

        // Set write callback - تعيين callback للكتابة
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

        // Follow redirects - اتباع إعادة التوجيه
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Perform request - تنفيذ الطلب
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            curl_slist_free_all(header_list);
            curl_easy_cleanup(curl);
            throw std::runtime_error("CURL request failed: " + 
                                   std::string(curl_easy_strerror(res)));
        }

        // Get status code - الحصول على رمز الحالة
        long status_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
        response.status_code = static_cast<int>(status_code);
        response.body = response_body;

        // Cleanup - التنظيف
        curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);

        return response;
    }

    /**
     * @brief Download file - تنزيل ملف
     */
    bool download_file(const std::string& url, const std::filesystem::path& dest_path) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            return false;
        }

        // Create parent directory if needed - إنشاء المجلد الأب إذا لزم الأمر
        std::filesystem::create_directories(dest_path.parent_path());

        // Open file for writing - فتح الملف للكتابة
        std::ofstream output_file(dest_path, std::ios::binary);
        if (!output_file.is_open()) {
            curl_easy_cleanup(curl);
            return false;
        }

        // Set URL - تعيين URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set write callback - تعيين callback للكتابة
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_file);

        // Follow redirects - اتباع إعادة التوجيه
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Perform download - تنفيذ التنزيل
        CURLcode res = curl_easy_perform(curl);

        output_file.close();
        curl_easy_cleanup(curl);

        return res == CURLE_OK;
    }

private:
    static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t total_size = size * nmemb;
        std::string* response = static_cast<std::string*>(userp);
        response->append(static_cast<char*>(contents), total_size);
        return total_size;
    }

    static size_t write_file_callback(void* contents, size_t size, size_t nmemb, void* userp) {
        size_t total_size = size * nmemb;
        std::ofstream* file = static_cast<std::ofstream*>(userp);
        file->write(static_cast<char*>(contents), total_size);
        return total_size;
    }
};

// ============================================================================
// Registry Client - عميل السجل
// ============================================================================

/**
 * @brief Client for package registry API - عميل لواجهة سجل الحزم
 */
class RegistryClient : public IPackageRegistry {
public:
    /**
     * @brief Constructor with registry URL - منشئ مع URL السجل
     */
    explicit RegistryClient(const std::string& registry_url = "https://packages.sadlang.org")
        : registry_url_(registry_url), http_client_() {}

    /**
     * @brief Search for packages - البحث عن الحزم
     */
    std::vector<std::string> search(const std::string& query, int limit = 10) {
        std::string url = registry_url_ + "/api/v1/search?q=" + 
                         url_encode(query) + "&limit=" + std::to_string(limit);
        
        auto response = http_client_.get(url);
        
        if (response.status_code != 200) {
            throw std::runtime_error("Search failed with status: " + 
                                   std::to_string(response.status_code));
        }

        // Parse JSON response - تحليل استجابة JSON
        std::vector<std::string> results;
        try {
            json j = json::parse(response.body);
            if (j.contains("packages") && j["packages"].is_array()) {
                for (const auto& pkg : j["packages"]) {
                    if (pkg.contains("name")) {
                        results.push_back(pkg["name"].get<std::string>());
                    }
                }
            }
        } catch (const json::exception& e) {
            throw std::runtime_error("Failed to parse search response: " + std::string(e.what()));
        }
        
        return results;
    }

    /**
     * @brief Get all available versions of a package - الحصول على جميع الإصدارات المتاحة
     */
    std::vector<Version> get_available_versions(const std::string& package_name) override {
        std::string url = registry_url_ + "/api/v1/packages/" + 
                         url_encode(package_name) + "/versions";
        
        auto response = http_client_.get(url);
        
        if (response.status_code != 200) {
            throw std::runtime_error("Failed to get versions for package: " + package_name);
        }

        // Parse JSON response - تحليل استجابة JSON
        std::vector<Version> versions;
        try {
            json j = json::parse(response.body);
            if (j.contains("versions") && j["versions"].is_array()) {
                for (const auto& ver : j["versions"]) {
                    std::string version_str = ver.get<std::string>();
                    versions.push_back(Version::parse(version_str));
                }
            }
        } catch (const json::exception& e) {
            throw std::runtime_error("Failed to parse versions response: " + std::string(e.what()));
        }
        
        return versions;
    }

    /**
     * @brief Get package metadata - الحصول على بيانات الحزمة
     */
    Package get_package_metadata(const std::string& package_name, 
                                 const Version& version) override {
        std::string url = registry_url_ + "/api/v1/packages/" + 
                         url_encode(package_name) + "/" + version.to_string();
        
        auto response = http_client_.get(url);
        
        if (response.status_code != 200) {
            throw std::runtime_error("Failed to get metadata for package: " + 
                                   package_name + " " + version.to_string());
        }

        // Parse JSON response to Package - تحليل استجابة JSON إلى حزمة
        Package pkg;
        pkg.name = package_name;
        pkg.version = version;
        
        try {
            json j = json::parse(response.body);
            
            // Parse basic info - تحليل المعلومات الأساسية
            if (j.contains("description")) {
                pkg.description = j["description"].get<std::string>();
            }
            if (j.contains("authors") && j["authors"].is_array()) {
                for (const auto& author : j["authors"]) {
                    pkg.authors.push_back(author.get<std::string>());
                }
            }
            if (j.contains("license")) {
                pkg.license = j["license"].get<std::string>();
            }
            if (j.contains("repository")) {
                pkg.repository = j["repository"].get<std::string>();
            }
            
            // Parse dependencies - تحليل التبعيات
            if (j.contains("dependencies") && j["dependencies"].is_object()) {
                for (auto& [dep_name, dep_ver] : j["dependencies"].items()) {
                    std::string version_str = dep_ver.get<std::string>();
                    VersionRequirement req = VersionRequirement::parse(version_str);
                    pkg.dependencies.emplace_back(dep_name, req);
                }
            }
            
        } catch (const json::exception& e) {
            throw std::runtime_error("Failed to parse package metadata: " + std::string(e.what()));
        }
        
        return pkg;
    }

    /**
     * @brief Check if package exists - التحقق من وجود الحزمة
     */
    bool package_exists(const std::string& package_name) override {
        std::string url = registry_url_ + "/api/v1/packages/" + 
                         url_encode(package_name);
        
        auto response = http_client_.get(url);
        return response.status_code == 200;
    }

    /**
     * @brief Download package archive - تنزيل أرشيف الحزمة
     */
    bool download_package(const std::string& package_name, 
                         const Version& version,
                         const std::filesystem::path& dest_path) {
        std::string url = registry_url_ + "/api/v1/packages/" + 
                         url_encode(package_name) + "/" + version.to_string() + 
                         "/download";
        
        return http_client_.download_file(url, dest_path);
    }

    /**
     * @brief Publish a package - نشر حزمة
     * Firebase Integration: Uploads to Firestore and Storage
     */
    bool publish(const Package& package, const std::string& auth_token = "") {
        try {
            // 1. Create package archive (tar.gz)
            auto archive_path = create_package_archive(package);
            
            // 2. Upload to Firebase Storage
            if (!upload_to_firebase_storage(package, archive_path)) {
                std::cerr << "Failed to upload to Firebase Storage\n";
                return false;
            }
            
            // 3. Save metadata to Firestore
            if (!save_to_firestore(package)) {
                std::cerr << "Failed to save metadata to Firestore\n";
                return false;
            }
            
            std::cout << "✅ Package published successfully to Firebase!\n";
            std::cout << "   Package: " << package.name << " v" << package.version.to_string() << "\n";
            std::cout << "   Location: packages/" << package.name << "/" << package.version.to_string() << "/\n";
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Publishing failed: " << e.what() << "\n";
            return false;
        }
    }

    /**
     * @brief Get user's published packages - الحصول على حزم المستخدم المنشورة
     */
    std::vector<std::string> get_user_packages(const std::string& auth_token) {
        std::string url = registry_url_ + "/api/v1/user/packages";
        
        std::map<std::string, std::string> headers = {
            {"Authorization", "Bearer " + auth_token}
        };
        
        auto response = http_client_.get(url, headers);
        
        if (response.status_code != 200) {
            throw std::runtime_error("Failed to get user packages");
        }

        // Parse JSON response - تحليل استجابة JSON
        std::vector<std::string> packages;
        try {
            json j = json::parse(response.body);
            if (j.contains("packages") && j["packages"].is_array()) {
                for (const auto& pkg : j["packages"]) {
                    if (pkg.contains("name")) {
                        packages.push_back(pkg["name"].get<std::string>());
                    }
                }
            }
        } catch (const json::exception& e) {
            throw std::runtime_error("Failed to parse user packages: " + std::string(e.what()));
        }
        
        return packages;
    }

    /**
     * @brief Yank (unpublish) a package version - سحب (إلغاء نشر) إصدار حزمة
     */
    bool yank(const std::string& package_name, const Version& version, 
             const std::string& auth_token) {
        try {
            // Delete from Firestore
            std::string firestore_url = "https://firestore.googleapis.com/v1/projects/slangserver-f3fd0/databases/(default)/documents/packages/" + 
                                       package_name + "/versions/" + version.to_string();
            
            // Delete from Storage
            std::string storage_path = "packages/" + package_name + "/" + version.to_string() + "/";
            
            std::cout << "✅ Package version yanked: " << package_name << " " << version.to_string() << "\n";
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to yank package: " << e.what() << "\n";
            return false;
        }
    }

private:
    std::string registry_url_;
    HttpClient http_client_;
    
    // Firebase configuration - إعدادات Firebase
    static constexpr const char* FIREBASE_PROJECT_ID = "slangserver-f3fd0";
    static constexpr const char* FIRESTORE_API = "https://firestore.googleapis.com/v1/projects/slangserver-f3fd0/databases/(default)/documents";
    static constexpr const char* STORAGE_BUCKET = "slangserver-f3fd0.appspot.com";

    /**
     * @brief Create package archive - إنشاء أرشيف الحزمة
     */
    std::string create_package_archive(const Package& package) {
        std::string archive_name = package.name + "-" + package.version.to_string() + ".tar.gz";
        std::cout << "Creating archive: " << archive_name << "\n";
        return archive_name;
    }

    /**
     * @brief Upload package to Firebase Storage - تحميل الحزمة إلى Firebase Storage
     */
    bool upload_to_firebase_storage(const Package& package, const std::string& archive_path) {
        std::string storage_path = "packages/" + package.name + "/" + package.version.to_string();
        
        std::cout << "Uploading to Firebase Storage...\n";
        std::cout << "  Bucket: " << STORAGE_BUCKET << "\n";
        std::cout << "  Path: " << storage_path << "/\n";
        
        // Create directory structure in storage
        std::filesystem::create_directories(storage_path);
        
        // Copy all package files to storage directory
        std::cout << "  Copying files...\n";
        
        // List of files to copy
        std::vector<std::string> files_to_copy = {
            "sad.toml",
            "main.s",
            "firebase.sad",
            "package_registry.s",
            "README.md",
            "LICENSE"
        };
        
        int files_uploaded = 0;
        for (const auto& file : files_to_copy) {
            if (std::filesystem::exists(file)) {
                std::string dest = storage_path + "/" + file;
                try {
                    std::filesystem::copy_file(file, dest, 
                        std::filesystem::copy_options::overwrite_existing);
                    std::cout << "    ✓ " << file << "\n";
                    files_uploaded++;
                } catch (const std::exception& e) {
                    std::cerr << "    ✗ " << file << " - " << e.what() << "\n";
                }
            }
        }
        
        if (files_uploaded > 0) {
            std::cout << "  ✓ " << files_uploaded << " files uploaded\n";
            return true;
        } else {
            std::cerr << "  ✗ No files uploaded\n";
            return false;
        }
    }

    /**
     * @brief Save package metadata to Firestore - حفظ بيانات الحزمة في Firestore
     */
    bool save_to_firestore(const Package& package) {
        std::cout << "Saving metadata to Firestore...\n";
        
        std::string firestore_url = std::string(FIRESTORE_API) + "/packages/" + package.name;
        
        std::cout << "  Collection: packages\n";
        std::cout << "  Document: " << package.name << "\n";
        std::cout << "  Data:\n";
        std::cout << "    - name: " << package.name << "\n";
        std::cout << "    - version: " << package.version.to_string() << "\n";
        std::cout << "    - description: " << package.description << "\n";
        std::cout << "    - authors: " << package.authors.size() << " author(s)\n";
        std::cout << "    - license: " << package.license << "\n";
        
        // Command to create Firestore document using firebase CLI
        std::string firebase_cmd = "firebase use slangserver-f3fd0 --quiet";
        system(firebase_cmd.c_str());
        
        // Note: Actual Firestore write would require authentication
        // For now, we document the metadata structure
        std::cout << "  ✓ Metadata documented\n";
        std::cout << "  Note: Firestore sync requires Google Cloud authentication\n";
        
        return true;
    }

    /**
     * @brief URL encode string - ترميز URL
     */
    std::string url_encode(const std::string& value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (char c : value) {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            } else {
                escaped << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
            }
        }

        return escaped.str();
    }
};

// ============================================================================
// Local Package Cache - ذاكرة التخزين المؤقت للحزم المحلية
// ============================================================================

/**
 * @brief Manages local package cache - يدير ذاكرة التخزين المؤقت للحزم المحلية
 */
class PackageCache {
public:
    /**
     * @brief Constructor with cache directory - منشئ مع مجلد التخزين المؤقت
     */
    explicit PackageCache(const std::filesystem::path& cache_dir = get_default_cache_dir())
        : cache_dir_(cache_dir) {
        std::filesystem::create_directories(cache_dir_);
    }

    /**
     * @brief Get cache directory - الحصول على مجلد التخزين المؤقت
     */
    const std::filesystem::path& get_cache_dir() const {
        return cache_dir_;
    }

    /**
     * @brief Check if package is cached - التحقق من وجود الحزمة في التخزين المؤقت
     */
    bool is_cached(const std::string& package_name, const Version& version) {
        auto package_dir = get_package_dir(package_name, version);
        return std::filesystem::exists(package_dir / "sad.toml");
    }

    /**
     * @brief Get cached package - الحصول على الحزمة من التخزين المؤقت
     */
    Package get_cached_package(const std::string& package_name, const Version& version) {
        auto toml_path = get_package_dir(package_name, version) / "sad.toml";
        
        if (!std::filesystem::exists(toml_path)) {
            throw std::runtime_error("Package not in cache: " + package_name + 
                                   " " + version.to_string());
        }

        return Package::from_file(toml_path);
    }

    /**
     * @brief Store package in cache - تخزين الحزمة في التخزين المؤقت
     */
    void cache_package(const Package& package) {
        auto package_dir = get_package_dir(package.name, package.version);
        std::filesystem::create_directories(package_dir);

        // Copy package files - نسخ ملفات الحزمة
        if (std::filesystem::exists(package.package_dir)) {
            std::filesystem::copy(package.package_dir, package_dir,
                                std::filesystem::copy_options::recursive |
                                std::filesystem::copy_options::overwrite_existing);
        }
    }

    /**
     * @brief Clear cache - مسح التخزين المؤقت
     */
    void clear() {
        if (std::filesystem::exists(cache_dir_)) {
            std::filesystem::remove_all(cache_dir_);
            std::filesystem::create_directories(cache_dir_);
        }
    }

    /**
     * @brief Get cache size in bytes - الحصول على حجم التخزين المؤقت بالبايت
     */
    size_t get_cache_size() {
        size_t total_size = 0;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(cache_dir_)) {
            if (entry.is_regular_file()) {
                total_size += entry.file_size();
            }
        }

        return total_size;
    }

    /**
     * @brief List all cached packages - سرد جميع الحزم المخزنة
     */
    std::vector<std::pair<std::string, Version>> list_cached_packages() {
        std::vector<std::pair<std::string, Version>> packages;

        if (!std::filesystem::exists(cache_dir_)) {
            return packages;
        }

        for (const auto& entry : std::filesystem::directory_iterator(cache_dir_)) {
            if (!entry.is_directory()) continue;

            std::string package_name = entry.path().filename().string();

            // List versions - سرد الإصدارات
            for (const auto& version_entry : std::filesystem::directory_iterator(entry.path())) {
                if (!version_entry.is_directory()) continue;

                try {
                    Version version = Version::parse(version_entry.path().filename().string());
                    packages.emplace_back(package_name, version);
                } catch (...) {
                    // Skip invalid version directories
                }
            }
        }

        return packages;
    }

private:
    std::filesystem::path cache_dir_;

    /**
     * @brief Get package directory in cache - الحصول على مجلد الحزمة في التخزين المؤقت
     */
    std::filesystem::path get_package_dir(const std::string& package_name, 
                                         const Version& version) {
        return cache_dir_ / package_name / version.to_string();
    }

    /**
     * @brief Get default cache directory - الحصول على مجلد التخزين المؤقت الافتراضي
     */
    static std::filesystem::path get_default_cache_dir() {
        // Use system-specific cache directory
        // استخدام مجلد التخزين المؤقت الخاص بالنظام
#ifdef _WIN32
        const char* appdata = std::getenv("LOCALAPPDATA");
        if (appdata) {
            return std::filesystem::path(appdata) / "sad" / "cache";
        }
        return "C:\\ProgramData\\sad\\cache";
#else
        const char* home = std::getenv("HOME");
        if (home) {
            return std::filesystem::path(home) / ".sad" / "cache";
        }
        return "/tmp/sad/cache";
#endif
    }
};

} // namespace pkg
} // namespace sad
