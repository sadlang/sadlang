// بسم الله الرحمن الرحيم
// HTTP Base - أساسيات HTTP
// HTTP Base Utilities and Common Definitions

#ifndef SAD_NETWORK_HTTP_BASE_H
#define SAD_NETWORK_HTTP_BASE_H

#include <string>
#include <map>
#include <vector>
#include <cstdint>

#ifdef DELETE
#undef DELETE
#endif

namespace sad
{
    namespace network
    {
        namespace http
        {

            // ==========================================
            // HTTP Version Enum
            // ==========================================

            enum class HttpVersion
            {
                HTTP_1_0, // HTTP/1.0
                HTTP_1_1, // HTTP/1.1 (default)
                HTTP_2_0  // HTTP/2.0
            };

            // ==========================================
            // HTTP Method Enum
            // ==========================================

            enum class HttpMethod
            {
                GET,
                POST,
                PUT,
                DELETE,
                PATCH,
                HEAD,
                OPTIONS,
                CONNECT,
                TRACE
            };

            // ==========================================
            // HTTP Status Code Enum
            // ==========================================

            enum class HttpStatus
            {
                // 1xx Informational
                Continue = 100,
                SwitchingProtocols = 101,
                Processing = 102,

                // 2xx Success
                OK = 200,
                Created = 201,
                Accepted = 202,
                NonAuthoritativeInformation = 203,
                NoContent = 204,
                ResetContent = 205,
                PartialContent = 206,

                // 3xx Redirection
                MultipleChoices = 300,
                MovedPermanently = 301,
                Found = 302,
                SeeOther = 303,
                NotModified = 304,
                UseProxy = 305,
                TemporaryRedirect = 307,
                PermanentRedirect = 308,

                // 4xx Client Error
                BadRequest = 400,
                Unauthorized = 401,
                PaymentRequired = 402,
                Forbidden = 403,
                NotFound = 404,
                MethodNotAllowed = 405,
                NotAcceptable = 406,
                ProxyAuthenticationRequired = 407,
                RequestTimeout = 408,
                Conflict = 409,
                Gone = 410,
                LengthRequired = 411,
                PreconditionFailed = 412,
                PayloadTooLarge = 413,
                URITooLong = 414,
                UnsupportedMediaType = 415,
                RangeNotSatisfiable = 416,
                ExpectationFailed = 417,
                ImATeapot = 418,
                UnprocessableEntity = 422,
                TooManyRequests = 429,

                // 5xx Server Error
                InternalServerError = 500,
                NotImplemented = 501,
                BadGateway = 502,
                ServiceUnavailable = 503,
                GatewayTimeout = 504,
                HTTPVersionNotSupported = 505
            };

            // ==========================================
            // Type Aliases
            // ==========================================

            using Headers = std::map<std::string, std::string>;
            using QueryParams = std::map<std::string, std::string>;

            // ==========================================
            // Helper Functions
            // ==========================================

            /**
             * @brief Convert HTTP method to string
             * @param method The HTTP method
             * @return String representation (e.g., "GET", "POST")
             */
            std::string method_to_string(HttpMethod method);

            /**
             * @brief Convert string to HTTP method
             * @param method_str The method string (e.g., "GET", "POST")
             * @return The HTTP method enum
             * @throws std::invalid_argument if method is unknown
             */
            HttpMethod string_to_method(const std::string &method_str);

            /**
             * @brief Convert HTTP version to string
             * @param version The HTTP version
             * @return String representation (e.g., "HTTP/1.1")
             */
            std::string version_to_string(HttpVersion version);

            /**
             * @brief Convert HTTP status code to string
             * @param status The HTTP status code
             * @return Status text (e.g., "OK", "Not Found")
             */
            std::string status_to_string(HttpStatus status);

            /**
             * @brief Get status text in Arabic
             * @param status The HTTP status code
             * @return Arabic status text
             */
            std::string status_to_arabic(HttpStatus status);

            /**
             * @brief URL encode a string
             * @param value The string to encode
             * @return URL-encoded string
             */
            std::string url_encode(const std::string &value);

            /**
             * @brief URL decode a string
             * @param value The string to decode
             * @return URL-decoded string
             */
            std::string url_decode(const std::string &value);

            /**
             * @brief Build query string from parameters
             * @param params The query parameters
             * @return Query string (e.g., "?key1=value1&key2=value2")
             */
            std::string build_query_string(const QueryParams &params);

            /**
             * @brief Parse query string into parameters
             * @param query The query string
             * @return Map of query parameters
             */
            QueryParams parse_query_string(const std::string &query);

            /**
             * @brief Case-insensitive header comparison
             */
            struct CaseInsensitiveCompare
            {
                bool operator()(const std::string &a, const std::string &b) const;
            };

            using HeadersCI = std::map<std::string, std::string, CaseInsensitiveCompare>;

            /**
             * @brief Common HTTP headers as constants
             */
            namespace headers
            {
                // Request headers
                constexpr const char *Accept = "Accept";
                constexpr const char *AcceptEncoding = "Accept-Encoding";
                constexpr const char *AcceptLanguage = "Accept-Language";
                constexpr const char *Authorization = "Authorization";
                constexpr const char *CacheControl = "Cache-Control";
                constexpr const char *Connection = "Connection";
                constexpr const char *ContentType = "Content-Type";
                constexpr const char *ContentLength = "Content-Length";
                constexpr const char *Cookie = "Cookie";
                constexpr const char *Host = "Host";
                constexpr const char *Origin = "Origin";
                constexpr const char *Referer = "Referer";
                constexpr const char *UserAgent = "User-Agent";

                // Response headers
                constexpr const char *AccessControlAllowOrigin = "Access-Control-Allow-Origin";
                constexpr const char *AccessControlAllowMethods = "Access-Control-Allow-Methods";
                constexpr const char *AccessControlAllowHeaders = "Access-Control-Allow-Headers";
                constexpr const char *ContentEncoding = "Content-Encoding";
                constexpr const char *Date = "Date";
                constexpr const char *ETag = "ETag";
                constexpr const char *Expires = "Expires";
                constexpr const char *LastModified = "Last-Modified";
                constexpr const char *Location = "Location";
                constexpr const char *Server = "Server";
                constexpr const char *SetCookie = "Set-Cookie";
                constexpr const char *TransferEncoding = "Transfer-Encoding";
                constexpr const char *Vary = "Vary";
            }

            /**
             * @brief Common MIME types
             */
            namespace mime_types
            {
                constexpr const char *TextPlain = "text/plain";
                constexpr const char *TextHtml = "text/html";
                constexpr const char *TextCss = "text/css";
                constexpr const char *TextJavascript = "text/javascript";
                constexpr const char *ApplicationJson = "application/json";
                constexpr const char *ApplicationXml = "application/xml";
                constexpr const char *ApplicationFormUrlencoded = "application/x-www-form-urlencoded";
                constexpr const char *MultipartFormData = "multipart/form-data";
                constexpr const char *ApplicationOctetStream = "application/octet-stream";
                constexpr const char *ImagePng = "image/png";
                constexpr const char *ImageJpeg = "image/jpeg";
                constexpr const char *ImageGif = "image/gif";
                constexpr const char *ImageSvg = "image/svg+xml";
            }

        } // namespace http
    } // namespace network
} // namespace sad

#endif // SAD_NETWORK_HTTP_BASE_H
