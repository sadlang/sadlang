// بسم الله الرحمن الرحيم
/**
 * @file auth.js
 * @description مسارات المصادقة - Authentication routes
 */

const express = require('express');
const router = express.Router();
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const crypto = require('crypto');
const { body, validationResult } = require('express-validator');

const JWT_SECRET = process.env.JWT_SECRET || 'sad-registry-default-secret';
const JWT_EXPIRY = process.env.JWT_EXPIRY || '30d';
const BCRYPT_ROUNDS = parseInt(process.env.BCRYPT_ROUNDS) || 12;

// ============================================================================
// POST /api/v1/auth/register — تسجيل مستخدم جديد / Register
// ============================================================================

router.post('/register', [
    body('username')
        .isLength({ min: 3, max: 50 })
        .matches(/^[\u0600-\u06FFa-zA-Z0-9_-]+$/)
        .withMessage('اسم المستخدم يجب أن يحتوي على 3-50 حرفاً (عربي/إنجليزي/أرقام)'),
    body('email')
        .isEmail()
        .normalizeEmail()
        .withMessage('بريد إلكتروني غير صالح'),
    body('password')
        .isLength({ min: 8 })
        .withMessage('كلمة المرور يجب أن تكون 8 أحرف على الأقل'),
], async (req, res) => {
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
        return res.status(400).json({ 
            error: 'بيانات غير صالحة / Invalid data',
            details: errors.array() 
        });
    }

    const { username, email, password, display_name } = req.body;
    const db = req.app.locals.db;

    try {
        // التحقق من عدم تكرار المستخدم
        const existing = db.prepare(
            'SELECT id FROM users WHERE username = ? OR email = ?'
        ).get(username, email);

        if (existing) {
            return res.status(409).json({
                error: 'اسم المستخدم أو البريد الإلكتروني مسجل مسبقاً',
                error_en: 'Username or email already registered'
            });
        }

        // تشفير كلمة المرور / Hash password
        const passwordHash = await bcrypt.hash(password, BCRYPT_ROUNDS);

        // إنشاء رمز API / Generate API token
        const apiToken = `sad_${crypto.randomBytes(32).toString('hex')}`;

        // إدخال المستخدم / Insert user
        const result = db.prepare(`
            INSERT INTO users (username, email, password_hash, display_name, api_token)
            VALUES (?, ?, ?, ?, ?)
        `).run(username, email, passwordHash, display_name || username, apiToken);

        // إنشاء JWT
        const token = jwt.sign(
            { id: result.lastInsertRowid, username },
            JWT_SECRET,
            { expiresIn: JWT_EXPIRY }
        );

        res.status(201).json({
            success: true,
            message: `مرحباً ${username}! تم إنشاء حسابك بنجاح`,
            message_en: `Welcome ${username}! Account created successfully`,
            user: {
                id: result.lastInsertRowid,
                username,
                email,
                display_name: display_name || username,
            },
            token,
            api_token: apiToken,
        });

    } catch (error) {
        console.error('خطأ في التسجيل:', error.message);
        res.status(500).json({ error: 'فشل التسجيل / Registration failed' });
    }
});

// ============================================================================
// POST /api/v1/auth/login — تسجيل الدخول / Login
// ============================================================================

router.post('/login', [
    body('username').notEmpty().withMessage('اسم المستخدم مطلوب'),
    body('password').notEmpty().withMessage('كلمة المرور مطلوبة'),
], async (req, res) => {
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
        return res.status(400).json({ error: 'بيانات غير صالحة', details: errors.array() });
    }

    const { username, password } = req.body;
    const db = req.app.locals.db;

    try {
        // البحث عن المستخدم (بالاسم أو البريد)
        const user = db.prepare(
            'SELECT * FROM users WHERE (username = ? OR email = ?) AND is_active = 1'
        ).get(username, username);

        if (!user) {
            return res.status(401).json({
                error: 'اسم المستخدم أو كلمة المرور غير صحيحة',
                error_en: 'Invalid username or password'
            });
        }

        // التحقق من كلمة المرور
        const isValid = await bcrypt.compare(password, user.password_hash);
        if (!isValid) {
            return res.status(401).json({
                error: 'اسم المستخدم أو كلمة المرور غير صحيحة',
                error_en: 'Invalid username or password'
            });
        }

        // إنشاء JWT
        const token = jwt.sign(
            { id: user.id, username: user.username },
            JWT_SECRET,
            { expiresIn: JWT_EXPIRY }
        );

        res.json({
            success: true,
            message: `مرحباً مجدداً ${user.display_name || user.username}`,
            user: {
                id: user.id,
                username: user.username,
                email: user.email,
                display_name: user.display_name,
            },
            token,
            api_token: user.api_token,
        });

    } catch (error) {
        console.error('خطأ في تسجيل الدخول:', error.message);
        res.status(500).json({ error: 'فشل تسجيل الدخول / Login failed' });
    }
});

// ============================================================================
// POST /api/v1/auth/token — إنشاء رمز API جديد / Generate new API token
// ============================================================================

router.post('/token', [
    body('name').notEmpty().withMessage('اسم الرمز مطلوب'),
], async (req, res) => {
    // يتطلب JWT صالح في الرأس
    const authHeader = req.headers.authorization;
    if (!authHeader || !authHeader.startsWith('Bearer ')) {
        return res.status(401).json({ error: 'مطلوب تسجيل الدخول' });
    }

    try {
        const decoded = jwt.verify(authHeader.split(' ')[1], JWT_SECRET);
        const { name, scopes } = req.body;
        const db = req.app.locals.db;

        const rawToken = `sad_${crypto.randomBytes(32).toString('hex')}`;
        const tokenHash = crypto.createHash('sha256').update(rawToken).digest('hex');

        db.prepare(`
            INSERT INTO api_tokens (user_id, name, token_hash, scopes)
            VALUES (?, ?, ?, ?)
        `).run(decoded.id, name, tokenHash, JSON.stringify(scopes || ['publish']));

        res.status(201).json({
            success: true,
            message: 'تم إنشاء رمز API جديد',
            token: rawToken,
            warning: 'احفظ هذا الرمز! لن يظهر مرة أخرى / Save this token! It won\'t be shown again'
        });

    } catch (error) {
        res.status(401).json({ error: 'رمز غير صالح / Invalid token' });
    }
});

module.exports = router;
