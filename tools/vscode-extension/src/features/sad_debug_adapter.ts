/**
 * @file sad_debug_adapter.ts
 * @brief محول تصحيح DAP كامل للغة ص — Full DAP Debug Adapter for Sad Language
 * 
 * يتواصل مع مفسر sad.exe في وضع --debug-server عبر stdin/stdout
 * باستخدام بروتوكول JSON Lines (سطر JSON واحد لكل رسالة)
 * 
 * Communicates with sad.exe in --debug-server mode via stdin/stdout
 * using JSON Lines protocol (one JSON line per message)
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';
import * as path from 'path';
import * as fs from 'fs';
import { ChildProcess, spawn } from 'child_process';

// ═══════════════════════════════════════════════════════════════════
// أنواع البيانات — Data Types
// ═══════════════════════════════════════════════════════════════════

interface DebugMessage {
    type: 'event' | 'response' | 'request';
    event?: string;
    command?: string;
    request_seq?: number;
    seq?: number;
    success?: boolean;
    message?: string;
    body?: any;
}

interface SadBreakpoint {
    id: number;
    verified: boolean;
    line: number;
}

// ═══════════════════════════════════════════════════════════════════
// SadDebugSession — جلسة تصحيح كاملة
// ═══════════════════════════════════════════════════════════════════

export class SadDebugSession implements vscode.DebugAdapterDescriptorFactory {
    createDebugAdapterDescriptor(
        _session: vscode.DebugSession,
        _executable: vscode.DebugAdapterExecutable | undefined
    ): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
        return new vscode.DebugAdapterInlineImplementation(
            new SadInlineDebugAdapter()
        );
    }
}

// ═══════════════════════════════════════════════════════════════════
// SadInlineDebugAdapter — المحول المضمن
// ═══════════════════════════════════════════════════════════════════

export class SadInlineDebugAdapter implements vscode.DebugAdapter {
    private sendMessage = new vscode.EventEmitter<vscode.DebugProtocolMessage>();
    readonly onDidSendMessage: vscode.Event<vscode.DebugProtocolMessage> = this.sendMessage.event;

    private process: ChildProcess | null = null;
    private seq = 1;
    private pendingRequests = new Map<number, { resolve: (msg: any) => void; command: string }>();
    private lineBuffer = '';
    private disposed = false;
    private launchArgs: any = null;
    private configDone = false;
    private configDoneResolve: (() => void) | null = null;

    /**
     * @brief معالجة الرسائل الواردة من VS Code
     */
    handleMessage(message: vscode.DebugProtocolMessage): void {
        const msg = message as any;
        
        if (msg.type === 'request') {
            this.handleRequest(msg);
        }
    }

    private async handleRequest(request: any): Promise<void> {
        const { command, seq, arguments: args } = request;

        try {
            switch (command) {
                case 'initialize':
                    return this.handleInitialize(seq, args);
                case 'launch':
                    return await this.handleLaunch(seq, args);
                case 'setBreakpoints':
                    return await this.handleSetBreakpoints(seq, args);
                case 'setExceptionBreakpoints':
                    return this.sendResponseDirect(seq, command, {});
                case 'configurationDone':
                    return this.handleConfigurationDone(seq);
                case 'threads':
                    return this.handleThreads(seq);
                case 'stackTrace':
                    return await this.handleStackTrace(seq, args);
                case 'scopes':
                    return await this.handleScopes(seq, args);
                case 'variables':
                    return await this.handleVariables(seq, args);
                case 'continue':
                    return await this.handleContinue(seq, args);
                case 'next':
                    return await this.handleNext(seq, args);
                case 'stepIn':
                    return await this.handleStepIn(seq, args);
                case 'stepOut':
                    return await this.handleStepOut(seq, args);
                case 'pause':
                    return await this.handlePause(seq, args);
                case 'evaluate':
                    return await this.handleEvaluate(seq, args);
                case 'disconnect':
                    return await this.handleDisconnect(seq, args);
                case 'source':
                    return this.sendResponseDirect(seq, command, { content: '' });
                default:
                    return this.sendResponseDirect(seq, command, {});
            }
        } catch (e: any) {
            this.sendErrorResponse(seq, command, e.message || 'خطأ غير متوقع');
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // Initialize
    // ═══════════════════════════════════════════════════════════════

    private handleInitialize(seq: number, args: any): void {
        this.sendResponseDirect(seq, 'initialize', {
            supportsConfigurationDoneRequest: true,
            supportsConditionalBreakpoints: true,
            supportsHitConditionalBreakpoints: true,
            supportsEvaluateForHovers: true,
            supportsStepBack: false,
            supportTerminateDebuggee: true,
            supportsRestartFrame: false,
            supportsExceptionInfoRequest: true,
            supportsDelayedStackTraceLoading: false,
        });

        // إرسال حدث initialized
        this.sendEvent('initialized');
    }

    // ═══════════════════════════════════════════════════════════════
    // Launch
    // ═══════════════════════════════════════════════════════════════

    private async handleLaunch(seq: number, args: any): Promise<void> {
        this.launchArgs = args;
        const program = args.program as string;
        const stopOnEntry = args.stopOnEntry === true;

        if (!program) {
            this.sendErrorResponse(seq, 'launch', 'لم يُحدد ملف البرنامج / No program file specified');
            return;
        }

        if (!fs.existsSync(program)) {
            this.sendErrorResponse(seq, 'launch', `الملف غير موجود: ${program}`);
            return;
        }

        // إيجاد مسار المفسر
        const interpreterPath = this.findInterpreter(args);
        if (!interpreterPath) {
            this.sendErrorResponse(seq, 'launch', 
                'لم يُعثر على مفسر لغة ص (sad.exe). حدد المسار في الإعدادات.\n' +
                'Sad interpreter not found. Set path in settings.');
            return;
        }

        // تشغيل المفسر في وضع --debug-server
        const cwd = args.cwd || path.dirname(program);
        this.process = spawn(interpreterPath, ['--debug-server', program], {
            cwd,
            stdio: ['pipe', 'pipe', 'pipe'],
            env: { ...process.env, ...(args.env || {}) }
        });

        // استقبال المخرجات (JSON Lines)
        this.process.stdout!.on('data', (data: Buffer) => {
            this.onProcessOutput(data.toString('utf-8'));
        });

        // استقبال الأخطاء
        this.process.stderr!.on('data', (data: Buffer) => {
            const text = data.toString('utf-8');
            this.sendEvent('output', {
                category: 'stderr',
                output: text
            });
        });

        // انتهاء العملية
        this.process.on('exit', (code: number | null) => {
            this.sendEvent('terminated');
            this.sendEvent('exited', { exitCode: code || 0 });
        });

        this.process.on('error', (err: Error) => {
            this.sendEvent('output', {
                category: 'stderr',
                output: `خطأ في تشغيل المفسر: ${err.message}\n`
            });
            this.sendEvent('terminated');
        });

        // انتظار حتى يصبح المفسر جاهزاً
        await this.waitForReady();

        // إرسال initialize للمفسر
        await this.sendToDebugServer('initialize', {});

        this.sendResponseDirect(seq, 'launch', {});

        // إذا طلب التوقف عند الدخول
        if (stopOnEntry) {
            // سيتوقف عند أول سطر تلقائياً عبر step
            await this.sendToDebugServer('stepIn', {});
        }
    }

    private findInterpreter(args: any): string | null {
        // 1. من launch.json
        if (args.interpreter && fs.existsSync(args.interpreter)) {
            return args.interpreter;
        }

        // 2. من إعدادات VS Code
        const config = vscode.workspace.getConfiguration('sad');
        const configPath = config.get<string>('interpreter.path', '');
        if (configPath && fs.existsSync(configPath)) {
            return configPath;
        }

        // 3. بحث تلقائي في مسارات شائعة
        const workspaceRoot = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath || '';
        const candidates = [
            path.join(workspaceRoot, 'build', 'bin', 'Debug', 'sad.exe'),
            path.join(workspaceRoot, 'build', 'bin', 'Release', 'sad.exe'),
            path.join(workspaceRoot, 'build', 'bin', 'sad.exe'),
            path.join(workspaceRoot, 'build', 'Debug', 'sad.exe'),
            path.join(workspaceRoot, 'build', 'Release', 'sad.exe'),
        ];

        for (const p of candidates) {
            if (fs.existsSync(p)) return p;
        }

        return null;
    }

    private waitForReady(): Promise<void> {
        return new Promise((resolve) => {
            // ننتظر قليلاً لتهيئة المفسر
            setTimeout(resolve, 200);
        });
    }

    // ═══════════════════════════════════════════════════════════════
    // Configuration Done
    // ═══════════════════════════════════════════════════════════════

    private handleConfigurationDone(seq: number): void {
        this.configDone = true;
        if (this.configDoneResolve) {
            this.configDoneResolve();
            this.configDoneResolve = null;
        }

        // إرسال configurationDone للمفسر ثم بدء التنفيذ
        this.sendToDebugServer('configurationDone', {});
        this.sendToDebugServer('continue', {});
        
        this.sendResponseDirect(seq, 'configurationDone', {});
    }

    // ═══════════════════════════════════════════════════════════════
    // Breakpoints
    // ═══════════════════════════════════════════════════════════════

    private async handleSetBreakpoints(seq: number, args: any): Promise<void> {
        const source = args.source;
        const breakpoints = args.breakpoints || [];
        const filePath = source.path || '';

        // إرسال للمفسر
        const response = await this.sendToDebugServer('setBreakpoints', {
            source: { path: filePath },
            breakpoints: breakpoints.map((bp: any) => ({
                line: bp.line,
                condition: bp.condition || '',
                hitCondition: bp.hitCondition || ''
            })),
            lines: breakpoints.map((bp: any) => bp.line),
            path: filePath
        });

        // إرجاع النتيجة
        const verifiedBreakpoints = (response?.body?.breakpoints || []).map((bp: any) => ({
            id: bp.id,
            verified: bp.verified !== false,
            line: bp.line,
            source: { path: filePath }
        }));

        // fallback إذا لم يرد المفسر
        if (verifiedBreakpoints.length === 0) {
            for (const bp of breakpoints) {
                verifiedBreakpoints.push({
                    id: bp.line,
                    verified: true,
                    line: bp.line,
                    source: { path: filePath }
                });
            }
        }

        this.sendResponseDirect(seq, 'setBreakpoints', {
            breakpoints: verifiedBreakpoints
        });
    }

    // ═══════════════════════════════════════════════════════════════
    // Threads
    // ═══════════════════════════════════════════════════════════════

    private handleThreads(seq: number): void {
        this.sendResponseDirect(seq, 'threads', {
            threads: [{ id: 1, name: 'الرئيسي / Main Thread' }]
        });
    }

    // ═══════════════════════════════════════════════════════════════
    // Stack Trace
    // ═══════════════════════════════════════════════════════════════

    private async handleStackTrace(seq: number, args: any): Promise<void> {
        const response = await this.sendToDebugServer('stackTrace', {
            threadId: args.threadId || 1,
            startFrame: args.startFrame || 0,
            levels: args.levels || 20
        });

        const frames = (response?.body?.stackFrames || []).map((f: any) => ({
            id: f.id,
            name: f.name || '<غير معروف>',
            source: f.source ? { 
                name: path.basename(f.source.path || ''),
                path: f.source.path || ''
            } : undefined,
            line: f.line || 1,
            column: f.column || 1
        }));

        // fallback
        if (frames.length === 0 && this.launchArgs?.program) {
            frames.push({
                id: 0,
                name: '<رئيسي>',
                source: {
                    name: path.basename(this.launchArgs.program),
                    path: this.launchArgs.program
                },
                line: 1,
                column: 1
            });
        }

        this.sendResponseDirect(seq, 'stackTrace', {
            stackFrames: frames,
            totalFrames: frames.length
        });
    }

    // ═══════════════════════════════════════════════════════════════
    // Scopes
    // ═══════════════════════════════════════════════════════════════

    private async handleScopes(seq: number, args: any): Promise<void> {
        const response = await this.sendToDebugServer('scopes', {
            frameId: args.frameId || 0
        });

        const scopes = response?.body?.scopes || [
            { name: 'محلي / Local', variablesReference: 1, expensive: false },
            { name: 'عام / Global', variablesReference: 2, expensive: false }
        ];

        this.sendResponseDirect(seq, 'scopes', { scopes });
    }

    // ═══════════════════════════════════════════════════════════════
    // Variables
    // ═══════════════════════════════════════════════════════════════

    private async handleVariables(seq: number, args: any): Promise<void> {
        const response = await this.sendToDebugServer('variables', {
            variablesReference: args.variablesReference || 1
        });

        const variables = (response?.body?.variables || []).map((v: any) => ({
            name: v.name,
            value: v.value || '',
            type: v.type || '',
            variablesReference: v.variablesReference || 0,
            evaluateName: v.name
        }));

        this.sendResponseDirect(seq, 'variables', { variables });
    }

    // ═══════════════════════════════════════════════════════════════
    // Execution Control
    // ═══════════════════════════════════════════════════════════════

    private async handleContinue(seq: number, args: any): Promise<void> {
        await this.sendToDebugServer('continue', {});
        this.sendResponseDirect(seq, 'continue', { allThreadsContinued: true });
    }

    private async handleNext(seq: number, args: any): Promise<void> {
        await this.sendToDebugServer('next', {});
        this.sendResponseDirect(seq, 'next', {});
    }

    private async handleStepIn(seq: number, args: any): Promise<void> {
        await this.sendToDebugServer('stepIn', {});
        this.sendResponseDirect(seq, 'stepIn', {});
    }

    private async handleStepOut(seq: number, args: any): Promise<void> {
        await this.sendToDebugServer('stepOut', {});
        this.sendResponseDirect(seq, 'stepOut', {});
    }

    private async handlePause(seq: number, args: any): Promise<void> {
        await this.sendToDebugServer('pause', {});
        this.sendResponseDirect(seq, 'pause', {});
    }

    // ═══════════════════════════════════════════════════════════════
    // Evaluate
    // ═══════════════════════════════════════════════════════════════

    private async handleEvaluate(seq: number, args: any): Promise<void> {
        const response = await this.sendToDebugServer('evaluate', {
            expression: args.expression || '',
            context: args.context || 'repl',
            frameId: args.frameId || 0
        });

        this.sendResponseDirect(seq, 'evaluate', {
            result: response?.body?.result || '<لا نتيجة>',
            variablesReference: response?.body?.variablesReference || 0
        });
    }

    // ═══════════════════════════════════════════════════════════════
    // Disconnect
    // ═══════════════════════════════════════════════════════════════

    private async handleDisconnect(seq: number, args: any): Promise<void> {
        try {
            await this.sendToDebugServer('disconnect', {});
        } catch { }

        this.cleanup();
        this.sendResponseDirect(seq, 'disconnect', {});
    }

    private cleanup(): void {
        if (this.process) {
            try {
                this.process.kill();
            } catch { }
            this.process = null;
        }
        this.disposed = true;
    }

    // ═══════════════════════════════════════════════════════════════
    // Communication with sad.exe debug server
    // ═══════════════════════════════════════════════════════════════

    private sendToDebugServer(command: string, args: any): Promise<DebugMessage | null> {
        return new Promise((resolve) => {
            if (!this.process || !this.process.stdin) {
                resolve(null);
                return;
            }

            const msgSeq = this.seq++;
            const msg: any = {
                type: 'request',
                seq: msgSeq,
                command,
                arguments: args
            };

            // تسجيل callback للرد
            const timeout = setTimeout(() => {
                this.pendingRequests.delete(msgSeq);
                resolve(null);
            }, 5000);

            this.pendingRequests.set(msgSeq, {
                resolve: (response: any) => {
                    clearTimeout(timeout);
                    resolve(response);
                },
                command
            });

            // إرسال JSON line
            const jsonLine = JSON.stringify(msg) + '\n';
            try {
                this.process.stdin.write(jsonLine);
            } catch {
                clearTimeout(timeout);
                this.pendingRequests.delete(msgSeq);
                resolve(null);
            }
        });
    }

    private onProcessOutput(data: string): void {
        this.lineBuffer += data;
        
        let newlineIdx: number;
        while ((newlineIdx = this.lineBuffer.indexOf('\n')) >= 0) {
            const line = this.lineBuffer.substring(0, newlineIdx).trim();
            this.lineBuffer = this.lineBuffer.substring(newlineIdx + 1);

            if (line.length === 0) continue;

            try {
                const msg: DebugMessage = JSON.parse(line);
                this.handleDebugServerMessage(msg);
            } catch {
                // ليس JSON — مخرجات عادية
                this.sendEvent('output', {
                    category: 'stdout',
                    output: line + '\n'
                });
            }
        }
    }

    private handleDebugServerMessage(msg: DebugMessage): void {
        if (msg.type === 'response' && msg.request_seq !== undefined) {
            // رد على طلب سابق
            const pending = this.pendingRequests.get(msg.request_seq);
            if (pending) {
                this.pendingRequests.delete(msg.request_seq);
                pending.resolve(msg);
            }
        } else if (msg.type === 'event') {
            // حدث من المفسر
            switch (msg.event) {
                case 'stopped':
                    this.sendEvent('stopped', {
                        reason: msg.body?.reason || 'breakpoint',
                        threadId: 1,
                        allThreadsStopped: true,
                        text: msg.body?.text || '',
                        description: msg.body?.description || ''
                    });
                    break;

                case 'output':
                    this.sendEvent('output', {
                        category: msg.body?.category || 'stdout',
                        output: msg.body?.output || ''
                    });
                    break;

                case 'terminated':
                    this.sendEvent('terminated');
                    break;

                case 'exited':
                    this.sendEvent('exited', {
                        exitCode: msg.body?.exitCode || 0
                    });
                    break;

                case 'initialized':
                    // تم بالفعل إرسال initialized في handleInitialize
                    break;

                default:
                    // أحداث أخرى نمررها كما هي
                    if (msg.event) {
                        this.sendEvent(msg.event, msg.body || {});
                    }
                    break;
            }
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // DAP Message Sending
    // ═══════════════════════════════════════════════════════════════

    private sendResponseDirect(requestSeq: number, command: string, body: any): void {
        this.sendMessage.fire({
            type: 'response',
            request_seq: requestSeq,
            seq: this.seq++,
            command,
            success: true,
            body
        } as any);
    }

    private sendErrorResponse(requestSeq: number, command: string, message: string): void {
        this.sendMessage.fire({
            type: 'response',
            request_seq: requestSeq,
            seq: this.seq++,
            command,
            success: false,
            message,
            body: { error: { id: 1, format: message } }
        } as any);
    }

    private sendEvent(event: string, body: any = {}): void {
        this.sendMessage.fire({
            type: 'event',
            seq: this.seq++,
            event,
            body
        } as any);
    }

    dispose(): void {
        this.cleanup();
        this.sendMessage.dispose();
    }
}

// ═══════════════════════════════════════════════════════════════════
// SadConfigurationProvider — يوفر launch.json افتراضي
// ═══════════════════════════════════════════════════════════════════

export class SadConfigurationProvider implements vscode.DebugConfigurationProvider {
    resolveDebugConfiguration(
        folder: vscode.WorkspaceFolder | undefined,
        config: vscode.DebugConfiguration,
        _token?: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.DebugConfiguration> {
        // إذا لم يوجد launch.json أو كان فارغاً
        if (!config.type && !config.request && !config.name) {
            const editor = vscode.window.activeTextEditor;
            if (editor && editor.document.languageId === 'sad') {
                config.type = 'sad';
                config.name = 'تصحيح ملف ص / Debug Sad File';
                config.request = 'launch';
                config.program = '${file}';
                config.stopOnEntry = true;
            }
        }

        if (!config.program) {
            return vscode.window.showInformationMessage(
                'لم يُحدد ملف البرنامج / No program specified'
            ).then(_ => undefined);
        }

        return config;
    }

    provideDebugConfigurations(
        folder: vscode.WorkspaceFolder | undefined,
        _token?: vscode.CancellationToken
    ): vscode.ProviderResult<vscode.DebugConfiguration[]> {
        return [
            {
                type: 'sad',
                request: 'launch',
                name: 'تصحيح ملف ص الحالي / Debug Current Sad File',
                program: '${file}',
                stopOnEntry: true
            },
            {
                type: 'sad',
                request: 'launch',
                name: 'تشغيل بدون توقف / Run Without Stopping',
                program: '${file}',
                stopOnEntry: false
            }
        ];
    }
}
