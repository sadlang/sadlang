/**
 * @file dependency_graph.ts
 * @brief رسم بياني للاعتمادات — Dependency Graph Visualization
 * 
 * يعرض رسماً بيانياً تفاعلياً للعلاقات بين الملفات والوحدات
 * (استيراد/تصدير) في مشروع لغة ص
 */

import * as vscode from 'vscode';
import * as path from 'path';

interface FileNode {
    name: string;
    fullPath: string;
    imports: string[];
    exports: string[];
}

export function registerDependencyGraph(context: vscode.ExtensionContext): void {
    context.subscriptions.push(
        vscode.commands.registerCommand('sad.showDependencyGraph', async () => {
            await showDependencyGraph(context);
        })
    );
}

async function showDependencyGraph(context: vscode.ExtensionContext): Promise<void> {
    // البحث عن جميع ملفات .ص
    const files = await vscode.workspace.findFiles('**/*.ص', '**/node_modules/**');
    if (files.length === 0) {
        vscode.window.showInformationMessage('لا توجد ملفات .ص في المشروع');
        return;
    }

    const nodes: FileNode[] = [];

    for (const file of files) {
        const doc = await vscode.workspace.openTextDocument(file);
        const text = doc.getText();
        const name = path.basename(file.fsPath);

        const imports: string[] = [];
        const exports: string[] = [];

        // اكتشاف الاستيرادات
        const importRegex = /استورد\s+.*?\s+من\s+"([^"]+)"/g;
        let match;
        while ((match = importRegex.exec(text)) !== null) {
            imports.push(match[1]);
        }

        // اكتشاف التصديرات
        const exportRegex = /صدّر\s+(?:دالة|صنف|متغير|ثابت)\s+(\S+)/g;
        while ((match = exportRegex.exec(text)) !== null) {
            exports.push(match[1]);
        }

        nodes.push({ name, fullPath: file.fsPath, imports, exports });
    }

    // بناء Webview
    const panel = vscode.window.createWebviewPanel(
        'sadDependencyGraph', 'رسم الاعتمادات — لغة ص',
        vscode.ViewColumn.One, { enableScripts: true }
    );

    panel.webview.html = buildGraphHTML(nodes);

    panel.webview.onDidReceiveMessage((msg) => {
        if (msg.type === 'openFile') {
            const node = nodes.find(n => n.name === msg.name);
            if (node) {
                vscode.workspace.openTextDocument(node.fullPath)
                    .then(doc => vscode.window.showTextDocument(doc));
            }
        }
    });
}

function buildGraphHTML(nodes: FileNode[]): string {
    // بناء العلاقات
    let edges = '';
    let nodesHTML = '';
    const colors = ['#569cd6', '#4ec9b0', '#c586c0', '#dcdcaa', '#ce9178', '#6a9955', '#f44747'];

    for (let i = 0; i < nodes.length; i++) {
        const node = nodes[i];
        const color = colors[i % colors.length];
        const x = 100 + (i % 4) * 200;
        const y = 100 + Math.floor(i / 4) * 150;

        nodesHTML += `<div class="node" style="left:${x}px;top:${y}px;border-color:${color}" 
            onclick="openFile('${node.name}')" draggable="true"
            ondragstart="startDrag(event, ${i})" id="node-${i}">
            <div class="node-name" style="color:${color}">📄 ${node.name}</div>
            <div class="node-stats">
                ${node.imports.length > 0 ? `<span class="import-count">⬇ ${node.imports.length} استيراد</span>` : ''}
                ${node.exports.length > 0 ? `<span class="export-count">⬆ ${node.exports.length} تصدير</span>` : ''}
            </div>
        </div>\n`;

        // الحواف
        for (const imp of node.imports) {
            const target = nodes.findIndex(n => n.name.replace('.ص', '') === imp.replace('.ص', ''));
            if (target >= 0) {
                edges += `{from:${i},to:${target}},`;
            }
        }
    }

    return `<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head><meta charset="UTF-8">
<style>
body { background: #1e1e1e; color: #d4d4d4; font-family: 'Cascadia Code', monospace; margin: 0; overflow: hidden; }
.header { background: #252526; padding: 8px 16px; border-bottom: 1px solid #333; display: flex; justify-content: space-between; direction: rtl; }
.header h2 { color: #569cd6; margin: 0; font-size: 14px; }
.header .stats { color: #858585; font-size: 12px; }
.canvas { position: relative; width: 100%; height: calc(100vh - 40px); overflow: auto; }
.node { position: absolute; background: #252526; border: 2px solid #569cd6; border-radius: 8px; padding: 10px 16px; cursor: pointer; transition: all 0.2s; min-width: 120px; text-align: center; z-index: 2; }
.node:hover { transform: scale(1.05); box-shadow: 0 4px 12px rgba(0,0,0,0.5); }
.node-name { font-weight: bold; font-size: 13px; white-space: nowrap; }
.node-stats { font-size: 11px; color: #858585; margin-top: 4px; }
.import-count { color: #4ec9b0; margin-left: 8px; }
.export-count { color: #c586c0; margin-left: 8px; }
svg { position: absolute; top: 0; left: 0; width: 100%; height: 100%; z-index: 1; }
</style>
</head>
<body>
<div class="header">
    <h2>🕸️ رسم الاعتمادات</h2>
    <span class="stats">${nodes.length} ملف | ${nodes.reduce((a, n) => a + n.imports.length, 0)} علاقة</span>
</div>
<div class="canvas" id="canvas">
    <svg id="edges"></svg>
    ${nodesHTML}
</div>
<script>
const vscode = acquireVsCodeApi();
const edges = [${edges}];
function openFile(name) { vscode.postMessage({ type: 'openFile', name }); }

// رسم الخطوط بين العقد
function drawEdges() {
    const svg = document.getElementById('edges');
    svg.innerHTML = '';
    for (const e of edges) {
        const from = document.getElementById('node-' + e.from);
        const to = document.getElementById('node-' + e.to);
        if (!from || !to) continue;
        const x1 = from.offsetLeft + from.offsetWidth/2;
        const y1 = from.offsetTop + from.offsetHeight/2;
        const x2 = to.offsetLeft + to.offsetWidth/2;
        const y2 = to.offsetTop + to.offsetHeight/2;
        const line = document.createElementNS('http://www.w3.org/2000/svg', 'line');
        line.setAttribute('x1', x1); line.setAttribute('y1', y1);
        line.setAttribute('x2', x2); line.setAttribute('y2', y2);
        line.setAttribute('stroke', '#555'); line.setAttribute('stroke-width', '1.5');
        line.setAttribute('stroke-dasharray', '4,4');
        svg.appendChild(line);
    }
}

let dragOffset = {};
function startDrag(e, idx) {
    const el = document.getElementById('node-' + idx);
    dragOffset = { x: e.clientX - el.offsetLeft, y: e.clientY - el.offsetTop, idx };
    document.addEventListener('dragover', onDrag);
    document.addEventListener('drop', onDrop);
}
function onDrag(e) { e.preventDefault(); }
function onDrop(e) {
    e.preventDefault();
    const el = document.getElementById('node-' + dragOffset.idx);
    el.style.left = (e.clientX - dragOffset.x) + 'px';
    el.style.top = (e.clientY - dragOffset.y) + 'px';
    drawEdges();
    document.removeEventListener('dragover', onDrag);
    document.removeEventListener('drop', onDrop);
}

drawEdges();
</script>
</body></html>`;
}
