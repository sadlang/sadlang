/**
 * @file debugger.test.ts
 * @brief اختبارات وحدة للمنقّح المرئي — 40 اختبار
 *
 * يختبر استخراج المتغيرات، بناء مكدس الاستدعاء،
 * تحليل نقاط التوقف، ومعالجة مخرجات التنفيذ.
 */

import * as assert from 'assert';

// ─── محاكاة استخراج المتغيرات ───

interface VariableInfo {
    name: string;
    value: string;
    type: string;
}

interface CallFrame {
    name: string;
    line: number;
}

/** استخراج المتغيرات من كود */
function extractVariables(code: string): VariableInfo[] {
    const vars: VariableInfo[] = [];
    const lines = code.split('\n');
    const varRegex = /(?:متغير|ثابت)\s+(\S+)\s*=\s*(.+)/;

    for (const line of lines) {
        const match = line.trim().match(varRegex);
        if (match) {
            const name = match[1];
            const rawVal = match[2].trim();
            let type = 'غير معروف';
            if (/^\d+$/.test(rawVal)) type = 'رقم';
            else if (/^\d+\.\d+$/.test(rawVal)) type = 'عشري';
            else if (/^".*"$/.test(rawVal)) type = 'نص';
            else if (rawVal === 'صحيح' || rawVal === 'خطأ') type = 'منطقي';
            else if (/^\[.*\]$/.test(rawVal)) type = 'مصفوفة';
            else if (rawVal === 'لاشيء') type = 'عدم';
            vars.push({ name, value: rawVal, type });
        }
    }
    return vars;
}

/** بناء مكدس الاستدعاء */
function buildCallStack(code: string): CallFrame[] {
    const frames: CallFrame[] = [];
    const lines = code.split('\n');
    const fnRegex = /^دالة\s+(\S+)\s*\(/;

    for (let i = 0; i < lines.length; i++) {
        const match = lines[i].trim().match(fnRegex);
        if (match) {
            frames.push({ name: match[1], line: i + 1 });
        }
    }
    return frames;
}

/** تحقق من نقاط التوقف */
function isBreakpointLine(line: string): boolean {
    const trimmed = line.trim();
    // نقطة توقف = سطر تنفيذي وليس تعليق أو كتلة فارغة
    if (trimmed === '' || trimmed.startsWith('#') || trimmed === 'نهاية') return false;
    if (trimmed.startsWith('دالة ') || trimmed.startsWith('صنف ') || trimmed.startsWith('بنية ')) return false;
    return true;
}

/** تحليل مخرجات التنفيذ */
function parseOutput(raw: string): { lines: string[]; hasError: boolean; errorLine: number } {
    const lines = raw.split('\n').filter((l: string) => l.trim() !== '');
    let hasError = false;
    let errorLine = -1;
    for (let i = 0; i < lines.length; i++) {
        if (lines[i].includes('خطأ') || lines[i].includes('Error') || lines[i].includes('error')) {
            hasError = true;
            errorLine = i;
            break;
        }
    }
    return { lines, hasError, errorLine };
}

/** عدّ نقاط التوقف الصالحة */
function countBreakpoints(code: string): number {
    return code.split('\n').filter((l: string) => isBreakpointLine(l)).length;
}

// ═════════════════════════════════════════════
suite('المنقّح المرئي — Visual Debugger', () => {

    // ─── مجموعة 1: استخراج المتغيرات (10) ───
    test('1. رقم صحيح', () => {
        const v = extractVariables('متغير س = 42');
        assert.strictEqual(v.length, 1);
        assert.strictEqual(v[0].type, 'رقم');
    });
    test('2. نص', () => {
        const v = extractVariables('متغير اسم = "أحمد"');
        assert.strictEqual(v[0].type, 'نص');
    });
    test('3. عشري', () => {
        const v = extractVariables('متغير ط = 3.14');
        assert.strictEqual(v[0].type, 'عشري');
    });
    test('4. منطقي صحيح', () => {
        const v = extractVariables('متغير ع = صحيح');
        assert.strictEqual(v[0].type, 'منطقي');
    });
    test('5. منطقي خطأ', () => {
        const v = extractVariables('متغير ع = خطأ');
        assert.strictEqual(v[0].type, 'منطقي');
    });
    test('6. مصفوفة', () => {
        const v = extractVariables('متغير أ = [1، 2، 3]');
        assert.strictEqual(v[0].type, 'مصفوفة');
    });
    test('7. ثابت', () => {
        const v = extractVariables('ثابت حد = 100');
        assert.strictEqual(v.length, 1);
        assert.strictEqual(v[0].name, 'حد');
    });
    test('8. عدة متغيرات', () => {
        const v = extractVariables('متغير أ = 1\nمتغير ب = 2\nمتغير ج = 3');
        assert.strictEqual(v.length, 3);
    });
    test('9. لاشيء', () => {
        const v = extractVariables('متغير ن = لاشيء');
        assert.strictEqual(v[0].type, 'عدم');
    });
    test('10. بدون متغيرات', () => {
        assert.strictEqual(extractVariables('اطبع("مرحبا")').length, 0);
    });

    // ─── مجموعة 2: مكدس الاستدعاء (10) ───
    test('11. دالة واحدة', () => {
        const s = buildCallStack('دالة مرحبا()\nنهاية');
        assert.strictEqual(s.length, 1);
        assert.strictEqual(s[0].name, 'مرحبا');
    });
    test('12. دالتان', () => {
        const s = buildCallStack('دالة أ()\nنهاية\nدالة ب()\nنهاية');
        assert.strictEqual(s.length, 2);
    });
    test('13. رقم السطر', () => {
        const s = buildCallStack('\n\nدالة ث()\nنهاية');
        assert.strictEqual(s[0].line, 3);
    });
    test('14. بدون دوال', () => {
        assert.strictEqual(buildCallStack('متغير أ = 1').length, 0);
    });
    test('15. ثلاث دوال', () => {
        const s = buildCallStack('دالة أ()\nنهاية\nدالة ب()\nنهاية\nدالة ج()\nنهاية');
        assert.strictEqual(s.length, 3);
    });
    test('16. اسم الدالة بمعاملات', () => {
        const s = buildCallStack('دالة جمع(أ، ب)\nنهاية');
        assert.strictEqual(s[0].name, 'جمع');
    });
    test('17. كود فارغ', () => {
        assert.strictEqual(buildCallStack('').length, 0);
    });
    test('18. تعليقات فقط', () => {
        assert.strictEqual(buildCallStack('# تعليق').length, 0);
    });
    test('19. دالة داخل صنف', () => {
        const s = buildCallStack('صنف أ\n    دالة ب()\n    نهاية\nنهاية');
        assert.strictEqual(s.length, 1);
        assert.strictEqual(s[0].name, 'ب');
    });
    test('20. ترتيب الدوال محفوظ', () => {
        const s = buildCallStack('دالة ألف()\nنهاية\nدالة باء()\nنهاية');
        assert.strictEqual(s[0].name, 'ألف');
        assert.strictEqual(s[1].name, 'باء');
    });

    // ─── مجموعة 3: نقاط التوقف (10) ───
    test('21. سطر تنفيذي — نقطة توقف صالحة', () => {
        assert.strictEqual(isBreakpointLine('اطبع("مرحبا")'), true);
    });
    test('22. سطر فارغ — ليس نقطة توقف', () => {
        assert.strictEqual(isBreakpointLine(''), false);
    });
    test('23. تعليق — ليس نقطة توقف', () => {
        assert.strictEqual(isBreakpointLine('# تعليق'), false);
    });
    test('24. نهاية — ليست نقطة توقف', () => {
        assert.strictEqual(isBreakpointLine('نهاية'), false);
    });
    test('25. تعريف دالة — ليس نقطة توقف', () => {
        assert.strictEqual(isBreakpointLine('دالة جمع(أ)'), false);
    });
    test('26. تعريف صنف — ليس نقطة توقف', () => {
        assert.strictEqual(isBreakpointLine('صنف حيوان'), false);
    });
    test('27. إسناد — نقطة توقف', () => {
        assert.strictEqual(isBreakpointLine('متغير س = 5'), true);
    });
    test('28. استدعاء تابع — نقطة توقف', () => {
        assert.strictEqual(isBreakpointLine('ش.اطبع()'), true);
    });
    test('29. عدّ نقاط التوقف', () => {
        const code = 'دالة أ()\n    متغير س = 1\n    اطبع(س)\nنهاية';
        assert.strictEqual(countBreakpoints(code), 2); // متغير + اطبع
    });
    test('30. عدّ في كود فارغ', () => {
        assert.strictEqual(countBreakpoints(''), 0);
    });

    // ─── مجموعة 4: تحليل المخرجات (10) ───
    test('31. مخرجات عادية', () => {
        const r = parseOutput('مرحبا\nعالم');
        assert.strictEqual(r.lines.length, 2);
        assert.strictEqual(r.hasError, false);
    });
    test('32. مخرجات بخطأ عربي', () => {
        const r = parseOutput('نتيجة: 5\nخطأ: قسمة على صفر');
        assert.strictEqual(r.hasError, true);
    });
    test('33. مخرجات بخطأ إنجليزي', () => {
        const r = parseOutput('Error: undefined variable');
        assert.strictEqual(r.hasError, true);
    });
    test('34. مخرجات فارغة', () => {
        const r = parseOutput('');
        assert.strictEqual(r.lines.length, 0);
        assert.strictEqual(r.hasError, false);
    });
    test('35. رقم سطر الخطأ', () => {
        const r = parseOutput('سطر 1\nسطر 2\nخطأ: مشكلة');
        assert.strictEqual(r.errorLine, 2);
    });
    test('36. لا خطأ في نص عادي', () => {
        const r = parseOutput('التنفيذ ناجح\nالنتيجة: 42');
        assert.strictEqual(r.hasError, false);
    });
    test('37. تصفية أسطر فارغة', () => {
        const r = parseOutput('مرحبا\n\n\nعالم');
        assert.strictEqual(r.lines.length, 2);
    });
    test('38. خطأ في السطر الأول', () => {
        const r = parseOutput('خطأ: ملف غير موجود');
        assert.strictEqual(r.errorLine, 0);
    });
    test('39. error صغير', () => {
        const r = parseOutput('some error occurred');
        assert.strictEqual(r.hasError, true);
    });
    test('40. مخرجات متعددة الأسطر بدون خطأ', () => {
        const r = parseOutput('1\n2\n3\n4\n5');
        assert.strictEqual(r.lines.length, 5);
        assert.strictEqual(r.hasError, false);
        assert.strictEqual(r.errorLine, -1);
    });
});
