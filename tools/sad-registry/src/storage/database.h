// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: database.h
 * الوصف: طبقة قاعدة البيانات لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف يوفر واجهة موحدة للتعامل مع قاعدة بيانات SQLite.
 * يغلّف عمليات SQL في دوال C++ آمنة وسهلة الاستخدام.
 *
 * المسؤوليات:
 *   ١. إدارة الاتصال بقاعدة البيانات (فتح/إغلاق)
 *   ٢. تهيئة المخطط عند أول تشغيل
 *   ٣. عمليات CRUD على الحزم والإصدارات والمستخدمين
 *   ٤. البحث النصي باستخدام FTS5
 *   ٥. إحصائيات التنزيل
 *   ٦. إدارة رموز المصادقة
 *
 * الأمان:
 *   - جميع الاستعلامات تستخدم عبارات مُعدّة (prepared statements)
 *     لمنع هجمات حقن SQL
 *   - كلمات المرور تُشفَّر قبل التخزين
 *   - الرموز تُخزَّن كبصمات SHA256 فقط
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <functional>
#include <cstdint>
#include <chrono>

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// هياكل البيانات - النماذج (Models)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct معلومات_المستخدم
 * @brief بيانات مستخدم من قاعدة البيانات
 *
 * يمثل صفاً واحداً من جدول المستخدمين.
 * يُستخدم لنقل البيانات بين طبقة قاعدة البيانات وطبقة API.
 */
struct UserInfo {
    int64_t id = 0;                  // المعرف الفريد
    std::string username;            // اسم المستخدم
    std::string email;               // البريد الإلكتروني
    std::string password_hash;       // بصمة كلمة المرور
    std::string display_name;        // الاسم المعروض
    std::string bio;                 // السيرة الذاتية
    std::string avatar_url;          // رابط الصورة
    bool is_active = false;          // هل الحساب مفعّل؟
    bool is_admin = false;           // هل المستخدم مشرف؟
    int package_count = 0;           // عدد الحزم
    std::string created_at;          // تاريخ الإنشاء
    std::string last_login_at;       // تاريخ آخر دخول
};

/**
 * @struct معلومات_الحزمة
 * @brief بيانات حزمة من قاعدة البيانات
 *
 * يمثل صفاً واحداً من جدول الحزم مع البيانات المجمّعة.
 * يحتوي على جميع المعلومات اللازمة لعرض الحزمة في API والويب.
 */
struct PackageInfo {
    int64_t id = 0;                      // المعرف الفريد
    std::string name;                    // اسم الحزمة
    std::string display_name;            // الاسم المعروض
    std::string arabic_name;             // الاسم العربي
    std::string description;             // الوصف
    std::string description_ar;          // الوصف بالعربية
    int64_t owner_id = 0;               // معرف المالك
    std::string owner_username;          // اسم المالك (محسوب)
    std::string license;                 // الترخيص
    std::string repository_url;          // مستودع الكود
    std::string homepage_url;            // الصفحة الرئيسية
    std::string documentation_url;       // التوثيق
    std::string latest_version;          // أحدث إصدار
    int64_t total_downloads = 0;         // إجمالي التنزيلات
    int64_t monthly_downloads = 0;       // تنزيلات الشهر
    int64_t weekly_downloads = 0;        // تنزيلات الأسبوع
    double rating = 0.0;                 // التقييم
    int rating_count = 0;                // عدد التقييمات
    bool is_verified = false;            // هل موثّقة؟
    bool is_featured = false;            // هل مميزة؟
    bool is_yanked = false;              // هل مسحوبة؟
    std::string category;                // التصنيف
    std::string created_at;              // تاريخ الإنشاء
    std::string updated_at;             // تاريخ التحديث
    std::vector<std::string> keywords;   // كلمات مفتاحية
    std::vector<std::string> versions;   // قائمة الإصدارات
};

/**
 * @struct معلومات_الإصدار
 * @brief بيانات إصدار من قاعدة البيانات
 */
struct VersionInfo {
    int64_t id = 0;                  // المعرف الفريد
    int64_t package_id = 0;          // معرف الحزمة
    std::string version;             // رقم الإصدار (مثل 1.2.3)
    int major = 0;                   // الرئيسي
    int minor = 0;                   // الثانوي
    int patch = 0;                   // الترقيع
    std::string prerelease;          // ما قبل الإصدار
    std::string release_notes;       // ملاحظات الإصدار
    std::string release_notes_en;    // ملاحظات بالإنجليزية
    std::string checksum_sha256;     // البصمة
    std::string blob_path;           // مسار الملف
    int64_t file_size = 0;           // حجم الملف
    bool is_yanked = false;          // هل مسحوب؟
    std::string yank_reason;         // سبب السحب
    int64_t downloads = 0;           // عدد التنزيلات
    std::string created_at;          // تاريخ النشر

    // التبعيات المرتبطة بهذا الإصدار
    struct Dependency {
        std::string name;            // اسم الحزمة المطلوبة
        std::string version_req;     // متطلب الإصدار
        std::string dep_type;        // نوع التبعية
    };
    std::vector<Dependency> dependencies;
};

/**
 * @struct نتيجة_البحث
 * @brief نتيجة بحث واحدة
 *
 * تحتوي على المعلومات الأساسية لعرض الحزمة في نتائج البحث.
 * أخف وزناً من PackageInfo لأداء أفضل.
 */
struct SearchResult {
    int64_t id = 0;                  // المعرف
    std::string name;                // الاسم
    std::string arabic_name;         // الاسم العربي
    std::string description;         // الوصف
    std::string description_ar;      // الوصف بالعربية
    std::string latest_version;      // أحدث إصدار
    int64_t total_downloads = 0;     // التنزيلات
    double rating = 0.0;             // التقييم
    std::string category;            // التصنيف
    bool is_verified = false;        // موثّقة؟
    double relevance = 0.0;          // درجة الصلة (من محرك البحث)
};

/**
 * @struct إحصائيات_المستودع
 * @brief إحصائيات عامة عن المستودع
 */
struct RegistryStats {
    int64_t total_packages = 0;      // إجمالي الحزم
    int64_t total_versions = 0;      // إجمالي الإصدارات
    int64_t total_downloads = 0;     // إجمالي التنزيلات
    int64_t total_users = 0;         // إجمالي المستخدمين
    int64_t packages_this_week = 0;  // حزم جديدة هذا الأسبوع
    int64_t downloads_this_week = 0; // تنزيلات هذا الأسبوع
};

// ═══════════════════════════════════════════════════════════════════════════════
// هياكل التحسينات الجديدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct مراجعة
 * @brief تقييم ومراجعة حزمة من مستخدم
 */
struct ReviewInfo {
    int64_t id = 0;                  // المعرف الفريد
    int64_t package_id = 0;          // معرف الحزمة
    int64_t user_id = 0;             // معرف المراجِع
    std::string username;            // اسم المراجِع (محسوب)
    int rating = 0;                  // التقييم (١-٥ نجوم)
    std::string title;               // عنوان المراجعة
    std::string body;                // نص المراجعة
    int helpful_count = 0;           // عدد "مفيد"
    bool is_edited = false;          // هل تم التعديل؟
    std::string created_at;          // تاريخ الإنشاء
    std::string updated_at;          // تاريخ التحديث
};

/**
 * @struct إبلاغ
 * @brief بلاغ عن حزمة مخالفة
 */
struct ReportInfo {
    int64_t id = 0;
    int64_t reporter_id = 0;
    int64_t package_id = 0;
    std::string reporter_username;
    std::string package_name;
    std::string reason;              // malware, spam, copyright, inappropriate
    std::string details;
    std::string status;              // pending, reviewing, resolved, dismissed
    std::string admin_notes;
    int64_t resolved_by = 0;
    std::string created_at;
    std::string resolved_at;
};

/**
 * @struct خطّاف (webhook)
 * @brief إشعار HTTP تلقائي عند حدوث حدث
 */
struct WebhookInfo {
    int64_t id = 0;
    int64_t user_id = 0;
    int64_t package_id = 0;          // 0 = كل الحزم
    std::string url;
    std::string secret;
    std::string events;              // فاصلة بين الأحداث
    bool is_active = true;
    int failure_count = 0;
    std::string last_triggered;
    std::string created_at;
};

/**
 * @struct إحصائيات_تنزيل_يومية
 * @brief بيانات التنزيل لحزمة في يوم واحد (للرسوم البيانية)
 */
struct DailyDownloadInfo {
    std::string date;
    int64_t count = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
// هياكل التحسينات الثورية الجديدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct مفتاح_الناشر
 * @brief المفتاح العام للتوقيع الرقمي
 */
struct PublisherKeyInfo {
    int64_t id = 0;
    int64_t user_id = 0;
    std::string key_type;            // ed25519, rsa4096
    std::string public_key;          // Base64
    std::string fingerprint;         // SHA256 الملخص
    std::string key_name;            // اسم ودود
    bool is_primary = false;
    bool is_revoked = false;
    std::string revoked_at;
    std::string revoke_reason;
    std::string expires_at;
    std::string created_at;
};

/**
 * @struct توقيع_الإصدار
 * @brief التوقيع الرقمي على ملف الحزمة
 */
struct VersionSignature {
    int64_t id = 0;
    int64_t version_id = 0;
    int64_t key_id = 0;
    std::string signature;           // Base64
    std::string signature_type;      // detached, inline
    std::string signed_content_hash; // SHA256 of tarball
    std::string timestamp_authority;
    std::string timestamp_signature;
    bool is_valid = true;
    std::string verified_at;
    std::string created_at;
};

/**
 * @struct تبعية_متقدمة
 * @brief معلومات تبعية إصدار موسّعة
 */
struct DependencyV2 {
    int64_t id = 0;
    int64_t version_id = 0;
    std::string dependency_name;
    std::string version_constraint;  // >=1.0.0, ^2.0.0, ~1.2
    std::string constraint_type;     // exact, caret, tilde, range, any
    std::string dependency_type;     // runtime, dev, build, optional, peer
    bool is_optional = false;
    std::string features;            // JSON array
    std::string platform_filter;     // windows, linux, macos, all
    std::string registry_url;        // for external registries
    std::string resolved_version;
};

/**
 * @struct تعارض_معروف
 * @brief تعارض موثّق بين حزمتين
 */
struct KnownConflict {
    int64_t id = 0;
    std::string package_a;
    std::string version_a;
    std::string package_b;
    std::string version_b;
    std::string conflict_reason;
    std::string severity;            // warning, error, critical
    std::string workaround;
    int64_t reported_by = 0;
    bool verified = false;
    std::string created_at;
};

/**
 * @struct ذاكرة_حل_التبعيات
 * @brief ذاكرة مؤقتة لنتيجة حل التبعيات
 */
struct DependencyCacheEntry {
    int64_t id = 0;
    std::string cache_key;
    std::string resolved_tree;       // JSON
    int resolution_time_ms = 0;
    int hit_count = 0;
    std::string last_hit_at;
    std::string expires_at;
    std::string created_at;
};

/**
 * @struct إشعار
 * @brief إشعار لمستخدم
 */
struct NotificationInfo {
    int64_t id = 0;
    int64_t user_id = 0;
    std::string notification_type;   // new_version, security_alert, review, mention
    std::string title;
    std::string body;
    int64_t related_package_id = 0;
    int64_t related_user_id = 0;
    std::string action_url;
    bool is_read = false;
    bool is_archived = false;
    std::string created_at;
    std::string read_at;
};

/**
 * @struct اشتراك
 * @brief متابعة حزمة أو مستخدم
 */
struct SubscriptionInfo {
    int64_t id = 0;
    int64_t user_id = 0;
    std::string target_type;         // package, user, category
    int64_t target_id = 0;
    bool notify_new_version = true;
    bool notify_security = true;
    bool notify_deprecation = true;
    bool email_enabled = false;
    std::string created_at;
};

/**
 * @struct منظمة
 * @brief مجموعة منظمة لإدارة الحزم
 */
struct OrganizationInfo {
    int64_t id = 0;
    std::string name;
    std::string display_name;
    std::string description;
    std::string avatar_url;
    std::string website_url;
    bool is_verified = false;
    std::string plan_type;           // free, pro, enterprise
    std::string plan_expires_at;
    int64_t created_by = 0;
    std::string created_at;
};

/**
 * @struct عضو_منظمة
 * @brief عضوية في منظمة
 */
struct OrgMemberInfo {
    int64_t id = 0;
    int64_t org_id = 0;
    int64_t user_id = 0;
    std::string role;                // owner, admin, member, readonly
    int64_t invited_by = 0;
    std::string accepted_at;
    std::string created_at;
    std::string username;            // محسوب
    std::string display_name;        // محسوب
};

/**
 * @struct تنبيه_أمني
 * @brief تنبيه ثغرة أمنية في حزمة
 */
struct SecurityAdvisory {
    int64_t id = 0;
    std::string advisory_id;         // SAD-2024-001
    int64_t package_id = 0;
    std::string package_name;        // محسوب
    std::string title;
    std::string description;
    std::string severity;            // low, medium, high, critical
    double cvss_score = 0.0;
    std::string affected_versions;
    std::string patched_versions;
    std::string cve_ids;             // JSON array
    std::string cwe_ids;             // JSON array
    std::string references_json;     // JSON array
    int64_t reporter_id = 0;
    bool is_public = false;
    std::string published_at;
    std::string withdrawn_at;
    std::string created_at;
    std::string updated_at;
};

/**
 * @struct مرآة_المستودع
 * @brief معلومات مرآة(mirror) للمستودع
 */
struct RegistryMirror {
    int64_t id = 0;
    std::string name;
    std::string base_url;
    std::string region;              // MENA, EU, NA, APAC
    std::string country_code;
    std::string mirror_type;         // pull, push, bidirectional
    int sync_frequency_minutes = 60;
    bool is_official = false;
    bool is_active = true;
    std::string last_sync_at;
    std::string last_sync_status;
    int64_t packages_synced = 0;
    int64_t bandwidth_used_mb = 0;
    double health_score = 1.0;
    std::string created_at;
};

/**
 * @struct نتيجة_حل_التبعيات
 * @brief نتيجة عملية حل التبعيات
 */
struct DependencyResolution {
    bool success = false;
    std::string error_message;
    
    struct ResolvedPackage {
        std::string name;
        std::string version;
        std::string checksum;
        std::vector<std::string> depends_on;
    };
    std::vector<ResolvedPackage> resolved;  // بترتيب التثبيت
    
    struct Conflict {
        std::string package_a;
        std::string version_a;
        std::string package_b;
        std::string version_b;
        std::string reason;
    };
    std::vector<Conflict> conflicts;
    
    int resolution_time_ms = 0;
};

/**
 * @struct تحليلات_التنزيل
 * @brief بيانات تحليلية مفصّلة عن تنزيل
 */
struct DownloadAnalytics {
    int64_t id = 0;
    int64_t version_id = 0;
    std::string downloaded_at;
    std::string country_code;
    std::string region;
    std::string city;
    std::string client_type;         // cli, browser, ci, mirror
    std::string client_version;
    std::string os_name;
    std::string os_version;
    std::string arch;
    std::string sad_version;
    bool is_ci = false;
};

/**
 * @struct إحصائيات_ساعية
 * @brief إحصائيات مجمعة بالساعة
 */
struct HourlyStats {
    int64_t id = 0;
    std::string stat_hour;
    int64_t package_id = 0;
    int64_t version_id = 0;
    std::string stat_type;           // downloads, searches, api_calls
    int64_t count = 0;
    int64_t unique_count = 0;
    std::string metadata;            // JSON
};

/**
 * @struct مهمة_المعالجة
 * @brief مهمة في قائمة انتظار المعالجة الخلفية
 */
struct JobInfo {
    int64_t id = 0;
    std::string job_type;            // reindex, cleanup, notify, mirror
    std::string payload;             // JSON
    int priority = 5;
    std::string status;              // pending, running, completed, failed
    int attempts = 0;
    int max_attempts = 3;
    std::string run_at;
    std::string started_at;
    std::string completed_at;
    std::string result;
    std::string error_message;
    std::string worker_id;
    std::string created_at;
};

/**
 * @struct حد_المعدل
 * @brief حالة تحديد المعدل لمُعرِّف معين
 */
struct RateLimitInfo {
    std::string identifier;
    std::string identifier_type;     // ip, user, token
    std::string endpoint_pattern;
    int64_t request_count = 0;
    int64_t limit_value = 0;
    bool is_blocked = false;
    std::string blocked_until;
    std::string window_start;
};

/**
 * @struct صفحة_نتائج
 * @brief نتائج مصفحة (paginated)
 *
 * تُستخدم لإرجاع نتائج البحث والقوائم مع معلومات التصفيح.
 */
template<typename T>
struct PagedResults {
    std::vector<T> items;            // العناصر في الصفحة الحالية
    int64_t total_count = 0;         // إجمالي العناصر
    int page = 1;                    // رقم الصفحة الحالية
    int per_page = 20;               // عناصر لكل صفحة
    int total_pages = 0;             // إجمالي الصفحات

    bool has_next() const { return page < total_pages; }
    bool has_prev() const { return page > 1; }
};


// ═══════════════════════════════════════════════════════════════════════════════
// صنف قاعدة البيانات الرئيسي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class Database
 * @brief طبقة الوصول لقاعدة بيانات مستودع الحزم
 *
 * هذا الصنف يغلّف جميع عمليات قاعدة البيانات في واجهة C++ نظيفة.
 * يستخدم نمط التصميم "المستودع" (Repository Pattern) حيث أن كل
 * مجموعة من العمليات المتعلقة بكيان معين (حزمة، مستخدم، إصدار)
 * مجمّعة معاً.
 *
 * الاستخدام:
 * @code
 *   Database db("./data/packages.db");
 *   if (!db.initialize()) {
 *       std::cerr << "فشل تهيئة قاعدة البيانات!" << std::endl;
 *       return 1;
 *   }
 *
 *   // البحث عن حزم
 *   auto results = db.search_packages("شبكة", 1, 20);
 *
 *   // إضافة حزمة جديدة
 *   PackageInfo pkg;
 *   pkg.name = "my_package";
 *   pkg.description = "حزمة رائعة";
 *   int64_t id = db.create_package(pkg);
 * @endcode
 *
 * ملاحظات أمنية:
 *   - جميع المدخلات تُمرَّر عبر عبارات مُعدّة (prepared statements)
 *   - لا يتم بناء نصوص SQL يدوياً أبداً
 *   - الاتصال يُفتح في وضع WAL للأداء مع القراءة المتزامنة
 */
class Database {
public:
    // ─────────────────────────────────────────────────────────────────────────
    // الإنشاء والإعداد
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief المُنشئ
     * @param db_path مسار ملف قاعدة البيانات
     *
     * لا يفتح الاتصال فوراً - يجب استدعاء initialize() أولاً.
     * إذا كان المسار ":memory:" يتم إنشاء قاعدة بيانات في الذاكرة فقط (للاختبارات).
     */
    explicit Database(const std::string& db_path = "./data/packages.db");

    /**
     * @brief المُهدم - يُغلق الاتصال تلقائياً
     */
    ~Database();

    // منع النسخ (الاتصال ليس قابلاً للنسخ)
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    /**
     * @brief تهيئة قاعدة البيانات
     * @return true إذا نجحت التهيئة
     *
     * تقوم بـ:
     *   ١. فتح الاتصال بقاعدة البيانات (أو إنشائها إن لم تكن موجودة)
     *   ٢. تفعيل المفاتيح الخارجية ووضع WAL
     *   ٣. تنفيذ ملفات الهجرة (migrations) إن لم تُنفَّذ من قبل
     *   ٤. التحقق من سلامة المخطط
     */
    bool initialize();

    /**
     * @brief إغلاق الاتصال بقاعدة البيانات
     */
    void close();

    /**
     * @brief هل الاتصال مفتوح؟
     */
    bool is_open() const;

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات المستخدمين
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء مستخدم جديد
     * @param user بيانات المستخدم (اسم، بريد، كلمة مرور مشفرة)
     * @return معرف المستخدم الجديد، أو -1 عند الفشل
     *
     * يتحقق من:
     *   - عدم تكرار اسم المستخدم
     *   - عدم تكرار البريد الإلكتروني
     *   - صحة صيغة البريد الإلكتروني
     */
    int64_t create_user(const UserInfo& user);

    /**
     * @brief جلب مستخدم بالمعرف
     */
    std::optional<UserInfo> get_user(int64_t id);

    /**
     * @brief جلب مستخدم باسم المستخدم
     */
    std::optional<UserInfo> get_user_by_username(const std::string& username);

    /**
     * @brief جلب مستخدم بالبريد الإلكتروني
     */
    std::optional<UserInfo> get_user_by_email(const std::string& email);

    /**
     * @brief تحديث بيانات مستخدم
     */
    bool update_user(const UserInfo& user);

    /**
     * @brief تحديث تاريخ آخر تسجيل دخول
     */
    bool update_last_login(int64_t user_id);

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات الحزم
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء حزمة جديدة
     * @param pkg بيانات الحزمة
     * @return معرف الحزمة الجديدة، أو -1 عند الفشل
     *
     * يتحقق من عدم تكرار اسم الحزمة.
     * يُنشئ أيضاً الإدخالات في جدول الكلمات المفتاحية وفهرس البحث.
     */
    int64_t create_package(const PackageInfo& pkg);

    /**
     * @brief جلب حزمة بالاسم
     * @param name اسم الحزمة (عربي أو إنجليزي)
     * @return بيانات الحزمة، أو nullopt إذا لم تُوجد
     *
     * يبحث أولاً في حقل name، ثم في arabic_name.
     */
    std::optional<PackageInfo> get_package(const std::string& name);

    /**
     * @brief جلب حزمة بالمعرف
     */
    std::optional<PackageInfo> get_package_by_id(int64_t id);

    /**
     * @brief تحديث بيانات حزمة
     */
    bool update_package(const PackageInfo& pkg);

    /**
     * @brief سحب حزمة (yank) - إخفاؤها من البحث دون حذفها
     */
    bool yank_package(int64_t package_id, bool yank = true);

    /**
     * @brief حذف حزمة نهائياً (للمشرفين فقط)
     */
    bool delete_package(int64_t package_id);

    /**
     * @brief قائمة الحزم الأحدث
     */
    PagedResults<PackageInfo> list_packages(int page = 1, int per_page = 20,
                                            const std::string& sort = "created_at",
                                            bool descending = true);

    /**
     * @brief الحزم الأكثر تحميلاً
     */
    std::vector<PackageInfo> get_popular_packages(int limit = 10);

    /**
     * @brief الحزم المميزة (المختارة من المشرفين)
     */
    std::vector<PackageInfo> get_featured_packages(int limit = 10);

    /**
     * @brief الحزم الجديدة هذا الأسبوع
     */
    std::vector<PackageInfo> get_new_packages(int limit = 10);

    /**
     * @brief حزم مستخدم معين
     */
    std::vector<PackageInfo> get_user_packages(int64_t user_id);

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات الإصدارات
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إضافة إصدار جديد لحزمة
     * @param version بيانات الإصدار
     * @return معرف الإصدار الجديد، أو -1 عند الفشل
     *
     * يتحقق من:
     *   - عدم تكرار رقم الإصدار لنفس الحزمة
     *   - صحة صيغة الإصدار الدلالي
     *   - وجود بصمة SHA256 صالحة
     * يُحدّث أيضاً حقل latest_version في جدول الحزم.
     */
    int64_t create_version(const VersionInfo& version);

    /**
     * @brief جلب إصدار محدد
     */
    std::optional<VersionInfo> get_version(int64_t package_id, const std::string& version);

    /**
     * @brief قائمة جميع إصدارات حزمة
     */
    std::vector<VersionInfo> get_package_versions(int64_t package_id);

    /**
     * @brief سحب إصدار (yank)
     */
    bool yank_version(int64_t version_id, const std::string& reason = "");

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات البحث
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief البحث عن حزم
     * @param query نص البحث (عربي أو إنجليزي)
     * @param page رقم الصفحة
     * @param per_page عدد النتائج لكل صفحة
     * @return نتائج مصفحة مرتبة حسب الصلة
     *
     * يستخدم FTS5 للبحث النصي الكامل.
     * يبحث في: الاسم، الاسم العربي، الوصف، الكلمات المفتاحية.
     * يدعم البحث بالجذور العربية (عبر جدول arabic_search_index).
     */
    PagedResults<SearchResult> search_packages(const std::string& query,
                                               int page = 1, int per_page = 20);

    /**
     * @brief البحث حسب التصنيف
     */
    PagedResults<SearchResult> search_by_category(const std::string& category,
                                                   int page = 1, int per_page = 20);

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات التنزيل والإحصائيات
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief تسجيل عملية تنزيل
     */
    void record_download(int64_t version_id, const std::string& ip_hash = "",
                         const std::string& user_agent = "");

    /**
     * @brief الحصول على إحصائيات المستودع
     */
    RegistryStats get_stats();

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات المصادقة والرموز
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء رمز وصول جديد
     * @param user_id معرف المستخدم
     * @param token_name اسم الرمز
     * @param token_hash بصمة الرمز (SHA256)
     * @param scopes الصلاحيات
     * @return معرف الرمز، أو -1 عند الفشل
     */
    int64_t create_token(int64_t user_id, const std::string& token_name,
                         const std::string& token_hash, const std::string& scopes = "publish");

    /**
     * @brief التحقق من رمز وصول
     * @param token_hash بصمة الرمز
     * @return بيانات المستخدم صاحب الرمز، أو nullopt إذا كان الرمز غير صالح
     */
    std::optional<UserInfo> verify_token(const std::string& token_hash);

    /**
     * @brief إبطال رمز وصول
     */
    bool revoke_token(int64_t token_id);

    // ─────────────────────────────────────────────────────────────────────────
    // سجل المراجعة
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief تسجيل عملية في سجل المراجعة
     */
    void log_audit(int64_t user_id, const std::string& action,
                   const std::string& details = "{}",
                   const std::string& ip = "", const std::string& ua = "");

    // ─────────────────────────────────────────────────────────────────────────
    // البحث العربي
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief تحديث فهرس البحث العربي لحزمة
     * @param package_id معرف الحزمة
     * @param words الكلمات مع جذورها وترجماتها
     *
     * يُستدعى عند إنشاء حزمة أو تحديثها لتحديث فهرس البحث العربي.
     */
    void update_arabic_index(int64_t package_id,
                             const std::vector<std::tuple<std::string, std::string, std::string>>& words);

    /**
     * @brief البحث بالجذر العربي
     * @param root الجذر العربي (مثل: كتب، قرأ، حسب)
     * @return قائمة معرفات الحزم المطابقة
     */
    std::vector<int64_t> search_by_arabic_root(const std::string& root);

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات التقييمات والمراجعات
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إضافة أو تحديث مراجعة حزمة
     * @param review بيانات المراجعة
     * @return معرف المراجعة، أو -1 عند الفشل
     * 
     * إذا كان المستخدم قد راجع الحزمة سابقاً، يتم التحديث.
     * المحفّزات تُحدّث تلقائياً متوسط التقييم في جدول الحزم.
     */
    int64_t create_or_update_review(const ReviewInfo& review);

    /**
     * @brief جلب مراجعات حزمة
     */
    std::vector<ReviewInfo> get_package_reviews(int64_t package_id, int limit = 50);

    /**
     * @brief جلب مراجعة مستخدم لحزمة
     */
    std::optional<ReviewInfo> get_user_review(int64_t package_id, int64_t user_id);

    /**
     * @brief حذف مراجعة (المراجِع أو المشرف)
     */
    bool delete_review(int64_t review_id);

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات التفضيلات (Stars)
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إضافة تفضيل (نجمة) لحزمة
     */
    bool star_package(int64_t package_id, int64_t user_id);

    /**
     * @brief إزالة تفضيل من حزمة
     */
    bool unstar_package(int64_t package_id, int64_t user_id);

    /**
     * @brief هل المستخدم يفضّل هذه الحزمة؟
     */
    bool is_starred(int64_t package_id, int64_t user_id);

    /**
     * @brief عدد تفضيلات حزمة
     */
    int64_t get_star_count(int64_t package_id);

    /**
     * @brief الحزم المفضّلة لمستخدم
     */
    std::vector<PackageInfo> get_user_stars(int64_t user_id);

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات الإبلاغات
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء بلاغ عن حزمة
     */
    int64_t create_report(const ReportInfo& report);

    /**
     * @brief جلب الإبلاغات (للمشرفين)
     */
    std::vector<ReportInfo> get_reports(const std::string& status = "pending", int limit = 50);

    /**
     * @brief تحديث حالة بلاغ (حل/رفض)
     */
    bool resolve_report(int64_t report_id, int64_t admin_id,
                        const std::string& status, const std::string& notes = "");

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات الخطّافات (Webhooks)
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء خطّاف جديد
     */
    int64_t create_webhook(const WebhookInfo& webhook);

    /**
     * @brief خطّافات المستخدم
     */
    std::vector<WebhookInfo> get_user_webhooks(int64_t user_id);

    /**
     * @brief حذف خطّاف
     */
    bool delete_webhook(int64_t webhook_id, int64_t user_id);

    /**
     * @brief جلب الخطّافات المطلوب إطلاقها لحدث معين
     */
    std::vector<WebhookInfo> get_webhooks_for_event(int64_t package_id,
                                                     const std::string& event);

    // ─────────────────────────────────────────────────────────────────────────
    // إحصائيات التنزيل اليومية
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إحصائيات التنزيل اليومية لحزمة (آخر N يوم)
     */
    std::vector<DailyDownloadInfo> get_daily_downloads(int64_t package_id, int days = 30);

    // ─────────────────────────────────────────────────────────────────────────
    // إعدادات النظام
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief قراءة إعداد
     */
    std::string get_setting(const std::string& key, const std::string& default_val = "");

    /**
     * @brief تعيين إعداد
     */
    bool set_setting(const std::string& key, const std::string& value);

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات إدارية
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief ترقية مستخدم إلى مشرف
     */
    bool set_admin(int64_t user_id, bool is_admin);

    /**
     * @brief تمييز حزمة كمميزة/موثقة
     */
    bool set_package_featured(int64_t package_id, bool featured);
    bool set_package_verified(int64_t package_id, bool verified);

    /**
     * @brief جلب سجل المراجعة (المشرفين)
     */
    struct AuditEntry {
        int64_t id;
        int64_t user_id;
        std::string username;
        std::string action;
        std::string details;
        std::string ip;
        std::string created_at;
    };
    std::vector<AuditEntry> get_audit_log(int limit = 100, int64_t user_id = 0);

    /**
     * @brief الحصول على مقبض قاعدة البيانات الخام
     * 
     * يُستخدم من الأنظمة الفرعية (مثل الاتحاد) التي تحتاج
     * الوصول المباشر لقاعدة البيانات.
     * 
     * @return مؤشر void* إلى مقبض sqlite3 (أو nullptr)
     */
    void* get_handle() const { return db_; }

    // ═════════════════════════════════════════════════════════════════════════
    // ★ العمليات الثورية الجديدة ★
    // ═════════════════════════════════════════════════════════════════════════

    // ─────────────────────────────────────────────────────────────────────────
    // عمليات التوقيعات الرقمية
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إضافة مفتاح عام للناشر
     */
    int64_t add_publisher_key(const PublisherKeyInfo& key);

    /**
     * @brief جلب مفاتيح المستخدم
     */
    std::vector<PublisherKeyInfo> get_user_keys(int64_t user_id);

    /**
     * @brief جلب مفتاح بالبصمة
     */
    std::optional<PublisherKeyInfo> get_key_by_fingerprint(const std::string& fingerprint);

    /**
     * @brief إبطال مفتاح
     */
    bool revoke_key(int64_t key_id, const std::string& reason);

    /**
     * @brief إضافة توقيع على إصدار
     */
    int64_t add_version_signature(const VersionSignature& sig);

    /**
     * @brief جلب توقيعات إصدار
     */
    std::vector<VersionSignature> get_version_signatures(int64_t version_id);

    /**
     * @brief تسجيل نتيجة التحقق من التوقيع
     */
    void log_verification(int64_t version_id, const std::string& status,
                          const std::string& error = "", const std::string& ip = "",
                          const std::string& client_version = "");

    // ─────────────────────────────────────────────────────────────────────────
    // حل التبعيات
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إضافة تبعية متقدمة لإصدار
     */
    int64_t add_dependency_v2(const DependencyV2& dep);

    /**
     * @brief جلب تبعيات إصدار
     */
    std::vector<DependencyV2> get_dependencies_v2(int64_t version_id);

    /**
     * @brief جلب الحزم التي تعتمد على حزمة معينة (تبعيات عكسية)
     */
    std::vector<std::pair<std::string, std::string>> get_reverse_dependencies(
        const std::string& package_name, int limit = 100);

    /**
     * @brief البحث عن ذاكرة حل التبعيات
     */
    std::optional<DependencyCacheEntry> get_dependency_cache(const std::string& cache_key);

    /**
     * @brief تخزين نتيجة حل التبعيات
     */
    void cache_dependency_resolution(const std::string& cache_key,
                                     const std::string& resolved_tree,
                                     int resolution_time_ms);

    /**
     * @brief الإبلاغ عن تعارض
     */
    int64_t report_conflict(const KnownConflict& conflict);

    /**
     * @brief جلب التعارضات المعروفة لحزمة
     */
    std::vector<KnownConflict> get_conflicts_for_package(const std::string& package_name);

    // ─────────────────────────────────────────────────────────────────────────
    // الإشعارات
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء إشعار
     */
    int64_t create_notification(const NotificationInfo& notif);

    /**
     * @brief جلب إشعارات المستخدم
     */
    std::vector<NotificationInfo> get_user_notifications(int64_t user_id,
                                                          bool unread_only = false,
                                                          int limit = 50);

    /**
     * @brief تعليم إشعار كمقروء
     */
    bool mark_notification_read(int64_t notification_id, int64_t user_id);

    /**
     * @brief تعليم جميع إشعارات المستخدم كمقروءة
     */
    bool mark_all_notifications_read(int64_t user_id);

    /**
     * @brief عدد الإشعارات غير المقروءة
     */
    int64_t get_unread_notification_count(int64_t user_id);

    // ─────────────────────────────────────────────────────────────────────────
    // الاشتراكات
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء اشتراك (متابعة)
     */
    int64_t create_subscription(const SubscriptionInfo& sub);

    /**
     * @brief إلغاء اشتراك
     */
    bool delete_subscription(int64_t user_id, const std::string& target_type,
                            int64_t target_id);

    /**
     * @brief جلب اشتراكات المستخدم
     */
    std::vector<SubscriptionInfo> get_user_subscriptions(int64_t user_id);

    /**
     * @brief جلب المشتركين في هدف (لإرسال الإشعارات)
     */
    std::vector<int64_t> get_subscribers(const std::string& target_type,
                                         int64_t target_id);

    // ─────────────────────────────────────────────────────────────────────────
    // المنظمات
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء منظمة
     */
    int64_t create_organization(const OrganizationInfo& org);

    /**
     * @brief جلب منظمة بالاسم
     */
    std::optional<OrganizationInfo> get_organization(const std::string& name);

    /**
     * @brief تحديث منظمة
     */
    bool update_organization(const OrganizationInfo& org);

    /**
     * @brief إضافة عضو لمنظمة
     */
    bool add_org_member(int64_t org_id, int64_t user_id,
                        const std::string& role, int64_t invited_by);

    /**
     * @brief إزالة عضو من منظمة
     */
    bool remove_org_member(int64_t org_id, int64_t user_id);

    /**
     * @brief جلب أعضاء منظمة
     */
    std::vector<OrgMemberInfo> get_org_members(int64_t org_id);

    /**
     * @brief جلب منظمات المستخدم
     */
    std::vector<OrganizationInfo> get_user_organizations(int64_t user_id);

    /**
     * @brief التحقق من صلاحية المستخدم في منظمة
     */
    std::string get_user_org_role(int64_t org_id, int64_t user_id);

    // ─────────────────────────────────────────────────────────────────────────
    // التنبيهات الأمنية
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إنشاء تنبيه أمني
     */
    int64_t create_security_advisory(const SecurityAdvisory& advisory);

    /**
     * @brief جلب التنبيهات الأمنية لحزمة
     */
    std::vector<SecurityAdvisory> get_package_advisories(int64_t package_id,
                                                          bool public_only = true);

    /**
     * @brief جلب التنبيهات الأمنية الحديثة
     */
    std::vector<SecurityAdvisory> get_recent_advisories(int limit = 20);

    /**
     * @brief تحديث تنبيه أمني
     */
    bool update_security_advisory(const SecurityAdvisory& advisory);

    /**
     * @brief التحقق من وجود تنبيهات أمنية لإصدار
     */
    std::vector<SecurityAdvisory> check_version_advisories(const std::string& package_name,
                                                            const std::string& version);

    // ─────────────────────────────────────────────────────────────────────────
    // المرايا
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إضافة مرآة
     */
    int64_t add_mirror(const RegistryMirror& mirror);

    /**
     * @brief جلب المرايا النشطة
     */
    std::vector<RegistryMirror> get_active_mirrors();

    /**
     * @brief تحديث حالة المرآة
     */
    bool update_mirror_status(int64_t mirror_id, const std::string& status,
                              int64_t packages_synced);

    // ─────────────────────────────────────────────────────────────────────────
    // التحليلات المتقدمة
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief تسجيل تنزيل مفصّل (للتحليلات)
     */
    void record_download_analytics(const DownloadAnalytics& analytics);

    /**
     * @brief جلب إحصائيات التنزيل حسب البلد
     */
    std::vector<std::pair<std::string, int64_t>> get_downloads_by_country(
        int64_t package_id, int days = 30);

    /**
     * @brief جلب إحصائيات التنزيل حسب نظام التشغيل
     */
    std::vector<std::pair<std::string, int64_t>> get_downloads_by_os(
        int64_t package_id, int days = 30);

    /**
     * @brief تحديث/إنشاء إحصائيات ساعية
     */
    void update_hourly_stats(int64_t package_id, int64_t version_id,
                             const std::string& stat_type, int count = 1);

    /**
     * @brief جلب إحصائيات ساعية
     */
    std::vector<HourlyStats> get_hourly_stats(int64_t package_id,
                                               const std::string& stat_type,
                                               int hours = 24);

    /**
     * @brief تسجيل تحليلات البحث
     */
    void record_search_analytics(const std::string& query, int result_count,
                                 int search_time_ms, int64_t clicked_package = 0,
                                 int clicked_position = 0);

    /**
     * @brief جلب عمليات البحث الشائعة
     */
    std::vector<std::pair<std::string, int64_t>> get_popular_searches(int limit = 20);

    // ─────────────────────────────────────────────────────────────────────────
    // قائمة انتظار المهام
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief إضافة مهمة للمعالجة الخلفية
     */
    int64_t enqueue_job(const std::string& job_type, const std::string& payload,
                        int priority = 5, int delay_seconds = 0);

    /**
     * @brief جلب المهمة التالية للمعالجة
     */
    std::optional<JobInfo> dequeue_job(const std::string& worker_id);

    /**
     * @brief تحديث حالة مهمة
     */
    bool update_job_status(int64_t job_id, const std::string& status,
                           const std::string& result = "", const std::string& error = "");

    /**
     * @brief إعادة جدولة المهام الفاشلة
     */
    int reschedule_failed_jobs();

    // ─────────────────────────────────────────────────────────────────────────
    // تحديد المعدل
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief التحقق من حد المعدل وتسجيل الطلب
     * @return true إذا كان الطلب مسموحاً، false إذا تجاوز الحد
     */
    bool check_rate_limit(const std::string& identifier,
                          const std::string& identifier_type,
                          const std::string& endpoint_pattern,
                          int limit_per_window, int window_seconds);

    /**
     * @brief جلب حالة تحديد المعدل
     */
    std::optional<RateLimitInfo> get_rate_limit_status(const std::string& identifier,
                                                        const std::string& endpoint_pattern);

    /**
     * @brief حظر معرّف
     */
    bool block_identifier(const std::string& identifier,
                          const std::string& identifier_type,
                          int duration_seconds);

    /**
     * @brief إلغاء حظر معرّف
     */
    bool unblock_identifier(const std::string& identifier);

    // ─────────────────────────────────────────────────────────────────────────
    // التخزين المؤقت
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief جلب من ذاكرة API المؤقتة
     */
    std::optional<std::pair<std::string, std::string>> get_api_cache(
        const std::string& cache_key);

    /**
     * @brief تخزين في ذاكرة API المؤقتة
     */
    void set_api_cache(const std::string& cache_key, const std::string& response,
                       const std::string& content_type, int ttl_seconds);

    /**
     * @brief إبطال جزء من الذاكرة المؤقتة
     */
    void invalidate_cache(const std::string& pattern);

    /**
     * @brief تنظيف الذاكرة المنتهية
     */
    int cleanup_expired_cache();

    // ─────────────────────────────────────────────────────────────────────────
    // سجل الأحداث
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief تسجيل حدث في السجل
     */
    void log_event(const std::string& event_type, const std::string& event_subtype,
                   const std::string& actor_type, int64_t actor_id,
                   const std::string& target_type, int64_t target_id,
                   const std::string& metadata_json = "{}",
                   const std::string& request_id = "");

    /**
     * @brief جلب أحداث حزمة
     */
    struct EventLogEntry {
        int64_t id;
        std::string event_type;
        std::string event_subtype;
        std::string actor_type;
        int64_t actor_id;
        std::string target_type;
        int64_t target_id;
        std::string metadata;
        std::string created_at;
    };
    std::vector<EventLogEntry> get_package_events(int64_t package_id, int limit = 50);

    /**
     * @brief جلب أحداث المستخدم
     */
    std::vector<EventLogEntry> get_user_events(int64_t user_id, int limit = 50);

private:
    // ─────────────────────────────────────────────────────────────────────────
    // الأعضاء الخاصة
    // ─────────────────────────────────────────────────────────────────────────

    std::string db_path_;            // مسار ملف قاعدة البيانات
    void* db_ = nullptr;            // مقبض SQLite (sqlite3*)
    bool is_initialized_ = false;   // هل تمت التهيئة؟

    // ─────────────────────────────────────────────────────────────────────────
    // دوال مساعدة داخلية
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * @brief تنفيذ ملف SQL
     */
    bool execute_sql_file(const std::string& path);

    /**
     * @brief تنفيذ استعلام SQL
     */
    bool execute(const std::string& sql);

    /**
     * @brief الحصول على إصدار المخطط الحالي
     */
    int get_schema_version();

    /**
     * @brief تسجيل رسالة
     */
    void log(const std::string& level, const std::string& message);
};

} // namespace registry
} // namespace sad
