/**
 * @file snippet_generator.ts
 * @brief مولد القوالب — Generate snippets from selected code
 * 
 * يحول الكود المحدد إلى قالب (snippet) قابل لإعادة الاستخدام
 * مع دعم المتغيرات والنقاط القابلة للتعديل
 */

import * as vscode from 'vscode';

export function registerSnippetGenerator(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.generateSnippet', async () => {
            await generateSnippetFromSelection();
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.wrapWithSnippet', async () => {
            await wrapWithSnippet();
        })
    );
}

async function generateSnippetFromSelection(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const selection = editor.selection;
    if (selection.isEmpty) {
        vscode.window.showWarningMessage('حدد كوداً أولاً لتحويله إلى قالب');
        return;
    }

    const code = editor.document.getText(selection);
    const name = await vscode.window.showInputBox({
        prompt: 'اسم القالب — Snippet name',
        placeHolder: 'مثال: دالة_جديدة'
    });
    if (!name) { return; }

    const prefix = await vscode.window.showInputBox({
        prompt: 'الاختصار — Trigger prefix',
        placeHolder: 'مثال: دج',
        value: name.substring(0, 3)
    });
    if (!prefix) { return; }

    // تحويل الكود إلى صيغة snippet مع tabstops
    let snippetBody = code;
    let tabstopIndex = 1;

    // اكتشاف أسماء المتغيرات والمعاملات لتحويلها إلى tabstops
    const varNames = new Set<string>();
    const varRegex = /(?:متغير|ثابت)\s+(\S+)/g;
    let match;
    while ((match = varRegex.exec(code)) !== null) {
        varNames.add(match[1]);
    }

    // تحويل أسماء المتغيرات إلى tabstops
    for (const varName of varNames) {
        const regex = new RegExp(varName.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), 'g');
        snippetBody = snippetBody.replace(regex, `\${${tabstopIndex}:${varName}}`);
        tabstopIndex++;
    }

    // إضافة tabstop نهائي
    snippetBody += `\n\$0`;

    const snippetJSON = {
        [name]: {
            prefix: prefix,
            body: snippetBody.split('\n'),
            description: `قالب مخصص: ${name}`
        }
    };

    // عرض النتيجة
    const panel = vscode.window.createWebviewPanel(
        'sadSnippetGenerator', `قالب: ${name}`,
        vscode.ViewColumn.Beside, { enableScripts: true }
    );

    const jsonStr = JSON.stringify(snippetJSON, null, 4);

    panel.webview.html = `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
body { background: #1e1e1e; color: #d4d4d4; font-family: 'Cascadia Code', monospace; padding: 16px; direction: rtl; }
h2 { color: #569cd6; }
pre { background: #252526; padding: 16px; border-radius: 6px; border: 1px solid #333; overflow-x: auto; direction: ltr; text-align: left; }
.tip { color: #858585; font-size: 12px; margin-top: 12px; }
button { background: #569cd6; color: white; border: none; padding: 8px 16px; border-radius: 4px; cursor: pointer; margin-top: 12px; }
button:hover { background: #4e8cc2; }
.success { color: #6a9955; display: none; margin-top: 8px; }
</style>
</head>
<body>
<h2>📋 قالب مُولَّد: ${name}</h2>
<p>الاختصار: <code>${prefix}</code></p>
<pre>${jsonStr.replace(/</g, '&lt;')}</pre>
<button onclick="copyToClipboard()">📋 نسخ إلى الحافظة</button>
<p class="success" id="success">✅ تم النسخ!</p>
<p class="tip">💡 ألصق هذا في ملف <code>snippets/sad.json</code> لاستخدامه كاختصار</p>
<script>
function copyToClipboard() {
    navigator.clipboard.writeText(${JSON.stringify(jsonStr)});
    document.getElementById('success').style.display = 'block';
    setTimeout(() => document.getElementById('success').style.display = 'none', 2000);
}
</script>
</body></html>`;
}

async function wrapWithSnippet(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const wrappers = [
        { label: 'إذا...نهاية', value: 'if' },
        { label: 'بينما...نهاية', value: 'while' },
        { label: 'لكل...نهاية', value: 'foreach' },
        { label: 'حاول...امسك...نهاية', value: 'try' },
        { label: 'دالة...نهاية', value: 'func' },
    ];

    const choice = await vscode.window.showQuickPick(wrappers, {
        placeHolder: 'اختر الغلاف — Choose wrapper'
    });
    if (!choice) { return; }

    const selection = editor.selection;
    const selectedText = editor.document.getText(selection);
    const indent = '    ';

    let wrapped = '';
    switch (choice.value) {
        case 'if':
            wrapped = `إذا (\${1:شرط})\n${indent}${selectedText.split('\n').join('\n' + indent)}\nنهاية`;
            break;
        case 'while':
            wrapped = `بينما (\${1:شرط})\n${indent}${selectedText.split('\n').join('\n' + indent)}\nنهاية`;
            break;
        case 'foreach':
            wrapped = `لكل \${1:عنصر} في \${2:مجموعة}\n${indent}${selectedText.split('\n').join('\n' + indent)}\nنهاية`;
            break;
        case 'try':
            wrapped = `حاول\n${indent}${selectedText.split('\n').join('\n' + indent)}\nامسك (خطأ)\n${indent}اطبع_سطر(خطأ)\nنهاية`;
            break;
        case 'func':
            wrapped = `دالة \${1:اسم_الدالة}(\${2:معاملات})\n${indent}${selectedText.split('\n').join('\n' + indent)}\nنهاية`;
            break;
    }

    editor.insertSnippet(new vscode.SnippetString(wrapped), selection);
}
