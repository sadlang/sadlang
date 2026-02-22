// بسم الله الرحمن الرحيم
// =========================================================================
// مسارات المستخدم — الملف الشخصي والحزم
// User Routes — Profile and packages
// =========================================================================

const { Router } = require('express');
const { sendError } = require('../utils/error-codes');
const { authenticate } = require('../middleware/auth');

function createUserRouter(db, services) {
    const router = Router();
    const { userService } = services;

    // ═══════════════════════════════════════════════════════════════
    // GET /profile — الملف الشخصي
    // ═══════════════════════════════════════════════════════════════
    router.get('/profile', authenticate(db), (req, res) => {
        try {
            const result = userService.getProfile(req.user.id);
            if (!result) {
                return sendError(res, 'AUTH_002', {
                    explain: 'لم يتم العثور على بيانات المستخدم',
                });
            }
            res.json(result);
        } catch (err) {
            console.error('خطأ في الملف الشخصي:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /packages — حزم المستخدم
    // ═══════════════════════════════════════════════════════════════
    router.get('/packages', authenticate(db), (req, res) => {
        try {
            const packages = userService.getUserPackages(req.user.id);
            res.json({
                success: true,
                packages,
                total: packages.length,
            });
        } catch (err) {
            console.error('خطأ في جلب الحزم:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    return router;
}

module.exports = createUserRouter;
