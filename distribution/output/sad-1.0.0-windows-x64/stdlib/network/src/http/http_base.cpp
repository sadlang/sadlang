// بسم الله الرحمن الرحيم
// HTTP Base Implementation

#include "http_base.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace sad {
namespace network {
namespace http {

// ==========================================
// Method Conversion Functions
// ==========================================

std::string method_to_string(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET:     return "GET";
        case HttpMethod::POST:    return "POST";
        case HttpMethod::PUT:     return "PUT";
        case HttpMethod::DELETE:  return "DELETE";
        case HttpMethod::PATCH:   return "PATCH";
        case HttpMethod::HEAD:    return "HEAD";
        case HttpMethod::OPTIONS: return "OPTIONS";
        case HttpMethod::CONNECT: return "CONNECT";
        case HttpMethod::TRACE:   return "TRACE";
        default:                  return "UNKNOWN";
    }
}

HttpMethod string_to_method(const std::string& method_str) {
    if (method_str == "GET")     return HttpMethod::GET;
    if (method_str == "POST")    return HttpMethod::POST;
    if (method_str == "PUT")     return HttpMethod::PUT;
    if (method_str == "DELETE")  return HttpMethod::DELETE;
    if (method_str == "PATCH")   return HttpMethod::PATCH;
    if (method_str == "HEAD")    return HttpMethod::HEAD;
    if (method_str == "OPTIONS") return HttpMethod::OPTIONS;
    if (method_str == "CONNECT") return HttpMethod::CONNECT;
    if (method_str == "TRACE")   return HttpMethod::TRACE;
    
    throw std::invalid_argument("Unknown HTTP method: " + method_str);
}

// ==========================================
// Version Conversion Functions
// ==========================================

std::string version_to_string(HttpVersion version) {
    switch (version) {
        case HttpVersion::HTTP_1_0: return "HTTP/1.0";
        case HttpVersion::HTTP_1_1: return "HTTP/1.1";
        case HttpVersion::HTTP_2_0: return "HTTP/2.0";
        default:                    return "HTTP/1.1";
    }
}

// ==========================================
// Status Code Functions
// ==========================================

std::string status_to_string(HttpStatus status) {
    switch (status) {
        // 1xx Informational
        case HttpStatus::Continue:                   return "Continue";
        case HttpStatus::SwitchingProtocols:         return "Switching Protocols";
        case HttpStatus::Processing:                 return "Processing";
        
        // 2xx Success
        case HttpStatus::OK:                         return "OK";
        case HttpStatus::Created:                    return "Created";
        case HttpStatus::Accepted:                   return "Accepted";
        case HttpStatus::NonAuthoritativeInformation: return "Non-Authoritative Information";
        case HttpStatus::NoContent:                  return "No Content";
        case HttpStatus::ResetContent:               return "Reset Content";
        case HttpStatus::PartialContent:             return "Partial Content";
        
        // 3xx Redirection
        case HttpStatus::MultipleChoices:            return "Multiple Choices";
        case HttpStatus::MovedPermanently:           return "Moved Permanently";
        case HttpStatus::Found:                      return "Found";
        case HttpStatus::SeeOther:                   return "See Other";
        case HttpStatus::NotModified:                return "Not Modified";
        case HttpStatus::UseProxy:                   return "Use Proxy";
        case HttpStatus::TemporaryRedirect:          return "Temporary Redirect";
        case HttpStatus::PermanentRedirect:          return "Permanent Redirect";
        
        // 4xx Client Error
        case HttpStatus::BadRequest:                 return "Bad Request";
        case HttpStatus::Unauthorized:               return "Unauthorized";
        case HttpStatus::PaymentRequired:            return "Payment Required";
        case HttpStatus::Forbidden:                  return "Forbidden";
        case HttpStatus::NotFound:                   return "Not Found";
        case HttpStatus::MethodNotAllowed:           return "Method Not Allowed";
        case HttpStatus::NotAcceptable:              return "Not Acceptable";
        case HttpStatus::ProxyAuthenticationRequired: return "Proxy Authentication Required";
        case HttpStatus::RequestTimeout:             return "Request Timeout";
        case HttpStatus::Conflict:                   return "Conflict";
        case HttpStatus::Gone:                       return "Gone";
        case HttpStatus::LengthRequired:             return "Length Required";
        case HttpStatus::PreconditionFailed:         return "Precondition Failed";
        case HttpStatus::PayloadTooLarge:            return "Payload Too Large";
        case HttpStatus::URITooLong:                 return "URI Too Long";
        case HttpStatus::UnsupportedMediaType:       return "Unsupported Media Type";
        case HttpStatus::RangeNotSatisfiable:        return "Range Not Satisfiable";
        case HttpStatus::ExpectationFailed:          return "Expectation Failed";
        case HttpStatus::ImATeapot:                  return "I'm a teapot";
        case HttpStatus::UnprocessableEntity:        return "Unprocessable Entity";
        case HttpStatus::TooManyRequests:            return "Too Many Requests";
        
        // 5xx Server Error
        case HttpStatus::InternalServerError:        return "Internal Server Error";
        case HttpStatus::NotImplemented:             return "Not Implemented";
        case HttpStatus::BadGateway:                 return "Bad Gateway";
        case HttpStatus::ServiceUnavailable:         return "Service Unavailable";
        case HttpStatus::GatewayTimeout:             return "Gateway Timeout";
        case HttpStatus::HTTPVersionNotSupported:    return "HTTP Version Not Supported";
        
        default:                                     return "Unknown";
    }
}

std::string status_to_arabic(HttpStatus status) {
    switch (status) {
        // 1xx Informational
        case HttpStatus::Continue:                   return "استمر";
        case HttpStatus::SwitchingProtocols:         return "تبديل البروتوكولات";
        case HttpStatus::Processing:                 return "قيد المعالجة";
        
        // 2xx Success
        case HttpStatus::OK:                         return "نجح";
        case HttpStatus::Created:                    return "تم الإنشاء";
        case HttpStatus::Accepted:                   return "تم القبول";
        case HttpStatus::NonAuthoritativeInformation: return "معلومات غير موثوقة";
        case HttpStatus::NoContent:                  return "بدون محتوى";
        case HttpStatus::ResetContent:               return "إعادة تعيين المحتوى";
        case HttpStatus::PartialContent:             return "محتوى جزئي";
        
        // 3xx Redirection
        case HttpStatus::MultipleChoices:            return "خيارات متعددة";
        case HttpStatus::MovedPermanently:           return "نُقل نهائياً";
        case HttpStatus::Found:                      return "تم العثور عليه";
        case HttpStatus::SeeOther:                   return "انظر آخر";
        case HttpStatus::NotModified:                return "لم يتم التعديل";
        case HttpStatus::UseProxy:                   return "استخدم الوكيل";
        case HttpStatus::TemporaryRedirect:          return "إعادة توجيه مؤقتة";
        case HttpStatus::PermanentRedirect:          return "إعادة توجيه دائمة";
        
        // 4xx Client Error
        case HttpStatus::BadRequest:                 return "طلب خاطئ";
        case HttpStatus::Unauthorized:               return "غير مصرح";
        case HttpStatus::PaymentRequired:            return "الدفع مطلوب";
        case HttpStatus::Forbidden:                  return "ممنوع";
        case HttpStatus::NotFound:                   return "غير موجود";
        case HttpStatus::MethodNotAllowed:           return "الطريقة غير مسموحة";
        case HttpStatus::NotAcceptable:              return "غير مقبول";
        case HttpStatus::ProxyAuthenticationRequired: return "مصادقة الوكيل مطلوبة";
        case HttpStatus::RequestTimeout:             return "انتهت مهلة الطلب";
        case HttpStatus::Conflict:                   return "تعارض";
        case HttpStatus::Gone:                       return "لم يعد موجوداً";
        case HttpStatus::LengthRequired:             return "الطول مطلوب";
        case HttpStatus::PreconditionFailed:         return "فشل الشرط المسبق";
        case HttpStatus::PayloadTooLarge:            return "الحمولة كبيرة جداً";
        case HttpStatus::URITooLong:                 return "العنوان طويل جداً";
        case HttpStatus::UnsupportedMediaType:       return "نوع وسائط غير مدعوم";
        case HttpStatus::RangeNotSatisfiable:        return "النطاق غير قابل للتحقيق";
        case HttpStatus::ExpectationFailed:          return "فشل التوقع";
        case HttpStatus::ImATeapot:                  return "أنا إبريق شاي";
        case HttpStatus::UnprocessableEntity:        return "كيان غير قابل للمعالجة";
        case HttpStatus::TooManyRequests:            return "طلبات كثيرة جداً";
        
        // 5xx Server Error
        case HttpStatus::InternalServerError:        return "خطأ داخلي في الخادم";
        case HttpStatus::NotImplemented:             return "غير مُنفذ";
        case HttpStatus::BadGateway:                 return "بوابة سيئة";
        case HttpStatus::ServiceUnavailable:         return "الخدمة غير متاحة";
        case HttpStatus::GatewayTimeout:             return "انتهت مهلة البوابة";
        case HttpStatus::HTTPVersionNotSupported:    return "إصدار HTTP غير مدعوم";
        
        default:                                     return "غير معروف";
    }
}

// ==========================================
// URL Encoding Functions
// ==========================================

std::string url_encode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : value) {
        // Keep alphanumeric and other safe characters intact
        if (std::isalnum(static_cast<unsigned char>(c)) || 
            c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            // Percent-encode everything else
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
            escaped << std::nouppercase;
        }
    }

    return escaped.str();
}

std::string url_decode(const std::string& value) {
    std::string result;
    result.reserve(value.size());

    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%') {
            if (i + 2 < value.size()) {
                int hex_val;
                std::istringstream is(value.substr(i + 1, 2));
                if (is >> std::hex >> hex_val) {
                    result += static_cast<char>(hex_val);
                    i += 2;
                } else {
                    result += value[i];
                }
            } else {
                result += value[i];
            }
        } else if (value[i] == '+') {
            result += ' ';
        } else {
            result += value[i];
        }
    }

    return result;
}

// ==========================================
// Query String Functions
// ==========================================

std::string build_query_string(const QueryParams& params) {
    if (params.empty()) {
        return "";
    }

    std::ostringstream query;
    bool first = true;

    for (const auto& [key, value] : params) {
        if (!first) {
            query << '&';
        }
        first = false;
        query << url_encode(key) << '=' << url_encode(value);
    }

    return "?" + query.str();
}

QueryParams parse_query_string(const std::string& query) {
    QueryParams params;

    // Remove leading '?' if present
    std::string query_str = query;
    if (!query_str.empty() && query_str[0] == '?') {
        query_str = query_str.substr(1);
    }

    // Split by '&'
    size_t start = 0;
    size_t pos = 0;

    while ((pos = query_str.find('&', start)) != std::string::npos) {
        std::string param = query_str.substr(start, pos - start);
        size_t eq_pos = param.find('=');
        
        if (eq_pos != std::string::npos) {
            std::string key = url_decode(param.substr(0, eq_pos));
            std::string value = url_decode(param.substr(eq_pos + 1));
            params[key] = value;
        }
        
        start = pos + 1;
    }

    // Handle last parameter
    if (start < query_str.size()) {
        std::string param = query_str.substr(start);
        size_t eq_pos = param.find('=');
        
        if (eq_pos != std::string::npos) {
            std::string key = url_decode(param.substr(0, eq_pos));
            std::string value = url_decode(param.substr(eq_pos + 1));
            params[key] = value;
        }
    }

    return params;
}

// ==========================================
// Case-Insensitive Comparison
// ==========================================

bool CaseInsensitiveCompare::operator()(const std::string& a, const std::string& b) const {
    return std::lexicographical_compare(
        a.begin(), a.end(),
        b.begin(), b.end(),
        [](char a, char b) {
            return std::tolower(static_cast<unsigned char>(a)) < 
                   std::tolower(static_cast<unsigned char>(b));
        }
    );
}

} // namespace http
} // namespace network
} // namespace sad
