/**
 * @file code_timeline.ts
 * @brief الخط الزمني للكود — Code Timeline
 * 
 * عرض تاريخ التغييرات على الملف مع مخطط زمني تفاعلي
 */

import * as vscode from 'vscode';
import { execFile } from 'child_process';

interface TimelineEntry {
    hash: string;
    author: string;
    date: string;
    message: string;
    linesChanged: number;
}

export function registerCodeTimeline(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.showTimeline', () => showTimeline(context)),
        vscode.commands.registerCommand('sad.fileHistory', () => showFileHistory()),
        vscode.commands.registerCommand('sad.lineHistory', () => showLineHistory()),
    );
}

async function showTimeline(context: vscode.ExtensionContext): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) {
        vscode.window.showWarningMessage('افتح ملف .ص أولاً');
        return;
    }

    const entries = await getGitLog(editor.document.uri.fsPath);
    if (entries.length === 0) {
        vscode.window.showInformationMessage('لا يوجد تاريخ Git لهذا الملف');
        return;
    }

    const panel = vscode.window.createWebviewPanel(
        'sadTimeline', `⏳ الخط الزمني — ${vscode.workspace.asRelativePath(editor.document.uri)}`,
        vscode.ViewColumn.Two, { enableScripts: true }
    );

    const isDark = vscode.window.activeColorTheme.kind === vscode.ColorThemeKind.Dark;
    panel.webview.html = getTimelineHTML(entries, isDark);
}

function getGitLog(filePath: string): Promise<TimelineEntry[]> {
    return new Promise((resolve) => {
        const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
        if (!workspaceFolder) { resolve([]); return; }

        execFile('git', [
            'log', '--pretty=format:%H|%an|%ai|%s', '--numstat', '-30', '--', filePath
        ], { cwd: workspaceFolder }, (err, stdout) => {
            if (err) { resolve([]); return; }

            const entries: TimelineEntry[] = [];
            const lines = stdout.split('\n');
            let current: TimelineEntry | null = null;

            for (const line of lines) {
                if (line.includes('|')) {
                    const parts = line.split('|');
                    if (parts.length >= 4) {
                        current = {
                            hash: parts[0].substring(0, 8),
                            author: parts[1],
                            date: parts[2],
                            message: parts.slice(3).join('|'),
                            linesChanged: 0,
                        };
                        entries.push(current);
                    }
                } else if (current && line.trim()) {
                    const numMatch = line.match(/^(\d+)\s+(\d+)/);
                    if (numMatch) {
                        current.linesChanged += parseInt(numMatch[1]) + parseInt(numMatch[2]);
                    }
                }
            }

            resolve(entries);
        });
    });
}

function getTimelineHTML(entries: TimelineEntry[], isDark: boolean): string {
    const bg = isDark ? '#1e1e1e' : '#fff';
    const fg = isDark ? '#d4d4d4' : '#333';
    const cardBg = isDark ? '#252526' : '#f5f5f5';
    const accent = '#4fc3f7';

    const maxLines = Math.max(...entries.map(e => e.linesChanged), 1);

    const items = entries.map((e, i) => {
        const barWidth = Math.max(5, (e.linesChanged / maxLines) * 100);
        const date = new Date(e.date);
        const dateStr = date.toLocaleDateString('ar', { year: 'numeric', month: 'short', day: 'numeric' });
        const timeStr = date.toLocaleTimeString('ar', { hour: '2-digit', minute: '2-digit' });
        const side = i % 2 === 0 ? 'right' : 'left';

        return `
        <div class="entry ${side}" style="animation-delay:${i * 0.1}s">
            <div class="dot"></div>
            <div class="card">
                <div class="hash">${e.hash}</div>
                <div class="message">${escapeHtml(e.message)}</div>
                <div class="meta">
                    <span>👤 ${escapeHtml(e.author)}</span>
                    <span>📅 ${dateStr} ${timeStr}</span>
                </div>
                <div class="bar-container">
                    <div class="bar" style="width:${barWidth}%"></div>
                    <span class="bar-label">${e.linesChanged} سطر</span>
                </div>
            </div>
        </div>`;
    }).join('');

    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
    body { font-family: 'Segoe UI', sans-serif; background: ${bg}; color: ${fg}; padding: 20px; }
    h1 { text-align: center; color: ${accent}; }
    .timeline { position: relative; padding: 20px 0; }
    .timeline::before { content: ''; position: absolute; right: 50%; width: 3px; height: 100%; background: ${accent}; }
    .entry { position: relative; width: 45%; margin-bottom: 20px; opacity: 0; animation: fadeIn 0.5s forwards; }
    .entry.right { margin-right: 5%; }
    .entry.left { margin-right: 50%; }
    .dot { position: absolute; width: 16px; height: 16px; background: ${accent}; border-radius: 50%; top: 20px; }
    .entry.right .dot { left: -28px; }
    .entry.left .dot { right: -28px; }
    .card { background: ${cardBg}; border-radius: 10px; padding: 16px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); }
    .hash { font-family: monospace; font-size: 12px; color: ${accent}; margin-bottom: 4px; }
    .message { font-weight: bold; margin-bottom: 8px; }
    .meta { display: flex; gap: 16px; font-size: 12px; opacity: 0.7; margin-bottom: 8px; flex-wrap: wrap; }
    .bar-container { display: flex; align-items: center; gap: 8px; }
    .bar { height: 8px; background: linear-gradient(90deg, #4caf50, #ff9800); border-radius: 4px; min-width: 5px; }
    .bar-label { font-size: 11px; opacity: 0.6; }
    @keyframes fadeIn { to { opacity: 1; } }
    .stats { text-align: center; margin: 20px; padding: 16px; background: ${cardBg}; border-radius: 10px; }
</style></head>
<body>
    <h1>⏳ الخط الزمني للكود</h1>
    <div class="stats">
        📊 ${entries.length} تعديل | 
        👥 ${new Set(entries.map(e => e.author)).size} مساهم | 
        📝 ${entries.reduce((s, e) => s + e.linesChanged, 0)} سطر تم تعديله
    </div>
    <div class="timeline">${items}</div>
</body></html>`;
}

async function showFileHistory(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const entries = await getGitLog(editor.document.uri.fsPath);
    if (entries.length === 0) {
        vscode.window.showInformationMessage('لا يوجد تاريخ Git');
        return;
    }

    const items = entries.map(e => ({
        label: `$(git-commit) ${e.hash}`,
        description: e.message,
        detail: `${e.author} — ${new Date(e.date).toLocaleDateString('ar')} (${e.linesChanged} سطر)`,
        hash: e.hash,
    }));

    const selected = await vscode.window.showQuickPick(items, {
        placeHolder: 'اختر تعديلاً لعرض التفاصيل'
    });

    if (selected) {
        vscode.window.showInformationMessage(`التعديل ${selected.hash}: ${selected.description}`);
    }
}

async function showLineHistory(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const line = editor.selection.active.line + 1;
    const filePath = editor.document.uri.fsPath;
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
    if (!workspaceFolder) { return; }

    const result = await new Promise<string>((resolve) => {
        execFile('git', [
            'log', `--pretty=format:%h | %an | %s`, `-L${line},${line}:${vscode.workspace.asRelativePath(editor.document.uri)}`,
            '-10'
        ], { cwd: workspaceFolder }, (err, stdout) => {
            resolve(err ? '' : stdout);
        });
    });

    if (result) {
        const channel = vscode.window.createOutputChannel('تاريخ السطر');
        channel.appendLine(`تاريخ السطر ${line}:`);
        channel.appendLine(result);
        channel.show();
    } else {
        vscode.window.showInformationMessage(`لا يوجد تاريخ Git للسطر ${line}`);
    }
}

function escapeHtml(text: string): string {
    return text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
}
