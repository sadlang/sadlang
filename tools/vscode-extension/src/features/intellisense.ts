/**
 * @file intellisense.ts
 * @brief نظام الإكمال الذكي المتقدم — Advanced IntelliSense Provider
 * 
 * يوفر إكمال تلقائي متقدم، تلميحات عند التمرير، ومساعدة في التوقيعات
 */

import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';

// ─── قاعدة بيانات الكلمات المفتاحية والدوال ───

interface CompletionEntry {
    label: string;
    kind: vscode.CompletionItemKind;
    detail: string;
    documentation: string;
    insertText: string;
    sortGroup: number; // 0=highest priority
}

const KEYWORD_COMPLETIONS: CompletionEntry[] = [
    // كلمات التحكم
    { label: 'إذا', kind: vscode.CompletionItemKind.Keyword, detail: 'شرط — if', documentation: 'كتلة شرطية\n```sad\nإذا (شرط)\n   # كود\nنهاية\n```', insertText: 'إذا (${1:شرط})\n\t${2}\nنهاية', sortGroup: 0 },
    { label: 'وإلا', kind: vscode.CompletionItemKind.Keyword, detail: 'وإلا — else', documentation: 'الفرع البديل في الشرط', insertText: 'وإلا\n\t${1}', sortGroup: 0 },
    { label: 'بينما', kind: vscode.CompletionItemKind.Keyword, detail: 'حلقة — while', documentation: 'حلقة تكرارية\n```sad\nبينما (شرط)\n   # كود\nنهاية\n```', insertText: 'بينما (${1:شرط})\n\t${2}\nنهاية', sortGroup: 0 },
    { label: 'لكل', kind: vscode.CompletionItemKind.Keyword, detail: 'حلقة — for each', documentation: 'حلقة لكل عنصر\n```sad\nلكل عنصر في المصفوفة\n   اطبع(عنصر)\nنهاية\n```', insertText: 'لكل ${1:عنصر} في ${2:القائمة}\n\t${3}\nنهاية', sortGroup: 0 },
    { label: 'طابق', kind: vscode.CompletionItemKind.Keyword, detail: 'مطابقة — match', documentation: 'مطابقة أنماط\n```sad\nطابق (قيمة)\n   عندما 1\n      اطبع("واحد")\n   افتراضي\n      اطبع("آخر")\nنهاية\n```', insertText: 'طابق (${1:قيمة})\n\tعندما ${2:نمط}\n\t\t${3}\n\tافتراضي\n\t\t${4}\nنهاية', sortGroup: 0 },
    { label: 'عندما', kind: vscode.CompletionItemKind.Keyword, detail: 'حالة — when', documentation: 'حالة داخل طابق', insertText: 'عندما ${1:نمط}\n\t${2}', sortGroup: 0 },
    { label: 'افتراضي', kind: vscode.CompletionItemKind.Keyword, detail: 'الافتراضي — default', documentation: 'الحالة الافتراضية في طابق', insertText: 'افتراضي\n\t${1}', sortGroup: 0 },
    { label: 'حاول', kind: vscode.CompletionItemKind.Keyword, detail: 'معالجة أخطاء — try', documentation: 'كتلة معالجة الأخطاء\n```sad\nحاول\n   # كود\nامسك (خطأ)\n   اطبع(خطأ)\nأخيراً\n   # تنظيف\nنهاية\n```', insertText: 'حاول\n\t${1}\nامسك (${2:خطأ})\n\t${3}\nنهاية', sortGroup: 0 },
    { label: 'امسك', kind: vscode.CompletionItemKind.Keyword, detail: 'التقاط خطأ — catch', documentation: 'التقاط الخطأ من كتلة حاول', insertText: 'امسك (${1:خطأ})\n\t${2}', sortGroup: 0 },
    { label: 'ارمي', kind: vscode.CompletionItemKind.Keyword, detail: 'رمي خطأ — throw', documentation: 'رمي خطأ أو استثناء', insertText: 'ارمي ${1:"رسالة الخطأ"}', sortGroup: 0 },
    { label: 'أخيراً', kind: vscode.CompletionItemKind.Keyword, detail: 'دائماً — finally', documentation: 'كتلة تُنفذ دائماً بعد حاول/امسك', insertText: 'أخيراً\n\t${1}', sortGroup: 0 },
    { label: 'ارجع', kind: vscode.CompletionItemKind.Keyword, detail: 'إرجاع — return', documentation: 'إرجاع قيمة من الدالة', insertText: 'ارجع ${1}', sortGroup: 0 },
    { label: 'توقف', kind: vscode.CompletionItemKind.Keyword, detail: 'إيقاف حلقة — break', documentation: 'الخروج من الحلقة الحالية', insertText: 'توقف', sortGroup: 0 },
    { label: 'استمر', kind: vscode.CompletionItemKind.Keyword, detail: 'تخطي — continue', documentation: 'تخطي إلى الدورة التالية', insertText: 'استمر', sortGroup: 0 },
    { label: 'نهاية', kind: vscode.CompletionItemKind.Keyword, detail: 'نهاية كتلة — end', documentation: 'إنهاء كتلة (دالة/صنف/إذا/بينما/لكل...)', insertText: 'نهاية', sortGroup: 0 },

    // التعريفات
    { label: 'دالة', kind: vscode.CompletionItemKind.Keyword, detail: 'تعريف دالة — function', documentation: 'تعريف دالة جديدة\n```sad\nدالة اسم(معامل1، معامل2)\n   # كود\n   ارجع نتيجة\nنهاية\n```', insertText: 'دالة ${1:اسم}(${2:معاملات})\n\t${3}\nنهاية', sortGroup: 0 },
    { label: 'صنف', kind: vscode.CompletionItemKind.Keyword, detail: 'تعريف صنف — class', documentation: 'تعريف صنف جديد\n```sad\nصنف اسم\n   باني(معامل)\n      هذا.خاصية = معامل\n   نهاية\nنهاية\n```', insertText: 'صنف ${1:اسم}\n\tباني(${2:معاملات})\n\t\t${3}\n\تنهاية\nنهاية', sortGroup: 0 },
    { label: 'بنية', kind: vscode.CompletionItemKind.Keyword, detail: 'تعريف بنية — struct', documentation: 'تعريف بنية بيانات', insertText: 'بنية ${1:اسم}\n\t${2}\nنهاية', sortGroup: 0 },
    { label: 'تعداد', kind: vscode.CompletionItemKind.Keyword, detail: 'تعريف تعداد — enum', documentation: 'تعريف نوع تعداد', insertText: 'تعداد ${1:اسم}\n\t${2}\nنهاية', sortGroup: 0 },

    // OOP
    { label: 'يرث', kind: vscode.CompletionItemKind.Keyword, detail: 'وراثة — extends', documentation: 'وراثة من صنف آخر\n```sad\nصنف ابن يرث أب\nnنهاية\n```', insertText: 'يرث ${1:الأساس}', sortGroup: 0 },
    { label: 'جديد', kind: vscode.CompletionItemKind.Keyword, detail: 'إنشاء كائن — new', documentation: 'إنشاء كائن جديد من صنف', insertText: 'جديد ${1:صنف}(${2})', sortGroup: 0 },
    { label: 'هذا', kind: vscode.CompletionItemKind.Keyword, detail: 'المرجع الذاتي — this', documentation: 'مرجع للكائن الحالي', insertText: 'هذا.${1}', sortGroup: 0 },
    { label: 'باني', kind: vscode.CompletionItemKind.Keyword, detail: 'منشئ — constructor', documentation: 'منشئ الصنف (يُستدعى عند جديد)', insertText: 'باني(${1:معاملات})\n\t${2}\nنهاية', sortGroup: 0 },
    { label: 'الأساس', kind: vscode.CompletionItemKind.Keyword, detail: 'الأساس — super', documentation: 'استدعاء الصنف الأساسي', insertText: 'الأساس.${1}', sortGroup: 0 },

    // المتغيرات
    { label: 'متغير', kind: vscode.CompletionItemKind.Keyword, detail: 'تعريف متغير — var', documentation: 'تعريف متغير قابل للتغيير\n```sad\nمتغير اسم = قيمة\n```', insertText: 'متغير ${1:اسم} = ${2:قيمة}', sortGroup: 0 },
    { label: 'ثابت', kind: vscode.CompletionItemKind.Keyword, detail: 'تعريف ثابت — const', documentation: 'تعريف ثابت لا يتغير\n```sad\nثابت PI = 3.14159\n```', insertText: 'ثابت ${1:اسم} = ${2:قيمة}', sortGroup: 0 },
    { label: 'ساكن', kind: vscode.CompletionItemKind.Keyword, detail: 'عضو ساكن — static', documentation: 'عضو ثابت مشترك بين جميع الكائنات', insertText: 'ساكن ${1}', sortGroup: 0 },
    { label: 'خارجي', kind: vscode.CompletionItemKind.Keyword, detail: 'تعريف خارجي — extern', documentation: 'ربط مع كود خارجي', insertText: 'خارجي ${1}', sortGroup: 0 },

    // استيراد
    { label: 'استورد', kind: vscode.CompletionItemKind.Keyword, detail: 'استيراد — import', documentation: 'استيراد وحدة\n```sad\nاستورد "ملف.ص"\nاستورد اسم من "ملف.ص"\n```', insertText: 'استورد ${1:"ملف.ص"}', sortGroup: 0 },
    { label: 'من', kind: vscode.CompletionItemKind.Keyword, detail: 'من — from', documentation: 'استيراد محدد من ملف', insertText: 'من ${1}', sortGroup: 0 },
    { label: 'صدّر', kind: vscode.CompletionItemKind.Keyword, detail: 'تصدير — export', documentation: 'تصدير دالة أو صنف للاستخدام الخارجي', insertText: 'صدّر ${1}', sortGroup: 0 },
    { label: 'كـ', kind: vscode.CompletionItemKind.Keyword, detail: 'تسمية بديلة — as', documentation: 'إعادة تسمية عند الاستيراد', insertText: 'كـ ${1:اسم}', sortGroup: 0 },

    // صلاحيات
    { label: 'عام', kind: vscode.CompletionItemKind.Keyword, detail: 'عام — public', documentation: 'وصول عام من أي مكان', insertText: 'عام', sortGroup: 1 },
    { label: 'خاص', kind: vscode.CompletionItemKind.Keyword, detail: 'خاص — private', documentation: 'وصول خاص داخل الصنف فقط', insertText: 'خاص', sortGroup: 1 },
    { label: 'محمي', kind: vscode.CompletionItemKind.Keyword, detail: 'محمي — protected', documentation: 'وصول محمي للصنف والأبناء', insertText: 'محمي', sortGroup: 1 },
    { label: 'مجرد', kind: vscode.CompletionItemKind.Keyword, detail: 'مجرد — abstract', documentation: 'صنف أو دالة مجردة يجب تنفيذها في الصنف الابن', insertText: 'مجرد', sortGroup: 1 },

    // ثوابت اللغة
    { label: 'صحيح', kind: vscode.CompletionItemKind.Value, detail: 'صحيح — true', documentation: 'القيمة المنطقية: صحيح', insertText: 'صحيح', sortGroup: 1 },
    { label: 'خطأ', kind: vscode.CompletionItemKind.Value, detail: 'خطأ — false', documentation: 'القيمة المنطقية: خطأ', insertText: 'خطأ', sortGroup: 1 },
    { label: 'لاشيء', kind: vscode.CompletionItemKind.Value, detail: 'لاشيء — null', documentation: 'القيمة الفارغة', insertText: 'لاشيء', sortGroup: 1 },

    // العوامل المنطقية
    { label: 'و', kind: vscode.CompletionItemKind.Operator, detail: 'و — AND (&&)', documentation: 'عامل منطقي: و', insertText: 'و', sortGroup: 2 },
    { label: 'أو', kind: vscode.CompletionItemKind.Operator, detail: 'أو — OR (||)', documentation: 'عامل منطقي: أو', insertText: 'أو', sortGroup: 2 },
    { label: 'ليس', kind: vscode.CompletionItemKind.Operator, detail: 'ليس — NOT (!)', documentation: 'عامل منطقي: ليس', insertText: 'ليس', sortGroup: 2 },
    { label: 'في', kind: vscode.CompletionItemKind.Operator, detail: 'عضوية — in', documentation: 'عامل عضوية: هل العنصر في المجموعة', insertText: 'في', sortGroup: 2 },
];

const BUILTIN_FUNCTIONS: CompletionEntry[] = [
    // IO
    { label: 'اطبع', kind: vscode.CompletionItemKind.Function, detail: 'طباعة — print', documentation: 'طباعة قيمة بدون سطر جديد\n```sad\nاطبع("مرحباً")\n```', insertText: 'اطبع(${1})', sortGroup: 0 },
    { label: 'اطبع_سطر', kind: vscode.CompletionItemKind.Function, detail: 'طباعة مع سطر — println', documentation: 'طباعة قيمة مع سطر جديد\n```sad\nاطبع_سطر("مرحباً بالعالم")\n```', insertText: 'اطبع_سطر(${1})', sortGroup: 0 },
    { label: 'ادخل', kind: vscode.CompletionItemKind.Function, detail: 'إدخال — input', documentation: 'قراءة مدخلات من المستخدم\n```sad\nمتغير اسم = ادخل("أدخل اسمك: ")\n```', insertText: 'ادخل(${1:"رسالة: "})', sortGroup: 0 },
    { label: 'ادخل_رقم', kind: vscode.CompletionItemKind.Function, detail: 'إدخال رقم — input_number', documentation: 'قراءة رقم من المستخدم', insertText: 'ادخل_رقم(${1:"أدخل رقماً: "})', sortGroup: 0 },

    // تحويل الأنواع
    { label: 'نص', kind: vscode.CompletionItemKind.Function, detail: 'تحويل لنص — str()', documentation: 'تحويل قيمة إلى نص', insertText: 'نص(${1})', sortGroup: 1 },
    { label: 'رقم', kind: vscode.CompletionItemKind.Function, detail: 'تحويل لرقم — int()', documentation: 'تحويل قيمة إلى رقم صحيح', insertText: 'رقم(${1})', sortGroup: 1 },
    { label: 'عشري', kind: vscode.CompletionItemKind.Function, detail: 'تحويل لعشري — float()', documentation: 'تحويل قيمة إلى عدد عشري', insertText: 'عشري(${1})', sortGroup: 1 },

    // النص
    { label: 'طول', kind: vscode.CompletionItemKind.Function, detail: 'طول — length', documentation: 'الحصول على طول نص أو مصفوفة', insertText: 'طول(${1})', sortGroup: 1 },
    { label: 'قسّم', kind: vscode.CompletionItemKind.Function, detail: 'تقسيم نص — split', documentation: 'تقسيم نص إلى مصفوفة', insertText: 'قسّم(${1:نص}، ${2:فاصل})', sortGroup: 1 },
    { label: 'ادمج', kind: vscode.CompletionItemKind.Function, detail: 'دمج — join', documentation: 'دمج مصفوفة إلى نص', insertText: 'ادمج(${1:مصفوفة}، ${2:فاصل})', sortGroup: 1 },
    { label: 'استبدل', kind: vscode.CompletionItemKind.Function, detail: 'استبدال — replace', documentation: 'استبدال نص فرعي', insertText: 'استبدل(${1:نص}، ${2:قديم}، ${3:جديد})', sortGroup: 1 },

    // المصفوفة
    { label: 'أضف', kind: vscode.CompletionItemKind.Function, detail: 'إضافة عنصر — push', documentation: 'إضافة عنصر لنهاية المصفوفة', insertText: 'أضف(${1:مصفوفة}، ${2:قيمة})', sortGroup: 1 },
    { label: 'احذف', kind: vscode.CompletionItemKind.Function, detail: 'حذف عنصر — remove', documentation: 'حذف عنصر من مصفوفة بالفهرس', insertText: 'احذف(${1:مصفوفة}، ${2:فهرس})', sortGroup: 1 },
    { label: 'رتّب', kind: vscode.CompletionItemKind.Function, detail: 'ترتيب — sort', documentation: 'ترتيب المصفوفة', insertText: 'رتّب(${1:مصفوفة})', sortGroup: 1 },
    { label: 'عكس', kind: vscode.CompletionItemKind.Function, detail: 'عكس — reverse', documentation: 'عكس ترتيب المصفوفة', insertText: 'عكس(${1:مصفوفة})', sortGroup: 1 },

    // الرياضيات
    { label: 'جذر', kind: vscode.CompletionItemKind.Function, detail: 'جذر تربيعي — sqrt', documentation: 'حساب الجذر التربيعي', insertText: 'جذر(${1:رقم})', sortGroup: 1 },
    { label: 'مطلق', kind: vscode.CompletionItemKind.Function, detail: 'القيمة المطلقة — abs', documentation: 'القيمة المطلقة لرقم', insertText: 'مطلق(${1:رقم})', sortGroup: 1 },
    { label: 'أقصى', kind: vscode.CompletionItemKind.Function, detail: 'الأقصى — max', documentation: 'أكبر قيمة', insertText: 'أقصى(${1:أ}، ${2:ب})', sortGroup: 1 },
    { label: 'أدنى', kind: vscode.CompletionItemKind.Function, detail: 'الأدنى — min', documentation: 'أصغر قيمة', insertText: 'أدنى(${1:أ}، ${2:ب})', sortGroup: 1 },
    { label: 'عشوائي', kind: vscode.CompletionItemKind.Function, detail: 'رقم عشوائي — random', documentation: 'توليد رقم عشوائي', insertText: 'عشوائي(${1:حد_أدنى}، ${2:حد_أقصى})', sortGroup: 1 },

    // الملفات
    { label: 'اقرأ_ملف', kind: vscode.CompletionItemKind.Function, detail: 'قراءة ملف — read_file', documentation: 'قراءة محتوى ملف نصي', insertText: 'اقرأ_ملف(${1:"مسار.ص"})', sortGroup: 1 },
    { label: 'اكتب_ملف', kind: vscode.CompletionItemKind.Function, detail: 'كتابة ملف — write_file', documentation: 'كتابة محتوى في ملف', insertText: 'اكتب_ملف(${1:"مسار"}، ${2:محتوى})', sortGroup: 1 },

    // النوع
    { label: 'نوع', kind: vscode.CompletionItemKind.Function, detail: 'نوع القيمة — typeof', documentation: 'الحصول على نوع القيمة كنص', insertText: 'نوع(${1:قيمة})', sortGroup: 1 },
];

const TYPE_COMPLETIONS: CompletionEntry[] = [
    { label: 'رقم', kind: vscode.CompletionItemKind.TypeParameter, detail: 'نوع: عدد صحيح — integer', documentation: 'نوع الأعداد الصحيحة (64-bit)', insertText: 'رقم', sortGroup: 1 },
    { label: 'عشري', kind: vscode.CompletionItemKind.TypeParameter, detail: 'نوع: عدد عشري — float', documentation: 'نوع الأعداد العشرية (64-bit)', insertText: 'عشري', sortGroup: 1 },
    { label: 'نص', kind: vscode.CompletionItemKind.TypeParameter, detail: 'نوع: نص — string', documentation: 'نوع السلاسل النصية (UTF-8)', insertText: 'نص', sortGroup: 1 },
    { label: 'منطقي', kind: vscode.CompletionItemKind.TypeParameter, detail: 'نوع: منطقي — boolean', documentation: 'نوع منطقي (صحيح/خطأ)', insertText: 'منطقي', sortGroup: 1 },
    { label: 'فراغ', kind: vscode.CompletionItemKind.TypeParameter, detail: 'نوع: فراغ — void', documentation: 'نوع بدون قيمة إرجاع', insertText: 'فراغ', sortGroup: 1 },
    { label: 'عدم', kind: vscode.CompletionItemKind.TypeParameter, detail: 'نوع: عدم — null type', documentation: 'نوع اللاشيء', insertText: 'عدم', sortGroup: 1 },
    { label: 'مصفوفة', kind: vscode.CompletionItemKind.TypeParameter, detail: 'نوع: مصفوفة — array', documentation: 'نوع المصفوفات الديناميكية', insertText: 'مصفوفة', sortGroup: 1 },
    { label: 'خريطة', kind: vscode.CompletionItemKind.TypeParameter, detail: 'نوع: خريطة — map', documentation: 'نوع القاموس (مفتاح → قيمة)', insertText: 'خريطة', sortGroup: 1 },
    { label: 'أي', kind: vscode.CompletionItemKind.TypeParameter, detail: 'نوع: أي — any', documentation: 'نوع عام يقبل أي قيمة', insertText: 'أي', sortGroup: 1 },
];

// ─── تحليل المعرّفات من الملف الحالي ───

interface ParsedSymbol {
    name: string;
    kind: vscode.CompletionItemKind;
    detail: string;
    line: number;
    params?: string[];
}

function parseDocumentSymbols(document: vscode.TextDocument): ParsedSymbol[] {
    const symbols: ParsedSymbol[] = [];
    const text = document.getText();
    const lines = text.split('\n');

    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();

        // دالة
        const funcMatch = line.match(/^دالة\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)\s*\(([^)]*)\)/);
        if (funcMatch) {
            const params = funcMatch[2] ? funcMatch[2].split(/[،,]/).map(s => s.trim()).filter(Boolean) : [];
            symbols.push({ name: funcMatch[1], kind: vscode.CompletionItemKind.Function, detail: `دالة(${params.join('، ')})`, line: i, params });
        }

        // صنف
        const classMatch = line.match(/^صنف\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/);
        if (classMatch) {
            symbols.push({ name: classMatch[1], kind: vscode.CompletionItemKind.Class, detail: 'صنف', line: i });
        }

        // متغير
        const varMatch = line.match(/^متغير\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/);
        if (varMatch) {
            symbols.push({ name: varMatch[1], kind: vscode.CompletionItemKind.Variable, detail: 'متغير', line: i });
        }

        // ثابت
        const constMatch = line.match(/^ثابت\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/);
        if (constMatch) {
            symbols.push({ name: constMatch[1], kind: vscode.CompletionItemKind.Constant, detail: 'ثابت', line: i });
        }

        // بنية
        const structMatch = line.match(/^بنية\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/);
        if (structMatch) {
            symbols.push({ name: structMatch[1], kind: vscode.CompletionItemKind.Struct, detail: 'بنية', line: i });
        }

        // تعداد
        const enumMatch = line.match(/^تعداد\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/);
        if (enumMatch) {
            symbols.push({ name: enumMatch[1], kind: vscode.CompletionItemKind.Enum, detail: 'تعداد', line: i });
        }
    }

    return symbols;
}

// ─── تحليل الرموز من ملفات المشروع (cross-file) ───

function parseWorkspaceSymbols(): ParsedSymbol[] {
    const symbols: ParsedSymbol[] = [];
    const folders = vscode.workspace.workspaceFolders;
    if (!folders) { return symbols; }

    for (const folder of folders) {
        try {
            const files = findSadFiles(folder.uri.fsPath, 3); // max depth 3
            for (const filePath of files) {
                try {
                    const content = fs.readFileSync(filePath, 'utf-8');
                    const lines = content.split('\n');
                    for (let i = 0; i < lines.length; i++) {
                        const line = lines[i].trim();
                        const funcMatch = line.match(/^(?:صدّر\s+)?دالة\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)\s*\(([^)]*)\)/);
                        if (funcMatch) {
                            const params = funcMatch[2] ? funcMatch[2].split(/[،,]/).map(s => s.trim()) : [];
                            symbols.push({
                                name: funcMatch[1],
                                kind: vscode.CompletionItemKind.Function,
                                detail: `دالة(${params.join('، ')}) — ${path.basename(filePath)}`,
                                line: i,
                                params,
                            });
                        }
                        const classMatch = line.match(/^(?:صدّر\s+)?صنف\s+([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/);
                        if (classMatch) {
                            symbols.push({
                                name: classMatch[1],
                                kind: vscode.CompletionItemKind.Class,
                                detail: `صنف — ${path.basename(filePath)}`,
                                line: i,
                            });
                        }
                    }
                } catch { /* ignore read errors */ }
            }
        } catch { /* ignore folder errors */ }
    }

    return symbols;
}

function findSadFiles(dir: string, maxDepth: number, depth = 0): string[] {
    if (depth >= maxDepth) { return []; }
    const results: string[] = [];
    try {
        const entries = fs.readdirSync(dir, { withFileTypes: true });
        for (const entry of entries) {
            if (entry.name.startsWith('.') || entry.name === 'node_modules' || entry.name === 'build') { continue; }
            const fullPath = path.join(dir, entry.name);
            if (entry.isDirectory()) {
                results.push(...findSadFiles(fullPath, maxDepth, depth + 1));
            } else if (entry.name.endsWith('.ص')) {
                results.push(fullPath);
            }
        }
    } catch { /* ignore */ }
    return results;
}

// ─── مزود التلميحات عند التحويم ───

class SadHoverProvider implements vscode.HoverProvider {
    provideHover(document: vscode.TextDocument, position: vscode.Position): vscode.ProviderResult<vscode.Hover> {
        const wordRange = document.getWordRangeAtPosition(position, /[\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*/);
        if (!wordRange) { return null; }
        const word = document.getText(wordRange);

        // بحث في الكلمات المفتاحية
        const kw = KEYWORD_COMPLETIONS.find(k => k.label === word);
        if (kw) {
            const md = new vscode.MarkdownString();
            md.appendMarkdown(`**${kw.detail}**\n\n`);
            md.appendMarkdown(kw.documentation);
            return new vscode.Hover(md, wordRange);
        }

        // بحث في الدوال المدمجة
        const fn = BUILTIN_FUNCTIONS.find(f => f.label === word);
        if (fn) {
            const md = new vscode.MarkdownString();
            md.appendMarkdown(`**${fn.detail}**\n\n`);
            md.appendMarkdown(fn.documentation);
            return new vscode.Hover(md, wordRange);
        }

        // بحث في الأنواع
        const tp = TYPE_COMPLETIONS.find(t => t.label === word);
        if (tp) {
            const md = new vscode.MarkdownString();
            md.appendMarkdown(`**${tp.detail}**\n\n`);
            md.appendMarkdown(tp.documentation);
            return new vscode.Hover(md, wordRange);
        }

        // بحث في الرموز المحلية
        const symbols = parseDocumentSymbols(document);
        const sym = symbols.find(s => s.name === word);
        if (sym) {
            const md = new vscode.MarkdownString();
            md.appendMarkdown(`**${sym.detail}** — سطر ${sym.line + 1}\n\n`);
            if (sym.params) {
                md.appendMarkdown(`المعاملات: \`${sym.params.join('، ')}\``);
            }
            return new vscode.Hover(md, wordRange);
        }

        return null;
    }
}

// ─── مزود مساعدة التوقيع ───

class SadSignatureHelpProvider implements vscode.SignatureHelpProvider {
    provideSignatureHelp(document: vscode.TextDocument, position: vscode.Position): vscode.ProviderResult<vscode.SignatureHelp> {
        const lineText = document.lineAt(position).text;
        const textBefore = lineText.substring(0, position.character);

        // البحث عن اسم الدالة قبل القوس المفتوح
        const match = textBefore.match(/([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)\s*\([^)]*$/);
        if (!match) { return null; }
        const funcName = match[1];

        // بحث في الدوال المدمجة
        const fn = BUILTIN_FUNCTIONS.find(f => f.label === funcName);
        if (fn) {
            const sig = new vscode.SignatureInformation(
                fn.insertText.replace(/\$\{\d+:?([^}]*)\}/g, '$1'),
                new vscode.MarkdownString(fn.documentation)
            );
            const help = new vscode.SignatureHelp();
            help.signatures = [sig];
            help.activeSignature = 0;

            // حساب المعامل النشط
            const afterParen = textBefore.substring(textBefore.lastIndexOf('(') + 1);
            help.activeParameter = (afterParen.match(/[،,]/g) || []).length;

            return help;
        }

        // بحث في الرموز المحلية
        const symbols = parseDocumentSymbols(document);
        const sym = symbols.find(s => s.name === funcName && s.params);
        if (sym && sym.params) {
            const paramStr = sym.params.join('، ');
            const sig = new vscode.SignatureInformation(
                `${funcName}(${paramStr})`,
                `دالة محلية — سطر ${sym.line + 1}`
            );
            sym.params.forEach(p => {
                sig.parameters.push(new vscode.ParameterInformation(p));
            });

            const help = new vscode.SignatureHelp();
            help.signatures = [sig];
            help.activeSignature = 0;
            const afterParen = textBefore.substring(textBefore.lastIndexOf('(') + 1);
            help.activeParameter = (afterParen.match(/[،,]/g) || []).length;
            return help;
        }

        return null;
    }
}

// ─── مزود الإكمال التلقائي ───

class SadCompletionProvider implements vscode.CompletionItemProvider {
    private workspaceSymbolsCache: ParsedSymbol[] = [];
    private lastCacheTime = 0;

    provideCompletionItems(document: vscode.TextDocument, position: vscode.Position): vscode.CompletionItem[] {
        const items: vscode.CompletionItem[] = [];
        const lineText = document.lineAt(position).text;
        const textBefore = lineText.substring(0, position.character);

        // 1. الكلمات المفتاحية
        for (const entry of KEYWORD_COMPLETIONS) {
            const item = this.createItem(entry);
            items.push(item);
        }

        // 2. الدوال المدمجة
        for (const entry of BUILTIN_FUNCTIONS) {
            const item = this.createItem(entry);
            items.push(item);
        }

        // 3. أسماء الأنواع المدمجة
        for (const entry of TYPE_COMPLETIONS) {
            const item = this.createItem(entry);
            items.push(item);
        }

        // 4. الرموز المحلية من الملف الحالي
        const localSymbols = parseDocumentSymbols(document);
        for (const sym of localSymbols) {
            const item = new vscode.CompletionItem(sym.name, sym.kind);
            item.detail = sym.detail;
            if (sym.params) {
                item.insertText = new vscode.SnippetString(
                    `${sym.name}(${sym.params.map((p, i) => `\${${i + 1}:${p}}`).join('، ')})`
                );
            }
            item.sortText = `2_${sym.name}`;
            items.push(item);
        }

        // 5. رموز من ملفات المشروع (with cache)
        const now = Date.now();
        if (now - this.lastCacheTime > 10000) { // refresh every 10s
            this.workspaceSymbolsCache = parseWorkspaceSymbols();
            this.lastCacheTime = now;
        }
        for (const sym of this.workspaceSymbolsCache) {
            const item = new vscode.CompletionItem(sym.name, sym.kind);
            item.detail = sym.detail;
            item.sortText = `3_${sym.name}`;
            items.push(item);
        }

        // 6. إكمال بعد هذا.
        if (textBefore.match(/هذا\.\s*$/)) {
            // البحث عن خصائص الصنف الحالي
            const text = document.getText();
            const thisProps = new Set<string>();
            const propRegex = /هذا\.([\u0600-\u06FFa-zA-Z_][\u0600-\u06FFa-zA-Z0-9_]*)/g;
            let m;
            while ((m = propRegex.exec(text)) !== null) {
                thisProps.add(m[1]);
            }
            for (const prop of thisProps) {
                const item = new vscode.CompletionItem(prop, vscode.CompletionItemKind.Property);
                item.detail = 'خاصية — property';
                item.sortText = '0_' + prop;
                items.push(item);
            }
        }

        // 7. قصاصات مركبة (Snippets) في السياق
        if (textBefore.trim() === '' || /^\s*$/.test(textBefore)) {
            items.push(...this.contextSnippets());
        }

        return items;
    }

    private createItem(entry: CompletionEntry): vscode.CompletionItem {
        const item = new vscode.CompletionItem(entry.label, entry.kind);
        item.detail = entry.detail;
        item.documentation = new vscode.MarkdownString(entry.documentation);
        item.insertText = new vscode.SnippetString(entry.insertText);
        item.sortText = `${entry.sortGroup}_${entry.label}`;
        return item;
    }

    private contextSnippets(): vscode.CompletionItem[] {
        const snippets: vscode.CompletionItem[] = [];

        const fileSnippet = new vscode.CompletionItem('ملف_جديد', vscode.CompletionItemKind.Snippet);
        fileSnippet.detail = 'قالب ملف جديد';
        fileSnippet.insertText = new vscode.SnippetString(
            '# ${1:وصف الملف}\n\n${2}\n'
        );
        fileSnippet.sortText = '4_ملف';
        snippets.push(fileSnippet);

        const mainSnippet = new vscode.CompletionItem('رئيسية', vscode.CompletionItemKind.Snippet);
        mainSnippet.detail = 'نقطة الدخول الرئيسية';
        mainSnippet.insertText = new vscode.SnippetString(
            'دالة رئيسية()\n\t${1:اطبع_سطر("مرحباً بالعالم")}\nنهاية\n\nرئيسية()\n'
        );
        mainSnippet.sortText = '4_رئيسية';
        snippets.push(mainSnippet);

        const classSnippet = new vscode.CompletionItem('صنف_كامل', vscode.CompletionItemKind.Snippet);
        classSnippet.detail = 'قالب صنف كامل مع باني';
        classSnippet.insertText = new vscode.SnippetString(
            'صنف ${1:اسم}\n\tباني(${2:معاملات})\n\t\tهذا.${3:خاصية} = ${2:معاملات}\n\تنهاية\n\n\tدالة ${4:طريقة}()\n\t\t${5}\n\تنهاية\nنهاية\n'
        );
        classSnippet.sortText = '4_صنف';
        snippets.push(classSnippet);

        return snippets;
    }
}

export function registerIntelliSense(context: vscode.ExtensionContext): void {
    const selector: vscode.DocumentSelector = { language: 'sad', scheme: 'file' };

    context.subscriptions.push(
        vscode.languages.registerCompletionItemProvider(
            selector,
            new SadCompletionProvider(),
            '.', '،', '('
        ),
        vscode.languages.registerHoverProvider(selector, new SadHoverProvider()),
        vscode.languages.registerSignatureHelpProvider(
            selector,
            new SadSignatureHelpProvider(),
            '(', '،', ','
        ),
    );
}
