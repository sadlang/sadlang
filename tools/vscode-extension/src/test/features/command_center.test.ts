/**
 * @file command_center.test.ts
 * @brief اختبارات لوحة التحكم — 10 اختبارات
 *
 * يختبر بيانات الميزات، التصنيفات، البحث،
 * وصحة أوامر VS Code المسجلة.
 */

import * as assert from 'assert';

// ─── محاكاة بيانات لوحة التحكم ───

interface FeatureItem {
    id: string;
    name: string;
    description: string;
    icon: string;
    command: string;
    shortcut?: string;
    category: string;
}

function getFeatures(): FeatureItem[] {
    return [
        { id: 'run', name: 'تشغيل الملف', description: 'تشغيل الملف الحالي', icon: '▶️', command: 'sad.runFile', shortcut: 'F5', category: 'التشغيل والتصحيح' },
        { id: 'debug', name: 'التصحيح المرئي', description: 'تصحيح خطوة بخطوة', icon: '🐛', command: 'sad.debug', category: 'التشغيل والتصحيح' },
        { id: 'complexity', name: 'تحليل التعقيد', description: 'تحليل Cyclomatic', icon: '📊', command: 'sad.analyzeComplexity', category: 'التحليل والمراجعة' },
        { id: 'smartRename', name: 'إعادة التسمية', description: 'إعادة تسمية ذكية', icon: '✏️', command: 'sad.smartRename', shortcut: 'F6', category: 'التحرير والتنسيق' },
        { id: 'bookmark', name: 'علامة', description: 'إضافة علامة', icon: '🔖', command: 'sad.toggleBookmark', shortcut: 'Ctrl+F2', category: 'التنقل والعلامات' },
        { id: 'generateDoc', name: 'توليد التوثيق', description: 'توثيق تلقائي', icon: '📄', command: 'sad.generateDoc', shortcut: 'Ctrl+Shift+D', category: 'التوثيق والمشاركة' },
        { id: 'aiSuggest', name: 'اقتراحات ذكية', description: 'اقتراحات سياقية', icon: '🤖', command: 'sad.aiSuggest', category: 'المساعد الذكي' },
        { id: 'dashboard', name: 'لوحة القيادة', description: 'إحصائيات المشروع', icon: '📊', command: 'sad.showDashboard', category: 'المشروع والأدوات' },
        { id: 'rtl', name: 'تبديل RTL', description: 'تبديل الاتجاه', icon: '↔️', command: 'sad.toggleRTL', shortcut: 'Ctrl+Shift+;', category: 'العرض والاتجاه' },
        { id: 'foldFunc', name: 'طي الدوال', description: 'طي جميع الدوال', icon: '📂', command: 'sad.foldFunctions', category: 'الطي والتنسيق' },
    ];
}

function getCategories(features: FeatureItem[]): Map<string, FeatureItem[]> {
    const map = new Map<string, FeatureItem[]>();
    for (const f of features) {
        if (!map.has(f.category)) { map.set(f.category, []); }
        map.get(f.category)!.push(f);
    }
    return map;
}

function searchFeatures(features: FeatureItem[], query: string): FeatureItem[] {
    const q = query.toLowerCase();
    return features.filter(f =>
        f.name.toLowerCase().includes(q) ||
        f.description.toLowerCase().includes(q) ||
        f.category.toLowerCase().includes(q) ||
        f.command.toLowerCase().includes(q)
    );
}

function getFeaturesWithShortcuts(features: FeatureItem[]): FeatureItem[] {
    return features.filter(f => f.shortcut);
}

// ─── الاختبارات ───

function testFeaturesNotEmpty() {
    const features = getFeatures();
    assert.ok(features.length > 0, 'يجب أن تكون قائمة الميزات غير فارغة');
    console.log('✅ 1. الميزات غير فارغة');
}

function testAllFeaturesHaveRequiredFields() {
    const features = getFeatures();
    for (const f of features) {
        assert.ok(f.id, `الميزة يجب أن تحتوي على id`);
        assert.ok(f.name, `الميزة ${f.id} يجب أن تحتوي على name`);
        assert.ok(f.command, `الميزة ${f.id} يجب أن تحتوي على command`);
        assert.ok(f.command.startsWith('sad.'), `الأمر يجب أن يبدأ بـ sad.`);
        assert.ok(f.category, `الميزة ${f.id} يجب أن تحتوي على category`);
        assert.ok(f.icon, `الميزة ${f.id} يجب أن تحتوي على icon`);
    }
    console.log('✅ 2. جميع الميزات تحتوي على الحقول المطلوبة');
}

function testUniqueIds() {
    const features = getFeatures();
    const ids = features.map(f => f.id);
    const uniqueIds = new Set(ids);
    assert.strictEqual(ids.length, uniqueIds.size, 'يجب أن تكون جميع المعرفات فريدة');
    console.log('✅ 3. المعرفات فريدة');
}

function testUniqueCommands() {
    const features = getFeatures();
    const commands = features.map(f => f.command);
    const uniqueCommands = new Set(commands);
    assert.strictEqual(commands.length, uniqueCommands.size, 'يجب أن تكون جميع الأوامر فريدة');
    console.log('✅ 4. الأوامر فريدة');
}

function testCategoriesGrouping() {
    const features = getFeatures();
    const categories = getCategories(features);
    assert.ok(categories.size > 0, 'يجب أن يكون هناك تصنيف واحد على الأقل');
    for (const [cat, items] of categories) {
        assert.ok(items.length > 0, `التصنيف ${cat} يجب أن يحتوي على ميزة واحدة على الأقل`);
    }
    console.log('✅ 5. التصنيفات صحيحة');
}

function testSearchByName() {
    const features = getFeatures();
    const results = searchFeatures(features, 'تشغيل');
    assert.ok(results.length > 0, 'يجب أن يجد نتائج للبحث عن "تشغيل"');
    assert.ok(results.some(r => r.id === 'run'), 'يجب أن تتضمن النتائج ميزة التشغيل');
    console.log('✅ 6. البحث بالاسم يعمل');
}

function testSearchByCategory() {
    const features = getFeatures();
    const results = searchFeatures(features, 'التحليل');
    assert.ok(results.length > 0, 'يجب أن يجد نتائج في فئة التحليل');
    console.log('✅ 7. البحث بالتصنيف يعمل');
}

function testSearchNoResults() {
    const features = getFeatures();
    const results = searchFeatures(features, 'xyznonexistent');
    assert.strictEqual(results.length, 0, 'يجب ألا يجد نتائج لبحث غير موجود');
    console.log('✅ 8. البحث بلا نتائج يعمل');
}

function testShortcutsFilter() {
    const features = getFeatures();
    const withShortcuts = getFeaturesWithShortcuts(features);
    assert.ok(withShortcuts.length > 0, 'يجب أن يكون هناك ميزات مع اختصارات');
    assert.ok(withShortcuts.length < features.length, 'ليست جميع الميزات لها اختصارات');
    console.log('✅ 9. تصفية الاختصارات تعمل');
}

function testSearchByCommand() {
    const features = getFeatures();
    const results = searchFeatures(features, 'sad.runFile');
    assert.strictEqual(results.length, 1, 'يجب أن يجد ميزة واحدة بالأمر');
    assert.strictEqual(results[0].id, 'run');
    console.log('✅ 10. البحث بالأمر يعمل');
}

// ─── تشغيل الاختبارات ───
testFeaturesNotEmpty();
testAllFeaturesHaveRequiredFields();
testUniqueIds();
testUniqueCommands();
testCategoriesGrouping();
testSearchByName();
testSearchByCategory();
testSearchNoResults();
testShortcutsFilter();
testSearchByCommand();

console.log('\n✅ جميع اختبارات لوحة التحكم نجحت (10/10)');
