/**
 * =============================================================================
 * ملف: event_system.h
 * الوصف: نظام أحداث متقدم — onClick, onHover, onKeyPress, onDrag
 * (AR) @brief نظام أحداث موحد مع توزيع ذكي
 * (EN) @brief Unified event system with smart dispatch
 * المهمة: T308 - كامل: Event system
 * =============================================================================
 */

#ifndef GRAPHICS_UI_EVENT_SYSTEM_H
#define GRAPHICS_UI_EVENT_SYSTEM_H

#include "widget.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <queue>

namespace sad {
namespace graphics {
namespace ui {

// ============================================================================
// أنواع الأحداث / Event Types
// ============================================================================

enum class EventType {
    // أحداث الفأرة
    MouseDown, MouseUp, MouseMove, MouseWheel,
    Click, DoubleClick, RightClick,
    // أحداث السحب
    DragStart, DragMove, DragEnd, Drop,
    // أحداث التحويم  
    HoverEnter, HoverLeave,
    // أحداث المفاتيح
    KeyDown, KeyUp, KeyPress, TextInput,
    // أحداث التركيز
    FocusIn, FocusOut,
    // أحداث مخصصة
    Custom
};

// ============================================================================
// بيانات الحدث / Event Data
// ============================================================================

struct MouseData {
    int x = 0, y = 0;           ///< موقع الفأرة
    int button = 0;              ///< زر الفأرة (0=يسار, 1=وسط, 2=يمين)
    int wheelDelta = 0;          ///< اتجاه العجلة
    bool shift = false;          ///< مفتاح Shift
    bool ctrl = false;           ///< مفتاح Ctrl
    bool alt = false;            ///< مفتاح Alt
};

struct KeyData {
    int keyCode = 0;             ///< كود المفتاح
    int scanCode = 0;            ///< كود المسح
    bool shift = false;
    bool ctrl = false;
    bool alt = false;
    std::string text;            ///< النص المدخل (للأحرف)
};

struct DragData {
    int startX = 0, startY = 0;  ///< نقطة البداية
    int currentX = 0, currentY = 0; ///< الموقع الحالي
    Widget* source = nullptr;     ///< المصدر
    Widget* target = nullptr;     ///< الهدف
};

// ============================================================================
// الحدث الموحد / Unified Event
// ============================================================================

struct Event {
    EventType type;
    Widget* target = nullptr;     ///< العنصر الهدف
    bool handled = false;         ///< هل عولج الحدث؟
    bool propagate = true;        ///< هل ينتشر للأب؟
    
    MouseData mouse;
    KeyData key;
    DragData drag;
    
    std::string customType;       ///< نوع مخصص (لأحداث Custom)
    
    void stopPropagation() { propagate = false; }
    void preventDefault() { handled = true; }
};

// ============================================================================
// مستمع الأحداث / Event Listener
// ============================================================================

using EventHandler = std::function<void(Event&)>;

struct EventListener {
    EventType type;
    EventHandler handler;
    int priority = 0;             ///< الأولوية (أعلى = أولاً)
    bool once = false;            ///< مرة واحدة فقط
    int id = 0;                   ///< معرف فريد للإلغاء
};

// ============================================================================
// مدير الأحداث / Event Manager
// ============================================================================

class EventManager {
public:
    static EventManager& getInstance();

    /**
     * (AR) تسجيل مستمع على عنصر
     * (EN) Register listener on widget
     * @return معرف المستمع (للإلغاء)
     */
    int addEventListener(Widget* widget, EventType type, EventHandler handler,
                         int priority = 0);
    
    /**
     * (AR) تسجيل مستمع يعمل مرة واحدة
     */
    int addOnceListener(Widget* widget, EventType type, EventHandler handler);
    
    /**
     * (AR) إلغاء مستمع
     */
    void removeEventListener(int listenerId);
    
    /**
     * (AR) إلغاء جميع مستمعي عنصر
     */
    void removeAllListeners(Widget* widget);
    
    /**
     * (AR) إطلاق حدث على عنصر (مع انتشار)
     */
    void dispatchEvent(Event& event);
    
    /**
     * (AR) إطلاق حدث مخصص
     */
    void emitCustomEvent(Widget* target, const std::string& eventName);

    // ========================================================================
    // معالجة إدخال عالية المستوى / High-level input processing
    // ========================================================================
    
    /**
     * (AR) معالجة حركة الفأرة — يولّد HoverEnter/HoverLeave
     */
    void processMouseMove(Widget* root, int x, int y, bool shift = false,
                          bool ctrl = false, bool alt = false);
    
    /**
     * (AR) معالجة نقر الفأرة — يولّد Click/DoubleClick
     */
    void processMouseDown(Widget* root, int x, int y, int button);
    void processMouseUp(Widget* root, int x, int y, int button);
    
    /**
     * (AR) معالجة السحب — يولّد DragStart/DragMove/DragEnd
     */
    void processDrag(int x, int y);
    
    /**
     * (AR) معالجة المفاتيح
     */
    void processKeyDown(Widget* focused, int keyCode, bool shift, bool ctrl, bool alt);
    void processKeyUp(Widget* focused, int keyCode);

private:
    EventManager() = default;
    
    int nextId_ = 1;
    
    // Widget → listeners
    std::unordered_map<Widget*, std::vector<EventListener>> listeners_;
    
    // Hover tracking
    Widget* lastHovered_ = nullptr;
    
    // Drag tracking
    bool dragging_ = false;
    DragData dragState_;
    
    // Click timing for double-click detection
    float lastClickTime_ = 0;
    int lastClickX_ = 0, lastClickY_ = 0;
    
    // Helper: find widget at point
    Widget* hitTest(Widget* root, int x, int y);
    
    // Helper: dispatch with bubble
    void bubbleEvent(Event& event);
    
    // Helper: fire listeners for a widget
    void fireListeners(Widget* widget, Event& event);
};

// ============================================================================
// واجهة مختصرة للأحداث / Shorthand Event API
// ============================================================================

/**
 * (AR) دوال مختصرة لتسجيل الأحداث — onClick(widget, handler)
 * (EN) Shorthand functions — onClick(widget, handler)
 */
inline int onClick(Widget* w, std::function<void(int x, int y)> h) {
    return EventManager::getInstance().addEventListener(w, EventType::Click,
        [h](Event& e) { h(e.mouse.x, e.mouse.y); });
}

inline int onDoubleClick(Widget* w, std::function<void(int x, int y)> h) {
    return EventManager::getInstance().addEventListener(w, EventType::DoubleClick,
        [h](Event& e) { h(e.mouse.x, e.mouse.y); });
}

inline int onHover(Widget* w, std::function<void()> enter, std::function<void()> leave) {
    auto& mgr = EventManager::getInstance();
    mgr.addEventListener(w, EventType::HoverEnter, [enter](Event&) { enter(); });
    return mgr.addEventListener(w, EventType::HoverLeave, [leave](Event&) { leave(); });
}

inline int onKeyPress(Widget* w, std::function<void(int keyCode)> h) {
    return EventManager::getInstance().addEventListener(w, EventType::KeyDown,
        [h](Event& e) { h(e.key.keyCode); });
}

inline int onDrag(Widget* w, std::function<void(int dx, int dy)> h) {
    return EventManager::getInstance().addEventListener(w, EventType::DragMove,
        [h](Event& e) {
            h(e.drag.currentX - e.drag.startX, e.drag.currentY - e.drag.startY);
        });
}

inline int onTextInput(Widget* w, std::function<void(const std::string&)> h) {
    return EventManager::getInstance().addEventListener(w, EventType::TextInput,
        [h](Event& e) { h(e.key.text); });
}

} // namespace ui
} // namespace graphics
} // namespace sad

#endif // GRAPHICS_UI_EVENT_SYSTEM_H
