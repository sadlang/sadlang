/**
 * @file graphics_preview.test.ts
 * @brief اختبارات وحدة لمعاينة الرسوميات — 40 اختبار
 *
 * يختبر استخراج أوامر الرسم من كود لغة ص،
 * تحويلها إلى Canvas API، ومعالجة الأنماط المختلفة.
 */

import * as assert from 'assert';

// ─── محاكاة استخراج الأوامر ───

interface DrawCommand {
    type: string;
    args: string[];
}

const DRAW_PATTERNS: Array<{ regex: RegExp; type: string }> = [
    { regex: /ارسم_مستطيل\((.+)\)/, type: 'rect' },
    { regex: /ارسم_دائرة\((.+)\)/, type: 'circle' },
    { regex: /ارسم_خط\((.+)\)/, type: 'line' },
    { regex: /ارسم_نص\((.+)\)/, type: 'text' },
    { regex: /لون\((.+)\)/, type: 'color' },
    { regex: /لون_الخلفية\((.+)\)/, type: 'bgColor' },
    { regex: /عرض_الخط\((.+)\)/, type: 'lineWidth' },
    { regex: /امسح\(\)/, type: 'clear' },
    { regex: /ارسم_مثلث\((.+)\)/, type: 'triangle' },
    { regex: /ارسم_قوس\((.+)\)/, type: 'arc' },
];

/** استخراج أوامر الرسم من كود */
function extractDrawCommands(code: string): DrawCommand[] {
    const commands: DrawCommand[] = [];
    for (const line of code.split('\n')) {
        const trimmed = line.trim();
        if (trimmed.startsWith('#') || trimmed === '') continue;
        for (const { regex, type } of DRAW_PATTERNS) {
            const match = trimmed.match(regex);
            if (match) {
                const args = match[1] ? match[1].split(/[،,]/).map((a: string) => a.trim()) : [];
                commands.push({ type, args });
                break;
            }
        }
    }
    return commands;
}

/** تحويل أمر إلى كود Canvas */
function commandToCanvasJS(cmd: DrawCommand): string {
    switch (cmd.type) {
        case 'rect': {
            const [x, y, w, h] = cmd.args;
            return `ctx.fillRect(${x}, ${y}, ${w}, ${h});`;
        }
        case 'circle': {
            const [cx, cy, r] = cmd.args;
            return `ctx.beginPath(); ctx.arc(${cx}, ${cy}, ${r}, 0, Math.PI*2); ctx.fill();`;
        }
        case 'line': {
            const [x1, y1, x2, y2] = cmd.args;
            return `ctx.beginPath(); ctx.moveTo(${x1}, ${y1}); ctx.lineTo(${x2}, ${y2}); ctx.stroke();`;
        }
        case 'text': {
            const [txt, x, y] = cmd.args;
            return `ctx.fillText(${txt}, ${x}, ${y});`;
        }
        case 'color':
            return `ctx.fillStyle = ${cmd.args[0]}; ctx.strokeStyle = ${cmd.args[0]};`;
        case 'bgColor':
            return `document.body.style.backgroundColor = ${cmd.args[0]};`;
        case 'lineWidth':
            return `ctx.lineWidth = ${cmd.args[0]};`;
        case 'clear':
            return `ctx.clearRect(0, 0, canvas.width, canvas.height);`;
        case 'triangle': {
            const [x1, y1, x2, y2, x3, y3] = cmd.args;
            return `ctx.beginPath(); ctx.moveTo(${x1}, ${y1}); ctx.lineTo(${x2}, ${y2}); ctx.lineTo(${x3}, ${y3}); ctx.closePath(); ctx.fill();`;
        }
        case 'arc': {
            const [cx, cy, r, start, end] = cmd.args;
            return `ctx.beginPath(); ctx.arc(${cx}, ${cy}, ${r}, ${start}, ${end}); ctx.stroke();`;
        }
        default:
            return '';
    }
}

/** تحويل عدة أوامر */
function commandsToCanvasJS(cmds: DrawCommand[]): string {
    return cmds.map(commandToCanvasJS).join('\n');
}

/** هل الكود يحتوي أوامر رسم */
function hasGraphics(code: string): boolean {
    return extractDrawCommands(code).length > 0;
}

// ═════════════════════════════════════════════
suite('معاينة الرسوميات — Graphics Preview', () => {

    // ─── مجموعة 1: استخراج الأوامر (10) ───
    test('1. استخراج مستطيل', () => {
        const cmds = extractDrawCommands('ارسم_مستطيل(10، 20، 100، 50)');
        assert.strictEqual(cmds.length, 1);
        assert.strictEqual(cmds[0].type, 'rect');
    });
    test('2. استخراج دائرة', () => {
        const cmds = extractDrawCommands('ارسم_دائرة(50، 50، 30)');
        assert.strictEqual(cmds[0].type, 'circle');
        assert.strictEqual(cmds[0].args.length, 3);
    });
    test('3. استخراج خط', () => {
        const cmds = extractDrawCommands('ارسم_خط(0، 0، 100، 100)');
        assert.strictEqual(cmds[0].type, 'line');
    });
    test('4. استخراج نص', () => {
        const cmds = extractDrawCommands('ارسم_نص("مرحبا"، 10، 20)');
        assert.strictEqual(cmds[0].type, 'text');
    });
    test('5. استخراج لون', () => {
        const cmds = extractDrawCommands('لون("أحمر")');
        assert.strictEqual(cmds[0].type, 'color');
    });
    test('6. استخراج عرض خط', () => {
        const cmds = extractDrawCommands('عرض_الخط(3)');
        assert.strictEqual(cmds[0].type, 'lineWidth');
    });
    test('7. استخراج مسح', () => {
        const cmds = extractDrawCommands('امسح()');
        assert.strictEqual(cmds[0].type, 'clear');
    });
    test('8. عدة أوامر', () => {
        const code = 'لون("أحمر")\nارسم_مستطيل(0، 0، 50، 50)\nارسم_دائرة(25، 25، 10)';
        assert.strictEqual(extractDrawCommands(code).length, 3);
    });
    test('9. تجاهل الأسطر العادية', () => {
        const code = 'متغير س = 10\nاطبع(س)';
        assert.strictEqual(extractDrawCommands(code).length, 0);
    });
    test('10. تجاهل التعليقات', () => {
        const code = '# ارسم_مستطيل(0، 0، 10، 10)';
        assert.strictEqual(extractDrawCommands(code).length, 0);
    });

    // ─── مجموعة 2: تحويل لـ Canvas (10) ───
    test('11. مستطيل → fillRect', () => {
        const js = commandToCanvasJS({ type: 'rect', args: ['10', '20', '30', '40'] });
        assert.ok(js.includes('fillRect'));
    });
    test('12. دائرة → arc', () => {
        const js = commandToCanvasJS({ type: 'circle', args: ['50', '50', '25'] });
        assert.ok(js.includes('arc(50, 50, 25'));
    });
    test('13. خط → moveTo + lineTo', () => {
        const js = commandToCanvasJS({ type: 'line', args: ['0', '0', '100', '100'] });
        assert.ok(js.includes('moveTo'));
        assert.ok(js.includes('lineTo'));
    });
    test('14. نص → fillText', () => {
        const js = commandToCanvasJS({ type: 'text', args: ['"سلام"', '10', '20'] });
        assert.ok(js.includes('fillText'));
    });
    test('15. لون → fillStyle', () => {
        const js = commandToCanvasJS({ type: 'color', args: ['"blue"'] });
        assert.ok(js.includes('fillStyle'));
    });
    test('16. مسح → clearRect', () => {
        const js = commandToCanvasJS({ type: 'clear', args: [] });
        assert.ok(js.includes('clearRect'));
    });
    test('17. عرض خط → lineWidth', () => {
        const js = commandToCanvasJS({ type: 'lineWidth', args: ['5'] });
        assert.ok(js.includes('ctx.lineWidth = 5'));
    });
    test('18. خلفية → backgroundColor', () => {
        const js = commandToCanvasJS({ type: 'bgColor', args: ['"white"'] });
        assert.ok(js.includes('backgroundColor'));
    });
    test('19. مثلث → 3 نقاط', () => {
        const js = commandToCanvasJS({ type: 'triangle', args: ['0', '0', '50', '0', '25', '50'] });
        assert.ok(js.includes('moveTo'));
        assert.ok(js.includes('closePath'));
    });
    test('20. نوع غير معروف → فارغ', () => {
        assert.strictEqual(commandToCanvasJS({ type: 'unknown', args: [] }), '');
    });

    // ─── مجموعة 3: الوسائط (10) ───
    test('21. فاصلة عربية', () => {
        const cmds = extractDrawCommands('ارسم_مستطيل(10، 20، 30، 40)');
        assert.strictEqual(cmds[0].args.length, 4);
    });
    test('22. فاصلة لاتينية', () => {
        const cmds = extractDrawCommands('ارسم_مستطيل(10, 20, 30, 40)');
        assert.strictEqual(cmds[0].args.length, 4);
    });
    test('23. وسيطة واحدة', () => {
        const cmds = extractDrawCommands('عرض_الخط(5)');
        assert.strictEqual(cmds[0].args.length, 1);
        assert.strictEqual(cmds[0].args[0], '5');
    });
    test('24. بدون وسائط', () => {
        const cmds = extractDrawCommands('امسح()');
        assert.strictEqual(cmds[0].args.length, 0);
    });
    test('25. مسافات حول الوسائط', () => {
        const cmds = extractDrawCommands('ارسم_دائرة( 50 ، 50 ، 30 )');
        assert.strictEqual(cmds[0].args.length, 3);
        assert.strictEqual(cmds[0].args[0], '50');
    });
    test('26. قيم سالبة', () => {
        const cmds = extractDrawCommands('ارسم_مستطيل(-10، -20، 30، 40)');
        assert.strictEqual(cmds[0].args[0], '-10');
    });
    test('27. قيم عشرية', () => {
        const cmds = extractDrawCommands('ارسم_دائرة(12.5، 30.7، 15.2)');
        assert.strictEqual(cmds[0].args[0], '12.5');
    });
    test('28. نص كوسيطة', () => {
        const cmds = extractDrawCommands('ارسم_نص("عنوان"، 100، 50)');
        assert.strictEqual(cmds[0].args[0], '"عنوان"');
    });
    test('29. قوس', () => {
        const cmds = extractDrawCommands('ارسم_قوس(100، 100، 50، 0، 3.14)');
        assert.strictEqual(cmds[0].type, 'arc');
        assert.strictEqual(cmds[0].args.length, 5);
    });
    test('30. مثلث', () => {
        const cmds = extractDrawCommands('ارسم_مثلث(0، 0، 50، 0، 25، 50)');
        assert.strictEqual(cmds[0].type, 'triangle');
        assert.strictEqual(cmds[0].args.length, 6);
    });

    // ─── مجموعة 4: حالات مختلفة (10) ───
    test('31. hasGraphics — نعم', () => {
        assert.strictEqual(hasGraphics('ارسم_مستطيل(0، 0، 10، 10)'), true);
    });
    test('32. hasGraphics — لا', () => {
        assert.strictEqual(hasGraphics('اطبع("مرحبا")'), false);
    });
    test('33. commandsToCanvasJS — عدة أوامر', () => {
        const cmds: DrawCommand[] = [
            { type: 'color', args: ['"red"'] },
            { type: 'rect', args: ['0', '0', '50', '50'] },
        ];
        const js = commandsToCanvasJS(cmds);
        assert.ok(js.includes('fillStyle'));
        assert.ok(js.includes('fillRect'));
    });
    test('34. commandsToCanvasJS — فارغ', () => {
        assert.strictEqual(commandsToCanvasJS([]), '');
    });
    test('35. سطر فارغ يُتجاهل', () => {
        assert.strictEqual(extractDrawCommands('\n\n\n').length, 0);
    });
    test('36. لون الخلفية', () => {
        const cmds = extractDrawCommands('لون_الخلفية("أبيض")');
        assert.strictEqual(cmds[0].type, 'bgColor');
    });
    test('37. ترتيب الأوامر محفوظ', () => {
        const code = 'لون("أحمر")\nارسم_مستطيل(0، 0، 50، 50)';
        const cmds = extractDrawCommands(code);
        assert.strictEqual(cmds[0].type, 'color');
        assert.strictEqual(cmds[1].type, 'rect');
    });
    test('38. كود مختلط — رسوميات وعادي', () => {
        const code = 'متغير س = 10\nارسم_دائرة(س، س، 5)\nاطبع(س)';
        assert.strictEqual(extractDrawCommands(code).length, 1);
    });
    test('39. عدة مستطيلات', () => {
        const code = 'ارسم_مستطيل(0، 0، 10، 10)\nارسم_مستطيل(20، 20، 10، 10)';
        assert.strictEqual(extractDrawCommands(code).length, 2);
    });
    test('40. قوس → stroke', () => {
        const js = commandToCanvasJS({ type: 'arc', args: ['50', '50', '30', '0', '6.28'] });
        assert.ok(js.includes('arc'));
        assert.ok(js.includes('stroke'));
    });
});
