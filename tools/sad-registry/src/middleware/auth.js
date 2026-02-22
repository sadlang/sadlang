// بسم الله الرحمن الرحيم
// =========================================================================
// طبقة المصادقة الوسيطة — JWT + API Token
// Authentication Middleware — JWT + API Token verification
// =========================================================================

const jwt = require('jsonwebtoken');
const crypto = require('crypto');
const { sendError } = require('../utils/error-codes');

const JWT_SECRET = process.env.JWT_SECRET || 'sad-registry-default-secret-change-me';

/**
 * مصادقة إلزامية — يرفض الطلب إذا لم يكن مسجل الدخول
 */
function authenticate(db) {
    return async (req, res, next) => {
        const authHeader = req.headers.authorization;

        if (!authHeader) {
            return sendError(res, 'AUTH_001');
        }

        // ─── محاولة JWT (Bearer token) ───
        if (authHeader.startsWith('Bearer ')) {
            const token = authHeader.slice(7);
            try {
                const decoded = jwt.verify(token, JWT_SECRET);
                const user = db.prepare(
                    'SELECT id, username, email, display_name, is_active, is_admin FROM users WHERE id = ?'
                ).get(decoded.id);

                if (!user) {
                    return sendError(res, 'AUTH_002', {
                        explain: 'المستخدم المرتبط بهذا الرمز لم يعد موجوداً',
                    });
                }

                if (!user.is_active) {
                    return sendError(res, 'AUTH_005');
                }

                req.user = user;
                return next();
            } catch (err) {
                if (err.name === 'TokenExpiredError') {
                    return sendError(res, 'AUTH_002', {
                        explain: 'رمز JWT منتهي الصلاحية. صلاحية الرمز الافتراضية 30 يوماً',
                        hint: 'أعد تسجيل الدخول: sad-pkg login',
                    });
                }
                return sendError(res, 'AUTH_002');
            }
        }

        // ─── محاولة API Token (sad_xxxxx) ───
        if (authHeader.startsWith('sad_')) {
            const rawToken = authHeader;
            const tokenHash = crypto.createHash('sha256').update(rawToken).digest('hex');

            // البحث في جدول api_tokens
            const tokenRecord = db.prepare(`
                SELECT at.*, u.id as uid, u.username, u.email, u.display_name, u.is_active, u.is_admin
                FROM api_tokens at
                JOIN users u ON at.user_id = u.id
                WHERE at.token_hash = ? AND at.is_active = 1
            `).get(tokenHash);

            if (tokenRecord) {
                if (!tokenRecord.is_active) {
                    return sendError(res, 'AUTH_005');
                }

                // تحديث آخر استخدام
                db.prepare('UPDATE api_tokens SET last_used_at = datetime("now") WHERE id = ?')
                    .run(tokenRecord.id);

                req.user = {
                    id: tokenRecord.uid,
                    username: tokenRecord.username,
                    email: tokenRecord.email,
                    display_name: tokenRecord.display_name,
                    is_active: tokenRecord.is_active,
                    is_admin: tokenRecord.is_admin,
                };
                return next();
            }

            // البحث في حقل api_token للمستخدم مباشرة
            const user = db.prepare(
                'SELECT id, username, email, display_name, is_active, is_admin FROM users WHERE api_token = ? AND is_active = 1'
            ).get(rawToken);

            if (user) {
                req.user = user;
                return next();
            }

            return sendError(res, 'AUTH_003');
        }

        // ─── صيغة غير معروفة ───
        return sendError(res, 'AUTH_001', {
            explain: 'صيغة ترويسة Authorization غير معروفة. الصيغ المدعومة: "Bearer <jwt>" أو "sad_xxxx"',
        });
    };
}

/**
 * مصادقة اختيارية — لا يرفض الطلب لكن يضيف req.user إذا كان مسجل الدخول
 */
function optionalAuth(db) {
    return async (req, res, next) => {
        const authHeader = req.headers.authorization;
        if (!authHeader) {
            req.user = null;
            return next();
        }

        // نستخدم نفس منطق authenticate لكن بدون رفض
        try {
            if (authHeader.startsWith('Bearer ')) {
                const token = authHeader.slice(7);
                const decoded = jwt.verify(token, JWT_SECRET);
                const user = db.prepare(
                    'SELECT id, username, email, display_name, is_active, is_admin FROM users WHERE id = ? AND is_active = 1'
                ).get(decoded.id);
                req.user = user || null;
            } else if (authHeader.startsWith('sad_')) {
                const user = db.prepare(
                    'SELECT id, username, email, display_name, is_active, is_admin FROM users WHERE api_token = ? AND is_active = 1'
                ).get(authHeader);
                req.user = user || null;
            } else {
                req.user = null;
            }
        } catch {
            req.user = null;
        }

        next();
    };
}

module.exports = { authenticate, optionalAuth };
