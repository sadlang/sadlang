/**
 * @file ast_explorer.test.ts
 * @brief اختبارات وحدة لمستكشف شجرة AST — 40 اختبار
 *
 * يختبر بناء شجرة AST من كود لغة ص،
 * اكتشاف أنواع العقد، القواعد المتداخلة، والبحث.
 */

import * as assert from 'assert';

// ─── محاكاة بناء AST ───

interface ASTNode {
    type: string;
    name: string;
    line: number;
    children: ASTNode[];
}

/** أنماط اكتشاف العقد */
const NODE_PATTERNS: Array<{ regex: RegExp; type: string; nameGroup: number }> = [
    { regex: /^دالة\s+(\S+)\s*\(/, type: 'دالة', nameGroup: 1 },
    { regex: /^صنف\s+(\S+)/, type: 'صنف', nameGroup: 1 },
    { regex: /^بنية\s+(\S+)/, type: 'بنية', nameGroup: 1 },
    { regex: /^تعداد\s+(\S+)/, type: 'تعداد', nameGroup: 1 },
    { regex: /^سمة\s+(\S+)/, type: 'سمة', nameGroup: 1 },
    { regex: /^إذا\s*\(/, type: 'شرط', nameGroup: 0 },
    { regex: /^بينما\s*\(/, type: 'حلقة-بينما', nameGroup: 0 },
    { regex: /^لكل\s+/, type: 'حلقة-لكل', nameGroup: 0 },
    { regex: /^حاول\s*$/, type: 'حاول', nameGroup: 0 },
    { regex: /^باني\s*\(/, type: 'باني', nameGroup: 0 },
    { regex: /^متغير\s+(\S+)\s*=/, type: 'متغير', nameGroup: 1 },
    { regex: /^ثابت\s+(\S+)\s*=/, type: 'ثابت', nameGroup: 1 },
    { regex: /^طابق\s*\(/, type: 'طابق', nameGroup: 0 },
];

/** اكتشاف نوع عقدة من سطر */
function detectNode(line: string): { type: string; name: string } | null {
    const trimmed = line.trim();
    if (trimmed === '' || trimmed.startsWith('#')) return null;
    for (const { regex, type, nameGroup } of NODE_PATTERNS) {
        const match = trimmed.match(regex);
        if (match) {
            return { type, name: nameGroup > 0 ? (match[nameGroup] || type) : type };
        }
    }
    return null;
}

/** بناء شجرة AST مبسطة */
function buildAST(code: string): ASTNode[] {
    const lines = code.split('\n');
    const root: ASTNode[] = [];
    const stack: ASTNode[][] = [root];
    
    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();
        
        if (trimmed === 'نهاية') {
            if (stack.length > 1) stack.pop();
            continue;
        }
        
        const node = detectNode(trimmed);
        if (node) {
            const astNode: ASTNode = { ...node, line: i, children: [] };
            stack[stack.length - 1].push(astNode);
            
            // كتل تفتح مستوى جديد
            if (['دالة', 'صنف', 'بنية', 'شرط', 'حلقة-بينما', 'حلقة-لكل', 'حاول', 'باني', 'تعداد', 'سمة', 'طابق'].includes(node.type)) {
                stack.push(astNode.children);
            }
        }
    }
    
    return root;
}

/** بحث في الشجرة */
function searchAST(nodes: ASTNode[], query: string): ASTNode[] {
    const results: ASTNode[] = [];
    for (const node of nodes) {
        if (node.name.includes(query) || node.type.includes(query)) {
            results.push(node);
        }
        results.push(...searchAST(node.children, query));
    }
    return results;
}

/** عدّ العقد */
function countNodes(nodes: ASTNode[]): number {
    let count = nodes.length;
    for (const n of nodes) count += countNodes(n.children);
    return count;
}

// ═════════════════════════════════════════════
suite('مستكشف شجرة AST — AST Explorer', () => {

    // ─── مجموعة 1: اكتشاف أنواع العقد (10) ───
    test('1. اكتشاف دالة', () => {
        const n = detectNode('دالة جمع(أ، ب)');
        assert.ok(n);
        assert.strictEqual(n!.type, 'دالة');
        assert.strictEqual(n!.name, 'جمع');
    });
    test('2. اكتشاف صنف', () => {
        const n = detectNode('صنف حيوان');
        assert.ok(n);
        assert.strictEqual(n!.type, 'صنف');
    });
    test('3. اكتشاف شرط', () => {
        const n = detectNode('إذا (س > 0)');
        assert.ok(n);
        assert.strictEqual(n!.type, 'شرط');
    });
    test('4. اكتشاف حلقة بينما', () => {
        const n = detectNode('بينما (س > 0)');
        assert.ok(n);
        assert.strictEqual(n!.type, 'حلقة-بينما');
    });
    test('5. اكتشاف حلقة لكل', () => {
        const n = detectNode('لكل عنصر في المصفوفة');
        assert.ok(n);
        assert.strictEqual(n!.type, 'حلقة-لكل');
    });
    test('6. اكتشاف متغير', () => {
        const n = detectNode('متغير س = 10');
        assert.ok(n);
        assert.strictEqual(n!.type, 'متغير');
        assert.strictEqual(n!.name, 'س');
    });
    test('7. اكتشاف ثابت', () => {
        const n = detectNode('ثابت ط = 3.14');
        assert.ok(n);
        assert.strictEqual(n!.type, 'ثابت');
    });
    test('8. اكتشاف باني', () => {
        assert.strictEqual(detectNode('باني(الاسم)')?.type, 'باني');
    });
    test('9. سطر عادي — لا عقدة', () => {
        assert.strictEqual(detectNode('اطبع("مرحبا")'), null);
    });
    test('10. سطر تعليق — لا عقدة', () => {
        assert.strictEqual(detectNode('# تعليق'), null);
    });

    // ─── مجموعة 2: بناء شجرة (10) ───
    test('11. دالة واحدة', () => {
        const tree = buildAST('دالة مرحبا()\n    اطبع("مرحبا")\nنهاية');
        assert.strictEqual(tree.length, 1);
        assert.strictEqual(tree[0].type, 'دالة');
    });
    test('12. دالتان متتاليتان', () => {
        const tree = buildAST('دالة أ()\nنهاية\nدالة ب()\nنهاية');
        assert.strictEqual(tree.length, 2);
    });
    test('13. صنف مع دالة', () => {
        const tree = buildAST('صنف حيوان\n    دالة صوت()\n    نهاية\nنهاية');
        assert.strictEqual(tree.length, 1);
        assert.strictEqual(tree[0].children.length, 1);
    });
    test('14. كود فارغ', () => {
        assert.strictEqual(buildAST('').length, 0);
    });
    test('15. متغيرات فقط', () => {
        const tree = buildAST('متغير أ = 1\nمتغير ب = 2');
        assert.strictEqual(tree.length, 2);
    });
    test('16. شرط متداخل', () => {
        const tree = buildAST('إذا (صحيح)\n    إذا (خطأ)\n    نهاية\nنهاية');
        assert.strictEqual(tree.length, 1);
        assert.strictEqual(tree[0].children.length, 1);
    });
    test('17. حلقة مع متغير', () => {
        const tree = buildAST('بينما (صحيح)\n    متغير ع = 0\nنهاية');
        assert.strictEqual(tree[0].children.length, 1);
    });
    test('18. صنف مع باني', () => {
        const tree = buildAST('صنف شخص\n    باني(اسم)\n    نهاية\nنهاية');
        assert.strictEqual(tree[0].children[0].type, 'باني');
    });
    test('19. رقم السطر صحيح', () => {
        const tree = buildAST('\n\nدالة ثالثة()');
        assert.strictEqual(tree[0].line, 2);
    });
    test('20. عدد العقد الإجمالي', () => {
        const tree = buildAST('دالة أ()\n    متغير س = 1\nنهاية\nدالة ب()\nنهاية');
        assert.strictEqual(countNodes(tree), 3); // أ، س، ب
    });

    // ─── مجموعة 3: بحث في الشجرة (10) ───
    test('21. بحث بالاسم', () => {
        const tree = buildAST('دالة جمع()\nنهاية\nدالة طرح()\nنهاية');
        assert.strictEqual(searchAST(tree, 'جمع').length, 1);
    });
    test('22. بحث بالنوع', () => {
        const tree = buildAST('دالة أ()\nنهاية\nصنف ب\nنهاية');
        assert.strictEqual(searchAST(tree, 'صنف').length, 1);
    });
    test('23. بحث لا نتائج', () => {
        const tree = buildAST('دالة أ()\nنهاية');
        assert.strictEqual(searchAST(tree, 'غير_موجود').length, 0);
    });
    test('24. بحث في أطفال', () => {
        const tree = buildAST('صنف حيوان\n    دالة صوت()\n    نهاية\nنهاية');
        assert.strictEqual(searchAST(tree, 'صوت').length, 1);
    });
    test('25. بحث يعيد عدة نتائج', () => {
        const tree = buildAST('دالة أ()\nنهاية\nدالة ب()\nنهاية');
        assert.strictEqual(searchAST(tree, 'دالة').length, 2);
    });
    test('26. بحث في شجرة فارغة', () => {
        assert.strictEqual(searchAST([], 'أي').length, 0);
    });
    test('27. بحث جزئي', () => {
        const tree = buildAST('دالة حساب_المجموع()\nنهاية');
        assert.strictEqual(searchAST(tree, 'حساب').length, 1);
    });
    test('28. بحث متغير', () => {
        const tree = buildAST('متغير النتيجة = 0');
        assert.strictEqual(searchAST(tree, 'النتيجة').length, 1);
    });
    test('29. بحث شرط', () => {
        const tree = buildAST('إذا (صحيح)\nنهاية');
        assert.strictEqual(searchAST(tree, 'شرط').length, 1);
    });
    test('30. بحث عميق (3 مستويات)', () => {
        const tree = buildAST('صنف أ\n    دالة ب()\n        متغير ج = 0\n    نهاية\nنهاية');
        assert.strictEqual(searchAST(tree, 'ج').length, 1);
    });

    // ─── مجموعة 4: حالات حدودية (10) ───
    test('31. سطر فارغ فقط', () => {
        assert.strictEqual(buildAST('\n\n\n').length, 0);
    });
    test('32. تعليقات فقط', () => {
        assert.strictEqual(buildAST('# تعليق\n# آخر').length, 0);
    });
    test('33. نهاية بدون بداية', () => {
        // لا ينهار — يتجاهل النهاية الزائدة
        const tree = buildAST('نهاية\nنهاية');
        assert.strictEqual(tree.length, 0);
    });
    test('34. بنية', () => {
        const n = detectNode('بنية نقطة');
        assert.strictEqual(n?.type, 'بنية');
    });
    test('35. تعداد', () => {
        const n = detectNode('تعداد اللون');
        assert.strictEqual(n?.type, 'تعداد');
    });
    test('36. سمة', () => {
        const n = detectNode('سمة مقارن');
        assert.strictEqual(n?.type, 'سمة');
    });
    test('37. طابق', () => {
        const n = detectNode('طابق (قيمة)');
        assert.strictEqual(n?.type, 'طابق');
    });
    test('38. حاول', () => {
        const n = detectNode('حاول');
        assert.strictEqual(n?.type, 'حاول');
    });
    test('39. مسافات بادئة لا تؤثر على الاكتشاف', () => {
        const n = detectNode('        دالة داخلية()');
        assert.ok(n);
        assert.strictEqual(n!.type, 'دالة');
    });
    test('40. عقدة مع رقم سطر 0', () => {
        const tree = buildAST('متغير أول = 1');
        assert.strictEqual(tree[0].line, 0);
    });
});
