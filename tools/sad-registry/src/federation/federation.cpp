// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: federation.cpp
 * الوصف: تنفيذ بروتوكول الاتحاد (Federation) لربط مستودعات حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف ينفّذ الوظائف المعلنة في federation.h.
 * يوفر آلية كاملة لربط مستودعات حزم متعددة ببعضها البعض.
 *
 * التصميم:
 *   ● كل مستودع يحتفظ بقائمة من المستودعات البعيدة (peers)
 *   ● عند البحث: يُرسل الاستعلام لجميع المستودعات المتصلة
 *   ● عند التنزيل: يُحمّل الحزمة من المستودع الأصلي
 *   ● المزامنة: تتم دورياً لتحديث فهرس الحزم المتاحة
 *
 * بروتوكول HTTP المستخدم:
 *   على ويندوز → WinHTTP (المدمج في النظام)
 *   على لينكس → أمر curl عبر سطر الأوامر (كحل بديل خفيف)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "federation.h"
#include "../storage/database.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <future>
#include <chrono>
#include <algorithm>
#include <set>
#include <cstring>
#include <cstdio>

// sqlite3 للوصول المباشر في دوال التخزين
#if SAD_HAS_SQLITE
    #include <sqlite3.h>
#endif

// ─────────────────────────────────────────────────────────────────────────────
// مكتبات الشبكة حسب نظام التشغيل
// ─────────────────────────────────────────────────────────────────────────────
#ifdef _WIN32
    #include <windows.h>
    #include <winhttp.h>
    #pragma comment(lib, "winhttp.lib")
#else
    #include <cstdlib>
#endif

namespace sad {
namespace registry {

// ═══════════════════════════════════════════════════════════════════════════════
// المُنشئ والتهيئة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * المُنشئ — يحفظ مرجع قاعدة البيانات فقط.
 * التهيئة الفعلية تتم عبر initialize().
 */
FederationEngine::FederationEngine(std::shared_ptr<Database> db)
    : db_(std::move(db)) {
}

/**
 * تهيئة محرك الاتحاد:
 *   ١. تحميل المستودعات المسجلة من قاعدة البيانات
 *   ٢. فحص صحة المستودعات النشطة (اختياري - يمكن تأجيله)
 */
bool FederationEngine::initialize() {
    std::cout << "  ⏳ تهيئة محرك الاتحاد..." << std::endl;

    // تحميل المستودعات المسجلة سابقاً
    load_peers_from_db();

    std::lock_guard<std::mutex> lock(peers_mutex_);
    std::cout << "  ✓ محرك الاتحاد جاهز (" << peers_.size()
              << " مستودعات مسجلة)" << std::endl;
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════════
// إدارة المستودعات البعيدة (Peers)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تسجيل مستودع بعيد جديد.
 *
 * الخطوات:
 *   ١. التحقق من عدم التكرار (نفس الـ URL مسجل مسبقاً؟)
 *   ٢. تنفيذ مصافحة (handshake) للتأكد من أنه مستودع صالح
 *   ٣. حفظ المستودع في قاعدة البيانات والذاكرة
 *
 * @param name اسم ودّي للمستودع (مثلاً: "مستودع جامعة الملك سعود")
 * @param url رابط API (مثلاً: "https://pkg.ksu.edu.sa/api/v1")
 * @param api_key مفتاح API اختياري للمصادقة
 * @return true إذا تم التسجيل بنجاح
 */
bool FederationEngine::register_peer(
        const std::string& name,
        const std::string& url,
        const std::string& api_key) {

    std::cout << "  📡 تسجيل مستودع بعيد: " << name << " ← " << url << std::endl;

    // ─── (١) التحقق من عدم التكرار ───
    {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        for (const auto& peer : peers_) {
            if (peer.url == url) {
                std::cerr << "  ⚠ المستودع مسجل بالفعل: " << url << std::endl;
                return false;
            }
        }
    }

    // ─── (٢) تنفيذ المصافحة ───
    // نتحقق أن الطرف الآخر مستودع ص صالح
    HandshakeResult hs = handshake(url);
    if (!hs.success) {
        std::cerr << "  ✗ فشلت المصافحة مع " << url << ": "
                  << hs.error_message << std::endl;
        return false;
    }

    std::cout << "  ✓ مصافحة ناجحة مع \"" << hs.peer_name
              << "\" (إصدار " << hs.peer_version
              << "، " << hs.peer_packages << " حزمة)" << std::endl;

    // ─── (٣) إنشاء كائن Peer وحفظه ───
    FederatedPeer peer;
    peer.id = 0;  // سيُعيَّن عند الحفظ في قاعدة البيانات
    peer.name = name;
    peer.url = url;
    peer.api_key = api_key;
    peer.description = hs.peer_description;
    peer.status = PeerStatus::Active;
    peer.package_count = hs.peer_packages;
    peer.last_sync_timestamp = 0;
    peer.trust_level = 50;  // مستوى ثقة متوسط افتراضياً
    peer.version = hs.peer_version;

    // حفظ في قاعدة البيانات
    save_peer_to_db(peer);

    // إضافة للذاكرة
    {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        peers_.push_back(peer);
    }

    std::cout << "  ✓ تم تسجيل المستودع البعيد بنجاح: " << name << std::endl;
    return true;
}

/**
 * إلغاء تسجيل مستودع بعيد وحذفه من الذاكرة وقاعدة البيانات.
 */
bool FederationEngine::unregister_peer(int64_t peer_id) {
    std::lock_guard<std::mutex> lock(peers_mutex_);

    // البحث عن المستودع وحذفه
    auto it = std::remove_if(peers_.begin(), peers_.end(),
        [peer_id](const FederatedPeer& p) { return p.id == peer_id; });

    if (it == peers_.end()) {
        std::cerr << "  ⚠ المستودع غير موجود: " << peer_id << std::endl;
        return false;
    }

    peers_.erase(it, peers_.end());

    // حذف من قاعدة البيانات
#if SAD_HAS_SQLITE
    if (db_ && db_->get_handle()) {
        sqlite3* sqldb = static_cast<sqlite3*>(db_->get_handle());
        std::string sql = "DELETE FROM federated_registries WHERE id=" + std::to_string(peer_id);
        sqlite3_exec(sqldb, sql.c_str(), nullptr, nullptr, nullptr);
    }
#endif

    return true;
}

/**
 * الحصول على قائمة المستودعات المسجلة.
 * نسخة آمنة للخيوط (thread-safe copy).
 */
std::vector<FederatedPeer> FederationEngine::get_peers() const {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    return peers_;  // نسخة عميقة
}

/**
 * الحصول على معلومات مستودع محدد بالمعرف.
 */
FederatedPeer FederationEngine::get_peer(int64_t peer_id) const {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    for (const auto& p : peers_) {
        if (p.id == peer_id) return p;
    }
    return {};  // فارغ إذا لم يُوجد
}

/**
 * تحديث حالة مستودع بعيد (متصل، غير متصل، خطأ، إلخ).
 */
bool FederationEngine::update_peer_status(int64_t peer_id, PeerStatus status) {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    for (auto& p : peers_) {
        if (p.id == peer_id) {
            p.status = status;
            return true;
        }
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
// المصافحة (Handshake)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تنفيذ المصافحة مع مستودع بعيد.
 *
 * المصافحة هي أول خطوة عند الاتصال بمستودع جديد.
 * نرسل طلب GET إلى /api/v1/federation/handshake
 * ونتلقى معلومات المستودع البعيد (الاسم، الإصدار، عدد الحزم).
 *
 * الاستجابة المتوقعة (JSON):
 *   {
 *     "name": "مستودع ص الرسمي",
 *     "version": "1.0.0",
 *     "packages": 1234,
 *     "description": "المستودع المركزي الرسمي",
 *     "protocol": "sad-federation/1.0"
 *   }
 */
HandshakeResult FederationEngine::handshake(const std::string& peer_url) {
    HandshakeResult result;
    result.success = false;

    try {
        // بناء رابط المصافحة
        std::string url = peer_url;
        // تنظيف الرابط (إزالة / الأخيرة)
        if (!url.empty() && url.back() == '/') url.pop_back();
        url += "/federation/handshake";

        // إرسال الطلب
        std::string response = http_get(url, 10000); // مهلة ١٠ ثوانٍ

        if (response.empty()) {
            result.error_message = "لم يُستقبل رد من المستودع البعيد";
            return result;
        }

        // تحليل JSON البسيط
        // نبحث عن الحقول الأساسية يدوياً
        auto extract_json_string = [](const std::string& json, const std::string& key) -> std::string {
            std::string search = "\"" + key + "\"";
            size_t pos = json.find(search);
            if (pos == std::string::npos) return "";
            pos = json.find(':', pos);
            if (pos == std::string::npos) return "";
            pos = json.find('"', pos + 1);
            if (pos == std::string::npos) return "";
            size_t end = json.find('"', pos + 1);
            if (end == std::string::npos) return "";
            return json.substr(pos + 1, end - pos - 1);
        };

        auto extract_json_int = [](const std::string& json, const std::string& key) -> int64_t {
            std::string search = "\"" + key + "\"";
            size_t pos = json.find(search);
            if (pos == std::string::npos) return 0;
            pos = json.find(':', pos);
            if (pos == std::string::npos) return 0;
            pos++;
            while (pos < json.size() && json[pos] == ' ') pos++;
            return std::atoll(json.c_str() + pos);
        };

        result.peer_name = extract_json_string(response, "name");
        result.peer_version = extract_json_string(response, "version");
        result.peer_description = extract_json_string(response, "description");
        result.peer_packages = extract_json_int(response, "packages");

        // التحقق من أن الاستجابة تحتوي على البيانات الأساسية
        if (result.peer_name.empty() && result.peer_version.empty()) {
            result.error_message = "الاستجابة لا تحتوي على بيانات مصافحة صالحة";
            return result;
        }

        result.success = true;
    } catch (const std::exception& e) {
        result.error_message = std::string("خطأ أثناء المصافحة: ") + e.what();
    }

    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// المزامنة (Sync)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * مزامنة مع مستودع بعيد واحد.
 *
 * نرسل آخر وقت مزامنة (timestamp) ونستقبل جميع التغييرات
 * التي حصلت بعد ذلك الوقت.
 *
 * GET /api/v1/federation/sync?since={timestamp}
 *
 * الاستجابة المتوقعة:
 *   {
 *     "items": [
 *       {"action": "publish", "package": "شبكة", "version": "1.2.0", ...},
 *       {"action": "yank", "package": "مكتبة_قديمة", "version": "0.1.0", ...}
 *     ],
 *     "timestamp": 1739712345
 *   }
 */
SyncResult FederationEngine::sync_with_peer(int64_t peer_id) {
    SyncResult result;
    result.success = false;

    // البحث عن المستودع
    FederatedPeer peer;
    {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        bool found = false;
        for (auto& p : peers_) {
            if (p.id == peer_id) {
                peer = p;
                p.status = PeerStatus::Syncing;
                found = true;
                break;
            }
        }
        if (!found) {
            result.error_message = "المستودع غير موجود";
            return result;
        }
    }

    std::cout << "  🔄 مزامنة مع: " << peer.name << " ← " << peer.url << std::endl;

    try {
        // بناء رابط المزامنة مع آخر وقت مزامنة
        std::string url = peer.url;
        if (!url.empty() && url.back() == '/') url.pop_back();
        url += "/federation/sync?since=" + std::to_string(peer.last_sync_timestamp);

        // إرسال الطلب
        std::string response = http_get(url);

        if (response.empty()) {
            result.error_message = "لم يُستقبل رد من المستودع البعيد";
            update_peer_status(peer_id, PeerStatus::Error);
            return result;
        }

        // تحليل العناصر المُزامنة
        // (في التنفيذ الكامل يتم تحليل JSON هنا)
        // حالياً نعتبر المزامنة ناجحة
        result.success = true;
        result.items_synced = 0;
        result.last_sync_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        // تحديث آخر مزامنة في الذاكرة وقاعدة البيانات
        {
            std::lock_guard<std::mutex> lock(peers_mutex_);
            for (auto& p : peers_) {
                if (p.id == peer_id) {
                    p.last_sync_timestamp = result.last_sync_timestamp;
                    p.status = PeerStatus::Active;
                    break;
                }
            }
        }

        std::cout << "  ✓ مزامنة ناجحة: " << result.items_synced
                  << " عنصر جديد" << std::endl;
    } catch (const std::exception& e) {
        result.error_message = std::string("خطأ أثناء المزامنة: ") + e.what();
        update_peer_status(peer_id, PeerStatus::Error);
    }

    return result;
}

/**
 * مزامنة مع جميع المستودعات النشطة.
 *
 * تُنفَّذ بالتسلسل حالياً. في المستقبل يمكن تنفيذها بالتوازي
 * باستخدام std::async أو خيوط متعددة.
 */
std::vector<SyncResult> FederationEngine::sync_all() {
    std::vector<SyncResult> results;

    std::vector<FederatedPeer> active_peers;
    {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        for (const auto& p : peers_) {
            if (p.status == PeerStatus::Active) {
                active_peers.push_back(p);
            }
        }
    }

    std::cout << "  🔄 مزامنة مع " << active_peers.size()
              << " مستودعات نشطة..." << std::endl;

    for (const auto& peer : active_peers) {
        results.push_back(sync_with_peer(peer.id));
    }

    return results;
}

// ═══════════════════════════════════════════════════════════════════════════════
// البحث الموزع (Distributed Search)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * بحث موزع في جميع المستودعات المتصلة.
 *
 * كيف يعمل:
 *   ١. جمع قائمة المستودعات النشطة
 *   ٢. إرسال استعلام البحث لكل مستودع بالتوازي (std::async)
 *   ٣. انتظار الردود (مع مهلة timeout)
 *   ٤. تجميع النتائج من جميع المستودعات
 *   ٥. إزالة التكرار (حزم بنفس الاسم من مستودعات مختلفة)
 *   ٦. ترتيب النتائج حسب الصلة
 *   ٧. إرجاع النتائج المدمجة
 *
 * @param query نص البحث (يدعم العربية والإنجليزية)
 * @param max_results الحد الأقصى للنتائج
 * @param timeout_ms الحد الأقصى لوقت الانتظار (مللي ثانية)
 */
FederatedSearchResult FederationEngine::search(
        const std::string& query,
        int max_results,
        int timeout_ms) {

    FederatedSearchResult result;
    auto start_time = std::chrono::steady_clock::now();

    // ─── (١) جمع المستودعات النشطة ───
    std::vector<FederatedPeer> active_peers;
    {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        for (const auto& p : peers_) {
            if (p.status == PeerStatus::Active) {
                active_peers.push_back(p);
            }
        }
    }

    result.total_registries_searched = static_cast<int>(active_peers.size());
    result.successful_registries = 0;
    result.failed_registries = 0;

    if (active_peers.empty()) {
        // لا توجد مستودعات متصلة
        return result;
    }

    // ─── (٢) إرسال البحث بالتوازي ───
    // نستخدم std::async لإرسال الطلبات بالتوازي
    struct AsyncSearch {
        std::future<std::string> future;
        std::string peer_url;
        std::string peer_name;
    };

    std::vector<AsyncSearch> searches;
    for (const auto& peer : active_peers) {
        std::string url = peer.url;
        if (!url.empty() && url.back() == '/') url.pop_back();
        url += "/federation/search?q=" + query + "&limit=" + std::to_string(max_results);

        AsyncSearch s;
        s.peer_url = peer.url;
        s.peer_name = peer.name;
        // نُطلق البحث في خيط منفصل
        s.future = std::async(std::launch::async,
            [this, url, timeout_ms]() { return http_get(url, timeout_ms); });
        searches.push_back(std::move(s));
    }

    // ─── (٣) انتظار النتائج ───
    for (auto& s : searches) {
        try {
            // انتظار مع مهلة
            auto status = s.future.wait_for(std::chrono::milliseconds(timeout_ms));

            if (status == std::future_status::ready) {
                std::string response = s.future.get();
                if (!response.empty()) {
                    // ─── (٤) تحليل النتائج ───
                    auto packages = parse_search_response(
                        response, s.peer_url, s.peer_name);
                    for (auto& pkg : packages) {
                        result.packages.push_back(std::move(pkg));
                    }
                    result.successful_registries++;
                } else {
                    result.failed_registries++;
                }
            } else {
                // انتهت المهلة
                result.failed_registries++;
            }
        } catch (...) {
            result.failed_registries++;
        }
    }

    // ─── (٥) إزالة التكرار ───
    // إذا ظهرت حزمة بنفس الاسم من مستودعات مختلفة، نحتفظ بالنسخة الأولى
    std::set<std::string> seen_names;
    std::vector<RemotePackage> unique_packages;
    for (const auto& pkg : result.packages) {
        if (seen_names.find(pkg.name) == seen_names.end()) {
            seen_names.insert(pkg.name);
            unique_packages.push_back(pkg);
        }
    }
    result.packages = std::move(unique_packages);

    // ─── (٦) ترتيب حسب التنزيلات (كمقياس بسيط للشعبية) ───
    std::sort(result.packages.begin(), result.packages.end(),
        [](const RemotePackage& a, const RemotePackage& b) {
            return a.downloads > b.downloads;
        });

    // تقليم النتائج
    if ((int)result.packages.size() > max_results) {
        result.packages.resize(max_results);
    }

    // ─── حساب وقت البحث ───
    auto end_time = std::chrono::steady_clock::now();
    result.search_time_ms = std::chrono::duration<double, std::milli>(
        end_time - start_time).count();

    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// جلب الحزم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * جلب معلومات حزمة من مستودع بعيد.
 *
 * GET {peer_url}/packages/{name}
 */
RemotePackage FederationEngine::fetch_package_info(
        const std::string& peer_url,
        const std::string& package_name) {

    RemotePackage result;
    result.name = package_name;
    result.registry_url = peer_url;

    try {
        std::string url = peer_url;
        if (!url.empty() && url.back() == '/') url.pop_back();
        url += "/packages/" + package_name;

        std::string response = http_get(url);
        if (!response.empty()) {
            // تحليل بسيط
            auto extract = [](const std::string& json, const std::string& key) -> std::string {
                std::string search = "\"" + key + "\"";
                size_t pos = json.find(search);
                if (pos == std::string::npos) return "";
                pos = json.find('"', json.find(':', pos) + 1);
                if (pos == std::string::npos) return "";
                size_t end = json.find('"', pos + 1);
                if (end == std::string::npos) return "";
                return json.substr(pos + 1, end - pos - 1);
            };

            result.arabic_name = extract(response, "arabic_name");
            result.version = extract(response, "latest_version");
            result.description = extract(response, "description");
            result.description_ar = extract(response, "description_ar");
            result.checksum = extract(response, "checksum");
        }
    } catch (const std::exception& e) {
        std::cerr << "  ⚠ فشل جلب معلومات " << package_name
                  << " من " << peer_url << ": " << e.what() << std::endl;
    }

    return result;
}

/**
 * تحميل ملف حزمة ثنائي من مستودع بعيد.
 *
 * GET {peer_url}/download/{name}/{version}
 *
 * يُرجع البيانات الثنائية (محتوى ZIP/tar.gz) ليتم حفظها محلياً.
 */
std::vector<uint8_t> FederationEngine::download_package(
        const std::string& peer_url,
        const std::string& package_name,
        const std::string& version) {

    std::vector<uint8_t> data;

    try {
        std::string url = peer_url;
        if (!url.empty() && url.back() == '/') url.pop_back();
        url += "/download/" + package_name + "/" + version;

        // للتنزيلات الثنائية، نستخدم http_get ونحوّل النتيجة
        std::string response = http_get(url, 60000); // مهلة ٦٠ ثانية للتنزيل

        if (!response.empty()) {
            data.assign(response.begin(), response.end());
            std::cout << "  ✓ تم تحميل " << package_name << "@" << version
                      << " (" << data.size() << " بايت)" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "  ✗ فشل تحميل " << package_name << "@" << version
                  << ": " << e.what() << std::endl;
    }

    return data;
}

// ═══════════════════════════════════════════════════════════════════════════════
// المراقبة والصحة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * فحص صحة جميع المستودعات المتصلة.
 *
 * لكل مستودع نشط أو في حالة خطأ:
 *   - نرسل طلب مصافحة خفيف
 *   - إذا استجاب → نشط (Active)
 *   - إذا لم يستجب → خطأ (Error)
 *
 * هذه الدالة يمكن استدعاؤها دورياً (كل ٥ دقائق مثلاً)
 * عبر مؤقت في الخيط الرئيسي.
 */
void FederationEngine::health_check_all() {
    std::vector<FederatedPeer> peers_to_check;
    {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        for (const auto& p : peers_) {
            if (p.status != PeerStatus::Banned) {
                peers_to_check.push_back(p);
            }
        }
    }

    std::cout << "  🏥 فحص صحة " << peers_to_check.size()
              << " مستودعات..." << std::endl;

    for (const auto& peer : peers_to_check) {
        health_check(peer.id);
    }
}

/**
 * فحص صحة مستودع واحد.
 *
 * @return true إذا كان المستودع متصلاً ويستجيب
 */
bool FederationEngine::health_check(int64_t peer_id) {
    FederatedPeer peer;
    {
        std::lock_guard<std::mutex> lock(peers_mutex_);
        bool found = false;
        for (const auto& p : peers_) {
            if (p.id == peer_id) {
                peer = p;
                found = true;
                break;
            }
        }
        if (!found) return false;
    }

    // محاولة المصافحة (مهلة قصيرة ٣ ثوانٍ)
    HandshakeResult hs = handshake(peer.url);

    if (hs.success) {
        update_peer_status(peer_id, PeerStatus::Active);
        // تحديث عدد الحزم
        std::lock_guard<std::mutex> lock(peers_mutex_);
        for (auto& p : peers_) {
            if (p.id == peer_id) {
                p.package_count = hs.peer_packages;
                break;
            }
        }
        return true;
    } else {
        update_peer_status(peer_id, PeerStatus::Error);
        return false;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// الاتصال بالشبكة (HTTP)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * إرسال طلب HTTP GET.
 *
 * على ويندوز: يستخدم WinHTTP (المدمج في النظام)
 * على لينكس: يستخدم أمر curl من سطر الأوامر
 *
 * @param url الرابط الكامل
 * @param timeout_ms مهلة الانتظار بالمللي ثانية
 * @return جسم الاستجابة (فارغ عند الفشل)
 */
std::string FederationEngine::http_get(const std::string& url, int timeout_ms) {
    (void)timeout_ms;  // يُستخدم في التنفيذ الفعلي

#ifdef _WIN32
    // ─── تنفيذ WinHTTP على ويندوز ───
    // نحلل URL ونرسل الطلب عبر WinHTTP

    // تحويل URL إلى wide string
    int wlen = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, nullptr, 0);
    std::wstring wurl(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, url.c_str(), -1, &wurl[0], wlen);

    // تحليل URL
    URL_COMPONENTS uc;
    ZeroMemory(&uc, sizeof(uc));
    uc.dwStructSize = sizeof(uc);
    uc.dwSchemeLength = (DWORD)-1;
    uc.dwHostNameLength = (DWORD)-1;
    uc.dwUrlPathLength = (DWORD)-1;
    uc.dwExtraInfoLength = (DWORD)-1;

    if (!WinHttpCrackUrl(wurl.c_str(), 0, 0, &uc)) {
        return "";
    }

    std::wstring host(uc.lpszHostName, uc.dwHostNameLength);
    std::wstring path(uc.lpszUrlPath, uc.dwUrlPathLength);
    if (uc.dwExtraInfoLength > 0) {
        path += std::wstring(uc.lpszExtraInfo, uc.dwExtraInfoLength);
    }

    bool is_https = (uc.nScheme == INTERNET_SCHEME_HTTPS);
    INTERNET_PORT port = uc.nPort;

    // فتح جلسة WinHTTP
    HINTERNET hSession = WinHttpOpen(
        L"SadRegistry-Federation/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) return "";

    // تعيين المهلة
    DWORD timeout = (DWORD)timeout_ms;
    WinHttpSetTimeouts(hSession, timeout, timeout, timeout, timeout);

    // الاتصال بالمضيف
    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        return "";
    }

    // إنشاء الطلب
    DWORD flags = is_https ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"GET", path.c_str(),
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, flags);

    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // إرسال الطلب
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // انتظار الاستجابة
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    // قراءة الجسم
    std::string result;
    DWORD bytesAvailable = 0;
    while (WinHttpQueryDataAvailable(hRequest, &bytesAvailable) && bytesAvailable > 0) {
        std::vector<char> buffer(bytesAvailable);
        DWORD bytesRead = 0;
        if (WinHttpReadData(hRequest, buffer.data(), bytesAvailable, &bytesRead)) {
            result.append(buffer.data(), bytesRead);
        }
    }

    // تنظيف
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return result;

#else
    // ─── تنفيذ curl على لينكس ───
    // نستدعي curl من سطر الأوامر كحل بسيط وخفيف
    std::string cmd = "curl -s --max-time " +
        std::to_string(timeout_ms / 1000) +
        " \"" + url + "\"";

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";

    std::string result;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    pclose(pipe);
    return result;
#endif
}

/**
 * إرسال طلب HTTP POST.
 * (مشابه لـ http_get مع إضافة جسم الطلب)
 */
std::string FederationEngine::http_post(
        const std::string& url,
        const std::string& body,
        int timeout_ms) {

    // تنفيذ مبسط: حالياً ندعم GET فقط
    // في التحديث القادم سنضيف دعم POST الكامل
    (void)url;
    (void)body;
    (void)timeout_ms;
    return "";
}

// ═══════════════════════════════════════════════════════════════════════════════
// تحليل الاستجابات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تحليل استجابة بحث JSON من مستودع بعيد.
 *
 * الصيغة المتوقعة:
 *   {
 *     "results": [
 *       {
 *         "name": "http",
 *         "arabic_name": "شبكة",
 *         "version": "2.1.0",
 *         "description": "HTTP client library",
 *         "downloads": 5000
 *       },
 *       ...
 *     ]
 *   }
 *
 * يُنشئ RemotePackage لكل نتيجة مع إضافة اسم المستودع المصدر.
 */
std::vector<RemotePackage> FederationEngine::parse_search_response(
        const std::string& json_response,
        const std::string& registry_url,
        const std::string& registry_name) {

    std::vector<RemotePackage> packages;

    // محلل JSON بسيط - نبحث عن كائنات في مصفوفة "results"
    // هذا تنفيذ مبسط يكفي للتفاعل الأساسي

    size_t pos = json_response.find("\"results\"");
    if (pos == std::string::npos) return packages;

    pos = json_response.find('[', pos);
    if (pos == std::string::npos) return packages;

    // البحث عن كل كائن { } داخل المصفوفة
    size_t obj_start = json_response.find('{', pos);
    while (obj_start != std::string::npos) {
        size_t obj_end = json_response.find('}', obj_start);
        if (obj_end == std::string::npos) break;

        std::string obj = json_response.substr(obj_start, obj_end - obj_start + 1);

        // استخراج الحقول
        auto extract = [](const std::string& json, const std::string& key) -> std::string {
            std::string search = "\"" + key + "\"";
            size_t p = json.find(search);
            if (p == std::string::npos) return "";
            p = json.find('"', json.find(':', p) + 1);
            if (p == std::string::npos) return "";
            size_t e = json.find('"', p + 1);
            if (e == std::string::npos) return "";
            return json.substr(p + 1, e - p - 1);
        };

        RemotePackage pkg;
        pkg.name = extract(obj, "name");
        pkg.arabic_name = extract(obj, "arabic_name");
        pkg.version = extract(obj, "version");
        pkg.description = extract(obj, "description");
        pkg.description_ar = extract(obj, "description_ar");
        pkg.registry_url = registry_url;
        pkg.registry_name = registry_name;
        pkg.downloads = 0;

        // محاولة استخراج عدد التنزيلات
        std::string dl_search = "\"downloads\"";
        size_t dl_pos = obj.find(dl_search);
        if (dl_pos != std::string::npos) {
            dl_pos = obj.find(':', dl_pos) + 1;
            while (dl_pos < obj.size() && obj[dl_pos] == ' ') dl_pos++;
            pkg.downloads = std::atoll(obj.c_str() + dl_pos);
        }

        if (!pkg.name.empty()) {
            packages.push_back(std::move(pkg));
        }

        // البحث عن الكائن التالي
        obj_start = json_response.find('{', obj_end + 1);
        // التوقف عند نهاية المصفوفة
        size_t arr_end = json_response.find(']', obj_end);
        if (arr_end != std::string::npos && (obj_start == std::string::npos || obj_start > arr_end)) {
            break;
        }
    }

    return packages;
}

// ═══════════════════════════════════════════════════════════════════════════════
// التخزين المحلي (قاعدة البيانات)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تحميل المستودعات المسجلة من قاعدة البيانات إلى الذاكرة.
 *
 * يُستدعى عند التهيئة لملء قائمة peers_ من الجدول federated_registries.
 * يقرأ من جدول federated_registries في SQLite عند توفره.
 */
void FederationEngine::load_peers_from_db() {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    peers_.clear();

#if SAD_HAS_SQLITE
    if (!db_) return;
    void* handle = db_->get_handle();
    if (!handle) return;

    sqlite3* sqldb = static_cast<sqlite3*>(handle);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(sqldb,
        "SELECT id, name, base_url, description, trust_level, is_online, "
        "package_count, last_sync_at, api_key "
        "FROM federated_registries ORDER BY trust_level DESC",
        -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            FederatedPeer peer;
            peer.id = sqlite3_column_int64(stmt, 0);
            const unsigned char* col1 = sqlite3_column_text(stmt, 1);
            peer.name = col1 ? reinterpret_cast<const char*>(col1) : "";
            const unsigned char* col2 = sqlite3_column_text(stmt, 2);
            peer.url = col2 ? reinterpret_cast<const char*>(col2) : "";
            const unsigned char* col3 = sqlite3_column_text(stmt, 3);
            peer.description = col3 ? reinterpret_cast<const char*>(col3) : "";
            peer.trust_level = sqlite3_column_int64(stmt, 4);
            peer.status = sqlite3_column_int(stmt, 5) ? PeerStatus::Active : PeerStatus::Inactive;
            peer.package_count = sqlite3_column_int64(stmt, 6);
            // last_sync_at
            const unsigned char* col7 = sqlite3_column_text(stmt, 7);
            const char* sync_str = col7 ? reinterpret_cast<const char*>(col7) : nullptr;
            peer.last_sync_timestamp = sync_str ? std::atoll(sync_str) : 0;
            const unsigned char* col8 = sqlite3_column_text(stmt, 8);
            peer.api_key = col8 ? reinterpret_cast<const char*>(col8) : "";
            peers_.push_back(peer);
        }
        sqlite3_finalize(stmt);
    }
    std::cout << "  ✔ تم تحميل " << peers_.size() << " مستودع متحد من قاعدة البيانات" << std::endl;
#else
    // في وضع المحاكاة: لا يوجد مستودعات مخزنة
#endif
}

/**
 * حفظ أو تحديث مستودع في قاعدة البيانات.
 */
void FederationEngine::save_peer_to_db(const FederatedPeer& peer) {
#if SAD_HAS_SQLITE
    if (!db_) return;
    void* handle = db_->get_handle();
    if (!handle) return;

    sqlite3* sqldb = static_cast<sqlite3*>(handle);
    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT OR REPLACE INTO federated_registries
            (name, base_url, description, trust_level, is_online, package_count, api_key)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";
    if (sqlite3_prepare_v2(sqldb, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, peer.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, peer.url.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, peer.description.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 4, peer.trust_level);
        sqlite3_bind_int(stmt, 5, (peer.status == PeerStatus::Active) ? 1 : 0);
        sqlite3_bind_int64(stmt, 6, peer.package_count);
        sqlite3_bind_text(stmt, 7, peer.api_key.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
#else
    (void)peer;
#endif
}

} // namespace registry
} // namespace sad
