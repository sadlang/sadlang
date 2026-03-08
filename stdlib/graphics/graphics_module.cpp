// بسم الله الرحمن الرحيم
/**
 * @file graphics_module.cpp
 * @brief Graphics Module Implementation - تنفيذ وحدة الرسومات
 * 
 * Implementation of graphics bindings for Sad Language
 * تنفيذ روابط الرسومات للغة ص
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include "graphics_module.h"                         // ملف الرأس الخاص بالوحدة / Module header file
#include <memory>                                    // للمؤشرات الذكية / For smart pointers
#include <stdexcept>                                 // للاستثناءات / For exceptions
#include <iostream>                                  // للطباعة / For output
#include <map>                                       // للخرائط / For maps
#include <SDL.h>                                     // مكتبة SDL2 / SDL2 library
#include "../../../graphics/include/text/font.h"              // نظام الخطوط / Font system
#include "../../../graphics/include/window/window.h"             // نظام النوافذ / Window system
#include "../../../graphics/include/rendering/context.h"         // سياق الرسم / Render context
#include "../../../graphics/include/rendering/renderer2d.h"      // محرك الرسم 2D / 2D renderer
#include "../../../graphics/include/core/types.h"                // الأنواع الأساسية / Basic types
#include "../../../graphics/include/ui/widget.h"                 // عناصر الواجهة / UI widgets
#include "../../../graphics/include/texture/texture.h"           // النسيج / Textures
#include "../../../graphics/include/audio/audio_system.h"        // نظام الصوت / Audio system

namespace sad {
namespace stdlib {
namespace graphics {

// ============================================================================
// نظام إدارة الموارد الرسومية المتقدم
// Advanced Graphics Resource Management System
// ============================================================================

/// هيكل لتخزين معلومات النافذة / Structure to store window information
struct WindowData {
    std::shared_ptr<sad::graphics::Window> window;          // مؤشر ذكي للنافذة / Smart pointer to window
    std::shared_ptr<SadGraphics::RenderContext> context;    // سياق الرسم OpenGL / OpenGL render context
    bool isOpen;                                            // حالة النافذة / Window state
    
    // المنشئ / Constructor
    WindowData() : window(nullptr), context(nullptr), isOpen(false) {}
};

/// هيكل لتخزين معلومات الرسام / Structure to store renderer information
struct RendererData {
    std::shared_ptr<SadGraphics::Renderer2D> renderer;      // مؤشر ذكي للرسام / Smart pointer to renderer
    int windowId;                                           // معرف النافذة المرتبطة / Associated window ID
    
    // المنشئ / Constructor
    RendererData() : renderer(nullptr), windowId(-1) {}
};

/// هيكل لتخزين معلومات عناصر الواجهة / Structure to store widget information
struct WidgetData {
    std::string type;           // نوع العنصر (label, button, textinput, checkbox) / Widget type
    std::string text;           // النص أو القيمة / Text or value
    float x, y;                 // الموقع / Position
    float width, height;        // الأبعاد / Dimensions
    bool state;                 // الحالة (للأزرار ومربعات الاختيار) / State for buttons/checkboxes
    
    // المنشئ / Constructor
    WidgetData() : type(""), text(""), x(0), y(0), width(0), height(0), state(false) {}
};

// ============================================================================
// التخزين العام للكائنات الرسومية / Global storage for graphics objects
// ============================================================================

static std::map<int, WindowData> g_windows;              // خريطة النوافذ / Windows map
static std::map<int, RendererData> g_renderers;          // خريطة الرسامين / Renderers map
static std::map<int, WidgetData> g_widgets;              // خريطة العناصر / Widgets map
static std::map<int, sad::graphics::FontRef> g_fontsBySize; // كاش الخطوط حسب الحجم / Font cache by size
static int g_nextId = 1;                                 // المعرف التالي / Next ID
static bool g_sdlInitialized = false;                    // حالة تهيئة SDL / SDL initialization state

// ============================================================================
// دوال مساعدة خاصة / Private helper functions
// ============================================================================

/// تهيئة نظام SDL إذا لم يكن مهيئاً / Initialize SDL system if not already initialized
static bool InitializeSDL() {
    if (g_sdlInitialized) {                              // إذا كان SDL مهيئاً بالفعل / If SDL already initialized
        return true;                                     // إرجاع نجاح / Return success
    }
    
    // تهيئة SDL مع دعم الفيديو والأحداث / Initialize SDL with video and events
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        std::cerr << "فشل تهيئة SDL / SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;                                    // إرجاع فشل / Return failure
    }
    
    g_sdlInitialized = true;                             // تعيين حالة التهيئة / Set initialized state
    std::cout << "تم تهيئة SDL بنجاح / SDL initialized successfully" << std::endl;
    return true;                                         // إرجاع نجاح / Return success
}

/// الحصول على بيانات نافذة / Get window data
static WindowData* GetWindow(int windowId) {
    auto it = g_windows.find(windowId);                  // البحث عن النافذة / Search for window
    if (it == g_windows.end()) {                         // إذا لم توجد / If not found
        throw std::runtime_error("معرف نافذة غير صالح / Invalid window ID: " + std::to_string(windowId));
    }
    return &it->second;                                  // إرجاع مؤشر للبيانات / Return pointer to data
}

/// الحصول على بيانات رسام / Get renderer data
static RendererData* GetRenderer(int rendererId) {
    auto it = g_renderers.find(rendererId);              // البحث عن الرسام / Search for renderer
    if (it == g_renderers.end()) {                       // إذا لم يوجد / If not found
        throw std::runtime_error("معرف رسام غير صالح / Invalid renderer ID: " + std::to_string(rendererId));
    }
    return &it->second;                                  // إرجاع مؤشر للبيانات / Return pointer to data
}

/// الحصول على بيانات عنصر واجهة / Get widget data
static WidgetData* GetWidget(int widgetId) {
    auto it = g_widgets.find(widgetId);                  // البحث عن العنصر / Search for widget
    if (it == g_widgets.end()) {                         // إذا لم يوجد / If not found
        throw std::runtime_error("معرف عنصر غير صالح / Invalid widget ID: " + std::to_string(widgetId));
    }
    return &it->second;                                  // إرجاع مؤشر للبيانات / Return pointer to data
}

/// الحصول على خط افتراضي بحجم معين مع كاش داخلي / Get cached default font by size
static sad::graphics::FontRef GetOrCreateDefaultFont(float size) {
    int roundedSize = static_cast<int>(size <= 0.0f ? 16.0f : size);
    auto it = g_fontsBySize.find(roundedSize);
    if (it != g_fontsBySize.end() && it->second && it->second->IsValid()) {
        return it->second;
    }

    auto font = sad::graphics::Font::CreateDefault(static_cast<float>(roundedSize));
    if (font && font->IsValid()) {
        g_fontsBySize[roundedSize] = font;
    }
    return font;
}


// ============================================================================
// Window Management Implementation - تنفيذ إدارة النوافذ
// ============================================================================

/// إنشاء نافذة جديدة / Create new window
int window_create_impl(const std::string& title, int width, int height) {
    try {
        // تهيئة SDL إذا لم يكن مهيئاً / Initialize SDL if not initialized
        if (!InitializeSDL()) {                          // محاولة التهيئة / Try initialization
            std::cerr << "فشل إنشاء النافذة: SDL غير مهيأ / Window creation failed: SDL not initialized" << std::endl;
            return -1;                                   // إرجاع خطأ / Return error
        }
        
        // إنشاء كائن بيانات النافذة / Create window data object
        WindowData windowData;                           // بيانات النافذة الجديدة / New window data
        
        // إنشاء نافذة SDL مع خيارات افتراضية / Create SDL window with default options
        using namespace sad::graphics;                   // استخدام namespace النوافذ / Use windows namespace
        WindowFlags flags = WindowFlags::Resizable;      // جعل النافذة قابلة لتغيير الحجم / Make window resizable
        
        // إنشاء كائن النافذة / Create window object
        windowData.window = std::make_shared<sad::graphics::Window>(title, width, height, flags);
        
        // التحقق من نجاح الإنشاء / Verify creation success
        if (!windowData.window || !windowData.window->IsOpen()) {
            std::cerr << "فشل إنشاء كائن النافذة / Failed to create window object" << std::endl;
            return -1;                                   // إرجاع خطأ / Return error
        }
        
        // إنشاء سياق OpenGL للنافذة / Create OpenGL context for window
        SadGraphics::ContextOptions contextOpts;         // خيارات السياق / Context options
        contextOpts.majorVersion = 3;                    // إصدار OpenGL الرئيسي / OpenGL major version
        contextOpts.minorVersion = 3;                    // إصدار OpenGL الفرعي / OpenGL minor version
        contextOpts.coreProfile = true;                  // استخدام Core Profile / Use core profile
        
        // الحصول على نافذة SDL الأساسية / Get underlying SDL window
        SDL_Window* sdlWindow = windowData.window->GetNativeWindow();
        
        // إنشاء سياق الرسم / Create render context
        windowData.context = std::make_shared<SadGraphics::RenderContext>(sdlWindow, contextOpts);
        
        // التحقق من صلاحية السياق / Verify context validity
        if (!windowData.context || !windowData.context->IsValid()) {
            std::cerr << "فشل إنشاء سياق الرسم / Failed to create render context" << std::endl;
            return -1;                                   // إرجاع خطأ / Return error
        }
        
        // تعيين حالة النافذة / Set window state
        windowData.isOpen = true;                        // النافذة مفتوحة الآن / Window is now open
        
        // إظهار النافذة / Show window
        windowData.window->Show();                       // عرض النافذة / Display window
        
        // حفظ البيانات وتوليد المعرف / Store data and generate ID
        int windowId = g_nextId++;                       // توليد معرف فريد / Generate unique ID
        g_windows[windowId] = std::move(windowData);     // حفظ في الخريطة / Store in map
        
        std::cout << "تم إنشاء نافذة: \"" << title << "\" (" << width << "x" << height << ") - ID: " << windowId << std::endl;
        std::cout << "Created window: \"" << title << "\" (" << width << "x" << height << ") - ID: " << windowId << std::endl;
        
        return windowId;                                 // إرجاع المعرف / Return ID
    }
    catch (const std::exception& e) {                    // معالجة الاستثناءات / Handle exceptions
        std::cerr << "استثناء عند إنشاء النافذة / Exception during window creation: " << e.what() << std::endl;
        return -1;                                       // إرجاع خطأ / Return error
    }
}

/// إظهار النافذة / Show window
void window_show_impl(int windowId) {
    try {
        auto windowData = GetWindow(windowId);           // الحصول على بيانات النافذة / Get window data
        if (windowData->window) {                        // التحقق من وجود النافذة / Check window exists
            windowData->window->Show();                  // إظهار النافذة / Show window
            std::cout << "تم إظهار النافذة ID: " << windowId << " / Shown window ID: " << windowId << std::endl;
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في إظهار النافذة / Error showing window: " << e.what() << std::endl;
    }
}

/// إخفاء النافذة / Hide window
void window_hide_impl(int windowId) {
    try {
        auto windowData = GetWindow(windowId);           // الحصول على بيانات النافذة / Get window data
        if (windowData->window) {                        // التحقق من وجود النافذة / Check window exists
            windowData->window->Hide();                  // إخفاء النافذة / Hide window
            std::cout << "تم إخفاء النافذة ID: " << windowId << " / Hidden window ID: " << windowId << std::endl;
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في إخفاء النافذة / Error hiding window: " << e.what() << std::endl;
    }
}

/// إغلاق النافذة / Close window
void window_close_impl(int windowId) {
    try {
        auto windowData = GetWindow(windowId);           // الحصول على بيانات النافذة / Get window data
        if (windowData->window) {                        // التحقق من وجود النافذة / Check window exists
            windowData->window->Close();                 // إغلاق النافذة / Close window
            windowData->isOpen = false;                  // تحديث الحالة / Update state
            std::cout << "تم إغلاق النافذة ID: " << windowId << " / Closed window ID: " << windowId << std::endl;
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في إغلاق النافذة / Error closing window: " << e.what() << std::endl;
    }
}

/// فحص إذا كان يجب إغلاق النافذة / Check if window should close
bool window_should_close_impl(int windowId) {
    try {
        auto windowData = GetWindow(windowId);           // الحصول على بيانات النافذة / Get window data
        if (windowData->window) {                        // التحقق من وجود النافذة / Check window exists
            return !windowData->window->IsOpen();        // إرجاع عكس حالة الفتح / Return inverse of open state
        }
        return true;                                     // افتراضياً يجب الإغلاق / Default to should close
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في فحص حالة النافذة / Error checking window state: " << e.what() << std::endl;
        return true;                                     // عند الخطأ، افترض الإغلاق / On error, assume should close
    }
}

/// معالجة أحداث النافذة / Poll window events
void window_poll_events_impl(int windowId) {
    try {
        auto windowData = GetWindow(windowId);           // الحصول على بيانات النافذة / Get window data
        if (windowData->window) {                        // التحقق من وجود النافذة / Check window exists
            windowData->window->ProcessEvents();         // معالجة الأحداث / Process events
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في معالجة الأحداث / Error polling events: " << e.what() << std::endl;
    }
}

/// تبديل المخازن المؤقتة للنافذة / Swap window buffers
void window_swap_buffers_impl(int windowId) {
    try {
        auto windowData = GetWindow(windowId);           // الحصول على بيانات النافذة / Get window data
        if (windowData->context) {                       // التحقق من وجود السياق / Check context exists
            windowData->context->SwapBuffers();          // تبديل المخازن / Swap buffers
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في تبديل المخازن / Error swapping buffers: " << e.what() << std::endl;
    }
}


// ============================================================================
// Renderer Management Implementation - تنفيذ إدارة الرسام
// ============================================================================

/// إنشاء رسام جديد / Create new renderer
int renderer_create_impl(int windowId) {
    try {
        auto windowData = GetWindow(windowId);           // الحصول على بيانات النافذة / Get window data
        
        // التحقق من وجود السياق / Verify context exists
        if (!windowData->context || !windowData->context->IsValid()) {
            std::cerr << "فشل إنشاء الرسام: سياق غير صالح / Renderer creation failed: invalid context" << std::endl;
            return -1;                                   // إرجاع خطأ / Return error
        }
        
        // إنشاء كائن بيانات الرسام / Create renderer data object
        RendererData rendererData;                       // بيانات الرسام الجديد / New renderer data
        rendererData.windowId = windowId;                // ربط بالنافذة / Link to window
        
        // إنشاء كائن الرسام 2D / Create 2D renderer object
        rendererData.renderer = std::make_shared<SadGraphics::Renderer2D>(windowData->context.get());
        
        // التحقق من نجاح الإنشاء / Verify creation success
        if (!rendererData.renderer) {                    // إذا فشل الإنشاء / If creation failed
            std::cerr << "فشل إنشاء كائن الرسام / Failed to create renderer object" << std::endl;
            return -1;                                   // إرجاع خطأ / Return error
        }
        
        // حفظ البيانات وتوليد المعرف / Store data and generate ID
        int rendererId = g_nextId++;                     // توليد معرف فريد / Generate unique ID
        g_renderers[rendererId] = std::move(rendererData); // حفظ في الخريطة / Store in map
        
        std::cout << "تم إنشاء رسام للنافذة ID: " << windowId << " - Renderer ID: " << rendererId << std::endl;
        std::cout << "Created renderer for window ID: " << windowId << " - Renderer ID: " << rendererId << std::endl;
        
        return rendererId;                               // إرجاع المعرف / Return ID
    }
    catch (const std::exception& e) {                    // معالجة الاستثناءات / Handle exceptions
        std::cerr << "استثناء عند إنشاء الرسام / Exception during renderer creation: " << e.what() << std::endl;
        return -1;                                       // إرجاع خطأ / Return error
    }
}

/// بدء إطار رسم جديد / Begin rendering frame
void renderer_begin_frame_impl(int rendererId) {
    try {
        auto rendererData = GetRenderer(rendererId);     // الحصول على بيانات الرسام / Get renderer data
        if (rendererData->renderer) {                    // التحقق من وجود الرسام / Check renderer exists
            rendererData->renderer->BeginFrame();        // بدء الإطار / Begin frame
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في بدء الإطار / Error beginning frame: " << e.what() << std::endl;
    }
}

/// إنهاء إطار الرسم / End rendering frame
void renderer_end_frame_impl(int rendererId) {
    try {
        auto rendererData = GetRenderer(rendererId);     // الحصول على بيانات الرسام / Get renderer data
        if (rendererData->renderer) {                    // التحقق من وجود الرسام / Check renderer exists
            rendererData->renderer->EndFrame();          // إنهاء الإطار / End frame
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في إنهاء الإطار / Error ending frame: " << e.what() << std::endl;
    }
}

/// مسح الشاشة بلون / Clear screen with color
void renderer_clear_impl(int rendererId, int r, int g, int b, int a) {
    try {
        auto rendererData = GetRenderer(rendererId);     // الحصول على بيانات الرسام / Get renderer data
        if (rendererData->renderer) {                    // التحقق من وجود الرسام / Check renderer exists
            // إنشاء كائن لون من القيم / Create color object from values
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);
            
            rendererData->renderer->Clear(color);        // مسح الشاشة / Clear screen
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في مسح الشاشة / Error clearing screen: " << e.what() << std::endl;
    }
}

/// رسم خط / Draw line
void renderer_draw_line_impl(int rendererId, float x1, float y1, float x2, float y2,
                             int r, int g, int b, int a) {
    try {
        auto rendererData = GetRenderer(rendererId);     // الحصول على بيانات الرسام / Get renderer data
        if (rendererData->renderer) {                    // التحقق من وجود الرسام / Check renderer exists
            // إنشاء كائن لون / Create color object
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);
            
            // رسم الخط / Draw the line
            rendererData->renderer->DrawLine(x1, y1, x2, y2, color);
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في رسم الخط / Error drawing line: " << e.what() << std::endl;
    }
}

/// رسم مستطيل / Draw rectangle
void renderer_draw_rect_impl(int rendererId, float x, float y, float width, float height,
                             int r, int g, int b, int a, bool filled) {
    try {
        auto rendererData = GetRenderer(rendererId);     // الحصول على بيانات الرسام / Get renderer data
        if (rendererData->renderer) {                    // التحقق من وجود الرسام / Check renderer exists
            // إنشاء كائن لون / Create color object
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);
            
            // رسم المستطيل / Draw the rectangle
            rendererData->renderer->DrawRect(x, y, width, height, color, filled);
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في رسم المستطيل / Error drawing rectangle: " << e.what() << std::endl;
    }
}

/// رسم دائرة / Draw circle
void renderer_draw_circle_impl(int rendererId, float x, float y, float radius,
                               int r, int g, int b, int a, bool filled) {
    try {
        auto rendererData = GetRenderer(rendererId);     // الحصول على بيانات الرسام / Get renderer data
        if (rendererData->renderer) {                    // التحقق من وجود الرسام / Check renderer exists
            // إنشاء كائن لون / Create color object
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);
            
            // رسم الدائرة / Draw the circle
            rendererData->renderer->DrawCircle(x, y, radius, color, filled);
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في رسم الدائرة / Error drawing circle: " << e.what() << std::endl;
    }
}

/// رسم نص / Draw text
void renderer_draw_text_impl(int rendererId, const std::string& text, float x, float y,
                             float size, int r, int g, int b, int a) {
    try {
        auto rendererData = GetRenderer(rendererId);     // الحصول على بيانات الرسام / Get renderer data
        if (rendererData->renderer) {                    // التحقق من وجود الرسام / Check renderer exists
            // إنشاء كائن لون / Create color object
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);

            // محاولة رسم النص الحقيقي عبر نظام الخطوط / Render real text via font system
            auto font = GetOrCreateDefaultFont(size);
            if (font && font->IsValid()) {
                rendererData->renderer->DrawTextAuto(text, font, x, y, color);
            } else {
                // fallback واضح بدل السبام / Explicit fallback without noisy placeholder spam
                std::cerr << "تعذر تحميل خط افتراضي لرسم النص / Failed to load default font for text rendering" << std::endl;
            }
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في رسم النص / Error drawing text: " << e.what() << std::endl;
    }
}


// ============================================================================
// UI Widget Implementation - تنفيذ عناصر الواجهة
// ============================================================================

/// إنشاء تسمية نصية / Create label
int label_create_impl(const std::string& text, float x, float y, float width, float height) {
    try {
        // إنشاء بيانات العنصر / Create widget data
        WidgetData widgetData;                           // بيانات العنصر / Widget data
        widgetData.type = "label";                       // نوع العنصر / Widget type
        widgetData.text = text;                          // النص / Text
        widgetData.x = x;                                // الموقع الأفقي / X position
        widgetData.y = y;                                // الموقع العمودي / Y position
        widgetData.width = width;                        // العرض / Width
        widgetData.height = height;                      // الارتفاع / Height
        widgetData.state = false;                        // الحالة غير مستخدمة / State unused
        
        // حفظ وتوليد المعرف / Store and generate ID
        int widgetId = g_nextId++;                       // توليد معرف فريد / Generate unique ID
        g_widgets[widgetId] = widgetData;                // حفظ في الخريطة / Store in map
        
        std::cout << "تم إنشاء تسمية: \"" << text << "\" في (" << x << "," << y << ") - ID: " << widgetId << std::endl;
        std::cout << "Created label: \"" << text << "\" at (" << x << "," << y << ") - ID: " << widgetId << std::endl;
        
        return widgetId;                                 // إرجاع المعرف / Return ID
    }
    catch (const std::exception& e) {                    // معالجة الاستثناءات / Handle exceptions
        std::cerr << "خطأ في إنشاء التسمية / Error creating label: " << e.what() << std::endl;
        return -1;                                       // إرجاع خطأ / Return error
    }
}

/// ضبط نص التسمية / Set label text
void label_set_text_impl(int labelId, const std::string& text) {
    try {
        auto widgetData = GetWidget(labelId);            // الحصول على بيانات العنصر / Get widget data
        if (widgetData->type == "label") {               // التحقق من نوع العنصر / Verify widget type
            widgetData->text = text;                     // تحديث النص / Update text
            std::cout << "تم تحديث نص التسمية ID: " << labelId << " -> \"" << text << "\"" << std::endl;
        } else {
            std::cerr << "خطأ: المعرف ليس تسمية / Error: ID is not a label" << std::endl;
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في ضبط نص التسمية / Error setting label text: " << e.what() << std::endl;
    }
}

/// الحصول على نص التسمية / Get label text
std::string label_get_text_impl(int labelId) {
    try {
        auto widgetData = GetWidget(labelId);            // الحصول على بيانات العنصر / Get widget data
        if (widgetData->type == "label") {               // التحقق من نوع العنصر / Verify widget type
            return widgetData->text;                     // إرجاع النص / Return text
        }
        std::cerr << "خطأ: المعرف ليس تسمية / Error: ID is not a label" << std::endl;
        return "";                                       // إرجاع نص فارغ / Return empty string
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في الحصول على نص التسمية / Error getting label text: " << e.what() << std::endl;
        return "";                                       // إرجاع نص فارغ / Return empty string
    }
}

/// إنشاء زر / Create button
int button_create_impl(const std::string& text, float x, float y, float width, float height) {
    try {
        // إنشاء بيانات الزر / Create button data
        WidgetData widgetData;                           // بيانات العنصر / Widget data
        widgetData.type = "button";                      // نوع العنصر / Widget type
        widgetData.text = text;                          // نص الزر / Button text
        widgetData.x = x;                                // الموقع الأفقي / X position
        widgetData.y = y;                                // الموقع العمودي / Y position
        widgetData.width = width;                        // العرض / Width
        widgetData.height = height;                      // الارتفاع / Height
        widgetData.state = false;                        // الزر غير مضغوط / Button not pressed
        
        // حفظ وتوليد المعرف / Store and generate ID
        int widgetId = g_nextId++;                       // توليد معرف فريد / Generate unique ID
        g_widgets[widgetId] = widgetData;                // حفظ في الخريطة / Store in map
        
        std::cout << "تم إنشاء زر: \"" << text << "\" في (" << x << "," << y << ") - ID: " << widgetId << std::endl;
        std::cout << "Created button: \"" << text << "\" at (" << x << "," << y << ") - ID: " << widgetId << std::endl;
        
        return widgetId;                                 // إرجاع المعرف / Return ID
    }
    catch (const std::exception& e) {                    // معالجة الاستثناءات / Handle exceptions
        std::cerr << "خطأ في إنشاء الزر / Error creating button: " << e.what() << std::endl;
        return -1;                                       // إرجاع خطأ / Return error
    }
}

/// فحص إذا تم الضغط على الزر / Check if button is clicked
bool button_is_clicked_impl(int buttonId) {
    try {
        auto widgetData = GetWidget(buttonId);           // الحصول على بيانات العنصر / Get widget data
        if (widgetData->type == "button") {              // التحقق من نوع العنصر / Verify widget type
            // مؤقتاً إرجاع false (يحتاج تطبيق معالجة الأحداث) / Temporarily return false (needs event handling)
            return widgetData->state;                    // إرجاع حالة الزر / Return button state
        }
        std::cerr << "خطأ: المعرف ليس زر / Error: ID is not a button" << std::endl;
        return false;                                    // إرجاع false / Return false
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في فحص الزر / Error checking button: " << e.what() << std::endl;
        return false;                                    // إرجاع false / Return false
    }
}

/// ضبط نص الزر / Set button text
void button_set_text_impl(int buttonId, const std::string& text) {
    try {
        auto widgetData = GetWidget(buttonId);           // الحصول على بيانات العنصر / Get widget data
        if (widgetData->type == "button") {              // التحقق من نوع العنصر / Verify widget type
            widgetData->text = text;                     // تحديث النص / Update text
            std::cout << "تم تحديث نص الزر ID: " << buttonId << " -> \"" << text << "\"" << std::endl;
        } else {
            std::cerr << "خطأ: المعرف ليس زر / Error: ID is not a button" << std::endl;
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في ضبط نص الزر / Error setting button text: " << e.what() << std::endl;
    }
}

/// إنشاء حقل إدخال نص / Create text input
int textinput_create_impl(const std::string& placeholder, float x, float y,
                         float width, float height) {
    try {
        // إنشاء بيانات حقل الإدخال / Create input field data
        WidgetData widgetData;                           // بيانات العنصر / Widget data
        widgetData.type = "textinput";                   // نوع العنصر / Widget type
        widgetData.text = "";                            // النص فارغ في البداية / Empty text initially
        widgetData.x = x;                                // الموقع الأفقي / X position
        widgetData.y = y;                                // الموقع العمودي / Y position
        widgetData.width = width;                        // العرض / Width
        widgetData.height = height;                      // الارتفاع / Height
        widgetData.state = false;                        // غير نشط / Not active
        
        // حفظ وتوليد المعرف / Store and generate ID
        int widgetId = g_nextId++;                       // توليد معرف فريد / Generate unique ID
        g_widgets[widgetId] = widgetData;                // حفظ في الخريطة / Store in map
        
        std::cout << "تم إنشاء حقل إدخال: placeholder=\"" << placeholder << "\" في (" << x << "," << y << ") - ID: " << widgetId << std::endl;
        
        return widgetId;                                 // إرجاع المعرف / Return ID
    }
    catch (const std::exception& e) {                    // معالجة الاستثناءات / Handle exceptions
        std::cerr << "خطأ في إنشاء حقل الإدخال / Error creating text input: " << e.what() << std::endl;
        return -1;                                       // إرجاع خطأ / Return error
    }
}

/// الحصول على قيمة حقل الإدخال / Get text input value
std::string textinput_get_value_impl(int inputId) {
    try {
        auto widgetData = GetWidget(inputId);            // الحصول على بيانات العنصر / Get widget data
        if (widgetData->type == "textinput") {           // التحقق من نوع العنصر / Verify widget type
            return widgetData->text;                     // إرجاع النص / Return text
        }
        std::cerr << "خطأ: المعرف ليس حقل إدخال / Error: ID is not a text input" << std::endl;
        return "";                                       // إرجاع نص فارغ / Return empty string
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في الحصول على قيمة الإدخال / Error getting input value: " << e.what() << std::endl;
        return "";                                       // إرجاع نص فارغ / Return empty string
    }
}

/// ضبط قيمة حقل الإدخال / Set text input value
void textinput_set_value_impl(int inputId, const std::string& value) {
    try {
        auto widgetData = GetWidget(inputId);            // الحصول على بيانات العنصر / Get widget data
        if (widgetData->type == "textinput") {           // التحقق من نوع العنصر / Verify widget type
            widgetData->text = value;                    // تحديث القيمة / Update value
            std::cout << "تم تحديث قيمة حقل الإدخال ID: " << inputId << " -> \"" << value << "\"" << std::endl;
        } else {
            std::cerr << "خطأ: المعرف ليس حقل إدخال / Error: ID is not a text input" << std::endl;
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في ضبط قيمة الإدخال / Error setting input value: " << e.what() << std::endl;
    }
}

/// إنشاء مربع اختيار / Create checkbox
int checkbox_create_impl(const std::string& label, float x, float y,
                        float width, float height) {
    try {
        // إنشاء بيانات مربع الاختيار / Create checkbox data
        WidgetData widgetData;                           // بيانات العنصر / Widget data
        widgetData.type = "checkbox";                    // نوع العنصر / Widget type
        widgetData.text = label;                         // نص التسمية / Label text
        widgetData.x = x;                                // الموقع الأفقي / X position
        widgetData.y = y;                                // الموقع العمودي / Y position
        widgetData.width = width;                        // العرض / Width
        widgetData.height = height;                      // الارتفاع / Height
        widgetData.state = false;                        // غير محدد / Unchecked
        
        // حفظ وتوليد المعرف / Store and generate ID
        int widgetId = g_nextId++;                       // توليد معرف فريد / Generate unique ID
        g_widgets[widgetId] = widgetData;                // حفظ في الخريطة / Store in map
        
        std::cout << "تم إنشاء مربع اختيار: \"" << label << "\" في (" << x << "," << y << ") - ID: " << widgetId << std::endl;
        
        return widgetId;                                 // إرجاع المعرف / Return ID
    }
    catch (const std::exception& e) {                    // معالجة الاستثناءات / Handle exceptions
        std::cerr << "خطأ في إنشاء مربع الاختيار / Error creating checkbox: " << e.what() << std::endl;
        return -1;                                       // إرجاع خطأ / Return error
    }
}

/// فحص إذا كان مربع الاختيار محدداً / Check if checkbox is checked
bool checkbox_is_checked_impl(int checkboxId) {
    try {
        auto widgetData = GetWidget(checkboxId);         // الحصول على بيانات العنصر / Get widget data
        if (widgetData->type == "checkbox") {            // التحقق من نوع العنصر / Verify widget type
            return widgetData->state;                    // إرجاع حالة التحديد / Return checked state
        }
        std::cerr << "خطأ: المعرف ليس مربع اختيار / Error: ID is not a checkbox" << std::endl;
        return false;                                    // إرجاع false / Return false
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في فحص مربع الاختيار / Error checking checkbox: " << e.what() << std::endl;
        return false;                                    // إرجاع false / Return false
    }
}

/// ضبط حالة مربع الاختيار / Set checkbox checked state
void checkbox_set_checked_impl(int checkboxId, bool checked) {
    try {
        auto widgetData = GetWidget(checkboxId);         // الحصول على بيانات العنصر / Get widget data
        if (widgetData->type == "checkbox") {            // التحقق من نوع العنصر / Verify widget type
            widgetData->state = checked;                 // تحديث الحالة / Update state
            std::cout << "تم تحديث حالة مربع الاختيار ID: " << checkboxId 
                      << " -> " << (checked ? "محدد / checked" : "غير محدد / unchecked") << std::endl;
        } else {
            std::cerr << "خطأ: المعرف ليس مربع اختيار / Error: ID is not a checkbox" << std::endl;
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في ضبط حالة مربع الاختيار / Error setting checkbox state: " << e.what() << std::endl;
    }
}

/// رسم عنصر واجهة / Draw widget
void widget_draw_impl(int widgetId, int rendererId) {
    try {
        auto widgetData = GetWidget(widgetId);           // الحصول على بيانات العنصر / Get widget data
        auto rendererData = GetRenderer(rendererId);     // الحصول على بيانات الرسام / Get renderer data
        
        if (!rendererData->renderer) {                   // التحقق من وجود الرسام / Check renderer exists
            std::cerr << "خطأ: رسام غير صالح / Error: Invalid renderer" << std::endl;
            return;
        }
        
        // رسم حسب نوع العنصر / Draw based on widget type
        sad::graphics::Color bgColor = sad::graphics::Color::FromBytes(200, 200, 200, 255);    // لون الخلفية / Background color
        sad::graphics::Color textColor = sad::graphics::Color::FromBytes(0, 0, 0, 255);        // لون النص / Text color
        
        // رسم خلفية العنصر / Draw widget background
        rendererData->renderer->DrawRect(widgetData->x, widgetData->y, 
                                        widgetData->width, widgetData->height, 
                                        bgColor, true);
        
        // رسم حدود العنصر / Draw widget border
        sad::graphics::Color borderColor = sad::graphics::Color::FromBytes(100, 100, 100, 255); // لون الحدود / Border color
        rendererData->renderer->DrawRect(widgetData->x, widgetData->y,
                                        widgetData->width, widgetData->height,
                                        borderColor, false);
        
        std::cout << "تم رسم العنصر ID: " << widgetId << " نوع: " << widgetData->type << std::endl;
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في رسم العنصر / Error drawing widget: " << e.what() << std::endl;
    }
}

/// تحديث عنصر واجهة / Update widget
void widget_update_impl(int widgetId, float deltaTime) {
    try {
        auto widgetData = GetWidget(widgetId);           // الحصول على بيانات العنصر / Get widget data
        // تحديث العنصر (مؤقتاً لا يوجد شيء للتحديث) / Update widget (temporarily nothing to update)
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في تحديث العنصر / Error updating widget: " << e.what() << std::endl;
    }
}

/// معالجة حدث الماوس لعنصر الواجهة / Handle mouse event for widget
void widget_mouse_event_impl(int widgetId, int x, int y, bool pressed) {
    try {
        auto widgetData = GetWidget(widgetId);           // الحصول على بيانات العنصر / Get widget data
        
        // فحص إذا كان الماوس داخل حدود العنصر / Check if mouse is within widget bounds
        bool inside = (x >= widgetData->x && x <= widgetData->x + widgetData->width &&
                      y >= widgetData->y && y <= widgetData->y + widgetData->height);
        
        if (inside && pressed) {                         // إذا كان الضغط داخل العنصر / If clicked inside widget
            if (widgetData->type == "button") {          // إذا كان زر / If button
                widgetData->state = true;                // تفعيل الزر / Activate button
                std::cout << "تم الضغط على الزر ID: " << widgetId << std::endl;
            } else if (widgetData->type == "checkbox") { // إذا كان مربع اختيار / If checkbox
                widgetData->state = !widgetData->state;  // عكس الحالة / Toggle state
                std::cout << "تم تبديل مربع الاختيار ID: " << widgetId << std::endl;
            }
        } else if (!pressed && widgetData->type == "button") {  // عند تحرير الزر / On button release
            widgetData->state = false;                   // إلغاء تفعيل الزر / Deactivate button
        }
    }
    catch (const std::exception& e) {                    // معالجة الأخطاء / Handle errors
        std::cerr << "خطأ في معالجة حدث الماوس / Error handling mouse event: " << e.what() << std::endl;
    }
}

// ============================================================================
// Input Handling Implementation - تنفيذ معالجة المدخلات
// ============================================================================

bool key_is_pressed_impl(int keycode) {
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    if (keycode >= 0 && keycode < SDL_NUM_SCANCODES) {
        return state[keycode] != 0;
    }
    return false;
}

int mouse_get_x_impl() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return x;
}

int mouse_get_y_impl() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return y;
}

bool mouse_button_pressed_impl(int button) {
    Uint32 state = SDL_GetMouseState(nullptr, nullptr);
    switch (button) {
        case 1: return (state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        case 2: return (state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
        case 3: return (state & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
        default: return false;
    }
}

// ============================================================================
// Texture / Image Management Implementation
// ============================================================================

struct TextureData {
    std::shared_ptr<sad::graphics::Texture> texture;
    int width = 0;
    int height = 0;
};
static std::map<int, TextureData> g_textures;

int texture_load_impl(const std::string& filePath) {
    try {
        auto tex = sad::graphics::Texture::CreateFromFile(filePath);
        if (tex && tex->IsValid()) {
            int id = g_nextId++;
            TextureData td;
            td.texture = tex;
            td.width = static_cast<int>(tex->GetWidth());
            td.height = static_cast<int>(tex->GetHeight());
            g_textures[id] = td;
            return id;
        }
    } catch (const std::exception& e) {
        std::cerr << "خطأ في تحميل النسيج / Error loading texture: " << e.what() << std::endl;
    }
    return -1;
}

void texture_draw_impl(int rendererId, int textureId, float x, float y, float width, float height) {
    try {
        auto renderer = GetRenderer(rendererId);
        auto texIt = g_textures.find(textureId);
        if (texIt == g_textures.end()) {
            throw std::runtime_error("معرف نسيج غير صالح / Invalid texture ID");
        }
        // Draw a textured quad using the renderer
        // For now, draw a placeholder rect (actual texture binding requires Renderer2D texture support)
        if (renderer->renderer) {
            sad::graphics::Color white(1.0f, 1.0f, 1.0f, 1.0f);
            renderer->renderer->DrawFilledRect(x, y, width, height, white);
        }
    } catch (const std::exception& e) {
        std::cerr << "خطأ في رسم النسيج / Error drawing texture: " << e.what() << std::endl;
    }
}

void texture_unload_impl(int textureId) {
    g_textures.erase(textureId);
}

// ============================================================================
// Audio System Implementation
// ============================================================================

static bool g_audioInitialized = false;
static sad::graphics::AudioSystem* g_audioSystem = nullptr;

bool audio_init_impl() {
    if (g_audioInitialized) return true;
    g_audioSystem = &sad::graphics::AudioSystem::GetInstance();
    g_audioInitialized = g_audioSystem->Initialize();
    return g_audioInitialized;
}

int sound_load_impl(const std::string& filePath, const std::string& name) {
    if (!g_audioInitialized) {
        if (!audio_init_impl()) return -1;
    }
    auto id = g_audioSystem->LoadSound(filePath, name);
    return static_cast<int>(id);
}

int sound_play_impl(int soundId) {
    if (!g_audioSystem) return -1;
    auto channelId = g_audioSystem->Play(static_cast<sad::graphics::SoundId>(soundId));
    return static_cast<int>(channelId);
}

void sound_stop_impl(int channelId) {
    if (g_audioSystem) {
        g_audioSystem->Stop(static_cast<sad::graphics::ChannelId>(channelId));
    }
}

void sound_set_volume_impl(int channelId, float volume) {
    if (g_audioSystem) {
        g_audioSystem->SetVolume(static_cast<sad::graphics::ChannelId>(channelId), volume);
    }
}

void audio_set_master_volume_impl(float volume) {
    if (g_audioSystem) {
        g_audioSystem->SetMasterVolume(volume);
    }
}

// ============================================================================
// Drawing Shapes Implementation
// ============================================================================

void renderer_draw_triangle_impl(int rendererId, float x1, float y1,
                                  float x2, float y2, float x3, float y3,
                                  int r, int g, int b, int a, bool filled) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) {
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);
            renderer->renderer->DrawTriangle(x1, y1, x2, y2, x3, y3, color, filled);
        }
    } catch (const std::exception& e) {
        std::cerr << "خطأ في رسم المثلث / Error drawing triangle: " << e.what() << std::endl;
    }
}

void renderer_draw_rounded_rect_impl(int rendererId, float x, float y, float w, float h,
                                      float radius, int r, int g, int b, int a, bool filled) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) {
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);
            // Use DrawRect as fallback since DrawRoundedRect may not exist
            renderer->renderer->DrawRect(x, y, w, h, color, filled);
        }
    } catch (const std::exception& e) {
        std::cerr << "خطأ في رسم المستطيل الدائري / Error drawing rounded rect: " << e.what() << std::endl;
    }
}

// ============================================================================
// Extended Drawing Implementations
// ============================================================================

void renderer_draw_ellipse_impl(int rendererId, float cx, float cy, float rx, float ry,
                                int r, int g, int b, int a, bool filled) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) {
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);
            renderer->renderer->DrawEllipse(cx, cy, rx, ry, color, filled);
        }
    } catch (const std::exception& e) {
        std::cerr << "خطأ في رسم القطع الناقص / Error drawing ellipse: " << e.what() << std::endl;
    }
}

void renderer_draw_arc_impl(int rendererId, float cx, float cy, float radius,
                            float startAngle, float endAngle, int r, int g, int b, int a) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) {
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);
            renderer->renderer->DrawArc(cx, cy, radius, startAngle, endAngle, color);
        }
    } catch (const std::exception& e) {
        std::cerr << "خطأ في رسم القوس / Error drawing arc: " << e.what() << std::endl;
    }
}

void renderer_draw_rect_outline_impl(int rendererId, float x, float y, float w, float h,
                                      int fillR, int fillG, int fillB, int fillA,
                                      int outR, int outG, int outB, int outA,
                                      float outlineThickness) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) {
            sad::graphics::Color fillColor = sad::graphics::Color::FromBytes(fillR, fillG, fillB, fillA);
            sad::graphics::Color outlineColor = sad::graphics::Color::FromBytes(outR, outG, outB, outA);
            renderer->renderer->DrawRectOutline(x, y, w, h, fillColor, outlineColor, outlineThickness);
        }
    } catch (const std::exception& e) {
        std::cerr << "خطأ في رسم المستطيل المحدد / Error drawing rect outline: " << e.what() << std::endl;
    }
}

// ============================================================================
// Transformation Implementations
// ============================================================================

void renderer_push_transform_impl(int rendererId) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) renderer->renderer->PushTransform();
    } catch (const std::exception& e) {
        std::cerr << "خطأ في حفظ التحويل / Error pushing transform: " << e.what() << std::endl;
    }
}

void renderer_pop_transform_impl(int rendererId) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) renderer->renderer->PopTransform();
    } catch (const std::exception& e) {
        std::cerr << "خطأ في استرجاع التحويل / Error popping transform: " << e.what() << std::endl;
    }
}

void renderer_translate_impl(int rendererId, float x, float y) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) renderer->renderer->Translate(x, y);
    } catch (const std::exception& e) {
        std::cerr << "خطأ في التحريك / Error translating: " << e.what() << std::endl;
    }
}

void renderer_rotate_impl(int rendererId, float angle) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) renderer->renderer->Rotate(angle);
    } catch (const std::exception& e) {
        std::cerr << "خطأ في التدوير / Error rotating: " << e.what() << std::endl;
    }
}

void renderer_scale_impl(int rendererId, float sx, float sy) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) renderer->renderer->Scale(sx, sy);
    } catch (const std::exception& e) {
        std::cerr << "خطأ في القياس / Error scaling: " << e.what() << std::endl;
    }
}

void renderer_reset_transform_impl(int rendererId) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) renderer->renderer->ResetTransform();
    } catch (const std::exception& e) {
        std::cerr << "خطأ في إعادة التحويل / Error resetting transform: " << e.what() << std::endl;
    }
}

// ============================================================================
// Renderer Settings Implementations
// ============================================================================

void renderer_set_draw_color_impl(int rendererId, int r, int g, int b, int a) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) {
            sad::graphics::Color color = sad::graphics::Color::FromBytes(r, g, b, a);
            renderer->renderer->SetDrawColor(color);
        }
    } catch (const std::exception& e) {
        std::cerr << "خطأ في تعيين اللون / Error setting draw color: " << e.what() << std::endl;
    }
}

void renderer_set_line_thickness_impl(int rendererId, float thickness) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) renderer->renderer->SetLineThickness(thickness);
    } catch (const std::exception& e) {
        std::cerr << "خطأ في تعيين سمك الخط / Error setting line thickness: " << e.what() << std::endl;
    }
}

void renderer_set_viewport_impl(int rendererId, int x, int y, int w, int h) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) renderer->renderer->SetViewport(x, y, w, h);
    } catch (const std::exception& e) {
        std::cerr << "خطأ في تعيين منطقة العرض / Error setting viewport: " << e.what() << std::endl;
    }
}

void renderer_set_projection_impl(int rendererId, float left, float right, float bottom, float top) {
    try {
        auto renderer = GetRenderer(rendererId);
        if (renderer->renderer) renderer->renderer->SetOrthographicProjection(left, right, bottom, top);
    } catch (const std::exception& e) {
        std::cerr << "خطأ في تعيين الإسقاط / Error setting projection: " << e.what() << std::endl;
    }
}

// ============================================================================
// Window Properties Implementation
// ============================================================================

int window_get_width_impl(int windowId) {
    try {
        auto win = GetWindow(windowId);
        if (win->window) return static_cast<int>(win->window->GetWidth());
    } catch (...) {}
    return 0;
}

int window_get_height_impl(int windowId) {
    try {
        auto win = GetWindow(windowId);
        if (win->window) return static_cast<int>(win->window->GetHeight());
    } catch (...) {}
    return 0;
}

void window_set_title_impl(int windowId, const std::string& title) {
    try {
        auto win = GetWindow(windowId);
        if (win->window) win->window->SetTitle(title);
    } catch (const std::exception& e) {
        std::cerr << "خطأ في تعيين عنوان النافذة / Error setting title: " << e.what() << std::endl;
    }
}

// ============================================================================
// Time Utilities Implementation
// ============================================================================

int get_ticks_impl() {
    return static_cast<int>(SDL_GetTicks());
}

} // namespace graphics
} // namespace stdlib
} // namespace sad
