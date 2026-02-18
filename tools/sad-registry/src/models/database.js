// بسم الله الرحمن الرحيم
/**
 * @file database.js
 * @description قاعدة بيانات SQLite لمستودع حزم لغة ص
 * 
 * تستخدم better-sqlite3 لأداء عالٍ ومتزامن
 * Uses better-sqlite3 for high-performance synchronous access
 */

const Database = require('better-sqlite3');
const path = require('path');
const fs = require('fs');

let db = null;

/**
 * تهيئة قاعدة البيانات / Initialize Database
 */
function initDatabase(dbPath) {
    const resolvedPath = dbPath || process.env.DB_PATH || './data/registry.db';
    
    // إنشاء المجلد إذا لم يكن موجوداً
    const dir = path.dirname(resolvedPath);
    if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
    }

    db = new Database(resolvedPath);

    // تحسينات الأداء / Performance optimizations
    db.pragma('journal_mode = WAL');
    db.pragma('foreign_keys = ON');
    db.pragma('cache_size = -64000'); // 64MB cache

    // إنشاء الجداول / Create tables
    createTables();

    return db;
}

/**
 * الحصول على قاعدة البيانات / Get Database
 */
function getDatabase() {
    if (!db) {
        throw new Error('قاعدة البيانات غير مهيأة / Database not initialized');
    }
    return db;
}

/**
 * إنشاء الجداول / Create Tables
 */
function createTables() {
    db.exec(`
        -- ================================================================
        -- جدول المستخدمين / Users Table
        -- ================================================================
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            display_name TEXT,
            bio TEXT,
            avatar_url TEXT,
            api_token TEXT UNIQUE,
            is_admin INTEGER DEFAULT 0,
            is_active INTEGER DEFAULT 1,
            created_at TEXT DEFAULT (datetime('now')),
            updated_at TEXT DEFAULT (datetime('now'))
        );

        -- ================================================================
        -- جدول الحزم / Packages Table
        -- ================================================================
        CREATE TABLE IF NOT EXISTS packages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            description TEXT DEFAULT '',
            description_ar TEXT DEFAULT '',
            homepage TEXT DEFAULT '',
            repository TEXT DEFAULT '',
            documentation TEXT DEFAULT '',
            license TEXT DEFAULT 'MIT',
            keywords TEXT DEFAULT '[]',
            categories TEXT DEFAULT '[]',
            owner_id INTEGER NOT NULL,
            latest_version TEXT,
            total_downloads INTEGER DEFAULT 0,
            is_yanked INTEGER DEFAULT 0,
            created_at TEXT DEFAULT (datetime('now')),
            updated_at TEXT DEFAULT (datetime('now')),
            FOREIGN KEY (owner_id) REFERENCES users(id)
        );

        -- ================================================================
        -- جدول الإصدارات / Versions Table
        -- ================================================================
        CREATE TABLE IF NOT EXISTS versions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            package_id INTEGER NOT NULL,
            version TEXT NOT NULL,
            major INTEGER NOT NULL DEFAULT 0,
            minor INTEGER NOT NULL DEFAULT 0,
            patch INTEGER NOT NULL DEFAULT 0,
            prerelease TEXT DEFAULT '',
            description TEXT DEFAULT '',
            dependencies TEXT DEFAULT '{}',
            dev_dependencies TEXT DEFAULT '{}',
            build_dependencies TEXT DEFAULT '{}',
            authors TEXT DEFAULT '[]',
            license TEXT DEFAULT 'MIT',
            readme TEXT DEFAULT '',
            checksum_sha256 TEXT,
            archive_path TEXT,
            archive_size INTEGER DEFAULT 0,
            downloads INTEGER DEFAULT 0,
            is_yanked INTEGER DEFAULT 0,
            published_by INTEGER,
            created_at TEXT DEFAULT (datetime('now')),
            FOREIGN KEY (package_id) REFERENCES packages(id),
            FOREIGN KEY (published_by) REFERENCES users(id),
            UNIQUE(package_id, version)
        );

        -- ================================================================
        -- جدول المالكين / Owners Table (حزمة يمكن أن يملكها عدة مستخدمين)
        -- ================================================================
        CREATE TABLE IF NOT EXISTS package_owners (
            package_id INTEGER NOT NULL,
            user_id INTEGER NOT NULL,
            role TEXT DEFAULT 'maintainer',
            added_at TEXT DEFAULT (datetime('now')),
            PRIMARY KEY (package_id, user_id),
            FOREIGN KEY (package_id) REFERENCES packages(id),
            FOREIGN KEY (user_id) REFERENCES users(id)
        );

        -- ================================================================
        -- جدول رموز API / API Tokens Table
        -- ================================================================
        CREATE TABLE IF NOT EXISTS api_tokens (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            name TEXT NOT NULL,
            token_hash TEXT UNIQUE NOT NULL,
            scopes TEXT DEFAULT '["publish"]',
            last_used_at TEXT,
            expires_at TEXT,
            created_at TEXT DEFAULT (datetime('now')),
            FOREIGN KEY (user_id) REFERENCES users(id)
        );

        -- ================================================================
        -- جدول سجل التنزيلات / Download Log Table
        -- ================================================================
        CREATE TABLE IF NOT EXISTS download_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            version_id INTEGER NOT NULL,
            ip_address TEXT,
            user_agent TEXT,
            downloaded_at TEXT DEFAULT (datetime('now')),
            FOREIGN KEY (version_id) REFERENCES versions(id)
        );

        -- ================================================================
        -- جدول التصنيفات / Categories Table
        -- ================================================================
        CREATE TABLE IF NOT EXISTS categories (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            slug TEXT UNIQUE NOT NULL,
            name_en TEXT NOT NULL,
            name_ar TEXT NOT NULL,
            description_en TEXT DEFAULT '',
            description_ar TEXT DEFAULT '',
            icon TEXT DEFAULT '📦'
        );

        -- ================================================================
        -- الفهارس / Indexes
        -- ================================================================
        CREATE INDEX IF NOT EXISTS idx_packages_name ON packages(name);
        CREATE INDEX IF NOT EXISTS idx_packages_owner ON packages(owner_id);
        CREATE INDEX IF NOT EXISTS idx_packages_downloads ON packages(total_downloads DESC);
        CREATE INDEX IF NOT EXISTS idx_versions_package ON versions(package_id);
        CREATE INDEX IF NOT EXISTS idx_versions_semver ON versions(major, minor, patch);
        CREATE INDEX IF NOT EXISTS idx_download_log_version ON download_log(version_id);
        CREATE INDEX IF NOT EXISTS idx_download_log_date ON download_log(downloaded_at);
        CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);
        CREATE INDEX IF NOT EXISTS idx_users_token ON users(api_token);
    `);

    // إدخال التصنيفات الافتراضية / Insert default categories
    const insertCategory = db.prepare(`
        INSERT OR IGNORE INTO categories (slug, name_en, name_ar, description_en, description_ar, icon)
        VALUES (?, ?, ?, ?, ?, ?)
    `);

    const defaultCategories = [
        ['math', 'Mathematics', 'رياضيات', 'Math and numerical computation', 'حسابات رياضية وعددية', '🔢'],
        ['text', 'Text Processing', 'معالجة نصوص', 'String manipulation and text processing', 'معالجة النصوص والسلاسل', '📝'],
        ['web', 'Web', 'ويب', 'Web frameworks and HTTP', 'أطر ويب وHTTP', '🌐'],
        ['network', 'Networking', 'شبكات', 'Network protocols and tools', 'بروتوكولات وأدوات شبكات', '🔌'],
        ['database', 'Database', 'قواعد بيانات', 'Database drivers and ORMs', 'محركات قواعد بيانات', '🗃️'],
        ['crypto', 'Cryptography', 'تشفير', 'Encryption and security', 'تشفير وأمان', '🔐'],
        ['ui', 'User Interface', 'واجهات', 'GUI and UI frameworks', 'أطر واجهات المستخدم', '🎨'],
        ['game', 'Game Development', 'ألعاب', 'Game engines and tools', 'محركات وأدوات ألعاب', '🎮'],
        ['data', 'Data Structures', 'هياكل بيانات', 'Collections and data structures', 'مجموعات وهياكل بيانات', '📊'],
        ['io', 'File I/O', 'ملفات', 'File system and I/O', 'نظام ملفات ومدخلات/مخرجات', '📁'],
        ['date', 'Date & Time', 'تاريخ ووقت', 'Date, time, calendars (Hijri)', 'تاريخ ووقت وتقاويم (هجري)', '📅'],
        ['i18n', 'Internationalization', 'تدويل', 'i18n and Arabic text support', 'تدويل ودعم النص العربي', '🌍'],
        ['islamic', 'Islamic', 'إسلامي', 'Islamic utilities (prayer, Quran, Hijri)', 'أدوات إسلامية (صلاة، قرآن، هجري)', '🕌'],
        ['finance', 'Finance', 'مالي', 'Finance and currency tools', 'أدوات مالية وعملات', '💰'],
        ['testing', 'Testing', 'اختبار', 'Testing frameworks and tools', 'أطر وأدوات اختبار', '🧪'],
        ['cli', 'CLI', 'سطر أوامر', 'Command-line tools', 'أدوات سطر الأوامر', '⌨️'],
    ];

    const insertMany = db.transaction((categories) => {
        for (const cat of categories) {
            insertCategory.run(...cat);
        }
    });
    insertMany(defaultCategories);
}

module.exports = { initDatabase, getDatabase };
