// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: middleware.h
 * الوصف: وسيطات الطلبات (Middleware) لخادم مستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوسيطات تُنفَّذ قبل أو بعد كل طلب HTTP:
 *   - تسجيل الطلبات (Logging)
 *   - تقييد المعدل (Rate Limiting)
 *   - التحقق من الهوية (Authentication check)
 *   - CORS (Cross-Origin Resource Sharing)
 *   - ضغط الاستجابات (Compression)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "../server/http_server.h"
#include "../auth/auth.h"

#include <string>
#include <map>
#include <chrono>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <functional>
#include <vector>
#include <ctime>

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// مسجِّل الطلبات
// ═══════════════════════════════════════════════════════════════════════════════
//
// يُسجِّل كل طلب وارد مع:
//   - الطابع الزمني
//   - الطريقة (GET/POST/PUT/DELETE)
//   - المسار
//   - رمز الحالة
//   - وقت المعالجة بالمللي ثانية

/**
 * @brief وسيط تسجيل الطلبات
 *
 * مثال المخرج:
 *   [2025-01-15 14:30:22] GET /api/v1/packages → 200 (12ms)
 *   [2025-01-15 14:30:23] POST /api/v1/publish → 201 (156ms)
 */
class RequestLogger {
public:
    /**
     * @brief تغليف معالج بتسجيل
     *
     * يقيس وقت المعالجة ويُسجِّل المعلومات
     */
    static RequestHandler wrap(RequestHandler handler) {
        return [handler](const HttpRequest& req) -> HttpResponse {
            auto start = std::chrono::high_resolution_clock::now();

            // تنفيذ المعالج الأصلي
            HttpResponse response = handler(req);

            auto end = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            // تسجيل الطلب
            std::cerr << "  [طلب] " << req.method << " " << req.path
                      << " → " << response.status_code
                      << " (" << ms << "مللي ثانية)\n";

            return response;
        };
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// محدِّد المعدل (Rate Limiter)
// ═══════════════════════════════════════════════════════════════════════════════
//
// يمنع الإسراف في الطلبات عبر تتبع عدد الطلبات لكل عنوان IP.
//
// الخوارزمية: نافذة منزلقة بسيطة
//   ١. لكل IP، نحتفظ بعدد الطلبات وطابع آخر إعادة تعيين
//   ٢. إذا مر وقت كافٍ منذ آخر إعادة تعيين → إعادة العداد
//   ٣. إذا تجاوز العداد الحد الأقصى → رفض الطلب (429)

/**
 * @brief وسيط تحديد المعدل
 */
class RateLimiter {
public:
    /**
     * @brief إنشاء محدد المعدل
     * @param max_requests أقصى عدد طلبات في كل نافذة
     * @param window_seconds مدة النافذة بالثواني
     */
    RateLimiter(int max_requests = 100, int window_seconds = 60)
        : max_requests_(max_requests)
        , window_seconds_(window_seconds) {}

    /**
     * @brief التحقق مما إذا كان الطلب مسموحاً
     * @param client_ip عنوان IP للعميل
     * @return true إذا كان مسموحاً، false إذا تجاوز الحد
     */
    bool allow(const std::string& client_ip) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();

        auto& entry = clients_[client_ip];

        // إذا مرت النافذة → إعادة تعيين
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - entry.window_start).count();
        if (elapsed >= window_seconds_) {
            entry.count = 0;
            entry.window_start = now;
        }

        entry.count++;
        return entry.count <= max_requests_;
    }

    /**
     * @brief تغليف معالج بتحديد المعدل
     */
    RequestHandler wrap(RequestHandler handler) {
        return [this, handler](const HttpRequest& req) -> HttpResponse {
            // استخراج IP من الرؤوس أو استخدام عنوان افتراضي
            std::string ip = "unknown";
            auto it = req.headers.find("X-Forwarded-For");
            if (it != req.headers.end()) ip = it->second;
            else {
                it = req.headers.find("X-Real-IP");
                if (it != req.headers.end()) ip = it->second;
            }

            if (!allow(ip)) {
                return HttpResponse::error(429, "تم تجاوز الحد الأقصى للطلبات. حاول مرة أخرى لاحقاً.");
            }

            return handler(req);
        };
    }

    /**
     * @brief الطلبات المتبقية لعنوان IP
     */
    int remaining(const std::string& client_ip) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clients_.find(client_ip);
        if (it == clients_.end()) return max_requests_;
        return std::max(0, max_requests_ - it->second.count);
    }

private:
    struct ClientEntry {
        int count = 0;
        std::chrono::steady_clock::time_point window_start = std::chrono::steady_clock::now();
    };

    std::map<std::string, ClientEntry> clients_;
    std::mutex mutex_;
    int max_requests_;
    int window_seconds_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// مُصادق الطلبات
// ═══════════════════════════════════════════════════════════════════════════════
//
// يتحقق من رمز المصادقة في رأس Authorization ويُضيف
// معلومات المستخدم إلى الطلب.

/**
 * @brief بنية معلومات المصادقة
 */
struct AuthContext {
    bool is_authenticated = false;
    int64_t user_id = 0;
    std::string username;
    bool is_admin = false;
};

/**
 * @brief استخراج سياق المصادقة من الطلب
 *
 * يدعم نوعين من المصادقة:
 *   ١. JWT في رأس Authorization: Bearer <token>
 *   ٢. رمز API في رأس X-API-Token: sad_<token>
 */
inline AuthContext extract_auth(const HttpRequest& req, AuthManager& auth) {
    AuthContext ctx;

    // محاولة JWT أولاً
    std::string token = req.get_auth_token();
    if (!token.empty()) {
        auto result = auth.verify_token(token);
        if (result) {
            ctx.is_authenticated = true;
            ctx.user_id = result->id;
            ctx.username = result->username;
            ctx.is_admin = result->is_admin;
        }
        return ctx;
    }

    // محاولة رمز API
    auto it = req.headers.find("X-API-Token");
    if (it != req.headers.end()) {
        auto result = auth.verify_api_token(it->second);
        if (result) {
            ctx.is_authenticated = true;
            ctx.user_id = result->id;
            ctx.username = result->username;
        }
    }

    return ctx;
}

/**
 * @brief وسيط يتطلب المصادقة
 *
 * يرفض الطلبات غير المصادق عليها برمز 401
 */
inline RequestHandler require_auth(RequestHandler handler, AuthManager& auth) {
    return [handler, &auth](const HttpRequest& req) -> HttpResponse {
        auto ctx = extract_auth(req, auth);
        if (!ctx.is_authenticated) {
            return HttpResponse::unauthorized("يجب تسجيل الدخول أولاً");
        }
        return handler(req);
    };
}

/**
 * @brief وسيط يتطلب صلاحيات المشرف
 */
inline RequestHandler require_admin(RequestHandler handler, AuthManager& auth) {
    return [handler, &auth](const HttpRequest& req) -> HttpResponse {
        auto ctx = extract_auth(req, auth);
        if (!ctx.is_authenticated) {
            return HttpResponse::unauthorized("يجب تسجيل الدخول أولاً");
        }
        if (!ctx.is_admin) {
            return HttpResponse::error(403, "صلاحيات غير كافية - هذا الإجراء يتطلب صلاحيات مشرف");
        }
        return handler(req);
    };
}

// ═══════════════════════════════════════════════════════════════════════════════
// سلسلة الوسيطات
// ═══════════════════════════════════════════════════════════════════════════════
//
// تسمح بتطبيق عدة وسيطات بالتسلسل على معالج واحد.
//
// مثال:
//   auto handler = MiddlewareChain::build(my_handler, {
//       logger_middleware,
//       rate_limiter_middleware,
//       auth_middleware,
//   });

/**
 * @brief سلسلة وسيطات قابلة للتكوين
 */
class MiddlewareChain {
public:
    using Middleware = std::function<RequestHandler(RequestHandler)>;

    /**
     * @brief بناء سلسلة وسيطات
     * @param handler المعالج الأصلي
     * @param middlewares قائمة الوسيطات (تُطبَّق من الأخيرة للأولى)
     */
    static RequestHandler build(RequestHandler handler,
                                 const std::vector<Middleware>& middlewares) {
        RequestHandler current = handler;
        // تطبيق الوسيطات بالترتيب العكسي
        for (auto it = middlewares.rbegin(); it != middlewares.rend(); ++it) {
            current = (*it)(current);
        }
        return current;
    }
};

} // namespace registry
} // namespace sad
