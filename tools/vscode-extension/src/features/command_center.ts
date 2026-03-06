/**
 * @file command_center.ts
 * @brief لوحة التحكم — Command Center Panel
 * 
 * لوحة تحكم تفاعلية مركزية تعرض جميع ميزات إضافة لغة ص
 * مع أيقونات وتصنيفات وأزرار تشغيل سريع
 */

import * as vscode from 'vscode';

interface FeatureItem {
    id: string;
    name: string;
    description: string;
    icon: string;
    command: string;
    shortcut?: string;
    category: string;
}

const FEATURES: FeatureItem[] = [
    // ─── التشغيل والتصحيح ───
    { id: 'run', name: 'تشغيل الملف', description: 'تشغيل الملف الحالي بالمفسر أو المترجم', icon: '▶️', command: 'sad.runFile', shortcut: 'F5', category: 'التشغيل والتصحيح' },
    { id: 'runInterpreter', name: 'تشغيل بالمفسر', description: 'تشغيل بمفسر sad.exe', icon: '🔵', command: 'sad.runWithInterpreter', shortcut: '', category: 'التشغيل والتصحيح' },
    { id: 'runCompiler', name: 'ترجمة وتشغيل', description: 'ترجمة بمترجم sadc.exe ثم تشغيل', icon: '🔴', command: 'sad.runWithCompiler', shortcut: '', category: 'التشغيل والتصحيح' },
    { id: 'runSelection', name: 'تشغيل النص المحدد', description: 'تشغيل الاختيار مع عرض النتيجة مضمنة', icon: '⚡', command: 'sad.runSelection', shortcut: 'Ctrl+Shift+Enter', category: 'التشغيل والتصحيح' },
    { id: 'runLine', name: 'تشغيل السطر الحالي', description: 'تشغيل السطر الذي عليه المؤشر', icon: '➡️', command: 'sad.runLine', shortcut: '', category: 'التشغيل والتصحيح' },
    { id: 'runToCursor', name: 'تشغيل حتى المؤشر', description: 'تشغيل الكود من البداية حتى موقع المؤشر', icon: '⏩', command: 'sad.runToCursor', shortcut: '', category: 'التشغيل والتصحيح' },
    { id: 'debug', name: 'التصحيح المرئي', description: 'تصحيح الملف خطوة بخطوة', icon: '🐛', command: 'sad.debug', shortcut: '', category: 'التشغيل والتصحيح' },
    { id: 'terminal', name: 'فتح طرفية ص', description: 'طرفية مخصصة للغة ص', icon: '💻', command: 'sad.openTerminal', shortcut: 'Ctrl+Shift+`', category: 'التشغيل والتصحيح' },
    { id: 'runInTerminal', name: 'تشغيل في الطرفية', description: 'تشغيل الملف في الطرفية المخصصة', icon: '📟', command: 'sad.runInTerminal', shortcut: '', category: 'التشغيل والتصحيح' },

    // ─── التحليل والمراجعة ───
    { id: 'complexity', name: 'تحليل التعقيد', description: 'تحليل تعقيد الكود Cyclomatic', icon: '📊', command: 'sad.analyzeComplexity', shortcut: '', category: 'التحليل والمراجعة' },
    { id: 'performance', name: 'تحليل الأداء', description: 'تحليل أداء الكود وقياس الزمن', icon: '⚙️', command: 'sad.analyzePerformance', shortcut: '', category: 'التحليل والمراجعة' },
    { id: 'codeReview', name: 'مراجعة الكود', description: 'تحليل ثابت واقتراحات تحسين', icon: '🔍', command: 'sad.reviewCode', shortcut: '', category: 'التحليل والمراجعة' },
    { id: 'callHierarchy', name: 'هرمية الاستدعاءات', description: 'رسم شجرة استدعاءات الدوال', icon: '🌳', command: 'sad.showCallHierarchy', shortcut: '', category: 'التحليل والمراجعة' },
    { id: 'depGraph', name: 'رسم الاعتمادات', description: 'رسم بياني للاستيرادات بين الملفات', icon: '🕸️', command: 'sad.showDependencyGraph', shortcut: '', category: 'التحليل والمراجعة' },
    { id: 'minimapSummary', name: 'ملخص الخريطة', description: 'ملخص سريع لمحتوى الملف', icon: '🗺️', command: 'sad.minimapSummary', shortcut: '', category: 'التحليل والمراجعة' },

    // ─── التحرير والتنسيق ───
    { id: 'smartRename', name: 'إعادة التسمية الذكية', description: 'إعادة تسمية عبر الملفات مع معاينة', icon: '✏️', command: 'sad.smartRename', shortcut: 'F6', category: 'التحرير والتنسيق' },
    { id: 'batchRename', name: 'إعادة تسمية جماعية', description: 'استبدال أسماء متعددة دفعة واحدة', icon: '📝', command: 'sad.batchRename', shortcut: '', category: 'التحرير والتنسيق' },
    { id: 'organizeImports', name: 'تنظيم الاستيرادات', description: 'فرز وتنظيف الاستيرادات تلقائياً', icon: '📦', command: 'sad.organizeImports', shortcut: 'Ctrl+Shift+O', category: 'التحرير والتنسيق' },
    { id: 'removeUnused', name: 'إزالة الاستيرادات الزائدة', description: 'حذف الاستيرادات غير المستخدمة', icon: '🗑️', command: 'sad.removeUnusedImports', shortcut: '', category: 'التحرير والتنسيق' },
    { id: 'addImport', name: 'إضافة استيراد', description: 'إضافة استيراد جديد من قائمة', icon: '➕', command: 'sad.addImport', shortcut: '', category: 'التحرير والتنسيق' },
    { id: 'wrapSnippet', name: 'تغليف بقصاصة', description: 'إحاطة الكود بكتلة (إذا/بينما/حاول)', icon: '🎁', command: 'sad.wrapWithSnippet', shortcut: '', category: 'التحرير والتنسيق' },
    { id: 'generateSnippet', name: 'توليد قصاصة', description: 'توليد snippet من الكود المحدد', icon: '✂️', command: 'sad.generateSnippet', shortcut: '', category: 'التحرير والتنسيق' },
    { id: 'insertColor', name: 'إدراج لون', description: 'اختيار وإدراج لون للرسوميات', icon: '🎨', command: 'sad.insertColor', shortcut: '', category: 'التحرير والتنسيق' },
    { id: 'refactor', name: 'إعادة هيكلة', description: 'استخراج كود كدالة جديدة', icon: '♻️', command: 'sad.aiRefactor', shortcut: '', category: 'التحرير والتنسيق' },

    // ─── التنقل والعلامات ───
    { id: 'goToSymbol', name: 'انتقل إلى رمز', description: 'تنقل سريع إلى دالة/صنف/متغير', icon: '🔎', command: 'sad.goToSymbol', shortcut: 'Ctrl+Shift+.', category: 'التنقل والعلامات' },
    { id: 'toggleBookmark', name: 'إضافة/إزالة علامة', description: 'وضع علامة مرجعية على السطر الحالي', icon: '🔖', command: 'sad.toggleBookmark', shortcut: 'Ctrl+F2', category: 'التنقل والعلامات' },
    { id: 'bookmarkLabel', name: 'علامة بوصف', description: 'إضافة علامة مع وصف وتصنيف', icon: '🏷️', command: 'sad.toggleBookmarkWithLabel', shortcut: '', category: 'التنقل والعلامات' },
    { id: 'nextBookmark', name: 'العلامة التالية', description: 'الانتقال للعلامة التالية', icon: '⏭️', command: 'sad.nextBookmark', shortcut: 'F2', category: 'التنقل والعلامات' },
    { id: 'prevBookmark', name: 'العلامة السابقة', description: 'الانتقال للعلامة السابقة', icon: '⏮️', command: 'sad.prevBookmark', shortcut: '', category: 'التنقل والعلامات' },
    { id: 'listBookmarks', name: 'قائمة العلامات', description: 'عرض جميع العلامات للتنقل السريع', icon: '📑', command: 'sad.listBookmarks', shortcut: '', category: 'التنقل والعلامات' },
    { id: 'bookmarkPanel', name: 'لوحة العلامات', description: 'لوحة تفاعلية لإدارة العلامات', icon: '📋', command: 'sad.showBookmarkPanel', shortcut: '', category: 'التنقل والعلامات' },

    // ─── التوثيق والمشاركة ───
    { id: 'generateDoc', name: 'توليد التوثيق', description: 'توليد تعليقات توثيقية تلقائياً', icon: '📄', command: 'sad.generateDoc', shortcut: 'Ctrl+Shift+D', category: 'التوثيق والمشاركة' },
    { id: 'previewDocs', name: 'معاينة التوثيق', description: 'معاينة توثيق Markdown تفاعلي', icon: '📖', command: 'sad.previewDocs', shortcut: '', category: 'التوثيق والمشاركة' },
    { id: 'exportDocs', name: 'تصدير التوثيق', description: 'تصدير التوثيق كملف Markdown', icon: '💾', command: 'sad.exportDocs', shortcut: '', category: 'التوثيق والمشاركة' },
    { id: 'fileDocHeader', name: 'رأس التوثيق', description: 'إضافة رأس توثيقي للملف', icon: '📝', command: 'sad.generateFileDoc', shortcut: '', category: 'التوثيق والمشاركة' },
    { id: 'shareCode', name: 'مشاركة الكود', description: 'مشاركة الكود عبر خادم محلي', icon: '🔗', command: 'sad.shareCode', shortcut: 'Ctrl+Shift+S', category: 'التوثيق والمشاركة' },
    { id: 'shareSelection', name: 'مشاركة الاختيار', description: 'مشاركة النص المحدد فقط', icon: '📤', command: 'sad.shareSelection', shortcut: '', category: 'التوثيق والمشاركة' },
    { id: 'codeAsImage', name: 'نسخ كصورة', description: 'نسخ الكود كصورة SVG', icon: '🖼️', command: 'sad.copyCodeAsImage', shortcut: '', category: 'التوثيق والمشاركة' },
    { id: 'exportSnippet', name: 'تصدير مقطع', description: 'تصدير كود بصيغة Markdown/HTML', icon: '📋', command: 'sad.exportCodeSnippet', shortcut: '', category: 'التوثيق والمشاركة' },

    // ─── المساعد الذكي والتعليم ───
    { id: 'aiSuggest', name: 'اقتراحات ذكية', description: 'اقتراحات سياقية للكود', icon: '🤖', command: 'sad.aiSuggest', shortcut: '', category: 'المساعد الذكي' },
    { id: 'aiFix', name: 'إصلاح ذكي', description: 'محاولة إصلاح الأخطاء تلقائياً', icon: '🔧', command: 'sad.aiFix', shortcut: '', category: 'المساعد الذكي' },
    { id: 'aiExplain', name: 'شرح الكود', description: 'شرح ما يفعله السطر أو الاختيار', icon: '💡', command: 'sad.aiExplain', shortcut: '', category: 'المساعد الذكي' },
    { id: 'aiGenerate', name: 'توليد كود', description: 'توليد كود من قوالب جاهزة', icon: '✨', command: 'sad.aiGenerate', shortcut: '', category: 'المساعد الذكي' },
    { id: 'tutorial', name: 'المساعد التعليمي', description: 'دروس تفاعلية لتعلم لغة ص', icon: '🎓', command: 'sad.openTutorial', shortcut: 'Ctrl+Shift+L', category: 'المساعد الذكي' },

    // ─── المشروع والأدوات ───
    { id: 'dashboard', name: 'لوحة القيادة', description: 'إحصائيات المشروع ومؤشر الصحة', icon: '📊', command: 'sad.showDashboard', shortcut: 'Ctrl+Shift+P Ctrl+D', category: 'المشروع والأدوات' },
    { id: 'newProject', name: 'مشروع جديد', description: 'إنشاء مشروع من قالب', icon: '📁', command: 'sad.newProject', shortcut: '', category: 'المشروع والأدوات' },
    { id: 'addModule', name: 'إضافة وحدة', description: 'إنشاء ملف وحدة جديدة', icon: '📄', command: 'sad.addModule', shortcut: '', category: 'المشروع والأدوات' },
    { id: 'addTest', name: 'إضافة اختبار', description: 'إنشاء ملف اختبار للملف الحالي', icon: '🧪', command: 'sad.addTest', shortcut: '', category: 'المشروع والأدوات' },
    { id: 'testExplorer', name: 'مستكشف الاختبارات', description: 'عرض وتشغيل جميع الاختبارات', icon: '✅', command: 'sad.refreshTests', shortcut: '', category: 'المشروع والأدوات' },
    { id: 'database', name: 'قاعدة بيانات', description: 'فتح وإدارة قواعد البيانات', icon: '🗄️', command: 'sad.openDatabase', shortcut: '', category: 'المشروع والأدوات' },
    { id: 'timeline', name: 'الخط الزمني', description: 'تاريخ التغييرات عبر Git', icon: '⏳', command: 'sad.showTimeline', shortcut: '', category: 'المشروع والأدوات' },

    // ─── العرض والاتجاه ───
    { id: 'ast', name: 'مستكشف AST', description: 'عرض شجرة التحليل النحوي', icon: '🌲', command: 'sad.showAST', shortcut: 'Ctrl+Shift+A', category: 'العرض والاتجاه' },
    { id: 'rtl', name: 'تبديل RTL/LTR', description: 'تبديل اتجاه النص', icon: '↔️', command: 'sad.toggleRTL', shortcut: 'Ctrl+Shift+;', category: 'العرض والاتجاه' },
    { id: 'rtlPanel', name: 'لوحة RTL', description: 'فتح لوحة العرض من اليمين لليسار', icon: '⬅️', command: 'sad.openRTLPanel', shortcut: '', category: 'العرض والاتجاه' },
    { id: 'graphics', name: 'معاينة الرسوميات', description: 'معاينة أوامر الرسم', icon: '🖌️', command: 'sad.previewGraphics', shortcut: 'Ctrl+Shift+G', category: 'العرض والاتجاه' },
    { id: 'translate', name: 'ترجمة تلقائية', description: 'ترجمة بين Python ولغة ص', icon: '🔄', command: 'sad.translateAuto', shortcut: 'Ctrl+Shift+T', category: 'العرض والاتجاه' },

    // ─── الطي والتنسيق ───
    { id: 'foldFunc', name: 'طي الدوال', description: 'طي جميع الدوال', icon: '📂', command: 'sad.foldFunctions', shortcut: '', category: 'الطي والتنسيق' },
    { id: 'foldClass', name: 'طي الأصناف', description: 'طي جميع الأصناف', icon: '📂', command: 'sad.foldClasses', shortcut: '', category: 'الطي والتنسيق' },
    { id: 'foldComment', name: 'طي التعليقات', description: 'طي جميع التعليقات', icon: '📂', command: 'sad.foldComments', shortcut: '', category: 'الطي والتنسيق' },
    { id: 'foldImport', name: 'طي الاستيرادات', description: 'طي جميع الاستيرادات', icon: '📂', command: 'sad.foldImports', shortcut: '', category: 'الطي والتنسيق' },
];

export function registerCommandCenter(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.commandCenter', () => {
            CommandCenterPanel.createOrShow(context.extensionUri);
        }),
        vscode.commands.registerCommand('sad.quickCommand', () => quickCommand()),
    );
}

async function quickCommand(): Promise<void> {
    const items = FEATURES.map(f => ({
        label: `${f.icon} ${f.name}`,
        description: f.shortcut || '',
        detail: `[${f.category}] ${f.description}`,
        command: f.command,
    }));

    const selected = await vscode.window.showQuickPick(items, {
        placeHolder: '🎯 ابحث عن ميزة... — Search features...',
        matchOnDescription: true,
        matchOnDetail: true,
    });

    if (selected) {
        vscode.commands.executeCommand(selected.command);
    }
}

class CommandCenterPanel {
    public static currentPanel: CommandCenterPanel | undefined;
    private readonly panel: vscode.WebviewPanel;
    private disposables: vscode.Disposable[] = [];

    static createOrShow(extensionUri: vscode.Uri): void {
        if (CommandCenterPanel.currentPanel) {
            CommandCenterPanel.currentPanel.panel.reveal(vscode.ViewColumn.One);
            return;
        }
        const panel = vscode.window.createWebviewPanel(
            'sadCommandCenter', '🎯 لوحة التحكم — Command Center',
            vscode.ViewColumn.One, { enableScripts: true }
        );
        CommandCenterPanel.currentPanel = new CommandCenterPanel(panel);
    }

    private constructor(panel: vscode.WebviewPanel) {
        this.panel = panel;
        this.panel.webview.html = this.getHTML();

        this.panel.webview.onDidReceiveMessage(msg => {
            if (msg.type === 'execute') {
                vscode.commands.executeCommand(msg.command);
            }
        }, null, this.disposables);

        this.panel.onDidDispose(() => {
            CommandCenterPanel.currentPanel = undefined;
            this.disposables.forEach(d => d.dispose());
        }, null, this.disposables);
    }

    private getHTML(): string {
        const isDark = vscode.window.activeColorTheme.kind === vscode.ColorThemeKind.Dark ||
                       vscode.window.activeColorTheme.kind === vscode.ColorThemeKind.HighContrast;
        const bg = isDark ? '#1e1e1e' : '#ffffff';
        const fg = isDark ? '#d4d4d4' : '#333333';
        const cardBg = isDark ? '#252526' : '#f3f3f3';
        const cardHover = isDark ? '#2d2d30' : '#e8e8e8';
        const accent = '#4fc3f7';
        const borderColor = isDark ? '#3e3e42' : '#d4d4d4';

        // تجميع حسب التصنيف
        const categories = new Map<string, FeatureItem[]>();
        for (const f of FEATURES) {
            if (!categories.has(f.category)) { categories.set(f.category, []); }
            categories.get(f.category)!.push(f);
        }

        const categoryIcons: Record<string, string> = {
            'التشغيل والتصحيح': '🚀',
            'التحليل والمراجعة': '🔬',
            'التحرير والتنسيق': '✏️',
            'التنقل والعلامات': '🧭',
            'التوثيق والمشاركة': '📚',
            'المساعد الذكي': '🤖',
            'المشروع والأدوات': '🛠️',
            'العرض والاتجاه': '👁️',
            'الطي والتنسيق': '📂',
        };

        let sectionsHTML = '';
        for (const [catName, items] of categories) {
            const catIcon = categoryIcons[catName] || '📌';
            const cards = items.map(f => `
                <div class="card" onclick="execute('${f.command}')" title="${f.description}">
                    <div class="card-icon">${f.icon}</div>
                    <div class="card-body">
                        <div class="card-title">${f.name}</div>
                        <div class="card-desc">${f.description}</div>
                    </div>
                    ${f.shortcut ? `<div class="shortcut">${f.shortcut}</div>` : ''}
                </div>
            `).join('');

            sectionsHTML += `
                <div class="category">
                    <h2 class="category-title">${catIcon} ${catName}</h2>
                    <div class="cards">${cards}</div>
                </div>
            `;
        }

        // جدول الاختصارات
        const shortcutRows = FEATURES
            .filter(f => f.shortcut)
            .map(f => `<tr><td>${f.icon} ${f.name}</td><td><kbd>${f.shortcut}</kbd></td><td>${f.description}</td></tr>`)
            .join('');

        return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
    * { box-sizing: border-box; }
    body {
        font-family: 'Segoe UI', Tahoma, sans-serif;
        background: ${bg}; color: ${fg};
        margin: 0; padding: 0; direction: rtl;
    }
    .header {
        background: linear-gradient(135deg, #0d47a1, #1565c0, #1976d2);
        padding: 30px 40px; text-align: center; position: sticky; top: 0; z-index: 10;
    }
    .header h1 { color: #fff; margin: 0 0 8px; font-size: 28px; }
    .header p { color: rgba(255,255,255,0.8); margin: 0; font-size: 14px; }
    .search-container { margin: 16px auto 0; max-width: 500px; }
    .search {
        width: 100%; padding: 12px 20px; border: none; border-radius: 25px;
        font-size: 15px; background: rgba(255,255,255,0.15); color: #fff;
        outline: none; direction: rtl; backdrop-filter: blur(10px);
    }
    .search::placeholder { color: rgba(255,255,255,0.6); }
    .search:focus { background: rgba(255,255,255,0.25); }
    .content { padding: 20px 30px; max-width: 1200px; margin: 0 auto; }
    .stats-bar {
        display: flex; gap: 20px; justify-content: center; margin: 20px 0;
        flex-wrap: wrap;
    }
    .stat { background: ${cardBg}; padding: 12px 24px; border-radius: 10px; text-align: center; }
    .stat-num { font-size: 24px; font-weight: bold; color: ${accent}; }
    .stat-label { font-size: 12px; opacity: 0.7; margin-top: 4px; }
    .category { margin-bottom: 30px; }
    .category-title {
        font-size: 18px; color: ${accent}; margin-bottom: 12px;
        padding-bottom: 8px; border-bottom: 2px solid ${borderColor};
    }
    .cards {
        display: grid;
        grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
        gap: 10px;
    }
    .card {
        background: ${cardBg}; border-radius: 10px; padding: 14px 16px;
        cursor: pointer; transition: all 0.2s; border: 1px solid transparent;
        display: flex; align-items: center; gap: 12px; position: relative;
    }
    .card:hover {
        background: ${cardHover}; border-color: ${accent};
        transform: translateY(-2px); box-shadow: 0 4px 12px rgba(0,0,0,0.15);
    }
    .card-icon { font-size: 24px; flex-shrink: 0; width: 36px; text-align: center; }
    .card-body { flex: 1; min-width: 0; }
    .card-title { font-weight: 600; font-size: 14px; margin-bottom: 2px; }
    .card-desc { font-size: 11px; opacity: 0.6; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
    .shortcut {
        position: absolute; top: 8px; left: 8px;
        background: ${isDark ? '#3e3e42' : '#d4d4d4'}; color: ${fg};
        padding: 2px 8px; border-radius: 4px; font-size: 10px;
        font-family: monospace; white-space: nowrap;
    }
    .hidden { display: none !important; }

    .shortcuts-section { margin-top: 30px; }
    .shortcuts-table { width: 100%; border-collapse: collapse; }
    .shortcuts-table th { background: ${accent}; color: #fff; padding: 10px; text-align: right; }
    .shortcuts-table td { padding: 8px 10px; border-bottom: 1px solid ${borderColor}; }
    .shortcuts-table tr:hover { background: ${cardHover}; }
    kbd {
        background: ${isDark ? '#3e3e42' : '#e0e0e0'}; padding: 3px 8px;
        border-radius: 4px; font-family: monospace; font-size: 12px;
        border: 1px solid ${borderColor};
    }

    .tab-bar { display: flex; gap: 0; margin: 20px 0 10px; border-bottom: 2px solid ${borderColor}; }
    .tab {
        padding: 10px 20px; cursor: pointer; border-bottom: 2px solid transparent;
        margin-bottom: -2px; font-size: 14px; opacity: 0.7; transition: all 0.2s;
    }
    .tab:hover { opacity: 1; }
    .tab.active { border-bottom-color: ${accent}; opacity: 1; color: ${accent}; font-weight: bold; }
    .tab-content { display: none; }
    .tab-content.active { display: block; }
</style></head>
<body>
    <div class="header">
        <h1>🎯 لوحة تحكم لغة ص</h1>
        <p>جميع الميزات والأدوات في مكان واحد — ${FEATURES.length} ميزة متاحة</p>
        <div class="search-container">
            <input type="text" class="search" id="search" placeholder="🔍 ابحث عن ميزة..." oninput="filterCards()">
        </div>
    </div>

    <div class="content">
        <div class="stats-bar">
            <div class="stat"><div class="stat-num">${FEATURES.length}</div><div class="stat-label">ميزة</div></div>
            <div class="stat"><div class="stat-num">${categories.size}</div><div class="stat-label">تصنيف</div></div>
            <div class="stat"><div class="stat-num">${FEATURES.filter(f => f.shortcut).length}</div><div class="stat-label">اختصار</div></div>
        </div>

        <div class="tab-bar">
            <div class="tab active" onclick="switchTab('features')">⚡ الميزات</div>
            <div class="tab" onclick="switchTab('shortcuts')">⌨️ الاختصارات</div>
        </div>

        <div class="tab-content active" id="tab-features">
            ${sectionsHTML}
        </div>

        <div class="tab-content" id="tab-shortcuts">
            <div class="shortcuts-section">
                <h2>⌨️ جدول الاختصارات</h2>
                <table class="shortcuts-table">
                    <tr><th>الميزة</th><th>الاختصار</th><th>الوصف</th></tr>
                    ${shortcutRows}
                </table>
            </div>
        </div>
    </div>

    <script>
        const vscode = acquireVsCodeApi();

        function execute(command) {
            vscode.postMessage({ type: 'execute', command });
        }

        function filterCards() {
            const query = document.getElementById('search').value.trim().toLowerCase();
            const cards = document.querySelectorAll('.card');
            const categories = document.querySelectorAll('.category');

            cards.forEach(card => {
                const text = card.textContent.toLowerCase();
                card.classList.toggle('hidden', query && !text.includes(query));
            });

            // إخفاء التصنيفات الفارغة
            categories.forEach(cat => {
                const visibleCards = cat.querySelectorAll('.card:not(.hidden)');
                cat.classList.toggle('hidden', visibleCards.length === 0);
            });
        }

        function switchTab(tab) {
            document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            document.querySelectorAll('.tab-content').forEach(t => t.classList.remove('active'));
            document.getElementById('tab-' + tab).classList.add('active');
            document.querySelector('.tab[onclick*="' + tab + '"]').classList.add('active');
        }

        // اختصارات لوحة المفاتيح في اللوحة
        document.addEventListener('keydown', (e) => {
            if (e.key === '/' || e.key === 'ئ') {
                e.preventDefault();
                document.getElementById('search').focus();
            }
            if (e.key === 'Escape') {
                document.getElementById('search').value = '';
                filterCards();
                document.getElementById('search').blur();
            }
        });
    </script>
</body></html>`;
    }
}
