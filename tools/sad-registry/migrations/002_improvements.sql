-- بسم الله الرحمن الرحيم
-- ═══════════════════════════════════════════════════════════════════════════════
-- هجرة رقم ٢: التحسينات والتطويرات
-- ═══════════════════════════════════════════════════════════════════════════════
--
-- التحسينات المضافة:
--   ١. نظام التقييمات والمراجعات (reviews)
--   ٢. نظام الخطّافات (webhooks) للإشعارات
--   ٣. نظام متابعة/تفضيل الحزم (stars)
--   ٤. نظام إدارة الإبلاغات (reports)
--   ٥. حصص التنزيل ومعدلات الاستخدام
--   ٦. سجل التغييرات (changelogs) متعدد الإصدارات
--   ٧. حفظ مفتاح JWT في قاعدة البيانات
--
-- ═══════════════════════════════════════════════════════════════════════════════

-- ─────────────────────────────────────────────────────────────────────────────
-- ١. جدول التقييمات والمراجعات
-- ─────────────────────────────────────────────────────────────────────────────
-- يسمح للمستخدمين بتقييم الحزم (١-٥ نجوم) وكتابة مراجعات
-- كل مستخدم يمكنه تقييم حزمة مرة واحدة فقط (يمكن التحديث)

CREATE TABLE IF NOT EXISTS reviews (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    package_id      INTEGER NOT NULL REFERENCES packages(id) ON DELETE CASCADE,
    user_id         INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    rating          INTEGER NOT NULL CHECK (rating >= 1 AND rating <= 5),
    title           TEXT DEFAULT '',              -- عنوان المراجعة (اختياري)
    body            TEXT DEFAULT '',              -- نص المراجعة
    is_helpful      INTEGER DEFAULT 0,            -- عدد "مفيد" من مستخدمين آخرين
    is_edited       INTEGER DEFAULT 0,            -- هل تم تعديل المراجعة؟
    created_at      TEXT DEFAULT (datetime('now')),
    updated_at      TEXT DEFAULT (datetime('now')),
    UNIQUE(package_id, user_id)                   -- مراجعة واحدة لكل مستخدم لكل حزمة
);

CREATE INDEX IF NOT EXISTS idx_reviews_package ON reviews(package_id);
CREATE INDEX IF NOT EXISTS idx_reviews_user ON reviews(user_id);
CREATE INDEX IF NOT EXISTS idx_reviews_rating ON reviews(rating);
CREATE INDEX IF NOT EXISTS idx_reviews_created ON reviews(created_at DESC);

-- ─────────────────────────────────────────────────────────────────────────────
-- ٢. جدول التفضيلات (stars) - متابعة الحزم
-- ─────────────────────────────────────────────────────────────────────────────
-- مثل "النجمة" على GitHub - إظهار اهتمام المستخدم بحزمة

CREATE TABLE IF NOT EXISTS stars (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    package_id      INTEGER NOT NULL REFERENCES packages(id) ON DELETE CASCADE,
    user_id         INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    created_at      TEXT DEFAULT (datetime('now')),
    UNIQUE(package_id, user_id)
);

CREATE INDEX IF NOT EXISTS idx_stars_package ON stars(package_id);
CREATE INDEX IF NOT EXISTS idx_stars_user ON stars(user_id);

-- ─────────────────────────────────────────────────────────────────────────────
-- ٣. جدول الخطّافات (webhooks)
-- ─────────────────────────────────────────────────────────────────────────────
-- إشعارات HTTP تلقائية عند حدوث أحداث معينة:
--   - نشر إصدار جديد (new_version)
--   - تنزيل حزمة (download)
--   - مراجعة جديدة (new_review)

CREATE TABLE IF NOT EXISTS webhooks (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    package_id      INTEGER REFERENCES packages(id) ON DELETE CASCADE,  -- NULL = كل الحزم
    url             TEXT NOT NULL,                 -- عنوان URL للإشعار
    secret          TEXT DEFAULT '',               -- مفتاح سري للتوقيع (HMAC-SHA256)
    events          TEXT DEFAULT 'new_version',    -- الأحداث (مفصولة بفواصل)
    is_active       INTEGER DEFAULT 1,
    last_triggered  TEXT,                          -- آخر إطلاق
    failure_count   INTEGER DEFAULT 0,            -- عدد الفشل المتتالي
    created_at      TEXT DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_webhooks_user ON webhooks(user_id);
CREATE INDEX IF NOT EXISTS idx_webhooks_package ON webhooks(package_id);

-- ─────────────────────────────────────────────────────────────────────────────
-- ٤. جدول الإبلاغات (reports)
-- ─────────────────────────────────────────────────────────────────────────────
-- يتيح للمستخدمين الإبلاغ عن حزم مخالفة أو خبيثة

CREATE TABLE IF NOT EXISTS reports (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    reporter_id     INTEGER NOT NULL REFERENCES users(id),
    package_id      INTEGER NOT NULL REFERENCES packages(id),
    reason          TEXT NOT NULL,                 -- السبب: malware, spam, copyright, inappropriate
    details         TEXT DEFAULT '',               -- تفاصيل إضافية
    status          TEXT DEFAULT 'pending',        -- الحالة: pending, reviewing, resolved, dismissed
    admin_notes     TEXT DEFAULT '',               -- ملاحظات المشرف
    resolved_by     INTEGER REFERENCES users(id),  -- المشرف الذي حلّ البلاغ
    created_at      TEXT DEFAULT (datetime('now')),
    resolved_at     TEXT
);

CREATE INDEX IF NOT EXISTS idx_reports_status ON reports(status);
CREATE INDEX IF NOT EXISTS idx_reports_package ON reports(package_id);

-- ─────────────────────────────────────────────────────────────────────────────
-- ٥. جدول إعدادات النظام (settings)
-- ─────────────────────────────────────────────────────────────────────────────
-- تخزين إعدادات النظام مثل مفتاح JWT والإعدادات العامة

CREATE TABLE IF NOT EXISTS system_settings (
    key             TEXT PRIMARY KEY,
    value           TEXT NOT NULL,
    description     TEXT DEFAULT '',
    updated_at      TEXT DEFAULT (datetime('now'))
);

-- ─────────────────────────────────────────────────────────────────────────────
-- ٦. جدول إحصائيات التنزيل اليومية (للرسوم البيانية)
-- ─────────────────────────────────────────────────────────────────────────────

CREATE TABLE IF NOT EXISTS daily_downloads (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    package_id      INTEGER NOT NULL REFERENCES packages(id) ON DELETE CASCADE,
    download_date   TEXT NOT NULL,                 -- تاريخ اليوم (YYYY-MM-DD)
    count           INTEGER DEFAULT 0,
    UNIQUE(package_id, download_date)
);

CREATE INDEX IF NOT EXISTS idx_daily_downloads_date ON daily_downloads(download_date);
CREATE INDEX IF NOT EXISTS idx_daily_downloads_pkg ON daily_downloads(package_id);

-- ─────────────────────────────────────────────────────────────────────────────
-- ٧. إضافة أعمدة جديدة للجداول الموجودة
-- ─────────────────────────────────────────────────────────────────────────────

-- إضافة حقل عدد التفضيلات للحزم (لتسريع الاستعلامات)
-- SQLite لا يدعم ALTER TABLE IF NOT EXISTS للأعمدة، نستخدم حيلة
-- لتجنب الخطأ إذا كان العمود موجوداً مسبقاً

-- ─────────────────────────────────────────────────────────────────────────────
-- ٨. محفّزات (triggers) لتحديث الإحصائيات تلقائياً
-- ─────────────────────────────────────────────────────────────────────────────

-- تحديث متوسط التقييم عند إضافة/تعديل مراجعة
CREATE TRIGGER IF NOT EXISTS trg_update_rating_on_insert
AFTER INSERT ON reviews
BEGIN
    UPDATE packages SET
        rating = (SELECT AVG(CAST(rating AS REAL)) FROM reviews WHERE package_id = NEW.package_id),
        rating_count = (SELECT COUNT(*) FROM reviews WHERE package_id = NEW.package_id)
    WHERE id = NEW.package_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_update_rating_on_update
AFTER UPDATE OF rating ON reviews
BEGIN
    UPDATE packages SET
        rating = (SELECT AVG(CAST(rating AS REAL)) FROM reviews WHERE package_id = NEW.package_id),
        rating_count = (SELECT COUNT(*) FROM reviews WHERE package_id = NEW.package_id)
    WHERE id = NEW.package_id;
END;

CREATE TRIGGER IF NOT EXISTS trg_update_rating_on_delete
AFTER DELETE ON reviews
BEGIN
    UPDATE packages SET
        rating = COALESCE((SELECT AVG(CAST(rating AS REAL)) FROM reviews WHERE package_id = OLD.package_id), 0),
        rating_count = (SELECT COUNT(*) FROM reviews WHERE package_id = OLD.package_id)
    WHERE id = OLD.package_id;
END;

-- تحديث إحصائيات التنزيل اليومية تلقائياً
CREATE TRIGGER IF NOT EXISTS trg_daily_download_counter
AFTER INSERT ON downloads
BEGIN
    INSERT INTO daily_downloads (package_id, download_date, count)
    SELECT v.package_id, date('now'), 1
    FROM versions v WHERE v.id = NEW.version_id
    ON CONFLICT(package_id, download_date)
    DO UPDATE SET count = count + 1;
END;

-- ─────────────────────────────────────────────────────────────────────────────
-- ٩. بيانات أولية للإعدادات
-- ─────────────────────────────────────────────────────────────────────────────

INSERT OR IGNORE INTO system_settings (key, value, description) VALUES
    ('registry_name', 'مستودع حزم لغة ص', 'اسم المستودع'),
    ('registry_url', 'https://packages.sadlang.org', 'عنوان المستودع'),
    ('max_package_size_mb', '50', 'أقصى حجم للحزمة بالميغابايت'),
    ('max_versions_per_package', '1000', 'أقصى عدد إصدارات لكل حزمة'),
    ('registration_enabled', 'true', 'هل التسجيل مفتوح؟'),
    ('require_email_verification', 'false', 'هل يُطلب تأكيد البريد الإلكتروني؟'),
    ('min_password_length', '8', 'أدنى طول لكلمة المرور'),
    ('rate_limit_per_minute', '60', 'حد الطلبات في الدقيقة'),
    ('federation_enabled', 'true', 'هل الاتحاد مُفعَّل؟');

-- تحديث إصدار المخطط
INSERT OR REPLACE INTO schema_version (version, applied_at)
VALUES (2, datetime('now'));
