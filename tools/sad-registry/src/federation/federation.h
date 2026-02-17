// بسم الله الرحمن الرحيم
#pragma once
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: federation.h
 * الوصف: بروتوكول الاتحاد (Federation) لربط مستودعات حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * نظام الاتحاد يسمح بربط عدة مستودعات معاً في شبكة لامركزية:
 *
 *   ● كل مستودع يمكنه العمل مستقلاً (centralized)
 *   ● يمكن ربط المستودعات ببعضها (federated)
 *   ● البحث يشمل جميع المستودعات المتصلة
 *   ● النسخ الاحتياطي التلقائي للحزم المهمة
 *
 * الهندسة:
 *
 *   ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
 *   │ مستودع أ    │◄───►│ مستودع ب    │◄───►│ مستودع ج    │
 *   │ (رئيسي)    │     │ (جامعة)     │     │ (شركة)      │
 *   └──────┬──────┘     └──────┬──────┘     └──────┬──────┘
 *          │                   │                   │
 *          └───────────────────┼───────────────────┘
 *                              │
 *                     ┌────────▼────────┐
 *                     │  بحث موحد      │
 *                     │  حزم مشتركة   │
 *                     │  مزامنة تلقائية│
 *                     └────────────────┘
 *
 * البروتوكول:
 *
 *   ١. التسجيل (Registration):
 *      POST /api/v1/federation/register
 *      يسجل مستودع بعيد للاتصال
 *
 *   ٢. المصافحة (Handshake):
 *      GET /api/v1/federation/handshake
 *      تبادل المعلومات الأساسية (اسم، إصدار، عدد الحزم)
 *
 *   ٣. المزامنة (Sync):
 *      GET /api/v1/federation/sync?since=TIMESTAMP
 *      جلب التحديثات منذ آخر مزامنة
 *
 *   ٤. البحث الموزع (Distributed Search):
 *      GET /api/v1/federation/search?q=QUERY
 *      بحث في المستودع البعيد
 *
 *   ٥. جلب حزمة (Fetch Package):
 *      GET /api/v1/federation/package/:name/:version
 *      تحميل حزمة من مستودع بعيد
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <memory>
#include <functional>
#include <chrono>
#include <mutex>

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// هياكل البيانات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief حالة اتصال المستودع البعيد
 */
enum class PeerStatus {
    Active,       // متصل ونشط
    Inactive,     // مسجل لكن غير متصل
    Syncing,      // قيد المزامنة
    Error,        // خطأ في الاتصال
    Banned        // محظور (لأسباب أمنية)
};

/**
 * @brief معلومات مستودع بعيد (Peer)
 *
 * كل مستودع متصل يُمثَّل بهذا الهيكل.
 */
struct FederatedPeer {
    int64_t id;                       // المعرف في قاعدة البيانات المحلية
    std::string name;                 // اسم المستودع البعيد
    std::string url;                  // رابط API الأساسي
    std::string api_key;              // مفتاح API للمصادقة
    std::string description;          // وصف المستودع
    PeerStatus status;                // الحالة الحالية
    int64_t package_count;            // عدد الحزم لديه
    int64_t last_sync_timestamp;      // آخر مزامنة (Unix timestamp)
    int64_t trust_level;              // مستوى الثقة (0-100)
    std::string version;              // إصدار بروتوكول الاتحاد
};

/**
 * @brief نتيجة المصافحة
 *
 * البيانات المتبادلة عند أول اتصال بين مستودعين.
 */
struct HandshakeResult {
    bool success;                     // هل نجحت المصافحة
    std::string peer_name;            // اسم المستودع البعيد
    std::string peer_version;         // إصدار البروتوكول
    int64_t peer_packages;            // عدد حزمه
    std::string peer_description;     // وصفه
    std::string error_message;        // رسالة خطأ (إن فشلت)
};

/**
 * @brief حزمة بعيدة
 *
 * معلومات حزمة مستضافة في مستودع آخر.
 */
struct RemotePackage {
    std::string name;                 // اسم الحزمة
    std::string arabic_name;          // الاسم العربي
    std::string version;              // آخر إصدار
    std::string description;          // الوصف
    std::string description_ar;       // الوصف العربي
    std::string registry_url;         // رابط المستودع المستضيف
    std::string registry_name;        // اسم المستودع المستضيف
    int64_t downloads;                // عدد التنزيلات الكلي
    std::string checksum;             // بصمة الحزمة (SHA-256)
};

/**
 * @brief نتيجة بحث موزع
 *
 * تجمع نتائج من عدة مستودعات.
 */
struct FederatedSearchResult {
    std::vector<RemotePackage> packages;  // الحزم الموجودة
    int total_registries_searched;         // عدد المستودعات المبحوثة
    int successful_registries;             // عدد المستودعات التي استجابت
    int failed_registries;                 // عدد المستودعات التي فشلت
    double search_time_ms;                 // وقت البحث بالمللي ثانية
};

/**
 * @brief عنصر مزامنة
 *
 * تغيير واحد حصل في مستودع بعيد.
 */
struct SyncItem {
    std::string action;               // "publish" | "yank" | "update"
    std::string package_name;         // اسم الحزمة
    std::string version;              // الإصدار
    int64_t timestamp;                // وقت التغيير
    std::string checksum;             // بصمة الحزمة
};

/**
 * @brief نتيجة المزامنة
 */
struct SyncResult {
    bool success;
    int items_synced;                 // عدد العناصر المُزامنة
    int64_t last_sync_timestamp;      // آخر وقت مزامنة
    std::string error_message;
    std::vector<SyncItem> items;      // العناصر المُزامنة
};

// ═══════════════════════════════════════════════════════════════════════════════
// محرك الاتحاد
// ═══════════════════════════════════════════════════════════════════════════════

// إعلان مسبق
class Database;

/**
 * @brief محرك الاتحاد (Federation Engine)
 *
 * المسؤول عن:
 *   ● إدارة المستودعات المتصلة (Peers)
 *   ● تنفيذ عمليات المزامنة
 *   ● البحث الموزع
 *   ● جلب الحزم من المستودعات البعيدة
 *   ● مراقبة صحة الاتصالات
 *
 * الاستخدام:
 *   FederationEngine fed(db);
 *   fed.initialize();
 *   fed.register_peer("مستودع الجامعة", "https://sad-pkg.university.edu");
 *   auto results = fed.search("شبكة");
 */
class FederationEngine {
public:
    /**
     * المُنشئ
     * @param db قاعدة البيانات المحلية (لحفظ معلومات المستودعات)
     */
    explicit FederationEngine(std::shared_ptr<Database> db);

    /**
     * تهيئة المحرك وتحميل المستودعات المسجلة
     */
    bool initialize();

    // ─────────────────────────────────────────────────────────────────
    // إدارة المستودعات البعيدة
    // ─────────────────────────────────────────────────────────────────

    /**
     * تسجيل مستودع بعيد جديد
     * @param name اسم المستودع
     * @param url رابط API الأساسي
     * @param api_key مفتاح المصادقة (اختياري)
     * @return true إذا نجح التسجيل والمصافحة
     */
    bool register_peer(
        const std::string& name,
        const std::string& url,
        const std::string& api_key = "");

    /**
     * إلغاء تسجيل مستودع بعيد
     */
    bool unregister_peer(int64_t peer_id);

    /**
     * الحصول على قائمة المستودعات المسجلة
     */
    std::vector<FederatedPeer> get_peers() const;

    /**
     * الحصول على معلومات مستودع بعيد محدد
     */
    FederatedPeer get_peer(int64_t peer_id) const;

    /**
     * تحديث حالة مستودع بعيد
     */
    bool update_peer_status(int64_t peer_id, PeerStatus status);

    // ─────────────────────────────────────────────────────────────────
    // المصافحة والمزامنة
    // ─────────────────────────────────────────────────────────────────

    /**
     * تنفيذ مصافحة مع مستودع بعيد
     * يتبادل المعلومات الأساسية ويتحقق من التوافق.
     */
    HandshakeResult handshake(const std::string& peer_url);

    /**
     * مزامنة مع مستودع بعيد
     * يجلب التغييرات منذ آخر مزامنة.
     */
    SyncResult sync_with_peer(int64_t peer_id);

    /**
     * مزامنة مع جميع المستودعات النشطة
     */
    std::vector<SyncResult> sync_all();

    // ─────────────────────────────────────────────────────────────────
    // البحث الموزع
    // ─────────────────────────────────────────────────────────────────

    /**
     * بحث موزع في جميع المستودعات المتصلة
     *
     * يرسل الاستعلام بالتوازي لجميع المستودعات النشطة
     * ويجمع النتائج مع إزالة التكرار.
     *
     * @param query نص البحث (عربي أو إنجليزي)
     * @param max_results الحد الأقصى للنتائج
     * @param timeout_ms الحد الأقصى لوقت الانتظار
     */
    FederatedSearchResult search(
        const std::string& query,
        int max_results = 20,
        int timeout_ms = 5000);

    // ─────────────────────────────────────────────────────────────────
    // جلب الحزم
    // ─────────────────────────────────────────────────────────────────

    /**
     * جلب معلومات حزمة من مستودع بعيد
     */
    RemotePackage fetch_package_info(
        const std::string& peer_url,
        const std::string& package_name);

    /**
     * تحميل ملف حزمة من مستودع بعيد
     * @return البيانات الثنائية للحزمة
     */
    std::vector<uint8_t> download_package(
        const std::string& peer_url,
        const std::string& package_name,
        const std::string& version);

    // ─────────────────────────────────────────────────────────────────
    // المراقبة والصحة
    // ─────────────────────────────────────────────────────────────────

    /**
     * فحص صحة جميع المستودعات المتصلة
     * يُحدِّث حالة كل مستودع.
     */
    void health_check_all();

    /**
     * فحص صحة مستودع واحد
     */
    bool health_check(int64_t peer_id);

    // ─────────────────────────────────────────────────────────────────
    // الإعدادات
    // ─────────────────────────────────────────────────────────────────

    /**
     * تعيين الاسم المحلي (يُرسل في المصافحة)
     */
    void set_local_name(const std::string& name) { local_name_ = name; }

    /**
     * تعيين الوصف المحلي
     */
    void set_local_description(const std::string& desc) { local_description_ = desc; }

    /**
     * تعيين إصدار البروتوكول
     */
    static std::string protocol_version() { return "1.0.0"; }

private:
    // ─────────────────────────────────────────────────────────────────
    // البيانات الداخلية
    // ─────────────────────────────────────────────────────────────────

    /// قاعدة البيانات المحلية
    std::shared_ptr<Database> db_;

    /// المستودعات المسجلة (محمَّلة في الذاكرة)
    std::vector<FederatedPeer> peers_;

    /// قفل للوصول المتزامن
    mutable std::mutex peers_mutex_;

    /// الاسم المحلي
    std::string local_name_ = "\xD9\x85\xD8\xB3\xD8\xAA\xD9\x88\xD8\xAF\xD8\xB9 \xD8\xB5"; // مستودع ص

    /// الوصف المحلي
    std::string local_description_;

    // ─────────────────────────────────────────────────────────────────
    // دوال مساعدة
    // ─────────────────────────────────────────────────────────────────

    /**
     * إرسال طلب HTTP GET لمستودع بعيد
     */
    std::string http_get(const std::string& url, int timeout_ms = 5000);

    /**
     * إرسال طلب HTTP POST لمستودع بعيد
     */
    std::string http_post(const std::string& url, const std::string& body,
                          int timeout_ms = 5000);

    /**
     * تحليل استجابة JSON من مستودع بعيد
     */
    std::vector<RemotePackage> parse_search_response(
        const std::string& json_response,
        const std::string& registry_url,
        const std::string& registry_name);

    /**
     * تحميل المستودعات المسجلة من قاعدة البيانات
     */
    void load_peers_from_db();

    /**
     * حفظ/تحديث مستودع في قاعدة البيانات
     */
    void save_peer_to_db(const FederatedPeer& peer);
};

} // namespace registry
} // namespace sad
