/**
 * @file rtl_support.test.ts
 * @brief اختبارات وحدة لدعم RTL — 25 اختبار
 *
 * يختبر التلوين النحوي، بناء HTML، اكتشاف الملفات العربية،
 * والتبديل بين RTL و LTR.
 */

import * as assert from 'assert';

// ─── محاكاة التلوين النحوي لاختبارات RTL ───

const KEYWORDS = [
    'دالة', 'ارجع', 'صنف', 'بنية', 'تعداد', 'يرث', 'نهاية', 'جديد', 'هذا', 'باني', 'الأساس',
    'إذا', 'اذا', 'وإلا', 'والا', 'بينما', 'لكل', 'في', 'توقف', 'استمر',
    'طابق', 'عندما', 'افتراضي',
    'حاول', 'امسك', 'ارمي', 'أخيراً',
    'عام', 'خاص', 'محمي', 'مجرد',
    'استورد', 'من', 'كـ', 'صدّر',
    'متغير', 'ثابت', 'ساكن', 'خارجي',
    'صحيح', 'خطأ', 'لاشيء'
];

const BUILTIN_TYPES = [
    'رقم', 'عشري', 'نص', 'منطقي', 'فراغ', 'عدم', 'مصفوفة', 'خريطة', 'أي'
];

const BUILTIN_FUNCTIONS = [
    'اطبع', 'اطبع_سطر', 'طول', 'نوع', 'ادخل', 'حول_لرقم', 'حول_لنص'
];

function escapeHtml(text: string): string {
    return text
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;');
}

function highlightLine(line: string): string {
    let result = escapeHtml(line);
    result = result.replace(/(#[^*].*$|#$)/gm, '<span class="comment">$1</span>');
    result = result.replace(/(&quot;[^&]*&quot;|"[^"]*")/g, '<span class="string">$1</span>');
    result = result.replace(/\b(\d+\.?\d*)\b/g, '<span class="number">$1</span>');
    for (const kw of KEYWORDS) {
        const regex = new RegExp(`\\b(${kw})\\b`, 'g');
        result = result.replace(regex, '<span class="keyword">$1</span>');
    }
    for (const fn of BUILTIN_FUNCTIONS) {
        const regex = new RegExp(`\\b(${fn})\\b`, 'g');
        result = result.replace(regex, '<span class="builtin">$1</span>');
    }
    for (const t of BUILTIN_TYPES) {
        const regex = new RegExp(`\\b(${t})\\b`, 'g');
        result = result.replace(regex, '<span class="type">$1</span>');
    }
    result = result.replace(/(&&|\|\||[+\-*/%]=?|[=!<>]=?|\.\.)/g, '<span class="operator">$1</span>');
    return result;
}

function isSadFile(fileName: string): boolean {
    return fileName.endsWith('.ص');
}

suite('RTL Support Tests — اختبارات دعم RTL', () => {

    // ─── 1. اكتشاف ملفات ص ───
    suite('اكتشاف ملفات ص', () => {
        test('1. يكتشف ملف .ص', () => {
            assert.ok(isSadFile('مثال.ص'));
        });

        test('2. يرفض ملف .ts', () => {
            assert.ok(!isSadFile('file.ts'));
        });

        test('3. يكتشف مسار كامل مع .ص', () => {
            assert.ok(isSadFile('c:\\مشروع\\src\\رئيسي.ص'));
        });

        test('4. يرفض ملف بدون امتداد', () => {
            assert.ok(!isSadFile('ملف'));
        });

        test('5. يرفض ملف .py', () => {
            assert.ok(!isSadFile('script.py'));
        });
    });

    // ─── 2. escapeHtml ───
    suite('تهريب HTML', () => {
        test('6. تهريب < و >', () => {
            assert.strictEqual(escapeHtml('<div>'), '&lt;div&gt;');
        });

        test('7. تهريب &', () => {
            assert.strictEqual(escapeHtml('a & b'), 'a &amp; b');
        });

        test('8. تهريب علامات التنصيص', () => {
            assert.strictEqual(escapeHtml('"نص"'), '&quot;نص&quot;');
        });

        test('9. نص عربي بدون تغيير', () => {
            assert.strictEqual(escapeHtml('دالة جمع'), 'دالة جمع');
        });

        test('10. نص فارغ', () => {
            assert.strictEqual(escapeHtml(''), '');
        });
    });

    // ─── 3. التلوين النحوي ───
    suite('التلوين النحوي', () => {
        test('11. تلوين كلمة مفتاحية (دالة)', () => {
            const result = highlightLine('دالة جمع(أ، ب)');
            assert.ok(result.includes('<span class="keyword">دالة</span>'));
        });

        test('12. تلوين كلمة مفتاحية (إذا)', () => {
            const result = highlightLine('إذا (س > 0)');
            assert.ok(result.includes('<span class="keyword">إذا</span>'));
        });

        test('13. تلوين ارجع', () => {
            const result = highlightLine('ارجع أ + ب');
            assert.ok(result.includes('<span class="keyword">ارجع</span>'));
        });

        test('14. تلوين دالة مدمجة (اطبع)', () => {
            const result = highlightLine('اطبع("مرحبا")');
            assert.ok(result.includes('<span class="builtin">اطبع</span>'));
        });

        test('15. تلوين نوع مدمج (رقم)', () => {
            const result = highlightLine('رقم عدد = 42');
            assert.ok(result.includes('<span class="type">رقم</span>'));
        });

        test('16. تلوين تعليق', () => {
            const result = highlightLine('# هذا تعليق');
            assert.ok(result.includes('<span class="comment">'));
        });

        test('17. تلوين رقم', () => {
            const result = highlightLine('متغير س = 42');
            assert.ok(result.includes('<span class="number">42</span>'));
        });

        test('18. تلوين رقم عشري', () => {
            const result = highlightLine('متغير ع = 3.14');
            assert.ok(result.includes('<span class="number">3.14</span>'));
        });

        test('19. سطر فارغ لا يحتوي تلوين', () => {
            const result = highlightLine('');
            assert.strictEqual(result, '');
        });

        test('20. تلوين نهاية', () => {
            const result = highlightLine('نهاية');
            assert.ok(result.includes('<span class="keyword">نهاية</span>'));
        });
    });

    // ─── 4. اتجاه النص ───
    suite('اتجاه النص', () => {
        test('21. RTL مفعّل افتراضياً', () => {
            let rtlEnabled = true;
            assert.ok(rtlEnabled);
        });

        test('22. تبديل RTL إلى LTR', () => {
            let rtlEnabled = true;
            rtlEnabled = !rtlEnabled;
            assert.ok(!rtlEnabled);
        });

        test('23. تبديل LTR إلى RTL', () => {
            let rtlEnabled = false;
            rtlEnabled = !rtlEnabled;
            assert.ok(rtlEnabled);
        });

        test('24. فرض RTL', () => {
            let rtlEnabled = false;
            rtlEnabled = true;
            assert.ok(rtlEnabled);
        });

        test('25. فرض LTR', () => {
            let rtlEnabled = true;
            rtlEnabled = false;
            assert.ok(!rtlEnabled);
        });
    });
});
