/**
 * @file database_explorer.ts
 * @brief مستكشف قواعد البيانات — Database Explorer
 * 
 * عرض وتحرير بيانات SQLite المرتبطة بمشاريع لغة ص
 */

import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';

interface TableInfo {
    name: string;
    columns: string[];
    rowCount: number;
}

export function registerDatabaseExplorer(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.openDatabase', () => openDatabase(context)),
        vscode.commands.registerCommand('sad.newDatabase', () => createNewDatabase()),
        vscode.commands.registerCommand('sad.insertDbSnippet', () => insertDbSnippet()),
    );
}

async function openDatabase(context: vscode.ExtensionContext): Promise<void> {
    // البحث عن ملفات .db في المشروع
    const dbFiles = await vscode.workspace.findFiles('**/*.{db,sqlite,sqlite3}', '**/node_modules/**');

    if (dbFiles.length === 0) {
        const create = await vscode.window.showInformationMessage(
            'لم يتم العثور على قواعد بيانات. هل تريد إنشاء قاعدة جديدة؟',
            'نعم', 'لا'
        );
        if (create === 'نعم') {
            await createNewDatabase();
        }
        return;
    }

    const items = dbFiles.map(f => ({
        label: `$(database) ${path.basename(f.fsPath)}`,
        description: vscode.workspace.asRelativePath(f),
        uri: f,
    }));

    const selected = await vscode.window.showQuickPick(items, {
        placeHolder: 'اختر قاعدة بيانات لفتحها'
    });

    if (selected) {
        showDatabasePanel(selected.uri, context);
    }
}

function showDatabasePanel(dbUri: vscode.Uri, context: vscode.ExtensionContext): void {
    const panel = vscode.window.createWebviewPanel(
        'sadDatabase', `🗄️ ${path.basename(dbUri.fsPath)}`,
        vscode.ViewColumn.One, { enableScripts: true }
    );

    const isDark = vscode.window.activeColorTheme.kind === vscode.ColorThemeKind.Dark;

    // عرض واجهة إدارة قاعدة البيانات
    panel.webview.html = getDatabaseHTML(dbUri.fsPath, isDark);

    panel.webview.onDidReceiveMessage(async msg => {
        if (msg.type === 'query') {
            vscode.window.showInformationMessage(`🔍 الاستعلام: ${msg.sql}`);
        } else if (msg.type === 'insertSnippet') {
            const editor = vscode.window.activeTextEditor;
            if (editor) {
                editor.insertSnippet(new vscode.SnippetString(msg.code));
            }
        }
    }, undefined, context.subscriptions);
}

function getDatabaseHTML(dbPath: string, isDark: boolean): string {
    const bg = isDark ? '#1e1e1e' : '#fff';
    const fg = isDark ? '#d4d4d4' : '#333';
    const cardBg = isDark ? '#252526' : '#f5f5f5';
    const accent = '#4fc3f7';
    const dbName = path.basename(dbPath);

    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
    body { font-family: 'Segoe UI', sans-serif; background: ${bg}; color: ${fg}; padding: 20px; direction: rtl; }
    h1 { color: ${accent}; }
    .toolbar { display: flex; gap: 8px; margin: 16px 0; flex-wrap: wrap; }
    .btn { background: ${accent}; color: #1e1e1e; border: none; padding: 8px 16px; border-radius: 6px; cursor: pointer; font-weight: bold; font-size: 13px; }
    .btn:hover { opacity: 0.9; }
    .btn-secondary { background: ${cardBg}; color: ${fg}; border: 1px solid ${isDark ? '#555' : '#ccc'}; }
    textarea { width: 100%; height: 100px; background: ${cardBg}; color: ${fg}; border: 1px solid ${isDark ? '#555' : '#ccc'}; border-radius: 6px; padding: 12px; font-family: 'Cascadia Code', monospace; font-size: 13px; resize: vertical; direction: ltr; }
    .status { padding: 8px; background: ${cardBg}; border-radius: 6px; margin: 12px 0; font-size: 12px; }
    .snippet-list { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 12px; margin-top: 16px; }
    .snippet-card { background: ${cardBg}; border-radius: 8px; padding: 16px; cursor: pointer; border: 1px solid transparent; transition: border-color 0.2s; }
    .snippet-card:hover { border-color: ${accent}; }
    .snippet-card h3 { margin: 0 0 8px; color: ${accent}; font-size: 14px; }
    .snippet-card code { font-size: 11px; opacity: 0.8; }
</style></head>
<body>
    <h1>🗄️ قاعدة البيانات: ${dbName}</h1>
    
    <div class="status">
        📂 المسار: ${dbPath}<br>
        💡 يمكنك استخدام مكتبة قاعدة البيانات في لغة ص للتعامل مع هذا الملف
    </div>

    <h2>📝 استعلام SQL</h2>
    <textarea id="sqlInput" placeholder="اكتب استعلام SQL هنا...&#10;مثال: SELECT * FROM الجدول"></textarea>
    <div class="toolbar">
        <button class="btn" onclick="runQuery()">▶ تنفيذ</button>
        <button class="btn btn-secondary" onclick="clearQuery()">🗑️ مسح</button>
    </div>

    <h2>📋 قوالب كود لغة ص</h2>
    <p>انقر على قالب لإدراجه في المحرر:</p>
    <div class="snippet-list">
        <div class="snippet-card" onclick="insertSnippet('connect')">
            <h3>🔌 اتصال بقاعدة البيانات</h3>
            <code>استورد "قاعدة_بيانات" من "مكتبة_قياسية"</code>
        </div>
        <div class="snippet-card" onclick="insertSnippet('create')">
            <h3>🏗️ إنشاء جدول</h3>
            <code>قب.نفّذ("CREATE TABLE...")</code>
        </div>
        <div class="snippet-card" onclick="insertSnippet('insert')">
            <h3>➕ إدراج بيانات</h3>
            <code>قب.نفّذ("INSERT INTO...")</code>
        </div>
        <div class="snippet-card" onclick="insertSnippet('select')">
            <h3>🔍 استعلام بيانات</h3>
            <code>متغير نتائج = قب.استعلم(...)</code>
        </div>
        <div class="snippet-card" onclick="insertSnippet('update')">
            <h3>✏️ تحديث بيانات</h3>
            <code>قب.نفّذ("UPDATE...")</code>
        </div>
        <div class="snippet-card" onclick="insertSnippet('delete')">
            <h3>🗑️ حذف بيانات</h3>
            <code>قب.نفّذ("DELETE FROM...")</code>
        </div>
    </div>

    <script>
        const vscode = acquireVsCodeApi();
        
        const snippets = {
            connect: 'استورد "قاعدة_بيانات" من "مكتبة_قياسية"\\n\\nمتغير قب = جديد قاعدة_بيانات("${dbName}")\\nقب.اتصل()\\n\\n# استخدم قاعدة البيانات هنا\\n\\nقب.أغلق()',
            create: 'قب.نفّذ("CREATE TABLE IF NOT EXISTS الجدول (\\n\\tالمعرف INTEGER PRIMARY KEY,\\n\\tالاسم TEXT NOT NULL,\\n\\tالقيمة REAL\\n)")',
            insert: 'قب.نفّذ("INSERT INTO الجدول (الاسم، القيمة) VALUES (?, ?)"، ["أحمد"، 42])',
            select: 'متغير نتائج = قب.استعلم("SELECT * FROM الجدول WHERE الاسم = ?"، ["أحمد"])\\nلكل صف في نتائج\\n\\تاطبع_سطر(صف.الاسم + ": " + صف.القيمة)\\nنهاية',
            update: 'قب.نفّذ("UPDATE الجدول SET القيمة = ? WHERE الاسم = ?"، [100، "أحمد"])',
            delete: 'قب.نفّذ("DELETE FROM الجدول WHERE الاسم = ?"، ["أحمد"])',
        };

        function runQuery() {
            const sql = document.getElementById('sqlInput').value;
            if (sql.trim()) {
                vscode.postMessage({ type: 'query', sql });
            }
        }

        function clearQuery() {
            document.getElementById('sqlInput').value = '';
        }

        function insertSnippet(type) {
            vscode.postMessage({ type: 'insertSnippet', code: snippets[type] });
        }
    </script>
</body></html>`;
}

async function createNewDatabase(): Promise<void> {
    const name = await vscode.window.showInputBox({
        prompt: 'اسم قاعدة البيانات الجديدة',
        placeHolder: 'قاعدتي.db',
        validateInput: (v) => v.trim() ? null : 'أدخل اسماً صالحاً',
    });

    if (!name) { return; }

    const workspaceFolder = vscode.workspace.workspaceFolders?.[0];
    if (!workspaceFolder) { return; }

    const dbName = name.endsWith('.db') ? name : name + '.db';
    const dbPath = path.join(workspaceFolder.uri.fsPath, dbName);

    // إنشاء ملف فارغ
    fs.writeFileSync(dbPath, '');

    vscode.window.showInformationMessage(`✅ تم إنشاء قاعدة البيانات: ${dbName}`);
}

async function insertDbSnippet(): Promise<void> {
    const snippets = [
        { label: '🔌 اتصال', code: 'استورد "قاعدة_بيانات" من "مكتبة_قياسية"\n\nمتغير قب = جديد قاعدة_بيانات("${1:الملف.db}")\nقب.اتصل()\n\n${2:// الكود}\n\nقب.أغلق()' },
        { label: '🏗️ إنشاء جدول', code: 'قب.نفّذ("CREATE TABLE IF NOT EXISTS ${1:الجدول} (\n\tالمعرف INTEGER PRIMARY KEY,\n\t${2:العمود} ${3:TEXT}\n)")' },
        { label: '🔍 استعلام', code: 'متغير ${1:نتائج} = قب.استعلم("SELECT * FROM ${2:الجدول}")\nلكل ${3:صف} في ${1:نتائج}\n\تاطبع_سطر(${3:صف})\nنهاية' },
    ];

    const selected = await vscode.window.showQuickPick(
        snippets.map(s => ({ label: s.label, snippet: s })),
        { placeHolder: 'اختر قالب قاعدة بيانات' }
    );

    if (selected) {
        const editor = vscode.window.activeTextEditor;
        if (editor) {
            editor.insertSnippet(new vscode.SnippetString(selected.snippet.code));
        }
    }
}
