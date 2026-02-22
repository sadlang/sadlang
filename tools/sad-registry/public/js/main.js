// بسم الله الرحمن الرحيم
// =========================================================================
// JavaScript للواجهة الأمامية — سجل حزم لغة ص
// Frontend JavaScript — Sad Language Package Registry
// =========================================================================
//
// هذا الملف يحتوي على التفاعلات الأمامية (client-side) للموقع:
//
//   1. التركيز التلقائي على حقل البحث في الصفحة الرئيسية
//   2. نسخ أوامر التثبيت عند الضغط عليها
//   3. تحريك عداد الإحصائيات (count-up animation)
//   4. اختصار لوحة المفاتيح للبحث السريع (/)
//   5. تأثيرات دخول البطاقات مع التمرير (intersection observer)
//
// الملف يُحمّل في نهاية body (عبر footer.ejs) لضمان أن DOM جاهز.
// نستخدم DOMContentLoaded كطبقة أمان إضافية.
// =========================================================================

document.addEventListener('DOMContentLoaded', () => {

    // =====================================================================
    // 1. التركيز التلقائي على حقل البحث
    // =====================================================================
    // في الصفحة الرئيسية فقط (التي تحتوي على .hero-search-input)،
    // نركز تلقائياً على حقل البحث حتى يستطيع المستخدم الكتابة فوراً.
    // =====================================================================
    const heroSearch = document.querySelector('.hero-search-input');
    if (heroSearch) {
        heroSearch.focus();
    }

    // =====================================================================
    // 2. نسخ أوامر التثبيت عند الضغط
    // =====================================================================
    // عناصر .install-code (في الصفحة الرئيسية) و .install-block (في
    // صفحة تفاصيل الحزمة) تحتوي على أوامر مثل:
    //   sad-pkg install sad-math
    //
    // عند الضغط: ننسخ النص → نعرض "✅ تم النسخ!" → نعيد النص الأصلي
    //
    // نستخدم Clipboard API الحديثة (navigator.clipboard.writeText)
    // مع fallback لأنها تتطلب HTTPS أو localhost.
    // =====================================================================
    document.querySelectorAll('.install-code, .install-block').forEach(el => {
        // تغيير المؤشر والعنوان للإشارة أن العنصر قابل للنقر
        el.style.cursor = 'pointer';
        el.title = 'اضغط للنسخ';

        el.addEventListener('click', () => {
            const text = el.textContent.trim();

            // محاولة النسخ باستخدام Clipboard API
            if (navigator.clipboard && navigator.clipboard.writeText) {
                navigator.clipboard.writeText(text).then(() => {
                    showCopyFeedback(el);
                }).catch(() => {
                    // فشل — ربما بسبب عدم وجود HTTPS
                    fallbackCopy(text, el);
                });
            } else {
                // متصفح لا يدعم Clipboard API
                fallbackCopy(text, el);
            }
        });
    });

    // =====================================================================
    // 3. تحريك عداد الإحصائيات (Count-Up Animation)
    // =====================================================================
    // الأرقام في قسم البطل (.stat-number) تبدأ من 0 وتصعد
    // تدريجياً إلى القيمة الحقيقية — تأثير بصري جذاب.
    //
    // نستخدم requestAnimationFrame لأداء سلس.
    // المدة: 1.5 ثانية مع تباطؤ في النهاية (easing).
    // =====================================================================
    document.querySelectorAll('.stat-number').forEach(el => {
        // تحويل الرقم العربي/الإنجليزي إلى رقم عادي
        const rawText = el.textContent.trim();
        const targetNum = parseInt(rawText.replace(/[^\d]/g, ''), 10);

        // إذا كان رقماً صالحاً وأكبر من 0، نفعّل الحركة
        if (!isNaN(targetNum) && targetNum > 0) {
            animateCounter(el, targetNum, 1500);
        }
    });

    // =====================================================================
    // 4. اختصار لوحة المفاتيح: "/" للبحث السريع
    // =====================================================================
    // مثل GitHub: الضغط على "/" ينقل التركيز لحقل البحث فوراً.
    // لا يعمل إذا المستخدم يكتب في حقل إدخال آخر.
    // =====================================================================
    document.addEventListener('keydown', (e) => {
        // تجاهل إذا المستخدم يكتب في input أو textarea
        const tag = document.activeElement.tagName.toLowerCase();
        if (tag === 'input' || tag === 'textarea' || tag === 'select') return;

        if (e.key === '/') {
            e.preventDefault(); // منع كتابة "/" في الحقل
            const searchInput = document.querySelector('.search-input') 
                             || document.querySelector('.hero-search-input')
                             || document.querySelector('.search-input-large');
            if (searchInput) {
                searchInput.focus();
                searchInput.select(); // تحديد النص الموجود
            }
        }
    });

    // =====================================================================
    // 5. تأثير دخول البطاقات مع التمرير (Scroll Animation)
    // =====================================================================
    // البطاقات (.package-card) تظهر بتأثير fade-in + slide-up عند
    // دخولها مجال الرؤية أثناء التمرير.
    //
    // نستخدم IntersectionObserver وهو أفضل أداءً من scroll event
    // لأنه لا يُحسب في كل فريم.
    // =====================================================================
    if ('IntersectionObserver' in window) {
        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    entry.target.classList.add('card-visible');
                    observer.unobserve(entry.target); // تحريك مرة واحدة فقط
                }
            });
        }, {
            threshold: 0.1,      // 10% من البطاقة مرئي = تفعيل
            rootMargin: '50px'   // تبدأ قبل الدخول بـ 50px
        });

        document.querySelectorAll('.package-card, .package-list-item').forEach(card => {
            card.classList.add('card-hidden'); // حالة أولية: مخفي
            observer.observe(card);
        });
    }

});

// =========================================================================
// الدوال المساعدة / Helper Functions
// =========================================================================

/**
 * عرض تأكيد النسخ على العنصر
 * 
 * يغيّر نص العنصر مؤقتاً لـ "✅ تم النسخ!" ثم يعيده بعد 1.5 ثانية.
 * يضيف أيضاً كلاس CSS لتأثير بصري (وميض أخضر مثلاً).
 * 
 * @param {HTMLElement} el — العنصر الذي تم نسخ محتواه
 */
function showCopyFeedback(el) {
    const original = el.textContent;
    el.textContent = '✅ تم النسخ!';
    el.classList.add('copied');

    setTimeout(() => {
        el.textContent = original;
        el.classList.remove('copied');
    }, 1500);
}

/**
 * نسخ نص بطريقة بديلة (للمتصفحات القديمة أو بدون HTTPS)
 * 
 * يستخدم execCommand('copy') القديم كبديل عن Clipboard API.
 * ينشئ textarea مخفي → يحدد النص → ينسخ → يحذف textarea.
 * 
 * @param {string} text — النص المراد نسخه
 * @param {HTMLElement} el — العنصر لعرض التأكيد عليه
 */
function fallbackCopy(text, el) {
    const textarea = document.createElement('textarea');
    textarea.value = text;
    textarea.style.position = 'fixed';
    textarea.style.opacity = '0';
    document.body.appendChild(textarea);
    textarea.select();

    try {
        document.execCommand('copy');
        showCopyFeedback(el);
    } catch {
        // فشل النسخ — لا نعرض خطأ للمستخدم
        console.warn('فشل نسخ النص');
    }

    document.body.removeChild(textarea);
}

/**
 * تحريك عداد من 0 إلى القيمة المستهدفة
 * 
 * يستخدم requestAnimationFrame لحركة سلسة بـ 60fps.
 * يطبق easing (تباطؤ في النهاية) لتأثير أجمل.
 * 
 * الـ easing function: easeOutQuart = 1 - (1 - t)^4
 * هذا يعني: سريع في البداية → بطيء في النهاية
 * 
 * @param {HTMLElement} el — العنصر الذي يحتوي على الرقم
 * @param {number} target — الرقم المستهدف
 * @param {number} duration — مدة الحركة بالمللي ثانية
 */
function animateCounter(el, target, duration) {
    const start = performance.now();

    function update(currentTime) {
        // حساب النسبة المئوية للوقت المنقضي (0 → 1)
        const elapsed = currentTime - start;
        const progress = Math.min(elapsed / duration, 1);

        // تطبيق easing: تباطؤ في النهاية
        const eased = 1 - Math.pow(1 - progress, 4);

        // حساب القيمة الحالية وعرضها
        const current = Math.floor(eased * target);

        // تنسيق الرقم بالعربية (فواصل الآلاف)
        el.textContent = current.toLocaleString('ar-SA');

        // الاستمرار حتى الوصول للهدف
        if (progress < 1) {
            requestAnimationFrame(update);
        } else {
            // التأكد من عرض الرقم النهائي بالضبط
            el.textContent = target.toLocaleString('ar-SA');
        }
    }

    requestAnimationFrame(update);
}
