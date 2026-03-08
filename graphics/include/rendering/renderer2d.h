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
    // دوال مساعدة للرسم الممتلئ / Convenience filled drawing functions
    // ==============================================================================
    
    /// رسم مستطيل ممتلئ / Draw filled rectangle
    void DrawFilledRect(Float32 x, Float32 y, Float32 width, Float32 height,
                        const Color& color = Color::White) {
        DrawRect(x, y, width, height, color, true);
    }
    
    /// رسم دائرة ممتلئة / Draw filled circle
    void DrawFilledCircle(Float32 x, Float32 y, Float32 radius,
                          const Color& color = Color::White, Int32 segments = 32) {
        DrawCircle(x, y, radius, color, true, segments);
    }

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
    
    /// رسم نص عربي مع تشكيل تلقائي واتجاه RTL
    /// Draw Arabic text with automatic shaping and RTL direction
    /// text: النص العربي (UTF-8) / Arabic text (UTF-8)
    /// font: الخط (يجب أن يحتوي على حروف عربية) / Font (must contain Arabic glyphs)
    /// x, y: الموقع (الزاوية العلوية اليمنى للنص العربي) / Position (top-right for Arabic)
    /// color: لون النص / Text color
    void DrawTextArabic(const std::string& text,
                        const sad::graphics::FontRef& font,
                        Float32 x, Float32 y,
                        const Color& color = Color::White);
    
    /// رسم نص تلقائي الاتجاه (يكتشف عربي/إنجليزي)
    /// Draw auto-direction text (detects Arabic/English)
    /// text: النص (UTF-8) / Text (UTF-8)
    /// font: الخط / Font
    /// x, y: الموقع / Position
    /// color: لون النص / Text color
    void DrawTextAuto(const std::string& text,
                      const sad::graphics::FontRef& font,
                      Float32 x, Float32 y,
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
    
    // حالة الدفعة / Batch state
    DrawMode m_currentBatchMode;  // نمط الرسم الحالي للدفعة / Current batch draw mode
    
    // ثوابت الدفعة / Batch limits
    static constexpr size_t MAX_BATCH_VERTICES = 65536;   // أقصى عدد رؤوس / Max vertices
    static constexpr size_t MAX_BATCH_INDICES  = 131072;  // أقصى عدد فهارس / Max indices
    
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
    
    /// تحضير الدفعة (فحص توافق الحالة والفيض)
    /// Prepare batch (check state compatibility and overflow)
    void BeginBatch(DrawMode mode, bool textured = false,
                    const sad::graphics::TextureRef& texture = nullptr);
    
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
    
    /// رسم نص من رموز Unicode / Draw text from Unicode codepoints (private helper)
    void DrawTextUTF8(const std::vector<sad::graphics::u32>& codepoints,
                      const sad::graphics::FontRef& font,
                      Float32 x, Float32 y,
                      const Color& color);
    
    /// حساب نقطة بزيه التكعيبية / Compute cubic Bézier point (private helper)
    void BezierPoint(Float32 t, Float32 x0, Float32 y0, Float32 x1, Float32 y1,
                     Float32 x2, Float32 y2, Float32 x3, Float32 y3,
                     Float32& outX, Float32& outY);
};

// ==============================================================================
// ==============================================================================
//
//   القسم المتقدم — أشكال متقدمة، ظلال، فلاتر، وتحكم بالبكسل
//   Advanced Section — curves, shadows, filters, pixel-level control
//
//   أُضيف في: أبريل 2026 — المهندس كامل
//   Added: April 2026 — Engineer Kamel
//
// ==============================================================================
// ==============================================================================

// ==============================================================================
// مصفوفة التحويل ثنائية الأبعاد / 2D Transform Matrix
// ==============================================================================
/// مصفوفة تحويل مخصصة 3×3 للرسم ثنائي الأبعاد
/// Custom 3x3 transform matrix for 2D rendering
/// تدعم: ترجمة، دوران، تكبير، قص، ضرب المصفوفات
struct Transform2D {
    Float32 m[3][3];  // مصفوفة 3×3 / 3x3 matrix
    
    /// إنشاء مصفوفة هوية / Create identity matrix
    Transform2D();
    
    /// مصفوفة ترجمة / Translation matrix
    static Transform2D ترجمة(Float32 tx, Float32 ty);
    static Transform2D Translate(Float32 tx, Float32 ty) { return ترجمة(tx, ty); }
    
    /// مصفوفة دوران / Rotation matrix
    /// angle: الزاوية بالراديان / Angle in radians
    static Transform2D دوران(Float32 angle);
    static Transform2D Rotation(Float32 angle) { return دوران(angle); }
    
    /// مصفوفة تكبير / Scale matrix
    static Transform2D تكبير(Float32 sx, Float32 sy);
    static Transform2D Scaling(Float32 sx, Float32 sy) { return تكبير(sx, sy); }
    
    /// مصفوفة قص / Shear matrix
    static Transform2D قص(Float32 shx, Float32 shy);
    static Transform2D Shear(Float32 shx, Float32 shy) { return قص(shx, shy); }
    
    /// ضرب مصفوفتين / Multiply matrices
    Transform2D operator*(const Transform2D& other) const;
    
    /// تحويل نقطة / Transform point
    void حوّل(Float32& x, Float32& y) const;
    void Apply(Float32& x, Float32& y) const { حوّل(x, y); }
    
    /// المعكوس / Inverse
    Transform2D معكوس() const;
    Transform2D Inverse() const { return معكوس(); }
};

// ==============================================================================
// إعدادات الظلال / Shadow Settings
// ==============================================================================
/// إعدادات ظل مسقط لأي شكل مرسوم
/// Drop shadow settings for any drawn shape
struct إعدادات_الظل {
    Float32 إزاحة_س = 3.0f;         // الإزاحة الأفقية / X offset
    Float32 إزاحة_ص = 3.0f;         // الإزاحة العمودية / Y offset
    Float32 نصف_قطر_الضبابية = 5.0f; // ضبابية الظل / Blur radius
    Color لون = {0, 0, 0, 128};     // لون الظل (شبه شفاف) / Shadow color
    bool مفعّل = false;              // هل الظل مفعّل / Is shadow enabled
};

// ==============================================================================
// إعدادات التدرج اللوني / Gradient Settings
// ==============================================================================
/// نوع التدرج / Gradient type
enum class نوع_التدرج : uint8_t {
    خطي      = 0,  // Linear gradient
    شعاعي    = 1,  // Radial gradient
    مخروطي   = 2   // Conic gradient
};

/// إعدادات التدرج اللوني — لتعبئة الأشكال بألوان متدرجة
/// Gradient settings — fill shapes with color gradients
struct إعدادات_التدرج {
    نوع_التدرج النوع = نوع_التدرج::خطي;
    Color لون_البداية = Color::White;     // Start color
    Color لون_النهاية = Color::Black;     // End color
    Float32 زاوية = 0.0f;                 // Angle (for linear)
    Float32 مركز_س = 0.5f;               // Center X (for radial/conic)
    Float32 مركز_ص = 0.5f;               // Center Y (for radial/conic)
    Float32 نصف_قطر = 1.0f;              // Radius (for radial)
};

// ==============================================================================
// مخزن البكسل — تحكم كامل بكل بكسل / Pixel Buffer — Full pixel control
// ==============================================================================
/// مخزن بكسل خام — يمنح تحكماً كاملاً بكل بكسل على الشاشة
/// Raw pixel buffer — provides full per-pixel screen control
///
/// الاستخدام / Usage:
///   مخزن_البكسل buf(800, 600);
///   buf.عيّن_بكسل(100, 200, Color::Red);  // تلوين بكسل واحد
///   buf.املأ(Color::Black);                // ملء الكل
///   buf.ارسم_إلى_الشاشة(renderer);       // رسم على الشاشة
///
class مخزن_البكسل {
public:
    /// إنشاء مخزن بكسل بأبعاد محددة
    /// Create pixel buffer with given dimensions
    مخزن_البكسل(UInt32 عرض, UInt32 ارتفاع);
    
    /// التدمير / Destructor
    ~مخزن_البكسل();
    
    // ====================================================================
    // عمليات البكسل الأساسية / Basic Pixel Operations
    // ====================================================================
    
    /// تعيين لون بكسل واحد / Set single pixel color
    /// x, y: إحداثيات البكسل / Pixel coordinates
    /// color: اللون / Color
    void عيّن_بكسل(UInt32 x, UInt32 y, const Color& color);
    void SetPixel(UInt32 x, UInt32 y, const Color& color) { عيّن_بكسل(x, y, color); }
    
    /// قراءة لون بكسل / Read pixel color
    Color اقرأ_بكسل(UInt32 x, UInt32 y) const;
    Color GetPixel(UInt32 x, UInt32 y) const { return اقرأ_بكسل(x, y); }
    
    /// ملء الكل بلون واحد / Fill entire buffer with color
    void املأ(const Color& color);
    void Fill(const Color& color) { املأ(color); }
    
    /// مسح (ملء بالأسود الشفاف) / Clear (fill with transparent black)
    void امسح();
    void Clear() { امسح(); }
    
    // ====================================================================
    // عمليات متقدمة / Advanced Operations
    // ====================================================================
    
    /// نسخ منطقة من مخزن آخر / Copy region from another buffer
    void انسخ_منطقة(const مخزن_البكسل& مصدر,
                     UInt32 مصدر_س, UInt32 مصدر_ص,
                     UInt32 عرض, UInt32 ارتفاع,
                     UInt32 هدف_س, UInt32 هدف_ص);
    
    /// مزج بكسل مع شفافية (Alpha Blending)
    /// Blend pixel with alpha blending
    void امزج_بكسل(UInt32 x, UInt32 y, const Color& color);
    
    /// الوصول المباشر للبيانات الخام / Direct access to raw data
    /// يُرجع مؤشراً لبيانات RGBA (4 بايت لكل بكسل)
    /// Returns pointer to RGBA data (4 bytes per pixel)
    uint8_t* البيانات() { return بيانات_.data(); }
    const uint8_t* البيانات() const { return بيانات_.data(); }
    
    /// رسم المخزن على الشاشة عبر texture / Draw buffer to screen via texture
    void ارسم_إلى_الشاشة(Renderer2D& renderer, Float32 x = 0, Float32 y = 0);
    
    // ====================================================================
    // الأبعاد / Dimensions
    // ====================================================================
    UInt32 العرض() const { return العرض_; }
    UInt32 الارتفاع() const { return الارتفاع_; }
    UInt32 Width() const { return العرض_; }
    UInt32 Height() const { return الارتفاع_; }
    
private:
    UInt32 العرض_;                    // Width
    UInt32 الارتفاع_;                 // Height
    std::vector<uint8_t> بيانات_;     // RGBA pixel data
    UInt32 معرّف_texture_ = 0;       // OpenGL texture ID (lazy)
    bool متسخ_ = true;               // Needs GPU upload
};

// ==============================================================================
// امتداد Renderer2D المتقدم / Advanced Renderer2D Extension
// ==============================================================================
/// امتداد لمحرك الرسم يضيف: منحنيات بزيه، ظلال، تدرجات، فلاتر
/// Extension for Renderer2D adding: Bézier curves, shadows, gradients, filters
///
/// يعمل فوق Renderer2D الموجود ويستخدمه للرسم الفعلي
/// Works on top of existing Renderer2D, using it for actual drawing
///
class Renderer2DAdvanced {
public:
    /// إنشاء من renderer موجود / Create from existing renderer
    explicit Renderer2DAdvanced(Renderer2D* renderer);
    ~Renderer2DAdvanced() = default;
    
    // ==================================================================
    // منحنيات بزيه / Bézier Curves
    // ==================================================================
    
    /// رسم منحنى بزيه تربيعي / Draw quadratic Bézier curve
    /// x0,y0: البداية / Start    x1,y1: نقطة التحكم / Control
    /// x2,y2: النهاية / End
    void DrawBezierQuadratic(Float32 x0, Float32 y0,
                             Float32 x1, Float32 y1,
                             Float32 x2, Float32 y2,
                             const Color& color = Color::White,
                             Float32 thickness = 1.0f, Int32 segments = 32);
    
    /// رسم منحنى بزيه تكعيبي / Draw cubic Bézier curve
    /// x0,y0: البداية / Start    x1,y1: تحكم 1 / Control 1
    /// x2,y2: تحكم 2 / Control 2   x3,y3: النهاية / End
    void DrawBezierCubic(Float32 x0, Float32 y0,
                         Float32 x1, Float32 y1,
                         Float32 x2, Float32 y2,
                         Float32 x3, Float32 y3,
                         const Color& color = Color::White,
                         Float32 thickness = 1.0f, Int32 segments = 48);
    
    /// رسم مسار بزيه متعدد النقاط / Draw multi-point Bézier path
    void DrawBezierPath(const std::vector<Point2D>& controlPoints,
                        const Color& color = Color::White,
                        Float32 thickness = 1.0f, Int32 segmentsPerCurve = 32);
    
    /// رسم منحنى B-Spline / Draw B-Spline curve
    void DrawBSpline(const std::vector<Point2D>& controlPoints,
                     Int32 degree = 3,
                     const Color& color = Color::White,
                     Float32 thickness = 1.0f, Int32 segments = 64);
    
    // ==================================================================
    // مستطيلات دائرية الزوايا / Rounded Rectangles
    // ==================================================================
    
    /// رسم مستطيل بزوايا دائرية / Draw rounded rectangle
    void DrawRoundedRect(Float32 x, Float32 y, Float32 w, Float32 h,
                         Float32 radius, const Color& color = Color::White,
                         bool filled = true);
    
    // ==================================================================
    // الظلال المسقطة / Drop Shadows
    // ==================================================================
    
    /// تفعيل/تعطيل الظل العام / Enable/disable global shadow
    void SetShadow(const إعدادات_الظل& shadow);
    void DisableShadow();
    
    /// رسم مستطيل مع ظل / Draw rectangle with shadow
    void DrawRectWithShadow(Float32 x, Float32 y, Float32 w, Float32 h,
                            const Color& color, const إعدادات_الظل& shadow);
    
    /// رسم دائرة مع ظل / Draw circle with shadow
    void DrawCircleWithShadow(Float32 x, Float32 y, Float32 radius,
                              const Color& color, const إعدادات_الظل& shadow);
    
    // ==================================================================
    // التدرجات اللونية / Gradients
    // ==================================================================
    
    /// رسم مستطيل بتدرج لوني / Draw gradient-filled rectangle
    void DrawGradientRect(Float32 x, Float32 y, Float32 w, Float32 h,
                          const إعدادات_التدرج& gradient);
    
    /// رسم دائرة بتدرج شعاعي / Draw radial-gradient circle
    void DrawGradientCircle(Float32 x, Float32 y, Float32 radius,
                            const Color& center, const Color& edge,
                            Int32 segments = 48);
    
    // ==================================================================
    // الفلاتر والمؤثرات / Filters and Effects
    // ==================================================================
    
    /// تطبيق ضبابية Gaussian على مخزن بكسل
    /// Apply Gaussian blur to pixel buffer
    static void ApplyGaussianBlur(مخزن_البكسل& buffer, Float32 radius);
    
    /// تطبيق توهج (Glow) حول الأشكال
    /// Apply glow effect around shapes
    static void ApplyGlow(مخزن_البكسل& buffer, const Color& glowColor, Float32 radius);
    
    /// تطبيق فلتر لوني / Apply color filter
    static void ApplyColorFilter(مخزن_البكسل& buffer,
                                  Float32 r_scale, Float32 g_scale,
                                  Float32 b_scale, Float32 a_scale = 1.0f);
    
    /// تطبيق فلتر التباين / Apply contrast filter
    static void ApplyContrast(مخزن_البكسل& buffer, Float32 factor);
    
    /// تطبيق فلتر السطوع / Apply brightness filter
    static void ApplyBrightness(مخزن_البكسل& buffer, Float32 offset);
    
    /// تحويل إلى تدرج رمادي / Convert to grayscale
    static void ApplyGrayscale(مخزن_البكسل& buffer);
    
    /// قلب الألوان / Invert colors
    static void ApplyInvert(مخزن_البكسل& buffer);
    
    // ==================================================================
    // تحويلات مخصصة / Custom Transforms
    // ==================================================================
    
    /// تطبيق مصفوفة تحويل / Apply transform matrix
    void SetTransform(const Transform2D& transform);
    
    /// إرجاع التحويل الحالي / Get current transform
    const Transform2D& GetTransform() const { return التحويل_الحالي_; }
    
    /// حفظ/استرجاع التحويل / Save/restore transform
    void PushTransform();
    void PopTransform();
    
    // ==================================================================
    // الوصول للمحرك الأساسي / Access base renderer
    // ==================================================================
    Renderer2D* GetBaseRenderer() { return المحرك_; }

private:
    Renderer2D* المحرك_;                          // المحرك الأساسي / Base renderer
    إعدادات_الظل الظل_الحالي_;                     // الظل الحالي / Current shadow
    Transform2D التحويل_الحالي_;                   // التحويل الحالي / Current transform
    std::vector<Transform2D> كومة_التحويلات_;     // كومة التحويلات / Transform stack
};

} // namespace SadGraphics

#endif // SAD_GRAPHICS_RENDERING_RENDERER2D_H
