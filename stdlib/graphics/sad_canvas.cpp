// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_canvas.cpp — تنفيذ واجهة الرسم المخصص
// ─────────────────────────────────────────────────────────────────────────────
// يُنفّذ رسومات ثنائية الأبعاد كاملة فوق SDL2_Renderer
// ═══════════════════════════════════════════════════════════════════════════════
#include "sad_canvas.h"
#include <SDL.h>
#include <cmath>
#include <unordered_map>
#include <stack>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace sad { namespace canvas {

// ═══════════════════════════════════════════════════════════════════
// حالة القماش
// ═══════════════════════════════════════════════════════════════════

struct Transform {
    float offsetX = 0, offsetY = 0;
    float scaleX = 1, scaleY = 1;
    float rotation = 0; // بالراديان
    float pivotX = 0, pivotY = 0;
};

struct CanvasState {
    int widgetId = 0;
    bool needsRedraw = true;
    DrawCallback drawCallback;
    
    // مكدس التحويلات
    Transform currentTransform;
    std::stack<Transform> transformStack;
    
    // القصاصة
    bool hasClip = false;
    Rect clipRect_;
    
    // أوامر الرسم المؤجلة (Command Buffer)
    struct DrawCommand {
        enum Type {
            Line, Rect_, RoundRect_, Circle_, Ellipse_,
            Arc_, Triangle_, Polygon_, Point_,
            GradientRect_, GradientCircle_, Text_, Path_, Clear_
        };
        Type type;
        std::vector<float> params;
        Paint paint;
        Gradient gradient;
        std::string textData;
        std::vector<Point> points;
        int pathId = 0;
    };
    std::vector<DrawCommand> commands;
};

// حالة المسار
struct PathState {
    std::vector<Point> points;
    bool closed = false;
    Point currentPoint = {0, 0};
};

// ───── المتغيرات العالمية ─────
static std::unordered_map<int, CanvasState> g_canvases;
static int g_nextCanvasId = 1;
static std::unordered_map<int, PathState> g_paths;
static int g_nextPathId = 1;

// ═══════════════════════════════════════════════════════════════════
// دوال رسم مساعدة (SDL2)
// ═══════════════════════════════════════════════════════════════════

// رسم خط بعرض (SDL2 لا يدعم خطوط سميكة — نرسم عدة خطوط متوازية)
static void sdl_draw_thick_line(SDL_Renderer* r, float x1, float y1, float x2, float y2,
                                float width, const Color& c) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    
    if (width <= 1.0f) {
        SDL_RenderDrawLineF(r, x1, y1, x2, y2);
        return;
    }
    
    // حساب عمودي على الخط
    float dx = x2 - x1;
    float dy = y2 - y1;
    float len = std::sqrt(dx*dx + dy*dy);
    if (len < 0.001f) return;
    
    float nx = -dy / len * width * 0.5f;
    float ny = dx / len * width * 0.5f;
    
    // رسم مستطيل (4 رؤوس)
    SDL_FPoint verts[4] = {
        {x1 + nx, y1 + ny},
        {x1 - nx, y1 - ny},
        {x2 - nx, y2 - ny},
        {x2 + nx, y2 + ny}
    };
    
    // تقسيم إلى مثلثين
    SDL_Vertex sdlVerts[6];
    SDL_Color sc = {(Uint8)c.r, (Uint8)c.g, (Uint8)c.b, (Uint8)c.a};
    
    int indices[] = {0, 1, 2, 0, 2, 3};
    for (int i = 0; i < 6; i++) {
        sdlVerts[i].position = verts[indices[i]];
        sdlVerts[i].color = sc;
        sdlVerts[i].tex_coord = {0, 0};
    }
    
    SDL_RenderGeometry(r, nullptr, sdlVerts, 6, nullptr, 0);
}

// رسم دائرة ممتلئة (Bresenham + خطوط أفقية)
static void sdl_fill_circle(SDL_Renderer* r, float cx, float cy, float radius, const Color& c) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    
    int intR = (int)radius;
    int x0 = (int)cx;
    int y0 = (int)cy;
    
    for (int w = 0; w < intR * 2; w++) {
        int dx = intR - w;
        int h = (int)std::sqrt(intR * intR - dx * dx);
        SDL_RenderDrawLine(r, x0 + dx, y0 - h, x0 + dx, y0 + h);
    }
}

// رسم حدود دائرة (Midpoint Circle Algorithm)
static void sdl_stroke_circle(SDL_Renderer* r, float cx, float cy, float radius, const Color& c) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    
    int x = (int)radius;
    int y = 0;
    int err = 0;
    int x0 = (int)cx;
    int y0 = (int)cy;
    
    while (x >= y) {
        SDL_RenderDrawPoint(r, x0 + x, y0 + y);
        SDL_RenderDrawPoint(r, x0 + y, y0 + x);
        SDL_RenderDrawPoint(r, x0 - y, y0 + x);
        SDL_RenderDrawPoint(r, x0 - x, y0 + y);
        SDL_RenderDrawPoint(r, x0 - x, y0 - y);
        SDL_RenderDrawPoint(r, x0 - y, y0 - x);
        SDL_RenderDrawPoint(r, x0 + y, y0 - x);
        SDL_RenderDrawPoint(r, x0 + x, y0 - y);
        
        y += 1;
        err += 2 * y + 1;
        if (2 * err + 1 > 2 * x) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

// رسم مستطيل مدور ممتلئ
static void sdl_fill_round_rect(SDL_Renderer* r, float x, float y, float w, float h,
                                 float radius, const Color& c) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
    
    int rad = std::min((int)radius, std::min((int)(w/2), (int)(h/2)));
    
    // المستطيل الأوسط
    SDL_Rect center = {(int)(x + rad), (int)y, (int)(w - 2*rad), (int)h};
    SDL_RenderFillRect(r, &center);
    
    // الأشرطة الجانبية
    SDL_Rect left = {(int)x, (int)(y + rad), rad, (int)(h - 2*rad)};
    SDL_Rect right = {(int)(x + w - rad), (int)(y + rad), rad, (int)(h - 2*rad)};
    SDL_RenderFillRect(r, &left);
    SDL_RenderFillRect(r, &right);
    
    // الزوايا الأربع
    auto fillQuarter = [&](int cx_, int cy_, int startAngle) {
        for (int dy_ = 0; dy_ <= rad; dy_++) {
            int dx_ = (int)std::sqrt(rad * rad - dy_ * dy_);
            int y1_ = 0, y2_ = 0, x1_ = 0, x2_ = 0;
            switch (startAngle) {
                case 0:   x1_ = cx_; x2_ = cx_ + dx_; y1_ = cy_ - dy_; y2_ = y1_; break;
                case 90:  x1_ = cx_ - dx_; x2_ = cx_; y1_ = cy_ - dy_; y2_ = y1_; break;
                case 180: x1_ = cx_ - dx_; x2_ = cx_; y1_ = cy_ + dy_; y2_ = y1_; break;
                case 270: x1_ = cx_; x2_ = cx_ + dx_; y1_ = cy_ + dy_; y2_ = y1_; break;
            }
            SDL_RenderDrawLine(r, x1_, y1_, x2_, y2_);
        }
    };
    
    fillQuarter((int)(x + rad), (int)(y + rad), 90);
    fillQuarter((int)(x + w - rad), (int)(y + rad), 0);
    fillQuarter((int)(x + rad), (int)(y + h - rad), 180);
    fillQuarter((int)(x + w - rad), (int)(y + h - rad), 270);
}

// تطبيق التحويل
static void applyTransform(const Transform& t, float& x, float& y) {
    // تكبير
    x *= t.scaleX;
    y *= t.scaleY;
    
    // تدوير
    if (std::abs(t.rotation) > 0.001f) {
        float cosA = std::cos(t.rotation);
        float sinA = std::sin(t.rotation);
        float rx = x * cosA - y * sinA;
        float ry = x * sinA + y * cosA;
        x = rx;
        y = ry;
    }
    
    // إزاحة
    x += t.offsetX;
    y += t.offsetY;
}

// ═══════════════════════════════════════════════════════════════════
// إنشاء وإدارة القماش
// ═══════════════════════════════════════════════════════════════════

int createCanvas(int widgetId) {
    int id = g_nextCanvasId++;
    CanvasState& cs = g_canvases[id];
    cs.widgetId = widgetId;
    cs.needsRedraw = true;
    return id;
}

void setDrawCallback(int canvasId, DrawCallback callback) {
    auto it = g_canvases.find(canvasId);
    if (it != g_canvases.end()) {
        it->second.drawCallback = callback;
    }
}

void invalidate(int canvasId) {
    auto it = g_canvases.find(canvasId);
    if (it != g_canvases.end()) {
        it->second.needsRedraw = true;
    }
}

// ═══════════════════════════════════════════════════════════════════
// أوامر الرسم — تُخزَّن في command buffer
// ═══════════════════════════════════════════════════════════════════

void drawLine(int canvasId, float x1, float y1, float x2, float y2, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Line;
    cmd.params = {x1, y1, x2, y2};
    cmd.paint = paint;
    it->second.commands.push_back(cmd);
}

void drawLineSimple(int canvasId, float x1, float y1, float x2, float y2,
                    int r, int g, int b, float width) {
    Paint p;
    p.color = {r, g, b, 255};
    p.style = PaintStyle::Stroke;
    p.strokeWidth = width;
    drawLine(canvasId, x1, y1, x2, y2, p);
}

void drawRect(int canvasId, float x, float y, float w, float h, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Rect_;
    cmd.params = {x, y, w, h};
    cmd.paint = paint;
    it->second.commands.push_back(cmd);
}

void drawRoundRect(int canvasId, float x, float y, float w, float h,
                   float radius, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::RoundRect_;
    cmd.params = {x, y, w, h, radius};
    cmd.paint = paint;
    it->second.commands.push_back(cmd);
}

void drawCircle(int canvasId, float cx, float cy, float radius, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Circle_;
    cmd.params = {cx, cy, radius};
    cmd.paint = paint;
    it->second.commands.push_back(cmd);
}

void drawEllipse(int canvasId, float cx, float cy, float rx, float ry, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Ellipse_;
    cmd.params = {cx, cy, rx, ry};
    cmd.paint = paint;
    it->second.commands.push_back(cmd);
}

void drawArc(int canvasId, float cx, float cy, float radius,
             float startAngle, float sweepAngle, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Arc_;
    cmd.params = {cx, cy, radius, startAngle, sweepAngle};
    cmd.paint = paint;
    it->second.commands.push_back(cmd);
}

void drawTriangle(int canvasId, float x1, float y1, float x2, float y2,
                  float x3, float y3, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Triangle_;
    cmd.params = {x1, y1, x2, y2, x3, y3};
    cmd.paint = paint;
    it->second.commands.push_back(cmd);
}

void drawPolygon(int canvasId, const std::vector<Point>& points, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Polygon_;
    cmd.points = points;
    cmd.paint = paint;
    it->second.commands.push_back(cmd);
}

void drawPoint(int canvasId, float x, float y, float size, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Point_;
    cmd.params = {x, y, size};
    cmd.paint = paint;
    it->second.commands.push_back(cmd);
}

void drawGradientRect(int canvasId, float x, float y, float w, float h,
                      const Gradient& gradient) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::GradientRect_;
    cmd.params = {x, y, w, h};
    cmd.gradient = gradient;
    it->second.commands.push_back(cmd);
}

void drawGradientCircle(int canvasId, float cx, float cy, float radius,
                        const Gradient& gradient) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::GradientCircle_;
    cmd.params = {cx, cy, radius};
    cmd.gradient = gradient;
    it->second.commands.push_back(cmd);
}

// ═══════════════════════════════════════════════════════════════════
// المسارات
// ═══════════════════════════════════════════════════════════════════

int createPath() {
    int id = g_nextPathId++;
    g_paths[id] = PathState();
    return id;
}

void pathMoveTo(int pathId, float x, float y) {
    auto it = g_paths.find(pathId);
    if (it == g_paths.end()) return;
    it->second.currentPoint = {x, y};
    it->second.points.push_back({x, y});
}

void pathLineTo(int pathId, float x, float y) {
    auto it = g_paths.find(pathId);
    if (it == g_paths.end()) return;
    it->second.points.push_back({x, y});
    it->second.currentPoint = {x, y};
}

void pathQuadTo(int pathId, float cx, float cy, float x, float y) {
    auto it = g_paths.find(pathId);
    if (it == g_paths.end()) return;
    
    // تقريب منحنى بيزييه التربيعي بخطوط
    Point start = it->second.currentPoint;
    int segments = 16;
    for (int i = 1; i <= segments; i++) {
        float t = (float)i / segments;
        float t1 = 1.0f - t;
        float px = t1*t1*start.x + 2*t1*t*cx + t*t*x;
        float py = t1*t1*start.y + 2*t1*t*cy + t*t*y;
        it->second.points.push_back({px, py});
    }
    it->second.currentPoint = {x, y};
}

void pathCubicTo(int pathId, float cx1, float cy1, float cx2, float cy2, float x, float y) {
    auto it = g_paths.find(pathId);
    if (it == g_paths.end()) return;
    
    Point start = it->second.currentPoint;
    int segments = 20;
    for (int i = 1; i <= segments; i++) {
        float t = (float)i / segments;
        float t1 = 1.0f - t;
        float px = t1*t1*t1*start.x + 3*t1*t1*t*cx1 + 3*t1*t*t*cx2 + t*t*t*x;
        float py = t1*t1*t1*start.y + 3*t1*t1*t*cy1 + 3*t1*t*t*cy2 + t*t*t*y;
        it->second.points.push_back({px, py});
    }
    it->second.currentPoint = {x, y};
}

void pathArcTo(int pathId, float cx, float cy, float radius,
               float startAngle, float sweepAngle) {
    auto it = g_paths.find(pathId);
    if (it == g_paths.end()) return;
    
    float startRad = startAngle * (float)M_PI / 180.0f;
    float sweepRad = sweepAngle * (float)M_PI / 180.0f;
    int segments = std::max(8, (int)(std::abs(sweepAngle) / 5));
    
    for (int i = 0; i <= segments; i++) {
        float angle = startRad + sweepRad * i / segments;
        float px = cx + radius * std::cos(angle);
        float py = cy + radius * std::sin(angle);
        it->second.points.push_back({px, py});
    }
    
    float endAngle = startRad + sweepRad;
    it->second.currentPoint = {cx + radius * std::cos(endAngle), cy + radius * std::sin(endAngle)};
}

void pathClose(int pathId) {
    auto it = g_paths.find(pathId);
    if (it == g_paths.end() || it->second.points.empty()) return;
    it->second.points.push_back(it->second.points[0]);
    it->second.closed = true;
}

void drawPath(int canvasId, int pathId, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    auto pathIt = g_paths.find(pathId);
    if (pathIt == g_paths.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Path_;
    cmd.points = pathIt->second.points;
    cmd.paint = paint;
    cmd.pathId = pathId;
    it->second.commands.push_back(cmd);
}

void deletePath(int pathId) {
    g_paths.erase(pathId);
}

// ═══════════════════════════════════════════════════════════════════
// التحويلات
// ═══════════════════════════════════════════════════════════════════

void save(int canvasId) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    it->second.transformStack.push(it->second.currentTransform);
}

void restore(int canvasId) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end() || it->second.transformStack.empty()) return;
    it->second.currentTransform = it->second.transformStack.top();
    it->second.transformStack.pop();
}

void translate(int canvasId, float dx, float dy) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    it->second.currentTransform.offsetX += dx;
    it->second.currentTransform.offsetY += dy;
}

void rotate(int canvasId, float degrees) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    it->second.currentTransform.rotation += degrees * (float)M_PI / 180.0f;
}

void rotateAt(int canvasId, float degrees, float px, float py) {
    translate(canvasId, px, py);
    rotate(canvasId, degrees);
    translate(canvasId, -px, -py);
}

void scale(int canvasId, float sx, float sy) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    it->second.currentTransform.scaleX *= sx;
    it->second.currentTransform.scaleY *= sy;
}

void resetTransform(int canvasId) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    it->second.currentTransform = Transform();
}

// ═══════════════════════════════════════════════════════════════════
// القصاصة
// ═══════════════════════════════════════════════════════════════════

void clipRect(int canvasId, float x, float y, float w, float h) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    it->second.hasClip = true;
    it->second.clipRect_ = {x, y, w, h};
}

void clipCircle(int canvasId, float cx, float cy, float radius) {
    // تقريب الدائرة بمستطيل محيط (تبسيط)
    clipRect(canvasId, cx - radius, cy - radius, radius * 2, radius * 2);
}

void resetClip(int canvasId) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    it->second.hasClip = false;
}

// ═══════════════════════════════════════════════════════════════════
// رسم النص
// ═══════════════════════════════════════════════════════════════════

void drawText(int canvasId, const std::string& text, float x, float y,
              float fontSize, const Paint& paint) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Text_;
    cmd.params = {x, y, fontSize};
    cmd.paint = paint;
    cmd.textData = text;
    it->second.commands.push_back(cmd);
}

float measureText(const std::string& text, float fontSize) {
    // تقدير بسيط — كل حرف = 60% من حجم الخط
    return text.length() * fontSize * 0.6f;
}

// ═══════════════════════════════════════════════════════════════════
// مسح القماش
// ═══════════════════════════════════════════════════════════════════

void clear(int canvasId, int r, int g, int b, int a) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    it->second.commands.clear();
    
    CanvasState::DrawCommand cmd;
    cmd.type = CanvasState::DrawCommand::Clear_;
    cmd.paint.color = {r, g, b, a};
    it->second.commands.push_back(cmd);
}

// ═══════════════════════════════════════════════════════════════════
// تنفيذ الرسم — يُحوِّل command buffer إلى SDL calls
// ═══════════════════════════════════════════════════════════════════

void renderCanvas(int canvasId, SDL_Renderer* renderer, float ox, float oy, float ow, float oh) {
    auto it = g_canvases.find(canvasId);
    if (it == g_canvases.end()) return;
    
    CanvasState& cs = it->second;
    
    // استدعاء دالة الرسم إذا كانت محددة
    if (cs.drawCallback && cs.needsRedraw) {
        cs.commands.clear();
        cs.drawCallback(canvasId);
        cs.needsRedraw = false;
    }
    
    // تعيين القصاصة
    if (cs.hasClip) {
        SDL_Rect clip = {
            (int)(ox + cs.clipRect_.x), (int)(oy + cs.clipRect_.y),
            (int)cs.clipRect_.w, (int)cs.clipRect_.h
        };
        SDL_RenderSetClipRect(renderer, &clip);
    }
    
    // تنفيذ جميع أوامر الرسم
    for (const auto& cmd : cs.commands) {
        const auto& p = cmd.params;
        const Color& c = cmd.paint.color;
        
        switch (cmd.type) {
            case CanvasState::DrawCommand::Clear_: {
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                SDL_Rect rect = {(int)ox, (int)oy, (int)ow, (int)oh};
                SDL_RenderFillRect(renderer, &rect);
                break;
            }
            
            case CanvasState::DrawCommand::Line: {
                sdl_draw_thick_line(renderer, ox + p[0], oy + p[1], ox + p[2], oy + p[3],
                                    cmd.paint.strokeWidth, c);
                break;
            }
            
            case CanvasState::DrawCommand::Rect_: {
                SDL_Rect rect = {(int)(ox + p[0]), (int)(oy + p[1]), (int)p[2], (int)p[3]};
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                if (cmd.paint.style == PaintStyle::Fill || cmd.paint.style == PaintStyle::FillAndStroke) {
                    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                    SDL_RenderFillRect(renderer, &rect);
                }
                if (cmd.paint.style == PaintStyle::Stroke || cmd.paint.style == PaintStyle::FillAndStroke) {
                    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                    SDL_RenderDrawRect(renderer, &rect);
                }
                break;
            }
            
            case CanvasState::DrawCommand::RoundRect_: {
                sdl_fill_round_rect(renderer, ox + p[0], oy + p[1], p[2], p[3], p[4], c);
                break;
            }
            
            case CanvasState::DrawCommand::Circle_: {
                if (cmd.paint.style == PaintStyle::Fill || cmd.paint.style == PaintStyle::FillAndStroke)
                    sdl_fill_circle(renderer, ox + p[0], oy + p[1], p[2], c);
                if (cmd.paint.style == PaintStyle::Stroke || cmd.paint.style == PaintStyle::FillAndStroke)
                    sdl_stroke_circle(renderer, ox + p[0], oy + p[1], p[2], c);
                break;
            }
            
            case CanvasState::DrawCommand::Ellipse_: {
                // رسم بيضاوي بتقريب بنقاط
                float cx_ = ox + p[0], cy_ = oy + p[1];
                float rx = p[2], ry = p[3];
                int segments = 60;
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                
                if (cmd.paint.style != PaintStyle::Stroke) {
                    for (int i = 0; i < segments; i++) {
                        float a1 = 2.0f * (float)M_PI * i / segments;
                        float a2 = 2.0f * (float)M_PI * (i + 1) / segments;
                        
                        SDL_Vertex verts[3];
                        SDL_Color sc = {(Uint8)c.r, (Uint8)c.g, (Uint8)c.b, (Uint8)c.a};
                        verts[0].position = {cx_, cy_};
                        verts[0].color = sc;
                        verts[0].tex_coord = {0, 0};
                        verts[1].position = {cx_ + rx * std::cos(a1), cy_ + ry * std::sin(a1)};
                        verts[1].color = sc;
                        verts[1].tex_coord = {0, 0};
                        verts[2].position = {cx_ + rx * std::cos(a2), cy_ + ry * std::sin(a2)};
                        verts[2].color = sc;
                        verts[2].tex_coord = {0, 0};
                        SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
                    }
                } else {
                    for (int i = 0; i < segments; i++) {
                        float a1 = 2.0f * (float)M_PI * i / segments;
                        float a2 = 2.0f * (float)M_PI * (i + 1) / segments;
                        SDL_RenderDrawLineF(renderer,
                            cx_ + rx * std::cos(a1), cy_ + ry * std::sin(a1),
                            cx_ + rx * std::cos(a2), cy_ + ry * std::sin(a2));
                    }
                }
                break;
            }
            
            case CanvasState::DrawCommand::Triangle_: {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_Color sc = {(Uint8)c.r, (Uint8)c.g, (Uint8)c.b, (Uint8)c.a};
                SDL_Vertex verts[3] = {
                    {{ox + p[0], oy + p[1]}, sc, {0, 0}},
                    {{ox + p[2], oy + p[3]}, sc, {0, 0}},
                    {{ox + p[4], oy + p[5]}, sc, {0, 0}}
                };
                SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
                break;
            }
            
            case CanvasState::DrawCommand::Polygon_: {
                if (cmd.points.size() < 3) break;
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_Color sc = {(Uint8)c.r, (Uint8)c.g, (Uint8)c.b, (Uint8)c.a};
                // تقسيم إلى مثلثات (fan)
                for (size_t i = 1; i + 1 < cmd.points.size(); i++) {
                    SDL_Vertex verts[3] = {
                        {{ox + cmd.points[0].x, oy + cmd.points[0].y}, sc, {0, 0}},
                        {{ox + cmd.points[i].x, oy + cmd.points[i].y}, sc, {0, 0}},
                        {{ox + cmd.points[i+1].x, oy + cmd.points[i+1].y}, sc, {0, 0}}
                    };
                    SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
                }
                break;
            }
            
            case CanvasState::DrawCommand::Path_: {
                // رسم مسار كسلسلة من الخطوط أو كمضلع ممتلئ
                if (cmd.paint.style == PaintStyle::Fill || cmd.paint.style == PaintStyle::FillAndStroke) {
                    // تعبئة: مثلثات fan
                    if (cmd.points.size() >= 3) {
                        SDL_Color sc = {(Uint8)c.r, (Uint8)c.g, (Uint8)c.b, (Uint8)c.a};
                        for (size_t i = 1; i + 1 < cmd.points.size(); i++) {
                            SDL_Vertex verts[3] = {
                                {{ox + cmd.points[0].x, oy + cmd.points[0].y}, sc, {0, 0}},
                                {{ox + cmd.points[i].x, oy + cmd.points[i].y}, sc, {0, 0}},
                                {{ox + cmd.points[i+1].x, oy + cmd.points[i+1].y}, sc, {0, 0}}
                            };
                            SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
                        }
                    }
                }
                if (cmd.paint.style == PaintStyle::Stroke || cmd.paint.style == PaintStyle::FillAndStroke) {
                    for (size_t i = 0; i + 1 < cmd.points.size(); i++) {
                        sdl_draw_thick_line(renderer,
                            ox + cmd.points[i].x, oy + cmd.points[i].y,
                            ox + cmd.points[i+1].x, oy + cmd.points[i+1].y,
                            cmd.paint.strokeWidth, c);
                    }
                }
                break;
            }
            
            case CanvasState::DrawCommand::GradientRect_: {
                // تدرج خطي رأسي (تبسيط — كل صف بلون)
                float rx = p[0], ry = p[1], rw = p[2], rh = p[3];
                const auto& grad = cmd.gradient;
                if (grad.colors.size() < 2) break;
                
                int rows = (int)rh;
                for (int row = 0; row < rows; row++) {
                    float t = (float)row / (rows - 1);
                    // استكمال خطي بين الألوان
                    int idx = (int)(t * (grad.colors.size() - 1));
                    if (idx >= (int)grad.colors.size() - 1) idx = (int)grad.colors.size() - 2;
                    float localT = t * (grad.colors.size() - 1) - idx;
                    const Color& c1 = grad.colors[idx];
                    const Color& c2 = grad.colors[idx + 1];
                    int gr = (int)(c1.r + (c2.r - c1.r) * localT);
                    int gg = (int)(c1.g + (c2.g - c1.g) * localT);
                    int gb = (int)(c1.b + (c2.b - c1.b) * localT);
                    int ga = (int)(c1.a + (c2.a - c1.a) * localT);
                    
                    SDL_SetRenderDrawColor(renderer, gr, gg, gb, ga);
                    SDL_RenderDrawLine(renderer,
                        (int)(ox + rx), (int)(oy + ry + row),
                        (int)(ox + rx + rw), (int)(oy + ry + row));
                }
                break;
            }
            
            case CanvasState::DrawCommand::Point_: {
                float size = p[2];
                if (size <= 1) {
                    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
                    SDL_RenderDrawPointF(renderer, ox + p[0], oy + p[1]);
                } else {
                    sdl_fill_circle(renderer, ox + p[0], oy + p[1], size / 2, c);
                }
                break;
            }
            
            default: break;
        }
    }
    
    // إزالة القصاصة
    if (cs.hasClip) {
        SDL_RenderSetClipRect(renderer, nullptr);
    }
}

void clearAllCanvases() {
    g_canvases.clear();
    g_paths.clear();
}

}} // namespace sad::canvas
