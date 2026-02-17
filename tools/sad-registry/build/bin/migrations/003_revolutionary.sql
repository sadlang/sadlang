-- بسم الله الرحمن الرحيم
-- ═══════════════════════════════════════════════════════════════════════════════
-- الهجرة ٠٠٣: التحسينات الثورية
-- ═══════════════════════════════════════════════════════════════════════════════
-- تشمل:
--   ● نظام التحقق والتوقيعات الرقمية
--   ● حل التبعيات المتقدم
--   ● التخزين المؤقت والأداء
--   ● تحليلات متقدمة
--   ● نظام الإشعارات
--   ● دعم المستودعات الخاصة
--   ● نظام الترجمة التلقائية
-- ═══════════════════════════════════════════════════════════════════════════════

-- ═══════════════════════════════════════════════════════════════════════════════
-- ١. نظام التحقق والتوقيعات الرقمية
-- ═══════════════════════════════════════════════════════════════════════════════

-- المفاتيح العامة للناشرين
CREATE TABLE IF NOT EXISTS publisher_keys (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    key_type TEXT NOT NULL DEFAULT 'ed25519', -- ed25519, rsa4096
    public_key TEXT NOT NULL,
    fingerprint TEXT NOT NULL UNIQUE,
    key_name TEXT DEFAULT 'default',
    is_primary BOOLEAN DEFAULT 0,
    is_revoked BOOLEAN DEFAULT 0,
    revoked_at DATETIME,
    revoke_reason TEXT,
    expires_at DATETIME,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- توقيعات الإصدارات
CREATE TABLE IF NOT EXISTS version_signatures (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    version_id INTEGER NOT NULL,
    key_id INTEGER NOT NULL,
    signature TEXT NOT NULL,
    signature_type TEXT DEFAULT 'detached', -- detached, inline
    signed_content_hash TEXT NOT NULL, -- sha256 of tarball
    timestamp_authority TEXT, -- optional TSA URL
    timestamp_signature TEXT,
    is_valid BOOLEAN DEFAULT 1,
    verified_at DATETIME,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (version_id) REFERENCES versions(id) ON DELETE CASCADE,
    FOREIGN KEY (key_id) REFERENCES publisher_keys(id)
);

-- سجل التحقق (للتدقيق الأمني)
CREATE TABLE IF NOT EXISTS verification_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    version_id INTEGER NOT NULL,
    verifier_ip TEXT,
    verification_status TEXT NOT NULL, -- valid, invalid, expired_key, revoked_key
    error_message TEXT,
    client_version TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (version_id) REFERENCES versions(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_verification_log_version ON verification_log(version_id);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ٢. نظام حل التبعيات المتقدم
-- ═══════════════════════════════════════════════════════════════════════════════

-- التبعيات مع معلومات موسعة
CREATE TABLE IF NOT EXISTS dependencies_v2 (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    version_id INTEGER NOT NULL,
    dependency_name TEXT NOT NULL,
    version_constraint TEXT NOT NULL, -- >=1.0.0, ^2.0.0, ~1.2
    constraint_type TEXT DEFAULT 'caret', -- exact, caret, tilde, range, any
    dependency_type TEXT DEFAULT 'runtime', -- runtime, dev, build, optional, peer
    is_optional BOOLEAN DEFAULT 0,
    features TEXT, -- JSON array of required features
    platform_filter TEXT, -- windows, linux, macos, all
    registry_url TEXT, -- for external registries
    resolved_version TEXT, -- last successfully resolved version
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (version_id) REFERENCES versions(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_deps_v2_name ON dependencies_v2(dependency_name);
CREATE INDEX IF NOT EXISTS idx_deps_v2_version ON dependencies_v2(version_id);

-- ذاكرة حل التبعيات
CREATE TABLE IF NOT EXISTS dependency_cache (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    cache_key TEXT NOT NULL UNIQUE, -- hash of dependency tree input
    resolved_tree TEXT NOT NULL, -- JSON of resolved versions
    resolution_time_ms INTEGER,
    hit_count INTEGER DEFAULT 0,
    last_hit_at DATETIME,
    expires_at DATETIME NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_dep_cache_key ON dependency_cache(cache_key);
CREATE INDEX IF NOT EXISTS idx_dep_cache_expires ON dependency_cache(expires_at);

-- التعارضات المعروفة
CREATE TABLE IF NOT EXISTS known_conflicts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    package_a TEXT NOT NULL,
    version_a TEXT NOT NULL,
    package_b TEXT NOT NULL,
    version_b TEXT NOT NULL,
    conflict_reason TEXT NOT NULL,
    severity TEXT DEFAULT 'warning', -- warning, error, critical
    workaround TEXT,
    reported_by INTEGER,
    verified BOOLEAN DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (reported_by) REFERENCES users(id)
);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ٣. التخزين المؤقت والأداء
-- ═══════════════════════════════════════════════════════════════════════════════

-- ذاكرة API المؤقتة
CREATE TABLE IF NOT EXISTS api_cache (
    cache_key TEXT PRIMARY KEY,
    response_body TEXT NOT NULL,
    content_type TEXT DEFAULT 'application/json',
    etag TEXT,
    last_modified DATETIME,
    expires_at DATETIME NOT NULL,
    hit_count INTEGER DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_api_cache_expires ON api_cache(expires_at);

-- ذاكرة البحث
CREATE TABLE IF NOT EXISTS search_cache (
    query_hash TEXT PRIMARY KEY,
    normalized_query TEXT NOT NULL,
    results_json TEXT NOT NULL,
    result_count INTEGER,
    search_time_ms INTEGER,
    hit_count INTEGER DEFAULT 0,
    last_hit_at DATETIME,
    expires_at DATETIME NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- قائمة انتظار المهام (للمهام الخلفية)
CREATE TABLE IF NOT EXISTS job_queue (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    job_type TEXT NOT NULL, -- reindex, cleanup, notify, mirror, analytics
    payload TEXT NOT NULL, -- JSON
    priority INTEGER DEFAULT 5, -- 1=highest, 10=lowest
    status TEXT DEFAULT 'pending', -- pending, running, completed, failed, retry
    attempts INTEGER DEFAULT 0,
    max_attempts INTEGER DEFAULT 3,
    run_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    started_at DATETIME,
    completed_at DATETIME,
    result TEXT,
    error_message TEXT,
    worker_id TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_job_queue_status ON job_queue(status, run_at);
CREATE INDEX IF NOT EXISTS idx_job_queue_type ON job_queue(job_type);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ٤. التحليلات المتقدمة
-- ═══════════════════════════════════════════════════════════════════════════════

-- تحليلات التنزيل التفصيلية
CREATE TABLE IF NOT EXISTS download_analytics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    version_id INTEGER NOT NULL,
    downloaded_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    client_ip_hash TEXT, -- مجزأ للخصوصية
    country_code TEXT,
    region TEXT,
    city TEXT,
    client_type TEXT, -- cli, browser, ci, mirror
    client_version TEXT,
    os_name TEXT,
    os_version TEXT,
    arch TEXT, -- x86_64, aarch64, wasm32
    sad_version TEXT, -- إصدار لغة ص
    is_ci BOOLEAN DEFAULT 0,
    referrer_hash TEXT,
    FOREIGN KEY (version_id) REFERENCES versions(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_download_analytics_version ON download_analytics(version_id);
CREATE INDEX IF NOT EXISTS idx_download_analytics_date ON download_analytics(downloaded_at);
CREATE INDEX IF NOT EXISTS idx_download_analytics_country ON download_analytics(country_code);

-- إحصائيات مجمعة بالساعة (للأداء)
CREATE TABLE IF NOT EXISTS hourly_stats (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    stat_hour DATETIME NOT NULL, -- بداية الساعة
    package_id INTEGER,
    version_id INTEGER,
    stat_type TEXT NOT NULL, -- downloads, searches, api_calls, errors
    count INTEGER DEFAULT 0,
    unique_count INTEGER DEFAULT 0, -- IPs فريدة
    metadata TEXT, -- JSON للبيانات الإضافية
    UNIQUE(stat_hour, package_id, version_id, stat_type),
    FOREIGN KEY (package_id) REFERENCES packages(id) ON DELETE CASCADE,
    FOREIGN KEY (version_id) REFERENCES versions(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_hourly_stats_hour ON hourly_stats(stat_hour);
CREATE INDEX IF NOT EXISTS idx_hourly_stats_package ON hourly_stats(package_id);

-- تتبع البحث (لتحسين الاقتراحات)
CREATE TABLE IF NOT EXISTS search_analytics (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    query_normalized TEXT NOT NULL,
    query_original TEXT NOT NULL,
    query_language TEXT, -- ar, en, mixed
    result_count INTEGER,
    clicked_package_id INTEGER,
    clicked_position INTEGER,
    search_time_ms INTEGER,
    client_ip_hash TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (clicked_package_id) REFERENCES packages(id) ON DELETE SET NULL
);

CREATE INDEX IF NOT EXISTS idx_search_analytics_query ON search_analytics(query_normalized);
CREATE INDEX IF NOT EXISTS idx_search_analytics_date ON search_analytics(created_at);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ٥. نظام الإشعارات
-- ═══════════════════════════════════════════════════════════════════════════════

-- الإشعارات
CREATE TABLE IF NOT EXISTS notifications (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    notification_type TEXT NOT NULL, -- new_version, security_alert, review, mention, system
    title TEXT NOT NULL,
    body TEXT,
    related_package_id INTEGER,
    related_user_id INTEGER,
    action_url TEXT,
    is_read BOOLEAN DEFAULT 0,
    is_archived BOOLEAN DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    read_at DATETIME,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (related_package_id) REFERENCES packages(id) ON DELETE SET NULL,
    FOREIGN KEY (related_user_id) REFERENCES users(id) ON DELETE SET NULL
);

CREATE INDEX IF NOT EXISTS idx_notifications_user ON notifications(user_id, is_read);
CREATE INDEX IF NOT EXISTS idx_notifications_date ON notifications(created_at);

-- الاشتراكات (متابعة حزم/مستخدمين)
CREATE TABLE IF NOT EXISTS subscriptions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    target_type TEXT NOT NULL, -- package, user, category
    target_id INTEGER NOT NULL,
    notify_new_version BOOLEAN DEFAULT 1,
    notify_security BOOLEAN DEFAULT 1,
    notify_deprecation BOOLEAN DEFAULT 1,
    email_enabled BOOLEAN DEFAULT 0,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE(user_id, target_type, target_id)
);

CREATE INDEX IF NOT EXISTS idx_subscriptions_target ON subscriptions(target_type, target_id);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ٦. المستودعات الخاصة والمنظمات
-- ═══════════════════════════════════════════════════════════════════════════════

-- المنظمات
CREATE TABLE IF NOT EXISTS organizations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    display_name TEXT NOT NULL,
    description TEXT,
    avatar_url TEXT,
    website_url TEXT,
    is_verified BOOLEAN DEFAULT 0,
    plan_type TEXT DEFAULT 'free', -- free, pro, enterprise
    plan_expires_at DATETIME,
    created_by INTEGER NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (created_by) REFERENCES users(id)
);

-- أعضاء المنظمات
CREATE TABLE IF NOT EXISTS organization_members (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    org_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    role TEXT DEFAULT 'member', -- owner, admin, member, readonly
    invited_by INTEGER,
    accepted_at DATETIME,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (org_id) REFERENCES organizations(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (invited_by) REFERENCES users(id) ON DELETE SET NULL,
    UNIQUE(org_id, user_id)
);

-- النطاقات (scopes) للحزم الخاصة
CREATE TABLE IF NOT EXISTS package_scopes (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    scope_name TEXT NOT NULL UNIQUE, -- @myorg
    owner_type TEXT NOT NULL, -- user, organization
    owner_id INTEGER NOT NULL,
    is_public BOOLEAN DEFAULT 0, -- هل الحزم عامة افتراضياً
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- صلاحيات الوصول للحزم الخاصة
CREATE TABLE IF NOT EXISTS package_access (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    package_id INTEGER NOT NULL,
    grantee_type TEXT NOT NULL, -- user, organization, team
    grantee_id INTEGER NOT NULL,
    access_level TEXT DEFAULT 'read', -- read, write, admin
    granted_by INTEGER NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (package_id) REFERENCES packages(id) ON DELETE CASCADE,
    FOREIGN KEY (granted_by) REFERENCES users(id) ON DELETE SET NULL,
    UNIQUE(package_id, grantee_type, grantee_id)
);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ٧. الترجمة والدعم متعدد اللغات
-- ═══════════════════════════════════════════════════════════════════════════════

-- الترجمات المجتمعية
CREATE TABLE IF NOT EXISTS community_translations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    package_id INTEGER NOT NULL,
    language_code TEXT NOT NULL, -- ar, en, fr, ur, etc.
    field_name TEXT NOT NULL, -- description, readme, release_notes
    original_text TEXT NOT NULL,
    translated_text TEXT NOT NULL,
    translator_id INTEGER,
    is_approved BOOLEAN DEFAULT 0,
    approved_by INTEGER,
    machine_translated BOOLEAN DEFAULT 0,
    translation_quality REAL, -- 0.0-1.0
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (package_id) REFERENCES packages(id) ON DELETE CASCADE,
    FOREIGN KEY (translator_id) REFERENCES users(id) ON DELETE SET NULL,
    FOREIGN KEY (approved_by) REFERENCES users(id) ON DELETE SET NULL,
    UNIQUE(package_id, language_code, field_name)
);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ٨. التنبيهات الأمنية
-- ═══════════════════════════════════════════════════════════════════════════════

-- التنبيهات الأمنية
CREATE TABLE IF NOT EXISTS security_advisories (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    advisory_id TEXT NOT NULL UNIQUE, -- SAD-2024-001
    package_id INTEGER NOT NULL,
    title TEXT NOT NULL,
    description TEXT NOT NULL,
    severity TEXT NOT NULL, -- low, medium, high, critical
    cvss_score REAL,
    affected_versions TEXT NOT NULL, -- version range
    patched_versions TEXT, -- version range
    cve_ids TEXT, -- JSON array
    cwe_ids TEXT, -- JSON array
    references_json TEXT, -- JSON array of URLs
    reporter_id INTEGER,
    is_public BOOLEAN DEFAULT 0,
    published_at DATETIME,
    withdrawn_at DATETIME,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (package_id) REFERENCES packages(id) ON DELETE CASCADE,
    FOREIGN KEY (reporter_id) REFERENCES users(id) ON DELETE SET NULL
);

CREATE INDEX IF NOT EXISTS idx_security_advisories_package ON security_advisories(package_id);
CREATE INDEX IF NOT EXISTS idx_security_advisories_severity ON security_advisories(severity);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ٩. المرآة والفدرالية
-- ═══════════════════════════════════════════════════════════════════════════════

-- مرايا المستودع
CREATE TABLE IF NOT EXISTS registry_mirrors (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    base_url TEXT NOT NULL UNIQUE,
    region TEXT, -- MENA, EU, NA, APAC
    country_code TEXT,
    mirror_type TEXT DEFAULT 'pull', -- pull, push, bidirectional
    sync_frequency_minutes INTEGER DEFAULT 60,
    is_official BOOLEAN DEFAULT 0,
    is_active BOOLEAN DEFAULT 1,
    last_sync_at DATETIME,
    last_sync_status TEXT,
    packages_synced INTEGER DEFAULT 0,
    bandwidth_used_mb INTEGER DEFAULT 0,
    health_score REAL DEFAULT 1.0,
    api_key_hash TEXT,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ١٠. تحديد المعدل (Rate Limiting)
-- ═══════════════════════════════════════════════════════════════════════════════

-- حدود المعدل
CREATE TABLE IF NOT EXISTS rate_limits (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    identifier TEXT NOT NULL, -- IP hash or user_id or API token prefix
    identifier_type TEXT NOT NULL, -- ip, user, token
    endpoint_pattern TEXT NOT NULL, -- /api/v1/packages, /api/v1/search, *
    window_start DATETIME NOT NULL,
    window_size_seconds INTEGER NOT NULL,
    request_count INTEGER DEFAULT 1,
    limit_value INTEGER NOT NULL,
    is_blocked BOOLEAN DEFAULT 0,
    blocked_until DATETIME,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(identifier, endpoint_pattern, window_start)
);

CREATE INDEX IF NOT EXISTS idx_rate_limits_identifier ON rate_limits(identifier, endpoint_pattern);
CREATE INDEX IF NOT EXISTS idx_rate_limits_window ON rate_limits(window_start);

-- ═══════════════════════════════════════════════════════════════════════════════
-- ١١. سجل الأحداث الشامل
-- ═══════════════════════════════════════════════════════════════════════════════

-- سجل الأحداث (للتدقيق والتصحيح)
CREATE TABLE IF NOT EXISTS event_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    event_type TEXT NOT NULL, -- publish, yank, login, register, download, search, admin_action
    event_subtype TEXT,
    actor_type TEXT, -- user, system, mirror, webhook
    actor_id INTEGER,
    target_type TEXT, -- package, version, user
    target_id INTEGER,
    metadata TEXT, -- JSON
    ip_address_hash TEXT,
    user_agent TEXT,
    request_id TEXT, -- للتتبع
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_event_log_type ON event_log(event_type, created_at);
CREATE INDEX IF NOT EXISTS idx_event_log_actor ON event_log(actor_type, actor_id);
CREATE INDEX IF NOT EXISTS idx_event_log_target ON event_log(target_type, target_id);
CREATE INDEX IF NOT EXISTS idx_event_log_date ON event_log(created_at);

-- ═══════════════════════════════════════════════════════════════════════════════
-- إعدادات النظام الإضافية
-- ═══════════════════════════════════════════════════════════════════════════════

INSERT OR IGNORE INTO system_settings (key, value, description) VALUES
    ('require_signature', 'false', 'هل يجب توقيع جميع الحزم؟'),
    ('allow_anonymous_downloads', 'true', 'السماح بالتنزيل بدون تسجيل'),
    ('max_versions_per_package', '1000', 'الحد الأقصى للإصدارات لكل حزمة'),
    ('enable_dependency_cache', 'true', 'تفعيل ذاكرة حل التبعيات'),
    ('cache_ttl_seconds', '3600', 'مدة صلاحية الذاكرة المؤقتة'),
    ('enable_analytics', 'true', 'تفعيل التحليلات المتقدمة'),
    ('analytics_retention_days', '365', 'مدة الاحتفاظ بالتحليلات'),
    ('enable_notifications', 'true', 'تفعيل نظام الإشعارات'),
    ('enable_security_advisories', 'true', 'تفعيل التنبيهات الأمنية'),
    ('enable_organizations', 'true', 'تفعيل المنظمات'),
    ('enable_private_packages', 'false', 'تفعيل الحزم الخاصة'),
    ('rate_limit_downloads', '1000', 'حد تنزيلات في الساعة'),
    ('rate_limit_api', '100', 'حد طلبات API في الدقيقة'),
    ('rate_limit_search', '60', 'حد عمليات البحث في الدقيقة'),
    ('mirror_sync_enabled', 'false', 'تفعيل مزامنة المرايا');

-- ═══════════════════════════════════════════════════════════════════════════════
-- تنظيف الذاكرة المؤقتة القديمة (تشغيل دوري)
-- ═══════════════════════════════════════════════════════════════════════════════

-- محفز لتنظيف الذاكرة المنتهية
CREATE TRIGGER IF NOT EXISTS trg_cleanup_expired_cache
AFTER INSERT ON api_cache
WHEN (SELECT COUNT(*) FROM api_cache) > 10000
BEGIN
    DELETE FROM api_cache WHERE expires_at < datetime('now');
    DELETE FROM search_cache WHERE expires_at < datetime('now');
    DELETE FROM dependency_cache WHERE expires_at < datetime('now');
END;

-- محفز لتنظيف سجل الأحداث القديم
CREATE TRIGGER IF NOT EXISTS trg_cleanup_old_events
AFTER INSERT ON event_log
WHEN (SELECT COUNT(*) FROM event_log) > 1000000
BEGIN
    DELETE FROM event_log WHERE created_at < datetime('now', '-90 days');
END;

-- ═══════════════════════════════════════════════════════════════════════════════
-- الفهارس الإضافية للأداء
-- ═══════════════════════════════════════════════════════════════════════════════

-- فهرس مركب للبحث السريع
CREATE INDEX IF NOT EXISTS idx_packages_search ON packages(name, arabic_name, is_yanked);
CREATE INDEX IF NOT EXISTS idx_packages_category ON packages(category, is_featured);
CREATE INDEX IF NOT EXISTS idx_packages_downloads ON packages(total_downloads DESC);
CREATE INDEX IF NOT EXISTS idx_packages_rating ON packages(rating DESC);

-- فهارس الإصدارات
CREATE INDEX IF NOT EXISTS idx_versions_package_created ON versions(package_id, created_at DESC);
CREATE INDEX IF NOT EXISTS idx_versions_downloads ON versions(downloads DESC);

-- ═══════════════════════════════════════════════════════════════════════════════
-- عرض مُحسّن للحزم (view)
-- ═══════════════════════════════════════════════════════════════════════════════

CREATE VIEW IF NOT EXISTS v_package_details AS
SELECT
    p.*,
    u.username AS owner_username,
    u.display_name AS owner_display_name,
    (SELECT COUNT(*) FROM stars WHERE package_id = p.id) AS star_count,
    (SELECT COUNT(*) FROM reviews WHERE package_id = p.id) AS review_count,
    (SELECT AVG(rating) FROM reviews WHERE package_id = p.id) AS avg_rating,
    (SELECT COUNT(*) FROM versions WHERE package_id = p.id AND is_yanked = 0) AS version_count,
    (SELECT COUNT(*) FROM subscriptions WHERE target_type = 'package' AND target_id = p.id) AS subscriber_count,
    (SELECT COUNT(*) FROM security_advisories WHERE package_id = p.id AND is_public = 1) AS advisory_count
FROM packages p
LEFT JOIN users u ON p.owner_id = u.id;
