// بسم الله الرحمن الرحيم
// =========================================================================
// مسارات المستخدم — User Routes
// =========================================================================
//
// @file user.js
//
// نقاط النهاية:
//   GET /api/v1/user/packages — حزم المستخدم الحالي (يتطلب مصادقة)
//   GET /api/v1/user/profile  — الملف الشخصي مع الإحصائيات
//
// المصادقة:
//   كل نقاط النهاية هنا تتطلب رمز JWT أو API token صالح.
//   يُستخدم middleware authenticate لفرض ذلك.
//
// الاستخدام في sad-pkg:
//   sad-pkg my-packages → GET /api/v1/user/packages
//   sad-pkg whoami      → GET /api/v1/user/profile
// =========================================================================

const express = require('express');
const router = express.Router();
const { authenticate } = require('../middleware/auth');

// ============================================================================
// GET /api/v1/user/packages — حزم المستخدم / User's Packages
// ============================================================================

router.get('/packages', authenticate, (req, res) => {
    const db = req.app.locals.db;
    const userId = req.user.id;

    try {
        const packages = db.prepare(`
            SELECT DISTINCT p.name, p.description, p.description_ar, p.latest_version,
                   p.total_downloads, p.license, p.created_at, p.updated_at
            FROM packages p
            LEFT JOIN package_owners po ON p.id = po.package_id
            WHERE p.owner_id = ? OR po.user_id = ?
            ORDER BY p.updated_at DESC
        `).all(userId, userId);

        res.json({
            packages: packages.map(p => ({
                name: p.name,
                description: p.description,
                description_ar: p.description_ar,
                latest_version: p.latest_version,
                downloads: p.total_downloads,
                license: p.license,
                created_at: p.created_at,
                updated_at: p.updated_at,
            }))
        });
    } catch (error) {
        console.error('خطأ في جلب حزم المستخدم:', error.message);
        res.status(500).json({ error: 'خطأ في الخادم' });
    }
});

// ============================================================================
// GET /api/v1/user/profile — الملف الشخصي / Profile
// ============================================================================

router.get('/profile', authenticate, (req, res) => {
    const db = req.app.locals.db;
    const userId = req.user.id;

    try {
        const user = db.prepare(`
            SELECT id, username, email, display_name, bio, avatar_url, created_at
            FROM users WHERE id = ?
        `).get(userId);

        const packageCount = db.prepare(`
            SELECT COUNT(*) as count FROM packages WHERE owner_id = ?
        `).get(userId);

        const totalDownloads = db.prepare(`
            SELECT COALESCE(SUM(p.total_downloads), 0) as total
            FROM packages p WHERE p.owner_id = ?
        `).get(userId);

        res.json({
            ...user,
            stats: {
                packages: packageCount.count,
                total_downloads: totalDownloads.total,
            }
        });
    } catch (error) {
        res.status(500).json({ error: 'خطأ في الخادم' });
    }
});

module.exports = router;
