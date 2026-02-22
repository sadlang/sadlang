// بسم الله الرحمن الرحيم
// =========================================================================
//  سجل حزم لغة ص — الخادم الرئيسي (الإصدار 2.0)
//  Sad Language Package Registry — Main Server (v2.0)
// =========================================================================
//
//  ميزات الإصدار 2.0:
//    ✓ نظام أخطاء شامل مع شرح تفصيلي وحلول مقترحة
//    ✓ رقم طلب فريد (request_id) لتتبع المشاكل
//    ✓ قياس زمن الاستجابة لكل طلب
//    ✓ فحص صحة الخادم (health check)
//    ✓ قائمة رموز الأخطاء للمطورين
//    ✓ تقييد معدل الطلبات مع رسائل واضحة
//    ✓ معالجة JSON التالف بأخطاء مفهومة
//    ✓ معالجة أخطاء رفع الملفات
//    ✓ دعم كامل لـ UTF-8 والعربية
//
//  ملاحظة: يتطلب تشغيل الخادم الخيار --experimental-sqlite
//  node --experimental-sqlite src/server.js
//
// =========================================================================

require('dotenv').config();

const express = require('express');
const helmet = require('helmet');
const cors = require('cors');
const compression = require('compression');
const morgan = require('morgan');
const crypto = require('crypto');
const path = require('path');

// ─── إعداد قاعدة البيانات ───
const { initializeDatabase } = require('./models/database');
const db = initializeDatabase();

// ─── إعداد الخدمات ───
const UserService = require('./services/user-service');
const PackageService = require('./services/package-service');
const StatsService = require('./services/stats-service');

const storagePath = process.env.STORAGE_PATH || './storage/packages';
const services = {
    userService: new UserService(db),
    packageService: new PackageService(db, storagePath),
    statsService: new StatsService(db),
};

// ─── إعداد Express ───
const app = express();

// ─── Middleware: الأمان ───
app.use(helmet({
    contentSecurityPolicy: false, // مرن للصفحات الويب
}));

// ─── Middleware: CORS ───
app.use(cors({
    origin: '*',
    methods: ['GET', 'POST', 'PUT', 'DELETE', 'OPTIONS'],
    allowedHeaders: ['Content-Type', 'Authorization', 'X-Request-ID'],
    exposedHeaders: ['X-Checksum-SHA256', 'X-Request-ID', 'X-Response-Time'],
}));

// ─── Middleware: ضغط الاستجابات ───
app.use(compression());

// ─── Middleware: رقم طلب فريد ───
app.use((req, res, next) => {
    req.requestId = req.headers['x-request-id'] || crypto.randomBytes(8).toString('hex');
    res.set('X-Request-ID', req.requestId);
    next();
});

// ─── Middleware: قياس زمن الاستجابة ───
app.use((req, res, next) => {
    const start = process.hrtime.bigint();
    // نحفظ الدالة الأصلية ونضيف الترويسة قبل الإرسال
    const originalWriteHead = res.writeHead;
    res.writeHead = function (...args) {
        const duration = Number(process.hrtime.bigint() - start) / 1e6;
        this.setHeader('X-Response-Time', `${duration.toFixed(2)}ms`);
        return originalWriteHead.apply(this, args);
    };
    next();
});

// ─── Middleware: تسجيل الطلبات ───
morgan.token('request-id', (req) => req.requestId);
morgan.token('user', (req) => req.user?.username || '-');
app.use(morgan(':request-id :method :url :status :response-time[0]ms :user', {
    skip: (req) => req.path === '/api/v1/health', // لا نسجل طلبات health
}));

// ─── Middleware: تحليل JSON ───
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ extended: true, limit: '10mb' }));

// ─── تقييد معدل الطلبات ───
const { generalLimiter } = require('./middleware/rate-limit');
app.use('/api/', generalLimiter);

// ─── معالج أخطاء JSON ───
const { jsonErrorHandler, uploadErrorHandler, notFoundHandler, globalErrorHandler } = require('./middleware/errors');
app.use(jsonErrorHandler);

// ─── القوالب (EJS) ───
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, '..', 'views'));

// ─── الملفات الثابتة ───
app.use('/public', express.static(path.join(__dirname, '..', 'public')));
app.use('/web', express.static(path.join(__dirname, '..', 'web')));

// ═══════════════════════════════════════════════════════════════
//  المسارات
// ═══════════════════════════════════════════════════════════════

// API v1
const createApiRouter = require('./routes/api');
app.use('/api/v1', createApiRouter(db, services));

// صفحات الويب
const createWebRouter = require('./routes/web');
app.use('/', createWebRouter(db, services));

// ═══════════════════════════════════════════════════════════════
//  معالجة الأخطاء (آخر middleware)
// ═══════════════════════════════════════════════════════════════
app.use(uploadErrorHandler);
app.use(notFoundHandler);
app.use(globalErrorHandler);

// ═══════════════════════════════════════════════════════════════
//  تشغيل الخادم
// ═══════════════════════════════════════════════════════════════
function startServer() {
    const PORT = parseInt(process.env.PORT) || 3000;
    const HOST = process.env.HOST || '0.0.0.0';

    const server = app.listen(PORT, HOST, () => {
        console.log('');
        console.log('╔══════════════════════════════════════════════════════════════╗');
        console.log('║        سجل حزم لغة ص — الإصدار 2.0                         ║');
        console.log('║        Sad Package Registry — v2.0                          ║');
        console.log('╠══════════════════════════════════════════════════════════════╣');
        console.log(`║  العنوان:  http://${HOST}:${PORT}`);
        console.log(`║  API:      http://${HOST}:${PORT}/api/v1/`);
        console.log(`║  الصحة:    http://${HOST}:${PORT}/api/v1/health`);
        console.log(`║  الأخطاء:  http://${HOST}:${PORT}/api/v1/errors`);
        console.log(`║  البيئة:   ${process.env.NODE_ENV || 'development'}`);
        console.log('╠══════════════════════════════════════════════════════════════╣');

        // إحصائيات سريعة
        try {
            const stats = services.statsService.getSummary();
            console.log(`║  الحزم: ${stats.total_packages} | المستخدمين: ${stats.total_users} | الإصدارات: ${stats.total_versions}`);
        } catch {
            console.log('║  (لا يمكن قراءة الإحصائيات)');
        }

        console.log('╚══════════════════════════════════════════════════════════════╝');
        console.log('');
    });

    // معالجة إغلاق الخادم بنظافة
    process.on('SIGINT', () => {
        console.log('\n⏹ جاري إيقاف الخادم...');
        server.close(() => {
            db.close();
            console.log('✓ تم إيقاف الخادم وإغلاق قاعدة البيانات');
            process.exit(0);
        });
    });

    process.on('SIGTERM', () => {
        server.close(() => {
            db.close();
            process.exit(0);
        });
    });

    // معالجة الأخطاء غير المتوقعة
    process.on('uncaughtException', (err) => {
        console.error('╔═ خطأ غير متوقع ═══════════════════════════════════════╗');
        console.error(`║ ${err.message}`);
        console.error(`║ ${err.stack?.split('\n')[1] || ''}`);
        console.error('╚═══════════════════════════════════════════════════════════╝');
    });

    process.on('unhandledRejection', (reason) => {
        console.error('╔═ Promise مرفوض ════════════════════════════════════════╗');
        console.error(`║ ${reason}`);
        console.error('╚═══════════════════════════════════════════════════════════╝');
    });

    return server;
}

// تشغيل مباشر أو تصدير
if (require.main === module) {
    startServer();
} else {
    module.exports = { app, startServer, db, services };
}
