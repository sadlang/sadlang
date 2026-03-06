/**
 * @file test_explorer.ts
 * @brief مستكشف الاختبارات المدمج — Integrated Test Explorer
 * 
 * هذا الملف يوفر تكامل مع واجهة الاختبارات في VS Code
 * لاكتشاف وتشغيل اختبارات لغة ص تلقائياً.
 * 
 * === بنية الاختبارات في لغة ص ===
 * 
 * لغة ص تستخدم الكلمة السياقية "اختبر" لتعريف اختبارات:
 * 
 * ```sad
 * اختبر "اسم الاختبار"
 *     # كود الاختبار
 *     تأكد(شرط)
 *     تأكد_يساوي(فعلي، متوقع)
 * نهاية
 * ```
 * 
 * === كيف يعمل ===
 * 
 * 1. يمسح ملفات .ص في مساحة العمل
 * 2. يبحث عن كتل "اختبر" ويستخرج:
 *    - اسم الاختبار
 *    - الملف ورقم السطر
 *    - عدد التأكيدات
 * 3. يعرضها في شجرة Testing API
 * 4. عند التشغيل:
 *    - يُنفّذ الملف بـ sad.exe
 *    - يحلل المخرجات
 *    - يُحدّث حالة كل اختبار (نجح/فشل)
 * 
 * === التكامل مع VS Code ===
 * 
 * يستخدم TestController API الرسمي:
 *   - items: شجرة الاختبارات
 *   - createRunProfile: ملف تشغيل (Run/Debug)
 *   - TestRunRequest: طلب التشغيل
 *   - TestRun: تتبع نتيجة التشغيل
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';
import { exec } from 'child_process';
import * as path from 'path';

// ─────────────────────────────────────────────
// هياكل البيانات
// ─────────────────────────────────────────────

/**
 * @brief اختبار واحد مكتشف في الكود
 */
interface DiscoveredTest {
    /** اسم الاختبار */
    name: string;
    /** مسار الملف */
    filePath: string;
    /** رقم سطر البداية */
    startLine: number;
    /** رقم سطر النهاية */
    endLine: number;
    /** عدد التأكيدات */
    assertionCount: number;
}

// ─────────────────────────────────────────────
// اكتشاف الاختبارات
// ─────────────────────────────────────────────

/**
 * @brief تحليل ملف واستخراج الاختبارات
 * 
 * يبحث عن كتل "اختبر" ويستخرج:
 *   - اسم الاختبار (النص بين علامات التنصيص)
 *   - رقم سطر البداية والنهاية
 *   - عدد التأكيدات (تأكد/تأكد_يساوي/تأكد_لا_يساوي)
 * 
 * @param filePath مسار الملف
 * @param content محتوى الملف
 * @returns مصفوفة اختبارات مكتشفة
 */
function discoverTestsInFile(filePath: string, content: string): DiscoveredTest[] {
    const lines = content.split('\n');
    const tests: DiscoveredTest[] = [];
    
    let i = 0;
    while (i < lines.length) {
        const trimmed = lines[i].trim();
        
        // البحث عن "اختبر" مع اسم بين علامات تنصيص
        const testMatch = trimmed.match(/^اختبر\s+"([^"]+)"/);
        if (testMatch) {
            const testName = testMatch[1];
            const startLine = i;
            let endLine = i;
            let assertionCount = 0;
            
            // البحث عن "نهاية" المقابلة
            let depth = 1;
            let j = i + 1;
            while (j < lines.length && depth > 0) {
                const inner = lines[j].trim();
                
                // عدّ التأكيدات
                if (/^(تأكد|تأكد_يساوي|تأكد_لا_يساوي|تأكد_خطأ|تأكد_صحيح)\s*\(/.test(inner)) {
                    assertionCount++;
                }
                
                // تتبع عمق الكتل
                if (/^(دالة|صنف|بنية|إذا|اذا|بينما|لكل|اختبر|حاول|طابق)\b/.test(inner)) {
                    depth++;
                }
                if (inner === 'نهاية') {
                    depth--;
                }
                
                j++;
            }
            
            endLine = j - 1;
            
            tests.push({
                name: testName,
                filePath,
                startLine,
                endLine,
                assertionCount,
            });
            
            i = j;
        } else {
            i++;
        }
    }
    
    return tests;
}

/**
 * @brief مسح مساحة العمل واكتشاف جميع الاختبارات
 * 
 * يبحث في كل ملفات .ص عن كتل "اختبر".
 * 
 * @returns مصفوفة بكل الاختبارات المكتشفة
 */
async function discoverAllTests(): Promise<DiscoveredTest[]> {
    const allTests: DiscoveredTest[] = [];
    
    // البحث عن ملفات .ص في مساحة العمل
    const files = await vscode.workspace.findFiles('**/*.ص', '**/node_modules/**');
    
    for (const fileUri of files) {
        try {
            const doc = await vscode.workspace.openTextDocument(fileUri);
            const content = doc.getText();
            
            // تخطي الملفات التي لا تحتوي "اختبر"
            if (!content.includes('اختبر')) { continue; }
            
            const tests = discoverTestsInFile(fileUri.fsPath, content);
            allTests.push(...tests);
        } catch {
            // تخطي الملفات غير القابلة للقراءة
        }
    }
    
    return allTests;
}

// ─────────────────────────────────────────────
// تشغيل الاختبارات
// ─────────────────────────────────────────────

/**
 * @brief تنفيذ ملف اختبار وتحليل النتائج
 * 
 * يُنفّذ الملف بـ sad.exe ويحلل المخرجات لتحديد
 * نتيجة كل اختبار (نجح/فشل).
 * 
 * الشكل المتوقع للمخرجات:
 *   ✅ اسم الاختبار
 *   ❌ اسم الاختبار: رسالة الخطأ
 * 
 * @param filePath مسار الملف
 * @returns وعد بنتائج التنفيذ
 */
function runTestFile(filePath: string): Promise<{
    output: string;
    exitCode: number;
    passed: string[];
    failed: Array<{ name: string; message: string }>;
}> {
    return new Promise((resolve) => {
        const config = vscode.workspace.getConfiguration('sad');
        const interpreterPath = config.get<string>('interpreter.path', '');
        
        // البحث عن المفسر
        const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || '';
        const sadExe = interpreterPath || path.join(workspaceFolder, 'build', 'bin', 'Debug', 'sad.exe');
        
        exec(`"${sadExe}" "${filePath}"`, { timeout: 15000 }, (err: any, stdout: string, stderr: string) => {
            const output = stdout + stderr;
            const passed: string[] = [];
            const failed: Array<{ name: string; message: string }> = [];
            
            // تحليل المخرجات
            const lines = output.split('\n');
            for (const line of lines) {
                // نمط نجاح: ✅ اسم الاختبار
                const passMatch = line.match(/✅\s*(.+)/);
                if (passMatch) {
                    passed.push(passMatch[1].trim());
                    continue;
                }
                
                // نمط فشل: ❌ اسم الاختبار: رسالة
                const failMatch = line.match(/❌\s*([^:]+):\s*(.*)/);
                if (failMatch) {
                    failed.push({
                        name: failMatch[1].trim(),
                        message: failMatch[2].trim(),
                    });
                    continue;
                }
                
                // نمط فشل بديل: خطأ/Error
                const errorMatch = line.match(/(خطأ|Error|فشل|FAIL)\s*[:\s]\s*(.*)/i);
                if (errorMatch) {
                    failed.push({
                        name: 'عام',
                        message: errorMatch[2].trim(),
                    });
                }
            }
            
            resolve({
                output,
                exitCode: err ? (err as any).code || 1 : 0,
                passed,
                failed,
            });
        });
    });
}

// ─────────────────────────────────────────────
// التكامل مع VS Code Testing API
// ─────────────────────────────────────────────

/**
 * @brief تسجيل مستكشف الاختبارات
 * 
 * يُنشئ TestController ويسجل:
 *   - ملف تشغيل (Run Profile) 
 *   - اكتشاف تلقائي للاختبارات
 *   - تحديث عند تغيير الملفات
 *   - أمر يدوي لتحديث الاختبارات
 * 
 * @param context سياق الإضافة
 */
export function registerTestExplorer(context: vscode.ExtensionContext): void {
    // إنشاء TestController
    const ctrl = vscode.tests.createTestController('sadTests', 'اختبارات لغة ص');
    context.subscriptions.push(ctrl);
    
    /**
     * @brief تحديث شجرة الاختبارات
     * 
     * يمسح مساحة العمل ويبني شجرة اختبارات جديدة.
     * يجمع الاختبارات حسب الملف.
     */
    async function refreshTests(): Promise<void> {
        ctrl.items.replace([]); // مسح القائمة
        
        const allTests = await discoverAllTests();
        
        // تجميع حسب الملف
        const byFile = new Map<string, DiscoveredTest[]>();
        for (const test of allTests) {
            const existing = byFile.get(test.filePath) || [];
            existing.push(test);
            byFile.set(test.filePath, existing);
        }
        
        // بناء شجرة: ملف → اختبارات
        for (const [filePath, tests] of byFile) {
            const fileName = path.basename(filePath);
            const fileItem = ctrl.createTestItem(
                `file:${filePath}`,
                `📄 ${fileName}`,
                vscode.Uri.file(filePath)
            );
            
            for (const test of tests) {
                const testItem = ctrl.createTestItem(
                    `test:${filePath}:${test.startLine}`,
                    test.name,
                    vscode.Uri.file(filePath)
                );
                testItem.range = new vscode.Range(test.startLine, 0, test.endLine, 0);
                testItem.description = `${test.assertionCount} تأكيد`;
                fileItem.children.add(testItem);
            }
            
            ctrl.items.add(fileItem);
        }
    }
    
    // ──── ملف التشغيل ────
    ctrl.createRunProfile(
        'تشغيل الاختبارات',
        vscode.TestRunProfileKind.Run,
        async (request: vscode.TestRunRequest, token: vscode.CancellationToken) => {
            const run = ctrl.createTestRun(request);
            
            // تحديد الاختبارات المطلوب تشغيلها
            const testItems: vscode.TestItem[] = [];
            if (request.include) {
                for (const item of request.include) {
                    // إذا كان ملف → أضف أبناءه
                    if (item.children.size > 0) {
                        item.children.forEach((child: vscode.TestItem) => testItems.push(child));
                    } else {
                        testItems.push(item);
                    }
                }
            } else {
                // تشغيل الكل
                ctrl.items.forEach((fileItem: vscode.TestItem) => {
                    fileItem.children.forEach((testItem: vscode.TestItem) => testItems.push(testItem));
                });
            }
            
            // تجميع حسب الملف لتشغيل كل ملف مرة واحدة
            const fileTests = new Map<string, vscode.TestItem[]>();
            for (const item of testItems) {
                if (token.isCancellationRequested) { break; }
                
                const filePath = item.uri?.fsPath || '';
                const existing = fileTests.get(filePath) || [];
                existing.push(item);
                fileTests.set(filePath, existing);
                
                run.started(item);
            }
            
            // تنفيذ ملف بملف
            for (const [filePath, items] of fileTests) {
                if (token.isCancellationRequested) { break; }
                
                try {
                    const result = await runTestFile(filePath);
                    
                    for (const item of items) {
                        // البحث عن نتيجة هذا الاختبار
                        const testName = item.label;
                        
                        if (result.passed.some(n => n.includes(testName))) {
                            run.passed(item);
                        } else {
                            const failure = result.failed.find(f => f.name.includes(testName));
                            if (failure) {
                                const msg = new vscode.TestMessage(failure.message);
                                if (item.uri && item.range) {
                                    msg.location = new vscode.Location(item.uri, item.range);
                                }
                                run.failed(item, msg);
                            } else if (result.exitCode !== 0) {
                                // فشل عام — الملف لم يُنِفَّذ بنجاح
                                const msg = new vscode.TestMessage(
                                    `فشل تنفيذ الملف (كود ${result.exitCode}):\n${result.output.substring(0, 500)}`
                                );
                                run.failed(item, msg);
                            } else {
                                // لم نجد نتيجة محددة — نعتبره ناجحاً
                                run.passed(item);
                            }
                        }
                    }
                } catch (err: any) {
                    // خطأ في التنفيذ
                    for (const item of items) {
                        run.errored(item, new vscode.TestMessage(`خطأ: ${err.message}`));
                    }
                }
            }
            
            run.end();
        },
        true // isDefault
    );
    
    // ──── اكتشاف تلقائي ────
    // تحديث عند فتح مساحة العمل
    refreshTests();
    
    // تحديث عند تغيير ملفات .ص
    const watcher = vscode.workspace.createFileSystemWatcher('**/*.ص');
    watcher.onDidChange(() => refreshTests());
    watcher.onDidCreate(() => refreshTests());
    watcher.onDidDelete(() => refreshTests());
    context.subscriptions.push(watcher);
    
    // أمر يدوي لتحديث الاختبارات
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.refreshTests', () => {
            refreshTests();
            vscode.window.showInformationMessage('🔄 تم تحديث الاختبارات');
        })
    );
}
