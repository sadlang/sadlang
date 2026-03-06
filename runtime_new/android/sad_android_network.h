/**
 * @file sad_android_network.h
 * @brief (AR) نظام الشبكات لرانتايم أندرويد — لغة ص
 * @brief (EN) Network system for Android runtime — Sad Language
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    نظام الشبكات — لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يوفر هذا النظام بنية تحتية للاتصال عبر الشبكة باستخدام
 * مقابس bionic الأصلية. مُصمم خصيصاً لتطبيق المحادثة (PoC).
 *
 * ╔══════════════════════════════════════════════════════════════════╗
 * ║                    المكونات الرئيسية                           ║
 * ╠══════════════════════════════════════════════════════════════════╣
 * ║                                                                ║
 * ║  ١. مقبس TCP (SadSocket):                                     ║
 * ║     • اتصال TCP/IP أساسي                                      ║
 * ║     • إرسال واستقبال بيانات خام                                ║
 * ║     • دعم IPv4 و IPv6                                         ║
 * ║                                                                ║
 * ║  ٢. مقبس TLS (SadTlsSocket):                                  ║
 * ║     • اتصال TCP مُشفّر عبر BoringSSL                          ║
 * ║     • أندرويد يتضمن BoringSSL مدمجاً                          ║
 * ║     • ضروري للمحادثة الآمنة                                   ║
 * ║                                                                ║
 * ║  ٣. عميل WebSocket (SadWebSocket):                            ║
 * ║     • بروتوكول WebSocket (RFC 6455)                            ║
 * ║     • ثنائي الاتجاه — مثالي للمحادثة الفورية                  ║
 * ║     • يدعم النص والبيانات الثنائية                            ║
 * ║                                                                ║
 * ║  ٤. عميل HTTP (SadHttp):                                      ║
 * ║     • طلبات GET/POST/PUT/DELETE                                ║
 * ║     • رؤوس مخصصة وجسم الطلب                                  ║
 * ║     • تحليل الاستجابة                                         ║
 * ║                                                                ║
 * ║  مخطط تطبيق المحادثة:                                         ║
 * ║  ──────────────────────                                       ║
 * ║  تطبيق ص ←─ WebSocket ──→ خادم المحادثة                      ║
 * ║           ←─ HTTP ──→ التسجيل/الملفات                         ║
 * ║                                                                ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#ifndef SAD_ANDROID_NETWORK_H
#define SAD_ANDROID_NETWORK_H

#include "sad_android_memory.h"
#include "sad_android_string.h"
#include "sad_android_value.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — ثوابت الشبكة
 * ═══════════════════════════════════════════════════════════════════════════ */

/** @brief (AR) مهلة الاتصال الافتراضية (5 ثوانٍ) */
#define SAD_NET_CONNECT_TIMEOUT_MS   5000

/** @brief (AR) مهلة القراءة/الكتابة (30 ثانية) */
#define SAD_NET_IO_TIMEOUT_MS        30000

/** @brief (AR) حجم مخزن الاستقبال المؤقت */
#define SAD_NET_RECV_BUFFER_SIZE     8192

/** @brief (AR) أقصى طول لعنوان URL */
#define SAD_NET_MAX_URL_LENGTH       4096

/** @brief (AR) منفذ HTTPS الافتراضي */
#define SAD_NET_HTTPS_PORT           443

/** @brief (AR) منفذ HTTP الافتراضي */
#define SAD_NET_HTTP_PORT            80

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — حالة المقبس
 * ═══════════════════════════════════════════════════════════════════════════ */

typedef enum {
    SAD_SOCK_CLOSED      = 0,    /**< مغلق */
    SAD_SOCK_CONNECTING  = 1,    /**< قيد الاتصال */
    SAD_SOCK_CONNECTED   = 2,    /**< متصل */
    SAD_SOCK_ERROR       = 3,    /**< خطأ */
} SadSocketState;

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — مقبس TCP
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) مقبس TCP — اتصال شبكة أساسي
 *
 * يُغلّف مقبس POSIX (socket/connect/send/recv).
 * يدعم IPv4 و IPv6 تلقائياً.
 */
typedef struct SadSocket {
    int              fd;              /**< واصف الملف (file descriptor) */
    SadSocketState   state;           /**< حالة المقبس */
    SadString        host;            /**< اسم المُضيف */
    uint16_t         port;            /**< المنفذ */
    uint32_t         connect_timeout; /**< مهلة الاتصال (ms) */
    uint32_t         io_timeout;      /**< مهلة القراءة/الكتابة (ms) */
    int              last_error;      /**< آخر رمز خطأ (errno) */
} SadSocket;

/**
 * @brief (AR) إنشاء مقبس TCP جديد
 * @return مؤشر للمقبس، أو NULL عند الفشل
 */
SadSocket* sad_socket_new(void);

/**
 * @brief (AR) الاتصال بخادم
 * @param sock المقبس
 * @param host اسم المُضيف أو عنوان IP
 * @param port المنفذ
 * @return true إذا نجح الاتصال
 *
 * يُحلّل اسم DNS تلقائياً.
 * يستخدم getaddrinfo لدعم IPv6.
 */
bool sad_socket_connect(SadSocket* sock, const char* host, uint16_t port);

/**
 * @brief (AR) إرسال بيانات
 * @param sock المقبس
 * @param data البيانات
 * @param length طول البيانات
 * @return عدد البايتات المُرسلة، أو -1 عند الخطأ
 */
int64_t sad_socket_send(SadSocket* sock, const void* data, size_t length);

/**
 * @brief (AR) استقبال بيانات
 * @param sock المقبس
 * @param buffer المخزن
 * @param buffer_size حجم المخزن
 * @return عدد البايتات المُستقبلة، 0 = انتهى الاتصال، -1 = خطأ
 */
int64_t sad_socket_receive(SadSocket* sock, void* buffer, size_t buffer_size);

/**
 * @brief (AR) إرسال نص كامل (يُضمن إرسال كل البايتات)
 */
bool sad_socket_send_all(SadSocket* sock, const void* data, size_t length);

/**
 * @brief (AR) إغلاق المقبس
 */
void sad_socket_close(SadSocket* sock);

/**
 * @brief (AR) تحرير المقبس
 */
void sad_socket_free(SadSocket* sock);

/** @brief (AR) هل المقبس متصل؟ */
bool sad_socket_is_connected(const SadSocket* sock);

/** @brief (AR) تعيين مهلة الاتصال */
void sad_socket_set_connect_timeout(SadSocket* sock, uint32_t ms);

/** @brief (AR) تعيين مهلة القراءة/الكتابة */
void sad_socket_set_io_timeout(SadSocket* sock, uint32_t ms);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — مقبس TLS (مُشفّر)
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * TLS (Transport Layer Security) يُشفّر الاتصال.
 * أندرويد يتضمن BoringSSL (فرع Google من OpenSSL).
 *
 * المصافحة:
 *  ┌────────┐                   ┌────────┐
 *  │ العميل │──── ClientHello ─→│ الخادم │
 *  │        │←── ServerHello ───│        │
 *  │        │←── شهادة ─────────│        │
 *  │        │──── مفتاح مشترك ─→│        │
 *  │        │←── ✓ مُشفّر! ─────│        │
 *  └────────┘                   └────────┘
 */
typedef struct SadTlsSocket {
    SadSocket       base;             /**< المقبس الأساسي */
    void*           ssl_ctx;          /**< سياق SSL (SSL_CTX*) */
    void*           ssl;              /**< اتصال SSL (SSL*) */
    bool            verify_cert;      /**< هل نتحقق من الشهادة؟ */
} SadTlsSocket;

/**
 * @brief (AR) إنشاء مقبس TLS مُشفّر
 * @return مؤشر للمقبس، أو NULL
 */
SadTlsSocket* sad_tls_socket_new(void);

/**
 * @brief (AR) الاتصال بخادم عبر TLS
 */
bool sad_tls_connect(SadTlsSocket* sock, const char* host, uint16_t port);

/**
 * @brief (AR) إرسال بيانات مُشفّرة
 */
int64_t sad_tls_send(SadTlsSocket* sock, const void* data, size_t length);

/**
 * @brief (AR) استقبال بيانات مُشفّرة
 */
int64_t sad_tls_receive(SadTlsSocket* sock, void* buffer, size_t buffer_size);

/**
 * @brief (AR) إغلاق اتصال TLS
 */
void sad_tls_close(SadTlsSocket* sock);

/**
 * @brief (AR) تحرير مقبس TLS
 */
void sad_tls_free(SadTlsSocket* sock);

/** @brief (AR) تفعيل/تعطيل التحقق من الشهادة */
void sad_tls_set_verify(SadTlsSocket* sock, bool verify);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — عميل WebSocket
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * WebSocket يوفر اتصالاً ثنائي الاتجاه دائم.
 * مثالي لتطبيق المحادثة — رسائل فورية بدون إعادة اتصال.
 *
 * بروتوكول الإطار (Frame):
 * ┌──────┬──────┬──────────┬──────────┬─────────────┐
 * │ FIN  │ OpC  │ MASK     │ طول      │ البيانات    │
 * │ 1bit │ 4bit │ 1bit     │ 7+16/64  │ متغير      │
 * └──────┴──────┴──────────┴──────────┴─────────────┘
 *
 * أنواع الإطارات:
 *  0x1 = نص (UTF-8)
 *  0x2 = ثنائي (binary)
 *  0x8 = إغلاق
 *  0x9 = ping
 *  0xA = pong
 */

/** @brief (AR) حالة WebSocket */
typedef enum {
    SAD_WS_CLOSED       = 0,    /**< مغلق */
    SAD_WS_CONNECTING   = 1,    /**< قيد المصافحة */
    SAD_WS_OPEN         = 2,    /**< متصل ومفتوح */
    SAD_WS_CLOSING      = 3,    /**< قيد الإغلاق */
    SAD_WS_ERROR        = 4,    /**< خطأ */
} SadWebSocketState;

/** @brief (AR) نوع رسالة WebSocket */
typedef enum {
    SAD_WS_MSG_TEXT     = 0x1,  /**< رسالة نصية */
    SAD_WS_MSG_BINARY   = 0x2,  /**< بيانات ثنائية */
    SAD_WS_MSG_CLOSE    = 0x8,  /**< طلب إغلاق */
    SAD_WS_MSG_PING     = 0x9,  /**< ping — إبقاء الاتصال حياً */
    SAD_WS_MSG_PONG     = 0xA,  /**< pong — استجابة لـ ping */
} SadWebSocketMsgType;

/** @brief (AR) دالة استقبال رسائل WebSocket */
typedef void (*SadWebSocketOnMessage)(SadWebSocketMsgType type,
                                       const void* data, size_t length,
                                       void* context);

/** @brief (AR) دالة تغيير حالة WebSocket */
typedef void (*SadWebSocketOnStateChange)(SadWebSocketState state,
                                           void* context);

/**
 * @brief (AR) عميل WebSocket
 */
typedef struct SadWebSocket {
    SadTlsSocket*               tls;              /**< مقبس TLS (للاتصال الآمن) */
    SadSocket*                  plain;             /**< مقبس عادي (ws://) */
    SadWebSocketState           state;             /**< حالة الاتصال */
    SadString                   url;               /**< عنوان URL */
    SadString                   protocols;         /**< البروتوكولات الفرعية */
    SadWebSocketOnMessage       on_message;        /**< دالة استقبال */
    SadWebSocketOnStateChange   on_state_change;   /**< دالة تغيير حالة */
    void*                       user_context;      /**< سياق المستخدم */
    bool                        use_tls;           /**< هل نستخدم TLS؟ (wss://) */
} SadWebSocket;

/**
 * @brief (AR) إنشاء عميل WebSocket
 * @param url عنوان URL (ws:// أو wss://)
 * @return مؤشر لعميل WebSocket، أو NULL
 */
SadWebSocket* sad_websocket_new(const char* url);

/**
 * @brief (AR) الاتصال بخادم WebSocket
 * @param ws العميل
 * @return true إذا نجحت المصافحة
 *
 * الخطوات:
 *  ١. اتصال TCP (أو TLS إذا wss://)
 *  ٢. إرسال HTTP Upgrade
 *  ٣. استلام والتحقق من استجابة 101
 */
bool sad_websocket_connect(SadWebSocket* ws);

/**
 * @brief (AR) إرسال رسالة نصية
 * @param ws العميل
 * @param text النص
 * @return true إذا نجح الإرسال
 */
bool sad_websocket_send_text(SadWebSocket* ws, const char* text);

/**
 * @brief (AR) إرسال بيانات ثنائية
 */
bool sad_websocket_send_binary(SadWebSocket* ws, const void* data, size_t length);

/**
 * @brief (AR) إرسال ping
 */
bool sad_websocket_ping(SadWebSocket* ws);

/**
 * @brief (AR) استقبال رسالة واحدة (يُنتظر)
 * @param ws العميل
 * @param out_type مؤشر لنوع الرسالة
 * @param out_data مؤشر لبيانات الرسالة (يجب تحريرها)
 * @param out_length مؤشر لطول البيانات
 * @return true إذا استُقبلت رسالة
 */
bool sad_websocket_receive(SadWebSocket* ws, SadWebSocketMsgType* out_type,
                            void** out_data, size_t* out_length);

/**
 * @brief (AR) تعيين دالة استقبال الرسائل
 */
void sad_websocket_set_on_message(SadWebSocket* ws,
                                   SadWebSocketOnMessage callback,
                                   void* context);

/**
 * @brief (AR) تعيين دالة تغيير الحالة
 */
void sad_websocket_set_on_state_change(SadWebSocket* ws,
                                        SadWebSocketOnStateChange callback,
                                        void* context);

/**
 * @brief (AR) إغلاق اتصال WebSocket (بلطف)
 * @param ws العميل
 * @param code رمز الإغلاق (1000 = عادي)
 * @param reason سبب الإغلاق (اختياري)
 */
void sad_websocket_close(SadWebSocket* ws, uint16_t code, const char* reason);

/**
 * @brief (AR) تحرير عميل WebSocket
 */
void sad_websocket_free(SadWebSocket* ws);

/** @brief (AR) هل WebSocket متصل؟ */
bool sad_websocket_is_open(const SadWebSocket* ws);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — عميل HTTP
 * ═══════════════════════════════════════════════════════════════════════════ */

/** @brief (AR) طريقة HTTP */
typedef enum {
    SAD_HTTP_GET     = 0,
    SAD_HTTP_POST    = 1,
    SAD_HTTP_PUT     = 2,
    SAD_HTTP_DELETE  = 3,
    SAD_HTTP_PATCH   = 4,
    SAD_HTTP_HEAD    = 5,
} SadHttpMethod;

/**
 * @brief (AR) استجابة HTTP
 */
typedef struct SadHttpResponse {
    int             status_code;      /**< رمز الحالة (200, 404, ...) */
    SadString       status_text;      /**< نص الحالة ("OK", "Not Found") */
    SadMap*         headers;          /**< رؤوس الاستجابة */
    void*           body;             /**< جسم الاستجابة */
    size_t          body_length;      /**< طول الجسم */
    SadString       content_type;     /**< نوع المحتوى */
} SadHttpResponse;

/**
 * @brief (AR) إرسال طلب HTTP
 * @param method الطريقة (GET, POST, ...)
 * @param url العنوان الكامل
 * @param headers رؤوس إضافية (اختياري)
 * @param body جسم الطلب (اختياري)
 * @param body_length طول الجسم
 * @return مؤشر للاستجابة (يجب تحريرها)
 */
SadHttpResponse* sad_http_request(SadHttpMethod method, const char* url,
                                   SadMap* headers,
                                   const void* body, size_t body_length);

/**
 * @brief (AR) اختصار: طلب GET
 */
SadHttpResponse* sad_http_get(const char* url);

/**
 * @brief (AR) اختصار: طلب POST مع JSON
 */
SadHttpResponse* sad_http_post_json(const char* url, const char* json);

/**
 * @brief (AR) الحصول على جسم الاستجابة كنص
 */
SadString sad_http_response_text(const SadHttpResponse* response);

/**
 * @brief (AR) تحرير الاستجابة
 */
void sad_http_response_free(SadHttpResponse* response);

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٧ — أدوات DNS
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) تحليل اسم DNS لعنوان IP
 * @param hostname اسم المُضيف
 * @param out_ip مؤشر لتخزين عنوان IP (نص)
 * @return true إذا نجح التحليل
 */
bool sad_dns_resolve(const char* hostname, SadString* out_ip);

#ifdef __cplusplus
}
#endif

#endif /* SAD_ANDROID_NETWORK_H */
