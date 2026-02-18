// بسم الله الرحمن الرحيم
// =========================================================================
// أدوات التحقق من صحة البيانات — Validation Utilities
// =========================================================================
//
// هذا الملف يحتوي على دوال التحقق المستخدمة في كل أنحاء الخادم:
//   - التحقق من أسماء الحزم (عربية وإنجليزية)
//   - التحقق من أرقام الإصدارات (Semantic Versioning)
//   - تطهير النصوص من HTML الضار
//
// تُستخدم في:
//   - مسار النشر (packages.js → POST /publish)
//   - خدمة الحزم (package-service.js → publish())
//   - مسار المصادقة (auth.js → register)
//
// قواعد أسماء الحزم:
//   ✅ sad-math          ← حروف لاتينية مع شرطة
//   ✅ هجري             ← حروف عربية
//   ✅ نص-عربي          ← حروف عربية مع شرطة
//   ✅ sad_utils_v2      ← حروف لاتينية مع شرطة سفلية وأرقام
//   ❌ -invalid          ← لا يبدأ بشرطة
//   ❌ 123pkg            ← لا يبدأ برقم
//   ❌ a                 ← أقل من حرفين
//   ❌ pkg with spaces   ← لا مسافات
// =========================================================================

/**
 * التحقق من صحة اسم الحزمة / Validate package name
 * 
 * القواعد:
 *   1. النوع: يجب أن يكون نصاً (string)
 *   2. الطول: 2-100 حرف
 *   3. البداية: يجب أن يبدأ بحرف عربي أو لاتيني (ليس رقماً أو رمزاً)
 *   4. المحتوى: حروف عربية (U+0600-U+06FF, U+0750-U+077F)،
 *              حروف لاتينية (a-z, A-Z)، أرقام (0-9)،
 *              شرطات (-) وشرطات سفلية (_)
 * 
 * نطاقات Unicode المدعومة:
 *   U+0600-U+06FF: العربية الأساسية (حروف، أرقام عربية، تشكيل)
 *   U+0750-U+077F: العربية التكميلية (حروف إضافية لبعض اللهجات)
 * 
 * @param {string} name — اسم الحزمة المراد التحقق منه
 * @returns {boolean} صحيح إذا كان الاسم صالحاً
 * 
 * @example
 *   validatePackageName('sad-math')   // → true
 *   validatePackageName('هجري')       // → true
 *   validatePackageName('نص-عربي')   // → true
 *   validatePackageName('')           // → false (فارغ)
 *   validatePackageName('a')          // → false (أقل من حرفين)
 *   validatePackageName('-bad')       // → false (يبدأ بشرطة)
 */
function validatePackageName(name) {
    // التحقق من النوع والوجود
    if (!name || typeof name !== 'string') return false;

    // التحقق من الطول: 2 حرف على الأقل لتجنب أسماء غامضة
    // 100 حرف كحد أقصى لتجنب أسماء طويلة بشكل غير معقول
    if (name.length < 2 || name.length > 100) return false;

    // التعبير النمطي (Regex) يتحقق من:
    //   ^                          ← بداية النص
    //   [\u0600-\u06FF\u0750-\u077Fa-zA-Z]  ← الحرف الأول: عربي أو لاتيني (ليس رقماً)
    //   [\u0600-\u06FF\u0750-\u077Fa-zA-Z0-9_-]*  ← بقية الأحرف: عربي/لاتيني/رقم/شرطة
    //   $                          ← نهاية النص
    const validPattern = /^[\u0600-\u06FF\u0750-\u077Fa-zA-Z][\u0600-\u06FF\u0750-\u077Fa-zA-Z0-9_-]*$/;
    return validPattern.test(name);
}

/**
 * التحقق من صحة رقم الإصدار / Validate version string
 * 
 * يتبع نظام Semantic Versioning (semver.org):
 *   MAJOR.MINOR.PATCH[-prerelease][+build]
 * 
 * أمثلة صالحة:
 *   "1.0.0"            ← إصدار بسيط
 *   "0.1.0"            ← إصدار ما قبل الاستقرار
 *   "2.3.1-beta.1"     ← إصدار تجريبي
 *   "1.0.0+build.123"  ← مع بيانات بناء
 * 
 * أمثلة غير صالحة:
 *   "1.0"              ← ينقص PATCH
 *   "v1.0.0"           ← بادئة v غير مسموحة
 *   "1.0.0.0"          ← أكثر من 3 أقسام
 * 
 * @param {string} version — رقم الإصدار المراد التحقق منه
 * @returns {boolean} صحيح إذا كان الإصدار صالحاً
 * 
 * @example
 *   validateVersion('1.0.0')         // → true
 *   validateVersion('0.1.0-alpha')   // → true
 *   validateVersion('invalid')       // → false
 */
function validateVersion(version) {
    if (!version || typeof version !== 'string') return false;

    // التعبير النمطي يطابق: MAJOR.MINOR.PATCH مع prerelease و build metadata اختياريين
    //   ^\d+          ← MAJOR: رقم واحد أو أكثر
    //   \.\d+         ← .MINOR: نقطة ثم رقم
    //   \.\d+         ← .PATCH: نقطة ثم رقم
    //   (-[a-zA-Z0-9.]+)?   ← prerelease اختياري (مثل: -beta.1)
    //   (\+[a-zA-Z0-9.]+)?  ← build metadata اختياري (مثل: +build.123)
    const semverPattern = /^\d+\.\d+\.\d+(-[a-zA-Z0-9.]+)?(\+[a-zA-Z0-9.]+)?$/;
    return semverPattern.test(version);
}

/**
 * تطهير النص من محتوى HTML الضار / Sanitize text from harmful HTML
 * 
 * يزيل كل وسوم HTML لمنع هجمات XSS (Cross-Site Scripting).
 * يقطع النص عند الحد الأقصى المحدد.
 * 
 * لماذا نحتاج هذا؟
 *   عند نشر حزمة، يرسل المستخدم وصفاً نصياً.
 *   إذا أدخل <script>alert('hack')</script> ولم ننظفه،
 *   سيُنفذ الكود عند عرض الوصف في واجهة الويب.
 * 
 * @param {string} text — النص المراد تطهيره
 * @param {number} maxLength — الحد الأقصى للطول (افتراضي: 500)
 * @returns {string} النص المطهّر
 * 
 * @example
 *   sanitizeText('مكتبة <b>رائعة</b>')
 *   // → 'مكتبة رائعة'
 *   
 *   sanitizeText('<script>alert(1)</script>Hello')
 *   // → 'Hello'
 */
function sanitizeText(text, maxLength = 500) {
    if (!text || typeof text !== 'string') return '';

    return text
        .slice(0, maxLength)          // قطع عند الحد الأقصى
        .replace(/<[^>]*>/g, '')      // إزالة كل وسوم HTML
        .trim();                       // إزالة المسافات الطرفية
}

module.exports = { validatePackageName, validateVersion, sanitizeText };
