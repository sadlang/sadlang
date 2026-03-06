/**
 * @file code_translator.ts
 * @brief محوّل الكود الذكي — Python ↔ ص Code Translator
 * 
 * هذا الملف يوفر تحويل ثنائي الاتجاه بين Python ولغة ص.
 * التحويل يعتمد على قواعد ثابتة (Rule-based) وليس ذكاء اصطناعي،
 * مما يجعله سريعاً ودقيقاً وقابلاً للتوقع.
 * 
 * === اتجاهات التحويل ===
 * 
 * 1. Python → ص:
 *    - def → دالة
 *    - class → صنف  
 *    - if/elif/else → إذا/وإلا إذا/وإلا
 *    - while → بينما
 *    - for x in y → لكل x في y
 *    - return → ارجع
 *    - True/False/None → صحيح/خطأ/لاشيء
 *    - print() → اطبع_سطر()
 *    - self → هذا
 *    - import → استورد
 *    - إضافة نهاية لكل كتلة
 * 
 * 2. ص → Python:
 *    - العكس من أعلاه
 *    - إزالة نهاية واستبدالها بمسافة بادئة
 * 
 * === الخوارزمية ===
 * 
 * 1. تحليل سطر بسطر
 * 2. تحديد مستوى المسافة البادئة
 * 3. تطبيق قواعد الاستبدال حسب السياق
 * 4. إعادة بناء المسافات البادئة
 * 5. إضافة/إزالة نهاية حسب الاتجاه
 * 
 * ملاحظة: التحويل ليس مثالياً لكل الحالات — هو نقطة بداية
 * يحتاج المبرمج لمراجعة وتعديل النتيجة.
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';

/**
 * @brief جدول تحويل الكلمات المفتاحية من Python إلى ص
 * 
 * كل صف: [نمط Python, البديل في ص]
 * الأنماط مرتبة من الأطول إلى الأقصر لتجنب الاستبدال الجزئي.
 */
const PY_TO_SAD_KEYWORDS: Array<[RegExp, string]> = [
    // === كلمات مفتاحية أساسية ===
    [/\bdef\s+/g, 'دالة '],
    [/\bclass\s+/g, 'صنف '],
    [/\belif\s+/g, 'وإلا إذا '],
    [/\belse\s*:/g, 'وإلا'],
    [/\bif\s+/g, 'إذا '],
    [/\bwhile\s+/g, 'بينما '],
    [/\bfor\s+/g, 'لكل '],
    [/\bin\s+/g, 'في '],
    [/\breturn\b/g, 'ارجع'],
    [/\bpass\b/g, '# فارغ'],
    [/\bbreak\b/g, 'توقف'],
    [/\bcontinue\b/g, 'استمر'],
    
    // === القيم ===
    [/\bTrue\b/g, 'صحيح'],
    [/\bFalse\b/g, 'خطأ'],
    [/\bNone\b/g, 'لاشيء'],
    
    // === العوامل المنطقية ===
    [/\band\b/g, 'و'],
    [/\bor\b/g, 'أو'],
    [/\bnot\b/g, 'ليس'],
    
    // === الدوال المدمجة ===
    [/\bprint\s*\(/g, 'اطبع_سطر('],
    [/\blen\s*\(/g, 'طول('],
    [/\brange\s*\(/g, 'نطاق('],
    [/\binput\s*\(/g, 'أدخل('],
    [/\bstr\s*\(/g, 'نص('],
    [/\bint\s*\(/g, 'رقم('],
    [/\bfloat\s*\(/g, 'عشري('],
    
    // === OOP ===
    [/\bself\./g, 'هذا.'],
    [/\bself\b/g, 'هذا'],
    [/\bsuper\(\)/g, 'الأساس()'],
    [/\b__init__\s*\(/g, 'باني('],
    
    // === الاستيراد ===
    [/\bimport\s+/g, 'استورد '],
    [/\bfrom\s+/g, 'من '],
    [/\bas\s+/g, 'كـ '],
    
    // === الاستثناءات ===
    [/\btry\s*:/g, 'حاول'],
    [/\bexcept\b/g, 'امسك'],
    [/\bfinally\s*:/g, 'أخيراً'],
    [/\braise\b/g, 'ارمي'],
    
    // === الأنواع ===
    [/\blist\b/g, 'مصفوفة'],
    [/\bdict\b/g, 'خريطة'],
    [/\bbool\b/g, 'منطقي'],
];

/**
 * @brief جدول تحويل الكلمات المفتاحية من ص إلى Python
 */
const SAD_TO_PY_KEYWORDS: Array<[RegExp, string]> = [
    // === الكلمات المفتاحية ===
    [/\bدالة\s+/g, 'def '],
    [/\bصنف\s+/g, 'class '],
    [/\bوإلا\s+إذا\s+/g, 'elif '],
    [/\bوالا\s+اذا\s+/g, 'elif '],
    [/\bوإلا\b/g, 'else:'],
    [/\bوالا\b/g, 'else:'],
    [/\bإذا\s+/g, 'if '],
    [/\bاذا\s+/g, 'if '],
    [/\bبينما\s+/g, 'while '],
    [/\bلكل\s+/g, 'for '],
    [/\bفي\s+/g, 'in '],
    [/\bارجع\b/g, 'return'],
    [/\bتوقف\b/g, 'break'],
    [/\bاستمر\b/g, 'continue'],
    
    // === القيم ===
    [/\bصحيح\b/g, 'True'],
    [/\bخطأ\b/g, 'False'],
    [/\bلاشيء\b/g, 'None'],
    
    // === العوامل ===
    [/\bو\b/g, 'and'],
    [/\bأو\b/g, 'or'],
    [/\bليس\b/g, 'not'],
    
    // === الدوال ===
    [/\bاطبع_سطر\s*\(/g, 'print('],
    [/\bاطبع\s*\(/g, 'print('],
    [/\bطول\s*\(/g, 'len('],
    [/\bأدخل\s*\(/g, 'input('],
    
    // === OOP ===
    [/\bهذا\./g, 'self.'],
    [/\bهذا\b/g, 'self'],
    [/\bالأساس\(\)/g, 'super()'],
    [/\bباني\s*\(/g, '__init__('],
    [/\bجديد\s+/g, ''],
    
    // === الاستيراد ===
    [/\bاستورد\s+/g, 'import '],
    [/\bمن\s+/g, 'from '],
    [/\bكـ\s+/g, 'as '],
    
    // === الاستثناءات ===
    [/\bحاول\b/g, 'try:'],
    [/\bامسك\b/g, 'except'],
    [/\bأخيراً\b/g, 'finally:'],
    [/\bارمي\b/g, 'raise'],
    
    // === المتغيرات ===
    [/\bمتغير\s+/g, ''],
    [/\bثابت\s+/g, ''],
];

/**
 * @brief تحويل كود Python إلى كود لغة ص
 * 
 * خوارزمية التحويل:
 *   1. تقسيم النص إلى أسطر
 *   2. تحليل المسافة البادئة لكل سطر
 *   3. إزالة النقطتين : من نهاية الأسطر التي تبدأ كتل
 *   4. تطبيق جدول الاستبدال PY_TO_SAD_KEYWORDS
 *   5. إضافة نهاية في نهاية كل كتلة
 *   6. إضافة أقواس حول الشروط
 * 
 * @param pythonCode نص كود Python
 * @returns نص كود لغة ص
 */
function pythonToSad(pythonCode: string): string {
    const lines = pythonCode.split('\n');
    const result: string[] = [];
    
    // مكدس المسافات البادئة — لتتبع فتح/إغلاق الكتل
    const indentStack: number[] = [0];
    
    for (let i = 0; i < lines.length; i++) {
        let line = lines[i];
        const trimmed = line.trimStart();
        
        // تخطي الأسطر الفارغة
        if (!trimmed) {
            result.push('');
            continue;
        }
        
        // حساب المسافة البادئة الحالية
        const indent = line.length - trimmed.length;
        
        // إغلاق الكتل: إذا قلت المسافة البادئة، نضيف نهاية
        while (indentStack.length > 1 && indent < indentStack[indentStack.length - 1]) {
            indentStack.pop();
            const endIndent = ' '.repeat(indentStack[indentStack.length - 1]);
            result.push(`${endIndent}نهاية`);
        }
        
        // تحويل التعليقات: # في Python = # في ص
        if (trimmed.startsWith('#')) {
            result.push(line);
            continue;
        }
        
        // === تطبيق الاستبدالات ===
        let converted = line;
        
        // إزالة : من نهاية السطر (لغة Python تستخدمها، لغة ص لا)
        const endsWithColon = trimmed.endsWith(':');
        if (endsWithColon) {
            converted = converted.replace(/:\s*$/, '');
        }
        
        // تطبيق جدول التحويل
        for (const [pattern, replacement] of PY_TO_SAD_KEYWORDS) {
            converted = converted.replace(pattern, replacement);
        }
        
        // إضافة أقواس حول الشروط (إذا/بينما)
        const condMatch = converted.match(/^(\s*)(إذا|بينما|وإلا إذا)\s+(.+)$/);
        if (condMatch) {
            const condIndent = condMatch[1];
            const keyword = condMatch[2];
            const condition = condMatch[3].trim();
            converted = `${condIndent}${keyword} (${condition})`;
        }
        
        // تحويل تعريف الدالة: إضافة أقواس إذا لم تكن موجودة
        const funcMatch = converted.match(/^(\s*)دالة\s+([\u0600-\u06FF\u0750-\u077F\w]+)\s*\(([^)]*)\)\s*$/);
        if (funcMatch) {
            // الدالة بصيغة صحيحة بالفعل
        }
        
        // تحويل تعريف الصنف: إزالة () إذا كانت فارغة أو إضافة يرث
        const classMatch = converted.match(/^(\s*)صنف\s+([\u0600-\u06FF\u0750-\u077F\w]+)\s*\(([^)]*)\)/);
        if (classMatch && classMatch[3].trim()) {
            const cls = classMatch[2];
            const parent = classMatch[3].trim();
            converted = `${classMatch[1]}صنف ${cls} يرث ${parent}`;
        } else if (classMatch) {
            converted = `${classMatch[1]}صنف ${classMatch[2]}`;
        }
        
        result.push(converted);
        
        // فتح كتلة جديدة إذا كان السطر الأصلي ينتهي بـ :
        if (endsWithColon && !trimmed.startsWith('#')) {
            indentStack.push(indent + 4); // نتوقع مسافة 4
        }
    }
    
    // إغلاق جميع الكتل المفتوحة
    while (indentStack.length > 1) {
        indentStack.pop();
        const endIndent = ' '.repeat(indentStack[indentStack.length - 1]);
        result.push(`${endIndent}نهاية`);
    }
    
    return result.join('\n');
}

/**
 * @brief تحويل كود لغة ص إلى Python
 * 
 * خوارزمية التحويل:
 *   1. تقسيم النص إلى أسطر
 *   2. إزالة أسطر نهاية (لا يوجد ما يقابلها في Python)
 *   3. تطبيق جدول الاستبدال SAD_TO_PY_KEYWORDS
 *   4. إضافة : لنهاية أسطر فتح الكتل
 *   5. إزالة الأقواس حول الشروط
 * 
 * @param sadCode نص كود لغة ص
 * @returns نص كود Python
 */
function sadToPython(sadCode: string): string {
    const lines = sadCode.split('\n');
    const result: string[] = [];
    
    for (let i = 0; i < lines.length; i++) {
        let line = lines[i];
        const trimmed = line.trimStart();
        
        // تخطي الأسطر الفارغة
        if (!trimmed) {
            result.push('');
            continue;
        }
        
        // تخطي أسطر نهاية — لا يوجد ما يقابلها في Python
        if (trimmed === 'نهاية') {
            continue;
        }
        
        // تحويل التعليقات
        if (trimmed.startsWith('#')) {
            result.push(line);
            continue;
        }
        
        // === تطبيق الاستبدالات ===
        let converted = line;
        
        for (const [pattern, replacement] of SAD_TO_PY_KEYWORDS) {
            converted = converted.replace(pattern, replacement);
        }
        
        // إزالة الأقواس من الشروط
        const condMatch = converted.match(/^(\s*)(if|while|elif)\s+\((.+)\)\s*$/);
        if (condMatch) {
            converted = `${condMatch[1]}${condMatch[2]} ${condMatch[3]}:`;
        }
        
        // إضافة : لأسطر فتح الكتل
        const blockPattern = /^(\s*)(def|class|if|elif|else|while|for|try|except|finally)\b/;
        if (blockPattern.test(converted) && !converted.trimEnd().endsWith(':')) {
            converted = converted.trimEnd() + ':';
        }
        
        // تحويل __init__(self, ...) — إضافة self كأول معامل
        const initMatch = converted.match(/^(\s*)def\s+__init__\s*\(([^)]*)\)/);
        if (initMatch) {
            const params = initMatch[2].trim();
            if (params && !params.startsWith('self')) {
                converted = converted.replace(`(${params})`, `(self, ${params})`);
            } else if (!params) {
                converted = converted.replace('()', '(self)');
            }
        }
        
        // تحويل الدوال العادية داخل الصنف — إضافة self
        // (هذا تبسيط — التحويل الحقيقي يحتاج تحليل AST)
        const methodMatch = converted.match(/^(\s{4,})def\s+([\w]+)\s*\(([^)]*)\)/);
        if (methodMatch) {
            const params = methodMatch[3].trim();
            if (params && !params.startsWith('self')) {
                converted = converted.replace(`(${params})`, `(self, ${params})`);
            } else if (!params) {
                converted = converted.replace('()', '(self)');
            }
        }
        
        result.push(converted);
    }
    
    return result.join('\n');
}

/**
 * @brief اكتشاف لغة النص تلقائياً
 * 
 * يحلل النص لتحديد ما إذا كان Python أو لغة ص.
 * يعتمد على وجود كلمات مفتاحية مميزة لكل لغة.
 * 
 * @param code النص المراد تحليله
 * @returns 'python' أو 'sad' أو 'unknown'
 */
function detectLanguage(code: string): 'python' | 'sad' | 'unknown' {
    // عدّادات الكلمات المفتاحية لكل لغة
    const pythonKeywords = ['def ', 'class ', 'import ', 'print(', 'self.', 'True', 'False', 'None', 'elif '];
    const sadKeywords = ['دالة ', 'صنف ', 'استورد ', 'اطبع', 'هذا.', 'صحيح', 'خطأ', 'لاشيء', 'نهاية', 'متغير ', 'ثابت '];
    
    let pyCount = 0;
    let sadCount = 0;
    
    for (const kw of pythonKeywords) {
        if (code.includes(kw)) { pyCount++; }
    }
    
    for (const kw of sadKeywords) {
        if (code.includes(kw)) { sadCount++; }
    }
    
    if (pyCount > sadCount) { return 'python'; }
    if (sadCount > pyCount) { return 'sad'; }
    return 'unknown';
}

/**
 * @brief تسجيل أوامر المحوّل في VS Code
 * 
 * يسجل الأوامر التالية:
 *   - sad.translateToSad — تحويل Python إلى ص
 *   - sad.translateToPython — تحويل ص إلى Python
 *   - sad.translateAuto — اكتشاف تلقائي + تحويل
 * 
 * @param context سياق الإضافة
 */
export function registerCodeTranslator(context: vscode.ExtensionContext): void {
    // === أمر: تحويل إلى ص ===
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.translateToSad', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showWarningMessage('لا يوجد محرر نشط');
                return;
            }
            
            const selection = editor.selection;
            const text = selection.isEmpty 
                ? editor.document.getText() 
                : editor.document.getText(selection);
            
            const translated = pythonToSad(text);
            
            // فتح المستند المترجم في محرر جديد
            const doc = await vscode.workspace.openTextDocument({
                language: 'sad',
                content: `# تم التحويل من Python إلى لغة ص\n# يرجى مراجعة الكود والتعديل حسب الحاجة\n\n${translated}`
            });
            await vscode.window.showTextDocument(doc, vscode.ViewColumn.Beside);
            
            vscode.window.showInformationMessage('تم تحويل الكود من Python إلى لغة ص ✅');
        })
    );
    
    // === أمر: تحويل إلى Python ===
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.translateToPython', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showWarningMessage('لا يوجد محرر نشط');
                return;
            }
            
            const selection = editor.selection;
            const text = selection.isEmpty 
                ? editor.document.getText() 
                : editor.document.getText(selection);
            
            const translated = sadToPython(text);
            
            const doc = await vscode.workspace.openTextDocument({
                language: 'python',
                content: `# Translated from Sad Language (لغة ص) to Python\n# Please review and adjust as needed\n\n${translated}`
            });
            await vscode.window.showTextDocument(doc, vscode.ViewColumn.Beside);
            
            vscode.window.showInformationMessage('تم تحويل الكود من لغة ص إلى Python ✅');
        })
    );
    
    // === أمر: تحويل تلقائي (يكتشف اللغة) ===
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.translateAuto', async () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor) {
                vscode.window.showWarningMessage('لا يوجد محرر نشط');
                return;
            }
            
            const text = editor.document.getText();
            const lang = detectLanguage(text);
            
            if (lang === 'python') {
                await vscode.commands.executeCommand('sad.translateToSad');
            } else if (lang === 'sad') {
                await vscode.commands.executeCommand('sad.translateToPython');
            } else {
                const choice = await vscode.window.showQuickPick(
                    ['تحويل إلى ص (من Python)', 'تحويل إلى Python (من ص)'],
                    { placeHolder: 'لم أستطع تحديد اللغة تلقائياً. اختر اتجاه التحويل:' }
                );
                
                if (choice?.includes('ص (من Python)')) {
                    await vscode.commands.executeCommand('sad.translateToSad');
                } else if (choice?.includes('Python')) {
                    await vscode.commands.executeCommand('sad.translateToPython');
                }
            }
        })
    );
}
