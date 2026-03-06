/**
 * @file ai_assistant.ts
 * @brief المساعد الذكي — AI Code Assistant
 * 
 * مساعد ذكي محلي يقترح إكمال الكود، إصلاح الأخطاء، وتوليد كود بلغة ص
 */

import * as vscode from 'vscode';

const CODE_PATTERNS: { trigger: string; suggestion: string; doc: string }[] = [
    { trigger: 'إذا', suggestion: 'إذا (${1:شرط})\n\t${2:// الكود}\nنهاية', doc: 'كتلة شرطية' },
    { trigger: 'بينما', suggestion: 'بينما (${1:شرط})\n\t${2:// الكود}\nنهاية', doc: 'حلقة بينما' },
    { trigger: 'لكل', suggestion: 'لكل ${1:عنصر} في ${2:مجموعة}\n\t${3:// الكود}\nنهاية', doc: 'حلقة لكل' },
    { trigger: 'دالة', suggestion: 'دالة ${1:الاسم}(${2:معاملات})\n\t${3:// الكود}\n\tارجع ${4:قيمة}\nنهاية', doc: 'تعريف دالة' },
    { trigger: 'صنف', suggestion: 'صنف ${1:الاسم}\n\tباني(${2:معاملات})\n\t\t${3:// تهيئة}\n\tنهاية\n\n\tعام دالة ${4:اسم_الدالة}()\n\t\t${5:// الكود}\n\tنهاية\nنهاية', doc: 'تعريف صنف' },
    { trigger: 'حاول', suggestion: 'حاول\n\t${1:// الكود}\nامسك ${2:خطأ}\n\t${3:// معالجة الخطأ}\nنهاية', doc: 'معالجة الاستثناءات' },
    { trigger: 'طابق', suggestion: 'طابق ${1:قيمة}\n\tعندما ${2:حالة1}\n\t\t${3:// كود}\n\tعندما ${4:حالة2}\n\t\t${5:// كود}\n\تافتراضي\n\t\t${6:// افتراضي}\nنهاية', doc: 'تعبير المطابقة' },
    { trigger: 'استورد', suggestion: 'استورد "${1:وحدة}" من "${2:مصدر}"', doc: 'استيراد وحدة' },
    { trigger: 'باني', suggestion: 'باني(${1:معاملات})\n\t${2:هذا.الاسم = الاسم}\nنهاية', doc: 'منشئ الصنف' },
    { trigger: 'خاصية', suggestion: 'خاصية ${1:الاسم}\n\تاحصل\n\t\تارجع هذا._${1:الاسم}\n\تنهاية\n\تعيّن(${2:قيمة})\n\t\تهذا._${1:الاسم} = ${2:قيمة}\n\تنهاية\nنهاية', doc: 'خاصية مع getter/setter' },
];

const ERROR_FIXES: { pattern: RegExp; fix: string; description: string }[] = [
    { pattern: /نهاية.*متوقعة/i, fix: 'أضف "نهاية" في نهاية الكتلة', description: 'كتلة غير مغلقة' },
    { pattern: /متغير.*غير.*معرف/i, fix: 'عرّف المتغير باستخدام "متغير اسم = قيمة"', description: 'متغير غير معرّف' },
    { pattern: /دالة.*غير.*موجودة/i, fix: 'تحقق من اسم الدالة أو عرّفها', description: 'دالة غير موجودة' },
    { pattern: /خطأ.*نحوي/i, fix: 'تحقق من البنية النحوية', description: 'خطأ في البنية' },
];

export function registerAIAssistant(context: vscode.ExtensionContext): void {
    // مزود الإكمال الذكي
    context.subscriptions.push(
        vscode.languages.registerCompletionItemProvider(
            { language: 'sad' },
            new SadCompletionProvider(),
            ...'ابتحدسصعقلمنوأإ'.split('')
        )
    );

    // أوامر المساعد
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.aiSuggest', () => suggestCode()),
        vscode.commands.registerCommand('sad.aiFix', () => fixError()),
        vscode.commands.registerCommand('sad.aiExplain', () => explainCode()),
        vscode.commands.registerCommand('sad.aiGenerate', () => generateCode()),
        vscode.commands.registerCommand('sad.aiRefactor', () => refactorCode()),
    );

    // Code Actions للإصلاح السريع
    context.subscriptions.push(
        vscode.languages.registerCodeActionsProvider(
            { language: 'sad' },
            new SadCodeActionProvider(),
            { providedCodeActionKinds: [vscode.CodeActionKind.QuickFix, vscode.CodeActionKind.Refactor] }
        )
    );
}

class SadCompletionProvider implements vscode.CompletionItemProvider {
    provideCompletionItems(doc: vscode.TextDocument, position: vscode.Position): vscode.CompletionItem[] {
        const lineText = doc.lineAt(position.line).text;
        const textBefore = lineText.substring(0, position.character);
        const items: vscode.CompletionItem[] = [];

        // أنماط الكود
        for (const pattern of CODE_PATTERNS) {
            if (textBefore.includes(pattern.trigger) || textBefore.trim() === '') {
                const item = new vscode.CompletionItem(
                    `🔮 ${pattern.trigger} — ${pattern.doc}`,
                    vscode.CompletionItemKind.Snippet
                );
                item.insertText = new vscode.SnippetString(pattern.suggestion);
                item.documentation = new vscode.MarkdownString(`**${pattern.doc}**\n\n\`\`\`sad\n${pattern.suggestion.replace(/\$\{\d+:?([^}]*)\}/g, '$1')}\n\`\`\``);
                item.sortText = '0' + pattern.trigger;
                items.push(item);
            }
        }

        // اقتراحات السياق
        const contextItems = getContextualSuggestions(doc, position);
        items.push(...contextItems);

        return items;
    }
}

function getContextualSuggestions(doc: vscode.TextDocument, position: vscode.Position): vscode.CompletionItem[] {
    const items: vscode.CompletionItem[] = [];
    const text = doc.getText();

    // اقتراح أسماء المتغيرات الموجودة
    const varRegex = /(?:متغير|ثابت)\s+([\p{L}\p{N}_]+)/gu;
    const seen = new Set<string>();
    let match;
    while ((match = varRegex.exec(text)) !== null) {
        if (!seen.has(match[1])) {
            seen.add(match[1]);
            const item = new vscode.CompletionItem(match[1], vscode.CompletionItemKind.Variable);
            item.detail = 'متغير محلي';
            items.push(item);
        }
    }

    // اقتراح أسماء الدوال الموجودة
    const funcRegex = /دالة\s+([\p{L}\p{N}_]+)\s*\(([^)]*)\)/gu;
    while ((match = funcRegex.exec(text)) !== null) {
        if (!seen.has(match[1])) {
            seen.add(match[1]);
            const item = new vscode.CompletionItem(match[1], vscode.CompletionItemKind.Function);
            item.detail = `دالة(${match[2]})`;
            item.insertText = new vscode.SnippetString(`${match[1]}($1)`);
            items.push(item);
        }
    }

    // اقتراح أسماء الأصناف
    const classRegex = /صنف\s+([\p{L}\p{N}_]+)/gu;
    while ((match = classRegex.exec(text)) !== null) {
        if (!seen.has(match[1])) {
            seen.add(match[1]);
            const item = new vscode.CompletionItem(match[1], vscode.CompletionItemKind.Class);
            item.detail = 'صنف';
            items.push(item);
        }
    }

    // الدوال المدمجة
    const builtins = [
        { name: 'اطبع', detail: 'طباعة نص', params: 'نص' },
        { name: 'اطبع_سطر', detail: 'طباعة نص مع سطر جديد', params: 'نص' },
        { name: 'طول', detail: 'طول النص أو المصفوفة', params: 'قيمة' },
        { name: 'نوع', detail: 'نوع القيمة', params: 'قيمة' },
        { name: 'ادخل', detail: 'قراءة مدخل من المستخدم', params: 'رسالة' },
        { name: 'رقم', detail: 'تحويل إلى رقم', params: 'قيمة' },
        { name: 'نص', detail: 'تحويل إلى نص', params: 'قيمة' },
    ];

    for (const bi of builtins) {
        if (!seen.has(bi.name)) {
            const item = new vscode.CompletionItem(bi.name, vscode.CompletionItemKind.Function);
            item.detail = `📦 ${bi.detail}`;
            item.insertText = new vscode.SnippetString(`${bi.name}($1)`);
            items.push(item);
        }
    }

    return items;
}

class SadCodeActionProvider implements vscode.CodeActionProvider {
    provideCodeActions(doc: vscode.TextDocument, range: vscode.Range, context: vscode.CodeActionContext): vscode.CodeAction[] {
        const actions: vscode.CodeAction[] = [];

        for (const diag of context.diagnostics) {
            for (const errorFix of ERROR_FIXES) {
                if (errorFix.pattern.test(diag.message)) {
                    const action = new vscode.CodeAction(
                        `🔧 ${errorFix.description}: ${errorFix.fix}`,
                        vscode.CodeActionKind.QuickFix
                    );
                    action.diagnostics = [diag];
                    action.isPreferred = true;

                    // إضافة إصلاح تلقائي حيث ممكن
                    if (errorFix.description === 'كتلة غير مغلقة') {
                        const edit = new vscode.WorkspaceEdit();
                        edit.insert(doc.uri, new vscode.Position(diag.range.end.line + 1, 0), 'نهاية\n');
                        action.edit = edit;
                    }

                    actions.push(action);
                }
            }
        }

        // Refactor: استخراج دالة
        if (!range.isEmpty) {
            const extractAction = new vscode.CodeAction(
                '♻️ استخراج كدالة',
                vscode.CodeActionKind.Refactor
            );
            extractAction.command = {
                title: 'استخراج كدالة',
                command: 'sad.aiRefactor',
            };
            actions.push(extractAction);
        }

        return actions;
    }
}

async function suggestCode(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const lineText = editor.document.lineAt(editor.selection.active.line).text.trim();

    // تحليل السياق
    const suggestions: string[] = [];

    if (lineText.startsWith('دالة')) {
        suggestions.push('💡 أضف معالجة أخطاء بـ "حاول...امسك"');
        suggestions.push('💡 أضف تعليق توثيقي قبل الدالة');
    }
    if (lineText.startsWith('صنف')) {
        suggestions.push('💡 أضف باني (constructor)');
        suggestions.push('💡 أضف دالة عرض (toString)');
    }
    if (lineText === '' || !lineText) {
        suggestions.push('💡 أضف تعليق للتوثيق');
        suggestions.push('💡 أنشئ دالة جديدة');
        suggestions.push('💡 أنشئ صنف جديد');
    }

    if (suggestions.length > 0) {
        const selected = await vscode.window.showQuickPick(suggestions, {
            placeHolder: '🔮 اقتراحات المساعد الذكي'
        });
        if (selected) {
            vscode.window.showInformationMessage(selected);
        }
    } else {
        vscode.window.showInformationMessage('💡 لا توجد اقتراحات للسياق الحالي');
    }
}

async function fixError(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const diagnostics = vscode.languages.getDiagnostics(editor.document.uri);
    const errors = diagnostics.filter(d => d.severity === vscode.DiagnosticSeverity.Error);

    if (errors.length === 0) {
        vscode.window.showInformationMessage('✅ لا توجد أخطاء في الملف');
        return;
    }

    const items = errors.map(e => ({
        label: `❌ سطر ${e.range.start.line + 1}`,
        description: e.message,
        diagnostic: e,
    }));

    const selected = await vscode.window.showQuickPick(items, {
        placeHolder: 'اختر خطأ لمحاولة إصلاحه'
    });

    if (selected) {
        // الانتقال إلى الخطأ
        editor.selection = new vscode.Selection(selected.diagnostic.range.start, selected.diagnostic.range.end);
        editor.revealRange(selected.diagnostic.range, vscode.TextEditorRevealType.InCenter);

        // محاولة إصلاح تلقائي
        for (const fix of ERROR_FIXES) {
            if (fix.pattern.test(selected.diagnostic.message)) {
                vscode.window.showInformationMessage(`🔧 الإصلاح المقترح: ${fix.fix}`);
                return;
            }
        }

        vscode.window.showInformationMessage('💡 لم يتم العثور على إصلاح تلقائي — حاول مراجعة الكود يدوياً');
    }
}

async function explainCode(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const selection = editor.selection.isEmpty
        ? editor.document.lineAt(editor.selection.active.line).text
        : editor.document.getText(editor.selection);

    const trimmed = selection.trim();
    let explanation = '🔍 ';

    if (/^دالة\s+/.test(trimmed)) {
        const match = trimmed.match(/دالة\s+([\p{L}\p{N}_]+)\s*\(([^)]*)\)/u);
        explanation += `تعريف دالة "${match?.[1]}" بمعاملات [${match?.[2] || 'بدون'}]`;
    } else if (/^صنف\s+/.test(trimmed)) {
        const match = trimmed.match(/صنف\s+([\p{L}\p{N}_]+)(?:\s+يرث\s+([\p{L}\p{N}_]+))?/u);
        explanation += `تعريف صنف "${match?.[1]}"${match?.[2] ? ` يرث من "${match[2]}"` : ''}`;
    } else if (/^إذا\s*\(/.test(trimmed)) {
        explanation += 'كتلة شرطية — تنفذ الكود إذا تحقق الشرط';
    } else if (/^بينما\s*\(/.test(trimmed)) {
        explanation += 'حلقة تكرارية — تستمر طالما الشرط متحقق';
    } else if (/^لكل\s+/.test(trimmed)) {
        explanation += 'حلقة لكل — تمر على جميع عناصر المجموعة';
    } else if (/^متغير\s+/.test(trimmed)) {
        explanation += 'تعريف متغير قابل للتغيير';
    } else if (/^ثابت\s+/.test(trimmed)) {
        explanation += 'تعريف ثابت — لا يمكن تغيير قيمته بعد التعيين';
    } else if (/^ارجع\s+/.test(trimmed)) {
        explanation += 'إرجاع قيمة من الدالة الحالية';
    } else {
        explanation += `التعبير: "${trimmed.substring(0, 50)}"`;
    }

    vscode.window.showInformationMessage(explanation);
}

async function generateCode(): Promise<void> {
    const templates = [
        { label: '⚡ دالة فارغة', code: 'دالة اسم_الدالة()\n\t# الكود هنا\nنهاية\n' },
        { label: '🏗️ صنف فارغ', code: 'صنف الاسم\n\tباني()\n\t\t# تهيئة\n\تنهاية\n\n\tعام دالة عرض()\n\t\t# الكود\n\تنهاية\nنهاية\n' },
        { label: '🔄 حلقة for-each', code: 'لكل عنصر في مصفوفة\n\تاطبع_سطر(عنصر)\nنهاية\n' },
        { label: '⚠️ معالجة أخطاء', code: 'حاول\n\ت# الكود الخطير\nامسك خطأ\n\تاطبع_سطر("خطأ: " + خطأ)\nنهاية\n' },
        { label: '📦 وحدة مع تصدير', code: '# وحدة: الاسم\n\nصدّر دالة الدالة_الأولى()\n\تارجع صحيح\nنهاية\n\nصدّر دالة الدالة_الثانية()\n\تارجع صحيح\ننهاية\n' },
        { label: '🧪 ملف اختبار', code: 'اختبر "الاختبار الأول"\n\ت# ضع اختباراتك هنا\n\تاطبع_سطر("✅ نجح")\ننهاية\n' },
    ];

    const selected = await vscode.window.showQuickPick(templates, {
        placeHolder: 'اختر قالب الكود المراد توليده'
    });

    if (selected) {
        const editor = vscode.window.activeTextEditor;
        if (editor) {
            editor.insertSnippet(new vscode.SnippetString(selected.code));
        }
    }
}

async function refactorCode(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.selection.isEmpty) {
        vscode.window.showWarningMessage('اختر كوداً لاستخراجه كدالة');
        return;
    }

    const funcName = await vscode.window.showInputBox({
        prompt: 'اسم الدالة الجديدة',
        placeHolder: 'الدالة_المستخرجة',
    });

    if (!funcName) { return; }

    const selectedCode = editor.document.getText(editor.selection);
    const indent = '\t';

    const funcDef = `\nدالة ${funcName}()\n${selectedCode.split('\n').map(l => indent + l).join('\n')}\nنهاية\n`;
    const funcCall = `${funcName}()`;

    const edit = new vscode.WorkspaceEdit();
    edit.replace(editor.document.uri, editor.selection, funcCall);

    // إضافة تعريف الدالة في نهاية الملف
    const lastLine = editor.document.lineCount - 1;
    edit.insert(editor.document.uri, new vscode.Position(lastLine, editor.document.lineAt(lastLine).text.length), funcDef);

    await vscode.workspace.applyEdit(edit);
    vscode.window.showInformationMessage(`✅ تم استخراج الكود إلى دالة "${funcName}"`);
}
