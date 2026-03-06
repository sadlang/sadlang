/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ نظام الشبكة — HTTP عبر مقابس BSD
 *  Network System Implementation — HTTP via BSD sockets
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_network.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

// مقابس POSIX
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SadNetwork", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SadNetwork", __VA_ARGS__)
#else
#include <stdio.h>
#define LOGI(...) do { printf("[SadNetwork] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define LOGE(...) do { fprintf(stderr, "[SadNetwork] "); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  هيكل داخلي
// ═══════════════════════════════════════════════════════════════════════════════

#define MAX_DEFAULT_HEADERS 16
#define RECV_BUF_SIZE 8192
#define MAX_RESPONSE_SIZE (16 * 1024 * 1024) // 16 MB

typedef struct {
    char* مفتاح;
    char* قيمة;
} ترويسة_داخلية;

struct مقبض_شبكة {
    ترويسة_داخلية ترويسات_افتراضية[MAX_DEFAULT_HEADERS];
    int عدد_ترويسات_افتراضية;
    int مهلة_اتصال;  // ms
    int مهلة_قراءة;   // ms
};

// ═══════════════════════════════════════════════════════════════════════════════
//  مساعدات داخلية
// ═══════════════════════════════════════════════════════════════════════════════

// تحليل URL: استخراج المضيف والمنفذ والمسار
static bool parse_url(const char* url, char* host, int* port, char* path, bool* is_https) {
    *is_https = false;
    *port = 80;
    
    const char* p = url;
    if (strncmp(p, "https://", 8) == 0) {
        *is_https = true;
        *port = 443;
        p += 8;
    } else if (strncmp(p, "http://", 7) == 0) {
        p += 7;
    }
    
    // استخراج المضيف
    const char* slash = strchr(p, '/');
    const char* colon = strchr(p, ':');
    
    if (colon && (!slash || colon < slash)) {
        // منفذ مخصص
        int hostLen = (int)(colon - p);
        strncpy(host, p, hostLen);
        host[hostLen] = '\0';
        *port = atoi(colon + 1);
        p = slash ? slash : p + strlen(p);
    } else if (slash) {
        int hostLen = (int)(slash - p);
        strncpy(host, p, hostLen);
        host[hostLen] = '\0';
        p = slash;
    } else {
        strcpy(host, p);
        p = "";
    }
    
    if (p[0] == '/' || p[0] == '\0') {
        strcpy(path, p[0] ? p : "/");
    } else {
        strcpy(path, "/");
    }
    
    return host[0] != '\0';
}

// الاتصال بالخادم
static int connect_to_host(const char* host, int port, int timeout_ms) {
    struct addrinfo hints = {0}, *res = NULL;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    char portStr[8];
    snprintf(portStr, sizeof(portStr), "%d", port);
    
    int err = getaddrinfo(host, portStr, &hints, &res);
    if (err != 0 || !res) {
        LOGE("DNS failed for %s: %s", host, gai_strerror(err));
        return -1;
    }
    
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        freeaddrinfo(res);
        LOGE("Socket creation failed: %s", strerror(errno));
        return -1;
    }
    
    // وضع non-blocking للمهلة
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    
    int ret = connect(sock, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    
    if (ret < 0 && errno != EINPROGRESS) {
        close(sock);
        LOGE("Connect failed: %s", strerror(errno));
        return -1;
    }
    
    if (ret < 0) {
        // انتظار الاتصال مع مهلة
        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(sock, &writefds);
        
        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        
        ret = select(sock + 1, NULL, &writefds, NULL, &tv);
        if (ret <= 0) {
            close(sock);
            LOGE("Connection timeout to %s:%d", host, port);
            return -1;
        }
        
        int error = 0;
        socklen_t len = sizeof(error);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &error, &len);
        if (error) {
            close(sock);
            LOGE("Connect error: %s", strerror(error));
            return -1;
        }
    }
    
    // رجوع لوضع blocking
    fcntl(sock, F_SETFL, flags);
    
    return sock;
}

// إرسال بيانات كاملة
static bool send_all(int sock, const char* data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(sock, data + sent, len - sent, 0);
        if (n <= 0) return false;
        sent += n;
    }
    return true;
}

// استقبال HTTP response كاملة
static char* recv_http_response(int sock, int timeout_ms, size_t* out_len, int* out_status) {
    // تعيين مهلة القراءة
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    size_t capacity = RECV_BUF_SIZE;
    size_t total = 0;
    char* buf = malloc(capacity);
    if (!buf) return NULL;
    
    while (total < MAX_RESPONSE_SIZE) {
        if (total + RECV_BUF_SIZE > capacity) {
            capacity *= 2;
            char* newbuf = realloc(buf, capacity);
            if (!newbuf) { free(buf); return NULL; }
            buf = newbuf;
        }
        
        ssize_t n = recv(sock, buf + total, RECV_BUF_SIZE, 0);
        if (n <= 0) break;
        total += n;
    }
    
    buf[total] = '\0';
    *out_len = total;
    
    // استخراج status code
    *out_status = 0;
    if (total > 12 && strncmp(buf, "HTTP/", 5) == 0) {
        const char* sp = strchr(buf, ' ');
        if (sp) *out_status = atoi(sp + 1);
    }
    
    return buf;
}

// فصل الترويسات عن الجسم
static char* split_headers_body(char* raw, size_t raw_len, char** headers_out, size_t* body_len) {
    char* sep = strstr(raw, "\r\n\r\n");
    if (!sep) {
        *headers_out = NULL;
        *body_len = raw_len;
        return raw;
    }
    
    *sep = '\0';
    *headers_out = raw; // الترويسات
    
    char* body = sep + 4;
    *body_len = raw_len - (body - raw);
    return body;
}

// الطريقة كنص
static const char* method_string(طريقة_طلب m) {
    switch (m) {
        case طريقة_GET:     return "GET";
        case طريقة_POST:    return "POST";
        case طريقة_PUT:     return "PUT";
        case طريقة_DELETE:  return "DELETE";
        case طريقة_PATCH:   return "PATCH";
        case طريقة_HEAD:    return "HEAD";
        case طريقة_OPTIONS: return "OPTIONS";
        default:             return "GET";
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إنشاء وتدمير
// ═══════════════════════════════════════════════════════════════════════════════

مقبض_شبكة* أنشئ_شبكة(void) {
    مقبض_شبكة* ش = calloc(1, sizeof(مقبض_شبكة));
    if (!ش) return NULL;
    ش->مهلة_اتصال = 30000;
    ش->مهلة_قراءة = 30000;
    LOGI("تم إنشاء مقبض الشبكة");
    return ش;
}

void دمر_شبكة(مقبض_شبكة* الشبكة) {
    if (!الشبكة) return;
    for (int i = 0; i < الشبكة->عدد_ترويسات_افتراضية; i++) {
        free(الشبكة->ترويسات_افتراضية[i].مفتاح);
        free(الشبكة->ترويسات_افتراضية[i].قيمة);
    }
    free(الشبكة);
    LOGI("تم تدمير مقبض الشبكة");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تنفيذ الطلب
// ═══════════════════════════════════════════════════════════════════════════════

استجابة_شبكة أرسل_طلب_مخصص(مقبض_شبكة* الشبكة, const إعدادات_طلب* إ) {
    استجابة_شبكة رد = {0};
    
    if (!الشبكة || !إ || !إ->الرابط) {
        رد.الحالة = حالة_فشل_اتصال;
        رد.رسالة_الخطأ = strdup("معاملات غير صالحة");
        return رد;
    }
    
    // تحليل URL
    char host[256] = {0};
    char path[2048] = {0};
    int port = 80;
    bool is_https = false;
    
    if (!parse_url(إ->الرابط, host, &port, path, &is_https)) {
        رد.الحالة = حالة_خطأ_DNS;
        رد.رسالة_الخطأ = strdup("رابط غير صالح");
        return رد;
    }
    
    /**
     * TODO: دعم HTTPS
     * ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
     * لإضافة دعم HTTPS، نحتاج:
     *   1. تضمين mbedTLS (أخف من OpenSSL)
     *   2. تهيئة SSL context
     *   3. إجراء SSL handshake بعد connect()
     *   4. استخدام mbedtls_ssl_read/write بدلاً من recv/send
     * 
     * الخطوات:
     *   - تحميل mbedTLS من https://github.com/Mbed-TLS/mbedtls
     *   - ترجمة لـ Android NDK
     *   - إضافة دعم شهادات CA
     * ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
     */
    if (is_https) {
        رد.الحالة = حالة_خطأ_SSL;
        رد.رسالة_الخطأ = strdup("HTTPS غير مدعوم حالياً — سيُضاف في إصدار قادم. استخدم HTTP مؤقتاً");
        LOGE("HTTPS not supported yet - will be added with mbedTLS");
        return رد;
    }
    
    LOGI("%s %s:%d%s", method_string(إ->الطريقة), host, port, path);
    
    // الاتصال
    int timeout = إ->مهلة_اتصال > 0 ? إ->مهلة_اتصال : الشبكة->مهلة_اتصال;
    int sock = connect_to_host(host, port, timeout);
    if (sock < 0) {
        رد.الحالة = حالة_فشل_اتصال;
        رد.رسالة_الخطأ = strdup("فشل الاتصال بالخادم");
        return رد;
    }
    
    // بناء طلب HTTP
    char request[4096];
    int reqLen = 0;
    const char* method = method_string(إ->الطريقة);
    const char* contentType = إ->نوع_المحتوى ? إ->نوع_المحتوى : "application/json; charset=utf-8";
    size_t bodyLen = إ->حجم_الجسم > 0 ? إ->حجم_الجسم : (إ->الجسم ? strlen(إ->الجسم) : 0);
    
    reqLen = snprintf(request, sizeof(request),
        "%s %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Connection: close\r\n"
        "Accept: */*\r\n"
        "User-Agent: SadLang/1.0\r\n",
        method, path, host);
    
    // ترويسات افتراضية
    for (int i = 0; i < الشبكة->عدد_ترويسات_افتراضية; i++) {
        reqLen += snprintf(request + reqLen, sizeof(request) - reqLen,
            "%s: %s\r\n", 
            الشبكة->ترويسات_افتراضية[i].مفتاح,
            الشبكة->ترويسات_افتراضية[i].قيمة);
    }
    
    // ترويسات الطلب
    if (إ->الترويسات) {
        for (int i = 0; i < إ->عدد_الترويسات; i++) {
            reqLen += snprintf(request + reqLen, sizeof(request) - reqLen,
                "%s: %s\r\n", إ->الترويسات[i].مفتاح, إ->الترويسات[i].قيمة);
        }
    }
    
    // جسم الطلب
    if (bodyLen > 0) {
        reqLen += snprintf(request + reqLen, sizeof(request) - reqLen,
            "Content-Type: %s\r\n"
            "Content-Length: %zu\r\n",
            contentType, bodyLen);
    }
    
    reqLen += snprintf(request + reqLen, sizeof(request) - reqLen, "\r\n");
    
    // إرسال الترويسات
    if (!send_all(sock, request, reqLen)) {
        close(sock);
        رد.الحالة = حالة_فشل_اتصال;
        رد.رسالة_الخطأ = strdup("فشل إرسال الطلب");
        return رد;
    }
    
    // إرسال الجسم
    if (bodyLen > 0 && إ->الجسم) {
        if (!send_all(sock, إ->الجسم, bodyLen)) {
            close(sock);
            رد.الحالة = حالة_فشل_اتصال;
            رد.رسالة_الخطأ = strdup("فشل إرسال جسم الطلب");
            return رد;
        }
    }
    
    // استقبال الاستجابة
    int readTimeout = إ->مهلة_قراءة > 0 ? إ->مهلة_قراءة : الشبكة->مهلة_قراءة;
    size_t rawLen = 0;
    int statusCode = 0;
    char* raw = recv_http_response(sock, readTimeout, &rawLen, &statusCode);
    close(sock);
    
    if (!raw || rawLen == 0) {
        free(raw);
        رد.الحالة = حالة_مهلة;
        رد.رسالة_الخطأ = strdup("لم يتم استقبال استجابة");
        return رد;
    }
    
    // تحليل الاستجابة
    char* headers = NULL;
    size_t bodySize = 0;
    char* body = split_headers_body(raw, rawLen, &headers, &bodySize);
    
    رد.الحالة = حالة_نجاح;
    رد.كود_الحالة = statusCode;
    رد.الجسم = malloc(bodySize + 1);
    if (رد.الجسم) {
        memcpy(رد.الجسم, body, bodySize);
        رد.الجسم[bodySize] = '\0';
    }
    رد.حجم_الجسم = bodySize;
    رد.الترويسات_خام = headers ? strdup(headers) : NULL;
    
    free(raw);
    
    LOGI("استجابة: %d, حجم: %zu", statusCode, bodySize);
    return رد;
}

استجابة_شبكة أرسل_طلب(مقبض_شبكة* الشبكة, const char* الرابط) {
    إعدادات_طلب إ = {0};
    إ.الطريقة = طريقة_GET;
    إ.الرابط = الرابط;
    return أرسل_طلب_مخصص(الشبكة, &إ);
}

استجابة_شبكة أرسل_POST(مقبض_شبكة* الشبكة, const char* الرابط, const char* json) {
    إعدادات_طلب إ = {0};
    إ.الطريقة = طريقة_POST;
    إ.الرابط = الرابط;
    إ.الجسم = json;
    return أرسل_طلب_مخصص(الشبكة, &إ);
}

استجابة_شبكة أرسل_PUT(مقبض_شبكة* الشبكة, const char* الرابط, const char* json) {
    إعدادات_طلب إ = {0};
    إ.الطريقة = طريقة_PUT;
    إ.الرابط = الرابط;
    إ.الجسم = json;
    return أرسل_طلب_مخصص(الشبكة, &إ);
}

استجابة_شبكة أرسل_DELETE(مقبض_شبكة* الشبكة, const char* الرابط) {
    إعدادات_طلب إ = {0};
    إ.الطريقة = طريقة_DELETE;
    إ.الرابط = الرابط;
    return أرسل_طلب_مخصص(الشبكة, &إ);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  طلبات غير متزامنة
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    مقبض_شبكة* الشبكة;
    إعدادات_طلب إعدادات;
    دالة_استجابة الدالة;
    void* بيانات;
    char* رابط_نسخة;
    char* جسم_نسخة;
} بيانات_خيط;

static void* async_thread_func(void* arg) {
    بيانات_خيط* بخ = (بيانات_خيط*)arg;
    
    استجابة_شبكة رد = أرسل_طلب_مخصص(بخ->الشبكة, &بخ->إعدادات);
    
    if (بخ->الدالة) {
        بخ->الدالة(&رد, بخ->بيانات);
    }
    
    حرر_استجابة(&رد);
    free(بخ->رابط_نسخة);
    free(بخ->جسم_نسخة);
    free(بخ);
    return NULL;
}

void أرسل_طلب_متزامن(مقبض_شبكة* الشبكة, const char* الرابط,
                       دالة_استجابة الدالة, void* بيانات_مستخدم) {
    إعدادات_طلب إ = {0};
    إ.الطريقة = طريقة_GET;
    إ.الرابط = الرابط;
    أرسل_طلب_مخصص_متزامن(الشبكة, &إ, الدالة, بيانات_مستخدم);
}

void أرسل_طلب_مخصص_متزامن(مقبض_شبكة* الشبكة, const إعدادات_طلب* الإعدادات,
                            دالة_استجابة الدالة, void* بيانات_مستخدم) {
    if (!الشبكة || !الإعدادات) return;
    
    بيانات_خيط* بخ = calloc(1, sizeof(بيانات_خيط));
    if (!بخ) return;
    
    بخ->الشبكة = الشبكة;
    بخ->إعدادات = *الإعدادات;
    بخ->الدالة = الدالة;
    بخ->بيانات = بيانات_مستخدم;
    
    // نسخ النصوص لأنها قد تتحرر
    if (الإعدادات->الرابط) {
        بخ->رابط_نسخة = strdup(الإعدادات->الرابط);
        بخ->إعدادات.الرابط = بخ->رابط_نسخة;
    }
    if (الإعدادات->الجسم) {
        بخ->جسم_نسخة = strdup(الإعدادات->الجسم);
        بخ->إعدادات.الجسم = بخ->جسم_نسخة;
    }
    
    pthread_t thread;
    pthread_create(&thread, NULL, async_thread_func, بخ);
    pthread_detach(thread);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إعدادات
// ═══════════════════════════════════════════════════════════════════════════════

void عيّن_ترويسة_افتراضية(مقبض_شبكة* الشبكة, const char* مفتاح, const char* قيمة) {
    if (!الشبكة || !مفتاح || !قيمة) return;
    if (الشبكة->عدد_ترويسات_افتراضية >= MAX_DEFAULT_HEADERS) return;
    
    int idx = الشبكة->عدد_ترويسات_افتراضية;
    الشبكة->ترويسات_افتراضية[idx].مفتاح = strdup(مفتاح);
    الشبكة->ترويسات_افتراضية[idx].قيمة = strdup(قيمة);
    الشبكة->عدد_ترويسات_افتراضية++;
}

void عيّن_مهلة_الاتصال(مقبض_شبكة* الشبكة, int مللي_ثانية) {
    if (الشبكة) الشبكة->مهلة_اتصال = مللي_ثانية;
}

void عيّن_مهلة_القراءة(مقبض_شبكة* الشبكة, int مللي_ثانية) {
    if (الشبكة) الشبكة->مهلة_قراءة = مللي_ثانية;
}

void حرر_استجابة(استجابة_شبكة* ا) {
    if (!ا) return;
    free(ا->الجسم);
    free(ا->الترويسات_خام);
    free(ا->رسالة_الخطأ);
    ا->الجسم = NULL;
    ا->الترويسات_خام = NULL;
    ا->رسالة_الخطأ = NULL;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تحميل ملفات
// ═══════════════════════════════════════════════════════════════════════════════

bool حمّل_ملف(مقبض_شبكة* الشبكة, const char* الرابط, const char* مسار_الحفظ) {
    if (!الشبكة || !الرابط || !مسار_الحفظ) return false;
    
    استجابة_شبكة رد = أرسل_طلب(الشبكة, الرابط);
    if (رد.الحالة != حالة_نجاح || !رد.الجسم || رد.حجم_الجسم == 0) {
        حرر_استجابة(&رد);
        return false;
    }
    
    FILE* f = fopen(مسار_الحفظ, "wb");
    if (!f) {
        حرر_استجابة(&رد);
        return false;
    }
    
    size_t written = fwrite(رد.الجسم, 1, رد.حجم_الجسم, f);
    fclose(f);
    حرر_استجابة(&رد);
    
    LOGI("تم تحميل %zu bytes → %s", written, مسار_الحفظ);
    return written == رد.حجم_الجسم;
}
