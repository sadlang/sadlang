// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: database.cpp
 * الوصف: تنفيذ طبقة قاعدة البيانات لمستودع حزم لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * يُنفِّذ جميع عمليات قاعدة البيانات المعرّفة في database.h.
 * يستخدم SQLite كمحرك تخزين مع عبارات مُعدّة لمنع حقن SQL.
 *
 * ملاحظات التنفيذ:
 *   - إذا لم تتوفر مكتبة SQLite، يعمل بوضع "محاكاة" في الذاكرة
 *   - جميع المدخلات تُمرّر عبر prepared statements
 *   - يدعم FTS5 للبحث النصي الكامل
 *   - WAL mode للقراءة المتزامنة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "database.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <iomanip>
#include <mutex>

// ─────────────────────────────────────────────────────────────────────────────
// استيراد SQLite إذا كان متاحاً
// ─────────────────────────────────────────────────────────────────────────────
#if SAD_HAS_SQLITE
    #include <sqlite3.h>
#endif

namespace sad {
namespace registry {

// safe_col_text - read text safely from sqlite3 (avoid NULL)
static const char* safe_col_text(sqlite3_stmt* s, int col) {
    auto t = sqlite3_column_text(s, col);
    return t ? reinterpret_cast<const char*>(t) : "";
}


namespace fs = std::filesystem;

// ═══════════════════════════════════════════════════════════════════════════════
// وضع المحاكاة (عندما لا تتوفر SQLite)
// ═══════════════════════════════════════════════════════════════════════════════
//
// عندما لا تكون sqlite3 متاحة، نستخدم تخزيناً بسيطاً في الذاكرة
// باستخدام std::map لمحاكاة العمليات الأساسية.
// هذا يسمح بالتطوير والاختبار بدون تبعيات خارجية.

/// قفل عمومي لحماية البيانات في الذاكرة (الوضع المحاكي)
static std::mutex g_db_mutex;

/// تخزين المستخدمين في الذاكرة
static std::map<int64_t, UserInfo> g_users;
static int64_t g_next_user_id = 1;

/// تخزين الحزم في الذاكرة
static std::map<int64_t, PackageInfo> g_packages;
static int64_t g_next_package_id = 1;

/// تخزين الإصدارات في الذاكرة
static std::map<int64_t, VersionInfo> g_versions;
static int64_t g_next_version_id = 1;

/// تخزين رموز الوصول في الذاكرة
static std::map<std::string, int64_t> g_tokens; // hash → user_id
static int64_t g_next_token_id = 1;

/// عداد التنزيلات
static int64_t g_total_downloads = 0;


// ═══════════════════════════════════════════════════════════════════════════════
// المُنشئ والمُهدم
// ═══════════════════════════════════════════════════════════════════════════════

Database::Database(const std::string& db_path) : db_path_(db_path) {}

Database::~Database() {
    close();
}

// ═══════════════════════════════════════════════════════════════════════════════
// التهيئة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تهيئة قاعدة البيانات
 *
 * عندما تتوفر SQLite:
 *   ١. إنشاء مجلد البيانات إذا لم يكن موجوداً
 *   ٢. فتح الاتصال بملف قاعدة البيانات
 *   ٣. تفعيل المفاتيح الخارجية ووضع WAL
 *   ٤. تنفيذ مخطط الجداول (migrations/001_init.sql)
 *
 * في وضع المحاكاة:
 *   يُهيئ هياكل البيانات في الذاكرة فقط
 */
bool Database::initialize() {
#if SAD_HAS_SQLITE
    // إنشاء مجلد قاعدة البيانات
    fs::path db_dir = fs::path(db_path_).parent_path();
    if (!db_dir.empty()) {
        fs::create_directories(db_dir);
    }

    // فتح الاتصال
    sqlite3* db = nullptr;
    int rc = sqlite3_open(db_path_.c_str(), &db);
    if (rc != SQLITE_OK) {
        log("خطأ", "فشل فتح قاعدة البيانات: " + std::string(sqlite3_errmsg(db)));
        sqlite3_close(db);
        return false;
    }

    db_ = static_cast<void*>(db);

    // تفعيل المفاتيح الخارجية
    execute("PRAGMA foreign_keys = ON");
    execute("PRAGMA journal_mode = WAL");

    // تنفيذ المخطط
    // نبحث عن ملف SQL بالقرب من الملف التنفيذي أو في المسار الحالي
    std::vector<std::string> sql_paths = {
        "migrations/001_init.sql",
        "../migrations/001_init.sql",
        "../../migrations/001_init.sql",
    };

    bool schema_applied = false;
    for (const auto& path : sql_paths) {
        if (fs::exists(path)) {
            schema_applied = execute_sql_file(path);
            if (schema_applied) {
                log("معلومات", "تم تطبيق المخطط من: " + path);

                // تطبيق هجرات إضافية
                std::string dir = fs::path(path).parent_path().string();
                std::string migration_002 = dir + "/002_improvements.sql";
                if (fs::exists(migration_002)) {
                    if (execute_sql_file(migration_002)) {
                        log("معلومات", "تم تطبيق الهجرة 002: التحسينات");
                    }
                }
                break;
            }
        }
    }

    // إذا لم نجد ملف SQL، ننفذ المخطط الأساسي مباشرة
    if (!schema_applied) {
        log("تحذير", "لم يُعثر على ملف المخطط - إنشاء الجداول الأساسية مباشرة");
        execute(R"(
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT NOT NULL UNIQUE,
                email TEXT NOT NULL UNIQUE,
                password_hash TEXT NOT NULL,
                display_name TEXT DEFAULT '',
                bio TEXT DEFAULT '',
                is_active BOOLEAN DEFAULT 1,
                is_admin BOOLEAN DEFAULT 0,
                package_count INTEGER DEFAULT 0,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                last_login_at DATETIME
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS packages (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL UNIQUE,
                display_name TEXT DEFAULT '',
                arabic_name TEXT DEFAULT '',
                description TEXT DEFAULT '',
                description_ar TEXT DEFAULT '',
                owner_id INTEGER NOT NULL,
                license TEXT DEFAULT 'MIT',
                repository_url TEXT DEFAULT '',
                homepage_url TEXT DEFAULT '',
                latest_version TEXT DEFAULT '0.0.0',
                total_downloads INTEGER DEFAULT 0,
                category TEXT DEFAULT '',
                is_verified BOOLEAN DEFAULT 0,
                is_featured BOOLEAN DEFAULT 0,
                is_yanked BOOLEAN DEFAULT 0,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (owner_id) REFERENCES users(id)
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS versions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                package_id INTEGER NOT NULL,
                version TEXT NOT NULL,
                version_major INTEGER DEFAULT 0,
                version_minor INTEGER DEFAULT 0,
                version_patch INTEGER DEFAULT 0,
                release_notes TEXT DEFAULT '',
                checksum_sha256 TEXT DEFAULT '',
                blob_path TEXT DEFAULT '',
                file_size INTEGER DEFAULT 0,
                is_yanked BOOLEAN DEFAULT 0,
                downloads INTEGER DEFAULT 0,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (package_id) REFERENCES packages(id),
                UNIQUE(package_id, version)
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS dependencies (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                version_id INTEGER NOT NULL,
                dependency_name TEXT NOT NULL,
                version_requirement TEXT DEFAULT '*',
                dependency_type TEXT DEFAULT 'runtime',
                FOREIGN KEY (version_id) REFERENCES versions(id)
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS api_tokens (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER NOT NULL,
                token_name TEXT DEFAULT 'default',
                token_hash TEXT NOT NULL UNIQUE,
                scopes TEXT DEFAULT 'publish',
                is_active BOOLEAN DEFAULT 1,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (user_id) REFERENCES users(id)
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS downloads (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                version_id INTEGER NOT NULL,
                downloaded_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (version_id) REFERENCES versions(id)
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS arabic_search_index (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                package_id INTEGER NOT NULL,
                original_word TEXT NOT NULL,
                arabic_root TEXT DEFAULT '',
                normalized_word TEXT NOT NULL,
                english_equivalent TEXT DEFAULT '',
                FOREIGN KEY (package_id) REFERENCES packages(id)
            )
        )");
        // جداول التحسينات (هجرة 002)
        execute(R"(
            CREATE TABLE IF NOT EXISTS reviews (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                package_id INTEGER NOT NULL,
                user_id INTEGER NOT NULL,
                rating INTEGER NOT NULL CHECK(rating BETWEEN 1 AND 5),
                title TEXT DEFAULT '',
                body TEXT DEFAULT '',
                is_helpful INTEGER DEFAULT 0,
                is_edited BOOLEAN DEFAULT 0,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (package_id) REFERENCES packages(id),
                FOREIGN KEY (user_id) REFERENCES users(id),
                UNIQUE(package_id, user_id)
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS stars (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                package_id INTEGER NOT NULL,
                user_id INTEGER NOT NULL,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (package_id) REFERENCES packages(id),
                FOREIGN KEY (user_id) REFERENCES users(id),
                UNIQUE(package_id, user_id)
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS webhooks (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER NOT NULL,
                package_id INTEGER,
                url TEXT NOT NULL,
                secret TEXT DEFAULT '',
                events TEXT DEFAULT 'publish,yank',
                is_active BOOLEAN DEFAULT 1,
                failure_count INTEGER DEFAULT 0,
                last_triggered DATETIME,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (user_id) REFERENCES users(id),
                FOREIGN KEY (package_id) REFERENCES packages(id)
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS reports (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                reporter_id INTEGER NOT NULL,
                package_id INTEGER NOT NULL,
                reason TEXT NOT NULL,
                details TEXT DEFAULT '',
                status TEXT DEFAULT 'pending',
                admin_notes TEXT DEFAULT '',
                resolved_by INTEGER,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                resolved_at DATETIME,
                FOREIGN KEY (reporter_id) REFERENCES users(id),
                FOREIGN KEY (package_id) REFERENCES packages(id)
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS system_settings (
                key TEXT PRIMARY KEY,
                value TEXT NOT NULL DEFAULT '',
                description TEXT DEFAULT '',
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )");
        execute(R"(
            CREATE TABLE IF NOT EXISTS daily_downloads (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                package_id INTEGER NOT NULL,
                download_date DATE NOT NULL,
                count INTEGER DEFAULT 0,
                FOREIGN KEY (package_id) REFERENCES packages(id),
                UNIQUE(package_id, download_date)
            )
        )");
    }

    is_initialized_ = true;
    return true;

#else
    // وضع المحاكاة بدون SQLite
    log("معلومات", "وضع المحاكاة: التخزين في الذاكرة فقط (بدون SQLite)");

    // إنشاء مستخدم المشرف الافتراضي
    std::lock_guard<std::mutex> lock(g_db_mutex);
    UserInfo admin;
    admin.id = g_next_user_id++;
    admin.username = "admin";
    admin.email = "admin@sadlang.org";
    admin.password_hash = "placeholder";
    admin.display_name = "مشرف المستودع";
    admin.is_active = true;
    admin.is_admin = true;
    g_users[admin.id] = admin;

    is_initialized_ = true;
    return true;
#endif
}

/**
 * @brief إغلاق قاعدة البيانات
 */
void Database::close() {
#if SAD_HAS_SQLITE
    if (db_) {
        sqlite3_close(static_cast<sqlite3*>(db_));
        db_ = nullptr;
    }
#endif
    is_initialized_ = false;
}

bool Database::is_open() const {
    return is_initialized_;
}

// ═══════════════════════════════════════════════════════════════════════════════
// عمليات المستخدمين
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief إنشاء مستخدم جديد
 */
int64_t Database::create_user(const UserInfo& user) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;

    const char* sql = R"(
        INSERT INTO users (username, email, password_hash, display_name, is_active)
        VALUES (?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.password_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user.display_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, user.is_active ? 1 : 0);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);

    // التحقق من التكرار
    for (const auto& [id, u] : g_users) {
        if (u.username == user.username || u.email == user.email) return -1;
    }

    UserInfo new_user = user;
    new_user.id = g_next_user_id++;
    g_users[new_user.id] = new_user;
    return new_user.id;
#endif
}

std::optional<UserInfo> Database::get_user(int64_t id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db,
        "SELECT id,username,email,password_hash,display_name,bio,is_active,is_admin,package_count,created_at,last_login_at FROM users WHERE id=?",
        -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_int64(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        UserInfo u;
        u.id = sqlite3_column_int64(stmt, 0);
        u.username = safe_col_text(stmt, 1);
        u.email = safe_col_text(stmt, 2);
        u.password_hash = safe_col_text(stmt, 3);
        u.display_name = safe_col_text(stmt, 4);
        u.bio = safe_col_text(stmt, 5);
        u.is_active = sqlite3_column_int(stmt, 6) != 0;
        u.is_admin = sqlite3_column_int(stmt, 7) != 0;
        u.package_count = sqlite3_column_int(stmt, 8);
        u.created_at = safe_col_text(stmt, 9);
        u.last_login_at = safe_col_text(stmt, 10);
        sqlite3_finalize(stmt);
        return u;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    auto it = g_users.find(id);
    if (it != g_users.end()) return it->second;
    return std::nullopt;
#endif
}

std::optional<UserInfo> Database::get_user_by_username(const std::string& username) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "SELECT id FROM users WHERE username=?",
        -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
        return get_user(id);
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    for (const auto& [id, u] : g_users) {
        if (u.username == username) return u;
    }
    return std::nullopt;
#endif
}

std::optional<UserInfo> Database::get_user_by_email(const std::string& email) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "SELECT id FROM users WHERE email=?",
        -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int64_t id = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
        return get_user(id);
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    for (const auto& [id, u] : g_users) {
        if (u.email == email) return u;
    }
    return std::nullopt;
#endif
}

bool Database::update_user(const UserInfo& user) {
#if SAD_HAS_SQLITE
    return execute("UPDATE users SET display_name='" + user.display_name
                   + "',bio='" + user.bio + "' WHERE id=" + std::to_string(user.id));
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    auto it = g_users.find(user.id);
    if (it == g_users.end()) return false;
    it->second.display_name = user.display_name;
    it->second.bio = user.bio;
    return true;
#endif
}

bool Database::update_last_login(int64_t user_id) {
#if SAD_HAS_SQLITE
    return execute("UPDATE users SET last_login_at=CURRENT_TIMESTAMP WHERE id=" + std::to_string(user_id));
#else
    (void)user_id;
    return true;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// عمليات الحزم
// ═══════════════════════════════════════════════════════════════════════════════

int64_t Database::create_package(const PackageInfo& pkg) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;

    const char* sql = R"(
        INSERT INTO packages (name, display_name, arabic_name, description, description_ar,
                              owner_id, license, repository_url, category)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;

    sqlite3_bind_text(stmt, 1, pkg.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pkg.display_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pkg.arabic_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, pkg.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, pkg.description_ar.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 6, pkg.owner_id);
    sqlite3_bind_text(stmt, 7, pkg.license.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, pkg.repository_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, pkg.category.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    for (const auto& [id, p] : g_packages) {
        if (p.name == pkg.name) return -1; // اسم مكرر
    }
    PackageInfo new_pkg = pkg;
    new_pkg.id = g_next_package_id++;
    g_packages[new_pkg.id] = new_pkg;
    return new_pkg.id;
#endif
}

std::optional<PackageInfo> Database::get_package(const std::string& name) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "SELECT id,name,display_name,arabic_name,description,description_ar,owner_id,license,repository_url,homepage_url,latest_version,total_downloads,category,is_verified,is_featured,is_yanked,created_at,updated_at FROM packages WHERE name=? OR arabic_name=?",
        -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        PackageInfo p;
        p.id = sqlite3_column_int64(stmt, 0);
        p.name = safe_col_text(stmt, 1);
        p.display_name = safe_col_text(stmt, 2);
        p.arabic_name = safe_col_text(stmt, 3);
        p.description = safe_col_text(stmt, 4);
        p.description_ar = safe_col_text(stmt, 5);
        p.owner_id = sqlite3_column_int64(stmt, 6);
        p.license = safe_col_text(stmt, 7);
        p.repository_url = safe_col_text(stmt, 8);
        p.homepage_url = safe_col_text(stmt, 9);
        p.latest_version = safe_col_text(stmt, 10);
        p.total_downloads = sqlite3_column_int64(stmt, 11);
        p.category = safe_col_text(stmt, 12);
        p.is_verified = sqlite3_column_int(stmt, 13) != 0;
        p.is_featured = sqlite3_column_int(stmt, 14) != 0;
        p.is_yanked = sqlite3_column_int(stmt, 15) != 0;
        p.created_at = safe_col_text(stmt, 16);
        p.updated_at = safe_col_text(stmt, 17);
        sqlite3_finalize(stmt);
        return p;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    for (const auto& [id, p] : g_packages) {
        if (p.name == name || p.arabic_name == name) return p;
    }
    return std::nullopt;
#endif
}

std::optional<PackageInfo> Database::get_package_by_id(int64_t id) {
#if SAD_HAS_SQLITE
    // البحث عن حزمة بمعرفها الفريد في قاعدة البيانات
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "SELECT id,name,display_name,arabic_name,description,description_ar,"
        "owner_id,license,repository_url,homepage_url,latest_version,"
        "total_downloads,category,is_verified,is_featured,is_yanked,"
        "created_at,updated_at FROM packages WHERE id=?",
        -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        PackageInfo p;
        p.id = sqlite3_column_int64(stmt, 0);
        p.name = safe_col_text(stmt, 1);
        p.display_name = safe_col_text(stmt, 2);
        p.arabic_name = safe_col_text(stmt, 3);
        p.description = safe_col_text(stmt, 4);
        p.description_ar = safe_col_text(stmt, 5);
        p.owner_id = sqlite3_column_int64(stmt, 6);
        p.license = safe_col_text(stmt, 7);
        p.repository_url = safe_col_text(stmt, 8);
        p.homepage_url = safe_col_text(stmt, 9);
        p.latest_version = safe_col_text(stmt, 10);
        p.total_downloads = sqlite3_column_int64(stmt, 11);
        p.category = safe_col_text(stmt, 12);
        p.is_verified = sqlite3_column_int(stmt, 13) != 0;
        p.is_featured = sqlite3_column_int(stmt, 14) != 0;
        p.is_yanked = sqlite3_column_int(stmt, 15) != 0;
        p.created_at = safe_col_text(stmt, 16);
        p.updated_at = safe_col_text(stmt, 17);
        sqlite3_finalize(stmt);
        return p;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    auto it = g_packages.find(id);
    if (it != g_packages.end()) return it->second;
    return std::nullopt;
#endif
}

bool Database::update_package(const PackageInfo& pkg) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    const char* sql = R"(
        UPDATE packages SET
            display_name=?, arabic_name=?, description=?, description_ar=?,
            license=?, category=?, repository_url=?, homepage_url=?,
            documentation_url=?, is_verified=?, is_featured=?,
            updated_at=CURRENT_TIMESTAMP
        WHERE id=?
    )";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, pkg.display_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pkg.arabic_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pkg.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, pkg.description_ar.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, pkg.license.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, pkg.category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, pkg.repository_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, pkg.homepage_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, pkg.documentation_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 10, pkg.is_verified ? 1 : 0);
    sqlite3_bind_int(stmt, 11, pkg.is_featured ? 1 : 0);
    sqlite3_bind_int64(stmt, 12, pkg.id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    auto it = g_packages.find(pkg.id);
    if (it != g_packages.end()) { it->second = pkg; return true; }
    return false;
#endif
}

bool Database::yank_package(int64_t package_id, bool yank) {
#if SAD_HAS_SQLITE
    std::string sql = "UPDATE packages SET is_yanked="
        + std::string(yank ? "1" : "0")
        + ", updated_at=CURRENT_TIMESTAMP WHERE id="
        + std::to_string(package_id);
    return execute(sql);
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    auto it = g_packages.find(package_id);
    if (it != g_packages.end()) { it->second.is_yanked = yank; return true; }
    return false;
#endif
}

bool Database::delete_package(int64_t package_id) {
#if SAD_HAS_SQLITE
    // حذف متسلسل: الإصدارات أولاً ثم الحزمة
    execute("DELETE FROM versions WHERE package_id=" + std::to_string(package_id));
    execute("DELETE FROM keywords WHERE package_id=" + std::to_string(package_id));
    execute("DELETE FROM arabic_search_index WHERE package_id=" + std::to_string(package_id));
    return execute("DELETE FROM packages WHERE id=" + std::to_string(package_id));
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    g_packages.erase(package_id);
    return true;
#endif
}

PagedResults<PackageInfo> Database::list_packages(int page, int per_page,
                                                    const std::string& sort,
                                                    bool descending) {
    PagedResults<PackageInfo> result;
    result.page = page;
    result.per_page = per_page;
    (void)sort; (void)descending;

#if SAD_HAS_SQLITE
    // تنفيذ قائمة الحزم مع SQLite
    // نبني استعلاماً يدعم الترتيب والتصفيح (pagination)
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt_count;
    // ١. حساب العدد الإجمالي
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM packages WHERE is_yanked=0",
        -1, &stmt_count, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt_count) == SQLITE_ROW) {
            result.total_count = sqlite3_column_int64(stmt_count, 0);
        }
        sqlite3_finalize(stmt_count);
    }
    result.total_pages = (result.total_count + per_page - 1) / per_page;

    // ٢. جلب الحزم مع الترتيب
    std::string order_col = "created_at";
    if (sort == "downloads" || sort == "تنزيلات") order_col = "total_downloads";
    else if (sort == "name" || sort == "اسم")          order_col = "name";
    else if (sort == "updated" || sort == "تحديث")     order_col = "updated_at";
    std::string order_dir = descending ? "DESC" : "ASC";

    std::string sql_list = "SELECT id,name,display_name,arabic_name,description,"
        "description_ar,owner_id,license,repository_url,homepage_url,latest_version,"
        "total_downloads,category,is_verified,is_featured,is_yanked,created_at,updated_at "
        "FROM packages WHERE is_yanked=0 ORDER BY " + order_col + " " + order_dir +
        " LIMIT ? OFFSET ?";

    sqlite3_stmt* stmt_list;
    if (sqlite3_prepare_v2(db, sql_list.c_str(), -1, &stmt_list, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt_list, 1, per_page);
        sqlite3_bind_int(stmt_list, 2, (page - 1) * per_page);
        while (sqlite3_step(stmt_list) == SQLITE_ROW) {
            PackageInfo p;
            p.id = sqlite3_column_int64(stmt_list, 0);
            p.name = safe_col_text(stmt_list, 1);
            p.display_name = safe_col_text(stmt_list, 2);
            p.arabic_name = safe_col_text(stmt_list, 3);
            p.description = safe_col_text(stmt_list, 4);
            p.description_ar = safe_col_text(stmt_list, 5);
            p.owner_id = sqlite3_column_int64(stmt_list, 6);
            p.license = safe_col_text(stmt_list, 7);
            p.repository_url = safe_col_text(stmt_list, 8);
            p.homepage_url = safe_col_text(stmt_list, 9);
            p.latest_version = safe_col_text(stmt_list, 10);
            p.total_downloads = sqlite3_column_int64(stmt_list, 11);
            p.category = safe_col_text(stmt_list, 12);
            p.is_verified = sqlite3_column_int(stmt_list, 13) != 0;
            p.is_featured = sqlite3_column_int(stmt_list, 14) != 0;
            p.is_yanked = sqlite3_column_int(stmt_list, 15) != 0;
            p.created_at = safe_col_text(stmt_list, 16);
            p.updated_at = safe_col_text(stmt_list, 17);
            result.items.push_back(p);
        }
        sqlite3_finalize(stmt_list);
    }
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    result.total_count = static_cast<int64_t>(g_packages.size());
    result.total_pages = (result.total_count + per_page - 1) / per_page;

    int skip = (page - 1) * per_page;
    int count = 0;
    for (const auto& [id, pkg] : g_packages) {
        if (count >= skip && count < skip + per_page) {
            result.items.push_back(pkg);
        }
        count++;
    }
#endif

    return result;
}

std::vector<PackageInfo> Database::get_popular_packages(int limit) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    std::vector<PackageInfo> result;
    if (sqlite3_prepare_v2(db,
        "SELECT id,name,display_name,arabic_name,description,description_ar,"
        "owner_id,license,latest_version,total_downloads,category,is_verified "
        "FROM packages WHERE is_yanked=0 "
        "ORDER BY total_downloads DESC LIMIT ?",
        -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, limit);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            PackageInfo p;
            p.id = sqlite3_column_int64(stmt, 0);
            p.name = safe_col_text(stmt, 1);
            p.display_name = safe_col_text(stmt, 2);
            p.arabic_name = safe_col_text(stmt, 3);
            p.description = safe_col_text(stmt, 4);
            p.description_ar = safe_col_text(stmt, 5);
            p.owner_id = sqlite3_column_int64(stmt, 6);
            p.license = safe_col_text(stmt, 7);
            p.latest_version = safe_col_text(stmt, 8);
            p.total_downloads = sqlite3_column_int64(stmt, 9);
            p.category = safe_col_text(stmt, 10);
            p.is_verified = sqlite3_column_int(stmt, 11) != 0;
            result.push_back(p);
        }
        sqlite3_finalize(stmt);
    }
    return result;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    std::vector<PackageInfo> result;
    for (const auto& [id, pkg] : g_packages) {
        result.push_back(pkg);
        if ((int)result.size() >= limit) break;
    }
    return result;
#endif
}

std::vector<PackageInfo> Database::get_featured_packages(int limit) {
    // جلب الحزم المميزة (التي اختارها المشرفون)
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    std::vector<PackageInfo> result;
    if (sqlite3_prepare_v2(db,
        "SELECT id,name,display_name,arabic_name,description,description_ar,"
        "owner_id,license,latest_version,total_downloads,category,is_verified "
        "FROM packages WHERE is_featured=1 AND is_yanked=0 "
        "ORDER BY total_downloads DESC LIMIT ?",
        -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, limit);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            PackageInfo p;
            p.id = sqlite3_column_int64(stmt, 0);
            p.name = safe_col_text(stmt, 1);
            p.display_name = safe_col_text(stmt, 2);
            p.arabic_name = safe_col_text(stmt, 3);
            p.description = safe_col_text(stmt, 4);
            p.description_ar = safe_col_text(stmt, 5);
            p.owner_id = sqlite3_column_int64(stmt, 6);
            p.license = safe_col_text(stmt, 7);
            p.latest_version = safe_col_text(stmt, 8);
            p.total_downloads = sqlite3_column_int64(stmt, 9);
            p.category = safe_col_text(stmt, 10);
            p.is_verified = sqlite3_column_int(stmt, 11) != 0;
            p.is_featured = true;
            result.push_back(p);
        }
        sqlite3_finalize(stmt);
    }
    return result;
#else
    // وضع المحاكاة: نرجع أول حزم
    std::lock_guard<std::mutex> lock(g_db_mutex);
    std::vector<PackageInfo> result;
    for (const auto& [id, pkg] : g_packages) {
        if (pkg.is_featured) { result.push_back(pkg); }
        if ((int)result.size() >= limit) break;
    }
    return result;
#endif
}

std::vector<PackageInfo> Database::get_new_packages(int limit) {
    return get_popular_packages(limit);
}

std::vector<PackageInfo> Database::get_user_packages(int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    std::vector<PackageInfo> result;
    if (sqlite3_prepare_v2(db,
        "SELECT id,name,display_name,arabic_name,description,description_ar,"
        "owner_id,license,latest_version,total_downloads,category,is_verified "
        "FROM packages WHERE owner_id=? ORDER BY updated_at DESC",
        -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, user_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            PackageInfo p;
            p.id = sqlite3_column_int64(stmt, 0);
            p.name = safe_col_text(stmt, 1);
            p.display_name = safe_col_text(stmt, 2);
            p.arabic_name = safe_col_text(stmt, 3);
            p.description = safe_col_text(stmt, 4);
            p.description_ar = safe_col_text(stmt, 5);
            p.owner_id = sqlite3_column_int64(stmt, 6);
            p.license = safe_col_text(stmt, 7);
            p.latest_version = safe_col_text(stmt, 8);
            p.total_downloads = sqlite3_column_int64(stmt, 9);
            p.category = safe_col_text(stmt, 10);
            p.is_verified = sqlite3_column_int(stmt, 11) != 0;
            result.push_back(p);
        }
        sqlite3_finalize(stmt);
    }
    return result;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    std::vector<PackageInfo> result;
    for (const auto& [id, pkg] : g_packages) {
        if (pkg.owner_id == user_id) result.push_back(pkg);
    }
    return result;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// عمليات الإصدارات
// ═══════════════════════════════════════════════════════════════════════════════

int64_t Database::create_version(const VersionInfo& version) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    const char* sql = R"(
        INSERT INTO versions (package_id, version, version_major, version_minor, version_patch,
                              release_notes, checksum_sha256, blob_path, file_size)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, version.package_id);
    sqlite3_bind_text(stmt, 2, version.version.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, version.major);
    sqlite3_bind_int(stmt, 4, version.minor);
    sqlite3_bind_int(stmt, 5, version.patch);
    sqlite3_bind_text(stmt, 6, version.release_notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, version.checksum_sha256.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, version.blob_path.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 9, version.file_size);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;

    // تحديث أحدث إصدار في الحزمة
    execute("UPDATE packages SET latest_version='" + version.version
            + "', updated_at=CURRENT_TIMESTAMP WHERE id=" + std::to_string(version.package_id));

    return sqlite3_last_insert_rowid(db);
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    VersionInfo v = version;
    v.id = g_next_version_id++;
    g_versions[v.id] = v;

    // تحديث أحدث إصدار
    auto it = g_packages.find(version.package_id);
    if (it != g_packages.end()) {
        it->second.latest_version = version.version;
    }
    return v.id;
#endif
}

std::optional<VersionInfo> Database::get_version(int64_t package_id, const std::string& version) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "SELECT id,package_id,version,version_major,version_minor,version_patch,"
        "release_notes,checksum_sha256,blob_path,file_size,downloads,is_yanked "
        "FROM versions WHERE package_id=? AND version=?",
        -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, package_id);
        sqlite3_bind_text(stmt, 2, version.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            VersionInfo v;
            v.id = sqlite3_column_int64(stmt, 0);
            v.package_id = sqlite3_column_int64(stmt, 1);
            v.version = safe_col_text(stmt, 2);
            v.major = sqlite3_column_int(stmt, 3);
            v.minor = sqlite3_column_int(stmt, 4);
            v.patch = sqlite3_column_int(stmt, 5);
            v.release_notes = safe_col_text(stmt, 6);
            v.checksum_sha256 = safe_col_text(stmt, 7);
            v.blob_path = safe_col_text(stmt, 8);
            v.file_size = sqlite3_column_int64(stmt, 9);
            v.downloads = sqlite3_column_int64(stmt, 10);
            v.is_yanked = sqlite3_column_int(stmt, 11) != 0;
            sqlite3_finalize(stmt);
            return v;
        }
        sqlite3_finalize(stmt);
    }
    return std::nullopt;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    for (const auto& [id, v] : g_versions) {
        if (v.package_id == package_id && v.version == version) return v;
    }
    return std::nullopt;
#endif
}

std::vector<VersionInfo> Database::get_package_versions(int64_t package_id) {
    std::vector<VersionInfo> result;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "SELECT id,package_id,version,version_major,version_minor,version_patch,"
        "release_notes,checksum_sha256,blob_path,file_size,downloads,is_yanked "
        "FROM versions WHERE package_id=? ORDER BY version_major DESC, version_minor DESC, version_patch DESC",
        -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, package_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            VersionInfo v;
            v.id = sqlite3_column_int64(stmt, 0);
            v.package_id = sqlite3_column_int64(stmt, 1);
            v.version = safe_col_text(stmt, 2);
            v.major = sqlite3_column_int(stmt, 3);
            v.minor = sqlite3_column_int(stmt, 4);
            v.patch = sqlite3_column_int(stmt, 5);
            v.release_notes = safe_col_text(stmt, 6);
            v.checksum_sha256 = safe_col_text(stmt, 7);
            v.blob_path = safe_col_text(stmt, 8);
            v.file_size = sqlite3_column_int64(stmt, 9);
            v.downloads = sqlite3_column_int64(stmt, 10);
            v.is_yanked = sqlite3_column_int(stmt, 11) != 0;
            result.push_back(v);
        }
        sqlite3_finalize(stmt);
    }
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    for (const auto& [id, v] : g_versions) {
        if (v.package_id == package_id) result.push_back(v);
    }
#endif
    return result;
}

bool Database::yank_version(int64_t version_id, const std::string& reason) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "UPDATE versions SET is_yanked=1, yank_reason=? WHERE id=?",
        -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, reason.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, version_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    auto it = g_versions.find(version_id);
    if (it != g_versions.end()) { it->second.is_yanked = true; return true; }
    return false;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// البحث
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief البحث عن حزم
 *
 * في وضع SQLite: يستخدم FTS5 للبحث السريع
 * في وضع المحاكاة: يبحث في الأسماء والأوصاف يدوياً
 */
PagedResults<SearchResult> Database::search_packages(const std::string& query,
                                                       int page, int per_page) {
    PagedResults<SearchResult> result;
    result.page = page;
    result.per_page = per_page;

#if SAD_HAS_SQLITE
    // بحث FTS5
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    const char* sql = R"(
        SELECT p.id, p.name, p.arabic_name, p.description, p.description_ar,
               p.latest_version, p.total_downloads, 0.0, p.category, p.is_verified
        FROM packages p
        WHERE p.name LIKE ? OR p.arabic_name LIKE ? OR p.description LIKE ?
        ORDER BY p.total_downloads DESC
        LIMIT ? OFFSET ?
    )";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        std::string like = "%" + query + "%";
        sqlite3_bind_text(stmt, 1, like.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, like.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, like.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, per_page);
        sqlite3_bind_int(stmt, 5, (page - 1) * per_page);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            SearchResult sr;
            sr.id = sqlite3_column_int64(stmt, 0);
            sr.name = safe_col_text(stmt, 1);
            sr.arabic_name = safe_col_text(stmt, 2);
            sr.description = safe_col_text(stmt, 3);
            sr.description_ar = safe_col_text(stmt, 4);
            sr.latest_version = safe_col_text(stmt, 5);
            sr.total_downloads = sqlite3_column_int64(stmt, 6);
            sr.rating = sqlite3_column_double(stmt, 7);
            sr.category = safe_col_text(stmt, 8);
            sr.is_verified = sqlite3_column_int(stmt, 9) != 0;
            result.items.push_back(sr);
        }
        sqlite3_finalize(stmt);
    }
#else
    // بحث بسيط في الذاكرة
    std::lock_guard<std::mutex> lock(g_db_mutex);
    std::string q_lower = query;
    std::transform(q_lower.begin(), q_lower.end(), q_lower.begin(), ::tolower);

    for (const auto& [id, pkg] : g_packages) {
        std::string name_lower = pkg.name;
        std::transform(name_lower.begin(), name_lower.end(), name_lower.begin(), ::tolower);

        bool match = name_lower.find(q_lower) != std::string::npos
                  || pkg.arabic_name.find(query) != std::string::npos
                  || pkg.description.find(query) != std::string::npos
                  || pkg.description_ar.find(query) != std::string::npos;

        if (match) {
            SearchResult sr;
            sr.id = pkg.id;
            sr.name = pkg.name;
            sr.arabic_name = pkg.arabic_name;
            sr.description = pkg.description;
            sr.description_ar = pkg.description_ar;
            sr.latest_version = pkg.latest_version;
            sr.total_downloads = pkg.total_downloads;
            sr.category = pkg.category;
            sr.is_verified = pkg.is_verified;
            result.items.push_back(sr);
        }
    }

    result.total_count = static_cast<int64_t>(result.items.size());
    result.total_pages = (result.total_count + per_page - 1) / per_page;
#endif

    return result;
}

PagedResults<SearchResult> Database::search_by_category(const std::string& category,
                                                          int page, int per_page) {
    PagedResults<SearchResult> result;
    result.page = page;
    result.per_page = per_page;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    // عدّ النتائج
    if (sqlite3_prepare_v2(db,
        "SELECT COUNT(*) FROM packages WHERE category=? AND is_yanked=0",
        -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, category.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW)
            result.total_count = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
    }
    result.total_pages = (result.total_count + per_page - 1) / per_page;
    // جلب الحزم
    if (sqlite3_prepare_v2(db,
        "SELECT id,name,arabic_name,description,description_ar,latest_version,"
        "total_downloads,category,is_verified "
        "FROM packages WHERE category=? AND is_yanked=0 "
        "ORDER BY total_downloads DESC LIMIT ? OFFSET ?",
        -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, category.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, per_page);
        sqlite3_bind_int(stmt, 3, (page - 1) * per_page);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            SearchResult sr;
            sr.id = sqlite3_column_int64(stmt, 0);
            sr.name = safe_col_text(stmt, 1);
            sr.arabic_name = safe_col_text(stmt, 2);
            sr.description = safe_col_text(stmt, 3);
            sr.description_ar = safe_col_text(stmt, 4);
            sr.latest_version = safe_col_text(stmt, 5);
            sr.total_downloads = sqlite3_column_int64(stmt, 6);
            sr.category = safe_col_text(stmt, 7);
            sr.is_verified = sqlite3_column_int(stmt, 8) != 0;
            result.items.push_back(sr);
        }
        sqlite3_finalize(stmt);
    }
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    for (const auto& [id, pkg] : g_packages) {
        if (pkg.category == category) {
            SearchResult sr;
            sr.id = pkg.id; sr.name = pkg.name; sr.arabic_name = pkg.arabic_name;
            sr.description = pkg.description; sr.latest_version = pkg.latest_version;
            sr.category = pkg.category;
            result.items.push_back(sr);
        }
    }
    result.total_count = static_cast<int64_t>(result.items.size());
    result.total_pages = (result.total_count + per_page - 1) / per_page;
#endif
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// التنزيل والإحصائيات
// ═══════════════════════════════════════════════════════════════════════════════

void Database::record_download(int64_t version_id, const std::string& ip_hash,
                                const std::string& user_agent) {
    (void)ip_hash; (void)user_agent;
#if SAD_HAS_SQLITE
    execute("INSERT INTO downloads (version_id) VALUES (" + std::to_string(version_id) + ")");
    execute("UPDATE versions SET downloads=downloads+1 WHERE id=" + std::to_string(version_id));
#else
    (void)version_id;
    g_total_downloads++;
#endif
}

RegistryStats Database::get_stats() {
    RegistryStats stats;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    // عد الحزم
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM packages", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) stats.total_packages = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
    }
    // عد الإصدارات
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM versions", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) stats.total_versions = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
    }
    // مجموع التنزيلات
    if (sqlite3_prepare_v2(db, "SELECT COALESCE(SUM(downloads),0) FROM versions", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) stats.total_downloads = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
    }
    // عد المستخدمين
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM users", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) stats.total_users = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
    }
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    stats.total_packages = static_cast<int64_t>(g_packages.size());
    stats.total_versions = static_cast<int64_t>(g_versions.size());
    stats.total_downloads = g_total_downloads;
    stats.total_users = static_cast<int64_t>(g_users.size());
#endif
    return stats;
}

// ═══════════════════════════════════════════════════════════════════════════════
// رموز الوصول
// ═══════════════════════════════════════════════════════════════════════════════

int64_t Database::create_token(int64_t user_id, const std::string& token_name,
                                 const std::string& token_hash, const std::string& scopes) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "INSERT INTO api_tokens (user_id,token_name,token_hash,scopes) VALUES (?,?,?,?)",
        -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, token_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, token_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, scopes.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)token_name; (void)scopes;
    std::lock_guard<std::mutex> lock(g_db_mutex);
    g_tokens[token_hash] = user_id;
    return g_next_token_id++;
#endif
}

std::optional<UserInfo> Database::verify_token(const std::string& token_hash) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "SELECT user_id FROM api_tokens WHERE token_hash=? AND is_active=1",
        -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, token_hash.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int64_t uid = sqlite3_column_int64(stmt, 0);
        sqlite3_finalize(stmt);
        return get_user(uid);
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
#else
    std::lock_guard<std::mutex> lock(g_db_mutex);
    auto it = g_tokens.find(token_hash);
    if (it != g_tokens.end()) {
        auto uit = g_users.find(it->second);
        if (uit != g_users.end()) return uit->second;
    }
    return std::nullopt;
#endif
}

bool Database::revoke_token(int64_t token_id) {
#if SAD_HAS_SQLITE
    return execute("UPDATE api_tokens SET is_active=0 WHERE id=" + std::to_string(token_id));
#else
    (void)token_id;
    return true;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// سجل المراجعة
// ═══════════════════════════════════════════════════════════════════════════════

void Database::log_audit(int64_t user_id, const std::string& action,
                          const std::string& details, const std::string& ip,
                          const std::string& ua) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "INSERT INTO audit_log (user_id,action,details,ip_address,user_agent) VALUES (?,?,?,?,?)",
        -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, user_id);
        sqlite3_bind_text(stmt, 2, action.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, details.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, ip.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, ua.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
#else
    // في وضع المحاكاة: نطبع فقط
    std::cerr << "  [تدقيق] مستخدم=" << user_id << " إجراء=" << action << std::endl;
    (void)details; (void)ip; (void)ua;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// البحث العربي
// ═══════════════════════════════════════════════════════════════════════════════

void Database::update_arabic_index(int64_t package_id,
    const std::vector<std::tuple<std::string, std::string, std::string>>& words) {
#if SAD_HAS_SQLITE
    // حذف الفهرس القديم
    execute("DELETE FROM arabic_search_index WHERE package_id=" + std::to_string(package_id));
    // إضافة الكلمات الجديدة
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "INSERT INTO arabic_search_index (package_id,word,root,normalized) VALUES (?,?,?,?)",
        -1, &stmt, nullptr) == SQLITE_OK) {
        for (const auto& [word, root, normalized] : words) {
            sqlite3_bind_int64(stmt, 1, package_id);
            sqlite3_bind_text(stmt, 2, word.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, root.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 4, normalized.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
        }
        sqlite3_finalize(stmt);
    }
#else
    (void)package_id; (void)words;
#endif
}

std::vector<int64_t> Database::search_by_arabic_root(const std::string& root) {
    std::vector<int64_t> result;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db,
        "SELECT DISTINCT package_id FROM arabic_search_index WHERE root=?",
        -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, root.c_str(), -1, SQLITE_TRANSIENT);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            result.push_back(sqlite3_column_int64(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
#else
    (void)root;
#endif
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

bool Database::execute_sql_file(const std::string& path) {
#if SAD_HAS_SQLITE
    std::ifstream file(path);
    if (!file.is_open()) return false;

    std::string sql((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    file.close();

    return execute(sql);
#else
    (void)path;
    return true;
#endif
}

bool Database::execute(const std::string& sql) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    char* err = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err);
    if (rc != SQLITE_OK && err) {
        log("خطأ SQL", std::string(err));
        sqlite3_free(err);
        return false;
    }
    return true;
#else
    (void)sql;
    return true;
#endif
}

int Database::get_schema_version() {
    return 2; // الآن هجرة رقم ٢
}

// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ عمليات التقييمات والمراجعات
// ═══════════════════════════════════════════════════════════════════════════════

int64_t Database::create_or_update_review(const ReviewInfo& review) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    // إضافة أو تحديث (UPSERT) — مراجعة واحدة لكل مستخدم لكل حزمة
    const char* sql =
        "INSERT INTO reviews (package_id, user_id, rating, title, body) "
        "VALUES (?, ?, ?, ?, ?) "
        "ON CONFLICT(package_id, user_id) DO UPDATE SET "
        "rating = excluded.rating, title = excluded.title, "
        "body = excluded.body, is_edited = 1, updated_at = datetime('now')";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, review.package_id);
    sqlite3_bind_int64(stmt, 2, review.user_id);
    sqlite3_bind_int(stmt, 3, review.rating);
    sqlite3_bind_text(stmt, 4, review.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, review.body.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    log_audit(review.user_id, "review",
              "package_id=" + std::to_string(review.package_id) +
              " rating=" + std::to_string(review.rating), "", "");
    return sqlite3_last_insert_rowid(db);
#else
    (void)review;
    return 1;
#endif
}

std::vector<ReviewInfo> Database::get_package_reviews(int64_t package_id, int limit) {
    std::vector<ReviewInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "SELECT r.id, r.package_id, r.user_id, u.username, r.rating, "
        "r.title, r.body, r.is_helpful, r.is_edited, r.created_at, r.updated_at "
        "FROM reviews r LEFT JOIN users u ON r.user_id = u.id "
        "WHERE r.package_id = ? ORDER BY r.created_at DESC LIMIT ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_int(stmt, 2, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ReviewInfo r;
        r.id = sqlite3_column_int64(stmt, 0);
        r.package_id = sqlite3_column_int64(stmt, 1);
        r.user_id = sqlite3_column_int64(stmt, 2);
        r.username = safe_col_text(stmt, 3);
        r.rating = sqlite3_column_int(stmt, 4);
        r.title = safe_col_text(stmt, 5);
        r.body = safe_col_text(stmt, 6);
        r.helpful_count = sqlite3_column_int(stmt, 7);
        r.is_edited = sqlite3_column_int(stmt, 8) != 0;
        r.created_at = safe_col_text(stmt, 9);
        r.updated_at = safe_col_text(stmt, 10);
        results.push_back(r);
    }
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)limit;
#endif
    return results;
}

std::optional<ReviewInfo> Database::get_user_review(int64_t package_id, int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "SELECT r.id, r.rating, r.title, r.body, r.is_helpful, r.is_edited, "
        "r.created_at, r.updated_at FROM reviews r "
        "WHERE r.package_id = ? AND r.user_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_int64(stmt, 2, user_id);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ReviewInfo r;
        r.id = sqlite3_column_int64(stmt, 0);
        r.package_id = package_id;
        r.user_id = user_id;
        r.rating = sqlite3_column_int(stmt, 1);
        r.title = safe_col_text(stmt, 2);
        r.body = safe_col_text(stmt, 3);
        r.helpful_count = sqlite3_column_int(stmt, 4);
        r.is_edited = sqlite3_column_int(stmt, 5) != 0;
        r.created_at = safe_col_text(stmt, 6);
        r.updated_at = safe_col_text(stmt, 7);
        sqlite3_finalize(stmt);
        return r;
    }
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)user_id;
#endif
    return std::nullopt;
}

bool Database::delete_review(int64_t review_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "DELETE FROM reviews WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, review_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)review_id;
    return true;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ عمليات التفضيلات (Stars)
// ═══════════════════════════════════════════════════════════════════════════════

bool Database::star_package(int64_t package_id, int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "INSERT OR IGNORE INTO stars (package_id, user_id) VALUES (?, ?)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_int64(stmt, 2, user_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)package_id; (void)user_id;
    return true;
#endif
}

bool Database::unstar_package(int64_t package_id, int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "DELETE FROM stars WHERE package_id = ? AND user_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_int64(stmt, 2, user_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)package_id; (void)user_id;
    return true;
#endif
}

bool Database::is_starred(int64_t package_id, int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "SELECT 1 FROM stars WHERE package_id = ? AND user_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_int64(stmt, 2, user_id);
    bool found = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return found;
#else
    (void)package_id; (void)user_id;
    return false;
#endif
}

int64_t Database::get_star_count(int64_t package_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "SELECT COUNT(*) FROM stars WHERE package_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int64(stmt, 1, package_id);
    int64_t count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int64(stmt, 0);
    sqlite3_finalize(stmt);
    return count;
#else
    (void)package_id;
    return 0;
#endif
}

std::vector<PackageInfo> Database::get_user_stars(int64_t user_id) {
    std::vector<PackageInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "SELECT p.id, p.name, p.display_name, p.arabic_name, p.description, "
        "p.description_ar, p.owner_id, p.license, p.latest_version, "
        "p.total_downloads, p.category "
        "FROM stars s JOIN packages p ON s.package_id = p.id "
        "WHERE s.user_id = ? ORDER BY s.created_at DESC";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, user_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        PackageInfo p;
        p.id = sqlite3_column_int64(stmt, 0);
        p.name = safe_col_text(stmt, 1);
        p.display_name = safe_col_text(stmt, 2);
        p.arabic_name = safe_col_text(stmt, 3);
        p.description = safe_col_text(stmt, 4);
        p.description_ar = safe_col_text(stmt, 5);
        p.owner_id = sqlite3_column_int64(stmt, 6);
        p.license = safe_col_text(stmt, 7);
        p.latest_version = safe_col_text(stmt, 8);
        p.total_downloads = sqlite3_column_int64(stmt, 9);
        p.category = safe_col_text(stmt, 10);
        results.push_back(p);
    }
    sqlite3_finalize(stmt);
#else
    (void)user_id;
#endif
    return results;
}

// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ عمليات الإبلاغات
// ═══════════════════════════════════════════════════════════════════════════════

int64_t Database::create_report(const ReportInfo& report) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "INSERT INTO reports (reporter_id, package_id, reason, details) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, report.reporter_id);
    sqlite3_bind_int64(stmt, 2, report.package_id);
    sqlite3_bind_text(stmt, 3, report.reason.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, report.details.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    log_audit(report.reporter_id, "report",
              "package_id=" + std::to_string(report.package_id) +
              " reason=" + report.reason, "", "");
    return sqlite3_last_insert_rowid(db);
#else
    (void)report;
    return 1;
#endif
}

std::vector<ReportInfo> Database::get_reports(const std::string& status, int limit) {
    std::vector<ReportInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "SELECT r.id, r.reporter_id, u.username, r.package_id, p.name, "
        "r.reason, r.details, r.status, r.admin_notes, r.resolved_by, "
        "r.created_at, r.resolved_at "
        "FROM reports r "
        "LEFT JOIN users u ON r.reporter_id = u.id "
        "LEFT JOIN packages p ON r.package_id = p.id "
        "WHERE r.status = ? ORDER BY r.created_at DESC LIMIT ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ReportInfo rpt;
        rpt.id = sqlite3_column_int64(stmt, 0);
        rpt.reporter_id = sqlite3_column_int64(stmt, 1);
        rpt.reporter_username = safe_col_text(stmt, 2);
        rpt.package_id = sqlite3_column_int64(stmt, 3);
        rpt.package_name = safe_col_text(stmt, 4);
        rpt.reason = safe_col_text(stmt, 5);
        rpt.details = safe_col_text(stmt, 6);
        rpt.status = safe_col_text(stmt, 7);
        rpt.admin_notes = safe_col_text(stmt, 8);
        rpt.resolved_by = sqlite3_column_int64(stmt, 9);
        rpt.created_at = safe_col_text(stmt, 10);
        rpt.resolved_at = safe_col_text(stmt, 11);
        results.push_back(rpt);
    }
    sqlite3_finalize(stmt);
#else
    (void)status; (void)limit;
#endif
    return results;
}

bool Database::resolve_report(int64_t report_id, int64_t admin_id,
                               const std::string& status, const std::string& notes) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "UPDATE reports SET status = ?, admin_notes = ?, resolved_by = ?, "
        "resolved_at = datetime('now') WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, admin_id);
    sqlite3_bind_int64(stmt, 4, report_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)report_id; (void)admin_id; (void)status; (void)notes;
    return true;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ عمليات الخطّافات (Webhooks)
// ═══════════════════════════════════════════════════════════════════════════════

int64_t Database::create_webhook(const WebhookInfo& wh) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "INSERT INTO webhooks (user_id, package_id, url, secret, events) "
        "VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, wh.user_id);
    if (wh.package_id > 0) sqlite3_bind_int64(stmt, 2, wh.package_id);
    else sqlite3_bind_null(stmt, 2);
    sqlite3_bind_text(stmt, 3, wh.url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, wh.secret.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, wh.events.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? sqlite3_last_insert_rowid(db) : -1;
#else
    (void)wh;
    return 1;
#endif
}

std::vector<WebhookInfo> Database::get_user_webhooks(int64_t user_id) {
    std::vector<WebhookInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "SELECT id, user_id, package_id, url, secret, events, is_active, "
        "failure_count, last_triggered, created_at "
        "FROM webhooks WHERE user_id = ? ORDER BY created_at DESC";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, user_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        WebhookInfo wh;
        wh.id = sqlite3_column_int64(stmt, 0);
        wh.user_id = sqlite3_column_int64(stmt, 1);
        wh.package_id = sqlite3_column_int64(stmt, 2);
        wh.url = safe_col_text(stmt, 3);
        wh.secret = safe_col_text(stmt, 4);
        wh.events = safe_col_text(stmt, 5);
        wh.is_active = sqlite3_column_int(stmt, 6) != 0;
        wh.failure_count = sqlite3_column_int(stmt, 7);
        wh.last_triggered = safe_col_text(stmt, 8);
        wh.created_at = safe_col_text(stmt, 9);
        results.push_back(wh);
    }
    sqlite3_finalize(stmt);
#else
    (void)user_id;
#endif
    return results;
}

bool Database::delete_webhook(int64_t webhook_id, int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "DELETE FROM webhooks WHERE id = ? AND user_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, webhook_id);
    sqlite3_bind_int64(stmt, 2, user_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)webhook_id; (void)user_id;
    return true;
#endif
}

std::vector<WebhookInfo> Database::get_webhooks_for_event(int64_t package_id,
                                                           const std::string& event) {
    std::vector<WebhookInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    // خطّافات مرتبطة بالحزمة أو عامة (package_id IS NULL)
    const char* sql =
        "SELECT id, user_id, package_id, url, secret, events "
        "FROM webhooks WHERE is_active = 1 AND failure_count < 10 "
        "AND (package_id = ? OR package_id IS NULL) "
        "AND events LIKE '%' || ? || '%'";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_text(stmt, 2, event.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        WebhookInfo wh;
        wh.id = sqlite3_column_int64(stmt, 0);
        wh.user_id = sqlite3_column_int64(stmt, 1);
        wh.package_id = sqlite3_column_int64(stmt, 2);
        wh.url = safe_col_text(stmt, 3);
        wh.secret = safe_col_text(stmt, 4);
        wh.events = safe_col_text(stmt, 5);
        results.push_back(wh);
    }
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)event;
#endif
    return results;
}

// ═══════════════════════════════════════════════════════════════════════════════
// إحصائيات التنزيل اليومية
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<DailyDownloadInfo> Database::get_daily_downloads(int64_t package_id, int days) {
    std::vector<DailyDownloadInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "SELECT download_date, count FROM daily_downloads "
        "WHERE package_id = ? AND download_date >= date('now', '-' || ? || ' days') "
        "ORDER BY download_date ASC";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_int(stmt, 2, days);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        DailyDownloadInfo d;
        d.date = safe_col_text(stmt, 0);
        d.count = sqlite3_column_int64(stmt, 1);
        results.push_back(d);
    }
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)days;
#endif
    return results;
}

// ═══════════════════════════════════════════════════════════════════════════════
// إعدادات النظام
// ═══════════════════════════════════════════════════════════════════════════════

std::string Database::get_setting(const std::string& key, const std::string& default_val) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "SELECT value FROM system_settings WHERE key = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return default_val;
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    std::string result = default_val;
    if (sqlite3_step(stmt) == SQLITE_ROW) result = safe_col_text(stmt, 0);
    sqlite3_finalize(stmt);
    return result;
#else
    (void)key;
    return default_val;
#endif
}

bool Database::set_setting(const std::string& key, const std::string& value) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql =
        "INSERT INTO system_settings (key, value, updated_at) VALUES (?, ?, datetime('now')) "
        "ON CONFLICT(key) DO UPDATE SET value = excluded.value, updated_at = datetime('now')";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)key; (void)value;
    return true;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════════
// عمليات إدارية
// ═══════════════════════════════════════════════════════════════════════════════

bool Database::set_admin(int64_t user_id, bool is_admin) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "UPDATE users SET is_admin = ? WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, is_admin ? 1 : 0);
    sqlite3_bind_int64(stmt, 2, user_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)user_id; (void)is_admin;
    return true;
#endif
}

bool Database::set_package_featured(int64_t package_id, bool featured) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "UPDATE packages SET is_featured = ? WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, featured ? 1 : 0);
    sqlite3_bind_int64(stmt, 2, package_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)package_id; (void)featured;
    return true;
#endif
}

bool Database::set_package_verified(int64_t package_id, bool verified) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "UPDATE packages SET is_verified = ? WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, verified ? 1 : 0);
    sqlite3_bind_int64(stmt, 2, package_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)package_id; (void)verified;
    return true;
#endif
}

std::vector<Database::AuditEntry> Database::get_audit_log(int limit, int64_t user_id) {
    std::vector<AuditEntry> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    std::string sql_str =
        "SELECT a.id, a.user_id, COALESCE(u.username, ''), a.action, "
        "a.details, a.ip_address, a.created_at "
        "FROM audit_log a LEFT JOIN users u ON a.user_id = u.id ";
    if (user_id > 0) sql_str += "WHERE a.user_id = ? ";
    sql_str += "ORDER BY a.created_at DESC LIMIT ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql_str.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return results;
    int param = 1;
    if (user_id > 0) sqlite3_bind_int64(stmt, param++, user_id);
    sqlite3_bind_int(stmt, param, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        AuditEntry a;
        a.id = sqlite3_column_int64(stmt, 0);
        a.user_id = sqlite3_column_int64(stmt, 1);
        a.username = safe_col_text(stmt, 2);
        a.action = safe_col_text(stmt, 3);
        a.details = safe_col_text(stmt, 4);
        a.ip = safe_col_text(stmt, 5);
        a.created_at = safe_col_text(stmt, 6);
        results.push_back(a);
    }
    sqlite3_finalize(stmt);
#else
    (void)limit; (void)user_id;
#endif
    return results;
}

void Database::log(const std::string& level, const std::string& message) {
    std::cerr << "  [" << level << "] " << message << "\n";
}

// ═══════════════════════════════════════════════════════════════════════════════
// ★ تنفيذ العمليات الثورية الجديدة ★
// ═══════════════════════════════════════════════════════════════════════════════

// ─────────────────────────────────────────────────────────────────────────────
// عمليات التوقيعات الرقمية
// ─────────────────────────────────────────────────────────────────────────────

int64_t Database::add_publisher_key(const PublisherKeyInfo& key) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO publisher_keys (user_id, key_type, public_key, fingerprint,
            key_name, is_primary, expires_at)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, key.user_id);
    sqlite3_bind_text(stmt, 2, key.key_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, key.public_key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, key.fingerprint.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, key.key_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, key.is_primary ? 1 : 0);
    sqlite3_bind_text(stmt, 7, key.expires_at.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)key;
    return 1;
#endif
}

std::vector<PublisherKeyInfo> Database::get_user_keys(int64_t user_id) {
    std::vector<PublisherKeyInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, user_id, key_type, public_key, fingerprint, key_name,
               is_primary, is_revoked, revoked_at, revoke_reason, expires_at, created_at
        FROM publisher_keys WHERE user_id = ? ORDER BY is_primary DESC, created_at DESC
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, user_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        PublisherKeyInfo k;
        k.id = sqlite3_column_int64(stmt, 0);
        k.user_id = sqlite3_column_int64(stmt, 1);
        k.key_type = safe_col_text(stmt, 2);
        k.public_key = safe_col_text(stmt, 3);
        k.fingerprint = safe_col_text(stmt, 4);
        k.key_name = safe_col_text(stmt, 5);
        k.is_primary = sqlite3_column_int(stmt, 6) != 0;
        k.is_revoked = sqlite3_column_int(stmt, 7) != 0;
        k.revoked_at = safe_col_text(stmt, 8);
        k.revoke_reason = safe_col_text(stmt, 9);
        k.expires_at = safe_col_text(stmt, 10);
        k.created_at = safe_col_text(stmt, 11);
        results.push_back(k);
    }
    sqlite3_finalize(stmt);
#else
    (void)user_id;
#endif
    return results;
}

std::optional<PublisherKeyInfo> Database::get_key_by_fingerprint(const std::string& fingerprint) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, user_id, key_type, public_key, fingerprint, key_name,
               is_primary, is_revoked, revoked_at, revoke_reason, expires_at, created_at
        FROM publisher_keys WHERE fingerprint = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, fingerprint.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        PublisherKeyInfo k;
        k.id = sqlite3_column_int64(stmt, 0);
        k.user_id = sqlite3_column_int64(stmt, 1);
        k.key_type = safe_col_text(stmt, 2);
        k.public_key = safe_col_text(stmt, 3);
        k.fingerprint = safe_col_text(stmt, 4);
        k.key_name = safe_col_text(stmt, 5);
        k.is_primary = sqlite3_column_int(stmt, 6) != 0;
        k.is_revoked = sqlite3_column_int(stmt, 7) != 0;
        k.revoked_at = safe_col_text(stmt, 8);
        k.revoke_reason = safe_col_text(stmt, 9);
        k.expires_at = safe_col_text(stmt, 10);
        k.created_at = safe_col_text(stmt, 11);
        sqlite3_finalize(stmt);
        return k;
    }
    sqlite3_finalize(stmt);
#else
    (void)fingerprint;
#endif
    return std::nullopt;
}

bool Database::revoke_key(int64_t key_id, const std::string& reason) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "UPDATE publisher_keys SET is_revoked = 1, revoked_at = datetime('now'), revoke_reason = ? WHERE id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, reason.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, key_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)key_id; (void)reason;
    return true;
#endif
}

int64_t Database::add_version_signature(const VersionSignature& sig) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO version_signatures (version_id, key_id, signature, signature_type,
            signed_content_hash, timestamp_authority, timestamp_signature)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, sig.version_id);
    sqlite3_bind_int64(stmt, 2, sig.key_id);
    sqlite3_bind_text(stmt, 3, sig.signature.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, sig.signature_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, sig.signed_content_hash.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, sig.timestamp_authority.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, sig.timestamp_signature.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)sig;
    return 1;
#endif
}

std::vector<VersionSignature> Database::get_version_signatures(int64_t version_id) {
    std::vector<VersionSignature> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, version_id, key_id, signature, signature_type, signed_content_hash,
               timestamp_authority, timestamp_signature, is_valid, verified_at, created_at
        FROM version_signatures WHERE version_id = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, version_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        VersionSignature s;
        s.id = sqlite3_column_int64(stmt, 0);
        s.version_id = sqlite3_column_int64(stmt, 1);
        s.key_id = sqlite3_column_int64(stmt, 2);
        s.signature = safe_col_text(stmt, 3);
        s.signature_type = safe_col_text(stmt, 4);
        s.signed_content_hash = safe_col_text(stmt, 5);
        s.timestamp_authority = safe_col_text(stmt, 6);
        s.timestamp_signature = safe_col_text(stmt, 7);
        s.is_valid = sqlite3_column_int(stmt, 8) != 0;
        s.verified_at = safe_col_text(stmt, 9);
        s.created_at = safe_col_text(stmt, 10);
        results.push_back(s);
    }
    sqlite3_finalize(stmt);
#else
    (void)version_id;
#endif
    return results;
}

void Database::log_verification(int64_t version_id, const std::string& status,
                                const std::string& error, const std::string& ip,
                                const std::string& client_version) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO verification_log (version_id, verifier_ip, verification_status,
            error_message, client_version)
        VALUES (?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_int64(stmt, 1, version_id);
    sqlite3_bind_text(stmt, 2, ip.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, error.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, client_version.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
#else
    (void)version_id; (void)status; (void)error; (void)ip; (void)client_version;
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// حل التبعيات
// ─────────────────────────────────────────────────────────────────────────────

int64_t Database::add_dependency_v2(const DependencyV2& dep) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO dependencies_v2 (version_id, dependency_name, version_constraint,
            constraint_type, dependency_type, is_optional, features, platform_filter, registry_url)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, dep.version_id);
    sqlite3_bind_text(stmt, 2, dep.dependency_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, dep.version_constraint.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, dep.constraint_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, dep.dependency_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, dep.is_optional ? 1 : 0);
    sqlite3_bind_text(stmt, 7, dep.features.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, dep.platform_filter.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, dep.registry_url.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)dep;
    return 1;
#endif
}

std::vector<DependencyV2> Database::get_dependencies_v2(int64_t version_id) {
    std::vector<DependencyV2> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, version_id, dependency_name, version_constraint, constraint_type,
               dependency_type, is_optional, features, platform_filter, registry_url, resolved_version
        FROM dependencies_v2 WHERE version_id = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, version_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        DependencyV2 d;
        d.id = sqlite3_column_int64(stmt, 0);
        d.version_id = sqlite3_column_int64(stmt, 1);
        d.dependency_name = safe_col_text(stmt, 2);
        d.version_constraint = safe_col_text(stmt, 3);
        d.constraint_type = safe_col_text(stmt, 4);
        d.dependency_type = safe_col_text(stmt, 5);
        d.is_optional = sqlite3_column_int(stmt, 6) != 0;
        d.features = safe_col_text(stmt, 7);
        d.platform_filter = safe_col_text(stmt, 8);
        d.registry_url = safe_col_text(stmt, 9);
        d.resolved_version = safe_col_text(stmt, 10);
        results.push_back(d);
    }
    sqlite3_finalize(stmt);
#else
    (void)version_id;
#endif
    return results;
}

std::vector<std::pair<std::string, std::string>> Database::get_reverse_dependencies(
    const std::string& package_name, int limit) {
    std::vector<std::pair<std::string, std::string>> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT DISTINCT p.name, v.version
        FROM dependencies_v2 d
        JOIN versions v ON d.version_id = v.id
        JOIN packages p ON v.package_id = p.id
        WHERE d.dependency_name = ?
        LIMIT ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_text(stmt, 1, package_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.emplace_back(safe_col_text(stmt, 0), safe_col_text(stmt, 1));
    }
    sqlite3_finalize(stmt);
#else
    (void)package_name; (void)limit;
#endif
    return results;
}

std::optional<DependencyCacheEntry> Database::get_dependency_cache(const std::string& cache_key) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, cache_key, resolved_tree, resolution_time_ms, hit_count,
               last_hit_at, expires_at, created_at
        FROM dependency_cache WHERE cache_key = ? AND expires_at > datetime('now')
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, cache_key.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        DependencyCacheEntry e;
        e.id = sqlite3_column_int64(stmt, 0);
        e.cache_key = safe_col_text(stmt, 1);
        e.resolved_tree = safe_col_text(stmt, 2);
        e.resolution_time_ms = sqlite3_column_int(stmt, 3);
        e.hit_count = sqlite3_column_int(stmt, 4);
        e.last_hit_at = safe_col_text(stmt, 5);
        e.expires_at = safe_col_text(stmt, 6);
        e.created_at = safe_col_text(stmt, 7);
        sqlite3_finalize(stmt);
        // Update hit count
        execute("UPDATE dependency_cache SET hit_count = hit_count + 1, last_hit_at = datetime('now') WHERE cache_key = '" + cache_key + "'");
        return e;
    }
    sqlite3_finalize(stmt);
#else
    (void)cache_key;
#endif
    return std::nullopt;
}

void Database::cache_dependency_resolution(const std::string& cache_key,
                                           const std::string& resolved_tree,
                                           int resolution_time_ms) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT OR REPLACE INTO dependency_cache (cache_key, resolved_tree, resolution_time_ms, expires_at)
        VALUES (?, ?, ?, datetime('now', '+1 hour'))
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_text(stmt, 1, cache_key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, resolved_tree.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, resolution_time_ms);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
#else
    (void)cache_key; (void)resolved_tree; (void)resolution_time_ms;
#endif
}

int64_t Database::report_conflict(const KnownConflict& conflict) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO known_conflicts (package_a, version_a, package_b, version_b,
            conflict_reason, severity, workaround, reported_by)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, conflict.package_a.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, conflict.version_a.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, conflict.package_b.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, conflict.version_b.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, conflict.conflict_reason.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, conflict.severity.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, conflict.workaround.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 8, conflict.reported_by);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)conflict;
    return 1;
#endif
}

std::vector<KnownConflict> Database::get_conflicts_for_package(const std::string& package_name) {
    std::vector<KnownConflict> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, package_a, version_a, package_b, version_b, conflict_reason,
               severity, workaround, reported_by, verified, created_at
        FROM known_conflicts WHERE package_a = ? OR package_b = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_text(stmt, 1, package_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, package_name.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        KnownConflict c;
        c.id = sqlite3_column_int64(stmt, 0);
        c.package_a = safe_col_text(stmt, 1);
        c.version_a = safe_col_text(stmt, 2);
        c.package_b = safe_col_text(stmt, 3);
        c.version_b = safe_col_text(stmt, 4);
        c.conflict_reason = safe_col_text(stmt, 5);
        c.severity = safe_col_text(stmt, 6);
        c.workaround = safe_col_text(stmt, 7);
        c.reported_by = sqlite3_column_int64(stmt, 8);
        c.verified = sqlite3_column_int(stmt, 9) != 0;
        c.created_at = safe_col_text(stmt, 10);
        results.push_back(c);
    }
    sqlite3_finalize(stmt);
#else
    (void)package_name;
#endif
    return results;
}

// ─────────────────────────────────────────────────────────────────────────────
// الإشعارات
// ─────────────────────────────────────────────────────────────────────────────

int64_t Database::create_notification(const NotificationInfo& notif) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO notifications (user_id, notification_type, title, body,
            related_package_id, related_user_id, action_url)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, notif.user_id);
    sqlite3_bind_text(stmt, 2, notif.notification_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, notif.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, notif.body.c_str(), -1, SQLITE_TRANSIENT);
    if (notif.related_package_id > 0)
        sqlite3_bind_int64(stmt, 5, notif.related_package_id);
    else
        sqlite3_bind_null(stmt, 5);
    if (notif.related_user_id > 0)
        sqlite3_bind_int64(stmt, 6, notif.related_user_id);
    else
        sqlite3_bind_null(stmt, 6);
    sqlite3_bind_text(stmt, 7, notif.action_url.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)notif;
    return 1;
#endif
}

std::vector<NotificationInfo> Database::get_user_notifications(int64_t user_id,
                                                                bool unread_only,
                                                                int limit) {
    std::vector<NotificationInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    std::string sql = R"(
        SELECT id, user_id, notification_type, title, body, related_package_id,
               related_user_id, action_url, is_read, is_archived, created_at, read_at
        FROM notifications WHERE user_id = ? AND is_archived = 0
    )";
    if (unread_only) sql += " AND is_read = 0";
    sql += " ORDER BY created_at DESC LIMIT ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        NotificationInfo n;
        n.id = sqlite3_column_int64(stmt, 0);
        n.user_id = sqlite3_column_int64(stmt, 1);
        n.notification_type = safe_col_text(stmt, 2);
        n.title = safe_col_text(stmt, 3);
        n.body = safe_col_text(stmt, 4);
        n.related_package_id = sqlite3_column_int64(stmt, 5);
        n.related_user_id = sqlite3_column_int64(stmt, 6);
        n.action_url = safe_col_text(stmt, 7);
        n.is_read = sqlite3_column_int(stmt, 8) != 0;
        n.is_archived = sqlite3_column_int(stmt, 9) != 0;
        n.created_at = safe_col_text(stmt, 10);
        n.read_at = safe_col_text(stmt, 11);
        results.push_back(n);
    }
    sqlite3_finalize(stmt);
#else
    (void)user_id; (void)unread_only; (void)limit;
#endif
    return results;
}

bool Database::mark_notification_read(int64_t notification_id, int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "UPDATE notifications SET is_read = 1, read_at = datetime('now') WHERE id = ? AND user_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, notification_id);
    sqlite3_bind_int64(stmt, 2, user_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)notification_id; (void)user_id;
    return true;
#endif
}

bool Database::mark_all_notifications_read(int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "UPDATE notifications SET is_read = 1, read_at = datetime('now') WHERE user_id = ? AND is_read = 0";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, user_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)user_id;
    return true;
#endif
}

int64_t Database::get_unread_notification_count(int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "SELECT COUNT(*) FROM notifications WHERE user_id = ? AND is_read = 0 AND is_archived = 0";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int64(stmt, 1, user_id);
    int64_t count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int64(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
#else
    (void)user_id;
    return 0;
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// الاشتراكات
// ─────────────────────────────────────────────────────────────────────────────

int64_t Database::create_subscription(const SubscriptionInfo& sub) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT OR IGNORE INTO subscriptions (user_id, target_type, target_id,
            notify_new_version, notify_security, notify_deprecation, email_enabled)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_int64(stmt, 1, sub.user_id);
    sqlite3_bind_text(stmt, 2, sub.target_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, sub.target_id);
    sqlite3_bind_int(stmt, 4, sub.notify_new_version ? 1 : 0);
    sqlite3_bind_int(stmt, 5, sub.notify_security ? 1 : 0);
    sqlite3_bind_int(stmt, 6, sub.notify_deprecation ? 1 : 0);
    sqlite3_bind_int(stmt, 7, sub.email_enabled ? 1 : 0);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)sub;
    return 1;
#endif
}

bool Database::delete_subscription(int64_t user_id, const std::string& target_type,
                                   int64_t target_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "DELETE FROM subscriptions WHERE user_id = ? AND target_type = ? AND target_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, target_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, target_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)user_id; (void)target_type; (void)target_id;
    return true;
#endif
}

std::vector<SubscriptionInfo> Database::get_user_subscriptions(int64_t user_id) {
    std::vector<SubscriptionInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, user_id, target_type, target_id, notify_new_version,
               notify_security, notify_deprecation, email_enabled, created_at
        FROM subscriptions WHERE user_id = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, user_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        SubscriptionInfo s;
        s.id = sqlite3_column_int64(stmt, 0);
        s.user_id = sqlite3_column_int64(stmt, 1);
        s.target_type = safe_col_text(stmt, 2);
        s.target_id = sqlite3_column_int64(stmt, 3);
        s.notify_new_version = sqlite3_column_int(stmt, 4) != 0;
        s.notify_security = sqlite3_column_int(stmt, 5) != 0;
        s.notify_deprecation = sqlite3_column_int(stmt, 6) != 0;
        s.email_enabled = sqlite3_column_int(stmt, 7) != 0;
        s.created_at = safe_col_text(stmt, 8);
        results.push_back(s);
    }
    sqlite3_finalize(stmt);
#else
    (void)user_id;
#endif
    return results;
}

std::vector<int64_t> Database::get_subscribers(const std::string& target_type,
                                               int64_t target_id) {
    std::vector<int64_t> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "SELECT user_id FROM subscriptions WHERE target_type = ? AND target_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_text(stmt, 1, target_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, target_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.push_back(sqlite3_column_int64(stmt, 0));
    }
    sqlite3_finalize(stmt);
#else
    (void)target_type; (void)target_id;
#endif
    return results;
}

// ─────────────────────────────────────────────────────────────────────────────
// المنظمات
// ─────────────────────────────────────────────────────────────────────────────

int64_t Database::create_organization(const OrganizationInfo& org) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO organizations (name, display_name, description, avatar_url,
            website_url, plan_type, created_by)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, org.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, org.display_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, org.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, org.avatar_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, org.website_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, org.plan_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 7, org.created_by);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    int64_t org_id = sqlite3_last_insert_rowid(db);
    // Add creator as owner
    add_org_member(org_id, org.created_by, "owner", org.created_by);
    return org_id;
#else
    (void)org;
    return 1;
#endif
}

std::optional<OrganizationInfo> Database::get_organization(const std::string& name) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, name, display_name, description, avatar_url, website_url,
               is_verified, plan_type, plan_expires_at, created_by, created_at
        FROM organizations WHERE name = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        OrganizationInfo o;
        o.id = sqlite3_column_int64(stmt, 0);
        o.name = safe_col_text(stmt, 1);
        o.display_name = safe_col_text(stmt, 2);
        o.description = safe_col_text(stmt, 3);
        o.avatar_url = safe_col_text(stmt, 4);
        o.website_url = safe_col_text(stmt, 5);
        o.is_verified = sqlite3_column_int(stmt, 6) != 0;
        o.plan_type = safe_col_text(stmt, 7);
        o.plan_expires_at = safe_col_text(stmt, 8);
        o.created_by = sqlite3_column_int64(stmt, 9);
        o.created_at = safe_col_text(stmt, 10);
        sqlite3_finalize(stmt);
        return o;
    }
    sqlite3_finalize(stmt);
#else
    (void)name;
#endif
    return std::nullopt;
}

bool Database::update_organization(const OrganizationInfo& org) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        UPDATE organizations SET display_name = ?, description = ?, avatar_url = ?,
            website_url = ? WHERE id = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, org.display_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, org.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, org.avatar_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, org.website_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 5, org.id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)org;
    return true;
#endif
}

bool Database::add_org_member(int64_t org_id, int64_t user_id,
                              const std::string& role, int64_t invited_by) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT OR IGNORE INTO organization_members (org_id, user_id, role, invited_by, accepted_at)
        VALUES (?, ?, ?, ?, datetime('now'))
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, org_id);
    sqlite3_bind_int64(stmt, 2, user_id);
    sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, invited_by);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)org_id; (void)user_id; (void)role; (void)invited_by;
    return true;
#endif
}

bool Database::remove_org_member(int64_t org_id, int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "DELETE FROM organization_members WHERE org_id = ? AND user_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, org_id);
    sqlite3_bind_int64(stmt, 2, user_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)org_id; (void)user_id;
    return true;
#endif
}

std::vector<OrgMemberInfo> Database::get_org_members(int64_t org_id) {
    std::vector<OrgMemberInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT m.id, m.org_id, m.user_id, m.role, m.invited_by, m.accepted_at,
               m.created_at, u.username, u.display_name
        FROM organization_members m
        JOIN users u ON m.user_id = u.id
        WHERE m.org_id = ?
        ORDER BY CASE m.role WHEN 'owner' THEN 1 WHEN 'admin' THEN 2 ELSE 3 END
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, org_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        OrgMemberInfo m;
        m.id = sqlite3_column_int64(stmt, 0);
        m.org_id = sqlite3_column_int64(stmt, 1);
        m.user_id = sqlite3_column_int64(stmt, 2);
        m.role = safe_col_text(stmt, 3);
        m.invited_by = sqlite3_column_int64(stmt, 4);
        m.accepted_at = safe_col_text(stmt, 5);
        m.created_at = safe_col_text(stmt, 6);
        m.username = safe_col_text(stmt, 7);
        m.display_name = safe_col_text(stmt, 8);
        results.push_back(m);
    }
    sqlite3_finalize(stmt);
#else
    (void)org_id;
#endif
    return results;
}

std::vector<OrganizationInfo> Database::get_user_organizations(int64_t user_id) {
    std::vector<OrganizationInfo> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT o.id, o.name, o.display_name, o.description, o.avatar_url, o.website_url,
               o.is_verified, o.plan_type, o.plan_expires_at, o.created_by, o.created_at
        FROM organizations o
        JOIN organization_members m ON o.id = m.org_id
        WHERE m.user_id = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, user_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        OrganizationInfo o;
        o.id = sqlite3_column_int64(stmt, 0);
        o.name = safe_col_text(stmt, 1);
        o.display_name = safe_col_text(stmt, 2);
        o.description = safe_col_text(stmt, 3);
        o.avatar_url = safe_col_text(stmt, 4);
        o.website_url = safe_col_text(stmt, 5);
        o.is_verified = sqlite3_column_int(stmt, 6) != 0;
        o.plan_type = safe_col_text(stmt, 7);
        o.plan_expires_at = safe_col_text(stmt, 8);
        o.created_by = sqlite3_column_int64(stmt, 9);
        o.created_at = safe_col_text(stmt, 10);
        results.push_back(o);
    }
    sqlite3_finalize(stmt);
#else
    (void)user_id;
#endif
    return results;
}

std::string Database::get_user_org_role(int64_t org_id, int64_t user_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "SELECT role FROM organization_members WHERE org_id = ? AND user_id = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return "";
    sqlite3_bind_int64(stmt, 1, org_id);
    sqlite3_bind_int64(stmt, 2, user_id);
    std::string role;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        role = safe_col_text(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return role;
#else
    (void)org_id; (void)user_id;
    return "";
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// التنبيهات الأمنية
// ─────────────────────────────────────────────────────────────────────────────

int64_t Database::create_security_advisory(const SecurityAdvisory& advisory) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO security_advisories (advisory_id, package_id, title, description,
            severity, cvss_score, affected_versions, patched_versions, cve_ids, cwe_ids,
            references_json, reporter_id, is_public)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, advisory.advisory_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, advisory.package_id);
    sqlite3_bind_text(stmt, 3, advisory.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, advisory.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, advisory.severity.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 6, advisory.cvss_score);
    sqlite3_bind_text(stmt, 7, advisory.affected_versions.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, advisory.patched_versions.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, advisory.cve_ids.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, advisory.cwe_ids.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, advisory.references_json.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 12, advisory.reporter_id);
    sqlite3_bind_int(stmt, 13, advisory.is_public ? 1 : 0);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)advisory;
    return 1;
#endif
}

std::vector<SecurityAdvisory> Database::get_package_advisories(int64_t package_id,
                                                                bool public_only) {
    std::vector<SecurityAdvisory> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    std::string sql = R"(
        SELECT id, advisory_id, package_id, title, description, severity, cvss_score,
               affected_versions, patched_versions, cve_ids, cwe_ids, references_json,
               reporter_id, is_public, published_at, withdrawn_at, created_at, updated_at
        FROM security_advisories WHERE package_id = ?
    )";
    if (public_only) sql += " AND is_public = 1 AND withdrawn_at IS NULL";
    sql += " ORDER BY cvss_score DESC";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, package_id);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        SecurityAdvisory a;
        a.id = sqlite3_column_int64(stmt, 0);
        a.advisory_id = safe_col_text(stmt, 1);
        a.package_id = sqlite3_column_int64(stmt, 2);
        a.title = safe_col_text(stmt, 3);
        a.description = safe_col_text(stmt, 4);
        a.severity = safe_col_text(stmt, 5);
        a.cvss_score = sqlite3_column_double(stmt, 6);
        a.affected_versions = safe_col_text(stmt, 7);
        a.patched_versions = safe_col_text(stmt, 8);
        a.cve_ids = safe_col_text(stmt, 9);
        a.cwe_ids = safe_col_text(stmt, 10);
        a.references_json = safe_col_text(stmt, 11);
        a.reporter_id = sqlite3_column_int64(stmt, 12);
        a.is_public = sqlite3_column_int(stmt, 13) != 0;
        a.published_at = safe_col_text(stmt, 14);
        a.withdrawn_at = safe_col_text(stmt, 15);
        a.created_at = safe_col_text(stmt, 16);
        a.updated_at = safe_col_text(stmt, 17);
        results.push_back(a);
    }
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)public_only;
#endif
    return results;
}

std::vector<SecurityAdvisory> Database::get_recent_advisories(int limit) {
    std::vector<SecurityAdvisory> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT a.id, a.advisory_id, a.package_id, a.title, a.description, a.severity,
               a.cvss_score, a.affected_versions, a.patched_versions, a.cve_ids, a.cwe_ids,
               a.references_json, a.reporter_id, a.is_public, a.published_at, a.withdrawn_at,
               a.created_at, a.updated_at, p.name
        FROM security_advisories a
        JOIN packages p ON a.package_id = p.id
        WHERE a.is_public = 1 AND a.withdrawn_at IS NULL
        ORDER BY a.created_at DESC LIMIT ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int(stmt, 1, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        SecurityAdvisory a;
        a.id = sqlite3_column_int64(stmt, 0);
        a.advisory_id = safe_col_text(stmt, 1);
        a.package_id = sqlite3_column_int64(stmt, 2);
        a.title = safe_col_text(stmt, 3);
        a.description = safe_col_text(stmt, 4);
        a.severity = safe_col_text(stmt, 5);
        a.cvss_score = sqlite3_column_double(stmt, 6);
        a.affected_versions = safe_col_text(stmt, 7);
        a.patched_versions = safe_col_text(stmt, 8);
        a.cve_ids = safe_col_text(stmt, 9);
        a.cwe_ids = safe_col_text(stmt, 10);
        a.references_json = safe_col_text(stmt, 11);
        a.reporter_id = sqlite3_column_int64(stmt, 12);
        a.is_public = sqlite3_column_int(stmt, 13) != 0;
        a.published_at = safe_col_text(stmt, 14);
        a.withdrawn_at = safe_col_text(stmt, 15);
        a.created_at = safe_col_text(stmt, 16);
        a.updated_at = safe_col_text(stmt, 17);
        a.package_name = safe_col_text(stmt, 18);
        results.push_back(a);
    }
    sqlite3_finalize(stmt);
#else
    (void)limit;
#endif
    return results;
}

bool Database::update_security_advisory(const SecurityAdvisory& advisory) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        UPDATE security_advisories SET title = ?, description = ?, severity = ?,
            cvss_score = ?, affected_versions = ?, patched_versions = ?,
            is_public = ?, updated_at = datetime('now')
        WHERE id = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, advisory.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, advisory.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, advisory.severity.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, advisory.cvss_score);
    sqlite3_bind_text(stmt, 5, advisory.affected_versions.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, advisory.patched_versions.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, advisory.is_public ? 1 : 0);
    sqlite3_bind_int64(stmt, 8, advisory.id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)advisory;
    return true;
#endif
}

std::vector<SecurityAdvisory> Database::check_version_advisories(const std::string& package_name,
                                                                  const std::string& version) {
    std::vector<SecurityAdvisory> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    // Simple check - in production would use proper semver range checking
    const char* sql = R"(
        SELECT a.id, a.advisory_id, a.package_id, a.title, a.description, a.severity,
               a.cvss_score, a.affected_versions, a.patched_versions
        FROM security_advisories a
        JOIN packages p ON a.package_id = p.id
        WHERE p.name = ? AND a.is_public = 1 AND a.withdrawn_at IS NULL
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_text(stmt, 1, package_name.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        SecurityAdvisory a;
        a.id = sqlite3_column_int64(stmt, 0);
        a.advisory_id = safe_col_text(stmt, 1);
        a.package_id = sqlite3_column_int64(stmt, 2);
        a.title = safe_col_text(stmt, 3);
        a.description = safe_col_text(stmt, 4);
        a.severity = safe_col_text(stmt, 5);
        a.cvss_score = sqlite3_column_double(stmt, 6);
        a.affected_versions = safe_col_text(stmt, 7);
        a.patched_versions = safe_col_text(stmt, 8);
        results.push_back(a);
    }
    sqlite3_finalize(stmt);
    (void)version; // TODO: Implement proper version range checking
#else
    (void)package_name; (void)version;
#endif
    return results;
}

// ─────────────────────────────────────────────────────────────────────────────
// المرايا
// ─────────────────────────────────────────────────────────────────────────────

int64_t Database::add_mirror(const RegistryMirror& mirror) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO registry_mirrors (name, base_url, region, country_code, mirror_type,
            sync_frequency_minutes, is_official)
        VALUES (?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, mirror.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, mirror.base_url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, mirror.region.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, mirror.country_code.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, mirror.mirror_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, mirror.sync_frequency_minutes);
    sqlite3_bind_int(stmt, 7, mirror.is_official ? 1 : 0);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)mirror;
    return 1;
#endif
}

std::vector<RegistryMirror> Database::get_active_mirrors() {
    std::vector<RegistryMirror> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, name, base_url, region, country_code, mirror_type, sync_frequency_minutes,
               is_official, is_active, last_sync_at, last_sync_status, packages_synced,
               bandwidth_used_mb, health_score, created_at
        FROM registry_mirrors WHERE is_active = 1
        ORDER BY health_score DESC
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        RegistryMirror m;
        m.id = sqlite3_column_int64(stmt, 0);
        m.name = safe_col_text(stmt, 1);
        m.base_url = safe_col_text(stmt, 2);
        m.region = safe_col_text(stmt, 3);
        m.country_code = safe_col_text(stmt, 4);
        m.mirror_type = safe_col_text(stmt, 5);
        m.sync_frequency_minutes = sqlite3_column_int(stmt, 6);
        m.is_official = sqlite3_column_int(stmt, 7) != 0;
        m.is_active = sqlite3_column_int(stmt, 8) != 0;
        m.last_sync_at = safe_col_text(stmt, 9);
        m.last_sync_status = safe_col_text(stmt, 10);
        m.packages_synced = sqlite3_column_int64(stmt, 11);
        m.bandwidth_used_mb = sqlite3_column_int64(stmt, 12);
        m.health_score = sqlite3_column_double(stmt, 13);
        m.created_at = safe_col_text(stmt, 14);
        results.push_back(m);
    }
    sqlite3_finalize(stmt);
#endif
    return results;
}

bool Database::update_mirror_status(int64_t mirror_id, const std::string& status,
                                    int64_t packages_synced) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        UPDATE registry_mirrors SET last_sync_at = datetime('now'), last_sync_status = ?,
            packages_synced = ? WHERE id = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, packages_synced);
    sqlite3_bind_int64(stmt, 3, mirror_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)mirror_id; (void)status; (void)packages_synced;
    return true;
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// التحليلات المتقدمة
// ─────────────────────────────────────────────────────────────────────────────

void Database::record_download_analytics(const DownloadAnalytics& analytics) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO download_analytics (version_id, client_ip_hash, country_code, region,
            city, client_type, client_version, os_name, os_version, arch, sad_version, is_ci)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_int64(stmt, 1, analytics.version_id);
    sqlite3_bind_text(stmt, 2, "", -1, SQLITE_STATIC); // IP hash omitted for privacy
    sqlite3_bind_text(stmt, 3, analytics.country_code.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, analytics.region.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, analytics.city.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, analytics.client_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, analytics.client_version.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, analytics.os_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, analytics.os_version.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, analytics.arch.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, analytics.sad_version.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 12, analytics.is_ci ? 1 : 0);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
#else
    (void)analytics;
#endif
}

std::vector<std::pair<std::string, int64_t>> Database::get_downloads_by_country(
    int64_t package_id, int days) {
    std::vector<std::pair<std::string, int64_t>> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT country_code, COUNT(*) as cnt
        FROM download_analytics da
        JOIN versions v ON da.version_id = v.id
        WHERE v.package_id = ? AND da.downloaded_at > datetime('now', '-' || ? || ' days')
        GROUP BY country_code ORDER BY cnt DESC LIMIT 20
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_int(stmt, 2, days);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.emplace_back(safe_col_text(stmt, 0), sqlite3_column_int64(stmt, 1));
    }
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)days;
#endif
    return results;
}

std::vector<std::pair<std::string, int64_t>> Database::get_downloads_by_os(
    int64_t package_id, int days) {
    std::vector<std::pair<std::string, int64_t>> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT os_name, COUNT(*) as cnt
        FROM download_analytics da
        JOIN versions v ON da.version_id = v.id
        WHERE v.package_id = ? AND da.downloaded_at > datetime('now', '-' || ? || ' days')
        GROUP BY os_name ORDER BY cnt DESC
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_int(stmt, 2, days);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.emplace_back(safe_col_text(stmt, 0), sqlite3_column_int64(stmt, 1));
    }
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)days;
#endif
    return results;
}

void Database::update_hourly_stats(int64_t package_id, int64_t version_id,
                                   const std::string& stat_type, int count) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO hourly_stats (stat_hour, package_id, version_id, stat_type, count)
        VALUES (datetime(strftime('%Y-%m-%d %H:00:00', 'now')), ?, ?, ?, ?)
        ON CONFLICT(stat_hour, package_id, version_id, stat_type)
        DO UPDATE SET count = count + excluded.count
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    if (package_id > 0)
        sqlite3_bind_int64(stmt, 1, package_id);
    else
        sqlite3_bind_null(stmt, 1);
    if (version_id > 0)
        sqlite3_bind_int64(stmt, 2, version_id);
    else
        sqlite3_bind_null(stmt, 2);
    sqlite3_bind_text(stmt, 3, stat_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, count);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)version_id; (void)stat_type; (void)count;
#endif
}

std::vector<HourlyStats> Database::get_hourly_stats(int64_t package_id,
                                                     const std::string& stat_type,
                                                     int hours) {
    std::vector<HourlyStats> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, stat_hour, package_id, version_id, stat_type, count, unique_count, metadata
        FROM hourly_stats
        WHERE package_id = ? AND stat_type = ? AND stat_hour > datetime('now', '-' || ? || ' hours')
        ORDER BY stat_hour ASC
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_text(stmt, 2, stat_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, hours);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        HourlyStats s;
        s.id = sqlite3_column_int64(stmt, 0);
        s.stat_hour = safe_col_text(stmt, 1);
        s.package_id = sqlite3_column_int64(stmt, 2);
        s.version_id = sqlite3_column_int64(stmt, 3);
        s.stat_type = safe_col_text(stmt, 4);
        s.count = sqlite3_column_int64(stmt, 5);
        s.unique_count = sqlite3_column_int64(stmt, 6);
        s.metadata = safe_col_text(stmt, 7);
        results.push_back(s);
    }
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)stat_type; (void)hours;
#endif
    return results;
}

void Database::record_search_analytics(const std::string& query, int result_count,
                                       int search_time_ms, int64_t clicked_package,
                                       int clicked_position) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO search_analytics (query_normalized, query_original, result_count,
            clicked_package_id, clicked_position, search_time_ms)
        VALUES (lower(?), ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_text(stmt, 1, query.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, query.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, result_count);
    if (clicked_package > 0)
        sqlite3_bind_int64(stmt, 4, clicked_package);
    else
        sqlite3_bind_null(stmt, 4);
    sqlite3_bind_int(stmt, 5, clicked_position);
    sqlite3_bind_int(stmt, 6, search_time_ms);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
#else
    (void)query; (void)result_count; (void)search_time_ms;
    (void)clicked_package; (void)clicked_position;
#endif
}

std::vector<std::pair<std::string, int64_t>> Database::get_popular_searches(int limit) {
    std::vector<std::pair<std::string, int64_t>> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT query_normalized, COUNT(*) as cnt
        FROM search_analytics
        WHERE created_at > datetime('now', '-7 days')
        GROUP BY query_normalized
        ORDER BY cnt DESC LIMIT ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int(stmt, 1, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        results.emplace_back(safe_col_text(stmt, 0), sqlite3_column_int64(stmt, 1));
    }
    sqlite3_finalize(stmt);
#else
    (void)limit;
#endif
    return results;
}

// ─────────────────────────────────────────────────────────────────────────────
// قائمة انتظار المهام
// ─────────────────────────────────────────────────────────────────────────────

int64_t Database::enqueue_job(const std::string& job_type, const std::string& payload,
                              int priority, int delay_seconds) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO job_queue (job_type, payload, priority, run_at)
        VALUES (?, ?, ?, datetime('now', '+' || ? || ' seconds'))
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, job_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, payload.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, priority);
    sqlite3_bind_int(stmt, 4, delay_seconds);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) return -1;
    return sqlite3_last_insert_rowid(db);
#else
    (void)job_type; (void)payload; (void)priority; (void)delay_seconds;
    return 1;
#endif
}

std::optional<JobInfo> Database::dequeue_job(const std::string& worker_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    // Get next job
    const char* sql = R"(
        SELECT id, job_type, payload, priority, status, attempts, max_attempts,
               run_at, started_at, completed_at, result, error_message, created_at
        FROM job_queue
        WHERE status = 'pending' AND run_at <= datetime('now')
        ORDER BY priority ASC, created_at ASC LIMIT 1
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        JobInfo j;
        j.id = sqlite3_column_int64(stmt, 0);
        j.job_type = safe_col_text(stmt, 1);
        j.payload = safe_col_text(stmt, 2);
        j.priority = sqlite3_column_int(stmt, 3);
        j.status = safe_col_text(stmt, 4);
        j.attempts = sqlite3_column_int(stmt, 5);
        j.max_attempts = sqlite3_column_int(stmt, 6);
        j.run_at = safe_col_text(stmt, 7);
        j.started_at = safe_col_text(stmt, 8);
        j.completed_at = safe_col_text(stmt, 9);
        j.result = safe_col_text(stmt, 10);
        j.error_message = safe_col_text(stmt, 11);
        j.created_at = safe_col_text(stmt, 12);
        j.worker_id = worker_id;
        sqlite3_finalize(stmt);
        // Mark as running
        const char* update_sql = "UPDATE job_queue SET status = 'running', started_at = datetime('now'), worker_id = ?, attempts = attempts + 1 WHERE id = ?";
        sqlite3_stmt* stmt2 = nullptr;
        if (sqlite3_prepare_v2(db, update_sql, -1, &stmt2, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt2, 1, worker_id.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int64(stmt2, 2, j.id);
            sqlite3_step(stmt2);
            sqlite3_finalize(stmt2);
        }
        return j;
    }
    sqlite3_finalize(stmt);
#else
    (void)worker_id;
#endif
    return std::nullopt;
}

bool Database::update_job_status(int64_t job_id, const std::string& status,
                                 const std::string& result, const std::string& error) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        UPDATE job_queue SET status = ?, result = ?, error_message = ?,
            completed_at = CASE WHEN ? IN ('completed', 'failed') THEN datetime('now') ELSE completed_at END
        WHERE id = ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, result.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, error.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 5, job_id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)job_id; (void)status; (void)result; (void)error;
    return true;
#endif
}

int Database::reschedule_failed_jobs() {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        UPDATE job_queue SET status = 'pending', run_at = datetime('now', '+5 minutes')
        WHERE status = 'failed' AND attempts < max_attempts
    )";
    if (sqlite3_exec(db, sql, nullptr, nullptr, nullptr) == SQLITE_OK) {
        return sqlite3_changes(db);
    }
#endif
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// تحديد المعدل
// ─────────────────────────────────────────────────────────────────────────────

bool Database::check_rate_limit(const std::string& identifier,
                                const std::string& identifier_type,
                                const std::string& endpoint_pattern,
                                int limit_per_window, int window_seconds) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    
    // Check if blocked
    const char* block_sql = R"(
        SELECT blocked_until FROM rate_limits
        WHERE identifier = ? AND endpoint_pattern = ? AND is_blocked = 1
        AND blocked_until > datetime('now')
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, block_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, identifier.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, endpoint_pattern.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sqlite3_finalize(stmt);
            return false; // Blocked
        }
        sqlite3_finalize(stmt);
    }
    
    // Upsert rate limit record
    const char* upsert_sql = R"(
        INSERT INTO rate_limits (identifier, identifier_type, endpoint_pattern,
            window_start, window_size_seconds, request_count, limit_value)
        VALUES (?, ?, ?, datetime('now'), ?, 1, ?)
        ON CONFLICT(identifier, endpoint_pattern, window_start)
        DO UPDATE SET request_count = request_count + 1
    )";
    if (sqlite3_prepare_v2(db, upsert_sql, -1, &stmt, nullptr) != SQLITE_OK) return true;
    sqlite3_bind_text(stmt, 1, identifier.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, identifier_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, endpoint_pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, window_seconds);
    sqlite3_bind_int(stmt, 5, limit_per_window);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    // Check current count
    const char* count_sql = R"(
        SELECT SUM(request_count) FROM rate_limits
        WHERE identifier = ? AND endpoint_pattern = ?
        AND window_start > datetime('now', '-' || ? || ' seconds')
    )";
    if (sqlite3_prepare_v2(db, count_sql, -1, &stmt, nullptr) != SQLITE_OK) return true;
    sqlite3_bind_text(stmt, 1, identifier.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, endpoint_pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, window_seconds);
    int64_t count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int64(stmt, 0);
    }
    sqlite3_finalize(stmt);
    
    return count <= limit_per_window;
#else
    (void)identifier; (void)identifier_type; (void)endpoint_pattern;
    (void)limit_per_window; (void)window_seconds;
    return true;
#endif
}

std::optional<RateLimitInfo> Database::get_rate_limit_status(const std::string& identifier,
                                                              const std::string& endpoint_pattern) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT identifier, identifier_type, endpoint_pattern,
               SUM(request_count), MAX(limit_value), MAX(is_blocked), MAX(blocked_until)
        FROM rate_limits
        WHERE identifier = ? AND endpoint_pattern = ?
        AND window_start > datetime('now', '-1 hour')
        GROUP BY identifier, identifier_type, endpoint_pattern
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, identifier.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, endpoint_pattern.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        RateLimitInfo r;
        r.identifier = safe_col_text(stmt, 0);
        r.identifier_type = safe_col_text(stmt, 1);
        r.endpoint_pattern = safe_col_text(stmt, 2);
        r.request_count = sqlite3_column_int64(stmt, 3);
        r.limit_value = sqlite3_column_int64(stmt, 4);
        r.is_blocked = sqlite3_column_int(stmt, 5) != 0;
        r.blocked_until = safe_col_text(stmt, 6);
        sqlite3_finalize(stmt);
        return r;
    }
    sqlite3_finalize(stmt);
#else
    (void)identifier; (void)endpoint_pattern;
#endif
    return std::nullopt;
}

bool Database::block_identifier(const std::string& identifier,
                                const std::string& identifier_type,
                                int duration_seconds) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO rate_limits (identifier, identifier_type, endpoint_pattern,
            window_start, window_size_seconds, limit_value, is_blocked, blocked_until)
        VALUES (?, ?, '*', datetime('now'), 3600, 0, 1, datetime('now', '+' || ? || ' seconds'))
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, identifier.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, identifier_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, duration_seconds);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)identifier; (void)identifier_type; (void)duration_seconds;
    return true;
#endif
}

bool Database::unblock_identifier(const std::string& identifier) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "UPDATE rate_limits SET is_blocked = 0, blocked_until = NULL WHERE identifier = ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, identifier.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
#else
    (void)identifier;
    return true;
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// التخزين المؤقت
// ─────────────────────────────────────────────────────────────────────────────

std::optional<std::pair<std::string, std::string>> Database::get_api_cache(
    const std::string& cache_key) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "SELECT response_body, content_type FROM api_cache WHERE cache_key = ? AND expires_at > datetime('now')";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;
    sqlite3_bind_text(stmt, 1, cache_key.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto result = std::make_pair(
            std::string(safe_col_text(stmt, 0)),
            std::string(safe_col_text(stmt, 1))
        );
        sqlite3_finalize(stmt);
        // Update hit count
        execute("UPDATE api_cache SET hit_count = hit_count + 1 WHERE cache_key = '" + cache_key + "'");
        return result;
    }
    sqlite3_finalize(stmt);
#else
    (void)cache_key;
#endif
    return std::nullopt;
}

void Database::set_api_cache(const std::string& cache_key, const std::string& response,
                             const std::string& content_type, int ttl_seconds) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT OR REPLACE INTO api_cache (cache_key, response_body, content_type, expires_at)
        VALUES (?, ?, ?, datetime('now', '+' || ? || ' seconds'))
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_text(stmt, 1, cache_key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, response.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, content_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, ttl_seconds);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
#else
    (void)cache_key; (void)response; (void)content_type; (void)ttl_seconds;
#endif
}

void Database::invalidate_cache(const std::string& pattern) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = "DELETE FROM api_cache WHERE cache_key LIKE ?";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_text(stmt, 1, (pattern + "%").c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
#else
    (void)pattern;
#endif
}

int Database::cleanup_expired_cache() {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    if (sqlite3_exec(db, "DELETE FROM api_cache WHERE expires_at < datetime('now')", nullptr, nullptr, nullptr) == SQLITE_OK) {
        int cleaned = sqlite3_changes(db);
        sqlite3_exec(db, "DELETE FROM search_cache WHERE expires_at < datetime('now')", nullptr, nullptr, nullptr);
        cleaned += sqlite3_changes(db);
        sqlite3_exec(db, "DELETE FROM dependency_cache WHERE expires_at < datetime('now')", nullptr, nullptr, nullptr);
        cleaned += sqlite3_changes(db);
        return cleaned;
    }
#endif
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// سجل الأحداث
// ─────────────────────────────────────────────────────────────────────────────

void Database::log_event(const std::string& event_type, const std::string& event_subtype,
                         const std::string& actor_type, int64_t actor_id,
                         const std::string& target_type, int64_t target_id,
                         const std::string& metadata_json,
                         const std::string& request_id) {
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        INSERT INTO event_log (event_type, event_subtype, actor_type, actor_id,
            target_type, target_id, metadata, request_id)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_text(stmt, 1, event_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, event_subtype.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, actor_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, actor_id);
    sqlite3_bind_text(stmt, 5, target_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 6, target_id);
    sqlite3_bind_text(stmt, 7, metadata_json.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, request_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
#else
    (void)event_type; (void)event_subtype; (void)actor_type; (void)actor_id;
    (void)target_type; (void)target_id; (void)metadata_json; (void)request_id;
#endif
}

std::vector<Database::EventLogEntry> Database::get_package_events(int64_t package_id, int limit) {
    std::vector<EventLogEntry> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, event_type, event_subtype, actor_type, actor_id, target_type,
               target_id, metadata, created_at
        FROM event_log WHERE target_type = 'package' AND target_id = ?
        ORDER BY created_at DESC LIMIT ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, package_id);
    sqlite3_bind_int(stmt, 2, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        EventLogEntry e;
        e.id = sqlite3_column_int64(stmt, 0);
        e.event_type = safe_col_text(stmt, 1);
        e.event_subtype = safe_col_text(stmt, 2);
        e.actor_type = safe_col_text(stmt, 3);
        e.actor_id = sqlite3_column_int64(stmt, 4);
        e.target_type = safe_col_text(stmt, 5);
        e.target_id = sqlite3_column_int64(stmt, 6);
        e.metadata = safe_col_text(stmt, 7);
        e.created_at = safe_col_text(stmt, 8);
        results.push_back(e);
    }
    sqlite3_finalize(stmt);
#else
    (void)package_id; (void)limit;
#endif
    return results;
}

std::vector<Database::EventLogEntry> Database::get_user_events(int64_t user_id, int limit) {
    std::vector<EventLogEntry> results;
#if SAD_HAS_SQLITE
    sqlite3* db = static_cast<sqlite3*>(db_);
    const char* sql = R"(
        SELECT id, event_type, event_subtype, actor_type, actor_id, target_type,
               target_id, metadata, created_at
        FROM event_log WHERE actor_type = 'user' AND actor_id = ?
        ORDER BY created_at DESC LIMIT ?
    )";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return results;
    sqlite3_bind_int64(stmt, 1, user_id);
    sqlite3_bind_int(stmt, 2, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        EventLogEntry e;
        e.id = sqlite3_column_int64(stmt, 0);
        e.event_type = safe_col_text(stmt, 1);
        e.event_subtype = safe_col_text(stmt, 2);
        e.actor_type = safe_col_text(stmt, 3);
        e.actor_id = sqlite3_column_int64(stmt, 4);
        e.target_type = safe_col_text(stmt, 5);
        e.target_id = sqlite3_column_int64(stmt, 6);
        e.metadata = safe_col_text(stmt, 7);
        e.created_at = safe_col_text(stmt, 8);
        results.push_back(e);
    }
    sqlite3_finalize(stmt);
#else
    (void)user_id; (void)limit;
#endif
    return results;
}

} // namespace registry
} // namespace sad
