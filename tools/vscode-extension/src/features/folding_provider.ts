/**
 * @file folding_provider.ts
 * @brief مزود الطي — Custom Folding Provider
 * 
 * طي ذكي للكود في لغة ص — يدعم الدوال والأصناف والكتل والتعليقات والمناطق
 */

import * as vscode from 'vscode';

export function registerFoldingProvider(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.languages.registerFoldingRangeProvider(
            { language: 'sad' },
            new SadFoldingRangeProvider()
        )
    );

    // أوامر الطي المخصصة
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.foldFunctions', () => foldByType('function')),
        vscode.commands.registerCommand('sad.foldClasses', () => foldByType('class')),
        vscode.commands.registerCommand('sad.foldComments', () => foldByType('comment')),
        vscode.commands.registerCommand('sad.foldImports', () => foldByType('import')),
    );
}

class SadFoldingRangeProvider implements vscode.FoldingRangeProvider {
    provideFoldingRanges(doc: vscode.TextDocument): vscode.FoldingRange[] {
        const ranges: vscode.FoldingRange[] = [];
        const lines = doc.getText().split('\n');

        // مكدس لتتبع الكتل المفتوحة
        const blockStack: { line: number; kind: vscode.FoldingRangeKind }[] = [];

        let inBlockComment = false;
        let blockCommentStart = -1;
        let consecutiveImportStart = -1;
        let lastImportLine = -1;
        let consecutiveCommentStart = -1;
        let lastCommentLine = -1;

        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            const trimmed = line.trim();

            // تعليقات الكتلة #* ... *#
            if (!inBlockComment && trimmed.startsWith('#*')) {
                inBlockComment = true;
                blockCommentStart = i;
            }
            if (inBlockComment && trimmed.endsWith('*#')) {
                inBlockComment = false;
                if (blockCommentStart >= 0 && i > blockCommentStart) {
                    ranges.push(new vscode.FoldingRange(blockCommentStart, i, vscode.FoldingRangeKind.Comment));
                }
                blockCommentStart = -1;
                continue;
            }
            if (inBlockComment) { continue; }

            // تعليقات توثيقية #** ... **#
            if (trimmed.startsWith('#**')) {
                const endLine = findMatchingEnd(lines, i, '#**', '**#');
                if (endLine > i) {
                    ranges.push(new vscode.FoldingRange(i, endLine, vscode.FoldingRangeKind.Comment));
                }
            }

            // تعليقات متتالية
            if (trimmed.startsWith('#') && !trimmed.startsWith('#*')) {
                if (lastCommentLine === i - 1) {
                    // استمرار التعليقات المتتالية
                } else {
                    // بداية مجموعة جديدة
                    if (consecutiveCommentStart >= 0 && lastCommentLine - consecutiveCommentStart >= 2) {
                        ranges.push(new vscode.FoldingRange(consecutiveCommentStart, lastCommentLine, vscode.FoldingRangeKind.Comment));
                    }
                    consecutiveCommentStart = i;
                }
                lastCommentLine = i;
            } else {
                // نهاية مجموعة تعليقات
                if (consecutiveCommentStart >= 0 && lastCommentLine - consecutiveCommentStart >= 2) {
                    ranges.push(new vscode.FoldingRange(consecutiveCommentStart, lastCommentLine, vscode.FoldingRangeKind.Comment));
                }
                consecutiveCommentStart = -1;
            }

            // استيرادات متتالية
            if (trimmed.startsWith('استورد')) {
                if (lastImportLine === i - 1 || consecutiveImportStart < 0) {
                    if (consecutiveImportStart < 0) {
                        consecutiveImportStart = i;
                    }
                }
                lastImportLine = i;
            } else {
                if (consecutiveImportStart >= 0 && lastImportLine - consecutiveImportStart >= 1) {
                    ranges.push(new vscode.FoldingRange(consecutiveImportStart, lastImportLine, vscode.FoldingRangeKind.Imports));
                }
                if (!trimmed.startsWith('استورد')) {
                    consecutiveImportStart = -1;
                }
            }

            // مناطق مخصصة: # region و # endregion (أو # منطقة / # نهاية_منطقة)
            if (/^#\s*(?:region|منطقة)\b/i.test(trimmed)) {
                blockStack.push({ line: i, kind: vscode.FoldingRangeKind.Region });
                continue;
            }
            if (/^#\s*(?:endregion|نهاية_منطقة)\b/i.test(trimmed)) {
                for (let j = blockStack.length - 1; j >= 0; j--) {
                    if (blockStack[j].kind === vscode.FoldingRangeKind.Region) {
                        ranges.push(new vscode.FoldingRange(blockStack[j].line, i, vscode.FoldingRangeKind.Region));
                        blockStack.splice(j, 1);
                        break;
                    }
                }
                continue;
            }

            // كتل الكود: دالة، صنف، إذا، بينما، لكل، طابق، حاول، باني، تعداد، بنية
            const blockOpeners = /^(?:(?:عام|خاص|محمي|مجرد)\s+)?(?:دالة|صنف|بنية|تعداد|إذا|اذا|بينما|لكل|طابق|حاول|باني|هدم|خاصية|اختبر|سمة|واجهة)\b/;
            if (blockOpeners.test(trimmed)) {
                blockStack.push({ line: i, kind: vscode.FoldingRangeKind.Region });
            }

            // وإلا — إغلاق كتلة إذا وفتح كتلة جديدة
            if (/^(?:وإلا|والا)/.test(trimmed)) {
                for (let j = blockStack.length - 1; j >= 0; j--) {
                    if (blockStack[j].kind === vscode.FoldingRangeKind.Region) {
                        ranges.push(new vscode.FoldingRange(blockStack[j].line, i - 1));
                        blockStack[j].line = i;
                        break;
                    }
                }
            }

            // نهاية — إغلاق آخر كتلة مفتوحة
            if (trimmed === 'نهاية') {
                for (let j = blockStack.length - 1; j >= 0; j--) {
                    if (blockStack[j].kind === vscode.FoldingRangeKind.Region) {
                        ranges.push(new vscode.FoldingRange(blockStack[j].line, i));
                        blockStack.splice(j, 1);
                        break;
                    }
                }
            }
        }

        // إغلاق أي تعليقات أو استيرادات متبقية
        if (consecutiveCommentStart >= 0 && lastCommentLine - consecutiveCommentStart >= 2) {
            ranges.push(new vscode.FoldingRange(consecutiveCommentStart, lastCommentLine, vscode.FoldingRangeKind.Comment));
        }
        if (consecutiveImportStart >= 0 && lastImportLine - consecutiveImportStart >= 1) {
            ranges.push(new vscode.FoldingRange(consecutiveImportStart, lastImportLine, vscode.FoldingRangeKind.Imports));
        }

        return ranges;
    }
}

function findMatchingEnd(lines: string[], start: number, openTag: string, closeTag: string): number {
    for (let i = start; i < lines.length; i++) {
        if (lines[i].trim().endsWith(closeTag) && i > start) {
            return i;
        }
    }
    return -1;
}

async function foldByType(type: 'function' | 'class' | 'comment' | 'import'): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'sad') { return; }

    const lines = editor.document.getText().split('\n');
    const ranges: vscode.Range[] = [];

    for (let i = 0; i < lines.length; i++) {
        const trimmed = lines[i].trim();
        let match = false;

        switch (type) {
            case 'function':
                match = /^(?:(?:عام|خاص|محمي)\s+)?دالة\s+/.test(trimmed);
                break;
            case 'class':
                match = /^صنف\s+|^بنية\s+/.test(trimmed);
                break;
            case 'comment':
                match = trimmed.startsWith('#');
                break;
            case 'import':
                match = trimmed.startsWith('استورد');
                break;
        }

        if (match) {
            ranges.push(new vscode.Range(i, 0, i, 0));
        }
    }

    if (ranges.length > 0) {
        // طي كل النطاقات المطابقة
        for (const range of ranges) {
            editor.selection = new vscode.Selection(range.start, range.start);
            await vscode.commands.executeCommand('editor.fold', { selectionLines: [range.start.line] });
        }

        const typeNames: Record<string, string> = {
            'function': 'الدوال',
            'class': 'الأصناف',
            'comment': 'التعليقات',
            'import': 'الاستيرادات',
        };
        vscode.window.showInformationMessage(`📂 تم طي ${ranges.length} ${typeNames[type]}`);
    }
}
