/**
 * @file test_explorer.test.ts
 * @brief اختبارات وحدة لمستكشف الاختبارات — 40 اختبار
 *
 * يختبر اكتشاف كتل الاختبار، تحليل النتائج،
 * تصنيف الملفات، ومعالجة المخرجات.
 */

import * as assert from 'assert';

// ─── محاكاة مستكشف الاختبارات ───

interface TestItem {
    name: string;
    line: number;
    file: string;
}

interface TestResult {
    name: string;
    passed: boolean;
    duration?: number;
    message?: string;
}

/** اكتشاف كتل الاختبار في ملف */
function discoverTests(code: string, fileName: string): TestItem[] {
    const tests: TestItem[] = [];
    const lines = code.split('\n');
    const testRegex = /^اختبر\s*\(\s*"([^"]+)"\s*\)/;
    const testRegex2 = /^اختبر\s+(\S+)\s*\(/;

    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();
        let match = trimmed.match(testRegex);
        if (match) {
            tests.push({ name: match[1], line: i + 1, file: fileName });
            continue;
        }
        match = trimmed.match(testRegex2);
        if (match) {
            tests.push({ name: match[1], line: i + 1, file: fileName });
        }
    }
    return tests;
}

/** هل الملف يحتوي اختبارات */
function isTestFile(fileName: string): boolean {
    const lower = fileName.toLowerCase();
    return lower.includes('test') || lower.includes('اختبار') || lower.includes('_test.');
}

/** تحليل سطر نتيجة */
function parseResultLine(line: string): TestResult | null {
    // ✅ اسم_الاختبار (12ms)
    const passMatch = line.match(/✅\s+(.+?)(?:\s+\((\d+)ms\))?$/);
    if (passMatch) {
        return {
            name: passMatch[1].trim(),
            passed: true,
            duration: passMatch[2] ? parseInt(passMatch[2]) : undefined,
        };
    }
    // ❌ اسم_الاختبار: رسالة
    const failMatch = line.match(/❌\s+(.+?)(?::\s+(.+))?$/);
    if (failMatch) {
        return {
            name: failMatch[1].trim(),
            passed: false,
            message: failMatch[2]?.trim(),
        };
    }
    return null;
}

/** تحليل مخرجات الاختبار بالكامل */
function parseTestOutput(output: string): TestResult[] {
    return output.split('\n')
        .map((l: string) => parseResultLine(l))
        .filter((r: TestResult | null): r is TestResult => r !== null);
}

/** إحصائيات الاختبارات */
function testStats(results: TestResult[]): { total: number; passed: number; failed: number; passRate: number } {
    const total = results.length;
    const passed = results.filter((r: TestResult) => r.passed).length;
    const failed = total - passed;
    const passRate = total === 0 ? 0 : Math.round((passed / total) * 100);
    return { total, passed, failed, passRate };
}

/** ترتيب حسب المدة */
function slowestTests(results: TestResult[], limit: number): TestResult[] {
    return results
        .filter((r: TestResult) => r.duration !== undefined)
        .sort((a: TestResult, b: TestResult) => (b.duration || 0) - (a.duration || 0))
        .slice(0, limit);
}

/** تصفية الفاشلة فقط */
function failedOnly(results: TestResult[]): TestResult[] {
    return results.filter((r: TestResult) => !r.passed);
}

// ═════════════════════════════════════════════
suite('مستكشف الاختبارات — Test Explorer', () => {

    // ─── مجموعة 1: اكتشاف الاختبارات (10) ───
    test('1. اكتشاف اختبار بنص', () => {
        const code = 'اختبر("جمع عددين")\n    # كود\nنهاية';
        const tests = discoverTests(code, 'test.ص');
        assert.strictEqual(tests.length, 1);
        assert.strictEqual(tests[0].name, 'جمع عددين');
    });
    test('2. اكتشاف اختبار باسم', () => {
        const code = 'اختبر اختبار_الجمع()\nنهاية';
        const tests = discoverTests(code, 'test.ص');
        assert.strictEqual(tests[0].name, 'اختبار_الجمع');
    });
    test('3. عدة اختبارات', () => {
        const code = 'اختبر("أ")\nنهاية\nاختبر("ب")\nنهاية\nاختبر("ج")\nنهاية';
        assert.strictEqual(discoverTests(code, 't.ص').length, 3);
    });
    test('4. بدون اختبارات', () => {
        assert.strictEqual(discoverTests('دالة أ()\nنهاية', 'a.ص').length, 0);
    });
    test('5. رقم السطر', () => {
        const tests = discoverTests('\n\nاختبر("ث")\nنهاية', 't.ص');
        assert.strictEqual(tests[0].line, 3);
    });
    test('6. اسم الملف محفوظ', () => {
        const tests = discoverTests('اختبر("أ")\nنهاية', 'ملف.ص');
        assert.strictEqual(tests[0].file, 'ملف.ص');
    });
    test('7. كود فارغ', () => {
        assert.strictEqual(discoverTests('', 'a.ص').length, 0);
    });
    test('8. تعليقات فقط', () => {
        assert.strictEqual(discoverTests('# اختبر("أ")', 'a.ص').length, 0);
    });
    test('9. اختبار مع فراغات', () => {
        const tests = discoverTests('  اختبر("اختبار بسيط")\n  نهاية', 't.ص');
        assert.strictEqual(tests.length, 1);
    });
    test('10. اسم عربي كامل', () => {
        const tests = discoverTests('اختبر("التحقق من المجموع")\nنهاية', 't.ص');
        assert.strictEqual(tests[0].name, 'التحقق من المجموع');
    });

    // ─── مجموعة 2: تحليل النتائج (10) ───
    test('11. نتيجة ناجحة', () => {
        const r = parseResultLine('✅ اختبار_الجمع');
        assert.ok(r);
        assert.strictEqual(r!.passed, true);
    });
    test('12. نتيجة فاشلة', () => {
        const r = parseResultLine('❌ اختبار_الطرح');
        assert.ok(r);
        assert.strictEqual(r!.passed, false);
    });
    test('13. نتيجة مع مدة', () => {
        const r = parseResultLine('✅ اختبار (15ms)');
        assert.strictEqual(r?.duration, 15);
    });
    test('14. نتيجة فاشلة مع رسالة', () => {
        const r = parseResultLine('❌ اختبار: القيمة غير متطابقة');
        assert.strictEqual(r?.message, 'القيمة غير متطابقة');
    });
    test('15. سطر عادي — لا نتيجة', () => {
        assert.strictEqual(parseResultLine('مرحبا'), null);
    });
    test('16. parseTestOutput — عدة نتائج', () => {
        const output = '✅ أ\n❌ ب\n✅ ج';
        assert.strictEqual(parseTestOutput(output).length, 3);
    });
    test('17. parseTestOutput — فارغ', () => {
        assert.strictEqual(parseTestOutput('').length, 0);
    });
    test('18. parseTestOutput — مع أسطر عادية', () => {
        const output = 'بدء الاختبارات...\n✅ أ\nانتهى.\n❌ ب';
        assert.strictEqual(parseTestOutput(output).length, 2);
    });
    test('19. اسم الاختبار محفوظ', () => {
        const r = parseResultLine('✅ حساب_المتوسط_الحسابي');
        assert.strictEqual(r?.name, 'حساب_المتوسط_الحسابي');
    });
    test('20. نتيجة بدون مدة', () => {
        const r = parseResultLine('✅ اختبار_بسيط');
        assert.strictEqual(r?.duration, undefined);
    });

    // ─── مجموعة 3: إحصائيات (10) ───
    test('21. كل ناجح', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true },
            { name: 'ب', passed: true },
        ];
        assert.strictEqual(testStats(results).passRate, 100);
    });
    test('22. كل فاشل', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: false },
        ];
        assert.strictEqual(testStats(results).passRate, 0);
    });
    test('23. نصف ونصف', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true },
            { name: 'ب', passed: false },
        ];
        assert.strictEqual(testStats(results).passRate, 50);
    });
    test('24. إجمالي', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true },
            { name: 'ب', passed: false },
            { name: 'ج', passed: true },
        ];
        assert.strictEqual(testStats(results).total, 3);
    });
    test('25. عدد الناجحة', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true },
            { name: 'ب', passed: false },
            { name: 'ج', passed: true },
        ];
        assert.strictEqual(testStats(results).passed, 2);
    });
    test('26. عدد الفاشلة', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true },
            { name: 'ب', passed: false },
        ];
        assert.strictEqual(testStats(results).failed, 1);
    });
    test('27. لا نتائج', () => {
        assert.strictEqual(testStats([]).passRate, 0);
    });
    test('28. failedOnly', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true },
            { name: 'ب', passed: false },
            { name: 'ج', passed: false },
        ];
        assert.strictEqual(failedOnly(results).length, 2);
    });
    test('29. failedOnly — لا فاشل', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true },
        ];
        assert.strictEqual(failedOnly(results).length, 0);
    });
    test('30. failedOnly — أسماء صحيحة', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true },
            { name: 'ب', passed: false },
        ];
        assert.strictEqual(failedOnly(results)[0].name, 'ب');
    });

    // ─── مجموعة 4: ملفات وأبطأ اختبارات (10) ───
    test('31. isTestFile — test.ص', () => {
        assert.strictEqual(isTestFile('test.ص'), true);
    });
    test('32. isTestFile — اختبار.ص', () => {
        assert.strictEqual(isTestFile('اختبار.ص'), true);
    });
    test('33. isTestFile — عادي', () => {
        assert.strictEqual(isTestFile('main.ص'), false);
    });
    test('34. isTestFile — _test.ص', () => {
        assert.strictEqual(isTestFile('math_test.ص'), true);
    });
    test('35. isTestFile — MyTestFile.ص', () => {
        assert.strictEqual(isTestFile('MyTestFile.ص'), true);
    });
    test('36. slowestTests — ترتيب', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true, duration: 10 },
            { name: 'ب', passed: true, duration: 50 },
            { name: 'ج', passed: true, duration: 30 },
        ];
        const slow = slowestTests(results, 2);
        assert.strictEqual(slow[0].name, 'ب');
        assert.strictEqual(slow[1].name, 'ج');
    });
    test('37. slowestTests — حد', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true, duration: 10 },
            { name: 'ب', passed: true, duration: 50 },
        ];
        assert.strictEqual(slowestTests(results, 1).length, 1);
    });
    test('38. slowestTests — بدون مدة', () => {
        const results: TestResult[] = [
            { name: 'أ', passed: true },
        ];
        assert.strictEqual(slowestTests(results, 5).length, 0);
    });
    test('39. slowestTests — فارغ', () => {
        assert.strictEqual(slowestTests([], 5).length, 0);
    });
    test('40. تكامل: اكتشاف + تحليل', () => {
        const code = 'اختبر("جمع")\nنهاية\nاختبر("طرح")\nنهاية';
        const tests = discoverTests(code, 'math_test.ص');
        const output = '✅ جمع (5ms)\n❌ طرح: خطأ';
        const results = parseTestOutput(output);
        assert.strictEqual(tests.length, 2);
        assert.strictEqual(results.length, 2);
        const stats = testStats(results);
        assert.strictEqual(stats.total, 2);
        assert.strictEqual(stats.passed, 1);
    });
});
