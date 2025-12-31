// ==============================================================================
// renderer2d.h - محرك الرسم ثنائي الأبعاد / 2D Rendering Engine
// ==============================================================================
// الوصف: محرك رسم عالي الأداء للأشكال والنصوص والصور
// Description: High-performance rendering engine for shapes, text, and images
// ==============================================================================

#ifndef SAD_GRAPHICS_RENDERING_RENDERER2D_H
#define SAD_GRAPHICS_RENDERING_RENDERER2D_H

#include "../core/types.h"      // الأنواع الأساسية / Basic types
#include "context.h"             // سياق الرسم / Render context
#include "shader.h"              // برامج التظليل / Shader programs
#include "../texture/texture.h"  // نظام الـ textures / Texture system
#include "../text/font.h"        // نظام الخطوط / Font system
#include <memory>                // للمؤشرات الذكية / For smart pointers
#include <vector>                // للمصفوفات الديناميكية / For dynamic arrays
#include <string>                // للنصوص / For strings

namespace SadGraphics {          // مساحة الأسماء / Namespace

// ==============================================================================
// بيانات الرأس / Vertex Data
// ==============================================================================
struct Vertex2D {
    Float32 x, y;                // الموقع / Position
    Float32 u, v;                // إحداثيات texture / Texture coordinates
    Color color;                 // اللون / Color
};

// ==============================================================================
// أنواع الرسم البدائية / Primitive Drawing Types
// ==============================================================================
enum class DrawMode {
    Points,                      // نقاط / Points
    Lines,                       // خطوط / Lines
    LineStrip,                   // خط متصل / Line strip
    LineLoop,                    // خط مغلق / Line loop
    Triangles,                   // مثلثات / Triangles
    TriangleStrip,               // شريط مثلثات / Triangle strip
    TriangleFan,                 // مروحة مثلثات / Triangle fan
};

// ==============================================================================
// إحصائيات الرسم / Rendering Statistics
// ==============================================================================
struct RenderStats {
    UInt32 drawCalls;            // عدد استدعاءات الرسم / Number of draw calls
    UInt32 vertices;             // عدد الرؤوس المرسومة / Vertices drawn
    UInt32 triangles;            // عدد المثلثات / Triangles drawn
    
    /// إعادة تعيين الإحصائيات / Reset statistics
    void Reset() {
        drawCalls = 0;           // إعادة تعيين الاستدعاءات / Reset calls
        vertices = 0;            // إعادة تعيين الرؤوس / Reset vertices
        triangles = 0;           // إعادة تعيين المثلثات / Reset triangles
    }
};

// ==============================================================================
// فئة محرك الرسم ثنائي الأبعاد / 2D Renderer Class
// ==============================================================================
class Renderer2D {
public:
    // ==============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ==============================================================================
    
    /// إنشاء محرك رسم جديد / Create new renderer
    /// context: سياق الرسم / Render context
    Renderer2D(RenderContext* context);
    
    /// المدمر / Destructor
    ~Renderer2D();
    
    // منع النسخ / Prevent copying
    Renderer2D(const Renderer2D&) = delete;              // منع النسخ / Disable copy
    Renderer2D& operator=(const Renderer2D&) = delete;   // منع الإسناد / Disable assignment
    
    // ==============================================================================
    // إدارة الإطار / Frame Management
    // ==============================================================================
    
    /// بدء إطار رسم جديد / Begin new rendering frame
    void BeginFrame();
    
    /// إنهاء إطار الرسم / End rendering frame
    void EndFrame();
    
    /// مسح الشاشة / Clear screen
    /// color: لون المسح / Clear color
    void Clear(const Color& color = Color::Black);
    
    // ==============================================================================
    // رسم الأشكال الأساسية / Drawing Basic Shapes
    // ==============================================================================
    
    /// رسم خط / Draw line
    /// x1, y1: نقطة البداية / Start point
    /// x2, y2: نقطة النهاية / End point
    /// color: لون الخط / Line color
    /// thickness: سمك الخط / Line thickness
    void DrawLine(Float32 x1, Float32 y1, Float32 x2, Float32 y2, 
                  const Color& color = Color::White, Float32 thickness = 1.0f);
    
    /// رسم مستطيل / Draw rectangle
    /// x, y: الزاوية العلوية اليسرى / Top-left corner
    /// width, height: الأبعاد / Dimensions
    /// color: اللون / Color
    /// filled: ممتلئ أم فارغ / Filled or outline
    void DrawRect(Float32 x, Float32 y, Float32 width, Float32 height,
                  const Color& color = Color::White, bool filled = true);
    
    /// رسم مستطيل مع حدود / Draw rectangle with outline
    /// x, y: الموقع / Position
    /// width, height: الأبعاد / Dimensions
    /// fillColor: لون التعبئة / Fill color
    /// outlineColor: لون الحدود / Outline color
    /// outlineThickness: سمك الحدود / Outline thickness
    void DrawRectOutline(Float32 x, Float32 y, Float32 width, Float32 height,
                         const Color& fillColor, const Color& outlineColor, 
                         Float32 outlineThickness = 1.0f);
    
    /// رسم دائرة / Draw circle
    /// x, y: المركز / Center
    /// radius: نصف القطر / Radius
    /// color: اللون / Color
    /// filled: ممتلئة أم فارغة / Filled or outline
    /// segments: عدد الأضلاع (دقة الدائرة) / Number of segments (circle quality)
    void DrawCircle(Float32 x, Float32 y, Float32 radius,
                    const Color& color = Color::White, bool filled = true,
                    Int32 segments = 32);
    
    /// رسم قطع ناقص / Draw ellipse
    /// x, y: المركز / Center
    /// radiusX, radiusY: نصفي القطر / Radii
    /// color: اللون / Color
    /// filled: ممتلئ أم فارغ / Filled or outline
    /// segments: عدد الأضلاع / Number of segments
    void DrawEllipse(Float32 x, Float32 y, Float32 radiusX, Float32 radiusY,
                     const Color& color = Color::White, bool filled = true,
                     Int32 segments = 32);
    
    /// رسم مضلع / Draw polygon
    /// points: نقاط المضلع / Polygon points
    /// numPoints: عدد النقاط / Number of points
    /// color: اللون / Color
    /// filled: ممتلئ أم فارغ / Filled or outline
    void DrawPolygon(const Point2D* points, Int32 numPoints,
                     const Color& color = Color::White, bool filled = true);
    
    /// رسم مثلث / Draw triangle
    /// x1, y1, x2, y2, x3, y3: نقاط المثلث / Triangle points
    /// color: اللون / Color
    /// filled: ممتلئ أم فارغ / Filled or outline
    void DrawTriangle(Float32 x1, Float32 y1, Float32 x2, Float32 y2,
                      Float32 x3, Float32 y3, const Color& color = Color::White,
                      bool filled = true);
    
    /// رسم قوس / Draw arc
    /// x, y: المركز / Center
    /// radius: نصف القطر / Radius
    /// startAngle: زاوية البداية (بالراديان) / Start angle (radians)
    /// endAngle: زاوية النهاية (بالراديان) / End angle (radians)
    /// color: اللون / Color
    /// segments: عدد الأضلاع / Number of segments
    void DrawArc(Float32 x, Float32 y, Float32 radius,
                 Float32 startAngle, Float32 endAngle,
                 const Color& color = Color::White, Int32 segments = 32);
    
    // ==============================================================================
    // رسم النصوص / Text Drawing
    // ==============================================================================
    
    /// رسم نص / Draw text
    /// text: النص المراد رسمه / Text to draw
    /// font: الخط المستخدم / Font to use
    /// x, y: الموقع / Position
    /// color: لون النص / Text color
    void DrawText(const std::string& text, 
                  const sad::graphics::FontRef& font,
                  Float32 x, Float32 y,
                  const Color& color = Color::White);
    
    /// رسم نص في المنتصف / Draw centered text
    /// text: النص المراد رسمه / Text to draw
    /// font: الخط المستخدم / Font to use
    /// centerX, centerY: نقطة المركز / Center point
    /// color: لون النص / Text color
    void DrawTextCentered(const std::string& text,
                          const sad::graphics::FontRef& font,
                          Float32 centerX, Float32 centerY,
                          const Color& color = Color::White);
    
    /// رسم نص مع محاذاة / Draw aligned text
    /// text: النص المراد رسمه / Text to draw
    /// font: الخط المستخدم / Font to use
    /// x, y: الموقع / Position
    /// alignX, alignY: المحاذاة (0=يسار/أعلى, 0.5=وسط, 1=يمين/أسفل) / Alignment
    /// color: لون النص / Text color
    void DrawTextAligned(const std::string& text,
                         const sad::graphics::FontRef& font,
                         Float32 x, Float32 y,
                         Float32 alignX, Float32 alignY,
                         const Color& color = Color::White);
    
    // ==============================================================================
    // رسم الصور والـ Textures / Drawing Images and Textures
    // ==============================================================================
    
    /// رسم texture / Draw texture
    /// texture: الـ texture المراد رسمه / Texture to draw
    /// x, y: الموقع / Position
    /// width, height: الأبعاد / Dimensions
    /// tint: لون التلوين (أبيض = بدون تلوين) / Tint color (white = no tint)
    void DrawTexture(const sad::graphics::TextureRef& texture,
                     Float32 x, Float32 y, Float32 width, Float32 height,
                     const Color& tint = Color::White);
    
    /// رسم texture مع دوران / Draw texture with rotation
    /// texture: الـ texture المراد رسمه / Texture to draw
    /// x, y: الموقع / Position
    /// width, height: الأبعاد / Dimensions
    /// rotation: زاوية الدوران (بالراديان) / Rotation angle (radians)
    /// originX, originY: نقطة الدوران (0-1) / Rotation origin (0-1)
    /// tint: لون التلوين / Tint color
    void DrawTextureRotated(const sad::graphics::TextureRef& texture,
                            Float32 x, Float32 y, Float32 width, Float32 height,
                            Float32 rotation, Float32 originX = 0.5f, Float32 originY = 0.5f,
                            const Color& tint = Color::White);
    
    /// رسم جزء من texture / Draw texture region
    /// texture: الـ texture المراد رسمه / Texture to draw
    /// destX, destY: موقع الوجهة / Destination position
    /// destWidth, destHeight: أبعاد الوجهة / Destination dimensions
    /// srcX, srcY: موقع المصدر في الـ texture / Source position in texture
    /// srcWidth, srcHeight: أبعاد المصدر / Source dimensions
    /// tint: لون التلوين / Tint color
    void DrawTextureRegion(const sad::graphics::TextureRef& texture,
                           Float32 destX, Float32 destY, Float32 destWidth, Float32 destHeight,
                           Float32 srcX, Float32 srcY, Float32 srcWidth, Float32 srcHeight,
                           const Color& tint = Color::White);
    
    /// رسم quad مع texture وإحداثيات UV مخصصة / Draw quad with texture and custom UV
    /// texture: الـ texture المراد رسمه / Texture to draw
    /// x, y: الموقع / Position
    /// width, height: الأبعاد / Dimensions
    /// u0, v0, u1, v1: إحداثيات UV (0-1) / UV coordinates (0-1)
    /// tint: لون التلوين / Tint color
    void DrawTexturedQuad(const sad::graphics::TextureRef& texture,
                          Float32 x, Float32 y, Float32 width, Float32 height,
                          Float32 u0, Float32 v0, Float32 u1, Float32 v1,
                          const Color& tint = Color::White);
    
    // ==============================================================================
    // التحويلات / Transformations
    // ==============================================================================
    
    /// حفظ حالة التحويل الحالية / Save current transformation state
    void PushTransform();
    
    /// استرجاع حالة التحويل السابقة / Restore previous transformation state
    void PopTransform();
    
    /// تحريك (ترجمة) / Translate
    /// x, y: مقدار التحريك / Translation amount
    void Translate(Float32 x, Float32 y);
    
    /// تدوير / Rotate
    /// angle: الزاوية بالراديان / Angle in radians
    void Rotate(Float32 angle);
    
    /// قياس (تكبير/تصغير) / Scale
    /// x, y: عوامل القياس / Scale factors
    void Scale(Float32 x, Float32 y);
    
    /// إعادة تعيين التحويل / Reset transformation
    void ResetTransform();
    
    // ==============================================================================
    // إعدادات الرسم / Drawing Settings
    // ==============================================================================
    
    /// ضبط منطقة العرض / Set viewport
    /// x, y: الموقع / Position
    /// width, height: الأبعاد / Dimensions
    void SetViewport(Int32 x, Int32 y, Int32 width, Int32 height);
    
    /// ضبط مصفوفة الإسقاط / Set projection matrix
    /// left, right, bottom, top: حدود الإسقاط / Projection bounds
    void SetOrthographicProjection(Float32 left, Float32 right, 
                                    Float32 bottom, Float32 top);
    
    /// ضبط اللون الافتراضي للرسم / Set default drawing color
    /// color: اللون الجديد / New color
    void SetDrawColor(const Color& color);
    
    /// الحصول على اللون الافتراضي / Get default color
    /// Returns: اللون الحالي / Current color
    Color GetDrawColor() const { return m_drawColor; }
    
    /// ضبط سمك الخط الافتراضي / Set default line thickness
    /// thickness: السمك / Thickness
    void SetLineThickness(Float32 thickness);
    
    /// الحصول على سمك الخط / Get line thickness
    /// Returns: السمك الحالي / Current thickness
    Float32 GetLineThickness() const { return m_lineThickness; }
    
    // ==============================================================================
    // إحصائيات وتشخيص / Statistics and Diagnostics
    // ==============================================================================
    
    /// الحصول على إحصائيات الإطار الحالي / Get current frame statistics
    /// Returns: الإحصائيات / Statistics
    const RenderStats& GetStats() const { return m_stats; }
    
    /// إعادة تعيين الإحصائيات / Reset statistics
    void ResetStats() { m_stats.Reset(); }
    
    /// طباعة إحصائيات الأداء / Print performance statistics
    void PrintStats() const;

private:
    // ==============================================================================
    // البيانات الخاصة / Private Data
    // ==============================================================================
    
    RenderContext* m_context;    // سياق الرسم / Render context
    ShaderProgram m_defaultShader; // برنامج shader الافتراضي / Default shader program
    ShaderProgram m_textureShader; // برنامج shader للـ textures / Texture shader program
    
    // مخازن الرؤوس / Vertex buffers
    std::vector<Vertex2D> m_vertices;  // مخزن الرؤوس / Vertex buffer
    std::vector<UInt32> m_indices;     // مخزن الفهارس / Index buffer
    
    // معرفات OpenGL / OpenGL IDs
    UInt32 m_vao;                // Vertex Array Object
    UInt32 m_vbo;                // Vertex Buffer Object
    UInt32 m_ebo;                // Element Buffer Object
    
    // إعدادات الرسم / Drawing settings
    Color m_drawColor;           // اللون الافتراضي / Default color
    Float32 m_lineThickness;     // سمك الخط الافتراضي / Default line thickness
    
    // الـ Texture الحالي / Current texture
    sad::graphics::TextureRef m_currentTexture;  // الـ texture المُربوط حالياً / Currently bound texture
    bool m_textureEnabled;       // تفعيل الـ textures / Textures enabled
    
    // المصفوفات / Matrices
    Float32 m_projectionMatrix[16];   // مصفوفة الإسقاط / Projection matrix
    Float32 m_transformMatrix[16];    // مصفوفة التحويل / Transform matrix
    std::vector<Float32> m_transformStack;  // كومة التحويلات / Transform stack
    
    // الإحصائيات / Statistics
    RenderStats m_stats;         // إحصائيات الإطار / Frame statistics
    
    // ==============================================================================
    // الدوال الخاصة / Private Functions
    // ==============================================================================
    
    /// تهيئة المخازن / Initialize buffers
    void InitializeBuffers();
    
    /// تدمير المخازن / Destroy buffers
    void DestroyBuffers();
    
    /// دفع الرؤوس إلى OpenGL / Flush vertices to OpenGL
    /// mode: نمط الرسم / Draw mode
    void FlushBatch(DrawMode mode);
    
    /// إنشاء مصفوفة إسقاط / Create projection matrix
    /// left, right, bottom, top: حدود الإسقاط / Projection bounds
    void CreateOrthographicMatrix(Float32 left, Float32 right, 
                                  Float32 bottom, Float32 top);
    
    /// إنشاء مصفوفة هوية / Create identity matrix
    void CreateIdentityMatrix(Float32* matrix);
    
    /// ضرب مصفوفتين / Multiply matrices
    void MultiplyMatrices(const Float32* a, const Float32* b, Float32* result);
    
    /// إضافة رأس / Add vertex
    void AddVertex(Float32 x, Float32 y, const Color& color);
    
    /// إضافة رأس مع إحداثيات texture / Add vertex with texture coordinates
    void AddVertex(Float32 x, Float32 y, Float32 u, Float32 v, const Color& color);
    
    /// إضافة رؤوس لخط / Add vertices for line
    void AddLineVertices(Float32 x1, Float32 y1, Float32 x2, Float32 y2,
                         const Color& color, Float32 thickness);
    
    /// إنشاء shader للـ textures / Create texture shader
    void CreateTextureShader();
    
    /// تفعيل/تعطيل texture mode / Enable/disable texture mode
    void SetTextureMode(bool enabled);
};

} // namespace SadGraphics

#endif // SAD_GRAPHICS_RENDERING_RENDERER2D_H
