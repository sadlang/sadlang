/**
 * @file inline_runner.ts
 * @brief تشغيل أجزاء الكود مباشرة — Run code blocks inline
 * 
 * يتيح تحديد جزء من الكود وتشغيله مباشرة
 * مع عرض النتيجة كتعليق مضمن بجانب الكود
 */

import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { execFile } from 'child_process';

let resultDecorationType: vscode.TextEditorDecorationType;

export function registerInlineRunner(context: vscode.ExtensionContext): void {
    resultDecorationType = vscode.window.createTextEditorDecorationType({
        after: {
            margin: '0 0 0 2em',
            fontStyle: 'italic',
        }
    });
    context.subscriptions.push(resultDecorationType);

    // تشغيل الكود المحدد
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.runSelection', async () => {
            await runSelectedCode();
        })
    );

    // تشغيل السطر الحالي
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.runLine', async () => {
            await runCurrentLine();
        })
    );

    // تشغيل من البداية حتى المؤشر
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.runToCursor', async () => {
            await runToCursor();
        })
    );

    // مسح النتائج المضمنة
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.clearInlineResults', () => {
            const editor = vscode.window.activeTextEditor;
            if (editor) { editor.setDecorations(resultDecorationType, []); }
        })
    );
}

async function runSelectedCode(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const selection = editor.selection;
    const code = selection.isEmpty
        ? editor.document.lineAt(selection.active.line).text
        : editor.document.getText(selection);

    if (!code.trim()) { return; }

    const result = await executeCode(code);
    showInlineResult(editor, selection.end.line, result);
}

async function runCurrentLine(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const line = editor.selection.active.line;
    const code = editor.document.lineAt(line).text;
    if (!code.trim()) { return; }

    const result = await executeCode(code);
    showInlineResult(editor, line, result);
}

async function runToCursor(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const endLine = editor.selection.active.line;
    const lines: string[] = [];
    for (let i = 0; i <= endLine; i++) {
        lines.push(editor.document.lineAt(i).text);
    }
    const code = lines.join('\n');

    const result = await executeCode(code);
    showInlineResult(editor, endLine, result);
}

async function executeCode(code: string): Promise<string> {
    const interpreterPath = findInterpreter();
    if (!interpreterPath) {
        return '⚠️ لم يتم العثور على المفسر';
    }

    // كتابة الكود في ملف مؤقت
    const tmpDir = require('os').tmpdir();
    const tmpFile = path.join(tmpDir, `sad_inline_${Date.now()}.ص`);

    try {
        fs.writeFileSync(tmpFile, code, 'utf-8');

        return new Promise<string>((resolve) => {
            execFile(interpreterPath, [tmpFile], {
                timeout: 5000,
                encoding: 'utf-8',
                maxBuffer: 1024 * 1024
            }, (error, stdout, stderr) => {
                try { fs.unlinkSync(tmpFile); } catch { /* ignore */ }

                if (error) {
                    if (error.killed) {
                        resolve('⏰ مهلة — timeout');
                    } else {
                        resolve(`❌ ${stderr || error.message}`);
                    }
                } else {
                    const output = stdout.trim();
                    resolve(output ? `✅ ${output}` : '✅ (لا ناتج)');
                }
            });
        });
    } catch (err) {
        return `❌ خطأ: ${err}`;
    }
}

function showInlineResult(editor: vscode.TextEditor, line: number, result: string): void {
    const lineRange = editor.document.lineAt(line).range;
    const firstLine = result.split('\n')[0].substring(0, 80);
    const hasMore = result.includes('\n') || result.length > 80;

    const decoration: vscode.DecorationOptions = {
        range: lineRange,
        renderOptions: {
            after: {
                contentText: `  → ${firstLine}${hasMore ? '...' : ''}`,
                color: result.startsWith('✅') ? '#6a9955' :
                       result.startsWith('❌') ? '#f44747' : '#dcdcaa',
                fontStyle: 'italic',
            }
        }
    };

    editor.setDecorations(resultDecorationType, [decoration]);
}

function findInterpreter(): string {
    const config = vscode.workspace.getConfiguration('sad');
    const customPath = config.get<string>('interpreter.path', '');
    if (customPath && fs.existsSync(customPath)) { return customPath; }

    const workspaceRoot = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || '';
    const candidates = [
        path.join(workspaceRoot, 'build', 'bin', 'Debug', 'sad.exe'),
        path.join(workspaceRoot, 'build', 'bin', 'Release', 'sad.exe'),
        path.join(workspaceRoot, 'build', 'bin', 'sad.exe'),
    ];

    for (const p of candidates) {
        if (fs.existsSync(p)) { return p; }
    }
    return '';
}
