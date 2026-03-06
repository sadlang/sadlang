/**
 * @file variable_inspector.ts
 * @brief مراقب المتغيرات — عرض قيم المتغيرات مباشرة أثناء التشغيل
 * 
 * يعرض قيم المتغيرات المكتشفة كنوافذ صغيرة (inline) بجانب التعريفات
 * ويوفر لوحة جانبية لمراقبة المتغيرات المحددة
 */

import * as vscode from 'vscode';

interface WatchedVariable {
    name: string;
    value: string;
    type: string;
    line: number;
}

let watchPanel: vscode.WebviewPanel | undefined;
let watchedVars: WatchedVariable[] = [];
let varDecorationType: vscode.TextEditorDecorationType;

export function registerVariableInspector(context: vscode.ExtensionContext): void {
    varDecorationType = vscode.window.createTextEditorDecorationType({
        after: {
            margin: '0 0 0 1.5em',
            fontStyle: 'italic',
            color: '#888',
        }
    });

    context.subscriptions.push(varDecorationType);

    // أمر: مراقبة المتغير المحدد
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.watchVariable', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) { return; }
            const selection = editor.document.getText(editor.selection);
            if (selection) {
                addWatchedVariable(selection, editor);
            } else {
                const word = editor.document.getText(
                    editor.document.getWordRangeAtPosition(editor.selection.active)
                );
                if (word) { addWatchedVariable(word, editor); }
            }
        })
    );

    // أمر: عرض لوحة المراقبة
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.showVariablePanel', () => {
            openWatchPanel(context);
        })
    );

    // أمر: مسح قائمة المراقبة
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.clearWatch', () => {
            watchedVars = [];
            updateDecorations();
            if (watchPanel) { refreshWatchPanel(); }
            vscode.window.showInformationMessage('تم مسح قائمة المراقبة');
        })
    );

    // تحديث الزخرفة عند التعديل
    context.subscriptions.push(
        vscode.workspace.onDidChangeTextDocument(() => detectVariables())
    );
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor(() => detectVariables())
    );

    detectVariables();
}

function addWatchedVariable(name: string, editor: vscode.TextEditor): void {
    // تحليل قيمة المتغير من الكود
    const doc = editor.document;
    const text = doc.getText();
    const varPattern = new RegExp(`(?:متغير|ثابت)\\s+${name}\\s*=\\s*(.+)`, 'gm');
    const match = varPattern.exec(text);

    const value = match ? match[1].trim() : '?';
    const type = detectType(value);

    // تجنب التكرار
    const existing = watchedVars.findIndex(v => v.name === name);
    if (existing >= 0) {
        watchedVars[existing].value = value;
        watchedVars[existing].type = type;
    } else {
        watchedVars.push({ name, value, type, line: match ? doc.positionAt(match.index).line : 0 });
    }

    updateDecorations();
    if (watchPanel) { refreshWatchPanel(); }
    vscode.window.showInformationMessage(`🔍 مراقبة: ${name} = ${value}`);
}

function detectType(value: string): string {
    if (value.startsWith('"') || value.startsWith("'")) return 'نص';
    if (value === 'صحيح' || value === 'خطأ') return 'منطقي';
    if (value === 'لاشيء') return 'عدم';
    if (/^\d+$/.test(value)) return 'رقم';
    if (/^\d+\.\d+$/.test(value)) return 'عشري';
    if (value.startsWith('[')) return 'مصفوفة';
    if (value.startsWith('{')) return 'خريطة';
    return 'أي';
}

function detectVariables(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }
    updateDecorations();
}

function updateDecorations(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const decorations: vscode.DecorationOptions[] = [];
    const text = editor.document.getText();

    for (const v of watchedVars) {
        const regex = new RegExp(`(?:متغير|ثابت)\\s+${v.name}\\s*=`, 'gm');
        const match = regex.exec(text);
        if (match) {
            const pos = editor.document.positionAt(match.index);
            const line = editor.document.lineAt(pos.line);
            decorations.push({
                range: line.range,
                renderOptions: {
                    after: {
                        contentText: ` 🔍 ${v.type}: ${v.value}`,
                        color: '#6a9955',
                        fontStyle: 'italic',
                    }
                }
            });
        }
    }

    editor.setDecorations(varDecorationType, decorations);
}

function openWatchPanel(context: vscode.ExtensionContext): void {
    if (watchPanel) {
        watchPanel.reveal(vscode.ViewColumn.Beside);
        refreshWatchPanel();
        return;
    }

    watchPanel = vscode.window.createWebviewPanel(
        'sadVariableInspector', 'مراقب المتغيرات — لغة ص',
        vscode.ViewColumn.Beside, { enableScripts: true }
    );

    watchPanel.onDidDispose(() => { watchPanel = undefined; });
    refreshWatchPanel();
}

function refreshWatchPanel(): void {
    if (!watchPanel) { return; }

    let tableRows = '';
    for (const v of watchedVars) {
        tableRows += `<tr>
            <td class="name">${v.name}</td>
            <td class="type">${v.type}</td>
            <td class="value">${v.value}</td>
        </tr>\n`;
    }

    watchPanel.webview.html = `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
body { background: #1e1e1e; color: #d4d4d4; font-family: 'Cascadia Code', monospace; padding: 16px; direction: rtl; }
h2 { color: #569cd6; border-bottom: 1px solid #333; padding-bottom: 8px; }
table { width: 100%; border-collapse: collapse; margin-top: 12px; }
th { background: #252526; color: #569cd6; padding: 8px; text-align: right; border-bottom: 2px solid #333; }
td { padding: 8px; border-bottom: 1px solid #333; }
.name { color: #9cdcfe; font-weight: bold; }
.type { color: #4ec9b0; }
.value { color: #ce9178; }
.empty { color: #858585; text-align: center; padding: 24px; }
</style>
</head>
<body>
<h2>🔍 مراقب المتغيرات</h2>
${watchedVars.length > 0 ? `<table>
<tr><th>الاسم</th><th>النوع</th><th>القيمة</th></tr>
${tableRows}
</table>` : '<p class="empty">لا توجد متغيرات مراقبة. حدد متغيراً واستخدم أمر "مراقبة متغير"</p>'}
</body></html>`;
}
