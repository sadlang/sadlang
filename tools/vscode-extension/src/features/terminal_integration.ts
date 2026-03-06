import * as vscode from 'vscode';
import * as path from 'path';
import * as cp from 'child_process';
import * as fs from 'fs';
import * as os from 'os';

let historyItems: string[] = [];

export function registerTerminalIntegration(context: vscode.ExtensionContext): void {
    historyItems = context.workspaceState.get<string[]>('sadTerminalHistory', []);

    context.subscriptions.push(
        vscode.commands.registerCommand('sad.openTerminal', () => openSadRepl()),
        vscode.commands.registerCommand('sad.runInTerminal', () => runCurrentFileInTerminal()),
        vscode.commands.registerCommand('sad.runSelectionInTerminal', () => runSelectionInTerminal()),
        vscode.commands.registerCommand('sad.terminalHistory', () => showHistory(context))
    );
}

const RLE = '\u202B'; // Right-to-Left Embedding
const PDF = '\u202C'; // Pop Directional Formatting

function wrapRtl(text: string): string {
    return `${RLE}${text}${PDF}`;
}

class SadRunnerTerminal implements vscode.Pseudoterminal {
    private writeEmitter = new vscode.EventEmitter<string>();
    onDidWrite: vscode.Event<string> = this.writeEmitter.event;
    private closeEmitter = new vscode.EventEmitter<number>();
    onDidClose: vscode.Event<number> = this.closeEmitter.event;

    private process?: cp.ChildProcess;
    
    // Line editing state
    private inputBuffer = '';
    private cursorPos = 0;
    
    // Session history
    private replHistory: string[] = [];
    private historyIndex = -1;

    constructor(
        private interpreterPath: string,
        private filePath: string,
        private isRepl: boolean = false
    ) {}

    open(): void {
        if (!this.isRepl) {
            this.writeEmitter.fire(`\x1b[36m╭━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╮\x1b[0m\r\n`);
            this.writeEmitter.fire(`\x1b[36m┃         ✨ ${wrapRtl("بيئة تشغيل لـغـة ص (Sad Language)")} ✨          ┃\x1b[0m\r\n`);
            this.writeEmitter.fire(`\x1b[36m╰━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╯\x1b[0m\r\n\r\n`);
            
            this.writeEmitter.fire(`\x1b[32m 📂 ${wrapRtl("الملف")}:\x1b[0m ${path.basename(this.filePath)}\r\n`);
            this.writeEmitter.fire(`\x1b[33m ⏰ ${wrapRtl("البدء")}:\x1b[0m ${new Date().toLocaleTimeString()}\r\n`);
            this.writeEmitter.fire(`\x1b[90m────────────────────────────────────────────────────────────\x1b[0m\r\n`);
            this.writeEmitter.fire(`\x1b[37m ▶ ${wrapRtl("المخرجات")}:\x1b[0m\r\n\r\n`);
        } else {
            this.writeEmitter.fire(`\x1b[36m${wrapRtl("🔵 مرحباً بك في لغة ص — مساحة العمل التفاعلية")}\x1b[0m\r\n\r\n`);
        }

        const startTime = Date.now();
        const args = this.isRepl ? [] : [this.filePath];

        try {
            this.process = cp.spawn(this.interpreterPath, args, {
                cwd: this.isRepl ? vscode.workspace.workspaceFolders?.[0]?.uri.fsPath : path.dirname(this.filePath),
                shell: false
            });

            const handleOutput = (data: Buffer) => {
                 let str = data.toString('utf8');
                 str = str.replace(/\r?\n/g, '\r\n');
                 
                 const lines = str.split('\r\n');
                 const rtlLines = lines.map(l => {
                     if (l.trim().length > 0 && /[\u0600-\u06FF]/.test(l)) {
                         return `${RLE}${l}${PDF}`;
                     }
                     return l;
                 });
                 // Print output line by line natively
                 this.writeEmitter.fire(rtlLines.join('\r\n'));
            };

            this.process.stdout?.on('data', handleOutput);
            this.process.stderr?.on('data', handleOutput);

            this.process.on('close', (code) => {
                const duration = ((Date.now() - startTime) / 1000).toFixed(3);
                if (!this.isRepl) {
                    this.writeEmitter.fire(`\r\n\x1b[90m────────────────────────────────────────────────────────────\x1b[0m\r\n`);
                    if (code === 0) {
                        this.writeEmitter.fire(`\x1b[32m ✅ ${wrapRtl("اكتمل التنفيذ بنجاح (Code 0)")}\x1b[0m\r\n`);
                    } else {
                        this.writeEmitter.fire(`\x1b[31m ❌ ${wrapRtl(`انتهى التنفيذ بخطأ: ${code}`)}\x1b[0m\r\n`);
                    }
                    this.writeEmitter.fire(`\x1b[33m ⏱️ ${wrapRtl(`المدة: ${duration} ثانية`)}\x1b[0m\r\n\r\n`);
                    this.writeEmitter.fire(`\x1b[90m${wrapRtl("اضغط أي مفتاح للإغلاق...")}\x1b[0m`);
                } else {
                    this.writeEmitter.fire(`\r\n\x1b[90m${wrapRtl("تم إنهاء الجلسة.")}\x1b[0m\r\n`);
                    this.closeEmitter.fire(0);
                }
            });

            this.process.on('error', (err) => {
                this.writeEmitter.fire(`\r\n\x1b[31m❌ ${wrapRtl(`فشل تشغيل المفسر: ${err.message}`)}\x1b[0m\r\n`);
            });

        } catch (e: any) {
            this.writeEmitter.fire(`\r\n\x1b[31m❌ ${wrapRtl(`خطأ: ${e.message}`)}\x1b[0m\r\n`);
        }
    }

    close(): void {
        if (this.process) {
            this.process.kill();
        }
    }

    handleInput(data: string): void {
        if (!this.process || this.process.exitCode !== null) {
            if (!this.isRepl) {
                this.closeEmitter.fire(0);
            }
            return;
        }

        // Handle Enter
        if (data === '\r' || data === '\n') {
            this.writeEmitter.fire('\r\n');
            const cmd = this.inputBuffer;
            if (cmd.trim().length > 0) {
                if (this.replHistory.length === 0 || this.replHistory[this.replHistory.length - 1] !== cmd) {
                    this.replHistory.push(cmd);
                }
            }
            this.historyIndex = this.replHistory.length;
            
            // Send to actual process
            this.process.stdin?.write(cmd + '\n');
            
            this.inputBuffer = '';
            this.cursorPos = 0;
            return;
        }

        // Backspace
        if (data === '\x7f' || data === '\b') {
            if (this.cursorPos > 0) {
                this.inputBuffer = this.inputBuffer.slice(0, this.cursorPos - 1) + this.inputBuffer.slice(this.cursorPos);
                this.cursorPos--;
                this.redrawInputLine();
            }
            return;
        }
        
        // Delete (\x1b[3~)
        if (data === '\x1b[3~') {
            if (this.cursorPos < this.inputBuffer.length) {
                this.inputBuffer = this.inputBuffer.slice(0, this.cursorPos) + this.inputBuffer.slice(this.cursorPos + 1);
                this.redrawInputLine();
            }
            return;
        }

        // Arrows (ANSI Escape codes)
        if (data === '\x1b[D') { // Left
            if (this.cursorPos > 0) {
                this.cursorPos--;
                this.writeEmitter.fire('\x1b[D');
            }
            return;
        }
        if (data === '\x1b[C') { // Right
            if (this.cursorPos < this.inputBuffer.length) {
                this.cursorPos++;
                this.writeEmitter.fire('\x1b[C');
            }
            return;
        }
        if (data === '\x1b[A') { // Up
            if (this.historyIndex > 0) {
                this.historyIndex--;
                this.inputBuffer = this.replHistory[this.historyIndex];
                this.cursorPos = this.inputBuffer.length;
                this.redrawInputLine();
            }
            return;
        }
        if (data === '\x1b[B') { // Down
            if (this.historyIndex < this.replHistory.length - 1) {
                this.historyIndex++;
                this.inputBuffer = this.replHistory[this.historyIndex];
                this.cursorPos = this.inputBuffer.length;
                this.redrawInputLine();
            } else if (this.historyIndex === this.replHistory.length - 1) {
                this.historyIndex++;
                this.inputBuffer = '';
                this.cursorPos = 0;
                this.redrawInputLine();
            }
            return;
        }
        
        // Home (\x1b[H / \x1bOH) / End (\x1b[F / \x1bOF)
        if (data === '\x1b[H' || data === '\x1bOH') {
            if (this.cursorPos > 0) {
                this.writeEmitter.fire(`\x1b[${this.cursorPos}D`);
                this.cursorPos = 0;
            }
            return;
        }
        if (data === '\x1b[F' || data === '\x1bOF') {
            const spaces = this.inputBuffer.length - this.cursorPos;
            if (spaces > 0) {
                this.writeEmitter.fire(`\x1b[${spaces}C`);
                this.cursorPos = this.inputBuffer.length;
            }
            return;
        }
        
        // Ignore other escape codes
        if (data.startsWith('\x1b')) {
            return;
        }

        // Typable characters
        this.inputBuffer = this.inputBuffer.slice(0, this.cursorPos) + data + this.inputBuffer.slice(this.cursorPos);
        this.cursorPos += data.length;
        
        this.redrawInputLine();
    }
    
    private redrawInputLine() {
        // Assume prompt is "> " (2 chars).
        this.writeEmitter.fire('\r\x1b[2C'); // Carriage return and 2 Right
        this.writeEmitter.fire(this.inputBuffer);
        this.writeEmitter.fire('\x1b[K'); // clear to end of line
        
        const spacesToLeft = this.inputBuffer.length - this.cursorPos;
        if (spacesToLeft > 0) {
            this.writeEmitter.fire(`\x1b[${spacesToLeft}D`);
        }
    }
}

function runCurrentFileInTerminal(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor) {
        vscode.window.showWarningMessage('افتح ملف .ص أولاً');
        return;
    }

    editor.document.save().then(() => {
        const filePath = editor.document.uri.fsPath;
        const interpreterPath = findInterpreter();

        const pty = new SadRunnerTerminal(interpreterPath, filePath, false);
        const terminal = vscode.window.createTerminal({
            name: `ص: ${path.basename(filePath)}`,
            pty
        });
        
        terminal.show();
        addToHistory(filePath);
    });
}

function openSadRepl(): void {
    const interpreterPath = findInterpreter();
    const pty = new SadRunnerTerminal(interpreterPath, '', true);
    const terminal = vscode.window.createTerminal({
        name: '🔵 مساحة عمل ص',
        pty
    });
    terminal.show();
}

function runSelectionInTerminal(): void {
    const editor = vscode.window.activeTextEditor;
    if (!editor) return;

    const selection = editor.document.getText(editor.selection);
    if (!selection.trim()) {
        vscode.window.showWarningMessage('اختر نصاً لتشغيله');
        return;
    }

    const tmpFile = path.join(os.tmpdir(), '_sad_selection.ص');
    fs.writeFileSync(tmpFile, selection, 'utf-8');

    const interpreterPath = findInterpreter();
    const pty = new SadRunnerTerminal(interpreterPath, tmpFile, false);
    const terminal = vscode.window.createTerminal({
        name: 'ص: تشغيل المحدد',
        pty
    });
    terminal.show();
}

async function showHistory(context: vscode.ExtensionContext): Promise<void> {
    if (historyItems.length === 0) {
        vscode.window.showInformationMessage('لا يوجد تاريخ تشغيل');
        return;
    }

    const items = historyItems.map((h, i) => ({
        label: `$(history) ${path.basename(h)}`,
        description: h,
        detail: `#${historyItems.length - i}`,
        filePath: h,
    }));

    const selected = await vscode.window.showQuickPick(items, {
        placeHolder: 'اختر ملفاً لإعادة تشغيله'
    });

    if (selected) {
        const interpreterPath = findInterpreter();
        // Fallback to older terminal integration mechanism if needed? No, just use Pseudoterminal
        const pty = new SadRunnerTerminal(interpreterPath, selected.filePath, false);
        const terminal = vscode.window.createTerminal({
            name: `ص: ${path.basename(selected.filePath)}`,
            pty
        });
        terminal.show();
        addToHistory(selected.filePath);
    }
}

function addToHistory(filePath: string): void {
    historyItems = historyItems.filter(h => h !== filePath);
    historyItems.unshift(filePath);
    if (historyItems.length > 50) historyItems = historyItems.slice(0, 50);
}

function findInterpreter(): string {
    const config = vscode.workspace.getConfiguration('sad');
    const customPath = config.get<string>('interpreter.path');
    if (customPath) return customPath;

    const workspaceFolder = vscode.workspace.workspaceFolders?.[0]?.uri.fsPath;
    if (workspaceFolder) {
        const candidates = [
            path.join(workspaceFolder, 'build', 'bin', 'Debug', 'sad.exe'),
            path.join(workspaceFolder, 'build', 'bin', 'Release', 'sad.exe'),
            path.join(workspaceFolder, 'build', 'bin', 'sad.exe'),
        ];
        for (const p of candidates) {
            try {
                fs.accessSync(p);
                return p;
            } catch { /* ignore */ }
        }
    }
    return 'sad';
}
