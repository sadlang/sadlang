/**
 * @file debugAdapter.ts
 * @brief Debug Adapter Protocol implementation for Sad Language
 * @brief (AR) تطبيق بروتوكول محول التصحيح للغة ص
 * 
 * @author عمر
 * @date نوفمبر 2026
 */

import {
    DebugSession,
    InitializedEvent,
    StoppedEvent,
    BreakpointEvent,
    OutputEvent,
    TerminatedEvent,
    Thread,
    StackFrame,
    Scope,
    Source,
    Variable,
    Breakpoint,
    Handles,
} from 'vscode-debugadapter';
import { DebugProtocol } from 'vscode-debugprotocol';
import * as net from 'net';
import * as path from 'path';
import { ChildProcess, spawn } from 'child_process';

//==============================================================================
// أنواع البيانات
//==============================================================================

interface LaunchRequestArguments extends DebugProtocol.LaunchRequestArguments {
    program: string;
    stopOnEntry?: boolean;
    trace?: boolean;
    args?: string[];
    cwd?: string;
    env?: { [key: string]: string };
    interpreterPath?: string;
}

interface AttachRequestArguments extends DebugProtocol.AttachRequestArguments {
    port: number;
    host?: string;
}

interface SadBreakpoint {
    id: number;
    line: number;
    verified: boolean;
    source: string;
}

interface SadStackFrame {
    id: number;
    name: string;
    file: string;
    line: number;
    column: number;
}

interface SadVariable {
    name: string;
    value: string;
    type: string;
    variablesReference: number;
}

//==============================================================================
// محول التصحيح
//==============================================================================

export class SadDebugSession extends DebugSession {
    private static THREAD_ID = 1;
    
    private interpreter: ChildProcess | null = null;
    private debugClient: net.Socket | null = null;
    
    private breakpoints: Map<string, SadBreakpoint[]> = new Map();
    private nextBreakpointId = 1;
    
    private stackFrames: SadStackFrame[] = [];
    private variablesHandles = new Handles<string>();
    
    private trace = false;
    private configurationDone = false;

    constructor() {
        super();
        this.setDebuggerLinesStartAt1(true);
        this.setDebuggerColumnsStartAt1(true);
    }

    //==========================================================================
    // Initialization
    //==========================================================================

    protected initializeRequest(
        response: DebugProtocol.InitializeResponse,
        args: DebugProtocol.InitializeRequestArguments
    ): void {
        response.body = response.body || {};
        
        // Capabilities
        response.body.supportsConfigurationDoneRequest = true;
        response.body.supportsEvaluateForHovers = true;
        response.body.supportsStepBack = false;
        response.body.supportsSetVariable = true;
        response.body.supportsRestartFrame = false;
        response.body.supportsGotoTargetsRequest = false;
        response.body.supportsStepInTargetsRequest = false;
        response.body.supportsCompletionsRequest = true;
        response.body.supportsModulesRequest = false;
        response.body.supportsExceptionOptions = true;
        response.body.supportsValueFormattingOptions = true;
        response.body.supportsExceptionInfoRequest = true;
        response.body.supportTerminateDebuggee = true;
        response.body.supportsDelayedStackTraceLoading = true;
        response.body.supportsLoadedSourcesRequest = true;
        response.body.supportsLogPoints = true;
        response.body.supportsTerminateThreadsRequest = false;
        response.body.supportsSetExpression = false;
        response.body.supportsTerminateRequest = true;
        response.body.supportsDataBreakpoints = false;
        response.body.supportsReadMemoryRequest = false;
        response.body.supportsDisassembleRequest = false;
        response.body.supportsCancelRequest = false;
        response.body.supportsBreakpointLocationsRequest = true;
        response.body.supportsClipboardContext = true;
        response.body.supportsSteppingGranularity = true;
        response.body.supportsInstructionBreakpoints = false;
        response.body.supportsExceptionFilterOptions = true;

        // Exception filter
        response.body.exceptionBreakpointFilters = [
            {
                filter: 'all',
                label: 'All Exceptions / كل الاستثناءات',
                default: false
            },
            {
                filter: 'uncaught',
                label: 'Uncaught Exceptions / الاستثناءات غير الملتقطة',
                default: true
            }
        ];

        this.sendResponse(response);
        this.sendEvent(new InitializedEvent());
    }

    protected configurationDoneRequest(
        response: DebugProtocol.ConfigurationDoneResponse,
        args: DebugProtocol.ConfigurationDoneArguments
    ): void {
        this.configurationDone = true;
        this.sendResponse(response);
    }

    //==========================================================================
    // Launch / Attach
    //==========================================================================

    protected async launchRequest(
        response: DebugProtocol.LaunchResponse,
        args: LaunchRequestArguments
    ): Promise<void> {
        this.trace = args.trace || false;
        
        const interpreterPath = args.interpreterPath || 'sad';
        const program = args.program;
        const cwd = args.cwd || path.dirname(program);
        
        if (this.trace) {
            this.log(`Launching: ${interpreterPath} --debug ${program}`);
        }
        
        try {
            // Start interpreter in debug mode
            const interpreterArgs = ['--debug', '--debug-port', '9229', program];
            if (args.args) {
                interpreterArgs.push('--', ...args.args);
            }
            
            this.interpreter = spawn(interpreterPath, interpreterArgs, {
                cwd,
                env: { ...process.env, ...args.env }
            });
            
            this.interpreter.stdout?.on('data', (data) => {
                this.sendEvent(new OutputEvent(data.toString(), 'stdout'));
            });
            
            this.interpreter.stderr?.on('data', (data) => {
                this.sendEvent(new OutputEvent(data.toString(), 'stderr'));
            });
            
            this.interpreter.on('exit', (code) => {
                this.sendEvent(new TerminatedEvent());
            });
            
            // Connect to debug server
            await this.connectToDebugServer('localhost', 9229);
            
            // Set breakpoints
            await this.sendBreakpointsToInterpreter();
            
            if (args.stopOnEntry) {
                this.sendEvent(new StoppedEvent('entry', SadDebugSession.THREAD_ID));
            } else {
                await this.sendDebugCommand('continue');
            }
            
            this.sendResponse(response);
        } catch (error) {
            this.sendErrorResponse(response, 1, `Launch failed: ${error}`);
        }
    }

    protected async attachRequest(
        response: DebugProtocol.AttachResponse,
        args: AttachRequestArguments
    ): Promise<void> {
        try {
            await this.connectToDebugServer(args.host || 'localhost', args.port);
            this.sendResponse(response);
        } catch (error) {
            this.sendErrorResponse(response, 2, `Attach failed: ${error}`);
        }
    }

    //==========================================================================
    // Breakpoints
    //==========================================================================

    protected setBreakPointsRequest(
        response: DebugProtocol.SetBreakpointsResponse,
        args: DebugProtocol.SetBreakpointsArguments
    ): void {
        const sourcePath = args.source.path || '';
        const clientLines = args.lines || [];
        
        // Clear old breakpoints for this file
        this.breakpoints.delete(sourcePath);
        
        // Set new breakpoints
        const newBreakpoints: SadBreakpoint[] = [];
        const responseBreakpoints: Breakpoint[] = [];
        
        for (const line of clientLines) {
            const bp: SadBreakpoint = {
                id: this.nextBreakpointId++,
                line,
                verified: true,
                source: sourcePath
            };
            newBreakpoints.push(bp);
            
            responseBreakpoints.push({
                id: bp.id,
                verified: bp.verified,
                line: bp.line,
                source: args.source
            });
        }
        
        this.breakpoints.set(sourcePath, newBreakpoints);
        
        // Send to interpreter if connected
        if (this.debugClient) {
            this.sendBreakpointsToInterpreter();
        }
        
        response.body = { breakpoints: responseBreakpoints };
        this.sendResponse(response);
    }

    //==========================================================================
    // Execution Control
    //==========================================================================

    protected continueRequest(
        response: DebugProtocol.ContinueResponse,
        args: DebugProtocol.ContinueArguments
    ): void {
        this.sendDebugCommand('continue');
        this.sendResponse(response);
    }

    protected nextRequest(
        response: DebugProtocol.NextResponse,
        args: DebugProtocol.NextArguments
    ): void {
        this.sendDebugCommand('next');
        this.sendResponse(response);
    }

    protected stepInRequest(
        response: DebugProtocol.StepInResponse,
        args: DebugProtocol.StepInArguments
    ): void {
        this.sendDebugCommand('stepIn');
        this.sendResponse(response);
    }

    protected stepOutRequest(
        response: DebugProtocol.StepOutResponse,
        args: DebugProtocol.StepOutArguments
    ): void {
        this.sendDebugCommand('stepOut');
        this.sendResponse(response);
    }

    protected pauseRequest(
        response: DebugProtocol.PauseResponse,
        args: DebugProtocol.PauseArguments
    ): void {
        this.sendDebugCommand('pause');
        this.sendResponse(response);
    }

    //==========================================================================
    // Stack / Variables
    //==========================================================================

    protected threadsRequest(response: DebugProtocol.ThreadsResponse): void {
        response.body = {
            threads: [new Thread(SadDebugSession.THREAD_ID, 'الخيط الرئيسي')]
        };
        this.sendResponse(response);
    }

    protected async stackTraceRequest(
        response: DebugProtocol.StackTraceResponse,
        args: DebugProtocol.StackTraceArguments
    ): Promise<void> {
        const frames = await this.getStackFrames();
        
        response.body = {
            stackFrames: frames.map((f, i) => new StackFrame(
                f.id,
                f.name,
                new Source(path.basename(f.file), f.file),
                f.line,
                f.column
            )),
            totalFrames: frames.length
        };
        this.sendResponse(response);
    }

    protected scopesRequest(
        response: DebugProtocol.ScopesResponse,
        args: DebugProtocol.ScopesArguments
    ): void {
        const scopes: Scope[] = [
            new Scope('المتغيرات المحلية', this.variablesHandles.create('local:' + args.frameId), false),
            new Scope('المتغيرات العامة', this.variablesHandles.create('global'), true)
        ];
        
        response.body = { scopes };
        this.sendResponse(response);
    }

    protected async variablesRequest(
        response: DebugProtocol.VariablesResponse,
        args: DebugProtocol.VariablesArguments
    ): Promise<void> {
        const ref = this.variablesHandles.get(args.variablesReference);
        const variables = await this.getVariables(ref);
        
        response.body = {
            variables: variables.map(v => ({
                name: v.name,
                value: v.value,
                type: v.type,
                variablesReference: v.variablesReference
            }))
        };
        this.sendResponse(response);
    }

    //==========================================================================
    // Evaluate
    //==========================================================================

    protected async evaluateRequest(
        response: DebugProtocol.EvaluateResponse,
        args: DebugProtocol.EvaluateArguments
    ): Promise<void> {
        const result = await this.evaluateExpression(args.expression, args.frameId);
        
        response.body = {
            result: result.value,
            type: result.type,
            variablesReference: result.variablesReference
        };
        this.sendResponse(response);
    }

    //==========================================================================
    // Termination
    //==========================================================================

    protected terminateRequest(
        response: DebugProtocol.TerminateResponse,
        args: DebugProtocol.TerminateArguments
    ): void {
        if (this.interpreter) {
            this.interpreter.kill();
        }
        if (this.debugClient) {
            this.debugClient.destroy();
        }
        this.sendResponse(response);
    }

    protected disconnectRequest(
        response: DebugProtocol.DisconnectResponse,
        args: DebugProtocol.DisconnectArguments
    ): void {
        if (args.terminateDebuggee && this.interpreter) {
            this.interpreter.kill();
        }
        if (this.debugClient) {
            this.debugClient.destroy();
        }
        this.sendResponse(response);
    }

    //==========================================================================
    // Helpers
    //==========================================================================

    private async connectToDebugServer(host: string, port: number): Promise<void> {
        return new Promise((resolve, reject) => {
            this.debugClient = new net.Socket();
            
            this.debugClient.connect(port, host, () => {
                this.log(`Connected to debug server at ${host}:${port}`);
                resolve();
            });
            
            this.debugClient.on('data', (data) => {
                this.handleDebugMessage(data.toString());
            });
            
            this.debugClient.on('error', (err) => {
                reject(err);
            });
            
            this.debugClient.on('close', () => {
                this.sendEvent(new TerminatedEvent());
            });
        });
    }

    private handleDebugMessage(message: string): void {
        try {
            const msg = JSON.parse(message);
            
            switch (msg.type) {
                case 'stopped':
                    this.sendEvent(new StoppedEvent(
                        msg.reason || 'breakpoint',
                        SadDebugSession.THREAD_ID
                    ));
                    break;
                    
                case 'output':
                    this.sendEvent(new OutputEvent(msg.text, msg.category || 'stdout'));
                    break;
                    
                case 'breakpointValidated':
                    this.sendEvent(new BreakpointEvent('changed', {
                        id: msg.breakpointId,
                        verified: msg.verified,
                        line: msg.line
                    }));
                    break;
                    
                case 'terminated':
                    this.sendEvent(new TerminatedEvent());
                    break;
            }
        } catch (e) {
            this.log(`Failed to parse message: ${message}`);
        }
    }

    private sendDebugCommand(command: string, args?: any): void {
        if (this.debugClient) {
            const message = JSON.stringify({ command, ...args });
            this.debugClient.write(message + '\n');
        }
    }

    private async sendBreakpointsToInterpreter(): Promise<void> {
        const allBreakpoints: any[] = [];
        
        for (const [file, bps] of this.breakpoints) {
            for (const bp of bps) {
                allBreakpoints.push({
                    file,
                    line: bp.line,
                    id: bp.id
                });
            }
        }
        
        this.sendDebugCommand('setBreakpoints', { breakpoints: allBreakpoints });
    }

    private async getStackFrames(): Promise<SadStackFrame[]> {
        // Would normally query the interpreter
        return this.stackFrames;
    }

    private async getVariables(scope: string): Promise<SadVariable[]> {
        // Would normally query the interpreter
        return [];
    }

    private async evaluateExpression(
        expression: string,
        frameId?: number
    ): Promise<{ value: string; type: string; variablesReference: number }> {
        // Would normally send to interpreter
        return { value: '', type: '', variablesReference: 0 };
    }

    private log(message: string): void {
        if (this.trace) {
            this.sendEvent(new OutputEvent(`[DEBUG] ${message}\n`, 'console'));
        }
    }
}

//==============================================================================
// Entry Point
//==============================================================================

DebugSession.run(SadDebugSession);
