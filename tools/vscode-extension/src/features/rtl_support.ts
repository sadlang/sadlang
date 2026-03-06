/**
 * @file rtl_support.ts
 * @brief دعم الكتابة من اليمين لليسار (RTL) — RTL Support for Arabic Code
 *
 * الطريقة المُتبّعة:
 *   1. لوحة Webview جانبية تعرض الكود بتنسيق RTL حقيقي مع تلوين نحوي
 *   2. حقن CSS عبر TextEditorDecorationType (يعمل جزئياً في Monaco)
 *   3. شريط حالة لتبديل RTL/LTR وفتح لوحة المعاينة
 *   4. تزامن آني: أي تغيير في المحرر يُحدَّث فوراً في لوحة RTL
 *   5. نقر على سطر في لوحة RTL يقفز إلى نفس السطر في المحرر
 *
 * This file provides RTL support via:
 *   1. A Webview side panel that renders code with real CSS direction:rtl + syntax coloring
 *   2. CSS decoration injection in the editor (partial support in Monaco)
 *   3. Status bar toggle for RTL/LTR and opening the RTL panel
 *   4. Live sync: any editor change instantly updates the RTL panel
 *   5. Click-to-navigate: clicking a line in the RTL panel jumps to that line
 */

import * as vscode from 'vscode';

// ─── متغيرات عامة ───
let rtlEnabled = true;
let statusBarItem: vscode.StatusBarItem;
let rtlPanel: vscode.WebviewPanel | undefined;
let rtlDecorationType: vscode.TextEditorDecorationType;

// ─── الكلمات المفتاحية للتلوين النحوي ───
const KEYWORDS = [
    'دالة', 'ارجع', 'صنف', 'بنية', 'تعداد', 'يرث', 'نهاية', 'جديد', 'هذا', 'باني', 'الأساس',
    'إذا', 'اذا', 'وإلا', 'والا', 'بينما', 'لكل', 'في', 'توقف', 'استمر',
    'طابق', 'عندما', 'افتراضي',
    'حاول', 'امسك', 'ارمي', 'أخيراً',
    'عام', 'خاص', 'محمي', 'مجرد',
    'استورد', 'من', 'كـ', 'صدّر',
    'متغير', 'ثابت', 'ساكن', 'خارجي',
    'صحيح', 'خطأ', 'لاشيء'
];

const CONTEXTUAL_KEYWORDS = [
    'غير_متزامن', 'انتظر', 'لامدا', 'أنتج', 'مولد', 'باستخدام', 'نهاية_استخدام',
    'سمة', 'واجهة', 'نفّذ', 'نفذ', 'قالب', 'فضاء', 'نهاية_فضاء',
    'اختبر', 'خاصية', 'احصل', 'عيّن', 'هدم', 'عامل', 'رئيسية', 'حالة'
];

const BUILTIN_TYPES = [
    'رقم', 'عشري', 'نص', 'منطقي', 'فراغ', 'عدم', 'مصفوفة', 'خريطة', 'أي'
];

const BUILTIN_FUNCTIONS = [
    'اطبع', 'اطبع_سطر', 'طول', 'نوع', 'ادخل', 'حول_لرقم', 'حول_لنص'
];

/**
 * @brief تسجيل ميزة RTL — Register RTL feature
 */
export function registerRTLSupport(context: vscode.ExtensionContext): void {
    const config = vscode.workspace.getConfiguration('sad');
    rtlEnabled = config.get<boolean>('rtl.enabled', true);

    // ─── إنشاء زخرفة CSS للمحرر ───
    rtlDecorationType = vscode.window.createTextEditorDecorationType({
        isWholeLine: true,
        textDecoration: 'none; direction: rtl; unicode-bidi: plaintext;'
    });

    // ─── أمر تبديل RTL/LTR ───
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.toggleRTL', () => {
            rtlEnabled = !rtlEnabled;
            context.globalState.update('sad.rtl.enabled', rtlEnabled);
            updateStatusBar();
            applyEditorDecorations();
            if (rtlPanel) {
                refreshRTLPanel();
            }
            const msg = rtlEnabled
                ? '✅ تم تفعيل اتجاه اليمين لليسار (RTL)'
                : '↩️ تم تبديل الاتجاه إلى اليسار لليمين (LTR)';
            vscode.window.showInformationMessage(msg);
        })
    );

    // ─── أمر فرض RTL ───
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.forceRTL', () => {
            rtlEnabled = true;
            context.globalState.update('sad.rtl.enabled', true);
            updateStatusBar();
            applyEditorDecorations();
            if (rtlPanel) { refreshRTLPanel(); }
            vscode.window.showInformationMessage('✅ تم فرض RTL على جميع الأسطر');
        })
    );

    // ─── أمر فرض LTR ───
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.forceLTR', () => {
            rtlEnabled = false;
            context.globalState.update('sad.rtl.enabled', false);
            updateStatusBar();
            applyEditorDecorations();
            if (rtlPanel) { refreshRTLPanel(); }
            vscode.window.showInformationMessage('↩️ تم فرض LTR على جميع الأسطر');
        })
    );

    // ─── أمر فتح لوحة RTL ───
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.openRTLPanel', () => {
            openRTLPanel(context);
        })
    );

    // ─── شريط الحالة ───
    statusBarItem = vscode.window.createStatusBarItem(
        vscode.StatusBarAlignment.Right, 100
    );
    statusBarItem.command = 'sad.openRTLPanel';
    statusBarItem.tooltip = 'فتح لوحة RTL / تبديل الاتجاه — Open RTL Panel / Toggle Direction';
    context.subscriptions.push(statusBarItem);
    updateStatusBar();

    // ─── مراقبة الأحداث ───
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor((editor) => {
            if (editor && isSadFile(editor.document)) {
                statusBarItem.show();
                applyEditorDecorations();
                if (rtlPanel) { refreshRTLPanel(); }
            } else {
                statusBarItem.hide();
            }
        })
    );

    context.subscriptions.push(
        vscode.workspace.onDidChangeTextDocument((event) => {
            const editor = vscode.window.activeTextEditor;
            if (editor && event.document === editor.document && isSadFile(event.document)) {
                applyEditorDecorations();
                if (rtlPanel) { refreshRTLPanel(); }
            }
        })
    );

    context.subscriptions.push(
        vscode.workspace.onDidChangeConfiguration((event) => {
            if (event.affectsConfiguration('sad.rtl.enabled')) {
                const config = vscode.workspace.getConfiguration('sad');
                rtlEnabled = config.get<boolean>('rtl.enabled', true);
                updateStatusBar();
                applyEditorDecorations();
                if (rtlPanel) { refreshRTLPanel(); }
            }
        })
    );

    // تطبيق على المحرر الحالي فوراً
    const activeEditor = vscode.window.activeTextEditor;
    if (activeEditor && isSadFile(activeEditor.document)) {
        statusBarItem.show();
        applyEditorDecorations();
    }

    context.subscriptions.push(rtlDecorationType);
}

// ─── لوحة RTL Webview ───

function openRTLPanel(context: vscode.ExtensionContext): void {
    if (rtlPanel) {
        rtlPanel.reveal(vscode.ViewColumn.Beside);
        refreshRTLPanel();
        return;
    }

    rtlPanel = vscode.window.createWebviewPanel(
        'sadRTLView',
        'عرض RTL — لغة ص',
        vscode.ViewColumn.Beside,
        {
            enableScripts: true,
            retainContextWhenHidden: true
        }
    );

    rtlPanel.onDidDispose(() => {
        rtlPanel = undefined;
    });

    // معالجة الرسائل من Webview (نقر على سطر)
    rtlPanel.webview.onDidReceiveMessage((message) => {
        if (message.type === 'gotoLine') {
            const editor = vscode.window.activeTextEditor;
            if (editor && isSadFile(editor.document)) {
                const line = message.line;
                const pos = new vscode.Position(line, 0);
                editor.selection = new vscode.Selection(pos, pos);
                editor.revealRange(new vscode.Range(pos, pos), vscode.TextEditorRevealType.InCenter);
            }
        }
    });

    refreshRTLPanel();
}

function refreshRTLPanel(): void {
    if (!rtlPanel) { return; }

    const editor = vscode.window.activeTextEditor;
    if (!editor || !isSadFile(editor.document)) {
        rtlPanel.webview.html = getEmptyHTML();
        return;
    }

    const document = editor.document;
    const code = document.getText();
    const fileName = document.fileName.split(/[/\\]/).pop() || 'ملف.ص';
    const direction = rtlEnabled ? 'rtl' : 'ltr';
    const textAlign = rtlEnabled ? 'right' : 'left';

    rtlPanel.webview.html = getRTLViewHTML(code, fileName, direction, textAlign);
}

// ─── تلوين نحوي ───

function escapeHtml(text: string): string {
    return text
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;');
}

function highlightLine(line: string): string {
    // ترتيب التلوين: تعليقات → نصوص → أرقام → كلمات مفتاحية → دوال → أنواع → سياقية
    let result = escapeHtml(line);

    // تعليقات السطر  
    result = result.replace(/(#[^*].*$|#$)/gm, '<span class="comment">$1</span>');

    // نصوص (بين علامات تنصيص)
    result = result.replace(/(&quot;[^&]*&quot;|"[^"]*")/g, '<span class="string">$1</span>');

    // أرقام
    result = result.replace(/\b(\d+\.?\d*)\b/g, '<span class="number">$1</span>');

    // كلمات مفتاحية
    for (const kw of KEYWORDS) {
        const regex = new RegExp(`\\b(${kw})\\b`, 'g');
        result = result.replace(regex, '<span class="keyword">$1</span>');
    }

    // دوال مدمجة
    for (const fn of BUILTIN_FUNCTIONS) {
        const regex = new RegExp(`\\b(${fn})\\b`, 'g');
        result = result.replace(regex, '<span class="builtin">$1</span>');
    }

    // أنواع مدمجة
    for (const t of BUILTIN_TYPES) {
        const regex = new RegExp(`\\b(${t})\\b`, 'g');
        result = result.replace(regex, '<span class="type">$1</span>');
    }

    // كلمات سياقية
    for (const ck of CONTEXTUAL_KEYWORDS) {
        const regex = new RegExp(`\\b(${ck})\\b`, 'g');
        result = result.replace(regex, '<span class="contextual">$1</span>');
    }

    // عوامل
    result = result.replace(/(&&|\|\||[+\-*/%]=?|[=!<>]=?|\.\.)/g, '<span class="operator">$1</span>');

    return result;
}

// ─── بناء HTML للوحة RTL ───

function getRTLViewHTML(code: string, fileName: string, direction: string, textAlign: string): string {
    const lines = code.split('\n');
    const lineCount = lines.length;
    const digitWidth = String(lineCount).length;

    let linesHTML = '';
    for (let i = 0; i < lines.length; i++) {
        const lineNum = String(i + 1).padStart(digitWidth, ' ');
        const highlighted = highlightLine(lines[i]);
        const emptyClass = lines[i].trim() === '' ? ' empty' : '';
        linesHTML += `<div class="line${emptyClass}" data-line="${i}" onclick="gotoLine(${i})">`;
        linesHTML += `<span class="line-number">${lineNum}</span>`;
        linesHTML += `<span class="line-content">${highlighted || '&nbsp;'}</span>`;
        linesHTML += `</div>\n`;
    }

    return `<!DOCTYPE html>
<html lang="ar" dir="${direction}">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
    :root {
        --bg: #1e1e1e;
        --fg: #d4d4d4;
        --line-bg: transparent;
        --line-hover: #2a2d2e;
        --line-num: #858585;
        --gutter-bg: #1e1e1e;
        --keyword: #569cd6;
        --string: #ce9178;
        --comment: #6a9955;
        --number: #b5cea8;
        --operator: #d4d4d4;
        --type: #4ec9b0;
        --builtin: #dcdcaa;
        --contextual: #c586c0;
        --border: #333;
        --header-bg: #252526;
        --active-line: #264f78;
    }

    @media (prefers-color-scheme: light) {
        :root {
            --bg: #ffffff;
            --fg: #1e1e1e;
            --line-hover: #f3f3f3;
            --line-num: #999;
            --gutter-bg: #f8f8f8;
            --keyword: #0000ff;
            --string: #a31515;
            --comment: #008000;
            --number: #098658;
            --type: #267f99;
            --builtin: #795e26;
            --contextual: #af00db;
            --border: #e0e0e0;
            --header-bg: #f3f3f3;
            --active-line: #add6ff;
        }
    }

    * { margin: 0; padding: 0; box-sizing: border-box; }

    body {
        background: var(--bg);
        color: var(--fg);
        font-family: 'Cascadia Code', 'Fira Code', 'Consolas', 'Courier New', 'Noto Sans Arabic', monospace;
        font-size: 14px;
        line-height: 1.6;
        direction: ${direction};
        overflow-x: auto;
    }

    .header {
        background: var(--header-bg);
        border-bottom: 1px solid var(--border);
        padding: 8px 16px;
        display: flex;
        justify-content: space-between;
        align-items: center;
        position: sticky;
        top: 0;
        z-index: 10;
        direction: rtl;
    }

    .header .filename {
        font-weight: bold;
        color: var(--keyword);
        font-size: 13px;
    }

    .header .info {
        color: var(--line-num);
        font-size: 12px;
    }

    .header .direction-badge {
        background: var(--keyword);
        color: var(--bg);
        padding: 2px 8px;
        border-radius: 4px;
        font-size: 11px;
        font-weight: bold;
    }

    .code-container {
        padding: 8px 0;
    }

    .line {
        display: flex;
        direction: ${direction};
        text-align: ${textAlign};
        padding: 0 16px;
        cursor: pointer;
        border-${direction === 'rtl' ? 'right' : 'left'}: 3px solid transparent;
        transition: background 0.1s;
        min-height: 22px;
    }

    .line:hover {
        background: var(--line-hover);
        border-${direction === 'rtl' ? 'right' : 'left'}-color: var(--keyword);
    }

    .line-number {
        color: var(--line-num);
        min-width: ${digitWidth + 1}ch;
        text-align: center;
        user-select: none;
        padding: 0 12px 0 8px;
        border-${direction === 'rtl' ? 'left' : 'right'}: 1px solid var(--border);
        margin-${direction === 'rtl' ? 'left' : 'right'}: 12px;
        flex-shrink: 0;
        font-size: 13px;
    }

    .line-content {
        flex: 1;
        white-space: pre;
        direction: ${direction};
        text-align: ${textAlign};
        unicode-bidi: plaintext;
        tab-size: 4;
    }

    .line.empty .line-content {
        min-height: 22px;
    }

    /* تلوين نحوي */
    .keyword    { color: var(--keyword); font-weight: bold; }
    .string     { color: var(--string); }
    .comment    { color: var(--comment); font-style: italic; }
    .number     { color: var(--number); }
    .operator   { color: var(--operator); }
    .type       { color: var(--type); }
    .builtin    { color: var(--builtin); }
    .contextual { color: var(--contextual); }

    /* تمرير أفقي */
    .line-content::-webkit-scrollbar { height: 4px; }
    .line-content::-webkit-scrollbar-thumb { background: var(--border); border-radius: 2px; }

    /* Footer */
    .footer {
        border-top: 1px solid var(--border);
        padding: 6px 16px;
        color: var(--line-num);
        font-size: 11px;
        text-align: center;
        direction: rtl;
    }
</style>
</head>
<body>
    <div class="header">
        <span class="filename">📄 ${escapeHtml(fileName)}</span>
        <span class="direction-badge">${direction === 'rtl' ? '← RTL' : 'LTR →'}</span>
        <span class="info">${lineCount} سطر</span>
    </div>
    <div class="code-container">
${linesHTML}
    </div>
    <div class="footer">
        لوحة عرض RTL — لغة ص | انقر على سطر للانتقال إليه في المحرر | Ctrl+Shift+; لتبديل الاتجاه
    </div>
    <script>
        const vscode = acquireVsCodeApi();
        function gotoLine(line) {
            vscode.postMessage({ type: 'gotoLine', line: line });
            // تمييز السطر المنقور
            document.querySelectorAll('.line').forEach(el => el.style.background = '');
            const el = document.querySelector('[data-line="' + line + '"]');
            if (el) el.style.background = 'var(--active-line)';
        }
    </script>
</body>
</html>`;
}

function getEmptyHTML(): string {
    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
body { background: #1e1e1e; color: #858585; font-family: 'Noto Sans Arabic', sans-serif;
       display: flex; align-items: center; justify-content: center; height: 100vh; direction: rtl; }
.msg { text-align: center; }
h2 { color: #569cd6; margin-bottom: 8px; }
</style>
</head>
<body>
<div class="msg">
    <h2>لوحة RTL — لغة ص</h2>
    <p>افتح ملف <code>.ص</code> لعرض الكود من اليمين لليسار</p>
</div>
</body></html>`;
}

// ─── تطبيق الزخرفة على المحرر ───

function applyEditorDecorations(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor || !isSadFile(editor.document)) { return; }

    if (!rtlEnabled) {
        editor.setDecorations(rtlDecorationType, []);
        return;
    }

    const ranges: vscode.DecorationOptions[] = [];
    for (let i = 0; i < editor.document.lineCount; i++) {
        ranges.push({ range: editor.document.lineAt(i).range });
    }
    editor.setDecorations(rtlDecorationType, ranges);
}

// ─── مساعدات ───

function isSadFile(doc: vscode.TextDocument): boolean {
    return doc.languageId === 'sad' || doc.fileName.endsWith('.ص');
}

function updateStatusBar(): void {
    if (rtlEnabled) {
        statusBarItem.text = '$(arrow-right) RTL ←';
        statusBarItem.backgroundColor = undefined;
    } else {
        statusBarItem.text = '$(arrow-left) → LTR';
        statusBarItem.backgroundColor = new vscode.ThemeColor(
            'statusBarItem.warningBackground'
        );
    }
}
