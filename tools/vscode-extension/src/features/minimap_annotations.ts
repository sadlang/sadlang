/**
 * @file minimap_annotations.ts
 * @brief تعليقات الخريطة المصغرة — Minimap Annotations
 * 
 * تلوين مخصص للخريطة المصغرة يبرز الدوال والأصناف والأخطاء والعلامات
 */

import * as vscode from 'vscode';

let functionDecoration: vscode.TextEditorDecorationType;
let classDecoration: vscode.TextEditorDecorationType;
let errorDecoration: vscode.TextEditorDecorationType;
let todoDecoration: vscode.TextEditorDecorationType;
let importDecoration: vscode.TextEditorDecorationType;
let sectionDecoration: vscode.TextEditorDecorationType;

export function registerMinimapAnnotations(context: vscode.ExtensionContext): void {
    // إنشاء أنواع التزيينات
    functionDecoration = vscode.window.createTextEditorDecorationType({
        overviewRulerColor: '#4caf50',
        overviewRulerLane: vscode.OverviewRulerLane.Right,
    });

    classDecoration = vscode.window.createTextEditorDecorationType({
        overviewRulerColor: '#2196f3',
        overviewRulerLane: vscode.OverviewRulerLane.Right,
    });

    errorDecoration = vscode.window.createTextEditorDecorationType({
        overviewRulerColor: '#f44336',
        overviewRulerLane: vscode.OverviewRulerLane.Full,
    });

    todoDecoration = vscode.window.createTextEditorDecorationType({
        overviewRulerColor: '#ff9800',
        overviewRulerLane: vscode.OverviewRulerLane.Left,
    });

    importDecoration = vscode.window.createTextEditorDecorationType({
        overviewRulerColor: '#9c27b0',
        overviewRulerLane: vscode.OverviewRulerLane.Left,
    });

    sectionDecoration = vscode.window.createTextEditorDecorationType({
        overviewRulerColor: '#ffd700',
        overviewRulerLane: vscode.OverviewRulerLane.Center,
        isWholeLine: true,
    });

    context.subscriptions.push(
        functionDecoration, classDecoration, errorDecoration,
        todoDecoration, importDecoration, sectionDecoration
    );

    // تحديث عند فتح أو تغيير الملف
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor(editor => {
            if (editor && editor.document.languageId === 'sad') {
                updateAnnotations(editor);
            }
        }),
        vscode.workspace.onDidChangeTextDocument(event => {
            const editor = vscode.window.activeTextEditor;
            if (editor && event.document === editor.document && editor.document.languageId === 'sad') {
                updateAnnotations(editor);
            }
        }),
        vscode.languages.onDidChangeDiagnostics(() => {
            const editor = vscode.window.activeTextEditor;
            if (editor && editor.document.languageId === 'sad') {
                updateErrorAnnotations(editor);
            }
        })
    );

    // التحديث الأولي
    const editor = vscode.window.activeTextEditor;
    if (editor && editor.document.languageId === 'sad') {
        updateAnnotations(editor);
    }

    // أمر: عرض ملخص الخريطة المصغرة
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.minimapSummary', () => showMinimapSummary())
    );
}

function updateAnnotations(editor: vscode.TextEditor): void {
    const text = editor.document.getText();
    const lines = text.split('\n');

    const functions: vscode.DecorationOptions[] = [];
    const classes: vscode.DecorationOptions[] = [];
    const todos: vscode.DecorationOptions[] = [];
    const imports: vscode.DecorationOptions[] = [];
    const sections: vscode.DecorationOptions[] = [];

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();
        const range = new vscode.Range(i, 0, i, lines[i].length);

        // الدوال
        if (/^(عام\s+|خاص\s+|محمي\s+)?دالة\s+/.test(line)) {
            const funcMatch = line.match(/دالة\s+([\p{L}\p{N}_]+)/u);
            functions.push({
                range,
                hoverMessage: `⚡ دالة: ${funcMatch ? funcMatch[1] : ''}`,
            });
        }

        // الأصناف والبنى
        if (/^صنف\s+|^بنية\s+/.test(line)) {
            const classMatch = line.match(/(صنف|بنية)\s+([\p{L}\p{N}_]+)/u);
            classes.push({
                range,
                hoverMessage: `🏗️ ${classMatch ? classMatch[1] : 'صنف'}: ${classMatch ? classMatch[2] : ''}`,
            });
        }

        // المهام (TODO)
        if (/#.*(?:TODO|FIXME|HACK|مهمة|إصلاح)/i.test(line)) {
            todos.push({
                range,
                hoverMessage: `📌 مهمة: ${line.substring(line.indexOf('#'))}`,
            });
        }

        // الاستيرادات
        if (/^استورد\s+/.test(line)) {
            imports.push({ range });
        }

        // علامات الأقسام (تعليقات بارزة)
        if (/^#{2,}\s*[-=]+|^#\s*[-=]{3,}|^##\s+/.test(line)) {
            sections.push({
                range,
                hoverMessage: `📑 ${line.replace(/^#+\s*[-=]*\s*/, '')}`,
            });
        }
    }

    editor.setDecorations(functionDecoration, functions);
    editor.setDecorations(classDecoration, classes);
    editor.setDecorations(todoDecoration, todos);
    editor.setDecorations(importDecoration, imports);
    editor.setDecorations(sectionDecoration, sections);

    updateErrorAnnotations(editor);
}

function updateErrorAnnotations(editor: vscode.TextEditor): void {
    const diagnostics = vscode.languages.getDiagnostics(editor.document.uri);
    const errors = diagnostics
        .filter(d => d.severity === vscode.DiagnosticSeverity.Error)
        .map(d => ({
            range: d.range,
            hoverMessage: `❌ ${d.message}`,
        }));

    editor.setDecorations(errorDecoration, errors);
}

function showMinimapSummary(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const text = editor.document.getText();
    const lines = text.split('\n');

    let funcs = 0, cls = 0, imp = 0, todo = 0;
    for (const line of lines) {
        const trimmed = line.trim();
        if (/^(عام\s+|خاص\s+|محمي\s+)?دالة\s+/.test(trimmed)) { funcs++; }
        if (/^صنف\s+|^بنية\s+/.test(trimmed)) { cls++; }
        if (/^استورد\s+/.test(trimmed)) { imp++; }
        if (/#.*(?:TODO|FIXME|مهمة|إصلاح)/i.test(trimmed)) { todo++; }
    }

    const diags = vscode.languages.getDiagnostics(editor.document.uri);
    const errors = diags.filter(d => d.severity === vscode.DiagnosticSeverity.Error).length;
    const warnings = diags.filter(d => d.severity === vscode.DiagnosticSeverity.Warning).length;

    vscode.window.showInformationMessage(
        `📊 ملخص: ${funcs} دالة | ${cls} صنف | ${imp} استيراد | ${todo} مهمة | ${errors} خطأ | ${warnings} تحذير`
    );
}
