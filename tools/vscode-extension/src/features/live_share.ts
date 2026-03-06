/**
 * @file live_share.ts
 * @brief مشاركة الكود الحي — Live Code Sharing
 * 
 * مشاركة مقاطع الكود عبر خادم بسيط مع QR وروابط مباشرة
 */

import * as vscode from 'vscode';
import * as path from 'path';
import * as http from 'http';

let shareServer: http.Server | undefined;
let sharedContent: { code: string; fileName: string; language: string } | undefined;

export function registerLiveShare(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.shareCode', () => shareCode()),
        vscode.commands.registerCommand('sad.shareSelection', () => shareSelection()),
        vscode.commands.registerCommand('sad.stopSharing', () => stopSharing()),
        vscode.commands.registerCommand('sad.copyCodeAsImage', () => copyCodeAsImage()),
        vscode.commands.registerCommand('sad.exportCodeSnippet', () => exportCodeSnippet()),
    );
}

async function shareCode(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const code = editor.document.getText();
    const fileName = path.basename(editor.document.uri.fsPath);

    await startShareServer(code, fileName);
}

async function shareSelection(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const code = editor.document.getText(editor.selection);
    if (!code.trim()) {
        vscode.window.showWarningMessage('اختر نصاً لمشاركته');
        return;
    }

    const fileName = path.basename(editor.document.uri.fsPath);
    await startShareServer(code, fileName + ' (مقطع)');
}

async function startShareServer(code: string, fileName: string): Promise<void> {
    if (shareServer) {
        shareServer.close();
    }

    sharedContent = { code, fileName, language: 'sad' };

    const port = 9876 + Math.floor(Math.random() * 100);

    shareServer = http.createServer((req, res) => {
        if (!sharedContent) {
            res.writeHead(404);
            res.end('Not found');
            return;
        }

        if (req.url === '/raw') {
            res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
            res.end(sharedContent.code);
            return;
        }

        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end(getShareHTML(sharedContent));
    });

    return new Promise((resolve, reject) => {
        shareServer!.listen(port, () => {
            const url = `http://localhost:${port}`;
            const rawUrl = `${url}/raw`;

            vscode.window.showInformationMessage(
                `🔗 الكود متاح على: ${url}`,
                'نسخ الرابط', 'فتح في المتصفح', 'إيقاف'
            ).then(choice => {
                if (choice === 'نسخ الرابط') {
                    vscode.env.clipboard.writeText(url);
                    vscode.window.showInformationMessage('✅ تم نسخ الرابط');
                } else if (choice === 'فتح في المتصفح') {
                    vscode.env.openExternal(vscode.Uri.parse(url));
                } else if (choice === 'إيقاف') {
                    stopSharing();
                }
            });

            resolve();
        });

        shareServer!.on('error', (err) => {
            vscode.window.showErrorMessage(`خطأ في بدء خادم المشاركة: ${err.message}`);
            reject(err);
        });
    });
}

function stopSharing(): void {
    if (shareServer) {
        shareServer.close();
        shareServer = undefined;
        sharedContent = undefined;
        vscode.window.showInformationMessage('⏹️ تم إيقاف المشاركة');
    }
}

function getShareHTML(content: { code: string; fileName: string }): string {
    const escaped = content.code
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;');

    // تلوين بسيط
    const highlighted = escaped
        .replace(/(دالة|صنف|إذا|وإلا|بينما|لكل|ارجع|متغير|ثابت|نهاية|جديد|هذا|باني|طابق|عندما|افتراضي|حاول|امسك|ارمي|أخيراً|عام|خاص|محمي|مجرد|استورد|من|كـ|صدّر|في|توقف|استمر|ساكن|خارجي|صحيح|خطأ|لاشيء|بنية|تعداد|يرث)\b/g, '<span style="color:#c586c0">$1</span>')
        .replace(/(اطبع|اطبع_سطر|طول|نوع)\b/g, '<span style="color:#dcdcaa">$1</span>')
        .replace(/#[^\n]*/g, '<span style="color:#6a9955">$&</span>')
        .replace(/"[^"]*"/g, '<span style="color:#ce9178">$&</span>')
        .replace(/\b(\d+\.?\d*)\b/g, '<span style="color:#b5cea8">$1</span>');

    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8"><title>${content.fileName} — لغة ص</title>
<style>
    body { font-family: 'Segoe UI', sans-serif; background: #1e1e1e; color: #d4d4d4; margin: 0; padding: 0; }
    .header { background: #2d2d30; padding: 16px 24px; display: flex; justify-content: space-between; align-items: center; }
    .header h1 { margin: 0; font-size: 18px; color: #4fc3f7; }
    .actions { display: flex; gap: 8px; }
    .btn { background: #4fc3f7; color: #1e1e1e; border: none; padding: 8px 16px; border-radius: 6px; cursor: pointer; font-weight: bold; }
    .btn:hover { background: #81d4fa; }
    pre { margin: 0; padding: 24px; font-family: 'Cascadia Code', 'Fira Code', monospace; font-size: 14px; line-height: 1.6; direction: rtl; unicode-bidi: plaintext; overflow-x: auto; }
    .footer { background: #2d2d30; padding: 12px 24px; text-align: center; font-size: 12px; opacity: 0.6; }
    .line-num { color: #858585; display: inline-block; min-width: 40px; text-align: left; margin-left: 16px; user-select: none; }
</style></head>
<body>
    <div class="header">
        <h1>📄 ${content.fileName}</h1>
        <div class="actions">
            <button class="btn" onclick="copyCode()">📋 نسخ</button>
            <button class="btn" onclick="downloadCode()">⬇️ تحميل</button>
        </div>
    </div>
    <pre>${highlighted.split('\n').map((l, i) => `<span class="line-num">${i + 1}</span>${l}`).join('\n')}</pre>
    <div class="footer">مشاركة عبر إضافة لغة ص — Sad Language VS Code Extension</div>
    <script>
        function copyCode() {
            fetch('/raw').then(r => r.text()).then(t => {
                navigator.clipboard.writeText(t);
                alert('✅ تم نسخ الكود');
            });
        }
        function downloadCode() {
            fetch('/raw').then(r => r.text()).then(t => {
                const a = document.createElement('a');
                a.href = URL.createObjectURL(new Blob([t], {type:'text/plain'}));
                a.download = '${content.fileName}';
                a.click();
            });
        }
    </script>
</body></html>`;
}

async function copyCodeAsImage(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const code = editor.selection.isEmpty ? editor.document.getText() : editor.document.getText(editor.selection);
    const fileName = path.basename(editor.document.uri.fsPath);

    // إنشاء SVG من الكود
    const lines = code.split('\n');
    const lineHeight = 20;
    const padding = 30;
    const width = 700;
    const height = lines.length * lineHeight + padding * 2 + 40;

    const escapedLines = lines.map((l, i) => {
        const escaped = l.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
        const y = padding + 30 + i * lineHeight;
        return `<text x="${width - padding}" y="${y}" text-anchor="end" fill="#d4d4d4" font-family="monospace" font-size="13">${escaped}</text>`;
    }).join('\n');

    const svg = `<svg xmlns="http://www.w3.org/2000/svg" width="${width}" height="${height}">
        <rect width="100%" height="100%" fill="#1e1e1e" rx="10"/>
        <text x="${width - padding}" y="22" text-anchor="end" fill="#4fc3f7" font-family="sans-serif" font-size="12">${fileName}</text>
        ${escapedLines}
    </svg>`;

    await vscode.env.clipboard.writeText(svg);
    vscode.window.showInformationMessage('✅ تم نسخ الكود كـ SVG — الصقه في أي محرر رسومي');
}

async function exportCodeSnippet(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const code = editor.selection.isEmpty ? editor.document.getText() : editor.document.getText(editor.selection);
    const fileName = path.basename(editor.document.uri.fsPath, '.ص');

    const formats = [
        { label: 'Markdown', ext: 'md', wrap: (c: string) => `\`\`\`sad\n${c}\n\`\`\`` },
        { label: 'HTML', ext: 'html', wrap: (c: string) => `<pre><code class="language-sad">${c.replace(/</g, '&lt;')}</code></pre>` },
        { label: 'نص عادي', ext: 'txt', wrap: (c: string) => c },
    ];

    const selected = await vscode.window.showQuickPick(
        formats.map(f => ({ label: f.label, format: f })),
        { placeHolder: 'اختر صيغة التصدير' }
    );

    if (!selected) { return; }

    const content = selected.format.wrap(code);
    await vscode.env.clipboard.writeText(content);
    vscode.window.showInformationMessage(`✅ تم نسخ الكود بصيغة ${selected.label}`);
}
