// بسم الله الرحمن الرحيم
/**
 * @file server.js
 * @description خادم مستودع حزم لغة ص - Sad Language Package Registry Server
 * 
 * الخادم الرئيسي الذي يوفر:
 * - API RESTful لإدارة الحزم (بحث، تنزيل، نشر)
 * - واجهة ويب عربية لتصفح الحزم
 * - نظام مصادقة JWT
 * - تخزين الحزم والبيانات الوصفية
 * 
 * @author فريق لغة ص
 * @date فبراير 2026
 */

require('dotenv').config();

const express = require('express');
const helmet = require('helmet');
const cors = require('cors');
const compression = require('compression');
const morgan = require('morgan');
const path = require('path');
const fs = require('fs');

const { initDatabase } = require('./models/database');
const apiRoutes = require('./routes/api');
const webRoutes = require('./routes/web');
const { errorHandler, notFoundHandler } = require('./middleware/errors');
const { generalLimiter } = require('./middleware/rate-limit');

// ============================================================================
// إعداد التطبيق / Application Setup
// ============================================================================

const app = express();
const PORT = process.env.PORT || 3000;
const HOST = process.env.HOST || '0.0.0.0';

// إنشاء المجلدات الضرورية / Create necessary directories
const dirs = [
    process.env.STORAGE_PATH || './storage/packages',
    './data',
    './logs'
];
dirs.forEach(dir => {
    if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
    }
});

// ============================================================================
// البرمجيات الوسيطة / Middleware
// ============================================================================

// الأمان / Security
app.use(helmet({
    contentSecurityPolicy: {
        directives: {
            defaultSrc: ["'self'"],
            styleSrc: ["'self'", "'unsafe-inline'", "https://fonts.googleapis.com"],
            fontSrc: ["'self'", "https://fonts.gstatic.com"],
            scriptSrc: ["'self'", "'unsafe-inline'"],
            imgSrc: ["'self'", "data:", "https:"],
        },
    },
}));

// CORS - مشاركة الموارد عبر الأصول
app.use(cors({
    origin: process.env.NODE_ENV === 'production' 
        ? process.env.REGISTRY_URL 
        : '*',
    methods: ['GET', 'POST', 'PUT', 'DELETE'],
    allowedHeaders: ['Content-Type', 'Authorization'],
}));

// الضغط / Compression
app.use(compression());

// تحليل الجسم / Body Parsing
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ extended: true, limit: '10mb' }));

// التسجيل / Logging
if (process.env.NODE_ENV !== 'test') {
    app.use(morgan('combined'));
}

// الحد من المعدل / Rate Limiting
app.use(generalLimiter);

// الملفات الثابتة / Static Files
app.use(express.static(path.join(__dirname, '..', 'public')));

// محرك القوالب / Template Engine
app.set('view engine', 'ejs');
app.set('views', path.join(__dirname, '..', 'views'));

// ============================================================================
// المسارات / Routes
// ============================================================================

// API v1 - واجهة البرمجة
app.use('/api/v1', apiRoutes);

// واجهة الويب / Web Interface
app.use('/', webRoutes);

// معالجة الأخطاء / Error Handling
app.use(notFoundHandler);
app.use(errorHandler);

// ============================================================================
// بدء الخادم / Start Server
// ============================================================================

async function startServer() {
    try {
        // تهيئة قاعدة البيانات / Initialize Database
        const db = initDatabase();
        app.locals.db = db;

        console.log('');
        console.log('╔══════════════════════════════════════════════════════════╗');
        console.log('║                                                          ║');
        console.log('║   سجل حزم لغة ص - Sad Language Package Registry         ║');
        console.log('║                                                          ║');
        console.log('╚══════════════════════════════════════════════════════════╝');
        console.log('');

        app.listen(PORT, HOST, () => {
            console.log(`  ✅ الخادم يعمل / Server running`);
            console.log(`     📍 http://${HOST}:${PORT}`);
            console.log(`     📦 API: http://${HOST}:${PORT}/api/v1`);
            console.log(`     🌐 Web: http://${HOST}:${PORT}`);
            console.log(`     📁 DB:  ${process.env.DB_PATH || './data/registry.db'}`);
            console.log('');
        });

    } catch (error) {
        console.error('❌ فشل بدء الخادم / Server start failed:', error.message);
        process.exit(1);
    }
}

// التشغيل / Run
if (require.main === module) {
    startServer();
}

module.exports = { app, startServer };
