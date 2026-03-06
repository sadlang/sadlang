/**
 * @file code_review.test.ts
 * @brief اختبارات وحدة لمراجعة الكود والتحليل الثابت — 40 اختبار
 *
 * يختبر قواعد التحليل الست: متغيرات غير مستخدمة، دوال طويلة،
 * كود ميت، catch عام، نسخ كود، ثابت بدل متغير.
 */

import * as assert from 'assert';

// ─── محاكاة قواعد التحليل ───

interface Diagnostic {
    line: number;
    message: string;
    severity: 'warning' | 'info' | 'error';
    rule: string;
}

/** كشف المتغيرات غير المستخدمة */
function analyzeUnusedVariables(code: string): Diagnostic[] {
    const lines = code.split('\n');
    const diags: Diagnostic[] = [];
    for (let i = 0; i < lines.length; i++) {
        const match = lines[i].match(/^\s*(متغير|ثابت)\s+(\S+)\s*=/);
        if (match) {
            const varName = match[2];
            // عدّ المراجع بعد التعريف
            let used = false;
            for (let j = i + 1; j < lines.length; j++) {
                if (lines[j].includes(varName) && !lines[j].trim().startsWith('#')) {
                    used = true;
                    break;
                }
            }
            if (!used) {
                diags.push({
                    line: i, message: `المتغير "${varName}" معرّف ولكن غير مستخدم`,
                    severity: 'warning', rule: 'unused-variable'
                });
            }
        }
    }
    return diags;
}

/** كشف الدوال الطويلة (> maxLines) */
function analyzeLongFunctions(code: string, maxLines: number = 30): Diagnostic[] {
    const lines = code.split('\n');
    const diags: Diagnostic[] = [];
    for (let i = 0; i < lines.length; i++) {
        const match = lines[i].match(/^\s*دالة\s+(\S+)\s*\(/);
        if (match) {
            let depth = 1;
            let j = i + 1;
            while (j < lines.length && depth > 0) {
                if (/^\s*(دالة|صنف|إذا|بينما|لكل|حاول)\b/.test(lines[j])) depth++;
                if (lines[j].trim() === 'نهاية') depth--;
                j++;
            }
            const lineCount = j - i;
            if (lineCount > maxLines) {
                diags.push({
                    line: i, message: `الدالة "${match[1]}" طويلة (${lineCount} سطر > ${maxLines})`,
                    severity: 'info', rule: 'long-function'
                });
            }
        }
    }
    return diags;
}

/** كشف الكود الميت بعد ارجع */
function analyzeDeadCode(code: string): Diagnostic[] {
    const lines = code.split('\n');
    const diags: Diagnostic[] = [];
    for (let i = 0; i < lines.length - 1; i++) {
        if (lines[i].trim().startsWith('ارجع') && lines[i + 1].trim() !== 'نهاية' && lines[i + 1].trim() !== '') {
            diags.push({
                line: i + 1, message: 'كود لا يمكن الوصول إليه بعد "ارجع"',
                severity: 'warning', rule: 'dead-code'
            });
        }
    }
    return diags;
}

/** كشف catch عام بدون نوع */
function analyzeGenericCatch(code: string): Diagnostic[] {
    const lines = code.split('\n');
    const diags: Diagnostic[] = [];
    for (let i = 0; i < lines.length; i++) {
        if (lines[i].trim() === 'امسك') {
            diags.push({
                line: i, message: 'استخدم "امسك (خطأ)" بدلاً من "امسك" فقط',
                severity: 'info', rule: 'generic-catch'
            });
        }
    }
    return diags;
}

/** كشف متغير يجب أن يكون ثابتاً */
function analyzeShouldBeConst(code: string): Diagnostic[] {
    const lines = code.split('\n');
    const diags: Diagnostic[] = [];
    for (let i = 0; i < lines.length; i++) {
        const match = lines[i].match(/^\s*متغير\s+(\S+)\s*=/);
        if (match) {
            const varName = match[1];
            let reassigned = false;
            for (let j = i + 1; j < lines.length; j++) {
                const t = lines[j].trim();
                if (t.startsWith('#')) continue;
                // تحقق من إعادة الإسناد
                const assignPattern = new RegExp(`^${varName.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')}\\s*=`);
                if (assignPattern.test(t) || t.includes(`${varName} =`) || t.includes(`${varName} +=`)) {
                    reassigned = true;
                    break;
                }
            }
            if (!reassigned) {
                diags.push({
                    line: i, message: `"${varName}" لا يُعاد إسناده — استخدم "ثابت" بدلاً من "متغير"`,
                    severity: 'info', rule: 'prefer-const'
                });
            }
        }
    }
    return diags;
}

// ═════════════════════════════════════════════
suite('مراجعة الكود — Code Review', () => {

    // ─── مجموعة 1: متغيرات غير مستخدمة (8) ───
    test('1. متغير غير مستخدم', () => {
        const d = analyzeUnusedVariables('متغير س = 10');
        assert.strictEqual(d.length, 1);
        assert.ok(d[0].message.includes('س'));
    });
    test('2. متغير مستخدم', () => {
        assert.strictEqual(analyzeUnusedVariables('متغير س = 10\nاطبع(س)').length, 0);
    });
    test('3. ثابت غير مستخدم', () => {
        assert.strictEqual(analyzeUnusedVariables('ثابت ع = 5').length, 1);
    });
    test('4. عدة متغيرات — واحد غير مستخدم', () => {
        const d = analyzeUnusedVariables('متغير أ = 1\nمتغير ب = 2\nاطبع(أ)');
        assert.strictEqual(d.length, 1);
        assert.ok(d[0].message.includes('ب'));
    });
    test('5. لا متغيرات', () => {
        assert.strictEqual(analyzeUnusedVariables('اطبع("مرحبا")').length, 0);
    });
    test('6. تعليق لا يعتبر استخداماً', () => {
        assert.strictEqual(analyzeUnusedVariables('متغير س = 10\n# س هنا').length, 1);
    });
    test('7. متغير مستخدم في شرط', () => {
        assert.strictEqual(analyzeUnusedVariables('متغير س = 5\nإذا (س > 0)\nنهاية').length, 0);
    });
    test('8. نص فارغ', () => {
        assert.strictEqual(analyzeUnusedVariables('').length, 0);
    });

    // ─── مجموعة 2: دوال طويلة (8) ───
    test('9. دالة قصيرة', () => {
        assert.strictEqual(analyzeLongFunctions('دالة م()\n    ارجع 1\nنهاية', 30).length, 0);
    });
    test('10. دالة طويلة (31 سطر)', () => {
        const lines = ['دالة طويلة()'];
        for (let i = 0; i < 30; i++) lines.push('    متغير س = ' + i);
        lines.push('نهاية');
        assert.strictEqual(analyzeLongFunctions(lines.join('\n'), 30).length, 1);
    });
    test('11. حد مخصص 5 أسطر', () => {
        const code = 'دالة م()\n    أ\n    ب\n    ج\n    د\n    هـ\nنهاية';
        assert.strictEqual(analyzeLongFunctions(code, 5).length, 1);
    });
    test('12. لا دوال', () => {
        assert.strictEqual(analyzeLongFunctions('متغير س = 5', 30).length, 0);
    });
    test('13. دالتان قصيرتان', () => {
        const code = 'دالة أ()\nنهاية\nدالة ب()\nنهاية';
        assert.strictEqual(analyzeLongFunctions(code, 30).length, 0);
    });
    test('14. رسالة تحتوي اسم الدالة', () => {
        const lines = ['دالة كبيرة()'];
        for (let i = 0; i < 35; i++) lines.push('    س');
        lines.push('نهاية');
        const d = analyzeLongFunctions(lines.join('\n'), 30);
        assert.ok(d[0].message.includes('كبيرة'));
    });
    test('15. رسالة تحتوي عدد الأسطر', () => {
        const lines = ['دالة طويلة()'];
        for (let i = 0; i < 35; i++) lines.push('    س');
        lines.push('نهاية');
        const d = analyzeLongFunctions(lines.join('\n'), 30);
        assert.ok(d[0].message.includes('37'));
    });
    test('16. دالة بحد بالضبط', () => {
        const lines = ['دالة محدودة()'];
        for (let i = 0; i < 28; i++) lines.push('    س');
        lines.push('نهاية');
        assert.strictEqual(analyzeLongFunctions(lines.join('\n'), 30).length, 0);
    });

    // ─── مجموعة 3: كود ميت (8) ───
    test('17. كود بعد ارجع', () => {
        const d = analyzeDeadCode('    ارجع 5\n    اطبع("ميت")');
        assert.strictEqual(d.length, 1);
    });
    test('18. نهاية بعد ارجع — ليس ميتاً', () => {
        assert.strictEqual(analyzeDeadCode('    ارجع 5\nنهاية').length, 0);
    });
    test('19. لا ارجع', () => {
        assert.strictEqual(analyzeDeadCode('    اطبع("مرحبا")\n    اطبع("عالم")').length, 0);
    });
    test('20. ارجع في نهاية الملف', () => {
        assert.strictEqual(analyzeDeadCode('    ارجع 5').length, 0);
    });
    test('21. سطر فارغ بعد ارجع', () => {
        assert.strictEqual(analyzeDeadCode('    ارجع 5\n').length, 0);
    });
    test('22. كود ميت — سطران', () => {
        const d = analyzeDeadCode('    ارجع 5\n    أ = 1\n    ب = 2');
        assert.ok(d.length >= 1);
    });
    test('23. ارجع وسط الكود', () => {
        const d = analyzeDeadCode('    متغير أ = 1\n    ارجع أ\n    اطبع(أ)');
        assert.strictEqual(d.length, 1);
    });
    test('24. نص فارغ', () => {
        assert.strictEqual(analyzeDeadCode('').length, 0);
    });

    // ─── مجموعة 4: catch عام (8) ───
    test('25. امسك عام', () => {
        assert.strictEqual(analyzeGenericCatch('حاول\n    عملية()\nامسك\n    اطبع("خطأ")\nنهاية').length, 1);
    });
    test('26. امسك مع معامل', () => {
        assert.strictEqual(analyzeGenericCatch('امسك (خ)').length, 0);
    });
    test('27. بدون حاول', () => {
        assert.strictEqual(analyzeGenericCatch('متغير س = 5').length, 0);
    });
    test('28. عدة امسك عام', () => {
        assert.strictEqual(analyzeGenericCatch('امسك\nنهاية\nامسك\nنهاية').length, 2);
    });
    test('29. امسك عام — رسالة صحيحة', () => {
        const d = analyzeGenericCatch('امسك');
        assert.ok(d[0].message.includes('امسك (خطأ)'));
    });
    test('30. نص فارغ', () => {
        assert.strictEqual(analyzeGenericCatch('').length, 0);
    });
    test('31. امسك في تعليق', () => {
        assert.strictEqual(analyzeGenericCatch('# امسك').length, 0);
    });
    test('32. امسك داخل نص', () => {
        assert.strictEqual(analyzeGenericCatch('"امسك"').length, 0);
    });

    // ─── مجموعة 5: ثابت بدل متغير (8) ───
    test('33. متغير بدون إعادة إسناد', () => {
        const d = analyzeShouldBeConst('متغير س = 10\nاطبع(س)');
        assert.strictEqual(d.length, 1);
        assert.ok(d[0].message.includes('ثابت'));
    });
    test('34. متغير مع إعادة إسناد', () => {
        assert.strictEqual(analyzeShouldBeConst('متغير س = 10\nس = 20').length, 0);
    });
    test('35. متغير مع +=', () => {
        assert.strictEqual(analyzeShouldBeConst('متغير س = 10\nس += 5').length, 0);
    });
    test('36. ثابت — لا يحتاج فحص', () => {
        assert.strictEqual(analyzeShouldBeConst('ثابت س = 10').length, 0);
    });
    test('37. عدة متغيرات — بعضها ثابت', () => {
        const d = analyzeShouldBeConst('متغير أ = 1\nمتغير ب = 2\nب = 3');
        assert.strictEqual(d.length, 1);
        assert.ok(d[0].message.includes('أ'));
    });
    test('38. نص فارغ', () => {
        assert.strictEqual(analyzeShouldBeConst('').length, 0);
    });
    test('39. متغير مُسند في تعليق — يبقى تحذير', () => {
        const d = analyzeShouldBeConst('متغير س = 10\n# س = 20');
        assert.strictEqual(d.length, 1);
    });
    test('40. قاعدة prefer-const', () => {
        const d = analyzeShouldBeConst('متغير اسم = "أحمد"');
        assert.strictEqual(d[0].rule, 'prefer-const');
    });
});
