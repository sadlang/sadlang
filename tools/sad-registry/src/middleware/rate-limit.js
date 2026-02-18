// بسم الله الرحمن الرحيم
// =========================================================================
// تحديد معدل الطلبات — Rate Limiting Middleware
// =========================================================================
//
// @file rate-limit.js
//
// الوظيفة:
//   يمنع إساءة استخدام API عبر تحديد عدد الطلبات المسموحة لكل عميل.
//   يستخدم مكتبة express-rate-limit التي تتبع الطلبات بناءً على IP.
//
// المحددات الثلاثة:
//   ┌──────────────────┬──────────────┬──────────────────────────────┐
//   │ المحدد            │ الحد          │ الاستخدام                     │
//   ├──────────────────┼──────────────┼──────────────────────────────┤
//   │ generalLimiter   │ 200/15 دقيقة │ كل الطلبات                   │
//   │ publishLimiter   │ 10/ساعة      │ نشر الحزم فقط                │
//   │ authLimiter      │ 20/15 دقيقة  │ تسجيل الدخول/التسجيل         │
//   └──────────────────┴──────────────┴──────────────────────────────┘
//
// كيف يعمل:
//   - كل IP يحصل على "نافذة" زمنية (windowMs)
//   - عند تجاوز الحد (max)، يُرجع 429 Too Many Requests
//   - الرؤوس القياسية (RateLimit-*) تُفعّل ليعرف العميل الحد المتبقي
//
// ملاحظة:
//   في الإنتاج خلف Nginx/reverse proxy، يجب تفعيل:
//     app.set('trust proxy', 1)
//   ليتعرف express-rate-limit على IP الحقيقي من X-Forwarded-For.
// =========================================================================

const rateLimit = require('express-rate-limit');

// ─── المحدد العام (كل الطلبات) ─────────────────────────────────────────
const generalLimiter = rateLimit({
    windowMs: parseInt(process.env.RATE_LIMIT_WINDOW_MS) || 15 * 60 * 1000, // 15 دقيقة
    max: parseInt(process.env.RATE_LIMIT_MAX_REQUESTS) || 200,
    message: {
        error: 'تم تجاوز الحد المسموح من الطلبات. حاول مرة أخرى لاحقاً',
        error_en: 'Too many requests. Please try again later.',
        retry_after_seconds: 900,
    },
    standardHeaders: true,
    legacyHeaders: false,
});

// ─── محدد النشر (publish فقط) ───────────────────────────────────────────
// أكثر صرامة لأن النشر عملية مكلفة (رفع ملفات، كتابة في DB وStorage).
// keyGenerator يستخدم user ID بدل IP لمنع التحايل عبر VPN.
const publishLimiter = rateLimit({
    windowMs: 60 * 60 * 1000, // ساعة واحدة
    max: parseInt(process.env.PUBLISH_RATE_LIMIT_MAX) || 10,
    message: {
        error: 'تم تجاوز الحد المسموح للنشر. حاول مرة أخرى بعد ساعة',
        error_en: 'Publish rate limit exceeded. Try again in an hour.',
    },
    keyGenerator: (req) => req.user?.id || req.ip,
});

// ─── محدد المصادقة ──────────────────────────────────────────────────────
// يحمي من هجمات brute-force على كلمات المرور.
// 20 محاولة / 15 دقيقة = كافٍ للاستخدام العادي مع حماية من التخمين.
const authLimiter = rateLimit({
    windowMs: 15 * 60 * 1000,
    max: 20,
    message: {
        error: 'محاولات تسجيل دخول كثيرة. حاول بعد 15 دقيقة',
        error_en: 'Too many login attempts. Try again in 15 minutes.',
    },
});

module.exports = { generalLimiter, publishLimiter, authLimiter };
