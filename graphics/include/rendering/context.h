// ==============================================================================
// context.h - سياق الرسم OpenGL / OpenGL Rendering Context
// ==============================================================================
// الوصف: إدارة سياق OpenGL وعمليات الرسم الأساسية
// Description: OpenGL context management and basic rendering operations
// ==============================================================================

#ifndef SAD_GRAPHICS_RENDERING_CONTEXT_H
#define SAD_GRAPHICS_RENDERING_CONTEXT_H

#include "../core/types.h"      // تضمين الأنواع الأساسية / Include basic types
#include <string>                // لأسماء النصوص / For string names
#include <memory>                // للمؤشرات الذكية / For smart pointers

// التعريفات الأمامية / Forward declarations
struct SDL_Window;               // نافذة SDL / SDL window

namespace SadGraphics {          // مساحة الأسماء للمكتبة / Library namespace

// ==============================================================================
// خيارات سياق OpenGL / OpenGL Context Options
// ==============================================================================
struct ContextOptions {
    // الإصدار المطلوب / Required version
    Int32 majorVersion = 3;      // الإصدار الرئيسي / Major version (default: 3)
    Int32 minorVersion = 3;      // الإصدار الفرعي / Minor version (default: 3)
    
    // خيارات الملف الشخصي / Profile options
    bool coreProfile = true;     // استخدام Core Profile / Use core profile
    bool forwardCompatible = true; // التوافق للأمام / Forward compatibility
    
    // خيارات المخازن المؤقتة / Buffer options
    Int32 depthBits = 24;        // عمق بت العمق / Depth bits (default: 24)
    Int32 stencilBits = 8;       // عمق بت القالب / Stencil bits (default: 8)
    Int32 redBits = 8;           // عمق اللون الأحمر / Red color bits
    Int32 greenBits = 8;         // عمق اللون الأخضر / Green color bits
    Int32 blueBits = 8;          // عمق اللون الأزرق / Blue color bits
    Int32 alphaBits = 8;         // عمق الشفافية / Alpha bits
    
    // خيارات الأداء / Performance options
    bool doubleBuffer = true;    // استخدام مخزن مزدوج / Use double buffering
    bool hardwareAcceleration = true; // التسريع العتادي / Hardware acceleration
    Int32 multiSampleBuffers = 1; // عدد مخازن العينات المتعددة / Multisample buffers
    Int32 multiSampleSamples = 4; // عدد العينات لكل بكسل / Samples per pixel (MSAA)
    
    // خيارات VSync / VSync options
    bool vsync = false;          // مزامنة التحديث الرأسي / Vertical sync
    Int32 swapInterval = 0;      // فترة التبديل (0=معطل، 1=مفعل، -1=adaptive) / Swap interval
};

// ==============================================================================
// فئة سياق الرسم / Rendering Context Class
// ==============================================================================
class RenderContext {
public:
    // ==============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ==============================================================================
    
    /// إنشاء سياق رسم جديد / Create new render context
    /// window: نافذة SDL المرتبطة / Associated SDL window
    /// options: خيارات السياق / Context options
    RenderContext(SDL_Window* window, const ContextOptions& options = ContextOptions());
    
    /// المدمر - تدمير السياق / Destructor - destroy context
    ~RenderContext();
    
    // منع النسخ / Prevent copying
    RenderContext(const RenderContext&) = delete;              // منع النسخ / Disable copy constructor
    RenderContext& operator=(const RenderContext&) = delete;   // منع الإسناد / Disable copy assignment
    
    // السماح بالنقل / Allow moving
    RenderContext(RenderContext&& other) noexcept;             // منشئ النقل / Move constructor
    RenderContext& operator=(RenderContext&& other) noexcept;  // إسناد النقل / Move assignment
    
    // ==============================================================================
    // إدارة السياق / Context Management
    // ==============================================================================
    
    /// جعل هذا السياق نشطاً / Make this context current
    /// Returns: نجح أم لا / Success or failure
    Result MakeCurrent();
    
    /// تبديل المخازن المؤقتة (عرض المرسوم) / Swap buffers (display rendered content)
    void SwapBuffers();
    
    /// التحقق من نجاح الإنشاء / Check if successfully created
    /// Returns: true إذا كان السياق صالح / true if context is valid
    bool IsValid() const;
    
    // ==============================================================================
    // عمليات المسح / Clear Operations
    // ==============================================================================
    
    /// مسح الشاشة بلون محدد / Clear screen with specified color
    /// color: اللون المستخدم للمسح / Color to clear with
    void Clear(const Color& color = Color::Black);
    
    /// مسح مخزن العمق / Clear depth buffer
    void ClearDepth(Float32 depth = 1.0f);
    
    /// مسح مخزن القالب / Clear stencil buffer
    void ClearStencil(Int32 stencil = 0);
    
    /// مسح كل المخازن / Clear all buffers
    /// color: لون المسح / Clear color
    /// depth: قيمة العمق / Depth value
    /// stencil: قيمة القالب / Stencil value
    void ClearAll(const Color& color = Color::Black, Float32 depth = 1.0f, Int32 stencil = 0);
    
    // ==============================================================================
    // إعدادات الرسم / Rendering Settings
    // ==============================================================================
    
    /// ضبط منطقة العرض / Set viewport
    /// x, y: موقع الزاوية السفلية اليسرى / Bottom-left corner position
    /// width, height: أبعاد منطقة العرض / Viewport dimensions
    void SetViewport(Int32 x, Int32 y, Int32 width, Int32 height);
    
    /// ضبط لون المسح الافتراضي / Set default clear color
    /// color: اللون الجديد / New color
    void SetClearColor(const Color& color);
    
    /// الحصول على لون المسح الحالي / Get current clear color
    /// Returns: اللون الحالي / Current color
    Color GetClearColor() const;
    
    /// تفعيل/تعطيل مزج الألوان (الشفافية) / Enable/disable blending (transparency)
    /// enable: تفعيل أو تعطيل / Enable or disable
    void SetBlendingEnabled(bool enable);
    
    /// تفعيل/تعطيل اختبار العمق / Enable/disable depth testing
    /// enable: تفعيل أو تعطيل / Enable or disable
    void SetDepthTestEnabled(bool enable);
    
    /// تفعيل/تعطيل إزالة الوجوه الخلفية / Enable/disable backface culling
    /// enable: تفعيل أو تعطيل / Enable or disable
    void SetCullingEnabled(bool enable);
    
    /// ضبط عرض الخط / Set line width
    /// width: العرض بالبكسل / Width in pixels
    void SetLineWidth(Float32 width);
    
    /// ضبط حجم النقطة / Set point size
    /// size: الحجم بالبكسل / Size in pixels
    void SetPointSize(Float32 size);
    
    // ==============================================================================
    // معلومات OpenGL / OpenGL Information
    // ==============================================================================
    
    /// الحصول على إصدار OpenGL / Get OpenGL version
    /// Returns: نص الإصدار / Version string
    std::string GetVersion() const;
    
    /// الحصول على اسم البطاقة الرسومية / Get graphics card vendor
    /// Returns: اسم المصنع / Vendor name
    std::string GetVendor() const;
    
    /// الحصول على اسم المعالج الرسومي / Get renderer name
    /// Returns: اسم المعالج / Renderer name
    std::string GetRenderer() const;
    
    /// الحصول على لغة تظليل GLSL المدعومة / Get supported GLSL version
    /// Returns: إصدار GLSL / GLSL version
    std::string GetShadingLanguageVersion() const;
    
    /// التحقق من دعم امتداد معين / Check if extension is supported
    /// extension: اسم الامتداد / Extension name
    /// Returns: مدعوم أم لا / Supported or not
    bool IsExtensionSupported(const std::string& extension) const;
    
    /// طباعة معلومات OpenGL / Print OpenGL information
    /// callback: دالة طباعة السجل / Log callback function
    void PrintInfo(LogCallback callback = nullptr) const;
    
    // ==============================================================================
    // الحصول على المعلومات / Getters
    // ==============================================================================
    
    /// الحصول على معرف السياق الأصلي / Get native context handle
    /// Returns: معرف السياق / Context handle
    void* GetNativeHandle() const { return m_glContext; }
    
    /// الحصول على النافذة المرتبطة / Get associated window
    /// Returns: مؤشر النافذة / Window pointer
    SDL_Window* GetWindow() const { return m_window; }
    
    /// الحصول على الخيارات المستخدمة / Get used options
    /// Returns: خيارات السياق / Context options
    const ContextOptions& GetOptions() const { return m_options; }

private:
    // ==============================================================================
    // البيانات الخاصة / Private Data
    // ==============================================================================
    
    SDL_Window* m_window;        // النافذة المرتبطة / Associated window
    void* m_glContext;           // معرف سياق OpenGL / OpenGL context handle
    ContextOptions m_options;    // خيارات السياق / Context options
    Color m_clearColor;          // لون المسح الافتراضي / Default clear color
    bool m_isValid;              // هل السياق صالح / Is context valid
    
    // ==============================================================================
    // الدوال الخاصة / Private Functions
    // ==============================================================================
    
    /// إنشاء سياق OpenGL / Create OpenGL context
    /// Returns: نجح أم لا / Success or failure
    bool CreateContext();
    
    /// تدمير السياق / Destroy context
    void DestroyContext();
    
    /// تهيئة إعدادات OpenGL / Initialize OpenGL settings
    void InitializeSettings();
    
    /// تحميل دوال OpenGL / Load OpenGL functions
    /// Returns: نجح أم لا / Success or failure
    bool LoadGLFunctions();
    
    /// ضبط خصائص SDL قبل الإنشاء / Set SDL attributes before creation
    void SetSDLAttributes();
};

} // namespace SadGraphics

#endif // SAD_GRAPHICS_RENDERING_CONTEXT_H
