// بسم الله الرحمن الرحيم
// =========================================================================
// قاعدة البيانات — إعداد SQLite وإنشاء الجداول
// Database — SQLite setup and schema initialization
// =========================================================================

const { DatabaseSync } = require('node:sqlite');
const path = require('path');
const fs = require('fs');

const DB_PATH = process.env.DB_PATH || './data/registry.db';

/**
 * إنشاء وتهيئة قاعدة البيانات
 */
function initializeDatabase() {
    const dbDir = path.dirname(DB_PATH);
    if (!fs.existsSync(dbDir)) {
        fs.mkdirSync(dbDir, { recursive: true });
    }

    const db = new DatabaseSync(DB_PATH);

    // تحسينات الأداء
    db.exec('PRAGMA journal_mode = WAL');
    db.exec('PRAGMA synchronous = NORMAL');
    db.exec('PRAGMA foreign_keys = ON');
    db.exec('PRAGMA cache_size = -64000'); // 64MB cache

    // إنشاء الجداول
    createTables(db);

    // إنشاء الفهارس
    createIndexes(db);

    // بذر البيانات الأولية
    seedDefaults(db);

    return db;
}

function createTables(db) {
    db.exec(`
        -- ═══════════════════════════════════════════════════════════
        -- جدول المستخدمين
        -- ═══════════════════════════════════════════════════════════
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            email TEXT NOT NULL UNIQUE,
            password_hash TEXT NOT NULL,
            display_name TEXT DEFAULT '',
            bio TEXT DEFAULT '',
            avatar_url TEXT DEFAULT '',
            api_token TEXT UNIQUE,
            is_active INTEGER DEFAULT 1,
            is_admin INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT (datetime('now')),
            updated_at DATETIME DEFAULT (datetime('now'))
        );

        -- ═══════════════════════════════════════════════════════════
        -- جدول الحزم
        -- ═══════════════════════════════════════════════════════════
        CREATE TABLE IF NOT EXISTS packages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            description TEXT DEFAULT '',
            description_ar TEXT DEFAULT '',
            homepage TEXT DEFAULT '',
            repository TEXT DEFAULT '',
            documentation TEXT DEFAULT '',
            license TEXT DEFAULT 'MIT',
            keywords TEXT DEFAULT '[]',
            categories TEXT DEFAULT '[]',
            owner_id INTEGER NOT NULL,
            latest_version TEXT DEFAULT '',
            total_downloads INTEGER DEFAULT 0,
            is_yanked INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT (datetime('now')),
            updated_at DATETIME DEFAULT (datetime('now')),
            FOREIGN KEY (owner_id) REFERENCES users(id)
        );

        -- ═══════════════════════════════════════════════════════════
        -- جدول الإصدارات
        -- ═══════════════════════════════════════════════════════════
        CREATE TABLE IF NOT EXISTS versions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            package_id INTEGER NOT NULL,
            version TEXT NOT NULL,
            major INTEGER DEFAULT 0,
            minor INTEGER DEFAULT 0,
            patch INTEGER DEFAULT 0,
            prerelease TEXT DEFAULT '',
            description TEXT DEFAULT '',
            dependencies TEXT DEFAULT '{}',
            dev_dependencies TEXT DEFAULT '{}',
            build_dependencies TEXT DEFAULT '{}',
            authors TEXT DEFAULT '[]',
            license TEXT DEFAULT 'MIT',
            checksum_sha256 TEXT DEFAULT '',
            archive_path TEXT DEFAULT '',
            archive_size INTEGER DEFAULT 0,
            downloads INTEGER DEFAULT 0,
            is_yanked INTEGER DEFAULT 0,
            published_by INTEGER,
            created_at DATETIME DEFAULT (datetime('now')),
            UNIQUE(package_id, version),
            FOREIGN KEY (package_id) REFERENCES packages(id),
            FOREIGN KEY (published_by) REFERENCES users(id)
        );

        -- ═══════════════════════════════════════════════════════════
        -- جدول مالكي الحزم
        -- ═══════════════════════════════════════════════════════════
        CREATE TABLE IF NOT EXISTS package_owners (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            package_id INTEGER NOT NULL,
            user_id INTEGER NOT NULL,
            role TEXT DEFAULT 'owner',
            created_at DATETIME DEFAULT (datetime('now')),
            UNIQUE(package_id, user_id),
            FOREIGN KEY (package_id) REFERENCES packages(id),
            FOREIGN KEY (user_id) REFERENCES users(id)
        );

        -- ═══════════════════════════════════════════════════════════
        -- جدول رموز API
        -- ═══════════════════════════════════════════════════════════
        CREATE TABLE IF NOT EXISTS api_tokens (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            name TEXT NOT NULL,
            token_hash TEXT NOT NULL UNIQUE,
            scopes TEXT DEFAULT '["publish"]',
            last_used_at DATETIME,
            expires_at DATETIME,
            is_active INTEGER DEFAULT 1,
            created_at DATETIME DEFAULT (datetime('now')),
            FOREIGN KEY (user_id) REFERENCES users(id)
        );

        -- ═══════════════════════════════════════════════════════════
        -- سجل التنزيلات
        -- ═══════════════════════════════════════════════════════════
        CREATE TABLE IF NOT EXISTS download_log (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            version_id INTEGER NOT NULL,
            ip_address TEXT DEFAULT '',
            user_agent TEXT DEFAULT '',
            downloaded_at DATETIME DEFAULT (datetime('now')),
            FOREIGN KEY (version_id) REFERENCES versions(id)
        );

        -- ═══════════════════════════════════════════════════════════
        -- جدول التصنيفات
        -- ═══════════════════════════════════════════════════════════
        CREATE TABLE IF NOT EXISTS categories (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            slug TEXT NOT NULL UNIQUE,
            name_ar TEXT NOT NULL,
            name_en TEXT NOT NULL,
            description_ar TEXT DEFAULT '',
            description_en TEXT DEFAULT '',
            icon TEXT DEFAULT '',
            package_count INTEGER DEFAULT 0
        );
    `);
}

function createIndexes(db) {
    db.exec(`
        CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);
        CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
        CREATE INDEX IF NOT EXISTS idx_users_api_token ON users(api_token);
        CREATE INDEX IF NOT EXISTS idx_packages_name ON packages(name);
        CREATE INDEX IF NOT EXISTS idx_packages_owner ON packages(owner_id);
        CREATE INDEX IF NOT EXISTS idx_packages_downloads ON packages(total_downloads DESC);
        CREATE INDEX IF NOT EXISTS idx_packages_created ON packages(created_at DESC);
        CREATE INDEX IF NOT EXISTS idx_versions_package ON versions(package_id);
        CREATE INDEX IF NOT EXISTS idx_versions_semver ON versions(package_id, major DESC, minor DESC, patch DESC);
        CREATE INDEX IF NOT EXISTS idx_api_tokens_hash ON api_tokens(token_hash);
        CREATE INDEX IF NOT EXISTS idx_api_tokens_user ON api_tokens(user_id);
        CREATE INDEX IF NOT EXISTS idx_download_log_version ON download_log(version_id);
        CREATE INDEX IF NOT EXISTS idx_download_log_date ON download_log(downloaded_at);
        CREATE INDEX IF NOT EXISTS idx_package_owners_pkg ON package_owners(package_id);
        CREATE INDEX IF NOT EXISTS idx_package_owners_user ON package_owners(user_id);
    `);
}

function seedDefaults(db) {
    const count = db.prepare('SELECT COUNT(*) as c FROM categories').get().c;
    if (count > 0) return;

    const categories = [
        ['math', 'رياضيات', 'Mathematics', 'مكتبات حسابية ورياضية', 'Math and computation libraries', '🔢'],
        ['text', 'نصوص', 'Text Processing', 'معالجة وتحليل النصوص', 'Text processing and analysis', '📝'],
        ['web', 'ويب', 'Web', 'تطوير الويب والخوادم', 'Web development and servers', '🌐'],
        ['network', 'شبكات', 'Networking', 'بروتوكولات وشبكات', 'Networking and protocols', '📡'],
        ['database', 'قواعد بيانات', 'Database', 'قواعد البيانات والتخزين', 'Database and storage', '🗄️'],
        ['crypto', 'تشفير', 'Cryptography', 'تشفير وأمان', 'Encryption and security', '🔐'],
        ['ui', 'واجهات', 'UI', 'واجهات المستخدم', 'User interfaces', '🖼️'],
        ['game', 'ألعاب', 'Games', 'تطوير الألعاب', 'Game development', '🎮'],
        ['data', 'بيانات', 'Data', 'هياكل البيانات والخوارزميات', 'Data structures and algorithms', '📊'],
        ['io', 'إدخال/إخراج', 'I/O', 'عمليات الملفات والإدخال/الإخراج', 'File and I/O operations', '📁'],
        ['date', 'تاريخ', 'Date/Time', 'التاريخ والوقت', 'Date and time handling', '📅'],
        ['i18n', 'تدويل', 'Internationalization', 'التدويل والترجمة', 'Internationalization', '🌍'],
        ['islamic', 'إسلامي', 'Islamic', 'أدوات إسلامية (هجري، قرآن...)', 'Islamic tools (Hijri, Quran...)', '🕌'],
        ['finance', 'مالية', 'Finance', 'حسابات مالية', 'Financial computations', '💰'],
        ['testing', 'اختبارات', 'Testing', 'أدوات الاختبار', 'Testing tools', '🧪'],
        ['cli', 'سطر أوامر', 'CLI', 'أدوات سطر الأوامر', 'Command-line tools', '⌨️'],
    ];

    const insert = db.prepare(`
        INSERT INTO categories (slug, name_ar, name_en, description_ar, description_en, icon)
        VALUES (?, ?, ?, ?, ?, ?)
    `);

    db.exec('BEGIN');
    try {
        for (const cat of categories) {
            insert.run(...cat);
        }
        db.exec('COMMIT');
    } catch (err) {
        db.exec('ROLLBACK');
        throw err;
    }
}

module.exports = { initializeDatabase, DB_PATH };
