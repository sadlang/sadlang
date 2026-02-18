// بسم الله الرحمن الرحيم
// =========================================================================
// برمجية وسيطة للمصادقة — Authentication Middleware
// =========================================================================
//
// @file auth.js (middleware)
//
// الوظيفة:
//   التحقق من هوية المستخدم قبل السماح بالوصول إلى نقاط النهاية المحمية.
//
// آليات المصادقة المدعومة (حسب الأولوية):
//   ┌─────────────────────────────────────────────────────────────┐
//   │ 1. JWT Bearer Token                                        │
//   │    الرأس: Authorization: Bearer <jwt_token>                │
//   │    يُستخدم من: واجهة الويب، التطبيقات العميلة             │
//   │    الصلاحية: حسب JWT_EXPIRY (افتراضي 30 يوم)              │
//   ├─────────────────────────────────────────────────────────────┤
//   │ 2. API Token (البسيط)                                      │
//   │    الرأس: Authorization: Bearer sad_xxxxxxxxxx             │
//   │    يُستخدم من: sad-pkg CLI، سكربتات CI/CD                 │
//   │    مُخزن كنص صريح في عمود api_token بجدول users            │
//   ├─────────────────────────────────────────────────────────────┤
//   │ 3. API Token (المتقدم / المجزّأ)                           │
//   │    الرأس: نفس الصيغة                                      │
//   │    مُخزن كـ SHA-256 hash في جدول api_tokens                │
//   │    يدعم: صلاحيات محددة (scopes)، انتهاء صلاحية             │
//   └─────────────────────────────────────────────────────────────┘
//
// تصدير:
//   - authenticate(req, res, next): مصادقة إلزامية (401 عند الفشل)
//   - optionalAuth(req, res, next): مصادقة اختيارية (null عند الفشل)
//
// الاستخدام في المسارات:
//   router.get('/private', authenticate, handler);
//   router.get('/mixed', optionalAuth, handler);
// =========================================================================

const jwt = require('jsonwebtoken');
const crypto = require('crypto');

// ─── ثوابت ─────────────────────────────────────────────────────────────

/** سر توقيع JWT — يجب تغييره في بيئة الإنتاج! */
const JWT_SECRET = process.env.JWT_SECRET || 'sad-registry-default-secret';

// ─── المصادقة الإلزامية ────────────────────────────────────────────────

/**
 * مصادقة إلزامية / Required Authentication
 *
 * تتحقق من رأس Authorization وتضع كائن المستخدم في req.user.
 * ترفض الطلب بـ 401 إذا لم يتوفر رمز صالح.
 *
 * @param {import('express').Request} req — الطلب (يُقرأ منه Authorization header)
 * @param {import('express').Response} res — الاستجابة (يُرسل 401 عند الفشل)
 * @param {import('express').NextFunction} next — الانتقال للمعالج التالي
 *
 * عند النجاح يُضاف:
 *   req.user = { id, username, is_admin }
 *   req.tokenScopes = [...] (فقط مع API tokens المتقدمة)
 */
function authenticate(req, res, next) {
    const authHeader = req.headers.authorization;

    if (!authHeader) {
        return res.status(401).json({
            error: 'مطلوب تسجيل الدخول / Authentication required',
            hint: 'أضف رأس Authorization: Bearer <token>'
        });
    }

    const [scheme, token] = authHeader.split(' ');
    
    if (!token) {
        return res.status(401).json({ error: 'رمز غير صالح / Invalid token format' });
    }

    const db = req.app.locals.db;

    // محاولة 1: JWT Token
    if (scheme === 'Bearer') {
        try {
            const decoded = jwt.verify(token, JWT_SECRET);
            const user = db.prepare('SELECT id, username, is_admin FROM users WHERE id = ? AND is_active = 1').get(decoded.id);
            if (!user) {
                return res.status(401).json({ error: 'المستخدم غير موجود أو معطل' });
            }
            req.user = user;
            return next();
        } catch (jwtError) {
            // إذا فشل JWT، حاول كرمز API
        }
    }

    // محاولة 2: API Token (sad_xxxxx)
    const rawToken = scheme === 'Bearer' ? token : `${scheme} ${token}`.trim();
    
    if (rawToken.startsWith('sad_')) {
        const user = db.prepare(`
            SELECT id, username, is_admin FROM users WHERE api_token = ? AND is_active = 1
        `).get(rawToken);

        if (user) {
            req.user = user;
            return next();
        }

        // حاول في جدول الرموز المتقدم
        const tokenHash = crypto.createHash('sha256').update(rawToken).digest('hex');
        const tokenRecord = db.prepare(`
            SELECT t.user_id, t.scopes, u.username, u.is_admin
            FROM api_tokens t
            JOIN users u ON t.user_id = u.id
            WHERE t.token_hash = ? AND u.is_active = 1
              AND (t.expires_at IS NULL OR t.expires_at > datetime('now'))
        `).get(tokenHash);

        if (tokenRecord) {
            db.prepare('UPDATE api_tokens SET last_used_at = datetime("now") WHERE token_hash = ?').run(tokenHash);
            req.user = { id: tokenRecord.user_id, username: tokenRecord.username, is_admin: tokenRecord.is_admin };
            req.tokenScopes = JSON.parse(tokenRecord.scopes || '[]');
            return next();
        }
    }

    return res.status(401).json({
        error: 'رمز المصادقة غير صالح أو منتهي الصلاحية',
        error_en: 'Invalid or expired authentication token'
    });
}

/**
 * مصادقة اختيارية / Optional Authentication
 */
function optionalAuth(req, res, next) {
    const authHeader = req.headers.authorization;
    if (!authHeader) {
        req.user = null;
        return next();
    }

    // محاولة المصادقة بدون رفض
    authenticate(req, res, (err) => {
        if (err) {
            req.user = null;
        }
        next();
    });
}

module.exports = { authenticate, optionalAuth };
