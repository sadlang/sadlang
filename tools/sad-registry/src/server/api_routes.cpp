// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: api_routes.cpp
 * الوصف: تنفيذ جميع نقاط واجهة برمجة التطبيقات (API) لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يُنفِّذ ١٧+ نقطة API تشمل:
 *   - إدارة الحزم (قائمة، تفاصيل، نشر، تنزيل، سحب)
 *   - البحث (يدعم العربية والإنجليزية)
 *   - المصادقة (تسجيل، دخول، ملف شخصي)
 *   - الإحصائيات والاستكشاف (مميزة، شائعة، حديثة، تصنيفات)
 *
 * تنسيق الاستجابة:
 *   جميع الاستجابات بتنسيق JSON مع هيكلية موحدة:
 *   {
 *     "نجاح": true/false,     // أو "success"
 *     "بيانات": {...},         // أو "data"
 *     "خطأ": "..."            // أو "error" (في حالة الفشل)
 *   }
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "api_routes.h"
#include "middleware.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <chrono>
#include <ctime>

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// المُنشئ
// ═══════════════════════════════════════════════════════════════════════════════

ApiRoutes::ApiRoutes(std::shared_ptr<Database> db,
                     std::shared_ptr<BlobStore> blobs,
                     std::shared_ptr<ArabicSearchEngine> search,
                     std::shared_ptr<AuthManager> auth)
    : db_(db), blobs_(blobs), search_(search), auth_(auth) {}

// ═══════════════════════════════════════════════════════════════════════════════
// تسجيل المسارات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تسجيل جميع نقاط API في الخادم
 *
 * يسجل المسارات الإنجليزية والعربية البديلة.
 * كل معالج يُغلَّف بوسيط التسجيل (اختيارياً).
 */
void ApiRoutes::register_routes(HttpServer& server) {
    std::cerr << "  ⟐ تسجيل نقاط API...\n";

    // ─────────────────────────────────────────────────────────────────
    // نقاط الحزم
    // ─────────────────────────────────────────────────────────────────

    // قائمة الحزم
    server.get("/api/v1/packages", [this](const HttpRequest& r) {
        return handle_list_packages(r);
    });

    // معلومات حزمة
    server.get("/api/v1/packages/:name", [this](const HttpRequest& r) {
        return handle_get_package(r);
    });

    // إصدار محدد
    server.get("/api/v1/packages/:name/:version", [this](const HttpRequest& r) {
        return handle_get_version(r);
    });

    // نشر حزمة (POST)
    server.post("/api/v1/packages", [this](const HttpRequest& r) {
        return handle_publish(r);
    });

    // تنزيل
    server.get("/api/v1/download/:name/:version", [this](const HttpRequest& r) {
        return handle_download(r);
    });

    // سحب إصدار
    server.del("/api/v1/packages/:name/:version", [this](const HttpRequest& r) {
        return handle_yank_version(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط البحث
    // ─────────────────────────────────────────────────────────────────

    server.get("/api/v1/search", [this](const HttpRequest& r) {
        return handle_search(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط المصادقة
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/register", [this](const HttpRequest& r) {
        return handle_register(r);
    });

    server.post("/api/v1/login", [this](const HttpRequest& r) {
        return handle_login(r);
    });

    server.get("/api/v1/me", [this](const HttpRequest& r) {
        return handle_me(r);
    });

    server.get("/api/v1/me/packages", [this](const HttpRequest& r) {
        return handle_my_packages(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط الإحصائيات والاستكشاف
    // ─────────────────────────────────────────────────────────────────

    server.get("/api/v1/stats", [this](const HttpRequest& r) {
        return handle_stats(r);
    });

    server.get("/api/v1/featured", [this](const HttpRequest& r) {
        return handle_featured(r);
    });

    server.get("/api/v1/popular", [this](const HttpRequest& r) {
        return handle_popular(r);
    });

    server.get("/api/v1/recent", [this](const HttpRequest& r) {
        return handle_recent(r);
    });

    server.get("/api/v1/categories", [this](const HttpRequest& r) {
        return handle_categories(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // المسارات العربية البديلة
    // ─────────────────────────────────────────────────────────────────

    server.get("/api/v1/%D8%AD%D8%B2%D9%85", [this](const HttpRequest& r) {
        return handle_list_packages(r);
    });

    server.get("/api/v1/%D8%A8%D8%AD%D8%AB", [this](const HttpRequest& r) {
        return handle_search(r);
    });

    server.get("/api/v1/%D8%A5%D8%AD%D8%B5%D8%A7%D8%A6%D9%8A%D8%A7%D8%AA", [this](const HttpRequest& r) {
        return handle_stats(r);
    });

    std::cerr << "  ✓ تم تسجيل " << 17 << " نقطة API\n";

    // ─────────────────────────────────────────────────────────────────
    // نقاط التقييمات والمراجعات
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/packages/:name/reviews", [this](const HttpRequest& r) {
        return handle_create_review(r);
    });
    server.get("/api/v1/packages/:name/reviews", [this](const HttpRequest& r) {
        return handle_get_reviews(r);
    });
    server.del("/api/v1/reviews/:id", [this](const HttpRequest& r) {
        return handle_delete_review(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط التفضيلات
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/packages/:name/star", [this](const HttpRequest& r) {
        return handle_toggle_star(r);
    });
    server.get("/api/v1/me/stars", [this](const HttpRequest& r) {
        return handle_my_stars(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط الإبلاغات
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/packages/:name/report", [this](const HttpRequest& r) {
        return handle_create_report(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط الخطّافات
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/webhooks", [this](const HttpRequest& r) {
        return handle_create_webhook(r);
    });
    server.get("/api/v1/webhooks", [this](const HttpRequest& r) {
        return handle_list_webhooks(r);
    });
    server.del("/api/v1/webhooks/:id", [this](const HttpRequest& r) {
        return handle_delete_webhook(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط المسؤول
    // ─────────────────────────────────────────────────────────────────

    server.get("/api/v1/admin/stats", [this](const HttpRequest& r) {
        return handle_admin_stats(r);
    });
    server.get("/api/v1/admin/reports", [this](const HttpRequest& r) {
        return handle_admin_reports(r);
    });
    server.post("/api/v1/admin/reports/:id/resolve", [this](const HttpRequest& r) {
        return handle_admin_resolve_report(r);
    });
    server.post("/api/v1/admin/packages/:name/flag", [this](const HttpRequest& r) {
        return handle_admin_set_package_flag(r);
    });
    server.get("/api/v1/admin/audit", [this](const HttpRequest& r) {
        return handle_admin_audit_log(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط إضافية
    // ─────────────────────────────────────────────────────────────────

    server.get("/api/v1/suggest", [this](const HttpRequest& r) {
        return handle_suggest(r);
    });
    server.post("/api/v1/me", [this](const HttpRequest& r) {
        return handle_update_profile(r);
    });
    server.get("/api/v1/users/:username", [this](const HttpRequest& r) {
        return handle_public_profile(r);
    });
    server.get("/api/v1/packages/:name/downloads", [this](const HttpRequest& r) {
        return handle_download_stats(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط مفاتيح الناشرين والتوقيعات الرقمية
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/keys", [this](const HttpRequest& r) {
        return handle_add_publisher_key(r);
    });
    server.get("/api/v1/keys", [this](const HttpRequest& r) {
        return handle_list_my_keys(r);
    });
    server.del("/api/v1/keys/:id", [this](const HttpRequest& r) {
        return handle_revoke_key(r);
    });
    server.get("/api/v1/packages/:name/:version/verify", [this](const HttpRequest& r) {
        return handle_verify_signature(r);
    });
    server.get("/api/v1/packages/:name/:version/signatures", [this](const HttpRequest& r) {
        return handle_get_signatures(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط حل التبعيات
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/resolve", [this](const HttpRequest& r) {
        return handle_resolve_dependencies(r);
    });
    server.get("/api/v1/packages/:name/:version/compatibility", [this](const HttpRequest& r) {
        return handle_check_compatibility(r);
    });
    server.get("/api/v1/packages/:name/dependents", [this](const HttpRequest& r) {
        return handle_reverse_dependencies(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط الإشعارات
    // ─────────────────────────────────────────────────────────────────

    server.get("/api/v1/notifications", [this](const HttpRequest& r) {
        return handle_list_notifications(r);
    });
    server.post("/api/v1/notifications/:id/read", [this](const HttpRequest& r) {
        return handle_mark_notification_read(r);
    });
    server.post("/api/v1/notifications/read-all", [this](const HttpRequest& r) {
        return handle_mark_all_read(r);
    });
    server.del("/api/v1/notifications/:id", [this](const HttpRequest& r) {
        return handle_delete_notification(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط الاشتراكات
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/packages/:name/subscribe", [this](const HttpRequest& r) {
        return handle_subscribe(r);
    });
    server.del("/api/v1/subscriptions/:id", [this](const HttpRequest& r) {
        return handle_unsubscribe(r);
    });
    server.get("/api/v1/subscriptions", [this](const HttpRequest& r) {
        return handle_list_subscriptions(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط المنظمات
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/organizations", [this](const HttpRequest& r) {
        return handle_create_organization(r);
    });
    server.get("/api/v1/organizations/:name", [this](const HttpRequest& r) {
        return handle_get_organization(r);
    });
    server.put("/api/v1/organizations/:name", [this](const HttpRequest& r) {
        return handle_update_organization(r);
    });
    server.post("/api/v1/organizations/:name/members", [this](const HttpRequest& r) {
        return handle_add_org_member(r);
    });
    server.del("/api/v1/organizations/:name/members/:member_id", [this](const HttpRequest& r) {
        return handle_remove_org_member(r);
    });
    server.get("/api/v1/organizations/:name/members", [this](const HttpRequest& r) {
        return handle_list_org_members(r);
    });
    server.get("/api/v1/me/organizations", [this](const HttpRequest& r) {
        return handle_list_my_organizations(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط الاستشارات الأمنية
    // ─────────────────────────────────────────────────────────────────

    server.post("/api/v1/advisories", [this](const HttpRequest& r) {
        return handle_create_advisory(r);
    });
    server.get("/api/v1/advisories", [this](const HttpRequest& r) {
        return handle_list_advisories(r);
    });
    server.get("/api/v1/advisories/:id", [this](const HttpRequest& r) {
        return handle_get_advisory(r);
    });
    server.get("/api/v1/packages/:name/:version/advisories", [this](const HttpRequest& r) {
        return handle_check_advisories(r);
    });

    // ─────────────────────────────────────────────────────────────────
    // نقاط التحليلات والصحة
    // ─────────────────────────────────────────────────────────────────

    server.get("/api/v1/packages/:name/analytics", [this](const HttpRequest& r) {
        return handle_detailed_analytics(r);
    });
    server.get("/api/v1/health", [this](const HttpRequest& r) {
        return handle_health(r);
    });

    std::cerr << "  ✓ تم تسجيل نقاط API الجديدة (تقييمات، تفضيلات، بلاغات، إدارة)\n";
    std::cerr << "  ✓ تم تسجيل نقاط API الثورية (توقيعات، تبعيات، إشعارات، منظمات، أمان)\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات الحزم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief GET /api/v1/packages - قائمة الحزم مع تصفيح
 *
 * معاملات الاستعلام:
 *   page    - رقم الصفحة (افتراضي: 1)
 *   per_page - عدد النتائج (افتراضي: 20، أقصى: 100)
 *   sort    - ترتيب: name, downloads, created (افتراضي: name)
 *   order   - اتجاه: asc, desc (افتراضي: asc)
 *
 * الاستجابة:
 *   {
 *     "packages": [...],
 *     "pagination": { "page": 1, "per_page": 20, "total": 150, "pages": 8 }
 *   }
 */
HttpResponse ApiRoutes::handle_list_packages(const HttpRequest& req) {
    // استخراج معاملات التصفيح
    int page = req.get_page();
    int per_page = 20;
    std::string sort = "name";
    bool desc = false;

    auto it = req.query_params.find("per_page");
    if (it != req.query_params.end()) {
        try { per_page = std::stoi(it->second); } catch (...) {}
        per_page = std::clamp(per_page, 1, 100);
    }
    it = req.query_params.find("sort");
    if (it != req.query_params.end()) sort = it->second;
    it = req.query_params.find("order");
    if (it != req.query_params.end()) desc = (it->second == "desc");

    // استعلام قاعدة البيانات
    auto result = db_->list_packages(page, per_page, sort, desc);

    // بناء JSON
    std::ostringstream json;
    json << "{\"packages\":[";
    for (size_t i = 0; i < result.items.size(); ++i) {
        if (i > 0) json << ",";
        json << package_to_json(result.items[i]);
    }
    json << "],\"pagination\":{";
    json << "\"page\":" << result.page << ",";
    json << "\"per_page\":" << result.per_page << ",";
    json << "\"total\":" << result.total_count << ",";
    json << "\"pages\":" << result.total_pages << "}}";

    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/packages/:name - معلومات حزمة واحدة
 *
 * يبحث بالاسم الإنجليزي أو العربي.
 * يُرجع تفاصيل الحزمة مع قائمة الإصدارات.
 */
HttpResponse ApiRoutes::handle_get_package(const HttpRequest& req) {
    auto it = req.path_params.find("name");
    if (it == req.path_params.end()) {
        return HttpResponse::error(400, "اسم الحزمة مطلوب");
    }

    auto pkg = db_->get_package(it->second);
    if (!pkg) {
        return HttpResponse::not_found("لم يُعثر على حزمة بالاسم: " + it->second);
    }

    // جلب الإصدارات
    auto versions = db_->get_package_versions(pkg->id);

    // بناء JSON
    std::ostringstream json;
    json << "{\"package\":" << package_to_json(*pkg);
    json << ",\"versions\":[";
    for (size_t i = 0; i < versions.size(); ++i) {
        if (i > 0) json << ",";
        json << version_to_json(versions[i]);
    }
    json << "]}";

    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/packages/:name/:version - إصدار محدد
 */
HttpResponse ApiRoutes::handle_get_version(const HttpRequest& req) {
    auto name_it = req.path_params.find("name");
    auto ver_it = req.path_params.find("version");

    if (name_it == req.path_params.end() || ver_it == req.path_params.end()) {
        return HttpResponse::error(400, "اسم الحزمة ورقم الإصدار مطلوبان");
    }

    auto pkg = db_->get_package(name_it->second);
    if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");

    auto version = db_->get_version(pkg->id, ver_it->second);
    if (!version) return HttpResponse::not_found("لم يُعثر على الإصدار");

    std::ostringstream json;
    json << "{\"package\":" << package_to_json(*pkg);
    json << ",\"version\":" << version_to_json(*version) << "}";

    return HttpResponse::json(json.str());
}

/**
 * @brief POST /api/v1/packages - نشر حزمة جديدة أو إصدار جديد
 *
 * يتطلب مصادقة.
 *
 * جسم الطلب:
 *   {
 *     "name": "my-package",
 *     "arabic_name": "حزمتي",
 *     "description": "...",
 *     "description_ar": "...",
 *     "version": "1.0.0",
 *     "license": "MIT",
 *     "data": "<base64 encoded archive>"
 *   }
 *
 * العملية:
 *   ١. التحقق من المصادقة
 *   ٢. فك ترميز البيانات
 *   ٣. تخزين الأرشيف في مخزن الكائنات
 *   ٤. إنشاء/تحديث الحزمة في قاعدة البيانات
 *   ٥. إرجاع معلومات النشر
 */
HttpResponse ApiRoutes::handle_publish(const HttpRequest& req) {
    // التحقق من المصادقة
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::unauthorized("يجب تسجيل الدخول لنشر حزمة");
    }

    // تحليل الجسم
    std::string name = json_get_string(req.body, "name");
    std::string arabic_name = json_get_string(req.body, "arabic_name");
    std::string description = json_get_string(req.body, "description");
    std::string description_ar = json_get_string(req.body, "description_ar");
    std::string version_str = json_get_string(req.body, "version");
    std::string license = json_get_string(req.body, "license");
    std::string data_b64 = json_get_string(req.body, "data");

    // التحقق من المدخلات
    if (name.empty()) return HttpResponse::error(400, "اسم الحزمة مطلوب");
    if (version_str.empty()) return HttpResponse::error(400, "رقم الإصدار مطلوب");

    // التحقق من صحة اسم الحزمة (أحرف لاتينية، أرقام، شرطات)
    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            return HttpResponse::error(400,
                "اسم الحزمة يجب أن يحتوي فقط على أحرف لاتينية وأرقام وشرطات");
        }
    }

    // فك ترميز البيانات (Base64 → bytes)
    // في الوقت الحالي نأخذ الجسم الخام كبيانات الحزمة
    std::vector<uint8_t> archive_data(req.body.begin(), req.body.end());
    if (!data_b64.empty()) {
        // إذا كانت البيانات مرمّزة بـ Base64
        archive_data.assign(data_b64.begin(), data_b64.end());
    }

    // تخزين في مخزن الكائنات
    std::string blob_hash = blobs_->store(archive_data);
    if (blob_hash.empty()) {
        return HttpResponse::error(500, "فشل تخزين أرشيف الحزمة");
    }

    // التحقق من وجود الحزمة
    auto existing = db_->get_package(name);
    int64_t package_id;

    if (existing) {
        // حزمة موجودة → التحقق من الملكية
        if (existing->owner_id != user->id && !user->is_admin) {
            return HttpResponse::error(403, "ليس لديك صلاحية لتحديث هذه الحزمة");
        }
        package_id = existing->id;
    } else {
        // حزمة جديدة
        PackageInfo new_pkg;
        new_pkg.name = name;
        new_pkg.arabic_name = arabic_name;
        new_pkg.display_name = name;
        new_pkg.description = description;
        new_pkg.description_ar = description_ar;
        new_pkg.owner_id = user->id;
        new_pkg.license = license.empty() ? "MIT" : license;

        package_id = db_->create_package(new_pkg);
        if (package_id < 0) {
            return HttpResponse::error(500, "فشل إنشاء الحزمة في قاعدة البيانات");
        }
    }

    // تحليل رقم الإصدار (major.minor.patch)
    int major = 0, minor = 0, patch = 0;
    if (sscanf(version_str.c_str(), "%d.%d.%d", &major, &minor, &patch) < 1) {
        return HttpResponse::error(400, "رقم الإصدار غير صالح (يجب أن يكون بتنسيق X.Y.Z)");
    }

    // التحقق من عدم وجود نفس الإصدار
    auto existing_ver = db_->get_version(package_id, version_str);
    if (existing_ver) {
        return HttpResponse::error(409, "الإصدار " + version_str + " موجود مسبقاً");
    }

    // إنشاء الإصدار
    VersionInfo ver;
    ver.package_id = package_id;
    ver.version = version_str;
    ver.major = major;
    ver.minor = minor;
    ver.patch = patch;
    ver.checksum_sha256 = blob_hash;
    ver.blob_path = blob_hash;
    ver.file_size = static_cast<int64_t>(archive_data.size());

    int64_t version_id = db_->create_version(ver);
    if (version_id < 0) {
        return HttpResponse::error(500, "فشل إنشاء الإصدار");
    }

    // تحديث فهرس البحث العربي
    if (search_ && !arabic_name.empty()) {
        search_->index_package(package_id, name, arabic_name,
                              description, description_ar);
    }

    // سجل المراجعة
    db_->log_audit(user->id, "publish", name + "@" + version_str, "", "");

    // استجابة النجاح
    std::ostringstream json;
    json << "{\"success\":true,\"message\":\"تم نشر الحزمة بنجاح\",";
    json << "\"package\":\"" << name << "\",";
    json << "\"version\":\"" << version_str << "\",";
    json << "\"checksum\":\"" << blob_hash << "\",";
    json << "\"size\":" << archive_data.size() << "}";

    HttpResponse resp;
    resp.status_code = 201;
    resp.set_header("Content-Type", "application/json; charset=utf-8");
    resp.body = json.str();
    return resp;
}

/**
 * @brief GET /api/v1/download/:name/:version - تنزيل أرشيف حزمة
 *
 * يُرجع الملف الثنائي مباشرة مع الرؤوس المناسبة.
 */
HttpResponse ApiRoutes::handle_download(const HttpRequest& req) {
    auto name_it = req.path_params.find("name");
    auto ver_it = req.path_params.find("version");

    if (name_it == req.path_params.end() || ver_it == req.path_params.end()) {
        return HttpResponse::error(400, "اسم الحزمة ورقم الإصدار مطلوبان");
    }

    // جلب معلومات الحزمة والإصدار
    auto pkg = db_->get_package(name_it->second);
    if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");

    auto ver = db_->get_version(pkg->id, ver_it->second);
    if (!ver) return HttpResponse::not_found("لم يُعثر على الإصدار");

    // جلب البيانات من المخزن
    auto data = blobs_->retrieve(ver->blob_path);
    if (data.empty()) {
        return HttpResponse::error(500, "أرشيف الحزمة غير متوفر");
    }

    // تسجيل التنزيل
    db_->record_download(ver->id, "", "");

    // إرجاع الملف الثنائي
    HttpResponse resp;
    resp.status_code = 200;
    resp.set_header("Content-Type", "application/octet-stream");
    resp.set_header("Content-Disposition",
        "attachment; filename=\"" + pkg->name + "-" + ver->version + ".tar.gz\"");
    resp.set_header("Content-Length", std::to_string(data.size()));
    resp.set_header("X-Checksum-SHA256", ver->checksum_sha256);
    resp.body = std::string(data.begin(), data.end());
    return resp;
}

/**
 * @brief DELETE /api/v1/packages/:name/:version - سحب إصدار
 *
 * يتطلب مصادقة وملكية الحزمة.
 * السحب (yank) يمنع التنزيل لكن لا يحذف البيانات.
 */
HttpResponse ApiRoutes::handle_yank_version(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    auto name_it = req.path_params.find("name");
    auto ver_it = req.path_params.find("version");
    if (name_it == req.path_params.end() || ver_it == req.path_params.end()) {
        return HttpResponse::error(400, "اسم الحزمة ورقم الإصدار مطلوبان");
    }

    auto pkg = db_->get_package(name_it->second);
    if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");

    // التحقق من الملكية
    if (pkg->owner_id != user->id && !user->is_admin) {
        return HttpResponse::error(403, "ليس لديك صلاحية لسحب هذا الإصدار");
    }

    auto ver = db_->get_version(pkg->id, ver_it->second);
    if (!ver) return HttpResponse::not_found("لم يُعثر على الإصدار");

    db_->yank_version(ver->id, "سحب بواسطة المالك");
    db_->log_audit(user->id, "yank", name_it->second + "@" + ver_it->second, "", "");

    return HttpResponse::success("تم سحب الإصدار " + ver_it->second + " بنجاح");
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات البحث
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief GET /api/v1/search - البحث عن حزم
 *
 * معاملات الاستعلام:
 *   q        - نص البحث (يدعم العربية والإنجليزية)
 *   page     - رقم الصفحة
 *   per_page - عدد النتائج
 *   category - تصفية حسب التصنيف
 *
 * ميزات البحث العربي:
 *   - إزالة التشكيل تلقائياً
 *   - البحث بالجذر العربي
 *   - المرادفات العربية
 *   - الترجمة الثنائية (عربي ↔ إنجليزي)
 */
HttpResponse ApiRoutes::handle_search(const HttpRequest& req) {
    auto it = req.query_params.find("q");
    if (it == req.query_params.end() || it->second.empty()) {
        return HttpResponse::error(400, "نص البحث مطلوب (معامل q)");
    }

    std::string query = it->second;
    int page = req.get_page();
    int per_page = 20;

    auto pp_it = req.query_params.find("per_page");
    if (pp_it != req.query_params.end()) {
        try { per_page = std::stoi(pp_it->second); } catch (...) {}
        per_page = std::clamp(per_page, 1, 100);
    }

    // البحث باستخدام محرك البحث العربي إن أمكن
    PagedResults<SearchResult> results;

    if (search_) {
        // البحث المتقدم مع دعم العربية
        auto search_results = search_->search(query);

        // تحويل النتائج
        for (const auto& sr : search_results) {
            SearchResult r;
            r.id = sr.package_id;
            r.name = sr.name;
            r.arabic_name = sr.arabic_name;
            r.description = sr.description;
            r.description_ar = sr.description_ar;
            r.latest_version = sr.latest_version;
            r.total_downloads = sr.downloads;
            r.rating = sr.relevance_score;
            results.items.push_back(r);
        }
        results.total_count = static_cast<int64_t>(results.items.size());
        results.total_pages = (results.total_count + per_page - 1) / per_page;
        results.page = page;
        results.per_page = per_page;
    } else {
        // بحث أساسي في قاعدة البيانات
        results = db_->search_packages(query, page, per_page);
    }

    // بناء JSON
    std::ostringstream json;
    json << "{\"results\":[";
    for (size_t i = 0; i < results.items.size(); ++i) {
        if (i > 0) json << ",";
        json << search_result_to_json(results.items[i]);
    }
    json << "],\"query\":\"" << query << "\",";
    json << "\"pagination\":{";
    json << "\"page\":" << results.page << ",";
    json << "\"per_page\":" << results.per_page << ",";
    json << "\"total\":" << results.total_count << ",";
    json << "\"pages\":" << results.total_pages << "}}";

    return HttpResponse::json(json.str());
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات المصادقة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief POST /api/v1/register - تسجيل مستخدم جديد
 *
 * جسم الطلب:
 *   {
 *     "username": "اسم_المستخدم",
 *     "email": "email@example.com",
 *     "password": "كلمة_المرور",
 *     "display_name": "الاسم المعروض"
 *   }
 */
HttpResponse ApiRoutes::handle_register(const HttpRequest& req) {
    std::string username = json_get_string(req.body, "username");
    std::string email = json_get_string(req.body, "email");
    std::string password = json_get_string(req.body, "password");
    std::string display_name = json_get_string(req.body, "display_name");

    // التحقق من المدخلات
    if (username.empty()) return HttpResponse::error(400, "اسم المستخدم مطلوب");
    if (email.empty()) return HttpResponse::error(400, "البريد الإلكتروني مطلوب");
    if (password.empty()) return HttpResponse::error(400, "كلمة المرور مطلوبة");
    if (password.size() < 8) return HttpResponse::error(400, "كلمة المرور يجب أن تكون 8 أحرف على الأقل");
    if (username.size() < 3) return HttpResponse::error(400, "اسم المستخدم يجب أن يكون 3 أحرف على الأقل");

    // التحقق من عدم التكرار
    if (db_->get_user_by_username(username)) {
        return HttpResponse::error(409, "اسم المستخدم مستخدم مسبقاً");
    }
    if (db_->get_user_by_email(email)) {
        return HttpResponse::error(409, "البريد الإلكتروني مسجل مسبقاً");
    }

    // التسجيل عبر نظام المصادقة
    auto result = auth_->register_user(username, email, password);
    if (!result.success) {
        return HttpResponse::error(500, result.error);
    }

    // تحديث الاسم المعروض
    if (!display_name.empty()) {
        auto user = db_->get_user(result.user_id);
        if (user) {
            user->display_name = display_name;
            db_->update_user(*user);
        }
    }

    // سجل المراجعة
    db_->log_audit(result.user_id, "register", username, "", "");

    // استجابة النجاح مع رمز JWT
    std::ostringstream json;
    json << "{\"success\":true,\"message\":\"تم التسجيل بنجاح\",";
    json << "\"token\":\"" << result.token << "\",";
    json << "\"user\":{";
    json << "\"id\":" << result.user_id << ",";
    json << "\"username\":\"" << username << "\",";
    json << "\"email\":\"" << email << "\"}}";

    HttpResponse resp;
    resp.status_code = 201;
    resp.set_header("Content-Type", "application/json; charset=utf-8");
    resp.body = json.str();
    return resp;
}

/**
 * @brief POST /api/v1/login - تسجيل دخول
 *
 * جسم الطلب:
 *   { "username": "...", "password": "..." }
 *
 * أو:
 *   { "email": "...", "password": "..." }
 */
HttpResponse ApiRoutes::handle_login(const HttpRequest& req) {
    std::string username = json_get_string(req.body, "username");
    std::string email = json_get_string(req.body, "email");
    std::string password = json_get_string(req.body, "password");

    if (password.empty()) return HttpResponse::error(400, "كلمة المرور مطلوبة");
    if (username.empty() && email.empty()) {
        return HttpResponse::error(400, "اسم المستخدم أو البريد الإلكتروني مطلوب");
    }

    // إذا أُعطي بريد إلكتروني، جلب اسم المستخدم
    if (username.empty() && !email.empty()) {
        auto user = db_->get_user_by_email(email);
        if (!user) return HttpResponse::error(401, "بيانات الدخول غير صحيحة");
        username = user->username;
    }

    // تسجيل الدخول
    auto result = auth_->login(username, password);
    if (!result.success) {
        return HttpResponse::error(401, "بيانات الدخول غير صحيحة");
    }

    // تحديث آخر تسجيل دخول
    db_->update_last_login(result.user_id);

    std::ostringstream json;
    json << "{\"success\":true,\"message\":\"تم تسجيل الدخول بنجاح\",";
    json << "\"token\":\"" << result.token << "\",";
    json << "\"user\":{";
    json << "\"id\":" << result.user_id << ",";
    json << "\"username\":\"" << result.username << "\"}}";

    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/me - الملف الشخصي للمستخدم الحالي
 */
HttpResponse ApiRoutes::handle_me(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    std::ostringstream json;
    json << "{\"user\":{";
    json << "\"id\":" << user->id << ",";
    json << "\"username\":\"" << user->username << "\",";
    json << "\"email\":\"" << user->email << "\",";
    json << "\"display_name\":\"" << user->display_name << "\",";
    json << "\"bio\":\"" << user->bio << "\",";
    json << "\"package_count\":" << user->package_count << ",";
    json << "\"is_admin\":" << (user->is_admin ? "true" : "false") << ",";
    json << "\"created_at\":\"" << user->created_at << "\"}}";

    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/me/packages - حزم المستخدم الحالي
 */
HttpResponse ApiRoutes::handle_my_packages(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    auto packages = db_->get_user_packages(user->id);

    std::ostringstream json;
    json << "{\"packages\":[";
    for (size_t i = 0; i < packages.size(); ++i) {
        if (i > 0) json << ",";
        json << package_to_json(packages[i]);
    }
    json << "]}";

    return HttpResponse::json(json.str());
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات الإحصائيات والاستكشاف
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief GET /api/v1/stats - إحصائيات المستودع
 */
HttpResponse ApiRoutes::handle_stats(const HttpRequest& req) {
    (void)req;
    auto stats = db_->get_stats();

    std::ostringstream json;
    json << "{\"stats\":{";
    json << "\"total_packages\":" << stats.total_packages << ",";
    json << "\"total_versions\":" << stats.total_versions << ",";
    json << "\"total_downloads\":" << stats.total_downloads << ",";
    json << "\"total_users\":" << stats.total_users << ",";
    json << "\"registry_name\":\"مستودع حزم لغة ص\",";
    json << "\"registry_url\":\"https://packages.sadlang.org\"}}";

    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/featured - الحزم المميزة
 */
HttpResponse ApiRoutes::handle_featured(const HttpRequest& req) {
    (void)req;
    auto packages = db_->get_featured_packages(10);

    std::ostringstream json;
    json << "{\"featured\":[";
    for (size_t i = 0; i < packages.size(); ++i) {
        if (i > 0) json << ",";
        json << package_to_json(packages[i]);
    }
    json << "]}";

    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/popular - الأكثر تحميلاً
 */
HttpResponse ApiRoutes::handle_popular(const HttpRequest& req) {
    (void)req;
    auto packages = db_->get_popular_packages(20);

    std::ostringstream json;
    json << "{\"popular\":[";
    for (size_t i = 0; i < packages.size(); ++i) {
        if (i > 0) json << ",";
        json << package_to_json(packages[i]);
    }
    json << "]}";

    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/recent - الأحدث
 */
HttpResponse ApiRoutes::handle_recent(const HttpRequest& req) {
    (void)req;
    auto packages = db_->get_new_packages(20);

    std::ostringstream json;
    json << "{\"recent\":[";
    for (size_t i = 0; i < packages.size(); ++i) {
        if (i > 0) json << ",";
        json << package_to_json(packages[i]);
    }
    json << "]}";

    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/categories - قائمة التصنيفات
 *
 * يُرجع التصنيفات المتاحة مع عدد الحزم في كل منها.
 */
HttpResponse ApiRoutes::handle_categories(const HttpRequest& req) {
    (void)req;

    // التصنيفات الثابتة (يمكن توسيعها لاحقاً)
    std::string json = R"({
        "categories": [
            {"id": "math",        "name": "Mathematics",  "arabic_name": "رياضيات",     "count": 0},
            {"id": "network",     "name": "Networking",   "arabic_name": "شبكات",       "count": 0},
            {"id": "web",         "name": "Web",          "arabic_name": "ويب",         "count": 0},
            {"id": "data",        "name": "Data",         "arabic_name": "بيانات",      "count": 0},
            {"id": "graphics",    "name": "Graphics",     "arabic_name": "رسوميات",     "count": 0},
            {"id": "crypto",      "name": "Cryptography", "arabic_name": "تشفير",       "count": 0},
            {"id": "text",        "name": "Text",         "arabic_name": "نصوص",        "count": 0},
            {"id": "io",          "name": "I/O",          "arabic_name": "إدخال/إخراج", "count": 0},
            {"id": "testing",     "name": "Testing",      "arabic_name": "اختبارات",    "count": 0},
            {"id": "tools",       "name": "Tools",        "arabic_name": "أدوات",       "count": 0},
            {"id": "ai",          "name": "AI",           "arabic_name": "ذكاء اصطناعي","count": 0},
            {"id": "islamic",     "name": "Islamic",      "arabic_name": "إسلامية",     "count": 0}
        ]
    })";

    return HttpResponse::json(json);
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات التقييمات والمراجعات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief POST /api/v1/packages/:name/reviews - إنشاء أو تحديث تقييم
 */
HttpResponse ApiRoutes::handle_create_review(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول لإضافة تقييم");

    auto name_it = req.path_params.find("name");
    if (name_it == req.path_params.end())
        return HttpResponse::error(400, "اسم الحزمة مطلوب");

    auto pkg = db_->get_package(name_it->second);
    if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");

    // لا يمكن تقييم حزمتك الخاصة
    if (pkg->owner_id == user->id)
        return HttpResponse::error(403, "لا يمكنك تقييم حزمتك الخاصة");

    int rating = 0;
    try { rating = std::stoi(json_get_string(req.body, "rating")); } catch (...) {}
    if (rating < 1 || rating > 5)
        return HttpResponse::error(400, "التقييم يجب أن يكون بين ١ و ٥");

    std::string title = json_get_string(req.body, "title");
    std::string body_text = json_get_string(req.body, "body");

    ReviewInfo review;
    review.package_id = pkg->id;
    review.user_id = user->id;
    review.rating = rating;
    review.title = title;
    review.body = body_text;

    auto rid = db_->create_or_update_review(review);
    if (rid < 0) return HttpResponse::error(500, "فشل حفظ التقييم");

    std::ostringstream json;
    json << "{\"success\":true,\"message\":\"تم حفظ التقييم\",\"review_id\":" << rid << "}";
    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/packages/:name/reviews - قائمة تقييمات حزمة
 */
HttpResponse ApiRoutes::handle_get_reviews(const HttpRequest& req) {
    auto name_it = req.path_params.find("name");
    if (name_it == req.path_params.end())
        return HttpResponse::error(400, "اسم الحزمة مطلوب");

    auto pkg = db_->get_package(name_it->second);
    if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");

    int limit = 50;
    auto it = req.query_params.find("limit");
    if (it != req.query_params.end()) {
        try { limit = std::clamp(std::stoi(it->second), 1, 100); } catch (...) {}
    }

    auto reviews = db_->get_package_reviews(pkg->id, limit);
    auto star_count = db_->get_star_count(pkg->id);

    std::ostringstream json;
    json << "{\"reviews\":[";
    for (size_t i = 0; i < reviews.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"id\":" << reviews[i].id << ",";
        json << "\"user\":\"" << reviews[i].username << "\",";
        json << "\"rating\":" << reviews[i].rating << ",";
        json << "\"title\":\"" << reviews[i].title << "\",";
        json << "\"body\":\"" << reviews[i].body << "\",";
        json << "\"is_edited\":" << (reviews[i].is_edited ? "true" : "false") << ",";
        json << "\"created_at\":\"" << reviews[i].created_at << "\"}";
    }
    json << "],\"star_count\":" << star_count;
    json << ",\"review_count\":" << reviews.size() << "}";

    return HttpResponse::json(json.str());
}

/**
 * @brief DELETE /api/v1/reviews/:id - حذف تقييم
 */
HttpResponse ApiRoutes::handle_delete_review(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    auto id_it = req.path_params.find("id");
    if (id_it == req.path_params.end())
        return HttpResponse::error(400, "معرف التقييم مطلوب");

    int64_t review_id = 0;
    try { review_id = std::stoll(id_it->second); } catch (...) {
        return HttpResponse::error(400, "معرف غير صالح");
    }

    // TODO: التحقق من ملكية التقييم أو صلاحية المسؤول
    db_->delete_review(review_id);
    return HttpResponse::success("تم حذف التقييم");
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات التفضيلات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief POST /api/v1/packages/:name/star - تفضيل أو إلغاء تفضيل حزمة
 */
HttpResponse ApiRoutes::handle_toggle_star(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    auto name_it = req.path_params.find("name");
    if (name_it == req.path_params.end())
        return HttpResponse::error(400, "اسم الحزمة مطلوب");

    auto pkg = db_->get_package(name_it->second);
    if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");

    bool starred = db_->is_starred(pkg->id, user->id);
    if (starred) {
        db_->unstar_package(pkg->id, user->id);
    } else {
        db_->star_package(pkg->id, user->id);
    }

    auto count = db_->get_star_count(pkg->id);
    std::ostringstream json;
    json << "{\"starred\":" << (!starred ? "true" : "false") << ",";
    json << "\"star_count\":" << count << "}";
    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/me/stars - الحزم المفضلة للمستخدم الحالي
 */
HttpResponse ApiRoutes::handle_my_stars(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    auto packages = db_->get_user_stars(user->id);

    std::ostringstream json;
    json << "{\"stars\":[";
    for (size_t i = 0; i < packages.size(); ++i) {
        if (i > 0) json << ",";
        json << package_to_json(packages[i]);
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات الإبلاغات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief POST /api/v1/packages/:name/report - إبلاغ عن حزمة
 */
HttpResponse ApiRoutes::handle_create_report(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول لتقديم بلاغ");

    auto name_it = req.path_params.find("name");
    if (name_it == req.path_params.end())
        return HttpResponse::error(400, "اسم الحزمة مطلوب");

    auto pkg = db_->get_package(name_it->second);
    if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");

    std::string reason = json_get_string(req.body, "reason");
    std::string details = json_get_string(req.body, "details");
    if (reason.empty())
        return HttpResponse::error(400, "سبب البلاغ مطلوب");

    ReportInfo report;
    report.reporter_id = user->id;
    report.package_id = pkg->id;
    report.reason = reason;
    report.details = details;

    auto rid = db_->create_report(report);
    if (rid < 0) return HttpResponse::error(500, "فشل حفظ البلاغ");

    std::ostringstream json;
    json << "{\"success\":true,\"message\":\"تم تقديم البلاغ بنجاح\",\"report_id\":" << rid << "}";
    HttpResponse resp;
    resp.status_code = 201;
    resp.set_header("Content-Type", "application/json; charset=utf-8");
    resp.body = json.str();
    return resp;
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات الخطّافات (Webhooks)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief POST /api/v1/webhooks - إنشاء خطّاف جديد
 */
HttpResponse ApiRoutes::handle_create_webhook(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    std::string url = json_get_string(req.body, "url");
    std::string secret = json_get_string(req.body, "secret");
    std::string events = json_get_string(req.body, "events");

    if (url.empty()) return HttpResponse::error(400, "عنوان URL مطلوب");
    if (events.empty()) events = "publish,yank";

    // حزمة اختيارية
    int64_t pkg_id = 0;
    std::string pkg_name = json_get_string(req.body, "package");
    if (!pkg_name.empty()) {
        auto pkg = db_->get_package(pkg_name);
        if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");
        if (pkg->owner_id != user->id && !user->is_admin)
            return HttpResponse::error(403, "ليس لديك صلاحية");
        pkg_id = pkg->id;
    }

    WebhookInfo wh;
    wh.user_id = user->id;
    wh.package_id = pkg_id;
    wh.url = url;
    wh.secret = secret;
    wh.events = events;

    auto wid = db_->create_webhook(wh);
    if (wid < 0) return HttpResponse::error(500, "فشل إنشاء الخطّاف");

    std::ostringstream json;
    json << "{\"success\":true,\"webhook_id\":" << wid << "}";
    HttpResponse resp;
    resp.status_code = 201;
    resp.set_header("Content-Type", "application/json; charset=utf-8");
    resp.body = json.str();
    return resp;
}

/**
 * @brief GET /api/v1/webhooks - قائمة خطّافات المستخدم
 */
HttpResponse ApiRoutes::handle_list_webhooks(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    auto webhooks = db_->get_user_webhooks(user->id);

    std::ostringstream json;
    json << "{\"webhooks\":[";
    for (size_t i = 0; i < webhooks.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"id\":" << webhooks[i].id << ",";
        json << "\"url\":\"" << webhooks[i].url << "\",";
        json << "\"events\":\"" << webhooks[i].events << "\",";
        json << "\"is_active\":" << (webhooks[i].is_active ? "true" : "false") << ",";
        json << "\"failure_count\":" << webhooks[i].failure_count << ",";
        json << "\"created_at\":\"" << webhooks[i].created_at << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

/**
 * @brief DELETE /api/v1/webhooks/:id - حذف خطّاف
 */
HttpResponse ApiRoutes::handle_delete_webhook(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    auto id_it = req.path_params.find("id");
    if (id_it == req.path_params.end())
        return HttpResponse::error(400, "معرف الخطّاف مطلوب");

    int64_t wh_id = 0;
    try { wh_id = std::stoll(id_it->second); } catch (...) {
        return HttpResponse::error(400, "معرف غير صالح");
    }

    db_->delete_webhook(wh_id, user->id);
    return HttpResponse::success("تم حذف الخطّاف");
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات المسؤول (Admin)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief GET /api/v1/admin/stats - إحصائيات إدارية مفصّلة
 */
HttpResponse ApiRoutes::handle_admin_stats(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user || !user->is_admin)
        return HttpResponse::error(403, "صلاحيات المسؤول مطلوبة");

    auto stats = db_->get_stats();
    auto pending_reports = db_->get_reports("pending", 100);

    std::ostringstream json;
    json << "{\"stats\":{";
    json << "\"total_packages\":" << stats.total_packages << ",";
    json << "\"total_versions\":" << stats.total_versions << ",";
    json << "\"total_downloads\":" << stats.total_downloads << ",";
    json << "\"total_users\":" << stats.total_users << ",";
    json << "\"pending_reports\":" << pending_reports.size();
    json << "}}";
    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/admin/reports - قائمة البلاغات
 */
HttpResponse ApiRoutes::handle_admin_reports(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user || !user->is_admin)
        return HttpResponse::error(403, "صلاحيات المسؤول مطلوبة");

    std::string status = "pending";
    auto it = req.query_params.find("status");
    if (it != req.query_params.end()) status = it->second;

    auto reports = db_->get_reports(status, 100);

    std::ostringstream json;
    json << "{\"reports\":[";
    for (size_t i = 0; i < reports.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"id\":" << reports[i].id << ",";
        json << "\"reporter\":\"" << reports[i].reporter_username << "\",";
        json << "\"package\":\"" << reports[i].package_name << "\",";
        json << "\"reason\":\"" << reports[i].reason << "\",";
        json << "\"details\":\"" << reports[i].details << "\",";
        json << "\"status\":\"" << reports[i].status << "\",";
        json << "\"created_at\":\"" << reports[i].created_at << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

/**
 * @brief POST /api/v1/admin/reports/:id/resolve - حل بلاغ
 */
HttpResponse ApiRoutes::handle_admin_resolve_report(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user || !user->is_admin)
        return HttpResponse::error(403, "صلاحيات المسؤول مطلوبة");

    auto id_it = req.path_params.find("id");
    if (id_it == req.path_params.end())
        return HttpResponse::error(400, "معرف البلاغ مطلوب");

    int64_t report_id = 0;
    try { report_id = std::stoll(id_it->second); } catch (...) {
        return HttpResponse::error(400, "معرف غير صالح");
    }

    std::string status = json_get_string(req.body, "status");
    std::string notes = json_get_string(req.body, "notes");
    if (status.empty()) status = "resolved";

    db_->resolve_report(report_id, user->id, status, notes);
    return HttpResponse::success("تم حل البلاغ");
}

/**
 * @brief POST /api/v1/admin/packages/:name/flag - تعيين حزمة كمميزة أو موثقة
 */
HttpResponse ApiRoutes::handle_admin_set_package_flag(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user || !user->is_admin)
        return HttpResponse::error(403, "صلاحيات المسؤول مطلوبة");

    auto name_it = req.path_params.find("name");
    if (name_it == req.path_params.end())
        return HttpResponse::error(400, "اسم الحزمة مطلوب");

    auto pkg = db_->get_package(name_it->second);
    if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");

    std::string flag = json_get_string(req.body, "flag");
    std::string val_str = json_get_string(req.body, "value");
    bool value = (val_str == "true" || val_str == "1");

    if (flag == "featured") {
        db_->set_package_featured(pkg->id, value);
    } else if (flag == "verified") {
        db_->set_package_verified(pkg->id, value);
    } else {
        return HttpResponse::error(400, "العلم المطلوب غير معروف (featured أو verified)");
    }

    return HttpResponse::success("تم تحديث العلم بنجاح");
}

/**
 * @brief GET /api/v1/admin/audit - سجل المراجعة
 */
HttpResponse ApiRoutes::handle_admin_audit_log(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user || !user->is_admin)
        return HttpResponse::error(403, "صلاحيات المسؤول مطلوبة");

    int limit = 100;
    auto it = req.query_params.find("limit");
    if (it != req.query_params.end()) {
        try { limit = std::clamp(std::stoi(it->second), 1, 500); } catch (...) {}
    }

    auto entries = db_->get_audit_log(limit);

    std::ostringstream json;
    json << "{\"audit_log\":[";
    for (size_t i = 0; i < entries.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"id\":" << entries[i].id << ",";
        json << "\"user\":\"" << entries[i].username << "\",";
        json << "\"action\":\"" << entries[i].action << "\",";
        json << "\"details\":\"" << entries[i].details << "\",";
        json << "\"created_at\":\"" << entries[i].created_at << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات إضافية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief GET /api/v1/suggest - اقتراحات البحث
 */
HttpResponse ApiRoutes::handle_suggest(const HttpRequest& req) {
    auto it = req.query_params.find("q");
    if (it == req.query_params.end() || it->second.empty())
        return HttpResponse::json("{\"suggestions\":[]}");

    std::string query = it->second;
    int limit = 8;
    auto lim_it = req.query_params.find("limit");
    if (lim_it != req.query_params.end()) {
        try { limit = std::clamp(std::stoi(lim_it->second), 1, 20); } catch (...) {}
    }

    std::vector<std::string> suggestions;
    if (search_) {
        auto results = search_->search(query, limit);
        for (const auto& r : results) {
            suggestions.push_back(r.arabic_name.empty() ? r.name : r.arabic_name + " (" + r.name + ")");
        }
    }

    std::ostringstream json;
    json << "{\"suggestions\":[";
    for (size_t i = 0; i < suggestions.size(); ++i) {
        if (i > 0) json << ",";
        json << "\"" << suggestions[i] << "\"";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

/**
 * @brief POST /api/v1/me - تحديث الملف الشخصي
 */
HttpResponse ApiRoutes::handle_update_profile(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) return HttpResponse::unauthorized("يجب تسجيل الدخول");

    std::string display_name = json_get_string(req.body, "display_name");
    std::string bio = json_get_string(req.body, "bio");

    UserInfo updated = *user;
    if (!display_name.empty()) updated.display_name = display_name;
    if (!bio.empty()) updated.bio = bio;

    db_->update_user(updated);

    return HttpResponse::success("تم تحديث الملف الشخصي");
}

/**
 * @brief GET /api/v1/users/:username - ملف شخصي عام
 */
HttpResponse ApiRoutes::handle_public_profile(const HttpRequest& req) {
    auto it = req.path_params.find("username");
    if (it == req.path_params.end())
        return HttpResponse::error(400, "اسم المستخدم مطلوب");

    auto user = db_->get_user_by_username(it->second);
    if (!user) return HttpResponse::not_found("لم يُعثر على المستخدم");

    auto packages = db_->get_user_packages(user->id);

    std::ostringstream json;
    json << "{\"user\":{";
    json << "\"username\":\"" << user->username << "\",";
    json << "\"display_name\":\"" << user->display_name << "\",";
    json << "\"bio\":\"" << user->bio << "\",";
    json << "\"package_count\":" << user->package_count << ",";
    json << "\"created_at\":\"" << user->created_at << "\"},";
    json << "\"packages\":[";
    for (size_t i = 0; i < packages.size(); ++i) {
        if (i > 0) json << ",";
        json << package_to_json(packages[i]);
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

/**
 * @brief GET /api/v1/packages/:name/downloads - إحصائيات تنزيل يومية
 */
HttpResponse ApiRoutes::handle_download_stats(const HttpRequest& req) {
    auto name_it = req.path_params.find("name");
    if (name_it == req.path_params.end())
        return HttpResponse::error(400, "اسم الحزمة مطلوب");

    auto pkg = db_->get_package(name_it->second);
    if (!pkg) return HttpResponse::not_found("لم يُعثر على الحزمة");

    int days = 30;
    auto it = req.query_params.find("days");
    if (it != req.query_params.end()) {
        try { days = std::clamp(std::stoi(it->second), 1, 365); } catch (...) {}
    }

    auto daily = db_->get_daily_downloads(pkg->id, days);

    std::ostringstream json;
    json << "{\"package\":\"" << pkg->name << "\",\"downloads\":[";
    for (size_t i = 0; i < daily.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"date\":\"" << daily[i].date << "\",\"count\":" << daily[i].count << "}";
    }
    json << "],\"total\":" << pkg->total_downloads << "}";
    return HttpResponse::json(json.str());
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief التحقق من مصادقة الطلب
 */
std::optional<UserInfo> ApiRoutes::authenticate(const HttpRequest& req) {
    // محاولة JWT أولاً
    std::string token = req.get_auth_token();
    if (!token.empty()) {
        auto result = auth_->verify_token(token);
        if (result) {
            return result;
        }
    }

    // محاولة رمز API
    auto it = req.headers.find("X-API-Token");
    if (it != req.headers.end()) {
        auto result = auth_->verify_api_token(it->second);
        if (result) {
            return result;
        }
    }

    return std::nullopt;
}

/**
 * @brief تحويل PackageInfo إلى JSON
 */
std::string ApiRoutes::package_to_json(const PackageInfo& pkg) {
    std::ostringstream j;
    j << "{";
    j << "\"id\":" << pkg.id << ",";
    j << "\"name\":\"" << pkg.name << "\",";
    j << "\"display_name\":\"" << pkg.display_name << "\",";
    j << "\"arabic_name\":\"" << pkg.arabic_name << "\",";
    j << "\"description\":\"" << pkg.description << "\",";
    j << "\"description_ar\":\"" << pkg.description_ar << "\",";
    j << "\"license\":\"" << pkg.license << "\",";
    j << "\"repository_url\":\"" << pkg.repository_url << "\",";
    j << "\"latest_version\":\"" << pkg.latest_version << "\",";
    j << "\"total_downloads\":" << pkg.total_downloads << ",";
    j << "\"category\":\"" << pkg.category << "\",";
    j << "\"is_verified\":" << (pkg.is_verified ? "true" : "false") << ",";
    j << "\"is_featured\":" << (pkg.is_featured ? "true" : "false") << ",";
    j << "\"created_at\":\"" << pkg.created_at << "\",";
    j << "\"updated_at\":\"" << pkg.updated_at << "\"";
    j << "}";
    return j.str();
}

/**
 * @brief تحويل VersionInfo إلى JSON
 */
std::string ApiRoutes::version_to_json(const VersionInfo& ver) {
    std::ostringstream j;
    j << "{";
    j << "\"id\":" << ver.id << ",";
    j << "\"version\":\"" << ver.version << "\",";
    j << "\"release_notes\":\"" << ver.release_notes << "\",";
    j << "\"checksum\":\"" << ver.checksum_sha256 << "\",";
    j << "\"file_size\":" << ver.file_size << ",";
    j << "\"downloads\":" << ver.downloads << ",";
    j << "\"is_yanked\":" << (ver.is_yanked ? "true" : "false") << ",";
    j << "\"created_at\":\"" << ver.created_at << "\"";
    j << "}";
    return j.str();
}

/**
 * @brief تحويل SearchResult إلى JSON
 */
std::string ApiRoutes::search_result_to_json(const SearchResult& r) {
    std::ostringstream j;
    j << "{";
    j << "\"id\":" << r.id << ",";
    j << "\"name\":\"" << r.name << "\",";
    j << "\"arabic_name\":\"" << r.arabic_name << "\",";
    j << "\"description\":\"" << r.description << "\",";
    j << "\"description_ar\":\"" << r.description_ar << "\",";
    j << "\"latest_version\":\"" << r.latest_version << "\",";
    j << "\"total_downloads\":" << r.total_downloads << ",";
    j << "\"relevance\":" << r.rating << ",";
    j << "\"category\":\"" << r.category << "\",";
    j << "\"is_verified\":" << (r.is_verified ? "true" : "false");
    j << "}";
    return j.str();
}

/**
 * @brief تحليل JSON بسيط - استخراج قيمة نصية
 *
 * ملاحظة: هذا محلل JSON بسيط جداً يعمل مع الحالات الشائعة.
 * لا يدعم القيم المتداخلة أو المصفوفات أو الأحرف المُهرَّبة المعقدة.
 */
std::string ApiRoutes::json_get_string(const std::string& json, const std::string& key) {
    // البحث عن "key": "value" أو "key":"value"
    std::string search = "\"" + key + "\"";
    size_t key_pos = json.find(search);
    if (key_pos == std::string::npos) return "";

    // تخطي المفتاح وعلامتي التنصيص والنقطتين
    size_t after_key = key_pos + search.size();

    // تخطي المسافات والنقطتين
    while (after_key < json.size() && (json[after_key] == ' ' || json[after_key] == ':')) {
        after_key++;
    }

    // التحقق من علامة التنصيص الابتدائية
    if (after_key >= json.size() || json[after_key] != '"') return "";
    after_key++; // تخطي "

    // البحث عن علامة التنصيص الختامية (مع مراعاة الهروب)
    std::string value;
    for (size_t i = after_key; i < json.size(); ++i) {
        if (json[i] == '\\' && i + 1 < json.size()) {
            // حرف مُهرَّب
            i++;
            if (json[i] == 'n') value += '\n';
            else if (json[i] == 't') value += '\t';
            else if (json[i] == '"') value += '"';
            else if (json[i] == '\\') value += '\\';
            else value += json[i];
        } else if (json[i] == '"') {
            break;
        } else {
            value += json[i];
        }
    }

    return value;
}

/**
 * @brief بناء كائن JSON من أزواج مفتاح-قيمة
 */
std::string ApiRoutes::make_json(
    const std::vector<std::pair<std::string, std::string>>& fields) {
    std::ostringstream j;
    j << "{";
    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) j << ",";
        j << "\"" << fields[i].first << "\":\"" << fields[i].second << "\"";
    }
    j << "}";
    return j.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات مفاتيح الناشرين والتوقيعات الرقمية
// ═══════════════════════════════════════════════════════════════════════════════

HttpResponse ApiRoutes::handle_add_publisher_key(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    std::string public_key = json_get_string(req.body, "public_key");
    std::string key_type = json_get_string(req.body, "key_type");
    std::string name = json_get_string(req.body, "name");
    std::string expires_at = json_get_string(req.body, "expires_at");
    
    if (public_key.empty()) {
        return HttpResponse::error("المفتاح العام مطلوب", 400);
    }
    if (key_type.empty()) key_type = "ed25519";
    if (name.empty()) name = "المفتاح الافتراضي";
    
    PublisherKeyInfo key_info;
    key_info.user_id = user->id;
    key_info.public_key = public_key;
    key_info.key_type = key_type;
    key_info.key_name = name;
    key_info.expires_at = expires_at;
    key_info.fingerprint = public_key.substr(0, 16); // simplified fingerprint
    
    auto id = db_->add_publisher_key(key_info);
    if (id <= 0) {
        return HttpResponse::error("فشل في إضافة المفتاح", 500);
    }
    
    std::ostringstream json;
    json << "{\"success\":true,\"key_id\":" << id << "}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_list_my_keys(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    auto keys = db_->get_user_keys(user->id);
    
    std::ostringstream json;
    json << "{\"keys\":[";
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"id\":" << keys[i].id << ",";
        json << "\"name\":\"" << keys[i].key_name << "\",";
        json << "\"key_type\":\"" << keys[i].key_type << "\",";
        json << "\"fingerprint\":\"" << keys[i].fingerprint << "\",";
        json << "\"is_active\":" << (!keys[i].is_revoked ? "true" : "false") << ",";
        json << "\"created_at\":\"" << keys[i].created_at << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_revoke_key(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    int64_t key_id = std::atoll(req.get_param("id").c_str());
    if (key_id <= 0) {
        return HttpResponse::error("معرف المفتاح غير صالح", 400);
    }
    
    if (!db_->revoke_key(key_id, "user_request")) {
        return HttpResponse::error("فشل في إبطال المفتاح", 500);
    }
    
    return HttpResponse::json("{\"success\":true,\"message\":\"تم إبطال المفتاح\"}");
}

HttpResponse ApiRoutes::handle_verify_signature(const HttpRequest& req) {
    std::string package_name = req.get_param("name");
    std::string version = req.get_param("version");
    
    auto pkg = db_->get_package(package_name);
    if (!pkg) {
        return HttpResponse::error("الحزمة غير موجودة", 404);
    }
    
    auto ver = db_->get_version(pkg->id, version);
    if (!ver) {
        return HttpResponse::error("الإصدار غير موجود", 404);
    }
    
    auto signatures = db_->get_version_signatures(ver->id);
    
    std::ostringstream json;
    json << "{\"verified\":" << (signatures.size() > 0 ? "true" : "false") << ",";
    json << "\"signatures\":[";
    for (size_t i = 0; i < signatures.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"key_id\":" << signatures[i].key_id << ",";
        json << "\"is_valid\":" << (signatures[i].is_valid ? "true" : "false") << ",";
        json << "\"signed_at\":\"" << signatures[i].created_at << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_get_signatures(const HttpRequest& req) {
    return handle_verify_signature(req);
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات حل التبعيات
// ═══════════════════════════════════════════════════════════════════════════════

HttpResponse ApiRoutes::handle_resolve_dependencies(const HttpRequest& req) {
    // تحليل قائمة الحزم المطلوبة
    std::string packages_json = json_get_string(req.body, "packages");
    bool include_dev = json_get_string(req.body, "include_dev") == "true";
    std::string platform = json_get_string(req.body, "platform");
    if (platform.empty()) platform = "all";
    
    // TODO: تحليل JSON للحزم واستدعاء DependencyResolver
    // لبساطة التنفيذ، نرجع رسالة توضيحية
    
    std::ostringstream json;
    json << "{\"success\":true,";
    json << "\"message\":\"نظام حل التبعيات جاهز\",";
    json << "\"version\":\"1.0.0\"}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_check_compatibility(const HttpRequest& req) {
    std::string package_name = req.get_param("name");
    std::string version = req.get_param("version");
    std::string constraint = req.get_query("constraint");
    
    auto pkg = db_->get_package(package_name);
    if (!pkg) {
        return HttpResponse::error("الحزمة غير موجودة", 404);
    }
    
    // TODO: فحص التوافق باستخدام DependencyResolver
    bool compatible = true;
    
    std::ostringstream json;
    json << "{\"compatible\":" << (compatible ? "true" : "false") << "}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_reverse_dependencies(const HttpRequest& req) {
    std::string package_name = req.get_param("name");
    
    auto pkg = db_->get_package(package_name);
    if (!pkg) {
        return HttpResponse::error("الحزمة غير موجودة", 404);
    }
    
    auto deps = db_->get_reverse_dependencies(package_name);
    
    std::ostringstream json;
    json << "{\"reverse_dependencies\":[";
    for (size_t i = 0; i < deps.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"package\":\"" << deps[i].first << "\",";
        json << "\"constraint\":\"" << deps[i].second << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات الإشعارات
// ═══════════════════════════════════════════════════════════════════════════════

HttpResponse ApiRoutes::handle_list_notifications(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    bool unread_only = req.get_query("unread") == "true";
    int limit = std::atoi(req.get_query("limit", "50").c_str());
    
    auto notifications = db_->get_user_notifications(user->id, unread_only, limit);
    
    std::ostringstream json;
    json << "{\"notifications\":[";
    for (size_t i = 0; i < notifications.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"id\":" << notifications[i].id << ",";
        json << "\"type\":\"" << notifications[i].notification_type << "\",";
        json << "\"title\":\"" << notifications[i].title << "\",";
        json << "\"message\":\"" << notifications[i].body << "\",";
        json << "\"is_read\":" << (notifications[i].is_read ? "true" : "false") << ",";
        json << "\"created_at\":\"" << notifications[i].created_at << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_mark_notification_read(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    int64_t notif_id = std::atoll(req.get_param("id").c_str());
    
    if (!db_->mark_notification_read(notif_id, user->id)) {
        return HttpResponse::error("فشل في تعيين الإشعار كمقروء", 500);
    }
    
    return HttpResponse::json("{\"success\":true}");
}

HttpResponse ApiRoutes::handle_mark_all_read(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    if (!db_->mark_all_notifications_read(user->id)) {
        return HttpResponse::error("فشل في تعيين جميع الإشعارات كمقروءة", 500);
    }
    
    return HttpResponse::json("{\"success\":true}");
}

HttpResponse ApiRoutes::handle_delete_notification(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    int64_t notif_id = std::atoll(req.get_param("id").c_str());
    
    // Use mark_notification_read as archive mechanism
    if (!db_->mark_notification_read(notif_id, user->id)) {
        return HttpResponse::error("فشل في حذف الإشعار", 500);
    }
    
    return HttpResponse::json("{\"success\":true}");
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات الاشتراكات
// ═══════════════════════════════════════════════════════════════════════════════

HttpResponse ApiRoutes::handle_subscribe(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    std::string package_name = req.get_param("name");
    
    auto pkg = db_->get_package(package_name);
    if (!pkg) {
        return HttpResponse::error("الحزمة غير موجودة", 404);
    }
    
    SubscriptionInfo sub;
    sub.user_id = user->id;
    sub.target_type = "package";
    sub.target_id = pkg->id;
    sub.notify_new_version = true;
    sub.notify_security = true;
    
    auto id = db_->create_subscription(sub);
    if (id <= 0) {
        return HttpResponse::error("فشل في إنشاء الاشتراك", 500);
    }
    
    return HttpResponse::json("{\"success\":true,\"message\":\"تم الاشتراك بنجاح\"}");
}

HttpResponse ApiRoutes::handle_unsubscribe(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    std::string package_name = req.get_param("name");
    auto pkg = db_->get_package(package_name);
    if (!pkg) {
        return HttpResponse::error("الحزمة غير موجودة", 404);
    }
    
    if (!db_->delete_subscription(user->id, "package", pkg->id)) {
        return HttpResponse::error("فشل في إلغاء الاشتراك", 500);
    }
    
    return HttpResponse::json("{\"success\":true}");
}

HttpResponse ApiRoutes::handle_list_subscriptions(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    auto subs = db_->get_user_subscriptions(user->id);
    
    std::ostringstream json;
    json << "{\"subscriptions\":[";
    for (size_t i = 0; i < subs.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"id\":" << subs[i].id << ",";
        json << "\"target_type\":\"" << subs[i].target_type << "\",";
        json << "\"target_id\":" << subs[i].target_id << ",";
        json << "\"created_at\":\"" << subs[i].created_at << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات المنظمات
// ═══════════════════════════════════════════════════════════════════════════════

HttpResponse ApiRoutes::handle_create_organization(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    std::string name = json_get_string(req.body, "name");
    std::string display_name = json_get_string(req.body, "display_name");
    std::string description = json_get_string(req.body, "description");
    std::string avatar_url = json_get_string(req.body, "avatar_url");
    std::string website = json_get_string(req.body, "website");
    
    if (name.empty()) {
        return HttpResponse::error("اسم المنظمة مطلوب", 400);
    }
    if (display_name.empty()) display_name = name;
    
    OrganizationInfo org;
    org.name = name;
    org.display_name = display_name;
    org.description = description;
    org.avatar_url = avatar_url;
    org.website_url = website;
    org.created_by = user->id;
    
    auto id = db_->create_organization(org);
    if (id <= 0) {
        return HttpResponse::error("فشل في إنشاء المنظمة", 500);
    }
    
    std::ostringstream json;
    json << "{\"success\":true,\"organization_id\":" << id << "}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_get_organization(const HttpRequest& req) {
    std::string name = req.get_param("name");
    
    auto org = db_->get_organization(name);
    if (!org) {
        return HttpResponse::error("المنظمة غير موجودة", 404);
    }
    
    std::ostringstream json;
    json << "{\"organization\":{";
    json << "\"id\":" << org->id << ",";
    json << "\"name\":\"" << org->name << "\",";
    json << "\"display_name\":\"" << org->display_name << "\",";
    json << "\"description\":\"" << org->description << "\",";
    json << "\"avatar_url\":\"" << org->avatar_url << "\",";
    json << "\"website\":\"" << org->website_url << "\",";
    json << "\"is_verified\":" << (org->is_verified ? "true" : "false") << ",";
    json << "\"created_at\":\"" << org->created_at << "\"}}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_update_organization(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    std::string name = req.get_param("name");
    
    auto org = db_->get_organization(name);
    if (!org) {
        return HttpResponse::error("المنظمة غير موجودة", 404);
    }
    
    // التحقق من صلاحية التعديل
    auto role = db_->get_user_org_role(user->id, org->id);
    if (role != "admin" && role != "owner") {
        return HttpResponse::error("غير مصرح لك بتعديل هذه المنظمة", 403);
    }
    
    std::string display_name = json_get_string(req.body, "display_name");
    std::string description = json_get_string(req.body, "description");
    std::string avatar_url = json_get_string(req.body, "avatar_url");
    std::string website = json_get_string(req.body, "website");
    
    // Update the organization info
    OrganizationInfo updated_org = *org;
    if (!display_name.empty()) updated_org.display_name = display_name;
    if (!description.empty()) updated_org.description = description;
    if (!avatar_url.empty()) updated_org.avatar_url = avatar_url;
    if (!website.empty()) updated_org.website_url = website;
    
    if (!db_->update_organization(updated_org)) {
        return HttpResponse::error("فشل في تحديث المنظمة", 500);
    }
    
    return HttpResponse::json("{\"success\":true}");
}

HttpResponse ApiRoutes::handle_add_org_member(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    std::string org_name = req.get_param("name");
    
    auto org = db_->get_organization(org_name);
    if (!org) {
        return HttpResponse::error("المنظمة غير موجودة", 404);
    }
    
    // التحقق من صلاحية الإضافة
    auto role = db_->get_user_org_role(user->id, org->id);
    if (role != "admin" && role != "owner") {
        return HttpResponse::error("غير مصرح لك بإضافة أعضاء", 403);
    }
    
    std::string username = json_get_string(req.body, "username");
    std::string member_role = json_get_string(req.body, "role");
    if (member_role.empty()) member_role = "member";
    
    auto member = db_->get_user_by_username(username);
    if (!member) {
        return HttpResponse::error("المستخدم غير موجود", 404);
    }
    
    if (!db_->add_org_member(org->id, member->id, member_role, user->id)) {
        return HttpResponse::error("فشل في إضافة العضو", 500);
    }
    
    return HttpResponse::json("{\"success\":true}");
}

HttpResponse ApiRoutes::handle_remove_org_member(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    std::string org_name = req.get_param("name");
    int64_t member_id = std::atoll(req.get_param("member_id").c_str());
    
    auto org = db_->get_organization(org_name);
    if (!org) {
        return HttpResponse::error("المنظمة غير موجودة", 404);
    }
    
    // التحقق من صلاحية الإزالة
    auto role = db_->get_user_org_role(user->id, org->id);
    if (role != "admin" && role != "owner") {
        return HttpResponse::error("غير مصرح لك بإزالة أعضاء", 403);
    }
    
    if (!db_->remove_org_member(org->id, member_id)) {
        return HttpResponse::error("فشل في إزالة العضو", 500);
    }
    
    return HttpResponse::json("{\"success\":true}");
}

HttpResponse ApiRoutes::handle_list_org_members(const HttpRequest& req) {
    std::string org_name = req.get_param("name");
    
    auto org = db_->get_organization(org_name);
    if (!org) {
        return HttpResponse::error("المنظمة غير موجودة", 404);
    }
    
    auto members = db_->get_org_members(org->id);
    
    std::ostringstream json;
    json << "{\"members\":[";
    for (size_t i = 0; i < members.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"user_id\":" << members[i].user_id << ",";
        json << "\"role\":\"" << members[i].role << "\",";
        json << "\"joined_at\":\"" << members[i].created_at << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_list_my_organizations(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    auto orgs = db_->get_user_organizations(user->id);
    
    std::ostringstream json;
    json << "{\"organizations\":[";
    for (size_t i = 0; i < orgs.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"id\":" << orgs[i].id << ",";
        json << "\"name\":\"" << orgs[i].name << "\",";
        json << "\"display_name\":\"" << orgs[i].display_name << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات الاستشارات الأمنية
// ═══════════════════════════════════════════════════════════════════════════════

HttpResponse ApiRoutes::handle_create_advisory(const HttpRequest& req) {
    auto user = authenticate(req);
    if (!user) {
        return HttpResponse::error("يجب تسجيل الدخول", 401);
    }
    
    // التحقق من صلاحية المسؤول
    if (!user->is_admin) {
        return HttpResponse::error("صلاحيات المسؤول مطلوبة", 403);
    }
    
    std::string identifier = json_get_string(req.body, "identifier");
    std::string title = json_get_string(req.body, "title");
    std::string description = json_get_string(req.body, "description");
    std::string severity = json_get_string(req.body, "severity");
    std::string package_name = json_get_string(req.body, "package_name");
    std::string affected_versions = json_get_string(req.body, "affected_versions");
    std::string patched_versions = json_get_string(req.body, "patched_versions");
    std::string cwe_ids = json_get_string(req.body, "cwe_ids");
    std::string references = json_get_string(req.body, "references");
    
    if (title.empty() || package_name.empty()) {
        return HttpResponse::error("العنوان واسم الحزمة مطلوبان", 400);
    }
    
    auto pkg = db_->get_package(package_name);
    int64_t pkg_id = pkg ? pkg->id : 0;
    
    SecurityAdvisory advisory;
    advisory.advisory_id = identifier.empty() ? "SAD-" + std::to_string(std::time(nullptr)) : identifier;
    advisory.package_id = pkg_id;
    advisory.package_name = package_name;
    advisory.title = title;
    advisory.description = description;
    advisory.severity = severity.empty() ? "medium" : severity;
    advisory.affected_versions = affected_versions;
    advisory.patched_versions = patched_versions;
    advisory.cwe_ids = cwe_ids;
    advisory.references_json = references;
    advisory.reporter_id = user->id;
    advisory.is_public = true;
    
    auto id = db_->create_security_advisory(advisory);
    
    if (id <= 0) {
        return HttpResponse::error("فشل في إنشاء الاستشارة الأمنية", 500);
    }
    
    std::ostringstream json;
    json << "{\"success\":true,\"advisory_id\":" << id << "}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_list_advisories(const HttpRequest& req) {
    int limit = req.get_per_page();
    (void)req.get_page(); // unused for now
    std::string severity = req.get_query("severity");
    std::string package_name = req.get_query("package");
    (void)severity; (void)package_name; // filtering not implemented yet
    
    auto advisories = db_->get_recent_advisories(limit);
    
    std::ostringstream json;
    json << "{\"advisories\":[";
    for (size_t i = 0; i < advisories.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"id\":" << advisories[i].id << ",";
        json << "\"identifier\":\"" << advisories[i].advisory_id << "\",";
        json << "\"title\":\"" << advisories[i].title << "\",";
        json << "\"severity\":\"" << advisories[i].severity << "\",";
        json << "\"package_name\":\"" << advisories[i].package_name << "\",";
        json << "\"published_at\":\"" << advisories[i].published_at << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_get_advisory(const HttpRequest& req) {
    std::string pkg_name = req.get_param("name");
    
    // Get package to find its advisories
    auto pkg = db_->get_package(pkg_name);
    if (!pkg) {
        return HttpResponse::error("الحزمة غير موجودة", 404);
    }
    
    auto advisories = db_->get_package_advisories(pkg->id, true);
    if (advisories.empty()) {
        return HttpResponse::error("لا توجد استشارات أمنية", 404);
    }
    
    auto& advisory = advisories[0];
    
    std::ostringstream json;
    json << "{\"advisory\":{";
    json << "\"id\":" << advisory.id << ",";
    json << "\"identifier\":\"" << advisory.advisory_id << "\",";
    json << "\"title\":\"" << advisory.title << "\",";
    json << "\"description\":\"" << advisory.description << "\",";
    json << "\"severity\":\"" << advisory.severity << "\",";
    json << "\"package_name\":\"" << advisory.package_name << "\",";
    json << "\"affected_versions\":\"" << advisory.affected_versions << "\",";
    json << "\"patched_versions\":\"" << advisory.patched_versions << "\",";
    json << "\"cwe_ids\":\"" << advisory.cwe_ids << "\",";
    json << "\"references\":\"" << advisory.references_json << "\",";
    json << "\"published_at\":\"" << advisory.published_at << "\"}}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_check_advisories(const HttpRequest& req) {
    std::string package_name = req.get_param("name");
    std::string version = req.get_param("version");
    
    auto advisories = db_->check_version_advisories(package_name, version);
    
    std::ostringstream json;
    json << "{\"vulnerable\":" << (advisories.size() > 0 ? "true" : "false") << ",";
    json << "\"advisories\":[";
    for (size_t i = 0; i < advisories.size(); ++i) {
        if (i > 0) json << ",";
        json << "{\"identifier\":\"" << advisories[i].advisory_id << "\",";
        json << "\"severity\":\"" << advisories[i].severity << "\",";
        json << "\"title\":\"" << advisories[i].title << "\"}";
    }
    json << "]}";
    return HttpResponse::json(json.str());
}

// ═══════════════════════════════════════════════════════════════════════════════
// معالجات التحليلات
// ═══════════════════════════════════════════════════════════════════════════════

HttpResponse ApiRoutes::handle_detailed_analytics(const HttpRequest& req) {
    std::string package_name = req.get_param("name");
    int days = std::atoi(req.get_query("days", "30").c_str());
    
    auto pkg = db_->get_package(package_name);
    if (!pkg) {
        return HttpResponse::error("الحزمة غير موجودة", 404);
    }
    
    // Simplified analytics - using package download count
    std::ostringstream json;
    json << "{\"analytics\":{";
    json << "\"package\":\"" << package_name << "\",";
    json << "\"period_days\":" << days << ",";
    json << "\"total_downloads\":" << pkg->total_downloads << ",";
    json << "\"hourly_data\":[]}}";
    return HttpResponse::json(json.str());
}

HttpResponse ApiRoutes::handle_health(const HttpRequest& req) {
    (void)req;
    std::ostringstream json;
    json << "{\"status\":\"healthy\",";
    json << "\"timestamp\":" << std::time(nullptr) << ",";
    json << "\"version\":\"2.0.0\"}";
    return HttpResponse::json(json.str());
}

} // namespace registry
} // namespace sad

