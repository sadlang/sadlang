/**
 * @file color_picker.ts
 * @brief لوحة الألوان — Color Picker for Graphics Code
 * 
 * يوفر لوحة ألوان تفاعلية لبرامج الرسوميات في لغة ص
 * مع اكتشاف تلقائي لقيم الألوان في الكود
 */

import * as vscode from 'vscode';

let colorDecorationType: vscode.TextEditorDecorationType;

export function registerColorPicker(context: vscode.ExtensionContext): void {
    colorDecorationType = vscode.window.createTextEditorDecorationType({});
    context.subscriptions.push(colorDecorationType);

    // ColorProvider للألوان في الكود
    context.subscriptions.push(
        vscode.languages.registerColorProvider({ language: 'sad' }, new SadColorProvider())
    );

    // أمر: إدراج لون
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.insertColor', async () => {
            await insertColor();
        })
    );

    // تزيين الألوان في الكود
    context.subscriptions.push(
        vscode.window.onDidChangeActiveTextEditor((editor) => {
            if (editor && editor.document.languageId === 'sad') {
                decorateColors(editor);
            }
        })
    );

    context.subscriptions.push(
        vscode.workspace.onDidChangeTextDocument((event) => {
            const editor = vscode.window.activeTextEditor;
            if (editor && event.document === editor.document && editor.document.languageId === 'sad') {
                decorateColors(editor);
            }
        })
    );

    const editor = vscode.window.activeTextEditor;
    if (editor && editor.document.languageId === 'sad') {
        decorateColors(editor);
    }
}

class SadColorProvider implements vscode.DocumentColorProvider {
    provideDocumentColors(doc: vscode.TextDocument): vscode.ColorInformation[] {
        const colors: vscode.ColorInformation[] = [];
        const text = doc.getText();

        // اكتشاف أنماط الألوان: لون(ر، خ، ز) أو لون(ر، خ، ز، ش) أو #RRGGBB
        // نمط rgb
        const rgbRegex = /لون\s*\(\s*(\d+)\s*[,،]\s*(\d+)\s*[,،]\s*(\d+)\s*(?:[,،]\s*(\d+\.?\d*))?\s*\)/g;
        let match;
        while ((match = rgbRegex.exec(text)) !== null) {
            const r = parseInt(match[1]) / 255;
            const g = parseInt(match[2]) / 255;
            const b = parseInt(match[3]) / 255;
            const a = match[4] ? parseFloat(match[4]) : 1;

            const startPos = doc.positionAt(match.index);
            const endPos = doc.positionAt(match.index + match[0].length);
            colors.push(new vscode.ColorInformation(
                new vscode.Range(startPos, endPos),
                new vscode.Color(r, g, b, a)
            ));
        }

        // نمط hex #RRGGBB
        const hexRegex = /#([0-9a-fA-F]{6})\b/g;
        while ((match = hexRegex.exec(text)) !== null) {
            const hex = match[1];
            const r = parseInt(hex.substring(0, 2), 16) / 255;
            const g = parseInt(hex.substring(2, 4), 16) / 255;
            const b = parseInt(hex.substring(4, 6), 16) / 255;

            const startPos = doc.positionAt(match.index);
            const endPos = doc.positionAt(match.index + match[0].length);
            colors.push(new vscode.ColorInformation(
                new vscode.Range(startPos, endPos),
                new vscode.Color(r, g, b, 1)
            ));
        }

        return colors;
    }

    provideColorPresentations(color: vscode.Color, context: { document: vscode.TextDocument; range: vscode.Range }): vscode.ColorPresentation[] {
        const r = Math.round(color.red * 255);
        const g = Math.round(color.green * 255);
        const b = Math.round(color.blue * 255);
        const a = color.alpha;

        const presentations: vscode.ColorPresentation[] = [];

        // صيغة لون()
        if (a < 1) {
            presentations.push(new vscode.ColorPresentation(`لون(${r}، ${g}، ${b}، ${a.toFixed(2)})`));
        } else {
            presentations.push(new vscode.ColorPresentation(`لون(${r}، ${g}، ${b})`));
        }

        // صيغة hex
        const hex = '#' + [r, g, b].map(c => c.toString(16).padStart(2, '0')).join('');
        presentations.push(new vscode.ColorPresentation(hex));

        return presentations;
    }
}

function decorateColors(editor: vscode.TextEditor): void {
    const decorations: vscode.DecorationOptions[] = [];
    const text = editor.document.getText();

    // ألوان مسماة
    const namedColors: Record<string, string> = {
        'أحمر': '#ff0000', 'أخضر': '#00ff00', 'أزرق': '#0000ff',
        'أبيض': '#ffffff', 'أسود': '#000000', 'أصفر': '#ffff00',
        'برتقالي': '#ffa500', 'بنفسجي': '#800080', 'وردي': '#ffc0cb',
        'رمادي': '#808080', 'بني': '#8b4513', 'ذهبي': '#ffd700',
    };

    for (const [name, hex] of Object.entries(namedColors)) {
        const regex = new RegExp(`"${name}"`, 'g');
        let match;
        while ((match = regex.exec(text)) !== null) {
            const startPos = editor.document.positionAt(match.index);
            const endPos = editor.document.positionAt(match.index + match[0].length);
            decorations.push({
                range: new vscode.Range(startPos, endPos),
                renderOptions: {
                    before: {
                        contentText: '■ ',
                        color: hex,
                    }
                }
            });
        }
    }

    editor.setDecorations(colorDecorationType, decorations);
}

async function insertColor(): Promise<void> {
    const editor = vscode.window.activeTextEditor;
    if (!editor) { return; }

    const colors = [
        { label: '🔴 أحمر', value: 'لون(255، 0، 0)' },
        { label: '🟢 أخضر', value: 'لون(0، 255، 0)' },
        { label: '🔵 أزرق', value: 'لون(0، 0، 255)' },
        { label: '⚪ أبيض', value: 'لون(255، 255، 255)' },
        { label: '⚫ أسود', value: 'لون(0، 0، 0)' },
        { label: '🟡 أصفر', value: 'لون(255، 255، 0)' },
        { label: '🟠 برتقالي', value: 'لون(255، 165، 0)' },
        { label: '🟣 بنفسجي', value: 'لون(128، 0، 128)' },
        { label: '🎨 مخصص...', value: 'custom' },
    ];

    const choice = await vscode.window.showQuickPick(colors, {
        placeHolder: 'اختر لوناً — Choose a color'
    });

    if (!choice) { return; }

    if (choice.value === 'custom') {
        const input = await vscode.window.showInputBox({
            prompt: 'أدخل اللون بصيغة R، G، B',
            placeHolder: '128، 64، 255'
        });
        if (input) {
            editor.insertSnippet(new vscode.SnippetString(`لون(${input})`));
        }
    } else {
        editor.insertSnippet(new vscode.SnippetString(choice.value));
    }
}
