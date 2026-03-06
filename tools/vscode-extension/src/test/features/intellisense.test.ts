/**
 * @file intellisense.test.ts
 * @brief اختبارات الإكمال الذكي — 15 اختبار
 *
 * يختبر الإكمال التلقائي، التلميحات، مساعدة التوقيعات،
 * تحليل الرموز من الملفات، وإكمال خصائص الكائنات.
 */

import * as assert from 'assert';

// ─── محاكاة بيانات الإكمال ───

interface CompletionEntry {
    label: string;
    kind: string;
    detail: string;
    documentation: string;
    insertText: string;
}

const KEYWORDS: CompletionEntry[] = [
    { label: 'إذا', kind: 'Keyword', detail: 'شرط — if', documentation: 'كتلة شرطية', insertText: 'إذا (${1:شرط})\n\t${2}\nنهاية' },
    { label: 'بينما', kind: 'Keyword', detail: 'حلقة — while', documentation: 'حلقة تكرارية', insertText: 'بينما (${1:شرط})\n\t${2}\nنهاية' },
    { label: 'لكل', kind: 'Keyword', detail: 'حلقة — for each', documentation: 'حلقة لكل عنصر', insertText: 'لكل ${1:عنصر} في ${2:القائمة}\n\t${3}\nنهاية' },
    { label: 'دالة', kind: 'Keyword', detail: 'تعريف دالة — function', documentation: 'تعريف دالة جديدة', insertText: 'دالة ${1:اسم}(${2:معاملات})\n\t${3}\nنهاية' },
    { label: 'صنف', kind: 'Keyword', detail: 'تعريف صنف — class', documentation: 'تعريف صنف جديد', insertText: 'صنف ${1:اسم}\n\tباني(${2})\n\t\t${3}\n\تنهاية\nنهاية' },
    { label: 'متغير', kind: 'Keyword', detail: 'تعريف متغير — var', documentation: 'تعريف متغير', insertText: 'متغير ${1:اسم} = ${2:قيمة}' },
    { label: 'ثابت', kind: 'Keyword', detail: 'تعريف ثابت — const', documentation: 'تعريف ثابت', insertText: 'ثابت ${1:اسم} = ${2:قيمة}' },
    { label: 'ارجع', kind: 'Keyword', detail: 'إرجاع — return', documentation: 'إرجاع قيمة', insertText: 'ارجع ${1}' },
    { label: 'حاول', kind: 'Keyword', detail: 'معالجة أخطاء — try', documentation: 'كتلة معالجة', insertText: 'حاول\n\t${1}\nامسك (${2:خطأ})\n\t${3}\nنهاية' },
    { label: 'نهاية', kind: 'Keyword', detail: 'نهاية كتلة — end', documentation: 'إنهاء كتلة', insertText: 'نهاية' },
];

const BUILTINS: CompletionEntry[] = [
    { label: 'اطبع', kind: 'Function', detail: 'طباعة — print', documentation: 'طباعة قيمة', insertText: 'اطبع(${1})' },
    { label: 'اطبع_سطر', kind: 'Function', detail: 'طباعة مع سطر', documentation: 'طباعة مع سطر جديد', insertText: 'اطبع_سطر(${1})' },
    { label: 'ادخل', kind: 'Function', detail: 'إدخال — input', documentation: 'قراءة مدخلات', insertText: 'ادخل(${1})' },
    { label: 'طول', kind: 'Function', detail: 'طول — length', documentation: 'الحصول على الطول', insertText: 'طول(${1})' },
    { label: 'نوع', kind: 'Function', detail: 'نوع — typeof', documentation: 'نوع القيمة', insertText: 'نوع(${1})' },
];

const TYPES: CompletionEntry[] = [
    { label: 'رقم', kind: 'Type', detail: 'نوع: عدد صحيح', documentation: 'عدد صحيح 64-bit', insertText: 'رقم' },
    { label: 'عشري', kind: 'Type', detail: 'نوع: عدد عشري', documentation: 'عدد عشري 64-bit', insertText: 'عشري' },
    { label: 'نص', kind: 'Type', detail: 'نوع: نص', documentation: 'سلسلة نصية UTF-8', insertText: 'نص' },
    { label: 'منطقي', kind: 'Type', detail: 'نوع: منطقي', documentation: 'صحيح/خطأ', insertText: 'منطقي' },
    { label: 'مصفوفة', kind: 'Type', detail: 'نوع: مصفوفة', documentation: 'مصفوفة ديناميكية', insertText: 'مصفوفة' },
];

// ─── تحليل الرموز ───

interface ParsedSymbol {
    name: string;
    kind: string;
    detail: string;
    line: number;
    params?: string[];
}

function parseSymbols(code: string): ParsedSymbol[] {
    const symbols: ParsedSymbol[] = [];
    const lines = code.split('\n');

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();

        const funcMatch = line.match(/^دالة\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)\s*\(([^)]*)\)/);
        if (funcMatch) {
            const params = funcMatch[2] ? funcMatch[2].split(/[،,]/).map(s => s.trim()).filter(Boolean) : [];
            symbols.push({ name: funcMatch[1], kind: 'Function', detail: `دالة(${params.join('، ')})`, line: i, params });
        }

        const classMatch = line.match(/^صنف\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/);
        if (classMatch) {
            symbols.push({ name: classMatch[1], kind: 'Class', detail: 'صنف', line: i });
        }

        const varMatch = line.match(/^متغير\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/);
        if (varMatch) {
            symbols.push({ name: varMatch[1], kind: 'Variable', detail: 'متغير', line: i });
        }

        const constMatch = line.match(/^ثابت\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/);
        if (constMatch) {
            symbols.push({ name: constMatch[1], kind: 'Constant', detail: 'ثابت', line: i });
        }
    }

    return symbols;
}

function findHover(word: string): string | null {
    const kw = KEYWORDS.find(k => k.label === word);
    if (kw) { return `**${kw.detail}**\n\n${kw.documentation}`; }

    const fn = BUILTINS.find(f => f.label === word);
    if (fn) { return `**${fn.detail}**\n\n${fn.documentation}`; }

    const tp = TYPES.find(t => t.label === word);
    if (tp) { return `**${tp.detail}**\n\n${tp.documentation}`; }

    return null;
}

function extractThisProperties(code: string): string[] {
    const props = new Set<string>();
    const regex = /هذا\.([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/g;
    let m;
    while ((m = regex.exec(code)) !== null) {
        props.add(m[1]);
    }
    return Array.from(props);
}

function getSignatureParams(funcName: string, symbols: ParsedSymbol[]): string[] | null {
    const sym = symbols.find(s => s.name === funcName && s.params);
    return sym?.params || null;
}

function countActiveParam(textBeforeCursor: string): number {
    const afterParen = textBeforeCursor.substring(textBeforeCursor.lastIndexOf('(') + 1);
    return (afterParen.match(/[،,]/g) || []).length;
}

// ─── الاختبارات ───

function test1_KeywordCompletionsExist() {
    assert.ok(KEYWORDS.length >= 10, 'يجب أن يكون هناك على الأقل 10 كلمات مفتاحية');
    console.log('✅ 1. الكلمات المفتاحية موجودة');
}

function test2_BuiltinFunctionsExist() {
    assert.ok(BUILTINS.length >= 5, 'يجب أن يكون هناك على الأقل 5 دوال مدمجة');
    console.log('✅ 2. الدوال المدمجة موجودة');
}

function test3_TypesExist() {
    assert.ok(TYPES.length >= 5, 'يجب أن يكون هناك على الأقل 5 أنواع');
    console.log('✅ 3. الأنواع موجودة');
}

function test4_AllKeywordsHaveInsertText() {
    for (const kw of KEYWORDS) {
        assert.ok(kw.insertText.length > 0, `الكلمة ${kw.label} يجب أن تحتوي على insertText`);
    }
    console.log('✅ 4. جميع الكلمات تحتوي على نص إدراج');
}

function test5_ParseFunctions() {
    const code = `دالة جمع(أ، ب)\n   ارجع أ + ب\nنهاية\n\nدالة حساب(س)\n   ارجع س * 2\nنهاية`;
    const symbols = parseSymbols(code);
    const funcs = symbols.filter(s => s.kind === 'Function');
    assert.strictEqual(funcs.length, 2, 'يجب أن يكتشف دالتين');
    assert.strictEqual(funcs[0].name, 'جمع');
    assert.deepStrictEqual(funcs[0].params, ['أ', 'ب']);
    assert.strictEqual(funcs[1].name, 'حساب');
    console.log('✅ 5. تحليل الدوال يعمل');
}

function test6_ParseClasses() {
    const code = `صنف شخص\n   باني(اسم)\n      هذا.اسم = اسم\n   نهاية\nنهاية\n\nصنف طالب يرث شخص\nنهاية`;
    const symbols = parseSymbols(code);
    const classes = symbols.filter(s => s.kind === 'Class');
    assert.strictEqual(classes.length, 2, 'يجب أن يكتشف صنفين');
    assert.strictEqual(classes[0].name, 'شخص');
    assert.strictEqual(classes[1].name, 'طالب');
    console.log('✅ 6. تحليل الأصناف يعمل');
}

function test7_ParseVariablesAndConstants() {
    const code = `متغير اسم = "أحمد"\nثابت PI = 3.14\nمتغير عمر = 25`;
    const symbols = parseSymbols(code);
    const vars = symbols.filter(s => s.kind === 'Variable');
    const consts = symbols.filter(s => s.kind === 'Constant');
    assert.strictEqual(vars.length, 2, 'يجب أن يكتشف متغيرين');
    assert.strictEqual(consts.length, 1, 'يجب أن يكتشف ثابت واحد');
    console.log('✅ 7. تحليل المتغيرات والثوابت');
}

function test8_HoverKeyword() {
    const hover = findHover('إذا');
    assert.ok(hover !== null, 'يجب أن يجد تلميحة لـ إذا');
    assert.ok(hover!.includes('if'), 'يجب أن تحتوي على الترجمة الإنجليزية');
    console.log('✅ 8. التلميحة للكلمات المفتاحية');
}

function test9_HoverBuiltinFunction() {
    const hover = findHover('اطبع');
    assert.ok(hover !== null, 'يجب أن يجد تلميحة لـ اطبع');
    assert.ok(hover!.includes('print'), 'يجب أن تحتوي على الترجمة');
    console.log('✅ 9. التلميحة للدوال المدمجة');
}

function test10_HoverType() {
    const hover = findHover('رقم');
    assert.ok(hover !== null, 'يجب أن يجد تلميحة للنوع');
    console.log('✅ 10. التلميحة للأنواع');
}

function test11_HoverUnknownWord() {
    const hover = findHover('كلمة_غير_موجودة');
    assert.strictEqual(hover, null, 'يجب ألا يجد تلميحة لكلمة غير معروفة');
    console.log('✅ 11. لا تلميحة للكلمات غير المعروفة');
}

function test12_ThisPropertyExtraction() {
    const code = `صنف شخص\n   باني(اسم، عمر)\n      هذا.اسم = اسم\n      هذا.عمر = عمر\n   نهاية\n   دالة وصف()\n      ارجع هذا.اسم + " - " + هذا.عمر\n   نهاية\nنهاية`;
    const props = extractThisProperties(code);
    assert.ok(props.includes('اسم'), 'يجب أن يجد خاصية اسم');
    assert.ok(props.includes('عمر'), 'يجب أن يجد خاصية عمر');
    assert.strictEqual(props.length, 2, 'يجب أن يكون هناك خاصيتين فقط');
    console.log('✅ 12. استخراج خصائص هذا.');
}

function test13_SignatureHelp() {
    const code = `دالة جمع(أ، ب)\n   ارجع أ + ب\nنهاية`;
    const symbols = parseSymbols(code);
    const params = getSignatureParams('جمع', symbols);
    assert.ok(params !== null, 'يجب أن يجد معاملات الدالة');
    assert.deepStrictEqual(params, ['أ', 'ب']);
    console.log('✅ 13. مساعدة التوقيعات');
}

function test14_ActiveParameterCount() {
    assert.strictEqual(countActiveParam('جمع(1'), 0, 'المعامل الأول');
    assert.strictEqual(countActiveParam('جمع(1، '), 1, 'المعامل الثاني');
    assert.strictEqual(countActiveParam('جمع(1، 2، '), 2, 'المعامل الثالث');
    assert.strictEqual(countActiveParam('جمع('), 0, 'بدون معاملات');
    console.log('✅ 14. حساب المعامل النشط');
}

function test15_FunctionWithNoParams() {
    const code = `دالة مرحبا()\n   اطبع_سطر("مرحباً")\nنهاية`;
    const symbols = parseSymbols(code);
    const func = symbols.find(s => s.name === 'مرحبا');
    assert.ok(func !== null, 'يجب أن يجد الدالة');
    assert.deepStrictEqual(func!.params, [], 'دالة بدون معاملات');
    console.log('✅ 15. دالة بدون معاملات');
}

// ─── تشغيل ───
test1_KeywordCompletionsExist();
test2_BuiltinFunctionsExist();
test3_TypesExist();
test4_AllKeywordsHaveInsertText();
test5_ParseFunctions();
test6_ParseClasses();
test7_ParseVariablesAndConstants();
test8_HoverKeyword();
test9_HoverBuiltinFunction();
test10_HoverType();
test11_HoverUnknownWord();
test12_ThisPropertyExtraction();
test13_SignatureHelp();
test14_ActiveParameterCount();
test15_FunctionWithNoParams();

console.log('\n✅ جميع اختبارات الإكمال الذكي نجحت (15/15)');
