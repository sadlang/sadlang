// ===================================================================
// طلب_متسلسل - بناء طلبات HTTP بنمط السلسلة
// fluent_request.cpp - Fluent HTTP Request Builder
// ===================================================================
// يوفر واجهة عربية لبناء طلبات HTTP بطريقة متسلسلة:
// طلب().عنوان("...").طريقة(حصول).ترويسة("نوع", "json").أرسل()
// ===================================================================

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <chrono>
#include <sstream>

namespace sad {
namespace stdlib {
namespace network {

/// طريقة HTTP
enum class HttpMethod {
    GET,        // حصول
    POST,       // إرسال
    PUT,        // وضع 
    DELETE_,    // حذف
    PATCH,      // تصحيح
    HEAD,       // رأس
    OPTIONS,    // خيارات
};

/// ترويسة HTTP
struct HttpHeader {
    std::string name;
    std::string value;
};

/// جسم الطلب
struct RequestBody {
    std::string content;
    std::string content_type;
};

/// نتيجة الاستجابة
struct HttpResponse {
    int status_code;
    std::string status_text;
    std::map<std::string, std::string> headers;
    std::string body;
    std::chrono::milliseconds elapsed;
    
    /// هل الاستجابة ناجحة؟
    bool isSuccess() const { return status_code >= 200 && status_code < 300; }
    
    /// هل حدث خطأ في الخادم؟
    bool isServerError() const { return status_code >= 500; }
    
    /// هل حدث خطأ في العميل؟
    bool isClientError() const { return status_code >= 400 && status_code < 500; }
};

/// باني الطلبات المتسلسل
class FluentRequest {
public:
    /// إنشاء طلب جديد
    FluentRequest() = default;
    
    /// تحديد عنوان URL
    FluentRequest& عنوان(const std::string& url) {
        url_ = url;
        return *this;
    }
    
    /// تحديد طريقة HTTP
    FluentRequest& طريقة(HttpMethod method) {
        method_ = method;
        return *this;
    }
    
    /// اختصار: طلب GET
    FluentRequest& حصول(const std::string& url) {
        method_ = HttpMethod::GET;
        url_ = url;
        return *this;
    }
    
    /// اختصار: طلب POST
    FluentRequest& إرسال_إلى(const std::string& url) {
        method_ = HttpMethod::POST;
        url_ = url;
        return *this;
    }
    
    /// إضافة ترويسة
    FluentRequest& ترويسة(const std::string& name, const std::string& value) {
        headers_.push_back({name, value});
        return *this;
    }
    
    /// تحديد نوع المحتوى
    FluentRequest& نوع_المحتوى(const std::string& content_type) {
        headers_.push_back({"Content-Type", content_type});
        return *this;
    }
    
    /// إضافة ترويسة التفويض
    FluentRequest& تفويض(const std::string& token) {
        headers_.push_back({"Authorization", "Bearer " + token});
        return *this;
    }
    
    /// تحديد جسم الطلب كنص
    FluentRequest& جسم(const std::string& body) {
        body_ = RequestBody{body, "text/plain"};
        return *this;
    }
    
    /// تحديد جسم الطلب كـ JSON
    FluentRequest& جسم_json(const std::string& json) {
        body_ = RequestBody{json, "application/json"};
        headers_.push_back({"Content-Type", "application/json"});
        return *this;
    }
    
    /// تحديد المهلة الزمنية
    FluentRequest& مهلة(std::chrono::milliseconds timeout) {
        timeout_ = timeout;
        return *this;
    }
    
    /// تحديد المهلة بالثواني
    FluentRequest& مهلة_ثواني(int seconds) {
        timeout_ = std::chrono::seconds(seconds);
        return *this;
    }
    
    /// تحديد عدد المحاولات
    FluentRequest& محاولات(int count) {
        retry_count_ = count;
        return *this;
    }
    
    /// إضافة معامل استعلام
    FluentRequest& معامل(const std::string& key, const std::string& value) {
        query_params_.push_back({key, value});
        return *this;
    }
    
    /// تتبع التقدم
    FluentRequest& عند_تقدم(std::function<void(size_t, size_t)> callback) {
        progress_callback_ = std::move(callback);
        return *this;
    }
    
    /// بناء عنوان URL النهائي مع المعاملات
    std::string buildUrl() const {
        if (query_params_.empty()) return url_;
        
        std::string result = url_;
        result += "?";
        for (size_t i = 0; i < query_params_.size(); i++) {
            if (i > 0) result += "&";
            result += query_params_[i].name + "=" + query_params_[i].value;
        }
        return result;
    }
    
    /// الحصول على معلومات الطلب
    const std::string& url() const { return url_; }
    HttpMethod method() const { return method_; }
    const std::vector<HttpHeader>& headers() const { return headers_; }
    const std::optional<RequestBody>& body() const { return body_; }
    std::chrono::milliseconds timeout() const { return timeout_; }
    int retryCount() const { return retry_count_; }

private:
    std::string url_;
    HttpMethod method_ = HttpMethod::GET;
    std::vector<HttpHeader> headers_;
    std::optional<RequestBody> body_;
    std::chrono::milliseconds timeout_{30000};
    int retry_count_ = 0;
    std::vector<HttpHeader> query_params_;
    std::function<void(size_t, size_t)> progress_callback_;
};

/// بناء طلب جديد (نقطة دخول)
inline FluentRequest طلب() {
    return FluentRequest();
}

} // namespace network
} // namespace stdlib
} // namespace sad
