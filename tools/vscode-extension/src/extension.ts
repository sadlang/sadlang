// بسم الله الرحمن الرحيم
/**
 * @file extension.ts
 * @brief VS Code Extension for Sad Language - إضافة VS Code للغة ص
 * 
 * هذا الملف يحتوي على الإضافة الرئيسية لدعم لغة ص في VS Code
 * يتعامل مع خادم اللغة (LSP) ويوفر ميزات البرمجة المتقدمة
 * 
 * This file contains the main extension for Sad language support in VS Code
 * Handles the language server (LSP) and provides advanced programming features
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

import * as path from 'path';
import * as vscode from 'vscode';
import {
    LanguageClient,
    LanguageClientOptions,
    ServerOptions,
    TransportKind,
    Executable
} from 'vscode-languageclient/node';

// ─── استيراد الميزات العشر الأصلية ───
import { registerAutoDoc } from './features/auto_doc';
import { registerCodeLens } from './features/code_lens';
import { registerCodeReview } from './features/code_review';
import { registerCodeTranslator } from './features/code_translator';
import { registerASTExplorer } from './features/ast_explorer';
import { registerDebugger } from './features/debugger';
import { SadDebugSession, SadConfigurationProvider } from './features/sad_debug_adapter';
import { registerGraphicsPreview } from './features/graphics_preview';
import { registerTutorial } from './features/tutorial';
import { registerPerformanceAnalyzer } from './features/performance';
import { registerTestExplorer } from './features/test_explorer';
import { registerRTLSupport } from './features/rtl_support';

// ─── استيراد الميزات العشرين الثورية الجديدة ───
import { registerErrorLens } from './features/error_lens';
import { registerVariableInspector } from './features/variable_inspector';
import { registerInlineRunner } from './features/inline_runner';
import { registerCallHierarchy } from './features/call_hierarchy';
import { registerDependencyGraph } from './features/dependency_graph';
import { registerCodeComplexity } from './features/code_complexity';
import { registerImportOrganizer } from './features/import_organizer';
import { registerSnippetGenerator } from './features/snippet_generator';
import { registerColorPicker } from './features/color_picker';
import { registerProjectDashboard } from './features/project_dashboard';
import { registerBookmarkManager } from './features/bookmark_manager';
import { registerCodeTimeline } from './features/code_timeline';
import { registerSmartRename } from './features/smart_rename';
import { registerTerminalIntegration } from './features/terminal_integration';
import { registerProjectScaffolding } from './features/project_scaffolding';
import { registerMinimapAnnotations } from './features/minimap_annotations';
import { registerMarkdownDocPreview } from './features/markdown_doc_preview';
import { registerLiveShare } from './features/live_share';
import { registerAIAssistant } from './features/ai_assistant';
import { registerDatabaseExplorer } from './features/database_explorer';
import { registerFoldingProvider } from './features/folding_provider';
import { registerBreadcrumbProvider } from './features/breadcrumb_provider';
import { registerCommandCenter } from './features/command_center';
import { registerSidebarView } from './features/sidebar_view';
import { registerIntelliSense } from './features/intellisense';

// Global variables - المتغيرات العامة
let client: LanguageClient | undefined;
let outputChannel: vscode.OutputChannel;
let extensionContext: vscode.ExtensionContext | undefined;
let executionModeStatusBar: vscode.StatusBarItem;

/**
 * @brief Activate extension - تفعيل الإضافة
 * 
 * يتم استدعاؤها عند تفعيل الإضافة لأول مرة
 * Called when the extension is activated
 */
export function activate(context: vscode.ExtensionContext) {
    extensionContext = context;
    outputChannel = vscode.window.createOutputChannel('Sad Language Server');
    outputChannel.appendLine('بسم الله الرحمن الرحيم');
    outputChannel.appendLine('Sad Language extension is activating...');
    outputChannel.appendLine('جاري تفعيل إضافة لغة ص...');

    // ═══════════════════════════════════════════════════════════════
    // تسجيل الشريط الجانبي مباشرة وفوراً — قبل أي شيء آخر
    // Register sidebar providers INLINE immediately — before anything else
    // ═══════════════════════════════════════════════════════════════
    try {
        // Quick Actions provider — إجراءات سريعة
        const quickActionsProvider: vscode.TreeDataProvider<vscode.TreeItem> = {
            getTreeItem: (element: vscode.TreeItem) => element,
            getChildren: () => {
                const items = [
                    { label: '▶️ تشغيل الملف الحالي', icon: 'play', cmd: 'sad.runFile' },
                    { label: '📄 ملف جديد', icon: 'new-file', cmd: 'sad.newFile' },
                    { label: '↔️ تبديل RTL/LTR', icon: 'arrow-both', cmd: 'sad.toggleRTL' },
                    { label: '🎯 لوحة التحكم', icon: 'layout', cmd: 'sad.commandCenter' },
                ];
                return items.map(i => {
                    const item = new vscode.TreeItem(i.label);
                    item.iconPath = new vscode.ThemeIcon(i.icon);
                    item.command = { title: i.label, command: i.cmd };
                    return item;
                });
            }
        };

        // Features provider — جميع الميزات
        const featureCategories = [
            { label: '🚀 التشغيل والتصحيح', icon: 'play', items: [
                { label: 'تشغيل الملف', icon: 'play', cmd: 'sad.runFile' },
                { label: 'تشغيل بالمفسر', icon: 'play-circle', cmd: 'sad.runWithInterpreter' },
                { label: 'ترجمة وتشغيل', icon: 'zap', cmd: 'sad.runWithCompiler' },
                { label: 'فتح طرفية ص', icon: 'terminal-bash', cmd: 'sad.openTerminal' },
                { label: 'التصحيح المرئي', icon: 'debug-alt', cmd: 'sad.debug' },
            ]},
            { label: '🔬 التحليل والمراجعة', icon: 'search', items: [
                { label: 'تحليل التعقيد', icon: 'pulse', cmd: 'sad.analyzeComplexity' },
                { label: 'تحليل الأداء', icon: 'dashboard', cmd: 'sad.analyzePerformance' },
                { label: 'مراجعة الكود', icon: 'checklist', cmd: 'sad.reviewCode' },
                { label: 'مستكشف AST', icon: 'list-tree', cmd: 'sad.showAST' },
            ]},
            { label: '✏️ التحرير والتنسيق', icon: 'edit', items: [
                { label: 'إعادة التسمية الذكية', icon: 'edit', cmd: 'sad.smartRename' },
                { label: 'تنظيم الاستيرادات', icon: 'sort-precedence', cmd: 'sad.organizeImports' },
                { label: 'توليد قصاصة', icon: 'diff', cmd: 'sad.generateSnippet' },
            ]},
            { label: '📚 التوثيق', icon: 'book', items: [
                { label: 'توليد التوثيق', icon: 'note', cmd: 'sad.generateDoc' },
                { label: 'معاينة التوثيق', icon: 'open-preview', cmd: 'sad.previewDocs' },
            ]},
            { label: '🛠️ المشروع والأدوات', icon: 'tools', items: [
                { label: 'لوحة القيادة', icon: 'dashboard', cmd: 'sad.showDashboard' },
                { label: 'لوحة التحكم الكاملة', icon: 'layout', cmd: 'sad.commandCenter' },
                { label: 'مشروع جديد', icon: 'new-folder', cmd: 'sad.newProject' },
            ]},
        ];

        type CategoryItem = { label: string; icon: string; items: { label: string; icon: string; cmd: string }[] };
        const categoryMap = new Map<string, CategoryItem>();
        featureCategories.forEach(c => categoryMap.set(c.label, c));

        const featuresProvider: vscode.TreeDataProvider<vscode.TreeItem> = {
            onDidChangeTreeData: undefined,
            getTreeItem: (element: vscode.TreeItem) => element,
            getChildren: (element?: vscode.TreeItem) => {
                if (!element) {
                    return featureCategories.map(cat => {
                        const item = new vscode.TreeItem(cat.label, vscode.TreeItemCollapsibleState.Collapsed);
                        item.iconPath = new vscode.ThemeIcon(cat.icon);
                        return item;
                    });
                }
                const cat = categoryMap.get(element.label as string);
                if (cat) {
                    return cat.items.map(i => {
                        const item = new vscode.TreeItem(i.label, vscode.TreeItemCollapsibleState.None);
                        item.iconPath = new vscode.ThemeIcon(i.icon);
                        item.command = { title: i.label, command: i.cmd };
                        return item;
                    });
                }
                return [];
            }
        };

        // Use createTreeView — الأكثر موثوقية
        const featuresView = vscode.window.createTreeView('sadFeatures', {
            treeDataProvider: featuresProvider,
            showCollapseAll: true,
        });
        const quickActionsView = vscode.window.createTreeView('sadQuickActions', {
            treeDataProvider: quickActionsProvider,
        });
        context.subscriptions.push(featuresView, quickActionsView);
        outputChannel.appendLine('✅ تم تسجيل الشريط الجانبي بنجاح');
    } catch (sidebarError) {
        outputChannel.appendLine(`❌ خطأ في تسجيل الشريط الجانبي: ${sidebarError}`);
        console.error('Sidebar registration failed:', sidebarError);
    }

    // Register commands - تسجيل الأوامر
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.restartServer', async () => {
            await restartServer();
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.showOutputChannel', () => {
            outputChannel.show();
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.runFile', async () => {
            await runCurrentFile();
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.runWithInterpreter', async () => {
            await runWithInterpreter();
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.runWithCompiler', async () => {
            await runWithCompiler();
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.switchExecutionMode', async () => {
            await switchExecutionMode();
        })
    );

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.newFile', async () => {
            await createNewSadFile();
        })
    );

    // ─── شريط حالة وضع التنفيذ ───
    executionModeStatusBar = vscode.window.createStatusBarItem(
        vscode.StatusBarAlignment.Left, 50
    );
    executionModeStatusBar.command = 'sad.switchExecutionMode';
    executionModeStatusBar.tooltip = 'تبديل وضع التنفيذ: مفسر / مترجم — Switch execution mode: Interpreter / Compiler';
    context.subscriptions.push(executionModeStatusBar);
    updateExecutionModeStatusBar();

    // إظهار/إخفاء شريط الحالة حسب نوع الملف المفتوح
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor((editor) => {
            if (editor && editor.document.languageId === 'sad') {
                executionModeStatusBar.show();
            } else {
                executionModeStatusBar.hide();
            }
        })
    );
    if (vscode.window.activeTextEditor?.document.languageId === 'sad') {
        executionModeStatusBar.show();
    }

    // Start language server - بدء خادم اللغة
    startLanguageServer(context);

    // ─── دالة مساعدة لتسجيل آمن ───
    const safeRegister = (name: string, fn: (ctx: vscode.ExtensionContext) => void) => {
        try {
            fn(context);
        } catch (e) {
            outputChannel.appendLine(`⚠️ فشل تسجيل ${name}: ${e}`);
        }
    };

    // ─── تم تسجيل الشريط الجانبي أعلاه مباشرة ───
    // safeRegister('SidebarView', registerSidebarView);  // INLINED above
    safeRegister('CommandCenter', registerCommandCenter);
    safeRegister('IntelliSense', registerIntelliSense);

    // ─── تسجيل الميزات العشر الأصلية ───
    safeRegister('AutoDoc', registerAutoDoc);
    safeRegister('CodeLens', registerCodeLens);
    safeRegister('CodeReview', registerCodeReview);
    safeRegister('CodeTranslator', registerCodeTranslator);
    safeRegister('ASTExplorer', registerASTExplorer);
    safeRegister('Debugger', registerDebugger);

    // ─── تسجيل محول تصحيح DAP الأصلي ───
    safeRegister('DAP Debugger', () => {
        const debugSession = new SadDebugSession();
        const configProvider = new SadConfigurationProvider();
        context.subscriptions.push(
            vscode.debug.registerDebugAdapterDescriptorFactory('sad', debugSession),
            vscode.debug.registerDebugConfigurationProvider('sad', configProvider)
        );
    });
    safeRegister('GraphicsPreview', registerGraphicsPreview);
    safeRegister('Tutorial', registerTutorial);
    safeRegister('PerformanceAnalyzer', registerPerformanceAnalyzer);
    safeRegister('TestExplorer', registerTestExplorer);
    safeRegister('RTLSupport', registerRTLSupport);

    // ─── تسجيل الميزات العشرين الثورية الجديدة ───
    safeRegister('ErrorLens', registerErrorLens);
    safeRegister('VariableInspector', registerVariableInspector);
    safeRegister('InlineRunner', registerInlineRunner);
    safeRegister('CallHierarchy', registerCallHierarchy);
    safeRegister('DependencyGraph', registerDependencyGraph);
    safeRegister('CodeComplexity', registerCodeComplexity);
    safeRegister('ImportOrganizer', registerImportOrganizer);
    safeRegister('SnippetGenerator', registerSnippetGenerator);
    safeRegister('ColorPicker', registerColorPicker);
    safeRegister('ProjectDashboard', registerProjectDashboard);
    safeRegister('BookmarkManager', registerBookmarkManager);
    safeRegister('CodeTimeline', registerCodeTimeline);
    safeRegister('SmartRename', registerSmartRename);
    safeRegister('TerminalIntegration', registerTerminalIntegration);
    safeRegister('ProjectScaffolding', registerProjectScaffolding);
    safeRegister('MinimapAnnotations', registerMinimapAnnotations);
    safeRegister('MarkdownDocPreview', registerMarkdownDocPreview);
    safeRegister('LiveShare', registerLiveShare);
    safeRegister('AIAssistant', registerAIAssistant);
    safeRegister('DatabaseExplorer', registerDatabaseExplorer);
    safeRegister('FoldingProvider', registerFoldingProvider);
    safeRegister('BreadcrumbProvider', registerBreadcrumbProvider);

    outputChannel.appendLine('Extension activated successfully!');
    outputChannel.appendLine('تم تفعيل الإضافة بنجاح!');
}

/**
 * @brief Deactivate extension - إلغاء تفعيل الإضافة
 * 
 * يتم استدعاؤها عند إلغاء تفعيل الإضافة
 * Called when the extension is deactivated
 */
export function deactivate(): Thenable<void> | undefined {
    if (!client) {
        return undefined;
    }
    
    outputChannel.appendLine('Deactivating extension...');
    outputChannel.appendLine('جاري إلغاء تفعيل الإضافة...');
    
    return client.stop();
}

/**
 * @brief Start language server - بدء خادم اللغة
 */
async function startLanguageServer(context: vscode.ExtensionContext) {
    try {
        // Get configuration - الحصول على الإعدادات
        const config = vscode.workspace.getConfiguration('sad');
        const enabled = config.get<boolean>('server.enabled', true);

        if (!enabled) {
            outputChannel.appendLine('Language server is disabled in settings');
            outputChannel.appendLine('خادم اللغة معطّل في الإعدادات');
            return;
        }

        // Find server executable - إيجاد ملف تنفيذي الخادم
        let serverPath = config.get<string>('server.path', '');
        
        if (!serverPath) {
            // Try to find in common locations - محاولة إيجاد في المواقع الشائعة
            // نبحث في عدة مسارات محتملة بالترتيب:
            //   ① مجلد الإضافة (عند التوزيع)
            //   ② مجلد البناء المحلي (عند التطوير)
            //   ③ في PATH (عند التثبيت العام)
            const workspaceRoot = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || '';
            const possiblePaths = [
                // داخل مجلد الإضافة (للتوزيع)
                path.join(context.extensionPath, 'server', 'sad-lsp'),
                path.join(context.extensionPath, 'server', 'sad-lsp.exe'),
                // مجلد البناء المحلي - Debug
                path.join(workspaceRoot, 'build', 'bin', 'Debug', 'sad-lsp.exe'),
                path.join(workspaceRoot, 'build', 'bin', 'Debug', 'sad-lsp'),
                // مجلد البناء المحلي - Release
                path.join(workspaceRoot, 'build', 'bin', 'Release', 'sad-lsp.exe'),
                path.join(workspaceRoot, 'build', 'bin', 'Release', 'sad-lsp'),
                // مجلد البناء المحلي - بدون config
                path.join(workspaceRoot, 'build', 'bin', 'sad-lsp.exe'),
                path.join(workspaceRoot, 'build', 'bin', 'sad-lsp'),
                // مسار الإضافة القديم
                path.join(context.extensionPath, '..', '..', 'build', 'bin', 'Debug', 'sad-lsp.exe'),
                path.join(context.extensionPath, '..', '..', 'build', 'bin', 'Release', 'sad-lsp.exe'),
                // في PATH
                'sad-lsp',
                'sad-lsp.exe'
            ];

            for (const p of possiblePaths) {
                try {
                    const fs = require('fs');
                    if (fs.existsSync(p)) {
                        serverPath = p;
                        break;
                    }
                } catch (e) {
                    // Continue to next path
                }
            }

            if (!serverPath) {
                const message = 'لم يتم العثور على خادم اللغة. يرجى تحديد المسار في الإعدادات.\n' +
                                'Language server not found. Please specify the path in settings.';
                vscode.window.showErrorMessage(message);
                outputChannel.appendLine('ERROR: ' + message);
                return;
            }
        }

        outputChannel.appendLine(`Using server at: ${serverPath}`);
        outputChannel.appendLine(`استخدام الخادم في: ${serverPath}`);

        // Server options - خيارات الخادم
        const serverOptions: ServerOptions = {
            command: serverPath,
            args: [],
            options: {
                cwd: vscode.workspace.workspaceFolders?.[0]?.uri.fsPath
            }
        };

        // Client options - خيارات العميل
        const clientOptions: LanguageClientOptions = {
            // Document selector - محدد المستندات
            documentSelector: [
                { scheme: 'file', language: 'sad' },
                { scheme: 'untitled', language: 'sad' }
            ],
            
            // Synchronization options - خيارات المزامنة
            synchronize: {
                // Notify server about file changes - إشعار الخادم بتغييرات الملفات
                fileEvents: vscode.workspace.createFileSystemWatcher('**/*.ص')
            },
            
            // Output channel - قناة الإخراج
            outputChannel: outputChannel,
            
            // Trace level - مستوى التتبع
            traceOutputChannel: outputChannel,
            
            // Initialization options - خيارات التهيئة
            initializationOptions: {
                locale: vscode.env.language,
                formatting: {
                    tabSize: config.get('formatting.tabSize', 4),
                    insertSpaces: config.get('formatting.insertSpaces', true)
                }
            }
        };

        // Create language client - إنشاء عميل اللغة
        client = new LanguageClient(
            'sadLanguageServer',
            'Sad Language Server / خادم لغة ص',
            serverOptions,
            clientOptions
        );

        // Register client handlers - تسجيل معالجات العميل
        registerClientHandlers();

        // Start client - بدء العميل
        await client.start();

        outputChannel.appendLine('Language server started successfully!');
        outputChannel.appendLine('تم بدء خادم اللغة بنجاح!');

    } catch (error) {
        const message = `Failed to start language server: ${error}`;
        outputChannel.appendLine('ERROR: ' + message);
        vscode.window.showErrorMessage(message);
    }
}

/**
 * @brief Restart language server - إعادة تشغيل خادم اللغة
 */
async function restartServer() {
    outputChannel.appendLine('Restarting language server...');
    outputChannel.appendLine('جاري إعادة تشغيل خادم اللغة...');

    if (client) {
        await client.stop();
        client = undefined;
    }

    if (extensionContext) {
        await startLanguageServer(extensionContext);
    } else {
        outputChannel.appendLine('ERROR: Extension context not available');
    }
}

/**
 * @brief Register client handlers - تسجيل معالجات العميل
 */
function registerClientHandlers() {
    if (!client) return;

    // Handle custom notifications - معالجة الإشعارات المخصصة
    client.onNotification('sad/status', (params: any) => {
        outputChannel.appendLine(`Server status: ${params.message}`);
    });

    // Handle state changes - معالجة تغيرات الحالة
    client.onDidChangeState((event) => {
        outputChannel.appendLine(`Server state changed: ${event.newState}`);
        if (event.newState === 2 /* Running */) {
            outputChannel.appendLine('Language server is ready!');
            outputChannel.appendLine('خادم اللغة جاهز!');
            vscode.window.showInformationMessage(
                'خادم لغة ص جاهز! / Sad Language Server is ready!'
            );
        }
    });
}

/**
 * @brief Run current .ص file - تشغيل ملف ص الحالي
 * 
 * يتحقق من وضع التنفيذ المحدد في الإعدادات:
 *   - "interpreter" → يشغّل بالمفسر (sad.exe)
 *   - "compiler"   → يترجم بالمترجم (sadc.exe) ثم يشغّل الملف التنفيذي
 *   - "ask"        → يسأل المستخدم في كل مرة
 */
async function runCurrentFile() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') {
        vscode.window.showWarningMessage('لا يوجد ملف .ص مفتوح / No .ص file is open');
        return;
    }

    const config = vscode.workspace.getConfiguration('sad');
    const mode = config.get<string>('execution.mode', 'ask');

    if (mode === 'interpreter') {
        await runWithInterpreter();
    } else if (mode === 'compiler') {
        await runWithCompiler();
    } else {
        // وضع "ask": اسأل المستخدم
        const choice = await vscode.window.showQuickPick([
            {
                label: '$(play) مفسر (Interpreter)',
                description: 'تشغيل مباشر بالمفسر — sad.exe',
                mode: 'interpreter'
            },
            {
                label: '$(gear) مترجم (Compiler)',
                description: 'ترجمة إلى ملف تنفيذي ثم تشغيل — sadc.exe',
                mode: 'compiler'
            }
        ], {
            placeHolder: 'اختر وضع التنفيذ — Choose execution mode',
            title: 'تشغيل ملف ص — Run Sad File'
        });

        if (choice) {
            if (choice.mode === 'interpreter') {
                await runWithInterpreter();
            } else {
                await runWithCompiler();
            }
        }
    }
}

/**
 * @brief تشغيل بالمفسر — Run with interpreter (sad.exe)
 */
async function runWithInterpreter() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') {
        vscode.window.showWarningMessage('لا يوجد ملف .ص مفتوح / No .ص file is open');
        return;
    }

    await editor.document.save();

    const filePath = editor.document.uri.fsPath;
    const interpreterPath = findExecutable('interpreter');

    if (!interpreterPath) {
        vscode.window.showErrorMessage(
            'لم يتم العثور على مفسر لغة ص (sad.exe). يرجى تحديد المسار في الإعدادات.\n' +
            'Sad interpreter not found. Please set sad.interpreter.path in settings.'
        );
        return;
    }

    const isWindows = process.platform === 'win32';
    const terminal = vscode.window.createTerminal({
        name: `ص (مفسر): ${path.basename(filePath)}`,
        cwd: path.dirname(filePath),
        env: isWindows ? { 'PYTHONIOENCODING': 'utf-8' } : undefined
    });
    terminal.show();
    
    if (isWindows) {
        terminal.sendText('chcp 65001 > $null');
        terminal.sendText(`[Console]::OutputEncoding = [System.Text.Encoding]::UTF8`);
    }
    terminal.sendText(`"${interpreterPath}" "${filePath}"`);

    outputChannel.appendLine(`[مفسر] تشغيل: ${filePath}`);
}

/**
 * @brief تشغيل بالمترجم — Run with compiler (sadc.exe)
 * 
 * المراحل:
 *   1. ترجمة .ص إلى ملف تنفيذي باستخدام sadc.exe
 *   2. تشغيل الملف التنفيذي الناتج
 */
async function runWithCompiler() {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') {
        vscode.window.showWarningMessage('لا يوجد ملف .ص مفتوح / No .ص file is open');
        return;
    }

    await editor.document.save();

    const filePath = editor.document.uri.fsPath;
    const compilerPath = findExecutable('compiler');

    if (!compilerPath) {
        vscode.window.showErrorMessage(
            'لم يتم العثور على مترجم لغة ص (sadc.exe). يرجى تحديد المسار في الإعدادات.\n' +
            'Sad compiler not found. Please set sad.compiler.path in settings.'
        );
        return;
    }

    const isWindows = process.platform === 'win32';
    const fileDir = path.dirname(filePath);
    const baseName = path.basename(filePath, path.extname(filePath));
    // اسم الملف التنفيذي الناتج
    const outputExe = path.join(fileDir, baseName + (isWindows ? '.exe' : ''));

    const terminal = vscode.window.createTerminal({
        name: `ص (مترجم): ${path.basename(filePath)}`,
        cwd: fileDir,
        env: isWindows ? { 'PYTHONIOENCODING': 'utf-8' } : undefined
    });
    terminal.show();

    if (isWindows) {
        terminal.sendText('chcp 65001 > $null');
        terminal.sendText(`[Console]::OutputEncoding = [System.Text.Encoding]::UTF8`);
    }

    // المرحلة 1: الترجمة
    terminal.sendText(`Write-Host "═══ مرحلة الترجمة ═══" -ForegroundColor Cyan`);
    terminal.sendText(`Write-Host "المترجم: ${compilerPath}" -ForegroundColor DarkGray`);
    terminal.sendText(`Write-Host "الملف: ${filePath}" -ForegroundColor DarkGray`);
    terminal.sendText(`Write-Host "الناتج: ${outputExe}" -ForegroundColor DarkGray`);
    terminal.sendText(`Write-Host ""`);

    // ترجمة الملف (sadc يأخذ ملف المصدر وينتج ملف تنفيذي)
    terminal.sendText(`& "${compilerPath}" "${filePath}" -o "${outputExe}"`);

    // المرحلة 2: التشغيل (فقط إذا نجحت الترجمة)
    terminal.sendText(`if ($LASTEXITCODE -eq 0) {`);
    terminal.sendText(`    Write-Host ""`);
    terminal.sendText(`    Write-Host "═══ مرحلة التشغيل ═══" -ForegroundColor Green`);
    terminal.sendText(`    & "${outputExe}"`);
    terminal.sendText(`} else {`);
    terminal.sendText(`    Write-Host "فشلت الترجمة! رمز الخطأ: $LASTEXITCODE" -ForegroundColor Red`);
    terminal.sendText(`}`);

    outputChannel.appendLine(`[مترجم] ترجمة وتشغيل: ${filePath}`);
    outputChannel.appendLine(`[مترجم] ملف تنفيذي: ${outputExe}`);
}

/**
 * @brief ايجاد المسار التنفيذي — Find executable path
 * @param type 'interpreter' | 'compiler'
 */
function findExecutable(type: 'interpreter' | 'compiler'): string {
    const config = vscode.workspace.getConfiguration('sad');
    const fs = require('fs');
    const workspaceRoot = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || '';

    // تحقق من الإعداد المخصص أولاً
    const customPath = config.get<string>(
        type === 'interpreter' ? 'interpreter.path' : 'compiler.path', ''
    );
    if (customPath && fs.existsSync(customPath)) {
        return customPath;
    }

    const exeName = type === 'interpreter' ? 'sad' : 'sadc';
    const possiblePaths = [
        path.join(workspaceRoot, 'build', 'bin', 'Debug', exeName + '.exe'),
        path.join(workspaceRoot, 'build', 'bin', 'Release', exeName + '.exe'),
        path.join(workspaceRoot, 'build', 'bin', exeName + '.exe'),
        path.join(workspaceRoot, 'build', 'bin', 'Debug', exeName),
        path.join(workspaceRoot, 'build', 'bin', 'Release', exeName),
        path.join(workspaceRoot, 'build', 'bin', exeName),
        exeName, exeName + '.exe'
    ];

    for (const p of possiblePaths) {
        try {
            if (fs.existsSync(p)) { return p; }
        } catch { /* continue */ }
    }

    return '';
}

/**
 * @brief تبديل وضع التنفيذ — Switch execution mode
 */
async function switchExecutionMode() {
    const config = vscode.workspace.getConfiguration('sad');
    const currentMode = config.get<string>('execution.mode', 'ask');

    const choices = [
        {
            label: '$(question) اسأل كل مرة (Ask)',
            description: currentMode === 'ask' ? '✓ الوضع الحالي' : '',
            mode: 'ask'
        },
        {
            label: '$(play) مفسر (Interpreter)',
            description: (currentMode === 'interpreter' ? '✓ الوضع الحالي — ' : '') + 'تشغيل مباشر بـ sad.exe',
            mode: 'interpreter'
        },
        {
            label: '$(gear) مترجم (Compiler)',
            description: (currentMode === 'compiler' ? '✓ الوضع الحالي — ' : '') + 'ترجمة وتشغيل بـ sadc.exe',
            mode: 'compiler'
        }
    ];

    const choice = await vscode.window.showQuickPick(choices, {
        placeHolder: 'اختر وضع التنفيذ الافتراضي — Choose default execution mode',
        title: 'وضع التنفيذ — Execution Mode'
    });

    if (choice) {
        await config.update('execution.mode', choice.mode, vscode.ConfigurationTarget.Global);
        updateExecutionModeStatusBar();
        const modeNames: Record<string, string> = {
            'ask': 'اسأل كل مرة',
            'interpreter': 'مفسر (sad.exe)',
            'compiler': 'مترجم (sadc.exe)'
        };
        vscode.window.showInformationMessage(`وضع التنفيذ: ${modeNames[choice.mode]} — Execution mode updated`);
    }
}

/**
 * @brief تحديث شريط حالة وضع التنفيذ — Update execution mode status bar
 */
function updateExecutionModeStatusBar() {
    const config = vscode.workspace.getConfiguration('sad');
    const mode = config.get<string>('execution.mode', 'ask');

    switch (mode) {
        case 'interpreter':
            executionModeStatusBar.text = '$(play) مفسر';
            executionModeStatusBar.backgroundColor = undefined;
            break;
        case 'compiler':
            executionModeStatusBar.text = '$(gear) مترجم';
            executionModeStatusBar.backgroundColor = new vscode.ThemeColor('statusBarItem.prominentBackground');
            break;
        default: // ask
            executionModeStatusBar.text = '$(question) تنفيذ';
            executionModeStatusBar.backgroundColor = undefined;
            break;
    }
}

/**
 * @brief Create a new .ص file - إنشاء ملف ص جديد
 */
async function createNewSadFile() {
    const defaultContent = [
        '# بسم الله الرحمن الرحيم',
        '',
        'دالة رئيسية()',
        '    اطبع_سطر("مرحباً بالعالم!")',
        'نهاية',
        ''
    ].join('\n');

    const doc = await vscode.workspace.openTextDocument({
        language: 'sad',
        content: defaultContent
    });
    await vscode.window.showTextDocument(doc);
}


