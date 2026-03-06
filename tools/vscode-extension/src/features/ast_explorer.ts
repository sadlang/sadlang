/**
 * @file ast_explorer.ts
 * @brief مستكشف شجرة AST التفاعلية — Interactive AST Explorer
 * 
 * هذا الملف يوفر WebView تفاعلي يعرض شجرة بناء الجملة التجريدية (AST)
 * للملف الحالي. يتم تحليل الكود محلياً واستخراج البنية الشجرية.
 * 
 * === الميزات ===
 * 
 * 1. عرض شجري تفاعلي:
 *    - كل عقدة تمثل عنصراً نحوياً (دالة، شرط، حلقة، إلخ)
 *    - الألوان تميّز الأنواع المختلفة
 *    - يمكن طي/فتح الفروع
 * 
 * 2. ربط ثنائي:
 *    - الضغط على عقدة يُحدد الكود المقابل في المحرر
 *    - تحريك المؤشر في المحرر يُبرز العقدة المقابلة
 * 
 * 3. تحديث مباشر:
 *    - الشجرة تتحدث تلقائياً عند تغيير الكود
 *    - debounce لتجنب التحديث المتكرر السريع
 * 
 * 4. بحث:
 *    - يمكن البحث عن عقد بالاسم أو النوع
 * 
 * === بنية الشجرة ===
 * 
 * العقدة الجذرية = البرنامج (Program)
 *   ├── دالة (Function) — اسم، معاملات
 *   │   ├── شرط (If) — الشرط
 *   │   │   ├── جسم الشرط
 *   │   │   └── وإلا
 *   │   └── ارجع (Return) — التعبير
 *   ├── صنف (Class) — اسم، أب
 *   │   ├── باني (Constructor)
 *   │   └── دالة (Method)
 *   └── تعبير (Expression) — نوع التعبير
 * 
 * @author فريق لغة ص
 * @date 2025
 */

import * as vscode from 'vscode';

/**
 * @brief عقدة في شجرة AST المبسطة
 * 
 * كل عقدة تمثل عنصراً واحداً في بناء الجملة.
 * يُحافظ على بنية شجرية قابلة للتسلسل إلى JSON لإرسالها إلى WebView.
 */
interface ASTNode {
    /** نوع العقدة (بالعربية) */
    type: string;
    /** الاسم (إن وجد) — مثل اسم الدالة أو المتغير */
    name?: string;
    /** تفاصيل إضافية */
    details?: string;
    /** رقم السطر (1-based) */
    line: number;
    /** العقد الأبناء */
    children: ASTNode[];
    /** لون العقدة في العرض */
    color: string;
    /** أيقونة العقدة */
    icon: string;
}

/**
 * @brief ألوان أنواع العقد المختلفة
 * 
 * كل نوع عقدة له لون مميز لتسهيل القراءة البصرية.
 */
const NODE_COLORS: Record<string, string> = {
    'برنامج': '#4fc3f7',
    'دالة': '#81c784',
    'صنف': '#ffb74d',
    'بنية': '#ce93d8',
    'تعداد': '#f06292',
    'سمة': '#a1887f',
    'شرط': '#64b5f6',
    'حلقة': '#4dd0e1',
    'لكل': '#4dd0e1',
    'متغير': '#dce775',
    'ثابت': '#fff176',
    'ارجع': '#ef5350',
    'باني': '#aed581',
    'استورد': '#90a4ae',
    'طابق': '#b39ddb',
    'حاول': '#ffcc80',
    'تعبير': '#e0e0e0',
    'خاصية': '#80deea',
    'تعليق': '#757575',
};

/**
 * @brief أيقونات أنواع العقد
 */
const NODE_ICONS: Record<string, string> = {
    'برنامج': '📄',
    'دالة': '⚡',
    'صنف': '🏗️',
    'بنية': '📦',
    'تعداد': '📋',
    'سمة': '🔌',
    'شرط': '❓',
    'حلقة': '🔄',
    'لكل': '🔁',
    'متغير': '📝',
    'ثابت': '🔒',
    'ارجع': '↩️',
    'باني': '🔨',
    'استورد': '📥',
    'طابق': '🎯',
    'حاول': '🛡️',
    'تعبير': '💬',
    'خاصية': '🏷️',
    'تعليق': '💭',
};

/**
 * @brief تحليل كود لغة ص وبناء شجرة AST مبسطة
 * 
 * خوارزمية البناء:
 *   1. قراءة الكود سطراً سطراً
 *   2. تحديد نوع كل سطر (دالة، شرط، متغير، إلخ) باستخدام الأنماط
 *   3. استخدام مكدس (stack) لتتبع الكتل المفتوحة
 *   4. إغلاق الكتلة عند مصادفة نهاية بالمسافة البادئة المطابقة
 *   5. إرجاع العقدة الجذرية مع جميع الأبناء
 * 
 * @param code نص كود لغة ص
 * @returns عقدة الجذر (برنامج) مع الأبناء
 */
function buildAST(code: string): ASTNode {
    const lines = code.split('\n');
    
    // العقدة الجذرية
    const root: ASTNode = {
        type: 'برنامج',
        line: 1,
        children: [],
        color: NODE_COLORS['برنامج'],
        icon: NODE_ICONS['برنامج'],
    };
    
    // مكدس العقد — العنصر الأخير هو الحاوية الحالية
    const stack: ASTNode[] = [root];
    
    /**
     * إنشاء عقدة جديدة وإضافتها كابن للحاوية الحالية
     */
    function addNode(type: string, line: number, name?: string, details?: string, isBlock: boolean = false): ASTNode {
        const node: ASTNode = {
            type,
            name,
            details,
            line,
            children: [],
            color: NODE_COLORS[type] || '#e0e0e0',
            icon: NODE_ICONS[type] || '📌',
        };
        
        // إضافة كابن للحاوية الحالية
        const parent = stack[stack.length - 1];
        parent.children.push(node);
        
        // إذا كانت عقدة كتلة (تحتوي أبناء)، ندفعها للمكدس
        if (isBlock) {
            stack.push(node);
        }
        
        return node;
    }
    
    // أنماط الاكتشاف
    const patterns = [
        { regex: /^(\s*)(غير_متزامن\s+)?(ساكن\s+)?دالة\s+([\u0600-\u06FF\u0750-\u077F\w]+)\s*\(([^)]*)\)/, 
          handler: (m: RegExpMatchArray, ln: number) => {
            const params = m[5] ? m[5].trim() : '';
            addNode('دالة', ln, m[4], params ? `(${params})` : '()', true);
          }
        },
        { regex: /^(\s*)صنف\s+([\u0600-\u06FF\u0750-\u077F\w]+)(\s+يرث\s+([\u0600-\u06FF\u0750-\u077F\w]+))?/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('صنف', ln, m[2], m[4] ? `يرث ${m[4]}` : undefined, true);
          }
        },
        { regex: /^(\s*)بنية\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('بنية', ln, m[2], undefined, true);
          }
        },
        { regex: /^(\s*)تعداد\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('تعداد', ln, m[2], undefined, true);
          }
        },
        { regex: /^(\s*)سمة\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('سمة', ln, m[2], undefined, true);
          }
        },
        { regex: /^(\s*)باني\s*\(([^)]*)\)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('باني', ln, 'باني', m[2] ? `(${m[2].trim()})` : '()', true);
          }
        },
        { regex: /^(\s*)(إذا|اذا)\s*\((.+)\)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('شرط', ln, 'إذا', m[3].trim(), true);
          }
        },
        { regex: /^(\s*)(وإلا|والا)\s*(إذا|اذا)?\s*(\((.+)\))?/,
          handler: (m: RegExpMatchArray, ln: number) => {
            // إغلاق الشرط السابق
            if (stack.length > 1 && stack[stack.length - 1].type === 'شرط') {
              stack.pop();
            }
            const label = m[3] ? 'وإلا إذا' : 'وإلا';
            addNode('شرط', ln, label, m[5] || undefined, true);
          }
        },
        { regex: /^(\s*)بينما\s*\((.+)\)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('حلقة', ln, 'بينما', m[2].trim(), true);
          }
        },
        { regex: /^(\s*)لكل\s+(.+)\s+في\s+(.+)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('لكل', ln, m[2].trim(), `في ${m[3].trim()}`, true);
          }
        },
        { regex: /^(\s*)طابق\s*\((.+)\)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('طابق', ln, 'طابق', m[2].trim(), true);
          }
        },
        { regex: /^(\s*)حاول\s*$/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('حاول', ln, 'حاول', undefined, true);
          }
        },
        { regex: /^(\s*)متغير\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('متغير', ln, m[2]);
          }
        },
        { regex: /^(\s*)ثابت\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('ثابت', ln, m[2]);
          }
        },
        { regex: /^(\s*)ارجع\b/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('ارجع', ln, undefined, undefined);
          }
        },
        { regex: /^(\s*)استورد\s+(.+)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('استورد', ln, m[2].trim());
          }
        },
        { regex: /^(\s*)خاصية\s+([\u0600-\u06FF\u0750-\u077F\w]+)/,
          handler: (m: RegExpMatchArray, ln: number) => {
            addNode('خاصية', ln, m[2], undefined, true);
          }
        },
    ];
    
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i];
        const trimmed = line.trimStart();
        const lineNum = i + 1; // 1-based
        
        // تخطي الأسطر الفارغة
        if (!trimmed) { continue; }
        
        // إغلاق الكتل عند نهاية
        if (trimmed === 'نهاية') {
            if (stack.length > 1) {
                stack.pop();
            }
            continue;
        }
        
        // تخطي التعليقات (لكن نضيف عقدة للتعليقات التوثيقية)
        if (trimmed.startsWith('#**') || trimmed.startsWith('##')) {
            addNode('تعليق', lineNum, trimmed.substring(0, 30) + (trimmed.length > 30 ? '...' : ''));
            continue;
        }
        if (trimmed.startsWith('#')) { continue; }
        
        // محاولة مطابقة كل نمط
        let matched = false;
        for (const { regex, handler } of patterns) {
            const match = line.match(regex);
            if (match) {
                handler(match, lineNum);
                matched = true;
                break;
            }
        }
        
        // إذا لم يتطابق أي نمط — عبارة عادية (تعبير)
        if (!matched && trimmed.length > 2) {
            // نضيف فقط التعبيرات المهمة (مثل استدعاءات الدوال)
            if (trimmed.includes('(') || trimmed.includes('=')) {
                addNode('تعبير', lineNum, undefined, trimmed.substring(0, 40) + (trimmed.length > 40 ? '...' : ''));
            }
        }
    }
    
    return root;
}

/**
 * @brief إنشاء محتوى HTML لـ WebView
 * 
 * يبني صفحة HTML كاملة تعرض شجرة AST بشكل تفاعلي.
 * تستخدم CSS مخصص ورسم شجري بـ JavaScript.
 * 
 * @param webview كائن WebView لبناء URIs
 * @param ast الشجرة المراد عرضها
 * @returns نص HTML كامل
 */
function getWebviewContent(ast: ASTNode): string {
    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>مستكشف AST — لغة ص</title>
    <style>
        /* === الأنماط العامة === */
        body {
            font-family: 'Segoe UI', Tahoma, sans-serif;
            background: var(--vscode-editor-background);
            color: var(--vscode-editor-foreground);
            margin: 0;
            padding: 16px;
            direction: rtl;
        }
        
        /* === شريط الأدوات === */
        .toolbar {
            display: flex;
            gap: 8px;
            margin-bottom: 16px;
            align-items: center;
        }
        
        .toolbar input {
            flex: 1;
            padding: 6px 12px;
            background: var(--vscode-input-background);
            color: var(--vscode-input-foreground);
            border: 1px solid var(--vscode-input-border);
            border-radius: 4px;
            font-size: 14px;
        }
        
        .toolbar button {
            padding: 6px 12px;
            background: var(--vscode-button-background);
            color: var(--vscode-button-foreground);
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 13px;
        }
        
        .toolbar button:hover {
            background: var(--vscode-button-hoverBackground);
        }
        
        /* === عقدة الشجرة === */
        .tree-node {
            margin-right: 16px;
            padding: 2px 0;
        }
        
        .tree-node > .node-header {
            display: flex;
            align-items: center;
            gap: 6px;
            padding: 4px 8px;
            border-radius: 4px;
            cursor: pointer;
            user-select: none;
            transition: background 0.15s;
        }
        
        .tree-node > .node-header:hover {
            background: var(--vscode-list-hoverBackground);
        }
        
        .tree-node > .node-header.selected {
            background: var(--vscode-list-activeSelectionBackground);
            color: var(--vscode-list-activeSelectionForeground);
        }
        
        .node-toggle {
            width: 16px;
            text-align: center;
            font-size: 12px;
            color: var(--vscode-foreground);
            opacity: 0.6;
        }
        
        .node-icon { font-size: 14px; }
        
        .node-type {
            font-weight: bold;
            font-size: 13px;
            padding: 1px 6px;
            border-radius: 3px;
            color: #fff;
        }
        
        .node-name {
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 13px;
            color: var(--vscode-symbolIcon-functionForeground, #dcdcaa);
        }
        
        .node-details {
            font-size: 12px;
            opacity: 0.7;
            font-style: italic;
        }
        
        .node-line {
            font-size: 11px;
            opacity: 0.5;
            margin-right: auto;
        }
        
        .node-children {
            border-right: 1px solid var(--vscode-tree-indentGuidesStroke, #585858);
            margin-right: 8px;
        }
        
        .node-children.collapsed { display: none; }
        
        /* === إحصائيات === */
        .stats {
            display: flex;
            gap: 16px;
            padding: 8px 12px;
            background: var(--vscode-editorWidget-background);
            border-radius: 4px;
            margin-bottom: 12px;
            font-size: 13px;
            flex-wrap: wrap;
        }
        
        .stat-item { display: flex; gap: 4px; align-items: center; }
        .stat-value { font-weight: bold; }
        
        .highlight { background: rgba(255, 213, 79, 0.3) !important; }
    </style>
</head>
<body>
    <div class="toolbar">
        <input type="text" id="search" placeholder="بحث عن عقدة..." />
        <button onclick="expandAll()">فتح الكل</button>
        <button onclick="collapseAll()">طي الكل</button>
    </div>
    
    <div class="stats" id="stats"></div>
    
    <div id="tree"></div>
    
    <script>
        // بيانات AST المُرسلة من الإضافة
        const astData = ${JSON.stringify(ast)};
        const vscode = acquireVsCodeApi();
        
        /**
         * عد العقد حسب النوع لعرض الإحصائيات
         */
        function countNodes(node) {
            const counts = {};
            function walk(n) {
                counts[n.type] = (counts[n.type] || 0) + 1;
                for (const child of n.children) { walk(child); }
            }
            walk(node);
            return counts;
        }
        
        /**
         * بناء HTML لعقدة واحدة وأبنائها
         */
        function renderNode(node, depth) {
            const hasChildren = node.children.length > 0;
            const toggle = hasChildren ? '▼' : '·';
            
            let html = '<div class="tree-node" data-line="' + node.line + '">';
            html += '<div class="node-header" onclick="toggleNode(this, ' + node.line + ')">';
            html += '<span class="node-toggle">' + toggle + '</span>';
            html += '<span class="node-icon">' + node.icon + '</span>';
            html += '<span class="node-type" style="background:' + node.color + '">' + node.type + '</span>';
            if (node.name) {
                html += '<span class="node-name">' + node.name + '</span>';
            }
            if (node.details) {
                html += '<span class="node-details">' + node.details + '</span>';
            }
            html += '<span class="node-line">سطر ' + node.line + '</span>';
            html += '</div>';
            
            if (hasChildren) {
                html += '<div class="node-children">';
                for (const child of node.children) {
                    html += renderNode(child, depth + 1);
                }
                html += '</div>';
            }
            
            html += '</div>';
            return html;
        }
        
        /**
         * طي/فتح عقدة عند الضغط عليها
         */
        function toggleNode(header, line) {
            const node = header.parentElement;
            const children = node.querySelector('.node-children');
            if (children) {
                children.classList.toggle('collapsed');
                const toggle = header.querySelector('.node-toggle');
                toggle.textContent = children.classList.contains('collapsed') ? '▶' : '▼';
            }
            
            // إرسال رقم السطر للإضافة لتحريك المؤشر
            vscode.postMessage({ type: 'goToLine', line: line });
            
            // تحديد العقدة
            document.querySelectorAll('.node-header.selected').forEach(
                el => el.classList.remove('selected')
            );
            header.classList.add('selected');
        }
        
        /**
         * فتح جميع العقد
         */
        function expandAll() {
            document.querySelectorAll('.node-children').forEach(el => {
                el.classList.remove('collapsed');
            });
            document.querySelectorAll('.node-toggle').forEach(el => {
                if (el.textContent === '▶') el.textContent = '▼';
            });
        }
        
        /**
         * طي جميع العقد
         */
        function collapseAll() {
            document.querySelectorAll('.node-children').forEach(el => {
                el.classList.add('collapsed');
            });
            document.querySelectorAll('.node-toggle').forEach(el => {
                if (el.textContent === '▼') el.textContent = '▶';
            });
        }
        
        // === البحث ===
        document.getElementById('search').addEventListener('input', function(e) {
            const query = e.target.value.trim().toLowerCase();
            document.querySelectorAll('.tree-node').forEach(node => {
                const header = node.querySelector('.node-header');
                if (!query) {
                    header.classList.remove('highlight');
                    return;
                }
                const text = header.textContent.toLowerCase();
                if (text.includes(query)) {
                    header.classList.add('highlight');
                    // فتح الآباء
                    let parent = node.parentElement;
                    while (parent) {
                        if (parent.classList.contains('node-children')) {
                            parent.classList.remove('collapsed');
                        }
                        parent = parent.parentElement;
                    }
                } else {
                    header.classList.remove('highlight');
                }
            });
        });
        
        // === الرسم الأولي ===
        document.getElementById('tree').innerHTML = renderNode(astData, 0);
        
        // عرض الإحصائيات
        const counts = countNodes(astData);
        let statsHtml = '';
        for (const [type, count] of Object.entries(counts)) {
            if (type === 'برنامج') continue;
            statsHtml += '<span class="stat-item"><span class="stat-value">' + count + '</span> ' + type + '</span>';
        }
        document.getElementById('stats').innerHTML = statsHtml;
        
        // === استقبال رسائل من الإضافة ===
        window.addEventListener('message', event => {
            const msg = event.data;
            if (msg.type === 'update') {
                // تحديث الشجرة
                document.getElementById('tree').innerHTML = renderNode(msg.ast, 0);
                const counts = countNodes(msg.ast);
                let statsHtml = '';
                for (const [type, count] of Object.entries(counts)) {
                    if (type === 'برنامج') continue;
                    statsHtml += '<span class="stat-item"><span class="stat-value">' + count + '</span> ' + type + '</span>';
                }
                document.getElementById('stats').innerHTML = statsHtml;
            }
            if (msg.type === 'highlight') {
                // تحديد عقدة من المحرر
                document.querySelectorAll('.node-header.selected').forEach(
                    el => el.classList.remove('selected')
                );
                const target = document.querySelector('[data-line="' + msg.line + '"]');
                if (target) {
                    const header = target.querySelector('.node-header');
                    header.classList.add('selected');
                    header.scrollIntoView({ behavior: 'smooth', block: 'center' });
                }
            }
        });
    </script>
</body>
</html>`;
}

/**
 * @brief تسجيل مستكشف AST في VS Code
 * 
 * يسجل الأمر sad.showAST الذي:
 *   1. يبني AST من الملف الحالي
 *   2. يفتح WebView جانبي يعرض الشجرة
 *   3. يراقب تغييرات المستند لتحديث الشجرة
 *   4. يراقب تحريك المؤشر لتحديد العقدة المقابلة
 * 
 * @param context سياق الإضافة
 */
export function registerASTExplorer(context: vscode.ExtensionContext): void {
    let currentPanel: vscode.WebviewPanel | undefined;
    
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.showAST', () => {
            const editor = vscode.window.activeTextEditor;
            if (!editor || editor.document.languageId !== 'sad') {
                vscode.window.showWarningMessage('افتح ملف .ص لعرض شجرة AST');
                return;
            }
            
            // إذا كان اللوح مفتوحاً بالفعل، نحدّثه
            if (currentPanel) {
                currentPanel.reveal(vscode.ViewColumn.Beside);
                updatePanel(editor.document);
                return;
            }
            
            // إنشاء لوح WebView جديد
            currentPanel = vscode.window.createWebviewPanel(
                'sadAST',
                'شجرة AST — لغة ص',
                vscode.ViewColumn.Beside,
                {
                    enableScripts: true,
                    retainContextWhenHidden: true,
                }
            );
            
            // البناء الأولي
            updatePanel(editor.document);
            
            // === استقبال رسائل من WebView ===
            currentPanel.webview.onDidReceiveMessage(
                (message: any) => {
                    if (message.type === 'goToLine') {
                        const line = message.line - 1; // تحويل إلى 0-based
                        const editor = vscode.window.activeTextEditor;
                        if (editor) {
                            const range = new vscode.Range(line, 0, line, 0);
                            editor.selection = new vscode.Selection(range.start, range.start);
                            editor.revealRange(range, vscode.TextEditorRevealType.InCenter);
                        }
                    }
                },
                undefined,
                context.subscriptions
            );
            
            // === تحديث عند تغيير المستند ===
            let debounceTimer: ReturnType<typeof setTimeout> | undefined;
            const changeDisposable = vscode.workspace.onDidChangeTextDocument((event: vscode.TextDocumentChangeEvent) => {
                if (event.document.languageId === 'sad' && currentPanel) {
                    if (debounceTimer) { clearTimeout(debounceTimer); }
                    debounceTimer = setTimeout(() => {
                        updatePanel(event.document);
                    }, 500); // debounce: 500ms
                }
            });
            
            // === تحديد عقدة عند تحريك المؤشر ===
            const cursorDisposable = vscode.window.onDidChangeTextEditorSelection((event: vscode.TextEditorSelectionChangeEvent) => {
                if (event.textEditor.document.languageId === 'sad' && currentPanel) {
                    const line = event.selections[0].active.line + 1; // 1-based
                    currentPanel.webview.postMessage({ type: 'highlight', line });
                }
            });
            
            // تنظيف عند إغلاق اللوح
            currentPanel.onDidDispose(() => {
                currentPanel = undefined;
                changeDisposable.dispose();
                cursorDisposable.dispose();
                if (debounceTimer) { clearTimeout(debounceTimer); }
            });
            
            /**
             * تحديث محتوى اللوح
             */
            function updatePanel(document: vscode.TextDocument): void {
                if (!currentPanel) { return; }
                const ast = buildAST(document.getText());
                currentPanel.webview.html = getWebviewContent(ast);
            }
        })
    );
}
