/**
 * @file sad_android_network.c
 * @brief (AR) تطبيق نظام الشبكات — TCP, TLS, WebSocket, HTTP
 * @brief (EN) Network system implementation — TCP, TLS, WebSocket, HTTP
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                    تطبيق الشبكات — لغة ص لأندرويد
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يستخدم مقابس POSIX (socket API) المدعومة مباشرة في bionic.
 * TLS يستخدم واجهة BoringSSL/OpenSSL المتوفرة في أندرويد.
 *
 * @author فريق تطوير لغة ص
 * @version 1.0.0
 */

#include "sad_android_network.h"
#include <string.h>       /* memset, memcpy, strlen */
#include <stdio.h>        /* snprintf */
#include <errno.h>        /* errno */
#include <unistd.h>       /* close */
#include <fcntl.h>        /* fcntl, O_NONBLOCK */
#include <sys/socket.h>   /* socket, connect, send, recv */
#include <sys/select.h>   /* select, fd_set */
#include <netinet/in.h>   /* sockaddr_in, sockaddr_in6 */
#include <netdb.h>        /* getaddrinfo, freeaddrinfo */
#include <arpa/inet.h>    /* inet_ntop */

/*
 * ملاحظة مهمة عن TLS/SSL:
 * ─────────────────────
 * أندرويد يتضمن BoringSSL (libssl.so, libcrypto.so) كجزء من النظام.
 * نستخدم الرؤوس القياسية <openssl/ssl.h> التي يوفرها NDK.
 *
 * في البناء المتقاطع (cross-compilation)، نربط مع:
 *   -lssl -lcrypto
 *
 * على الأنظمة التي لا يتوفر فيها OpenSSL (بيئة الاختبار على الحاسوب)،
 * نُعطّل ميزات TLS مع رسائل تحذيرية.
 */

/* اكتشاف توفر OpenSSL/BoringSSL */
#if defined(__ANDROID__) || defined(HAVE_OPENSSL)
  #include <openssl/ssl.h>
  #include <openssl/err.h>
  #define SAD_HAS_TLS 1
#else
  #define SAD_HAS_TLS 0
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §١ — أدوات داخلية
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) تعيين المقبس كغير حاجز (non-blocking)
 *
 * مهم للاتصال المُهيّأ (مع مهلة):
 *  ١. اجعل المقبس non-blocking
 *  ٢. ابدأ connect() — يرجع فوراً
 *  ٣. استخدم select() لانتظار النتيجة مع مهلة
 *  ٤. أعد المقبس لـ blocking
 */
static bool set_nonblocking(int fd, bool nonblocking) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return false;

    if (nonblocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    return fcntl(fd, F_SETFL, flags) == 0;
}

/**
 * @brief (AR) تعيين مهلة القراءة/الكتابة على المقبس
 */
static void set_socket_timeout(int fd, uint32_t timeout_ms) {
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٢ — مقبس TCP
 * ═══════════════════════════════════════════════════════════════════════════ */

SadSocket* sad_socket_new(void) {
    SadSocket* sock = (SadSocket*)sad_rc_alloc(sizeof(SadSocket),
                                                SAD_OBJ_NONE, NULL);
    if (!sock) return NULL;

    sock->fd = -1;
    sock->state = SAD_SOCK_CLOSED;
    sock->host = sad_string_empty();
    sock->port = 0;
    sock->connect_timeout = SAD_NET_CONNECT_TIMEOUT_MS;
    sock->io_timeout = SAD_NET_IO_TIMEOUT_MS;
    sock->last_error = 0;

    return sock;
}

bool sad_socket_connect(SadSocket* sock, const char* host, uint16_t port) {
    if (!sock || !host) return false;
    if (sock->state == SAD_SOCK_CONNECTED) return true;

    sock->state = SAD_SOCK_CONNECTING;
    sad_string_free(&sock->host);
    sock->host = sad_string_from_cstr(host);
    sock->port = port;

    /*
     * استخدام getaddrinfo — يتعامل مع:
     *  • أسماء DNS (مثال: "chat.example.com")
     *  • عناوين IPv4 (مثال: "192.168.1.1")
     *  • عناوين IPv6 (مثال: "::1")
     */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      /* IPv4 أو IPv6 */
    hints.ai_socktype = SOCK_STREAM;  /* TCP */

    char port_str[8];
    snprintf(port_str, sizeof(port_str), "%u", port);

    struct addrinfo* result = NULL;
    int gai_err = getaddrinfo(host, port_str, &hints, &result);
    if (gai_err != 0 || !result) {
        sock->state = SAD_SOCK_ERROR;
        return false;
    }

    /* جرّب كل عنوان حتى ينجح أحدها */
    bool connected = false;
    for (struct addrinfo* addr = result; addr != NULL; addr = addr->ai_next) {
        sock->fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock->fd < 0) continue;

        /* اتصال مع مهلة */
        set_nonblocking(sock->fd, true);
        int conn_result = connect(sock->fd, addr->ai_addr, addr->ai_addrlen);

        if (conn_result == 0) {
            /* اتصال فوري (نادر) */
            connected = true;
        } else if (errno == EINPROGRESS) {
            /* الاتصال قيد التقدم — انتظر مع مهلة */
            fd_set write_fds;
            FD_ZERO(&write_fds);
            FD_SET(sock->fd, &write_fds);

            struct timeval timeout;
            timeout.tv_sec = sock->connect_timeout / 1000;
            timeout.tv_usec = (sock->connect_timeout % 1000) * 1000;

            int sel_result = select(sock->fd + 1, NULL, &write_fds, NULL, &timeout);
            if (sel_result > 0) {
                /* تحقق من نجاح الاتصال */
                int so_error = 0;
                socklen_t len = sizeof(so_error);
                getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, &so_error, &len);
                if (so_error == 0) {
                    connected = true;
                }
            }
        }

        if (connected) {
            set_nonblocking(sock->fd, false);
            set_socket_timeout(sock->fd, sock->io_timeout);
            break;
        }

        /* فشل — أغلق وجرّب العنوان التالي */
        close(sock->fd);
        sock->fd = -1;
    }

    freeaddrinfo(result);

    if (connected) {
        sock->state = SAD_SOCK_CONNECTED;
    } else {
        sock->state = SAD_SOCK_ERROR;
    }

    return connected;
}

int64_t sad_socket_send(SadSocket* sock, const void* data, size_t length) {
    if (!sock || sock->fd < 0 || !data || length == 0) return -1;

    ssize_t sent = send(sock->fd, data, length, MSG_NOSIGNAL);
    if (sent < 0) {
        sock->last_error = errno;
        return -1;
    }

    return (int64_t)sent;
}

int64_t sad_socket_receive(SadSocket* sock, void* buffer, size_t buffer_size) {
    if (!sock || sock->fd < 0 || !buffer || buffer_size == 0) return -1;

    ssize_t received = recv(sock->fd, buffer, buffer_size, 0);
    if (received < 0) {
        sock->last_error = errno;
        return -1;
    }

    return (int64_t)received; /* 0 = الطرف الآخر أغلق الاتصال */
}

bool sad_socket_send_all(SadSocket* sock, const void* data, size_t length) {
    /*
     * ضمان إرسال كل البايتات:
     * send() قد لا يُرسل كل شيء دفعة واحدة
     * (خاصة مع بيانات كبيرة أو شبكة مزدحمة).
     */
    const uint8_t* ptr = (const uint8_t*)data;
    size_t remaining = length;

    while (remaining > 0) {
        int64_t sent = sad_socket_send(sock, ptr, remaining);
        if (sent <= 0) return false;

        ptr += sent;
        remaining -= (size_t)sent;
    }

    return true;
}

void sad_socket_close(SadSocket* sock) {
    if (!sock) return;

    if (sock->fd >= 0) {
        shutdown(sock->fd, SHUT_RDWR);
        close(sock->fd);
        sock->fd = -1;
    }

    sock->state = SAD_SOCK_CLOSED;
}

void sad_socket_free(SadSocket* sock) {
    if (!sock) return;

    sad_socket_close(sock);
    sad_string_free(&sock->host);
    sad_release(sock);
}

bool sad_socket_is_connected(const SadSocket* sock) {
    return sock && sock->state == SAD_SOCK_CONNECTED && sock->fd >= 0;
}

void sad_socket_set_connect_timeout(SadSocket* sock, uint32_t ms) {
    if (sock) sock->connect_timeout = ms;
}

void sad_socket_set_io_timeout(SadSocket* sock, uint32_t ms) {
    if (sock) {
        sock->io_timeout = ms;
        if (sock->fd >= 0) {
            set_socket_timeout(sock->fd, ms);
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٣ — مقبس TLS
 * ═══════════════════════════════════════════════════════════════════════════ */

SadTlsSocket* sad_tls_socket_new(void) {
    SadTlsSocket* sock = (SadTlsSocket*)sad_rc_alloc(sizeof(SadTlsSocket),
                                                       SAD_OBJ_NONE, NULL);
    if (!sock) return NULL;

    sock->base.fd = -1;
    sock->base.state = SAD_SOCK_CLOSED;
    sock->base.host = sad_string_empty();
    sock->base.port = 0;
    sock->base.connect_timeout = SAD_NET_CONNECT_TIMEOUT_MS;
    sock->base.io_timeout = SAD_NET_IO_TIMEOUT_MS;
    sock->base.last_error = 0;
    sock->ssl_ctx = NULL;
    sock->ssl = NULL;
    sock->verify_cert = true;

    return sock;
}

bool sad_tls_connect(SadTlsSocket* sock, const char* host, uint16_t port) {
    if (!sock || !host) return false;

#if SAD_HAS_TLS
    /* ١. اتصال TCP عادي */
    if (!sad_socket_connect(&sock->base, host, port)) {
        return false;
    }

    /* ٢. إنشاء سياق SSL */
    sock->ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!sock->ssl_ctx) {
        sad_socket_close(&sock->base);
        return false;
    }

    /* إعداد التحقق من الشهادة */
    if (sock->verify_cert) {
        SSL_CTX_set_default_verify_paths(sock->ssl_ctx);
        SSL_CTX_set_verify(sock->ssl_ctx, SSL_VERIFY_PEER, NULL);
    }

    /* ٣. إنشاء اتصال SSL */
    sock->ssl = SSL_new(sock->ssl_ctx);
    if (!sock->ssl) {
        SSL_CTX_free(sock->ssl_ctx);
        sock->ssl_ctx = NULL;
        sad_socket_close(&sock->base);
        return false;
    }

    /* ربط SSL بالمقبس */
    SSL_set_fd(sock->ssl, sock->base.fd);

    /* SNI — ضروري لأن كثيراً من الخوادم تستضيف نطاقات متعددة */
    SSL_set_tlsext_host_name(sock->ssl, host);

    /* ٤. مصافحة TLS */
    int ssl_result = SSL_connect(sock->ssl);
    if (ssl_result != 1) {
        SSL_free(sock->ssl);
        sock->ssl = NULL;
        SSL_CTX_free(sock->ssl_ctx);
        sock->ssl_ctx = NULL;
        sad_socket_close(&sock->base);
        return false;
    }

    return true;

#else
    /* TLS غير متوفر */
    (void)port;
    sock->base.state = SAD_SOCK_ERROR;
    return false;
#endif
}

int64_t sad_tls_send(SadTlsSocket* sock, const void* data, size_t length) {
#if SAD_HAS_TLS
    if (!sock || !sock->ssl || !data) return -1;
    int sent = SSL_write(sock->ssl, data, (int)length);
    return sent > 0 ? (int64_t)sent : -1;
#else
    (void)sock; (void)data; (void)length;
    return -1;
#endif
}

int64_t sad_tls_receive(SadTlsSocket* sock, void* buffer, size_t buffer_size) {
#if SAD_HAS_TLS
    if (!sock || !sock->ssl || !buffer) return -1;
    int received = SSL_read(sock->ssl, buffer, (int)buffer_size);
    return received > 0 ? (int64_t)received : (received == 0 ? 0 : -1);
#else
    (void)sock; (void)buffer; (void)buffer_size;
    return -1;
#endif
}

void sad_tls_close(SadTlsSocket* sock) {
    if (!sock) return;

#if SAD_HAS_TLS
    if (sock->ssl) {
        SSL_shutdown(sock->ssl);
        SSL_free(sock->ssl);
        sock->ssl = NULL;
    }
    if (sock->ssl_ctx) {
        SSL_CTX_free(sock->ssl_ctx);
        sock->ssl_ctx = NULL;
    }
#endif

    sad_socket_close(&sock->base);
}

void sad_tls_free(SadTlsSocket* sock) {
    if (!sock) return;
    sad_tls_close(sock);
    sad_string_free(&sock->base.host);
    sad_release(sock);
}

void sad_tls_set_verify(SadTlsSocket* sock, bool verify) {
    if (sock) sock->verify_cert = verify;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٤ — عميل WebSocket
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief (AR) تحليل عنوان WebSocket URL
 *
 *  ws://host:port/path  → TLS=false, port=80
 *  wss://host:port/path → TLS=true, port=443
 */
static bool parse_ws_url(const char* url, char* host, size_t host_len,
                          uint16_t* port, char* path, size_t path_len,
                          bool* use_tls) {
    if (!url || !host || !port || !path || !use_tls) return false;

    *use_tls = false;
    *port = 80;

    const char* p = url;

    if (strncmp(p, "wss://", 6) == 0) {
        *use_tls = true;
        *port = 443;
        p += 6;
    } else if (strncmp(p, "ws://", 5) == 0) {
        p += 5;
    } else {
        return false;
    }

    /* استخراج host */
    const char* host_end = p;
    while (*host_end && *host_end != ':' && *host_end != '/') {
        host_end++;
    }

    size_t hlen = host_end - p;
    if (hlen >= host_len) hlen = host_len - 1;
    memcpy(host, p, hlen);
    host[hlen] = '\0';

    p = host_end;

    /* استخراج port (اختياري) */
    if (*p == ':') {
        p++;
        *port = (uint16_t)atoi(p);
        while (*p && *p != '/') p++;
    }

    /* استخراج path */
    if (*p == '/') {
        size_t plen = strlen(p);
        if (plen >= path_len) plen = path_len - 1;
        memcpy(path, p, plen);
        path[plen] = '\0';
    } else {
        path[0] = '/';
        path[1] = '\0';
    }

    return true;
}

SadWebSocket* sad_websocket_new(const char* url) {
    if (!url) return NULL;

    SadWebSocket* ws = (SadWebSocket*)sad_rc_alloc(sizeof(SadWebSocket),
                                                     SAD_OBJ_NONE, NULL);
    if (!ws) return NULL;

    memset(ws, 0, sizeof(SadWebSocket));
    ws->url = sad_string_from_cstr(url);
    ws->protocols = sad_string_empty();
    ws->state = SAD_WS_CLOSED;
    ws->on_message = NULL;
    ws->on_state_change = NULL;
    ws->user_context = NULL;

    /* تحديد نوع الاتصال */
    ws->use_tls = (strncmp(url, "wss://", 6) == 0);

    return ws;
}

bool sad_websocket_connect(SadWebSocket* ws) {
    if (!ws) return false;

    char host[256], path[1024];
    uint16_t port;
    bool use_tls;

    if (!parse_ws_url(sad_string_data(&ws->url), host, sizeof(host),
                       &port, path, sizeof(path), &use_tls)) {
        ws->state = SAD_WS_ERROR;
        return false;
    }

    ws->state = SAD_WS_CONNECTING;

    /* اتصال TCP أو TLS */
    bool connected;
    if (use_tls) {
        ws->tls = sad_tls_socket_new();
        if (!ws->tls) {
            ws->state = SAD_WS_ERROR;
            return false;
        }
        connected = sad_tls_connect(ws->tls, host, port);
    } else {
        ws->plain = sad_socket_new();
        if (!ws->plain) {
            ws->state = SAD_WS_ERROR;
            return false;
        }
        connected = sad_socket_connect(ws->plain, host, port);
    }

    if (!connected) {
        ws->state = SAD_WS_ERROR;
        return false;
    }

    /*
     * مصافحة WebSocket — HTTP Upgrade:
     *
     *  GET /path HTTP/1.1\r\n
     *  Host: host\r\n
     *  Upgrade: websocket\r\n
     *  Connection: Upgrade\r\n
     *  Sec-WebSocket-Key: base64-random-key\r\n
     *  Sec-WebSocket-Version: 13\r\n
     *  \r\n
     *
     * الخادم يجب أن يرد بـ:
     *  HTTP/1.1 101 Switching Protocols\r\n
     */
    char request[2048];
    /* مفتاح WebSocket ثابت (للتبسيط — في الإنتاج يجب أن يكون عشوائي) */
    const char* ws_key = "dGhlIHNhbXBsZSBub25jZQ==";

    int req_len = snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s:%u\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        path, host, port, ws_key);

    /* إرسال المصافحة */
    bool sent;
    if (use_tls) {
        sent = (sad_tls_send(ws->tls, request, (size_t)req_len) > 0);
    } else {
        sent = sad_socket_send_all(ws->plain, request, (size_t)req_len);
    }

    if (!sent) {
        ws->state = SAD_WS_ERROR;
        return false;
    }

    /* استقبال الاستجابة */
    char response[4096];
    int64_t recv_len;
    if (use_tls) {
        recv_len = sad_tls_receive(ws->tls, response, sizeof(response) - 1);
    } else {
        recv_len = sad_socket_receive(ws->plain, response, sizeof(response) - 1);
    }

    if (recv_len <= 0) {
        ws->state = SAD_WS_ERROR;
        return false;
    }

    response[recv_len] = '\0';

    /* التحقق من "101 Switching Protocols" */
    if (strstr(response, "101") == NULL) {
        ws->state = SAD_WS_ERROR;
        return false;
    }

    ws->state = SAD_WS_OPEN;

    if (ws->on_state_change) {
        ws->on_state_change(SAD_WS_OPEN, ws->user_context);
    }

    return true;
}

bool sad_websocket_send_text(SadWebSocket* ws, const char* text) {
    if (!ws || !text || ws->state != SAD_WS_OPEN) return false;

    size_t text_len = strlen(text);

    /*
     * بناء إطار WebSocket:
     *
     * بايت ١: FIN(1) + RSV(000) + Opcode(0001 = نص)
     *       = 0x81
     *
     * بايت ٢: MASK(1) + طول
     *   إذا طول ≤ 125: بايت واحد
     *   إذا طول ≤ 65535: 126 + 2 بايت
     *   إذا أكبر: 127 + 8 بايت
     *
     * ٤ بايت: مفتاح القناع (masking key)
     * ثم البيانات المُقنّعة
     */
    uint8_t frame[14]; /* أقصى حجم رأس */
    size_t frame_len = 0;

    frame[0] = 0x81; /* FIN + TEXT */
    frame_len = 1;

    if (text_len <= 125) {
        frame[1] = 0x80 | (uint8_t)text_len; /* MASK + length */
        frame_len = 2;
    } else if (text_len <= 65535) {
        frame[1] = 0x80 | 126;
        frame[2] = (uint8_t)(text_len >> 8);
        frame[3] = (uint8_t)(text_len & 0xFF);
        frame_len = 4;
    } else {
        frame[1] = 0x80 | 127;
        for (int i = 0; i < 8; i++) {
            frame[2 + i] = (uint8_t)(text_len >> ((7 - i) * 8));
        }
        frame_len = 10;
    }

    /* مفتاح قناع بسيط (في الإنتاج يجب أن يكون عشوائي) */
    uint8_t mask[4] = {0x12, 0x34, 0x56, 0x78};
    memcpy(frame + frame_len, mask, 4);
    frame_len += 4;

    /* إرسال الرأس */
    bool ok;
    if (ws->use_tls) {
        ok = (sad_tls_send(ws->tls, frame, frame_len) > 0);
    } else {
        ok = sad_socket_send_all(ws->plain, frame, frame_len);
    }
    if (!ok) return false;

    /* إرسال البيانات المُقنّعة */
    char* masked = (char*)sad_alloc(text_len);
    if (!masked) return false;

    for (size_t i = 0; i < text_len; i++) {
        masked[i] = text[i] ^ mask[i % 4];
    }

    if (ws->use_tls) {
        ok = (sad_tls_send(ws->tls, masked, text_len) > 0);
    } else {
        ok = sad_socket_send_all(ws->plain, masked, text_len);
    }

    sad_free(masked);
    return ok;
}

bool sad_websocket_send_binary(SadWebSocket* ws, const void* data, size_t length) {
    /* مشابه لـ send_text لكن مع opcode 0x02 (binary) */
    if (!ws || !data || ws->state != SAD_WS_OPEN) return false;

    uint8_t frame[14];
    size_t frame_len = 0;

    frame[0] = 0x82; /* FIN + BINARY */
    frame_len = 1;

    if (length <= 125) {
        frame[1] = 0x80 | (uint8_t)length;
        frame_len = 2;
    } else if (length <= 65535) {
        frame[1] = 0x80 | 126;
        frame[2] = (uint8_t)(length >> 8);
        frame[3] = (uint8_t)(length & 0xFF);
        frame_len = 4;
    } else {
        frame[1] = 0x80 | 127;
        for (int i = 0; i < 8; i++) {
            frame[2 + i] = (uint8_t)(length >> ((7 - i) * 8));
        }
        frame_len = 10;
    }

    uint8_t mask[4] = {0xAB, 0xCD, 0xEF, 0x01};
    memcpy(frame + frame_len, mask, 4);
    frame_len += 4;

    bool ok;
    if (ws->use_tls) {
        ok = (sad_tls_send(ws->tls, frame, frame_len) > 0);
    } else {
        ok = sad_socket_send_all(ws->plain, frame, frame_len);
    }
    if (!ok) return false;

    uint8_t* masked = (uint8_t*)sad_alloc(length);
    if (!masked) return false;

    const uint8_t* src = (const uint8_t*)data;
    for (size_t i = 0; i < length; i++) {
        masked[i] = src[i] ^ mask[i % 4];
    }

    if (ws->use_tls) {
        ok = (sad_tls_send(ws->tls, masked, length) > 0);
    } else {
        ok = sad_socket_send_all(ws->plain, masked, length);
    }

    sad_free(masked);
    return ok;
}

bool sad_websocket_ping(SadWebSocket* ws) {
    if (!ws || ws->state != SAD_WS_OPEN) return false;

    uint8_t frame[2] = {0x89, 0x80}; /* FIN + PING, MASK + 0 length */
    uint8_t mask[4] = {0, 0, 0, 0};

    /* إرسال رأس + قناع فارغ */
    uint8_t buf[6];
    memcpy(buf, frame, 2);
    memcpy(buf + 2, mask, 4);

    if (ws->use_tls) {
        return (sad_tls_send(ws->tls, buf, 6) > 0);
    }
    return sad_socket_send_all(ws->plain, buf, 6);
}

bool sad_websocket_receive(SadWebSocket* ws, SadWebSocketMsgType* out_type,
                            void** out_data, size_t* out_length) {
    if (!ws || ws->state != SAD_WS_OPEN) return false;

    /*
     * قراءة إطار WebSocket من الخادم:
     *
     * الخادم لا يستخدم القناع (masking) — فقط العميل يُقنّع.
     * بايت ١: FIN + Opcode
     * بايت ٢: طول (بدون mask bit)
     */
    uint8_t header[2];
    int64_t recv_len;

    if (ws->use_tls) {
        recv_len = sad_tls_receive(ws->tls, header, 2);
    } else {
        recv_len = sad_socket_receive(ws->plain, header, 2);
    }

    if (recv_len != 2) return false;

    uint8_t opcode = header[0] & 0x0F;
    bool masked = (header[1] & 0x80) != 0;
    uint64_t payload_len = header[1] & 0x7F;

    /* قراءة الطول الموسّع */
    if (payload_len == 126) {
        uint8_t ext[2];
        if (ws->use_tls) {
            recv_len = sad_tls_receive(ws->tls, ext, 2);
        } else {
            recv_len = sad_socket_receive(ws->plain, ext, 2);
        }
        if (recv_len != 2) return false;
        payload_len = ((uint64_t)ext[0] << 8) | ext[1];
    } else if (payload_len == 127) {
        uint8_t ext[8];
        if (ws->use_tls) {
            recv_len = sad_tls_receive(ws->tls, ext, 8);
        } else {
            recv_len = sad_socket_receive(ws->plain, ext, 8);
        }
        if (recv_len != 8) return false;
        payload_len = 0;
        for (int i = 0; i < 8; i++) {
            payload_len = (payload_len << 8) | ext[i];
        }
    }

    /* قراءة مفتاح القناع (إذا وُجد) */
    uint8_t mask_key[4] = {0};
    if (masked) {
        if (ws->use_tls) {
            recv_len = sad_tls_receive(ws->tls, mask_key, 4);
        } else {
            recv_len = sad_socket_receive(ws->plain, mask_key, 4);
        }
        if (recv_len != 4) return false;
    }

    /* قراءة البيانات */
    void* payload = NULL;
    if (payload_len > 0) {
        payload = sad_alloc((size_t)payload_len + 1);
        if (!payload) return false;

        size_t total_read = 0;
        while (total_read < payload_len) {
            if (ws->use_tls) {
                recv_len = sad_tls_receive(ws->tls,
                    (uint8_t*)payload + total_read,
                    (size_t)(payload_len - total_read));
            } else {
                recv_len = sad_socket_receive(ws->plain,
                    (uint8_t*)payload + total_read,
                    (size_t)(payload_len - total_read));
            }
            if (recv_len <= 0) {
                sad_free(payload);
                return false;
            }
            total_read += (size_t)recv_len;
        }

        /* فك القناع */
        if (masked) {
            uint8_t* p = (uint8_t*)payload;
            for (size_t i = 0; i < payload_len; i++) {
                p[i] ^= mask_key[i % 4];
            }
        }

        ((char*)payload)[payload_len] = '\0'; /* null-terminate للنصوص */
    }

    /* معالجة أنواع الإطارات الخاصة */
    if (opcode == 0x09) {
        /* ping — أرسل pong تلقائياً */
        /* TODO: إرسال pong مع نفس البيانات */
        if (payload) sad_free(payload);
        if (out_type) *out_type = SAD_WS_MSG_PING;
        if (out_data) *out_data = NULL;
        if (out_length) *out_length = 0;
        return true;
    }

    if (opcode == 0x08) {
        /* إغلاق */
        ws->state = SAD_WS_CLOSED;
        if (ws->on_state_change) {
            ws->on_state_change(SAD_WS_CLOSED, ws->user_context);
        }
        if (payload) sad_free(payload);
        if (out_type) *out_type = SAD_WS_MSG_CLOSE;
        if (out_data) *out_data = NULL;
        if (out_length) *out_length = 0;
        return true;
    }

    /* رسالة عادية */
    if (out_type) *out_type = (SadWebSocketMsgType)opcode;
    if (out_data) *out_data = payload; else if (payload) sad_free(payload);
    if (out_length) *out_length = (size_t)payload_len;

    /* إشعار callback */
    if (ws->on_message && payload) {
        ws->on_message((SadWebSocketMsgType)opcode, payload,
                        (size_t)payload_len, ws->user_context);
    }

    return true;
}

void sad_websocket_set_on_message(SadWebSocket* ws,
                                   SadWebSocketOnMessage callback,
                                   void* context) {
    if (ws) {
        ws->on_message = callback;
        ws->user_context = context;
    }
}

void sad_websocket_set_on_state_change(SadWebSocket* ws,
                                        SadWebSocketOnStateChange callback,
                                        void* context) {
    if (ws) {
        ws->on_state_change = callback;
        ws->user_context = context;
    }
}

void sad_websocket_close(SadWebSocket* ws, uint16_t code, const char* reason) {
    if (!ws || ws->state != SAD_WS_OPEN) return;

    ws->state = SAD_WS_CLOSING;

    /* إرسال إطار إغلاق */
    size_t reason_len = reason ? strlen(reason) : 0;
    size_t payload_len = 2 + reason_len; /* 2 بايت لرمز الإغلاق */

    uint8_t frame[4];
    frame[0] = 0x88; /* FIN + CLOSE */
    frame[1] = 0x80 | (uint8_t)(payload_len > 125 ? 125 : payload_len);
    /* ملاحظة: لتبسيط الكود، نُحدد طول الإغلاق ≤ 125 */

    uint8_t mask[4] = {0x55, 0xAA, 0x55, 0xAA};

    /* جسم الإغلاق: رمز + سبب */
    uint8_t close_data[128];
    close_data[0] = (uint8_t)(code >> 8) ^ mask[0];
    close_data[1] = (uint8_t)(code & 0xFF) ^ mask[1];
    for (size_t i = 0; i < reason_len && i < 123; i++) {
        close_data[2 + i] = (uint8_t)reason[i] ^ mask[(2 + i) % 4];
    }

    /* إرسال */
    uint8_t buf[132];
    memcpy(buf, frame, 2);
    memcpy(buf + 2, mask, 4);
    size_t total = 6;
    if (payload_len > 0) {
        size_t copy_len = payload_len > 125 ? 125 : payload_len;
        memcpy(buf + 6, close_data, copy_len);
        total += copy_len;
    }

    if (ws->use_tls && ws->tls) {
        sad_tls_send(ws->tls, buf, total);
    } else if (ws->plain) {
        sad_socket_send_all(ws->plain, buf, total);
    }

    /* إغلاق الاتصال */
    if (ws->use_tls && ws->tls) {
        sad_tls_close(ws->tls);
    } else if (ws->plain) {
        sad_socket_close(ws->plain);
    }

    ws->state = SAD_WS_CLOSED;

    if (ws->on_state_change) {
        ws->on_state_change(SAD_WS_CLOSED, ws->user_context);
    }
}

void sad_websocket_free(SadWebSocket* ws) {
    if (!ws) return;

    if (ws->state == SAD_WS_OPEN) {
        sad_websocket_close(ws, 1000, NULL);
    }

    if (ws->tls) sad_tls_free(ws->tls);
    if (ws->plain) sad_socket_free(ws->plain);

    sad_string_free(&ws->url);
    sad_string_free(&ws->protocols);

    sad_release(ws);
}

bool sad_websocket_is_open(const SadWebSocket* ws) {
    return ws && ws->state == SAD_WS_OPEN;
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٥ — عميل HTTP
 * ═══════════════════════════════════════════════════════════════════════════ */

static const char* http_method_str(SadHttpMethod method) {
    switch (method) {
        case SAD_HTTP_GET:    return "GET";
        case SAD_HTTP_POST:   return "POST";
        case SAD_HTTP_PUT:    return "PUT";
        case SAD_HTTP_DELETE: return "DELETE";
        case SAD_HTTP_PATCH:  return "PATCH";
        case SAD_HTTP_HEAD:   return "HEAD";
        default:              return "GET";
    }
}

SadHttpResponse* sad_http_request(SadHttpMethod method, const char* url,
                                   SadMap* headers,
                                   const void* body, size_t body_length) {
    if (!url) return NULL;

    /* تحليل URL بشكل مبسّط */
    bool use_tls = (strncmp(url, "https://", 8) == 0);
    const char* host_start = url + (use_tls ? 8 : 7);
    const char* path_start = strchr(host_start, '/');
    const char* path = path_start ? path_start : "/";

    char host[256];
    size_t host_len;
    if (path_start) {
        host_len = path_start - host_start;
    } else {
        host_len = strlen(host_start);
    }
    if (host_len >= sizeof(host)) host_len = sizeof(host) - 1;
    memcpy(host, host_start, host_len);
    host[host_len] = '\0';

    /* استخراج المنفذ */
    uint16_t port = use_tls ? SAD_NET_HTTPS_PORT : SAD_NET_HTTP_PORT;
    char* colon = strchr(host, ':');
    if (colon) {
        *colon = '\0';
        port = (uint16_t)atoi(colon + 1);
    }

    /* بناء الطلب */
    char request[4096];
    int req_len = snprintf(request, sizeof(request),
        "%s %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "Content-Length: %zu\r\n",
        http_method_str(method), path, host, body_length);

    /* رؤوس إضافية */
    if (headers) {
        /* TODO: تكرار على الخريطة وإضافة رؤوس */
    }

    /* نهاية الرؤوس */
    req_len += snprintf(request + req_len, sizeof(request) - req_len, "\r\n");

    /* اتصال وإرسال */
    void* recv_buffer = sad_alloc(SAD_NET_RECV_BUFFER_SIZE * 4);
    if (!recv_buffer) return NULL;

    size_t total_received = 0;
    bool success = false;

    if (use_tls) {
        SadTlsSocket* tls = sad_tls_socket_new();
        if (tls && sad_tls_connect(tls, host, port)) {
            sad_tls_send(tls, request, (size_t)req_len);
            if (body && body_length > 0) {
                sad_tls_send(tls, body, body_length);
            }

            int64_t r;
            while ((r = sad_tls_receive(tls, (uint8_t*)recv_buffer + total_received,
                                         SAD_NET_RECV_BUFFER_SIZE * 4 - total_received)) > 0) {
                total_received += (size_t)r;
            }
            success = true;
        }
        if (tls) sad_tls_free(tls);
    } else {
        SadSocket* sock = sad_socket_new();
        if (sock && sad_socket_connect(sock, host, port)) {
            sad_socket_send_all(sock, request, (size_t)req_len);
            if (body && body_length > 0) {
                sad_socket_send_all(sock, body, body_length);
            }

            int64_t r;
            while ((r = sad_socket_receive(sock, (uint8_t*)recv_buffer + total_received,
                                            SAD_NET_RECV_BUFFER_SIZE * 4 - total_received)) > 0) {
                total_received += (size_t)r;
            }
            success = true;
        }
        if (sock) sad_socket_free(sock);
    }

    if (!success || total_received == 0) {
        sad_free(recv_buffer);
        return NULL;
    }

    /* تحليل الاستجابة */
    SadHttpResponse* response = (SadHttpResponse*)sad_alloc(sizeof(SadHttpResponse));
    if (!response) {
        sad_free(recv_buffer);
        return NULL;
    }

    memset(response, 0, sizeof(SadHttpResponse));

    char* resp_str = (char*)recv_buffer;
    resp_str[total_received] = '\0';

    /* تحليل سطر الحالة: "HTTP/1.1 200 OK\r\n" */
    if (strncmp(resp_str, "HTTP/", 5) == 0) {
        char* space1 = strchr(resp_str, ' ');
        if (space1) {
            response->status_code = atoi(space1 + 1);
            char* space2 = strchr(space1 + 1, ' ');
            if (space2) {
                char* eol = strstr(space2, "\r\n");
                if (eol) {
                    size_t st_len = eol - space2 - 1;
                    response->status_text = sad_string_new(space2 + 1, st_len);
                }
            }
        }
    }

    /* إيجاد جسم الاستجابة (بعد \r\n\r\n) */
    char* body_start = strstr(resp_str, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
        size_t body_len = total_received - (body_start - resp_str);
        response->body = sad_alloc(body_len + 1);
        if (response->body) {
            memcpy(response->body, body_start, body_len);
            ((char*)response->body)[body_len] = '\0';
            response->body_length = body_len;
        }
    }

    response->headers = sad_map_new();

    sad_free(recv_buffer);
    return response;
}

SadHttpResponse* sad_http_get(const char* url) {
    return sad_http_request(SAD_HTTP_GET, url, NULL, NULL, 0);
}

SadHttpResponse* sad_http_post_json(const char* url, const char* json) {
    SadMap* headers = sad_map_new();
    SadString ct_key = sad_string_from_cstr("Content-Type");
    sad_map_set(headers, &ct_key, sad_val_ptr(NULL, SAD_ELEM_STRING));
    sad_string_free(&ct_key);
    /* TODO: إضافة Content-Type: application/json بشكل صحيح */

    SadHttpResponse* response = sad_http_request(SAD_HTTP_POST, url, headers,
                                                  json, json ? strlen(json) : 0);
    sad_map_free(headers);
    return response;
}

SadString sad_http_response_text(const SadHttpResponse* response) {
    if (!response || !response->body) return sad_string_empty();
    return sad_string_new((const char*)response->body, response->body_length);
}

void sad_http_response_free(SadHttpResponse* response) {
    if (!response) return;

    sad_string_free(&response->status_text);
    sad_string_free(&response->content_type);
    if (response->headers) sad_map_free(response->headers);
    if (response->body) sad_free(response->body);

    sad_free(response);
}

/* ═══════════════════════════════════════════════════════════════════════════
 *                    §٦ — أدوات DNS
 * ═══════════════════════════════════════════════════════════════════════════ */

bool sad_dns_resolve(const char* hostname, SadString* out_ip) {
    if (!hostname || !out_ip) return false;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* result = NULL;
    int err = getaddrinfo(hostname, NULL, &hints, &result);
    if (err != 0 || !result) return false;

    char ip_str[INET6_ADDRSTRLEN];
    bool resolved = false;

    if (result->ai_family == AF_INET) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)result->ai_addr;
        inet_ntop(AF_INET, &ipv4->sin_addr, ip_str, sizeof(ip_str));
        resolved = true;
    } else if (result->ai_family == AF_INET6) {
        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)result->ai_addr;
        inet_ntop(AF_INET6, &ipv6->sin6_addr, ip_str, sizeof(ip_str));
        resolved = true;
    }

    freeaddrinfo(result);

    if (resolved) {
        *out_ip = sad_string_from_cstr(ip_str);
    }

    return resolved;
}
