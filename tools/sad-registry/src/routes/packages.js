// بسم الله الرحمن الرحيم
// =========================================================================
// مسارات الحزم — CRUD + نشر + تنزيل + سحب
// Package Routes — CRUD, publish, download, yank
// =========================================================================

const { Router } = require('express');
const multer = require('multer');
const path = require('path');
const { sendError } = require('../utils/error-codes');
const { validatePackageName, validateVersion } = require('../utils/validators');
const { authenticate } = require('../middleware/auth');
const { publishLimiter } = require('../middleware/rate-limit');

// إعداد multer للرفع
const upload = multer({
    dest: path.join(process.cwd(), 'storage', 'temp'),
    limits: { fileSize: 50 * 1024 * 1024 }, // 50MB
    fileFilter: (req, file, cb) => {
        const allowed = ['.tar.gz', '.tgz', '.zip', '.gz'];
        const ext = path.extname(file.originalname).toLowerCase();
        // tar.gz يأتي كـ .gz
        if (allowed.includes(ext) || file.mimetype === 'application/gzip' ||
            file.mimetype === 'application/x-tar' || file.mimetype === 'application/zip' ||
            file.mimetype === 'application/x-gzip' || file.mimetype === 'application/octet-stream') {
            cb(null, true);
        } else {
            cb(new Error('نوع الملف غير مدعوم'));
        }
    },
});

function createPackageRouter(db, services) {
    const router = Router();
    const { packageService } = services;

    // ═══════════════════════════════════════════════════════════════
    // GET /search — البحث عن حزم
    // ═══════════════════════════════════════════════════════════════
    router.get('/search', (req, res) => {
        try {
            const { q = '', limit, offset, category } = req.query;
            const result = packageService.search(q, { limit, offset, category });
            res.json(result);
        } catch (err) {
            console.error('خطأ في البحث:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /:name — معلومات حزمة
    // ═══════════════════════════════════════════════════════════════
    router.get('/:name', (req, res) => {
        try {
            const { name } = req.params;

            const nameCheck = validatePackageName(name);
            if (!nameCheck.valid) {
                return sendError(res, 'PKG_005', { explain: nameCheck.reason });
            }

            const result = packageService.getPackageInfo(name);
            if (!result.success) {
                return res.status(result.error?.code === 'PKG_001' ? 404 : 400).json(result);
            }

            res.json(result);
        } catch (err) {
            console.error('خطأ في جلب الحزمة:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /:name/versions — قائمة إصدارات الحزمة
    // ═══════════════════════════════════════════════════════════════
    router.get('/:name/versions', (req, res) => {
        try {
            const { name } = req.params;
            const result = packageService.getVersionsList(name);

            if (!result.success) {
                return res.status(404).json(result);
            }

            res.json(result);
        } catch (err) {
            console.error('خطأ في جلب الإصدارات:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /:name/:version — معلومات إصدار محدد
    // ═══════════════════════════════════════════════════════════════
    router.get('/:name/:version', (req, res) => {
        try {
            const { name, version } = req.params;

            const versionCheck = validateVersion(version);
            if (!versionCheck.valid) {
                return sendError(res, 'PKG_006', { explain: versionCheck.reason });
            }

            const result = packageService.getVersionInfo(name, version);
            if (!result.success) {
                const status = result.error?.code === 'PKG_001' ? 404 :
                              result.error?.code === 'PKG_002' ? 404 : 400;
                return res.status(status).json(result);
            }

            res.json(result);
        } catch (err) {
            console.error('خطأ في جلب الإصدار:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // GET /:name/:version/download — تنزيل حزمة
    // ═══════════════════════════════════════════════════════════════
    router.get('/:name/:version/download', (req, res) => {
        try {
            const { name, version } = req.params;
            const ip = req.headers['x-forwarded-for'] || req.ip || '';
            const userAgent = req.headers['user-agent'] || '';

            const result = packageService.download(name, version, ip, userAgent);
            if (!result.success) {
                const status = result.error?.code === 'PKG_007' ? 410 :
                              result.error?.code === 'PKG_008' ? 500 : 404;
                return res.status(status).json(result);
            }

            // إرسال الملف
            res.set('Content-Type', 'application/gzip');
            res.set('Content-Disposition', `attachment; filename="${name}-${version}.tar.gz"`);
            if (result.checksum) {
                res.set('X-Checksum-SHA256', result.checksum);
            }
            res.sendFile(result.filePath);
        } catch (err) {
            console.error('خطأ في التنزيل:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // POST /publish — نشر حزمة جديدة
    // ═══════════════════════════════════════════════════════════════
    router.post('/publish', authenticate(db), publishLimiter, upload.single('package'), (req, res) => {
        try {
            // التحقق من وجود الملف
            if (!req.file) {
                return sendError(res, 'PUB_001');
            }

            // تحليل البيانات الوصفية
            let metadata;
            try {
                metadata = typeof req.body.metadata === 'string'
                    ? JSON.parse(req.body.metadata)
                    : req.body.metadata || req.body;
            } catch {
                return sendError(res, 'PUB_002', {
                    explain: 'حقل metadata يحتوي على JSON غير صالح',
                });
            }

            if (!metadata || (!metadata.name && !metadata.version)) {
                return sendError(res, 'PUB_002');
            }

            // التحقق من اسم الحزمة
            const nameCheck = validatePackageName(metadata.name);
            if (!nameCheck.valid) {
                return sendError(res, 'PKG_005', { explain: nameCheck.reason });
            }

            // التحقق من الإصدار
            const versionCheck = validateVersion(metadata.version);
            if (!versionCheck.valid) {
                return sendError(res, 'PKG_006', { explain: versionCheck.reason });
            }

            const result = packageService.publish(metadata, req.file.path, req.user.id);
            if (!result.success) {
                const errorCode = result.error?.code;
                const status = errorCode === 'PKG_003' ? 409 :
                              errorCode === 'PKG_004' ? 403 :
                              errorCode === 'PKG_006' ? 400 : 400;
                return res.status(status).json(result);
            }

            res.status(201).json(result);
        } catch (err) {
            console.error('خطأ في النشر:', err.message);
            sendError(res, 'GEN_002', {
                explain: process.env.NODE_ENV !== 'production' ? err.message : undefined,
            });
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // DELETE /:name/:version/yank — سحب إصدار
    // ═══════════════════════════════════════════════════════════════
    router.delete('/:name/:version/yank', authenticate(db), (req, res) => {
        try {
            const { name, version } = req.params;

            const result = packageService.yank(name, version, req.user.id);
            if (!result.success) {
                const errorCode = result.error?.code;
                const status = errorCode === 'PKG_001' ? 404 :
                              errorCode === 'PKG_002' ? 404 :
                              errorCode === 'PKG_004' ? 403 : 400;
                return res.status(status).json(result);
            }

            res.json(result);
        } catch (err) {
            console.error('خطأ في السحب:', err.message);
            sendError(res, 'GEN_002');
        }
    });

    return router;
}

module.exports = createPackageRouter;
