// بسم الله الرحمن الرحيم
// =========================================================================
// مسار الإحصائيات — Registry Statistics Route
// =========================================================================
//
// @file stats.js
//
// نقطة النهاية:
//   GET /api/v1/stats
//
// الوصف:
//   يُرجع إحصائيات شاملة عن المستودع في كائن JSON واحد.
//   لا يتطلب مصادقة (عام).
//
// البيانات المُرجعة:
//   - summary: الأرقام الأساسية (حزم، إصدارات، مستخدمين، تنزيلات)
//   - popular_packages: أكثر 10 حزم تنزيلاً
//   - newest_packages: أحدث 10 حزم
//   - recently_updated: آخر 10 حزم مُحدّثة
//   - categories: كل التصنيفات المتاحة
//
// الاستخدام في sad-pkg:
//   sad-pkg stats → يعرض ملخص المستودع في الطرفية
// =========================================================================

const express = require('express');
const router = express.Router();

// ============================================================================
// GET /api/v1/stats — إحصائيات عامة / General Stats
// ============================================================================

router.get('/', (req, res) => {
    const db = req.app.locals.db;

    try {
        const totalPackages = db.prepare('SELECT COUNT(*) as count FROM packages WHERE is_yanked = 0').get();
        const totalVersions = db.prepare('SELECT COUNT(*) as count FROM versions WHERE is_yanked = 0').get();
        const totalUsers = db.prepare('SELECT COUNT(*) as count FROM users WHERE is_active = 1').get();
        const totalDownloads = db.prepare('SELECT COALESCE(SUM(total_downloads), 0) as total FROM packages').get();

        // أكثر الحزم تنزيلاً / Most Downloaded
        const popular = db.prepare(`
            SELECT p.name, p.description_ar, p.description, p.total_downloads, p.latest_version
            FROM packages p
            WHERE p.is_yanked = 0
            ORDER BY p.total_downloads DESC
            LIMIT 10
        `).all();

        // أحدث الحزم / Newest
        const newest = db.prepare(`
            SELECT p.name, p.description_ar, p.description, p.latest_version, p.created_at
            FROM packages p
            WHERE p.is_yanked = 0
            ORDER BY p.created_at DESC
            LIMIT 10
        `).all();

        // أحدث التحديثات / Recently Updated
        const updated = db.prepare(`
            SELECT p.name, p.latest_version, p.updated_at
            FROM packages p
            WHERE p.is_yanked = 0
            ORDER BY p.updated_at DESC
            LIMIT 10
        `).all();

        // التصنيفات / Categories
        const categories = db.prepare('SELECT * FROM categories ORDER BY name_ar').all();

        res.json({
            summary: {
                total_packages: totalPackages.count,
                total_versions: totalVersions.count,
                total_users: totalUsers.count,
                total_downloads: totalDownloads.total,
            },
            popular_packages: popular,
            newest_packages: newest,
            recently_updated: updated,
            categories,
        });
    } catch (error) {
        console.error('خطأ في الإحصائيات:', error.message);
        res.status(500).json({ error: 'خطأ في الخادم' });
    }
});

module.exports = router;
