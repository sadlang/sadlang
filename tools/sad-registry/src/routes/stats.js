// بسم الله الرحمن الرحيم
// =========================================================================
// مسارات الإحصائيات — إحصائيات السجل العامة
// Stats Routes — Public registry statistics
// =========================================================================

const { Router } = require('express');
const { sendError } = require('../utils/error-codes');

function createStatsRouter(db, services) {
    const router = Router();
    const { statsService } = services;

    // ═══════════════════════════════════════════════════════════════
    // GET / — إحصائيات شاملة
    // ═══════════════════════════════════════════════════════════════
    router.get('/', (req, res) => {
        try {
            const result = statsService.getFullStats();
            res.json(result);
        } catch (err) {
            console.error('خطأ في الإحصائيات:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /downloads — إحصائيات التنزيلات اليومية
    // ═══════════════════════════════════════════════════════════════
    router.get('/downloads', (req, res) => {
        try {
            const days = parseInt(req.query.days) || 30;
            const downloads = statsService.getDailyDownloads(Math.min(days, 365));
            res.json({
                success: true,
                period_days: days,
                downloads,
            });
        } catch (err) {
            console.error('خطأ في إحصائيات التنزيلات:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /categories — قائمة التصنيفات
    // ═══════════════════════════════════════════════════════════════
    router.get('/categories', (req, res) => {
        try {
            const categories = statsService.getCategories();
            res.json({
                success: true,
                categories,
            });
        } catch (err) {
            console.error('خطأ في التصنيفات:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    return router;
}

module.exports = createStatsRouter;
