/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_backend_wasm.cpp — تنفيذ Backend الويب (Canvas2D)
 *  WASM/Web Backend Implementation — renders to HTML Canvas via EM_ASM
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  يرسم جميع المكونات على عنصر <canvas> باستخدام Canvas2D API عبر
 *  Emscripten EM_ASM macros. يدعم:
 *    - تخطيط Column/Row (Flexbox بسيط)
 *    - أزرار، نصوص، حقول إدخال، بطاقات، شرائط تقدم
 *    - نقر الأزرار عبر addEventListener
 *    - RTL/عربي
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_backend_wasm.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <algorithm>
#include <cmath>

namespace sad {
namespace unified {

// ═══════════════════════════════════════════════════════════════════════════════
//  Helper: EM_ASM wrappers — يُصدر JavaScript فقط عند البناء بـ Emscripten
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef __EMSCRIPTEN__

static void js_init_canvas(int w, int h) {
    EM_ASM({
        var canvas = document.getElementById('sad-canvas');
        if (!canvas) {
            canvas = document.createElement('canvas');
            canvas.id = 'sad-canvas';
            canvas.style.direction = 'rtl';
            document.body.appendChild(canvas);
        }
        canvas.width  = $0;
        canvas.height = $1;
        canvas.style.border = '1px solid #ccc';
        
        // تسجيل listener للنقر
        if (!canvas._sadClickRegistered) {
            canvas._sadClickRegistered = true;
            canvas._sadClickQueue = [];
            canvas.addEventListener('click', function(e) {
                var rect = canvas.getBoundingClientRect();
                var x = e.clientX - rect.left;
                var y = e.clientY - rect.top;
                canvas._sadClickQueue.push({x: x, y: y});
            });
        }
    }, w, h);
}

static void js_clear(int r, int g, int b, int w, int h) {
    EM_ASM({
        var canvas = document.getElementById('sad-canvas');
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = 'rgb(' + $0 + ',' + $1 + ',' + $2 + ')';
        ctx.fillRect(0, 0, $3, $4);
    }, r, g, b, w, h);
}

static void js_set_title(const char* title) {
    EM_ASM({
        document.title = UTF8ToString($0);
    }, title);
}

static void js_draw_rect(float x, float y, float w, float h,
                          int r, int g, int b, int a, float radius) {
    EM_ASM({
        var canvas = document.getElementById('sad-canvas');
        var ctx = canvas.getContext('2d');
        ctx.globalAlpha = $8 / 255.0;
        ctx.fillStyle = 'rgb(' + $4 + ',' + $5 + ',' + $6 + ')';
        var rx = $0; var ry = $1; var rw = $2; var rh = $3; var rad = $7;
        if (rad > 0) {
            ctx.beginPath();
            ctx.roundRect(rx, ry, rw, rh, rad);
            ctx.fill();
        } else {
            ctx.fillRect(rx, ry, rw, rh);
        }
        ctx.globalAlpha = 1.0;
    }, x, y, w, h, r, g, b, radius, a);
}

static void js_draw_border(float x, float y, float w, float h,
                            int r, int g, int b, float bw, float radius) {
    EM_ASM({
        var canvas = document.getElementById('sad-canvas');
        var ctx = canvas.getContext('2d');
        ctx.strokeStyle = 'rgb(' + $4 + ',' + $5 + ',' + $6 + ')';
        ctx.lineWidth = $7;
        var rx = $0; var ry = $1; var rw = $2; var rh = $3; var rad = $8;
        if (rad > 0) {
            ctx.beginPath();
            ctx.roundRect(rx, ry, rw, rh, rad);
            ctx.stroke();
        } else {
            ctx.strokeRect(rx, ry, rw, rh);
        }
    }, x, y, w, h, r, g, b, bw, radius);
}

static void js_draw_text(const char* text, float x, float y, float fontSize,
                          int r, int g, int b, const char* align) {
    EM_ASM({
        var canvas = document.getElementById('sad-canvas');
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = 'rgb(' + $3 + ',' + $4 + ',' + $5 + ')';
        ctx.font = $2 + 'px sans-serif';
        ctx.textAlign = UTF8ToString($6);
        ctx.textBaseline = 'middle';
        ctx.direction = 'rtl';
        ctx.fillText(UTF8ToString($0), $1, $7);
    }, text, x, y, fontSize, r, g, b, align);
}

static void js_draw_circle(float cx, float cy, float radius,
                            int r, int g, int b) {
    EM_ASM({
        var canvas = document.getElementById('sad-canvas');
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = 'rgb(' + $3 + ',' + $4 + ',' + $5 + ')';
        ctx.beginPath();
        ctx.arc($0, $1, $2, 0, 2 * Math.PI);
        ctx.fill();
    }, cx, cy, radius, r, g, b);
}

// فحص النقرات وإرجاع إحداثيات آخر نقرة
static bool js_pop_click(float* outX, float* outY) {
    int hasClick = EM_ASM_INT({
        var canvas = document.getElementById('sad-canvas');
        if (canvas && canvas._sadClickQueue && canvas._sadClickQueue.length > 0) {
            var click = canvas._sadClickQueue.shift();
            setValue($0, click.x, 'float');
            setValue($1, click.y, 'float');
            return 1;
        }
        return 0;
    }, outX, outY);
    return hasClick != 0;
}

#else
// ─── Stub implementations عندما لا يكون Emscripten متاحاً ────────────────
static void js_init_canvas(int, int) {}
static void js_clear(int, int, int, int, int) {}
static void js_set_title(const char*) {}
static void js_draw_rect(float, float, float, float, int, int, int, int, float) {}
static void js_draw_border(float, float, float, float, int, int, int, float, float) {}
static void js_draw_text(const char*, float, float, float, int, int, int, const char*) {}
static void js_draw_circle(float, float, float, int, int, int) {}
static bool js_pop_click(float*, float*) { return false; }
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  Constructor / Destructor
// ═══════════════════════════════════════════════════════════════════════════════

WasmBackend::WasmBackend() {}
WasmBackend::~WasmBackend() {}

WasmWidget* WasmBackend::get(int id) {
    auto it = m_widgets.find(id);
    return (it != m_widgets.end()) ? &it->second : nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دورة حياة التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

int WasmBackend::appCreate(const std::string& title, int width, int height) {
    m_canvasWidth  = width;
    m_canvasHeight = height;
    m_running = true;
    js_init_canvas(width, height);
    js_set_title(title.c_str());
    return 1;
}

bool WasmBackend::appIsRunning(int appId) {
    return m_running;
}

void WasmBackend::appUpdate(int appId) {
    if (!m_running) return;
    
    // معالجة النقرات
    float cx, cy;
    while (js_pop_click(&cx, &cy)) {
        // فحص أي widget تم النقر عليه (من الأعمق للأعلى)
        for (auto& [id, w] : m_widgets) {
            if (w.visible && cx >= w.x && cx <= w.x + w.w &&
                cy >= w.y && cy <= w.y + w.h) {
                w.clickFired = true;
            }
        }
    }
    
    // إعادة الرسم
    renderAll();
    
    // إعادة ضبط النقرات في الإطار التالي
    for (auto& [id, w] : m_widgets) {
        w.clickFired = false;
    }
}

void WasmBackend::appSetContent(int appId, WidgetId rootWidget) {
    m_rootWidget = rootWidget;
}

void WasmBackend::appSetBgColor(int appId, int r, int g, int b) {
    m_bgR = r; m_bgG = g; m_bgB = b;
}

void WasmBackend::appClose(int appId) {
    m_running = false;
}

void WasmBackend::appSetTitle(int appId, const std::string& title) {
    js_set_title(title.c_str());
}

int WasmBackend::appGetWidth(int appId)  { return m_canvasWidth; }
int WasmBackend::appGetHeight(int appId) { return m_canvasHeight; }
double WasmBackend::appGetFps(int appId) { return m_currentFps; }

// ═══════════════════════════════════════════════════════════════════════════════
//  إنشاء المكونات
// ═══════════════════════════════════════════════════════════════════════════════

WidgetId WasmBackend::createWidget(WidgetType type) {
    int id = m_nextId++;
    WasmWidget w;
    w.id = id;
    w.type = type;
    
    // ألوان افتراضية حسب النوع
    switch (type) {
        case WidgetType::Button:
        case WidgetType::FloatingButton:
            w.bgR = 66; w.bgG = 133; w.bgB = 244;
            w.txR = 255; w.txG = 255; w.txB = 255;
            w.borderRadius = 8;
            w.h = 44;
            break;
        case WidgetType::Card:
        case WidgetType::InfoCard:
            w.bgR = 255; w.bgG = 255; w.bgB = 255;
            w.borderRadius = 12;
            break;
        case WidgetType::TextField:
        case WidgetType::TextArea:
            w.bgR = 250; w.bgG = 250; w.bgB = 250;
            w.borderWidth = 1;
            w.borderRadius = 6;
            w.h = 40;
            break;
        case WidgetType::ProgressBar:
            w.bgR = 224; w.bgG = 224; w.bgB = 224;
            w.h = 8;
            break;
        case WidgetType::AppBar:
            w.bgR = 66; w.bgG = 133; w.bgB = 244;
            w.txR = 255; w.txG = 255; w.txB = 255;
            w.h = 56;
            w.fontSize = 20;
            break;
        case WidgetType::Divider:
            w.bgR = 200; w.bgG = 200; w.bgB = 200;
            w.h = 1;
            break;
        default:
            break;
    }
    
    m_widgets[id] = w;
    return id;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تعديل الخصائص
// ═══════════════════════════════════════════════════════════════════════════════

void WasmBackend::setText(WidgetId id, const std::string& text) {
    if (auto* w = get(id)) w->text = text;
}

void WasmBackend::setColor(WidgetId id, const std::string& prop,
                            int r, int g, int b, int a) {
    auto* w = get(id);
    if (!w) return;
    if (prop == "bg" || prop == "background" || prop == "خلفية") {
        w->bgR = r; w->bgG = g; w->bgB = b; w->bgA = a;
    } else if (prop == "text" || prop == "نص" || prop == "foreground") {
        w->txR = r; w->txG = g; w->txB = b; w->txA = a;
    } else if (prop == "border" || prop == "حدود") {
        w->brR = r; w->brG = g; w->brB = b; w->brA = a;
    }
}

void WasmBackend::setFloat(WidgetId id, const std::string& prop, float value) {
    auto* w = get(id);
    if (!w) return;
    if      (prop == "fontSize" || prop == "حجم_الخط")        w->fontSize = value;
    else if (prop == "opacity"  || prop == "شفافية")          w->opacity = value;
    else if (prop == "borderWidth" || prop == "عرض_الحدود")   w->borderWidth = value;
    else if (prop == "borderRadius" || prop == "استدارة_الحدود") w->borderRadius = value;
    else if (prop == "width"  || prop == "عرض")               w->w = value;
    else if (prop == "height" || prop == "ارتفاع_مكون")       w->h = value;
    else if (prop == "flex"   || prop == "مرونة")             w->flex = value;
    else if (prop == "value"  || prop == "قيمة")              w->value = value;
    else if (prop == "gap"    || prop == "فراغ")              w->spacing = value;
}

void WasmBackend::setInt(WidgetId id, const std::string& prop, int value) {
    auto* w = get(id);
    if (!w) return;
    if      (prop == "fontWeight" || prop == "وزن_الخط") w->fontWeight = value;
    else if (prop == "zIndex"     || prop == "ترتيب")     w->zIndex = value;
}

void WasmBackend::setBool(WidgetId id, const std::string& prop, bool value) {
    auto* w = get(id);
    if (!w) return;
    if      (prop == "visible" || prop == "مرئي")  w->visible = value;
    else if (prop == "enabled" || prop == "مفعل")  w->enabled = value;
    else if (prop == "checked" || prop == "محدد")  w->checked = value;
}

void WasmBackend::setString(WidgetId id, const std::string& prop,
                              const std::string& value) {
    auto* w = get(id);
    if (!w) return;
    if (prop == "text" || prop == "نص") w->text = value;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  قراءة الخصائص
// ═══════════════════════════════════════════════════════════════════════════════

std::string WasmBackend::getText(WidgetId id) {
    auto* w = get(id);
    return w ? w->text : "";
}

float WasmBackend::getFloat(WidgetId id, const std::string& prop) {
    auto* w = get(id);
    if (!w) return 0;
    if (prop == "value" || prop == "قيمة") return w->value;
    return 0;
}

bool WasmBackend::getBool(WidgetId id, const std::string& prop) {
    auto* w = get(id);
    if (!w) return false;
    if (prop == "checked" || prop == "محدد") return w->checked;
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  شجرة المكونات
// ═══════════════════════════════════════════════════════════════════════════════

void WasmBackend::addChild(WidgetId parent, WidgetId child) {
    auto* p = get(parent);
    auto* c = get(child);
    if (p && c) {
        p->children.push_back(child);
        c->parent = parent;
    }
}

void WasmBackend::removeChild(WidgetId parent, WidgetId child) {
    auto* p = get(parent);
    if (p) {
        auto& ch = p->children;
        ch.erase(std::remove(ch.begin(), ch.end(), child), ch.end());
    }
    auto* c = get(child);
    if (c) c->parent = -1;
}

void WasmBackend::clearChildren(WidgetId parent) {
    auto* p = get(parent);
    if (p) {
        for (int cid : p->children) {
            auto* c = get(cid);
            if (c) c->parent = -1;
        }
        p->children.clear();
    }
}

int WasmBackend::getParent(WidgetId id) {
    auto* w = get(id);
    return w ? w->parent : -1;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  التخطيط
// ═══════════════════════════════════════════════════════════════════════════════

void WasmBackend::setPadding(WidgetId id, float top, float right,
                               float bottom, float left) {
    auto* w = get(id);
    if (!w) return;
    w->paddingT = top; w->paddingR = right;
    w->paddingB = bottom; w->paddingL = left;
    w->padding = (top + right + bottom + left) / 4.0f;
}

void WasmBackend::setMargin(WidgetId id, float top, float right,
                              float bottom, float left) {
    auto* w = get(id);
    if (!w) return;
    w->marginT = top; w->marginR = right;
    w->marginB = bottom; w->marginL = left;
    w->margin = (top + right + bottom + left) / 4.0f;
}

void WasmBackend::setAlignment(WidgetId id, const std::string& main,
                                 const std::string& cross) {
    auto* w = get(id);
    if (w) {
        w->mainAlign  = main;
        w->crossAlign = cross;
    }
}

void WasmBackend::setSpacing(WidgetId id, float spacing) {
    auto* w = get(id);
    if (w) w->spacing = spacing;
}

void WasmBackend::setFlex(WidgetId id, float flex) {
    auto* w = get(id);
    if (w) w->flex = flex;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الأحداث
// ═══════════════════════════════════════════════════════════════════════════════

bool WasmBackend::wasClicked(WidgetId id) {
    auto* w = get(id);
    if (!w) return false;
    bool c = w->clickFired;
    w->clickFired = false;
    return c;
}

std::string WasmBackend::getInputText(WidgetId id) {
    auto* w = get(id);
    return w ? w->inputText : "";
}

void WasmBackend::notifyClick(int widgetId) {
    auto* w = get(widgetId);
    if (w) w->clickFired = true;
}

void WasmBackend::notifyInput(int widgetId, const std::string& text) {
    auto* w = get(widgetId);
    if (w) w->inputText = text;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تخطيط مبسط (Flexbox Column/Row)
// ═══════════════════════════════════════════════════════════════════════════════

void WasmBackend::layoutWidget(WasmWidget& w, float x, float y,
                                 float availW, float availH) {
    float pad = w.padding;
    float padT = (w.paddingT > 0) ? w.paddingT : pad;
    float padR = (w.paddingR > 0) ? w.paddingR : pad;
    float padB = (w.paddingB > 0) ? w.paddingB : pad;
    float padL = (w.paddingL > 0) ? w.paddingL : pad;

    w.x = x + w.margin;
    w.y = y + w.margin;
    
    float ww = (w.w > 0) ? w.w : (availW - 2 * w.margin);
    float wh = (w.h > 0) ? w.h : 0;
    
    w.w = ww;
    
    bool isColumn = (w.type == WidgetType::Column || w.type == WidgetType::Container ||
                     w.type == WidgetType::Card  || w.type == WidgetType::ScrollView ||
                     w.type == WidgetType::Stack);
    bool isRow    = (w.type == WidgetType::Row || w.type == WidgetType::ButtonGroup);
    
    if (w.children.empty()) {
        // عنصر ورقي — ارتفاع تلقائي إذا غير مُحدّد
        if (wh <= 0) {
            switch (w.type) {
                case WidgetType::Button:    wh = 44; break;
                case WidgetType::TextField: wh = 40; break;
                case WidgetType::AppBar:    wh = 56; break;
                case WidgetType::Divider:   wh = 1;  break;
                case WidgetType::Spacer:    wh = 16; break;
                default:
                    wh = (!w.text.empty()) ? (w.fontSize + padT + padB + 8) : 32;
                    break;
            }
        }
        w.h = wh;
        return;
    }
    
    // تخطيط الأبناء
    float contentX = w.x + padL;
    float contentY = w.y + padT;
    float contentW = ww - padL - padR;
    float contentH = availH - padT - padB;
    
    float cursorX = contentX;
    float cursorY = contentY;
    float totalChildH = 0;
    
    for (int cid : w.children) {
        auto* child = get(cid);
        if (!child || !child->visible) continue;
        
        if (isColumn) {
            layoutWidget(*child, contentX, cursorY, contentW, contentH);
            cursorY += child->h + child->margin * 2 + w.spacing;
            totalChildH = cursorY - contentY;
        }
        else if (isRow) {
            float childW = (child->w > 0) ? child->w : (contentW / (float)w.children.size());
            layoutWidget(*child, cursorX, contentY, childW, contentH);
            cursorX += child->w + child->margin * 2 + w.spacing;
            totalChildH = std::max(totalChildH, child->h + child->margin * 2);
        }
        else {
            // Stack: كل الأبناء فوق بعض
            layoutWidget(*child, contentX, contentY, contentW, contentH);
            totalChildH = std::max(totalChildH, child->h + child->margin * 2);
        }
    }
    
    if (wh <= 0) {
        w.h = totalChildH + padT + padB;
    } else {
        w.h = wh;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  رسم مكون واحد
// ═══════════════════════════════════════════════════════════════════════════════

void WasmBackend::renderWidget(const WasmWidget& w) {
    if (!w.visible || w.w <= 0 || w.h <= 0) return;
    
    // خلفية
    bool hasBg = (w.bgA > 0);
    if (hasBg) {
        js_draw_rect(w.x, w.y, w.w, w.h, w.bgR, w.bgG, w.bgB, w.bgA, w.borderRadius);
    }
    
    // حدود
    if (w.borderWidth > 0) {
        js_draw_border(w.x, w.y, w.w, w.h, w.brR, w.brG, w.brB,
                        w.borderWidth, w.borderRadius);
    }
    
    // نص
    if (!w.text.empty()) {
        float textX = w.x + w.w / 2.0f;   // وسط أفقياً
        float textY = w.y + w.h / 2.0f;   // وسط عمودياً
        js_draw_text(w.text.c_str(), textX, textY, w.fontSize,
                      w.txR, w.txG, w.txB, "center");
    }
    
    // شريط التقدم
    if (w.type == WidgetType::ProgressBar && w.value > 0) {
        float fillW = w.w * (w.value / 100.0f);
        js_draw_rect(w.x, w.y, fillW, w.h, 66, 133, 244, 255, w.borderRadius);
    }
    
    // تقدم دائري
    if (w.type == WidgetType::CircleProgress) {
        float cx = w.x + w.w / 2.0f;
        float cy = w.y + w.h / 2.0f;
        float r  = std::min(w.w, w.h) / 2.0f - 4;
        js_draw_circle(cx, cy, r, 66, 133, 244);
    }
    
    // مربع اختيار / مفتاح تبديل
    if (w.type == WidgetType::Checkbox || w.type == WidgetType::Switch) {
        float boxSize = 20;
        float bx = w.x + 8;
        float by = w.y + (w.h - boxSize) / 2.0f;
        if (w.checked) {
            js_draw_rect(bx, by, boxSize, boxSize, 66, 133, 244, 255, 4);
        } else {
            js_draw_border(bx, by, boxSize, boxSize, 150, 150, 150, 2, 4);
        }
    }
    
    // الأبناء
    for (int cid : w.children) {
        auto it = m_widgets.find(cid);
        if (it != m_widgets.end()) {
            renderWidget(it->second);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  رسم كل شيء
// ═══════════════════════════════════════════════════════════════════════════════

void WasmBackend::renderAll() {
    // مسح
    js_clear(m_bgR, m_bgG, m_bgB, m_canvasWidth, m_canvasHeight);
    
    // تخطيط ثم رسم من الجذر
    if (m_rootWidget >= 0) {
        auto* root = get(m_rootWidget);
        if (root) {
            layoutWidget(*root, 0, 0, (float)m_canvasWidth, (float)m_canvasHeight);
            renderWidget(*root);
        }
    }
}

}} // namespace sad::unified
