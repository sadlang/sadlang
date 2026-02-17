// ==============================================================================
// renderer2d.cpp - تطبيق محرك الرسم ثنائي الأبعاد / 2D Renderer Implementation
// ==============================================================================
// الوصف: تطبيق محرك الرسم عالي الأداء
// Description: High-performance renderer implementation
// ==============================================================================

#include "../../include/rendering/renderer2d.h"  // ملف الرأس / Header file
#include "../../include/text/arabic_text.h"      // دعم النص العربي / Arabic text support
#include <glad/glad.h>                           // GLAD OpenGL loader
#include <SDL.h>                                 // SDL2 for window functions
#include <algorithm>                             // std::reverse
#ifdef DrawText
#undef DrawText                                  // Remove Win32 DrawText macro collision
#endif
#include <cmath>                                 // للدوال الرياضية / For math functions
#include <cstring>                               // لعمليات الذاكرة / For memory operations
#include <iostream>                              // للطباعة / For printing

namespace SadGraphics {                          // مساحة الأسماء / Namespace

// ثوابت رياضية / Mathematical constants
constexpr Float32 PI = 3.14159265359f;           // باي / Pi
constexpr Float32 TWO_PI = 2.0f * PI;            // 2 باي / 2 Pi

// ==============================================================================
// الإنشاء والتدمير / Construction and Destruction
// ==============================================================================

/// المنشئ / Constructor
Renderer2D::Renderer2D(RenderContext* context)
    : m_context(context)                         // حفظ السياق / Store context
    , m_defaultShader(CreateDefaultShader())     // إنشاء shader افتراضي / Create default shader
    , m_textureShader()                          // shader للـ textures / Shader for textures
    , m_vao(0)                                   // تهيئة VAO / Initialize VAO
    , m_vbo(0)                                   // تهيئة VBO / Initialize VBO
    , m_ebo(0)                                   // تهيئة EBO / Initialize EBO
    , m_drawColor(Color::White)                  // اللون الافتراضي أبيض / Default color white
    , m_lineThickness(1.0f)                      // سمك الخط افتراضي / Default line thickness
    , m_currentTexture(nullptr)                  // لا يوجد texture / No texture
    , m_textureEnabled(false)                    // textures معطلة / Textures disabled
    , m_currentBatchMode(DrawMode::Triangles)    // نمط الدفعة الافتراضي / Default batch mode
{
    // تهيئة المخازن / Initialize buffers
    InitializeBuffers();                         // إنشاء المخازن / Create buffers
    
    // إنشاء texture shader / Create texture shader
    CreateTextureShader();                       // إنشاء shader للصور / Create shader for images
    
    // تهيئة المصفوفات / Initialize matrices
    CreateIdentityMatrix(m_transformMatrix);     // مصفوفة هوية للتحويل / Identity for transform
    
    // ضبط إسقاط افتراضي / Set default projection
    Int32 width, height;                         // أبعاد النافذة / Window dimensions
    SDL_GetWindowSize(m_context->GetWindow(), &width, &height);  // الحصول على الأبعاد / Get dimensions
    SetOrthographicProjection(0.0f, (Float32)width, (Float32)height, 0.0f);  // ضبط الإسقاط / Set projection
    
    // إعادة تعيين الإحصائيات / Reset statistics
    m_stats.Reset();                             // تنظيف الإحصائيات / Clean stats
}

/// المدمر / Destructor
Renderer2D::~Renderer2D() {
    DestroyBuffers();                            // تدمير المخازن / Destroy buffers
}

// ==============================================================================
// إدارة الإطار / Frame Management
// ==============================================================================

/// بدء إطار جديد / Begin new frame
void Renderer2D::BeginFrame() {
    // إعادة تعيين الإحصائيات / Reset statistics
    m_stats.Reset();                             // تنظيف الإحصائيات / Clean stats
    
    // تفريغ المخازن / Clear buffers
    m_vertices.clear();                          // تنظيف الرؤوس / Clear vertices
    m_indices.clear();                           // تنظيف الفهارس / Clear indices
}

/// إنهاء الإطار / End frame
void Renderer2D::EndFrame() {
    // دفع أي رسومات متبقية / Flush any remaining draws
    if (!m_vertices.empty()) {                   // إذا كان هناك رؤوس / If vertices exist
        FlushBatch(m_currentBatchMode);          // دفع الدفعة بالنمط الحالي / Flush with current mode
    }
    // إعادة تعيين حالة الدفعة / Reset batch state
    m_textureEnabled = false;
    m_currentTexture = nullptr;
    m_currentBatchMode = DrawMode::Triangles;
}

/// مسح الشاشة / Clear screen
void Renderer2D::Clear(const Color& color) {
    m_context->Clear(color);                     // استخدام سياق الرسم للمسح / Use render context to clear
}

// ==============================================================================
// رسم الأشكال الأساسية / Drawing Basic Shapes
// ==============================================================================

/// رسم خط / Draw line
void Renderer2D::DrawLine(Float32 x1, Float32 y1, Float32 x2, Float32 y2,
                          const Color& color, Float32 thickness) {
    // تحضير دفعة الخطوط / Prepare line batch
    BeginBatch(DrawMode::Lines);
    
    // إضافة رؤوس الخط / Add line vertices
    AddLineVertices(x1, y1, x2, y2, color, thickness);
    // التجميع - لا دفع / Batching - no flush
}

/// رسم مستطيل / Draw rectangle
void Renderer2D::DrawRect(Float32 x, Float32 y, Float32 width, Float32 height,
                          const Color& color, bool filled) {
    if (filled) {                                // إذا كان ممتلئ / If filled
        // تحضير دفعة المثلثات / Prepare triangle batch
        BeginBatch(DrawMode::Triangles);
        
        // إضافة 4 رؤوس للمستطيل / Add 4 vertices for rectangle
        UInt32 startIndex = static_cast<UInt32>(m_vertices.size());   // فهرس البداية / Start index
        
        AddVertex(x, y, color);                  // الزاوية العلوية اليسرى / Top-left
        AddVertex(x + width, y, color);          // الزاوية العلوية اليمنى / Top-right
        AddVertex(x + width, y + height, color); // الزاوية السفلية اليمنى / Bottom-right
        AddVertex(x, y + height, color);         // الزاوية السفلية اليسرى / Bottom-left
        
        // إضافة الفهارس (مثلثين) / Add indices (two triangles)
        m_indices.push_back(startIndex + 0);     // المثلث الأول / First triangle
        m_indices.push_back(startIndex + 1);     // المثلث الأول / First triangle
        m_indices.push_back(startIndex + 2);     // المثلث الأول / First triangle
        
        m_indices.push_back(startIndex + 0);     // المثلث الثاني / Second triangle
        m_indices.push_back(startIndex + 2);     // المثلث الثاني / Second triangle
        m_indices.push_back(startIndex + 3);     // المثلث الثاني / Second triangle
        
        // التجميع - لا دفع / Batching - no flush
    } else {                                     // إذا كان فارغ / If outline
        // رسم 4 خطوط / Draw 4 lines
        DrawLine(x, y, x + width, y, color, m_lineThickness);              // أعلى / Top
        DrawLine(x + width, y, x + width, y + height, color, m_lineThickness);  // يمين / Right
        DrawLine(x + width, y + height, x, y + height, color, m_lineThickness); // أسفل / Bottom
        DrawLine(x, y + height, x, y, color, m_lineThickness);             // يسار / Left
    }
}

/// رسم مستطيل مع حدود / Draw rectangle with outline
void Renderer2D::DrawRectOutline(Float32 x, Float32 y, Float32 width, Float32 height,
                                 const Color& fillColor, const Color& outlineColor,
                                 Float32 outlineThickness) {
    // رسم المستطيل الممتلئ أولاً / Draw filled rectangle first
    DrawRect(x, y, width, height, fillColor, true);
    
    // رسم الحدود / Draw outline
    Float32 prevThickness = m_lineThickness;     // حفظ السمك السابق / Save previous thickness
    SetLineThickness(outlineThickness);          // ضبط السمك الجديد / Set new thickness
    DrawRect(x, y, width, height, outlineColor, false);  // رسم الحدود / Draw outline
    SetLineThickness(prevThickness);             // استعادة السمك / Restore thickness
}

/// رسم دائرة / Draw circle
void Renderer2D::DrawCircle(Float32 x, Float32 y, Float32 radius,
                            const Color& color, bool filled, Int32 segments) {
    if (segments < 3) segments = 3;              // الحد الأدنى 3 أضلاع / Minimum 3 segments
    
    if (filled) {                                // إذا كانت ممتلئة / If filled
        // تحضير دفعة المثلثات / Prepare triangle batch
        BeginBatch(DrawMode::Triangles);
        
        // إضافة نقطة المركز / Add center point
        UInt32 centerIndex = static_cast<UInt32>(m_vertices.size());  // فهرس المركز / Center index
        AddVertex(x, y, color);                  // المركز / Center
        
        // إضافة نقاط المحيط / Add perimeter points
        Float32 angleStep = TWO_PI / segments;   // خطوة الزاوية / Angle step
        for (Int32 i = 0; i <= segments; i++) {  // المرور على الأضلاع / Iterate segments
            Float32 angle = i * angleStep;       // الزاوية الحالية / Current angle
            Float32 px = x + radius * std::cos(angle);  // الموقع x / X position
            Float32 py = y + radius * std::sin(angle);  // الموقع y / Y position
            AddVertex(px, py, color);            // إضافة الرأس / Add vertex
            
            // إضافة مثلث / Add triangle
            if (i > 0) {                         // ليس الضلع الأول / Not first segment
                m_indices.push_back(centerIndex);           // المركز / Center
                m_indices.push_back(centerIndex + i);       // النقطة السابقة / Previous point
                m_indices.push_back(centerIndex + i + 1);   // النقطة الحالية / Current point
            }
        }
        
        // التجميع - لا دفع / Batching - no flush
    } else {                                     // إذا كانت فارغة / If outline
        // رسم كخطوط متصلة / Draw as line loop
        Float32 angleStep = TWO_PI / segments;   // خطوة الزاوية / Angle step
        Float32 prevX = x + radius;              // النقطة السابقة x / Previous x
        Float32 prevY = y;                       // النقطة السابقة y / Previous y
        
        for (Int32 i = 1; i <= segments; i++) {  // المرور على الأضلاع / Iterate segments
            Float32 angle = i * angleStep;       // الزاوية / Angle
            Float32 px = x + radius * std::cos(angle);  // الموقع x / X position
            Float32 py = y + radius * std::sin(angle);  // الموقع y / Y position
            
            DrawLine(prevX, prevY, px, py, color, m_lineThickness);  // رسم خط / Draw line
            
            prevX = px;                          // تحديث السابق / Update previous
            prevY = py;                          // تحديث السابق / Update previous
        }
    }
}

/// رسم قطع ناقص / Draw ellipse
void Renderer2D::DrawEllipse(Float32 x, Float32 y, Float32 radiusX, Float32 radiusY,
                             const Color& color, bool filled, Int32 segments) {
    if (segments < 3) segments = 3;              // الحد الأدنى / Minimum segments
    
    Float32 angleStep = TWO_PI / segments;       // خطوة الزاوية / Angle step
    
    if (filled) {                                // ممتلئ / Filled
        // تحضير دفعة المثلثات / Prepare triangle batch
        BeginBatch(DrawMode::Triangles);
        
        UInt32 centerIndex = static_cast<UInt32>(m_vertices.size());  // فهرس المركز / Center index
        AddVertex(x, y, color);                  // المركز / Center
        
        for (Int32 i = 0; i <= segments; i++) {  // الأضلاع / Segments
            Float32 angle = i * angleStep;       // الزاوية / Angle
            Float32 px = x + radiusX * std::cos(angle);  // x
            Float32 py = y + radiusY * std::sin(angle);  // y
            AddVertex(px, py, color);            // إضافة / Add
            
            if (i > 0) {                         // مثلث / Triangle
                m_indices.push_back(centerIndex);
                m_indices.push_back(centerIndex + i);
                m_indices.push_back(centerIndex + i + 1);
            }
        }
        
        // التجميع - لا دفع / Batching - no flush
    } else {                                     // فارغ / Outline
        Float32 prevX = x + radiusX;             // السابق / Previous
        Float32 prevY = y;
        
        for (Int32 i = 1; i <= segments; i++) {  // الأضلاع / Segments
            Float32 angle = i * angleStep;
            Float32 px = x + radiusX * std::cos(angle);
            Float32 py = y + radiusY * std::sin(angle);
            
            DrawLine(prevX, prevY, px, py, color, m_lineThickness);
            
            prevX = px;
            prevY = py;
        }
    }
}

/// رسم مضلع / Draw polygon
void Renderer2D::DrawPolygon(const Point2D* points, Int32 numPoints,
                             const Color& color, bool filled) {
    if (numPoints < 3) return;                   // الحد الأدنى 3 نقاط / Minimum 3 points
    
    if (filled) {                                // ممتلئ / Filled
        // تحضير دفعة المثلثات / Prepare triangle batch
        BeginBatch(DrawMode::Triangles);
        
        // استخدام تثليث بسيط (fan triangulation) / Simple fan triangulation
        UInt32 startIndex = static_cast<UInt32>(m_vertices.size());   // البداية / Start
        
        for (Int32 i = 0; i < numPoints; i++) {  // إضافة الرؤوس / Add vertices
            AddVertex(points[i].x, points[i].y, color);
        }
        
        // إضافة المثلثات / Add triangles
        for (Int32 i = 1; i < numPoints - 1; i++) {
            m_indices.push_back(startIndex);     // الرأس الأول / First vertex
            m_indices.push_back(startIndex + i);
            m_indices.push_back(startIndex + i + 1);
        }
        
        // التجميع - لا دفع / Batching - no flush
    } else {                                     // فارغ / Outline
        // رسم خطوط بين النقاط / Draw lines between points
        for (Int32 i = 0; i < numPoints; i++) {
            Int32 next = (i + 1) % numPoints;    // النقطة التالية / Next point
            DrawLine(points[i].x, points[i].y, points[next].x, points[next].y, 
                    color, m_lineThickness);
        }
    }
}

/// رسم مثلث / Draw triangle
void Renderer2D::DrawTriangle(Float32 x1, Float32 y1, Float32 x2, Float32 y2,
                              Float32 x3, Float32 y3, const Color& color, bool filled) {
    Point2D points[3] = {{x1, y1}, {x2, y2}, {x3, y3}};  // النقاط / Points
    DrawPolygon(points, 3, color, filled);     // رسم كمضلع / Draw as polygon
}

/// رسم قوس / Draw arc
void Renderer2D::DrawArc(Float32 x, Float32 y, Float32 radius,
                         Float32 startAngle, Float32 endAngle,
                         const Color& color, Int32 segments) {
    if (segments < 2) segments = 2;              // الحد الأدنى / Minimum
    
    Float32 angleRange = endAngle - startAngle;  // مدى الزاوية / Angle range
    Float32 angleStep = angleRange / segments;   // خطوة الزاوية / Angle step
    
    Float32 prevX = x + radius * std::cos(startAngle);  // البداية / Start
    Float32 prevY = y + radius * std::sin(startAngle);
    
    for (Int32 i = 1; i <= segments; i++) {      // الأضلاع / Segments
        Float32 angle = startAngle + i * angleStep;
        Float32 px = x + radius * std::cos(angle);
        Float32 py = y + radius * std::sin(angle);
        
        DrawLine(prevX, prevY, px, py, color, m_lineThickness);
        
        prevX = px;
        prevY = py;
    }
}

// ==============================================================================
// التحويلات / Transformations
// ==============================================================================

/// حفظ التحويل / Push transform
void Renderer2D::PushTransform() {
    // حفظ المصفوفة الحالية في الكومة / Save current matrix to stack
    for (int i = 0; i < 16; i++) {               // نسخ المصفوفة / Copy matrix
        m_transformStack.push_back(m_transformMatrix[i]);
    }
}

/// استرجاع التحويل / Pop transform
void Renderer2D::PopTransform() {
    if (m_transformStack.size() >= 16) {         // التحقق من وجود مصفوفة / Check matrix exists
        // استعادة المصفوفة من الكومة / Restore matrix from stack
        for (int i = 15; i >= 0; i--) {          // النسخ بالعكس / Copy in reverse
            m_transformMatrix[i] = m_transformStack.back();
            m_transformStack.pop_back();
        }
    }
}

/// تحريك / Translate
void Renderer2D::Translate(Float32 x, Float32 y) {
    // إنشاء مصفوفة ترجمة / Create translation matrix
    Float32 translation[16];
    CreateIdentityMatrix(translation);           // البدء بمصفوفة هوية / Start with identity
    translation[12] = x;                         // X translation
    translation[13] = y;                         // Y translation
    
    // ضرب المصفوفات / Multiply matrices
    Float32 result[16];
    MultiplyMatrices(m_transformMatrix, translation, result);
    std::memcpy(m_transformMatrix, result, sizeof(result));  // نسخ النتيجة / Copy result
}

/// تدوير / Rotate
void Renderer2D::Rotate(Float32 angle) {
    // إنشاء مصفوفة تدوير / Create rotation matrix
    Float32 rotation[16];
    CreateIdentityMatrix(rotation);              // البدء بمصفوفة هوية / Start with identity
    
    Float32 c = std::cos(angle);                 // cos
    Float32 s = std::sin(angle);                 // sin
    
    rotation[0] = c;                             // [0,0]
    rotation[1] = s;                             // [0,1]
    rotation[4] = -s;                            // [1,0]
    rotation[5] = c;                             // [1,1]
    
    // ضرب المصفوفات / Multiply matrices
    Float32 result[16];
    MultiplyMatrices(m_transformMatrix, rotation, result);
    std::memcpy(m_transformMatrix, result, sizeof(result));  // نسخ / Copy
}

/// قياس / Scale
void Renderer2D::Scale(Float32 x, Float32 y) {
    // إنشاء مصفوفة قياس / Create scale matrix
    Float32 scale[16];
    CreateIdentityMatrix(scale);                 // هوية / Identity
    scale[0] = x;                                // X scale
    scale[5] = y;                                // Y scale
    
    // ضرب / Multiply
    Float32 result[16];
    MultiplyMatrices(m_transformMatrix, scale, result);
    std::memcpy(m_transformMatrix, result, sizeof(result));  // نسخ / Copy
}

/// إعادة تعيين التحويل / Reset transform
void Renderer2D::ResetTransform() {
    CreateIdentityMatrix(m_transformMatrix);     // إنشاء مصفوفة هوية / Create identity
}

// ==============================================================================
// إعدادات الرسم / Drawing Settings
// ==============================================================================

/// ضبط منطقة العرض / Set viewport
void Renderer2D::SetViewport(Int32 x, Int32 y, Int32 width, Int32 height) {
    m_context->SetViewport(x, y, width, height); // استخدام السياق / Use context
}

/// ضبط إسقاط متعامد / Set orthographic projection
void Renderer2D::SetOrthographicProjection(Float32 left, Float32 right,
                                           Float32 bottom, Float32 top) {
    CreateOrthographicMatrix(left, right, bottom, top);  // إنشاء المصفوفة / Create matrix
}

/// ضبط اللون / Set draw color
void Renderer2D::SetDrawColor(const Color& color) {
    m_drawColor = color;                         // حفظ اللون / Store color
}

/// ضبط سمك الخط / Set line thickness
void Renderer2D::SetLineThickness(Float32 thickness) {
    m_lineThickness = thickness;                 // حفظ السمك / Store thickness
    m_context->SetLineWidth(thickness);          // تطبيق في OpenGL / Apply in OpenGL
}

// ==============================================================================
// إحصائيات / Statistics
// ==============================================================================

/// طباعة الإحصائيات / Print statistics
void Renderer2D::PrintStats() const {
    std::cout << "Render Statistics:\n";         // العنوان / Title
    std::cout << "  Draw Calls: " << m_stats.drawCalls << "\n";
    std::cout << "  Vertices: " << m_stats.vertices << "\n";
    std::cout << "  Triangles: " << m_stats.triangles << "\n";
}

// ==============================================================================
// الدوال الخاصة / Private Functions
// ==============================================================================

/// تهيئة المخازن / Initialize buffers
void Renderer2D::InitializeBuffers() {
    // إنشاء VAO / Create VAO
    glGenVertexArrays(1, &m_vao);                // توليد VAO / Generate VAO
    glBindVertexArray(m_vao);                    // ربط VAO / Bind VAO
    
    // إنشاء VBO مع تخصيص مسبق / Create VBO with pre-allocation
    glGenBuffers(1, &m_vbo);                     // توليد VBO / Generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);        // ربط VBO / Bind VBO
    glBufferData(GL_ARRAY_BUFFER,                // تخصيص مسبق / Pre-allocate
                 MAX_BATCH_VERTICES * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);
    
    // إنشاء EBO مع تخصيص مسبق / Create EBO with pre-allocation
    glGenBuffers(1, &m_ebo);                     // توليد EBO / Generate EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);  // ربط EBO / Bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,        // تخصيص مسبق / Pre-allocate
                 MAX_BATCH_INDICES * sizeof(UInt32), nullptr, GL_DYNAMIC_DRAW);
    
    // ضبط خصائص الرؤوس / Setup vertex attributes
    // Position (x, y) - 2 floats / الموقع
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);
    
    // TexCoord (u, v) - 2 floats / إحداثيات texture
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(2 * sizeof(Float32)));
    
    // Color (r, g, b, a) - 4 FLOATS (ليس بايتات!) / 4 FLOATS (not bytes!)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(4 * sizeof(Float32)));
    
    // فك الربط / Unbind
    glBindVertexArray(0);                        // فك VAO / Unbind VAO
}

/// تدمير المخازن / Destroy buffers
void Renderer2D::DestroyBuffers() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);  // حذف VAO / Delete VAO
    if (m_vbo) glDeleteBuffers(1, &m_vbo);       // حذف VBO / Delete VBO
    if (m_ebo) glDeleteBuffers(1, &m_ebo);       // حذف EBO / Delete EBO
}

/// دفع الدفعة / Flush batch
void Renderer2D::FlushBatch(DrawMode mode) {
    if (m_vertices.empty()) return;              // لا شيء للرسم / Nothing to draw
    
    // ربط VAO / Bind VAO
    glBindVertexArray(m_vao);
    
    // رفع بيانات الرؤوس (المخزن مُخصص مسبقاً) / Upload vertex data (buffer pre-allocated)
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    m_vertices.size() * sizeof(Vertex2D), m_vertices.data());
    
    // استخدام الشيدر الموحد دائماً (يدعم اللون لكل رأس + الـ textures)
    // Always use unified shader (per-vertex color + textures)
    ShaderProgram& shader = m_textureShader;
    
    // استخدام الـ shader / Use shader
    shader.Use();
    shader.SetMatrix4("uProjection", m_projectionMatrix);
    shader.SetMatrix4("uTransform", m_transformMatrix);
    
    // إعداد الـ texture إذا كانت مفعّلة / Setup texture if enabled
    if (m_textureEnabled && m_currentTexture) {
        shader.SetInt("uUseTexture", 1);         // تفعيل الـ texture / Enable texture
        m_currentTexture->Bind(0);               // ربط الـ texture / Bind texture
        shader.SetInt("uTexture", 0);            // ضبط وحدة الـ texture / Set texture unit
    } else {
        shader.SetInt("uUseTexture", 0);         // تعطيل الـ texture / Disable texture
    }
    
    // الرسم / Draw
    if (m_indices.empty()) {                     // رسم مباشر / Direct draw
        GLenum glMode = (mode == DrawMode::Lines) ? GL_LINES : GL_TRIANGLES;
        glDrawArrays(glMode, 0, static_cast<GLsizei>(m_vertices.size()));
        
        m_stats.drawCalls++;                     // زيادة العدد / Increment count
        m_stats.vertices += static_cast<UInt32>(m_vertices.size());
    } else {                                     // رسم بالفهارس / Indexed draw
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                        m_indices.size() * sizeof(UInt32), m_indices.data());
        
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()),
                       GL_UNSIGNED_INT, 0);
        
        m_stats.drawCalls++;
        m_stats.vertices += static_cast<UInt32>(m_vertices.size());
        m_stats.triangles += static_cast<UInt32>(m_indices.size() / 3);
    }
    
    // فك ربط الـ texture / Unbind texture
    if (m_textureEnabled && m_currentTexture) {
        m_currentTexture->Unbind();
    }
    
    // تنظيف بيانات الرؤوس والفهارس فقط (الحالة تُحفظ للتجميع)
    // Only clear vertex/index data (state preserved for batching)
    m_vertices.clear();
    m_indices.clear();
}

/// تحضير الدفعة / Prepare batch
void Renderer2D::BeginBatch(DrawMode mode, bool textured,
                            const sad::graphics::TextureRef& texture) {
    bool needsFlush = false;
    
    if (!m_vertices.empty()) {
        // تغيير نمط الرسم / Draw mode change
        if (m_currentBatchMode != mode) needsFlush = true;
        // تغيير حالة الـ texture / Texture state change
        if (m_textureEnabled != textured) needsFlush = true;
        // تغيير الـ texture / Texture change
        if (textured && texture && m_currentTexture != texture) needsFlush = true;
        // فيض مخزن الرؤوس / Vertex buffer overflow
        if (m_vertices.size() >= MAX_BATCH_VERTICES - 512) needsFlush = true;
        // فيض مخزن الفهارس / Index buffer overflow
        if (!m_indices.empty() && m_indices.size() >= MAX_BATCH_INDICES - 1536) needsFlush = true;
    }
    
    if (needsFlush) {
        FlushBatch(m_currentBatchMode);          // دفع الدفعة الحالية / Flush current batch
    }
    
    // تحديث الحالة / Update state
    m_currentBatchMode = mode;
    m_textureEnabled = textured;
    if (textured && texture) {
        m_currentTexture = texture;              // تحديث الـ texture / Update texture
    } else if (!textured) {
        m_currentTexture = nullptr;              // مسح الـ texture / Clear texture
    }
}

/// إنشاء مصفوفة إسقاط / Create orthographic matrix
void Renderer2D::CreateOrthographicMatrix(Float32 left, Float32 right,
                                          Float32 bottom, Float32 top) {
    // مصفوفة إسقاط متعامد / Orthographic projection matrix
    std::memset(m_projectionMatrix, 0, sizeof(m_projectionMatrix));
    
    m_projectionMatrix[0] = 2.0f / (right - left);
    m_projectionMatrix[5] = 2.0f / (top - bottom);
    m_projectionMatrix[10] = -1.0f;
    m_projectionMatrix[12] = -(right + left) / (right - left);
    m_projectionMatrix[13] = -(top + bottom) / (top - bottom);
    m_projectionMatrix[15] = 1.0f;
}

/// إنشاء مصفوفة هوية / Create identity matrix
void Renderer2D::CreateIdentityMatrix(Float32* matrix) {
    std::memset(matrix, 0, 16 * sizeof(Float32));  // تصفير / Zero
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;  // القطر / Diagonal
}

/// ضرب مصفوفتين / Multiply matrices
void Renderer2D::MultiplyMatrices(const Float32* a, const Float32* b, Float32* result) {
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result[row * 4 + col] = 
                a[row * 4 + 0] * b[0 * 4 + col] +
                a[row * 4 + 1] * b[1 * 4 + col] +
                a[row * 4 + 2] * b[2 * 4 + col] +
                a[row * 4 + 3] * b[3 * 4 + col];
        }
    }
}

/// إضافة رأس / Add vertex
void Renderer2D::AddVertex(Float32 x, Float32 y, const Color& color) {
    Vertex2D vertex;
    vertex.x = x;
    vertex.y = y;
    vertex.u = 0.0f;
    vertex.v = 0.0f;
    vertex.color = color;
    m_vertices.push_back(vertex);
}

/// إضافة رأس مع إحداثيات texture / Add vertex with texture coordinates
void Renderer2D::AddVertex(Float32 x, Float32 y, Float32 u, Float32 v, const Color& color) {
    Vertex2D vertex;
    vertex.x = x;
    vertex.y = y;
    vertex.u = u;
    vertex.v = v;
    vertex.color = color;
    m_vertices.push_back(vertex);
}

/// إضافة رؤوس خط / Add line vertices
void Renderer2D::AddLineVertices(Float32 x1, Float32 y1, Float32 x2, Float32 y2,
                                 const Color& color, Float32 thickness) {
    // رسم خط بسيط / Simple line drawing
    AddVertex(x1, y1, color);
    AddVertex(x2, y2, color);
}

// ==============================================================================
// رسم الصور والـ Textures / Drawing Images and Textures
// ==============================================================================

/// رسم texture / Draw texture
void Renderer2D::DrawTexture(const sad::graphics::TextureRef& texture,
                             Float32 x, Float32 y, Float32 width, Float32 height,
                             const Color& tint) {
    // التحقق من صحة الـ texture / Validate texture
    if (!texture || !texture->IsValid()) {
        return;
    }
    
    // DrawTexturedQuad يتولى إدارة الدفعة / DrawTexturedQuad handles batch management
    DrawTexturedQuad(texture, x, y, width, height, 0.0f, 0.0f, 1.0f, 1.0f, tint);
}

/// رسم texture مع دوران / Draw texture with rotation
void Renderer2D::DrawTextureRotated(const sad::graphics::TextureRef& texture,
                                    Float32 x, Float32 y, Float32 width, Float32 height,
                                    Float32 rotation, Float32 originX, Float32 originY,
                                    const Color& tint) {
    // التحقق من صحة الـ texture / Validate texture
    if (!texture || !texture->IsValid()) {
        return;
    }
    
    // حفظ التحويل الحالي / Save current transform
    PushTransform();
    
    // تطبيق التحويلات / Apply transformations
    Float32 centerX = x + width * originX;
    Float32 centerY = y + height * originY;
    
    Translate(centerX, centerY);
    Rotate(rotation);
    Translate(-width * originX, -height * originY);
    
    // رسم الـ texture / Draw texture
    DrawTexture(texture, 0, 0, width, height, tint);
    
    // استعادة التحويل / Restore transform
    PopTransform();
}

/// رسم جزء من texture / Draw texture region
void Renderer2D::DrawTextureRegion(const sad::graphics::TextureRef& texture,
                                   Float32 destX, Float32 destY, Float32 destWidth, Float32 destHeight,
                                   Float32 srcX, Float32 srcY, Float32 srcWidth, Float32 srcHeight,
                                   const Color& tint) {
    // التحقق من صحة الـ texture / Validate texture
    if (!texture || !texture->IsValid()) {
        return;
    }
    
    // حساب إحداثيات UV / Calculate UV coordinates
    Float32 texWidth = static_cast<Float32>(texture->GetWidth());
    Float32 texHeight = static_cast<Float32>(texture->GetHeight());
    
    Float32 u0 = srcX / texWidth;
    Float32 v0 = srcY / texHeight;
    Float32 u1 = (srcX + srcWidth) / texWidth;
    Float32 v1 = (srcY + srcHeight) / texHeight;
    
    // رسم quad مع إحداثيات UV مخصصة / Draw quad with custom UV
    DrawTexturedQuad(texture, destX, destY, destWidth, destHeight, u0, v0, u1, v1, tint);
}

/// رسم quad مع texture / Draw quad with texture
void Renderer2D::DrawTexturedQuad(const sad::graphics::TextureRef& texture,
                                  Float32 x, Float32 y, Float32 width, Float32 height,
                                  Float32 u0, Float32 v0, Float32 u1, Float32 v1,
                                  const Color& tint) {
    // التحقق من صحة الـ texture / Validate texture
    if (!texture || !texture->IsValid()) {
        return;
    }
    
    // تحضير دفعة الـ texture / Prepare texture batch
    BeginBatch(DrawMode::Triangles, true, texture);
    
    // حساب إحداثيات الزوايا / Calculate corner coordinates
    Float32 x0 = x;
    Float32 y0 = y;
    Float32 x1 = x + width;
    Float32 y1 = y + height;
    
    // إضافة 4 رؤوس للـ quad / Add 4 vertices for quad
    UInt32 startIndex = static_cast<UInt32>(m_vertices.size());
    
    AddVertex(x0, y0, u0, v0, tint);  // أعلى يسار / Top-left
    AddVertex(x1, y0, u1, v0, tint);  // أعلى يمين / Top-right
    AddVertex(x1, y1, u1, v1, tint);  // أسفل يمين / Bottom-right
    AddVertex(x0, y1, u0, v1, tint);  // أسفل يسار / Bottom-left
    
    // إضافة الفهارس لمثلثين / Add indices for two triangles
    m_indices.push_back(startIndex + 0);
    m_indices.push_back(startIndex + 1);
    m_indices.push_back(startIndex + 2);
    
    m_indices.push_back(startIndex + 0);
    m_indices.push_back(startIndex + 2);
    m_indices.push_back(startIndex + 3);
}

// ==============================================================================
// دوال مساعدة / Helper Functions
// ==============================================================================

/// إنشاء shader للـ textures / Create texture shader
void Renderer2D::CreateTextureShader() {
    // Vertex shader مع دعم textures / Vertex shader with texture support
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        layout (location = 2) in vec4 aColor;
        
        uniform mat4 uProjection;
        uniform mat4 uTransform;
        
        out vec2 TexCoord;
        out vec4 Color;
        
        void main() {
            gl_Position = uProjection * uTransform * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
            Color = aColor;
        }
    )";
    
    // Fragment shader مع دعم textures / Fragment shader with texture support
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        in vec4 Color;
        
        uniform sampler2D uTexture;
        uniform bool uUseTexture;
        
        out vec4 FragColor;
        
        void main() {
            if (uUseTexture) {
                FragColor = texture(uTexture, TexCoord) * Color;
            } else {
                FragColor = Color;
            }
        }
    )";
    
    // ترجمة وربط الـ shader / Compile and link shader
    m_textureShader.Compile(vertexShaderSource, fragmentShaderSource);
}

/// تفعيل/تعطيل texture mode / Enable/disable texture mode
void Renderer2D::SetTextureMode(bool enabled) {
    if (m_textureEnabled != enabled) {
        // دفع الدفعة الحالية / Flush current batch
        if (!m_vertices.empty()) {
            FlushBatch(DrawMode::Triangles);
        }
        
        m_textureEnabled = enabled;
    }
}

// ==============================================================================
// رسم النصوص / Text Drawing
// ==============================================================================

/// رسم نص / Draw text
void Renderer2D::DrawText(const std::string& text,
                          const sad::graphics::FontRef& font,
                          Float32 x, Float32 y,
                          const Color& color) {
    // التحقق من صحة الخط / Validate font
    if (!font || !font->IsValid()) {
        std::cerr << "(AR) خط غير صالح / (EN) Invalid font" << std::endl;
        return;  // الخروج / Exit
    }
    
    // تفعيل texture mode / Enable texture mode
    SetTextureMode(true);
    
    // الحصول على texture atlas / Get atlas texture
    const sad::graphics::TextureRef& atlas = font->GetAtlas();
    if (!atlas || !atlas->IsValid()) {
        std::cerr << "(AR) atlas غير صالح / (EN) Invalid atlas" << std::endl;
        return;  // الخروج / Exit
    }
    
    // ربط الـ texture / Bind texture
    if (m_currentTexture != atlas) {
        if (!m_vertices.empty()) {
            FlushBatch(DrawMode::Triangles);  // دفع الدفعة الحالية / Flush current batch
        }
        m_currentTexture = atlas;  // تحديث الـ texture / Update texture
    }
    
    // موقع القلم الحالي / Current pen position
    Float32 penX = x;
    Float32 penY = y + font->GetBaseline();  // إضافة baseline / Add baseline
    
    u32 prevCodepoint = 0;  // الحرف السابق / Previous character
    
    // المرور على كل حرف / Iterate through characters
    for (size_t i = 0; i < text.length(); ++i) {
        u32 codepoint = static_cast<u32>(static_cast<u8>(text[i]));  // الحرف الحالي / Current char
        
        // تخطي الأحرف الخاصة / Skip special characters
        if (codepoint == '\n') {  // سطر جديد / Newline
            penX = x;  // العودة لبداية السطر / Return to line start
            penY += font->GetLineHeight();  // الانتقال للسطر التالي / Move to next line
            prevCodepoint = 0;  // إعادة تعيين السابق / Reset previous
            continue;  // المتابعة / Continue
        }
        
        if (codepoint == '\r') {  // تجاهل carriage return / Ignore carriage return
            continue;  // المتابعة / Continue
        }
        
        // الحصول على معلومات الحرف / Get glyph info
        const sad::graphics::GlyphInfo* glyph = font->GetGlyph(codepoint);
        
        if (!glyph) {  // إذا لم يُوجد الحرف / If glyph not found
            // محاولة استخدام مسافة / Try using space
            glyph = font->GetGlyph(' ');
            if (!glyph) {  // إذا لم توجد المسافة أيضاً / If space also missing
                continue;  // تخطي الحرف / Skip character
            }
        }
        
        // إضافة kerning إذا كان متاحاً / Add kerning if available
        if (prevCodepoint != 0) {
            penX += font->GetKerning(prevCodepoint, codepoint);  // إضافة kerning / Add kerning
        }
        
        // حساب موقع ورسم الحرف / Calculate position and draw glyph
        Float32 glyphX = penX + glyph->bearingX;  // موقع X مع bearing / X with bearing
        Float32 glyphY = penY - glyph->bearingY;  // موقع Y مع bearing / Y with bearing
        
        // رسم quad للحرف / Draw quad for glyph
        if (glyph->width > 0 && glyph->height > 0) {  // إذا كان له حجم / If has size
            DrawTexturedQuad(
                atlas,  // texture atlas
                glyphX, glyphY,  // الموقع / Position
                glyph->width, glyph->height,  // الأبعاد / Dimensions
                glyph->u0, glyph->v0, glyph->u1, glyph->v1,  // UV coordinates
                color  // اللون / Color
            );
        }
        
        // تحريك القلم للحرف التالي / Advance pen for next character
        penX += glyph->advanceX;  // التقدم بمقدار advanceX / Advance by advanceX
        prevCodepoint = codepoint;  // تحديث السابق / Update previous
    }
}

/// رسم نص في المنتصف / Draw centered text
void Renderer2D::DrawTextCentered(const std::string& text,
                                  const sad::graphics::FontRef& font,
                                  Float32 centerX, Float32 centerY,
                                  const Color& color) {
    // التحقق من صحة الخط / Validate font
    if (!font || !font->IsValid()) {
        return;  // الخروج / Exit
    }
    
    // قياس النص / Measure text
    float width, height;
    font->MeasureText(text, width, height);  // الحصول على الأبعاد / Get dimensions
    
    // حساب الموقع للنص المتمركز / Calculate position for centered text
    Float32 x = centerX - width * 0.5f;  // نصف العرض لليسار / Half width to left
    Float32 y = centerY - height * 0.5f;  // نصف الارتفاع للأعلى / Half height up
    
    // رسم النص / Draw text
    DrawText(text, font, x, y, color);  // استدعاء DrawText الأساسي / Call basic DrawText
}

/// رسم نص مع محاذاة / Draw aligned text
void Renderer2D::DrawTextAligned(const std::string& text,
                                 const sad::graphics::FontRef& font,
                                 Float32 x, Float32 y,
                                 Float32 alignX, Float32 alignY,
                                 const Color& color) {
    // التحقق من صحة الخط / Validate font
    if (!font || !font->IsValid()) {
        return;  // الخروج / Exit
    }
    
    // قياس النص / Measure text
    float width, height;
    font->MeasureText(text, width, height);  // الحصول على الأبعاد / Get dimensions
    
    // تطبيق المحاذاة / Apply alignment
    Float32 alignedX = x - width * alignX;  // محاذاة X / X alignment
    Float32 alignedY = y - height * alignY;  // محاذاة Y / Y alignment
    
    // رسم النص / Draw text
    DrawText(text, font, alignedX, alignedY, color);  // استدعاء DrawText الأساسي / Call basic DrawText
}

// ==============================================================================
// رسم النصوص العربية / Arabic Text Drawing
// ==============================================================================

/// رسم نص عربي مع تشكيل تلقائي و RTL / Draw Arabic text with shaping and RTL
void Renderer2D::DrawTextArabic(const std::string& text,
                                const sad::graphics::FontRef& font,
                                Float32 x, Float32 y,
                                const Color& color) {
    if (!font || !font->IsValid()) return;
    
    // فك ترميز UTF-8 / Decode UTF-8
    auto codepoints = sad::graphics::ArabicText::DecodeUTF8(text);
    if (codepoints.empty()) return;
    
    // تشكيل الحروف العربية / Shape Arabic characters
    auto shaped = sad::graphics::ArabicText::ShapeArabic(codepoints);
    
    // عكس الترتيب للعرض RTL / Reverse for RTL display
    std::reverse(shaped.begin(), shaped.end());
    
    // الحصول على atlas / Get atlas
    const sad::graphics::TextureRef& atlas = font->GetAtlas();
    if (!atlas || !atlas->IsValid()) return;
    
    // حساب العرض الكلي لتحديد نقطة البداية / Calculate total width for start position
    Float32 penX = x;
    Float32 penY = y + font->GetBaseline();
    u32 prevCodepoint = 0;
    
    for (u32 cp : shaped) {
        if (sad::graphics::ArabicText::IsDiacritic(cp)) {
            // التشكيل يُرسم فوق/تحت الحرف السابق / Diacritics drawn over previous char
            const sad::graphics::GlyphInfo* diacGlyph = font->GetGlyph(cp);
            if (diacGlyph && diacGlyph->width > 0 && diacGlyph->height > 0) {
                // رسم التشكيل في موقع الحرف السابق / Draw diacritic at previous char position
                Float32 diacX = penX - diacGlyph->advanceX + diacGlyph->bearingX;
                Float32 diacY = penY - diacGlyph->bearingY;
                DrawTexturedQuad(atlas, diacX, diacY,
                    diacGlyph->width, diacGlyph->height,
                    diacGlyph->u0, diacGlyph->v0, diacGlyph->u1, diacGlyph->v1,
                    color);
            }
            continue;
        }
        
        if (cp == '\n') {
            penX = x;
            penY += font->GetLineHeight();
            prevCodepoint = 0;
            continue;
        }
        
        const sad::graphics::GlyphInfo* glyph = font->GetGlyph(cp);
        if (!glyph) {
            glyph = font->GetGlyph(' ');
            if (!glyph) continue;
        }
        
        if (prevCodepoint != 0) {
            penX += font->GetKerning(prevCodepoint, cp);
        }
        
        Float32 glyphX = penX + glyph->bearingX;
        Float32 glyphY = penY - glyph->bearingY;
        
        if (glyph->width > 0 && glyph->height > 0) {
            DrawTexturedQuad(atlas, glyphX, glyphY,
                glyph->width, glyph->height,
                glyph->u0, glyph->v0, glyph->u1, glyph->v1,
                color);
        }
        
        penX += glyph->advanceX;
        prevCodepoint = cp;
    }
}

/// رسم نص تلقائي الاتجاه / Draw auto-direction text
void Renderer2D::DrawTextAuto(const std::string& text,
                              const sad::graphics::FontRef& font,
                              Float32 x, Float32 y,
                              const Color& color) {
    if (!font || !font->IsValid()) return;
    
    auto codepoints = sad::graphics::ArabicText::DecodeUTF8(text);
    auto direction = sad::graphics::ArabicText::DetectDirection(codepoints);
    
    if (direction == sad::graphics::TextDirection::RTL) {
        DrawTextArabic(text, font, x, y, color);
    } else {
        // نص LTR - استخدام DrawText العادي مع دعم UTF-8
        // LTR text - use regular DrawText with UTF-8 support
        DrawTextUTF8(codepoints, font, x, y, color);
    }
}

/// رسم نص من رموز Unicode / Draw text from Unicode codepoints
void Renderer2D::DrawTextUTF8(const std::vector<sad::graphics::u32>& codepoints,
                              const sad::graphics::FontRef& font,
                              Float32 x, Float32 y,
                              const Color& color) {
    if (!font || !font->IsValid()) return;
    
    // الحصول على atlas / Get atlas
    const sad::graphics::TextureRef& atlas = font->GetAtlas();
    if (!atlas || !atlas->IsValid()) return;
    
    Float32 penX = x;
    Float32 penY = y + font->GetBaseline();
    u32 prevCodepoint = 0;
    
    for (u32 cp : codepoints) {
        if (cp == '\n') {
            penX = x;
            penY += font->GetLineHeight();
            prevCodepoint = 0;
            continue;
        }
        if (cp == '\r') continue;
        if (sad::graphics::ArabicText::IsDiacritic(cp)) continue;
        
        const sad::graphics::GlyphInfo* glyph = font->GetGlyph(cp);
        if (!glyph) {
            glyph = font->GetGlyph(' ');
            if (!glyph) continue;
        }
        
        if (prevCodepoint != 0) penX += font->GetKerning(prevCodepoint, cp);
        
        Float32 glyphX = penX + glyph->bearingX;
        Float32 glyphY = penY - glyph->bearingY;
        
        if (glyph->width > 0 && glyph->height > 0) {
            DrawTexturedQuad(atlas, glyphX, glyphY,
                glyph->width, glyph->height,
                glyph->u0, glyph->v0, glyph->u1, glyph->v1,
                color);
        }
        
        penX += glyph->advanceX;
        prevCodepoint = cp;
    }
}

} // namespace SadGraphics

