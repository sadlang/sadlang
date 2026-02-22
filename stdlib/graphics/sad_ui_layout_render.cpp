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
#include <cstring>
#include <iostream>
#include <sstream>

namespace sad {
namespace ui {



// ═══════════════════════════════════════════════════════════════════
// القسم 5: رسم الأشكال الأساسية
// ─────────────────────────────────────────────────────────────────
// مستطيل دائري الزوايا، حدود، دائرة، خطوط
// ═══════════════════════════════════════════════════════════════════

// رسم مستطيل دائري الزوايا — يُستخدم للأزرار والبطاقات
static void drawRoundedRect(SDL_Renderer* r, int x, int y, int w, int h,
                            int radius, RGBA color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    if (radius <= 0 || radius > std::min(w/2, h/2)) {
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(r, &rect);
        return;
    }
    radius = std::min(radius, std::min(w/2, h/2));
    // ثلاث مستطيلات مركزية تشكّل الجسم الرئيسي
    SDL_Rect rects[3] = {
        {x + radius, y, w - 2*radius, h},           // الشريط العمودي الوسط
        {x, y + radius, radius, h - 2*radius},       // الشريط الأيسر
        {x + w - radius, y + radius, radius, h - 2*radius} // الشريط الأيمن
    };
    for (auto& rect : rects) SDL_RenderFillRect(r, &rect);
    // رسم الزوايا الأربع كأرباع دوائر
    int cx[4] = {x+radius, x+w-radius-1, x+radius, x+w-radius-1};
    int cy[4] = {y+radius, y+radius, y+h-radius-1, y+h-radius-1};
    for (int corner = 0; corner < 4; corner++) {
        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                if (dx*dx + dy*dy <= radius*radius) {
                    bool valid = false;
                    if (corner==0 && dx<=0 && dy<=0) valid=true;
                    if (corner==1 && dx>=0 && dy<=0) valid=true;
                    if (corner==2 && dx<=0 && dy>=0) valid=true;
                    if (corner==3 && dx>=0 && dy>=0) valid=true;
                    if (valid) SDL_RenderDrawPoint(r, cx[corner]+dx, cy[corner]+dy);
                }
            }
        }
    }
}

// رسم حدود مستطيل (بدون تعبئة)
static void drawRectBorder(SDL_Renderer* r, int x, int y, int w, int h,
                           int thickness, RGBA color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    for (int i = 0; i < thickness; i++) {
        SDL_Rect rect = {x+i, y+i, w-2*i, h-2*i};
        SDL_RenderDrawRect(r, &rect);
    }
}

// رسم دائرة ممتلئة (للأيقونات والأفاتار ومؤشرات الحالة)
static void drawFilledCircle(SDL_Renderer* r, int cx, int cy, int radius, RGBA color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    for (int dy = -radius; dy <= radius; dy++)
        for (int dx = -radius; dx <= radius; dx++)
            if (dx*dx + dy*dy <= radius*radius)
                SDL_RenderDrawPoint(r, cx+dx, cy+dy);
}

// رسم خط أفقي
static void drawHLine(SDL_Renderer* r, int x, int y, int w, RGBA color) {
    SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(r, x, y, x+w, y);
}

// ═══════════════════════════════════════════════════════════════════
// القسم 6: حساب التخطيط (Layout) — تخطيط تلقائي تكراري
// ─────────────────────────────────────────────────────────────────
// يحسب موضع وحجم كل عنصر بناءً على نوعه:
//   Column: يوزّع الأبناء عمودياً (من أعلى لأسفل)
//   Row: يوزّع الأبناء أفقياً (من يسار ليمين)
//   Stack: يضع الأبناء فوق بعض
//   GridView: شبكة بأعمدة محددة
//   Center: يضع الابن في المنتصف
// ═══════════════════════════════════════════════════════════════════

// تقدير ارتفاع عنصر بدون أبناء (أوراق الشجرة)
static float estimateLeafHeight(Widget* child) {
    int scale = isFontLoaded() ? (int)child->fontSize : std::max(1, (int)(child->fontSize / 7));
    float th = (float)measureTextHeight(scale);
    switch (child->type) {
        case WidgetType::Text: case WidgetType::Label: case WidgetType::Heading:
        case WidgetType::Paragraph: case WidgetType::Link: case WidgetType::Quote:
        case WidgetType::Markdown: case WidgetType::CodeBlock:
            return th + 12;
        case WidgetType::Button: case WidgetType::OutlineButton:
        case WidgetType::TextButton: case WidgetType::ToggleButton:
        case WidgetType::IconButton: case WidgetType::FloatingButton:
            return th + 30;
        case WidgetType::TextField: case WidgetType::SearchField:
        case WidgetType::PasswordField: case WidgetType::TextArea:
        case WidgetType::NumberInput: case WidgetType::DateField:
            return (child->type == WidgetType::TextArea) ? th * 4 + 20 : th + 20;
        case WidgetType::Checkbox: case WidgetType::Radio:
        case WidgetType::Switch:
            return std::max(th + 8, 30.0f);
        case WidgetType::Slider: case WidgetType::RangeSlider:
            return 40;
        case WidgetType::Dropdown:
            return th + 24;
        case WidgetType::ProgressBar: return 24;
        case WidgetType::CircleProgress: return 60;
        case WidgetType::LoadingSpinner: return 40;
        case WidgetType::Skeleton: return 20;
        case WidgetType::Divider: case WidgetType::Separator: return 2;
        case WidgetType::Spacer: return (child->fixedHeight > 0) ? child->fixedHeight : 8;
        case WidgetType::ListTile: case WidgetType::DetailRow:
        case WidgetType::NavItem: case WidgetType::MenuItem:
            return th + 24;
        case WidgetType::Avatar: return 48;
        case WidgetType::Badge: return 24;
        case WidgetType::Chip: case WidgetType::Tag: return th + 12;
        case WidgetType::StatusDot: return 16;
        case WidgetType::ColorBox: case WidgetType::GradientBox: return 40;
        case WidgetType::RatingStars: return 30;
        case WidgetType::DotIndicator: return 20;
        case WidgetType::StepIndicator: return 40;
        case WidgetType::Tooltip: return th + 16;
        case WidgetType::Gauge: return 80;
        case WidgetType::SparkLine: return 40;
        default: return 40;
    }
}

// تقدير عرض عنصر ورقي — [FIX #31-38] تحسينات شاملة لتقدير أعرض العناصر
static float estimateLeafWidth(Widget* child) {
    int scale = isFontLoaded() ? (int)child->fontSize : std::max(1, (int)(child->fontSize / 7));
    float tw = (float)measureTextWidth(child->text, scale);
    switch (child->type) {
        case WidgetType::Button: case WidgetType::OutlineButton:
        case WidgetType::TextButton: case WidgetType::ToggleButton:
            return tw + 32; // [FIX #31] مساحة أكبر للأزرار
        case WidgetType::Text: case WidgetType::Label:
        case WidgetType::Paragraph: case WidgetType::Link:
        case WidgetType::Quote: case WidgetType::Markdown:
            return tw + 16; // [FIX #32] دعم أنواع نصية إضافية
        case WidgetType::Heading:
            return tw + 24; // [FIX #33] عنوان أوسع
        case WidgetType::IconButton:
            return std::max(40.0f, child->fixedWidth); // [FIX #34]
        case WidgetType::FloatingButton:
            return std::max(56.0f, child->fixedWidth); // [FIX #35]
        case WidgetType::Checkbox: case WidgetType::Radio:
            return tw + 34; // [FIX #36] مسافة للمربع + النص
        case WidgetType::Switch:
            return tw + 56; // [FIX #37] مفتاح التبديل + النص
        case WidgetType::Chip: case WidgetType::Tag: case WidgetType::Badge:
            return tw + 20; // [FIX #38]
        case WidgetType::Avatar:
            return std::max(48.0f, child->fixedWidth);
        case WidgetType::StatusDot:
            return 16;
        case WidgetType::Divider: case WidgetType::Separator:
            return 0; // يأخذ عرض الأب
        case WidgetType::Slider: case WidgetType::RangeSlider:
            return 120;
        case WidgetType::ProgressBar:
            return 100;
        case WidgetType::CircleProgress: case WidgetType::LoadingSpinner:
            return std::max(40.0f, child->fixedWidth);
        default: return std::max(40.0f, tw + 16);
    }
}

// الدالة الرئيسية لحساب التخطيط — تُستدعى تكرارياً
void layoutWidget(int widgetId, float x, float y, float availW, float availH) {
    Widget* w = getWidget(widgetId);
    if (!w || !w->visible) return;

    // [FIX #70] دعم الهامش (margin) — تقليص المساحة المتاحة
    float mT = w->marginTop, mR = w->marginRight, mB = w->marginBottom, mL = w->marginLeft;
    x += mL; y += mT;
    availW -= (mL + mR);
    availH -= (mT + mB);
    if (availW < 0) availW = 0;
    if (availH < 0) availH = 0;

    float myW = (w->fixedWidth > 0) ? w->fixedWidth : availW;
    float myH = (w->fixedHeight > 0) ? w->fixedHeight : availH;

    // [FIX #71] تطبيق القيود (min/max constraints)
    if (w->minWidth > 0 && myW < w->minWidth) myW = w->minWidth;
    if (w->maxWidth > 0 && myW > w->maxWidth) myW = w->maxWidth;
    if (w->minHeight > 0 && myH < w->minHeight) myH = w->minHeight;
    if (w->maxHeight > 0 && myH > w->maxHeight) myH = w->maxHeight;

    w->cx = x; w->cy = y; w->cw = myW; w->ch = myH;

    // حساب الحشوة (padding) — مسافة داخلية
    float pT = (w->padding > 0) ? w->padding : w->padTop;
    float pR = (w->padding > 0) ? w->padding : w->padRight;
    float pB = (w->padding > 0) ? w->padding : w->padBottom;
    float pL = (w->padding > 0) ? w->padding : w->padLeft;
    float iX = x+pL, iY = y+pT;
    float iW = myW-pL-pR, iH = myH-pT-pB;

    // إذا لم يكن له أبناء → حساب حجم تلقائي
    if (w->children.empty()) {
        if (w->fixedHeight < 0) w->ch = estimateLeafHeight(w) + pT + pB;
        return;
    }

    // ─── تخطيط عمودي (Column) ───
    if (w->type == WidgetType::Column || w->type == WidgetType::ListView ||
        w->type == WidgetType::ScrollView || w->type == WidgetType::Card ||
        w->type == WidgetType::Panel || w->type == WidgetType::GroupBox ||
        w->type == WidgetType::InfoCard || w->type == WidgetType::Accordion ||
        w->type == WidgetType::ExpansionPanel || w->type == WidgetType::Drawer ||
        w->type == WidgetType::SideMenu || w->type == WidgetType::Modal ||
        w->type == WidgetType::Dialog || w->type == WidgetType::ConfirmDialog) {
        float fixedH = 0, flexSum = 0;
        int cnt = 0;
        // المرور الأول: حساب المساحة الثابتة ومجموع flex
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            cnt++;
            if (c->flex > 0) flexSum += c->flex;
            else if (c->fixedHeight > 0) fixedH += c->fixedHeight;
            else fixedH += estimateLeafHeight(c);
        }
        float gaps = (cnt > 1) ? (cnt-1) * w->spacing : 0;
        float remain = iH - fixedH - gaps;
        if (remain < 0) remain = 0;
        // المرور الثاني: تخصيص المواضع والأحجام
        float curY = iY;
        // [FIX #72] ScrollView — تطبيق إزاحة التمرير
        if (w->type == WidgetType::ScrollView || w->type == WidgetType::ListView) {
            curY -= w->scrollY;
        }
        // [FIX #39-44] دعم mainAxisAlignment للعمود
        float effectiveSpacing = (w->gap > 0) ? w->gap : w->spacing; // [FIX #40] gap بديل
        float totalUsedH = fixedH + (cnt > 1 ? (cnt-1) * effectiveSpacing : 0);
        if (w->mainAxisAlignment == "center" || w->mainAxisAlignment == "\xd9\x88\xd8\xb3\xd8\xb7") {
            // [FIX #39] توسيط عمودي
            curY += (iH - totalUsedH) / 2;
        } else if (w->mainAxisAlignment == "end" || w->mainAxisAlignment == "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9") {
            // [FIX #41] محاذاة للنهاية
            curY += iH - totalUsedH;
        } else if (w->mainAxisAlignment == "spaceBetween" || w->mainAxisAlignment == "\xd8\xaa\xd9\x88\xd8\xb2\xd9\x8a\xd8\xb9_\xd9\x85\xd8\xaa\xd8\xb3\xd8\xa7\xd9\x88\xd9\x8a") {
            // [FIX #42] توزيع متساوي مع فراغ بين العناصر
            if (cnt > 1) effectiveSpacing = (iH - fixedH) / (cnt - 1);
        } else if (w->mainAxisAlignment == "spaceEvenly" || w->mainAxisAlignment == "\xd8\xaa\xd9\x88\xd8\xb2\xd9\x8a\xd8\xb9_\xd9\x85\xd8\xaa\xd8\xb3\xd8\xa7\xd9\x88\xd9\x8a_\xd9\x83\xd9\x84\xd9\x8a") {
            // [FIX #43] توزيع متساوي مع فراغ حول كل عنصر
            if (cnt > 0) {
                effectiveSpacing = (iH - fixedH) / (cnt + 1);
                curY += effectiveSpacing;
            }
        } else if (w->mainAxisAlignment == "spaceAround" || w->mainAxisAlignment == "\xd8\xaa\xd9\x88\xd8\xb2\xd9\x8a\xd8\xb9_\xd9\x85\xd8\xad\xd9\x8a\xd8\xb7") {
            // [FIX #44] نصف فراغ في البداية/النهاية، فراغ كامل بين
            if (cnt > 0) {
                float totalSpace = iH - fixedH;
                effectiveSpacing = totalSpace / cnt;
                curY += effectiveSpacing / 2;
            }
        }
        // [FIX #45] عكس ترتيب الأبناء
        std::vector<int> orderedChildren = w->children;
        if (w->reverse) std::reverse(orderedChildren.begin(), orderedChildren.end());
        for (int cid : orderedChildren) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            float ch;
            if (c->flex > 0 && flexSum > 0) ch = remain * (c->flex / flexSum);
            else if (c->fixedHeight > 0) ch = c->fixedHeight;
            else ch = estimateLeafHeight(c);
            // [FIX #73] crossAxisAlignment — محاذاة عرضية للعمود
            float childX = iX;
            float childW = iW;
            if (w->crossAxisAlignment == "center" || w->crossAxisAlignment == "وسط") {
                float cw = (c->fixedWidth > 0) ? c->fixedWidth : iW;
                childX = iX + (iW - cw) / 2; childW = cw;
            } else if (w->crossAxisAlignment == "end" || w->crossAxisAlignment == "نهاية") {
                float cw = (c->fixedWidth > 0) ? c->fixedWidth : iW;
                childX = iX + iW - cw; childW = cw;
            }
            layoutWidget(cid, childX, curY, childW, ch);
            curY += c->ch + effectiveSpacing; // [FIX #46] استخدام التباعد الفعال
        }
        // [FIX #74] حفظ إجمالي المحتوى للتمرير — [FIX #47] استخدام effectiveSpacing
        float totalContentH = curY - iY + (w->scrollY) - effectiveSpacing;
        if (w->type == WidgetType::ScrollView || w->type == WidgetType::ListView) {
            w->contentHeight = totalContentH;
        }
        if (w->fixedHeight < 0 && !w->children.empty())
            w->ch = (curY - iY + (w->type == WidgetType::ScrollView ? w->scrollY : 0)) - effectiveSpacing + pT + pB;
    }
    // ─── تخطيط أفقي (Row) ───
    else if (w->type == WidgetType::Row || w->type == WidgetType::ButtonGroup ||
             w->type == WidgetType::ToggleGroup || w->type == WidgetType::Toolbar ||
             w->type == WidgetType::Breadcrumb) {
        float fixedW = 0, flexSum = 0;
        int cnt = 0;
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            cnt++;
            if (c->flex > 0) flexSum += c->flex;
            else if (c->fixedWidth > 0) fixedW += c->fixedWidth;
            else fixedW += estimateLeafWidth(c);
        }
        float effectiveSpacing = (w->gap > 0) ? w->gap : w->spacing; // [FIX #48] gap بديل
        float gaps = (cnt > 1) ? (cnt-1) * effectiveSpacing : 0;
        float remain = iW - fixedW - gaps;
        if (remain < 0) remain = 0;
        float curX = iX;
        // [FIX #49-54] mainAxisAlignment للصف — دعم كامل
        if (w->mainAxisAlignment == "center" || w->mainAxisAlignment == "\xd9\x88\xd8\xb3\xd8\xb7") {
            float usedW = fixedW + gaps;
            curX = iX + (iW - usedW) / 2;
        } else if (w->mainAxisAlignment == "end" || w->mainAxisAlignment == "\xd9\x86\xd9\x87\xd8\xa7\xd9\x8a\xd8\xa9") {
            float usedW = fixedW + gaps;
            curX = iX + iW - usedW;
        } else if (w->mainAxisAlignment == "spaceBetween" || w->mainAxisAlignment == "\xd8\xaa\xd9\x88\xd8\xb2\xd9\x8a\xd8\xb9_\xd9\x85\xd8\xaa\xd8\xb3\xd8\xa7\xd9\x88\xd9\x8a") {
            // [FIX #50] spaceBetween — فراغ متساوي بين العناصر
            if (cnt > 1) effectiveSpacing = (iW - fixedW) / (cnt - 1);
        } else if (w->mainAxisAlignment == "spaceEvenly" || w->mainAxisAlignment == "\xd8\xaa\xd9\x88\xd8\xb2\xd9\x8a\xd8\xb9_\xd9\x85\xd8\xaa\xd8\xb3\xd8\xa7\xd9\x88\xd9\x8a_\xd9\x83\xd9\x84\xd9\x8a") {
            // [FIX #51] spaceEvenly — فراغ متساوي حول كل عنصر
            if (cnt > 0) {
                effectiveSpacing = (iW - fixedW) / (cnt + 1);
                curX += effectiveSpacing;
            }
        } else if (w->mainAxisAlignment == "spaceAround" || w->mainAxisAlignment == "\xd8\xaa\xd9\x88\xd8\xb2\xd9\x8a\xd8\xb9_\xd9\x85\xd8\xad\xd9\x8a\xd8\xb7") {
            // [FIX #52] spaceAround
            if (cnt > 0) {
                effectiveSpacing = (iW - fixedW) / cnt;
                curX += effectiveSpacing / 2;
            }
        }
        // [FIX #53] عكس ترتيب الأبناء في الصف
        std::vector<int> orderedChildren = w->children;
        if (w->reverse) std::reverse(orderedChildren.begin(), orderedChildren.end());
        for (int cid : orderedChildren) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            float cw;
            if (c->flex > 0 && flexSum > 0) cw = remain * (c->flex / flexSum);
            else if (c->fixedWidth > 0) cw = c->fixedWidth;
            else cw = estimateLeafWidth(c);
            // [FIX #76] crossAxisAlignment للصف (محاذاة عمودية)
            float childY = iY;
            float childH = iH;
            if (w->crossAxisAlignment == "center" || w->crossAxisAlignment == "وسط") {
                float ch = (c->fixedHeight > 0) ? c->fixedHeight : estimateLeafHeight(c);
                childY = iY + (iH - ch) / 2; childH = ch;
            } else if (w->crossAxisAlignment == "end" || w->crossAxisAlignment == "نهاية") {
                float ch = (c->fixedHeight > 0) ? c->fixedHeight : estimateLeafHeight(c);
                childY = iY + iH - ch; childH = ch;
            }
            layoutWidget(cid, curX, childY, cw, childH);
            curX += c->cw + effectiveSpacing; // [FIX #54] استخدام تباعد فعال
        }
    }
    // ─── تخطيط مكدّس (Stack) — الأبناء فوق بعض ───
    else if (w->type == WidgetType::Stack) {
        for (int cid : w->children) layoutWidget(cid, iX, iY, iW, iH);
    }
    // ─── توسيط (Center) ───
    else if (w->type == WidgetType::Center) {
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            float cw = (c->fixedWidth > 0) ? c->fixedWidth : iW * 0.5f;
            float ch = (c->fixedHeight > 0) ? c->fixedHeight : estimateLeafHeight(c);
            layoutWidget(cid, iX + (iW-cw)/2, iY + (iH-ch)/2, cw, ch);
        }
    }
    // ─── شبكة (GridView) / التفاف (Wrap) ───  [FIX #55-62]
    else if (w->type == WidgetType::GridView || w->type == WidgetType::Wrap) {
        float effSpacing = (w->gap > 0) ? w->gap : w->spacing; // [FIX #55]
        int cols = std::max(1, w->columns);
        float cellW = (iW - (cols-1)*effSpacing) / cols;
        float curX = iX, curY = iY;
        int col = 0;
        float rowMaxH = 0; // [FIX #56] تتبع أعلى عنصر في الصف
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            float ch = (c->fixedHeight > 0) ? c->fixedHeight : estimateLeafHeight(c);
            // [FIX #57] للالتفاف: حساب العرض المطلوب
            float useW = cellW;
            if (w->type == WidgetType::Wrap) {
                useW = (c->fixedWidth > 0) ? c->fixedWidth : estimateLeafWidth(c);
                if (curX + useW > iX + iW && col > 0) {
                    // [FIX #58] انتقال لصف جديد
                    col = 0; curX = iX; curY += rowMaxH + effSpacing;
                    rowMaxH = 0;
                }
            }
            layoutWidget(cid, curX, curY, useW, ch);
            if (c->ch > rowMaxH) rowMaxH = c->ch; // [FIX #59] تتبع أقصى ارتفاع
            col++;
            if (w->type == WidgetType::GridView) {
                if (col >= cols) { col = 0; curX = iX; curY += rowMaxH + effSpacing; rowMaxH = 0; }
                else curX += cellW + effSpacing;
            } else {
                curX += c->cw + effSpacing;
            }
        }
        // [FIX #60] حساب الارتفاع الإجمالي
        if (w->fixedHeight < 0) {
            w->ch = curY + rowMaxH - iY + pT + pB;
        }
    }
    // ─── [FIX #61] SizedBox — يمرر التخطيط للابن مع حجم ثابت ───
    else if (w->type == WidgetType::SizedBox) {
        for (int cid : w->children)
            layoutWidget(cid, iX, iY, iW, iH);
    }
    // ─── [FIX #62] AppBar — تخطيط أفقي للعنوان والأبناء ───
    else if (w->type == WidgetType::AppBar) {
        float titleW = iW * 0.5f;
        float actionsX = iX + titleW;
        float actionsW = iW - titleW;
        int childIdx = 0;
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            float cw = (c->fixedWidth > 0) ? c->fixedWidth : 48;
            layoutWidget(cid, actionsX, iY, cw, iH);
            actionsX += cw + 4;
            childIdx++;
        }
    }
    // ─── [FIX #63] BottomBar — تقسيم أفقي متساوي ───
    else if (w->type == WidgetType::BottomBar) {
        int cnt = 0;
        for (int cid : w->children) { Widget* c = getWidget(cid); if (c && c->visible) cnt++; }
        if (cnt > 0) {
            float itemW = iW / cnt;
            float curX = iX;
            for (int cid : w->children) {
                Widget* c = getWidget(cid);
                if (!c || !c->visible) continue;
                layoutWidget(cid, curX, iY, itemW, iH);
                curX += itemW;
            }
        }
    }
    // ─── [FIX #64] TabBar — تقسيم أفقي للأبناء مع مؤشر ───
    else if (w->type == WidgetType::TabBar) {
        int cnt = 0;
        for (int cid : w->children) { Widget* c = getWidget(cid); if (c && c->visible) cnt++; }
        if (cnt > 0) {
            float tabW = iW / cnt;
            float curX = iX;
            for (int cid : w->children) {
                Widget* c = getWidget(cid);
                if (!c || !c->visible) continue;
                layoutWidget(cid, curX, iY, tabW, iH);
                curX += tabW;
            }
        }
    }
    // ─── [FIX #65] Table — تقسيم شبكي بصفوف وأعمدة ───
    else if (w->type == WidgetType::Table || w->type == WidgetType::DataTable) {
        int cols = std::max(1, w->columns);
        float colW = iW / cols;
        float curY = iY;
        int colIdx = 0;
        float rowH = 0;
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            // [FIX #66] TableRow يأخذ الصف كاملاً
            if (c->type == WidgetType::TableRow) {
                float rowChildX = iX;
                float estimatedRowH = 36;
                for (int rcid : c->children) {
                    Widget* rc = getWidget(rcid);
                    if (!rc || !rc->visible) continue;
                    layoutWidget(rcid, rowChildX, curY, colW, estimatedRowH);
                    rowChildX += colW;
                }
                c->cx = iX; c->cy = curY; c->cw = iW; c->ch = estimatedRowH;
                curY += estimatedRowH;
            } else {
                // خلايا مباشرة
                float ch = (c->fixedHeight > 0) ? c->fixedHeight : estimateLeafHeight(c);
                if (ch > rowH) rowH = ch;
                layoutWidget(cid, iX + colIdx * colW, curY, colW, ch);
                colIdx++;
                if (colIdx >= cols) {
                    colIdx = 0; curY += rowH + 2; rowH = 0;
                }
            }
        }
    }
    // ─── [FIX #67] PageView — إظهار صفحة واحدة فقط ───
    else if (w->type == WidgetType::PageView) {
        int page = std::max(0, std::min(w->currentPage, (int)w->children.size() - 1));
        for (int i = 0; i < (int)w->children.size(); i++) {
            Widget* c = getWidget(w->children[i]);
            if (!c) continue;
            if (i == page) {
                c->visible = true;
                layoutWidget(w->children[i], iX, iY, iW, iH);
            } else {
                c->visible = false;
            }
        }
    }
    // ─── [FIX #68] Carousel — إظهار 3 صفحات مع تمرير ───
    else if (w->type == WidgetType::Carousel) {
        int page = std::max(0, std::min(w->currentPage, (int)w->children.size() - 1));
        for (int i = 0; i < (int)w->children.size(); i++) {
            Widget* c = getWidget(w->children[i]);
            if (!c) continue;
            float offset = (float)(i - page) * iW;
            c->visible = (std::abs(i - page) <= 1);
            if (c->visible) layoutWidget(w->children[i], iX + offset, iY, iW, iH);
        }
    }
    // ─── [FIX #69] SplitView — تقسيم ثنائي أفقي ───
    else if (w->type == WidgetType::SplitView) {
        float splitRatio = (w->value > 0 && w->value < 1) ? w->value : 0.5f;
        float divW = 4;
        float leftW = (iW - divW) * splitRatio;
        float rightW = iW - leftW - divW;
        int idx = 0;
        for (int cid : w->children) {
            Widget* c = getWidget(cid);
            if (!c || !c->visible) continue;
            if (idx == 0) layoutWidget(cid, iX, iY, leftW, iH);
            else if (idx == 1) layoutWidget(cid, iX + leftW + divW, iY, rightW, iH);
            idx++;
        }
    }
    // ─── [FIX #70] Notification/Toast — تثبيت في أسفل الشاشة ───
    else if (w->type == WidgetType::Notification || w->type == WidgetType::Toast ||
             w->type == WidgetType::Snackbar) {
        for (int cid : w->children)
            layoutWidget(cid, iX, iY, iW, iH);
    }
    // ─── حاوية عامة — ابن واحد يملأ المساحة ───
    else {
        for (int cid : w->children)
            layoutWidget(cid, iX, iY, iW, iH);
    }
}

// ═══════════════════════════════════════════════════════════════════
// القسم 7: محرك الرسم (Renderer) — يرسم كل نوع عنصر
// ─────────────────────────────────────────────────────────────────
// يُستدعى تكرارياً لكل عنصر في الشجرة
// كل نوع عنصر له شكل مختلف (زر، حقل، مؤشر، رسم بياني...)
// ═══════════════════════════════════════════════════════════════════

void renderWidget(SDL_Renderer* r, int widgetId) {
    Widget* w = getWidget(widgetId);
    if (!w || !w->visible) return;

    // [FIX #80] فحص الشفافية — لا ترسم عنصر بشفافية 0
    if (w->opacity <= 0.0f) return;
    // [FIX #131] فحص العنصر المعطل — تقليل الشفافية
    float effectiveOpacity = w->opacity;
    if (!w->enabled) effectiveOpacity *= 0.45f; // [FIX #132] عنصر معطل = نصف شفاف

    int x=(int)w->cx, y=(int)w->cy, ww=(int)w->cw, hh=(int)w->ch;
    // HD: fontSize يُستخدم مباشرة كحجم بكسلات TrueType
    // نقطي احتياطي: يُحوَّل لمقياس (scale = fontSize/7)
    int scale = isFontLoaded() ? (int)w->fontSize : std::max(1, (int)(w->fontSize / 7));

    // [FIX #81] تطبيق الشفافية (opacity) على الرسم — [FIX #133] استخدام الشفافية الفعالة
    uint8_t origAlpha = 255;
    if (effectiveOpacity < 1.0f) {
        origAlpha = (uint8_t)(effectiveOpacity * 255);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    }

    // [FIX #134] رسم الظل التلقائي (elevation)
    float shadowR = w->shadowRadius;
    if (shadowR <= 0 && w->elevation > 0) {
        shadowR = w->elevation * 1.5f;
        w->shadowOffsetY = w->elevation * 0.5f;
    }

    // [FIX #82] رسم الظل (Shadow / Elevation) قبل العنصر — [FIX #135] استخدام shadowR + effectiveOpacity
    if (shadowR > 0) {
        RGBA sc = w->shadowColor;
        sc.a = (uint8_t)(sc.a * effectiveOpacity);
        int sr = (int)shadowR;
        int sx = x + (int)w->shadowOffsetX;
        int sy = y + (int)w->shadowOffsetY;
        for (int i = sr; i > 0; i -= 2) {
            RGBA layerC = {sc.r, sc.g, sc.b, (uint8_t)(sc.a * i / (sr * 3))};
            drawRoundedRect(r, sx-i, sy-i, ww+2*i, hh+2*i, (int)w->borderRadius+i, layerC);
        }
    }

    // [FIX #83] رسم التدرج اللوني (Gradient) كخلفية — [FIX #136] دعم الاتجاه الأفقي والقطري
    if (w->hasGradient && w->gradientStart.a > 0 && w->gradientEnd.a > 0) {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        bool isHoriz = (w->gradientDirection == "horizontal" || w->gradientDirection == "\xd8\xa3" "فقي");
        for (int row = 0; row < hh; row++) {
            for (int col = 0; col < ww; col++) {
                float t;
                if (isHoriz) {
                    t = (ww > 1) ? (float)col / (float)(ww - 1) : 0.0f;
                } else {
                    t = (hh > 1) ? (float)row / (float)(hh - 1) : 0.0f;
                }
                uint8_t cr2 = (uint8_t)(w->gradientStart.r + t * (w->gradientEnd.r - w->gradientStart.r));
                uint8_t cg2 = (uint8_t)(w->gradientStart.g + t * (w->gradientEnd.g - w->gradientStart.g));
                uint8_t cb2 = (uint8_t)(w->gradientStart.b + t * (w->gradientEnd.b - w->gradientStart.b));
                uint8_t ca2 = (uint8_t)((w->gradientStart.a + t * (w->gradientEnd.a - w->gradientStart.a)) * effectiveOpacity);
                SDL_SetRenderDrawColor(r, cr2, cg2, cb2, ca2);
                SDL_RenderDrawPoint(r, x + col, y + row);
            }
        }
    }
    // [FIX #137] رسم الخلفية — بتطبيق الشفافية الفعالة + دعم shape="circle"
    else if (w->bgColor.a > 0) {
        RGBA bg = w->bgColor;
        bg.a = (uint8_t)(bg.a * effectiveOpacity);
        if (w->shape == "circle" || w->shape == "\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd8\xa9") {
            int cx2 = x + ww/2, cy2 = y + hh/2, rad = std::min(ww, hh)/2;
            for (int dy2 = -rad; dy2 <= rad; dy2++) {
                int dx2 = (int)std::sqrt((float)(rad*rad - dy2*dy2));
                SDL_SetRenderDrawColor(r, bg.r, bg.g, bg.b, bg.a);
                SDL_RenderDrawLine(r, cx2 - dx2, cy2 + dy2, cx2 + dx2, cy2 + dy2);
            }
        } else {
            drawRoundedRect(r, x, y, ww, hh, (int)w->borderRadius, bg);
        }
    }
    // [FIX #138] تراكب اللون (overlayColor) فوق الخلفية
    if (w->overlayColor.a > 0) {
        RGBA ov = w->overlayColor;
        ov.a = (uint8_t)(ov.a * effectiveOpacity);
        drawRoundedRect(r, x, y, ww, hh, (int)w->borderRadius, ov);
    }
    // [FIX #139] رسم الحدود — دعم الحدود المنفصلة لكل جهة
    if (w->borderWidth > 0) {
        RGBA bc = w->borderColor;
        bc.a = (uint8_t)(bc.a * effectiveOpacity);
        drawRectBorder(r, x, y, ww, hh, (int)w->borderWidth, bc);
    } else {
        // [FIX #140] حدود منفصلة لكل جهة (Top/Right/Bottom/Left)
        RGBA bc = w->borderColor;
        bc.a = (uint8_t)(bc.a * effectiveOpacity);
        if (w->borderTopWidth > 0) {
            SDL_SetRenderDrawColor(r, bc.r, bc.g, bc.b, bc.a);
            for (int i = 0; i < (int)w->borderTopWidth; i++) {
                SDL_RenderDrawLine(r, x, y+i, x+ww-1, y+i);
            }
        }
        if (w->borderBottomWidth > 0) {
            SDL_SetRenderDrawColor(r, bc.r, bc.g, bc.b, bc.a);
            for (int i = 0; i < (int)w->borderBottomWidth; i++) {
                SDL_RenderDrawLine(r, x, y+hh-1-i, x+ww-1, y+hh-1-i);
            }
        }
        if (w->borderLeftWidth > 0) {
            SDL_SetRenderDrawColor(r, bc.r, bc.g, bc.b, bc.a);
            for (int i = 0; i < (int)w->borderLeftWidth; i++) {
                SDL_RenderDrawLine(r, x+i, y, x+i, y+hh-1);
            }
        }
        if (w->borderRightWidth > 0) {
            SDL_SetRenderDrawColor(r, bc.r, bc.g, bc.b, bc.a);
            for (int i = 0; i < (int)w->borderRightWidth; i++) {
                SDL_RenderDrawLine(r, x+ww-1-i, y, x+ww-1-i, y+hh-1);
            }
        }
    }
    // [FIX #141] حلقة التركيز (Focus Ring) للعناصر القابلة للتركيز
    if (w->hasFocus && w->focusable) {
        RGBA fc = w->focusColor;
        fc.a = (uint8_t)(fc.a * effectiveOpacity);
        drawRectBorder(r, x-2, y-2, ww+4, hh+4, 2, fc);
    }
    // [FIX #142] مؤشر الخطأ (Error State) لحقول الإدخال
    if (w->hasError) {
        RGBA errC = {220, 38, 38, (uint8_t)(200 * effectiveOpacity)};
        drawRectBorder(r, x, y, ww, hh, 2, errC);
    }

    // [FIX #84] قص (clip) للعناصر القابلة للتمرير
    SDL_Rect clipRect = {x, y, ww, hh};
    SDL_Rect prevClip;
    bool hadClip = (SDL_RenderIsClipEnabled(r) == SDL_TRUE);
    if (hadClip) SDL_RenderGetClipRect(r, &prevClip);
    bool needsClip = (w->overflow == "clip" || w->overflow == "scroll" ||
                      w->type == WidgetType::ScrollView || w->type == WidgetType::ListView);
    if (needsClip) SDL_RenderSetClipRect(r, &clipRect);

    // ─── رسم حسب نوع العنصر ───
    switch (w->type) {

    // ─── نص عادي ─── [FIX #143] دعم textTransform + textOverflow + textDecoration
    case WidgetType::Text: case WidgetType::Label: case WidgetType::Paragraph:
    case WidgetType::Markdown: {
        int tH = measureTextHeight(scale);
        int tY = y + (hh - tH) / 2;
        float pL = (w->padding > 0) ? w->padding : w->padLeft;
        // [FIX #144] تحويل النص (uppercase/lowercase)
        std::string displayText = w->text;
        if (w->textTransform == "uppercase" || w->textTransform == "\xd8\xa3\xd8\xb9\xd9\x84\xd9\x89") {
            for (auto& ch : displayText) ch = (char)toupper((unsigned char)ch);
        }
        // [FIX #145] قطع النص الطويل بـ...
        int maxW = ww - (int)pL * 2;
        if (w->textOverflow == "ellipsis" && maxW > 0) {
            int tw = measureTextWidth(displayText, scale);
            if (tw > maxW) {
                while (displayText.size() > 3 && measureTextWidth(displayText + "...", scale) > maxW)
                    displayText.pop_back();
                displayText += "...";
            }
        }
        RGBA tc = w->textColor;
        tc.a = (uint8_t)(tc.a * effectiveOpacity);
        drawText(r, displayText, x+(int)pL, tY, scale, tc, w->textAlign, maxW);
        // [FIX #146] زخرفة النص (underline/strikethrough)
        if (w->textDecoration == "underline" || w->textDecoration == "\xd8\xaa\xd8\xad\xd8\xaa_\xd8\xae\xd8\xb7") {
            int tw = measureTextWidth(displayText, scale);
            int lx = x + (int)pL;
            if (w->textAlign == "center" || w->textAlign == "\xd9\x88\xd8\xb3\xd8\xb7") lx = x + (ww - tw) / 2;
            else if (w->textAlign == "right" || w->textAlign == "\xd9\x8a\xd9\x85\xd9\x8a\xd9\x86") lx = x + ww - (int)pL - tw;
            drawHLine(r, lx, tY + tH + 1, tw, tc);
        } else if (w->textDecoration == "strikethrough" || w->textDecoration == "\xd9\x8a\xd8\xaa\xd9\x88\xd8\xb3\xd8\xb7_\xd8\xae\xd8\xb7") {
            int tw = measureTextWidth(displayText, scale);
            int lx = x + (int)pL;
            if (w->textAlign == "center") lx = x + (ww - tw) / 2;
            drawHLine(r, lx, tY + tH / 2, tw, tc);
        }
        break;
    }
    // ─── عنوان (بحجم خط أكبر حسب المستوى) ───
    case WidgetType::Heading: {
        int hScale = std::max(1, scale + (4 - std::min(w->level, 4)));
        int tH = measureTextHeight(hScale);
        int tY = y + (hh - tH) / 2;
        drawText(r, w->text, x+4, tY, hScale, w->textColor, w->textAlign, ww-8);
        break;
    }
    // ─── رابط (نص بلون أزرق) ───
    case WidgetType::Link: {
        int tH = measureTextHeight(scale);
        int tY = y + (hh - tH) / 2;
        RGBA linkC = {30, 100, 220, 255};
        drawText(r, w->text, x+4, tY, scale, linkC, w->textAlign, ww-8);
        // خط تحت النص
        int tw = measureTextWidth(w->text, scale);
        int lx = x + 4;
        if (w->textAlign == "center") lx = x + (ww - tw) / 2;
        drawHLine(r, lx, tY + tH + 1, tw, linkC);
        break;
    }
    // ─── اقتباس (شريط أيسر ملون) ───
    case WidgetType::Quote: {
        RGBA bar = {66, 133, 244, 255};
        SDL_SetRenderDrawColor(r, bar.r, bar.g, bar.b, bar.a);
        SDL_Rect qr = {x, y, 4, hh};
        SDL_RenderFillRect(r, &qr);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y + (hh-tH)/2, scale, {100,100,100,255}, "left", ww-16);
        break;
    }
    // ─── كتلة كود (خلفية داكنة) ───
    case WidgetType::CodeBlock: {
        drawRoundedRect(r, x, y, ww, hh, 4, {40,44,52,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+8, y + (hh-tH)/2, scale, {180,220,180,255}, "left", ww-16);
        break;
    }
    // ─── زر عادي ───
    case WidgetType::Button: {
        RGBA bg = w->pressed ? w->btnPress : (w->hovered ? w->btnHover : w->btnColor);
        drawRoundedRect(r, x, y, ww, hh, w->borderRadius > 0 ? (int)w->borderRadius : 8, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, w->textColor, "center", ww);
        break;
    }
    // ─── زر مخطط (بدون تعبئة) ───
    case WidgetType::OutlineButton: {
        int rad = w->borderRadius > 0 ? (int)w->borderRadius : 8;
        if (w->hovered) drawRoundedRect(r, x, y, ww, hh, rad, {230,240,255,255});
        drawRectBorder(r, x, y, ww, hh, 2, w->btnColor);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, w->btnColor, "center", ww);
        break;
    }
    // ─── زر نصي (بدون خلفية) ───
    case WidgetType::TextButton: {
        RGBA tc = w->hovered ? w->btnHover : w->btnColor;
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, tc, "center", ww);
        break;
    }
    // ─── زر تبديل (toggle) ───
    case WidgetType::ToggleButton: {
        RGBA bg = w->checked ? w->btnColor : RGBA{200,200,200,255};
        if (w->hovered) bg = w->checked ? w->btnHover : RGBA{220,220,220,255};
        drawRoundedRect(r, x, y, ww, hh, 8, bg);
        RGBA tc = w->checked ? RGBA{255,255,255,255} : RGBA{60,60,60,255};
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, tc, "center", ww);
        break;
    }
    // ─── زر أيقونة (دائري) ───
    case WidgetType::IconButton: {
        RGBA bg = w->pressed ? w->btnPress : (w->hovered ? RGBA{230,230,230,255} : RGBA{0,0,0,0});
        int rad = std::min(ww, hh) / 2;
        if (bg.a > 0) drawFilledCircle(r, x+ww/2, y+hh/2, rad, bg);
        char ic[2] = {w->iconChar, 0};
        int tH = measureTextHeight(scale);
        drawText(r, ic, x, y+(hh-tH)/2, scale, w->textColor, "center", ww);
        break;
    }
    // ─── زر عائم (FAB) ─── دائري ملون
    case WidgetType::FloatingButton: {
        RGBA bg = w->pressed ? w->btnPress : (w->hovered ? w->btnHover : w->btnColor);
        int rad = std::min(ww, hh) / 2;
        drawFilledCircle(r, x+ww/2, y+hh/2, rad, bg);
        char ic[2] = {w->iconChar, 0};
        int tH = measureTextHeight(scale);
        drawText(r, ic, x, y+(hh-tH)/2, scale, {255,255,255,255}, "center", ww);
        break;
    }
    // ─── حقل إدخال نص ─── [FIX #147-155] دعم placeholder/errorText/helperText/readOnly/cursor/selection
    case WidgetType::TextField: case WidgetType::SearchField:
    case WidgetType::NumberInput: case WidgetType::DateField: {
        RGBA fbg = (w->bgColor.a > 0) ? w->bgColor : RGBA{255,255,255,255};
        drawRoundedRect(r, x, y, ww, hh, 4, fbg);
        // [FIX #148] لون الحدود حسب الحالة: خطأ > تركيز > عادي
        RGBA bc = w->hasError ? RGBA{220,38,38,255} : (w->inputFocused ? RGBA{66,133,244,255} : RGBA{200,200,200,255});
        drawRectBorder(r, x, y, ww, hh, w->inputFocused ? 2 : 1, bc);
        int tH = measureTextHeight(scale);
        int tY = y + (hh-tH)/2;
        // [FIX #149] placeholder vs inputText
        std::string plText = w->placeholder.empty() ? w->text : w->placeholder;
        if (w->inputText.empty()) {
            drawText(r, plText, x+8, tY, scale, {160,160,160,255}, "left", ww-16);
        } else {
            RGBA tc = w->readOnly ? RGBA{120,120,120,255} : RGBA{33,33,33,255};
            drawText(r, w->inputText, x+8, tY, scale, tc, "left", ww-16);
        }
        // [FIX #150] مؤشر الكتابة + تحديد النص
        if (w->inputFocused && !w->readOnly) {
            // تحديد النص (selection highlight)
            if (w->selectionStart >= 0 && w->selectionEnd >= 0 && w->selectionStart != w->selectionEnd) {
                int s1 = std::min(w->selectionStart, w->selectionEnd);
                int s2 = std::max(w->selectionStart, w->selectionEnd);
                std::string before = w->inputText.substr(0, std::min(s1, (int)w->inputText.size()));
                std::string sel = w->inputText.substr(s1, std::min(s2-s1, (int)w->inputText.size()-s1));
                int sx = x + 8 + measureTextWidth(before, scale);
                int sw = measureTextWidth(sel, scale);
                SDL_SetRenderDrawColor(r, 66, 133, 244, 80);
                SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
                SDL_Rect selR = {sx, tY, sw, tH};
                SDL_RenderFillRect(r, &selR);
            }
            // مؤشر الكتابة
            if ((SDL_GetTicks()/500)%2==0) {
                int cp = w->cursorPosition >= 0 ? std::min(w->cursorPosition, (int)w->inputText.size()) : (int)w->inputText.size();
                std::string beforeCur = w->inputText.substr(0, cp);
                int cx2 = x+8+measureTextWidth(beforeCur, scale);
                SDL_SetRenderDrawColor(r, 33, 33, 33, 255);
                SDL_RenderDrawLine(r, cx2, tY, cx2, tY+tH);
            }
        }
        // [FIX #151] أيقونة بحث
        if (w->type == WidgetType::SearchField) {
            drawFilledCircle(r, x+ww-20, y+hh/2, 6, {180,180,180,255});
        }
        // [FIX #152] نص الخطأ تحت الحقل
        if (w->hasError && !w->errorText.empty()) {
            int errScale = std::max(1, scale - 2);
            int eH = measureTextHeight(errScale);
            drawText(r, w->errorText, x+4, y+hh+2, errScale, {220,38,38,255}, "left", ww-8);
        }
        // [FIX #153] نص المساعدة تحت الحقل
        else if (!w->helperText.empty()) {
            int hlpScale = std::max(1, scale - 2);
            drawText(r, w->helperText, x+4, y+hh+2, hlpScale, {130,130,130,255}, "left", ww-8);
        }
        break;
    }
    // ─── حقل كلمة مرور (نجوم بدل النص) ───
    case WidgetType::PasswordField: {
        drawRoundedRect(r, x, y, ww, hh, 4, {255,255,255,255});
        RGBA bc = w->inputFocused ? RGBA{66,133,244,255} : RGBA{200,200,200,255};
        drawRectBorder(r, x, y, ww, hh, w->inputFocused ? 2 : 1, bc);
        int tH = measureTextHeight(scale);
        int tY = y + (hh-tH)/2;
        if (w->inputText.empty()) {
            drawText(r, w->text, x+8, tY, scale, {160,160,160,255}, "left", ww-16);
        } else {
            std::string masked(w->inputText.size(), '*');
            drawText(r, masked, x+8, tY, scale, {33,33,33,255}, "left", ww-16);
        }
        break;
    }
    // ─── حقل نص متعدد الأسطر ───
    case WidgetType::TextArea: {
        drawRoundedRect(r, x, y, ww, hh, 4, {255,255,255,255});
        drawRectBorder(r, x, y, ww, hh, w->inputFocused ? 2 : 1,
                       w->inputFocused ? RGBA{66,133,244,255} : RGBA{200,200,200,255});
        int tH = measureTextHeight(scale);
        if (w->inputText.empty()) {
            drawText(r, w->text, x+8, y+8, scale, {160,160,160,255}, "left", ww-16);
        } else {
            drawText(r, w->inputText, x+8, y+8, scale, {33,33,33,255}, "left", ww-16);
        }
        break;
    }
    // ─── مربع اختيار (Checkbox) ───
    case WidgetType::Checkbox: {
        // مربع 18×18
        int boxSz = 18, bx = x+4, by = y+(hh-boxSz)/2;
        RGBA boxBg = w->checked ? RGBA{66,133,244,255} : RGBA{255,255,255,255};
        drawRoundedRect(r, bx, by, boxSz, boxSz, 3, boxBg);
        drawRectBorder(r, bx, by, boxSz, boxSz, 1, {150,150,150,255});
        if (w->checked) { // علامة صح ✓
            SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
            SDL_RenderDrawLine(r, bx+4, by+9, bx+7, by+13);
            SDL_RenderDrawLine(r, bx+7, by+13, bx+14, by+5);
        }
        // النص بجانب المربع
        int tH = measureTextHeight(scale);
        drawText(r, w->text, bx+boxSz+8, y+(hh-tH)/2, scale, w->textColor, "left", ww-boxSz-16);
        break;
    }
    // ─── زر دائري (Radio) ───
    case WidgetType::Radio: {
        int rad = 9, cx0 = x+4+rad, cy0 = y+hh/2;
        drawFilledCircle(r, cx0, cy0, rad, {255,255,255,255});
        drawFilledCircle(r, cx0, cy0, rad,
                        w->checked ? RGBA{66,133,244,255} : RGBA{200,200,200,255});
        drawFilledCircle(r, cx0, cy0, rad-2, {255,255,255,255});
        if (w->checked) drawFilledCircle(r, cx0, cy0, 5, {66,133,244,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+4+rad*2+8, y+(hh-tH)/2, scale, w->textColor, "left", ww-rad*2-16);
        break;
    }
    // ─── مفتاح تبديل (Switch) ───
    case WidgetType::Switch: {
        int sw=40, sh=22, sx=x+4, sy=y+(hh-sh)/2;
        RGBA track = w->checked ? RGBA{66,133,244,255} : RGBA{180,180,180,255};
        drawRoundedRect(r, sx, sy, sw, sh, sh/2, track);
        int knobX = w->checked ? sx+sw-sh+2 : sx+2;
        drawFilledCircle(r, knobX+sh/2-2, sy+sh/2, sh/2-3, {255,255,255,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, sx+sw+8, y+(hh-tH)/2, scale, w->textColor, "left", ww-sw-16);
        break;
    }
    // ─── منزلق (Slider) ───
    case WidgetType::Slider: {
        int trackH = 6, trackY = y+hh/2 - trackH/2;
        drawRoundedRect(r, x+4, trackY, ww-8, trackH, 3, {210,210,210,255});
        float pct = (w->maxVal > w->minVal) ? (w->value - w->minVal)/(w->maxVal - w->minVal) : 0;
        pct = std::max(0.0f, std::min(1.0f, pct));
        int fillW = (int)((ww-8) * pct);
        drawRoundedRect(r, x+4, trackY, fillW, trackH, 3, w->accentColor);
        int knobX = x + 4 + fillW;
        drawFilledCircle(r, knobX, y+hh/2, 8, w->accentColor);
        drawFilledCircle(r, knobX, y+hh/2, 5, {255,255,255,255});
        break;
    }
    // ─── منزلق نطاق (RangeSlider) ───
    case WidgetType::RangeSlider: {
        int trackH = 6, trackY = y+hh/2 - trackH/2;
        drawRoundedRect(r, x+4, trackY, ww-8, trackH, 3, {210,210,210,255});
        float range = w->maxVal - w->minVal;
        if (range <= 0) range = 1;
        float p1 = (w->value - w->minVal)/range, p2 = (w->value2 - w->minVal)/range;
        p1 = std::max(0.f, std::min(1.f, p1)); p2 = std::max(0.f, std::min(1.f, p2));
        int x1 = x+4+(int)((ww-8)*p1), x2 = x+4+(int)((ww-8)*p2);
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        SDL_Rect fill = {std::min(x1,x2), trackY, abs(x2-x1), trackH};
        SDL_RenderFillRect(r, &fill);
        drawFilledCircle(r, x1, y+hh/2, 8, w->accentColor);
        drawFilledCircle(r, x2, y+hh/2, 8, w->accentColor);
        break;
    }
    // ─── قائمة منسدلة (Dropdown) ───
    case WidgetType::Dropdown: {
        drawRoundedRect(r, x, y, ww, hh, 4, {255,255,255,255});
        drawRectBorder(r, x, y, ww, hh, 1, {200,200,200,255});
        int tH = measureTextHeight(scale);
        int tY = y+(hh-tH)/2;
        std::string display = w->text; // النص التلميحي
        if (w->selectedIndex >= 0 && w->selectedIndex < (int)w->items.size())
            display = w->items[w->selectedIndex];
        drawText(r, display, x+8, tY, scale, {33,33,33,255}, "left", ww-30);
        // سهم ▼
        drawText(r, "v", x+ww-20, tY, scale, {120,120,120,255});
        break;
    }
    // ─── شريط تقدم (ProgressBar) ───
    case WidgetType::ProgressBar: {
        drawRoundedRect(r, x, y+hh/2-4, ww, 8, 4, {220,220,220,255});
        float pct = std::max(0.f, std::min(1.f, w->value / std::max(1.f, w->maxVal)));
        int fillW = (int)(ww * pct);
        if (fillW > 0) drawRoundedRect(r, x, y+hh/2-4, fillW, 8, 4, w->accentColor);
        break;
    }
    // ─── مؤشر تقدم دائري ───
    case WidgetType::CircleProgress: {
        int rad = std::min(ww, hh)/2 - 4;
        int cx0 = x+ww/2, cy0 = y+hh/2;
        drawFilledCircle(r, cx0, cy0, rad, {220,220,220,255});
        drawFilledCircle(r, cx0, cy0, rad-4, {255,255,255,255});
        // نسبة التقدم
        float pct = std::max(0.f, std::min(1.f, w->value / std::max(1.f, w->maxVal)));
        std::string pStr = std::to_string((int)(pct*100)) + "%";
        int tH = measureTextHeight(scale);
        drawText(r, pStr, x, cy0-tH/2, scale, w->textColor, "center", ww);
        // قوس التقدم (تقريبي — رسم نقاط على المحيط)
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        int totalPts = (int)(360 * pct);
        for (int deg = -90; deg < -90+totalPts; deg++) {
            float ra = deg * 3.14159f / 180.0f;
            for (int t = 0; t < 4; t++) {
                int px = cx0 + (int)((rad-t) * cosf(ra));
                int py = cy0 + (int)((rad-t) * sinf(ra));
                SDL_RenderDrawPoint(r, px, py);
            }
        }
        break;
    }
    // ─── مؤشر تحميل دوّار ───
    case WidgetType::LoadingSpinner: {
        int cx0 = x+ww/2, cy0 = y+hh/2, rad = std::min(ww,hh)/2 - 4;
        float angle = w->animationProgress;
        w->animationProgress += 5.0f; if (w->animationProgress > 360) w->animationProgress = 0;
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        for (int deg = 0; deg < 90; deg++) {
            float ra = (angle + deg) * 3.14159f / 180.0f;
            for (int t = 0; t < 3; t++) {
                int px = cx0 + (int)((rad-t)*cosf(ra));
                int py = cy0 + (int)((rad-t)*sinf(ra));
                SDL_RenderDrawPoint(r, px, py);
            }
        }
        break;
    }
    // ─── هيكل عظمي (Skeleton) — مستطيل رمادي يومض ───
    case WidgetType::Skeleton: {
        int alpha = 150 + (int)(50 * sinf(SDL_GetTicks() * 0.003f));
        drawRoundedRect(r, x, y, ww, hh, 4, {210,210,210,(int)std::min(255,std::max(0,alpha))});
        break;
    }
    // ─── أيقونة (حرف في دائرة أو بمفرده) ───
    case WidgetType::Icon: {
        char ic[2] = {w->iconChar, 0};
        int tH = measureTextHeight(scale);
        drawText(r, ic, x, y+(hh-tH)/2, scale, w->textColor, "center", ww);
        break;
    }
    // ─── أفاتار (دائرة ملونة بحرف) ───
    case WidgetType::Avatar: {
        int rad = std::min(ww, hh)/2 - 2;
        drawFilledCircle(r, x+ww/2, y+hh/2, rad, w->btnColor);
        int tH = measureTextHeight(scale);
        std::string letter = w->text.empty() ? "?" : w->text.substr(0,1);
        drawText(r, letter, x, y+(hh-tH)/2, scale, {255,255,255,255}, "center", ww);
        break;
    }
    // ─── شارة (Badge) — دائرة حمراء بعدد ───
    case WidgetType::Badge: {
        int rad = std::min(ww, hh)/2;
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{220,53,69,255};
        drawFilledCircle(r, x+ww/2, y+hh/2, rad, bg);
        std::string num = std::to_string((int)w->value);
        int tH = measureTextHeight(scale);
        drawText(r, num, x, y+(hh-tH)/2, scale, {255,255,255,255}, "center", ww);
        break;
    }
    // ─── رقاقة (Chip) — مستطيل دائري بنص ───
    case WidgetType::Chip: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{230,230,230,255};
        drawRoundedRect(r, x, y, ww, hh, hh/2, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, w->textColor, "center", ww);
        break;
    }
    // ─── وسم (Tag) — مثل Chip بلون ───
    case WidgetType::Tag: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : w->btnColor;
        drawRoundedRect(r, x, y, ww, hh, 4, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x, y+(hh-tH)/2, scale, {255,255,255,255}, "center", ww);
        break;
    }
    // ─── نقطة حالة (StatusDot) ───
    case WidgetType::StatusDot: {
        RGBA c = (w->bgColor.a > 0) ? w->bgColor : RGBA{76,175,80,255};
        drawFilledCircle(r, x+ww/2, y+hh/2, std::min(ww,hh)/2-1, c);
        break;
    }
    // ─── صندوق لون (ColorBox) ───
    case WidgetType::ColorBox: {
        RGBA c = (w->bgColor.a > 0) ? w->bgColor : RGBA{100,100,200,255};
        drawRoundedRect(r, x, y, ww, hh, (int)w->borderRadius, c);
        break;
    }
    // ─── صندوق متدرج (تدرج أفقي بسيط) ───
    case WidgetType::GradientBox: {
        RGBA c1 = w->btnColor, c2 = w->accentColor;
        for (int col = 0; col < ww; col++) {
            float t = (float)col / std::max(1, ww-1);
            int cr = c1.r + (int)(t*(c2.r - c1.r));
            int cg = c1.g + (int)(t*(c2.g - c1.g));
            int cb = c1.b + (int)(t*(c2.b - c1.b));
            SDL_SetRenderDrawColor(r, cr, cg, cb, 255);
            SDL_RenderDrawLine(r, x+col, y, x+col, y+hh);
        }
        break;
    }
    // ─── صورة (عنصر نائب) ───
    case WidgetType::ImageWidget: {
        drawRoundedRect(r, x, y, ww, hh, 4, {220,220,230,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text.empty() ? "[IMG]" : w->text, x, y+(hh-tH)/2, scale, {130,130,140,255}, "center", ww);
        break;
    }
    // ─── فاصل ─── [FIX #154] دعم السُمك واللون والاتجاه
    case WidgetType::Divider: case WidgetType::Separator: {
        int thickness = (w->borderWidth > 0) ? (int)w->borderWidth : 1;
        RGBA dc = (w->bgColor.a > 0) ? w->bgColor : RGBA{220,220,220,255};
        dc.a = (uint8_t)(dc.a * effectiveOpacity);
        // أفقي أو عمودي
        if (w->cw > w->ch * 2) {
            // أفقي
            for (int t = 0; t < thickness; t++) {
                SDL_SetRenderDrawColor(r, dc.r, dc.g, dc.b, dc.a);
                SDL_RenderDrawLine(r, x, y+hh/2-thickness/2+t, x+ww-1, y+hh/2-thickness/2+t);
            }
        } else {
            // عمودي
            for (int t = 0; t < thickness; t++) {
                SDL_SetRenderDrawColor(r, dc.r, dc.g, dc.b, dc.a);
                SDL_RenderDrawLine(r, x+ww/2-thickness/2+t, y, x+ww/2-thickness/2+t, y+hh-1);
            }
        }
        break;
    }
    // ─── نجوم تقييم (RatingStars) ───
    case WidgetType::RatingStars: {
        int maxS = std::max(1, (int)w->maxVal);
        int starW = std::min(24, ww/maxS);
        for (int i = 0; i < maxS; i++) {
            RGBA sc = (i < (int)w->value) ? RGBA{255, 193, 7, 255} : RGBA{200, 200, 200, 255};
            drawText(r, "*", x + i*starW, y+(hh-measureTextHeight(scale))/2, scale, sc);
        }
        break;
    }
    // ─── شريط تطبيق (AppBar) ───
    case WidgetType::AppBar: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{25,80,160,255};
        drawRoundedRect(r, x, y, ww, hh, 0, bg);
        int hScale = std::max(1, scale + 1);
        int tH = measureTextHeight(hScale);
        drawText(r, w->text, x+16, y+(hh-tH)/2, hScale, {255,255,255,255}, "left", ww-32);
        break;
    }
    // ─── شريط سفلي (BottomBar) ───
    case WidgetType::BottomBar: case WidgetType::StatusBar: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{250,250,250,255};
        drawRoundedRect(r, x, y, ww, hh, 0, bg);
        drawHLine(r, x, y, ww, {220,220,220,255});
        if (w->type == WidgetType::StatusBar) {
            int tH = measureTextHeight(scale);
            drawText(r, w->text, x+8, y+(hh-tH)/2, scale, {100,100,100,255}, "left", ww-16);
        }
        break;
    }
    // ─── شريط تبويبات (TabBar) ───
    case WidgetType::TabBar: {
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{250,250,250,255};
        drawRoundedRect(r, x, y, ww, hh, 0, bg);
        if (!w->items.empty()) {
            int tabW = ww / (int)w->items.size();
            for (int i = 0; i < (int)w->items.size(); i++) {
                int tH = measureTextHeight(scale);
                bool sel = (i == w->selectedIndex);
                RGBA tc = sel ? w->accentColor : RGBA{120,120,120,255};
                drawText(r, w->items[i], x+i*tabW, y+(hh-tH)/2, scale, tc, "center", tabW);
                if (sel) {
                    SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
                    SDL_Rect ind = {x+i*tabW+tabW/4, y+hh-3, tabW/2, 3};
                    SDL_RenderFillRect(r, &ind);
                }
            }
        }
        break;
    }
    // ─── عنصر ملاحة (NavItem) / عنصر قائمة (MenuItem) ───
    case WidgetType::NavItem: case WidgetType::MenuItem: {
        RGBA bg = w->active ? RGBA{230,240,255,255} : (w->hovered ? RGBA{245,245,245,255} : RGBA{0,0,0,0});
        if (bg.a > 0) drawRoundedRect(r, x, y, ww, hh, 4, bg);
        RGBA tc = w->active ? RGBA{25,80,160,255} : w->textColor;
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+(hh-tH)/2, scale, tc, "left", ww-24);
        break;
    }
    // ─── تنبيه (Alert) ───
    case WidgetType::Alert: {
        // نوع التنبيه يحدد اللون
        RGBA bg = {253,237,237,255}; RGBA bc = {220,53,69,255}; // error
        if (w->subtitle == "success") { bg = {237,253,237,255}; bc = {40,167,69,255}; }
        else if (w->subtitle == "warning") { bg = {255,248,225,255}; bc = {255,193,7,255}; }
        else if (w->subtitle == "info") { bg = {227,242,253,255}; bc = {33,150,243,255}; }
        drawRoundedRect(r, x, y, ww, hh, 8, bg);
        SDL_SetRenderDrawColor(r, bc.r, bc.g, bc.b, bc.a);
        SDL_Rect lb = {x, y, 4, hh}; SDL_RenderFillRect(r, &lb);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+(hh-tH)/2, scale, bc, "left", ww-20);
        break;
    }
    // ─── إشعار منبثق (Toast/Snackbar/Banner) ───
    case WidgetType::Toast: case WidgetType::Snackbar: case WidgetType::Banner: {
        RGBA bg = (w->type == WidgetType::Banner) ? RGBA{255,248,225,255} : RGBA{50,50,50,240};
        RGBA tc = (w->type == WidgetType::Banner) ? RGBA{100,80,0,255} : RGBA{255,255,255,255};
        drawRoundedRect(r, x, y, ww, hh, 8, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+(hh-tH)/2, scale, tc, "left", ww-24);
        break;
    }
    // ─── معلومات تفصيلية (ListTile / DetailRow) ───
    case WidgetType::ListTile: {
        if (w->hovered) drawRoundedRect(r, x, y, ww, hh, 0, {245,245,245,255});
        int tH = measureTextHeight(scale);
        int sScale = std::max(1, scale - 1);
        int sH = measureTextHeight(sScale);
        int totalH = tH + sH + 2;
        int tY = y + (hh - totalH) / 2;
        drawText(r, w->text, x+12, tY, scale, w->textColor, "left", ww-24);
        drawText(r, w->subtitle, x+12, tY+tH+2, sScale, {130,130,130,255}, "left", ww-24);
        break;
    }
    case WidgetType::DetailRow: {
        int tH = measureTextHeight(scale);
        int tY = y + (hh-tH)/2;
        drawText(r, w->text, x+8, tY, scale, {100,100,100,255}, "left", ww/2-8);
        drawText(r, w->subtitle, x+ww/2, tY, scale, w->textColor, "right", ww/2-8);
        break;
    }
    // ─── بطاقة معلومات / بطاقة إحصائية ───
    case WidgetType::InfoCard: case WidgetType::StatCard: {
        drawRoundedRect(r, x+2, y+2, ww, hh, 12, {0,0,0,25}); // ظل
        RGBA bg = (w->bgColor.a > 0) ? w->bgColor : RGBA{255,255,255,255};
        drawRoundedRect(r, x, y, ww, hh, 12, bg);
        int tH = measureTextHeight(scale);
        int bigScale = std::max(1, scale + 1);
        int bH = measureTextHeight(bigScale);
        if (w->type == WidgetType::StatCard) {
            // القيمة كبيرة في الأعلى، العنوان صغير تحت
            drawText(r, w->subtitle, x+12, y+12, bigScale, w->accentColor, "left", ww-24);
            drawText(r, w->text, x+12, y+12+bH+4, scale, {100,100,100,255}, "left", ww-24);
        } else {
            drawText(r, w->text, x+12, y+12, scale+1 > scale ? scale : scale, w->textColor, "left", ww-24);
            drawText(r, w->subtitle, x+12, y+12+tH+4, std::max(1,scale-1), {130,130,130,255}, "left", ww-24);
        }
        break;
    }
    // ─── لوحة قابلة للتوسيع (ExpansionPanel) ───
    case WidgetType::ExpansionPanel: {
        drawRoundedRect(r, x, y, ww, hh, 4, {255,255,255,255});
        drawRectBorder(r, x, y, ww, hh, 1, {220,220,220,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+8, scale, w->textColor, "left", ww-40);
        // سهم توسيع/طي
        drawText(r, w->expanded ? "^" : "v", x+ww-24, y+8, scale, {120,120,120,255});
        break;
    }
    // ─── مجموعة (GroupBox) ───
    case WidgetType::GroupBox: case WidgetType::Panel: {
        drawRectBorder(r, x, y+8, ww, hh-8, 1, {200,200,200,255});
        // عنوان المجموعة فوق الحد
        if (!w->text.empty()) {
            int tW = measureTextWidth(w->text, scale);
            int tH = measureTextHeight(scale);
            SDL_SetRenderDrawColor(r, 245, 245, 245, 255); // مسح الخط تحت العنوان
            SDL_Rect clear = {x+8, y, tW+8, tH+4};
            SDL_RenderFillRect(r, &clear);
            drawText(r, w->text, x+12, y, scale, w->textColor, "left");
        }
        break;
    }
    // ─── ملاحظة (Callout) ───
    case WidgetType::Callout: {
        RGBA bg = {227,242,253,255};
        if (w->subtitle == "warning") bg = {255,248,225,255};
        else if (w->subtitle == "error") bg = {253,237,237,255};
        else if (w->subtitle == "success") bg = {237,253,237,255};
        drawRoundedRect(r, x, y, ww, hh, 8, bg);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+12, y+(hh-tH)/2, scale, {50,50,50,255}, "left", ww-24);
        break;
    }
    // ─── رسم بياني عمودي (BarChart) ───
    case WidgetType::BarChart: {
        if (w->dataValues.empty()) break;
        float maxV = *std::max_element(w->dataValues.begin(), w->dataValues.end());
        if (maxV <= 0) maxV = 1;
        int n = (int)w->dataValues.size();
        int barW = std::max(4, (ww - 8) / n - 4);
        RGBA defColors[] = {{66,133,244,255},{234,67,53,255},{52,168,83,255},
                            {251,188,4,255},{171,71,188,255},{0,172,193,255}};
        for (int i = 0; i < n; i++) {
            float pct = w->dataValues[i] / maxV;
            int bH = (int)((hh-16) * pct);
            RGBA c = (i < (int)w->dataColors.size()) ? w->dataColors[i] : defColors[i%6];
            drawRoundedRect(r, x+4+i*(barW+4), y+hh-8-bH, barW, bH, 2, c);
        }
        break;
    }
    // ─── رسم بياني أفقي (HBarChart) ───
    case WidgetType::HBarChart: {
        if (w->dataValues.empty()) break;
        float maxV = *std::max_element(w->dataValues.begin(), w->dataValues.end());
        if (maxV <= 0) maxV = 1;
        int n = (int)w->dataValues.size();
        int barH = std::max(4, (hh - 8) / n - 4);
        RGBA defColors[] = {{66,133,244,255},{234,67,53,255},{52,168,83,255},{251,188,4,255}};
        for (int i = 0; i < n; i++) {
            float pct = w->dataValues[i] / maxV;
            int bW = (int)((ww-16) * pct);
            RGBA c = (i < (int)w->dataColors.size()) ? w->dataColors[i] : defColors[i%4];
            drawRoundedRect(r, x+8, y+4+i*(barH+4), bW, barH, 2, c);
        }
        break;
    }
    // ─── رسم بياني دائري / كعكة (PieChart/DonutChart) ───
    case WidgetType::PieChart: case WidgetType::DonutChart: {
        if (w->dataValues.empty()) break;
        float total = 0;
        for (float v : w->dataValues) total += v;
        if (total <= 0) break;
        int cx0 = x+ww/2, cy0 = y+hh/2;
        int rad = std::min(ww, hh)/2 - 4;
        RGBA defColors[] = {{66,133,244,255},{234,67,53,255},{52,168,83,255},
                            {251,188,4,255},{171,71,188,255},{0,172,193,255}};
        float startAngle = -90;
        for (int i = 0; i < (int)w->dataValues.size(); i++) {
            float sweep = 360 * (w->dataValues[i] / total);
            RGBA c = (i < (int)w->dataColors.size()) ? w->dataColors[i] : defColors[i%6];
            SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
            for (float deg = startAngle; deg < startAngle + sweep; deg += 0.5f) {
                float ra = deg * 3.14159f / 180.0f;
                int startR = (w->type == WidgetType::DonutChart) ? rad/2 : 0;
                for (int rr = startR; rr <= rad; rr++) {
                    SDL_RenderDrawPoint(r, cx0+(int)(rr*cosf(ra)), cy0+(int)(rr*sinf(ra)));
                }
            }
            startAngle += sweep;
        }
        break;
    }
    // ─── خط بياني (LineChart) ───
    case WidgetType::LineChart: {
        if (w->dataValues.size() < 2) break;
        float maxV = *std::max_element(w->dataValues.begin(), w->dataValues.end());
        float minV = *std::min_element(w->dataValues.begin(), w->dataValues.end());
        float range = maxV - minV; if (range <= 0) range = 1;
        int n = (int)w->dataValues.size();
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        for (int i = 1; i < n; i++) {
            float t0 = (float)(i-1)/(n-1), t1 = (float)i/(n-1);
            float v0 = (w->dataValues[i-1]-minV)/range, v1 = (w->dataValues[i]-minV)/range;
            int x0 = x+4+(int)(t0*(ww-8)), y0 = y+hh-4-(int)(v0*(hh-8));
            int x1 = x+4+(int)(t1*(ww-8)), y1 = y+hh-4-(int)(v1*(hh-8));
            SDL_RenderDrawLine(r, x0, y0, x1, y1);
            SDL_RenderDrawLine(r, x0, y0+1, x1, y1+1); // سمك 2
        }
        break;
    }
    // ─── خط مصغّر (SparkLine) ───
    case WidgetType::SparkLine: {
        if (w->dataValues.size() < 2) break;
        float maxV = *std::max_element(w->dataValues.begin(), w->dataValues.end());
        float minV = *std::min_element(w->dataValues.begin(), w->dataValues.end());
        float range = maxV - minV; if (range <= 0) range = 1;
        int n = (int)w->dataValues.size();
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        for (int i = 1; i < n; i++) {
            float t0 = (float)(i-1)/(n-1), t1 = (float)i/(n-1);
            float v0 = (w->dataValues[i-1]-minV)/range, v1 = (w->dataValues[i]-minV)/range;
            SDL_RenderDrawLine(r, x+(int)(t0*ww), y+hh-(int)(v0*hh), x+(int)(t1*ww), y+hh-(int)(v1*hh));
        }
        break;
    }
    // ─── عدّاد (Gauge) — نصف دائرة ───
    case WidgetType::Gauge: {
        int cx0 = x+ww/2, cy0 = y+hh-8;
        int rad = std::min(ww/2, hh) - 8;
        // خلفية القوس
        SDL_SetRenderDrawColor(r, 220, 220, 220, 255);
        for (float deg = 180; deg <= 360; deg += 0.5f) {
            float ra = deg * 3.14159f / 180.0f;
            for (int t = 0; t < 6; t++)
                SDL_RenderDrawPoint(r, cx0+(int)((rad-t)*cosf(ra)), cy0+(int)((rad-t)*sinf(ra)));
        }
        // قوس القيمة
        float pct = std::max(0.f, std::min(1.f, w->value / std::max(1.f, w->maxVal)));
        SDL_SetRenderDrawColor(r, w->accentColor.r, w->accentColor.g, w->accentColor.b, 255);
        for (float deg = 180; deg <= 180 + 180*pct; deg += 0.5f) {
            float ra = deg * 3.14159f / 180.0f;
            for (int t = 0; t < 6; t++)
                SDL_RenderDrawPoint(r, cx0+(int)((rad-t)*cosf(ra)), cy0+(int)((rad-t)*sinf(ra)));
        }
        std::string val = std::to_string((int)w->value);
        int tH = measureTextHeight(scale);
        drawText(r, val, x, cy0-tH-4, scale, w->textColor, "center", ww);
        break;
    }
    // ─── مؤشر نقطي (DotIndicator) ───
    case WidgetType::DotIndicator: {
        int total = std::max(1, (int)w->maxVal);
        int current = std::max(0, std::min((int)w->value, total-1));
        int dotSz = 8, gap = 6;
        int totalW = total * dotSz + (total-1) * gap;
        int startX = x + (ww - totalW) / 2;
        for (int i = 0; i < total; i++) {
            RGBA dc = (i == current) ? w->accentColor : RGBA{200,200,200,255};
            drawFilledCircle(r, startX + i*(dotSz+gap) + dotSz/2, y+hh/2, dotSz/2, dc);
        }
        break;
    }
    // ─── مؤشر خطوات (StepIndicator) ───
    case WidgetType::StepIndicator: {
        int total = std::max(1, (int)w->maxVal);
        int current = std::max(0, std::min((int)w->value, total));
        int stepW = ww / total;
        for (int i = 0; i < total; i++) {
            RGBA c = (i < current) ? w->accentColor : RGBA{200,200,200,255};
            drawFilledCircle(r, x+i*stepW+stepW/2, y+hh/2, 10, c);
            std::string num = std::to_string(i+1);
            int tH = measureTextHeight(2);
            drawText(r, num, x+i*stepW, y+(hh-tH)/2, 2, {255,255,255,255}, "center", stepW);
            if (i < total-1) {
                RGBA lc = (i < current-1) ? w->accentColor : RGBA{200,200,200,255};
                drawHLine(r, x+i*stepW+stepW/2+12, y+hh/2, stepW-24, lc);
            }
        }
        break;
    }
    // ─── ترقيم صفحات (Pagination) ───
    case WidgetType::Pagination: {
        int total = std::max(1, w->totalPages);
        int btnW = 30, gap = 4;
        int totalW = total * btnW + (total-1) * gap;
        int startX = x + (ww - totalW) / 2;
        for (int i = 0; i < total; i++) {
            bool cur = (i == w->currentPage);
            RGBA bg = cur ? w->accentColor : RGBA{230,230,230,255};
            RGBA tc = cur ? RGBA{255,255,255,255} : RGBA{60,60,60,255};
            drawRoundedRect(r, startX+i*(btnW+gap), y+(hh-btnW)/2, btnW, btnW, 4, bg);
            std::string num = std::to_string(i+1);
            int tH = measureTextHeight(2);
            drawText(r, num, startX+i*(btnW+gap), y+(hh-tH)/2, 2, tc, "center", btnW);
        }
        break;
    }
    // ─── تسلسل زمني (Timeline) ───
    case WidgetType::Timeline: {
        // خط عمودي في اليسار
        SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
        SDL_RenderDrawLine(r, x+12, y, x+12, y+hh);
        break;
    }
    case WidgetType::TimelineItem: {
        drawFilledCircle(r, x+12, y+12, 6, w->accentColor);
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+28, y+(hh-tH)/2, scale, w->textColor, "left", ww-36);
        SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
        SDL_RenderDrawLine(r, x+12, y, x+12, y+hh);
        break;
    }
    // ─── عنصر شجرة (TreeItem) ───
    case WidgetType::TreeItem: {
        int indent = w->level * 20;
        if (w->hovered) drawRoundedRect(r, x, y, ww, hh, 0, {245,245,245,255});
        int tH = measureTextHeight(scale);
        drawText(r, w->text, x+indent+8, y+(hh-tH)/2, scale, w->textColor, "left", ww-indent-16);
        break;
    }
    // ─── تلميح (Tooltip) ───
    case WidgetType::Tooltip: {
        drawRoundedRect(r, x, y, ww, hh, 6, {50,50,50,230});
        int tH = measureTextHeight(std::max(1, scale-1));
        drawText(r, w->text, x+8, y+(hh-tH)/2, std::max(1,scale-1), {255,255,255,255}, "center", ww-16);
        break;
    }
    // ─── مُنتقي ألوان (مبسّط — شبكة ألوان) ───
    case WidgetType::ColorPicker: {
        int cols = 8, rows = 4;
        int cw_ = ww/cols, ch_ = hh/rows;
        for (int ry = 0; ry < rows; ry++) {
            for (int cx_ = 0; cx_ < cols; cx_++) {
                int r_ = (cx_*255)/(cols-1), g_ = (ry*255)/(rows-1), b_ = 128;
                SDL_SetRenderDrawColor(r, r_, g_, b_, 255);
                SDL_Rect cr = {x+cx_*cw_, y+ry*ch_, cw_-1, ch_-1};
                SDL_RenderFillRect(r, &cr);
            }
        }
        break;
    }

    default: break;
    } // end switch

    // ─── رسم الأبناء (مع دعم العرض الافتراضي للقوائم) ───
    //
    // [FIX #85] ترتيب الأبناء حسب z-index قبل الرسم
    // [FIX #86] دعم العرض الافتراضي للقوائم الطويلة
    //    
    bool isScrollable = (w->type == WidgetType::ListView || w->type == WidgetType::ScrollView);
    
    // ترتيب الأبناء حسب z-index
    std::vector<int> sortedChildren = w->children;
    if (sortedChildren.size() > 1) {
        bool hasZOrder = false;
        for (int cid : sortedChildren) {
            Widget* c = getWidget(cid);
            if (c && c->zIndex != 0) { hasZOrder = true; break; }
        }
        if (hasZOrder) {
            std::stable_sort(sortedChildren.begin(), sortedChildren.end(), [](int a, int b) {
                Widget* wa = getWidget(a); Widget* wb = getWidget(b);
                int za = wa ? wa->zIndex : 0, zb = wb ? wb->zIndex : 0;
                return za < zb;
            });
        }
    }

    if (isScrollable && sortedChildren.size() > 10) {
        float viewTop = w->cy;
        float viewBottom = w->cy + w->ch;
        int firstVisible = -1, lastVisible = -1;
        
        for (int i = 0; i < (int)sortedChildren.size(); i++) {
            int cid = sortedChildren[i];
            Widget* child = getWidget(cid);
            if (!child || !child->visible) continue;
            
            float childTop = child->cy;
            float childBottom = child->cy + child->ch;
            
            if (childBottom >= viewTop && childTop <= viewBottom) {
                if (firstVisible < 0) firstVisible = i;
                lastVisible = i;
                renderWidget(r, cid);
            }
        }
        
        w->virtualFirstVisible = firstVisible;
        w->virtualLastVisible = lastVisible;
    } else {
        for (int cid : sortedChildren) renderWidget(r, cid);
    }

    // [FIX #87] استعادة القصاصة
    if (needsClip) {
        if (hadClip) SDL_RenderSetClipRect(r, &prevClip);
        else SDL_RenderSetClipRect(r, nullptr);
    }

    // [FIX #88] رسم التلميح (Tooltip) عند التحويم
    if (w->hovered && !w->tooltipText.empty()) {
        int tipScale = isFontLoaded() ? 14 : 2;
        int tipW = measureTextWidth(w->tooltipText, tipScale) + 12;
        int tipH = measureTextHeight(tipScale) + 8;
        int tipX = x + ww/2 - tipW/2;
        int tipY = y - tipH - 4;
        if (tipY < 0) tipY = y + hh + 4;  // أسفل العنصر إذا لم يكن هناك مساحة فوقه
        drawRoundedRect(r, tipX-1, tipY-1, tipW+2, tipH+2, 4, {0,0,0,200}); // خلفية
        drawText(r, w->tooltipText, tipX+6, tipY+4, tipScale, {255,255,255,255}, "center", tipW);
    }

    // [FIX #89] [FIX #155] رسم لون التحويم والضغط — استخدام effectiveOpacity
    if (w->hovered && w->hoverColor.a > 0) {
        RGBA hc = w->hoverColor;
        hc.a = (uint8_t)(hc.a * 0.3f * effectiveOpacity);
        drawRoundedRect(r, x, y, ww, hh, (int)w->borderRadius, hc);
    } else if (w->pressed && w->pressColor.a > 0) {
        RGBA pc = w->pressColor;
        pc.a = (uint8_t)(pc.a * 0.4f * effectiveOpacity);
        drawRoundedRect(r, x, y, ww, hh, (int)w->borderRadius, pc);
    }

    // [FIX #156] رسم الشارة (Badge) فوق العنصر
    if (w->badgeCount > 0 || !w->badge.empty()) {
        std::string badgeText = w->badge.empty() ? std::to_string(w->badgeCount) : w->badge;
        int bScale = std::max(1, scale - 3);
        int btW = measureTextWidth(badgeText, bScale);
        int bW = std::max(16, btW + 8);
        int bH = 16;
        int bx = x + ww - bW + 4;
        int by = y - 4;
        RGBA badgeBg = {220, 53, 69, (uint8_t)(255 * effectiveOpacity)};
        drawRoundedRect(r, bx, by, bW, bH, bH/2, badgeBg);
        drawText(r, badgeText, bx, by + (bH - measureTextHeight(bScale))/2, bScale, {255,255,255,255}, "center", bW);
    }

    // [FIX #157] رسم مؤشر التحديد (selected state)
    if (w->selected) {
        RGBA selC = {66, 133, 244, (uint8_t)(40 * effectiveOpacity)};
        drawRoundedRect(r, x, y, ww, hh, (int)w->borderRadius, selC);
    }

    // [FIX #158] شريط التمرير العمودي المرئي
    if (w->scrollbarVisible && (w->type == WidgetType::ScrollView || w->type == WidgetType::ListView)) {
        float contentH = w->contentHeight;
        if (contentH > w->ch && contentH > 0) {
            float viewRatio = w->ch / contentH;
            int scrollbarH = std::max(20, (int)(hh * viewRatio));
            float scrollRatio = w->scrollY / (contentH - w->ch);
            scrollRatio = std::max(0.0f, std::min(1.0f, scrollRatio));
            int scrollbarY = y + (int)((hh - scrollbarH) * scrollRatio);
            int scrollbarX = x + ww - 6;
            RGBA sbBg = {0, 0, 0, (uint8_t)(30 * effectiveOpacity)};
            RGBA sbFg = {0, 0, 0, (uint8_t)(80 * effectiveOpacity)};
            drawRoundedRect(r, scrollbarX, y, 4, hh, 2, sbBg);
            drawRoundedRect(r, scrollbarX, scrollbarY, 4, scrollbarH, 2, sbFg);
        }
    }

    // [FIX #159] مؤشر التقدم (progress) كشريط تحت العنصر
    if (w->progress > 0 && w->progress < 1.0f && w->type != WidgetType::ProgressBar && w->type != WidgetType::CircleProgress) {
        int progW = (int)(ww * w->progress);
        RGBA progC = {66, 133, 244, (uint8_t)(180 * effectiveOpacity)};
        SDL_SetRenderDrawColor(r, progC.r, progC.g, progC.b, progC.a);
        SDL_Rect progR = {x, y + hh - 3, progW, 3};
        SDL_RenderFillRect(r, &progR);
    }
}

}} // namespace sad::ui
