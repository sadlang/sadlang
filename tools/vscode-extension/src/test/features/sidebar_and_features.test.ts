/**
 * @file sidebar_and_features.test.ts
 * @brief اختبارات الشريط الجانبي والميزات المتقدمة — 15 اختبار
 *
 * يختبر بنية الشجرة، التصنيفات، الأيقونات، الأوامر،
 * وتكامل الميزات الجديدة.
 */

import * as assert from 'assert';

// ─── محاكاة بنية الشريط الجانبي ───

interface SidebarCategory {
    label: string;
    icon: string;
    children: SidebarItem[];
}

interface SidebarItem {
    label: string;
    icon: string;
    command: string;
    shortcut?: string;
}

function getSidebarTree(): SidebarCategory[] {
    return [
        {
            label: '🚀 التشغيل والتصحيح', icon: 'play',
            children: [
                { label: 'تشغيل الملف', icon: 'play', command: 'sad.runFile', shortcut: 'F5' },
                { label: 'تشغيل بالمفسر', icon: 'play-circle', command: 'sad.runWithInterpreter' },
                { label: 'ترجمة وتشغيل', icon: 'zap', command: 'sad.runWithCompiler' },
                { label: 'تشغيل النص المحدد', icon: 'selection', command: 'sad.runSelection', shortcut: 'Ctrl+Shift+Enter' },
                { label: 'فتح طرفية ص', icon: 'terminal-bash', command: 'sad.openTerminal', shortcut: 'Ctrl+Shift+`' },
            ]
        },
        {
            label: '🔬 التحليل والمراجعة', icon: 'search',
            children: [
                { label: 'تحليل التعقيد', icon: 'pulse', command: 'sad.analyzeComplexity' },
                { label: 'مراجعة الكود', icon: 'checklist', command: 'sad.reviewCode' },
                { label: 'مستكشف AST', icon: 'list-tree', command: 'sad.showAST', shortcut: 'Ctrl+Shift+A' },
            ]
        },
        {
            label: '✏️ التحرير والتنسيق', icon: 'edit',
            children: [
                { label: 'إعادة التسمية الذكية', icon: 'edit', command: 'sad.smartRename', shortcut: 'F6' },
                { label: 'تنظيم الاستيرادات', icon: 'sort-precedence', command: 'sad.organizeImports', shortcut: 'Ctrl+Shift+O' },
            ]
        },
        {
            label: '🧭 التنقل والعلامات', icon: 'bookmark',
            children: [
                { label: 'إضافة/إزالة علامة', icon: 'bookmark', command: 'sad.toggleBookmark', shortcut: 'Ctrl+F2' },
            ]
        },
    ];
}

// ─── محاكاة تحليل التعقيد ───

function analyzeCyclomaticComplexity(code: string): number {
    let complexity = 1; // base
    const controlKeywords = /\b(إذا|اذا|بينما|لكل|طابق|عندما|حاول|امسك|و|أو|\|\||&&)\b/g;
    const matches = code.match(controlKeywords);
    if (matches) { complexity += matches.length; }
    return complexity;
}

function getComplexityLevel(complexity: number): { level: string; color: string } {
    if (complexity <= 5) { return { level: 'بسيط', color: '🟢' }; }
    if (complexity <= 10) { return { level: 'معتدل', color: '🟡' }; }
    if (complexity <= 20) { return { level: 'معقد', color: '🟠' }; }
    return { level: 'خطير', color: '🔴' };
}

// ─── محاكاة تنظيم الاستيرادات ───

function organizeImports(code: string): string {
    const lines = code.split('\n');
    const imports: string[] = [];
    const nonImports: string[] = [];

    for (const line of lines) {
        if (line.trim().startsWith('استورد')) {
            imports.push(line);
        } else {
            nonImports.push(line);
        }
    }

    // ترتيب أبجدي
    imports.sort();

    // إزالة المكرر
    const unique = [...new Set(imports)];

    return [...unique, '', ...nonImports.filter(l => l.trim() !== '' || nonImports.indexOf(l) > 0)].join('\n');
}

// ─── محاكاة مدير العلامات ───

interface Bookmark {
    file: string;
    line: number;
    label: string;
    category: string;
}

class BookmarkStore {
    private bookmarks: Bookmark[] = [];

    add(file: string, line: number, label: string, category: string): void {
        this.bookmarks.push({ file, line, label, category });
    }

    remove(file: string, line: number): void {
        this.bookmarks = this.bookmarks.filter(b => !(b.file === file && b.line === line));
    }

    getAll(): Bookmark[] {
        return [...this.bookmarks];
    }

    getByCategory(category: string): Bookmark[] {
        return this.bookmarks.filter(b => b.category === category);
    }

    toggle(file: string, line: number, label: string, category: string): boolean {
        const existing = this.bookmarks.find(b => b.file === file && b.line === line);
        if (existing) {
            this.remove(file, line);
            return false; // removed
        }
        this.add(file, line, label, category);
        return true; // added
    }
}

// ─── الاختبارات ───

function test1_SidebarTreeStructure() {
    const tree = getSidebarTree();
    assert.ok(tree.length >= 4, 'يجب أن يكون هناك 4 تصنيفات على الأقل');
    console.log('✅ 1. بنية الشجرة صحيحة');
}

function test2_AllCategoriesHaveChildren() {
    const tree = getSidebarTree();
    for (const cat of tree) {
        assert.ok(cat.children.length > 0, `التصنيف ${cat.label} يجب أن يحتوي على عناصر`);
        assert.ok(cat.icon, `التصنيف ${cat.label} يجب أن يحتوي على أيقونة`);
    }
    console.log('✅ 2. جميع التصنيفات لها عناصر وأيقونات');
}

function test3_AllItemsHaveCommands() {
    const tree = getSidebarTree();
    for (const cat of tree) {
        for (const item of cat.children) {
            assert.ok(item.command.startsWith('sad.'), `الأمر ${item.command} يجب أن يبدأ بـ sad.`);
            assert.ok(item.icon, `العنصر ${item.label} يجب أن يحتوي على أيقونة`);
        }
    }
    console.log('✅ 3. جميع العناصر لها أوامر صحيحة');
}

function test4_TotalItemCount() {
    const tree = getSidebarTree();
    const total = tree.reduce((sum, cat) => sum + cat.children.length, 0);
    assert.ok(total >= 10, `يجب أن يكون هناك 10 عناصر على الأقل (الحالي: ${total})`);
    console.log('✅ 4. عدد العناصر كافٍ');
}

function test5_SimpleComplexity() {
    const code = `دالة مرحبا()\n   اطبع_سطر("مرحباً")\nنهاية`;
    const c = analyzeCyclomaticComplexity(code);
    assert.strictEqual(c, 1, 'كود بسيط يجب أن يكون تعقيده 1');
    const level = getComplexityLevel(c);
    assert.strictEqual(level.level, 'بسيط');
    console.log('✅ 5. تعقيد بسيط');
}

function test6_ModerateComplexity() {
    const code = `دالة اختبار(س)\n   إذا (س > 0)\n      بينما (س > 10)\n         إذا (س > 50 و س < 100)\n            اطبع(س)\n         نهاية\n      نهاية\n   نهاية\nنهاية`;
    const c = analyzeCyclomaticComplexity(code);
    assert.ok(c >= 4, `التعقيد يجب أن يكون 4 على الأقل (الحالي: ${c})`);
    console.log('✅ 6. تعقيد متوسط');
}

function test7_HighComplexity() {
    const c = 25;
    const level = getComplexityLevel(c);
    assert.strictEqual(level.level, 'خطير');
    assert.strictEqual(level.color, '🔴');
    console.log('✅ 7. تعقيد مرتفع');
}

function test8_OrganizeImports() {
    const code = `استورد "ب.ص"\nاستورد "أ.ص"\nاستورد "ب.ص"\n\nمتغير س = 1`;
    const organized = organizeImports(code);
    const lines = organized.split('\n');
    assert.ok(lines[0].includes('أ.ص'), 'الاستيراد الأول يجب أن يكون أ.ص');
    assert.ok(lines[1].includes('ب.ص'), 'الاستيراد الثاني يجب أن يكون ب.ص');
    // التأكد من إزالة المكرر
    const importLines = lines.filter(l => l.trim().startsWith('استورد'));
    assert.strictEqual(importLines.length, 2, 'يجب إزالة الاستيراد المكرر');
    console.log('✅ 8. تنظيم الاستيرادات');
}

function test9_BookmarkAdd() {
    const store = new BookmarkStore();
    store.add('ملف.ص', 10, 'علامة اختبار', 'عام');
    const all = store.getAll();
    assert.strictEqual(all.length, 1);
    assert.strictEqual(all[0].line, 10);
    console.log('✅ 9. إضافة علامة');
}

function test10_BookmarkToggle() {
    const store = new BookmarkStore();
    const added = store.toggle('ملف.ص', 5, 'علامة', 'عام');
    assert.strictEqual(added, true, 'يجب أن يُضاف');
    const removed = store.toggle('ملف.ص', 5, 'علامة', 'عام');
    assert.strictEqual(removed, false, 'يجب أن يُزال');
    assert.strictEqual(store.getAll().length, 0);
    console.log('✅ 10. تبديل العلامة');
}

function test11_BookmarkByCategory() {
    const store = new BookmarkStore();
    store.add('أ.ص', 1, 'أ', 'مهم');
    store.add('ب.ص', 2, 'ب', 'عام');
    store.add('ج.ص', 3, 'ج', 'مهم');
    const important = store.getByCategory('مهم');
    assert.strictEqual(important.length, 2);
    console.log('✅ 11. تصنيف العلامات');
}

function test12_EmptyCodeComplexity() {
    const c = analyzeCyclomaticComplexity('');
    assert.strictEqual(c, 1, 'الكود الفارغ تعقيده 1');
    console.log('✅ 12. تعقيد الكود الفارغ');
}

function test13_OrganizeEmptyImports() {
    const code = `متغير س = 1\nاطبع_سطر(س)`;
    const organized = organizeImports(code);
    assert.ok(!organized.includes('استورد'), 'لا يجب أن تكون هناك استيرادات');
    console.log('✅ 13. تنظيم بلا استيرادات');
}

function test14_BookmarkRemoveNonExistent() {
    const store = new BookmarkStore();
    store.add('أ.ص', 1, 'أ', 'عام');
    store.remove('ب.ص', 99); // غير موجود
    assert.strictEqual(store.getAll().length, 1, 'يجب ألا يتأثر');
    console.log('✅ 14. إزالة علامة غير موجودة');
}

function test15_ComplexityLevelBoundaries() {
    assert.strictEqual(getComplexityLevel(1).level, 'بسيط');
    assert.strictEqual(getComplexityLevel(5).level, 'بسيط');
    assert.strictEqual(getComplexityLevel(6).level, 'معتدل');
    assert.strictEqual(getComplexityLevel(10).level, 'معتدل');
    assert.strictEqual(getComplexityLevel(11).level, 'معقد');
    assert.strictEqual(getComplexityLevel(20).level, 'معقد');
    assert.strictEqual(getComplexityLevel(21).level, 'خطير');
    console.log('✅ 15. حدود مستويات التعقيد');
}

// ─── تشغيل ───
test1_SidebarTreeStructure();
test2_AllCategoriesHaveChildren();
test3_AllItemsHaveCommands();
test4_TotalItemCount();
test5_SimpleComplexity();
test6_ModerateComplexity();
test7_HighComplexity();
test8_OrganizeImports();
test9_BookmarkAdd();
test10_BookmarkToggle();
test11_BookmarkByCategory();
test12_EmptyCodeComplexity();
test13_OrganizeEmptyImports();
test14_BookmarkRemoveNonExistent();
test15_ComplexityLevelBoundaries();

console.log('\n✅ جميع اختبارات الشريط الجانبي والميزات نجحت (15/15)');
