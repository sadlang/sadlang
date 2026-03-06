/**
 * @file call_hierarchy.ts
 * @brief شجرة استدعاء الدوال — Visual Call Hierarchy
 * 
 * يعرض شجرة بصرية لاستدعاءات الدوال في المشروع
 * مع إمكانية التنقل بين الدوال المستدعاة والمستدعية
 */

import * as vscode from 'vscode';

interface FunctionInfo {
    name: string;
    line: number;
    file: string;
    calls: string[];
    calledBy: string[];
}

let hierarchyPanel: vscode.WebviewPanel | undefined;

export function registerCallHierarchy(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.showCallHierarchy', () => {
            showCallHierarchy(context);
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.showIncomingCalls', () => {
            showCallHierarchy(context, 'incoming');
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.showOutgoingCalls', () => {
            showCallHierarchy(context, 'outgoing');
        })
    );
}

function showCallHierarchy(context: vscode.ExtensionContext, direction: 'incoming' | 'outgoing' | 'both' = 'both'): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const functions = analyzeFunctions(editor.document);

    // تحديد الدالة الحالية
    const currentLine = editor.selection.active.line;
    const currentFunc = functions.find(f => f.line <= currentLine &&
        (functions.find(next => next.line > f.line)?.line ?? Infinity) > currentLine
    );

    if (hierarchyPanel) {
        hierarchyPanel.reveal(vscode.ViewColumn.Beside);
    } else {
        hierarchyPanel = vscode.window.createWebviewPanel(
            'sadCallHierarchy', 'شجرة الاستدعاء — لغة ص',
            vscode.ViewColumn.Beside, { enableScripts: true }
        );
        hierarchyPanel.onDidDispose(() => { hierarchyPanel = undefined; });

        hierarchyPanel.webview.onDidReceiveMessage((msg) => {
            if (msg.type === 'gotoFunction') {
                const func = functions.find(f => f.name === msg.name);
                if (func) {
                    const pos = new vscode.Position(func.line, 0);
                    editor.selection = new vscode.Selection(pos, pos);
                    editor.revealRange(new vscode.Range(pos, pos));
                }
            }
        });
    }

    hierarchyPanel.webview.html = buildHierarchyHTML(functions, currentFunc?.name || '', direction);
}

function analyzeFunctions(doc: vscode.TextDocument): FunctionInfo[] {
    const text = doc.getText();
    const lines = text.split('\n');
    const functions: FunctionInfo[] = [];

    // اكتشاف تعريفات الدوال
    const funcRegex = /^(?:\s*)دالة\s+(\S+)\s*\(/;
    for (let i = 0; i < lines.length; i++) {
        const match = lines[i].match(funcRegex);
        if (match) {
            functions.push({
                name: match[1],
                line: i,
                file: doc.fileName,
                calls: [],
                calledBy: []
            });
        }
    }

    // اكتشاف الاستدعاءات
    const funcNames = functions.map(f => f.name);
    for (const func of functions) {
        // ابحث في جسم الدالة
        const startLine = func.line + 1;
        const endFunc = functions.find(f => f.line > func.line);
        const endLine = endFunc ? endFunc.line : lines.length;

        for (let i = startLine; i < endLine; i++) {
            for (const name of funcNames) {
                if (name !== func.name && lines[i].includes(name + '(')) {
                    if (!func.calls.includes(name)) {
                        func.calls.push(name);
                    }
                    const callee = functions.find(f => f.name === name);
                    if (callee && !callee.calledBy.includes(func.name)) {
                        callee.calledBy.push(func.name);
                    }
                }
            }
        }
    }

    return functions;
}

function buildHierarchyHTML(functions: FunctionInfo[], current: string, direction: string): string {
    let nodesHTML = '';
    for (const f of functions) {
        const isCurrent = f.name === current;
        const calls = direction !== 'incoming' ? f.calls.map(c => `<span class="call-out" onclick="goto('${c}')">← ${c}</span>`).join('') : '';
        const calledBy = direction !== 'outgoing' ? f.calledBy.map(c => `<span class="call-in" onclick="goto('${c}')">→ ${c}</span>`).join('') : '';

        nodesHTML += `<div class="func ${isCurrent ? 'current' : ''}" onclick="goto('${f.name}')">
            <div class="func-name">🔹 ${f.name}()</div>
            <div class="func-line">سطر ${f.line + 1}</div>
            ${calledBy ? `<div class="section"><span class="label">يُستدعى من:</span> ${calledBy}</div>` : ''}
            ${calls ? `<div class="section"><span class="label">يستدعي:</span> ${calls}</div>` : ''}
        </div>\n`;
    }

    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
body { background: #1e1e1e; color: #d4d4d4; font-family: 'Cascadia Code', monospace; padding: 16px; direction: rtl; }
h2 { color: #569cd6; border-bottom: 1px solid #333; padding-bottom: 8px; }
.func { background: #252526; border: 1px solid #333; border-radius: 6px; padding: 12px; margin: 8px 0; cursor: pointer; transition: all 0.2s; }
.func:hover { border-color: #569cd6; background: #2d2d30; }
.func.current { border-color: #569cd6; border-width: 2px; background: #1e3a5f; }
.func-name { color: #dcdcaa; font-size: 15px; font-weight: bold; }
.func-line { color: #858585; font-size: 12px; margin-top: 4px; }
.section { margin-top: 8px; }
.label { color: #858585; font-size: 12px; }
.call-out, .call-in { display: inline-block; background: #333; padding: 2px 8px; border-radius: 3px; margin: 2px; font-size: 12px; cursor: pointer; }
.call-out { color: #4ec9b0; }
.call-out:hover { background: #264f78; }
.call-in { color: #c586c0; }
.call-in:hover { background: #3a1e4f; }
.stats { color: #858585; font-size: 12px; margin-bottom: 12px; }
</style>
</head>
<body>
<h2>📞 شجرة استدعاء الدوال</h2>
<p class="stats">${functions.length} دالة مكتشفة ${current ? `— الدالة الحالية: ${current}` : ''}</p>
${nodesHTML || '<p style="color:#858585">لا توجد دوال مكتشفة</p>'}
<script>
const vscode = acquireVsCodeApi();
function goto(name) { vscode.postMessage({ type: 'gotoFunction', name }); }
</script>
</body></html>`;
}
