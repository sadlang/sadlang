/**
 * @file markdown_doc_preview.ts
 * @brief معاينة التوثيق — Documentation Preview
 * 
 * توليد ومعاينة توثيق تلقائي من تعليقات الكود بصيغة Markdown
 */

import * as vscode from 'vscode';
import * as path from 'path';

interface DocEntry {
    type: 'function' | 'class' | 'variable' | 'constant';
    name: string;
    line: number;
    doc: string;
    params: string[];
    returnType: string;
    visibility: string;
    body: string;
}

export function registerMarkdownDocPreview(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.previewDocs', () => previewDocs(context)),
        vscode.commands.registerCommand('sad.exportDocs', () => exportDocs()),
        vscode.commands.registerCommand('sad.generateFileDoc', () => generateFileDocHeader()),
    );
}

async function previewDocs(context: vscode.ExtensionContext): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') {
        vscode.window.showWarningMessage('افتح ملف .ص أولاً');
        return;
    }

    const entries = parseDocEntries(editor.document);
    const markdown = generateMarkdown(editor.document, entries);

    const panel = vscode.window.createWebviewPanel(
        'sadDocPreview', `📖 توثيق — ${path.basename(editor.document.uri.fsPath)}`,
        vscode.ViewColumn.Two, { enableScripts: true }
    );

    const isDark = vscode.window.activeColorTheme.kind === vscode.ColorThemeKind.Dark;
    panel.webview.html = getPreviewHTML(markdown, isDark);
}

function parseDocEntries(doc: vscode.TextDocument): DocEntry[] {
    const entries: DocEntry[] = [];
    const text = doc.getText();
    const lines = text.split('\n');

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();

        // جمع التعليقات التوثيقية قبل التعريف
        let docComment = '';
        let j = i - 1;
        while (j >= 0 && (lines[j].trim().startsWith('##') || lines[j].trim().startsWith('#'))) {
            const commentLine = lines[j].trim().replace(/^#+\s*/, '');
            docComment = commentLine + '\n' + docComment;
            j--;
        }

        // الدوال
        const funcMatch = line.match(/^(عام\s+|خاص\s+|محمي\s+)?دالة\s+([\p{L}\p{N}_]+)\s*\((.*?)\)/u);
        if (funcMatch) {
            const visibility = funcMatch[1]?.trim() || 'عام';
            const name = funcMatch[2];
            const params = funcMatch[3] ? funcMatch[3].split(/[,،]/).map(p => p.trim()).filter(p => p) : [];

            // البحث عن ارجع لتحديد نوع الإرجاع
            let returnType = 'فراغ';
            for (let k = i + 1; k < Math.min(i + 50, lines.length); k++) {
                if (lines[k].trim() === 'نهاية') { break; }
                if (lines[k].trim().startsWith('ارجع')) {
                    returnType = 'قيمة';
                    break;
                }
            }

            entries.push({
                type: 'function',
                name,
                line: i + 1,
                doc: docComment.trim(),
                params,
                returnType,
                visibility,
                body: '',
            });
        }

        // الأصناف
        const classMatch = line.match(/^صنف\s+([\p{L}\p{N}_]+)(?:\s+يرث\s+([\p{L}\p{N}_]+))?/u);
        if (classMatch) {
            const parent = classMatch[2] || '';
            entries.push({
                type: 'class',
                name: classMatch[1],
                line: i + 1,
                doc: docComment.trim(),
                params: parent ? [parent] : [],
                returnType: '',
                visibility: 'عام',
                body: '',
            });
        }

        // المتغيرات والثوابت
        const varMatch = line.match(/^(متغير|ثابت)\s+([\p{L}\p{N}_]+)/u);
        if (varMatch) {
            entries.push({
                type: varMatch[1] === 'ثابت' ? 'constant' : 'variable',
                name: varMatch[2],
                line: i + 1,
                doc: docComment.trim(),
                params: [],
                returnType: '',
                visibility: 'عام',
                body: line,
            });
        }
    }

    return entries;
}

function generateMarkdown(doc: vscode.TextDocument, entries: DocEntry[]): string {
    const fileName = path.basename(doc.uri.fsPath, '.ص');
    let md = `# 📖 توثيق: ${fileName}\n\n`;

    // جدول المحتويات
    md += `## 📑 جدول المحتويات\n\n`;
    const functions = entries.filter(e => e.type === 'function');
    const classes = entries.filter(e => e.type === 'class');
    const variables = entries.filter(e => e.type === 'variable' || e.type === 'constant');

    if (classes.length > 0) {
        md += `### 🏗️ الأصناف\n`;
        classes.forEach(c => { md += `- [${c.name}](#${c.name})\n`; });
        md += `\n`;
    }

    if (functions.length > 0) {
        md += `### ⚡ الدوال\n`;
        functions.forEach(f => { md += `- [${f.name}](#${f.name}) ${f.doc ? '— ' + f.doc.split('\n')[0] : ''}\n`; });
        md += `\n`;
    }

    if (variables.length > 0) {
        md += `### 📦 المتغيرات والثوابت\n`;
        variables.forEach(v => { md += `- \`${v.name}\` (${v.type === 'constant' ? 'ثابت' : 'متغير'})\n`; });
        md += `\n`;
    }

    md += `---\n\n`;

    // تفاصيل الأصناف
    for (const cls of classes) {
        md += `## 🏗️ صنف ${cls.name}\n\n`;
        if (cls.params.length > 0) { md += `**يرث من:** ${cls.params[0]}\n\n`; }
        if (cls.doc) { md += `${cls.doc}\n\n`; }
        md += `📍 سطر ${cls.line}\n\n---\n\n`;
    }

    // تفاصيل الدوال
    for (const func of functions) {
        const icon = func.visibility === 'خاص' ? '🔒' : func.visibility === 'محمي' ? '🛡️' : '🔓';
        md += `## ⚡ ${icon} ${func.name}\n\n`;
        if (func.doc) { md += `${func.doc}\n\n`; }
        md += `\`\`\`sad\n${func.visibility} دالة ${func.name}(${func.params.join('، ')})\n\`\`\`\n\n`;

        if (func.params.length > 0) {
            md += `**المعاملات:**\n\n`;
            md += `| المعامل | الوصف |\n`;
            md += `|---------|-------|\n`;
            func.params.forEach(p => { md += `| \`${p}\` | — |\n`; });
            md += `\n`;
        }

        md += `**الإرجاع:** ${func.returnType}\n\n`;
        md += `📍 سطر ${func.line}\n\n---\n\n`;
    }

    // تفاصيل المتغيرات
    if (variables.length > 0) {
        md += `## 📦 المتغيرات والثوابت\n\n`;
        md += `| الاسم | النوع | القيمة | الوصف |\n`;
        md += `|-------|-------|--------|-------|\n`;
        for (const v of variables) {
            const valueMatch = v.body.match(/=\s*(.+)$/);
            const value = valueMatch ? valueMatch[1].trim() : '—';
            md += `| \`${v.name}\` | ${v.type === 'constant' ? 'ثابت' : 'متغير'} | ${value} | ${v.doc.split('\n')[0] || '—'} |\n`;
        }
        md += `\n`;
    }

    // إحصائيات
    md += `---\n\n`;
    md += `## 📊 إحصائيات\n\n`;
    md += `- **الأسطر:** ${doc.lineCount}\n`;
    md += `- **الدوال:** ${functions.length}\n`;
    md += `- **الأصناف:** ${classes.length}\n`;
    md += `- **المتغيرات:** ${variables.length}\n`;
    md += `\n*تم التوليد تلقائياً بواسطة أداة توثيق لغة ص*\n`;

    return md;
}

function getPreviewHTML(markdown: string, isDark: boolean): string {
    const bg = isDark ? '#1e1e1e' : '#fff';
    const fg = isDark ? '#d4d4d4' : '#333';
    const codeBg = isDark ? '#2d2d30' : '#f5f5f5';
    const borderColor = isDark ? '#333' : '#ddd';

    // تحويل Markdown بسيط إلى HTML
    let html = markdown
        .replace(/^### (.+)$/gm, '<h3>$1</h3>')
        .replace(/^## (.+)$/gm, '<h2>$1</h2>')
        .replace(/^# (.+)$/gm, '<h1>$1</h1>')
        .replace(/\*\*(.+?)\*\*/g, '<strong>$1</strong>')
        .replace(/`([^`]+)`/g, '<code>$1</code>')
        .replace(/```sad\n([\s\S]*?)```/g, `<pre style="background:${codeBg};padding:12px;border-radius:6px;direction:rtl"><code>$1</code></pre>`)
        .replace(/^- (.+)$/gm, '<li>$1</li>')
        .replace(/(<li>.*<\/li>\n?)+/g, '<ul>$&</ul>')
        .replace(/^\|(.+)\|$/gm, (match) => {
            const cells = match.split('|').filter(c => c.trim());
            if (cells.every(c => /^[-]+$/.test(c.trim()))) { return ''; }
            const isHeader = cells.some(c => c.includes('الاسم') || c.includes('المعامل'));
            const tag = isHeader ? 'th' : 'td';
            return `<tr>${cells.map(c => `<${tag} style="padding:8px;border:1px solid ${borderColor}">${c.trim()}</${tag}>`).join('')}</tr>`;
        })
        .replace(/(<tr>.*<\/tr>\n?)+/g, '<table style="border-collapse:collapse;width:100%">$&</table>')
        .replace(/^---$/gm, '<hr>')
        .replace(/\n\n/g, '<br><br>')
        .replace(/📍 سطر (\d+)/g, '<span style="opacity:0.6">📍 سطر $1</span>');

    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
    body { font-family: 'Segoe UI', Tahoma, sans-serif; background: ${bg}; color: ${fg}; padding: 30px; direction: rtl; line-height: 1.8; max-width: 900px; margin: 0 auto; }
    h1 { color: #4fc3f7; border-bottom: 2px solid #4fc3f7; padding-bottom: 8px; }
    h2 { color: #81c784; margin-top: 30px; }
    h3 { color: #ffb74d; }
    code { background: ${codeBg}; padding: 2px 6px; border-radius: 3px; font-family: 'Cascadia Code', monospace; }
    pre code { padding: 0; background: none; }
    hr { border: none; border-top: 1px solid ${borderColor}; margin: 20px 0; }
    table { margin: 10px 0; }
    th { background: #4fc3f7; color: white; }
    ul { padding-right: 20px; }
</style></head>
<body>${html}</body></html>`;
}

async function exportDocs(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const entries = parseDocEntries(editor.document);
    const markdown = generateMarkdown(editor.document, entries);

    const fileName = path.basename(editor.document.uri.fsPath, '.ص');
    const saveUri = await vscode.window.showSaveDialog({
        defaultUri: vscode.Uri.file(path.join(path.dirname(editor.document.uri.fsPath), `${fileName}_توثيق.md`)),
        filters: { 'Markdown': ['md'] },
    });

    if (saveUri) {
        await vscode.workspace.fs.writeFile(saveUri, Buffer.from(markdown, 'utf-8'));
        vscode.window.showInformationMessage(`✅ تم تصدير التوثيق: ${path.basename(saveUri.fsPath)}`);

        // فتح الملف
        const doc = await vscode.workspace.openTextDocument(saveUri);
        await vscode.window.showTextDocument(doc, vscode.ViewColumn.Two);
    }
}

async function generateFileDocHeader(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const fileName = path.basename(editor.document.uri.fsPath, '.ص');
    const today = new Date().toLocaleDateString('ar');
    const entries = parseDocEntries(editor.document);
    const funcs = entries.filter(e => e.type === 'function').map(e => e.name);

    const header = `#** 
# @ملف ${fileName}.ص
# @تاريخ ${today}
# @وصف [أضف وصف الملف هنا]
#
# الدوال المُصدّرة:
${funcs.map(f => `#   - ${f}()`).join('\n')}
#
# الاعتمادات: [أضف الاعتمادات]
**#

`;

    const edit = new vscode.WorkspaceEdit();
    edit.insert(editor.document.uri, new vscode.Position(0, 0), header);
    await vscode.workspace.applyEdit(edit);

    vscode.window.showInformationMessage('✅ تم إضافة رأس التوثيق');
}
