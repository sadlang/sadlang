// بسم الله الرحمن الرحيم
// =========================================================================
// مسارات واجهة الويب العربية — Arabic Web Interface Routes
// =========================================================================
//
// @file web.js
//
// الوظيفة:
//   توفر صفحات HTML للتصفح البشري (بخلاف API JSON).
//   تستخدم محرك قوالب EJS مع تنسيق RTL عربي.
//
// الصفحات:
//   GET /               — الرئيسية (إحصائيات + حزم شعبية + أحدث)
//   GET /search          — البحث مع تصفية بالتصنيف
//   GET /packages/:name  — تفاصيل حزمة (إصدارات + ملّاك + اعتماديات)
//
// ملاحظة:
//   هذه المسارات لا تتطلب مصادقة — الواجهة للقراءة فقط.
//   لنشر الحزم يجب استخدام API عبر sad-pkg.
// =========================================================================

const express = require('express');
const router = express.Router();

// ============================================================================
// الصفحة الرئيسية / Home Page
// ============================================================================

router.get('/', (req, res) => {
    const db = req.app.locals.db;

    try {
        const stats = {
            packages: db.prepare('SELECT COUNT(*) as c FROM packages WHERE is_yanked = 0').get().c,
            downloads: db.prepare('SELECT COALESCE(SUM(total_downloads), 0) as c FROM packages').get().c,
            users: db.prepare('SELECT COUNT(*) as c FROM users WHERE is_active = 1').get().c,
        };

        const popular = db.prepare(`
            SELECT p.name, p.description_ar, p.description, p.total_downloads, 
                   p.latest_version, u.username as author
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.is_yanked = 0
            ORDER BY p.total_downloads DESC LIMIT 12
        `).all();

        const newest = db.prepare(`
            SELECT p.name, p.description_ar, p.description, p.latest_version, 
                   p.created_at, u.username as author
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.is_yanked = 0
            ORDER BY p.created_at DESC LIMIT 6
        `).all();

        const categories = db.prepare('SELECT * FROM categories ORDER BY name_ar').all();

        res.render('index', { stats, popular, newest, categories });
    } catch (error) {
        console.error('خطأ:', error.message);
        res.render('index', { stats: { packages: 0, downloads: 0, users: 0 }, popular: [], newest: [], categories: [] });
    }
});

// ============================================================================
// صفحة البحث / Search Page
// ============================================================================

router.get('/search', (req, res) => {
    const { q = '', category = '' } = req.query;
    const db = req.app.locals.db;

    try {
        const search = `%${q}%`;
        let packages;

        if (category) {
            packages = db.prepare(`
                SELECT p.name, p.description_ar, p.description, p.total_downloads,
                       p.latest_version, p.keywords, u.username as author
                FROM packages p
                JOIN users u ON p.owner_id = u.id
                WHERE p.is_yanked = 0 AND p.categories LIKE ? 
                  AND (p.name LIKE ? OR p.description LIKE ? OR p.description_ar LIKE ?)
                ORDER BY p.total_downloads DESC LIMIT 50
            `).all(`%"${category}"%`, search, search, search);
        } else {
            packages = db.prepare(`
                SELECT p.name, p.description_ar, p.description, p.total_downloads,
                       p.latest_version, p.keywords, u.username as author
                FROM packages p
                JOIN users u ON p.owner_id = u.id
                WHERE p.is_yanked = 0
                  AND (p.name LIKE ? OR p.description LIKE ? OR p.description_ar LIKE ?)
                ORDER BY p.total_downloads DESC LIMIT 50
            `).all(search, search, search);
        }

        const categories = db.prepare('SELECT * FROM categories ORDER BY name_ar').all();
        res.render('search', { q, category, packages, categories });
    } catch (error) {
        res.render('search', { q, category: '', packages: [], categories: [] });
    }
});

// ============================================================================
// صفحة الحزمة / Package Page
// ============================================================================

router.get('/packages/:name', (req, res) => {
    const { name } = req.params;
    const db = req.app.locals.db;

    try {
        const pkg = db.prepare(`
            SELECT p.*, u.username as author
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.name = ?
        `).get(name);

        if (!pkg) {
            return res.status(404).render('404', { message: `الحزمة "${name}" غير موجودة` });
        }

        const versions = db.prepare(`
            SELECT v.*, u.username as publisher
            FROM versions v
            LEFT JOIN users u ON v.published_by = u.id
            WHERE v.package_id = ?
            ORDER BY v.major DESC, v.minor DESC, v.patch DESC
        `).all(pkg.id);

        const owners = db.prepare(`
            SELECT u.username, u.display_name, po.role
            FROM package_owners po
            JOIN users u ON po.user_id = u.id
            WHERE po.package_id = ?
        `).all(pkg.id);

        const latestVersion = versions.find(v => !v.is_yanked) || null;

        res.render('package', { 
            pkg, 
            versions, 
            owners, 
            latestVersion,
            dependencies: latestVersion ? JSON.parse(latestVersion.dependencies || '{}') : {},
        });
    } catch (error) {
        console.error('خطأ:', error.message);
        res.status(500).render('error', { message: 'خطأ في الخادم' });
    }
});

module.exports = router;
