/**
 * @file debugger.ts
 * @brief مصحح الأخطاء المرئي العربي — Visual Arabic Debugger
 * 
 * هذا الملف يوفر مصحح أخطاء مبسط لملفات لغة ص عبر VS Code.
 * يعمل عن طريق تشغيل المفسر (sad.exe) مع وسيطات خاصة
 * والتقاط المخرجات وعرضها في WebView.
 * 
 * === كيف يعمل ===
 * 
 * بما أن لغة ص مفسّرة، نستخدم نهجاً مبسطاً:
 *   1. عند تشغيل التصحيح، نشغّل المفسر مع وسيط --debug
 *   2. المفسر يطبع معلومات حالة كل سطر
 *   3. نعرض المعلومات في WebView مع تحديد السطر الحالي
 * 
 * في غياب دعم --debug في المفسر حالياً، نوفر:
 *   - تشغيل الملف والتقاط المخرجات
 *   - عرض نقاط التوقف (Breakpoints) بصرياً
 *   - عرض قيم المتغيرات المحلية (عبر تحليل الكود)
 *   - Call Stack عربي (عبر تحليل تتبع الأخطاء)
 * 
 * === البنية ===
 * 
 * DebugPanel (WebView) ← يتواصل مع → DebugSession (Extension)
 *                                           ↕
 *                                     sad.exe (Terminal)
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { exec } from 'child_process';

/**
 * @brief معلومات نقطة التوقف
 */
interface BreakpointInfo {
    /** رقم السطر (1-based) */
    line: number;
    /** مسار الملف */
    file: string;
    /** مفعّلة أم معطّلة */
    enabled: boolean;
    /** شرط (اختياري) */
    condition?: string;
}

/**
 * @brief متغير في لوحة المتغيرات
 */
interface VariableInfo {
    /** اسم المتغير */
    name: string;
    /** قيمته (كنص) */
    value: string;
    /** نوعه */
    type: string;
    /** رقم السطر الذي عُرّف فيه */
    definedAt: number;
}

/**
 * @brief إطار في مكدس الاستدعاءات
 */
interface StackFrame {
    /** اسم الدالة */
    functionName: string;
    /** رقم السطر */
    line: number;
    /** اسم الملف */
    file: string;
}

/**
 * @brief حالة جلسة التصحيح
 */
interface DebugState {
    /** هل الجلسة قيد التشغيل؟ */
    running: boolean;
    /** السطر الحالي (1-based) */
    currentLine: number;
    /** المتغيرات المكتشفة */
    variables: VariableInfo[];
    /** مكدس الاستدعاءات */
    callStack: StackFrame[];
    /** المخرجات */
    output: string[];
    /** نقاط التوقف */
    breakpoints: BreakpointInfo[];
    /** الأخطاء */
    errors: string[];
}

/**
 * @brief تحليل الكود لاستخراج المتغيرات المحلية
 * 
 * يمسح الكود من البداية حتى السطر المحدد
 * ويجمع جميع تعريفات المتغيرات والثوابت.
 * 
 * @param code نص الكود الكامل
 * @param upToLine آخر سطر للتحليل (1-based)
 * @returns مصفوفة المتغيرات المكتشفة
 */
function extractVariables(code: string, upToLine: number): VariableInfo[] {
    const lines = code.split('\n');
    const variables: VariableInfo[] = [];
    const varPattern = /^(\s*)(متغير|ثابت)\s+([\u0600-\u06FF\u0750-\u077F\w]+)\s*=\s*(.+)/;
    
    for (let i = 0; i < Math.min(upToLine, lines.length); i++) {
        const match = lines[i].match(varPattern);
        if (match) {
            const isConst = match[2] === 'ثابت';
            const name = match[3];
            let value = match[4].trim();
            
            // تحديد النوع من القيمة
            let type = 'أي';
            if (value.startsWith('"') || value.startsWith("'")) {
                type = 'نص';
            } else if (value === 'صحيح' || value === 'خطأ') {
                type = 'منطقي';
            } else if (value === 'لاشيء') {
                type = 'عدم';
            } else if (/^\d+\.\d+$/.test(value)) {
                type = 'عشري';
            } else if (/^\d+$/.test(value)) {
                type = 'رقم';
            } else if (value.startsWith('[')) {
                type = 'مصفوفة';
            } else if (value.startsWith('{')) {
                type = 'خريطة';
            } else if (value.startsWith('جديد ')) {
                type = 'كائن';
            }
            
            variables.push({
                name,
                value: value.substring(0, 50),
                type: isConst ? `${type} (ثابت)` : type,
                definedAt: i + 1,
            });
        }
    }
    
    return variables;
}

/**
 * @brief بناء مكدس الاستدعاءات من الكود
 * 
 * يحلل الكود لبناء مكدس تقريبي بناءً على
 * الدوال المعرّفة وموقع السطر الحالي.
 * 
 * @param code نص الكود
 * @param currentLine السطر الحالي (1-based)
 * @param fileName اسم الملف
 * @returns مصفوفة إطارات المكدس
 */
function buildCallStack(code: string, currentLine: number, fileName: string): StackFrame[] {
    const lines = code.split('\n');
    const stack: StackFrame[] = [];
    const funcPattern = /^(\s*)دالة\s+([\u0600-\u06FF\u0750-\u077F\w]+)/;
    
    // نبحث عن الدوال التي تحتوي السطر الحالي
    const funcStack: { name: string; startLine: number; indent: number }[] = [];
    
    for (let i = 0; i < lines.length; i++) {
        const match = lines[i].match(funcPattern);
        if (match) {
            const indent = match[1].length;
            funcStack.push({ name: match[2], startLine: i + 1, indent });
        }
        
        if (i + 1 === currentLine) {
            break;
        }
        
        const trimmed = lines[i].trimStart();
        if (trimmed === 'نهاية' && funcStack.length > 0) {
            const lastFunc = funcStack[funcStack.length - 1];
            const indent = lines[i].length - trimmed.length;
            if (indent <= lastFunc.indent) {
                funcStack.pop();
            }
        }
    }
    
    // بناء المكدس (الأحدث أولاً)
    for (let i = funcStack.length - 1; i >= 0; i--) {
        stack.push({
            functionName: funcStack[i].name,
            line: i === funcStack.length - 1 ? currentLine : funcStack[i].startLine,
            file: fileName,
        });
    }
    
    // إضافة الإطار العلوي (البرنامج الرئيسي)
    if (stack.length === 0 || stack[stack.length - 1].functionName !== '<رئيسي>') {
        stack.push({
            functionName: '<رئيسي>',
            line: currentLine,
            file: fileName,
        });
    }
    
    return stack;
}

/**
 * @brief إنشاء HTML لوحة المصحح
 * 
 * تعرض:
 *   - مكدس الاستدعاءات (يمين)
 *   - المتغيرات المحلية (وسط)
 *   - المخرجات (أسفل)
 *   - أزرار التحكم (أعلى)
 */
function getDebugHTML(state: DebugState, code: string): string {
    const lines = code.split('\n');
    
    // بناء HTML للكود مع تحديد السطر الحالي
    let codeHTML = '';
    for (let i = 0; i < lines.length; i++) {
        const lineNum = i + 1;
        const isCurrent = lineNum === state.currentLine;
        const isBreakpoint = state.breakpoints.some(bp => bp.line === lineNum);
        
        const lineClass = isCurrent ? 'current-line' : '';
        const bpClass = isBreakpoint ? 'has-breakpoint' : '';
        const bpMarker = isBreakpoint ? '🔴' : '  ';
        const arrow = isCurrent ? '→' : ' ';
        
        const escaped = lines[i]
            .replace(/&/g, '&amp;')
            .replace(/</g, '&lt;')
            .replace(/>/g, '&gt;');
        
        codeHTML += `<div class="code-line ${lineClass} ${bpClass}" data-line="${lineNum}">
            <span class="bp-marker" onclick="toggleBreakpoint(${lineNum})">${bpMarker}</span>
            <span class="line-num">${lineNum}</span>
            <span class="arrow">${arrow}</span>
            <span class="line-code">${escaped}</span>
        </div>`;
    }
    
    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <title>مصحح لغة ص</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, sans-serif;
            background: var(--vscode-editor-background);
            color: var(--vscode-editor-foreground);
            margin: 0; padding: 0;
            display: grid;
            grid-template-rows: auto 1fr auto;
            grid-template-columns: 1fr 300px;
            height: 100vh;
        }
        
        /* شريط التحكم */
        .controls {
            grid-column: 1 / -1;
            display: flex; gap: 8px;
            padding: 8px 16px;
            background: var(--vscode-titleBar-activeBackground);
            border-bottom: 1px solid var(--vscode-panel-border);
        }
        .controls button {
            padding: 4px 12px;
            background: var(--vscode-button-background);
            color: var(--vscode-button-foreground);
            border: none; border-radius: 4px;
            cursor: pointer; font-size: 13px;
        }
        .controls button:hover { opacity: 0.9; }
        .controls .status {
            margin-right: auto;
            display: flex; align-items: center; gap: 6px;
            font-size: 13px; opacity: 0.7;
        }
        
        /* لوحة الكود */
        .code-panel {
            overflow: auto; padding: 8px 0;
            font-family: 'Consolas', monospace;
            font-size: 14px; line-height: 1.8;
        }
        .code-line {
            display: flex; align-items: center;
            padding: 0 16px; white-space: pre;
        }
        .code-line.current-line {
            background: rgba(255, 213, 79, 0.15);
            border-right: 3px solid #ffd54f;
        }
        .code-line.has-breakpoint .bp-marker { cursor: pointer; }
        .bp-marker { width: 20px; text-align: center; cursor: pointer; font-size: 10px; }
        .line-num { width: 40px; text-align: left; opacity: 0.4; margin-left: 8px; }
        .arrow { width: 20px; color: #ffd54f; font-weight: bold; }
        .line-code { flex: 1; }
        
        /* اللوحة الجانبية */
        .sidebar {
            border-right: 1px solid var(--vscode-panel-border);
            overflow: auto;
            display: flex; flex-direction: column;
        }
        .sidebar-section {
            padding: 8px 12px;
            border-bottom: 1px solid var(--vscode-panel-border);
        }
        .sidebar-section h3 {
            font-size: 12px; text-transform: uppercase;
            opacity: 0.6; margin: 0 0 8px 0;
        }
        .var-item, .stack-item {
            display: flex; justify-content: space-between;
            padding: 3px 0; font-size: 13px;
        }
        .var-name { color: #dcdcaa; }
        .var-value { color: #ce9178; font-family: monospace; }
        .var-type { opacity: 0.5; font-size: 11px; }
        .stack-item { cursor: pointer; padding: 4px 8px; border-radius: 3px; }
        .stack-item:hover { background: var(--vscode-list-hoverBackground); }
        .stack-item.active { background: var(--vscode-list-activeSelectionBackground); }
        
        /* لوحة المخرجات */
        .output-panel {
            grid-column: 1 / -1;
            border-top: 1px solid var(--vscode-panel-border);
            padding: 8px 16px;
            max-height: 200px; overflow: auto;
            font-family: monospace; font-size: 13px;
            background: var(--vscode-terminal-background, #1e1e1e);
        }
        .output-panel h3 {
            font-size: 12px; opacity: 0.6; margin: 0 0 4px 0;
        }
        .output-line { padding: 1px 0; }
        .output-error { color: #f44336; }
    </style>
</head>
<body>
    <div class="controls">
        <button onclick="cmd('run')">▶ تشغيل</button>
        <button onclick="cmd('stop')">⏹ إيقاف</button>
        <button onclick="cmd('step')">⏭ خطوة</button>
        <button onclick="cmd('stepOver')">⤵ تخطي</button>
        <div class="status">
            <span>${state.running ? '🟢 قيد التشغيل' : '⏸ متوقف'}</span>
            ${state.currentLine > 0 ? `<span>— سطر ${state.currentLine}</span>` : ''}
        </div>
    </div>
    
    <div class="code-panel">${codeHTML}</div>
    
    <div class="sidebar">
        <div class="sidebar-section">
            <h3>📝 المتغيرات</h3>
            ${state.variables.map(v => `
                <div class="var-item">
                    <span class="var-name">${v.name}</span>
                    <span class="var-value">${v.value}</span>
                    <span class="var-type">${v.type}</span>
                </div>
            `).join('')}
            ${state.variables.length === 0 ? '<div style="opacity:0.5">لا متغيرات</div>' : ''}
        </div>
        
        <div class="sidebar-section">
            <h3>📚 مكدس الاستدعاءات</h3>
            ${state.callStack.map((f, i) => `
                <div class="stack-item ${i === 0 ? 'active' : ''}" onclick="goToStack(${f.line})">
                    ${f.functionName} — سطر ${f.line}
                </div>
            `).join('')}
        </div>
        
        <div class="sidebar-section">
            <h3>🔴 نقاط التوقف (${state.breakpoints.length})</h3>
            ${state.breakpoints.map(bp => `
                <div class="var-item">
                    <span>سطر ${bp.line}</span>
                    <span style="opacity:0.5">${bp.enabled ? 'مفعّل' : 'معطّل'}</span>
                </div>
            `).join('')}
        </div>
    </div>
    
    <div class="output-panel">
        <h3>📺 المخرجات</h3>
        ${state.output.map(line => `<div class="output-line">${line}</div>`).join('')}
        ${state.errors.map(err => `<div class="output-line output-error">❌ ${err}</div>`).join('')}
        ${state.output.length === 0 && state.errors.length === 0 ? '<div style="opacity:0.5">لا مخرجات بعد</div>' : ''}
    </div>
    
    <script>
        const vscode = acquireVsCodeApi();
        function cmd(action) { vscode.postMessage({ type: action }); }
        function toggleBreakpoint(line) { vscode.postMessage({ type: 'toggleBreakpoint', line: line }); }
        function goToStack(line) { vscode.postMessage({ type: 'goToLine', line: line }); }
    </script>
</body>
</html>`;
}

/**
 * @brief تسجيل المصحح في VS Code
 * 
 * يسجل الأمر sad.debug الذي:
 *   1. يفتح WebView مع لوحة المصحح
 *   2. يعرض الكود مع نقاط التوقف
 *   3. عند الضغط على "تشغيل"، يشغّل المفسر ويلتقط المخرجات
 *   4. يعرض المتغيرات ومكدس الاستدعاءات
 * 
 * @param context سياق الإضافة
 */
export function registerDebugger(context: vscode.ExtensionContext): void {
    let panel: vscode.WebviewPanel | undefined;
    let breakpoints: BreakpointInfo[] = [];
    
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.debug', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor || editor.document.languageId !== 'sad') {
                vscode.window.showWarningMessage('افتح ملف .ص لبدء التصحيح');
                return;
            }

            // ─── محاولة تشغيل المصحح الأصلي DAP أولاً ───
            try {
                const program = editor.document.uri.fsPath;
                await vscode.debug.startDebugging(
                    vscode.workspace.getWorkspaceFolder(editor.document.uri),
                    {
                        type: 'sad',
                        request: 'launch',
                        name: 'تصحيح ص / Debug Sad',
                        program: program,
                        stopOnEntry: true
                    }
                );
                return; // نجح — لا حاجة للـ WebView
            } catch (e) {
                // فشل DAP — نستخدم الطريقة القديمة (WebView)
                console.warn('DAP debug failed, falling back to WebView:', e);
            }

            // ─── طريقة WebView الاحتياطية ───
            const document = editor.document;
            const code = document.getText();
            const fileName = path.basename(document.uri.fsPath);
            
            // حالة أولية
            const state: DebugState = {
                running: false,
                currentLine: 0,
                variables: extractVariables(code, 999),
                callStack: [],
                output: [],
                breakpoints: breakpoints.filter(bp => bp.file === document.uri.fsPath),
                errors: [],
            };
            
            // إنشاء WebView
            if (panel) {
                panel.reveal();
            } else {
                panel = vscode.window.createWebviewPanel(
                    'sadDebug',
                    `مصحح: ${fileName}`,
                    vscode.ViewColumn.Beside,
                    { enableScripts: true, retainContextWhenHidden: true }
                );
                
                panel.onDidDispose(() => { panel = undefined; });
            }
            
            // تحديث العرض
            function updateView(): void {
                if (panel) {
                    panel.webview.html = getDebugHTML(state, code);
                }
            }
            
            updateView();
            
            // استقبال رسائل من WebView
            panel.webview.onDidReceiveMessage(async (message: any) => {
                switch (message.type) {
                    case 'run': {
                        // تشغيل المفسر والتقاط المخرجات
                        state.running = true;
                        state.output = [];
                        state.errors = [];
                        state.currentLine = 1;
                        updateView();
                        
                        // إيجاد المفسر
                        const config = vscode.workspace.getConfiguration('sad');
                        let interpreterPath = config.get<string>('interpreter.path', '');
                        
                        if (!interpreterPath) {
                            const workspaceRoot = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || '';
                            const possiblePaths = [
                                path.join(workspaceRoot, 'build', 'bin', 'Debug', 'sad.exe'),
                                path.join(workspaceRoot, 'build', 'bin', 'Release', 'sad.exe'),
                                path.join(workspaceRoot, 'build', 'bin', 'sad.exe'),
                            ];
                            for (const p of possiblePaths) {
                                try { if (fs.existsSync(p)) { interpreterPath = p; break; } } catch { }
                            }
                        }
                        
                        if (!interpreterPath) {
                            state.errors.push('لم يُعثر على مفسر لغة ص');
                            state.running = false;
                            updateView();
                            return;
                        }
                        
                        // تشغيل المفسر
                        const filePath = document.uri.fsPath;
                        
                        exec(`"${interpreterPath}" "${filePath}"`, {
                            timeout: 30000,
                            cwd: path.dirname(filePath),
                        }, (error: any, stdout: string, stderr: string) => {
                            state.running = false;
                            
                            if (stdout) {
                                state.output = stdout.split('\n').filter((l: string) => l.trim());
                            }
                            if (stderr) {
                                state.errors = stderr.split('\n').filter((l: string) => l.trim());
                            }
                            if (error && error.code) {
                                state.errors.push(`رمز الخروج: ${error.code}`);
                            }
                            
                            state.currentLine = 0;
                            updateView();
                        });
                        break;
                    }
                    
                    case 'stop': {
                        state.running = false;
                        state.currentLine = 0;
                        updateView();
                        break;
                    }
                    
                    case 'step': {
                        if (state.currentLine < code.split('\n').length) {
                            state.currentLine++;
                            state.variables = extractVariables(code, state.currentLine);
                            state.callStack = buildCallStack(code, state.currentLine, fileName);
                            updateView();
                        }
                        break;
                    }
                    
                    case 'stepOver': {
                        // تخطي: ننتقل للسطر التالي بنفس المسافة البادئة
                        const lines = code.split('\n');
                        if (state.currentLine > 0 && state.currentLine <= lines.length) {
                            const currentIndent = lines[state.currentLine - 1].length - lines[state.currentLine - 1].trimStart().length;
                            let next = state.currentLine + 1;
                            while (next <= lines.length) {
                                const nextLine = lines[next - 1];
                                const nextIndent = nextLine.length - nextLine.trimStart().length;
                                if (nextLine.trim() && nextIndent <= currentIndent) { break; }
                                next++;
                            }
                            state.currentLine = Math.min(next, lines.length);
                            state.variables = extractVariables(code, state.currentLine);
                            state.callStack = buildCallStack(code, state.currentLine, fileName);
                            updateView();
                        }
                        break;
                    }
                    
                    case 'toggleBreakpoint': {
                        const line = message.line;
                        const existing = breakpoints.findIndex(bp => bp.line === line && bp.file === document.uri.fsPath);
                        if (existing >= 0) {
                            breakpoints.splice(existing, 1);
                        } else {
                            breakpoints.push({ line, file: document.uri.fsPath, enabled: true });
                        }
                        state.breakpoints = breakpoints.filter(bp => bp.file === document.uri.fsPath);
                        updateView();
                        break;
                    }
                    
                    case 'goToLine': {
                        const line = message.line - 1;
                        const ed = vscode.window.activeTextEditor;
                        if (ed) {
                            const range = new vscode.Range(line, 0, line, 0);
                            ed.selection = new vscode.Selection(range.start, range.start);
                            ed.revealRange(range, vscode.TextEditorRevealType.InCenter);
                        }
                        break;
                    }
                }
            });
        })
    );
}
