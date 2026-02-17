// ============================================================================
// window.cpp - تطبيق نظام النوافذ
// Window System Implementation
// ============================================================================

#include "window/window.h"
#include <SDL.h>           // مكتبة SDL2 الأساسية / Core SDL2 library
#include <SDL_syswm.h>     // معلومات النظام / System information
#include <atomic>               // للعمليات الذرية / Atomic operations
#include <stdexcept>            // للاستثناءات / Exceptions

// Remove Win32 macro collisions after all includes
#ifdef CreateWindow
#undef CreateWindow
#endif

namespace sad {
namespace graphics {

// ============================================================================
// متغيرات عامة / Global Variables
// ============================================================================

/// عداد ذري للمعرفات الفريدة / Atomic counter for unique handles
static std::atomic<UInt64> s_handleCounter{1};

// ============================================================================
// تطبيق Window / Window Implementation
// ============================================================================

// ----------------------------------------------------------------------------
// المنشئ / Constructor
// ----------------------------------------------------------------------------
Window::Window(const String& title, Int32 width, Int32 height, WindowFlags flags)
    : m_handle(GenerateHandle())              // توليد معرف فريد / Generate unique handle
    , m_nativeWindow(nullptr)                 // تهيئة مؤشر النافذة / Initialize window pointer
    , m_title(title)                          // حفظ العنوان / Store title
    , m_width(width)                          // حفظ العرض / Store width
    , m_height(height)                        // حفظ الارتفاع / Store height
    , m_posX(SDL_WINDOWPOS_CENTERED)          // مركز الشاشة أفقياً / Center horizontally
    , m_posY(SDL_WINDOWPOS_CENTERED)          // مركز الشاشة عمودياً / Center vertically
    , m_isOpen(false)                         // النافذة مغلقة مبدئياً / Window initially closed
    , m_isVisible(false)                      // النافذة مخفية مبدئياً / Window initially hidden
    , m_isFocused(false)                      // النافذة غير مركزة مبدئياً / Window initially unfocused
    , m_isMinimized(false)                    // النافذة غير مصغرة / Window not minimized
    , m_isMaximized(false)                    // النافذة غير مكبرة / Window not maximized
    , m_isFullscreen(false)                   // النافذة ليست في ملء الشاشة / Window not fullscreen
{
    // إنشاء النافذة الأساسية / Create native window
    if (!CreateNativeWindow(flags)) {
        throw std::runtime_error("Failed to create window: " + title); // فشل إنشاء النافذة / Failed to create window
    }
    
    m_isOpen = true;  // النافذة الآن مفتوحة / Window is now open
}

// ----------------------------------------------------------------------------
// المدمر / Destructor
// ----------------------------------------------------------------------------
Window::~Window() {
    DestroyNativeWindow(); // تدمير النافذة الأساسية / Destroy native window
}

// ----------------------------------------------------------------------------
// منشئ النقل / Move Constructor
// ----------------------------------------------------------------------------
Window::Window(Window&& other) noexcept
    : m_handle(other.m_handle)
    , m_nativeWindow(other.m_nativeWindow)
    , m_title(std::move(other.m_title))
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_posX(other.m_posX)
    , m_posY(other.m_posY)
    , m_isOpen(other.m_isOpen)
    , m_isVisible(other.m_isVisible)
    , m_isFocused(other.m_isFocused)
    , m_isMinimized(other.m_isMinimized)
    , m_isMaximized(other.m_isMaximized)
    , m_isFullscreen(other.m_isFullscreen)
    , m_onClose(std::move(other.m_onClose))
    , m_onResize(std::move(other.m_onResize))
    , m_onMove(std::move(other.m_onMove))
    , m_onFocus(std::move(other.m_onFocus))
    , m_onBlur(std::move(other.m_onBlur))
{
    other.m_nativeWindow = nullptr; // تصفير المؤشر في الكائن القديم / Nullify pointer in old object
    other.m_isOpen = false;         // إغلاق النافذة القديمة / Close old window
}

// ----------------------------------------------------------------------------
// مُعامل النقل / Move Assignment
// ----------------------------------------------------------------------------
Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {  // فحص النسخ الذاتي / Check for self-assignment
        DestroyNativeWindow(); // تدمير النافذة الحالية / Destroy current window
        
        // نقل البيانات / Transfer data
        m_handle = other.m_handle;
        m_nativeWindow = other.m_nativeWindow;
        m_title = std::move(other.m_title);
        m_width = other.m_width;
        m_height = other.m_height;
        m_posX = other.m_posX;
        m_posY = other.m_posY;
        m_isOpen = other.m_isOpen;
        m_isVisible = other.m_isVisible;
        m_isFocused = other.m_isFocused;
        m_isMinimized = other.m_isMinimized;
        m_isMaximized = other.m_isMaximized;
        m_isFullscreen = other.m_isFullscreen;
        m_onClose = std::move(other.m_onClose);
        m_onResize = std::move(other.m_onResize);
        m_onMove = std::move(other.m_onMove);
        m_onFocus = std::move(other.m_onFocus);
        m_onBlur = std::move(other.m_onBlur);
        
        // تصفير الكائن القديم / Nullify old object
        other.m_nativeWindow = nullptr;
        other.m_isOpen = false;
    }
    return *this;
}

// ============================================================================
// الدوال العامة / Public Methods
// ============================================================================

// ----------------------------------------------------------------------------
// عرض النافذة / Show Window
// ----------------------------------------------------------------------------
void Window::Show() {
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_ShowWindow(m_nativeWindow); // عرض النافذة / Show window
        m_isVisible = true;          // تحديث الحالة / Update state
    }
}

// ----------------------------------------------------------------------------
// إخفاء النافذة / Hide Window
// ----------------------------------------------------------------------------
void Window::Hide() {
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_HideWindow(m_nativeWindow); // إخفاء النافذة / Hide window
        m_isVisible = false;         // تحديث الحالة / Update state
    }
}

// ----------------------------------------------------------------------------
// إغلاق النافذة / Close Window
// ----------------------------------------------------------------------------
void Window::Close() {
    if (m_isOpen) {                  // التحقق من أن النافذة مفتوحة / Check if window is open
        m_isOpen = false;            // تحديث الحالة / Update state
        
        if (m_onClose) {             // إذا كان هناك رد نداء للإغلاق / If close callback exists
            m_onClose();             // استدعاء رد النداء / Call callback
        }
    }
}

// ----------------------------------------------------------------------------
// رفع النافذة للأمام / Raise Window
// ----------------------------------------------------------------------------
void Window::Raise() {
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_RaiseWindow(m_nativeWindow); // رفع النافذة / Raise window
    }
}

// ----------------------------------------------------------------------------
// تركيز النافذة / Focus Window
// ----------------------------------------------------------------------------
void Window::Focus() {
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_RaiseWindow(m_nativeWindow); // رفع النافذة (يفعّل التركيز أيضاً) / Raise window (also focuses)
        m_isFocused = true;          // تحديث الحالة / Update state
    }
}

// ----------------------------------------------------------------------------
// تصغير النافذة / Minimize Window
// ----------------------------------------------------------------------------
void Window::Minimize() {
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_MinimizeWindow(m_nativeWindow); // تصغير النافذة / Minimize window
        m_isMinimized = true;        // تحديث الحالة / Update state
        m_isMaximized = false;       // إلغاء التكبير / Cancel maximized
    }
}

// ----------------------------------------------------------------------------
// تكبير النافذة / Maximize Window
// ----------------------------------------------------------------------------
void Window::Maximize() {
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_MaximizeWindow(m_nativeWindow); // تكبير النافذة / Maximize window
        m_isMaximized = true;        // تحديث الحالة / Update state
        m_isMinimized = false;       // إلغاء التصغير / Cancel minimized
    }
}

// ----------------------------------------------------------------------------
// استعادة الحجم الطبيعي / Restore Normal Size
// ----------------------------------------------------------------------------
void Window::Restore() {
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_RestoreWindow(m_nativeWindow); // استعادة الحجم / Restore size
        m_isMinimized = false;       // إلغاء التصغير / Cancel minimized
        m_isMaximized = false;       // إلغاء التكبير / Cancel maximized
    }
}

// ----------------------------------------------------------------------------
// ضبط العنوان / Set Title
// ----------------------------------------------------------------------------
void Window::SetTitle(const String& title) {
    m_title = title;                 // حفظ العنوان الجديد / Store new title
    
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_SetWindowTitle(m_nativeWindow, title.c_str()); // ضبط عنوان النافذة / Set window title
    }
}

// ----------------------------------------------------------------------------
// ضبط الحجم / Set Size
// ----------------------------------------------------------------------------
void Window::SetSize(Int32 width, Int32 height) {
    m_width = width;                 // حفظ العرض الجديد / Store new width
    m_height = height;               // حفظ الارتفاع الجديد / Store new height
    
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_SetWindowSize(m_nativeWindow, width, height); // ضبط حجم النافذة / Set window size
    }
}

// ----------------------------------------------------------------------------
// ضبط الموقع / Set Position
// ----------------------------------------------------------------------------
void Window::SetPosition(Int32 x, Int32 y) {
    m_posX = x;                      // حفظ الموقع الأفقي / Store X position
    m_posY = y;                      // حفظ الموقع العمودي / Store Y position
    
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_SetWindowPosition(m_nativeWindow, x, y); // ضبط موقع النافذة / Set window position
    }
}

// ----------------------------------------------------------------------------
// ضبط ملء الشاشة / Set Fullscreen
// ----------------------------------------------------------------------------
void Window::SetFullscreen(bool fullscreen) {
    m_isFullscreen = fullscreen;     // تحديث الحالة / Update state
    
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        Uint32 flags = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0; // تحديد الأعلام / Determine flags
        SDL_SetWindowFullscreen(m_nativeWindow, flags); // ضبط وضع ملء الشاشة / Set fullscreen mode
    }
}

// ----------------------------------------------------------------------------
// ضبط قابلية تغيير الحجم / Set Resizable
// ----------------------------------------------------------------------------
void Window::SetResizable(bool resizable) {
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_SetWindowResizable(m_nativeWindow, resizable ? SDL_TRUE : SDL_FALSE); // ضبط قابلية التغيير / Set resizable
    }
}

// ----------------------------------------------------------------------------
// ضبط الشفافية / Set Opacity
// ----------------------------------------------------------------------------
void Window::SetOpacity(Float32 opacity) {
    if (m_nativeWindow) {           // التحقق من وجود النافذة / Check window exists
        SDL_SetWindowOpacity(m_nativeWindow, opacity); // ضبط الشفافية / Set opacity
    }
}

// ============================================================================
// دوال الاستعلام / Query Methods
// ============================================================================

String Window::GetTitle() const { return m_title; }           // إرجاع العنوان / Return title
Int32 Window::GetWidth() const { return m_width; }            // إرجاع العرض / Return width
Int32 Window::GetHeight() const { return m_height; }          // إرجاع الارتفاع / Return height
Size2D Window::GetSize() const { return Size2D(static_cast<Float32>(m_width), static_cast<Float32>(m_height)); } // إرجاع الحجم / Return size
Point2D Window::GetPosition() const { return Point2D(static_cast<Float32>(m_posX), static_cast<Float32>(m_posY)); } // إرجاع الموقع / Return position
bool Window::IsVisible() const { return m_isVisible; }        // فحص الرؤية / Check visibility
bool Window::IsFocused() const { return m_isFocused; }        // فحص التركيز / Check focus
bool Window::IsMinimized() const { return m_isMinimized; }    // فحص التصغير / Check minimized
bool Window::IsMaximized() const { return m_isMaximized; }    // فحص التكبير / Check maximized
bool Window::IsFullscreen() const { return m_isFullscreen; }  // فحص ملء الشاشة / Check fullscreen
bool Window::IsOpen() const { return m_isOpen; }              // فحص الانفتاح / Check open

// ============================================================================
// معالجة الأحداث / Event Handling
// ============================================================================

void Window::OnClose(CallbackFunction callback) { m_onClose = callback; }           // تسجيل رد نداء الإغلاق / Register close callback
void Window::OnResize(CallbackWithParam<Size2D> callback) { m_onResize = callback; } // تسجيل رد نداء تغيير الحجم / Register resize callback
void Window::OnMove(CallbackWithParam<Point2D> callback) { m_onMove = callback; }   // تسجيل رد نداء التحريك / Register move callback
void Window::OnFocus(CallbackFunction callback) { m_onFocus = callback; }           // تسجيل رد نداء التركيز / Register focus callback
void Window::OnBlur(CallbackFunction callback) { m_onBlur = callback; }             // تسجيل رد نداء فقدان التركيز / Register blur callback

// ----------------------------------------------------------------------------
// معالجة الأحداث / Process Events
// ----------------------------------------------------------------------------
void Window::ProcessEvents() {
    SDL_Event event;                                    // متغير الحدث / Event variable
    
    while (SDL_PollEvent(&event)) {                     // حلقة معالجة الأحداث / Event processing loop
        HandleEvent(event);                             // معالجة الحدث / Handle event
    }
    
    UpdateState();                                      // تحديث الحالة / Update state
}

// ============================================================================
// الدوال الخاصة / Private Methods
// ============================================================================

// ----------------------------------------------------------------------------
// إنشاء النافذة الأساسية / Create Native Window
// ----------------------------------------------------------------------------
bool Window::CreateNativeWindow(WindowFlags flags) {
    Uint32 sdlFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;  // أعلام SDL الافتراضية + OpenGL / Default SDL flags + OpenGL
    
    // تحويل الأعلام المخصصة إلى أعلام SDL / Convert custom flags to SDL flags
    if (HasFlag(flags, WindowFlags::Fullscreen)) {
        sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;      // إضافة علم ملء الشاشة / Add fullscreen flag
        m_isFullscreen = true;
    }
    if (HasFlag(flags, WindowFlags::Resizable)) {
        sdlFlags |= SDL_WINDOW_RESIZABLE;               // إضافة علم قابل لتغيير الحجم / Add resizable flag
    }
    if (HasFlag(flags, WindowFlags::Borderless)) {
        sdlFlags |= SDL_WINDOW_BORDERLESS;              // إضافة علم بدون إطار / Add borderless flag
    }
    if (HasFlag(flags, WindowFlags::Hidden)) {
        sdlFlags |= SDL_WINDOW_HIDDEN;                  // إضافة علم مخفي / Add hidden flag
        sdlFlags &= ~SDL_WINDOW_SHOWN;                  // إزالة علم الظهور / Remove shown flag
        m_isVisible = false;
    } else {
        m_isVisible = true;
    }
    if (HasFlag(flags, WindowFlags::Minimized)) {
        sdlFlags |= SDL_WINDOW_MINIMIZED;               // إضافة علم مصغر / Add minimized flag
        m_isMinimized = true;
    }
    if (HasFlag(flags, WindowFlags::Maximized)) {
        sdlFlags |= SDL_WINDOW_MAXIMIZED;               // إضافة علم مكبر / Add maximized flag
        m_isMaximized = true;
    }
    if (HasFlag(flags, WindowFlags::HighDPI)) {
        sdlFlags |= SDL_WINDOW_ALLOW_HIGHDPI;           // إضافة علم DPI عالي / Add high DPI flag
    }
    if (HasFlag(flags, WindowFlags::AlwaysOnTop)) {
        sdlFlags |= SDL_WINDOW_ALWAYS_ON_TOP;           // إضافة علم دائماً في المقدمة / Add always on top flag
    }
    
    // إنشاء نافذة SDL / Create SDL window
    m_nativeWindow = SDL_CreateWindow(
        m_title.c_str(),    // العنوان / Title
        m_posX,             // الموقع الأفقي / X position
        m_posY,             // الموقع العمودي / Y position
        m_width,            // العرض / Width
        m_height,           // الارتفاع / Height
        sdlFlags            // الأعلام / Flags
    );
    
    return m_nativeWindow != nullptr; // إرجاع النجاح / Return success
}

// ----------------------------------------------------------------------------
// تدمير النافذة الأساسية / Destroy Native Window
// ----------------------------------------------------------------------------
void Window::DestroyNativeWindow() {
    if (m_nativeWindow) {               // التحقق من وجود النافذة / Check window exists
        SDL_DestroyWindow(m_nativeWindow); // تدمير نافذة SDL / Destroy SDL window
        m_nativeWindow = nullptr;        // تصفير المؤشر / Nullify pointer
    }
}

// ----------------------------------------------------------------------------
// معالجة حدث SDL / Handle SDL Event
// ----------------------------------------------------------------------------
void Window::HandleEvent(const SDL_Event& event) {
    // فحص أحداث النافذة / Check window events
    if (event.type == SDL_WINDOWEVENT) {
        if (event.window.windowID == SDL_GetWindowID(m_nativeWindow)) { // التحقق من أن الحدث لهذه النافذة / Check event is for this window
            
            switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:         // حدث إغلاق النافذة / Window close event
                    Close();
                    break;
                    
                case SDL_WINDOWEVENT_SIZE_CHANGED:  // حدث تغيير الحجم / Size changed event
                    m_width = event.window.data1;   // تحديث العرض / Update width
                    m_height = event.window.data2;  // تحديث الارتفاع / Update height
                    if (m_onResize) {               // إذا كان هناك رد نداء / If callback exists
                        m_onResize(GetSize());      // استدعاء رد النداء / Call callback
                    }
                    break;
                    
                case SDL_WINDOWEVENT_MOVED:         // حدث تحريك النافذة / Window moved event
                    m_posX = event.window.data1;    // تحديث الموقع الأفقي / Update X position
                    m_posY = event.window.data2;    // تحديث الموقع العمودي / Update Y position
                    if (m_onMove) {                 // إذا كان هناك رد نداء / If callback exists
                        m_onMove(GetPosition());    // استدعاء رد النداء / Call callback
                    }
                    break;
                    
                case SDL_WINDOWEVENT_FOCUS_GAINED:  // حدث اكتساب التركيز / Focus gained event
                    m_isFocused = true;             // تحديث الحالة / Update state
                    if (m_onFocus) {                // إذا كان هناك رد نداء / If callback exists
                        m_onFocus();                // استدعاء رد النداء / Call callback
                    }
                    break;
                    
                case SDL_WINDOWEVENT_FOCUS_LOST:    // حدث فقدان التركيز / Focus lost event
                    m_isFocused = false;            // تحديث الحالة / Update state
                    if (m_onBlur) {                 // إذا كان هناك رد نداء / If callback exists
                        m_onBlur();                 // استدعاء رد النداء / Call callback
                    }
                    break;
                    
                case SDL_WINDOWEVENT_MINIMIZED:     // حدث التصغير / Minimized event
                    m_isMinimized = true;           // تحديث الحالة / Update state
                    break;
                    
                case SDL_WINDOWEVENT_MAXIMIZED:     // حدث التكبير / Maximized event
                    m_isMaximized = true;           // تحديث الحالة / Update state
                    break;
                    
                case SDL_WINDOWEVENT_RESTORED:      // حدث الاستعادة / Restored event
                    m_isMinimized = false;          // إلغاء التصغير / Cancel minimized
                    m_isMaximized = false;          // إلغاء التكبير / Cancel maximized
                    break;
                    
                case SDL_WINDOWEVENT_SHOWN:         // حدث الظهور / Shown event
                    m_isVisible = true;             // تحديث الحالة / Update state
                    break;
                    
                case SDL_WINDOWEVENT_HIDDEN:        // حدث الإخفاء / Hidden event
                    m_isVisible = false;            // تحديث الحالة / Update state
                    break;
            }
        }
    }
}

// ----------------------------------------------------------------------------
// تحديث الحالة / Update State
// ----------------------------------------------------------------------------
void Window::UpdateState() {
    if (!m_nativeWindow) return;    // إذا لم تكن النافذة موجودة، الخروج / If no window, exit
    
    // الحصول على الحالة الحالية من SDL / Get current state from SDL
    Uint32 flags = SDL_GetWindowFlags(m_nativeWindow);
    
    // تحديث الحالات / Update states
    m_isMinimized = (flags & SDL_WINDOW_MINIMIZED) != 0;    // فحص التصغير / Check minimized
    m_isMaximized = (flags & SDL_WINDOW_MAXIMIZED) != 0;    // فحص التكبير / Check maximized
    m_isFocused = (flags & SDL_WINDOW_INPUT_FOCUS) != 0;    // فحص التركيز / Check focus
}

// ----------------------------------------------------------------------------
// توليد معرف فريد / Generate Unique Handle
// ----------------------------------------------------------------------------
Handle Window::GenerateHandle() {
    return s_handleCounter.fetch_add(1); // زيادة العداد بشكل ذري / Atomically increment counter
}

// ============================================================================
// تطبيق WindowManager / WindowManager Implementation
// ============================================================================

// ----------------------------------------------------------------------------
// الحصول على النسخة الوحيدة / Get Singleton Instance
// ----------------------------------------------------------------------------
WindowManager& WindowManager::GetInstance() {
    static WindowManager instance;  // نسخة ساكنة وحيدة / Static singleton instance
    return instance;                // إرجاع النسخة / Return instance
}

// ----------------------------------------------------------------------------
// منشئ خاص / Private Constructor
// ----------------------------------------------------------------------------
WindowManager::WindowManager()
    : m_initialized(false)          // غير مهيأ مبدئياً / Not initialized initially
{
}

// ----------------------------------------------------------------------------
// مدمر خاص / Private Destructor
// ----------------------------------------------------------------------------
WindowManager::~WindowManager() {
    Shutdown();                     // إنهاء النظام / Shutdown system
}

// ----------------------------------------------------------------------------
// التهيئة / Initialize
// ----------------------------------------------------------------------------
Result WindowManager::Initialize() {
    if (m_initialized) {            // إذا كان مهيأاً بالفعل / If already initialized
        return Result::Success;      // إرجاع النجاح / Return success
    }
    
    // تهيئة نظام الفيديو في SDL / Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return Result::Failure;      // فشل التهيئة / Initialization failed
    }
    
    m_initialized = true;            // تحديث الحالة / Update state
    return Result::Success;          // إرجاع النجاح / Return success
}

// ----------------------------------------------------------------------------
// الإنهاء / Shutdown
// ----------------------------------------------------------------------------
void WindowManager::Shutdown() {
    if (!m_initialized) return;     // إذا لم يكن مهيأاً، الخروج / If not initialized, exit
    
    m_windows.clear();              // مسح جميع النوافذ / Clear all windows
    SDL_Quit();                     // إنهاء SDL / Quit SDL
    m_initialized = false;           // تحديث الحالة / Update state
}

// ----------------------------------------------------------------------------
// إنشاء نافذة / Create Window
// ----------------------------------------------------------------------------
Window* WindowManager::CreateWindow(const String& title, Int32 width, Int32 height, WindowFlags flags) {
    if (!m_initialized) {           // التحقق من التهيئة / Check initialization
        Initialize();                // التهيئة إذا لزم الأمر / Initialize if needed
    }
    
    // إنشاء نافذة جديدة / Create new window
    auto window = std::make_unique<Window>(title, width, height, flags);
    Handle handle = window->GetHandle(); // الحصول على المعرف / Get handle
    
    Window* windowPtr = window.get();    // حفظ المؤشر / Store pointer
    m_windows[handle] = std::move(window); // نقل الملكية / Transfer ownership
    
    return windowPtr;                    // إرجاع المؤشر / Return pointer
}

// ----------------------------------------------------------------------------
// تدمير نافذة / Destroy Window
// ----------------------------------------------------------------------------
void WindowManager::DestroyWindow(Handle handle) {
    auto it = m_windows.find(handle);   // البحث عن النافذة / Find window
    if (it != m_windows.end()) {        // إذا وُجدت / If found
        m_windows.erase(it);            // إزالة النافذة / Remove window
    }
}

// ----------------------------------------------------------------------------
// الحصول على نافذة / Get Window
// ----------------------------------------------------------------------------
Window* WindowManager::GetWindow(Handle handle) {
    auto it = m_windows.find(handle);   // البحث عن النافذة / Find window
    if (it != m_windows.end()) {        // إذا وُجدت / If found
        return it->second.get();        // إرجاع المؤشر / Return pointer
    }
    return nullptr;                     // لم تُوجد / Not found
}

// ----------------------------------------------------------------------------
// الحصول على عدد النوافذ / Get Window Count
// ----------------------------------------------------------------------------
size_t WindowManager::GetWindowCount() const {
    return m_windows.size();            // إرجاع العدد / Return count
}

// ----------------------------------------------------------------------------
// معالجة أحداث جميع النوافذ / Process All Events
// ----------------------------------------------------------------------------
void WindowManager::ProcessAllEvents() {
    for (auto& pair : m_windows) {      // حلقة على جميع النوافذ / Loop through all windows
        pair.second->ProcessEvents();   // معالجة أحداث النافذة / Process window events
    }
}

} // namespace graphics
} // namespace sad
