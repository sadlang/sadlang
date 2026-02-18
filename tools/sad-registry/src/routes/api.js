// بسم الله الرحمن الرحيم
/**
 * @file api.js
 * @description مسارات API v1 لمستودع حزم لغة ص
 * 
 * جميع نقاط النهاية متوافقة مع عميل sad-pkg (registry_client.h)
 * All endpoints are compatible with the sad-pkg client (registry_client.h)
 * 
 * API Contract (from registry_client.h):
 *   GET  /api/v1/search?q=<query>&limit=<n>           → {packages: [{name, ...}]}
 *   GET  /api/v1/packages/:name                        → {name, description, ...} (200 = exists)
 *   GET  /api/v1/packages/:name/versions               → {versions: ["1.0.0", ...]}
 *   GET  /api/v1/packages/:name/:version               → {description, authors, license, dependencies, ...}
 *   GET  /api/v1/packages/:name/:version/download      → binary file download
 *   POST /api/v1/packages/publish                      → upload package (auth required)
 *   GET  /api/v1/user/packages                         → {packages: [{name, ...}]} (auth required)
 */

const express = require('express');
const router = express.Router();

const packagesRouter = require('./packages');
const authRouter = require('./auth');
const userRouter = require('./user');
const statsRouter = require('./stats');

// ============================================================================
// معلومات API / API Info
// ============================================================================

router.get('/', (req, res) => {
    res.json({
        name: 'سجل حزم لغة ص',
        name_en: 'Sad Language Package Registry',
        version: '1.0.0',
        api_version: 'v1',
        endpoints: {
            search: 'GET /api/v1/search?q=<query>&limit=<n>',
            package_info: 'GET /api/v1/packages/:name',
            package_versions: 'GET /api/v1/packages/:name/versions',
            version_info: 'GET /api/v1/packages/:name/:version',
            download: 'GET /api/v1/packages/:name/:version/download',
            publish: 'POST /api/v1/packages/publish',
            register: 'POST /api/v1/auth/register',
            login: 'POST /api/v1/auth/login',
            user_packages: 'GET /api/v1/user/packages',
            stats: 'GET /api/v1/stats',
        }
    });
});

// ============================================================================
// البحث / Search
// ============================================================================

router.get('/search', (req, res) => {
    const { q = '', limit = 20, offset = 0, category } = req.query;
    const db = req.app.locals.db;
    const limitNum = Math.min(parseInt(limit) || 20, 100);
    const offsetNum = parseInt(offset) || 0;

    let query, params;

    if (category) {
        query = `
            SELECT p.name, p.description, p.description_ar, p.latest_version,
                   p.total_downloads, p.license, p.keywords, p.created_at,
                   u.username as author
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.is_yanked = 0
              AND (p.name LIKE ? OR p.description LIKE ? OR p.description_ar LIKE ? OR p.keywords LIKE ?)
              AND p.categories LIKE ?
            ORDER BY p.total_downloads DESC
            LIMIT ? OFFSET ?
        `;
        const search = `%${q}%`;
        const catSearch = `%"${category}"%`;
        params = [search, search, search, search, catSearch, limitNum, offsetNum];
    } else {
        query = `
            SELECT p.name, p.description, p.description_ar, p.latest_version,
                   p.total_downloads, p.license, p.keywords, p.created_at,
                   u.username as author
            FROM packages p
            JOIN users u ON p.owner_id = u.id
            WHERE p.is_yanked = 0
              AND (p.name LIKE ? OR p.description LIKE ? OR p.description_ar LIKE ? OR p.keywords LIKE ?)
            ORDER BY p.total_downloads DESC
            LIMIT ? OFFSET ?
        `;
        const search = `%${q}%`;
        params = [search, search, search, search, limitNum, offsetNum];
    }

    try {
        const packages = db.prepare(query).all(...params);

        // عدد النتائج الكلي / Total count
        const countQuery = `
            SELECT COUNT(*) as total FROM packages 
            WHERE is_yanked = 0
              AND (name LIKE ? OR description LIKE ? OR description_ar LIKE ? OR keywords LIKE ?)
        `;
        const search = `%${q}%`;
        const { total } = db.prepare(countQuery).get(search, search, search, search);

        res.json({
            packages: packages.map(p => ({
                name: p.name,
                description: p.description,
                description_ar: p.description_ar,
                latest_version: p.latest_version,
                downloads: p.total_downloads,
                license: p.license,
                keywords: JSON.parse(p.keywords || '[]'),
                author: p.author,
                created_at: p.created_at,
            })),
            total,
            limit: limitNum,
            offset: offsetNum,
        });
    } catch (error) {
        console.error('خطأ في البحث / Search error:', error.message);
        res.status(500).json({ error: 'فشل البحث / Search failed' });
    }
});

// ============================================================================
// المسارات الفرعية / Sub-routes
// ============================================================================

router.use('/packages', packagesRouter);
router.use('/auth', authRouter);
router.use('/user', userRouter);
router.use('/stats', statsRouter);

module.exports = router;
