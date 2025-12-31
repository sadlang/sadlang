// ============================================================================
// window.h - نظام النوافذ الأساسي
// Core Window System
// ============================================================================
// الوصف: واجهة نظام النوافذ الموحدة عبر المنصات
// Description: Unified cross-platform window system interface
// ============================================================================


#ifndef SAD_GRAPHICS_WINDOW_H
#define SAD_GRAPHICS_WINDOW_H

#include "core/types.h"
#include <vector>       // للقوائم الديناميكية / Dynamic arrays
#include <map>          // للخرائط / Maps

// تضمينات SDL2 / SDL2 includes
struct SDL_Window;      // تصريح مسبق لنافذة SDL / Forward declaration for SDL window
union SDL_Event;        // تصريح مسبق لحدث SDL / Forward declaration for SDL event

namespace sad {
namespace graphics {

// ============================================================================
// تعدادات النافذة / Window Enumerations
// ============================================================================

/// أعلام خيارات النافذة / Window option flags
enum class WindowFlags : UInt32 {
    None            = 0,        ///< لا خيارات / No options
    Fullscreen      = 1 << 0,   ///< ملء الشاشة / Fullscreen
    Resizable       = 1 << 1,   ///< قابل لتغيير الحجم / Resizable
    Borderless      = 1 << 2,   ///< بدون إطار / Borderless
    Hidden          = 1 << 3,   ///< مخفي / Hidden
    Minimized       = 1 << 4,   ///< مُصغّر / Minimized
    Maximized       = 1 << 5,   ///< مُكبّر / Maximized
    HighDPI         = 1 << 6,   ///< دعم DPI عالي / High DPI support
    AlwaysOnTop     = 1 << 7    ///< دائماً في المقدمة / Always on top
};

/// عملية ثنائية على الأعلام / Binary operation on flags
inline WindowFlags operator|(WindowFlags a, WindowFlags b) {
    return static_cast<WindowFlags>(static_cast<UInt32>(a) | static_cast<UInt32>(b));
}

/// فحص العلم / Check flag
inline bool HasFlag(WindowFlags flags, WindowFlags flag) {
    return (static_cast<UInt32>(flags) & static_cast<UInt32>(flag)) != 0;
}

/// نوع الحدث / Event type
enum class EventType {
    None,           ///< لا حدث / No event
    WindowClose,    ///< إغلاق النافذة / Window close
    WindowResize,   ///< تغيير حجم النافذة / Window resize
    WindowMove,     ///< تحريك النافذة / Window move
    WindowFocus,    ///< تركيز النافذة / Window focus
    WindowBlur,     ///< فقدان التركيز / Window blur
    WindowMinimize, ///< تصغير النافذة / Window minimize
    WindowMaximize, ///< تكبير النافذة / Window maximize
    WindowRestore,  ///< استعادة النافذة / Window restore
    KeyDown,        ///< ضغط مفتاح / Key down
    KeyUp,          ///< تحرير مفتاح / Key up
    MouseMove,      ///< حركة الماوس / Mouse move
    MouseDown,      ///< ضغط زر الماوس / Mouse button down
    MouseUp,        ///< تحرير زر الماوس / Mouse button up
    MouseWheel      ///< عجلة الماوس / Mouse wheel
};

// ============================================================================
// هياكل الأحداث / Event Structures
// ============================================================================

/// بيانات حدث تغيير الحجم / Resize event data
struct ResizeEventData {
    Int32 width;    ///< العرض الجديد / New width
    Int32 height;   ///< الارتفاع الجديد / New height
};

/// بيانات حدث التحريك / Move event data
struct MoveEventData {
    Int32 x; ///< الموقع الأفقي الجديد / New X position
    Int32 y; ///< الموقع العمودي الجديد / New Y position
};

/// بيانات الحدث / Event data
struct Event {
    EventType type;             ///< نوع الحدث / Event type
    Handle windowHandle;        ///< معرف النافذة / Window handle
    
    // بيانات إضافية حسب نوع الحدث / Additional data based on event type
    union {
        ResizeEventData resize; ///< بيانات تغيير الحجم / Resize data
        MoveEventData move;     ///< بيانات التحريك / Move data
    } data;
};

// ============================================================================
// صنف النافذة / Window Class
// ============================================================================

/// صنف النافذة - يمثل نافذة رسومية واحدة
/// Window class - represents a single graphical window
class Window {
public:
    // ========================================================================
    // المنشئ والمدمر / Constructor and Destructor
    // ========================================================================
    
    /// منشئ النافذة / Window constructor
    /// @param title عنوان النافذة / Window title
    /// @param width عرض النافذة / Window width
    /// @param height ارتفاع النافذة / Window height
    /// @param flags أعلام خيارات النافذة / Window option flags
    Window(const String& title, Int32 width, Int32 height, 
           WindowFlags flags = WindowFlags::None);
    
    /// المدمر - ينظف موارد النافذة / Destructor - cleans up window resources
    ~Window();
    
    // منع النسخ / Prevent copying
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    
    // السماح بالنقل / Allow moving
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;
    
    // ========================================================================
    // الدوال العامة / Public Methods
    // ========================================================================
    
    /// يعرض النافذة / Shows the window
    void Show();
    
    /// يخفي النافذة / Hides the window
    void Hide();
    
    /// يُغلق النافذة / Closes the window
    void Close();
    
    /// يرفع النافذة للأمام / Raises the window to front
    void Raise();
    
    /// يركز على النافذة / Focuses the window
    void Focus();
    
    /// يصغر النافذة / Minimizes the window
    void Minimize();
    
    /// يكبر النافذة / Maximizes the window
    void Maximize();
    
    /// يستعيد حجم النافذة الطبيعي / Restores normal window size
    void Restore();
    
    /// يضبط عنوان النافذة / Sets the window title
    /// @param title العنوان الجديد / New title
    void SetTitle(const String& title);
    
    /// يضبط حجم النافذة / Sets the window size
    /// @param width العرض الجديد / New width
    /// @param height الارتفاع الجديد / New height
    void SetSize(Int32 width, Int32 height);
    
    /// يضبط موقع النافذة / Sets the window position
    /// @param x الموقع الأفقي / X position
    /// @param y الموقع العمودي / Y position
    void SetPosition(Int32 x, Int32 y);
    
    /// يضبط وضع ملء الشاشة / Sets fullscreen mode
    /// @param fullscreen true لتفعيل ملء الشاشة / true to enable fullscreen
    void SetFullscreen(bool fullscreen);
    
    /// يضبط قابلية تغيير الحجم / Sets resizability
    /// @param resizable true لجعل النافذة قابلة لتغيير الحجم / true to make resizable
    void SetResizable(bool resizable);
    
    /// يضبط الشفافية / Sets opacity
    /// @param opacity الشفافية (0.0 - 1.0) / Opacity (0.0 - 1.0)
    void SetOpacity(Float32 opacity);
    
    // ========================================================================
    // دوال الاستعلام / Query Methods
    // ========================================================================
    
    /// يُرجع عنوان النافذة / Returns the window title
    String GetTitle() const;
    
    /// يُرجع عرض النافذة / Returns the window width
    Int32 GetWidth() const;
    
    /// يُرجع ارتفاع النافذة / Returns the window height
    Int32 GetHeight() const;
    
    /// يُرجع حجم النافذة / Returns the window size
    Size2D GetSize() const;
    
    /// يُرجع موقع النافذة / Returns the window position
    Point2D GetPosition() const;
    
    /// يتحقق إذا كانت النافذة مرئية / Checks if window is visible
    bool IsVisible() const;
    
    /// يتحقق إذا كانت النافذة مركزة / Checks if window is focused
    bool IsFocused() const;
    
    /// يتحقق إذا كانت النافذة مصغرة / Checks if window is minimized
    bool IsMinimized() const;
    
    /// يتحقق إذا كانت النافذة مكبرة / Checks if window is maximized
    bool IsMaximized() const;
    
    /// يتحقق إذا كانت النافذة في وضع ملء الشاشة / Checks if window is fullscreen
    bool IsFullscreen() const;
    
    /// يتحقق إذا كانت النافذة مفتوحة / Checks if window is open
    bool IsOpen() const;
    
    /// يُرجع معرف النافذة / Returns the window handle
    Handle GetHandle() const { return m_handle; }
    
    // ========================================================================
    // معالجة الأحداث / Event Handling
    // ========================================================================
    
    /// يسجل دالة رد نداء لحدث الإغلاق / Registers close event callback
    void OnClose(CallbackFunction callback);
    
    /// يسجل دالة رد نداء لحدث تغيير الحجم / Registers resize event callback
    void OnResize(CallbackWithParam<Size2D> callback);
    
    /// يسجل دالة رد نداء لحدث التحريك / Registers move event callback
    void OnMove(CallbackWithParam<Point2D> callback);
    
    /// يسجل دالة رد نداء لحدث التركيز / Registers focus event callback
    void OnFocus(CallbackFunction callback);
    
    /// يسجل دالة رد نداء لحدث فقدان التركيز / Registers blur event callback
    void OnBlur(CallbackFunction callback);
    
    /// يعالج الأحداث المعلقة / Processes pending events
    void ProcessEvents();
    
    // ========================================================================
    // الوصول الداخلي / Internal Access
    // ========================================================================
    
    /// يُرجع مؤشر نافذة SDL الأساسية / Returns underlying SDL window pointer
    /// @warning للاستخدام الداخلي فقط / For internal use only
    SDL_Window* GetNativeWindow() const { return m_nativeWindow; }

private:
    // ========================================================================
    // الأعضاء الخاصة / Private Members
    // ========================================================================
    
    Handle m_handle;                    ///< معرف فريد للنافذة / Unique window identifier
    SDL_Window* m_nativeWindow;         ///< مؤشر نافذة SDL الأساسية / Underlying SDL window pointer
    
    String m_title;                     ///< عنوان النافذة / Window title
    Int32 m_width;                      ///< عرض النافذة / Window width
    Int32 m_height;                     ///< ارتفاع النافذة / Window height
    Int32 m_posX;                       ///< الموقع الأفقي / X position
    Int32 m_posY;                       ///< الموقع العمودي / Y position
    
    bool m_isOpen;                      ///< هل النافذة مفتوحة / Is window open
    bool m_isVisible;                   ///< هل النافذة مرئية / Is window visible
    bool m_isFocused;                   ///< هل النافذة مركزة / Is window focused
    bool m_isMinimized;                 ///< هل النافذة مصغرة / Is window minimized
    bool m_isMaximized;                 ///< هل النافذة مكبرة / Is window maximized
    bool m_isFullscreen;                ///< هل النافذة في ملء الشاشة / Is window fullscreen
    
    // دوال رد النداء / Callback functions
    CallbackFunction m_onClose;                     ///< رد نداء الإغلاق / Close callback
    CallbackWithParam<Size2D> m_onResize;           ///< رد نداء تغيير الحجم / Resize callback
    CallbackWithParam<Point2D> m_onMove;            ///< رد نداء التحريك / Move callback
    CallbackFunction m_onFocus;                     ///< رد نداء التركيز / Focus callback
    CallbackFunction m_onBlur;                      ///< رد نداء فقدان التركيز / Blur callback
    
    // ========================================================================
    // الدوال الخاصة / Private Methods
    // ========================================================================
    
    /// ينشئ نافذة SDL / Creates SDL window
    bool CreateNativeWindow(WindowFlags flags);
    
    /// يدمر نافذة SDL / Destroys SDL window
    void DestroyNativeWindow();
    
    /// يعالج حدث SDL / Processes SDL event
    void HandleEvent(const SDL_Event& event);
    
    /// يحدث حالة النافذة / Updates window state
    void UpdateState();
    
    /// يولد معرف فريد / Generates unique handle
    static Handle GenerateHandle();
};

// ============================================================================
// مدير النوافذ / Window Manager
// ============================================================================

/// مدير النوافذ - يدير جميع النوافذ النشطة (Singleton)
/// Window Manager - manages all active windows (Singleton)
class WindowManager {
public:
    /// يُرجع النسخة الوحيدة من المدير / Returns singleton instance
    static WindowManager& GetInstance();
    
    /// ينشئ نافذة جديدة / Creates a new window
    Window* CreateWindow(const String& title, Int32 width, Int32 height,
                        WindowFlags flags = WindowFlags::None);
    
    /// يدمر نافذة / Destroys a window
    void DestroyWindow(Handle handle);
    
    /// يُرجع نافذة بالمعرف / Returns window by handle
    Window* GetWindow(Handle handle);
    
    /// يُرجع عدد النوافذ المفتوحة / Returns number of open windows
    size_t GetWindowCount() const;
    
    /// يعالج أحداث جميع النوافذ / Processes events for all windows
    void ProcessAllEvents();
    
    /// يُهيئ نظام النوافذ / Initializes window system
    Result Initialize();
    
    /// ينهي نظام النوافذ / Shuts down window system
    void Shutdown();
    
private:
    /// منشئ خاص (Singleton) / Private constructor (Singleton)
    WindowManager();
    
    /// مدمر خاص / Private destructor
    ~WindowManager();
    
    // منع النسخ / Prevent copying
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    
    std::map<Handle, std::unique_ptr<Window>> m_windows; ///< خريطة النوافذ / Windows map
    bool m_initialized;                                   ///< هل تم التهيئة / Is initialized
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_WINDOW_H
