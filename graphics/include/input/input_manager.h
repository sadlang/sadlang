// ==============================================================================
// input_manager.h - مدير الإدخال / Input Manager
// ==============================================================================
// الوصف: إدارة مركزية لجميع أنواع الإدخال (لوحة المفاتيح، الفأرة)
// Description: Central management for all input types (keyboard, mouse)
// ==============================================================================

#ifndef SAD_GRAPHICS_INPUT_INPUT_MANAGER_H
#define SAD_GRAPHICS_INPUT_INPUT_MANAGER_H

#include "../core/types.h"      // الأنواع الأساسية / Basic types
#include <unordered_map>         // للخرائط / For maps
#include <unordered_set>         // للمجموعات / For sets
#include <functional>            // للدوال / For functions

// التعريفات الأمامية / Forward declarations
union SDL_Event;                 // حدث SDL / SDL event

namespace SadGraphics {          // مساحة الأسماء / Namespace

// ==============================================================================
// رموز المفاتيح / Key Codes
// ==============================================================================
enum class KeyCode {
    // الحروف / Letters
    A = 4, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    
    // الأرقام / Numbers
    Num1 = 30, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Num0,
    
    // مفاتيح خاصة / Special keys
    Return = 40,                 // مفتاح الإدخال / Enter key
    Escape = 41,                 // مفتاح الخروج / Escape key
    Backspace = 42,              // مفتاح المسح / Backspace key
    Tab = 43,                    // مفتاح الجدولة / Tab key
    Space = 44,                  // مفتاح المسافة / Space key
    
    // مفاتيح الأسهم / Arrow keys
    ArrowRight = 79,             // سهم يمين / Right arrow
    ArrowLeft = 80,              // سهم يسار / Left arrow
    ArrowDown = 81,              // سهم أسفل / Down arrow
    ArrowUp = 82,                // سهم أعلى / Up arrow
    
    // مفاتيح التحكم / Control keys
    LeftCtrl = 224,              // Ctrl أيسر / Left Ctrl
    LeftShift = 225,             // Shift أيسر / Left Shift
    LeftAlt = 226,               // Alt أيسر / Left Alt
    RightCtrl = 228,             // Ctrl أيمن / Right Ctrl
    RightShift = 229,            // Shift أيمن / Right Shift
    RightAlt = 230,              // Alt أيمن / Right Alt
    
    // مفاتيح F / F keys
    F1 = 58, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    
    Unknown = 0                  // مفتاح غير معروف / Unknown key
};

// ==============================================================================
// أزرار الفأرة / Mouse Buttons
// ==============================================================================
enum class MouseButton {
    Left = 1,                    // الزر الأيسر / Left button
    Middle = 2,                  // الزر الأوسط / Middle button
    Right = 3,                   // الزر الأيمن / Right button
    X1 = 4,                      // زر إضافي 1 / Extra button 1
    X2 = 5                       // زر إضافي 2 / Extra button 2
};

// ==============================================================================
// حالة المفتاح / Key State
// ==============================================================================
enum class KeyState {
    Released,                    // غير مضغوط / Not pressed
    Pressed,                     // مضغوط الآن / Just pressed
    Held                         // مضغوط مستمر / Held down
};

// ==============================================================================
// أنواع callbacks للإدخال / Input Callback Types
// ==============================================================================
using KeyCallback = std::function<void(KeyCode key, KeyState state)>;
using MouseButtonCallback = std::function<void(MouseButton button, bool pressed)>;
using MouseMoveCallback = std::function<void(Float32 x, Float32 y)>;
using MouseWheelCallback = std::function<void(Float32 deltaX, Float32 deltaY)>;
using TextInputCallback = std::function<void(const std::string& text)>;

// ==============================================================================
// فئة مدير الإدخال / Input Manager Class
// ==============================================================================
class InputManager {
public:
    // ==============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ==============================================================================
    
    /// إنشاء مدير إدخال / Create input manager
    InputManager();
    
    /// المدمر / Destructor
    ~InputManager();
    
    // منع النسخ / Prevent copying
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;
    
    // ==============================================================================
    // معالجة الأحداث / Event Processing
    // ==============================================================================
    
    /// معالجة حدث SDL / Process SDL event
    /// event: حدث SDL / SDL event
    void ProcessEvent(const SDL_Event& event);
    
    /// تحديث حالة الإدخال / Update input state
    /// يُستدعى كل إطار / Called every frame
    void Update();
    
    /// إعادة تعيين حالة الإدخال / Reset input state
    void Reset();
    
    // ==============================================================================
    // لوحة المفاتيح / Keyboard
    // ==============================================================================
    
    /// التحقق من ضغط المفتاح (لحظة الضغط فقط) / Check if key just pressed
    /// key: رمز المفتاح / Key code
    /// Returns: true إذا ضُغط للتو / true if just pressed
    bool IsKeyPressed(KeyCode key) const;
    
    /// التحقق من استمرار ضغط المفتاح / Check if key held down
    /// key: رمز المفتاح / Key code
    /// Returns: true إذا كان مضغوطاً / true if held
    bool IsKeyHeld(KeyCode key) const;
    
    /// التحقق من تحرير المفتاح / Check if key released
    /// key: رمز المفتاح / Key code
    /// Returns: true إذا حُرر / true if released
    bool IsKeyReleased(KeyCode key) const;
    
    /// الحصول على حالة المفتاح / Get key state
    /// key: رمز المفتاح / Key code
    /// Returns: حالة المفتاح / Key state
    KeyState GetKeyState(KeyCode key) const;
    
    /// التحقق من ضغط Ctrl / Check if Ctrl pressed
    /// Returns: true إذا كان Ctrl مضغوطاً / true if Ctrl held
    bool IsCtrlDown() const;
    
    /// التحقق من ضغط Shift / Check if Shift pressed
    /// Returns: true إذا كان Shift مضغوطاً / true if Shift held
    bool IsShiftDown() const;
    
    /// التحقق من ضغط Alt / Check if Alt pressed
    /// Returns: true إذا كان Alt مضغوطاً / true if Alt held
    bool IsAltDown() const;
    
    // ==============================================================================
    // الفأرة / Mouse
    // ==============================================================================
    
    /// التحقق من ضغط زر الفأرة / Check if mouse button pressed
    /// button: رمز الزر / Button code
    /// Returns: true إذا ضُغط للتو / true if just pressed
    bool IsMouseButtonPressed(MouseButton button) const;
    
    /// التحقق من استمرار ضغط زر الفأرة / Check if mouse button held
    /// button: رمز الزر / Button code
    /// Returns: true إذا كان مضغوطاً / true if held
    bool IsMouseButtonHeld(MouseButton button) const;
    
    /// التحقق من تحرير زر الفأرة / Check if mouse button released
    /// button: رمز الزر / Button code
    /// Returns: true إذا حُرر / true if released
    bool IsMouseButtonReleased(MouseButton button) const;
    
    /// الحصول على موقع الفأرة / Get mouse position
    /// Returns: موقع الفأرة (x, y) / Mouse position
    Point2D GetMousePosition() const { return m_mousePosition; }
    
    /// الحصول على موقع الفأرة X / Get mouse X position
    /// Returns: الموقع الأفقي / Horizontal position
    Float32 GetMouseX() const { return m_mousePosition.x; }
    
    /// الحصول على موقع الفأرة Y / Get mouse Y position
    /// Returns: الموقع العمودي / Vertical position
    Float32 GetMouseY() const { return m_mousePosition.y; }
    
    /// الحصول على حركة الفأرة / Get mouse delta
    /// Returns: التغيير في الموقع (dx, dy) / Position change
    Point2D GetMouseDelta() const { return m_mouseDelta; }
    
    /// الحصول على حركة عجلة الفأرة / Get mouse wheel delta
    /// Returns: التغيير في العجلة (dx, dy) / Wheel change
    Point2D GetMouseWheelDelta() const { return m_mouseWheelDelta; }
    
    /// التحقق من حركة الفأرة / Check if mouse moved
    /// Returns: true إذا تحركت / true if moved
    bool IsMouseMoved() const { return m_mouseDelta.x != 0 || m_mouseDelta.y != 0; }
    
    // ==============================================================================
    // Callbacks
    // ==============================================================================
    
    /// تسجيل callback لضغط المفاتيح / Register key callback
    /// callback: الدالة المستدعاة / Callback function
    void SetKeyCallback(KeyCallback callback) { m_keyCallback = callback; }
    
    /// تسجيل callback لأزرار الفأرة / Register mouse button callback
    /// callback: الدالة المستدعاة / Callback function
    void SetMouseButtonCallback(MouseButtonCallback callback) { m_mouseButtonCallback = callback; }
    
    /// تسجيل callback لحركة الفأرة / Register mouse move callback
    /// callback: الدالة المستدعاة / Callback function
    void SetMouseMoveCallback(MouseMoveCallback callback) { m_mouseMoveCallback = callback; }
    
    /// تسجيل callback لعجلة الفأرة / Register mouse wheel callback
    /// callback: الدالة المستدعاة / Callback function
    void SetMouseWheelCallback(MouseWheelCallback callback) { m_mouseWheelCallback = callback; }
    
    /// تسجيل callback لإدخال النص / Register text input callback
    /// callback: الدالة المستدعاة / Callback function
    void SetTextInputCallback(TextInputCallback callback) { m_textInputCallback = callback; }
    
    // ==============================================================================
    // إعدادات / Settings
    // ==============================================================================
    
    /// تفعيل/تعطيل إدخال النص / Enable/disable text input
    /// enable: تفعيل أو تعطيل / Enable or disable
    void SetTextInputEnabled(bool enable);
    
    /// التحقق من تفعيل إدخال النص / Check if text input enabled
    /// Returns: true إذا كان مفعلاً / true if enabled
    bool IsTextInputEnabled() const { return m_textInputEnabled; }

private:
    // ==============================================================================
    // البيانات الخاصة / Private Data
    // ==============================================================================
    
    // حالة المفاتيح / Key states
    std::unordered_map<KeyCode, KeyState> m_keyStates;       // حالة كل مفتاح / State of each key
    std::unordered_set<KeyCode> m_keysPressed;               // مفاتيح ضُغطت هذا الإطار / Keys pressed this frame
    std::unordered_set<KeyCode> m_keysReleased;              // مفاتيح حُررت هذا الإطار / Keys released this frame
    
    // حالة أزرار الفأرة / Mouse button states
    std::unordered_map<MouseButton, bool> m_mouseButtonStates;      // حالة كل زر / State of each button
    std::unordered_set<MouseButton> m_mouseButtonsPressed;          // أزرار ضُغطت / Buttons pressed
    std::unordered_set<MouseButton> m_mouseButtonsReleased;         // أزرار حُررت / Buttons released
    
    // موقع الفأرة / Mouse position
    Point2D m_mousePosition;     // الموقع الحالي / Current position
    Point2D m_mouseDelta;        // التغيير منذ الإطار السابق / Change since last frame
    Point2D m_mouseWheelDelta;   // حركة العجلة / Wheel movement
    Point2D m_lastMousePosition; // الموقع السابق / Previous position
    
    // Callbacks
    KeyCallback m_keyCallback;                   // callback المفاتيح / Key callback
    MouseButtonCallback m_mouseButtonCallback;   // callback أزرار الفأرة / Mouse button callback
    MouseMoveCallback m_mouseMoveCallback;       // callback حركة الفأرة / Mouse move callback
    MouseWheelCallback m_mouseWheelCallback;     // callback عجلة الفأرة / Mouse wheel callback
    TextInputCallback m_textInputCallback;       // callback إدخال النص / Text input callback
    
    // إعدادات / Settings
    bool m_textInputEnabled;     // هل إدخال النص مفعل / Is text input enabled
    
    // ==============================================================================
    // الدوال الخاصة / Private Functions
    // ==============================================================================
    
    /// معالجة حدث لوحة المفاتيح / Process keyboard event
    /// event: حدث SDL / SDL event
    void ProcessKeyboardEvent(const SDL_Event& event);
    
    /// معالجة حدث الفأرة / Process mouse event
    /// event: حدث SDL / SDL event
    void ProcessMouseEvent(const SDL_Event& event);
    
    /// معالجة حدث إدخال النص / Process text input event
    /// event: حدث SDL / SDL event
    void ProcessTextInputEvent(const SDL_Event& event);
    
    /// تحويل SDL scancode إلى KeyCode / Convert SDL scancode to KeyCode
    /// scancode: رمز SDL / SDL scancode
    /// Returns: رمز المفتاح / Key code
    KeyCode SDLScancodeToKeyCode(Int32 scancode) const;
};

} // namespace SadGraphics

#endif // SAD_GRAPHICS_INPUT_INPUT_MANAGER_H
