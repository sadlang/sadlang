// بسم الله الرحمن الرحيم
// =========================================================================
// مسارات المصادقة — تسجيل الحساب وتسجيل الدخول ورموز API
// Auth Routes — Registration, login, API tokens
// =========================================================================

const { Router } = require('express');
const { sendError } = require('../utils/error-codes');
const { validateUsername, validateEmail, validatePassword, sanitizeText } = require('../utils/validators');
const { authenticate } = require('../middleware/auth');
const { authLimiter } = require('../middleware/rate-limit');

function createAuthRouter(db, services) {
    const router = Router();
    const { userService } = services;

    // ═══════════════════════════════════════════════════════════════
    // POST /register — تسجيل حساب جديد
    // ═══════════════════════════════════════════════════════════════
    router.post('/register', authLimiter, async (req, res) => {
        try {
            const { username, email, password, display_name } = req.body;

            // التحقق من الحقول
            if (!username || !email || !password) {
                return sendError(res, 'VAL_001', {
                    hint: 'الحقول المطلوبة: username, email, password',
                    details: {
                        missing_fields: [
                            ...(!username ? ['username'] : []),
                            ...(!email ? ['email'] : []),
                            ...(!password ? ['password'] : []),
                        ],
                    },
                });
            }

            // تحقق من اسم المستخدم
            const usernameCheck = validateUsername(username);
            if (!usernameCheck.valid) {
                return sendError(res, 'REG_003', { explain: usernameCheck.reason });
            }

            // تحقق من البريد
            const emailCheck = validateEmail(email);
            if (!emailCheck.valid) {
                return sendError(res, 'REG_004', { explain: emailCheck.reason });
            }

            // تحقق من كلمة المرور
            const passwordCheck = validatePassword(password);
            if (!passwordCheck.valid) {
                return sendError(res, 'REG_005', { explain: passwordCheck.reason });
            }

            const result = await userService.register({
                username: sanitizeText(username),
                email: email.trim().toLowerCase(),
                password,
                display_name: display_name ? sanitizeText(display_name) : undefined,
            });

            if (!result.success) {
                const status = result.error?.code === 'REG_001' || result.error?.code === 'REG_002' ? 409 : 400;
                return res.status(status).json(result);
            }

            res.status(201).json(result);
        } catch (err) {
            console.error('خطأ في التسجيل:', err.message);
            return sendError(res, 'GEN_002', {
                explain: process.env.NODE_ENV !== 'production' ? err.message : undefined,
            });
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // POST /login — تسجيل الدخول
    // ═══════════════════════════════════════════════════════════════
    router.post('/login', authLimiter, async (req, res) => {
        try {
            const { username, password } = req.body;

            if (!username || !password) {
                return sendError(res, 'VAL_001', {
                    hint: 'الحقول المطلوبة: username (أو email), password',
                    details: {
                        missing_fields: [
                            ...(!username ? ['username'] : []),
                            ...(!password ? ['password'] : []),
                        ],
                    },
                });
            }

            const result = await userService.login({
                username: username.trim(),
                password,
            });

            if (!result.success) {
                const status = result.error?.code === 'AUTH_005' ? 403 : 401;
                return res.status(status).json(result);
            }

            res.json(result);
        } catch (err) {
            console.error('خطأ في تسجيل الدخول:', err.message);
            return sendError(res, 'GEN_002');
        }
    });

    // ═══════════════════════════════════════════════════════════════
    // POST /token — إنشاء رمز API جديد
    // ═══════════════════════════════════════════════════════════════
    router.post('/token', authenticate(db), async (req, res) => {
        try {
            const { name, scopes } = req.body;

            if (!name) {
                return sendError(res, 'TKN_001');
            }

            const scopeList = Array.isArray(scopes) ? scopes : ['publish'];
            const result = userService.createApiToken(req.user.id, name, scopeList);

            if (!result.success) {
                return res.status(400).json(result);
            }

            res.status(201).json(result);
        } catch (err) {
            console.error('خطأ في إنشاء رمز:', err.message);
            return sendError(res, 'GEN_002');
        }
    });

    return router;
}

module.exports = createAuthRouter;
