/**
 * @file code_complexity.ts
 * @brief تحليل التعقيد — Cyclomatic Complexity Analyzer
 * 
 * يحلل تعقيد الدوال ويعرض مؤشرات بصرية
 * (أخضر = بسيط، أصفر = متوسط، أحمر = معقد)
 */

import * as vscode from 'vscode';

interface ComplexityResult {
    name: string;
    line: number;
    complexity: number;
    level: 'low' | 'medium' | 'high' | 'very-high';
}

let complexityDecorationType: vscode.TextEditorDecorationType;

export function registerCodeComplexity(context: vscode.ExtensionContext): void {
    complexityDecorationType = vscode.window.createTextEditorDecorationType({
        gutterIconSize: 'contain',
    });
    context.subscriptions.push(complexityDecorationType);

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.analyzeComplexity', () => {
            analyzeAndShow();
        })
    );

    // تحليل تلقائي عند فتح ملف
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor((editor) => {
            if (editor && editor.document.languageId === 'sad') {
                analyzeAndShow();
            }
        })
    );
}

function analyzeAndShow(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const results = analyzeComplexity(editor.document);
    showComplexityDecorations(editor, results);
}

function analyzeComplexity(doc: vscode.TextDocument): ComplexityResult[] {
    const text = doc.getText();
    const lines = text.split('\n');
    const results: ComplexityResult[] = [];

    let currentFunc: { name: string; line: number; complexity: number } | null = null;

    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();

        // بداية دالة
        const funcMatch = trimmed.match(/^دالة\s+(\S+)\s*\(/);
        if (funcMatch) {
            if (currentFunc) {
                results.push(finishFunc(currentFunc));
            }
            currentFunc = { name: funcMatch[1], line: i, complexity: 1 };
            continue;
        }

        // نهاية دالة
        if (trimmed === 'نهاية' && currentFunc) {
            results.push(finishFunc(currentFunc));
            currentFunc = null;
            continue;
        }

        // عناصر التعقيد
        if (currentFunc) {
            if (/^(إذا|اذا)\s*\(/.test(trimmed)) currentFunc.complexity++;
            if (/^وإلا|^والا/.test(trimmed)) currentFunc.complexity++;
            if (/^بينما\s*\(/.test(trimmed)) currentFunc.complexity++;
            if (/^لكل\s+/.test(trimmed)) currentFunc.complexity++;
            if (/^طابق\s*\(/.test(trimmed)) currentFunc.complexity++;
            if (/^عندما\s+/.test(trimmed)) currentFunc.complexity++;
            if (/^حاول/.test(trimmed)) currentFunc.complexity++;
            if (/^امسك/.test(trimmed)) currentFunc.complexity++;
            if (/\s+و\s+|\s+أو\s+|&&|\|\|/.test(trimmed)) currentFunc.complexity++;
            if (/\?\s*/.test(trimmed) && trimmed.includes(':')) currentFunc.complexity++;
        }
    }

    if (currentFunc) {
        results.push(finishFunc(currentFunc));
    }

    return results;
}

function finishFunc(func: { name: string; line: number; complexity: number }): ComplexityResult {
    let level: ComplexityResult['level'];
    if (func.complexity <= 5) level = 'low';
    else if (func.complexity <= 10) level = 'medium';
    else if (func.complexity <= 20) level = 'high';
    else level = 'very-high';

    return { name: func.name, line: func.line, complexity: func.complexity, level };
}

function showComplexityDecorations(editor: vscode.TextEditor, results: ComplexityResult[]): void {
    const decorations: vscode.DecorationOptions[] = [];

    for (const r of results) {
        const line = editor.document.lineAt(r.line);
        const emoji = r.level === 'low' ? '🟢' :
                      r.level === 'medium' ? '🟡' :
                      r.level === 'high' ? '🟠' : '🔴';
        const color = r.level === 'low' ? '#6a9955' :
                      r.level === 'medium' ? '#dcdcaa' :
                      r.level === 'high' ? '#ce9178' : '#f44747';
        const label = r.level === 'low' ? 'بسيط' :
                      r.level === 'medium' ? 'متوسط' :
                      r.level === 'high' ? 'معقد' : 'معقد جداً';

        decorations.push({
            range: line.range,
            renderOptions: {
                after: {
                    contentText: `  ${emoji} تعقيد: ${r.complexity} (${label})`,
                    color: color,
                    fontStyle: 'italic',
                    margin: '0 0 0 2em',
                }
            }
        });
    }

    editor.setDecorations(complexityDecorationType, decorations);
}
