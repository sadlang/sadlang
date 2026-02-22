// بسم الله الرحمن الرحيم
// =========================================================================
// أدوات التحقق — التحقق من صحة البيانات المُدخلة
// Validators — Input validation utilities
// =========================================================================

const { sendError } = require('./error-codes');

/**
 * التحقق من اسم الحزمة
 * يقبل: حروف عربية، إنجليزية، أرقام، شرطات، شرطات سفلية
 * الطول: 2-100 حرف
 */
function validatePackageName(name) {
    if (!name || typeof name !== 'string') {
        return { valid: false, reason: 'اسم الحزمة مطلوب' };
    }

    const trimmed = name.trim();
    if (trimmed.length < 2) {
        return { valid: false, reason: 'اسم الحزمة يجب أن يكون حرفين على الأقل' };
    }
    if (trimmed.length > 100) {
        return { valid: false, reason: 'اسم الحزمة يجب أن يكون 100 حرف أو أقل' };
    }

    // حروف عربية + إنجليزية + أرقام + شرطات + شرطات سفلية
    const pattern = /^[\u0600-\u06FFa-zA-Z0-9\-_]+$/;
    if (!pattern.test(trimmed)) {
        return {
            valid: false,
            reason: 'اسم الحزمة يقبل فقط: حروف عربية/إنجليزية، أرقام، شرطات (-) وشرطات سفلية (_)',
        };
    }

    // لا يبدأ بشرطة أو رقم
    if (/^[-_0-9]/.test(trimmed)) {
        return { valid: false, reason: 'اسم الحزمة يجب أن يبدأ بحرف' };
    }

    return { valid: true };
}

/**
 * التحقق من رقم الإصدار (SemVer)
 */
function validateVersion(version) {
    if (!version || typeof version !== 'string') {
        return { valid: false, reason: 'رقم الإصدار مطلوب' };
    }

    const semverPattern = /^\d+\.\d+\.\d+(-[\w.]+)?(\+[\w.]+)?$/;
    if (!semverPattern.test(version.trim())) {
        return {
            valid: false,
            reason: `رقم الإصدار "${version}" غير صالح. استخدم صيغة SemVer: كبير.متوسط.صغير (مثال: 1.0.0)`,
        };
    }

    return { valid: true };
}

/**
 * التحقق من اسم المستخدم
 */
function validateUsername(username) {
    if (!username || typeof username !== 'string') {
        return { valid: false, reason: 'اسم المستخدم مطلوب' };
    }

    const trimmed = username.trim();
    if (trimmed.length < 3) {
        return { valid: false, reason: 'اسم المستخدم يجب أن يكون 3 أحرف على الأقل' };
    }
    if (trimmed.length > 50) {
        return { valid: false, reason: 'اسم المستخدم يجب أن يكون 50 حرف أو أقل' };
    }

    const pattern = /^[\u0600-\u06FFa-zA-Z0-9\-_]+$/;
    if (!pattern.test(trimmed)) {
        return {
            valid: false,
            reason: 'اسم المستخدم يقبل فقط: حروف عربية/إنجليزية، أرقام، شرطات (-) وشرطات سفلية (_)',
        };
    }

    return { valid: true };
}

/**
 * التحقق من البريد الإلكتروني
 */
function validateEmail(email) {
    if (!email || typeof email !== 'string') {
        return { valid: false, reason: 'البريد الإلكتروني مطلوب' };
    }

    const pattern = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    if (!pattern.test(email.trim())) {
        return { valid: false, reason: 'صيغة البريد الإلكتروني غير صحيحة' };
    }

    return { valid: true };
}

/**
 * التحقق من كلمة المرور
 */
function validatePassword(password) {
    if (!password || typeof password !== 'string') {
        return { valid: false, reason: 'كلمة المرور مطلوبة' };
    }
    if (password.length < 8) {
        return { valid: false, reason: 'كلمة المرور يجب أن تكون 8 أحرف على الأقل' };
    }

    return { valid: true };
}

/**
 * تنظيف النص من HTML خطير
 */
function sanitizeText(text) {
    if (!text || typeof text !== 'string') return '';
    return text
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#x27;')
        .trim();
}

/**
 * Middleware للتحقق من الحقول المطلوبة
 * @param {string[]} fields - قائمة أسماء الحقول المطلوبة
 */
function requireFields(...fields) {
    return (req, res, next) => {
        const missing = [];
        for (const field of fields) {
            if (!req.body[field] && req.body[field] !== 0 && req.body[field] !== false) {
                missing.push(field);
            }
        }

        if (missing.length > 0) {
            return sendError(res, 'VAL_001', {
                hint: `الحقول المفقودة: ${missing.join('، ')}`,
                details: { missing_fields: missing },
            });
        }

        next();
    };
}

module.exports = {
    validatePackageName,
    validateVersion,
    validateUsername,
    validateEmail,
    validatePassword,
    sanitizeText,
    requireFields,
};
