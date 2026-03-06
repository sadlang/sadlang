/**
 * @file sidebar_view.ts
 * @brief عرض الشريط الجانبي — Sidebar Activity Bar View
 * 
 * شريط جانبي يظهر في Activity Bar مع شجرة ميزات تفاعلية
 * لفتح لوحة التحكم وجميع أوامر لغة ص بنقرة واحدة
 */

import * as vscode from 'vscode';

interface SidebarCategory {
    label: string;
    icon: string;
    children: SidebarItem[];
}

interface SidebarItem {
    label: string;
    icon: string;
    command: string;
    description?: string;
    shortcut?: string;
}

const SIDEBAR_TREE: SidebarCategory[] = [
    {
        label: '🚀 التشغيل والتصحيح',
        icon: 'play',
        children: [
            { label: 'تشغيل الملف', icon: 'play', command: 'sad.runFile', shortcut: 'F5' },
            { label: 'تشغيل بالمفسر', icon: 'play-circle', command: 'sad.runWithInterpreter' },
            { label: 'ترجمة وتشغيل', icon: 'zap', command: 'sad.runWithCompiler' },
            { label: 'تشغيل النص المحدد', icon: 'selection', command: 'sad.runSelection', shortcut: 'Ctrl+Shift+Enter' },
            { label: 'تشغيل السطر', icon: 'arrow-right', command: 'sad.runLine', shortcut: 'Ctrl+Shift+E' },
            { label: 'تشغيل في الطرفية', icon: 'terminal', command: 'sad.runInTerminal', shortcut: 'Ctrl+F5' },
            { label: 'فتح طرفية ص', icon: 'terminal-bash', command: 'sad.openTerminal', shortcut: 'Ctrl+Shift+`' },
            { label: 'التصحيح المرئي', icon: 'debug-alt', command: 'sad.debug' },
        ]
    },
    {
        label: '🔬 التحليل والمراجعة',
        icon: 'search',
        children: [
            { label: 'تحليل التعقيد', icon: 'pulse', command: 'sad.analyzeComplexity' },
            { label: 'تحليل الأداء', icon: 'dashboard', command: 'sad.analyzePerformance' },
            { label: 'مراجعة الكود', icon: 'checklist', command: 'sad.reviewCode' },
            { label: 'هرمية الاستدعاءات', icon: 'type-hierarchy', command: 'sad.showCallHierarchy', shortcut: 'Ctrl+Shift+H' },
            { label: 'رسم الاعتمادات', icon: 'graph', command: 'sad.showDependencyGraph' },
            { label: 'مستكشف AST', icon: 'list-tree', command: 'sad.showAST', shortcut: 'Ctrl+Shift+A' },
        ]
    },
    {
        label: '✏️ التحرير والتنسيق',
        icon: 'edit',
        children: [
            { label: 'إعادة التسمية الذكية', icon: 'edit', command: 'sad.smartRename', shortcut: 'F6' },
            { label: 'إعادة تسمية جماعية', icon: 'replace-all', command: 'sad.batchRename' },
            { label: 'تنظيم الاستيرادات', icon: 'sort-precedence', command: 'sad.organizeImports', shortcut: 'Ctrl+Shift+O' },
            { label: 'تغليف بقصاصة', icon: 'bracket', command: 'sad.wrapWithSnippet', shortcut: 'Ctrl+Shift+W' },
            { label: 'توليد قصاصة', icon: 'diff', command: 'sad.generateSnippet' },
            { label: 'إدراج لون', icon: 'symbol-color', command: 'sad.insertColor' },
            { label: 'إعادة هيكلة', icon: 'lightbulb', command: 'sad.aiRefactor' },
        ]
    },
    {
        label: '🧭 التنقل والعلامات',
        icon: 'bookmark',
        children: [
            { label: 'انتقل إلى رمز', icon: 'symbol-method', command: 'sad.goToSymbol', shortcut: 'Ctrl+Shift+.' },
            { label: 'إضافة/إزالة علامة', icon: 'bookmark', command: 'sad.toggleBookmark', shortcut: 'Ctrl+F2' },
            { label: 'العلامة التالية', icon: 'arrow-down', command: 'sad.nextBookmark', shortcut: 'F2' },
            { label: 'العلامة السابقة', icon: 'arrow-up', command: 'sad.prevBookmark', shortcut: 'Shift+F2' },
            { label: 'لوحة العلامات', icon: 'list-unordered', command: 'sad.showBookmarkPanel', shortcut: 'Ctrl+Shift+F2' },
        ]
    },
    {
        label: '📚 التوثيق والمشاركة',
        icon: 'book',
        children: [
            { label: 'توليد التوثيق', icon: 'note', command: 'sad.generateDoc', shortcut: 'Ctrl+Shift+D' },
            { label: 'معاينة التوثيق', icon: 'open-preview', command: 'sad.previewDocs' },
            { label: 'تصدير التوثيق', icon: 'save', command: 'sad.exportDocs' },
            { label: 'مشاركة الكود', icon: 'link-external', command: 'sad.shareCode', shortcut: 'Ctrl+Shift+S' },
            { label: 'نسخ كصورة', icon: 'file-media', command: 'sad.copyCodeAsImage' },
        ]
    },
    {
        label: '🤖 المساعد الذكي',
        icon: 'robot',
        children: [
            { label: 'شرح الكود', icon: 'comment-discussion', command: 'sad.aiExplain' },
            { label: 'توليد كود', icon: 'sparkle', command: 'sad.aiGenerate' },
            { label: 'إصلاح ذكي', icon: 'wrench', command: 'sad.aiFix' },
            { label: 'المساعد التعليمي', icon: 'mortar-board', command: 'sad.openTutorial', shortcut: 'Ctrl+Shift+L' },
            { label: 'ترجمة تلقائية', icon: 'arrow-swap', command: 'sad.translateAuto', shortcut: 'Ctrl+Shift+T' },
        ]
    },
    {
        label: '🛠️ المشروع والأدوات',
        icon: 'tools',
        children: [
            { label: 'لوحة القيادة', icon: 'dashboard', command: 'sad.showDashboard' },
            { label: 'لوحة التحكم الكاملة', icon: 'layout', command: 'sad.commandCenter' },
            { label: 'مشروع جديد', icon: 'new-folder', command: 'sad.newProject', shortcut: 'Ctrl+Shift+N' },
            { label: 'إضافة وحدة', icon: 'new-file', command: 'sad.addModule' },
            { label: 'إضافة اختبار', icon: 'beaker', command: 'sad.addTest' },
            { label: 'الخط الزمني', icon: 'history', command: 'sad.showTimeline' },
            { label: 'قاعدة بيانات', icon: 'database', command: 'sad.openDatabase' },
        ]
    },
    {
        label: '👁️ العرض والاتجاه',
        icon: 'eye',
        children: [
            { label: 'تبديل RTL/LTR', icon: 'arrow-both', command: 'sad.toggleRTL', shortcut: 'Ctrl+Shift+;' },
            { label: 'لوحة RTL', icon: 'layout-sidebar-right', command: 'sad.openRTLPanel' },
            { label: 'معاينة الرسوميات', icon: 'preview', command: 'sad.previewGraphics', shortcut: 'Ctrl+Shift+G' },
        ]
    },
    {
        label: '📂 الطي',
        icon: 'fold',
        children: [
            { label: 'طي الدوال', icon: 'fold', command: 'sad.foldFunctions' },
            { label: 'طي الأصناف', icon: 'fold', command: 'sad.foldClasses' },
            { label: 'طي التعليقات', icon: 'fold', command: 'sad.foldComments' },
            { label: 'طي الاستيرادات', icon: 'fold', command: 'sad.foldImports' },
        ]
    },
];

class SadTreeItem extends vscode.TreeItem {
    constructor(
        public readonly label: string,
        public readonly collapsibleState: vscode.TreeItemCollapsibleState,
        public readonly itemCommand?: string,
        public readonly codicon?: string,
        public readonly shortcut?: string,
    ) {
        super(label, collapsibleState);

        if (codicon) {
            this.iconPath = new vscode.ThemeIcon(codicon);
        }

        if (itemCommand) {
            this.command = {
                title: label,
                command: itemCommand,
            };
            this.contextValue = 'sadFeature';
        } else {
            this.contextValue = 'sadCategory';
        }

        if (shortcut) {
            this.description = shortcut;
        }
    }
}

class SadSidebarProvider implements vscode.TreeDataProvider<SadTreeItem> {
    private _onDidChangeTreeData = new vscode.EventEmitter<SadTreeItem | undefined | null>();
    readonly onDidChangeTreeData = this._onDidChangeTreeData.event;

    private categoryMap = new Map<string, SidebarCategory>();

    constructor() {
        for (const cat of SIDEBAR_TREE) {
            this.categoryMap.set(cat.label, cat);
        }
    }

    refresh(): void {
        this._onDidChangeTreeData.fire(undefined);
    }

    getTreeItem(element: SadTreeItem): SadTreeItem {
        return element;
    }

    getChildren(element?: SadTreeItem): SadTreeItem[] {
        if (!element) {
            return SIDEBAR_TREE.map(cat =>
                new SadTreeItem(
                    cat.label,
                    vscode.TreeItemCollapsibleState.Collapsed,
                    undefined,
                    cat.icon,
                )
            );
        }

        const cat = this.categoryMap.get(element.label);
        if (cat) {
            return cat.children.map(item =>
                new SadTreeItem(
                    item.label,
                    vscode.TreeItemCollapsibleState.None,
                    item.command,
                    item.icon,
                    item.shortcut,
                )
            );
        }

        return [];
    }
}

class SadWelcomeProvider implements vscode.TreeDataProvider<SadTreeItem> {
    getTreeItem(element: SadTreeItem): SadTreeItem {
        return element;
    }

    getChildren(): SadTreeItem[] {
        return [
            new SadTreeItem(
                '▶️ تشغيل الملف الحالي',
                vscode.TreeItemCollapsibleState.None,
                'sad.runFile',
                'play',
                'F5',
            ),
            new SadTreeItem(
                '📄 ملف جديد',
                vscode.TreeItemCollapsibleState.None,
                'sad.newFile',
                'new-file',
            ),
            new SadTreeItem(
                '↔️ تبديل RTL/LTR',
                vscode.TreeItemCollapsibleState.None,
                'sad.toggleRTL',
                'arrow-both',
                'Ctrl+Shift+;',
            ),
            new SadTreeItem(
                '🎯 لوحة التحكم',
                vscode.TreeItemCollapsibleState.None,
                'sad.commandCenter',
                'layout',
            ),
            new SadTreeItem(
                '⚡ بحث سريع عن ميزة',
                vscode.TreeItemCollapsibleState.None,
                'sad.quickCommand',
                'search',
            ),
        ];
    }
}

export function registerSidebarView(context: vscode.ExtensionContext): void {
    const sidebarProvider = new SadSidebarProvider();
    const welcomeProvider = new SadWelcomeProvider();

    context.subscriptions.push(
        vscode.window.registerTreeDataProvider('sadFeatures', sidebarProvider),
        vscode.window.registerTreeDataProvider('sadQuickActions', welcomeProvider),
        vscode.commands.registerCommand('sad.refreshSidebar', () => sidebarProvider.refresh()),
    );
}