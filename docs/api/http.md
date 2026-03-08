# وثائق API — وحدة http

> المسار: `stdlib/http/`
> مساحة الأسماء: `sad::stdlib::http`

## نظرة عامة
الوحدة تقدم طبقة HTTP عالية المستوى: methods/status, headers, request/response, cookies, clients/servers.

## الملفات الأساسية
- `http/http_module.h`
- `http/http_module.cpp`

## API رئيسية
- `enum class HttpMethod`
- `enum class HttpStatus`
- `method_to_string(method)`
- `string_to_method(text)`
- `get_status_description(status)`
- `class HttpHeaders`
  - `set/get/has/remove/clear/to_string`
- `struct HttpCookie`
  - `to_set_cookie_string()`
  - `from_set_cookie_string(text)`
- `class HttpRequest`
  - `set_method`, `set_url`, `set_header`, `add_cookie`, `get_query_param`

## مثال سريع
```cpp
using namespace sad::stdlib::http;
HttpRequest req(HttpMethod::GET, "https://example.com");
req.set_header("Accept", "application/json");
```

## ملاحظات
- اختبارات الوحدة تغطي headers/cookies/status/method conversion بشكل مكثف.
