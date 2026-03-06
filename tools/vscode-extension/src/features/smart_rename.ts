/**
 * @file smart_rename.ts
 * @brief إعادة التسمية الذكية — Smart Rename
 * 
 * إعادة تسمية متقدمة مع معاينة وتأثيرات عبر الملفات
 */

import * as vscode from 'vscode';

export function registerSmartRename(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.smartRename', () => smartRename()),
        vscode.commands.registerCommand('sad.renameSymbol', () => renameSymbolWizard()),
        vscode.commands.registerCommand('sad.batchRename', () => batchRename()),
    );
}

async function smartRename(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    // استخراج الكلمة تحت المؤشر
    const wordRange = editor.document.getWordRangeAtPosition(editor.selection.active, /[\p{L}\p{N}_]+/u);
    if (!wordRange) {
        vscode.window.showWarningMessage('ضع المؤشر على اسم لإعادة تسميته');
        return;
    }

    const oldName = editor.document.getText(wordRange);

    // البحث عن جميع الاستخدامات في نفس الملف
    const text = editor.document.getText();
    const regex = new RegExp(`\\b${escapeRegex(oldName)}\\b`, 'gu');
    const occurrences: vscode.Range[] = [];
    let match;
    while ((match = regex.exec(text)) !== null) {
        const startPos = editor.document.positionAt(match.index);
        const endPos = editor.document.positionAt(match.index + match[0].length);
        occurrences.push(new vscode.Range(startPos, endPos));
    }

    // الطلب من المستخدم
    const newName = await vscode.window.showInputBox({
        prompt: `إعادة تسمية "${oldName}" (${occurrences.length} استخدام في هذا الملف)`,
        value: oldName,
        validateInput: (value) => {
            if (!value.trim()) { return 'الاسم لا يمكن أن يكون فارغاً'; }
            if (/^[0-9]/.test(value)) { return 'الاسم لا يمكن أن يبدأ برقم'; }
            return null;
        }
    });

    if (!newName || newName === oldName) { return; }

    // البحث في جميع ملفات المشروع
    const allFiles = await vscode.workspace.findFiles('**/*.ص', '**/node_modules/**');
    const affectedFiles: { uri: vscode.Uri; count: number }[] = [];

    for (const file of allFiles) {
        try {
            const content = (await vscode.workspace.fs.readFile(file)).toString();
            const fileRegex = new RegExp(`\\b${escapeRegex(oldName)}\\b`, 'gu');
            const matches = content.match(fileRegex);
            if (matches && matches.length > 0) {
                affectedFiles.push({ uri: file, count: matches.length });
            }
        } catch { /* skip */ }
    }

    // عرض معاينة
    const totalOccurrences = affectedFiles.reduce((s, f) => s + f.count, 0);
    const fileList = affectedFiles.map(f => `  ${vscode.workspace.asRelativePath(f.uri)}: ${f.count}`).join('\n');

    const confirm = await vscode.window.showWarningMessage(
        `إعادة تسمية "${oldName}" → "${newName}"\n${totalOccurrences} استخدام في ${affectedFiles.length} ملف`,
        { modal: true, detail: fileList },
        'تأكيد', 'هذا الملف فقط'
    );

    if (!confirm) { return; }

    const edit = new vscode.WorkspaceEdit();

    if (confirm === 'هذا الملف فقط') {
        // إعادة تسمية في الملف الحالي فقط
        for (const range of occurrences) {
            edit.replace(editor.document.uri, range, newName);
        }
    } else {
        // إعادة تسمية عبر جميع الملفات
        for (const file of affectedFiles) {
            try {
                const doc = await vscode.workspace.openTextDocument(file.uri);
                const content = doc.getText();
                const fileRegex = new RegExp(`\\b${escapeRegex(oldName)}\\b`, 'gu');
                let m;
                while ((m = fileRegex.exec(content)) !== null) {
                    const start = doc.positionAt(m.index);
                    const end = doc.positionAt(m.index + m[0].length);
                    edit.replace(file.uri, new vscode.Range(start, end), newName);
                }
            } catch { /* skip */ }
        }
    }

    const success = await vscode.workspace.applyEdit(edit);
    if (success) {
        vscode.window.showInformationMessage(
            `✅ تم إعادة تسمية "${oldName}" → "${newName}" (${totalOccurrences} موقع)`
        );
    }
}

async function renameSymbolWizard(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const text = editor.document.getText();

    // جمع جميع الرموز في الملف
    const symbols: { name: string; type: string; line: number }[] = [];

    // الدوال
    const funcRegex = /دالة\s+([\p{L}\p{N}_]+)/gu;
    let match;
    while ((match = funcRegex.exec(text)) !== null) {
        const pos = editor.document.positionAt(match.index);
        symbols.push({ name: match[1], type: 'دالة', line: pos.line + 1 });
    }

    // الأصناف
    const classRegex = /صنف\s+([\p{L}\p{N}_]+)/gu;
    while ((match = classRegex.exec(text)) !== null) {
        const pos = editor.document.positionAt(match.index);
        symbols.push({ name: match[1], type: 'صنف', line: pos.line + 1 });
    }

    // المتغيرات
    const varRegex = /(?:متغير|ثابت)\s+([\p{L}\p{N}_]+)/gu;
    while ((match = varRegex.exec(text)) !== null) {
        const pos = editor.document.positionAt(match.index);
        symbols.push({ name: match[1], type: 'متغير', line: pos.line + 1 });
    }

    if (symbols.length === 0) {
        vscode.window.showInformationMessage('لم يتم العثور على رموز في الملف');
        return;
    }

    const items = symbols.map(s => ({
        label: `$(symbol-${s.type === 'دالة' ? 'method' : s.type === 'صنف' ? 'class' : 'variable'}) ${s.name}`,
        description: `${s.type} — سطر ${s.line}`,
        symbol: s
    }));

    const selected = await vscode.window.showQuickPick(items, {
        placeHolder: 'اختر رمزاً لإعادة تسميته'
    });

    if (selected) {
        // الانتقال إلى الرمز ثم تسمية
        const pos = new vscode.Position(selected.symbol.line - 1, 0);
        editor.selection = new vscode.Selection(pos, pos);
        editor.revealRange(new vscode.Range(pos, pos));

        // استخدام الاسم القديم مباشرة
        const wordRange = editor.document.getWordRangeAtPosition(
            new vscode.Position(selected.symbol.line - 1, editor.document.lineAt(selected.symbol.line - 1).text.indexOf(selected.symbol.name)),
            /[\p{L}\p{N}_]+/u
        );

        if (wordRange) {
            editor.selection = new vscode.Selection(wordRange.start, wordRange.end);
            await smartRename();
        }
    }
}

async function batchRename(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const input = await vscode.window.showInputBox({
        prompt: 'أدخل أزواج الاستبدال (قديم→جديد، قديم2→جديد2)',
        placeHolder: 'مثال: س→عدد، ن→اسم'
    });

    if (!input) { return; }

    const pairs = input.split(/[,،]/).map(p => {
        const parts = p.split('→');
        return parts.length === 2 ? { old: parts[0].trim(), new: parts[1].trim() } : null;
    }).filter((p): p is { old: string; new: string } => p !== null && p.old.length > 0 && p.new.length > 0);

    if (pairs.length === 0) {
        vscode.window.showWarningMessage('صيغة غير صحيحة. استخدم: قديم→جديد');
        return;
    }

    const edit = new vscode.WorkspaceEdit();
    let totalReplacements = 0;

    for (const pair of pairs) {
        const regex = new RegExp(`\\b${escapeRegex(pair.old)}\\b`, 'gu');
        const text = editor.document.getText();
        let match;
        while ((match = regex.exec(text)) !== null) {
            const start = editor.document.positionAt(match.index);
            const end = editor.document.positionAt(match.index + match[0].length);
            edit.replace(editor.document.uri, new vscode.Range(start, end), pair.new);
            totalReplacements++;
        }
    }

    if (totalReplacements > 0) {
        await vscode.workspace.applyEdit(edit);
        vscode.window.showInformationMessage(`✅ تم استبدال ${totalReplacements} موقع في ${pairs.length} زوج`);
    } else {
        vscode.window.showInformationMessage('لم يتم العثور على أي تطابق');
    }
}

function escapeRegex(str: string): string {
    return str.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}
