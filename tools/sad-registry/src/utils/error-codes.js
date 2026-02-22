// بسم الله الرحمن الرحيم
// =========================================================================
// رموز الأخطاء — نظام أخطاء شامل مع شرح وحلول مقترحة
// Error Codes — Comprehensive error system with explanations & solutions
// =========================================================================

/**
 * كل خطأ يحتوي على:
 *   code       — رمز فريد (مثل AUTH_001)
 *   status     — HTTP status code
 *   message    — رسالة عربية مختصرة
 *   message_en — رسالة إنجليزية مختصرة
 *   explain    — شرح تفصيلي للخطأ
 *   hint       — حل مقترح أو خطوة تالية
 */

const ERROR_CODES = {
    // ═══════════════════════════════════════════════════════════════
    // أخطاء المصادقة — Authentication Errors (AUTH_*)
    // ═══════════════════════════════════════════════════════════════
    AUTH_001: {
        code: 'AUTH_001',
        status: 401,
        message: 'لم يتم توفير رمز المصادقة',
        message_en: 'No authentication token provided',
        explain: 'يجب إرسال رمز JWT أو رمز API في ترويسة Authorization',
        hint: 'استخدم: sad-pkg login <اسم_المستخدم> أولاً، أو أضف الترويسة: Authorization: Bearer <token>',
    },
    AUTH_002: {
        code: 'AUTH_002',
        status: 401,
        message: 'رمز المصادقة غير صالح أو منتهي الصلاحية',
        message_en: 'Invalid or expired authentication token',
        explain: 'رمز JWT الذي أرسلته إما تالف أو منتهي الصلاحية (30 يوماً افتراضياً)',
        hint: 'أعد تسجيل الدخول: sad-pkg login <اسم_المستخدم>',
    },
    AUTH_003: {
        code: 'AUTH_003',
        status: 401,
        message: 'رمز API غير صالح أو معطّل',
        message_en: 'Invalid or revoked API token',
        explain: 'رمز API الذي أرسلته (sad_xxxx) غير موجود أو تم تعطيله أو انتهت صلاحيته',
        hint: 'أنشئ رمز API جديد: sad-pkg token create <اسم_الرمز>',
    },
    AUTH_004: {
        code: 'AUTH_004',
        status: 401,
        message: 'اسم المستخدم أو كلمة المرور غير صحيحة',
        message_en: 'Invalid username or password',
        explain: 'لم يتم العثور على حساب بهذه البيانات، أو أن كلمة المرور خاطئة',
        hint: 'تأكد من اسم المستخدم وكلمة المرور. إذا نسيتها، تواصل مع مدير النظام',
    },
    AUTH_005: {
        code: 'AUTH_005',
        status: 403,
        message: 'الحساب معطّل',
        message_en: 'Account is deactivated',
        explain: 'حسابك معطّل. قد يكون بسبب مخالفة سياسات الاستخدام',
        hint: 'تواصل مع مدير النظام لاستعادة حسابك',
    },

    // ═══════════════════════════════════════════════════════════════
    // أخطاء التسجيل — Registration Errors (REG_*)
    // ═══════════════════════════════════════════════════════════════
    REG_001: {
        code: 'REG_001',
        status: 409,
        message: 'اسم المستخدم مسجل مسبقاً',
        message_en: 'Username already taken',
        explain: 'يوجد حساب آخر بنفس اسم المستخدم',
        hint: 'اختر اسم مستخدم آخر. يمكنك تجربة إضافة أرقام أو رموز',
    },
    REG_002: {
        code: 'REG_002',
        status: 409,
        message: 'البريد الإلكتروني مسجل مسبقاً',
        message_en: 'Email already registered',
        explain: 'يوجد حساب مرتبط بهذا البريد الإلكتروني',
        hint: 'استخدم بريداً إلكترونياً آخر، أو سجّل الدخول بحسابك الحالي: sad-pkg login',
    },
    REG_003: {
        code: 'REG_003',
        status: 400,
        message: 'اسم المستخدم غير صالح',
        message_en: 'Invalid username format',
        explain: 'اسم المستخدم يجب أن يكون 3-50 حرفاً ويحتوي على حروف عربية أو إنجليزية وأرقام وشرطات فقط',
        hint: 'مثال صحيح: محمد-123 أو ahmad_dev',
    },
    REG_004: {
        code: 'REG_004',
        status: 400,
        message: 'البريد الإلكتروني غير صالح',
        message_en: 'Invalid email format',
        explain: 'صيغة البريد الإلكتروني غير صحيحة',
        hint: 'استخدم صيغة صحيحة مثل: user@example.com',
    },
    REG_005: {
        code: 'REG_005',
        status: 400,
        message: 'كلمة المرور ضعيفة',
        message_en: 'Password too weak',
        explain: 'كلمة المرور يجب أن تكون 8 أحرف على الأقل',
        hint: 'استخدم كلمة مرور أطول تحتوي على حروف وأرقام ورموز',
    },

    // ═══════════════════════════════════════════════════════════════
    // أخطاء الحزم — Package Errors (PKG_*)
    // ═══════════════════════════════════════════════════════════════
    PKG_001: {
        code: 'PKG_001',
        status: 404,
        message: 'الحزمة غير موجودة',
        message_en: 'Package not found',
        explain: 'لم يتم العثور على حزمة بهذا الاسم في السجل',
        hint: 'تأكد من كتابة اسم الحزمة بالشكل الصحيح. ابحث عن الحزم المتاحة: sad-pkg search <كلمة>',
    },
    PKG_002: {
        code: 'PKG_002',
        status: 404,
        message: 'الإصدار غير موجود',
        message_en: 'Version not found',
        explain: 'الحزمة موجودة لكن هذا الإصدار المحدد غير متوفر',
        hint: 'اعرض الإصدارات المتاحة: sad-pkg versions <اسم_الحزمة>',
    },
    PKG_003: {
        code: 'PKG_003',
        status: 409,
        message: 'الإصدار موجود بالفعل',
        message_en: 'Version already exists',
        explain: 'لا يمكن نشر إصدار بنفس الرقم مرتين. الإصدارات المنشورة غير قابلة للتعديل',
        hint: 'زِد رقم الإصدار في ملف sad.json ثم أعد النشر. مثلاً: 1.0.0 → 1.0.1',
    },
    PKG_004: {
        code: 'PKG_004',
        status: 403,
        message: 'ليس لديك صلاحية على هذه الحزمة',
        message_en: 'You do not have permission for this package',
        explain: 'أنت لست مالك هذه الحزمة ولم يتم إضافتك كمشارك فيها',
        hint: 'اتصل بمالك الحزمة لإضافتك. أو انشر حزمة بإسم مختلف',
    },
    PKG_005: {
        code: 'PKG_005',
        status: 400,
        message: 'اسم الحزمة غير صالح',
        message_en: 'Invalid package name',
        explain: 'اسم الحزمة يجب أن يكون 2-100 حرف ويحتوي على حروف (عربية/إنجليزية) وأرقام وشرطات فقط',
        hint: 'أمثلة صحيحة: رياضيات، json-parser، مكتبة-الشبكة',
    },
    PKG_006: {
        code: 'PKG_006',
        status: 400,
        message: 'رقم الإصدار غير صالح',
        message_en: 'Invalid version number',
        explain: 'رقم الإصدار يجب أن يتبع نظام الترقيم الدلالي (SemVer)',
        hint: 'استخدم الصيغة: كبير.متوسط.صغير (مثلاً: 1.0.0، 2.3.1-beta)',
    },
    PKG_007: {
        code: 'PKG_007',
        status: 410,
        message: 'هذا الإصدار مسحوب (yanked)',
        message_en: 'This version has been yanked',
        explain: 'تم سحب هذا الإصدار من قبل المالك وهو غير متاح للتحميل',
        hint: 'استخدم إصداراً أحدث: sad-pkg versions <اسم_الحزمة>',
    },
    PKG_008: {
        code: 'PKG_008',
        status: 404,
        message: 'ملف الحزمة غير موجود على الخادم',
        message_en: 'Package archive file missing from server',
        explain: 'ملف الأرشيف المرتبط بهذا الإصدار مفقود من التخزين. قد يكون خطأ في الخادم',
        hint: 'أبلغ عن المشكلة لمدير الخادم. حاول أيضاً تحميل إصدار آخر',
    },

    // ═══════════════════════════════════════════════════════════════
    // أخطاء النشر — Publish Errors (PUB_*)
    // ═══════════════════════════════════════════════════════════════
    PUB_001: {
        code: 'PUB_001',
        status: 400,
        message: 'لم يتم رفع ملف الحزمة',
        message_en: 'No package file uploaded',
        explain: 'طلب النشر يجب أن يحتوي على ملف أرشيف (.tar.gz أو .tgz أو .zip)',
        hint: 'استخدم: sad-pkg publish من داخل مجلد المشروع الذي يحتوي على sad.json',
    },
    PUB_002: {
        code: 'PUB_002',
        status: 400,
        message: 'بيانات الحزمة الوصفية مفقودة أو تالفة',
        message_en: 'Missing or invalid package metadata',
        explain: 'يجب إرسال حقل metadata يحتوي على الاسم والإصدار والوصف على الأقل',
        hint: 'تأكد من وجود ملف sad.json صحيح في مشروعك. الحد الأدنى: {"name": "...", "version": "1.0.0"}',
    },
    PUB_003: {
        code: 'PUB_003',
        status: 413,
        message: 'حجم الملف يتجاوز الحد المسموح',
        message_en: 'File size exceeds the limit',
        explain: 'الحد الأقصى لحجم الحزمة هو 50 ميجابايت',
        hint: 'قلّص حجم الحزمة بإزالة الملفات غير الضرورية. أضف .sadignore لاستثناء الملفات الكبيرة',
    },
    PUB_004: {
        code: 'PUB_004',
        status: 400,
        message: 'نوع الملف غير مدعوم',
        message_en: 'Unsupported file type',
        explain: 'الأنواع المدعومة: .tar.gz, .tgz, .zip فقط',
        hint: 'استخدم sad-pkg publish الذي يقوم بضغط المشروع تلقائياً بالصيغة الصحيحة',
    },

    // ═══════════════════════════════════════════════════════════════
    // أخطاء التحقق — Validation Errors (VAL_*)
    // ═══════════════════════════════════════════════════════════════
    VAL_001: {
        code: 'VAL_001',
        status: 400,
        message: 'حقول مطلوبة مفقودة',
        message_en: 'Required fields missing',
        explain: 'بعض الحقول الإلزامية لم يتم إرسالها في الطلب',
        hint: '', // يتم تعبئته ديناميكياً حسب الحقول المفقودة
    },
    VAL_002: {
        code: 'VAL_002',
        status: 400,
        message: 'خطأ في التحقق من البيانات',
        message_en: 'Validation error',
        explain: 'بعض البيانات المرسلة لا تتوافق مع الصيغة المطلوبة',
        hint: '', // يتم تعبئته ديناميكياً
    },

    // ═══════════════════════════════════════════════════════════════
    // أخطاء معدل الطلبات — Rate Limit Errors (RATE_*)
    // ═══════════════════════════════════════════════════════════════
    RATE_001: {
        code: 'RATE_001',
        status: 429,
        message: 'تجاوزت الحد الأقصى للطلبات',
        message_en: 'Too many requests',
        explain: 'أرسلت عدداً كبيراً من الطلبات في فترة قصيرة',
        hint: 'انتظر بضع دقائق ثم حاول مرة أخرى. الحد العام: 200 طلب / 15 دقيقة',
    },
    RATE_002: {
        code: 'RATE_002',
        status: 429,
        message: 'تجاوزت الحد الأقصى لمحاولات النشر',
        message_en: 'Too many publish attempts',
        explain: 'الحد الأقصى للنشر: 10 حزم في الساعة',
        hint: 'انتظر حتى انتهاء فترة التقييد ثم حاول مرة أخرى',
    },
    RATE_003: {
        code: 'RATE_003',
        status: 429,
        message: 'تجاوزت الحد الأقصى لمحاولات المصادقة',
        message_en: 'Too many authentication attempts',
        explain: 'الحد الأقصى: 20 محاولة مصادقة / 15 دقيقة',
        hint: 'انتظر 15 دقيقة ثم حاول مرة أخرى. تأكد من صحة بياناتك قبل المحاولة',
    },

    // ═══════════════════════════════════════════════════════════════
    // أخطاء عامة — General Errors (GEN_*)
    // ═══════════════════════════════════════════════════════════════
    GEN_001: {
        code: 'GEN_001',
        status: 404,
        message: 'المسار غير موجود',
        message_en: 'Route not found',
        explain: 'الرابط الذي طلبته غير موجود في واجهة API',
        hint: 'راجع التوثيق: GET /api/v1/ لعرض جميع المسارات المتاحة',
    },
    GEN_002: {
        code: 'GEN_002',
        status: 500,
        message: 'خطأ داخلي في الخادم',
        message_en: 'Internal server error',
        explain: 'حدث خطأ غير متوقع في الخادم',
        hint: 'حاول مرة أخرى. إذا استمرت المشكلة، أبلغ مدير الخادم مع ذكر رقم الطلب (request_id)',
    },
    GEN_003: {
        code: 'GEN_003',
        status: 400,
        message: 'صيغة JSON غير صالحة',
        message_en: 'Invalid JSON format',
        explain: 'جسم الطلب يحتوي على JSON غير صالح أو تالف',
        hint: 'تأكد من صحة صيغة JSON. استخدم أداة تحقق من JSON قبل الإرسال',
    },
    GEN_004: {
        code: 'GEN_004',
        status: 503,
        message: 'الخدمة غير متاحة مؤقتاً',
        message_en: 'Service temporarily unavailable',
        explain: 'الخادم يخضع للصيانة أو تحت ضغط عالٍ',
        hint: 'انتظر بضع دقائق وحاول مرة أخرى',
    },

    // ═══════════════════════════════════════════════════════════════
    // أخطاء الرمز — Token Errors (TKN_*)
    // ═══════════════════════════════════════════════════════════════
    TKN_001: {
        code: 'TKN_001',
        status: 400,
        message: 'اسم الرمز مطلوب',
        message_en: 'Token name is required',
        explain: 'يجب تحديد اسم وصفي لرمز API عند إنشائه',
        hint: 'مثال: sad-pkg token create "رمز-النشر"',
    },
    TKN_002: {
        code: 'TKN_002',
        status: 400,
        message: 'الصلاحيات غير صالحة',
        message_en: 'Invalid token scopes',
        explain: 'الصلاحيات المسموحة: publish, read, admin',
        hint: 'مثال: sad-pkg token create "رمز" --scopes publish,read',
    },

    // ═══════════════════════════════════════════════════════════════
    // أخطاء البحث — Search Errors (SRC_*)
    // ═══════════════════════════════════════════════════════════════
    SRC_001: {
        code: 'SRC_001',
        status: 400,
        message: 'استعلام البحث قصير جداً',
        message_en: 'Search query too short',
        explain: 'يجب أن يكون استعلام البحث حرفاً واحداً على الأقل',
        hint: 'جرب كلمات أطول أو أكثر تحديداً للحصول على نتائج أفضل',
    },
};

/**
 * إنشاء كائن خطأ مُهيكل من رمز الخطأ
 * @param {string} errorCode - رمز الخطأ (مثال: 'PKG_001')
 * @param {object} overrides - قيم لتجاوز الافتراضية (message, hint, details)
 * @returns {object} كائن خطأ مُهيكل
 */
function createError(errorCode, overrides = {}) {
    const template = ERROR_CODES[errorCode];
    if (!template) {
        return {
            success: false,
            error: {
                code: 'UNKNOWN',
                message: 'خطأ غير معروف',
                message_en: 'Unknown error',
                explain: '',
                hint: '',
            },
            status: 500,
        };
    }

    return {
        success: false,
        error: {
            code: template.code,
            message: overrides.message || template.message,
            message_en: overrides.message_en || template.message_en,
            explain: overrides.explain || template.explain,
            hint: overrides.hint || template.hint,
            ...(overrides.details ? { details: overrides.details } : {}),
            ...(overrides.fields ? { fields: overrides.fields } : {}),
        },
    };
}

/**
 * إنشاء خطأ وإرساله مباشرة في الاستجابة
 */
function sendError(res, errorCode, overrides = {}) {
    const template = ERROR_CODES[errorCode];
    const status = overrides.status || (template ? template.status : 500);
    const errorResponse = createError(errorCode, overrides);
    return res.status(status).json(errorResponse);
}

/**
 * فئة خطأ مخصصة للاستخدام مع throw
 */
class RegistryError extends Error {
    constructor(errorCode, overrides = {}) {
        const template = ERROR_CODES[errorCode] || {};
        super(overrides.message || template.message || 'خطأ غير معروف');
        this.name = 'RegistryError';
        this.errorCode = errorCode;
        this.status = overrides.status || template.status || 500;
        this.overrides = overrides;
    }

    toJSON() {
        return createError(this.errorCode, this.overrides);
    }
}

module.exports = {
    ERROR_CODES,
    createError,
    sendError,
    RegistryError,
};
