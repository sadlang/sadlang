/**
 * @file breadcrumb_provider.ts
 * @brief مزود مسار التنقل — Arabic Breadcrumb & Outline Provider
 * 
 * عرض مسار تنقل عربي + هيكل المستند (Outline) مع أيقونات مخصصة
 */

import * as vscode from 'vscode';

export function registerBreadcrumbProvider(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.languages.registerDocumentSymbolProvider(
            { language: 'sad' },
            new SadDocumentSymbolProvider()
        )
    );

    // أمر: الانتقال إلى رمز
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.goToSymbol', () => goToSymbol())
    );
}

class SadDocumentSymbolProvider implements vscode.DocumentSymbolProvider {
    provideDocumentSymbols(doc: vscode.TextDocument): vscode.DocumentSymbol[] {
        const symbols: vscode.DocumentSymbol[] = [];
        const text = doc.getText();
        const lines = text.split('\n');

        const stack: { symbol: vscode.DocumentSymbol; indent: number; endKeyword: boolean }[] = [];

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const trimmed = line.trim();
            const indent = line.length - line.trimStart().length;

            // الأصناف
            const classMatch = trimmed.match(/^صنف\s+([\p{L}\p{N}_]+)(?:\s+يرث\s+([\p{L}\p{N}_]+))?/u);
            if (classMatch) {
                const endLine = findBlockEnd(lines, i);
                const range = new vscode.Range(i, 0, endLine, lines[endLine].length);
                const selRange = new vscode.Range(i, line.indexOf(classMatch[1]), i, line.indexOf(classMatch[1]) + classMatch[1].length);
                const detail = classMatch[2] ? `يرث ${classMatch[2]}` : '';
                const symbol = new vscode.DocumentSymbol(
                    classMatch[1], detail, vscode.SymbolKind.Class, range, selRange
                );
                addToStack(symbols, stack, symbol, indent);
                continue;
            }

            // البنى
            const structMatch = trimmed.match(/^بنية\s+([\p{L}\p{N}_]+)/u);
            if (structMatch) {
                const endLine = findBlockEnd(lines, i);
                const range = new vscode.Range(i, 0, endLine, lines[endLine].length);
                const selRange = new vscode.Range(i, line.indexOf(structMatch[1]), i, line.indexOf(structMatch[1]) + structMatch[1].length);
                const symbol = new vscode.DocumentSymbol(
                    structMatch[1], 'بنية', vscode.SymbolKind.Struct, range, selRange
                );
                addToStack(symbols, stack, symbol, indent);
                continue;
            }

            // التعدادات
            const enumMatch = trimmed.match(/^تعداد\s+([\p{L}\p{N}_]+)/u);
            if (enumMatch) {
                const endLine = findBlockEnd(lines, i);
                const range = new vscode.Range(i, 0, endLine, lines[endLine].length);
                const selRange = new vscode.Range(i, line.indexOf(enumMatch[1]), i, line.indexOf(enumMatch[1]) + enumMatch[1].length);
                const symbol = new vscode.DocumentSymbol(
                    enumMatch[1], 'تعداد', vscode.SymbolKind.Enum, range, selRange
                );
                addToStack(symbols, stack, symbol, indent);
                continue;
            }

            // الدوال
            const funcMatch = trimmed.match(/^(?:(عام|خاص|محمي|مجرد|ساكن)\s+)*دالة\s+([\p{L}\p{N}_]+)\s*\(([^)]*)\)/u);
            if (funcMatch) {
                const endLine = findBlockEnd(lines, i);
                const name = funcMatch[2];
                const params = funcMatch[3] || '';
                const visibility = funcMatch[1] || '';
                const range = new vscode.Range(i, 0, endLine, lines[endLine].length);
                const nameStart = line.indexOf(name);
                const selRange = new vscode.Range(i, nameStart, i, nameStart + name.length);
                const detail = `(${params})${visibility ? ' — ' + visibility : ''}`;
                const kind = stack.length > 0 ? vscode.SymbolKind.Method : vscode.SymbolKind.Function;
                const symbol = new vscode.DocumentSymbol(name, detail, kind, range, selRange);
                addToStack(symbols, stack, symbol, indent);
                continue;
            }

            // الباني
            if (/^باني\s*\(/.test(trimmed)) {
                const endLine = findBlockEnd(lines, i);
                const range = new vscode.Range(i, 0, endLine, lines[endLine].length);
                const selRange = new vscode.Range(i, line.indexOf('باني'), i, line.indexOf('باني') + 4);
                const symbol = new vscode.DocumentSymbol('باني', 'المنشئ', vscode.SymbolKind.Constructor, range, selRange);
                addToStack(symbols, stack, symbol, indent);
                continue;
            }

            // الخصائص
            const propMatch = trimmed.match(/^خاصية\s+([\p{L}\p{N}_]+)/u);
            if (propMatch) {
                const endLine = findBlockEnd(lines, i);
                const range = new vscode.Range(i, 0, endLine, lines[endLine].length);
                const nameStart = line.indexOf(propMatch[1]);
                const selRange = new vscode.Range(i, nameStart, i, nameStart + propMatch[1].length);
                const symbol = new vscode.DocumentSymbol(propMatch[1], 'خاصية', vscode.SymbolKind.Property, range, selRange);
                addToStack(symbols, stack, symbol, indent);
                continue;
            }

            // المتغيرات والثوابت (المستوى الأعلى فقط)
            const varMatch = trimmed.match(/^(متغير|ثابت)\s+([\p{L}\p{N}_]+)\s*=\s*(.+)?/u);
            if (varMatch && indent === 0) {
                const kind = varMatch[1] === 'ثابت' ? vscode.SymbolKind.Constant : vscode.SymbolKind.Variable;
                const range = new vscode.Range(i, 0, i, line.length);
                const nameStart = line.indexOf(varMatch[2]);
                const selRange = new vscode.Range(i, nameStart, i, nameStart + varMatch[2].length);
                const value = varMatch[3] ? varMatch[3].trim().substring(0, 30) : '';
                const symbol = new vscode.DocumentSymbol(varMatch[2], `${varMatch[1]} = ${value}`, kind, range, selRange);
                symbols.push(symbol);
                continue;
            }

            // السمات / الواجهات
            const traitMatch = trimmed.match(/^(?:سمة|واجهة)\s+([\p{L}\p{N}_]+)/u);
            if (traitMatch) {
                const endLine = findBlockEnd(lines, i);
                const range = new vscode.Range(i, 0, endLine, lines[endLine].length);
                const nameStart = line.indexOf(traitMatch[1]);
                const selRange = new vscode.Range(i, nameStart, i, nameStart + traitMatch[1].length);
                const symbol = new vscode.DocumentSymbol(traitMatch[1], 'سمة', vscode.SymbolKind.Interface, range, selRange);
                addToStack(symbols, stack, symbol, indent);
                continue;
            }

            // الاختبارات
            const testMatch = trimmed.match(/^اختبر\s+"([^"]+)"/u);
            if (testMatch) {
                const endLine = findBlockEnd(lines, i);
                const range = new vscode.Range(i, 0, endLine, lines[endLine].length);
                const selRange = new vscode.Range(i, 0, i, trimmed.length);
                const symbol = new vscode.DocumentSymbol(`🧪 ${testMatch[1]}`, 'اختبار', vscode.SymbolKind.Function, range, selRange);
                symbols.push(symbol);
                continue;
            }
        }

        return symbols;
    }
}

function addToStack(
    symbols: vscode.DocumentSymbol[],
    stack: { symbol: vscode.DocumentSymbol; indent: number }[],
    symbol: vscode.DocumentSymbol,
    indent: number
): void {
    // إزالة العناصر بمستوى مسافة أكبر أو يساوي
    while (stack.length > 0 && stack[stack.length - 1].indent >= indent) {
        stack.pop();
    }

    if (stack.length > 0) {
        stack[stack.length - 1].symbol.children.push(symbol);
    } else {
        symbols.push(symbol);
    }

    stack.push({ symbol, indent });
}

function findBlockEnd(lines: string[], startLine: number): number {
    let depth = 1;
    const openers = /^(?:(?:عام|خاص|محمي|مجرد|ساكن)\s+)*(?:دالة|صنف|بنية|تعداد|إذا|اذا|بينما|لكل|طابق|حاول|باني|هدم|خاصية|اختبر|سمة|واجهة)\b/;

    for (let i = startLine + 1; i < lines.length; i++) {
        const trimmed = lines[i].trim();

        if (openers.test(trimmed)) {
            depth++;
        }

        if (trimmed === 'نهاية') {
            depth--;
            if (depth === 0) {
                return i;
            }
        }
    }

    return lines.length - 1;
}

async function goToSymbol(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const provider = new SadDocumentSymbolProvider();
    const symbols = provider.provideDocumentSymbols(editor.document);

    const flatSymbols = flattenSymbols(symbols);

    const items = flatSymbols.map(s => {
        const icon = getSymbolIcon(s.kind);
        return {
            label: `${icon} ${s.name}`,
            description: s.detail,
            detail: `سطر ${s.range.start.line + 1}`,
            symbol: s,
        };
    });

    const selected = await vscode.window.showQuickPick(items, {
        placeHolder: 'انتقل إلى رمز — Go to Symbol',
        matchOnDescription: true,
    });

    if (selected) {
        const pos = selected.symbol.selectionRange.start;
        editor.selection = new vscode.Selection(pos, pos);
        editor.revealRange(selected.symbol.selectionRange, vscode.TextEditorRevealType.InCenter);
    }
}

function flattenSymbols(symbols: vscode.DocumentSymbol[], prefix: string = ''): vscode.DocumentSymbol[] {
    const result: vscode.DocumentSymbol[] = [];
    for (const s of symbols) {
        result.push(s);
        if (s.children.length > 0) {
            result.push(...flattenSymbols(s.children, s.name + '.'));
        }
    }
    return result;
}

function getSymbolIcon(kind: vscode.SymbolKind): string {
    switch (kind) {
        case vscode.SymbolKind.Class: return '🏗️';
        case vscode.SymbolKind.Function: return '⚡';
        case vscode.SymbolKind.Method: return '🔧';
        case vscode.SymbolKind.Constructor: return '🔨';
        case vscode.SymbolKind.Property: return '📋';
        case vscode.SymbolKind.Variable: return '📦';
        case vscode.SymbolKind.Constant: return '🔒';
        case vscode.SymbolKind.Enum: return '📊';
        case vscode.SymbolKind.Interface: return '🔌';
        case vscode.SymbolKind.Struct: return '🧱';
        default: return '•';
    }
}
