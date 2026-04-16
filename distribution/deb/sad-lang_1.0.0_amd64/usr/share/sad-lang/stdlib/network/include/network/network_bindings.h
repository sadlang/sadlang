// بسم الله الرحمن الرحيم
// مكتبة الشبكات - واجهة C++ للغة ص
// Network Library - C++ Interface for Sad Language

#ifndef SAD_NETWORK_BINDINGS_H
#define SAD_NETWORK_BINDINGS_H

#include <cstdint>
#include <cstddef>

// تصدير الدوال للغة ص
// Export functions for Sad language

extern "C" {

// ===== NetworkError Functions =====

// إنشاء رسالة خطأ بالعربية
// Create Arabic error message
const char* sad_network_error_arabic(int error_code);

// إنشاء رسالة خطأ بالإنجليزية
// Create English error message
const char* sad_network_error_english(int error_code);

// ===== SocketAddress Functions =====

// إنشاء عنوان مقبس IPv4
// Create IPv4 socket address
void* sad_socket_address_create(const char* ip, uint16_t port);

// إنشاء عنوان مقبس IPv6
// Create IPv6 socket address
void* sad_socket_address_create_v6(const char* ip, uint16_t port);

// حذف عنوان المقبس
// Delete socket address
void sad_socket_address_delete(void* addr);

// الحصول على عنوان IP
// Get IP address
const char* sad_socket_address_get_ip(void* addr);

// الحصول على رقم المنفذ
// Get port number
uint16_t sad_socket_address_get_port(void* addr);

// هل العنوان IPv4؟
// Is IPv4 address?
bool sad_socket_address_is_ipv4(void* addr);

// هل العنوان IPv6؟
// Is IPv6 address?
bool sad_socket_address_is_ipv6(void* addr);

// تحويل إلى نص
// Convert to string
const char* sad_socket_address_to_string(void* addr);

// ===== TcpSocket Functions =====

// إنشاء مقبس TCP
// Create TCP socket
void* sad_tcp_socket_create();

// حذف مقبس TCP
// Delete TCP socket
void sad_tcp_socket_delete(void* socket);

// ربط المقبس بمنفذ
// Bind socket to port
bool sad_tcp_socket_bind(void* socket, uint16_t port);

// ربط المقبس بعنوان ومنفذ
// Bind socket to address and port
bool sad_tcp_socket_bind_addr(void* socket, const char* address, uint16_t port);

// الاستماع للاتصالات
// Listen for connections
bool sad_tcp_socket_listen(void* socket, int backlog);

// قبول اتصال
// Accept connection
void* sad_tcp_socket_accept(void* socket);

// الاتصال بخادم
// Connect to server
bool sad_tcp_socket_connect(void* socket, const char* address, uint16_t port);

// إرسال بيانات
// Send data
int sad_tcp_socket_send(void* socket, const char* data, size_t size);

// إرسال جميع البيانات
// Send all data
bool sad_tcp_socket_send_all(void* socket, const char* data, size_t size);

// استقبال بيانات
// Receive data
int sad_tcp_socket_receive(void* socket, char* buffer, size_t size);

// استقبال عدد محدد من البايتات
// Receive exact number of bytes
bool sad_tcp_socket_receive_exact(void* socket, char* buffer, size_t size);

// هل المقبس متصل؟
// Is socket connected?
bool sad_tcp_socket_is_connected(void* socket);

// هل المقبس صالح؟
// Is socket valid?
bool sad_tcp_socket_is_valid(void* socket);

// إغلاق المقبس
// Close socket
void sad_tcp_socket_close(void* socket);

// إيقاف الإرسال
// Shutdown write
void sad_tcp_socket_shutdown_write(void* socket);

// إيقاف الاستقبال
// Shutdown read
void sad_tcp_socket_shutdown_read(void* socket);

// تعيين مهلة الاستقبال
// Set receive timeout
void sad_tcp_socket_set_receive_timeout(void* socket, int milliseconds);

// تعيين مهلة الإرسال
// Set send timeout
void sad_tcp_socket_set_send_timeout(void* socket, int milliseconds);

// تعيين إعادة استخدام العنوان
// Set reuse address
void sad_tcp_socket_set_reuse_address(void* socket, bool enable);

// تعيين keepalive
// Set keepalive
void sad_tcp_socket_set_keepalive(void* socket, bool enable);

// تعيين no delay (disable Nagle's algorithm)
// Set no delay
void sad_tcp_socket_set_no_delay(void* socket, bool enable);

// الحصول على عنوان الطرف الآخر
// Get peer address
void* sad_tcp_socket_get_peer_address(void* socket);

// الحصول على العنوان المحلي
// Get local address
void* sad_tcp_socket_get_local_address(void* socket);

// ===== UdpSocket Functions =====

// إنشاء مقبس UDP
// Create UDP socket
void* sad_udp_socket_create();

// حذف مقبس UDP
// Delete UDP socket
void sad_udp_socket_delete(void* socket);

// ربط المقبس بمنفذ
// Bind socket to port
bool sad_udp_socket_bind(void* socket, uint16_t port);

// ربط المقبس بعنوان ومنفذ
// Bind socket to address and port
bool sad_udp_socket_bind_addr(void* socket, const char* address, uint16_t port);

// إرسال بيانات إلى عنوان
// Send data to address
int sad_udp_socket_send_to(void* socket, const char* data, size_t size,
                            const char* address, uint16_t port);

// استقبال بيانات من عنوان
// Receive data from address
int sad_udp_socket_receive_from(void* socket, char* buffer, size_t size,
                                  char* from_address, uint16_t* from_port);

// الاتصال (وضع متصل)
// Connect (connected mode)
bool sad_udp_socket_connect(void* socket, const char* address, uint16_t port);

// إرسال في الوضع المتصل
// Send in connected mode
int sad_udp_socket_send(void* socket, const char* data, size_t size);

// استقبال في الوضع المتصل
// Receive in connected mode
int sad_udp_socket_receive(void* socket, char* buffer, size_t size);

// الانضمام لمجموعة بث متعدد
// Join multicast group
bool sad_udp_socket_join_multicast(void* socket, const char* group);

// مغادرة مجموعة بث متعدد
// Leave multicast group
bool sad_udp_socket_leave_multicast(void* socket, const char* group);

// تعيين TTL للبث المتعدد
// Set multicast TTL
void sad_udp_socket_set_multicast_ttl(void* socket, int ttl);

// تعيين loopback للبث المتعدد
// Set multicast loopback
void sad_udp_socket_set_multicast_loopback(void* socket, bool enable);

// تعيين البث
// Set broadcast
void sad_udp_socket_set_broadcast(void* socket, bool enable);

// تعيين مهلة الاستقبال
// Set receive timeout
void sad_udp_socket_set_receive_timeout(void* socket, int milliseconds);

// تعيين إعادة استخدام العنوان
// Set reuse address
void sad_udp_socket_set_reuse_address(void* socket, bool enable);

// هل المقبس صالح؟
// Is socket valid?
bool sad_udp_socket_is_valid(void* socket);

// إغلاق المقبس
// Close socket
void sad_udp_socket_close(void* socket);

// الحصول على العنوان المحلي
// Get local address
void* sad_udp_socket_get_local_address(void* socket);

// تعيين حجم buffer الاستقبال
// Set receive buffer size
void sad_udp_socket_set_receive_buffer_size(void* socket, int size);

// تعيين حجم buffer الإرسال
// Set send buffer size
void sad_udp_socket_set_send_buffer_size(void* socket, int size);

// الحصول على حجم buffer الاستقبال
// Get receive buffer size
int sad_udp_socket_get_receive_buffer_size(void* socket);

// الحصول على حجم buffer الإرسال
// Get send buffer size
int sad_udp_socket_get_send_buffer_size(void* socket);

// ===== Utility Functions =====

// تهيئة مكتبة الشبكات
// Initialize network library
bool sad_network_init();

// تنظيف مكتبة الشبكات
// Cleanup network library
void sad_network_cleanup();

// الحصول على آخر رمز خطأ
// Get last error code
int sad_network_get_last_error();

// الحصول على آخر رسالة خطأ
// Get last error message
const char* sad_network_get_last_error_message();

} // extern "C"

#endif // SAD_NETWORK_BINDINGS_H
