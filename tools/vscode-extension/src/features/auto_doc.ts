/**
 * @file auto_doc.ts
 * @brief مولد التوثيق التلقائي — Auto Documentation Generator
 * 
 * هذا الملف يوفر ميزة توليد التوثيق التلقائي للدوال والأصناف في لغة ص.
 * عند الضغط على Ctrl+Shift+D، يتم تحليل الدالة أو الصنف الحالي
 * وتوليد كتلة توثيق #** **# تلقائياً تحتوي على:
 *   - وصف الدالة/الصنف
 *   - المعاملات وأنواعها (مستنتجة من السياق)
 *   - القيمة المرجعة
 *   - أمثلة استخدام
 * 
 * الخوارزمية:
 *   1. نحدد موقع المؤشر الحالي
 *   2. نبحث عن أقرب تعريف دالة/صنف/بنية/سمة فوق المؤشر
 *   3. نحلل التوقيع (signature) لاستخراج الاسم والمعاملات
 *   4. نبحث داخل جسم الدالة عن عبارات `ارجع` لاستنتاج نوع الإرجاع
 *   5. نولد كتلة التوثيق ونُدرجها فوق التعريف
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';

/**
 * @brief أنواع التعريفات التي يمكن توثيقها
 * 
 * نوع التعريف يحدد شكل كتلة التوثيق المولدة.
 * مثلاً: الدالة تحتوي على @معاملات و @إرجاع،
 * بينما الصنف يحتوي على @وصف و @وراثة.
 */
type DefinitionKind = 'function' | 'class' | 'struct' | 'enum' | 'trait' | 'property';

/**
 * @brief معلومات التعريف المستخرجة من الكود
 * 
 * هذه البنية تحتوي على جميع المعلومات المحللة
 * من سطر التعريف وجسم الكتلة.
 */
interface DefinitionInfo {
    /** نوع التعريف (دالة، صنف، إلخ) */
    kind: DefinitionKind;
    /** اسم الرمز (مثلاً: "جمع"، "حيوان") */
    name: string;
    /** قائمة أسماء المعاملات (للدوال فقط) */
    params: string[];
    /** هل الدالة تحتوي على عبارة ارجع؟ */
    hasReturn: boolean;
    /** رقم السطر الذي يبدأ فيه التعريف (0-based) */
    line: number;
    /** الصنف الأب إذا وُجد (يرث منه) */
    parentClass?: string;
    /** هل الدالة غير متزامنة؟ */
    isAsync: boolean;
    /** هل الدالة ساكنة (static)? */
    isStatic: boolean;
}

/**
 * @brief الأنماط المعبرية (Regex) لتحليل تعريفات لغة ص
 * 
 * كل نمط يلتقط مجموعة (group) محددة:
 *   - المجموعة 1: الاسم
 *   - المجموعة 2: المعاملات (إن وجدت)
 *   - المجموعة 3: معلومات إضافية (مثل الصنف الأب)
 */
const PATTERNS = {
    /**
     * نمط الدالة: دالة اسم_الدالة(معامل1، معامل2)
     * يلتقط: الاسم + المعاملات بين الأقواس
     * يدعم: غير_متزامن دالة، ساكن دالة
     */
    function: /^(\s*)(غير_متزامن\s+)?(ساكن\s+)?دالة\s+([\u0600-\u06FF\u0750-\u077F\w]+)\s*\(([^)]*)\)/,
    
    /**
     * نمط الصنف: صنف اسم_الصنف يرث الأب
     * يلتقط: الاسم + الصنف الأب (اختياري)
     */
    class: /^(\s*)صنف\s+([\u0600-\u06FF\u0750-\u077F\w]+)(\s+يرث\s+([\u0600-\u06FF\u0750-\u077F\w]+))?/,
    
    /**
     * نمط البنية: بنية اسم_البنية
     * يلتقط: الاسم فقط
     */
    struct: /^(\s*)بنية\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
    
    /**
     * نمط التعداد: تعداد اسم_التعداد
     * يلتقط: الاسم فقط
     */
    enum: /^(\s*)تعداد\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
    
    /**
     * نمط السمة: سمة اسم_السمة
     * يلتقط: الاسم فقط
     */
    trait: /^(\s*)سمة\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
    
    /**
     * نمط الباني: باني(معاملات)
     * يلتقط: المعاملات بين الأقواس
     */
    constructor: /^(\s*)باني\s*\(([^)]*)\)/,
    
    /**
     * نمط الخاصية: خاصية اسم_الخاصية
     * يلتقط: الاسم فقط
     */
    property: /^(\s*)خاصية\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
};

/**
 * @brief نمط عبارة الإرجاع: ارجع قيمة
 * يستخدم لاكتشاف ما إذا كانت الدالة تُرجع قيمة
 */
const RETURN_PATTERN = /\bارجع\b/;

/**
 * @brief تحليل سطر التعريف واستخراج معلومات التعريف
 * 
 * يأخذ سطر الكود ويحاول مطابقته مع أنماط التعريف المعروفة.
 * إذا نجحت المطابقة، يستخرج الاسم والمعاملات والمعلومات الإضافية.
 * 
 * @param lineText نص السطر المراد تحليله
 * @param lineNumber رقم السطر (0-based)
 * @param document المستند لتحليل الجسم
 * @returns معلومات التعريف أو null إذا لم يكن السطر تعريفاً
 */
function parseDefinition(lineText: string, lineNumber: number, document: vscode.TextDocument): DefinitionInfo | null {
    // === تحليل الدوال ===
    const funcMatch = lineText.match(PATTERNS.function);
    if (funcMatch) {
        const isAsync = !!funcMatch[2];
        const isStatic = !!funcMatch[3];
        const name = funcMatch[4];
        const paramsStr = funcMatch[5].trim();
        const params = paramsStr ? paramsStr.split(/[,،]/).map(p => p.trim()).filter(p => p) : [];
        
        // البحث عن عبارة ارجع في جسم الدالة
        const hasReturn = scanBodyForReturn(document, lineNumber);
        
        return { kind: 'function', name, params, hasReturn, line: lineNumber, isAsync, isStatic };
    }
    
    // === تحليل الباني ===
    const ctorMatch = lineText.match(PATTERNS.constructor);
    if (ctorMatch) {
        const paramsStr = ctorMatch[2].trim();
        const params = paramsStr ? paramsStr.split(/[,،]/).map(p => p.trim()).filter(p => p) : [];
        return { kind: 'function', name: 'باني', params, hasReturn: false, line: lineNumber, isAsync: false, isStatic: false };
    }
    
    // === تحليل الأصناف ===
    const classMatch = lineText.match(PATTERNS.class);
    if (classMatch) {
        return {
            kind: 'class',
            name: classMatch[2],
            params: [],
            hasReturn: false,
            line: lineNumber,
            parentClass: classMatch[4] || undefined,
            isAsync: false,
            isStatic: false,
        };
    }
    
    // === تحليل البنى ===
    const structMatch = lineText.match(PATTERNS.struct);
    if (structMatch) {
        return { kind: 'struct', name: structMatch[2], params: [], hasReturn: false, line: lineNumber, isAsync: false, isStatic: false };
    }
    
    // === تحليل التعدادات ===
    const enumMatch = lineText.match(PATTERNS.enum);
    if (enumMatch) {
        return { kind: 'enum', name: enumMatch[2], params: [], hasReturn: false, line: lineNumber, isAsync: false, isStatic: false };
    }
    
    // === تحليل السمات ===
    const traitMatch = lineText.match(PATTERNS.trait);
    if (traitMatch) {
        return { kind: 'trait', name: traitMatch[2], params: [], hasReturn: false, line: lineNumber, isAsync: false, isStatic: false };
    }
    
    // === تحليل الخصائص ===
    const propMatch = lineText.match(PATTERNS.property);
    if (propMatch) {
        return { kind: 'property', name: propMatch[2], params: [], hasReturn: false, line: lineNumber, isAsync: false, isStatic: false };
    }
    
    return null;
}

/**
 * @brief البحث في جسم الدالة عن عبارات ارجع
 * 
 * يبدأ من السطر التالي لتعريف الدالة ويبحث حتى يجد `نهاية`
 * المطابقة (بنفس مستوى المسافة البادئة). يبحث عن `ارجع` في الجسم.
 * 
 * @param document المستند الحالي
 * @param startLine سطر بداية الدالة (0-based)
 * @returns true إذا وُجدت عبارة ارجع واحدة على الأقل
 */
function scanBodyForReturn(document: vscode.TextDocument, startLine: number): boolean {
    // نحسب المسافة البادئة لسطر التعريف
    const defLine = document.lineAt(startLine).text;
    const defIndent = defLine.length - defLine.trimStart().length;
    
    // نبحث في الأسطر التالية حتى نجد نهاية بنفس المستوى
    for (let i = startLine + 1; i < document.lineCount; i++) {
        const line = document.lineAt(i).text;
        const trimmed = line.trimStart();
        
        // تخطي الأسطر الفارغة
        if (!trimmed) { continue; }
        
        const indent = line.length - trimmed.length;
        
        // إذا وجدنا نهاية بنفس المستوى أو أقل، نتوقف
        if (indent <= defIndent && trimmed.startsWith('نهاية')) {
            break;
        }
        
        // البحث عن ارجع
        if (RETURN_PATTERN.test(trimmed)) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief توليد كتلة التوثيق لتعريف معين
 * 
 * ينشئ كتلة توثيق #** **# مع:
 *   - وصف عام
 *   - المعاملات مع أنواعها (فراغات ليملأها المستخدم)
 *   - القيمة المرجعة
 *   - مثال استخدام
 * 
 * @param info معلومات التعريف المحللة
 * @returns النص الكامل لكتلة التوثيق (متعدد الأسطر)
 */
function generateDocBlock(info: DefinitionInfo): string {
    // المسافة البادئة: نستخدم نفس المسافة التي يستخدمها التعريف
    const indent = '';
    const lines: string[] = [];
    
    lines.push(`${indent}#**`);
    
    switch (info.kind) {
        case 'function': {
            if (info.name === 'باني') {
                lines.push(`${indent} * @وصف باني الصنف — يُنشئ كائناً جديداً`);
            } else {
                // وصف مبدئي يعتمد على اسم الدالة
                lines.push(`${indent} * @وصف ${info.name} — [أضف وصفاً هنا]`);
            }
            lines.push(`${indent} *`);
            
            // المعاملات
            if (info.params.length > 0) {
                for (const param of info.params) {
                    lines.push(`${indent} * @معامل ${param} — [وصف المعامل]`);
                }
                lines.push(`${indent} *`);
            }
            
            // القيمة المرجعة
            if (info.hasReturn) {
                lines.push(`${indent} * @إرجاع [وصف القيمة المرجعة]`);
                lines.push(`${indent} *`);
            }
            
            // خصائص إضافية
            if (info.isAsync) {
                lines.push(`${indent} * @ملاحظة هذه دالة غير متزامنة — استخدم انتظر عند استدعائها`);
                lines.push(`${indent} *`);
            }
            if (info.isStatic) {
                lines.push(`${indent} * @ملاحظة دالة ساكنة — تُستدعى على الصنف مباشرة`);
                lines.push(`${indent} *`);
            }
            
            // مثال
            if (info.name !== 'باني') {
                lines.push(`${indent} * @مثال`);
                if (info.params.length > 0) {
                    const args = info.params.map(() => '...').join('، ');
                    if (info.hasReturn) {
                        lines.push(`${indent} *   متغير نتيجة = ${info.name}(${args})`);
                    } else {
                        lines.push(`${indent} *   ${info.name}(${args})`);
                    }
                } else {
                    if (info.hasReturn) {
                        lines.push(`${indent} *   متغير نتيجة = ${info.name}()`);
                    } else {
                        lines.push(`${indent} *   ${info.name}()`);
                    }
                }
            }
            break;
        }
        
        case 'class': {
            lines.push(`${indent} * @صنف ${info.name} — [وصف الصنف]`);
            lines.push(`${indent} *`);
            if (info.parentClass) {
                lines.push(`${indent} * @يرث ${info.parentClass}`);
                lines.push(`${indent} *`);
            }
            lines.push(`${indent} * @مثال`);
            lines.push(`${indent} *   متغير كائن = جديد ${info.name}()`);
            break;
        }
        
        case 'struct': {
            lines.push(`${indent} * @بنية ${info.name} — [وصف البنية]`);
            lines.push(`${indent} *`);
            lines.push(`${indent} * @مثال`);
            lines.push(`${indent} *   متغير ب = جديد ${info.name}()`);
            break;
        }
        
        case 'enum': {
            lines.push(`${indent} * @تعداد ${info.name} — [وصف التعداد]`);
            lines.push(`${indent} *`);
            lines.push(`${indent} * @القيم`);
            lines.push(`${indent} *   [أضف قيم التعداد هنا]`);
            break;
        }
        
        case 'trait': {
            lines.push(`${indent} * @سمة ${info.name} — [وصف السمة/الواجهة]`);
            lines.push(`${indent} *`);
            lines.push(`${indent} * @مثال`);
            lines.push(`${indent} *   نفّذ ${info.name} لـ اسم_الصنف`);
            break;
        }
        
        case 'property': {
            lines.push(`${indent} * @خاصية ${info.name} — [وصف الخاصية]`);
            lines.push(`${indent} *`);
            lines.push(`${indent} * @نوع [نوع الخاصية]`);
            break;
        }
    }
    
    lines.push(`${indent} **#`);
    
    return lines.join('\n');
}

/**
 * @brief البحث عن أقرب تعريف قابل للتوثيق حول المؤشر
 * 
 * يبدأ من سطر المؤشر ويبحث لأعلى حتى يجد أول تعريف دالة/صنف/إلخ.
 * إذا كان المؤشر على سطر تعريف، يستخدمه مباشرة.
 * إذا كان داخل جسم دالة، يبحث عن تعريف الدالة المحيطة.
 * 
 * @param document المستند الحالي
 * @param position موقع المؤشر
 * @returns معلومات التعريف أو null إذا لم يُوجد تعريف قريب
 */
function findNearestDefinition(document: vscode.TextDocument, position: vscode.Position): DefinitionInfo | null {
    // أولاً: نتحقق من السطر الحالي
    const currentLine = document.lineAt(position.line).text;
    const currentDef = parseDefinition(currentLine, position.line, document);
    if (currentDef) {
        return currentDef;
    }
    
    // ثانياً: نبحث لأعلى (حتى 50 سطر)
    const maxLookback = Math.min(position.line, 50);
    for (let i = 1; i <= maxLookback; i++) {
        const lineNum = position.line - i;
        const lineText = document.lineAt(lineNum).text;
        const def = parseDefinition(lineText, lineNum, document);
        if (def) {
            return def;
        }
    }
    
    // ثالثاً: نبحث لأسفل (حتى 5 أسطر)
    const maxLookforward = Math.min(document.lineCount - position.line - 1, 5);
    for (let i = 1; i <= maxLookforward; i++) {
        const lineNum = position.line + i;
        const lineText = document.lineAt(lineNum).text;
        const def = parseDefinition(lineText, lineNum, document);
        if (def) {
            return def;
        }
    }
    
    return null;
}

/**
 * @brief التحقق من وجود توثيق مسبق
 * 
 * يتحقق مما إذا كان السطر السابق للتعريف يحتوي بالفعل على
 * كتلة توثيق #** أو ##. إذا وُجد، نعرض رسالة بدلاً من التكرار.
 * 
 * @param document المستند الحالي
 * @param defLine سطر التعريف (0-based)
 * @returns true إذا وُجد توثيق سابق
 */
function hasExistingDoc(document: vscode.TextDocument, defLine: number): boolean {
    if (defLine <= 0) { return false; }
    
    // نفحص الأسطر السابقة (حتى 3 أسطر)
    for (let i = defLine - 1; i >= Math.max(0, defLine - 3); i--) {
        const line = document.lineAt(i).text.trim();
        if (line === '' || line === '') { continue; }
        if (line.includes('**#') || line.startsWith('##')) {
            return true;
        }
        break; // أقرب سطر غير فارغ ليس توثيقاً
    }
    
    return false;
}

/**
 * @brief تسجيل أمر التوثيق التلقائي في VS Code
 * 
 * يسجل الأمر sad.generateDoc الذي يمكن استدعاؤه عبر:
 *   - اختصار Ctrl+Shift+D
 *   - لوحة الأوامر: "Sad: توليد توثيق"
 * 
 * عند الاستدعاء:
 *   1. يبحث عن أقرب تعريف حول المؤشر
 *   2. يتحقق من عدم وجود توثيق مسبق
 *   3. يولد كتلة التوثيق
 *   4. يُدرجها فوق سطر التعريف
 *   5. يضع المؤشر في أول مكان [أضف وصفاً]
 * 
 * @param context سياق الإضافة لتسجيل الاشتراكات
 */
export function registerAutoDoc(context: vscode.ExtensionContext): void {
    const disposable = vscode.commands.registerCommand('sad.generateDoc', async () => {
        const editor = vscode.window.activeTextEditor;
        if (!editor || editor.document.languageId !== 'sad') {
            vscode.window.showWarningMessage('لا يوجد ملف .ص مفتوح');
            return;
        }
        
        const document = editor.document;
        const position = editor.selection.active;
        
        // البحث عن التعريف
        const def = findNearestDefinition(document, position);
        if (!def) {
            vscode.window.showInformationMessage('لم يُعثر على تعريف قابل للتوثيق بالقرب من المؤشر');
            return;
        }
        
        // التحقق من عدم وجود توثيق مسبق
        if (hasExistingDoc(document, def.line)) {
            vscode.window.showInformationMessage(`التعريف "${def.name}" لديه توثيق مسبق بالفعل`);
            return;
        }
        
        // توليد كتلة التوثيق
        const docBlock = generateDocBlock(def);
        
        // إدراج التوثيق فوق سطر التعريف
        await editor.edit((editBuilder: vscode.TextEditorEdit) => {
            const insertPos = new vscode.Position(def.line, 0);
            editBuilder.insert(insertPos, docBlock + '\n');
        });
        
        // تحريك المؤشر إلى أول placeholder
        const docLines = docBlock.split('\n');
        for (let i = 0; i < docLines.length; i++) {
            const placeholder = docLines[i].indexOf('[أضف وصفاً هنا]');
            if (placeholder !== -1) {
                const newPos = new vscode.Position(def.line + i, placeholder);
                const newEnd = new vscode.Position(def.line + i, placeholder + '[أضف وصفاً هنا]'.length);
                editor.selection = new vscode.Selection(newPos, newEnd);
                editor.revealRange(new vscode.Range(newPos, newEnd));
                break;
            }
        }
        
        vscode.window.showInformationMessage(`تم توليد توثيق لـ "${def.name}" ✅`);
    });
    
    context.subscriptions.push(disposable);
}
