// ==============================================================================
// context.cpp - تطبيق سياق الرسم OpenGL / OpenGL Rendering Context Implementation
// ==============================================================================
// الوصف: تطبيق إدارة سياق OpenGL وعمليات الرسم الأساسية
// Description: Implementation of OpenGL context management and basic rendering
// ==============================================================================

#include "../../include/rendering/context.h"  // ملف الرأس / Header file
#include <glad/glad.h>                        // GLAD OpenGL loader
#include <SDL.h>                              // مكتبة SDL2 / SDL2 library
#include <stdexcept>                          // للاستثناءات / For exceptions
#include <sstream>                            // لبناء النصوص / For string building

using namespace sad::graphics;                // استخدام namespace الأساسي

namespace SadGraphics {                       // مساحة الأسماء / Namespace

// ==============================================================================
// الإنشاء والتدمير / Construction and Destruction
// ==============================================================================

/// المنشئ - إنشاء سياق رسم جديد / Constructor - create new render context
RenderContext::RenderContext(SDL_Window* window, const ContextOptions& options)
    : m_window(window)                        // حفظ مؤشر النافذة / Store window pointer
    , m_glContext(nullptr)                    // تهيئة معرف السياق بـ null / Initialize context to null
    , m_options(options)                      // حفظ الخيارات / Store options
    , m_clearColor(Color::Black)              // لون المسح الافتراضي أسود / Default clear color is black
    , m_isValid(false)                        // البداية بحالة غير صالحة / Start as invalid
{
    // التحقق من صلاحية النافذة / Verify window validity
    if (!m_window) {                          // إذا كانت النافذة null / If window is null
        return;                               // الخروج مباشرة / Exit immediately
    }
    
    // ضبط خصائص SDL قبل إنشاء السياق / Set SDL attributes before creating context
    SetSDLAttributes();                       // تطبيق الخصائص / Apply attributes
    
    // إنشاء سياق OpenGL / Create OpenGL context
    m_isValid = CreateContext();              // محاولة الإنشاء وحفظ النتيجة / Try creation and store result
    
    // إذا نجح الإنشاء / If creation succeeded
    if (m_isValid) {                          // التحقق من النجاح / Check success
        // جعل السياق نشطاً / Make context current
        MakeCurrent();                        // تفعيل السياق / Activate context
        
        // تحميل دوال OpenGL / Load OpenGL functions
        if (!LoadGLFunctions()) {             // محاولة التحميل / Try loading
            m_isValid = false;                // فشل التحميل / Loading failed
            DestroyContext();                 // تدمير السياق / Destroy context
            return;                           // الخروج / Exit
        }
        
        // تهيئة الإعدادات الافتراضية / Initialize default settings
        InitializeSettings();                 // تطبيق الإعدادات / Apply settings
    }
}

/// المدمر - تدمير السياق / Destructor - destroy context
RenderContext::~RenderContext() {
    DestroyContext();                         // تدمير السياق عند التدمير / Destroy context on destruction
}

/// منشئ النقل / Move constructor
RenderContext::RenderContext(RenderContext&& other) noexcept
    : m_window(other.m_window)                // نقل النافذة / Move window
    , m_glContext(other.m_glContext)          // نقل السياق / Move context
    , m_options(other.m_options)              // نقل الخيارات / Move options
    , m_clearColor(other.m_clearColor)        // نقل لون المسح / Move clear color
    , m_isValid(other.m_isValid)              // نقل حالة الصلاحية / Move validity state
{
    // إبطال السياق المنقول منه / Invalidate moved-from object
    other.m_window = nullptr;                 // إزالة النافذة / Remove window
    other.m_glContext = nullptr;              // إزالة السياق / Remove context
    other.m_isValid = false;                  // جعله غير صالح / Make invalid
}

/// إسناد النقل / Move assignment
RenderContext& RenderContext::operator=(RenderContext&& other) noexcept {
    // التحقق من عدم الإسناد الذاتي / Check for self-assignment
    if (this != &other) {                     // إذا لم يكن نفس الكائن / If not same object
        // تدمير الموارد الحالية / Destroy current resources
        DestroyContext();                     // تنظيف السياق الحالي / Clean up current context
        
        // نقل البيانات / Move data
        m_window = other.m_window;            // نقل النافذة / Move window
        m_glContext = other.m_glContext;      // نقل السياق / Move context
        m_options = other.m_options;          // نقل الخيارات / Move options
        m_clearColor = other.m_clearColor;    // نقل لون المسح / Move clear color
        m_isValid = other.m_isValid;          // نقل حالة الصلاحية / Move validity state
        
        // إبطال الكائن المنقول منه / Invalidate moved-from object
        other.m_window = nullptr;             // إزالة النافذة / Remove window
        other.m_glContext = nullptr;          // إزالة السياق / Remove context
        other.m_isValid = false;              // جعله غير صالح / Make invalid
    }
    return *this;                             // إرجاع المرجع / Return reference
}

// ==============================================================================
// إدارة السياق / Context Management
// ==============================================================================

/// جعل هذا السياق نشطاً / Make this context current
Result RenderContext::MakeCurrent() {
    // التحقق من صلاحية السياق / Check context validity
    if (!m_isValid || !m_glContext) {         // إذا كان السياق غير صالح / If context invalid
        return Result::Failure;                 // إرجاع خطأ / Return error
    }
    
    // جعل السياق نشطاً في SDL / Make context current in SDL
    if (SDL_GL_MakeCurrent(m_window, m_glContext) != 0) {  // محاولة التفعيل / Try activation
        return Result::Failure;                 // فشل التفعيل / Activation failed
    }
    
    return Result::Success;                   // نجح التفعيل / Activation succeeded
}

/// تبديل المخازن المؤقتة / Swap buffers
void RenderContext::SwapBuffers() {
    // التحقق من صلاحية النافذة / Check window validity
    if (m_window) {                           // إذا كانت النافذة صالحة / If window valid
        SDL_GL_SwapWindow(m_window);          // تبديل المخازن / Swap buffers
    }
}

/// التحقق من صلاحية السياق / Check context validity
bool RenderContext::IsValid() const {
    return m_isValid;                         // إرجاع حالة الصلاحية / Return validity state
}

// ==============================================================================
// عمليات المسح / Clear Operations
// ==============================================================================

/// مسح الشاشة بلون / Clear screen with color
void RenderContext::Clear(const Color& color) {
    // اللون بالفعل في النطاق 0.0-1.0 / Color is already in 0.0-1.0 range
    glClearColor(color.r, color.g, color.b, color.a);  // تطبيق اللون / Apply color
    
    // مسح مخزن الألوان / Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);             // تنفيذ المسح / Execute clear
}

/// مسح مخزن العمق / Clear depth buffer
void RenderContext::ClearDepth(Float32 depth) {
    glClearDepth(depth);                      // ضبط قيمة العمق / Set depth value
    glClear(GL_DEPTH_BUFFER_BIT);             // مسح مخزن العمق / Clear depth buffer
}

/// مسح مخزن القالب / Clear stencil buffer
void RenderContext::ClearStencil(Int32 stencil) {
    glClearStencil(stencil);                  // ضبط قيمة القالب / Set stencil value
    glClear(GL_STENCIL_BUFFER_BIT);           // مسح مخزن القالب / Clear stencil buffer
}

/// مسح كل المخازن / Clear all buffers
void RenderContext::ClearAll(const Color& color, Float32 depth, Int32 stencil) {
    // ضبط القيم / Set values
    glClearColor(color.r, color.g, color.b, color.a);  // لون المسح / Clear color
    glClearDepth(depth);                      // عمق المسح / Clear depth
    glClearStencil(stencil);                  // قالب المسح / Clear stencil
    
    // مسح جميع المخازن / Clear all buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// ==============================================================================
// إعدادات الرسم / Rendering Settings
// ==============================================================================

/// ضبط منطقة العرض / Set viewport
void RenderContext::SetViewport(Int32 x, Int32 y, Int32 width, Int32 height) {
    glViewport(x, y, width, height);          // تطبيق منطقة العرض / Apply viewport
}

/// ضبط لون المسح الافتراضي / Set default clear color
void RenderContext::SetClearColor(const Color& color) {
    m_clearColor = color;                     // حفظ اللون / Store color
    
    // تطبيق اللون في OpenGL / Apply color in OpenGL
    glClearColor(color.r, color.g, color.b, color.a);  // ضبط في OpenGL / Set in OpenGL
}

/// الحصول على لون المسح / Get clear color
Color RenderContext::GetClearColor() const {
    return m_clearColor;                      // إرجاع اللون المحفوظ / Return stored color
}

/// تفعيل/تعطيل المزج / Enable/disable blending
void RenderContext::SetBlendingEnabled(bool enable) {
    if (enable) {                             // إذا كان التفعيل مطلوب / If enabling requested
        glEnable(GL_BLEND);                   // تفعيل المزج / Enable blending
        // ضبط وظيفة المزج (شفافية قياسية) / Set blend function (standard transparency)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {                                  // إذا كان التعطيل مطلوب / If disabling requested
        glDisable(GL_BLEND);                  // تعطيل المزج / Disable blending
    }
}

/// تفعيل/تعطيل اختبار العمق / Enable/disable depth testing
void RenderContext::SetDepthTestEnabled(bool enable) {
    if (enable) {                             // التفعيل / Enabling
        glEnable(GL_DEPTH_TEST);              // تفعيل اختبار العمق / Enable depth test
    } else {                                  // التعطيل / Disabling
        glDisable(GL_DEPTH_TEST);             // تعطيل اختبار العمق / Disable depth test
    }
}

/// تفعيل/تعطيل إزالة الوجوه / Enable/disable culling
void RenderContext::SetCullingEnabled(bool enable) {
    if (enable) {                             // التفعيل / Enabling
        glEnable(GL_CULL_FACE);               // تفعيل الإزالة / Enable culling
        glCullFace(GL_BACK);                  // إزالة الوجوه الخلفية / Cull back faces
    } else {                                  // التعطيل / Disabling
        glDisable(GL_CULL_FACE);              // تعطيل الإزالة / Disable culling
    }
}

/// ضبط عرض الخط / Set line width
void RenderContext::SetLineWidth(Float32 width) {
    glLineWidth(width);                       // تطبيق العرض / Apply width
}

/// ضبط حجم النقطة / Set point size
void RenderContext::SetPointSize(Float32 size) {
    glPointSize(size);                        // تطبيق الحجم / Apply size
}

// ==============================================================================
// معلومات OpenGL / OpenGL Information
// ==============================================================================

/// الحصول على إصدار OpenGL / Get OpenGL version
std::string RenderContext::GetVersion() const {
    // الحصول على نص الإصدار من OpenGL / Get version string from OpenGL
    const GLubyte* version = glGetString(GL_VERSION);  // طلب الإصدار / Request version
    return version ? std::string(reinterpret_cast<const char*>(version)) : "Unknown";
}

/// الحصول على اسم المصنع / Get vendor name
std::string RenderContext::GetVendor() const {
    const GLubyte* vendor = glGetString(GL_VENDOR);  // طلب المصنع / Request vendor
    return vendor ? std::string(reinterpret_cast<const char*>(vendor)) : "Unknown";
}

/// الحصول على اسم المعالج / Get renderer name
std::string RenderContext::GetRenderer() const {
    const GLubyte* renderer = glGetString(GL_RENDERER);  // طلب المعالج / Request renderer
    return renderer ? std::string(reinterpret_cast<const char*>(renderer)) : "Unknown";
}

/// الحصول على إصدار GLSL / Get GLSL version
std::string RenderContext::GetShadingLanguageVersion() const {
    const GLubyte* version = glGetString(GL_SHADING_LANGUAGE_VERSION);  // طلب إصدار GLSL / Request GLSL version
    return version ? std::string(reinterpret_cast<const char*>(version)) : "Unknown";
}

/// التحقق من دعم امتداد / Check extension support
bool RenderContext::IsExtensionSupported(const std::string& extension) const {
    // الحصول على عدد الامتدادات / Get number of extensions
    GLint numExtensions = 0;                  // متغير للعدد / Variable for count
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);  // طلب العدد / Request count
    
    // البحث عن الامتداد / Search for extension
    for (GLint i = 0; i < numExtensions; i++) {  // المرور على الامتدادات / Iterate extensions
        const GLubyte* ext = glGetStringi(GL_EXTENSIONS, i);  // الحصول على الامتداد / Get extension
        if (ext && extension == reinterpret_cast<const char*>(ext)) {  // المقارنة / Compare
            return true;                      // وُجد / Found
        }
    }
    
    return false;                             // لم يُوجد / Not found
}

/// طباعة معلومات OpenGL / Print OpenGL info
void RenderContext::PrintInfo(LogCallback callback) const {
    // إنشاء النص / Build text
    std::stringstream ss;                     // باني النصوص / String builder
    ss << "OpenGL Information:\n";            // العنوان / Title
    ss << "  Version: " << GetVersion() << "\n";           // الإصدار / Version
    ss << "  Vendor: " << GetVendor() << "\n";             // المصنع / Vendor
    ss << "  Renderer: " << GetRenderer() << "\n";         // المعالج / Renderer
    ss << "  GLSL Version: " << GetShadingLanguageVersion() << "\n";  // إصدار GLSL / GLSL version
    
    // الطباعة / Print
    if (callback) {                           // إذا كان هناك callback / If callback exists
        callback(LogLevel::Info, ss.str());   // استخدام الـ callback / Use callback
    }
}

// ==============================================================================
// الدوال الخاصة / Private Functions
// ==============================================================================

/// إنشاء سياق OpenGL / Create OpenGL context
bool RenderContext::CreateContext() {
    // إنشاء السياق عبر SDL / Create context through SDL
    m_glContext = SDL_GL_CreateContext(m_window);  // محاولة الإنشاء / Try creation
    
    // التحقق من النجاح / Check success
    if (!m_glContext) {                       // إذا فشل الإنشاء / If creation failed
        return false;                         // إرجاع فشل / Return failure
    }
    
    // تحميل دوال OpenGL عبر GLAD / Load OpenGL functions via GLAD
    #ifndef EMSCRIPTEN
    if (!gladLoadGL()) {
#else
    if (false) {
#endif                      // إذا فشل التحميل / If loading failed
        SDL_GL_DeleteContext(m_glContext);    // حذف السياق / Delete context
        m_glContext = nullptr;                // إعادة التهيئة / Reset
        return false;                         // إرجاع فشل / Return failure
    }
    
    // ضبط VSync إذا كان مطلوباً / Set VSync if requested
    if (m_options.vsync) {                    // إذا كان VSync مفعل / If VSync enabled
        SDL_GL_SetSwapInterval(m_options.swapInterval);  // ضبط فترة التبديل / Set swap interval
    } else {                                  // إذا كان معطل / If disabled
        SDL_GL_SetSwapInterval(0);            // تعطيل VSync / Disable VSync
    }
    
    return true;                              // نجح الإنشاء / Creation succeeded
}

/// تدمير السياق / Destroy context
void RenderContext::DestroyContext() {
    // التحقق من وجود السياق / Check context existence
    if (m_glContext) {                        // إذا كان السياق موجود / If context exists
        SDL_GL_DeleteContext(m_glContext);    // تدمير السياق / Destroy context
        m_glContext = nullptr;                // إزالة المؤشر / Remove pointer
    }
    
    m_isValid = false;                        // جعل الحالة غير صالحة / Make invalid
}

/// تهيئة الإعدادات / Initialize settings
void RenderContext::InitializeSettings() {
    // ضبط لون المسح الافتراضي / Set default clear color
    SetClearColor(m_clearColor);              // تطبيق اللون / Apply color
    
    // تفعيل المزج للشفافية / Enable blending for transparency
    SetBlendingEnabled(true);                 // تفعيل المزج / Enable blending
    
    // تعطيل اختبار العمق افتراضياً (2D) / Disable depth test by default (2D)
    SetDepthTestEnabled(false);               // تعطيل العمق / Disable depth
    
    // تعطيل إزالة الوجوه (2D) / Disable culling (2D)
    SetCullingEnabled(false);                 // تعطيل الإزالة / Disable culling
}

/// تحميل دوال OpenGL / Load OpenGL functions
bool RenderContext::LoadGLFunctions() {
    // SDL تحمل الدوال تلقائياً / SDL loads functions automatically
    // يمكن إضافة GLEW أو GLAD لاحقاً / Can add GLEW or GLAD later
    return true;                              // نجح التحميل / Loading succeeded
}

/// ضبط خصائص SDL / Set SDL attributes
void RenderContext::SetSDLAttributes() {
    // ضبط إصدار OpenGL / Set OpenGL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, m_options.majorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, m_options.minorVersion);
    
    // ضبط الملف الشخصي / Set profile
    if (m_options.coreProfile) {              // إذا كان Core Profile مطلوب / If core profile requested
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    }
    
    // التوافق للأمام / Forward compatibility
    if (m_options.forwardCompatible) {        // إذا كان مطلوب / If requested
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    }
    
    // خيارات المخازن / Buffer options
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, m_options.depthBits);      // عمق العمق / Depth bits
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, m_options.stencilBits);  // عمق القالب / Stencil bits
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, m_options.redBits);          // بتات الأحمر / Red bits
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, m_options.greenBits);      // بتات الأخضر / Green bits
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, m_options.blueBits);        // بتات الأزرق / Blue bits
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, m_options.alphaBits);      // بتات الشفافية / Alpha bits
    
    // المخزن المزدوج / Double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, m_options.doubleBuffer ? 1 : 0);
    
    // التسريع العتادي / Hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, m_options.hardwareAcceleration ? 1 : 0);
    
    // Anti-aliasing (MSAA)
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, m_options.multiSampleBuffers);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, m_options.multiSampleSamples);
}

} // namespace SadGraphics
