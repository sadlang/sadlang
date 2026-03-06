/**
 * @file bookmark_manager.ts
 * @brief مدير العلامات المرجعية — Bookmark Manager
 * 
 * نظام علامات مرجعية عربي مع تصنيف وملاحظات وتنقل سريع
 */

import * as vscode from 'vscode';
import * as path from 'path';

interface Bookmark {
    id: string;
    file: string;
    line: number;
    text: string;
    label: string;
    category: string;
    timestamp: number;
}

let bookmarks: Bookmark[] = [];
let bookmarkDecoration: vscode.TextEditorDecorationType;
let statusBar: vscode.StatusBarItem;

export function registerBookmarkManager(context: vscode.ExtensionContext): void {
    // استعادة العلامات المحفوظة
    bookmarks = context.workspaceState.get<Bookmark[]>('sadBookmarks', []);

    bookmarkDecoration = vscode.window.createTextEditorDecorationType({
        gutterIconPath: vscode.Uri.parse('data:image/svg+xml,' + encodeURIComponent(
            '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 16"><path fill="#4fc3f7" d="M3 2v12l5-3 5 3V2z"/></svg>'
        )),
        gutterIconSize: 'contain',
        overviewRulerColor: '#4fc3f7',
        overviewRulerLane: vscode.OverviewRulerLane.Right,
        backgroundColor: 'rgba(79, 195, 247, 0.08)',
        isWholeLine: true,
    });
    context.subscriptions.push(bookmarkDecoration);

    // شريط الحالة
    statusBar = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 80);
    statusBar.command = 'sad.listBookmarks';
    updateStatusBar();
    statusBar.show();
    context.subscriptions.push(statusBar);

    // الأوامر
    const commands: [string, (...args: any[]) => any][] = [
        ['sad.toggleBookmark', () => toggleBookmark(context)],
        ['sad.toggleBookmarkWithLabel', () => toggleBookmarkWithLabel(context)],
        ['sad.listBookmarks', () => listBookmarks()],
        ['sad.nextBookmark', () => navigateBookmark(1)],
        ['sad.prevBookmark', () => navigateBookmark(-1)],
        ['sad.clearBookmarks', () => clearBookmarks(context)],
        ['sad.showBookmarkPanel', () => showBookmarkPanel(context)],
    ];

    for (const [cmd, handler] of commands) {
        context.subscriptions.push(vscode.commands.registerCommand(cmd, handler));
    }

    // تحديث التزيين عند تغيير المحرر أو المستند
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor(() => refreshDecorations()),
        vscode.workspace.onDidChangeTextDocument(() => refreshDecorations())
    );

    refreshDecorations();
}

function saveBookmarks(context: vscode.ExtensionContext): void {
    context.workspaceState.update('sadBookmarks', bookmarks);
    updateStatusBar();
    refreshDecorations();
}

function updateStatusBar(): void {
    statusBar.text = `$(bookmark) ${bookmarks.length} علامة`;
    statusBar.tooltip = 'عرض العلامات المرجعية';
}

function toggleBookmark(context: vscode.ExtensionContext): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const line = editor.selection.active.line;
    const file = editor.document.uri.fsPath;

    const existing = bookmarks.findIndex(b => b.file === file && b.line === line);
    if (existing >= 0) {
        bookmarks.splice(existing, 1);
        vscode.window.showInformationMessage('🔖 تم إزالة العلامة');
    } else {
        const lineText = editor.document.lineAt(line).text.trim();
        bookmarks.push({
            id: Date.now().toString(),
            file,
            line,
            text: lineText.substring(0, 80),
            label: '',
            category: 'عام',
            timestamp: Date.now(),
        });
        vscode.window.showInformationMessage('🔖 تم إضافة علامة');
    }
    saveBookmarks(context);
}

async function toggleBookmarkWithLabel(context: vscode.ExtensionContext): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const label = await vscode.window.showInputBox({
        prompt: 'وصف العلامة المرجعية',
        placeHolder: 'اكتب وصفاً للعلامة...'
    });
    if (label === undefined) { return; }

    const categories = ['عام', 'مهم', 'مراجعة', 'خطأ', 'تحسين'];
    const category = await vscode.window.showQuickPick(categories, {
        placeHolder: 'اختر التصنيف'
    });

    const line = editor.selection.active.line;
    const file = editor.document.uri.fsPath;
    const lineText = editor.document.lineAt(line).text.trim();

    bookmarks.push({
        id: Date.now().toString(),
        file,
        line,
        text: lineText.substring(0, 80),
        label: label || '',
        category: category || 'عام',
        timestamp: Date.now(),
    });
    saveBookmarks(context);
    vscode.window.showInformationMessage(`🔖 تم إضافة علامة: ${label || lineText.substring(0, 30)}`);
}

async function listBookmarks(): Promise<void> {
    if (bookmarks.length === 0) {
        vscode.window.showInformationMessage('لا توجد علامات مرجعية');
        return;
    }

    const items = bookmarks.map(b => ({
        label: `$(bookmark) ${b.label || b.text.substring(0, 50)}`,
        description: `${path.basename(b.file)}:${b.line + 1}`,
        detail: `[${b.category}] ${b.text}`,
        bookmark: b,
    }));

    const selected = await vscode.window.showQuickPick(items, {
        placeHolder: 'اختر علامة للانتقال إليها',
        matchOnDescription: true,
        matchOnDetail: true,
    });

    if (selected) {
        await navigateTo(selected.bookmark);
    }
}

async function navigateTo(bookmark: Bookmark): Promise<void> {
    const doc = await vscode.workspace.openTextDocument(bookmark.file);
    const editor = await vscode.window.showTextDocument(doc);
    const pos = new vscode.Position(bookmark.line, 0);
    editor.selection = new vscode.Selection(pos, pos);
    editor.revealRange(new vscode.Range(pos, pos), vscode.TextEditorRevealType.InCenter);
}

function navigateBookmark(direction: number): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor || bookmarks.length === 0) { return; }

    const file = editor.document.uri.fsPath;
    const line = editor.selection.active.line;

    // العلامات في نفس الملف أولاً
    const fileBookmarks = bookmarks.filter(b => b.file === file).sort((a, b) => a.line - b.line);

    if (fileBookmarks.length > 0) {
        let target: Bookmark | undefined;
        if (direction > 0) {
            target = fileBookmarks.find(b => b.line > line) || fileBookmarks[0];
        } else {
            target = [...fileBookmarks].reverse().find(b => b.line < line) || fileBookmarks[fileBookmarks.length - 1];
        }
        if (target) { navigateTo(target); }
    } else if (bookmarks.length > 0) {
        navigateTo(bookmarks[0]);
    }
}

function clearBookmarks(context: vscode.ExtensionContext): void {
    bookmarks = [];
    saveBookmarks(context);
    vscode.window.showInformationMessage('🗑️ تم مسح جميع العلامات المرجعية');
}

function refreshDecorations(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const file = editor.document.uri.fsPath;
    const fileBookmarks = bookmarks.filter(b => b.file === file);

    const decorations: vscode.DecorationOptions[] = fileBookmarks.map(b => {
        const line = Math.min(b.line, editor.document.lineCount - 1);
        return {
            range: new vscode.Range(line, 0, line, 0),
            hoverMessage: new vscode.MarkdownString(`**🔖 ${b.label || 'علامة'}**\n\n[${b.category}] — ${new Date(b.timestamp).toLocaleString('ar')}`),
        };
    });

    editor.setDecorations(bookmarkDecoration, decorations);
}

function showBookmarkPanel(context: vscode.ExtensionContext): void {
    const panel = vscode.window.createWebviewPanel(
        'sadBookmarks', '🔖 العلامات المرجعية', vscode.ViewColumn.Two,
        { enableScripts: true }
    );

    const isDark = vscode.window.activeColorTheme.kind === vscode.ColorThemeKind.Dark;

    const rows = bookmarks.map((b, i) => `
        <tr>
            <td>${b.label || b.text.substring(0, 40)}</td>
            <td>${path.basename(b.file)}:${b.line + 1}</td>
            <td><span class="badge">${b.category}</span></td>
            <td>${new Date(b.timestamp).toLocaleDateString('ar')}</td>
            <td><button onclick="goTo(${i})">🔗</button> <button onclick="remove(${i})">🗑️</button></td>
        </tr>
    `).join('');

    panel.webview.html = `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
    body { font-family: 'Segoe UI', sans-serif; background: ${isDark ? '#1e1e1e' : '#fff'}; color: ${isDark ? '#d4d4d4' : '#333'}; padding: 20px; }
    table { width: 100%; border-collapse: collapse; }
    th { background: #4fc3f7; color: #fff; padding: 10px; }
    td { padding: 8px; border-bottom: 1px solid ${isDark ? '#333' : '#ddd'}; }
    .badge { background: #4fc3f7; color: #fff; padding: 2px 8px; border-radius: 10px; font-size: 12px; }
    button { background: none; border: none; cursor: pointer; font-size: 16px; }
</style></head>
<body>
    <h1>🔖 العلامات المرجعية (${bookmarks.length})</h1>
    <table><tr><th>الوصف</th><th>الموقع</th><th>التصنيف</th><th>التاريخ</th><th>إجراء</th></tr>${rows}</table>
    <script>
        const vscode = acquireVsCodeApi();
        function goTo(i) { vscode.postMessage({type:'goto', index:i}); }
        function remove(i) { vscode.postMessage({type:'remove', index:i}); }
    </script>
</body></html>`;

    panel.webview.onDidReceiveMessage(msg => {
        if (msg.type === 'goto' && bookmarks[msg.index]) {
            navigateTo(bookmarks[msg.index]);
        } else if (msg.type === 'remove') {
            bookmarks.splice(msg.index, 1);
            saveBookmarks(context);
            showBookmarkPanel(context); // refresh
        }
    }, undefined, context.subscriptions);
}
