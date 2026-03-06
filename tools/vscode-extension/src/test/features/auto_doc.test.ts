/**
 * @file auto_doc.test.ts
 * @brief اختبارات وحدة لمولد التوثيق التلقائي — 40 اختبار
 * 
 * يختبر جميع حالات تحليل التعريفات وتوليد كتل التوثيق.
 */

import * as assert from 'assert';

// ─── دوال مساعدة لمحاكاة الوظائف المُصدّرة ───
// ملاحظة: نختبر المنطق الداخلي مباشرة بدون VS Code API

/**
 * تحليل تعريف من سطر كود (محاكاة parseDefinition)
 */
function parseDefinition(line: string): { type: string; name: string; params: string[] } | null {
    // دالة
    const funcMatch = line.match(/^\s*دالة\s+(\S+)\s*\(([^)]*)\)/);
    if (funcMatch) {
        const params = funcMatch[2] ? funcMatch[2].split(/[,،]/).map(p => p.trim()).filter(Boolean) : [];
        return { type: 'دالة', name: funcMatch[1], params };
    }
    // باني
    const ctorMatch = line.match(/^\s*باني\s*\(([^)]*)\)/);
    if (ctorMatch) {
        const params = ctorMatch[1] ? ctorMatch[1].split(/[,،]/).map(p => p.trim()).filter(Boolean) : [];
        return { type: 'باني', name: 'باني', params };
    }
    // صنف
    const classMatch = line.match(/^\s*صنف\s+(\S+)/);
    if (classMatch) { return { type: 'صنف', name: classMatch[1], params: [] }; }
    // بنية
    const structMatch = line.match(/^\s*بنية\s+(\S+)/);
    if (structMatch) { return { type: 'بنية', name: structMatch[1], params: [] }; }
    // تعداد
    const enumMatch = line.match(/^\s*تعداد\s+(\S+)/);
    if (enumMatch) { return { type: 'تعداد', name: enumMatch[1], params: [] }; }
    // سمة
    const traitMatch = line.match(/^\s*سمة\s+(\S+)/);
    if (traitMatch) { return { type: 'سمة', name: traitMatch[1], params: [] }; }
    // خاصية
    const propMatch = line.match(/^\s*خاصية\s+(\S+)/);
    if (propMatch) { return { type: 'خاصية', name: propMatch[1], params: [] }; }
    return null;
}

/**
 * فحص وجود ارجع في جسم الدالة
 */
function scanBodyForReturn(lines: string[], startIndex: number): boolean {
    for (let i = startIndex + 1; i < lines.length; i++) {
        const t = lines[i].trim();
        if (t === 'نهاية') { break; }
        if (t.startsWith('ارجع')) { return true; }
    }
    return false;
}

/**
 * توليد كتلة التوثيق
 */
function generateDocBlock(def: { type: string; name: string; params: string[] }, hasReturn: boolean): string {
    const lines: string[] = ['#**'];
    lines.push(` * @وصف ${def.type} ${def.name}`);
    for (const p of def.params) {
        lines.push(` * @معامل ${p} — `);
    }
    if (hasReturn) {
        lines.push(` * @إرجاع `);
    }
    lines.push(` * @مثال`);
    if (def.params.length > 0) {
        lines.push(` *   ${def.name}(${def.params.join('، ')})`);
    }
    lines.push(' **#');
    return lines.join('\n');
}

/**
 * فحص وجود توثيق سابق
 */
function hasExistingDoc(lines: string[], lineIndex: number): boolean {
    for (let i = lineIndex - 1; i >= 0; i--) {
        const t = lines[i].trim();
        if (t === '') { continue; }
        if (t.endsWith('**#') || t.startsWith('##')) { return true; }
        break;
    }
    return false;
}

// ═════════════════════════════════════════════
// الاختبارات — 40 اختبار
// ═════════════════════════════════════════════

suite('مولد التوثيق التلقائي — Auto Doc', () => {

    // ─── مجموعة 1: تحليل الدوال (10 اختبارات) ───
    test('1. دالة بسيطة بمعاملين', () => {
        const r = parseDefinition('دالة جمع(أ، ب)');
        assert.ok(r);
        assert.strictEqual(r!.type, 'دالة');
        assert.strictEqual(r!.name, 'جمع');
        assert.deepStrictEqual(r!.params, ['أ', 'ب']);
    });

    test('2. دالة بدون معاملات', () => {
        const r = parseDefinition('دالة مرحبا()');
        assert.ok(r);
        assert.strictEqual(r!.name, 'مرحبا');
        assert.deepStrictEqual(r!.params, []);
    });

    test('3. دالة بمعامل واحد', () => {
        const r = parseDefinition('دالة طباعة(رسالة)');
        assert.ok(r);
        assert.deepStrictEqual(r!.params, ['رسالة']);
    });

    test('4. دالة بثلاث معاملات مع فاصلة عربية', () => {
        const r = parseDefinition('دالة حساب(أ، ب، ج)');
        assert.ok(r);
        assert.deepStrictEqual(r!.params, ['أ', 'ب', 'ج']);
    });

    test('5. دالة بمسافات بادئة', () => {
        const r = parseDefinition('    دالة داخلية(س)');
        assert.ok(r);
        assert.strictEqual(r!.name, 'داخلية');
    });

    test('6. دالة باسم لاتيني', () => {
        const r = parseDefinition('دالة calculate(x, y)');
        assert.ok(r);
        assert.strictEqual(r!.name, 'calculate');
        assert.deepStrictEqual(r!.params, ['x', 'y']);
    });

    test('7. دالة بفاصلة لاتينية', () => {
        const r = parseDefinition('دالة عملية(أ, ب)');
        assert.ok(r);
        assert.deepStrictEqual(r!.params, ['أ', 'ب']);
    });

    test('8. سطر ليس دالة', () => {
        assert.strictEqual(parseDefinition('متغير س = 10'), null);
    });

    test('9. سطر فارغ', () => {
        assert.strictEqual(parseDefinition(''), null);
    });

    test('10. سطر تعليق', () => {
        assert.strictEqual(parseDefinition('# هذا تعليق'), null);
    });

    // ─── مجموعة 2: تحليل الأصناف والبنى (10 اختبارات) ───
    test('11. صنف بسيط', () => {
        const r = parseDefinition('صنف حيوان');
        assert.ok(r);
        assert.strictEqual(r!.type, 'صنف');
        assert.strictEqual(r!.name, 'حيوان');
    });

    test('12. صنف يرث', () => {
        const r = parseDefinition('صنف قطة يرث حيوان');
        assert.ok(r);
        assert.strictEqual(r!.name, 'قطة');
    });

    test('13. باني بمعاملين', () => {
        const r = parseDefinition('    باني(الاسم، العمر)');
        assert.ok(r);
        assert.strictEqual(r!.type, 'باني');
        assert.deepStrictEqual(r!.params, ['الاسم', 'العمر']);
    });

    test('14. باني بدون معاملات', () => {
        const r = parseDefinition('    باني()');
        assert.ok(r);
        assert.deepStrictEqual(r!.params, []);
    });

    test('15. بنية', () => {
        const r = parseDefinition('بنية نقطة');
        assert.ok(r);
        assert.strictEqual(r!.type, 'بنية');
        assert.strictEqual(r!.name, 'نقطة');
    });

    test('16. تعداد', () => {
        const r = parseDefinition('تعداد اللون');
        assert.ok(r);
        assert.strictEqual(r!.type, 'تعداد');
    });

    test('17. سمة', () => {
        const r = parseDefinition('سمة قابل_للطباعة');
        assert.ok(r);
        assert.strictEqual(r!.type, 'سمة');
    });

    test('18. خاصية', () => {
        const r = parseDefinition('    خاصية الاسم');
        assert.ok(r);
        assert.strictEqual(r!.type, 'خاصية');
    });

    test('19. متغير ليس تعريفاً', () => {
        assert.strictEqual(parseDefinition('متغير س = 5'), null);
    });

    test('20. بينما ليست تعريفاً', () => {
        assert.strictEqual(parseDefinition('بينما (س > 0)'), null);
    });

    // ─── مجموعة 3: فحص الإرجاع (10 اختبارات) ───
    test('21. دالة مع ارجع', () => {
        const lines = ['دالة جمع(أ، ب)', '    ارجع أ + ب', 'نهاية'];
        assert.strictEqual(scanBodyForReturn(lines, 0), true);
    });

    test('22. دالة بدون ارجع', () => {
        const lines = ['دالة طباعة()', '    اطبع("مرحبا")', 'نهاية'];
        assert.strictEqual(scanBodyForReturn(lines, 0), false);
    });

    test('23. ارجع في السطر الثاني', () => {
        const lines = ['دالة حساب()', '    متغير س = 10', '    ارجع س * 2', 'نهاية'];
        assert.strictEqual(scanBodyForReturn(lines, 0), true);
    });

    test('24. نهاية بدون ارجع', () => {
        const lines = ['دالة فارغة()', 'نهاية'];
        assert.strictEqual(scanBodyForReturn(lines, 0), false);
    });

    test('25. ارجع بقيمة نصية', () => {
        const lines = ['دالة رسالة()', '    ارجع "مرحبا"', 'نهاية'];
        assert.strictEqual(scanBodyForReturn(lines, 0), true);
    });

    test('26. دالة فارغة تماماً', () => {
        const lines = ['دالة لاشيء()'];
        assert.strictEqual(scanBodyForReturn(lines, 0), false);
    });

    test('27. ارجع لاشيء', () => {
        const lines = ['دالة اختبار()', '    ارجع لاشيء', 'نهاية'];
        assert.strictEqual(scanBodyForReturn(lines, 0), true);
    });

    test('28. ارجع مع شرط', () => {
        const lines = ['دالة فحص(س)', '    إذا (س > 0)', '    ارجع صحيح', '    نهاية', 'نهاية'];
        assert.strictEqual(scanBodyForReturn(lines, 0), true);
    });

    test('29. عدة أسطر بدون ارجع', () => {
        const lines = ['دالة عملية()', '    متغير أ = 1', '    متغير ب = 2', '    اطبع(أ + ب)', 'نهاية'];
        assert.strictEqual(scanBodyForReturn(lines, 0), false);
    });

    test('30. ملف فارغ', () => {
        assert.strictEqual(scanBodyForReturn([], 0), false);
    });

    // ─── مجموعة 4: توليد كتل التوثيق (10 اختبارات) ───
    test('31. توثيق دالة بمعاملين مع إرجاع', () => {
        const doc = generateDocBlock({ type: 'دالة', name: 'جمع', params: ['أ', 'ب'] }, true);
        assert.ok(doc.includes('@وصف'));
        assert.ok(doc.includes('@معامل أ'));
        assert.ok(doc.includes('@معامل ب'));
        assert.ok(doc.includes('@إرجاع'));
        assert.ok(doc.includes('#**'));
        assert.ok(doc.includes('**#'));
    });

    test('32. توثيق دالة بدون معاملات', () => {
        const doc = generateDocBlock({ type: 'دالة', name: 'مرحبا', params: [] }, false);
        assert.ok(!doc.includes('@معامل'));
        assert.ok(!doc.includes('@إرجاع'));
    });

    test('33. توثيق صنف', () => {
        const doc = generateDocBlock({ type: 'صنف', name: 'حيوان', params: [] }, false);
        assert.ok(doc.includes('@وصف صنف حيوان'));
    });

    test('34. توثيق باني', () => {
        const doc = generateDocBlock({ type: 'باني', name: 'باني', params: ['الاسم'] }, false);
        assert.ok(doc.includes('@معامل الاسم'));
    });

    test('35. مثال في التوثيق', () => {
        const doc = generateDocBlock({ type: 'دالة', name: 'حساب', params: ['س', 'ع'] }, true);
        assert.ok(doc.includes('@مثال'));
        assert.ok(doc.includes('حساب(س، ع)'));
    });

    test('36. فحص توثيق سابق — موجود', () => {
        const lines = ['', ' **#', 'دالة جمع(أ، ب)'];
        assert.strictEqual(hasExistingDoc(lines, 2), true);
    });

    test('37. فحص توثيق سابق — غير موجود', () => {
        const lines = ['متغير س = 10', '', 'دالة جمع(أ، ب)'];
        assert.strictEqual(hasExistingDoc(lines, 2), false);
    });

    test('38. فحص توثيق سطر — ##', () => {
        const lines = ['## توثيق سطر', 'دالة عملية()'];
        assert.strictEqual(hasExistingDoc(lines, 1), true);
    });

    test('39. توثيق بنية', () => {
        const doc = generateDocBlock({ type: 'بنية', name: 'نقطة', params: [] }, false);
        assert.ok(doc.includes('بنية نقطة'));
    });

    test('40. توثيق سمة', () => {
        const doc = generateDocBlock({ type: 'سمة', name: 'مقارن', params: [] }, false);
        assert.ok(doc.includes('سمة مقارن'));
    });
});
