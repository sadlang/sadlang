/**
 * @file error_lens.ts
 * @brief عدسات الأخطاء — عرض الأخطاء والتحذيرات مباشرة في السطر
 * 
 * يعرض رسائل الأخطاء والتحذيرات بجانب السطر المعني مباشرة
 * بدلاً من الذهاب إلى لوحة المشاكل
 */

import * as vscode from 'vscode';

let errorDecorationType: vscode.TextEditorDecorationType;
let warningDecorationType: vscode.TextEditorDecorationType;
let infoDecorationType: vscode.TextEditorDecorationType;

export function registerErrorLens(context: vscode.ExtensionContext): void {
    errorDecorationType = vscode.window.createTextEditorDecorationType({
        after: {
            margin: '0 0 0 2em',
            fontStyle: 'italic',
        },
        isWholeLine: true,
        backgroundColor: 'rgba(255, 0, 0, 0.08)',
    });

    warningDecorationType = vscode.window.createTextEditorDecorationType({
        after: {
            margin: '0 0 0 2em',
            fontStyle: 'italic',
        },
        isWholeLine: true,
        backgroundColor: 'rgba(255, 200, 0, 0.08)',
    });

    infoDecorationType = vscode.window.createTextEditorDecorationType({
        after: {
            margin: '0 0 0 2em',
            fontStyle: 'italic',
        },
        isWholeLine: true,
    });

    context.subscriptions.push(errorDecorationType, warningDecorationType, infoDecorationType);

    // مراقبة التشخيصات
    context.subscriptions.push(
        vscode.languages.onDidChangeDiagnostics(() => updateDecorations())
    );
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor(() => updateDecorations())
    );

    updateDecorations();
}

function updateDecorations(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor || (editor.document.languageId !== 'sad' && !editor.document.fileName.endsWith('.ص'))) {
        return;
    }

    const diagnostics = vscode.languages.getDiagnostics(editor.document.uri);
    const errors: vscode.DecorationOptions[] = [];
    const warnings: vscode.DecorationOptions[] = [];
    const infos: vscode.DecorationOptions[] = [];

    for (const diag of diagnostics) {
        const line = diag.range.start.line;
        const lineText = editor.document.lineAt(line);
        const decoration: vscode.DecorationOptions = {
            range: lineText.range,
            renderOptions: {
                after: {
                    contentText: `  ◀ ${diag.message}`,
                    color: diag.severity === vscode.DiagnosticSeverity.Error ? '#ff6b6b' :
                           diag.severity === vscode.DiagnosticSeverity.Warning ? '#ffc107' : '#69b3ff',
                }
            }
        };

        if (diag.severity === vscode.DiagnosticSeverity.Error) {
            errors.push(decoration);
        } else if (diag.severity === vscode.DiagnosticSeverity.Warning) {
            warnings.push(decoration);
        } else {
            infos.push(decoration);
        }
    }

    editor.setDecorations(errorDecorationType, errors);
    editor.setDecorations(warningDecorationType, warnings);
    editor.setDecorations(infoDecorationType, infos);
}
