// بسم الله الرحمن الرحيم
// =========================================================================
// معالجة الأخطاء المركزية — رسائل مفصّلة مع شرح وحلول
// Central Error Handler — Detailed messages with explanations & solutions
// =========================================================================

const { createError, RegistryError, sendError } = require('../utils/error-codes');

/**
 * معالج الأخطاء 404 — مسار غير موجود
 */
function notFoundHandler(req, res, next) {
    // API requests
    if (req.path.startsWith('/api/')) {
        const availableRoutes = [
            'GET  /api/v1/                        — معلومات API',
            'GET  /api/v1/health                   — فحص صحة الخادم',
            'POST /api/v1/auth/register            — تسجيل حساب جديد',
            'POST /api/v1/auth/login               — تسجيل الدخول',
            'POST /api/v1/auth/token               — إنشاء رمز API',
            'GET  /api/v1/packages/search?q=...    — البحث عن حزم',
            'GET  /api/v1/packages/:name           — معلومات حزمة',
            'GET  /api/v1/packages/:name/versions  — إصدارات حزمة',
            'GET  /api/v1/packages/:name/:version  — معلومات إصدار',
            'GET  /api/v1/packages/:name/:version/download — تنزيل',
            'POST /api/v1/packages/publish         — نشر حزمة',
            'GET  /api/v1/user/packages            — حزمي',
            'GET  /api/v1/user/profile             — ملفي الشخصي',
            'GET  /api/v1/stats                    — إحصائيات',
        ];

        return res.status(404).json({
            ...createError('GEN_001', {
                hint: `المسار "${req.method} ${req.path}" غير موجود.\n\nالمسارات المتاحة:\n${availableRoutes.join('\n')}`,
            }),
            requested_path: `${req.method} ${req.path}`,
            timestamp: new Date().toISOString(),
        });
    }

    // Web pages — redirect to home
    res.status(404).render('404', {
        title: 'الصفحة غير موجودة — 404',
        path: req.path,
    });
}

/**
 * معالج أخطاء JSON التالفة
 */
function jsonErrorHandler(err, req, res, next) {
    if (err.type === 'entity.parse.failed') {
        return sendError(res, 'GEN_003', {
            details: { received: err.body?.substring(0, 200) },
        });
    }
    next(err);
}

/**
 * معالج أخطاء multer (رفع الملفات)
 */
function uploadErrorHandler(err, req, res, next) {
    if (err.code === 'LIMIT_FILE_SIZE') {
        return sendError(res, 'PUB_003');
    }
    if (err.code === 'LIMIT_UNEXPECTED_FILE') {
        return sendError(res, 'PUB_004', {
            explain: 'حقل الملف غير متوقع. استخدم حقل "package" لرفع ملف الحزمة',
        });
    }
    if (err.message && err.message.includes('multer')) {
        return sendError(res, 'PUB_001');
    }
    next(err);
}

/**
 * معالج الأخطاء العام — آخر خط دفاع
 */
function globalErrorHandler(err, req, res, next) {
    // إذا كان خطأ RegistryError مخصص
    if (err instanceof RegistryError) {
        return res.status(err.status).json({
            ...err.toJSON(),
            request_id: req.requestId,
            timestamp: new Date().toISOString(),
        });
    }

    // تسجيل الخطأ
    const requestId = req.requestId || 'unknown';
    console.error(`\n╔══════════════════════════════════════════════════`);
    console.error(`║ خطأ في الخادم — Request ID: ${requestId}`);
    console.error(`║ المسار: ${req.method} ${req.path}`);
    console.error(`║ الوقت: ${new Date().toISOString()}`);
    console.error(`╠──────────────────────────────────────────────────`);
    console.error(`║ ${err.message}`);
    if (process.env.NODE_ENV !== 'production') {
        console.error(`║ Stack: ${err.stack}`);
    }
    console.error(`╚══════════════════════════════════════════════════\n`);

    // استجابة للعميل
    const isProduction = process.env.NODE_ENV === 'production';
    const errorResponse = createError('GEN_002', {
        ...(isProduction ? {} : {
            explain: err.message,
            details: { stack: err.stack?.split('\n').slice(0, 5) },
        }),
    });

    res.status(500).json({
        ...errorResponse,
        request_id: requestId,
        timestamp: new Date().toISOString(),
    });
}

module.exports = {
    notFoundHandler,
    jsonErrorHandler,
    uploadErrorHandler,
    globalErrorHandler,
};
