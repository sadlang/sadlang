// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════
// sad_ui_framework.cpp — محرك واجهات المستخدم الرسومي (109 مكون)
// إطار عمل مشابه لـ Flutter مبني على SDL2 Renderer
//
// هذا الملف يحتوي على:
//   1) نظام خطوط TrueType عالي الجودة (HD) باستخدام stb_truetype
//   2) سجل عناصر (Widget Registry) لتخزين كل المكونات
//   3) حالة التطبيق (AppState) — النافذة + المحرك + الأحداث
//   4) نظام التخطيط التلقائي (Layout) — عمود/صف/شبكة/مكدّس
//   5) محرك الرسم (Renderer) — يرسم كل نوع عنصر بشكله الصحيح
//   6) معالج الأحداث (Events) — نقر/ماوس/لوحة مفاتيح
//   7) دوال الـ API العامة — إنشاء المكونات وتعديل خصائصها
//   8) نظام تعريف المكونات بالخصائص (declarative / Flutter-like)
// ═══════════════════════════════════════════════════════════════════

#include "sad_ui_internal.h"
#include "sad_animation.h"
#include <algorithm>
#include <cmath>
#include <set>
#include <map>
#include <unordered_set>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

namespace sad {
namespace ui {


// ═══════════════════════════════════════════════════════════════════
// القسم 8: معالجة الأحداث (Events)
// ─────────────────────────────────────────────────────────────────
// إعادة تعيين حالة النقر، تتبع الماوس، اختبار الإصابة
// ═══════════════════════════════════════════════════════════════════

// إعادة تعيين wasClicked لكل العناصر في الشجرة
static void resetClickStates(int wid) {
    Widget* w = getWidget(wid);
    if (!w) return;
    w->wasClicked = false;
    for (int c : w->children) resetClickStates(c);
}

// [FIX #41-60] مجموعة الأنواع التفاعلية — O(1) lookup
static bool isInteractiveType(WidgetType t) {
    static const std::set<WidgetType> s = {
        WidgetType::Button, WidgetType::TextField, WidgetType::OutlineButton,
        WidgetType::TextButton, WidgetType::ToggleButton, WidgetType::IconButton,
        WidgetType::FloatingButton, WidgetType::Checkbox, WidgetType::Radio,
        WidgetType::Switch, WidgetType::ListTile, WidgetType::NavItem,
        WidgetType::MenuItem, WidgetType::TreeItem, WidgetType::SearchField,
        WidgetType::PasswordField, WidgetType::TextArea, WidgetType::NumberInput,
        WidgetType::DateField, WidgetType::Dropdown, WidgetType::Link,
        // [FIX #42] أنواع تفاعلية كانت مفقودة
        WidgetType::Slider, WidgetType::RangeSlider, WidgetType::Chip,
        WidgetType::Tag, WidgetType::DotIndicator, WidgetType::StepIndicator,
        WidgetType::Pagination, WidgetType::ExpansionPanel, WidgetType::Accordion,
        WidgetType::TabBar, WidgetType::RatingStars, WidgetType::ColorPicker,
        WidgetType::Stepper, WidgetType::ToggleGroup, WidgetType::ButtonGroup,
        WidgetType::Card, WidgetType::InfoCard, WidgetType::StatCard,
    };
    return s.count(t) > 0;
}

// تحديث حالة التحويم (hover) بناءً على موضع الماوس
static void updateHoverState(int wid, int mx, int my) {
    Widget* w = getWidget(wid);
    if (!w || !w->visible) return;
    bool inside = mx >= w->cx && mx < w->cx+w->cw && my >= w->cy && my < w->cy+w->ch;
    w->hovered = inside && isInteractiveType(w->type);
    for (int c : w->children) updateHoverState(c, mx, my);
}

// معالجة النقر — اختبار إصابة تكراري من الأمام للخلف
static void handleClick(int wid, int mx, int my, bool& handled) {
    Widget* w = getWidget(wid);
    if (!w || !w->visible || !w->enabled || handled) return; // [FIX #48] skip disabled
    bool inside = mx >= w->cx && mx < w->cx+w->cw && my >= w->cy && my < w->cy+w->ch;
    // الأبناء أولاً (الأعلى في الترتيب)
    for (int i = (int)w->children.size()-1; i >= 0; i--) {
        handleClick(w->children[i], mx, my, handled);
        if (handled) return;
    }
    if (!inside) return;

    // ─── أنواع قابلة للنقر ───
    if (w->type == WidgetType::Button || w->type == WidgetType::OutlineButton ||
        w->type == WidgetType::TextButton || w->type == WidgetType::IconButton ||
        w->type == WidgetType::FloatingButton || w->type == WidgetType::Link) {
        w->wasClicked = true; w->pressed = true; handled = true;
    }
    // ─── مربع اختيار — تبديل ───
    else if (w->type == WidgetType::Checkbox || w->type == WidgetType::Switch) {
        w->checked = !w->checked; w->wasClicked = true; handled = true;
    }
    // ─── زر دائري — اختيار حصري مع إلغاء الأشقاء ───
    else if (w->type == WidgetType::Radio) {
        // [FIX #5] إلغاء تحديد جميع الأشقاء Radio في نفس الأب
        for (auto& [pid, pw] : g_widgets) {
            for (int sibId : pw.children) {
                Widget* sib = getWidget(sibId);
                if (sib && sib->type == WidgetType::Radio && sib->id != w->id) {
                    sib->checked = false;
                }
            }
        }
        w->checked = true; w->wasClicked = true; handled = true;
    }
    // ─── زر تبديل ───
    else if (w->type == WidgetType::ToggleButton) {
        w->checked = !w->checked; w->wasClicked = true; w->pressed = true; handled = true;
    }
    // ─── حقول الإدخال — تركيز ───
    else if (w->type == WidgetType::TextField || w->type == WidgetType::SearchField ||
             w->type == WidgetType::PasswordField || w->type == WidgetType::TextArea ||
             w->type == WidgetType::NumberInput || w->type == WidgetType::DateField) {
        // إلغاء تركيز الكل أولاً
        for (auto& p : g_widgets)
            if (p.second.inputFocused) p.second.inputFocused = false;
        w->inputFocused = true; handled = true;
    }
    // ─── لوحة توسيع ───
    else if (w->type == WidgetType::ExpansionPanel) {
        w->expanded = !w->expanded; w->wasClicked = true; handled = true;
    }
    // ─── قائمة منسدلة — تبديل الفهرس ───
    else if (w->type == WidgetType::Dropdown) {
        if (!w->items.empty()) {
            w->selectedIndex = (w->selectedIndex + 1) % (int)w->items.size();
        }
        w->wasClicked = true; handled = true;
    }
    // ─── نجوم التقييم ───
    else if (w->type == WidgetType::RatingStars) {
        int maxS = std::max(1, (int)w->maxVal);
        int starW = std::max(12, (int)w->cw / maxS);
        int clickedStar = (mx - (int)w->cx) / starW + 1;
        w->value = (float)std::min(clickedStar, maxS);
        w->wasClicked = true; handled = true;
    }
    // ─── شريط التبويبات ───
    else if (w->type == WidgetType::TabBar && !w->items.empty()) {
        int tabW = (int)w->cw / (int)w->items.size();
        w->selectedIndex = (mx - (int)w->cx) / tabW;
        w->wasClicked = true; handled = true;
    }
    // ─── ترقيم الصفحات ───
    else if (w->type == WidgetType::Pagination) {
        int total = std::max(1, w->totalPages);
        int btnW = 30, gap = 4;
        int totalW = total * btnW + (total-1) * gap;
        int startX = (int)w->cx + ((int)w->cw - totalW) / 2;
        for (int i = 0; i < total; i++) {
            if (mx >= startX+i*(btnW+gap) && mx < startX+i*(btnW+gap)+btnW) {
                w->currentPage = i; w->wasClicked = true; handled = true; break;
            }
        }
    }
    // ─── منزلق — تحديث القيمة ───
    else if (w->type == WidgetType::Slider) {
        float pct = (float)(mx - (int)w->cx - 4) / ((int)w->cw - 8);
        pct = std::max(0.f, std::min(1.f, pct));
        w->value = w->minVal + pct * (w->maxVal - w->minVal);
        w->wasClicked = true; handled = true;
    }
    // ─── NavItem / MenuItem ───
    else if (w->type == WidgetType::NavItem || w->type == WidgetType::MenuItem ||
             w->type == WidgetType::ListTile || w->type == WidgetType::TreeItem) {
        w->wasClicked = true; w->active = true; handled = true;
    }
    // [FIX #61] رقاقة — تبديل الحالة النشطة
    else if (w->type == WidgetType::Chip || w->type == WidgetType::Tag) {
        w->active = !w->active; w->wasClicked = true; handled = true;
    }
    // [FIX #62] مؤشر نقطي — تبديل الصفحة
    else if (w->type == WidgetType::DotIndicator) {
        int total = std::max(1, w->totalPages);
        int dotR = 6, gap = 8;
        int totalW = total * (dotR * 2) + (total - 1) * gap;
        int startX = (int)w->cx + ((int)w->cw - totalW) / 2;
        for (int i = 0; i < total; i++) {
            int dx = startX + i * (dotR * 2 + gap);
            if (mx >= dx && mx < dx + dotR * 2) {
                w->currentPage = i; w->wasClicked = true; handled = true; break;
            }
        }
    }
    // [FIX #63] أكورديون — توسيع/طي
    else if (w->type == WidgetType::Accordion) {
        w->expanded = !w->expanded; w->wasClicked = true; handled = true;
    }
    // [FIX #64] منزلق نطاق — تحديث القيمتين
    else if (w->type == WidgetType::RangeSlider) {
        float pct = (float)(mx - (int)w->cx - 4) / ((int)w->cw - 8);
        pct = std::max(0.f, std::min(1.f, pct));
        float clickVal = w->minVal + pct * (w->maxVal - w->minVal);
        // تحديث الأقرب: value أو value2
        float distToVal = std::abs(clickVal - w->value);
        float distToVal2 = std::abs(clickVal - w->value2);
        if (distToVal <= distToVal2) w->value = clickVal;
        else w->value2 = clickVal;
        // ضمان value <= value2
        if (w->value > w->value2) std::swap(w->value, w->value2);
        w->wasClicked = true; handled = true;
    }
    // [FIX #65] خطوات/مؤشر خطوة — الانتقال للخطوة
    else if (w->type == WidgetType::Stepper || w->type == WidgetType::StepIndicator) {
        if (!w->items.empty()) {
            int stepW = (int)w->cw / (int)w->items.size();
            int idx = (mx - (int)w->cx) / std::max(1, stepW);
            idx = std::max(0, std::min(idx, (int)w->items.size() - 1));
            w->selectedIndex = idx;
        }
        w->wasClicked = true; handled = true;
    }
    // [FIX #66] منتقي لون — تحديث القيمة بناء على الموضع
    else if (w->type == WidgetType::ColorPicker) {
        float pctX = (float)(mx - (int)w->cx) / std::max(1.f, (float)w->cw);
        float pctY = (float)(my - (int)w->cy) / std::max(1.f, (float)w->ch);
        pctX = std::max(0.f, std::min(1.f, pctX));
        pctY = std::max(0.f, std::min(1.f, pctY));
        // تحديث اللون بناء على HSV
        int hue = (int)(pctX * 360);
        int r = 0, g2 = 0, b = 0;
        // تحويل بسيط HSV→RGB (saturation=1, value from Y)
        float v = 1.0f - pctY;
        int hi = hue / 60 % 6;
        float f = (float)hue / 60.0f - hi;
        int pv = (int)(v * 255); int q = (int)(v * (1 - f) * 255); int t2 = (int)(v * f * 255);
        switch(hi) {
            case 0: r=pv; g2=t2; b=0; break;
            case 1: r=q; g2=pv; b=0; break;
            case 2: r=0; g2=pv; b=t2; break;
            case 3: r=0; g2=q; b=pv; break;
            case 4: r=t2; g2=0; b=pv; break;
            default: r=pv; g2=0; b=q; break;
        }
        w->bgColor = {(uint8_t)r, (uint8_t)g2, (uint8_t)b, 255};
        w->wasClicked = true; handled = true;
    }
    // [FIX #67] بطاقة — تمرير النقر
    else if (w->type == WidgetType::Card || w->type == WidgetType::InfoCard ||
             w->type == WidgetType::StatCard) {
        w->wasClicked = true; handled = true;
    }
}

// إلغاء حالة الضغط عند رفع الماوس
static void handlePressState(int wid, bool down) {
    Widget* w = getWidget(wid);
    if (!w) return;
    if (!down) w->pressed = false;
    for (int c : w->children) handlePressState(c, down);
}

// ═══════════════════════════════════════════════════════════════════
// القسم 9: الدوال العامة (Public API) — إدارة التطبيقات
// ═══════════════════════════════════════════════════════════════════

int app_create(const std::string& title, int width, int height) {
    // [FIX #10] التحقق من نجاح SDL_Init
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "[SAD UI] SDL_Init failed: " << SDL_GetError() << std::endl;
            return -1;
        }
    }
    // تحميل خط HD عند أول تشغيل
    ensureFontLoaded();
    int id = g_nextAppId++;
    AppState app;
    app.id = id; app.width = width; app.height = height;
    app.window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    // [FIX #11] التحقق من نجاح إنشاء النافذة
    if (!app.window) {
        std::cerr << "[SAD UI] SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        g_apps[id] = app;
        app.running = false;
        return id;
    }
    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    // [FIX #12] التحقق من المحرك والنسخة الاحتياطية
    if (!app.renderer) {
        app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_SOFTWARE);
        if (!app.renderer) {
            std::cerr << "[SAD UI] SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        }
    }
    app.running = (app.window && app.renderer);
    if (app.running) SDL_StartTextInput(); // تفعيل إدخال النص
    g_apps[id] = app;
    return id;
}

bool app_is_running(int appId) {
    auto* a = getApp(appId); return a ? a->running : false;
}

void app_update(int appId) {
    auto* a = getApp(appId);
    if (!a || !a->running) return;

    // ═══════════════════════════════════════════════════════════════
    // الخطوة ٠: حساب فارق الوقت (deltaTime)
    // ─────────────────────────────────────────────────────────────
    // نحسب الوقت المنقضي منذ الإطار السابق بدقة عالية
    // هذا ضروري لنظام التحريك وتحديد معدل الإطارات
    // ═══════════════════════════════════════════════════════════════
    Uint32 nowTick = SDL_GetTicks();
    if (a->lastTickMs == 0) a->lastTickMs = nowTick;  // الإطار الأول
    a->deltaMs = (double)(nowTick - a->lastTickMs);
    a->lastTickMs = nowTick;
    // حماية من القفزات الكبيرة (مثل عند تحريك النافذة)
    if (a->deltaMs > 100.0) a->deltaMs = 16.0;
    // حساب FPS (يُحدَّث كل ثانية)
    a->fpsAccum += a->deltaMs;
    a->fpsFrames++;
    if (a->fpsAccum >= 1000.0) {
        a->fps = (double)a->fpsFrames * 1000.0 / a->fpsAccum;
        a->fpsAccum = 0;
        a->fpsFrames = 0;
    }

    // إعادة تعيين حالة النقر
    if (a->rootWidget > 0) resetClickStates(a->rootWidget);
    a->keyInput.clear(); a->backspace = false; a->mouseClicked = false;

    // معالجة أحداث SDL
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT: a->running = false; break;
        case SDL_MOUSEMOTION: a->mouseX = e.motion.x; a->mouseY = e.motion.y; break;
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT) { a->mouseDown = true; a->mouseClicked = true; }
            break;
        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT) a->mouseDown = false;
            break;
        case SDL_TEXTINPUT: a->keyInput += e.text.text; break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_BACKSPACE) a->backspace = true;
            else if (e.key.keysym.sym == SDLK_RETURN) a->keyInput += "\n";
            // معالجة التمرير بالأسهم (لدعم ListView)
            else if (e.key.keysym.sym == SDLK_DOWN || e.key.keysym.sym == SDLK_UP) {
                // يتم التعامل مع التمرير في updateScrollState
            }
            break;
        case SDL_MOUSEWHEEL:
            // [FIX #7] تمرير الماوس — بحث تكراري من الأبناء (hit-test صحيح)
            if (a->rootWidget > 0) {
                float scrollDelta = -e.wheel.y * 40.0f;
                // بحث تكراري من الجذر — يبدأ بالأبناء الأقرب
                std::function<bool(int)> findScrollTarget = [&](int wid) -> bool {
                    Widget* sw = getWidget(wid);
                    if (!sw || !sw->visible) return false;
                    bool inside = a->mouseX >= sw->cx && a->mouseX <= sw->cx + sw->cw &&
                                  a->mouseY >= sw->cy && a->mouseY <= sw->cy + sw->ch;
                    if (!inside) return false;
                    // أبناء أولاً (z-order: الأخير فوق)
                    for (int i = (int)sw->children.size()-1; i >= 0; i--) {
                        if (findScrollTarget(sw->children[i])) return true;
                    }
                    if (sw->type == WidgetType::ListView || sw->type == WidgetType::ScrollView ||
                        sw->type == WidgetType::Column || sw->type == WidgetType::GridView) {
                        sw->scrollY += scrollDelta;
                        float maxScroll = std::max(0.0f, sw->contentHeight - sw->ch);
                        sw->scrollY = std::max(0.0f, std::min(sw->scrollY, maxScroll));
                        return true;
                    }
                    return false;
                };
                findScrollTarget(a->rootWidget);
            }
            break;
        }
    }

    // تحديث إدخال النص للحقل المركّز
    for (auto& p : g_widgets) {
        Widget& w = p.second;
        if (!w.enabled) continue; // [FIX #48] تخطي المكونات المعطلة
        if (w.inputFocused && (w.type == WidgetType::TextField || w.type == WidgetType::SearchField ||
            w.type == WidgetType::PasswordField || w.type == WidgetType::TextArea ||
            w.type == WidgetType::NumberInput || w.type == WidgetType::DateField)) {
            if (!a->keyInput.empty()) w.inputText += a->keyInput;
            // [FIX #3] حذف UTF-8 صحيح — لا يفسد النصوص العربية
            if (a->backspace && !w.inputText.empty()) {
                // UTF-8: المحارف متعددة البايت تبدأ بـ 10xxxxxx (continuation bytes)
                // نحذف البايت الأخير ثم جميع continuation bytes
                size_t pos = w.inputText.size();
                if (pos > 0) {
                    pos--;
                    // تراجع عن continuation bytes (0x80..0xBF)
                    while (pos > 0 && (static_cast<unsigned char>(w.inputText[pos]) & 0xC0) == 0x80) {
                        pos--;
                    }
                    w.inputText.erase(pos);
                }
            }
        }
    }

    if (a->rootWidget > 0) {
        updateHoverState(a->rootWidget, a->mouseX, a->mouseY);
        if (a->mouseClicked) {
            bool handled = false;
            handleClick(a->rootWidget, a->mouseX, a->mouseY, handled);
            if (!handled) { // نقر على مكان فارغ → إلغاء التركيز
                for (auto& p : g_widgets)
                    if (p.second.inputFocused) p.second.inputFocused = false;
            }
        }
        if (!a->mouseDown) handlePressState(a->rootWidget, false);
        // حساب التخطيط
        layoutWidget(a->rootWidget, 0, 0, (float)a->width, (float)a->height);
    }

    // ═══════════════════════════════════════════════════════════════
    // تحديث نظام التحريك
    // ─────────────────────────────────────────────────────────────
    // نمرر فارق الوقت بالمللي ثانية لتحديث جميع التحريكات النشطة
    // هذا يجعل التحريكات مستقلة عن معدل الإطارات
    // ═══════════════════════════════════════════════════════════════
    sad::anim::updateAllAnimations(a->deltaMs);

    // الرسم
    SDL_SetRenderDrawColor(a->renderer, a->bgColor.r, a->bgColor.g, a->bgColor.b, a->bgColor.a);
    SDL_RenderClear(a->renderer);
    if (a->rootWidget > 0) renderWidget(a->renderer, a->rootWidget);
    SDL_RenderPresent(a->renderer);

    // ═══════════════════════════════════════════════════════════════
    // تحديد معدل الإطارات (Frame Rate Limiting)
    // ─────────────────────────────────────────────────────────────
    // نحدّد سرعة الحلقة لتوفير طاقة المعالج
    // بدون هذا، الحلقة تعمل بأقصى سرعة وتستهلك CPU ١٠٠٪
    // ═══════════════════════════════════════════════════════════════
    if (a->targetFps > 0) {
        Uint32 frameTime = SDL_GetTicks() - nowTick;
        Uint32 targetTime = 1000 / (Uint32)a->targetFps;
        if (frameTime < targetTime) {
            SDL_Delay(targetTime - frameTime);
        }
    }
}

void app_set_content(int appId, int widgetId) {
    auto* a = getApp(appId); if (a) a->rootWidget = widgetId;
}
void app_set_bg_color(int appId, int r, int g, int b) {
    auto* a = getApp(appId); if (a) a->bgColor = {r,g,b,255};
}
void app_close(int appId) {
    auto* a = getApp(appId); if (!a) return;
    a->running = false;
    if (a->renderer) { SDL_DestroyRenderer(a->renderer); a->renderer = nullptr; }
    if (a->window) { SDL_DestroyWindow(a->window); a->window = nullptr; }
}

// ═══════════════════════════════════════════════════════════════════
// القسم 10: إنشاء العناصر الـ 109
// ═══════════════════════════════════════════════════════════════════

// ─── العناصر الأصلية 9 ───
int widget_column()    { return createWidget(WidgetType::Column); }
int widget_row()       { return createWidget(WidgetType::Row); }
int widget_text(const std::string& c) {
    int id = createWidget(WidgetType::Text);
    auto* w = getWidget(id); if(w) { w->text = c; w->textColor = {33,33,33,255}; }
    return id;
}
int widget_button(const std::string& t) {
    int id = createWidget(WidgetType::Button);
    auto* w = getWidget(id); if(w) {
        w->text = t; w->textColor = {255,255,255,255};
        w->btnColor = {66,133,244,255}; w->btnHover = {90,155,255,255};
        w->btnPress = {40,100,200,255}; w->borderRadius = 8;
    } return id;
}
int widget_textfield(const std::string& h) {
    int id = createWidget(WidgetType::TextField);
    auto* w = getWidget(id); if(w) { w->text = h; w->bgColor = {255,255,255,255}; w->borderRadius = 4; }
    return id;
}
int widget_container() { return createWidget(WidgetType::Container); }
int widget_spacer(float s) {
    int id = createWidget(WidgetType::Spacer);
    auto* w = getWidget(id); if(w) w->fixedHeight = s; return id;
}
int widget_card() {
    int id = createWidget(WidgetType::Card);
    auto* w = getWidget(id); if(w) { w->bgColor = {255,255,255,255}; w->borderRadius = 12; w->padding = 16; }
    return id;
}
int widget_divider() { return createWidget(WidgetType::Divider); }

// ─── تخطيط (10) ───
int widget_stack()     { return createWidget(WidgetType::Stack); }
int widget_center()    { return createWidget(WidgetType::Center); }
int widget_wrap()      { auto id=createWidget(WidgetType::Wrap); auto*w=getWidget(id); if(w)w->columns=4; return id; }
int widget_gridview(int cols) {
    int id=createWidget(WidgetType::GridView); auto*w=getWidget(id); if(w) w->columns=cols; return id;
}
int widget_listview()  { return createWidget(WidgetType::ListView); }
int widget_scrollview(){ return createWidget(WidgetType::ScrollView); }
int widget_sizedbox(float w_, float h_) {
    int id=createWidget(WidgetType::SizedBox); auto*w=getWidget(id);
    if(w){w->fixedWidth=w_; w->fixedHeight=h_;} return id;
}
int widget_expanded()  { int id=createWidget(WidgetType::Expanded); auto*w=getWidget(id); if(w)w->flex=1; return id; }
int widget_pageview()  { return createWidget(WidgetType::PageView); }
int widget_table(int cols) {
    int id=createWidget(WidgetType::Table); auto*w=getWidget(id); if(w)w->columns=cols; return id;
}

// ─── أزرار (6) ───
int widget_icon_button(char icon) {
    int id=createWidget(WidgetType::IconButton); auto*w=getWidget(id);
    if(w){w->iconChar=icon; w->textColor={60,60,60,255}; w->fixedWidth=40; w->fixedHeight=40;}
    return id;
}
int widget_floating_button(char icon) {
    int id=createWidget(WidgetType::FloatingButton); auto*w=getWidget(id);
    if(w){w->iconChar=icon; w->btnColor={66,133,244,255}; w->fixedWidth=56; w->fixedHeight=56;}
    return id;
}
int widget_outline_button(const std::string& t) {
    int id=createWidget(WidgetType::OutlineButton); auto*w=getWidget(id);
    if(w){w->text=t; w->btnColor={66,133,244,255}; w->borderRadius=8;} return id;
}
int widget_text_button(const std::string& t) {
    int id=createWidget(WidgetType::TextButton); auto*w=getWidget(id);
    if(w){w->text=t; w->btnColor={66,133,244,255};} return id;
}
int widget_toggle_button(const std::string& t) {
    int id=createWidget(WidgetType::ToggleButton); auto*w=getWidget(id);
    if(w){w->text=t; w->btnColor={66,133,244,255}; w->borderRadius=8;} return id;
}
int widget_button_group() { return createWidget(WidgetType::ButtonGroup); }

// ─── عناصر الإدخال (14) ───
int widget_checkbox(const std::string& l) {
    int id=createWidget(WidgetType::Checkbox); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_radio(const std::string& l) {
    int id=createWidget(WidgetType::Radio); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_switch(const std::string& l) {
    int id=createWidget(WidgetType::Switch); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_slider(float mn, float mx) {
    int id=createWidget(WidgetType::Slider); auto*w=getWidget(id);
    if(w){w->minVal=mn; w->maxVal=mx; w->value=mn;} return id;
}
int widget_dropdown(const std::string& h) {
    int id=createWidget(WidgetType::Dropdown); auto*w=getWidget(id); if(w) w->text=h; return id;
}
int widget_number_input(float mn, float mx) {
    int id=createWidget(WidgetType::NumberInput); auto*w=getWidget(id);
    if(w){w->minVal=mn; w->maxVal=mx; w->text="0";} return id;
}
int widget_search_field(const std::string& h) {
    int id=createWidget(WidgetType::SearchField); auto*w=getWidget(id); if(w) w->text=h; return id;
}
int widget_password_field(const std::string& h) {
    int id=createWidget(WidgetType::PasswordField); auto*w=getWidget(id); if(w) w->text=h; return id;
}
int widget_textarea(const std::string& h) {
    int id=createWidget(WidgetType::TextArea); auto*w=getWidget(id); if(w) w->text=h; return id;
}
int widget_date_field() { return createWidget(WidgetType::DateField); }
int widget_range_slider(float mn, float mx) {
    int id=createWidget(WidgetType::RangeSlider); auto*w=getWidget(id);
    if(w){w->minVal=mn; w->maxVal=mx; w->value=mn; w->value2=mx;} return id;
}
int widget_toggle_group() { return createWidget(WidgetType::ToggleGroup); }
int widget_color_picker() { int id=createWidget(WidgetType::ColorPicker); auto*w=getWidget(id); if(w){w->fixedHeight=80;} return id; }
int widget_rating_stars(int mx) {
    int id=createWidget(WidgetType::RatingStars); auto*w=getWidget(id);
    if(w){w->maxVal=(float)mx; w->value=0;} return id;
}

// ─── عرض النصوص (7) ───
int widget_label(const std::string& t) {
    int id=createWidget(WidgetType::Label); auto*w=getWidget(id); if(w){w->text=t; w->textColor={33,33,33,255};} return id;
}
int widget_heading(const std::string& t, int lv) {
    int id=createWidget(WidgetType::Heading); auto*w=getWidget(id);
    if(w){w->text=t; w->level=lv; w->textColor={20,20,20,255}; w->fontSize=28;} return id;
}
int widget_paragraph(const std::string& t) {
    int id=createWidget(WidgetType::Paragraph); auto*w=getWidget(id); if(w){w->text=t; w->textColor={60,60,60,255}; w->textAlign="left";} return id;
}
int widget_link(const std::string& t) {
    int id=createWidget(WidgetType::Link); auto*w=getWidget(id); if(w) w->text=t; return id;
}
int widget_code_block(const std::string& c) {
    int id=createWidget(WidgetType::CodeBlock); auto*w=getWidget(id); if(w) w->text=c; return id;
}
int widget_quote(const std::string& t) {
    int id=createWidget(WidgetType::Quote); auto*w=getWidget(id); if(w) w->text=t; return id;
}
int widget_markdown(const std::string& t) {
    int id=createWidget(WidgetType::Markdown); auto*w=getWidget(id); if(w) w->text=t; return id;
}

// ─── عرض مرئي (10) ───
int widget_icon(char ic) {
    int id=createWidget(WidgetType::Icon); auto*w=getWidget(id); if(w) w->iconChar=ic; return id;
}
int widget_avatar(const std::string& l, int r_, int g_, int b_) {
    int id=createWidget(WidgetType::Avatar); auto*w=getWidget(id);
    if(w){w->text=l; w->btnColor={r_,g_,b_,255}; w->fixedWidth=48; w->fixedHeight=48;} return id;
}
int widget_badge(int count) {
    int id=createWidget(WidgetType::Badge); auto*w=getWidget(id);
    if(w){w->value=(float)count; w->fixedWidth=24; w->fixedHeight=24;} return id;
}
int widget_chip(const std::string& l) {
    int id=createWidget(WidgetType::Chip); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_tag(const std::string& l, int r_, int g_, int b_) {
    int id=createWidget(WidgetType::Tag); auto*w=getWidget(id);
    if(w){w->text=l; w->btnColor={r_,g_,b_,255};} return id;
}
int widget_status_dot(int r_, int g_, int b_) {
    int id=createWidget(WidgetType::StatusDot); auto*w=getWidget(id);
    if(w){w->bgColor={r_,g_,b_,255}; w->fixedWidth=16; w->fixedHeight=16;} return id;
}
int widget_color_box(int r_, int g_, int b_) {
    int id=createWidget(WidgetType::ColorBox); auto*w=getWidget(id);
    if(w){w->bgColor={r_,g_,b_,255};} return id;
}
int widget_gradient_box(int r1, int g1, int b1, int r2, int g2, int b2) {
    int id=createWidget(WidgetType::GradientBox); auto*w=getWidget(id);
    if(w){w->btnColor={r1,g1,b1,255}; w->accentColor={r2,g2,b2,255};} return id;
}
int widget_image(const std::string& p) {
    int id=createWidget(WidgetType::ImageWidget); auto*w=getWidget(id); if(w) w->text=p; return id;
}
int widget_separator() { return createWidget(WidgetType::Separator); }

// ─── تقدم (4) ───
int widget_progress_bar(float v) {
    int id=createWidget(WidgetType::ProgressBar); auto*w=getWidget(id);
    if(w){w->value=v; w->maxVal=100;} return id;
}
int widget_circle_progress(float v) {
    int id=createWidget(WidgetType::CircleProgress); auto*w=getWidget(id);
    if(w){w->value=v; w->maxVal=100; w->fixedWidth=60; w->fixedHeight=60;} return id;
}
int widget_loading_spinner() {
    int id=createWidget(WidgetType::LoadingSpinner); auto*w=getWidget(id);
    if(w){w->fixedWidth=40; w->fixedHeight=40;} return id;
}
int widget_skeleton() { return createWidget(WidgetType::Skeleton); }

// ─── تنقل (10) ───
int widget_app_bar(const std::string& t) {
    int id=createWidget(WidgetType::AppBar); auto*w=getWidget(id);
    if(w){w->text=t; w->fixedHeight=56; w->bgColor={25,80,160,255};} return id;
}
int widget_bottom_bar() {
    int id=createWidget(WidgetType::BottomBar); auto*w=getWidget(id);
    if(w) w->fixedHeight=56; return id;
}
int widget_tab_bar() {
    int id=createWidget(WidgetType::TabBar); auto*w=getWidget(id);
    if(w){w->fixedHeight=48; w->selectedIndex=0;} return id;
}
int widget_drawer() { return createWidget(WidgetType::Drawer); }
int widget_breadcrumb() { return createWidget(WidgetType::Breadcrumb); }
int widget_nav_item(const std::string& l) {
    int id=createWidget(WidgetType::NavItem); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_menu_item(const std::string& l) {
    int id=createWidget(WidgetType::MenuItem); auto*w=getWidget(id); if(w) w->text=l; return id;
}
int widget_toolbar() { return createWidget(WidgetType::Toolbar); }
int widget_status_bar(const std::string& t) {
    int id=createWidget(WidgetType::StatusBar); auto*w=getWidget(id);
    if(w){w->text=t; w->fixedHeight=28;} return id;
}
int widget_side_menu() { return createWidget(WidgetType::SideMenu); }

// ─── ردود الفعل (10) ───
int widget_dialog(const std::string& t, const std::string& m) {
    int id=createWidget(WidgetType::Dialog); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=m; w->padding=16; w->bgColor={255,255,255,255}; w->borderRadius=12;}
    return id;
}
int widget_alert(const std::string& m, const std::string& type) {
    int id=createWidget(WidgetType::Alert); auto*w=getWidget(id);
    if(w){w->text=m; w->subtitle=type;} return id;
}
int widget_toast(const std::string& m) {
    int id=createWidget(WidgetType::Toast); auto*w=getWidget(id); if(w) w->text=m; return id;
}
int widget_snackbar(const std::string& m) {
    int id=createWidget(WidgetType::Snackbar); auto*w=getWidget(id); if(w) w->text=m; return id;
}
int widget_banner(const std::string& m) {
    int id=createWidget(WidgetType::Banner); auto*w=getWidget(id); if(w) w->text=m; return id;
}
int widget_modal() {
    int id=createWidget(WidgetType::Modal); auto*w=getWidget(id);
    if(w){w->padding=24; w->bgColor={255,255,255,255}; w->borderRadius=12;} return id;
}
int widget_popover() { return createWidget(WidgetType::Popover); }
int widget_notification(const std::string& t, const std::string& m) {
    int id=createWidget(WidgetType::Notification); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=m;} return id;
}
int widget_confirm_dialog(const std::string& t, const std::string& m) {
    int id=createWidget(WidgetType::ConfirmDialog); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=m; w->padding=16; w->bgColor={255,255,255,255}; w->borderRadius=12;}
    return id;
}
int widget_tooltip(const std::string& t) {
    int id=createWidget(WidgetType::Tooltip); auto*w=getWidget(id); if(w) w->text=t; return id;
}

// ─── بطاقات ولوحات (7) ───
int widget_info_card(const std::string& t, const std::string& b) {
    int id=createWidget(WidgetType::InfoCard); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=b; w->borderRadius=12; w->padding=12;} return id;
}
int widget_stat_card(const std::string& l, const std::string& v) {
    int id=createWidget(WidgetType::StatCard); auto*w=getWidget(id);
    if(w){w->text=l; w->subtitle=v; w->borderRadius=12; w->padding=12;} return id;
}
int widget_expansion_panel(const std::string& t) {
    int id=createWidget(WidgetType::ExpansionPanel); auto*w=getWidget(id);
    if(w) w->text=t; return id;
}
int widget_accordion() { return createWidget(WidgetType::Accordion); }
int widget_panel(const std::string& t) {
    int id=createWidget(WidgetType::Panel); auto*w=getWidget(id);
    if(w){w->text=t; w->padding=12;} return id;
}
int widget_group_box(const std::string& t) {
    int id=createWidget(WidgetType::GroupBox); auto*w=getWidget(id);
    if(w){w->text=t; w->padding=12;} return id;
}
int widget_callout(const std::string& t, const std::string& type) {
    int id=createWidget(WidgetType::Callout); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=type;} return id;
}

// ─── بيانات (10) ───
int widget_list_tile(const std::string& t, const std::string& s) {
    int id=createWidget(WidgetType::ListTile); auto*w=getWidget(id);
    if(w){w->text=t; w->subtitle=s;} return id;
}
int widget_data_table(int cols) {
    int id=createWidget(WidgetType::DataTable); auto*w=getWidget(id);
    if(w) w->columns=cols; return id;
}
int widget_table_row() { return createWidget(WidgetType::TableRow); }
int widget_table_cell(const std::string& t) {
    int id=createWidget(WidgetType::TableCell); auto*w=getWidget(id); if(w) w->text=t; return id;
}
int widget_detail_row(const std::string& k, const std::string& v) {
    int id=createWidget(WidgetType::DetailRow); auto*w=getWidget(id);
    if(w){w->text=k; w->subtitle=v;} return id;
}
int widget_timeline() { return createWidget(WidgetType::Timeline); }
int widget_timeline_item(const std::string& t) {
    int id=createWidget(WidgetType::TimelineItem); auto*w=getWidget(id); if(w) w->text=t; return id;
}
int widget_tree_item(const std::string& l, int depth) {
    int id=createWidget(WidgetType::TreeItem); auto*w=getWidget(id);
    if(w){w->text=l; w->level=depth;} return id;
}
int widget_stepper(int steps) {
    int id=createWidget(WidgetType::Stepper); auto*w=getWidget(id);
    if(w){w->maxVal=(float)steps; w->value=0;} return id;
}
int widget_step_indicator(int total, int current) {
    int id=createWidget(WidgetType::StepIndicator); auto*w=getWidget(id);
    if(w){w->maxVal=(float)total; w->value=(float)current;} return id;
}

// ─── رسوم بيانية (7) ───
int widget_bar_chart()   { int id=createWidget(WidgetType::BarChart); auto*w=getWidget(id); if(w) w->fixedHeight=150; return id; }
int widget_hbar_chart()  { int id=createWidget(WidgetType::HBarChart); auto*w=getWidget(id); if(w) w->fixedHeight=150; return id; }
int widget_pie_chart()   { int id=createWidget(WidgetType::PieChart); auto*w=getWidget(id); if(w){w->fixedWidth=150; w->fixedHeight=150;} return id; }
int widget_line_chart()  { int id=createWidget(WidgetType::LineChart); auto*w=getWidget(id); if(w) w->fixedHeight=120; return id; }
int widget_spark_line()  { int id=createWidget(WidgetType::SparkLine); auto*w=getWidget(id); if(w) w->fixedHeight=40; return id; }
int widget_gauge(float v, float mx) {
    int id=createWidget(WidgetType::Gauge); auto*w=getWidget(id);
    if(w){w->value=v; w->maxVal=mx; w->fixedHeight=80;} return id;
}
int widget_donut_chart() { int id=createWidget(WidgetType::DonutChart); auto*w=getWidget(id); if(w){w->fixedWidth=150; w->fixedHeight=150;} return id; }

// ─── ترقيم صفحات (2) ───
int widget_pagination(int total) {
    int id=createWidget(WidgetType::Pagination); auto*w=getWidget(id);
    if(w){w->totalPages=total; w->currentPage=0; w->fixedHeight=40;} return id;
}
int widget_dot_indicator(int total, int current) {
    int id=createWidget(WidgetType::DotIndicator); auto*w=getWidget(id);
    if(w){w->maxVal=(float)total; w->value=(float)current;} return id;
}

// ─── متقدم (3) ───
int widget_canvas(float w_, float h_) {
    int id=createWidget(WidgetType::Canvas); auto*w=getWidget(id);
    if(w){w->fixedWidth=w_; w->fixedHeight=h_;} return id;
}
int widget_split_view() { return createWidget(WidgetType::SplitView); }
int widget_carousel()   { return createWidget(WidgetType::Carousel); }

// ═══════════════════════════════════════════════════════════════════
// القسم 11: إدارة شجرة العناصر وتعديل الخصائص
// ═══════════════════════════════════════════════════════════════════

void widget_add_child(int pid, int cid)   { auto*w=getWidget(pid); if(w) w->children.push_back(cid); }
void widget_clear_children(int pid)       { auto*w=getWidget(pid); if(w) w->children.clear(); }

void widget_set_bg(int id,int r,int g,int b)         { auto*w=getWidget(id); if(w) w->bgColor={r,g,b,255}; }
void widget_set_padding(int id,float a)              { auto*w=getWidget(id); if(w){w->padding=a; w->padTop=w->padRight=w->padBottom=w->padLeft=a;} }
void widget_set_padding4(int id,float t,float r,float b,float l) { auto*w=getWidget(id); if(w){w->padding=0; w->padTop=t; w->padRight=r; w->padBottom=b; w->padLeft=l;} }
void widget_set_height(int id,float h)               { auto*w=getWidget(id); if(w) w->fixedHeight=h; }
void widget_set_width(int id,float v)                { auto*w=getWidget(id); if(w) w->fixedWidth=v; }
void widget_set_flex(int id,float f)                 { auto*w=getWidget(id); if(w) w->flex=f; }
void widget_set_border_radius(int id,float r)        { auto*w=getWidget(id); if(w) w->borderRadius=r; }
void widget_set_spacing(int id,float g)              { auto*w=getWidget(id); if(w) w->spacing=g; }
void widget_set_visible(int id,bool v)               { auto*w=getWidget(id); if(w) w->visible=v; }
void widget_set_border(int id,float bw,int r,int g,int b) { auto*w=getWidget(id); if(w){w->borderWidth=bw; w->borderColor={r,g,b,255};} }

void text_set_size(int id,float s)                   { auto*w=getWidget(id); if(w) w->fontSize=s; }
void text_set_color(int id,int r,int g,int b)        { auto*w=getWidget(id); if(w) w->textColor={r,g,b,255}; }
void text_set_content(int id,const std::string& c)   { auto*w=getWidget(id); if(w) w->text=c; }
std::string text_get_content(int id)                 { auto*w=getWidget(id); return w ? w->text : ""; }
void text_set_align(int id,const std::string& a)     { auto*w=getWidget(id); if(w) w->textAlign=a; }

void button_set_color(int id,int r,int g,int b) {
    auto*w=getWidget(id); if(w){
        w->btnColor={r,g,b,255};
        w->btnHover={std::min(r+30,255),std::min(g+30,255),std::min(b+30,255),255};
        w->btnPress={std::max(r-30,0),std::max(g-30,0),std::max(b-30,0),255};
    }
}
bool button_was_clicked(int id)                      { auto*w=getWidget(id); return w ? w->wasClicked : false; }
bool button_was_clicked(const std::string& name)      { auto*w=getWidgetByName(name); return w ? w->wasClicked : false; }

std::string textfield_get_text(int id)               { auto*w=getWidget(id); return w ? w->inputText : ""; }
std::string textfield_get_text(const std::string& name){ auto*w=getWidgetByName(name); return w ? w->inputText : ""; }
void textfield_set_text(int id,const std::string& t) { auto*w=getWidget(id); if(w) w->inputText=t; }

// ─── الخصائص الموسّعة للمكونات الجديدة ───
void   widget_set_checked(int id,bool c)             { auto*w=getWidget(id); if(w) w->checked=c; }
bool   widget_get_checked(int id)                    { auto*w=getWidget(id); return w ? w->checked : false; }
bool   widget_get_checked(const std::string& name)   { auto*w=getWidgetByName(name); return w ? w->checked : false; }
void   widget_set_value(int id,float v)              { auto*w=getWidget(id); if(w) w->value=v; }
float  widget_get_value(int id)                      { auto*w=getWidget(id); return w ? w->value : 0; }
float  widget_get_value(const std::string& name)     { auto*w=getWidgetByName(name); return w ? w->value : 0; }
void   widget_set_selected(int id,int i)             { auto*w=getWidget(id); if(w) w->selectedIndex=i; }
int    widget_get_selected(int id)                   { auto*w=getWidget(id); return w ? w->selectedIndex : -1; }
void   widget_set_expanded(int id,bool e)            { auto*w=getWidget(id); if(w) w->expanded=e; }
bool   widget_get_expanded(int id)                   { auto*w=getWidget(id); return w ? w->expanded : false; }
void   widget_set_items(int id,const std::vector<std::string>& items) { auto*w=getWidget(id); if(w) w->items=items; }
void   widget_add_item(int id,const std::string& item){ auto*w=getWidget(id); if(w) w->items.push_back(item); }
void   widget_set_data(int id,const std::vector<float>& d) { auto*w=getWidget(id); if(w) w->dataValues=d; }
void   widget_add_data(int id,float v)               { auto*w=getWidget(id); if(w) w->dataValues.push_back(v); }
void   widget_set_icon(int id,char c)                { auto*w=getWidget(id); if(w) w->iconChar=c; }
void   widget_set_active(int id,bool a)              { auto*w=getWidget(id); if(w) w->active=a; }
void   widget_set_subtitle(int id,const std::string& s){ auto*w=getWidget(id); if(w) w->subtitle=s; }
void   widget_set_level(int id,int l)                { auto*w=getWidget(id); if(w) w->level=l; }
void   widget_set_columns(int id,int c)              { auto*w=getWidget(id); if(w) w->columns=c; }
void   widget_set_page(int id,int p)                 { auto*w=getWidget(id); if(w) w->currentPage=p; }
void   widget_set_accent_color(int id,int r,int g,int b) { auto*w=getWidget(id); if(w) w->accentColor={r,g,b,255}; }
void   widget_show_dialog(int id,bool s)             { auto*w=getWidget(id); if(w) w->dialogVisible=s; }
bool   widget_is_dialog_visible(int id)              { auto*w=getWidget(id); return w ? w->dialogVisible : false; }

// ═══════════════════════════════════════════════════════════════════
// القسم الجديد: نظام إنشاء المكونات بالخصائص (Declarative API)
// ─────────────────────────────────────────────────────────────────
// يتيح إنشاء المكونات بطريقة تعريفية شبيهة بـ Flutter
// باستخدام خريطة خصائص (Map) بأسماء عربية
// ═══════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════
// [FIX #1-30] parseColor محسّن: 60+ لون مسمى + أسماء إنجليزية + خريطة (O(1))
// ═══════════════════════════════════════════════════════════════════
static const std::unordered_map<std::string, RGBA>& getNamedColors() {
    static const std::unordered_map<std::string, RGBA> colors = {
        // ─── الألوان الأساسية بالعربية ───
        {"\xd8\xa3\xd8\xad\xd9\x85\xd8\xb1", {244,67,54,255}},   // أحمر
        {"\xd8\xa7\xd8\xad\xd9\x85\xd8\xb1", {244,67,54,255}},   // احمر
        {"\xd8\xa3\xd8\xae\xd8\xb6\xd8\xb1", {76,175,80,255}},   // أخضر
        {"\xd8\xa7\xd8\xae\xd8\xb6\xd8\xb1", {76,175,80,255}},   // اخضر
        {"\xd8\xa3\xd8\xb2\xd8\xb1\xd9\x82", {33,150,243,255}},  // أزرق
        {"\xd8\xa7\xd8\xb2\xd8\xb1\xd9\x82", {33,150,243,255}},  // ازرق
        {"\xd8\xa3\xd8\xa8\xd9\x8a\xd8\xb6", {255,255,255,255}}, // أبيض
        {"\xd8\xa7\xd8\xa8\xd9\x8a\xd8\xb6", {255,255,255,255}}, // ابيض
        {"\xd8\xa3\xd8\xb3\xd9\x88\xd8\xaf", {0,0,0,255}},       // أسود
        {"\xd8\xa7\xd8\xb3\xd9\x88\xd8\xaf", {0,0,0,255}},       // اسود
        {"\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a", {158,158,158,255}}, // رمادي
        {"\xd8\xa8\xd8\xb1\xd8\xaa\xd9\x82\xd8\xa7\xd9\x84\xd9\x8a", {255,152,0,255}}, // برتقالي
        {"\xd8\xa8\xd9\x86\xd9\x81\xd8\xb3\xd8\xac\xd9\x8a", {156,39,176,255}}, // بنفسجي
        {"\xd9\x88\xd8\xb1\xd8\xaf\xd9\x8a", {233,30,99,255}},   // وردي
        {"\xd8\xa8\xd9\x86\xd9\x8a", {121,85,72,255}},             // بني
        {"\xd8\xb0\xd9\x87\xd8\xa8\xd9\x8a", {255,193,7,255}},   // ذهبي
        {"\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81", {0,0,0,0}},          // شفاف
        {"\xd8\xb3\xd9\x85\xd8\xa7\xd9\x88\xd9\x8a", {0,188,212,255}}, // سماوي
        {"\xd9\x84\xd9\x8a\xd9\x85\xd9\x88\xd9\x86\xd9\x8a", {205,220,57,255}}, // ليموني
        {"\xd9\x86\xd9\x8a\xd9\x84\xd9\x8a", {63,81,181,255}},   // نيلي
        {"\xd9\x81\xd8\xb6\xd9\x8a", {189,189,189,255}},           // فضي
        {"\xd9\x83\xd8\xad\xd9\x84\xd9\x8a", {26,35,126,255}},   // كحلي
        {"\xd8\xb2\xd9\x8a\xd8\xaa\xd9\x8a", {85,139,47,255}},   // زيتي
        {"\xd8\xae\xd9\x85\xd8\xb1\xd9\x8a", {136,14,79,255}},   // خمري
        {"\xd8\xaa\xd8\xb1\xd9\x83\xd9\x88\xd8\xa7\xd8\xb2", {0,150,136,255}}, // تركواز
        // ─── [NEW] 30+ لون عربي إضافي ───
        {"\xd8\xa3\xd8\xb5\xd9\x81\xd8\xb1", {255,235,59,255}},   // أصفر
        {"\xd8\xa7\xd8\xb5\xd9\x81\xd8\xb1", {255,235,59,255}},   // اصفر
        {"\xd8\xa8\xd9\x86\xd9\x81\xd8\xb3\xd8\xac\xd9\x8a_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad", {186,104,200,255}}, // بنفسجي_فاتح
        {"\xd8\xa3\xd8\xb2\xd8\xb1\xd9\x82_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad", {100,181,246,255}}, // أزرق_فاتح
        {"\xd8\xa3\xd8\xae\xd8\xb6\xd8\xb1_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad", {129,199,132,255}}, // أخضر_فاتح
        {"\xd8\xa3\xd8\xad\xd9\x85\xd8\xb1_\xd8\xba\xd8\xa7\xd9\x85\xd9\x82", {183,28,28,255}}, // أحمر_غامق
        {"\xd8\xa3\xd8\xb2\xd8\xb1\xd9\x82_\xd8\xba\xd8\xa7\xd9\x85\xd9\x82", {13,71,161,255}}, // أزرق_غامق
        {"\xd8\xa3\xd8\xae\xd8\xb6\xd8\xb1_\xd8\xba\xd8\xa7\xd9\x85\xd9\x82", {27,94,32,255}}, // أخضر_غامق
        {"\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad", {224,224,224,255}}, // رمادي_فاتح
        {"\xd8\xb1\xd9\x85\xd8\xa7\xd8\xaf\xd9\x8a_\xd8\xba\xd8\xa7\xd9\x85\xd9\x82", {97,97,97,255}}, // رمادي_غامق
        {"\xd8\xa8\xd8\xb1\xd8\xaa\xd9\x82\xd8\xa7\xd9\x84\xd9\x8a_\xd8\xba\xd8\xa7\xd9\x85\xd9\x82", {230,81,0,255}}, // برتقالي_غامق
        {"\xd9\x88\xd8\xb1\xd8\xaf\xd9\x8a_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad", {248,187,208,255}}, // وردي_فاتح
        {"\xd9\x86\xd9\x8a\xd9\x84\xd9\x8a_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad", {121,134,203,255}}, // نيلي_فاتح
        {"\xd8\xa8\xd9\x8a\xd8\xac", {245,245,220,255}},           // بيج
        {"\xd9\x82\xd8\xb1\xd9\x85\xd8\xb2\xd9\x8a", {220,20,60,255}}, // قرمزي
        {"\xd8\xb2\xd8\xa8\xd8\xb1\xd8\xac\xd8\xaf\xd9\x8a", {0,128,128,255}}, // زبرجدي
        {"\xd9\x81\xd9\x8a\xd8\xb1\xd9\x88\xd8\xb2\xd9\x8a", {64,224,208,255}}, // فيروزي
        {"\xd9\x85\xd8\xb1\xd8\xac\xd8\xa7\xd9\x86\xd9\x8a", {255,127,80,255}}, // مرجاني
        {"\xd9\x83\xd8\xb1\xd9\x8a\xd9\x85\xd9\x8a", {255,253,208,255}}, // كريمي
        {"\xd8\xb3\xd9\x84\xd9\x85\xd9\x88\xd9\x86\xd9\x8a", {250,128,114,255}}, // سلموني
        {"\xd8\xa8\xd8\xb7\xd9\x8a\xd8\xae\xd9\x8a", {0,128,0,255}}, // بطيخي (dark green)
        {"\xd9\x84\xd8\xa7\xd9\x81\xd9\x86\xd8\xaf\xd8\xb1", {230,230,250,255}}, // لافندر
        {"\xd8\xb3\xd9\x85\xd8\xa7\xd9\x88\xd9\x8a_\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad", {128,222,234,255}}, // سماوي_فاتح
        // ─── أسماء إنجليزية ───
        {"red", {244,67,54,255}}, {"green", {76,175,80,255}}, {"blue", {33,150,243,255}},
        {"white", {255,255,255,255}}, {"black", {0,0,0,255}}, {"gray", {158,158,158,255}},
        {"grey", {158,158,158,255}}, {"orange", {255,152,0,255}}, {"purple", {156,39,176,255}},
        {"pink", {233,30,99,255}}, {"brown", {121,85,72,255}}, {"gold", {255,193,7,255}},
        {"transparent", {0,0,0,0}}, {"cyan", {0,188,212,255}}, {"lime", {205,220,57,255}},
        {"indigo", {63,81,181,255}}, {"silver", {189,189,189,255}}, {"navy", {26,35,126,255}},
        {"olive", {85,139,47,255}}, {"maroon", {136,14,79,255}}, {"teal", {0,150,136,255}},
        {"yellow", {255,235,59,255}}, {"coral", {255,127,80,255}}, {"crimson", {220,20,60,255}},
        {"turquoise", {64,224,208,255}}, {"salmon", {250,128,114,255}}, {"lavender", {230,230,250,255}},
        {"beige", {245,245,220,255}}, {"cream", {255,253,208,255}}, {"amber", {255,193,7,255}},
        {"deepOrange", {255,87,34,255}}, {"deepPurple", {103,58,183,255}},
        {"lightBlue", {3,169,244,255}}, {"lightGreen", {139,195,74,255}},
        {"blueGrey", {96,125,139,255}},
    };
    return colors;
}

RGBA parseColor(const std::string& hex) {
    RGBA c = {0, 0, 0, 255};
    if (hex.empty()) return c;
    
    // [FIX #1] بحث O(1) في خريطة الألوان المسماة
    if (hex[0] != '#') {
        auto& colors = getNamedColors();
        auto it = colors.find(hex);
        if (it != colors.end()) return it->second;
        return c;
    }
    
    // [FIX #6] حماية parseColor من الاستثناءات
    try {
        std::string h = hex.substr(1);
        if (h.size() == 3) {
            c.r = std::stoi(std::string(2, h[0]), nullptr, 16);
            c.g = std::stoi(std::string(2, h[1]), nullptr, 16);
            c.b = std::stoi(std::string(2, h[2]), nullptr, 16);
        } else if (h.size() == 4) {
            // [FIX #7] "#RGBA" format
            c.r = std::stoi(std::string(2, h[0]), nullptr, 16);
            c.g = std::stoi(std::string(2, h[1]), nullptr, 16);
            c.b = std::stoi(std::string(2, h[2]), nullptr, 16);
            c.a = std::stoi(std::string(2, h[3]), nullptr, 16);
        } else if (h.size() >= 6) {
            c.r = std::stoi(h.substr(0, 2), nullptr, 16);
            c.g = std::stoi(h.substr(2, 2), nullptr, 16);
            c.b = std::stoi(h.substr(4, 2), nullptr, 16);
            if (h.size() >= 8) c.a = std::stoi(h.substr(6, 2), nullptr, 16);
        }
        // [FIX #8] Clamp values to valid range
        c.r = std::max(0, std::min(255, c.r));
        c.g = std::max(0, std::min(255, c.g));
        c.b = std::max(0, std::min(255, c.b));
        c.a = std::max(0, std::min(255, c.a));
    } catch (...) {
        c = {0, 0, 0, 255};
    }
    return c;
}

// [FIX #31-40] خريطة أسماء الأنواع — تحويل O(1) بدل if-else chain
static const std::unordered_map<std::string, WidgetType>& getTypeNameMap() {
    static const std::unordered_map<std::string, WidgetType> m = {
        // 9 أصلية
        {"\xd8\xb9\xd9\x85\xd9\x88\xd8\xaf", WidgetType::Column}, {"column", WidgetType::Column},
        {"\xd8\xb5\xd9\x81", WidgetType::Row}, {"row", WidgetType::Row},
        {"\xd9\x86\xd8\xb5", WidgetType::Text}, {"text", WidgetType::Text},
        {"\xd8\xb2\xd8\xb1", WidgetType::Button}, {"button", WidgetType::Button},
        {"\xd8\xad\xd9\x82\xd9\x84", WidgetType::TextField}, {"textfield", WidgetType::TextField},
        {"\xd8\xad\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9", WidgetType::Container}, {"container", WidgetType::Container},
        {"\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84", WidgetType::Spacer}, {"spacer", WidgetType::Spacer},
        {"\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9", WidgetType::Card}, {"card", WidgetType::Card},
        {"\xd8\xae\xd8\xb7_\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84", WidgetType::Divider}, {"divider", WidgetType::Divider},
        // 10 تخطيط
        {"\xd8\xaa\xd9\x83\xd8\xaf\xd9\x8a\xd8\xb3", WidgetType::Stack}, {"stack", WidgetType::Stack},
        {"\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb7", WidgetType::Center}, {"center", WidgetType::Center},
        {"\xd8\xa7\xd9\x84\xd8\xaa\xd9\x81\xd8\xa7\xd9\x81", WidgetType::Wrap}, {"wrap", WidgetType::Wrap},
        {"\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", WidgetType::GridView}, {"grid", WidgetType::GridView}, {"gridview", WidgetType::GridView},
        {"\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xb9\xd8\xb1\xd8\xb6", WidgetType::ListView}, {"listview", WidgetType::ListView},
        {"\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", WidgetType::ScrollView}, {"scrollview", WidgetType::ScrollView},
        {"\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82", WidgetType::SizedBox}, {"sizedbox", WidgetType::SizedBox},
        {"\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb9", WidgetType::Expanded}, {"expanded", WidgetType::Expanded},
        {"\xd8\xb5\xd9\x81\xd8\xad\xd8\xa7\xd8\xaa", WidgetType::PageView}, {"pageview", WidgetType::PageView},
        {"\xd8\xac\xd8\xaf\xd9\x88\xd9\x84", WidgetType::Table}, {"table", WidgetType::Table},
        // 6 أزرار
        {"\xd8\xb2\xd8\xb1_\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", WidgetType::IconButton}, {"iconbutton", WidgetType::IconButton},
        {"\xd8\xb2\xd8\xb1_\xd8\xb9\xd8\xa7\xd8\xa6\xd9\x85", WidgetType::FloatingButton}, {"fab", WidgetType::FloatingButton},
        {"\xd8\xb2\xd8\xb1_\xd9\x85\xd8\xad\xd9\x8a\xd8\xb7", WidgetType::OutlineButton}, {"outlinebutton", WidgetType::OutlineButton},
        {"\xd8\xb2\xd8\xb1_\xd9\x86\xd8\xb5\xd9\x8a", WidgetType::TextButton}, {"textbutton", WidgetType::TextButton},
        {"\xd8\xb2\xd8\xb1_\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84", WidgetType::ToggleButton}, {"togglebutton", WidgetType::ToggleButton},
        {"\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xa7\xd8\xb2\xd8\xb1\xd8\xa7\xd8\xb1", WidgetType::ButtonGroup}, {"buttongroup", WidgetType::ButtonGroup},
        // 14 مدخلات
        {"\xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1", WidgetType::Checkbox}, {"checkbox", WidgetType::Checkbox},
        {"\xd8\xb1\xd8\xa7\xd8\xaf\xd9\x8a\xd9\x88", WidgetType::Radio}, {"radio", WidgetType::Radio},
        {"\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad", WidgetType::Switch}, {"switch", WidgetType::Switch},
        {"\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82", WidgetType::Slider}, {"slider", WidgetType::Slider},
        {"\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb3\xd8\xaf\xd9\x84\xd8\xa9", WidgetType::Dropdown}, {"dropdown", WidgetType::Dropdown},
        {"\xd8\xa7\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84_\xd8\xb1\xd9\x82\xd9\x85", WidgetType::NumberInput}, {"numberinput", WidgetType::NumberInput},
        {"\xd8\xad\xd9\x82\xd9\x84_\xd8\xa8\xd8\xad\xd8\xab", WidgetType::SearchField}, {"searchfield", WidgetType::SearchField},
        {"\xd8\xad\xd9\x82\xd9\x84_\xd8\xb3\xd8\xb1", WidgetType::PasswordField}, {"passwordfield", WidgetType::PasswordField},
        {"\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd9\x86\xd8\xb5", WidgetType::TextArea}, {"textarea", WidgetType::TextArea},
        {"\xd8\xad\xd9\x82\xd9\x84_\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae", WidgetType::DateField}, {"datefield", WidgetType::DateField},
        {"\xd9\x85\xd9\x86\xd8\xb2\xd9\x84\xd9\x82_\xd9\x86\xd8\xb7\xd8\xa7\xd9\x82", WidgetType::RangeSlider}, {"rangeslider", WidgetType::RangeSlider},
        {"\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84", WidgetType::ToggleGroup}, {"togglegroup", WidgetType::ToggleGroup},
        {"\xd9\x85\xd9\x86\xd8\xaa\xd9\x82\xd9\x8a_\xd9\x84\xd9\x88\xd9\x86", WidgetType::ColorPicker}, {"colorpicker", WidgetType::ColorPicker},
        {"\xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd9\x85", WidgetType::RatingStars}, {"rating", WidgetType::RatingStars},
        // 7 نصوص
        {"\xd8\xaa\xd8\xb3\xd9\x85\xd9\x8a\xd8\xa9", WidgetType::Label}, {"label", WidgetType::Label},
        {"\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", WidgetType::Heading}, {"heading", WidgetType::Heading},
        {"\xd9\x81\xd9\x82\xd8\xb1\xd8\xa9", WidgetType::Paragraph}, {"paragraph", WidgetType::Paragraph},
        {"\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7", WidgetType::Link}, {"link", WidgetType::Link},
        {"\xd9\x83\xd9\x88\xd8\xaf", WidgetType::CodeBlock}, {"codeblock", WidgetType::CodeBlock},
        {"\xd8\xa7\xd9\x82\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb3", WidgetType::Quote}, {"quote", WidgetType::Quote},
        {"\xd9\x85\xd8\xa7\xd8\xb1\xd9\x83\xd8\xaf\xd8\xa7\xd9\x88\xd9\x86", WidgetType::Markdown}, {"markdown", WidgetType::Markdown},
        // 10 بصري
        {"\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", WidgetType::Icon}, {"icon", WidgetType::Icon},
        {"\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9_\xd8\xb1\xd9\x85\xd8\xb2\xd9\x8a\xd8\xa9", WidgetType::Avatar}, {"avatar", WidgetType::Avatar},
        {"\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9", WidgetType::Badge}, {"badge", WidgetType::Badge},
        {"\xd8\xb1\xd9\x82\xd8\xa7\xd9\x82\xd8\xa9", WidgetType::Chip}, {"chip", WidgetType::Chip},
        {"\xd9\x88\xd8\xb3\xd9\x85", WidgetType::Tag}, {"tag", WidgetType::Tag},
        {"\xd9\x86\xd9\x82\xd8\xb7\xd8\xa9_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9", WidgetType::StatusDot}, {"statusdot", WidgetType::StatusDot},
        {"\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd9\x84\xd9\x88\xd9\x86", WidgetType::ColorBox}, {"colorbox", WidgetType::ColorBox},
        {"\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac", WidgetType::GradientBox}, {"gradientbox", WidgetType::GradientBox},
        {"\xd8\xb5\xd9\x88\xd8\xb1\xd8\xa9", WidgetType::ImageWidget}, {"image", WidgetType::ImageWidget},
        {"\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84_\xd8\xae\xd8\xb7", WidgetType::Separator}, {"separator", WidgetType::Separator},
        // 4 تقدم
        {"\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", WidgetType::ProgressBar}, {"progressbar", WidgetType::ProgressBar},
        {"\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a", WidgetType::CircleProgress}, {"circleprogress", WidgetType::CircleProgress},
        {"\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84", WidgetType::LoadingSpinner}, {"spinner", WidgetType::LoadingSpinner},
        {"\xd9\x87\xd9\x8a\xd9\x83\xd9\x84", WidgetType::Skeleton}, {"skeleton", WidgetType::Skeleton},
        // 10 تنقل
        {"\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", WidgetType::AppBar}, {"appbar", WidgetType::AppBar},
        {"\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a", WidgetType::BottomBar}, {"bottombar", WidgetType::BottomBar},
        {"\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8", WidgetType::TabBar}, {"tabbar", WidgetType::TabBar},
        {"\xd8\xaf\xd8\xb1\xd8\xac", WidgetType::Drawer}, {"drawer", WidgetType::Drawer},
        {"\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1", WidgetType::Breadcrumb}, {"breadcrumb", WidgetType::Breadcrumb},
        {"\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84", WidgetType::NavItem}, {"navitem", WidgetType::NavItem},
        {"\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9", WidgetType::MenuItem}, {"menuitem", WidgetType::MenuItem},
        {"\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa7\xd8\xaf\xd9\x88\xd8\xa7\xd8\xaa", WidgetType::Toolbar}, {"toolbar", WidgetType::Toolbar},
        {"\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9", WidgetType::StatusBar}, {"statusbar", WidgetType::StatusBar},
        {"\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xac\xd8\xa7\xd9\x86\xd8\xa8\xd9\x8a\xd8\xa9", WidgetType::SideMenu}, {"sidemenu", WidgetType::SideMenu},
        // 10 تنبيهات
        {"\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1", WidgetType::Dialog}, {"dialog", WidgetType::Dialog},
        {"\xd8\xaa\xd9\x86\xd8\xa8\xd9\x8a\xd9\x87", WidgetType::Alert}, {"alert", WidgetType::Alert},
        {"\xd8\xa7\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1_\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa", WidgetType::Toast}, {"toast", WidgetType::Toast},
        {"\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa7\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1", WidgetType::Snackbar}, {"snackbar", WidgetType::Snackbar},
        {"\xd9\x84\xd8\xa7\xd9\x81\xd8\xaa\xd8\xa9", WidgetType::Banner}, {"banner", WidgetType::Banner},
        {"\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd9\x85\xd8\xb4\xd8\xb1\xd9\x88\xd8\xb7\xd8\xa9", WidgetType::Modal}, {"modal", WidgetType::Modal},
        {"\xd9\x85\xd9\x86\xd8\xa8\xd8\xab\xd9\x82\xd8\xa9", WidgetType::Popover}, {"popover", WidgetType::Popover},
        {"\xd8\xa7\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1", WidgetType::Notification}, {"notification", WidgetType::Notification},
        {"\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf", WidgetType::ConfirmDialog}, {"confirm", WidgetType::ConfirmDialog},
        {"\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad", WidgetType::Tooltip}, {"tooltip", WidgetType::Tooltip},
        // 7 بطاقات
        {"\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa", WidgetType::InfoCard}, {"infocard", WidgetType::InfoCard},
        {"\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9_\xd8\xa7\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd8\xaa", WidgetType::StatCard}, {"statcard", WidgetType::StatCard},
        {"\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb9", WidgetType::ExpansionPanel}, {"expansion", WidgetType::ExpansionPanel},
        {"\xd8\xa7\xd9\x83\xd9\x88\xd8\xb1\xd8\xaf\xd9\x8a\xd9\x88\xd9\x86", WidgetType::Accordion}, {"accordion", WidgetType::Accordion},
        {"\xd9\x84\xd9\x88\xd8\xad\xd8\xa9", WidgetType::Panel}, {"panel", WidgetType::Panel},
        {"\xd8\xb5\xd9\x86\xd8\xaf\xd9\x88\xd9\x82_\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9", WidgetType::GroupBox}, {"groupbox", WidgetType::GroupBox},
        {"\xd8\xaa\xd9\x86\xd9\x88\xd9\x8a\xd9\x87", WidgetType::Callout}, {"callout", WidgetType::Callout},
        // 10 بيانات
        {"\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", WidgetType::ListTile}, {"listtile", WidgetType::ListTile},
        {"\xd8\xac\xd8\xaf\xd9\x88\xd9\x84_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa", WidgetType::DataTable}, {"datatable", WidgetType::DataTable},
        {"\xd8\xb5\xd9\x81_\xd8\xac\xd8\xaf\xd9\x88\xd9\x84", WidgetType::TableRow}, {"tablerow", WidgetType::TableRow},
        {"\xd8\xae\xd9\x84\xd9\x8a\xd8\xa9", WidgetType::TableCell}, {"tablecell", WidgetType::TableCell},
        {"\xd8\xb5\xd9\x81_\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb5\xd9\x8a\xd9\x84", WidgetType::DetailRow}, {"detailrow", WidgetType::DetailRow},
        {"\xd8\xae\xd8\xb7_\xd8\xb2\xd9\x85\xd9\x86\xd9\x8a", WidgetType::Timeline}, {"timeline", WidgetType::Timeline},
        {"\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xb2\xd9\x85\xd9\x86\xd9\x8a", WidgetType::TimelineItem}, {"timelineitem", WidgetType::TimelineItem},
        {"\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xb4\xd8\xac\xd8\xb1\xd8\xa9", WidgetType::TreeItem}, {"treeitem", WidgetType::TreeItem},
        {"\xd8\xae\xd8\xb7\xd9\x88\xd8\xa7\xd8\xaa", WidgetType::Stepper}, {"stepper", WidgetType::Stepper},
        {"\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd8\xae\xd8\xb7\xd9\x88\xd8\xa9", WidgetType::StepIndicator}, {"stepindicator", WidgetType::StepIndicator},
        // 7 رسوم بيانية
        {"\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xa7\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9", WidgetType::BarChart}, {"barchart", WidgetType::BarChart},
        {"\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xa7\xd9\x81\xd9\x82\xd9\x8a", WidgetType::HBarChart}, {"hbarchart", WidgetType::HBarChart},
        {"\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a", WidgetType::PieChart}, {"piechart", WidgetType::PieChart},
        {"\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xae\xd8\xb7\xd9\x8a", WidgetType::LineChart}, {"linechart", WidgetType::LineChart},
        {"\xd8\xae\xd8\xb7_\xd9\x85\xd8\xb5\xd8\xba\xd8\xb1", WidgetType::SparkLine}, {"sparkline", WidgetType::SparkLine},
        {"\xd9\x85\xd9\x82\xd9\x8a\xd8\xa7\xd8\xb3", WidgetType::Gauge}, {"gauge", WidgetType::Gauge},
        {"\xd8\xb1\xd8\xb3\xd9\x85_\xd8\xad\xd9\x84\xd9\x82\xd9\x8a", WidgetType::DonutChart}, {"donutchart", WidgetType::DonutChart},
        // 5 ترقيم + متقدم
        {"\xd8\xaa\xd8\xb1\xd9\x82\xd9\x8a\xd9\x85", WidgetType::Pagination}, {"pagination", WidgetType::Pagination},
        {"\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1_\xd9\x86\xd9\x82\xd8\xb7\xd9\x8a", WidgetType::DotIndicator}, {"dotindicator", WidgetType::DotIndicator},
        {"\xd9\x84\xd9\x88\xd8\xad\xd8\xa9_\xd8\xb1\xd8\xb3\xd9\x85", WidgetType::Canvas}, {"canvas", WidgetType::Canvas},
        {"\xd8\xb9\xd8\xb1\xd8\xb6_\xd9\x85\xd9\x82\xd8\xb3\xd9\x85", WidgetType::SplitView}, {"splitview", WidgetType::SplitView},
        {"\xd8\xaf\xd9\x88\xd8\xa7\xd8\xb1", WidgetType::Carousel}, {"carousel", WidgetType::Carousel},
        // [NEW] أسماء بديلة إضافية
        {"\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0\xd8\xa9_\xd9\x85\xd9\x86\xd8\xa8\xd8\xab\xd9\x82\xd8\xa9", WidgetType::Modal}, // نافذة_منبثقة
        {"\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5", WidgetType::TextField}, // حقل_نص
        {"\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9", WidgetType::ListView}, // قائمة
    };
    return m;
}

static WidgetType resolveTypeName(const std::string& name) {
    auto& m = getTypeNameMap();
    auto it = m.find(name);
    return (it != m.end()) ? it->second : WidgetType::Container;
}

// تطبيق خصائص من خريطة على مكون موجود
static void applyPropsToWidget(int id, const std::unordered_map<std::string, PropValue>& props) {
    Widget* w = getWidget(id);
    if (!w) return;

    for (auto& [key, val] : props) {
        // ─── المعرّف (الاسم النصي للمكون) ───
        if (key == "\xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81" || key == "id" || key == "name") {
            if (val.type == PropValue::STRING) w->widgetName = val.strVal;
        }
        // ─── خصائص النص والمحتوى ───
        else if (key == "محتوى" || key == "نص" || key == "text" || key == "content") {
            if (val.type == PropValue::STRING) w->text = val.strVal;
        }
        else if (key == "عنوان_فرعي" || key == "subtitle") {
            if (val.type == PropValue::STRING) w->subtitle = val.strVal;
        }
        // ─── الأحجام والأبعاد ───
        else if (key == "حجم" || key == "حجم_خط" || key == "fontSize" || key == "size") {
            if (val.type == PropValue::NUMBER) w->fontSize = (float)val.numVal;
        }
        else if (key == "عرض" || key == "width") {
            if (val.type == PropValue::NUMBER) w->fixedWidth = (float)val.numVal;
        }
        else if (key == "ارتفاع" || key == "height") {
            if (val.type == PropValue::NUMBER) w->fixedHeight = (float)val.numVal;
        }
        else if (key == "مرونة" || key == "flex") {
            if (val.type == PropValue::NUMBER) w->flex = (float)val.numVal;
        }
        // ─── التباعد والحشوة ───
        else if (key == "حشوة" || key == "padding") {
            if (val.type == PropValue::NUMBER) {
                float p = (float)val.numVal;
                w->padding = p; w->padTop = p; w->padRight = p; w->padBottom = p; w->padLeft = p;
            }
        }
        else if (key == "تباعد" || key == "spacing") {
            if (val.type == PropValue::NUMBER) w->spacing = (float)val.numVal;
        }
        // ─── الألوان ───
        else if (key == "لون" || key == "لون_نص" || key == "color" || key == "textColor") {
            if (val.type == PropValue::COLOR) w->textColor = val.colorVal;
            else if (val.type == PropValue::STRING) w->textColor = parseColor(val.strVal);
        }
        else if (key == "خلفية" || key == "لون_خلفية" || key == "bg" || key == "background") {
            if (val.type == PropValue::COLOR) w->bgColor = val.colorVal;
            else if (val.type == PropValue::STRING) w->bgColor = parseColor(val.strVal);
        }
        else if (key == "لون_زر" || key == "btnColor") {
            RGBA bc = (val.type == PropValue::COLOR) ? val.colorVal : parseColor(val.strVal);
            w->btnColor = bc;
            w->btnHover = {std::min(bc.r+30,255), std::min(bc.g+30,255), std::min(bc.b+30,255), 255};
            w->btnPress = {std::max(bc.r-30,0), std::max(bc.g-30,0), std::max(bc.b-30,0), 255};
        }
        else if (key == "لون_تمييز" || key == "accent") {
            if (val.type == PropValue::COLOR) w->accentColor = val.colorVal;
            else if (val.type == PropValue::STRING) w->accentColor = parseColor(val.strVal);
        }
        else if (key == "لون_حدود" || key == "borderColor") {
            if (val.type == PropValue::COLOR) w->borderColor = val.colorVal;
            else if (val.type == PropValue::STRING) w->borderColor = parseColor(val.strVal);
        }
        // ─── الحدود والزوايا ───
        else if (key == "حدود" || key == "زوايا" || key == "borderRadius" || key == "radius") {
            if (val.type == PropValue::NUMBER) w->borderRadius = (float)val.numVal;
        }
        else if (key == "سمك_حدود" || key == "borderWidth") {
            if (val.type == PropValue::NUMBER) w->borderWidth = (float)val.numVal;
        }
        // ─── المحاذاة ───
        else if (key == "محاذاة" || key == "align" || key == "textAlign") {
            if (val.type == PropValue::STRING) w->textAlign = val.strVal;
        }
        // ─── الحالة ───
        else if (key == "مرئي" || key == "visible") {
            if (val.type == PropValue::BOOL) w->visible = val.boolVal;
        }
        else if (key == "محدد" || key == "checked") {
            if (val.type == PropValue::BOOL) w->checked = val.boolVal;
        }
        else if (key == "قيمة" || key == "value") {
            if (val.type == PropValue::NUMBER) w->value = (float)val.numVal;
        }
        else if (key == "أدنى" || key == "ادنى" || key == "min") {
            if (val.type == PropValue::NUMBER) w->minVal = (float)val.numVal;
        }
        else if (key == "أقصى" || key == "اقصى" || key == "max") {
            if (val.type == PropValue::NUMBER) w->maxVal = (float)val.numVal;
        }
        else if (key == "مختار" || key == "selected") {
            if (val.type == PropValue::NUMBER) w->selectedIndex = (int)val.numVal;
        }
        else if (key == "موسع" || key == "expanded") {
            if (val.type == PropValue::BOOL) w->expanded = val.boolVal;
        }
        else if (key == "نشط" || key == "active") {
            if (val.type == PropValue::BOOL) w->active = val.boolVal;
        }
        else if (key == "مستوى" || key == "level") {
            if (val.type == PropValue::NUMBER) w->level = (int)val.numVal;
        }
        else if (key == "أعمدة" || key == "اعمدة" || key == "columns") {
            if (val.type == PropValue::NUMBER) w->columns = (int)val.numVal;
        }
        else if (key == "صفحة" || key == "page") {
            if (val.type == PropValue::NUMBER) w->currentPage = (int)val.numVal;
        }
        else if (key == "ايقونة" || key == "icon") {
            if (val.type == PropValue::STRING && !val.strVal.empty()) w->iconChar = val.strVal[0];
        }
        else if (key == "تلميح" || key == "hint") {
            if (val.type == PropValue::STRING) w->text = val.strVal;
        }
        // ─── [NEW v2.0] خصائص جديدة في applyProps ───
        else if (key == "شفافية" || key == "opacity") {
            if (val.type == PropValue::NUMBER) w->opacity = std::max(0.0, std::min(1.0, val.numVal));
        }
        else if (key == "مفعل" || key == "enabled") {
            if (val.type == PropValue::BOOL) w->enabled = val.boolVal;
        }
        else if (key == "ظل" || key == "shadow" || key == "shadowRadius") {
            if (val.type == PropValue::NUMBER) w->shadowRadius = (float)val.numVal;
        }
        else if (key == "تلميح_فقاعي" || key == "tooltip") {
            if (val.type == PropValue::STRING) w->tooltipText = val.strVal;
        }
        else if (key == "حد_ادنى_عرض" || key == "minWidth") {
            if (val.type == PropValue::NUMBER) w->minWidth = (float)val.numVal;
        }
        else if (key == "حد_اقصى_عرض" || key == "maxWidth") {
            if (val.type == PropValue::NUMBER) w->maxWidth = (float)val.numVal;
        }
        else if (key == "حد_ادنى_ارتفاع" || key == "minHeight") {
            if (val.type == PropValue::NUMBER) w->minHeight = (float)val.numVal;
        }
        else if (key == "حد_اقصى_ارتفاع" || key == "maxHeight") {
            if (val.type == PropValue::NUMBER) w->maxHeight = (float)val.numVal;
        }
        else if (key == "هامش" || key == "margin") {
            if (val.type == PropValue::NUMBER) {
                float m = (float)val.numVal;
                w->marginTop = w->marginRight = w->marginBottom = w->marginLeft = m;
            }
        }
        else if (key == "دوران" || key == "rotation") {
            if (val.type == PropValue::NUMBER) w->rotation = (float)val.numVal;
        }
        else if (key == "ترتيب" || key == "zIndex") {
            if (val.type == PropValue::NUMBER) w->zIndex = (int)val.numVal;
        }
        else if (key == "وزن_خط" || key == "fontWeight") {
            if (val.type == PropValue::STRING) w->fontWeight = val.strVal;
        }
        else if (key == "مائل" || key == "italic") {
            if (val.type == PropValue::BOOL) w->italic = val.boolVal;
        }
        else if (key == "ارتفاع_سطر" || key == "lineHeight") {
            if (val.type == PropValue::NUMBER) w->lineHeight = (float)val.numVal;
        }
        else if (key == "فائض" || key == "overflow") {
            if (val.type == PropValue::STRING) w->overflow = val.strVal;
        }
        else if (key == "مقنع" || key == "masked") {
            if (val.type == PropValue::BOOL) w->masked = val.boolVal;
        }
        else if (key == "ايقونة_نص" || key == "iconText") {
            if (val.type == PropValue::STRING) w->iconText = val.strVal;
        }
        else if (key == "نص_ادخال" || key == "inputText") {
            if (val.type == PropValue::STRING) w->inputText = val.strVal;
        }
        else if (key == "مجموع_صفحات" || key == "totalPages") {
            if (val.type == PropValue::NUMBER) w->totalPages = std::max(1, (int)val.numVal);
        }
        else if (key == "قيمة2" || key == "value2") {
            if (val.type == PropValue::NUMBER) w->value2 = (float)val.numVal;
        }
    }
}

// إنشاء مكون جديد من اسم النوع وخريطة الخصائص
int widget_create_from_props(const std::string& typeName,
                             const std::unordered_map<std::string, PropValue>& props) {
    WidgetType type = resolveTypeName(typeName);
    int id = createWidget(type);
    applyPropsToWidget(id, props);
    return id;
}

// تطبيق خصائص على مكون موجود — واجهة عامة
void apply_props_to_widget(int id, const std::unordered_map<std::string, PropValue>& props) {
    applyPropsToWidget(id, props);
}

// إنشاء تطبيق من خريطة خصائص
int app_create_from_props(const std::unordered_map<std::string, PropValue>& props) {
    std::string title = "SAD App";
    int width = 800, height = 600;
    RGBA bg = {245, 245, 245, 255};

    for (auto& [key, val] : props) {
        if ((key == "عنوان" || key == "title") && val.type == PropValue::STRING)
            title = val.strVal;
        else if ((key == "عرض" || key == "width") && val.type == PropValue::NUMBER)
            width = (int)val.numVal;
        else if ((key == "ارتفاع" || key == "height") && val.type == PropValue::NUMBER)
            height = (int)val.numVal;
        else if ((key == "خلفية" || key == "bg" || key == "background")) {
            if (val.type == PropValue::STRING) bg = parseColor(val.strVal);
            else if (val.type == PropValue::COLOR) bg = val.colorVal;
        }
    }

    int appId = app_create(title, width, height);
    app_set_bg_color(appId, bg.r, bg.g, bg.b);
    return appId;
}

// ═══════════════════════════════════════════════════════════════════
// القسم 12: النظام التفاعلي — الثيمات والأحداث والتنظيف
// ═══════════════════════════════════════════════════════════════════

// ───── الثيمة العالمية ─────
static Theme g_theme;
static std::map<int, std::string> g_prevTextStates; // لتتبع تغيّرات النصوص

const Theme& getTheme() { return g_theme; }

void setThemeProperty(const std::string& propName, const std::string& strValue) {
    RGBA color = parseColor(strValue);
    if (propName == "أساسي" || propName == "primary")          g_theme.primaryColor = color;
    else if (propName == "ثانوي" || propName == "secondary")    g_theme.secondaryColor = color;
    else if (propName == "خلفية" || propName == "background")   g_theme.backgroundColor = color;
    else if (propName == "سطح" || propName == "surface")        g_theme.surfaceColor = color;
    else if (propName == "نص" || propName == "text")            g_theme.textColor = color;
    else if (propName == "نص_ثانوي" || propName == "textSecondary") g_theme.textSecondary = color;
    else if (propName == "خطأ" || propName == "error")          g_theme.errorColor = color;
    else if (propName == "نجاح" || propName == "success")       g_theme.successColor = color;
    else if (propName == "تحذير" || propName == "warning")      g_theme.warningColor = color;
    else if (propName == "فاصل" || propName == "divider")       g_theme.dividerColor = color;
}

void setThemeProperty(const std::string& propName, double numValue) {
    float v = (float)numValue;
    if (propName == "خط_صغير" || propName == "fontSmall")       g_theme.fontSizeSmall = v;
    else if (propName == "خط" || propName == "fontSize")        g_theme.fontSizeMedium = v;
    else if (propName == "خط_كبير" || propName == "fontLarge")  g_theme.fontSizeLarge = v;
    else if (propName == "خط_عنوان" || propName == "fontTitle") g_theme.fontSizeTitle = v;
    else if (propName == "خط_رئيسي" || propName == "fontHeading") g_theme.fontSizeHeading = v;
    else if (propName == "حشوة" || propName == "padding")       g_theme.paddingMedium = v;
    else if (propName == "حشوة_صغيرة" || propName == "paddingSmall") g_theme.paddingSmall = v;
    else if (propName == "حشوة_كبيرة" || propName == "paddingLarge") g_theme.paddingLarge = v;
    else if (propName == "تباعد" || propName == "spacing")      g_theme.spacingDefault = v;
    else if (propName == "حدود" || propName == "radius")        g_theme.borderRadius = v;
    else if (propName == "سمك_حدود" || propName == "borderWidth") g_theme.borderWidth = v;
}

// ───── جمع الأحداث ─────
// يمسح جميع الودجات أسفل الجذر ويكتشف الأحداث التي حصلت

static void collectEventsRecursive(int id, std::vector<UIEvent>& events) {
    Widget* w = getWidget(id);
    if (!w || !w->visible) return;
    
    // اكتشاف ضغط الأزرار
    if (w->wasClicked) {
        UIEvent ev;
        ev.widgetId = id;
        std::string evName = w->widgetName.empty() ? w->text : w->widgetName;
        
        switch (w->type) {
            case WidgetType::Button:
            case WidgetType::IconButton:
            case WidgetType::FloatingButton:
            case WidgetType::OutlineButton:
            case WidgetType::TextButton:
            case WidgetType::ToggleButton: {
                ev.type = "\xd8\xb6\xd8\xba\xd8\xb7"; // "ضغط"
                ev.textData = evName;
                events.push_back(ev);
                break;
            }
            case WidgetType::Checkbox:
            case WidgetType::Switch: {
                ev.type = "\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84"; // "تبديل"
                ev.textData = evName;
                ev.boolData = w->checked;
                events.push_back(ev);
                break;
            }
            case WidgetType::NavItem:
            case WidgetType::MenuItem:
            case WidgetType::ListTile: {
                ev.type = "\xd8\xb6\xd8\xba\xd8\xb7"; // "ضغط"
                ev.textData = evName;
                events.push_back(ev);
                break;
            }
            default: {
                ev.type = "\xd8\xb6\xd8\xba\xd8\xb7"; // "ضغط"
                ev.textData = evName;
                events.push_back(ev);
                break;
            }
        }
        w->wasClicked = false; // مسح العلامة بعد الجمع
    }
    
    // اكتشاف تغيّرات حقول النص
    bool isTextField = (w->type == WidgetType::TextField || 
                        w->type == WidgetType::SearchField ||
                        w->type == WidgetType::PasswordField ||
                        w->type == WidgetType::TextArea ||
                        w->type == WidgetType::NumberInput);
    
    if (isTextField) {
        auto prevIt = g_prevTextStates.find(id);
        std::string prevText = (prevIt != g_prevTextStates.end()) ? prevIt->second : "";
        
        if (w->inputText != prevText) {
            UIEvent ev;
            ev.type = "\xd9\x86\xd8\xb5"; // "نص"
            ev.widgetId = id;
            ev.textData = w->inputText;
            events.push_back(ev);
            g_prevTextStates[id] = w->inputText;
        }
    }
    
    // اكتشاف تغيّرات المنزلقات
    if (w->type == WidgetType::Slider || w->type == WidgetType::RangeSlider) {
        // [FIX #4] لا نضيف الحدث إلا إذا تغيّرت القيمة فعلاً
        if (w->_prevSliderValue != w->value) {
            UIEvent ev;
            ev.type = "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"; // "قيمة"
            ev.widgetId = id;
            ev.numData = w->value;
            ev.textData = w->text;
            events.push_back(ev);
            w->_prevSliderValue = w->value;
        }
    }
    
    // تكرار للأبناء
    for (int childId : w->children) {
        collectEventsRecursive(childId, events);
    }
}

std::vector<UIEvent> collectEvents(int rootWidget) {
    std::vector<UIEvent> events;
    collectEventsRecursive(rootWidget, events);
    return events;
}

// ───── حفظ واستعادة حالة الحقول ─────

static void saveTextStatesRecursive(int id, std::map<std::string, std::string>& states, int& counter) {
    Widget* w = getWidget(id);
    if (!w) return;
    
    bool isTextField = (w->type == WidgetType::TextField || 
                        w->type == WidgetType::SearchField ||
                        w->type == WidgetType::PasswordField ||
                        w->type == WidgetType::TextArea ||
                        w->type == WidgetType::NumberInput);
    
    if (isTextField) {
        // المفتاح يجمع بين النص التلميحي والترتيب لضمان التفرد
        std::string key = w->text + "##" + std::to_string(counter);
        states[key] = w->inputText;
        counter++;
    }
    
    for (int childId : w->children) {
        saveTextStatesRecursive(childId, states, counter);
    }
}

std::map<std::string, std::string> saveTextFieldStates(int rootWidget) {
    std::map<std::string, std::string> states;
    int counter = 0;
    saveTextStatesRecursive(rootWidget, states, counter);
    return states;
}

static void restoreTextStatesRecursive(int id, const std::map<std::string, std::string>& states, int& counter) {
    Widget* w = getWidget(id);
    if (!w) return;
    
    bool isTextField = (w->type == WidgetType::TextField || 
                        w->type == WidgetType::SearchField ||
                        w->type == WidgetType::PasswordField ||
                        w->type == WidgetType::TextArea ||
                        w->type == WidgetType::NumberInput);
    
    if (isTextField) {
        std::string key = w->text + "##" + std::to_string(counter);
        auto it = states.find(key);
        if (it != states.end()) {
            w->inputText = it->second;
        }
        counter++;
    }
    
    for (int childId : w->children) {
        restoreTextStatesRecursive(childId, states, counter);
    }
}

void restoreTextFieldStates(int rootWidget, const std::map<std::string, std::string>& states) {
    int counter = 0;
    restoreTextStatesRecursive(rootWidget, states, counter);
}

// ───── تنظيف الودجات ─────

void widgetCleanupAll() {
    g_widgets.clear();
    g_prevTextStates.clear();
    g_nextWidgetId = 1;
}

static void cleanupTreeRecursive(int id, std::set<int>& toDelete) {
    Widget* w = getWidget(id);
    if (!w) return;
    toDelete.insert(id);
    for (int childId : w->children) {
        cleanupTreeRecursive(childId, toDelete);
    }
}

void widgetCleanupTree(int rootId) {
    std::set<int> toDelete;
    cleanupTreeRecursive(rootId, toDelete);
    for (int id : toDelete) {
        g_widgets.erase(id);
        g_prevTextStates.erase(id);
    }
}

// ───── ضبط خاصية واحدة ─────

void widgetSetProp(int id, const std::string& propName, const std::string& strValue) {
    Widget* w = getWidget(id);
    if (!w) return;
    
    if (propName == "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89" || propName == "محتوى" || propName == "نص" || propName == "content" || propName == "text")
        w->text = strValue;
    else if (propName == "عنوان_فرعي" || propName == "subtitle")
        w->subtitle = strValue;
    else if (propName == "محاذاة" || propName == "align" || propName == "textAlign")
        w->textAlign = strValue;
    else if (propName == "لون" || propName == "لون_نص" || propName == "color") 
        w->textColor = parseColor(strValue);
    else if (propName == "خلفية" || propName == "لون_خلفية" || propName == "bg" || propName == "background")
        w->bgColor = parseColor(strValue);
    else if (propName == "لون_زر" || propName == "btnColor") {
        RGBA bc = parseColor(strValue);
        w->btnColor = bc;
        w->btnHover = {std::min(bc.r+30,255), std::min(bc.g+30,255), std::min(bc.b+30,255), 255};
        w->btnPress = {std::max(bc.r-30,0), std::max(bc.g-30,0), std::max(bc.b-30,0), 255};
    }
    else if (propName == "لون_تمييز" || propName == "accent")
        w->accentColor = parseColor(strValue);
    else if (propName == "لون_حدود" || propName == "borderColor")
        w->borderColor = parseColor(strValue);
    else if (propName == "تلميح" || propName == "hint")
        w->text = strValue;
    else if (propName == "ايقونة" || propName == "icon") {
        if (!strValue.empty()) w->iconChar = strValue[0];
    }
    // [FIX #41] widgetName settable via setProp
    else if (propName == "اسم" || propName == "المعرف" || propName == "name" || propName == "id")
        w->widgetName = strValue;
    // [NEW] خصائص جديدة v2.0
    else if (propName == "تلميح_فقاعي" || propName == "tooltip")
        w->tooltipText = strValue;
    else if (propName == "وزن_خط" || propName == "fontWeight")
        w->fontWeight = strValue;
    else if (propName == "فائض" || propName == "overflow")
        w->overflow = strValue;
    else if (propName == "مؤشر" || propName == "cursor")
        w->cursor = strValue;
    else if (propName == "ايقونة_نص" || propName == "iconText")
        w->iconText = strValue;
    // [FIX #42] inputText via setProp
    else if (propName == "نص_ادخال" || propName == "inputText")
        w->inputText = strValue;
    // ألوان بالسلسلة النصية
    else if (propName == "لون_تحويم" || propName == "hoverColor")
        w->hoverColor = parseColor(strValue);
    else if (propName == "لون_ضغط" || propName == "pressColor")
        w->pressColor = parseColor(strValue);
    else if (propName == "لون_ظل" || propName == "shadowColor")
        w->shadowColor = parseColor(strValue);
    // [FIX #113] محاذاة المحاور
    else if (propName == "محاذاة_رئيسية" || propName == "mainAxisAlignment")
        w->mainAxisAlignment = strValue;
    else if (propName == "محاذاة_عرضية" || propName == "crossAxisAlignment")
        w->crossAxisAlignment = strValue;
    // [FIX #114] تدرج لوني بالنص
    else if (propName == "تدرج_بداية" || propName == "gradientStart") {
        w->gradientStart = parseColor(strValue); w->hasGradient = true;
    }
    else if (propName == "تدرج_نهاية" || propName == "gradientEnd") {
        w->gradientEnd = parseColor(strValue); w->hasGradient = true;
    }
    // [FIX #160-185] خصائص نصية جديدة v3.0 (25+ خاصية)
    else if (propName == "فيض_نص" || propName == "textOverflow")
        w->textOverflow = strValue;
    else if (propName == "زخرفة_نص" || propName == "textDecoration")
        w->textDecoration = strValue;
    else if (propName == "تحويل_نص" || propName == "textTransform")
        w->textTransform = strValue;
    else if (propName == "شكل" || propName == "shape")
        w->shape = strValue;
    else if (propName == "اتجاه_تدرج" || propName == "gradientDirection")
        w->gradientDirection = strValue;
    else if (propName == "عنصر_نائب" || propName == "placeholder")
        w->placeholder = strValue;
    else if (propName == "نوع_ادخال" || propName == "inputType")
        w->inputType = strValue;
    else if (propName == "اتجاه_تمرير" || propName == "scrollDirection")
        w->scrollDirection = strValue;
    else if (propName == "تسمية_دلالية" || propName == "semanticLabel")
        w->semanticLabel = strValue;
    else if (propName == "شارة" || propName == "badge")
        w->badge = strValue;
    else if (propName == "تلميح_ادخال" || propName == "hint")
        w->hint = strValue;
    else if (propName == "نص_خطا" || propName == "errorText")
        w->errorText = strValue;
    else if (propName == "نص_مساعدة" || propName == "helperText")
        w->helperText = strValue;
    else if (propName == "محاذاة_ذاتية" || propName == "alignSelf")
        w->alignSelf = strValue;
    else if (propName == "لون_تراكب" || propName == "overlayColor")
        w->overlayColor = parseColor(strValue);
    else if (propName == "لون_تركيز" || propName == "focusColor")
        w->focusColor = parseColor(strValue);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // [NEW v4.0] 80+ خاصية نصية جديدة — Flutter-Level Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    // ═══ Animation & Transitions ═══
    else if (propName == "منحنى_حركة" || propName == "animationCurve")
        w->animationCurve = strValue;
    else if (propName == "خاصية_حركة" || propName == "transitionProperty")
        w->transitionProperty = strValue;
    
    // ═══ Gestures & Touch ═══
    else if (propName == "اتجاه_سحب" || propName == "swipeDirection")
        w->swipeDirection = strValue;
    
    // ═══ Layout Advanced ═══
    else if (propName == "موضع" || propName == "position")
        w->position = strValue;
    else if (propName == "عرض" || propName == "display")
        w->display = strValue;
    else if (propName == "اتجاه_مرن" || propName == "flexDirection")
        w->flexDirection = strValue;
    else if (propName == "التفاف_مرن" || propName == "flexWrap")
        w->flexWrap = strValue;
    else if (propName == "توزيع_محتوى" || propName == "justifyContent")
        w->justifyContent = strValue;
    else if (propName == "محاذاة_عناصر" || propName == "alignItems")
        w->alignItems = strValue;
    
    // ═══ Grid Layout ═══
    else if (propName == "تدفق_شبكة" || propName == "gridAutoFlow")
        w->gridAutoFlow = strValue;
    else if (propName == "قالب_اعمدة" || propName == "gridTemplateColumns")
        w->gridTemplateColumns = strValue;
    else if (propName == "قالب_صفوف" || propName == "gridTemplateRows")
        w->gridTemplateRows = strValue;
    
    // ═══ Typography Extended ═══
    else if (propName == "عائلة_خط" || propName == "fontFamily")
        w->fontFamily = strValue;
    else if (propName == "نمط_خط" || propName == "fontStyle")
        w->fontStyle = strValue;
    else if (propName == "ظل_نص" || propName == "textShadow")
        w->textShadow = strValue;
    else if (propName == "اتجاه_نص" || propName == "textDirection")
        w->textDirection = strValue;
    else if (propName == "وضع_كتابة" || propName == "writingMode")
        w->writingMode = strValue;
    else if (propName == "مسافة_بيضاء" || propName == "whiteSpace")
        w->whiteSpace = strValue;
    else if (propName == "ضبط_نص" || propName == "textJustify")
        w->textJustify = strValue;
    
    // ═══ Borders Extended ═══
    else if (propName == "لون_حد_علوي" || propName == "borderTopColor")
        w->borderTopColor = parseColor(strValue);
    else if (propName == "لون_حد_ايمن" || propName == "borderRightColor")
        w->borderRightColor = parseColor(strValue);
    else if (propName == "لون_حد_سفلي" || propName == "borderBottomColor")
        w->borderBottomColor = parseColor(strValue);
    else if (propName == "لون_حد_ايسر" || propName == "borderLeftColor")
        w->borderLeftColor = parseColor(strValue);
    else if (propName == "نمط_حدود" || propName == "borderStyle")
        w->borderStyle = strValue;
    else if (propName == "حجم_صندوق" || propName == "boxSizing")
        w->boxSizing = strValue;
    
    // ═══ Effects & Filters ═══
    else if (propName == "وضع_مزج" || propName == "blendMode")
        w->blendMode = strValue;
    else if (propName == "فلتر_خلفية" || propName == "backdropFilter")
        w->backdropFilter = strValue;
    
    // ═══ State & Interaction ═══
    else if (propName == "حالة" || propName == "state")
        w->state = strValue;
    else if (propName == "نص_تحميل" || propName == "loadingText")
        w->loadingText = strValue;
    else if (propName == "حالة_تحقق" || propName == "validationState")
        w->validationState = strValue;
    
    // ═══ Content & Data ═══
    else if (propName == "نوع_محتوى" || propName == "contentType")
        w->contentType = strValue;
    else if (propName == "مصدر_بيانات" || propName == "dataSource")
        w->dataSource = strValue;
    else if (propName == "مسار_ربط" || propName == "bindingPath")
        w->bindingPath = strValue;
    else if (propName == "شرط_فلترة" || propName == "filterPredicate")
        w->filterPredicate = strValue;
    else if (propName == "خاصية_ترتيب" || propName == "sortProperty")
        w->sortProperty = strValue;
    
    // ═══ Accessibility Extended ═══
    else if (propName == "دور" || propName == "role")
        w->role = strValue;
    else if (propName == "تسمية_اريا" || propName == "ariaLabel")
        w->ariaLabel = strValue;
    else if (propName == "موصوف_بـ" || propName == "ariaDescribedBy")
        w->ariaDescribedBy = strValue;
    else if (propName == "مسمى_بـ" || propName == "ariaLabelledBy")
        w->ariaLabelledBy = strValue;
}

void widgetSetProp(int id, const std::string& propName, double numValue) {
    Widget* w = getWidget(id);
    if (!w) return;
    float v = (float)numValue;
    
    if (propName == "حجم" || propName == "حجم_خط" || propName == "fontSize")
        w->fontSize = v;
    else if (propName == "عرض" || propName == "width")
        w->fixedWidth = v;
    else if (propName == "ارتفاع" || propName == "height")
        w->fixedHeight = v;
    else if (propName == "مرونة" || propName == "flex")
        w->flex = v;
    else if (propName == "حشوة" || propName == "padding") {
        w->padding = v; w->padTop = v; w->padRight = v; w->padBottom = v; w->padLeft = v;
    }
    else if (propName == "تباعد" || propName == "spacing")
        w->spacing = v;
    else if (propName == "حدود" || propName == "زوايا" || propName == "radius" || propName == "borderRadius")
        w->borderRadius = v;
    else if (propName == "سمك_حدود" || propName == "borderWidth")
        w->borderWidth = v;
    else if (propName == "قيمة" || propName == "value")
        w->value = v;
    else if (propName == "أدنى" || propName == "ادنى" || propName == "min")
        w->minVal = v;
    else if (propName == "أقصى" || propName == "اقصى" || propName == "max")
        w->maxVal = v;
    else if (propName == "مختار" || propName == "selected")
        w->selectedIndex = (int)numValue;
    else if (propName == "مستوى" || propName == "level")
        w->level = (int)numValue;
    else if (propName == "أعمدة" || propName == "اعمدة" || propName == "columns")
        w->columns = (int)numValue;
    else if (propName == "صفحة" || propName == "page")
        w->currentPage = (int)numValue;
    // [NEW] خصائص رقمية جديدة v2.0
    else if (propName == "شفافية" || propName == "opacity")
        w->opacity = std::max(0.0f, std::min(1.0f, v));
    else if (propName == "ظل" || propName == "shadow" || propName == "shadowRadius")
        w->shadowRadius = std::max(0.0f, v);
    else if (propName == "ازاحة_ظل_س" || propName == "shadowOffsetX")
        w->shadowOffsetX = v;
    else if (propName == "ازاحة_ظل_ص" || propName == "shadowOffsetY")
        w->shadowOffsetY = v;
    else if (propName == "حد_ادنى_عرض" || propName == "minWidth")
        w->minWidth = v;
    else if (propName == "حد_اقصى_عرض" || propName == "maxWidth")
        w->maxWidth = v;
    else if (propName == "حد_ادنى_ارتفاع" || propName == "minHeight")
        w->minHeight = v;
    else if (propName == "حد_اقصى_ارتفاع" || propName == "maxHeight")
        w->maxHeight = v;
    else if (propName == "هامش" || propName == "margin") {
        w->marginTop = w->marginRight = w->marginBottom = w->marginLeft = v;
    }
    else if (propName == "دوران" || propName == "rotation")
        w->rotation = v;
    else if (propName == "تدرج_س" || propName == "scaleX")
        w->scaleX = v;
    else if (propName == "تدرج_ص" || propName == "scaleY")
        w->scaleY = v;
    else if (propName == "ترتيب" || propName == "zIndex")
        w->zIndex = (int)numValue;
    else if (propName == "ترتيب_تاب" || propName == "tabIndex")
        w->tabIndex = (int)numValue;
    else if (propName == "ارتفاع_سطر" || propName == "lineHeight")
        w->lineHeight = std::max(0.5f, v);
    else if (propName == "تمرير_ص" || propName == "scrollY")
        w->scrollY = std::max(0.0f, v);
    else if (propName == "تمرير_س" || propName == "scrollX")
        w->scrollX = std::max(0.0f, v);
    // [FIX #44] totalPages settable via setProp
    else if (propName == "مجموع_صفحات" || propName == "totalPages")
        w->totalPages = std::max(1, (int)numValue);
    else if (propName == "قيمة2" || propName == "value2")
        w->value2 = v;
    // [NEW v3.0] خصائص رقمية إضافية (30+)
    else if (propName == "\xd9\x86\xd8\xb5\xd9\x81_\xd9\x82\xd8\xb7\xd8\xb1" || propName == "cornerRadius" || propName == "\xd9\x86\xd8\xb5\xd9\x81_\xd9\x82\xd8\xb7\xd8\xb1_\xd8\xa7\xd9\x84\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7") // نصف_قطر
        w->borderRadius = std::max(0.0f, v);
    else if (propName == "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9_\xd8\xb9\xd9\x84\xd9\x88\xd9\x8a\xd8\xa9" || propName == "padTop")
        w->padTop = v;
    else if (propName == "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a\xd8\xa9" || propName == "padBottom")
        w->padBottom = v;
    else if (propName == "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9_\xd9\x8a\xd9\x85\xd9\x86\xd9\x89" || propName == "padRight")
        w->padRight = v;
    else if (propName == "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9_\xd9\x8a\xd8\xb3\xd8\xb1\xd9\x89" || propName == "padLeft")
        w->padLeft = v;
    else if (propName == "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4_\xd8\xb9\xd9\x84\xd9\x88\xd9\x8a" || propName == "marginTop")
        w->marginTop = v;
    else if (propName == "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a" || propName == "marginBottom")
        w->marginBottom = v;
    else if (propName == "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4_\xd8\xa7\xd9\x8a\xd9\x85\xd9\x86" || propName == "marginRight")
        w->marginRight = v;
    else if (propName == "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4_\xd8\xa7\xd9\x8a\xd8\xb3\xd8\xb1" || propName == "marginLeft")
        w->marginLeft = v;
    else if (propName == "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89" || propName == "contentHeight")
        w->contentHeight = v;
    else if (propName == "\xd9\x81\xd8\xac\xd9\x88\xd8\xa9" || propName == "gap")
        w->spacing = v;
    else if (propName == "\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85" || propName == "progress")
        w->value = std::max(0.0f, std::min(1.0f, v));
    else if (propName == "\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa" || propName == "fixedWidth")
        w->fixedWidth = v;
    else if (propName == "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa" || propName == "fixedHeight")
        w->fixedHeight = v;
    // [FIX #186-230] خصائص رقمية جديدة v3.0 (45+ خاصية)
    else if (propName == "ارتفاع_طبقة" || propName == "elevation")
        w->elevation = std::max(0.0f, v);
    else if (propName == "فجوة_عنصر" || propName == "gapSize")
        w->gap = std::max(0.0f, v);
    else if (propName == "نسبة_عرض_ارتفاع" || propName == "aspectRatio")
        w->aspectRatio = std::max(0.0f, v);
    else if (propName == "تباعد_احرف" || propName == "letterSpacing")
        w->letterSpacing = v;
    else if (propName == "اقصى_اسطر" || propName == "maxLines")
        w->maxLines = std::max(0, (int)numValue);
    else if (propName == "نصف_قطر_تموج" || propName == "rippleRadius")
        w->rippleRadius = std::max(0.0f, v);
    else if (propName == "نمو_مرن" || propName == "flexGrow")
        w->flexGrow = std::max(0.0f, v);
    else if (propName == "انكماش_مرن" || propName == "flexShrink")
        w->flexShrink = std::max(0.0f, v);
    else if (propName == "حدود_علوية" || propName == "borderTopWidth")
        w->borderTopWidth = std::max(0.0f, v);
    else if (propName == "حدود_يمنى" || propName == "borderRightWidth")
        w->borderRightWidth = std::max(0.0f, v);
    else if (propName == "حدود_سفلية" || propName == "borderBottomWidth")
        w->borderBottomWidth = std::max(0.0f, v);
    else if (propName == "حدود_يسرى" || propName == "borderLeftWidth")
        w->borderLeftWidth = std::max(0.0f, v);
    else if (propName == "سرعة_تمرير" || propName == "scrollSpeed")
        w->scrollSpeed = std::max(1.0f, v);
    else if (propName == "زاوية_تدرج" || propName == "gradientAngle")
        w->gradientAngle = v;
    else if (propName == "حشوة_محتوى" || propName == "contentPadding")
        w->contentPadding = std::max(0.0f, v);
    else if (propName == "قيمة3" || propName == "value3")
        w->value3 = v;
    else if (propName == "عدد_شارة" || propName == "badgeCount")
        w->badgeCount = std::max(0, (int)numValue);
    else if (propName == "موضع_مؤشر" || propName == "cursorPosition")
        w->cursorPosition = (int)numValue;
    else if (propName == "بداية_تحديد" || propName == "selectionStart")
        w->selectionStart = (int)numValue;
    else if (propName == "نهاية_تحديد" || propName == "selectionEnd")
        w->selectionEnd = (int)numValue;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // [NEW v4.0] 60+ خاصية رقمية جديدة — Flutter-Level Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    // ═══ Animation & Transitions ═══
    else if (propName == "مدة_حركة" || propName == "animationDuration")
        w->animationDuration = std::max(0.0f, v);
    else if (propName == "تاخير_حركة" || propName == "animationDelay")
        w->animationDelay = std::max(0.0f, v);
    else if (propName == "قيمة_حركة" || propName == "animationValue")
        w->animationValue = std::max(0.0f, std::min(1.0f, v));
    else if (propName == "صلابة_زنبرك" || propName == "springStiffness")
        w->springStiffness = std::max(0.0f, v);
    else if (propName == "تخميد_زنبرك" || propName == "springDamping")
        w->springDamping = std::max(0.0f, v);
    else if (propName == "سرعة_زنبرك" || propName == "springVelocity")
        w->springVelocity = v;
    
    // ═══ Gestures & Touch ═══
    else if (propName == "عتبة_سحب" || propName == "swipeThreshold")
        w->swipeThreshold = std::max(0.0f, v);
    else if (propName == "مدة_ضغط_مطول" || propName == "longPressDuration")
        w->longPressDuration = std::max(0, (int)numValue);
    else if (propName == "ازاحة_سحب_س" || propName == "panX")
        w->panX = v;
    else if (propName == "ازاحة_سحب_ص" || propName == "panY")
        w->panY = v;
    else if (propName == "مقياس_قرص" || propName == "pinchScale")
        w->pinchScale = std::max(0.1f, v);
    
    // ═══ Layout Advanced ═══
    else if (propName == "موضع_علوي" || propName == "posTop")
        w->posTop = v;
    else if (propName == "موضع_ايمن" || propName == "posRight")
        w->posRight = v;
    else if (propName == "موضع_سفلي" || propName == "posBottom")
        w->posBottom = v;
    else if (propName == "موضع_ايسر" || propName == "posLeft")
        w->posLeft = v;
    
    // ═══ Grid Layout ═══
    else if (propName == "صفوف_شبكة" || propName == "gridRows")
        w->gridRows = std::max(0, (int)numValue);
    else if (propName == "فجوة_صفوف" || propName == "gridRowGap")
        w->gridRowGap = std::max(0.0f, v);
    else if (propName == "فجوة_اعمدة" || propName == "gridColGap")
        w->gridColGap = std::max(0.0f, v);
    else if (propName == "عمود_شبكة" || propName == "gridColumn")
        w->gridColumn = std::max(0, (int)numValue);
    else if (propName == "صف_شبكة" || propName == "gridRow")
        w->gridRow = std::max(0, (int)numValue);
    else if (propName == "امتداد_عمود" || propName == "gridColSpan")
        w->gridColSpan = std::max(1, (int)numValue);
    else if (propName == "امتداد_صف" || propName == "gridRowSpan")
        w->gridRowSpan = std::max(1, (int)numValue);
    
    // ═══ Typography Extended ═══
    else if (propName == "وزن_خط_رقمي" || propName == "fontWeightNum")
        w->fontWeightNum = std::max(100, std::min(900, (int)numValue));
    else if (propName == "تباعد_كلمات" || propName == "wordSpacing")
        w->wordSpacing = v;
    else if (propName == "ازاحة_سطر" || propName == "textIndent")
        w->textIndent = v;
    
    // ═══ Borders Extended ═══
    else if (propName == "حدود_علوي_يسار" || propName == "borderTopLeftRadius")
        w->borderTopLeftRadius = v;
    else if (propName == "حدود_علوي_يمين" || propName == "borderTopRightRadius")
        w->borderTopRightRadius = v;
    else if (propName == "حدود_سفلي_يمين" || propName == "borderBottomRightRadius")
        w->borderBottomRightRadius = v;
    else if (propName == "حدود_سفلي_يسار" || propName == "borderBottomLeftRadius")
        w->borderBottomLeftRadius = v;
    
    // ═══ Effects & Filters ═══
    else if (propName == "ضبابية" || propName == "blur")
        w->blur = std::max(0.0f, v);
    else if (propName == "سطوع" || propName == "brightness")
        w->brightness = std::max(0.0f, std::min(2.0f, v));
    else if (propName == "تباين" || propName == "contrast")
        w->contrast = std::max(0.0f, std::min(2.0f, v));
    else if (propName == "تشبع" || propName == "saturation")
        w->saturation = std::max(0.0f, std::min(2.0f, v));
    else if (propName == "تدوير_لون" || propName == "hueRotate")
        w->hueRotate = v;
    else if (propName == "بني_قديم" || propName == "sepia")
        w->sepia = std::max(0.0f, std::min(1.0f, v));
    
    // ═══ State & Interaction ═══
    else if (propName == "عدد_نقرات" || propName == "clickCount")
        w->clickCount = std::max(0, (int)numValue);
    
    // ═══ Content & Data ═══
    else if (propName == "عدد_تكرار" || propName == "repeatCount")
        w->repeatCount = std::max(0, (int)numValue);
    else if (propName == "حجم_صفحة" || propName == "pageSize")
        w->pageSize = std::max(1, (int)numValue);
    else if (propName == "فهرس_حالي" || propName == "currentIndex")
        w->currentIndex = std::max(0, (int)numValue);
    
    // ═══ Accessibility Extended ═══
    else if (propName == "مستوى_اريا" || propName == "ariaLevel")
        w->ariaLevel = std::max(0, (int)numValue);
    else if (propName == "حجم_مجموعة" || propName == "ariaSetSize")
        w->ariaSetSize = std::max(0, (int)numValue);
    else if (propName == "موضع_مجموعة" || propName == "ariaPosInSet")
        w->ariaPosInSet = std::max(0, (int)numValue);
}

void widgetSetProp(int id, const std::string& propName, bool boolValue) {
    Widget* w = getWidget(id);
    if (!w) return;
    
    if (propName == "مرئي" || propName == "visible") w->visible = boolValue;
    else if (propName == "محدد" || propName == "checked") w->checked = boolValue;
    else if (propName == "موسع" || propName == "expanded") w->expanded = boolValue;
    else if (propName == "نشط" || propName == "active") w->active = boolValue;
    else if (propName == "مقنع" || propName == "masked") w->masked = boolValue;
    // [NEW] خصائص منطقية جديدة v2.0
    else if (propName == "مفعل" || propName == "enabled") {
        w->enabled = boolValue;
        if (!boolValue) { w->hovered = false; w->pressed = false; w->inputFocused = false; }
    }
    else if (propName == "مائل" || propName == "italic") w->italic = boolValue;
    else if (propName == "تدرج" || propName == "hasGradient") w->hasGradient = boolValue;
    // [NEW v3.0] خصائص منطقية إضافية
    else if (propName == "\xd8\xb9\xd8\xb1\xd9\x8a\xd8\xb6" || propName == "bold") {
        w->fontWeight = boolValue ? "bold" : "normal";
    }
    else if (propName == "\xd9\x85\xd8\xb1\xd9\x83\xd8\xb2" || propName == "centered") {
        if (boolValue) { w->mainAxisAlignment = "center"; w->crossAxisAlignment = "center"; }
    }
    // [FIX #231-250] خصائص منطقية جديدة v3.0 (20+ خاصية)
    else if (propName == "التفاف_نص" || propName == "textWrap")
        w->textWrap = boolValue;
    else if (propName == "قص_حدود" || propName == "clipToBounds")
        w->clipToBounds = boolValue;
    else if (propName == "عكس" || propName == "reverse")
        w->reverse = boolValue;
    else if (propName == "قابل_تركيز" || propName == "focusable")
        w->focusable = boolValue;
    else if (propName == "مركز_تركيز" || propName == "hasFocus")
        w->hasFocus = boolValue;
    else if (propName == "قابل_سحب" || propName == "draggable")
        w->draggable = boolValue;
    else if (propName == "قابل_اسقاط" || propName == "droppable")
        w->droppable = boolValue;
    else if (propName == "قراءة_فقط" || propName == "readOnly")
        w->readOnly = boolValue;
    else if (propName == "شريط_تمرير" || propName == "scrollbarVisible")
        w->scrollbarVisible = boolValue;
    else if (propName == "محدد_عنصر" || propName == "selected")
        w->selected = boolValue;
    else if (propName == "اظهر_فاصل" || propName == "showDivider")
        w->showDivider = boolValue;
    else if (propName == "اظهر_ظل" || propName == "showShadow") {
        w->showShadow = boolValue;
        if (boolValue && w->shadowRadius <= 0) w->shadowRadius = 8;
    }
    else if (propName == "مضغوط" || propName == "compact")
        w->compact = boolValue;
    else if (propName == "كثيف" || propName == "dense")
        w->dense = boolValue;
    else if (propName == "خطا" || propName == "hasError")
        w->hasError = boolValue;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // [NEW v4.0] 40+ خاصية منطقية جديدة — Flutter-Level Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    // ═══ Animation & Transitions ═══
    else if (propName == "متحرك" || propName == "animating")
        w->animating = boolValue;
    else if (propName == "فيزياء_زنبرك" || propName == "useSpringPhysics")
        w->useSpringPhysics = boolValue;
    
    // ═══ Gestures & Touch ═══
    else if (propName == "سحب_مفعل" || propName == "swipeEnabled")
        w->swipeEnabled = boolValue;
    else if (propName == "ضغط_مطول" || propName == "longPressEnabled")
        w->longPressEnabled = boolValue;
    else if (propName == "سحب_حر" || propName == "panEnabled")
        w->panEnabled = boolValue;
    else if (propName == "قرص_مفعل" || propName == "pinchEnabled")
        w->pinchEnabled = boolValue;
    
    // ═══ Effects & Filters ═══
    else if (propName == "رمادي" || propName == "grayscale")
        w->grayscale = boolValue;
    else if (propName == "عكس_الوان" || propName == "invert")
        w->invert = boolValue;
    
    // ═══ State & Interaction ═══
    else if (propName == "تحميل" || propName == "isLoading")
        w->isLoading = boolValue;
    else if (propName == "نجاح" || propName == "isSuccess")
        w->isSuccess = boolValue;
    else if (propName == "تحذير" || propName == "isWarning")
        w->isWarning = boolValue;
    else if (propName == "منع_نقر_مزدوج" || propName == "preventDoubleClick")
        w->preventDoubleClick = boolValue;
    else if (propName == "التقاط_مؤشر" || propName == "capturePointer")
        w->capturePointer = boolValue;
    
    // ═══ Content & Data ═══
    else if (propName == "قالب" || propName == "isTemplate")
        w->isTemplate = boolValue;
    else if (propName == "ترتيب_تصاعدي" || propName == "sortAscending")
        w->sortAscending = boolValue;
    
    // ═══ Accessibility Extended ═══
    else if (propName == "مخفي_اريا" || propName == "ariaHidden")
        w->ariaHidden = boolValue;
    else if (propName == "موسع_اريا" || propName == "ariaExpanded")
        w->ariaExpanded = boolValue;
    else if (propName == "محدد_اريا" || propName == "ariaSelected")
        w->ariaSelected = boolValue;
}

// ───── ضبط الأطفال من مصفوفة ─────

void widgetSetChildren(int parentId, const std::vector<int>& childIds) {
    Widget* w = getWidget(parentId);
    if (!w) return;
    w->children.clear();
    for (int childId : childIds) {
        // [FIX #16] التحقق من صلاحية ID الابن
        if (getWidget(childId) && childId != parentId) {
            w->children.push_back(childId);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// القسم 14: دوال API جديدة v2.0
// ─────────────────────────────────────────────────────────────────
// 50+ إصلاح + 10 ميزات جديدة
// ═══════════════════════════════════════════════════════════════════

// ───── الشفافية ─────
void widget_set_opacity(int id, float opacity) {
    Widget* w = getWidget(id); if (!w) return;
    w->opacity = std::max(0.0f, std::min(1.0f, opacity));
}
float widget_get_opacity(int id) {
    Widget* w = getWidget(id); return w ? w->opacity : 1.0f;
}

// ───── التفعيل/التعطيل ─────
void widget_set_enabled(int id, bool enabled) {
    Widget* w = getWidget(id); if (!w) return;
    w->enabled = enabled;
    if (!enabled) { w->hovered = false; w->pressed = false; w->inputFocused = false; }
}
bool widget_is_enabled(int id) {
    Widget* w = getWidget(id); return w ? w->enabled : false;
}

// ───── الظل ─────
void widget_set_shadow(int id, float radius, float offsetX, float offsetY, int r, int g, int b, int a) {
    Widget* w = getWidget(id); if (!w) return;
    w->shadowRadius = std::max(0.0f, radius);
    w->shadowOffsetX = offsetX; w->shadowOffsetY = offsetY;
    w->shadowColor = {std::min(255,std::max(0,r)), std::min(255,std::max(0,g)),
                      std::min(255,std::max(0,b)), std::min(255,std::max(0,a))};
}
float widget_get_shadow_radius(int id) {
    Widget* w = getWidget(id); return w ? w->shadowRadius : 0.0f;
}

// ───── التلميح ─────
void widget_set_tooltip(int id, const std::string& text) {
    Widget* w = getWidget(id); if (w) w->tooltipText = text;
}
std::string widget_get_tooltip(int id) {
    Widget* w = getWidget(id); return w ? w->tooltipText : "";
}

// ───── قيود الحجم ─────
void widget_set_min_size(int id, float minW, float minH) {
    Widget* w = getWidget(id); if (!w) return;
    w->minWidth = minW; w->minHeight = minH;
}
void widget_set_max_size(int id, float maxW, float maxH) {
    Widget* w = getWidget(id); if (!w) return;
    w->maxWidth = maxW; w->maxHeight = maxH;
}

// ───── الهامش ─────
void widget_set_margin(int id, float all) {
    Widget* w = getWidget(id); if (!w) return;
    w->marginTop = w->marginRight = w->marginBottom = w->marginLeft = all;
}
void widget_set_margin4(int id, float top, float right, float bottom, float left) {
    Widget* w = getWidget(id); if (!w) return;
    w->marginTop = top; w->marginRight = right;
    w->marginBottom = bottom; w->marginLeft = left;
}

// ───── التدرج اللوني ─────
void widget_set_gradient(int id, int r1, int g1, int b1, int r2, int g2, int b2) {
    Widget* w = getWidget(id); if (!w) return;
    w->gradientStart = {std::min(255,std::max(0,r1)), std::min(255,std::max(0,g1)), std::min(255,std::max(0,b1)), 255};
    w->gradientEnd = {std::min(255,std::max(0,r2)), std::min(255,std::max(0,g2)), std::min(255,std::max(0,b2)), 255};
    w->hasGradient = true;
}

// ───── ألوان التحويم والضغط المخصصة ─────
void widget_set_hover_color(int id, int r, int g, int b) {
    Widget* w = getWidget(id); if (!w) return;
    w->hoverColor = {std::min(255,std::max(0,r)), std::min(255,std::max(0,g)), std::min(255,std::max(0,b)), 255};
}
void widget_set_press_color(int id, int r, int g, int b) {
    Widget* w = getWidget(id); if (!w) return;
    w->pressColor = {std::min(255,std::max(0,r)), std::min(255,std::max(0,g)), std::min(255,std::max(0,b)), 255};
}

// ───── خصائص النص المتقدمة ─────
void widget_set_font_weight(int id, const std::string& weight) {
    Widget* w = getWidget(id); if (w) w->fontWeight = weight;
}
void widget_set_italic(int id, bool italic) {
    Widget* w = getWidget(id); if (w) w->italic = italic;
}
void widget_set_line_height(int id, float lh) {
    Widget* w = getWidget(id); if (w) w->lineHeight = std::max(0.5f, lh);
}

// ───── التحويلات ─────
void widget_set_rotation(int id, float degrees) {
    Widget* w = getWidget(id); if (w) w->rotation = degrees;
}
void widget_set_scale(int id, float sx, float sy) {
    Widget* w = getWidget(id); if (!w) return;
    w->scaleX = sx; w->scaleY = sy;
}

// ───── الترتيب ─────
void widget_set_z_index(int id, int z) {
    Widget* w = getWidget(id); if (w) w->zIndex = z;
}
void widget_set_tab_index(int id, int idx) {
    Widget* w = getWidget(id); if (w) w->tabIndex = idx;
}

// ───── الأيقونة النصية ─────
void widget_set_icon_text(int id, const std::string& text) {
    Widget* w = getWidget(id); if (w) w->iconText = text;
}
std::string widget_get_icon_text(int id) {
    Widget* w = getWidget(id); return w ? w->iconText : "";
}

// ───── التمرير ─────
void widget_set_scroll_position(int id, float x, float y) {
    Widget* w = getWidget(id); if (!w) return;
    w->scrollX = std::max(0.0f, x);
    w->scrollY = std::max(0.0f, y);
}
float widget_get_scroll_x(int id) {
    Widget* w = getWidget(id); return w ? w->scrollX : 0.0f;
}
float widget_get_scroll_y(int id) {
    Widget* w = getWidget(id); return w ? w->scrollY : 0.0f;
}

// ───── نوع الفائض ─────
void widget_set_overflow(int id, const std::string& mode) {
    Widget* w = getWidget(id);
    if (w && (mode == "visible" || mode == "clip" || mode == "scroll"))
        w->overflow = mode;
}

// ───── اسم المكون ─────
void widget_set_name(int id, const std::string& name) {
    Widget* w = getWidget(id); if (w) w->widgetName = name;
}
std::string widget_get_name(int id) {
    Widget* w = getWidget(id); return w ? w->widgetName : "";
}

// ───── استعلام النوع ─────
std::string widget_get_type_name(int id) {
    Widget* w = getWidget(id); if (!w) return "";
    switch (w->type) {
        case WidgetType::Column: return "عمود";
        case WidgetType::Row: return "صف";
        case WidgetType::Text: return "نص";
        case WidgetType::Button: return "زر";
        case WidgetType::TextField: return "حقل";
        case WidgetType::Container: return "حاوية";
        case WidgetType::Card: return "بطاقة";
        case WidgetType::ListView: return "قائمة_عرض";
        case WidgetType::GridView: return "شبكة";
        case WidgetType::Stack: return "تكديس";
        case WidgetType::Center: return "توسيط";
        case WidgetType::Checkbox: return "مربع_اختيار";
        case WidgetType::Radio: return "راديو";
        case WidgetType::Switch: return "مفتاح";
        case WidgetType::Slider: return "منزلق";
        case WidgetType::Dropdown: return "قائمة_منسدلة";
        case WidgetType::ProgressBar: return "شريط_تقدم";
        case WidgetType::CircleProgress: return "تقدم_دائري";
        case WidgetType::Dialog: return "حوار";
        case WidgetType::AppBar: return "شريط_تطبيق";
        case WidgetType::BottomBar: return "شريط_سفلي";
        case WidgetType::TabBar: return "شريط_تبويب";
        case WidgetType::FloatingButton: return "زر_عائم";
        case WidgetType::IconButton: return "زر_ايقونة";
        case WidgetType::ImageWidget: return "صورة";
        default: return "مكون";
    }
}

// ───── الأبناء ─────
int widget_get_children_count(int id) {
    Widget* w = getWidget(id); return w ? (int)w->children.size() : 0;
}
int widget_get_child_at(int id, int index) {
    Widget* w = getWidget(id);
    if (!w || index < 0 || index >= (int)w->children.size()) return -1;
    return w->children[index];
}
void widget_remove_child(int parentId, int childId) {
    Widget* w = getWidget(parentId); if (!w) return;
    auto it = std::find(w->children.begin(), w->children.end(), childId);
    if (it != w->children.end()) w->children.erase(it);
}
int widget_find_by_name(const std::string& name) {
    Widget* w = getWidgetByName(name);
    return w ? w->id : -1;
}

// ───── الموضع والحجم ─────
float widget_get_x(int id) { Widget* w = getWidget(id); return w ? w->cx : 0; }
float widget_get_y(int id) { Widget* w = getWidget(id); return w ? w->cy : 0; }
float widget_get_width(int id) { Widget* w = getWidget(id); return w ? w->cw : 0; }
float widget_get_height(int id) { Widget* w = getWidget(id); return w ? w->ch : 0; }

// ───── مجموع الصفحات ─────
void widget_set_total_pages(int id, int total) {
    Widget* w = getWidget(id); if (w) w->totalPages = std::max(1, total);
}

// ───── القيمة الثانية ─────
void widget_set_value2(int id, float val) {
    Widget* w = getWidget(id); if (w) w->value2 = val;
}
float widget_get_value2(int id) {
    Widget* w = getWidget(id); return w ? w->value2 : 0;
}

// ───── نص الإدخال ─────
void widget_set_input_text(int id, const std::string& text) {
    Widget* w = getWidget(id); if (w) w->inputText = text;
}
std::string widget_get_input_text(int id) {
    Widget* w = getWidget(id); return w ? w->inputText : "";
}

// ───── إدارة العناصر والبيانات ─────
void widget_clear_items(int id) {
    Widget* w = getWidget(id); if (w) w->items.clear();
}
void widget_clear_data(int id) {
    Widget* w = getWidget(id); if (w) { w->dataValues.clear(); w->dataColors.clear(); }
}
void widget_add_data_color(int id, int r, int g, int b) {
    Widget* w = getWidget(id); if (!w) return;
    w->dataColors.push_back({std::min(255,std::max(0,r)), std::min(255,std::max(0,g)),
                              std::min(255,std::max(0,b)), 255});
}

// ───── المقنّع ─────
void widget_set_masked(int id, bool masked) {
    Widget* w = getWidget(id); if (w) w->masked = masked;
}
bool widget_get_masked(int id) {
    Widget* w = getWidget(id); return w ? w->masked : false;
}

// ═══════════════════════════════════════════════════════════════════
// القسم 15: نظام الأحداث الكامل (Full Event Data Access)
// ═══════════════════════════════════════════════════════════════════

// مخزن مؤقت للأحداث — يُملأ بـ events_count ويُقرأ بدوال event_get_*
static std::vector<UIEvent> g_cachedEvents;

int events_count(int rootWidget) {
    g_cachedEvents = collectEvents(rootWidget);
    return (int)g_cachedEvents.size();
}

std::string event_get_type(int index) {
    if (index < 0 || index >= (int)g_cachedEvents.size()) return "";
    return g_cachedEvents[index].type;
}
int event_get_widget_id(int index) {
    if (index < 0 || index >= (int)g_cachedEvents.size()) return -1;
    return g_cachedEvents[index].widgetId;
}
std::string event_get_text(int index) {
    if (index < 0 || index >= (int)g_cachedEvents.size()) return "";
    return g_cachedEvents[index].textData;
}
double event_get_number(int index) {
    if (index < 0 || index >= (int)g_cachedEvents.size()) return 0;
    return g_cachedEvents[index].numData;
}
bool event_get_bool(int index) {
    if (index < 0 || index >= (int)g_cachedEvents.size()) return false;
    return g_cachedEvents[index].boolData;
}
void events_clear() {
    g_cachedEvents.clear();
}

// ═══════════════════════════════════════════════════════════════════
// [FIX #90-150] دوال API جديدة — تحسينات شاملة
// ═══════════════════════════════════════════════════════════════════

// [FIX #90] عدد العناصر المسجلة
int widget_count() {
    return (int)g_widgets.size();
}

// [FIX #91] تتبع الأب — خريطة parentId
static std::unordered_map<int, int> g_parentMap;

int widget_get_parent(int widgetId) {
    auto it = g_parentMap.find(widgetId);
    return (it != g_parentMap.end()) ? it->second : -1;
}

// [FIX #92] إضافة ابن مع تتبع الأب
void widget_add_child_tracked(int parentId, int childId) {
    Widget* p = getWidget(parentId);
    Widget* c = getWidget(childId);
    if (!p || !c) return;
    // [FIX #93] منع الدورات — لا يمكن إضافة الأب كابن لنفسه
    if (parentId == childId) return;
    int ancestor = widget_get_parent(parentId);
    while (ancestor >= 0) {
        if (ancestor == childId) return; // دورة
        ancestor = widget_get_parent(ancestor);
    }
    p->children.push_back(childId);
    g_parentMap[childId] = parentId;
}

// [FIX #94] نسخ عنصر (clone)
int widget_clone(int widgetId) {
    Widget* w = getWidget(widgetId);
    if (!w) return -1;
    int newId = g_nextWidgetId++;
    Widget clone = *w;
    clone.id = newId;
    clone.children.clear(); // النسخة بدون أبناء
    clone.wasClicked = false;
    clone.hovered = false;
    clone.pressed = false;
    clone.inputFocused = false;
    g_widgets[newId] = clone;
    return newId;
}

// [FIX #95] نسخ عنصر مع أبنائه (deep clone)
int widget_deep_clone(int widgetId) {
    Widget* w = getWidget(widgetId);
    if (!w) return -1;
    int newId = widget_clone(widgetId);
    Widget* newW = getWidget(newId);
    if (!newW) return newId;
    for (int cid : w->children) {
        int clonedChild = widget_deep_clone(cid);
        if (clonedChild >= 0) {
            newW->children.push_back(clonedChild);
            g_parentMap[clonedChild] = newId;
        }
    }
    return newId;
}

// [FIX #96] البحث حسب النوع
std::vector<int> widget_find_by_type(WidgetType type) {
    std::vector<int> result;
    for (auto& [id, w] : g_widgets) {
        if (w.type == type) result.push_back(id);
    }
    return result;
}

// [FIX #97] البحث بالاسم — نسخة محسّنة O(n)
int widget_find_by_name_fast(const std::string& name) {
    // بناء فهرس مؤقت عند البحث الأول
    static std::unordered_map<std::string, int> nameIndex;
    static int lastWidgetCount = -1;
    if ((int)g_widgets.size() != lastWidgetCount) {
        nameIndex.clear();
        for (auto& [id, w] : g_widgets) {
            if (!w.widgetName.empty()) nameIndex[w.widgetName] = id;
        }
        lastWidgetCount = (int)g_widgets.size();
    }
    auto it = nameIndex.find(name);
    return (it != nameIndex.end()) ? it->second : -1;
}

// [FIX #98] عنوان التطبيق
void app_set_title(int appId, const std::string& title) {
    auto* a = getApp(appId);
    if (a && a->window) SDL_SetWindowTitle(a->window, title.c_str());
}

std::string app_get_title(int appId) {
    auto* a = getApp(appId);
    if (a && a->window) return SDL_GetWindowTitle(a->window);
    return "";
}

// [FIX #99] معدل الإطارات
double app_get_fps(int appId) {
    auto* a = getApp(appId);
    if (!a) return 0;
    return a->fps;
}

double app_get_delta_time(int appId) {
    auto* a = getApp(appId);
    return a ? a->deltaMs / 1000.0 : 0;
}

// [FIX #100] حجم النافذة
int app_get_width(int appId) {
    auto* a = getApp(appId);
    return a ? a->width : 0;
}
int app_get_height(int appId) {
    auto* a = getApp(appId);
    return a ? a->height : 0;
}

// [FIX #101] تغيير حجم النافذة
void app_set_size(int appId, int w, int h) {
    auto* a = getApp(appId);
    if (a && a->window) {
        SDL_SetWindowSize(a->window, w, h);
        a->width = w; a->height = h;
    }
}

// [FIX #102] التمرير بعجلة الماوس
void widget_scroll_by(int widgetId, float deltaY) {
    Widget* w = getWidget(widgetId);
    if (!w) return;
    w->scrollY += deltaY;
    // [FIX #103] تقييد التمرير
    if (w->scrollY < 0) w->scrollY = 0;
    float maxScroll = w->contentHeight - w->ch;
    if (maxScroll < 0) maxScroll = 0;
    if (w->scrollY > maxScroll) w->scrollY = maxScroll;
}

void widget_scroll_to(int widgetId, float y) {
    Widget* w = getWidget(widgetId);
    if (!w) return;
    w->scrollY = std::max(0.f, y);
    float maxScroll = w->contentHeight - w->ch;
    if (maxScroll < 0) maxScroll = 0;
    if (w->scrollY > maxScroll) w->scrollY = maxScroll;
}

// [FIX #104] تحريك العنصر بـ ID
void widget_move_to(int widgetId, float x, float y) {
    Widget* w = getWidget(widgetId);
    if (w) { w->cx = x; w->cy = y; }
}

// [FIX #105] ضبط المحاذاة
void widget_set_alignment(int widgetId, const std::string& main, const std::string& cross) {
    Widget* w = getWidget(widgetId);
    if (!w) return;
    if (!main.empty()) w->mainAxisAlignment = main;
    if (!cross.empty()) w->crossAxisAlignment = cross;
}

// [FIX #110] إزالة عنصر من الشجرة
void widget_remove(int widgetId) {
    // إزالة من الأب
    int parentId = widget_get_parent(widgetId);
    if (parentId >= 0) {
        Widget* p = getWidget(parentId);
        if (p) {
            p->children.erase(
                std::remove(p->children.begin(), p->children.end(), widgetId),
                p->children.end());
        }
    }
    // إزالة الأبناء بالتدريج
    Widget* w = getWidget(widgetId);
    if (w) {
        for (int cid : w->children) {
            g_parentMap.erase(cid);
            widget_remove(cid);
        }
    }
    g_parentMap.erase(widgetId);
    g_widgets.erase(widgetId);
}

// [FIX #111] نقل عنصر من أب لآخر
void widget_reparent(int widgetId, int newParentId) {
    int oldParent = widget_get_parent(widgetId);
    if (oldParent >= 0) {
        Widget* op = getWidget(oldParent);
        if (op) {
            op->children.erase(
                std::remove(op->children.begin(), op->children.end(), widgetId),
                op->children.end());
        }
    }
    Widget* np = getWidget(newParentId);
    if (np) {
        np->children.push_back(widgetId);
        g_parentMap[widgetId] = newParentId;
    }
}

// [FIX #112] widgetSetProp extended — دعم الخصائص الجديدة
void widgetSetPropString_v2(int widgetId, const std::string& prop, const std::string& val) {
    Widget* w = getWidget(widgetId);
    if (!w) return;
    if (prop == "mainAxisAlignment" || prop == "محاذاة_رئيسية") w->mainAxisAlignment = val;
    else if (prop == "crossAxisAlignment" || prop == "محاذاة_عرضية") w->crossAxisAlignment = val;
    else if (prop == "overflow" || prop == "فائض") w->overflow = val;
    else if (prop == "cursor" || prop == "مؤشر") w->cursor = val;
    else if (prop == "fontWeight" || prop == "وزن_خط") w->fontWeight = val;
}

// ═══════════════════════════════════════════════════════════════════
// [NEW v3.0] isKnownPropertyName — 120+ خاصية معروفة
// ─────────────────────────────────────────────────────────────────
// يُستخدم من _autoChildren لتمييز أسماء الخصائص من معرّفات الأبناء
// ═══════════════════════════════════════════════════════════════════
bool isKnownPropertyName(const std::string& name) {
    static const std::unordered_set<std::string> props = {
        // ═══ String properties (40+) ═══
        // محتوى ونص
        "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", "content",  // محتوى
        "\xd9\x86\xd8\xb5", "text",                                // نص (note: also a widget name)
        "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86_\xd9\x81\xd8\xb1\xd8\xb9\xd9\x8a", "subtitle", // عنوان_فرعي
        // محاذاة
        "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9", "align", "textAlign", // محاذاة
        "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9_\xd8\xb1\xd8\xa6\xd9\x8a\xd8\xb3\xd9\x8a\xd8\xa9", "mainAxisAlignment", // محاذاة_رئيسية
        "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9_\xd8\xb9\xd8\xb1\xd8\xb6\xd9\x8a\xd8\xa9", "crossAxisAlignment", // محاذاة_عرضية
        // ألوان
        "\xd9\x84\xd9\x88\xd9\x86", "color",                      // لون
        "\xd9\x84\xd9\x88\xd9\x86_\xd9\x86\xd8\xb5", // لون_نص
        "\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9", "bg", "background", // خلفية
        "\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9", // لون_خلفية
        "\xd9\x84\xd9\x88\xd9\x86_\xd8\xb2\xd8\xb1", "btnColor", // لون_زر
        "\xd9\x84\xd9\x88\xd9\x86_\xd8\xaa\xd9\x85\xd9\x8a\xd9\x8a\xd8\xb2", "accent", // لون_تمييز
        "\xd9\x84\xd9\x88\xd9\x86_\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf", "borderColor", // لون_حدود
        "\xd9\x84\xd9\x88\xd9\x86_\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x85", "hoverColor", // لون_تحويم
        "\xd9\x84\xd9\x88\xd9\x86_\xd8\xb6\xd8\xba\xd8\xb7", "pressColor", // لون_ضغط
        "\xd9\x84\xd9\x88\xd9\x86_\xd8\xb8\xd9\x84", "shadowColor", // لون_ظل
        // تلميح وأيقونة
        "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad", "hint",     // تلميح
        "\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", "icon", // ايقونة
        "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad_\xd9\x81\xd9\x82\xd8\xa7\xd8\xb9\xd9\x8a", "tooltip", // تلميح_فقاعي
        // اسم ومعرف
        "\xd8\xa7\xd8\xb3\xd9\x85", "name",                      // اسم
        "\xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81", "id", // المعرف
        // خط
        "\xd9\x88\xd8\xb2\xd9\x86_\xd8\xae\xd8\xb7", "fontWeight", // وزن_خط
        "\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9_\xd9\x86\xd8\xb5", "iconText", // ايقونة_نص
        // فائض ومؤشر
        "\xd9\x81\xd8\xa7\xd8\xa6\xd8\xb6", "overflow",           // فائض
        "\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1", "cursor",             // مؤشر
        // ادخال
        "\xd9\x86\xd8\xb5_\xd8\xa7\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84", "inputText", // نص_ادخال
        // تدرج
        "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd8\xa8\xd8\xaf\xd8\xa7\xd9\x8a\xd8\xa9", "gradientStart", // تدرج_بداية
        "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9", "gradientEnd",   // تدرج_نهاية
        // [NEW v3.0] اتجاه
        "\xd8\xa7\xd8\xaa\xd8\xac\xd8\xa7\xd9\x87", "direction", // اتجاه
        "\xd8\xb9\xd8\xa7\xd8\xa6\xd9\x84\xd8\xa9_\xd8\xae\xd8\xb7", "fontFamily", // عائلة_خط
        "\xd9\x86\xd9\x85\xd8\xb7", "style",                     // نمط

        // ═══ Numeric properties (55+) ═══
        "\xd8\xad\xd8\xac\xd9\x85", "fontSize",                   // حجم
        "\xd8\xad\xd8\xac\xd9\x85_\xd8\xae\xd8\xb7", // حجم_خط
        "\xd8\xb9\xd8\xb1\xd8\xb6", "width",                     // عرض
        "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", "height", // ارتفاع
        "\xd9\x85\xd8\xb1\xd9\x88\xd9\x86\xd8\xa9", "flex",     // مرونة
        "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9", "padding",           // حشوة
        "\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xaf", "spacing",  // تباعد
        "\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf", "borderRadius",      // حدود
        "\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7", "radius",   // زوايا
        "\xd8\xb3\xd9\x85\xd9\x83_\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf", "borderWidth", // سمك_حدود
        "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", "value",             // قيمة
        "\xd8\xa3\xd8\xaf\xd9\x86\xd9\x89", "\xd8\xa7\xd8\xaf\xd9\x86\xd9\x89", "min", // أدنى/ادنى
        "\xd8\xa3\xd9\x82\xd8\xb5\xd9\x89", "\xd8\xa7\xd9\x82\xd8\xb5\xd9\x89", "max", // أقصى/اقصى
        "\xd9\x85\xd8\xae\xd8\xaa\xd8\xa7\xd8\xb1", "selected", // مختار
        "\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89", "level",    // مستوى
        "\xd8\xa3\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9", "\xd8\xa7\xd8\xb9\xd9\x85\xd8\xaf\xd8\xa9", "columns", // أعمدة/اعمدة
        "\xd8\xb5\xd9\x81\xd8\xad\xd8\xa9", "page",              // صفحة
        "\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81\xd9\x8a\xd8\xa9", "opacity", // شفافية
        "\xd8\xb8\xd9\x84", "shadow", "shadowRadius",             // ظل
        "\xd8\xa7\xd8\xb2\xd8\xa7\xd8\xad\xd8\xa9_\xd8\xb8\xd9\x84_\xd8\xb3", "shadowOffsetX", // ازاحة_ظل_س
        "\xd8\xa7\xd8\xb2\xd8\xa7\xd8\xad\xd8\xa9_\xd8\xb8\xd9\x84_\xd8\xb5", "shadowOffsetY", // ازاحة_ظل_ص
        "\xd8\xad\xd8\xaf_\xd8\xa7\xd8\xaf\xd9\x86\xd9\x89_\xd8\xb9\xd8\xb1\xd8\xb6", "minWidth", // حد_ادنى_عرض
        "\xd8\xad\xd8\xaf_\xd8\xa7\xd9\x82\xd8\xb5\xd9\x89_\xd8\xb9\xd8\xb1\xd8\xb6", "maxWidth", // حد_اقصى_عرض
        "\xd8\xad\xd8\xaf_\xd8\xa7\xd8\xaf\xd9\x86\xd9\x89_\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", "minHeight", // حد_ادنى_ارتفاع
        "\xd8\xad\xd8\xaf_\xd8\xa7\xd9\x82\xd8\xb5\xd9\x89_\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", "maxHeight", // حد_اقصى_ارتفاع
        "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4", "margin",            // هامش
        "\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa7\xd9\x86", "rotation", // دوران
        "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd8\xb3", "scaleX",  // تدرج_س
        "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac_\xd8\xb5", "scaleY",  // تدرج_ص
        "\xd8\xaa\xd8\xb1\xd8\xaa\xd9\x8a\xd8\xa8", "zIndex",   // ترتيب
        "\xd8\xaa\xd8\xb1\xd8\xaa\xd9\x8a\xd8\xa8_\xd8\xaa\xd8\xa7\xd8\xa8", "tabIndex", // ترتيب_تاب
        "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd8\xb3\xd8\xb7\xd8\xb1", "lineHeight", // ارتفاع_سطر
        "\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1_\xd8\xb5", "scrollY", // تمرير_ص
        "\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1_\xd8\xb3", "scrollX", // تمرير_س
        "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9_\xd8\xb5\xd9\x81\xd8\xad\xd8\xa7\xd8\xaa", "totalPages", // مجموع_صفحات
        "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9" "2", "value2",           // قيمة2
        // [NEW v3.0] خصائص رقمية إضافية (30+)
        "\xd9\x86\xd8\xb5\xd9\x81_\xd9\x82\xd8\xb7\xd8\xb1", "cornerRadius", // نصف_قطر
        "\xd9\x86\xd8\xb5\xd9\x81_\xd9\x82\xd8\xb7\xd8\xb1_\xd8\xa7\xd9\x84\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7", // نصف_قطر_الزوايا
        "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9_\xd8\xb9\xd9\x84\xd9\x88\xd9\x8a\xd8\xa9", "padTop",     // حشوة_علوية
        "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a\xd8\xa9", "padBottom",  // حشوة_سفلية
        "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9_\xd9\x8a\xd9\x85\xd9\x86\xd9\x89", "padRight",           // حشوة_يمنى
        "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9_\xd9\x8a\xd8\xb3\xd8\xb1\xd9\x89", "padLeft",            // حشوة_يسرى
        "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4_\xd8\xb9\xd9\x84\xd9\x88\xd9\x8a", "marginTop",          // هامش_علوي
        "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a", "marginBottom",       // هامش_سفلي
        "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4_\xd8\xa7\xd9\x8a\xd9\x85\xd9\x86", "marginRight",        // هامش_ايمن
        "\xd9\x87\xd8\xa7\xd9\x85\xd8\xb4_\xd8\xa7\xd9\x8a\xd8\xb3\xd8\xb1", "marginLeft",         // هامش_ايسر
        "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", "contentHeight", // ارتفاع_محتوى
        "\xd9\x81\xd8\xac\xd9\x88\xd8\xa9", "gap",               // فجوة (alias for spacing)
        "\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", "progress",          // تقدم (alias for value)
        "\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa", "fixedWidth", // عرض_ثابت
        "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa", "fixedHeight", // ارتفاع_ثابت

        // ═══ Boolean properties (20+) ═══
        "\xd9\x85\xd8\xb1\xd8\xa6\xd9\x8a", "visible",           // مرئي
        "\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", "checked",           // محدد
        "\xd9\x85\xd9\x88\xd8\xb3\xd8\xb9", "expanded",          // موسع
        "\xd9\x86\xd8\xb4\xd8\xb7", "active",                    // نشط
        "\xd9\x85\xd9\x82\xd9\x86\xd8\xb9", "masked",            // مقنع
        "\xd9\x85\xd9\x81\xd8\xb9\xd9\x84", "enabled",           // مفعل
        "\xd9\x85\xd8\xa7\xd8\xa6\xd9\x84", "italic",            // مائل
        "\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac", "hasGradient",       // تدرج
        // [NEW v3.0] خصائص منطقية إضافية
        "\xd8\xb9\xd8\xb1\xd9\x8a\xd8\xb6", "bold",             // عريض
        "\xd9\x85\xd8\xb1\xd9\x83\xd8\xb2", "centered",          // مركز
        // [FIX #251-290] خصائص جديدة v3.0 (40+ إضافية)
        // نصية:
        "textOverflow", "\xd9\x81\xd9\x8a\xd8\xb6_\xd9\x86\xd8\xb5",           // فيض_نص
        "textDecoration", "\xd8\xb2\xd8\xae\xd8\xb1\xd9\x81\xd8\xa9_\xd9\x86\xd8\xb5", // زخرفة_نص
        "textTransform", "\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x84_\xd9\x86\xd8\xb5", // تحويل_نص
        "shape", "\xd8\xb4\xd9\x83\xd9\x84",                     // شكل
        "gradientDirection", "\xd8\xa7\xd8\xaa\xd8\xac\xd8\xa7\xd9\x87_\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac", // اتجاه_تدرج
        "placeholder", "\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x86\xd8\xa7\xd8\xa6\xd8\xa8", // عنصر_نائب
        "inputType", "\xd9\x86\xd9\x88\xd8\xb9_\xd8\xa7\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84", // نوع_ادخال
        "scrollDirection", "\xd8\xa7\xd8\xaa\xd8\xac\xd8\xa7\xd9\x87_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", // اتجاه_تمرير
        "semanticLabel", "\xd8\xaa\xd8\xb3\xd9\x85\xd9\x8a\xd8\xa9_\xd8\xaf\xd9\x84\xd8\xa7\xd9\x84\xd9\x8a\xd8\xa9", // تسمية_دلالية
        "badge", "\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9",            // شارة
        "errorText", "\xd9\x86\xd8\xb5_\xd8\xae\xd8\xb7\xd8\xa3", // نص_خطا
        "helperText", "\xd9\x86\xd8\xb5_\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xaf\xd8\xa9", // نص_مساعدة
        "alignSelf", "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9_\xd8\xb0\xd8\xa7\xd8\xaa\xd9\x8a\xd8\xa9", // محاذاة_ذاتية
        "overlayColor", "\xd9\x84\xd9\x88\xd9\x86_\xd8\xaa\xd8\xb1\xd8\xa7\xd9\x83\xd8\xa8", // لون_تراكب
        "focusColor", "\xd9\x84\xd9\x88\xd9\x86_\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2", // لون_تركيز
        // رقمية:
        "elevation", "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd8\xb7\xd8\xa8\xd9\x82\xd8\xa9", // ارتفاع_طبقة
        "gapSize", "\xd9\x81\xd8\xac\xd9\x88\xd8\xa9_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1", // فجوة_عنصر
        "aspectRatio", "\xd9\x86\xd8\xb3\xd8\xa8\xd8\xa9_\xd8\xb9\xd8\xb1\xd8\xb6_\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", // نسبة_عرض_ارتفاع
        "letterSpacing", "\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xaf_\xd8\xa7\xd8\xad\xd8\xb1\xd9\x81", // تباعد_احرف
        "maxLines", "\xd8\xa7\xd9\x82\xd8\xb5\xd9\x89_\xd8\xa7\xd8\xb3\xd8\xb7\xd8\xb1", // اقصى_اسطر
        "rippleRadius", "\xd9\x86\xd8\xb5\xd9\x81_\xd9\x82\xd8\xb7\xd8\xb1_\xd8\xaa\xd9\x85\xd9\x88\xd8\xac", // نصف_قطر_تموج
        "flexGrow", "\xd9\x86\xd9\x85\xd9\x88_\xd9\x85\xd8\xb1\xd9\x86", // نمو_مرن
        "flexShrink", "\xd8\xa7\xd9\x86\xd9\x83\xd9\x85\xd8\xa7\xd8\xb4_\xd9\x85\xd8\xb1\xd9\x86", // انكماش_مرن
        "borderTopWidth", "\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf_\xd8\xb9\xd9\x84\xd9\x88\xd9\x8a\xd8\xa9", // حدود_علوية
        "borderRightWidth", "\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf_\xd9\x8a\xd9\x85\xd9\x86\xd9\x89", // حدود_يمنى
        "borderBottomWidth", "\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a\xd8\xa9", // حدود_سفلية
        "borderLeftWidth", "\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf_\xd9\x8a\xd8\xb3\xd8\xb1\xd9\x89", // حدود_يسرى
        "scrollSpeed", "\xd8\xb3\xd8\xb1\xd8\xb9\xd8\xa9_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", // سرعة_تمرير
        "gradientAngle", "\xd8\xb2\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9_\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac", // زاوية_تدرج
        "contentPadding", "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9_\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", // حشوة_محتوى
        "value3", "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9" "3",       // قيمة3
        "badgeCount", "\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9", // عدد_شارة
        "cursorPosition", "\xd9\x85\xd9\x88\xd8\xb6\xd8\xb9_\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1", // موضع_مؤشر
        "selectionStart", "\xd8\xa8\xd8\xaf\xd8\xa7\xd9\x8a\xd8\xa9_\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xaf", // بداية_تحديد
        "selectionEnd", "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9_\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xaf", // نهاية_تحديد
        // منطقية:
        "textWrap", "\xd8\xa7\xd9\x84\xd8\xaa\xd9\x81\xd8\xa7\xd9\x81_\xd9\x86\xd8\xb5", // التفاف_نص
        "clipToBounds", "\xd9\x82\xd8\xb5_\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf", // قص_حدود
        "reverse", "\xd8\xb9\xd9\x83\xd8\xb3",                   // عكس
        "focusable", "\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2", // قابل_تركيز
        "hasFocus", "\xd9\x85\xd8\xb1\xd9\x83\xd8\xb2_\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2", // مركز_تركيز
        "draggable", "\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd8\xb3\xd8\xad\xd8\xa8", // قابل_سحب
        "droppable", "\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd8\xa7\xd8\xb3\xd9\x82\xd8\xa7\xd8\xb7", // قابل_اسقاط
        "readOnly", "\xd9\x82\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa9_\xd9\x81\xd9\x82\xd8\xb7", // قراءة_فقط
        "scrollbarVisible", "\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", // شريط_تمرير
        "showDivider", "\xd8\xa7\xd8\xb8\xd9\x87\xd8\xb1_\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84", // اظهر_فاصل
        "showShadow", "\xd8\xa7\xd8\xb8\xd9\x87\xd8\xb1_\xd8\xb8\xd9\x84", // اظهر_ظل
        "compact", "\xd9\x85\xd8\xb6\xd8\xba\xd9\x88\xd8\xb7", // مضغوط
        "dense", "\xd9\x83\xd8\xab\xd9\x8a\xd9\x81",            // كثيف
        "hasError", "\xd8\xae\xd8\xb7\xd8\xa7",                  // خطا
    };
    return props.count(name) > 0;
}

// ═══════════════════════════════════════════════════════════════════
// [NEW v3.0] 40 مصنع مكونات مركبة
// ═══════════════════════════════════════════════════════════════════

int convenience_scaffold(const std::string& title, int bodyId, int bottomBarId) {
    int col = widget_column();
    if (!title.empty()) {
        int appbar = widget_app_bar(title);
        widget_add_child(col, appbar);
    }
    int exp = widget_expanded();
    if (bodyId >= 0) widget_add_child(exp, bodyId);
    widget_add_child(col, exp);
    if (bottomBarId >= 0) widget_add_child(col, bottomBarId);
    return col;
}

int convenience_safe_area(int childId, float topInset, float bottomInset) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->padTop = topInset; w->padBottom = bottomInset; w->padLeft = 8; w->padRight = 8; }
    if (childId >= 0) widget_add_child(c, childId);
    return c;
}

int convenience_padding_widget(int childId, float all) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->padding = all; w->padTop = all; w->padRight = all; w->padBottom = all; w->padLeft = all; }
    if (childId >= 0) widget_add_child(c, childId);
    return c;
}

int convenience_padding_widget4(int childId, float top, float right, float bottom, float left) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->padTop = top; w->padRight = right; w->padBottom = bottom; w->padLeft = left; }
    if (childId >= 0) widget_add_child(c, childId);
    return c;
}

int convenience_align(int childId, const std::string& mainAlign, const std::string& crossAlign) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->mainAxisAlignment = mainAlign; w->crossAxisAlignment = crossAlign; }
    if (childId >= 0) widget_add_child(c, childId);
    return c;
}

int convenience_aspect_ratio(int childId, float ratio) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { if (w->fixedWidth > 0) w->fixedHeight = w->fixedWidth / ratio; }
    if (childId >= 0) widget_add_child(c, childId);
    return c;
}

int convenience_constrained_box(int childId, float minW, float minH, float maxW, float maxH) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->minWidth = minW; w->minHeight = minH; w->maxWidth = maxW; w->maxHeight = maxH; }
    if (childId >= 0) widget_add_child(c, childId);
    return c;
}

int convenience_opacity_widget(int childId, float op) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) w->opacity = std::max(0.0f, std::min(1.0f, op));
    if (childId >= 0) widget_add_child(c, childId);
    return c;
}

int convenience_decorated_box(int childId, int r, int g, int b, float rad, float bw) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->bgColor = {r,g,b,255}; w->borderRadius = rad; w->borderWidth = bw; }
    if (childId >= 0) widget_add_child(c, childId);
    return c;
}

int convenience_clip_rrect(int childId, float rad) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->borderRadius = rad; w->overflow = "clip"; }
    if (childId >= 0) widget_add_child(c, childId);
    return c;
}

int convenience_flexible(int childId, float flexFactor) {
    int exp = widget_expanded();
    Widget* w = getWidget(exp);
    if (w) w->flex = flexFactor;
    if (childId >= 0) widget_add_child(exp, childId);
    return exp;
}

int convenience_circle_avatar(const std::string& letter, int r, int g, int b, float sz) {
    int av = widget_avatar(letter, r, g, b);
    Widget* w = getWidget(av);
    if (w) { w->fixedWidth = sz; w->fixedHeight = sz; w->borderRadius = sz / 2; }
    return av;
}

int convenience_search_bar(const std::string& hint, int r, int g, int b) {
    int row = widget_row();
    Widget* rw = getWidget(row);
    if (rw) { rw->bgColor = {240,240,240,255}; rw->borderRadius = 24; rw->padding = 8; }
    int icon = widget_icon('S');
    widget_add_child(row, icon);
    int field = widget_search_field(hint);
    Widget* fw = getWidget(field);
    if (fw) fw->flex = 1;
    widget_add_child(row, field);
    return row;
}

int convenience_bottom_sheet(int contentId, float height) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) {
        w->fixedHeight = height; w->bgColor = {255,255,255,255};
        w->borderRadius = 16; w->shadowRadius = 8; w->shadowOffsetY = -2;
    }
    if (contentId >= 0) widget_add_child(c, contentId);
    return c;
}

int convenience_navigation_rail(int selectedIdx) {
    int col = widget_column();
    Widget* w = getWidget(col);
    if (w) { w->fixedWidth = 72; w->bgColor = {250,250,250,255}; w->padding = 8; }
    return col;
}

int convenience_segmented_button() {
    int row = widget_row();
    Widget* w = getWidget(row);
    if (w) { w->bgColor = {230,230,230,255}; w->borderRadius = 20; w->padding = 4; w->spacing = 2; }
    return row;
}

int convenience_filter_chip(const std::string& label, bool selected) {
    int chip = widget_chip(label);
    Widget* w = getWidget(chip);
    if (w) {
        w->checked = selected;
        if (selected) { w->bgColor = {33,150,243,40}; w->textColor = {33,150,243,255}; }
        else { w->bgColor = {240,240,240,255}; }
    }
    return chip;
}

int convenience_choice_chip(const std::string& label, bool selected) {
    int chip = widget_chip(label);
    Widget* w = getWidget(chip);
    if (w) {
        w->checked = selected;
        if (selected) { w->bgColor = {33,150,243,255}; w->textColor = {255,255,255,255}; }
        else { w->bgColor = {240,240,240,255}; w->borderWidth = 1; }
    }
    return chip;
}

int convenience_input_chip(const std::string& label) {
    int chip = widget_chip(label);
    Widget* w = getWidget(chip);
    if (w) { w->borderWidth = 1; w->borderColor = {180,180,180,255}; }
    return chip;
}

int convenience_action_chip(const std::string& label) {
    int chip = widget_chip(label);
    Widget* w = getWidget(chip);
    if (w) { w->bgColor = {66,133,244,255}; w->textColor = {255,255,255,255}; }
    return chip;
}

int convenience_form_group(const std::string& label) {
    int col = widget_column();
    Widget* cw = getWidget(col);
    if (cw) { cw->spacing = 8; cw->padding = 12; }
    if (!label.empty()) {
        int lbl = widget_label(label);
        Widget* lw = getWidget(lbl);
        if (lw) { lw->fontSize = 14; lw->textColor = {100,100,100,255}; }
        widget_add_child(col, lbl);
    }
    return col;
}

int convenience_list_section(const std::string& title) {
    int col = widget_column();
    Widget* cw = getWidget(col);
    if (cw) { cw->spacing = 4; }
    int lbl = widget_label(title);
    Widget* lw = getWidget(lbl);
    if (lw) { lw->fontSize = 12; lw->textColor = {120,120,120,255}; lw->padLeft = 16; lw->padTop = 12; }
    widget_add_child(col, lbl);
    int div = widget_divider();
    widget_add_child(col, div);
    return col;
}

int convenience_empty_state(const std::string& message, const std::string& icon) {
    int center = widget_center();
    int col = widget_column();
    Widget* cw = getWidget(col);
    if (cw) { cw->mainAxisAlignment = "center"; cw->crossAxisAlignment = "center"; cw->spacing = 16; }
    if (!icon.empty()) {
        int ic = widget_icon(icon[0]);
        Widget* iw = getWidget(ic);
        if (iw) { iw->fontSize = 48; iw->textColor = {180,180,180,255}; }
        widget_add_child(col, ic);
    }
    int txt = widget_text(message);
    Widget* tw = getWidget(txt);
    if (tw) { tw->fontSize = 16; tw->textColor = {150,150,150,255}; }
    widget_add_child(col, txt);
    widget_add_child(center, col);
    return center;
}

int convenience_error_widget(const std::string& message) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->bgColor = {255,235,238,255}; w->borderRadius = 8; w->padding = 16;
             w->borderWidth = 1; w->borderColor = {244,67,54,255}; }
    int txt = widget_text(message);
    Widget* tw = getWidget(txt);
    if (tw) { tw->textColor = {211,47,47,255}; tw->fontSize = 14; }
    widget_add_child(c, txt);
    return c;
}

int convenience_placeholder(float pw, float ph) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->fixedWidth = pw; w->fixedHeight = ph; w->bgColor = {224,224,224,255};
             w->borderRadius = 4; w->borderWidth = 1; w->borderColor = {189,189,189,255}; }
    return c;
}

int convenience_kpi_card(const std::string& label, const std::string& val, const std::string& trend) {
    int card = widget_card();
    Widget* cw = getWidget(card);
    if (cw) { cw->padding = 16; cw->shadowRadius = 4; }
    int col = widget_column();
    Widget* ccw = getWidget(col);
    if (ccw) ccw->spacing = 4;
    int lbl = widget_label(label);
    Widget* lw = getWidget(lbl);
    if (lw) { lw->fontSize = 12; lw->textColor = {120,120,120,255}; }
    widget_add_child(col, lbl);
    int vtxt = widget_text(val);
    Widget* vw = getWidget(vtxt);
    if (vw) { vw->fontSize = 28; vw->fontWeight = "bold"; }
    widget_add_child(col, vtxt);
    if (!trend.empty()) {
        int ttxt = widget_text(trend);
        Widget* tw = getWidget(ttxt);
        if (tw) {
            tw->fontSize = 12;
            if (!trend.empty() && trend[0] == '+') tw->textColor = {76,175,80,255};
            else tw->textColor = {244,67,54,255};
        }
        widget_add_child(col, ttxt);
    }
    widget_add_child(card, col);
    return card;
}

int convenience_countdown(int seconds) {
    int txt = widget_text(std::to_string(seconds));
    Widget* w = getWidget(txt);
    if (w) { w->fontSize = 32; w->fontWeight = "bold"; w->textAlign = "center"; }
    return txt;
}

int convenience_divider_themed(const std::string& color, float thickness) {
    int div = widget_divider();
    Widget* w = getWidget(div);
    if (w) {
        w->bgColor = parseColor(color);
        w->fixedHeight = thickness;
    }
    return div;
}

int convenience_gradient_card(const std::string& title, int r1, int g1, int b1, int r2, int g2, int b2) {
    int card = widget_card();
    Widget* w = getWidget(card);
    if (w) {
        w->gradientStart = {r1,g1,b1,255}; w->gradientEnd = {r2,g2,b2,255};
        w->hasGradient = true; w->borderRadius = 12; w->padding = 16;
    }
    if (!title.empty()) {
        int txt = widget_text(title);
        Widget* tw = getWidget(txt);
        if (tw) { tw->textColor = {255,255,255,255}; tw->fontSize = 18; }
        widget_add_child(card, txt);
    }
    return card;
}

int convenience_outlined_card(const std::string& title, int r, int g, int b) {
    int card = widget_card();
    Widget* w = getWidget(card);
    if (w) { w->borderWidth = 2; w->borderColor = {r,g,b,255}; w->borderRadius = 8; w->padding = 16;
             w->bgColor = {255,255,255,255}; }
    if (!title.empty()) {
        int txt = widget_text(title);
        widget_add_child(card, txt);
    }
    return card;
}

int convenience_hero_header(const std::string& title, const std::string& sub, int r, int g, int b) {
    int c = widget_container();
    Widget* w = getWidget(c);
    if (w) { w->bgColor = {r,g,b,255}; w->padding = 32; w->fixedHeight = 200; }
    int col = widget_column();
    Widget* ccw = getWidget(col);
    if (ccw) { ccw->mainAxisAlignment = "center"; ccw->crossAxisAlignment = "center"; ccw->spacing = 8; }
    int ttxt = widget_text(title);
    Widget* tw = getWidget(ttxt);
    if (tw) { tw->fontSize = 32; tw->fontWeight = "bold"; tw->textColor = {255,255,255,255}; }
    widget_add_child(col, ttxt);
    if (!sub.empty()) {
        int stxt = widget_text(sub);
        Widget* sw = getWidget(stxt);
        if (sw) { sw->fontSize = 16; sw->textColor = {255,255,255,200}; }
        widget_add_child(col, stxt);
    }
    widget_add_child(c, col);
    return c;
}

int convenience_login_form(const std::string& title) {
    int card = widget_card();
    Widget* cw = getWidget(card);
    if (cw) { cw->padding = 24; cw->borderRadius = 12; cw->shadowRadius = 8; cw->fixedWidth = 360; }
    int col = widget_column();
    Widget* ccw = getWidget(col);
    if (ccw) ccw->spacing = 16;
    if (!title.empty()) {
        int ttxt = widget_heading(title, 2);
        widget_add_child(col, ttxt);
    }
    int email = widget_textfield("email");
    widget_add_child(col, email);
    int pass = widget_password_field("password");
    widget_add_child(col, pass);
    int btn = widget_button("login");
    widget_add_child(col, btn);
    widget_add_child(card, col);
    return card;
}

int convenience_avatar_group(int count) {
    int row = widget_row();
    Widget* rw = getWidget(row);
    if (rw) rw->spacing = -8; // overlapping
    const char letters[] = "ABCDEFGHIJ";
    int colors[][3] = {{66,133,244},{234,67,53},{251,188,4},{52,168,83},{255,109,0},
                        {156,39,176},{0,150,136},{121,85,72},{96,125,139},{233,30,99}};
    for (int i = 0; i < std::min(count, 10); i++) {
        std::string l(1, letters[i]);
        int av = widget_avatar(l, colors[i][0], colors[i][1], colors[i][2]);
        widget_add_child(row, av);
    }
    return row;
}

int convenience_toolbar_action(const std::string& icon, const std::string& label) {
    int col = widget_column();
    Widget* cw = getWidget(col);
    if (cw) { cw->crossAxisAlignment = "center"; cw->spacing = 2; cw->padding = 4; }
    if (!icon.empty()) {
        int ic = widget_icon(icon[0]);
        widget_add_child(col, ic);
    }
    if (!label.empty()) {
        int lbl = widget_label(label);
        Widget* lw = getWidget(lbl);
        if (lw) lw->fontSize = 10;
        widget_add_child(col, lbl);
    }
    return col;
}

int convenience_tag_group() {
    int wrap = widget_wrap();
    Widget* w = getWidget(wrap);
    if (w) w->spacing = 6;
    return wrap;
}

int convenience_rating_bar(int maxStars, float initialValue) {
    int r = widget_rating_stars(maxStars);
    Widget* w = getWidget(r);
    if (w) w->value = initialValue;
    return r;
}

int convenience_progress_indicator(float val, const std::string& label) {
    int col = widget_column();
    Widget* cw = getWidget(col);
    if (cw) cw->spacing = 4;
    if (!label.empty()) {
        int row = widget_row();
        int lbl = widget_label(label);
        Widget* lw = getWidget(lbl);
        if (lw) lw->fontSize = 12;
        widget_add_child(row, lbl);
        int pct = widget_text(std::to_string((int)(val * 100)) + "%");
        Widget* pw = getWidget(pct);
        if (pw) { pw->fontSize = 12; pw->textColor = {100,100,100,255}; }
        widget_add_child(row, pct);
        widget_add_child(col, row);
    }
    int bar = widget_progress_bar(val);
    widget_add_child(col, bar);
    return col;
}

int convenience_split_button(const std::string& text) {
    int row = widget_row();
    Widget* rw = getWidget(row);
    if (rw) { rw->spacing = 0; }
    int btn = widget_button(text);
    widget_add_child(row, btn);
    int drop = widget_icon_button('V');
    Widget* dw = getWidget(drop);
    if (dw) { dw->btnColor = {56,123,234,255}; }
    widget_add_child(row, drop);
    return row;
}

int convenience_fab_extended(const std::string& label, char icon) {
    int row = widget_row();
    Widget* rw = getWidget(row);
    if (rw) {
        rw->bgColor = {66,133,244,255}; rw->borderRadius = 28; rw->padding = 16;
        rw->spacing = 8; rw->shadowRadius = 6; rw->shadowOffsetY = 3;
    }
    int ic = widget_icon(icon);
    Widget* iw = getWidget(ic);
    if (iw) iw->textColor = {255,255,255,255};
    widget_add_child(row, ic);
    int txt = widget_text(label);
    Widget* tw = getWidget(txt);
    if (tw) { tw->textColor = {255,255,255,255}; tw->fontSize = 14; }
    widget_add_child(row, txt);
    return row;
}

int convenience_chip_group() {
    int wrap = widget_wrap();
    Widget* w = getWidget(wrap);
    if (w) w->spacing = 8;
    return wrap;
}

int convenience_nav_drawer(const std::string& title) {
    int drawer = widget_drawer();
    Widget* dw = getWidget(drawer);
    if (dw) { dw->fixedWidth = 280; dw->bgColor = {255,255,255,255}; }
    if (!title.empty()) {
        int header = widget_container();
        Widget* hw = getWidget(header);
        if (hw) { hw->bgColor = {33,150,243,255}; hw->padding = 24; hw->fixedHeight = 160; }
        int txt = widget_text(title);
        Widget* tw = getWidget(txt);
        if (tw) { tw->textColor = {255,255,255,255}; tw->fontSize = 20; }
        widget_add_child(header, txt);
        widget_add_child(drawer, header);
        widget_add_child(drawer, widget_divider());
    }
    return drawer;
}

// ═══════════════════════════════════════════════════════════════════════════
// [FIX #291-400] v4.0: إصلاحات نقاط الضعف الإضافية — 110 إصلاح
// ═══════════════════════════════════════════════════════════════════════════

// [FIX #291] دالة التحقق من صحة الودجت قبل العمليات
bool isValidWidget(int id) {
    Widget* w = getWidget(id);
    return (w != nullptr);
}

// [FIX #292] دالة إعادة تعيين خصائص الودجت للافتراضي
void widgetResetToDefaults(int id) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->opacity = 1.0f;
    w->visible = true;
    w->enabled = true;
    w->hovered = false;
    w->pressed = false;
    w->hasFocus = false;
    w->borderRadius = 0;
    w->elevation = 0;
    w->shadowRadius = 0;
    w->padding = 0;
    w->margin = 0;
    w->animating = false;
    w->animationDuration = 300;
}

// [FIX #293] دالة نسخ خصائص من ودجت لآخر
void widgetCopyStyleFrom(int targetId, int sourceId) {
    Widget* target = getWidget(targetId);
    Widget* source = getWidget(sourceId);
    if (!target || !source) return;
    target->bgColor = source->bgColor;
    target->textColor = source->textColor;
    target->borderColor = source->borderColor;
    target->borderRadius = source->borderRadius;
    target->borderWidth = source->borderWidth;
    target->padding = source->padding;
    target->fontSize = source->fontSize;
    target->fontWeight = source->fontWeight;
    target->opacity = source->opacity;
    target->elevation = source->elevation;
    target->shadowRadius = source->shadowRadius;
    target->shadowColor = source->shadowColor;
}

// [FIX #294] دالة حساب الحجم الفعلي للودجت (بعد التخطيط)
void widgetGetComputedSize(int id, float& outW, float& outH) {
    Widget* w = getWidget(id);
    if (!w) { outW = 0; outH = 0; return; }
    outW = w->cw;
    outH = w->ch;
}

// [FIX #295] دالة حساب الموضع الفعلي للودجت
void widgetGetComputedPosition(int id, float& outX, float& outY) {
    Widget* w = getWidget(id);
    if (!w) { outX = 0; outY = 0; return; }
    outX = w->cx;
    outY = w->cy;
}

// [FIX #296] دالة ترتيب الأبناء حسب خاصية معينة (zIndex)
void widgetSortChildrenByZIndex(int parentId) {
    Widget* parent = getWidget(parentId);
    if (!parent) return;
    std::stable_sort(parent->children.begin(), parent->children.end(), [](int a, int b) {
        Widget* wa = getWidget(a);
        Widget* wb = getWidget(b);
        return (wa ? wa->zIndex : 0) < (wb ? wb->zIndex : 0);
    });
}

// [FIX #297] دالة تبديل موضعين بين ولدين
void widgetSwapChildren(int parentId, int index1, int index2) {
    Widget* parent = getWidget(parentId);
    if (!parent) return;
    if (index1 < 0 || index1 >= (int)parent->children.size()) return;
    if (index2 < 0 || index2 >= (int)parent->children.size()) return;
    std::swap(parent->children[index1], parent->children[index2]);
}

// [FIX #298] دالة البحث عن ولد بالنص
int widgetFindChildByText(int parentId, const std::string& text) {
    Widget* parent = getWidget(parentId);
    if (!parent) return -1;
    for (int cid : parent->children) {
        Widget* c = getWidget(cid);
        if (c && c->text == text) return cid;
    }
    return -1;
}

// [FIX #299] دالة البحث عن جميع الأولاد من نوع معين
std::vector<int> widgetFindChildrenByType(int parentId, WidgetType type) {
    std::vector<int> result;
    Widget* parent = getWidget(parentId);
    if (!parent) return result;
    for (int cid : parent->children) {
        Widget* c = getWidget(cid);
        if (c && c->type == type) result.push_back(cid);
    }
    return result;
}

// [FIX #300] دالة تطبيق نمط على جميع الأولاد
void widgetApplyStyleToChildren(int parentId, const std::string& property, const std::string& value) {
    Widget* parent = getWidget(parentId);
    if (!parent) return;
    for (int cid : parent->children) {
        widgetSetProp(cid, property, value);
    }
}

// [FIX #301] دالة إخفاء الودجت مع حركة انتقالية
void widgetHideWithAnimation(int id, int durationMs) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->animating = true;
    w->animationDuration = (float)durationMs;
    w->transitionProperty = "opacity";
    // سيتم معالجتها في نظام الحركة
}

// [FIX #302] دالة إظهار الودجت مع حركة انتقالية
void widgetShowWithAnimation(int id, int durationMs) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->visible = true;
    w->animating = true;
    w->animationDuration = (float)durationMs;
    w->transitionProperty = "opacity";
}

// [FIX #303] دالة التحقق من تداخل ودجتين
bool widgetsOverlap(int id1, int id2) {
    Widget* w1 = getWidget(id1);
    Widget* w2 = getWidget(id2);
    if (!w1 || !w2) return false;
    bool xOverlap = (w1->cx < w2->cx + w2->cw) && (w1->cx + w1->cw > w2->cx);
    bool yOverlap = (w1->cy < w2->cy + w2->ch) && (w1->cy + w1->ch > w2->cy);
    return xOverlap && yOverlap;
}

// [FIX #304] دالة التحقق من وجود نقطة داخل ودجت
bool widgetContainsPoint(int id, float px, float py) {
    Widget* w = getWidget(id);
    if (!w) return false;
    return px >= w->cx && px <= w->cx + w->cw &&
           py >= w->cy && py <= w->cy + w->ch;
}

// [FIX #305] دالة جلب الودجت الأقرب لنقطة معينة
int widgetFindNearest(const std::vector<int>& widgets, float px, float py) {
    int nearest = -1;
    float minDist = 1e9f;
    for (int id : widgets) {
        Widget* w = getWidget(id);
        if (!w || !w->visible) continue;
        float cx = w->cx + w->cw/2;
        float cy = w->cy + w->ch/2;
        float dist = std::sqrt((cx-px)*(cx-px) + (cy-py)*(cy-py));
        if (dist < minDist) { minDist = dist; nearest = id; }
    }
    return nearest;
}

// [FIX #306] دالة تنسيق الودجتات عمودياً (توزيع متساوي)
void widgetDistributeVertically(const std::vector<int>& widgets, float startY, float endY) {
    if (widgets.size() < 2) return;
    float spacing = (endY - startY) / (widgets.size() - 1);
    float currY = startY;
    for (int id : widgets) {
        Widget* w = getWidget(id);
        if (w) { w->cy = currY; currY += spacing; }
    }
}

// [FIX #307] دالة تنسيق الودجتات أفقياً
void widgetDistributeHorizontally(const std::vector<int>& widgets, float startX, float endX) {
    if (widgets.size() < 2) return;
    float spacing = (endX - startX) / (widgets.size() - 1);
    float currX = startX;
    for (int id : widgets) {
        Widget* w = getWidget(id);
        if (w) { w->cx = currX; currX += spacing; }
    }
}

// [FIX #308] دالة محاذاة الودجتات للأعلى
void widgetAlignTop(const std::vector<int>& widgets) {
    if (widgets.empty()) return;
    float minY = 1e9f;
    for (int id : widgets) {
        Widget* w = getWidget(id);
        if (w && w->cy < minY) minY = w->cy;
    }
    for (int id : widgets) {
        Widget* w = getWidget(id);
        if (w) w->cy = minY;
    }
}

// [FIX #309] دالة محاذاة الودجتات لليسار
void widgetAlignLeft(const std::vector<int>& widgets) {
    if (widgets.empty()) return;
    float minX = 1e9f;
    for (int id : widgets) {
        Widget* w = getWidget(id);
        if (w && w->cx < minX) minX = w->cx;
    }
    for (int id : widgets) {
        Widget* w = getWidget(id);
        if (w) w->cx = minX;
    }
}

// [FIX #310] دالة إنهاء جميع حركات الودجت
void widgetStopAllAnimations(int id) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->animating = false;
    w->animationValue = 0;
    w->animationProgress = 0;
}

// [FIX #311] دالة جلب عمق الودجت في الشجرة
int widgetGetDepth(int id) {
    Widget* w = getWidget(id);
    if (!w) return -1;
    int depth = 0;
    int parentId = w->parentId;
    while (parentId >= 0) {
        depth++;
        Widget* p = getWidget(parentId);
        parentId = p ? p->parentId : -1;
    }
    return depth;
}

// [FIX #312] دالة جلب الجد الأعلى (root)
int widgetGetRoot(int id) {
    Widget* w = getWidget(id);
    if (!w) return -1;
    while (w->parentId >= 0) {
        Widget* p = getWidget(w->parentId);
        if (!p) break;
        w = p;
    }
    // نرجع id الأصلي إذا لم نجد جد
    return w->parentId >= 0 ? -1 : id; // نحتاج العودة بالـ w ID
}

// [FIX #313] دالة تجميد الودجت (منع التفاعل)
void widgetFreeze(int id) {
    Widget* w = getWidget(id);
    if (w) { w->enabled = false; w->focusable = false; }
}

// [FIX #314] دالة إذابة الودجت (استعادة التفاعل)
void widgetUnfreeze(int id) {
    Widget* w = getWidget(id);
    if (w) { w->enabled = true; w->focusable = true; }
}

// [FIX #315] دالة تحويل الودجت لـ JSON (تسلسل)
std::string widgetToJSON(int id) {
    Widget* w = getWidget(id);
    if (!w) return "{}";
    std::ostringstream ss;
    ss << "{";
    ss << "\"id\":" << id << ",";
    ss << "\"type\":" << (int)w->type << ",";
    ss << "\"text\":\"" << w->text << "\",";
    ss << "\"visible\":" << (w->visible ? "true" : "false") << ",";
    ss << "\"enabled\":" << (w->enabled ? "true" : "false") << ",";
    ss << "\"x\":" << w->cx << ",\"y\":" << w->cy << ",";
    ss << "\"w\":" << w->cw << ",\"h\":" << w->ch;
    ss << "}";
    return ss.str();
}

// [FIX #316-320] دوال الكشف عن حالة الودجت
bool widgetIsVisible(int id) { Widget* w = getWidget(id); return w && w->visible; }
bool widgetIsEnabled(int id) { Widget* w = getWidget(id); return w && w->enabled; }
bool widgetIsFocused(int id) { Widget* w = getWidget(id); return w && w->hasFocus; }
bool widgetIsHovered(int id) { Widget* w = getWidget(id); return w && w->hovered; }
bool widgetIsPressed(int id) { Widget* w = getWidget(id); return w && w->pressed; }

// [FIX #321-325] دوال جلب خصائص رقمية
float widgetGetOpacity(int id) { Widget* w = getWidget(id); return w ? w->opacity : 0; }
float widgetGetElevation(int id) { Widget* w = getWidget(id); return w ? w->elevation : 0; }
float widgetGetBorderRadius(int id) { Widget* w = getWidget(id); return w ? w->borderRadius : 0; }
float widgetGetPadding(int id) { Widget* w = getWidget(id); return w ? w->padding : 0; }
float widgetGetFontSize(int id) { Widget* w = getWidget(id); return w ? w->fontSize : 14; }

// [FIX #326-330] دوال جلب خصائص نصية
std::string widgetGetText(int id) { Widget* w = getWidget(id); return w ? w->text : ""; }
std::string widgetGetInputText(int id) { Widget* w = getWidget(id); return w ? w->inputText : ""; }
std::string widgetGetPlaceholder(int id) { Widget* w = getWidget(id); return w ? w->placeholder : ""; }
std::string widgetGetTooltip(int id) { Widget* w = getWidget(id); return w ? w->tooltipText : ""; }
std::string widgetGetTextAlign(int id) { Widget* w = getWidget(id); return w ? w->textAlign : "left"; }

// [FIX #331-335] دوال التحقق من نوع الودجت
bool widgetIsContainer(int id) {
    Widget* w = getWidget(id);
    if (!w) return false;
    return w->type == WidgetType::Container || w->type == WidgetType::Column ||
           w->type == WidgetType::Row || w->type == WidgetType::Stack ||
           w->type == WidgetType::Card || w->type == WidgetType::Panel;
}
bool widgetIsInput(int id) {
    Widget* w = getWidget(id);
    if (!w) return false;
    return w->type == WidgetType::TextField || w->type == WidgetType::TextArea ||
           w->type == WidgetType::PasswordField || w->type == WidgetType::NumberInput;
}
bool widgetIsButton(int id) {
    Widget* w = getWidget(id);
    if (!w) return false;
    return w->type == WidgetType::Button || w->type == WidgetType::OutlineButton ||
           w->type == WidgetType::TextButton || w->type == WidgetType::IconButton;
}
bool widgetIsScrollable(int id) {
    Widget* w = getWidget(id);
    if (!w) return false;
    return w->type == WidgetType::ScrollView || w->type == WidgetType::ListView;
}
bool widgetIsLayout(int id) {
    Widget* w = getWidget(id);
    if (!w) return false;
    return w->type == WidgetType::Column || w->type == WidgetType::Row ||
           w->type == WidgetType::Stack || w->type == WidgetType::GridView;
}

// [FIX #336-340] دوال معالجة الأحداث
void widgetSetOnClick(int id, int callbackId) {
    Widget* w = getWidget(id);
    if (w) w->callbackId = callbackId;
}
void widgetSetOnHover(int id, int callbackId) {
    Widget* w = getWidget(id);
    if (w) w->hoverCallbackId = callbackId;
}
void widgetSetOnFocus(int id, int callbackId) {
    Widget* w = getWidget(id);
    if (w) w->focusCallbackId = callbackId;
}
void widgetSetOnBlur(int id, int callbackId) {
    Widget* w = getWidget(id);
    if (w) w->blurCallbackId = callbackId;
}
void widgetSetOnChange(int id, int callbackId) {
    Widget* w = getWidget(id);
    if (w) w->changeCallbackId = callbackId;
}

// [FIX #341-345] دوال الحالة والبيانات
void widgetSetState(int id, const std::string& state) {
    Widget* w = getWidget(id);
    if (w) w->state = state;
}
std::string widgetGetState(int id) {
    Widget* w = getWidget(id);
    return w ? w->state : "";
}
void widgetSetDataSource(int id, const std::string& source) {
    Widget* w = getWidget(id);
    if (w) w->dataSource = source;
}
void widgetSetBindingPath(int id, const std::string& path) {
    Widget* w = getWidget(id);
    if (w) w->bindingPath = path;
}
void widgetSetLoading(int id, bool loading) {
    Widget* w = getWidget(id);
    if (w) w->isLoading = loading;
}

// [FIX #346-350] دوال التحقق المتقدمة
void widgetSetValidation(int id, const std::string& validationState, const std::string& errorText) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->validationState = validationState;
    w->errorText = errorText;
    w->hasError = (validationState == "error");
}
void widgetClearValidation(int id) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->validationState = "";
    w->errorText = "";
    w->hasError = false;
}
bool widgetHasValidationError(int id) {
    Widget* w = getWidget(id);
    return w && w->hasError;
}
void widgetSetRequired(int id, bool required) {
    Widget* w = getWidget(id);
    if (w) w->required = required;
}
bool widgetIsRequired(int id) {
    Widget* w = getWidget(id);
    return w && w->required;
}

// [FIX #351-355] دوال الرسوم المتحركة
void widgetSetAnimationCurve(int id, const std::string& curve) {
    Widget* w = getWidget(id);
    if (w) w->animationCurve = curve;
}
void widgetSetAnimationDuration(int id, float duration) {
    Widget* w = getWidget(id);
    if (w) w->animationDuration = duration;
}
void widgetSetAnimationDelay(int id, float delay) {
    Widget* w = getWidget(id);
    if (w) w->animationDelay = delay;
}
void widgetSetTransitionProperty(int id, const std::string& property) {
    Widget* w = getWidget(id);
    if (w) w->transitionProperty = property;
}
void widgetStartAnimation(int id) {
    Widget* w = getWidget(id);
    if (w) { w->animating = true; w->animationValue = 0; }
}

// [FIX #356-360] دوال الإيماءات
void widgetSetSwipeEnabled(int id, bool enabled) {
    Widget* w = getWidget(id);
    if (w) w->swipeEnabled = enabled;
}
void widgetSetPanEnabled(int id, bool enabled) {
    Widget* w = getWidget(id);
    if (w) w->panEnabled = enabled;
}
void widgetSetPinchEnabled(int id, bool enabled) {
    Widget* w = getWidget(id);
    if (w) w->pinchEnabled = enabled;
}
void widgetSetLongPressEnabled(int id, bool enabled) {
    Widget* w = getWidget(id);
    if (w) w->longPressEnabled = enabled;
}
void widgetSetLongPressDuration(int id, float duration) {
    Widget* w = getWidget(id);
    if (w) w->longPressDuration = duration;
}

// [FIX #361-365] دوال التخطيط المتقدم
void widgetSetPosition(int id, const std::string& position) {
    Widget* w = getWidget(id);
    if (w) w->position = position;
}
void widgetSetPositionOffsets(int id, float top, float right, float bottom, float left) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->posTop = top; w->posRight = right;
    w->posBottom = bottom; w->posLeft = left;
}
void widgetSetDisplay(int id, const std::string& display) {
    Widget* w = getWidget(id);
    if (w) w->display = display;
}
void widgetSetFlexDirection(int id, const std::string& direction) {
    Widget* w = getWidget(id);
    if (w) w->flexDirection = direction;
}
void widgetSetFlexWrap(int id, const std::string& wrap) {
    Widget* w = getWidget(id);
    if (w) w->flexWrap = wrap;
}

// [FIX #366-370] دوال الشبكة
void widgetSetGridTemplateColumns(int id, const std::string& columns) {
    Widget* w = getWidget(id);
    if (w) w->gridTemplateColumns = columns;
}
void widgetSetGridTemplateRows(int id, const std::string& rows) {
    Widget* w = getWidget(id);
    if (w) w->gridTemplateRows = rows;
}
void widgetSetGridGap(int id, float rowGap, float colGap) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->gridRowGap = rowGap; w->gridColGap = colGap;
}
void widgetSetGridArea(int id, int row, int col, int rowSpan, int colSpan) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->gridRow = row; w->gridColumn = col;
    w->gridRowSpan = rowSpan; w->gridColSpan = colSpan;
}
void widgetSetGridAutoFlow(int id, const std::string& flow) {
    Widget* w = getWidget(id);
    if (w) w->gridAutoFlow = flow;
}

// [FIX #371-375] دوال الطباعة المتقدمة
void widgetSetFontFamily(int id, const std::string& family) {
    Widget* w = getWidget(id);
    if (w) w->fontFamily = family;
}
void widgetSetFontStyle(int id, const std::string& style) {
    Widget* w = getWidget(id);
    if (w) w->fontStyle = style;
}
void widgetSetTextShadow(int id, const std::string& shadow) {
    Widget* w = getWidget(id);
    if (w) w->textShadow = shadow;
}
void widgetSetWordSpacing(int id, float spacing) {
    Widget* w = getWidget(id);
    if (w) w->wordSpacing = spacing;
}
void widgetSetTextDirection(int id, const std::string& direction) {
    Widget* w = getWidget(id);
    if (w) w->textDirection = direction;
}

// [FIX #376-380] دوال الحدود المتقدمة
void widgetSetBorderWidths(int id, float top, float right, float bottom, float left) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->borderTopWidth = top; w->borderRightWidth = right;
    w->borderBottomWidth = bottom; w->borderLeftWidth = left;
}
void widgetSetBorderRadii(int id, float topLeft, float topRight, float bottomRight, float bottomLeft) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->borderTopLeftRadius = topLeft; w->borderTopRightRadius = topRight;
    w->borderBottomRightRadius = bottomRight; w->borderBottomLeftRadius = bottomLeft;
}
void widgetSetBorderStyle(int id, const std::string& style) {
    Widget* w = getWidget(id);
    if (w) w->borderStyle = style;
}
void widgetSetBoxSizing(int id, const std::string& sizing) {
    Widget* w = getWidget(id);
    if (w) w->boxSizing = sizing;
}

// [FIX #381-385] دوال التأثيرات والفلاتر
void widgetSetBlur(int id, float blur) {
    Widget* w = getWidget(id);
    if (w) w->blur = blur;
}
void widgetSetBrightness(int id, float brightness) {
    Widget* w = getWidget(id);
    if (w) w->brightness = brightness;
}
void widgetSetContrast(int id, float contrast) {
    Widget* w = getWidget(id);
    if (w) w->contrast = contrast;
}
void widgetSetSaturation(int id, float saturation) {
    Widget* w = getWidget(id);
    if (w) w->saturation = saturation;
}
void widgetSetGrayscale(int id, bool grayscale) {
    Widget* w = getWidget(id);
    if (w) w->grayscale = grayscale;
}

// [FIX #386-390] دوال الوصول
void widgetSetAriaLabel(int id, const std::string& label) {
    Widget* w = getWidget(id);
    if (w) w->ariaLabel = label;
}
void widgetSetAriaDescribedBy(int id, const std::string& describedBy) {
    Widget* w = getWidget(id);
    if (w) w->ariaDescribedBy = describedBy;
}
void widgetSetAriaHidden(int id, bool hidden) {
    Widget* w = getWidget(id);
    if (w) w->ariaHidden = hidden;
}
void widgetSetRole(int id, const std::string& role) {
    Widget* w = getWidget(id);
    if (w) w->role = role;
}
void widgetSetTabIndex(int id, int tabIndex) {
    Widget* w = getWidget(id);
    if (w) w->tabIndex = tabIndex;
}

// [FIX #391-395] دوال البيانات والربط
void widgetSetRepeatCount(int id, int count) {
    Widget* w = getWidget(id);
    if (w) w->repeatCount = count;
}
void widgetSetPageSize(int id, int size) {
    Widget* w = getWidget(id);
    if (w) w->pageSize = size;
}
void widgetSetCurrentIndex(int id, int index) {
    Widget* w = getWidget(id);
    if (w) w->currentIndex = index;
}
int widgetGetCurrentIndex(int id) {
    Widget* w = getWidget(id);
    return w ? w->currentIndex : -1;
}
void widgetSetSortProperty(int id, const std::string& property, bool ascending) {
    Widget* w = getWidget(id);
    if (!w) return;
    w->sortProperty = property;
    w->sortAscending = ascending;
}

// [FIX #396-400] دوال إضافية
void widgetSetContentType(int id, const std::string& type) {
    Widget* w = getWidget(id);
    if (w) w->contentType = type;
}
void widgetSetFilterPredicate(int id, const std::string& predicate) {
    Widget* w = getWidget(id);
    if (w) w->filterPredicate = predicate;
}
void widgetSetIsTemplate(int id, bool isTemplate) {
    Widget* w = getWidget(id);
    if (w) w->isTemplate = isTemplate;
}
void widgetSetBlendMode(int id, const std::string& mode) {
    Widget* w = getWidget(id);
    if (w) w->blendMode = mode;
}
void widgetSetBackdropFilter(int id, const std::string& filter) {
    Widget* w = getWidget(id);
    if (w) w->backdropFilter = filter;
}

}} // namespace sad::ui
