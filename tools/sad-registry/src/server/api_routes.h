// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: api_routes.h
 * الوصف: تعريف نقاط واجهة برمجة التطبيقات (API) لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يعرّف الصنف المسؤول عن تسجيل وتنفيذ جميع نقاط API.
 *
 * نقاط API المدعومة:
 *
 *   ╔═══════════════════════════════════════════════════════════════════════╗
 *   ║  الفعل  │  المسار                          │  الوصف                ║
 *   ╠═══════════════════════════════════════════════════════════════════════╣
 *   ║  GET    │  /api/v1/packages                 │  قائمة الحزم          ║
 *   ║  GET    │  /api/v1/packages/:name            │  معلومات حزمة        ║
 *   ║  GET    │  /api/v1/packages/:name/:version   │  إصدار محدد          ║
 *   ║  GET    │  /api/v1/search                    │  البحث               ║
 *   ║  POST   │  /api/v1/packages                  │  نشر حزمة           ║
 *   ║  POST   │  /api/v1/register                  │  تسجيل مستخدم        ║
 *   ║  POST   │  /api/v1/login                     │  تسجيل دخول          ║
 *   ║  GET    │  /api/v1/stats                     │  إحصائيات            ║
 *   ║  GET    │  /api/v1/download/:name/:version   │  تنزيل حزمة          ║
 *   ║  DELETE │  /api/v1/packages/:name/:version   │  سحب إصدار           ║
 *   ║  GET    │  /api/v1/me                        │  الملف الشخصي        ║
 *   ║  GET    │  /api/v1/me/packages               │  حزم المستخدم        ║
 *   ║  GET    │  /api/v1/categories                │  التصنيفات           ║
 *   ║  GET    │  /api/v1/featured                  │  الحزم المميزة       ║
 *   ║  GET    │  /api/v1/popular                   │  الأكثر تحميلاً      ║
 *   ║  GET    │  /api/v1/recent                    │  الأحدث              ║
 *   ╚═══════════════════════════════════════════════════════════════════════╝
 *
 * مسارات عربية بديلة (اختيارية):
 *   /api/v1/حزم ، /api/v1/بحث ، /api/v1/نشر ، /api/v1/إحصائيات
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "http_server.h"
#include "../storage/database.h"
#include "../storage/blob_store.h"
#include "../search/arabic_search.h"
#include "../auth/auth.h"

#include <memory>
#include <string>

namespace sad {
namespace registry {

/**
 * @class ApiRoutes
 * @brief يسجل ويُنفذ جميع نقاط API في مستودع الحزم
 *
 * هذا الصنف هو المنسق الرئيسي بين:
 *   - خادم HTTP (يستقبل الطلبات)
 *   - قاعدة البيانات (يخزن/يسترجع البيانات)
 *   - مخزن الكائنات (يخزن ملفات الحزم)
 *   - محرك البحث (يبحث بالعربية والإنجليزية)
 *   - نظام المصادقة (يتحقق من الرموز)
 *
 * كل دالة معالجة:
 *   ١. تتحقق من المصادقة (إن لزم)
 *   ٢. تتحقق من صحة المدخلات
 *   ٣. تنفذ العملية على قاعدة البيانات
 *   ٤. تبني استجابة JSON
 */
class ApiRoutes {
public:
    /**
     * @brief المُنشئ
     * @param db مؤشر على قاعدة البيانات المشتركة
     * @param blobs مؤشر على مخزن الكائنات
     * @param search مؤشر على محرك البحث العربي
     * @param auth مؤشر على نظام المصادقة
     */
    ApiRoutes(std::shared_ptr<Database> db,
              std::shared_ptr<BlobStore> blobs,
              std::shared_ptr<ArabicSearchEngine> search,
              std::shared_ptr<AuthManager> auth);

    /**
     * @brief تسجيل جميع نقاط API في الخادم
     * @param server الخادم المراد تسجيل النقاط فيه
     *
     * يسجل جميع المسارات (الإنجليزية والعربية) مع معالجاتها.
     * يُستدعى مرة واحدة عند بدء التشغيل.
     */
    void register_routes(HttpServer& server);

private:
    // ─────────────────────────────────────────────────────────────────────
    // المكونات المشتركة
    // ─────────────────────────────────────────────────────────────────────
    std::shared_ptr<Database> db_;                  // قاعدة البيانات
    std::shared_ptr<BlobStore> blobs_;              // مخزن الكائنات
    std::shared_ptr<ArabicSearchEngine> search_;    // محرك البحث
    std::shared_ptr<AuthManager> auth_;             // نظام المصادقة

    // ─────────────────────────────────────────────────────────────────────
    // معالجات الحزم
    // ─────────────────────────────────────────────────────────────────────

    /// قائمة الحزم (مع تصفيح)
    HttpResponse handle_list_packages(const HttpRequest& req);

    /// معلومات حزمة واحدة
    HttpResponse handle_get_package(const HttpRequest& req);

    /// معلومات إصدار محدد من حزمة
    HttpResponse handle_get_version(const HttpRequest& req);

    /// نشر حزمة جديدة (يتطلب مصادقة)
    HttpResponse handle_publish(const HttpRequest& req);

    /// تنزيل حزمة
    HttpResponse handle_download(const HttpRequest& req);

    /// سحب إصدار (يتطلب مصادقة + ملكية)
    HttpResponse handle_yank_version(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات البحث
    // ─────────────────────────────────────────────────────────────────────

    /// البحث عن حزم (يدعم العربية والإنجليزية)
    HttpResponse handle_search(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات المصادقة
    // ─────────────────────────────────────────────────────────────────────

    /// تسجيل مستخدم جديد
    HttpResponse handle_register(const HttpRequest& req);

    /// تسجيل دخول
    HttpResponse handle_login(const HttpRequest& req);

    /// الملف الشخصي للمستخدم الحالي
    HttpResponse handle_me(const HttpRequest& req);

    /// حزم المستخدم الحالي
    HttpResponse handle_my_packages(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات الإحصائيات والاستكشاف
    // ─────────────────────────────────────────────────────────────────────

    /// إحصائيات المستودع
    HttpResponse handle_stats(const HttpRequest& req);

    /// الحزم المميزة
    HttpResponse handle_featured(const HttpRequest& req);

    /// الأكثر تحميلاً
    HttpResponse handle_popular(const HttpRequest& req);

    /// الأحدث
    HttpResponse handle_recent(const HttpRequest& req);

    /// قائمة التصنيفات
    HttpResponse handle_categories(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات التقييمات والمراجعات
    // ─────────────────────────────────────────────────────────────────────

    /// إنشاء أو تحديث تقييم
    HttpResponse handle_create_review(const HttpRequest& req);

    /// قائمة تقييمات حزمة
    HttpResponse handle_get_reviews(const HttpRequest& req);

    /// حذف تقييم
    HttpResponse handle_delete_review(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات التفضيلات (Stars)
    // ─────────────────────────────────────────────────────────────────────

    /// تفضيل / إلغاء تفضيل حزمة
    HttpResponse handle_toggle_star(const HttpRequest& req);

    /// الحزم المفضلة للمستخدم الحالي
    HttpResponse handle_my_stars(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات الإبلاغات
    // ─────────────────────────────────────────────────────────────────────

    /// إنشاء بلاغ على حزمة
    HttpResponse handle_create_report(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات الخطّافات (Webhooks)
    // ─────────────────────────────────────────────────────────────────────

    /// إنشاء خطّاف
    HttpResponse handle_create_webhook(const HttpRequest& req);

    /// قائمة خطّافات المستخدم
    HttpResponse handle_list_webhooks(const HttpRequest& req);

    /// حذف خطّاف
    HttpResponse handle_delete_webhook(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات المسؤول (Admin)
    // ─────────────────────────────────────────────────────────────────────

    /// لوحة إحصائيات المسؤول
    HttpResponse handle_admin_stats(const HttpRequest& req);

    /// قائمة البلاغات (للمسؤول)
    HttpResponse handle_admin_reports(const HttpRequest& req);

    /// حل بلاغ
    HttpResponse handle_admin_resolve_report(const HttpRequest& req);

    /// تعيين حزمة مميزة / موثقة
    HttpResponse handle_admin_set_package_flag(const HttpRequest& req);

    /// سجل المراجعة
    HttpResponse handle_admin_audit_log(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات إضافية
    // ─────────────────────────────────────────────────────────────────────

    /// اقتراحات البحث
    HttpResponse handle_suggest(const HttpRequest& req);

    /// تحديث الملف الشخصي
    HttpResponse handle_update_profile(const HttpRequest& req);

    /// ملف شخصي عام
    HttpResponse handle_public_profile(const HttpRequest& req);

    /// إحصائيات تنزيل يومية
    HttpResponse handle_download_stats(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات مفاتيح الناشرين والتوقيعات الرقمية
    // ─────────────────────────────────────────────────────────────────────

    /// إضافة مفتاح جديد
    HttpResponse handle_add_publisher_key(const HttpRequest& req);

    /// قائمة مفاتيح المستخدم
    HttpResponse handle_list_my_keys(const HttpRequest& req);

    /// إبطال مفتاح
    HttpResponse handle_revoke_key(const HttpRequest& req);

    /// التحقق من توقيع إصدار
    HttpResponse handle_verify_signature(const HttpRequest& req);

    /// الحصول على توقيعات إصدار
    HttpResponse handle_get_signatures(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات حل التبعيات
    // ─────────────────────────────────────────────────────────────────────

    /// حل تبعيات قائمة من الحزم
    HttpResponse handle_resolve_dependencies(const HttpRequest& req);

    /// التحقق من توافق إصدار
    HttpResponse handle_check_compatibility(const HttpRequest& req);

    /// الحصول على التبعيات العكسية
    HttpResponse handle_reverse_dependencies(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات الإشعارات
    // ─────────────────────────────────────────────────────────────────────

    /// قائمة إشعارات المستخدم
    HttpResponse handle_list_notifications(const HttpRequest& req);

    /// تعيين إشعار كمقروء
    HttpResponse handle_mark_notification_read(const HttpRequest& req);

    /// تعيين جميع الإشعارات كمقروءة
    HttpResponse handle_mark_all_read(const HttpRequest& req);

    /// حذف إشعار
    HttpResponse handle_delete_notification(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات الاشتراكات
    // ─────────────────────────────────────────────────────────────────────

    /// الاشتراك في حزمة
    HttpResponse handle_subscribe(const HttpRequest& req);

    /// إلغاء الاشتراك
    HttpResponse handle_unsubscribe(const HttpRequest& req);

    /// قائمة اشتراكات المستخدم
    HttpResponse handle_list_subscriptions(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات المنظمات
    // ─────────────────────────────────────────────────────────────────────

    /// إنشاء منظمة
    HttpResponse handle_create_organization(const HttpRequest& req);

    /// معلومات منظمة
    HttpResponse handle_get_organization(const HttpRequest& req);

    /// تحديث منظمة
    HttpResponse handle_update_organization(const HttpRequest& req);

    /// إضافة عضو للمنظمة
    HttpResponse handle_add_org_member(const HttpRequest& req);

    /// إزالة عضو من المنظمة
    HttpResponse handle_remove_org_member(const HttpRequest& req);

    /// قائمة أعضاء المنظمة
    HttpResponse handle_list_org_members(const HttpRequest& req);

    /// قائمة منظمات المستخدم
    HttpResponse handle_list_my_organizations(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات الاستشارات الأمنية
    // ─────────────────────────────────────────────────────────────────────

    /// إنشاء استشارة أمنية
    HttpResponse handle_create_advisory(const HttpRequest& req);

    /// قائمة الاستشارات الأمنية
    HttpResponse handle_list_advisories(const HttpRequest& req);

    /// معلومات استشارة محددة
    HttpResponse handle_get_advisory(const HttpRequest& req);

    /// التحقق من استشارات لحزمة
    HttpResponse handle_check_advisories(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // معالجات التحليلات
    // ─────────────────────────────────────────────────────────────────────

    /// تحليلات تنزيل مفصلة
    HttpResponse handle_detailed_analytics(const HttpRequest& req);

    /// نقطة صحة الخادم
    HttpResponse handle_health(const HttpRequest& req);

    // ─────────────────────────────────────────────────────────────────────
    // دوال مساعدة
    // ─────────────────────────────────────────────────────────────────────

    /**
     * @brief تحقق من مصادقة الطلب
     * @param req الطلب الوارد
     * @return بيانات المستخدم، أو nullopt إذا لم يكن موثقاً
     */
    std::optional<UserInfo> authenticate(const HttpRequest& req);

    /**
     * @brief تحويل PackageInfo إلى JSON
     */
    std::string package_to_json(const PackageInfo& pkg);

    /**
     * @brief تحويل VersionInfo إلى JSON
     */
    std::string version_to_json(const VersionInfo& ver);

    /**
     * @brief تحويل SearchResult إلى JSON
     */
    std::string search_result_to_json(const SearchResult& result);

    /**
     * @brief تحويل قائمة إلى مصفوفة JSON
     */
    template<typename T>
    std::string list_to_json(const std::vector<T>& items,
                             std::function<std::string(const T&)> converter);

    /**
     * @brief تحليل JSON بسيط - استخراج قيمة نصية
     * @param json نص JSON
     * @param key المفتاح المطلوب
     * @return القيمة أو سلسلة فارغة
     */
    static std::string json_get_string(const std::string& json,
                                        const std::string& key);

    /**
     * @brief بناء كائن JSON من أزواج مفتاح-قيمة
     */
    static std::string make_json(
        const std::vector<std::pair<std::string, std::string>>& fields);
};

} // namespace registry
} // namespace sad
