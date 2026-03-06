/**
 * @file code_review.ts
 * @brief مراجعة الكود الذكية — Smart Code Review & Static Analysis
 * 
 * هذا الملف يوفر تحليلاً ثابتاً متقدماً لكود لغة ص.
 * يكتشف المشاكل التالية ويعرضها كتشخيصات (Diagnostics):
 * 
 * === قواعد التحليل ===
 * 
 * 1. متغيرات غير مستخدمة:
 *    - إذا عُرّف متغير بـ `متغير` ولم يُستخدم في أي مكان آخر
 *    - مستوى: تحذير (Warning)
 *    - إصلاح سريع: حذف السطر أو إضافة تعليق تبرير
 * 
 * 2. دوال طويلة جداً:
 *    - إذا تجاوزت الدالة 50 سطراً (قابل للتخصيص)
 *    - مستوى: معلومة (Information)
 *    - اقتراح: تقسيم الدالة إلى دوال أصغر
 * 
 * 3. تكرار الكود (Code Clones):
 *    - اكتشاف كتل متطابقة (3+ أسطر) في نفس الملف
 *    - مستوى: تلميح (Hint)
 *    - اقتراح: استخراج الكود المشترك إلى دالة
 * 
 * 4. ثوابت يجب أن تكون متغيرات:
 *    - إذا عُرّف بـ `متغير` ولم يُعدّل قيمته أبداً
 *    - مستوى: تلميح (Hint)
 *    - إصلاح سريع: تغيير `متغير` إلى `ثابت`
 * 
 * 5. استثناءات عامة:
 *    - استخدام `امسك` بدون تحديد نوع
 *    - مستوى: تحذير (Warning)
 * 
 * 6. أكواد ميتة (Dead Code):
 *    - كود بعد عبارة `ارجع` في نفس الكتلة
 *    - مستوى: تحذير (Warning)
 * 
 * 7. تسمية غير متسقة:
 *    - خلط بين أنماط التسمية في نفس الملف
 *    - مستوى: تلميح (Hint)
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';

/**
 * @brief معرّف مجموعة التشخيصات
 * يُستخدم لتمييز تشخيصات مراجعة الكود عن التشخيصات الأخرى
 */
const DIAGNOSTIC_SOURCE = 'مراجعة-ص';

/**
 * @brief أنماط تعريف المتغيرات والثوابت
 * 
 * يلتقط:
 *   - المجموعة 1: الكلمة المفتاحية (متغير أو ثابت)
 *   - المجموعة 2: اسم المتغير
 */
const VAR_PATTERN = /^(\s*)(متغير|ثابت)\s+([\u0600-\u06FF\u0750-\u077F\w]+)/;

/**
 * @brief نمط تعريف الدوال — يلتقط بداية ونهاية كل دالة
 */
const FUNC_DEF_PATTERN = /^(\s*)(غير_متزامن\s+)?(ساكن\s+)?دالة\s+([\u0600-\u06FF\u0750-\u077F\w]+)/;

/**
 * @brief نمط نهاية الكتلة
 */
const END_PATTERN = /^(\s*)نهاية\s*$/;

/**
 * @brief نمط عبارة الإرجاع
 */
const RETURN_PATTERN = /^(\s*)ارجع\b/;

/**
 * @brief نمط catch (امسك) العامة
 */
const CATCH_PATTERN = /^(\s*)امسك\s*$/;

/**
 * @brief التحقق مما إذا كان حرف جزءاً من كلمة عربية أو لاتينية
 * 
 * @param c الحرف المراد فحصه
 * @returns true إذا كان حرف كلمة
 */
function isWordChar(c: string): boolean {
    return /[\u0600-\u06FF\u0750-\u077F\w]/.test(c);
}

/**
 * @brief التحقق مما إذا كان الاسم يظهر كمرجع (استخدام) في سطر معين
 * 
 * يبحث عن الاسم مع حدود الكلمة.
 * يستثني التعريف نفسه والتعليقات والنصوص المقتبسة.
 * 
 * @param line نص السطر
 * @param name اسم المتغير
 * @param isDefLine هل هذا سطر التعريف؟
 * @returns true إذا وُجد استخدام
 */
function hasReference(line: string, name: string, isDefLine: boolean): boolean {
    // تخطي التعليقات
    const trimmed = line.trimStart();
    if (trimmed.startsWith('#')) { return false; }
    
    let searchStart = 0;
    let found = false;
    let occurrences = 0;
    
    while (true) {
        const idx = line.indexOf(name, searchStart);
        if (idx === -1) { break; }
        
        const before = idx > 0 ? line.charAt(idx - 1) : ' ';
        const after = idx + name.length < line.length ? line.charAt(idx + name.length) : ' ';
        
        if (!isWordChar(before) && !isWordChar(after)) {
            // تحقق من أننا خارج النص المقتبس
            const beforeText = line.substring(0, idx);
            const quoteCount = (beforeText.match(/"/g) || []).length;
            if (quoteCount % 2 === 0) {
                occurrences++;
            }
        }
        
        searchStart = idx + name.length;
    }
    
    // في سطر التعريف، الاسم يظهر مرة واحدة (التعريف نفسه)
    // فيجب أن يظهر أكثر من مرة ليُعتبر مستخدماً
    if (isDefLine) {
        return occurrences > 1;
    }
    
    return occurrences > 0;
}

/**
 * @brief تحليل المستند واكتشاف المشاكل
 * 
 * يمسح المستند سطراً سطراً ويطبق مجموعة من القواعد.
 * يُرجع مصفوفة من التشخيصات لعرضها في VS Code.
 * 
 * @param document المستند المراد تحليله
 * @returns مصفوفة التشخيصات المكتشفة
 */
function analyzeDocument(document: vscode.TextDocument): vscode.Diagnostic[] {
    const diagnostics: vscode.Diagnostic[] = [];
    const text = document.getText();
    const lines = text.split('\n');
    
    // === القاعدة 1: متغيرات غير مستخدمة ===
    analyzeUnusedVariables(document, lines, diagnostics);
    
    // === القاعدة 2: دوال طويلة ===
    analyzeLongFunctions(document, lines, diagnostics);
    
    // === القاعدة 3: كود بعد ارجع ===
    analyzeDeadCode(document, lines, diagnostics);
    
    // === القاعدة 4: امسك عامة ===
    analyzeGenericCatch(document, lines, diagnostics);
    
    // === القاعدة 5: تكرار الكود ===
    analyzeCodeClones(document, lines, diagnostics);
    
    // === القاعدة 6: ثابت مقترح ===
    analyzeShouldBeConst(document, lines, diagnostics);
    
    return diagnostics;
}

/**
 * @brief اكتشاف المتغيرات غير المستخدمة
 * 
 * خوارزمية:
 *   1. يجمع جميع تعريفات `متغير` و `ثابت`
 *   2. لكل تعريف، يبحث في بقية المستند عن أي استخدام
 *   3. إذا لم يُوجد استخدام، يُضاف تشخيص تحذيري
 * 
 * استثناءات:
 *   - المعاملات في تعريف الدالة (ليست متغيرات محلية)
 *   - المتغيرات المبدوءة بـ _ (تقليد لتجاهلها)
 */
function analyzeUnusedVariables(
    document: vscode.TextDocument,
    lines: string[],
    diagnostics: vscode.Diagnostic[]
): void {
    for (let i = 0; i < lines.length; i++) {
        const match = lines[i].match(VAR_PATTERN);
        if (!match) { continue; }
        
        const varName = match[3];
        
        // تخطي المتغيرات المبدوءة بـ _
        if (varName.startsWith('_')) { continue; }
        
        // البحث عن استخدام في بقية المستند
        let used = false;
        for (let j = 0; j < lines.length; j++) {
            if (j === i) { continue; }
            if (hasReference(lines[j], varName, false)) {
                used = true;
                break;
            }
        }
        
        if (!used) {
            const nameStart = lines[i].indexOf(varName);
            const range = new vscode.Range(i, nameStart, i, nameStart + varName.length);
            const diag = new vscode.Diagnostic(
                range,
                `المتغير "${varName}" مُعرّف لكن غير مستخدم`,
                vscode.DiagnosticSeverity.Warning
            );
            diag.source = DIAGNOSTIC_SOURCE;
            diag.code = 'unused-variable';
            diag.tags = [vscode.DiagnosticTag.Unnecessary];
            diagnostics.push(diag);
        }
    }
}

/**
 * @brief اكتشاف الدوال الطويلة جداً
 * 
 * خوارزمية:
 *   1. يبحث عن تعريفات الدوال
 *   2. يعد الأسطر حتى `نهاية` المطابقة
 *   3. إذا تجاوز 50 سطراً، يُضاف تشخيص معلوماتي
 * 
 * الحد الأقصى الافتراضي: 50 سطراً (قابل للتخصيص في المستقبل)
 */
function analyzeLongFunctions(
    document: vscode.TextDocument,
    lines: string[],
    diagnostics: vscode.Diagnostic[]
): void {
    const MAX_LINES = 50;
    
    for (let i = 0; i < lines.length; i++) {
        const match = lines[i].match(FUNC_DEF_PATTERN);
        if (!match) { continue; }
        
        const funcName = match[4];
        const defIndent = lines[i].length - lines[i].trimStart().length;
        
        // عد الأسطر حتى نهاية الدالة
        let endLine = i;
        for (let j = i + 1; j < lines.length; j++) {
            const trimmed = lines[j].trimStart();
            const indent = lines[j].length - trimmed.length;
            
            if (indent <= defIndent && trimmed.match(END_PATTERN)) {
                endLine = j;
                break;
            }
        }
        
        const funcLength = endLine - i;
        if (funcLength > MAX_LINES) {
            const range = new vscode.Range(i, 0, i, lines[i].length);
            const diag = new vscode.Diagnostic(
                range,
                `الدالة "${funcName}" طويلة جداً (${funcLength} سطر). يُنصح بتقسيمها إلى دوال أصغر (الحد: ${MAX_LINES} سطر)`,
                vscode.DiagnosticSeverity.Information
            );
            diag.source = DIAGNOSTIC_SOURCE;
            diag.code = 'long-function';
            diagnostics.push(diag);
        }
    }
}

/**
 * @brief اكتشاف الكود الميت بعد عبارة ارجع
 * 
 * خوارزمية:
 *   1. يبحث عن عبارات `ارجع`
 *   2. يتحقق من الأسطر التالية (حتى نهاية الكتلة)
 *   3. إذا وُجد كود (غير تعليق وغير نهاية) بعد ارجع، يُعتبر ميتاً
 */
function analyzeDeadCode(
    document: vscode.TextDocument,
    lines: string[],
    diagnostics: vscode.Diagnostic[]
): void {
    for (let i = 0; i < lines.length; i++) {
        if (!lines[i].match(RETURN_PATTERN)) { continue; }
        
        const returnIndent = lines[i].length - lines[i].trimStart().length;
        
        // فحص السطر التالي
        for (let j = i + 1; j < lines.length; j++) {
            const trimmed = lines[j].trimStart();
            if (!trimmed) { continue; } // سطر فارغ
            
            const indent = lines[j].length - trimmed.length;
            
            // إذا خرجنا من الكتلة، نتوقف
            if (indent <= returnIndent && trimmed.match(END_PATTERN)) { break; }
            if (indent < returnIndent) { break; }
            
            // إذا كان نفس المستوى وليس تعليقاً ولا نهاية
            if (indent === returnIndent && !trimmed.startsWith('#') && !trimmed.startsWith('نهاية') 
                && !trimmed.startsWith('وإلا') && !trimmed.startsWith('والا')) {
                const range = new vscode.Range(j, 0, j, lines[j].length);
                const diag = new vscode.Diagnostic(
                    range,
                    'كود لن يُنفّذ — يقع بعد عبارة ارجع',
                    vscode.DiagnosticSeverity.Warning
                );
                diag.source = DIAGNOSTIC_SOURCE;
                diag.code = 'dead-code';
                diag.tags = [vscode.DiagnosticTag.Unnecessary];
                diagnostics.push(diag);
            }
        }
    }
}

/**
 * @brief اكتشاف استخدام امسك العامة بدون نوع
 * 
 * يبحث عن `امسك` بدون تحديد نوع الاستثناء.
 * الصيغة الصحيحة: `امسك (خطأ)` أو `امسك (اسم)`
 * الصيغة المشكلة: `امسك` فقط (بدون أقواس)
 */
function analyzeGenericCatch(
    document: vscode.TextDocument,
    lines: string[],
    diagnostics: vscode.Diagnostic[]
): void {
    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trimStart();
        if (trimmed === 'امسك' || trimmed === 'امسك ') {
            const range = new vscode.Range(i, 0, i, lines[i].length);
            const diag = new vscode.Diagnostic(
                range,
                'استخدام امسك بدون تحديد نوع الاستثناء. يُنصح بـ: امسك (خطأ)',
                vscode.DiagnosticSeverity.Warning
            );
            diag.source = DIAGNOSTIC_SOURCE;
            diag.code = 'generic-catch';
            diagnostics.push(diag);
        }
    }
}

/**
 * @brief اكتشاف تكرار الكود (Code Clones)
 * 
 * خوارزمية:
 *   1. يجمع كتل من 3 أسطر متتابعة (بعد إزالة المسافات البادئة)
 *   2. يبحث عن كتل متطابقة في مواقع مختلفة
 *   3. إذا وُجد تكرار، يُضاف تشخيص تلميحي
 * 
 * الحد الأدنى: 3 أسطر متتالية متطابقة
 * يستثني: التعليقات والأسطر الفارغة والنهايات
 */
function analyzeCodeClones(
    document: vscode.TextDocument,
    lines: string[],
    diagnostics: vscode.Diagnostic[]
): void {
    const CLONE_SIZE = 3;
    const blocks: Map<string, number[]> = new Map();
    
    for (let i = 0; i <= lines.length - CLONE_SIZE; i++) {
        // بناء مفتاح من 3 أسطر متتابعة (بعد تقليم المسافات)
        const blockLines: string[] = [];
        let valid = true;
        
        for (let j = 0; j < CLONE_SIZE; j++) {
            const trimmed = lines[i + j].trim();
            // تخطي الكتل التي تحتوي فقط على تعليقات أو نهاية أو أسطر فارغة
            if (!trimmed || trimmed.startsWith('#') || trimmed === 'نهاية') {
                valid = false;
                break;
            }
            blockLines.push(trimmed);
        }
        
        if (!valid) { continue; }
        
        const key = blockLines.join('\n');
        if (!blocks.has(key)) {
            blocks.set(key, []);
        }
        blocks.get(key)!.push(i);
    }
    
    // الإبلاغ عن الكتل المتكررة (الظهور الثاني فقط)
    const reported = new Set<number>();
    for (const [, positions] of blocks) {
        if (positions.length < 2) { continue; }
        
        // تخطي إذا كانت المواقع متداخلة
        let nonOverlapping = [positions[0]];
        for (let i = 1; i < positions.length; i++) {
            if (positions[i] - nonOverlapping[nonOverlapping.length - 1] >= CLONE_SIZE) {
                nonOverlapping.push(positions[i]);
            }
        }
        
        if (nonOverlapping.length < 2) { continue; }
        
        // الإبلاغ عن الظهور الثاني وما بعده
        for (let i = 1; i < nonOverlapping.length; i++) {
            const pos = nonOverlapping[i];
            if (reported.has(pos)) { continue; }
            reported.add(pos);
            
            const range = new vscode.Range(pos, 0, pos + CLONE_SIZE - 1, lines[pos + CLONE_SIZE - 1].length);
            const diag = new vscode.Diagnostic(
                range,
                `كود مكرر (${CLONE_SIZE} أسطر) — يُنصح باستخراجه إلى دالة مشتركة (أول ظهور: سطر ${nonOverlapping[0] + 1})`,
                vscode.DiagnosticSeverity.Hint
            );
            diag.source = DIAGNOSTIC_SOURCE;
            diag.code = 'code-clone';
            diagnostics.push(diag);
        }
    }
}

/**
 * @brief اكتشاف المتغيرات التي يجب أن تكون ثوابت
 * 
 * خوارزمية:
 *   1. يجمع تعريفات `متغير`
 *   2. يبحث عن أي عملية إسناد (=) للمتغير في بقية المستند
 *   3. إذا لم يُعدّل أبداً، يقترح تحويله إلى `ثابت`
 * 
 * يستثني: معاملات الدوال (ليست من نوع متغير)
 */
function analyzeShouldBeConst(
    document: vscode.TextDocument,
    lines: string[],
    diagnostics: vscode.Diagnostic[]
): void {
    for (let i = 0; i < lines.length; i++) {
        const match = lines[i].match(VAR_PATTERN);
        if (!match) { continue; }
        if (match[2] !== 'متغير') { continue; } // فقط `متغير`
        
        const varName = match[3];
        if (varName.startsWith('_')) { continue; }
        
        // البحث عن عملية تعديل: varName = ... أو varName += ... إلخ
        const assignPattern = new RegExp(
            `(?<![\\u0600-\\u06FF\\u0750-\\u077F\\w])${escapeRegex(varName)}\\s*[+\\-*/%]?=`
        );
        
        let modified = false;
        for (let j = 0; j < lines.length; j++) {
            if (j === i) { continue; }
            const trimmed = lines[j].trimStart();
            if (trimmed.startsWith('#')) { continue; }
            
            if (assignPattern.test(lines[j])) {
                // التحقق من أنها ليست مقارنة (==)
                const idx = lines[j].search(assignPattern);
                if (idx >= 0) {
                    const afterVar = lines[j].substring(idx + varName.length).trimStart();
                    if (afterVar.startsWith('==') || afterVar.startsWith('!=')) { continue; }
                    modified = true;
                    break;
                }
            }
        }
        
        if (!modified) {
            const nameStart = lines[i].indexOf(varName);
            const range = new vscode.Range(i, nameStart, i, nameStart + varName.length);
            const diag = new vscode.Diagnostic(
                range,
                `المتغير "${varName}" لا يُعدّل بعد التعريف. يُنصح باستخدام "ثابت" بدلاً من "متغير"`,
                vscode.DiagnosticSeverity.Hint
            );
            diag.source = DIAGNOSTIC_SOURCE;
            diag.code = 'prefer-const';
            diagnostics.push(diag);
        }
    }
}

/**
 * @brief تهريب الأحرف الخاصة في regex
 */
function escapeRegex(str: string): string {
    return str.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');
}

/**
 * @brief مزود الإصلاحات السريعة (Quick Fix) للتشخيصات
 * 
 * يوفر إصلاحات سريعة لبعض التشخيصات:
 *   - unused-variable: حذف السطر
 *   - prefer-const: تغيير متغير إلى ثابت
 */
class SadCodeActionProvider implements vscode.CodeActionProvider {
    provideCodeActions(
        document: vscode.TextDocument,
        range: vscode.Range,
        context: vscode.CodeActionContext
    ): vscode.CodeAction[] {
        const actions: vscode.CodeAction[] = [];
        
        for (const diag of context.diagnostics) {
            if (diag.source !== DIAGNOSTIC_SOURCE) { continue; }
            
            if (diag.code === 'unused-variable') {
                // إصلاح: حذف السطر
                const deleteLine = new vscode.CodeAction(
                    'حذف المتغير غير المستخدم',
                    vscode.CodeActionKind.QuickFix
                );
                deleteLine.edit = new vscode.WorkspaceEdit();
                const fullLineRange = new vscode.Range(
                    diag.range.start.line, 0,
                    diag.range.start.line + 1, 0
                );
                deleteLine.edit.delete(document.uri, fullLineRange);
                deleteLine.diagnostics = [diag];
                deleteLine.isPreferred = true;
                actions.push(deleteLine);
                
                // إصلاح: إضافة _ للتجاهل
                const prefixUnderscore = new vscode.CodeAction(
                    'إضافة _ لتجاهل التحذير',
                    vscode.CodeActionKind.QuickFix
                );
                const varName = document.getText(diag.range);
                prefixUnderscore.edit = new vscode.WorkspaceEdit();
                prefixUnderscore.edit.replace(document.uri, diag.range, '_' + varName);
                prefixUnderscore.diagnostics = [diag];
                actions.push(prefixUnderscore);
            }
            
            if (diag.code === 'prefer-const') {
                // إصلاح: تغيير متغير إلى ثابت
                const toConst = new vscode.CodeAction(
                    'تغيير إلى ثابت',
                    vscode.CodeActionKind.QuickFix
                );
                const line = document.lineAt(diag.range.start.line).text;
                const varIdx = line.indexOf('متغير');
                if (varIdx >= 0) {
                    toConst.edit = new vscode.WorkspaceEdit();
                    toConst.edit.replace(
                        document.uri,
                        new vscode.Range(diag.range.start.line, varIdx, diag.range.start.line, varIdx + 'متغير'.length),
                        'ثابت'
                    );
                    toConst.diagnostics = [diag];
                    toConst.isPreferred = true;
                    actions.push(toConst);
                }
            }
        }
        
        return actions;
    }
}

/**
 * @brief تسجيل مراجعة الكود الذكية في VS Code  
 * 
 * يسجل:
 *   1. DiagnosticCollection لعرض التشخيصات
 *   2. مستمعات لتحديث التحليل عند التعديل والحفظ
 *   3. CodeActionProvider للإصلاحات السريعة
 * 
 * @param context سياق الإضافة
 */
export function registerCodeReview(context: vscode.ExtensionContext): void {
    const collection = vscode.languages.createDiagnosticCollection('sad-review');
    context.subscriptions.push(collection);
    
    /**
     * تحليل المستند وتحديث التشخيصات
     */
    function updateDiagnostics(document: vscode.TextDocument): void {
        if (document.languageId !== 'sad') {
            collection.delete(document.uri);
            return;
        }
        
        const diagnostics = analyzeDocument(document);
        collection.set(document.uri, diagnostics);
    }
    
    // تحليل عند الفتح
    if (vscode.window.activeTextEditor) {
        updateDiagnostics(vscode.window.activeTextEditor.document);
    }
    
    // تحليل عند تغيير المحرر النشط
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor((editor: vscode.TextEditor | undefined) => {
            if (editor) { updateDiagnostics(editor.document); }
        })
    );
    
    // تحليل عند الحفظ
    context.subscriptions.push(
        vscode.workspace.onDidSaveTextDocument((doc: vscode.TextDocument) => {
            updateDiagnostics(doc);
        })
    );
    
    // تحليل عند التعديل (مع debounce)
    let debounceTimer: ReturnType<typeof setTimeout> | undefined;
    context.subscriptions.push(
        vscode.workspace.onDidChangeTextDocument((event: vscode.TextDocumentChangeEvent) => {
            if (event.document.languageId !== 'sad') { return; }
            if (debounceTimer) { clearTimeout(debounceTimer); }
            debounceTimer = setTimeout(() => {
                updateDiagnostics(event.document);
            }, 1000); // انتظار ثانية بعد آخر تعديل
        })
    );
    
    // تسجيل مزود الإصلاحات السريعة
    context.subscriptions.push(
        vscode.languages.registerCodeActionsProvider(
            { language: 'sad', scheme: 'file' },
            new SadCodeActionProvider(),
            { providedCodeActionKinds: [vscode.CodeActionKind.QuickFix] }
        )
    );
}
