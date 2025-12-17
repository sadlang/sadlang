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

// Global variables - المتغيرات العامة
let client: LanguageClient | undefined;
let outputChannel: vscode.OutputChannel;

/**
 * @brief Activate extension - تفعيل الإضافة
 * 
 * يتم استدعاؤها عند تفعيل الإضافة لأول مرة
 * Called when the extension is activated
 */
export function activate(context: vscode.ExtensionContext) {
    outputChannel = vscode.window.createOutputChannel('Sad Language Server');
    outputChannel.appendLine('بسم الله الرحمن الرحيم');
    outputChannel.appendLine('Sad Language extension is activating...');
    outputChannel.appendLine('جاري تفعيل إضافة لغة ص...');

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

    // Start language server - بدء خادم اللغة
    startLanguageServer(context);

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
            const possiblePaths = [
                path.join(context.extensionPath, 'server', 'sad-lsp'),
                path.join(context.extensionPath, 'server', 'sad-lsp.exe'),
                path.join(context.extensionPath, '..', '..', 'build', 'tools', 'lsp', 'sad-lsp'),
                path.join(context.extensionPath, '..', '..', 'build', 'tools', 'lsp', 'sad-lsp.exe'),
                'sad-lsp', // In PATH
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
                fileEvents: vscode.workspace.createFileSystemWatcher('**/*.{s,sad}')
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

    const context = getExtensionContext();
    if (context) {
        await startLanguageServer(context);
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

    // Handle ready notification - معالجة إشعار الجاهزية
    client.onReady().then(() => {
        outputChannel.appendLine('Language server is ready!');
        outputChannel.appendLine('خادم اللغة جاهز!');
        
        vscode.window.showInformationMessage(
            'خادم لغة ص جاهز! / Sad Language Server is ready!'
        );
    });

    // Handle errors - معالجة الأخطاء
    client.onDidChangeState((event) => {
        outputChannel.appendLine(`Server state changed: ${event.newState}`);
    });
}

/**
 * @brief Get extension context - الحصول على سياق الإضافة
 */
function getExtensionContext(): vscode.ExtensionContext | undefined {
    const ext = vscode.extensions.getExtension('sad-language.sad-language');
    return ext?.isActive ? (ext as any).context : undefined;
}

/**
 * @brief Register additional features - تسجيل ميزات إضافية
 */
function registerAdditionalFeatures(context: vscode.ExtensionContext) {
    // Code lens provider - مزود عدسات الكود
    // Folding range provider - مزود نطاقات الطي
    // Symbol provider - مزود الرموز
    // TODO: Implement additional features
}
