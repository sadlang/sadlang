// بسم الله الرحمن الرحيم
// =========================================================================
// معالجة الأخطاء — Error Handling Middleware
// =========================================================================
//
// @file errors.js
//
// الوظيفة:
//   يوفر معالجَين للأخطاء في Express:
//
//   1. notFoundHandler — يُركَّب بعد كل المسارات:
//      - إذا وصل الطلب إلى هنا = لا يوجد مسار مطابق
//      - يميّز بين API (JSON 404) والويب (صفحة 404 HTML)
//
//   2. errorHandler — معالج أخطاء Express (4 معاملات):
//      - يلتقط أي throw أو next(error) من أي مسار
//      - يسجل الخطأ في console
//      - يخفي تفاصيل الخطأ في الإنتاج (حماية أمنية)
//      - يميّز بين API (JSON) والويب (HTML)
//
// الترتيب مهم!
//   app.use(notFoundHandler);  // ←  يجب أن يكون بعد كل app.use('/...')
//   app.use(errorHandler);     // ←  يجب أن يكون الأخير
//
// لماذا نميّز بين API والويب؟
//   - عميل sad-pkg يتوقع JSON دائماً
//   - المتصفح يتوقع HTML مع تنسيق مناسب
//   - التمييز عبر req.path.startsWith('/api/')
// =========================================================================

/**
 * معالج 404 — نقطة النهاية غير موجودة
 *
 * @param {import('express').Request} req
 * @param {import('express').Response} res
 * @param {import('express').NextFunction} next
 */
function notFoundHandler(req, res, next) {
    // تمييز بين طلبات API وطلبات الويب
    if (req.path.startsWith('/api/')) {
        res.status(404).json({
            error: 'نقطة النهاية غير موجودة',
            error_en: 'Endpoint not found',
            path: req.path,
        });
    } else {
        res.status(404).render('404', { 
            message: 'الصفحة المطلوبة غير موجودة' 
        });
    }
}

/**
 * معالج الأخطاء العام — 500 Internal Server Error
 *
 * ملاحظة: Express يتعرف على معالج الأخطاء بوجود 4 معاملات (err, req, res, next).
 * حذف أي معامل يجعل Express يعتبره middleware عادي!
 *
 * @param {Error} err — كائن الخطأ (يحتوي message, stack, و statusCode اختياري)
 * @param {import('express').Request} req
 * @param {import('express').Response} res
 * @param {import('express').NextFunction} next — مطلوب حتى لو لم يُستدعى
 */
function errorHandler(err, req, res, next) {
    console.error('❌ خطأ:', err.stack || err.message);

    const statusCode = err.statusCode || 500;
    const message = process.env.NODE_ENV === 'production' 
        ? 'خطأ في الخادم / Server error'
        : err.message;

    if (req.path.startsWith('/api/')) {
        res.status(statusCode).json({
            error: message,
            ...(process.env.NODE_ENV !== 'production' && { stack: err.stack }),
        });
    } else {
        res.status(statusCode).render('error', { message });
    }
}

module.exports = { notFoundHandler, errorHandler };
