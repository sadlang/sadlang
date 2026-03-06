/**
 * @file code_lens.ts
 * @brief عدسات الكود العربية الذكية — Arabic Smart CodeLens
 * 
 * هذا الملف يوفر ميزة CodeLens لعرض معلومات فوق كل دالة وصنف:
 *   - عدد المراجع (كم مرة استُخدم هذا الرمز)
 *   - زر تشغيل ▶ (للدوال القابلة للتشغيل)
 *   - زر تصحيح 🐛 (لبدء جلسة تصحيح)
 *   - عدد الاختبارات المرتبطة
 * 
 * كيف يعمل:
 *   1. CodeLensProvider يمسح المستند بحثاً عن تعريفات الدوال والأصناف
 *   2. لكل تعريف، يُنشئ CodeLens واحد أو أكثر (مرجع، تشغيل، إلخ)
 *   3. عند حل (resolve) كل CodeLens، يعد المراجع الفعلية في المستند
 *   4. الأرقام تُعرض بالعربية (مثلاً: "٣ مراجع")
 * 
 * الأنماط المعبرية المستخدمة:
 *   - دالة: /دالة\s+اسم\s*\(/ 
 *   - صنف: /صنف\s+اسم/
 *   - باني: /باني\s*\(/
 *   - خاصية: /خاصية\s+اسم/
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';

/**
 * @brief تحويل رقم إنجليزي إلى أرقام عربية-هندية
 * 
 * يستبدل كل رقم 0-9 بالرقم العربي المقابل ٠-٩.
 * مثال: 42 → "٤٢"
 * 
 * @param num الرقم الإنجليزي
 * @returns النص بالأرقام العربية
 */
function toArabicNumerals(num: number): string {
    const arabicDigits = ['٠', '١', '٢', '٣', '٤', '٥', '٦', '٧', '٨', '٩'];
    return num.toString().replace(/\d/g, d => arabicDigits[parseInt(d)]);
}

/**
 * @brief معلومات الرمز المكتشف في المستند
 * 
 * يحتوي على اسم الرمز، نوعه، والسطر الذي يقع فيه.
 * تُستخدم لإنشاء CodeLens وعد المراجع.
 */
interface SymbolEntry {
    /** اسم الرمز (مثلاً: "جمع") */
    name: string;
    /** نوع الرمز: دالة، صنف، بنية، إلخ */
    kind: 'function' | 'class' | 'constructor' | 'property' | 'method';
    /** رقم السطر (0-based) */
    line: number;
    /** النطاق الكامل للسطر */
    range: vscode.Range;
}

/**
 * @brief أنماط اكتشاف التعريفات
 * 
 * كل نمط يكتشف نوعاً معيناً من التعريفات.
 * المجموعة الأولى (group 1) تلتقط الاسم دائماً.
 */
const DEFINITION_PATTERNS: Array<{ pattern: RegExp; kind: SymbolEntry['kind'] }> = [
    { pattern: /^[\t ]*(غير_متزامن\s+)?(ساكن\s+)?دالة\s+([\u0600-\u06FF\u0750-\u077F\w]+)\s*\(/, kind: 'function' },
    { pattern: /^[\t ]*صنف\s+([\u0600-\u06FF\u0750-\u077F\w]+)/, kind: 'class' },
    { pattern: /^[\t ]*باني\s*\(/, kind: 'constructor' },
    { pattern: /^[\t ]*خاصية\s+([\u0600-\u06FF\u0750-\u077F\w]+)/, kind: 'property' },
];

/**
 * @brief عد مراجع رمز معين في المستند
 * 
 * يبحث عن ظهور اسم الرمز في المستند بالكامل.
 * يستثني:
 *   - سطر التعريف نفسه
 *   - المراجع داخل التعليقات (# ...)
 *   - المراجع داخل النصوص المقتبسة ("...")
 * 
 * @param document المستند الحالي
 * @param symbolName اسم الرمز المراد عد مراجعه
 * @param definitionLine سطر التعريف (لاستثنائه)
 * @returns عدد المراجع
 */
function countReferences(document: vscode.TextDocument, symbolName: string, definitionLine: number): number {
    let count = 0;
    const text = document.getText();
    
    // بناء نمط بحدود الكلمة (word boundary)
    // نستخدم بحث نصي بسيط مع تحقق يدوي من الحدود
    // لأن \b لا يعمل جيداً مع النصوص العربية
    for (let i = 0; i < document.lineCount; i++) {
        if (i === definitionLine) { continue; } // تخطي سطر التعريف
        
        const line = document.lineAt(i).text;
        
        // تخطي أسطر التعليقات
        const trimmed = line.trimStart();
        if (trimmed.startsWith('#')) { continue; }
        
        // البحث عن الاسم في السطر
        let searchStart = 0;
        while (true) {
            const idx = line.indexOf(symbolName, searchStart);
            if (idx === -1) { break; }
            
            // التحقق من حدود الكلمة
            const before = idx > 0 ? line.charAt(idx - 1) : ' ';
            const after = idx + symbolName.length < line.length ? line.charAt(idx + symbolName.length) : ' ';
            
            // حرف عربي أو _ أو حرف لاتيني يعني أن الاسم جزء من كلمة أكبر
            const isWordChar = (c: string) => /[\u0600-\u06FF\u0750-\u077F\w]/.test(c);
            
            if (!isWordChar(before) && !isWordChar(after)) {
                // التحقق من أننا لسنا داخل نص مقتبس
                const beforeText = line.substring(0, idx);
                const quoteCount = (beforeText.match(/"/g) || []).length;
                if (quoteCount % 2 === 0) { // خارج النص المقتبس
                    count++;
                }
            }
            
            searchStart = idx + symbolName.length;
        }
    }
    
    return count;
}

/**
 * @brief فئة مزود عدسات الكود — CodeLens Provider
 * 
 * تنفذ واجهة vscode.CodeLensProvider لتوفير عدسات الكود.
 * يتم تحديث العدسات عند كل تغيير في المستند.
 * 
 * العدسات المعروضة لكل تعريف:
 *   - "٣ مراجع" — عدد المراجع (يُفتح Find References عند الضغط)
 *   - "▶ تشغيل" — تشغيل الملف (للدوال المسماة رئيسية أو على المستوى الأعلى)
 *   - "📝 توثيق" — توليد توثيق تلقائي
 */
class SadCodeLensProvider implements vscode.CodeLensProvider {
    
    /**
     * @brief حدث تغير CodeLens
     * يُطلق عند حفظ المستند لتحديث العدسات
     */
    private _onDidChangeCodeLenses = new vscode.EventEmitter<void>();
    public readonly onDidChangeCodeLenses = this._onDidChangeCodeLenses.event;
    
    constructor() {
        // تحديث العدسات عند حفظ أي مستند .ص
        vscode.workspace.onDidSaveTextDocument((doc: vscode.TextDocument) => {
            if (doc.languageId === 'sad') {
                this._onDidChangeCodeLenses.fire();
            }
        });
    }
    
    /**
     * @brief توفير عدسات الكود للمستند
     * 
     * يمسح المستند سطراً سطراً، يبحث عن التعريفات،
     * ولكل تعريف يُنشئ عدسات.
     * 
     * @param document المستند المراد مسحه
     * @returns مصفوفة عدسات الكود
     */
    provideCodeLenses(document: vscode.TextDocument): vscode.CodeLens[] {
        const lenses: vscode.CodeLens[] = [];
        const symbols: SymbolEntry[] = [];
        
        // === المرحلة 1: اكتشاف جميع التعريفات ===
        for (let i = 0; i < document.lineCount; i++) {
            const lineText = document.lineAt(i).text;
            const trimmed = lineText.trimStart();
            
            // تخطي الأسطر الفارغة والتعليقات
            if (!trimmed || trimmed.startsWith('#')) { continue; }
            
            for (const { pattern, kind } of DEFINITION_PATTERNS) {
                const match = lineText.match(pattern);
                if (match) {
                    // استخراج الاسم: الدالة في المجموعة 3 (بسبب مجموعات async/static)
                    // الصنف/الخاصية في المجموعة 1
                    let name: string;
                    if (kind === 'function') {
                        name = match[3]; // بعد مجموعات غير_متزامن وساكن
                    } else if (kind === 'constructor') {
                        name = 'باني';
                    } else {
                        name = match[1];
                    }
                    
                    const range = new vscode.Range(i, 0, i, lineText.length);
                    symbols.push({ name, kind, line: i, range });
                    break; // نمط واحد لكل سطر
                }
            }
        }
        
        // === المرحلة 2: إنشاء CodeLens لكل تعريف ===
        for (const sym of symbols) {
            // عدسة المراجع
            const refCount = countReferences(document, sym.name, sym.line);
            const refText = refCount === 0 
                ? 'لا مراجع' 
                : refCount === 1 
                    ? 'مرجع واحد' 
                    : refCount === 2 
                        ? 'مرجعان' 
                        : `${toArabicNumerals(refCount)} مراجع`;
            
            lenses.push(new vscode.CodeLens(sym.range, {
                title: `📎 ${refText}`,
                command: 'editor.action.findReferences',
                arguments: [document.uri, new vscode.Position(sym.line, 0)]
            }));
            
            // عدسة التوثيق
            lenses.push(new vscode.CodeLens(sym.range, {
                title: '📝 توثيق',
                command: 'sad.generateDoc',
            }));
            
            // عدسة التشغيل (للدوال فقط)
            if (sym.kind === 'function') {
                lenses.push(new vscode.CodeLens(sym.range, {
                    title: '▶ تشغيل',
                    command: 'sad.runFile',
                }));
            }
        }
        
        return lenses;
    }
}

/**
 * @brief تسجيل مزود CodeLens في VS Code
 * 
 * يسجل SadCodeLensProvider لملفات لغة ص.
 * يتم تفعيله تلقائياً عند فتح ملف .ص.
 * 
 * @param context سياق الإضافة لتسجيل الاشتراكات
 */
export function registerCodeLens(context: vscode.ExtensionContext): void {
    const provider = new SadCodeLensProvider();
    const disposable = vscode.languages.registerCodeLensProvider(
        { language: 'sad', scheme: 'file' },
        provider
    );
    context.subscriptions.push(disposable);
}
