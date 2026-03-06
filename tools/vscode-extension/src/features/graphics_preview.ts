/**
 * @file graphics_preview.ts
 * @brief معاينة الرسوميات المباشرة — Live Graphics Preview
 * 
 * هذا الملف يوفر WebView يعرض نتيجة أوامر الرسم في لغة ص.
 * عند استخدام مكتبة الرسوميات (رسوميات/)، يُحلل الكود
 * ويُنفذ أوامر الرسم في Canvas داخل VS Code.
 * 
 * === أوامر الرسم المدعومة ===
 * 
 * الكود يبحث عن استدعاءات دوال الرسم ويحولها إلى
 * أوامر Canvas2D المكافئة:
 * 
 *   ارسم_خط(س1، ع1، س2، ع2)           → lineTo
 *   ارسم_مستطيل(س، ع، عرض، ارتفاع)     → fillRect
 *   ارسم_دائرة(س، ع، نصف_قطر)          → arc
 *   ارسم_نص(نص، س، ع)                  → fillText
 *   لون_الرسم(لون)                      → fillStyle/strokeStyle
 *   لون_الخلفية(لون)                    → clearRect + fillRect
 *   سمك_الخط(سمك)                      → lineWidth
 *   امسح()                             → clearRect
 * 
 * === كيف يعمل ===
 * 
 * 1. يمسح الكود بحثاً عن استدعاءات دوال الرسم
 * 2. يحوّل كل استدعاء إلى أمر Canvas2D
 * 3. يُنفذ الأوامر في WebView
 * 4. يتحدث تلقائياً عند تغيير الكود (debounce)
 * 
 * ملاحظة: هذا محاكي بسيط — لا يُنفّذ الكود فعلياً،
 * بل يحلل أوامر الرسم فقط.
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';

/**
 * @brief أمر رسم واحد
 * 
 * يمثل أمر Canvas2D واحد مع معاملاته.
 */
interface DrawCommand {
    /** نوع الأمر */
    type: 'rect' | 'circle' | 'line' | 'text' | 'color' | 'bgColor' | 'lineWidth' | 'clear';
    /** المعاملات (أرقام أو نصوص) */
    args: (number | string)[];
    /** رقم السطر في المصدر */
    sourceLine: number;
}

/**
 * @brief أنماط اكتشاف أوامر الرسم في كود لغة ص
 * 
 * كل نمط يلتقط:
 *   - اسم الدالة (بالعربية)
 *   - المعاملات بين الأقواس
 */
const DRAW_PATTERNS: Array<{
    regex: RegExp;
    type: DrawCommand['type'];
    parseArgs: (args: string) => (number | string)[];
}> = [
    {
        regex: /ارسم_مستطيل\s*\(([^)]+)\)/,
        type: 'rect',
        parseArgs: (args) => args.split(/[,،]/).map(a => parseFloat(a.trim())),
    },
    {
        regex: /ارسم_دائرة\s*\(([^)]+)\)/,
        type: 'circle',
        parseArgs: (args) => args.split(/[,،]/).map(a => parseFloat(a.trim())),
    },
    {
        regex: /ارسم_خط\s*\(([^)]+)\)/,
        type: 'line',
        parseArgs: (args) => args.split(/[,،]/).map(a => parseFloat(a.trim())),
    },
    {
        regex: /ارسم_نص\s*\(([^)]+)\)/,
        type: 'text',
        parseArgs: (args) => {
            // ارسم_نص("نص"، س، ع)
            const match = args.match(/"([^"]*)"[,،]\s*(\d+)[,،]\s*(\d+)/);
            if (match) { return [match[1], parseFloat(match[2]), parseFloat(match[3])]; }
            return [];
        },
    },
    {
        regex: /لون_الرسم\s*\("([^"]+)"\)/,
        type: 'color',
        parseArgs: (args) => [args],
    },
    {
        regex: /لون_الخلفية\s*\("([^"]+)"\)/,
        type: 'bgColor',
        parseArgs: (args) => [args],
    },
    {
        regex: /سمك_الخط\s*\(([^)]+)\)/,
        type: 'lineWidth',
        parseArgs: (args) => [parseFloat(args.trim())],
    },
    {
        regex: /امسح\s*\(\s*\)/,
        type: 'clear',
        parseArgs: () => [],
    },
];

/**
 * @brief تحليل الكود واستخراج أوامر الرسم
 * 
 * يمسح الكود سطراً سطراً ويبحث عن استدعاءات
 * دوال الرسم المعروفة.
 * 
 * @param code نص الكود
 * @returns مصفوفة أوامر الرسم
 */
function extractDrawCommands(code: string): DrawCommand[] {
    const lines = code.split('\n');
    const commands: DrawCommand[] = [];
    
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i];
        const trimmed = line.trimStart();
        
        // تخطي التعليقات
        if (trimmed.startsWith('#')) { continue; }
        
        for (const { regex, type, parseArgs } of DRAW_PATTERNS) {
            const match = line.match(regex);
            if (match) {
                const argsStr = match[1] || '';
                
                let args: (number | string)[];
                if (type === 'color' || type === 'bgColor') {
                    args = [argsStr]; // اللون كنص مباشر
                } else {
                    args = parseArgs(argsStr);
                }
                
                commands.push({ type, args, sourceLine: i + 1 });
                break;
            }
        }
    }
    
    return commands;
}

/**
 * @brief تحويل أوامر الرسم إلى كود JavaScript Canvas2D
 * 
 * يحوّل كل أمر رسم إلى الاستدعاء المكافئ في Canvas API.
 * 
 * @param commands مصفوفة أوامر الرسم
 * @returns كود JavaScript لتنفيذه في Canvas
 */
function commandsToCanvasJS(commands: DrawCommand[]): string {
    const jsLines: string[] = [];
    
    jsLines.push('const canvas = document.getElementById("canvas");');
    jsLines.push('const ctx = canvas.getContext("2d");');
    jsLines.push('ctx.fillStyle = "#333";');
    jsLines.push('ctx.fillRect(0, 0, canvas.width, canvas.height);');
    jsLines.push('ctx.fillStyle = "#fff";');
    jsLines.push('ctx.strokeStyle = "#fff";');
    jsLines.push('ctx.lineWidth = 2;');
    jsLines.push('ctx.font = "16px Segoe UI, Tahoma, sans-serif";');
    jsLines.push('ctx.textAlign = "right";');
    jsLines.push('');
    
    for (const cmd of commands) {
        jsLines.push(`// سطر ${cmd.sourceLine}`);
        
        switch (cmd.type) {
            case 'rect': {
                const [x, y, w, h] = cmd.args as number[];
                jsLines.push(`ctx.fillRect(${x}, ${y}, ${w}, ${h});`);
                break;
            }
            case 'circle': {
                const [cx, cy, r] = cmd.args as number[];
                jsLines.push(`ctx.beginPath();`);
                jsLines.push(`ctx.arc(${cx}, ${cy}, ${r}, 0, Math.PI * 2);`);
                jsLines.push(`ctx.fill();`);
                jsLines.push(`ctx.stroke();`);
                break;
            }
            case 'line': {
                const [x1, y1, x2, y2] = cmd.args as number[];
                jsLines.push(`ctx.beginPath();`);
                jsLines.push(`ctx.moveTo(${x1}, ${y1});`);
                jsLines.push(`ctx.lineTo(${x2}, ${y2});`);
                jsLines.push(`ctx.stroke();`);
                break;
            }
            case 'text': {
                const [text, x, y] = cmd.args;
                jsLines.push(`ctx.fillText("${text}", ${x}, ${y});`);
                break;
            }
            case 'color': {
                const [color] = cmd.args;
                jsLines.push(`ctx.fillStyle = "${color}";`);
                jsLines.push(`ctx.strokeStyle = "${color}";`);
                break;
            }
            case 'bgColor': {
                const [color] = cmd.args;
                jsLines.push(`ctx.fillStyle = "${color}";`);
                jsLines.push(`ctx.fillRect(0, 0, canvas.width, canvas.height);`);
                break;
            }
            case 'lineWidth': {
                const [w] = cmd.args;
                jsLines.push(`ctx.lineWidth = ${w};`);
                break;
            }
            case 'clear': {
                jsLines.push(`ctx.clearRect(0, 0, canvas.width, canvas.height);`);
                break;
            }
        }
    }
    
    return jsLines.join('\n');
}

/**
 * @brief إنشاء HTML لمعاينة الرسوميات
 * 
 * يبني صفحة HTML مع Canvas ولوحة تحكم.
 * 
 * @param commands أوامر الرسم المستخرجة
 * @returns نص HTML كامل
 */
function getGraphicsHTML(commands: DrawCommand[]): string {
    const canvasJS = commandsToCanvasJS(commands);
    
    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <title>معاينة الرسوميات — لغة ص</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, sans-serif;
            background: var(--vscode-editor-background);
            color: var(--vscode-editor-foreground);
            margin: 0;
            display: flex;
            flex-direction: column;
            height: 100vh;
        }
        .toolbar {
            display: flex; gap: 8px; padding: 8px 16px;
            background: var(--vscode-titleBar-activeBackground);
            border-bottom: 1px solid var(--vscode-panel-border);
            align-items: center;
        }
        .toolbar span { font-size: 13px; opacity: 0.7; }
        .canvas-container {
            flex: 1;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 16px;
        }
        canvas {
            border: 1px solid var(--vscode-panel-border);
            border-radius: 4px;
            background: #333;
        }
        .info {
            padding: 8px 16px;
            font-size: 12px;
            opacity: 0.6;
            border-top: 1px solid var(--vscode-panel-border);
        }
    </style>
</head>
<body>
    <div class="toolbar">
        <span>🎨 معاينة الرسوميات</span>
        <span>— ${commands.length} أمر رسم</span>
    </div>
    
    <div class="canvas-container">
        <canvas id="canvas" width="600" height="400"></canvas>
    </div>
    
    <div class="info">
        أوامر الرسم المكتشفة: ${commands.map(c => `${c.type} (سطر ${c.sourceLine})`).join('، ')}
        ${commands.length === 0 ? '— لم يُعثر على أوامر رسم. استخدم: ارسم_مستطيل()، ارسم_دائرة()، ارسم_خط()، ارسم_نص()' : ''}
    </div>
    
    <script>
        try {
            ${canvasJS}
        } catch(e) {
            const canvas = document.getElementById("canvas");
            const ctx = canvas.getContext("2d");
            ctx.fillStyle = "#f44336";
            ctx.font = "16px Segoe UI";
            ctx.textAlign = "center";
            ctx.fillText("خطأ: " + e.message, canvas.width/2, canvas.height/2);
        }
        
        // استقبال تحديثات من الإضافة
        const vscode = acquireVsCodeApi();
    </script>
</body>
</html>`;
}

/**
 * @brief تسجيل معاينة الرسوميات في VS Code
 * 
 * يسجل الأمر sad.previewGraphics الذي:
 *   1. يحلل الكود الحالي ويستخرج أوامر الرسم
 *   2. يفتح WebView جانبي مع Canvas
 *   3. ينفذ الأوامر في Canvas
 *   4. يتحدث تلقائياً عند تعديل الكود
 * 
 * @param context سياق الإضافة
 */
export function registerGraphicsPreview(context: vscode.ExtensionContext): void {
    let panel: vscode.WebviewPanel | undefined;
    
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.previewGraphics', () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor || editor.document.languageId !== 'sad') {
                vscode.window.showWarningMessage('افتح ملف .ص يحتوي أوامر رسم');
                return;
            }
            
            if (panel) {
                panel.reveal(vscode.ViewColumn.Beside);
            } else {
                panel = vscode.window.createWebviewPanel(
                    'sadGraphics',
                    '🎨 معاينة الرسوميات',
                    vscode.ViewColumn.Beside,
                    { enableScripts: true, retainContextWhenHidden: true }
                );
                
                panel.onDidDispose(() => { panel = undefined; });
            }
            
            // عرض أولي
            updatePreview(editor.document);
            
            // تحديث عند التعديل (مع debounce)
            let timer: ReturnType<typeof setTimeout> | undefined;
            const disposable = vscode.workspace.onDidChangeTextDocument((event: vscode.TextDocumentChangeEvent) => {
                if (event.document.languageId === 'sad' && panel) {
                    if (timer) { clearTimeout(timer); }
                    timer = setTimeout(() => updatePreview(event.document), 300);
                }
            });
            
            panel.onDidDispose(() => { disposable.dispose(); });
            
            function updatePreview(doc: vscode.TextDocument): void {
                if (!panel) { return; }
                const commands = extractDrawCommands(doc.getText());
                panel.webview.html = getGraphicsHTML(commands);
            }
        })
    );
}
