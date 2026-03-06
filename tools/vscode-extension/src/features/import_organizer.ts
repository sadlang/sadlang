/**
 * @file import_organizer.ts
 * @brief منظم الاستيرادات — Import Organizer
 * 
 * يرتب عبارات الاستيراد تلقائياً حسب:
 *   1. المكتبة القياسية أولاً
 *   2. المكتبات الخارجية
 *   3. الاستيرادات المحلية
 * مع إزالة التكرارات وإضافة أسطر فاصلة بين المجموعات
 */

import * as vscode from 'vscode';

interface ImportStatement {
    text: string;
    line: number;
    module: string;
    group: 'stdlib' | 'external' | 'local';
}

const STDLIB_MODULES = [
    'رياضيات', 'نصوص', 'ملفات', 'شبكة', 'نظام', 'وقت',
    'تجميع', 'إدخال', 'math', 'io', 'string', 'core',
    'network', 'graphics', 'json', 'regex'
];

export function registerImportOrganizer(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.organizeImports', () => {
            organizeImports();
        })
    );

    // تشغيل تلقائي عند الحفظ (إذا مُفعّل)
    context.subscriptions.push(
        vscode.workspace.onWillSaveTextDocument((event) => {
            if (event.document.languageId === 'sad') {
                const config = vscode.workspace.getConfiguration('sad');
                if (config.get<boolean>('organizeImportsOnSave', false)) {
                    event.waitUntil(getOrganizedEdits(event.document));
                }
            }
        })
    );

    // أمر إزالة الاستيرادات غير المستخدمة
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.removeUnusedImports', () => {
            removeUnusedImports();
        })
    );

    // أمر إضافة استيراد تلقائي
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.addImport', async () => {
            await addImportInteractive();
        })
    );
}

function organizeImports(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const edits = computeOrganizedEdits(editor.document);
    if (edits.length === 0) {
        vscode.window.showInformationMessage('✅ الاستيرادات مرتبة بالفعل');
        return;
    }

    const wsEdit = new vscode.WorkspaceEdit();
    for (const edit of edits) {
        wsEdit.replace(editor.document.uri, edit.range, edit.newText);
    }
    vscode.workspace.applyEdit(wsEdit).then(() => {
        vscode.window.showInformationMessage(`✅ تم ترتيب ${edits.length > 0 ? 'الاستيرادات' : ''}`);
    });
}

async function getOrganizedEdits(doc: vscode.TextDocument): Promise<vscode.TextEdit[]> {
    return computeOrganizedEdits(doc);
}

function computeOrganizedEdits(doc: vscode.TextDocument): vscode.TextEdit[] {
    const imports = parseImports(doc);
    if (imports.length === 0) return [];

    // ترتيب: stdlib → external → local
    const stdlib = imports.filter(i => i.group === 'stdlib').sort((a, b) => a.module.localeCompare(b.module));
    const external = imports.filter(i => i.group === 'external').sort((a, b) => a.module.localeCompare(b.module));
    const local = imports.filter(i => i.group === 'local').sort((a, b) => a.module.localeCompare(b.module));

    // إزالة التكرارات
    const unique = (arr: ImportStatement[]) => {
        const seen = new Set<string>();
        return arr.filter(i => {
            if (seen.has(i.text)) return false;
            seen.add(i.text);
            return true;
        });
    };

    const organized = [
        ...unique(stdlib),
        ...(unique(stdlib).length > 0 && (unique(external).length > 0 || unique(local).length > 0) ? [null] : []),
        ...unique(external),
        ...(unique(external).length > 0 && unique(local).length > 0 ? [null] : []),
        ...unique(local)
    ];

    const newText = organized.map(i => i ? i.text : '').join('\n');

    // نطاق الاستبدال: من أول استيراد إلى آخر استيراد
    const firstLine = Math.min(...imports.map(i => i.line));
    const lastLine = Math.max(...imports.map(i => i.line));
    const range = new vscode.Range(firstLine, 0, lastLine, doc.lineAt(lastLine).text.length);

    return [new vscode.TextEdit(range, newText)];
}

function parseImports(doc: vscode.TextDocument): ImportStatement[] {
    const imports: ImportStatement[] = [];
    const importRegex = /^(استورد\s+.+)$/;

    for (let i = 0; i < doc.lineCount; i++) {
        const line = doc.lineAt(i).text.trim();
        if (importRegex.test(line)) {
            // استخراج اسم الوحدة
            const fromMatch = line.match(/من\s+"([^"]+)"/);
            const module = fromMatch ? fromMatch[1] : line;

            // تحديد المجموعة
            let group: ImportStatement['group'] = 'local';
            if (STDLIB_MODULES.some(m => module.includes(m))) {
                group = 'stdlib';
            } else if (module.startsWith('.') || module.startsWith('/') || module.startsWith('\\')) {
                group = 'local';
            } else if (!module.includes('/') && !module.includes('\\')) {
                group = 'external';
            }

            imports.push({ text: doc.lineAt(i).text, line: i, module, group });
        }
    }

    return imports;
}

function removeUnusedImports(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const doc = editor.document;
    const text = doc.getText();
    const imports = parseImports(doc);
    const linesToRemove: number[] = [];

    for (const imp of imports) {
        // استخراج الأسماء المستوردة
        const namesMatch = imp.text.match(/استورد\s+(.+?)\s+من/);
        if (namesMatch) {
            const names = namesMatch[1].split(/[,،]/).map(n => n.trim());
            const allUnused = names.every(name => {
                const usageRegex = new RegExp(`\\b${name}\\b`, 'g');
                const matches = text.match(usageRegex);
                return !matches || matches.length <= 1; // 1 = الاستيراد نفسه
            });
            if (allUnused) {
                linesToRemove.push(imp.line);
            }
        }
    }

    if (linesToRemove.length === 0) {
        vscode.window.showInformationMessage('✅ جميع الاستيرادات مستخدمة');
        return;
    }

    const wsEdit = new vscode.WorkspaceEdit();
    for (const line of linesToRemove.reverse()) {
        const range = new vscode.Range(line, 0, line + 1, 0);
        wsEdit.delete(doc.uri, range);
    }

    vscode.workspace.applyEdit(wsEdit).then(() => {
        vscode.window.showInformationMessage(`🗑️ تم إزالة ${linesToRemove.length} استيراد غير مستخدم`);
    });
}

async function addImportInteractive(): Promise<void> {
    const module = await vscode.window.showInputBox({
        prompt: 'اسم الوحدة — Module name',
        placeHolder: 'مثل: رياضيات أو ./مساعدات'
    });
    if (!module) { return; }

    const name = await vscode.window.showInputBox({
        prompt: 'الاسم المستورد — Import name',
        placeHolder: 'مثل: جذر_تربيعي أو *'
    });
    if (!name) { return; }

    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const importLine = `استورد ${name} من "${module}"`;
    const position = new vscode.Position(0, 0);

    editor.edit(builder => {
        builder.insert(position, importLine + '\n');
    });
}
