/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: html_css_codegen.cpp
 * المسار: features/graphics/backends/web/src/html_css_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * توليد CSS الأساسي (Design Tokens، Dark Mode، Responsive، Animations).
 * جزء من مولّد HTML لنظام واجهات لغة ص.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/web/html_codegen.h"

namespace sad {
namespace ui {
namespace web {

void HtmlCodegen::generateBaseCSS(std::ostringstream& out) {
    if (options_.include_reset_css) {
        out << ind(2) << "* { margin: 0; padding: 0; box-sizing: border-box; }\n";
    }

    // ═══ CSS Custom Properties (Design Tokens) ═══
    out << ind(2) << ":root {\n"
        << ind(3) << "--sad-primary: #1E88E5;\n"
        << ind(3) << "--sad-primary-dark: #1565C0;\n"
        << ind(3) << "--sad-primary-light: #64B5F6;\n"
        << ind(3) << "--sad-secondary: #FF6F00;\n"
        << ind(3) << "--sad-success: #4CAF50;\n"
        << ind(3) << "--sad-error: #F44336;\n"
        << ind(3) << "--sad-warning: #FF9800;\n"
        << ind(3) << "--sad-info: #2196F3;\n"
        << ind(3) << "--sad-bg: #FFFFFF;\n"
        << ind(3) << "--sad-bg-secondary: #FAFAFA;\n"
        << ind(3) << "--sad-surface: #FFFFFF;\n"
        << ind(3) << "--sad-text: #212121;\n"
        << ind(3) << "--sad-text-secondary: #757575;\n"
        << ind(3) << "--sad-border: #E0E0E0;\n"
        << ind(3) << "--sad-shadow: rgba(0,0,0,0.1);\n"
        << ind(3) << "--sad-shadow-hover: rgba(0,0,0,0.15);\n"
        << ind(3) << "--sad-radius: 8px;\n"
        << ind(3) << "--sad-radius-lg: 12px;\n"
        << ind(3) << "--sad-radius-full: 9999px;\n"
        << ind(3) << "--sad-font: 'Cairo', 'Segoe UI', system-ui, sans-serif;\n"
        << ind(3) << "--sad-font-mono: 'Fira Code', 'Consolas', monospace;\n"
        << ind(3) << "--sad-transition: 0.2s ease;\n"
        << ind(3) << "--sad-focus-ring: 0 0 0 3px rgba(30,136,229,0.3);\n"
        << ind(2) << "}\n"

    // ═══ Dark Mode CSS Variables ═══
        << ind(2) << "@media (prefers-color-scheme: dark) {\n"
        << ind(3) << ":root {\n"
        << ind(4) << "--sad-primary: #64B5F6;\n"
        << ind(4) << "--sad-primary-dark: #42A5F5;\n"
        << ind(4) << "--sad-bg: #121212;\n"
        << ind(4) << "--sad-bg-secondary: #1E1E1E;\n"
        << ind(4) << "--sad-surface: #1E1E1E;\n"
        << ind(4) << "--sad-text: #E0E0E0;\n"
        << ind(4) << "--sad-text-secondary: #9E9E9E;\n"
        << ind(4) << "--sad-border: #333333;\n"
        << ind(4) << "--sad-shadow: rgba(0,0,0,0.3);\n"
        << ind(4) << "--sad-shadow-hover: rgba(0,0,0,0.4);\n"
        << ind(4) << "--sad-focus-ring: 0 0 0 3px rgba(100,181,246,0.3);\n"
        << ind(3) << "}\n"
        << ind(2) << "}\n";

    // ═══ Base Layout ═══
    out << ind(2) << "html { scroll-behavior: smooth; }\n"
        << ind(2) << "body { font-family: var(--sad-font); "
        << "direction: " << options_.dir << "; line-height: 1.6; "
        << "background: var(--sad-bg); color: var(--sad-text); }\n"
        << ind(2) << ".sad-column { display: flex; flex-direction: column; gap: 8px; }\n"
        << ind(2) << ".sad-row { display: flex; flex-direction: "
        << (options_.dir == "rtl" ? "row-reverse" : "row") << "; gap: 8px; align-items: center; }\n"
        << ind(2) << ".sad-stack { position: relative; }\n"
        << ind(2) << ".sad-grid { display: grid; }\n"
        << ind(2) << ".sad-spacer { flex-grow: 1; }\n"
        << ind(2) << ".sad-divider { border-bottom: 1px solid var(--sad-border); width: 100%; }\n";

    // ═══ Component Styles ═══
    out << ind(2) << ".sad-card { background: var(--sad-surface); border-radius: var(--sad-radius); "
        << "box-shadow: 0 2px 4px var(--sad-shadow); padding: 16px; }\n"
        << ind(2) << ".sad-button { padding: 12px 24px; border: none; border-radius: var(--sad-radius); "
        << "background: var(--sad-primary); color: white; cursor: pointer; "
        << "font-size: 16px; font-family: inherit; }\n"
        << ind(2) << ".sad-button:hover { background: var(--sad-primary-dark); }\n"
        << ind(2) << ".sad-input { padding: 12px; border: 1px solid var(--sad-border); "
        << "border-radius: var(--sad-radius); font-size: 16px; font-family: inherit; "
        << "background: var(--sad-bg); color: var(--sad-text); "
        << "direction: " << options_.dir << "; }\n"
        << ind(2) << ".sad-toggle { position: relative; display: inline-block; width: 50px; height: 26px; }\n"
        << ind(2) << ".sad-toggle input { opacity: 0; width: 0; height: 0; }\n"
        << ind(2) << ".sad-slider { width: 100%; accent-color: var(--sad-primary); }\n"
        << ind(2) << ".sad-checkbox { accent-color: var(--sad-primary); width: 18px; height: 18px; }\n"
        << ind(2) << ".sad-radio { accent-color: var(--sad-primary); width: 18px; height: 18px; }\n"
        << ind(2) << ".sad-search { padding: 12px 12px 12px 36px; border: 1px solid #ccc; "
        << "border-radius: 24px; font-size: 16px; width: 100%; "
        << "background: url('data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"16\" height=\"16\" viewBox=\"0 0 24 24\"><circle cx=\"10\" cy=\"10\" r=\"7\" fill=\"none\" stroke=\"gray\" stroke-width=\"2\"/><line x1=\"15\" y1=\"15\" x2=\"21\" y2=\"21\" stroke=\"gray\" stroke-width=\"2\"/></svg>') no-repeat 12px center; }\n"
        << ind(2) << ".sad-progress { width: 100%; height: 6px; accent-color: var(--sad-primary); }\n"
        << ind(2) << ".sad-badge { display: inline-flex; align-items: center; justify-content: center; "
        << "min-width: 20px; height: 20px; border-radius: 10px; background: #F44336; "
        << "color: white; font-size: 12px; padding: 0 6px; }\n"
        << ind(2) << ".sad-chip { display: inline-flex; align-items: center; padding: 6px 12px; "
        << "border-radius: 16px; background: #E0E0E0; font-size: 14px; gap: 4px; }\n"
        << ind(2) << ".sad-avatar { display: inline-flex; align-items: center; justify-content: center; "
        << "width: 40px; height: 40px; border-radius: 50%; background: var(--sad-primary); "
        << "color: white; font-weight: bold; font-size: 16px; }\n"
        << ind(2) << ".sad-fab { position: fixed; bottom: 24px; left: 24px; width: 56px; height: 56px; "
        << "border-radius: 50%; background: var(--sad-primary); color: white; border: none; "
        << "font-size: 24px; cursor: pointer; box-shadow: 0 4px 8px rgba(0,0,0,0.2); "
        << "display: flex; align-items: center; justify-content: center; }\n"
        << ind(2) << ".sad-snackbar { position: fixed; bottom: 16px; left: 50%; transform: translateX(-50%); "
        << "background: #323232; color: white; padding: 14px 24px; border-radius: 8px; "
        << "font-size: 14px; box-shadow: 0 3px 6px rgba(0,0,0,0.2); }\n"
        << ind(2) << ".sad-appbar { display: flex; align-items: center; padding: 12px 16px; "
        << "background: var(--sad-primary); color: white; font-size: 20px; }\n"
        << ind(2) << ".sad-scaffold { display: flex; flex-direction: column; min-height: 100vh; }\n"
        << ind(2) << ".sad-drawer { position: fixed; top: 0; right: 0; width: 280px; height: 100%; "
        << "background: var(--sad-surface); box-shadow: -2px 0 8px var(--sad-shadow); z-index: 1000; }\n"
        << ind(2) << ".sad-bottom-sheet { position: fixed; bottom: 0; left: 0; right: 0; "
        << "background: white; border-radius: 16px 16px 0 0; box-shadow: 0 -2px 8px rgba(0,0,0,0.1); "
        << "padding: 16px; min-height: 200px; }\n"
        << ind(2) << ".sad-tabs { display: flex; border-bottom: 2px solid #E0E0E0; }\n"
        << ind(2) << ".sad-tab { padding: 12px 24px; cursor: pointer; border: none; background: transparent; "
        << "font-size: 14px; }\n"
        << ind(2) << ".sad-tab.active { border-bottom: 2px solid var(--sad-primary); color: var(--sad-primary); }\n"
        << ind(2) << ".sad-nav { display: flex; flex-direction: column; }\n"
        << ind(2) << ".sad-list { list-style: none; padding: 0; }\n"
        << ind(2) << ".sad-list-item { padding: 12px 16px; border-bottom: 1px solid var(--sad-border); }\n"
        << ind(2) << ".sad-alert { position: fixed; top: 50%; left: 50%; transform: translate(-50%, -50%); "
        << "background: white; padding: 24px; border-radius: 12px; box-shadow: 0 4px 16px rgba(0,0,0,0.2); "
        << "z-index: 1001; min-width: 280px; }\n"
        << ind(2) << ".sad-dialog { position: fixed; top: 50%; left: 50%; transform: translate(-50%, -50%); "
        << "background: white; padding: 24px; border-radius: 12px; box-shadow: 0 4px 16px rgba(0,0,0,0.2); "
        << "z-index: 1001; min-width: 320px; }\n"
        << ind(2) << ".sad-canvas { display: block; }\n"
        << ind(2) << ".sad-icon { display: inline-block; font-size: 24px; }\n"
        << ind(2) << ".sad-picker { padding: 12px; border: 1px solid #ccc; border-radius: 8px; "
        << "font-size: 16px; font-family: inherit; }\n"
        << ind(2) << ".sad-lazy-column { display: flex; flex-direction: column; overflow-y: auto; }\n"
        << ind(2) << ".sad-lazy-row { display: flex; flex-direction: row; overflow-x: auto; }\n"
        << ind(2) << ".sad-scroll-view { display: flex; flex-direction: column; overflow-y: auto; max-height: 100%; }\n"
        << ind(2) << ".sad-container { display: flex; flex-direction: column; }\n"
        << ind(2) << ".sad-surface { background: var(--sad-surface); border-radius: 4px; }\n"
        << ind(2) << ".sad-wrap { display: flex; flex-wrap: wrap; gap: 8px; }\n"
        << ind(2) << ".sad-group { display: contents; }\n";

    // ═══ v3 Data Components ═══
    out << ind(2) << ".sad-data-table { width: 100%; border-collapse: collapse; }\n"
        << ind(2) << ".sad-data-table th, .sad-data-table td { padding: 12px 16px; "
        << "border-bottom: 1px solid #E0E0E0; text-align: start; }\n"
        << ind(2) << ".sad-data-table th { font-weight: 600; background: #FAFAFA; }\n"
        << ind(2) << ".sad-data-table tr:hover { background: #F5F5F5; }\n"
        << ind(2) << ".sad-tree-view { padding: 4px 0; }\n"
        << ind(2) << ".sad-tree-view details { padding-inline-start: 16px; }\n"
        << ind(2) << ".sad-tree-view summary { cursor: pointer; padding: 4px 0; }\n"
        << ind(2) << ".sad-breadcrumb { display: flex; align-items: center; gap: 8px; font-size: 14px; }\n"
        << ind(2) << ".sad-breadcrumb-sep::before { content: '/'; color: #9E9E9E; }\n"
        << ind(2) << ".sad-pagination { display: flex; align-items: center; gap: 4px; }\n"
        << ind(2) << ".sad-pagination button { min-width: 36px; height: 36px; border: 1px solid #E0E0E0; "
        << "border-radius: 4px; background: white; cursor: pointer; }\n"
        << ind(2) << ".sad-pagination button.active { background: var(--sad-primary); color: white; border-color: var(--sad-primary); }\n"
        << ind(2) << ".sad-timeline { position: relative; padding-inline-start: 24px; }\n"
        << ind(2) << ".sad-timeline::before { content: ''; position: absolute; inset-inline-start: 8px; "
        << "top: 0; bottom: 0; width: 2px; background: #1E88E5; }\n"
        << ind(2) << ".sad-timeline-item { position: relative; padding: 8px 0 16px; }\n"
        << ind(2) << ".sad-timeline-item::before { content: ''; position: absolute; inset-inline-start: -20px; "
        << "top: 12px; width: 10px; height: 10px; border-radius: 50%; background: #1E88E5; border: 2px solid white; }\n";

    // ═══ v3 Media Components ═══
    out << ind(2) << ".sad-carousel { position: relative; overflow: hidden; border-radius: 8px; }\n"
        << ind(2) << ".sad-carousel-track { display: flex; transition: transform 0.4s ease; }\n"
        << ind(2) << ".sad-carousel-item { min-width: 100%; }\n"
        << ind(2) << ".sad-rich-text { line-height: 1.7; }\n"
        << ind(2) << ".sad-markdown { line-height: 1.7; }\n"
        << ind(2) << ".sad-code-block { background: #F5F5F5; padding: 16px; border-radius: 8px; "
        << "font-family: 'Fira Code', monospace; font-size: 14px; overflow-x: auto; white-space: pre; margin: 0; }\n"
        << ind(2) << ".sad-code-wrapper { display: flex; position: relative; border-radius: 8px; overflow: hidden; }\n"
        << ind(2) << ".sad-line-numbers { display: flex; flex-direction: column; padding: 16px 12px 16px 12px; "
        << "background: #EBEBEB; color: #999; font-family: 'Fira Code', monospace; font-size: 14px; "
        << "text-align: right; user-select: none; line-height: 1.5; min-width: 2em; }\n"
        << ind(2) << ".sad-line-numbers span { display: block; }\n"
        << ind(2) << ".sad-code-wrapper .sad-code-block { flex: 1; border-radius: 0; line-height: 1.5; }\n"
        // تلوين نحوي للغة ص
        << ind(2) << ".sad-code-block .tok-keyword { color: #D32F2F; font-weight: bold; }\n"
        << ind(2) << ".sad-code-block .tok-string { color: #2E7D32; }\n"
        << ind(2) << ".sad-code-block .tok-number { color: #1565C0; }\n"
        << ind(2) << ".sad-code-block .tok-comment { color: #9E9E9E; font-style: italic; }\n"
        << ind(2) << ".sad-code-block .tok-type { color: #00838F; }\n"
        << ind(2) << ".sad-code-block .tok-builtin { color: #6A1B9A; }\n"
        << ind(2) << ".sad-code-block .tok-operator { color: #E65100; }\n"
        // تحسين التحديد
        << ind(2) << ".sad-input::selection, .sad-code-block::selection { background: rgba(66,133,244,0.3); }\n"
        << ind(2) << ".sad-audio-player { display: flex; align-items: center; gap: 12px; padding: 8px 16px; "
        << "background: #F5F5F5; border-radius: 24px; }\n"
        << ind(2) << ".sad-image-gallery { display: grid; grid-template-columns: repeat(auto-fill, minmax(200px, 1fr)); gap: 8px; }\n";

    // ═══ v3 Advanced Controls ═══
    out << ind(2) << ".sad-stepper { display: flex; align-items: center; gap: 8px; }\n"
        << ind(2) << ".sad-stepper button { width: 36px; height: 36px; border-radius: 50%; border: 1px solid #E0E0E0; "
        << "background: white; cursor: pointer; font-size: 18px; }\n"
        << ind(2) << ".sad-segmented { display: inline-flex; background: #E0E0E0; border-radius: 8px; padding: 2px; }\n"
        << ind(2) << ".sad-segmented button { padding: 8px 16px; border: none; background: transparent; "
        << "border-radius: 6px; cursor: pointer; }\n"
        << ind(2) << ".sad-segmented button.active { background: white; box-shadow: 0 1px 3px rgba(0,0,0,0.12); }\n"
        << ind(2) << ".sad-rating { display: flex; gap: 4px; font-size: 24px; color: #FFD700; cursor: pointer; }\n"
        << ind(2) << ".sad-rating .empty { color: #E0E0E0; }\n";

    // ═══ v3 Navigation ═══
    out << ind(2) << ".sad-bottom-nav { display: flex; justify-content: space-around; align-items: center; "
        << "padding: 8px; background: white; border-top: 1px solid #E0E0E0; position: fixed; bottom: 0; "
        << "left: 0; right: 0; }\n"
        << ind(2) << ".sad-side-nav { width: 240px; height: 100vh; background: #FAFAFA; "
        << "border-inline-end: 1px solid #E0E0E0; padding: 16px; position: fixed; inset-inline-start: 0; top: 0; }\n"
        << ind(2) << ".sad-toolbar { display: flex; align-items: center; gap: 8px; padding: 8px 16px; "
        << "background: #FAFAFA; border-bottom: 1px solid #E0E0E0; }\n"
        << ind(2) << ".sad-split-view { display: flex; height: 100%; }\n"
        << ind(2) << ".sad-split-view > * { flex: 1; overflow: auto; }\n"
        << ind(2) << ".sad-split-view .divider { width: 4px; background: #E0E0E0; cursor: col-resize; flex: none; }\n"
        << ind(2) << ".sad-tooltip { position: relative; }\n"
        << ind(2) << ".sad-tooltip::after { content: attr(data-tooltip); position: absolute; bottom: 100%; "
        << "left: 50%; transform: translateX(-50%); padding: 4px 8px; background: #333; color: white; "
        << "border-radius: 4px; font-size: 12px; white-space: nowrap; opacity: 0; pointer-events: none; "
        << "transition: opacity 0.2s; }\n"
        << ind(2) << ".sad-tooltip:hover::after { opacity: 1; }\n";

    // ═══ CSS Transitions & Hover Effects ═══
    out << ind(2) << ".sad-button { transition: background 0.2s, transform 0.1s, box-shadow 0.2s; }\n"
        << ind(2) << ".sad-button:active { transform: scale(0.97); }\n"
        << ind(2) << ".sad-card { transition: box-shadow 0.3s, transform 0.2s; }\n"
        << ind(2) << ".sad-card:hover { box-shadow: 0 6px 16px rgba(0,0,0,0.15); transform: translateY(-2px); }\n"
        << ind(2) << ".sad-chip { transition: background 0.2s; cursor: pointer; }\n"
        << ind(2) << ".sad-chip:hover { background: #D0D0D0; }\n"
        << ind(2) << ".sad-input:focus { outline: none; border-color: var(--sad-primary); box-shadow: var(--sad-focus-ring); }\n"
        << ind(2) << ".sad-list-item { transition: background 0.15s; }\n"
        << ind(2) << ".sad-list-item:hover { background: #F5F5F5; }\n"
        << ind(2) << ".sad-fab:hover { box-shadow: 0 6px 12px rgba(0,0,0,0.3); transform: scale(1.05); }\n"
        << ind(2) << ".sad-fab { transition: box-shadow 0.2s, transform 0.15s; }\n";

    // ═══ CSS Animations (@keyframes) ═══
    out << ind(2) << "@keyframes sad-shimmer { 0% { background-position: -200% 0; } 100% { background-position: 200% 0; } }\n"
        << ind(2) << ".sad-skeleton { background: linear-gradient(90deg, #E0E0E0 25%, #F5F5F5 50%, #E0E0E0 75%); "
        << "background-size: 200% 100%; animation: sad-shimmer 1.5s ease-in-out infinite; border-radius: 4px; }\n"
        << ind(2) << ".sad-shimmer { background: linear-gradient(90deg, #F0F0F0 25%, #FAFAFA 50%, #F0F0F0 75%); "
        << "background-size: 200% 100%; animation: sad-shimmer 1.2s ease-in-out infinite; }\n"
        << ind(2) << "@keyframes sad-pulse { 0%,100% { opacity: 1; } 50% { opacity: 0.5; } }\n"
        << ind(2) << "@keyframes sad-fade-in { from { opacity: 0; } to { opacity: 1; } }\n"
        << ind(2) << "@keyframes sad-slide-up { from { transform: translateY(16px); opacity: 0; } to { transform: translateY(0); opacity: 1; } }\n"
        << ind(2) << "@keyframes sad-slide-in-right { from { transform: translateX(100%); } to { transform: translateX(0); } }\n"
        << ind(2) << ".sad-animate-fade { animation: sad-fade-in 0.3s ease; }\n"
        << ind(2) << ".sad-animate-slide { animation: sad-slide-up 0.4s ease; }\n"
        << ind(2) << ".sad-snackbar { animation: sad-slide-up 0.3s ease; }\n"
        << ind(2) << ".sad-dialog { animation: sad-fade-in 0.25s ease; }\n"
        << ind(2) << ".sad-alert { animation: sad-fade-in 0.25s ease; }\n"
        << ind(2) << ".sad-drawer { animation: sad-slide-in-right 0.3s ease; }\n"
        << ind(2) << ".sad-bottom-sheet { animation: sad-slide-up 0.35s ease; }\n";

    // ═══ Responsive Design (@media queries) ═══
    out << ind(2) << "@media (max-width: 768px) {\n"
        << ind(3) << ".sad-row { flex-direction: column; }\n"
        << ind(3) << ".sad-side-nav { width: 100%; height: auto; position: relative; }\n"
        << ind(3) << ".sad-split-view { flex-direction: column; }\n"
        << ind(3) << ".sad-image-gallery { grid-template-columns: repeat(auto-fill, minmax(140px, 1fr)); }\n"
        << ind(3) << ".sad-data-table { font-size: 13px; }\n"
        << ind(3) << ".sad-data-table th, .sad-data-table td { padding: 8px; }\n"
        << ind(2) << "}\n"
        << ind(2) << "@media (max-width: 480px) {\n"
        << ind(3) << ".sad-dialog, .sad-alert { min-width: auto; width: 90vw; }\n"
        << ind(3) << ".sad-appbar { font-size: 16px; padding: 8px 12px; }\n"
        << ind(3) << ".sad-drawer { width: 85vw; }\n"
        << ind(2) << "}\n";

    // ═══ Dark Mode Element Overrides ═══
    out << ind(2) << "@media (prefers-color-scheme: dark) {\n"
        << ind(3) << "body { background: #121212; color: #E0E0E0; }\n"
        << ind(3) << ".sad-card { background: #1E1E1E; box-shadow: 0 2px 4px rgba(0,0,0,0.3); }\n"
        << ind(3) << ".sad-card:hover { box-shadow: 0 6px 16px rgba(0,0,0,0.4); }\n"
        << ind(3) << ".sad-button { background: #64B5F6; color: #121212; }\n"
        << ind(3) << ".sad-button:hover { background: #42A5F5; }\n"
        << ind(3) << ".sad-input { background: #2C2C2C; border-color: #444; color: #E0E0E0; }\n"
        << ind(3) << ".sad-input:focus { border-color: #64B5F6; box-shadow: 0 0 0 3px rgba(100,181,246,0.2); }\n"
        << ind(3) << ".sad-surface { background: #1E1E1E; }\n"
        << ind(3) << ".sad-appbar { background: #1E1E1E; }\n"
        << ind(3) << ".sad-dialog, .sad-alert { background: #2C2C2C; }\n"
        << ind(3) << ".sad-drawer { background: #1E1E1E; }\n"
        << ind(3) << ".sad-bottom-sheet { background: #2C2C2C; }\n"
        << ind(3) << ".sad-snackbar { background: #424242; }\n"
        << ind(3) << ".sad-chip { background: #333; color: #E0E0E0; }\n"
        << ind(3) << ".sad-chip:hover { background: #444; }\n"
        << ind(3) << ".sad-list-item { border-color: #333; }\n"
        << ind(3) << ".sad-list-item:hover { background: #2C2C2C; }\n"
        << ind(3) << ".sad-divider { border-color: #333; }\n"
        << ind(3) << ".sad-data-table th { background: #2C2C2C; }\n"
        << ind(3) << ".sad-data-table td { border-color: #333; }\n"
        << ind(3) << ".sad-data-table tr:hover { background: #2C2C2C; }\n"
        << ind(3) << ".sad-code-block { background: #2C2C2C; color: #E0E0E0; }\n"
        << ind(3) << ".sad-line-numbers { background: #252525; color: #666; }\n"
        << ind(3) << ".sad-code-block .tok-keyword { color: #FF7043; }\n"
        << ind(3) << ".sad-code-block .tok-string { color: #A5D6A7; }\n"
        << ind(3) << ".sad-code-block .tok-number { color: #90CAF9; }\n"
        << ind(3) << ".sad-code-block .tok-comment { color: #757575; }\n"
        << ind(3) << ".sad-code-block .tok-type { color: #4DD0E1; }\n"
        << ind(3) << ".sad-code-block .tok-builtin { color: #CE93D8; }\n"
        << ind(3) << ".sad-code-block .tok-operator { color: #FFB74D; }\n"
        << ind(3) << ".sad-input::selection, .sad-code-block::selection { background: rgba(100,181,246,0.3); }\n"
        << ind(3) << ".sad-audio-player { background: #2C2C2C; }\n"
        << ind(3) << ".sad-bottom-nav { background: #1E1E1E; border-color: #333; }\n"
        << ind(3) << ".sad-side-nav { background: #1E1E1E; border-color: #333; }\n"
        << ind(3) << ".sad-toolbar { background: #1E1E1E; border-color: #333; }\n"
        << ind(3) << ".sad-segmented { background: #333; }\n"
        << ind(3) << ".sad-segmented button.active { background: #444; }\n"
        << ind(3) << ".sad-skeleton { background: linear-gradient(90deg, #333 25%, #444 50%, #333 75%); "
        << "background-size: 200% 100%; }\n"
        << ind(2) << "}\n";

    // ═══ Print Styles ═══
    out << ind(2) << "@media print {\n"
        << ind(3) << "body { background: white !important; color: black !important; font-size: 12pt; }\n"
        << ind(3) << ".sad-appbar { background: none !important; color: black !important; border-bottom: 2px solid #333; position: static; }\n"
        << ind(3) << ".sad-card { box-shadow: none !important; border: 1px solid #ccc; break-inside: avoid; }\n"
        << ind(3) << ".sad-button { border: 1px solid #333 !important; background: white !important; color: black !important; }\n"
        << ind(3) << ".sad-fab, .sad-snackbar, .sad-drawer, .sad-bottom-sheet, .sad-dialog, .sad-alert { display: none !important; }\n"
        << ind(3) << ".sad-bottom-nav, .sad-side-nav { display: none !important; }\n"
        << ind(3) << ".sad-input, .sad-slider, .sad-toggle, .sad-picker { border: 1px solid #999; }\n"
        << ind(3) << ".sad-progress { -webkit-appearance: none; border: 1px solid #ccc; }\n"
        << ind(3) << "* { transition: none !important; animation: none !important; }\n"
        << ind(3) << "a[href]::after { content: ' (' attr(href) ')'; font-size: 0.8em; }\n"
        << ind(2) << "}\n";

    // ═══ Accessibility ═══
    out << ind(2) << ".sad-skip-link { position: absolute; left: -9999px; top: auto; width: 1px; height: 1px; "
        << "overflow: hidden; z-index: 9999; }\n"
        << ind(2) << ".sad-skip-link:focus { position: fixed; top: 0; left: 0; width: auto; height: auto; "
        << "padding: 12px 24px; background: var(--sad-primary); color: white; font-size: 16px; z-index: 9999; }\n"
        << ind(2) << "*:focus-visible { outline: 2px solid var(--sad-primary); outline-offset: 2px; }\n"
        << ind(2) << ".sad-button:focus-visible { box-shadow: var(--sad-focus-ring); }\n"
        << ind(2) << ".sad-sr-only { position: absolute; width: 1px; height: 1px; padding: 0; margin: -1px; "
        << "overflow: hidden; clip: rect(0,0,0,0); white-space: nowrap; border: 0; }\n";

    // ═══ Reduced Motion ═══
    out << ind(2) << "@media (prefers-reduced-motion: reduce) {\n"
        << ind(3) << "*, *::before, *::after { animation-duration: 0.01ms !important; "
        << "animation-iteration-count: 1 !important; transition-duration: 0.01ms !important; scroll-behavior: auto !important; }\n"
        << ind(2) << "}\n";

    // ═══ Scrollbar Styling ═══
    out << ind(2) << ".sad-lazy-column::-webkit-scrollbar, .sad-lazy-row::-webkit-scrollbar, .sad-scroll-view::-webkit-scrollbar { width: 6px; height: 6px; }\n"
        << ind(2) << ".sad-lazy-column::-webkit-scrollbar-thumb, .sad-lazy-row::-webkit-scrollbar-thumb, .sad-scroll-view::-webkit-scrollbar-thumb { "
        << "background: #BDBDBD; border-radius: 3px; }\n"
        << ind(2) << ".sad-lazy-column::-webkit-scrollbar-thumb:hover, .sad-lazy-row::-webkit-scrollbar-thumb:hover, .sad-scroll-view::-webkit-scrollbar-thumb:hover { "
        << "background: #9E9E9E; }\n\n";

    // ═══ Advanced Animations ═══
    out << ind(2) << ".sad-hero { view-transition-name: var(--hero-tag); }\n"
        << ind(2) << "::view-transition-old(*), ::view-transition-new(*) { animation-duration: 0.35s; }\n\n"
        << ind(2) << "@keyframes sad-slide-in-right { from { transform: translateX(100%); opacity: 0; } to { transform: translateX(0); opacity: 1; } }\n"
        << ind(2) << "@keyframes sad-slide-in-left { from { transform: translateX(-100%); opacity: 0; } to { transform: translateX(0); opacity: 1; } }\n"
        << ind(2) << "@keyframes sad-slide-in-up { from { transform: translateY(100%); opacity: 0; } to { transform: translateY(0); opacity: 1; } }\n"
        << ind(2) << "@keyframes sad-slide-in-down { from { transform: translateY(-100%); opacity: 0; } to { transform: translateY(0); opacity: 1; } }\n"
        << ind(2) << "@keyframes sad-fade-in { from { opacity: 0; } to { opacity: 1; } }\n"
        << ind(2) << "@keyframes sad-fade-out { from { opacity: 1; } to { opacity: 0; } }\n"
        << ind(2) << "@keyframes sad-scale-in { from { transform: scale(0.8); opacity: 0; } to { transform: scale(1); opacity: 1; } }\n"
        << ind(2) << "@keyframes sad-scale-out { from { transform: scale(1); opacity: 1; } to { transform: scale(0.8); opacity: 0; } }\n"
        << ind(2) << "@keyframes sad-rotate-in { from { transform: rotate(-90deg) scale(0); opacity: 0; } to { transform: rotate(0) scale(1); opacity: 1; } }\n"
        << ind(2) << "@keyframes sad-bounce-in { "
        << "0% { transform: scale(0); } "
        << "50% { transform: scale(1.15); } "
        << "70% { transform: scale(0.95); } "
        << "100% { transform: scale(1); } }\n"
        << ind(2) << "@keyframes sad-elastic-in { "
        << "0% { transform: scale(0); } "
        << "55% { transform: scale(1.1); } "
        << "75% { transform: scale(0.98); } "
        << "100% { transform: scale(1); } }\n\n"
        << ind(2) << ".sad-animate-slide-right { animation: sad-slide-in-right 0.35s ease-out; }\n"
        << ind(2) << ".sad-animate-slide-left { animation: sad-slide-in-left 0.35s ease-out; }\n"
        << ind(2) << ".sad-animate-slide-up { animation: sad-slide-in-up 0.35s ease-out; }\n"
        << ind(2) << ".sad-animate-slide-down { animation: sad-slide-in-down 0.35s ease-out; }\n"
        << ind(2) << ".sad-animate-fade { animation: sad-fade-in 0.3s ease; }\n"
        << ind(2) << ".sad-animate-scale { animation: sad-scale-in 0.3s ease-out; }\n"
        << ind(2) << ".sad-animate-bounce { animation: sad-bounce-in 0.5s ease; }\n"
        << ind(2) << ".sad-animate-elastic { animation: sad-elastic-in 0.6s ease; }\n"
        << ind(2) << ".sad-animate-rotate { animation: sad-rotate-in 0.4s ease-out; }\n\n";

    // ═══ Staggered Animation ═══
    out << ind(2) << ".sad-stagger > * { opacity: 0; animation: sad-fade-in 0.3s ease forwards; }\n"
        << ind(2) << ".sad-stagger > *:nth-child(1) { animation-delay: 0.05s; }\n"
        << ind(2) << ".sad-stagger > *:nth-child(2) { animation-delay: 0.1s; }\n"
        << ind(2) << ".sad-stagger > *:nth-child(3) { animation-delay: 0.15s; }\n"
        << ind(2) << ".sad-stagger > *:nth-child(4) { animation-delay: 0.2s; }\n"
        << ind(2) << ".sad-stagger > *:nth-child(5) { animation-delay: 0.25s; }\n"
        << ind(2) << ".sad-stagger > *:nth-child(6) { animation-delay: 0.3s; }\n"
        << ind(2) << ".sad-stagger > *:nth-child(7) { animation-delay: 0.35s; }\n"
        << ind(2) << ".sad-stagger > *:nth-child(8) { animation-delay: 0.4s; }\n"
        << ind(2) << ".sad-stagger > *:nth-child(n+9) { animation-delay: 0.45s; }\n\n"
        << ind(2) << ".sad-on-appear { opacity: 0; transform: translateY(20px); transition: opacity 0.4s ease, transform 0.4s ease; }\n"
        << ind(2) << ".sad-on-appear.visible { opacity: 1; transform: translateY(0); }\n\n"
        << ind(2) << "@keyframes sad-pulse { 0%,100% { transform: scale(1); } 50% { transform: scale(1.05); } }\n"
        << ind(2) << "@keyframes sad-shake { 0%,100% { transform: translateX(0); } 25% { transform: translateX(-8px); } 75% { transform: translateX(8px); } }\n"
        << ind(2) << "@keyframes sad-wobble { 0%,100% { transform: rotate(0); } 25% { transform: rotate(-3deg); } 75% { transform: rotate(3deg); } }\n"
        << ind(2) << ".sad-pulse { animation: sad-pulse 2s infinite; }\n"
        << ind(2) << ".sad-shake { animation: sad-shake 0.5s ease; }\n"
        << ind(2) << ".sad-wobble { animation: sad-wobble 0.5s ease; }\n\n";

    // ═══ Responsive Layout System ═══
    out << ind(2) << ".sad-container { width: 100%; max-width: 1200px; margin: 0 auto; padding: 0 16px; }\n"
        << ind(2) << ".sad-grid-responsive { display: grid; grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); gap: 16px; }\n"
        << ind(2) << ".sad-hide-mobile { }\n"
        << ind(2) << ".sad-hide-desktop { display: none; }\n"
        << ind(2) << "@media (max-width: 768px) {\n"
        << ind(3) << ".sad-hide-mobile { display: none !important; }\n"
        << ind(3) << ".sad-hide-desktop { display: block; }\n"
        << ind(3) << ".sad-container { padding: 0 8px; }\n"
        << ind(3) << ".sad-appbar { font-size: 16px; padding: 12px; }\n"
        << ind(3) << ".sad-split-view { flex-direction: column !important; }\n"
        << ind(3) << ".sad-side-nav { display: none; }\n"
        << ind(3) << ".sad-grid-responsive { grid-template-columns: 1fr; }\n"
        << ind(2) << "}\n"
        << ind(2) << "@media (max-width: 480px) {\n"
        << ind(3) << ".sad-card { margin: 4px; padding: 12px; }\n"
        << ind(3) << ".sad-button { width: 100%; }\n"
        << ind(3) << ".sad-fab { width: 48px; height: 48px; font-size: 20px; }\n"
        << ind(2) << "}\n"
        << ind(2) << "@media (min-width: 1200px) {\n"
        << ind(3) << ".sad-container { max-width: 1400px; }\n"
        << ind(2) << "}\n\n";

    // ═══ Form Validation Styles ═══
    out << ind(2) << ".sad-form-group { margin-bottom: 16px; }\n"
        << ind(2) << ".sad-form-label { display: block; margin-bottom: 4px; font-weight: 600; color: var(--sad-text); font-size: 14px; }\n"
        << ind(2) << ".sad-form-error { color: var(--sad-error); font-size: 12px; margin-top: 4px; display: none; }\n"
        << ind(2) << ".sad-form-group.has-error .sad-input { border-color: var(--sad-error); }\n"
        << ind(2) << ".sad-form-group.has-error .sad-form-error { display: block; }\n"
        << ind(2) << ".sad-form-group.has-success .sad-input { border-color: var(--sad-success); }\n"
        << ind(2) << ".sad-input:invalid { border-color: var(--sad-error); }\n"
        << ind(2) << ".sad-input:valid { border-color: var(--sad-success); }\n"
        << ind(2) << ".sad-input::placeholder { color: var(--sad-text-secondary); opacity: 0.6; }\n\n";

    // ═══ Drag & Drop ═══
    out << ind(2) << ".sad-draggable { cursor: grab; user-select: none; }\n"
        << ind(2) << ".sad-draggable:active { cursor: grabbing; opacity: 0.7; }\n"
        << ind(2) << ".sad-drop-zone { transition: background 0.2s, border-color 0.2s; border: 2px dashed transparent; }\n"
        << ind(2) << ".sad-drop-zone.drag-over { border-color: var(--sad-primary); background: rgba(30,136,229,0.05); }\n\n";

    // ═══ Glassmorphism & Neumorphism ═══
    out << ind(2) << ".sad-glass { background: rgba(255,255,255,0.15); backdrop-filter: blur(12px); "
        << "-webkit-backdrop-filter: blur(12px); border: 1px solid rgba(255,255,255,0.2); }\n"
        << ind(2) << ".sad-neumorphic { background: var(--sad-bg); border-radius: var(--sad-radius); "
        << "box-shadow: 8px 8px 16px rgba(0,0,0,0.1), -8px -8px 16px rgba(255,255,255,0.7); }\n"
        << ind(2) << ".sad-neumorphic-inset { box-shadow: inset 4px 4px 8px rgba(0,0,0,0.1), inset -4px -4px 8px rgba(255,255,255,0.7); }\n";
}

} // namespace web
} // namespace ui
} // namespace sad
