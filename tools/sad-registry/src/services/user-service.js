// بسم الله الرحمن الرحيم
// =========================================================================
// خدمة المستخدمين — إدارة الحسابات والمصادقة
// User Service — Account management and authentication
// =========================================================================

const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const crypto = require('crypto');
const { createError } = require('../utils/error-codes');

const JWT_SECRET = process.env.JWT_SECRET || 'sad-registry-default-secret-change-me';
const JWT_EXPIRY = process.env.JWT_EXPIRY || '30d';
const BCRYPT_ROUNDS = parseInt(process.env.BCRYPT_ROUNDS) || 12;

class UserService {
    constructor(db) {
        this.db = db;

        // إعداد الاستعلامات المُحضّرة
        this._getUserByUsername = db.prepare(
            'SELECT * FROM users WHERE (username = ? OR email = ?) AND is_active = 1'
        );
        this._getUserById = db.prepare(
            'SELECT id, username, email, display_name, bio, avatar_url, created_at FROM users WHERE id = ?'
        );
        this._checkExistingUsername = db.prepare(
            'SELECT id FROM users WHERE username = ?'
        );
        this._checkExistingEmail = db.prepare(
            'SELECT id FROM users WHERE email = ?'
        );
    }

    /**
     * تسجيل مستخدم جديد
     */
    async register({ username, email, password, display_name }) {
        // التحقق من وجود اسم المستخدم
        const existingUsername = this._checkExistingUsername.get(username);
        if (existingUsername) {
            return createError('REG_001');
        }

        // التحقق من وجود البريد
        const existingEmail = this._checkExistingEmail.get(email);
        if (existingEmail) {
            return createError('REG_002');
        }

        // تشفير كلمة المرور
        const passwordHash = await bcrypt.hash(password, BCRYPT_ROUNDS);
        const apiToken = `sad_${crypto.randomBytes(32).toString('hex')}`;

        const result = this.db.prepare(`
            INSERT INTO users (username, email, password_hash, display_name, api_token)
            VALUES (?, ?, ?, ?, ?)
        `).run(username, email, passwordHash, display_name || username, apiToken);

        // إنشاء JWT
        const token = jwt.sign(
            { id: result.lastInsertRowid, username },
            JWT_SECRET,
            { expiresIn: JWT_EXPIRY }
        );

        return {
            success: true,
            message: 'تم إنشاء الحساب بنجاح',
            message_en: 'Account created successfully',
            user: {
                id: result.lastInsertRowid,
                username,
                email,
                display_name: display_name || username,
            },
            token,
            api_token: apiToken,
        };
    }

    /**
     * تسجيل الدخول
     */
    async login({ username, password }) {
        const user = this._getUserByUsername.get(username, username);

        if (!user) {
            return createError('AUTH_004');
        }

        if (!user.is_active) {
            return createError('AUTH_005');
        }

        const isValid = await bcrypt.compare(password, user.password_hash);
        if (!isValid) {
            return createError('AUTH_004');
        }

        const token = jwt.sign(
            { id: user.id, username: user.username },
            JWT_SECRET,
            { expiresIn: JWT_EXPIRY }
        );

        return {
            success: true,
            message: 'تم تسجيل الدخول بنجاح',
            message_en: 'Login successful',
            user: {
                id: user.id,
                username: user.username,
                email: user.email,
                display_name: user.display_name,
            },
            token,
            api_token: user.api_token,
        };
    }

    /**
     * الحصول على ملف المستخدم الشخصي
     */
    getProfile(userId) {
        const user = this._getUserById.get(userId);
        if (!user) return null;

        const packageCount = this.db.prepare(
            'SELECT COUNT(*) as count FROM packages WHERE owner_id = ?'
        ).get(userId);

        const totalDownloads = this.db.prepare(
            'SELECT COALESCE(SUM(total_downloads), 0) as total FROM packages WHERE owner_id = ?'
        ).get(userId);

        return {
            success: true,
            user: {
                ...user,
                stats: {
                    packages: packageCount.count,
                    total_downloads: totalDownloads.total,
                },
            },
        };
    }

    /**
     * حزم المستخدم
     */
    getUserPackages(userId) {
        return this.db.prepare(`
            SELECT DISTINCT p.name, p.description, p.description_ar, p.latest_version,
                   p.total_downloads, p.license, p.created_at, p.updated_at
            FROM packages p
            LEFT JOIN package_owners po ON p.id = po.package_id
            WHERE p.owner_id = ? OR po.user_id = ?
            ORDER BY p.updated_at DESC
        `).all(userId, userId);
    }

    /**
     * إنشاء رمز API جديد
     */
    createApiToken(userId, name, scopes = ['publish']) {
        if (!name || typeof name !== 'string' || name.trim().length === 0) {
            return createError('TKN_001');
        }

        const validScopes = ['publish', 'read', 'admin'];
        const invalidScopes = scopes.filter(s => !validScopes.includes(s));
        if (invalidScopes.length > 0) {
            return createError('TKN_002', {
                details: { invalid: invalidScopes, valid: validScopes },
            });
        }

        const rawToken = `sad_${crypto.randomBytes(32).toString('hex')}`;
        const tokenHash = crypto.createHash('sha256').update(rawToken).digest('hex');

        this.db.prepare(`
            INSERT INTO api_tokens (user_id, name, token_hash, scopes)
            VALUES (?, ?, ?, ?)
        `).run(userId, name.trim(), tokenHash, JSON.stringify(scopes));

        return {
            success: true,
            message: 'تم إنشاء رمز API بنجاح',
            message_en: 'API token created successfully',
            token: rawToken,
            warning: 'احفظ هذا الرمز الآن! لن يظهر مرة أخرى',
            warning_en: 'Save this token now! It will not be shown again',
        };
    }
}

module.exports = UserService;
