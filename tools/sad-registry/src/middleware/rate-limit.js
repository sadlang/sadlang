// بسم الله الرحمن الرحيم
// =========================================================================
// تقييد معدل الطلبات — حماية من الإساءة
// Rate Limiting — Protection against abuse
// =========================================================================

const rateLimit = require('express-rate-limit');
const { createError } = require('../utils/error-codes');

/**
 * مُحدّد عام — 200 طلب / 15 دقيقة
 */
const generalLimiter = rateLimit({
    windowMs: 15 * 60 * 1000,
    max: 200,
    standardHeaders: true,
    legacyHeaders: false,
    handler: (req, res) => {
        const retryAfter = Math.ceil(req.rateLimit.resetTime
            ? (req.rateLimit.resetTime - Date.now()) / 1000
            : 900);

        res.status(429).json({
            ...createError('RATE_001', {
                hint: `انتظر ${Math.ceil(retryAfter / 60)} دقيقة ثم حاول مرة أخرى`,
                details: {
                    limit: req.rateLimit.limit,
                    remaining: req.rateLimit.remaining,
                    retry_after_seconds: retryAfter,
                },
            }),
            timestamp: new Date().toISOString(),
        });
    },
    keyGenerator: (req) => {
        return req.headers['x-forwarded-for'] || req.ip || req.connection.remoteAddress;
    },
});

/**
 * مُحدّد النشر — 10 طلبات / ساعة
 */
const publishLimiter = rateLimit({
    windowMs: 60 * 60 * 1000,
    max: 10,
    standardHeaders: true,
    legacyHeaders: false,
    handler: (req, res) => {
        const retryAfter = Math.ceil(req.rateLimit.resetTime
            ? (req.rateLimit.resetTime - Date.now()) / 1000
            : 3600);

        res.status(429).json({
            ...createError('RATE_002', {
                hint: `انتظر ${Math.ceil(retryAfter / 60)} دقيقة ثم حاول النشر مرة أخرى`,
                details: {
                    limit: req.rateLimit.limit,
                    remaining: req.rateLimit.remaining,
                    retry_after_seconds: retryAfter,
                },
            }),
            timestamp: new Date().toISOString(),
        });
    },
    keyGenerator: (req) => {
        return req.user?.id || req.headers['x-forwarded-for'] || req.ip;
    },
});

/**
 * مُحدّد المصادقة — 20 طلب / 15 دقيقة
 */
const authLimiter = rateLimit({
    windowMs: 15 * 60 * 1000,
    max: 20,
    standardHeaders: true,
    legacyHeaders: false,
    handler: (req, res) => {
        const retryAfter = Math.ceil(req.rateLimit.resetTime
            ? (req.rateLimit.resetTime - Date.now()) / 1000
            : 900);

        res.status(429).json({
            ...createError('RATE_003', {
                hint: `حاولت المصادقة كثيراً. انتظر ${Math.ceil(retryAfter / 60)} دقيقة`,
                details: {
                    limit: req.rateLimit.limit,
                    remaining: req.rateLimit.remaining,
                    retry_after_seconds: retryAfter,
                },
            }),
            timestamp: new Date().toISOString(),
        });
    },
    keyGenerator: (req) => {
        return req.headers['x-forwarded-for'] || req.ip || req.connection.remoteAddress;
    },
});

module.exports = { generalLimiter, publishLimiter, authLimiter };
