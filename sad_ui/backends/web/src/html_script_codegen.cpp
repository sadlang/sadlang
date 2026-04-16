/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: html_script_codegen.cpp
 * المسار: sad_ui/backends/web/src/html_script_codegen.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * توليد JavaScript: إدارة الحالة، الإيماءات، السحب والإفلات، التحقق من النماذج.
 * جزء من تقسيم html_codegen.cpp الأصلي.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/web/html_codegen.h"

namespace sad {
namespace ui {
namespace web {

void HtmlCodegen::generateScript(std::ostringstream& out, const IRModule& module) {
    out << "<script>\n";

    // ═══════════════════════════════════════════════════════════
    // State Management — نظام إدارة الحالة التفاعلي
    // ═══════════════════════════════════════════════════════════
    out << ind(1) << "// === نظام الحالة التفاعلي ===\n";
    out << ind(1) << "var __state = {};\n";
    out << ind(1) << "var __listeners = {};\n\n";

    // تعريف متغيرات الحالة
    for (const auto& [name, type] : module.stateDefinitions) {
        std::string defaultVal = "0";
        if (type == "\xd9\x86\xd8\xb5") defaultVal = "''";
        else if (type == "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a") defaultVal = "false";
        else if (type == "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9") defaultVal = "[]";
        else if (type == "\xd8\xae\xd8\xb1\xd9\x8a\xd8\xb7\xd8\xa9") defaultVal = "{}";

        out << ind(1) << "__state['" << name << "'] = " << defaultVal << ";\n";
    }

    // دالة setState — تحديث الحالة وإعادة العرض
    out << "\n"
        << ind(1) << "function setState(key, value) {\n"
        << ind(2) << "var old = __state[key];\n"
        << ind(2) << "__state[key] = value;\n"
        << ind(2) << "if (old !== value) {\n"
        << ind(3) << "if (__listeners[key]) {\n"
        << ind(4) << "__listeners[key].forEach(function(fn) { fn(value, old); });\n"
        << ind(3) << "}\n"
        << ind(3) << "update();\n"
        << ind(2) << "}\n"
        << ind(1) << "}\n\n";

    // دالة getState
    out << ind(1) << "function getState(key, def) {\n"
        << ind(2) << "return key in __state ? __state[key] : (def !== undefined ? def : null);\n"
        << ind(1) << "}\n\n";

    // دالة watch — مراقبة مفتاح حالة معين
    out << ind(1) << "function watch(key, fn) {\n"
        << ind(2) << "if (!__listeners[key]) __listeners[key] = [];\n"
        << ind(2) << "__listeners[key].push(fn);\n"
        << ind(1) << "}\n\n";

    // دالة update — إعادة بناء UI بناءً على الحالة
    out << ind(1) << "// إعادة بناء عناصر UI عند تغيير الحالة\n"
        << ind(1) << "function update() {\n"
        << ind(2) << "// تحديث عناصر التبديل\n"
        << ind(2) << "document.querySelectorAll('[data-state-key]').forEach(function(el) {\n"
        << ind(3) << "var key = el.dataset.stateKey;\n"
        << ind(3) << "if (key && key in __state) {\n"
        << ind(4) << "var val = __state[key];\n"
        << ind(4) << "if (el.tagName === 'INPUT' && el.type === 'checkbox') {\n"
        << ind(5) << "el.checked = !!val;\n"
        << ind(4) << "} else if (el.tagName === 'INPUT' || el.tagName === 'TEXTAREA') {\n"
        << ind(5) << "el.value = val;\n"
        << ind(4) << "} else if (el.tagName === 'PROGRESS') {\n"
        << ind(5) << "el.value = val;\n"
        << ind(4) << "} else {\n"
        << ind(5) << "el.textContent = val;\n"
        << ind(4) << "}\n"
        << ind(3) << "}\n"
        << ind(2) << "});\n"
        << ind(2) << "// تحديث الأقسام المشروطة\n"
        << ind(2) << "document.querySelectorAll('[data-state-show]').forEach(function(el) {\n"
        << ind(3) << "var key = el.dataset.stateShow;\n"
        << ind(3) << "if (key) el.style.display = __state[key] ? '' : 'none';\n"
        << ind(2) << "});\n"
        << ind(1) << "}\n\n";

    // دوال تفاعلية للأزرار (ripple effect)
    out << ind(1) << "// تأثير الموجة على الأزرار\n"
        << ind(1) << "document.addEventListener('DOMContentLoaded', function() {\n"
        << ind(2) << "document.querySelectorAll('.sad-button').forEach(function(btn) {\n"
        << ind(3) << "btn.addEventListener('click', function(e) {\n"
        << ind(4) << "var ripple = document.createElement('span');\n"
        << ind(4) << "ripple.style.cssText = 'position:absolute;border-radius:50%;background:rgba(255,255,255,0.3);'"
        << "+'width:20px;height:20px;transform:scale(0);animation:sad-ripple 0.6s;';\n"
        << ind(4) << "var rect = this.getBoundingClientRect();\n"
        << ind(4) << "ripple.style.left = (e.clientX - rect.left - 10) + 'px';\n"
        << ind(4) << "ripple.style.top = (e.clientY - rect.top - 10) + 'px';\n"
        << ind(4) << "this.style.position = 'relative';\n"
        << ind(4) << "this.style.overflow = 'hidden';\n"
        << ind(4) << "this.appendChild(ripple);\n"
        << ind(4) << "setTimeout(function() { ripple.remove(); }, 600);\n"
        << ind(3) << "});\n"
        << ind(2) << "});\n\n"

        // Stepper
        << ind(2) << "// Stepper\n"
        << ind(2) << "document.querySelectorAll('.sad-stepper').forEach(function(s) {\n"
        << ind(3) << "var up = s.querySelector('.up'), dn = s.querySelector('.down');\n"
        << ind(3) << "var val = s.querySelector('.sad-stepper-value');\n"
        << ind(3) << "if(up && val) up.addEventListener('click', function(){\n"
        << ind(4) << "val.textContent = parseInt(val.textContent || '0') + 1;\n"
        << ind(3) << "});\n"
        << ind(3) << "if(dn && val) dn.addEventListener('click', function(){\n"
        << ind(4) << "val.textContent = Math.max(0, parseInt(val.textContent || '0') - 1);\n"
        << ind(3) << "});\n"
        << ind(2) << "});\n\n"

        // Rating
        << ind(2) << "// Rating stars\n"
        << ind(2) << "document.querySelectorAll('.sad-rating').forEach(function(r) {\n"
        << ind(3) << "r.querySelectorAll('.sad-star').forEach(function(star) {\n"
        << ind(4) << "star.addEventListener('click', function() {\n"
        << ind(5) << "var val = parseInt(this.dataset.value);\n"
        << ind(5) << "r.dataset.rating = val;\n"
        << ind(5) << "r.querySelectorAll('.sad-star').forEach(function(s) {\n"
        << ind(6) << "s.style.color = parseInt(s.dataset.value) <= val ? '#FFD700' : '#E0E0E0';\n"
        << ind(5) << "});\n"
        << ind(4) << "});\n"
        << ind(3) << "});\n"
        << ind(2) << "});\n\n"

        // Tabs
        << ind(2) << "// Tabs\n"
        << ind(2) << "document.querySelectorAll('.sad-tabs').forEach(function(tabs) {\n"
        << ind(3) << "tabs.querySelectorAll('.sad-tab-btn').forEach(function(btn, i) {\n"
        << ind(4) << "btn.addEventListener('click', function() {\n"
        << ind(5) << "tabs.querySelectorAll('.sad-tab-btn').forEach(function(b) { b.classList.remove('active'); });\n"
        << ind(5) << "tabs.querySelectorAll('.sad-tab-panel').forEach(function(p) { p.style.display = 'none'; });\n"
        << ind(5) << "btn.classList.add('active');\n"
        << ind(5) << "var panels = tabs.querySelectorAll('.sad-tab-panel');\n"
        << ind(5) << "if (panels[i]) panels[i].style.display = '';\n"
        << ind(4) << "});\n"
        << ind(3) << "});\n"
        << ind(2) << "});\n\n"

        // SnackBar auto-hide
        << ind(2) << "// SnackBar auto-hide\n"
        << ind(2) << "document.querySelectorAll('.sad-snackbar').forEach(function(s) {\n"
        << ind(3) << "setTimeout(function() { s.style.opacity = '0'; s.style.transition = 'opacity 0.4s'; }, 3000);\n"
        << ind(3) << "setTimeout(function() { s.style.display = 'none'; }, 3400);\n"
        << ind(2) << "});\n\n"

        // Expandable/Collapsible
        << ind(2) << "// Expandable details\n"
        << ind(2) << "document.querySelectorAll('details.sad-expandable, details.sad-collapsible').forEach(function(d) {\n"
        << ind(3) << "d.addEventListener('toggle', function() {\n"
        << ind(4) << "var icon = this.querySelector('.toggle-icon');\n"
        << ind(4) << "if(icon) icon.textContent = this.open ? '\\u25BC' : '\\u25B6';\n"
        << ind(3) << "});\n"
        << ind(2) << "});\n\n"

        // انيميشن ripple keyframe إضافة
        << ind(2) << "// Ripple keyframe\n"
        << ind(2) << "var style = document.createElement('style');\n"
        << ind(2) << "style.textContent = '@keyframes sad-ripple { to { transform: scale(4); opacity: 0; } }';\n"
        << ind(2) << "document.head.appendChild(style);\n\n"

        // ═══════════════════════════════════════════════════════════
        // نظام الإيماءات المتقدم — Touch Gestures System
        // ═══════════════════════════════════════════════════════════
        << ind(2) << "// === نظام الإيماءات ===\n"
        << ind(2) << "(function() {\n"
        << ind(3) << "var touchStart = null, touchStartTime = 0, longPressTimer = null;\n"
        << ind(3) << "var pinchStartDist = 0, lastScale = 1;\n\n"

        // Touch Start
        << ind(3) << "document.addEventListener('touchstart', function(e) {\n"
        << ind(4) << "if (e.touches.length === 1) {\n"
        << ind(5) << "touchStart = {x: e.touches[0].clientX, y: e.touches[0].clientY};\n"
        << ind(5) << "touchStartTime = Date.now();\n"
        << ind(5) << "longPressTimer = setTimeout(function() {\n"
        << ind(6) << "var el = document.elementFromPoint(touchStart.x, touchStart.y);\n"
        << ind(6) << "if (el) el.dispatchEvent(new CustomEvent('sad-longpress', {bubbles: true}));\n"
        << ind(5) << "}, 500);\n"
        << ind(4) << "} else if (e.touches.length === 2) {\n"
        << ind(5) << "clearTimeout(longPressTimer);\n"
        << ind(5) << "var dx = e.touches[0].clientX - e.touches[1].clientX;\n"
        << ind(5) << "var dy = e.touches[0].clientY - e.touches[1].clientY;\n"
        << ind(5) << "pinchStartDist = Math.sqrt(dx*dx + dy*dy);\n"
        << ind(5) << "lastScale = 1;\n"
        << ind(4) << "}\n"
        << ind(3) << "}, {passive: true});\n\n"

        // Touch Move (Pinch)
        << ind(3) << "document.addEventListener('touchmove', function(e) {\n"
        << ind(4) << "clearTimeout(longPressTimer);\n"
        << ind(4) << "if (e.touches.length === 2 && pinchStartDist > 0) {\n"
        << ind(5) << "var dx = e.touches[0].clientX - e.touches[1].clientX;\n"
        << ind(5) << "var dy = e.touches[0].clientY - e.touches[1].clientY;\n"
        << ind(5) << "var dist = Math.sqrt(dx*dx + dy*dy);\n"
        << ind(5) << "var scale = dist / pinchStartDist;\n"
        << ind(5) << "if (Math.abs(scale - lastScale) > 0.01) {\n"
        << ind(6) << "lastScale = scale;\n"
        << ind(6) << "var cx = (e.touches[0].clientX + e.touches[1].clientX) / 2;\n"
        << ind(6) << "var cy = (e.touches[0].clientY + e.touches[1].clientY) / 2;\n"
        << ind(6) << "var el = document.elementFromPoint(cx, cy);\n"
        << ind(6) << "if (el) el.dispatchEvent(new CustomEvent('sad-pinch', {bubbles: true, detail: {scale: scale}}));\n"
        << ind(5) << "}\n"
        << ind(4) << "}\n"
        << ind(3) << "}, {passive: true});\n\n"

        // Touch End (Swipe + Double Tap)
        << ind(3) << "var lastTapTime = 0;\n"
        << ind(3) << "document.addEventListener('touchend', function(e) {\n"
        << ind(4) << "clearTimeout(longPressTimer);\n"
        << ind(4) << "if (!touchStart) return;\n"
        << ind(4) << "var duration = Date.now() - touchStartTime;\n"
        << ind(4) << "var changedTouch = e.changedTouches[0];\n"
        << ind(4) << "if (!changedTouch) return;\n"
        << ind(4) << "var dx = changedTouch.clientX - touchStart.x;\n"
        << ind(4) << "var dy = changedTouch.clientY - touchStart.y;\n"
        << ind(4) << "var absDx = Math.abs(dx), absDy = Math.abs(dy);\n"
        << ind(4) << "var el = document.elementFromPoint(changedTouch.clientX, changedTouch.clientY);\n\n"
        << ind(4) << "if (duration < 300 && (absDx > 80 || absDy > 80)) {\n"
        << ind(5) << "var dir = absDx > absDy ? (dx > 0 ? 'right' : 'left') : (dy > 0 ? 'down' : 'up');\n"
        << ind(5) << "if (el) el.dispatchEvent(new CustomEvent('sad-swipe', {bubbles: true, detail: {direction: dir, dx: dx, dy: dy}}));\n"
        << ind(4) << "} else if (duration < 200 && absDx < 20 && absDy < 20) {\n"
        << ind(5) << "var now = Date.now();\n"
        << ind(5) << "if (now - lastTapTime < 300) {\n"
        << ind(6) << "if (el) el.dispatchEvent(new CustomEvent('sad-doubletap', {bubbles: true}));\n"
        << ind(6) << "lastTapTime = 0;\n"
        << ind(5) << "} else { lastTapTime = now; }\n"
        << ind(4) << "}\n"
        << ind(4) << "touchStart = null;\n"
        << ind(4) << "pinchStartDist = 0;\n"
        << ind(3) << "});\n"
        << ind(2) << "})();\n"

        << ind(1) << "});\n\n";

    // دوال helper عامة
    out << ind(1) << "function stepUp(btn) {\n"
        << ind(2) << "var v = btn.closest('.sad-stepper').querySelector('.sad-stepper-value');\n"
        << ind(2) << "if(v) v.textContent = parseInt(v.textContent || '0') + 1;\n"
        << ind(1) << "}\n"
        << ind(1) << "function stepDown(btn) {\n"
        << ind(2) << "var v = btn.closest('.sad-stepper').querySelector('.sad-stepper-value');\n"
        << ind(2) << "if(v) v.textContent = Math.max(0, parseInt(v.textContent || '0') - 1);\n"
        << ind(1) << "}\n\n"

        // ═══ Intersection Observer (Scroll Animations) ═══
        << ind(1) << "// Scroll-triggered animations\n"
        << ind(1) << "if ('IntersectionObserver' in window) {\n"
        << ind(2) << "var scrollObserver = new IntersectionObserver(function(entries) {\n"
        << ind(3) << "entries.forEach(function(entry) {\n"
        << ind(4) << "if (entry.isIntersecting) {\n"
        << ind(5) << "entry.target.classList.add('visible');\n"
        << ind(5) << "scrollObserver.unobserve(entry.target);\n"
        << ind(4) << "}\n"
        << ind(3) << "});\n"
        << ind(2) << "}, { threshold: 0.1 });\n"
        << ind(2) << "document.querySelectorAll('.sad-on-appear').forEach(function(el) { scrollObserver.observe(el); });\n"
        << ind(1) << "}\n\n"

        // ═══ Drag & Drop System ═══
        << ind(1) << "// Drag & Drop\n"
        << ind(1) << "(function() {\n"
        << ind(2) << "document.querySelectorAll('.sad-draggable').forEach(function(el) {\n"
        << ind(3) << "el.setAttribute('draggable', 'true');\n"
        << ind(3) << "el.addEventListener('dragstart', function(e) {\n"
        << ind(4) << "e.dataTransfer.setData('text/plain', el.dataset.dragId || '');\n"
        << ind(4) << "el.classList.add('dragging');\n"
        << ind(3) << "});\n"
        << ind(3) << "el.addEventListener('dragend', function() { el.classList.remove('dragging'); });\n"
        << ind(2) << "});\n"
        << ind(2) << "document.querySelectorAll('.sad-drop-zone').forEach(function(zone) {\n"
        << ind(3) << "zone.addEventListener('dragover', function(e) { e.preventDefault(); zone.classList.add('drag-over'); });\n"
        << ind(3) << "zone.addEventListener('dragleave', function() { zone.classList.remove('drag-over'); });\n"
        << ind(3) << "zone.addEventListener('drop', function(e) {\n"
        << ind(4) << "e.preventDefault();\n"
        << ind(4) << "zone.classList.remove('drag-over');\n"
        << ind(4) << "var data = e.dataTransfer.getData('text/plain');\n"
        << ind(4) << "zone.dispatchEvent(new CustomEvent('sad-drop', {bubbles: true, detail: {id: data}}));\n"
        << ind(3) << "});\n"
        << ind(2) << "});\n"
        << ind(1) << "})();\n\n"

        // ═══ Form Validation ═══
        << ind(1) << "// Form Validation\n"
        << ind(1) << "function validateField(input, rules) {\n"
        << ind(2) << "var group = input.closest('.sad-form-group');\n"
        << ind(2) << "if (!group) return true;\n"
        << ind(2) << "var val = input.value;\n"
        << ind(2) << "var error = group.querySelector('.sad-form-error');\n"
        << ind(2) << "var valid = true;\n"
        << ind(2) << "if (rules.required && !val.trim()) { valid = false; if(error) error.textContent = rules.requiredMsg || 'هذا الحقل مطلوب'; }\n"
        << ind(2) << "else if (rules.minLength && val.length < rules.minLength) { valid = false; "
        << "if(error) error.textContent = 'الحد الأدنى ' + rules.minLength + ' أحرف'; }\n"
        << ind(2) << "else if (rules.maxLength && val.length > rules.maxLength) { valid = false; "
        << "if(error) error.textContent = 'الحد الأقصى ' + rules.maxLength + ' أحرف'; }\n"
        << ind(2) << "else if (rules.pattern && !new RegExp(rules.pattern).test(val)) { valid = false; "
        << "if(error) error.textContent = rules.patternMsg || 'صيغة غير صحيحة'; }\n"
        << ind(2) << "else if (rules.email && !/^[^\\s@]+@[^\\s@]+\\.[^\\s@]+$/.test(val)) { valid = false; "
        << "if(error) error.textContent = 'بريد إلكتروني غير صحيح'; }\n"
        << ind(2) << "group.classList.toggle('has-error', !valid);\n"
        << ind(2) << "group.classList.toggle('has-success', valid && val.length > 0);\n"
        << ind(2) << "return valid;\n"
        << ind(1) << "}\n"
        << ind(1) << "function validateForm(formEl) {\n"
        << ind(2) << "var inputs = formEl.querySelectorAll('.sad-input[data-rules]');\n"
        << ind(2) << "var allValid = true;\n"
        << ind(2) << "inputs.forEach(function(input) {\n"
        << ind(3) << "try { var rules = JSON.parse(input.dataset.rules); if (!validateField(input, rules)) allValid = false; } catch(e) {}\n"
        << ind(2) << "});\n"
        << ind(2) << "return allValid;\n"
        << ind(1) << "}\n\n"

        // ═══ Scroll-to-Top ═══
        << ind(1) << "// Scroll-to-Top button\n"
        << ind(1) << "(function() {\n"
        << ind(2) << "var btn = document.getElementById('sad-scroll-top');\n"
        << ind(2) << "if (!btn) return;\n"
        << ind(2) << "window.addEventListener('scroll', function() {\n"
        << ind(3) << "if (window.scrollY > 300) {\n"
        << ind(4) << "btn.style.opacity = '1'; btn.style.visibility = 'visible';\n"
        << ind(3) << "} else {\n"
        << ind(4) << "btn.style.opacity = '0'; btn.style.visibility = 'hidden';\n"
        << ind(3) << "}\n"
        << ind(2) << "});\n"
        << ind(2) << "btn.addEventListener('click', function() {\n"
        << ind(3) << "window.scrollTo({top: 0, behavior: 'smooth'});\n"
        << ind(2) << "});\n"
        << ind(1) << "})();\n\n"

        // ═══ Image Lazy Loading ═══
        << ind(1) << "// Lazy loading for images\n"
        << ind(1) << "if ('loading' in HTMLImageElement.prototype) {\n"
        << ind(2) << "document.querySelectorAll('img[data-src]').forEach(function(img) {\n"
        << ind(3) << "img.src = img.dataset.src;\n"
        << ind(2) << "});\n"
        << ind(1) << "} else if ('IntersectionObserver' in window) {\n"
        << ind(2) << "var imgObserver = new IntersectionObserver(function(entries) {\n"
        << ind(3) << "entries.forEach(function(entry) {\n"
        << ind(4) << "if (entry.isIntersecting) {\n"
        << ind(5) << "var img = entry.target;\n"
        << ind(5) << "if (img.dataset.src) { img.src = img.dataset.src; }\n"
        << ind(5) << "imgObserver.unobserve(img);\n"
        << ind(4) << "}\n"
        << ind(3) << "});\n"
        << ind(2) << "});\n"
        << ind(2) << "document.querySelectorAll('img[data-src]').forEach(function(img) { imgObserver.observe(img); });\n"
        << ind(1) << "}\n\n"

        // ═══ Performance: Debounced Resize ═══
        << ind(1) << "// Debounced resize handler\n"
        << ind(1) << "var __resizeTimer;\n"
        << ind(1) << "window.addEventListener('resize', function() {\n"
        << ind(2) << "clearTimeout(__resizeTimer);\n"
        << ind(2) << "__resizeTimer = setTimeout(function() {\n"
        << ind(3) << "document.dispatchEvent(new CustomEvent('sad-resize', {detail: {"
        << "width: window.innerWidth, height: window.innerHeight}}));\n"
        << ind(2) << "}, 150);\n"
        << ind(1) << "});\n\n"

        // ═══ نظام التراجع/الإعادة للحقول النصية ═══
        << ind(1) << "// === Undo/Redo for text inputs ===\n"
        << ind(1) << "var __undoStacks = new WeakMap();\n"
        << ind(1) << "var __redoStacks = new WeakMap();\n"
        << ind(1) << "function __pushUndo(el) {\n"
        << ind(2) << "if (!__undoStacks.has(el)) __undoStacks.set(el, []);\n"
        << ind(2) << "var stack = __undoStacks.get(el);\n"
        << ind(2) << "if (stack.length > 100) stack.shift();\n"
        << ind(2) << "stack.push({v: el.value, s: el.selectionStart, e: el.selectionEnd});\n"
        << ind(2) << "if (!__redoStacks.has(el)) __redoStacks.set(el, []);\n"
        << ind(2) << "__redoStacks.get(el).length = 0;\n"
        << ind(1) << "}\n"
        << ind(1) << "function __undo(el) {\n"
        << ind(2) << "var stack = __undoStacks.get(el);\n"
        << ind(2) << "if (!stack || !stack.length) return;\n"
        << ind(2) << "if (!__redoStacks.has(el)) __redoStacks.set(el, []);\n"
        << ind(2) << "__redoStacks.get(el).push({v: el.value, s: el.selectionStart, e: el.selectionEnd});\n"
        << ind(2) << "var s = stack.pop();\n"
        << ind(2) << "el.value = s.v;\n"
        << ind(2) << "el.setSelectionRange(s.s, s.e);\n"
        << ind(1) << "}\n"
        << ind(1) << "function __redo(el) {\n"
        << ind(2) << "var stack = __redoStacks.get(el);\n"
        << ind(2) << "if (!stack || !stack.length) return;\n"
        << ind(2) << "if (!__undoStacks.has(el)) __undoStacks.set(el, []);\n"
        << ind(2) << "__undoStacks.get(el).push({v: el.value, s: el.selectionStart, e: el.selectionEnd});\n"
        << ind(2) << "var s = stack.pop();\n"
        << ind(2) << "el.value = s.v;\n"
        << ind(2) << "el.setSelectionRange(s.s, s.e);\n"
        << ind(1) << "}\n\n"

        // ربط الأحداث
        << ind(1) << "document.querySelectorAll('.sad-input').forEach(function(el) {\n"
        << ind(2) << "el.addEventListener('input', function() { __pushUndo(el); });\n"
        << ind(2) << "el.addEventListener('keydown', function(e) {\n"
        << ind(3) << "if ((e.ctrlKey || e.metaKey) && e.key === 'z' && !e.shiftKey) {\n"
        << ind(4) << "e.preventDefault(); __undo(el);\n"
        << ind(3) << "} else if ((e.ctrlKey || e.metaKey) && (e.key === 'y' || (e.key === 'z' && e.shiftKey))) {\n"
        << ind(4) << "e.preventDefault(); __redo(el);\n"
        << ind(3) << "}\n"
        << ind(2) << "});\n"
        << ind(1) << "});\n";

    out << "</script>\n";
}

} // namespace web
} // namespace ui
} // namespace sad
