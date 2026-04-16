/**
 * @file registry_client_v2.h
 * @brief عميل السجل المركزي v2 — بدون تبعيات خارجية
 * @brief Registry Client v2 — zero external dependencies
 * 
 * يستخدم http_client.h المحلي للتواصل مع خادم السجل.
 * Uses local http_client.h for registry server communication.
 * 
 * @author فريق لغة ص
 * @date مارس 2026
 */

#pragma once

#include "http_client.h"
#include "package.h"
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

namespace sad {
namespace pkg {

/**
 * @brief عميل السجل المركزي للحزم (الإصدار ٢)
 * @brief Central package registry client (version 2)
 * 
 * يوفر وظائف:
 * - البحث عن حزم
 * - الحصول على الإصدارات المتاحة
 * - تنزيل وتثبيت حزمة
 */
class RegistryClientV2 {
public:
    /**
     * @brief إنشاء عميل جديد
     * @param base_url عنوان السجل (مثل: http://185.47.174.39:3000)
     */
    explicit RegistryClientV2(const std::string& base_url)
        : base_url_(base_url) {
        // (AR) إزالة / من نهاية العنوان إذا وُجد
        // (EN) Remove trailing slash if present
        if (!base_url_.empty() && base_url_.back() == '/') {
            base_url_.pop_back();
        }
    }

    /**
     * @brief البحث عن حزم في السجل
     * @param query نص البحث
     * @return قائمة أسماء الحزم المطابقة
     */
    std::vector<std::string> search(const std::string& query) {
        std::vector<std::string> results;
        try {
            HttpClient http;
            std::string url = base_url_ + "/api/packages/search?q=" + urlEncode(query);
            auto response = http.get(url);
            
            if (response.status_code == 200) {
                results = parseJsonStringArray(response.body, "name");
            }
        } catch (const std::exception& e) {
            std::cerr << "(AR) خطأ في البحث: " << e.what()
                      << " / (EN) Search error: " << e.what() << std::endl;
        }
        return results;
    }

    /**
     * @brief الحصول على الإصدارات المتاحة لحزمة
     * @param package_name اسم الحزمة
     * @return قائمة الإصدارات (مرتبة من الأحدث)
     */
    std::vector<Version> get_available_versions(const std::string& package_name) {
        std::vector<Version> versions;
        try {
            HttpClient http;
            std::string url = base_url_ + "/api/packages/" + urlEncode(package_name) + "/versions";
            auto response = http.get(url);
            
            if (response.status_code == 200) {
                auto versionStrs = parseJsonStringArray(response.body, "version");
                for (const auto& vs : versionStrs) {
                    try {
                        versions.push_back(Version::parse(vs));
                    } catch (...) {}
                }
                // (AR) ترتيب تنازلي (الأحدث أولاً)
                // (EN) Sort descending (newest first)
                std::sort(versions.begin(), versions.end(), std::greater<Version>());
            }
        } catch (const std::exception& e) {
            std::cerr << "(AR) خطأ في جلب الإصدارات: " << e.what()
                      << " / (EN) Version fetch error: " << e.what() << std::endl;
        }
        return versions;
    }

    /**
     * @brief تنزيل وتثبيت حزمة
     * @param name اسم الحزمة
     * @param ver الإصدار المطلوب
     * @param dest_dir مجلد الوجهة
     * @return true إذا نجحت العملية
     */
    bool download_and_install(const std::string& name, const Version& ver,
                              const std::filesystem::path& dest_dir) {
        try {
            HttpClient http;
            std::string url = base_url_ + "/api/packages/" + urlEncode(name) 
                            + "/download/" + ver.to_string();
            auto response = http.get(url);
            
            if (response.status_code != 200) {
                std::cerr << "(AR) فشل التنزيل: HTTP " << response.status_code
                          << " / (EN) Download failed: HTTP " << response.status_code << std::endl;
                return false;
            }
            
            // (AR) إنشاء مجلد الحزمة
            // (EN) Create package directory
            std::filesystem::path pkg_dir = dest_dir / name / ver.to_string();
            std::filesystem::create_directories(pkg_dir);
            
            // (AR) حفظ أرشيف الحزمة
            // (EN) Save package archive
            std::filesystem::path archive_path = pkg_dir / (name + ".tar.gz");
            {
                std::ofstream ofs(archive_path, std::ios::binary);
                if (!ofs) {
                    std::cerr << "(AR) فشل الكتابة إلى: " << archive_path.string()
                              << " / (EN) Failed to write: " << archive_path.string() << std::endl;
                    return false;
                }
                ofs.write(response.body.data(), static_cast<std::streamsize>(response.body.size()));
            }
            
            // (AR) فك الضغط (إذا كان tar.gz)
            // (EN) Extract (if tar.gz)
            #ifdef _WIN32
            std::string cmd = "tar -xzf \"" + archive_path.string() + "\" -C \"" + pkg_dir.string() + "\"";
            #else
            std::string cmd = "tar -xzf '" + archive_path.string() + "' -C '" + pkg_dir.string() + "'";
            #endif
            
            int ret = std::system(cmd.c_str());
            if (ret != 0) {
                // (AR) ربما ليس tar.gz — نحتفظ بالملف كما هو
                // (EN) Maybe not tar.gz — keep file as-is
            }
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "(AR) خطأ في التثبيت: " << e.what()
                      << " / (EN) Install error: " << e.what() << std::endl;
            return false;
        }
    }

private:
    std::string base_url_;

    /**
     * @brief ترميز URL بسيط
     */
    static std::string urlEncode(const std::string& str) {
        std::string result;
        result.reserve(str.size() * 3);
        for (unsigned char c : str) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
                result += static_cast<char>(c);
            } else if (c == ' ') {
                result += '+';
            } else {
                // (AR) ترميز UTF-8 بايتات عربية
                // (EN) Encode UTF-8 bytes for Arabic characters
                char hex[4];
                std::snprintf(hex, sizeof(hex), "%%%02X", c);
                result += hex;
            }
        }
        return result;
    }

    /**
     * @brief تحليل بسيط لمصفوفة JSON — يستخرج قيم حقل معين
     * @brief Simple JSON array parser — extracts values of a specific field
     * 
     * يتعامل مع: [{"name": "حزمة1"}, {"name": "حزمة2"}]
     */
    static std::vector<std::string> parseJsonStringArray(const std::string& json, const std::string& field) {
        std::vector<std::string> results;
        std::string searchKey = "\"" + field + "\"";
        size_t pos = 0;
        
        while ((pos = json.find(searchKey, pos)) != std::string::npos) {
            pos += searchKey.size();
            // (AR) تخطي : ومسافات
            // (EN) Skip : and whitespace
            pos = json.find(':', pos);
            if (pos == std::string::npos) break;
            pos++;
            while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
            
            if (pos < json.size() && json[pos] == '"') {
                pos++; // skip opening quote
                size_t end = json.find('"', pos);
                if (end != std::string::npos) {
                    results.push_back(json.substr(pos, end - pos));
                    pos = end + 1;
                }
            }
        }
        return results;
    }
};

} // namespace pkg
} // namespace sad
