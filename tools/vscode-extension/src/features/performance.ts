/**
 * @file performance.ts
 * @brief تحليل الأداء البصري — Performance Flame Chart
 * 
 * هذا الملف يوفر أداة تحليل أداء بصرية لبرامج لغة ص.
 * يُنفّذ البرنامج مع قياس الزمن، ثم يعرض النتائج
 * في WebView بشكل مخطط لهبي (Flame Chart).
 * 
 * === كيف يعمل ===
 * 
 * 1. يحلل الكود ويستخرج الدوال وعدد الأسطر
 * 2. يُنفّذ البرنامج باستخدام sad.exe
 * 3. يقيس وقت التنفيذ الإجمالي
 * 4. يقدّر تكلفة كل دالة بناءً على:
 *    - عدد الأسطر
 *    - وجود حلقات (بينما/لكل)
 *    - تعقيد الحلقات المتداخلة
 * 5. يعرض النتائج في Flame Chart تفاعلي
 * 
 * ملاحظة: التقديرات تقريبية — لا يُستخدم profiler حقيقي.
 * الأرقام مبنية على تحليل ثابت + زمن التنفيذ الفعلي.
 * 
 * === مكونات المخطط اللهبي ===
 * 
 * - كل مستطيل يمثل دالة
 * - العرض يمثل الوقت النسبي
 * - اللون يعكس التعقيد (أخضر=بسيط، أحمر=معقد)
 * - النقر يكشف التفاصيل
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';
import { exec } from 'child_process';

// ─────────────────────────────────────────────
// هيكل بيانات التحليل
// ─────────────────────────────────────────────

/**
 * @brief معلومات دالة واحدة
 * 
 * تحتوي على بيانات الأداء المُقدّرة.
 */
interface FunctionProfile {
    /** اسم الدالة */
    name: string;
    /** رقم سطر البداية */
    startLine: number;
    /** رقم سطر النهاية */
    endLine: number;
    /** عدد الأسطر */
    lineCount: number;
    /** عدد الحلقات */
    loopCount: number;
    /** أقصى تداخل حلقات */
    maxLoopDepth: number;
    /** التعقيد المُقدّر (0-100) */
    complexity: number;
    /** الوقت المُقدّر (نسبة من الإجمالي) */
    estimatedTimePercent: number;
    /** الدوال المستدعاة */
    callees: string[];
}

/**
 * @brief نتيجة تحليل الأداء الكاملة
 */
interface ProfileResult {
    /** قائمة الدوال */
    functions: FunctionProfile[];
    /** وقت التنفيذ الفعلي (ملي ثانية) */
    totalTimeMs: number;
    /** مخرجات البرنامج */
    output: string;
    /** هل نجح التنفيذ */
    success: boolean;
}

// ─────────────────────────────────────────────
// تحليل الكود
// ─────────────────────────────────────────────

/**
 * @brief استخراج الدوال وتحليل تعقيدها
 * 
 * يمسح الكود ويبني قائمة بالدوال مع:
 *   - حساب عدد الأسطر
 *   - عدّ الحلقات وتداخلها
 *   - تقدير التعقيد الحسابي
 * 
 * @param code نص الكود
 * @returns مصفوفة معلومات الدوال
 */
function analyzeFunctions(code: string): FunctionProfile[] {
    const lines = code.split('\n');
    const functions: FunctionProfile[] = [];
    
    /** مكدس الدوال المفتوحة */
    const funcStack: Array<{
        name: string;
        startLine: number;
        loopCount: number;
        maxLoopDepth: number;
        currentLoopDepth: number;
        /** مكدس الكتل — لمعرفة أي "نهاية" تغلق الدالة */
        blockDepth: number;
        callees: string[];
    }> = [];
    
    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();
        
        // تخطي التعليقات والأسطر الفارغة
        if (trimmed === '' || trimmed.startsWith('#')) { continue; }
        
        // بداية دالة
        const funcMatch = trimmed.match(/^دالة\s+(\S+)\s*\(/);
        if (funcMatch) {
            funcStack.push({
                name: funcMatch[1],
                startLine: i + 1,
                loopCount: 0,
                maxLoopDepth: 0,
                currentLoopDepth: 0,
                blockDepth: 1, // الدالة نفسها كتلة
                callees: [],
            });
            continue;
        }
        
        // داخل دالة — تتبع الحلقات والاستدعاءات
        if (funcStack.length > 0) {
            const top = funcStack[funcStack.length - 1];
            
            // كلمة تفتح كتلة
            if (/^(إذا|اذا|بينما|لكل|صنف|حاول)\b/.test(trimmed)) {
                top.blockDepth++;
                
                if (/^(بينما|لكل)\b/.test(trimmed)) {
                    top.loopCount++;
                    top.currentLoopDepth++;
                    top.maxLoopDepth = Math.max(top.maxLoopDepth, top.currentLoopDepth);
                }
            }
            
            // نهاية كتلة
            if (trimmed === 'نهاية') {
                // تقليل عمق الحلقات إذا كنا نغلق حلقة
                if (top.currentLoopDepth > 0) {
                    top.currentLoopDepth--;
                }
                
                top.blockDepth--;
                
                // إذا وصل العمق إلى 0 → نهاية الدالة
                if (top.blockDepth === 0) {
                    const popped = funcStack.pop()!;
                    const lineCount = (i + 1) - popped.startLine + 1;
                    
                    // حساب التعقيد
                    let complexity = Math.min(lineCount * 2, 30); // أساس: أسطر
                    complexity += popped.loopCount * 10; // كل حلقة تضيف 10
                    complexity += popped.maxLoopDepth * 20; // تداخل يضيف 20
                    complexity = Math.min(complexity, 100);
                    
                    functions.push({
                        name: popped.name,
                        startLine: popped.startLine,
                        endLine: i + 1,
                        lineCount,
                        loopCount: popped.loopCount,
                        maxLoopDepth: popped.maxLoopDepth,
                        complexity,
                        estimatedTimePercent: 0,
                        callees: popped.callees,
                    });
                    continue;
                }
            }
            
            // استدعاء دالة
            const callMatch = trimmed.match(/(\S+)\s*\(/);
            if (callMatch && !trimmed.startsWith('دالة') && !trimmed.startsWith('صنف')) {
                const calledName = callMatch[1];
                if (!top.callees.includes(calledName)) {
                    top.callees.push(calledName);
                }
            }
        }
    }
    
    // حساب التوزيع النسبي
    const totalComplexity = functions.reduce((s, f) => s + f.complexity, 0) || 1;
    for (const f of functions) {
        f.estimatedTimePercent = Math.round((f.complexity / totalComplexity) * 100);
    }
    
    return functions;
}

/**
 * @brief تنفيذ البرنامج وقياس الوقت
 * 
 * يُنفّذ ملف .ص باستخدام sad.exe ويقيس وقت التنفيذ.
 * 
 * @param filePath مسار الملف
 * @returns وعد بنتيجة التنفيذ
 */
function executeAndMeasure(filePath: string): Promise<{ timeMs: number; output: string; success: boolean }> {
    return new Promise((resolve) => {
        // البحث عن المفسر
        const config = vscode.workspace.getConfiguration('sad');
        const interpreterPath = config.get<string>('interpreter.path', '') || findInterpreter();
        
        const start = Date.now();
        
        exec(`"${interpreterPath}" "${filePath}"`, { timeout: 10000 }, (err: any, stdout: string, stderr: string) => {
            const timeMs = Date.now() - start;
            resolve({
                timeMs,
                output: stdout + (stderr ? '\n' + stderr : ''),
                success: !err,
            });
        });
    });
}

/**
 * @brief البحث عن المفسر في المسارات الشائعة
 */
function findInterpreter(): string {
    const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || '';
    const paths = [
        `${workspaceFolder}/build/bin/Debug/sad.exe`,
        `${workspaceFolder}/build/bin/Release/sad.exe`,
        `${workspaceFolder}/build/bin/sad.exe`,
    ];
    return paths[0]; // نستخدم المسار الأول كافتراضي
}

/**
 * @brief لون التعقيد
 * 
 * يُرجع لوناً من أخضر (بسيط) إلى أحمر (معقد).
 */
function complexityColor(complexity: number): string {
    if (complexity < 20) { return '#4caf50'; } // أخضر
    if (complexity < 40) { return '#8bc34a'; } // أخضر فاتح
    if (complexity < 60) { return '#ffeb3b'; } // أصفر
    if (complexity < 80) { return '#ff9800'; } // برتقالي
    return '#f44336'; // أحمر
}

/**
 * @brief إنشاء HTML مخطط الأداء
 * 
 * يبني WebView مع:
 *   - ملخص الأداء (إجمالي، أبطأ دالة)
 *   - Flame Chart بصري
 *   - جدول تفصيلي لكل دالة
 *   - توصيات تحسين
 */
function getPerformanceHTML(result: ProfileResult): string {
    const sortedFuncs = [...result.functions].sort((a, b) => b.complexity - a.complexity);
    
    // بناء أشرطة Flame Chart
    const flameHTML = result.functions.map(f => {
        const color = complexityColor(f.complexity);
        const width = Math.max(f.estimatedTimePercent, 5);
        return `<div class="flame-bar" style="width:${width}%;background:${color};"
                    title="${f.name}: تعقيد ${f.complexity}%, ${f.loopCount} حلقة"
                    onclick="goToLine(${f.startLine})">
                    <span>${f.name}</span>
                    <small>${f.estimatedTimePercent}%</small>
                </div>`;
    }).join('\n');
    
    // بناء جدول التفاصيل
    const tableRows = sortedFuncs.map(f => `
        <tr onclick="goToLine(${f.startLine})" style="cursor:pointer">
            <td><strong>${f.name}</strong></td>
            <td>${f.lineCount}</td>
            <td>${f.loopCount}</td>
            <td>${f.maxLoopDepth}</td>
            <td><span style="color:${complexityColor(f.complexity)}">${f.complexity}</span></td>
            <td>${f.estimatedTimePercent}%</td>
            <td>${f.callees.length > 0 ? f.callees.join('، ') : '—'}</td>
        </tr>
    `).join('\n');
    
    // توصيات تحسين
    const recommendations: string[] = [];
    for (const f of sortedFuncs) {
        if (f.maxLoopDepth >= 2) {
            recommendations.push(`⚠️ <strong>${f.name}</strong>: حلقات متداخلة (عمق ${f.maxLoopDepth}) — قد تسبب بطءاً مع بيانات كبيرة`);
        }
        if (f.lineCount > 50) {
            recommendations.push(`📏 <strong>${f.name}</strong>: دالة طويلة (${f.lineCount} سطر) — فكر بتقسيمها`);
        }
        if (f.complexity >= 80) {
            recommendations.push(`🔴 <strong>${f.name}</strong>: تعقيد عالي جداً (${f.complexity}) — يحتاج إعادة هيكلة`);
        }
    }
    
    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <title>تحليل الأداء — لغة ص</title>
    <style>
        body {
            font-family: 'Segoe UI', Tahoma, sans-serif;
            background: var(--vscode-editor-background);
            color: var(--vscode-editor-foreground);
            margin: 0; padding: 20px;
        }
        h2, h3 { margin-bottom: 8px; }
        
        .summary {
            display: flex; gap: 16px; margin-bottom: 20px;
            flex-wrap: wrap;
        }
        .summary-card {
            background: var(--vscode-textCodeBlock-background);
            padding: 16px; border-radius: 8px; min-width: 150px;
            border: 1px solid var(--vscode-panel-border);
        }
        .summary-card .value { font-size: 28px; font-weight: bold; }
        .summary-card .label { font-size: 12px; opacity: 0.7; }
        
        .flame-chart {
            background: var(--vscode-textCodeBlock-background);
            padding: 16px; border-radius: 8px; margin-bottom: 20px;
            border: 1px solid var(--vscode-panel-border);
        }
        .flame-bar {
            padding: 8px 12px; margin: 4px 0; border-radius: 4px;
            display: flex; justify-content: space-between; align-items: center;
            cursor: pointer; min-width: 60px; color: #000;
            font-weight: 500; font-size: 13px;
            transition: opacity 0.2s;
        }
        .flame-bar:hover { opacity: 0.8; }
        .flame-bar small { opacity: 0.8; }
        
        table {
            width: 100%; border-collapse: collapse;
            font-size: 13px; margin-bottom: 20px;
        }
        th, td {
            padding: 8px 12px; text-align: right;
            border-bottom: 1px solid var(--vscode-panel-border);
        }
        th { font-weight: 600; opacity: 0.7; }
        tr:hover { background: var(--vscode-list-hoverBackground); }
        
        .reco { padding: 8px 12px; margin: 4px 0; font-size: 13px;
                 background: var(--vscode-textCodeBlock-background);
                 border-radius: 4px; }
        
        .badge {
            font-size: 11px; padding: 2px 8px; border-radius: 10px;
            color: #fff; display: inline-block;
        }
    </style>
</head>
<body>
    <h2>📊 تحليل الأداء</h2>
    
    <div class="summary">
        <div class="summary-card">
            <div class="value">${result.totalTimeMs}</div>
            <div class="label">وقت التنفيذ (مل.ث)</div>
        </div>
        <div class="summary-card">
            <div class="value">${result.functions.length}</div>
            <div class="label">عدد الدوال</div>
        </div>
        <div class="summary-card">
            <div class="value">${sortedFuncs.length > 0 ? sortedFuncs[0].name : '—'}</div>
            <div class="label">أكثر دالة تعقيداً</div>
        </div>
        <div class="summary-card">
            <div class="value" style="color:${result.success ? '#4caf50' : '#f44336'}">${result.success ? '✅ نجح' : '❌ فشل'}</div>
            <div class="label">حالة التنفيذ</div>
        </div>
    </div>
    
    <h3>🔥 المخطط اللهبي</h3>
    <div class="flame-chart">
        ${flameHTML || '<p style="opacity:0.5">لم يُعثر على دوال لتحليلها</p>'}
    </div>
    
    <h3>📋 جدول التفاصيل</h3>
    <table>
        <tr>
            <th>الدالة</th>
            <th>أسطر</th>
            <th>حلقات</th>
            <th>تداخل</th>
            <th>تعقيد</th>
            <th>نسبة</th>
            <th>دوال مُستدعاة</th>
        </tr>
        ${tableRows || '<tr><td colspan="7" style="text-align:center;opacity:0.5">لا توجد دوال</td></tr>'}
    </table>
    
    ${recommendations.length > 0 ? `
    <h3>💡 توصيات تحسين</h3>
    ${recommendations.map(r => `<div class="reco">${r}</div>`).join('\n')}
    ` : '<p style="opacity:0.5">✅ لا توجد مشاكل أداء واضحة</p>'}
    
    <script>
        const vscode = acquireVsCodeApi();
        function goToLine(line) {
            vscode.postMessage({ command: 'goToLine', line });
        }
    </script>
</body>
</html>`;
}

/**
 * @brief تسجيل أداة تحليل الأداء
 * 
 * يسجل أمر sad.analyzePerformance الذي:
 *   1. يحلل الكود واستخراج الدوال
 *   2. يُنفّذ البرنامج مع قياس الزمن
 *   3. يعرض Flame Chart في WebView
 * 
 * @param context سياق الإضافة
 */
export function registerPerformanceAnalyzer(context: vscode.ExtensionContext): void {
    let panel: vscode.WebviewPanel | undefined;
    
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.analyzePerformance', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor || editor.document.languageId !== 'sad') {
                vscode.window.showWarningMessage('افتح ملف .ص لتحليل أدائه');
                return;
            }
            
            // حفظ الملف أولاً
            await editor.document.save();
            
            const code = editor.document.getText();
            const filePath = editor.document.uri.fsPath;
            
            // === الخطوة 1: تحليل الكود ===
            const functions = analyzeFunctions(code);
            
            // === الخطوة 2: تنفيذ البرنامج وقياس الزمن ===
            vscode.window.withProgress(
                {
                    location: vscode.ProgressLocation.Notification,
                    title: '⏱️ جاري تحليل الأداء...',
                    cancellable: false,
                },
                async () => {
                    const execResult = await executeAndMeasure(filePath);
                    
                    const result: ProfileResult = {
                        functions,
                        totalTimeMs: execResult.timeMs,
                        output: execResult.output,
                        success: execResult.success,
                    };
                    
                    // === الخطوة 3: عرض النتائج ===
                    if (panel) {
                        panel.reveal();
                    } else {
                        panel = vscode.window.createWebviewPanel(
                            'sadPerformance',
                            '📊 تحليل الأداء',
                            vscode.ViewColumn.Beside,
                            { enableScripts: true }
                        );
                        panel.onDidDispose(() => { panel = undefined; });
                    }
                    
                    panel.webview.html = getPerformanceHTML(result);
                    
                    // معالجة رسائل WebView (الانتقال إلى سطر)
                    panel.webview.onDidReceiveMessage((msg: any) => {
                        if (msg.command === 'goToLine') {
                            const line = msg.line - 1;
                            if (editor) {
                                const pos = new vscode.Position(line, 0);
                                editor.selection = new vscode.Selection(pos, pos);
                                editor.revealRange(new vscode.Range(pos, pos), vscode.TextEditorRevealType.InCenter);
                            }
                        }
                    });
                }
            );
        })
    );
}
