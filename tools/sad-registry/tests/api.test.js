// بسم الله الرحمن الرحيم
/**
 * @file api.test.js
 * @description اختبارات API لمستودع حزم لغة ص
 */

const request = require('supertest');
const path = require('path');
const fs = require('fs');
const { app } = require('../src/server');
const { initDatabase } = require('../src/models/database');

// ============================================
// إعداد الاختبارات / Test Setup
// ============================================

const TEST_DB = path.join(__dirname, '..', 'data', 'test-registry.db');

let db;
let authToken;
let apiToken;

beforeAll(async () => {
    // حذف قاعدة البيانات الاختبارية القديمة
    if (fs.existsSync(TEST_DB)) {
        fs.unlinkSync(TEST_DB);
    }

    process.env.DB_PATH = TEST_DB;
    process.env.JWT_SECRET = 'test-secret-key';
    process.env.STORAGE_PATH = path.join(__dirname, '..', 'storage', 'test-packages');

    db = initDatabase(TEST_DB);
    app.locals.db = db;
});

afterAll(() => {
    if (db) db.close();
    // تنظيف
    if (fs.existsSync(TEST_DB)) {
        fs.unlinkSync(TEST_DB);
    }
});

// ============================================
// اختبارات المعلومات / Info Tests
// ============================================

describe('GET /api/v1', () => {
    it('يرجع معلومات API', async () => {
        const res = await request(app).get('/api/v1');
        expect(res.status).toBe(200);
        expect(res.body.api_version).toBe('v1');
        expect(res.body.endpoints).toBeDefined();
    });
});

// ============================================
// اختبارات المصادقة / Auth Tests
// ============================================

describe('POST /api/v1/auth/register', () => {
    it('ينشئ مستخدم جديد', async () => {
        const res = await request(app)
            .post('/api/v1/auth/register')
            .send({
                username: 'test-user',
                email: 'test@example.com',
                password: 'password123',
                display_name: 'مستخدم اختباري'
            });

        expect(res.status).toBe(201);
        expect(res.body.success).toBe(true);
        expect(res.body.token).toBeDefined();
        expect(res.body.api_token).toBeDefined();

        authToken = res.body.token;
        apiToken = res.body.api_token;
    });

    it('يرفض تسجيل مكرر', async () => {
        const res = await request(app)
            .post('/api/v1/auth/register')
            .send({
                username: 'test-user',
                email: 'test@example.com',
                password: 'password123',
            });

        expect(res.status).toBe(409);
    });

    it('يرفض كلمة مرور قصيرة', async () => {
        const res = await request(app)
            .post('/api/v1/auth/register')
            .send({
                username: 'short-pass',
                email: 'short@example.com',
                password: '123',
            });

        expect(res.status).toBe(400);
    });
});

describe('POST /api/v1/auth/login', () => {
    it('يسجل الدخول بنجاح', async () => {
        const res = await request(app)
            .post('/api/v1/auth/login')
            .send({
                username: 'test-user',
                password: 'password123',
            });

        expect(res.status).toBe(200);
        expect(res.body.token).toBeDefined();
    });

    it('يرفض كلمة مرور خاطئة', async () => {
        const res = await request(app)
            .post('/api/v1/auth/login')
            .send({
                username: 'test-user',
                password: 'wrong-password',
            });

        expect(res.status).toBe(401);
    });
});

// ============================================
// اختبارات البحث / Search Tests
// ============================================

describe('GET /api/v1/search', () => {
    it('يبحث في الحزم (نتائج فارغة)', async () => {
        const res = await request(app)
            .get('/api/v1/search?q=nonexistent');

        expect(res.status).toBe(200);
        expect(res.body.packages).toBeInstanceOf(Array);
        expect(res.body.total).toBeDefined();
    });

    it('يبحث بدون معاملات', async () => {
        const res = await request(app)
            .get('/api/v1/search');

        expect(res.status).toBe(200);
        expect(res.body.packages).toBeInstanceOf(Array);
    });
});

// ============================================
// اختبارات الحزم / Package Tests  
// ============================================

describe('GET /api/v1/packages/:name', () => {
    it('يرجع 404 لحزمة غير موجودة', async () => {
        const res = await request(app)
            .get('/api/v1/packages/nonexistent-package');

        expect(res.status).toBe(404);
    });
});

describe('GET /api/v1/packages/:name/versions', () => {
    it('يرجع 404 لحزمة غير موجودة', async () => {
        const res = await request(app)
            .get('/api/v1/packages/nonexistent/versions');

        expect(res.status).toBe(404);
    });
});

// ============================================
// اختبارات حزم المستخدم / User Packages Tests
// ============================================

describe('GET /api/v1/user/packages', () => {
    it('يرفض بدون مصادقة', async () => {
        const res = await request(app)
            .get('/api/v1/user/packages');

        expect(res.status).toBe(401);
    });

    it('يرجع حزم المستخدم مع JWT', async () => {
        const res = await request(app)
            .get('/api/v1/user/packages')
            .set('Authorization', `Bearer ${authToken}`);

        expect(res.status).toBe(200);
        expect(res.body.packages).toBeInstanceOf(Array);
    });

    it('يرجع حزم المستخدم مع API token', async () => {
        const res = await request(app)
            .get('/api/v1/user/packages')
            .set('Authorization', `Bearer ${apiToken}`);

        expect(res.status).toBe(200);
        expect(res.body.packages).toBeInstanceOf(Array);
    });
});

// ============================================
// اختبارات النشر / Publish Tests
// ============================================

describe('POST /api/v1/packages/publish', () => {
    it('يرفض بدون مصادقة', async () => {
        const res = await request(app)
            .post('/api/v1/packages/publish');

        expect(res.status).toBe(401);
    });

    it('يرفض بدون ملف', async () => {
        const res = await request(app)
            .post('/api/v1/packages/publish')
            .set('Authorization', `Bearer ${authToken}`)
            .field('metadata', JSON.stringify({
                name: 'test-pkg',
                version: '1.0.0',
            }));

        expect(res.status).toBe(400);
    });
});

// ============================================
// اختبارات الإحصائيات / Stats Tests
// ============================================

describe('GET /api/v1/stats', () => {
    it('يرجع إحصائيات المستودع', async () => {
        const res = await request(app)
            .get('/api/v1/stats');

        expect(res.status).toBe(200);
        expect(res.body.summary).toBeDefined();
        expect(res.body.summary.total_packages).toBeDefined();
        expect(res.body.summary.total_users).toBeDefined();
        expect(res.body.categories).toBeInstanceOf(Array);
    });
});

// ============================================
// اختبارات الواجهة / Web Tests
// ============================================

describe('GET / (Web)', () => {
    it('يرجع الصفحة الرئيسية', async () => {
        const res = await request(app).get('/');
        expect(res.status).toBe(200);
        expect(res.text).toContain('حزم');
    });
});

describe('GET /search (Web)', () => {
    it('يرجع صفحة البحث', async () => {
        const res = await request(app).get('/search?q=test');
        expect(res.status).toBe(200);
    });
});
