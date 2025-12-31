// ==============================================================================
// input_manager.cpp - تطبيق مدير الإدخال / Input Manager Implementation
// ==============================================================================
// الوصف: تطبيق إدارة الإدخال المركزية
// Description: Implementation of central input management
// ==============================================================================

#include "../../include/input/input_manager.h"  // ملف الرأس / Header file
#include <SDL.h>                                // مكتبة SDL2 / SDL2 library

namespace SadGraphics {                         // مساحة الأسماء / Namespace

// ==============================================================================
// الإنشاء والتدمير / Construction and Destruction
// ==============================================================================

/// المنشئ / Constructor
InputManager::InputManager()
    : m_mousePosition{0.0f, 0.0f}               // تهيئة موقع الفأرة / Initialize mouse position
    , m_mouseDelta{0.0f, 0.0f}                  // تهيئة حركة الفأرة / Initialize mouse delta
    , m_mouseWheelDelta{0.0f, 0.0f}             // تهيئة حركة العجلة / Initialize wheel delta
    , m_lastMousePosition{0.0f, 0.0f}           // تهيئة الموقع السابق / Initialize last position
    , m_textInputEnabled(false)                 // إدخال النص معطل افتراضياً / Text input disabled by default
{
    // لا حاجة لمزيد من التهيئة / No additional initialization needed
}

/// المدمر / Destructor
InputManager::~InputManager() {
    // تنظيف الموارد / Clean up resources
}

// ==============================================================================
// معالجة الأحداث / Event Processing
// ==============================================================================

/// معالجة حدث SDL / Process SDL event
void InputManager::ProcessEvent(const SDL_Event& event) {
    // تحديد نوع الحدث / Determine event type
    switch (event.type) {
        // أحداث لوحة المفاتيح / Keyboard events
        case SDL_KEYDOWN:                       // ضغط مفتاح / Key pressed
        case SDL_KEYUP:                         // تحرير مفتاح / Key released
            ProcessKeyboardEvent(event);        // معالجة الحدث / Process event
            break;
        
        // أحداث الفأرة / Mouse events
        case SDL_MOUSEMOTION:                   // حركة الفأرة / Mouse motion
        case SDL_MOUSEBUTTONDOWN:               // ضغط زر / Button pressed
        case SDL_MOUSEBUTTONUP:                 // تحرير زر / Button released
        case SDL_MOUSEWHEEL:                    // حركة العجلة / Wheel motion
            ProcessMouseEvent(event);           // معالجة الحدث / Process event
            break;
        
        // إدخال نص / Text input
        case SDL_TEXTINPUT:                     // إدخال نص / Text input
            ProcessTextInputEvent(event);       // معالجة الحدث / Process event
            break;
    }
}

/// تحديث حالة الإدخال / Update input state
void InputManager::Update() {
    // مسح الأحداث الفورية / Clear instant events
    m_keysPressed.clear();                      // مسح المفاتيح المضغوطة / Clear pressed keys
    m_keysReleased.clear();                     // مسح المفاتيح المحررة / Clear released keys
    m_mouseButtonsPressed.clear();              // مسح أزرار الفأرة المضغوطة / Clear pressed buttons
    m_mouseButtonsReleased.clear();             // مسح أزرار الفأرة المحررة / Clear released buttons
    
    // تحديث حالة المفاتيح من Pressed إلى Held / Update key states from Pressed to Held
    for (auto& pair : m_keyStates) {            // المرور على جميع المفاتيح / Iterate all keys
        if (pair.second == KeyState::Pressed) { // إذا كان مضغوطاً حديثاً / If just pressed
            pair.second = KeyState::Held;       // تحويله إلى مستمر / Convert to held
        }
    }
    
    // حساب حركة الفأرة / Calculate mouse delta
    m_mouseDelta.x = m_mousePosition.x - m_lastMousePosition.x;  // التغيير في X / X change
    m_mouseDelta.y = m_mousePosition.y - m_lastMousePosition.y;  // التغيير في Y / Y change
    m_lastMousePosition = m_mousePosition;      // تحديث الموقع السابق / Update last position
    
    // إعادة تعيين حركة العجلة / Reset wheel delta
    m_mouseWheelDelta = {0.0f, 0.0f};          // تصفير الحركة / Zero movement
}

/// إعادة تعيين حالة الإدخال / Reset input state
void InputManager::Reset() {
    // مسح جميع الحالات / Clear all states
    m_keyStates.clear();                        // مسح حالات المفاتيح / Clear key states
    m_keysPressed.clear();                      // مسح المفاتيح المضغوطة / Clear pressed keys
    m_keysReleased.clear();                     // مسح المفاتيح المحررة / Clear released keys
    
    m_mouseButtonStates.clear();                // مسح حالات الأزرار / Clear button states
    m_mouseButtonsPressed.clear();              // مسح الأزرار المضغوطة / Clear pressed buttons
    m_mouseButtonsReleased.clear();             // مسح الأزرار المحررة / Clear released buttons
    
    m_mouseDelta = {0.0f, 0.0f};               // إعادة تعيين الحركة / Reset delta
    m_mouseWheelDelta = {0.0f, 0.0f};          // إعادة تعيين العجلة / Reset wheel
}

// ==============================================================================
// لوحة المفاتيح / Keyboard
// ==============================================================================

/// التحقق من ضغط المفتاح / Check if key pressed
bool InputManager::IsKeyPressed(KeyCode key) const {
    return m_keysPressed.find(key) != m_keysPressed.end();  // البحث في المجموعة / Search in set
}

/// التحقق من استمرار ضغط المفتاح / Check if key held
bool InputManager::IsKeyHeld(KeyCode key) const {
    auto it = m_keyStates.find(key);            // البحث عن المفتاح / Search for key
    if (it != m_keyStates.end()) {              // إذا وُجد / If found
        return it->second == KeyState::Held || it->second == KeyState::Pressed;  // التحقق من الحالة / Check state
    }
    return false;                               // غير موجود / Not found
}

/// التحقق من تحرير المفتاح / Check if key released
bool InputManager::IsKeyReleased(KeyCode key) const {
    return m_keysReleased.find(key) != m_keysReleased.end();  // البحث في المجموعة / Search in set
}

/// الحصول على حالة المفتاح / Get key state
KeyState InputManager::GetKeyState(KeyCode key) const {
    auto it = m_keyStates.find(key);            // البحث عن المفتاح / Search for key
    if (it != m_keyStates.end()) {              // إذا وُجد / If found
        return it->second;                      // إرجاع الحالة / Return state
    }
    return KeyState::Released;                  // افتراضياً غير مضغوط / Default released
}

/// التحقق من ضغط Ctrl / Check if Ctrl down
bool InputManager::IsCtrlDown() const {
    return IsKeyHeld(KeyCode::LeftCtrl) || IsKeyHeld(KeyCode::RightCtrl);  // التحقق من أي منهما / Check either
}

/// التحقق من ضغط Shift / Check if Shift down
bool InputManager::IsShiftDown() const {
    return IsKeyHeld(KeyCode::LeftShift) || IsKeyHeld(KeyCode::RightShift);  // التحقق من أي منهما / Check either
}

/// التحقق من ضغط Alt / Check if Alt down
bool InputManager::IsAltDown() const {
    return IsKeyHeld(KeyCode::LeftAlt) || IsKeyHeld(KeyCode::RightAlt);  // التحقق من أي منهما / Check either
}

// ==============================================================================
// الفأرة / Mouse
// ==============================================================================

/// التحقق من ضغط زر الفأرة / Check if mouse button pressed
bool InputManager::IsMouseButtonPressed(MouseButton button) const {
    return m_mouseButtonsPressed.find(button) != m_mouseButtonsPressed.end();  // البحث / Search
}

/// التحقق من استمرار ضغط زر الفأرة / Check if mouse button held
bool InputManager::IsMouseButtonHeld(MouseButton button) const {
    auto it = m_mouseButtonStates.find(button); // البحث عن الزر / Search for button
    if (it != m_mouseButtonStates.end()) {      // إذا وُجد / If found
        return it->second;                      // إرجاع الحالة / Return state
    }
    return false;                               // غير موجود / Not found
}

/// التحقق من تحرير زر الفأرة / Check if mouse button released
bool InputManager::IsMouseButtonReleased(MouseButton button) const {
    return m_mouseButtonsReleased.find(button) != m_mouseButtonsReleased.end();  // البحث / Search
}

// ==============================================================================
// إعدادات / Settings
// ==============================================================================

/// تفعيل/تعطيل إدخال النص / Enable/disable text input
void InputManager::SetTextInputEnabled(bool enable) {
    m_textInputEnabled = enable;                // حفظ الحالة / Store state
    
    if (enable) {                               // إذا كان التفعيل / If enabling
        SDL_StartTextInput();                   // بدء إدخال النص / Start text input
    } else {                                    // إذا كان التعطيل / If disabling
        SDL_StopTextInput();                    // إيقاف إدخال النص / Stop text input
    }
}

// ==============================================================================
// الدوال الخاصة / Private Functions
// ==============================================================================

/// معالجة حدث لوحة المفاتيح / Process keyboard event
void InputManager::ProcessKeyboardEvent(const SDL_Event& event) {
    // تحويل SDL scancode إلى KeyCode / Convert SDL scancode to KeyCode
    KeyCode key = SDLScancodeToKeyCode(event.key.keysym.scancode);
    
    if (key == KeyCode::Unknown) return;        // تجاهل المفاتيح غير المعروفة / Ignore unknown keys
    
    if (event.type == SDL_KEYDOWN) {            // ضغط مفتاح / Key pressed
        // التحقق من عدم التكرار / Check not repeat
        if (event.key.repeat == 0) {            // ليس تكرار / Not repeat
            m_keyStates[key] = KeyState::Pressed;  // ضبط الحالة / Set state
            m_keysPressed.insert(key);          // إضافة للمضغوطة / Add to pressed
            
            // استدعاء callback إن وُجد / Call callback if exists
            if (m_keyCallback) {                // إذا كان هناك callback / If callback exists
                m_keyCallback(key, KeyState::Pressed);  // استدعاؤه / Call it
            }
        }
    } else if (event.type == SDL_KEYUP) {       // تحرير مفتاح / Key released
        m_keyStates[key] = KeyState::Released;  // ضبط الحالة / Set state
        m_keysReleased.insert(key);             // إضافة للمحررة / Add to released
        
        // استدعاء callback إن وُجد / Call callback if exists
        if (m_keyCallback) {                    // إذا كان هناك callback / If callback exists
            m_keyCallback(key, KeyState::Released);  // استدعاؤه / Call it
        }
    }
}

/// معالجة حدث الفأرة / Process mouse event
void InputManager::ProcessMouseEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {        // حركة الفأرة / Mouse motion
        // تحديث الموقع / Update position
        m_mousePosition.x = static_cast<Float32>(event.motion.x);  // الموقع X / X position
        m_mousePosition.y = static_cast<Float32>(event.motion.y);  // الموقع Y / Y position
        
        // استدعاء callback إن وُجد / Call callback if exists
        if (m_mouseMoveCallback) {              // إذا كان هناك callback / If callback exists
            m_mouseMoveCallback(m_mousePosition.x, m_mousePosition.y);  // استدعاؤه / Call it
        }
    } 
    else if (event.type == SDL_MOUSEBUTTONDOWN) {  // ضغط زر / Button pressed
        // تحويل SDL button إلى MouseButton / Convert SDL button to MouseButton
        MouseButton button = static_cast<MouseButton>(event.button.button);
        
        m_mouseButtonStates[button] = true;     // ضبط الحالة / Set state
        m_mouseButtonsPressed.insert(button);   // إضافة للمضغوطة / Add to pressed
        
        // استدعاء callback إن وُجد / Call callback if exists
        if (m_mouseButtonCallback) {            // إذا كان هناك callback / If callback exists
            m_mouseButtonCallback(button, true);  // استدعاؤه / Call it
        }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {    // تحرير زر / Button released
        // تحويل SDL button إلى MouseButton / Convert SDL button to MouseButton
        MouseButton button = static_cast<MouseButton>(event.button.button);
        
        m_mouseButtonStates[button] = false;    // ضبط الحالة / Set state
        m_mouseButtonsReleased.insert(button);  // إضافة للمحررة / Add to released
        
        // استدعاء callback إن وُجد / Call callback if exists
        if (m_mouseButtonCallback) {            // إذا كان هناك callback / If callback exists
            m_mouseButtonCallback(button, false);  // استدعاؤه / Call it
        }
    }
    else if (event.type == SDL_MOUSEWHEEL) {       // حركة العجلة / Wheel motion
        // تحديث حركة العجلة / Update wheel delta
        m_mouseWheelDelta.x = static_cast<Float32>(event.wheel.x);  // الحركة الأفقية / Horizontal
        m_mouseWheelDelta.y = static_cast<Float32>(event.wheel.y);  // الحركة العمودية / Vertical
        
        // استدعاء callback إن وُجد / Call callback if exists
        if (m_mouseWheelCallback) {             // إذا كان هناك callback / If callback exists
            m_mouseWheelCallback(m_mouseWheelDelta.x, m_mouseWheelDelta.y);  // استدعاؤه / Call it
        }
    }
}

/// معالجة حدث إدخال النص / Process text input event
void InputManager::ProcessTextInputEvent(const SDL_Event& event) {
    // التحقق من تفعيل إدخال النص / Check if text input enabled
    if (!m_textInputEnabled) return;            // تجاهل إذا كان معطلاً / Ignore if disabled
    
    // الحصول على النص المدخل / Get input text
    std::string text = event.text.text;         // نص SDL / SDL text
    
    // استدعاء callback إن وُجد / Call callback if exists
    if (m_textInputCallback) {                  // إذا كان هناك callback / If callback exists
        m_textInputCallback(text);              // استدعاؤه / Call it
    }
}

/// تحويل SDL scancode إلى KeyCode / Convert SDL scancode to KeyCode
KeyCode InputManager::SDLScancodeToKeyCode(Int32 scancode) const {
    // تحويل مباشر حيث أن KeyCode يستخدم نفس قيم SDL / Direct conversion as KeyCode uses SDL values
    // هذا آمن لأن KeyCode enum مصمم ليطابق SDL scancodes / Safe as KeyCode enum matches SDL scancodes
    
    // الحروف / Letters (A-Z: 4-29)
    if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z) {
        return static_cast<KeyCode>(scancode);  // تحويل مباشر / Direct conversion
    }
    
    // الأرقام / Numbers (1-0: 30-39)
    if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_0) {
        return static_cast<KeyCode>(scancode);  // تحويل مباشر / Direct conversion
    }
    
    // مفاتيح خاصة معروفة / Known special keys
    switch (scancode) {
        case SDL_SCANCODE_RETURN: return KeyCode::Return;      // مفتاح الإدخال / Enter
        case SDL_SCANCODE_ESCAPE: return KeyCode::Escape;      // مفتاح الخروج / Escape
        case SDL_SCANCODE_BACKSPACE: return KeyCode::Backspace;  // مفتاح المسح / Backspace
        case SDL_SCANCODE_TAB: return KeyCode::Tab;            // مفتاح الجدولة / Tab
        case SDL_SCANCODE_SPACE: return KeyCode::Space;        // مفتاح المسافة / Space
        
        // الأسهم / Arrows
        case SDL_SCANCODE_RIGHT: return KeyCode::ArrowRight;   // سهم يمين / Right arrow
        case SDL_SCANCODE_LEFT: return KeyCode::ArrowLeft;     // سهم يسار / Left arrow
        case SDL_SCANCODE_DOWN: return KeyCode::ArrowDown;     // سهم أسفل / Down arrow
        case SDL_SCANCODE_UP: return KeyCode::ArrowUp;         // سهم أعلى / Up arrow
        
        // مفاتيح التحكم / Control keys
        case SDL_SCANCODE_LCTRL: return KeyCode::LeftCtrl;     // Ctrl أيسر / Left Ctrl
        case SDL_SCANCODE_LSHIFT: return KeyCode::LeftShift;   // Shift أيسر / Left Shift
        case SDL_SCANCODE_LALT: return KeyCode::LeftAlt;       // Alt أيسر / Left Alt
        case SDL_SCANCODE_RCTRL: return KeyCode::RightCtrl;    // Ctrl أيمن / Right Ctrl
        case SDL_SCANCODE_RSHIFT: return KeyCode::RightShift;  // Shift أيمن / Right Shift
        case SDL_SCANCODE_RALT: return KeyCode::RightAlt;      // Alt أيمن / Right Alt
        
        // مفاتيح F / F keys (F1-F12: 58-69)
        case SDL_SCANCODE_F1: return KeyCode::F1;
        case SDL_SCANCODE_F2: return KeyCode::F2;
        case SDL_SCANCODE_F3: return KeyCode::F3;
        case SDL_SCANCODE_F4: return KeyCode::F4;
        case SDL_SCANCODE_F5: return KeyCode::F5;
        case SDL_SCANCODE_F6: return KeyCode::F6;
        case SDL_SCANCODE_F7: return KeyCode::F7;
        case SDL_SCANCODE_F8: return KeyCode::F8;
        case SDL_SCANCODE_F9: return KeyCode::F9;
        case SDL_SCANCODE_F10: return KeyCode::F10;
        case SDL_SCANCODE_F11: return KeyCode::F11;
        case SDL_SCANCODE_F12: return KeyCode::F12;
        
        default: return KeyCode::Unknown;      // مفتاح غير معروف / Unknown key
    }
}

} // namespace SadGraphics
