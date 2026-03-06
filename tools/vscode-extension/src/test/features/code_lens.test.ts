/**
 * @file code_lens.test.ts
 * @brief اختبارات وحدة لعدسات الكود الذكية — 40 اختبار
 *
 * يختبر تحويل الأرقام العربية وعدّ المراجع واكتشاف التعريفات.
 */

import * as assert from 'assert';

// ─── محاكاة الدوال ───

/** تحويل رقم إلى أرقام هندية-عربية */
function toArabicNumerals(n: number): string {
    const digits = ['٠','١','٢','٣','٤','٥','٦','٧','٨','٩'];
    return String(n).split('').map(d => digits[parseInt(d)] || d).join('');
}

/** عدّ المراجع لرمز في نص */
function countReferences(code: string, symbol: string): number {
    let count = 0;
    const lines = code.split('\n');
    const escaped = symbol.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
    const regex = new RegExp(`\\b${escaped}\\b`, 'g');
    for (const line of lines) {
        const trimmed = line.trim();
        if (trimmed.startsWith('#')) { continue; }
        // skip string literals
        const noStrings = trimmed.replace(/"[^"]*"/g, '').replace(/'[^']*'/g, '');
        const matches = noStrings.match(regex);
        if (matches) { count += matches.length; }
    }
    return count;
}

/** اكتشاف التعريفات */
function findDefinitions(code: string): Array<{ name: string; type: string; line: number }> {
    const lines = code.split('\n');
    const defs: Array<{ name: string; type: string; line: number }> = [];
    for (let i = 0; i < lines.length; i++) {
        const t = lines[i].trim();
        const funcMatch = t.match(/^دالة\s+(\S+)\s*\(/);
        if (funcMatch) { defs.push({ name: funcMatch[1], type: 'دالة', line: i }); }
        const classMatch = t.match(/^صنف\s+(\S+)/);
        if (classMatch) { defs.push({ name: classMatch[1], type: 'صنف', line: i }); }
    }
    return defs;
}

/** تنسيق عدد المراجع بقواعد العربية */
function formatRefCount(count: number): string {
    if (count === 0) { return 'لا مراجع'; }
    if (count === 1) { return 'مرجع واحد'; }
    if (count === 2) { return 'مرجعان'; }
    if (count <= 10) { return `${toArabicNumerals(count)} مراجع`; }
    return `${toArabicNumerals(count)} مرجعاً`;
}

// ═════════════════════════════════════════════
suite('عدسات الكود — CodeLens', () => {

    // ─── مجموعة 1: تحويل الأرقام (10) ───
    test('1. تحويل 0', () => assert.strictEqual(toArabicNumerals(0), '٠'));
    test('2. تحويل 1', () => assert.strictEqual(toArabicNumerals(1), '١'));
    test('3. تحويل 5', () => assert.strictEqual(toArabicNumerals(5), '٥'));
    test('4. تحويل 10', () => assert.strictEqual(toArabicNumerals(10), '١٠'));
    test('5. تحويل 42', () => assert.strictEqual(toArabicNumerals(42), '٤٢'));
    test('6. تحويل 100', () => assert.strictEqual(toArabicNumerals(100), '١٠٠'));
    test('7. تحويل 999', () => assert.strictEqual(toArabicNumerals(999), '٩٩٩'));
    test('8. تحويل 1234', () => assert.strictEqual(toArabicNumerals(1234), '١٢٣٤'));
    test('9. تحويل 7', () => assert.strictEqual(toArabicNumerals(7), '٧'));
    test('10. تحويل 50', () => assert.strictEqual(toArabicNumerals(50), '٥٠'));

    // ─── مجموعة 2: عدّ المراجع (10) ───
    test('11. مرجع واحد', () => {
        assert.strictEqual(countReferences('متغير س = 10', 'س'), 1);
    });
    test('12. مرجعان', () => {
        assert.strictEqual(countReferences('متغير س = 10\nاطبع(س)', 'س'), 2);
    });
    test('13. لا مراجع', () => {
        assert.strictEqual(countReferences('متغير ع = 5', 'س'), 0);
    });
    test('14. تخطي التعليقات', () => {
        assert.strictEqual(countReferences('# س = 10\nمتغير ع = 5', 'س'), 0);
    });
    test('15. تخطي النصوص', () => {
        assert.strictEqual(countReferences('متغير ع = "س هنا"', 'س'), 0);
    });
    test('16. مراجع متعددة في سطر', () => {
        assert.strictEqual(countReferences('متغير ع = س + س', 'س'), 2);
    });
    test('17. رمز لاتيني', () => {
        assert.strictEqual(countReferences('var x = 10\nprint(x)', 'x'), 2);
    });
    test('18. لا يطابق جزئياً', () => {
        assert.strictEqual(countReferences('متغير سلام = 5', 'س'), 0);
    });
    test('19. مرجع في شرط', () => {
        assert.strictEqual(countReferences('إذا (س > 0)\n    اطبع(س)\nنهاية', 'س'), 2);
    });
    test('20. نص فارغ', () => {
        assert.strictEqual(countReferences('', 'س'), 0);
    });

    // ─── مجموعة 3: اكتشاف التعريفات (10) ───
    test('21. دالة واحدة', () => {
        const defs = findDefinitions('دالة جمع(أ، ب)\n    ارجع أ + ب\nنهاية');
        assert.strictEqual(defs.length, 1);
        assert.strictEqual(defs[0].name, 'جمع');
    });
    test('22. صنف واحد', () => {
        const defs = findDefinitions('صنف حيوان\nنهاية');
        assert.strictEqual(defs.length, 1);
        assert.strictEqual(defs[0].type, 'صنف');
    });
    test('23. دالة وصنف', () => {
        const defs = findDefinitions('دالة م()\nنهاية\nصنف ص\nنهاية');
        assert.strictEqual(defs.length, 2);
    });
    test('24. لا تعريفات', () => {
        assert.strictEqual(findDefinitions('متغير س = 5').length, 0);
    });
    test('25. عدة دوال', () => {
        const code = 'دالة أ()\nنهاية\nدالة ب()\nنهاية\nدالة ج()\nنهاية';
        assert.strictEqual(findDefinitions(code).length, 3);
    });
    test('26. رقم السطر صحيح', () => {
        const defs = findDefinitions('\n\nدالة ثالث()');
        assert.strictEqual(defs[0].line, 2);
    });
    test('27. نص فارغ', () => {
        assert.strictEqual(findDefinitions('').length, 0);
    });
    test('28. تعليق فقط', () => {
        assert.strictEqual(findDefinitions('# دالة وهمية()').length, 0);
    });
    test('29. صنف يرث', () => {
        const defs = findDefinitions('صنف قطة يرث حيوان');
        assert.strictEqual(defs[0].name, 'قطة');
    });
    test('30. دالة بدون أقواس لا تُطابق', () => {
        assert.strictEqual(findDefinitions('دالة_غريبة').length, 0);
    });

    // ─── مجموعة 4: تنسيق عدد المراجع (10) ───
    test('31. صفر مراجع', () => assert.strictEqual(formatRefCount(0), 'لا مراجع'));
    test('32. مرجع واحد', () => assert.strictEqual(formatRefCount(1), 'مرجع واحد'));
    test('33. مرجعان', () => assert.strictEqual(formatRefCount(2), 'مرجعان'));
    test('34. ثلاثة مراجع', () => assert.strictEqual(formatRefCount(3), '٣ مراجع'));
    test('35. خمسة مراجع', () => assert.strictEqual(formatRefCount(5), '٥ مراجع'));
    test('36. عشرة مراجع', () => assert.strictEqual(formatRefCount(10), '١٠ مراجع'));
    test('37. أحد عشر مرجعاً', () => assert.strictEqual(formatRefCount(11), '١١ مرجعاً'));
    test('38. خمسون مرجعاً', () => assert.strictEqual(formatRefCount(50), '٥٠ مرجعاً'));
    test('39. تسعة مراجع', () => assert.strictEqual(formatRefCount(9), '٩ مراجع'));
    test('40. أربعة مراجع', () => assert.strictEqual(formatRefCount(4), '٤ مراجع'));
});
