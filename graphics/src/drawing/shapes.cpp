// ==============================================================================
// shapes.cpp - تنفيذ نظام رسم الأشكال / Shape Drawing Implementation
// ==============================================================================

#include "../../include/drawing/shapes.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <glad/glad.h>

namespace sad {
namespace graphics {

// ==============================================================================
// الثوابت / Constants
// ==============================================================================

constexpr Float32 PI = 3.14159265358979323846f;
constexpr Float32 TWO_PI = 2.0f * PI;

// ==============================================================================
// Path - تنفيذ المسار / Path Implementation
// ==============================================================================

Path& Path::Clear() {
    m_commands.clear();
    m_currentPoint = Vec2(0, 0);
    m_startPoint = Vec2(0, 0);
    return *this;
}

Path& Path::MoveTo(Float32 x, Float32 y) {
    return MoveTo(Vec2(x, y));
}

Path& Path::MoveTo(Vec2 point) {
    PathCommand cmd;
    cmd.type = PathCommandType::MoveTo;
    cmd.point = point;
    m_commands.push_back(cmd);
    m_currentPoint = point;
    m_startPoint = point;
    return *this;
}

Path& Path::LineTo(Float32 x, Float32 y) {
    return LineTo(Vec2(x, y));
}

Path& Path::LineTo(Vec2 point) {
    PathCommand cmd;
    cmd.type = PathCommandType::LineTo;
    cmd.point = point;
    m_commands.push_back(cmd);
    m_currentPoint = point;
    return *this;
}

Path& Path::HorizontalTo(Float32 x) {
    return LineTo(x, m_currentPoint.y);
}

Path& Path::VerticalTo(Float32 y) {
    return LineTo(m_currentPoint.x, y);
}

Path& Path::QuadraticTo(Vec2 control, Vec2 end) {
    PathCommand cmd;
    cmd.type = PathCommandType::QuadraticTo;
    cmd.control1 = control;
    cmd.point = end;
    m_commands.push_back(cmd);
    m_currentPoint = end;
    return *this;
}

Path& Path::QuadraticTo(Float32 cx, Float32 cy, Float32 ex, Float32 ey) {
    return QuadraticTo(Vec2(cx, cy), Vec2(ex, ey));
}

Path& Path::CubicTo(Vec2 control1, Vec2 control2, Vec2 end) {
    PathCommand cmd;
    cmd.type = PathCommandType::CubicTo;
    cmd.control1 = control1;
    cmd.control2 = control2;
    cmd.point = end;
    m_commands.push_back(cmd);
    m_currentPoint = end;
    return *this;
}

Path& Path::CubicTo(Float32 c1x, Float32 c1y, Float32 c2x, Float32 c2y, 
                    Float32 ex, Float32 ey) {
    return CubicTo(Vec2(c1x, c1y), Vec2(c2x, c2y), Vec2(ex, ey));
}

Path& Path::ArcTo(Vec2 center, Float32 radius, Float32 startAngle, Float32 endAngle) {
    PathCommand cmd;
    cmd.type = PathCommandType::ArcTo;
    cmd.point = center;
    cmd.radius = radius;
    cmd.control1 = Vec2(startAngle, endAngle); // تخزين الزوايا في control1
    m_commands.push_back(cmd);
    
    // تحديث النقطة الحالية / Update current point
    m_currentPoint = Vec2(
        center.x + radius * std::cos(endAngle),
        center.y + radius * std::sin(endAngle)
    );
    return *this;
}

Path& Path::Close() {
    PathCommand cmd;
    cmd.type = PathCommandType::Close;
    cmd.point = m_startPoint;
    m_commands.push_back(cmd);
    m_currentPoint = m_startPoint;
    return *this;
}

Path& Path::AddRect(const Rect& rect) {
    return AddRect(rect.x, rect.y, rect.width, rect.height);
}

Path& Path::AddRect(Float32 x, Float32 y, Float32 width, Float32 height) {
    MoveTo(x, y);
    LineTo(x + width, y);
    LineTo(x + width, y + height);
    LineTo(x, y + height);
    Close();
    return *this;
}

Path& Path::AddRoundedRect(const Rect& rect, Float32 radius) {
    RoundedRect rr;
    rr.rect = rect;
    rr.cornerRadius = radius;
    rr.useUniformRadius = true;
    return AddRoundedRect(rr);
}

Path& Path::AddRoundedRect(const RoundedRect& rr) {
    Float32 x = rr.rect.x;
    Float32 y = rr.rect.y;
    Float32 w = rr.rect.width;
    Float32 h = rr.rect.height;
    
    Float32 tl, tr, bl, br;
    if (rr.useUniformRadius) {
        tl = tr = bl = br = rr.cornerRadius;
    } else {
        tl = rr.topLeftRadius;
        tr = rr.topRightRadius;
        bl = rr.bottomLeftRadius;
        br = rr.bottomRightRadius;
    }
    
    // تحديد نصف القطر الأقصى / Clamp radii
    Float32 maxRadius = std::min(w, h) / 2.0f;
    tl = std::min(tl, maxRadius);
    tr = std::min(tr, maxRadius);
    bl = std::min(bl, maxRadius);
    br = std::min(br, maxRadius);
    
    // رسم المستطيل المدور باستخدام أقواس / Draw rounded rect with arcs
    MoveTo(x + tl, y);
    LineTo(x + w - tr, y);
    if (tr > 0) ArcTo(Vec2(x + w - tr, y + tr), tr, -PI/2, 0);
    LineTo(x + w, y + h - br);
    if (br > 0) ArcTo(Vec2(x + w - br, y + h - br), br, 0, PI/2);
    LineTo(x + bl, y + h);
    if (bl > 0) ArcTo(Vec2(x + bl, y + h - bl), bl, PI/2, PI);
    LineTo(x, y + tl);
    if (tl > 0) ArcTo(Vec2(x + tl, y + tl), tl, PI, PI * 1.5f);
    Close();
    
    return *this;
}

Path& Path::AddCircle(Vec2 center, Float32 radius) {
    return AddCircle(center.x, center.y, radius);
}

Path& Path::AddCircle(Float32 cx, Float32 cy, Float32 radius) {
    // رسم دائرة كاملة باستخدام 4 أقواس / Draw full circle with 4 arcs
    MoveTo(cx + radius, cy);
    ArcTo(Vec2(cx, cy), radius, 0, PI/2);
    ArcTo(Vec2(cx, cy), radius, PI/2, PI);
    ArcTo(Vec2(cx, cy), radius, PI, PI * 1.5f);
    ArcTo(Vec2(cx, cy), radius, PI * 1.5f, TWO_PI);
    Close();
    return *this;
}

Path& Path::AddEllipse(Vec2 center, Float32 radiusX, Float32 radiusY) {
    // تقريب البيضاوي باستخدام منحنيات بيزييه / Approximate ellipse with Bezier curves
    constexpr Float32 kappa = 0.5522847498f; // 4 * (sqrt(2) - 1) / 3
    
    Float32 ox = radiusX * kappa;
    Float32 oy = radiusY * kappa;
    Float32 cx = center.x;
    Float32 cy = center.y;
    
    MoveTo(cx + radiusX, cy);
    CubicTo(Vec2(cx + radiusX, cy + oy), Vec2(cx + ox, cy + radiusY), Vec2(cx, cy + radiusY));
    CubicTo(Vec2(cx - ox, cy + radiusY), Vec2(cx - radiusX, cy + oy), Vec2(cx - radiusX, cy));
    CubicTo(Vec2(cx - radiusX, cy - oy), Vec2(cx - ox, cy - radiusY), Vec2(cx, cy - radiusY));
    CubicTo(Vec2(cx + ox, cy - radiusY), Vec2(cx + radiusX, cy - oy), Vec2(cx + radiusX, cy));
    Close();
    
    return *this;
}

Path& Path::AddArc(const Arc& arc) {
    Float32 startX = arc.center.x + arc.radius * std::cos(arc.startAngle);
    Float32 startY = arc.center.y + arc.radius * std::sin(arc.startAngle);
    
    MoveTo(startX, startY);
    ArcTo(arc.center, arc.radius, arc.startAngle, arc.endAngle);
    
    return *this;
}

Path& Path::AddPie(const Pie& pie) {
    MoveTo(pie.center);
    
    Float32 startX = pie.center.x + pie.radius * std::cos(pie.startAngle);
    Float32 startY = pie.center.y + pie.radius * std::sin(pie.startAngle);
    LineTo(startX, startY);
    
    ArcTo(pie.center, pie.radius, pie.startAngle, pie.endAngle);
    Close();
    
    return *this;
}

Path& Path::AddRing(const Ring& ring) {
    // الدائرة الخارجية / Outer circle
    Float32 startX = ring.center.x + ring.outerRadius * std::cos(ring.startAngle);
    Float32 startY = ring.center.y + ring.outerRadius * std::sin(ring.startAngle);
    MoveTo(startX, startY);
    ArcTo(ring.center, ring.outerRadius, ring.startAngle, ring.endAngle);
    
    // الدائرة الداخلية (عكس الاتجاه) / Inner circle (reverse direction)
    Float32 innerEndX = ring.center.x + ring.innerRadius * std::cos(ring.endAngle);
    Float32 innerEndY = ring.center.y + ring.innerRadius * std::sin(ring.endAngle);
    LineTo(innerEndX, innerEndY);
    ArcTo(ring.center, ring.innerRadius, ring.endAngle, ring.startAngle);
    
    Close();
    return *this;
}

Path& Path::AddPolygon(const std::vector<Vec2>& points) {
    if (points.empty()) return *this;
    
    MoveTo(points[0]);
    for (size_t i = 1; i < points.size(); ++i) {
        LineTo(points[i]);
    }
    Close();
    
    return *this;
}

Path& Path::AddRegularPolygon(const RegularPolygon& polygon) {
    if (polygon.sides < 3) return *this;
    
    Float32 angleStep = TWO_PI / static_cast<Float32>(polygon.sides);
    
    for (Int32 i = 0; i < polygon.sides; ++i) {
        Float32 angle = polygon.rotation + i * angleStep;
        Float32 x = polygon.center.x + polygon.radius * std::cos(angle);
        Float32 y = polygon.center.y + polygon.radius * std::sin(angle);
        
        if (i == 0) {
            MoveTo(x, y);
        } else {
            LineTo(x, y);
        }
    }
    Close();
    
    return *this;
}

Path& Path::AddStar(const Star& star) {
    if (star.points < 2) return *this;
    
    Float32 angleStep = PI / static_cast<Float32>(star.points);
    
    for (Int32 i = 0; i < star.points * 2; ++i) {
        Float32 radius = (i % 2 == 0) ? star.outerRadius : star.innerRadius;
        Float32 angle = star.rotation + i * angleStep - PI / 2;
        Float32 x = star.center.x + radius * std::cos(angle);
        Float32 y = star.center.y + radius * std::sin(angle);
        
        if (i == 0) {
            MoveTo(x, y);
        } else {
            LineTo(x, y);
        }
    }
    Close();
    
    return *this;
}

Path& Path::AddQuadraticBezier(const QuadraticBezier& bezier) {
    MoveTo(bezier.start);
    QuadraticTo(bezier.control, bezier.end);
    return *this;
}

Path& Path::AddCubicBezier(const CubicBezier& bezier) {
    MoveTo(bezier.start);
    CubicTo(bezier.control1, bezier.control2, bezier.end);
    return *this;
}

Path& Path::AddSpline(const Spline& spline) {
    if (spline.points.size() < 2) return *this;

    // Catmull-Rom spline tessellation
    constexpr Int32 subdivisions = 16;
    size_t n = spline.points.size();

    MoveTo(spline.points[0]);
    for (size_t i = 0; i < n - 1; ++i) {
        Vec2 p0 = spline.points[i > 0 ? i - 1 : i];
        Vec2 p1 = spline.points[i];
        Vec2 p2 = spline.points[i + 1];
        Vec2 p3 = spline.points[i + 2 < n ? i + 2 : i + 1];

        for (Int32 j = 1; j <= subdivisions; ++j) {
            Float32 t = static_cast<Float32>(j) / static_cast<Float32>(subdivisions);
            Float32 t2 = t * t;
            Float32 t3 = t2 * t;

            Float32 x = 0.5f * ((2.0f * p1.x) +
                        (-p0.x + p2.x) * t +
                        (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                        (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);

            Float32 y = 0.5f * ((2.0f * p1.y) +
                        (-p0.y + p2.y) * t +
                        (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
                        (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);

            LineTo(x, y);
        }
    }
    if (spline.closed) {
        Close();
    }

    return *this;
}

Path& Path::Translate(Float32 x, Float32 y) {
    return Translate(Vec2(x, y));
}

Path& Path::Translate(Vec2 offset) {
    for (auto& cmd : m_commands) {
        cmd.point.x += offset.x;
        cmd.point.y += offset.y;
        cmd.control1.x += offset.x;
        cmd.control1.y += offset.y;
        cmd.control2.x += offset.x;
        cmd.control2.y += offset.y;
    }
    m_currentPoint.x += offset.x;
    m_currentPoint.y += offset.y;
    m_startPoint.x += offset.x;
    m_startPoint.y += offset.y;
    return *this;
}

Path& Path::Scale(Float32 s) {
    return Scale(s, s);
}

Path& Path::Scale(Float32 sx, Float32 sy) {
    for (auto& cmd : m_commands) {
        cmd.point.x *= sx;
        cmd.point.y *= sy;
        cmd.control1.x *= sx;
        cmd.control1.y *= sy;
        cmd.control2.x *= sx;
        cmd.control2.y *= sy;
        cmd.radius *= std::max(sx, sy); // تقريب
    }
    m_currentPoint.x *= sx;
    m_currentPoint.y *= sy;
    m_startPoint.x *= sx;
    m_startPoint.y *= sy;
    return *this;
}

Path& Path::Rotate(Float32 angle, Vec2 center) {
    Float32 cosA = std::cos(angle);
    Float32 sinA = std::sin(angle);
    
    auto rotatePoint = [&](Vec2& p) {
        Float32 dx = p.x - center.x;
        Float32 dy = p.y - center.y;
        p.x = center.x + dx * cosA - dy * sinA;
        p.y = center.y + dx * sinA + dy * cosA;
    };
    
    for (auto& cmd : m_commands) {
        rotatePoint(cmd.point);
        rotatePoint(cmd.control1);
        rotatePoint(cmd.control2);
    }
    rotatePoint(m_currentPoint);
    rotatePoint(m_startPoint);
    
    return *this;
}

Rect Path::GetBounds() const {
    if (m_commands.empty()) {
        return Rect(0, 0, 0, 0);
    }
    
    Float32 minX = std::numeric_limits<Float32>::max();
    Float32 minY = std::numeric_limits<Float32>::max();
    Float32 maxX = std::numeric_limits<Float32>::lowest();
    Float32 maxY = std::numeric_limits<Float32>::lowest();
    
    for (const auto& cmd : m_commands) {
        minX = std::min(minX, cmd.point.x);
        minY = std::min(minY, cmd.point.y);
        maxX = std::max(maxX, cmd.point.x);
        maxY = std::max(maxY, cmd.point.y);
        
        if (cmd.type == PathCommandType::QuadraticTo || 
            cmd.type == PathCommandType::CubicTo) {
            minX = std::min(minX, cmd.control1.x);
            minY = std::min(minY, cmd.control1.y);
            maxX = std::max(maxX, cmd.control1.x);
            maxY = std::max(maxY, cmd.control1.y);
        }
        
        if (cmd.type == PathCommandType::CubicTo) {
            minX = std::min(minX, cmd.control2.x);
            minY = std::min(minY, cmd.control2.y);
            maxX = std::max(maxX, cmd.control2.x);
            maxY = std::max(maxY, cmd.control2.y);
        }
    }
    
    return Rect(minX, minY, maxX - minX, maxY - minY);
}

// ==============================================================================
// ShapeRenderer - تنفيذ راسم الأشكال / Shape Renderer Implementation
// ==============================================================================

// Vertex structure for shape rendering
struct ShapeVertex {
    Float32 x, y;        // Position
    Float32 r, g, b, a;  // Color
};

static const char* s_shapeVertexShader = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
uniform mat4 uProjection;
out vec4 vColor;
void main() {
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

static const char* s_shapeFragmentShader = R"(
#version 330 core
in vec4 vColor;
out vec4 FragColor;
void main() {
    FragColor = vColor;
}
)";

static GLuint CompileShapeShader(const char* vertSrc, const char* fragSrc) {
    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertSrc, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragSrc, nullptr);
    glCompileShader(frag);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    glDeleteShader(vert);
    glDeleteShader(frag);
    return prog;
}

static constexpr size_t MAX_SHAPE_VERTICES = 65536;

struct ShapeRenderer::Impl {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint shaderProgram = 0;
    GLint projLoc = -1;
    Mat4 projection;
    bool initialized = false;

    std::vector<ShapeVertex> vertices;

    void Init() {
        shaderProgram = CompileShapeShader(s_shapeVertexShader, s_shapeFragmentShader);
        projLoc = glGetUniformLocation(shaderProgram, "uProjection");

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, MAX_SHAPE_VERTICES * sizeof(ShapeVertex), nullptr, GL_DYNAMIC_DRAW);

        // Position attribute (location 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)0);

        // Color attribute (location 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex),
                              (void*)(2 * sizeof(Float32)));

        glBindVertexArray(0);
        initialized = true;
    }

    void Destroy() {
        if (vao) { glDeleteVertexArrays(1, &vao); vao = 0; }
        if (vbo) { glDeleteBuffers(1, &vbo); vbo = 0; }
        if (shaderProgram) { glDeleteProgram(shaderProgram); shaderProgram = 0; }
        initialized = false;
    }

    void FlushTriangles() {
        if (vertices.empty()) return;

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.m);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Upload in batches if needed
        size_t offset = 0;
        while (offset < vertices.size()) {
            size_t count = std::min(vertices.size() - offset, MAX_SHAPE_VERTICES);
            glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(ShapeVertex), &vertices[offset]);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count));
            offset += count;
        }

        glBindVertexArray(0);
        vertices.clear();
    }

    void AddTriangle(Float32 x1, Float32 y1, Float32 x2, Float32 y2,
                     Float32 x3, Float32 y3, const Color& color) {
        ShapeVertex v1 = { x1, y1, color.r, color.g, color.b, color.a };
        ShapeVertex v2 = { x2, y2, color.r, color.g, color.b, color.a };
        ShapeVertex v3 = { x3, y3, color.r, color.g, color.b, color.a };
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
    }
};

ShapeRenderer::ShapeRenderer() : m_impl(std::make_unique<Impl>()) {}

ShapeRenderer::~ShapeRenderer() {
    Cleanup();
}

bool ShapeRenderer::Initialize() {
    m_impl->Init();
    return m_impl->initialized;
}

void ShapeRenderer::Cleanup() {
    m_impl->Destroy();
}

void ShapeRenderer::Begin(const Mat4& projection) {
    m_impl->projection = projection;
    m_impl->vertices.clear();
}

void ShapeRenderer::End() {
    Flush();
}

void ShapeRenderer::Flush() {
    m_impl->FlushTriangles();
}

// ==============================================================================
// دوال التقسيم / Tessellation Functions
// ==============================================================================

std::vector<Vec2> ShapeRenderer::TessellateCircle(Vec2 center, Float32 radius, Int32 segments) {
    std::vector<Vec2> points;
    points.reserve(segments);
    
    Float32 angleStep = TWO_PI / static_cast<Float32>(segments);
    for (Int32 i = 0; i < segments; ++i) {
        Float32 angle = i * angleStep;
        points.push_back(Vec2(
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle)
        ));
    }
    
    return points;
}

std::vector<Vec2> ShapeRenderer::TessellateArc(Vec2 center, Float32 radius,
                                                Float32 startAngle, Float32 endAngle,
                                                Int32 segments) {
    std::vector<Vec2> points;
    points.reserve(segments + 1);
    
    Float32 angleRange = endAngle - startAngle;
    Float32 angleStep = angleRange / static_cast<Float32>(segments);
    
    for (Int32 i = 0; i <= segments; ++i) {
        Float32 angle = startAngle + i * angleStep;
        points.push_back(Vec2(
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle)
        ));
    }
    
    return points;
}

std::vector<Vec2> ShapeRenderer::TessellateQuadraticBezier(Vec2 p0, Vec2 p1, Vec2 p2, 
                                                            Int32 segments) {
    std::vector<Vec2> points;
    points.reserve(segments + 1);
    
    for (Int32 i = 0; i <= segments; ++i) {
        Float32 t = static_cast<Float32>(i) / static_cast<Float32>(segments);
        Float32 u = 1.0f - t;
        
        Vec2 point;
        point.x = u * u * p0.x + 2 * u * t * p1.x + t * t * p2.x;
        point.y = u * u * p0.y + 2 * u * t * p1.y + t * t * p2.y;
        
        points.push_back(point);
    }
    
    return points;
}

std::vector<Vec2> ShapeRenderer::TessellateCubicBezier(Vec2 p0, Vec2 p1, Vec2 p2, Vec2 p3,
                                                        Int32 segments) {
    std::vector<Vec2> points;
    points.reserve(segments + 1);
    
    for (Int32 i = 0; i <= segments; ++i) {
        Float32 t = static_cast<Float32>(i) / static_cast<Float32>(segments);
        Float32 u = 1.0f - t;
        Float32 tt = t * t;
        Float32 uu = u * u;
        Float32 uuu = uu * u;
        Float32 ttt = tt * t;
        
        Vec2 point;
        point.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
        point.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
        
        points.push_back(point);
    }
    
    return points;
}

std::vector<Vec2> ShapeRenderer::TessellateRoundedRect(const RoundedRect& rect, 
                                                         Int32 cornerSegments) {
    std::vector<Vec2> points;
    
    Float32 x = rect.rect.x;
    Float32 y = rect.rect.y;
    Float32 w = rect.rect.width;
    Float32 h = rect.rect.height;
    Float32 r = rect.cornerRadius;
    
    // تحديد نصف القطر الأقصى / Clamp radius
    r = std::min(r, std::min(w, h) / 2.0f);
    
    // الزاوية العلوية اليسرى / Top-left corner
    auto tlCorner = TessellateArc(Vec2(x + r, y + r), r, PI, PI * 1.5f, cornerSegments);
    points.insert(points.end(), tlCorner.begin(), tlCorner.end());
    
    // الحافة العلوية / Top edge
    points.push_back(Vec2(x + w - r, y));
    
    // الزاوية العلوية اليمنى / Top-right corner
    auto trCorner = TessellateArc(Vec2(x + w - r, y + r), r, -PI / 2, 0, cornerSegments);
    points.insert(points.end(), trCorner.begin(), trCorner.end());
    
    // الحافة اليمنى / Right edge
    points.push_back(Vec2(x + w, y + h - r));
    
    // الزاوية السفلية اليمنى / Bottom-right corner
    auto brCorner = TessellateArc(Vec2(x + w - r, y + h - r), r, 0, PI / 2, cornerSegments);
    points.insert(points.end(), brCorner.begin(), brCorner.end());
    
    // الحافة السفلية / Bottom edge
    points.push_back(Vec2(x + r, y + h));
    
    // الزاوية السفلية اليسرى / Bottom-left corner
    auto blCorner = TessellateArc(Vec2(x + r, y + h - r), r, PI / 2, PI, cornerSegments);
    points.insert(points.end(), blCorner.begin(), blCorner.end());
    
    return points;
}

std::vector<Vec2> ShapeRenderer::TessellatePath(const Path& path, Int32 curveSegments) {
    std::vector<Vec2> points;
    Vec2 currentPoint(0, 0);
    Vec2 startPoint(0, 0);
    
    for (const auto& cmd : path.GetCommands()) {
        switch (cmd.type) {
            case PathCommandType::MoveTo:
                currentPoint = cmd.point;
                startPoint = cmd.point;
                points.push_back(cmd.point);
                break;
                
            case PathCommandType::LineTo:
                points.push_back(cmd.point);
                currentPoint = cmd.point;
                break;
                
            case PathCommandType::QuadraticTo: {
                auto bezierPoints = TessellateQuadraticBezier(
                    currentPoint, cmd.control1, cmd.point, curveSegments);
                // تخطي النقطة الأولى لأنها تكرار / Skip first point (duplicate)
                points.insert(points.end(), bezierPoints.begin() + 1, bezierPoints.end());
                currentPoint = cmd.point;
                break;
            }
            
            case PathCommandType::CubicTo: {
                auto bezierPoints = TessellateCubicBezier(
                    currentPoint, cmd.control1, cmd.control2, cmd.point, curveSegments);
                points.insert(points.end(), bezierPoints.begin() + 1, bezierPoints.end());
                currentPoint = cmd.point;
                break;
            }
            
            case PathCommandType::ArcTo: {
                Float32 startAngle = cmd.control1.x;
                Float32 endAngle = cmd.control1.y;
                auto arcPoints = TessellateArc(cmd.point, cmd.radius, 
                                                startAngle, endAngle, curveSegments);
                points.insert(points.end(), arcPoints.begin() + 1, arcPoints.end());
                currentPoint = arcPoints.back();
                break;
            }
            
            case PathCommandType::Close:
                if (points.size() > 0 && 
                    (points.back().x != startPoint.x || points.back().y != startPoint.y)) {
                    points.push_back(startPoint);
                }
                currentPoint = startPoint;
                break;
        }
    }
    
    return points;
}

// ==============================================================================
// رسم الأشكال الأساسية / Basic Shape Drawing
// ==============================================================================

void ShapeRenderer::DrawLine(Vec2 start, Vec2 end, const StrokeStyle& stroke) {
    DrawPolyline({start, end}, stroke);
}

void ShapeRenderer::DrawLine(Float32 x1, Float32 y1, Float32 x2, Float32 y2, 
                              const StrokeStyle& stroke) {
    DrawLine(Vec2(x1, y1), Vec2(x2, y2), stroke);
}

void ShapeRenderer::DrawPolyline(const std::vector<Vec2>& points, const StrokeStyle& stroke) {
    if (points.size() < 2) return;
    DrawStrokedPolyline(points, stroke, false);
}

void ShapeRenderer::DrawRect(const Rect& rect, const FillStyle& fill, const StrokeStyle& stroke) {
    DrawRect(rect.x, rect.y, rect.width, rect.height, fill, stroke);
}

void ShapeRenderer::DrawRect(Float32 x, Float32 y, Float32 width, Float32 height,
                              const FillStyle& fill, const StrokeStyle& stroke) {
    std::vector<Vec2> vertices = {
        {x, y},
        {x + width, y},
        {x + width, y + height},
        {x, y + height}
    };
    
    if (fill.type != FillStyle::Type::None) {
        DrawFilledPolygon(vertices, fill);
    }
    
    if (stroke.width > 0) {
        DrawStrokedPolyline(vertices, stroke, true);
    }
}

void ShapeRenderer::DrawRoundedRect(const Rect& rect, Float32 radius,
                                     const FillStyle& fill, const StrokeStyle& stroke) {
    RoundedRect rr;
    rr.rect = rect;
    rr.cornerRadius = radius;
    DrawRoundedRect(rr, fill, stroke);
}

void ShapeRenderer::DrawRoundedRect(const RoundedRect& roundedRect,
                                     const FillStyle& fill, const StrokeStyle& stroke) {
    auto vertices = TessellateRoundedRect(roundedRect, m_curveSegments / 4);
    
    if (fill.type != FillStyle::Type::None) {
        DrawFilledPolygon(vertices, fill);
    }
    
    if (stroke.width > 0) {
        DrawStrokedPolyline(vertices, stroke, true);
    }
}

void ShapeRenderer::DrawCircle(Vec2 center, Float32 radius,
                                const FillStyle& fill, const StrokeStyle& stroke) {
    auto vertices = TessellateCircle(center, radius, m_curveSegments);
    
    if (fill.type != FillStyle::Type::None) {
        DrawFilledPolygon(vertices, fill);
    }
    
    if (stroke.width > 0) {
        DrawStrokedPolyline(vertices, stroke, true);
    }
}

void ShapeRenderer::DrawCircle(Float32 cx, Float32 cy, Float32 radius,
                                const FillStyle& fill, const StrokeStyle& stroke) {
    DrawCircle(Vec2(cx, cy), radius, fill, stroke);
}

void ShapeRenderer::DrawEllipse(Vec2 center, Float32 radiusX, Float32 radiusY,
                                 const FillStyle& fill, const StrokeStyle& stroke) {
    std::vector<Vec2> vertices;
    vertices.reserve(m_curveSegments);
    
    Float32 angleStep = TWO_PI / static_cast<Float32>(m_curveSegments);
    for (Int32 i = 0; i < m_curveSegments; ++i) {
        Float32 angle = i * angleStep;
        vertices.push_back(Vec2(
            center.x + radiusX * std::cos(angle),
            center.y + radiusY * std::sin(angle)
        ));
    }
    
    if (fill.type != FillStyle::Type::None) {
        DrawFilledPolygon(vertices, fill);
    }
    
    if (stroke.width > 0) {
        DrawStrokedPolyline(vertices, stroke, true);
    }
}

void ShapeRenderer::DrawArc(const Arc& arc, const StrokeStyle& stroke) {
    auto vertices = TessellateArc(arc.center, arc.radius, 
                                   arc.startAngle, arc.endAngle, m_curveSegments);
    DrawStrokedPolyline(vertices, stroke, false);
}

void ShapeRenderer::DrawPie(const Pie& pie, const FillStyle& fill, const StrokeStyle& stroke) {
    auto arcPoints = TessellateArc(pie.center, pie.radius,
                                    pie.startAngle, pie.endAngle, m_curveSegments);
    
    std::vector<Vec2> vertices;
    vertices.push_back(pie.center);
    vertices.insert(vertices.end(), arcPoints.begin(), arcPoints.end());
    
    if (fill.type != FillStyle::Type::None) {
        DrawFilledPolygon(vertices, fill);
    }
    
    if (stroke.width > 0) {
        DrawStrokedPolyline(vertices, stroke, true);
    }
}

void ShapeRenderer::DrawRing(const Ring& ring, const FillStyle& fill, const StrokeStyle& stroke) {
    // Ring = filled area between inner and outer circles
    Int32 segments = std::max(m_curveSegments, 16);
    Float32 angleStep = TWO_PI / static_cast<Float32>(segments);

    if (fill.type != FillStyle::Type::None) {
        // Tessellate ring as triangle strip between inner and outer radii
        for (Int32 i = 0; i < segments; ++i) {
            Float32 a1 = i * angleStep;
            Float32 a2 = (i + 1) * angleStep;

            Float32 cos1 = std::cos(a1), sin1 = std::sin(a1);
            Float32 cos2 = std::cos(a2), sin2 = std::sin(a2);

            Vec2 outer1(ring.center.x + ring.outerRadius * cos1, ring.center.y + ring.outerRadius * sin1);
            Vec2 inner1(ring.center.x + ring.innerRadius * cos1, ring.center.y + ring.innerRadius * sin1);
            Vec2 outer2(ring.center.x + ring.outerRadius * cos2, ring.center.y + ring.outerRadius * sin2);
            Vec2 inner2(ring.center.x + ring.innerRadius * cos2, ring.center.y + ring.innerRadius * sin2);

            // Two triangles per segment
            std::vector<Vec2> quad = { outer1, inner1, outer2, inner2 };
            DrawFilledPolygon({ outer1, inner1, outer2 }, fill);
            DrawFilledPolygon({ inner1, inner2, outer2 }, fill);
        }
    }

    if (stroke.width > 0) {
        // Stroke outer circle
        auto outerPts = TessellateArc(ring.center, ring.outerRadius, 0.0f, TWO_PI, segments);
        outerPts.push_back(outerPts.front()); // close
        DrawStrokedPolyline(outerPts, stroke, true);

        // Stroke inner circle
        auto innerPts = TessellateArc(ring.center, ring.innerRadius, 0.0f, TWO_PI, segments);
        innerPts.push_back(innerPts.front());
        DrawStrokedPolyline(innerPts, stroke, true);
    }
}

void ShapeRenderer::DrawPolygon(const std::vector<Vec2>& points,
                                 const FillStyle& fill, const StrokeStyle& stroke) {
    if (points.size() < 3) return;
    
    if (fill.type != FillStyle::Type::None) {
        DrawFilledPolygon(points, fill);
    }
    
    if (stroke.width > 0) {
        DrawStrokedPolyline(points, stroke, true);
    }
}

void ShapeRenderer::DrawRegularPolygon(const RegularPolygon& polygon,
                                        const FillStyle& fill, const StrokeStyle& stroke) {
    if (polygon.sides < 3) return;
    
    std::vector<Vec2> vertices;
    vertices.reserve(polygon.sides);
    
    Float32 angleStep = TWO_PI / static_cast<Float32>(polygon.sides);
    for (Int32 i = 0; i < polygon.sides; ++i) {
        Float32 angle = polygon.rotation + i * angleStep;
        vertices.push_back(Vec2(
            polygon.center.x + polygon.radius * std::cos(angle),
            polygon.center.y + polygon.radius * std::sin(angle)
        ));
    }
    
    if (fill.type != FillStyle::Type::None) {
        DrawFilledPolygon(vertices, fill);
    }
    
    if (stroke.width > 0) {
        DrawStrokedPolyline(vertices, stroke, true);
    }
}

void ShapeRenderer::DrawStar(const Star& star, const FillStyle& fill, const StrokeStyle& stroke) {
    if (star.points < 2) return;
    
    std::vector<Vec2> vertices;
    vertices.reserve(star.points * 2);
    
    Float32 angleStep = PI / static_cast<Float32>(star.points);
    for (Int32 i = 0; i < star.points * 2; ++i) {
        Float32 radius = (i % 2 == 0) ? star.outerRadius : star.innerRadius;
        Float32 angle = star.rotation + i * angleStep - PI / 2;
        vertices.push_back(Vec2(
            star.center.x + radius * std::cos(angle),
            star.center.y + radius * std::sin(angle)
        ));
    }
    
    if (fill.type != FillStyle::Type::None) {
        DrawFilledPolygon(vertices, fill);
    }
    
    if (stroke.width > 0) {
        DrawStrokedPolyline(vertices, stroke, true);
    }
}

void ShapeRenderer::DrawTriangle(Vec2 p1, Vec2 p2, Vec2 p3,
                                  const FillStyle& fill, const StrokeStyle& stroke) {
    DrawPolygon({p1, p2, p3}, fill, stroke);
}

// ==============================================================================
// رسم المنحنيات / Curve Drawing
// ==============================================================================

void ShapeRenderer::DrawQuadraticBezier(const QuadraticBezier& bezier, 
                                         const StrokeStyle& stroke) {
    auto vertices = TessellateQuadraticBezier(bezier.start, bezier.control, 
                                               bezier.end, m_curveSegments);
    DrawStrokedPolyline(vertices, stroke, false);
}

void ShapeRenderer::DrawCubicBezier(const CubicBezier& bezier, const StrokeStyle& stroke) {
    auto vertices = TessellateCubicBezier(bezier.start, bezier.control1,
                                           bezier.control2, bezier.end, m_curveSegments);
    DrawStrokedPolyline(vertices, stroke, false);
}

void ShapeRenderer::DrawSpline(const Spline& spline, const StrokeStyle& stroke) {
    if (spline.points.size() < 2) return;

    // Catmull-Rom spline interpolation
    std::vector<Vec2> smoothPoints;
    Int32 subdivisions = m_curveSegments;
    
    size_t n = spline.points.size();
    for (size_t i = 0; i < n - 1; ++i) {
        // Get 4 control points: p0, p1, p2, p3
        Vec2 p0 = spline.points[i > 0 ? i - 1 : i];
        Vec2 p1 = spline.points[i];
        Vec2 p2 = spline.points[i + 1];
        Vec2 p3 = spline.points[i + 2 < n ? i + 2 : i + 1];

        for (Int32 j = 0; j < subdivisions; ++j) {
            Float32 t = static_cast<Float32>(j) / static_cast<Float32>(subdivisions);
            Float32 t2 = t * t;
            Float32 t3 = t2 * t;

            // Catmull-Rom matrix coefficients (tension = 0.5)
            Float32 x = 0.5f * ((2.0f * p1.x) +
                        (-p0.x + p2.x) * t +
                        (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                        (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);

            Float32 y = 0.5f * ((2.0f * p1.y) +
                        (-p0.y + p2.y) * t +
                        (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
                        (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);

            smoothPoints.push_back(Vec2(x, y));
        }
    }
    // Add last point
    smoothPoints.push_back(spline.points.back());

    DrawStrokedPolyline(smoothPoints, stroke, spline.closed);
}

// ==============================================================================
// رسم المسار / Path Drawing
// ==============================================================================

void ShapeRenderer::StrokePath(const Path& path, const StrokeStyle& stroke) {
    auto vertices = TessellatePath(path, m_curveSegments);
    if (!vertices.empty()) {
        DrawStrokedPolyline(vertices, stroke, false);
    }
}

void ShapeRenderer::FillPath(const Path& path, const FillStyle& fill) {
    auto vertices = TessellatePath(path, m_curveSegments);
    if (vertices.size() >= 3) {
        DrawFilledPolygon(vertices, fill);
    }
}

void ShapeRenderer::DrawPath(const Path& path, const FillStyle& fill, const StrokeStyle& stroke) {
    if (fill.type != FillStyle::Type::None) {
        FillPath(path, fill);
    }
    if (stroke.width > 0) {
        StrokePath(path, stroke);
    }
}

// ==============================================================================
// أشكال خاصة / Special Shapes
// ==============================================================================

void ShapeRenderer::DrawArrow(Vec2 start, Vec2 end, Float32 headSize, const StrokeStyle& stroke) {
    // رسم الخط الرئيسي / Draw main line
    DrawLine(start, end, stroke);
    
    // حساب اتجاه السهم / Calculate arrow direction
    Float32 dx = end.x - start.x;
    Float32 dy = end.y - start.y;
    Float32 length = std::sqrt(dx * dx + dy * dy);
    if (length < 0.001f) return;
    
    dx /= length;
    dy /= length;
    
    // نقاط رأس السهم / Arrow head points
    Float32 angle = std::atan2(dy, dx);
    Float32 headAngle = 0.5f; // ~30 degrees
    
    Vec2 left(
        end.x - headSize * std::cos(angle - headAngle),
        end.y - headSize * std::sin(angle - headAngle)
    );
    Vec2 right(
        end.x - headSize * std::cos(angle + headAngle),
        end.y - headSize * std::sin(angle + headAngle)
    );
    
    DrawLine(end, left, stroke);
    DrawLine(end, right, stroke);
}

void ShapeRenderer::DrawDoubleArrow(Vec2 start, Vec2 end, Float32 headSize,
                                     const StrokeStyle& stroke) {
    DrawArrow(start, end, headSize, stroke);
    DrawArrow(end, start, headSize, stroke);
}

void ShapeRenderer::DrawGrid(const Rect& bounds, Float32 cellWidth, Float32 cellHeight,
                              const StrokeStyle& stroke) {
    // الخطوط العمودية / Vertical lines
    for (Float32 x = bounds.x; x <= bounds.x + bounds.width; x += cellWidth) {
        DrawLine(Vec2(x, bounds.y), Vec2(x, bounds.y + bounds.height), stroke);
    }
    
    // الخطوط الأفقية / Horizontal lines
    for (Float32 y = bounds.y; y <= bounds.y + bounds.height; y += cellHeight) {
        DrawLine(Vec2(bounds.x, y), Vec2(bounds.x + bounds.width, y), stroke);
    }
}

void ShapeRenderer::DrawX(Vec2 center, Float32 size, const StrokeStyle& stroke) {
    Float32 half = size / 2;
    DrawLine(Vec2(center.x - half, center.y - half), 
             Vec2(center.x + half, center.y + half), stroke);
    DrawLine(Vec2(center.x + half, center.y - half), 
             Vec2(center.x - half, center.y + half), stroke);
}

void ShapeRenderer::DrawCheckmark(Vec2 center, Float32 size, const StrokeStyle& stroke) {
    Float32 quarterW = size / 4;
    Float32 halfH = size / 2;
    
    Vec2 start(center.x - quarterW * 2, center.y);
    Vec2 middle(center.x - quarterW, center.y + halfH * 0.6f);
    Vec2 end(center.x + quarterW * 2, center.y - halfH);
    
    DrawPolyline({start, middle, end}, stroke);
}

void ShapeRenderer::DrawHeart(Vec2 center, Float32 size, const FillStyle& fill,
                               const StrokeStyle& stroke) {
    // رسم قلب باستخدام منحنيات بيزييه / Draw heart using Bezier curves
    Path path;
    
    Float32 s = size / 2;
    path.MoveTo(center.x, center.y + s * 0.8f); // الطرف السفلي / Bottom tip
    
    // الجانب الأيسر / Left side
    path.CubicTo(
        Vec2(center.x - s * 1.5f, center.y),
        Vec2(center.x - s * 1.2f, center.y - s),
        Vec2(center.x, center.y - s * 0.3f)
    );
    
    // الجانب الأيمن / Right side
    path.CubicTo(
        Vec2(center.x + s * 1.2f, center.y - s),
        Vec2(center.x + s * 1.5f, center.y),
        Vec2(center.x, center.y + s * 0.8f)
    );
    
    path.Close();
    
    DrawPath(path, fill, stroke);
}

// ==============================================================================
// دوال الرسم الداخلية / Internal Drawing Functions
// ==============================================================================

void ShapeRenderer::DrawFilledPolygon(const std::vector<Vec2>& vertices, const FillStyle& fill) {
    if (vertices.size() < 3) return;

    Color color = fill.color;

    // Ear clipping triangulation for simple polygons
    std::vector<Vec2> poly(vertices);

    // Ensure counter-clockwise winding
    Float32 area = 0.0f;
    for (size_t i = 0, n = poly.size(); i < n; ++i) {
        size_t j = (i + 1) % n;
        area += poly[i].x * poly[j].y;
        area -= poly[j].x * poly[i].y;
    }
    if (area > 0.0f) {
        std::reverse(poly.begin(), poly.end());
    }

    // Simple ear clipping
    std::vector<size_t> indices;
    for (size_t i = 0; i < poly.size(); ++i) indices.push_back(i);

    size_t safety = poly.size() * poly.size();
    while (indices.size() > 2 && safety > 0) {
        --safety;
        bool earFound = false;
        size_t n = indices.size();
        for (size_t i = 0; i < n; ++i) {
            size_t prev = indices[(i + n - 1) % n];
            size_t curr = indices[i];
            size_t next = indices[(i + 1) % n];

            Vec2 a = poly[prev], b = poly[curr], c = poly[next];

            // Check if this is a convex vertex (ear tip)
            Float32 cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
            if (cross >= 0.0f) continue; // Reflex vertex, not an ear

            // Check no other vertex is inside this triangle
            bool inside = false;
            for (size_t j = 0; j < n; ++j) {
                if (j == (i + n - 1) % n || j == i || j == (i + 1) % n) continue;
                Vec2 p = poly[indices[j]];
                // Barycentric test
                Float32 d1 = (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
                Float32 d2 = (p.x - b.x) * (c.y - b.y) - (p.y - b.y) * (c.x - b.x);
                Float32 d3 = (p.x - c.x) * (a.y - c.y) - (p.y - c.y) * (a.x - c.x);
                bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
                bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
                if (!(has_neg && has_pos)) { inside = true; break; }
            }
            if (inside) continue;

            // Found an ear - emit triangle
            m_impl->AddTriangle(a.x, a.y, b.x, b.y, c.x, c.y, color);
            indices.erase(indices.begin() + static_cast<std::ptrdiff_t>(i));
            earFound = true;
            break;
        }
        if (!earFound) break;
    }
}

void ShapeRenderer::DrawStrokedPolyline(const std::vector<Vec2>& vertices, 
                                         const StrokeStyle& stroke, bool closed) {
    if (vertices.size() < 2) return;

    Color color = stroke.color;
    Float32 halfW = stroke.width * 0.5f;

    size_t n = vertices.size();
    size_t lineCount = closed ? n : n - 1;

    for (size_t i = 0; i < lineCount; ++i) {
        Vec2 p1 = vertices[i];
        Vec2 p2 = vertices[(i + 1) % n];

        Float32 dx = p2.x - p1.x;
        Float32 dy = p2.y - p1.y;
        Float32 len = std::sqrt(dx * dx + dy * dy);
        if (len < 1e-6f) continue;

        // Perpendicular normal
        Float32 nx = -dy / len * halfW;
        Float32 ny =  dx / len * halfW;

        // Build quad as two triangles
        Vec2 a(p1.x + nx, p1.y + ny);
        Vec2 b(p1.x - nx, p1.y - ny);
        Vec2 c(p2.x - nx, p2.y - ny);
        Vec2 d(p2.x + nx, p2.y + ny);

        m_impl->AddTriangle(a.x, a.y, b.x, b.y, c.x, c.y, color);
        m_impl->AddTriangle(a.x, a.y, c.x, c.y, d.x, d.y, color);
    }
}

// ==============================================================================
// الوصول العالمي / Global Access
// ==============================================================================

ShapeRenderer& GetShapeRenderer() {
    static ShapeRenderer instance;
    return instance;
}

} // namespace graphics
} // namespace sad
