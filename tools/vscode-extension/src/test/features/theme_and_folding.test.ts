/**
 * @file theme_and_folding.test.ts
 * @brief اختبارات السمات اللونية ومزود الطي ومزود المسار — 15 اختبار
 *
 * يختبر بنية السمات، ألوان الرموز، نطاقات الطي،
 * وتوفير رموز المستند للمسار.
 */

import * as assert from 'assert';

// ─── محاكاة بنية السمة اللونية ───

interface TokenColorRule {
    name?: string;
    scope: string | string[];
    settings: {
        foreground?: string;
        fontStyle?: string;
    };
}

interface ThemeData {
    name: string;
    type: 'dark' | 'light';
    colors: Record<string, string>;
    tokenColors: TokenColorRule[];
}

function createDarkTheme(): ThemeData {
    return {
        name: 'ص - داكن',
        type: 'dark',
        colors: {
            'editor.background': '#1a1a2e',
            'editor.foreground': '#e0e0e0',
            'activityBar.background': '#0f0f23',
            'sideBar.background': '#16162b',
            'statusBar.background': '#0f0f23',
            'terminal.background': '#1a1a2e',
            'editorLineNumber.foreground': '#4a4a6a',
        },
        tokenColors: [
            { name: 'comments', scope: 'comment', settings: { foreground: '#6a6a8a', fontStyle: 'italic' } },
            { name: 'strings', scope: 'string', settings: { foreground: '#a8db8a' } },
            { name: 'keywords', scope: 'keyword.control', settings: { foreground: '#c792ea' } },
            { name: 'functions', scope: 'entity.name.function', settings: { foreground: '#82aaff' } },
            { name: 'types', scope: 'support.type.builtin', settings: { foreground: '#89ddff', fontStyle: 'italic' } },
            { name: 'classes', scope: 'entity.name.class', settings: { foreground: '#7ce0c3', fontStyle: 'bold' } },
            { name: 'numbers', scope: 'constant.numeric', settings: { foreground: '#f78c6c' } },
            { name: 'operators', scope: 'keyword.operator', settings: { foreground: '#89ddff' } },
            { name: 'builtins', scope: 'support.function.builtin', settings: { foreground: '#82aaff', fontStyle: 'bold' } },
            { name: 'variables', scope: 'variable', settings: { foreground: '#e0e0e0' } },
        ]
    };
}

function createLightTheme(): ThemeData {
    return {
        name: 'ص - فاتح',
        type: 'light',
        colors: {
            'editor.background': '#fafafa',
            'editor.foreground': '#333333',
            'activityBar.background': '#e8e8e8',
            'sideBar.background': '#f0f0f0',
            'statusBar.background': '#e0e0e0',
            'terminal.background': '#fafafa',
            'editorLineNumber.foreground': '#999999',
        },
        tokenColors: [
            { name: 'comments', scope: 'comment', settings: { foreground: '#999999', fontStyle: 'italic' } },
            { name: 'strings', scope: 'string', settings: { foreground: '#2e7d32' } },
            { name: 'keywords', scope: 'keyword.control', settings: { foreground: '#7b1fa2' } },
            { name: 'functions', scope: 'entity.name.function', settings: { foreground: '#1565c0' } },
            { name: 'types', scope: 'support.type.builtin', settings: { foreground: '#00838f', fontStyle: 'italic' } },
            { name: 'classes', scope: 'entity.name.class', settings: { foreground: '#00695c', fontStyle: 'bold' } },
            { name: 'numbers', scope: 'constant.numeric', settings: { foreground: '#e65100' } },
            { name: 'operators', scope: 'keyword.operator', settings: { foreground: '#546e7a' } },
            { name: 'builtins', scope: 'support.function.builtin', settings: { foreground: '#1565c0', fontStyle: 'bold' } },
            { name: 'variables', scope: 'variable', settings: { foreground: '#333333' } },
        ]
    };
}

// ─── محاكاة مزود الطي ───

interface FoldingRange {
    start: number;
    end: number;
    kind: 'region' | 'comment' | 'imports';
}

function computeFoldingRanges(code: string): FoldingRange[] {
    const lines = code.split('\n');
    const ranges: FoldingRange[] = [];
    const stack: number[] = [];

    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();

        // كتل التعليقات
        if (trimmed.startsWith('#*')) {
            const start = i;
            while (i < lines.length && !lines[i].includes('*#')) { i++; }
            ranges.push({ start, end: i, kind: 'comment' });
            continue;
        }

        // بداية كتلة
        if (/^(دالة|صنف|إذا|اذا|بينما|لكل|طابق|حاول)\b/.test(trimmed)) {
            stack.push(i);
        }

        // نهاية كتلة
        if (trimmed === 'نهاية' && stack.length > 0) {
            const start = stack.pop()!;
            ranges.push({ start, end: i, kind: 'region' });
        }
    }

    return ranges;
}

// ─── محاكاة مزود المسار (Breadcrumb) ───

interface DocumentSymbol {
    name: string;
    kind: 'function' | 'class' | 'variable' | 'constructor';
    range: { start: number; end: number };
    children: DocumentSymbol[];
}

function extractDocumentSymbols(code: string): DocumentSymbol[] {
    const lines = code.split('\n');
    const symbols: DocumentSymbol[] = [];
    const symbolStack: { symbol: DocumentSymbol; depth: number }[] = [];

    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();

        const funcMatch = trimmed.match(/^دالة\s+(\S+)/);
        if (funcMatch) {
            const sym: DocumentSymbol = { name: funcMatch[1].replace('(', ''), kind: 'function', range: { start: i, end: i }, children: [] };
            symbolStack.push({ symbol: sym, depth: symbolStack.length });
            continue;
        }

        const classMatch = trimmed.match(/^صنف\s+(\S+)/);
        if (classMatch) {
            const sym: DocumentSymbol = { name: classMatch[1], kind: 'class', range: { start: i, end: i }, children: [] };
            symbolStack.push({ symbol: sym, depth: symbolStack.length });
            continue;
        }

        if (trimmed === 'نهاية' && symbolStack.length > 0) {
            const { symbol } = symbolStack.pop()!;
            symbol.range.end = i;
            if (symbolStack.length > 0) {
                symbolStack[symbolStack.length - 1].symbol.children.push(symbol);
            } else {
                symbols.push(symbol);
            }
        }
    }

    return symbols;
}

// ─── الاختبارات ───

function test1_DarkThemeStructure() {
    const theme = createDarkTheme();
    assert.strictEqual(theme.type, 'dark');
    assert.ok(theme.colors['editor.background'], 'يجب أن يحتوي على لون خلفية');
    assert.ok(theme.tokenColors.length >= 10, 'يجب أن يحتوي على 10 قواعد ألوان على الأقل');
    console.log('✅ 1. بنية السمة الداكنة');
}

function test2_LightThemeStructure() {
    const theme = createLightTheme();
    assert.strictEqual(theme.type, 'light');
    assert.ok(theme.colors['editor.background']);
    assert.ok(theme.tokenColors.length >= 10);
    console.log('✅ 2. بنية السمة الفاتحة');
}

function test3_DarkHexColors() {
    const theme = createDarkTheme();
    const hexPattern = /^#[0-9a-fA-F]{6}$/;
    for (const [key, value] of Object.entries(theme.colors)) {
        assert.ok(hexPattern.test(value), `${key}: ${value} ليس لوناً صحيحاً`);
    }
    console.log('✅ 3. ألوان السمة الداكنة بصيغة hex صحيحة');
}

function test4_LightHexColors() {
    const theme = createLightTheme();
    const hexPattern = /^#[0-9a-fA-F]{6}$/;
    for (const [key, value] of Object.entries(theme.colors)) {
        assert.ok(hexPattern.test(value), `${key}: ${value} ليس لوناً صحيحاً`);
    }
    console.log('✅ 4. ألوان السمة الفاتحة بصيغة hex صحيحة');
}

function test5_TokenColorScopes() {
    const theme = createDarkTheme();
    const requiredScopes = ['comment', 'string', 'keyword.control', 'entity.name.function'];
    for (const scope of requiredScopes) {
        const found = theme.tokenColors.find(tc =>
            (typeof tc.scope === 'string' && tc.scope === scope) ||
            (Array.isArray(tc.scope) && tc.scope.includes(scope))
        );
        assert.ok(found, `النطاق ${scope} يجب أن يكون موجوداً`);
    }
    console.log('✅ 5. نطاقات الرموز الأساسية');
}

function test6_TokenColorForegrounds() {
    const theme = createDarkTheme();
    for (const tc of theme.tokenColors) {
        if (tc.settings.foreground) {
            assert.ok(tc.settings.foreground.startsWith('#'), `اللون يجب أن يبدأ بـ #`);
        }
    }
    console.log('✅ 6. ألوان الرموز صحيحة');
}

function test7_ThemeContrast() {
    const dark = createDarkTheme();
    const light = createLightTheme();
    assert.notStrictEqual(dark.colors['editor.background'], light.colors['editor.background']);
    assert.notStrictEqual(dark.colors['editor.foreground'], light.colors['editor.foreground']);
    console.log('✅ 7. تباين بين السمتين');
}

function test8_SimpleFolding() {
    const code = `دالة مرحبا()\n   اطبع("أهلاً")\nنهاية`;
    const ranges = computeFoldingRanges(code);
    assert.strictEqual(ranges.length, 1);
    assert.strictEqual(ranges[0].start, 0);
    assert.strictEqual(ranges[0].end, 2);
    assert.strictEqual(ranges[0].kind, 'region');
    console.log('✅ 8. طي دالة بسيطة');
}

function test9_NestedFolding() {
    const code = `صنف حيوان\n   دالة صوت()\n      اطبع("...")\n   نهاية\nنهاية`;
    const ranges = computeFoldingRanges(code);
    assert.strictEqual(ranges.length, 2);
    console.log('✅ 9. طي متداخل');
}

function test10_CommentFolding() {
    const code = `#* تعليق\nطويل\nجداً *#\nمتغير س = 1`;
    const ranges = computeFoldingRanges(code);
    assert.ok(ranges.some(r => r.kind === 'comment'), 'يجب طي تعليق الكتلة');
    console.log('✅ 10. طي تعليق كتلة');
}

function test11_NoFoldingEmptyCode() {
    const ranges = computeFoldingRanges('');
    assert.strictEqual(ranges.length, 0);
    console.log('✅ 11. لا طي في كود فارغ');
}

function test12_BreadcrumbSimpleFunction() {
    const code = `دالة حساب(أ)\n   ارجع أ * 2\nنهاية`;
    const symbols = extractDocumentSymbols(code);
    assert.strictEqual(symbols.length, 1);
    assert.ok(symbols[0].name.includes('حساب'));
    assert.strictEqual(symbols[0].kind, 'function');
    console.log('✅ 12. رمز دالة في المسار');
}

function test13_BreadcrumbClassWithMethods() {
    const code = `صنف شكل\n   دالة ارسم()\n      اطبع("رسم")\n   نهاية\nنهاية`;
    const symbols = extractDocumentSymbols(code);
    assert.strictEqual(symbols.length, 1);
    assert.strictEqual(symbols[0].kind, 'class');
    assert.strictEqual(symbols[0].children.length, 1);
    assert.strictEqual(symbols[0].children[0].kind, 'function');
    console.log('✅ 13. صنف مع دالة في المسار');
}

function test14_BreadcrumbMultipleSymbols() {
    const code = `دالة أ()\n   ارجع 1\nنهاية\nدالة ب()\n   ارجع 2\nنهاية`;
    const symbols = extractDocumentSymbols(code);
    assert.strictEqual(symbols.length, 2);
    console.log('✅ 14. رموز متعددة في المسار');
}

function test15_BreadcrumbEmptyCode() {
    const symbols = extractDocumentSymbols('');
    assert.strictEqual(symbols.length, 0);
    console.log('✅ 15. لا رموز في كود فارغ');
}

// ─── تشغيل ───
test1_DarkThemeStructure();
test2_LightThemeStructure();
test3_DarkHexColors();
test4_LightHexColors();
test5_TokenColorScopes();
test6_TokenColorForegrounds();
test7_ThemeContrast();
test8_SimpleFolding();
test9_NestedFolding();
test10_CommentFolding();
test11_NoFoldingEmptyCode();
test12_BreadcrumbSimpleFunction();
test13_BreadcrumbClassWithMethods();
test14_BreadcrumbMultipleSymbols();
test15_BreadcrumbEmptyCode();

console.log('\n✅ جميع اختبارات السمات والطي والمسار نجحت (15/15)');
