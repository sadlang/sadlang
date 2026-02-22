// بسم الله الرحمن الرحيم
// =========================================================================
// الموجّه الرئيسي لـ API v1 — يربط جميع المسارات الفرعية
// Main API v1 Router — Connects all sub-routes
// =========================================================================

const { Router } = require('express');
const { ERROR_CODES } = require('../utils/error-codes');

function createApiRouter(db, services) {
    const router = Router();

    // ═══════════════════════════════════════════════════════════════
    // GET /api/v1/ — معلومات عن API
    // ═══════════════════════════════════════════════════════════════
    router.get('/', (req, res) => {
        res.json({
            success: true,
            name: 'سجل حزم لغة ص',
            name_en: 'Sad Language Package Registry',
            version: '2.0.0',
            description: 'واجهة برمجية لإدارة حزم لغة ص البرمجية',
            endpoints: {
                info: 'GET /api/v1/',
                health: 'GET /api/v1/health',
                auth: {
                    register: 'POST /api/v1/auth/register',
                    login: 'POST /api/v1/auth/login',
                    token: 'POST /api/v1/auth/token',
                },
                packages: {
                    search: 'GET /api/v1/packages/search?q={query}&limit=20&offset=0&category={slug}',
                    info: 'GET /api/v1/packages/{name}',
                    versions: 'GET /api/v1/packages/{name}/versions',
                    version_info: 'GET /api/v1/packages/{name}/{version}',
                    download: 'GET /api/v1/packages/{name}/{version}/download',
                    publish: 'POST /api/v1/packages/publish (multipart: package + metadata)',
                    yank: 'DELETE /api/v1/packages/{name}/{version}/yank',
                },
                user: {
                    profile: 'GET /api/v1/user/profile',
                    packages: 'GET /api/v1/user/packages',
                },
                stats: {
                    overview: 'GET /api/v1/stats',
                    downloads: 'GET /api/v1/stats/downloads?days=30',
                    categories: 'GET /api/v1/stats/categories',
                },
            },
            error_format: {
                description: 'جميع الأخطاء تُرجع بنفس الهيكل',
                example: {
                    success: false,
                    error: {
                        code: 'PKG_001',
                        message: 'الحزمة غير موجودة (رسالة عربية مختصرة)',
                        message_en: 'Package not found (brief English message)',
                        explain: 'شرح تفصيلي للمشكلة',
                        hint: 'الحل المقترح أو الخطوة التالية',
                    },
                    request_id: 'abc123',
                    timestamp: '2026-01-01T00:00:00.000Z',
                },
            },
            documentation: 'https://sadlang.org/docs/packages',
            timestamp: new Date().toISOString(),
        });
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /api/v1/health — فحص صحة الخادم
    // ═══════════════════════════════════════════════════════════════
    router.get('/health', (req, res) => {
        try {
            // فحص قاعدة البيانات
            const dbCheck = db.prepare('SELECT 1 as ok').get();
            const stats = services.statsService.getSummary();

            res.json({
                success: true,
                status: 'healthy',
                status_ar: 'الخادم يعمل بشكل طبيعي',
                uptime_seconds: Math.floor(process.uptime()),
                database: dbCheck ? 'connected' : 'disconnected',
                stats: {
                    packages: stats.total_packages,
                    users: stats.total_users,
                    versions: stats.total_versions,
                },
                memory: {
                    used_mb: Math.round(process.memoryUsage().heapUsed / 1024 / 1024),
                    total_mb: Math.round(process.memoryUsage().heapTotal / 1024 / 1024),
                },
                node_version: process.version,
                timestamp: new Date().toISOString(),
            });
        } catch (err) {
            res.status(503).json({
                success: false,
                status: 'unhealthy',
                status_ar: 'الخادم يعاني من مشاكل',
                error: err.message,
                timestamp: new Date().toISOString(),
            });
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /api/v1/errors — قائمة رموز الأخطاء المتاحة (للمطورين)
    // ═══════════════════════════════════════════════════════════════
    router.get('/errors', (req, res) => {
        const errors = Object.entries(ERROR_CODES).map(([key, val]) => ({
            code: val.code,
            status: val.status,
            message: val.message,
            message_en: val.message_en,
        }));

        res.json({
            success: true,
            description: 'قائمة بجميع رموز الأخطاء المستخدمة في API',
            total: errors.length,
            errors,
        });
    });

    // ربط المسارات الفرعية
    const createAuthRouter = require('./auth');
    const createPackageRouter = require('./packages');
    const createUserRouter = require('./user');
    const createStatsRouter = require('./stats');

    router.use('/auth', createAuthRouter(db, services));
    router.use('/packages', createPackageRouter(db, services));
    router.use('/user', createUserRouter(db, services));
    router.use('/stats', createStatsRouter(db, services));

    return router;
}

module.exports = createApiRouter;
