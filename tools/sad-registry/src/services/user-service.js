// بسم الله الرحمن الرحيم
// =========================================================================
// خدمة المستخدمين — إدارة الحسابات والمصادقة
// User Service — Account management and authentication
// =========================================================================
//
// هذه الخدمة تتعامل مع:
//   1. تسجيل المستخدمين الجدد
//   2. تسجيل الدخول (بالاسم أو البريد الإلكتروني)
//   3. التحقق من رموز JWT و API tokens
//   4. إدارة الملف الشخصي
//   5. إنشاء رموز API جديدة
//
// الأمان:
//   - كلمات المرور تُشفر بـ bcrypt (12 جولة افتراضياً)
//   - رموز API تُولد بـ crypto.randomBytes (256 بت)
//   - JWT يُوقع بسر خاص ومدة صلاحية محددة
//   - رموز API المتقدمة تُخزن كـ hash (SHA-256) وليس كنص صريح
// =========================================================================

const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const crypto = require('crypto');

// =========================================================================
// ثوابت الإعداد / Configuration Constants
// =========================================================================

/** سر JWT — يجب تغييره في الإنتاج */
const JWT_SECRET = process.env.JWT_SECRET || 'sad-registry-default-secret';

/** مدة صلاحية JWT (افتراضي: 30 يوم) */
const JWT_EXPIRY = process.env.JWT_EXPIRY || '30d';

/** عدد جولات bcrypt (كلما زاد = أبطأ لكن أكثر أماناً) */
const BCRYPT_ROUNDS = parseInt(process.env.BCRYPT_ROUNDS) || 12;

// =========================================================================
// فئة خدمة المستخدمين / UserService Class
// =========================================================================

class UserService {
    /**
     * إنشاء خدمة المستخدمين
     * 
     * @param {import('better-sqlite3').Database} db — كائن قاعدة بيانات SQLite
     */
    constructor(db) {
        this.db = db;
    }

    // =====================================================================
    // تسجيل مستخدم جديد / Register New User
    // =====================================================================
    //
    // الخطوات:
    //   1. التحقق من عدم تكرار اسم المستخدم أو البريد
    //   2. تشفير كلمة المرور بـ bcrypt
    //   3. توليد رمز API فريد
    //   4. إدخال المستخدم في قاعدة البيانات
    //   5. توليد JWT
    //
    // @param {{ username, email, password, display_name? }} data
    // @returns {{ success, user?, token?, api_token?, error? }}
    // =====================================================================

    async register({ username, email, password, display_name }) {
        // التحقق من عدم التكرار
        const existing = this.db.prepare(
            'SELECT id FROM users WHERE username = ? OR email = ?'
        ).get(username, email);

        if (existing) {
            return {
                success: false,
                error: 'اسم المستخدم أو البريد الإلكتروني مسجل مسبقاً',
                code: 409,
            };
        }

        // تشفير كلمة المرور
        // bcrypt مع 12 جولة يأخذ ~0.3 ثانية — توازن جيد بين الأمان والسرعة
        const passwordHash = await bcrypt.hash(password, BCRYPT_ROUNDS);

        // توليد رمز API
        // البادئة sad_ تسهل التعرف على الرمز ومنع تسريبه في السجلات
        const apiToken = `sad_${crypto.randomBytes(32).toString('hex')}`;

        // إدخال المستخدم
        const result = this.db.prepare(`
            INSERT INTO users (username, email, password_hash, display_name, api_token)
            VALUES (?, ?, ?, ?, ?)
        `).run(username, email, passwordHash, display_name || username, apiToken);

        // توليد JWT
        const token = jwt.sign(
            { id: result.lastInsertRowid, username },
            JWT_SECRET,
            { expiresIn: JWT_EXPIRY }
        );

        return {
            success: true,
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

    // =====================================================================
    // تسجيل الدخول / Login
    // =====================================================================
    //
    // يقبل اسم المستخدم أو البريد الإلكتروني.
    // يقارن كلمة المرور بالـ hash المخزن.
    //
    // @param {{ username, password }} data — اسم/بريد + كلمة المرور
    // @returns {{ success, user?, token?, api_token?, error? }}
    // =====================================================================

    async login({ username, password }) {
        // البحث بالاسم أو البريد
        const user = this.db.prepare(
            'SELECT * FROM users WHERE (username = ? OR email = ?) AND is_active = 1'
        ).get(username, username);

        if (!user) {
            return {
                success: false,
                error: 'اسم المستخدم أو كلمة المرور غير صحيحة',
                code: 401,
            };
        }

        // مقارنة كلمة المرور
        const isValid = await bcrypt.compare(password, user.password_hash);
        if (!isValid) {
            return {
                success: false,
                error: 'اسم المستخدم أو كلمة المرور غير صحيحة',
                code: 401,
            };
        }

        // توليد JWT جديد
        const token = jwt.sign(
            { id: user.id, username: user.username },
            JWT_SECRET,
            { expiresIn: JWT_EXPIRY }
        );

        return {
            success: true,
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

    // =====================================================================
    // جلب الملف الشخصي / Get Profile
    // =====================================================================
    //
    // يرجع معلومات المستخدم مع إحصائياته (عدد الحزم والتنزيلات).
    //
    // @param {number} userId — معرف المستخدم
    // @returns {Object|null}
    // =====================================================================

    getProfile(userId) {
        const user = this.db.prepare(`
            SELECT id, username, email, display_name, bio, avatar_url, created_at
            FROM users WHERE id = ?
        `).get(userId);

        if (!user) return null;

        const packageCount = this.db.prepare(
            'SELECT COUNT(*) as count FROM packages WHERE owner_id = ?'
        ).get(userId);

        const totalDownloads = this.db.prepare(
            'SELECT COALESCE(SUM(total_downloads), 0) as total FROM packages WHERE owner_id = ?'
        ).get(userId);

        return {
            ...user,
            stats: {
                packages: packageCount.count,
                total_downloads: totalDownloads.total,
            }
        };
    }

    // =====================================================================
    // جلب حزم المستخدم / Get User Packages
    // =====================================================================
    //
    // يرجع كل الحزم التي يملكها المستخدم (كمالك أصلي أو مشارك).
    //
    // @param {number} userId — معرف المستخدم
    // @returns {Array}
    // =====================================================================

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

    // =====================================================================
    // إنشاء رمز API جديد / Create New API Token
    // =====================================================================
    //
    // لماذا رموز متعددة؟
    //   - رمز لـ CI/CD (صلاحية نشر فقط)
    //   - رمز لسطح المكتب (كل الصلاحيات)
    //   - إلغاء رمز واحد بدون التأثير على الآخرين
    //
    // الأمان:
    //   - الرمز يُعرض مرة واحدة فقط عند الإنشاء
    //   - يُخزن كـ SHA-256 hash في قاعدة البيانات
    //   - عند المصادقة، يُحسب hash الرمز المرسل ويُقارن
    //
    // @param {number} userId — معرف المستخدم
    // @param {string} name — اسم وصفي للرمز
    // @param {string[]} scopes — الصلاحيات (مثلاً: ["publish"])
    // @returns {{ success, token?, error? }}
    // =====================================================================

    createApiToken(userId, name, scopes = ['publish']) {
        // توليد رمز عشوائي قوي
        const rawToken = `sad_${crypto.randomBytes(32).toString('hex')}`;

        // تخزين hash فقط (لا نخزن الرمز الأصلي أبداً)
        const tokenHash = crypto.createHash('sha256').update(rawToken).digest('hex');

        this.db.prepare(`
            INSERT INTO api_tokens (user_id, name, token_hash, scopes)
            VALUES (?, ?, ?, ?)
        `).run(userId, name, tokenHash, JSON.stringify(scopes));

        return {
            success: true,
            token: rawToken,
            warning: 'احفظ هذا الرمز! لن يظهر مرة أخرى',
        };
    }
}

module.exports = UserService;
