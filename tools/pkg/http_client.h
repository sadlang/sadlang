// بسم الله الرحمن الرحيم
/**
 * @file http_client.h
 * @brief HTTP Client using WinHTTP/POSIX - عميل HTTP باستخدام WinHTTP
 *
 * عميل HTTP لا يحتاج تبعيات خارجية:
 * - على Windows: يستخدم WinHTTP (مضمّن في النظام)
 * - على Linux/macOS: يستخدم libcurl عبر سطر الأوامر
 *
 * HTTP client with zero external dependencies:
 * - On Windows: uses WinHTTP (built into the OS)
 * - On Linux/macOS: uses curl CLI as fallback
 *
 * @author فريق لغة ص
 * @date فبراير 2026
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <cstdlib>
#endif

namespace sad {
namespace pkg {

// ============================================================================
// HTTP Response - استجابة HTTP
// ============================================================================

struct HttpResponse {
    int status_code = 0;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string error;

    bool ok() const { return status_code >= 200 && status_code < 300; }
};

// ============================================================================
// URL Parser - محلل URL
// ============================================================================

struct ParsedUrl {
    std::string scheme;   // "https" or "http"
    std::string host;
    int port = 0;
    std::string path;
    std::string query;

    static ParsedUrl parse(const std::string& url) {
        ParsedUrl result;

        // Scheme
        size_t scheme_end = url.find("://");
        if (scheme_end == std::string::npos) {
            result.scheme = "https";
            scheme_end = 0;
        } else {
            result.scheme = url.substr(0, scheme_end);
            scheme_end += 3;
        }

        // Host and port
        size_t path_start = url.find('/', scheme_end);
        std::string host_port;
        if (path_start == std::string::npos) {
            host_port = url.substr(scheme_end);
            result.path = "/";
        } else {
            host_port = url.substr(scheme_end, path_start - scheme_end);
            result.path = url.substr(path_start);
        }

        // Query string
        size_t query_start = result.path.find('?');
        if (query_start != std::string::npos) {
            result.query = result.path.substr(query_start);
            result.path = result.path.substr(0, query_start);
        }

        // Port
        size_t colon = host_port.find(':');
        if (colon != std::string::npos) {
            result.host = host_port.substr(0, colon);
            result.port = std::stoi(host_port.substr(colon + 1));
        } else {
            result.host = host_port;
            result.port = (result.scheme == "https") ? 443 : 80;
        }

        return result;
    }
};

// ============================================================================
// HTTP Client - عميل HTTP
// ============================================================================

/**
 * @brief عميل HTTP بدون تبعيات خارجية
 *
 * يستخدم WinHTTP على Windows (مضمّن في النظام)
 * لا يحتاج libcurl أو أي مكتبة خارجية
 */
class HttpClient {
public:
    struct Response {
        int status_code = 0;
        std::string body;
        std::map<std::string, std::string> headers;
        std::string error;

        bool ok() const { return status_code >= 200 && status_code < 300; }
    };

    HttpClient() = default;
    ~HttpClient() = default;

    /**
     * @brief طلب GET
     * @param url العنوان الكامل
     * @param headers رؤوس HTTP إضافية
     * @return استجابة HTTP
     */
    Response get(const std::string& url,
                 const std::map<std::string, std::string>& headers = {}) {
#ifdef _WIN32
        return winhttp_request("GET", url, "", headers);
#else
        return curl_cli_request("GET", url, "", headers);
#endif
    }

    /**
     * @brief طلب POST
     * @param url العنوان الكامل
     * @param body محتوى الطلب
     * @param headers رؤوس HTTP إضافية
     * @return استجابة HTTP
     */
    Response post(const std::string& url,
                  const std::string& body,
                  const std::map<std::string, std::string>& headers = {}) {
#ifdef _WIN32
        return winhttp_request("POST", url, body, headers);
#else
        return curl_cli_request("POST", url, body, headers);
#endif
    }

    /**
     * @brief طلب HTTP عام (GET, POST, PUT, DELETE, PATCH...)
     * @param method الطريقة (GET, POST, DELETE, ...)
     * @param url العنوان الكامل
     * @param body محتوى الطلب (فارغ لطلبات بدون جسم)
     * @param headers رؤوس HTTP إضافية
     * @return استجابة HTTP
     */
    Response request(const std::string& method,
                     const std::string& url,
                     const std::string& body = "",
                     const std::vector<std::pair<std::string, std::string>>& headers = {}) {
        std::map<std::string, std::string> hdr_map;
        for (const auto& [k, v] : headers) hdr_map[k] = v;
#ifdef _WIN32
        return winhttp_request(method, url, body, hdr_map);
#else
        return curl_cli_request(method, url, body, hdr_map);
#endif
    }

    /**
     * @brief تنزيل ملف
     * @param url عنوان التنزيل
     * @param dest_path مسار الملف الهدف
     * @param progress_callback دالة التقدم (اختياري)
     * @return true إذا نجح التنزيل
     */
    bool download_file(const std::string& url,
                       const std::filesystem::path& dest_path,
                       std::function<void(size_t, size_t)> progress_callback = nullptr) {
        std::filesystem::create_directories(dest_path.parent_path());

#ifdef _WIN32
        return winhttp_download(url, dest_path, progress_callback);
#else
        return curl_cli_download(url, dest_path);
#endif
    }

    /**
     * @brief رفع ملف (multipart/form-data)
     * @param url عنوان الرفع
     * @param file_path مسار الملف
     * @param field_name اسم الحقل
     * @param auth_token رمز المصادقة
     * @param extra_fields حقول إضافية (اسم → قيمة) ترسل مع الملف
     * @return استجابة HTTP
     */
    Response upload_file(const std::string& url,
                         const std::filesystem::path& file_path,
                         const std::string& field_name = "package",
                         const std::string& auth_token = "",
                         const std::vector<std::pair<std::string, std::string>>& extra_fields = {}) {
#ifdef _WIN32
        return winhttp_upload(url, file_path, field_name, auth_token, extra_fields);
#else
        return curl_cli_upload(url, file_path, field_name, auth_token, extra_fields);
#endif
    }

private:

#ifdef _WIN32
    // ========================================================================
    // WinHTTP Implementation - تنفيذ WinHTTP
    // ========================================================================

    /**
     * @brief تنفيذ طلب HTTP باستخدام WinHTTP
     */
    Response winhttp_request(const std::string& method,
                             const std::string& url,
                             const std::string& body,
                             const std::map<std::string, std::string>& headers) {
        Response response;
        auto parsed = ParsedUrl::parse(url);

        // تحويل السلاسل إلى wstring
        std::wstring whost(parsed.host.begin(), parsed.host.end());
        std::wstring wpath = string_to_wstring(parsed.path + parsed.query);
        std::wstring wmethod(method.begin(), method.end());

        // فتح جلسة WinHTTP
        HINTERNET hSession = WinHttpOpen(
            L"SadLang-PackageManager/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

        if (!hSession) {
            response.error = "فشل فتح جلسة WinHTTP";
            return response;
        }

        // الاتصال بالخادم
        HINTERNET hConnect = WinHttpConnect(
            hSession,
            whost.c_str(),
            static_cast<INTERNET_PORT>(parsed.port),
            0);

        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            response.error = "فشل الاتصال بالخادم: " + parsed.host;
            return response;
        }

        // إنشاء الطلب
        DWORD flags = (parsed.scheme == "https") ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET hRequest = WinHttpOpenRequest(
            hConnect,
            wmethod.c_str(),
            wpath.c_str(),
            NULL,
            WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            flags);

        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            response.error = "فشل إنشاء الطلب";
            return response;
        }

        // إضافة الرؤوس
        for (const auto& [key, value] : headers) {
            std::wstring wheader = string_to_wstring(key + ": " + value);
            WinHttpAddRequestHeaders(hRequest, wheader.c_str(),
                                     static_cast<DWORD>(-1L), WINHTTP_ADDREQ_FLAG_ADD);
        }

        // إرسال الطلب
        BOOL result;
        if (!body.empty()) {
            std::wstring content_type = L"Content-Type: application/json\r\n";
            WinHttpAddRequestHeaders(hRequest, content_type.c_str(),
                                     static_cast<DWORD>(-1L), WINHTTP_ADDREQ_FLAG_ADD);
            result = WinHttpSendRequest(
                hRequest,
                WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                (LPVOID)body.c_str(), static_cast<DWORD>(body.size()),
                static_cast<DWORD>(body.size()),
                0);
        } else {
            result = WinHttpSendRequest(
                hRequest,
                WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
        }

        if (!result) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            response.error = "فشل إرسال الطلب (خطأ: " + std::to_string(GetLastError()) + ")";
            return response;
        }

        // استقبال الاستجابة
        result = WinHttpReceiveResponse(hRequest, NULL);
        if (!result) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            response.error = "فشل استقبال الاستجابة";
            return response;
        }

        // قراءة رمز الحالة
        DWORD statusCode = 0;
        DWORD statusCodeSize = sizeof(statusCode);
        WinHttpQueryHeaders(hRequest,
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX,
                            &statusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX);
        response.status_code = static_cast<int>(statusCode);

        // قراءة محتوى الاستجابة
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        std::string responseBody;

        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (dwSize == 0) break;

            std::vector<char> buffer(dwSize + 1, 0);
            if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) break;

            responseBody.append(buffer.data(), dwDownloaded);
        } while (dwSize > 0);

        response.body = responseBody;

        // التنظيف
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        return response;
    }

    /**
     * @brief تنزيل ملف باستخدام WinHTTP
     */
    bool winhttp_download(const std::string& url,
                          const std::filesystem::path& dest_path,
                          std::function<void(size_t, size_t)> progress_callback) {
        auto parsed = ParsedUrl::parse(url);
        std::wstring whost(parsed.host.begin(), parsed.host.end());
        std::wstring wpath = string_to_wstring(parsed.path + parsed.query);

        HINTERNET hSession = WinHttpOpen(
            L"SadLang-PackageManager/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);
        if (!hSession) return false;

        HINTERNET hConnect = WinHttpConnect(
            hSession, whost.c_str(),
            static_cast<INTERNET_PORT>(parsed.port), 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return false;
        }

        DWORD flags = (parsed.scheme == "https") ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET hRequest = WinHttpOpenRequest(
            hConnect, L"GET", wpath.c_str(), NULL,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        // الحصول على حجم الملف الكلي
        DWORD contentLength = 0;
        DWORD contentLengthSize = sizeof(contentLength);
        WinHttpQueryHeaders(hRequest,
                            WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX,
                            &contentLength, &contentLengthSize,
                            WINHTTP_NO_HEADER_INDEX);

        // فتح الملف للكتابة
        std::ofstream outFile(dest_path, std::ios::binary);
        if (!outFile.is_open()) {
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        // قراءة وكتابة البيانات
        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        size_t totalDownloaded = 0;

        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
            if (dwSize == 0) break;

            std::vector<char> buffer(dwSize);
            if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) break;

            outFile.write(buffer.data(), dwDownloaded);
            totalDownloaded += dwDownloaded;

            if (progress_callback && contentLength > 0) {
                progress_callback(totalDownloaded, contentLength);
            }
        } while (dwSize > 0);

        outFile.close();

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        return totalDownloaded > 0;
    }

    /**
     * @brief رفع ملف باستخدام WinHTTP (multipart/form-data)
     */
    Response winhttp_upload(const std::string& url,
                            const std::filesystem::path& file_path,
                            const std::string& field_name,
                            const std::string& auth_token,
                            const std::vector<std::pair<std::string, std::string>>& extra_fields = {}) {
        // قراءة محتوى الملف
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            return Response{0, "", {}, "فشل فتح الملف: " + file_path.string()};
        }

        std::string file_content((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
        file.close();

        // بناء multipart body
        std::string boundary = "----SadPkgBoundary" + std::to_string(
            std::chrono::system_clock::now().time_since_epoch().count());
        std::string filename = file_path.filename().string();

        std::string multipart_body;

        // حقول إضافية (مثل metadata)
        for (const auto& [key, value] : extra_fields) {
            multipart_body += "--" + boundary + "\r\n";
            multipart_body += "Content-Disposition: form-data; name=\"" + key + "\"\r\n\r\n";
            multipart_body += value + "\r\n";
        }

        // حقل الملف
        multipart_body += "--" + boundary + "\r\n";
        multipart_body += "Content-Disposition: form-data; name=\"" + field_name +
                          "\"; filename=\"" + filename + "\"\r\n";
        multipart_body += "Content-Type: application/octet-stream\r\n\r\n";
        multipart_body += file_content;
        multipart_body += "\r\n--" + boundary + "--\r\n";

        // إرسال الطلب مع رؤوس مخصصة
        std::map<std::string, std::string> headers;
        headers["Content-Type"] = "multipart/form-data; boundary=" + boundary;
        if (!auth_token.empty()) {
            headers["Authorization"] = "Bearer " + auth_token;
        }

        return winhttp_request("POST", url, multipart_body, headers);
    }

    /**
     * @brief تحويل string إلى wstring
     */
    std::wstring string_to_wstring(const std::string& str) {
        if (str.empty()) return L"";
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                                               static_cast<int>(str.size()), nullptr, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(),
                            static_cast<int>(str.size()), &wstr[0], size_needed);
        return wstr;
    }

#else
    // ========================================================================
    // POSIX/CLI Fallback - بديل POSIX باستخدام أمر curl
    // ========================================================================

    Response curl_cli_request(const std::string& method,
                              const std::string& url,
                              const std::string& body,
                              const std::map<std::string, std::string>& headers) {
        Response response;
        std::string cmd = "curl -s -w '\\n%{http_code}' -X " + method;

        for (const auto& [key, value] : headers) {
            cmd += " -H '" + key + ": " + value + "'";
        }
        if (!body.empty()) {
            cmd += " -d '" + body + "'";
        }
        cmd += " '" + url + "' 2>/dev/null";

        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            response.error = "Failed to execute curl";
            return response;
        }

        std::string output;
        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }
        int exit_code = pclose(pipe);

        if (exit_code != 0 || output.empty()) {
            response.error = "curl request failed";
            return response;
        }

        // آخر سطر هو رمز الحالة
        size_t last_newline = output.rfind('\n', output.size() - 2);
        if (last_newline != std::string::npos) {
            response.body = output.substr(0, last_newline);
            response.status_code = std::stoi(output.substr(last_newline + 1));
        } else {
            response.body = output;
            response.status_code = 200;
        }

        return response;
    }

    bool curl_cli_download(const std::string& url,
                           const std::filesystem::path& dest_path) {
        std::string cmd = "curl -s -L -o '" + dest_path.string() + "' '" + url + "'";
        return system(cmd.c_str()) == 0;
    }

    Response curl_cli_upload(const std::string& url,
                             const std::filesystem::path& file_path,
                             const std::string& field_name,
                             const std::string& auth_token,
                             const std::vector<std::pair<std::string, std::string>>& extra_fields = {}) {
        std::string cmd = "curl -s -w '\\n%{http_code}'";
        for (const auto& [key, value] : extra_fields) {
            cmd += " -F '" + key + "=" + value + "'";
        }
        cmd += " -F '" + field_name + "=@" + file_path.string() + "'";
        if (!auth_token.empty()) {
            cmd += " -H 'Authorization: Bearer " + auth_token + "'";
        }
        cmd += " '" + url + "' 2>/dev/null";

        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) return Response{0, "", {}, "Failed to execute curl"};

        std::string output;
        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            output += buffer;
        }
        pclose(pipe);

        Response response;
        size_t last_newline = output.rfind('\n', output.size() - 2);
        if (last_newline != std::string::npos) {
            response.body = output.substr(0, last_newline);
            response.status_code = std::stoi(output.substr(last_newline + 1));
        }
        return response;
    }
#endif
};

// ============================================================================
// URL Encoding Helper - مساعد ترميز URL
// ============================================================================

/**
 * @brief ترميز URL
 */
inline std::string url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        if (std::isalnum(static_cast<unsigned char>(c)) ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
            escaped << std::nouppercase;
        }
    }

    return escaped.str();
}

} // namespace pkg
} // namespace sad
