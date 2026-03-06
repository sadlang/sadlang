/**
 * @file performance.test.ts
 * @brief اختبارات وحدة لمحلل الأداء — 40 اختبار
 *
 * يختبر تحليل الدوال، حساب التعقيد،
 * اكتشاف الحلقات، وقياس عمق التداخل.
 */

import * as assert from 'assert';

// ─── محاكاة تحليل الأداء ───

interface FunctionInfo {
    name: string;
    startLine: number;
    endLine: number;
    lineCount: number;
    loopCount: number;
    nestingDepth: number;
    complexity: number;
}

/** تحليل الدوال من كود */
function analyzeFunctions(code: string): FunctionInfo[] {
    const lines = code.split('\n');
    const functions: FunctionInfo[] = [];
    const fnStack: Array<{ name: string; start: number; loops: number; depth: number; maxDepth: number; nestLevel: number }> = [];

    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();

        // بداية دالة
        const fnMatch = trimmed.match(/^دالة\s+(\S+)\s*\(/);
        if (fnMatch) {
            fnStack.push({ name: fnMatch[1], start: i, loops: 0, depth: 0, maxDepth: 0, nestLevel: 1 });
            continue;
        }

        // حلقة
        if (/^(بينما|لكل)\s/.test(trimmed)) {
            if (fnStack.length > 0) {
                const top = fnStack[fnStack.length - 1];
                top.loops++;
                top.depth++;
                if (top.depth > top.maxDepth) top.maxDepth = top.depth;
                top.nestLevel++;
            }
            continue;
        }

        // إذا
        if (/^(إذا|اذا)\s*\(/.test(trimmed)) {
            if (fnStack.length > 0) {
                const top = fnStack[fnStack.length - 1];
                top.depth++;
                if (top.depth > top.maxDepth) top.maxDepth = top.depth;
                top.nestLevel++;
            }
            continue;
        }

        // نهاية
        if (trimmed === 'نهاية') {
            if (fnStack.length > 0) {
                const top = fnStack[fnStack.length - 1];
                top.nestLevel--;
                if (top.nestLevel > 0) {
                    top.depth = Math.max(0, top.depth - 1);
                } else {
                    // نهاية الدالة
                    const fn = fnStack.pop()!;
                    const lineCount = i - fn.start + 1;
                    const complexity = calculateComplexity(lineCount, fn.loops, fn.maxDepth);
                    functions.push({
                        name: fn.name,
                        startLine: fn.start + 1,
                        endLine: i + 1,
                        lineCount,
                        loopCount: fn.loops,
                        nestingDepth: fn.maxDepth,
                        complexity,
                    });
                }
            }
        }
    }
    return functions;
}

/** حساب التعقيد (0-100) */
function calculateComplexity(lines: number, loops: number, depth: number): number {
    let score = 0;
    // عدد الأسطر (حد أقصى 30 نقطة)
    score += Math.min(30, Math.floor(lines / 3));
    // عدد الحلقات (حد أقصى 40 نقطة)
    score += Math.min(40, loops * 15);
    // عمق التداخل (حد أقصى 30 نقطة)
    score += Math.min(30, depth * 10);
    return Math.min(100, score);
}

/** تصنيف التعقيد */
function complexityLabel(score: number): string {
    if (score <= 20) return 'بسيط';
    if (score <= 50) return 'متوسط';
    if (score <= 75) return 'معقد';
    return 'عالي التعقيد';
}

/** لون التعقيد */
function complexityColor(score: number): string {
    if (score <= 20) return 'green';
    if (score <= 50) return 'yellow';
    if (score <= 75) return 'orange';
    return 'red';
}

/** دالة عدّ أسطر الكود (بدون فراغات وتعليقات) */
function effectiveLines(code: string): number {
    return code.split('\n').filter((l: string) => {
        const t = l.trim();
        return t !== '' && !t.startsWith('#');
    }).length;
}

/** أبطأ دالة */
function slowestFunction(fns: FunctionInfo[]): FunctionInfo | null {
    if (fns.length === 0) return null;
    return fns.reduce((max: FunctionInfo, fn: FunctionInfo) => fn.complexity > max.complexity ? fn : max);
}

// ═════════════════════════════════════════════
suite('محلل الأداء — Performance Analyzer', () => {

    // ─── مجموعة 1: حساب التعقيد (10) ───
    test('1. دالة فارغة — تعقيد منخفض', () => {
        const c = calculateComplexity(2, 0, 0);
        assert.ok(c <= 20);
    });
    test('2. حلقة واحدة', () => {
        const c = calculateComplexity(5, 1, 1);
        assert.ok(c > 0);
    });
    test('3. حلقتان متداخلتان', () => {
        const c1 = calculateComplexity(10, 2, 2);
        const c2 = calculateComplexity(10, 1, 1);
        assert.ok(c1 > c2);
    });
    test('4. تعقيد لا يتجاوز 100', () => {
        const c = calculateComplexity(1000, 100, 50);
        assert.strictEqual(c, 100);
    });
    test('5. تعقيد 0 عند صفر', () => {
        assert.strictEqual(calculateComplexity(0, 0, 0), 0);
    });
    test('6. أسطر كثيرة بدون حلقات', () => {
        const c = calculateComplexity(90, 0, 0);
        assert.strictEqual(c, 30); // حد أقصى من الأسطر فقط
    });
    test('7. حلقات كثيرة بدون عمق', () => {
        const c = calculateComplexity(3, 3, 0);
        assert.ok(c >= 40);
    });
    test('8. عمق كبير فقط', () => {
        const c = calculateComplexity(3, 0, 4);
        assert.ok(c >= 30);
    });
    test('9. تصنيف بسيط', () => {
        assert.strictEqual(complexityLabel(10), 'بسيط');
    });
    test('10. تصنيف عالي', () => {
        assert.strictEqual(complexityLabel(90), 'عالي التعقيد');
    });

    // ─── مجموعة 2: تحليل الدوال (10) ───
    test('11. دالة بسيطة', () => {
        const fns = analyzeFunctions('دالة مرحبا()\n    اطبع("مرحبا")\nنهاية');
        assert.strictEqual(fns.length, 1);
        assert.strictEqual(fns[0].name, 'مرحبا');
    });
    test('12. عدد الأسطر', () => {
        const fns = analyzeFunctions('دالة أ()\n    سطر1\n    سطر2\n    سطر3\nنهاية');
        assert.strictEqual(fns[0].lineCount, 5);
    });
    test('13. دالتان', () => {
        const fns = analyzeFunctions('دالة أ()\nنهاية\nدالة ب()\nنهاية');
        assert.strictEqual(fns.length, 2);
    });
    test('14. دالة مع حلقة', () => {
        const fns = analyzeFunctions('دالة ف()\n    بينما (صحيح)\n    نهاية\nنهاية');
        assert.strictEqual(fns[0].loopCount, 1);
    });
    test('15. دالة مع حلقتين', () => {
        const code = 'دالة ف()\n    بينما (صحيح)\n    نهاية\n    لكل ع في م\n    نهاية\nنهاية';
        const fns = analyzeFunctions(code);
        assert.strictEqual(fns[0].loopCount, 2);
    });
    test('16. كود بدون دوال', () => {
        assert.strictEqual(analyzeFunctions('متغير أ = 1\nاطبع(أ)').length, 0);
    });
    test('17. كود فارغ', () => {
        assert.strictEqual(analyzeFunctions('').length, 0);
    });
    test('18. رقم سطر البداية', () => {
        const fns = analyzeFunctions('\n\nدالة ث()\nنهاية');
        assert.strictEqual(fns[0].startLine, 3);
    });
    test('19. رقم سطر النهاية', () => {
        const fns = analyzeFunctions('دالة أ()\n    كود\nنهاية');
        assert.strictEqual(fns[0].endLine, 3);
    });
    test('20. عمق التداخل', () => {
        const code = 'دالة ف()\n    إذا (صحيح)\n        بينما (صحيح)\n        نهاية\n    نهاية\nنهاية';
        const fns = analyzeFunctions(code);
        assert.ok(fns[0].nestingDepth >= 2);
    });

    // ─── مجموعة 3: ألوان وتصنيفات (10) ───
    test('21. لون أخضر', () => {
        assert.strictEqual(complexityColor(10), 'green');
    });
    test('22. لون أصفر', () => {
        assert.strictEqual(complexityColor(35), 'yellow');
    });
    test('23. لون برتقالي', () => {
        assert.strictEqual(complexityColor(60), 'orange');
    });
    test('24. لون أحمر', () => {
        assert.strictEqual(complexityColor(85), 'red');
    });
    test('25. تصنيف متوسط', () => {
        assert.strictEqual(complexityLabel(40), 'متوسط');
    });
    test('26. تصنيف معقد', () => {
        assert.strictEqual(complexityLabel(65), 'معقد');
    });
    test('27. حد التصنيف 20', () => {
        assert.strictEqual(complexityLabel(20), 'بسيط');
    });
    test('28. حد التصنيف 50', () => {
        assert.strictEqual(complexityLabel(50), 'متوسط');
    });
    test('29. حد التصنيف 75', () => {
        assert.strictEqual(complexityLabel(75), 'معقد');
    });
    test('30. تصنيف 100', () => {
        assert.strictEqual(complexityLabel(100), 'عالي التعقيد');
    });

    // ─── مجموعة 4: دوال مساعدة (10) ───
    test('31. effectiveLines — بدون فراغ', () => {
        assert.strictEqual(effectiveLines('سطر1\nسطر2'), 2);
    });
    test('32. effectiveLines — مع فراغ', () => {
        assert.strictEqual(effectiveLines('سطر1\n\nسطر2'), 2);
    });
    test('33. effectiveLines — تعليقات', () => {
        assert.strictEqual(effectiveLines('# تعليق\nكود'), 1);
    });
    test('34. effectiveLines — فارغ', () => {
        assert.strictEqual(effectiveLines(''), 0);
    });
    test('35. slowestFunction — أبطأ', () => {
        const fns: FunctionInfo[] = [
            { name: 'أ', startLine: 1, endLine: 3, lineCount: 3, loopCount: 0, nestingDepth: 0, complexity: 10 },
            { name: 'ب', startLine: 4, endLine: 10, lineCount: 7, loopCount: 2, nestingDepth: 2, complexity: 80 },
        ];
        assert.strictEqual(slowestFunction(fns)?.name, 'ب');
    });
    test('36. slowestFunction — فارغ', () => {
        assert.strictEqual(slowestFunction([]), null);
    });
    test('37. slowestFunction — واحدة', () => {
        const fns: FunctionInfo[] = [
            { name: 'أ', startLine: 1, endLine: 3, lineCount: 3, loopCount: 0, nestingDepth: 0, complexity: 5 },
        ];
        assert.strictEqual(slowestFunction(fns)?.name, 'أ');
    });
    test('38. لكل يُحسب كحلقة', () => {
        const fns = analyzeFunctions('دالة ف()\n    لكل ع في م\n    نهاية\nنهاية');
        assert.strictEqual(fns[0].loopCount, 1);
    });
    test('39. إذا يزيد العمق', () => {
        const code = 'دالة ف()\n    إذا (صحيح)\n    نهاية\nنهاية';
        const fns = analyzeFunctions(code);
        assert.ok(fns[0].nestingDepth >= 1);
    });
    test('40. التعقيد يزداد مع الكود', () => {
        const small = analyzeFunctions('دالة أ()\nنهاية');
        const big = analyzeFunctions('دالة ب()\n    بينما (صحيح)\n        بينما (صحيح)\n        نهاية\n    نهاية\nنهاية');
        assert.ok(big[0].complexity > small[0].complexity);
    });
});
