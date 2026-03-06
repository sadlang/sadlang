/**
 * @file project_dashboard.ts
 * @brief لوحة قيادة المشروع — Project Dashboard
 * 
 * لوحة تفاعلية تعرض إحصائيات المشروع وصحته
 */

import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

export function registerProjectDashboard(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.showDashboard', () => {
            DashboardPanel.createOrShow(context.extensionUri);
        })
    );
}

class DashboardPanel {
    public static currentPanel: DashboardPanel | undefined;
    private readonly panel: vscode.WebviewPanel;
    private disposables: vscode.Disposable[] = [];

    static createOrShow(extensionUri: vscode.Uri): void {
        if (DashboardPanel.currentPanel) {
            DashboardPanel.currentPanel.panel.reveal(vscode.ViewColumn.One);
            DashboardPanel.currentPanel.update();
            return;
        }
        const panel = vscode.window.createWebviewPanel(
            'sadDashboard', 'لوحة القيادة — Dashboard', vscode.ViewColumn.One,
            { enableScripts: true }
        );
        DashboardPanel.currentPanel = new DashboardPanel(panel);
    }

    private constructor(panel: vscode.WebviewPanel) {
        this.panel = panel;
        this.update();
        this.panel.onDidDispose(() => {
            DashboardPanel.currentPanel = undefined;
            this.disposables.forEach(d => d.dispose());
        }, null, this.disposables);
    }

    private async update(): Promise<void> {
        const stats = await this.collectStats();
        this.panel.webview.html = this.getHTML(stats);
    }

    private async collectStats(): Promise<ProjectStats> {
        const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
        if (!workspaceFolder) {
            return { files: 0, lines: 0, functions: 0, classes: 0, imports: 0, comments: 0, todos: 0, errors: 0, fileList: [] };
        }

        const files = await vscode.workspace.findFiles('**/*.ص', '**/node_modules/**');
        let totalLines = 0, totalFunctions = 0, totalClasses = 0, totalImports = 0;
        let totalComments = 0, totalTodos = 0;
        const fileList: FileInfo[] = [];

        for (const file of files) {
            try {
                const content = (await vscode.workspace.fs.readFile(file)).toString();
                const lines = content.split('\n');
                const funcs = (content.match(/دالة\s+\S+/g) || []).length;
                const classes = (content.match(/صنف\s+\S+/g) || []).length;
                const imports = (content.match(/استورد\s+/g) || []).length;
                const comments = lines.filter(l => l.trim().startsWith('#')).length;
                const todos = (content.match(/#.*TODO|#.*مهمة/gi) || []).length;

                totalLines += lines.length;
                totalFunctions += funcs;
                totalClasses += classes;
                totalImports += imports;
                totalComments += comments;
                totalTodos += todos;

                const relativePath = vscode.workspace.asRelativePath(file);
                fileList.push({ name: relativePath, lines: lines.length, functions: funcs, classes });
            } catch { /* skip unreadable files */ }
        }

        // جمع الأخطاء من التشخيصات
        const allDiags = vscode.languages.getDiagnostics();
        let errors = 0;
        for (const [uri, diags] of allDiags) {
            if (uri.fsPath.endsWith('.ص')) {
                errors += diags.filter(d => d.severity === vscode.DiagnosticSeverity.Error).length;
            }
        }

        fileList.sort((a, b) => b.lines - a.lines);

        return { files: files.length, lines: totalLines, functions: totalFunctions, classes: totalClasses, imports: totalImports, comments: totalComments, todos: totalTodos, errors, fileList: fileList.slice(0, 20) };
    }

    private getHTML(stats: ProjectStats): string {
        const isDark = vscode.window.activeColorTheme.kind === vscode.ColorThemeKind.Dark || vscode.window.activeColorTheme.kind === vscode.ColorThemeKind.HighContrast;
        const bg = isDark ? '#1e1e1e' : '#ffffff';
        const fg = isDark ? '#d4d4d4' : '#333333';
        const cardBg = isDark ? '#252526' : '#f3f3f3';
        const accent = '#4fc3f7';
        const green = '#4caf50';
        const red = '#f44336';
        const orange = '#ff9800';

        const commentRatio = stats.lines > 0 ? ((stats.comments / stats.lines) * 100).toFixed(1) : '0';
        const healthScore = this.calcHealth(stats);
        const healthColor = healthScore >= 80 ? green : healthScore >= 50 ? orange : red;

        const fileRows = stats.fileList.map(f => `
            <tr>
                <td style="text-align:right;padding:6px 12px">${this.escapeHtml(f.name)}</td>
                <td style="text-align:center;padding:6px 12px">${f.lines}</td>
                <td style="text-align:center;padding:6px 12px">${f.functions}</td>
                <td style="text-align:center;padding:6px 12px">${f.classes}</td>
            </tr>
        `).join('');

        return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <style>
        body { font-family: 'Segoe UI', Tahoma, sans-serif; background: ${bg}; color: ${fg}; margin: 0; padding: 20px; direction: rtl; }
        h1 { text-align: center; color: ${accent}; margin-bottom: 30px; }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 16px; margin-bottom: 30px; }
        .card { background: ${cardBg}; border-radius: 12px; padding: 20px; text-align: center; box-shadow: 0 2px 8px rgba(0,0,0,0.15); }
        .card .number { font-size: 36px; font-weight: bold; margin: 10px 0; }
        .card .label { font-size: 14px; opacity: 0.7; }
        .health { text-align: center; margin: 20px 0; }
        .health-bar { width: 80%; max-width: 400px; height: 24px; background: #333; border-radius: 12px; margin: 10px auto; overflow: hidden; }
        .health-fill { height: 100%; border-radius: 12px; transition: width 0.5s; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th { background: ${accent}; color: #fff; padding: 10px 12px; text-align: center; }
        tr:nth-child(even) { background: ${cardBg}; }
        td { border-bottom: 1px solid ${isDark ? '#333' : '#ddd'}; }
    </style>
</head>
<body>
    <h1>📊 لوحة قيادة المشروع</h1>
    
    <div class="health">
        <h2>صحة المشروع: <span style="color:${healthColor}">${healthScore}%</span></h2>
        <div class="health-bar">
            <div class="health-fill" style="width:${healthScore}%;background:${healthColor}"></div>
        </div>
    </div>

    <div class="grid">
        <div class="card">
            <div class="label">📄 الملفات</div>
            <div class="number" style="color:${accent}">${stats.files}</div>
        </div>
        <div class="card">
            <div class="label">📝 الأسطر</div>
            <div class="number" style="color:${accent}">${stats.lines.toLocaleString('ar')}</div>
        </div>
        <div class="card">
            <div class="label">⚡ الدوال</div>
            <div class="number" style="color:${green}">${stats.functions}</div>
        </div>
        <div class="card">
            <div class="label">🏗️ الأصناف</div>
            <div class="number" style="color:${green}">${stats.classes}</div>
        </div>
        <div class="card">
            <div class="label">📦 الاستيرادات</div>
            <div class="number">${stats.imports}</div>
        </div>
        <div class="card">
            <div class="label">💬 التعليقات (${commentRatio}%)</div>
            <div class="number">${stats.comments}</div>
        </div>
        <div class="card">
            <div class="label">📌 المهام (TODO)</div>
            <div class="number" style="color:${orange}">${stats.todos}</div>
        </div>
        <div class="card">
            <div class="label">❌ الأخطاء</div>
            <div class="number" style="color:${red}">${stats.errors}</div>
        </div>
    </div>

    <h2>📂 أكبر الملفات</h2>
    <table>
        <tr><th>الملف</th><th>الأسطر</th><th>الدوال</th><th>الأصناف</th></tr>
        ${fileRows}
    </table>
</body>
</html>`;
    }

    private calcHealth(stats: ProjectStats): number {
        let score = 100;
        if (stats.errors > 0) { score -= Math.min(30, stats.errors * 5); }
        if (stats.lines > 0) {
            const commentRatio = stats.comments / stats.lines;
            if (commentRatio < 0.05) { score -= 15; }
            else if (commentRatio < 0.1) { score -= 5; }
        }
        if (stats.todos > 10) { score -= 10; }
        else if (stats.todos > 5) { score -= 5; }
        return Math.max(0, Math.min(100, score));
    }

    private escapeHtml(text: string): string {
        return text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    }
}

interface ProjectStats {
    files: number;
    lines: number;
    functions: number;
    classes: number;
    imports: number;
    comments: number;
    todos: number;
    errors: number;
    fileList: FileInfo[];
}

interface FileInfo {
    name: string;
    lines: number;
    functions: number;
    classes: number;
}
