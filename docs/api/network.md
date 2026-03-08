# وثائق API — وحدة network

> المسار: `stdlib/network/`
> مساحة الأسماء: `sad::network` + `sad::network::http` + `sad::network::websocket`

## نظرة عامة
وحدة الشبكات توفر طبقة أخطاء موحّدة، عناوين مقابس، HTTP primitives، ووحدات WebSocket frame parsing/building.

## الملفات الأساسية
- `include/network/network_error.h`
- `include/socket/socket_address.h`
- `include/http/http_base.h`
- `include/http/http_request.h`
- `include/http/http_response.h`
- `include/http/http_cookie.h`
- `include/http/http_url.h`
- `include/websocket/websocket_base.h`
- `include/websocket/websocket_frame.h`

## أهم الأنواع والدوال
- `enum class NetworkErrorCode`
- `error_code_to_arabic(code)`
- `error_code_to_english(code)`
- `class NetworkException`
  - `error_code()`
  - `what_arabic()`
  - `what_english()`
  - `set_system_error_code(int)`
- `class SocketAddress`
  - `SocketAddress(ip, port)`
  - `any_ipv4(port)`
  - `localhost_ipv4(port)`
  - `is_valid_ipv4()`, `is_valid_ipv6()`
  - `get_ip()`, `get_port()`

## HTTP داخل network
- `method_to_string(HttpMethod)`
- `string_to_method("GET")`
- `status_to_string(HttpStatus)`
- `status_to_arabic(HttpStatus)`
- `url_encode()/url_decode()`
- `build_query_string()/parse_query_string()`

## مثال سريع
```cpp
using namespace sad::network;
using namespace sad::network::http;

SocketAddress addr("127.0.0.1", 8080);
auto code = NetworkErrorCode::TIMEOUT;
std::string msg = error_code_to_arabic(code);
```

## ملاحظات
- على Windows يلزم الانتباه لتضمين `winsock2.h` قبل `windows.h` في بعض السيناريوهات.
- اختبارات الوحدة تغطي حالات خطأ وتحقق عناوين IPv4/IPv6 وحالات HTTP الأساسية.
